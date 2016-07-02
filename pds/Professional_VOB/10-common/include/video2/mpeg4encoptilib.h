/******************************************************************************
文件名	    ：mpeg4encoptilib.h
相关文件	：
文件实现功能：mepg4编码优化库
作者		：奚荣勇
版本		：1.0
-------------------------------------------------------------------------------
日  期		版本		修改人		走读人    修改内容
2006/08/28	1.0         奚荣勇                  创建
*******************************************************************************/

#ifndef  _MPEG4ENCOPTILIB_H_
#define  _MPEG4ENCOPTILIB_H_

#include <stdio.h>
#include <string.h>
#include <algorithmtype.h>

#if  defined(MAP_CODE)

/*=====================================================================
函数名       :  MP4PixAvgH8x8_map
功能         ： 源宏块的水平方向插值
引用全局变量 ： 无
输入参数说明 ：  pu8Cur        ：源宏块指针
                 l32CurStride  ：源宏块步长
                 pu8Dst        ：目标宏块指针
                 l32DstStride  ：目标宏块步长
                 l32Width      ：宽度（为8的倍数）
                 l32Height     ：高度
返回值说明   ： 无
特殊说明     ： 无
======================================================================*/
void MP4PixAvgH8x8_map(const u8* pu8Cur, l32 l32CurStride, u8* pu8Dst, l32 l32Rouding);

/*=====================================================================
函数名       :  MP4PixAvgV8x8_map
功能         ： 源宏块的垂直方向插值
引用全局变量 ： 无
输入参数说明 ：  pu8Cur        ：源宏块指针
                 l32CurStride  ：源宏块步长
                 pu8Dst        ：目标宏块指针
                 l32DstStride  ：目标宏块步长
                 l32Width      ：宽度（为8的倍数）
                 l32Height     ：高度
返回值说明   ： 无
特殊说明     ： 无
======================================================================*/
void MP4PixAvgV8x8_map(const u8* pu8Cur, l32 l32CurStride, u8* pu8Dst, l32 l32Rouding);

/*=====================================================================
函数名       :  MP4PixAvgC8x8_map
功能         ： 源宏块的中心方向插值
引用全局变量 ： 无
输入参数说明 ：  pu8Cur        ：源宏块指针
                 l32CurStride  ：源宏块步长
                 pu8Dst        ：目标宏块指针
                 l32DstStride  ：目标宏块步长
                 l32Width      ：宽度（为8的倍数）
                 l32Height     ：高度
返回值说明   ： 无
特殊说明     ： 无
======================================================================*/
void MP4PixAvgC8x8_map(const u8* pu8Cur, l32 l32CurStride, u8* pu8Dst, l32 l32Rouding);

/*=====================================================================
函数名       :  MP4DCT_map
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16* ps16Block   指向8×8模块
返回值说明   ： 无
特殊说明     ： ps16Block地址需要32位对齐
======================================================================*/
void MP4DCT_map(s16* ps16Block);

/*====================================================================
函数名       :  MP4IDCT_map
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16* ps16Block   指向8×8模块
返回值说明   ： 无
特殊说明     ： pBlock地址需要32位对齐
======================================================================*/
void MP4IDCT_map(s16* ps16Block);

/*=====================================================================
函 数 名： MP4AddiDCT8x8_map                                           
功    能： 8x8反DCT变换                                                
算法实现： 无                                                          
全局变量： 无                                                          
参    数： ps16Dct        DCT变换系数                                  
           pu8Ref         预测图像                                     
           pu8Rec         目的图像                                     
返 回 值： 无                                                          
======================================================================*/
void MP4AddiDCT8x8_map(s16* ps16DCT, u8* pu8Rec, l32 l32RecStride, u8* pu8Ref, l32 l32RefStride);

