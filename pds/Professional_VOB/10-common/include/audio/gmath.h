#ifndef GMATH_H
#define GMATH_H

/*___________________________________________________________________________
 |                                                                           |
 |   Constants and Globals                                                   |
 |___________________________________________________________________________|
*/
extern l32 Overflow;
extern l32 Carry;


#define MAX_32 (l32)0x7fffffffL
#define MIN_32 (l32)0x80000000L

#define MAX_16 (s16)0x7fff
#define MIN_16 (s16)0x8000


/*___________________________________________________________________________
 |                                                                           |
 |   Operators prototypes                                                    |
 |___________________________________________________________________________|
*/
s16 add(s16 var1, s16 var2);     /* Short add,           1 */
s16 sub(s16 var1, s16 var2);     /* Short sub,           1 */
s16 abs_s(s16 var1);                /* Short abs,           1 */
s16 shl(s16 var1, s16 var2);     /* Short shift left,    1 */
s16 shr(s16 var1, s16 var2);     /* Short shift right,   1 */
s16 mult(s16 var1, s16 var2);    /* Short mult,          1 */
l32 L_mult(s16 var1, s16 var2);  /* Long mult,           1 */
s16 negate(s16 var1);               /* Short negate,        1 */
s16 extract_h(l32 L_var1);          /* Extract high,        1 */
s16 extract_l(l32 L_var1);          /* Extract low,         1 */
s16 round(l32 L_var1);              /* Round,               1 */
l32 L_mac(l32 L_var3, s16 var1, s16 var2); /* Mac,    1 */
l32 L_msu(l32 L_var3, s16 var1, s16 var2); /* Msu,    1 */
l32 L_macNs(l32 L_var3, s16 var1, s16 var2);/*[Mac without sat, 
1*/
l32 L_msuNs(l32 L_var3, s16 var1, s16 var2);/* Msu without sat, 
1*/

l32 L_add(l32 L_var1, l32 L_var2);   /* Long add,        2 */
l32 L_sub(l32 L_var1, l32 L_var2);   /* Long sub,        2 */
l32 L_add_c(l32 L_var1, l32 L_var2); /*Long add with c,  2 */
l32 L_sub_c(l32 L_var1, l32 L_var2); /*Long sub with c,  2 */
l32 L_negate(l32 L_var1);               /* Long negate,     2 */
s16 mult_r(s16 var1, s16 var2);  /* Mult with round,     2 */
l32 L_shl(l32 L_var1, s16 var2); /* Long shift left,     2 */
l32 L_shr(l32 L_var1, s16 var2); /* Long shift right,    2 */
s16 shr_r(s16 var1, s16 var2);/* Shift right with round, 2 */
s16 mac_r(l32 L_var3, s16 var1, s16 var2);/* Mac with rounding, 
2*/
s16 msu_r(l32 L_var3, s16 var1, s16 var2);/* Msu with rounding, 
2*/
l32 L_deposit_h(s16 var1);       /* 16 bit var1 -> MSB,     2 */
l32 L_deposit_l(s16 var1);       /* 16 bit var1 -> LSB,     2 */

l32 L_shr_r(l32 L_var1, s16 var2);/* Long shift right with round, 
 3*/
l32 L_abs(l32 L_var1);            /* Long abs,              3 */

l32 L_sat(l32 L_var1);            /* Long saturation,       4 */

s16 norm_s(s16 var1);             /* Short norm,           15 */

s16 div_s(s16 var1, s16 var2); /* Short division,       18 */

s16 norm_l(l32 L_var1);           /* Long norm,            30 */


/*
   Additional operators
*/
l32 L_mls( l32, s16 ) ;        /* Wght ?? */
s16 div_l( l32, s16 ) ;
s16 i_mult(s16 a, s16 b);

/* Double precision operations */
void  L_Extract(l32 L_32, s16 *hi, s16 *lo);
l32   L_Comp(s16 hi, s16 lo);
l32   Mpy_32(s16 hi1, s16 lo1, s16 hi2, s16 lo2);
l32   Mpy_32_16(s16 hi, s16 lo, s16 n);
l32   Div_32(l32 L_num, s16 denom_hi, s16 denom_lo);

//add two function
d64 inttofloat( //returnvalue: 转换后的浮点数
				 l32  r_iData, //input: Q型整数
				 l32  r_iQ //input: Q位
				 );


l32 floattoint(//output: 转换后的Q型整数
				 d64  r_fData, //input: 浮点数
				 l32  r_iQ //input: Q位
				 );


#endif // endif GMATH_H

