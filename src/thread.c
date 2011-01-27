#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <pthread.h>
#include <errno.h>
#ifndef DISABLE_GC
#include <gc/gc.h>
#endif

#include        "system.h"
#include        "gasman.h"
#include        "objects.h"
#include	"scanner.h"
#include	"code.h"
#include	"plist.h"
#include        "tls.h"
#include        "thread.h"

#define LOG2_NUM_LOCKS 11
#define NUM_LOCKS (1 << LOG2_NUM_LOCKS)

typedef struct {
  pthread_t pthread_id;
  int joined;
  void *tls;
  void (*start)(void *);
  void *arg;
  int next;
} ThreadData;

static ThreadData thread_data[MAX_THREADS];
static int thread_free_list;

static pthread_mutex_t master_lock;

static pthread_rwlock_t ObjLock[NUM_LOCKS];

int PreThreadCreation = 1;

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#ifndef HAVE_NATIVE_TLS

void *AllocateTLS()
{
  void *addr;
  void *result;
  size_t pagesize = getpagesize();
  size_t tlssize = (sizeof(ThreadLocalStorage)+pagesize-1) & ~ (pagesize-1);
  addr = mmap(0, 2 * TLS_SIZE, PROT_READ|PROT_WRITE,
    MAP_PRIVATE|MAP_ANONYMOUS, -1 , 0);
  result = (void *)((((uintptr_t) addr) + (TLS_SIZE-1)) & TLS_MASK);
  munmap(addr, (char *)result-(char *)addr);
  munmap((char *)result+TLS_SIZE, (char *)addr-(char *)result+TLS_SIZE);
  /* generate a stack overflow protection area */
#ifdef STACK_GROWS_UP
  mprotect((char *) result + TLS_SIZE - tlssize - pagesize, pagesize, PROT_NONE);
#else
  mprotect((char *) result + tlssize, pagesize, PROT_NONE);
#endif
  return result;
}

void FreeTLS(void *address)
{
  /* We currently cannot free this memory because of garbage collector
   * issues. Instead, it will be reused */
#if 0
  munmap(address, TLS_STACK_SIZE);
#endif
}

#endif /* HAVE_NATIVE_TLS */

#ifndef DISABLE_GC
void AddGCRoots()
{
  void *p = TLS;
  GC_add_roots(p, (char *)p + sizeof(ThreadLocalStorage));
}

void RemoveGCRoots()
{
  void *p = TLS;
  GC_remove_roots(p, (char *)p + sizeof(ThreadLocalStorage));
}
#endif /* DISABLE_GC */

#ifdef __GNUC__
static void SetupTLS() __attribute__((noinline));
static void GrowStack() __attribute__((noinline));
#endif

#ifndef HAVE_NATIVE_TLS
static void GrowStack()
{
  char *tls = (char *) TLS;
  size_t pagesize = getpagesize();
  char *p = alloca(pagesize);
  while (p > tls)
  {
    *p = '\0'; /* touch memory */
    p = alloca(pagesize);
  }
}
#endif

static void SetupTLS()
{
#ifndef HAVE_NATIVE_TLS
  GrowStack();
#endif
  InitializeTLS();
  MainThreadTLS = TLS;
  TLS->threadID = -1;
}

void RunThreadedMain(
  int (*mainFunction)(int, char **, char **),
  int argc,
  char **argv,
  char **environ )
{
  int i;
#ifdef STACK_GROWS_UP
#error Upward growing stack not yet supported
#else
#ifndef HAVE_NATIVE_TLS
  int dummy[0];
  alloca(((uintptr_t) dummy) &~TLS_MASK);
#endif
  SetupTLS();
#endif
  for (i=0; i<MAX_THREADS-1; i++)
    thread_data[i].next = i+1;
  for (i=0; i<NUM_LOCKS; i++)
    pthread_rwlock_init(&ObjLock[i], 0);
  thread_data[MAX_THREADS-1].next = -1;
  for (i=0; i<MAX_THREADS; i++)
    thread_data[i].tls = 0;
  thread_free_list = 0;
  pthread_mutex_init(&master_lock, 0);
  exit((*mainFunction)(argc, argv, environ));
}

