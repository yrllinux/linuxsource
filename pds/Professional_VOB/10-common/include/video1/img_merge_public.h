/******************************************************************************
模块名	    ：img_merge_public
文件名	    ：img_merge_public.h            
相关文件	：
文件实现功能：定义画面合成参数结构体
作者		：孙晓霞
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人    修改内容
2007/09/12		1.0		        孙晓霞                 创建
2007/11/02		1.1		        孙晓霞                 添加画面合成设置边界线宽度参数
******************************************************************************/
#ifndef _IMG_MERGE_PUBLIC_H_
#define _IMG_MERGE_PUBLIC_H_

#include "videodefines.h"

#ifdef __cplusplus
extern "C"
{
#endif

//前景图像信息结构体
typedef struct
{
	l32 l32Width;         //图像的宽度
	l32 l32Height;        //图像的高度
	l32 l32IsInterlace;   //图像的类型(帧格式或者场格式)
	l32 l32YUVType;       //图标图像格式(YUV422或者YUV420)
	u8 u8DrawFocusFlag;   //画面是否勾画边框的标记,u8Drawfocus为1时可用
	u8 u8FocuslineR;	  //画面边框色R
	u8 u8FocuslineG;	  //画面边框色G
	u8 u8FocuslineB;	  //画面边框色B
    u8 u8FocusWidth;      //边框统一宽度（暂不可设置）
	u8 u8Pos;			  //画面位置编号
	u8 u8NoPicInput;      //无前景图像输入标志位IMG_MERGE_NO_PIC_INPUT
	u32 u32Reserved;	  //保留参数
} TImageFgParam;

//画面合成模块初始化参数结构
typedef struct
{  
	u16	u16MergeStyle;	   //画面合成模式
	l32 l32Width;          //背景图像的宽度
	l32 l32Height;         //背景图像的高度
	l32	l32IsInterlace;    //背景图像的类型(帧格式或者场格式)
	l32 l32YUVType;        //背景图像格式(YUV422或者YUV420)
	u8 u8BackgroundR;	   //画面合成背景色R
	u8 u8BackgroundG;	   //画面合成背景色G
	u8 u8BackgroundB;	   //画面合成背景色B
    u8 u8BoundaryR;        //画面合成各路图像统一边界线颜色R
    u8 u8BoundaryG;        //画面合成各路图像统一边界线颜色G
    u8 u8BoundaryB;        //画面合成各路图像统一边界线颜色B
    u8 u8BoundaryWidth;    //画面合成边界线宽度（缺省值为0，背景为CIF时边界线宽度默认为2，背景为D1时边界线宽度默认为4）
    u8 u8PreprocessPic;    //PREPROCESS_NO_PIC_FILL_BG 前处理无图像处填充背景色
    u8 u8Drawsides;        //前处理中画边框 PREPROCESS_DRAWSIDES
    u8 u8Drawfocus;        //前景图像勾画选定边框 赋值为PREPROCESS_DRAWFOCUS，前景图像才会根据各自的设置画边框
	TImageFgParam atImageInput[PIC_MERGE_NUM_MAX];  //前景图像信息
} TImageMergeParam;

//画面合成模块状态查询结构体
#define TImageMergeStatusParam TImageMergeParam

//画面合成模块输入结构体
typedef struct 
{
    u8 *apu8InY[PIC_MERGE_NUM_MAX];  //输入前景N路图像的Y分量地址
    u8 *apu8InU[PIC_MERGE_NUM_MAX];  //输入前景N路图像的U分量地址(如果YUV地址连续存放可将U地址设置为NULL)
    u8 *apu8InV[PIC_MERGE_NUM_MAX];  //输入前景N路图像的V分量地址(如果YUV地址连续存放可将V地址设置为NULL)
    u8 *pu8OutY;                 //输出合成图像的Y分量地址
    u8 *pu8OutU;                 //输出合成图像的U分量地址(如果YUV地址连续存放可将U地址设置为NULL)
    u8 *pu8OutV;                 //输出合成图像的V分量地址(如果YUV地址连续存放可将V地址设置为NULL)
} TImageMergeInput;
 
//画面合成模块输出结构体
typedef struct 
{
    u32 u32Reserved;   //保留参数 
} TImageMergeOutput;

//画中画模块初始化参数结构
typedef struct
{  
	l32 l32BgWidth;          //背景图像的宽度
	l32 l32BgHeight;         //背景图像的高度
	l32 l32BgIsInterlace;    //背景图像的类型(帧格式或者场格式)
	l32 l32BgYUVType;	     //背景图像YUV类型
	l32 l32FgWidth;          //前景图像的宽度
	l32 l32FgHeight;         //前景图像的高度
	l32	l32FgIsInterlace;    //前景图像的类型(帧格式或者场格式)
	l32 l32FgYUVType;	     //前景图像YUV类型
	l32 l32Top;              //前景图像垂直位置偏移
	l32 l32Left;             //前景图像水平位置偏移
	l32 l32TopCutLine;       //前景上方裁减高度
	l32 l32BottomCutLine;    //前景下方裁减高度
	l32 l32LeftCutLine;      //前景左侧裁减宽度
	l32 l32RightCutLine;     //前景右侧裁减宽度
}TImageAddImageParam;

//画中画模块统计信息结构体
#define TImageAddImageStatusParam TImageAddImageParam

//画中画模块输入结构体
typedef struct 
{
    u8 *pu8FgYUV;    //前景图像地址
    u8 *pu8BgYUV;    //背景图像地址
}TImageAddImageInput;
 
//画中画模块输出结构体
typedef struct 
{
    u32 u32Reserved;   //保留参数 
}TImageAddImageOutput;

#ifdef __cplusplus
}
#endif

#endif
