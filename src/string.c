/****************************************************************************
**
*A  string.c                    GAP source                   Martin Schoenert
**
*H  @(#)$Id$
**
*Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
**
**  This file contains the functions which mainly deal with strings.
**
**  A *string* is a  list that  has no  holes, and  whose  elements  are  all
**  characters.  For the full definition of strings see chapter  "Strings" in
**  the {\GAP} manual.  Read also "More about Strings" about the  string flag
**  and the compact representation of strings.
**
**  A list  that  is  known to  be a  string is  represented by a bag of type
**  'T_STRING', which has the following format:
**
**      +----+----+- - - -+----+----+
**      |1st |2nd |       |last|null|
**      |char|char|       |char|char|
**      +----+----+- - - -+----+----+
**
**  Each entry is a  single character (of C type 'unsigned char').   The last
**  entry  in  the  bag is the  null  character  ('\0'),  which terminates  C
**  strings.
**
**  Note that a list represented by a bag of type  'T_PLIST' or 'T_SET' might
**  still be a string.  It is just that the kernel does not know this.
**
**  This package consists of three parts.
**
**  The first  part consists  of    the macros 'NEW_STRING',   'CSTR_STRING',
**  'GET_LEN_STRING',    and   'GET_ELM_STRING'.    They     determine    the
**  respresentation of strings.
**
**  The second part  consists  of  the  functions  'LenString',  'ElmString',
**  'ElmsStrings', 'AssString',  'AsssString', PlainString', 'IsDenseString',
**  and 'IsPossString'.  They are the functions requried by the generic lists
**  package.  Using these functions the other  parts of the {\GAP} kernel can
**  access and  modify strings  without actually  being aware  that they  are
**  dealing with a string.
**
**  The third part consists  of the functions 'PrintString', which is  called
**  by 'FunPrint', and 'IsString', which test whether an arbitrary list  is a
**  string, and if so converts it into the above format.
*/
char *          Revision_string_c =
   "@(#)$Id$";

#include        "system.h"              /* system dependent functions      */
#include        "scanner.h"             /* Pr                              */
#include        "gasman.h"              /* NewBag, ResizeBag, CHANGED_BAG  */

#include        "objects.h"             /* Obj, TYPE_OBJ, SIZE_OBJ, ...    */
#include        "gvars.h"               /* AssGVar, GVarName               */

#include        "calls.h"               /* generic call mechanism          */
#include        "opers.h"               /* generic operations package      */

#include        "ariths.h"              /* generic operations package      */
#include        "lists.h"               /* generic list package            */

#include        "bool.h"                /* True, False                     */

#include        "plist.h"               /* GET_LEN_PLIST, GET_ELM_PLIST,...*/

#include        "range.h"               /* GET_LEN_RANGE, GET_LOW_RANGE,...*/

#define INCLUDE_DECLARATION_PART
#include        "string.h"              /* declaration part of the package */
#undef  INCLUDE_DECLARATION_PART

#include        "gap.h"                 /* Error                           */


/****************************************************************************
**
*V  ObjsChar[<chr>] . . . . . . . . . . . . . . . . table of character values
**
**  'ObjsChar' contains all the character values.  That way we do not need to
**  allocate new bags for new characters.
*/
Obj             ObjsChar [256];


/****************************************************************************
**
*F  KindChar(<chr>) . . . . . . . . . . . . . . . . kind of a character value
**
**  'KindChar' returns the kind of the character <chr>.
**
**  'KindChar' is the function in 'KindObjFuncs' for character values.
*/
Obj             KIND_CHAR;

Obj             KindChar (
    Obj                 chr )
{
    return KIND_CHAR;
}


/****************************************************************************
**
*F  EqChar(<charL>,<charR>) . . . . . . . . . . . . .  compare two characters
**
**  'EqChar'  returns 'true'  if the two  characters <charL>  and <charR> are
**  equal, and 'false' otherwise.
*/
Int             EqChar (
    Obj                 charL,
    Obj                 charR )
{
    return (*(UChar*)ADDR_OBJ(charL) == *(UChar*)ADDR_OBJ(charR));
}


/****************************************************************************
**
*F  LtChar(<charL>,<charR>) . . . . . . . . . . . . .  compare two characters
**
**  'LtChar' returns  'true' if the    character <charL>  is less than    the
**  character <charR>, and 'false' otherwise.
*/
Int             LtChar (
    Obj                 charL,
    Obj                 charR )
{
    return (*(UChar*)ADDR_OBJ(charL) < *(UChar*)ADDR_OBJ(charR));
}


/****************************************************************************
**
*F  PrintChar(<chr>)  . . . . . . . . . . . . . . . . . . . print a character
**
**  'PrChar' prints the character <chr>.
*/
void            PrintChar (
    Obj                 val )
{
    UChar               chr;

    chr = *(UChar*)ADDR_OBJ(val);
    if      ( chr == '\n'  )  Pr("'\\n'",0L,0L);
    else if ( chr == '\t'  )  Pr("'\\t'",0L,0L);
    else if ( chr == '\r'  )  Pr("'\\r'",0L,0L);
    else if ( chr == '\b'  )  Pr("'\\b'",0L,0L);
    else if ( chr == '\03' )  Pr("'\\c'",0L,0L);
    else if ( chr == '\''  )  Pr("'\\''",0L,0L);
    else if ( chr == '\\'  )  Pr("'\\\\'",0L,0L);
    else                      Pr("'%c'",(Int)chr,0L);
}


