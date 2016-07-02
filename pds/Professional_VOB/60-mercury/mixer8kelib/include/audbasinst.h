/*****************************************************************************
  模块名      : mixer8kelib
  文件名      : audbasinst.h
  相关文件    : 
  文件实现功能: 8ki音频适配
  作者        : 周晶晶
  版本        : 1.0
-----------------------------------------------------------------------------*/
#ifndef _8KIAUDBASINST_H_
#define _8KIAUDBASINST_H_

#ifdef _8KI_
#include "mcustruct.h"
#include "eqpcfg.h"
#include "mmpcommon.h"
#include "audiomixer_gpu.h"
#include "commonlib.h"
#include "vccommon.h"
#include "mcuinnerstruct.h"
#include "boardagentbasic.h"


#define MAX_VIDEO_FRAME_SIZE (512 * 1024)

#define MAX_AUDIO_FRAME_SIZE (8 * 1024)

#define MAXNUM_8KI_AUD_BAS MAXNUM_8KI_AUD_BASCHN
#define MAXNUM_8KIAUDBAS_ENCNUM MAXNUM_8KIAUDBAS_OUTPUT
#define BAS_CONNETC_TIMEOUT         (u16)3*1000  //connect time
#define BAS_REGISTER_TIMEOUT        (u16)6*1000  //register time

OSPEVENT( EV_BAS_TIMER,                EV_BAS_BGN );                //内部定时器
OSPEVENT( EV_BAS_NEEDIFRAME_TIMER,     EV_BAS_BGN + 1 );            //关键帧定时器
OSPEVENT( EV_BAS_CONNECT_TIMER,        EV_BAS_BGN + 2 );            //连接定时器
OSPEVENT( EV_BAS_CONNECT_TIMERB,       EV_BAS_BGN + 3 );            //连接定时器
OSPEVENT( EV_BAS_REGISTER_TIMER,       EV_BAS_BGN + 4 );            //注册定时器
OSPEVENT( EV_BAS_REGISTER_TIMERB,      EV_BAS_BGN + 5 );            //注册定时器
OSPEVENT( TIME_GET_MSSTATUS,           EV_BAS_BGN + 6 );            //取主备倒换状态
OSPEVENT( TIME_ERRPRINT,               EV_BAS_BGN + 7 );            //错误打印定时器
OSPEVENT( TIME_BAS_FLASHLOG,           EV_BAS_BGN + 8 );            //刷新日志记录定时器
OSPEVENT( EV_BAS_SHOW,				   EV_BAS_BGN + 9 );			//显示适配器状态
OSPEVENT( EV_BAS_INI,				   EV_BAS_BGN + 10 );           //bas初始化
OSPEVENT( EV_BAS_QUIT,				   EV_BAS_BGN + 11 );			//停止bas
/************************************************************************/
/*							C8KIAudBasInst类定义							*/
/************************************************************************/
class C8KIAudBasInst: public CInstance
{
	enum //实例状态
	{
		IDLE   = 0,
		NORMAL = 1,
		RUNNING = 2
	};
public:
	C8KIAudBasInst();
	~C8KIAudBasInst();
public:
	void DaemonInstanceEntry(CMessage * const pcMsg, CApp* pcApp );
	void DaemonProcInit(CMessage * const pcMsg);	
	void DaemonProcConnectTimeOut( void );
	void DaemonProcRegisterTimeOut( void );
	void DaemonProcRegisterAck( CMessage * const pcMsg );
	void DaemonProcRegisterNack( void );
	void DaemonProcAdpatMsg( CMessage * const pcMsg );
	void DaemonProcDisconnect( CMessage * const pcMsg );

    void InstanceEntry( CMessage * const pcMsg );	
	void ProcInit( CMessage * const pcMsg );
	void ProcStartAdaptReq( CMessage * const pcMsg );		
	void ProcChgAdaptReq( CMessage * const pcMsg );
	void ProcStopAdaptReq( CMessage * const pcMsg );
	void ProcDisconnect( CMessage * const pcMsg );

	BOOL32 SetRcvNetParam( void );
	BOOL32 SetRcvObjectPara( void );
	BOOL32 SetMediaEncParam( void );
	BOOL32 SetMediaDecParam( void );

	u8 GetEncAudType( u8 byEndIdx );
	
	CKdvMediaSnd* GetMediaSndObj( u8 byEndIdx );

	CAudioAdaptor* GetAdaptObj( void );

