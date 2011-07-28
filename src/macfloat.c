/****************************************************************************
**
*W  macfloat.c                   GAP source                      Steve Linton
**
*H  @(#)$Id: macfloat.c,v 4.12 2011/06/06 16:28:08 sal Exp $
**
*Y  Copyright (C)  1996,  Lehrstuhl D für Mathematik,  RWTH Aachen,  Germany
*Y  (C) 1998 School Math and Comp. Sci., University of St Andrews, Scotland
*Y  Copyright (C) 2002 The GAP Group
**
**  This file contains the functions for the macfloat package.
**
** macfloats are stored as bags containing a 64 bit value
*/
#include        "system.h"              /* system dependent part           */

const char * Revision_macfloat_c =
   "@(#)$Id: macfloat.c,v 4.12 2011/06/06 16:28:08 sal Exp $";

#include        "gasman.h"              /* garbage collector               */
#include        "objects.h"             /* objects                         */

#include        "gap.h"                 /* error handling, initialisation  */


#include        "plist.h"               /* lists */
#include        "ariths.h"              /* basic arithmetic                */
#include        "integer.h"             /* basic arithmetic                */

#define INCLUDE_DECLARATION_PART
#include        "macfloat.h"                /* macfloateans                        */
#undef  INCLUDE_DECLARATION_PART

#include        "bool.h"
#include        "scanner.h"
#include        "string.h"
#include        <assert.h>

#include	"code.h"		/* coder                           */
#include	"thread.h"		/* threads			   */
#include	"tls.h"			/* thread-local storage		   */

/* the following two declarations would belong in `saveload.h', but then all
 * files get macfloat dependencies */
extern Double LoadDouble( void);
extern void SaveDouble( Double d);

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_MATH_H
#include <math.h>
#endif

#include <stdlib.h>

#define VAL_MACFLOAT(obj) (*(Double *)ADDR_OBJ(obj))
#define SET_VAL_MACFLOAT(obj, val) (*(Double *)ADDR_OBJ(obj) = val)
#define IS_MACFLOAT(obj) (TNUM_OBJ(obj) == T_MACFLOAT)
#define SIZE_MACFLOAT   sizeof(Double)

/****************************************************************************
**
*F  TypeMacfloat( <macfloat> )  . . . . . . . . . . . . . . . kind of a macfloat value
**
**  'TypeMacfloat' returns the kind of macfloatean values.
**
**  'TypeMacfloat' is the function in 'TypeObjFuncs' for macfloatean values.
*/
Obj TYPE_MACFLOAT;
Obj TYPE_MACFLOAT0;

Obj TypeMacfloat (
    Obj                 val )
{
  
    return VAL_MACFLOAT(val) == 0.0L ? TYPE_MACFLOAT0 : TYPE_MACFLOAT;
}


/****************************************************************************
**
*F  PrintMacfloat( <macfloat> ) . . . . . . . . . . . . . . . . print a macfloat value
**
**  'PrintMacfloat' prints the macfloating value <macfloat>.
*/
void PrintMacfloat (
    Obj                 x )
{
  Char buf[32];
  sprintf(buf, "%.16" PRINTFFORMAT, (TOPRINTFFORMAT) VAL_MACFLOAT(x));
  Pr("%s",(Int)buf, 0);
}


/****************************************************************************
**
*F  EqMacfloat( <macfloatL>, <macfloatR> )  . . . . . . . . .  test if <macfloatL> =  <macfloatR>
**
**  'EqMacfloat' returns 'True' if the two macfloatean values <macfloatL> and <macfloatR> are
**  equal, and 'False' otherwise.
*/
Int EqMacfloat (
    Obj                 macfloatL,
    Obj                 macfloatR )
{
  return VAL_MACFLOAT(macfloatL) == VAL_MACFLOAT(macfloatR);
}


