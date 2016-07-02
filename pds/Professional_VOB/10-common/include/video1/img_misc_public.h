/******************************************************************************
模块名	    ：img_misc_public
文件名	    ：img_misc_public.h            
相关文件	：
文件实现功能：定义生成图像的参数结构体
作者		：zwenyi
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人    修改内容
2007/09/14		1.0				zwenyi                  创建
******************************************************************************/
#ifndef _IMG_MISC_PUBLIC_H_
#define _IMG_MISC_PUBLIC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "videodefines.h"

//generateimage设置参数结构体
typedef struct
{
    l32 l32PatternType; //图案类型
	l32 l32Width;       //输入图像高度
	l32 l32Height;      //输入图像宽度
}TImageGenerateImageParam;

//generateimage参数状态结构体
#define TImageGenerateImageStatusParam TImageGenerateImageParam

//GenerateImage的输入参数
typedef struct  
{
    u8 *pu8YUV;   //指向图像指针
}TImageGenerateImageInput;

//GenerateImage的输出参数
typedef struct  
{
    u32 u32Reserved; //保留参数
}TImageGenerateImageOutput;

#ifdef __cplusplus
}
#endif

#endif // _IMG_MISC_PUBLIC_H_
