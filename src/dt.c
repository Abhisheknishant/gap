/****************************************************************************
**
** Deep Thought deals with trees.  A tree < tree > is a concatenation of 
** several nodes where each node is a 7-tuple of immediate integers.  If
** < tree > is an atom it contains only one node,  thus it is itself a
** 7-tuple. If < tree > is not an atom we obtain its list representation by
**
**  < tree >  :=  topnode(<tree>) concat left(<tree>) concat right(<tree>) .
**
** Let us denote the i-th node of <tree> by (<tree>, i)  and the tree rooted by
** (<tree>, i) by tree(<tree>, i).  Let <a> be tree(<tree>, i)
** The first entry of (<tree>, i) is pos(a),
** and the second entry is num(a). The third entry of (<tree>, i) gives a mark.
** (<tree>, i)[3] = 1  means that (<tree>, i) is marked,  (<tree>, i)[3] = 0
** means that (<tree>, i) is not marked.  The fourth entry of the node tells,
** if the corresponding polynomial to <a> has to be calculated later.
** (<tree>, i)[4] = 1  means that the polynamial has to be calculated,
** (<tree>, i)[4] = 0  means that the polynomial has not to be calculated.
** If <a> is not an Atom then the fifth entry of (<tree>, i) tells us which
** node of <tree> is rooting right(<a>). In this case (<tree>, i>) = j
** means that the (i+j)-th node of <tree> is rooting right(<a>).  If <a> is
** an atom then (<tree>, i)[5] is either 0 or -1. (<tree>, i)[5] = 0 then
** means that <a> is an atom from the Right-hand word,  and (<tree, i) = -1
** means that <a> is an atom from the Left-hand word.  The sixth entry of
** (<tree>, i) gives the number of nodes of <a>.  The seventh entry of the
** node finally gives us a boundary for pos(<a>).  (<tree>, i)[7] <= 0
** means that pos(<a>) is unbound.
*/






#include       "system.h"
#include       "scanner.h"
#include       "gasman.h"
#include       "objects.h"
#include       "bool.h"
#include       "calls.h"
#include       "gap.h"
#include       "gvars.h"
#include       "plist.h"
#include       "lists.h"
#include       "listfunc.h"
#include       "precord.h"
#include       "records.h"
#include       "integer.h"
#include       "dt.h"


/***************************************************************************
**
*F  DT_POS(tree, index) . . . . . . . . . . . . . . position of (<tree>, index)
**
**  'DT_POS' returns pos(<a>) where <a> is the subtree of <tree> rooted by
**  (<tree>, index).  <index> has to be a positive integer less or equal than
**  the number of nodes of <tree>.
*/
#define  DT_POS(tree, index) \
              (ELM_PLIST(tree, (index-1)*5 + 1 ) ) 


/**************************************************************************
**
*F  SET_DT_POS(tree, index, obj) . . . . assign the position of(<tree>, index)
**
**  'SET_DT_POS sets pos(<a>) to the object <obj>, where <a> is the subtree
**  of <tree>,  rooted by (<tree>, index).  <index> has to be an positive
**  integer less or equal to the number of nodes of <tree>
*/
#define  SET_DT_POS(tree, index, obj) \
              (SET_ELM_PLIST(tree, (index-1)*5 + 1, obj) )


/***************************************************************************
**
*F  DT_GEN(tree, index) . . . . . . . . . . . . . generator of (<tree>, index)
**
**  'DT_GEN' returns num(<a>) where <a> is the subtree of <tree> rooted by
**  (<tree>, index).  <index> has to be a positive integer less or equal than
**  the number of nodes of <tree>.
*/
#define  DT_GEN(tree, index) \
              (ELM_PLIST(tree, (index-1)*5 + 2) )


/**************************************************************************
**
*F  SET_DT_GEN(tree, index, obj) . . . assign the generator of(<tree>, index)
**
**  'SET_DT_GEN sets num(<a>) to the object <obj>, where <a> is the subtree
**  of <tree>,  rooted by (<tree>, index).  <index> has to be an positive
**  integer less or equal to the number of nodes of <tree>
*/
#define  SET_DT_GEN(tree, index, obj) \
              (SET_ELM_PLIST(tree, (index-1)*5 + 2, obj) )


/**************************************************************************
**
*F  DT_IS_MARKED(tree, index) . . . . . . tests if (<tree>, index) is marked
**
**  'DT_IS_MARKED' returns 1 (as C integer) if (<tree>, index) is marked, and
**  0 otherwise.  <index> has to be a positive integer less or equal to the
**  number of nodes of <tree>.
*/
#define  DT_IS_MARKED(tree, index)  \
             (INT_INTOBJ (ELM_PLIST(tree, (index-1)*5 + 3) ) )


/**************************************************************************
**
*F  DT_MARK(tree, index) . . . . . . . . . . . . . . . . . . . . mark a node
**
**  'DT_MARK' marks the node (<tree>, index). <index> has to be a positive
**  integer less or equal to the number of nodes of <tree>.
*/
#define  DT_MARK(tree, index) \
              (SET_ELM_PLIST(tree, (index-1)*5 + 3, INTOBJ_INT(1) ) )


/**************************************************************************
**
*F  DT_UNMARK(tree, index) . . . . . . . . . . . remove the mark from a node
**
**  'DT_UNMARK' removes the mark from the node (<tree>, index). <index> has 
**  has to be a positive integer less or equal to the number of nodes of
**  <tree>.
*/
#define  DT_UNMARK(tree, index) \
              (SET_ELM_PLIST(tree, (index-1)*5 + 3, INTOBJ_INT(0) ) )


/****************************************************************************
**
*F  DT_RIGHT(tree, index) . . . .determine the right subnode of (<tree>, index)
*F  DT_LEFT(tree, index) . . . . determine the left subnode of (<tree>, index)
**
**  'DT_RIGHT' returns the right subnode of (<tree>, index).  That means if
**  DT_RIGHT(tree, index) = index2,  then (<tree>, index2) is the right
**  subnode of (<tree>, index).
**
**  'DT_LEFT' returns the left subnode of (<tree>, index).  That means if
**  DT_LEFT(tree, index) = index2,  then (<tree>, index2) is the left
**  subnode of (<tree>, index).
**
**  Before calling 'DT_RIGHT' or 'DT_LEFT' it should be ensured,  that 
**  (<tree>, index) is not an atom.  <index> has to be a positive integer
**  less or equal to the number of nodes of <tree>.
*/
#define  DT_RIGHT(tree, index) \
              ( INT_INTOBJ(ELM_PLIST(tree, index*5 + 4) ) + index + 1)
#define  DT_LEFT(tree, index) \
              ( index + 1 )


/****************************************************************************
**
*F  DT_SIDE(tree, index) . . . . . . . determine the side of (<tree>, index)
*V  RIGHT. . . . . . . . . . . . . . . integer describing "right"
*V  LEFT . . . . . . . . . . . . . . . integer describing "left"
**
**  'DT_SIDE' returns 'LEFT' if (<tree>, index) is an atom from the Left-hand
**  word,  and 'RIGHT'  if (<tree>, index) is an atom of the Right-hand word.
**  Otherwise 'DT_SIDE' returns an integer bigger than 1.  <index> has to be
**  a positive integer less or equal to the number of nodes of <tree>.
*/
#define  RIGHT                  -1
#define  LEFT                   -2
#define  DT_SIDE(tree, index) \
              (INT_INTOBJ( ELM_PLIST(tree, (index-1)*5 + 5 ) )  )


/****************************************************************************
**
*F  DT_LENGTH(tree, index) . . . . . . . . number of nodes of (<tree>, index)
**
**  'DT_LENGTH' returns the number of nodes of (<tree>, index).  <index> has
**  to be a positive integer less or equal to the number of nodes of <tree>.
*/
#define  DT_LENGTH(tree, index) \
              ( INT_INTOBJ(ELM_PLIST(tree, (index-1)*5 + 4) )  )


/***************************************************************************
**
*F  DT_MAX(tree, index) . . . . . . . . . . . . . . . . boundary of a node
**
**  'DT_MAX(tree, index)' returns a boundary for 'DT_POS(tree, index)'.
**  'DT_MAX(tree, index) = 0 ' means that 'DT_POS(tree, index)' is unbound.
**  <index> has to be a positive integer less or equal to the number of nodes
**  of tree.
*/
#define  DT_MAX(tree, index) \
              (ELM_PLIST(tree, (index-1)*5 + 5 ) )


/****************************************************************************
**
*F  CELM(list, pos) . . . . . . . . . . element of a plain list as C integer
**
**  'CELM' returns the <pos>-th element of the plain list <list>.  <pos> has
**  to be a positive integer less or equal to the physical length of <list>.
**  Before calling 'CELM' it should be ensured that the <pos>-th entry of
**  <list> is an immediate integer object.
*/
#define  CELM(list, pos)         ( INT_INTOBJ(ELM_PLIST(list, pos) ) )



/***************************************************************************
**
*V  bas
*V  exp
**
**  'bas' and 'exp' are the record component names for the relations of the
**  pc-presentations.
*/
static int    bas, exp;
Obj                    Dt_add;
extern Obj             ShallowCopyPlist( Obj  list );

/****************************************************************************
**
*F  UnmarkTree( <tree> ) . . . . . . . remove the marks of all nodes of <tree>
**
**  'UnmarkTree' removes all marks of all nodes of the tree <tree>.
*/
void  UnmarkTree(
		  Obj   tree )
{
    int     i; /*  loop variable                     */

    for (i=1; i <= DT_LENGTH(tree, 1); i++ )
        DT_UNMARK(tree, i);
}


