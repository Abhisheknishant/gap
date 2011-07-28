############################################################################
##
#W snf.gi		IsoPcp					René Hartung
##
#H   @(#)$Id: snf.gi,v 1.1 2010/07/26 05:18:46 gap Exp $
##
Revision.("isopcp/gap/solv/snf_gi"):=
  "@(#)$Id: snf.gi,v 1.1 2010/07/26 05:18:46 gap Exp $";

############################################################################
##
#F SolutionPGroup_SNF( <mat>, <b>, <mod> )
##
## solve the system using an integral Smith normal form algorithm
##
############################################################################
SolutionPGroup_SNF := function( A, b, modulus )
  local prime, 	# the prime number 
	m,n,	# the dimensions of <A>
	D,	# diagonal matrix added to <A>
	Mat,	# the modified matrix <A>
	SNF,	# the Smith normal form of <Mat>
	diag,	# diagonal of <SNF>
	rhs,	# modified right-hand-side
	y,	# solution to Dy=Lb
	ell,	# last non-trivial position in <diag>
	special,# the special solution
	sol,	# the final solutions
	i;	# loop variable
  
  prime := FactorsInt( modulus[1] )[1];
  n := Length( A[1] );
  m := Length( A );

  D := DiagonalMat( modulus );
  Mat := List( [ 1 .. m ], x -> Concatenation( A[x], D[x] ) );

  SNF  := SmithNormalFormIntegerMatTransforms( Mat );
  diag := List( [ 1 .. m ], x -> SNF.normal[x][x] );
  rhs  := SNF.rowtrans * b;

  # the solution to Dy = Lb
  ell := First( [ 1 .. m ], x -> diag[ x ] = 0 );
  if ell = fail then ell := m; else ell := ell - 1; fi;
  for i in [ ell+1 .. m ] do 
    if rhs[i] <> 0 then Display( "in SNF" ); return( fail ); fi; 
  od;

  y := [];
  for i in [ 1 .. ell ] do 
    y[i] := rhs[i] / diag[i];
    if not IsInt( y[i] ) then return( fail ); fi;
  od;
  
  special := ( SNF.coltrans{[ 1 .. n+m ]}{[ 1 .. ell ]} * y ) mod modulus;
  sol := rec( special := special{[ 1 .. m ]}, homogeneous := [] );
# for i in [ell+1..n+m] do
#   Add( sol.homogeneous, SNF.coltrans{[1..m]}[i] );
# od;
  Append( sol.homogeneous, Filtered( List( [ ell+1 .. n+m ], 
          x -> SNF.coltrans{[ 1 .. m ]}[x] mod modulus ), y -> y <> 0 * y ) );

  return( sol );
  end;
