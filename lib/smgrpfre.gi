#############################################################################
##
#W  smgrpfre.gi                 GAP library                     Thomas Breuer
#W                                                             & Frank Celler
##
#H  @(#)$Id$
##
#Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
##
##  This file contains the methods for free semigroups.
##
##  Element objects of free semigroups, free monoids and free groups are
##  associative words.
##  For the external representation see the file 'word.g'.
##
Revision.smgrpfre_gi :=
    "@(#)$Id$";


#############################################################################
##
#M  IsWholeFamily( <S> )  . . . . . . .  is a free semigroup the whole family
##
##  <S> contains the whole family of its elements if and only if all
##  magma generators of the family are among the semigroup generators of <S>.
##
InstallMethod( IsWholeFamily, true,
    [ IsSemigroup and IsAssocWordCollection ], 0,
    S -> IsSubset( GeneratorsMagmaFamily( ElementsFamily( FamilyObj( S ) ) ),
                   GeneratorsOfMagma( S ) ) );


#############################################################################
##
#M  Iterator( <S> ) . . . . . . . . . . . . . . iterator for a free semigroup
##
##  Iterator and enumerator of free semigroups are implemented as follows.
##  Words appear in increasing length in terms of the generators
##  $s_1, s_2, \ldots s_n$.
##  So first all words of length 1 are enumerated, then words of length 2,
##  and so on.
##  There are exactly $n^l$ words of length $l$.
##  They are parametrized by $l$-tuples $(c_1, c_2, \ldots, c_l)$,
##  corresponding to $s_{c_1} s_{c_2} \cdots s_{c_l}$.
##
##  So the word corresponding to the integer
##  $m = \sum_{i=1}^{l-1} n^i + m^{\prime}$,
##  with $1 \leq m^{\prime} \leq n^l$,
##  is the $m^{\prime}$-th word of length $l$.
##  Let $m^{\prime} = \sum_{i=1}^l c_i n^{i-1}$, with $1 \leq c_i \leq n$.
##  Then this word is $s_{c_1} s_{c_2} \cdots s_{c_l}$.
##
FreeSemigroup_NextWordExp := function( iter )

    local counter,
          len,
          pos,
          word,
          maxexp,
          i,
          exp;

    counter:= iter!.counter;
    len:= iter!.length;
    pos:= 1;
    while counter[ pos ] = iter!.nrgenerators do
      pos:= pos + 1;
    od;
    if pos > len then

      # All words of length at most 'len' have been used already.
      len:= len + 1;
      iter!.length:= len;
      counter:= List( [ 1 .. len ], x -> 1 );
      Add( counter, 0 );
      iter!.counter:= counter;

      # The first word of length 'len' is the power of the first generator.
      word:= [ 1, len ];
      maxexp:= len;

    else

      # Increase the counter for words of length 'iter!.length'.
      for i in [ 1 .. pos-1 ] do
        counter[i]:= 1;
      od;
      counter[ pos ]:= counter[ pos ] + 1;

      # Convert the string of generators numbers.
      word:= [];
      i:= 1;
      maxexp:= 1;
      while i <= len do
        Add( word, counter[i] );
        exp:= 1;
        while counter[i] = counter[ i+1 ] do
          exp:= exp + 1;
          i:= i+1;
        od;
        Add( word, exp );
        if maxexp < exp then
          maxexp:= exp;
        fi;
        i:= i+1;
      od;

    fi;

    iter!.word:= word;
    iter!.exp:= maxexp;
    end;

#############################################################################
##
#R  IsFreeSemigroupIterator
##
IsFreeSemigroupIterator := NewRepresentation( "IsFreeSemigroupIterator",
    IsIterator,
    [ "family", "nrgenerators", "exp", "word", "counter", "length" ] );

InstallMethod( NextIterator, true, [ IsFreeSemigroupIterator ], 0,
    function( iter )

    local word;

    word:= ObjByExtRep( iter!.family, 1, iter!.exp, iter!.word );
    FreeSemigroup_NextWordExp( iter );
    return word;
    end );

InstallMethod( IsDoneIterator, true, [ IsFreeSemigroupIterator ], 0,
               ReturnFalse );

InstallMethod( Iterator, true, [ IsSemigroup and IsAssocWordCollection ], 0,
    function( S )

    local iter;

    iter:= rec(
                family         := ElementsFamily( FamilyObj( S ) ),
                nrgenerators   := Length( GeneratorsOfMagma( S ) ),
                exp            := 1,
                word           := [ 1, 1 ],
                counter        := [ 1, 0 ],
                length         := 1
               );

    return Objectify( NewKind( IteratorsFamily, IsFreeSemigroupIterator ),
                      iter );
    end );

#############################################################################
##
#M  Enumerator( <S> ) . . . . . . . . . . . . enumerator for a free semigroup
##
FreeMonoid_ElementNumber := function( enum, nr )

    local n,
          l,
          power,
          word,
          exp,
          maxexp,
          cc,
          i,
          c;

    if nr = 1 then
      return One( enum!.family );
    fi;

    n:= enum!.nrgenerators;

    # Compute the length of the word corresponding to 'nr'.
    l:= 0;
    power:= 1;
    nr:= nr - 1;
    while 0 < nr do
      power:= power * n;
      nr:= nr - power;
      l:= l+1;
    od;
    nr:= nr + power - 1;

    # Compute the vector of the '(nr + 1)'-th element of length 'l'.
    exp:= 0;
    maxexp:= 1;
    c:= nr mod n;
    word:= [ c+1 ];
    cc:= c;
    for i in [ 1 .. l ] do
      if c = cc then
        exp:= exp + 1;
      else
        cc:= c;
        Add( word, exp );
        Add( word, c+1 );
        if maxexp < exp then
          maxexp:= exp;
        fi;
        exp:= 1;
      fi;
      nr:= ( nr - c ) / n;
      c:= nr mod n;
    od;
    Add( word, exp );

    # Return the element.
    return ObjByExtRep( enum!.family, 1, maxexp, word );
    end;

