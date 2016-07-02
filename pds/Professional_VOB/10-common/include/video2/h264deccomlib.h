/******************************************************************************
模块名	    ：H264解码公共库函数库
相关文件	：
文件实现功能：声明一些经过优化了的函数的原型。
              为了在PC和EQUATOR平台上最大限度的共享代码，避免维护Win32和Equator
              两个程序版本，这里采用预处理区别不同平台上的函数。在调用程序中只需
              写函数名的主体，具体调用什么平台上的函数由编译器自动选择。预处理是
              这样做的：
              根据它的取值区分不同平台：
                      MAP_CODE:在EQUATOR平台上优化
			                DM642_CODE:TI DM642的优化	
			                C_CODE：C语言版本
---------------------------------------------------------------------------------------------------------------------
修改记录:
  日  期	      版本	    	   修改人		       修改内容
2006.10.16      1.0            李婧           创建
******************************************************************************/
#ifndef _H264DF_LIB_H_
#define _H264DF_LIB_H_

#include <stdio.h>
#include <string.h>
#include "algorithmtype.h"
#include "h264decoder.h"

#ifdef _MSC_VER
	//warning C4206: nonstandard extension used : translation unit is empty
	#pragma warning(disable : 4206)
#endif//_MSC_VER

//H264LIB 版本号
#define VER_VCCDEC	(const char*)"h264dflib.36.36.01.00.051031"