/****************************************************************************
**
*F  FuncUnmarkTree(<self>, <tree>) . . remove the marks of all nodes of <tree>
**
**  'FuncUnmarkTree' implements the internal function 'UnmarkTree'.
**
**  'UnmarkTree( <tree> )'
**
**  'UnmarkTree' removes all marks of all nodes of the tree <tree>.
*/
Obj  FuncUnmarkTree(
		     Obj  self,
		     Obj  tree   )
{
    UnmarkTree(tree);
    return  0;
}


/*****************************************************************************
**
*F  Mark(<tree>, <reftree>, <index>) . . . . . . . . find all nodes of <tree> 
**                                                   which are almost equal
**                                                   to (<reftree>, index)
**
**  'Mark' determines all nodes of the tree <tree>, rooting subtrees almost
**  equal to the tree rooted by (<reftree>, index).  'Mark' marks these nodes
**  and returns the number of different nodes among these nodes.  Since it
**  is assumed that the set {pos(a) | a almost equal to (<reftree>, index) }
**  is equal to {1,...,n} for a positive integer n,  'Mark' actually returns
**  the Maximum of {pos(a) | a almost equal to (<reftree>, index)}.
*/
int   Mark(
	    Obj   tree,
	    Obj   reftree,
	    int   index  )
{
    int  i, /*  loop variable                    */
         m; /*  integer to return                */

    m = 0;
    i = 1;
    while ( i <= DT_LENGTH(tree, 1) )
    {
        /*  skip all nodes (<tree>, i) with 
        **  num(<tree>, i) > num(<reftree>, index)     */
        while( i < DT_LENGTH(tree, 1) && 
               DT_GEN(tree, i)  >  DT_GEN(reftree, index) )
	    i++;
	if ( AlmostEqual(tree, i, reftree, index) )
	{
	    DT_MARK(tree, i);
	    if ( m < INT_INTOBJ( DT_POS(tree, i) )  )
	        m = INT_INTOBJ( DT_POS(tree, i) );
	}
	/*  Since num(a) < num(b) holds for all subtrees <a> of an arbitrary
	**  tree <b> we can now skip the whole tree rooted by (<tree>, i).
        **  If (<tree>, i) is the left subnode of another node we can even
        **  skip the tree rooted by that node,  because of 
        **  num( right(a) )  <  num( left(a) ) for all trees <a>.
        **  Note that (<tree>, i) is the left subnode of another node,  if and
        **  only if the previous node (<tree>, i-1) is not an atom. in this
        **  case (<tree>, i) is the left subnode of (<tree>, i-1).          */
	if ( DT_LENGTH(tree, i-1) == 1 )
	    /*   skip the tree rooted by (<tree>, i).                    */
	    i = i + DT_LENGTH(tree, i);
	else
	    /*   skip the tree rooted by (<tree>, i-1)                   */
	    i = i - 1 + DT_LENGTH(tree, i-1);
    }
    return m;
}


/****************************************************************************
**
*F  AmostEqual(<tree1>,<index1>,<tree2>,<index2>) . . test of almost equality
**
**  'AlmostEqual' tests if tree(<tree1>, index1) is almost equal to
**  tree(<tree2>, index2).  'AlmostEqual' returns 1
**  if these trees are almost equal,  and 0 otherwise.  <index1> has to be
**  a positive integer less or equal to the number of nodes of <tree1>,
**  and <index2> has to be a positive integer less or equal to the number of
**  nodes of <tree2>.
*/
int     AlmostEqual(
		     Obj    tree1,
		     int    index1,
		     Obj    tree2,
		     int    index2    )
{
    int   k; /*   loop variable                                             */
    /*  First the two top nodes of tree(<tree1>, index1) and
    **  tree(<tree2>, index2) (that are (<tree1>, index1) and 
    **  (<tree2, index2) ) are compared by testing the equality of the 2-nd,
    **  5-th and 6-th entries the nodes.                                    */
    if ( DT_GEN(tree1, index1) != DT_GEN(tree2, index2) )
        return  0;
    if ( DT_SIDE(tree1, index1) != DT_SIDE(tree2, index2)  )
        return  0;
    if ( DT_LENGTH(tree1, index1) != DT_LENGTH(tree2, index2)  )
        return  0;
    /*  For the comparison of the remaining nodes of tree(<tree1>, index1)
    **  and tree(<tree2>, index2) it is also necessary to compare the first
    **  entries of the nodes.  Note that we know at this point,  that 
    **  tree(<tree1>, index1) and tree(<tree2>, index2) have the same number
    **  of nodes                                                             */
    for (k = index1 + 1;  k < index1 + DT_LENGTH(tree1, index1);  k++ )
    {
        if ( DT_GEN(tree1, k) != DT_GEN(tree2, k + index2 - index1 ) )
	    return  0;
	if ( DT_POS(tree1, k) != DT_POS(tree2, k + index2 - index1 ) )
	    return  0;
	if ( DT_SIDE(tree1, k)    !=
	     DT_SIDE(tree2, k + index2 - index1)  )
	    return  0;
	if ( DT_LENGTH(tree1, k) != DT_LENGTH(tree2, k + index2 - index1) )
	    return  0;
    }
    return  1;
}


/*****************************************************************************
**
*F  Equal(<tree1>,<index1>,<tree2>,<index2>) . . . . . . . . test of equality
**
**  'Equal' tests if tree(<tree1>, index1) is equal to
**  tree(<tree2>, index2).  'Equal' returns 1
**  if these trees are  equal,  and 0 otherwise.  <index1> has to be
**  a positive integer less or equal to the number of nodes of <tree1>,
**  and <index2> has to be a positive integer less or equal to the number of
**  nodes of <tree2>.
*/
int     Equal(
	       Obj     tree1,
	       int     index1,
	       Obj     tree2,
	       int     index2   )
{
    int   k; /*  loop variable                                               */

    /*  Each node of tree(<tree1>, index1) is compared to the corresponding
    **  node of tree(<tree2>, index2) by testing the equality of the 1-st,
    **  2-nd,  5-th and 6-th nodes.                                          */
    for (k=index1; k < index1 + DT_LENGTH(tree1, index1);  k++)
    {
        if ( DT_GEN(tree1, k) != DT_GEN(tree2, k + index2 - index1 ) )
	    return  0;
	if ( DT_POS(tree1, k) != DT_POS(tree2, k + index2 - index1 ) )
	    return  0;
	if ( DT_SIDE(tree1, k)   !=
	     DT_SIDE(tree2, k + index2 - index1)   )
	    return  0;
	if ( DT_LENGTH(tree1, k) != DT_LENGTH(tree2, k + index2 - index1) )
	    return  0;
    }
    return  1;
}


/****************************************************************************
**
*F  Mark2(<tree>,<index1>,<reftree>,<index2>) . . find all subtrees of
**                                                tree(<tree>, index1) which
**                                                are almost equal to
**                                                tree(<reftree>, index2)
**
**  'Mark2' determines all subtrees of tree(<tree>, index1) that are almost
**  equal to tree(<reftree>, index2).  'Mark2' marks the top nodes of these
**  trees and returns a list of lists <list> such that <list>[i]
**  for each subtree <a> of <tree> which is  almost equal to
**  tree(<reftree>, index2) and for which pos(<a>) = i holds contains an
**  integer describing the position of the top node of <a> in <tree>.
**  For example <list>[i] = [j, k] means that tree(<tree>, j) and
**  tree(<tree>, k) are almost equal to tree(<reftree>, index2) and
**  that pos(tree(<tree>, j) = pos(tree(<tree>, k) = i holds.
**
**  <index1> has to be a positive integer less or equal to the number of nodes
**  of <tree>,  and <index2> has to be a positive integer less or equal to
**  the number of nodes of <reftree>.
*/
Obj    Mark2(
	      Obj        tree,
	      int        index1,
	      Obj        reftree,
	      int        index2   )
{
    int    i; /*  loop variable                                          */
    Obj    new, 
           list; /*  list to return                                      */

    /*  initialize <list>                                                 */
    list = NEW_PLIST(T_PLIST, 0);
    SET_LEN_PLIST(list, 0);
    i = index1;
    while( i < index1 + DT_LENGTH(tree, index1) )
    {
        /*  skip all nodes (<tree>, i) with 
        **  num(<tree>, i) > num(<reftree>, index)     */
        while( i < index1 + DT_LENGTH(tree, index1) - 1    &&
	       DT_GEN(tree, i) > DT_GEN(reftree, index2)   )
	    i++;
	if ( AlmostEqual(tree, i, reftree, index2) )
	{
	    DT_MARK(tree, i);
	    /*  if <list> is too small grow it appropiately               */
	    if ( LEN_PLIST(list) < INT_INTOBJ( DT_POS(tree, i) )  )
	    {
	        GROW_PLIST(list, INT_INTOBJ( DT_POS(tree, i) ) );
		SET_LEN_PLIST(list, INT_INTOBJ( DT_POS(tree, i) )  );
	    }
	    /*  if <list> has no entry at position pos(tree(<tree>, i))
            **  create a new list <new>,  assign it to list at position
            **  pos(tree(<tree>, i)),  and add i to <new>                  */
	    if ( ELM_PLIST(list, INT_INTOBJ( DT_POS(tree, i) )  )  ==  0)
	    {
	        new = NEW_PLIST( T_PLIST, 1);
		SET_LEN_PLIST(new, 1);
		SET_ELM_PLIST(new, 1, INTOBJ_INT(i) );
		SET_ELM_PLIST(list, INT_INTOBJ( DT_POS(tree, i) ),  new);
		/*  tell gasman that list has changed                      */
		CHANGED_BAG(list);
	    }
            /*  add i to <list>[ pos(tree(<tree>, i)) ]                    */ 
            else
	    {
	        new = ELM_PLIST(list, INT_INTOBJ( DT_POS(tree, i) )  );
		GROW_PLIST(new, LEN_PLIST(new) + 1);
		SET_LEN_PLIST(new, LEN_PLIST(new) + 1);
		SET_ELM_PLIST(new, LEN_PLIST(new), INTOBJ_INT(i) );
		/*  tell gasman that new has changed                         */
		CHANGED_BAG(new);
	    }
	}
	/*  Since num(a) < num(b) holds for all subtrees <a> of an arbitrary
	**  tree <b> we can now skip the whole tree rooted by (<tree>, i).
        **  If (<tree>, i) is the left subnode of another node we can even
        **  skip the tree rooted by that node,  because of 
        **  num( right(a) )  <  num( left(a) ) for all trees <a>.
        **  Note that (<tree>, i) is the left subnode of another node,  if and
        **  only if the previous node (<tree>, i-1) is not an atom. In this
        **  case (<tree>, i) is the left subnode of (<tree>, i-1).          */
	if ( DT_LENGTH(tree, i-1) == 1 )
	    /*  skip tree(<tree>, i)                                        */
	    i = i + DT_LENGTH(tree, i);
	else
	    /*  skip tree(<tree>, i-1)                                      */
	    i = i - 1 + DT_LENGTH(tree, i-1);
    }
    return  list;
}


