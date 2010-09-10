#############################################################################
##
#W  addcoset.gd                 GAP library                     Thomas Breuer
##
#H  @(#)$Id: addcoset.gd,v 4.6 2010/02/23 15:12:44 gap Exp $
##
#Y  Copyright (C)  1996,  Lehrstuhl D für Mathematik,  RWTH Aachen,  Germany
#Y  (C) 1998 School Math and Comp. Sci., University of St Andrews, Scotland
#Y  Copyright (C) 2002 The GAP Group
##
##  This file declares the operations for additive cosets.
##
Revision.addcoset_gd :=
    "@(#)$Id: addcoset.gd,v 4.6 2010/02/23 15:12:44 gap Exp $";


#############################################################################
##
#C  IsAdditiveCoset( <D> )
##
##  An additive coset is an external additive set whose additively acting
##  domain is an additive group.
##  The additive coset and its additively acting domain lie in the same
##  family.
##
##  Note that additive cosets for non-commutative addition are not supported.
##
DeclareCategory( "IsAdditiveCoset",
    IsExtASet and IsAssociativeAOpESum and IsTrivialAOpEZero );


#############################################################################
##
#O  AdditiveCoset( <A>, <a> )
##
DeclareOperation( "AdditiveCoset", [ IsAdditiveGroup, IsAdditiveElement ] );


#############################################################################
##
#E

