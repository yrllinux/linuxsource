/******************************************************************************
文件名	    ：mpeg4decoptilib.h
相关文件	：
文件实现功能：mepg4解码优化库
作者		：奚荣勇
版本		：1.0
-------------------------------------------------------------------------------
日  期		版本		修改人		走读人    修改内容
2006/08/28	1.0         奚荣勇                  创建
*******************************************************************************/

#ifndef  _MPEG4DECOPTILIB_H_
#define  _MPEG4DECOPTILIB_H_

#include <stdio.h>
#include <string.h>
#include <algorithmtype.h>

#if  defined(MAP_CODE)

/*=====================================================================
函数名       :  MP4IDCT_map
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16* ps16Block   指向8×8模块
返回值说明   ： 无
特殊说明     ： pBlock地址需要32位对齐
======================================================================*/
void MP4IDCT_map(s16* ps16Block);

/*=====================================================================
函 数 名： McCopy16to8_map
功    能： 
算法实现： 无
全局变量： 无
参    数： 
返 回 值： 无
======================================================================*/
void McCopy16to8_map(u8 *const pu8Dst, const s16 *const ps16Src, u32 u32Stride);

/*====================================================================
函 数 名： McCopy8x8_map
功    能： 
算法实现： 无
全局变量： 无
参    数： 
返 回 值： 无
====================================================================*/
void McCopy8x8_map(u8 *const pu8Dst, const u8 *const pu8Src, const u32 u32Stride);

/*====================================================================
函 数 名： McCopy8x4_map
功    能： 
算法实现： 无
全局变量： 无
参    数： 
返 回 值： 无
====================================================================*/
void McCopy8x4_map(u8 *const pu8Dst, const u8 *const pu8Src, const u32 u32Stride);

/*====================================================================
函 数 名： McAdd16to8_map
功    能： 
算法实现： 无
全局变量： 无
参    数： 
返 回 值： 无
=====================================================================*/
void McAdd16to8_map(u8 *const pu8Dst, const s16 *const ps16Src, u32 u32Stride);

/*====================================================================
函 数 名： DequantH263Inter_map
功    能： 8x8块inter块反量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32QP            量化步长
返 回 值： 无 
=====================================================================*/
void DequantH263Inter_map(s16* ps16DCTOut, s16* ps16DCTIn, l32 l32QP);

/*=====================================================================
函 数 名： DequantH263Intra_map
功    能： 8x8块intra块反量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32DCScaler      DC系数量化精度
           l32QP            量化步长
返 回 值： 无 
=====================================================================*/
void DequantH263Intra_map(s16* ps16DCTOut, s16* ps16DCTIn, l32 l32QP, l32 l32DCScaler);

/*====================================================================
函 数 名： DequantMpegInter_map
功    能： 8x8块inter块反量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32QP            量化步长
返 回 值： 无 
====================================================================*/
void DequantMpegInter_map(s16 *const ps16Data, const s16 *ps16Coeff, const u32 u32Quant);

/*=====================================================================
函 数 名： DequantMpegIntra_map
功    能： 8x8块intra块反量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32DCScaler      DC系数量化精度
           l32QP            量化步长
返 回 值： 无 
=====================================================================*/
void DequantMpegIntra_map(s16 *ps16Data, const s16 *ps16Coeff, const u32 u32Quant, const l32 l32DcScalar);

/*====================================================================
函数名       : Interpolate8x8H_map                            
功能         : 8x8块的水平方向2Tap插值                        
引用全局变量 : 无                                             
输入参数说明 :  const u8   *pu8Src        :源块指针           
                 u16        u16SrcStride  :源块步长           
                 u8        *pu8Dst        :目标块指针         
                 u16        u16DstStride  :目标块步长         
                 BOOL        bRounding   :是否进行四舍五入    
返回值说明   : 无                                             
特殊说明     : 无                                             
=====================================================================*/
void Interpolate8x8H_map(u8 *const pu8Dst, const u8 *pu8Src, const u32 u32Stride, const u32 u32Rounding);

