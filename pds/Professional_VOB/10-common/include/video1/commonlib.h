/******************************************************************************
模块名	    ：H261、H263和Mpeg4公共函数库
相关文件	：
文件实现功能：声明一些经过优化了的函数的原型。
              为了在PC和EQUATOR平台上最大限度的共享代码，避免维护Win32和Equator
              两个程序版本，这里采用预处理区别不同平台上的函数。在调用程序中只需
              写函数名的主体，具体调用什么平台上的函数由编译器自动选择。预处理是
              这样做的：
              定义一个OPTIMIZE_FOR，根据它的取值区分不同平台：
              OPTIMIZE_FOR_EQUATOR:在EQUATOR平台上优化
              OPTIMIZE_FOR_EQUATOR_INLINE:在EQUATOR平台上优化(采用inline方式)
              否则就没有优化(C语言版本)
              具体实现时，在EQUATOR平台上，在makefile中加入：OPTIMIZE_FOR = 100
              或者：OPTIMIZE_FOR_INLINE = 200，实现inline方式的调用
---------------------------------------------------------------------------------------------------------------------
修改记录:
  日  期	      版本	    	修改人		修改内容
2004.09.07        1.0          ZouWenyi       创建
******************************************************************************/

#ifndef _COMMONLIB_H_
#define _COMMONLIB_H_

#include "algorithmtype.h"

#ifdef _MSC_VER
	//warning C4206: nonstandard extension used : translation unit is empty
	#pragma warning(disable : 4206)
#endif//_MSC_VER

#ifndef ARCH_BLACKFIN_STANDALONE 	//sq 05.6.6
#define abs(x)  ((x) > 0)?(x):(-(x))
#endif
		
//运动矢量
typedef struct MotionVector
{
    int16_t x;
    int16_t y;    
} TMotionVector;

typedef struct
{
	int32_t		iActQuant;      //act_quant;	
	int32_t		iFrameRate;     //帧率
	int32_t		iTargetRate;    //目标码率
	int32_t		iMaxQuant;      //最大量化系数
	int32_t		iMinQuant;      //最小量化系数 

	int32_t		iTargetFrameSize;

	int32_t		iVbvDelay;
	int32_t		iVopDelay;
	int32_t		iClickPerBit;
	int32_t		iMaxVbvDelay;
	int32_t		iVbvDelayUpperTh;
	int32_t		iVbvDelayLowerTh;
	int32_t		iMidVbvDelay;
	int32_t		iLowVbvDelay;


	int32_t		iRCConstant;
	int32_t		iKp;
	int32_t         iSceneChange;

	int32_t		iFrameLength[25];  //每帧码流的长度

	int64_t  	iTotalSize;
	int64_t  	iDecodingTime;
	int64_t 	iTargetBits;
	
	//H261 Liwei Song  7/17/2004
	int32_t         iPreLose;       //之前丢帧数
	int32_t         iLoseFrameNum;  //根据码率设置不同的丢帧数目768丢一帧，384丢两帧
	
}TRateControl;  //码率控制


//码率控制初始化
int LibRateControlInit(TRateControl *pRateControl, int32_t iTargetRate, int32_t iFrameRate,
					int32_t iMaxQuant, int32_t iMinQuant, BOOL IsLoseFrame);
					
//获取估计的量化系数
int LibGetEstimatedQ(TRateControl *pRateControl);

//计算量化系数
int LibRateControlGetQ(TRateControl *pRateControl, int32_t iVopType, BOOL IsLoseFrame);
					
//更新码率控制参数
int LibRateControlUpdate(TRateControl * pRateControl, int32_t iFrameSize, int32_t iVopType);					
					

//计算两帧图像的信噪比
float  PSNR(uint8_t *pSrcFrame, uint8_t *pRecFrame, int32_t iHeight, int32_t iWidth);
					

