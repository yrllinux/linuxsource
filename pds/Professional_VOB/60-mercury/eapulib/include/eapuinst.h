/*****************************************************************************
  模块名      : libeapu.a
  文件名      : eapuinst.h
  相关文件    : kdvmixer_eapu.h
  文件实现功能: 
  作者        : 田志勇
  版本        : V1.0  Copyright(C) 2009-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2010/01/20  1.0         田志勇        创建
******************************************************************************/

#include "osp.h"
#include "kdvmedianet.h"   //网络支持
#include "eqpcfg.h"        //外设支持
#include "mcustruct.h"     //CServer等支持
#include "mcuver.h"
#include "kdvmixer_eapu.h"
#include "eveapumixer.h"
#include "boardagent.h"
#define MIXER_EAPU_CHN_MIXNUM           (u16)2
#define FIRST_REGACK                    (u8 )1              // 第一次收到注册Ack
#define MIX_CONNETC_TIMEOUT             (u16)(3 * 1000)     // 连接超时值3s
#define MIX_REGISTER_TIMEOUT            (u16)(4 * 1000)     // 注册超时值4s
#define MAX_AUDIO_FRAME_SIZE            (u32)8 * 1024       //接受大小
#define BRD_EAPU_SNDPORT                (u16)10000          //
#define ERR_MIXER_NONE               ( ERR_MIXER_BGN  )     //!无错误 
#define ERR_MIXER_GRPNOTEXIST        ( ERR_MIXER_BGN + 1 )  //!指定的混音组不存在
#define ERR_MIXER_MMBNOTEXIST        ( ERR_MIXER_BGN + 2 )  //!指定的混音成员不存在
#define ERR_MIXER_NOFREEGRP          ( ERR_MIXER_BGN + 3 )  //!没有空闲的混音组
#define ERR_MIXER_NOFREEMMB          ( ERR_MIXER_BGN + 4 )  //!没有空闲的混音通道
#define ERR_MIXER_ILLEGALBODY        ( ERR_MIXER_BGN + 5 )  //!消息中的消息体非法
#define ERR_MIXER_CALLDEVICE         ( ERR_MIXER_BGN + 6 )  //!调用底层函数失败
#define ERR_MIXER_MIXING             ( ERR_MIXER_BGN + 7 )  //!正在混音状态
#define ERR_MIXER_NOTMIXING          ( ERR_MIXER_BGN + 8 )  //!未混音
#define ERR_MIXER_ERRCONFID          ( ERR_MIXER_BGN + 9 )  //!无效的会议ID
#define ERR_MIXER_GRPINVALID         ( ERR_MIXER_BGN + 10 ) //无效的混音组ID
#define ERR_MIXER_CHNINVALID         ( ERR_MIXER_BGN + 11 ) //无效的混音组ID
#define ERR_MIXER_STATUIDLE          ( ERR_MIXER_BGN + 12 )

#define ERR_MIXER_MIXMODEAUTOMIX     ( ERR_MIXER_BGN + 13 )  //!正在智能混音模式
#define ERR_MIXER_MIXMODESPECMIX     ( ERR_MIXER_BGN + 14 )  //!正定制混音模式
#define ERR_MIXER_MIXMODEVAC         ( ERR_MIXER_BGN + 15 )  //!正在语音激励模式
#define ERR_MIXER_MIXMODEAUTOANDVAL  ( ERR_MIXER_BGN + 16 )  //!正在语音激励与会议讨论同时开启模式
#define MAXVALUE_EMIXER_VOLUME       (u8)31          //EAPU混音器最大音量值
//打印级别定义
#define MIXLOG_LEVER_CRIT                (u8)0       //关键点/错误/异常
#define MIXLOG_LEVER_WARN                (u8)1       //警告
#define MIXLOG_LEVER_INFO                (u8)2       //流程
#define MIXLOG_LEVER_RCVDATA             (u8)3       //接收数据
#define MIXLOG_LEVER_SNDDATA             (u8)4       //发送数据
#define MIXLOG_LEVER_DETAIL              (u8)5       //流程细节/详细调试点

