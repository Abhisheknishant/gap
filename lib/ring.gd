#############################################################################
##
#W  ring.gd                     GAP library                     Thomas Breuer
##
#H  @(#)$Id$
##
#Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
##
##  This file declares the operations for rings.
##
Revision.ring_gd :=
    "@(#)$Id$";


#############################################################################
##
#P  IsLDistributive( <R> )
##
##  is 'true' if the relation $a \* ( b + c ) = ( a \* b ) + ( a \* c )$
##  holds for all elements $a$, $b$, $c$ in the ring <R>,
##  and 'false' otherwise.
##
IsLDistributive := NewProperty( "IsLDistributive", IsRingElementCollection );
SetIsLDistributive := Setter( IsLDistributive );
HasIsLDistributive := Tester( IsLDistributive );

InstallSubsetTrueMethod( IsLDistributive,
    IsRingElementCollection and IsLDistributive,
    IsRingElementCollection );

InstallFactorTrueMethod( IsLDistributive,
    IsRingElementCollection and IsLDistributive,
    IsRingElementCollection,
    IsRingElementCollection );


#############################################################################
##
#P  IsRDistributive( <R> )
##
##  is 'true' if the relation $( a + b ) \* c = ( a \* c ) + ( b \* c )$
##  holds for all elements $a$, $b$, $c$ in the ring <R>,
##  and 'false' otherwise.
##
IsRDistributive := NewProperty( "IsRDistributive", IsRingElementCollection );
SetIsRDistributive := Setter( IsRDistributive );
HasIsRDistributive := Tester( IsRDistributive );

InstallSubsetTrueMethod( IsRDistributive,
    IsRingElementCollection and IsRDistributive,
    IsRingElementCollection );

InstallFactorTrueMethod( IsRDistributive,
    IsRingElementCollection and IsRDistributive,
    IsRingElementCollection,
    IsRingElementCollection );


#############################################################################
##
#P  IsDistributive( <D> )
##
IsDistributive := IsLDistributive and IsRDistributive;
SetIsDistributive := Setter( IsDistributive );
HasIsDistributive := Tester( IsDistributive );


#############################################################################
##
#C  IsRing( <R> )
##
##  A ring in {\GAP} is an additive group that is also a magma,
##  such that addition and multiplication are distributive.
##  (The multiplication need *not* be associative.)
##
IsRing := IsAdditiveGroup and IsMagma and IsDistributive;
SetIsRing := Setter( IsRing );
HasIsRing := Tester( IsRing );


#############################################################################
##
#C  IsUnitalRing( <R> )
##
##  A unital ring in {\GAP} is an additive group that is also a
##  magma-with-one,
##  such that addition and multiplication are distributive.
##  (The multiplication need not be associative.)
##
IsUnitalRing := IsAdditiveGroup and IsMagmaWithOne and IsDistributive;
SetIsUnitalRing := Setter( IsUnitalRing );
HasIsUnitalRing := Tester( IsUnitalRing );


#############################################################################
##
#C  IsUniqueFactorizationRing( <R> )
##
##  A ring <R> is  called a *unique factorization ring* if it is an integral
##  ring, and every element has a unique factorization into irreducible
##  elements, i.e., a  unique representation as product  of irreducibles (see
##  "IsIrreducible").
##  Unique in this context means unique up to permutations of the factors and
##  up to multiplication of the factors by units (see "Units").
##  
IsUniqueFactorizationRing := NewCategory( "IsUniqueFactorizationRing",
    IsRing );

#T InstallSubsetTrueMethod( IsUniqueFactorizationRing,
#T     IsRing and IsUniqueFactorizationRing, IsRing );
#T ???


#############################################################################
##
#C  IsEuclideanRing( <R> )
##  
##  A ring $R$ is called a Euclidean ring if it is an integral ring and there
##  exists a function $\delta$, called the Euclidean degree, from $R-\{0_R\}$
##  to the nonnegative integers, such that for every pair $r \in R$ and
##  $s \in  R-\{0_R\}$ there exists an element $q$ such that either
##  $r - q s = 0_R$ or $\delta(r - q s) \< \delta( s )$.
##  The existence of this division with remainder implies that the Euclidean
##  algorithm can be applied to compute a greatest common divisor of two
##  elements, which in turn implies that $R$ is a unique factorization ring.
##
#T new category ``valuated domain''?
##
IsEuclideanRing := NewCategory( "IsEuclideanRing",
    IsUnitalRing and IsUniqueFactorizationRing );