//求16×16宏块的运动矢量，全搜索方式(精度位为整象素)
int32_t MotionSearch16x16FullPelFullSearch(uint8_t *pRefMB, int32_t iRefStride, uint8_t  *pCurrMB, 
				  int32_t  iCurrStride, int32_t  iSearchMinX, int32_t iSearchMaxX, 
				  int32_t   iSearchMinY, int32_t iSearchMaxY, TMotionVector  *pMV);
				  

//求16×16宏块的运动矢量，快速搜索方式(精度位为整象素)
int32_t MotionSearch16x16FullPelMVFAST(uint8_t *pRefMB, int32_t iRefStride, uint8_t  *pCurrMB, 
					   int32_t  iCurrStride, int32_t  iSearchMinX, int32_t iSearchMaxX, 
					   int32_t   iSearchMinY, int32_t iSearchMaxY,  TMotionVector  predMV[3],
					   TMotionVector  *pMV);

//求16×16宏块的运动矢量(精度位为半象素)					   
int32_t MotionSearch16x16HalfPel8Points(uint8_t *pRefMB, int32_t iRefStride, uint8_t *pCurrMB, 
                                           int32_t iCurrStride, int32_t iSearchMinX, int32_t iSearchMaxX,
                                           int32_t iSearchMinY, int32_t iSearchMaxY, int32_t iSad, 
                                           int32_t iRounding, TMotionVector *pMV);
                                           

//求8×8块的运动矢量，全搜索方式(精度位为整象素)                                          
int32_t MotionSearch8x8FullPelFullSearch(uint8_t *pRefMB, int32_t iRefStride, uint8_t *pCurrMB, 
                                           int32_t iCurrStride, int32_t iSearchMinX, int32_t iSearchMaxX,
                                           int32_t iSearchMinY, int32_t iSearchMaxY, TMotionVector *pMV);
                                     
      
//求8×8宏块的运动矢量(精度为半象素)                                            
int32_t MotionSearch8x8HalfPel8Points(uint8_t *pRefMB, int32_t iRefStride, uint8_t *pCurrMB, 
                                           int32_t iCurrStride, int32_t iSearchMinX, int32_t iSearchMaxX,
                                           int32_t iSearchMinY, int32_t iSearchMaxY, int32_t iSad, 
                                           int32_t iRounding, TMotionVector *pMV);                                          

void MotionSearch16x16FullPelEtiH263( uint8_t *pRefMB, int32_t iRefStride,uint8_t *pCurrMB, 
                                     int32_t iCurrStride,int32_t iSearchMinX, int32_t iSearchMaxX,
                                     int32_t iSearchMinY, int32_t iSearchMaxY, int32_t *pSadOpt,
                                     TMotionVector *pMV, int32_t iQuant);                                           					  

//获取版本信息
void  GetCommonLibVersion(void *pVer, int iBufLen, int *pVerLen);

#if  defined(OPTIMIZE_FOR) && (OPTIMIZE_FOR == 100)   //在EQUATOR上优化

//QUANAT/DEQUANT
//对8x8的Intra块做量化
uint32_t QuantIntra_map(int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t iQuant);

//对8x8的Inter块做量化
uint32_t QuantInter_map(const int16_t *pInCoeff,int16_t *pOutCoeff, uint8_t iQuant);

//对8x8的Intra块做反量化
void DeQuantIntra_map(int16_t *pInCoeff,int16_t *pOutCoeff, uint8_t iQuant);

//对8x8的Inter块做反量化
void DeQuantInter_map(int16_t *pInCoeff, int16_t *pOutCoeff, int32_t iQuant);

//对8x8的Intra块做反量化(处理直接从Vlx输出的数据)
void DeQuantIntraVlx_map(const int16_t *pInCoeff, int16_t *pOutCoeff,const int32_t iQuant);

//对8x8的Inter块做反量化(处理直接从Vlx输出的数据)
void DeQuantInterVlx_map(const int16_t *pInCoeff, int16_t *pOutCoeff, const int32_t iQuant);

