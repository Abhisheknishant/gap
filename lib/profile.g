#############################################################################
##
#W  profile.g                   GAP Library                      Frank Celler
##
#H  @(#)$Id: profile.g,v 4.49 2010/07/09 16:47:07 alexk Exp $
##
#Y  Copyright (C)  1996,  Lehrstuhl D für Mathematik,  RWTH Aachen,  Germany
#Y  (C) 1998 School Math and Comp. Sci., University of St Andrews, Scotland
#Y  Copyright (C) 2002 The GAP Group
##
##  This file contains the profiling functions.
##
Revision.profile_g :=
    "@(#)$Id: profile.g,v 4.49 2010/07/09 16:47:07 alexk Exp $";


#############################################################################
##
#V  PROFILED_FUNCTIONS  . . . . . . . . . . . . . . list of profiled function
#V  PREV_PROFILED_FUNCTIONS . . . . . . list of previously profiled functions
##
PROFILED_FUNCTIONS := [];
PROFILED_FUNCTIONS_NAMES := [];

PREV_PROFILED_FUNCTIONS := [];
PREV_PROFILED_FUNCTIONS_NAMES := [];


#############################################################################
##
#F  ClearProfile()  . . . . . . . . . . . . . . clear all profile information
##
##  <#GAPDoc Label="ClearProfile">
##  <ManSection>
##  <Func Name="ClearProfile" Arg=''/>
##
##  <Description>
##  clears all stored profile information.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
BIND_GLOBAL("ClearProfile",function()
    local   i;

    for i  in Concatenation(PROFILED_FUNCTIONS, PREV_PROFILED_FUNCTIONS)  do
        CLEAR_PROFILE_FUNC(i);
    od;
end);


#############################################################################
##
#F  ProfileInfo( <functions>, <mincount>, <mintime> )
##
##  This function collects the information about the currently profiled
##  functions in a table.
##  It is assumed that profiling has been switched off before the function
##  gets called, in order not to mess up the shown data by the function
##  calls inside `ProfileInfo' or inside the function that called it.
##
##  The differences between this function and the corresponding code used in
##  the GAP 4.4 function `DisplayProfile' are that
##  - negative values in the columns 3 and 5 are replaced by zero,
##  - the footer line showing ``OTHER'' contains a value (total memory
##    allocated) in column 4,
##  - the table really contains the rows for those functions that satisfy the
##    conditions defined for `PROFILETHRESHOLD' (its definition says `<=' but
##    the old implementation checked `<'),
##  - the ``TOTAL'' and ``OTHER'' lines show the summation over all profiled
##    functions, including the ones for which no line is shown due to the
##    restrictions imposed by `PROFILETHRESHOLD'.
##
##  This function, in particular the component `funs' in the record it
##  returns, is used also in the package `Browse'.
##
BindGlobal( "ProfileInfo", function( funcs, mincount, mintime )
    local all, nam, prof, sort, funs, ttim, tsto, otim, osto, i, tmp, str,
          v3, v5, pi;

    all:= Concatenation( PROFILED_FUNCTIONS,
                         PREV_PROFILED_FUNCTIONS );
    nam:= Concatenation( PROFILED_FUNCTIONS_NAMES,
                         PREV_PROFILED_FUNCTIONS_NAMES );

    if funcs = "all" then
      funcs:= all;
    fi;

    prof:= [];
    sort:= [];
    funs:= [];
    ttim:= 0;
    tsto:= 0;
    otim:= 0;
    osto:= 0;
    for i in [ 1 .. Length( all ) ] do
      tmp:= PROF_FUNC( all[i] );
      if ( mincount <= tmp[1] or mintime <= tmp[2] ) and all[i] in funcs then
        if IsString( nam[i] ) then
          str:= nam[i];
        else
          str:= Concatenation( nam[i] );
        fi;
        v3:= tmp[2] - tmp[3];
        if v3 < 0 then
          v3:= 0;
        fi;
        v5:= tmp[4] - tmp[5];
        if v5 < 0 then
          v5:= 0;
        fi;
        Add( prof, [ tmp[1], tmp[3], v3, tmp[5], v5, str ] );
        Add( funs, all[i] );
        Add( sort, tmp[2] );
      else
        otim:= otim + tmp[3];
        osto:= osto + tmp[5];
      fi;
      ttim:= ttim + tmp[3];
      tsto:= tsto + tmp[5];
    od;

    # sort functions according to total time spent
    pi:= Sortex( sort );
    prof:= Permuted( prof, pi );
    funs:= Permuted( funs, pi );

    return rec( prof:= prof, ttim:= ttim, tsto:= tsto,
                funs:= funs, otim:= otim, osto:= osto,
                denom:= [ 1, 1, 1, 1024, 1024, 1 ],
                widths:= [ 7, 7, 7, 7, 7, -1 ],
                labelsCol:= [ "  count", "self/ms", "chld/ms", "stor/kb",
                              "chld/kb", "function" ],
                sepCol:= "  " );
end );