/****************************************************************************
**
*F  NEW_STRING(<len>) . . . . . . . . . . . . . . . . . . . make a new string
**
**  'NEW_STRING' makes a new string with room for <len> characters.
**
**  Note that 'NEW_STRING' is a macro, so do not  call it with arguments that
**  have sideeffects.
**
**  'NEW_STRING'  is  defined in   the declaration part  of this   package as
**  follows
**
#define NEW_STRING(len) \
                        NewBag( T_STRING, (len) + 1 )
*/


/****************************************************************************
**
*F  CSTR_STRING(<list>) . . . . . . . . . . . . . . . .  C string of a string
**
**  'CSTR_STRING'  returns the (address  of the)  C  character string of  the
**  string <list>.
**
**  Note that 'CSTR_STRING' is a macro, so do not call it with arguments that
**  have sideeffects.
**
**  'CSTR_STRING' is  defined  in the declaration part   of  this package  as
**  follows
**
#define CSTR_STRING(list) \
                        ((Char*)ADDR_OBJ(list))
*/


/****************************************************************************
**
*F  GET_LEN_STRING(<list>)  . . . . . . . . . . . . . . .  length of a string
**
**  'GET_LEN_STRING' returns the length of the string <list>, as a C integer.
**
**  Note that  'GET_LEN_STRING' is a macro, so  do not call it with arguments
**  that have sideeffects.
**
**  'GET_LEN_STRING' is defined  in the declaration  part of this  package as
**  follows
**
#define GET_LEN_STRING(list) \
                        (SIZE_OBJ(list)-1)
*/


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
**
**  'GET_ELM_STRING'  is defined in  the declaration part  of this package as
**  follows:
**
#define GET_ELM_STRING(list,pos) \
                        ObjsChar[ (UChar)(CSTR_STRING(list)[(pos)-1]) ]
*/


/****************************************************************************
**
*F  KindString(<list>)  . . . . . . . . . . . . . . . . . .  kind of a string
**
**  'KindString' returns the kind of the string <list>.
**
**  'KindString' is the function in 'KindObjFuncs' for strings.
*/
extern  Obj             KIND_LIST_EMPTY_MUTABLE;

extern  Obj             KIND_LIST_EMPTY_IMMUTABLE;

extern  Obj             KIND_LIST_HOM;

Obj             KindString (
    Obj                 list )
{
    Obj                 kind;           /* kind, result                    */
    Int                 ktype;          /* kind type of <list>             */
    Obj                 family;         /* family of elements              */
    Obj                 kinds;          /* kinds list of <family>          */

    /* special case for the empty string                                   */
    if ( GET_LEN_STRING(list) == 0 ) {
        if ( IS_MUTABLE_OBJ(list) ) {
            return KIND_LIST_EMPTY_MUTABLE;
        }
        else {
            return KIND_LIST_EMPTY_IMMUTABLE;
        }
    }

    /* get the kind type and the family of the elements                    */
    ktype  = TYPE_OBJ( list );
    family = FAMILY_KIND( KIND_CHAR );

    /* get the list kinds of that family                                   */
    kinds  = KINDS_LIST_FAM( family );

    /* if the kind is not yet known, compute it                            */
    kind = ELM0_LIST( kinds, ktype-T_STRING+1 );
    if ( kind == 0 ) {
        kind = CALL_2ARGS( KIND_LIST_HOM,
            family, INTOBJ_INT(ktype-T_STRING+1) );
        ASS_LIST( kinds, ktype-T_STRING+1, kind );
    }

    /* return the kind                                                     */
    return kind;
}


/****************************************************************************
**

*F  CopyString( <list>, <mut> ) . . . . . . . . . . . . . . . . copy a string
**
**  'CopyString' returns a structural (deep) copy of the string <list>, i.e.,
**  a recursive copy that preserves the structure.
**
**  If <list> has not  yet  been copied, it makes   a copy, leaves  a forward
**  pointer to the copy in  the first entry of  the string, where the size of
**  the string usually resides,  and copies  all the  entries.  If  the plain
**  list  has already  been copied, it   returns the value of the  forwarding
**  pointer.
**
**  'CopyString' is the function in 'CopyObjFuncs' for strings.
**
**  'CleanString' removes the mark and the forwarding pointer from the string
**  <list>.
**
**  'CleanString' is the function in 'CleanObjFuncs' for strings.
*/
Obj             CopyString (
    Obj                 list,
    Int                 mut )
{
    Obj                 copy;           /* handle of the copy, result      */
    UInt                i;              /* loop variable                   */

    /* don't change immutable objects                                      */
    if ( ! IS_MUTABLE_OBJ(list) ) {
        return list;
    }

    /* make a copy                                                         */
    if ( mut ) {
        copy = NewBag( TYPE_OBJ(list), SIZE_OBJ(list) );
    }
    else {
        copy = NewBag( IMMUTABLE_TYPE( TYPE_OBJ(list) ), SIZE_OBJ(list) );
    }
    ADDR_OBJ(copy)[0] = ADDR_OBJ(list)[0];

    /* leave a forwarding pointer                                          */
    ADDR_OBJ(list)[0] = copy;
    CHANGED_BAG( list );

    /* now it is copied                                                    */
    RetypeBag( list, TYPE_OBJ(list) + COPYING );

    /* copy the subvalues                                                  */
    for ( i = 1; i < (SIZE_OBJ(copy)+sizeof(Obj)-1)/sizeof(Obj); i++ ) {
        ADDR_OBJ(copy)[i] = ADDR_OBJ(list)[i];
    }

    /* return the copy                                                     */
    return copy;
}


/****************************************************************************
**
*F  CopyStringCopy( <list>, <mut> ) . . . . . . . . . .  copy a copied string
*/
Obj CopyStringCopy (
    Obj                 list,
    Int                 mut )
{
    return ADDR_OBJ(list)[0];
}


/****************************************************************************
**
*F  CleanString( <list> ) . . . . . . . . . . . . . . . . . clean up a string
*/
void CleanString (
    Obj                 list )
{
}


/****************************************************************************
**
*F  CleanStringCopy( <list> ) . . . . . . . . . . .  clean up a copied string
*/
void CleanStringCopy (
    Obj                 list )
{
    /* remove the forwarding pointer                                       */
    ADDR_OBJ(list)[0] = ADDR_OBJ( ADDR_OBJ(list)[0] )[0];

    /* now it is cleaned                                                   */
    RetypeBag( list, TYPE_OBJ(list) - COPYING );
}


/****************************************************************************
**

*F  PrintString(<list>) . . . . . . . . . . . . . . . . . . .  print a string
**
**  'PrintString' prints the string with the handle <list>.
**
**  No  linebreaks are allowed,  if one must be  inserted  anyhow, it must be
**  escaped by a backslash '\', which is done in 'Pr'.
*/
void            PrintString (
    Obj                 list )
{
    Pr( "\"%S\"", (Int)CSTR_STRING(list), 0L );
}


/****************************************************************************
**
*F  PrintString1(<list>)  . . . . . . . . . . . .  print a string for 'Print'
**
**  'PrintString1' prints the string  constant  in  the  format  used  by the
**  'Print' and 'PrintTo' function.
*/
void            PrintString1 (
    Obj                 list )
{
    Pr( "%s", (Int)CSTR_STRING(list), 0L );
}


/****************************************************************************
**
*F  EqString(<listL>,<listR>) . . . . . . . .  test whether strings are equal
**
**  'EqString'  returns  'true' if the  two  strings <listL>  and <listR> are
**  equal and 'false' otherwise.
*/
Int             EqString (
    Obj                 listL,
    Obj                 listR )
{
    return (SyStrcmp( CSTR_STRING(listL), CSTR_STRING(listR) ) == 0);
}


/****************************************************************************
**
*F  LtString(<listL>,<listR>) .  test whether one string is less than another
**
**  'LtString' returns 'true' if  the string <listL> is  less than the string
**  <listR> and 'false' otherwise.
*/
Int             LtString (
    Obj                 listL,
    Obj                 listR )
{
    return (SyStrcmp( CSTR_STRING(listL), CSTR_STRING(listR) ) < 0);
}


/****************************************************************************
**
*F  LenString(<list>) . . . . . . . . . . . . . . . . . .  length of a string
**
**  'LenString' returns the length of the string <list> as a C integer.
**
**  'LenString' is the function in 'LenListFuncs' for strings.
*/
Int             LenString (
    Obj                 list )
{
    return GET_LEN_STRING( list );
}


/****************************************************************************
**
*F  IsbString(<list>,<pos>) . . . . . . . . . test for an element of a string
*F  IsbvString(<list>,<pos>)  . . . . . . . . test for an element of a string
**
**  'IsbString' returns 1 if the string <list> contains
**  a character at the position <pos> and 0 otherwise.
**  It can rely on <pos> being a positive integer.
**
**  'IsbvString' does the same thing as 'IsbString', but it can 
**  also rely on <pos> not being larger than the length of <list>.
**
**  'IsbString'  is the function in 'IsbListFuncs'  for strings.
**  'IsbvString' is the function in 'IsbvListFuncs' for strings.
*/
Int             IsbString (
    Obj                 list,
    Int                 pos )
{
    /* since strings are dense, this must only test for the length         */
    return (pos <= GET_LEN_STRING(list));
}

Int             IsbvString (
    Obj                 list,
    Int                 pos )
{
    /* since strings are dense, this can only return 1                     */
    return 1L;
}


