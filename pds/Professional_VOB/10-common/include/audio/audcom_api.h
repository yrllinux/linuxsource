/*****************************************************************************
模块名      : 
文件名      : audcom_api.h
相关文件    : 
文件实现功能: 
作者        : 陆志洋
版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/5/13   1.0         陆志洋         创建
******************************************************************************/
#ifndef  _AUDCOM_API_H_
#define  _AUDCOM_API_H_
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

typedef struct
{	
	l32	(*Open) (void **ppvHandle, void *in);
	l32 (*Close)(void *pvHandle);
	l32 (*Process)(void *pvHandle, void *in, void *out, void *unionparam);
	l32 (*Control)(void *pvHandle, l32 cmdid, void *in, void *out);
	l32 (*GetVersion)(void *in, void *out);
}TAudCodecApi;

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif
