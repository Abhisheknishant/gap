#############################################################################
##
#W  list.gd                     GAP library                  Martin Schoenert
#W                                                            & Werner Nickel
##
#H  @(#)$Id$
##
#Y  Copyright (C)  1997,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
##
##  This file contains the definition of operations and functions for lists.
##
Revision.list_gd :=
    "@(#)$Id$";


#############################################################################
##

#C  IsList( <obj> ) . . . . . . . . . . . . . . . test if an object is a list
##
IsList := NewCategoryKernel( "IsList",
    IsListOrCollection,
    IS_LIST );


#############################################################################
##
#C  IsConstantTimeAccessList( <list> )
##
##  This is implied by 'IsList and InternalRep',
##  so all strings, Boolean lists, ranges, and internal plain lists are
##  in this category.
##
##  But also enumerators can have this representation if they know about
##  constant time access to their elements.
##
IsConstantTimeAccessList := NewCategory( "IsConstantTimeAccessList",
    IsList );

InstallTrueMethod( IsConstantTimeAccessList, IsList and IsInternalRep );


#############################################################################
##
#R  IsEnumerator  . . . . . . . . . . . . . .  representation for enumerators
##
IsEnumerator := NewRepresentation( "IsEnumerator",
    IsComponentObjectRep and IsAttributeStoringRep and IsList,
    [] );


#############################################################################
##

#O  Length( <list> )  . . . . . . . . . . . . . . . . . . .  length of a list
##
Length := NewAttributeKernel( "Length",
    IsList,
    LENGTH );
SetLength := Setter( Length );
HasLength := Tester( Length );


#############################################################################
##
#O  IsBound( <list>[<pos>] )  . . . . . . . . test for an element from a list
##
IsBound\[\] := NewOperationKernel(
    "IsBound[]",
    [ IsList, IS_INT ],
    ISB_LIST );


#############################################################################
##
#O  <list>[<pos>] . . . . . . . . . . . . . . . select an element from a list
##
\[\] := NewOperationKernel(
    "[]",
    [ IsList, IS_INT ],
    ELM_LIST );


#############################################################################
##
#O  <list>{<poss>}  . . . . . . . . . . . . . . . select elements from a list
##
\{\} := NewOperationKernel(
    "{}",
    [ IsList, IsList ],
    ELMS_LIST );


#############################################################################
##
#O  ELM0_LIST( <list>, <pos> )
##
ELM0_LIST := NewOperationKernel(
    "ELM0_LIST",
    [ IsList, IS_INT ],
    ELM0_LIST );


#############################################################################
##
#O  Unbind( <list>[<pos>] )
##
Unbind\[\] := NewOperationKernel(
    "Unbind[]",
    [ IsList and IsMutable, IS_INT ],
    UNB_LIST );


#############################################################################
##
#O  <list>[<pos>] := <obj>
##
\[\]\:\= := NewOperationKernel(
    "[]:=",
    [ IsList and IsMutable, IS_INT, IsObject ],
    ASS_LIST );


#############################################################################
##
#O  <list>{<poss>} := <objs>
##
\{\}\:\= := NewOperationKernel(
    "{}:=",
    [ IsList and IsMutable, IsList, IsList ],
    ASSS_LIST );


#############################################################################
##
#O  ConstantTimeAccessList( <list> )
##
##  'ConstantTimeAccessList' returns an immutable list containing the same
##  elements as the list <list> (which may have holes) in the same order.
##  If <list> is already a constant time access list,
##  'ConstantTimeAccessList' returns <list> directly.
##  Otherwise it puts all elements and holes of <list> into a new list and
##  makes that list immutable.
##
ConstantTimeAccessList := NewOperation( "ConstantTimeAccessList",
    [ IsList ] );


#############################################################################
##
#O  AsListSortedList( <list> )
##
##  'AsListSortedList' returns an immutable list containing the same elements
##  as the list <list> (which may have holes) in strictly sorted order.
##  If <list> is already  immutable and  strictly sorted,
##  'AsListSortedList' returns <list> directly.
##  Otherwise it makes a deep copy, and makes that copy immutable.
##  'AsListSortedList' is an internal function.
##
#AsListSortedList := NewOperationKernel(
#    "AsListSortedList",
#    [ IsList ],
#    AS_LIST_SORTED_LIST );
#T  1996/10/28 fceller at the moment this is defined as function in kernel.g
AsListSortedList := AS_LIST_SORTED_LIST;


#############################################################################
##
#C  IsDenseList(<obj>)
##
IsDenseList :=
    NewCategoryKernel( "IsDenseList",
        IsList,
        IS_DENSE_LIST );


