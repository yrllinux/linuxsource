/******************************************************************************
模块名	    ：img_multidec_new_public
文件名	    ：img_multidec_new_public.h
相关文件	：
文件实现功能：定义多解码器参数结构体
作者		：金立田
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人    修改内容
2008/05/09		1.0		        王建                 创建
******************************************************************************/
#ifndef _IMG_MULTIDEC_NEW_PUBLIC_H_
#define _IMG_MULTIDEC_NEW_PUBLIC_H_

#include "videodefines.h"
#include "vid_videounit.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_MULTIDEC_NUM    96

//多解码器参数结构
typedef struct
{
    l32 l32DecoderNum;                                      //解码器个数
    TVideoUnitDecParam atDecParam[MAX_MULTIDEC_NUM];	//解码器参数
    u32 u32Reserved;
}TImageMultiDecNewParam;

//多解码器输入参数结构
typedef struct
{
    l32 al32NeedDecode[MAX_MULTIDEC_NUM];               //是否解码的标志位
    TDecInput atDecInput[MAX_MULTIDEC_NUM];	            //解码器输入参数指针
    u32 u32Reserved;
}TImageMultiDecNewInput;

//多解码器输出参数结构
typedef struct
{
    TMultiDecOutput atMultiDecOutput[MAX_MULTIDEC_NUM]; //多路解码输出信息
    l32 al32RetVal[MAX_MULTIDEC_NUM];                   //每路解码返回值
    u8 *pu8OutBuf;                                      //多路解码输出Buf(一块大Buf)
    TBufGroupInfo atDecBufInfo[MAX_BUF_GROUPS];         //多路解码输出BUF组信息
    l32 l32DecBufGroups;                                //多路解码输出BUF组数
    l32 l32OffsetY;                                     //U分量起始偏移
    u32 u32Reserved;                                    
}TImageMultiDecNewOutput;

//多解码器状态参数结构
typedef struct
{
    l32 l32DecoderNum;                                              //解码器个数
	TVideoUnitDecStatusParam atDecStatus[MAX_MULTIDEC_NUM];     //解码器状态参数指针
	l32 al32RetVal[MAX_MULTIDEC_NUM];                           //函数返回值
    u32 u32Reserved;
}TImageMultiDecNewStatusParam;

#ifdef __cplusplus
}
#endif

#endif
