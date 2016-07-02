/*****************************************************************************
模   块   名: gmath
文   件   名: gmath10.h
相 关  文 件: 
文件实现功能: gmath库实现的源码头文件(库为g.723及g.729的库所调用)
版        本: V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
 2005/04/19      1.0         徐  超      创    建
 2005/05/08      1.0         王水晶      全局变量Overflow and Carry参数化
 2005/05/21      1.0         王水晶      s32---l32
******************************************************************************/
#ifndef GMATH_H
#define GMATH_H


#include "algorithmtype.h"
/*___________________________________________________________________________
 |                                                                           |
 |   Constants and Globals                                                   |
 |___________________________________________________________________________|
*/
//extern l32 Overflow;
//extern l32 Carry;


#define MAX_32 (l32)0x7fffffffL
#define MIN_32 (l32)0x80000000L

#define MAX_16 (s16)0x7fff
#define MIN_16 (s16)0x8000


/*___________________________________________________________________________
 |                                                                           |
 |   Operators prototypes                                                    |
 |___________________________________________________________________________|
*/
//s16 add(s16 var1, s16 var2);     /* Short add,           1 */
//s16 sub(s16 var1, s16 var2);     /* Short sub,           1 */
s16 abs_s(s16 var1);                /* Short abs,           1 */
//s16 shl(s16 var1, s16 var2);     /* Short shift left,    1 */
//s16 shr(s16 var1, s16 var2);     /* Short shift right,   1 */
//s16 mult(s16 var1, s16 var2);    /* Short mult,          1 */
//l32 L_mult(s16 var1, s16 var2);  /* Long mult,           1 */
s16 negate(s16 var1);               /* Short negate,        1 */
s16 extract_h(l32 L_var1);          /* Extract high,        1 */
s16 extract_l(l32 L_var1);          /* Extract low,         1 */
//s16 round(l32 L_var1);              /* Round,               1 */
//l32 L_mac(l32 L_var3, s16 var1, s16 var2); /* Mac,    1 */
//l32 L_msu(l32 L_var3, s16 var1, s16 var2); /* Msu,    1 */
//l32 L_macNs(l32 L_var3, s16 var1, s16 var2);/*[Mac without sat,1*/
//l32 L_msuNs(l32 L_var3, s16 var1, s16 var2);/* Msu without sat,1*/

//l32 L_add(l32 L_var1, l32 L_var2);   /* Long add,        2 */
//l32 L_sub(l32 L_var1, l32 L_var2);   /* Long sub,        2 */
//l32 L_add_c(l32 L_var1, l32 L_var2); /*Long add with c,  2 */
//l32 L_sub_c(l32 L_var1, l32 L_var2); /*Long sub with c,  2 */
l32 L_negate(l32 L_var1);               /* Long negate,     2 */
//s16 mult_r(s16 var1, s16 var2);  /* Mult with round,     2 */
//l32 L_shl(l32 L_var1, s16 var2); /* Long shift left,     2 */
//l32 L_shr(l32 L_var1, s16 var2); /* Long shift right,    2 */
//s16 shr_r(s16 var1, s16 var2);/* Shift right with round, 2 */
//s16 mac_r(l32 L_var3, s16 var1, s16 var2);/* Mac with rounding,2*/
//s16 msu_r(l32 L_var3, s16 var1, s16 var2);/* Msu with rounding,2*/
l32 L_deposit_h(s16 var1);       /* 16 bit var1 -> MSB,     2 */
l32 L_deposit_l(s16 var1);       /* 16 bit var1 -> LSB,     2 */

//l32 L_shr_r(l32 L_var1, s16 var2);/* Long shift right with round, 3*/
l32 L_abs(l32 L_var1);            /* Long abs,              3 */

//l32 L_sat(l32 L_var1);            /* Long saturation,       4 */

s16 norm_s(s16 var1);             /* Short norm,           15 */