FreeMonoid_NumberElement := function( enum, elm, zero )

    local l,
          len,
          i,
          n,
          nr,
          power,
          c,
          exp;

    elm:= ExtRepOfObj( elm );
    l:= Length( elm ) / 2;

    # Calculate the length of the word.
    len:= 0;
    for i in [ 2, 4 .. 2*l ] do
      len:= len + elm[i];
    od;

    # Calculate the number of words of smaller length, plus 1.
    n:= enum!.nrgenerators;
    nr:= 1;
    power:= 1;
    for i in [ 1 .. len ] do
      nr:= nr + power;
      power:= power * n;
    od;

    # Add the position in the words of length 'len'.
    power:= 1;
    for i in [ 2, 4 .. 2*l ] do
      c:= elm[ i-1 ] - 1;
      for exp in [ 1 .. elm[i] ] do
        nr:= nr + c * power;
        power:= power * n;
      od;
    od;

    return nr;
    end;

#############################################################################
##
#R  IsFreeSemigroupEnumerator
##
IsFreeSemigroupEnumerator := NewRepresentation( "FreeSemigroupEnumerator",
    IsDomainEnumerator and IsAttributeStoringRep,
    [ "family", "nrgenerators" ] );

InstallMethod( \[\], true,
    [ IsFreeSemigroupEnumerator, IsInt and IsPosRat ], 0,
    function( enum, nr )
    return FreeMonoid_ElementNumber( enum, nr+1 );
    end );

InstallMethod( Position,
    function(F1,F2,F3) return IsCollsElms(F1,F2); end,
    [ IsFreeSemigroupEnumerator, IsAssocWord, IsZeroCyc ], 0,
    function( enum, elm, zero )
    return FreeMonoid_NumberElement( enum, elm, zero ) - 1;
    end );

InstallMethod( Enumerator, true,
    [ IsSemigroup and IsAssocWordCollection ], 0,
    function( S )
    local enum;
    enum:= Objectify( NewKind( FamilyObj( S ), IsFreeSemigroupEnumerator ),
                   rec( family       := ElementsFamily( FamilyObj( S ) ),
                        nrgenerators := Length( GeneratorsOfMagma( S ) ) ) );
    SetUnderlyingCollection( enum, S );
    return enum;
    end );


#############################################################################
##
#M  Size( <S> ) . . . . . . . . . . . . . . . . . .  size of a free semigroup
##
InstallMethod( Size, true,
    [ IsSemigroup and IsAssocWordWithOneCollection ], 0,
    function( S )
    if IsTrivial( S ) then
      return 1;
    else
      return infinity;
    fi;
    end );


#############################################################################
##
#M  Random( <S> ) . . . . . . . . . . . .  random element of a free semigroup
##
#T use better method for the whole family
##
InstallMethod( Random, true, [ IsSemigroup and IsAssocWordCollection ], 0,
    function( S )

    local len,
          result,
          gens,
          i;

    # Get a random length for the word.
    len:= Random( Integers );
    if 0 <= len then
      len:= 2 * len;
    else
      len:= -2 * len - 1;
    fi;

    # Multiply $'len' + 1$ random generators.
    gens:= GeneratorsOfMagma( S );
    result:= Random( gens );
    for i in [ 1 .. len ] do
      result:= result * Random( gens );
    od;

    # Return the result.
    return result;
    end );


#############################################################################
##
#M  GeneratorsMagmaFamily( <F> )
##
InstallMethod( GeneratorsMagmaFamily, true, [ IsAssocWordFamily ], 0,
    F -> List( [ 1 .. Length( F!.names ) ],
                 i -> ObjByExtRep( F, 1, 1, [ i, 1 ] ) ) );


#############################################################################
##
#F  FreeSemigroup( <n> )
#F  FreeSemigroup( <names> )
##
##  is a free semigroup on <n> generators, or on generators that shall be
##  printed as the strings in the list <names>.
##
FreeSemigroup := function( n )

    local   names,      # list of generators names
            F,          # family of free semigroup element objects
            S;          # free semigroup, result

    # Check the arguments.
    if IsInt( n ) and 0 <= n then
      names:= List( [ 1 .. n ], x -> Concatenation( "s.", String( x ) ) );
    elif IsList( n ) and 0 < Length( n ) and ForAll( n, IsString ) then
      names:= n;
      n:= Length( names );
    else
      Error( "<n> must be a pos. integer or a nonempty list of strings" );
    fi;

    if n = 0 then
      return FreeMonoid( 0 );
    fi;

    # Construct the family of element objects of our semigroup.
    F:= NewFamily( "FreeSemigroupElementsFamily", IsAssocWord );

    # Install the data (names, no. of bits available for exponents, kinds).
    StoreInfoFreeMagma( F, names, IsAssocWord );

    # Make the semigroup.
    S:= SemigroupByGenerators( GeneratorsMagmaFamily( F ) );

    SetIsWholeFamily( S, true );
    SetIsTrivial( S, false );
    return S;
end;


#############################################################################
##
#E  smgrpfre.gi . . . . . . . . . . . . . . . . . . . . . . . . . . ends here



