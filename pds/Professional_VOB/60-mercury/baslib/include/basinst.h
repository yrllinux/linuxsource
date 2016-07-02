/*****************************************************************************
   模块名      : 码流适配器
   文件名      : basinst.h
   创建时间    : 2003年 6月 25日
   实现功能    : 适配实例头文件
   作者        : 张明义
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/03/21  4.0         张宝卿      代码优化
******************************************************************************/
#ifndef _BASINST_H_
#define _BASINST_H_

#include "osp.h"
#include "mcuconst.h"
#include "basstruct.h"
#include "mcustruct.h"
#include "kdvadapter.h"
#include "eqpcfg.h"
#ifdef _LINUX_
#include "libsswrap.h"
#endif

const u16 EnVidFmt[6][12]=
{
    //类型               压缩码率控制参数 最大关键帧 最大量化 最小量化 编码比特率K 图像制式 采集端口 帧率       图像质量         视频高度 视频宽度
    //mpeg4
    MEDIA_TYPE_MP4,           0,            75,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //261
    MEDIA_TYPE_H261,          0,            75,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //262(mpeg-2)
    MEDIA_TYPE_H262,          0,            75,        31,      2,       2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //263
    MEDIA_TYPE_H263,          1,            75,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //h263+
    MEDIA_TYPE_H263PLUS,      0,            300,       31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
    //h264
    MEDIA_TYPE_H264,          0,            300,       51,      2,       2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720
};

class CBasInst : public CInstance
{
protected:
    enum ESTATE
    {
        IDLE,
        NORMAL,
    };
protected:
    u8  m_byAudioSN;                // 用于计算音频通道端口号
    u8  m_byVideoSN;                // 用于计算视频通道端口号

    u8  m_byNextChnIdx;             // use in inital 
    u8  m_byAudioChnNum;            // total audio channel
    u8  m_byNextAudioChnIdx;
    u8  m_byAudioStartChnIdx;       // audio channel start index
    u8  m_byAudioGrpNum;            // 音频适配组数( 目前只支持一个音频适配组)
    u8  m_byAudioGrpStartIdx;       // audio start adp group 
    u8  m_byNextAudioGrpIdx;
    
    u8  m_byVideoChnNum;            // total video channel 
    u8  m_byVideoStartChnIdx;
    u8  m_byNextVideoChnIdx;
    u8  m_byVideoGrpNum;            // 视频频适配组数
    u8  m_byVideoGrpStartIdx;       // video start adp group 
    u8  m_byNextVideoGrpIdx;

    u8  m_byAllGrpNum;              // total adp group num
    u8  m_byAllChnNum;              // total channel group num

    TChannelInfo    m_atChnInfo[MAX_CHANNELS_BAS];
    TAdpGrpInfo     m_atAdpInfo[MAXNUM_BASGRP];

public:
    CBasInst(  );
    ~CBasInst(  );

    // ----------- 消息入口 ------------
    void InstanceEntry( CMessage* const pMsg );
    void DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp );
    
    // --------- 消息处理函数 ----------
	void  DaemonProcInit( CMessage* const pMsg, CApp* pcApp );    //初始化
    void  ProcConnectTimeOut( BOOL32 bIsConnectA );               //连接总入口
    void  ProcRegisterTimeOut( BOOL32 bIsRegiterA );              //注册总入口
    void  MsgDisconnectProc( CMessage* const pMsg, CApp* pcApp ); //断链处理
    void  MsgRegAckProc( CMessage* const pMsg, CApp* pcApp );     //处理注册成功
    void  MsgRegNackProc( CMessage* const pMsg );                 //处理注册失败
	void  DeamonProcGetMsStatusRsp( CMessage* const pMsg );       //处理取Mpc板主备倒换状态

    void  SendChnNotif( u8 byChnIdx );                            //发送通道状态给Mcu
    void  MsgStartAdptProc( u8 byChnIdx, CMessage* const pMsg );  //开始适配
    void  MsgAdjustAdptProc( u8 byChnIdx, CMessage* const pMsg ); //调整适配参数
    void  MsgStopAdptProc( u8 byChnIdx, CMessage* const pMsg );   //停止适配
	void  MsgFastUpdatePicProc( u8 byChnIdx );                    //Mcu请求Bas编关键帧
    void  MsgTimerNeedIFrameProc( void );                         //Bas请求Mcu发关键帧
    void  ProcSetQosMsg( CMessage* const pMsg );                  //设置Qos值

	void  DaemonProcReconnectBCmd( CMessage* const pMsg, CApp* pcApp ); //MCU 通知会话 重联MPCB 相应实例, zgc, 2007/04/30


	// ----------- 功能函数 ------------
    s32  AddChannel( u8 byChnIdx, TAdaptParam* ptParm );          //增加通道
	s32  Init( CMessage* const pMsg, CApp* pcApp );	              //初始化
	BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );      //具体连接函数
	void Register( BOOL32 bIsRegiterA, u32 dwMcuNode );           //向MCU注册
    u16  AddAudioChannel(  u8 byChnIdx, TAdaptParam& tParm  );    //增加音频通道
    u16  AddVideoChannel(  u8 byChnIdx, TAdaptParam& tParm  );    //增加视频通道
	s32  SendMsgToMcu( u16 wEvent, CServMsg const &cServMsg );
        
    u8   GetGrpNoFromChnIdx( u8 byChnIdx );                           //根据通道得到它所在的适配组
    void ComplexQos( u8& byValue, u8 byPrior );                       //组合Qos值
	void ClearInstStatus( void );                                     //清空本实例保存的状态信息
	
	/*lint -save -sem(CBasInst::InitalData,initializer)*/
    void InitalData( void );                          //处理内部数据的初始化
	void ProcEmbed( void );
	u8   ProcFreeConfChns( u8 byChnIdx, CConfId&  cConfId );    
	s8*  ipStr( u32 dwIP );	
	void BasInfo( void );                             //调试接口
    void StatusShow( CApp* pcApp );                   //状态显示
    BOOL32 GetMediaType( u8 byType, s8* aszTypeStr ); //取媒体类型
    
	// 设置编码参数
    void SetEncryptParam( u8 byChnIdx, TMediaEncrypt*  ptMediaEncryptVideo,
                          TDoublePayload* ptDoublePayloadVideo,
                          TMediaEncrypt*  ptMediaEncryptAudio,
                          TDoublePayload* ptDoublePayloadAudio,
                          u8 byVidEncryptPT, u8 byAudEncryptPT,
						  u8 byRealEncodePT);

    // 平滑发送支持
    void SetSmoothSendRule(u32 dwDestIp, u16 wPort, u32 dwRate);
    void ClearSmoothSendRule(u32 dwDestIp, u16 wPort);
    void StopSmoothSend();
};

