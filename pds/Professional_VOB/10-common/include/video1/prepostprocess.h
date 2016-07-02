/**                                                                            
*** Copyright (c) 2004-2005 Keda Communication Co.LTD.                         
**/

// Name    : prepostprocess.h 

#ifndef _IMAGE_PREPOSTPROCESS_H_
#define _IMAGE_PREPOSTPROCESS_H_

#include "algorithmtype.h"
#include "videocommondefines.h"

#define  QUALITY  0xf1f1

typedef struct tagImageInfo
{
	uint8_t  *pY;          //图像的Y分量
	uint8_t  *pU;          //图像的U分量
	uint8_t  *pV;          //图像的V分量
	int 	 iWidth;       //图像的宽度
	int 	 iHeight;      //图像的高度
	int	 iYStride;     //图像的Y分量步长
	int      iUVStride;    //图像的UV分量步长
	int	 iType;        //图像的类型
	int  iQuality;     //是否质量优先
}ImageInfo;


//////////huangkui 添加///////////////////////////////
typedef struct tagPreProcessType3Handle
{
	uint16_t width;		//output image width
	uint16_t height;	//output iamge height
		
	uint16_t *pref;		//pointer to reference image
	uint8_t *counter_y; //pointer to counter of previous  continous 4*4 block's averaged times
	uint8_t *y_status;	//pointer to average or not table for 4*4 blocks
	uint8_t *u_status;	//pointer to average or not table for 2*2 blocks
	uint8_t *v_status;	//pointer to average or not table for 2*2 blocks
	uint8_t *y2_status;	//pointer to average or not table for 4*4 blocks
	
	uint16_t *mbsad;	//pointer to current image's 4*4 block sad
	uint16_t *mbsadref;	//pointer to reference image's 4*4 block sad
	
 	uint16_t *sumrefprev[16];	//pointer to previous continous 15 image's 4*4 block sad
 	
 	uint16_t *sumrefprevbackup;	//pointer to save free pointer for sumrefprev[16]
		
}PreProcessType3Handle;
///////////////////////////////////////////////////////////////////////////

//返回值说明
/**************************
  
  0   ： 参数设置不合理（失败）
  1   ： 参数设置合理 （成功）
  
**************************/

//图像缩小
int ImageDownSize(ImageInfo *pOld, ImageInfo *pNew);


//图像放大
int ImageEnlargeSize(ImageInfo *pOld, ImageInfo *pNew);


//Deblock(H263和Mpeg4调用)
int ImageDeblock(ImageInfo *pOld, ImageInfo *pNew, int iQuant);
//DeblockVariableQp(H261调用)
int ImageDeblockVariableQp(ImageInfo *pImage, int *pQuant);

//Dering
int ImageDering(ImageInfo *pImage, int iQuant);


//RGB与YUV的格式转换
void Generate_yuv2rgb_tables(void);

int RGBtoYUV(ImageInfo *pRGB, ImageInfo *pYUV);

int YUVtoRGB(ImageInfo *pYUV, ImageInfo *pRGB);


//帧场变换(变换前后的图像大小一样)
int  FieldtoFrame(ImageInfo *pField, ImageInfo *pFrame);

int FrametoField(ImageInfo *pFrame, ImageInfo *pField);

//数据类型转换
int CopyUint8toInt16(uint8_t *pSrc, int16_t *pDest, int iSrcStride, int iDestStride);

int CopyUint8toInt16(uint8_t *pSrc, int16_t *pDest, int iSrcStride, int iDestStride);

int SubUint8toInt16(uint8_t *pSrc, uint8_t *pSrcRef, int16_t *pDest, int iSrcStride, int iSrcRefStride, int iDestStride);

int AddInt16toUint8(int16_t *pSrc, uint8_t *pSrcRef, uint8_t *pDest, int iSrcStride, int iSrcRefStride, int iDestStride);



//测试图像
int GenerateTestImage(ImageInfo *pImage, int iTestImageType);