/*====================================================================
函数名       : Interpolate8x8V_map
功能         : 8x8块的垂直方向2Tap插值
引用全局变量 : 无
输入参数说明 :  const u8   *pu8Src        :源块指针
                 u16        u16SrcStride  :源块步长
                 u8        *pu8Dst        :目标块指针
                 u16        u16DstStride  :目标块步长
                 BOOL        bRounding   :是否进行四舍五入
返回值说明   : 无
特殊说明     : 无
======================================================================*/
void Interpolate8x8V_map(u8 *const pu8Dst, const u8 *pu8Src, const u32 u32Stride, const u32 u32Rounding);

/*====================================================================
函数名       : Interpolate8x8HV_map
功能         : 8x8块的斜角方向2Tap插值
引用全局变量 : 无
输入参数说明 :  const u8   *pu8Src        :源块指针
                 u16        u16SrcStride  :源块步长
                 u8        *pu8Dst        :目标块指针
                 u16        u16DstStride  :目标块步长
                 BOOL        bRounding   :是否进行四舍五入
返回值说明   : 无
特殊说明     : 无
======================================================================*/
void Interpolate8x8HV_map(u8 *const pu8Dst, const u8 *pu8Src, const u32 u32Stride, const u32 u32Rounding);

/*====================================================================
函数名       : Interpolate8x4H_map                            
功能         : 8x4块的水平方向2Tap插值                        
引用全局变量 : 无                                             
输入参数说明 :  const u8   *pu8Src        :源块指针           
                 u16        u16SrcStride  :源块步长           
                 u8        *pu8Dst        :目标块指针         
                 u16        u16DstStride  :目标块步长         
                 BOOL        bRounding   :是否进行四舍五入    
返回值说明   : 无                                             
特殊说明     : 无                                             
=====================================================================*/
void Interpolate8x4H_map(u8 *const pu8Dst, const u8 *pu8Src, const u32 u32Stride, const u32 u32Rounding);

/*====================================================================
函数名       : Interpolate8x4V_map
功能         : 8x4块的垂直方向2Tap插值
引用全局变量 : 无
输入参数说明 :  const u8   *pu8Src        :源块指针
                 u16        u16SrcStride  :源块步长
                 u8        *pu8Dst        :目标块指针
                 u16        u16DstStride  :目标块步长
                 BOOL        bRounding   :是否进行四舍五入
返回值说明   : 无
特殊说明     : 无
======================================================================*/
void Interpolate8x4V_map(u8 *const pu8Dst, const u8 *pu8Src, const u32 u32Stride, const u32 u32Rounding);

/*====================================================================
函数名       : Interpolate8x4HV_map
功能         : 8x4块的斜角方向2Tap插值
引用全局变量 : 无
输入参数说明 :  const u8   *pu8Src        :源块指针
                 u16        u16SrcStride  :源块步长
                 u8        *pu8Dst        :目标块指针
                 u16        u16DstStride  :目标块步长
                 BOOL        bRounding   :是否进行四舍五入
返回值说明   : 无
特殊说明     : 无
======================================================================*/
void Interpolate8x4HV_map(u8 *const pu8Dst, const u8 *pu8Src, const u32 u32Stride, const u32 u32Rounding);

#define MP4IDCT MP4IDCT_map

#define McCopy16to8 McCopy16to8_map

#define McCopy8x8 McCopy8x8_map

#define McCopy8x4 McCopy8x4_map

#define McAdd16to8 McAdd16to8_map

#define DequantH263Inter DequantH263Inter_map

#define DequantH263Intra DequantH263Intra_map

#define DequantMpegInter DequantMpegInter_map

#define DequantMpegIntra DequantMpegIntra_map

#define Interpolate8x8H Interpolate8x8H_map

