/******************************************************************************
模块名  ： DM647_AUDIO_DRV
文件名  ： dm647_audio_api.h
相关文件： dm647_audio_api.c
文件实现功能：
作者    ：张方明
版本    ：1.0.0.0.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
08/15/2007  1.0         张方明      创建
04/22/2008  1.1         张方明      修改类型定义头文件，修改ioctl类型宏定义
******************************************************************************/
#ifndef __DM647_AUDIO_API_H
#define __DM647_AUDIO_API_H

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
模块版本：DM647_AUDIO_DRV 1.1.1.200090203
增加功能：创建
修改缺陷：无
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM647_AUDIO_DRV 1.3.1.200090703
增加功能：1. buff管理及EDMA使用修改
          2. 加入获得mcasp出错信息的方法
修改缺陷：无
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM647_AUDIO_DRV 1.3.2.200091009
增加功能：1. 音频播放时钟模式不同的自动配置
          2. 根据不同设备配置不同的音频通道
修改缺陷：无
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM647_AUDIO_DRV 1.3.3.200091010
增加功能：音频采集驱动修改
修改缺陷：无
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM647_AUDIO_DRV 1.3.4.200091203
增加功能：音频open时加入设置channel的配置
修改缺陷：无
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM647_AUDIO_DRV 1.3.5.200091230
增加功能：更新了KDV7820的配置
修改缺陷：无
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM647_AUDIO_DRV 1.3.6.20100107
增加功能：无
修改缺陷：修正了关闭音频采集导致播放无法工作的问题
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM647_AUDIO_DRV 1.3.7.20100111
增加功能：无
修改缺陷：修正了关闭音频后打开不同ser出现不工作的问题
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM647_AUDIO_DRV 1.3.8.20100329
增加功能：无
修改缺陷：修正了按非128字节分配buffer内存导致出错的问题
提交人：赫诚杰
----------------------------------------------------------------------------
模块版本：DM647_AUDIO_DRV 1.3.9.20100520
增加功能：无
修改缺陷：修正了无音频时钟时打开音频采集死循环问题
提交人：张方明
----------------------------------------------------------------------------
模块版本：DM647_AUDIO_DRV 1.3.10.20100521
增加功能：修改策略当无音频时钟时打开音频采集返回失败
修改缺陷：解决打开音频采集播放设备失败后没有清打开标志导致以后再也打不开设备
提交人：张方明

****************************************************************************/

/* 版本号定义 */
#define VER_DM647_AUDIO_DRV         (const s8*)"DM647_AUDIO_DRV 1.3.10.20100521" 

/* 板级驱动模块返回值定义  */
#define DM647_AUD_SUCCESS                0                  /* 成功 */

#define DM647_AUD_ERR_BASE               0                  /* 错误码基值 */
#define DM647_AUD_DEV_NOT_EXIST          (DM647_AUD_SUCCESS+1)  /* 设备不存在 */
#define DM647_AUD_DEV_NOT_OPEN           (DM647_AUD_SUCCESS+2)  /* 设备没有打开 */
#define DM647_AUD_PARAM_ERR              (DM647_AUD_SUCCESS+3)  /* 参数非法 */
#define DM647_AUD_LENGTH_ERR             (DM647_AUD_SUCCESS+4)  /* 信息长度错误 */
#define DM647_AUD_QUEUE_FULL             (DM647_AUD_SUCCESS+5)  /* 数据队列已满 */
#define DM647_AUD_NOT_SUPPORT            (DM647_AUD_SUCCESS+6)  /* 设备不支持该操作 */
#define DM647_AUD_OPT_TIMEOUT            (DM647_AUD_SUCCESS+7)  /* 操作超时 */
#define DM647_AUD_NO_MEM                 (DM647_AUD_SUCCESS+8)  /* 没有可用内存 */
#define DM647_AUD_DEV_BUSY               (DM647_AUD_SUCCESS+9)  /* 设备忙，已经被打开 */
#define DM647_AUD_GIO_FAILE              (DM647_AUD_SUCCESS+10) /* GIO操作失败 */
#define DM647_AUD_FAILURE                -1                 /* 未知的异常失败 */

