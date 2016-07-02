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

******************************************************************************/

// mtssn.h: interface for the CMtSsnInst class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MODEMSSN_H_)
#define _MODEMSSN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vctopo.h"
#include "evmcu.h"
#include "modemcmdproxy.h"


extern TMcuTopo	g_atMcuTopo[MAXNUM_TOPO_MCU];
extern u8		g_wMcuTopoNum;
extern TMtTopo		g_atMtTopo[MAXNUM_TOPO_MT];	//保存全网中MT拓扑信息
extern u16		g_wMtTopoNum;		//保存全网中MT数目

//和终端roundTripDelay最大值（秒）
#define TIMERESULT	        5
#define TIMESTATUS          15
#define MODE_RECEIVE        1
#define MODE_SEND           2
#define MODE_STOPSEND       4
#define MODE_RELEASE        16
#define MODE_SENDSRC        32
#define MODE_REVSRC         64


//会话定时器长度系数

class CModemSsnInst : public CInstance  
{
public:
	enum 
	{ 
		STATE_IDLE		= 0,
		STATE_CONNECTED	= 1,	//连接建立但未加入会议状态
		STATE_RECEIVE	= 2,	//接收状态
		STATE_SENDRECEIVE	= 3,	//发送接收状态
		STATE_SEND = 4
	};

	enum
	{
		TIMER_CHECK_RESULT,	//命令发出等待检测结果
		TIMER_CHECK_STATUS
	};

protected:
	ModemCmdProxy m_cModem;


	u32		m_dwMtNode;		//对应终端节点号
	u16		m_wMtId;		//对应终端号
	u32		m_dwIpAddr;
	u16		m_wPort;
	u8		m_byType;
	u8      m_bySetMode;
	u8      m_byConfIdx;

	u32		m_dwRecvMode;
	u32		m_dwSendMode;
	u32		m_dwSendFrq;
	u32     m_dwRecFrq;
	u32     m_dwSendRate;
	u32     m_dwRecRate;
	BOOL	m_bCheckResult;
	BOOL	m_bCheckStatus;

public:
	void Reset()
	{
		m_dwMtNode = 0;		//对应终端节点号
		m_wMtId = 0;		//对应终端号
		m_dwIpAddr = 0;
		m_wPort = 0;
		m_byType = 0;
		m_bySetMode = 0;
		
		m_dwRecvMode = 0;
		m_dwSendMode = 0;
		m_dwSendFrq = 0;
		m_dwRecFrq = 0;
		m_dwSendRate = 0;
		m_dwRecRate = 0;
		m_bCheckStatus = FALSE;
		m_bCheckResult = FALSE;
	}
	void ProcMcuModeSetSrc( const CMessage *pcMsg );
	void ProcModemMcuRegReq( const CMessage * pcMsg );
	void ProcMcuModemReceive( const CMessage * pcMsg );
	void ProcMcuModemSend( const CMessage * pcMsg );
	void ProcMcuModemStopSend( const CMessage * pcMsg );	
	void ProcMcuModemRelease( const CMessage * pcMsg );
	void ProcMcuModemSetBitRate( const CMessage * pcMsg );
	void PostMessage2Conf( u16 wInstanceId, u16 wEvent,const CServMsg & cServMsg );
		
	void CheckResult( const CMessage * pcMsg );
	void CheckStatus( const CMessage * pcMsg );
	void DaemonProcAppTaskRequest( const CMessage * pcMsg );
	void DaemonProcPowerOn( const CMessage * pcMsg, CApp * pcApp );
	void InstanceEntry( CMessage * const pcMsg );
	void DaemonInstanceEntry( CMessage * const pcMsg, CApp * pcApp );
	void InstanceDump( u32 param = 0 );

	void MdmssnLog(s8 * pszFmt, ...);
	CModemSsnInst();
	virtual ~CModemSsnInst();
};


class CModemConfig
{
public:
	BOOL SendMsgToModemSsn( u16 wMtId, u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	void BroadcastToAllModemSsn( u16 wEvent, u8 * const pbyMsg = NULL, u16 wLen = 0 );
	CModemConfig();
	virtual ~CModemConfig();
};

typedef zTemplate< CModemSsnInst, MAXNUM_MCU_MT+MAXNUM_MCU_MODEM+1, CModemConfig, sizeof( u8 ) > CModemSsnApp;

extern CModemSsnApp	g_cModemSsnApp;	//下级MT会话应用实例


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
inline void CModemConfig::BroadcastToAllModemSsn( u16 wEvent, u8 * const pbyMsg, u16 wLen )
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
inline BOOL CModemConfig::SendMsgToModemSsn( u16 wMtId, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	if( wMtId == NULL )	//发给任意IDLE实例
	{
		::OspPost( MAKEIID( AID_MCU_MODEMSSN, CInstance::PENDING ), wEvent, pbyMsg, wLen );
		return( TRUE );
	}
	else if( wMtId >= MAXNUM_MCU_MT+16 )
	{
		return( FALSE );
	}
	else	//利用Alias
	{
		OspPost( MAKEIID( AID_MCU_MODEMSSN, wMtId ), wEvent, pbyMsg, wLen );
		return( TRUE );
	}
}


API void pmdmmsg(void);
API void npmdmmsg(void);


#endif

