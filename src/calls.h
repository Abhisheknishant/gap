/****************************************************************************
**
*A  calls.h                     GAP source                   Martin Schoenert
**
*H  @(#)$Id$
**
*Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
**
**  This file  declares the functions of the  generic function call mechanism
**  package.
**
**  This package defines the *call mechanism* through which one GAP function,
**  named the *caller*, can temporarily transfer control to another function,
**  named the *callee*.
**
**  There are *compiled functions* and  *interpreted functions*.  Thus  there
**  are four possible pairings of caller and callee.
**
**  If the caller is compiled,  then the call comes directly from the caller.
**  If it  is interpreted, then   the call comes  from one  of the  functions
**  'EvalFunccall<i>args' that implement evaluation of function calls.
**
**  If the callee is compiled,  then the call goes  directly  to the  callee.
**  If   it is interpreted,   then the  call   goes to one  of  the  handlers
**  'DoExecFunc<i>args' that implement execution of function bodies.
**
**  The call mechanism makes it in any case unneccessary for the calling code
**  to  know  whether the callee  is  a compiled or  an interpreted function.
**  Likewise the called code need not know, actually cannot know, whether the
**  caller is a compiled or an interpreted function.
**
**  Also the call mechanism checks that the number of arguments passed by the
**  caller is the same as the number of arguments  expected by the callee, or
**  it  collects the arguments   in a list  if  the callee allows  a variable
**  number of arguments.
**
**  Finally the call mechanism profiles all functions if requested.
**
**  All this has very little overhead.  In the  case of one compiled function
**  calling  another compiled function, which expects fewer than 4 arguments,
**  with no profiling, the overhead is only a couple of instructions.
*/
#ifdef  INCLUDE_DECLARATION_PART
char *          Revision_calls_h =
   "@(#)$Id$";
#endif


/****************************************************************************
**
*T  ObjFunc . . . . . . . . . . . . . . . . type of function returning object
**
**  'ObjFunc' is the type of a function returning an object.
*/
typedef Obj             (* ObjFunc) ();


/****************************************************************************
**
*F  HDLR_FUNC(<func>,<i>) . . . . . . . . . <i>-th call handler of a function
*F  NAME_FUNC(<func>) . . . . . . . . . . . . . . . . . .  name of a function
*F  NARG_FUNC(<func>) . . . . . . . . . . . number of arguments of a function
*F  NAMS_FUNC(<func>) . . . . . . . .  names of local variables of a function
*F  NAMI_FUNC(<func>) . . . . . . name of <i>-th local variable of a function
*F  PROF_FUNC(<func>) . . . . . . . . profiling information bag of a function
*F  NLOC_FUNC(<func>) . . . . . . . . . . . .  number of locals of a function
*F  BODY_FUNC(<func>) . . . . . . . . . . . . . . . . . .  body of a function
*F  ENVI_FUNC(<func>) . . . . . . . . . . . . . . . environment of a function
*F  FEXS_FUNC(<func>) . . . . . . . . . . . .  func. expr. list of a function
*V  SIZE_FUNC . . . . . . . . . . . . . . . . . size of the bag of a function
**
**  These macros  make it possible  to access  the  various components  of  a
**  function.
**
**  'HDLR_FUNC(<func>,<i>)' is the <i>-th handler of the function <func>.
**
**  'NAME_FUNC(<func>)' is the name of the function.
**
**  'NARG_FUNC(<func>)' is the number of arguments (-1  if  <func>  accepts a
**  variable number of arguments).
**
**  'NAMS_FUNC(<func>)'  is the list of the names of the local variables,
**
**  'NAMI_FUNC(<func>,<i>)' is the name of the <i>-th local variable.
**
**  'PROF_FUNC(<func>)' is the profiling information bag.
**
**  'NLOC_FUNC(<func>)' is the number of local variables of  the  interpreted
**  function <func>.
**
**  'BODY_FUNC(<func>)' is the body.
**
**  'ENVI_FUNC(<func>)'  is the  environment  (i.e., the local  variables bag
**  that was current when <func> was created).
**
**  'FEXS_FUNC(<func>)'  is the function expressions list (i.e., the list of
**  the function expressions of the functions defined inside of <func>).
**
*/
#define HDLR_FUNC(func,i)       (* (ObjFunc*) (ADDR_OBJ(func) + 0 +(i)) )
#define NAME_FUNC(func)         (*            (ADDR_OBJ(func) + 8     ) )
#define NARG_FUNC(func)         (* (Int*)     (ADDR_OBJ(func) + 9     ) )
#define NAMS_FUNC(func)         (*            (ADDR_OBJ(func) +10     ) )
#define NAMI_FUNC(func,i)       ((Char*)ADDR_OBJ(ELM_LIST(NAMS_FUNC(func),i)))
#define PROF_FUNC(func)         (*            (ADDR_OBJ(func) +11     ) )
#define NLOC_FUNC(func)         (* (Int*)     (ADDR_OBJ(func) +12     ) )
#define BODY_FUNC(func)         (*            (ADDR_OBJ(func) +13     ) )
#define ENVI_FUNC(func)         (*            (ADDR_OBJ(func) +14     ) )
#define FEXS_FUNC(func)         (*            (ADDR_OBJ(func) +15     ) )
#define SIZE_FUNC               (16*sizeof(Bag))