/*****************************************************************************
**
*F  FindTree(<tree>, <index>)
**
**  'FindTree' looks for a subtree <a> of tree(<tree>, index) such that 
**  the top node of
**  <a> is not marked but all the other nodes of <a> are marked.  It is
**  assumed that if the top node of a subtree <b> of tree(<tree>, index) 
**  is marked,  all
**  nodes of of <b> are marked.  Hence it suffices to look for a subtree <a>
**  of <tree> such that the top node of <a> is unmarked and the left and the
**  right node of <a> are marked.  'FindTree' returns an integer <i> such 
**  that tree(<tree> ,i) has the properties mentioned above.  If such a tree
**  does not exist 'Findtree' returns 0 (as C integer).  Note that this holds
**  if and only if tree(<tree>, index) is marked.
*/
int    FindTree(
		 Obj     tree,
		 int     index )
{
    int   i; /*     loop variable                                       */

    /*  return 0 if (<tree>, index) is marked                           */
    if ( DT_IS_MARKED(tree, index) )
        return  0;
    i = index;
    /*  loop over all nodes of tree(<tree>, index) to find a tree with the
    **  properties described above.                                       */
    while( i < index + DT_LENGTH(tree, index)  )
    {
        /*  skip all nodes that are unmarked and rooting non-atoms        */
        while( !( DT_IS_MARKED(tree, i) )  &&  DT_LENGTH(tree, i) > 1  )
	    i++;
	/*  if (<tree>, i) is unmarked we now know that tree(<tree>, i) is
        **  an atom and we can return i.  Note that an unmarked atom has the
        **  desired properties.                                             */
	if ( !( DT_IS_MARKED(tree, i) )  )
	    return  i;
        /*  go to the previous node                                          */
	i--;
        /*  If the right node of tree(<tree>, i) is marked return i.
        **  Else go to the right node of tree(<tree>, i).                    */
	if  ( DT_IS_MARKED(tree, DT_RIGHT(tree, i) )  )
	    return   i;
	i = DT_RIGHT(tree, i);
    }
}


/****************************************************************************
**
*F  MakeFormulaVector(<tree>, <pr>) . . . . . . . . . compute the polynomial 
**                                                    g_<tree> for <tree>
**
**  'MakeFormulaVector' returns the polynomial g_<tree> for a tree <tree>
**  and a pc-presentation <pr> of a nilpotent group.  This polynomial g_<tree>
**  is a product of binomial coefficients with a coefficient c.  A detailed
**  description of g_<tree> can be found in the diploma thesis of Wolfgang
**  Merkwitz or in the paper of C. Leedham-Green and L. H. Soicher.  The
**  polynomial g_<tree> is represented in this implementation a formula
**  vector,  which is a list of integers.  The first entry of the formula
**  vector is 0,  to distinguish formula vectors from trees.  The second
**  entry is the coefficient c,  and the third and fourth entries are
**  num( left(tree) ) and num( right(tree) ).  The remaining part of the
**  formula vector is a concatenation of pairs of integers.  A pair (i, j)
**  with i > 0 represents binomial(x_i, j).  A pair (0, j) represents
**  binomial(y_gen, j) when word*gen^power is calculated.
**
**  For the calculation of the coefficient c the top node of <tree> is ignored
**  because it can happen that trees are equal exept for the top node.
**  Hence it suffices to compute the formula vector for one of these trees.
**  Then we get the "correct" coefficient for the polynomial for each <tree'>
**  of those trees by multiplying the coefficient given by the formula vector
**  with c_( num(left(<tree'>)),  num(right(<tree'>));  num(<tree'>) ).  This
**  is also the reason for storing num(left(<tree>)) and num(right(<tree>))
**  in the formula vector.
**
**  'MakeFormulaVector' only returns correct results if all nodes of <tree>
**  are unmarked.
*/
Obj    MakeFormulaVector(
			  Obj    tree,
			  Obj    pr   )
{
    int   i, /*    denominator of a binomial coefficient              */
          j, /*    loop variable                                      */
          u; /*    node index                                         */
    Obj   rel, /*  stores relations of <pr>                           */
          vec, /*  stores formula vector to return                    */
          prod;/*  stores the product of two integers                 */

    /*  initialize <vec> and set the first four elements              */
    vec = NEW_PLIST(T_PLIST, 4);
    SET_LEN_PLIST(vec, 4);
    SET_ELM_PLIST(vec, 1, INTOBJ_INT(0) );
    SET_ELM_PLIST(vec, 2, INTOBJ_INT(1) );
    SET_ELM_PLIST(vec, 3, DT_GEN(tree, DT_LEFT(tree, 1) )  );
    SET_ELM_PLIST(vec, 4, DT_GEN(tree, DT_RIGHT(tree, 1) )  );
    /*  loop over all almost equal classes of subtrees of <tree> exept for
    **  <tree> itself.                                                    */
    u = FindTree(tree, 1);
    while( u > 1 )
    {
        /*  mark all subtrees of <tree> almost equal to tree(<tree>, u) and
        **  get the number of different trees in this almost equal class    */
        i = Mark(tree, tree, u);
	/*  if tree(<tree>, u) is an atom from the Right-hand word append
        **  [ 0, i ] to <vec>                                               */
	if  ( DT_SIDE(tree, u) == RIGHT )
	{
	    GROW_PLIST(vec, LEN_PLIST(vec)+2);
	    SET_LEN_PLIST(vec, LEN_PLIST(vec)+2);
	    SET_ELM_PLIST(vec, LEN_PLIST(vec)-1, INTOBJ_INT(0) );
	    SET_ELM_PLIST(vec, LEN_PLIST(vec), INTOBJ_INT(i) );
	}
	/*  if tree(<tree>, u) is an atom from the Left-hand word append
        **  [ num(tree(<tree>, u)), i ] to <vec>                            */
	else if  ( DT_SIDE(tree, u) == LEFT)
	{
	    GROW_PLIST(vec, LEN_PLIST(vec)+2);
	    SET_LEN_PLIST(vec, LEN_PLIST(vec)+2);
	    SET_ELM_PLIST(vec, LEN_PLIST(vec)-1, DT_GEN(tree, u) );
	    SET_ELM_PLIST(vec, LEN_PLIST(vec), INTOBJ_INT(i) );
	}
	/*  if tree(<tree>, u) is not an atom multiply 
        **  <vec>[2] with binomial(d, i) where
        **  d = c_(num(left(<tree>,u)), num(right(<tree>,u)); num(<tree>,u)) */
	else
	{
	    j = 3;
	    rel = ELM_PLIST( ELM_PLIST(pr, INT_INTOBJ( DT_GEN(tree, 
                                                        DT_LEFT(tree, u) ) ) ),
			     INT_INTOBJ( DT_GEN(tree, DT_RIGHT(tree, u) ) )  );
	    while ( 1  )
	    {
	        if ( ELM_PLIST(rel, j) == DT_GEN(tree, u)  )
		{
		    prod = ProdInt(ELM_PLIST(vec, 2),
				   binomial(ELM_PLIST(rel, j+1), 
					    INTOBJ_INT(i)        )        );
		    SET_ELM_PLIST(vec,  2, prod);
                    /*  tell gasman that vec has changed                     */
		    CHANGED_BAG(vec);
		    break;
		}
		j++;j++;
	    }
	}
	u = FindTree(tree, 1);
    }
    return vec;
}


/**************************************************************************
**
*F  FuncMakeFormulaVector(<self>,<tree>,<pr>) . . . . . compute the formula 
**                                                      vector for <tree>
**
**  'FuncMakeFormulaVector' implements the iternal function
**  'MakeFormulaVector(<tree>, <pr>)'.
**
**  'MakeFormulaVector(<tree>, <pr>)'
**
**  'MakeFormulaVector' returns the formula vector for the tree <tree> and
**  the pc-presentation <pr>.
*/
Obj    FuncMakeFormulaVector(
			      Obj      self,
			      Obj      tree,
			      Obj      pr            )
{
    if  (LEN_PLIST(tree) == 5)
        ErrorReturnVoid("<tree> has to be a non-atom", 0L, 0L,
			"you can return");
    return  MakeFormulaVector(tree, pr);
}