/****************************************************************************
**
*F  LtMacfloat( <macfloatL>, <macfloatR> )  . . . . . . . . .  test if <macfloatL> <  <macfloatR>
**
*/
Int LtMacfloat (
    Obj                 macfloatL,
    Obj                 macfloatR )
{
  return VAL_MACFLOAT(macfloatL) < VAL_MACFLOAT(macfloatR);
}


/****************************************************************************
**
*F  IsMacfloatFilt( <self>, <obj> ) . . . . . . . . . .  test for a macfloatean value
**
**  'IsMacfloatFilt' implements the internal filter 'IsMacfloat'.
**
**  'IsMacfloat( <obj> )'
**
**  'IsMacfloat'  returns  'true'  if  <obj>  is   a macfloatean  value  and  'false'
**  otherwise.
*/
Obj IsMacfloatFilt;

Obj IsMacfloatHandler (
    Obj                 self,
    Obj                 obj )
{
  return IS_MACFLOAT(obj) ? True : False;
}



/****************************************************************************
**
*F  SaveMacfloat( <macfloat> ) . . . . . . . . . . . . . . . . . . . . save a Macfloatean 
**
*/

void SaveMacfloat( Obj obj )
{
  SaveDouble(VAL_MACFLOAT(obj));
  return;
}

/****************************************************************************
**
*F  LoadMacfloat( <macfloat> ) . . . . . . . . . . . . . . . . . . . . save a Macfloatean 
**
*/

void LoadMacfloat( Obj obj )
{
  SET_VAL_MACFLOAT(obj, LoadDouble());
}

static inline Obj NEW_MACFLOAT( Double val )
{
  Obj f;
  f = NewBag(T_MACFLOAT,SIZE_MACFLOAT);
  SET_VAL_MACFLOAT(f,val);
  return f;
}

/****************************************************************************
**
*F  ZeroMacfloat(<macfloat> ) . . . . . . . . . . . . . . . . . . . return the zero 
**
*/


Obj ZeroMacfloat( Obj f )
{
  return NEW_MACFLOAT((Double)0.0);
}

/****************************************************************************
**
*F  AinvMacfloat(<macfloat> ) . . . . . . . . . . . . . . . . . . . unary minus 
**
*/


Obj AInvMacfloat( Obj f )
{
  return NEW_MACFLOAT(-VAL_MACFLOAT(f));
}

/****************************************************************************
**
*F  OneMacfloat(<macfloat> ) . . . . . . . . . . . . . . . . . . . return the one 
**
*/


Obj OneMacfloat( Obj f )
{
  return NEW_MACFLOAT((Double)1.0);
}

/****************************************************************************
**
*F  InvMacfloat(<macfloat> ) . . . . . . . . . . . . . . . . . . . reciprocal
**
*/


Obj InvMacfloat( Obj f )
{
  return NEW_MACFLOAT((Double)1.0/VAL_MACFLOAT(f));
}

/****************************************************************************
**
*F  ProdMacfloat(<macfloatl>, <macfloatr> ) . . . . . . . . . . . . . . . product
**
*/


Obj ProdMacfloat( Obj fl, Obj fr )
{
  return NEW_MACFLOAT(VAL_MACFLOAT(fl)*VAL_MACFLOAT(fr));
}

/****************************************************************************
**
*F  PowMacfloat(<macfloatl>, <macfloatr> ) . . . . . . . . . . . . . . exponentiation
**
*/


Obj PowMacfloat( Obj fl, Obj fr )
{
  return NEW_MACFLOAT(MATH(pow)(VAL_MACFLOAT(fl),VAL_MACFLOAT(fr)));
}

/****************************************************************************
**
*F  SumMacfloat(<macfloatl>, <macfloatr> ) . . . . . . . . . . . . . .  sum
**
*/


Obj SumMacfloat( Obj fl, Obj fr )
{
  return NEW_MACFLOAT(VAL_MACFLOAT(fl)+VAL_MACFLOAT(fr));
}

