/****************************************************************************
**
*W  gap.c                       GAP source                       Frank Celler
*W                                                         & Martin Schoenert
**
*H  @(#)$Id$
**
*Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
**
**  This file contains the various read-eval-print loops and  related  stuff.
*/
char * Revision_gap_c =
   "@(#)$Id$";


#include        <stdio.h>

#include        "system.h"              /* Ints, UInts                     */
#include        "scanner.h"             /* Pr                              */
extern char * In;
#include        "gasman.h"              /* NewBag, CHANGED_BAG             */

#include        "objects.h"             /* Obj, TYPE_OBJ, types            */
#include        "gvars.h"               /* InitGVars                       */

#include        "calls.h"               /* InitCalls                       */
#include        "opers.h"               /* InitOpers                       */

#include        "ariths.h"              /* InitAriths                      */
#include        "records.h"             /* InitRecords                     */
#include        "lists.h"               /* InitLists                       */

#include        "bool.h"                /* InitBool                        */

#include        "integer.h"             /* InitInt                         */
#include        "rational.h"            /* InitRat                         */
#include        "cyclotom.h"            /* InitCyc                         */

#include        "finfield.h"            /* InitFinfield                    */
#include        "permutat.h"            /* InitPermutat                    */

#include        "precord.h"             /* InitPRecord                     */

#include        "listoper.h"            /* InitListOper                    */
#include        "listfunc.h"            /* InitListFunc                    */

#include        "plist.h"               /* InitPlist                       */
#include        "set.h"                 /* InitSet                         */
#include        "vector.h"              /* InitVector                      */

#include        "blister.h"             /* InitBlist                       */
#include        "range.h"               /* InitRange                       */
#include        "string.h"              /* InitString                      */

#include        "objfgelm.h"            /* InitFreeGroupElements           */
#include        "objscoll.h"            /* InitSingleCollector             */
#include        "objpcgel.h"            /* InitPcElements                  */
#include        "objcftl.h"             /* Init polycyclic collector       */

#include        "sctable.h"             /* InitSCTable                     */
#include        "costab.h"              /* InitCosetTable                  */

#include        "code.h"                /* InitCode                        */

#include        "vars.h"                /* InitVars                        */
#include        "exprs.h"               /* InitExprs                       */
#include        "stats.h"               /* InitStats                       */
#include        "funcs.h"               /* InitFuncs                       */

#include        "dt.h"                  /* InitDeepThought                 */
#include        "dteval.h"              /* InitDTEvaluation                */

#include        "intrprtr.h"            /* InitInterpreter                 */

#include        "compiler.h"            /* InitCompiler                    */

#include        "read.h"                /* ReadEvalCommand, ReadEvalResult */

#include        "compstat.h"            /* statically linked modules       */

#define INCLUDE_DECLARATION_PART
#include        "gap.h"                 /* declaration part of the package */
#undef  INCLUDE_DECLARATION_PART


/****************************************************************************
**

*V  Last  . . . . . . . . . . . . . . . . . . . . . . global variable  'last'
*V  Last2 . . . . . . . . . . . . . . . . . . . . . . global variable 'last2'
*V  Last3 . . . . . . . . . . . . . . . . . . . . . . global variable 'last3'
**
**  'Last',  'Last2', and 'Last3'  are the  global variables 'last', 'last2',
**  and  'last3', which are automatically  assigned  the result values in the
**  main read-eval-print loop.
*/
UInt Last;
UInt Last2;
UInt Last3;


/****************************************************************************
**
*V  Time  . . . . . . . . . . . . . . . . . . . . . . global variable  'time'
**
**  'Time' is the global variable 'time', which is automatically assigned the
**  time the last command took.
*/
UInt Time;


/****************************************************************************
**

*F  main( <argc>, <argv> )  . . . . . . .  main program, read-eval-print loop
*/
int main (
    int                 argc,
    char *              argv [] )
{
    UInt                type;                   /* type of command         */
    UInt                time;                   /* start time              */
    Obj                 func;                   /* function (compiler)     */
    UInt4               crc;                    /* crc of file to compile  */

    /* initialize everything                                               */
    InitGap( &argc, argv );

    /* maybe compile                                                       */
    if ( SyCompilePlease ) {
        func = READ_AS_FUNC( SyCompileInput );
        if ( func == Fail )  SyExit( 1 );
        crc = SyGAPCRC( SyCompileInput );
        type = CompileFunc(
            SyCompileOutput, func, SyCompileName,
            crc, SyCompileMagic1 );
        if ( type == 0 )  SyExit( 1 );
        SyExit( 0 );
    }

    /* read-eval-print loop                                                */
    while ( 1 ) {

        /* start the stopwatch                                             */
        time = SyTime();

        /* read and evaluate one command                                   */
        Prompt = "gap> ";
        NrError = 0;
        type = ReadEvalCommand();

        /* stop the stopwatch                                              */
        AssGVar( Time, INTOBJ_INT( SyTime() - time ) );

        /* handle ordinary command                                         */
        if ( type == 0 && ReadEvalResult != 0 ) {

            /* remember the value in 'last' and the time in 'time'         */
            AssGVar( Last3, VAL_GVAR( Last2 ) );
            AssGVar( Last2, VAL_GVAR( Last  ) );
            AssGVar( Last,  ReadEvalResult   );

            /* print the result                                            */
            if ( *In != ';' ) {
                IsStringConv( ReadEvalResult );
                PrintObj( ReadEvalResult );
                Pr( "\n", 0L, 0L );
            }
            else {
                Match( S_SEMICOLON, ";", 0UL );
            }

        }

        /* handle return-value or return-void command                      */
        else if ( type == 1 || type == 2 ) {
            Pr(
                "'return' must not be used in main read-eval-print loop",
                0L, 0L );
        }

        /* handle quit command or <end-of-file>                            */
        else if ( type == 8 || type == 16 ) {
            break;
        }

    }

    /* exit to the operating system, the return is there to please lint    */
    SyExit( 0 );
    return 0;
}


/****************************************************************************
**
*F  FuncID_FUNC( <self>, <val1> ) . . . . . . . . . . . . . . . return <val1>
*/
Obj FuncID_FUNC (
    Obj                 self,
    Obj                 val1 )
{
    return val1;
}


/****************************************************************************
**
*F  FuncRuntime( <self> ) . . . . . . . . . . . . internal function 'Runtime'
**
**  'FuncRuntime' implements the internal function 'Runtime'.
**
**  'Runtime()'
**
**  'Runtime' returns the time spent since the start of GAP in  milliseconds.
**  How much time execution of statements take is of course system dependent.
**  The accuracy of this number is also system dependent.
*/
Obj FuncRuntime (
    Obj                 self )
{
    return INTOBJ_INT( SyTime() );
}


/****************************************************************************
**
*F  SizeScreenHandler(<self>,<args>)  . . . .  internal function 'SizeScreen'
**
**  'SizeScreenHandler' implements  the internal function 'SizeScreen' to get
**  or set the actual screen size.
**
**  'SizeScreen()'
**
**  In this form 'SizeScreen'  returns the size of the  screen as a list with
**  two  entries.  The first is  the length of each line,  the  second is the
**  number of lines.
**
**  'SizeScreen( [ <x>, <y> ] )'
**
**  In this form 'SizeScreen' sets the size of the screen.  <x> is the length
**  of each line, <y> is the number of lines.  Either value may  be  missing,
**  to leave this value unaffected.  Note that those parameters can  also  be
**  set with the command line options '-x <x>' and '-y <y>'.
*/
Obj SizeScreenHandler (
    Obj                 self,
    Obj                 args )
{
    Obj                 size;           /* argument and result list        */
    Obj                 elm;            /* one entry from size             */
    UInt                len;            /* length of lines on the screen   */
    UInt                nr;             /* number of lines on the screen   */

    /* check the arguments                                                 */
    while ( ! IS_LIST(args) || 1 < LEN_LIST(args) ) {
        args = ErrorReturnObj(
            "Function: number of arguments must be 0 or 1 (not %d)",
            LEN_LIST(args), 0L,
            "you can return a list of arguments" );
    }

    /* get the arguments                                                   */
    if ( LEN_LIST(args) == 0 ) {
        size = NEW_PLIST( T_PLIST, 0 );
        SET_LEN_PLIST( size, 0 );
    }

    /* otherwise check the argument                                        */
    else {
        size = ELM_LIST( args, 1 );
        while ( ! IS_LIST(size) || 2 < LEN_LIST(size) ) {
            size = ErrorReturnObj(
                "SizeScreen: <size> must be a list of length 2",
                0L, 0L,
                "you can return a new list for <size>" );
        }
    }

    /* extract the length                                                  */
    if ( LEN_LIST(size) < 1 || ELM0_LIST(size,1) == 0 ) {
        len = SyNrCols;
    }
    else {
        elm = ELMW_LIST(size,1);
        while ( TYPE_OBJ(elm) != T_INT ) {
            elm = ErrorReturnObj(
                "SizeScreen: <x> must be an integer",
                0L, 0L,
                "you can return a new integer for <x>" );
        }
        len = INT_INTOBJ( elm );
        if ( len < 20  )  len = 20;
        if ( 256 < len )  len = 256;
    }

    /* extract the number                                                  */
    if ( LEN_LIST(size) < 2 || ELM0_LIST(size,2) == 0 ) {
        nr = SyNrRows;
    }
    else {
        elm = ELMW_LIST(size,2);
        while ( TYPE_OBJ(elm) != T_INT ) {
            elm = ErrorReturnObj(
                "SizeScreen: <y> must be an integer",
                0L, 0L,
                "you can return a new integer for <y>" );
        }
        nr = INT_INTOBJ( elm );
        if ( nr < 10 )  nr = 10;
    }

    /* set length and number                                               */
    SyNrCols = len;
    SyNrRows = nr;

    /* make and return the size of the screen                              */
    size = NEW_PLIST( T_PLIST, 2 );
    SET_LEN_PLIST( size, 2 );
    SET_ELM_PLIST( size, 1, INTOBJ_INT(len) );
    SET_ELM_PLIST( size, 2, INTOBJ_INT(nr)  );
    return size;

}


/****************************************************************************
**

*F * * * * * * * * * * * * * * print and error  * * * * * * * * * * * * * * *
*/



/****************************************************************************
**

*F  FuncPrint( <self>, <args> ) . . . . . . . . . . . . . . . .  print <args>
*/
Obj FuncPrint (
    Obj                 self,
    Obj                 args )
{
    Obj                 arg;
    UInt                i;

    /* print all the arguments, take care of strings and functions         */
    for ( i = 1; i <= LEN_PLIST(args); i++ ) {
        arg = ELM_LIST(args,i);
        if ( IsStringConv(arg) && MUTABLE_TYPE(TYPE_OBJ(arg))==T_STRING ) {
            PrintString1(arg);
        }
        else if ( TYPE_OBJ( arg ) == T_FUNCTION ) {
            PrintObjFull = 1;
            PrintFunction( arg );
            PrintObjFull = 0;
        }
        else {
            PrintObj( arg );
        }
    }

    return 0;
}


/****************************************************************************
**
*F  FuncDownEnv( <<self>, level> )  . . . . . . . . .  change the environment
*/
UInt            ErrorLevel;

Obj             ErrorLVars0;    
Obj             ErrorLVars;
Int             ErrorLLevel;

extern  Obj     BottomLVars;


Obj FuncDownEnv (
    Obj                 self,
    Obj                 args )
{
    Int                 depth;

    if ( LEN_LIST(args) == 0 ) {
        depth = 1;
    }
    else if ( LEN_LIST(args) == 1 && IS_INTOBJ( ELM_PLIST(args,1) ) ) {
        depth = INT_INTOBJ( ELM_PLIST( args, 1 ) );
    }
    else {
        ErrorQuit( "usage: DownEnv( [ <depth> ] )", 0L, 0L );
        return 0;
    }

    /* if we really want to go up                                          */
    if ( depth < 0 && -ErrorLLevel <= -depth ) {
        depth = 0;
        ErrorLVars = ErrorLVars0;
        ErrorLLevel = 0;
    }
    else if ( depth < 0 ) {
        depth = -ErrorLLevel + depth;
        ErrorLVars = ErrorLVars0;
        ErrorLLevel = 0;
    }

    /* now go down                                                         */
    while ( 0 < depth
         && ErrorLVars != BottomLVars
         && PTR_BAG(ErrorLVars)[2] != BottomLVars ) {
        ErrorLVars = PTR_BAG(ErrorLVars)[2];
        ErrorLLevel--;
        depth--;
    }

    /* return nothing                                                      */
    return 0;
}


/****************************************************************************
**
*F  FuncWhere( <self>, <depth> )  . . . . . . . . . . . .  print stack frames
*/
Obj FuncWhere (
    Obj                 self,
    Obj                 args )
{
    Obj                 currLVars;
    Int                 depth;
    Expr                call;

#ifndef NO_BRK_CALLS

    /* evaluate the argument                                               */
    if ( LEN_LIST(args) == 0 ) {
        depth = 10;
    }
    else if ( LEN_LIST(args) == 1 && IS_INTOBJ( ELM_PLIST(args,1) ) ) {
        depth = INT_INTOBJ( ELM_PLIST( args, 1 ) );
    }
    else {
        ErrorQuit( "usage: Where( [ <depth> ] )", 0L, 0L );
        return 0;
    }

    currLVars = CurrLVars;

    if ( ErrorLVars != BottomLVars ) {
        SWITCH_TO_OLD_LVARS( ErrorLVars );
        SWITCH_TO_OLD_LVARS( BRK_CALL_FROM() );
        while ( CurrLVars != BottomLVars && 0 < depth ) {
            call = BRK_CALL_TO();
            if ( call == 0 ) {
                Pr( "<corrupted call value> ", 0L, 0L );
            }
            else if ( T_PROCCALL_0ARGS <= TYPE_STAT(call)
                   && TYPE_STAT(call)  <= T_PROCCALL_XARGS ) {
                PrintStat( call );
            }
            else if ( T_FUNCCALL_0ARGS <= TYPE_EXPR(call)
                   && TYPE_EXPR(call)  <= T_FUNCCALL_XARGS ) {
                PrintExpr( call );
            }
            Pr( " called from\n", 0L, 0L );
            SWITCH_TO_OLD_LVARS( BRK_CALL_FROM() );
            depth--;
        }
        if ( 0 < depth ) {
            Pr( "<function>( <arguments> ) called from read-eval-loop\n",
                0L, 0L );
        }
        else {
            Pr( "...\n", 0L, 0L );
        }
    }
    else {
        Pr( "not in any function\n", 0L, 0L );
    }

    SWITCH_TO_OLD_LVARS( currLVars );

#endif

    return 0;
}


/****************************************************************************
**
*F  ErrorMode( <msg>, <arg1>, <arg2>, <args>, <msg2>, <mode> )
*/
Obj ErrorMode (
    Char *              msg,
    Int                 arg1,
    Int                 arg2,
    Obj                 args,
    Char *              msg2,
    Char                mode )
{
    Obj                 errorLVars0;
    Obj                 errorLVars;
    UInt                errorLLevel;
    UInt                type;
    char                prompt [16];

    /* open the standard error output file                                 */
    OpenOutput( "*errout*" );
    ErrorLevel += 1;
    errorLVars0 = ErrorLVars0;
    ErrorLVars0 = CurrLVars;
    errorLVars  = ErrorLVars;
    ErrorLVars  = CurrLVars;
    errorLLevel = ErrorLLevel;
    ErrorLLevel = 0;

    /* print the error message                                             */
    if ( msg != (Char*)0 ) {
        Pr( msg, arg1, arg2 );
    }
    else if ( args != (Obj)0 ) {
        Pr( "Error ", 0L, 0L );
        FuncPrint( (Obj)0, args );
    }

    /* print the location                                                  */
    if ( CurrStat != 0 ) {
        Pr( " at\n", 0L, 0L );
        PrintStat( CurrStat );
        Pr( "\n", 0L, 0L );
    }
    else {
        Pr( "\n", 0L, 0L );
    }

    /* try to open input for a break loop                                  */
    if ( mode == 'q' || ! OpenInput( "*errin*") ) {
        ErrorLevel -= 1;
        ErrorLVars0 = errorLVars0;
        ErrorLVars = errorLVars;
        ErrorLLevel = errorLLevel;
        CloseOutput();
        ReadEvalError();
    }
    NrError = 0;

    /* print the sencond message                                           */
    Pr( "Entering break read-eval-print loop, ", 0L, 0L );
    Pr( "you can 'quit;' to quit to outer loop,\n", 0L, 0L );
    Pr( "or %s to continue\n", (Int)msg2, 0L );

    /* read-eval-print loop                                                */
    while ( 1 ) {

        /* read and evaluate one command                                   */
        if ( ErrorLevel == 1 ) {
            Prompt = "brk> ";
        }
        else {
            prompt[0] = 'b';
            prompt[1] = 'r';
            prompt[2] = 'k';
            prompt[3] = '_';
            prompt[4] = ErrorLevel / 10 + '0';
            prompt[5] = ErrorLevel % 10 + '0';
            prompt[6] = '>';
            prompt[7] = ' ';
            prompt[8] = '\0';
            Prompt = prompt;
        }

        /* read and evaluate one command                                   */
        NrError = 0;
        type = ReadEvalCommand();

        /* handle ordinary command                                         */
        if ( type == 0 && ReadEvalResult != 0 ) {

            /* remember the value in 'last'                                */
            AssGVar( Last,  ReadEvalResult   );

            /* print the result                                            */
            if ( *In != ';' ) {
                IsStringConv( ReadEvalResult );
                PrintObj( ReadEvalResult );
                Pr( "\n", 0L, 0L );
            }

        }

        /* handle return-value                                             */
        else if ( type == 1 ) {
            if ( mode == 'v' ) {
                ErrorLevel -= 1;
                ErrorLVars0 = errorLVars0;
                ErrorLVars = errorLVars;
                ErrorLLevel = errorLLevel;
                CloseInput();
                NrError = 0;
                CloseOutput();
                return ReadEvalResult;
            }
            else {
                Pr(
                    "'return <value>;' cannot be used in this break-loop",
                    0L, 0L );
            }
        }

        /* handle return-value                                             */
        else if ( type == 2 ) {
            if ( mode == 'x' ) {
                ErrorLevel -= 1;
                ErrorLVars0 = errorLVars0;
                ErrorLVars = errorLVars;
                ErrorLLevel = errorLLevel;
                CloseInput();
                NrError = 0;
                CloseOutput();
                return (Obj)0;
            }
            else {
                Pr(
                    "'return;' cannot be used in this break-loop",
                    0L, 0L );
            }
        }

        /* handle quit command or <end-of-file>                            */
        else if ( type == 8 || type == 16 ) {
            break;
        }

    }

    /* return to the outer read-eval-print loop                            */
    ErrorLevel -= 1;
    ErrorLVars0 = errorLVars0;
    ErrorLVars = errorLVars;
    ErrorLLevel = errorLLevel;
    CloseInput();
    NrError = 0;
    CloseOutput();
    ReadEvalError();

    /* this is just to please GNU cc, 'ReadEvalError' never returns        */
    return 0;
}


/****************************************************************************
**
*F  ErrorQuit( <msg>, <arg1>, <arg2> )  . . . . . . . . . . .  print and quit
*/
void            ErrorQuit (
    Char *              msg,
    Int                 arg1,
    Int                 arg2 )
{
    ErrorMode( msg, arg1, arg2, (Obj)0, (Char*)0, 'q' );
}


/****************************************************************************
**
*F  ErrorReturnObj( <msg>, <arg1>, <arg2>, <msg2> ) . .  print and return obj
*/
Obj ErrorReturnObj (
    Char *              msg,
    Int                 arg1,
    Int                 arg2,
    Char *              msg2 )
{
    return ErrorMode( msg, arg1, arg2, (Obj)0, msg2, 'v' );
}


/****************************************************************************
**
*F  ErrorReturnVoid( <msg>, <arg1>, <arg2>, <msg2> )  . . .  print and return
*/
void ErrorReturnVoid (
    Char *              msg,
    Int                 arg1,
    Int                 arg2,
    Char *              msg2 )
{
    ErrorMode( msg, arg1, arg2, (Obj)0, msg2, 'x' );
}


/****************************************************************************
**
*F  FuncError( <self>, <args> ) . . . . . . . . . . . . . . . signal an error
**
*/
Obj FuncError (
    Obj                 self,
    Obj                 args )
{
    return ErrorMode( (Char*)0, 0L, 0L, args, "you can return", 'x' );
}


/****************************************************************************
**

*F * * * * * * * * * functions for creating the init file * * * * * * * * * *
*/



/****************************************************************************
**

*F  Complete( <list> )  . . . . . . . . . . . . . . . . . . . complete a file
*/
Obj             CompNowFuncs;

UInt            CompNowCount;

