#############################################################################
##
#W  float.gd                       GAP library              Laurent Bartholdi
##
#H  @(#)$Id: float.gd,v 4.6 2011/06/20 21:55:23 gap Exp $
##
#Y  Copyright (C) 2011 Laurent Bartholdi
##
##  This file deals with general float functions
##
Revision.float_gd :=
  "@(#)$Id: float.gd,v 4.6 2011/06/20 21:55:23 gap Exp $";

#############################################################################
##
#C  Floateans
##
DeclareCategory("IsFloat", IsScalar and IsCommutativeElement and IsZDFRE);
DeclareCategoryFamily("IsFloat");
DeclareCategoryCollections("IsFloat");
DeclareCategoryCollections("IsFloatCollection");
BindGlobal("FloatsFamily", NewFamily("FloatsFamily", IsFloat));
DeclareConstructor("NewFloat",[IsFloat,IsObject]);
#############################################################################

#############################################################################
##
#O Unary operations
##
## <#GAPDoc Label="FLOAT_UNARY">
## <ManSection>
##   <Oper Name="Cos" Arg="x"/>
##   <Oper Name="Sin" Arg="x"/>
##   <Oper Name="Tan" Arg="x"/>
##   <Oper Name="Sec" Arg="x"/>
##   <Oper Name="Csc" Arg="x"/>
##   <Oper Name="Cot" Arg="x"/>
##   <Oper Name="Asin" Arg="x"/>
##   <Oper Name="Acos" Arg="x"/>
##   <Oper Name="Atan" Arg="x"/>
##   <Oper Name="Atan2" Arg="y x"/>
##   <Oper Name="Cosh" Arg="x"/>
##   <Oper Name="Sinh" Arg="x"/>
##   <Oper Name="Tanh" Arg="x"/>
##   <Oper Name="Sech" Arg="x"/>
##   <Oper Name="Csch" Arg="x"/>
##   <Oper Name="Coth" Arg="x"/>
##   <Oper Name="Asinh" Arg="x"/>
##   <Oper Name="Acosh" Arg="x"/>
##   <Oper Name="Atanh" Arg="x"/>
##   <Oper Name="Log" Arg="x"/>
##   <Oper Name="Log2" Arg="x"/>
##   <Oper Name="Log10" Arg="x"/>
##   <Oper Name="Exp" Arg="x"/>
##   <Oper Name="Exp2" Arg="x"/>
##   <Oper Name="Exp10" Arg="x"/>
##   <Oper Name="Cuberoot" Arg="x"/>
##   <Oper Name="Square" Arg="x"/>
##   <Oper Name="Ceil" Arg="x"/>
##   <Oper Name="Floor" Arg="x"/>
##   <Oper Name="Round" Arg="x"/>
##   <Oper Name="Trunc" Arg="x"/>
##   <Oper Name="SignFloat" Arg="x"/>
##   <Description>
##     Usual mathematical functions.
##   </Description>
## </ManSection>
##
## <ManSection>
##   <Oper Name="PrecisionFloat" Arg="x"/>
##   <Returns>The precision of <A>x</A></Returns>
##   <Description>
##     This function returns the precision, counted in number of bits,
##     of the floating-point number <A>x</A>.
##   </Description>
## </ManSection>
##
## <ManSection>
##   <Prop Name="IsPInfinity" Arg="x"/>
##   <Prop Name="IsNInfinity" Arg="x"/>
##   <Prop Name="IsXInfinity" Arg="x"/>
##   <Prop Name="IsFinite" Arg="x" Label="float"/>
##   <Prop Name="IsNaN" Arg="x"/>
##   <Description>
##     Returns <K>true</K> if the floating-point number <A>x</A> is
##     respectively <M>+\infty</M>, <M>-\infty</M>, <M>\pm\infty</M>,
##     finite, or `not a number', such as the result of <C>0.0/0.0</C>.
##   </Description>
## </ManSection>
##
## <ManSection>
##   <Var Name="FLOAT"/>
##   <Description>
##     This record contains useful floating-point constants: <List>
##     <Mark>DECIMAL_DIG</Mark> <Item>Maximal number of useful digits;</Item>
##     <Mark>DIG</Mark> <Item>Number of significant digits;</Item>
##     <Mark>VIEW_DIG</Mark> <Item>Number of digits to print in short view;</Item>
##     <Mark>EPSILON</Mark> <Item>Smallest number such that <M>1\neq1+\epsilon</M>;</Item>
##     <Mark>MANT_DIG</Mark> <Item>Number of bits in the mantissa;</Item>
##     <Mark>MAX</Mark> <Item>Maximal representable number;</Item>
##     <Mark>MAX_10_EXP</Mark> <Item>Maximal decimal exponent;</Item>
##     <Mark>MAX_EXP</Mark> <Item>Maximal binary exponent;</Item>
##     <Mark>MIN</Mark> <Item>Minimal positive representable number;</Item>
##     <Mark>MIN_10_EXP</Mark> <Item>Minimal decimal exponent;</Item>
##     <Mark>MIN_EXP</Mark> <Item>Minimal exponent;</Item>
##     <Mark>INFINITY</Mark> <Item>Positive infinity;</Item>
##     <Mark>NINFINITY</Mark> <Item>Negative infinity;</Item>
##     <Mark>NAN</Mark> <Item>Not-a-number,</Item>
##     </List>
##     as well as mathematical constants <C>E</C>, <C>LOG2E</C>, <C>LOG10E</C>,
##     <C>LN2</C>, <C>LN10</C>, <C>PI</C>, <C>PI_2</C>, <C>PI_4</C>,
##     <C>1_PI</C>, <C>2_PI</C>, <C>2_SQRTPI</C>, <C>SQRT2</C>, <C>SQRT1_2</C>.
##   </Description>
## </ManSection>
## <#/GAPDoc>
##
DeclareOperation("Cos",[IsFloat]);
DeclareOperation("Sin",[IsFloat]);
DeclareOperation("Tan",[IsFloat]);
DeclareOperation("Sec",[IsFloat]);
DeclareOperation("Csc",[IsFloat]);
DeclareOperation("Cot",[IsFloat]);
DeclareOperation("Asin",[IsFloat]);
DeclareOperation("Acos",[IsFloat]);
DeclareOperation("Atan",[IsFloat]);
DeclareOperation("Cosh",[IsFloat]);
DeclareOperation("Sinh",[IsFloat]);
DeclareOperation("Tanh",[IsFloat]);
DeclareOperation("Sech",[IsFloat]);
DeclareOperation("Csch",[IsFloat]);
DeclareOperation("Coth",[IsFloat]);
DeclareOperation("Asinh",[IsFloat]);
DeclareOperation("Acosh",[IsFloat]);
DeclareOperation("Atanh",[IsFloat]);
DeclareOperation("Log",[IsFloat]);
DeclareOperation("Log2",[IsFloat]);
DeclareOperation("Log10",[IsFloat]);
DeclareOperation("Exp",[IsFloat]);
DeclareOperation("Exp2",[IsFloat]);
DeclareOperation("Exp10",[IsFloat]);
DeclareOperation("CubeRoot",[IsFloat]);
DeclareOperation("Square",[IsFloat]);
DeclareOperation("Ceil",[IsFloat]);
DeclareOperation("Floor",[IsFloat]);
DeclareOperation("Round",[IsFloat]);
DeclareOperation("Trunc",[IsFloat]);
DeclareOperation("Atan2", [IsFloat,IsFloat]);
DeclareOperation("FrExp", [IsFloat]);
DeclareOperation("LdExp", [IsFloat,IsInt]);
DeclareAttribute("Argument", IsFloat);
DeclareAttribute("AbsoluteValue", IsFloat);
#DeclareAttribute("Norm", IsFloat); #already defined
DeclareOperation("Hypothenuse", [IsFloat,IsFloat]);
DeclareOperation("Frac",[IsFloat]);
DeclareOperation("SinCos",[IsFloat]);
DeclareOperation("Erf",[IsFloat]);
DeclareOperation("Zeta",[IsFloat]);
DeclareOperation("Gamma",[IsFloat]);

