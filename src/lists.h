/****************************************************************************
**
*A  lists.h                     GAP source                   Martin Schoenert
**
*H  @(#)$Id$
**
*Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
**
**  This file declares the functions of the generic list package.
**
**  This package provides a uniform   interface to the functions that  access
**  lists and their elements  for the other packages  in the GAP kernel.  For
**  example, 'ExecFor' can loop over the elements  in a list using the macros
**  'LEN_LIST' and 'ELM_LIST' independently of the type of the list.
*/
#ifdef  INCLUDE_DECLARATION_PART
char *          Revision_lists_h =
   "@(#)$Id$";
#endif


/****************************************************************************
**
*F  IS_LIST(<obj>)  . . . . . . . . . . . . . . . . . . . is an object a list
*V  IsListFuncs[<type>] . . . . . . . . . . . . . . . . . table for list test
**
**  'IS_LIST' returns a nonzero value if  the object <obj> is a list and zero
**  otherwise.
**
**  Note that 'IS_LIST'  is a macro,  so do not  call it with arguments  that
**  have sideeffects.
**
**  A package implementing  an ordinary list type <type>   must set the  flag
**  'IsListFlag[<type>]'  for  this type to '1'.   A  package  implementing a
**  vector type must set  it to '2'.  A  package implementing an matrix  type
**  must set it to '3'.
*/
#define IS_LIST(obj)    ((*IsListFuncs[ TYPE_OBJ( obj ) ])( obj ))

extern  Int             (*IsListFuncs [LAST_REAL_TYPE+1]) ( Obj obj );


/****************************************************************************
**
*F  LEN_LIST(<list>)  . . . . . . . . . . . . . . . . . . .  length of a list
*V  LenListFuncs[<type>]  . . . . . . . . . . . . . table of length functions
**
**  'LEN_LIST' returns the logical length of the list <list>  as a C integer.
**  An error is signalled if <list> is not a list.
**
**  Note that  'LEN_LIST' is a  macro, so do  not call it with arguments that
**  have sideeffects.
**
**  A package  implementing a list type <type>  must  provide such a function
**  and install it in 'LenListFuncs[<type>]'.
*/
#define LEN_LIST(list)  ((*LenListFuncs[ TYPE_OBJ(list) ])( list ))

extern  Int             (*LenListFuncs[LAST_REAL_TYPE+1]) ( Obj list );


/****************************************************************************
**
*F  ISB_LIST(<list>,<pos>)  . . . . . . . . . .  test for element from a list
*F  ISBV_LIST(<list>,<pos>) . . . . . . . . . .  test for element from a list
*V  IsbListFuncs[<type>]  . . . . . . . . . . . . . . table of test functions
*V  IsbvListFuncs[<type>] . . . . . . . . . . . . . . table of test functions
**
**  'ISB_LIST' returns 1  if the list <list>  has an entry at  position <pos>
**  and 0 otherwise.  An error is signalled  if <list> is not  a list.  It is
**  the  responsibility of  the  caller to  ensure that  <pos> is a  positive
**  integer.
**
**  'ISBV_LIST'  does the same as  'ISB_LIST', but the caller also guarantees
**  that <list> ist a list and that <pos> is less than or equal to the length
**  of <list>.
**
**  Note that 'ISB_LIST' and 'ISBV_LIST are macros, so  do not call them with
**  arguments that have sideeffects.
**
**  A  package implementing a  list type <type>  must  provide a function for
**  'ISB_LIST' and install it in 'IsbListFuncs[<type>]'.
**
**  A package  implementing  a list type  <type> must  provide a function for
**  'ISBV_LIST' and  install  it in 'IsbvListFuncs[<type>]'.   This  function
**  need not  test whether  <pos> is less  than   or equal to the   length of
**  <list>.
*/
#define ISB_LIST(list,pos) \
                        ((*IsbListFuncs[TYPE_OBJ(list)])(list,pos))

