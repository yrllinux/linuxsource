/******************************************************************************
模块名	    ：rgb_resizer_public
文件名	    ：rgb_resizer_public.h
相关文件	：
文件实现功能：定义图像缩放库参数结构体
作者		：张全磊
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人      修改内容
2008/10/17		1.0				张全磊                  create
******************************************************************************/
#ifndef _RGB_RESIZER_PUBLIC_H_
#define _RGB_RESIZER_PUBLIC_H_

#include "videodefines.h"

typedef struct
{
	l32 l32SrcWidth;     //源图像的宽度
	l32 l32SrcHeight;    //源图像的高度
	l32 l32SrcRGBType;

	l32 l32DstWidth;        //目的图像的宽度
	l32 l32DstHeight;		//目的图像的高度
	l32 l32DstRGBType;

    u32 u32Reserved;
}TRGBResizerParam;

//图像缩放模块状态结构体
#define TRGBResizerStatusParam TRGBResizerParam

//图像缩放输入参数
typedef struct
{
	u8 *pu8SrcAddr;			//源图像Y分量地址

	l32 l32SrcStride;          //源图像的Y步进值

	u32 u32Reserved;
}TRGBResizerInput;

//图像缩放输出参数
typedef struct
{
	u8 *pu8DstAddr;			//目的图像的Y分量地址

	l32 l32DstStride;          //目的图像的Y步进值

	u32 u32Reserved;
}TRGBResizerOutput;

#endif //_RGB_RESIZER_PUBLIC_H_