DeclareOperation("PrecisionFloat",[IsFloat]);
DeclareOperation("SignFloat",[IsFloat]);

DeclareOperation("Sup", [IsFloat]);
DeclareOperation("Inf", [IsFloat]);
DeclareOperation("Mid", [IsFloat]);
DeclareOperation("DiameterOfInterval", [IsFloat]);
#DeclareOperation("Diameter", [IsFloat]);
DeclareOperation("Overlaps", [IsFloat,IsFloat]);
DeclareOperation("IsDisjoint", [IsFloat,IsFloat]);

DeclareProperty("IsPInfinity", IsFloat);
DeclareProperty("IsNInfinity", IsFloat);
DeclareProperty("IsXInfinity", IsFloat);
DeclareProperty("IsFinite", IsFloat);
DeclareProperty("IsNaN", IsFloat);
#############################################################################

#############################################################################
##
#O Constructor
##
## <#GAPDoc Label="Float">
## <ManSection>
##   <Oper Name="Float" Arg="obj"/>
##   <Oper Name="NewFloat" Arg="filter, obj"/>
##   <Returns>A new floating-point number, based on <A>obj</A></Returns>
##   <Description>
##     This function creates a new floating-point number.
##
##     <P/> If <A>obj</A> is a rational number, the created number is created
##     with sufficient precision so that the number can (usually) be converted
##     back to the original number (see <Ref Oper="Rat" BookName="ref"/> and
##     <Ref Oper="Rat"/>). For an integer, the precision, if unspecified, is
##     chosen sufficient so that <C>Int(Float(obj))=obj</C> always holds, but
##     at least 64 bits.
##
##     <P/> <A>obj</A> may also be a string, which may be of the form
##     <C>"3.14e0"</C> or <C>".314e1"</C> or <C>".314@1"</C> etc.
##
##     <P/> An option may be passed to specify, it bits, a desired precision.
##     The format is <C>Float("3.14":PrecisionFloat:=1000)</C> to create
##     a 1000-bit approximation of <M>3.14</M>.
##
##     <P/> In particular, if <A>obj</A> is already a floating-point number,
##     then <C>Float(obj:PrecisionFloat:=prec)</C> creates a copy of
##     <A>obj</A> with a new precision.
##     prec
##   </Description>
## </ManSection>
##
## <ManSection>
##   <Oper Name="Rat" Arg="f"/>
##   <Returns>A rational approximation to <A>f</A></Returns>
##   <Description>
##     This command constructs a rational approximation to the
##     floating-point number <A>f</A>. Of course, it is not guaranteed to
##     return the original rational number <A>f</A> was created from, though
##     it returns the most `reasonable' one given the precision of
##     <A>f</A>.
##
##     <P/> If used in the form <C>Rat(f:maxdenom:=max)</C>, the rational returned is
##     the first one with denominator at most <A>max</A>.
##   </Description>
## </ManSection>
##
## <ManSection>
##   <Func Name="InstallFloatsHandler" Arg="rec"/>
##   <Func Name="InstallFloatsConstructors" Arg="rec"/>
##   <Description>
##     Installs a new interface to floating-point numbers in &GAP;.
##   </Description>
## </ManSection>
## <#/GAPDoc>
##
DeclareGlobalFunction("Float");
DeclareGlobalFunction("InstallFloatsHandler");
DeclareGlobalFunction("InstallFloatsConstructors");
#############################################################################

# these variables are read-write
FLOAT := fail; # record holding all float information

# MAX_FLOAT_LITERAL_CACHE_SIZE := 1000; # this could be set to avoid saturating the cache, in case some code evaluates lots of function expressions

#############################################################################
##
#E
