/****************************************************************************
**
*A  integer.c                   GAP source                   Martin Schoenert
**                                                           & Alice Niemeyer
**                                                           & Werner  Nickel
**
*H  @(#)$Id$
**
*Y  Copyright (C)  1996,  Lehrstuhl D fuer Mathematik,  RWTH Aachen,  Germany
**
**  This file implements the  functions  handling  arbitrary  size  integers.
**
**  There are three integer types in GAP: 'T_INT', 'T_INTPOS' and 'T_INTNEG'.
**  Each integer has a unique representation, e.g., an integer  that  can  be
**  represented as 'T_INT' is never  represented as 'T_INTPOS' or 'T_INTNEG'.
**
**  'T_INT' is the type of those integers small enough to fit into  29  bits.
**  Therefor the value range of this small integers is: $-2^{28}...2^{28}-1$.
**  This range contains about 99\% of all integers that usually occur in GAP.
**  (I just made up this number, obviously it depends on the application  :-)
**  Only these small integers can be used as index expression into sequences.
**
**  Small integers are represented by an immediate integer handle, containing
**  the value instead of pointing  to  it,  which  has  the  following  form:
**
**      +-------+-------+-------+-------+- - - -+-------+-------+-------+
**      | guard | sign  | bit   | bit   |       | bit   | tag   | tag   |
**      | bit   | bit   | 27    | 26    |       | 0     | 0     | 1     |
**      +-------+-------+-------+-------+- - - -+-------+-------+-------+
**
**  Immediate integers handles carry the tag 'T_INT', i.e. the last bit is 1.
**  This distuingishes immediate integers from other handles which  point  to
**  structures aligned on 4 byte boundaries and therefor have last bit  zero.
**  (The second bit is reserved as tag to allow extensions of  this  scheme.)
**  Using immediates as pointers and dereferencing them gives address errors.
**
**  To aid overflow check the most significant two bits must always be equal,
**  that is to say that the sign bit of immediate integers has a  guard  bit.
**
**  The macros 'INTOBJ_INT' and 'INT_INTOBJ' should be used to convert  between
**  a small integer value and its representation as immediate integer handle.
**
**  'T_INTPOS' and 'T_INTPOS' are the types of positive  respective  negative
**  integer values  that  can  not  be  represented  by  immediate  integers.
**
**  This large integers values are represented in signed base 65536 notation.
**  That means that the bag of  a  large  integer  has  the  following  form:
**
**      +-------+-------+-------+-------+- - - -+-------+-------+-------+
**      | digit | digit | digit | digit |       | digit | digit | digit |
**      | 0     | 1     | 2     | 3     |       | <n>-2 | <n>-1 | <n>   |
**      +-------+-------+-------+-------+- - - -+-------+-------+-------+
**
**  The value of this  is:  $d0 + d1 65536 + d2 65536^2 + ... + d_n 65536^n$,
**  respectivly the negative of this if the type of this object is T_INTNEG'.
**
**  Each digit is  of  course  stored  as  a  16  bit  wide  unsigned  short.
**  Note that base 65536 allows us to multiply 2 digits and add a carry digit
**  without overflow in 32 bit long arithmetic, available on most processors.
**
**  The number of digits in every  large  integer  is  a  multiple  of  four.
**  Therefor the leading three digits of some values will actually  be  zero.
**  Note that the uniqueness of representation implies that not four or  more
**  leading digits may be zero, since |d0|d1|d2|d3| and |d0|d1|d2|d3|0|0|0|0|
**  have the same value only one, the first, can be a  legal  representation.
**
**  Because of this it is possible to do a  little  bit  of  loop  unrolling.
**  Thus instead of looping <n> times, handling one digit in each  iteration,
**  we can loop <n>/4 times, handling  four  digits  during  each  iteration.
**  This reduces the overhead of the loop by a factor of  approximatly  four.
**
**  Using base 65536 representation has advantages over  using  other  bases.
**  Integers in base 65536 representation can be packed  dense  and  therefor
**  use roughly 20\% less space than integers in base  10000  representation.
**  'SumInt' is 20\% and 'ProdInt' is 40\% faster for 65536 than  for  10000,
**  as their runtime is linear respectivly quadratic in the number of digits.
**  Dividing by 65536 and computing the remainder mod 65536 can be done  fast
**  by shifting 16 bit to  the  right  and  by  taking  the  lower  16  bits.
**  Larger bases are difficult because the product of two digits will not fit
**  into 32 bit, which is the word size  of  most  modern  micro  processors.
**  Base 10000 would have the advantage that printing is  very  much  easier,
**  but 'PrInt' keeps a terminal at 9600 baud busy for almost  all  integers.
*/
char *          Revision_integer_c =
   "@(#)$Id$";

#include        "system.h"              /* Ints, UInts                     */
#include        "scanner.h"             /* Pr                              */
#include        "gasman.h"              /* NewBag, CHANGED_BAG             */

#include        "objects.h"             /* Obj, TYPE_OBJ, types            */
#include        "gvars.h"               /* AssGVar, GVarName               */

#include        "calls.h"               /* NewFunctionC                    */
#include        "opers.h"               /* NewFilterC                      */

#include        "ariths.h"              /* generic operations package      */

#include        "bool.h"                /* True, False                     */

#define INCLUDE_DECLARATION_PART
#include        "integer.h"             /* declaration part of the package */
#undef  INCLUDE_DECLARATION_PART

#include        "gap.h"                 /* Error                           */


/****************************************************************************
**
*T  TypDigit  . . . . . . . . . . . . . . . . . . . .  type of a single digit
**
**  'TypDigit' is the type of a single digit of an  arbitrary  size  integer.
**  This is of course unsigned short int, which gives us the 16 bits we want.
**
**  'TypDigit' is defined in the declaration file of the package as follows:
**
typedef UInt2           TypDigit;
*/

#define SIZE_INT(op)    (SIZE_OBJ(op) / sizeof(TypDigit))
#define ADDR_INT(op)    ((TypDigit*)ADDR_OBJ(op))


/****************************************************************************
**
*F  KindInt(<int>)  . . . . . . . . . . . . . . . . . . . . . kind of integer
**
**  'KindInt' returns the kind of the integer <int>.
**
**  'KindInt' is the function in 'KindObjFuncs' for integers.
*/
Obj             KIND_INT_SMALL_ZERO;
Obj             KIND_INT_SMALL_POS;
Obj             KIND_INT_SMALL_NEG;
Obj             KIND_INT_LARGE_POS;
Obj             KIND_INT_LARGE_NEG;

Obj             KindIntSmall (
    Obj                 val )
{
    if ( 0 == INT_INTOBJ(val) ) {
        return KIND_INT_SMALL_ZERO;
    }
    else if ( 0 < INT_INTOBJ(val) ) {
        return KIND_INT_SMALL_POS;
    }
    else /* if ( 0 > INT_INTOBJ(val) ) */ {
        return KIND_INT_SMALL_NEG;
    }
}

Obj             KindIntLargePos (
    Obj                 val )
{
    return KIND_INT_LARGE_POS;
}

Obj             KindIntLargeNeg (
    Obj                 val )
{
    return KIND_INT_LARGE_NEG;
}


/****************************************************************************
**
*F  PrintInt( <int> ) . . . . . . . . . . . . . . . print an integer constant
**
**  'PrintInt'  prints  the integer  <int>   in the  usual  decimal notation.
**  'PrintInt' handles objects of type 'T_INT', 'T_INTPOS' and 'T_INTNEG'.
**
**  Large integers are first converted into  base  10000  and  then  printed.
**  The time for a conversion depends quadratically on the number of  digits.
**  For 2000 decimal digit integers, a screenfull,  it  is  reasonable  fast.
*/

TypDigit        PrIntC [1000];          /* copy of integer to be printed   */
TypDigit        PrIntD [1205];          /* integer converted to base 10000 */

void            PrintInt (
    Obj                 op )
{
    Int                 i, k;           /* loop counter                    */
    TypDigit *          p;              /* loop pointer                    */
    UInt                c;              /* carry in division step          */

    /* print a small integer                                               */
    if ( IS_INTOBJ(op) ) {
        Pr( "%>%d%<", INT_INTOBJ(op), 0L );
    }

    /* print a large integer                                               */
    else if ( SIZE_INT(op) < 1000 ) {

        /* start printing, %> means insert '\' before a linebreak          */
        Pr("%>",0L,0L);

        if ( TYPE_OBJ(op) == T_INTNEG )
            Pr("-",0L,0L);

        /* convert the integer into base 10000                             */
        i = 0;
        for ( k = 0; k < SIZE_INT(op); k++ )
            PrIntC[k] = ADDR_INT(op)[k];
        while ( k > 0 && PrIntC[k-1] == 0 )  k--;
        while ( k > 0 ) {
            for ( c = 0, p = PrIntC+k-1; p >= PrIntC; p-- ) {
                c  = (c<<16) + *p;
                *p = c / 10000;
                c  = c - 10000 * *p;
            }
            PrIntD[i++] = c;
            while ( k > 0 && PrIntC[k-1] == 0 )  k--;
        }

        /* print the base 10000 digits                                     */
        Pr( "%d", (Int)PrIntD[--i], 0L );
        while ( i > 0 )
            Pr( "%04d", (Int)PrIntD[--i], 0L );

        Pr("%<",0L,0L);

    }

    else {
        Pr("<<an integer too large to be printed>>",0L,0L);
    }
}


/****************************************************************************
**
*F  EqInt( <intL>, <intR> ) . . . . . . . . .  test if two integers are equal
**
**  'EqInt' returns 1  if  the two integer   arguments <intL> and  <intR> are
**  equal and 0 otherwise.
*/
Int             EqInt ( 
    Obj                 opL,
    Obj                 opR )
{
    Int                 k;              /* loop counter                    */
    TypDigit *          l;              /* pointer into the left operand   */
    TypDigit *          r;              /* pointer into the right operand  */

    /* compare two small integers                                          */
    if ( ARE_INTOBJS( opL, opR ) ) {
        if ( INT_INTOBJ(opL) == INT_INTOBJ(opR) )  return 1L;
        else                                       return 0L;
    }

    /* compare a small and a large integer                                 */
    else if ( IS_INTOBJ(opL) ) {
        return 0L;
    }
    else if ( IS_INTOBJ(opR) ) {
        return 0L;
    }

    /* compare two large integers                                          */
    else {

        /* compare the sign and size                                       */
        if ( TYPE_OBJ(opL) != TYPE_OBJ(opR)
          || SIZE_INT(opL) != SIZE_INT(opR) )
            return 0L;

        /* set up the pointers                                             */
        l = ADDR_INT(opL);
        r = ADDR_INT(opR);

        /* run through the digits, four at a time                          */
        for ( k = SIZE_INT(opL)/4-1; k >= 0; k-- ) {
            if ( *l++ != *r++ )  return 0L;
            if ( *l++ != *r++ )  return 0L;
            if ( *l++ != *r++ )  return 0L;
            if ( *l++ != *r++ )  return 0L;
        }

        /* no differences found, so they must be equal                     */
        return 1L;

    }
}


/****************************************************************************
**
*F  LtInt( <intL>, <intR> ) . . . . . test if an integer is less than another
**
**  'LtInt' returns 1 if the integer <intL> is strictly less than the integer
**  <intR> and 0 otherwise.
*/
Int             LtInt (
    Obj                 opL,
    Obj                 opR )
{
    Int                 k;              /* loop counter                    */
    TypDigit *          l;              /* pointer into the left operand   */
    TypDigit *          r;              /* pointer into the right operand  */

    /* compare two small integers                                          */
    if ( ARE_INTOBJS( opL, opR ) ) {
        if ( INT_INTOBJ(opL) <  INT_INTOBJ(opR) )  return 1L;
        else                                       return 0L;
    }

    /* compare a small and a large integer                                 */
    else if ( IS_INTOBJ(opL) ) {
        if ( TYPE_OBJ(opR) == T_INTPOS )  return 1L;
        else                              return 0L;
    }
    else if ( IS_INTOBJ(opR) ) {
        if ( TYPE_OBJ(opL) == T_INTPOS )  return 0L;
        else                              return 1L;
    }

    /* compare two large integers                                          */
    else {

        /* compare the sign and size                                       */
        if (      TYPE_OBJ(opL) == T_INTNEG
               && TYPE_OBJ(opR) == T_INTPOS )
            return 1L;
        else if ( TYPE_OBJ(opL) == T_INTPOS
               && TYPE_OBJ(opR) == T_INTNEG )
            return 0L;
        else if ( (TYPE_OBJ(opL) == T_INTPOS
                && SIZE_INT(opL) < SIZE_INT(opR))
               || (TYPE_OBJ(opL) == T_INTNEG
                && SIZE_INT(opL) > SIZE_INT(opR)) )
            return 1L;
        else if ( (TYPE_OBJ(opL) == T_INTPOS
                && SIZE_INT(opL) > SIZE_INT(opR))
               || (TYPE_OBJ(opL) == T_INTNEG
                && SIZE_INT(opL) < SIZE_INT(opR)) )
            return 0L;

        /* set up the pointers                                             */
        l = ADDR_INT(opL);
        r = ADDR_INT(opR);

        /* run through the digits, from the end downwards                  */
        for ( k = SIZE_INT(opL)-1; k >= 0; k-- ) {
            if ( l[k] != r[k] ) {
                if ( (TYPE_OBJ(opL) == T_INTPOS
                   && l[k] < r[k])
                  || (TYPE_OBJ(opL) == T_INTNEG
                   && l[k] > r[k]) )
                    return 1L;
                else
                    return 0L;
            }
        }

        /* no differences found, so they must be equal                     */
        return 0L;

    }
}