#define ISBV_LIST(list,pos) \
                        ((*IsbvListFuncs[TYPE_OBJ(list)])(list,pos))

extern  Int             (*IsbListFuncs[LAST_REAL_TYPE+1]) ( Obj list, Int pos );

extern  Int             (*IsbvListFuncs[LAST_REAL_TYPE+1]) ( Obj list, Int pos );


/****************************************************************************
**
*F  ELM0_LIST(<list>,<pos>) . . . . . . . . . . select an element from a list
*F  ELMV0_LIST(<list>,<pos>)  . . . . . . . . . select an element from a list
*V  Elm0ListFuncs[<type>] . . . . . . . . . . .  table of selection functions
*V  Elmv0ListFuncs[<type>]  . . . . . . . . . .  table of selection functions
**
**  'ELM0_LIST' returns the element at the position <pos> in the list <list>,
**  or 0 if <list>  has no assigned  object at position  <pos>.  An  error is
**  signalled if <list>  is  not a list.  It   is the responsibility   of the
**  caller to ensure that <pos> is a positive integer.
**
**  'ELMV0_LIST' does the same as 'ELM0_LIST', but the caller also guarantees
**  that <list> is a list and that <pos> is less thatn or equal to the length
**  of <list>.
**
**  Note that  'ELM0_LIST' and 'ELMV0_LIST'  are macros, so  do not call them
**  with arguments that have sideeffects.
**
**  A package  implementing a  list type <type>  must provide  a function for
**  'ELM0_LIST' and install it in 'Elm0ListFuncs[<type>]'.
**
**  A package implementing  a lists type  <type> must provide a function  for
**  'ELMV0_LIST' and install it  in 'Elmv0ListFuncs[<type>]'.   This function
**  need not test   whether <pos> is less  than   or equal to  the  length of
**  <list>.
*/
#define ELM0_LIST(list,pos) \
                        ((*Elm0ListFuncs[TYPE_OBJ(list)])(list,pos))

#define ELMV0_LIST(list,pos) \
                        ((*Elm0vListFuncs[TYPE_OBJ(list)])(list,pos))

extern  Obj             (*Elm0ListFuncs[LAST_REAL_TYPE+1]) ( Obj list, Int pos );

extern  Obj             (*Elm0vListFuncs[LAST_REAL_TYPE+1]) ( Obj list, Int pos );


/****************************************************************************
**
*F  ELM_LIST(<list>,<pos>)  . . . . . . . . . . select an element from a list
*F  ELMV_LIST(<list>,<pos>) . . . . . . . . . . select an element from a list
*F  ELMW_LIST(<list>,<pos>) . . . . . . . . . . select an element from a list
*V  ElmListFuncs[<type>]  . . . . . . . . . . .  table of selection functions
*V  ElmvListFuncs[<type>] . . . . . . . . . . .  table of selection functions
*V  ElmwListFuncs[<type>] . . . . . . . . . . .  table of selection functions
**
**  'ELM_LIST' returns the element at the position  <pos> in the list <list>.
**  An  error is signalled if  <list> is not a list,  if <pos> is larger than
**  the length of <list>, or if <list>  has no assigned  object at <pos>.  It
**  is the responsibility  of the caller to  ensure that <pos>  is a positive
**  integer.
**
**  'ELMV_LIST' does  the same as 'ELM_LIST', but  the caller also guarantees
**  that <list> is a list and that <pos> is less  than or equal to the length
**  of <list>.
**
**  'ELMW_LIST' does the same as 'ELMV_LIST', but  the caller also guarantees
**  that <list> has an assigned object at the position <pos>.
**
**  Note that 'ELM_LIST', 'ELMV_LIST', and  'ELMW_LIST' are macros, so do not
**  call them with arguments that have sideeffects.
**
**  A package implementing a  list  type <type> must  provide a  function for
**  'ELM_LIST' and install it  in 'ElmListFuncs[<type>]'.  This function must
**  signal an error if <pos> is larger than the length of <list> or if <list>
**  has no assigned object at <pos>.
**
**  A package implementing  a list  type  <type> must provide a  function for
**  'ELMV_LIST' and  install  it  in 'ElmvListFuncs[<type>]'.   This function
**  need not check that <pos> is less than or equal to  the length of <list>,
**  but it must signal an error if <list> has no assigned object at <pos>.
**
**  A package implementing a  list type  <type>  must provide a  function for
**  'ELMW_LIST' and install them  in 'ElmwListFuncs[<type>]'.  This  function
**  need not check that <pos> is  less than or equal  to the length of <list>
**  or that <list> has an assigned object at <pos>.
*/
#define ELM_LIST(list,pos) \
                        ((*ElmListFuncs[TYPE_OBJ(list)])(list,pos))

#define ELMV_LIST(list,pos) \
                        ((*ElmvListFuncs[TYPE_OBJ(list)])(list,pos))

#define ELMW_LIST(list,pos) \
                        ((*ElmwListFuncs[TYPE_OBJ(list)])(list,pos))

extern  Obj             (*ElmListFuncs[LAST_REAL_TYPE+1]) ( Obj list, Int pos );

extern  Obj             (*ElmvListFuncs[LAST_REAL_TYPE+1]) ( Obj list, Int pos );

extern  Obj             (*ElmwListFuncs[LAST_REAL_TYPE+1]) ( Obj list, Int pos );


/****************************************************************************
**
*F  ELMS_LIST(<list>,<poss>)  . . . . . . select several elements from a list
*V  ElmsListFuncs[<type>] . . . . . . . . . . .  table of selection functions
**
**  'ELMS_LIST' returns a  new list containing the  elements at the positions
**  given in the list <poss> from the list <list>.  An  error is signalled if
**  <list> is not a list, if  any of the positions  is larger than the length
**  of  <list>, or if <list> has  no assigned object at any of the positions.
**  It is the responsibility of the  caller to ensure  that <poss> is a dense
**  list of positive integers.
**
**  Note that 'ELMS_LIST' is a  macro, so do not call it with arguments  that
**  have sideeffects.
**
**  A package implementing a list  type <type> must  provide such a  function
**  and install it in 'ElmsListFuncs[<type>]'.  This  function must signal an
**  error if any of the positions  is larger than the length  of <list> or if
**  <list>  has no assigned object at any of the positions  (and thus it will
**  always return a dense list).  It *must* create a new list, even if <poss>
**  is equal to  '[1..Length(<list>)]', 'EvalElmListLevel' depends on this so
**  that it can call 'ElmListLevel', which  overwrites this new list.  If the
**  result is a list of lists, then it also *must* create a new list that has
**  the same representation as a plain list.
*/
#define ELMS_LIST(list,poss) \
                        ((*ElmsListFuncs[TYPE_OBJ(list)])(list,poss))

extern  Obj             (*ElmsListFuncs[LAST_REAL_TYPE+1]) ( Obj list, Obj poss );

extern  Obj             ElmsListDefault (
            Obj                 list,
            Obj                 poss );


/****************************************************************************
**
*F  UNB_LIST(<list>,<pos>)  . . . . . . . . . . .  unbind element from a list
*V  UnbListFuncs[<type>]  . . . . . . . . . . . . . table of unbind functions
**
**  'UNB_LiST' unbinds the element at the position <pos> in the list  <list>.
**  Note that the unbinding may change the length of  the  representation  of
**  <list>.  An error is signalled if  <list>  is  not  a  list.  It  is  the
**  responsibility of the caller to ensure that <pos> is a positive integer.
**
**  Note that 'UNB_LIST' is a macro, so do not call it  with  arguments  that
**  have sideeffects.
**
**  A package implementing a list type <type> must provide  such  a  function
**  and install it in 'UnbListFuncs[<type>]'.  This function must change  the
**  representation of <list> to that of a plain list if necessary.
*/
#define UNB_LIST(list,pos) \
                        ((*UnbListFuncs[TYPE_OBJ(list)])(list,pos))

