#############################################################################
##
#W  rws.gi                      GAP Library                      Frank Celler
##
#H  @(#)$Id$
##
#Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
##
##  This file contains generic methods for rewriting systems.
##
Revision.rws_gi :=
    "@(#)$Id$";


#############################################################################
##

#M  AddGenerators( <rws>, <gens> )
##
InstallMethod( AddGenerators,
    true, 
    [ IsRewritingSystem and IsMutable,
      IsHomogeneousList ],
    0,

function( rws, gens )
    local   lst;

    lst := GeneratorsOfRws(rws);
    if not IsEmpty( Intersection( rws, gens ) )  then
        Error( "new generators contain old ones" );
    fi;
    lst := Concatenation( lst, gens );
    SetGeneratorsOfRws( rws, lst );
    SetNumberGeneratorsOfRws( rws, Length(lst) );

end );


#############################################################################
##
#M  NumberGeneratorsOfRws( <rws> )
##
InstallMethod( NumberGeneratorsOfRws,
    true,
    [ IsRewritingSystem ],
    0,

function( rws )
    return Length( GeneratorsOfRws(rws) );
end );


#############################################################################
##
#M  UnderlyingFamily( <rws> )
##
InstallMethod( UnderlyingFamily,
    true,
    [ IsRewritingSystem ],
    0,

function( rws )
    return FamilyObj(rws)!.underlyingFamily;
end );


#############################################################################
##
#M  PrintObj( <rws> )
##


#############################################################################
InstallMethod( PrintObj,
    true,
    [ IsRewritingSystem ],
    0,

function( rws )
    Print( "<<rewriting system>>" );
end );


#############################################################################
InstallMethod( PrintObj, true,
    [ IsRewritingSystem and IsBuiltFromGroup ],
    0,

function( rws )
    Print( "<<group rewriting system>>" );
end );


#############################################################################
##
#M  ReduceRules( <rws> )
##
InstallMethod( ReduceRules,
    true,
    [ IsRewritingSystem and IsMutable ],
    0,
    function( rws ) return; end );


#############################################################################
##

#F  IsIdenticalFamiliesRwsObj( <rws>, <obj> )
##
IsIdenticalFamiliesRwsObj := function( a, b )
    return IsIdentical( a!.underlyingFamily, b );
end;


#############################################################################
##
#F  IsIdenticalFamiliesRwsObjObj( <rws>, <obj>, <obj> )
##
IsIdenticalFamiliesRwsObjObj := function( a, b, c )
    return IsIdentical( a!.underlyingFamily, b )
       and IsIdentical( b, c );
end;


#############################################################################
##
#F  IsIdenticalFamiliesRwsObjXXX( <rws>, <obj>, <obj> )
##
IsIdenticalFamiliesRwsObjXXX := function( a, b, c )
    return IsIdentical( a!.underlyingFamily, b );
end;


#############################################################################
##

#M  ReducedAdditiveInverse( <rws>, <obj> )
##
InstallMethod( ReducedAdditiveInverse,
    "ReducedForm",
    IsIdenticalFamiliesRwsObj,
    [ IsRewritingSystem and IsBuiltFromAdditiveMagmaWithInverses,
      IsAdditiveElementWithInverse ],
    0,

function( rws, obj )
    return ReducedForm( rws, -obj );
end );


#############################################################################
##
#M  ReducedComm( <rws>, <left>, <right> )
##
InstallMethod( ReducedComm, 
    "ReducedLeftQuotient/ReducedProduct",
    IsIdenticalFamiliesRwsObjObj,
    [ IsRewritingSystem and IsBuiltFromGroup, 
      IsMultiplicativeElementWithInverse,
      IsMultiplicativeElementWithInverse ],
    0,

function( rws, left, right )
    return ReducedLeftQuotient( rws, 
        ReducedProduct( rws, right, left ), 
        ReducedProduct( rws, left, right ) );
end );


#############################################################################
##
#M  ReducedConjugate( <rws>, <left>, <right> )
##
InstallMethod( ReducedConjugate,
    "ReducedLeftQuotient/ReducedProduct",
    IsIdenticalFamiliesRwsObjObj,
    [ IsRewritingSystem and IsBuiltFromGroup, 
      IsMultiplicativeElementWithInverse,
      IsMultiplicativeElementWithInverse ], 0,

function( rws, left, right )
    return ReducedLeftQuotient( rws, right,
               ReducedProduct( rws, left, right ) );
end );


#############################################################################
##
#M  ReducedDifference( <rws>, <left>, <right> )
##
InstallMethod( ReducedDifference,
    "ReducedSum/ReducedAdditiveInverse",
    IsIdenticalFamiliesRwsObjObj,
    [ IsRewritingSystem and IsBuiltFromAdditiveMagmaWithInverses,
      IsAdditiveElementWithInverse,
      IsAdditiveElementWithInverse ],
    0,

function( rws, left, right )
    return ReducedSum( rws, left, ReducedAdditiveInverse( rws, right ) );
end );


#############################################################################
##
#M  ReducedInverse( <rws>, <obj> )
##
InstallMethod( ReducedInverse, 
    "ReducedForm",
    IsIdenticalFamiliesRwsObj,
    [ IsRewritingSystem and IsBuiltFromMagmaWithInverses,
      IsMultiplicativeElementWithInverse ],
    0,

function( rws, obj )
    return ReducedForm( rws, obj^-1 );
end );


#############################################################################
##
#M  ReducedLeftQuotient( <rws>, <left>, <right> )
##
InstallMethod( ReducedLeftQuotient,
    "ReducedProduct/ReducedInverse",
    IsIdenticalFamiliesRwsObjObj,
    [ IsRewritingSystem and IsBuiltFromMagmaWithInverses,
      IsMultiplicativeElementWithInverse,
      IsMultiplicativeElementWithInverse ],
    0,

function( rws, left, right )
    return ReducedProduct( rws, ReducedInverse( rws, left ), right );
end );


#############################################################################
##
#M  ReducedOne( <rws> )
##
InstallMethod( ReducedOne, 
    "ReducedForm",
    true,
    [ IsRewritingSystem and IsBuiltFromMagmaWithOne ],
    0,

function( rws )
    return ReducedForm( rws, One(UnderlyingFamily(rws)) );
end );


#############################################################################
##
#M  ReducedPower( <rws>, <obj>, <pow> )
##
InstallMethod( ReducedPower, 
    "ReducedProduct/ReducedInverse",
    IsIdenticalFamiliesRwsObjXXX,
    [ IsRewritingSystem and IsBuiltFromGroup,
      IsMultiplicativeElement,
      IsInt ],
    0,

function( rws, obj, pow )
    local   res,  i;

    # if <pow> is negative invert <obj> first
    if pow < 0  then
        obj := ReducedInverse( rws, obj );
        pow := -pow;
    fi;

    # if <pow> is zero, reduce the identity
    if pow = 0  then
        return ReducedOne(rws);

    # catch some trivial cases
    elif pow <= 5  then
        if pow = 1  then
            return obj;
        elif pow = 2  then
            return ReducedProduct( rws, obj, obj );
        elif pow = 3  then
            res := ReducedProduct( rws, obj, obj );
            return ReducedProduct( rws, res, obj );
        elif pow = 4  then
            res := ReducedProduct( rws, obj, obj );
            return ReducedProduct( rws, res, res );
        elif pow = 5  then
            res := ReducedProduct( rws, obj, obj );
            res := ReducedProduct( rws, res, res );
            return ReducedProduct( rws, res, obj );
        fi;
    fi;

    # use repeated squaring (right to left)
    res := ReducedOne(rws);
    i   := 1;
    while i <= pow  do i := i * 2; od;
    while 1 < i  do
        res := ReducedProduct( rws, res, res );
        i   := QuoInt( i, 2 );
        if i <= pow  then
            res := ReducedProduct( rws, res, obj );
            pow := pow - i;
        fi;
    od;
    return res;

end );


#############################################################################
##
#M  ReducedProduct( <rws>, <left>, <right> )
##
InstallMethod( ReducedProduct,
    "ReducedForm",
    IsIdenticalFamiliesRwsObjObj,
    [ IsRewritingSystem and IsBuiltFromMagma,
      IsMultiplicativeElement,
      IsMultiplicativeElement ],
    0,

function( rws, left, right )
    return ReducedForm( rws, left * right );
end );


#############################################################################
##
#M  ReducedQuotient( <rws>, <left>, <right> )
##
InstallMethod( ReducedQuotient,
    "ReducedProduct/ReducedInverse",
    IsIdenticalFamiliesRwsObjObj,
    [ IsRewritingSystem and IsBuiltFromMagmaWithInverses,
      IsMultiplicativeElementWithInverse,
      IsMultiplicativeElementWithInverse ],
    0,

function( rws, left, right )
    return ReducedProduct( rws, left, ReducedInverse( rws, right ) );
end );


#############################################################################
##
#M  ReducedSum( <rws>, <left>, <right> )
##
InstallMethod( ReducedSum,
    "ReducedForm",
    IsIdenticalFamiliesRwsObjObj,
    [ IsRewritingSystem and IsBuiltFromAdditiveMagmaWithInverses,
      IsAdditiveElement,
      IsAdditiveElement ],
    0,

function( rws, left, right )
    return ReducedForm( rws, left + right );
end );


#############################################################################
##
#M  ReducedZero( <rws> )
##
InstallMethod( ReducedOne, 
    "ReducedForm",
    true,
    [ IsRewritingSystem and IsBuiltFromAdditiveMagmaWithInverses ],
    0,

function( rws )
    return ReducedForm( rws, Zero(UnderlyingFamily(rws)) );
end );


#############################################################################
##

#E  rws.gi  . . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
##