/****************************************************************************
**
*F  DiffMacfloat(<macfloatl>, <macfloatr> ) . . . . . . . . . . . . . . difference
**
*/


Obj DiffMacfloat( Obj fl, Obj fr )
{
  return NEW_MACFLOAT(VAL_MACFLOAT(fl)-VAL_MACFLOAT(fr));
}

/****************************************************************************
**
*F  QuoMacfloat(<macfloatl>, <macfloatr> ) . . . . . . . . . . . . . . quotient
**
*/


Obj QuoMacfloat( Obj fl, Obj fr )
{
  return NEW_MACFLOAT(VAL_MACFLOAT(fl)/VAL_MACFLOAT(fr));
}

/****************************************************************************
**
*F  LQuoMacfloat(<macfloatl>, <macfloatr> ) . . . . . . . . . . . . . .left quotient
**
*/


Obj LQuoMacfloat( Obj fl, Obj fr )
{
  return NEW_MACFLOAT(VAL_MACFLOAT(fr)/VAL_MACFLOAT(fl));
}

/****************************************************************************
**
*F  ModMacfloat(<macfloatl>, <macfloatr> ) . . . . . . . . . . . . . . .mod
**
*/


Obj ModMacfloat( Obj fl, Obj fr )
{
  return NEW_MACFLOAT(MATH(fmod)(VAL_MACFLOAT(fl),VAL_MACFLOAT(fr)));
}


/****************************************************************************
**
*F  FuncMACFLOAT_INT(<int>) . . . . . . . . . . . . . . . conversion
**
*/

Obj FuncMACFLOAT_INT( Obj self, Obj i)
{
  if (!IS_INTOBJ(i))
    return Fail;
  else
    return NEW_MACFLOAT((Double)INT_INTOBJ(i));
}

/****************************************************************************
**
*F  FuncMACFLOAT_STRING(<string>) . . . . . . . . . . . . . . . conversion
**
*/

Obj FuncMACFLOAT_STRING( Obj self, Obj s)
{

  while (!IsStringConv(s))
    {
      s = ErrorReturnObj("MACFLOAT_STRING: object to be converted must be a string not a %s",
			 (Int)(InfoBags[TNUM_OBJ(s)].name),0,"You can return a string to continue" );
    }
  char * endptr;
  UChar *sp = CHARS_STRING(s);
  Obj res= NEW_MACFLOAT((Double) STRTOD((char *)sp,&endptr));
  if ((UChar *)endptr != sp + GET_LEN_STRING(s)) 
    return Fail;
  return res;
}

/****************************************************************************
**
*F SumIntMacfloat( <int>, <macfloat> )
**
*/

Obj SumIntMacfloat( Obj i, Obj f )
{
  return NEW_MACFLOAT( (Double)(INT_INTOBJ(i)) + VAL_MACFLOAT(f));
}


/****************************************************************************
**
*F FuncSIN_MACFLOAT( <self>, <macfloat> ) . .The sin function from the math library
**
*/

#define MAKEMATHPRIMITIVE(NAME,name)			\
  Obj Func##NAME##_MACFLOAT( Obj self, Obj f)		\
  {							\
    return NEW_MACFLOAT(MATH(name)(VAL_MACFLOAT(f)));	\
  }

#define MAKEMATHPRIMITIVE2(NAME,name)					\
  Obj Func##NAME##_MACFLOAT( Obj self, Obj f, Obj g)			\
  {									\
    return NEW_MACFLOAT(MATH(name)(VAL_MACFLOAT(f),VAL_MACFLOAT(g)));	\
  }