extern void             (*UnbListFuncs[LAST_REAL_TYPE+1]) ( Obj list, Int pos );


/****************************************************************************
**
*F  ASS_LIST(<list>,<pos>,<obj>)  . . . . . . . . assign an element to a list
*V  AssListFuncs[<type>]  . . . . . . . . . . . table of assignment functions
**
**  'ASS_LIST' assigns the object <obj> to the list <list> at position <pos>.
**  Note that  the assignment may change  the length or the representation of
**  <list>.  An error   is signalled if  <list>  is not a  list.    It is the
**  responsibility of the caller to ensure that <pos>  is a positive integer,
**  and that <obj> is not 0.
**
**  Note that 'ASS_LIST' is a macro,  so do not  call it with arguments  that
**  have sideeffects.
**
**  A package  implementing a list type  <type> must provide  such a function
**  and   install it in  'AssListFuncs[<type>]'.   This  function must extend
**  <list> if <pos> is larger than the length of  <list> and must also change
**  the representation of <list> to that of a plain list if necessary.
*/
#define ASS_LIST(list,pos,obj) \
                        ((*AssListFuncs[TYPE_OBJ(list)])(list,pos,obj))

extern  void            (*AssListFuncs[LAST_REAL_TYPE+1]) ( Obj list, Int pos, Obj obj );


/****************************************************************************
**
*F  ASSS_LIST(<list>,<poss>,<objs>) . . . . assign several elements to a list
*V  AsssListFuncs[<type>] . . . . . . . . . . .  table of assignment function
**
**  'ASSS_LIST'  assignes the objects from  the list  <objs> at the positions
**  given in the  list <poss> to the list  <list>.  Note that  the assignment
**  may  change  the length or  the  representation of  <list>.   An error is
**  signalled if  <list> is  not  a list.   It  is the  responsibility of the
**  caller to ensure that  <poss> is a dense  list  of positive  integers and
**  that <objs> is a dense list of the same length as <poss>.
**
**  Note that 'ASSS_LIST' is a macro, so do not call it with  arguments  that
**  have sideeffects.
**
**  A package implementing  a list type <type> must  provide  such a function
**  and install it in 'AsssListFuncs[<type>]'.  This function must extend the
**  <list> if any of the  positions is larger than the  length of <list>  and
**  must also change the representation of <list> to that  of a plain list if
**  necessary.
*/
#define ASSS_LIST(list,poss,objs) \
                        ((*AsssListFuncs[TYPE_OBJ(list)])(list,poss,objs))

extern  void            (*AsssListFuncs[LAST_REAL_TYPE+1]) (Obj list, Obj poss, Obj objs);

extern  void            AsssListDefault (
            Obj                 list,
            Obj                 poss,
            Obj                 objs );


/****************************************************************************
**
*F  IS_DENSE_LIST(<list>) . . . . . . . . . . . . . . .  test for dense lists
*V  IsDenseListFuncs[<type>]  . . . . . .  table of dense list test functions
**
**  'IS_DENSE_LIST'  returns 1 if the   list <list> is a   dense  list and  0
**  otherwise, i.e., if either <list> is not a list, or if it is not dense.
**
**  Note that  'IS_DENSE_LIST' is a macro, so  do not call it  with arguments
**  that have sideeffects.
**
**  A package  implementing a list type  <type> must provide such  a function
**  and  install it in  'IsDenseListFuncs[<type>]'.   This function must loop
**  over the list and test for holes, unless  the type of the list guarantees
**  already that the list is dense (e.g. for sets).
*/
#define IS_DENSE_LIST(list) \
                        ((*IsDenseListFuncs[TYPE_OBJ(list)])(list))

extern  Int             (*IsDenseListFuncs[LAST_REAL_TYPE+1]) ( Obj list );