/*****************************************************************************
**
*F  binomial(<n>, <k>) . . . . . . . . . binomial coefficient of <n> and <k>
**
**  'binomial' returns the binomial coefficient of the integers <n> and <k>.
**  This function is identical to the gap library function 'Binomial'.
*/
Obj  binomial(
	       Obj     n,
	       Obj     k    )
{
    int  j;
    Obj  bin,i;

    if   ( LtInt(k, INTOBJ_INT(0) ) )
        bin = INTOBJ_INT(0);
    else if ( EqInt(k, INTOBJ_INT(0) ) )
        bin = INTOBJ_INT(1);
    else if ( LtInt(n, INTOBJ_INT(0)) )
        bin = ProdInt( PowInt( INTOBJ_INT(-1), k),
		       binomial( DiffInt( DiffInt(k, n), INTOBJ_INT(1) ), k ));
    else if ( LtInt(n, k) )
        bin = INTOBJ_INT(0);
    else if ( EqInt(n, k) )
        bin = INTOBJ_INT(1);
    else if ( LtInt( DiffInt(n, k), k) )
        bin = binomial(n, DiffInt(n, k) );
    else
    {
        bin = INTOBJ_INT(1);
	j = 1;
	i = SumInt( DiffInt(n, k), INTOBJ_INT(1) );
	while ( !( LtInt(n, i) )  )
	{
	    bin = ProdInt(bin, i);
	    while( j <= INT_INTOBJ(k)  &&  ModInt( bin, INTOBJ_INT(j) )  ==
		                           INTOBJ_INT(0)                    )
	    {
	        bin = QuoInt(bin, INTOBJ_INT(j) );
		j = j+1;
	    }
	    i = SumInt(i, INTOBJ_INT(1) );
	}
    }
    return  bin;
}


/****************************************************************************
**
*F  Leftof(<tree1>,<index1>,<tree2>,<index2>) . . . . test if one tree is left
**                                                    of another tree
**
**  'Leftof' returns 1 if tree(<tree1>, index1) is left of tree(<tree2>,index2)
**  in the word being collected at the first instance,  that 
**  tree(<tree1>, index1) and tree(<tree2>, index2) both occur. It is assumed
**  that tree(<tree1>, index1) is not equal to tree(<tree2>, index2).  'Leftof'
**  is described in the paper of C. R. Leedham-Green and L. H. Soicher and in 
**  the diploma thesis of Wolfgang Merkwitz.
*/
int     Leftof(
	        Obj     tree1,
	        int     index1,
	        Obj     tree2,
	        int     index2    )
{
    if  ( DT_LENGTH(tree1, index1) ==  1  &&  DT_LENGTH(tree2, index2) == 1  )
        if (DT_SIDE(tree1, index1) == LEFT && DT_SIDE(tree2, index2) == RIGHT)
	    return  1;
        else if  (DT_SIDE(tree1, index1) == RIGHT  &&
		  DT_SIDE(tree2, index2) == LEFT         )
	    return  0;
        else if (DT_GEN(tree1, index1) == DT_GEN(tree2, index2)  )
	    return ( DT_POS(tree1, index1) < DT_POS(tree2, index2) );
        else
	    return ( DT_GEN(tree1, index1) < DT_GEN(tree2, index2) );
    if  ( DT_LENGTH(tree1, index1) > 1                         &&  
	  DT_LENGTH(tree2, index2) > 1                         &&
	  Equal( tree1, DT_RIGHT(tree1, index1) , 
                 tree2, DT_RIGHT(tree2, index2)    )                    )
        if  ( Equal( tree1, DT_LEFT(tree1, index1),
                     tree2, DT_LEFT(tree2, index2)  )     )
	    if  ( DT_GEN(tree1, index1) == DT_GEN(tree2, index2)  )
	        return   ( DT_POS(tree1, index1) < DT_POS(tree2, index2) );
            else
	        return   ( DT_GEN(tree1, index1) < DT_GEN(tree2, index2) );
    if( Earlier(tree1, index1, tree2, index2)  )
        return  !Leftof2( tree2, index2, tree1, index1);
    else
        return  Leftof2( tree1, index1, tree2, index2);
}
     
  
/*****************************************************************************
**
*F  Leftof2(<tree1>,<index1>,<tree2>,<index2>) . . . . . test if one tree is
**                                                       left of another tree
**
**  'Leftof2' returns 1 if tree(<tree1>, index1) is left of 
**  tree(<tree2>,index2)in the word being collected at the first instance,  
**  that tree(<tree1>, index1) and tree(<tree2>, index2) both occur.  It is
**  assumed that tree(<tree2>, index2) occurs earlier than 
**  tree(<tree1>,index1).  Furthemore it is assumed that if both
**  tree(<tree1>, index1) and tree(<tree2>, index2) are non-atoms,  then their
**  right trees and their left trees are not equal.  'Leftof2' is the second
**  part of the function leftof described in the works mentioned in the
**  comment to 'Leftof'.
*/
int    Leftof2(
	        Obj    tree1,
	        int    index1,
	        Obj    tree2,
	        int    index2     )
{
    if  ( DT_GEN(tree2, index2) < DT_GEN(tree1, DT_RIGHT(tree1, index1) )  )
        return  0;
    else if  (Equal(tree1, DT_RIGHT(tree1, index1), tree2, index2 )  )
        return  0;
    else if  (DT_GEN(tree2, index2) == DT_GEN(tree1, DT_RIGHT(tree1, index1)) )
        return  Leftof(tree1, DT_RIGHT(tree1, index1), tree2, index2 );
    else if  (Equal(tree1, DT_LEFT(tree1, index1), tree2, index2) )
        return  0;
    else
        return  Leftof(tree1, DT_LEFT(tree1, index1), tree2, index2);
}


/****************************************************************************
**
*F  Earlier(<tree1>,<index1>,<tree2>,<index2>) . . . test if one tree occurs
**                                                   earlier than another
**
**  'Earlier' returns 1 if tree(<tree1>, index1) occurs strictly earlier than
**  tree(<tree2>, index2).  It is assumed that at least one of these trees
**  is a non-atom. Furthermore it is assumed that if both of these trees are
**  non-atoms,  right(tree(<tree1>, index1) ) does not equal
**  right(tree(<tree2>, index2) ) or left(tree(<tree1>, index1) ) does not
**  equal left(tree(<tree2>, index2) ). 'Earlier is described in the paper
**  of  C. R. Leedham-Green and L. H. Soicher.
*/
int    Earlier(
	        Obj    tree1,
	        int    index1,
	        Obj    tree2,
	        int    index2         )
{
    if  ( DT_LENGTH(tree1, index1) == 1 )
        return  1;
    if  ( DT_LENGTH(tree2, index2) == 1 )
        return  0;
    if ( Equal(tree1, DT_RIGHT(tree1, index1), 
               tree2, DT_RIGHT(tree2, index2)  ) )
        return Leftof(tree1, DT_LEFT(tree2, index2),
                      tree2, DT_LEFT(tree1, index1)  );
    if  ( DT_GEN(tree1, DT_RIGHT(tree1, index1) )  ==
	  DT_GEN(tree2, DT_RIGHT(tree2, index2) )            )
        return  Leftof( tree1, DT_RIGHT(tree1, index1) ,
		        tree2, DT_RIGHT(tree2, index2)      );
    return  (DT_GEN(tree1, DT_RIGHT(tree1, index1) )   <
	     DT_GEN(tree2, DT_RIGHT(tree2, index2) )      );
}


void    GetPols( 
		Obj    list,
		Obj    pr,
		Obj    pols     )
{
    Obj    lreps,
           rreps,
           tree,
           tree1;
    void   GetReps(),
           FindNewReps2();
    UInt   i,j,k,l;

    lreps = NEW_PLIST(T_PLIST, 2);
    rreps = NEW_PLIST(T_PLIST, 2);
    SET_LEN_PLIST(lreps, 0);
    SET_LEN_PLIST(rreps, 0);
    GetReps( ELM_PLIST(list, 1), pr, lreps );
    GetReps( ELM_PLIST(list, 2), pr, rreps );
    for  (i=1; i<=LEN_PLIST(lreps); i++)
        for  (j=1; j<=LEN_PLIST(rreps); j++)
	    {
	        k = LEN_PLIST( ELM_PLIST(lreps, i) )
		  + LEN_PLIST( ELM_PLIST(rreps, j) ) + 5;/* m"ogliche Inkom-*/
		tree = NEW_PLIST(T_PLIST, k);            /* patibilit"at nach*/
		SET_LEN_PLIST(tree, k);        /*"Anderung der Datenstruktur */
		SET_ELM_PLIST(tree, 1, INTOBJ_INT(1) );
		SET_ELM_PLIST(tree, 2, ELM_PLIST( list, 3) );
		SET_ELM_PLIST(tree, 3, INTOBJ_INT(0) );
		SET_ELM_PLIST(tree, 4, INTOBJ_INT((int)(k/5)) );
		SET_ELM_PLIST(tree, 5, INTOBJ_INT(0) );
		tree1 = ELM_PLIST(lreps, i);
		for  (l=1; l<=LEN_PLIST( tree1 ); l++)
		    SET_ELM_PLIST(tree, l+5, ELM_PLIST(tree1, l) );
		k = LEN_PLIST(tree1) + 5;
		tree1 = ELM_PLIST(rreps, j);
		for  (l=1; l<=LEN_PLIST(tree1); l++)
		    SET_ELM_PLIST(tree, l+k, ELM_PLIST(tree1, l) );
		UnmarkTree(tree);
		FindNewReps2(tree, pols, pr);
	    }
}


Obj      FuncGetPols(
		     Obj      self,
		     Obj      list,
		     Obj      pr,
                     Obj      pols      )
{
    void    GetPols();

    if  (LEN_PLIST(list) != 4)
        ErrorReturnVoid("<list> must be a generalised representativ not a tree"
			,0L, 0L, "you can return");
    GetPols(list, pr, pols);
    return  0;
}


