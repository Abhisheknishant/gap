#############################################################################
##
#W  oprt.gd                     GAP library                    Heiko Thei"sen
##
#H  @(#)$Id$
##
#H  $Log$
#H  Revision 4.21  1997/04/09 09:17:08  htheisse
#H  gave sparse homomorphisms an external set
#H  allowed \`Permutation' to return fail instead of error
#H  added new methods for matrix groups nice monomorphisms
#H
#H  Revision 4.20  1997/03/17 14:20:42  htheisse
#H  added generic method for `OrbitStabilizer'
#H
#H  Revision 4.19  1997/03/12 13:17:51  htheisse
#H  added method for preimages under GL -> perm op homom
#H
#H  Revision 4.18  1997/03/04 16:04:43  htheisse
#H  checked the `oprt*' functions against the descriptions of the 3.4 manual
#H
#H  Revision 4.17  1997/02/12 16:30:18  htheisse
#H  corrected enumerators for external subsets; cleaned up the code
#H
#H  Revision 4.16  1997/02/07 13:55:44  ahulpke
#H  Added 'CanonicalRepresentativeDeterminatorOfExternalSet' and utilized it for
#H  right cosets.
#H
#H  Revision 4.15  1997/02/06 09:51:24  htheisse
#H  introduced `IsPrimitiveAffine'
#H
#H  Revision 4.14  1997/01/30 13:26:18  htheisse
#H  reorganised the representation of operation homomorphisms to fix a bug
#H
#H  Revision 4.13  1997/01/09 18:03:00  htheisse
#H  added `SparseOperationHomomorphism'
#H
#H  Revision 4.12  1997/01/09 16:31:37  ahulpke
#H  Added StabilizerOfBlockNC
#H
#H  Revision 4.11  1996/12/19 09:40:51  htheisse
#H  reduced number of calls of `NewKind'
#H
##
Revision.oprt_gd :=
    "@(#)$Id$";

InfoOperation := NewInfoClass( "InfoOperation" );

IsExternalSet := NewCategory( "IsExternalSet", IsDomain );
IsExternalSubset := NewRepresentation( "IsExternalSubset",
    IsComponentObjectRep and IsAttributeStoringRep and IsExternalSet,
    [ "start" ] );                            
IsExternalOrbit := NewRepresentation( "IsExternalOrbit",
    IsExternalSubset, [ "start" ] );
IsExternalSetByPcgs := NewCategory( "IsExternalSetByPcgs", IsExternalSet );

XSET_XSSETKIND := 4;
XSET_XORBKIND  := 5;

IsExternalSetDefaultRep := NewRepresentation( "IsExternalSetDefaultRep",
    IsComponentObjectRep and IsAttributeStoringRep and IsExternalSet,
    [  ] );
IsExternalSetByOperatorsRep := NewRepresentation
  ( "IsExternalSetByOperatorsRep",
    IsComponentObjectRep and IsAttributeStoringRep and IsExternalSet,
    [ "generators", "operators", "funcOperation" ] );

IsOperationHomomorphism := NewRepresentation( "IsOperationHomomorphism",
    IsGroupHomomorphism and
    IsGroupGeneralMappingByAsGroupGeneralMappingByImages and
    IsAttributeStoringRep, [ "externalSet" ] );

IsOperationHomomorphismDirectly := NewRepresentation
    ( "IsOperationHomomorphismDirectly",
      IsOperationHomomorphism,
      [ "externalSet" ] );
IsOperationHomomorphismByOperators := NewRepresentation
    ( "IsOperationHomomorphismByOperators",
      IsOperationHomomorphism,
      [ "externalSet" ] );
IsOperationHomomorphismSubset := NewRepresentation
    ( "IsOperationHomomorphismSubset",
      IsOperationHomomorphism,
      [ "externalSet" ] );
IsOperationHomomorphismByBase := NewRepresentation
    ( "IsOperationHomomorphismByBase",
      IsOperationHomomorphism,
      [ "externalSet" ] );

IsConstituentHomomorphism := NewRepresentation( "IsConstituentHomomorphism",
    IsOperationHomomorphismDirectly, [ "externalSet", "conperm" ] );

IsBlocksHomomorphism := NewRepresentation( "IsBlocksHomomorphism",
    IsOperationHomomorphismDirectly, [ "externalSet", "reps" ] );

IsGeneralLinearOperationHomomorphism := NewRepresentation
    ( "IsGeneralLinearOperationHomomorphism",
      IsOperationHomomorphismDirectly,
      [ "externalSet" ] );

IsGeneralLinearOperationHomomorphismWithBase := NewRepresentation
    ( "IsGeneralLinearOperationHomomorphismWithBase",
      IsGeneralLinearOperationHomomorphism,
      [ "externalSet" ] );

ActingDomain := NewAttribute( "ActingDomain", IsExternalSet );
SetActingDomain := Setter( ActingDomain );
HasActingDomain := Tester( ActingDomain );

