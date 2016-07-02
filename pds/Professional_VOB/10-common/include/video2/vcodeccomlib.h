/******************************************************************************
模块名	    ：H261、H263和Mpeg4公共函数库
相关文件	：
文件实现功能：声明一些经过优化了的函数的原型。
              为了在PC和EQUATOR平台上最大限度的共享代码，避免维护Win32和Equator
              两个程序版本，这里采用预处理区别不同平台上的函数。在调用程序中只需
              写函数名的主体，具体调用什么平台上的函数由编译器自动选择。预处理是
              这样做的：
              定义宏，根据它的取值区分不同平台：
			  SSE2_CODE:在PC上的SSE2优化
			  BLACKFIN_CODE:在Blackfin平台上优化
              MAP_CODE:在EQUATOR平台上优化
              C_CODE: C语言版本
---------------------------------------------------------------------------------------------------------------------
修改记录:
  日  期	      版本	    	修改人		修改内容
2004.09.07        1.0          ZouWenyi       创建
2005.04.15        1.1           胡小鹏        增加其它平台
2006.05.16        1.2           胡小鹏        更改优化的宏开关
******************************************************************************/

#ifndef _VCODEC_COMMON_LIB_H_
#define _VCODEC_COMMON_LIB_H_

#include <stdio.h>
#include <string.h>
#include "algorithmtype.h"

#ifdef _MSC_VER
	//warning C4206: nonstandard extension used : translation unit is empty
	#pragma warning(disable : 4206)
#endif//_MSC_VER

//VCodecComLib 版本号
#define VER_VCCL	(const char*)"VCodecComLib.36.36.01.00.051223"
#define MAX_FRAME_COUNT  30 