extern  Int             IsDenseListDefault (
            Obj                 list );


/****************************************************************************
**
*F  IS_HOMOG_LIST(<list>) . . . . . . . . . . . .  test for homogeneous lists
*V  IsHomogListFuncs[<type>]  . . .  table of homogeneous list test functions
**
**  'IS_HOMOG_LIST' returns 1 if  the list <list>  is a  homogeneous list and
**  0 otherwise, i.e., if either <list> is not  a  list,  or  if  it  is  not
**  homogeneous.
**
**  'IS_HOMOG_LIST' is a macro, so do not call it with  arguments  that  have
**  sideeffects.
**
**  A  package implementing a list  type <type> must  provide such a function
**  and install  it  in 'IsHomogListFuncs[<type>]'.  This function  must loop
**  over the list   and test whether all  elements  lie in  the  same family,
**  unless  the type  of   the list  guarantees    already that the  list  is
**  homogeneous (e.g. for sets).
*/
#define IS_HOMOG_LIST(list) \
                        ((*IsHomogListFuncs[TYPE_OBJ(list)])(list))

extern  Int             (*IsHomogListFuncs[LAST_REAL_TYPE+1]) ( Obj list );

extern  Int             IsHomogListDefault (
            Obj                 list );


/****************************************************************************
**
*F  IS_TABLE_LIST(<list>) . . . . . . . . . . . . . . .  test for table lists
*V  IsTableListFuncs[<type>]  . . . . . .  table of table list test functions
**
**  'IS_TABLE_LIST'  returns  1 if  the  list  <list>  is  a  table, i.e.,  a
**  homogeneous list of homogeneous lists of equal length, and 0 otherwise.
**
**  'IS_TABLE_LIST' is a macro, so do not call it with  arguments  that  have
**  sideeffects.
**
**  A  package implementing a list  type <type> must  provide such a function
**  and install it in  'IsTableListFuncs[<type>]'.   This function must  loop
**  over the list and test whether  all elements lie  in the same family, are
**  homogenous lists, and have  the same length, unless the  type of the list
**  guarantees already that the list has this property.
*/
#define IS_TABLE_LIST(list) \
                        ((*IsTableListFuncs[TYPE_OBJ(list)])(list))

extern  Int             (*IsTableListFuncs[LAST_REAL_TYPE+1]) ( Obj list );

extern  Int             IsTableListDefault (
            Obj                 list );


/****************************************************************************
**
*F  IS_SSORT_LIST(<list>) . . . . . . . . . .  test for strictly sorted lists
*V  IsSSortListFuncs[<type>]  .  table of strictly sorted list test functions
**
**  'IS_SSORT_LIST' returns 2 if the list <list> is  a strictly  sorted  list
**  and 0 otherwise,  i.e., if either <list>  is not a list,  or if it is not
**  strictly sorted.
**
**  'IS_SSORT_LIST' is a macro, so do not call it  with arguments  that  have
**  sideeffects.
**
**  A  package implementing a  list type <type>  must provide such a function
**  and install it  in  'IsSSortListFuncs[<type>]'.  This function must  loop
**  over the list and compare each element with the next one, unless the type
**  of the list guarantees already that the list is strictly sorted.
*/
#define IS_SSORT_LIST(list) \
                        ((*IsSSortListFuncs[TYPE_OBJ(list)])(list))

extern  Int             (*IsSSortListFuncs[LAST_REAL_TYPE+1]) ( Obj list );

extern  Int             IsSSortListDefault (
            Obj                 list );


/****************************************************************************
**
*F  IsSSortListProp
*/
extern Obj IsSSortListProp;


/****************************************************************************
**
*F  IsNSortListProp
*/
extern Obj IsNSortListProp;