//Mpeg2对8x8的Intra块做量化 
int ComlibMpeg2QuantIntra_map(int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t iDcScaler, uint8_t iQuant);
 
//Mpeg2对8x8的Inter块做量化
int ComlibMpeg2QuantInter_map(int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t iQuant);

//Mpeg2对8x8的Intra块做反量化
void Mpeg2DeQuantIntra_map(int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t  iDcScaler, uint8_t  iQuant);

//Mpeg2对8x8的Inter块做反量化
void Mpeg2DeQuantInter_map(int16_t  *pInCoeff, int16_t  *pOutCoeff,  uint8_t  iQuant);

//对8x8的Intra块做反量化(处理直接从Vlx输出的数据)
void Mpeg2DeQuantIntraVlx_map(int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t iDcScaler, uint8_t iQuant);

//对8x8的Inter块做反量化(处理直接从Vlx输出的数据)
void Mpeg2DeQuantInterVlx_map(int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t iQuant);




//Mpeg4对8x8的Intra块做量化
uint32_t ComlibMpeg4QuantIntra_map(int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t iQuant, int32_t iDcScaler);

//Mpeg4对8x8的Inter块做量化
uint32_t ComlibMpeg4QuantInter_map(int16_t *pInCoeff, int16_t * pOutCoeff, int32_t iQuant);

//Mpeg4对8x8的Intra块做反量化
uint32_t Mpeg4DeQuantIntra_map(int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t iQuant, int32_t iDcScaler);

//Mpeg4对8x8的Inter块做反量化
void Mpeg4DeQuantInter_map(int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t iQuant);



//DCT/IDCT
//8×8块DCT变换
void  DCT_map(int16_t* pBlock);

//8×8块DCT变换
void  IDCT_map(int16_t* pBlock);

//SAD
//计算16x16模块SAD
uint32_t SAD16x16_map(uint8_t *pCurrMB, int32_t iCurrStride,uint8_t *pRefMB, int32_t iRefStride);

//计算16x16模块SAD
uint32_t SAD8x8_map(const uint8_t *pCurrMB, int32_t iCurrStride, const uint8_t *pRefMB, int iRefStride);


//求运动矢量
void  RowSAD16_map(uint8_t *pCurrMB, int32_t iCurrStride, uint8_t *pRefMB, int32_t iRefStride,
		int32_t *psad_opt, TMotionVector *pmv, int16_t min_x, int16_t min_y);

//DEVIATION
//求16×16宏块内各象素值与平均值差的绝对值之和
int32_t DeviationMB_map(const uint8_t *pMB, int32_t iStride);

//Interpolation
//16×16宏块的水平方向插值
void InterpolateHalfPelH16x16_map(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst, int32_t iDstStride, int32_t iRounding);

//16×16宏块的垂直方向插值
void InterpolateHalfPelV16x16_map(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,  int32_t iDstStride, int32_t iRounding);

//16×16宏块的对角方向插值
void InterpolateHalfPelHV16x16_map(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst, int32_t iDstStride, int32_t iRounding);

//8×8块的水平方向插值
void InterpolateHalfPelH8x8_map(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst, int32_t iDstStride, int32_t iRounding);

//8×8块的垂直方向插值
void InterpolateHalfPelV8x8_map(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst, int32_t iDstStride, int32_t iRounding);

//8×8块的对角方向插值
void InterpolateHalfPelHV8x8_map(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst, int32_t iDstStride, int32_t iRounding);


//QUANT/DEQUANT
#define	  QuantIntra       QuantIntra_map    
#define   QuantInter       QuantInter_map    
#define   DeQuantIntra     DeQuantIntra_map  
#define   DeQuantInter     DeQuantInter_map  
#define	  DeQuantIntraVlx  DeQuantIntraVlx_map  
#define   DeQuantInterVlx  DeQuantInterVlx_map
 
