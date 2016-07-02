#ifndef _ADDLOGO_H_
#define _ADDLOGO_H_
/*
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
typedef short          int16_t;
typedef unsigned long  n32u;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long LONG;
typedef unsigned char  BYTE;
#define uint32_t	unsigned int
#define int32_t		int

#define FALSE 0
#define TRUE  1
*/


typedef struct tagLogoInfo
{
  uint8_t   *pFg;                      //前景图象数据指针地址
  uint8_t   *pAlphaY;                  //前景图像Y分量的alpha通道指针
  uint8_t   *pBetaY;                   //对应背景图像的内插系数
  uint8_t   *pAlphaUV;                 //前景图像UV分量的alpha通道指针
  uint8_t   *pBetaUV;                  //对应背景图像的内插系数 
  uint16_t  nWidth, nHeight;           //前景图像的宽度和高度
  uint8_t  byTransparency;             //前景图象透明系数
  int16_t   sTop,sLeft;                //前景图象位置偏移
  int16_t	sTopCut, sBottomCut, sLeftCut, sRightCut;  	//前景图象上、下裁去行数
} TLogoInfo;


typedef struct tagImageParam
{
  uint8_t   *pImageData;      		//图像YUV数据         
  int nWidth;				//图像宽度
  int nHeight;          		//图像高度
  int  nStride;				//图像步长
} TImageParam;


void ResCreate (uint8_t *YImg, uint8_t *UImg, uint8_t *VImg);
void ColorCreate (uint8_t *YImg, uint8_t *UImg, uint8_t *VImg);
void HueCreate (uint8_t *YImg, uint8_t *UImg, uint8_t *VImg);

//获取版本信息
void GetZoomImageVersion(void *pVer, int iBufLen, int *pVerLen);


//BMP转换到YUV并生成ALPHA矩阵
int ConvertBMPToYUVAndAlpha(uint8_t *pLogoBmp, TLogoInfo *pFgInfo, int nYUVType, uint8_t back_r, uint8_t back_g, uint8_t back_b, uint8_t transparency);



//加图标
//void AddLogo(TImageParam *pBackgroundImage, TLogoInfo *pForegroundInfo, int nYUVType);
int AddLogo(TImageParam *pBackgroundImage, TLogoInfo *pForegroundInfo, int nTop, int nLeft, int nYUVType);


//图像缩小后覆盖到大图像
//nZoomOutType 1:1/4; 2:1/9; 3:1/16; 4:1/2(horizontal) 5:1/2(vertical) 6:4/9 7:9/16 8:1/25 9:3/4(vertical)
int ZoomOutImageAndMerge(int nZoomOutType, TImageParam *pBackgroundImage, TImageParam *pForegroundImage, int nTop, int nLeft, int nIsFrame);

//422格式720*576图像缩小后覆盖到大图像   05.2.20 sq add
//s32ZoomOutType 1:1/4; 2:1/9; 3:1/16
int ZoomOutImageAndMerge422Field(int s32ZoomOutType, TImageParam *ptBackgroundImage, TImageParam *ptForegroundImage, int s32Top, int s32Left);
//sq end

//缩小图像至原图1/4
void ZoomOutImageQuarter(int nOldWidth, int nOldHeight, unsigned char *pOldImageData, unsigned char *pNewImageData);

//缩小图像至原图1/9
void ZoomOutImageNinth(int nOldWidth, int nOldHeight, unsigned char *pOldImageData, unsigned char *pNewImageData);

//缩小图像至原图1/16
void ZoomOutImageSixteenth(int nOldWidth, int nOldHeight, unsigned char *pOldImageData, unsigned char *pNewImageData);

//小图像覆盖大图像
/*====================================================================
    函数名      ：MergePicture
    功能        ：将小图像（YUV420）覆盖到大图像（YUV420）的指定位置（场格式）
    算法实现    ：(可选项)
    引用全局变量：无
    输入参数说明：TImageParam *pBackgroundImage: 背景图像指针 
		  TImageParam *pForegroundImage: 前景图像指针
    		  uint16_t fgTop： 小图像垂直方向偏移
    		  uint16_t fgLeft：小图像水平方向偏移
			  int nIsFrame：是否为帧格式
    		  uint16_t fgTopOffset: 小图像上边删除行数
    		  uint16_t fgLeftOffset：小图像左边删除行数
    		  uint16_t fgBottonOffset:小图像下边删除行数
    		  uint16_t fgRightOffset:小图像右边删除行数
    返回值说明  ：无
====================================================================*/
void MergePicture(TImageParam *pBackgroundImage,TImageParam *pForegroundImage, uint16_t fgTop, uint16_t fgLeft, int nIsFrameFg, int nIsFrameBg, int fgTopOffset, int fgLeftOffset,
				uint16_t fgBottonOffset, uint16_t fgRightOffset);

void de_interlacing_map(unsigned char *pYUV, int width, int height, int stride);

//生成一组运动连续的帧
void TestFrameGenerator(int nColumnWidth, int nSpeed, int nSkipInterval, int nSkipCount, 
		unsigned char *pYUV, int nHeight, int nWidth, unsigned char *pBuffer);
/*************
调用一次生成一帧
int nColumnWidth 滚动条宽度（建议设为20）
int nSpeed  滚动条速度（建议设为8）
int nSkipInterval 连续丢帧数量（为0表示不丢帧，运动连续）
int nSkipCount 丢帧间隔
unsigned char *pYUV 输出图像数据
int nHeight 图像高度
int nWidth 图像宽度
unsigned char *pBuffer 缓存（720*576*3/2 byte）
*************************/







#endif
