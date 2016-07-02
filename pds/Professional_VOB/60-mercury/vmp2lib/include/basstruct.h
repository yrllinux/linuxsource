/*****************************************************************************
   模块名      : 码流适配器
   文件名      : basinst.h
   创建时间    : 2003年 6月 25日
   实现功能    : 适配宏定义和通道结构定义头文件
   作者        : 张明义
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/03/21  4.0         张宝卿      代码优化
******************************************************************************/ 
#ifndef _BASSTRUCT_H_
#define _BASSTRUCT_H_

#include "mcuconst.h"
#include "mcustruct.h"

// 适配组、通道类型
#define TYPE_AUDIO                  (u8)1           // 音频类型
#define TYPE_VIDEO                  (u8)2           // 视频类型
#define LENGTH_TYPE                 (u8)64          // 音视频通道类型长度

#define MAX_CHANNEL_NUM_GROUP       (u8)3           // 一个适配组最大通道号
#define MAX_CHANNELS_BAS            (u8)15          // Bas中最大通道数

#define CHECK_IFRAME_INTERVAL       (u16)1000       // 检测关键帧时间间隔(ms)
#define DBG_PRINT                   printf
#define FIRST_REGACK                (u8)1           // 第一次收到注册Ack
#define BAS_CONNETC_TIMEOUT         (u16)3*1000     // 连接超时值3s
#define BAS_REGISTER_TIMEOUT        (u16)5*1000     // 注册超时值5s

#define LOCALBINDSTARTPORT          (u16)10300
#define BAS_PRI                     (u16)60

typedef struct tagChnInfo
{
    tagChnInfo()
    {
        m_dwLastFUPTick = 0;
    }

    CConfId         m_cChnConfId;
    u32             m_dwLastFUPTick;    //最后收到关键帧请求的Tick数

    THDBasVidChnStatus  m_tChnStatus;
    THDBasDVidChnStatus m_tChnStatusD;
}TChannelInfo;


#endif // _BASSTRUCT_H_
