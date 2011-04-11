#!/bin/sh
#############################################################################
##
#W  mktest.sh      Test the examples in GAP manual files       Volkmar Felsch
##
#H  $Id$
##
#Y  Copyright (C) 2002, Lehrstuhl D für Mathematik, RWTH Aachen, Germany
##
##  mktest.sh [-f] [-i] [-o] [-d] [-c] [-p path] [-s suffix] [-r package]
##            [file1 file2 ...]
##
##  For each of the specified manual files, 'mktest.sh' runs all examples
##  given in that file and constructs a new version of the file which is
##  up-to-date with respect to the output of the examples. The new file gets
##  the suffix '.tst'.
##
##  If '-f' is specified then a full test (including the 'no test' examples)
##  is done.
##
##  If '-i' is specified then the input file for the examples is saved
##  (suffix '.in').
##
##  If '-o' is specified then the output file of the examples is saved
##  (suffix '.out').
##
##  If '-d' is specified then, in addition to the new manual file itself, a
##  file with suffix '.dif' is provided which lists all differences between
##  the old and the new manual file.
##
##  If '-c' is specified then use the context output format of the 'diff'
##  command. This option has no effect if the option '-d' is not specified.
##
##  If '-p path' is specified then that path is assumed to be the path of the
##  directory which contains the given manual files (the default path is
##  '../').
##
##  If '-s suffix' is specified then that suffix is assumed to be the suffix
##  of the manual files (the default suffix is '.tex').
##
##  If '-r pkgname' is specified then GAP will load the package 'pkgname'
##  before running the examples. The option '-r' may be specified more than
##  once if there are more than one packages to be loaded.
##
##  If '-R' is specified then GAP will call 'LoadAllPackages()' before the
##  tests, which loads all available packages.
##  (The default is to load no package.)
##
##  If '-L wspfile' is specified then GAP will load the workspace in
##  'wspfile'.
##
##  If no file names are specified then all files with the given suffix in
##  the directory with the given suffix are handled.
##

#############################################################################
##
##  Define the local call of GAP.
##
gap="../../../bin/gap.sh -b -m 100m -o 500m -A -N -x 80 -r -T"


#############################################################################
##
##  Initialize the input file.
##
echo ' ' > @.pack


#############################################################################
##
##  Parse the arguments.
##
full_test="no"
save_input="no"
save_output="no"
save_dif="no"
context=""
path="../"
suffix=".tex"
wspfile="-"
option="yes"

while [ $option = "yes" ]; do
  option="no"
  case $1 in

    -f) shift; option="yes"; full_test="yes";;

    -i) shift; option="yes"; save_input="yes";;

    -o) shift; option="yes"; save_output="yes";;

    -d) shift; option="yes"; save_dif="yes";;

    -c) shift; option="yes"; context="-c";;

    -p) shift; option="yes"; path=$1; shift;;

    -s) shift; option="yes"; suffix=$1; shift;;

    -r) shift; option="yes"; echo 'LoadPackage("'$1'");' >> @.pack; shift;;

    -R) shift; option="yes"; echo 'LoadAllPackages();' >> @.pack;;

    -L) shift; option="yes"; wspfile=$1; shift;;

  esac
done