#############################################################################
##
#F  DisplayProfile( [<functions>][,][<mincount>, <mintime>] )
#V  PROFILETHRESHOLD
##
##  <#GAPDoc Label="DisplayProfile">
##  <ManSection>
##  <Func Name="DisplayProfile" Arg="[functions][,][mincount, mintime]"/>
##  <Var Name="PROFILETHRESHOLD"/>
##
##  <Description>
##  Called without arguments, <Ref Func="DisplayProfile"/> displays the
##  profile information for profiled operations, methods and functions.
##  If an argument <A>functions</A> is given, only profile information for
##  the functions in the list <A>functions</A> is shown.
##  If two integer values <A>mincount</A>, <A>mintime</A> are given as
##  arguments then the output is restricted to those functions that were
##  called at least <A>mincount</A> times or for which the total time spent
##  (see below) was at least <A>mintime</A> milliseconds.
##  The defaults for <A>mincount</A> and <A>mintime</A> are the entries of
##  the list stored in the global variable <Ref Var="PROFILETHRESHOLD"/>.
##  <P/>
##  The default value of <Ref Var="PROFILETHRESHOLD"/> is
##  <C>[ 10000, 30 ]</C>.
##  <P/>
##  Profile information is displayed in a list of lines for all functions
##  (including operations and methods) which are profiled.
##  For each function,
##  <Q>count</Q> gives the number of times the function has been called.
##  <Q>self/ms</Q> gives the time (in milliseconds) spent in the function
##  itself,
##  <Q>chld/ms</Q> the time (in milliseconds) spent in profiled functions
##  called from within this function,
##  <Q>stor/kb</Q> the amount of storage (in kilobytes) allocated by the
##  function itself, and
##  <Q>chld/kb</Q> the amount of storage (in kilobytes) allocated by
##  profiled functions called from within this function.
##  <P/>
##  The list is sorted according to the total time spent in the functions,
##  that is the sum of the values in the columns
##  <Q>self/ms</Q> and <Q>chld/ms</Q>.
##  <P/>
##  At the end of the list, two lines are printed that show the total time
##  used and the total memory allocated by the profiled functions not shown
##  in the list (label <C>OTHER</C>)
##  and by all profiled functions (label <C>TOTAL</C>), respectively.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
PROFILETHRESHOLD:=[10000,30]; # cnt, time