//s16 div_s(s16 var1, s16 var2); /* Short division,       18 */

s16 norm_l(l32 L_var1);           /* Long norm,            30 */


/*
   Additional operators
*/
//l32 L_mls( l32, s16 ) ;        /* Wght ?? */
//s16 div_l( l32, s16 ) ;
s16 i_mult(s16 a, s16 b);

/* Double precision operations */
//void  L_Extract(l32 L_32, s16 *hi, s16 *lo);
//l32   L_Comp(s16 hi, s16 lo);
//l32   Mpy_32(s16 hi1, s16 lo1, s16 hi2, s16 lo2);
//l32   Mpy_32_16(s16 hi, s16 lo, s16 n);
//l32   Div_32(l32 L_num, s16 denom_hi, s16 denom_lo);

//add two function
d64 inttofloat( //returnvalue: 转换后的浮点数
				 l32  r_iData, //input: Q型整数
				 l32  r_iQ //input: Q位
				 );


l32 floattoint(//output: 转换后的Q型整数
				 d64  r_fData, //input: 浮点数
				 l32  r_iQ //input: Q位
				 );

// 全局变量Overflow and Carry参数化
s16 add(s16 var1,s16 var2,l32* pOverflow); 
s16 sub(s16 var1,s16 var2,l32* pOverflow);
s16 shl(s16 var1,s16 var2,l32* pOverflow);
s16 shr(s16 var1,s16 var2,l32* pOverflow);
s16 mult(s16 var1, s16 var2, l32* pOverflow);
s16 round(l32 L_var1, l32* pOverflow);
l32 L_mult(s16	var1,s16 var2,l32* pOverflow);
l32 L_mac(l32 L_var3, s16 var1, s16 var2, l32* pOverflow);
l32 L_msu(l32 L_var3, s16 var1, s16 var2, l32* pOverflow);
l32 L_macNs(l32 L_var3, s16 var1, s16 var2, l32* pOverflow, l32* pCarry);
l32 L_msuNs(l32 L_var3, s16 var1, s16 var2, l32* pOverflow, l32* pCarry);
l32 L_add(l32 L_var1, l32 L_var2, l32* pOverflow);
l32 L_sub(l32 L_var1, l32 L_var2, l32* pOverflow);
l32 L_add_c(l32 L_var1, l32 L_var2, l32* pOverflow, l32* pCarry);
l32 L_sub_c(l32 L_var1, l32 L_var2, l32* pOverflow, l32* pCarry);
s16 mult_r(s16 var1, s16 var2, l32* pOverflow);
l32 L_shl(l32 L_var1, s16 var2, l32* pOverflow);
l32 L_shr(l32 L_var1, s16 var2, l32* pOverflow);

s16 shr_r(s16 var1, s16 var2, l32* pOverflow);
s16 mac_r(l32 L_var3, s16 var1, s16 var2, l32* pOverflow);
s16 msu_r(l32 L_var3, s16 var1, s16 var2, l32* pOverflow);
l32 L_shr_r(l32 L_var1,s16 var2,l32* pOverflow);
l32 L_sat (l32 L_var1, l32* pOverflow, l32* pCarry);
s16 div_s(s16 var1, s16 var2, l32* pOverflow);
s16 div_l( l32  L_num, s16 den, l32* pOverflow);


l32 L_mls( l32 Lv, s16 v, l32* pOverflow);

l32 Mpy_32_16(s16 hi, s16 lo, s16 n, l32* pOverflow);
l32 Mpy_32(s16 hi1, s16 lo1, s16 hi2, s16 lo2, l32* pOverflow);
void L_Extract(l32 L_32, s16 *hi, s16 *lo, l32* pOverflow);
l32 Div_32(l32 L_num, s16 denom_hi, s16 denom_lo, l32* pOverflow);
l32 L_Comp(s16 hi, s16 lo, l32* pOverflow);

#endif // endif GMATH_H