/****************************************************************************
**
*F  Elm0String(<list>,<pos>)  . . . . . . . . . select an element of a string
*F  Elm0vString(<list>,<pos>) . . . . . . . . . select an element of a string
**
**  'Elm0String' returns the element at the position <pos> of the string
**  <list>, or returns 0 if <list> has no assigned object at <pos>.
**  It can rely on <pos> being a positive integer.
**
**  'Elm0vString' does the same thing as 'Elm0String', but it can
**  also rely on <pos> not being larger than the length of <list>.
**
**  'Elm0String'  is the function on 'Elm0ListFuncs'  for strings.
**  'Elm0vString' is the function in 'Elm0vListFuncs' for strings.
*/
Obj             Elm0String (
    Obj                 list,
    Int                 pos )
{
    if ( pos <= GET_LEN_STRING( list ) ) {
        return GET_ELM_STRING( list, pos );
    }
    else {
        return 0;
    }
}

Obj             Elm0vString (
    Obj                 list,
    Int                 pos )
{
    return GET_ELM_STRING( list, pos );
}


/****************************************************************************
**
*F  ElmString(<list>,<pos>) . . . . . . . . . . select an element of a string
*F  ElmvString(<list>,<pos>)  . . . . . . . . . select an element of a string
**
**  'ElmString' returns the element at the position <pos> of the string
**  <list>, or signals an error if <list> has no assigned object at <pos>.
**  It can rely on <pos> being a positive integer.
**
**  'ElmvString' does the same thing as 'ElmString', but it can
**  also rely on <pos> not being larger than the length of <list>.
**
**  'ElmwString' does the same thing as 'ElmString', but it can
**  also rely on <list> having an assigned object at <pos>.
**
**  'ElmString'  is the function in 'ElmListFuncs'  for strings.
**  'ElmfString' is the function in 'ElmfListFuncs' for strings.
**  'ElmwString' is the function in 'ElmwListFuncs' for strings.
*/
Obj             ElmString (
    Obj                 list,
    Int                 pos )
{
    /* check the position                                                  */
    if ( GET_LEN_STRING( list ) < pos ) {
        ErrorReturnVoid(
            "List Element: <list>[%d] must have an assigned value",
            (Int)pos, 0L,
            "you can return after assigning a value" );
        return ELM_LIST( list, pos );
    }

    /* return the selected element                                         */
    return GET_ELM_STRING( list, pos );
}

#define ElmvString      Elm0vString

#define ElmwString      Elm0vString


/****************************************************************************
**
*F  ElmsString(<list>,<poss>) . . . . . . . .  select a sublist from a string
**
**  'ElmsString' returns a new list containing the  elements at the positions
**  given   in  the  list   <poss> from   the  string   <list>.   It  is  the
**  responsibility of the called to ensure that  <poss> is dense and contains
**  only positive integers.  An error is signalled if an element of <poss> is
**  larger than the length of <list>.
**
**  'ElmsString' is the function in 'ElmsListFuncs' for strings.
*/
Obj             ElmsString (
    Obj                 list,
    Obj                 poss )
{
    Obj                 elms;         /* selected sublist, result        */
    Int                 lenList;        /* length of <list>                */
    Char                elm;            /* one element from <list>         */
    Int                 lenPoss;        /* length of <positions>           */
    Int                 pos;            /* <position> as integer           */
    Int                 inc;            /* increment in a range            */
    Int                 i;              /* loop variable                   */

    /* general code                                                        */
    if ( ! IS_RANGE(poss) ) {

        /* get the length of <list>                                        */
        lenList = LEN_PLIST( list );

        /* get the length of <positions>                                   */
        lenPoss = LEN_LIST( poss );

        /* make the result list                                            */
        elms = NEW_STRING( lenPoss );

        /* loop over the entries of <positions> and select                 */
        for ( i = 1; i <= lenPoss; i++ ) {

            /* get <position>                                              */
            pos = INT_INTOBJ( ELMW_LIST( poss, i ) );
            if ( lenList < pos ) {
                ErrorReturnVoid(
                    "List Elements: <list>[%d] must have an assigned value",
                    (Int)pos, 0L,
                    "you can return after assigning a value" );
                return ELMS_LIST( list, poss );
            }

            /* select the element                                          */
            elm = CSTR_STRING(list)[pos-1];

            /* assign the element into <elms>                              */
            CSTR_STRING(elms)[i-1] = elm;

        }

    }

    /* special code for ranges                                             */
    else {

        /* get the length of <list>                                        */
        lenList = LEN_PLIST( list );

        /* get the length of <positions>, the first elements, and the inc. */
        lenPoss = GET_LEN_RANGE( poss );
        pos = GET_LOW_RANGE( poss );
        inc = GET_INC_RANGE( poss );

        /* check that no <position> is larger than 'LEN_LIST(<list>)'      */
        if ( lenList < pos ) {
            ErrorReturnVoid(
                "List Elements: <list>[%d] must have an assigned value",
                (Int)pos, 0L,
                "you can return after assigning a value" );
            return ELMS_LIST( list, poss );
        }
        if ( lenList < pos + (lenPoss-1) * inc ) {
            ErrorReturnVoid(
                "List Elements: <list>[%d] must have an assigned value",
                (Int)(pos + (lenPoss-1) * inc), 0L,
                "you can return after assigning a value" );
            return ELMS_LIST( list, poss );
        }

        /* make the result list                                            */
        elms = NEW_STRING( lenPoss );

        /* loop over the entries of <positions> and select                 */
        for ( i = 1; i <= lenPoss; i++, pos += inc ) {

            /* select the element                                          */
            elm = CSTR_STRING(list)[pos-1];

            /* assign the element into <elms>                              */
            CSTR_STRING(elms)[i-1] = elm;

        }

    }

    /* return the result                                                   */
    return elms;
}


