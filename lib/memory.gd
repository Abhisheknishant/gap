#############################################################################
##
##  memory.gd          recog package                      Max Neunhoeffer
##                                                            �kos Seress
##
##  Copyright 2005 Lehrstuhl D f�r Mathematik, RWTH Aachen
##
##  Group objects remembering how they were created from the generators.
##
#############################################################################

Revision.memory_gd :=
  "@(#)$Id$";

DeclareFilter("IsObjWithMemoryRankFilter",100); 

DeclareRepresentation("IsObjWithMemory", 
    IsComponentObjectRep and IsObjWithMemoryRankFilter and
    IsMultiplicativeElementWithInverse, ["slp","n","el"]);

DeclareAttribute("TypeOfObjWithMemory",IsFamily);

DeclareGlobalFunction( "GeneratorsWithMemory" );
DeclareGlobalFunction( "StripMemory" );
DeclareGlobalFunction( "StripStabChain" );
DeclareGlobalFunction( "CopyMemory" );
DeclareGlobalFunction( "GroupWithMemory" );
DeclareGlobalFunction( "SLPOfElm" );
DeclareGlobalFunction( "SLPOfElms" );

DeclareGlobalFunction( "SortFunctionWithMemory" );