void    GetReps( 
		Obj    list,
		Obj    pr,
		Obj    reps     )
{
    Obj    lreps,
           rreps,
           tree,
           tree1;
    void   FindNewReps1();
    UInt   i,j,k,l;

    if  ( LEN_PLIST(list) != 4 )
    {
        SET_ELM_PLIST(reps, 1, list);
	SET_LEN_PLIST(reps, 1);
	return;
    }
    lreps = NEW_PLIST(T_PLIST, 2);
    rreps = NEW_PLIST(T_PLIST, 2);
    SET_LEN_PLIST(lreps, 0);
    SET_LEN_PLIST(rreps, 0);
    GetReps( ELM_PLIST(list, 1), pr, lreps );
    GetReps( ELM_PLIST(list, 2), pr, rreps );
    for  (i=1; i<=LEN_PLIST(lreps); i++)
        for  (j=1; j<=LEN_PLIST(rreps); j++)
	{
            k = LEN_PLIST( ELM_PLIST(lreps, i) )
	        + LEN_PLIST( ELM_PLIST(rreps, j) ) + 5;/* m"ogliche Inkom-*/
	    tree = NEW_PLIST(T_PLIST, k);            /* patibilit"at nach*/
	    SET_LEN_PLIST(tree, k);        /*"Anderung der Datenstruktur */
	    SET_ELM_PLIST(tree, 1, INTOBJ_INT(1) );
	    SET_ELM_PLIST(tree, 2, ELM_PLIST( list, 3) );
	    SET_ELM_PLIST(tree, 3, INTOBJ_INT(0) );
	    SET_ELM_PLIST(tree, 4, INTOBJ_INT((int)(k/5)) );
	    if  (  TYPE_OBJ( ELM_PLIST(list, 4) ) == T_INT        &&
		   CELM(list, 4) < 100                            &&
		   CELM(list, 4) > 0                                 )
 		SET_ELM_PLIST(tree, 5, ELM_PLIST(list, 4) );
	    else
		SET_ELM_PLIST(tree, 5, INTOBJ_INT(0) );
	    tree1 = ELM_PLIST(lreps, i);
	    for  (l=1; l<=LEN_PLIST( tree1 ); l++)
	        SET_ELM_PLIST(tree, l+5, ELM_PLIST(tree1, l) );
	    k = LEN_PLIST(tree1) + 5;
	    tree1 = ELM_PLIST(rreps, j);
	    for  (l=1; l<=LEN_PLIST(tree1); l++)
	        SET_ELM_PLIST(tree, l+k, ELM_PLIST(tree1, l) );
	    UnmarkTree(tree);
	    FindNewReps1(tree, reps);
	}
}


/**************************************************************************
**
*F  FindNewReps(<tree>,<reps>,<pr>,<max>) . . construct new representatives
**
**  'FindNewReps' constructs all trees <tree'> with the following properties.
**  1) left(<tree'>) is equivalent to left(<tree>).
**     right(<tree'>) is equivalent to right(<tree>).
**  2) <tree'> is the least tree in its equivalence class.
**  3) for each marked node of (<tree>, i) of <tree> tree(<tree>, i) is equal
**     to tree(<tree'>, i).
**  'FindNewReps' adds these trees to the list of representatives <reps>.
**  It is assumed that both left(<tree>) and right(<tree>) are the least
**  elements in their equivalence class.
**  'FindNewReps' works in double recursion with 'Findsubs'.  A detailed
**  description of the whole process can be found in the diploma thesis
**  of Wolfgang Merkwitz.
*/
void   FindNewReps1(
		    Obj     tree,
		    Obj     reps
                                      )
{
    Obj   y,           /*  stores a copy of <tree>                       */
          lsubs,       /*  stores pos(<subtree>) for all subtrees of
                       **  left(<tree>) in a given almost equal class    */

          rsubs,       /*  stores pos(<subtree>) for all subtrees of
                       **  right(<tree>) in the same almost equal class  */

          llist,       /*  stores all elements of an almost equal class
                       **  of subtrees of left(<tree>)                   */

          rlist;       /*  stores all elments of the same almost equal
                       **  class of subtrees of right(<tree>)            */
    int   a,           /*  stores a subtree of right((<tree>)            */
          n,           /*  Length of lsubs                               */
          m,           /*  Length of rsubs                               */
          i;           /*  loop variable                                 */
    void  FindSubs1();

    /*  get a subtree of right(<tree>) which is unmarked but whose 
    **  subtrees are all marked                                          */
    a = FindTree(tree, DT_RIGHT(tree, 1) );
    /*  If we do not find such a tree we at the bottom of the recursion.
    **  If leftof(left(<tree>),  right(<tree>) ) holds we add all trees
    **  <tree'> with left(<tree'>) = left(<tree>), 
    **  right(<tree'>) = right(<tree>) to <reps>,  and <tree'> is the
    **  least element in its equivalence calss.  Note that for such a 
    **  tree we have pos(<tree'>) = 1 and num(<tree'>) = j where j is a
    **  positive integer for which
    **  c_( num(left(<tree>),  num(right(<tree>)), j ) does not equal
    **  0.  These integers are contained in the list
    **  pr[ num(left(<tree>)) ][ num(right(<tree>)) ].bas .             */
    if  ( a == 0 )
    {
        if ( Leftof(tree, DT_LEFT(tree, 1), tree, DT_RIGHT(tree, 1) )  )
	{
            y = ShallowCopyPlist(tree);
            GROW_PLIST(reps, LEN_PLIST(reps) + 1);
            SET_LEN_PLIST(reps, LEN_PLIST(reps) + 1);
            SET_ELM_PLIST(reps, LEN_PLIST(reps), y);
	    /*  tell gasman that <reps> has changed           */
	    CHANGED_BAG(reps);		    
	}
	return;
    }
    /*  get all subtrees of left(<tree>) which are almost equal to
    **  tree(<tree>, a) and mark them                                  */
    llist = Mark2(tree, DT_LEFT(tree, 1), tree, a);
    /*  get all subtrees of right(<tree>) which are almost equal to
    **  tree(<tree>, a) and mark them                                  */
    rlist = Mark2(tree, DT_RIGHT(tree, 1), tree, a);
    n = LEN_PLIST(llist);
    m = LEN_PLIST(rlist);
    /*  if no subtrees of left(<tree>) almost equal to
    **  tree(<tree>, a) have been found there is no possibility
    **  to change the pos-argument in the trees stored in llist and
    **  rlist,  so call FindNewReps without changing any pos-arguments.
    */
    if  ( n == 0 )
    {
        FindNewReps1(tree, reps);
	/*  unmark all top nodes of the trees stored in rlist          */
	UnmarkAEClass(tree, rlist);
	return;
    }
    /*  store all pos-arguments that occur in the trees of llist.
    **  Note that the set of the pos-arguments in llist actually
    **  equals {1,...,n}.                                              */
    lsubs = NEW_PLIST( T_PLIST, n );
    SET_LEN_PLIST(lsubs, n);
    for (i=1; i<=n; i++)
        SET_ELM_PLIST(lsubs, i, INTOBJ_INT(i) );
    /*  store all pos-arguments that occur in the trees of rlist.
    **  Note that the set of the pos-arguments in rlist actually
    **  equals {1,...,m}.                                              */
    rsubs = NEW_PLIST( T_PLIST, m );
    SET_LEN_PLIST(rsubs, m);
    for (i=1; i<=m; i++)
        SET_ELM_PLIST(rsubs, i, INTOBJ_INT(i) );
    /*  find all possibilities for lsubs and rsubs such that
    **  lsubs[1] < lsubs[2] <...<lsubs[n],
    **  rsubs[1] < rsubs[2] <...<rsubs[n],
    **  and set(lsubs concat rsubs) equals {1,...,k} for a positiv
    **  integer k.  For each found lsubs and rsubs 'FindSubs' changes
    **  pos-arguments of the subtrees in llist and rlist accordingly
    **  and  then calls 'FindNewReps' with the changed tree <tree>.
    */
    FindSubs1(tree, a, llist, rlist, lsubs, rsubs, 1, n, 1, m, reps);
    /*  Unmark the subtrees of <tree> in llist and rlist and reset
    **  pos-arguments to the original state.                            */
    UnmarkAEClass(tree, rlist);
    UnmarkAEClass(tree, llist);
}


void   FindNewReps2(
		    Obj     tree,
		    Obj     reps,
		    Obj     pr  /*  pc-presentation for a 
                                 **  nilpotent group <G>                 */
                                      )
{
    Obj   lsubs,       /*  stores pos(<subtree>) for all subtrees of
                       **  left(<tree>) in a given almost equal class    */

          rsubs,       /*  stores pos(<subtree>) for all subtrees of
                       **  right(<tree>) in the same almost equal class  */

          llist,       /*  stores all elements of an almost equal class
                       **  of subtrees of left(<tree>)                   */

          rlist;       /*  stores all elments of the same almost equal
                       **  class of subtrees of right(<tree>)            */
    int   a,           /*  stores a subtree of right((<tree>)            */
          n,           /*  Length of lsubs                               */
          m,           /*  Length of rsubs                               */
          i;           /*  loop variable                                 */
    void  FindSubs2();

    /*  get a subtree of right(<tree>) which is unmarked but whose 
    **  subtrees are all marked                                          */
    a = FindTree(tree, DT_RIGHT(tree, 1) );
    /*  If we do not find such a tree we at the bottom of the recursion.
    **  If leftof(left(<tree>),  right(<tree>) ) holds we add all trees
    **  <tree'> with left(<tree'>) = left(<tree>), 
    **  right(<tree'>) = right(<tree>) to <reps>,  and <tree'> is the
    **  least element in its equivalence calss.  Note that for such a 
    **  tree we have pos(<tree'>) = 1 and num(<tree'>) = j where j is a
    **  positive integer for which
    **  c_( num(left(<tree>),  num(right(<tree>)), j ) does not equal
    **  0.  These integers are contained in the list
    **  pr[ num(left(<tree>)) ][ num(right(<tree>)) ].bas .             */
    if  ( a == 0 )
    {
        if ( Leftof(tree, DT_LEFT(tree, 1), tree, DT_RIGHT(tree, 1) )  )
	{
                /*  get the formula vector of tree and add it to
                **  reps[ rel.bas[1] ].                                */  
            UnmarkTree(tree);
	    tree = MakeFormulaVector( tree, pr);
	    CALL_3ARGS(Dt_add, tree, reps, pr);
	}
	return;
    }
    /*  get all subtrees of left(<tree>) which are almost equal to
    **  tree(<tree>, a) and mark them                                  */
    llist = Mark2(tree, DT_LEFT(tree, 1), tree, a);
    /*  get all subtrees of right(<tree>) which are almost equal to
    **  tree(<tree>, a) and mark them                                  */
    rlist = Mark2(tree, DT_RIGHT(tree, 1), tree, a);
    n = LEN_PLIST(llist);
    m = LEN_PLIST(rlist);
    /*  if no subtrees of left(<tree>) almost equal to
    **  tree(<tree>, a) have been found there is no possibility
    **  to change the pos-argument in the trees stored in llist and
    **  rlist,  so call FindNewReps without changing any pos-arguments.
    */
    if  ( n == 0 )
    {
        FindNewReps2(tree, reps, pr);
	/*  unmark all top nodes of the trees stored in rlist          */
	UnmarkAEClass(tree, rlist);
	return;
    }
    /*  store all pos-arguments that occur in the trees of llist.
    **  Note that the set of the pos-arguments in llist actually
    **  equals {1,...,n}.                                              */
    lsubs = NEW_PLIST( T_PLIST, n );
    SET_LEN_PLIST(lsubs, n);
    for (i=1; i<=n; i++)
        SET_ELM_PLIST(lsubs, i, INTOBJ_INT(i) );
    /*  store all pos-arguments that occur in the trees of rlist.
    **  Note that the set of the pos-arguments in rlist actually
    **  equals {1,...,m}.                                              */
    rsubs = NEW_PLIST( T_PLIST, m );
    SET_LEN_PLIST(rsubs, m);
    for (i=1; i<=m; i++)
        SET_ELM_PLIST(rsubs, i, INTOBJ_INT(i) );
    /*  find all possibilities for lsubs and rsubs such that
    **  lsubs[1] < lsubs[2] <...<lsubs[n],
    **  rsubs[1] < rsubs[2] <...<rsubs[n],
    **  and set(lsubs concat rsubs) equals {1,...,k} for a positiv
    **  integer k.  For each found lsubs and rsubs 'FindSubs' changes
    **  pos-arguments of the subtrees in llist and rlist accordingly
    **  and  then calls 'FindNewReps' with the changed tree <tree>.
    */
    FindSubs2(tree, a, llist, rlist, lsubs, rsubs, 1, n, 1, m, reps, pr);
    /*  Unmark the subtrees of <tree> in llist and rlist and reset
    **  pos-arguments to the original state.                            */
    UnmarkAEClass(tree, rlist);
    UnmarkAEClass(tree, llist);
}


Obj    Funcposition(Obj      self,
                    Obj      vector)
{
    UInt   res,i;

    res = CELM(vector, 6)*CELM(vector, 6);
    for  (i=7; i < LEN_PLIST(vector); i+=2)
        res += CELM(vector, i)*CELM(vector, i+1)*CELM(vector, i+1);
    return INTOBJ_INT(res);
}


void   FindNewReps(
		    Obj     tree,
		    Obj     reps,
		    Obj     pr,  /*  pc-presentation for a 
                                 **  nilpotent group <G>                 */

		    Obj     max  /*  every generator <g_i> of <G> with
                                 **  i > max lies in the center of <G>   */
                                      )
{
    Obj   y,           /*  stores a copy of <tree>                       */
          lsubs,       /*  stores pos(<subtree>) for all subtrees of
                       **  left(<tree>) in a given almost equal class    */

          rsubs,       /*  stores pos(<subtree>) for all subtrees of
                       **  right(<tree>) in the same almost equal class  */

          llist,       /*  stores all elements of an almost equal class
                       **  of subtrees of left(<tree>)                   */

          rlist,       /*  stores all elments of the same almost equal
                       **  class of subtrees of right(<tree>)            */
          list1,       /*  stores a sublist of <reps>                    */
          rel;         /*  stores a commutator relation from <pr>        */
    int   a,           /*  stores a subtree of right((<tree>)            */
          n,           /*  Length of lsubs                               */
          m,           /*  Length of rsubs                               */
          i;           /*  loop variable                                 */

    /*  get a subtree of right(<tree>) which is unmarked but whose 
    **  subtrees are all marked                                          */
    a = FindTree(tree, DT_RIGHT(tree, 1) );
    /*  If we do not find such a tree we at the bottom of the recursion.
    **  If leftof(left(<tree>),  right(<tree>) ) holds we add all trees
    **  <tree'> with left(<tree'>) = left(<tree>), 
    **  right(<tree'>) = right(<tree>) to <reps>,  and <tree'> is the
    **  least element in its equivalence calss.  Note that for such a 
    **  tree we have pos(<tree'>) = 1 and num(<tree'>) = j where j is a
    **  positive integer for which
    **  c_( num(left(<tree>),  num(right(<tree>)), j ) does not equal
    **  0.  These integers are contained in the list
    **  pr[ num(left(<tree>)) ][ num(right(<tree>)) ].bas .             */
    if  ( a == 0 )
    {
        if ( Leftof(tree, DT_LEFT(tree, 1), tree, DT_RIGHT(tree, 1) )  )
	{
            /*  get  pr[ num(left(<tree>)) ][ num(right(<tree>)) ]      */
	    rel = ELM_PLIST( ELM_PLIST(pr, INT_INTOBJ( DT_GEN(tree, 
                                                         DT_LEFT(tree, 1)))) ,
			     INT_INTOBJ( DT_GEN(tree, DT_RIGHT(tree, 1) ) )  );
	    if  ( ELM_PLIST(rel, 3) > max )
	    {
	      UnmarkTree(tree);
	      tree = MakeFormulaVector(tree, pr);
	      list1 = ELM_PLIST(reps, CELM(rel, 3) );
	      GROW_PLIST(list1, LEN_PLIST(list1) + 1 );
	      SET_LEN_PLIST(list1, LEN_PLIST(list1) + 1 );
	      SET_ELM_PLIST(list1, LEN_PLIST(list1), tree);
	      CHANGED_BAG(list1);
	    }
	    else
	    {
                y = ShallowCopyPlist(tree);
                for  (  i=3;  
                        i < LEN_PLIST( rel )  &&
                        ELM_PLIST(rel, i) <= max;  
                        i+=2                                        )
		{
		    list1 = ELM_PLIST(reps, CELM(rel, i)  );
		    GROW_PLIST(list1, LEN_PLIST(list1) + 1);
                    SET_LEN_PLIST(list1, LEN_PLIST(list1) + 1);
	            SET_ELM_PLIST(list1, LEN_PLIST(list1), y);
		    /*  tell gasman that <list1> has changed           */
		    CHANGED_BAG(list1);		    
	        }
	    }
	}
	return;
    }
    /*  get all subtrees of left(<tree>) which are almost equal to
    **  tree(<tree>, a) and mark them                                  */
    llist = Mark2(tree, DT_LEFT(tree, 1), tree, a);
    /*  get all subtrees of right(<tree>) which are almost equal to
    **  tree(<tree>, a) and mark them                                  */
    rlist = Mark2(tree, DT_RIGHT(tree, 1), tree, a);
    n = LEN_PLIST(llist);
    m = LEN_PLIST(rlist);
    /*  if no subtrees of left(<tree>) almost equal to
    **  tree(<tree>, a) have been found there is no possibility
    **  to change the pos-argument in the trees stored in llist and
    **  rlist,  so call FindNewReps without changing any pos-arguments.
    */
    if  ( n == 0 )
    {
        FindNewReps(tree, reps, pr, max);
	/*  unmark all top nodes of the trees stored in rlist          */
	UnmarkAEClass(tree, rlist);
	return;
    }
    /*  store all pos-arguments that occur in the trees of llist.
    **  Note that the set of the pos-arguments in llist actually
    **  equals {1,...,n}.                                              */
    lsubs = NEW_PLIST( T_PLIST, n );
    SET_LEN_PLIST(lsubs, n);
    for (i=1; i<=n; i++)
        SET_ELM_PLIST(lsubs, i, INTOBJ_INT(i) );
    /*  store all pos-arguments that occur in the trees of rlist.
    **  Note that the set of the pos-arguments in rlist actually
    **  equals {1,...,m}.                                              */
    rsubs = NEW_PLIST( T_PLIST, m );
    SET_LEN_PLIST(rsubs, m);
    for (i=1; i<=m; i++)
        SET_ELM_PLIST(rsubs, i, INTOBJ_INT(i) );
    /*  find all possibilities for lsubs and rsubs such that
    **  lsubs[1] < lsubs[2] <...<lsubs[n],
    **  rsubs[1] < rsubs[2] <...<rsubs[n],
    **  and set(lsubs concat rsubs) equals {1,...,k} for a positiv
    **  integer k.  For each found lsubs and rsubs 'FindSubs' changes
    **  pos-arguments of the subtrees in llist and rlist accordingly
    **  and  then calls 'FindNewReps' with the changed tree <tree>.
    */
    FindSubs(tree, a, llist, rlist, lsubs, rsubs, 1, n, 1, m, reps, pr, max);
    /*  Unmark the subtrees of <tree> in llist and rlist and reset
    **  pos-arguments to the original state.                            */
    UnmarkAEClass(tree, rlist);
    UnmarkAEClass(tree, llist);
}


/***************************************************************************
**
*F  FuncFindNewReps(<self>,<args>) . . . . . . construct new representatives
**
**  'FuncFindNewReps' implements the internal function 'FindNewReps'.
**
**  'FindNewReps(<tree>, <reps>, <pr>, <max>)'
**
**  'FindNewReps' constructs all trees <tree'> with the following properties.
**  1) left(<tree'>) is equivalent to left(<tree>).
**     right(<tree'>) is equivalent to right(<tree>).
**  2) <tree'> is the least tree in its equivalence class.
**  3) for each marked node of (<tree>, i) of <tree> tree(<tree>, i) is equal
**     to tree(<tree'>, i).
**  'FindNewReps' adds these trees to the list of representatives <reps>.
**  It is assumed that both left(<tree>) and right(<tree>) are the least
**  elements in their equivalence class.
**
**  'FindNewReps only returns correct results if all nodes of <tree> are
**  unmarked.
*/
Obj    FuncFindNewReps(
	 	        Obj     self,
		        Obj     tree,
                        Obj     reps,
                        Obj     pr,
		        Obj     max       )
{

    /*  test if <tree> is really a tree                                    */
    /* TestTree(tree);                                                     */
    if  ( LEN_PLIST(tree) < 15 )  
        ErrorReturnVoid("<tree> must be a tree not a plain list", 0L, 0L,
			"you can return");
    FindNewReps(tree, reps, pr, max);   
    return  0;
}


/***************************************************************************
**
*F  TestTree(<obj>) . . . . . . . . . . . . . . . . . . . . . . test a tree
**
**  'TestTree' tests if <tree> is a tree. If <tree> is not a tree 'TestTree'
**  signals an error.
*/
void  TestTree(
               Obj     tree)
{

    if ( TYPE_OBJ(tree) != T_PLIST || LEN_PLIST(tree) % 7 != 0)
        ErrorReturnVoid("<tree> must be a plain list,  whose length is a multiple of 7", 0L, 0L, "you can return");
    if ( DT_LENGTH(tree, 1) != LEN_PLIST(tree)/7 )
        ErrorReturnVoid("<tree> must be a tree, not a plain list.", 0L, 0L,
                        "you can return");
    if ( DT_SIDE(tree, 1) >= DT_LENGTH(tree, 1) )
        ErrorReturnVoid("<tree> must be a tree, not a plain list.", 0L, 0L,
                        "you can return");        
    if ( DT_LENGTH(tree, 1) == 1)
    {
        if ( DT_SIDE(tree, 1) != LEFT && DT_SIDE(tree, 1) != RIGHT )
            ErrorReturnVoid("<tree> must be a tree, not a plain list.", 0L, 0L,
                            "you can return");
	return;
    }
    if ( DT_SIDE(tree, 1) <= 1 )
        ErrorReturnVoid("<tree> must be a tree, not a plain list.", 0L, 0L,
                        "you can return");
    if (DT_LENGTH(tree, 1) !=
          DT_LENGTH(tree, DT_LEFT(tree, 1)) + 
          DT_LENGTH(tree, DT_RIGHT(tree, 1)) +  
          1                                           )
        ErrorReturnVoid(" < tree > must be a tree, not a plain list.", 0L, 0L,
                        "you can return");
    if ( DT_SIDE(tree, 1) != DT_LENGTH(tree, DT_LEFT(tree, 1) ) + 1 )
        ErrorReturnVoid("<tree> must be a tree, not a plain list.", 0L, 0L,
                        "you can return");
    TestTree( Part(tree, (DT_LEFT(tree, 1) - 1)*7, 
                         (DT_RIGHT(tree, 1) - 1)*7                    )    );
    TestTree( Part(tree, (DT_RIGHT(tree, 1) - 1)*7,  LEN_PLIST(tree) ) );
}


/****************************************************************************
**
*F  Part(<list>, <pos1>, <pos2> . . . . . . . . . . . . return a part of list
**
**  'Part' returns <list>{ [<pos1>+1 .. <pos2>] }.
*/
Obj    Part(
	     Obj      list,
	     int      pos1,
	     int      pos2  )
{
    int      i, length;
    Obj      part;

    length = pos2 - pos1;
    part = NEW_PLIST(T_PLIST, length);
    SET_LEN_PLIST(part, length);
    for (i=1; i <= length; i++)
    {
        SET_ELM_PLIST(part, i, ELM_PLIST(list, pos1+i) );
    }
    return part;
}


/***************************************************************************
**
*F  FindSubs(<tree>,<x>,<list1>,<list2>,<a>,<b>,<al>,<ar>,<bl>,<br>,<reps>,
**           <pr>,<max>  ) . . . . . . . . . find possible pos-arguments for 
**                                           the trees in <list1> and <list2>
**
**  'FindSubs' finds all possibilities for a and b such that
**  1) a[1] < a[2] <..< a[ ar ]
**     b[1] < b[2] <..< b[ br ]
**  2) set( a concat b ) = {1,..,k} for a positiv integer k.
**  3) a[1],...,a[ al-1 ] and b[1],..,b[ bl-1 ] remain unchanged.
**  For each found possibility 'FindSubs' sets the pos-arguments in the
**  trees of <list1> and <list2> according to the entries of <a> and
**  <b>.  Then it calls 'FindNewReps' with the changed tree <tree> as
v**  argument.
**
**  It is assumed that the conditions 1) and 2) hold for a{ [1..al-1] } and
**  b{ [1..bl-1] }.  A detailed description of 'FindSubs' can be found in
**  the diploma thesis of Wolfgang Merkwitz.
*/
void  FindSubs1(
	        Obj        tree,
                int        x,     /*  subtree of <tree>                     */
                Obj        list1, /*  list containing all subtrees of
                                  **  left(<tree>) almost equal to
                                  **  tree(<tree>, x)                       */
                                  
                Obj        list2, /*  list containing all subtrees of
                                  **  right(<tree>) almost equal to
                                  **  tree(<tree>, x)                       */

                Obj        a,     /*  list to change,  containing the
                                  **  pos-arguments of the trees in list1   */

                Obj        b,     /*  list to change,  containing tthe
                                  **  pos-arguments of the trees in list2   */
                int        al,
	        int        ar,
	        int        bl,
	        int        br,
	        Obj        reps  /*  list of representatives for all trees */
                                        )
{
   int    i;  /*  loop variable                                             */
   void   FindNewReps1();

   /*  if <al> > <ar> or <bl> > <br> nothing remains to change.             */
   if (  al > ar  ||  bl > br  )
   {
       /*  Set the pos-arguments of the trees in <list1> and <list2>
       **  according to the entries of <a> and <b>.                         */
       SetSubs( list1, a, tree);
       SetSubs( list2, b, tree);
       FindNewReps1(tree, reps);
       return;
   }
   /*  If a[ ar] is bigger or equal to the boundary of pos(tree(<tree>, x)
   **  the execution of the statements in the body of this if-statement
   **  would have the consequence that some subtrees of <tree> in <list1>
   **  would get a pos-argument bigger than the boundary of
   **  pos(tree<tree>, x).  But since the trees in <list1> are almost
   **  equal to tree(<tree>, x) they have all the same boundary for their
   **  pos-argument as tree(<tree>, x).  So these statements are only
   **  executed when <a>[ar] is less than the boundary of 
   **  pos(tree(<tree>, x).
   */
   if ( INT_INTOBJ( DT_MAX(tree, x) ) <= 0  ||  
        ELM_PLIST(a, ar) < DT_MAX(tree, x)   )
   {
       for (i=al; i<=ar; i++)
	   SET_ELM_PLIST(a, i, INTOBJ_INT( CELM(a,i) + 1 ) );
       FindSubs1(tree, x, list1, list2, a, b, al, ar, bl+1, br, reps);
       for  (i=al; i<=ar; i++)
	   SET_ELM_PLIST(a, i, INTOBJ_INT( CELM(a, i) - 1  ) );
   }
   FindSubs1(tree, x, list1, list2, a, b, al+1, ar, bl+1, br, reps);
   /*  If b[ br] is bigger or equal to the boundary of pos(tree(<tree>, x)
   **  the execution of the statements in the body of this if-statement
   **  would have the consequence that some subtrees of <tree> in <list2>
   **  would get a pos-argument bigger than the boundary of
   **  pos(tree<tree>, x).  But since the trees in <list2> are almost
   **  equal to tree(<tree>, x) they have all the same boundary for their
   **  pos-argument as tree(<tree>, x).  So these statements are only
   **  executed when <b>[br] is less than the boundary of 
   **  pos(tree(<tree>, x).
   */
   if ( INT_INTOBJ( DT_MAX(tree, x) ) <= 0  ||
        ELM_PLIST(b, br) < DT_MAX(tree, x)        )
   {
       for  (i=bl; i<=br; i++)
	   SET_ELM_PLIST(b, i, INTOBJ_INT( CELM(b, i) + 1  ) );
       FindSubs1(tree, x, list1, list2, a, b, al+1, ar, bl, br, reps);
       for  (i=bl; i<=br; i++)
	   SET_ELM_PLIST(b, i, INTOBJ_INT( CELM(b, i) - 1 ) );
   }
}