#if defined __cplusplus
extern "C" { /* Begin "C" */
/* Intrinsics use C name-mangling.
 */
#endif /* __cplusplus */

#if  defined(DM642_CODE) ////在TI上优化   /* OPTIMIZE_FOR */
/*====================================================================
函数名	     :DecGetStrengthPlane
功能		 ：获取整帧图像的边缘强度，存放到每个宏块的au8MBStrength里面
算法实现	 ：参考H264白皮书loopfilter.pdf
引用全局变量 ：无
输入参数说明 ：*ptDecMBData：指向位于DecImg中的存放宏块信息的数组的指针（输入）
			   u32PicSizeInMbs:帧图像总的宏块数目（输入）
			   u32PicWInMbs：帧图像在宽度(x方向)总的宏块数目（输入）
返回值说明   ：void
====================================================================*/
void DecGetStrengthPlane_ti(TDecMacroBlock *ptDecMBData, u32 u32PicSizeInMbs, u32 u32PicWInMbs);
//void DecGetStrengthPlane_ti(TDecMacroBlock *ptMbQ, l32 l32TSize, l32 l32FilterLeftMbEdgeFlag, l32 l32FilterToptMbEdgeFlag);
//void DecGetStrengthPlane_ti(TDecMacroBlock *ptMbQ, TDecMacroBlock *ptTempMbQ, l32 l32TSize, l32 l32FilterLeftMbEdgeFlag, l32 l32FilterToptMbEdgeFlag);

/*====================================================================
函数名	      : DecGetStrengthPlane
功能		  ：获取整帧图像的边缘强度，存放到每个宏块的au8MBStrength里面
算法实现	  ：参考H264白皮书loopfilter.pdf
引用全局变量 ：无
输入参数说明 ：*ptDecCurMBData：当前宏块信息的指针,在C里用DMA搬运,将它减结构体大小就是顶上宏块信息指针（输入）
			   *ptDecLeftMBData：当前宏块左边宏块信息的指针			    
			   u32PicWInMbs：帧图像在宽度(x方向)总的宏块数目（输入）
			   l32FilterLeftMbEdgeFlag： 是否是左边边界的标志
			   l32FilterToptMbEdgeFlag： 是否是顶边边界的标志
返回值说明   ：void
---------------------------------------------------------------------
修改记录:
日期：      版本        修改人      修改内容
2007/04/04  1.0         王立军       创建
====================================================================*/ 
void DecGetStrengthMB_ti(TDecMacroBlock *ptDecCurMBData, TDecMacroBlock *ptDecLeftMBData, u32 u32PicWInMbs, 
							l32 l32FilterLeftMbEdgeFlag, l32 l32FilterToptMbEdgeFlag); 

/*====================================================================
函数名	    ：H264CabacDecRenorm             
功能		：熵解码重新归一化处理           
算法实现	：（可选项）                     
引用全局变量：无                            
输入参数说明：ptCabac  CABAC模块句柄（输入）
返回值说明  ：                              
====================================================================*/
void H264CabacDecRenorm_ti(void *ptCabac);
/*=============================================================================
函 数 名： McUVEighthPix2x2
功    能： UV分量2x2块1/8像素运动补偿
算法实现： 参考JVT－G050
全局变量： 无
参    数： pu8Src           源图像指针[in]
           l32SrcStride     源图像宽度[in]
           pu8Dst           目的图像指针[out]
           l32DstStride     目的图像宽度[in]
           s16MVx           运动矢量s16TmpMVx分量[in]
           s16MVy           运动矢量s16TmpMVy分量[in]
返 回 值： 无 
=============================================================================*/
void McUVEighthPix2x2_ti(u8* pu8Src, l32 l32SrcStride, u8* pu8Dst, l32 l32DstStride, s16 s16MVx, s16 s16MVy);
/*====================================================================
函数名       :  McUVEighthPix8x8_ti
功能         :  UV分量8x8块1/8像素运动补偿 
引用全局变量 :  无
输入参数说明 :  u8  *pu8Src        ：源块指针
                u16  u16SrcStride  ：源块步长
                u8  *pu8Dst        ：目标块指针
                l32  l32DstStride  ：目标块步长
                s16  s16Mvx        ：运动矢量x分量
                s16  s16Mvy        ：运动矢量y分量
返回值说明   :  无
特殊说明     :  无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
======================================================================*/
void McUVEighthPix8x8_ti(u8 *pu8Src, l32 l32SrcStride, u8 *pu8Dst, l32 l32DstStride, s16 s16Mvx, s16 s16Mvy);
/*====================================================================
函数名	     ：ReconI4MB
功能		 ：重建I4x4MB
算法实现	 ：（可选项）
引用全局变量：无
输入参数说明：*ptDecImg：DecImg指针（输入）
              *ptDecoder：解码器指针（输入）
			        *ptMB：当前宏块mb指针，使用其存放的熵解码后的变量属性值（输入）
返回值说明  ：成功返回H264OK，失败返回H264FAIL
======================================================================*/
//l32 ReconI4MB_ti(void *ptDecImg, void *ptDecoder, TMBCavlcData *ptMB);
/*====================================================================
	函数名	    ：iHadamard4x4dequant
	功能		：实现Hadamard反变换和dequant
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：as16Block[16]：输入的4x4块，也作为输出结果；（输入/输出）
				  s16Dequant:  反量化系数；（输入）
                  qbits_offset：直流量化系数的偏移位数（输入）
	返回值说明  ：无
====================================================================*/
void iHadamard4x4dequant_ti(s16 as16Block[16],s16 s16Dequant,l32 l32QPPer);
/*====================================================================
函数名	    ：UnscanZigzag15
功能		：zigzag反扫描，15个数据
算法实现	：（可选项）
引用全局变量：无
输入参数说明：as16Block[16]：反扫描结果存入此（输出）
              as16Level[16]：待反扫描的数据（输入）
返回值说明  ：void
======================================================================*/
void UnscanZigzag15_ti(s16 as16Block[16], s16 as16Level[16]);
/*====================================================================
函数名	    ：UnscanZigzag16
功能		：zigzag反扫描，16个数据
算法实现	：（可选项）
引用全局变量：无
输入参数说明：block[16]：反扫描结果存入此（输出）
              level[16]：待反扫描的数据（输入）
返回值说明  ：void
====================================================================*/
void UnscanZigzag16_ti(s16 as16Block[16], s16 as16Level[16]);
/*====================================================================
函数名	    ：UVDQuantAndiDCT
功能		：UV系数的反量化和反DCT
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptDecImg：DecImg指针（输入）
              *ptDecoder：解码器指针（输入）
			  *ptMB：当前宏块mb指针，使用其存放的熵解码后的变量属性值（输入）
返回值说明  ：void
====================================================================*/
void UVDQuantAndiDCT_ti(u8 *pu8DstU, u8 *pu8DstV, s16 *ptMbDCU, s16 *ptMbDCV, s16 as16Coeff[6][4][16], l32 l32QP, l32 l32UVStride);
/*====================================================================
函数名	    :Transfer4x4Copy
功能		:对4x4块进行拷贝
算法实现	:（可选项）
引用全局变量:无
输入参数说明:pu8Src当前面（输入）
             pu8Dst目标面（输入）
			 s16Stride:步长（输入）
返回值说明  :void
====================================================================*/
void Transfer4x4Copy_ti(u8 * const pu8Dst, const s16 s16DstStride, const u8 * const pu8Src, const s16 s16Stride);

void MemCopy8x8_ti(const u8* pu8Src, l32 l32SrcStride, u8* pu8Dst, l32 l32DstStride);

/*====================================================================
函数名	    :PixAvg4x4
功能		:对4x4块进行象素平均值求取
算法实现	:（可选项）
引用全局变量:无
输入参数说明:pu8Src1:         源数据1（输入）
             l32Src1Stride:   源数据1的步长（输入）
			 pu8Src2:         源数据2（输入）
			 l32Src2Stride:   源数据2的步长（输入）
			 pu8Dst:          目的数据（输出）
             l32DstStride:    目的数据的步长（输出）
返回值说明  :void
====================================================================*/
void PixAvg4x4_ti(u8 *pu8Src1, l32 l32Src1Stride, u8 *pu8Src2, l32 l32Src2Stride, u8 *pu8Dst, l32 l32DstStride);
/*====================================================================
函数名	    :PixAvg8x8
功能		:对8x8块进行象素平均值求取
算法实现	:（可选项）
引用全局变量:无
输入参数说明:pu8Src1:         源数据1（输入）
             l32Src1Stride:   源数据1的步长（输入）
			 pu8Src2:         源数据2（输入）
			 l32Src2Stride:   源数据2的步长（输入）
			 pu8Dst:          目的数据（输出）
             l32DstStride:    目的数据的步长（输出）
返回值说明  :void
====================================================================*/
void PixAvg8x8_ti(u8 *pu8Src1, l32 l32Src1Stride, u8 *pu8Src2, l32 l32Src2Stride, u8 *pu8Dst, l32 l32DstStride);
/*====================================================================
	函数名	    ：Predict16x16P
	功能		：16x16plane预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict16x16P_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left);
/*====================================================================
函数名	    ：Predict16x16P
功能		：16x16plane预测
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*pu8Src（输入）, l32Stride（输入）
返回值说明  ： void
====================================================================*/
void Predict16x16PNormal_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict16x16V
	功能		：16x16Vertical预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict16x16V_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict16x16DC
	功能		：16x16DC预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict16x16DC_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict16x16Dc128
	功能		：16x16DC128预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict16x16Dc128_ti(u8 *pu8Src, l32 l32Stride);
/*====================================================================
	函数名	    ：Predict16x16DcLeft
	功能		：16x16DCLeft预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict16x16DcLeft_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict16x16DcTop
	功能		：16x16DCTop预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict16x16DcTop_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict16x16H
	功能		：16x16Horizontal预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict16x16H_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict8x8P
	功能		：8x8plane预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict8x8P_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left);
/*====================================================================
函数名	    ：Predict8x8PNormal_c
功能		：8x8plane预测
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*pu8Src（输入）, l32Stride（输入）
返回值说明  ： void
====================================================================*/
void Predict8x8PNormal_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict8x8V
	功能		：8x8Vertical预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict8x8V_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict8x8Dc128
	功能		：8x8DC128预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict8x8Dc128_ti(u8 *pu8Src, l32 l32Stride);
/*====================================================================
	函数名	    ：Predict8x8Dc
	功能		：8x8DC预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict8x8DC_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict8x8DcLeft
	功能		：8x8DCLeft预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict8x8DcLeft_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict8x8DcTop
	功能		：8x8DCTop预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict8x8DcTop_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict8x8H
	功能		：8x8Horizontal预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict8x8H_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict4x4H
	功能		：4x4Horizontal预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict4x4H_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict4x4DDL
	功能		：Predict4x4DDL预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4DDL_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict4x4DDLNoUR
	功能		：Predict4x4DDLNoUR预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4DDLNoUR_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict4x4DDR
	功能		：Predict4x4DDR预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4DDR_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left, u8 *pu84x4);
/*====================================================================
	函数名	    ：Predict4x4HD
	功能		：Predict4x4HD预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4HD_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left, u8 *pu84x4);
/*====================================================================
	函数名	    ：Predict4x4HU
	功能		：Predict4x4HU预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4HU_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Left);
/*====================================================================

	函数名	    ：Predict4x4V
	功能		：Predict4x4V预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4V_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict4x4VL
	功能		：Predict4x4VL预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4VL_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict4x4VLNoUR
	功能		：Predict4x4VLNoUR预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4VLNoUR_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict4x4VR
	功能		：Predict4x4VR预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4VR_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left, u8 *pu84x4);
/*====================================================================
	函数名	    ：Predict4x4Dc128
	功能		：Predict4x4Dc128预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict4x4Dc128_ti(u8 *pu8Src, l32 l32Stride);
/*====================================================================
	函数名	    ：Predict4x4DC
	功能		：Predict4x4DC预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4DC_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict4x4DcLeft
	功能		：Predict4x4DcLeft预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4DcLeft_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict4x4DcTop
	功能		：Predict4x4DcTop预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict4x4DcTop_ti(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
函数名       :  AddiDCT4x4_ti
功能         :  4x4块反DCT 
引用全局变量 :  无
输入参数说明 :  s16 *ps16Block     ：块系数指针
                u8  *pu8Pred       ：预测块指针
                l32  l32PredStride ：预测块步长
                u8  *pu8Dst        ：目标块指针
                l32  l32DstStride  ：目标块步长                
返回值说明   :  无
特殊说明     :  无
======================================================================*/
void H264AddiDCT4x4_ti(s16* ps16Block, u8* pu8Pred, l32 l32PredStride, u8* pu8Dst, l32 l32DstStride);
/*====================================================================
	函数名	    :	H264Dequant4x4_ti
	功能		:	实现 4x4 块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵，
					s16 *ps16OutCoeff	:输出结果
					u8  u32Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264Dequant4x4_ti(s16 *ps16DCTIn, s16 *ps16DCTOut, u32 u32Quant);
/*====================================================================
函数名       :  Intp6TapHalfPelH4x4_ti
功能         ： 4x4块的水平方向6Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp6TapHalfPelH4x4_ti(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelV4x4_ti
功能         ： 4x4块的垂直方向6Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp6TapHalfPelV4x4_ti(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelHV4x4_ti
功能         ： 4x4块的对角方向6Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp6TapHalfPelHV4x4_ti(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);
/*====================================================================
函数名       :  Intp6TapHalfPelH8x8_ti
功能         ： 8x8块的水平方向6Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp6TapHalfPelH8x8_ti(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelV8x8_ti
功能         ： 8x8块的垂直方向6Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16       *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp6TapHalfPelV8x8_ti(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelHV8x8_ti
功能         ： 8x8块的对角方向6Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp6TapHalfPelHV8x8_ti(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);
/*====================================================================
函数名	    ：BsReadUe_ti
功能		：按照ue方式读取数据
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，bs指针
返回值说明  ：返回读取到的数值 
====================================================================*/
l32 BsReadUe_ti(TBitStream *ptBS);
/*====================================================================
函数名	    ：NalDecode
功能		：解析nalu的type，并进行ebsp -> rbsp 转换
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptNal： nalu指针（输入/输出）
              *pu8Data：传入数据（输入）
			   l32DataLen：数据大小（输入）
返回值说明  ：正确返回H264OK，错误返回H264FAIL   
=====================================================================*/
l32 NalDecode_ti(TKDCNal *ptNal, u8 *pu8Data, l32 l32DataLen);

/*====================================================================
函数名	    :	H264DeQuant4x4Opt_ti
功能		:	实现 4x4 块的反量化
算法实现	:	(可选项)
引用全局变量:	无
输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵，
s16 *ps16OutCoeff	:输出结果
s16QPRem			:反量化所要用到的参数
s16QPper			:反量化所要用到的参数
返回值说明  :	无
====================================================================*/
void H264DeQuant4x4Opt_ti(s16* ps16In, s16 * ps16Out, s16 s16QPRem, s16 s16QPper);


l32 GetMedian_ti(l32 l32X, l32 l32Y, l32 l32Z);

#define   DecGetStrengthPlane    DecGetStrengthPlane_ti
//#define   H264CabacDecRenorm     H264CabacDecRenorm_ti
#define   BsReadUe               BsReadUe_c
//#define   ReconI4MB              ReconI4MB_ti
#define   UnscanZigzag15         UnscanZigzag15_ti
#define   UnscanZigzag16         UnscanZigzag16_ti
#define   UVDQuantAndiDCT        UVDQuantAndiDCT_ti
#define   Transfer4x4Copy        Transfer4x4Copy_ti
#define   PixAvg4x4              PixAvg4x4_ti
#define   PixAvg8x8              PixAvg8x8_ti
#define   iHadamard4x4dequant    iHadamard4x4dequant_ti
#define   McUVEighthPix2x2       McUVEighthPix2x2_ti
#define   H264AddiDCT4x4         H264AddiDCT4x4_ti
#define   H264Dequant4x4         H264Dequant4x4_ti
#define	  H264DeQuant4x4Opt		 H264DeQuant4x4Opt_ti
#define   NalDecode              NalDecode_ti

#define EighthPelMCChroma8x8     McUVEighthPix8x8_ti

#define MemCopy8x8               MemCopy8x8_ti

#define GetMedian                GetMedian_ti

//4x4 6Tap 半像素插值
#define Intp6TapHalfPelH4x4		    Intp6TapHalfPelH4x4_ti
#define Intp6TapHalfPelV4x4		    Intp6TapHalfPelV4x4_ti
#define Intp6TapHalfPelHV4x4		Intp6TapHalfPelHV4x4_ti

//8x8 6Tap 半像素插值
#define Intp6TapHalfPelH8x8		    Intp6TapHalfPelH8x8_ti
#define Intp6TapHalfPelV8x8		    Intp6TapHalfPelV8x8_ti
#define Intp6TapHalfPelHV8x8		Intp6TapHalfPelHV8x8_ti

//predict
#define Predict16x16P  Predict16x16P_ti

#define Predict16x16PNormal  Predict16x16PNormal_ti

#define Predict16x16V  Predict16x16V_ti

#define Predict16x16DC Predict16x16DC_ti

#define Predict16x16Dc128 Predict16x16Dc128_ti

#define Predict16x16DcLeft Predict16x16DcLeft_ti

#define Predict16x16DcTop Predict16x16DcTop_ti

#define Predict16x16H Predict16x16H_ti

#define Predict8x8P Predict8x8P_ti

#define Predict8x8PNormal  Predict8x8PNormal_ti

#define Predict8x8V Predict8x8V_ti

#define Predict8x8Dc128 Predict8x8Dc128_ti

#define Predict8x8DC Predict8x8DC_ti

#define Predict8x8DcLeft Predict8x8DcLeft_ti

#define Predict8x8DcTop Predict8x8DcTop_ti

#define Predict8x8H Predict8x8H_ti

#define Predict4x4H Predict4x4H_ti

#define Predict4x4DDL  Predict4x4DDL_ti

#define Predict4x4DDLNoUR  Predict4x4DDLNoUR_ti

#define Predict4x4DDR  Predict4x4DDR_ti

#define Predict4x4HD  Predict4x4HD_ti

#define Predict4x4HU  Predict4x4HU_ti

#define Predict4x4V  Predict4x4V_ti

#define Predict4x4VL  Predict4x4VL_ti

#define Predict4x4VLNoUR  Predict4x4VLNoUR_ti

#define Predict4x4VR  Predict4x4VR_ti

#define Predict4x4Dc128 Predict4x4Dc128_ti

#define Predict4x4DC Predict4x4DC_ti

#define Predict4x4DcLeft Predict4x4DcLeft_ti

#define Predict4x4DcTop Predict4x4DcTop_ti

#elif defined(C_CODE)     //C代码
/*====================================================================
函数名	     :DecGetStrengthPlane
功能		 ：获取整帧图像的边缘强度，存放到每个宏块的au8MBStrength里面
算法实现	 ：参考H264白皮书loopfilter.pdf
引用全局变量 ：无
输入参数说明 ：*ptDecMBData：指向位于DecImg中的存放宏块信息的数组的指针（输入）
			   u32PicSizeInMbs:帧图像总的宏块数目（输入）
			   u32PicWInMbs：帧图像在宽度(x方向)总的宏块数目（输入）
返回值说明   ：void
====================================================================*/
//void DecGetStrengthPlane_c(TDecMacroBlock *ptDecMBData, u32 u32PicSizeInMbs, u32 u32PicWInMbs);
/*====================================================================
函数名	    ：UVDQuantAndiDCT
功能		：UV系数的反量化和反DCT
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptDecImg：DecImg指针（输入）
              *ptDecoder：解码器指针（输入）
			  *ptMB：当前宏块mb指针，使用其存放的熵解码后的变量属性值（输入）
返回值说明  ：void
====================================================================*/
void UVDQuantAndiDCT_c(u8 *pu8DstU, u8 *pu8DstV, s16 *ptMbDCU, s16 *ptMbDCV, s16 as16Coeff[6][4][16], l32 l32QP, l32 l32UVStride);
/*====================================================================
	函数名	    ：iHadamard4x4dequant
	功能		：实现Hadamard反变换和dequant
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：as16Block[16]：输入的4x4块，也作为输出结果；（输入/输出）
				  s16Dequant:  反量化系数；（输入）
                  qbits_offset：直流量化系数的偏移位数（输入）
	返回值说明  ：无
====================================================================*/
void iHadamard4x4dequant_c(s16 as16Block[16],s16 s16Dequant,l32 l32QPPer);
/*=============================================================================
函 数 名： McUVEighthPix2x2
功    能： UV分量2x2块1/8像素运动补偿
算法实现： 参考JVT－G050
全局变量： 无
参    数： pu8Src           源图像指针[in]
           l32SrcStride     源图像宽度[in]
           pu8Dst           目的图像指针[out]
           l32DstStride     目的图像宽度[in]
           s16MVx           运动矢量s16TmpMVx分量[in]
           s16MVy           运动矢量s16TmpMVy分量[in]
返 回 值： 无 
=============================================================================*/
void McUVEighthPix2x2_c(u8* pu8Src, l32 l32SrcStride, u8* pu8Dst, l32 l32DstStride, s16 s16MVx, s16 s16MVy);
/*====================================================================
函数名       :  McUVEighthPix8x8_ti
功能         :  UV分量8x8块1/8像素运动补偿 
引用全局变量 :  无
输入参数说明 :  u8  *pu8Src        ：源块指针
                u16  u16SrcStride  ：源块步长
                u8  *pu8Dst        ：目标块指针
                l32  l32DstStride  ：目标块步长
                s16  s16Mvx        ：运动矢量x分量
                s16  s16Mvy        ：运动矢量y分量
返回值说明   :  无
特殊说明     :  无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
======================================================================*/
void McUVEighthPix8x8_c(u8 *pu8Src, l32 l32SrcStride, u8 *pu8Dst, l32 l32DstStride, s16 s16Mvx, s16 s16Mvy);
/*====================================================================
函数名	    :Transfer4x4Copy
功能		:对4x4块进行拷贝
算法实现	:（可选项）
引用全局变量:无
输入参数说明:pu8Src当前面（输入）
             pu8Dst目标面（输入）
			 s16Stride:步长（输入）
返回值说明  :void
====================================================================*/
void Transfer4x4Copy_c(u8 * const pu8Dst, const s16 s16DstStride, const u8 * const pu8Src, const s16 s16Stride);

void MemCopy8x8_c(const u8* pu8Src, l32 l32SrcStride, u8* pu8Dst, l32 l32DstStride);

/*====================================================================
函数名	    :PixAvg4x4
功能		:对4x4块进行象素平均值求取
算法实现	:（可选项）
引用全局变量:无
输入参数说明:pu8Src1:         源数据1（输入）
             l32Src1Stride:   源数据1的步长（输入）
			 pu8Src2:         源数据2（输入）
			 l32Src2Stride:   源数据2的步长（输入）
			 pu8Dst:          目的数据（输出）
             l32DstStride:    目的数据的步长（输出）
返回值说明  :void
====================================================================*/
void PixAvg4x4_c(u8 *pu8Src1, l32 l32Src1Stride, u8 *pu8Src2, l32 l32Src2Stride, u8 *pu8Dst, l32 l32DstStride);
/*====================================================================
函数名	    :PixAvg8x8
功能		:对8x8块进行象素平均值求取
算法实现	:（可选项）
引用全局变量:无
输入参数说明:pu8Src1:         源数据1（输入）
             l32Src1Stride:   源数据1的步长（输入）
			 pu8Src2:         源数据2（输入）
			 l32Src2Stride:   源数据2的步长（输入）
			 pu8Dst:          目的数据（输出）
             l32DstStride:    目的数据的步长（输出）
返回值说明  :void
====================================================================*/
void PixAvg8x8_c(u8 *pu8Src1, l32 l32Src1Stride, u8 *pu8Src2, l32 l32Src2Stride, u8 *pu8Dst, l32 l32DstStride);
/*====================================================================
	函数名	    ：Predict16x16P
	功能		：16x16plane预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict16x16P_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left);

/*====================================================================
函数名	    ：Predict16x16P
功能		：16x16plane预测
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*pu8Src（输入）, l32Stride（输入）
返回值说明  ： void
====================================================================*/
void Predict16x16PNormal_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left);

