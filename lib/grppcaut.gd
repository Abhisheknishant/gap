#############################################################################
##
#W  grppcaut.gd                GAP library                      Bettina Eick
##
#Y  Copyright (C)  1997,  Lehrstuhl D für Mathematik,  RWTH Aachen, Germany
#Y  (C) 1998 School Math and Comp. Sci., University of St Andrews, Scotland
#Y  Copyright (C) 2002 The GAP Group
##
Revision.grppcaut_gd :=
    "@(#)$Id: grppcaut.gd,v 4.12 2011/06/17 21:11:55 gap Exp $";

#############################################################################
##
#P IsFrattiniFree
##
DeclareProperty( "IsFrattiniFree", IsGroup );

DeclareGlobalFunction("AutomorphismGroupNilpotentGroup");
DeclareGlobalFunction("AutomorphismGroupSolvableGroup");
DeclareGlobalFunction("AutomorphismGroupFrattFreeGroup");

#############################################################################
##
#I InfoAutGrp
##
DeclareInfoClass( "InfoAutGrp" ); 
DeclareInfoClass( "InfoMatOrb" ); 
DeclareInfoClass( "InfoOverGr" ); 

if not IsBound( CHOP ) then CHOP := false; fi;