//R3FULL平台支持EAPU模拟APU
#define DEVVER_MIXER_R3FULL              (u16)4301
class CEapuMixInst;
class CRcvCallBack
{
public:
    CEapuMixInst* m_pcMixerInst;    //回调的混音器
    u32           m_byChnnlId;      //回调的通道号 
	CRcvCallBack()
	{
		m_pcMixerInst = NULL;
		m_byChnnlId = 0;
	}
};
struct CRegAckInfo
{
public:
    u32     m_dwMcuIId;             //通信MCU.A 实例ID
    u32     m_dwMcuIIdB;            //通信MCU.B 实例ID
    u8      m_byRegAckNum;          //第一次收到注册ACK
    u32     m_dwMpcSSrc;            //业务侧会话校验值
    TPrsTimeSpan m_tPrsTimeSpan;
/*    void Reset()
    {
        m_dwMcuIId       = INVALID_INS;
        m_dwMcuIIdB      = INVALID_INS;
        m_dwMpcSSrc      = 0;
    }*/
    CRegAckInfo()
    {
        m_dwMcuIId       = INVALID_INS;
        m_dwMcuIIdB      = INVALID_INS;
        m_dwMpcSSrc      = 0;
        m_byRegAckNum    = 0;
    }
    void print()
    {
        OspPrintf(TRUE,FALSE,"RegAckInfo:McuIId[%d]McuIIdB[%d]RegAckNum[%d]MpcSSrc[%d]",
			m_dwMcuIId,m_dwMcuIIdB,m_byRegAckNum,m_dwMpcSSrc);
    }
};
class CEapuMixInst : public CInstance
{
public:
    CEapuMixInst();
    ~CEapuMixInst();
    CKdvMixer m_cMixer;                                          //混音器对象，底层提供
    //消息入口
    void DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp );
    void InstanceEntry( CMessage * const pcMsg );
    CKdvMediaSnd* GetMediaSndPtr(u32 dwChID, BOOL32 bDMode);         //获得某通道对应的发送对象
    u8     GetAudioMode(u8 byAudioType , BOOL32 bIsSingleChn = FALSE);
    //消息处理函数
private:
    //混音器初始化,消息体为TEapuMixerCfg
    void DaemonProcInitalMsg(CMessage* const pMsg);     //处理初始化消息(Daemon)
    void DaemonProcGetMsStatusRsp( CMessage * const pcMsg); // 处理取主备倒换状态
    void ProcGetMsStatusRsp( CMessage * const pcMsg); // 处理取主备倒换状态
    void MsgProcInitalMsg(CMessage* const pMsg);                     //生成混音器
    void MsgAddMixMemberProc(CMessage* const pMsg);                  //向混音器中添加混音成员，即增加混音通道
    void MsgRemoveMixMemberPorc(CMessage *const pMsg);               //从混音器中移除混音成员，即清除混音通道
    void MsgStartMixProc(CMessage* const pMsg);                      //处理启动混音器消息
    void MsgStopMixProc(CMessage* const pMsg);                       //处理停止混音器消息
    void MsgForceActiveProc( CMessage * const pcMsg );               // 强制混音
    void MsgCancelForceActiveProc( CMessage * const pcMsg );         // 取消强制混音
    void MsgSetMixDepthProc(CMessage *const pMsg);                   //设置混音器深度
	void MsgStartVacProc(CMessage *const pMsg);
	void MsgStopVacProc(CMessage *const pMsg);
    void MsgSetVolumeProc(CMessage* const pMsg);                     //设置混音成员音量
    void MsgSetVacKeepTimeProc( CMessage * const pcMsg );            // 设置语音激励
    void ProcConnectMcuTimeOut( BOOL32 bIsConnectA );                // 处理连接入口
    void ProcRegisterTimeOut( BOOL32 bIsRegisterA );                 // 处理注册入口
    void ProcSetQosInfo(CMessage* const pMsg);
    void DaemonMsgDisconnectProc( CMessage * const pcMsg);           // 管理实例处理断链
    void MsgDisconnectProc();                 // 普通实例处理断链
    void MsgRegAckProc( CMessage * const pcMsg);                     // 处理注册成功
    void MsgRegNackProc( CMessage * const pcMsg );                   // 处理注册失败
    void MsgActiveMmbChangeProc(CMessage * const pMsg);
    void MsgSetMixSendProc( CMessage * const pcMsg );
	void MsgModifySendAddr(CMessage * const pcMsg);


    void   StopAllTask();
    void   SendGrpNotif();
    BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );         // 连接Mcu
    void   Register(u32& dwMcuNode );          // 向MCU注册
    BOOL32 SendMsgToMcu( u16 wEvent, CServMsg const &cServMsg );    //向主备MCU发送信息
    BOOL32 SendMsgToMcuA( u16 wEvent, CServMsg const &cServMsg);    //向主备MCU发送信息
    BOOL32 SendMsgToMcuB( u16 wEvent, CServMsg const &cServMsg );    //向主备MCU发送信息
    void   ComplexQos(u8& byValue, u8 byPrior);
    u16    SetRcvObjectParaAndStartRcv(u8 byChnIdx);
    u16    SetSndObjectPara(u32 dwDstIp,u16 wDstPort,u8 byChnIdx);
    u16    FreeAllRcv();
    void   ShowAllMixGroupStatus( CApp* pcApp );              // 显示混音状态
    void   ShowGrpStatus();
    BOOL32 IsSupportAudiotype(u8 byAudioType);
    u8   GetMtId2ChnIdx( u8 byMtId, BOOL32 bAddMt = TRUE );

