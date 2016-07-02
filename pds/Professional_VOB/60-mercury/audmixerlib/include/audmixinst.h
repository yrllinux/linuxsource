/*****************************************************************************
   模块名      : 混音器
   文件名      : MixerInst.h
   创建时间    : 2003年 12月 9日
   实现功能    : 
   作者        : 张明义
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   03/12/09    3.0         zmy         创建
******************************************************************************/
#ifndef _AUDIO_MIXER_INSTANCE_H_
#define _AUDIO_MIXER_INSTANCE_H_

#include "osp.h"
#include "kdvsys.h"
#include "kdvmixer.h"
#include "mcustruct.h"
#include "eqpcfg.h"

/*
#if( MAXNUM_MIXER_DEPTH < MAX_MIXER_DEPTH  )
#error( "MAXNUM_MIXER_DEPTH > MAX_MIXER_DEPTH" )
#endif*/

#define FIRST_REGACK                    (u8 )1           // 第一次收到注册Ack
#define MIX_CONNETC_TIMEOUT             (u16)(3 * 1000)  // 连接超时值3s
#define MIX_REGISTER_TIMEOUT            (u16)(4 * 1000)  // 注册超时值4s
#define MIXER_PRI                       (u16)60
#define MIXER_LOCALSTARTPORT            (u16)10000
#define MIXER_LOCALRCVSTARTPORT         (u16)20000

class CAudMixInst : public CInstance
{
public:
    CAudMixInst();
    ~CAudMixInst();
    
	//////////////////////////////////////////////////////////////////////////
	///////   消息处理函数
	//////////////////////////////////////////////////////////////////////////
	
    //	<< 消息入口 >>
    void InstanceEntry( CMessage * const pcMsg );
    void DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp );

    // << 在DAEMON 实例中调用函数 >>
	void DaemonProcInitalMsg( CMessage * const pcMsg, CApp* pcApp ); // 处理初始化(Daemon)
    void ProcConnectMcuTimeOut( BOOL32 bIsConnectA );                // 处理连接入口
    void ProcRegisterTimeOut( BOOL32 bIsRegisterA );                 // 处理注册入口
    void MsgMixerQuit( CMessage * const pcMsg );                     // 处理混音退出
    void MsgDisconnectProc( CMessage * const pcMsg, CApp* pcApp );   // 处理断链
    void MsgRegAckProc( CMessage * const pcMsg, CApp* pcApp );       // 处理注册成功
    void MsgRegNackProc( CMessage * const pcMsg );                   // 处理注册失败
    void DaemonProcGetMsStatusRsp( CMessage * const pcMsg, CApp* pcApp ); // 处理取主备倒换状态
	//test, zgc, 2007/04/24
	void DaemonProcStopEqpCmd(CApp* pcApp ); //停止混音器的MAP
	void DaemonProcStartEqpCmd( CMessage * const pcMsg, CApp* pcApp ); //启动混音器的MAP
	
	void DaemonProcReconnectBCmd( CMessage * const pcMsg); // Mcu 通知外设重连 MPCB 板, zgc, 2007/04/30
	void DeamonProcModSendIpAddr( CMessage* const pMsg );

    // << 在普通是实例中调用函数>>
    void MsgAddMixMemberProc( CMessage * const pcMsg );
    void MsgRemoveMixMemberPorc( CMessage * const pcMsg );

    void MsgStartMixProc( CMessage * const pcMsg );           // 处理开始混音消息
    void MsgStopMixProc( CMessage * const pcMsg );            // 处理停止混音消息
    void MsgSetVolumeProc( CMessage * const pcMsg );          // 设置音量
    void MsgChangeMixDelay( CMessage * const pcMsg );         // 修改混音延时
    void MsgForceActiveProc( CMessage * const pcMsg );        // 强制混音
    void MsgCancelForceActiveProc( CMessage * const pcMsg );  // 取消强制混音
    void MsgActiveMmbChangeProc( CMessage * const pcMsg );
    void MsgSetMixDepthProc( CMessage * const pcMsg );        // 设置混音深度
    void MsgSetMixChannelProc();      // 设置混音通道
    void MsgSetMixSendProc( CMessage * const pcMsg );
    void MsgSetVacKeepTimeProc( CMessage * const pcMsg );     // 设置语音激励
    
    void MsgGroupShowProc();                                  // 显示具体混音通道信息
    void MsgGrpChnShowProc();                                 // 显示具体的混音通道和终端对应信息

	///////////////////////////////////////////////////////////
	////////// 功能函数
	///////////////////////////////////////////////////////////
	BOOL32 Init(CApp* pcApp );	      // 初始化
    BOOL32 InitMixGroup();                                    // 初始化混音组
    BOOL32 DestroyeMixGroup();                                // 将混音组释放掉
	BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );  // 连接Mcu
	void   Register( BOOL32 bIsRegisterA, u32& dwMcuNode );   // 向MCU注册
    BOOL32 StartMix( u8 byAudType, u8 byDepth, u8 byAll,      // 开始混音
                     TMediaEncrypt * ptMediaEncrypt, 
                     TDoublePayload * ptDoublePayload, u8 byFECMode = FECTYPE_NONE );
    BOOL32 StopMix();                                         // 停止混音
	void   SendGrpNotif();                                    // 发送混音组状态
    BOOL32 SendMsgToMcu( u16 wEvent, CServMsg const &cServMsg );
    BOOL32 SendMsgToMcuA( u16 wEvent, CServMsg const &cServMsg );
    BOOL32 SendMsgToMcuB( u16 wEvent, CServMsg const &cServMsg );
    void   SetEncryptParam( TMediaEncrypt * ptMediaEncrypt, TDoublePayload * ptDoublePayload );
    void   ProcSetQosInfo( CMessage * const pcMsg );          // 设置Qos信息
    void   ComplexQos( u8& byValue, u8 byPrior );             // 求复合Qos值
	void   ShowAllMixGroupStatus( CApp* pcApp );              // 显示混音状态
    void   ShowAllMixGrpChnStatus( CApp* pcApp );             // 显示通道状态
	void   StopAllMixGroup( CApp* pcApp );                    // 停止所有混音组
	BOOL32 ReadConfigFileApu( u32& dwStartRcvBufs, u32& dwFastRcvBufs );

    // xsl [7/29/2006] 混音通道与mtid的映射处理函数
    void SetMtIdPosInChnIdx( u8 byMtId, u8 byChanIdx, BOOL32 bRemove = FALSE );
	void ClearMapTable( void );
	
	void ProcModSndAddr();
    