/****************************************************************************
**
*F  SumInt( <intL>, <intR> )  . . . . . . . . . . . . . . sum of two integers
**
**  'SumInt' returns the sum of the two integer arguments <intL> and  <intR>.
**  'SumInt' handles operands of type 'T_INT', 'T_INTPOS' and 'T_INTNEG'.
**
**  It can also be used in the cases that both operands  are  small  integers
**  and the result is a small integer too,  i.e., that  no  overflow  occurs.
**  This case is usually already handled in 'EvSum' for a better  efficiency.
**
**  Is called from the 'EvSum'  binop so both operands are already evaluated.
**
**  'SumInt' is a little bit difficult since there are 16  different cases to
**  handle, each operand can be positive or negative, small or large integer.
**  If the operands have opposite sign 'SumInt' calls 'DiffInt',  this  helps
**  reduce the total amount of code by a factor of two.
*/
Obj             SumInt (
    Obj                 opL,
    Obj                 opR )
{
    Int                 i;              /* loop variable                   */
    Int                 k;              /* loop variable                   */
    Int                 c;              /* sum of two digits               */
    TypDigit *          l;              /* pointer into the left operand   */
    TypDigit *          r;              /* pointer into the right operand  */
    TypDigit *          s;              /* pointer into the sum            */
    UInt *              l2;             /* pointer to get 2 digits at once */
    UInt *              s2;             /* pointer to put 2 digits at once */
    Obj                 sum;            /* handle of the result bag        */

    /* adding two small integers                                           */
    if ( ARE_INTOBJS( opL, opR ) ) {

        /* add two small integers with a small sum                         */
        /* add and compare top two bits to check that no overflow occured  */
        if ( SUM_INTOBJS( sum, opL, opR ) ) {
            return sum;
        }

        /* add two small integers with a large sum                         */
        c = INT_INTOBJ(opL) + INT_INTOBJ(opR);
        if ( 0 < c ) {
            sum = NewBag( T_INTPOS, 4*sizeof(TypDigit) );
            ADDR_INT(sum)[0] = c;
            ADDR_INT(sum)[1] = c >> 16;
        }
        else {
            sum = NewBag( T_INTNEG, 4*sizeof(TypDigit) );
            ADDR_INT(sum)[0] = (-c);
            ADDR_INT(sum)[1] = (-c) >> 16;
        }

    }

    /* adding one large integer and one small integer                      */
    else if ( IS_INTOBJ(opL) || IS_INTOBJ(opR) ) {

        /* make the right operand the small one                            */
        if ( IS_INTOBJ(opL) ) {
            sum = opL;  opL = opR;  opR = sum;
        }

        /* if the integers have different sign, let 'DiffInt' do the work  */
        if ( (TYPE_OBJ(opL) == T_INTNEG && 0 <= INT_INTOBJ(opR))
          || (TYPE_OBJ(opL) == T_INTPOS && INT_INTOBJ(opR) <  0) ) {
            if ( TYPE_OBJ(opL) == T_INTPOS )  RetypeBag( opL, T_INTNEG );
            else                              RetypeBag( opL, T_INTPOS );
            sum = DiffInt( opR, opL );
            if ( TYPE_OBJ(opL) == T_INTPOS )  RetypeBag( opL, T_INTNEG );
            else                              RetypeBag( opL, T_INTPOS );
            return sum;
        }

        /* allocate the result bag and set up the pointers                 */
        if ( TYPE_OBJ(opL) == T_INTPOS ) {
            i   = INT_INTOBJ(opR);
            sum = NewBag( T_INTPOS, (SIZE_INT(opL)+4)*sizeof(TypDigit) );
        }
        else {
            i   = -INT_INTOBJ(opR);
            sum = NewBag( T_INTNEG, (SIZE_INT(opL)+4)*sizeof(TypDigit) );
        }
        l = ADDR_INT(opL);
        s = ADDR_INT(sum);

        /* add the first four digit, note the left operand has only two    */
        c = *l++ + (TypDigit)i;                  *s++ = c;
        c = *l++ + (TypDigit)(i>>16) + (c>>16);  *s++ = c;
        c = *l++                     + (c>>16);  *s++ = c;
        c = *l++                     + (c>>16);  *s++ = c;

        /* propagate the carry, this loop is almost never executed         */
        for ( k = SIZE_INT(opL)/4-1; k != 0 && (c>>16) != 0; k-- ) {
            c = *l++ + (c>>16);  *s++ = c;
            c = *l++ + (c>>16);  *s++ = c;
            c = *l++ + (c>>16);  *s++ = c;
            c = *l++ + (c>>16);  *s++ = c;
        }

        /* just copy the remaining digits, do it two digits at once        */
        for ( l2 = (UInt*)l, s2 = (UInt*)s; k != 0; k-- ) {
            *s2++ = *l2++;
            *s2++ = *l2++;
        }

        /* if there is a carry, enter it, otherwise shrink the sum         */
        if ( (c>>16) != 0 )
            *s++ = (c>>16);
        else
            ResizeBag( sum, (SIZE_INT(sum)-4)*sizeof(TypDigit) );

    }

    /* add two large integers                                              */
    else {

        /* if the integers have different sign, let 'DiffInt' do the work  */
        if ( (TYPE_OBJ(opL) == T_INTPOS && TYPE_OBJ(opR) == T_INTNEG)
          || (TYPE_OBJ(opL) == T_INTNEG && TYPE_OBJ(opR) == T_INTPOS) ) {
            if ( TYPE_OBJ(opL) == T_INTPOS )  RetypeBag( opL, T_INTNEG );
            else                              RetypeBag( opL, T_INTPOS );
            sum = DiffInt( opR, opL );
            if ( TYPE_OBJ(opL) == T_INTPOS )  RetypeBag( opL, T_INTNEG );
            else                              RetypeBag( opL, T_INTPOS );
            return sum;
        }

        /* make the right operand the smaller one                          */
        if ( SIZE_INT(opL) < SIZE_INT(opR) ) {
            sum = opL;  opL = opR;  opR = sum;
        }

        /* allocate the result bag and set up the pointers                 */
        if ( TYPE_OBJ(opL) == T_INTPOS ) {
            sum = NewBag( T_INTPOS, (SIZE_INT(opL)+4)*sizeof(TypDigit) );
        }
        else {
            sum = NewBag( T_INTNEG, (SIZE_INT(opL)+4)*sizeof(TypDigit) );
        }
        l = ADDR_INT(opL);
        r = ADDR_INT(opR);
        s = ADDR_INT(sum);

        /* add the digits                                                  */
        c = 0;
        for ( k = SIZE_INT(opR)/4; k != 0; k-- ) {
            c = *l++ + *r++ + (c>>16);  *s++ = c;
            c = *l++ + *r++ + (c>>16);  *s++ = c;
            c = *l++ + *r++ + (c>>16);  *s++ = c;
            c = *l++ + *r++ + (c>>16);  *s++ = c;
        }

        /* propagate the carry, this loop is almost never executed         */
        for ( k=(SIZE_INT(opL)-SIZE_INT(opR))/4; k!=0 && (c>>16)!=0; k-- ) {
            c = *l++ + (c>>16);  *s++ = c;
            c = *l++ + (c>>16);  *s++ = c;
            c = *l++ + (c>>16);  *s++ = c;
            c = *l++ + (c>>16);  *s++ = c;
        }

        /* just copy the remaining digits, do it two digits at once        */
        for ( l2 = (UInt*)l, s2 = (UInt*)s; k != 0; k-- ) {
            *s2++ = *l2++;
            *s2++ = *l2++;
        }

        /* if there is a carry, enter it, otherwise shrink the sum         */
        if ( (c>>16) != 0 )
            *s++ = (c>>16);
        else
            ResizeBag( sum, (SIZE_INT(sum)-4)*sizeof(TypDigit) );

    }

    /* return the sum                                                      */
    return sum;
}


/****************************************************************************
**
*F  ZeroInt(<int>)  . . . . . . . . . . . . . . . . . . . .  zero of integers
*/
Obj         ZeroInt (
    Obj                 op )
{
    return INTOBJ_INT(0);
}


/****************************************************************************
**
*F  AInvInt(<int>)  . . . . . . . . . . . . .  additive inverse of an integer
*/
Obj         AInvInt (
    Obj                 op )
{
    Obj                 inv;
    UInt                i;

    /* handle small integer                                                */
    if ( IS_INTOBJ( op ) ) {

        /* special case (ugh)                                              */
        if ( op == INTOBJ_INT( -(1L<<28) ) ) {
            inv = NewBag( T_INTPOS, 4*sizeof(TypDigit) );
            ADDR_INT(inv)[0] = 0;
            ADDR_INT(inv)[1] = (1L<<12);
        }

        /* general case                                                    */
        else {
            inv = INTOBJ_INT( - INT_INTOBJ( op ) );
        }

    }

    /* invert a large integer                                              */
    else {

        /* special case (ugh)                                              */
        if ( TYPE_OBJ(op) == T_INTPOS && SIZE_INT(op) == 4
          && ADDR_INT(op)[3] == 0 && ADDR_INT(op)[2] == 0
          && 65536*ADDR_INT(op)[1] + ADDR_INT(op)[0] == (1L<<28) ) {
            inv = INTOBJ_INT( -(1L<<28) );
        }

        /* general case                                                    */
        else {
            if ( TYPE_OBJ(op) == T_INTPOS ) {
                inv = NewBag( T_INTNEG, SIZE_OBJ(op) );
            }
            else {
                inv = NewBag( T_INTPOS, SIZE_OBJ(op) );
            }
            for ( i = 0; i < SIZE_INT(op); i++ ) {
                ADDR_INT(inv)[i] = ADDR_INT(op)[i];
            }
        }

    }

    /* return the inverse                                                  */
    return inv;
}