/****************************************************************************
**
*F  AssString(<list>,<pos>,<val>) . . . . . . . . . . . .  assign to a string
**
**  'AssString' assigns the value <val> to the  string <list> at the position
**  <pos>.   It is the responsibility  of the caller to  ensure that <pos> is
**  positive, and that <val> is not 0.
**
**  'AssString' is the function in 'AssListFuncs' for strings.
**
**  'AssString' simply converts the  string into a plain  list, and then does
**  the same stuff   as 'AssPlist'.  This is  because  a string  is  not very
**  likely to stay a string after the assignment.
**
*N  1996/06/11 mschoene this is the default and should probably not be here
*/
void            AssString (
    Obj                 list,
    Int                 pos,
    Obj                 val )
{
    /* convert the range into a plain list                                 */
    PLAIN_LIST( list );
    RetypeBag( list, T_PLIST );

    /* resize the list if necessary                                        */
    if ( LEN_PLIST(list) < pos ) {
        GROW_PLIST( list, pos );
        SET_LEN_PLIST( list, pos );
    }

    /* now perform the assignment and return the assigned value            */
    SET_ELM_PLIST( list, pos, val );
    CHANGED_BAG( list );
}

void            AssStringImm (
    Obj                 list,
    Int                 pos,
    Obj                 val )
{
    ErrorReturnVoid(
        "Lists Assignment: <list> must be a mutable list",
        0L, 0L,
        "you can return and ignore the assignment" );
}


/****************************************************************************
**
*F  AsssString(<list>,<poss>,<vals>)  . . assign several elements to a string
**
**  'AsssString' assignes the  values from the  list <vals> at the  positions
**  given in the list <poss> to the string  <list>.  It is the responsibility
**  of the caller to ensure that  <poss> is dense  and contains only positive
**  integers, that <poss> and <vals> have the same length, and that <vals> is
**  dense.
**
**  'AsssString' is the function in 'AsssListFuncs' for strings.
**
**  'AsssString' simply converts the string to a plain list and then does the
**  same stuff as 'AsssPlist'.  This is because a  string  is not very likely
**  to stay a string after the assignment.
*/
void            AsssString (
    Obj                 list,
    Obj                 poss,
    Obj                 vals )
{
    /* convert <list> to a plain list                                      */
    PLAIN_LIST( list );
    RetypeBag( list, T_PLIST );

    /* and delegate                                                        */
    ASSS_LIST( list, poss, vals );
}

void            AsssStringImm (
    Obj                 list,
    Obj                 poss,
    Obj                 val )
{
    ErrorReturnVoid(
        "Lists Assignments: <list> must be a mutable list",
        0L, 0L,
        "you can return and ignore the assignment" );
}


/****************************************************************************
**
*F  IsDenseString(<list>) . . . . . . .  dense list test function for strings
**
**  'IsDenseString' returns 1, since every string is dense.
**
**  'IsDenseString' is the function in 'IsDenseListFuncs' for strings.
*/
Int             IsDenseString (
    Obj                 list )
{
    return 1L;
}


/****************************************************************************
**
*F  IsHomogString(<list>) . . . .  homogeneous list test function for strings
**
**  'IsHomogString' returns  1 if  the string  <list>  is homogeneous.  Every
**  nonempty string is homogeneous.
**
**  'IsHomogString' is the function in 'IsHomogListFuncs' for strings.
*/
Int             IsHomogString (
    Obj                 list )
{
    return (0 < GET_LEN_STRING(list));
}


/****************************************************************************
**
*F  IsSSortString(<list>) . . . . . . . strictly sorted list test for strings
**
**  'IsSSortString'  returns 1 if the string  <list> is strictly sorted and 0
**  otherwise.
**
**  'IsSSortString' is the function in 'IsSSortListFuncs' for strings.
*/
Int             IsSSortString (
    Obj                 list )
{
    Int                 len;
    Int                 i;

    /* test whether the string is strictly sorted                          */
    len = GET_LEN_STRING( list );
    for ( i = 1; i < len; i++ ) {
        if ( ! (CSTR_STRING(list)[i] < CSTR_STRING(list)[i+1]) )
            break;
    }

    /* retype according to the outcome                                     */
    RetypeBag( list, (len <= i ? T_STRING_SSORT : T_STRING_NSORT)
                   + (IS_MUTABLE_OBJ(list) ? 0 : IMMUTABLE) );
    return (len <= i);
}

Int             IsSSortStringNot (
    Obj                 list )
{
    return 0L;
}

Int             IsSSortStringYes (
    Obj                 list )
{
    return 1L;
}


/****************************************************************************
**
*F  IsPossString(<list>)  . . . . .  positions list test function for strings
**
**  'IsPossString' returns 0, since every string contains no integers.
**
**  'IsPossString' is the function in 'TabIsPossList' for strings.
*/
Int             IsPossString (
    Obj                 list )
{
    return GET_LEN_STRING( list ) == 0;
}