/*=====================================================================
函 数 名： MP4SubDCT8x8_map
功    能： 8x8块DCT
算法实现： 无
全局变量： 无
参    数： ps16Dct        DCT变换系数
           pu8Src         原始图像
           pu8Dst         目的图像
返 回 值： 无 
=====================================================================*/
void MP4SubDCT8x8_map(s16* ps16DCT, u8* pu8Src, l32 l32SrcStride, u8* pu8Dst, l32 l32DstStride);

/*=====================================================================
函数名       :  MP4Deviation16x16_map
功能         ： 求宏块内各象素值与平均值差的绝对值之和
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8MB     ：指向宏块指针
                 l32       l32Stride  ：宏块步长

返回值说明   ： 宏块内各象素值与平均值差的绝对值之和
特殊说明     ： 无
=====================================================================*/
u32 MP4Deviation16x16_map(const u8 *pu8Cur, l32 l32Stride);

/*====================================================================
函数名       :  MP4MemCopy8x8_map
功能         :  8x8块数据拷贝 
引用全局变量 :  无
输入参数说明 :  u8  *pu8Src        ：源块指针
                l32  l32SrcStride  ：源块步长
                u8  *pu8Dst        ：目标块指针
                l32  l32DstStride  ：目标块步长
返回值说明   :  无
特殊说明     :  无
====================================================================*/
void MP4MemCopy8x8_map(const u8 *pu8Src, l32 l32SrcStride, u8 *pu8Dst, l32 l32DstStride);

/*====================================================================
函 数 名： QuantIntra8x8_c
功    能： 8x8块intra块量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32DCScaler      DC系数量化精度
           l32QP            量化步长
返 回 值： 无 
====================================================================*/
void MP4QuantIntra8x8_map(s16* ps16DCTIn, s16* ps16DCTOut, l32 l32DCScaler, l32 l32QP);

/*====================================================================
函 数 名： MP4QuantInter8x8_map
功    能： 8x8块inter块量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32QP            量化步长
返 回 值： 无 
====================================================================*/
l32 MP4QuantInter8x8_map(s16* ps16DCTIn, s16* ps16DCTOut, l32 l32QP);

/*====================================================================
函 数 名： MP4DeQuantInter8x8_map
功    能： 8x8块inter块反量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32QP            量化步长
返 回 值： 无 
====================================================================*/
void MP4DeQuantInter8x8_map(s16* ps16DCTIn, s16* ps16DCTOut, l32 l32QP);

/*====================================================================
函 数 名： MP4DeQuantIntra8x8_map
功    能： 8x8块intra块反量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32DCScaler      DC系数量化精度
           l32QP            量化步长
返 回 值： 无 
====================================================================*/
void MP4DeQuantIntra8x8_map(s16* ps16DCTIn, s16* ps16DCTOut,  l32 l32DCScaler, l32 l32QP);

/*=============================================================================
函 数 名： MP4AddiDCTIQuant8x8_map
功    能： 16x16反DCT变换，Dequant
算法实现： 无
全局变量： 无 
参    数： ps16DCTIn     ：输入系数指针
           ps16DCTOut    ：输出系数指针
           pu8Rec        : 重建块存放地址
           l32RecStride  : 重建块步长
           pu8Ref        ：参考图像指针
           l32RefStride  ：参考图像步长
           l32Qp         ：量化系数
           l32IsIntra    ：是否为Intra块(Intra : 1; Inter : 0)
           l32Cbp        ：当前块是否是全0（0表示是全0块，否则不是全0块）
           l32DcScaler   ：DC系数的量化精度(Intra块才会用到)
返 回 值： 无
=============================================================================*/
void MP4AddiDCTIQuant8x8_map(s16* ps16DCTIn, s16* ps16DCTOut, l32 l32QP, u8* pu8Rec, l32 l32RecStride, u8* pu8Ref, l32 l32RefStride, l32 l32IsIntra, l32 l32Cbp, l32 l32DCSalar);