/****************************************************************************
**
*F  DiffInt( <intL>, <intR> ) . . . . . . . . . .  difference of two integers
**
**  'DiffInt' returns the difference of the two integer arguments <intL>  and
**  <intR>.  'DiffInt' handles  operands  of  type  'T_INT',  'T_INTPOS'  and
**  'T_INTNEG'.
**
**  It can also be used in the cases that both operands  are  small  integers
**  and the result is a small integer too,  i.e., that  no  overflow  occurs.
**  This case is usually already handled in 'EvDiff' for a better efficiency.
**
**  Is called from the 'EvDiff' binop so both operands are already evaluated.
**
**  'DiffInt' is a little bit difficult since there are 16 different cases to
**  handle, each operand can be positive or negative, small or large integer.
**  If the operands have opposite sign 'DiffInt' calls 'SumInt',  this  helps
**  reduce the total amount of code by a factor of two.
*/
Obj             DiffInt (
    Obj                 opL,
    Obj                 opR )
{
    Int                 i;              /* loop variable                   */
    Int                 k;              /* loop variable                   */
    Int                 c;              /* difference of two digits        */
    TypDigit *          l;              /* pointer into the left operand   */
    TypDigit *          r;              /* pointer into the right operand  */
    TypDigit *          d;              /* pointer into the difference     */
    UInt *              l2;             /* pointer to get 2 digits at once */
    UInt *              d2;             /* pointer to put 2 digits at once */
    Obj                 dif;            /* handle of the result bag        */

    /* subtracting two small integers                                      */
    if ( ARE_INTOBJS( opL, opR ) ) {

        /* subtract two small integers with a small difference             */
        /* sub and compare top two bits to check that no overflow occured  */
        if ( DIFF_INTOBJS( dif, opL, opR ) ) {
            return dif;
        }

        /* subtract two small integers with a large difference             */
        c = INT_INTOBJ(opL) - INT_INTOBJ(opR);
        if ( 0 < c ) {
            dif = NewBag( T_INTPOS, 4*sizeof(TypDigit) );
            ADDR_INT(dif)[0] = c;
            ADDR_INT(dif)[1] = c >> 16;
        }
        else {
            dif = NewBag( T_INTNEG, 4*sizeof(TypDigit) );
            ADDR_INT(dif)[0] = (-c);
            ADDR_INT(dif)[1] = (-c) >> 16;
        }

    }

    /* subtracting one small integer and one large integer                 */
    else if ( IS_INTOBJ( opL ) || IS_INTOBJ( opR ) ) {

        /* make the right operand the small one                            */
        if ( IS_INTOBJ( opL ) ) {
            dif = opL;  opL = opR;  opR = dif;
            c = -1;
        }
        else {
            c =  1;
        }

        /* if the integers have different sign, let 'SumInt' do the work   */
        if ( (TYPE_OBJ(opL) == T_INTNEG && 0 <= INT_INTOBJ(opR))
          || (TYPE_OBJ(opL) == T_INTPOS && INT_INTOBJ(opR) < 0)  ) {
            if ( TYPE_OBJ(opL) == T_INTPOS )  RetypeBag( opL, T_INTNEG );
            else                              RetypeBag( opL, T_INTPOS );
            dif = SumInt( opL, opR );
            if ( TYPE_OBJ(opL) == T_INTPOS )  RetypeBag( opL, T_INTNEG );
            else                              RetypeBag( opL, T_INTPOS );
            if ( c == 1 ) {
                if ( TYPE_OBJ(dif) == T_INTPOS )  RetypeBag( dif, T_INTNEG );
                else                              RetypeBag( dif, T_INTPOS );
            }
            return dif;
        }

        /* allocate the result bag and set up the pointers                 */
        if ( TYPE_OBJ(opL) == T_INTPOS ) {
            i   = INT_INTOBJ(opR);
            if ( c == 1 )  dif = NewBag( T_INTPOS, SIZE_OBJ(opL) );
            else           dif = NewBag( T_INTNEG, SIZE_OBJ(opL) );
        }
        else {
            i   = - INT_INTOBJ(opR);
            if ( c == 1 )  dif = NewBag( T_INTNEG, SIZE_OBJ(opL) );
            else           dif = NewBag( T_INTPOS, SIZE_OBJ(opL) );
        }
        l = ADDR_INT(opL);
        d = ADDR_INT(dif);

        /* sub the first four digit, note the left operand has only two    */
        /*N (c>>16) need not work, replace by (c<0?-1:0)                   */
        c = *l++ - (TypDigit)i;                  *d++ = c;
        c = *l++ - (TypDigit)(i>>16) + (c>>16);  *d++ = c;
        c = *l++                     + (c>>16);  *d++ = c;
        c = *l++                     + (c>>16);  *d++ = c;

        /* propagate the carry, this loop is almost never executed         */
        for ( k = SIZE_INT(opL)/4-1; k != 0 && (c>>16) != 0; k-- ) {
            c = *l++ + (c>>16);  *d++ = c;
            c = *l++ + (c>>16);  *d++ = c;
            c = *l++ + (c>>16);  *d++ = c;
            c = *l++ + (c>>16);  *d++ = c;
        }

        /* just copy the remaining digits, do it two digits at once        */
        for ( l2 = (UInt*)l, d2 = (UInt*)d; k != 0; k-- ) {
            *d2++ = *l2++;
            *d2++ = *l2++;
        }

        /* no underflow since we subtracted a small int from a large one   */
        /* but there may be leading zeroes in the result, get rid of them  */
        /* occurs almost never, so it doesn't matter that it is expensive  */
        if ( ((UInt*)d == d2
          && d[-4] == 0 && d[-3] == 0 && d[-2] == 0 && d[-1] == 0)
          || (SIZE_INT(dif) == 4 && d[-2] == 0 && d[-1] == 0) ) {

            /* find the number of significant digits                       */
            d = ADDR_INT(dif);
            for ( k = SIZE_INT(dif); k != 0; k-- ) {
                if ( d[k-1] != 0 )
                    break;
            }

            /* reduce to small integer if possible, otherwise shrink bag   */
            if ( k <= 2 && TYPE_OBJ(dif) == T_INTPOS
              && (UInt)(65536*d[1]+d[0]) < (1<<28) )
                dif = INTOBJ_INT( 65536*d[1]+d[0] );
            else if ( k <= 2 && TYPE_OBJ(dif) == T_INTNEG
              && (UInt)(65536*d[1]+d[0]) <= (1<<28) )
                dif = INTOBJ_INT( -(65536*d[1]+d[0]) );
            else
                ResizeBag( dif, (((k + 3) / 4) * 4) * sizeof(TypDigit) );
        }

    }

    /* subtracting two large integers                                      */
    else {

        /* if the integers have different sign, let 'SumInt' do the work   */
        if ( (TYPE_OBJ(opL) == T_INTPOS && TYPE_OBJ(opR) == T_INTNEG)
          || (TYPE_OBJ(opL) == T_INTNEG && TYPE_OBJ(opR) == T_INTPOS) ) {
            if ( TYPE_OBJ(opR) == T_INTPOS )  RetypeBag( opR, T_INTNEG );
            else                              RetypeBag( opR, T_INTPOS );
            dif = SumInt( opL, opR );
            if ( TYPE_OBJ(opR) == T_INTPOS )  RetypeBag( opR, T_INTNEG );
            else                              RetypeBag( opR, T_INTPOS );
            return dif;
        }

        /* make the right operand the smaller one                          */
        if ( SIZE_INT(opL) <  SIZE_INT(opR)
          || (TYPE_OBJ(opL) == T_INTPOS && LtInt(opL,opR) )
          || (TYPE_OBJ(opL) == T_INTNEG && LtInt(opR,opL) ) ) {
            dif = opL;  opL = opR;  opR = dif;  c = -1;
        }
        else {
            c = 1;
        }

        /* allocate the result bag and set up the pointers                 */
        if ( (TYPE_OBJ(opL) == T_INTPOS && c ==  1)
          || (TYPE_OBJ(opL) == T_INTNEG && c == -1) )
            dif = NewBag( T_INTPOS, SIZE_OBJ(opL) );
        else
            dif = NewBag( T_INTNEG, SIZE_OBJ(opL) );
        l = ADDR_INT(opL);
        r = ADDR_INT(opR);
        d = ADDR_INT(dif);

        /* subtract the digits                                             */
        c = 0;
        for ( k = SIZE_INT(opR)/4; k != 0; k-- ) {
            c = *l++ - *r++ + (c>>16);  *d++ = c;
            c = *l++ - *r++ + (c>>16);  *d++ = c;
            c = *l++ - *r++ + (c>>16);  *d++ = c;
            c = *l++ - *r++ + (c>>16);  *d++ = c;
        }

        /* propagate the carry, this loop is almost never executed         */
        for ( k=(SIZE_INT(opL)-SIZE_INT(opR))/4; k!=0 && (c>>16)!=0; k-- ) {
            c = *l++ + (c>>16);  *d++ = c;
            c = *l++ + (c>>16);  *d++ = c;
            c = *l++ + (c>>16);  *d++ = c;
            c = *l++ + (c>>16);  *d++ = c;
        }

        /* just copy the remaining digits, do it two digits at once        */
        for ( d2 = (UInt*)d, l2 = (UInt*)l; k != 0; k-- ) {
            *d2++ = *l2++;
            *d2++ = *l2++;
        }

        /* no underflow since we subtracted a small int from a large one   */
        /* but there may be leading zeroes in the result, get rid of them  */
        /* occurs almost never, so it doesn't matter that it is expensive  */
        if ( ((UInt*)d == d2
          && d[-4] == 0 && d[-3] == 0 && d[-2] == 0 && d[-1] == 0)
          || (SIZE_INT(dif) == 4 && d[-2] == 0 && d[-1] == 0) ) {

            /* find the number of significant digits                       */
            d = ADDR_INT(dif);
            for ( k = SIZE_INT(dif); k != 0; k-- ) {
                if ( d[k-1] != 0 )
                    break;
            }

            /* reduce to small integer if possible, otherwise shrink bag   */
            if ( k <= 2 && TYPE_OBJ(dif) == T_INTPOS
              && (UInt)(65536*d[1]+d[0]) < (1<<28) )
                dif = INTOBJ_INT( 65536*d[1]+d[0] );
            else if ( k <= 2 && TYPE_OBJ(dif) == T_INTNEG
              && (UInt)(65536*d[1]+d[0]) <= (1<<28) )
                dif = INTOBJ_INT( -(65536*d[1]+d[0]) );
            else
                ResizeBag( dif, (((k + 3) / 4) * 4) * sizeof(TypDigit) );

        }

    }

    /* return the difference                                               */
    return dif;
}


/****************************************************************************
**
*F  ProdInt( <intL>, <intR> ) . . . . . . . . . . . . product of two integers
**
**  'ProdInt' returns the product of the two  integer  arguments  <intL>  and
**  <intR>.  'ProdInt' handles  operands  of  type  'T_INT',  'T_INTPOS'  and
**  'T_INTNEG'.
**
**  It can also be used in the cases that both operands  are  small  integers
**  and the result is a small integer too,  i.e., that  no  overflow  occurs.
**  This case is usually already handled in 'EvProd' for a better efficiency.
**
**  Is called from the 'EvProd' binop so both operands are already evaluated.
**
**  The only difficult about this function is the fact that is has two handle
**  3 different situation, depending on how many arguments  are  small  ints.
*/
Obj             ProdInt (
    Obj                 opL,
    Obj                 opR )
{
    Int                 i;              /* loop count, value for small int */
    Int                 k;              /* loop count, value for small int */
    UInt                c;              /* product of two digits           */
    TypDigit            l;              /* one digit of the left operand   */
    TypDigit *          r;              /* pointer into the right operand  */
    TypDigit *          p;              /* pointer into the product        */
    Obj                 prd;            /* handle of the result bag        */

    /* multiplying two small integers                                      */
    if ( ARE_INTOBJS( opL, opR ) ) {

        /* multiply two small integers with a small product                */
        /* multiply and divide back to check that no overflow occured      */
        if ( PROD_INTOBJS( prd, opL, opR ) ) {
            return prd;
        }

        /* get the integer values                                          */
        i = INT_INTOBJ(opL);
        k = INT_INTOBJ(opR);

        /* allocate the product bag                                        */
        if ( (0 < i && 0 < k) || (i < 0 && k < 0) )
            prd = NewBag( T_INTPOS, 4*sizeof(TypDigit) );
        else
            prd = NewBag( T_INTNEG, 4*sizeof(TypDigit) );
        p = ADDR_INT(prd);

        /* make both operands positive                                     */
        if ( i < 0 )  i = -i;
        if ( k < 0 )  k = -k;

        /* multiply digitwise                                              */
        c = (TypDigit)i       * (TypDigit)k;                        p[0] = c;
        c = (TypDigit)i       * (TypDigit)(k>>16)        + (c>>16); p[1] = c;
        p[2] = c>>16;
        c = (TypDigit)(i>>16) * (TypDigit)k       + p[1];           p[1] = c;
        c = (TypDigit)(i>>16) * (TypDigit)(k>>16) + p[2] + (c>>16); p[2] = c;
        p[3] = c>>16;

    }

    /* multiply a small and a large integer                                */
    else if ( IS_INTOBJ(opL) || IS_INTOBJ(opR) ) {

        /* make the left operand the small one                             */
        if ( IS_INTOBJ(opR) ) {
            i = INT_INTOBJ(opR);  opR = opL;
        }
        else {
            i = INT_INTOBJ(opL);
        }

        /* handle trivial cases first                                      */
        if ( i == 0 )
            return INTOBJ_INT(0);
        if ( i == 1 )
            return opR;

        /* the large integer 1<<28 times -1 is the small integer -(1<<28)  */
        if ( i == -1
          && TYPE_OBJ(opR) == T_INTPOS && SIZE_INT(opR) == 4
          && ADDR_INT(opR)[3] == 0     && ADDR_INT(opR)[2] == 0
          && 65536*ADDR_INT(opR)[1] + ADDR_INT(opR)[0] == (1<<28) )
            return INTOBJ_INT( -(1<<28) );

        /* multiplication by -1 is easy, just switch the sign and copy     */
        if ( i == -1 ) {
            if ( TYPE_OBJ(opR) == T_INTPOS )
                prd = NewBag( T_INTNEG, SIZE_OBJ(opR) );
            else
                prd = NewBag( T_INTPOS, SIZE_OBJ(opR) );
            r = ADDR_INT(opR);
            p = ADDR_INT(prd);
            for ( k = SIZE_INT(opR)/4; k != 0; k-- ) {
                /*N should be: *p2++=*r2++;  *p2++=*r2++;                  */
                *p++ = *r++;  *p++ = *r++;  *p++ = *r++;  *p++ = *r++;
            }
            return prd;
        }

        /* allocate a bag for the result                                   */
        if ( (0 < i && TYPE_OBJ(opR) == T_INTPOS)
          || (i < 0 && TYPE_OBJ(opR) == T_INTNEG) )
            prd = NewBag( T_INTPOS, (SIZE_INT(opR)+4)*sizeof(TypDigit) );
        else
            prd = NewBag( T_INTNEG, (SIZE_INT(opR)+4)*sizeof(TypDigit) );
        if ( i < 0 )  i = -i;

        /* multiply with the lower digit of the left operand               */
        l = (TypDigit)i;
        if ( l != 0 ) {

            r = ADDR_INT(opR);
            p = ADDR_INT(prd);
            c = 0;

            /* multiply the right with this digit and store in the product */
            for ( k = SIZE_INT(opR)/4; k != 0; k-- ) {
                c = l * *r++ + (c>>16);  *p++ = c;
                c = l * *r++ + (c>>16);  *p++ = c;
                c = l * *r++ + (c>>16);  *p++ = c;
                c = l * *r++ + (c>>16);  *p++ = c;
            }
            *p = (c>>16);
        }

        /* multiply with the larger digit of the left operand              */
        l = i >> 16;
        if ( l != 0 ) {

            r = ADDR_INT(opR);
            p = ADDR_INT(prd) + 1;
            c = 0;

            /* multiply the right with this digit and add into the product */
            for ( k = SIZE_INT(opR)/4; k != 0; k-- ) {
                c = l * *r++ + *p + (c>>16);  *p++ = c;
                c = l * *r++ + *p + (c>>16);  *p++ = c;
                c = l * *r++ + *p + (c>>16);  *p++ = c;
                c = l * *r++ + *p + (c>>16);  *p++ = c;
            }
            *p = (c>>16);
        }

        /* remove the leading zeroes, note that there can't be more than 6 */
        p = ADDR_INT(prd) + SIZE_INT(prd);
        if ( p[-4] == 0 && p[-3] == 0 && p[-2] == 0 && p[-1] == 0 ) {
            ResizeBag( prd, (SIZE_INT(prd)-4)*sizeof(TypDigit) );
        }

    }

    /* multiply two large integers                                         */
    else {

        /* make the left operand the smaller one, for performance          */
        if ( SIZE_INT(opL) > SIZE_INT(opR) ) {
            prd = opR;  opR = opL;  opL = prd;
        }

        /* allocate a bag for the result                                   */
        if ( TYPE_OBJ(opL) == TYPE_OBJ(opR) )
            prd = NewBag( T_INTPOS, SIZE_OBJ(opL)+SIZE_OBJ(opR) );
        else
            prd = NewBag( T_INTNEG, SIZE_OBJ(opL)+SIZE_OBJ(opR) );

        /* run through the digits of the left operand                      */
        for ( i = 0; i < SIZE_INT(opL); i++ ) {

            /* set up pointer for one loop iteration                       */
            l = ADDR_INT(opL)[i];
            if ( l == 0 )  continue;
            r = ADDR_INT(opR);
            p = ADDR_INT(prd) + i;
            c = 0;

            /* multiply the right with this digit and add into the product */
            for ( k = SIZE_INT(opR)/4; k != 0; k-- ) {
                c = l * *r++ + *p + (c>>16);  *p++ = c;
                c = l * *r++ + *p + (c>>16);  *p++ = c;
                c = l * *r++ + *p + (c>>16);  *p++ = c;
                c = l * *r++ + *p + (c>>16);  *p++ = c;
            }
            *p = (c>>16);
        }

        /* remove the leading zeroes, note that there can't be more than 7 */
        p = ADDR_INT(prd) + SIZE_INT(prd);
        if ( p[-4] == 0 && p[-3] == 0 && p[-2] == 0 && p[-1] == 0 ) {
            ResizeBag( prd, (SIZE_INT(prd)-4)*sizeof(TypDigit) );
        }

    }

    /* return the product                                                  */
    return prd;
}