/****************************************************************************
**
*F  PosString(<list>,<val>,<pos>) . . . .  position of an element in a string
**
**  'PosString' returns the position of the  value <val> in the string <list>
**  after the first position <start> as a C integer.   0 is returned if <val>
**  is not in the list.
**
**  'PosString' is the function in 'PosListFuncs' for strings.
*/
Int             PosString (
    Obj                 list,
    Obj                 val,
    Int                 start )
{
    Int                 lenList;        /* length of <list>                */
    Obj                 elm;          /* one element of <list>           */
    Int                 i;              /* loop variable                   */

    /* get the length of <list>                                            */
    lenList = GET_LEN_STRING( list );

    /* loop over all entries in <list>                                     */
    for ( i = start+1; i <= lenList; i++ ) {

        /* select one element from <list>                                  */
        elm = GET_ELM_STRING( list, i );

        /* compare with <val>                                              */
        if ( EQ( elm, val ) )
            break;

    }

    /* return the position (0 if <val> was not found)                      */
    return (lenList < i ? 0 : i);
}


/****************************************************************************
**
*F  PlainString(<list>) . . . . . . . . . .  convert a string to a plain list
**
**  'PlainString' converts the string <list> to a plain list.  Not much work.
**
**  'PlainString' is the function in 'PlainListFuncs' for strings.
*/
void            PlainString (
    Obj                 list )
{
    Int                 lenList;        /* logical length of the string    */
    Obj                 tmp;            /* handle of the list              */
    Int                 i;              /* loop variable                   */

    /* find the length and allocate a temporary copy                       */
    lenList = GET_LEN_STRING( list );
    tmp = NEW_PLIST( T_PLIST, lenList );
    SET_LEN_PLIST( tmp, lenList );

    /* create the finite field entries                                     */
    for ( i = 1; i <= lenList; i++ ) {
        SET_ELM_PLIST( tmp, i, GET_ELM_STRING( list, i ) );
    }

    /* change size and type of the string and copy back                    */
    ResizeBag( list, SIZE_OBJ(tmp) );
    RetypeBag( list, TYPE_OBJ(tmp) );
    SET_LEN_PLIST( list, lenList );
    for ( i = 1; i <= lenList; i++ ) {
        SET_ELM_PLIST( list, i, ELM_PLIST( tmp, i ) );
        CHANGED_BAG( list );
    }
}


/****************************************************************************
**
*F  IS_STRING(<obj>)  . . . . . . . . . . . . . test if an object is a string
**
**  'IS_STRING' returns 1  if the object <obj>  is a string  and 0 otherwise.
**  It does not change the representation of <obj>.
**
**  'IS_STRING' is defined in the declaration part of this package as follows
**
#define IS_STRING(obj)  ((*IsStringFuncs[ TYPE_OBJ( obj ) ])( obj ))
*/
Int             (*IsStringFuncs [LAST_REAL_TYPE+1]) ( Obj obj );

Obj             IsStringFilt;

Obj             IsStringHandler (
    Obj                 self,
    Obj                 obj )
{
    return (IS_STRING( obj ) ? True : False);
}

Int             IsStringNot (
    Obj                 obj )
{
    return 0;
}

Int             IsStringYes (
    Obj                 obj )
{
    return 1;
}

Int             IsStringList (
    Obj                 list )
{
    Int                 lenList;
    Obj                 elm;
    Int                 i;
    
    lenList = LEN_LIST( list );
    for ( i = 1; i <= lenList; i++ ) {
        elm = ELMV0_LIST( list, i );
        if ( elm == 0 || TYPE_OBJ( elm ) != T_CHAR )
            break;
    }

    return (lenList < i);
}

Int             IsStringListHom (
    Obj                 list )
{
    return (TYPE_OBJ( ELM_LIST(list,1) ) == T_CHAR);
}

Int             IsStringObject (
    Obj                 obj )
{
    return (DoFilter( IsStringFilt, obj ) != False);
}


/****************************************************************************
**
*F  ConvString(<string>)  . . . convert a string to the string representation
**
**  'ConvString' converts the string <list> to the string representation.
*/
Obj             ConvStringFunc;

void            ConvString (
    Obj                 string )
{
    Int                 lenString;      /* length of the string            */
    Obj                 elm;            /* one element of the string       */
    Int                 i;              /* loop variable                   */

    /* do nothing if the string is already in the string representation    */
    if ( T_STRING<=TYPE_OBJ(string) && TYPE_OBJ(string)<=T_STRING_SSORT ) {
        return;
    }

    /* convert the string to the string representation                     */
    /*N 1996/09/03 M.Schoenert it assumes the string rep. is more compact  */
    lenString = LEN_LIST( string );
    for ( i = 1; i <= lenString; i++ ) {
        elm = ELMW_LIST( string, i );
        CSTR_STRING(string)[i-1] = *((UChar*)ADDR_OBJ(elm));
    }
    CSTR_STRING(string)[lenString] = '\0';
    RetypeBag( string, T_STRING );
    ResizeBag( string, lenString+1 );
}

