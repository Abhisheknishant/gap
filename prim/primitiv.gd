#############################################################################
##
#W  primitiv.gd              GAP group library  Dixon,Mortimer,Short,Thei"sen
##
##
#H  @(#)$Id$
##
#H  $Log$
#H  Revision 1.1  1997/06/09 13:38:53  htheisse
#H  added the file
#H
##
Revision.primitiv_gd :=
    "@(#)$Id$";

coh := "2b defined";

PerfectResiduum := NewAttribute( "PerfectResiduum", IsGroup );
SimsNo := NewAttribute( "SimsNo", IsPermGroup );
SimsName := NewAttribute( "SimsName", IsPermGroup );
Rank := NewOperationArgs( "Rank" );
RankOp := NewOperation( "Rank", OrbitsishReq );
RankAttr := NewAttribute( "Rank", IsObject );

RepOpSuborbits := NewOperationArgs( "RepOpSuborbits" );
OnSuborbits := NewOperationArgs( "OnSuborbits" );
ConstructCohort := NewOperationArgs( "ConstructCohort" );
CohortOfGroup := NewOperationArgs( "CohortOfGroup" );
MakeCohort := NewOperationArgs( "MakeCohort" );
AlternatingCohortOnSets := NewOperationArgs( "AlternatingCohortOnSets" );
LinearCohortOnProjectivePoints := NewOperationArgs( "LinearCohortOnProjectivePoints" );
SymplecticCohortOnProjectivePoints := NewOperationArgs( "SymplecticCohortOnProjectivePoints" );
UnitaryCohortOnProjectivePoints := NewOperationArgs( "UnitaryCohortOnProjectivePoints" );
CohortProductAction := NewOperationArgs( "CohortProductAction" );
CohortPowerAlternating := NewOperationArgs( "CohortPowerAlternating" );
CohortPowerLinear := NewOperationArgs( "CohortPowerLinear" );
CohortDiagonalAction := NewOperationArgs( "CohortDiagonalAction" );
AffinePermGroupByMatrixGroup := NewOperationArgs( "AffinePermGroupByMatrixGroup" );
PrimitiveAffinePermGroupByMatrixGroup := NewOperationArgs( "PrimitiveAffinePermGroupByMatrixGroup" );
GLnbylqtolInGLnq := NewOperationArgs( "GLnbylqtolInGLnq" );
FrobInGLnq := NewOperationArgs( "FrobInGLnq" );
StabFldExt := NewOperationArgs( "StabFldExt" );

AlmostDerivedSubgroup := NewOperationArgs( "AlmostDerivedSubgroup" );
AFFINE_NON_SOLVABLE_GROUPS := NewOperationArgs( "AFFINE_NON_SOLVABLE_GROUPS" );
BOOT_AFFINE_NON_SOLVABLE_GROUPS := NewOperationArgs( "BOOT_AFFINE_NON_SOLVABLE_GROUPS" );
Cohort := NewOperationArgs( "Cohort" );
MakePrimitiveGroup := NewOperationArgs( "MakePrimitiveGroup" );
PrimitiveGroup := NewOperationArgs( "PrimitiveGroup" );
NrPrimitiveGroups := NewOperationArgs( "NrPrimitiveGroups" );
NrSolvableAffinePrimitiveGroups := NewOperationArgs( "NrSolvableAffinePrimitiveGroups" );
NrAffinePrimitiveGroups := NewOperationArgs( "NrAffinePrimitiveGroups" );

IrreducibleSolvableGroup := NewOperationArgs( "IrreducibleSolvableGroup" );

COHORTS := [  ];
COHORTS_DONE := [  ];
SIMS_NUMBERS := [  ];
SIMS_NAMES := [  ];
IrredSolGroupList := [  ];

#############################################################################
##
##  Local Variables:
##  mode:             outline-minor
##  outline-regexp:   "#[WCROAPMFVE]"
##  fill-column:      77
##  End:

#############################################################################
##
#E  primitiv.gd . . . . . . . . . . . . . . . . . . . . . . . . . . ends here