#############################################################################
##
##  Get a list of the file names to be handled.
##
if [ $# = 0 ]; then
  ls $path*$suffix > @.files
  ed - @.files << \%
    1,$s/^.*\///
    1,$s/\..*$//
    w
%
else
  touch @.files
  rm @.files
  touch @.files
  for i
  do
    echo $i >> @.files
  done
fi

#############################################################################
##
##  Loop over the given files.
##
for i in `cat @.files`
do

#############################################################################
##
##  Initialize the new manual file by a copy of the old one.
##
echo "testing "$i$suffix
old=$path$i$suffix
cp $old @.new
chmod 644 @.new

#############################################################################
##
##  Add a dummy example to make sure that there is at least one example to be
##  handled.
##
ed - @.new << \%
  $a
dummy example:
\beginexample
gap> dummy:=true;;
\endexample
.
  w
%

#############################################################################
##
##  Add a 'no test' example (to ensure that the following works) and then
##  change all 'no test' examples to ordinary text.
##
if [ $full_test = "no" ]; then
  ed - @.new << \%
    0a
%notest
\beginexample
\endexample
.
    1,$g/%notest/t /^\\endexample/-1
    1,$g/%notest/j
    w
%
fi

#############################################################################
##
##  Construct the input file:
##
##  Get a copy of the file,
##  remove all lines outside of the examples,
##  remove all output lines from the examples,
##  remove the prompts from the input lines,
##  insert approriate calls of the function 'LogTo' to write each example to
##  a separate file,
##  replace each occurrence of "||" by "|".
##
cp @.new @.in

ed - @.in << \%
  $a
dummy
\beginexample
.
  1,/^\\beginexample/-1d
  1,$g/^\\endexample/+1,/^\\beginexample/-1d
  $d
  1,$s/^/#@ /
  1,$s/^#@ [gap]*>/@&/
  1,$s/^.*beginexample/@&/
  1,$s/^.*endexample/@&/
  1,$v/^@#@/d
  1,$s/@#@ //
  1,$s/[gap]*> //
  1,$s/^.*beginexample.*$/E_1:=last;E_0:=E_0+1;E_2:=E_1;/
  1,$s/;E_2:=E_1;/&LogTo(Concatenation("@tmp",String(E_0)));/
  1,$s/^.*endexample.*/LogTo(   );/
  $a
quit;
||
.
  1,$s/||/|/g
  $d
  0r @.pack
  0a
E_0:=10000;
.
  w
%

#############################################################################
##
##  Run the examples.
##
touch @tmp
rm @tmp*
if [ $wspfile = "-" ]; then
  $gap < @.in > /dev/null
else
  $gap -L $wspfile < @.in > /dev/null
fi
cat @tmp* > @.out

#############################################################################
##
##  Remove the 'LogTo' statements from the output file.
##
ed - @.out << \%
  $a
LogTo(   ); gap> LogTo(   );
.
  1,$s/.gap> LogTo(   );/&@/
  1,$s/LogTo(   );@//
  1,$g/LogTo(   );/d
  w
%

#############################################################################
##
##  Insert the output of the examples into the new manual file.
##
ed - @.new << \%
  1,$s/^\\beginexample/\\begin1example/
  1,$s/^\\endexample/\\end1example/
  w
%

ls @tmp* > @.ls

for j in `cat @.ls`
do
  cp $j @tmpj

  ed - @tmpj << \%
    $a
||
.
    1,$s/|/||/g
    1,$s/||||/||/g
    $d
    w
%

  ed - @.new << \%
    /^\\begin1example/s/begin1example/begin2example/
    /^\\end1example/s/end1example/end2example/
    /^\\begin2example/+1,/^\\end2example/-1d
    /^\\begin2example/r @tmpj
    /^\\begin2example/s/begin2example/beginexample/
    /^\\end2example/s/end2example/endexample/
    w
%
done

#############################################################################
##
##  Remove the 'LogTo' statements and the dummy example from the new manual
##  file.
##
ed - @.new << \%
  $a
LogTo(   ); gap> LogTo(   );
.
  1,$s/.gap> LogTo(   );/&@/
  1,$s/LogTo(   );@//
  1,$g/LogTo(   );/d
  $-3,$d
  w
%

#############################################################################
##
##  Change the 'no test' examples back to what they were before and remove
##  the dummy 'no test' example.
##
if [ $full_test = "no" ]; then
  sed -e '/^%notest\\beginexample/a\
\\beginexample' @.new > @.tmp

  mv @.tmp @.new

  ed - @.new << \%
    1,$s/^%notest\\beginexample/%notest/
    1,$s/^%notest\\endexample/\\endexample/
    1,3d
    w
%
fi

#############################################################################
##
##  Save a file which lists the differences between the old and the new file.
##
if [ $save_dif = "yes" ]; then
  diff $context $old @.new > $i.dif
fi

#############################################################################
##
##  Save the new file.
##
mv @.new $i.tst

#############################################################################
##
##  Remove the dummy example from the input file and save it.
##
if [ $save_input = "yes" ]; then
  ed - @.in << \%
    $-3,$-1d
    w
%
  mv @.in $i.in
else
  rm @.in
fi

#############################################################################
##
##  Remove the dummy example from the output file and save it.
##
if [ $save_output = "yes" ]; then
  ed - @.out << \%
    $d
    w
%
  mv @.out $i.out
else
  rm @.out
fi

#############################################################################
##
done
rm @.files @.ls @.pack @tmp*