MAKEMATHPRIMITIVE(COS,cos);
MAKEMATHPRIMITIVE(SIN,sin);
MAKEMATHPRIMITIVE(TAN,tan);
MAKEMATHPRIMITIVE(ACOS,acos);
MAKEMATHPRIMITIVE(ASIN,asin);
MAKEMATHPRIMITIVE(ATAN,atan);
MAKEMATHPRIMITIVE(LOG,log);
MAKEMATHPRIMITIVE(EXP,exp);
MAKEMATHPRIMITIVE(SQRT,sqrt);
MAKEMATHPRIMITIVE(RINT,rint);
MAKEMATHPRIMITIVE(FLOOR,floor);
MAKEMATHPRIMITIVE(CEIL,ceil);
MAKEMATHPRIMITIVE2(ATAN2,atan2);
MAKEMATHPRIMITIVE2(HYPOT,hypot);

extern Obj FuncIntHexString(Obj,Obj);

Obj FuncINTFLOOR_MACFLOAT( Obj self, Obj obj )
{
#if defined(_ISOC99_SOURCE)
  Double f = trunc(VAL_MACFLOAT(obj));
#else
  Double f = VAL_MACFLOAT(obj);
  if (f >= 0.0)
    f = floor(f);
  else
    f = -floor(-f);
#endif


  if (fabs(f) < (Double) (1L<<NR_SMALL_INT_BITS))
    return INTOBJ_INT((Int)f);

  int strlen = (int) (log(fabs(f)) / log(16.0)) + 3;

  Obj str = NEW_STRING(strlen);
  char *s = CSTR_STRING(str), *p = s+strlen-1;
  if (f < 0.0)
    f = -f, s[0] = '-';
  while (p > s || (p == s && s[0] != '-')) {
    int d = (int) fmod(f,16.0);
    *p-- = d < 10 ? '0'+d : 'a'+d-10;
    f /= 16.0;
  }
  return FuncIntHexString(self,str);
}

Obj FuncSTRING_DIGITS_MACFLOAT( Obj self, Obj prec, Obj f)
{
  Char buf[32];
  Obj str;
  UInt len;
  sprintf(buf, "%.*" PRINTFFORMAT, (int)INT_INTOBJ(prec), (TOPRINTFFORMAT)VAL_MACFLOAT(f));
  len = SyStrlen(buf);
  str = NEW_STRING(len);
  SyStrncat(CSTR_STRING(str),buf,len);
  return str;
}

Obj FuncSTRING_MACFLOAT( Obj self, Obj f) /* backwards compatibility */
{
  return FuncSTRING_DIGITS_MACFLOAT(self,INTOBJ_INT(PRINTFDIGITS),f);
}

Obj FuncLDEXP_MACFLOAT( Obj self, Obj f, Obj i)
{
  return NEW_MACFLOAT(ldexp(VAL_MACFLOAT(f),INT_INTOBJ(i)));
}

Obj FuncFREXP_MACFLOAT( Obj self, Obj f)
{
  int i;
  Obj d = NEW_MACFLOAT(frexp (VAL_MACFLOAT(f), &i));
  Obj l = NEW_PLIST(T_PLIST,2);
  SET_ELM_PLIST(l,1,d);
  SET_ELM_PLIST(l,2,INTOBJ_INT(i));
  SET_LEN_PLIST(l,2);
  return l;
}

/****************************************************************************
**

*F * * * * * * * * * * * * * initialize package * * * * * * * * * * * * * * *
*/

/****************************************************************************
**

*V  GVarFilts . . . . . . . . . . . . . . . . . . . list of filters to export
*/
static StructGVarFilt GVarFilts [] = {

    { "IS_MACFLOAT", "obj", &IsMacfloatFilt,
      IsMacfloatHandler, "src/macfloat.c:IS_MACFLOAT" },

    { 0 }

};


