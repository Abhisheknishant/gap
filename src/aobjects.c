/****************************************************************************
**
*W  threadapi.c                 GAP source                    Reimer Behrends
**
*Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
*Y  (C) 1998 School Math and Comp. Sci., University of St.  Andrews, Scotland
*Y  Copyright (C) 2002 The GAP Group
**
**  This file contains the GAP interface for thread primitives.
*/
#include        <stdio.h>
#include        <assert.h>
#include        <setjmp.h>              /* jmp_buf, setjmp, longjmp        */
#include        <string.h>              /* memcpy */
#include        <stdlib.h>
#include	<pthread.h>
#include	<atomic_ops.h>

#include        "system.h"              /* system dependent part           */

#include        "gasman.h"              /* garbage collector               */
#include        "objects.h"             /* objects                         */
#include        "scanner.h"             /* scanner                         */

#include        "gap.h"                 /* error handling, initialisation  */

#include        "read.h"                /* reader                          */
#include        "gvars.h"               /* global variables                */

#include        "calls.h"               /* generic call mechanism          */
#include        "opers.h"               /* generic operations              */
#include        "ariths.h"              /* basic arithmetic                */

#include        "integer.h"             /* integers                        */
#include        "bool.h"                /* booleans                        */

#include        "records.h"             /* generic records                 */
#include        "precord.h"             /* plain records                   */

#include        "lists.h"               /* generic lists                   */
#include        "listoper.h"            /* operations for generic lists    */
#include        "listfunc.h"            /* functions for generic lists     */
#include        "plist.h"               /* plain lists                     */

#include        "code.h"                /* coder                           */

#include        "vars.h"                /* variables                       */
#include        "exprs.h"               /* expressions                     */
#include        "stats.h"               /* statements                      */
#include        "funcs.h"               /* functions                       */

#include	"fibhash.h"

#include	"string.h"

#include        "thread.h"
#include        "tls.h"


#include        "intrprtr.h"            /* interpreter                     */

#include        "compiler.h"            /* compiler                        */

Obj TYPE_ALIST;
Obj TYPE_AREC;

#ifndef WARD_ENABLED

Obj TypeAList(Obj obj)
{
  Obj result = ADDR_OBJ(obj)[1];
  return result != NULL ? result : TYPE_ALIST;
}

Obj TypeARecord(Obj obj)
{
  Obj result = ADDR_OBJ(obj)[0];
  return result != NULL ? result : TYPE_AREC;
}

static Int AlwaysMutable( Obj obj)
{
  return 1;
}

static void ArgumentError(char *message)
{
  ErrorQuit(message, 0, 0);
}

static Obj NewAtomicList(UInt length)
{
  return NewBag(T_ALIST, sizeof(Obj) * (length + 2));
}

static Obj FuncNewAtomicList(Obj self, Obj args)
{
  Obj init;
  Obj result;
  Obj *data;
  UInt i, len;
  switch (LEN_PLIST(args)) {
    case 1:
      init = ELM_PLIST(args, 1);
      if (!IS_DENSE_LIST(init))
        ArgumentError("NewAtomicList: Argument must be dense list");
      len = LEN_LIST(init);
      result = NewAtomicList(len);
      data = ADDR_OBJ(result);
      *data++ = (Obj) len;
      *data++ = NULL;
      for (i=1; i<= len; i++)
        *data++ = ELM_LIST(init, i);
      AO_nop_write(); /* Should not be necessary, but better be safe. */
      return result;
    case 2:
      if (!IS_INTOBJ(ELM_PLIST(args, 1)))
        ArgumentError("NewAtomicList: First argument must be a non-negative integer");
      len = INT_INTOBJ(ELM_PLIST(args, 1));
      if (len < 0)
        ArgumentError("NewAtomicList: First argument must be a non-negative integer");
      result = NewAtomicList(len);
      init = ELM_PLIST(args, 2);
      data = ADDR_OBJ(result);
      *data++ = (Obj) len;
      *data++ = NULL;
      for (i=1; i<=len; i++)
        *data++ = init;
      AO_nop_write(); /* Should not be necessary, but better be safe. */
      return result;
    default:
      ArgumentError("NewAtomicList: Too many arguments");
  }
}

