#############################################################################
##
#W  wordass.gd                  GAP library                     Thomas Breuer
#W                                                             & Frank Celler
##
#H  @(#)$Id$
##
#Y  Copyright 1997,    Lehrstuhl D fuer Mathematik,   RWTH Aachen,    Germany
##
##  This file declares the operations for associative words.
##
##  An *associative word* in {\GAP} is a word formed from generators under an
##  associative multiplication.
##
##  There is an external representation of the category of associative words,
##  a list of generators and exponents.
##  For example, the word $w = g_1^4 * g_2^3 * g_1$
##  has external representation '[ 1, 4, 2, 3, 1, 1 ]', where $g_i$ means
##  the $i$-th generator of the family of $w$.
##  The empty list describes the identity element (if exists) of the family.
##  Exponents may be negative if the family allows inverses.
##
Revision.wordass_gd :=
    "@(#)$Id$";


#############################################################################
##
#C  IsAssocWord( <obj> )
#C  IsAssocWordWithOne( <obj> )
#C  IsAssocWordWithInverse( <obj> )
##
IsAssocWord            := IsWord and IsAssociativeElement;
IsAssocWordWithOne     := IsAssocWord and IsWordWithOne;
IsAssocWordWithInverse := IsAssocWord and IsWordWithInverse;


#############################################################################
##
#C  IsAssocWordCollection( <obj> )
#C  IsAssocWordWithOneCollection( <obj> )
#C  IsAssocWordWithInverseCollection( <obj> )
##
IsAssocWordCollection            := CategoryCollections( IsAssocWord );

IsAssocWordWithOneCollection     := CategoryCollections(
    IsAssocWordWithOne );

IsAssocWordWithInverseCollection := CategoryCollections(
    IsAssocWordWithInverse );


IsAssocWordFamily := CategoryFamily( IsAssocWord );

IsAssocWordWithOneFamily := CategoryFamily( IsAssocWordWithOne );

IsAssocWordWithInverseFamily := CategoryFamily( IsAssocWordWithInverse );


#############################################################################
##
#C  Is8BitsFamily
#C  Is16BitsFamily
#C  Is32BitsFamily
#C  IsInfBitsFamily
##
Is8BitsFamily := NewCategory( "Is8BitsFamily", IsFamily );
Is16BitsFamily := NewCategory( "Is16BitsFamily", IsFamily );
Is32BitsFamily := NewCategory( "Is32BitsFamily", IsFamily );
IsInfBitsFamily := NewCategory( "IsInfBitsFamily", IsFamily );


#############################################################################
##
#T  IsFreeSemigroup( <obj> )
#T  IsFreeMonoid( <obj> )
#C  IsFreeGroup( <obj> )
##
#T  Note that we cannot define 'IsFreeMonoid' as
#T  'IsAssocWordWithOneCollection and IsMonoid' because then
#T  every free group would be a free monoid, which is not true!
##
IsFreeGroup := IsAssocWordWithInverseCollection and IsGroup;


#############################################################################
##
#A  NumberSyllables( <w> )
##
##  Let <w> be an associative word of the form
##  $x_1^{n_1} x_2^{n_2} \cdots x_k^{n_k}$, such that $x_i \not= x_{i+1}$
##  for $1 \leq i \leq k-1$.
##  Then 'NumberSyllables( <w> )' is $k$.
##
NumberSyllables := NewAttribute( "NumberSyllables", IsAssocWord );


#############################################################################
##
#O  ExponentSums( <w> )
#O  ExponentSums( <w>, <?>, <?> )
##
##  ???
##
ExponentSums := NewOperation( "ExponentSums", [ IsAssocWord ] );


#############################################################################
##
#O  ExponentSumWord( <w>, <gen> )
##
##  is the number of times the generator <gen> appears in the word <w>
##  minus the number of times its inverse appears in <w>.
##  If <gen> and its inverse do not occur in <w>, 0 is returned.
##  <gen> may also be the inverse of a generator.
##
ExponentSumWord := NewOperation( "ExponentSumWord",
    [ IsAssocWord, IsAssocWord ] );


#############################################################################
##
#O  Subword( <w>, <from>, <to> )
##
##  is the subword of the associative word <w> that begins at position <from>
##  and ends at position <to>.
##  <from> and <to> must be positive integers.
##  Indexing is done with origin 1.
##
Subword := NewOperation( "Subword",
    [ IsAssocWord, IsInt and IsPosRat, IsInt and IsPosRat ] );


#############################################################################
##
#O  PositionWord( <w>, <sub>, <from> )
##
##  is the position of the first occurrence of the associative word <sub>
##  in the associative word <w> starting at position <from>.
##  If there is no such occurrence, 'fail' is returned.
##  <from> must be a positive integer.
##  Indexing is done with origin 1.
##
##  In other words, 'PositionWord(<w>,<sub>,<from>)' is the smallest
##  integer <i> larger than or equal to <from> such that
##  'Subword( <w>, <i>, <i>+LengthWord(<sub>)-1 ) = <sub>' (see "Subword").
##
PositionWord := NewOperation( "PositionWord",
    [ IsAssocWord, IsAssocWord, IsInt and IsPosRat ] );


#############################################################################
##
#O  SubstitutedWord( <w>, <from>, <to>, <by> )
##
##  is a new associative word where the subword of the associative word <w>
##  that begins at position <from> and ends at position <to> is replaced by
##  the associative word <by>.
##  <from> and <to> must be positive integers.
##  Indexing is done with origin 1.
##
##  In other words 'SubstitutedWord(<w>,<from>,<to>,<by>)' is the word
##  'Subword(<w>,1,<from>-1) \*\ <by> \*\ Subword(<w>,<to>+1,LengthWord(<w>)'
##  (see "Subword").
##
SubstitutedWord := NewOperation( "SubstitutedWord",
    [ IsAssocWord, IsInt and IsPosRat, IsInt and IsPosRat, IsAssocWord ] );


#############################################################################
##
#O  EliminatedWord( <word>, <gen>, <by> )
##
##  is a new associative word where each occurrence of the generator <gen>
##  in the associative word <word> is replaced by the word <by>.
##
EliminatedWord := NewOperation( "EliminatedWord",
    [ IsAssocWord, IsAssocWord, IsAssocWord ] );


#############################################################################
##
#O  ExponentSyllable( <w>, <i> )
##
##  is the exponent of the <i>-th syllable of the associative word <w>.
##
ExponentSyllable := NewOperation( "ExponentSyllable",
    [ IsAssocWord, IsInt and IsPosRat ] );


#############################################################################
##
#O  GeneratorSyllable( <w>, <i> )
##
##  is the generator of the <i>-th syllable of the associative word <w>.
##
GeneratorSyllable := NewOperation( "GeneratorSyllable",
    [ IsAssocWord, IsInt ] );


#############################################################################
##
#O  AssocWord( <Fam>, <extrep> )  . . . .  construct word from external repr.
#O  AssocWord( <Type>, <extrep> ) . . . .  construct word from external repr.
##
AssocWord := NewOperationArgs( "AssocWord" );
#T maybe this will become a constructor of 'TypeArg1' type again


#############################################################################
##
#O  ObjByVector( <Fam>, <exponents> )
#O  ObjByVector( <Type>, <exponents> )
##
##  is the associative word in the family <Fam> that has exponents vector
##  <exponents>.
##
ObjByVector := NewOperationArgs( "ObjByVector" );
#T maybe this will become a constructor of 'TypeArg1' type again


#############################################################################
##
#O  CyclicReducedWordList( <word>, <gens> )
##
CyclicReducedWordList := NewOperation(
    "CyclicReducedWordList",
    [ IsAssocWord, IsList ] );


#############################################################################
##

#F  StoreInfoFreeMagma( <F>, <names>, <req> )
##
##  does the administrative work in the construction of free semigroups,
##  free monoids, and free groups.
##
##  <F> is the family of objects, <names> is a list of generators names,
##  and <req> is the required category for the elements, that is,
##  'IsAssocWord', 'IsAssocWordWithOne', or 'IsAssocWordWithInverse'.
##
StoreInfoFreeMagma := NewOperationArgs( "StoreInfoFreeMagma" );


#############################################################################
##
#F  InfiniteListOfNames( <string> )
##
InfiniteListOfNames := NewOperationArgs( "InfiniteListOfNames" );


#############################################################################
##
#F  InfiniteListOfGenerators( <F> )
##
InfiniteListOfGenerators := NewOperationArgs( "InfiniteListOfGenerators" );


#############################################################################
##

#E  wordass.gd  . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
##
##