/****************************************************************************
**
*V  GVarFuncs . . . . . . . . . . . . . . . . . . list of functions to export
*/
static StructGVarFunc GVarFuncs [] = {
  { "MACFLOAT_INT", 1, "int",
    FuncMACFLOAT_INT, "src/macfloat.c:MACFLOAT_INT" },

  { "MACFLOAT_STRING", 1, "string",
    FuncMACFLOAT_STRING, "src/macfloat.c:MACFLOAT_STRING" },

  { "SIN_MACFLOAT", 1, "macfloat",
    FuncSIN_MACFLOAT, "src/macfloat.c:SIN_MACFLOAT" },

  { "COS_MACFLOAT", 1, "macfloat",
    FuncCOS_MACFLOAT, "src/macfloat.c:COS_MACFLOAT" },

  { "TAN_MACFLOAT", 1, "macfloat",
    FuncTAN_MACFLOAT, "src/macfloat.c:TAN_MACFLOAT" },

  { "ASIN_MACFLOAT", 1, "macfloat",
    FuncASIN_MACFLOAT, "src/macfloat.c:ASIN_MACFLOAT" },

  { "ACOS_MACFLOAT", 1, "macfloat",
    FuncACOS_MACFLOAT, "src/macfloat.c:ACOS_MACFLOAT" },

  { "ATAN_MACFLOAT", 1, "macfloat",
    FuncATAN_MACFLOAT, "src/macfloat.c:ATAN_MACFLOAT" },

  { "ATAN2_MACFLOAT", 2, "real, imag",
    FuncATAN2_MACFLOAT, "src/macfloat.c:ATAN2_MACFLOAT" },

  { "LOG_MACFLOAT", 1, "macfloat",
    FuncLOG_MACFLOAT, "src/macfloat.c:LOG_MACFLOAT" },

  { "EXP_MACFLOAT", 1, "macfloat",
    FuncEXP_MACFLOAT, "src/macfloat.c:EXP_MACFLOAT" },

  { "LDEXP_MACFLOAT", 2, "macfloat, int",
    FuncLDEXP_MACFLOAT, "src/macfloat.c:LDEXP_MACFLOAT" },

  { "FREXP_MACFLOAT", 1, "macfloat",
    FuncFREXP_MACFLOAT, "src/macfloat.c:FREXP_MACFLOAT" },

  { "SQRT_MACFLOAT", 1, "macfloat",
    FuncSQRT_MACFLOAT, "src/macfloat.c:SQRT_MACFLOAT" },

  { "RINT_MACFLOAT", 1, "macfloat",
    FuncRINT_MACFLOAT, "src/macfloat.c:RINT_MACFLOAT" },

  { "INTFLOOR_MACFLOAT", 1, "macfloat",
    FuncINTFLOOR_MACFLOAT, "src/macfloat.c:INTFLOOR_MACFLOAT" },

  { "FLOOR_MACFLOAT", 1, "macfloat",
    FuncFLOOR_MACFLOAT, "src/macfloat.c:FLOOR_MACFLOAT" },

  { "STRING_MACFLOAT", 1, "macfloat",
    FuncSTRING_MACFLOAT, "src/macfloat.c:STRING_MACFLOAT" },

  { "STRING_DIGITS_MACFLOAT", 2, "digits, macfloat",
    FuncSTRING_DIGITS_MACFLOAT, "src/macfloat.c:STRING_DIGITS_MACFLOAT" },

  {0}
};