static Obj FuncGET_ATOMIC_LIST(Obj self, Obj list, Obj index)
{
  UInt n;
  UInt len;
  Obj result;
  if (TNUM_OBJ(list) != T_ALIST)
    ArgumentError("GET_ATOMIC_LIST: First argument must be an atomic list");
  len = (UInt) ADDR_OBJ(list)[0];
  if (!IS_INTOBJ(index))
    ArgumentError("GET_ATOMIC_LIST: Second argument must be an integer");
  n = INT_INTOBJ(index);
  if (n <= 0 || n > len)
    ArgumentError("GET_ATOMIC_LIST: Index out of range");
  AO_nop_read(); /* read barrier */
  return ADDR_OBJ(list)[n+1];
}

static Obj FuncSET_ATOMIC_LIST(Obj self, Obj list, Obj index, Obj value)
{
  UInt n;
  UInt len;
  if (TNUM_OBJ(list) != T_ALIST)
    ArgumentError("SET_ATOMIC_LIST: First argument must be an atomic list");
  len = (UInt) ADDR_OBJ(list)[0];
  if (!IS_INTOBJ(index))
    ArgumentError("SET_ATOMIC_LIST: Second argument must be an integer");
  n = INT_INTOBJ(index);
  if (n <= 0 || n > len)
    ArgumentError("SET_ATOMIC_LIST: Index out of range");
  ADDR_OBJ(list)[n+1] = value;
  AO_nop_write(); /* write barrier */
  return (Obj) 0;
}

static Obj FuncFromAtomicList(Obj self, Obj list)
{
  Obj result;
  Obj *data;
  UInt i, len;
  if (TNUM_OBJ(list) != T_ALIST)
    ArgumentError("FromAtomicList: First argument must be an atomic list");
  data = ADDR_OBJ(list);
  len = (UInt) *data++;
  result = NEW_PLIST(T_PLIST, len);
  SET_LEN_PLIST(result, len);
  AO_nop_read();
  for (i=1; i<=len; i++)
    SET_ELM_PLIST(result, i, data[i]);
  return result;
}

static void MarkAtomicList(Bag bag)
{
  UInt i, len;
  Bag *ptr, *ptrend;
  ptr = PTR_BAG(bag);
  ptrend = ptr + SIZE_BAG(bag);
  ptr++; /* skip length field */
  while (ptr < ptrend)
    MARK_BAG(*ptr++);
}

static void MarkAtomicRecord(Bag bag)
{
  MARK_BAG(ADDR_OBJ(bag)[0]);
}

typedef union AtomicObj
{
  AO_t atom;
  Obj obj;
} AtomicObj;


static void MarkAtomicRecord2(Bag bag)
{
  AtomicObj *p = (AtomicObj *)(ADDR_OBJ(bag));
  UInt cap = p->atom;
  p += 5;
  while (cap) {
    MARK_BAG(p->obj);
    p += 2;
    cap--;
  }
}

static void PrintAtomicList(Obj obj)
{
  Pr("<atomic list of size %d>", (UInt)(ADDR_OBJ(obj)[0]), 0L);
}

/* T_AREC2 substructure:
 * ADDR_OBJ(rec)[0] == capacity, must be a power of 2.
 * ADDR_OBJ(rec)[1] == log2(capacity).
 * ADDR_OBJ(rec)[2] == estimated size (occupied slots).
 * ADDR_OBJ(rec)[3] == update strategy.
 */

static inline Obj ARecordObj(Obj record)
{
  return ADDR_OBJ(record)[1];
}

static inline AtomicObj* ARecordTable(Obj record)
{
  return (AtomicObj *)(ADDR_OBJ(ARecordObj(record)));
}

static void PrintAtomicRecord(Obj record)
{
  UInt cap, size;
  Lock(record);
  AtomicObj *table = ARecordTable(record);
  cap = table[0].atom;
  size = table[2].atom;
  Unlock(record);
  Pr("<atomic record %d/%d full>", size, cap);
}


static Obj GetARecordField(Obj record, UInt field)
{
  AtomicObj *table = ARecordTable(record);
  AtomicObj *data = table + 4;
  UInt cap, bits, hash, n;
  /* We need a memory barrier to ensure that we see fields that
   * were updated before the table pointer was updated; there is
   * a matching write barrier in the set operation. */
  AO_nop_read();
  cap = table[0].atom;
  bits = table[1].atom;
  hash = FibHash(field, bits);
  n = cap;
  while (n-- > 0)
  {
    UInt key = data[hash*2].atom;
    if (key == field)
    {
      AO_nop_read(); /* memory barrier */
      return data[hash*2+1].obj;
    }
    if (!key)
      return (Obj) 0;
    hash++;
    if (hash == cap)
      hash = 0;
  }
  return (Obj) 0;
}

static UInt ARecordFastInsert(AtomicObj *table, AO_t field)
{
  AtomicObj *data = table + 4;
  UInt cap = table[0].atom;
  UInt bits = table[1].atom;
  UInt hash = FibHash(field, bits);
  table[2].atom++; /* increase size */
  for (;;)
  {
    AO_t key;
    key = data[hash*2].atom;
    if (!key)
    {
      data[hash*2].atom = field;
      return hash*2+1;
    }
    if (key == field)
      return hash*2+1;
    hash++;
    if (hash == cap)
      hash = 0;
  }
}

static Obj SetARecordField(Obj record, UInt field, Obj obj)
{
  AtomicObj *table, *data, *newtable, *newdata;
  Obj newarec, result;
  UInt cap, bits, hash, i, n, size;
  Int strat;
  int have_room;
  LockShared(record);
  table = ARecordTable(record);
  data = table + 4;
  cap = table[0].atom;
  bits = table[1].atom;
  strat = table[3].atom;
  hash = FibHash(field, bits);
  n = cap;
  /* case 1: key exists, we can replace it */
  while (n-- > 0)
  {
    if (data[hash*2].atom == field)
    {
      AO_nop_full(); /* memory barrier */
      if (strat < 0) {
        UnlockShared(record);
	return 0;
      }
      if (strat) {
        AtomicObj old;
	AtomicObj new;
	new.obj = obj;
	do {
	  do {
	    old = data[hash*2+1];
	  } while (!old.obj);
	} while (!AO_compare_and_swap_full(&data[hash*2+1].atom,
	          old.atom, new.atom));
	UnlockShared(record);
	return obj;
      } else {
        Obj result;
	do {
	  result = data[hash*2+1].obj;
	} while (!result);
	UnlockShared(record);
	return result;
      }
    }
    hash++;
    if (hash == cap)
      hash = 0;
  }
  do {
    size = table[2].atom + 1;
    have_room = (size * 3 / 2 < cap);
  } while (have_room && !AO_compare_and_swap_full(&table[2].atom,
                         size-1, size));
  /* we're guaranteed to have a non-full table for the insertion step */
  /* if have_room is true */
  if (have_room) for (;;) { /* hash iteration loop */
    AtomicObj old = data[hash*2];
    if (old.atom == field) {
      /* we don't actually need a new entry, so revert the size update */
      do {
	size = table[2].atom;
      } while (!AO_compare_and_swap_full(&table[2].atom, size, size-1));
      /* continue below */
    } else if (!old.atom) {
      AtomicObj new;
      new.atom = field;
      if (!AO_compare_and_swap_full(&data[hash*2].atom, old.atom, new.atom))
        continue;
      /* else continue below */
    } else {
      hash++;
      if (hash == cap)
        hash = 0;
      continue;
    }
    AO_nop_full(); /* memory barrier */
    for (;;) { /* CAS loop */
      old = data[hash*2+1];
      if (old.obj) {
        if (strat < 0) {
	  result = 0;
	  break;
	}
	if (strat) {
	  AtomicObj new;
	  new.obj = obj;
	  if (AO_compare_and_swap_full(&data[hash*2+1].atom,
	      old.atom, new.atom)) {
	    result = obj;
	    break;
	  }
	} else {
	  result = old.obj;
	  break;
	}
      } else {
        AtomicObj new;
	new.obj = obj;
	if (AO_compare_and_swap_full(&data[hash*2+1].atom,
	    old.atom, new.atom)) {
	  result = obj;
	  break;
	}
      }
    } /* end CAS loop */
    UnlockShared(record);
    return result;
  } /* end hash iteration loop */
  /* have_room is false at this point */
  UnlockShared(record);
  Lock(record);
  newarec = NewBag(T_AREC2, sizeof(AtomicObj) * (4 + cap * 2));
  newtable = (AtomicObj *)(ADDR_OBJ(newarec));
  newdata = newtable + 4;
  newtable[0].atom = cap * 2;
  newtable[1].atom = bits;
  newtable[2].atom = table[2].atom+1; /* size */
  newtable[3] = table[3]; /* strategy */
  for (i=0; i<cap; i++) {
    UInt key = table[2*i].atom;
    if (key) {
      n = ARecordFastInsert(newtable, key);
      newtable[4+n].obj = data[2*i+1].obj;
    }
  }
  n = ARecordFastInsert(newtable, field);
  if (newtable[4+n].obj)
  {
    if (strat < 0)
      result = (Obj) 0;
    else {
      if (strat)
        newtable[4+n].obj = result = obj;
      else
        result = newtable[4+n].obj;
    }
  }
  else
    newtable[4+n].obj = obj;
  AO_nop_write(); /* memory barrier */
  ADDR_OBJ(record)[1] = newarec;
  Unlock(record);
}

static Obj CreateAtomicRecord(UInt capacity)
{
  Obj arec, result;
  AtomicObj *table;
  UInt bits = 1;
  while (capacity > (1 << bits))
    bits++;
  capacity = 1 << bits;
  arec = NewBag(T_AREC2, sizeof(AtomicObj) * (4+capacity));
  table = (AtomicObj *)(ADDR_OBJ(arec));
  result = NewBag(T_AREC, 2*sizeof(Obj));
  table[0].atom = capacity;
  table[1].atom = bits;
  table[2].atom = 0;
  table[3].atom = 1;
  ADDR_OBJ(result)[1] = arec;
  return result;
}

static void SetARecordUpdateStrategy(Obj record, UInt strat)
{
  AtomicObj *table = ARecordTable(record);
  table[3].atom = strat;
}

Obj ElmARecord(Obj record, UInt rnam)
{
   Obj result;
   for (;;) {
     result = GetARecordField(record, rnam);
     if (result)
       return result;
     ErrorReturnVoid("Record: '<atomic record>.%s' must have an assigned value",
       (UInt)NAME_RNAM(rnam), 0L,
       "you can 'return;' after assigning a value" );
    }
}

void AssARecord(Obj record, UInt rnam, Obj value)
{
   SetARecordField(record, rnam, value);
}

Int IsbARecord(Obj record, UInt rnam)
{
  return GetARecordField(record, rnam) != (Obj) 0;
}

static Obj FuncNewAtomicRecord(Obj self, Obj args)
{
  Obj cap;
  switch (LEN_PLIST(args)) {
    case 0:
      return CreateAtomicRecord(8);
    case 1:
      cap = ELM_PLIST(args, 1);
      if (!IS_INTOBJ(cap) || INT_INTOBJ(cap) <= 0)
        ArgumentError("NewAtomicRecord: capacity must be a positive integer");
      return CreateAtomicRecord(INT_INTOBJ(cap));
    default:
      ArgumentError("NewAtomicRecord: takes one optional argument");
      return (Obj) 0;
  }
}

static Obj FuncGET_ATOMIC_RECORD(Obj self, Obj record, Obj field, Obj def)
{
  UInt fieldname;
  Obj result;
  if (TNUM_OBJ(record) != T_AREC)
    ArgumentError("GET_ATOMIC_RECORD: First argument must be an atomic record");
  if (TNUM_OBJ(field) != T_STRING)
    ArgumentError("GET_ATOMIC_RECORD: Second argument must be a string");
  fieldname = RNamName(CSTR_STRING(field));
  result = GetARecordField(record, fieldname);
  return result ? result : def;
}

static Obj FuncSET_ATOMIC_RECORD(Obj self, Obj record, Obj field, Obj value)
{
  UInt fieldname;
  Obj result;
  if (TNUM_OBJ(record) != T_AREC)
    ArgumentError("SET_ATOMIC_RECORD: First argument must be an atomic record");
  if (TNUM_OBJ(field) != T_STRING)
    ArgumentError("SET_ATOMIC_RECORD: Second argument must be a string");
  fieldname = RNamName(CSTR_STRING(field));
  result = SetARecordField(record, fieldname, value);
  if (!result)
    ErrorQuit("SET_ATOMIC_RECORD: Field '%s' already exists",
      (UInt) CSTR_STRING(field), 0L);
  return result;
}