/****************************************************************************
**
*F  CALL_0ARGS(<func>)  . . . . . . . . . call a function with 0    arguments
*F  CALL_1ARGS(<func>,<arg1>) . . . . . . call a function with 1    arguments
*F  CALL_2ARGS(<func>,<arg1>...)  . . . . call a function with 2    arguments
*F  CALL_3ARGS(<func>,<arg1>...)  . . . . call a function with 3    arguments
*F  CALL_4ARGS(<func>,<arg1>...)  . . . . call a function with 4    arguments
*F  CALL_5ARGS(<func>,<arg1>...)  . . . . call a function with 5    arguments
*F  CALL_6ARGS(<func>,<arg1>...)  . . . . call a function with 6    arguments
*F  CALL_XARGS(<func>,<args>) . . . . . . call a function with more arguments
**
**  'CALL_<i>ARGS' passes control  to  the function  <func>, which must  be a
**  function object  ('T_FUNCTION').  It returns the  return value of <func>.
**  'CALL_0ARGS' is for calls passing   no arguments, 'CALL_1ARGS' for  calls
**  passing one argument, and so on.   'CALL_XARGS' is for calls passing more
**  than 5 arguments, where the arguments must be collected  in a plain list,
**  and this plain list must then be passed.
**
**  'CALL_<i>ARGS' can be used independently  of whether the called  function
**  is a compiled   or interpreted function.    It checks that the number  of
**  passed arguments is the same  as the number of  arguments expected by the
**  callee,  or it collects the  arguments in a list  if  the callee allows a
**  variable number of arguments.
*/
#define CALL_0ARGS(f)                     HDLR_FUNC(f,0)(f)
#define CALL_1ARGS(f,a1)                  HDLR_FUNC(f,1)(f,a1)
#define CALL_2ARGS(f,a1,a2)               HDLR_FUNC(f,2)(f,a1,a2)
#define CALL_3ARGS(f,a1,a2,a3)            HDLR_FUNC(f,3)(f,a1,a2,a3)
#define CALL_4ARGS(f,a1,a2,a3,a4)         HDLR_FUNC(f,4)(f,a1,a2,a3,a4)
#define CALL_5ARGS(f,a1,a2,a3,a4,a5)      HDLR_FUNC(f,5)(f,a1,a2,a3,a4,a5)
#define CALL_6ARGS(f,a1,a2,a3,a4,a5,a6)   HDLR_FUNC(f,6)(f,a1,a2,a3,a4,a5,a6)
#define CALL_XARGS(f,as)                  HDLR_FUNC(f,7)(f,as)


