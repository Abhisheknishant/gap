#############################################################################
##
#W  zmodnz.gd                   GAP library                     Thomas Breuer
##
#H  @(#)$Id$
##
#Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
##
##  This file contains the design of the rings $Z / n Z$ and their elements.
##
##  The ordering of elements for nonprime $n$ is defined by the ordering of
##  the representatives.
##  For primes smaller than 'MAXSIZE_GF_INTERNAL', the ordering of the
##  internal finite field elements must be respected, for larger primes
##  again the ordering of representatives is chosen.
##
Revision.zmodnz_gd :=
    "@(#)$Id$";


#############################################################################
##
#C  IsZmodnZObj( <obj> )
#C  IsZmodnZObjNonprime( <obj> )
#C  IsZmodpZObj( <obj> )
#C  IsZmodpZObjSmall( <obj> )
#C  IsZmodpZObjLarge( <obj> )
##
##  The elements in the rings $Z / n Z$ are in the category 'IsZmodnZ'.
##  If $n$ is a prime then the elements are of course also in 'IsFFE',
##  otherwise they are in 'IsZmodnZObjNonprime'.
##  'IsZmodpZObj' is an abbreviation of 'IsZmodnZObj and IsFFE'.
##  This category is the disjoint union of 'IsZmodpZObjSmall' and
##  'IsZmodpZObjLarge', the former containing all elements with $n$ at most
##  'MAXSIZE_GF_INTERNAL'.
##
##  The reasons to distinguish the prime case from the nonprime case are
##  - that objects in 'IsZmodnZObjNonprime' have an external representation
##    (namely the modulus in the range $[ 0, 1, ... n-1 ]$),
##  - that the comparison of elements can be defined as comparison of the
##    moduli, and
##  - that the elements lie in a family of type 'IsZmodnZObjNonprimeFamily'
##    (note that for prime $n$, the family must be an 'IsFFEFamily').
##
##  The reasons to distinguish the small and the large case are
##  that for small $n$ the elements must be compatible with the internal
##  representation of finite field elementsi, whereas we are free to define
##  comparison as comparison of moduli for large $n$.
##
##  Note that we cannot claim that every finite field element of degree 1
##  is in 'IsZmodnZObj', since finite field elements in internal
##  representation may not know that they lie in the prime field.
##
IsZmodnZObj := NewCategory( "IsZmodnZObj", IsScalar );
IsZmodnZObjNonprime := NewCategory( "IsZmodnZObjNonprime", IsZmodnZObj );
IsZmodpZObj := IsZmodnZObj and IsFFE;
IsZmodpZObjSmall := NewCategory( "IsZmodpZObjSmall", IsZmodpZObj );
IsZmodpZObjLarge := NewCategory( "IsZmodpZObjLarge", IsZmodpZObj );


#############################################################################
##
#C  IsZmodnZObjNonprimeFamily( <obj> )
##
IsZmodnZObjNonprimeFamily := CategoryFamily( "IsZmodnZObjNonprimeFamily",
    IsZmodnZObjNonprime );


#############################################################################
##
#C  IsZmodnZObjNonprimeCollection( <obj> )
##
IsZmodnZObjNonprimeCollection := CategoryCollections(
    "IsZmodnZObjNonprimeCollection", IsZmodnZObjNonprime );


#############################################################################
##
#O  ZmodnZObj( <Fam>, <i> )
##
ZmodnZObj := NewConstructor( "ZmodnZObj",
    [ IsZmodnZObjNonprimeFamily, IsInt ] );


#############################################################################
##
#V  Z_MOD_NZ
##
##  is a list of length 2, the first containing at position <i> the <i>-th
##  'ZmodnZ( <n> )' that has been constructed already, and the second
##  containing this ring at position <i>.
##
Z_MOD_NZ := [ [], [] ];


#############################################################################
##
#F  ZmodnZ( <n> )
#F  ZmodpZ( <p> )
#F  ZmodpZNC( <p> )
##
##  Each ring $\Z / n \Z$ contains the whole elements family if $n$ is not a
##  prime, and is embedded into the family of finite field elements of
##  characteristic $n$ otherwise.
##
ZmodnZ := NewOperationArgs( "ZmodnZ" );
ZmodpZ := NewOperationArgs( "ZmodpZ" );
ZmodpZNC := NewOperationArgs( "ZmodpZNC" );


#############################################################################
##
#E  zmodnz.gd . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here



