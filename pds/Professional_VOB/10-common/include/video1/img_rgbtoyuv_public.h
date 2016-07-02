/******************************************************************************
模块名        ：img_rgbtoyuv_public
文件名        ：img_rgbtoyuv_public.h
相关文件    ：
文件实现功能：定义RGB转换成YUV格式的参数结构体
作者        ：张全磊
版本        ：1.0
-------------------------------------------------------------------------------
修改记录:
日  期            版本            修改人        走读人      修改内容
2009/12/22        1.0                张全磊                  create
******************************************************************************/
#ifndef _IMG_RGBTOYUV_PUBLIC_H_
#define _IMG_RGBTOYUV_PUBLIC_H_

#include "videodefines.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    l32 l32SrcWidth;         //源图像的宽度
    l32 l32SrcHeight;        //源图像的高度
    l32 l32SrcStride;        //源图像的Y步进值，目前需要传的就是RGB图像或者底图的实际物理宽度
    l32 l32SrcRGBType;

    l32 l32DstWidth;         //目的图像的宽度，目前只支持同尺寸的转换，此参数填成跟源一样即可，保留是为了以后扩展
    l32 l32DstHeight;         //目的图像的高度，目前只支持同尺寸的转换，此参数填成跟源一样即可，保留是为了以后扩展
    l32 l32DstStride;        //目的图像的Y步进值，这里可以填成图像宽度即可
    l32 l32DstYUVType;

    l32 l32TransposeFlag;    //是否需要图像翻转，0表示不翻转，1表示翻转
    u32 u32Reserved;
}TImageRGBtoYUVParam;

//图像缩放模块状态结构体
#define TImageRGBtoYUVStatusParam TImageRGBtoYUVParam

//图像缩放输入参数
typedef struct
{
    u8 *pu8SrcBAddr;         //源图像B分量地址,这里如果是标准的RGB格式，只需要填这一个指针即可
    u8 *pu8SrcGAddr;         //源图像G分量地址
    u8 *pu8SrcRAddr;         //源图像R分量地址

    void *pvRGBQuadMap;      //调色板数据区域,为了将来扩展，这里暂时可以不填
    u32 u32Reserved;
}TImageRGBtoYUVInput;

//图像缩放输出参数
typedef struct
{
    u8 *pu8DstYAddr;            //目的图像的Y分量地址
    u8 *pu8DstUAddr;            //目的图像的U分量地址
    u8 *pu8DstVAddr;            //目的图像的V分量地址

    u32 u32Reserved;
}TImageRGBtoYUVOutput;

#ifdef __cplusplus
}
#endif

#endif //_IMG_RGBTOYUV_PUBLIC_H_
