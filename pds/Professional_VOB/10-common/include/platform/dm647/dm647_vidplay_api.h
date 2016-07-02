/******************************************************************************
模块名  ： DM647_VIDPLAY_DRV
文件名  ： dm647_vidplay_api.h
相关文件： dm647_vidplay_api.c
文件实现功能：
作者    ：张方明
版本    ：1.0.0.0.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
09/11/2008  1.0         张方明      创建
******************************************************************************/
#ifndef __DM647_VIDPLAY_API_H
#define __DM647_VIDPLAY_API_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* runtime include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "dm647_drv_common.h"

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
模块版本：DM647_VIDPLAY_DRV 1.1.1.20080602
增加功能：创建
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.7.20090101
增加功能：支持HDU
修改缺陷：无
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.2.20090102
增加功能：增加1080p50/60支持；修正视频偏移
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.3.20090109
增加功能：调整VP1的DMA传输通道TC从3调到2
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.4.20090205
增加功能：增加XGA30的播放设置
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.5.20090512
增加功能：支持60/1.001 24/1.001 30/1.001帧率的视频输出
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.6.20090608
增加功能：支持播放状态统计查询功能
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.7.20090728
增加功能：按照VESA标准修正了的所有VGA分辨率的偏移参数
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.8.20090803
增加功能：按照CEA861标准修正了的所有HD/SD分辨率的偏移参数
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.9.20090904
增加功能：增加VGA60格式16位裸模式传输功能，修改了TVidInfo结构，用户需要指定裸模式
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.10.20090929
增加功能：增加播放口分配的原始Buf信息查询；修改了帧Buf结构，增加用户参数
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.11.20091017
增加功能：增加播放口buf统计接口；增加小画面大底图显示播放功能
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.12.20091028
增加功能：支持720P25/30播放
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.13.20091204
增加功能：支持外同步576i和480i
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.14.20091228
增加功能：支持SAA7121的DA芯片
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.15.20100202
增加功能：1、支持UXGA等高分辨率信号。
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.16.20100521
增加功能：无
修改缺陷：修改所有调用while可能发生死循环的代码，改为超时出错
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.17.20100608
增加功能：无
修改缺陷：修改一个while循环计数变量未初始化的问题
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_VIDPLAY_DRV 1.1.18.20101118
增加功能：增加XGA30的播放设置
修改缺陷：无
提交人：赫诚杰

****************************************************************************/
/* 版本号定义 */
#define VER_DM647_VIDPLAY_DRV         (const s8*)"DM647_VIDPLAY_DRV 1.1.18.20101118" 

/* 极限值定义 */
#define DM647_VIDPLAY_VP_MAX_NUM         5      /* 目前一个DM647上最大支持5个视频播放端口 */
#define DM647_VIDPLAY_BUF_MAX_NUM        16     /* 目前最多允许分配的BUF个数 */

/* Dm647VidPlayCtrl操作码定义 */
#define DM647_VIDPLAY_SET_BRIGHT         0      /* 亮度，  范围0x00~0xff */
#define DM647_VIDPLAY_SET_CONTRAST       1      /* 对比度，范围0x00~0xff */
#define DM647_VIDPLAY_SET_SATURATION     2      /* 饱和度，范围0x00~0xff */
#define DM647_VIDPLAY_SET_HUE            3      /* 色度，  范围0x00~0xff */
#define DM647_VIDPLAY_GET_STAT           4      /* 获取播放状态统计; pArgs为(TDm647VidPlayStat *) */
#define DM647_VIDPLAY_GET_BUF_INFO       5      /* 获取播放Buf信息; pArgs为(TDm647VidPlayBufInfo *) */

/* 类型定义 */
typedef void * HDm647VidPlayDev;


/* 视频播放IO设备创建的参数结构定义 */
typedef struct{
    u32             dwVidOutIntf;               /* 视频输出接口，见: 视频接口宏定义 */
    TVidInfo        tVidInfo;                   /* 视频信号信息，即要显示的视频制式 */
    TDM647FBufDesc *pBufDescs;                  /* 指向用户分配的FBufDesc结构变量数组的首地址 */
    u32             dwFBufNum;                  /* 要分配的FBufDesc的个数，范围：0~DM647_VIDPLAY_BUF_MAX_NUM-1 */
    u32	            dwWidthMax;                 /* 最大视频宽，像素为单位, 用于分配播放Buf */
    u32	            dwHeightMax;                /* 最大视频高，行为单位, 用于分配播放Buf
                                                   注意：由于用户会经常打开关闭播放设备，因此驱动中分配Buf时仅在第一次打开
                                                   设备时按照最大分辨率来分配Buf，后面打开设备继续沿用该Buf，这样可以避免
                                                   内存碎片过多引起分配失败问题 */
    u32             dwPicLeft;                  /* 画面相对底图左边右移n个象素，整个图像右移 !注意：必须是偶数!不能超出底图范围 */
    u32             dwPicTop;                   /* 画面相对底图顶部下移n行，整个图像下移，注意：不能超出底图范围 */
    u32             dwPicWidth;                 /* 画面在底图内显示n个象素/行，填0表示水平方向满屏 !注意：必须为16的整数倍!不能超出底图范围 */
    u32             dwPicHight;                 /* 画面在底图内显示n行，填0表示垂直方向满屏 注意：不能超出底图范围 */

    u32	            dwHScaleEn;                 /* 1=播放图像水平方向做2xScaling拉伸
                                                   0=原样显示 */
} TDm647VidPlayDevParam;