//填充矩形区域
/*====================================================================
    函数名      :    DrawRectangleField420
    功能        ：在给定的图像里填充矩形块（图像格式420）
    算法实现    ：(可选项)
    引用全局变量：无 
    输入参数说明：uint8_t *pYUV  :  图像YUV数据
                  int32_t iHeight :  图像的高度
                  int32_t iWidth  :  图像的宽度
    		      int32_t iLeft   ： 矩形区域左坐标
                  int32_t iTop    ： 矩形区域上坐标
                  int32_t iRectHeight ： 矩形区域的高度
                  int32_t iRectWidth  ： 矩形区域的宽度
                  uint8_t  iR : 矩形区域色彩R分量
                  uint8_t  iG : 矩形区域色彩G分量
                  uint8_t  iB : 矩形区域色彩B分量
    返回值说明  ：参数设置错误返回 －1， 否则返回1；
====================================================================*/	
int32_t DrawRectangleField420(uint8_t *pYUV, int32_t iHeight, int32_t iWidth, int32_t iLeft,
					  int32_t iTop, int32_t iRectHeight, int32_t iRectWidth, 
					  uint8_t iR, uint8_t iG, uint8_t iB);

/*====================================================================
    函数名      ：DrawRectangle422
    功能        ：在给定的图像里填充矩形块（图像格式422）
    算法实现    ：(可选项)
    引用全局变量：无 
    输入参数说明：uint8_t *pYUV  :  图像YUV数据
                  int32_t iHeight :  图像的高度
                  int32_t iWidth  :  图像的宽度
    		      int32_t iLeft   ： 矩形区域左坐标
                  int32_t iTop    ： 矩形区域上坐标
                  int32_t iRectHeight ： 矩形区域的高度
                  int32_t iRectWidth  ： 矩形区域的宽度
                  uint8_t  iR : 矩形区域色彩R分量
                  uint8_t  iG : 矩形区域色彩G分量
                  uint8_t  iB : 矩形区域色彩B分量
    返回值说明  ：参数设置错误返回 －1， 否则返回1；
====================================================================*/	
int32_t DrawRectangleField422(uint8_t *pYUV, int32_t iHeight, int32_t iWidth, int32_t iLeft,
					  int32_t iTop, int32_t iRectHeight, int32_t iRectWidth, 
					  uint8_t iR, uint8_t iG, uint8_t iB);

void  GetPrePostProcessVersion(void *pVer, int iBufLen, int *pVerLen);

//////////huangkui 添加///////////////////////////////
int PreprocessType3Initial(PreProcessType3Handle *pHandle, int width, int height);

int PreprocessType3(uint8_t *p422, uint8_t *p420, PreProcessType3Handle *pHandle);

int PreprocessType3Close(PreProcessType3Handle *pHandle);

int PreprocessType3SetParam(PreProcessType3Handle *pHandle, int width, int height);

//0:fail; 1:succeed
int ImagePostprocess(ImageInfo *ptOldImage, ImageInfo *ptNewImage, int u32Type);

//////////////////////////////////////////////////////////

/*====================================================================
函数名      ： MotionSurveil
功能    	： 图像的运动侦测
算法实现    ：
引用全局变量：无
输入参数说明： TImageInfo  *ptCurImage  : 指向当前帧图像的指针
               TImageInfo  *ptPreImage  : 指向前一帧图像的指针
			   			 u8 *pu8MotionLevel       : 指向运动检测结果的指针
			   			 														(对每个bit,0表示静止，1表示运动)
返回值说明  ：成功返回 VIDEOSUCCESSFUL，失败返回 VIDEOFAILURE
----------------------------------------------------------------------
修改记录    ：
日  期      版本    修改人     走读人      修改内容
08/15/2005  1.0    	zhaobo                   创建 
====================================================================*/
l32 MotionSurveil(ImageInfo *ptCurImage, ImageInfo *ptPreImage, u8 *pu8MotionLevel);

#endif