void CreateMainDataSpace()
{
  TLS->currentDataSpace = NewDataSpace();
  ((DataSpace *)TLS->currentDataSpace)->is_thread_local = 1;
  DataSpaceWriteLock(TLS->currentDataSpace);
}

void *DispatchThread(void *arg)
{
  ThreadData *this_thread = arg;
  InitializeTLS();
  TLS->threadID = this_thread - thread_data;
#ifndef DISABLE_GC
  AddGCRoots();
#endif
  InitTLS();
  TLS->currentDataSpace = NewDataSpace();
  ((DataSpace *)TLS->currentDataSpace)->is_thread_local = 1;
  DataSpaceWriteLock(TLS->currentDataSpace);
  this_thread->start(this_thread->arg);
  DataSpaceWriteUnlock(TLS->currentDataSpace);
  DestroyTLS();
#ifndef DISABLE_GC
  RemoveGCRoots();
#endif
  return 0;
}

int RunThread(void (*start)(void *), void *arg)
{
  int result;
#ifndef HAVE_NATIVE_TLS
  void *tls;
#endif
  pthread_attr_t thread_attr;
  size_t pagesize = getpagesize();
  pthread_mutex_lock(&master_lock);
  PreThreadCreation = 0;
  /* allocate a new thread id */
  if (thread_free_list < 0)
  {
    pthread_mutex_unlock(&master_lock);
    errno = ENOMEM;
    return -1;
  }
  result = thread_free_list;
  thread_free_list = thread_data[thread_free_list].next;
#ifndef HAVE_NATIVE_TLS
  tls = thread_data[result].tls = AllocateTLS();
#endif
  thread_data[result].arg = arg;
  thread_data[result].start = start;
  thread_data[result].joined = 0;
  /* set up the thread attribute to support a custom stack in our TLS */
  pthread_attr_init(&thread_attr);
#ifndef HAVE_NATIVE_TLS
  pthread_attr_setstack(&thread_attr, (char *)tls + pagesize * 2,
      TLS_SIZE-pagesize*2);
#endif
  pthread_mutex_unlock(&master_lock);
  /* fork the thread */
  if (pthread_create(&thread_data[result].pthread_id, &thread_attr,
                     DispatchThread, thread_data+result) < 0) {
    pthread_mutex_lock(&master_lock);
    thread_data[result].next = thread_free_list;
    thread_free_list = result;
    pthread_mutex_unlock(&master_lock);
    pthread_attr_destroy(&thread_attr);
#ifndef HAVE_NATIVE_TLS
    FreeTLS(tls);
  #endif
    return -1;
  }
  pthread_attr_destroy(&thread_attr);
  return result;
}

int JoinThread(int id)
{
  pthread_t pthread_id;
  void (*start)(void *);
#ifndef HAVE_NATIVE_TLS
  void *tls;
#endif
  if (id < 0 || id >= MAX_THREADS)
    return 0;
  pthread_mutex_lock(&master_lock);
  pthread_id = thread_data[id].pthread_id;
  start = thread_data[id].start;
#ifndef HAVE_NATIVE_TLS
  tls = thread_data[id].tls;
#endif
  if (thread_data[id].joined || start == NULL)
  {
    pthread_mutex_unlock(&master_lock);
    return 0;
  }
  thread_data[id].joined = 1;
  pthread_mutex_unlock(&master_lock);
  pthread_join(pthread_id, NULL);
  pthread_mutex_lock(&master_lock);
  thread_data[id].next = thread_free_list;
  thread_free_list = id;
  thread_data[id].tls = NULL;
  thread_data[id].start = NULL;
  pthread_mutex_unlock(&master_lock);
#ifndef HAVE_NATIVE_TLS
  FreeTLS(tls);
#endif
  return 1;
}

unsigned LockID(void *object) {
  unsigned p = (unsigned) object;
  if (sizeof(void *) == 4)
    return ((p >> 2)
      ^ (p >> (2 + LOG2_NUM_LOCKS))
      ^ (p << (LOG2_NUM_LOCKS - 2))) % NUM_LOCKS;
  else
    return ((p >> 3)
      ^ (p >> (3 + LOG2_NUM_LOCKS))
      ^ (p << (LOG2_NUM_LOCKS - 3))) % NUM_LOCKS;
}

