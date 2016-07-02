/******************************************************************************
模块名  ： DM6437_RESIZER_DRV
文件名  ： dm6437_resizer_api.h
相关文件： dm6437_resizer_api.c
文件实现功能：
作者    ：张方明
版本    ：1.0.0.0.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
04/22/2008  1.0         张方明      创建
******************************************************************************/
#ifndef __DM6437_RESIZER_API_H
#define __DM6437_RESIZER_API_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* runtime include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "dm6437_drv_common.h"

/****************************** 模块的版本号命名规定 *************************
总的结构：mn.mm.ii.cc.tttt
     如  Osp 1.1.7.20040318 表示
模块名称Osp
模块1版本
接口1版本
实现7版本
04年3月18号提交

版本修改记录：
----------------------------------------------------------------------------
模块版本：DM6437_RESIZER_DRV 1.1.1.20080602
增加功能：创建
修改缺陷：无
提交人：张方明

****************************************************************************/
/* 版本号定义 */
#define VER_DM6437_RESIZER_DRV         (const s8*)"DM6437_RESIZER_DRV 1.1.1.20080602" 


/* 极限值定义 */
#define DM6437_RESIZER_NUM_CHANNELS       (16u)     /* 一个DM6437上最大可以创建16个RESIZER通道,
                                                       每个通道支持一种resize模式 */

/* Dm6437ResizerCtrl操作码定义 */

/* 类型定义 */
typedef void * HDm6437ResizerChnl;


/* 视频RESIZER设备通道创建的参数结构定义 */
typedef struct{
    u16	    wWidthIn;                       /* 输入图像宽，像素为单位 */
    u16	    wHeightIn;                      /* 输入图像高，行为单位 */
    u16	    wWidthOut;                      /* 输出图像宽，像素为单位 */
    u16	    wHeightOut;                     /* 输出图像高，行为单位 */
} TDm6437ResizerChnlParam;

/* 视频RESIZER处理参数结构定义 */
typedef struct{
    void    *pInBuf;                        /* 输入图像的Buf指针,必须是128Byte对齐，要求图像数据格式为16位YUV422的UYVY图像 */
    u32	    dwInBufSize;                    /* 输入图像的Buf的大小 */
    BOOL    bInUseCache;                    /* 为TRUE表示输入的Buf带cache，驱动里面会进行刷cache处理；
                                               为FALSE为不带cache的，驱动不做刷cache处理 */

    void    *pOutBuf;                       /* 输出图像的Buf指针,必须是128Byte对齐 */
    u32	    dwOutBufSize;                   /* 输出图像的Buf的大小 */
    BOOL    bOutUseCache;                   /* 为TRUE表示输出的Buf带cache，驱动里面会进行刷cache处理；
                                               为FALSE为不带cache的，驱动不做刷cache处理 */
} TDm6437ResizeParam;


/*==============================================================================
    函数名      : Dm6437ResizerChnlOpen
    功能        ：视频RESIZER设备通道打开
    输入参数说明：dwChnlId: 0~DM6437_RESIZER_NUM_CHANNELS-1
                  ptParam: 打开的参数
    返回值说明  ：错误返回NULL；成功返回控制句柄
------------------------------------------------------------------------------*/
HDm6437ResizerChnl Dm6437ResizerChnlOpen(u32 dwChnlId, TDm6437ResizerChnlParam *ptParam);

/*==============================================================================
    函数名      : Dm6437ResizerChnlClose
    功能        ：视频RESIZER设备通道关闭。
    输入参数说明：hResizerChnl: Dm6437ResizerOpen函数返回的句柄;
    返回值说明  ：错误返回DM6437_DRV_FAILURE或错误码；成功返回DM6437_DRV_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm6437ResizerChnlClose(HDm6437ResizerChnl hResizerChnl);

/*==============================================================================
    函数名      : Dm6437ResizerProcess
    功能        ：取一个视频RESIZER BUF，用户可以多次调用取多个BUF。
    输入参数说明：hResizerChnl: 调用Dm6437ResizerOpen函数返回的句柄;
                  ptParam: 用户分配并传入指针，驱动将BUF信息拷贝给用户
    返回值说明  ：错误返回DM6437_DRV_FAILURE或错误码；成功返回DM6437_DRV_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm6437ResizerProcess(HDm6437ResizerChnl hResizerChnl, TDm6437ResizeParam *ptParam);

/*====================================================================
    函数名      : Dm6437ResizerGetVer
    功能        ：模块版本号查询。
    输入参数说明：pchVer： 给定的存放版本信息的buf指针
                  dwBufLen：给定buf的长度
    返回值说明  ：版本的实际字符串长度。小于0为出错;
                 如果实际字符串长度大于dwBufLen，赋值为0
--------------------------------------------------------------------*/
l32 Dm6437ResizerGetVer(s8 *pchVer, u32 dwBufLen);

#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif /* __DM6437_RESIZER_API_H */
