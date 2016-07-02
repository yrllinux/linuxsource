/*****************************************************************************
模块名      : 
文件名      : videnc_api.h
相关文件    : 
文件实现功能: 
作者        : 王建
版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/4/18   1.0         王建        创建
******************************************************************************/
#ifndef  _VIDENC_API_H_
#define  _VIDENC_API_H_

#ifdef __cplusplus
extern "C"
{
#endif

enum VidEncCmdId
{
    VIDENC_SET_PARAMS = 201,
    VIDENC_GET_STATUS,
    VIDENC_ANALYZE_ERRCODE,
    VIDENC_DEBUG		
};

l32 VidEncodeOpen(void **ppvHandle, void *in);
l32 VidEncodeClose(void *pvHandle);
l32 VidEncodeProcess(void *pvHandle, void *in, void *out, void *unionparam);
l32 VidEncodeControl(void *pvHandle, l32 cmdid, void *in, void *out);
l32 VidEncodeGetVersion(void *in, void *out);

extern TVidCodecApi tVidEnc;

#ifdef __cplusplus
}
#endif

#endif