static Obj FuncATOMIC_RECORD_REPLACEMENT(Obj self, Obj record, Obj strat)
{
  if (TNUM_OBJ(record) != T_AREC)
    ArgumentError("ATOMIC_RECORD_REPLACEMENT: First argument must be an atomic record");
  if (strat == Fail)
    SetARecordUpdateStrategy(record, -1);
  else if (strat == False)
    SetARecordUpdateStrategy(record, 0);
  else if (strat == True)
    SetARecordUpdateStrategy(record, 1);
  else
    ArgumentError("ATOMIC_RECORD_REPLACEMENT: Second argument must be true, false, or fail");
  return (Obj) 0;
}

#endif /* WARD_ENABLED */

/****************************************************************************
**
*V  GVarFuncs . . . . . . . . . . . . . . . . . . list of functions to export
*/

static StructGVarFunc GVarFuncs [] = {

    { "NewAtomicList", -1, "list|count, obj",
      FuncNewAtomicList, "src/aobjects.c:NewAtomicList" },

    { "FromAtomicList", 1, "list",
      FuncFromAtomicList, "src/aobjects.c:FromAtomicList" },

    { "GET_ATOMIC_LIST", 2, "list, index",
      FuncGET_ATOMIC_LIST, "src/aobjects.c:GET_ATOMIC_LIST" },

    { "SET_ATOMIC_LIST", 3, "list, index, value",
      FuncSET_ATOMIC_LIST, "src/aobjects.c:SET_ATOMIC_LIST" },

    { "NewAtomicRecord", -1, "[capacity]",
      FuncNewAtomicRecord, "src/aobjects.c:NewAtomicRecord" },

    { "GET_ATOMIC_RECORD", 3, "record, field, default",
      FuncGET_ATOMIC_RECORD, "src/aobjects.c:GET_ATOMIC_RECORD" },

    { "SET_ATOMIC_RECORD", 3, "record, field, value",
      FuncSET_ATOMIC_RECORD, "src/aobjects.c:SET_ATOMIC_RECORD" },

    { "ATOMIC_RECORD_REPLACEMENT", 2, "record, strategy",
      FuncATOMIC_RECORD_REPLACEMENT, "src/aobjects.c:ATOMIC_RECORD_REPLACEMENT" },

    { 0 }

};

static Int IsListAList(Obj list)
{
  return 1;
}

static Int IsSmallListAList(Obj list)
{
  return 1;
}

static Int LenListAList(Obj list)
{
  return (Int)(ADDR_OBJ(list)[0]);
}

static Obj LengthAList(Obj list)
{
  return INTOBJ_INT(ADDR_OBJ(list)[0]);
}

static Obj Elm0AList(Obj list, Int pos)
{
  UInt len = (UInt) ADDR_OBJ(list)[0];
  if (pos < 1 || pos > len)
    return 0;
  AO_nop_read();
  return ADDR_OBJ(list)[1+pos];
}

static Obj ElmAList(Obj list, Int pos)
{
  UInt len = (UInt)ADDR_OBJ(list)[0];
  while (pos < 1 || pos > len) {
    Obj posobj;
    do {
      posobj = ErrorReturnObj(
	"Atomic List Element: <pos>=%d is an invalid index for <list>",
	(Int) pos, 0L,
	"you can replace value <pos> via 'return <pos>;'" );
    } while (!IS_INTOBJ(posobj));
    pos = INT_INTOBJ(posobj);
  }
  AO_nop_read();
  return ADDR_OBJ(list)[1+pos];
}

static void AssAList(Obj list, Int pos, Obj obj)
{
  UInt len = (UInt)ADDR_OBJ(list)[0];
  while (pos < 1 || pos > len) {
    Obj posobj;
    do {
      posobj = ErrorReturnObj(
	"Atomic List Element: <pos>=%d is an invalid index for <list>",
	(Int) pos, 0L,
	"you can replace value <pos> via 'return <pos>;'" );
    } while (!IS_INTOBJ(posobj));
    pos = INT_INTOBJ(posobj);
  }
  ADDR_OBJ(list)[1+pos] = obj;
  AO_nop_write();
}


