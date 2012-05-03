#ifndef GAP_ATOMIC_H
#define GAP_ATOMIC_H

#include <atomic_ops.h>

#define MEMBAR_READ() (AO_nop_read())
#define MEMBAR_WRITE() (AO_nop_write())
#define MEMBAR_FULL() (AO_nop_full())
#define COMPARE_AND_SWAP(a,b,c) (AO_compare_and_swap_full((a), (b), (c)))
#define ATOMIC_INC(x) (AO_fetch_and_add1((x)))
#define ATOMIC_DEC(x) (AO_fetch_and_sub1((x)))

typedef AO_t AtomicUInt;

#endif