/****************************************************************************
**
*F  IS_POSS_LIST(<list>)  . . . . . . . . . . . . .  test for positions lists
*V  IsPossListFuncs[<type>] . . . . . . table of positions list test function
**
**  'IS_POSS_LIST' returns  1 if the list  <list> is  a dense list containing
**  only positive  integers and 0 otherwise, i.e.,  if either <list> is not a
**  list, or if it is not dense,  or if it contains  an element that is not a
**  positive integer.
**
**  Note that  'IS_POSS_LIST' is a macro,  so  do not  call it with arguments
**  that have sideeffects.
**
**  A package  implementing a list type  <type> must provide such  a function
**  and install  it  in 'IsPossListFuncs[<type>]'.   This function  must loop
**  over the list  and  test for holes   and elements that are  not  positive
**  integers, unless the type of the list guarantees already that the list is
**  acceptable (e.g. a range with positive <low> and <high> values).
*/
#define IS_POSS_LIST(list) \
                        ((*IsPossListFuncs[TYPE_OBJ(list)])(list))

extern  Int             (*IsPossListFuncs[LAST_REAL_TYPE+1]) ( Obj list );

extern  Int             IsPossListDefault (
            Obj                 list );


/****************************************************************************
**
*F  POS_LIST(<list>,<obj>,<start>)  . . . . . . . . find an element in a list
*V  PosListFuncs[<type>]  . . . . . . . . . . .  table of searching functions
**
**  'POS_LIST' returns  the  position of the  first  occurence of  the object
**  <obj>,  which may be an object of any type, in the  list <list> after the
**  position  <start> as a C  integer.  0 is returned if  <obj> is not in the
**  list after <start>.  An error is signalled if <list> is not a list.
**
**  Note that 'POS_LIST'  is a macro,  so do  not call it with arguments that
**  have sideeffects.
**
**  A package implementing a list  type <type> must  provide such a  function
**  and install it in 'PosListFuncs[<type>]'.
*/
#define POS_LIST(list,obj,start) \
                        ((*PosListFuncs[TYPE_OBJ(list)])(list,obj,start))

extern  Int             (*PosListFuncs[LAST_REAL_TYPE+1]) (Obj list, Obj obj, Int start);

extern  Int             PosListDefault (
            Obj                 list,
            Obj                 obj,
            Int                 start );


/****************************************************************************
**
*F  ElmListLevel(<lists>,<pos>,<level>) . . . . . . . . . . . . . . . . . . .
*F  . . . . . . . . . . . . .  select an element of several lists in parallel
**
**  'ElmListLevel' assigns to '<lists>[<p_1>][<p_2>]...[<p_level>]' the value
**  '<lists>[<p_1>][<p_2>]...[<p_level>][<pos>]' for all  appropriate  tuples
**  of positions <p_1>,<p_2>,...,<p_level>.  An error is signalled if for any
**  tuple of positions  '<list> = <lists>[<p_1>][<p_2>]...[<p_level>]' is not
**  a list, <pos>  is larger  than  the length of <list>,   or <list> has  no
**  assigned object  at <pos>.   It is the   responsibility of the  caller to
**  ensure that <pos> is a positive integer.
**
**  It  is also  the responsibility of   the caller  to ensure  that <lists>,
**  '<lists>[<p_1>]', ...,   '<lists>[<p_1>][<p_2>]...[<p_level-1>]'  are all
**  dense lists with the same representation as plain lists.  Usually <lists>
**  is the   result of <level>  nested applications   of 'ELMS_LIST',  so  we
**  require 'ELMS_LIST' (resp.  the   functions implementing 'ELMS_LIST')  to
**  satisfy this requirements.
*/
extern  void            ElmListLevel (
            Obj                 lists,
            Int                 pos,
            Int                 level );