/*=============================================================================
函 数 名： MP4SubDCTQuant8x8_c
功    能： 8x8块残差，DCT和Quant
算法实现： 无
全局变量： 无 
参    数： ps16Dct        DCT变换buffer
           pu8Src         原始图像
           l32SrcStride   原始图像步长
           pu8Dst         参考图像
           l32DstStride   参考图像步长
           ps16DctOut     量化后输出buffer
           l32Qp          量化值
           l32IsIntra     当前块是否是Intra块
           l32DCScaler    直流系数量化值
返 回 值： 无
=============================================================================*/
l32 MP4SubDCTQuant8x8_map(s16* ps16DCT, u8* pu8Src, l32 l32SrcStride, u8* pu8Dst, l32 l32DstStride, s16* ps16DCTOut, l32 l32Qp, l32 l32IsIntra, l32 l32DCScalar);

/*=============================================================================
函 数 名: MP4Interpolate8x8v_map
功    能: 对参考图像进行水平方向半象素插值
算法实现: 无
全局变量: 无
参    数: pu8Dst              源指针(8字节对齐)
		  pu8Src              目标指针
		  s16Sride            图像步长
返 回 值: 无 
=============================================================================*/
void MP4Interpolate8x8v_map(u8 *pu8Dst, s16 s16DstStride, u8 *pu8Src, s16 s16Stride, u32 u32Num, l32 l32Rouding);

/*=============================================================================
函 数 名: MP4Interpolate16x16v_map
功    能: 对参考图像进行水平方向半象素插值
算法实现: 无
全局变量: 无
参    数: pu8Dst              源指针(8字节对齐)
		  pu8Src              目标指针
		  s16Sride            图像步长
返 回 值: 无 
=============================================================================*/
void MP4Interpolate16x16v_map(u8 *pu8Dst, s16 s16DstStride, u8 *pu8Src, s16 s16Stride, u32 u32Num, l32 l32Rouding);

/*=============================================================================
函 数 名: MP4Interpolate8x8h_map
功    能: 对参考图像进行水平方向半象素八抽头插值
算法实现: 无
全局变量: 无
参    数: pu8Dst              源指针(8字节对齐)
		  pu8Src              目标指针
		  s16Sride            图像步长
返 回 值: 无 
=============================================================================*/
void MP4Interpolate8x8h_map(u8 *pu8Dst, s16 s16DstStride, u8 *pu8Src, s16 s16SrcStride, u32 u32Num, l32 l32Rouding);

/*=============================================================================
函 数 名: MP4Interpolate16x16h_map
功    能: 对参考图像进行水平方向半象素八抽头插值
算法实现: 无
全局变量: 无
参    数: pu8Dst              源指针(8字节对齐)
		  pu8Src              目标指针
		  s16Sride            图像步长
返 回 值: 无 
=============================================================================*/
void MP4Interpolate16x16h_map(u8 *pu8Dst, s16 s16DstStride, u8 *pu8Src, s16 s16SrcStride, u32 u32Num, l32 l32Rouding);

#define MP4PixAvgH8x8 MP4PixAvgH8x8_map

#define MP4PixAvgV8x8 MP4PixAvgV8x8_map

#define MP4PixAvgC8x8 MP4PixAvgC8x8_map

#define MP4DCT MP4DCT_map

#define MP4IDCT MP4IDCT_map

#define MP4AddiDCT8x8 MP4AddiDCT8x8_map

#define MP4SubDCT8x8 MP4SubDCT8x8_map

#define MP4Deviation16x16 MP4Deviation16x16_map

#define MP4MemCopy8x8 MP4MemCopy8x8_map

#define MP4QuantIntra8x8 MP4QuantIntra8x8_map

#define MP4QuantInter8x8 MP4QuantInter8x8_map

#define MP4DeQuantInter8x8 MP4DeQuantInter8x8_map

#define MP4DeQuantIntra8x8 MP4DeQuantIntra8x8_map

#define MP4AddiDCTIQuant8x8 MP4AddiDCTIQuant8x8_map

#define MP4SubDCTQuant8x8 MP4SubDCTQuant8x8_map

