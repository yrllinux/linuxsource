/*****************************************************************************
模块名      : 
文件名      : imgproc_api.h
相关文件    : 
文件实现功能: 
作者        : 王建
版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/4/18   1.0          王建         创建
******************************************************************************/
#ifndef  _VIDCOM_API_H_
#define  _VIDCOM_API_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{	
	l32	(*Open) (void **ppvHandle, void *in);
	l32 (*Close)(void *pvHandle);
	l32 (*Process)(void *pvHandle, void *in, void *out, void *unionparam);
	l32 (*Control)(void *pvHandle, l32 cmdid, void *in, void *out);
	l32 (*GetVerSion)(void *in, void *out);
}TVidCodecApi;

#ifdef __cplusplus
}
#endif

#endif