FunctionOperation := NewAttribute( "FunctionOperation", IsExternalSet );
SetFunctionOperation := Setter( FunctionOperation );
HasFunctionOperation := Tester( FunctionOperation );

HomeEnumerator := NewAttribute( "HomeEnumerator", IsExternalSet );
SetHomeEnumerator := Setter( HomeEnumerator );
HasHomeEnumerator := Tester( HomeEnumerator );

CanonicalRepresentativeOfExternalSet := NewAttribute
    ( "CanonicalRepresentativeOfExternalSet", IsExternalSet );
SetCanonicalRepresentativeOfExternalSet :=
  Setter( CanonicalRepresentativeOfExternalSet );
HasCanonicalRepresentativeOfExternalSet :=
  Tester( CanonicalRepresentativeOfExternalSet );

# a CanonicalRepresentativeDeterminatorOfExternalSet is a function that
# takes as arguments the acting group and the point. It returns a list
# of length 3: [CanonRep, NormalizerCanonRep, ConjugatingElm]. 
# list components 2 and 3 do not need to be bound.

CanonicalRepresentativeDeterminatorOfExternalSet := NewAttribute
    ( "CanonicalRepresentativeDeterminatorOfExternalSet", IsExternalSet );
SetCanonicalRepresentativeDeterminatorOfExternalSet :=
  Setter( CanonicalRepresentativeDeterminatorOfExternalSet );
HasCanonicalRepresentativeDeterminatorOfExternalSet :=
  Tester( CanonicalRepresentativeDeterminatorOfExternalSet );

# Xsets that know how to get a canonical representative should claim they
# have one for purposes of method selection
InstallTrueMethod(HasCanonicalRepresentativeOfExternalSet,
  HasCanonicalRepresentativeDeterminatorOfExternalSet);

OperatorOfExternalSet := NewAttribute( "OperatorOfExternalSet",
                                 IsExternalSet );
SetOperatorOfExternalSet := Setter( OperatorOfExternalSet );
HasOperatorOfExternalSet := Tester( OperatorOfExternalSet );

OperationHomomorphismAttr := NewAttribute( "OperationHomomorphism",
                                 IsExternalSet );

OrbitishReq  := [ IsGroup, IsList, IsObject,
                  IsList,
                  IsList,
                  IsFunction ];
OrbitsishReq := [ IsGroup, IsList,
                  IsList,
                  IsList,
                  IsFunction ];

ExternalSet := NewOperationArgs( "ExternalSet" );
ExternalSetOp := NewOperation( "ExternalSet", OrbitsishReq );
ExternalSetAttr := NewAttribute( "ExternalSet", IsGroup );
                                    # properly: ^IsExternalSet
ExternalSetByFilterConstructor := NewOperationArgs
                                  ( "ExternalSetByFilterConstructor" );
ExternalSetByKindConstructor := NewOperationArgs
                                ( "ExternalSetByKindConstructor" );

ExternalSubset := NewOperationArgs( "ExternalSubset" );
ExternalSubsetOp := NewOperation( "ExternalSubset",
    [ IsGroup, IsList, IsList,
      IsList,
      IsList,
      IsFunction ] );

ExternalOrbit := NewOperationArgs( "ExternalOrbit" );
ExternalOrbitOp := NewOperation( "ExternalOrbit", OrbitishReq );

Orbit := NewOperationArgs( "Orbit" );
OrbitOp := NewOperation( "Orbit", OrbitishReq );

OrbitStabilizer := NewOperationArgs( "OrbitStabilizer" );
OrbitStabilizerOp := NewOperation( "OrbitStabilizer", OrbitishReq );

Orbits := NewOperationArgs( "Orbits" );
OrbitsOp := NewOperation( "Orbits", OrbitsishReq );
OrbitsAttr := NewAttribute( "Orbits", IsExternalSet );

SparseOperationHomomorphism := NewOperationArgs
                               ( "SparseOperationHomomorphism" );
SparseOperationHomomorphismOp := NewOperation( "SparseOperationHomomorphismOp",
    OrbitishReq );

ExternalOrbits := NewOperationArgs( "ExternalOrbits" );
ExternalOrbitsOp := NewOperation( "ExternalOrbits", OrbitsishReq );
ExternalOrbitsAttr := NewAttribute( "ExternalOrbits", IsExternalSet );

Permutation := NewOperationArgs( "Permutation" );
PermutationOp := NewOperation( "Permutation",
    [ IsObject, IsList, IsFunction ] );

PermutationCycle := NewOperationArgs( "PermutationCycle" );
PermutationCycleOp := NewOperation( "PermutationCycle",
    [ IsObject, IsList, IsObject, IsFunction ] );

Cycle := NewOperationArgs( "Cycle" );
CycleOp := NewOperation( "Cycle",
    [ IsObject, IsList, IsObject, IsFunction ] );

Cycles := NewOperationArgs( "Cycles" );
CyclesOp := NewOperation( "Cycles",
    [ IsObject, IsList, IsFunction ] );