#define MP4Interpolate8x8v MP4Interpolate8x8v_map

#define MP4Interpolate16x16v MP4Interpolate16x16v_map

#define MP4Interpolate8x8h MP4Interpolate8x8h_map

#define MP4Interpolate16x16h MP4Interpolate16x16h_map

#elif defined(C_CODE)

/*=====================================================================
函数名       :  MP4PixAvgH8x8_c
功能         ： 源宏块的水平方向插值
引用全局变量 ： 无
输入参数说明 ：  pu8Cur        ：源宏块指针
                 l32CurStride  ：源宏块步长
                 pu8Dst        ：目标宏块指针
                 l32DstStride  ：目标宏块步长
                 l32Width      ：宽度（为8的倍数）
                 l32Height     ：高度
返回值说明   ： 无
特殊说明     ： 无
======================================================================*/
void MP4PixAvgH8x8_c(const u8* pu8Cur, l32 l32CurStride, u8* pu8Dst, l32 l32Rouding);

/*=====================================================================
函数名       :  MP4PixAvgV8x8_c
功能         ： 源宏块的垂直方向插值
引用全局变量 ： 无
输入参数说明 ：  pu8Cur        ：源宏块指针
                 l32CurStride  ：源宏块步长
                 pu8Dst        ：目标宏块指针
                 l32DstStride  ：目标宏块步长
                 l32Width      ：宽度（为8的倍数）
                 l32Height     ：高度
返回值说明   ： 无
特殊说明     ： 无
======================================================================*/
void MP4PixAvgV8x8_c(const u8* pu8Cur, l32 l32CurStride, u8* pu8Dst, l32 l32Rouding);

/*=====================================================================
函数名       :  MP4PixAvgC8x8_c
功能         ： 源宏块的中心方向插值
引用全局变量 ： 无
输入参数说明 ：  pu8Cur        ：源宏块指针
                 l32CurStride  ：源宏块步长
                 pu8Dst        ：目标宏块指针
                 l32DstStride  ：目标宏块步长
                 l32Width      ：宽度（为8的倍数）
                 l32Height     ：高度
返回值说明   ： 无
特殊说明     ： 无
======================================================================*/
void MP4PixAvgC8x8_c(const u8* pu8Cur, l32 l32CurStride, u8* pu8Dst, l32 l32Rouding);

/*=====================================================================
函数名       :  MP4DCT_c
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16* ps16Block   指向8×8模块
返回值说明   ： 无
特殊说明     ： ps16Block地址需要32位对齐
======================================================================*/
void MP4DCT_c(s16* ps16Block);

/*====================================================================
函数名       :  MP4IDCT_c
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16* ps16Block   指向8×8模块
返回值说明   ： 无
特殊说明     ： pBlock地址需要32位对齐
======================================================================*/
void MP4IDCT_c(s16* ps16Block);

/*=====================================================================
函 数 名： MP4AddiDCT8x8_c                                           
功    能： 8x8反DCT变换                                                
算法实现： 无                                                          
全局变量： 无                                                          
参    数： ps16Dct        DCT变换系数                                  
           pu8Ref         预测图像                                     
           pu8Rec         目的图像                                     
返 回 值： 无                                                          
======================================================================*/
void MP4AddiDCT8x8_c(s16* ps16DCT, u8* pu8Rec, l32 l32RecStride, u8* pu8Ref, l32 l32RefStride);

/*=====================================================================
函 数 名： MP4SubDCT8x8_c
功    能： 8x8块DCT
算法实现： 无
全局变量： 无
参    数： ps16Dct        DCT变换系数
           pu8Src         原始图像
           pu8Dst         目的图像
返 回 值： 无 
=====================================================================*/
void MP4SubDCT8x8_c(s16* ps16DCT, u8* pu8Src, l32 l32SrcStride, u8* pu8Dst, l32 l32DstStride);