/*====================================================================
	函数名	    ：Predict16x16V
	功能		：16x16Vertical预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict16x16V_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict16x16DC
	功能		：16x16DC预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict16x16DC_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict16x16Dc128
	功能		：16x16DC128预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict16x16Dc128_c(u8 *pu8Src, l32 l32Stride);
/*====================================================================
	函数名	    ：Predict16x16DcLeft
	功能		：16x16DCLeft预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict16x16DcLeft_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict16x16DcTop
	功能		：16x16DCTop预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict16x16DcTop_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict16x16H
	功能		：16x16Horizontal预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict16x16H_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict8x8Dc128
	功能		：8x8DC128预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict8x8Dc128_c(u8 *pu8Src, l32 l32Stride);
/*====================================================================
	函数名	    ：Predict8x8Dc
	功能		：8x8DC预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict8x8DC_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict8x8DcLeft
	功能		：8x8DCLeft预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict8x8DcLeft_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict8x8DcTop
	功能		：8x8DCTop预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict8x8DcTop_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict8x8H
	功能		：8x8Horizontal预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict8x8H_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict8x8P
	功能		：8x8plane预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict8x8P_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left);
/*====================================================================
函数名	    ：Predict8x8PNormal_c
功能		：8x8plane预测
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*pu8Src（输入）, l32Stride（输入）
返回值说明  ： void
====================================================================*/
void Predict8x8PNormal_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict8x8V
	功能		：8x8Vertical预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict8x8V_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict4x4H
	功能		：4x4Horizontal预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict4x4H_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict4x4DDL
	功能		：Predict4x4DDL预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4DDL_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict4x4DDLNoUR
	功能		：Predict4x4DDLNoUR预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4DDLNoUR_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict4x4DDR
	功能		：Predict4x4DDR预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4DDR_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left, u8 *pu84x4);