BIND_GLOBAL("DisplayProfile",function( arg )
    local i, funcs, mincount, mintime, prof, w, n, s, j, k, line, str, denom,
          qotim, qosto;

    # stop profiling of functions needed below
    for i  in PROFILED_FUNCTIONS  do
        UNPROFILE_FUNC(i);
    od;

    # unravel the arguments
    if 0 = Length(arg)  then
      funcs:= "all";
      mincount:= PROFILETHRESHOLD[1];
      mintime:= PROFILETHRESHOLD[2];
    elif Length( arg ) = 1 and IsList( arg[1] ) then
      funcs:= arg[1];
      mincount:= PROFILETHRESHOLD[1];
      mintime:= PROFILETHRESHOLD[2];
    elif Length( arg ) = 2 and IsInt( arg[1] ) and IsInt( arg[2] ) then
      funcs:= "all";
      mincount:= arg[1];
      mintime:= arg[2];
    elif Length( arg ) = 3 and IsList( arg[1] ) and IsInt( arg[2] )
                           and IsInt( arg[3] ) then
      funcs:= arg[1];
      mincount:= arg[2];
      mintime:= arg[3];
    elif ForAll( arg, IsFunction ) then
      funcs:= arg;
      mincount:= PROFILETHRESHOLD[1];
      mintime:= PROFILETHRESHOLD[2];
    else
      # Start profiling again.
      for i in PROFILED_FUNCTIONS do
        PROFILE_FUNC( i );
      od;
      Error(
        "usage: DisplayProfile( [<functions>][,][<mincount>, <mintime>] )" );
    fi;

    prof:= ProfileInfo( funcs, mincount, mintime );

    # set width and names
    w:= prof.widths;
    n:= prof.labelsCol;
    s:= prof.sepCol;

    # use screen size for the name
    j := 0;
    k:= Length( w );
    for i  in [ 1 .. Length(w) ]  do
        if i <> k then
            j := j + AbsInt(w[i]) + Length(s);
        fi;
    od;
    if w[k] < 0  then
        w[k] := - AbsInt( SizeScreen()[1] - j - Length(s) -2);
    else
        w[k] := AbsInt( SizeScreen()[1] - j - Length(s)-2 );
    fi;

    # print a nice header
    line := "";
    for j  in [ 1 .. Length( w ) ]  do
        if j <> 1 then
          Append( line, s );
        fi;
        str := String( n[j], w[j] );
        if Length(str) > AbsInt(w[j])  then
            str := str{[1..AbsInt(w[j])-1]};
            Add( str, '*' );
        fi;
        Append( line, str );
    od;
    Print( line, "\n" );

    # print profile
    denom:= prof.denom;
    for i in prof.prof do
        line := "";
        for j  in [ 1 .. Length( w ) ]  do
            if j <> 1 then
                Append( line, s );
            fi;
            if denom[j] <> 1 then
              str:= String( QuoInt( i[j], denom[j] ), w[j] );
            else
              str:= String( i[j], w[j] );
            fi;
            if Length(str) > AbsInt(w[j])  then
                str := str{[1..AbsInt(w[j])-1]};
                Add( str, '*' );
            fi;
            Append( line, str );
        od;
        Print( line, "\n" );
    od;

    # print other
    qotim:= QuoInt( prof.otim, denom[2] );
    qosto:= QuoInt( prof.osto, denom[4] );
    if 0 < qotim or 0 < qosto then
        line := "";
        for j  in [ 1 .. Length( w ) ]  do
            if j <> 1 then
              Append( line, s   );
            fi;
            if j = 2  then
                str := String( qotim, w[j] );
            elif j = 4  then
                str := String( qosto, w[j] );
            elif j = 6  then
                str := String( "OTHER", w[j] );
            else
                str := String( " ", w[j] );
            fi;
            if Length(str) > AbsInt(w[j])  then
                str := str{[1..AbsInt(w[j])-1]};
                Add( str, '*' );
            fi;
            Append( line, str );
        od;
        Print( line, "\n" );
    fi;

    # print total
    line := "";
    for j  in [ 1 .. Length( w ) ]  do
        if j <> 1 then
          Append( line, s   );
        fi;
        if j = 2  then
          str := String( prof.ttim, w[j] );
        elif j = 4  then
            str := String( QuoInt( prof.tsto, 1024 ), w[j] );
        elif j = 6  then
            str := String( "TOTAL", w[j] );
        else
            str := String( " ", w[j] );
        fi;
        if Length(str) > AbsInt(w[j])  then
            str := str{[1..AbsInt(w[j])-1]};
            Add( str, '*' );
        fi;
        Append( line, str );
    od;
    Print( line, "\n" );

    # start profiling of functions needed above
    for i  in PROFILED_FUNCTIONS  do
        PROFILE_FUNC(i);
    od;
end);