Obj             ConvStringHandler (
    Obj                 self,
    Obj                 string )
{
    /* check whether <string> is a string                                  */
    if ( ! IS_STRING( string ) ) {
        string = ErrorReturnObj(
            "ConvString: <string> must be a string (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(string)].name), 0L,
            "you can return a string for <string>" );
        return ConvStringHandler( self, string );
    }

    /* convert to the string representation                                */
    ConvString( string );

    /* return nothing                                                      */
    return 0;
}


/****************************************************************************
**
*F  IsStringConv(<obj>) . . . . . . test if an object is a string and convert
**
**  'IsStringConv'   returns 1  if   the object <obj>  is   a  string,  and 0
**  otherwise.   If <obj> is a  string it  changes  its representation to the
**  string representation.
*/
Obj             IsStringConvFilt;

Int             IsStringConv (
    Obj                 obj )
{
    Int                 res;

    /* test whether the object is a string                                 */
    res = IS_STRING( obj );

    /* if so, convert it to the string representation                      */
    /* NOTE that the empty list must not be converted into a string,       */
    /* so the string literal "" is the only empty list of type 'T_STRING'. */
    /* This is used in 'Print' to distinguish between empty strings (which */
    /* print nothing) and empty lists (which print as '[ ]').              */
    if ( res && LEN_LIST( obj ) != 0 ) {
        ConvString( obj );
    }

    /* return the result                                                   */
    return res;
}

Obj             IsStringConvHandler (
    Obj                 self,
    Obj                 obj )
{
    /* return 'true' if <obj> is a string and 'false' otherwise            */
    return (IsStringConv(obj) ? True : False);
}


