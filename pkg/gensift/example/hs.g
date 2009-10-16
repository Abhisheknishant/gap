# $Id: hs.g,v 1.1.1.1 2004/12/22 13:22:48 gap Exp $
# Example for sifting in HS:
LoadPackage("atlasrep");
gens := AtlasGenerators("HS",1);
g := Group(gens.generators);
sr := PrepareSiftRecords(PreSift.HScl8b,g);
ResetGeneralizedSiftProfile(Length(sr));
Print("Results: ",TestGeneralizedSift(sr,g,1/100,50),"\n");
DisplayGeneralizedSiftProfile();
