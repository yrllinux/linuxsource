/*****************************************************************************
   模块名      : 高清解码卡
   文件名      : hdustruct.h
   创建时间    : 2008.12.03
   实现功能    : HDU宏定义和通道结构定义头文件
   作者        : 江乐斌
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2008/12/03  4.6         江乐斌       创建
******************************************************************************/ 
#ifndef _HDUSTRUCT_H_
#define _HDUSTRUCT_H_

#define CHECK_IFRAME_INTERVAL       (u16)100       // 检测关键帧时间间隔(ms)
#define DBG_PRINT                   printf
#define FIRST_REGISTERACK           (u8)1           // 第一次收到注册Ack
#define HDU_CONNETC_TIMEOUT         (u16)3*1000     // 连接超时值3s
#define HDU_REGISTER_TIMEOUT        (u16)5*1000     // 注册超时值5s

struct TChannelInfo
{
public:
    TChannelInfo()
    {
        m_dwLastFUPTick = 0;
        m_dwLastFUPTick = 0;
		m_tHduChnStatus.SetVolume( HDU_VOLUME_DEFAULT );
		m_tHduChnStatus.SetIsMute( FALSE );
		m_tHduChnStatus.SetSchemeIdx( 0 );
		m_tHduChnStatus.SetStatus( 0 );
		m_tHduChnStatus.SetNull();
		m_tHduChnStatus.SetChnIdx( 0 );
	}
public:
    CConfId         m_cChnConfId;
    u32             m_dwLastFUPTick;    //最后收到关键帧请求的Tick数

	THduChnStatus   m_tHduChnStatus;    //通道状态
};

#endif //_HDUSTRUCT_H_