/****************************************************************************
**
*F  ElmsListLevel(<lists>,<poss>,<level>) . . . . . . . . . . . . . . . . . .
*F  . . . . . . . . . .  select several elements of several lists in parallel
**
**  'ElmsListLevel'    assigns  to '<lists>[<p_1>][<p_2>]...[<p_level>]'  the
**  objects '<lists>[<p_1>][<p_2>]...[<p_level>]{<poss>}' for all appropriate
**  tuples of positions <p_1>,<p_2>,...,<p_level>.   An error is signalled if
**  for any tuple of positions '<list> = <lists>[<p_1>][<p_2>]...[<p_level>]'
**  is not a list, any  of the positions of  <poss> is larger than the length
**  of <list>, or <list> has  no assigned object at any of the positions.  It
**  is also the responsibility of the caller to ensure that <poss> is a dense
**  list of positive integers.
**
**  It  is  also the  responsibility  of the  caller  to ensure that <lists>,
**  '<lists>[<p_1>]',  ...,   '<lists>[<p_1>][<p_2>]...[<p_level-1>]' are all
**  dense lists with the same representation as plain lists.  Usually <lists>
**  is the result   of  <level> nested applications   of 'ELMS_LIST',  so  we
**  require 'ELMS_LIST' (resp.  the   functions implementing 'ELMS_LIST')  to
**  satisfy this requirements.
*/
extern  void            ElmsListLevel (
            Obj                 lists,
            Obj                 poss,
            Int                 level );


/****************************************************************************
**
*F  AssListLevel(<lists>,<pos>,<objs>,<level>)  . . . . . . . . . . . . . . .
*F  . . . . . . . . . . . . .  assign an element to several lists in parallel
**
**  'AssListLevel' assigns to    '<lists>[<p_1>][<p_2>]...[<p_level>][<pos>]'
**  the value '<objs>[<p_1>][<p_2>]...[<p_level>]' for all appropriate tuples
**  of positions <p_1>,<p_2>,...,<p_level>.  An error is signalled if for any
**  tuple of positions '<list>  = <lists>[<p_1>][<p_2>]...[<p_level>]' is not
**  a list, '<obj> =  <objs>[<p_1>][<p_2>]...[<p_i-1>]' is not a  dense list,
**  or  <obj> has not  the same length as '<list>[<p_1>][<p_2>]...[<p_i-1>]'.
**  It is the responsibility of the caller to ensure that <pos> is a positive
**  integer.
**
**  It is  also  the responsibility of   the caller  to  ensure that <lists>,
**  '<lists>[<p_1>]',   ...,  '<lists>[<p_1>][<p_2>]...[<p_level-1>]' are all
**  dense lists with the same representation as plain lists.  Usually <lists>
**  is the  result of   <level> nested applications  of  'ELMS_LIST',   so we
**  require  'ELMS_LIST'  (resp.  the functions implementing  'ELMS_LIST') to
**  satisfy this requirements.
*/
extern  void            AssListLevel (
            Obj                 lists,
            Int                 pos,
            Obj                 objs,
            Int                 level );


/****************************************************************************
**
*F  AsssListLevel(<lists>,<poss>,<objs>,<level>)  . . . . . . . . . . . . . .
*F  . . . . . . . . . .  assign several elements to several lists in parallel
**
**  'AsssListLevel'  assigns to '<lists>[<p_1>][<p_2>]...[<p_level>]{<poss>}'
**  the  objects  '<objs>[<p_1>][<p_2>]...[<p_level>]' for  all   appropriate
**  tuples of positions <p_1>,<p_2>,...,<p_level>.   An error is signalled if
**  for any tuple of positions '<list> = <lists>[<p_1>][<p_2>]...[<p_level>]'
**  is not a list, '<obj> = <objs>[<p_1>][<p_2>]...[<p_i-1>]'  is not a dense
**  list, <obj> has not the same length as '<list>[<p_1>][<p_2>]...[<p_i-1>]'
**  or  '<objs>[<p_1>][<p_2>]...[<p_level>]' is not a  dense list of the same
**  length as <poss>.  It is the responsibility  of the caller to ensure that
**  <poss> is a dense list of positive integers.
**
**  It is also the   responsibility of the caller   to ensure that   <lists>,
**  '<lists>[<p_1>]',    ..., '<lists>[<p_1>][<p_2>]...[<p_level-1>]' are all
**  dense lists with the same representation as plain lists.  Usually <lists>
**  is the   result  of <level> nested applications   of   'ELMS_LIST', so we
**  require 'ELMS_LIST' (resp.   the functions  implementing 'ELMS_LIST')  to
**  satisfy this requirements.
*/
extern  void            AsssListLevel (
            Obj                 lists,
            Obj                 poss,
            Obj                 objs,
            Int                 lev );