#############################################################################
##
#F  ProfileFunctions( <funcs> )
##
##  <#GAPDoc Label="ProfileFunctions">
##  <ManSection>
##  <Func Name="ProfileFunctions" Arg='funcs'/>
##
##  <Description>
##  starts profiling for all function in the list <A>funcs</A>.
##  You can use <Ref Func="ProfileGlobalFunctions"/>
##  to turn profiling on for all globally declared functions simultaneously.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
BIND_GLOBAL("ProfileFunctions",function( arg )
    local   funcs,  names,  hands,  pi,  OLD_PROFILED_FUNCTIONS,
            OLD_PROFILED_FUNCTIONS_NAMES,
            i,  phands,  pi2,  j,  x,  y,
                        f;

  if Length(arg)=2 and IsList(arg[1]) and IsList(arg[2]) then
    funcs:=arg[1];
    names:=arg[2];
  else
    if IsFunction(arg[1]) then
      funcs:=arg;
    else
      funcs:=arg[1];
    fi;
    names:=List(funcs,NameFunction);
  fi;

  Append(PROFILED_FUNCTIONS, funcs);
  Append(PROFILED_FUNCTIONS_NAMES, names);
  hands := List(PROFILED_FUNCTIONS, HANDLE_OBJ);
  pi := Sortex(hands);
  OLD_PROFILED_FUNCTIONS := Permuted(PROFILED_FUNCTIONS, pi);
  OLD_PROFILED_FUNCTIONS_NAMES := Permuted(PROFILED_FUNCTIONS_NAMES, pi);
  PROFILED_FUNCTIONS := [OLD_PROFILED_FUNCTIONS[1]];
  PROFILED_FUNCTIONS_NAMES := [OLD_PROFILED_FUNCTIONS_NAMES[1]];
  for i in [2..Length(OLD_PROFILED_FUNCTIONS)] do
      if hands[i-1] <> hands[i] then
          Add(PROFILED_FUNCTIONS, OLD_PROFILED_FUNCTIONS[i]);
          Add(PROFILED_FUNCTIONS_NAMES, OLD_PROFILED_FUNCTIONS_NAMES[i]);
      fi;
  od;

  hands := List(funcs, HANDLE_OBJ);
  Sort(hands);
  phands := List(PREV_PROFILED_FUNCTIONS, HANDLE_OBJ);
  pi2 := Sortex(phands)^-1;
  i := 1;
  j := 1;
  while i <= Length(hands) and j <= Length(phands) do
      x := hands[i];
      y := phands[j];
      if x < y then
          i := i+1;
      elif y < x then
          j := j+1;
      else
          Unbind(PREV_PROFILED_FUNCTIONS[j^pi2]);
          Unbind(PREV_PROFILED_FUNCTIONS_NAMES[j^pi2]);
          j := j+1;
      fi;
  od;
  PREV_PROFILED_FUNCTIONS      :=Compacted(PREV_PROFILED_FUNCTIONS);
  PREV_PROFILED_FUNCTIONS_NAMES:=Compacted(PREV_PROFILED_FUNCTIONS_NAMES);
  for f in funcs do
      PROFILE_FUNC(f);
      CLEAR_PROFILE_FUNC(f);
  od;

end);


#############################################################################
##
#F  UnprofileFunctions( <funcs> ) . . . . . . . . . . . . unprofile functions
##
##  <#GAPDoc Label="UnprofileFunctions">
##  <ManSection>
##  <Func Name="UnprofileFunctions" Arg='funcs'/>
##
##  <Description>
##  stops profiling for all function in the list <A>funcs</A>.
##  Recorded information is still kept, so you can  display it even after
##  turning the profiling off.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
BIND_GLOBAL("UnprofileFunctions",function( arg )
local list,  f,  pos;

    if Length(arg)=1 and not IsFunction(arg[1]) then
      list:=arg[1];
    else
      list:=arg;
    fi;

    for f  in list  do
        pos := Position( PROFILED_FUNCTIONS, f );
        if pos <> fail  then
            Add(PREV_PROFILED_FUNCTIONS,PROFILED_FUNCTIONS[pos]);
            Add(PREV_PROFILED_FUNCTIONS_NAMES,PROFILED_FUNCTIONS_NAMES[pos]);
            Unbind( PROFILED_FUNCTIONS[pos] );
            Unbind( PROFILED_FUNCTIONS_NAMES[pos] );
            UNPROFILE_FUNC(f);
        fi;
    od;
    PROFILED_FUNCTIONS       := Compacted(PROFILED_FUNCTIONS);
    PROFILED_FUNCTIONS_NAMES := Compacted(PROFILED_FUNCTIONS_NAMES);
end);


#############################################################################
##
#V  PROFILED_METHODS  . . . . . . . . . . . . . . .  list of profiled methods
##
##  <ManSection>
##  <Var Name="PROFILED_METHODS"/>
##
##  <Description>
##  </Description>
##  </ManSection>
##
PROFILED_METHODS := [];