#define Interpolate8x8V Interpolate8x8V_map

#define Interpolate8x8HV Interpolate8x8HV_map

#define Interpolate8x4H Interpolate8x4H_map

#define Interpolate8x4V Interpolate8x4V_map

#define Interpolate8x4HV Interpolate8x4HV_map

#elif defined(C_CODE)

/*=====================================================================
函数名       :  MP4IDCT_c
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16* ps16Block   指向8×8模块
返回值说明   ： 无
特殊说明     ： pBlock地址需要32位对齐
======================================================================*/
void MP4IDCT_c(s16* ps16Block);

/*=====================================================================
函 数 名： McCopy16to8_c
功    能： 
算法实现： 无
全局变量： 无
参    数： 
返 回 值： 无
======================================================================*/
void McCopy16to8_c(u8 *const pu8Dst, const s16 *const ps16Src, u32 u32Stride);

/*====================================================================
函 数 名： McCopy8x8_c
功    能： 
算法实现： 无
全局变量： 无
参    数： 
返 回 值： 无
====================================================================*/
void McCopy8x8_c(u8 *const pu8Dst, const u8 *const pu8Src, const u32 u32Stride);

/*====================================================================
函 数 名： McCopy8x4_c
功    能： 
算法实现： 无
全局变量： 无
参    数： 
返 回 值： 无
====================================================================*/
void McCopy8x4_c(u8 *const pu8Dst, const u8 *const pu8Src, const u32 u32Stride);

/*====================================================================
函 数 名： McAdd16to8_c
功    能： 
算法实现： 无
全局变量： 无
参    数： 
返 回 值： 无
=====================================================================*/
void McAdd16to8_c(u8 *const pu8Dst, const s16 *const ps16Src, u32 u32Stride);

/*====================================================================
函 数 名： DequantH263Inter_c
功    能： 8x8块inter块反量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32QP            量化步长
返 回 值： 无 
=====================================================================*/
void DequantH263Inter_c(s16* ps16DCTOut, s16* ps16DCTIn, l32 l32QP);

/*=====================================================================
函 数 名： DequantH263Intra_c
功    能： 8x8块intra块反量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32DCScaler      DC系数量化精度
           l32QP            量化步长
返 回 值： 无 
=====================================================================*/
void DequantH263Intra_c(s16* ps16DCTOut, s16* ps16DCTIn, l32 l32QP, l32 l32DCScaler);

/*====================================================================
函 数 名： DequantMpegInter_c
功    能： 8x8块inter块反量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32QP            量化步长
返 回 值： 无 
====================================================================*/
void DequantMpegInter_c(s16 *const ps16Data, const s16 *ps16Coeff, const u32 u32Quant);

/*=====================================================================
函 数 名： DequantMpegIntra_c
功    能： 8x8块intra块反量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32DCScaler      DC系数量化精度
           l32QP            量化步长
返 回 值： 无 
=====================================================================*/
void DequantMpegIntra_c(s16 *ps16Data, const s16 *ps16Coeff, const u32 u32Quant, const l32 l32DcScalar);

/*====================================================================
函数名       : Interpolate8x8H_c                            
功能         : 8x8块的水平方向2Tap插值                        
引用全局变量 : 无                                             
输入参数说明 :  const u8   *pu8Src        :源块指针           
                 u16        u16SrcStride  :源块步长           
                 u8        *pu8Dst        :目标块指针         
                 u16        u16DstStride  :目标块步长         
                 BOOL        bRounding   :是否进行四舍五入    
返回值说明   : 无                                             
特殊说明     : 无                                             
=====================================================================*/
void Interpolate8x8H_c(u8 *const pu8Dst, const u8 *pu8Src, const u32 u32Stride, const u32 u32Rounding);

