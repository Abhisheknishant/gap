#############################################################################
##
#W  algsc.gi                    GAP library                     Thomas Breuer
##
#H  @(#)$Id$
##
#Y  Copyright (C)  1997,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
##
##  This file contains methods for elements of algebras given by structure
##  constants (s.c.).
##
##  The family of s.c. algebra elements has the following components.
##
##  'sctable' :
##        the structure constants table,
##  'names' :
##        list of names of the basis vectors (for printing only),
##  'zerocoeff' :
##        the zero coefficient (needed already for the s.c. table),
##  'defaultKindDenseCoeffVectorRep' :
##        the kind of s.c. algebra elements that are represented by
##        a dense list of coefficients.
##
##  If the family has *not* the category 'IsFamilyOverFullCoefficientsFamily'
##  then it has the component 'coefficientsDomain'.
##
Revision.algsc_gi :=
    "@(#)$Id$";


#############################################################################
##
#M  IsWholeFamily( <V> )  . . . . . . .  for s.c. algebra elements collection
##
InstallMethod( IsWholeFamily,
    "method for s.c. algebra elements collection",
    true,
    [ IsSCAlgebraObjCollection and IsLeftModule and IsFreeLeftModule ], 0,
    function( V )
    local Fam;
    Fam:= ElementsFamily( FamilyObj( V ) );
    if IsFamilyOverFullCoefficientsFamily( Fam ) then
      return     IsWholeFamily( LeftActingDomain( V ) )
             and IsFullSCAlgebra( V );
    else
      return     LeftActingDomain( V ) = Fam!.coefficientsDomain
             and IsFullSCAlgebra( V );
    fi;
    end );


#############################################################################
##
#R  IsDenseCoeffVectorRep( <obj> )
##
##  This representation uses a coefficients vector
##  w.r.t. the basis that is known for the whole family.
##
##  The external representation is the coefficients vector,
##  which is stored at position 1 in the object.
##
IsDenseCoeffVectorRep := NewRepresentation( "IsDenseCoeffVectorRep",
    IsPositionalObjectRep, [ 1 ] );


#############################################################################
##
#M  ObjByExtRep( <Fam>, <descr> ) . . . . . . . . . for s.c. algebra elements
##
##  Check whether the coefficients list <coeffs> has the right length,
##  and lies in the correct family.
##  If the coefficients family of <Fam> has a uniquely determined zero
##  element, we need to check only whether the family of <descr> is the
##  collections family of the coefficients family of <Fam>.
##
InstallMethod( ObjByExtRep,
    "method for s.c. algebra elements family",
    true,
    [ IsSCAlgebraObjFamily, IsHomogeneousList ], 0,
    function( Fam, coeffs )
    if    IsFamilyOverFullCoefficientsFamily( Fam )
       or not IsBound( Fam!.coefficientsDomain ) then
      TryNextMethod();
    elif Length( coeffs ) <> Length( Fam!.names ) then
      Error( "<coeffs> must be a list of length ", Fam!.names );
    elif not ForAll( coeffs, c -> c in Fam!.coefficientsDomain ) then
      Error( "all in <coeffs> must lie in '<Fam>!.coefficientsDomain'" );
    fi;
    return Objectify( Fam!.defaultKindDenseCoeffVectorRep,
                      [ Immutable( coeffs ) ] );
    end );

InstallMethod( ObjByExtRep,
    "method for s.c. alg. elms. family with coefficients family",
    true,
    [ IsSCAlgebraObjFamily and IsFamilyOverFullCoefficientsFamily,
      IsHomogeneousList ], 0,
    function( Fam, coeffs )
    if not IsIdentical( CoefficientsFamily( Fam ),
                        ElementsFamily( FamilyObj( coeffs ) ) ) then
      Error( "family of <coeffs> does not fit to <Fam>" );
    elif Length( coeffs ) <> Length( Fam!.names ) then
      Error( "<coeffs> must be a list of length ", Fam!.names );
    fi;
    return Objectify( Fam!.defaultKindDenseCoeffVectorRep,
                      [ Immutable( coeffs ) ] );
    end );


#############################################################################
##
#M  ExtRepOfObj( <elm> )  . . . . . . . . . . . . . for s.c. algebra elements
##
InstallMethod( ExtRepOfObj,
    "method for s.c. algebra element in dense coeff. vector rep.",
    true,
    [ IsSCAlgebraObj and IsDenseCoeffVectorRep ], 0,
    elm -> elm![1] );