#############################################################################
##
#F  ProfileMethods( <ops> ) . . . . . . . . . . . . . start profiling methods
##
##  <#GAPDoc Label="ProfileMethods">
##  <ManSection>
##  <Func Name="ProfileMethods" Arg='ops'/>
##
##  <Description>
##  starts profiling of the methods for all operations in the list
##  <A>ops</A>.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
BIND_GLOBAL("ProfileMethods",function( arg )
    local   funcs,  names,  op,  name,  i,  meth,  j,  hands,
            NEW_PROFILED_METHODS;
    arg := Flat(arg);
    funcs := [];
    names := [];
    for op  in arg  do
        name := NameFunction(op);
        for i  in [ 0 .. 6 ]  do
            meth := METHODS_OPERATION( op, i );
            if meth <> fail  then
                for j  in [ 0, (4+i) .. Length(meth)-(4+i) ]  do
                    Add( funcs, meth[j+(2+i)] );
                    if name = meth[j+(4+i)]  then
                        Add( names, [ "Meth(", name, ")" ] );
                    else
                        Add( names, meth[j+(4+i)] );
                    fi;
                od;
            fi;
        od;
    od;
    ProfileFunctions( funcs,names );
    Append(PROFILED_METHODS, funcs);
    hands := List(PROFILED_METHODS, HANDLE_OBJ);
    SortParallel(hands, PROFILED_METHODS);
    NEW_PROFILED_METHODS := [PROFILED_METHODS[1]];
    for i in [2..Length(hands)] do
        if hands[i] <> hands[i-1] then
            Add(NEW_PROFILED_METHODS, PROFILED_METHODS[i]);
        fi;
    od;
    PROFILED_METHODS := NEW_PROFILED_METHODS;
end);


#############################################################################
##
#F  UnprofileMethods( <ops> ) . . . . . . . . . . . .  stop profiling methods
##
##  <#GAPDoc Label="UnprofileMethods">
##  <ManSection>
##  <Func Name="UnprofileMethods" Arg='ops'/>
##
##  <Description>
##  stops profiling of the methods for all operations in the list <A>ops</A>.
##  Recorded information is still kept, so you can  display it even after
##  turning the profiling off.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
BIND_GLOBAL("UnprofileMethods",function( arg )
    local   funcs,  op,  i,  meth,  j;

    arg := Flat(arg);
    funcs := [];
    for op  in arg  do
        for i  in [ 0 .. 6 ]  do
            meth := METHODS_OPERATION( op, i );
            if meth <> fail  then
                for j  in [ 0, (4+i) .. Length(meth)-(4+i) ]  do
                    Add( funcs, meth[j+(2+i)] );
                od;
            fi;
        od;
    od;
    UnprofileFunctions(funcs);
end);


#############################################################################
##
#F  ProfileOperations( [<true/false>] ) . . . . . . . . .  start/stop/display
##
##  <#GAPDoc Label="ProfileOperations">
##  <ManSection>
##  <Func Name="ProfileOperations" Arg='[bool]'/>
##
##  <Description>
##  Called with argument <K>true</K>,
##  <Ref Func="ProfileOperations"/>
##  starts profiling of all operations.
##  Old profile information for all operations is cleared.
##  A function call with the argument <K>false</K>
##  stops profiling of all operations.
##  Recorded information is still kept,
##  so you can display it even after turning the profiling off.
##  <P/>
##  When <Ref Func="ProfileOperations"/> is called without argument,
##  profile information for all operations is displayed
##  (see&nbsp;<Ref Func="DisplayProfile"/>).
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
PROFILED_OPERATIONS := [];

BIND_GLOBAL("ProfileOperationsOn",function()
    local   prof;

    # Note that the list of operations may have grown since the last call.
    prof := OPERATIONS{[ 1, 3 .. Length(OPERATIONS)-1 ]};
    PROFILED_OPERATIONS := prof;
    UnprofileMethods(prof);
    ProfileFunctions( prof );
end);

BIND_GLOBAL("ProfileOperationsOff",function()
    UnprofileFunctions(PROFILED_OPERATIONS);
    UnprofileMethods(PROFILED_OPERATIONS);

    # methods for the kernel functions
    UnprofileMethods(\+,\-,\*,\/,\^,\mod,\<,\=,\in,
                     \.,\.\:\=,IsBound\.,Unbind\.,
                     \[\],\[\]\:\=,IsBound\[\],Unbind\[\]);
#T Why?  These operations are listed in PFOFILED_OPERATIONS!
end);

BIND_GLOBAL("ProfileOperations",function( arg )
    if 0 = Length(arg)  then
        DisplayProfile(PROFILED_OPERATIONS);
    elif arg[1] = true then
      ProfileOperationsOn();
    elif arg[1] = false then
      ProfileOperationsOff();
    else
        Print( "usage: ProfileOperations( [<true/false>] )" );
    fi;
end);


