/****************************************************************************
**
*A  string.h                    GAP source                   Martin Schoenert
**
*H  @(#)$Id$
**
*Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
**
**  This file declares the functions which mainly deal with strings.
**
**  A *string* is a  list that  has no  holes, and  whose  elements  are  all
**  characters.  For the full definition of strings see chapter  "Strings" in
**  the {\GAP} manual.  Read also "More about Strings" about the  string flag
**  and the compact representation of strings.
**
**  Strings  can be accessed  through the macros 'NEW_STRING', 'CSTR_STRING',
**  'GET_LEN_STRING', and 'GET_ELM_STRING'.
**
**  This  package also contains the   list  function  for ranges, which   are
**  installed in the appropriate tables by 'InitString'.
*/
#ifdef  INCLUDE_DECLARATION_PART
char *          Revision_string_h =
   "@(#)$Id$";
#endif


/****************************************************************************
**
*V  ObjsChar[<chr>] . . . . . . . . . . . . . . . . table of character values
**
**  'ObjsChar' contains all the character values.  That way we do not need to
**  allocate new bags for new characters.
*/
extern  Obj             ObjsChar [256];


/****************************************************************************
**
*F  NEW_STRING(<len>) . . . . . . . . . . . . . . . . . . . make a new string
**
**  'NEW_STRING' makes a new string with room for <len> characters.
**
**  Note that 'NEW_STRING' is a macro, so do not  call it with arguments that
**  have sideeffects.
*/
#define NEW_STRING(len)                 (NewBag( T_STRING, (len) + 1 ))


/****************************************************************************
**
*F  CSTR_STRING(<list>) . . . . . . . . . . . . . . . .  C string of a string
**
**  'CSTR_STRING'  returns the (address  of the)  C  character string of  the
**  string <list>.
**
**  Note that 'CSTR_STRING' is a macro, so do not call it with arguments that
**  have sideeffects.
*/
#define CSTR_STRING(list)               ((Char*)ADDR_OBJ(list))


/****************************************************************************
**
*F  GET_LEN_STRING(<list>)  . . . . . . . . . . . . . . .  length of a string
**
**  'GET_LEN_STRING' returns the length of the string <list>, as a C integer.
**
**  Note that  'GET_LEN_STRING' is a macro, so  do not call it with arguments
**  that have sideeffects.
*/
#define GET_LEN_STRING(list)            (SIZE_OBJ(list)-1)


/****************************************************************************
**
*F  GET_ELM_STRING(<list>,<pos>)  . . . . . . . select an element of a string
**
**  'GET_ELM_STRING'  returns the  <pos>-th  element  of  the string  <list>.
**  <pos> must be  a positive integer  less than  or  equal to  the length of
**  <list>.
**
**  Note that 'GET_ELM_STRING' is a  macro, so do not  call it with arguments
**  that have sideeffects.
*/
#define GET_ELM_STRING(list,pos)        (ObjsChar[ \
                                        (UChar)(CSTR_STRING(list)[(pos)-1])])


/****************************************************************************
**
*F  PrintString1(<list>)  . . . . . . . . . . . .  print a string for 'Print'
**
**  'PrintString1' prints the string  constant  in  the  format  used  by  the
**  'Print' and 'PrintTo' function.
*/
extern  void            PrintString1 (
            Obj                 list );


/****************************************************************************
**
*F  IS_STRING(<obj>)  . . . . . . . . . . . . . test if an object is a string
**
**  'IS_STRING' returns 1  if the object <obj>  is a string  and 0 otherwise.
**  It does not change the representation of <obj>.
**
**  Note that 'IS_STRING' is a  macro, so do not call  it with arguments that
**  have sideeffects.
*/
#define IS_STRING(obj)  ((*IsStringFuncs[ TYPE_OBJ( obj ) ])( obj ))

extern  Int             (*IsStringFuncs [LAST_REAL_TYPE+1]) ( Obj obj );


/****************************************************************************
**
*F  IsString(<obj>) . . . . . . . . . . . . . . test if an object is a string
**
**  'IsString' returns 1 if the object <obj> is a string and 0 otherwise.  It
**  does not change the representation of <obj>.
*/
extern  Int             IsString (
            Obj                 obj );


/****************************************************************************
**
*F  ConvString(<string>)  . . . convert a string to the string representation
**
**  'ConvString' converts the string <list> to the string representation.
*/
extern  void            ConvString (
            Obj                 string );


/****************************************************************************
**
*F  IsStringConv(<obj>) . . . . . . test if an object is a string and convert
**
**  'IsStringConv'   returns 1  if   the object <obj>  is   a  string,  and 0
**  otherwise.   If <obj> is a  string it  changes  its representation to the
**  string representation.
*/
extern  Int             IsStringConv (
            Obj                 obj );


/****************************************************************************
**
*F  InitString()  . . . . . . . . . . . . . . . .  initializes string package
**
**  'InitString' initializes the string package.
*/
extern  void            InitString ( void );