/****************************************************************************
**

*F  InitKernel( <module> )  . . . . . . . . initialise kernel data structures
*/
static Int InitKernel (
    StructInitInfo *    module )
{
  /* install info string */
  InfoBags[T_ALIST].name = "atomic list";
  InfoBags[T_AREC].name = "atomic record";
  
    /* install the kind methods */
    TypeObjFuncs[ T_ALIST ] = TypeAList;
    TypeObjFuncs[ T_AREC ] = TypeARecord;
    /* install global variables */
    InitCopyGVar("TYPE_ALIST", &TYPE_ALIST);
    InitCopyGVar("TYPE_AREC", &TYPE_AREC);
    /* install mark functions */
    InitMarkFuncBags(T_ALIST, MarkAtomicList);
    InitMarkFuncBags(T_AREC, MarkAtomicRecord);
    InitMarkFuncBags(T_AREC, MarkAtomicRecord2);
    /* install print functions */
    PrintObjFuncs[ T_ALIST ] = PrintAtomicList;
    PrintObjFuncs[ T_AREC ] = PrintAtomicRecord;
    /* install mutability functions */
    IsMutableObjFuncs [ T_ALIST ] = AlwaysMutable;
    IsMutableObjFuncs [ T_AREC ] = AlwaysMutable;
    MakeBagTypePublic(T_ALIST);
    MakeBagTypePublic(T_AREC);
    /* install list functions */
    IsListFuncs[T_ALIST] = IsListAList;
    IsSmallListFuncs[T_ALIST] = IsSmallListAList;
    LenListFuncs[T_ALIST] = LenListAList;
    LengthFuncs[T_ALIST] = LengthAList;
    Elm0ListFuncs[T_ALIST] = Elm0AList;
    Elm0vListFuncs[T_ALIST] = Elm0AList;
    ElmListFuncs[T_ALIST] = ElmAList;
    ElmvListFuncs[T_ALIST] = ElmAList;
    ElmwListFuncs[T_ALIST] = ElmAList;
    AssListFuncs[T_ALIST] = AssAList;
    /* AsssListFuncs[T_ALIST] = AsssAList; */
    /* install record functions */
    ElmRecFuncs[ T_AREC ] = ElmARecord;
    IsbRecFuncs[ T_AREC ] = IsbARecord;
    AssRecFuncs[ T_AREC ] = AssARecord;
    /* return success                                                      */
    return 0;
}


/****************************************************************************
**
*F  PostRestore( <module> ) . . . . . . . . . . . . . after restore workspace
*/
static Int PostRestore (
    StructInitInfo *    module )
{
    /* return success                                                      */
    return 0;
}


/****************************************************************************
**
*F  InitLibrary( <module> ) . . . . . . .  initialise library data structures
*/
static Int InitLibrary (
    StructInitInfo *    module )
{
    /* init filters and functions                                          */
    InitGVarFuncsFromTable( GVarFuncs );

    /* return success                                                      */
    return 0;
}

/****************************************************************************
**
*F  InitInfoAObjects() . . . . . . . . . . . . . . . table of init functions
*/
static StructInitInfo module = {
    MODULE_BUILTIN,                     /* type                           */
    "aobjects",                         /* name                           */
    0,                                  /* revision entry of c file       */
    0,                                  /* revision entry of h file       */
    0,                                  /* version                        */
    0,                                  /* crc                            */
    InitKernel,                         /* initKernel                     */
    InitLibrary,                        /* initLibrary                    */
    0,                                  /* checkInit                      */
    0,                                  /* preSave                        */
    0,                                  /* postSave                       */
    0                         		/* postRestore                    */
};

StructInitInfo * InitInfoAObjects ( void )
{
    /* TODO: Insert proper revision numbers. */
    module.revision_c = "@(#)$Id: aobjects.c,v 1.0 ";
    module.revision_h = "@(#)$Id: aobjects.h,v 1.0 ";
    FillInVersion( &module );
    return &module;
}
