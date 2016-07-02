/******************************************************************************
模块名	    ：img_deinterlace_public
文件名	    ：img_deinterlace_public.h
相关文件	：
文件实现功能：定义图像缩放库参数结构体
作者		：张全磊
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人      修改内容
2008/06/18		1.0				张全磊                  create
******************************************************************************/
#ifndef _IMG_DEINTERLACE_PUBLIC_H_
#define _IMG_DEINTERLACE_PUBLIC_H_

#include "videodefines.h"

#ifdef __cplusplus
extern "C"
{
#endif

//Deinterlace设置参数结构体
typedef struct
{
	l32 l32Width;           //图像高度
	l32 l32Height;          //图像宽度
	l32 l32YStride;         //图像Y分量步长
	l32 l32UVStride;        //图像UV分量步长
	l32 l32DeinterlaceMode; //DEINTERLACE模式
	l32 l32Interlace;	    //图像的帧场类型(推荐值为FRAME_FORMAT，目前只有DEINTERLACE_MODE_1下不允许FIELD_FORMAT的格式)
	u32 u32Reserve;
}TImageDeinterlaceParam;

//Deinterlace状态参数
#define TImageDeinterlaceStatusParam TImageDeinterlaceParam

//Deinterlace的输入结构体
typedef struct
{
	u8 *pu8Y;      //图像Y分量指针
	u8 *pu8U;      //图像U分量指针
	u8 *pu8V;      //图像V分量指针
}TImageDeinterlaceInput;

//Deinterlace的输出结构体
typedef struct
{
    u32 u32Reserved; //保留参数
}TImageDeinterlaceOutput;

#ifdef __cplusplus
}
#endif

#endif //_IMG_DEINTERLACE_PUBLIC_H_
