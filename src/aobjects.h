#ifndef GAP_AOBJECTS_H
#define GAP_AOBJECTS_H

StructInitInfo *InitInfoAObjects(void);
Obj SetARecordField(Obj record, UInt field, Obj obj);
Obj GetARecordField(Obj record, UInt field);
Obj AssTLRecord(Obj record, UInt field, Obj obj);
Obj GetTLRecordField(Obj record, UInt field);
Obj FromAtomicRecord(Obj record);
void SetTLDefault(Obj record, UInt rnam, Obj value);
void SetTLConstructor(Obj record, UInt rnam, Obj func);

/*****************************************************************************
**
*F  ATOMIC_SET_ELM_PLIST(<list>, <index>, <value>)
*F  ATOMIC_SET_ELM_PLIST_ONCE(<list>, <index>, <value>)
*F  ATOMIC_ELM_PLIST(<list>, <index>)
**
**  Set or access plain lists atomically. The plain lists must be of fixed
**  size and not be resized concurrently with such operations. The functions
**  assume that <index> is in the range 1..LEN_PLIST(<list>).
**
**  <value> must be an atomic or immutable object or access to it must be
**  properly regulated by locks.
**
**  'ATOMIC_ELM_PLIST' and 'ATOMIC_SET_ELM_PLIST' read and write plain lists,
**  annotated with memory barriers that ensure that concurrent threads do
**  not read objects that have not been fully initialized.
**
**  'ATOMIC_SET_ELM_PLIST_ONCE' assigns a value similar to 'SET_PLIST',
**  but only if <list>[<index>] is currently unbound. If that value has
**  been bound already, it will return the existing value; otherwise it
**  assigns <value> and returns it.
**
**  Canonical usage to read or initialize the field of a plist is as
**  follows:
**
**    obj = ATOMIC_ELM_PLIST(list, index);
**    if (!obj) {
**       obj = ...;
**       obj = ATOMIC_SET_ELM_PLIST(list, index, obj);
**    }
**
**  This construction ensures that while <obj> may be calculated more
**  than once, all threads will share the same value; furthermore,
**  reading an alreadu initialized value is generally very cheap,
**  incurring the cost of a read, a read barrier, and a branch (which,
**  after initialization, will generally predicted correctly by branch
**  prediction logic).
*/


static inline void ATOMIC_SET_ELM_PLIST(Obj list, UInt index, Obj value) {
#ifndef WARD_ENABLED
  Obj *contents = ADDR_OBJ(list);
  MEMBAR_WRITE(); /* ensure that contents[index] becomes visible to
                   * other threads only after value has become visible,
		   * too.
		   */
  contents[index] = value;
#endif
}

static inline Obj ATOMIC_SET_ELM_PLIST_ONCE(Obj list, UInt index, Obj value) {
#ifndef WARD_ENABLED
  Obj *contents = ADDR_OBJ(list);
  Obj result;
  for (;;) {
    result = contents[index];
    if (result) {
      MEMBAR_READ(); /* matching memory barrier. */
      return result;
    }
    if (COMPARE_AND_SWAP((AO_t *)(contents+index),
      (AO_t) 0, (AO_t) value)) {
      /* no extra memory barrier needed, a full barrier is implicit in the
       * COMPARE_AND_SWAP() call.
       */
      return value;
    }
  }
#endif
}

static inline Obj ATOMIC_ELM_PLIST(Obj list, UInt index) {
#ifndef WARD_ENABLED
  Obj *contents = ADDR_OBJ(list);
  Obj result;
  result = contents[index];
  MEMBAR_READ(); /* matching memory barrier. */
  return result;
#endif
}

#endif