/****************************************************************************
**
*F  InitString()  . . . . . . . . . . . . . . . .  initializes string package
**
**  'InitString' initializes the string package.
*/
void            InitString ( void )
{
    Int                 i;
    Int                 t1, t2;

    /* install the marking function                                        */
    InfoBags[           T_CHAR          ].name = "character";
    InitMarkFuncBags(   T_CHAR          , MarkNoSubBags );

    /* make all the character constants once and for all                   */
    for ( i = 0; i < 256; i++ ) {
        ObjsChar[i] = NewBag( T_CHAR, 1L );
        *(UChar*)ADDR_OBJ(ObjsChar[i]) = (UChar)i;
        InitGlobalBag( &ObjsChar[i] );
    }

    /* install the kind method                                             */
    InitCopyGVar( GVarName("KIND_CHAR"), &KIND_CHAR );
    KindObjFuncs[ T_CHAR ] = KindChar;

    /* install the character functions                                     */
    PrintObjFuncs[ T_CHAR ] = PrintChar;
    EqFuncs[ T_CHAR ][ T_CHAR ] = EqChar;
    LtFuncs[ T_CHAR ][ T_CHAR ] = LtChar;

    /* install the marking functions                                       */
    for ( t1 = T_STRING; t1 <= T_STRING_SSORT; t1 += 2 ) {
        InfoBags[         t1                     ].name
            = "list (string)";
        InitMarkFuncBags( t1                     , MarkOneSubBags );
        InfoBags[         t1          +IMMUTABLE ].name
            = "list (string)";
        InitMarkFuncBags( t1          +IMMUTABLE , MarkOneSubBags );
        InfoBags[         t1 +COPYING            ].name
            = "list (string), copied";
        InitMarkFuncBags( t1 +COPYING            , MarkOneSubBags );
        InfoBags[         t1 +COPYING +IMMUTABLE ].name
            = "list (string), copied";
        InitMarkFuncBags( t1 +COPYING +IMMUTABLE , MarkOneSubBags );
    }

    /* install the kind method                                             */
    for ( t1 = T_STRING; t1 <= T_STRING_SSORT; t1 += 2 ) {
        KindObjFuncs[ t1            ] = KindString;
        KindObjFuncs[ t1 +IMMUTABLE ] = KindString;
    }

    /* install the copy method                                             */
    for ( t1 = T_STRING; t1 <= T_STRING_SSORT; t1++ ) {
        CopyObjFuncs [ t1                     ] = CopyString;
        CopyObjFuncs [ t1          +IMMUTABLE ] = CopyString;
        CleanObjFuncs[ t1                     ] = CleanString;
        CleanObjFuncs[ t1          +IMMUTABLE ] = CleanString;
        CopyObjFuncs [ t1 +COPYING            ] = CopyStringCopy;
        CopyObjFuncs [ t1 +COPYING +IMMUTABLE ] = CopyStringCopy;
        CleanObjFuncs[ t1 +COPYING            ] = CleanStringCopy;
        CleanObjFuncs[ t1 +COPYING +IMMUTABLE ] = CleanStringCopy;
    }

    /* install the print method                                            */
    for ( t1 = T_STRING; t1 <= T_STRING_SSORT; t1 += 2 ) {
        PrintObjFuncs[ t1            ] = PrintString;
        PrintObjFuncs[ t1 +IMMUTABLE ] = PrintString;
    }

    /* install the comparison methods                                      */
    for ( t1 = T_STRING; t1 <= T_STRING_SSORT+IMMUTABLE; t1++ ) {
        for ( t2 = T_STRING; t2 <= T_STRING_SSORT+IMMUTABLE; t2++ ) {
            EqFuncs[ t1 ][ t2 ] = EqString;
            LtFuncs[ t1 ][ t2 ] = LtString;
        }
    }

    /* install the list methods                                            */
    for ( t1 = T_STRING; t1 <= T_STRING_SSORT; t1 += 2 ) {
        LenListFuncs    [ t1            ] = LenString;
        LenListFuncs    [ t1 +IMMUTABLE ] = LenString;
        IsbListFuncs    [ t1            ] = IsbString;
        IsbListFuncs    [ t1 +IMMUTABLE ] = IsbString;
        IsbvListFuncs   [ t1            ] = IsbvString;
        IsbvListFuncs   [ t1 +IMMUTABLE ] = IsbvString;
        Elm0ListFuncs   [ t1            ] = Elm0String;
        Elm0ListFuncs   [ t1 +IMMUTABLE ] = Elm0String;
        Elm0vListFuncs  [ t1            ] = Elm0vString;
        Elm0vListFuncs  [ t1 +IMMUTABLE ] = Elm0vString;
        ElmListFuncs    [ t1            ] = ElmString;
        ElmListFuncs    [ t1 +IMMUTABLE ] = ElmString;
        ElmvListFuncs   [ t1            ] = ElmvString;
        ElmvListFuncs   [ t1 +IMMUTABLE ] = ElmvString;
        ElmwListFuncs   [ t1            ] = ElmwString;
        ElmwListFuncs   [ t1 +IMMUTABLE ] = ElmwString;
        ElmsListFuncs   [ t1            ] = ElmsString;
        ElmsListFuncs   [ t1 +IMMUTABLE ] = ElmsString;
        AssListFuncs    [ t1            ] = AssString;
        AssListFuncs    [ t1 +IMMUTABLE ] = AssStringImm;
        AsssListFuncs   [ t1            ] = AsssString;
        AsssListFuncs   [ t1 +IMMUTABLE ] = AsssStringImm;
        IsDenseListFuncs[ t1            ] = IsDenseString;
        IsDenseListFuncs[ t1 +IMMUTABLE ] = IsDenseString;
        IsHomogListFuncs[ t1            ] = IsHomogString;
        IsHomogListFuncs[ t1 +IMMUTABLE ] = IsHomogString;
        IsSSortListFuncs[ t1            ] = IsSSortString;
        IsSSortListFuncs[ t1 +IMMUTABLE ] = IsSSortString;
        IsPossListFuncs [ t1            ] = IsPossString;
        IsPossListFuncs [ t1 +IMMUTABLE ] = IsPossString;
        PosListFuncs    [ t1            ] = PosString;
        PosListFuncs    [ t1 +IMMUTABLE ] = PosString;
        PlainListFuncs  [ t1            ] = PlainString;
        PlainListFuncs  [ t1 +IMMUTABLE ] = PlainString;
    }
    IsSSortListFuncs[ T_STRING_NSORT            ] = IsSSortStringNot;
    IsSSortListFuncs[ T_STRING_NSORT +IMMUTABLE ] = IsSSortStringNot;
    IsSSortListFuncs[ T_STRING_SSORT            ] = IsSSortStringYes;
    IsSSortListFuncs[ T_STRING_SSORT +IMMUTABLE ] = IsSSortStringYes;

    /* install the internal function                                       */
    for ( t1 = FIRST_REAL_TYPE; t1 <= LAST_REAL_TYPE; t1++ ) {
        IsStringFuncs[ t1 ] = IsStringNot;
    }
    for ( t1 = FIRST_LIST_TYPE; t1 <= LAST_LIST_TYPE; t1++ ) {
        IsStringFuncs[ t1 ] = IsStringList;
    }
    for ( t1 = T_STRING; t1 <= T_STRING_SSORT; t1++ ) {
        IsStringFuncs[ t1 ] = IsStringYes;
    }
    for ( t1 = FIRST_EXTERNAL_TYPE; t1 <= LAST_EXTERNAL_TYPE; t1++ ) {
        IsStringFuncs[ t1 ] = IsStringObject;
    }
    IsStringFilt = NewFilterC(
        "IS_STRING", 1L, "obj", IsStringHandler );
    AssGVar( GVarName( "IS_STRING" ), IsStringFilt );

    ConvStringFunc = NewFunctionC(
        "CONV_STRING", 1L, "string", ConvStringHandler );
    AssGVar( GVarName( "CONV_STRING" ), ConvStringFunc );

    IsStringConvFilt = NewBag( TYPE_OBJ(IsStringFilt),
                               SIZE_OBJ(IsStringFilt) );
    for ( i = 0; i < SIZE_OBJ(IsStringFilt)/sizeof(Obj); i++ ) {
        ADDR_OBJ(IsStringConvFilt)[i] = ADDR_OBJ(IsStringFilt)[i];
    }
    HDLR_FUNC(IsStringConvFilt,1) = IsStringConvHandler;
    AssGVar( GVarName( "IS_STRING_CONV" ), IsStringConvFilt );
}




