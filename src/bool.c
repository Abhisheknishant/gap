/****************************************************************************
**
*W  bool.c                      GAP source                   Martin Schoenert
**
*H  @(#)$Id$
**
*Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
**
**  This file contains the functions for the boolean package.
*/
char *          Revision_bool_c =
   "@(#)$Id$";

#include        "system.h"              /* Ints, UInts                     */
#include        "scanner.h"             /* Pr                              */
#include        "gasman.h"              /* NewBag, CHANGED_BAG             */

#include        "objects.h"             /* Obj, TYPE_OBJ, types            */
#include        "gvars.h"               /* AssGVar, GVarName               */

#include        "calls.h"               /* NewFunctionC                    */
#include        "opers.h"               /* NewFilterC                      */

#include        "ariths.h"              /* EqFuncs, LtFuncs                */

#define INCLUDE_DECLARATION_PART
#include        "bool.h"                /* declaration part of the package */
#undef  INCLUDE_DECLARATION_PART

#include        "gap.h"                 /* Error                          ?*/


/****************************************************************************
**

*V  True  . . . . . . . . . . . . . . . . . . . . . . . . . . . .  true value
**
**   'True' is the value 'true'.
*/
Obj True;


/****************************************************************************
**
*V  False . . . . . . . . . . . . . . . . . . . . . . . . . . . . false value
**
**  'False' is the value 'false'.
*/
Obj False;


/****************************************************************************
**
*V  Fail  . . . . . . . . . . . . . . . . . . . . . . . . . . . .  fail value
**
**  'Fail' is the value 'fail'.
*/
Obj Fail;


/****************************************************************************
**

*F  KindBool( <bool> )  . . . . . . . . . . . . . . . kind of a boolean value
**
**  'KindBool' returns the kind of boolean values.
**
**  'KindBool' is the function in 'KindObjFuncs' for boolean values.
*/
Obj KIND_BOOL;

Obj KindBool (
    Obj                 val )
{
    return KIND_BOOL;
}


/****************************************************************************
**
*F  PrintBool( <bool> ) . . . . . . . . . . . . . . . . print a boolean value
**
**  'PrintBool' prints the boolean value <bool>.
*/
void PrintBool (
    Obj                 bool )
{
    if ( bool == True ) {
        Pr( "true", 0L, 0L );
    }
    else if ( bool == False ) {
        Pr( "false", 0L, 0L );
    }
    else if ( bool == Fail ) {
        Pr( "fail", 0L, 0L );
    }
    else {
        Pr( "<<very strange boolean value>>", 0L, 0L );
    }
}


/****************************************************************************
**
*F  EqBool( <boolL>, <boolR> )  . . . . . . . . .  test if <boolL> =  <boolR>
**
**  'EqBool' returns 'True' if the two boolean values <boolL> and <boolR> are
**  equal, and 'False' otherwise.
*/
Int EqBool (
    Obj                 boolL,
    Obj                 boolR )
{
    if ( boolL == boolR ) {
        return 1L;
    }
    else {
        return 0L;
    }
}


/****************************************************************************
**
*F  LtBool( <boolL>, <boolR> )  . . . . . . . . .  test if <boolL> <  <boolR>
**
**  'LtBool' return  'True'  if the boolean   value <boolL> is less  than the
**  boolean value <boolR> and 'False' otherwise.
*/
Int LtBool (
    Obj                 boolL,
    Obj                 boolR )
{
    if ( boolL == True && boolR == False ) {
        return 1L;
    }
    else {
        return 0L;
    }
}


/****************************************************************************
**
*F  IsBoolFilt( <self>, <obj> ) . . . . . . . . . .  test for a boolean value
**
**  'IsBoolFilt' implements the internal filter 'IsBool'.
**
**  'IsBool( <obj> )'
**
**  'IsBool'  returns  'true'  if  <obj>  is   a boolean  value  and  'false'
**  otherwise.
*/
Obj IsBoolFilt;

Obj IsBoolHandler (
    Obj                 self,
    Obj                 obj )
{
    /* return 'true' if <obj> is a boolean and 'false' otherwise           */
    if ( TYPE_OBJ(obj) == T_BOOL ) {
        return True;
    }
    else if ( TYPE_OBJ(obj) < FIRST_EXTERNAL_TYPE ) {
        return False;
    }
    else {
        return DoFilter( self, obj );
    }
}


/****************************************************************************
**
*F  ReturnTrue1( <val1> ) . . . . . . . . . . . . . . . . . .  return  'True'
*F  ReturnTrue2( <val1>, <val2> ) . . . . . . . . . . . . . .  return  'True'
*F  ReturnTrue3( <val1>, <val2>, <val3> ) . . . . . . . . . .  return  'True'
*F  ReturnFalse1( <val1> )  . . . . . . . . . . . . . . . . .  return 'False'
*F  ReturnFalse2( <val1>, <val2> )  . . . . . . . . . . . . .  return 'False'
*F  ReturnFalse3( <val1>, <val2>, <val3> )  . . . . . . . . .  return 'False'
*F  ReturnFail1( <val1> ) . . . . . . . . . . . . . . . . . .  return  'Fail'
*F  ReturnFail2( <val1>, <val2> ) . . . . . . . . . . . . . .  return  'Fail'
*F  ReturnFail3( <val1>, <val2>, <val3> ) . . . . . . . . . .  return  'Fail'
**
**  'ReturnTrue?'  simply return  'True'  independent of  the values of   the
**  arguments.
**
**  'ReturnFalse?' likewise return 'False'.
**
**  'ReturnFail?' likewise return 'Fail'.
**
**  Those  functions are  useful for  dispatcher  tables if the types already
**  determine the outcome.
*/
Obj ReturnTrueFunc;

Obj ReturnTrue1 (
    Obj                 self,
    Obj                 val1 )
{
    return True;
}

Obj ReturnTrue2 (
    Obj                 self,
    Obj                 val1,
    Obj                 val2 )
{
    return True;
}

Obj ReturnTrue3 (
    Obj                 self,
    Obj                 val1,
    Obj                 val2,
    Obj                 val3 )
{
    return True;
}


Obj ReturnFalseFunc;

Obj ReturnFalse1 (
    Obj                 self,
    Obj                 val1 )
{
    return False;
}

Obj ReturnFalse2 (
    Obj                 self,
    Obj                 val1,
    Obj                 val2 )
{
    return False;
}

Obj ReturnFalse3 (
    Obj                 self,
    Obj                 val1,
    Obj                 val2,
    Obj                 val3 )
{
    return False;
}


Obj ReturnFailFunc;

Obj ReturnFail1 (
    Obj                 self,
    Obj                 val1 )
{
    return Fail;
}

Obj ReturnFail2 (
    Obj                 self,
    Obj                 val1,
    Obj                 val2 )
{
    return Fail;
}

Obj ReturnFail3 (
    Obj                 self,
    Obj                 val1,
    Obj                 val2,
    Obj                 val3 )
{
    return Fail;
}


/****************************************************************************
**

*E  InitBool()  . . . . . . . . . . . . . . . initialize the booleans package
**
**  'InitBool' initializes the boolean package.
*/
void            InitBool ( void )
{
    /* install the marking functions for boolean values                    */
    InfoBags[           T_BOOL          ].name = "boolean";
    InitMarkFuncBags(   T_BOOL          , MarkNoSubBags );

    /* make the two bags                                                   */
    InitGlobalBag( &True  );
    True  = NewBag( T_BOOL, 0L );
    InitGlobalBag( &False );
    False = NewBag( T_BOOL, 0L );
    InitGlobalBag( &Fail );
    Fail = NewBag( T_BOOL, 0L );
    AssGVar( GVarName( "FAIL" ), Fail );

    /* install the kind function                                           */
    ImportGVarFromLibrary( "KIND_BOOL", &KIND_BOOL );
    KindObjFuncs[ T_BOOL ] = KindBool;

    /* install the printer for boolean values                              */
    PrintObjFuncs[ T_BOOL ] = PrintBool;

    /* install the comparison functions                                    */
    EqFuncs[ T_BOOL ][ T_BOOL ] = EqBool;
    LtFuncs[ T_BOOL ][ T_BOOL ] = LtBool;

    /* make and install the 'IS_BOOL' filter                               */
    IsBoolFilt = NewFilterC( "IS_BOOL", 1L, "obj", IsBoolHandler );
    AssGVar( GVarName( "IS_BOOL" ), IsBoolFilt );

    /* make and install the 'RETURN_TRUE' function                         */
    ReturnTrueFunc = NewFunctionC( "RETURN_TRUE", -1L, "args", ReturnTrue1 );
    HDLR_FUNC( ReturnTrueFunc, 1 ) = ReturnTrue1;
    HDLR_FUNC( ReturnTrueFunc, 2 ) = ReturnTrue2;
    HDLR_FUNC( ReturnTrueFunc, 3 ) = ReturnTrue3;
    AssGVar( GVarName( "RETURN_TRUE" ), ReturnTrueFunc );

    /* make and install the 'RETURN_FALSE' function                        */
    ReturnFalseFunc = NewFunctionC("RETURN_FALSE",-1L,"args",ReturnFalse1);
    HDLR_FUNC( ReturnFalseFunc, 1 ) = ReturnFalse1;
    HDLR_FUNC( ReturnFalseFunc, 2 ) = ReturnFalse2;
    HDLR_FUNC( ReturnFalseFunc, 3 ) = ReturnFalse3;
    AssGVar( GVarName( "RETURN_FALSE" ), ReturnFalseFunc );

    /* make and install the 'RETURN_FAIL' function                        */
    ReturnFailFunc = NewFunctionC("RETURN_FAIL", -1L, "args", ReturnFail1);
    HDLR_FUNC( ReturnFailFunc, 1 ) = ReturnFail1;
    HDLR_FUNC( ReturnFailFunc, 2 ) = ReturnFail2;
    HDLR_FUNC( ReturnFailFunc, 3 ) = ReturnFail3;
    AssGVar( GVarName( "RETURN_FAIL" ), ReturnFailFunc );
}