#############################################################################
##
#M  Print( <elm> )  . . . . . . . . . . . . . . . . for s.c. algebra elements
##
InstallMethod( PrintObj,
    "method for s.c. algebra element",
    true,
    [ IsSCAlgebraObj ], 0,
    function( elm )

    local F,      # family of 'elm'
          names,  # generators names
          len,    # dimension of the algebra
          zero,   # zero element of the ring
          depth,  # first nonzero position in coefficients list
          one,    # identity element of the ring
          i;      # loop over the coefficients list
   
    F     := FamilyObj( elm );
    names := F!.names;
    elm   := ExtRepOfObj( elm );
    len   := Length( elm );
    zero  := Zero( elm[1] );
    depth := PositionNot( elm, zero );
  
    if len < depth then
 
      # Print the zero element.
      # (Note that the unique element of a zero algebra has a name.)
      Print( "0*", names[1] );

    else

      one:= One(  elm[1] );

      if elm[ depth ] <> one then
        Print( "(", elm[ depth ], ")*" );
      fi;
      Print( names[ depth ] );

      for i in [ depth+1 .. len ] do
        if elm[i] <> zero then
          Print( "+" );
          if elm[i] <> one then
            Print( "(", elm[i], ")*" );
          fi;
          Print( names[i] );
        fi;
      od;

    fi;
    end );


#############################################################################
##
#M  One( <Fam> )
##
##  Compute the identity (if exists) from the s.c. table.
##
InstallOtherMethod( One,
    "method for family of s.c. algebra elements",
    true,
    [ IsSCAlgebraObjFamily ], 0,
    function( F )
    local one;
    one:= IdentityFromSCTable( F!.sctable );
    if one <> fail then
      one:= ObjByExtRep( F, one );
    fi;
    return one;
    end );


#############################################################################
##
#M  \=( <x>, <y> )  . . . . . . . . . .  equality of two s.c. algebra objects
#M  \<( <x>, <y> )  . . . . . . . . .  comparison of two s.c. algebra objects
#M  \+( <x>, <y> )  . . . . . . . . . . . . . sum of two s.c. algebra objects
#M  \-( <x>, <y> )  . . . . . . . . .  difference of two s.c. algebra objects
#M  \*( <x>, <y> )  . . . . . . . . . . . product of two s.c. algebra objects
#M  Zero( <x> ) . . . . . . . . . . . . . . . zero of an s.c. algebra element
#M  AdditiveInverse( <x> )  . . . additive inverse of an s.c. algebra element
#M  Inverse( <x> )  . . . . . . . . . . .  inverse of an s.c. algebra element
##
InstallMethod( \=,
    "method for s.c. algebra elements",
    IsIdentical,
    [ IsSCAlgebraObj, IsSCAlgebraObj ], 0,
    function( x, y ) return ExtRepOfObj( x ) = ExtRepOfObj( y ); end );

InstallMethod( \=,
    "method for s.c. algebra elements in dense vector rep.",
    IsIdentical,
    [ IsSCAlgebraObj and IsDenseCoeffVectorRep,
      IsSCAlgebraObj and IsDenseCoeffVectorRep ], 0,
    function( x, y ) return x![1] = y![1]; end );

InstallMethod( \<,
    "method for s.c. algebra elements",
    IsIdentical,
    [ IsSCAlgebraObj, IsSCAlgebraObj ], 0,
    function( x, y ) return ExtRepOfObj( x ) < ExtRepOfObj( y ); end );

InstallMethod( \<,
    "method for s.c. algebra elements in dense vector rep.",
    IsIdentical,
    [ IsSCAlgebraObj and IsDenseCoeffVectorRep,
      IsSCAlgebraObj and IsDenseCoeffVectorRep ], 0,
    function( x, y ) return x![1] < y![1]; end );

InstallMethod( \+,
    "method for s.c. algebra elements",
    IsIdentical,
    [ IsSCAlgebraObj, IsSCAlgebraObj ], 0,
    function( x, y )
    return ObjByExtRep( FamilyObj(x), ExtRepOfObj(x) + ExtRepOfObj(y) );
    end );

InstallMethod( \+,
    "method for s.c. algebra elements in dense vector rep.",
    IsIdentical,
    [ IsSCAlgebraObj and IsDenseCoeffVectorRep,
      IsSCAlgebraObj and IsDenseCoeffVectorRep ], 0,
    function( x, y )
    return ObjByExtRep( FamilyObj( x ), x![1] + y![1] );
    end );

InstallMethod( \-,
    "method for s.c. algebra elements",
    IsIdentical,
    [ IsSCAlgebraObj, IsSCAlgebraObj ], 0,
    function( x, y )
    return ObjByExtRep( FamilyObj(x), ExtRepOfObj(x) - ExtRepOfObj(y) );
    end );