/*====================================================================
	函数名	    ：Predict4x4HD
	功能		：Predict4x4HD预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4HD_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left, u8 *pu84x4);
/*====================================================================
	函数名	    ：Predict4x4HU
	功能		：Predict4x4HU预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4HU_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict4x4V
	功能		：Predict4x4V预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4V_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict4x4VL
	功能		：Predict4x4VL预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4VL_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict4x4VLNoUR
	功能		：Predict4x4VLNoUR预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4VLNoUR_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
	函数名	    ：Predict4x4VR
	功能		：Predict4x4VR预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4VR_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left, u8 *pu84x4);

/*====================================================================
	函数名	    ：Predict4x4Dc128
	功能		：Predict4x4Dc128预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict4x4Dc128_c(u8 *pu8Src, l32 l32Stride);
/*====================================================================
	函数名	    ：Predict4x4DC
	功能		：Predict4x4DC预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4DC_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict4x4DcLeft
	功能		：Predict4x4DcLeft预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明： void
====================================================================*/
void Predict4x4DcLeft_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Left);
/*====================================================================
	函数名	    ：Predict4x4DcTop
	功能		：Predict4x4DcTop预测
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pu8Src（输入）, l32Stride（输入）
	返回值说明  ： void
====================================================================*/
void Predict4x4DcTop_c(u8 *pu8Src, l32 l32Stride, u8 *pu8Up);
/*====================================================================
函数名       :  AddiDCT4x4_ti
功能         :  4x4块反DCT 
引用全局变量 :  无
输入参数说明 :  s16 *ps16Block     ：块系数指针
                u8  *pu8Pred       ：预测块指针
                l32  l32PredStride ：预测块步长
                u8  *pu8Dst        ：目标块指针
                l32  l32DstStride  ：目标块步长                
返回值说明   :  无
特殊说明     :  无
======================================================================*/
void H264AddiDCT4x4_c(s16* ps16Block, u8* pu8Pred, l32 l32PredStride, u8* pu8Dst, l32 l32DstStride);
/*====================================================================
	函数名	    :	H264Dequant4x4_ti
	功能		:	实现 4x4 块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵，
					s16 *ps16OutCoeff	:输出结果
					u8  u32Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264Dequant4x4_c(s16 *ps16DCTIn, s16 *ps16DCTOut, u32 u32Quant);
/*====================================================================
函数名       :  Intp6TapHalfPelH4x4_c
功能         ： 4x4块的水平方向6Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp6TapHalfPelH4x4_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelV4x4_c
功能         ： 4x4块的垂直方向6Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp6TapHalfPelV4x4_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelHV4x4_c
功能         ： 4x4块的对角方向6Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp6TapHalfPelHV4x4_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);
/*====================================================================
函数名       :  Intp6TapHalfPelH8x8_c
功能         ： 8x8块的水平方向6Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp6TapHalfPelH8x8_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelV8x8_c
功能         ： 8x8块的垂直方向6Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16       *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp6TapHalfPelV8x8_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelHV8x8_c
功能         ： 8x8块的对角方向6Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp6TapHalfPelHV8x8_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);
/*====================================================================
函数名	    ：BsReadUe_ti
功能		：按照ue方式读取数据
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，bs指针
返回值说明  ：返回读取到的数值 
====================================================================*/
l32 BsReadUe_c(TBitStream *ptBS);
/*====================================================================
函数名	    ：NalDecode
功能		：解析nalu的type，并进行ebsp -> rbsp 转换
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptNal： nalu指针（输入/输出）
              *pu8Data：传入数据（输入）
			   l32DataLen：数据大小（输入）
返回值说明  ：正确返回H264OK，错误返回H264FAIL   
=====================================================================*/
l32 NalDecode_c(TKDCNal *ptNal, u8 *pu8Data, l32 l32DataLen);

