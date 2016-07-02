/*****************************************************************************
   模块名      : mcu_new
   文件名      : mtssn.h
   相关文件    : mtssn.cpp
   文件实现功能: 下级MT会话应用类头文件
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/07/25  0.9         李屹        创建
   2002/01/07  1.0         李屹        使用宏H323进行改造
   2009/09/01  4.6         张宝卿      移植到4.6高清平台
******************************************************************************/

// mtssn.h: interface for the CMtSsnInst class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MTSSN_H__AB423FC4_7512_4B60_9269_742737980FBF__INCLUDED_)
#define AFX_MTSSN_H__AB423FC4_7512_4B60_9269_742737980FBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vctopo.h"
#include "evmcu.h"

#ifndef SOCKET 
#define SOCKET   int
#endif


//和终端roundTripDelay最大值（秒）
#define MAXDELAY_MT_ROUNDTRIPDELAY	10

//会话定时器长度系数
const u16 TIMER_MTSSN_COEFF		= 8;	//0.8

class CMtSsnInst : public CInstance  
{
public:
	enum 
	{ 
		STATE_IDLE		= 0,
#ifndef H323
		STATE_CONNECTED	= 1,	//连接建立但未加入会议状态
#endif
		STATE_CALLING	= 2,	//发出加入邀请正在等待终端应答或
								//收到终端加入申请等待业务回应
		STATE_NORMAL	= 3,	//呼叫成功MT加入会议正常状态
		STATE_WAITING	= 4		//发给终端请求后等待状态
	};

	enum
	{
		TIMER_WAIT_REPLY,		//等待应答
		TIMER_WAIT_CALL_RSP,	//等待终端对会议邀请的应答或
								//等待业务对终端加入申请的应答
		TIMER_ROUNDTRIPDELAY_REQ,		//定时发送roundTripDelayRequest
		TIMER_WAIT_ROUNDTRIPDELAY_RSP,	//等待返回roundTripDelayResponse

		TIMER_RECEIVE_SIGNAL,	//接受信令时钟
	};

protected:
#ifndef H323
	u32		m_dwMtNode;		//对应终端节点号

#else
	HCALL	m_hsCall;	//stack handle of call
#endif	
	u8		m_byTimeoutTimes;	//roundTripDelayRsp time out times
	BOOL	m_bGotRoundTripDelayRsp;	//whether got roundTripDelay