	void SendStatusChangeMsg( u8 byOnline, u8 byState);
	BOOL SendMsgToMcu(u16 wEvent, CServMsg* const pcServMsg);
	BOOL32 SetAudResend(u8 byEncIdx);
	BOOL32 SetSndObjectPara( u32 dwDstIP,u16 wDstPort,u8 byEncIdx );
	u8 GetAudioMode(const u8 byAudioType, u8 byAudioTrackNum);
	BOOL32 ConnectMcu( u32& dwMcuNode );


	

private:
	CAudioAdaptor m_cBas;								/*媒体控制对象*/
	CKdvMediaRcv m_cAudioRcv;							/*音频接收对象*/
	CKdvMediaSnd m_cAudioSnd[MAXNUM_8KIAUDBAS_ENCNUM];	/*音频发送对象*/
	TAudAdaptParam m_atAudAdpParam[MAXNUM_8KIAUDBAS_ENCNUM];
	BOOL32	m_bIsNeedPrs;
	TMediaEncrypt m_tMediaEncrypt;
	TAudioCapInfo m_tAudioCapInfo;
	u8 m_bySampleRate;
	
	
	
};

class C8KIBASCfg
{
public:
    C8KIBASCfg();
    ~C8KIBASCfg();
public:
    u32           m_dwMcuNode;      //连接的MCU.A 节点号
	u32			  m_dwMcuIId;
	

private:
    u32			  m_dwConnectIP;
	u16			  m_wConnectPort;

	u16			  m_wEqpStartPort;
	u16			  m_wMcuStartPort;

	u8			  m_EqpId;
	u8			  m_byEqpType;

	TPrsTimeSpan  m_tPrsTimeSpan;
	u16			  m_MTUSize;

	u8			  m_byQualityLvl;
	u32			  m_dwLocalIP;
	u8			  m_byMcuId;
    s8			  m_achAlias[MAXLEN_ALIAS];

	u32			  m_dwMcuIp;
public:

	u8    GetQualityLvl(void) const { return m_byQualityLvl; }
    void  SetQualityLvl(u8 byQualityLvl) { m_byQualityLvl = byQualityLvl; }

	u16    GetMTUSize(void) const { return m_MTUSize; }
    void   SetMTUSize(u16 wMTUSize) { m_MTUSize = wMTUSize; }

	TPrsTimeSpan    GetPrsTimeSpan(void) const { return m_tPrsTimeSpan; }
    void  SetPrsTimeSpan(TPrsTimeSpan tPrsTimeSpan) { m_tPrsTimeSpan = tPrsTimeSpan; }

	u8    GetEqpId(void) const { return m_EqpId; }
    void  SetEqpId(u8 byEqpId) { m_EqpId = byEqpId; }

	u16   GetEqpStartPort(void) const { return ntohs(m_wEqpStartPort); }
    void  SetEqpStartPort(u16 wPort) { m_wEqpStartPort = htons(wPort); }

	u16   GetMcuStartPort(void) const { return ntohs(m_wMcuStartPort); }
    void  SetMcuStartPort(u16 wPort) { m_wMcuStartPort = htons(wPort); }

	u32   GetConnectIp(void) const { return ntohl(m_dwConnectIP); }
    void  SetConnectIp(u32 dwConnectIp) { m_dwConnectIP = htonl(dwConnectIp); }

	u32   GetMcuIp(void) const { return ntohl(m_dwMcuIp); }
    void  SetMcuIp(const u32 &dwMcuIp) { m_dwMcuIp = htonl(dwMcuIp); }

    u16   GetConnectPort(void) const { return ntohs(m_wConnectPort); }
    void  SetConnectPort(u16 wConnectPort) { m_wConnectPort = htons(wConnectPort); }

	u8    GetEqpType(void) const { return m_byEqpType; }
    void  SetEqpType(u8 byEqpType) { m_byEqpType = byEqpType; }

    u32   GetEqpIp(void) const { return ntohl(m_dwLocalIP); }
    void  SetEqpIp(u32 dwEqpIp) { m_dwLocalIP = htonl(dwEqpIp); }

	u8   GetMcuId(void) const { return m_byMcuId; }
    void  SetMcuId(u8 byMcuId) { m_byMcuId = byMcuId; }

	s8 *  GetAlias(void) const { return (s8*)m_achAlias; }
    void  SetAlias(s8 * pchAlias)
    {
        if ( NULL == pchAlias )
        {
            return;
        }
        strncpy( m_achAlias, pchAlias, MAXLEN_ALIAS );
        m_achAlias[MAXLEN_ALIAS] = '\0';
        return;
    }

private:
	void Clear( void );
    //void FreeStatusData();
	//void ConvertToRealFR(THDAdaptParam& tAdptParm);
};

typedef zTemplate< C8KIAudBasInst, MAXNUM_8KI_AUD_BAS, C8KIBASCfg> C8KIAudBasApp;

extern C8KIAudBasApp g_c8KIAudBasApp;

#endif//#ifdef _8KI_

#endif