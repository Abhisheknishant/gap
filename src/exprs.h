/****************************************************************************
**
*A  exprs.h                     GAP source                   Martin Schoenert
**
*H  @(#)$Id$
**
*Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
**
**  This file declares the functions of the expressions package.
**
**  The expressions  package is the  part  of the interpreter  that evaluates
**  expressions to their values and prints expressions.
*/
#ifdef  INCLUDE_DECLARATION_PART
char *          Revision_exprs_h =
   "@(#)$Id$";
#endif


/****************************************************************************
**
*F  OBJ_REFLVAR(<expr>) . . . . . . . . . . . value of a reference to a local
**
**  'OBJ_REFLVAR'  returns  the value of  the reference  to a  local variable
**  <expr>.
*/
#ifdef  NO_LVAR_CHECKS
#define OBJ_REFLVAR(expr)       \
                        OBJ_LVAR( LVAR_REFLVAR( (expr) ) )
#endif
#ifndef NO_LVAR_CHECKS
#define OBJ_REFLVAR(expr)       \
                        (OBJ_LVAR( LVAR_REFLVAR( expr ) ) != 0 ? \
                         OBJ_LVAR( LVAR_REFLVAR( expr ) ) : \
                         ObjLVar( LVAR_REFLVAR( expr ) ) )
#endif


/****************************************************************************
**
*F  OBJ_INTEXPR(<expr>) . . . . . . . . . . .  value of an integer expression
**
**  'OBJ_INTEXPR' returns the (immediate)  integer  value of the  (immediate)
**  integer expression <expr>.
**
**  'OBJ_INTEXPR(<expr>)'  should  be 'OBJ_INT(INT_INTEXPR(<expr>))', but for
**  performance  reasons we implement  it   as '(Obj)(<expr>)'.  This is   of
**  course    highly  dependent  on    (immediate)  integer   expressions and
**  (immediate) integer values having the same representation.
*/
#define OBJ_INTEXPR(expr)       \
                        ((Obj)(expr))


/****************************************************************************
**
*F  EVAL_EXPR(<expr>) . . . . . . . . . . . . . . . .  evaluate an expression
**
**  'EVAL_EXPR' evaluates the expression <expr>.
**
**  'EVAL_EXPR' returns the value of <expr>.
**
**  'EVAL_EXPR'  causes  the   evaluation of   <expr> by  dispatching  to the
**  evaluator, i.e., to  the function that evaluates  expressions of the type
**  of <expr>.
**
**  Note that 'EVAL_EXPR' does not use 'TYPE_EXPR', since it also handles the
**  two special cases that 'TYPE_EXPR' handles.
*/
#define EVAL_EXPR(expr) \
                        (IS_REFLVAR(expr) ? OBJ_REFLVAR(expr) : \
                         (IS_INTEXPR(expr) ? OBJ_INTEXPR(expr) : \
                          (*EvalExprFuncs[ TYPE_BAG(expr) ])( expr ) ))


/****************************************************************************
**
*V  EvalExprFuncs[<type>]  . . . . . evaluator for expressions of type <type>
**
**  'EvalExprFuncs'  is the dispatch table   that contains for  every type of
**  expressions a pointer  to the  evaluator  for expressions of this   type,
**  i.e., the function that should be  called to evaluate expressions of this
**  type.
*/
extern  Obj             (* EvalExprFuncs [256]) ( Expr expr );


/****************************************************************************
**
*F  EVAL_BOOL_EXPR(<expr>)  . . . . evaluate an expression to a boolean value
**
**  'EVAL_BOOL_EXPR' evaluates   the expression  <expr> and  checks  that the
**  value is either  'true' or 'false'.  If the  expression does not evaluate
**  to 'true' or 'false', then an error is signalled.
**
**  'EVAL_BOOL_EXPR' returns the  value of <expr> (which  is either 'true' or
**  'false').
*/
#define EVAL_BOOL_EXPR(expr) \
                        ( (*EvalBoolFuncs[ TYPE_EXPR( expr ) ])( expr ) )


/****************************************************************************
**
*V  EvalBoolFuncs[<type>] . . boolean evaluator for expression of type <type>
**
**  'EvalBoolFuncs'  is  the dispatch table that  contains  for every type of
**  expression a pointer to a boolean evaluator for expressions of this type,
**  i.e., a pointer to  a function which  is  guaranteed to return a  boolean
**  value that should be called to evaluate expressions of this type.
*/
extern  Obj             (* EvalBoolFuncs [256]) ( Expr expr );


/****************************************************************************
**
*F  PrintExpr(<expr>) . . . . . . . . . . . . . . . . . . print an expression
**
**  'PrintExpr' prints the expression <expr>.
*/
extern  void            PrintExpr (
            Expr                expr );


/****************************************************************************
**
*V  PrintExprFuncs[<type>]  . .  printing function for objects of type <type>
**
**  'PrintExprFuncs' is the dispatching table that contains for every type of
**  expressions a pointer to the printer for expressions  of this type, i.e.,
**  the function that should be called to print expressions of this type.
*/
extern  void            (* PrintExprFuncs [256] ) ( Expr expr );


/****************************************************************************
**
*F  InitExprs . . . . . . . . . . . . . .  initialize the expressions package
**
**  'InitEval' initializes the expressions package.
*/
extern  void            InitExprs ( void );