/****************************************************************************
**
*F  InitHandlerFunc( <handler>, <cookie> ) . . . . . . . . register a handler
**
**  Every handler should  be registered (once) before  it is installed in any
**  function bag. This is needed so that it can be  identified when loading a
**  saved workspace.  <cookie> should be a  unique  C string, identifying the
**  handler
*/
extern void InitHandlerFunc (
     ObjFunc        hdlr,
     Char *         cookie );


/****************************************************************************
**
*F  NewFunction(<name>,<narg>,<nams>,<hdlr>) . . . . . .  make a new function
*F  NewFunctionC(<name>,<narg>,<nams>,<hdlr>)  . . . . .  make a new function
*F  NewFunctionT(<type>,<size>,<name>,<narg>,<nams>,<hdlr>) . .  new function
*F  NewFunctionCT(<type>,<size>,<name>,<narg>,<nams>,<hdlr>)  .  new function
**
**  'NewFunction' creates and returns a new function.  <name> must be  a  GAP
**  string containing the name of the function.  <narg> must be the number of
**  arguments, where -1 means a variable number of arguments.  <nams> must be
**  a GAP list containg the names  of  the  arguments.  <hdlr>  must  be  the
**  C function (accepting <self> and  the  <narg>  arguments)  that  will  be
**  called to execute the function.
**
**  'NewFunctionC' does the same as 'NewFunction',  but  expects  <name>  and
**  <nams> as C strings.
**
**  'NewFunctionT' does the same as 'NewFunction', but allows to specify  the
**  <type> and <size> of the newly created bag.
**
**  'NewFunctionCT' does the same as 'NewFunction', but  expects  <name>  and
**  <nams> as C strings, and allows to specify the <type> and <size>  of  the
**  newly created bag.
*/
extern  Obj             NewFunction (
            Obj                 name,
            Int                 narg,
            Obj                 nams,
            ObjFunc             hdlr );
    
extern  Obj             NewFunctionC (
            Char *              name,
            Int                 narg,
            Char *              nams,
            ObjFunc             hdlr );
    
extern  Obj             NewFunctionT (
            UInt                type,
            UInt                size,
            Obj                 name,
            Int                 narg,
            Obj                 nams,
            ObjFunc             hdlr );
    
extern  Obj             NewFunctionCT (
            UInt                type,
            UInt                size,
            Char *              name,
            Int                 narg,
            Char *              nams,
            ObjFunc             hdlr );
    

/****************************************************************************
**
*F  PrintFunction( <func> )   . . . . . . . . . . . . . . .  print a function
**
**  'PrintFunction' prints  the   function  <func> in  abbreviated  form   if
**  'PrintObjFull' is false.
*/
extern void PrintFunction (
    Obj                 func );


/****************************************************************************
**
*F  C_NEW_GVAR_FUNC( <name>, <nargs>, <nams>, <hdlr>, <cookie> )
*/
#define C_NEW_GVAR_FUNC( name, nargs, nams, hdlr, cookie ) \
    InitHandlerFunc( hdlr, cookie ); \
    AssGVar( GVarName( name ), NewFunctionC( name, nargs, nams, hdlr ) )


/****************************************************************************
**
*F  C_NEW_GVAR_ATTR( <name>, <nams>, <attr>, <hdlr>, <cookie> )
*/
#define C_NEW_GVAR_ATTR( name, nams, attr, hdlr, cookie ) \
    InitHandlerFunc( hdlr, cookie ); \
    attr = NewAttributeC( name, 1L, nams, hdlr ); \
    AssGVar( GVarName( name ), attr )


/****************************************************************************
**
*F  C_NEW_GVAR_OPER( <name>, <nargs>, <nams>, <oper>, <hdlr>, <cookie> )
*/
#define C_NEW_GVAR_OPER( name, nargs, nams, oper, hdlr, cookie ) \
    InitHandlerFunc( hdlr, cookie ); \
    oper = NewOperationC( name, nargs, nams, hdlr ); \
    AssGVar( GVarName( name ), oper )


/****************************************************************************
**
*F  InitCalls() . . . . . . . . . . . . . . . . . initialize the call package
**
**  'InitCalls' initializes the call package.
*/
extern  void            InitCalls ( void );