/****************************************************************************
**
*F  ProdIntObj(<n>,<op>)  . . . . . . . . product of an integer and an object
*/
Obj             ProdIntObj (
    Obj                 n,
    Obj                 op )
{
    Obj                 res = 0;        /* result                          */
    UInt                i, k, l;        /* loop variables                  */

    /* if the integer is zero, return the neutral element of the operand   */
    if      ( TYPE_OBJ(n) == T_INT && INT_INTOBJ(n) ==  0 ) {
        res = ZERO( op );
    }

    /* if the integer is one, return a copy of the operand                 */
    else if ( TYPE_OBJ(n) == T_INT && INT_INTOBJ(n) ==  1 ) {
        res = CopyObj( op, 0 );
    }

    /* if the integer is minus one, return the inverse of the operand      */
    else if ( TYPE_OBJ(n) == T_INT && INT_INTOBJ(n) == -1 ) {
        res = AINV( op );
    }

    /* if the integer is negative, invert the operand and the integer      */
    else if ( TYPE_OBJ(n) == T_INT && INT_INTOBJ(n) <  -1 ) {
        res = AINV( op );
        if ( res == Fail ) {
            return ErrorReturnObj(
                "Operations: <obj> must have an additive inverse",
                0L, 0L,
                "you can return an inverse for <obj>" );
        }
        res = PROD( AINV( n ), res );
    }

    /* if the integer is negative, invert the operand and the integer      */
    else if ( TYPE_OBJ(n) == T_INTNEG ) {
        res = AINV( op );
        if ( res == Fail ) {
            return ErrorReturnObj(
                "Operations: <obj> must have an additive inverse",
                0L, 0L,
                "you can return an inverse for <obj>" );
        }
        res = PROD( AINV( n ), res );
    }

    /* if the integer is small, compute the product by repeated doubling   */
    /* the loop invariant is <result> = <k>*<res> + <l>*<op>, <l> < <k>    */
    /* <res> = 0 means that <res> is the neutral element                   */
    else if ( TYPE_OBJ(n) == T_INT && INT_INTOBJ(n) >   1 ) {
        res = 0;
        k = 1L << 31;
        l = INT_INTOBJ(n);
        while ( 1 < k ) {
            res = (res == 0 ? res : SUM( res, res ));
            k = k / 2;
            if ( k <= l ) {
                res = (res == 0 ? op : SUM( res, op ));
                l = l - k;
            }
        }
    }

    /* if the integer is large, compute the product by repeated doubling   */
    else if ( TYPE_OBJ(n) == T_INTPOS ) {
        res = 0;
        for ( i = SIZE_OBJ(n)/sizeof(TypDigit); 0 < i; i-- ) {
            k = 1L << (8*sizeof(TypDigit));
            l = ((TypDigit*) ADDR_OBJ(n))[i-1];
            while ( 1 < k ) {
                res = (res == 0 ? res : SUM( res, res ));
                k = k / 2;
                if ( k <= l ) {
                    res = (res == 0 ? op : SUM( res, op ));
                    l = l - k;
                }
            }
        }
    }

    /* return the result                                                   */
    return res;
}

Obj             ProdIntObjFunc;

Obj             ProdIntObjHandler (
    Obj                 self,
    Obj                 opL,
    Obj                 opR )
{
    return ProdIntObj( opL, opR );
}


/****************************************************************************
**
*F  OneInt(<int>) . . . . . . . . . . . . . . . . . . . . . one of an integer
*/
Obj             OneInt (
    Obj                 op )
{
    return INTOBJ_INT( 1L );
}


/****************************************************************************
**
*F  PowInt( <intL>, <intR> )  . . . . . . . . . . . . . . power of an integer
**
**  'PowInt' returns the <intR>-th (an integer) power of the integer  <intL>.
**  'PowInt' is handles operands of type 'T_INT', 'T_INTPOS' and 'T_INTNEG'.
**
**  It can also be used in the cases that both operands  are  small  integers
**  and the result is a small integer too,  i.e., that  no  overflow  occurs.
**  This case is usually already handled in 'EvPow' for a better  efficiency.
**
**  Is called from the 'EvPow'  binop so both operands are already evaluated.
*/
Obj             PowInt (
    Obj                 opL,
    Obj                 opR )
{
    Int                 i;
    Obj                 pow;

    /* power with a large exponent                                         */
    if ( ! IS_INTOBJ(opR) ) {
        if ( opL == INTOBJ_INT(0) )
            pow = INTOBJ_INT(0);
        else if ( opL == INTOBJ_INT(1) )
            pow = INTOBJ_INT(1);
        else if ( opL == INTOBJ_INT(-1) && ADDR_INT(opR)[0] % 2 == 0 )
            pow = INTOBJ_INT(1);
        else if ( opL == INTOBJ_INT(-1) && ADDR_INT(opR)[0] % 2 != 0 )
            pow = INTOBJ_INT(-1);
        else {
            opR = ErrorReturnObj(
                "Integer operands: <exponent> is to large",
                0L, 0L,
                "you can return a smaller <exponent>" );
            return POW( opL, opR );
        }
    }

    /* power with a negative exponent                                      */
    else if ( INT_INTOBJ(opR) < 0 ) {
        if ( opL == INTOBJ_INT(0) ) {
            opL = ErrorReturnObj(
                "Integer operands: <base> must not be zero",
                0L, 0L,
                "you can return a nonzero <base>" );
            return POW( opL, opR );
        }
        else if ( opL == INTOBJ_INT(1) )
            pow = INTOBJ_INT(1);
        else if ( opL == INTOBJ_INT(-1) && INT_INTOBJ(opR) % 2 == 0 )
            pow = INTOBJ_INT(1);
        else if ( opL == INTOBJ_INT(-1) && INT_INTOBJ(opR) % 2 != 0 )
            pow = INTOBJ_INT(-1);
        else
            pow = QUO( INTOBJ_INT(1),
                       PowInt( opL, INTOBJ_INT( -INT_INTOBJ(opR)) ) );
    }

    /* power with a small positive exponent, do it by a repeated squaring  */
    else {
        pow = INTOBJ_INT(1);
        i = INT_INTOBJ(opR);
        while ( i != 0 ) {
            if ( i % 2 == 1 )  pow = ProdInt( pow, opL );
            if ( i     >  1 )  opL = ProdInt( opL, opL );
            i = i / 2;
        }
    }

    /* return the power                                                    */
    return pow;
}


/****************************************************************************
**
*F  PowObjInt(<op>,<n>) . . . . . . . . . . power of an object and an integer
*/
Obj             PowObjInt (
    Obj                 op,
    Obj                 n )
{
    Obj                 res = 0;        /* result                          */
    UInt                i, k, l;        /* loop variables                  */

    /* if the integer is zero, return the neutral element of the operand   */
    if      ( TYPE_OBJ(n) == T_INT && INT_INTOBJ(n) ==  0 ) {
        res = ONE( op );
    }

    /* if the integer is one, return a copy of the operand                 */
    else if ( TYPE_OBJ(n) == T_INT && INT_INTOBJ(n) ==  1 ) {
        res = CopyObj( op, 0 );
    }

    /* if the integer is minus one, return the inverse of the operand      */
    else if ( TYPE_OBJ(n) == T_INT && INT_INTOBJ(n) == -1 ) {
        res = INV( op );
    }

    /* if the integer is negative, invert the operand and the integer      */
    else if ( TYPE_OBJ(n) == T_INT && INT_INTOBJ(n) <   0 ) {
        res = INV( op );
        if ( res == Fail ) {
            return ErrorReturnObj(
                "Operations: <obj> must have an inverse",
                0L, 0L,
                "you can return an inverse for <obj>" );
        }
        res = POW( res, AINV( n ) );
    }

    /* if the integer is negative, invert the operand and the integer      */
    else if ( TYPE_OBJ(n) == T_INTNEG ) {
        res = INV( op );
        if ( res == Fail ) {
            return ErrorReturnObj(
                "Operations: <obj> must have an inverse",
                0L, 0L,
                "you can return an inverse for <obj>" );
        }
        res = POW( res, AINV( n ) );
    }

    /* if the integer is small, compute the power by repeated squaring     */
    /* the loop invariant is <result> = <res>^<k> * <op>^<l>, <l> < <k>    */
    /* <res> = 0 means that <res> is the neutral element                   */
    else if ( TYPE_OBJ(n) == T_INT && INT_INTOBJ(n) >   0 ) {
        res = 0;
        k = 1L << 31;
        l = INT_INTOBJ(n);
        while ( 1 < k ) {
            res = (res == 0 ? res : PROD( res, res ));
            k = k / 2;
            if ( k <= l ) {
                res = (res == 0 ? op : PROD( res, op ));
                l = l - k;
            }
        }
    }

    /* if the integer is large, compute the power by repeated squaring     */
    else if ( TYPE_OBJ(n) == T_INTPOS ) {
        res = 0;
        for ( i = SIZE_OBJ(n)/sizeof(TypDigit); 0 < i; i-- ) {
            k = 1L << (8*sizeof(TypDigit));
            l = ((TypDigit*) ADDR_OBJ(n))[i-1];
            while ( 1 < k ) {
                res = (res == 0 ? res : PROD( res, res ));
                k = k / 2;
                if ( k <= l ) {
                    res = (res == 0 ? op : PROD( res, op ));
                    l = l - k;
                }
            }
        }
    }

    /* return the result                                                   */
    return res;
}

Obj             PowObjIntFunc;

Obj             PowObjIntHandler (
    Obj                 self,
    Obj                 opL,
    Obj                 opR )
{
    return PowObjInt( opL, opR );
}