void  FindSubs2(
	        Obj        tree,
                int        x,     /*  subtree of <tree>                     */
                Obj        list1, /*  list containing all subtrees of
                                  **  left(<tree>) almost equal to
                                  **  tree(<tree>, x)                       */
                                  
                Obj        list2, /*  list containing all subtrees of
                                  **  right(<tree>) almost equal to
                                  **  tree(<tree>, x)                       */

                Obj        a,     /*  list to change,  containing the
                                  **  pos-arguments of the trees in list1   */

                Obj        b,     /*  list to change,  containing tthe
                                  **  pos-arguments of the trees in list2   */
                int        al,
	        int        ar,
	        int        bl,
	        int        br,
	        Obj        reps,  /*  list of representatives for all trees */
	        Obj        pr    /*  pc-presentation                       */
                                        )
{
   int    i;  /*  loop variable                                             */
   void   FindNewReps2();

   /*  if <al> > <ar> or <bl> > <br> nothing remains to change.             */
   if (  al > ar  ||  bl > br  )
   {
       /*  Set the pos-arguments of the trees in <list1> and <list2>
       **  according to the entries of <a> and <b>.                         */
       SetSubs( list1, a, tree);
       SetSubs( list2, b, tree);
       FindNewReps2(tree, reps, pr);
       return;
   }
   /*  If a[ ar] is bigger or equal to the boundary of pos(tree(<tree>, x)
   **  the execution of the statements in the body of this if-statement
   **  would have the consequence that some subtrees of <tree> in <list1>
   **  would get a pos-argument bigger than the boundary of
   **  pos(tree<tree>, x).  But since the trees in <list1> are almost
   **  equal to tree(<tree>, x) they have all the same boundary for their
   **  pos-argument as tree(<tree>, x).  So these statements are only
   **  executed when <a>[ar] is less than the boundary of 
   **  pos(tree(<tree>, x).
   */
   if ( INT_INTOBJ( DT_MAX(tree, x) ) <= 0  ||  
        ELM_PLIST(a, ar) < DT_MAX(tree, x)   )
   {
       for (i=al; i<=ar; i++)
	   SET_ELM_PLIST(a, i, INTOBJ_INT( CELM(a,i) + 1 ) );
       FindSubs2(tree, x, list1, list2, a, b, al, ar, bl+1, br, reps, pr);
       for  (i=al; i<=ar; i++)
	   SET_ELM_PLIST(a, i, INTOBJ_INT( CELM(a, i) - 1  ) );
   }
   FindSubs2(tree, x, list1, list2, a, b, al+1, ar, bl+1, br, reps, pr);
   /*  If b[ br] is bigger or equal to the boundary of pos(tree(<tree>, x)
   **  the execution of the statements in the body of this if-statement
   **  would have the consequence that some subtrees of <tree> in <list2>
   **  would get a pos-argument bigger than the boundary of
   **  pos(tree<tree>, x).  But since the trees in <list2> are almost
   **  equal to tree(<tree>, x) they have all the same boundary for their
   **  pos-argument as tree(<tree>, x).  So these statements are only
   **  executed when <b>[br] is less than the boundary of 
   **  pos(tree(<tree>, x).
   */
   if ( INT_INTOBJ( DT_MAX(tree, x) ) <= 0  ||
        ELM_PLIST(b, br) < DT_MAX(tree, x)        )
   {
       for  (i=bl; i<=br; i++)
	   SET_ELM_PLIST(b, i, INTOBJ_INT( CELM(b, i) + 1  ) );
       FindSubs2(tree, x, list1, list2, a, b, al+1, ar, bl, br, reps, pr);
       for  (i=bl; i<=br; i++)
	   SET_ELM_PLIST(b, i, INTOBJ_INT( CELM(b, i) - 1 ) );
   }
}


void  FindSubs(
	        Obj        tree,
                int        x,     /*  subtree of <tree>                     */
                Obj        list1, /*  list containing all subtrees of
                                  **  left(<tree>) almost equal to
                                  **  tree(<tree>, x)                       */
                                  
                Obj        list2, /*  list containing all subtrees of
                                  **  right(<tree>) almost equal to
                                  **  tree(<tree>, x)                       */

                Obj        a,     /*  list to change,  containing the
                                  **  pos-arguments of the trees in list1   */

                Obj        b,     /*  list to change,  containing tthe
                                  **  pos-arguments of the trees in list2   */
                int        al,
	        int        ar,
	        int        bl,
	        int        br,
	        Obj        reps,  /*  list of representatives for all trees */
	        Obj        pr,    /*  pc-presentation                       */
	        Obj        max    /*  needed to call 'FindNewReps'          */
                                        )
{
   int    i;  /*  loop variable                                             */

   /*  if <al> > <ar> or <bl> > <br> nothing remains to change.             */
   if (  al > ar  ||  bl > br  )
   {
       /*  Set the pos-arguments of the trees in <list1> and <list2>
       **  according to the entries of <a> and <b>.                         */
       SetSubs( list1, a, tree);
       SetSubs( list2, b, tree);
       FindNewReps(tree, reps, pr, max);
       return;
   }
   /*  If a[ ar] is bigger or equal to the boundary of pos(tree(<tree>, x)
   **  the execution of the statements in the body of this if-statement
   **  would have the consequence that some subtrees of <tree> in <list1>
   **  would get a pos-argument bigger than the boundary of
   **  pos(tree<tree>, x).  But since the trees in <list1> are almost
   **  equal to tree(<tree>, x) they have all the same boundary for their
   **  pos-argument as tree(<tree>, x).  So these statements are only
   **  executed when <a>[ar] is less than the boundary of 
   **  pos(tree(<tree>, x).
   */
   if ( INT_INTOBJ( DT_MAX(tree, x) ) <= 0  ||  
        ELM_PLIST(a, ar) < DT_MAX(tree, x)   )
   {
       for (i=al; i<=ar; i++)
	   SET_ELM_PLIST(a, i, INTOBJ_INT( CELM(a,i) + 1 ) );
       FindSubs(tree, x, list1, list2, a, b, al, ar, bl+1, br, reps, pr, max);
       for  (i=al; i<=ar; i++)
	   SET_ELM_PLIST(a, i, INTOBJ_INT( CELM(a, i) - 1  ) );
   }
   FindSubs(tree, x, list1, list2, a, b, al+1, ar, bl+1, br, reps, pr, max);
   /*  If b[ br] is bigger or equal to the boundary of pos(tree(<tree>, x)
   **  the execution of the statements in the body of this if-statement
   **  would have the consequence that some subtrees of <tree> in <list2>
   **  would get a pos-argument bigger than the boundary of
   **  pos(tree<tree>, x).  But since the trees in <list2> are almost
   **  equal to tree(<tree>, x) they have all the same boundary for their
   **  pos-argument as tree(<tree>, x).  So these statements are only
   **  executed when <b>[br] is less than the boundary of 
   **  pos(tree(<tree>, x).
   */
   if ( INT_INTOBJ( DT_MAX(tree, x) ) <= 0  ||
        ELM_PLIST(b, br) < DT_MAX(tree, x)        )
   {
       for  (i=bl; i<=br; i++)
	   SET_ELM_PLIST(b, i, INTOBJ_INT( CELM(b, i) + 1  ) );
       FindSubs(tree, x, list1, list2, a, b, al+1, ar, bl, br, reps, pr, max);
       for  (i=bl; i<=br; i++)
	   SET_ELM_PLIST(b, i, INTOBJ_INT( CELM(b, i) - 1 ) );
   }
}


/****************************************************************************
**
*F  SetSubs(<list>, <a>, <tree>) . . . . . . . . . . .. .  set pos-arguments
** 
**  'SetSubs' sets the pos-arguments of the subtrees of <tree>,  contained
**  in <list> according to the entries in the list <a>.
*/
void    SetSubs(
		 Obj       list,
		 Obj       a,
		 Obj       tree    )
{
    int   i,j;  /*  loop variables                                         */
    
    for  (i=1; i <= LEN_PLIST(list); i++)
        for  (j=1;  j <= LEN_PLIST( ELM_PLIST(list, i) );  j++)
	    SET_DT_POS(tree, CELM( ELM_PLIST(list, i), j), ELM_PLIST(a, i) );
}


/****************************************************************************
**
*F  UnmarkAEClass(<tree>, <list>) . . . . . . . . . . . . reset pos-arguments
**
**  'UnmarkAEClass' resets the pos arguments of the subtrees of <tree>,
**  contained in <list> to the original state.  Furthermore it unmarks the
**  top node of each of those trees.
*/
void    UnmarkAEClass(
		       Obj      tree,
		       Obj      list  )
{
    int  i,j;

   for  (i=1; i <= LEN_PLIST(list); i++)
        for (j=1;  j <= LEN_PLIST( ELM_PLIST(list, i) );  j++)
	{
	    DT_UNMARK(tree, CELM( ELM_PLIST(list, i), j)  );
	    SET_DT_POS(tree, CELM( ELM_PLIST(list, i), j), INTOBJ_INT(i) );
	}
}



/****************************************************************************
**
*F  InitDeepThought() . . . . . . . . . . . . initialize Deep Thought package
**
**  'InitDeepThought' initializes the Deep Thought package.
*/
void    InitDeepThought( void )
{

    /*  get the record component names 'bas' and 'exp'                      */
    exp = RNamName("exp");
    bas = RNamName("bas");
    /*  install the internal functions                                      */
    AssGVar( GVarName( "MakeFormulaVector" ), NewFunctionC("MakeFormulaVector",
             2L, "tree, presentation", FuncMakeFormulaVector )       );
    AssGVar( GVarName( "FindNewReps" ), NewFunctionC("FindNewReps",
             4L, "tree, representatives, presentation, maximum",
	     FuncFindNewReps )                                    );
    AssGVar( GVarName( "UnmarkTree" ), NewFunctionC("UnmarkTree",
             1L, "tree",	     FuncUnmarkTree )  );
    AssGVar( GVarName( "GetPols" ), NewFunctionC(" GetPols",
             3L, "list, presentation, polynomials", FuncGetPols)      );
    AssGVar( GVarName( "DT_evaluation" ), NewFunctionC( "DT_evaluation",
	     1L, "vector", Funcposition)         );
    InitFopyGVar( GVarName( "dt_add" ), &Dt_add );
}
