#############################################################################
##
#W  ghom.gd                     GAP library                    Heiko Thei"sen
##
#H  @(#)$Id$
##
Revision.ghom_gd :=
    "@(#)$Id$";


#############################################################################
##
#O  GroupGeneralMappingByImages( <G>, <H>, <gensG>, <gensH> )
##
GroupGeneralMappingByImages := NewOperation( "GroupGeneralMappingByImages",
    [ IsGroup, IsGroup, IsList, IsList ] );


#############################################################################
##
#O  GroupHomomorphismByImages( <G>, <H>, <gensG>, <gensH> )
##
GroupHomomorphismByImages := NewOperation( "GroupHomomorphismByImages",
    [ IsGroup, IsGroup, IsList, IsList ] );


#############################################################################
##
#O  NaturalHomomorphismByNormalSubgroup( <G>, <N> ) . . map onto factor group
##
NaturalHomomorphismByNormalSubgroup := NewOperation(
    "NaturalHomomorphismByNormalSubgroup", [ IsGroup, IsGroup ] );


#############################################################################
##
#A  NaturalHomomorphismByNormalSubgroupInParent( <N> )  .  if G is the parent
##
NaturalHomomorphismByNormalSubgroupInParent := NewAttribute(
    "NaturalHomomorphismByNormalSubgroupInParent", IsGroup );


IsGroupGeneralMappingByImages := NewRepresentation
    ( "IsGroupGeneralMappingByImages",
      IsGroupGeneralMapping and IsAttributeStoringRep,
      [ "generators", "genimages", "elements", "images" ] );

IsGroupGeneralMappingByPcgs := NewRepresentation
    ( "IsGroupGeneralMappingByPcgs",
      IsGroupGeneralMappingByImages, [ "pcgs", "generators", "genimages" ] );

IsGroupGeneralMappingByAsGroupGeneralMappingByImages := NewRepresentation
    ( "IsGroupGeneralMappingByAsGroupGeneralMappingByImages",
      IsGroupGeneralMapping and IsAttributeStoringRep, [  ] );

AsGroupGeneralMappingByImages := NewAttribute( "AsGroupGeneralMappingByImages",
    IsGroupGeneralMapping );
SetAsGroupGeneralMappingByImages := Setter( AsGroupGeneralMappingByImages );
HasAsGroupGeneralMappingByImages := Tester( AsGroupGeneralMappingByImages );

InnerAutomorphism := NewOperation( "InnerAutomorphism",
    [ IsGroup, IsMultiplicativeElementWithInverse ] );

IsInnerAutomorphismRep := NewRepresentation( "IsInnerAutomorphismRep",
    IsGroupHomomorphism and IsBijective and IsAttributeStoringRep
    and IsMultiplicativeElementWithInverse, [ "conjugator" ] );

#############################################################################
##
#R  IsNaturalHomomorphismPcGroupRep . . . . . . . . natural hom in a pc group
##
##  In this representation, the range is always a pc group. This fact is used
##  by the methods for `IsLeftQuotientNaturalHomomorphismsPcGroup'.
##
IsNaturalHomomorphismPcGroupRep := NewRepresentation
    ( "IsNaturalHomomorphismPcGroupRep",
      IsGroupHomomorphism and IsSurjective and
      IsComponentObjectRep and IsAttributeStoringRep,
      [ "pcgsSource", "pcgsRange" ] );

#############################################################################
##
#R  IsLeftQuotientNaturalHomomorphisms  . . . natural homomorphism G/N -> G/M
##
IsLeftQuotientNaturalHomomorphisms := NewRepresentation
    ( "IsLeftQuotientNaturalHomomorphisms",
      IsGroupHomomorphism and IsSurjective and
      IsComponentObjectRep and IsAttributeStoringRep,
      [ "modM", "modN" ] );

#############################################################################
##
#R  IsLeftQuotientNaturalHomomorphismsPcGroup .  nat. homomorphism G/N -> G/M
##
##  Because   of     the  remark   after   `IsNaturalHomomorphismPcGroupRep',
##  homomorphisms in this representation  always go from a  pc group to  a pc
##  group.
##
IsLeftQuotientNaturalHomomorphismsPcGroup := NewRepresentation
    ( "IsLeftQuotientNaturalHomomorphismsPcGroup",
      IsLeftQuotientNaturalHomomorphisms,
      [ "modM", "modN" ] );

FilterGroupGeneralMappingByImages := NewOperationArgs( "FilterGroupGeneralMappingByImages" );
MakeMapping := NewOperationArgs( "MakeMapping" );
GroupIsomorphismByFunctions := NewOperationArgs( "GroupIsomorphismByFunctions" );

#############################################################################
##
##  Local Variables:
##  mode:             outline-minor
##  outline-regexp:   "#[WCROAPMFVE]"
##  fill-column:      77
##  End:

#############################################################################
##
#E  ghom.gd . . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here