/****************************************************************************
**
*F  ModInt( <intL>, <intR> )  . . representant of residue class of an integer
**
**  'ModInt' returns the smallest positive representant of the residue  class
**  of the  integer  <intL>  modulo  the  integer  <intR>.  'ModInt'  handles
**  operands of type 'T_INT', 'T_INTPOS', 'T_INTNEG'.
**
**  It can also be used in the cases that both operands  are  small  integers
**  and the result is a small integer too,  i.e., that  no  overflow  occurs.
**  This case is usually already handled in 'EvMod' for a better efficiency.
**
**  Is called from the 'EvMod'  binop so both operands are already evaluated.
*/
Obj             ModInt (
    Obj                 opL,
    Obj                 opR )
{
    Int                 i;              /* loop count, value for small int */
    Int                 k;              /* loop count, value for small int */
    UInt                c;              /* product of two digits           */
    TypDigit            d;              /* carry into the next digit       */
    TypDigit *          l;              /* pointer into the left operand   */
    TypDigit *          r;              /* pointer into the right operand  */
    TypDigit            r1;             /* leading digit of the right oper */
    TypDigit            r2;             /* next digit of the right operand */
    UInt                rs;             /* size of the right operand       */
    UInt                e;              /* we mult r by 2^e so r1 >= 32768 */
    Obj                 mod;            /* handle of the remainder bag     */
    TypDigit *          m;              /* pointer into the remainder      */
    UInt                m01;            /* leading two digits of the rem.  */
    TypDigit            m2;             /* next digit of the remainder     */
    TypDigit            qi;             /* guessed digit of the quotient   */

    /* compute the remainder of two small integers                         */
    if ( ARE_INTOBJS( opL, opR ) ) {

        /* pathological case first                                         */
        if ( opR == INTOBJ_INT(0) ) {
            opR = ErrorReturnObj(
                "Integer operations: <divisor> must be nonzero",
                0L, 0L,
                "you can return a nonzero value for <divisor>" );
            return MOD( opL, opR );
        }

        /* get the integer values                                          */
        i = INT_INTOBJ(opL);
        k = INT_INTOBJ(opR);

        /* compute the remainder, make sure we divide only positive numbers*/
        if (      0 <= i && 0 <= k )  i =       (  i %  k );
        else if ( 0 <= i && k <  0 )  i =       (  i % -k );
        else if ( i < 0  && 0 <= k )  i = ( k - ( -i %  k )) % k;
        else if ( i < 0  && k <  0 )  i = (-k - ( -i % -k )) % k;
        mod = INTOBJ_INT( i );

    }

    /* compute the remainder of a small integer by a large integer         */
    else if ( IS_INTOBJ(opL) ) {

        /* the small int -(1<<28) mod the large int (1<<28) is 0           */
        if ( opL == INTOBJ_INT(-(1<<28))
          && TYPE_OBJ(opR) == T_INTPOS && SIZE_INT(opR) == 4
          && ADDR_INT(opR)[3] == 0 && ADDR_INT(opR)[2] == 0
          && 65536*ADDR_INT(opR)[1] + ADDR_INT(opR)[0] == 1<<28 )
            mod = INTOBJ_INT(0);

        /* in all other cases the remainder is equal the left operand      */
        else if ( 0 <= INT_INTOBJ(opL) )
            mod = opL;
        else if ( TYPE_OBJ(opR) == T_INTPOS )
            mod = SumInt( opL, opR );
        else
            mod = DiffInt( opL, opR );

    }

    /* compute the remainder of a large integer by a small integer         */
    else if ( IS_INTOBJ(opR)
           && INT_INTOBJ(opR) < 65536 && -65536 <= INT_INTOBJ(opR) ) {

        /* pathological case first                                         */
        if ( opR == INTOBJ_INT(0) ) {
            opR = ErrorReturnObj(
                "Integer operations: <divisor> must be nonzero",
                0L, 0L,
                "you can return a nonzero value for <divisor>" );
            return MOD( opL, opR );
        }

        /* get the integer value, make positive                            */
        i = INT_INTOBJ(opR);  if ( i < 0 )  i = -i;

        /* maybe its trivial                                               */
        if ( 65536 % i == 0 ) {
            c = ADDR_INT(opL)[0] % i;
        }

        /* otherwise run through the left operand and divide digitwise     */
        else {
            l = ADDR_INT(opL) + SIZE_INT(opL) - 1;
            c = 0;
            for ( ; l >= ADDR_INT(opL); l-- ) {
                c  = (c<<16) + *l;
                c  = c % i;
            }
        }

        /* now c is the result, it has the same sign as the left operand   */
        if ( TYPE_OBJ(opL) == T_INTPOS )
            mod = INTOBJ_INT( c );
        else if ( c == 0 )
            mod = INTOBJ_INT( c );
        else if ( 0 <= INT_INTOBJ(opR) )
            mod = SumInt( INTOBJ_INT( -c ), opR );
        else
            mod = DiffInt( INTOBJ_INT( -c ), opR );

    }

    /* compute the remainder of a large integer modulo a large integer     */
    else {

        /* a small divisor larger than one digit isn't handled above       */
        if ( IS_INTOBJ(opR) ) {
            if ( 0 < INT_INTOBJ(opR) ) {
                mod = NewBag( T_INTPOS, 4*sizeof(TypDigit) );
                ADDR_INT(mod)[0] = (TypDigit)(INT_INTOBJ(opR));
                ADDR_INT(mod)[1] = (INT_INTOBJ(opR)>>16);
                opR = mod;
            }
            else {
                mod = NewBag( T_INTNEG, 4*sizeof(TypDigit) );
                ADDR_INT(mod)[0] = (TypDigit)(-INT_INTOBJ(opR));
                ADDR_INT(mod)[1] = (-INT_INTOBJ(opR))>>16;
                opR = mod;
            }
        }

        /* trivial case first                                              */
        if ( SIZE_INT(opL) < SIZE_INT(opR) ) {
            if ( TYPE_OBJ(opL) == T_INTPOS )
                return opL;
            else if ( TYPE_OBJ(opR) == T_INTPOS )
                return SumInt( opL, opR );
            else
                return DiffInt( opL, opR );
        }

        /* copy the left operand into a new bag, this holds the remainder  */
        mod = NewBag( TYPE_OBJ(opL), (SIZE_INT(opL)+4)*sizeof(TypDigit) );
        l = ADDR_INT(opL);
        m = ADDR_INT(mod);
        for ( k = SIZE_INT(opL)-1; k >= 0; k-- )
            *m++ = *l++;

        /* get the size of the right operand, and get the leading 2 digits */
        rs = SIZE_INT(opR);
        r  = ADDR_INT(opR);
        while ( r[rs-1] == 0 )  rs--;
        for ( e = 0; (r[rs-1]<<e) + (r[rs-2]>>(16-e)) < 65536/2; e++ ) ;
        r1 = (r[rs-1]<<e) + (r[rs-2]>>(16-e));
        r2 = (r[rs-2]<<e) + (rs>=3 ? r[rs-3]>>(16-e) : 0);

        /* run through the digits in the quotient                          */
        for ( i = SIZE_INT(mod)-SIZE_INT(opR)-1; i >= 0; i-- ) {

            /* guess the factor                                            */
            m = ADDR_INT(mod) + rs + i;
            m01 = ((65536*m[0]+m[-1])<<e) + (m[-2]>>(16-e));
            if ( m01 == 0 )  continue;
            m2  = (m[-2]<<e) + (rs+i>=3 ? m[-3]>>(16-e) : 0);
            if ( (m[0]<<e)+(m[-1]>>(16-e)) < r1 )  qi = m01 / r1;
            else                                   qi = 65536 - 1;
            while ( m01-qi*r1 < 65536 && 65536*(m01-qi*r1)+m2 < qi*r2 )
                qi--;

            /* m = m - qi * r;                                             */
            d = 0;
            m = ADDR_INT(mod) + i;
            r = ADDR_INT(opR);
            for ( k = 0; k < rs; ++k, ++m, ++r ) {
                c = *m - qi * *r - d;  *m = c;  d = -(c>>16);
            }
            c = *m - d;  *m = c;  d = -(c>>16);

            /* if we have a borrow then add back                           */
            if ( d != 0 ) {
                d = 0;
                m = ADDR_INT(mod) + i;
                r = ADDR_INT(opR);
                for ( k = 0; k < rs; ++k, ++m, ++r ) {
                    c = *m + *r + d;  *m = c;  d = (c>>16);
                }
                c = *m + d;  *m = c;  d = (c>>16);
                qi--;
            }

        }

        /* remove the leading zeroes                                       */
        m = ADDR_INT(mod) + SIZE_INT(mod);
        if ( (m[-4] == 0 && m[-3] == 0 && m[-2] == 0 && m[-1] == 0)
          || (SIZE_INT(mod) == 4 && m[-2] == 0 && m[-1] == 0) ) {

            /* find the number of significant digits                       */
            m = ADDR_INT(mod);
            for ( k = SIZE_INT(mod); k != 0; k-- ) {
                if ( m[k-1] != 0 )
                    break;
            }

            /* reduce to small integer if possible, otherwise shrink bag   */
            if ( k <= 2 && TYPE_OBJ(mod) == T_INTPOS
              && (UInt)(65536*m[1]+m[0]) < (1<<28) )
                mod = INTOBJ_INT( 65536*m[1]+m[0] );
            else if ( k <= 2 && TYPE_OBJ(mod) == T_INTNEG
              && (UInt)(65536*m[1]+m[0]) <= (1<<28) )
                mod = INTOBJ_INT( -(65536*m[1]+m[0]) );
            else
                ResizeBag( mod, (((k + 3) / 4) * 4) * sizeof(TypDigit) );
        }

        /* make the representant positive                                  */
        if ( (TYPE_OBJ(mod) == T_INT && INT_INTOBJ(mod) < 0)
          || TYPE_OBJ(mod) == T_INTNEG ) {
            if ( TYPE_OBJ(opR) == T_INTPOS )
                mod = SumInt( mod, opR );
            else
                mod = DiffInt( mod, opR );
        }

    }

    /* return the result                                                   */
    return mod;
}


/****************************************************************************
**
*F  IsIntHandler(<self>,<val>)  . . . . . . . . . . internal function 'IsInt'
**
**  'IsIntHandler' implements the internal filter 'IsInt'.
**
**  'IsInt( <val> )'
**
**  'IsInt'  returns 'true'  if the  value  <val>  is an integer  and 'false'
**  otherwise.
*/
Obj             IsIntFilt;

Obj             IsIntHandler (
    Obj                 self,
    Obj                 val )
{
    /* return 'true' if <obj> is an integer and 'false' otherwise          */
    if ( TYPE_OBJ(val) == T_INT
      || TYPE_OBJ(val) == T_INTPOS
      || TYPE_OBJ(val) == T_INTNEG ) {
        return True;
    }
    else if ( TYPE_OBJ(val) <= FIRST_EXTERNAL_TYPE ) {
        return False;
    }
    else {
        return DoFilter( self, val );
    }
}