/****************************************************************************
**

*F  InitKernel( <module> )  . . . . . . . . initialise kernel data structures
*/
static Int InitKernel (
    StructInitInfo *    module )
{
    Int EqObject (Obj,Obj);

    /* install the marking functions for macfloatean values                    */
    InfoBags[ T_MACFLOAT ].name = "macfloat";
    InitMarkFuncBags( T_MACFLOAT, MarkNoSubBags );

    /* init filters and functions                                          */
    InitHdlrFiltsFromTable( GVarFilts );
    InitHdlrFuncsFromTable( GVarFuncs );

    /* install the kind function                                           */
    ImportGVarFromLibrary( "TYPE_MACFLOAT", &TYPE_MACFLOAT );
    ImportGVarFromLibrary( "TYPE_MACFLOAT0", &TYPE_MACFLOAT0 );
    TypeObjFuncs[ T_MACFLOAT ] = TypeMacfloat;

    /* install the saving functions                                       */
    SaveObjFuncs[ T_MACFLOAT ] = SaveMacfloat;

    /* install the loading functions                                       */
    LoadObjFuncs[ T_MACFLOAT ] = LoadMacfloat;

    /* install the printer for macfloatean values                              */
    PrintObjFuncs[ T_MACFLOAT ] = PrintMacfloat;

    /* install the comparison functions                                    */
    EqFuncs[ T_MACFLOAT ][ T_MACFLOAT ] = EqMacfloat;
    LtFuncs[ T_MACFLOAT ][ T_MACFLOAT ] = LtMacfloat;

    /* allow method selection to protest against comparisons of float and int */
    {
      int t;
      for (t = T_INT; t <= T_CYC; t++)
	EqFuncs[T_MACFLOAT][t] = EqFuncs[t][T_MACFLOAT] = EqObject;
    }

    /* install the unary arithmetic methods                                */
    ZeroFuncs[ T_MACFLOAT ] = ZeroMacfloat;
    ZeroMutFuncs[ T_MACFLOAT ] = ZeroMacfloat;
    AInvMutFuncs[ T_MACFLOAT ] = AInvMacfloat;
    OneFuncs [ T_MACFLOAT ] = OneMacfloat;
    OneMutFuncs [ T_MACFLOAT ] = OneMacfloat;
    InvFuncs [ T_MACFLOAT ] = InvMacfloat;

    /* install binary arithmetic methods */
    ProdFuncs[ T_MACFLOAT ][ T_MACFLOAT ] = ProdMacfloat;
    PowFuncs [ T_MACFLOAT ][ T_MACFLOAT ] = PowMacfloat;
    SumFuncs[ T_MACFLOAT ][ T_MACFLOAT ] = SumMacfloat;
    DiffFuncs [ T_MACFLOAT ][ T_MACFLOAT ] = DiffMacfloat;
    QuoFuncs [ T_MACFLOAT ][ T_MACFLOAT ] = QuoMacfloat;
    LQuoFuncs [ T_MACFLOAT ][ T_MACFLOAT ] = LQuoMacfloat;
    ModFuncs [ T_MACFLOAT ][ T_MACFLOAT ] = ModMacfloat;
    SumFuncs [ T_INT ][ T_MACFLOAT ] = SumIntMacfloat;
    
    /* Probably support mixed ops with small ints in the kernel as well
       on any reasonable system, all small ints should have macfloat equivalents

       Anything else, like mixed ops with rationals, we can leave to the library
       at least for a while */
     
    
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
/*  UInt            gvar; */
/*  Obj             tmp;  */

    /* init filters and functions                                          */
    InitGVarFiltsFromTable( GVarFilts );
    InitGVarFuncsFromTable( GVarFuncs );

    /* return success                                                      */
    return 0;
}


/****************************************************************************
**
*F  InitInfoMacfloat()  . . . . . . . . . . . . . . . . . table of init functions
*/
static StructInitInfo module = {
    MODULE_BUILTIN,                     /* type                           */
    "macfloat",                             /* name                           */
    0,                                  /* revision entry of c file       */
    0,                                  /* revision entry of h file       */
    0,                                  /* version                        */
    0,                                  /* crc                            */
    InitKernel,                         /* initKernel                     */
    InitLibrary,                        /* initLibrary                    */
    0,                                  /* checkInit                      */
    0,                                  /* preSave                        */
    0,                                  /* postSave                       */
    0                                   /* postRestore                    */
};

StructInitInfo * InitInfoMacfloat ( void )
{
    module.revision_c = Revision_macfloat_c;
    module.revision_h = Revision_macfloat_h;
    FillInVersion( &module );
    return &module;
}


/****************************************************************************
**

*E  macfloat.c  . . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
*/
