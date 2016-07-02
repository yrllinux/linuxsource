/******************************************************************************
模块名	    ：img_merge_zoom_public
文件名	    ：img_merge_zoom_public.h            
相关文件	：
文件实现功能：定义画面合成参数结构体
作者		：奚荣勇
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人    修改内容
2009/03/10		1.0		        奚荣勇                 创建
******************************************************************************/
#ifndef _IMG_MERGE_ZOOM_PUBLIC_H_
#define _IMG_MERGE_ZOOM_PUBLIC_H_

#include "videodefines.h"
#include "img_common_public.h"

#ifdef __cplusplus
extern "C" {
#endif

//缩放画面合成模块前景图像参数结构
typedef struct
{
    TFgInputSource l32FgInputSource;            //前景图像的输入内容（其中FG_PIC_INPUT为前景有图像输入；NO_PIC_INPUT为前景无图像输入）
    TPicZoomStyle l32ZoomStyle;                 //缩放模式（PIC_ZOOM_ONE：全屏缩放，PIC_ZOOM_TWO：保持比例拉伸并保持图像完整，PIC_ZOOM_THREE：保持比例拉伸并充满目标区域，PIC_ZOOM_FOUR：保持原有尺寸,大于合成尺寸的图像会居中并做裁边处理, PIC_ZOOM_FIVE 保持原有尺寸,大于合成尺寸的图像会按照保持比例拉伸并保持图像完整缩放）
    l32 l32ZoomScaleWidth;                      //针对缩放模式中模式二和三的缩放宽高比例，该参数与l32ZoomHeightScale成对使用(如前景按照4：3比例缩放，则该值为4，该参数为零则按照保持输入图像比例缩放)
    l32 l32ZoomScaleHeight;                     //针对缩放模式中模式二和三的缩放宽高比例，该参数与l32ZoomWidthScale成对使用(如前景按照4：3比例缩放，则该值为3，该参数为零则按照保持输入图像比例缩放)
    l32 l32FgPositionNum;                       //画面位置编号（画面位置编号约定按照从上到下从左到右顺序依次编号）
	l32 l32FgSrcWidth;                          //前景的输入源图像宽度
	l32 l32FgSrcHeight;                         //前景的输入源图像高度
	l32 l32FgFrameFieldFormat;                  //前景图像的类型(帧格式或者场格式)（帧格式为FRAME_FORMAT；场格式为FIELD_FORMAT）
	l32 l32FgYUVType;                           //前景图像格式(YUV422或者YUV420)
	l32 l32DrawFocusFlag;                       //画面是否勾画边框的标记（l32DrawFocusFlag为1表明画边框，l32DrawFocusFlag为0表明不画边框）
	l32 l32FocusRGB;	                        //画面边框色RGB分量（格式为0x00RRGGBB）
    l32 l32FocusWidth;                          //边框统一宽度（帧格式图像须为2的倍数，场格式图像需为4的倍数）,目前只支持与边界线保持一致
	u32 u32Reserved;	                        //保留参数
} TMergeZoomFgParam;

//缩放画面合成模块背景图像参数结构
typedef struct
{  
	l32 l32BgWidth;                             //背景图像的宽度
	l32 l32BgHeight;                            //背景图像的高度
	l32 l32BgFrameFieldFormat;                  //背景图像的类型(帧格式或者场格式)（帧格式为FRAME_FORMAT；场格式为FIELD_FORMAT）
	l32 l32BgYUVType;                           //背景图像格式(YUV422或者YUV420)	
	l32 l32DrawBoundaryFlag;                    //画面是否勾画边界线的标记（l32DrawBoundaryFlag为1表明画边界线，l32DrawBoundaryFlag为0表明不画边界线）
    l32 l32BoundaryRGB;                         //画面边界线颜色RGB分量（格式为0x00RRGGBB）
    l32 l32BoundaryWidth;                       //边界线统一宽度（帧格式图像须为2的倍数，场格式图像需为4的倍数）
	l32 l32DrawBackgroundFlag;                  //背景无图像处是否填充背景色
	l32 l32BackgroundRGB;	                    //画面合成背景填充色RGB分量（格式为0x00RRGGBB）
	u32 u32Reserved;	                        //保留参数
} TMergeZoomBgParam;

//缩放画面合成模块参数结构
typedef struct
{  
    l32 l32MaxFgNum;                            //画面合成的最大前景数量（默认为最大64画面）
    l32	l32MergeStyle;	                        //画面合成模式（共计44种合成类型，详见PicMergeZoomStyle枚举类型）    
    TMergeZoomBgParam tMergeZoomBgPic;          //背景图像参数信息
	TMergeZoomFgParam *ptMergeZoomFgPic;        //前景图像参数信息（由于前景图像数量不确定，因此此处采用结构体指针形式，实际分配大小由画面合成的最大前景数量决定）
	u32 u32Reserved;	                        //保留参数
} TImageMergeZoomParam;

//缩放画面合成模块状态查询参数结构
#define TImageMergeZoomStatusParam TImageMergeZoomParam

//缩放画面合成模块图像信息结构体
typedef struct 
{
    u8 *pu8Y;                                   //图像Y分量
    u8 *pu8U;                                   //图像U分量(如果YUV地址连续存放可将U地址设置为NULL)
    u8 *pu8V;                                   //图像V分量(如果YUV地址连续存放可将U地址设置为NULL)
    l32 l32YStride;                             //图像Y分量步长(如果YUV地址连续存放可将l32YStride设置为0)
    l32 l32UVStride;                            //图像UV分量步长(如果YUV地址连续存放可将l32YStride设置为0)
	u32 u32Reserved;	                        //保留参数
} TImageMergeZoomPicInfo;

//画面合成模块输入结构体
typedef struct 
{
    TImageMergeZoomPicInfo *ptImageMergeZoomInputPic;    //画面合成的N路输入图像的信息（由于前景图像数量不确定，因此此处采用结构体指针形式，实际分配大小由画面合成的最大前景数量决定）    
    TImageMergeZoomPicInfo tImageMergeZoomOutputPic;     //画面合成的输出图像的信息
	u32 u32Reserved;	                                 //保留参数
} TImageMergeZoomInput;
 
//画面合成模块输出结构体
typedef struct 
{
    u32 u32Reserved;   //保留参数 
} TImageMergeZoomOutput;

#ifdef __cplusplus
}
#endif

#endif
