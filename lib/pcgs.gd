#############################################################################
##
#W  pcgs.gd                     GAP Library                      Frank Celler
##
#H  @(#)$Id$
##
#Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
##
##  This file contains the operations for polycylic generating systems.
##
Revision.pcgs_gd :=
    "@(#)$Id$";


#############################################################################
##

#C  IsPcgs
##
IsPcgs := NewCategory(
    "IsPcgs",
    IsHomogeneousList and IsDuplicateFreeList 
    and IsMultiplicativeElementWithInverseCollection );


#############################################################################
##
#C  IsPcgsFamily
##
IsPcgsFamily := NewCategory(
    "IsPcgsFamily",
    IsFamily );


#############################################################################
##
#R  IsPcgsDefaultRep
##
IsPcgsDefaultRep := NewRepresentation(
    "IsPcgsDefaultRep",
    IsComponentObjectRep and IsAttributeStoringRep, [] );


#############################################################################
##

#O  PcgsByPcSequence( <fam>, <pcs> )
##
PcgsByPcSequence := NewConstructor(
    "PcgsByPcSequence",
    [ IsFamily, IsList ] );


#############################################################################
##
#O  PcgsByPcSequenceNC( <fam>, <pcs> )
##
PcgsByPcSequenceNC := NewConstructor(
    "PcgsByPcSequenceNC",
    [ IsFamily, IsList ] );


#############################################################################
##

#A  GroupByPcgs( <pcgs> )
##
GroupByPcgs := NewAttribute(
    "GroupByPcgs",
    IsPcgs );

SetGroupByPcgs := Setter(GroupByPcgs);
HasGroupByPcgs := Tester(GroupByPcgs);


#############################################################################
##
#A  GroupOfPcgs( <pcgs> )
##
GroupOfPcgs := NewAttribute(
    "GroupOfPcgs",
    IsPcgs );

SetGroupOfPcgs := Setter(GroupOfPcgs);
HasGroupOfPcgs := Tester(GroupOfPcgs);


#############################################################################
##
#A  OneOfPcgs( <pcgs> )
##
OneOfPcgs := NewAttribute(
    "OneOfPcgs",
    IsPcgs );

SetOneOfPcgs := Setter(OneOfPcgs);
HasOneOfPcgs := Tester(OneOfPcgs);


#############################################################################
##
#A  PcSeries( <pcgs> )
##
PcSeries := NewAttribute(
    "PcSeries",
    IsPcgs );

SetPcSeries := Setter(PcSeries);
HasPcSeries := Tester(PcSeries);


#############################################################################
##

#P  IsPrimeOrdersPcgs( <pcgs> )
##
IsPrimeOrdersPcgs := NewProperty(
    "IsPrimeOrdersPcgs",
    IsPcgs );

SetIsPrimeOrdersPcgs := Setter(IsPrimeOrdersPcgs);
HasIsPrimeOrdersPcgs := Tester(IsPrimeOrdersPcgs);


#############################################################################
##
#P  IsFiniteOrdersPcgs( <pcgs> )
##
IsFiniteOrdersPcgs := NewProperty(
    "IsFiniteOrdersPcgs",
    IsPcgs );

SetIsFiniteOrdersPcgs := Setter(IsFiniteOrdersPcgs);
HasIsFiniteOrdersPcgs := Tester(IsFiniteOrdersPcgs);


#############################################################################
##

#O  DepthOfPcElement( <pcgs>, <elm> )
##
DepthOfPcElement := NewOperation(
    "DepthOfPcElement",
    [ IsPcgs, IsObject ] );


#############################################################################
##
#O  DifferenceOfPcElement( <pcgs>, <left>, <right> )
##
DifferenceOfPcElement := NewOperation(
    "DifferenceOfPcElement",
    [ IsPcgs, IsObject, IsObject ] );


#############################################################################
##
#O  ExponentOfPcElement( <pcgs>, <elm>, <pos> )
##
ExponentOfPcElement := NewOperation(
    "ExponentOfPcElement",
    [ IsPcgs, IsObject, IsInt and IsPosRat ] );


#############################################################################
##
#O  ExponentsOfPcElement( <pcgs>, <elm> )
##
ExponentsOfPcElement := NewOperation(
    "ExponentsOfPcElement",
    [ IsPcgs, IsObject ] );


#############################################################################
##
#O  LeadingExponentOfPcElement( <pcgs>, <elm> )
##
LeadingExponentOfPcElement := NewOperation(
    "LeadingExponentOfPcElement",
    [ IsPcgs, IsObject ] );


#############################################################################
##
#O  PcElementByExponents( <pcgs>, <list> )
##
PcElementByExponents := NewOperation(
    "PcElementByExponents",
    [ IsPcgs, IsList ] );


#############################################################################
##
#O  ReducedPcElement( <pcgs>, <left>, <right> )
##
ReducedPcElement := NewOperation(
    "ReducedPcElement",
    [ IsPcgs, IsObject, IsObject ] );


#############################################################################
##
#O  RelativeOrderOfPcElement( <pcgs>, <elm> )
##
RelativeOrderOfPcElement := NewOperation(
    "RelativeOrderOfPcElement",
    [ IsPcgs, IsObject ] );


#############################################################################
##
#O  SumOfPcElement( <pcgs>, <left>, <right> )
##
SumOfPcElement := NewOperation(
    "SumOfPcElement",
    [ IsPcgs, IsObject, IsObject ] );


#############################################################################
##

#E  pcgs.gd . . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
##