#define MIN_BITRATE_OUTPUT      (u16)128

struct TBasDebugVal
{
protected:
    BOOL32          m_bEnableBitrateCheat;   // 是否启用码率输出作弊
    u16             m_wDefault;              // 缺省下调的百分比
public:
    void   SetEnableBitrateCheat(BOOL32 bEnable)
    {
        m_bEnableBitrateCheat = bEnable;
    }
    BOOL32 IsEnableBitrateCheat() const
    {
        return m_bEnableBitrateCheat;
    }

    u16    GetDecRateDebug(u16 wOriginRate) const
    {
        // guzh [10/19/2007] 没有则返回缺省值
        wOriginRate = wOriginRate * m_wDefault / 100;

        if (wOriginRate > MIN_BITRATE_OUTPUT)
            return wOriginRate;
        else
            return MIN_BITRATE_OUTPUT;
    }
    void   SetDefaultRate(u16 wPctRate)
    {
        m_wDefault = wPctRate;
    }

};
class CBasCfg
{
public:
    CBasCfg(  );
    virtual ~CBasCfg(  );
public:
    u32              m_dwMcuIId;                 //通信MCU.A表示
    u32              m_dwMcuIIdB;                //通信MCU.B表示
    u32              m_dwMcuNode;                //与MCU.A通信节点号
    u32              m_dwMcuNodeB;               //与MCU.B通信节点号
    u32              m_dwMcuRcvIp;               //MCU.A 接收地址
    u32              m_dwMcuRcvIpB;              //MCU.B 接收地址
    u16              m_wMcuRcvStartPort;         //MCU.A 接收起始端口号
    u16              m_wMcuRcvStartPortB;        //MCU.A 接收起始端口号
    BOOL32           m_bEmbedMcu;                //是否内嵌在MCU.A中
    BOOL32           m_bEmbedMcuB;               //是否内嵌在MCU.B中
    u8               m_byChnNum;                 //通道个数
    u8               m_byRegAckNum;              //第一次注册成功
	u32              m_dwMpcSSrc;                // guzh [6/12/2007] 业务侧会话校验值 
	u16				 m_wMTUSize;				 //MTU大小, zgc, 2007-04-02
    TEqpCfg          m_tCfg;                     //适配器配置
    TEqp             m_tEqp;                     //本设备
    TPrsTimeSpan     m_tPrsTimeSpan;             //重传时间跨度
    CKdvAdapterGroup m_cAptGrp[MAXNUM_BASGRP+1]; //一个组为RTP头重组用
	
    
    TBasDebugVal     m_tDebugVal;                //作弊值

	// 是否打开平滑发送的开关, zgc, 2008-03-03
	BOOL32 m_bIsUseSmoothSend;
public:
    void FreeStatusDataA( void );
    void FreeStatusDataB( void );

    void ReadDebugValues();
    BOOL32 IsEnableCheat() const
    {
        return m_tDebugVal.IsEnableBitrateCheat();
    }
    u16 GetDecRateDebug(u16 wOriginRate) const
    {
        return m_tDebugVal.GetDecRateDebug(wOriginRate);
    }

    void GetDefaultParam(u8 byEnctype,TVideoEncParam& TEncparam);
    void GetDefaultDecParam( u8 mediatype, TVideoDecParam& tParam );  //取默认解码参数
};

void baslog( s8* pszFmt, ...);

typedef zTemplate< CBasInst , MAXNUM_BAS_CHNNL, CBasCfg > CBasApp;
extern CBasApp g_cBasApp;

#endif //!_BASINST_H_