/*====================================================================
函数名	    :	H264DeQuant4x4Opt_c
功能		:	实现 4x4 块的反量化
算法实现	:	(可选项)
引用全局变量:	无
输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵，
s16 *ps16OutCoeff	:输出结果
s16QPRem			:反量化所要用到的参数
s16QPper			:反量化所要用到的参数
返回值说明  :	无
====================================================================*/
void H264DeQuant4x4Opt_c(s16* ps16In, s16 * ps16Out, s16 s16QPRem, s16 s16QPper);

l32 GetMedian_c(l32 l32X, l32 l32Y, l32 l32Z);

//#define   DecGetStrengthPlane    DecGetStrengthPlane_c
#define   UVDQuantAndiDCT        UVDQuantAndiDCT_c
#define   Transfer4x4Copy        Transfer4x4Copy_c
#define   PixAvg4x4              PixAvg4x4_c
#define   PixAvg8x8              PixAvg8x8_c
#define   iHadamard4x4dequant    iHadamard4x4dequant_c
#define   McUVEighthPix2x2       McUVEighthPix2x2_c
#define   H264AddiDCT4x4         H264AddiDCT4x4_c
#define   H264Dequant4x4         H264Dequant4x4_c
#define	  H264DeQuant4x4Opt		 H264DeQuant4x4Opt_c
#define   BsReadUe               BsReadUe_c
#define   NalDecode              NalDecode_c