InstallMethod( \-,
    "method for s.c. algebra elements in dense vector rep.",
    IsIdentical,
    [ IsSCAlgebraObj and IsDenseCoeffVectorRep,
      IsSCAlgebraObj and IsDenseCoeffVectorRep ], 0,
    function( x, y )
    return ObjByExtRep( FamilyObj( x ), x![1] - y![1] );
    end );

InstallMethod( \*,
    "method for s.c. algebra elements",
    IsIdentical,
    [ IsSCAlgebraObj, IsSCAlgebraObj ], 0,
    function( x, y )
    local F;
    F:= FamilyObj( x );
    return ObjByExtRep( F, SCTableProduct( F!.sctable,
                        ExtRepOfObj( x ), ExtRepOfObj( y ) ) );
    end );

InstallMethod( \*,
    "method for s.c. algebra elements in dense vector rep.",
    IsIdentical,
    [ IsSCAlgebraObj and IsDenseCoeffVectorRep,
      IsSCAlgebraObj and IsDenseCoeffVectorRep ], 0,
    function( x, y )
    local F;
    F:= FamilyObj( x );
    return ObjByExtRep( F, SCTableProduct( F!.sctable, x![1], y![1] ) );
    end );

InstallMethod( \*,
    "method for ring element and s.c. algebra element",
    IsCoeffsElms,
    [ IsRingElement, IsSCAlgebraObj ], 0,
    function( x, y )
    return ObjByExtRep( FamilyObj( y ), x * ExtRepOfObj( y ) );
    end );

InstallMethod( \*,
    "method for ring element and s.c. algebra element in dense vector rep.",
    IsCoeffsElms,
    [ IsRingElement, IsSCAlgebraObj and IsDenseCoeffVectorRep ], 0,
    function( x, y )
    return ObjByExtRep( FamilyObj( y ), x * y![1] );
    end );

InstallMethod( \*,
    "method for s.c. algebra element and ring element",
    IsElmsCoeffs,
    [ IsSCAlgebraObj, IsRingElement ], 0,
    function( x, y )
    return ObjByExtRep( FamilyObj( x ), ExtRepOfObj( x ) * y );
    end );

InstallMethod( \*,
    "method for s.c. algebra element in dense vector rep. and ring element",
    IsElmsCoeffs,
    [ IsSCAlgebraObj and IsDenseCoeffVectorRep, IsRingElement ], 0,
    function( x, y )
    return ObjByExtRep( FamilyObj( x ), x![1] * y );
    end );

InstallMethod( \*,
    "method for integer and s.c. algebra element",
    true,
    [ IsInt, IsSCAlgebraObj ], 0,
    function( x, y )
    return ObjByExtRep( FamilyObj( y ), x * ExtRepOfObj( y ) );
    end );

InstallMethod( \*,
    "method for integer and s.c. algebra element in dense vector rep.",
    true,
    [ IsInt, IsSCAlgebraObj and IsDenseCoeffVectorRep ], 0,
    function( x, y )
    return ObjByExtRep( FamilyObj( y ), x * y![1] );
    end );

InstallMethod( \*,
    "method for s.c. algebra element and integer",
    true,
    [ IsSCAlgebraObj, IsInt ], 0,
    function( x, y )
    return ObjByExtRep( FamilyObj( x ), ExtRepOfObj( x ) * y );
    end );

InstallMethod( \*,
    "method for s.c. algebra element in dense vector rep. and integer",
    true,
    [ IsSCAlgebraObj and IsDenseCoeffVectorRep, IsInt ], 0,
    function( x, y )
    return ObjByExtRep( FamilyObj( x ), x![1] * y );
    end );

InstallMethod( \/,
    "method for s.c. algebra element and scalar",
    IsElmsCoeffs,
    [ IsSCAlgebraObj, IsScalar ], 0,
    function( x, y )
    return ObjByExtRep( FamilyObj( x ), ExtRepOfObj( x ) / y );
    end );

InstallMethod( \/,
    "method for s.c. algebra element in dense vector rep. and scalar",
    IsElmsCoeffs,
    [ IsSCAlgebraObj and IsDenseCoeffVectorRep, IsScalar ], 0,
    function( x, y )
    return ObjByExtRep( FamilyObj( x ), x![1] / y );
    end );

InstallMethod( Zero,
    "method for s.c. algebra element",
    true,
    [ IsSCAlgebraObj ], 0,
    x -> ObjByExtRep( FamilyObj( x ), Zero( ExtRepOfObj( x ) ) ) );

InstallMethod( AdditiveInverse,
    "method for s.c. algebra element",
    true,
    [ IsSCAlgebraObj ], 0,
    x -> ObjByExtRep( FamilyObj( x ),
                      AdditiveInverse( ExtRepOfObj( x ) ) ) );

InstallOtherMethod( One,
    "method for s.c. algebra element",
    true,
    [ IsSCAlgebraObj ], 0,
    function( x )
    local F, one;
    F:= FamilyObj( x );
    one:= IdentityFromSCTable( F!.sctable );
    if one <> fail then
      one:= ObjByExtRep( F, one );
    fi;
    return one;
    end );

InstallOtherMethod( Inverse,
    "method for s.c. algebra element",
    true,
    [ IsSCAlgebraObj ], 0,
    function( x )
    local one, F;
    one:= One( x );
    if one <> fail then
      F:= FamilyObj( x );
      one:= QuotientFromSCTable( F!.sctable, ExtRepOfObj( one ),
                                             ExtRepOfObj( x ) );
      if one <> fail then 
        one:= ObjByExtRep( F, one );
      fi;
    fi;
    return one;
    end );


#############################################################################
##
#M  \in( <a>, <A> )
##
InstallMethod( \in,
    "method for s.c. algebra element and full s.c. algebra",
    IsElmsColls,
    [ IsSCAlgebraObj, IsFullSCAlgebra ], 0,
    function( a, A )
    A:= LeftActingDomain( A );
    return ForAll( ExtRepOfObj( a ), x -> x in A );
    end );


#############################################################################
##
#F  AlgebraByStructureConstants( <R>, <sctable> )
#F  AlgebraByStructureConstants( <R>, <sctable>, <name> )
#F  AlgebraByStructureConstants( <R>, <sctable>, <name1>, <name2>, ... )
##
##  is an algebra $A$ over the ring <R>, defined by the structure constants
##  table <sctable> of length $n$, say.
##
##  The generators of $A$ are linearly independent abstract space generators
##  $x_1, x_2, \ldots, x_n$ which are multiplied according to the formula
##  $ x_i x_j = \sum_{k=1}^n c_{ijk} x_k$
##  where '$c_{ijk}$ = <sctable>[i][j][1][i_k]'
##  and '<sctable>[i][j][2][i_k] = k'.
##
AlgebraByStructureConstantsArg := function( arglist, filter )

    local   sctable,    # structure constants table
            n,          # dimensions of structure matrices
            R,          # coefficients field
            zero,       # zero of 'R'
            names,      # names of the algebra generators
            Fam,        # the family of algebra elements
            A,          # the algebra, result
            gens;       # algebra generators of 'A'

    # Check the argument list.
    if not (     1 < Length( arglist )
             and IsRing( arglist[1] )
             and IsList( arglist[2] )
             and ( Length( arglist ) = 2 or
                   ForAll( [ 3 .. Length( arglist ) ],
                           x -> IsString( arglist[x] ) ) ) ) then
      Error( "usage: AlgebraByStructureConstantsArg([<R>,<sctable>]) or \n",
             "AlgebraByStructureConstantsArg([<R>,<sctable>,<name1>,...])" );
    fi;

    # Check the s.c. table.
#T really do this!
    sctable := Immutable( arglist[2] );
    n       := Length( sctable[1] );
    R       := arglist[1];
    zero    := Zero( R );

    # Construct names of generators (used only for printing).
    if   Length( arglist ) = 2 then
      names:= List( [ 1 .. n ],
                    x -> Concatenation( "v.", String(x) ) );
    elif Length( arglist ) = 3 and IsString( arglist[3] ) then
      names:= List( [ 1 .. n ],
                    x -> Concatenation( "v.", arglist[3] ) );
    elif Length( arglist ) = 3 and IsHomogeneousList( arglist[3] )
                           and ForAll( arglist[3], IsString ) then
      names:= arglist[3];
    elif Length( arglist ) = 2 + n then
      names:= arglist{ [ 3 .. Length( arglist ) ] };
    else
      Error( "number of names does not match" );
    fi;

    # Construct the family of elements of our algebra.
    # If the elements family of 'R' has a uniquely determined zero element,
    # then all coefficients in this family are admissible.
    # Otherwise only coefficients from 'R' itself are allowed.
    Fam:= NewFamily( "SCAlgebraObjFamily", filter );
    if Zero( ElementsFamily( FamilyObj( R ) ) ) <> fail then
      SetFilterObj( Fam, IsFamilyOverFullCoefficientsFamily );
    else
      Fam!.coefficientsDomain:= R;
    fi;

    Fam!.sctable   := sctable;
    Fam!.names     := Immutable( names );
    Fam!.zerocoeff := zero;

    # Construct the default kind of the family.
    Fam!.defaultKindDenseCoeffVectorRep :=
        NewKind( Fam, IsSCAlgebraObj and IsDenseCoeffVectorRep );

    SetCharacteristic( Fam, Characteristic( R ) );
    SetCoefficientsFamily( Fam, ElementsFamily( FamilyObj( R ) ) );
    SetZero( Fam, ObjByExtRep( Fam, List( [ 1 .. n ], x -> zero ) ) );

    # Make the generators and the algebra.
    if 0 < n then
      gens:= Immutable( List( IdentityMat( n, R ),
                              x -> ObjByExtRep( Fam, x ) ) );
      A:= FLMLORByGenerators( R, gens );
      UseBasis( A, gens );
    else
      SetName( Zero( A ), "<zero of zero s.c. algebra>" );
      gens:= [];
      A:= FLMLORByGenerators( R, gens, Zero( Fam ) );
    fi;
    Fam!.basisVectors:= gens;

    SetIsFullSCAlgebra( A, true );

    # Return the algebra.
    return A;
