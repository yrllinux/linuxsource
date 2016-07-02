/******************************************************************************
模块名	    ：img_multienc_public
文件名	    ：img_multienc_public.h
相关文件	：
文件实现功能：定义多编码器参数结构体
作者		：王 建
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人    修改内容
2008/10/21		1.0		        王 建                创建
******************************************************************************/
#ifndef _IMG_MULTIENC_PUBLIC_H_
#define _IMG_MULTIENC_PUBLIC_H_

#include "videodefines.h"
#include "vid_videounit.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_MULTIENC_NUM    8

//多编码器参数结构
typedef struct
{
    l32 l32EncoderNum;                                  //编码器个数
    TVideoUnitEncParam atEncParam[MAX_MULTIENC_NUM];	//编码器参数
}TImageMultiEncParam;

//多编码器输入参数结构
typedef struct
{
    TEncInput atEncInput[MAX_MULTIENC_NUM];	        //编码器输入参数指针
    u32 u32Reserved;
}TImageMultiEncInput;

//多编码器输出参数结构
typedef struct
{
    TEncOutput atEncOutput[MAX_MULTIENC_NUM];	        //编码器输出参数指针
    l32 al32RetVal[MAX_MULTIENC_NUM];                  //函数返回值
    u32 u32Reserved;
}TImageMultiEncOutput;

//多编码器状态参数结构
typedef struct
{
    l32 l32EncoderNum;                                          //编码器个数
	TVideoUnitEncStatusParam atEncStatus[MAX_MULTIENC_NUM];     //编码器状态参数指针
	l32 al32RetVal[MAX_MULTIENC_NUM];                           //函数返回值
}TImageMultiEncStatusParam;

typedef struct
{
    TDebugInfoInput atDebugInfoInput[MAX_MULTIENC_NUM];
    u32 u32Reserved;
}TImageMultiEncDebugInput;

#ifdef __cplusplus
}
#endif

#endif
