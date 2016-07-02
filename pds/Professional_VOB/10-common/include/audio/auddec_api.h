/*****************************************************************************
模块名      : 
文件名      : auddec_api.h
相关文件    : 
文件实现功能: 
作者        : 陆志洋
版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/05/13   1.0         陆志洋        创建
2009/11/16   1.1         陆志洋        增加 AUDDEC_SETPARAMS 指令
******************************************************************************/
#ifndef  _AUDDEC_API_H_
#define  _AUDDEC_API_H_
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

#include "audcom_api.h"

enum AudDecCmdId
{
    AUDDEC_GET_STATUS = 601,
    AUDDEC_ANALYZE_ERRCODE,
    AUDDEC_DEBUG,
    AUDDEC_SET_PARAMS		
};

l32 AudDecodeOpen(void **ppvHandle, void *ptAudDecParam);
l32 AudDecodeClose(void *pvHandle);
l32 AudDecodeProcess(void *pvHandle, void *ptAudDecInput, void *ptAudDecOutput, void *unionparam);
l32 AudDecodeControl(void *pvHandle, l32 cmdid, void *ptAudDecControlInParam, void *ptAudDecControlOutParam);
l32 AudDecodeGetVersion(void *ptAudVerInfoInput, void *ptAudVerInfoOutput);

extern TAudCodecApi tAudDec;

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif


