#############################################################################
##
#W  grplatt.gi                GAP library                   Martin Sch"onert,
#W                                                          J"urgen Mnich,
#AH        How much of the code dates back to Mnich ?
#W                                                          Alexander Hulpke
##
#H  @(#)$Id$
##
#Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
##
##  This  file  contains declarations for subgroup latices
##
Revision.grplatt_gi:=
  "@(#)$Id$";

#############################################################################
##
#F  Zuppos(<G>) .  set of generators for cyclic subgroups of prime power size
##
InstallMethod(Zuppos,"group",true,[IsGroup],0,
function (G)
local   zuppos,            # set of zuppos,result
	c,                 # a representative of a class of elements
	o,                 # its order
	N,                 # normalizer of < c >
	t;                 # loop variable

  # compute the zuppos
  zuppos:=[One(G)];
  for c in List(ConjugacyClasses(G),Representative)  do
    o:=Order(c);
    if IsPrimePowerInt(o)  then
      if ForAll([2..o],i -> Gcd(o,i) <> 1 or not c^i in zuppos) then
	N:=Normalizer(G,Subgroup(G,[c]));
	for t in RightTransversal(G,N)  do
	  Add(zuppos,c^t);
	od;
      fi;
    fi;
  od;

  # return the set of zuppos
  Sort(zuppos);
  #IsSet(zuppos);
  return zuppos;
end);


#############################################################################
##
#M  ConjugacyClassSubgroups(<G>,<g>)  . . . . . . . . . . . .  constructor
##
InstallMethod(ConjugacyClassSubgroups,IsIdentical,[IsGroup,IsGroup],0,
function(G,U)
local filter,cl;

    cl:=Objectify(NewKind(CollectionsFamily(FamilyObj(G)),
      IsConjugacyClassSubgroupsRep),rec());
    SetActingDomain(cl,G);
    SetRepresentative(cl,U);
    SetFunctionOperation(cl,OnPoints);
    return cl;
end);

#############################################################################
##
#M  PrintObj(<cl>)  . . . . . . . . . . . . . . . . . . . .  print function
##
InstallMethod(PrintObj,true,[IsConjugacyClassSubgroupsRep],0,
function(cl)
    Print("ConjugacyClassSubgroups(",ActingDomain(cl),",",
           Representative(cl),")");
end);


#############################################################################
##
#M  ConjugacyClassesSubgroups(<G>) . classes of subgroups of a group
##
InstallMethod(ConjugacyClassesSubgroups,"group",true,[IsGroup],0,
function(G)
  return ConjugacyClassesSubgroups(LatticeSubgroups(G));
end);

InstallOtherMethod(ConjugacyClassesSubgroups,"lattice",true,
  [IsLatticeSubgroupsRep],0,
function(L)
  return L!.conjugacyClassesSubgroups;
end);

CopiedGroup := function (G)
local S;
  S:=Subgroup(Parent(G),GeneratorsOfGroup(G));
  if IsIdentical(S,G) then
    Error("Subgroup returned identical object!");
  fi;
  return S;
end;

#############################################################################
##
#M  LatticeSubgroups(<G>)  . . . . . . . . . .  lattice of subgroups
##
InstallMethod(LatticeSubgroups,"cyclic extension",true,[IsGroup],0,
function(G)
local   lattice,           # lattice (result)
	factors,           # factorization of <G>'s size
	zuppos,            # generators of prime power order
	zupposPrime,       # corresponding prime
	zupposPower,       # index of power of generator
	nrClasses,         # number of classes
	classes,           # list of all classes
	classesZups,       # zuppos blist of classes
	classesExts,       # extend-by blist of classes
	perfect,           # classes of perfect subgroups of <G>
	perfectNew,        # this class of perfect subgroups is new
	perfectZups,       # zuppos blist of perfect subgroups
	layerb,            # begin of previous layer
	layere,            # end of previous layer
	H,                 # representative of a class
	Hzups,             # zuppos blist of <H>
	Hexts,             # extend blist of <H>
	C,                 # class of <I>
	I,                 # new subgroup found
	Ielms,             # elements of <I>
	Izups,             # zuppos blist of <I>
	Icopy,             # copy of <I>
	N,                 # normalizer of <I>
	Nzups,             # zuppos blist of <N>
	Ncopy,             # copy of <N>
	Jzups,             # zuppos of a conjugate of <I>
	Kzups,             # zuppos of a representative in <classes>
	reps,              # transversal of <N> in <G>
	h,i,k,l,r;      # loop variables

    # compute the factorized size of <G>
    factors:=Factors(Size(G));

    # compute a system of generators for the cyclic sgr. of prime power size
    zuppos:=Zuppos(G);
    Info(InfoLattice,1,"<G> has ",Length(zuppos)," zuppos");

    # compute the prime corresponding to each zuppo and the index of power
    zupposPrime:=[];
    zupposPower:=[];
    for r  in zuppos  do
        i:=SmallestRootInt(Order(r));
        Add(zupposPrime,i);
        k:=0;
        while k <> false  do
            k:=k + 1;
            if GcdInt(i,k) = 1  then
                l:=Position(zuppos,r^(i*k));
                if l <> false  then
                    Add(zupposPower,l);
                    k:=false;
                fi;
            fi;
        od;
    od;
    Info(InfoLattice,1,"powers computed");

    perfect:=RepresentativesPerfectSubgroups(G);
    perfect:=Filtered(perfect,i->Size(i)>1);

    perfectZups:=[];
    perfectNew :=[];
    for i  in [1..Length(perfect)]  do
        I:=perfect[i];
        Icopy:=CopiedGroup(I);
        SetSize(I,Size(Icopy));
        perfectZups[i]:=BlistList(zuppos,AsList(Icopy));
        perfectNew[i]:=true;
    od;
    Info(InfoLattice,1,"<G> has ",Length(perfect),
                  " representatives of perfect subgroups");

    # initialize the classes list
    nrClasses:=1;
    classes:=ConjugacyClassSubgroups(G,TrivialSubgroup(G));
    SetSize(classes,1);
    classes:=[classes];
    classesZups:=[BlistList(zuppos,[One(G)])];
    classesExts:=[DifferenceBlist(BlistList(zuppos,zuppos),classesZups[1])];
    layerb:=1;
    layere:=1;

    # loop over the layers of group (except the group itself)
    for l  in [1..Length(factors)-1]  do
        Info(InfoLattice,1,"doing layer ",l,",",
                      "previous layer has ",layere-layerb+1," classes");

        # extend representatives of the classes of the previous layer
        for h  in [layerb..layere]  do

            # get the representative,its zuppos blist and extend-by blist
            H:=Representative(classes[h]);
            Hzups:=classesZups[h];
            Hexts:=classesExts[h];
            Info(InfoLattice,2,"extending subgroup ",h,", size = ",Size(H));

            # loop over the zuppos whose <p>-th power lies in <H>
            for i  in [1..Length(zuppos)]  do
                if Hexts[i] and Hzups[zupposPower[i]]  then

                    # make the new subgroup <I>
                    I:=Subgroup(Parent(G),Concatenation(GeneratorsOfGroup(H),
                                                             [zuppos[i]]));
                    Icopy:=CopiedGroup(I);
                    SetSize(Icopy,Size(H) * zupposPrime[i]);
                    SetSize(I,Size(Icopy));

                    # compute the zuppos blist of <I>
                    Ielms:=AsList(Icopy);
                    Izups:=BlistList(zuppos,Ielms);

                    # compute the normalizer of <I>
                    N:=Normalizer(G,Icopy);
                    Ncopy:=CopiedGroup(N);
                    SetSize(N,Size(Ncopy));
		    #AH 'NormalizerInParent' attribute ?
                    #if IsParent(G)  and not IsBound(I.normalizer)  then
                    #    I.normalizer:=Subgroup(Parent(G),GeneratorsOfGroup(N));
                    #    I.normalizer.size:=Size(N);
                    #fi;
                    Info(InfoLattice,2,"found new class ",nrClasses+1,
		         ", size = ",Size(I),
                         " length = ",Size(G) / Size(N));

                    # make the new conjugacy class
                    C:=ConjugacyClassSubgroups(G,I);
                    SetSize(C,Size(G) / Size(N));
                    SetStabilizerOfExternalSet(C,
		      Subgroup(Parent(G),GeneratorsOfGroup(N)));
                    nrClasses:=nrClasses + 1;
                    classes[nrClasses]:=C;

                    # store the extend by list
                    if l < Length(factors)-1  then
                        classesZups[nrClasses]:=Izups;
                        Nzups:=BlistList(zuppos,AsList(Ncopy));
                        SubtractBlist(Nzups,Izups);
                        classesExts[nrClasses]:=Nzups;
                    fi;

                    # compute the transversal
                    reps:=RightTransversal(G,Ncopy);
                    Unbind(Icopy);
                    Unbind(Ncopy);

                    # loop over the conjugates of <I>
                    for r  in reps  do

                        # compute the zuppos blist of the conjugate
                        if r = One(G)  then
                            Jzups:=Izups;
                        else
                            Jzups:=BlistList(zuppos,OnTuples(Ielms,r));
                        fi;

                        # loop over the already found classes
                        for k  in [h..layere]  do
                            Kzups:=classesZups[k];

                            # test if the <K> is a subgroup of <J>
                            if IsSubsetBlist(Jzups,Kzups)  then

                                # don't extend <K> by the elements of <J>
                                SubtractBlist(classesExts[k],Jzups);

                            fi;

                        od;

                    od;

                    # now we are done with the new class
                    Unbind(Ielms);
                    Unbind(reps);
                    Info(InfoLattice,2,"tested inclusions");

                fi; # if Hexts[i] and Hzups[zupposPower[i]]  then ...
            od; # for i  in [1..Length(zuppos)]  do ...

            # remove the stuff we don't need any more
            Unbind(classesZups[h]);
            Unbind(classesExts[h]);

        od; # for h  in [layerb..layere]  do ...

        # add the classes of perfect subgroups
        for i  in [1..Length(perfect)]  do
            if    perfectNew[i]
              and IsPerfectGroup(perfect[i])
              and Length(Factors(Size(perfect[i]))) = l
            then

                # make the new subgroup <I>
                I:=perfect[i];
                Icopy:=CopiedGroup(I);
                SetSize(I,Size(Icopy));

                # compute the zuppos blist of <I>
                Ielms:=AsList(Icopy);
                Izups:=BlistList(zuppos,Ielms);

                # compute the normalizer of <I>
                N:=Normalizer(G,Icopy);
                Ncopy:=CopiedGroup(N);
                SetSize(N,Size(Ncopy));
		# AH: NormalizerInParent ?
                #if IsParent(G)  and not IsBound(I.normalizer)  then
                #    I.normalizer:=Subgroup(Parent(G),N.generators);
                #    I.normalizer.size:=Size(N);
                #fi;
                Info(InfoLattice,2,"found perfect class ",nrClasses+1,
                     "size = ",Size(I),", length = ",
		     Size(G) / Size(N));

                # make the new conjugacy class
                C:=ConjugacyClassSubgroups(G,I);
                SetSize(C,Size(G)/Size(N));
                SetStabilizerOfExternalSet(C,
		  Subgroup(Parent(G),GeneratorsOfGroup(N)));
                nrClasses:=nrClasses + 1;
                classes[nrClasses]:=C;

                # store the extend by list
                if l < Length(factors)-1  then
                    classesZups[nrClasses]:=Izups;
                    Nzups:=BlistList(zuppos,AsList(Ncopy));
                    SubtractBlist(Nzups,Izups);
                    classesExts[nrClasses]:=Nzups;
                fi;

                # compute the transversal
                reps:=RightTransversal(G,Ncopy);
                Unbind(Icopy);
                Unbind(Ncopy);

                # loop over the conjugates of <I>
                for r  in reps  do

                    # compute the zuppos blist of the conjugate
                    if r = One(G)  then
                        Jzups:=Izups;
                    else
                        Jzups:=BlistList(zuppos,OnTuples(Ielms,r));
                    fi;

                    # loop over the perfect classes
                    for k  in [i+1..Length(perfect)]  do
                        Kzups:=perfectZups[k];

                        # throw away classes that appear twice in perfect
                        if Jzups = Kzups  then
                            perfectNew[k]:=false;
                            perfectZups[k]:=[];
                        fi;

                    od;

                od;

                # now we are done with the new class
                Unbind(Ielms);
                Unbind(reps);
                Info(InfoLattice,2,"tested equalities");

                # unbind the stuff we dont need any more
                perfectZups[i]:=[];

            fi; 
	    # if IsPerfectGroup(I) and Length(Factors(Size(I))) = layer the...
        od; # for i  in [1..Length(perfect)]  do

        # on to the next layer
        layerb:=layere+1;
        layere:=nrClasses;

    od; # for l  in [1..Length(factors)-1]  do ...

    # add the whole group to the list of classes
    Info(InfoLattice,1,"doing layer ",Length(factors),",",
                  " previous layer has ",layere-layerb+1," classes");
    Info(InfoLattice,2,"found whole group, size = ",Size(G),",","length = 1");
    C:=ConjugacyClassSubgroups(G,G);
    SetSize(C,1);
    nrClasses:=nrClasses + 1;
    classes[nrClasses]:=C;

    # return the list of classes
    Info(InfoLattice,1,"<G> has ",nrClasses," classes,",
                  " and ",Sum(classes,Size)," subgroups");

    # sort the classes
    Sort(classes,
                  function (c,d)
                     return Size(Representative(c)) < Size(Representative(d))
                        or (Size(Representative(c)) = Size(Representative(d))
                            and Size(c) < Size(d));
                   end);

    # create the lattice
    lattice:=Objectify(NewKind(FamilyObj(classes),IsLatticeSubgroupsRep),
                       rec());
    lattice!.conjugacyClassesSubgroups:=classes;
    lattice!.group     :=G;

    # return the lattice
    return lattice;
end);

#############################################################################
##
#M  Print for lattice
##
InstallMethod(PrintObj,"lattice",true,[IsLatticeSubgroupsRep],0,
function(l)
  Print("LatticeSubgroups(",l!.group,",\# ",
    Length(l!.conjugacyClassesSubgroups)," classes, ",
    Sum(l!.conjugacyClassesSubgroups,Size)," subgroups)");
end);

#############################################################################
##
#M  RepresentativesPerfectSubgroups
##
InstallMethod(RepresentativesPerfectSubgroups,"generic",true,[IsGroup],0,
function(G)
  if IsSolvableGroup(G) then
    return [TrivialSubgroup(G)];
  else
    Error("will be implemented as soon as the Holt/Plesken Library is available");
  fi;
end);

#############################################################################
##
#M  MaximalSubgroupsLattice
##
InstallMethod(MaximalSubgroupsLattice,"cyclic extension",true,
  [IsLatticeSubgroupsRep],0,
function (L)
    local   maximals,          # maximals as pair <class>,<conj> (result)
            maximalsZups,      # their zuppos blist
            cnt,               # count for information messages
            zuppos,            # generators of prime power order
            classes,           # list of all classes
            classesZups,       # zuppos blist of classes
            I,                 # representative of a class
            Ielms,             # elements of <I>
            Izups,             # zuppos blist of <I>
            Icopy,             # copy of <I>
            N,                 # normalizer of <I>
            Ncopy,             # copy of <N>
            Jzups,             # zuppos of a conjugate of <I>
            Kzups,             # zuppos of a representative in <classes>
            reps,              # transversal of <N> in <G>
	    grp,	       # the group
            i,k,l,r;         # loop variables

    grp:=L!.group;
    # compute the lattice,fetch the classes,zuppos,and representatives
    classes:=L!.conjugacyClassesSubgroups;
    classesZups:=[];

    # compute a system of generators for the cyclic sgr. of prime power size
    zuppos:=Zuppos(grp);

    # initialize the maximals list
    Info(InfoLattice,1,"computing maximal relationship");
    maximals:=List(classes,c -> []);
    maximalsZups:=List(classes,c -> []);

    # find the minimal supergroups of the whole group
    Info(InfoLattice,2,"testing class ",Length(classes),", size = ",
         Size(grp),", length = 1, included in 0 minimal subs");
    classesZups[Length(classes)]:=BlistList(zuppos,zuppos);

    # loop over all classes
    for i  in [Length(classes)-1,Length(classes)-2..1]  do

        # take the subgroup <I>
        I:=Representative(classes[i]);
        Icopy:=CopiedGroup(I);
        Info(InfoLattice,2," testing class ",i);

        # compute the zuppos blist of <I>
        Ielms:=AsList(Icopy);
        Izups:=BlistList(zuppos,Ielms);
        classesZups[i]:=Izups;

        # compute the normalizer of <I>
        N:=Normalizer(grp,Icopy);
        Ncopy:=CopiedGroup(N);

        # compute the right transversal
        reps:=RightTransversal(grp,Ncopy);
        Unbind(Icopy);
        Unbind(Ncopy);

        # initialize the counter
        cnt:=0;

        # loop over the conjugates of <I>
        for r  in [1..Length(reps)]  do

            # compute the zuppos blist of the conjugate
            if reps[r] = One(grp)  then
                Jzups:=Izups;
            else
                Jzups:=BlistList(zuppos,OnTuples(Ielms,reps[r]));
            fi;

            # loop over all other (larger classes)
            for k  in [i+1..Length(classes)]  do
                Kzups:=classesZups[k];

                # test if the <K> is a minimal supergroup of <J>
                if    IsSubsetBlist(Kzups,Jzups)
                  and ForAll(maximalsZups[k],
                              zups -> not IsSubsetBlist(zups,Jzups))
                then
                    Add(maximals[k],[ i,r ]);
                    Add(maximalsZups[k],Jzups);
                    cnt:=cnt + 1;
                fi;

            od;

        od;

        # inform about the count
        Unbind(Ielms);
        Unbind(reps);
        Info(InfoLattice,2,"size = ",Size(I),", length = ",
	  Size(grp) / Size(N),", included in ",cnt," minimal sups");

    od;

    return maximals;
end);

