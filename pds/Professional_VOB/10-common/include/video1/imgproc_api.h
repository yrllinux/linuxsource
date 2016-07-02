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
#ifndef  _IMGPROC_API_H_
#define  _IMGPROC_API_H_

#ifdef __cplusplus
extern "C"
{
#endif

enum VidImgProcCmdId
{
    IMGPROC_SET_PARAMS = 401,
    IMGPROC_GET_STATUS,
    IMGPROC_ANALYZE_ERRCODE,
    IMGPROC_DEBUG		
};

typedef struct
{
    void *pvImgParam;   //图像处理参数结构指针
	u32	u32ImgProcType; //图像处理类型
	u32 u32Reserved;    // 保留参数
}TImgProcParam;

l32 ImgProcOpen(void **ppvHandle, void *in);
l32 ImgProcClose(void *pvHandle);
l32 ImgProcProcess(void *pvHandle, void *in, void *out, void *unionparam);
l32 ImgProcControl(void *pvHandle, l32 cmdid, void *in, void *out);
l32 ImgProcGetVersion(void *in, void *out);

extern TVidCodecApi tImgProc;

#ifdef __cplusplus
}
#endif

#endif
