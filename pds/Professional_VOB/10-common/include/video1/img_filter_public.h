/******************************************************************************
模块名	    ：img_filter_public
文件名	    ：img_filter_public.h
相关文件	：
文件实现功能：定义图像滤波的参数结构体,目前制包含添加马赛克
作者		：zwenyi
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人    修改内容
2007/09/14		1.0				zwenyi                  创建
2009/06/17      2.0             Z.Q.L                   新增垂直滤波
******************************************************************************/
#ifndef _IMG_FILTER_PUBLIC_H_
#define _IMG_FILTER_PUBLIC_H_

#include "videodefines.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	l32 l32WindowLeft;		//加马赛克区域的左边界
	l32 l32WindowTop;		//加马赛克区域的上边界
	l32 l32WindowWidth;		//加马赛克区域的宽度
	l32 l32WindowHeight;	//加马赛克区域的高度
	l32 l32Granularity;		//马赛克的粒度
}TMosaicInfo;

//加马赛克的参数
typedef struct
{
	l32 l32Width;			//图像的宽度
	l32 l32Height;			//图像的高度
	l32 l32YStride;			//图像Y分量的跨度
	l32 l32UVStride;		//图像UV分量的跨度
	l32 l32YUVType;			//图像的图像类型
	l32 l32IsInterlace;		//图像的帧场类型

	l32 l32MosaicNum;		//添加马赛克的实际路数
	TMosaicInfo tMosaicInfo[32];
}TImageMosaicParam;

//加马赛克模块状态结构体
#define TImageMosaicStatusParam TImageMosaicParam

//加马赛克的输入参数
typedef struct
{
	u8 *pu8Input;			//源图像
	u8 *pu8Output;			//目标图象(源图像和目标图像可以一致)
}TImageMosaicInput;

//加马赛克的输出参数
typedef struct
{
	u32 u32Reserved;		//保留参数
}TImageMosaicOutput;

//OSD filter的参数
typedef struct
{
    u8  u8Mask11;           //平滑滤波强度
    s16 s16Brightness;      //亮度参数
    s16 s16Contrast;        //对比度参数
    l32 l32Width;           //图像的宽度
    l32 l32Height;          //图像的高度
    u32 *pu32RefPic;        //参考图像地址
    u8  u8Sign;             //是否有新的参考图像传入标志位
}TFilterOSDParam;

//OSD filter的输入参数
typedef struct
{
    u32 *pu32Input;			//源图像
	u32 *pu32Output;		//目标图象(源图像和目标图像可以一致)
}TFilterOSDInput;

//OSD filter的输出参数
typedef struct
{
	u32 u32Reserved;		//保留参数
}TFilterOSDOutput;

//图像滤波参数
typedef struct
{
	l32 l32ImgWidth;     //源图像的宽度
	l32 l32ImgHeight;    //源图像的高度
	l32 l32ImgYStride;   //源图像的Y步进值
	l32 l32ImgUVStride;  //源图像的UV步进值，如果是默认Y步进值的一半，这里可以赋成0

	l32 l32YUVType;      //源的YUVtype
	l32 l32Interlace;    //源的帧场类型
	l32 l32FilterType;   //滤波器的类型
	l32 l32Quility;      //质量等级，目前此参数并没有使用

    u32 u32Reserved;
}TImageFilterParam;

//图像滤波模块状态结构体
#define TImageFilterStatusParam TImageFilterParam

//图像滤波输入参数
typedef struct
{
	u8 *pu8ImgYAddr;			//源图像Y分量地址
	u8 *pu8ImgUAddr;			//源图像U分量地址,如果是连续空间，次分量可以赋空
	u8 *pu8ImgVAddr;			//源图像V分量地址,如果是连续空间，次分量可以赋空

	u32 u32Reserved;
}TImageFilterInput;

//图像滤波输出参数
typedef struct
{
	u32 u32Reserved;
}TImageFilterOutput;


//图像锐化参数，本模块支持原位锐化
typedef struct
{
	l32 l32ImgWidth;     //源图像的宽度
	l32 l32ImgHeight;    //源图像的高度

	l32 l32Factor;       //锐化时的权重值,范围为0-255，推荐值为128

    u32 u32Reserved;
}TImageSharpParam;

//图像锐化模块状态结构体
#define TImageSharpStatusParam TImageSharpParam

//图像锐化输入参数，pu8SrcUAddr、pu8SrcVAddr、l32SrcUVStride，任意一个为零则认为时连续buffer
typedef struct
{
	u8 *pu8SrcYAddr;			//源图像Y分量地址
	u8 *pu8SrcUAddr;			//源图像U分量地址，可以赋值为0，这样就认为图像是连续的
	u8 *pu8SrcVAddr;			//源图像V分量地址，可以赋值为0，这样就认为图像是连续的

	l32 l32SrcYStride;          //源图像的Y步进值
	l32 l32SrcUVStride;			//源图像的UV步进值，可以赋值为0，这样就认为图像是连续的

	u32 u32Reserved;
}TImageSharpInput;

//图像锐化输出参数,目的图像可以和源图像完全一致，参数要求一样
//pu8DstUAddr、pu8DstVAddr、l32DstUVStride，任意一个为零则认为时连续buffer
typedef struct
{
	u8 *pu8DstYAddr;			//目的图像的Y分量地址
	u8 *pu8DstUAddr;			//目的图像的U分量地址，可以赋值为0，这样就认为图像是连续的
	u8 *pu8DstVAddr;			//目的图像的V分量地址，可以赋值为0，这样就认为图像是连续的

	l32 l32DstYStride;          //目的图像的Y步进值
	l32 l32DstUVStride;			//目的图像的UV步进值，可以赋值为0，这样就认为图像是连续的

	u32 u32Reserved;
}TImageSharpOutput;

#ifdef __cplusplus
}
#endif

#endif //_IMG_FILTER_PUBLIC_H_
