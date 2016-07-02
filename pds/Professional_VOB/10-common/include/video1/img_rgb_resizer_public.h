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
#ifndef _IMG_RGB_RESIZER_PUBLIC_H_
#define _IMG_RGB_RESIZER_PUBLIC_H_

#include "videodefines.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	l32 l32SrcWidth;     //源图像的宽度
	l32 l32SrcHeight;    //源图像的高度
	l32 l32SrcRGBType;

	l32 l32DstWidth;        //目的图像的宽度
	l32 l32DstHeight;		//目的图像的高度
	l32 l32DstRGBType;

    u32 u32Reserved;
}TImageRGBResizerParam;

//图像缩放模块状态结构体
#define TImageRGBResizerStatusParam TImageRGBResizerParam

//图像缩放输入参数
typedef struct
{
    //源图像Y分量地址，由于计算RGB的stride需要四字节对齐，
    //所以这里空间申请的方式应该是stride(((ptInput->l32SrcStride  * ptRGBResizerInfo->l32SrcRGBType  + 31) & ~31) >> 3)乘以高度
    // 上层在申请内存的时候需要注意，不要申请的空间不够
	u8 *pu8SrcAddr;			//源图像Y分量地址
    void *pvRGBQuadMap;      //调色板数据区域
	l32 l32SrcStride;       //源图像的Y步进值，目前需要传的就是图像或者底图的实际物理宽度
	u32 u32Reserved;
}TImageRGBResizerInput;

//图像缩放输出参数
typedef struct
{
    //目的图像的Y分量地址，由于计算RGB的stride需要四字节对齐，
    //所以这里空间申请的方式应该是stride(((ptOutput->l32DstStride * ptRGBResizerInfo->l32DstRGBType + 31) & ~31) >> 3)乘以高度
    // 上层在申请内存的时候需要注意，不要申请的空间不够，造成底层写越界
	u8 *pu8DstAddr;			//目的图像的Y分量地址
	l32 l32DstStride;       //目的图像的Y步进值
	u32 u32Reserved;
}TImageRGBResizerOutput;

#ifdef __cplusplus
}
#endif

#endif //_IMG_RGB_RESIZER_PUBLIC_H_