end;

AlgebraByStructureConstants := function( arg )
    return AlgebraByStructureConstantsArg( arg, IsSCAlgebraObj );
end;

LieAlgebraByStructureConstants := function( arg )
    local A;
    A:= AlgebraByStructureConstantsArg( arg, IsSCAlgebraObj );
    SetIsLieAlgebra( A, true );
    return A;
end;


#############################################################################
##
#F  QuaternionAlgebra( <F>, <a>, <b> )
#F  QuaternionAlgebra( <F> )
##
QuaternionAlgebra := function( arg )
    local F, a, b, e, A;

    if Length( arg ) = 1 then
      F:= arg[1];
      a:= AdditiveInverse( One( F ) );
      b:= a;
    elif Length( arg ) = 3 then
      F:= arg[1];
      a:= arg[2];
      b:= arg[3];
    else
      Error( "usage: QuaternionAlgebra( <F>[, <a>, <b>] )" );
    fi;

    # Construct the algebra.
    e:= One( F );
    A:= AlgebraByStructureConstantsArg(
            [ F,
              [ [ [[1],[e]], [[2],[ e]], [[3],[ e]], [[4],[   e]] ],
                [ [[2],[e]], [[1],[ a]], [[4],[ e]], [[3],[   a]] ],
                [ [[3],[e]], [[4],[-e]], [[1],[ b]], [[2],[  -b]] ],
                [ [[4],[e]], [[3],[-a]], [[2],[ b]], [[1],[-a*b]] ],
                0, Zero(F) ],
              "e", "i", "j", "k" ],
            IsSCAlgebraObj and IsQuaternion );

    # A quaternion algebra with parameters $-1$ over the rationals
    # is a division ring.
    if F = Rationals and a = -1 and b = -1 then
      SetFilterObj( A, IsMagmaWithInversesAndZero );
#T better: use 'DivisionRingByGenerators' !
    fi;

    # Return the quaternion algebra.
    return A;
end;


#############################################################################
##
#M  One( <quat> ) . . . . . . . . . . . . . . . . . . . . .  for a quaternion
##
InstallMethod( One,
    "method for a quaternion",
    true,
    [ IsQuaternion and IsSCAlgebraObj ], 0,
    quat -> ObjByExtRep( FamilyObj( quat ), [ 1, 0, 0, 0 ] ) );


#############################################################################
##
#M  Inverse( <quat> ) . . . . . . . . . . . . . . . . . . .  for a quaternion
##
##  The inverse of $c_1 e + c_2 i + c_3 j + c_4 k$ is
##  $c_1/z e - c_2/z i - c_3/z j - c_4/z k$
##  where $z = c_1^2 + c_2^2 + c_3^2 + c_4^2$.
##
InstallMethod( Inverse,
    "method for a quaternion",
    true,
    [ IsQuaternion and IsSCAlgebraObj ], 0,
    function( quat )
    local data, z;
    data:= ShallowCopy( ExtRepOfObj( quat ) );
    z:= data[1]^2 + data[2]^2 + data[3]^2 + data[4]^2;
    data[1]:= data[1]/z;
    data[2]:= AdditiveInverse( data[2]/z );
    data[3]:= AdditiveInverse( data[3]/z );
    data[4]:= AdditiveInverse( data[4]/z );
    return ObjByExtRep( FamilyObj( quat ), data );
    end );