protected:
    //DAEMON 实例状态
    enum ESTATE
    {
        IDLE,
        NORMAL,
    };
    
protected:
    s32              m_nMaxMmbNum;          //该混音组中允许最大混音通道数
    CKdvMixerGroup   m_cMixGrp;             //混音组实例
    CConfId          m_cConfId;             //混音组服务的会议ID
    u16              m_wLocalRcvPort;       //本地接收起始端口号
    TMixerGrpStatus  m_tGrpStatus;          //混音组状态
    BOOL32           m_bNeedPrs;
    u8		         m_abyMtId2ChnIdx[MAX_CHANNEL_NUM+1];
};

class CMixerCfg
{
public:
    CMixerCfg();
    ~CMixerCfg();
public:
    u32     m_dwMcuIId;             //通信MCU.A 实例ID
    u32     m_dwMcuIIdB;            //通信MCU.B 实例ID
    u32     m_dwMcuNode;            //与MCU.A 通信节点号
    u32     m_dwMcuNodeB;           //与MCU.B 通信节点号
    u32     m_dwMcuRcvIp;           //MCU.A 接收地址
    u32     m_dwMcuRcvIpB;          //MCU.B 接收地址
    u16     m_wMcuRcvStartPort;     //MCU.A 接收起始端口号
    u16     m_wMcuRcvStartPortB;    //MCU.B 接收起始端口号
    BOOL32  m_bEmbed;               //是否本机MCU.A
    BOOL32  m_bEmbedB;              //是否内嵌在B板上MCU.B
    u8      m_byRegAckNum;          //第一次收到注册ACK
	u16		m_wMTUSize;				//MTU size, zgc, 2007-04-02
	BOOL32  m_bIsSendRedundancy;	//是否冗余发送, zgc, 2007-07-25

	u32     m_dwMpcSSrc;            // guzh [6/12/2007] 业务侧会话校验值
    
    TAudioMixerCfg m_tMixerCfg;     //MIXER的配置信息
    TNetRSParam    m_tNetRSParam;

    
public:
    void FreeStatusDataA( void );
    void FreeStatusDataB( void );	
};

typedef zTemplate< CAudMixInst, MAXNUM_MIXER_GROUP, CMixerCfg > CMixerApp;
extern CMixerApp g_cMixerApp;

#endif  //_AUDIO_MIXER_INSTANCE_H_