/*=====================================================================
函数名       :  MP4Deviation16x16_c
功能         ： 求宏块内各象素值与平均值差的绝对值之和
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8MB     ：指向宏块指针
                 l32       l32Stride  ：宏块步长

返回值说明   ： 宏块内各象素值与平均值差的绝对值之和
特殊说明     ： 无
=====================================================================*/
u32 MP4Deviation16x16_c(const u8 *pu8Cur, l32 l32Stride);

/*====================================================================
函数名       :  MP4MemCopy8x8_c
功能         :  8x8块数据拷贝 
引用全局变量 :  无
输入参数说明 :  u8  *pu8Src        ：源块指针
                l32  l32SrcStride  ：源块步长
                u8  *pu8Dst        ：目标块指针
                l32  l32DstStride  ：目标块步长
返回值说明   :  无
特殊说明     :  无
====================================================================*/
void MP4MemCopy8x8_c(const u8 *pu8Src, l32 l32SrcStride, u8 *pu8Dst, l32 l32DstStride);

/*====================================================================
函 数 名： QuantIntra8x8_c
功    能： 8x8块intra块量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32DCScaler      DC系数量化精度
           l32QP            量化步长
返 回 值： 无 
====================================================================*/
void MP4QuantIntra8x8_c(s16* ps16DCTIn, s16* ps16DCTOut, l32 l32DCScaler, l32 l32QP);

/*====================================================================
函 数 名： MP4QuantInter8x8_c
功    能： 8x8块inter块量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32QP            量化步长
返 回 值： 无 
====================================================================*/
l32 MP4QuantInter8x8_c(s16* ps16DCTIn, s16* ps16DCTOut, l32 l32QP);

/*====================================================================
函 数 名： MP4DeQuantInter8x8_c
功    能： 8x8块inter块反量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32QP            量化步长
返 回 值： 无 
====================================================================*/
void MP4DeQuantInter8x8_c(s16* ps16DCTIn, s16* ps16DCTOut, l32 l32QP);

/*====================================================================
函 数 名： MP4DeQuantIntra8x8_c
功    能： 8x8块intra块反量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32DCScaler      DC系数量化精度
           l32QP            量化步长
返 回 值： 无 
====================================================================*/
void MP4DeQuantIntra8x8_c(s16* ps16DCTIn, s16* ps16DCTOut,  l32 l32DCScaler, l32 l32QP);

/*=============================================================================
函 数 名： MP4AddiDCTIQuant8x8_c
功    能： 16x16反DCT变换，Dequant
算法实现： 无
全局变量： 无 
参    数： ps16DCTIn     ：输入系数指针
           ps16DCTOut    ：输出系数指针
           pu8Rec        : 重建块存放地址
           l32RecStride  : 重建块步长
           pu8Ref        ：参考图像指针
           l32RefStride  ：参考图像步长
           l32Qp         ：量化系数
           l32IsIntra    ：是否为Intra块(Intra : 1; Inter : 0)
           l32Cbp        ：当前块是否是全0（0表示是全0块，否则不是全0块）
           l32DcScaler   ：DC系数的量化精度(Intra块才会用到)
返 回 值： 无
=============================================================================*/
void MP4AddiDCTIQuant8x8_c(s16* ps16DCTIn, s16* ps16DCTOut, l32 l32QP, u8* pu8Rec, l32 l32RecStride, u8* pu8Ref, l32 l32RefStride, l32 l32IsIntra, l32 l32Cbp, l32 l32DCSalar);

/*=============================================================================
函 数 名： MP4SubDCTQuant8x8_c
功    能： 8x8块残差，DCT和Quant
算法实现： 无
全局变量： 无 
参    数： ps16Dct        DCT变换buffer
           pu8Src         原始图像
           l32SrcStride   原始图像步长
           pu8Dst         参考图像
           l32DstStride   参考图像步长
           ps16DctOut     量化后输出buffer
           l32Qp          量化值
           l32IsIntra     当前块是否是Intra块
           l32DCScaler    直流系数量化值
返 回 值： 无
=============================================================================*/
l32 MP4SubDCTQuant8x8_c(s16* ps16DCT, u8* pu8Src, l32 l32SrcStride, u8* pu8Dst, l32 l32DstStride, s16* ps16DCTOut, l32 l32Qp, l32 l32IsIntra, l32 l32DCScalar);