#############################################################################
##
#P  IsAnticommutative( <R> )
##
##  is 'true' if the relation $a \* b = - b \* a$
##  holds for all elements $a$, $b$ in the ring <R>,
##  and 'false' otherwise.
##
IsAnticommutative := NewProperty( "IsAnticommutative", IsRing );
SetIsAnticommutative := Setter( IsAnticommutative );
HasIsAnticommutative := Tester( IsAnticommutative );

InstallSubsetTrueMethod( IsAnticommutative,
    IsRing and IsAnticommutative, IsRing );

InstallFactorTrueMethod( IsAnticommutative,
    IsRing and IsAnticommutative, IsRing, IsRing );


#############################################################################
##
#P  IsIntegralRing( <R> )
##
##  A ring <R> is integral if it is commutative and contains no nontrivial
##  zero divisors.
##
IsIntegralRing := NewProperty( "IsIntegralRing", IsRing );
SetIsIntegralRing := Setter( IsIntegralRing );
HasIsIntegralRing := Tester( IsIntegralRing );

InstallSubsetTrueMethod( IsIntegralRing, IsRing and IsIntegralRing, IsRing );

#T method that fetches this from the family if possible?

InstallTrueMethod( IsIntegralRing, IsRing and IsMagmaWithInversesAndZero );
InstallTrueMethod( IsIntegralRing, IsUniqueFactorizationRing );


#############################################################################
##
#P  IsJacobianRing( <R> )
##
##  is 'true' if and only if the Jacobi identity holds in <R>, that is,
##  $x \* y \* z + z \* x \* y + y \* z \* x$ is the zero element of <R>,
##  for all elements $x$, $y$, $z$ in <R>.
##
IsJacobianRing := NewProperty( "IsJacobianRing", IsRing );
SetIsJacobianRing := Setter( IsJacobianRing );
HasIsJacobianRing := Tester( IsJacobianRing );

InstallCollectionsTrueMethod( IsJacobianRing,
    IsJacobianElement, IsRing );
InstallSubsetTrueMethod( IsJacobianRing,
    IsRing and IsJacobianRing, IsRing );
InstallFactorTrueMethod( IsJacobianRing,
    IsRing and IsJacobianRing, IsRing, IsRing );


#############################################################################
##
#P  IsZeroSquaredRing( <R> )
##
##  is 'true' if $a \* a$ is the zero element of the ring <R>
##  for all $a$ in <R>, and 'false' otherwise.
##
IsZeroSquaredRing := NewProperty( "IsZeroSquaredRing", IsRing );
SetIsZeroSquaredRing := Setter( IsZeroSquaredRing );
HasIsZeroSquaredRing := Tester( IsZeroSquaredRing );

InstallTrueMethod( IsAnticommutative, IsRing and IsZeroSquaredRing );

InstallCollectionsTrueMethod( IsZeroSquaredRing,
    IsZeroSquaredElement, IsRing );
InstallSubsetTrueMethod( IsZeroSquaredRing,
    IsRing and IsZeroSquaredRing, IsRing );
InstallFactorTrueMethod( IsZeroSquaredRing,
    IsRing and IsZeroSquaredRing, IsRing, IsRing );


#############################################################################
##
#A  AsRing( <R> )
##
AsRing := NewAttribute( "AsRing", IsRingElementCollection );
SetAsRing := Setter( AsRing );
HasAsRing := Tester( AsRing );


#############################################################################
##
#A  GeneratorsOfRing( <R> )
##
GeneratorsOfRing := NewAttribute( "GeneratorsOfRing", IsRing );
SetGeneratorsOfRing := Setter( GeneratorsOfRing );
HasGeneratorsOfRing := Tester( GeneratorsOfRing );


