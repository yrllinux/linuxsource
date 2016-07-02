/******************************************************************************
模块名	    ：img_zoom_public
文件名	    ：img_zoom_public.h            
相关文件	：
文件实现功能：定义图像缩放库参数结构体
作者		：zhaobo
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人      修改内容
2007/09/13		1.1				zhaobo                  create
2008/01/02      2.0             zhaobo                  add img_apt_zoom结构体
******************************************************************************/
#ifndef _IMG_ZOOM_PUBLIC_H_
#define _IMG_ZOOM_PUBLIC_H_

#include "videodefines.h"

#ifdef __cplusplus
extern "C"
{
#endif

//图像缩放参数
typedef struct  
{
	l32 l32SrcWidth;		//输入图像的宽度
	l32 l32SrcHeight;		//输入图像的高度
	l32 l32SrcYStride;		//输入图像Y分量的跨度
	l32 l32SrcUVStride;		//输入图像UV分量的跨度
	l32 l32SrcYUVType;		//输入图像的图像类型
	l32 l32SrcIsInterlace;	//输入图像的帧场类型

	l32 l32DstWidth;		//输出图像的宽度
	l32 l32DstHeight;		//输出图像的高度
	l32 l32DstYStride;		//输出图像Y分量的跨度
	l32 l32DstUVStride;		//输出图像UV分量的跨度
	l32 l32DstYUVType;		//输出图像的图像类型
	l32 l32DstIsInterlace;	//输出图像的帧场类型

	u8 u8ImageZoomID;		//图像缩放处理类型

}TImageZoomParam;

//图像缩放模块状态结构体
#define TImageZoomStatusParam TImageZoomParam

//图像缩放输入参数
typedef struct
{
	u8 *pu8Input;			//源图像
	u8 *pu8Output;			//目标图象
}TImageZoomInput;

//图像缩放输出参数
typedef struct
{
	u32 u32Reserved;		//保留参数
}TImageZoomOutput;

//PN转换的参数
typedef struct  
{
	l32 l32SrcWidth;		//输入图像的宽度
	l32 l32SrcHeight;		//输入图像的高度
	l32 l32SrcYStride;		//输入图像Y分量的跨度
	l32 l32SrcUVStride;		//输入图像UV分量的跨度
	l32 l32SrcYUVType;		//输入图像的图像类型
	l32 l32SrcIsInterlace;	//输入图像的帧场类型
	u8 u8SrcTVFormat;		//输入图像的制式

	l32 l32DstWidth;		//输出图像的宽度
	l32 l32DstHeight;		//输出图像的高度
	l32 l32DstYStride;		//输出图像Y分量的跨度
	l32 l32DstUVStride;		//输出图像UV分量的跨度
	l32 l32DstYUVType;		//输出图像的图像类型
	l32 l32DstIsInterlace;	//输出图像的帧场类型
	u8 u8DstTVFormat;		//输出图像的制式
	
	u8 u8InOutIsOneBuffer;	//PN制转换是否使用同一块内存
}TImagePNChangeParam;

//PN转换的状态结构体
#define TImagePNChangeStatusParam TImagePNChangeParam

//PN转换的输入参数
typedef struct
{
	u8 *pu8Input;			//源图像
	u8 *pu8Output;			//目标图像(源图像和目标图像可以一致)
}TImagePNChangeInput;

//PN转换的输出参数
typedef struct
{
	u32 u32Reserved;		//保留参数
}TImagePNChangeOutput;

//图像转换为16:9格式模块初始化参数结构
typedef struct
{  
	l32 l32Width;       //图像的宽度
	l32 l32Height;      //图像的高度
	l32 l32IsInterlace;	//图像的类型(帧格式或者场格式)
	l32 l32YUVType;		//图像YUV类型
	u8 u8TVFormat;		//图像制式
	
}TImageAdapt16vs9Param;

//图像转换为16:9格式模块状态结构体
#define TImageAdapt16vs9StatusParam TImageAdapt16vs9Param

//图像转换为16:9格式模块输入结构体
typedef struct 
{
    u8 *pu8YUV;    //图像地址
}TImageAdapt16vs9Input;
 
//图像转换为16:9格式模块输出结构体
typedef struct 
{
    u32 u32Reserved;   //保留参数 
}TImageAdapt16vs9Output;

//根据上层设置的参数缩放图像模块初始化参数结构
typedef struct
{
    l32 l32Width;		    //源图像的宽度
    l32 l32Height;		    //源图像的高度
    l32 l32YStride;		    //源图像Y分量的跨度
    l32 l32UVStride;		//源图像UV分量的跨度
    l32 l32SrcYUVType;	    //输入图像的图像类型
    l32 l32DstWidth;		//目的图像的宽度
    l32 l32DstHeight;		//目的图像的高度
    l32 l32DstStride;		//目的图像的跨度
    l32 l32DstYUVType;		//输出图像的图像类型
    l32 l32SrcIsInterlace;  //输入图像的帧场类型
    l32 l32DstIsInterlace;	//输出图像的帧场类型

    l32 l32LeftOffset;		//输出图像左边黑框宽度(必须是4的倍数)
    l32 l32RightOffset;		//输出图像右边黑框宽度(必须是4的倍数)
    l32 l32TopOffset;		//输出图像上边黑框宽度(必须是4的倍数)
    l32 l32BottomOffset;	//输出图像下边黑框宽度(必须是4的倍数)

    u32 u32Reserved;		//保留参数
}TImageAptZoomParam;

//根据上层设置的参数缩放图像模块状态结构体
#define TImageAptZoomStatusParam TImageAptZoomParam

//根据上层设置的参数缩放图像模块的输入结构体
typedef struct 
{
    u8 *pu8Input;			//源图像
    u8 *pu8Output;			//目标图像
}TImageAptZoomInput;

//根据上层设置的参数缩放图像模块的输出结构体
typedef struct 
{
    u8 *pu8Reserved;        //保留参数
}TImageAptZoomOutput;

//公用的结构
//UYVY图像缩放结构体
//图像的宽度包括左右的黑边宽度，图像宽度可以小于图像的跨度
//图像的高度包括上下的黑边宽度
//源图像和目的图像的结构类似
//通过这个结构就可以实现源图像中任意位置的图像缩放的目的图像的任意位置，还支持加黑边
//这个功能完全可以省去图像合成的消耗
typedef struct
{
    l32 l32SrcWidth;		//输入图像的宽度
    l32 l32SrcHeight;		//输入图像的高度
    l32 l32SrcStride;		//输入图像Y分量的跨度，如底图总宽度为1280，则其值为1280
    l32 l32SrcIsInterlace;	//输入图像的帧场类型

    l32 l32DstWidth;		//输出图像的宽度，包括黑边的宽度
    l32 l32DstHeight;		//输出图像的高度，包括黑边的宽度
    l32 l32DstStride;		//输出图像Y分量的跨度，如底图总宽度为1280，则其值为1280
    l32 l32DstIsInterlace;	//输出图像的帧场类型

	l32 l32TopBlackLine;    //上面的黑边行数
	l32 l32BottomBlackLine; //下面的黑边行数
	l32 l32LeftBlackLine;   //左边的黑边行数，应为2的倍数
	l32 l32RightBlackLine;  //右边的黑边行数，应为2的倍数

    l32 l32Reserved;
}TImageAptZoomUYVYParam;

//UYVY图像缩放模块状态结构体
#define TImageAptZoomUYVYStatusParam TImageAptZoomUYVYParam

//UYVY图像缩放输入参数
typedef struct
{
    u8 *pu8Input;			//源图像，可以是源图像的任意位置
    u8 *pu8Output;			//目标图象，可以是目的图像的任意位置
}TImageAptZoomUYVYInput;

//UYVY图像缩放输出参数
typedef struct
{
    u32 u32Reserved;		//保留参数
}TImageAptZoomUYVYOutput;

#ifdef __cplusplus
}
#endif

#endif //_IMG_ZOOM_PUBLIC_H_