Blocks := NewOperationArgs( "Blocks" );
BlocksOp := NewOperation( "Blocks",
    [ IsGroup, IsList, IsList,
      IsList,
      IsList,
      IsFunction ] );

MaximalBlocks := NewOperationArgs( "MaximalBlocks" );
MaximalBlocksOp := NewOperation( "MaximalBlocks",
    [ IsGroup, IsList, IsList,
      IsList,
      IsList,
      IsFunction ] );

OrbitLength := NewOperationArgs( "OrbitLength" );
OrbitLengthOp := NewOperation( "OrbitLength", OrbitishReq );

OrbitLengths := NewOperationArgs( "OrbitLengths" );
OrbitLengthsOp := NewOperation( "OrbitLengths", OrbitsishReq );
OrbitLengthsAttr := NewAttribute( "OrbitLengths", IsExternalSet );

CycleLength := NewOperationArgs( "CycleLength" );
CycleLengthOp := NewOperation( "CycleLength",
    [ IsObject, IsList, IsObject, IsFunction ] );

CycleLengths := NewOperationArgs( "CycleLengths" );
CycleLengthsOp := NewOperation( "CycleLengths",
    [ IsObject, IsList, IsFunction ] );

IsTransitive := NewOperationArgs( "IsTransitive" );
IsTransitiveOp := NewOperation( "IsTransitive", OrbitsishReq );
IsTransitiveProp := NewProperty( "IsTransitive", IsObject );

Transitivity := NewOperationArgs( "Transitivity" );
TransitivityOp := NewOperation( "Transitivity", OrbitsishReq );
TransitivityAttr := NewAttribute( "Transitivity", IsObject );

IsPrimitive := NewOperationArgs( "IsPrimitive" );
IsPrimitiveOp := NewOperation( "IsPrimitive", OrbitsishReq );
IsPrimitiveProp := NewProperty( "IsPrimitive", IsObject );

Earns := NewOperationArgs( "Earns" );
EarnsOp := NewOperation( "Earns", OrbitsishReq );
EarnsAttr := NewAttribute( "Earns", IsObject );

IsPrimitiveAffine := NewOperationArgs( "IsPrimitiveAffine" );
IsPrimitiveAffineOp := NewOperation( "IsPrimitiveAffine", OrbitsishReq );
IsPrimitiveAffineProp := NewProperty( "IsPrimitiveAffine", IsObject );

IsSemiRegular := NewOperationArgs( "IsSemiRegular" );
IsSemiRegularOp := NewOperation( "IsSemiRegular", OrbitsishReq );
IsSemiRegularProp := NewProperty( "IsSemiRegular", IsObject );

IsRegular := NewOperationArgs( "IsRegular" );
IsRegularOp := NewOperation( "IsRegular", OrbitsishReq );
IsRegularProp := NewProperty( "IsRegular", IsObject );

RepresentativeOperation := NewOperationArgs( "RepresentativeOperation" );
RepresentativeOperationOp := NewOperation( "RepresentativeOperation",
    [ IsGroup, IsList, IsObject, IsObject, IsFunction ] );

Stabilizer := NewOperationArgs( "Stabilizer" );
StabilizerOp := NewOperation( "Stabilizer", OrbitishReq );

StabilizerOfExternalSet := NewAttribute( "StabilizerOfExternalSet",
                                   IsExternalSet );
SetStabilizerOfExternalSet := Setter( StabilizerOfExternalSet );
HasStabilizerOfExternalSet := Tester( StabilizerOfExternalSet );

AttributeOperation := NewOperationArgs( "AttributeOperation" );
OrbitishOperation := NewOperationArgs( "OrbitishOperation" );
OperationHomomorphism := NewOperationArgs( "OperationHomomorphism" );
OperationHomomorphismSubsetAsGroupGeneralMappingByImages := NewOperationArgs
    ( "OperationHomomorphismSubsetAsGroupGeneralMappingByImages" );
Operation := NewOperationArgs( "Operation" );
OperationOrbit := NewOperationArgs( "OperationOrbit" );
OrbitByPosOp := NewOperationArgs( "OrbitByPosOp" );
OrbitStabilizerByGenerators := NewOperationArgs
                               ( "OrbitStabilizerByGenerators" );
SetCanonicalRepresentativeOfExternalOrbitByPcgs :=
  NewOperationArgs( "SetCanonicalRepresentativeOfExternalOrbitByPcgs" );

#############################################################################
##
#F  StabilizerOfBlockNC( <G>, <B> )  . . . . block stabilizer for perm groups
##
StabilizerOfBlockNC := NewOperationArgs( "StabilizerOfBlockNC" );

#############################################################################
##
##  Local Variables:
##  mode:             outline-minor
##  outline-regexp:   "#[WCROAPMFVE]"
##  fill-column:      77
##  End:

#############################################################################
##
#E  12345678.g  . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