#############################################################################
##
#A  GeneratorsOfUnitalRing( <R> )
##
GeneratorsOfUnitalRing := NewAttribute( "GeneratorsOfUnitalRing",
    IsUnitalRing );
SetGeneratorsOfUnitalRing := Setter( GeneratorsOfUnitalRing );
HasGeneratorsOfUnitalRing := Tester( GeneratorsOfUnitalRing );


#############################################################################
##
#A  Units( <R> )
##
##  'Units' returns the group of units of the ring <R>.
##  This may either be returned as a list or as a group.
##  
##  An element $r$ is called a *unit* of a ring $R$, if $r$ has an inverse in
##  $R$.
##  It is easy to see that the set of units forms a multiplicative group.
##  
Units := NewAttribute( "Units", IsRing );
SetUnits := Setter( Units );
HasUnits := Tester( Units );


#############################################################################
##
#O  Factors( <r> )
#O  Factors( <R>, <r> )
##
##  In the first form 'Factors' returns the factorization of the ring element
##  <r> in its default ring (see "DefaultRing").
##  In the second form 'Factors' returns the factorization of the ring
##  element <r> in the ring <R>.
##  The factorization is returned as a list of primes (see "IsPrime").
##  Each element in the list is a standard associate (see
##  "StandardAssociate") except the first one, which is multiplied by a unit
##  as necessary to have 'Product( Factors( <R>, <r> )  )  = <r>'.
##  This list is usually also sorted, thus smallest prime factors come first.
##  If <r> is a unit or zero, 'Factors( <R>, <r> ) = [ <r> ]'.
##  
Factors:= NewOperation( "Factors", [ IsRing, IsRingElement ] );
#T who does really need the additive structure?


#############################################################################
##
#O  IsAssociated( <r>, <s> )
#O  IsAssociated( <R>, <r>, <s> )
##
##  In the first form 'IsAssociated' returns 'true' if the two ring elements
##  <r> and <s> are associated in their default ring (see "DefaultRing") and
##  'false' otherwise.
##  In the second form 'IsAssociated' returns 'true' if the two ring elements
##  <r> and <s> are associated in the ring <R> and 'false' otherwise.
##  
##  Two elements $r$ and $s$ of a ring $R$ are called *associates* if there
##  is a unit $u$ of $R$ such that $r u = s$.
##  
IsAssociated := NewOperation( "IsAssociated",
    [ IsRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#O  Associates( <r> )
#O  Associates( <R>, <r> )
##  
##  In the first form 'Associates' returns the set of associates of the ring
##  element <r> in its default ring (see "DefaultRing").
##  In the second form 'Associates' returns the set of associates of <r> in
##  the ring <R>.
##  
##  Two elements $r$ and $s$ of a ring $R$ are called *associate* if there is
##  a unit $u$ of $R$ such that $r u = s$.
##  
Associates := NewOperation( "Associates",
    [ IsRing, IsRingElement ] );


#############################################################################
##
#O  IsIdeal( <R>, <I> )
#O  IsLeftIdeal( <R>, <I> )
#O  IsRightIdeal( <R>, <I> )
##
IsIdeal:= NewOperation( "IsIdeal", [ IsRing, IsRing ] );
IsLeftIdeal:= NewOperation( "IsLeftIdeal", [ IsRing, IsRing ] );
IsRightIdeal:= NewOperation( "IsRightIdeal", [ IsRing, IsRing ] );


#############################################################################
##
#P  IsLeftIdealInParent( <R> )
##
IsLeftIdealInParent := NewProperty( "IsLeftIdealInParent", IsRing );
SetIsLeftIdealInParent := Setter( IsLeftIdealInParent );
HasIsLeftIdealInParent := Tester( IsLeftIdealInParent );


#############################################################################
##
#P  IsRightIdealInParent( <R> )
##
IsRightIdealInParent := NewProperty( "IsRightIdealInParent", IsRing );
SetIsRightIdealInParent := Setter( IsRightIdealInParent );
HasIsRightIdealInParent := Tester( IsRightIdealInParent );


#############################################################################
##
#P  IsIdealInParent( <R> )
##
IsIdealInParent := IsLeftIdealInParent and IsRightIdealInParent;
SetIsIdealInParent := Setter( IsIdealInParent );
HasIsIdealInParent := Tester( IsIdealInParent );


#############################################################################
##
#O  IdealByGenerators( <R>, <gens> )  . . .  ideal in <R> generated by <gens>
##
##  is a subring with parent <R> that knows that it is an ideal
##  in its parent and that has <gens> as ideal generators.
##
IdealByGenerators := NewConstructor( "IdealByGenerators",
    [ IsRing, IsCollection ] );


#############################################################################
##
#O  LeftIdealByGenerators( <R>, <gens> )
##
##  is a subring with parent <R> that knows that it is a left ideal
##  in its parent and that has <gens> as left ideal generators.
##
LeftIdealByGenerators := NewConstructor( "LeftIdealByGenerators",
    [ IsRing, IsCollection ] );


#############################################################################
##
#O  RightIdealByGenerators( <R>, <gens> )
##
##  is a subring with parent <R> that knows that it is a right ideal
##  in its parent and that has <gens> as right ideal generators.
##
RightIdealByGenerators := NewConstructor( "RightIdealByGenerators",
    [ IsRing, IsCollection ] );


#############################################################################
##
#A  GeneratorsOfIdeal( <I> )
##
##  is the list of generators for <I> as ideal in its parent.
##
GeneratorsOfIdeal:= NewAttribute( "GeneratorsOfIdeal", IsRing );
SetGeneratorsOfIdeal:= Setter( GeneratorsOfIdeal );
HasGeneratorsOfIdeal:= Tester( GeneratorsOfIdeal );


#############################################################################
##
#A  GeneratorsOfLeftIdeal( <I> )
##
##  is the list of generators for <I> as left ideal in its parent.
##
GeneratorsOfLeftIdeal:= NewAttribute( "GeneratorsOfLeftIdeal", IsRing );
SetGeneratorsOfLeftIdeal:= Setter( GeneratorsOfLeftIdeal );
HasGeneratorsOfLeftIdeal:= Tester( GeneratorsOfLeftIdeal );


#############################################################################
##
#A  GeneratorsOfRightIdeal( <I> )
##
##  is the list of generators for <I> as right ideal in its parent.
##
GeneratorsOfRightIdeal:= NewAttribute( "GeneratorsOfRightIdeal", IsRing );
SetGeneratorsOfRightIdeal:= Setter( GeneratorsOfRightIdeal );
HasGeneratorsOfRightIdeal:= Tester( GeneratorsOfRightIdeal );


#############################################################################
##
#O  IsUnit( <r> ) . . . . . . check whether <r> is a unit in its default ring
#O  IsUnit( <R>, <r> )  . . . . . . . . .  check whether <r> is a unit in <R>
##
##  In the first form 'IsUnit' returns 'true' if the ring element <r> is a
##  unit in its default ring (see "DefaultRing").
##  In the second form 'IsUnit' returns 'true' if <r> is a unit in the ring
##  <R>.
##  
##  An element $r$ is called a *unit* in a ring $R$, if $r$ has an inverse in
##  $R$.
##
##  'IsUnit' may call 'Quotient'.
##
IsUnit := NewOperation( "IsUnit", [ IsRing, IsRingElement ] );


#############################################################################
##
#O  InterpolatedPolynomial( <R>, <x>, <y> ) . . . . . . . . . . interpolation
##
##  'InterpolatedPolynomial' returns, for given lists <x>, <y> of elements in
##  a ring <R> of the same length $n$, say, the unique  polynomial of  degree
##  less than $n$ which has value <y>[i] at <x>[i], for all $i=1,...,n$. Note
##  that the elements in <x> must be distinct.
##
InterpolatedPolynomial := NewOperation( "InterpolatedPolynomial",
    [ IsRing, IsHomogeneousList, IsHomogeneousList ] );


#############################################################################
##
#O  Quotient( <r>, <s> )
#O  Quotient( <R>, <r>, <s> )
##
##  In the first form 'Quotient' returns the quotient of the two ring
##  elements <r> and <s> in  their default ring.
##  In the second form 'Quotient' returns the quotient of the two ring
##  elements <r> and <s> in the ring <R>.
##  It returns 'fail' if the quotient does not exist in the respective ring.
##  
##  (To perform the division in the quotient field of a ring, use the
##  quotient operator '/'.)
##
Quotient := NewOperation( "Quotient",
    [ IsRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#O  StandardAssociate( <r> )
#O  StandardAssociate( <R>, <r> )
##
##  In the first form 'StandardAssociate' returns the standard associate of
##  the ring element <r> in its default ring (see "DefaultRing").
##  In the second form 'StandardAssociate' returns the standard associate of
##  the ring element <r> in the ring <R>.
##  
##  The *standard associate* of a ring element $r$ of $R$ is an associated
##  element of $r$ which is, in a ring dependent way, distinguished among the
##  set of associates of $r$.
##  For example, in the ring of integers the standard associate is the
##  absolute value.
##  
StandardAssociate := NewOperation( "StandardAssociate",
    [ IsRing, IsRingElement ] );


#############################################################################
##
#O  IsPrime( <r> )
#O  IsPrime( <R>, <r> )
##
##  In the first form 'IsPrime' returns 'true' if the ring element <r> is a
##  prime in its default ring (see "DefaultRing") and 'false' otherwise.
##  In the second form 'IsPrime' returns 'true' if the ring element <r> is a
##  prime in the ring <R> and 'false' otherwise.
##  
##  An element $r$ of a ring $R$ is called *prime* if for each pair $s$ and
##  $t$ such that $r$ divides $s t$ the element $r$ divides either $s$ or
##  $t$.
##  Note that there are rings where not every irreducible element
##  (see "IsIrreducible") is a prime.
##
IsPrime := NewOperation( "IsPrime", [ IsRing, IsRingElement ] );


#############################################################################
##
#O  IsIrreducible( <r> )
#O  IsIrreducible( <R>, <r> )
##
##  In the first form 'IsIrreducible' returns 'true' if the ring element <r>
##  is irreducible in its default ring (see "DefaultRing") and 'false'
##  otherwise.
##  In the second form 'IsIrreducible' returns 'true' if the ring element <r>
##  is irreducible in the ring <R> and 'false' otherwise.
##  
##  An element $r$ of a ring $R$ is called *irreducible* if there is no
##  nontrivial factorization of $r$ in $R$, i.e., if there is no
##  representation of $r$ as product $s t$ such that neither $s$ nor $t$ is a
##  unit (see "IsUnit").
##  Each prime element (see "IsPrime") is irreducible.
##  
IsIrreducible := NewOperation( "IsIrreducible", [ IsRing, IsRingElement ] );


#############################################################################
##
#O  EuclideanDegree( <r> )
#O  EuclideanDegree( <R>, <r> )
##
##  In the first form 'EuclideanDegree' returns the Euclidean degree of the
##  ring element <r> in its default ring.
##  In the second form 'EuclideanDegree' returns the Euclidean degree of the
##  ring element in the ring <R>.
##  <R> must of course be a Euclidean ring (see "IsEuclideanRing").
##  
EuclideanDegree := NewOperation( "EuclideanDegree",
    [ IsEuclideanRing, IsRingElement ] );


#############################################################################
##
#O  EuclideanRemainder( <r>, <m> )
#O  EuclideanRemainder( <R>, <r>, <m> )
##
##  In the first form 'EuclideanRemainder' returns the remainder of the ring
##  element <r> modulo the ring element <m> in their default ring.
##  In the second form 'EuclideanRemainder' returns the remainder of the ring
##  element <r> modulo the ring element <m> in the ring <R>.
##  The ring <R> must be a Euclidean ring (see "IsEuclideanRing") otherwise
##  an error is signalled.
##  
EuclideanRemainder := NewOperation( "EuclideanRemainder",
    [ IsEuclideanRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#O  EuclideanQuotient( <r>, <m> )
#O  EuclideanQuotient( <R>, <r>, <m> )
##
##  In the first form 'EuclideanQuotient' returns the Euclidean quotient of
##  the ring elements <r> and <m> in their default ring.
##  In the second form 'EuclideanQuotient' returns the Euclidean quotient of
##  the ring elements <r>and <m> in the ring <R>.
##  The ring <R> must be a Euclidean ring (see "IsEuclideanRing") otherwise
##  an error is signalled.
##  
EuclideanQuotient := NewOperation( "EuclideanQuotient",
    [ IsEuclideanRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#O  QuotientRemainder( <r>, <s> )
#O  QuotientRemainder( <R>, <r>, <s> )
##
##  In the first form 'QuotientRemainder' returns the Euclidean quotient and
##  the Euclidean remainder of the ring elements <r> and <m> in their default
##  ring as pair of ring elements.
##  In the second form 'QuotientRemainder' returns the Euclidean quotient
##  and the Euclidean remainder of the ring elements <r> and <m> in the ring
##  <R>.
##  The ring <R> must be a Euclidean ring (see "IsEuclideanRing") otherwise
##  an error is signalled.
##  
QuotientRemainder := NewOperation( "QuotientRemainder",
    [ IsRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#O  QuotientMod( <r>, <s>, <m> )
#O  QuotientMod( <R>, <r>, <s>, <m> )
##
##  In the first form 'QuotientMod' returns the quotient of the ring elements
##  <r> and  <s> modulo the ring element <m> in their default ring (see
##  "DefaultRing").
##  In the second form 'QuotientMod' returns the quotient of the ring
##  elements <r> and <s> modulo the ring element <m> in the ring <R>.
##  <R> must be a Euclidean ring (see "IsEuclideanRing") so that
##  'EuclideanRemainder' (see "EuclideanRemainder") can be applied.
##  If the modular quotient does not exist, 'fail' is returned.
##  
##  The quotient $q$ of $r$ and $s$ modulo $m$ is an element of $R$ such that
##  $q s = r$ modulo $m$, i.e., such that $q s - r$ is divisible by $m$ in
##  $R$ and that $q$ is either 0 (if $r$ is divisible by $m$) or the
##  Euclidean degree of $q$ is strictly smaller than the Euclidean degree of
##  $m$.
##  
QuotientMod := NewOperation( "QuotientMod",
    [ IsRing, IsRingElement, IsRingElement, IsRingElement ] );


#############################################################################
##
#O  PowerMod( <r>, <e>, <m> )
#O  PowerMod( <R>, <r>, <e>, <m> )
##
##  In the first form 'PowerMod' returns the <e>-th power of the ring element
##  <r> modulo the ring element <m> in their default ring (see
##  "DefaultRing").
##  In the second form 'PowerMod' returns the <e>-th power of the ring
##  element <r> modulo the ring element <m> in the ring <R>.
##  <e> must be an integer.
##  <R> must be a Euclidean ring (see "IsEuclideanRing") so that
##  'EuclideanRemainder' (see "EuclideanRemainder") can be applied to its
##  elements.
##  
##  If $e$ is positive the result is $r^e$ modulo $m$.
##  If $e$ is negative then 'PowerMod' first tries to find the inverse of $r$
##  modulo $m$, i.e., $i$ such that $i r = 1$ modulo $m$.
##  If the inverse does not exist an error is signalled.
##  If the inverse does exist 'PowerMod' returns
##  'PowerMod( <R>, <i>, -<e>, <m> )'.
##  
##  'PowerMod' reduces the intermediate values modulo $m$, improving
##  performance drastically when <e> is large and <m> small.
##  
PowerMod := NewOperation( "PowerMod",
    [ IsRing, IsRingElement, IsInt, IsRingElement ] );


#############################################################################
##
#O  Gcd( <r>, <s> )
#O  Gcd( <R>, <r>, <s> )
##
##  In the first form 'Gcd' returns the greatest common divisor of the ring
##  elements <r>, <s> in their default ring (see "DefaultRing").
##  In the second form 'Gcd' returns the greatest common divisor of the ring
##  elements <r>, <s> in the ring <R>.
##  <R> must be a Euclidean ring (see "IsEuclideanRing") so that
##  'QuotientRemainder' (see "QuotientRemainder") can be applied to its
##  elements.
##  'Gcd' returns the standard associate (see "StandardAssociate") of the
##  greatest common divisors.
##  
##  A greatest common divisor of the elements $r_1$, $r_2$... etc. of the
##  ring $R$ is an element of largest Euclidean degree (see
##  "EuclideanDegree") that is a divisor of $r_1$, $r_2$... etc.
##  We define $gcd( r, 0_R ) = gcd( 0_R, r ) = StandardAssociate( r )$
##  and $gcd( 0_R, 0_R ) = 0_R$.
##  
Gcd := NewOperation( "Gcd",
    [ IsEuclideanRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#O  GcdRepresentation( <r>, <s> )
#O  GcdRepresentation( <R>, <r>, <s> )
##
##  In the first form 'GcdRepresentation' returns the representation of the
##  greatest common divisor of the ring elements <r>, <s> in their default
##  ring (see "DefaultRing").
##  In the second form 'GcdRepresentation' returns the representation of the
##  greatest common divisor of the ring elements <r>, <s> in the ring <R>.
##  <R> must be a Euclidean ring (see "IsEuclideanRing") so that 'Gcd'
##  (see "Gcd") can be applied to its elements.
##  The representation is returned as a list of ring elements.
##  
##  The representation of the gcd  $g$ of  the elements $r_1$, $r_2$...  etc.
##  of a ring $R$ is a list of ring elements $s_1$, $s_2$... etc. of $R$,
##  such that $g = s_1 r_1 + s_2  r_2 ...$.
##  That this representation exists can be shown using the Euclidean
##  algorithm, which in fact can compute those coefficients.
##  
GcdRepresentation := NewOperation( "GcdRepresentation",
    [ IsEuclideanRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#O  Lcm( <r>, <s> )
#O  Lcm( <R>, <r>, <s> )
##
##  In the first form 'Lcm' returns the least common multiple of the ring
##  elements <r>, <s> in their default ring (see "DefaultRing").
##  In the second form 'Lcm' returns the least common multiple of the ring
##  elements <r>, <s> in the ring <R>.
##  <R> must be a Euclidean ring (see "IsEuclideanRing") so that 'Gcd'
##  (see "Gcd") can be applied to its elements.
##  'Lcm' returns the standard associate (see "StandardAssociate") of the
##  least common multiples.
##  
##  A least common multiple of the elements $r_1$, $r_2$... etc. of the
##  ring $R$ is an element of smallest Euclidean degree
##  (see "EuclideanDegree") that is a multiple of $r_1$, $r_2$... etc.
##  We define $lcm( r, 0_R ) = lcm( 0_R, r ) = StandardAssociate( r )$
##  and $Lcm( 0_R, 0_R ) = 0_R$.
##  
##  'Lcm' uses the equality $lcm(m,n) = m\*n / gcd(m,n)$ (see "Gcd").
##  
Lcm := NewOperation( "Lcm",
    [ IsEuclideanRing, IsRingElement, IsRingElement ] );


#############################################################################
##
#A  NiceRing( <D> )
##
NiceRing := NewAttribute( "NiceRing", IsRing );


#############################################################################
##
#O  RingByGenerators( [ <z>, ... ] )  . .  ring gener. by elements in a coll.
##
RingByGenerators := NewOperation( "RingByGenerators",
    [ IsRingElementCollection ] );


#############################################################################
##
#F  DefaultRingByGenerators( [ <z>, ... ] ) . default ring containing a coll.
##
DefaultRingByGenerators := NewOperation( "DefaultRingByGenerators",
    [ IsRingElementCollection ] );


#############################################################################
##
#F  Ring( <r> ,<s>, ... )  . . . . . . . . . . ring generated by a collection
#F  Ring( <coll> ) . . . . . . . . . . . . . . ring generated by a collection
##
##  In the first form 'Ring' returns the smallest ring that
##  contains all the elements <r>, <s>... etc.
##  In the second form 'Ring' returns the smallest ring that
##  contains all the elements in the collection <coll>.
##  If any element is not an element of a ring or if the elements lie in no
##  common ring an error is raised.
##  
##  'Ring' differs from 'DefaultRing' (see "DefaultRing") in that it returns
##  the smallest ring in which the elements lie, while 'DefaultRing' may
##  return a larger ring if that makes sense.
##
Ring := NewOperationArgs( "Ring" );


#############################################################################
##
#O  UnitalRingByGenerators( [ <z>, ... ] )
##
UnitalRingByGenerators := NewOperation( "UnitalRingByGenerators",
    [ IsRingElementCollection ] );


#############################################################################
##
#F  UnitalRing( <r>, <s>, ... )  . . .  unital ring generated by a collection
#F  UnitalRing( <coll> ) . . . . . . .  unital ring generated by a collection
##
##  In the first form 'UnitalRing' returns the smallest ring with one that
##  contains all the elements <r>, <s>... etc.
##  In the second form 'UnitalRing' returns the smallest ring with one that
##  contains all the elements in the collection <coll>.
##  If any element is not an element of a ring or if the elements lie in no
##  common ring an error is raised.
##  
UnitalRing := NewOperationArgs( "UnitalRing" );


#############################################################################
##
#F  DefaultRing( <r> ,<s>, ... )  . . .  default ring containing a collection
#F  DefaultRing( <coll> ) . . . . . . .  default ring containing a collection
##
##  In the first form 'DefaultRing' returns a ring that contains
##  all the elements <r>, <s>, ... etc.
##  In the second form 'DefaultRing' returns a ring that contains
##  all the elements in the collection <coll>.
##  If any element is not an element of a ring or if the elements lie in no
##  common ring an error is raised.
##  
##  The ring returned by 'DefaultRing' need not be the smallest ring in which
##  the elements lie.
##  For example for elements from cyclotomic fields 'DefaultRing' may return
##  the ring of integers of the smallest cyclotomic field in which the elements
##  lie, which need not be the smallest ring overall, because the elements may
##  in fact lie in a smaller number field which is not a cyclotomic field.
##  
##  (For the exact definition of the default ring of a certain type of elements
##  look at the corresponding method installation.)
##  
##  'DefaultRing' is used by the ring functions like 'Quotient', 'IsPrime',
##  'Factors', or 'Gcd' if no explicit ring is given.
##  
##  'Ring' (see "Ring") differs from 'DefaultRing' in that it returns the
##  smallest ring in which the elements lie, while 'DefaultRing' may return a
##  larger ring if that makes sense.
##
DefaultRing := NewOperationArgs( "DefaultRing" );


#############################################################################
##
#F  Subring( <R>, <gens> ) . . . . . . . . subring of <R> generated by <gens>
#F  SubringNC( <R>, <gens> ) . . . . . . . subring of <R> generated by <gens>
##
Subring := NewOperationArgs( "Subring" );
SubringNC := NewOperationArgs( "SubringNC" );


#############################################################################
##
#F  UnitalSubring( <R>, <gens> )  . unital subring of <R> generated by <gens>
#F  UnitalSubringNC( <R>, <gens> )  unital subring of <R> generated by <gens>
##
UnitalSubring := NewOperationArgs( "UnitalSubring" );
UnitalSubringNC := NewOperationArgs( "UnitalSubringNC" );


#############################################################################
##
#P  IsRingHomomorphism( <map> )
##
##  A mapping $f$ is a ring homomorphism if source and range are rings,
##  and if $f( a + b ) = f(a) + f(b)$ and $f( a \* b ) = f(a) \* f(b)$ hold
##  for all elements $a$, $b$ in the source of $f$.
##
IsRingHomomorphism := NewProperty( "IsRingHomomorphism", IsMapping );
SetIsRingHomomorphism := Setter( IsRingHomomorphism );
HasIsRingHomomorphism := Tester( IsRingHomomorphism );


#############################################################################
##
#A  KernelRingHomomorphism( <fun> ) . . . . . . kernel of a ring homomorphism
##
KernelRingHomomorphism := NewAttribute( "KernelRingHomomorphism",
    IsGeneralMapping );


#############################################################################
##
#E  ring.gd . . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here