/****************************************************************************
**
*F  QuoInt( <intL>, <intR> )  . . . . . . . . . . . quotient of two integers
**
**  'QuoInt' returns the integer part of the two integers <intL> and  <intR>.
**  'QuoInt' handles operands of type  'T_INT',  'T_INTPOS'  and  'T_INTNEG'.
**
**  It can also be used in the cases that both operands  are  small  integers
**  and the result is a small integer too,  i.e., that  no  overflow  occurs.
**
**  Note that this routine is not called from 'EvQuo', the  division  of  two
**  integers yields  a  rational  and  is  therefor  performed  in  'QuoRat'.
**  This operation is however available through the internal function 'Quo'.
*/
Obj             QuoInt (
    Obj                 opL,
    Obj                 opR )
{
    Int                 i;              /* loop count, value for small int */
    Int                 k;              /* loop count, value for small int */
    UInt                c;              /* product of two digits           */
    TypDigit            d;              /* carry into the next digit       */
    TypDigit *          l;              /* pointer into the left operand   */
    UInt                l01;            /* leading two digits of the left  */
    TypDigit            l2;             /* next digit of the left operand  */
    TypDigit *          r;              /* pointer into the right operand  */
    TypDigit            r1;             /* leading digit of the right oper */
    TypDigit            r2;             /* next digit of the right operand */
    UInt                rs;             /* size of the right operand       */
    UInt                e;              /* we mult r by 2^e so r1 >= 32768 */
    TypDigit *          q;              /* pointer into the quotient       */
    Obj                 quo;            /* handle of the result bag        */
    TypDigit            qi;             /* guessed digit of the quotient   */

    /* divide to small integers                                            */
    if ( ARE_INTOBJS( opL, opR ) ) {

        /* pathological case first                                         */
        if ( opR == INTOBJ_INT(0) ) {
            opR = ErrorReturnObj(
                "Integer operations: <divisor> must be nonzero",
                0L, 0L,
                "you can return a nonzero value for <divisor>" );
            return QUO( opL, opR );
        }

        /* the small int -(1<<28) divided by -1 is the large int (1<<28)   */
        if ( opL == INTOBJ_INT(-(1<<28)) && opR == INTOBJ_INT(-1) ) {
            quo = NewBag( T_INTPOS, 4*sizeof(TypDigit) );
            ADDR_INT(quo)[1] = 1<<12;
            ADDR_INT(quo)[0] = 0;
            return quo;
        }

        /* get the integer values                                          */
        i = INT_INTOBJ(opL);
        k = INT_INTOBJ(opR);

        /* divide, make sure we divide only positive numbers               */
        if (      0 <= i && 0 <= k )  i =    (  i /  k );
        else if ( 0 <= i && k <  0 )  i =  - (  i / -k );
        else if ( i < 0  && 0 <= k )  i =  - ( -i /  k );
        else if ( i < 0  && k <  0 )  i =    ( -i / -k );
        quo = INTOBJ_INT( i );

    }

    /* divide a small integer by a large one                               */
    else if ( IS_INTOBJ(opL) ) {

        /* the small int -(1<<28) divided by the large int (1<<28) is -1   */
        if ( opL == INTOBJ_INT(-(1<<28))
          && TYPE_OBJ(opR) == T_INTPOS && SIZE_INT(opR) == 4
          && ADDR_INT(opR)[3] == 0 && ADDR_INT(opR)[2] == 0
          && 65536*ADDR_INT(opR)[1] + ADDR_INT(opR)[0] == 1<<28 )
            quo = INTOBJ_INT(-1);

        /* in all other cases the quotient is of course zero               */
        else
            quo = INTOBJ_INT(0);

    }

    /* divide a large integer by a small integer                           */
    else if ( IS_INTOBJ(opR)
           && INT_INTOBJ(opR) < 65536 && -65536 <= INT_INTOBJ(opR) ) {

        /* pathological case first                                         */
        if ( opR == INTOBJ_INT(0) ) {
            opR = ErrorReturnObj(
                "Integer operations: <divisor> must be nonzero",
                0L, 0L,
                "you can return a nonzero value for <divisor>" );
            return QUO( opL, opR );
        }

        /* get the integer value, make positive                            */
        i = INT_INTOBJ(opR);  if ( i < 0 )  i = -i;

        /* allocate a bag for the result and set up the pointers           */
        if ( (TYPE_OBJ(opL)==T_INTPOS && 0 < INT_INTOBJ(opR))
          || (TYPE_OBJ(opL)==T_INTNEG && INT_INTOBJ(opR) < 0) )
            quo = NewBag( T_INTPOS, SIZE_OBJ(opL) );
        else
            quo = NewBag( T_INTNEG, SIZE_OBJ(opL) );
        l = ADDR_INT(opL) + SIZE_INT(opL) - 1;
        q = ADDR_INT(quo) + SIZE_INT(quo) - 1;

        /* run through the left operand and divide digitwise               */
        c = 0;
        for ( ; l >= ADDR_INT(opL); l--, q-- ) {
            c  = (c<<16) + *l;
            *q = c / i;
            c  = c - i * *q;
            /*N clever compilers may prefer:  c  = c % i;                  */
        }

        /* remove the leading zeroes, note that there can't be more than 5 */
        q = ADDR_INT(quo) + SIZE_INT(quo);
        if ( q[-4] == 0 && q[-3] == 0 && q[-2] == 0 && q[-1] == 0 ) {
            ResizeBag( quo, (SIZE_INT(quo)-4)*sizeof(TypDigit) );
        }

        /* reduce to small integer if possible                             */
        q = ADDR_INT(quo) + SIZE_INT(quo);
        if ( SIZE_INT(quo) == 4 && q[-2] == 0 && q[-1] == 0 ) {
            if ( TYPE_OBJ(quo) == T_INTPOS
              && (UInt)(65536*q[-3]+q[-4]) < (1<<28) )
                quo = INTOBJ_INT( 65536*q[-3]+q[-4] );
            else if ( TYPE_OBJ(quo) == T_INTNEG
              && (UInt)(65536*q[-3]+q[-4]) <= (1<<28) )
                quo = INTOBJ_INT( -(65536*q[-3]+q[-4]) );
        }

    }

    /* divide a large integer by a large integer                           */
    else {

        /* a small divisor larger than one digit isn't handled above       */
        if ( IS_INTOBJ(opR) ) {
            if ( 0 < INT_INTOBJ(opR) ) {
                quo = NewBag( T_INTPOS, 4*sizeof(TypDigit) );
                ADDR_INT(quo)[0] = (TypDigit)(INT_INTOBJ(opR));
                ADDR_INT(quo)[1] = (INT_INTOBJ(opR)>>16);
                opR = quo;
            }
            else {
                quo = NewBag( T_INTNEG, 4*sizeof(TypDigit) );
                ADDR_INT(quo)[0] = (TypDigit)(-INT_INTOBJ(opR));
                ADDR_INT(quo)[1] = (-INT_INTOBJ(opR))>>16;
                opR = quo;
            }
        }

        /* trivial case first                                              */
        if ( SIZE_INT(opL) < SIZE_INT(opR) )
            return INTOBJ_INT(0);

        /* copy the left operand into a new bag, this holds the remainder  */
        quo = NewBag( TYPE_OBJ(opL), (SIZE_INT(opL)+4)*sizeof(TypDigit) );
        l = ADDR_INT(opL);
        q = ADDR_INT(quo);
        for ( k = SIZE_INT(opL)-1; k >= 0; k-- )
            *q++ = *l++;
        opL = quo;

        /* get the size of the right operand, and get the leading 2 digits */
        rs = SIZE_INT(opR);
        r  = ADDR_INT(opR);
        while ( r[rs-1] == 0 )  rs--;
        for ( e = 0; (r[rs-1]<<e) + (r[rs-2]>>(16-e)) < 65536/2; e++ ) ;
        r1 = (r[rs-1]<<e) + (r[rs-2]>>(16-e));
        r2 = (r[rs-2]<<e) + (rs>=3 ? r[rs-3]>>(16-e) : 0);

        /* allocate a bag for the quotient                                 */
        if ( TYPE_OBJ(opL) == TYPE_OBJ(opR) )
            quo = NewBag( T_INTPOS, SIZE_OBJ(opL)-SIZE_OBJ(opR) );
        else
            quo = NewBag( T_INTNEG, SIZE_OBJ(opL)-SIZE_OBJ(opR) );

        /* run through the digits in the quotient                          */
        for ( i = SIZE_INT(opL)-SIZE_INT(opR)-1; i >= 0; i-- ) {

            /* guess the factor                                            */
            l = ADDR_INT(opL) + rs + i;
            l01 = ((65536*l[0]+l[-1])<<e) + (l[-2]>>(16-e));

            if ( l01 == 0 )  continue;
            l2  = (l[-2]<<e) + (rs+i>=3 ? l[-3]>>(16-e) : 0);
            if ( (l[0]<<e)+(l[-1]>>(16-e)) < r1 )  qi = l01 / r1;
            else                                   qi = 65536 - 1;
            while ( l01-qi*r1 < 65536 && 65536*(l01-qi*r1)+l2 < qi*r2 )
                qi--;

            /* l = l - qi * r;                                             */
            d = 0;
            l = ADDR_INT(opL) + i;
            r = ADDR_INT(opR);
            for ( k = 0; k < rs; ++k, ++l, ++r ) {
                c = *l - qi * *r - d;  *l = c;  d = -(c>>16);
            }
            c = *l - d; d = -(c>>16);

            /* if we have a borrow then add back                           */
            if ( d != 0 ) {
                d = 0;
                l = ADDR_INT(opL) + i;
                r = ADDR_INT(opR);
                for ( k = 0; k < rs; ++k, ++l, ++r ) {
                    c = *l + *r + d;  *l = c;  d = (c>>16);
                }
                c = *l + d; d = (c>>16);
                qi--;
            }

            /* store the digit in the quotient                             */
            ADDR_INT(quo)[i] = qi;

        }

        /* remove the leading zeroes, note that there can't be more than 7 */
        q = ADDR_INT(quo) + SIZE_INT(quo);
        if ( SIZE_INT(quo) > 4
          && q[-4] == 0 && q[-3] == 0 && q[-2] == 0 && q[-1] == 0 ) {
            ResizeBag( quo, (SIZE_INT(quo)-4)*sizeof(TypDigit) );
        }

        /* reduce to small integer if possible                             */
        q = ADDR_INT(quo) + SIZE_INT(quo);
        if ( SIZE_INT(quo) == 4 && q[-2] == 0 && q[-1] == 0 ) {
            if ( TYPE_OBJ(quo) == T_INTPOS
              && (UInt)(65536*q[-3]+q[-4]) < (1<<28) )
                quo = INTOBJ_INT( 65536*q[-3]+q[-4] );
            else if ( TYPE_OBJ(quo) == T_INTNEG
              && (UInt)(65536*q[-3]+q[-4]) <= (1<<28) )
                quo = INTOBJ_INT( -(65536*q[-3]+q[-4]) );
        }

    }

    /* return the result                                                   */
    return quo;
}


/****************************************************************************
**
*F  FuncQuoInt(<self>,<opL>,<opR>)  . . . . . . .  internal function 'QuoInt'
**
**  'FuncQuoInt' implements the internal function 'QuoInt'.
**
**  'QuoInt( <i>, <k> )'
**
**  'Quo' returns the  integer part of the quotient  of its integer operands.
**  If <i>  and <k> are  positive 'Quo( <i>,  <k> )' is  the largest positive
**  integer <q>  such that '<q> * <k>  \<= <i>'.  If  <i> or  <k> or both are
**  negative we define 'Abs( Quo(<i>,<k>) ) = Quo( Abs(<i>), Abs(<k>) )'  and
**  'Sign( Quo(<i>,<k>) ) = Sign(<i>) * Sign(<k>)'.  Dividing by 0  causes an
**  error.  'Rem' (see "Rem") can be used to compute the remainder.
*/
Obj             FuncQuoInt (
    Obj                 self,
    Obj                 opL,
    Obj                 opR )
{
    /* check the arguments                                                 */
    while ( TYPE_OBJ(opL) != T_INT
         && TYPE_OBJ(opL) != T_INTPOS
         && TYPE_OBJ(opL) != T_INTNEG ) {
        opL = ErrorReturnObj(
            "QuoInt: <left> must be an integer (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(opL)].name), 0L,
            "you can return an integer for <left>" );
    }
    while ( TYPE_OBJ(opR) != T_INT
         && TYPE_OBJ(opR) != T_INTPOS
         && TYPE_OBJ(opR) != T_INTNEG ) {
        opR = ErrorReturnObj(
            "QuoInt: <right> must be an integer (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(opR)].name), 0L,
            "you can return an integer for <rigth>" );
    }

    /* return the quotient                                                 */
    return QuoInt( opL, opR );
}