#############################################################################
##
#F  ProfileOperationsAndMethods( [<true/false>] ) . . . .  start/stop/display
##
##  <#GAPDoc Label="ProfileOperationsAndMethods">
##  <ManSection>
##  <Func Name="ProfileOperationsAndMethods" Arg='[bool]'/>
##
##  <Description>
##  Called with argument <K>true</K>,
##  <Ref Func="ProfileOperationsAndMethods"/>
##  starts profiling of all operations and their methods.
##  Old profile information for these functions is cleared.
##  A function call with the argument <K>false</K>
##  stops profiling of all operations and their methods.
##  Recorded information is still kept,
##  so you can display it even after turning the profiling off.
##  <P/>
##  When <Ref Func="ProfileOperationsAndMethods"/> is called without
##  argument,
##  profile information for all operations and their methods is displayed,
##  see&nbsp;<Ref Func="DisplayProfile"/>.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
BIND_GLOBAL("ProfileOperationsAndMethodsOn",function()
    local   prof;

    prof := OPERATIONS{[ 1, 3 .. Length(OPERATIONS)-1 ]};
    PROFILED_OPERATIONS := prof;
    ProfileFunctions( prof );
    ProfileMethods(prof);

    # methods for the kernel functions
    ProfileMethods(\+,\-,\*,\/,\^,\mod,\<,\=,\in,
                     \.,\.\:\=,IsBound\.,Unbind\.,
                     \[\],\[\]\:\=,IsBound\[\],Unbind\[\]);
#T Why?  These operations are listed in PFOFILED_OPERATIONS!
end);

ProfileOperationsAndMethodsOff := ProfileOperationsOff;

BIND_GLOBAL("ProfileOperationsAndMethods",function( arg )
    if 0 = Length(arg)  then
        DisplayProfile(Concatenation(PROFILED_OPERATIONS,PROFILED_METHODS));
    elif arg[1] = true then
      ProfileOperationsAndMethodsOn();
    elif arg[1] = false then
      ProfileOperationsAndMethodsOff();
    else
        Print( "usage: ProfileOperationsAndMethods( [<true/false>] )" );
    fi;
end );


#############################################################################
##
#F  ProfileGlobalFunctions( [<true/false>] )
##
##  <#GAPDoc Label="ProfileGlobalFunctions">
##  <ManSection>
##  <Func Name="ProfileGlobalFunctions" Arg='[bool]'/>
##
##  <Description>
##  Called with argument <K>true</K>,
##  <Ref Func="ProfileGlobalFunctions"/>
##  starts profiling of all functions that have been declared via
##  <Ref Func="DeclareGlobalFunction"/>.
##  Old profile information for all these functions is cleared.
##  A function call with the argument <K>false</K>
##  stops profiling of all these functions.
##  Recorded information is still kept,
##  so you can display it even after turning the profiling off.
##  <P/>
##  When <Ref Func="ProfileGlobalFunctions"/> is called without argument,
##  profile information for all global functions is displayed,
##  see&nbsp;<Ref Func="DisplayProfile"/>.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
PROFILED_GLOBAL_FUNCTIONS := [];

BIND_GLOBAL( "ProfileGlobalFunctions", function( arg )
    local name, func, funcs;
    if 0 = Length(arg) then
        DisplayProfile( PROFILED_GLOBAL_FUNCTIONS );
    elif arg[1] = true then
        PROFILED_GLOBAL_FUNCTIONS  := [];
        for name in GLOBAL_FUNCTION_NAMES do
            if IsBoundGlobal(name) then
                func := ValueGlobal(name);
                if IsFunction(func) then
                    Add(PROFILED_GLOBAL_FUNCTIONS, func);
                fi;
            fi;
        od;
        ProfileFunctions(PROFILED_GLOBAL_FUNCTIONS);
    elif arg[1] = false then
        UnprofileFunctions(PROFILED_GLOBAL_FUNCTIONS);
    else
      Print( "usage: ProfileGlobalFunctions( [<true/false>] )" );
    fi;
end);


#############################################################################
##
#F  ProfileFunctionsInGlobalVariables()
##
##  <ManSection>
##  <Func Name="ProfileFunctionsInGlobalVariables" Arg=''/>
##
##  <Description>
##  </Description>
##  </ManSection>
##
PROFILED_GLOBAL_VARIABLE_FUNCTIONS := [];

