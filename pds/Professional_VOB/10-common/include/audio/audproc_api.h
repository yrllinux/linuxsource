/*****************************************************************************
模块名      : 
文件名      : audproc_api.h
相关文件    : 
文件实现功能: 
作者        : 陆志洋
版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/5/13   1.0         陆志洋        创建
2009/11/16  1.1         陆志洋        增加 AUDPROC_SETPARAMS 指令
******************************************************************************/
#ifndef  _AUDPROC_API_H_
#define  _AUDPROC_API_H_
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

#include "audcom_api.h"

enum AudProcCmdId
{
    AUDPROC_GET_STATUS = 701,
    AUDPROC_ANALYZE_ERRCODE,
    AUDPROC_DEBUG,
    AUDPROC_SET_PARAMS		
};

l32 AudioProcOpen(void **ppvHandle, void *ptAudProcParam);
l32 AudioProcClose(void *pvHandle);
l32 AudioProcProcess(void *pvHandle, void *ptAudProcInput, void *ptAudProcOutput, void *unionparam);
l32 AudioProcControl(void *pvHandle, l32 cmdid, void *ptAudProControlInParam, void *ptAudProControlOutParam);
l32 AudioProcGetVersion(void *ptAudVerInfoInput, void *ptAudVerInfoOutput);

extern TAudCodecApi tAudioProc;

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif


