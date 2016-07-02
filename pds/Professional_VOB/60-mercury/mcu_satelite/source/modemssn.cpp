/*****************************************************************************
   模块名      : mcu
   文件名      : modemssn.cpp
   相关文件    : modemssn.h
   文件实现功能: Modem会话
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/07/25  0.9         李屹        创建
   2002/01/07  1.0         李屹        使用宏H323进行改造
   2009/09/01  4.6         张宝卿      移植到4.6高清平台
******************************************************************************/

// mtssn.cpp: implementation of the CModemSsnInst class.
//
//////////////////////////////////////////////////////////////////////
#include "osp.h"
#include "kdvsys.h"
#include "modemssn.h"
#include "evmcumt.h"
#include "evmodem.h"

#ifdef _MSC_VER
#include <ws2tcpip.h>
#endif

CModemSsnApp	g_cModemSsnApp;	//下级MCU会话应用实例

static BOOL bModemMsg = FALSE;

API void pmdmmsg(void)
{
	bModemMsg = TRUE;
}
API void npmdmmsg(void)
{
	bModemMsg = FALSE;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModemSsnInst::CModemSsnInst()
{
	Reset();
}

CModemSsnInst::~CModemSsnInst()
{

}

/*====================================================================
    函数名      ：InstanceDump
    功能        ：重载打印信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 param, 打印状态过滤
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/18    1.0         zhangsh         创建
====================================================================*/
void CModemSsnInst::InstanceDump( u32 param )
{
	char achState[16];
 	switch( CurState() )
	{
	case STATE_IDLE:
		strncpy( achState, "IDLE", sizeof( achState ));
		achState[sizeof( achState ) -1] = '\0';
		break;
	case STATE_CONNECTED:
		strncpy( achState, "CONNECTED", sizeof( achState ));
		achState[sizeof( achState ) -1] = '\0';
		break;
	case STATE_RECEIVE:
		strncpy( achState, "RECEIVE", sizeof( achState ));
		achState[sizeof( achState ) -1] = '\0';
		break;	
	case STATE_SENDRECEIVE:
		strncpy( achState, "SENDRECEIVE", sizeof( achState ));
		achState[sizeof( achState ) -1] = '\0';
		break;
	case STATE_SEND:
		strncpy( achState, "SEND", sizeof( achState ));
		achState[sizeof( achState ) -1] = '\0';
		break;
	default:
		strncpy( achState, "ERROR", sizeof( achState ));
		achState[sizeof( achState ) -1] = '\0';
		break;
	}
	OspPrintf( TRUE, FALSE, "%4u %4u %6u %-12s SM-%u RM-%u SF-%u SB-%u RF-%u RB-%u Type-%u\n",
		m_wMtId, m_byConfIdx, GetInsID(), achState,m_dwSendMode,m_dwRecvMode,
		m_dwSendFrq,m_dwSendRate,m_dwRecFrq,m_dwRecRate, m_byType );
}

/*====================================================================
    函数名      ：InstanceEntry
    功能        ：实例消息处理入口函数，必须override
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/25    1.0         zhangsh         创建
====================================================================*/
void CModemSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		log(LOGLVL_EXCEPTION, "CModemSsnInst: The received msg's pointer in the msg entry is NULL!");
		return;
	}
	switch( pcMsg->event )
	{
	case MODEM_MCU_REG_REQ:
		ProcModemMcuRegReq( pcMsg );
		break;
	case MCU_MODEM_RECEIVE:
		ProcMcuModemReceive( pcMsg );
		break;
	case MCU_MODEM_SEND:
		ProcMcuModemSend( pcMsg );
		break;
	case MCU_MODEM_STOPSEND:
		ProcMcuModemStopSend( pcMsg );
		break;
	case MCU_MODEM_RELEASE:
		ProcMcuModemRelease( pcMsg );
		break;
	case MCU_MODEM_SETSRC:
		ProcMcuModeSetSrc( pcMsg );
		break;
	case MCU_MODEM_SETBITRATE:
		ProcMcuModemSetBitRate( pcMsg );
		break;
	case TIMER_CHECK_RESULT:
		//CheckResult( pcMsg );
		break;
	case TIMER_CHECK_STATUS:
		//CheckStatus( pcMsg );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CModemSsnInst-%u: Wrong message %u(%s) received in current state %u, Inst.%d!\n", 
			GetInsID(), pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID() );
		break;
	}
}

/*====================================================================
    函数名      ：DaemonInstanceEntry
    功能        ：实例消息处理入口函数，必须override
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
				  CApp* pcApp
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/30    1.0         zhangsh          创建
====================================================================*/
void CModemSsnInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	
	if( NULL == pcMsg )
	{
		log(LOGLVL_EXCEPTION, "CModemSsnInst: The received msg's pointer in the msg entry is NULL!");
		return;
	}
		
	switch( pcMsg->event )
	{

	case MCU_APPTASKTEST_REQ:			//GUARD Probe Message
		DaemonProcAppTaskRequest( pcMsg );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CModemSsnInst-%u: Wrong message %u(%s) received in current state %u, Inst.%d!\n", 
			GetInsID(), pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID() );
		break;
	}
}

/*====================================================================
    函数名      ProcModemMcuRegReq
    功能        终端或MCU使用MODEM
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
				  CApp* pcApp
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/30    1.0         zhangsh          创建
====================================================================*/
void CModemSsnInst::ProcModemMcuRegReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt		tRegMt = *( TMt * )cServMsg.GetMsgBody();
	TTransportAddr tAddr = *(TTransportAddr*)(cServMsg.GetMsgBody() + sizeof(TMt) );
	u8 byType = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TTransportAddr) );
	u16 wInstId;
	BOOL bEnable = FALSE;
	switch( CurState() )
	{
	case STATE_IDLE:

	//FIXME: 暂时放开状态机保护
	case STATE_CONNECTED:
	case STATE_RECEIVE:
	case STATE_SEND:
	case STATE_SENDRECEIVE:
		
		//从TOPO中读取IP,端口,类型
		m_dwMtNode = pcMsg->srcnode;	//save MT node
		m_wMtId = tRegMt.GetMtId();	//save MT ID
		MdmssnLog( "Modem-%u for Mt-%u is registered at state.%d!\n", GetInsID(), m_wMtId, CurState() );

		m_dwIpAddr = tAddr.GetIpAddr();
		m_wPort = tAddr.GetPort();
		m_byType = byType;
		m_byConfIdx = cServMsg.GetConfIdx();
		m_dwSendMode = 0;
		m_dwRecvMode = 0;
		m_dwSendRate = 0;
		m_dwSendFrq = 0;
		m_dwRecRate = 0;
		m_dwRecFrq = 0;

		m_cModem.SetModemIp( m_dwIpAddr, m_wPort );
		m_cModem.SetModemType( m_byType );
		m_cModem.SetModOutput( bEnable );
		wInstId = GetInsID();
		cServMsg.SetMsgBody( (u8*)&wInstId, sizeof(u16) );
		
		OspPost( MAKEIID( AID_MCU_VC, CInstance::DAEMON), MODEM_MCU_REG_ACK, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		NEXTSTATE( STATE_CONNECTED );

        MdmssnLog( "[ProcModemMcuRegReq] ModemSsnInst.%d next state.%d!\n",GetInsID(), CurState());
		break;
	default:
		log( LOGLVL_EXCEPTION, "CModemSsnInst-%u: Wrong message %u(%s) received in current state %u, Inst.%d!\n", 
			GetInsID(), pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID() );
		break;
	}
}

/*====================================================================
    函数名      ProcMcuModemReceive
    功能        要求MODEM进入接收状态
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
				  CApp* pcApp
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/30    1.0         zhangsh          创建
====================================================================*/
void CModemSsnInst::ProcMcuModemReceive( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt		tRegMt = *( TMt * )cServMsg.GetMsgBody();
	m_dwRecFrq = *(u32*)(cServMsg.GetMsgBody() + sizeof(TMt) );
	u32 dwRecFrq = m_dwRecFrq;
	int Result;

	switch( CurState() )
	{
	case STATE_CONNECTED:
	case STATE_SEND:
		//发送UDP包
		Result = m_cModem.SetDemFreq( dwRecFrq );
		MdmssnLog( "Set Mt-%u to MODE_RECEIVE, Rate.%u, Freq.%u, ret.%d\n", m_wMtId, m_dwRecRate, dwRecFrq, Result );

		//TEST
		if (Result != MCMD_OK)
		{
			OspPrintf(TRUE, FALSE, "[Err] MdmInst.%d Set Mt-%u to MODE_RECEIVE, Rate.%u, Freq.%u, ret.%d\n", GetInsID(), m_wMtId, m_dwRecRate, dwRecFrq, Result );
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[OK] MdmInst.%d Set Mt-%u to MODE_RECEIVE, Rate.%u, Freq.%u, ret.%d\n", GetInsID(), m_wMtId, m_dwRecRate, dwRecFrq, Result );
		}

		Result = m_cModem.CheckDemFreq( dwRecFrq );
		if ( CurState() == STATE_SEND )
        {
			NEXTSTATE( STATE_SENDRECEIVE );
        }
		if ( CurState() == STATE_CONNECTED )
        {
			NEXTSTATE( STATE_RECEIVE );
        }
		SetTimer( TIMER_CHECK_RESULT, TIMERESULT * 1000 );
		break;

	default:
		log( LOGLVL_EXCEPTION, "CModemSsnInst-%u: Wrong message %u(%s) received in current state %u, Inst.%d!\n", 
			GetInsID(), pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID() );
		break;
	}
}

/*====================================================================
    函数名      ProcMcuModemSend
    功能        要求MODEM进入发送状态
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
				  CApp* pcApp
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/30    1.0         zhangsh           创建
====================================================================*/
void CModemSsnInst::ProcMcuModemSend( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt		tRegMt = *( TMt * )cServMsg.GetMsgBody();
	m_dwSendFrq = *(u32*)(cServMsg.GetMsgBody() + sizeof(TMt) );
	u32 dwSendFrq = m_dwSendFrq;
	BOOL32 bEnabel = TRUE;
	int Result;

	switch( CurState() )
	{
	case STATE_CONNECTED:
	case STATE_RECEIVE:
		//发送UDP包
		Result = m_cModem.SetModFreq( dwSendFrq );
		MdmssnLog( "Set Mt-%u to MODE_SEND, Rate-%u, Frq-%u, ret.%d\n", m_wMtId, m_dwSendRate, m_dwSendFrq, Result );
		
		Result = m_cModem.CheckModFreq( dwSendFrq );
		if ( CurState() == STATE_CONNECTED )
        {
			NEXTSTATE( STATE_SEND );
        }
		if ( CurState() == STATE_RECEIVE )
        {
			NEXTSTATE( STATE_SENDRECEIVE );
        }
		
		m_cModem.SetModOutput( TRUE );
		Result = m_cModem.CheckModOutput( bEnabel );
		if (Result != MCMD_OK)
		{
			MdmssnLog( "Mt-%u CheckModOutput.bEnable.1, ret.%d\n", m_wMtId, Result );
		}
		SetTimer( TIMER_CHECK_RESULT, TIMERESULT * 1000 );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CModemSsnInst-%u: Wrong message %u(%s) received in current state %u, Inst.%d!\n", 
			GetInsID(), pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID() );
		break;
	}
}


/*====================================================================
    函数名      ProcMcuModemStopSend
    功能        要求MODEM停止发送
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
				  CApp* pcApp
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/30    1.0         zhangsh          创建
====================================================================*/
void CModemSsnInst::ProcMcuModemStopSend( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt		tRegMt = *( TMt * )cServMsg.GetMsgBody();
	BOOL32 bEnable = FALSE;
	int Result;
	MdmssnLog( "Set Mt-%u to MODE_STOPSEND\n", m_wMtId );

	switch( CurState() )
	{
	case STATE_SENDRECEIVE:
	case STATE_SEND:

		MdmssnLog( "Set Mt-%u to MODE_STOPSEND\n", m_wMtId );

		//发送UDP包
		m_cModem.SetModOutput( FALSE );
		Result = m_cModem.CheckModOutput( bEnable );
		if ( CurState() == STATE_SENDRECEIVE )
        {
			NEXTSTATE( STATE_RECEIVE );
        }
		if ( CurState() == STATE_SEND )
        {
			NEXTSTATE( STATE_CONNECTED );
        }
		SetTimer( TIMER_CHECK_RESULT, TIMERESULT * 1000 );
		break;
	default:
		break;
	}
}


/*====================================================================
    函数名      ProcMcuModemRelease
    功能        释放MODEM
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
				  CApp* pcApp
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/30    1.0         zhangsh          创建
====================================================================*/
void CModemSsnInst::ProcMcuModemRelease( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	BOOL32 bEnable = FALSE;
	MdmssnLog( "Set Mt-%u to MODE_RELEASE\n", m_wMtId );

	switch( CurState() )
	{
	case STATE_CONNECTED:
	case STATE_RECEIVE:
	case STATE_SEND:
	case STATE_SENDRECEIVE:
		
		MdmssnLog( "Set Mt-%u to MODE_RELEASE\n", m_wMtId );

        //发送UDP包
		m_cModem.SetModOutput( FALSE, TRUE );
		KillTimer( TIMER_CHECK_STATUS);
		KillTimer( TIMER_CHECK_RESULT);
		
        if ( GetInsID() < 17 )
        {
			NEXTSTATE( STATE_CONNECTED );
        }
		else
        {
			NEXTSTATE( STATE_IDLE );
        }

		m_dwSendMode = 0;
		m_dwRecvMode = 0;
		m_dwSendRate = 0;
		m_dwSendFrq = 0;
		m_dwRecRate = 0;
		m_dwRecFrq = 0;
		m_byConfIdx = 0;
		m_wMtId = 0;
		break;

	default:
		log( LOGLVL_EXCEPTION, "CModemSsnInst-%u: Wrong message %u(%s) received in current state %u, Inst.%d!\n", 
			GetInsID(), pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcAppTaskRequest
    功能        ：GUARD模块探测消息处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/21    1.0         杨皞昀         创建
====================================================================*/
void CModemSsnInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );
}


/*====================================================================
    函数名      ：PostMessage2Conf
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/21    1.0         杨皞昀         创建
====================================================================*/
void CModemSsnInst::PostMessage2Conf( u16 wInstanceId, u16 wEvent, const CServMsg & cServMsg )
{
	post( MAKEIID( AID_MCU_VC, wInstanceId ), wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
}


/*====================================================================
    函数名      CheckResult
    功能        检测命令执行结果
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
				  CApp* pcApp
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/30    1.0         zhangsh          创建
====================================================================*/
void CModemSsnInst::CheckResult( const CMessage * pcMsg )
{

	//检查结果是否和实例状态一致
 	CServMsg cServMsg;
	TMt tMt;
	tMt.SetMt( 1, m_wMtId );
 	KillTimer( TIMER_CHECK_RESULT );

	BOOL bNeedStatusCheck = FALSE;
	BOOL32 bEnable;
	int Result;

	u32 dwSendRate = m_dwSendRate;
	u32 dwSendFrq = m_dwSendFrq;
	u32 dwRecRate = m_dwRecRate;
	u32 dwRecFrq = m_dwRecFrq;
	u32 dwRecvMode = m_dwRecvMode;
	u32 dwSendMode = m_dwSendMode;
	cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
	
    //发送时钟模式
	Result = m_cModem.CheckModClkSrc( dwSendMode );
	
	MdmssnLog("Check Modem Result\n" );

	if ( ( dwSendMode != m_dwSendMode) && (Result != MCMD_UNSURE) )
	{
		MdmssnLog("[CheckResult] Set Mt-%u ModeClk From %u to %u\n", m_wMtId ,dwSendMode , m_dwSendMode );

		dwSendMode = m_dwSendMode;
		m_cModem.SetModClkSrc( dwSendMode );

		bNeedStatusCheck = TRUE;
	}
	else
	{
		MdmssnLog("[CheckResult] Set Mt-%u ModeClk failed due to Mode<%d, %d>, Result.%d!\n",
				m_wMtId, dwSendMode, m_dwSendMode, Result);
	}

    //接收时钟模式
	Result = m_cModem.CheckDemClkSrc( dwRecvMode );
	if ( (dwRecvMode != m_dwRecvMode ) && (Result != MCMD_UNSURE) )
	{
		MdmssnLog( "Set Mt-%u DemClk From %u to %u\n", m_wMtId ,dwRecvMode , m_dwRecvMode );

		dwRecvMode = m_dwRecvMode;
		m_cModem.SetDemClkSrc( dwRecvMode );
		bNeedStatusCheck = TRUE;
	}
	else
	{
		MdmssnLog( "Set Mt-%u DemClk failed due to Mode<%d, %d>, Result.%d\n",
			m_wMtId, dwRecvMode , m_dwRecvMode, Result );
	}
	
	
    //接收状态
	if ( CurState() == STATE_RECEIVE || CurState() == STATE_SENDRECEIVE )
	{
		Result = m_cModem.CheckDemBitRate( dwRecRate );
		if ( (dwRecRate != m_dwRecRate) && (Result != MCMD_UNSURE) )
		{
			MdmssnLog( "Set Mt-%u RecRate form %u to %u\n", m_wMtId,dwRecRate,m_dwRecRate );

			dwRecRate = m_dwRecRate;
			m_cModem.SetDemBitRate( dwRecRate );
			bNeedStatusCheck = TRUE;
		}
		else
		{
			MdmssnLog( "Set Mt-%u RecRate failed due to Rate<%u, %u>, Result.%d\n",
				m_wMtId ,dwRecRate , m_dwRecRate, Result );
		}

		Result = m_cModem.CheckDemFreq( dwRecFrq );
		if ( (dwRecFrq != m_dwRecFrq) && (Result != MCMD_UNSURE) )
		{
			MdmssnLog( "Set Mt-%u RecFrq from %u to %u\n", m_wMtId,dwRecFrq ,m_dwRecFrq);

			dwRecFrq = m_dwRecFrq;
			m_cModem.SetDemFreq( m_dwRecFrq );
			bNeedStatusCheck = TRUE;
		}
		else
		{
			MdmssnLog( "Set Mt-%u RecFrq failed due to Freq<%u, %u>, Result.%d\n",
				m_wMtId,dwRecFrq ,m_dwRecFrq, Result);
		}
		
	}
	
    //发送状态
	if ( CurState() == STATE_SEND || CurState() == STATE_SENDRECEIVE )
	{
		
		Result = m_cModem.CheckModFreq( dwSendFrq );
		if ( (dwSendFrq != m_dwSendFrq) && (Result != MCMD_UNSURE) )
		{
			MdmssnLog( "Set Mt-%u SendFrq from %u to %u\n", m_wMtId, dwSendFrq ,m_dwSendFrq);

			dwSendFrq = m_dwSendFrq;
			m_cModem.SetModFreq( dwSendFrq );
			bNeedStatusCheck = TRUE;
		}
		else
		{
			MdmssnLog( "Set Mt-%u SendFrq failed due to Freq<%d, %d>, Result.%d\n",
				m_wMtId, dwSendFrq ,m_dwSendFrq, Result);
		}

		Result = m_cModem.CheckModBitRate( dwSendRate );
		if ( (dwSendRate != m_dwSendRate) && (Result != MCMD_UNSURE) )
		{
			MdmssnLog( "Set Mt-%u SendRate form %u to %u\n", m_wMtId,dwSendRate,m_dwSendRate );

			dwSendRate = m_dwSendRate;
			m_cModem.SetModBitRate( dwSendRate );
			bNeedStatusCheck = TRUE;
		}
		else
		{
			MdmssnLog( "Set Mt-%u SendRate failed due to SndRate<%d, %d>, Result.%d\n",
				m_wMtId,dwSendRate,m_dwSendRate, Result );
		}
	}
	
	if ( CurState() == STATE_SEND || CurState() == STATE_SENDRECEIVE )
	{
		bEnable = TRUE;

		Result = m_cModem.CheckModOutput( bEnable );
		if ( (bEnable != TRUE) && (Result != MCMD_UNSURE) )
		{
			MdmssnLog( "Set Mt-%u OutPut form %u to %u\n", m_wMtId,bEnable, 1 );

			m_cModem.SetModOutput( TRUE );
			bNeedStatusCheck = TRUE;
		}
		else
		{
			MdmssnLog( "Set Mt-%u OutPut failed due to bEnable.%d, Result.%d\n",
				m_wMtId,bEnable, Result );
		}
	}
	else
	{
		bEnable = FALSE;
		Result = m_cModem.CheckModOutput( bEnable );
		if ( (bEnable != FALSE) && (Result != MCMD_UNSURE) )
		{
			MdmssnLog( "Set Mt-%u OutPut form %u to %u\n", m_wMtId,bEnable, 0 );

			m_cModem.SetModOutput( FALSE );
			bNeedStatusCheck = TRUE;
		}
		else
		{
			MdmssnLog("Set Mt-%u OutPut failed due to bEnable.%d, Result.%d\n",
					   m_wMtId,bEnable, Result );
		}
	}

	if ( bNeedStatusCheck )
	{
		if ( GetInsID() > 16 )
        {
			PostMessage2Conf( m_byConfIdx, MODEM_MCU_ERRSTAT_NOTIF, cServMsg );
        }
		if ( CurState() != STATE_IDLE )
        {
			SetTimer( TIMER_CHECK_RESULT , TIMERESULT* 1000 );
        }
	}
	else
	{
		if ( GetInsID() > 16 )
        {
			PostMessage2Conf( m_byConfIdx, MODEM_MCU_RIGHTSTAT_NOTIF, cServMsg );
        }
		if ( CurState() != STATE_IDLE )
        {
			//stauts
			SetTimer( TIMER_CHECK_STATUS , TIMESTATUS * 1000 );
        }
	}
}


/*====================================================================
    函数名      CheckStatus
    功能        检测MODEM状态
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
				  CApp* pcApp
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/30    1.0         zhangsh          创建
====================================================================*/
void CModemSsnInst::CheckStatus( const CMessage * pcMsg )
{
	//检查结果是否和实例状态一致
 	CServMsg cServMsg;
	TMt tMt;
	tMt.SetMt( 1, m_wMtId );
 	KillTimer( TIMER_CHECK_STATUS );
	
	if ( CurState() == STATE_IDLE )
	{
		return;
	}
	
	//发送时钟模式
	if (bModemMsg)
	{
		log( LOGLVL_EXCEPTION, "Check Modem Status\n" );
	}
	
	m_cModem.QueryModClkSrc( );
	m_cModem.QueryDemClkSrc( );
	
	//接收状态
	if ( CurState() == STATE_RECEIVE || CurState() == STATE_SENDRECEIVE )
	{
		m_cModem.QueryDemBitRate( );
		m_cModem.QueryDemFreq(  );
	}
	
	//发送状态
	if ( CurState() == STATE_SEND || CurState() == STATE_SENDRECEIVE )
	{
		m_cModem.QueryModFreq( );
		m_cModem.QueryModBitRate();
	}
	

	if ( CurState() != STATE_IDLE )
	{
		m_cModem.QueryModOutput();
	}
	SetTimer( TIMER_CHECK_RESULT , TIMERESULT * 1000 );
	return;
}

/*====================================================================
    函数名      ProcMcuModeSetSrc
    功能        设置MODEM的主从时钟
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
				  CApp* pcApp
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/30    1.0         zhangsh          创建
====================================================================*/
void CModemSsnInst::ProcMcuModeSetSrc( const CMessage *pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt		tRegMt = *( TMt * )cServMsg.GetMsgBody();
	m_dwRecvMode = *(u32*)(cServMsg.GetMsgBody() + sizeof(TMt) );
	m_dwSendMode = *(u32*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u32) );
	u32 dwRecvMode = m_dwRecvMode;
	u32 dwSendMode = m_dwSendMode;
	
    MdmssnLog( "Set Mt-%u to MODE_SENDSRC,MODE_REVSRC \n", m_wMtId );
	int Result;
	
    switch( CurState() )
	{
	case STATE_CONNECTED:
	case STATE_RECEIVE:
	case STATE_SEND:
	case STATE_SENDRECEIVE:
		
		//发送UDP包
		m_cModem.SetModClkSrc( dwSendMode );
		Result = m_cModem.CheckModClkSrc( dwSendMode );

		m_cModem.SetDemClkSrc( dwRecvMode );
		Result = m_cModem.CheckDemClkSrc( dwRecvMode );
		SetTimer( TIMER_CHECK_RESULT, TIMERESULT * 1000 );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CModemSsnInst-%u: Wrong message %u(%s) received in current state %u, Inst.%d!\n", 
			GetInsID(), pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID() );
		break;
	}
}

/*====================================================================
    函数名      ProcMcuModemSetBitRate
    功能        码率控制
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
				  CApp* pcApp
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/30    1.0         zhangsh          创建
====================================================================*/
void CModemSsnInst::ProcMcuModemSetBitRate( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt		tRegMt = *( TMt * )cServMsg.GetMsgBody();
	u32 RecRate = *(u32*)(cServMsg.GetMsgBody() + sizeof(TMt) );
	u8 byMode = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMt)+sizeof(u32) );
	u32 dwSendRate;
	u32 dwRecRate;
	int Result;
	
    switch( CurState() )
	{
	case STATE_CONNECTED:
	case STATE_RECEIVE:
	case STATE_SEND:
	case STATE_SENDRECEIVE:

		MdmssnLog("[McuModemSetBitRate] Mt.%d, BitRate.%d, Mode.%d\n", tRegMt.GetMtId(), RecRate, byMode);
		
        //发送UDP包
		if ( byMode == 1 )
		{
			m_dwSendRate = RecRate;
			dwSendRate = RecRate;
			u32 dwRecRate = m_dwRecRate;
			m_cModem.SetModBitRate( dwSendRate );
			Result = m_cModem.CheckModBitRate( dwSendRate );
		}
		if ( byMode == 2 )
		{
			m_dwRecRate = RecRate;
			dwRecRate = m_dwRecRate;
			m_cModem.SetDemBitRate( dwRecRate );
			Result |= m_cModem.CheckDemBitRate( dwRecRate );
		}

		SetTimer( TIMER_CHECK_RESULT, TIMERESULT * 1000 );
		break;

	default:
		log( LOGLVL_EXCEPTION, "CModemSsnInst-%u: Wrong message %u(%s) received in current state %u, Inst.%d!\n", 
			GetInsID(), pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID() );
		break;
	}	
}

void CModemSsnInst::MdmssnLog(s8 * pszFmt, ...)
{
	s8 achBuf[255];
	va_list argptr;
	if (bModemMsg)
	{
		s32 nPrefix = sprintf( achBuf, "[Mdmssn.%d]: ", GetInsID() );
		va_start(argptr, pszFmt);    
		vsprintf(achBuf + nPrefix, pszFmt, argptr);   
		OspPrintf(TRUE, FALSE, achBuf);
		va_end(argptr); 	
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModemConfig::CModemConfig()
{

}

CModemConfig::~CModemConfig()
{

}