#if defined __cplusplus
extern "C" { /* Begin "C" */
/* Intrinsics use C name-mangling.
 */
#endif /* __cplusplus */

/*====================================================================
函数名	    :ReverseOneBit
功能		:对输入数据的指定位进行取反
算法实现	:（可选项）
引用全局变量:无
输入参数说明:  u8  *pu8Stream，输入数据
               l32  l32Offset 要取反位的位置
返回值说明  : void
====================================================================*/
void ReverseOneBit(u8 * pu8Stream, l32 l32Offset);

/*====================================================================
函数名	    :ReverseContinousBits
功能		:对输入数据中的某段数据进行按位取反
算法实现	:（可选项）
引用全局变量:无
输入参数说明:  u8  *pu8Stream，输入数据
               l32  l32Start  要取反的位的起始位置
			   l32  l32Length 要取反的位的长度
返回值说明  : void
====================================================================*/
void ReverseContinousBits(u8 * pu8Stream, l32 l32Start, l32 l32Length);

//Mem Manage	
/*====================================================================
函数名      ： MallocSpareMemory
功能    	： 分配剩余内存空间并赋予指定的数值
算法实现    ：
引用全局变量：无
输入参数说明：
            void *ptMem : 指向分配内存空间结构体的指针 

返回值说明  ：
====================================================================*/
void MallocSpareMemory(void *ptMem, u32 u32FreeMemSize);

/*====================================================================
函数名      ： FreeSpareMemory
功能    	： 释放由MallocSpareMemory分配的内存
算法实现    ：
引用全局变量：无
输入参数说明：
            void *ptMem : 指向分配内存空间结构体的指针 

返回值说明  ：
====================================================================*/
void FreeSpareMemory(void *ptMem);

/*====================================================================
函数名      ： SpareMemoryIsOverWrite 
功能    	： 比较内存空间的数值是否发生改变
算法实现    ：
引用全局变量：无
输入参数说明：
            void *ptMem : 指向分配内存空间结构体的指针 

返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
05/03/01               ZouWenyi	        创建
====================================================================*/
void SpareMemoryIsOverWrite (void *ptMem);

/*====================================================================
	函数名	    ：MallocAlignMem
	功能		：开辟空间，并进行内存对齐
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：u32Size：申请开辟的空间大小，u8Alignment：对齐的位数
	返回值说明：void
====================================================================*/
void *MallocAlignMem(u32 u32Size,	u8 u8Alignment);

/*====================================================================
	函数名	    ：AssignAlignMem
	功能		：虚拟分配内存，从已经分配的内存中占用内存，并进行数据对齐
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：u32Size:需要分配的内存大小，u8Alignment：对齐的位数，ptComBuf：内存管理指针
	返回值说明：void
====================================================================*/
void *AssignAlignMem(u32 u32Size,	u8 u8Alignment, void *ptComBuffer);

/*====================================================================
	函数名	    ：FreeAlignMem
	功能		：释放由MallocAlignMem分配的内存空间
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：*pvMem：内存指针
	返回值说明：void
====================================================================*/
void FreeAlignMem(void *pvMem);

//BitStream
/*====================================================================
函数名	    ：BsInit
功能		：bs初始化
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBitStream，要初始化的指针，*pvData,传入数据，u32Data,传入数据的大小
返回值说明  ：void
====================================================================*/

void BsInit( void *ptBitStream, void *pvData, u32 u32Data );

/*====================================================================
函数名	    ：BsPos
功能		：返回bs位置
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，码流指针
返回值说明  ：返回位置
====================================================================*/

u32 BsPos( void *ptBitStream );


/*====================================================================
函数名	    ：BsRead
功能		：读取bs，共读u32Count位
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，bs指针，u32Count要读取的位数
返回值说明  ：返回读取到的数值
====================================================================*/

u32 BsRead( void *ptBitStream, u32 u32Count );

/*====================================================================
函数名	    ：BsRead1Bit
功能		：读取bs，共读1位
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，bs指针
返回值说明  ：返回读取到的数值
====================================================================*/

u32 BsRead1Bit( void *ptBitStream );

/*====================================================================
函数名	    ：BsShow
功能		：显示u32Count位的数据值
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*s，bs指针, u32Count，位数
返回值说明  ：返回要显示的数值
====================================================================*/

u32 BsShow( void *ptBitStream, u32 u32Count);

/*====================================================================
函数名	    ：BsSkip
功能		：跳过u32Count位
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，bs指针, u32Count，跳过的位数
返回值说明  ：void
====================================================================*/

void BsSkip( void *ptBitStream, u32 u32Count );

/*====================================================================
函数名	    ：BsReadUe
功能		：按照ue方式读取数据
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，bs指针
返回值说明  ：返回读取到的数值
====================================================================*/

u32 BsReadUe( void *ptBitStream );

/*====================================================================
函数名	    ：BsReadSe
功能		：按照se的方式读取数据
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，bs指针
返回值说明  ：返回读取到的数值
====================================================================*/

l32 BsReadSe( void *ptBitStream );

/*====================================================================
函数名	    ：BsReadTe
功能		：按照te的方式读取数据
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，u32RefIdx
返回值说明  ：返回读取到的数值
====================================================================*/

l32 BsReadTe( void *ptBitStream, u32 u32RefIdx );

/*====================================================================
函数名	    ：BsWrite
功能		：将u32Val的低u32Count位写入bs
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，bs指针，u32Count，写入位数，u32Val, 写入码流的值 
返回值说明  ：无
====================================================================*/
/* TODO optimize (write x bits at once) */

void BsWrite( void *ptBitStream, u32 l32Count, u32 u32Val );

/*====================================================================
函数名	    ：BsWrite1Bit
功能		：将u32Val以1个bit写入bs
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，bs指针，u32Val，需要写入的值
返回值说明  ：无
====================================================================*/

void BsWrite1Bit( void *ptBitStream, u32 u32Val );

/*====================================================================
函数名	    ：BsWriteUe
功能		：将u32Val以无符号方式写入bs
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，bs指针，u32Val，需要写入的值
返回值说明  ：无
====================================================================*/

void BsWriteUe( void *ptBitStream, u32 u32Val );

/*====================================================================
函数名	    ：BsWriteSe
功能		：将l32Val以有符号方式写入bs
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，bs指针，l32Val，需要写入的值
返回值说明  ：无
====================================================================*/

void BsWriteSe( void *ptBitStream, l32 l32Val );

/*====================================================================
函数名	    ：BsRBSPTrailing
功能		：RBSP码流的结尾字节的处理
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，bs指针
返回值说明  ：无
====================================================================*/

void BsRBSPTrailing( void *ptBitStream );

/*====================================================================
函数名	    ：BsWriteVlc
功能		：向码流中写数据
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，bs指针， tV ，vlt
返回值说明  ：无
====================================================================*/

void BsWriteVlc( void *ptBitStream, void *ptV );

/*====================================================================
函数名	    ：BsReadVlc
功能		：读取vlc，从table中
算法实现	：（可选项）
引用全局变量：无
输入参数说明：*ptBS，*ptTable
返回值说明  ：返回读取到的数值
====================================================================*/

l32 BsReadVlc( void *ptBitStream, void *ptTable );

//Block Copy
/*====================================================================
函数名       :  BlockCopy4x4
功能         ： 4×4模块的BlockCopy
引用全局变量 ： 无
输入参数说明 ： u8 *pu8CurrMB		: 当前块
				u16 u16CurrStride	：当前块步长
				u8 *pu8RefMB		：参考块
				u16 u16RefStride	：参考块步长
返回值说明   ： 无
特殊说明     ： 无
======================================================================*/
void BlockCopy4x4(u8 *pu8CurrMB, u16 u16CurrStride, u8 *pu8RefMB, u16 u16RefStride);

/*====================================================================
函数名       :  BlockCopy8x8
功能         ： 8×8模块的BlockCopy
引用全局变量 ： 无
输入参数说明 ： u8 *pu8CurrMB		: 当前块
				u16 u16CurrStride	：当前块步长
				u8 *pu8RefMB		：参考块
				u16 u16RefStride	：参考块步长
返回值说明   ： 无
特殊说明     ： 无
======================================================================*/
void BlockCopy8x8(u8 *pu8CurrMB, u16 u16CurrStride, u8 *pu8RefMB, u16 u16RefStride);

/*====================================================================
函数名       :  BlockCopy16x16
功能         ： 16×16模块的BlockCopy 
引用全局变量 ： 无
输入参数说明 ： u8 *pu8CurrMB		: 当前块
				u16 u16CurrStride	：当前块步长
				u8 *pu8RefMB		：参考块
				u16 u16RefStride	：参考块步长
返回值说明   ： 无
======================================================================*/
void BlockCopy16x16(u8 *pu8CurrMB, u16 u16CurrStride, u8 *pu8RefMB, u16 u16RefStride);

/*===========================================================================
函数名	    ：PutMBToFrame
功能		：将一个MB的数据写入到输出缓存中去
算法实现	：
参数说明    ：u8 * pu8YPointer  输出缓存的Y分量指针
              u8 * pu8UPointer  输出缓存的U分量指针
              u8 * pu8VPointer  输出缓存的V分量指针
              u8 au8MBData[6][64] 存放MB数据的一个二维数组
              l32 l32FrameWidth 帧宽（Y分量）
             
返回值说明  ：void
=============================================================================*/
void PutMBToFrame(u8 *pu8YPointer, u8 * pu8UPointer, u8 * pu8VPointer, u8 au8MBData[6][64], u16 u16FrameWidth);

/*====================================================================
函数名       :  RateControlInit
功能         ： 初始码率控制参数
引用全局变量 ： 无
输入参数说明 ： void        *ptRateCtrl	        :  码率控制结构体指针
                u32        u32TargetRate		: 目标码率
				float        fFrameRate			: 帧率
				l32        l32MaxQuant			: 最大量化系数
				l32        l32MinQuant			: 最小量化系数
				BOOL           IsLoseFrame		: 是否进行丢帧处理
				u8 u8RCType						: 码流控制方式 0-严格控制码流在目标码流之下
												: 1-码流控制可以超出目标码流20%
				u32 u32ContinueLoseFrameNum		: 可以连续丢帧数
返回值说明   ： 有效的量化系数
特殊说明     ：  无

=============================================================================*/

l32 RateControlInit(void *ptRateCtrl, u32 u32TargetRate, float fFrameRate, l32 l32MaxQuant, 
					l32 l32MinQuant, BOOL IsLoseFrame, u8 u8RCType, u32 u32ContinueLoseFrameNum);

/*====================================================================
函数名       :  RateControlGetEstimatedQ
功能         ： 获取估计的Quant值
引用全局变量 ： 无
输入参数说明 ： TRateControl  *ptRateControl :  码率控制结构体指针
返回值说明   ： 估计的量化系数
特殊说明     ：  无				
======================================================================*/

l32 RateControlGetEstimatedQ(void *pRateCtrl);

/*====================================================================
函数名       :  RateControlGetQ
功能         ： 获取量化系数
引用全局变量 ： 无
输入参数说明 ： TRateControl  *ptRateControl : 码率控制结构体指针
                u8              u8VopType  PPICTURE: 表示P帧, IPICTURE: 表示I帧        
				BOOL           IsLoseFrame  : 是否丢帧处理

返回值说明   ： 估计的量化系数
特殊说明     ：  无		
======================================================================*/

l32 RateControlGetQ(void *pRateCtrl, u8 u8VopType, BOOL IsLoseFrame);
		
/*====================================================================
函数名       :  RateControlUpdate
功能         ： 获取量化系数
引用全局变量 ： 无
输入参数说明 ： TRateControl  *ptRateControl : 码率控制结构体指针
                l32        iFrameSize		 : 帧大小      
				u8        u8VopType   PPICTURE: 表示P帧, IPICTURE: 表示I帧        

返回值说明   ： 估计的量化系数
特殊说明     ：  无			
======================================================================*/

l32 RateControlUpdate(void * pRateCtrl, u32 u32FrameSize, u8 u8VopType);
					

/*====================================================================
函数名       :  PSNROneFrame
功能         ： 原始图像与重建图像的信噪比PSNR 
引用全局变量 ： 无
输入参数说明 ： u8 * pbySrcFrame	:原始图像指针
				u16 u16SrcStride	:原始图像步长
				u8 * pbyRefFrame	:重建图像指针
				u16 u16RefStride	:重建图像步长
				u16 u16Height		:图像的高度
				u16 u16Width		:图像的宽度
				float *fPSNR		:两帧图像的信噪比 
返回值说明   ： TRUE,参数输入错误返回FALSE
特殊说明     ： (输入参数错误时返回 FALSE )
======================================================================*/
BOOL PSNROneFrame(u8 *pu8SrcFrame, u16 u16SrcStride, u8 *pu8RefFrame, 
				  u16 u16RefStride, u16 u16Height, u16 u16Width, float *pfPSNR);

/*====================================================================
函数名      ：	PSNRFileFrame
功能		：	计算两帧图像的信噪比
参数		：	const char *lpszSrcFile		:录制图像文件
				const char *lpszRefFile		:参考的图像文件
				const char *lpszResultFile	:比较结果存放文件
				u16 u16Height				:图像的高度
				u16 u16Width				:图像的宽度
				u32 u32FrameCount			:帧数
返回值		：返回TRUE，如输入参数错误时返回 FALSE
====================================================================*/
BOOL PSNRFileFrame (const char *lpszSrcFile, const char *lpszRefFile, const char *lpszResultFile, 
					u16 u16Height, u16 u16Width, u32 u32FrameCount, u32 *pu32DiffCount);
					

/*====================================================================
函数名       :  MEFullPelFAST16x16
功能         ： 求16×16宏块的运动矢量(快速搜索算法，精度位为整象素) 
引用全局变量 ： 无
输入参数说明 ：  u8         *pu8RefMB         :     指向起始搜索宏块指针
                 u16         u16RefStride     :     参考宏块步长
				 u8         *pu8CurrMB        :     当前宏块指针
				 u16         u16CurrStride    :     当前宏块的步长
				 s16         s16SearchMinX    :     x负方向搜索的范围
				 s16         s16SearchMaxX    :     x正方向搜索的范围
				 s16         s16SearchMinY    :     y负方向搜索的范围
				 s16         s16SearchMaxY    :     y正方向搜索的范围
				 void *ptMVArray  :     当前宏块前三个宏块的运动矢量
				 void *ptMVector        :     当前宏块的运动矢量（out）
返回值说明   ：  最佳的SAD值
特殊说明     ：  无
======================================================================*/
l32 MEFullPelFAST16x16(u8 *pu8RefMB, u16 u16RefStride, u8 *pu8CurrMB, 
					u16 u16CurrStride, s16 s16SearchMinX, s16 s16SearchMaxX, 
					s16 s16SearchMinY, s16 s16SearchMaxY, void *ptMVArray,
					void *ptMVector);
				 

/*====================================================================
函数名       :  MEFullPelFullSearch16x16
功能         ： 求16×16宏块的运动矢量(全搜索算法，精度位为整象素) 
引用全局变量 ： 无
输入参数说明 ：  u8         *pu8RefMB         :     指向参考宏块指针
                 u16         u16RefStride      :     参考宏块步长
				 u8         *pu8CurrMB        :     当前宏块指针
				 u16         u16CurrStride     :     当前宏块的步长
				 s16         s16SearchMinX    :     x负方向搜索的范围
				 s16         s16SearchMaxX    :     x正方向搜索的范围
				 s16         s16SearchMinY    :     y负方向搜索的范围
				 s16         s16SearchMaxY    :     y正方向搜索的范围
				 void *ptMVector             :     当前宏块的运动矢量（out）
返回值说明   ：  最佳的SAD值
特殊说明     ：  无
======================================================================*/
l32 MEFullPelFullSearch16x16(u8 *pu8RefMB, u16 u16RefStride, u8 *pu8CurrMB, 
					  u16 u16CurrStride, s16 s16SearchMinX, s16 s16SearchMaxX, 
					  s16 s16SearchMinY, s16 s16SearchMaxY, void *ptMVector);

/*====================================================================
函数名       :  MEHalfPel8Points16x16
功能         ： 求16×16宏块的运动矢量(精度位为半象素) 
引用全局变量 ： 无
输入参数说明 ：  u8         *pu8RefMB         :     指向参考宏块指针
                 u16         u16RefStride      :     参考宏块步长
				 u8         *pu8CurrMB        :     当前宏块指针
				 u16         u16CurrStride     :     当前宏块的步长
				 s16         s16SearchMinX    :     x负方向搜索的范围
				 s16         s16SearchMaxX    :     x正方向搜索的范围
				 s16         s16SearchMinY    :     y负方向搜索的范围
				 s16         s16SearchMaxY    :     y正方向搜索的范围
				 l32         l32Sad            :     当前运动矢量下的SAD值
				 BOOL		 bRounding		:	是否进行四舍五入
				 void *ptMVector         :     当前宏块的运动矢量（in/out）
返回值说明   ：  最佳的SAD值
特殊说明     ：  无
======================================================================*/					  
l32 MEHalfPel8Points16x16(u8 *pu8RefMB, u16 u16RefStride, u8 *pu8CurrMB, 
                         u16 u16CurrStride, s16 s16SearchMinX, s16 s16SearchMaxX,
                         s16 s16SearchMinY, s16 s16SearchMaxY, l32 l32Sad, 
                         BOOL bRounding, void *ptMVector);
                                           

/*====================================================================
函数名       :  MEFullPelFullSearch8x8
功能         ： 求8×8块的运动矢量(全搜索算法，精度位为整象素) 
引用全局变量 ： 无
输入参数说明 ：  u8         *pu8RefMB         :     指向参考宏块指针
                 u16         u16RefStride      :     参考宏块步长
				 u8         *pu8CurrMB        :     当前宏块指针
				 u16         u16CurrStride     :     当前宏块的步长
				 s16         s16SearchMinX     :     x负方向搜索的范围
				 s16         s16SearchMaxX     :     x正方向搜索的范围
				 s16         s16SearchMinY     :     y负方向搜索的范围
				 s16         s16SearchMaxY     :     y正方向搜索的范围
				 void *ptMVector         :     当前宏块的运动矢量（out）
返回值说明   ：  最佳的SAD值
特殊说明     ：  无
======================================================================*/                                         
l32 MEFullPelFullSearch8x8(u8 *pu8RefMB, u16 u16RefStride, u8 *pu8CurrMB, 
						u16 u16CurrStride, s16 s16SearchMinX, s16 s16SearchMaxX,
                        s16 s16SearchMinY, s16 s16SearchMaxY, void *ptMVector);
                                    
/*====================================================================
函数名       :  MEHalfPel8Points8x8
功能         ： 求8×8宏块的运动矢量(精度为半象素) 
引用全局变量 ： 无
输入参数说明 ：  u8         *pu8RefMB         :     指向参考宏块指针
                 u16         u16RefStride      :     参考宏块步长
				 u8         *pu8CurrMB        :     当前宏块指针
				 u16         u16CurrStride     :     当前宏块的步长
				 s16         s16SearchMinX     :     x负方向搜索的范围
				 s16         s16SearchMaxX     :     x正方向搜索的范围
				 s16         s16SearchMinY     :     y负方向搜索的范围
				 s16         s16SearchMaxY     :     y正方向搜索的范围
				 l32         l32Sad            :     当前运动矢量下的SAD值
				 BOOL		 bRounding		:	是否进行四舍五入
				 void *ptMVector         :     当前宏块的运动矢量（in/out）
返回值说明   ：  最佳的SAD值
特殊说明     ：  无
======================================================================*/                                           
l32 MEHalfPel8Points8x8(u8 *pu8RefMB, u16 u16RefStride, u8 *pu8CurrMB, 
                        u16 u16CurrStride, s16 s16SearchMinX, s16 s16SearchMaxX,
                        s16 s16SearchMinY, s16 s16SearchMaxY, l32 l32Sad, 
                        BOOL bRounding, void *ptMVector); 

/*====================================================================
函数名       :  MEFullAndHalfPelFastSearch16x16
功能         ： 求16×16宏块的运动矢量(精度为半象素) 
引用全局变量 ： 无
输入参数说明 ：  u8         *pu8RefMB          :     指向参考宏块指针
                 u16         u16RefStride      :     参考宏块步长
				 u8         *pu8CurrMB         :     当前宏块指针
				 u16         u16CurrStride     :     当前宏块的步长
				 s16         s16SearchMinX     :     x负方向搜索的范围
				 s16         s16SearchMaxX     :     x正方向搜索的范围
				 s16         s16SearchMinY     :     y负方向搜索的范围
				 s16         s16SearchMaxY     :     y正方向搜索的范围
				 l32         l32Sad            :     当前运动矢量下的SAD值
				 BOOL		 bRounding		:	是否进行四舍五入
				 void *ptPredMVArray	   :	 相邻三点运动矢量
				 void *ptMVector         :     当前宏块的运动矢量（in/out）
返回值说明   ：  最佳的SAD值
特殊说明     ：  先进行全像素快速搜索，后进行半像素搜索
======================================================================*/
l32 MEFullAndHalfPelFastSearch16x16(u8 *pu8RefMB, u16 u16RefStride, u8 *pu8CurrMB, u16 u16CurrStride, 
						s16 s16SearchMinX, s16 s16SearchMaxX, s16 s16SearchMinY, s16 s16SearchMaxY, BOOL bRounding, 
						void *ptPredMVArray, void *ptMVector);
								
/*====================================================================
函数名       :  MEFullAndHalfPelSearch16x16
功能         ： 求16×16宏块的运动矢量(精度为半象素) 
引用全局变量 ： 无
输入参数说明 ：  u8         *pu8RefMB         :     指向参考宏块指针
                 u16         u16RefStride      :     参考宏块步长
				 u8         *pu8CurrMB        :     当前宏块指针
				 u16         u16CurrStride     :     当前宏块的步长
				 s16         s16SearchMinX     :     x负方向搜索的范围
				 s16         s16SearchMaxX     :     x正方向搜索的范围
				 s16         s16SearchMinY     :     y负方向搜索的范围
				 s16         s16SearchMaxY     :     y正方向搜索的范围
				 l32         l32Sad            :     当前运动矢量下的SAD值
				 BOOL		 bRounding		:	是否进行四舍五入
				 void *ptMVector         :     当前宏块的运动矢量（in/out）
返回值说明   ：  最佳的SAD值
特殊说明     ：  先进行全像素搜索，后进行半像素搜索
======================================================================*/
l32 MEFullAndHalfPelSearch16x16(u8 *pu8RefMB, u16 u16RefStride, u8  *pu8CurrMB, 
						u16 u16CurrStride, s16 s16SearchMinX, s16 s16SearchMaxX,  
						s16 s16SearchMinY, s16 s16SearchMaxY, BOOL bRounding, void *ptMVector);

/*====================================================================
函数名       :  MEFullAndHalfPelSearch8x8
功能         ： 求8×8宏块的运动矢量(精度为半象素) 
引用全局变量 ： 无
输入参数说明 ：  u8         *pu8RefMB         :     指向参考宏块指针
                 u16         u16RefStride      :     参考宏块步长
				 u8         *pu8CurrMB        :     当前宏块指针
				 u16         u16CurrStride     :     当前宏块的步长
				 s16         s16SearchMinX     :     x负方向搜索的范围
				 s16         s16SearchMaxX     :     x正方向搜索的范围
				 s16         s16SearchMinY     :     y负方向搜索的范围
				 s16         s16SearchMaxY     :     y正方向搜索的范围
				 l32         l32Sad            :     当前运动矢量下的SAD值
				 BOOL		 bRounding		:	是否进行四舍五入
				 void *ptMVector    TCommonBuffer *     :     当前宏块的运动矢量（in/out）
返回值说明   ：  最佳的SAD值
特殊说明     ：  先进行全像素搜索，后进行半像素搜索
======================================================================*/
l32 MEFullAndHalfPelSearch8x8 (u8 *pu8RefMB, u16 u16RefStride, u8 *pu8CurrMB, u16 u16CurrStride, 
						s16 s16SearchMinX, s16 s16SearchMaxX, s16 s16SearchMinY, s16 s16SearchMaxY,
						BOOL bRounding, void *ptMVector);

/*====================================================================
函数名		: GetVComLibVersion
功能		：VCodecComLib版本信息
算法实现	：（可选项）
引用全局变量：pvVer          版本信息
              pl32VerLen     版本信息buffer
输入参数说明：无
输出参数说明：无
返回值说明  ：无　              
====================================================================*/
void GetVComLibVersion(void *pVer, u16 u16BufLen, u16 *pu16VerLen);

#if  defined(MAP_CODE) //在EQUATOR上优化


//QUANAT/DEQUANT
/*====================================================================
	函数名	    :	H264DCQuant2x2_map
	功能		:	实现 2x2 DC块的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264DCQuant2x2_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264Quant4x4_map
	功能		:	实现 4x4 块的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264Quant4x4_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264DCQuant4x4_map
	功能		:	实现 4x4 块DC的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264DCQuant4x4_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264ACQuant4x4_map
	功能		:	实现 4x4 块AC的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264ACQuant4x4_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264DCDequant2x2_map
	功能		:	实现 2x2 DC块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵，
					s16 *ps16OutCoeff	:输出结果
					u8 u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264DCDequant2x2_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*====================================================================
	函数名	    :	H264Dequant4x4_map
	功能		:	实现 4x4 块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵，
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264Dequant4x4_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*====================================================================
	函数名	    :	H264DCDequant4x4_map
	功能		:	实现 4x4 DC块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8 u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264DCDequant4x4_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*====================================================================
	函数名	    :	H264ACDequant4x4_map
	功能		:	实现 4x4 AC块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8 u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264ACDequant4x4_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263QuantIntra_map
功能		：对8x8的Intra块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff		: 存放量化后结果
              u8  u8Quant			: 它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：无。
=============================================================================*/
u32 H263QuantIntra_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263QuantInter_map
功能		：对8x8的Inter块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:待量化的8x8块
              s16  *ps16OutCoeff		:存放量化后结果
              u8  u8Quant			:它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：量化系数的和
=============================================================================*/
u32 H263QuantInter_map(const s16 *ps16InCoeff,s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263DequantIntra_map
功能		：对8x8的Intra块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		 ：待量化的8x8块
              s16  *ps16OutCoeff		 ：存放量化后结果
              u8  u8Quant			 ：它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：void
=============================================================================*/
void H263DequantIntra_map(s16 *ps16InCoeff,s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263DequantInter_map
功能		：对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff		: 存放量化后结果
              u8  u8Quant			:它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：
=============================================================================*/
void H263DequantInter_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

//对8x8的Intra块做反量化(处理直接从Vlx输出的数据)
void H263DequantIntraVlx_map(const s16 *ps16InCoeff, s16 *ps16OutCoeff,const u8 u8Quant);

//对8x8的Inter块做反量化(处理直接从Vlx输出的数据)
void H263DequantInterVlx_map(const s16 *ps16InCoeff, s16 *ps16OutCoeff, const u8 u8Quant);

/*===========================================================================
函数名	    Mpeg2QuantIntra_map
功能		：Mpeg2 对8x8的Intra块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8DcScaler		: DC系数的量化参数
              u8  u8Quant			:量化步长   
注意：
返回值说明  ：
=============================================================================*/
void Mpeg2QuantIntra_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);
 
/*===========================================================================
函数名	    Mpeg2QuantInter_map
功能		：Mpeg2 对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:  待量化的8x8块
              s16  *ps16OutCoeff		: 存放量化后结果
              u8  u8Quant			:  量化步长
注意：
返回值说明  ：返回块中是否都为0
=============================================================================*/
u16 Mpeg2QuantInter_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg2DequantIntra_map
功能		：Mpeg2 对8x8的Intra块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8Quant			: 量化参数
              u8  u8DcScaler		: DC系数的量化参数 
注意：
返回值说明  ：
=============================================================================*/
void Mpeg2DequantIntra_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg2DequantInter_map
功能		：Mpeg2 对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:  待量化的8x8块
              s16  *ps16OutCoeff	:  存放量化后结果
              u8  u8Quant			:  量化步长
注意：
返回值说明  ：
=============================================================================*/
void Mpeg2DequantInter_map(s16 *ps16InCoeff, s16  *ps16OutCoeff,  u8 u8Quant);

//对8x8的Intra块做反量化(处理直接从Vlx输出的数据)
void Mpeg2DequantIntraVlx_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

//对8x8的Inter块做反量化(处理直接从Vlx输出的数据)
void Mpeg2DequantInterVlx_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg4QuantIntra_map
功能		：对8x8的Intra块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8Quant			: 量化参数
              u8  u8DcScaler		: DC系数的量化参数   
注意：
返回值说明  ：
=============================================================================*/
u32 Mpeg4QuantIntra_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg4QuantInter_map
功能		：对8x8的Inter块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:待量化的8x8块
              s16  *ps16OutCoeff		:存放量化后结果
              u8  u8Quant			:量化参数 
注意：
返回值说明  ：
=============================================================================*/
u32 Mpeg4QuantInter_map(s16 *ps16InCoeff, s16 * ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg4DequantIntra_map
功能		：对8x8的Intra块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8Quant			: 量化参数
              u8  u8DcScaler		: DC系数的量化参数   
注意：
返回值说明  ：
=============================================================================*/
void Mpeg4DequantIntra_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg4DequantInter_map
功能		：对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:待量化的8x8块
              s16  *ps16OutCoeff		:存放量化后结果
              u8  u8Quant			:量化参数 
注意：
返回值说明  ：
=============================================================================*/
void Mpeg4DequantInter_map(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

//DCT/IDCT
//8×8和4x4块DCT变换
/*====================================================================
函数名       :  DCT8x8_map
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock   ： 指向8×8模块
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void  DCT8x8_map(s16* ps16Block);

/*====================================================================
函数名       :  DCT4x4_map
功能         ： 4×4模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock  ： 指向4×4模块
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void  DCT4x4_map(s16* ps16Block);

//8×8和4x4块DCT变换
/*====================================================================
函数名       :  IDCT8x8_map
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock  ：  指向8×8模块
返回值说明   ： 无
特殊说明     ： pBlock地址需要32位对齐
------------------------------------------------------------------------
修改记录    :
日期：      版本        修改人      修改内容

======================================================================*/
void  IDCT8x8_map(s16* ps16Block);

/*====================================================================
函数名       :  IDCT4x4_map
功能         ： 4×4模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock  ：  指向4×4模块
返回值说明   ： 无
特殊说明     ： pBlock地址需要32位对齐
------------------------------------------------------------------------
修改记录    :
日期：      版本        修改人      修改内容

======================================================================*/
void  IDCT4x4_map(s16* ps16Block);

//Hadamard/IHadmard
/*====================================================================
	函数名	    ：Hadamard2x2_map
	功能		：实现2x2 Hadamard变换
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16 * ps16InBlock		: 输入的2x2块，
                  s16 * ps16OutBlock	: 输出结果2x2块
	返回值说明  ：无
====================================================================*/
void Hadamard2x2_map(s16 *ps16InBlock, s16 *ps16OutBlock);

/*====================================================================
	函数名	    ：IHadamard2x2_map
	功能		：实现2x2 IHadamard变换
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16 *ps16InBlock  : 输入的2x2块，
                  s16 *ps16OutBlock : 输出结果2x2块
	返回值说明  ：无
====================================================================*/
void IHadamard2x2_map(s16 *ps16Block, s16 *ps16OutBlock);

/*====================================================================
	函数名	    ：Hadamard4x4_map
	功能		：实现4x4 Hadamard变换
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16 as16Block[16]：输入的4x4块，也作为输出结果
	返回值说明  ：无
====================================================================*/
void Hadamard4x4_map (s16 as16Block[16]);

/*====================================================================
	函数名	    ：IHadamard4x4_map
	功能		：实现4x4块的哈达码反变换（IHadamard）
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16  as16Block[16] ：输入的4x4块，也作为输出结果;
	返回值说明  ：无
====================================================================*/
void IHadamard4x4_map (s16 as16Block[16]);

//SAD
/*====================================================================
函数名       :  SAD16x16_map
功能         ： 16x16模块SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u16  u16CurrStride  ：当前宏块步长
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride   ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SAD16x16_map(u8 *pu8CurrMB, u16 u16CurrStride, u8 *pu8RefMB, u16 u16RefStride);

/*====================================================================
函数名       :  SADNoStride16x16_map
功能         ： 16x16模块无步长SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride   ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SADNoStride16x16_map(u8 *pu8CurrMB, u8 *pu8RefMB, u16 u16RefStride);

/*====================================================================
函数名       :  SAD8x8_map
功能         ： 8x8模块SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u16  u16CurrStride  ：当前宏块步长
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride   ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SAD8x8_map(u8 *pu8CurrMB, u16 u16CurrStride, u8 *pu8RefMB, u16 u16RefStride);

/*====================================================================
函数名       :  SADNoStride8x8_map
功能         ： 8x8模块无步长SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride  ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SADNoStride8x8_map(u8 *pu8CurrMB, u8 *pu8RefMB, u16 u16RefStride);

//求运动矢量
void  RowSAD16_map(u8 *pu8CurrMB, u16 u16CurrStride, u8 *pu8RefMB, u16 u16RefStride,
		l32 *pl32Sad_opt, void *ptMVector, s16 s16Min_x, s16 s16Min_y);

//DEVIATION
/*====================================================================
函数名       :  DeviationMB_map
功能         ： 求宏块内各象素值与平均值差的绝对值之和
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8MB ：指向宏块指针
                 u16 u16Stride	  ：宏块步长                 
返回值说明   ： 宏块内各象素值与宏块内象素平均值差的绝对值之和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
u32 DeviationMB_map(const u8 *pu8MB, s16 s16Stride);

//Interpolation
/*====================================================================
函数名       :  Intp2TapHalfPelH4x4_map
功能         ： 4x4块的水平方向2Tap半象素插值
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
void Intp2TapHalfPelH4x4_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelV4x4_map
功能         ： 4x4块的垂直方向2Tap半象素插值
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
void Intp2TapHalfPelV4x4_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHV4x4_map
功能         ： 4x4块的对角方向2Tap半象素插值
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
void Intp2TapHalfPelHV4x4_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelH4x4_map
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
void Intp6TapHalfPelH4x4_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelV4x4_map
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
void Intp6TapHalfPelV4x4_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelHV4x4_map
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
void Intp6TapHalfPelHV4x4_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHRounding8x8_map
功能         ： 8x8块的水平方向2Tap半象素插值
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
void Intp2TapHalfPelHRounding8x8_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelH8x8_map
功能         ： 8x8块的水平方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelH8x8_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelVRounding8x8_map
功能         ： 8x8块的垂直方向2Tap半象素插值
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
void Intp2TapHalfPelVRounding8x8_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelV8x8_map
功能         ： 8x8块的垂直方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelV8x8_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelHVRounding8x8_map
功能         ： 8x8块的对角方向2Tap半象素插值
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
void Intp2TapHalfPelHVRounding8x8_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHV8x8_map
功能         ： 8x8块的对角方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelHV8x8_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp6TapHalfPelH8x8_map
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
void Intp6TapHalfPelH8x8_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelIntH8x8_map
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
void Intp6TapHalfPelIntH8x8_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelV8x8_map
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
void Intp6TapHalfPelV8x8_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelIntV8x8_map
功能         ： 8x8块的垂直方向6Tap半象素插值
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
void Intp6TapHalfPelIntV8x8_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, l32 *pl32IntDst);

/*====================================================================
函数名       :  Intp6TapHalfPelHV8x8_map
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
void Intp6TapHalfPelHV8x8_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHRounding16x16_map
功能         ： 16x16源块的水平方向2Tap半象素插值
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
void Intp2TapHalfPelHRounding16x16_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelH16x16_map
功能         ： 16x16源块的水平方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelH16x16_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelVRounding16x16_map
功能         ： 16x16源块的垂直方向2Tap半象素插值
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
void Intp2TapHalfPelVRounding16x16_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelV16x16_map
功能         ： 16x16源块的垂直方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelV16x16_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelHVRounding16x16_map
功能         ： 源块的对角方向2Tap半象素插值
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
void Intp2TapHalfPelHVRounding16x16_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHV16x16_map
功能         ： 源块的对角方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelHV16x16_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp6TapHalfPelH16x16_map
功能         ： 源块的水平方向6Tap半象素插值
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
void Intp6TapHalfPelH16x16_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelV16x16_map
功能         ： 16x16宏块的垂直方向6Tap半象素插值
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
void Intp6TapHalfPelV16x16_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelHV16x16_map
功能         ： 16x16宏块的对角方向6Tap半象素插值
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
void Intp6TapHalfPelHV16x16_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_01_map
功能         ： 4x4块的四分之一象素插值(a点)
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
void IntpQuarterPel4x4_01_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_03_map
功能         ： 4x4块的四分之一象素插值(c点)
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
void IntpQuarterPel4x4_03_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPelV4x4_10_map
功能         ： 4x4块的四分之一象素插值(d点)
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
void IntpQuarterPel4x4_10_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_11_map
功能         ： 4x4块的四分之一象素插值(e点)
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
void IntpQuarterPel4x4_11_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_12_map
功能         ： 4x4块的四分之一象素插值(f点)
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
void IntpQuarterPel4x4_12_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_13_map
功能         ： 4x4块的四分之一象素插值(g点)
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
void IntpQuarterPel4x4_13_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_21_map
功能         ： 4x4块的四分之一象素插值(i点)
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
void IntpQuarterPel4x4_21_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_23_map
功能         ： 4x4块的四分之一象素插值(k点)
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
void IntpQuarterPel4x4_23_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_30_map
功能         ： 4x4块的四分之一象素插值(n点)
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
void IntpQuarterPel4x4_30_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_31_map
功能         ： 4x4块的四分之一象素插值(p点)
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
void IntpQuarterPel4x4_31_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_32_map
功能         ： 4x4块的四分之一象素插值(q点)
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
void IntpQuarterPel4x4_32_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_33_map
功能         ： 4x4块的四分之一象素插值(r点)
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
void IntpQuarterPel4x4_33_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_01_map
功能         ： 8x8块的四分之一象素插值(a点)
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
void IntpQuarterPel8x8_01_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_03_map
功能         ： 8x8块的四分之一象素插值(c点)
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
void IntpQuarterPel8x8_03_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPelV8x8_10_map
功能         ： 8x8块的四分之一象素插值(d点)
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
void IntpQuarterPel8x8_10_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_11_map
功能         ： 8x8块的四分之一象素插值(e点)
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
void IntpQuarterPel8x8_11_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_12_map
功能         ： 8x8块的四分之一象素插值(f点)
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
void IntpQuarterPel8x8_12_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_13_map
功能         ： 8x8块的四分之一象素插值(g点)
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
void IntpQuarterPel8x8_13_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_21_map
功能         ： 8x8块的四分之一象素插值(i点)
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
void IntpQuarterPel8x8_21_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_23_map
功能         ： 8x8块的四分之一象素插值(k点)
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
void IntpQuarterPel8x8_23_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);


/*====================================================================
函数名       :  IntpQuarterPel8x8_30_map
功能         ： 8x8块的四分之一象素插值(n点)
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
void IntpQuarterPel8x8_30_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_31_map
功能         ： 8x8块的四分之一象素插值(p点)
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
void IntpQuarterPel8x8_31_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_32_map
功能         ： 8x8块的四分之一象素插值(q点)
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
void IntpQuarterPel8x8_32_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_33_map
功能         ： 8x8块的四分之一象素插值(r点)
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
void IntpQuarterPel8x8_33_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_01_map
功能         ： 16x16块的四分之一象素插值(a点)
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
void IntpQuarterPel16x16_01_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_03_map
功能         ： 16x16块的四分之一象素插值(c点)
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
void IntpQuarterPel16x16_03_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPelV16x16_10_map
功能         ： 16x16块的四分之一象素插值(d点)
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
void IntpQuarterPel16x16_10_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_11_map
功能         ： 16x16块的四分之一象素插值(e点)
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
void IntpQuarterPel16x16_11_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_12_map
功能         ： 16x16块的四分之一象素插值(f点)
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
void IntpQuarterPel16x16_12_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_13_map
功能         ： 16x16块的四分之一象素插值(g点)
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
void IntpQuarterPel16x16_13_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_21_map
功能         ： 16x16块的四分之一象素插值(i点)
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
void IntpQuarterPel16x16_21_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_23_map
功能         ： 16x16块的四分之一象素插值(k点)
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
void IntpQuarterPel16x16_23_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_30_map
功能         ： 16x16块的四分之一象素插值(n点)
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
void IntpQuarterPel16x16_30_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_31_map
功能         ： 16x16块的四分之一象素插值(p点)
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
void IntpQuarterPel16x16_31_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_32_map
功能         ： 16x16块的四分之一象素插值(q点)
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
void IntpQuarterPel16x16_32_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_33_map
功能         ： 16x16块的四分之一象素插值(r点)
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
void IntpQuarterPel16x16_33_map(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);


//QUANT/DEQUANT
#define	  H264DCQuant2x2		H264DCQuant2x2_map
#define	  H264Quant4x4			H264Quant4x4_map
#define	  H264DCQuant4x4		H264DCQuant4x4_map
#define	  H264ACQuant4x4		H264ACQuant4x4_map
#define	  H264DCDequant2x2		H264DCDequant2x2_map
#define	  H264Dequant4x4		H264Dequant4x4_map
#define	  H264DCDequant4x4		H264DCDequant4x4_map
#define	  H264ACDequant4x4		H264ACDequant4x4_map

#define	  H263QuantIntra		H263QuantIntra_map    
#define   H263QuantInter		H263QuantInter_map    
#define   H263DequantIntra		H263DequantIntra_map  
#define   H263DequantInter		H263DequantInter_map  
#define	  H263DequantIntraVlx	H263DequantIntraVlx_map  
#define   H263DequantInterVlx	H263DequantInterVlx_map
 
//MPEG2QUANT/MPEG4DEQUANT  
#define   Mpeg2QuantIntra        Mpeg2QuantIntra_map
#define   Mpeg2QuantInter        Mpeg2QuantInter_map
#define   Mpeg2DequantIntra      Mpeg2DequantIntra_map
#define   Mpeg2DequantInter      Mpeg2DequantInter_map
#define   Mpeg2DequantIntraVlx   Mpeg2DequantIntraVlx_map
#define   Mpeg2DequantInterVlx   Mpeg2DequantInterVlx_map
 
//MPEG4QUANT/MPEG4DEQUANT
#define   Mpeg4QuantIntra   Mpeg4QuantIntra_map
#define   Mpeg4QuantInter   Mpeg4QuantInter_map  
#define   Mpeg4DequantIntra Mpeg4DequantIntra_map 
#define   Mpeg4DequantInter Mpeg4DequantInter_map

//DCT/IDCT
#define   DCT8x8        DCT8x8_map  
#define   DCT4x4        DCT4x4_map  
#define   IDCT8x8       IDCT8x8_map 
#define   IDCT4x4       IDCT4x4_map 

//Hadamard/IHadmard
#define   Hadamard2x2	Hadamard2x2_map
#define   Hadamard4x4	Hadamard4x4_map

#define   IHadamard2x2	IHadamard2x2_map
#define   IHadamard4x4	IHadamard4x4_map

//SAD
#define   SAD16x16           SAD16x16_map  
#define   SADNoStride16x16   SADNoStride16x16_map 
#define   SAD8x8             SAD8x8_map
#define   SADNoStride8x8     SADNoStride8x8_map 
#define   RowSAD16           RowSAD16_map

//DEVIATIONAMB
#define    DeviationMB     DeviationMB_map

//INTERPOLATION

//4x4 1/4像素插值
#define IntpQuarterPel4x4_01		IntpQuarterPel4x4_01_map
#define IntpQuarterPel4x4_03		IntpQuarterPel4x4_03_map
#define IntpQuarterPel4x4_10		IntpQuarterPel4x4_10_map
#define IntpQuarterPel4x4_11		IntpQuarterPel4x4_11_map
#define IntpQuarterPel4x4_12		IntpQuarterPel4x4_12_map
#define IntpQuarterPel4x4_13		IntpQuarterPel4x4_13_map
#define IntpQuarterPel4x4_21		IntpQuarterPel4x4_21_map
#define IntpQuarterPel4x4_23		IntpQuarterPel4x4_23_map
#define IntpQuarterPel4x4_30		IntpQuarterPel4x4_30_map
#define IntpQuarterPel4x4_31		IntpQuarterPel4x4_31_map
#define IntpQuarterPel4x4_32		IntpQuarterPel4x4_32_map
#define IntpQuarterPel4x4_33		IntpQuarterPel4x4_33_map

//4x4 2Tap 半像素插值
#define Intp2TapHalfPelH4x4		    Intp2TapHalfPelH4x4_map
#define Intp2TapHalfPelV4x4		    Intp2TapHalfPelV4x4_map
#define Intp2TapHalfPelHV4x4		Intp2TapHalfPelHV4x4_map

//4x4 6Tap 半像素插值
#define Intp6TapHalfPelH4x4		    Intp6TapHalfPelH4x4_map
#define Intp6TapHalfPelV4x4		    Intp6TapHalfPelV4x4_map
#define Intp6TapHalfPelHV4x4		Intp6TapHalfPelHV4x4_map

//8x8 1/4像素插值
#define IntpQuarterPel8x8_01		IntpQuarterPel8x8_01_map
#define IntpQuarterPel8x8_03		IntpQuarterPel8x8_03_map
#define IntpQuarterPel8x8_10		IntpQuarterPel8x8_10_map
#define IntpQuarterPel8x8_11		IntpQuarterPel8x8_11_map
#define IntpQuarterPel8x8_12		IntpQuarterPel8x8_12_map
#define IntpQuarterPel8x8_13		IntpQuarterPel8x8_13_map
#define IntpQuarterPel8x8_21		IntpQuarterPel8x8_21_map
#define IntpQuarterPel8x8_23		IntpQuarterPel8x8_23_map
#define IntpQuarterPel8x8_30		IntpQuarterPel8x8_30_map
#define IntpQuarterPel8x8_31		IntpQuarterPel8x8_31_map
#define IntpQuarterPel8x8_32		IntpQuarterPel8x8_32_map
#define IntpQuarterPel8x8_33		IntpQuarterPel8x8_33_map

//8x8 2Tap 半像素插值
#define Intp2TapHalfPelHRounding8x8		Intp2TapHalfPelHRounding8x8_map
#define Intp2TapHalfPelVRounding8x8		Intp2TapHalfPelVRounding8x8_map
#define Intp2TapHalfPelHVRounding8x8		Intp2TapHalfPelHVRounding8x8_map

#define Intp2TapHalfPelH8x8		    Intp2TapHalfPelH8x8_map
#define Intp2TapHalfPelV8x8		    Intp2TapHalfPelV8x8_map
#define Intp2TapHalfPelHV8x8		Intp2TapHalfPelHV8x8_map

//8x8 6Tap 半像素插值
#define Intp6TapHalfPelH8x8		    Intp6TapHalfPelH8x8_map
#define Intp6TapHalfPelV8x8		    Intp6TapHalfPelV8x8_map
#define Intp6TapHalfPelHV8x8		Intp6TapHalfPelHV8x8_map

//16x16 1/4像素插值
#define IntpQuarterPel16x16_01		IntpQuarterPel16x16_01_map
#define IntpQuarterPel16x16_03		IntpQuarterPel16x16_03_map
#define IntpQuarterPel16x16_10		IntpQuarterPel16x16_10_map
#define IntpQuarterPel16x16_11		IntpQuarterPel16x16_11_map
#define IntpQuarterPel16x16_12		IntpQuarterPel16x16_12_map
#define IntpQuarterPel16x16_13		IntpQuarterPel16x16_13_map
#define IntpQuarterPel16x16_21		IntpQuarterPel16x16_21_map
#define IntpQuarterPel16x16_23		IntpQuarterPel16x16_23_map
#define IntpQuarterPel16x16_30		IntpQuarterPel16x16_30_map
#define IntpQuarterPel16x16_31		IntpQuarterPel16x16_31_map
#define IntpQuarterPel16x16_32		IntpQuarterPel16x16_32_map
#define IntpQuarterPel16x16_33		IntpQuarterPel16x16_33_map

//16x16 2Tap 半像素插值
#define Intp2TapHalfPelHRounding16x16		    Intp2TapHalfPelHRounding16x16_map
#define Intp2TapHalfPelVRounding16x16		    Intp2TapHalfPelVRounding16x16_map
#define Intp2TapHalfPelHVRounding16x16		Intp2TapHalfPelHVRounding16x16_map

#define Intp2TapHalfPelH16x16		Intp2TapHalfPelH16x16_map
#define Intp2TapHalfPelV16x16		Intp2TapHalfPelV16x16_map
#define Intp2TapHalfPelHV16x16		Intp2TapHalfPelHV16x16_map

//16x16 6Tap 半像素插值
#define Intp6TapHalfPelH16x16		Intp6TapHalfPelH16x16_map
#define Intp6TapHalfPelV16x16		Intp6TapHalfPelV16x16_map
#define Intp6TapHalfPelHV16x16		Intp6TapHalfPelHV16x16_map



#elif defined(MAP_INLINE_CODE)   //采用inline形式

inline_declare
{
		
	#include "../../../30-video/CommonLib/source/equator/DCT_IDCT.c"
	#include "../../../30-video/CommonLib/source/equator/DeviationMB.c"
	#include "../../../30-video/CommonLib/source/equator/QuantDequant.c"
	#include "../../../30-video/CommonLib/source/equator/SAD.c"
	#include "../../../30-video/CommonLib/source/equator/Interpolation.c"


}  

#elif defined(SSE2_CODE)

/*====================================================================
	函数名	    :	H264DCQuant2x2_ia
	功能		:	实现 2x2 DC块的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264DCQuant2x2_ia(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264Quant4x4_ia
	功能		:	实现 4x4 块的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264Quant4x4_ia(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264DCQuant4x4_ia
	功能		:	实现 4x4 块DC的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264DCQuant4x4_ia(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264ACQuant4x4_ia
	功能		:	实现 4x4 块AC的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264ACQuant4x4_ia(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264DCDequant2x2_ia
	功能		:	实现 2x2 DC块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵，
					s16 *ps16OutCoeff	:输出结果
					u8 u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264DCDequant2x2_ia(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*====================================================================
	函数名	    :	H264Dequant4x4_ia
	功能		:	实现 4x4 块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵，
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264Dequant4x4_ia(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*====================================================================
	函数名	    :	H264DCDequant4x4_ia
	功能		:	实现 4x4 DC块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8 u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264DCDequant4x4_ia(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*====================================================================
	函数名	    :	H264ACDequant4x4_ia
	功能		:	实现 4x4 AC块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8 u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264ACDequant4x4_ia(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263QuantIntra_ia
功能		：对8x8的Intra块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff		: 存放量化后结果
              u8  u8Quant			: 它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：无。
=============================================================================*/
u32 H263QuantIntra_ia(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263QuantInter_ia
功能		：对8x8的Inter块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:待量化的8x8块
              s16  *ps16OutCoeff		:存放量化后结果
              u8  u8Quant			:它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：量化系数的和
=============================================================================*/
u32 H263QuantInter_ia(const s16 *ps16InCoeff,s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263DequantIntra_ia
功能		：对8x8的Intra块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		 ：待量化的8x8块
              s16  *ps16OutCoeff		 ：存放量化后结果
              u8  u8Quant			 ：它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：void
=============================================================================*/
void H263DequantIntra_ia(s16 *ps16InCoeff,s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263DequantInter_ia
功能		：对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff		: 存放量化后结果
              u8  u8Quant			:它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：
=============================================================================*/
void H263DequantInter_ia(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg4QuantIntra_ia
功能		：对8x8的Intra块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8Quant			: 量化参数
              u8  u8DcScaler		: DC系数的量化参数   
注意：
返回值说明  ：
=============================================================================*/
u32 Mpeg4QuantIntra_ia(const s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg4QuantInter_ia
功能		：对8x8的Inter块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:待量化的8x8块
              s16  *ps16OutCoeff		:存放量化后结果
              u8  u8Quant			:量化参数 
注意：
返回值说明  ：
=============================================================================*/
u32 Mpeg4QuantInter_ia(const s16 *ps16InCoeff, s16 * ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg4DequantIntra_ia
功能		：对8x8的Intra块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8Quant			: 量化参数
              u8  u8DcScaler		: DC系数的量化参数   
注意：
返回值说明  ：
=============================================================================*/
void Mpeg4DequantIntra_ia(const s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg4DequantInter_ia
功能		：对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:待量化的8x8块
              s16  *ps16OutCoeff		:存放量化后结果
              u8  u8Quant			:量化参数 
注意：
返回值说明  ：
=============================================================================*/
void Mpeg4DequantInter_ia(const s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg2QuantIntra_ia
功能		：Mpeg2 对8x8的Intra块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8DcScaler		: DC系数的量化参数
              u8  u8Quant			:量化步长   
注意：
返回值说明  ：
=============================================================================*/
void Mpeg2QuantIntra_ia(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg2QuantInter_ia
功能		：Mpeg2 对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:  待量化的8x8块
              s16  *ps16OutCoeff		: 存放量化后结果
              u8  u8Quant			:  量化步长
注意：
返回值说明  ：返回块中是否都为0
=============================================================================*/
u16 Mpeg2QuantInter_ia(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg2DequantIntra_ia
功能		：Mpeg2 对8x8的Intra块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8Quant			: 量化参数
              u8  u8DcScaler		: DC系数的量化参数 
注意：
返回值说明  ：
=============================================================================*/
void Mpeg2DequantIntra_ia(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg2DequantInter_ia
功能		：Mpeg2 对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:  待量化的8x8块
              s16  *ps16OutCoeff	:  存放量化后结果
              u8  u8Quant			:  量化步长
注意：
返回值说明  ：
=============================================================================*/
void Mpeg2DequantInter_ia(s16 *ps16InCoeff, s16  *ps16OutCoeff,  u8 u8Quant);


//DCT/IDCT
/*====================================================================
函数名       :  DCT4x4_ia
功能         ： 4×4模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock  ： 指向4×4模块
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void  DCT4x4_ia(s16* ps16Block);

/*====================================================================
函数名       :  DCT8x8_ia
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock   ： 指向8×8模块
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void  DCT8x8_ia(s16* ps16Block);

//8×8和4x4块DCT变换
/*====================================================================
函数名       :  IDCT4x4_map
功能         ： 4×4模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock  ：  指向4×4模块
返回值说明   ： 无
特殊说明     ： pBlock地址需要32位对齐
------------------------------------------------------------------------
修改记录    :
日期：      版本        修改人      修改内容

======================================================================*/
void  IDCT4x4_ia(s16* ps16Block);

/*====================================================================
函数名       :  IDCT8x8_ia
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock  ：  指向8×8模块
返回值说明   ： 无
特殊说明     ： pBlock地址需要32位对齐
------------------------------------------------------------------------
修改记录    :
日期：      版本        修改人      修改内容

======================================================================*/
void  IDCT8x8_ia(s16* ps16Block);

//Hadamard/IHadmard
/*====================================================================
	函数名	    ：Hadamard2x2_ia
	功能		：实现2x2 Hadamard变换
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16 * ps16InBlock		: 输入的2x2块，
                  s16 * ps16OutBlock	: 输出结果2x2块
	返回值说明  ：无
====================================================================*/
void Hadamard2x2_ia(s16 *ps16InBlock, s16 *ps16OutBlock);

/*====================================================================
	函数名	    ：IHadamard2x2_ia
	功能		：实现2x2 IHadamard变换
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16 *ps16InBlock  : 输入的2x2块，
                  s16 *ps16OutBlock : 输出结果2x2块
	返回值说明  ：无
====================================================================*/
void IHadamard2x2_ia(s16 *ps16Block, s16 *ps16OutBlock);

/*====================================================================
	函数名	    ：Hadamard4x4_ia
	功能		：实现4x4 Hadamard变换
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16 as16Block[16]：输入的4x4块，也作为输出结果
	返回值说明  ：无
====================================================================*/
void Hadamard4x4_ia (s16 as16Block[16]);

/*====================================================================
	函数名	    ：IHadamard4x4_ia
	功能		：实现4x4块的哈达码反变换（IHadamard）
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16  as16Block[16] ：输入的4x4块，也作为输出结果;
	返回值说明  ：无
====================================================================*/
void IHadamard4x4_ia (s16 as16Block[16]);

//SAD
/*====================================================================
函数名       :  SAD16x16_ia
功能         ： 16x16模块SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u16  u16CurrStride  ：当前宏块步长
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride   ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SAD16x16_ia(u8 *pu8CurrMB, u16 u16CurrStride, u8 *pu8RefMB, u16 u16RefStride);

/*====================================================================
函数名       :  SADNoStride16x16_ia
功能         ： 16x16模块无步长SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride   ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SADNoStride16x16_ia(u8 *pu8CurrMB, u8 *pu8RefMB, u16 u16RefStride);

/*====================================================================
函数名       :  SAD8x8_ia
功能         ： 8x8模块SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u16  u16CurrStride  ：当前宏块步长
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride   ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SAD8x8_ia(u8 *pu8CurrMB, u16 u16CurrStride, u8 *pu8RefMB, u16 u16RefStride);

/*====================================================================
函数名       :  SADNoStride8x8_ia
功能         ： 8x8模块无步长SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride  ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SADNoStride8x8_ia(u8 *pu8CurrMB, u8 *pu8RefMB, u16 u16RefStride);


//DEVIATION
/*====================================================================
函数名       :  DeviationMB_ia
功能         ： 求宏块内各象素值与平均值差的绝对值之和
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8MB ：指向宏块指针
                 u16 u16Stride	  ：宏块步长                 
返回值说明   ： 宏块内各象素值与宏块内象素平均值差的绝对值之和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
u32 DeviationMB_ia(const u8 *pu8MB, u16 u16Stride);

//Interpolation
/*====================================================================
函数名       :  Intp2TapHalfPelH4x4_ia
功能         ： 4x4块的水平方向2Tap半象素插值
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
void Intp2TapHalfPelH4x4_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelV4x4_ia
功能         ： 4x4块的垂直方向2Tap半象素插值
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
void Intp2TapHalfPelV4x4_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHV4x4_ia
功能         ： 4x4块的对角方向2Tap半象素插值
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
void Intp2TapHalfPelHV4x4_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelH4x4_ia
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
void Intp6TapHalfPelH4x4_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelV4x4_ia
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
void Intp6TapHalfPelV4x4_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelHV4x4_ia
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
void Intp6TapHalfPelHV4x4_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHRounding8x8_ia
功能         ： 8x8块的水平方向2Tap半象素插值
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
void Intp2TapHalfPelHRounding8x8_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelH8x8_ia
功能         ： 8x8块的水平方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelH8x8_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelVRounding8x8_ia
功能         ： 8x8块的垂直方向2Tap半象素插值
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
void Intp2TapHalfPelVRounding8x8_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelV8x8_ia
功能         ： 8x8块的垂直方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelV8x8_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelHVRounding8x8_ia
功能         ： 8x8块的对角方向2Tap半象素插值
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
void Intp2TapHalfPelHVRounding8x8_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHV8x8_ia
功能         ： 8x8块的对角方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelHV8x8_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp6TapHalfPelH8x8_ia
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
void Intp6TapHalfPelH8x8_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelIntH8x8_ia
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
void Intp6TapHalfPelIntH8x8_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelV8x8_ia
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
void Intp6TapHalfPelV8x8_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelIntV8x8_ia
功能         ： 8x8块的垂直方向6Tap半象素插值
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
void Intp6TapHalfPelIntV8x8_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, l32 *pl32IntDst);

/*====================================================================
函数名       :  Intp6TapHalfPelHV8x8_ia
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
void Intp6TapHalfPelHV8x8_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHRounding16x16_ia
功能         ： 16x16源块的水平方向2Tap半象素插值
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
void Intp2TapHalfPelHRounding16x16_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelH16x16_ia
功能         ： 16x16源块的水平方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelH16x16_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelVRounding16x16_ia
功能         ： 16x16源块的垂直方向2Tap半象素插值
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
void Intp2TapHalfPelVRounding16x16_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelV16x16_ia
功能         ： 16x16源块的垂直方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelV16x16_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelHVRounding16x16_ia
功能         ： 源块的对角方向2Tap半象素插值
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
void Intp2TapHalfPelHVRounding16x16_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHV16x16_ia
功能         ： 源块的对角方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelHV16x16_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp6TapHalfPelH16x16_ia
功能         ： 源块的水平方向6Tap半象素插值
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
void Intp6TapHalfPelH16x16_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelV16x16_ia
功能         ： 16x16宏块的垂直方向6Tap半象素插值
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
void Intp6TapHalfPelV16x16_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelHV16x16_ia
功能         ： 16x16宏块的对角方向6Tap半象素插值
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
void Intp6TapHalfPelHV16x16_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_01_ia
功能         ： 4x4块的四分之一象素插值(a点)
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
void IntpQuarterPel4x4_01_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_03_ia
功能         ： 4x4块的四分之一象素插值(c点)
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
void IntpQuarterPel4x4_03_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPelV4x4_10_ia
功能         ： 4x4块的四分之一象素插值(d点)
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
void IntpQuarterPel4x4_10_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_11_ia
功能         ： 4x4块的四分之一象素插值(e点)
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
void IntpQuarterPel4x4_11_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_12_ia
功能         ： 4x4块的四分之一象素插值(f点)
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
void IntpQuarterPel4x4_12_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_13_ia
功能         ： 4x4块的四分之一象素插值(g点)
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
void IntpQuarterPel4x4_13_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_21_ia
功能         ： 4x4块的四分之一象素插值(i点)
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
void IntpQuarterPel4x4_21_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_23_ia
功能         ： 4x4块的四分之一象素插值(k点)
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
void IntpQuarterPel4x4_23_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_30_ia
功能         ： 4x4块的四分之一象素插值(n点)
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
void IntpQuarterPel4x4_30_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_31_ia
功能         ： 4x4块的四分之一象素插值(p点)
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
void IntpQuarterPel4x4_31_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_32_ia
功能         ： 4x4块的四分之一象素插值(q点)
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
void IntpQuarterPel4x4_32_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_33_ia
功能         ： 4x4块的四分之一象素插值(r点)
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
void IntpQuarterPel4x4_33_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_01_ia
功能         ： 8x8块的四分之一象素插值(a点)
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
void IntpQuarterPel8x8_01_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_03_ia
功能         ： 8x8块的四分之一象素插值(c点)
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
void IntpQuarterPel8x8_03_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPelV8x8_10_ia
功能         ： 8x8块的四分之一象素插值(d点)
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
void IntpQuarterPel8x8_10_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_11_ia
功能         ： 8x8块的四分之一象素插值(e点)
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
void IntpQuarterPel8x8_11_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_12_ia
功能         ： 8x8块的四分之一象素插值(f点)
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
void IntpQuarterPel8x8_12_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_13_ia
功能         ： 8x8块的四分之一象素插值(g点)
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
void IntpQuarterPel8x8_13_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_21_ia
功能         ： 8x8块的四分之一象素插值(i点)
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
void IntpQuarterPel8x8_21_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_23_ia
功能         ： 8x8块的四分之一象素插值(k点)
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
void IntpQuarterPel8x8_23_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);


/*====================================================================
函数名       :  IntpQuarterPel8x8_30_ia
功能         ： 8x8块的四分之一象素插值(n点)
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
void IntpQuarterPel8x8_30_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_31_ia
功能         ： 8x8块的四分之一象素插值(p点)
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
void IntpQuarterPel8x8_31_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_32_ia
功能         ： 8x8块的四分之一象素插值(q点)
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
void IntpQuarterPel8x8_32_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_33_ia
功能         ： 8x8块的四分之一象素插值(r点)
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
void IntpQuarterPel8x8_33_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_01_ia
功能         ： 16x16块的四分之一象素插值(a点)
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
void IntpQuarterPel16x16_01_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_03_ia
功能         ： 16x16块的四分之一象素插值(c点)
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
void IntpQuarterPel16x16_03_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPelV16x16_10_ia
功能         ： 16x16块的四分之一象素插值(d点)
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
void IntpQuarterPel16x16_10_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_11_ia
功能         ： 16x16块的四分之一象素插值(e点)
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
void IntpQuarterPel16x16_11_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_12_ia
功能         ： 16x16块的四分之一象素插值(f点)
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
void IntpQuarterPel16x16_12_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_13_ia
功能         ： 16x16块的四分之一象素插值(g点)
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
void IntpQuarterPel16x16_13_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_21_ia
功能         ： 16x16块的四分之一象素插值(i点)
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
void IntpQuarterPel16x16_21_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_23_ia
功能         ： 16x16块的四分之一象素插值(k点)
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
void IntpQuarterPel16x16_23_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_30_ia
功能         ： 16x16块的四分之一象素插值(n点)
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
void IntpQuarterPel16x16_30_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_31_ia
功能         ： 16x16块的四分之一象素插值(p点)
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
void IntpQuarterPel16x16_31_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_32_ia
功能         ： 16x16块的四分之一象素插值(q点)
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
void IntpQuarterPel16x16_32_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_33_ia
功能         ： 16x16块的四分之一象素插值(r点)
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
void IntpQuarterPel16x16_33_ia(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);


//QUANT/DEQUANT
#define	  H264DCQuant2x2		H264DCQuant2x2_ia
#define	  H264Quant4x4			H264Quant4x4_ia
#define	  H264DCQuant4x4		H264DCQuant4x4_ia
#define	  H264ACQuant4x4		H264ACQuant4x4_ia
#define	  H264DCDequant2x2		H264DCDequant2x2_ia
#define	  H264Dequant4x4		H264Dequant4x4_ia
#define	  H264DCDequant4x4		H264DCDequant4x4_ia
#define	  H264ACDequant4x4		H264ACDequant4x4_ia

#define	  H263QuantIntra       H263QuantIntra_ia    
#define   H263QuantInter       H263QuantInter_ia    
#define   H263DequantIntra     H263DequantIntra_ia  
#define   H263DequantInter     H263DequantInter_ia  

//MPEG4QUANT/MPEG4DEQUANT
#define   Mpeg4QuantIntra   Mpeg4QuantIntra_ia
#define   Mpeg4QuantInter   Mpeg4QuantInter_ia  
#define   Mpeg4DequantIntra Mpeg4DequantIntra_ia 
#define   Mpeg4DequantInter Mpeg4DequantInter_ia


//MPEG2QUANT/MPEG2DEQUANT
#define  Mpeg2QuantIntra     Mpeg2QuantIntra_ia
#define  Mpeg2QuantInter     Mpeg2QuantInter_ia
#define  Mpeg2DequantIntra   Mpeg2DequantIntra_ia
#define  Mpeg2DequantInter   Mpeg2DequantInter_ia

//Hadamard/IHadmard
#define   Hadamard2x2	Hadamard2x2_ia
#define   Hadamard4x4	Hadamard4x4_ia

#define   IHadamard2x2	IHadamard2x2_ia
#define   IHadamard4x4	IHadamard4x4_ia

//DCT/IDCT
#define   DCT4x4        DCT4x4_ia  
#define   DCT8x8        DCT8x8_ia  
#define   IDCT4x4       IDCT4x4_ia 
#define   IDCT8x8       IDCT8x8_ia 

//SAD
#define   SAD16x16           SAD16x16_ia  
#define   SADNoStride16x16   SADNoStride16x16_ia  
#define   SAD8x8             SAD8x8_ia
#define   SADNoStride8x8     SADNoStride8x8_ia
//DEVIATIONAMB
#define    DeviationMB     DeviationMB_ia

//INTERPOLATION
//4x4 1/4像素插值
#define IntpQuarterPel4x4_01		IntpQuarterPel4x4_01_ia
#define IntpQuarterPel4x4_03		IntpQuarterPel4x4_03_ia
#define IntpQuarterPel4x4_10		IntpQuarterPel4x4_10_ia
#define IntpQuarterPel4x4_11		IntpQuarterPel4x4_11_ia
#define IntpQuarterPel4x4_12		IntpQuarterPel4x4_12_ia
#define IntpQuarterPel4x4_13		IntpQuarterPel4x4_13_ia
#define IntpQuarterPel4x4_21		IntpQuarterPel4x4_21_ia
#define IntpQuarterPel4x4_23		IntpQuarterPel4x4_23_ia
#define IntpQuarterPel4x4_30		IntpQuarterPel4x4_30_ia
#define IntpQuarterPel4x4_31		IntpQuarterPel4x4_31_ia
#define IntpQuarterPel4x4_32		IntpQuarterPel4x4_32_ia
#define IntpQuarterPel4x4_33		IntpQuarterPel4x4_33_ia

//4x4 2Tap 半像素插值
#define Intp2TapHalfPelH4x4		    Intp2TapHalfPelH4x4_ia
#define Intp2TapHalfPelV4x4		    Intp2TapHalfPelV4x4_ia
#define Intp2TapHalfPelHV4x4		Intp2TapHalfPelHV4x4_ia

//4x4 6Tap 半像素插值
#define Intp6TapHalfPelH4x4		    Intp6TapHalfPelH4x4_ia
#define Intp6TapHalfPelV4x4		    Intp6TapHalfPelV4x4_ia
#define Intp6TapHalfPelHV4x4		Intp6TapHalfPelHV4x4_ia

//8x8 1/4像素插值
#define IntpQuarterPel8x8_01		IntpQuarterPel8x8_01_ia
#define IntpQuarterPel8x8_03		IntpQuarterPel8x8_03_ia
#define IntpQuarterPel8x8_10		IntpQuarterPel8x8_10_ia
#define IntpQuarterPel8x8_11		IntpQuarterPel8x8_11_ia
#define IntpQuarterPel8x8_12		IntpQuarterPel8x8_12_ia
#define IntpQuarterPel8x8_13		IntpQuarterPel8x8_13_ia
#define IntpQuarterPel8x8_21		IntpQuarterPel8x8_21_ia
#define IntpQuarterPel8x8_23		IntpQuarterPel8x8_23_ia
#define IntpQuarterPel8x8_30		IntpQuarterPel8x8_30_ia
#define IntpQuarterPel8x8_31		IntpQuarterPel8x8_31_ia
#define IntpQuarterPel8x8_32		IntpQuarterPel8x8_32_ia
#define IntpQuarterPel8x8_33		IntpQuarterPel8x8_33_ia

//8x8 2Tap 半像素插值
#define Intp2TapHalfPelHRounding8x8		Intp2TapHalfPelHRounding8x8_ia
#define Intp2TapHalfPelVRounding8x8		Intp2TapHalfPelVRounding8x8_ia
#define Intp2TapHalfPelHVRounding8x8		Intp2TapHalfPelHVRounding8x8_ia

#define Intp2TapHalfPelH8x8		    Intp2TapHalfPelH8x8_ia
#define Intp2TapHalfPelV8x8		    Intp2TapHalfPelV8x8_ia
#define Intp2TapHalfPelHV8x8		Intp2TapHalfPelHV8x8_ia

//8x8 6Tap 半像素插值
#define Intp6TapHalfPelH8x8		    Intp6TapHalfPelH8x8_ia
#define Intp6TapHalfPelV8x8		    Intp6TapHalfPelV8x8_ia
#define Intp6TapHalfPelHV8x8		Intp6TapHalfPelHV8x8_ia

//16x16 1/4像素插值
#define IntpQuarterPel16x16_01		IntpQuarterPel16x16_01_ia
#define IntpQuarterPel16x16_03		IntpQuarterPel16x16_03_ia
#define IntpQuarterPel16x16_10		IntpQuarterPel16x16_10_ia
#define IntpQuarterPel16x16_11		IntpQuarterPel16x16_11_ia
#define IntpQuarterPel16x16_12		IntpQuarterPel16x16_12_ia
#define IntpQuarterPel16x16_13		IntpQuarterPel16x16_13_ia
#define IntpQuarterPel16x16_21		IntpQuarterPel16x16_21_ia
#define IntpQuarterPel16x16_23		IntpQuarterPel16x16_23_ia
#define IntpQuarterPel16x16_30		IntpQuarterPel16x16_30_ia
#define IntpQuarterPel16x16_31		IntpQuarterPel16x16_31_ia
#define IntpQuarterPel16x16_32		IntpQuarterPel16x16_32_ia
#define IntpQuarterPel16x16_33		IntpQuarterPel16x16_33_ia

//16x16 2Tap 半像素插值
#define Intp2TapHalfPelHRounding16x16		Intp2TapHalfPelHRounding16x16_ia
#define Intp2TapHalfPelVRounding16x16		Intp2TapHalfPelVRounding16x16_ia
#define Intp2TapHalfPelHVRounding16x16		Intp2TapHalfPelHVRounding16x16_ia

#define Intp2TapHalfPelH16x16		Intp2TapHalfPelH16x16_ia
#define Intp2TapHalfPelV16x16		Intp2TapHalfPelV16x16_ia
#define Intp2TapHalfPelHV16x16		Intp2TapHalfPelHV16x16_ia

//16x16 6Tap 半像素插值
#define Intp6TapHalfPelH16x16		Intp6TapHalfPelH16x16_ia
#define Intp6TapHalfPelV16x16		Intp6TapHalfPelV16x16_ia
#define Intp6TapHalfPelHV16x16		Intp6TapHalfPelHV16x16_ia

#elif defined(BLACKFIN_CODE)
//QUANAT/DEQUANT
/*====================================================================
	函数名	    :	H264DCQuant2x2_bf
	功能		:	实现 2x2 DC块的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264DCQuant2x2_bf(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264Quant4x4_bf
	功能		:	实现 4x4 块的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264Quant4x4_bf(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264DCQuant4x4_bf
	功能		:	实现 4x4 块DC的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264DCQuant4x4_bf(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264ACQuant4x4_bf
	功能		:	实现 4x4 块AC的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264ACQuant4x4_bf(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264DCDequant2x2_bf
	功能		:	实现 2x2 DC块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵，
					s16 *ps16OutCoeff	:输出结果
					u8 u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264DCDequant2x2_bf(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*====================================================================
	函数名	    :	H264Dequant4x4_bf
	功能		:	实现 4x4 块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵，
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264Dequant4x4_bf(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*====================================================================
	函数名	    :	H264DCDequant4x4_bf
	功能		:	实现 4x4 DC块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8 u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264DCDequant4x4_bf(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*====================================================================
	函数名	    :	H264ACDequant4x4_bf
	功能		:	实现 4x4 AC块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8 u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264ACDequant4x4_bf(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263QuantIntra_c
功能		：对8x8的Intra块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff		: 存放量化后结果
              u8  u8Quant			: 它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：无。
=============================================================================*/
u32 H263QuantIntra_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263QuantInter_c
功能		：对8x8的Inter块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:待量化的8x8块
              s16  *ps16OutCoeff		:存放量化后结果
              u8  u8Quant			:它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：量化系数的和
=============================================================================*/
u32 H263QuantInter_c(const s16 *ps16InCoeff,s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263DequantIntra_c
功能		：对8x8的Intra块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		 ：待量化的8x8块
              s16  *ps16OutCoeff		 ：存放量化后结果
              u8  u8Quant			 ：它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：void
=============================================================================*/
void H263DequantIntra_c(s16 *ps16InCoeff,s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263DequantInter_c
功能		：对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff		: 存放量化后结果
              u8  u8Quant			:它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：
=============================================================================*/
void H263DequantInter_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg4QuantIntra_c
功能		：对8x8的Intra块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8Quant			: 量化参数
              u8  u8DcScaler		: DC系数的量化参数   
注意：
返回值说明  ：
=============================================================================*/
u32 Mpeg4QuantIntra_c(const s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg4QuantInter_c
功能		：对8x8的Inter块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:待量化的8x8块
              s16  *ps16OutCoeff		:存放量化后结果
              u8  u8Quant			:量化参数 
注意：
返回值说明  ：
=============================================================================*/
u32 Mpeg4QuantInter_c(const s16 *ps16InCoeff, s16 * ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg4DequantIntra_c
功能		：对8x8的Intra块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8Quant			: 量化参数
              u8  u8DcScaler		: DC系数的量化参数   
注意：
返回值说明  ：
=============================================================================*/
void Mpeg4DequantIntra_c(const s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg4DequantInter_c
功能		：对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:待量化的8x8块
              s16  *ps16OutCoeff		:存放量化后结果
              u8  u8Quant			:量化参数 
注意：
返回值说明  ：
=============================================================================*/
void Mpeg4DequantInter_c(const s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg2QuantIntra_c
功能		：Mpeg2 对8x8的Intra块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8DcScaler		: DC系数的量化参数
              u8  u8Quant			:量化步长   
注意：
返回值说明  ：
=============================================================================*/
void Mpeg2QuantIntra_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg2QuantInter_c
功能		：Mpeg2 对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:  待量化的8x8块
              s16  *ps16OutCoeff		: 存放量化后结果
              u8  u8Quant			:  量化步长
注意：
返回值说明  ：返回块中是否都为0
=============================================================================*/
u16 Mpeg2QuantInter_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg2DequantIntra_c
功能		：Mpeg2 对8x8的Intra块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8Quant			: 量化参数
              u8  u8DcScaler		: DC系数的量化参数 
注意：
返回值说明  ：
=============================================================================*/
void Mpeg2DequantIntra_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg2DequantInter_c
功能		：Mpeg2 对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:  待量化的8x8块
              s16  *ps16OutCoeff	:  存放量化后结果
              u8  u8Quant			:  量化步长
注意：
返回值说明  ：
=============================================================================*/
void Mpeg2DequantInter_c(s16 *ps16InCoeff, s16  *ps16OutCoeff,  u8 u8Quant);



//DCT/IDCT
/*====================================================================
函数名       :  DCT4x4_bf
功能         ： 4×4模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock  ： 指向4×4模块
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void  DCT4x4_bf(s16* ps16Block);
/*====================================================================
函数名       :  DCT8x8_bf
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock   ： 指向8×8模块
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void  DCT8x8_bf(s16* ps16Block);

//8×8和4x4块DCT变换
/*====================================================================
函数名       :  IDCT4x4_bf
功能         ： 4×4模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock  ：  指向4×4模块
返回值说明   ： 无
特殊说明     ： pBlock地址需要32位对齐
------------------------------------------------------------------------
修改记录    :
日期：      版本        修改人      修改内容

======================================================================*/
void  IDCT4x4_bf(s16* ps16Block);

/*====================================================================
函数名       :  IDCT8x8_bf
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock  ：  指向8×8模块
返回值说明   ： 无
特殊说明     ： pBlock地址需要32位对齐
------------------------------------------------------------------------
修改记录    :
日期：      版本        修改人      修改内容

======================================================================*/
void  IDCT8x8_bf(s16* ps16Block);

//Hadamard/IHadmard
/*====================================================================
	函数名	    ：Hadamard2x2_bf
	功能		：实现2x2 Hadamard变换
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16 * ps16InBlock		: 输入的2x2块，
                  s16 * ps16OutBlock	: 输出结果2x2块
	返回值说明  ：无
====================================================================*/
void Hadamard2x2_bf(s16 *ps16InBlock, s16 *ps16OutBlock);

/*====================================================================
	函数名	    ：IHadamard2x2_bf
	功能		：实现2x2 IHadamard变换
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16 *ps16InBlock  : 输入的2x2块，
                  s16 *ps16OutBlock : 输出结果2x2块
	返回值说明  ：无
====================================================================*/
void IHadamard2x2_bf(s16 *ps16Block, s16 *ps16OutBlock);

/*====================================================================
	函数名	    ：Hadamard4x4_bf
	功能		：实现4x4 Hadamard变换
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16 as16Block[16]：输入的4x4块，也作为输出结果
	返回值说明  ：无
====================================================================*/
void Hadamard4x4_bf (s16 as16Block[16]);

/*====================================================================
	函数名	    ：IHadamard4x4_bf
	功能		：实现4x4块的哈达码反变换（IHadamard）
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16  as16Block[16] ：输入的4x4块，也作为输出结果;
	返回值说明  ：无
====================================================================*/
void IHadamard4x4_bf (s16 as16Block[16]);

//SAD
/*====================================================================
函数名       :  SAD16x16_bf
功能         ： 16x16模块SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u16  u16CurrStride  ：当前宏块步长
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride   ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SAD16x16_bf(u8 *pu8CurrMB, u16 u16CurrStride, u8 *pu8RefMB, u16 u16RefStride);

/*====================================================================
函数名       :  SADNoStride16x16_c
功能         ： 16x16模块无步长SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride   ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SADNoStride16x16_bf(u8 *pu8CurrMB, u8 *pu8RefMB, u16 u16RefStride);

/*====================================================================
函数名       :  SAD8x8_c
功能         ： 8x8模块SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u16  u16CurrStride  ：当前宏块步长
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride   ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SAD8x8_bf(u8 *pu8CurrMB, u16 u16CurrStride, u8 *pu8RefMB, u16 u16RefStride);

/*====================================================================
函数名       :  SADNoStride8x8_c
功能         ： 8x8模块无步长SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride  ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SADNoStride8x8_bf(u8 *pu8CurrMB, u8 *pu8RefMB, u16 u16RefStride);


//DEVIATION
/*====================================================================
函数名       :  DeviationMB_bf
功能         ： 求宏块内各象素值与平均值差的绝对值之和
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8MB ：指向宏块指针
                 u16 u16Stride	  ：宏块步长                 
返回值说明   ： 宏块内各象素值与宏块内象素平均值差的绝对值之和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
u32 DeviationMB_bf(const u8 *pu8MB, u16 u16Stride);

//Interpolation
/*====================================================================
函数名       :  Intp2TapHalfPelH4x4_bf
功能         ： 4x4块的水平方向2Tap半象素插值
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
void Intp2TapHalfPelH4x4_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelV4x4_bf
功能         ： 4x4块的垂直方向2Tap半象素插值
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
void Intp2TapHalfPelV4x4_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHV4x4_bf
功能         ： 4x4块的对角方向2Tap半象素插值
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
void Intp2TapHalfPelHV4x4_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelH4x4_bf
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
void Intp6TapHalfPelH4x4_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelV4x4_bf
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
void Intp6TapHalfPelV4x4_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelHV4x4_bf
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
void Intp6TapHalfPelHV4x4_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHRounding8x8_bf
功能         ： 8x8块的水平方向2Tap半象素插值
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
void Intp2TapHalfPelHRounding8x8_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelH8x8_bf
功能         ： 8x8块的水平方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelH8x8_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelVRounding8x8_bf
功能         ： 8x8块的垂直方向2Tap半象素插值
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
void Intp2TapHalfPelVRounding8x8_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelV8x8_bf
功能         ： 8x8块的垂直方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelV8x8_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelHVRounding8x8_bf
功能         ： 8x8块的对角方向2Tap半象素插值
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
void Intp2TapHalfPelHVRounding8x8_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHV8x8_bf
功能         ： 8x8块的对角方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelHV8x8_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp6TapHalfPelH8x8_bf
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
void Intp6TapHalfPelH8x8_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelIntH8x8_bf
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
void Intp6TapHalfPelIntH8x8_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelV8x8_bf
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
void Intp6TapHalfPelV8x8_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelIntV8x8_bf
功能         ： 8x8块的垂直方向6Tap半象素插值
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
void Intp6TapHalfPelIntV8x8_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, l32 *pl32IntDst);

/*====================================================================
函数名       :  Intp6TapHalfPelHV8x8_bf
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
void Intp6TapHalfPelHV8x8_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHRounding16x16_bf
功能         ： 16x16源块的水平方向2Tap半象素插值
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
void Intp2TapHalfPelHRounding16x16_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelH16x16_bf
功能         ： 16x16源块的水平方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelH16x16_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelVRounding16x16_bf
功能         ： 16x16源块的垂直方向2Tap半象素插值
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
void Intp2TapHalfPelVRounding16x16_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelV16x16_bf
功能         ： 16x16源块的垂直方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelV16x16_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelHVRounding16x16_bf
功能         ： 源块的对角方向2Tap半象素插值
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
void Intp2TapHalfPelHVRounding16x16_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHV16x16_bf
功能         ： 源块的对角方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelHV16x16_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp6TapHalfPelH16x16_bf
功能         ： 源块的水平方向6Tap半象素插值
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
void Intp6TapHalfPelH16x16_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelV16x16_bf
功能         ： 16x16宏块的垂直方向6Tap半象素插值
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
void Intp6TapHalfPelV16x16_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelHV16x16_bf
功能         ： 16x16宏块的对角方向6Tap半象素插值
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
void Intp6TapHalfPelHV16x16_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_01_bf
功能         ： 4x4块的四分之一象素插值(a点)
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
void IntpQuarterPel4x4_01_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_03_bf
功能         ： 4x4块的四分之一象素插值(c点)
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
void IntpQuarterPel4x4_03_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPelV4x4_10_bf
功能         ： 4x4块的四分之一象素插值(d点)
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
void IntpQuarterPel4x4_10_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_11_bf
功能         ： 4x4块的四分之一象素插值(e点)
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
void IntpQuarterPel4x4_11_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_12_bf
功能         ： 4x4块的四分之一象素插值(f点)
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
void IntpQuarterPel4x4_12_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_13_bf
功能         ： 4x4块的四分之一象素插值(g点)
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
void IntpQuarterPel4x4_13_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_21_bf
功能         ： 4x4块的四分之一象素插值(i点)
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
void IntpQuarterPel4x4_21_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_23_bf
功能         ： 4x4块的四分之一象素插值(k点)
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
void IntpQuarterPel4x4_23_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_30_bf
功能         ： 4x4块的四分之一象素插值(n点)
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
void IntpQuarterPel4x4_30_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_31_bf
功能         ： 4x4块的四分之一象素插值(p点)
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
void IntpQuarterPel4x4_31_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_32_bf
功能         ： 4x4块的四分之一象素插值(q点)
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
void IntpQuarterPel4x4_32_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_33_bf
功能         ： 4x4块的四分之一象素插值(r点)
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
void IntpQuarterPel4x4_33_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_01_bf
功能         ： 8x8块的四分之一象素插值(a点)
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
void IntpQuarterPel8x8_01_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_03_bf
功能         ： 8x8块的四分之一象素插值(c点)
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
void IntpQuarterPel8x8_03_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPelV8x8_10_bf
功能         ： 8x8块的四分之一象素插值(d点)
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
void IntpQuarterPel8x8_10_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_11_bf
功能         ： 8x8块的四分之一象素插值(e点)
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
void IntpQuarterPel8x8_11_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_12_bf
功能         ： 8x8块的四分之一象素插值(f点)
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
void IntpQuarterPel8x8_12_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_13_bf
功能         ： 8x8块的四分之一象素插值(g点)
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
void IntpQuarterPel8x8_13_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_21_bf
功能         ： 8x8块的四分之一象素插值(i点)
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
void IntpQuarterPel8x8_21_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_23_bf
功能         ： 8x8块的四分之一象素插值(k点)
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
void IntpQuarterPel8x8_23_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);


/*====================================================================
函数名       :  IntpQuarterPel8x8_30_bf
功能         ： 8x8块的四分之一象素插值(n点)
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
void IntpQuarterPel8x8_30_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_31_bf
功能         ： 8x8块的四分之一象素插值(p点)
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
void IntpQuarterPel8x8_31_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_32_bf
功能         ： 8x8块的四分之一象素插值(q点)
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
void IntpQuarterPel8x8_32_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_33_bf
功能         ： 8x8块的四分之一象素插值(r点)
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
void IntpQuarterPel8x8_33_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_01_bf
功能         ： 16x16块的四分之一象素插值(a点)
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
void IntpQuarterPel16x16_01_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_03_bf
功能         ： 16x16块的四分之一象素插值(c点)
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
void IntpQuarterPel16x16_03_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPelV16x16_10_bf
功能         ： 16x16块的四分之一象素插值(d点)
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
void IntpQuarterPel16x16_10_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_11_bf
功能         ： 16x16块的四分之一象素插值(e点)
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
void IntpQuarterPel16x16_11_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_12_bf
功能         ： 16x16块的四分之一象素插值(f点)
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
void IntpQuarterPel16x16_12_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_13_bf
功能         ： 16x16块的四分之一象素插值(g点)
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
void IntpQuarterPel16x16_13_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_21_bf
功能         ： 16x16块的四分之一象素插值(i点)
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
void IntpQuarterPel16x16_21_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_23_bf
功能         ： 16x16块的四分之一象素插值(k点)
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
void IntpQuarterPel16x16_23_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_30_bf
功能         ： 16x16块的四分之一象素插值(n点)
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
void IntpQuarterPel16x16_30_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_31_bf
功能         ： 16x16块的四分之一象素插值(p点)
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
void IntpQuarterPel16x16_31_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_32_bf
功能         ： 16x16块的四分之一象素插值(q点)
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
void IntpQuarterPel16x16_32_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_33_bf
功能         ： 16x16块的四分之一象素插值(r点)
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
void IntpQuarterPel16x16_33_bf(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

//QUANT/DEQUANT
#define	  H264DCQuant2x2		H264DCQuant2x2_bf
#define	  H264Quant4x4			H264Quant4x4_bf
#define	  H264DCQuant4x4		H264DCQuant4x4_bf
#define	  H264ACQuant4x4		H264ACQuant4x4_bf
#define	  H264DCDequant2x2		H264DCDequant2x2_bf
#define	  H264Dequant4x4		H264Dequant4x4_bf
#define	  H264DCDequant4x4		H264DCDequant4x4_bf
#define	  H264ACDequant4x4		H264ACDequant4x4_bf

#define	  H263QuantIntra       H263QuantIntra_c    
#define   H263QuantInter       H263QuantInter_c    
#define   H263DequantIntra     H263DequantIntra_c  
#define   H263DequantInter     H263DequantInter_c  

//MPEG4QUANT/MPEG4DEQUANT
#define   Mpeg4QuantIntra   Mpeg4QuantIntra_c
#define   Mpeg4QuantInter   Mpeg4QuantInter_c  
#define   Mpeg4DequantIntra Mpeg4DequantIntra_c 
#define   Mpeg4DequantInter Mpeg4DequantInter_c


//MPEG2QUANT/MPEG2DEQUANT
#define  Mpeg2QuantIntra     Mpeg2QuantIntra_c
#define  Mpeg2QuantInter     Mpeg2QuantInter_c
#define  Mpeg2DequantIntra   Mpeg2DequantIntra_c
#define  Mpeg2DequantInter   Mpeg2DequantInter_c


//DCT/IDCT
#define   DCT4x4        DCT4x4_bf  
#define   DCT8x8        DCT8x8_bf  
#define   IDCT4x4       IDCT4x4_bf 
#define   IDCT8x8       IDCT8x8_bf 


//Hadamard/IHadmard
#define   Hadamard2x2	Hadamard2x2_bf
#define   Hadamard4x4	Hadamard4x4_bf

#define   IHadamard2x2	IHadamard2x2_bf
#define   IHadamard4x4	IHadamard4x4_bf

//SAD
#define   SAD16x16   SAD16x16_bf  
#define   SADNoStride16x16   SADNoStride16x16_bf
#define   SAD8x8     SAD8x8_bf
#define   SADNoStride8x8     SADNoStride8x8_bf

//DEVIATIONAMB
#define    DeviationMB     DeviationMB_bf

//INTERPOLATION
//4x4 1/4像素插值
#define IntpQuarterPel4x4_01		IntpQuarterPel4x4_01_bf
#define IntpQuarterPel4x4_03		IntpQuarterPel4x4_03_bf
#define IntpQuarterPel4x4_10		IntpQuarterPel4x4_10_bf
#define IntpQuarterPel4x4_11		IntpQuarterPel4x4_11_bf
#define IntpQuarterPel4x4_12		IntpQuarterPel4x4_12_bf
#define IntpQuarterPel4x4_13		IntpQuarterPel4x4_13_bf
#define IntpQuarterPel4x4_21		IntpQuarterPel4x4_21_bf
#define IntpQuarterPel4x4_23		IntpQuarterPel4x4_23_bf
#define IntpQuarterPel4x4_30		IntpQuarterPel4x4_30_bf
#define IntpQuarterPel4x4_31		IntpQuarterPel4x4_31_bf
#define IntpQuarterPel4x4_32		IntpQuarterPel4x4_32_bf
#define IntpQuarterPel4x4_33		IntpQuarterPel4x4_33_bf

//4x4 2Tap 半像素插值
#define Intp2TapHalfPelH4x4		Intp2TapHalfPelH4x4_bf
#define Intp2TapHalfPelV4x4		Intp2TapHalfPelV4x4_bf
#define Intp2TapHalfPelHV4x4	Intp2TapHalfPelHV4x4_bf

//4x4 6Tap 半像素插值
#define Intp6TapHalfPelH4x4		Intp6TapHalfPelH4x4_bf
#define Intp6TapHalfPelV4x4		Intp6TapHalfPelV4x4_bf
#define Intp6TapHalfPelHV4x4	Intp6TapHalfPelHV4x4_bf

//8x8 1/4像素插值
#define IntpQuarterPel8x8_01		IntpQuarterPel8x8_01_bf
#define IntpQuarterPel8x8_03		IntpQuarterPel8x8_03_bf
#define IntpQuarterPel8x8_10		IntpQuarterPel8x8_10_bf
#define IntpQuarterPel8x8_11		IntpQuarterPel8x8_11_bf
#define IntpQuarterPel8x8_12		IntpQuarterPel8x8_12_bf
#define IntpQuarterPel8x8_13		IntpQuarterPel8x8_13_bf
#define IntpQuarterPel8x8_21		IntpQuarterPel8x8_21_bf
#define IntpQuarterPel8x8_23		IntpQuarterPel8x8_23_bf
#define IntpQuarterPel8x8_30		IntpQuarterPel8x8_30_bf
#define IntpQuarterPel8x8_31		IntpQuarterPel8x8_31_bf
#define IntpQuarterPel8x8_32		IntpQuarterPel8x8_32_bf
#define IntpQuarterPel8x8_33		IntpQuarterPel8x8_33_bf

//8x8 2Tap 半像素插值
#define Intp2TapHalfPelHRounding8x8		Intp2TapHalfPelHRounding8x8_bf
#define Intp2TapHalfPelVRounding8x8		Intp2TapHalfPelVRounding8x8_bf
#define Intp2TapHalfPelHVRounding8x8		Intp2TapHalfPelHVRounding8x8_bf

#define Intp2TapHalfPelH8x8		Intp2TapHalfPelH8x8_bf
#define Intp2TapHalfPelV8x8		Intp2TapHalfPelV8x8_bf
#define Intp2TapHalfPelHV8x8		Intp2TapHalfPelHV8x8_bf

//8x8 6Tap 半像素插值
#define Intp6TapHalfPelH8x8		Intp6TapHalfPelH8x8_bf
#define Intp6TapHalfPelV8x8		Intp6TapHalfPelV8x8_bf
#define Intp6TapHalfPelHV8x8		Intp6TapHalfPelHV8x8_bf

//16x16 1/4像素插值
#define IntpQuarterPel16x16_01		IntpQuarterPel16x16_01_bf
#define IntpQuarterPel16x16_03		IntpQuarterPel16x16_03_bf
#define IntpQuarterPel16x16_10		IntpQuarterPel16x16_10_bf
#define IntpQuarterPel16x16_11		IntpQuarterPel16x16_11_bf
#define IntpQuarterPel16x16_12		IntpQuarterPel16x16_12_bf
#define IntpQuarterPel16x16_13		IntpQuarterPel16x16_13_bf
#define IntpQuarterPel16x16_21		IntpQuarterPel16x16_21_bf
#define IntpQuarterPel16x16_23		IntpQuarterPel16x16_23_bf
#define IntpQuarterPel16x16_30		IntpQuarterPel16x16_30_bf
#define IntpQuarterPel16x16_31		IntpQuarterPel16x16_31_bf
#define IntpQuarterPel16x16_32		IntpQuarterPel16x16_32_bf
#define IntpQuarterPel16x16_33		IntpQuarterPel16x16_33_bf

//16x16 2Tap 半像素插值
#define Intp2TapHalfPelHRounding16x16		Intp2TapHalfPelHRounding16x16_bf
#define Intp2TapHalfPelVRounding16x16		Intp2TapHalfPelVRounding16x16_bf
#define Intp2TapHalfPelHVRounding16x16		Intp2TapHalfPelHVRounding16x16_bf

#define Intp2TapHalfPelH16x16		Intp2TapHalfPelH16x16_bf
#define Intp2TapHalfPelV16x16		Intp2TapHalfPelV16x16_bf
#define Intp2TapHalfPelHV16x16		Intp2TapHalfPelHV16x16_bf

//16x16 6Tap 半像素插值
#define Intp6TapHalfPelH16x16		Intp6TapHalfPelH16x16_bf
#define Intp6TapHalfPelV16x16		Intp6TapHalfPelV16x16_bf
#define Intp6TapHalfPelHV16x16		Intp6TapHalfPelHV16x16_bf

#elif defined( C_CODE )
//QUANAT/DEQUANT
/*====================================================================
	函数名	    :	H264DCQuant2x2_c
	功能		:	实现 2x2 DC块的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264DCQuant2x2_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264Quant4x4_c
	功能		:	实现 4x4 块的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264Quant4x4_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264DCQuant4x4_c
	功能		:	实现 4x4 块DC的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264DCQuant4x4_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264ACQuant4x4_c
	功能		:	实现 4x4 块AC的量化
	算法实现	:	（可选项）
	引用全局变量:	无
    输入参数说明:	s16 * ps16InCoeff	:反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
					l32 l32SliceType	:slice类型
	返回值说明  :	无
====================================================================*/
void H264ACQuant4x4_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, l32 l32SliceType);

/*====================================================================
	函数名	    :	H264DCDequant2x2_c
	功能		:	实现 2x2 DC块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵，
					s16 *ps16OutCoeff	:输出结果
					u8 u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264DCDequant2x2_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*====================================================================
	函数名	    :	H264Dequant4x4_c
	功能		:	实现 4x4 块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵，
					s16 *ps16OutCoeff	:输出结果
					u8  u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264Dequant4x4_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*====================================================================
	函数名	    :	H264DCDequant4x4_c
	功能		:	实现 4x4 DC块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8 u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264DCDequant4x4_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*====================================================================
	函数名	    :	H264ACDequant4x4_c
	功能		:	实现 4x4 AC块的反量化
	算法实现	:	(可选项)
	引用全局变量:	无
    输入参数说明:	s16 *ps16InCoeff	:要反量化的系数矩阵
					s16 *ps16OutCoeff	:输出结果
					u8 u8Quant			:量化系数，
	返回值说明  :	无
====================================================================*/
void H264ACDequant4x4_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263QuantIntra_c
功能		：对8x8的Intra块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff		: 存放量化后结果
              u8  u8Quant			: 它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：无。
=============================================================================*/
u32 H263QuantIntra_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263QuantInter_c
功能		：对8x8的Inter块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:待量化的8x8块
              s16  *ps16OutCoeff		:存放量化后结果
              u8  u8Quant			:它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：量化系数的和
=============================================================================*/
u32 H263QuantInter_c(const s16 *ps16InCoeff,s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263DequantIntra_c
功能		：对8x8的Intra块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		 ：待量化的8x8块
              s16  *ps16OutCoeff		 ：存放量化后结果
              u8  u8Quant			 ：它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：void
=============================================================================*/
void H263DequantIntra_c(s16 *ps16InCoeff,s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    ：H263DequantInter_c
功能		：对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff		: 存放量化后结果
              u8  u8Quant			:它不是量化步长，而是量化步长的一半
注意：
返回值说明  ：
=============================================================================*/
void H263DequantInter_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg4QuantIntra_c
功能		：对8x8的Intra块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8Quant			: 量化参数
              u8  u8DcScaler		: DC系数的量化参数   
注意：
返回值说明  ：
=============================================================================*/
u32 Mpeg4QuantIntra_c(const s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg4QuantInter_c
功能		：对8x8的Inter块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:待量化的8x8块
              s16  *ps16OutCoeff		:存放量化后结果
              u8  u8Quant			:量化参数 
注意：
返回值说明  ：
=============================================================================*/
u32 Mpeg4QuantInter_c(const s16 *ps16InCoeff, s16 * ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg4DequantIntra_c
功能		：对8x8的Intra块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8Quant			: 量化参数
              u8  u8DcScaler		: DC系数的量化参数   
注意：
返回值说明  ：
=============================================================================*/
void Mpeg4DequantIntra_c(const s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg4DequantInter_c
功能		：对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:待量化的8x8块
              s16  *ps16OutCoeff		:存放量化后结果
              u8  u8Quant			:量化参数 
注意：
返回值说明  ：
=============================================================================*/
void Mpeg4DequantInter_c(const s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg2QuantIntra_c
功能		：Mpeg2 对8x8的Intra块做量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8DcScaler		: DC系数的量化参数
              u8  u8Quant			:量化步长   
注意：
返回值说明  ：
=============================================================================*/
void Mpeg2QuantIntra_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg2QuantInter_c
功能		：Mpeg2 对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:  待量化的8x8块
              s16  *ps16OutCoeff		: 存放量化后结果
              u8  u8Quant			:  量化步长
注意：
返回值说明  ：返回块中是否都为0
=============================================================================*/
u16 Mpeg2QuantInter_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant);

/*===========================================================================
函数名	    Mpeg2DequantIntra_c
功能		：Mpeg2 对8x8的Intra块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		: 待量化的8x8块
              s16  *ps16OutCoeff	: 存放量化后结果
              u8  u8Quant			: 量化参数
              u8  u8DcScaler		: DC系数的量化参数 
注意：
返回值说明  ：
=============================================================================*/
void Mpeg2DequantIntra_c(s16 *ps16InCoeff, s16 *ps16OutCoeff, u8 u8Quant, u8 u8DcScaler);

/*===========================================================================
函数名	    Mpeg2DequantInter_c
功能		：Mpeg2 对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *ps16InCoeff		:  待量化的8x8块
              s16  *ps16OutCoeff	:  存放量化后结果
              u8  u8Quant			:  量化步长
注意：
返回值说明  ：
=============================================================================*/
void Mpeg2DequantInter_c(s16 *ps16InCoeff, s16  *ps16OutCoeff,  u8 u8Quant);



//DCT/IDCT
/*====================================================================
函数名       :  DCT4x4_c
功能         ： 4×4模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock  ： 指向4×4模块
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void  DCT4x4_c(s16* ps16Block);

/*====================================================================
函数名       :  DCT8x8_c
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock   ： 指向8×8模块
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void  DCT8x8_c(s16* ps16Block);

//8×8和4x4块DCT变换
/*====================================================================
函数名       :  IDCT4x4_c
功能         ： 4×4模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock  ：  指向4×4模块
返回值说明   ： 无
特殊说明     ： pBlock地址需要32位对齐
------------------------------------------------------------------------
修改记录    :
日期：      版本        修改人      修改内容

======================================================================*/
void  IDCT4x4_c(s16* ps16Block);

/*====================================================================
函数名       :  IDCT8x8_c
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16  *pBlock  ：  指向8×8模块
返回值说明   ： 无
特殊说明     ： pBlock地址需要32位对齐
------------------------------------------------------------------------
修改记录    :
日期：      版本        修改人      修改内容

======================================================================*/
void  IDCT8x8_c(s16* ps16Block);

//Hadamard/IHadmard
/*====================================================================
	函数名	    ：Hadamard2x2_c
	功能		：实现2x2 Hadamard变换
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16 * ps16InBlock		: 输入的2x2块，
                  s16 * ps16OutBlock	: 输出结果2x2块
	返回值说明  ：无
====================================================================*/
void Hadamard2x2_c(s16 *ps16InBlock, s16 *ps16OutBlock);

/*====================================================================
	函数名	    ：IHadamard2x2_c
	功能		：实现2x2 IHadamard变换
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16 *ps16InBlock  : 输入的2x2块，
                  s16 *ps16OutBlock : 输出结果2x2块
	返回值说明  ：无
====================================================================*/
void IHadamard2x2_c(s16 *ps16Block, s16 *ps16OutBlock);

/*====================================================================
	函数名	    ：Hadamard4x4_c
	功能		：实现4x4 Hadamard变换
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16 as16Block[16]：输入的4x4块，也作为输出结果
	返回值说明  ：无
====================================================================*/
void Hadamard4x4_c (s16 as16Block[16]);

/*====================================================================
	函数名	    ：IHadamard4x4_c
	功能		：实现4x4块的哈达码反变换（IHadamard）
	算法实现	：（可选项）
	引用全局变量：无
    输入参数说明：s16  as16Block[16] ：输入的4x4块，也作为输出结果;
	返回值说明  ：无
====================================================================*/
void IHadamard4x4_c (s16 as16Block[16]);

//SAD
/*====================================================================
函数名       :  SAD16x16_c
功能         ： 16x16模块SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u16  u16CurrStride  ：当前宏块步长
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride   ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SAD16x16_c(u8 *pu8CurrMB, u16 u16CurrStride, u8 *pu8RefMB, u16 u16RefStride);

/*====================================================================
函数名       :  SADNoStride16x16_c
功能         ： 16x16模块无步长SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride   ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SADNoStride16x16_c(u8 *pu8CurrMB, u8 *pu8RefMB, u16 u16RefStride);

/*====================================================================
函数名       :  SAD8x8_c
功能         ： 8x8模块SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u16  u16CurrStride  ：当前宏块步长
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride   ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SAD8x8_c(u8 *pu8CurrMB, u16 u16CurrStride, u8 *pu8RefMB, u16 u16RefStride);

/*====================================================================
函数名       :  SADNoStride8x8_c
功能         ： 8x8模块无步长SAD 
引用全局变量 ： 无
输入参数说明 ：  u8  *pu8CurrMB     ：当前宏块指针
                 u8  *pu8RefMB      ：参考宏块指针
                 u16  u16RefStride  ：参考宏块步长
返回值说明   ： 当前宏块与参考宏块之差绝对值的和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容
2005-3-28   1.0          张葵阳      优化
======================================================================*/
u32 SADNoStride8x8_c(u8 *pu8CurrMB, u8 *pu8RefMB, u16 u16RefStride);


//DEVIATION
/*====================================================================
函数名       :  DeviationMB_c
功能         ： 求宏块内各象素值与平均值差的绝对值之和
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8MB ：指向宏块指针
                 u16 u16Stride	  ：宏块步长                 
返回值说明   ： 宏块内各象素值与宏块内象素平均值差的绝对值之和
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
u32 DeviationMB_c(const u8 *pu8MB, u16 u16Stride);

//Interpolation
/*====================================================================
函数名       :  Intp2TapHalfPelH4x4_c
功能         ： 4x4块的水平方向2Tap半象素插值
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
void Intp2TapHalfPelH4x4_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelV4x4_c
功能         ： 4x4块的垂直方向2Tap半象素插值
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
void Intp2TapHalfPelV4x4_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHV4x4_c
功能         ： 4x4块的对角方向2Tap半象素插值
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
void Intp2TapHalfPelHV4x4_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

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
函数名       :  Intp2TapHalfPelHRounding8x8_c
功能         ： 8x8块的水平方向2Tap半象素插值
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
void Intp2TapHalfPelHRounding8x8_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelH8x8_c
功能         ： 8x8块的水平方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelH8x8_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelVRounding8x8_c
功能         ： 8x8块的垂直方向2Tap半象素插值
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
void Intp2TapHalfPelVRounding8x8_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelV8x8_c
功能         ： 8x8块的垂直方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelV8x8_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelHVRounding8x8_c
功能         ： 8x8块的对角方向2Tap半象素插值
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
void Intp2TapHalfPelHVRounding8x8_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHV8x8_c
功能         ： 8x8块的对角方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelHV8x8_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

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
函数名       :  Intp6TapHalfPelIntH8x8_c
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
void Intp6TapHalfPelIntH8x8_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

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
函数名       :  Intp6TapHalfPelIntV8x8_c
功能         ： 8x8块的垂直方向6Tap半象素插值
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
void Intp6TapHalfPelIntV8x8_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, l32 *pl32IntDst);

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
函数名       :  Intp2TapHalfPelHRounding16x16_c
功能         ： 16x16源块的水平方向2Tap半象素插值
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
void Intp2TapHalfPelHRounding16x16_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelH16x16_c
功能         ： 16x16源块的水平方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelH16x16_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelVRounding16x16_c
功能         ： 16x16源块的垂直方向2Tap半象素插值
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
void Intp2TapHalfPelVRounding16x16_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelV16x16_c
功能         ： 16x16源块的垂直方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelV16x16_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp2TapHalfPelHVRounding16x16_c
功能         ： 源块的对角方向2Tap半象素插值
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
void Intp2TapHalfPelHVRounding16x16_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp2TapHalfPelHV16x16_c
功能         ： 源块的对角方向2Tap半象素插值
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8Src        ：源块指针
                 u16        u16SrcStride  ：源块步长
                 u8        *pu8Dst        ：目标块指针
                 u16        u16DstStride  ：目标块步长
				 BOOL		 bRounding		:	是否进行四舍五入
返回值说明   ： 无
特殊说明     ： 无
------------------------------------------------------------------------
修改记录     :
日期：      版本        修改人      修改内容

======================================================================*/
void Intp2TapHalfPelHV16x16_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride, BOOL bRounding);

/*====================================================================
函数名       :  Intp6TapHalfPelH16x16_c
功能         ： 源块的水平方向6Tap半象素插值
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
void Intp6TapHalfPelH16x16_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelV16x16_c
功能         ： 16x16宏块的垂直方向6Tap半象素插值
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
void Intp6TapHalfPelV16x16_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  Intp6TapHalfPelHV16x16_c
功能         ： 16x16宏块的对角方向6Tap半象素插值
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
void Intp6TapHalfPelHV16x16_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_01_c
功能         ： 4x4块的四分之一象素插值(a点)
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
void IntpQuarterPel4x4_01_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_03_c
功能         ： 4x4块的四分之一象素插值(c点)
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
void IntpQuarterPel4x4_03_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPelV4x4_10_c
功能         ： 4x4块的四分之一象素插值(d点)
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
void IntpQuarterPel4x4_10_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_11_c
功能         ： 4x4块的四分之一象素插值(e点)
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
void IntpQuarterPel4x4_11_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_12_c
功能         ： 4x4块的四分之一象素插值(f点)
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
void IntpQuarterPel4x4_12_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_13_c
功能         ： 4x4块的四分之一象素插值(g点)
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
void IntpQuarterPel4x4_13_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_21_c
功能         ： 4x4块的四分之一象素插值(i点)
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
void IntpQuarterPel4x4_21_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_23_c
功能         ： 4x4块的四分之一象素插值(k点)
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
void IntpQuarterPel4x4_23_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_30_c
功能         ： 4x4块的四分之一象素插值(n点)
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
void IntpQuarterPel4x4_30_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_31_c
功能         ： 4x4块的四分之一象素插值(p点)
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
void IntpQuarterPel4x4_31_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_32_c
功能         ： 4x4块的四分之一象素插值(q点)
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
void IntpQuarterPel4x4_32_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel4x4_33_c
功能         ： 4x4块的四分之一象素插值(r点)
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
void IntpQuarterPel4x4_33_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_01_c
功能         ： 8x8块的四分之一象素插值(a点)
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
void IntpQuarterPel8x8_01_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_03_c
功能         ： 8x8块的四分之一象素插值(c点)
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
void IntpQuarterPel8x8_03_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPelV8x8_10_c
功能         ： 8x8块的四分之一象素插值(d点)
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
void IntpQuarterPel8x8_10_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_11_c
功能         ： 8x8块的四分之一象素插值(e点)
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
void IntpQuarterPel8x8_11_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_12_c
功能         ： 8x8块的四分之一象素插值(f点)
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
void IntpQuarterPel8x8_12_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_13_c
功能         ： 8x8块的四分之一象素插值(g点)
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
void IntpQuarterPel8x8_13_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_21_c
功能         ： 8x8块的四分之一象素插值(i点)
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
void IntpQuarterPel8x8_21_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_23_c
功能         ： 8x8块的四分之一象素插值(k点)
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
void IntpQuarterPel8x8_23_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);


/*====================================================================
函数名       :  IntpQuarterPel8x8_30_c
功能         ： 8x8块的四分之一象素插值(n点)
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
void IntpQuarterPel8x8_30_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_31_c
功能         ： 8x8块的四分之一象素插值(p点)
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
void IntpQuarterPel8x8_31_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_32_c
功能         ： 8x8块的四分之一象素插值(q点)
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
void IntpQuarterPel8x8_32_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel8x8_33_c
功能         ： 8x8块的四分之一象素插值(r点)
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
void IntpQuarterPel8x8_33_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_01_c
功能         ： 16x16块的四分之一象素插值(a点)
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
void IntpQuarterPel16x16_01_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_03_c
功能         ： 16x16块的四分之一象素插值(c点)
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
void IntpQuarterPel16x16_03_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPelV16x16_10_c
功能         ： 16x16块的四分之一象素插值(d点)
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
void IntpQuarterPel16x16_10_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_11_c
功能         ： 16x16块的四分之一象素插值(e点)
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
void IntpQuarterPel16x16_11_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_12_c
功能         ： 16x16块的四分之一象素插值(f点)
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
void IntpQuarterPel16x16_12_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_13_c
功能         ： 16x16块的四分之一象素插值(g点)
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
void IntpQuarterPel16x16_13_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_21_c
功能         ： 16x16块的四分之一象素插值(i点)
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
void IntpQuarterPel16x16_21_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_23_c
功能         ： 16x16块的四分之一象素插值(k点)
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
void IntpQuarterPel16x16_23_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_30_c
功能         ： 16x16块的四分之一象素插值(n点)
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
void IntpQuarterPel16x16_30_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_31_c
功能         ： 16x16块的四分之一象素插值(p点)
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
void IntpQuarterPel16x16_31_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_32_c
功能         ： 16x16块的四分之一象素插值(q点)
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
void IntpQuarterPel16x16_32_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

/*====================================================================
函数名       :  IntpQuarterPel16x16_33_c
功能         ： 16x16块的四分之一象素插值(r点)
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
void IntpQuarterPel16x16_33_c(const u8 *pu8Src, u16 u16SrcStride, u8 *pu8Dst, u16 u16DstStride);

//QUANT/DEQUANT
#define	  H264DCQuant2x2		H264DCQuant2x2_c
#define	  H264Quant4x4			H264Quant4x4_c
#define	  H264DCQuant4x4		H264DCQuant4x4_c
#define	  H264ACQuant4x4		H264ACQuant4x4_c
#define	  H264DCDequant2x2		H264DCDequant2x2_c
#define	  H264Dequant4x4		H264Dequant4x4_c
#define	  H264DCDequant4x4		H264DCDequant4x4_c
#define	  H264ACDequant4x4		H264ACDequant4x4_c

#define	  H263QuantIntra       H263QuantIntra_c    
#define   H263QuantInter       H263QuantInter_c    
#define   H263DequantIntra     H263DequantIntra_c  
#define   H263DequantInter     H263DequantInter_c  

//MPEG4QUANT/MPEG4DEQUANT
#define   Mpeg4QuantIntra   Mpeg4QuantIntra_c
#define   Mpeg4QuantInter   Mpeg4QuantInter_c  
#define   Mpeg4DequantIntra Mpeg4DequantIntra_c 
#define   Mpeg4DequantInter Mpeg4DequantInter_c


//MPEG2QUANT/MPEG2DEQUANT
#define  Mpeg2QuantIntra     Mpeg2QuantIntra_c
#define  Mpeg2QuantInter     Mpeg2QuantInter_c
#define  Mpeg2DequantIntra   Mpeg2DequantIntra_c
#define  Mpeg2DequantInter   Mpeg2DequantInter_c


//DCT/IDCT
#define   DCT4x4        DCT4x4_c  
#define   DCT8x8        DCT8x8_c  
#define   IDCT4x4       IDCT4x4_c 
#define   IDCT8x8       IDCT8x8_c 

//Hadamard/IHadmard
#define   Hadamard2x2	Hadamard2x2_c
#define   Hadamard4x4	Hadamard4x4_c

#define   IHadamard2x2	IHadamard2x2_c
#define   IHadamard4x4	IHadamard4x4_c

//SAD
#define   SAD16x16   SAD16x16_c  
#define   SADNoStride16x16   SADNoStride16x16_c  
#define   SAD8x8     SAD8x8_c
#define   SADNoStride8x8     SADNoStride8x8_c

//DEVIATIONAMB
#define    DeviationMB     DeviationMB_c

//INTERPOLATION
//4x4 1/4像素插值
#define IntpQuarterPel4x4_01		IntpQuarterPel4x4_01_c
#define IntpQuarterPel4x4_03		IntpQuarterPel4x4_03_c
#define IntpQuarterPel4x4_10		IntpQuarterPel4x4_10_c
#define IntpQuarterPel4x4_11		IntpQuarterPel4x4_11_c
#define IntpQuarterPel4x4_12		IntpQuarterPel4x4_12_c
#define IntpQuarterPel4x4_13		IntpQuarterPel4x4_13_c
#define IntpQuarterPel4x4_21		IntpQuarterPel4x4_21_c
#define IntpQuarterPel4x4_23		IntpQuarterPel4x4_23_c
#define IntpQuarterPel4x4_30		IntpQuarterPel4x4_30_c
#define IntpQuarterPel4x4_31		IntpQuarterPel4x4_31_c
#define IntpQuarterPel4x4_32		IntpQuarterPel4x4_32_c
#define IntpQuarterPel4x4_33		IntpQuarterPel4x4_33_c

//4x4 2Tap 半像素插值
#define Intp2TapHalfPelH4x4		Intp2TapHalfPelH4x4_c
#define Intp2TapHalfPelV4x4		Intp2TapHalfPelV4x4_c
#define Intp2TapHalfPelHV4x4	Intp2TapHalfPelHV4x4_c

//4x4 6Tap 半像素插值
#define Intp6TapHalfPelH4x4		Intp6TapHalfPelH4x4_c
#define Intp6TapHalfPelV4x4		Intp6TapHalfPelV4x4_c
#define Intp6TapHalfPelHV4x4	Intp6TapHalfPelHV4x4_c

//8x8 1/4像素插值
#define IntpQuarterPel8x8_01		IntpQuarterPel8x8_01_c
#define IntpQuarterPel8x8_03		IntpQuarterPel8x8_03_c
#define IntpQuarterPel8x8_10		IntpQuarterPel8x8_10_c
#define IntpQuarterPel8x8_11		IntpQuarterPel8x8_11_c
#define IntpQuarterPel8x8_12		IntpQuarterPel8x8_12_c
#define IntpQuarterPel8x8_13		IntpQuarterPel8x8_13_c
#define IntpQuarterPel8x8_21		IntpQuarterPel8x8_21_c
#define IntpQuarterPel8x8_23		IntpQuarterPel8x8_23_c
#define IntpQuarterPel8x8_30		IntpQuarterPel8x8_30_c
#define IntpQuarterPel8x8_31		IntpQuarterPel8x8_31_c
#define IntpQuarterPel8x8_32		IntpQuarterPel8x8_32_c
#define IntpQuarterPel8x8_33		IntpQuarterPel8x8_33_c

//8x8 2Tap 半像素插值
#define Intp2TapHalfPelHRounding8x8		Intp2TapHalfPelHRounding8x8_c
#define Intp2TapHalfPelVRounding8x8		Intp2TapHalfPelVRounding8x8_c
#define Intp2TapHalfPelHVRounding8x8		Intp2TapHalfPelHVRounding8x8_c

#define Intp2TapHalfPelH8x8		Intp2TapHalfPelH8x8_c
#define Intp2TapHalfPelV8x8		Intp2TapHalfPelV8x8_c
#define Intp2TapHalfPelHV8x8		Intp2TapHalfPelHV8x8_c

//8x8 6Tap 半像素插值
#define Intp6TapHalfPelH8x8		Intp6TapHalfPelH8x8_c
#define Intp6TapHalfPelV8x8		Intp6TapHalfPelV8x8_c
#define Intp6TapHalfPelHV8x8		Intp6TapHalfPelHV8x8_c

//16x16 1/4像素插值
#define IntpQuarterPel16x16_01		IntpQuarterPel16x16_01_c
#define IntpQuarterPel16x16_03		IntpQuarterPel16x16_03_c
#define IntpQuarterPel16x16_10		IntpQuarterPel16x16_10_c
#define IntpQuarterPel16x16_11		IntpQuarterPel16x16_11_c
#define IntpQuarterPel16x16_12		IntpQuarterPel16x16_12_c
#define IntpQuarterPel16x16_13		IntpQuarterPel16x16_13_c
#define IntpQuarterPel16x16_21		IntpQuarterPel16x16_21_c
#define IntpQuarterPel16x16_23		IntpQuarterPel16x16_23_c
#define IntpQuarterPel16x16_30		IntpQuarterPel16x16_30_c
#define IntpQuarterPel16x16_31		IntpQuarterPel16x16_31_c
#define IntpQuarterPel16x16_32		IntpQuarterPel16x16_32_c
#define IntpQuarterPel16x16_33		IntpQuarterPel16x16_33_c

//16x16 2Tap 半像素插值
#define Intp2TapHalfPelHRounding16x16		Intp2TapHalfPelHRounding16x16_c
#define Intp2TapHalfPelVRounding16x16		Intp2TapHalfPelVRounding16x16_c
#define Intp2TapHalfPelHVRounding16x16		Intp2TapHalfPelHVRounding16x16_c

#define Intp2TapHalfPelH16x16		Intp2TapHalfPelH16x16_c
#define Intp2TapHalfPelV16x16		Intp2TapHalfPelV16x16_c
#define Intp2TapHalfPelHV16x16		Intp2TapHalfPelHV16x16_c

//16x16 6Tap 半像素插值
#define Intp6TapHalfPelH16x16		Intp6TapHalfPelH16x16_c
#define Intp6TapHalfPelV16x16		Intp6TapHalfPelV16x16_c
#define Intp6TapHalfPelHV16x16		Intp6TapHalfPelHV16x16_c

#endif

#if defined __cplusplus
}; /* End "C" */
#endif /* __cplusplus */

#endif // _VCODEC_COMMON_LIB_H_