#############################################################################
##
#M  MinimalSupergroupsLattice
##
InstallMethod(MinimalSupergroupsLattice,"cyclic extension",true,
  [IsLatticeSubgroupsRep],0,
function (L)
    local   minimals,          # minimals as pair <class>,<conj> (result)
            minimalsZups,      # their zuppos blist
            cnt,               # count for information messages
            zuppos,            # generators of prime power order
            classes,           # list of all classes
            classesZups,       # zuppos blist of classes
            I,                 # representative of a class
            Ielms,             # elements of <I>
            Izups,             # zuppos blist of <I>
            Icopy,             # copy of <I>
            N,                 # normalizer of <I>
            Ncopy,             # copy of <N>
            Jzups,             # zuppos of a conjugate of <I>
            Kzups,             # zuppos of a representative in <classes>
            reps,              # transversal of <N> in <G>
	    grp,	       # the group;
            i,k,l,r;         # loop variables

    grp:=L!.group;
    # compute the lattice,fetch the classes,zuppos,and representatives
    classes:=L!.conjugacyClassesSubgroups;
    classesZups:=[];

    # compute a system of generators for the cyclic sgr. of prime power size
    zuppos:=Zuppos(grp);

    # initialize the minimals list
    Info(InfoLattice,1,"computing minimal relationship");
    minimals:=List(classes,c -> []);
    minimalsZups:=List(classes,c -> []);

    # loop over all classes
    for i  in [1..Length(classes)-1]  do

        # take the subgroup <I>
        I:=Representative(classes[i]);
        Icopy:=CopiedGroup(I);

        # compute the zuppos blist of <I>
        Ielms:=AsList(Icopy);
        Izups:=BlistList(zuppos,Ielms);
        classesZups[i]:=Izups;

        # compute the normalizer of <I>
        N:=Normalizer(grp,Icopy);
        Ncopy:=CopiedGroup(N);

        # compute the right transversal
        reps:=RightTransversal(grp,Ncopy);
        Unbind(Icopy);
        Unbind(Ncopy);

        # initialize the counter
        cnt:=0;

        # loop over the conjugates of <I>
        for r  in [1..Length(reps)]  do

            # compute the zuppos blist of the conjugate
            if reps[r] = One(grp)  then
                Jzups:=Izups;
            else
                Jzups:=BlistList(zuppos,OnTuples(Ielms,reps[r]));
            fi;

            # loop over all other (smaller classes)
            for k  in [1..i-1]  do
                Kzups:=classesZups[k];

                # test if the <K> is a maximal subgroup of <J>
                if    IsSubsetBlist(Jzups,Kzups)
                  and ForAll(minimalsZups[k],
                              zups -> not IsSubsetBlist(Jzups,zups))
                then
                    Add(minimals[k],[ i,r ]);
                    Add(minimalsZups[k],Jzups);
                    cnt:=cnt + 1;
                fi;

            od;

        od;

        # inform about the count
        Unbind(Ielms);
        Unbind(reps);
        Info(InfoLattice,2,"testing class ",i,", size = ",Size(I),
	     ", length = ",Size(grp) / Size(N),", includes ",cnt,
	     " maximal subs");

    od;

    # find the maximal subgroups of the whole group
    cnt:=0;
    for k  in [1..Length(classes)-1]  do
        if minimals[k] = []  then
            Add(minimals[k],[ Length(classes),1 ]);
            cnt:=cnt + 1;
        fi;
    od;
    Info(InfoLattice,2,"testing class ",Length(classes),", size = ",
        Size(grp),", length = 1, includes ",cnt," maximal subs");

    return minimals;
end);

#############################################################################
##
#F  MaximalSubgroupClassReps(<G>) . . . . reps of conjugacy classes of
#F                                                          maximal subgroups
##
InstallMethod(MaximalSubgroupClassReps,"using lattice",true,[IsGroup],0,
function (G)
    local   maxs,lat;

    #AH special AG treatment
    # simply compute all conjugacy classes and take the maximals
    lat:=LatticeSubgroups(G);
    maxs:=MaximalSubgroupsLattice(lat)[Length(lat!.conjugacyClassesSubgroups)];
    maxs:=List(lat!.conjugacyClassesSubgroups{
       Set(maxs{[1..Length(maxs)]}[1])},Representative);
    return maxs;
end);

#############################################################################
##
#M  TableOfMarks(<G>)   . . . . . . . . . . . . . . . . make a table of marks
##
InstallMethod(TableOfMarks,"cyclic extension",true,[IsGroup],0,
function (G)
local   tom,               # table of marks (result)
	mrks,              # marks for one class
	ind,               # index of <I> in <N>
	zuppos,            # generators of prime power order
	classes,           # list of all classes
	classesZups,       # zuppos blist of classes
	I,                 # representative of a class
	Ielms,             # elements of <I>
	Izups,             # zuppos blist of <I>
	Icopy,             # copy of <I>
	N,                 # normalizer of <I>
	Ncopy,             # copy of <N>
	Jzups,             # zuppos of a conjugate of <I>
	Kzups,             # zuppos of a representative in <classes>
	reps,              # transversal of <N> in <G>
	i,k,l,r;         # loop variables

    # compute the lattice,fetch the classes,zuppos,and representatives
    classes:=ConjugacyClassesSubgroups(G);
    classesZups:=[];

    # compute a system of generators for the cyclic sgr. of prime power size
    zuppos:=Zuppos(G);

    # initialize the table of marks
    Info(InfoLattice,1,"computing table of marks");
    tom:=rec(subs:=List(classes,c -> []),
                marks:=List(classes,c -> []));

    # loop over all classes
    for i  in [1..Length(classes)-1]  do

        # take the subgroup <I>
        I:=Representative(classes[i]);
        Icopy:=CopiedGroup(I);

        # compute the zuppos blist of <I>
        Ielms:=AsList(Icopy);
        Izups:=BlistList(zuppos,Ielms);
        classesZups[i]:=Izups;

        # compute the normalizer of <I>
        N:=Normalizer(G,Icopy);
        Ncopy:=CopiedGroup(N);
        ind:=Size(Ncopy) / Size(Icopy);
        # compute the right transversal
        reps:=RightTransversal(G,Ncopy);
        Unbind(Icopy);
        Unbind(Ncopy);

        # set up the marking list
        mrks   :=0 * [1..Length(classes)];
        mrks[1]:=Length(reps) * ind;
        mrks[i]:=1 * ind;

        # loop over the conjugates of <I>
        for r  in [1..Length(reps)]  do

            # compute the zuppos blist of the conjugate
            if reps[r] = One(G) then
                Jzups:=Izups;
            else
                Jzups:=BlistList(zuppos,OnTuples(Ielms,reps[r]));
            fi;

            # loop over all other (smaller classes)
            for k  in [2..i-1]  do
                Kzups:=classesZups[k];

                # test if the <K> is a subgroup of <J>
                if IsSubsetBlist(Jzups,Kzups)  then
                    mrks[k]:=mrks[k] + ind;
                fi;

            od;

        od;

        # compress this line into the table of marks
        for k  in [1..i]  do
            if mrks[k] <> 0  then
                Add(tom.subs[i],k);
                Add(tom.marks[i],mrks[k]);
            fi;
        od;
        Unbind(Ielms);
        Unbind(reps);
        Info(InfoLattice,2,"testing class ",i,", size = ",Size(I),
	     ", length = ",Size(G) / Size(N),", includes ",
	     Length(tom.marks[i])," classes");

    od;

    # handle the whole group
      Info(InfoLattice,2,"testing class ",Length(classes),", size = ",Size(G),
	   ", length = ",1,", includes ",
           Length(tom.marks[Length(classes)])," classes");
    tom.subs[Length(classes)]:=[1..Length(classes)] + 0;
    tom.marks[Length(classes)]:=0 * [1..Length(classes)] + 1;

    # return the table of marks
    return tom;
end);

#############################################################################
##
#E  grplatt.gi . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
##