#define   EighthPelMCChroma8x8   McUVEighthPix8x8_c

#define   MemCopy8x8             MemCopy8x8_c

#define   GetMedian              GetMedian_c

//4x4 6Tap 半像素插值
#define Intp6TapHalfPelH4x4		    Intp6TapHalfPelH4x4_c
#define Intp6TapHalfPelV4x4		    Intp6TapHalfPelV4x4_c
#define Intp6TapHalfPelHV4x4		Intp6TapHalfPelHV4x4_c

//8x8 6Tap 半像素插值
#define Intp6TapHalfPelH8x8		    Intp6TapHalfPelH8x8_c
#define Intp6TapHalfPelV8x8		    Intp6TapHalfPelV8x8_c
#define Intp6TapHalfPelHV8x8		Intp6TapHalfPelHV8x8_c

//解码预测
//decode Intrapredict

#define Predict16x16P  Predict16x16P_c

#define Predict16x16V  Predict16x16V_c

#define Predict16x16PNormal  Predict16x16PNormal_c

#define Predict16x16DC Predict16x16DC_c

#define Predict16x16Dc128 Predict16x16Dc128_c

#define Predict16x16DcLeft Predict16x16DcLeft_c

#define Predict16x16DcTop Predict16x16DcTop_c

#define Predict16x16H Predict16x16H_c