private:
    //DAEMON 实例状态
    enum ESTATE
    {
        IDLE = 0 ,
        NORMAL,
    };
    u8               m_byMixMemNum;
    u8               m_byRcvOff;                             //接收偏移
    u8               m_bySndOff;                             //发送偏移
    BOOL32           m_bIsDMode;                             //该混音器是否为多格式混音器
    CConfId          m_cConfId;                              //混音器服务的会议ID
    TMixerGrpStatus  m_tGrpStatus;                           //混音器状态
    u8               m_byAudType[2];                         //记录该会议使用的主副音频格式
    u8               m_abyMtId2ChnIdx[MAXNUM_MIXER_CHNNL];//
    BOOL32           m_bNeedPrs;
    TMediaEncrypt    m_tMediaEncrypt;
    TDoublePayload   m_tDoublePayload;
    TCapSupportEx    m_tCapSupportEx;
    BOOL32           m_bIsSendRedundancy;    
	BOOL32           m_bIsStartVac;
	BOOL32           m_bIsStartMix;
    TRSParam         m_tNetRSParam;                                                 //接受端网络重传参数
	CRegAckInfo      m_tRegAckInfo;
    u32              m_dwMcuRcvIp;           //MCU.A 接收地址
    u32              m_dwMcuRcvIpB;          //MCU.B 接收地址
    u16              m_wMcuRcvStartPort;     //MCU.A 接收起始端口号
    u16              m_wMcuRcvStartPortB;    //MCU.B 接收起始端口号
	TEapuMixerCfg    m_tEapuMixerCfg;
};
class CEapuCfg
{
public:
    CEapuCfg();
    virtual ~CEapuCfg();
public:
    u32              m_dwMcuNode;
    u32              m_dwMcuNodeB;
    BOOL32           m_bEmbed;               //是否本机MCU.A
    BOOL32           m_bEmbedB;              //是否内嵌在B板上MCU.B
    TEapuCfg         m_tEapuCfg;                    //MIXER的配置信息
    CRcvCallBack     m_acRcvCB[MAXNUM_MIXER_CHNNL];                          //接收回调
    CKdvMediaRcv     m_acAudRcv[MAXNUM_MIXER_CHNNL];                             //通道对应媒体接受码流
    BOOL32           m_bIsChnUsed[MAXNUM_MIXER_CHNNL];                           //该通道是否使用
    CKdvMediaSnd     m_acAudSnd[MAXNUM_MIXER_CHNNL + 2 * MAXNUM_MIXER_AUDTYPE];   //通道对应媒体发送码流
};
typedef zTemplate< CEapuMixInst, MAXNUM_EAPU_MIXER, CEapuCfg > CEapuMixerApp;
extern CEapuMixerApp g_cEapuMixerApp;