void Lock(void *object) {
  pthread_rwlock_wrlock(&ObjLock[LockID(object)]);
}

void LockShared(void *object) {
  pthread_rwlock_rdlock(&ObjLock[LockID(object)]);
}

void Unlock(void *object) {
  pthread_rwlock_unlock(&ObjLock[LockID(object)]);
}

void UnlockShared(void *object) {
  pthread_rwlock_unlock(&ObjLock[LockID(object)]);
}

void DataSpaceWriteLock(DataSpace *dataspace)
{
  pthread_rwlock_wrlock(dataspace->lock);
  dataspace->owner = TLS;
}

void DataSpaceWriteUnlock(DataSpace *dataspace)
{
  dataspace->owner = NULL;
  pthread_rwlock_unlock(dataspace->lock);
}

void DataSpaceReadLock(DataSpace *dataspace)
{
  pthread_rwlock_rdlock(dataspace->lock);
  dataspace->readers[TLS->threadID+1] = 1;
}

void DataSpaceReadUnlock(DataSpace *dataspace)
{
  dataspace->readers[TLS->threadID+1] = 0;
  pthread_rwlock_unlock(dataspace->lock);
}

void DataSpaceUnlock(DataSpace *dataspace)
{
  dataspace->owner = NULL;
  dataspace->readers[TLS->threadID+1] = 0;
  pthread_rwlock_unlock(dataspace->lock);
}

int IsLocked(DataSpace *dataspace)
{
  if (!dataspace)
    return 2; /* public dataspace */
  if (dataspace->owner == TLS)
    return 1;
  if (dataspace->readers[TLS->threadID+1])
    return 2;
  return 0;
}

void GetLockStatus(int count, Obj *objects, int *status)
{
  int i;
  for (i=0; i<count; i++)
    status[i] = IsLocked(DS_BAG(objects[i]));
}

#define MAX_LOCKS 1024

typedef struct
{
  Obj obj;
  DataSpace *dataspace;
  int mode;
} LockRequest;

int CompareByDSRef(const void *a, const void *b)
{
  DataSpace *ds_a = ((LockRequest *)a)->dataspace;
  DataSpace *ds_b = ((LockRequest *)b)->dataspace;
  if (ds_a == ds_b) /* prioritize writes */
    return ((LockRequest *)b)->mode-((LockRequest *)a)->mode;
  return (char *)ds_a - (char *)ds_b;
}

int LockObjects(int count, Obj *objects, int *mode, DataSpace **locked)
{
  int result = 0;
  int i;
  LockRequest *order;
  if (count > MAX_LOCKS)
    return 0;
  order = alloca(sizeof(LockRequest)*count);
  for (i=0; i<count; i++)
  {
    order[i].obj = objects[i];
    order[i].dataspace = DS_BAG(objects[i]);
    order[i].mode = mode[i];
  }
  heapsort(order, count, sizeof(LockRequest), CompareByDSRef);
  for (i=0; i<count; i++)
  {
    DataSpace *ds = order[i].dataspace;
    /* If there are multiple lock requests with different modes,
     * they have been sorted for writes to occur first, so deadlock
     * cannot occur from doing readlocks before writelocks.
     */
    if (i > 0 && ds == order[i-1].dataspace)
      continue; /* skip duplicates */
    else if (IsLocked(ds) || ds->is_thread_local)
    {
      /* DataSpaces may not be locked twice. If that is attempted,
       * the entire lock operation is reverted. Similarly if one
       * attempts to lock another thread's data space.
       */
      while (--i >= 0)
        DataSpaceUnlock(order[i].dataspace);
      return 0;
    }
    if (order[i].mode)
      DataSpaceWriteLock(ds);
    else
      DataSpaceReadLock(ds);
    if (locked)
      locked[result] = ds;
    result++;
    if (DS_BAG(order[i].obj) != ds)
    {
      /* Race condition, revert locks and fail */
      while (i >= 0)
      {
        DataSpaceUnlock(order[i].dataspace);
	i--;
      }
      return 0;
    }
  }
  return result;
}

void UnlockObjects(int count, Obj *objects)
{
  int i;
  for (i=0; i<count; i++)
  {
    DataSpace *dataspace = DS_BAG(objects[i]);
    if (dataspace && IsLocked(dataspace))
      DataSpaceUnlock(dataspace);
  }
}