//MPEG2QUANT/MPEG4DEQUANT  
#define   Mpeg2QuantIntra        ComlibMpeg2QuantIntra_map
#define   Mpeg2QuantInter        ComlibMpeg2QuantInter_map
#define   Mpeg2DeQuantIntra      Mpeg2DeQuantIntra_map
#define   Mpeg2DeQuantInter      Mpeg2DeQuantInter_map
#define   Mpeg2DeQuantIntraVlx   Mpeg2DeQuantIntraVlx_map
#define   Mpeg2DeQuantInterVlx   Mpeg2DeQuantInterVlx_map
 
//MPEG4QUANT/MPEG4DEQUANT
#define   Mpeg4QuantIntra   ComlibMpeg4QuantIntra_map
#define   Mpeg4QuantInter   ComlibMpeg4QuantInter_map  
#define   Mpeg4DeQuantIntra Mpeg4DeQuantIntra_map 
#define   Mpeg4DeQuantInter Mpeg4DeQuantInter_map

//DCT/IDCT
#define   DCT        DCT_map  
#define   IDCT       IDCT_map 

//SAD
#define   SAD16x16   SAD16x16_map  
#define   SAD8x8     SAD8x8_map
#define   RowSAD16   RowSAD16_map

//DEVIATIONAMB
#define    DeviationMB     DeviationMB_map

//INTERPOLATION
#define     InterpolateHalfPelH16x16    InterpolateHalfPelH16x16_map
#define     InterpolateHalfPelV16x16    InterpolateHalfPelV16x16_map
#define     InterpolateHalfPelHV16x16   InterpolateHalfPelHV16x16_map
#define     InterpolateHalfPelH8x8      InterpolateHalfPelH8x8_map 
#define     InterpolateHalfPelV8x8      InterpolateHalfPelV8x8_map
#define     InterpolateHalfPelHV8x8     InterpolateHalfPelHV8x8_map



#elif defined(COMMONLIB_INLINE)   //采用inline形式

inline_declare
{
		
	#include "../../../30-video/CommonLib/source/equator/DCT_IDCT.c"
	#include "../../../30-video/CommonLib/source/equator/DeviationMB.c"
	#include "../../../30-video/CommonLib/source/equator/QuantDeQuant.c"
	#include "../../../30-video/CommonLib/source/equator/SAD.c"
	#include "../../../30-video/CommonLib/source/equator/Interpolation.c"


}  

#else       //没有优化(C语言版本)

//QUANAT/DEQUANT
//对8x8的Intra块做量化
uint32_t QuantIntra_c(int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t iQuant);

//对8x8的Inter块做量化
uint32_t QuantInter_c(const int16_t *pInCoeff,int16_t *pOutCoeff, uint8_t iQuant);

//对8x8的Intra块做反量化
void DeQuantIntra_c(int16_t *pInCoeff,int16_t *pOutCoeff, uint8_t iQuant);

//对8x8的Inter块做反量化
void DeQuantInter_c(int16_t *pInCoeff, int16_t *pOutCoeff, int32_t iQuant);



//Mpeg4对8x8的Intra块做量化
uint32_t ComlibMpeg4QuantIntra_c(const int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t iQuant, uint8_t iDcScaler);

//Mpeg4对8x8的Inter块做量化
uint32_t ComlibMpeg4QuantInter_c(const int16_t *pInCoeff, int16_t * pOutCoeff, uint8_t iQuant);

//Mpeg4对8x8的Intra块做反量化
void Mpeg4DeQuantIntra_c(const int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t iQuant, uint8_t iDcScaler);

//Mpeg4对8x8的Inter块做反量化
void Mpeg4DeQuantInter_c(const int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t iQuant);



//Mpeg2对8x8的Intra块做量化
int ComlibMpeg2QuantIntra_c(int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t iDcScaler, uint8_t iQuant);

//Mpeg2对8x8的Inter块做量化
int ComlibMpeg2QuantInter_c(int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t iQuant);