BIND_GLOBAL( "ProfileFunctionsInGlobalVariables", function( arg )
    local name, func, funcs;
    if 0 = Length(arg) then
        DisplayProfile( PROFILED_GLOBAL_VARIABLE_FUNCTIONS );
    elif arg[1] then
        PROFILED_GLOBAL_VARIABLE_FUNCTIONS  := [];
        for name in NamesGVars() do
            if IsBoundGlobal(name) then
                func := ValueGlobal(name);
                if IsFunction(func) then
                    Add(PROFILED_GLOBAL_VARIABLE_FUNCTIONS, func);
                fi;
            fi;
        od;
        ProfileFunctions(PROFILED_GLOBAL_VARIABLE_FUNCTIONS);
    else
        UnprofileFunctions(PROFILED_GLOBAL_VARIABLE_FUNCTIONS);
        PROFILED_GLOBAL_VARIABLE_FUNCTIONS := [];
    fi;
end);



#############################################################################
##
#F  DisplayRevision() . . . . . . . . . . . . . . .  display revision entries
##
##  <#GAPDoc Label="DisplayRevision">
##  <ManSection>
##  <Func Name="DisplayRevision" Arg=''/>
##
##  <Description>
##  Displays the revision numbers of all loaded files from the library.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
BIND_GLOBAL("DisplayRevision",function()
    local   names,  source,  library,  unknown,  name,  p,  s,  type,
            i,  j;

    names   := RecNames( Revision );
    source  := [];
    library := [];
    unknown := [];

    for name  in names  do
        p := Position( name, '_' );
        if p = fail  then
            Add( unknown, name );
        else
            s := name{[p+1..Length(name)]};
            if s = "c" or s = "h"  then
                Add( source, name );
            elif s = "g" or s = "gi" or s = "gd"  then
                Add( library, name );
            else
                Add( unknown, name );
            fi;
        fi;
    od;
    Sort( source );
    Sort( library );
    Sort( unknown );

    for type  in [ source, library, unknown ]  do
        if 0 < Length(type)  then
            if IsIdenticalObj(type,source)  then
                Print( "Source Files\n" );
            elif IsIdenticalObj(type,library)  then
                Print( "Library Files\n" );
            else
                Print( "Unknown Files\n" );
            fi;
            j := 1;
            for name  in type  do
                s := Revision.(name);
                p := Position( s, ',' )+3;
                i := p;
                while s[i] <> ' '  do i := i + 1;  od;
                s := Concatenation( String( Concatenation(
                         name, ":" ), -15 ), String( s{[p..i]},
                         -5 ) );
                if j = 3  then
                    Print( s, "\n" );
                    j := 1;
                else
                    Print( s, "    " );
                    j := j + 1;
                fi;
            od;
            if j <> 1  then Print( "\n" );  fi;
            Print( "\n" );
        fi;
    od;
end);


#############################################################################
##
#F  DisplayCacheStats() . . . . . . . . . . . . . .  display cache statistics
##
##  <#GAPDoc Label="DisplayCacheStats">
##  <ManSection>
##  <Func Name="DisplayCacheStats" Arg=''/>
##
##  <Description>
##  displays statistics about the different caches used by the method
##  selection.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
BIND_GLOBAL("DisplayCacheStats",function()
    local   cache,  names,  pos,  i;

    cache := ShallowCopy(OPERS_CACHE_INFO());
    Append( cache, [
        WITH_HIDDEN_IMPS_FLAGS_CACHE_HIT,
        WITH_HIDDEN_IMPS_FLAGS_CACHE_MISS,
        WITH_IMPS_FLAGS_CACHE_HIT,
        WITH_IMPS_FLAGS_CACHE_MISS,
        NEW_TYPE_CACHE_HIT,
        NEW_TYPE_CACHE_MISS,
    ] );

    names := [
        "AND_FLAGS cache hits",
        "AND_FLAGS cache miss",
        "AND_FLAGS cache losses",
        "Operation L1 cache hits",
        "Operation cache misses",
        "IS_SUBSET_FLAGS calls",
        "IS_SUBSET_FLAGS less trues",
        "IS_SUBSET_FLAGS few trues",
        "Operation TryNextMethod",
        "WITH_HIDDEN_IMPS hits",
        "WITH_HIDDEN_IMPS misses",
        "WITH_IMPS hits",
        "WITH_IMPS misses",
        "NEW_TYPE hits",
        "NEW_TYPE misses",
    ];

    pos := [ 1, 2, 3, 4, 9, 5, 6, 7, 8, 10, 11, 12, 13, 14, 15 ];

    if Length(pos) <> Length(names)  then
        Error( "<pos> and <names> have different lengths" );
    fi;
    if Length(pos) <> Length(cache)  then
        Error( "<pos> and <cache> have different lengths" );
    fi;

    for i  in pos  do
        Print( String( Concatenation(names[i],":"), -30 ),
               String( String(cache[i]), 12 ), "\n" );
    od;

end);


