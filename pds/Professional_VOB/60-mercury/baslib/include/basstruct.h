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

typedef struct tagChnInfo{
    enum ChnType
	{
        AudioChn = 1,
        VideoChn
	}tChnType;

    u8              byChnType;
    BOOL32          bIsUsed;            // 是否被使用
    CConfId         m_cChnConfId;
    u32             m_dwLastFUPTick;    //最后收到关键帧请求的Tick数
    TBasChnStatus   m_tChnStatus;
}TChannelInfo;

typedef struct tagAdpGrpInfo{

    tagAdpGrpInfo()
    {
        bIsUsed = FALSE;
        m_wLocalBindPort = 0;
        m_wLocalRcvPort = 0;
        byGrpIdx        = 0;
        byCanUseChnNum  = 0;
        byAudioOrVideoGrp = 0;
        memset(abyChnInfo, 240, sizeof(abyChnInfo) );
    }
    
    enum EGRPTYPE
    {
        AUDIO = 1,
        VIDEO
    }tGrpType;
    
    u8             byGrpType;
    BOOL32         bIsUsed;                    // 是否被使用         
    u16            m_wLocalBindPort;           // 本地绑定起始端口号    
    u16            m_wLocalRcvPort;            // 本地接收起始端口号
    u8             byGrpIdx;                   // 适配组序号
    u8             byCanUseChnNum;             // 该适配组总的通道数
    u8             byNextChnNum;               // 该适配组下一可用通道号
    u8             byAudioOrVideoGrp;          // 适配组类型，1：音频，2：视频
    s32            abyChnInfo[MAX_CHANNEL_NUM_GROUP];   // 该适配组的通道号

    u8 AddOneChannel( u8 byChnNo )
    {
        if(byCanUseChnNum < MAX_CHANNEL_NUM_GROUP)
        {
            abyChnInfo[byCanUseChnNum] = byChnNo;
        }
        byCanUseChnNum++;
        return 0;
    }

}TAdpGrpInfo;

#endif // _BASSTRUCT_H_