/****************************************************************************
**
*F  RemInt( <intL>, <intR> )  . . . . . . . . . . . remainder of two integers
**
**  'RemInt' returns the remainder of the quotient  of  the  integers  <intL>
**  and <intR>.  'RemInt' handles operands of type  'T_INT',  'T_INTPOS'  and
**  'T_INTNEG'.
**
**  Note that the remainder is different from the value returned by the 'mod'
**  operator which is always positive.
**
**  'RemInt' is called from 'FunRemInt'.
*/
Obj             RemInt (
    Obj                 opL,
    Obj                 opR )
{
    Int                 i;              /* loop count, value for small int */
    Int                 k;              /* loop count, value for small int */
    UInt                c;              /* product of two digits           */
    TypDigit            d;              /* carry into the next digit       */
    TypDigit *          l;              /* pointer into the left operand   */
    TypDigit *          r;              /* pointer into the right operand  */
    TypDigit            r1;             /* leading digit of the right oper */
    TypDigit            r2;             /* next digit of the right operand */
    UInt                rs;             /* size of the right operand       */
    UInt                e;              /* we mult r by 2^e so r1 >= 32768 */
    Obj                 rem;            /* handle of the remainder bag     */
    TypDigit *          m;              /* pointer into the remainder      */
    UInt                m01;            /* leading two digits of the rem.  */
    TypDigit            m2;             /* next digit of the remainder     */
    TypDigit            qi;             /* guessed digit of the quotient   */

    /* compute the remainder of two small integers                         */
    if ( ARE_INTOBJS( opL, opR ) ) {

        /* pathological case first                                         */
        if ( opR == INTOBJ_INT(0) ) {
            opR = ErrorReturnObj(
                "Integer operations: <divisor> must be nonzero",
                0L, 0L,
                "you can return a nonzero value for <divisor>" );
            return QUO( opL, opR );
        }

        /* get the integer values                                          */
        i = INT_INTOBJ(opL);
        k = INT_INTOBJ(opR);

        /* compute the remainder, make sure we divide only positive numbers*/
        if (      0 <= i && 0 <= k )  i =    (  i %  k );
        else if ( 0 <= i && k <  0 )  i =    (  i % -k );
        else if ( i < 0  && 0 <= k )  i =  - ( -i %  k );
        else if ( i < 0  && k <  0 )  i =  - ( -i % -k );
        rem = INTOBJ_INT( i );

    }

    /* compute the remainder of a small integer by a large integer         */
    else if ( IS_INTOBJ(opL) ) {

        /* the small int -(1<<28) rem the large int (1<<28) is 0           */
        if ( opL == INTOBJ_INT(-(1<<28))
          && TYPE_OBJ(opR) == T_INTPOS && SIZE_INT(opR) == 4
          && ADDR_INT(opR)[3] == 0 && ADDR_INT(opR)[2] == 0
          && 65536*ADDR_INT(opR)[1] + ADDR_INT(opR)[0] == 1<<28 )
            rem = INTOBJ_INT(0);

        /* in all other cases the remainder is equal the left operand      */
        else
            rem = opL;

    }

    /* compute the remainder of a large integer by a small integer         */
    else if ( IS_INTOBJ(opR)
           && INT_INTOBJ(opR) < 65536 && -65536 <= INT_INTOBJ(opR) ) {

        /* pathological case first                                         */
        if ( opR == INTOBJ_INT(0) ) {
            opR = ErrorReturnObj(
                "Integer operations: <divisor> must be nonzero",
                0L, 0L,
                "you can return a nonzero value for <divisor>" );
            return QUO( opL, opR );
        }

        /* get the integer value, make positive                            */
        i = INT_INTOBJ(opR);  if ( i < 0 )  i = -i;

        /* maybe its trivial                                               */
        if ( 65536 % i == 0 ) {
            c = ADDR_INT(opL)[0] % i;
        }

        /* otherwise run through the left operand and divide digitwise     */
        else {
            l = ADDR_INT(opL) + SIZE_INT(opL) - 1;
            c = 0;
            for ( ; l >= ADDR_INT(opL); l-- ) {
                c  = (c<<16) + *l;
                c  = c % i;
            }
        }

        /* now c is the result, it has the same sign as the left operand   */
        if ( TYPE_OBJ(opL) == T_INTPOS )
            rem = INTOBJ_INT(  c );
        else
            rem = INTOBJ_INT( -c );

    }

    /* compute the remainder of a large integer remulo a large integer     */
    else {

        /* a small divisor larger than one digit isn't handled above       */
        if ( IS_INTOBJ(opR) ) {
            if ( 0 < INT_INTOBJ(opR) ) {
                rem = NewBag( T_INTPOS, 4*sizeof(TypDigit) );
                ADDR_INT(rem)[0] = (TypDigit)(INT_INTOBJ(opR));
                ADDR_INT(rem)[1] = (INT_INTOBJ(opR)>>16);
                opR = rem;
            }
            else {
                rem = NewBag( T_INTNEG, 4*sizeof(TypDigit) );
                ADDR_INT(rem)[0] = (TypDigit)(-INT_INTOBJ(opR));
                ADDR_INT(rem)[1] = (-INT_INTOBJ(opR))>>16;
                opR = rem;
            }
        }

        /* trivial case first                                              */
        if ( SIZE_INT(opL) < SIZE_INT(opR) )
            return opL;

        /* copy the left operand into a new bag, this holds the remainder  */
        rem = NewBag( TYPE_OBJ(opL), (SIZE_INT(opL)+4)*sizeof(TypDigit) );
        l = ADDR_INT(opL);
        m = ADDR_INT(rem);
        for ( k = SIZE_INT(opL)-1; k >= 0; k-- )
            *m++ = *l++;

        /* get the size of the right operand, and get the leading 2 digits */
        rs = SIZE_INT(opR);
        r  = ADDR_INT(opR);
        while ( r[rs-1] == 0 )  rs--;
        for ( e = 0; (r[rs-1]<<e) + (r[rs-2]>>(16-e)) < 65536/2; e++ ) ;
        r1 = (r[rs-1]<<e) + (r[rs-2]>>(16-e));
        r2 = (r[rs-2]<<e) + (rs>=3 ? r[rs-3]>>(16-e) : 0);

        /* run through the digits in the quotient                          */
        for ( i = SIZE_INT(rem)-SIZE_INT(opR)-1; i >= 0; i-- ) {

            /* guess the factor                                            */
            m = ADDR_INT(rem) + rs + i;
            m01 = ((65536*m[0]+m[-1])<<e) + (m[-2]>>(16-e));
            if ( m01 == 0 )  continue;
            m2  = (m[-2]<<e) + (rs+i>=3 ? m[-3]>>(16-e) : 0);
            if ( (m[0]<<e)+(m[-1]>>(16-e)) < r1 )  qi = m01 / r1;
            else                                   qi = 65536 - 1;
            while ( m01-qi*r1 < 65536 && 65536*(m01-qi*r1)+m2 < qi*r2 )
                qi--;

            /* m = m - qi * r;                                             */
            d = 0;
            m = ADDR_INT(rem) + i;
            r = ADDR_INT(opR);
            for ( k = 0; k < rs; ++k, ++m, ++r ) {
                c = *m - qi * *r - d;  *m = c;  d = -(c>>16);
            }
            c = *m - d;  *m = c;  d = -(c>>16);

            /* if we have a borrow then add back                           */
            if ( d != 0 ) {
                d = 0;
                m = ADDR_INT(rem) + i;
                r = ADDR_INT(opR);
                for ( k = 0; k < rs; ++k, ++m, ++r ) {
                    c = *m + *r + d;  *m = c;  d = (c>>16);
                }
                c = *m + d;  *m = c;  d = (c>>16);
                qi--;
            }

        }

        /* remove the leading zeroes                                       */
        m = ADDR_INT(rem) + SIZE_INT(rem);
        if ( (m[-4] == 0 && m[-3] == 0 && m[-2] == 0 && m[-1] == 0)
          || (SIZE_INT(rem) == 4 && m[-2] == 0 && m[-1] == 0) ) {

            /* find the number of significant digits                       */
            m = ADDR_INT(rem);
            for ( k = SIZE_INT(rem); k != 0; k-- ) {
                if ( m[k-1] != 0 )
                    break;
            }

            /* reduce to small integer if possible, otherwise shrink bag   */
            if ( k <= 2 && TYPE_OBJ(rem) == T_INTPOS
              && (UInt)(65536*m[1]+m[0]) < (1<<28) )
                rem = INTOBJ_INT( 65536*m[1]+m[0] );
            else if ( k <= 2 && TYPE_OBJ(rem) == T_INTNEG
              && (UInt)(65536*m[1]+m[0]) <= (1<<28) )
                rem = INTOBJ_INT( -(65536*m[1]+m[0]) );
            else
                ResizeBag( rem, (((k + 3) / 4) * 4) * sizeof(TypDigit) );
        }

    }

    /* return the result                                                   */
    return rem;
}


/****************************************************************************
**
*F  FuncRemInt(<self>,<opL>,<opR>)  . . . . . . .  internal function 'RemInt'
**
**  'FuncRem' implements the internal function 'RemInt'.
**
**  'RemInt( <i>, <k> )'
**
**  'Rem' returns the remainder of its two integer operands,  i.e., if <k> is
**  not equal to zero 'Rem( <i>, <k> ) = <i> - <k> *  Quo( <i>, <k> )'.  Note
**  that the rules given  for 'Quo' (see "Quo") imply  that 'Rem( <i>, <k> )'
**  has the same sign as <i> and its absolute value is strictly less than the
**  absolute value of <k>.  Dividing by 0 causes an error.
*/
Obj             FuncRemInt (
    Obj                 self,
    Obj                 opL,
    Obj                 opR )
{
    /* check the arguments                                                 */
    while ( TYPE_OBJ(opL) != T_INT
         && TYPE_OBJ(opL) != T_INTPOS
         && TYPE_OBJ(opL) != T_INTNEG ) {
        opL = ErrorReturnObj(
            "RemInt: <left> must be an integer (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(opL)].name), 0L,
            "you can return an integer for <left>" );
    }
    while ( TYPE_OBJ(opR) != T_INT
         && TYPE_OBJ(opR) != T_INTPOS
         && TYPE_OBJ(opR) != T_INTNEG ) {
        opR = ErrorReturnObj(
            "RemInt: <right> must be an integer (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(opR)].name), 0L,
            "you can return an integer for <rigth>" );
    }

    /* return the remainder                                                */
    return RemInt( opL, opR );
}