#############################################################################
##
#C  IsHomogeneousList(<obj>)
##
##  A homogeneous list is a dense list whose elements lie in the same family.
##  The empty list is homogeneous but not a collection.
##  A nonempty homogeneous list is also a collection.
#T can we guarantee this?
##
IsHomogeneousList :=
    NewCategoryKernel( "IsHomogeneousList",
        IsDenseList,
        IS_HOMOG_LIST );


#############################################################################
##
#M  IsHomogeneousList( <coll_and_list> )  . . for a collection that is a list
#M  IsHomogeneousList( <empty> )  . . . . . . . . . . . . . for an empty list
##
InstallTrueMethod( IsHomogeneousList, IsList and IsCollection );

InstallTrueMethod( IsHomogeneousList, IsList and IsEmpty );


#############################################################################
##
#M  IsFinite( <homoglist> )
##
InstallTrueMethod( IsFinite, IsHomogeneousList and IsInternalRep );


#############################################################################
##
#P  IsNSortedList(<obj>)
##
IsNSortedList :=
    NewPropertyKernel( "IsNSortedList",
        IsDenseList,
        IS_NSORT_LIST );


#############################################################################
##
#P  IsSSortedList(<obj>)
##
#N  1996/09/01 M.Schoenert should inherit from 'IsHomogeneousList'
#N  but the empty list is a sorted list but not homogeneous
##
IsSSortedList :=
    NewPropertyKernel( "IsSSortedList",
        IsDenseList,
        IS_SSORT_LIST );


#############################################################################
##
#P  IsDuplicateFreeList(<obj>)
##
IsDuplicateFreeList :=
    NewProperty( "IsDuplicateFreeList",
        IsDenseList );

InstallTrueMethod( IsDuplicateFreeList, IsSSortedList );


#############################################################################
##
#P  IsPositionsList(<obj>)
##
#N  1996/09/01 M.Schoenert should inherit from 'IsHomogeneousList'
#N  but the empty list is a positions list but not homogeneous
##
IsPositionsList :=
    NewPropertyKernel( "IsPositionsList",
        IsDenseList,
        IS_POSS_LIST );


#############################################################################
##
#C  IsTable(<obj>)
##
IsTable :=
    NewCategoryKernel( "IsTable",
        IsHomogeneousList,
        IS_TABLE_LIST );


#############################################################################
##
#O  Position(<list>,<obj>,<from>)   . . . . . position of an object in a list
#O  Position(<list>,<obj>)  . . . . . . . . . position of an object in a list
##
##  Methods for the version without start position <from> must be installed
##  as methods with three arguments, the third being 'IsZeroCyc'.
##
Position :=
    NewOperationKernel( "Position",
        [ IsList, IsObject, IS_INT ],
        POS_LIST );

#############################################################################
##
#O  PositionCanonical( <list>, <obj> )  . . . position of canonical associate
##
PositionCanonical :=
    NewOperation( "PositionCanonical",
        [ IsList, IsObject ] );

#############################################################################
##
#O  PositionNthOccurence(<list>,<obj>,<n>)   pos. of <n>th occurence of <obj>
##
PositionNthOccurence :=
    NewOperation( "PositionNthOccurence",
        [ IsList, IsObject, IS_INT ] );

#############################################################################
##
#O  PositionSorted(<list>,<obj>)  . . .  position of an object in sorted list
##
PositionSorted :=
    NewOperation( "PositionSorted",
        [ IsHomogeneousList, IsObject ] );


#############################################################################
##
#O  PositionSortedWC(<list>,<obj>) . .  returns 'fail' is object is not found
##
PositionSortedWC := NewOperationArgs("PositionSortedWC");


#############################################################################
##
#O  PositionProperty(<list>,<func>) .  position of an element with a property
##
PositionProperty :=
    NewOperation( "PositionProperty",
        [ IsDenseList, IsFunction ] );


#############################################################################
##
#O  PositionBound(<list>) . . . . . position of first bound element in a list
##
PositionBound :=
    NewOperation( "PositionBound",
        [ IsList ] );


#############################################################################
##
#O  Add(<list>,<obj>) . . . . . . . . . . add an element to the end of a list
##
Add :=
    NewOperationKernel( "Add",
        [ IsList, IsObject ],
        ADD_LIST );


#############################################################################
##
#O  Append(<list1>,<list2>) . . . . . . . . . . . . . append a list to a list
##
Append :=
    NewOperationKernel( "Append",
        [ IsList and IsMutable, IsList ],
        APPEND_LIST );


#############################################################################
##
#O  Concatenation(<list>,<list>,...)  . . . . . . . .  concatenation of lists
##
Concatenation :=
    NewOperationArgs( "Concatenation" );


#############################################################################
##
#O  Compacted(<list>) . . . . . . . . . . . . . . .  remove holes from a list
##
Compacted :=
    NewOperation( "Compacted",
        [ IsList ] );


