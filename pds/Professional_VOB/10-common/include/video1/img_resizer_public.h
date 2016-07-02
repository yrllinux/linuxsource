/******************************************************************************
模块名	    ：img_resizer_public
文件名	    ：img_resizer_public.h
相关文件	：
文件实现功能：定义图像缩放库参数结构体
作者		：张全磊
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人      修改内容
2008/06/18		1.0				张全磊                  create
******************************************************************************/
#ifndef _IMG_RESIZER_PUBLIC_H_
#define _IMG_RESIZER_PUBLIC_H_

#include "videodefines.h"

#ifdef __cplusplus
extern "C"
{
#endif

//源图像的结构信息
typedef struct
{
	l32 l32SrcWidth;     //源图像的宽度
	l32 l32SrcHeight;    //源图像的高度
	l32 l32SrcYUVType;	 //源图像的图像类型，为YUV420或者YUV422
	l32 l32SrcInterlace; //源图像的帧场类型

	l32 l32SrcTopCutLine;     //上面的裁剪的行数，应为2的倍数
	l32 l32SrcBottomCutLine;  //下面的裁剪的行数，应为2的倍数
	l32 l32SrcLeftCutPoint;   //左边的裁剪的点数，应为2的倍数
	l32 l32SrcRightCutPoint;  //右边的裁剪的点数，应为2的倍数

	u32 u32Reserved;
}TSrcInfo;

//目的图像的结构参数
typedef struct
{
	l32 l32DstWidth;        //目的图像的宽度
	l32 l32DstHeight;		//目的图像的高度
	l32 l32DstYUVType;		//目的图像的图像类型，为YUV420或者YUV422
	l32 l32DstInterlace;	//目的图像的帧场类型

	l32 l32DstTopCutLine;     //上面的裁剪的行数，应为2的倍数
	l32 l32DstBottomCutLine;  //下面的裁剪的行数，应为2的倍数
	l32 l32DstLeftCutPoint;   //左边的裁剪的点数，应为2的倍数
	l32 l32DstRightCutPoint;  //右边的裁剪的点数，应为2的倍数

	l32 l32TopFillLine;     //上面的添加的颜色行数，应为2的倍数
	l32 l32BottomFillLine;  //下面的添加的颜色行数，应为2的倍数
	l32 l32LeftFillPoint;   //左边的添加的颜色点数，应为2的倍数
	l32 l32RightFillPoint;  //右边的添加的颜色点数，应为2的倍数

	l32 l32YColor;
	l32 l32UColor;
	l32 l32VColor;          //填充的边缘的颜色，目前黑色为Y:0x01,UV:0x80,绿色是全零，也可以考虑设置成RGB三个参数

	u32 u32Reserved;
}TDstInfo;

//图像缩放参数
//公用的结构
//图像通用缩放结构体
//图像的宽度包括左右的黑边宽度，图像宽度可以小于图像的跨度
//图像的高度包括上下的黑边宽度
//通过这个结构就可以实现源图像中任意位置的图像缩放的目的图像的任意位置，还支持加黑边
//这个功能完全可以省去图像合成的消耗
typedef struct
{
	TSrcInfo tSrcInfo;   //源图像的相关信息

	TDstInfo tDstInfo;   //目的图像的相关信息

    u32 u32Reserved;
}TImageResizerParam;

//图像缩放模块状态结构体
#define TImageResizerStatusParam TImageResizerParam

//图像缩放输入参数
typedef struct
{
	u8 *pu8SrcYAddr;			//源图像Y分量地址
	u8 *pu8SrcUAddr;			//源图像U分量地址
	u8 *pu8SrcVAddr;			//源图像V分量地址

	l32 l32SrcYStride;          //源图像的Y步进值
	l32 l32SrcUVStride;			//源图像的UV步进值

	u32 u32Reserved;
}TImageResizerInput;

//图像缩放输出参数
typedef struct
{
	u8 *pu8DstYAddr;			//目的图像的Y分量地址
	u8 *pu8DstUAddr;			//目的图像的U分量地址
	u8 *pu8DstVAddr;			//目的图像的V分量地址

	l32 l32DstYStride;          //目的图像的Y步进值
	l32 l32DstUVStride;			//目的图像的UV步进值

	l32 l32IntraFrameFlag;      //场景检测标记
	u32 u32Reserved;
}TImageResizerOutput;

#define RESIZER_MODE_EXPAND   0xaaaa
#define RESIZER_MODE_NEAREST  0x5555
//将参数结构体的u32Reserved指向下面的结构体
typedef struct
{
    u32 u32ResizerModeExpend;     //扩展类型，需要是上面的宏RESIZER_MODE_EXPAND的值
    l32 l32ResizerMode;           //缩放模式，目前只支持临近点RESIZER_MODE_NEAREST

    u32 u32Reserved;
}TReszierMode;
#ifdef __cplusplus
}
#endif

#endif //_IMG_RESIZER_PUBLIC_H_