/* 极限值定义 */
#define DM647_AUD_DEV_MAX_NUM             1              /* 目前一个DM647上最大支持1路音频芯片 */
#define DM647_AUD_BUF_MAX_NUM             32             /* 目前最多允许分配的BUF个数 */

/* 音频设备打开模式定义  */
#define DM647_AUD_IOM_INPUT               0              /* 输入模式，即采集 */
#define DM647_AUD_IOM_OUTPUT              1              /* 输出模式，即播放 */

/* Dm647AudCtrl操作码定义 */
#define DM647_AUD_GET_RX_STAT             0              /* 音频接收统计状态查询 */
#define DM647_AUD_GET_TX_STAT             1              /* 音频播放统计状态查询 */

/* 类型定义 */
typedef void * HDm647AudDev;

/* 物理口定义 */
#define AUD_SER0    (1 << 0)
#define AUD_SER1    (1 << 1)
#define AUD_SER2    (1 << 2)
#define AUD_SER3    (1 << 3)
#define AUD_SER4    (1 << 4)
#define AUD_SER5    (1 << 5)
#define AUD_SER6    (1 << 6)
#define AUD_SER7    (1 << 7)
#define AUD_SER8    (1 << 8)
#define AUD_SER9    (1 << 9)

/* 音频IO设备创建的参数结构定义 */
typedef struct{
    u32   dwBytesPerSample;      /* 一个样本的字节数：1 2 4 */
    u32   dwSamplePerFrame;      /* 一帧的样本个数 */
    u32   dwChanNum;             /* 声道个数，hdu填4（4表示两路2声道，若同时播放3路则填6），1帧的字节数=dwBytesPerSample*dwSamplePerFrame*dwChanNum */
    u32   dwFrameNum;            /* 缓存Frame的个数，范围：2~DM647_AUD_BUF_MAX_NUM-1 */
    u32   dwSampleFreq;          /* 8000，48000，96000，192000Hz ... */
    TFBufDesc *pBufDescs;        /* 指向用户分配的FBufDesc结构变量数组的首地址，用户可以自己分配数据Buf，将指针传递给驱动
                                    数组个数为dwFrameNum, 注意：音频需要128字节边界对齐；
                                    对于不想自己分配Buf的用户填为NULL时即可，驱动会按照前面的参数自动分配BUF */
    u32   dwChannelSet;          /* 填0表示默认设置 其他具体见Dm647AudOpen和接口文档说明 */
} TDm647AudDevParam;

/* 音频采集统计状态结构定义，对应操作码：DM647_AUD_GET_RX_STAT */
typedef struct{
    u32   dwBytesInDrv;          /* 驱动中缓存的采集到的音频数据字节数，回声抵消时要用 */
    u32   dwLostBytes;           /* 对于采集表示丢弃的字节数，一般在没有可用buf时发生，回声抵消时要用 */
    u32   dwDmaErr;              /* dma出错的次数 */
    u32   dwRunErr;              /* Overrun出错的次数 */
    u32   dwSyncErr;             /* 帧同步出错的次数 */
    u32   dwPingPongErr;         /* ping-pong反转出错的次数 */
    u32   dwPingPongErrLost;     /* ping-pong反转出错的次数 (Lost状态) */
    u32   dwPingPongErrDebug;    /* ping-pong反转出错的次数 (Debug状态) */
    u32   adwReserved[6];        /* reserved */
} TDm647AudRxStat;