void            Complete (
    Obj                 list )
{
    Obj                 filename;
    UInt                type;

    /* get and check the filename                                          */
    filename = ELM_PLIST( list, 1 );
    while ( ! IsStringConv( filename ) ) {
        filename = ErrorReturnObj(
            "COMPLETE: <filename> must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L,
            "you can return a string for <filename>" );
    }

    /* try to open the file                                                */
    if ( ! OpenInput( CSTR_STRING(filename) ) ) {
        return;
    }
    NrError = 0;

    /* we are now completing                                               */
    if ( SyDebugLoading ) {
        Pr( "#I  completing '%s'\n", (Int)CSTR_STRING(filename), 0L );
    }
    CompNowFuncs = list;
    CompNowCount = 1;

    /* now do the reading                                                  */
    while ( 1 ) {
        type = ReadEvalCommand();
        if ( type == 1 || type == 2 ) {
            Pr( "'return' must not be used in file read-eval loop",
                0L, 0L );
        }
        else if ( type == 8 || type == 16 ) {
            break;
        }
    }

    /* thats it for completing                                             */
    CompNowFuncs = 0;
    CompNowCount = 0;

    /* close the input file again, and return 'true'                       */
    if ( ! CloseInput() ) {
        ErrorQuit(
            "Panic: COMPLETE cannot close input, this should not happen",
            0L, 0L );
    }
    NrError = 0;
}


/****************************************************************************
**
*F  DoComplete<i>args( ... )  . . . . . . . . . .  handler to complete a file
*/
Obj             DoComplete0args (
    Obj                 self )
{
    Complete( BODY_FUNC( self ) );
    if ( HDLR_FUNC( self, 0 ) == DoComplete0args ) {
        ErrorQuit(
            "panic: completion did not define function",
            0L, 0L );
        return 0;
    }
    return CALL_0ARGS( self );
}

Obj             DoComplete1args (
    Obj                 self,
    Obj                 arg1 )
{
    Complete( BODY_FUNC( self ) );
    if ( HDLR_FUNC( self, 1 ) == DoComplete1args ) {
        ErrorQuit(
            "panic: completion did not define function",
            0L, 0L );
        return 0;
    }
    return CALL_1ARGS( self, arg1 );
}

Obj             DoComplete2args (
    Obj                 self,
    Obj                 arg1,
    Obj                 arg2 )
{
    Complete( BODY_FUNC( self ) );
    if ( HDLR_FUNC( self, 2 ) == DoComplete2args ) {
        ErrorQuit(
            "panic: completion did not define function",
            0L, 0L );
        return 0;
    }
    return CALL_2ARGS( self, arg1, arg2 );
}

Obj             DoComplete3args (
    Obj                 self,
    Obj                 arg1,
    Obj                 arg2,
    Obj                 arg3 )
{
    Complete( BODY_FUNC( self ) );
    if ( HDLR_FUNC( self, 3 ) == DoComplete3args ) {
        ErrorQuit(
            "panic: completion did not define function",
            0L, 0L );
        return 0;
    }
    return CALL_3ARGS( self, arg1, arg2, arg3 );
}

Obj             DoComplete4args (
    Obj                 self,
    Obj                 arg1,
    Obj                 arg2,
    Obj                 arg3,
    Obj                 arg4 )
{
    Complete( BODY_FUNC( self ) );
    if ( HDLR_FUNC( self, 4 ) == DoComplete4args ) {
        ErrorQuit(
            "panic: completion did not define function",
            0L, 0L );
        return 0;
    }
    return CALL_4ARGS( self, arg1, arg2, arg3, arg4 );
}

Obj             DoComplete5args (
    Obj                 self,
    Obj                 arg1,
    Obj                 arg2,
    Obj                 arg3,
    Obj                 arg4,
    Obj                 arg5 )
{
    Complete( BODY_FUNC( self ) );
    if ( HDLR_FUNC( self, 5 ) == DoComplete5args ) {
        ErrorQuit(
            "panic: completion did not define function",
            0L, 0L );
        return 0;
    }
    return CALL_5ARGS( self, arg1, arg2, arg3, arg4, arg5 );
}

Obj             DoComplete6args (
    Obj                 self,
    Obj                 arg1,
    Obj                 arg2,
    Obj                 arg3,
    Obj                 arg4,
    Obj                 arg5,
    Obj                 arg6 )
{
    Complete( BODY_FUNC( self ) );
    if ( HDLR_FUNC( self, 6 ) == DoComplete6args ) {
        ErrorQuit(
            "panic: completion did not define function",
            0L, 0L );
        return 0;
    }
    return CALL_6ARGS( self, arg1, arg2, arg3, arg4, arg5, arg6 );
}

Obj             DoCompleteXargs (
    Obj                 self,
    Obj                 args )
{
    Complete( BODY_FUNC( self ) );
    if ( HDLR_FUNC( self, 7 ) == DoCompleteXargs ) {
        ErrorQuit(
            "panic: completion did not define function",
            0L, 0L );
        return 0;
    }
    return CALL_XARGS( self, args );
}


/****************************************************************************
**
*F  FuncCOM_FILE( <self>, <filename>, <crc> ) . . . . . . . . .  set filename
*/
Obj  CompLists;
Obj  CompThenFuncs;


Obj FuncCOM_FILE (
    Obj                 self,
    Obj                 filename,
    Obj                 crc )
{
    Int                 len;
    StructCompInitInfo* info;
    UInt4               crc1;
    Char                result[256];
    Int                 res;
    Obj                 func;


    /* check the argument                                                  */
    while ( ! IsStringConv(filename) ) {
        filename = ErrorReturnObj(
            "<filename> must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L,
            "you can return a string for <filename>" );
    }
    while ( TYPE_OBJ(crc)!=T_INTPOS && !IS_INTOBJ(crc) ) {
        crc = ErrorReturnObj(
            "<crc> must be an positive integer (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(crc)].name), 0L,
            "you can return an integer or 'false' for <crc>" );
    }

    /* check if have a statically or dynamically loadable module           */
    crc1 = INT_INTOBJ( QuoInt( crc, INTOBJ_INT( 1 << 16 ) ) );
    crc1 = crc1 << 16;
    crc1 = crc1 + INT_INTOBJ( RemInt( crc, INTOBJ_INT( 1 << 16 ) ) );
    res = SyFindOrLinkGapRootFile(CSTR_STRING(filename), crc1, result, 256);

    /* not found                                                           */
    if ( res == 0 ) {
        ErrorQuit( "cannot find module or file '%s'", 
                   (Int)CSTR_STRING(filename), 0L );
        return 0;
    }

    /* dynamically linked                                                  */
    else if ( res == 1 ) {
        if ( SyDebugLoading ) {
            Pr( "#I  READ_GAP_ROOT: loading '%s' dynamically\n",
                (Int)CSTR_STRING(filename), 0L );
        }
        info = *(StructCompInitInfo**)result;
        (info->link)();
        func = (Obj)(info->function1)();
        CALL_0ARGS(func);
        return INTOBJ_INT(1);
    }

    /* statically linked                                                   */
    else if ( res == 2 ) {
        if ( SyDebugLoading ) {
            Pr( "#I  READ_GAP_ROOT: loading '%s' statically\n",
                (Int)CSTR_STRING(filename), 0L );
        }
        info = *(StructCompInitInfo**)result;
        (info->link)();
        func = (Obj)(info->function1)();
        CALL_0ARGS(func);
        return INTOBJ_INT(2);
    }


    /* we have to read the GAP file                                        */
    else if ( res == 3 ) {
        filename = NEW_STRING( SyStrlen(result) );
        SyStrncat( CSTR_STRING(filename), result, SyStrlen(result) );

        CompThenFuncs = NEW_PLIST( T_PLIST, 1 );
        SET_LEN_PLIST( CompThenFuncs, 1 );
        SET_ELM_PLIST( CompThenFuncs, 1, filename );

        len = LEN_PLIST( CompLists );
        GROW_PLIST(    CompLists, len+1 );
        SET_LEN_PLIST( CompLists, len+1 );
        SET_ELM_PLIST( CompLists, len+1, CompThenFuncs );
        CHANGED_BAG(   CompLists );

        return INTOBJ_INT(3);
    }

    /* don't know                                                          */
    else {
        ErrorQuit( "unknown result code %d from 'SyFindGapRoot'", res, 0L );
        return 0;
    }
}


/****************************************************************************
**
*F  FuncCOM_FUN( <self>, <num> )  . . . . . . . . make a completable function
*/
Obj FuncCOM_FUN (
    Obj                 self,
    Obj                 num )
{
    Obj                 func;
    Int                 n;

    /* if the file is not yet completed then make a new function           */
    n = INT_INTOBJ(num) + 1;
    if ( LEN_PLIST( CompThenFuncs ) <= n ) {
       
        /* make the function                                               */
        func = NewBag( T_FUNCTION, SIZE_FUNC );
        HDLR_FUNC( func, 0 ) = DoComplete0args;
        HDLR_FUNC( func, 1 ) = DoComplete1args;
        HDLR_FUNC( func, 2 ) = DoComplete2args;
        HDLR_FUNC( func, 3 ) = DoComplete3args;
        HDLR_FUNC( func, 4 ) = DoComplete4args;
        HDLR_FUNC( func, 5 ) = DoComplete5args;
        HDLR_FUNC( func, 6 ) = DoComplete6args;
        HDLR_FUNC( func, 7 ) = DoCompleteXargs;
        BODY_FUNC( func )    = CompThenFuncs;

        /* add the function to the list of functions to complete           */
        GROW_PLIST(    CompThenFuncs, n );
        SET_LEN_PLIST( CompThenFuncs, n );
        SET_ELM_PLIST( CompThenFuncs, n, func );
        CHANGED_BAG(   CompThenFuncs );

    }

    /* return the function                                                 */
    return ELM_PLIST( CompThenFuncs, n );
}


/****************************************************************************
**
*F  CompList( <name> )  . . . . . . . . . . . . . . . . find list with <name>
*/
Obj CompList (
    Char *              name )
{
    UInt                i;
    Obj                 list;
    Obj                 string;

    for ( i = 1; i <= LEN_LIST(CompLists); i++ ) {
        list = ELM_LIST( CompLists, i );
        string = ELM_LIST( list, 1 );
        if ( ! SyStrcmp( name, CSTR_STRING(string) ) ) {
            return list;
        }
    }
    return (Obj)0;
}


/****************************************************************************
**
*F  FuncMAKE_INIT( <out>, <in>, ... ) . . . . . . . . . .  generate init file
*/
#define MAKE_INIT_GET_SYMBOL                    \
    do {                                        \
        symbol = Symbol;                        \
        value[0] = '\0';                        \
        SyStrncat( value, Value, 1023 );        \
        if ( Symbol != S_EOF )  GetSymbol();    \
    } while (0)