#############################################################################
##
#O  Collected(<list>) . . . . . . . . . . . collect like elements from a list
##
Collected :=
    NewOperation( "Collected",
        [ IsList ] );


#############################################################################
##
#O  Flat(<list>)  . . . . . . . . list of elements of a nested list structure
##
Flat :=
    NewOperation( "Flat",
        [ IsList ] );


#############################################################################
##
#O  Reversed(<list>)  . . . . . . . . . . . .  reverse the elements in a list
##
Reversed :=
    NewOperation( "Reversed",
        [ IsDenseList ] );


#############################################################################
##
#O  Sort(<list>)  . . . . . . . . . . . . . . . . . . . . . . . . sort a list
##
Sort :=
    NewOperation( "Sort",
        [ IsList and IsMutable ] );


#############################################################################
##
#O  Sortex(<list>) . . . sort a list (stable), return the applied permutation
##
Sortex :=
    NewOperation( "Sortex",
        [ IsHomogeneousList and IsMutable ] );


#############################################################################
##
#O  SortParallel(<list>,<list2>)  . . . . . . . .  sort two lists in parallel
##
SortParallel :=
    NewOperation( "SortParallel",
        [ IsHomogeneousList and IsMutable, IsDenseList and IsMutable ] );


#############################################################################
##
#O  Maximum( <obj>, <obj>... )  . . . . . . . . . . . . .  maximum of objects
#O  MaximumList( [ <obj>, <obj>... ] )  . . . . . . . . . . . maximum of list
##
Maximum :=
    NewOperationArgs( "Maximum" );

MaximumList :=
    NewOperation( "MaximumList",
        [ IsHomogeneousList ] );


#############################################################################
##
#O  Minimum( <obj>, <obj>... )  . . . . . . . . . . . . .  minimum of objects
#O  MinimumList( [ <obj>, <obj>... ] )  . . . . . . . . . . . minimum of list
##
Minimum :=
    NewOperationArgs( "Minimum" );

MinimumList :=
    NewOperation( "MinimumList",
        [ IsHomogeneousList ] );


#############################################################################
##
#O  Cartesian(<list>,<list>...) . . . . . . . . .  cartesian product of lists
##
Cartesian :=
    NewOperationArgs( "Cartesian" );


#############################################################################
##
#O  Permuted(<list>,<perm>)  . . . . . . . . .  apply a permutation to a list
##
Permuted :=
    NewOperation( "Permuted",
        [ IsList, IS_PERM ] );


#############################################################################
##
#F  IteratorList(<list>)
##
##  'IteratorList' returns a new iterator that allows iteration over the
##  elements of <list> (which may have holes) in the same order.
##
IteratorList :=
    NewOperationArgs( "IteratorList" );

#############################################################################
##
#O  First(<C>,<func>) . . . . .  find first element in a list with a property
##
##  First returns the first element of <C> which fullfills <func>. If no 
##  such element is contained in <C>, then First returns fail.
##
First :=
    NewOperation( "First",
        [ IsList, IsFunction ] );


#############################################################################
##
#O  Iterated(<C>,<func>)  . . . . . . . . . .  iterate a function over a list
##
Iterated :=
    NewOperation( "Iterated",
        [ IsList, IsFunction ] );


#############################################################################
##
#F DifferenceBlist
##
DifferenceBlist := NewOperationArgs("DifferenceBlist");


#############################################################################
##
#F  ListWithIdenticalEntries( <n>, <obj> )
##
##  is a list of length <n> that has the object <obj> stored at each of the
##  positions from 1 to <n>.
##
ListWithIdenticalEntries := NewOperationArgs( "ListWithIdenticalEntries" );


#############################################################################
##
#F  ProductPol( <coeffs_f>, <coeffs_g> )  . . . .  product of two polynomials
##
##  Let <coeffs_f> and <coeffs_g> be coefficients lists of two univariate
##  polynomials $f$ and $g$, respectively.
##  'ProductPol' returns the coefficients list of the product $f g$.
##
##  The coefficient of $x^i$ is assumed to be stored at position $i+1$ in
##  the coefficients lists.
##
ProductPol := NewOperationArgs( "ProductPol" );


#############################################################################
##
#F  ValuePol( <coeffs_f>, <point> ) . . . .  evaluate a polynomial at a point
##
##  Let <coeffs_f> be the coefficients list of a univariate polynomial $f$,
##  and <x> a point.
##  'ValuePol' returns the value $f(<point>)$.
##
##  The coefficient of $x^i$ is assumed to be stored at position $i+1$ in
##  the coefficients list.
##
ValuePol := NewOperationArgs( "ValuePol" );


#############################################################################
##
#E  list.gd . . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
##