/* 音频播放状态结构定义，对应操作码：DM647_AUD_GET_TX_STAT */
typedef struct{
    u32   dwBytesInDrv;          /* 驱动中缓存的待播放的音频数据字节数，回声抵消时要用
                                    (注意：hdu是两路数据的和，即要获得每路数据俄话要除以2） */
    u32   dwMuteBytes;           /* 对于播放表示播放静音的字节数，一般在没有音频数据时发生，回声抵消时要用 */
    u32   dwDmaErr;              /* dma出错的次数 */
    u32   dwRunErr;              /* Underrun出错的次数 */
    u32   dwSyncErr;             /* 帧同步出错的次数 */
    u32   dwPingPongErr;         /* ping-pong反转出错的次数 */
    u32   dwPingPongErrMute;     /* ping-pong反转出错的次数 (Mute状态) */
    u32   dwPingPongErrDebug;    /* ping-pong反转出错的次数 (Debug状态) */
    u32   adwReserved[6];        /* reserved */
} TDm647AudTxStat;


/*==============================================================================
 *  函数名      : Dm647AudOpen
 *  功能        ：音频IO设备打开，1个dwDevId可以打开2次，分别为INPUT/OUPUT
 *  输入参数说明：dwDevId: 0~DM647_AUD_DEV_MAX_NUM-1;
 *               nMode: DM647_AUD_IOM_INPUT/DM647_AUD_IOM_OUTPUT
 *               ptParam: 打开的参数
 *               dwBoard:相应的设备
 *  返回值说明  ：错误返回NULL；成功返回控制句柄
 *
 *  dwBytesPerSample    一个样本的字节数：1 2 4 (目前只用4)
 *  dwSamplePerFrame    一帧的样本个数
 *  dwChanNum           对应音频采集或播放路数乘以2
 *
 *  TDm647AudDevParam中 dwChannelSet说明
 *
 *  dwChannelSet是32位，填0表示默认配置 非0时为自定义 从下面表中取出不同的宏或表示采集或播放的路数
 *
 *  在buffer中 AUD_SER0 到 AUD_SERn 路从左到右分布，长度为buffer的n分之1, 无法设置 AUD_SERn 在buffer中的位置
 *
 *                  物理口                  对应宏
 *  HDU:
 *      播放        AOUT1                   AUD_SER0
 *                  AOUT2                   AUD_SER1
 *
 *  KDV7820:
 *      dsp0采集    HDMI                    AUD_SER0 (采样频率不固定)
 *      dsp0播放    适配后的音频播放        AUD_SER1 (固定48K播放)
 *      dsp2采集    模拟音频                AUD_SER0
 *                  dsp0适配后的音频        AUD_SER1
 *                  数字MIC1                AUD_SER2
 *                  数字MIC2                AUD_SER3
 *      dsp2播放    音频输出                AUD_SER4
 *                  扬声器                AUD_SER5
 *
 *  默认配置(dwChannelSet填0时):
 *  HDU:
 *      播放: AUD_SER0 | AUD_SER1                           (dwChanNum对应4)
 *
 *  KDV7820:
 *      dsp0采集 AUD_SER0                                   (dwChanNum对应2)
 *      dsp0播放 AUD_SER1                                   (dwChanNum对应2)
 *      dsp2采集 AUD_SER0 | AUD_SER1 | AUD_SER2 | AUD_SER3  (dwChanNum对应8)
 *      dsp2播放 AUD_SER4 | AUD_SER5                        (dwChanNum对应4)
------------------------------------------------------------------------------*/
HDm647AudDev Dm647AudOpen(u32 dwDevId, l32 nMode, TDm647AudDevParam *ptParam);

/*==============================================================================
    函数名      : Dm647AudClose
    功能        ：音频IO设备关闭。
    输入参数说明：hAudDev: Dm647AudOpen函数返回的句柄;
    返回值说明  ：错误返回DM647_AUD_FAILURE或错误码；成功返回DM647_AUD_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm647AudClose(HDm647AudDev hAudDev);

/*==============================================================================
    函数名      : Dm647AudRead
    功能        ：音频IO设备读数据，读取长度必须是dwBytesPerSample*dwChanNum的整数倍。
    输入参数说明：hAudDev: 以DM647_AUD_IOM_INPUT模式调用Dm647AudOpen函数返回的句柄;
                 pBuf: 指向用户分配的Buf，用来存放采集的音频数据
                 size：要读取的数据字节数
                 nTimeoutMs: -1=wait forever; 0=no wait;其他正值为超时毫秒数
    返回值说明  ：错误返回DM647_AUD_FAILURE；超时返回0；成功返回读到的字节数(=size)
------------------------------------------------------------------------------*/
l32 Dm647AudRead(HDm647AudDev hAudDev, void *pBuf, size_t size, l32 nTimeoutMs);