Obj FuncMAKE_INIT (
    Obj                 self,
    Obj                 args )
{
    Int                 i;
    Obj                 filename;
    UInt                level;
    UInt                symbol;
    Char                value [1024];
    UInt                funcNum;
    UInt4               crc;
    Obj                 crc1;

    /* check the argument                                                  */
    if ( LEN_PLIST(args) < 2 ) {
        ErrorQuit( "usage: MAKE_INIT( <oot>, <in1>, <in2>, ... )", 0L, 0L );
    }
    for ( i = 1;  i <= LEN_PLIST(args);  i++ ) {
        filename = ELM_PLIST( args, i );
        if ( ! IsStringConv( filename ) ) {
            ErrorQuit( "%d.th argument must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L );
        }
    }

    /* try to open the output                                              */
    if ( ! OpenOutput( CSTR_STRING( ELM_PLIST( args, 1 ) ) ) ) {
        ErrorQuit( "cannot open '%s' for output",
                   (Int)CSTR_STRING(ELM_PLIST(args,1)), 0L );
    }

    /* loop over all remaining arguments                                   */
    for ( i = 2;  i <= LEN_PLIST(args);  i++ ) {
        filename = ELM_PLIST( args, i );

        /* try to open the file                                            */
        if ( ! OpenInput( CSTR_STRING(filename) ) ) {
	    CloseOutput();
            ErrorQuit( "'%s' must exist and be readable",
                       (Int)CSTR_STRING(filename), 0L );
        }
        NrError = 0;

        /* create a crc value                                              */
        crc = SyGAPCRC( CSTR_STRING(filename) );
        crc1 = INTOBJ_INT( crc >> 16 );
        crc1 = PROD( INTOBJ_INT(1<<16), crc1 );
        crc1 = SUM( crc1, INTOBJ_INT( crc & 0xFFFFL ) );

        /* where is this stuff                                             */
        Pr( "if COM_FILE( \"%S\", ", (Int)CSTR_STRING(filename), 0L );
        PrintInt( crc1 );
        Pr( " ) = 3  then\n", 0L, 0L );
        funcNum = 1;

        /* read the file                                                   */
        GetSymbol();
        MAKE_INIT_GET_SYMBOL;
        while ( symbol != S_EOF ) {

            /* handle function beginning and ending                        */
            if ( symbol == S_FUNCTION ) {
                Pr( "COM_FUN(%d)", funcNum++, 0L );
                MAKE_INIT_GET_SYMBOL;
                level = 0;
                while ( level != 0
                     || symbol != S_END )
                {
                    if ( symbol == S_FUNCTION )
                        level++;
                    if ( symbol == S_END )
                        level--;
                    MAKE_INIT_GET_SYMBOL;
                }
                MAKE_INIT_GET_SYMBOL;
            }

            /* handle -> expressions                                       */
            else if ( symbol == S_IDENT && Symbol == S_MAPTO ) {
                Pr( "COM_FUN(%d)", funcNum++, 0L );
                symbol = Symbol;  if ( Symbol != S_EOF )  GetSymbol();
                MAKE_INIT_GET_SYMBOL;
                level = 0;
                while ( level != 0
                   || (symbol != S_RBRACK  && symbol != S_RBRACE
                    && symbol != S_RPAREN  && symbol != S_COMMA
                    && symbol != S_DOTDOT  && symbol != S_SEMICOLON) )
                {
                    if ( symbol == S_LBRACK  || symbol == S_LBRACE
                      || symbol == S_LPAREN  || symbol == S_FUNCTION 
                      || symbol == S_BLBRACK || symbol == S_BLBRACE )
                        level++;
                    if ( symbol == S_RBRACK  || symbol == S_RBRACE
                      || symbol == S_RPAREN  || symbol == S_END )
                        level--;
                    MAKE_INIT_GET_SYMBOL;
                }
            }
        
            /* handle the other symbols                                    */
            else {

                switch ( symbol ) {

                case S_IDENT:    Pr( "%I",      (Int)value, 0L );  break;
                case S_UNBIND:   Pr( "Unbind",  0L, 0L );  break;
                case S_ISBOUND:  Pr( "IsBound", 0L, 0L );  break;

                case S_LBRACK:   Pr( "[",       0L, 0L );  break;
                case S_RBRACK:   Pr( "]",       0L, 0L );  break;
                case S_LBRACE:   Pr( "{",       0L, 0L );  break;
                case S_RBRACE:   Pr( "}",       0L, 0L );  break;
                case S_DOT:      Pr( ".",       0L, 0L );  break;
                case S_LPAREN:   Pr( "(",       0L, 0L );  break;
                case S_RPAREN:   Pr( ")",       0L, 0L );  break;
                case S_COMMA:    Pr( ",",       0L, 0L );  break;
                case S_DOTDOT:   Pr( "..",      0L, 0L );  break;

                case S_BDOT:     Pr( "!.",      0L, 0L );  break;
                case S_BLBRACK:  Pr( "![",      0L, 0L );  break;
                case S_BLBRACE:  Pr( "!{",      0L, 0L );  break;

                case S_INT:      Pr( "%s",      (Int)value, 0L );  break;
                case S_TRUE:     Pr( "true",    0L, 0L );  break;
                case S_FALSE:    Pr( "false",   0L, 0L );  break;
                case S_CHAR:     Pr( "'%c'",    (Int)value[0], 0L );  break;
                case S_STRING:   Pr( "\"%S\"",  (Int)value, 0L );  break;

                case S_REC:      Pr( "rec",     0L, 0L );  break;

                case S_FUNCTION: /* handled above */       break;
                case S_LOCAL:    /* shouldn't happen */    break;
                case S_END:      /* handled above */       break;
                case S_MAPTO:    /* handled above */       break;

                case S_MULT:     Pr( "*",       0L, 0L );  break;
                case S_DIV:      Pr( "/",       0L, 0L );  break;
                case S_MOD:      Pr( " mod ",   0L, 0L );  break;
                case S_POW:      Pr( "^",       0L, 0L );  break;

                case S_PLUS:     Pr( "+",       0L, 0L );  break;
                case S_MINUS:    Pr( "-",       0L, 0L );  break;

                case S_EQ:       Pr( "=",       0L, 0L );  break;
                case S_LT:       Pr( "<",       0L, 0L );  break;
                case S_GT:       Pr( ">",       0L, 0L );  break;
                case S_NE:       Pr( "<>",      0L, 0L );  break;
                case S_LE:       Pr( "<=",      0L, 0L );  break;
                case S_GE:       Pr( ">=",      0L, 0L );  break;
                case S_IN:       Pr( " in ",    0L, 0L );  break;

                case S_NOT:      Pr( "not ",    0L, 0L );  break;
                case S_AND:      Pr( " and ",   0L, 0L );  break;
                case S_OR:       Pr( " or ",    0L, 0L );  break;

                case S_ASSIGN:   Pr( ":=",      0L, 0L );  break;

                case S_IF:       Pr( "if ",     0L, 0L );  break;
                case S_FOR:      Pr( "for ",    0L, 0L );  break;
                case S_WHILE:    Pr( "while ",  0L, 0L );  break;
                case S_REPEAT:   Pr( "repeat ", 0L, 0L );  break;

                case S_THEN:     Pr( " then\n", 0L, 0L );  break;
                case S_ELIF:     Pr( "elif ",   0L, 0L );  break;
                case S_ELSE:     Pr( "else\n",  0L, 0L );  break;
                case S_FI:       Pr( "fi",      0L, 0L );  break;
                case S_DO:       Pr( " do\n",   0L, 0L );  break;
                case S_OD:       Pr( "od",      0L, 0L );  break;
                case S_UNTIL:    Pr( "until ",  0L, 0L );  break;

                case S_BREAK:    Pr( "break",   0L, 0L );  break;
                case S_RETURN:   Pr( "return ", 0L, 0L );  break;
                case S_QUIT:     Pr( "quit",    0L, 0L );  break;

                case S_SEMICOLON: Pr( ";\n",    0L, 0L );  break;

                default: CloseInput();
                         CloseOutput();
                         NrError = 0;
                         ErrorQuit( "unknown symbol %d", (Int)symbol, 0L );

                }

                /* get the next symbol                                     */
                MAKE_INIT_GET_SYMBOL;
            }

        }

        /* close the input file again                                      */
        Pr( "fi;\n\n", 0L, 0L );
        if ( ! CloseInput() ) {
            ErrorQuit(
              "Panic: MAKE_INIT cannot close input, this should not happen",
              0L, 0L );
        }
        NrError = 0;
    }
    CloseOutput();

    return 0;
}


/****************************************************************************
**

*F * * * * * * * * * functions for dynamical/static modules * * * * * * * * *
*/



/****************************************************************************
**

*F  FuncGAP_CRC( <self>, <name> ) . . . . . . . create a crc value for a file
*/
Obj FuncGAP_CRC (
    Obj                 self,
    Obj                 filename )
{
    UInt4               crc;
    Obj                 crc1;

    /* check the argument                                                  */
    while ( ! IsStringConv( filename ) ) {
        filename = ErrorReturnObj(
            "<filename> must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L,
            "you can return a string for <filename>" );
    }

    /* compute the crc value                                               */
    crc  = SyGAPCRC( CSTR_STRING(filename) );
    crc1 = INTOBJ_INT( crc >> 16 );
    crc1 = PROD( INTOBJ_INT(1<<16), crc1 );
    return SUM( crc1, INTOBJ_INT( crc & 0xFFFFL ) );
}


/****************************************************************************
**
*F  FuncLOAD_DYN( <self>, <name>, <crc> ) . . .  try to load a dynamic module
*/
Obj FuncLOAD_DYN (
    Obj                 self,
    Obj                 filename,
    Obj                 crc )
{
    CompInitFunc        init;
    StructCompInitInfo* info;
    Obj                 crc1;
    Obj                 func;

    /* check the argument                                                  */
    while ( ! IsStringConv( filename ) ) {
        filename = ErrorReturnObj(
            "<filename> must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L,
            "you can return a string for <filename>" );
    }
    while ( TYPE_OBJ(crc)!=T_INTPOS && !IS_INTOBJ(crc) && crc!=False ) {
        crc = ErrorReturnObj(
            "<crc> must be an integer or 'false' (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(crc)].name), 0L,
            "you can return an integer or 'false' for <crc>" );
    }

    /* try to read the module                                              */
    init = SyLoadModule( CSTR_STRING(filename) );
    if ( (Int)init == 1 )
        ErrorQuit( "module '%s' not found", (Int)CSTR_STRING(filename), 0L );
    else if ( (Int) init == 3 )
        ErrorQuit( "symbol 'Init_Dynamic' not found", 0L, 0L );
    else if ( (Int) init == 5 )
        ErrorQuit( "forget symbol failed", 0L, 0L );

    /* no dynamic library support                                          */
    else if ( (Int) init == 7 ) {
        if ( SyDebugLoading ) {
            Pr( "#I  LOAD_DYN: no support for dynamical loading\n", 0L, 0L );
        }
        return False; 
    }

    /* get the description structure                                       */
    info = (*init)();
    if ( info == 0 )
        ErrorQuit( "call to init function failed", 0L, 0L );

    /* check the crc value                                                 */
    if ( crc != False ) {
        crc1 = INTOBJ_INT( info->magic1 >> 16 );
        crc1 = PROD( INTOBJ_INT(1<<16), crc1 );
        crc1 = SUM( crc1, INTOBJ_INT( info->magic1 & 0xFFFFL ) );
        if ( ! EQ( crc, crc1 ) ) {
            if ( SyDebugLoading ) {
                Pr( "#I  LOAD_DYN: crc values do not match, gap ", 0L, 0L );
                PrintInt( crc );
                Pr( ", dyn ", 0L, 0L );
                PrintInt( crc1 );
                Pr( "\n", 0L, 0L );
            }
            return False;
        }
    }

    /* link and init me                                                    */
    (info->link)();
    func = (Obj)(info->function1)();
    CALL_0ARGS(func);

    return True;
}


/****************************************************************************
**
*F  FuncLOAD_STAT( <self>, <name>, <crc> )  . . . . try to load static module
*/
Obj FuncLOAD_STAT (
    Obj                 self,
    Obj                 filename,
    Obj                 crc )
{
    StructCompInitInfo* info;
    Obj                 crc1;
    Int                 k;
    Obj			func;

    /* check the argument                                                  */
    while ( ! IsStringConv( filename ) ) {
        filename = ErrorReturnObj(
            "<filename> must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L,
            "you can return a string for <filename>" );
    }
    while ( TYPE_OBJ(crc)!=T_INTPOS && !IS_INTOBJ(crc) && crc!=False ) {
        crc = ErrorReturnObj(
            "<crc> must be an integer or 'false' (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(crc)].name), 0L,
            "you can return an integer or 'false' for <crc>" );
    }

    /* try to find the module                                              */
    for ( k = 0;  CompInitFuncs[k];  k++ ) {
        info = (*(CompInitFuncs[k]))();
        if ( info == 0 ) {
            continue;
        }
        if ( ! SyStrcmp( CSTR_STRING(filename), info->magic2 ) ) {
            break;
        }
    }
    if ( CompInitFuncs[k] == 0 ) {
        if ( SyDebugLoading ) {
            Pr( "#I  LOAD_STAT: no module named '%s' found\n",
                (Int)CSTR_STRING(filename), 0L );
        }
        return False;
    }

    /* check the crc value                                                 */
    if ( crc != False ) {
        crc1 = INTOBJ_INT( info->magic1 >> 16 );
        crc1 = PROD( INTOBJ_INT(1<<16), crc1 );
        crc1 = SUM( crc1, INTOBJ_INT( info->magic1 & 0xFFFFL ) );
        if ( ! EQ( crc, crc1 ) ) {
            if ( SyDebugLoading ) {
                Pr( "#I  LOAD_STAT: crc values do not match, gap ", 0L, 0L );
                PrintInt( crc );
                Pr( ", stat ", 0L, 0L );
                PrintInt( crc1 );
                Pr( "\n", 0L, 0L );
            }
            return False;
        }
    }

    /* link and init me                                                    */
    (info->link)();
    func = (Obj)(info->function1)();
    CALL_0ARGS(func);

    return True;
}


/****************************************************************************
**

*F * * * * * * * * * streams and files related functions  * * * * * * * * * *
*/


/****************************************************************************
**

*F  FuncREAD( <filename> )  . . . . . . . . . . . . . . . . . . . read a file
*/
Obj FuncREAD (
    Obj                 self,
    Obj                 filename )
{
    UInt                type;

    /* check the argument                                                  */
    while ( ! IsStringConv( filename ) ) {
        filename = ErrorReturnObj(
            "READ: <filename> must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L,
            "you can return a string for <filename>" );
    }

    /* try to open the file                                                */
    if ( ! OpenInput( CSTR_STRING(filename) ) ) {
        return False;
    }
    NrError = 0;

    /* now do the reading                                                  */
    while ( 1 ) {
        type = ReadEvalCommand();

        /* handle return-value or return-void command                      */
        if ( type == 1 || type == 2 ) {
            Pr(
                "'return' must not be used in file read-eval loop",
                0L, 0L );
        }

        /* handle quit command or <end-of-file>                            */
        else if ( type == 8 || type == 16 ) {
            break;
        }

    }

    /* close the input file again, and return 'true'                       */
    if ( ! CloseInput() ) {
        ErrorQuit(
            "Panic: READ cannot close input, this should not happen",
            0L, 0L );
    }
    NrError = 0;
    return True;
}


/****************************************************************************
**
*F  FuncREAD_AS_FUNC( <filename> )  . . . . . . . . . . . . . . . read a file
*/
Obj READ_AS_FUNC (
    Char *              filename )
{
    Obj                 func;
    UInt                type;

    /* try to open the file                                                */
    if ( ! OpenInput( filename ) ) {
        return Fail;
    }
    NrError = 0;

    /* now do the reading                                                  */
    type = ReadEvalFile();

    /* get the function                                                    */
    if ( type == 0 ) {
        func = ReadEvalResult;
    }
    else {
        func = Fail;
    }

    /* close the input file again, and return 'true'                       */
    if ( ! CloseInput() ) {
        ErrorQuit(
            "Panic: READ_AS_FUNC cannot close input, this should not happen",
            0L, 0L );
    }
    NrError = 0;

    /* return the function                                                 */
    return func;
}

Obj             FuncREAD_AS_FUNC (
    Obj                 self,
    Obj                 filename )
{
    /* check the argument                                                  */
    while ( ! IsStringConv( filename ) ) {
        filename = ErrorReturnObj(
            "READ_AS_FUNC: <filename> must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L,
            "you can return a string for <filename>" );
    }

    /* read the function                                                   */
    return READ_AS_FUNC( CSTR_STRING(filename) );
}


/****************************************************************************
**
*F  READ_GAP_ROOT( <filename> ) . . .  read from gap root, dyn-load or static
**
**  'READ_GAP_ROOT' tries to find  a file under  the root directory,  it will
**  search all   directories given   in 'SyGapRootPaths',  check  dynamically
**  loadable modules and statically linked modules.
*/
Int READ_GAP_ROOT ( Char * filename )
{
    Char                result[256];
    Int                 res;
    UInt                type;
    StructCompInitInfo* info;
    Obj                 func;
    UInt4               crc;
    Char *              file;

    /* try to find the file                                                */
    file = SyFindGapRootFile(filename);
    if ( file ) {
	crc = SyGAPCRC(file);
    }
    else {
	crc = 0;
    }
    res = SyFindOrLinkGapRootFile( filename, crc, result, 256 );

    /* not found                                                           */
    if ( res == 0 ) {
        return 0;
    }

    /* dynamically linked                                                  */
    else if ( res == 1 ) {
        if ( SyDebugLoading ) {
            Pr( "#I  READ_GAP_ROOT: loading '%s' dynamically\n",
                (Int)filename, 0L );
        }
        info = *(StructCompInitInfo**)result;
        (info->link)();
        func = (Obj)(info->function1)();
        CALL_0ARGS(func);
        return 1;
    }

    /* statically linked                                                   */
    else if ( res == 2 ) {
        if ( SyDebugLoading ) {
            Pr( "#I  READ_GAP_ROOT: loading '%s' statically\n",
                (Int)filename, 0L );
        }
        info = *(StructCompInitInfo**)result;
        (info->link)();
        func = (Obj)(info->function1)();
        CALL_0ARGS(func);
        return 1;
    }

    /* ordinary gap file                                                   */
    else if ( res == 3 ) {
        if ( SyDebugLoading ) {
            Pr( "#I  READ_GAP_ROOT: loading '%s' as GAP file\n",
                (Int)filename, 0L );
        }
        if ( OpenInput(result) ) {
            NrError = 0;
            while ( 1 ) {
                type = ReadEvalCommand();
                if ( type == 1 || type == 2 ) {
                    Pr( "'return' must not be used in file", 0L, 0L );
                }
                else if ( type == 8 || type == 16 ) {
                    break;
                }
            }
            CloseInput();
            NrError = 0;
            return 1;
        }
        else {
            return 0;
        }
    }

    /* don't know                                                          */
    else {
        ErrorQuit( "unknown result code %d from 'SyFindGapRoot'", res, 0L );
        return 0;
    }
}


/****************************************************************************
**
*F  FuncREAD_GAP_ROOT( <filename> ) . . . . . . . . . . . . . . . read a file
*/
Obj FuncREAD_GAP_ROOT (
    Obj                 self,
    Obj                 filename )
{
    /* check the argument                                                  */
    while ( ! IsStringConv( filename ) ) {
        filename = ErrorReturnObj(
            "READ: <filename> must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L,
            "you can return a string for <filename>" );
    }

    /* try to open the file                                                */
    if ( READ_GAP_ROOT( CSTR_STRING(filename) ) ) {
        return True;
    }
    else {
        return False;
    }
}


/****************************************************************************
**
*F  FuncReadTest( <filename> )  . . . . . . . . . . . . . .  read a test file
*/
Obj FuncReadTest (
    Obj                 self,
    Obj                 filename )
{
    UInt                type;
    UInt                time;

    /* check the argument                                                  */
    while ( ! IsStringConv( filename ) ) {
        filename = ErrorReturnObj(
            "ReadTest: <filename> must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L,
            "you can return a string for <filename>" );
    }

    /* try to open the file                                                */
    if ( ! OpenTest( CSTR_STRING(filename) ) ) {
        return False;
    }
    NrError = 0;

    /* get the starting time                                               */
    time = SyTime();

    /* now do the reading                                                  */
    while ( 1 ) {

        /* read and evaluate the command                                   */
        type = ReadEvalCommand();

        /* stop the stopwatch                                              */
        AssGVar( Time, INTOBJ_INT( SyTime() - time ) );

        /* handle ordinary command                                         */
        if ( type == 0 && ReadEvalResult != 0 ) {

            /* print the result                                            */
            if ( *In != ';' ) {
                IsStringConv( ReadEvalResult );
                PrintObj( ReadEvalResult );
                Pr( "\n", 0L, 0L );
            }
            else {
                Match( S_SEMICOLON, ";", 0UL );
            }

        }

        /* handle return-value or return-void command                      */
        else if ( type == 1 || type == 2 ) {
            Pr(
                "'return' must not be used in file read-eval loop",
                0L, 0L );
        }

        /* handle quit command or <end-of-file>                            */
        else if ( type == 8 || type == 16 ) {
            break;
        }

    }

    /* close the input file again, and return 'true'                       */
    if ( ! CloseTest() ) {
        ErrorQuit(
            "Panic: ReadTest cannot close input, this should not happen",
            0L, 0L );
    }
    NrError = 0;
    return True;
}


/****************************************************************************
**
*F  FuncLogTo( <filename> ) . . . . . . . . . . . . internal function 'LogTo'
**
**  'FunLogTo' implements the internal function 'LogTo'.
**
**  'LogTo( <filename> )' \\
**  'LogTo()'
**
**  'LogTo' instructs GAP to echo all input from the  standard  input  files,
**  '*stdin*' and '*errin*' and all output  to  the  standard  output  files,
**  '*stdout*'  and  '*errout*',  to  the  file  with  the  name  <filename>.
**  The file is created if it does not  exist,  otherwise  it  is  truncated.
**
**  'LogTo' called with no argument closes the current logfile again, so that
**  input   from  '*stdin*'  and  '*errin*'  and  output  to  '*stdout*'  and
**  '*errout*' will no longer be echoed to a file.
*/
Obj FuncLogTo (
    Obj                 self,
    Obj                 args )
{
    Obj                 filename;

    /* 'LogTo()'                                                           */
    if ( LEN_LIST(args) == 0 ) {
        if ( ! CloseLog() ) {
            ErrorQuit("LogTo: can not close the logfile",0L,0L);
            return 0;
        }
    }

    /* 'LogTo( <filename> )'                                               */
    else if ( LEN_LIST(args) == 1 ) {
        filename = ELM_LIST(args,1);
        while ( ! IsStringConv(filename) ) {
            filename = ErrorReturnObj(
                "LogTo: <filename> must be a string (not a %s)",
                (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L,
                "you can return a string for <filename>" );
        }
        if ( ! OpenLog( CSTR_STRING(filename) ) ) {
            ErrorReturnVoid(
                "LogTo: cannot log to %s",
                (Int)CSTR_STRING(filename), 0L,
                "you can return" );
            return 0;
        }
    }

    return 0;
}


/****************************************************************************
**
*F  FuncIsExistingFile( <self>, <name> )  . . . . . . does file <name> exists
*/
Obj FuncIsExistingFile (
            Obj             self,
            Obj             filename )
{
    /* check the argument                                                  */
    while ( ! IsStringConv( filename ) ) {
        filename = ErrorReturnObj(
            "<filename> must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L,
            "you can return a string for <filename>" );
    }
    
    /* call the system dependent function                                  */
    return SyIsExistingFile( CSTR_STRING(filename) ) ? True : False;
}


/****************************************************************************
**
*F  FuncIsReadableFile( <self>, <name> )  . . . . . . is file <name> readable
*/
Obj FuncIsReadableFile (
            Obj             self,
            Obj             filename )
{
    /* check the argument                                                  */
    while ( ! IsStringConv( filename ) ) {
        filename = ErrorReturnObj(
            "<filename> must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L,
            "you can return a string for <filename>" );
    }
    
    /* call the system dependent function                                  */
    return SyIsReadableFile( CSTR_STRING(filename) ) ? True : False;
}


/****************************************************************************
**
*F  FuncIsWritableFile( <self>, <name> )  . . . . . . is file <name> writable
*/
Obj FuncIsWritableFile (
            Obj             self,
            Obj             filename )
{
    /* check the argument                                                  */
    while ( ! IsStringConv( filename ) ) {
        filename = ErrorReturnObj(
            "<filename> must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L,
            "you can return a string for <filename>" );
    }
    
    /* call the system dependent function                                  */
    return SyIsWritableFile( CSTR_STRING(filename) ) ? True : False;
}


/****************************************************************************
**
*F  FuncIsExecutableFile( <self>, <name> )  . . . . is file <name> executable
*/
Obj FuncIsExecutableFile (
            Obj             self,
            Obj             filename )
{
    /* check the argument                                                  */
    while ( ! IsStringConv( filename ) ) {
        filename = ErrorReturnObj(
            "<filename> must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(filename)].name), 0L,
            "you can return a string for <filename>" );
    }
    
    /* call the system dependent function                                  */
    return SyIsExecutableFile( CSTR_STRING(filename) ) ? True : False;
}


/****************************************************************************
**


*F * * * * * * * * * * * * * * debug functions  * * * * * * * * * * * * * * *
*/

/****************************************************************************
**

*F  FuncGASMAN( <self>, <args> )  . . . . . . . . .  expert function 'GASMAN'
**
**  'FuncGASMAN' implements the internal function 'GASMAN'
**
**  'GASMAN( "display" | "clear" | "collect" | "message" | "partial" )'
*/
Obj             FuncGASMAN (
    Obj                 self,
    Obj                 args )
{
    Obj                 cmd;            /* argument                        */
    UInt                i,  k;          /* loop variables                  */

    /* check the argument                                                  */
    while ( ! IS_LIST(args) || LEN_LIST(args) == 0 ) {
        args = ErrorReturnObj(
            "usage: GASMAN( \"display\"|\"clear\"|\"collect\"|\"message\" )",
            0L, 0L,
            "you can return a list of arguments" );
    }

    /* loop over the arguments                                             */
    for ( i = 1; i <= LEN_LIST(args); i++ ) {

        /* evaluate and check the command                                  */
        cmd = ELM_PLIST( args, i );
again:
        while ( ! IsStringConv(cmd) ) {
           cmd = ErrorReturnObj(
               "GASMAN: <cmd> must be a string (not a %s)",
               (Int)(InfoBags[TYPE_OBJ(cmd)].name), 0L,
               "you can return a string for <cmd>" );
       }

        /* if request display the statistics                               */
        if ( SyStrcmp( CSTR_STRING(cmd), "display" ) == 0 ) {
            Pr( "\t\t%30s  ", (Int)"type",  0L          );
            Pr( "%8d %8d  ",  (Int)"alive", (Int)"size" );
            Pr( "%8d %8d\n",  (Int)"total", (Int)"size" );
            for ( k = 0; k < 256; k++ ) {
                if ( InfoBags[k].name != 0 ) {
                    Pr("%30s  ",   (Int)InfoBags[k].name, 0L );
                    Pr("%8d %8d  ",(Int)InfoBags[k].nrLive,
                                   (Int)InfoBags[k].sizeLive);
                    Pr("%8d %8d\n",(Int)InfoBags[k].nrAll,
                                   (Int)InfoBags[k].sizeAll);
                }
            }
        }

        /* if request display the statistics                               */
        else if ( SyStrcmp( CSTR_STRING(cmd), "clear" ) == 0 ) {
            for ( k = 0; k < 256; k++ ) {
                InfoBags[k].nrAll   = InfoBags[k].nrLive;
                InfoBags[k].sizeAll = InfoBags[k].sizeLive;
            }
        }

        /* or collect the garbage                                          */
        else if ( SyStrcmp( CSTR_STRING(cmd), "collect" ) == 0 ) {
            CollectBags(0,1);
        }

        /* or collect the garbage                                          */
        else if ( SyStrcmp( CSTR_STRING(cmd), "partial" ) == 0 ) {
            CollectBags(0,0);
        }

        /* or finally toggle Gasman messages                               */
        else if ( SyStrcmp( CSTR_STRING(cmd), "message" ) == 0 ) {
            SyMsgsFlagBags = (SyMsgsFlagBags + 1) % 3;
        }

        /* otherwise complain                                              */
        else {
            cmd = ErrorReturnObj(
                "GASMAN: <cmd> must be %s or %s",
                (Int)"\"display\" or \"clear\"",
                (Int)"\"collect\" or \"message\" or \"partial\"",
                "you can return a new string for <cmd>" );
            goto again;
        }
    }

    /* return nothing, this function is a procedure                        */
    return 0;
}


/****************************************************************************
**
*F  FuncSHALLOW_SIZE( <self>, <obj> ) . . . .  expert function 'SHALLOW_SIZE'
*/
Obj FuncSHALLOW_SIZE (
    Obj                 self,
    Obj                 obj )
{
    return INTOBJ_INT( SIZE_BAG( obj ) );
}


/****************************************************************************
**
*F  FuncTYPE_OBJ( <self>, <obj> ) . . . . . . . .  expert function 'TYPE_OBJ'
*/
Obj FuncTYPE_OBJ (
    Obj                 self,
    Obj                 obj )
{
    Obj                 res;
    Obj                 str;
    Char *              cst;

    res = NEW_PLIST( T_PLIST, 2 );
    SET_LEN_PLIST( res, 2 );

    /* set the type                                                        */
    SET_ELM_PLIST( res, 1, INTOBJ_INT( TYPE_OBJ(obj) ) );
    cst = InfoBags[TYPE_OBJ(obj)].name;
    str = NEW_STRING( SyStrlen(cst) );
    SyStrncat( CSTR_STRING(str), cst, SyStrlen(cst) );
    SET_ELM_PLIST( res, 2, str );

    /* and return                                                          */
    return res;
}


/****************************************************************************
**
*F  FuncXTYPE_OBJ( <self>, <obj> )  . . . . . . . expert function 'XTYPE_OBJ'
*/
Obj FuncXTYPE_OBJ (
    Obj                 self,
    Obj                 obj )
{
    Obj                 res;
    Obj                 str;
    UInt                xtype;
    Char *              cst;

    res = NEW_PLIST( T_PLIST, 2 );
    SET_LEN_PLIST( res, 2 );

    /* set the type                                                        */
    xtype = XType(obj);
    SET_ELM_PLIST( res, 1, INTOBJ_INT(xtype) );
    if ( xtype == T_OBJECT ) {
        cst = "virtual object";
    }
    else if ( xtype == T_MAT_CYC ) {
        cst = "virtual mat cyc";
    }
    else if ( xtype == T_MAT_FFE ) {
        cst = "virtual mat ffe";
    }
    else {
        cst = InfoBags[xtype].name;
    }
    str = NEW_STRING( SyStrlen(cst) );
    SyStrncat( CSTR_STRING(str), cst, SyStrlen(cst) );
    SET_ELM_PLIST( res, 2, str );

    /* and return                                                          */
    return res;
}


/****************************************************************************
**
*F  FuncOBJ_HANDLE( <self>, <obj> ) . . . . . .  expert function 'OBJ_HANDLE'
*/
Obj FuncOBJ_HANLDE (
    Obj                 self,
    Obj                 obj )
{
    UInt                hand;
    UInt                prod;
    Obj                 rem;

    if ( IS_INTOBJ(obj) ) {
        return (Obj)INT_INTOBJ(obj);
    }
    else if ( TYPE_OBJ(obj) == T_INTPOS ) {
        hand = 0;
        prod = 1;
        while ( EQ( obj, INTOBJ_INT(0) ) == 0 ) {
            rem  = RemInt( obj, INTOBJ_INT( 1 << 16 ) );
            obj  = QuoInt( obj, INTOBJ_INT( 1 << 16 ) );
            hand = hand + prod * INT_INTOBJ(rem);
            prod = prod * ( 1 << 16 );
        }
        return (Obj) hand;
    }
    else {
        ErrorQuit( "<handle> must be a positive integer", 0L, 0L );
        return 0;
    }
}


/****************************************************************************
**
*F  FuncHANDLE_OBJ( <self>, <obj> ) . . . . . .  expert function 'HANDLE_OBJ'
*/
Obj FuncHANLDE_OBJ (
    Obj                 self,
    Obj                 obj )
{
    Obj                 hnum;
    Obj                 prod;
    Obj                 tmp;
    UInt                hand;

    hand = (UInt) obj;
    hnum = INTOBJ_INT(0);
    prod = INTOBJ_INT(1);
    while ( 0 < hand ) {
        tmp  = PROD( prod, INTOBJ_INT( hand & 0xffff ) );
        prod = PROD( prod, INTOBJ_INT( 1 << 16 ) );
        hnum = SUM(  hnum, tmp );
        hand = hand >> 16;
    }
    return hnum;
}


/****************************************************************************
**
*F  FuncSWAP_MPTR( <self>, <obj1>, <obj2> ) . . . . . . . swap master pointer
**
**  Never use this function unless you are debugging.
*/
Obj FuncSWAP_MPTR (
    Obj                 self,
    Obj                 obj1,
    Obj                 obj2 )
{
    if ( TYPE_OBJ(obj1) == T_INT || TYPE_OBJ(obj1) == T_FFE ) {
        ErrorQuit("SWAP_MPTR: <obj1> must not be an integer or ffe", 0L, 0L);
        return 0;
    }
    if ( TYPE_OBJ(obj2) == T_INT || TYPE_OBJ(obj2) == T_FFE ) {
        ErrorQuit("SWAP_MPTR: <obj2> must not be an integer or ffe", 0L, 0L);
        return 0;
    }
        
    SwapMasterPoint( obj1, obj2 );
    return 0;
}


/****************************************************************************
**

*F * * * * * * * * * * * * * initialize package * * * * * * * * * * * * * * *
*/

/****************************************************************************
**

*F  ImportGVarFromLibrary( <name>, <address> )  . . .  import global variable
*/
static UInt   ImportedGVars [1024];
static Obj *  ImportedGVarAddrs [1024];
static UInt   NrImportedGVars = 0;


void ImportGVarFromLibrary(
    Char *	    name,
    Obj *           address )
{
    if ( NrImportedGVars == 1024 ) {
	if ( ! SyQuiet ) {
	    Pr( "#W  warning: too many imported GVars\n", 0L, 0L );
	}
	InitCopyGVar( GVarName(name), address );
    }
    else {
	ImportedGVars[NrImportedGVars]     = GVarName(name);
	ImportedGVarAddrs[NrImportedGVars] = address;
	InitCopyGVar( ImportedGVars[NrImportedGVars], address );
	NrImportedGVars++;
    }
}


/****************************************************************************
**
*F  ImportFuncFromLibrary( <name>, <address> )  . . .  import global function
*/
static UInt   ImportedFuncs [1024];
static Obj *  ImportedFuncAddrs [1024];
static UInt   NrImportedFuncs = 0;


void ImportFuncFromLibrary(
    Char *	    name,
    Obj *           address )
{
    if ( NrImportedFuncs == 1024 ) {
	if ( ! SyQuiet ) {
	    Pr( "#W  warning: too many imported Funcs\n", 0L, 0L );
	}
	InitFopyGVar( GVarName(name), address );
    }
    else {
	ImportedFuncs[NrImportedFuncs]     = GVarName(name);
	ImportedFuncAddrs[NrImportedFuncs] = address;
	InitFopyGVar( ImportedFuncs[NrImportedFuncs], address );
	NrImportedFuncs++;
    }
}


/****************************************************************************
**
*F  FuncExportToKernelFinished( <self> )  . . . . . . . . . . check functions
*/
Obj FuncExportToKernelFinished (
    Obj		    self )
{
    UInt	    i;
    Int		    errs = 0;

    for ( i = 0;  i < NrImportedGVars;  i++ ) {
	if ( *ImportedGVarAddrs[i] == 0 ) {
	    errs++;
	    if ( ! SyQuiet ) {
		Pr( "#W  global variable '%s' has not been defined\n",
		    (Int)NameGVar(ImportedGVars[i]), 0L );
	    }
	}
	else {
	    SET_ELM_PLIST( WriteGVars, ImportedGVars[i], INTOBJ_INT(0) );
	}
    }
    
    for ( i = 0;  i < NrImportedFuncs;  i++ ) {
	if ( *ImportedFuncAddrs[i] == ErrorMustEvalToFuncFunc
          || *ImportedFuncAddrs[i] == ErrorMustHaveAssObjFunc )
	{
	    errs++;
	    if ( ! SyQuiet ) {
		Pr( "#W  global function '%s' has not been defined\n",
		    (Int)NameGVar(ImportedFuncs[i]), 0L );
	    }
	}
	else {
	    SET_ELM_PLIST( WriteGVars, ImportedFuncs[i], INTOBJ_INT(0) );
	}
    }
    
    return errs == 0 ? True : False;
}


/****************************************************************************
**
*F  SET_REVISION( <file>, <revision> )
*/
char * Revisions[1024];
UInt   RevisionsSize = 0;

#define SET_REVISION( file, revision ) \
  do { \
      extern char * revision; \
      Revisions[RevisionsSize++] = file; \
      Revisions[RevisionsSize++] = revision; \
  } while (0)


/****************************************************************************
**
*F  InitGap() . . . . . . . . . . . . . . . . . . . . . . . . . intialize GAP
*/
extern TypeMarkFuncBags TabMarkFuncBags [ 256 ];

void InitGap (
    int *               pargc,
    char *              argv [] )
{
    UInt                type;
    Char *              version;
    Obj                 string;
    Obj                 record;
    UInt                rnam;
    UInt                i;


    /* initialize the basic system and gasman                              */
    InitSystem( *pargc, argv );

    InitBags( SyAllocBags, SyStorMin,
              0, (Bag*)pargc, SyStackAlign,
              SyCacheSize, 0, SyAbortBags );
    InitMsgsFuncBags( SyMsgsBags );

    SET_REVISION( "system_c",   Revision_system_c );
    SET_REVISION( "system_h",   Revision_system_h );
    SET_REVISION( "gasman_c",   Revision_gasman_c );
    SET_REVISION( "gasman_h",   Revision_gasman_h );


    /* read all packages                                                   */
    SET_REVISION( "gap_c",      Revision_gap_c );
    SET_REVISION( "gap_h",      Revision_gap_h );

    InitGVars();
    SET_REVISION( "gvars_c",    Revision_gvars_c );
    SET_REVISION( "gvars_h",    Revision_gvars_h );

    InitObjects();
    SET_REVISION( "objects_c",  Revision_objects_c );
    SET_REVISION( "objects_h",  Revision_objects_h );

    InitCalls();
    SET_REVISION( "calls_c",    Revision_calls_c );
    SET_REVISION( "calls_h",    Revision_calls_h );

    InitOpers();
    SET_REVISION( "opers_c",    Revision_opers_c );
    SET_REVISION( "opers_h",    Revision_opers_h );

    InitAriths();
    SET_REVISION( "ariths_c",   Revision_ariths_c );
    SET_REVISION( "ariths_h",   Revision_ariths_h );

    InitRecords();
    SET_REVISION( "records_c",  Revision_records_c );
    SET_REVISION( "records_h",  Revision_records_h );

    InitLists();
    SET_REVISION( "lists_c",    Revision_lists_c );
    SET_REVISION( "lists_h",    Revision_lists_h );

    InitBool();
    SET_REVISION( "bool_c",     Revision_bool_c );
    SET_REVISION( "bool_h",     Revision_bool_h );

    InitInt();
    SET_REVISION( "integer_c",  Revision_integer_c );
    SET_REVISION( "integer_h",  Revision_integer_h );

    InitRat();
    SET_REVISION( "rational_c", Revision_rational_c );
    SET_REVISION( "rational_h", Revision_rational_h );

    InitCyc();
    SET_REVISION( "cyclotom_c", Revision_cyclotom_c );
    SET_REVISION( "cyclotom_h", Revision_cyclotom_h );

    InitFinfield();
    SET_REVISION( "finfield_c", Revision_finfield_c );
    SET_REVISION( "finfield_h", Revision_finfield_h );

    InitPermutat();
    SET_REVISION( "permutat_c", Revision_permutat_c );
    SET_REVISION( "permutat_h", Revision_permutat_h );

    InitPRecord();
    SET_REVISION( "precord_c",  Revision_precord_c );
    SET_REVISION( "precord_h",  Revision_precord_h );

    InitListOper();
    SET_REVISION( "listoper_c", Revision_listoper_c );
    SET_REVISION( "listoper_h", Revision_listoper_h );

    InitListFunc();
    SET_REVISION( "listfunc_c", Revision_listfunc_c );
    SET_REVISION( "listfunc_h", Revision_listfunc_h );

    InitPlist();
    SET_REVISION( "plist_c",    Revision_plist_c );
    SET_REVISION( "plist_h",    Revision_plist_h );

    InitSet();
    SET_REVISION( "set_c",      Revision_set_c );
    SET_REVISION( "set_h",      Revision_set_h );

    InitVector();
    SET_REVISION( "vector_c",   Revision_vector_c );
    SET_REVISION( "vector_h",   Revision_vector_h );

    InitBlist();
    SET_REVISION( "blister_c",  Revision_blister_c );
    SET_REVISION( "blister_h",  Revision_blister_h );

    InitRange();
    SET_REVISION( "range_c",    Revision_range_c );
    SET_REVISION( "range_h",    Revision_range_h );

    InitString();
    SET_REVISION( "string_c",   Revision_string_c );
    SET_REVISION( "string_h",   Revision_string_h );

    InitFreeGroupElements();
    SET_REVISION( "objfgelm_c", Revision_objfgelm_c );
    SET_REVISION( "objfgelm_h", Revision_objfgelm_h );

    InitSingleCollector();
    SET_REVISION( "objscoll_c", Revision_objscoll_c );
    SET_REVISION( "objscoll_h", Revision_objscoll_h );

    InitPcElements();
    SET_REVISION( "objpcgel_c", Revision_objpcgel_c );
    SET_REVISION( "objpcgel_h", Revision_objpcgel_h );

    InitSCTable();
    SET_REVISION( "sctable_c",  Revision_sctable_c );
    SET_REVISION( "sctable_h",  Revision_sctable_h );

    InitCosetTable();
    SET_REVISION( "costab_c",   Revision_costab_c );
    SET_REVISION( "costab_h",   Revision_costab_h );

    InitCode();
    SET_REVISION( "code_c",     Revision_code_c );
    SET_REVISION( "code_h",     Revision_code_h );

    InitExprs();
    SET_REVISION( "exprs_c",    Revision_exprs_c );
    SET_REVISION( "exprs_h",    Revision_exprs_h );

    InitStats();
    SET_REVISION( "stats_c",    Revision_stats_c );
    SET_REVISION( "stats_h",    Revision_stats_h );

    /* must come after InitExpr and InitStats                              */
    InitVars();
    SET_REVISION( "vars_c",     Revision_vars_c );
    SET_REVISION( "vars_h",     Revision_vars_h );

    InitFuncs();
    SET_REVISION( "funcs_c",    Revision_funcs_c );
    SET_REVISION( "funcs_h",    Revision_funcs_h );

    InitPcc();

    InitDeepThought();
    InitDTEvaluation();

    InitIntrprtr();
    SET_REVISION( "intrprtr_c", Revision_intrprtr_c );
    SET_REVISION( "intrprtr_h", Revision_intrprtr_h );

    InitCompiler();
    SET_REVISION( "compiler_c", Revision_compiler_c );
    SET_REVISION( "compiler_h", Revision_compiler_h );

    InitScanner();
    SET_REVISION( "scanner_c",  Revision_scanner_c );
    SET_REVISION( "scanner_h",  Revision_scanner_h );

    InitRead();
    SET_REVISION( "read_c",     Revision_read_c );
    SET_REVISION( "read_h",     Revision_read_h );


    /* and now for a special hack                                          */
    for ( i = LAST_CONSTANT_TYPE+1; i <= LAST_REAL_TYPE; i++ ) {
        TabMarkFuncBags[ i+COPYING ] = TabMarkFuncBags[ i ];
    }


    /* init the completion function                                        */
    InitGlobalBag( &CompNowFuncs  );
    InitGlobalBag( &CompThenFuncs );
    InitGlobalBag( &CompLists     );
    CompLists = NEW_PLIST( T_PLIST, 0 );
    SET_LEN_PLIST( CompLists, 0 );


    /* construct the last and time variables                               */
    Last  = GVarName( "last"  );
    Last2 = GVarName( "last2" );
    Last3 = GVarName( "last3" );
    Time  = GVarName( "time"  );


    /* version info                                                        */
    version = "v4r0p0 1996/06/06";
    string = NEW_STRING( SyStrlen(version) );
    SyStrncat( CSTR_STRING(string), version, SyStrlen(version) );
    AssGVar( GVarName( "VERSRC" ), string );

    string = NEW_STRING( SyStrlen(SyFlags) );
    SyStrncat( CSTR_STRING(string), SyFlags, SyStrlen(SyFlags) );
    AssGVar( GVarName( "VERSYS" ), string );

    record = NEW_PREC(0);
    for ( i = 0;  i < RevisionsSize;  i += 2 ) {
        rnam   = RNamName(Revisions[i]);
        string = NEW_STRING( SyStrlen(Revisions[i+1]) );
        SyStrncat( CSTR_STRING(string), Revisions[i+1],
                   SyStrlen(Revisions[i+1]) );
        AssPRec( record, rnam, string );
        CHANGED_BAG(record);
    }
    AssGVar( GVarName( "Revision" ), record );


    /* library name and other stuff                                        */
    AssGVar( GVarName( "QUIET" ),
	     (SyQuiet  ? True : False) );

    AssGVar( GVarName( "BANNER" ),
	     (SyBanner ? True : False) );

    AssGVar( GVarName( "DEBUG_LOADING" ),
	     (SyDebugLoading ? True : False) );

    AssGVar( GVarName( "CHECK_FOR_COMP_FILES" ),
	     (SyCheckForCompFiles ? True : False) );


    /* install the internal functions                                      */
    AssGVar( GVarName( "Runtime" ),
         NewFunctionC( "Runtime", 0L, "",
                    FuncRuntime ) );

    AssGVar( GVarName( "SizeScreen" ),
         NewFunctionC( "SizeScreen", -1L, "args",
                        SizeScreenHandler ) );

    AssGVar( GVarName( "ID_FUNC" ),
         NewFunctionC( "ID_FUNC", 1L, "object",
                    FuncID_FUNC ) );


    /* install the print and error functions                               */
    AssGVar( GVarName( "Print" ),
         NewFunctionC( "Print", -1L, "args",
                    FuncPrint ) );

    AssGVar( GVarName( "DownEnv" ),
         NewFunctionC( "DownEnv", -1L, "",
                    FuncDownEnv ) );

    AssGVar( GVarName( "Where" ),
         NewFunctionC( "Where", -1L, "",
                    FuncWhere ) );

    AssGVar( GVarName( "Error" ),
         NewFunctionC( "Error", -1L, "args",
                    FuncError ) );


    /* install the functions for creating the init file                    */
    AssGVar( GVarName( "COM_FILE" ),
         NewFunctionC( "COM_FILE", 2L, "filename, crc",
                    FuncCOM_FILE ) );

    AssGVar( GVarName( "COM_FUN" ),
         NewFunctionC( "COM_FUN", 1L, "number",
                    FuncCOM_FUN ) );

    AssGVar( GVarName( "MAKE_INIT" ),
         NewFunctionC( "MAKE_INIT", -1L, "output, input1, ...",
                   FuncMAKE_INIT ) );


    /* install functions for dynamically/statically loadable modules       */
    AssGVar( GVarName( "GAP_CRC" ),
         NewFunctionC( "GAP_CRC", 1L, "filename",
                    FuncGAP_CRC ) );

    AssGVar( GVarName( "LOAD_DYN" ),
         NewFunctionC( "LOAD_DYN", 2L, "filename, crc",
                    FuncLOAD_DYN ) );

    AssGVar( GVarName( "LOAD_STAT" ),
         NewFunctionC( "LOAD_STAT", 2L, "filename, crc",
                    FuncLOAD_STAT ) );


    /* streams and files related functions                                 */
    AssGVar( GVarName( "READ" ),
         NewFunctionC( "READ", 1L, "filename",
                    FuncREAD ) );

    AssGVar( GVarName( "READ_AS_FUNC" ),
         NewFunctionC( "READ_AS_FUNC", 1L, "filename",
                    FuncREAD_AS_FUNC ) );

    AssGVar( GVarName( "READ_GAP_ROOT" ),
         NewFunctionC( "READ_GAP_ROOT", 1L, "filename",
                    FuncREAD_GAP_ROOT ) );

    AssGVar( GVarName( "ReadTest" ),
         NewFunctionC( "ReadTest", 1L, "filename",
                    FuncReadTest ) );

    AssGVar( GVarName( "LogTo" ),
         NewFunctionC( "LogTo", -1L, "args",
                    FuncLogTo ) );

    AssGVar( GVarName( "IsExistingFile" ),
         NewFunctionC( "IsExistingFile", 1L, "filename",
                    FuncIsExistingFile ) );

    AssGVar( GVarName( "IsReadableFile" ),
         NewFunctionC( "IsReadableFile", 1L, "filename",
                    FuncIsReadableFile ) );

    AssGVar( GVarName( "IsWritableFile" ),
         NewFunctionC( "IsWritableFile", 1L, "filename",
                    FuncIsWritableFile ) );

    AssGVar( GVarName( "IsExecutableFile" ),
         NewFunctionC( "IsExecutableFile", 1L, "filename",
                    FuncIsExecutableFile ) );

    AssGVar( GVarName( "ExportToKernelFinished" ),
	 NewFunctionC( "ExportToKernelFinished", 0L, "",
		    FuncExportToKernelFinished ) );


    /* debugging functions                                                 */
    AssGVar( GVarName( "GASMAN" ),
         NewFunctionC( "GASMAN", -1L, "args",
                    FuncGASMAN ) );

    AssGVar( GVarName( "SHALLOW_SIZE" ),
         NewFunctionC( "SHALLOW_SIZE", 1L, "object",
                    FuncSHALLOW_SIZE ) );

    AssGVar( GVarName( "TYPE_OBJ" ),
         NewFunctionC( "TYPE_OBJ", 1L, "object",
                    FuncTYPE_OBJ ) );

    AssGVar( GVarName( "XTYPE_OBJ" ),
         NewFunctionC( "XTYPE_OBJ", 1L, "object",
                    FuncXTYPE_OBJ ) );

    AssGVar( GVarName( "OBJ_HANDLE" ),
         NewFunctionC( "OBJ_HANDLE", 1L, "object",
                    FuncOBJ_HANLDE ) );

    AssGVar( GVarName( "HANDLE_OBJ" ),
         NewFunctionC( "HANDLE_OBJ", 1L, "object",
                    FuncHANLDE_OBJ ) );

    AssGVar( GVarName( "SWAP_MPTR" ),
         NewFunctionC( "SWAP_MPTR", 2L, "obj1, obj2",
                    FuncSWAP_MPTR ) );


    /* read the init files                                                 */
    if ( SySystemInitFile[0] ) {
        if ( READ_GAP_ROOT(SySystemInitFile) == 0 ) {
            if ( ! SyQuiet ) {
                Pr( "gap: hmm, I cannot find '%s' maybe",
		    (Int)SySystemInitFile, 0L );
                Pr( " use option '-l <gaproot>'?\n", 0L, 0L );
            }
        }
    }
    for ( i = 0; i < sizeof(SyInitfiles)/sizeof(SyInitfiles[0]); i++ ) {
        if ( SyInitfiles[i][0] != '\0' ) {
            if ( OpenInput( SyInitfiles[i] ) ) {
                NrError = 0;
                while ( 1 ) {
                    type = ReadEvalCommand();
                    if ( type == 1 || type == 2 ) {
                        Pr("'return' must not be used in file",0L,0L);
                    }
                    else if ( type == 8 || type == 16 ) {
                        break;
                    }
                }
                CloseInput();
                NrError = 0;
            }
            else {
                ErrorQuit(
                    "file \"%s\" must exist and be readable",
                    (Int)SyInitfiles[i], 0L );
            }
        }
    }

}


/****************************************************************************
**

*E  gap.c . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
*/
