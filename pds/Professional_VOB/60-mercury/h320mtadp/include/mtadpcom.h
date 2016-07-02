/*****************************************************************************
模块名      : h320终端适配模块
文件名      : MtAdpcom.h
相关文件    : MtAdpcom.cpp
文件实现功能: 公共模块
作者        : 许世林
版本        : V3.6  Copyright(C) 1997-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005/06/07  3.6         许世林      创建
******************************************************************************/

#ifndef __MTADPCOM_H
#define __MTADPCOM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mcustruct.h"
#include "mcuconst.h"
#include "kdvh320def.h"
#include "kdvh320.h"


//////////////////////////////////////////////////////////////////////////
//                              宏定义
//////////////////////////////////////////////////////////////////////////

//码流收发起始端口号(临时，应该从dri板获取)
#define SWITCH_STARTPORT            30000

//呼叫超时时间(s)
#define CALLING_TIMEOUT             2

//开始远遥超时时间
#define STARTACTION_TIMEOUT         0

//E1通道最大个数
#define MAXNUM_CHAN                 4
//音频接收信道索引
#define AUD_RCV_INDEX               0
//音频发送信道索引
#define AUD_SND_INDEX               1
//视频接收信道索引
#define VID_RCV_INDEX               2
//视频发送信道索引
#define VID_SND_INDEX               3

//每个终端占用端口个数
#define PORTNUM_PERMT               8
//音频媒体接收端口(与起始端口间隔，以下同)
#define AUD_RCV_PORT                0
//音频媒体控制接收端口
#define AUD_RCV_CTRLPORT            1
//音频媒体发送端口
#define AUD_SND_PORT                2
//音频媒体控制发送端口
#define AUD_SND_CTRLPORT            3
//视频媒体接收端口
#define VID_RCV_PORT                4
//视频媒体控制接收端口
#define VID_RCV_CTRLPORT            5
//视频媒体发送端口
#define VID_SND_PORT                6
//视频媒体控制发送端口
#define VID_SND_CTRLPORT            7

//打印级别
#define LOG_CRITICAL                0
#define LOG_WARNING                 1
#define LOG_INFO                    2
#define LOG_DEBUG                   3

#define  DEF_VID_NETBAND            8000*1024   //缺省的视频位率
#define  DEF_AUD_NETBAND            200*1024    //缺省的音频位率
#define  DEF_VID_FRAMESIZE          1024*128 
#define  DEF_AUD_FRAMESIZE          1024*5     
#define  DEF_FRAME_RATE	            25

//默认远遥令牌释放超时时间
#define  DEF_LSDTOKEN_RELEASE_TIME  10

//def mcu id
#define  DEF_SMCUID                 2

//rtp切包处理后码流长度
#define  BUFLEN_RTPPACK             128*1024   

//信道状态
enum CHAN_STATE
{
    CHAN_IDLE = 0,
    CHAN_OPENING,
    CHAN_CONNECTED
};

#define SAFE_DELETE(p)              \
    if(NULL != p)                   \
    {                               \
        delete p;                   \
        p = NULL;                   \
    }


//////////////////////////////////////////////////////////////////////////
//                          数据结构定义
//////////////////////////////////////////////////////////////////////////

//逻辑信道结构
typedef struct tagChannel
{
    u8                  m_byState;          //信道状态
    TLogicalChannel     m_tLogicChan;       //信道结构
    TTransportAddr	    m_tSndMediaChannel;	//发送端媒体信道(用于net send初始化)

public:
    tagChannel(void)
    {         
        Reset();
    }

    ~tagChannel(void)
    {
    }

    void Reset(void)
    {
        m_byState = CHAN_IDLE;
        memset(&m_tLogicChan, 0, sizeof(m_tLogicChan));
        memset(&m_tSndMediaChannel, 0, sizeof(m_tSndMediaChannel));
        m_tLogicChan.m_tMediaEncrypt.Reset(); 
        m_tLogicChan.m_byChannelType = MEDIA_TYPE_NULL; 
    }

}TChannel;

//semaphore
class CEnter
{
public:
    CEnter(SEMHANDLE semSnd)
    {        
        OspSemTake(semSnd);
        m_semSnd = semSnd;
    }
    
    ~CEnter()
    {
        OspSemGive(m_semSnd);
    }
    
private:
    SEMHANDLE m_semSnd;
};

#define ENTER(sem)  CEnter cEnter(sem);

//////////////////////////////////////////////////////////////////////////
//                          全局接口定义
//////////////////////////////////////////////////////////////////////////

/*=============================================================================
  函 数 名： CapSetIn2Out
  功    能： 将上层会议能力集转换为协议栈能力集类型 
  参    数： TCapSupport &tCapSupport [in] 会议能力集
             u32 dwTransCap           [in] 传输率能力
             TCapSetEx &tCapSet       [out]协议栈能力集
  返 回 值： void 
=============================================================================*/
void CapSetIn2Out(TCapSupport &tCapSupport, u32 dwTransCap, TKdvH320CapSet &tCapSet);

/*=============================================================================
  函 数 名： MediaTypeIn2Out
  功    能： 将上层会议媒体类型转换为协议栈媒体类型 
  参    数： u8 byMediaType [in] 会议媒体类型
  返 回 值： u8            [out]协议栈媒体类型
=============================================================================*/
u8  MediaTypeIn2Out(u8 byMediaType);

/*=============================================================================
  函 数 名： MediaTypeIn2Out
  功    能： 将协议栈媒体类型转换为上层会议媒体类型 
  参    数： u8 byMediaType [in] 协议栈媒体类型
  返 回 值： u8             [out] 会议媒体类型
=============================================================================*/
u8  MediaTypeOut2In(u8 dwMediaType);

/*=============================================================================
  函 数 名： FeccActionIn2Out
  功    能： 将上层远遥类型转换为协议栈远遥类型 
  参    数： u8 byParam     [in] mcu远遥参数
  返 回 值： u8             [out]协议栈远遥参数
=============================================================================*/
u8  FeccActionIn2Out(u8 byParam);

/*=============================================================================
  函 数 名： FeccActionOut2In
  功    能： 将协议栈远遥类型转换为上层远遥类型
  参    数： u8 byAction     [in] 协议栈远遥参数
  返 回 值： u8             [out] mcu远遥参数
=============================================================================*/
u8  FeccActionOut2In(u8 byAction);

/*=============================================================================
  函 数 名： StackType2Cap
  功    能： 将协议栈类型转换为能力集 
  参    数： u8 byMediaType      [in] 类型
             u8 byResolution     [in] 分辨率
  返 回 值： u32            [out]能力集
=============================================================================*/
u32 StackType2Cap(u8 byMediaType, u8 byResolution = 0);

/*=============================================================================
  函 数 名： MuxMode2Cap
  功    能： 由复用模式转换为上层能力集格式  
  参    数： TKDVH320MuxMode &tMuxMode      [in] 复用模式
             TCapSupport &tCap              [out]能力集
  返 回 值： void 
=============================================================================*/
void MuxMode2Cap(TKDVH320MuxMode &tMuxMode, TKdvH320CapSet &tLocalCapSet, TCapSupport &tCap);

/*=============================================================================
  函 数 名： IsEqualMuxMode
  功    能： 判断两复用模式是否相等 
  参    数： TKDVH320MuxMode &tMuxMode1     [in]
             TKDVH320MuxMode &tMuxMode2     [in]
  返 回 值： BOOL32     (true - 相等，false - 不等)
=============================================================================*/
BOOL32 IsEqualMuxMode(const TKDVH320MuxMode &tMuxMode1, const TKDVH320MuxMode &tMuxMode2);

/*=============================================================================
  函 数 名： GetAudioBitrate
  功    能： 得到音频的编码比特率 
  参    数： u8 byAudioType     [in] 音频类型
  返 回 值： u16                [out]音频的编码比特率
=============================================================================*/
u16 GetAudioBitrate(u8 byAudioType);

/*=============================================================================
  函 数 名： GetTsMaskAndTransFromBitrate
  功    能： 从会议码率获得时隙掩码和传输率
  算法实现： 
  全局变量： 
  参    数： u16 wBitrate       [in] 会议码率
             u32 &dwTsMask      [out]时隙掩码
             u32 &dwTransCap    [out]传输率
  返 回 值： BOOL32         (TRUE-成功，FALSE-失败)
=============================================================================*/
BOOL32 GetTsMaskAndTransFromBitrate(u16 &wBitrate, u32 &dwTsMask, u32 &dwTransCap);

/*=============================================================================
  函 数 名： StrIPv4
  功    能： 将ip地址转换为字符串 
  参    数： u32 dwIP (net order)
  返 回 值： s8 * 
=============================================================================*/
s8 * StrIPv4(u32 dwIP);

#endif //__MTADPCOM_H