//Mpeg2对8x8的Intra块做反量化
void Mpeg2DeQuantIntra_c(int16_t *pInCoeff, int16_t *pOutCoeff, uint8_t  iDcScaler, uint8_t  iQuant);

//Mpeg2对8x8的Inter块做反量化
void Mpeg2DeQuantInter_c(int16_t  *pInCoeff, int16_t  *pOutCoeff,  uint8_t  iQuant);



//DCT/IDCT
//8×8块DCT变换
void  DCT_c(int16_t* pBlock);

//8×8块DCT变换
void  IDCT_c(int16_t* pBlock);

//SAD
//计算16x16模块SAD
uint32_t SAD16x16_c(uint8_t *pCurrMB, int32_t iCurrStride,uint8_t *pRefMB, int32_t iRefStride);

//计算16x16模块SAD
uint32_t SAD8x8_c(uint8_t *pCurrMB,int32_t iCurrStride, uint8_t *pRefMB, int32_t iRefStride);


//DEVIATION
//求16×16宏块内各象素值与平均值差的绝对值之和
int32_t DeviationMB_c(const uint8_t *pMB, int32_t iStride);

//Interpolation
//16×16宏块的水平方向插值
void InterpolateHalfPelH16x16_c(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst, int32_t iDstStride, int32_t iRounding);

//16×16宏块的垂直方向插值
void InterpolateHalfPelV16x16_c(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst,  int32_t iDstStride, int32_t iRounding);

//16×16宏块的对角方向插值
void InterpolateHalfPelHV16x16_c(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst, int32_t iDstStride, int32_t iRounding);

//8×8块的水平方向插值
void InterpolateHalfPelH8x8_c(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst, int32_t iDstStride, int32_t iRounding);

//8×8块的垂直方向插值
void InterpolateHalfPelV8x8_c(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst, int32_t iDstStride, int32_t iRounding);

//8×8块的对角方向插值
void InterpolateHalfPelHV8x8_c(const uint8_t *pSrc, int32_t iSrcStride, uint8_t *pDst, int32_t iDstStride, int32_t iRounding);


//QUANT/DEQUANT
#define	  QuantIntra       QuantIntra_c    
#define   QuantInter       QuantInter_c    
#define   DeQuantIntra     DeQuantIntra_c  
#define   DeQuantInter     DeQuantInter_c  

//MPEG4QUANT/MPEG4DEQUANT
#define   Mpeg4QuantIntra   ComlibMpeg4QuantIntra_c
#define   Mpeg4QuantInter   ComlibMpeg4QuantInter_c  
#define   Mpeg4DeQuantIntra Mpeg4DeQuantIntra_c 
#define   Mpeg4DeQuantInter Mpeg4DeQuantInter_c


//MPEG2QUANT/MPEG2DEQUANT
#define  Mpeg2QuantIntra     ComlibMpeg2QuantIntra_c
#define  Mpeg2QuantInter     ComlibMpeg2QuantInter_c
#define  Mpeg2DeQuantIntra   Mpeg2DeQuantIntra_c
#define  Mpeg2DeQuantInter   Mpeg2DeQuantInter_c


//DCT/IDCT
#define   DCT        DCT_c  
#define   IDCT       IDCT_c 

//SAD
#define   SAD16x16   SAD16x16_c  
#define   SAD8x8     SAD8x8_c

//DEVIATIONAMB
#define    DeviationMB     DeviationMB_c

//INTERPOLATION
#define     InterpolateHalfPelH16x16    InterpolateHalfPelH16x16_c
#define     InterpolateHalfPelV16x16    InterpolateHalfPelV16x16_c
#define     InterpolateHalfPelHV16x16   InterpolateHalfPelHV16x16_c
#define     InterpolateHalfPelH8x8      InterpolateHalfPelH8x8_c 
#define     InterpolateHalfPelV8x8      InterpolateHalfPelV8x8_c
#define     InterpolateHalfPelHV8x8     InterpolateHalfPelHV8x8_c

  

#endif

#endif // _COMMON_H_ 