#define Predict8x8P Predict8x8P_c

#define Predict8x8PNormal Predict8x8PNormal_c

#define Predict8x8V Predict8x8V_c

#define Predict8x8Dc128 Predict8x8Dc128_c

#define Predict8x8DC Predict8x8DC_c

#define Predict8x8DcLeft Predict8x8DcLeft_c

#define Predict8x8DcTop Predict8x8DcTop_c

#define Predict8x8H Predict8x8H_c

#define Predict4x4DDL  Predict4x4DDL_c

#define Predict4x4DDLNoUR  Predict4x4DDLNoUR_c

#define Predict4x4DDR  Predict4x4DDR_c

#define Predict4x4HD  Predict4x4HD_c

#define Predict4x4HU  Predict4x4HU_c

#define Predict4x4V  Predict4x4V_c

#define Predict4x4VL  Predict4x4VL_c

#define Predict4x4VLNoUR  Predict4x4VLNoUR_c

#define Predict4x4VR  Predict4x4VR_c

#define Predict4x4Dc128 Predict4x4Dc128_c

#define Predict4x4DC Predict4x4DC_c

#define Predict4x4DcLeft Predict4x4DcLeft_c

#define Predict4x4DcTop Predict4x4DcTop_c

#define Predict4x4H Predict4x4H_c


#endif  /*xxx_CODE */

#if defined __cplusplus
}; /* End "C" */
#endif /* __cplusplus */

#endif // _H264DF_LIB_H_
