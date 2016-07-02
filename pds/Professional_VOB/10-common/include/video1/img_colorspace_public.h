/******************************************************************************
模块名	    ：img_colorspace_public
文件名	    ：img_colorspace_public.h            
相关文件	：
文件实现功能：定义颜色空间转换参数结构体
作者		：wgz
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人    修改内容 
2007/09/14		1.0				王国忠                 创建
******************************************************************************/
#ifndef _IMG_COLORSPACE_PUBLIC_H_
#define _IMG_COLORSPACE_PUBLIC_H_

#include "videodefines.h"

#ifdef __cplusplus
extern "C"
{
#endif

//YUV422放缩的参数设置结构体
typedef struct 
{
    l32 l32SrcWidth;	   //YUV422VGA图像的宽度
    l32 l32SrcHeight;      //YUV422VGA图像的高度
	l32 l32SrcISInterlace; //YUV422VGA图像的帧场类型
    l32 l32DstWidth;       //放缩后的YUV422图像的宽度
    l32 l32DstHeight;      //放缩后的YUV422图像的高度
	l32 l32DstISInterlace; //放缩后的YUV422图像的帧场类型
}TYUV422VGAzoomParam;

//YUV422放缩模块状态参数结构体
#define TImageYUV422VGAzoomStatusParam TYUV422VGAzoomParam
//YUV422放缩的输入结构体
typedef struct
{
	u8 *pu8SrcImgBuff; //指向YUV422图像的储存空间
	u8 *pu8DstImgBuff; //指向YUV422图像的储存空间
}TImageYUV422VGAzoomInput;

//YUV422放缩的输出结构体
typedef struct
{
	l32 l32DstISInterlace;  //YUV422图像的帧场类型
}TImageYUV422VGAzoomOutput;

//YUV422转为YUV420的参数设置结构体
typedef struct 
{
    l32 l32SrcWidth;	  //YUV422图像的宽度
    l32 l32SrcHeight;     //YUV422图像的高度
	l32 l32SrcISInterlace;  //YUV422图像的帧场类型
    l32 l32DstWidth;      //YUV420图像的宽度
    l32 l32DstHeight;     //YUV420图像的高度
	l32 l32DstISInterlace;  //YUV420图像的帧场类型
}TYUV422ToYUV420Param;

//YUV422转为YUV420模块状态参数结构体
#define TImageYUV422ToYUV420StatusParam TYUV422ToYUV420Param
//YUV422转YUV420输入结构体
typedef struct
{
	u8 *pu8SrcImgBuff; //指向YUV422图像的储存空间
	u8 *pu8DstImgBuff; //指向YUV420图像的储存空间
}TImageYUV422ToYUV420Input;

//YUV422转YUV420输出结构体
typedef struct
{
	l32 l32DstISInterlace;  //YUV420图像的帧场类型
	l32 l32IntraFrameFlag;  //I帧帧侧标记
    u32 u32Reserved;  //保留参数
}TImageYUV422ToYUV420Output;

//YUV420转为YUV422的参数设置结构体
typedef struct 
{
    l32 l32SrcWidth;      //YUV420图像的宽度
    l32 l32SrcHeight;     //YUV420图像的高度
	l32 l32SrcISInterlace;  //YUV420图像的帧场类型
    l32 l32DstWidth;      //YUV422图像的宽度
    l32 l32DstHeight;     //YUV422图像的高度
	l32 l32DstISInterlace;  //YUV422图像的帧场类型
}TYUV420ToYUV422Param;
//YUV420转为YUV422模块状态参数结构体
#define TImageYUV420ToYUV422StatusParam TYUV420ToYUV422Param

//YUV420转YUV422输入结构体
typedef struct
{
	u8 *pu8SrcImgBuff;  //指向YUV420图像的储存空间
	u8 *pu8DstImgBuff;  //指向YUV422图像的储存空间
}TImageYUV420ToYUV422Input;

//YUV420转YUV422输出结构体
typedef struct
{
	l32 l32DstISInterlace;  //YUV422图像的帧场类型	
}TImageYUV420ToYUV422Output;

//YUV420转RGB扩展类型
typedef enum
{
    YUV420_TO_RGB_NO_FLIP_IMG = 1,
}TExtendYUV420ToRGBType;

//YUV420转RGB扩展结构体
typedef struct
{
    TExtendYUV420ToRGBType tExtendType; //结构扩展类型
    u32 u32IsNoFlipRGB;                 //是否不翻转RGB图像(这里不能定义u8 否则在android平台上会有字节对齐问题)
}TYUV420ToRGBExtend;

// YUV420转RGB的参数设置结构体
typedef struct
{
	l32 l32SrcWidth;      //YUV420图像的宽度
	l32 l32SrcHeight;     //YUV420图像的高度
	l32 l32RGBTop;        //RGB图像在YUV420图像上的起始y坐标
	l32 l32RGBLeft;       //RGB图像在YUV420图像上的起始x坐标
	l32 l32WindowWidth;   //RGB图像窗的宽度
	l32 l32WindowHeight;  //RGB图像窗的高度 
	u8 u8RGBType;         //RGB图像的类型
}TYUV420ToRGBParam;

//YUV420转为RGB模块状态参数结构体
#define TImageYUV420ToRGBStatusParam TYUV420ToRGBParam
//YUV420转RGB输入结构体
typedef struct
{
	u8 *pu8SrcImgBuff;  //指向YUV420图像的储存空间
	u8 *pu8DstImgBuff;  //指向RGB图像的储存空间
}TImageYUV420ToRGBInput;

//YUV420转RGB输出结构体
typedef struct
{
	u32 u32Reserved;  //保留参数	
}TImageYUV420ToRGBOutput;

//YUV422拆分为4个cif大小的YUV420的参数设置结构体
typedef struct 
{
    l32 l32SrcWidth;	  //YUV422图像的宽度
    l32 l32SrcHeight;     //YUV422图像的高度
	l32 l32DstWidth1;     //第一个cif图像的宽度
    l32 l32DstHeight1;    //第一个cif图像的高度
	l32 l32DstWidth2;     //第二个cif图像的宽度
    l32 l32DstHeight2;    //第二个cif图像的宽度
	l32 l32DstWidth3;     //第三个cif图像的宽度
    l32 l32DstHeight3;    //第三个cif图像的宽度
	l32 l32DstWidth4;     //第四个cif图像的宽度
    l32 l32DstHeight4;    //第四个cif图像的宽度
}TYUV422ToYUV420D1To4xCifParam;

//YUV422拆分为4个cif大小的YUV420模块状态参数结构体
#define TImageYUV422ToYUV420D1To4xCifStatusParam TYUV422ToYUV420D1To4xCifParam
//YUV422D1拆分为4个cifYUV420输入结构体
typedef struct
{
	u8 *pu8SrcImgBuff;	  //指向YUV422图像的储存空间
	u8 *pu8DstImgBuff[4]; //指向YUV420图像的储存空间
}TImageD1SplitInput;

//YUV422D1拆分为4个cifYUV420输出结构体
typedef struct
{
	u32 u32Reserved;  //保留参数	
}TImageD1SplitOutput;

//BMP转为YUV并生成Alpha通道参数结构体
typedef struct  
{
	l32 l32YUVType;    //YUV类型
	u8 u8BackR;        //背景颜色的R分量
	u8 u8BackG;        //背景颜色的G分量
	u8 u8BackB;        //背景颜色的B分量
	u8 u8Transparency; //透明度(0表示全透明,255表示不透明)
}TImageBMPToYUVAlphaParam;

//BMP转为YUV并生成Alpha通道的状态参数结构体
#define TImageBMPToYUVAlphaStatusParam TImageBMPToYUVAlphaParam

//BMP转为YUV并生成Alpha通道输入参数结构体
typedef struct  
{
	u8 *pu8InBMP;     //指向输入BMP图像指针
    u8 *pu8OutYUV;    //指向输出图像的起始地址指针
	u8 *pu8OutAlpha;  //指向输出Alpha系数的起始地址指针
	u8 *pu8OutBeta;   //指向输出Beta系数的起始地址指针
}TImageBMPToYUVAlphaInput;

//BMP转为YUV并生成Alpha通道输出参数结构体
typedef struct  
{
	l32 l32Height;     //YUV图像的高度
	l32 l32Width;      //YUV图像的宽度
	l32 l32YUVType;    //YUV图像类型
	u8 u8Transparency; //透明度(0表示全透明,255表示不透明)
}TImageBMPToYUVAlphaOutput;

//GM8180与YUV420互转的参数结构体
typedef struct
{
    l32 l32Width;         //源图像的宽
    l32 l32Height;        //源图像的高
    l32 l32Left;          //要转换的部分图像在源图像中的X坐标
    l32 l32Top;           //要转换的部分图像在源图像中的Y坐标
    l32 l32BlockWidth;    //要转换的部分图像宽度
    l32 l32BlockHeight;   //要转换的部分图像高度
    l32 l32ImageType;       //输入图像的类型(若为GM8180_YUV420输出为YUV420, 若为YUV420输出为GM8180_YUV420)
} TImgGM8180ToYUV420Param;

//GM8180与YUV420互转的状态参数结构体
#define TImageGM8180ToYUV420StatusParam TImgGM8180ToYUV420Param

//GM8180与YUV420互转的输入参数结构体
typedef struct  
{
    u8 *pu8Input;     //指向输入图像的指针
    u8 *pu8Output;    //指向输出图像的指针
}TImageGM8180ToYUV420Input;

//GM8180与YUV420互转的输出参数结构体
typedef struct  
{
    u32 u32Reserved;  //保留参数
}TImageGM8180ToYUV420Output;

#ifdef __cplusplus
}
#endif

#endif 