#############################################################################
##
#M  ComplexConjugate( <quat> )  . . . . . . . . . . . . . .  for a quaternion
##
InstallMethod( ComplexConjugate,
    "method for a quaternion",
    true,
    [ IsQuaternion and IsSCAlgebraObj ], 0,
    function( quat )
    local data;
    data:= ShallowCopy( ExtRepOfObj( quat ) );
    data[2]:= AdditiveInverse( data[2] );
    data[3]:= AdditiveInverse( data[3] );
    data[4]:= AdditiveInverse( data[4] );
    return ObjByExtRep( FamilyObj( quat ), data );
    end );


#############################################################################
##
#F  ComplexificationQuat( <vector> )
#F  ComplexificationQuat( <matrix> )
##
ComplexificationQuat := function( matrixorvector )

    local result,
          i, e,
          M,
          m,
          n,
          j, k,
          v,
          coeff;

    result:= [];
    i:= E(4);
    e:= 1;

    if   IsQuaternionCollColl( matrixorvector ) then

      M:= matrixorvector;
      m:= Length( M );
      n:= Length( M[1] );
      for j in [ 1 .. 2*m ] do
        result[j]:= [];
      od;
      for j in [ 1 .. m ] do
        for k in [ 1 .. n ] do
          coeff:= ExtRepOfObj( M[j][k] );
          result[  j  ][  k  ]:=   e * coeff[1] + i * coeff[2];
          result[  j  ][ n+k ]:=   e * coeff[3] + i * coeff[4];
          result[ m+j ][  k  ]:= - e * coeff[3] + i * coeff[4];
          result[ m+j ][ n+k ]:=   e * coeff[1] - i * coeff[2];
        od;
      od;

    elif IsQuaternionCollection( matrixorvector ) then

      v:= matrixorvector;
      n:= Length( v );
      for j in [ 1 .. n ] do
        coeff:= ExtRepOfObj( v[j] );
        result[  j  ]:= e * coeff[1] + i * coeff[2];
        result[ n+j ]:= e * coeff[3] + i * coeff[4];
      od;

    else
      Error( "<matrixorvector> must be a vector or matrix of quaternions" );
    fi;

    return result;
end;


#############################################################################
##
#F  OctaveAlgebra( <F> )
##
OctaveAlgebra := F -> AlgebraByStructureConstants(
    F,
    [ [ [[1],[1]],[[],[]],[[3],[1]],[[],[]],[[5],[1]],[[],[]],[[],[]],
        [[8],[1]] ],
      [ [[],[]],[[2],[1]],[[],[]],[[4],[1]],[[],[]],[[6],[1]],[[7],[1]],
        [[],[]] ],
      [ [[],[]],[[3],[1]],[[],[]],[[1],[1]],[[7],[1]],[[],[]],[[],[]],
        [[6],[1]] ],
      [ [[4],[1]],[[],[]],[[2],[1]],[[],[]],[[],[]],[[8],[1]],[[5],[1]],
        [[],[]] ],
      [ [[],[]],[[5],[1]],[[7],[-1]],[[],[]],[[],[]],[[1],[1]],[[],[]],
        [[4],[-1]] ],
      [ [[6],[1]],[[],[]],[[],[]],[[8],[-1]],[[2],[1]],[[],[]],[[3],[-1]],
        [[],[]] ],
      [ [[7],[1]],[[],[]],[[],[]],[[5],[-1]],[[],[]],[[3],[1]],[[],[]],
        [[2],[-1]] ],
      [ [[],[]],[[8],[1]],[[6],[-1]],[[],[]],[[4],[1]],[[],[]],[[1],[-1]],
        [[],[]] ],
      0, Zero(F) ],
    "s1", "t1", "s2", "t2", "s3", "t3", "s4", "t4" );


#############################################################################
##
#R  IsSCAlgebraObjSpaceRep
##
##  We use that the family of an s.c. algebra knows a constitutive basis.
##  The associated row vectors can be computed for the whole family,
##  i.e., independent of the substructure (subalgebra, subspace, ideal)
##  under consideration.
##
IsSCAlgebraObjSpaceRep := NewRepresentation( "IsSCAlgebraObjSpaceRep",
    IsAttributeStoringRep and IsHandledByNiceBasis, [] );


#############################################################################
##
#M  IsSCAlgebraObjSpaceRep( <V> )
##
##  We claim that a free left module of s.c. algebra elements is in
##  'IsSCAlgebraObjSpaceRep', which means that the free module is handled by
##  a nice module.
##
##  This allows to omit special methods for 'LeftModuleByGenerators' and
##  'FLMLORByGenerators' (which would differ from the default methods only
##  by setting this flag).
##
##  (So the right way to replace the handling of the module by a better one
##  is to overlay those methods to compute bases that use the flag
##  'IsHandledByNiceBasis'.)
##
InstallTrueMethod( IsSCAlgebraObjSpaceRep,
    IsSCAlgebraObjCollection and IsFreeLeftModule );


