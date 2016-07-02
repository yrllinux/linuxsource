/******************************************************************************
模块名	    ：img_multidec_public
文件名	    ：img_multidec_public.h
相关文件	：
文件实现功能：定义多解码器参数结构体
作者		：金立田
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人    修改内容
2008/01/02		1.0		        金立田                创建
******************************************************************************/
#ifndef _IMG_MULTIDEC_PUBLIC_H_
#define _IMG_MULTIDEC_PUBLIC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "videodefines.h"
#include "vid_videounit.h"

#define MAX_MULTIDEC_NUM    64

//多解码器参数结构
typedef struct
{
    l32 l32DecoderNum;                                  //解码器个数
    TVideoUnitDecParam atDecParam[MAX_MULTIDEC_NUM];	//解码器参数
}TImageMultiDecParam;

//多解码器输入参数结构
typedef struct
{
    l32 al32NeedDecode[MAX_MULTIDEC_NUM];             //解码标志位(0:不解码；1：解码；2：输出前一帧图像）
    TDecInput atDecInput[MAX_MULTIDEC_NUM];	         //解码器输入参数指针
}TImageMultiDecInput;

//多解码器输出参数结构
typedef struct
{
    TDecOutput atDecOutput[MAX_MULTIDEC_NUM];	        //解码器输出参数指针
    l32 al32RetVal[MAX_MULTIDEC_NUM];                  //函数返回值
}TImageMultiDecOutput;

//多解码器状态参数结构
typedef struct
{
    l32 l32DecoderNum;                                          //解码器个数
	TVideoUnitDecStatusParam atDecStatus[MAX_MULTIDEC_NUM];     //解码器状态参数指针
	l32 al32RetVal[MAX_MULTIDEC_NUM];                           //函数返回值
}TImageMultiDecStatusParam;

#ifdef __cplusplus
}
#endif

#endif
