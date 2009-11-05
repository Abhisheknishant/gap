#ifndef _TLS_H
#define _TLS_H

#include <stdint.h>

#include "tlsconfig.h"

typedef struct ThreadLocalStorage
{
  int threadID;
  /* From intrprtr.c */
  Obj intrResult;
  UInt intrIgnoring;
  UInt intrReturning;
  UInt intrCoding;
  Obj intrState;
  Obj stackObj;
  Int countObj;
  /* From vars.c */
  Bag bottomLVars;
  Bag currLVars;
  Obj *ptrLVars;
  /* From scanner.c */
  Char value[MAX_VALUE_LEN];
  UInt valueLen;
  UInt nrError;
  UInt nrErrLine;
} ThreadLocalStorage;

typedef struct
{
  struct TLSHandler *nextHandler;
  void (*constructor)();
  void (*destructor)();
} TLSHandler;

void InstallTLSHandler(
	TLSHandler *handler,
	void (*constructor)(),
	void (*destructor)()
);

void RunTLSConstructors();
void RunTLSDestructors();

#ifdef HAVE_NATIVE_TLS

extern __thread ThreadLocalStorage TLSInstance;

#define TLS (&TLSInstance)

#else

static inline ThreadLocalStorage *GetTLS()
{
  void *stack;
#ifdef __GNUC__
  stack = __builtin_frame_address(0);
#else
  int dummy[0];
  stack = dummy;
#endif
  return (ThreadLocalStorage *) (((uintptr_t) stack) & TLS_MASK);
}

#define TLS (GetTLS())

#endif /* HAVE_NATIVE_TLS */

void InitializeTLS();

#endif /* _TLS_H */
