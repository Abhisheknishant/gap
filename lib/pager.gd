#############################################################################
##  
#W  pager.gd                     GAP Library                     Frank Lübeck
##  
#H  @(#)$Id: pager.gd,v 1.6 2010/10/10 12:34:38 alexk Exp $
##  
#Y  Copyright  (C) 2001, Lehrstuhl  D  für  Mathematik, RWTH  Aachen, Germany 
#Y (C) 2001 School Math and  Comp. Sci., University of St Andrews, Scotland
#Y Copyright (C) 2002 The GAP Group
##  
##  The  files  pager.g{d,i}  contain  the `Pager'  utility.  A  rudimentary
##  version of this  was integrated in first versions of  GAP's help system.
##  But this utility is certainly useful for other purposes as well.
##  
Revision.pager_gd := 
  "@(#)$Id: pager.gd,v 1.6 2010/10/10 12:34:38 alexk Exp $";

#############################################################################
##  
#F  Pager( <lines> ) . . . . . . . . . . . . display text on screen in a pager
##  
##  This function can be used to display a text on screen using a pager, i.e.,
##  the text is shown page by page. 
##  
##  There is a default builtin pager in GAP which has very limited capabilities
##  but should work on any system.
##  
##  At least on a UNIX system one should use an external pager program like
##  `less' or `more'. {\GAP} assumes that this program has a command line option 
##  `+nr' which starts the display of the text with line number `nr'.
##  
##  Which pager is used can be controlled by setting the variable
##  `GAPInfo.UserPreferences.Pager'.
##  The default setting is `GAPInfo.UserPreferences.Pager := "builtin";'
##  which means that the internal pager is used.
##  
##  On UNIX systems you probably want to set
##  `GAPInfo.UserPreferences.Pager := "less";' or
##  `GAPInfo.UserPreferences.Pager := "more";',
##  you can do this for example in your `gap.ini' file.
##  In that case you can also tell {\GAP} a list of standard options for the
##  external pager. These are specified as list of strings in the variable
##  `GAPInfo.UserPreferences.PagerOptions'.
##  
##  Example:
##    GAPInfo.UserPreferences.Pager  := "less";
##    GAPInfo.UserPreferences.PagerOptions := ["-f", "-r", "-a", "-i", "-M", "-j2"];
##  
##  The argument <lines> can have one of the following forms:
##  
##  (1) a string (i.e., lines are separated by newline characters)
##  (2) a list of strings (without trailing newline characters) 
##  which are interpreted as lines of the text to be shown
##  (3) a record with component `.lines' as in (1) or (2) and 
##  optional further components
##  
##  In case (3) currently the following additional components are used:
##  
##  `.formatted' &
##  can be `false' or `true'. If set to `true' the builtin pager tries 
##  to show the text exactly as it is given (avoiding {\GAP}s automatic 
##  line breaking
##  
##  `.start' &
##  must be an integral number. This is interpreted as the number of the
##  first line shown by the pager (one may see the beginning of the text
##  via back scrolling).
##  

DeclareGlobalFunction("Pager");