/****************************************************************************
**
*F  PLAIN_LIST(<list>)  . . . . . . . . . . .  convert a list to a plain list
*V  PlainListFuncs[<type>]  . . . . . . . . . . table of conversion functions
**
**  'PLAIN_LIST' changes  the representation of the  list <list> to that of a
**  plain list Note that the type of <list> need not be 'T_PLIST' afterwards,
**  it could also be 'T_SET' or 'T_VECTOR'.  An  error is signalled if <list>
**  is not a list.
**
**  Note that 'PLAIN_LIST' is a macro, so do not call  it with arguments that
**  have sideeffects.
**
**  A package implementing a  list type <type>  must provide such  a function
**  and install it in 'PlainListFuncs[<type>]'.
*/
#define PLAIN_LIST(list) \
                        ((*PlainListFuncs[TYPE_OBJ(list)])(list))

extern  void            (*PlainListFuncs[LAST_REAL_TYPE+1]) ( Obj list );


/****************************************************************************
**
*F  XType(<obj>)  . . . . . . . . . . . . . . . .  extended type of an object
*F  IS_XTYPE_LIST(<type>,<obj>) . . . . . . . . . . .  test for extended type
*V  IsXTypeListFuncs[<type>]  . . . . . table of extended type test functions
**
**  'XType' returns  the extended type of  the object <obj>.   For everything
**  except objects of type 'T_PLIST' and 'T_SET' this is just the type of the
**  object.  For objects of type 'T_PLIST' and 'T_SET', 'XType'  examines the
**  object closer and returns  'T_VECTOR', 'T_VECFFE', 'T_BLIST', 'T_STRING',
**  'T_RANGE',  'T_MATRIX', 'T_MATFFE', and  'T_LISTX'.  As  a sideeffect the
**  object <obj> is converted into  the representation  of the extended  type,
**  e.g., if  'XType' returns 'T_MATFFE', <obj>  is converted into a  list of
**  vectors over  a  common  finite field.  'XType'  is   used by the  binary
**  operations  functions for lists to decide   to which function they should
**  dispatch.  'T_LISTX'  is the extended  type of otherwise untypable lists.
**  The  only operation defined  for such lists  is the product with a scalar
**  (where 'PROD( <list>[<pos>], <scl> )'  decides whether the multiplication
**  is allowed or not).
**
**  A package implementing    a    list  type    <type>  must    provide    a
**  tester/conversion function  and install it in 'IsXTypeListFuncs[<type>]'.
**  This function should accept a list <list>  (with a representation that is
**  equivalent  to  that of a  plain  list),  and  return 1 if  <list>  could
**  successfully be converted to the type <type> and 0 otherwise.
*/
#define IS_XTYPE_LIST(t,obj) \
                        ((*IsXTypeListFuncs[t])(obj))

extern  Int             (*IsXTypeListFuncs[LAST_VIRTUAL_TYPE+1]) ( Obj obj );

extern  Int             XType (
            Obj                 obj );


/****************************************************************************
**
*F  KINDS_LIST_FAM(<fam>) . . . . . . .  list of kinds of lists over a family
*/
extern  Obj             KINDS_LIST_FAM (
            Obj                 fam );


/****************************************************************************
**
*F  InitLists() . . . . . . . . . . . . . initialize the generic list package
**
**  'InitLists' initializes the generic list package.
*/
extern  void            InitLists ( void );