/*=============================================================================
函 数 名: MP4Interpolate8x8v_c
功    能: 对参考图像进行水平方向半象素插值
算法实现: 无
全局变量: 无
参    数: pu8Dst              源指针(8字节对齐)
		  pu8Src              目标指针
		  s16Sride            图像步长
返 回 值: 无 
=============================================================================*/
void MP4Interpolate8x8v_c(u8 *pu8Dst, s16 s16DstStride, u8 *pu8Src, s16 s16Stride, u32 u32Num, l32 l32Rouding);

/*=============================================================================
函 数 名: MP4Interpolate16x16v_c
功    能: 对参考图像进行水平方向半象素插值
算法实现: 无
全局变量: 无
参    数: pu8Dst              源指针(8字节对齐)
		  pu8Src              目标指针
		  s16Sride            图像步长
返 回 值: 无 
=============================================================================*/
void MP4Interpolate16x16v_c(u8 *pu8Dst, s16 s16DstStride, u8 *pu8Src, s16 s16Stride, u32 u32Num, l32 l32Rouding);

/*=============================================================================
函 数 名: MP4Interpolate8x8h_c
功    能: 对参考图像进行水平方向半象素八抽头插值
算法实现: 无
全局变量: 无
参    数: pu8Dst              源指针(8字节对齐)
		  pu8Src              目标指针
		  s16Sride            图像步长
返 回 值: 无 
=============================================================================*/
void MP4Interpolate8x8h_c(u8 *pu8Dst, s16 s16DstStride, u8 *pu8Src, s16 s16SrcStride, u32 u32Num, l32 l32Rouding);

/*=============================================================================
函 数 名: MP4Interpolate16x16h_c
功    能: 对参考图像进行水平方向半象素八抽头插值
算法实现: 无
全局变量: 无
参    数: pu8Dst              源指针(8字节对齐)
		  pu8Src              目标指针
		  s16Sride            图像步长
返 回 值: 无 
=============================================================================*/
void MP4Interpolate16x16h_c(u8 *pu8Dst, s16 s16DstStride, u8 *pu8Src, s16 s16SrcStride, u32 u32Num, l32 l32Rouding);

#define MP4PixAvgH8x8 MP4PixAvgH8x8_c

#define MP4PixAvgV8x8 MP4PixAvgV8x8_c

#define MP4PixAvgC8x8 MP4PixAvgC8x8_c

#define MP4DCT MP4DCT_c

#define MP4IDCT MP4IDCT_c

#define MP4AddiDCT8x8 MP4AddiDCT8x8_c

#define MP4SubDCT8x8 MP4SubDCT8x8_c

#define MP4Deviation16x16 MP4Deviation16x16_c

#define MP4MemCopy8x8 MP4MemCopy8x8_c

#define MP4QuantIntra8x8 MP4QuantIntra8x8_c

#define MP4QuantInter8x8 MP4QuantInter8x8_c

#define MP4DeQuantInter8x8 MP4DeQuantInter8x8_c

#define MP4DeQuantIntra8x8 MP4DeQuantIntra8x8_c

#define MP4AddiDCTIQuant8x8 MP4AddiDCTIQuant8x8_c

#define MP4SubDCTQuant8x8 MP4SubDCTQuant8x8_c

#define MP4Interpolate8x8v MP4Interpolate8x8v_c

#define MP4Interpolate16x16v MP4Interpolate16x16v_c

#define MP4Interpolate8x8h MP4Interpolate8x8h_c

#define MP4Interpolate16x16h MP4Interpolate16x16h_c

#endif

#endif
