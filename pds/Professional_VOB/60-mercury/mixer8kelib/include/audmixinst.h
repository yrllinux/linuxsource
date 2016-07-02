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
#include "mcustruct.h"
#include "eqpcfg.h"
#include "mmpcommon.h"
#if defined(_8KH_) || defined(_8KE_)
#include "audiomixer.h"
#else
#include "audiomixer_gpu.h"
#endif
#include "commonlib.h"
#include "mcuinnerstruct.h"

#if( MAXNUM_MIXER_DEPTH < MAX_MIXER_DEPTH  )
#error( "MAXNUM_MIXER_DEPTH > MAX_MIXER_DEPTH" )
#endif
#include "kdvlog.h"
#include "loguserdef.h"

#define FIRST_REGACK                    (u8 )1           // 第一次收到注册Ack
#define MIX_CONNETC_TIMEOUT             (u16)(3 * 1000)  // 连接超时值3s
#define MIX_REGISTER_TIMEOUT            (u16)(4 * 1000)  // 注册超时值4s

#if defined(_8KE_)
#define MAXNUM_8000X_MIXER              (u8)1
#elif defined(_8KH_)
#define MAXNUM_8000X_MIXER              (u8)2
#else
#define MAXNUM_8000X_MIXER              (u8)5
#endif

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
	//上电处理
	void DaemonProcPowerOnMsg(CMessage * const pcMsg); // 处理初始化
	//建联超时
	void   DaemonProcConnetTimerOut();
	//注册超时
	void   DaemonProcRegTimerOut();
	//注册成功
	void   DaemonProcRegAck(CMessage * const pcMsg, CApp* pcApp);
	//注册失败
	void   DaemonProcRegNack();
	//断链处理
	void   DaemonProcOspDisconnect();
	void   DaemonProcMixerStatus();                    // 显示混音器状态信息
	void   ProcMsgMixerInit(CMessage * const pcMsg);
    void ProcRegisterTimeOut( void );                                // 处理注册入口
    void MsgDisconnectProc();                                        // 普通实例处理断链
    void MsgRegAckProc( CMessage * const pcMsg);                     // 处理注册成功
    void MsgRegNackProc( CMessage * const pcMsg );                   // 处理注册失败

    // << 在普通是实例中调用函数>>
    void MsgAddMixMemberProc( CMessage * const pcMsg );
    void MsgRemoveMixMemberPorc( CMessage * const pcMsg );

    void MsgStartMixProc( CMessage * const pcMsg );           // 处理开始混音消息
    void MsgStopMixProc( CMessage * const pcMsg );            // 处理停止混音消息
    void MsgSetVolumeProc();          // 设置音量
    void MsgChangeMixDelay( CMessage * const pcMsg );         // 修改混音延时
    void MsgForceActiveProc( CMessage * const pcMsg );        // 强制混音
    void MsgCancelForceActiveProc( CMessage * const pcMsg );  // 取消强制混音
    void MsgActiveMmbChangeProc( CMessage * const pcMsg );    // 激活成员变化
    void MsgSetMixDepthProc();        // 设置混音深度
    void MsgSetMixChannelProc( CMessage * const pcMsg );      // 设置混音通道
    void MsgSetMixSendProc( CMessage * const pcMsg );         // 设置混音器是否发送码流
    void MsgSetVacKeepTimeProc( CMessage * const pcMsg );     // 设置语音激励
    void MsgGroupShowProc();                                  // 显示具体混音通道信息

	///////////////////////////////////////////////////////////
	////////// 功能函数
	///////////////////////////////////////////////////////////
    BOOL32 DestroyeMixGroup();                                // 将混音组释放掉
	BOOL32 ConnectMcu( u32& dwMcuNode );                      // 连接Mcu
	void   Register();                        // 向MCU注册
    BOOL32 StartMix( TMediaEncrypt * ptMediaEncrypt, 
                     TDoublePayload * ptDoublePayload,u8 byNChlNum = 0,TAudioCapInfo *ptAudioCapInfo = NULL );      // 开始混音
    BOOL32 StopMix();                                         // 停止混音
	void   SendGrpNotif();                                    // 发送混音组状态
    BOOL32 SendMsgToMcu( u16 wEvent, CServMsg const &cServMsg );
    void   SetEncryptParam( TMediaEncrypt * ptMediaEncrypt, TDoublePayload * ptDoublePayload,u8 byNModeChlNum,TAudioCapInfo *ptAudioCapInfo );

    // xsl [7/29/2006] 混音通道与mtid的映射处理函数
    void SetMtIdPosInChnIdx( u8 byMtId, u8 byChanIdx, BOOL32 bRemove = FALSE );
	void ClearMapTable( void );
#if !defined(_8KH_) && !defined(_8KE_)
	u8 GetAudioMode(const u8 byAudioType, u8 byAudioTrackNum);
#endif
    
protected:  
private:
    CAudioMixer      *m_pcAMixer;            //混音组实例
    CConfId          m_cConfId;              //混音组服务的会议ID
    TMixerGrpStatus  m_tGrpStatus;           //混音组状态
    BOOL32           m_bNeedPrs;
    u8		         m_abyMtId2ChnIdx[MAXNUM_MIXER_CHNNL + 1];
	u32              m_dwMcuIId;             //通信MCU.A 实例ID
    u8               m_byRegAckNum;          //第一次收到注册ACK
	u16		         m_wMTUSize;			 //MTU size, zgc, 2007-04-02
	BOOL32           m_bIsSendRedundancy;	 //是否冗余发送, zgc, 2007-07-25
    TNetRSParam      m_tNetRSParam;
	T8keAudioMixerCfg m_tMixerCfg;            //MIXER的配置信息
	u32              m_dwMcuNode;
};


typedef zTemplate< CAudMixInst, MAXNUM_8000X_MIXER > CMixerApp;
extern CMixerApp g_cMixerApp;

#endif  //_AUDIO_MIXER_INSTANCE_H_