	u8		m_byMtId;		//对应终端号
	CConfId	m_cConfId;	//终端所属会议号
	char	m_abySavedMsgHdr[SERV_MSGHEAD_LEN];	//保存原命令头
	u16		m_wSavedEvent;
    static SOCKET  m_hSocket;      //终端发送UDP控制包的SOCKET

#ifndef H323
	BOOL SendMsgToSubMt( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
#endif
	time_t timeold;
	u32    eventold;
public:
	void ProcMtMcuDaemonConfMsg( const CMessage * pcMsg );
	void ProcMtMcuMsg( const CMessage * pcMsg );
	void ProcMcuMtMsg( const CMessage * pcMsg );
	void ProcMtMcuResponse( const CMessage * pcMsg );
	void ProcMcuMtRequest( const CMessage * pcMsg );
	void ProcMtMcuGetMcuTopoReq( const CMessage * pcMsg );
	void ProcMtDisconnect( const CMessage * pcMsg );
	void ProcMtMcuRegReq( const CMessage * pcMsg );
	void ProcMtMcuReadyNotif( const CMessage * pcMsg );
	void ProcMcuMtDropMtCmd( const CMessage * pcMsg );
	void ProcMcuMtInviteMtReq( const CMessage * pcMsg );		//邀请终端加入
	void ProcMtMcuInviteMtRsp( const CMessage * pcMsg );		//终端对邀请入会的应答
	void ProcMtMcuApplyJoinRequest( const CMessage * pcMsg );	//终端申请加入
	void ProcMcuMtApplyJoinRsp( const CMessage * pcMsg );		//mcu对终端加入申请的应答
	void ProcMtMcuCreateConfRequest( const CMessage * pcMsg );	//终端创建会议
	void ProcMcuMtCreateConfRsp( const CMessage * pcMsg );		//mcu对终端会议创建邀请的应答

	void ProcMcuMtOpenLogicChnReq( const CMessage * pcMsg );	//mcu打开终端双流通道
	void ProcMcuMtCloseLogicChnCmd( const CMessage * pcMsg );	//mcu关闭终端双流通道
	
	/*
	 *	Some Special Message For Satellite MCU
	 */
	void ProcMcuMtCapbilitySetNotif( const CMessage * pcMsg );
	void PostMessage2Conf( u16 wInstanceId, const CServMsg & cServMsg );
	BOOL FilterMessage( u32 eventid );
	/* For Message Transport 				*/
	SOCKET  CreateUdpSocket( u32 dwIpAddr, u16 wPort );
	BOOL DropSocketMulticast( SOCKET sock ,u32 dwMulticastIP, u32 dwIfIP  );
	BOOL JoinSocketMulticast( SOCKET sock ,u32 dwMulticastIP ,u32 dwIfIP );
	
	void ProcTimerWaitReply( const CMessage * pcMsg );

#ifdef H323
	void SendH323MsgToSubMt( const CMessage * pcMsg );
#endif
	void ProcTimerRoundTripDelayReq( const CMessage * pcMsg );
	void ProcTimerWaitRoundTripDelayRsp( const CMessage * pcMsg );
	void ProcMtMcuRoundTripDelayRsp( const CMessage * pcMsg );
	
	void ProcTimerWaitCallRsp( const CMessage * pcMsg );
	void DaemonProcAppTaskRequest( const CMessage * pcMsg );
	void DaemonProcPowerOn( const CMessage * pcMsg, CApp * pcApp );
	void InstanceEntry( CMessage * const pcMsg );
	void DaemonInstanceEntry( CMessage * const pcMsg, CApp * pcApp );
	void InstanceDump( u32 param = 0 );
	void InstanceExit();

	CMtSsnInst();
	virtual ~CMtSsnInst();

    void SendMsgToConf( u8 byConfIdx, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
    void SendMsgToDaemonConf( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
};


class CMtConfig
{
public:
	BOOL SendMsgToMtSsn( u8 byMtId, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	void BroadcastToAllMtSsn( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	BOOL SendMultiCastMsg( const CServMsg& cServMsg );
    
	CMtConfig();
	virtual ~CMtConfig();
};

typedef zTemplate< CMtSsnInst, MAXNUM_MCU_MT, CMtConfig, sizeof( u8 ) > CMtSsnApp;

extern CMtSsnApp	g_cMtSsnApp;	//下级MT会话应用实例


/*====================================================================
    函数名      ：BroadcastToAllMtSsn
    功能        ：发消息给所有已经登记连接的下级终端对应的会话实例
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/28    1.0         LI Yi         创建
====================================================================*/
inline void CMtConfig::BroadcastToAllMtSsn( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	::OspPost( MAKEIID( AID_MCU_MTSSN, CInstance::EACH ), wEvent, pbyMsg, wLen );
}

/*====================================================================
    函数名      ：SendMsgToMtSsn
    功能        ：发消息给指定的下级终端对应的会话实例，终端号为0时发给
				  任意IDLE实例
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 指定的终端号，如果为NULL，发给任一IDLE实例
				  u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：成功返回TRUE，若该下级终端未登记返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/28    1.0         LI Yi         创建
====================================================================*/
inline BOOL CMtConfig::SendMsgToMtSsn( u8 byMtId, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	if( byMtId == NULL )	//发给任意IDLE实例
	{
		::OspPost( MAKEIID( AID_MCU_MTSSN, CInstance::PENDING ), wEvent, pbyMsg, wLen );
		return( TRUE );
	}
	else if( byMtId >= MAXNUM_MCU_MT )
	{
		return( FALSE );
	}
	else	//利用Alias
	{
//		::OspPost( (char*)&byMtId, sizeof( u8 ), AID_MCU_MTSSN, wEvent, pbyMsg, wLen );
		OspPost( MAKEIID( AID_MCU_MTSSN, byMtId ), wEvent, pbyMsg, wLen );
		return( TRUE );
	}
}

inline BOOL CMtConfig::SendMultiCastMsg( const CServMsg& cServMsg )
{
	::OspPost( MAKEIID( AID_MCU_MTSSN, CInstance::DAEMON ), MCU_MULTICAST_MSG, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	return TRUE;	
}

#endif // !defined(AFX_MTSSN_H__AB423FC4_7512_4B60_9269_742737980FBF__INCLUDED_)