/* 视频采集IO设备状态统计结构定义 */
typedef struct{
    u32  dwSubmitCount;                         /* Total Submit count for the channel */
    u32  dwErrCount;                            /* Display underrun error count */
    u32  dwFrameCount;                          /* EDMA ISR frame count */
    u32  dwShortFldDetectCount;                 /* Short field detect count for capture driver */
    u32  dwIsrSwapCount;                        /* Current and Next viop swap count in EDMA ISR */
    u32  dwIsrSwapErrCount;                     /* Invalid state error count while swapping current & next VIOP in ISR */
    u32  dwReconfigErrCount;                    /* Invalid state error count while reconfiguring EDMA channel */
    u32  dwTooLateInISRCount[5];                /* EDMA reload and EDMA ISR race condition error count */
    u32  dwNumFramesBuffed;                     /* Number of queued buffer in waiting play Que */
    u32  dwNumBufferInDrv;                      /* Number of buffer in driver total */
} TDm647VidPlayStat;

/* 视频播放口Buf描述结构定义 */
typedef struct{
    u32             dwFBufNum;                  /* 分配的FBufDesc的个数，范围：0~DM647_VIDPLAY_BUF_MAX_NUM-1 */
    TDM647FBufDesc  atBufDescs[DM647_VIDPLAY_BUF_MAX_NUM];/* 分配的FBufDesc结构变量数组 */
} TDm647VidPlayBufInfo;


/*==============================================================================
    函数名      : Dm647VidPlayOpen
    功能        ：视频播放IO设备打开
    输入参数说明：dwVpId: 0~DM647_VIDPLAY_VP_MAX_NUM-1;
                          !注意：每个VP同时只能配置为采集或播放设备，不能同时打开!
                          且一旦打开不能关闭后更改为采集设备!
                  dwChnl: 固定为0, 考虑扩展性和兼容性
                  ptParam: 打开的参数
                  phHandle: 存放设备句柄的变量指针
    返回值说明  ：错误返回DM647_DRV_FAILURE或错误码；成功返回DM647_DRV_SUCCESS并
                  返回控制句柄
------------------------------------------------------------------------------*/
l32 Dm647VidPlayOpen(u32 dwVpId, u32 dwChnl, TDm647VidPlayDevParam *ptParam, HDm647VidPlayDev *phHandle);

/*==============================================================================
    函数名      : Dm647VidPlayClose
    功能        ：视频播放IO设备关闭。
    输入参数说明：hVidPlayDev: Dm647VidPlayOpen函数返回的句柄;
    返回值说明  ：错误返回DM647_DRV_FAILURE或错误码；成功返回DM647_DRV_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm647VidPlayClose(HDm647VidPlayDev hVidPlayDev);

/*==============================================================================
    函数名      : Dm647VidPlayFBufGet
    功能        ：取一个视频播放BUF，用户可以多次调用取多个BUF。
    输入参数说明：hVidPlayDev: 调用Dm647VidPlayOpen函数返回的句柄;
                  pBufDesc: 用户分配并传入指针，驱动将BUF信息拷贝给用户
                  nTimeoutMs: -1=wait forever; 0=no wait;其他正值为超时毫秒数
    返回值说明  ：错误返回DM647_DRV_FAILURE或错误码；成功返回DM647_DRV_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm647VidPlayFBufGet(HDm647VidPlayDev hVidPlayDev, TDM647FBufDesc *pBufDesc, l32 nTimeoutMs);

/*==============================================================================
    函数名      : Dm647VidPlayFBufPut
    功能        ：归还一个视频BUF并加到播放队列中进行播放
    输入参数说明：hVidPlayDev: 调用Dm647VidPlayOpen函数返回的句柄;
                  pBufDesc: 用户调用Dm647VidPlayFBufGet得到的BufDesc信息
    返回值说明  ：错误返回DM647_DRV_FAILURE或错误码；成功返回DM647_DRV_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm647VidPlayFBufPut(HDm647VidPlayDev hVidPlayDev, TDM647FBufDesc *pBufDesc);

/*==============================================================================
    函数名      : Dm647VidPlayCtrl
    功能        ：视频播放IO设备控制，目前定义了
                    DM647_VIDPLAY_SET_BRIGHT:       pArgs为u8 *
                    DM647_VIDPLAY_SET_CONTRAST:     pArgs为u8 *
                    DM647_VIDPLAY_SET_SATURATION:   pArgs为u8 *
                    DM647_VIDPLAY_SET_HUE:          pArgs为u8 *
                  ......
    输入参数说明：hVidPlayDev: 调用Dm647VidPlayOpen函数返回的句柄;
                 nCmd: 操作码；pArgs: 参数指针
    返回值说明  ：错误返回DM647_DRV_FAILURE或错误码；成功返回DM647_DRV_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm647VidPlayCtrl(HDm647VidPlayDev hVidPlayDev, l32 nCmd, void *pArgs);

/*====================================================================
    函数名      : Dm647VidPlayGetVer
    功能        ：模块版本号查询。
    输入参数说明：pchVer： 给定的存放版本信息的buf指针
                  dwBufLen：给定buf的长度
    返回值说明  ：版本的实际字符串长度。小于0为出错;
                 如果实际字符串长度大于dwBufLen，赋值为0
--------------------------------------------------------------------*/
l32 Dm647VidPlayGetVer(s8 *pchVer, u32 dwBufLen);


#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif /* __DM647_VIDPLAY_API_H */