#T #############################################################################
#T ##
#T #M  LeftModuleByGenerators( <R>, <gens> ) . . . . . for s.c. algebra elements
#T #M  LeftModuleByGenerators( <R>, <gens>, <zero> ) . for s.c. algebra elements
#T ##
#T InstallMethod( LeftModuleByGenerators, true,
#T     [ IsRing, IsSCAlgebraObjCollection ], 0,
#T     function( R, gens )
#T     local V;
#T 
#T     if HasIsDivisionRing( R ) and IsDivisionRing( R ) then
#T       V:= Objectify( NewKind( FamilyObj( R ),
#T                                   IsLeftVectorSpace
#T                               and IsSCAlgebraObjSpace
#T                               and IsAttributeStoringRep ),
#T                      rec() );
#T     else
#T       V:= Objectify( NewKind( FamilyObj( R ),
#T                                   IsFreeLeftModule
#T                               and IsSCAlgebraObjSpace
#T                               and IsAttributeStoringRep ),
#T                      rec() );
#T     fi;
#T 
#T     SetLeftActingDomain( V, R );
#T     SetGeneratorsOfLeftModule( V, AsList( gens ) );
#T 
#T     return V;
#T     end );
#T 
#T InstallOtherMethod( LeftModuleByGenerators, true,
#T     [ IsRing, IsSCAlgebraObjCollection, IsSCAlgebraObj ], 0,
#T     function( R, gens, zero )
#T     local V;
#T 
#T     if HasIsDivisionRing( R ) and IsDivisionRing( R ) then
#T       V:= Objectify( NewKind( FamilyObj( R ),
#T                                   IsLeftVectorSpace
#T                               and IsSCAlgebraObjSpace
#T                               and IsAttributeStoringRep ),
#T                      rec() );
#T     else
#T       V:= Objectify( NewKind( FamilyObj( R ),
#T                                   IsFreeLeftModule
#T                               and IsSCAlgebraObjSpace
#T                               and IsAttributeStoringRep ),
#T                      rec() );
#T     fi;
#T 
#T     SetLeftActingDomain( V, R );
#T     SetGeneratorsOfLeftModule( V, AsList( mat ) );
#T     SetZero( V, Immutable( zero ) );
#T 
#T     return V;
#T     end );
#T 
#T 
#T #############################################################################
#T ##
#T #M  FLMLORByGenerators( <R>, <gens> ) . . . . . . . for s.c. algebra elements
#T #M  FLMLORByGenerators( <R>, <gens>, <zero> ) . . . for s.c. algebra elements
#T ##
#T InstallMethod( FLMLORByGenerators, true,
#T     [ IsRing, IsSCAlgebraObjCollection ], 0,
#T     function( R, gens )
#T     local V;
#T 
#T     if HasIsDivisionRing( R ) and IsDivisionRing( R ) then
#T       V:= Objectify( NewKind( FamilyObj( R ),
#T                                   IsAlgebra
#T                               and IsSCAlgebraObjSpace
#T                               and IsAttributeStoringRep ),
#T                      rec() );
#T     else
#T       V:= Objectify( NewKind( FamilyObj( R ),
#T                                   IsFLMLOR
#T                               and IsSCAlgebraObjSpace
#T                               and IsAttributeStoringRep ),
#T                      rec() );
#T     fi;
#T 
#T     SetLeftActingDomain( V, R );
#T     SetGeneratorsOfLeftOperatorRing( V, AsList( gens ) );
#T 
#T     return V;
#T     end );
#T 
#T InstallOtherMethod( FLMLORByGenerators, true,
#T     [ IsRing, IsSCAlgebraObjCollection, IsSCAlgebraObj ], 0,
#T     function( R, gens, zero )
#T     local V;
#T 
#T     if HasIsDivisionRing( R ) and IsDivisionRing( R ) then
#T       V:= Objectify( NewKind( FamilyObj( R ),
#T                                   IsAlgebra
#T                               and IsSCAlgebraObjSpace
#T                               and IsAttributeStoringRep ),
#T                      rec() );
#T     else
#T       V:= Objectify( NewKind( FamilyObj( R ),
#T                                   IsFLMLOR
#T                               and IsSCAlgebraObjSpace
#T                               and IsAttributeStoringRep ),
#T                      rec() );
#T     fi;
#T 
#T     SetLeftActingDomain( V, R );
#T     SetGeneratorsOfLeftOperatorRing( V, AsList( gens ) );
#T     SetZero( V, Immutable( zero ) );
#T 
#T     return V;
#T     end );


#############################################################################
##
#M  PrepareNiceFreeLeftModule( <V> )
##
##  We do not need additional data to perform 'NiceVector' and 'UglyVector'.
##
InstallMethod( PrepareNiceFreeLeftModule, true,
    [ IsFreeLeftModule and IsSCAlgebraObjSpaceRep ], 0,
    Ignore );


#############################################################################
##
#M  NiceVector( <V>, <v> )
##
InstallMethod( NiceVector,
    "method for s.c. algebra elements space and s.c. algebra element",
    IsCollsElms,
    [ IsFreeLeftModule and IsSCAlgebraObjSpaceRep, IsSCAlgebraObj ], 0,
    function( V, v )
    return ExtRepOfObj( v );
    end );


#############################################################################
##
#M  UglyVector( <V>, <r> )
##
InstallMethod( UglyVector,
    "method for s.c. algebra elements space and s.c. algebra element",
    true,
    [ IsFreeLeftModule and IsSCAlgebraObjSpaceRep, IsRowVector ], 0,
    function( V, r )
    local F;
    F:= ElementsFamily( FamilyObj( V ) );
    if Length( r ) <> Length( F!.names ) then
      return fail;
    fi;
    return ObjByExtRep( F, r );
    end );


#############################################################################
##
#M  MutableBasisByGenerators( <R>, <gens> )
#M  MutableBasisByGenerators( <R>, <gens>, <zero> )
##
##  We choose a mutable basis that stores a mutable basis for a nice module.
##
InstallMethod( MutableBasisByGenerators,
    "method for ring and collection of s.c. algebra elements",
    true,
    [ IsRing, IsSCAlgebraObjCollection ], 0,
    MutableBasisViaNiceMutableBasisMethod2 );

InstallOtherMethod( MutableBasisByGenerators,
    "method for ring, (possibly empty) list, and zero element",
    true,
    [ IsRing, IsList, IsSCAlgebraObj ], 0,
    MutableBasisViaNiceMutableBasisMethod3 );


#############################################################################
##
#M  Coefficients( <B>, <v> )  . . . . . . coefficients w.r.t. canonical basis
##
InstallMethod( Coefficients,
    "method for canonical basis of full s.c. algebra",
    IsCollsElms,
    [ IsBasis and IsCanonicalBasisFullSCAlgebra, IsSCAlgebraObj ], 0,
    function( B, v )
    return ExtRepOfObj( v );
    end );


#############################################################################
##
#M  LinearCombination( <B>, <coeffs> )  . . . . . . . . . for canonical basis
##
InstallMethod( LinearCombination,
    "method for canonical basis of full s.c. algebra",
    true,
    [ IsBasis and IsCanonicalBasisFullSCAlgebra, IsRowVector ], 0,
    function( B, coeffs )
    return ObjByExtRep( ElementsFamily( FamilyObj( B ) ), coeffs );
    end );


#############################################################################
##
#M  BasisVectors( <B> ) . . . . . .  for canonical basis of full s.c. algebra
##
InstallMethod( BasisVectors,
    "method for canonical basis of full s.c. algebra",
    true,
    [ IsBasis and IsCanonicalBasisFullSCAlgebra ], 0,
    B -> ElementsFamily( FamilyObj(
             UnderlyingLeftModule( B ) ) )!.basisVectors );


#############################################################################
##
#M  BasisOfDomain( <A> )  . . . . . . . . . . .  basis of a full s.c. algebra
##
InstallMethod( BasisOfDomain,
    "method for full s.c. algebra (call 'CanonicalBasis')",
    true,
    [ IsFreeLeftModule and IsSCAlgebraObjCollection and IsFullSCAlgebra ], 0,
    CanonicalBasis );


#############################################################################
##
#M  CanonicalBasis( <A> ) . . . . . . . . . . .  basis of a full s.c. algebra
##
InstallMethod( CanonicalBasis,
    "method for full s.c. algebras",
    true,
    [ IsFreeLeftModule and IsSCAlgebraObjCollection and IsFullSCAlgebra ], 0,
    function( A )
    local B;
    B:= Objectify( NewKind( FamilyObj( A ),
                                IsCanonicalBasisFullSCAlgebra
                            and IsAttributeStoringRep
                            and IsBasis
                            and IsCanonicalBasis ),
                   rec() );
    SetUnderlyingLeftModule( B, A );
    SetStructureConstantsTable( B,
        ElementsFamily( FamilyObj( A ) )!.sctable );
    return B;
    end );

#T change implementation: bases of their own right, as for Gaussian row spaces,
#T if the algebra is Gaussian


#############################################################################
##
#E  algsc.gi  . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here