void UnlockDataSpaces(int count, DataSpace **dataspaces)
{
  while (count--)
    DataSpaceUnlock(*dataspaces++);
}

DataSpace *CurrentDataSpace()
{
  return TLS->currentDataSpace;
}

static Obj NewList(int size)
{
  Obj list;
  list = NEW_PLIST(size == 0 ? T_PLIST_EMPTY : T_PLIST, size);
  SET_LEN_PLIST(list, size);
  return list;
}

TraversalFunction TraversalFunc[LAST_REAL_TNUM+1];

static void InitTraversal()
{
  TLS->travHash = NewList(16);
  TLS->travHashSize = 0;
  TLS->travHashCapacity = 16;
  TLS->travHashBits = 4;
  TLS->travList = NewList(10);
  TLS->travListSize = 0;
  TLS->travListCapacity = 10;
  TLS->travListCurrent = 0;
}

#if SIZEOF_VOID_P == 4
#define TRAV_HASH_MULT 0x9e3779b9UL
#else
#define TRAV_HASH_MULT 0x9e3779b97f4a7c13UL
#endif
#define TRAV_HASH_BITS (SIZEOF_VOID_P * 8)

static void TraversalRehash();

static int SeenDuringTraversal(Obj obj)
{
  Int type;
  Obj *hashTable = ADDR_OBJ(TLS->travHash)+1;
  unsigned long hash;
  if (!IS_BAG_REF(obj))
    return;
  hash = ((unsigned long) obj) * TRAV_HASH_MULT;
  hash >>= TRAV_HASH_BITS - TLS->travHashBits;
  if (TLS->travHashSize * 3 / 2 >= TLS->travHashCapacity)
    TraversalRehash();
  for (;;)
  {
    if (hashTable[hash] == NULL)
    {
      hashTable[hash] = obj;
      return 1;
    }
    if (hashTable[hash] == obj)
      return 0;
    hash = (hash + 1) & (TLS->travHashSize-1);
  }
}

static void TraversalRehash()
{
  Obj list = NewList(TLS->travHashCapacity * 2);
  int oldsize = TLS->travHashCapacity;
  int i;
  Obj oldlist = TLS->travHash;
  TLS->travHashCapacity *= 2;
  TLS->travHash = list;
  TLS->travHashSize = 0;
  TLS->travHashBits++;
  for (i = 1; i <= oldsize; i++)
  {
    Obj obj = ADDR_OBJ(oldlist)[i];
    if (obj != NULL)
      SeenDuringTraversal(obj);
  }
}

void QueueForTraversal(Obj obj)
{
  int i;
  if (!IS_BAG_REF(obj))
    return; /* skip ojects that aren't bags */
  if (DS_BAG(obj) != TLS->travDataSpace)
    return; /* stop traversal at the border of a data space */
  if (!SeenDuringTraversal(obj))
    return; /* don't revisit objects that we've already seen */
  if (TLS->travListSize == TLS->travListCapacity)
  {
    unsigned oldcapacity = TLS->travListCapacity;
    unsigned newcapacity = oldcapacity * 14/10;
    Obj oldlist = TLS->travList;
    Obj list = NewList(newcapacity);
    for (i=1; i<=oldcapacity; i++)
      ADDR_OBJ(list)[i] = ADDR_OBJ(oldlist)[i];
    TLS->travList = list;
    TLS->travListCapacity = newcapacity;
  }
  ADDR_OBJ(TLS->travList)[++TLS->travListSize] = obj;
}

Obj TraverseDataSpaceFrom(Obj obj)
{
  Obj result;
  if (!IS_BAG_REF(obj))
    return NewList(0);
  if (!CheckRead(obj))
    return NewList(0);
  TLS->travDataSpace = DS_BAG(obj);
  InitTraversal();
  QueueForTraversal(obj);
  while (TLS->travListCurrent < TLS->travListSize)
  {
    Obj current = ADDR_OBJ(TLS->travList)[++TLS->travListCurrent];
    TraversalFunction tfunc = TraversalFunc[TNUM_BAG(current)];
    if (tfunc)
      tfunc(current);
  }
  result = TLS->travList;
  TLS->travList = NULL;
  TLS->travHash = NULL;
  return result;
}