#############################################################################
##
#F  ClearCacheStats() . . . . . . . . . . . . . . . .  clear cache statistics
##
##  <#GAPDoc Label="ClearCacheStats">
##  <ManSection>
##  <Func Name="ClearCacheStats" Arg=''/>
##
##  <Description>
##  clears all statistics about the different caches used by the method
##  selection.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
BIND_GLOBAL("ClearCacheStats",function()
    CLEAR_CACHE_INFO();
    WITH_HIDDEN_IMPS_FLAGS_CACHE_HIT := 0;
    WITH_HIDDEN_IMPS_FLAGS_CACHE_MISS := 0;
    WITH_IMPS_FLAGS_CACHE_HIT := 0;
    WITH_IMPS_FLAGS_CACHE_MISS := 0;
    NEW_TYPE_CACHE_HIT := 0;
    NEW_TYPE_CACHE_MISS := 0;
end);


#############################################################################
##
#F  START_TEST( <id> )  . . . . . . . . . . . . . . . . . . . start test file
#F  STOP_TEST( <file>, <fac> )  . . . . . . . . . . . . . . .  stop test file
##
##  <#GAPDoc Label="StartStopTest">
##  <ManSection>
##  <Heading>Starting and stopping test</Heading>
##  <Func Name="START_TEST" Arg='id'/>
##  <Func Name="STOP_TEST" Arg='file, fac'/>
##
##  <Description>
##  <Ref Func="START_TEST"/> and <Ref Func="STOP_TEST"/> may be optionally
##  used in files that are read via <Ref Func="ReadTest"/>. If used,
##  <Ref Func="START_TEST"/> reinitialize the caches and the global 
##  random number generator, in order to be independent of the reading 
##  order of several test files. Furthermore, the assertion level 
##  (see&nbsp;<Ref Func="Assert"/>) is set to <M>2</M> by 
##  <Ref Func="START_TEST"/> and set back to the previous value in the 
##  subsequent <Ref Func="STOP_TEST"/> call.
##  <P/>
##  To use these options, a test file should be started with a line
##  <P/>
##  <Log><![CDATA[
##  gap> START_TEST( "arbitrary identifier string" );
##  ]]></Log>
##  <P/>
##  (Note that the <C>gap> </C> prompt is part of the line!)
##  <P/>
##  and should be finished with a line
##  <P/>
##  <Log><![CDATA[
##  gap> STOP_TEST( "filename", 10000 );
##  ]]></Log>
##  <P/>
##  Here the string <C>"filename"</C> should give the name of the test file.
##  The number is a proportionality factor that is used to output a
##  <Q>&GAP;stone</Q> speed ranking after the file has been completely
##  processed.
##  For the files provided with the distribution this scaling is roughly
##  equalized to yield the same numbers as produced by the test file
##  <F>tst/combinat.tst</F>.
##  <P/>
##  Note that the functions in <F>tst/testutil.g</F> temporarily replace
##  <Ref Func="STOP_TEST"/> before they call <Ref Func="ReadTest"/>.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
START_TEST := function( name )
    FlushCaches();
    RANDOM_SEED(1);
    Reset(GlobalMersenneTwister, 1);
    GASMAN( "collect" );
    GAPInfo.TestData.START_TIME := Runtime();
    GAPInfo.TestData.START_NAME := name;
    GAPInfo.TestData.AssertionLevel:= AssertionLevel();
    SetAssertionLevel( 2 );
end;

STOP_TEST := function( file, fac )
    local time;

    if not IsBound( GAPInfo.TestData.START_TIME ) then
      Error( "`STOP_TEST' command without `START_TEST' command for `",
             file, "'" );
    fi;
    time:= Runtime() - GAPInfo.TestData.START_TIME;
    Print( GAPInfo.TestData.START_NAME, "\n" );
    if time <> 0 and IsInt( fac ) then
      Print( "GAP4stones: ", QuoInt( fac, time ), "\n" );
    else
      Print( "GAP4stones: infinity\n" );
    fi;
    SetAssertionLevel( GAPInfo.TestData.AssertionLevel );
    Unbind( GAPInfo.TestData.AssertionLevel );
    Unbind( GAPInfo.TestData.START_TIME );
    Unbind( GAPInfo.TestData.START_NAME );
end;


#############################################################################
##
#E