/*==============================================================================
    函数名      : Dm647AudWrite
    功能        ：向音频IO设备写数据，数据字节数必须是dwBytesPerSample*dwChanNum的整数倍
    输入参数说明：hAudDev: 以DM647_AUD_IOM_OUTPUT模式调用Dm647AudOpen函数返回的句柄;
                 pData: 指向用户存放待播放的音频数据
                 size：要播放的数据字节数
                 nTimeoutMs: -1=wait forever; 0=no wait;其他正值为超时毫秒数
    返回值说明  ：错误返回DM647_AUD_FAILURE；超时返回0；成功返回写入的字节数(=size)
------------------------------------------------------------------------------*/
l32 Dm647AudWrite(HDm647AudDev hAudDev, void *pData, size_t size, l32 nTimeoutMs);

/*==============================================================================
    函数名      : Dm647AudFBufGet
    功能        ：取一帧音频BUF，对于采集用户拿到的是音频数据Buf，对于播放用户
                  拿到的是空Buf，用户可以多次调用取多个BUF。
                  # 如果用户使用read write的话不要使用该函数!!!
    输入参数说明：hAudDev: 调用Dm647AudOpen函数返回的句柄;
                  ptBufDesc: 用户分配并传入指针，驱动将BUF信息拷贝给用户
                  nTimeoutMs: -1=wait forever; 0=no wait;其他正值为超时毫秒数
    返回值说明  ：错误返回DM647_AUD_FAILURE或错误码；成功返回DM647_AUD_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm647AudFBufGet(HDm647AudDev hAudDev, TFBufDesc *ptBufDesc, l32 nTimeoutMs);

/*==============================================================================
    函数名      : Dm647AudFBufPut
    功能        ：归还一帧音频BUF
                  # 如果用户使用read write的安灰褂酶煤?!!
    输入参数说明：hAudDev: 调用Dm647AudOpen函数返回的句柄;
                  ptBufDesc: 用户调用Dm647AudFBufGet得到的BufDesc信息
    返回值说明  ：错误返回DM647_AUD_FAILURE或错误码；成功返回DM647_AUD_SUCCESS
------------------------------------------------------------------------------*/
l32 Dm647AudFBufPut(HDm647AudDev hAudDev, TFBufDesc *ptBufDesc);

/*====================================================================
    函数名      : Dm647AudCtrl
    功能        ：音频IO设备控制，目前没用，以后扩展用
                  ......
    输入参数说明：hAudDev: 调用Dm647AudOpen函数返回的句柄;
                 nCmd: 操作码；pArgs: 参数指针
    返回值说明  ：错误返回DM647_AUD_FAILURE或错误码；成功返回DM647_AUD_SUCCESS
---------------------------------------------------------------------*/
l32 Dm647AudCtrl(HDm647AudDev hAudDev, l32 nCmd, void *pArgs);

/*====================================================================
    函数名      : Dm647AudGetVer
    功能        ：模块版本号查询。
    输入参数说明：pchVer： 给定的存放版本信息的buf指针
                  dwBufLen：给定buf的长度
    返回值说明  ：版本的实际字符串长度。小于0为出错;
                 如果实际字符串长度大于dwBufLen，赋值为0
--------------------------------------------------------------------*/
l32 Dm647AudGetVer(s8 *pchVer, u32 dwBufLen);


#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif /* __DM647_AUDIO_API_H */
