#############################################################################
##
#W  gprd.gd                     GAP library                    Heiko Thei"sen
##
#H  @(#)$Id$
##
#H  $Log$
#H  Revision 4.7  1997/03/10 15:43:12  beick
#H  removed new catagory for products of groups, added products of pc groups
#H
#H  Revision 4.6  1997/02/13 10:38:18  ahulpke
#H  Added 'Embedding' and 'Projection' for semidirect products
#H
#H  Revision 4.5  1997/01/16 10:46:24  fceller
#H  renamed 'NewConstructor' to 'NewOperation',
#H  renamed 'NewOperationFlags1' to 'NewConstructor'
#H
#H  Revision 4.4  1997/01/13 16:39:58  htheisse
#H  made `Embeddings' and `Projections' mutable
#H
#H  Revision 4.3  1996/12/19 09:59:00  htheisse
#H  added revision lines
#H
#H  Revision 4.2  1996/10/30 15:46:31  htheisse
#H  fixed errors with group products
#H
#H  Revision 4.1  1996/10/30 15:16:59  htheisse
#H  added products of permutation groups
#H
##
Revision.gprd_gd :=
    "@(#)$Id$";

DirectProduct := NewOperationArgs( "DirectProduct" );
DirectProduct2 := NewOperation( "DirectProduct2", [IsGroup, IsGroup] );
SubdirectProduct := NewOperation( "SubdirectProduct",
    [ IsGroup, IsGroup, IsGroupHomomorphism, IsGroupHomomorphism ] );
SemidirectProduct := NewOperation( "SemidirectProduct",
    [ IsGroup, IsGroupHomomorphism, IsGroup ] );
WreathProduct := NewOperation( "WreathProduct",
    [ IsGroup, IsGroup, IsGroupHomomorphism ] );
WreathProductProductAction := NewOperationArgs( "WreathProductProductAction" );

#############################################################################
##
#A  DirectProductInfo( <G> )
##
DirectProductInfo := NewAttribute( "DirectProductInfo", IsGroup, "mutable" );
SetDirectProductInfo := Setter(DirectProductInfo);
HasDirectProductInfo := Tester(DirectProductInfo);

#############################################################################
##
#A  SubdirectProductInfo( <G> )
##
SubdirectProductInfo := NewAttribute( "SubdirectProductInfo", IsGroup, 
                                      "mutable" );
SetSubdirectProductInfo := Setter(SubdirectProductInfo);
HasSubdirectProductInfo := Tester(SubdirectProductInfo);

#############################################################################
##
#A  SemidirectProductInfo( <G> )
##
SemidirectProductInfo := NewAttribute( "SemidirectProductInfo", IsGroup, 
                                       "mutable" );
SetSemidirectProductInfo := Setter(SemidirectProductInfo);
HasSemidirectProductInfo := Tester(SemidirectProductInfo);

#############################################################################
##
#A  WreathProductInfo( <G> )
##
WreathProductInfo := NewAttribute( "WreathProductInfo", IsGroup, "mutable" );
SetWreathProductInfo := Setter(WreathProductInfo);
HasWreathProductInfo := Tester(WreathProductInfo);

#############################################################################
##
#O EmbeddingOp( <G>, <i> )
##
EmbeddingOp := NewOperation( "EmbeddingOp", [IsGroup, IsInt and IsPosRat] );

#############################################################################
##
#O ProjectionOp( <G>, <i> )
##
ProjectionOp := NewOperation( "ProjectionOp", [IsGroup, IsInt and IsPosRat] );

#############################################################################
##

#E  Emacs variables . . . . . . . . . . . . . . local variables for this file
##  Local Variables:
##  mode:             outline-minor
##  outline-regexp:   "#[WCROAPMFVE]"
##  fill-column:      77
##  End:
#############################################################################