/****************************************************************************
**
*F  GcdInt( <opL>, <opR> )  . . . . . . . . . . . . . . . gcd of two integers
**
**  'GcdInt' returns the gcd of the two integers <opL> and <opR>.
**
**  It is called from 'FunGcdInt' and the rational package.
*/
Obj             GcdInt (
    Obj                 opL,
    Obj                 opR )
{
    Int                 i;              /* loop count, value for small int */
    Int                 k;              /* loop count, value for small int */
    UInt                c;              /* product of two digits           */
    TypDigit            d;              /* carry into the next digit       */
    TypDigit *          r;              /* pointer into the right operand  */
    TypDigit            r1;             /* leading digit of the right oper */
    TypDigit            r2;             /* next digit of the right operand */
    UInt                rs;             /* size of the right operand       */
    UInt                e;              /* we mult r by 2^e so r1 >= 32768 */
    TypDigit *          l;              /* pointer into the left operand   */
    UInt                l01;            /* leading two digits of the rem.  */
    TypDigit            l2;             /* next digit of the remainder     */
    UInt                ls;             /* size of the left operand        */
    TypDigit            qi;             /* guessed digit of the quotient   */
    Obj                 gcd;            /* handle of the result            */

    /* compute the gcd of two small integers                               */
    if ( ARE_INTOBJS( opL, opR ) ) {

        /* get the integer values, make them positive                      */
        i = INT_INTOBJ(opL);  if ( i < 0 )  i = -i;
        k = INT_INTOBJ(opR);  if ( k < 0 )  k = -k;

        /* compute the gcd using Euclids algorithm                         */
        while ( k != 0 ) {
            c = k;
            k = i % k;
            i = c;
        }

        /* now i is the result                                             */
        if ( i == (1<<28) ) {
            gcd = NewBag( T_INTPOS, 4*sizeof(TypDigit) );
            ADDR_INT(gcd)[0] = i;
            ADDR_INT(gcd)[1] = (i>>16);
        }
        else {
            gcd = INTOBJ_INT( i );
        }

    }

    /* compute the gcd of a small and a large integer                      */
    else if ( (IS_INTOBJ(opL)
               && INT_INTOBJ(opL) < 65536 && -65536 <= INT_INTOBJ(opL))
           || (IS_INTOBJ(opR)
               && INT_INTOBJ(opR) < 65536 && -65536 <= INT_INTOBJ(opR)) ) {

        /* make the right operand the small one                            */
        if ( IS_INTOBJ(opL) ) {
            gcd = opL;  opL = opR;  opR = gcd;
        }

        /* maybe it's trivial                                              */
        if ( opR == INTOBJ_INT(0) )
            return opL;

        /* get the right operand value, make it positive                   */
        i = INT_INTOBJ(opR);  if ( i < 0 )  i = -i;

        /* do one remainder operation                                      */
        l = ADDR_INT(opL) + SIZE_INT(opL) - 1;
        c = 0;
        for ( ; l >= ADDR_INT(opL); l-- ) {
            c  = (c<<16) + *l;
            c  = c % i;
        }
        k = c;

        /* compute the gcd using Euclids algorithm                         */
        while ( k != 0 ) {
            c = k;
            k = i % k;
            i = c;
        }

        /* now i is the result                                             */
        if ( i == (1<<28) ) {
            gcd = NewBag( T_INTPOS, 4*sizeof(TypDigit) );
            ADDR_INT(gcd)[0] = i;
            ADDR_INT(gcd)[1] = (i>>16);
        }
        else {
            gcd = INTOBJ_INT( i );
        }

    }

    /* compute the gcd of two large integers                               */
    else {

        /* a small divisor larger than one digit isn't handled above       */
        if ( IS_INTOBJ(opL) ) {
            if ( 0 < INT_INTOBJ(opL) ) {
                gcd = NewBag( T_INTPOS, 4*sizeof(TypDigit) );
                ADDR_INT(gcd)[0] = (TypDigit)(INT_INTOBJ(opL));
                ADDR_INT(gcd)[1] = (INT_INTOBJ(opL)>>16);
                opL = gcd;
            }
            else {
                gcd = NewBag( T_INTNEG, 4*sizeof(TypDigit) );
                ADDR_INT(gcd)[0] = (TypDigit)(-INT_INTOBJ(opL));
                ADDR_INT(gcd)[1] = (-INT_INTOBJ(opL))>>16;
                opL = gcd;
            }
        }

        /* a small divisor larger than one digit isn't handled above       */
        if ( IS_INTOBJ(opR) ) {
            if ( 0 < INT_INTOBJ(opR) ) {
                gcd = NewBag( T_INTPOS, 4*sizeof(TypDigit) );
                ADDR_INT(gcd)[0] = (TypDigit)(INT_INTOBJ(opR));
                ADDR_INT(gcd)[1] = (INT_INTOBJ(opR)>>16);
                opR = gcd;
            }
            else {
                gcd = NewBag( T_INTNEG, 4*sizeof(TypDigit) );
                ADDR_INT(gcd)[0] = (TypDigit)(-INT_INTOBJ(opR));
                ADDR_INT(gcd)[1] = (-INT_INTOBJ(opR))>>16;
                opR = gcd;
            }
        }

        /* copy the left operand into a new bag                            */
        gcd = NewBag( T_INTPOS, (SIZE_INT(opL)+4)*sizeof(TypDigit) );
        l = ADDR_INT(opL);
        r = ADDR_INT(gcd);
        for ( k = SIZE_INT(opL)-1; k >= 0; k-- )
            *r++ = *l++;
        opL = gcd;

        /* get the size of the left operand                                */
        ls = SIZE_INT(opL);
        l  = ADDR_INT(opL);
        while ( ls >= 1 && l[ls-1] == 0 )  ls--;

        /* copy the right operand into a new bag                           */
        gcd = NewBag( T_INTPOS, (SIZE_INT(opR)+4)*sizeof(TypDigit) );
        r = ADDR_INT(opR);
        l = ADDR_INT(gcd);
        for ( k = SIZE_INT(opR)-1; k >= 0; k-- )
            *l++ = *r++;
        opR = gcd;

        /* get the size of the right operand                               */
        rs = SIZE_INT(opR);
        r  = ADDR_INT(opR);
        while ( rs >= 1 && r[rs-1] == 0 )  rs--;

        /* repeat while the right operand is large                         */
        while ( rs >= 2 ) {

            /* get the leading two digits                                  */
            for ( e = 0; (r[rs-1]<<e) + (r[rs-2]>>(16-e)) < 65536/2; e++ ) ;
            r1 = (r[rs-1]<<e) + (r[rs-2]>>(16-e));
            r2 = (r[rs-2]<<e) + (rs>=3 ? r[rs-3]>>(16-e) : 0);

            /* run through the digits in the quotient                      */
            for ( i = ls - rs; i >= 0; i-- ) {

                /* guess the factor                                        */
                l = ADDR_INT(opL) + rs + i;
                l01 = ((65536*l[0]+l[-1])<<e) + (l[-2]>>(16-e));
                if ( l01 == 0 )  continue;
                l2  = (l[-2]<<e) + (rs+i>=3 ? l[-3]>>(16-e) : 0);
                if ( (l[0]<<e)+(l[-1]>>(16-e)) < r1 )  qi = l01 / r1;
                else                                   qi = 65536 - 1;
                while ( l01-qi*r1 < 65536 && 65536*(l01-qi*r1)+l2 < qi*r2 )
                    qi--;

                /* l = l - qi * r;                                         */
                d = 0;
                l = ADDR_INT(opL) + i;
                r = ADDR_INT(opR);
                for ( k = 0; k < rs; ++k, ++l, ++r ) {
                    c = *l - qi * *r - d;  *l = c;  d = -(c>>16);
                }
                c = *l - d;  *l = c;  d = -(c>>16);

                /* if we have a borrow then add back                       */
                if ( d != 0 ) {
                    d = 0;
                    l = ADDR_INT(opL) + i;
                    r = ADDR_INT(opR);
                    for ( k = 0; k < rs; ++k, ++l, ++r ) {
                        c = *l + *r + d;  *l = c;  d = (c>>16);
                    }
                    c = *l + d;  *l = c;  d = (c>>16);
                    qi--;
                }
            }

            /* exchange the two operands                                   */
            gcd = opL;  opL = opR;  opR = gcd;
            ls = rs;

            /* get the size of the right operand                           */
            rs = SIZE_INT(opR);
            r  = ADDR_INT(opR);
            while ( rs >= 1 && r[rs-1] == 0 )  rs--;

        }

        /* if the right operand is zero now, the left is the gcd           */
        if ( rs == 0 ) {

            /* remove the leading zeroes                                   */
            l = ADDR_INT(opL) + SIZE_INT(opL);
            if ( (l[-4] == 0 && l[-3] == 0 && l[-2] == 0 && l[-1] == 0)
              || (SIZE_INT(opL) == 4 && l[-2] == 0 && l[-1] == 0) ) {

                /* find the number of significant digits                   */
                l = ADDR_INT(opL);
                for ( k = SIZE_INT(opL); k != 0; k-- ) {
                    if ( l[k-1] != 0 )
                        break;
                }

                /* reduce to small integer if possible, otherwise shrink b */
                if ( k <= 2 && TYPE_OBJ(opL) == T_INTPOS
                  && (UInt)(65536*l[1]+l[0]) < (1<<28) )
                    opL = INTOBJ_INT( 65536*l[1]+l[0] );
                else if ( k <= 2 && TYPE_OBJ(opL) == T_INTNEG
                  && (UInt)(65536*l[1]+l[0]) <= (1<<28) )
                    opL = INTOBJ_INT( -(65536*l[1]+l[0]) );
                else
                    ResizeBag( opL, (((k + 3) / 4) * 4) * sizeof(TypDigit) );
            }

            gcd = opL;

        }

        /* otherwise handle one large and one small integer as above       */
        else {

            /* get the right operand value, make it positive               */
            i = r[0];

            /* do one remainder operation                                  */
            l = ADDR_INT(opL) + SIZE_INT(opL) - 1;
            c = 0;
            for ( ; l >= ADDR_INT(opL); l-- ) {
                c  = (c<<16) + *l;
                c  = c % i;
            }
            k = c;

            /* compute the gcd using Euclids algorithm                     */
            while ( k != 0 ) {
                c = k;
                k = i % k;
                i = c;
            }

            /* now i is the result                                         */
            if ( i == (1<<28) ) {
                gcd = NewBag( T_INTPOS, 4*sizeof(TypDigit) );
                ADDR_INT(gcd)[0] = i;
                ADDR_INT(gcd)[1] = (i>>16);
            }
            else {
                gcd = INTOBJ_INT( i );
            }

        }

    }

    /* return the result                                                   */
    return gcd;
}


/****************************************************************************
**
*F  FuncGcdInt(<self>,<opL>,<opR>)  . . . . . . .  internal function 'GcdInt'
**
**  'FuncGcdInt' implements the internal function 'GcdInt'.
**
**  'GcdInt( <i>, <k> )'
**
**  'Gcd'  returns the greatest common divisor   of the two  integers <m> and
**  <n>, i.e.,  the  greatest integer that  divides  both <m>  and  <n>.  The
**  greatest common divisor is never negative, even if the arguments are.  We
**  define $gcd( m, 0 ) = gcd( 0, m ) = abs( m )$ and $gcd( 0, 0 ) = 0$.
*/
Obj             FuncGcdInt (
    Obj                 self,
    Obj                 opL,
    Obj                 opR )
{
    /* check the arguments                                                 */
    while ( TYPE_OBJ(opL) != T_INT
         && TYPE_OBJ(opL) != T_INTPOS
         && TYPE_OBJ(opL) != T_INTNEG ) {
        opL = ErrorReturnObj(
            "GcdInt: <left> must be an integer (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(opL)].name), 0L,
            "you can return an integer for <left>" );
    }
    while ( TYPE_OBJ(opR) != T_INT
         && TYPE_OBJ(opR) != T_INTPOS
         && TYPE_OBJ(opR) != T_INTNEG ) {
        opR = ErrorReturnObj(
            "GcdInt: <right> must be an integer (not a %s)",
            (Int)(InfoBags[TYPE_OBJ(opR)].name), 0L,
            "you can return an integer for <rigth>" );
    }

    /* return the gcd                                                      */
    return GcdInt( opL, opR );
}


/****************************************************************************
**
*F  InitInt() . . . . . . . . . . . . . . . . initializes the integer package
**
**  'InitInt' initializes the arbitrary size integer package.
*/
void            InitInt ( void )
{
    UInt                t1,  t2;

    /* install the marking functions                                       */
    InfoBags[           T_INT           ].name = "integer";
    InfoBags[           T_INTPOS        ].name = "integer (> 2^28)";
    InitMarkFuncBags(   T_INTPOS        , MarkNoSubBags );
    InfoBags[           T_INTNEG        ].name = "integer (< -2^28)";
    InitMarkFuncBags(   T_INTNEG        , MarkNoSubBags );

    /* install the kind functions                                          */
    InitCopyGVar( GVarName("KIND_INT_SMALL_ZERO"), &KIND_INT_SMALL_ZERO );
    InitCopyGVar( GVarName("KIND_INT_SMALL_POS" ), &KIND_INT_SMALL_POS  );
    InitCopyGVar( GVarName("KIND_INT_SMALL_NEG" ), &KIND_INT_SMALL_NEG  );
    InitCopyGVar( GVarName("KIND_INT_LARGE_POS" ), &KIND_INT_LARGE_POS  );
    InitCopyGVar( GVarName("KIND_INT_LARGE_NEG" ), &KIND_INT_LARGE_NEG  );
    KindObjFuncs[ T_INT    ] = KindIntSmall;
    KindObjFuncs[ T_INTPOS ] = KindIntLargePos;
    KindObjFuncs[ T_INTNEG ] = KindIntLargeNeg;

    /* install the printing function                                       */
    PrintObjFuncs[      T_INT           ] = PrintInt;
    PrintObjFuncs[      T_INTPOS        ] = PrintInt;
    PrintObjFuncs[      T_INTNEG        ] = PrintInt;

    /* install the comparison methods                                      */
    for ( t1 = T_INT; t1 <= T_INTNEG; t1++ ) {
        for ( t2 = T_INT; t2 <= T_INTNEG; t2++ ) {
            EqFuncs  [ t1 ][ t2 ] = EqInt;
            LtFuncs  [ t1 ][ t2 ] = LtInt;
        }
    }

    /* install the unary arithmetic methods                                */
    for ( t1 = T_INT; t1 <= T_INTNEG; t1++ ) {
        ZeroFuncs[ t1 ] = ZeroInt;
        AInvFuncs[ t1 ] = AInvInt;
        OneFuncs [ t1 ] = OneInt;
    }    

    /* install the default product and power methods                       */
    for ( t1 = T_INT; t1 <= T_INTNEG; t1++ ) {
        for ( t2 = FIRST_CONSTANT_TYPE;  t2 <= LAST_CONSTANT_TYPE;  t2++ ) {
            ProdFuncs[ t1 ][ t2 ] = ProdIntObj;
            PowFuncs [ t2 ][ t1 ] = PowObjInt;
        }
        for ( t2 = FIRST_RECORD_TYPE;  t2 <= LAST_RECORD_TYPE;  t2++ ) {
            ProdFuncs[ t1 ][ t2 ] = ProdIntObj;
            PowFuncs [ t2 ][ t1 ] = PowObjInt;
        }
        for ( t2 = FIRST_LIST_TYPE;    t2 <= LAST_LIST_TYPE;    t2++ ) {
            ProdFuncs[ t1 ][ t2 ] = ProdIntObj;
            PowFuncs [ t2 ][ t1 ] = PowObjInt;
        }
        for ( t2 = FIRST_VIRTUAL_TYPE; t2 <= LAST_VIRTUAL_TYPE; t2++ ) {
            ProdFuncs[ t1 ][ t2 ] = ProdIntObj;
            PowFuncs [ t2 ][ t1 ] = PowObjInt;
        }
    }

    /* install the binary arithmetic methods                               */
    for ( t1 = T_INT; t1 <= T_INTNEG; t1++ ) {
        for ( t2 = T_INT; t2 <= T_INTNEG; t2++ ) {
            EqFuncs  [ t1 ][ t2 ] = EqInt;
            LtFuncs  [ t1 ][ t2 ] = LtInt;
            SumFuncs [ t1 ][ t2 ] = SumInt;
            DiffFuncs[ t1 ][ t2 ] = DiffInt;
            ProdFuncs[ t1 ][ t2 ] = ProdInt;
            PowFuncs [ t1 ][ t2 ] = PowInt;
            ModFuncs [ t1 ][ t2 ] = ModInt;
        }
    }

    /* install the internal function                                       */
    IsIntFilt = NewFilterC(
        "IS_INT", 1L, "obj", IsIntHandler );
    AssGVar( GVarName( "IS_INT" ), IsIntFilt );
    AssGVar( GVarName( "QUO_INT" ),
             NewFunctionC( "QUO_INT", 2L, "int1, int2", FuncQuoInt ) );
    AssGVar( GVarName( "REM_INT" ),
             NewFunctionC( "REM_INT", 2L, "int1, int2", FuncRemInt ) );
    AssGVar( GVarName( "GCD_INT" ),
             NewFunctionC( "GCD_INT", 2L, "int1, int2", FuncGcdInt ) );

    ProdIntObjFunc = NewFunctionC(
        "PROD_INT_OBJ", 2L, "n, op", ProdIntObjHandler );
    AssGVar( GVarName( "PROD_INT_OBJ" ), ProdIntObjFunc );

    PowObjIntFunc = NewFunctionC(
        "POW_OBJ_INT", 2L, "op, n", PowObjIntHandler );
    AssGVar( GVarName( "POW_OBJ_INT" ), PowObjIntFunc );
}