/*====================================================================
函数名       : Interpolate8x8V_c
功能         : 8x8块的垂直方向2Tap插值
引用全局变量 : 无
输入参数说明 :  const u8   *pu8Src        :源块指针
                 u16        u16SrcStride  :源块步长
                 u8        *pu8Dst        :目标块指针
                 u16        u16DstStride  :目标块步长
                 BOOL        bRounding   :是否进行四舍五入
返回值说明   : 无
特殊说明     : 无
======================================================================*/
void Interpolate8x8V_c(u8 *const pu8Dst, const u8 *pu8Src, const u32 u32Stride, const u32 u32Rounding);

/*====================================================================
函数名       : Interpolate8x8HV_c
功能         : 8x8块的斜角方向2Tap插值
引用全局变量 : 无
输入参数说明 :  const u8   *pu8Src        :源块指针
                 u16        u16SrcStride  :源块步长
                 u8        *pu8Dst        :目标块指针
                 u16        u16DstStride  :目标块步长
                 BOOL        bRounding   :是否进行四舍五入
返回值说明   : 无
特殊说明     : 无
======================================================================*/
void Interpolate8x8HV_c(u8 *const pu8Dst, const u8 *pu8Src, const u32 u32Stride, const u32 u32Rounding);

/*====================================================================
函数名       : Interpolate8x4H_c                            
功能         : 8x4块的水平方向2Tap插值                        
引用全局变量 : 无                                             
输入参数说明 :  const u8   *pu8Src        :源块指针           
                 u16        u16SrcStride  :源块步长           
                 u8        *pu8Dst        :目标块指针         
                 u16        u16DstStride  :目标块步长         
                 BOOL        bRounding   :是否进行四舍五入    
返回值说明   : 无                                             
特殊说明     : 无                                             
=====================================================================*/
void Interpolate8x4H_c(u8 *const pu8Dst, const u8 *pu8Src, const u32 u32Stride, const u32 u32Rounding);

/*====================================================================
函数名       : Interpolate8x4V_c
功能         : 8x4块的垂直方向2Tap插值
引用全局变量 : 无
输入参数说明 :  const u8   *pu8Src        :源块指针
                 u16        u16SrcStride  :源块步长
                 u8        *pu8Dst        :目标块指针
                 u16        u16DstStride  :目标块步长
                 BOOL        bRounding   :是否进行四舍五入
返回值说明   : 无
特殊说明     : 无
======================================================================*/
void Interpolate8x4V_c(u8 *const pu8Dst, const u8 *pu8Src, const u32 u32Stride, const u32 u32Rounding);

/*====================================================================
函数名       : Interpolate8x4HV_c
功能         : 8x4块的斜角方向2Tap插值
引用全局变量 : 无
输入参数说明 :  const u8   *pu8Src        :源块指针
                 u16        u16SrcStride  :源块步长
                 u8        *pu8Dst        :目标块指针
                 u16        u16DstStride  :目标块步长
                 BOOL        bRounding   :是否进行四舍五入
返回值说明   : 无
特殊说明     : 无
======================================================================*/
void Interpolate8x4HV_c(u8 *const pu8Dst, const u8 *pu8Src, const u32 u32Stride, const u32 u32Rounding);

#define MP4IDCT MP4IDCT_c

#define McCopy16to8 McCopy16to8_c

#define McCopy8x8 McCopy8x8_c

#define McCopy8x4 McCopy8x4_c

#define McAdd16to8 McAdd16to8_c

#define DequantH263Inter DequantH263Inter_c

#define DequantH263Intra DequantH263Intra_c

#define DequantMpegInter DequantMpegInter_c

#define DequantMpegIntra DequantMpegIntra_c

#define Interpolate8x8H Interpolate8x8H_c

#define Interpolate8x8V Interpolate8x8V_c

#define Interpolate8x8HV Interpolate8x8HV_c

#define Interpolate8x4H Interpolate8x4H_c

#define Interpolate8x4V Interpolate8x4V_c

#define Interpolate8x4HV Interpolate8x4HV_c

#endif

#endif
