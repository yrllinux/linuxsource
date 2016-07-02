/*****************************************************************************
   模块名      : mcu配置界面化
   文件名      : mcucfg.cpp
   相关文件    : mcucfg.h
   文件实现功能: MCU配置界面化
   作者        : 许世林
   版本        : V4.0  Copyright(C) 2005-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/08/23  4.0         许世林        创建
******************************************************************************/

#include "evmcumcs.h"
#include "evmcuvcs.h"
#include "evmcumt.h"
#include "evmcueqp.h"
#include "evmcu.h"
#include "mcuvc.h"
#include "mcucfg.h"
#include "mtadpssn.h"
#include "eqpssn.h"
#include "mcsssn.h"
#include "msmanagerssn.h"
#include "mcuerrcode.h"
#include "mcuver.h"
#include "mcuagtstruct.h"

#ifdef _LINUX_
#include "boardwrapper.h"
#else
#include "brddrvlib.h"
#endif
#include "brdwrapperdef.h"

// BSP中定义,WINDOWS下模拟时使用
#ifdef WIN32

#pragma pack( push )
#pragma pack(1)

struct TBrdPosition
{
public:
    u8 byBrdID;         // 板子ID号 
    u8 byBrdLayer;      // 板子所在层号
    u8 byBrdSlot;       // 板子所在槽位号
    
public:
    TBrdPosition(){ Clear();}
    void Clear(void) { memset( this, 0, sizeof(TBrdPosition) );  }
    void SetBrdId(u8 byID ) { byBrdID = byID;  }
    u8   GetBrdId(void) const { return byBrdID; }
    void SetBrdLayer(u8 byLayer ) { byBrdLayer = byLayer; }
    u8   GetBrdLayer(void) const { return byBrdLayer; }
    void SetBrdSlot(u8 bySlot) { byBrdSlot = bySlot; }
    u8   GetBrdSlot(void) const { return byBrdSlot; }
    BOOL32 IsSameBrd( TBrdPosition &tBrdPosition)
    {
        if ( tBrdPosition.GetBrdId() == byBrdID &&
            tBrdPosition.GetBrdLayer() == byBrdLayer &&
            tBrdPosition.GetBrdSlot() == byBrdSlot ) 
        {
            return TRUE;
        }
        return FALSE;
    }
    
};

#pragma pack( pop )
#endif


CMcuCfgApp	g_cMcuCfgApp;	

//nack
#define MCUCFG_INVALIDPARAM_NACK(cMsg, event, code)     \
    cMsg.SetMsgBody(NULL, 0);                           \
    cMsg.SetErrorCode(code);                            \
    SendMsg2Mcsssn(cMsg, event);                        \
	NEXTSTATE(STATE_IDLE);								\

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMcuCfgInst::CMcuCfgInst()
{
	memset( m_abyServMsgHead, 0, SERV_MSGHEAD_LEN );
}

CMcuCfgInst::~CMcuCfgInst()
{
}

void CMcuCfgInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		OspPrintf( TRUE, FALSE, "CMcuCfgInst: The received msg's pointer in the msg entry is NULL!\n");
		return;
	}	
    
    CfgLog("receive msg %u(%s).\n", pcMsg->event, ::OspEventDesc( pcMsg->event ));

    switch(pcMsg->event)
    {
    //MCS请求mcu 的cpu占有率
    case MCS_MCU_CPUPERCENTAGE_REQ:       
    case MCS_MCU_CPUPERCENTAGE_CMD:
        ProcCpuPercentage(pcMsg);
        break;

    //mcu设备配置信息
    case MCS_MCU_CHANGEMCUEQPCFG_REQ:
    case MCS_MCU_GETMCUEQPCFG_REQ:
        ProcMcuEqpCfg(pcMsg);
        break;

    //单板配置信息
    case MCS_MCU_CHANGEBRDCFG_REQ:
    case MCS_MCU_GETBRDCFG_REQ:
        ProcBrdCfg(pcMsg);
        break;
        
    //mcu配置信息基本设置
    case MCS_MCU_CHANGEMCUGENERALCFG_REQ:     
    case MCS_MCU_GETMCUGENERALCFG_REQ:
        ProcMcuGeneralCfg(pcMsg);
        break;

	//更新telnet登陆信息
	case MCS_MCU_UPDATETELNETLOGININFO_REQ:
		ProcUpdateTelnetLoginInfo(pcMsg);
		break;

    // 会控取Vmp合成方案信息
    case MCS_MCU_GETVMPSCHEMEINFO_REQ:
        ProcMcsGetVmpScheme(pcMsg);
        break;

	//4.6版本 新加 jlb
	// 取HDU预案
    case MCS_MCU_GETHDUSCHEMEINFO_REQ:
		ProcMcsGetHduScheme( pcMsg );
        break;
	
	case MCS_MCU_CHANGEHDUSCHEMEINFO_REQ:
		ProcMcsChangeHduScheme( pcMsg );
        break;

	// 重启单板
    case MCS_MCU_REBOOTBRD_REQ:
        ProcRebootBrd(pcMsg);
        break;
        
    case MCS_MCU_UPDATEBRDVERSION_CMD:
	case MCS_MCU_STARTUPDATEDSCVERSION_REQ:
	case MCS_MCU_DSCUPDATEFILE_REQ:
        ProcUpdateBrdVersion( pcMsg );
        break;

	case AGT_SVC_STARTUPDATEDSCVERSION_ACK:
	case AGT_SVC_STARTUPDATEDSCVERSION_NACK:
		ProcStartUpdateDscVersionRsp( pcMsg );
		break;

	case AGT_SVC_DSCUPDATEFILE_ACK:
	case AGT_SVC_DSCUPDATEFILE_NACK:
		ProcDscUpdateFileRsp(pcMsg);
		break;

    // 单板管理发来的单板状态    
    case AGT_SVC_BOARDSTATUS_NOTIFY:
    //版本升级状态
    case AGT_SVC_UPDATEBRDVERSION_NOTIF:
	// 有新的DSC板注册请求
//	case AGT_SVC_NEWDSCREGREQ_NOTIFY:
        NotifyMcsBoardStatus(pcMsg);
        break;

	case AGT_SVC_SETDSCINFO_ACK:	// 设置/修改DSC info的回应消息, zgc, 2007-07-16
	case AGT_SVC_SETDSCINFO_NACK:
		ProcSetDscInfoRsp(pcMsg);
		break;

	case AGT_SVC_DSCREGSUCCEED_NOTIF:	// 有DSC板注册成功, zgc, 2007-08-24
		ProcDscRegSucceedNotif(pcMsg);
		break;

    //主备发生切换时,新的主用板将界面化配置信息重新通知当前所有会控
    case MCU_MSSTATE_EXCHANGE_NTF:
        ProcMcuSynMcsCfgNtf(pcMsg);
        break;

	case MCUCFG_DSCCFG_WAITINGCHANGE_OVER_TIMER:
		ProcDscCfgWaitingChangeOverTimerOut( pcMsg );
		break;

	case MCSSSN_MCUCFG_MCSDISSCONNNECT_NOTIFY:
		ProcMcsDisconnectNotif( pcMsg );
		break;
	// vcs	
	case VCS_MCU_GETSOFTNAME_REQ:
    case VCS_MCU_CHGSOFTNAME_REQ:
		ProcVCSGeneralCfg( pcMsg );
		break;

    default:
		OspPrintf(TRUE, FALSE, "[CfgSsn]: Wrong message %u(%s) received in InstanceEntry!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ) );
        break;
    }

	return;
}

/*=============================================================================
    函 数 名： DaemonInstanceEntry
    功    能： 实例消息处理Daemon入口函数，必须override
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
               CApp* pcApp
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/12  4.0			万春雷                  创建
=============================================================================*/
void CMcuCfgInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	if (NULL == pcMsg)
	{
		OspPrintf( TRUE, FALSE, "McuCfg[DaemonInstanceEntry]: The received msg's pointer is NULL!");
		return;
	}

	switch (pcMsg->event)
	{
	case MCU_APPTASKTEST_REQ:			//GUARD Probe Message
		DaemonProcAppTaskRequest( pcMsg );
		break;
	default:
		OspPrintf( TRUE, FALSE, "McuCfg[DaemonInstanceEntry]: Wrong message %u(%s) received!\n", 
			                     pcMsg->event, ::OspEventDesc( pcMsg->event ) );
		break;
	}
	
	return;
}

/*=============================================================================
    函 数 名： DaemonProcAppTaskRequest
    功    能： GUARD模块探测消息处理函数
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/12  4.0			万春雷                  创建
=============================================================================*/
void CMcuCfgInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );

	return;
}

/*=============================================================================
  函 数 名： ProcRebootBrd
  功    能： 会控重启某单板
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void  
=============================================================================*/
void  CMcuCfgInst::ProcRebootBrd(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 byBrdIndex = *cServMsg.GetMsgBody();

    u8 byBrdNum = 0;
    TBoardInfo atBrdInfo[MAXNUM_BOARD];
    u16 wRet = g_cMcuAgent.ReadBrdTable(&byBrdNum, atBrdInfo);
    if (SUCCESS_AGENT != wRet || 0 == byBrdNum || byBrdNum > MAXNUM_BOARD)
    {
        CfgLog("[ProcRebootBrd] read brd info failed! brd num :%d, wRet :%d\n", byBrdNum, wRet);
        SendMsg2Mcsssn(cServMsg, pcMsg->event+2);
        return;
    }

    u8 byIndex;
    for (byIndex = 0; byIndex < byBrdNum; byIndex++)
    {
        if (atBrdInfo[byIndex].GetBrdId() == byBrdIndex)
        {
            break;
        }
    }
    if (byIndex == byBrdNum)
    {
        CfgLog("[ProcRebootBrd] invalid brd index :%d\n", byBrdIndex);
        SendMsg2Mcsssn(cServMsg, pcMsg->event+2);
        return;
    }

    wRet = g_cMcuAgent.RebootBoard(atBrdInfo[byIndex].GetLayer(), atBrdInfo[byIndex].GetSlot(), atBrdInfo[byIndex].GetType());
    if (SUCCESS_AGENT != wRet)
    {
        CfgLog("[ProcRebootBrd] RebootBoard failed, ret :%d\n", wRet);
        SendMsg2Mcsssn(cServMsg, pcMsg->event+2);
        return;
    }

    SendMsg2Mcsssn(cServMsg, pcMsg->event+1);
    CfgLog("[ProcRebootBrd] RebootBoard success, Index:%d, layer:%d, slot:%d, type:%d\n", 
            atBrdInfo[byIndex].GetBrdId(), atBrdInfo[byIndex].GetLayer(), 
            atBrdInfo[byIndex].GetSlot(), atBrdInfo[byIndex].GetType());
    return;
}

/*=============================================================================
  函 数 名： ProcUpdateBrdVersion
  功    能： 处理会控升级软件
  算法实现： 
  全局变量： 
  参    数： const CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMcuCfgInst::ProcUpdateBrdVersion(const CMessage * const pcMsg )
{
	if ( pcMsg == NULL )
	{
		CfgLog( "[ProcUpdateBrdVersion] Msg is NULL, Error!\n" );
		return;
	}

	switch(pcMsg->event)
	{
	case MCS_MCU_UPDATEBRDVERSION_CMD:
		post( MAKEIID( AID_MCU_AGENT, 1 ), SVC_AGT_UPDATEBRDVERSION_CMD, pcMsg->content, pcMsg->length );
		break;
	case MCS_MCU_STARTUPDATEDSCVERSION_REQ:
		{
			CServMsg cRevMsg(pcMsg->content, pcMsg->length);
			CServMsg cSendMsg;
			u8 byMcsSsnId = cRevMsg.GetSrcSsnId();
			u8 byBrdIdx = *(cRevMsg.GetMsgBody());
			u8 byFileNum = *(cRevMsg.GetMsgBody()+sizeof(u8));
			cSendMsg.SetMsgBody((u8*)&byBrdIdx, sizeof(byBrdIdx));
			cSendMsg.CatMsgBody((u8*)&byMcsSsnId, sizeof(byMcsSsnId));
			cSendMsg.CatMsgBody((u8*)&byFileNum, sizeof(byFileNum));
			cSendMsg.CatMsgBody((u8*)(cRevMsg.GetMsgBody()+sizeof(u8)*2), cRevMsg.GetMsgBodyLen()-2*sizeof(u8));
			post( MAKEIID( AID_MCU_AGENT, 1 ), SVC_AGT_STARTUPDATEDSCVERSION_REQ, cSendMsg.GetServMsg(), cSendMsg.GetServMsgLen() );
		}
		break;
	case MCS_MCU_DSCUPDATEFILE_REQ:
		{
			CServMsg cRevMsg(pcMsg->content, pcMsg->length);
			CServMsg cSendMsg;
			u8 byMcsSsnId = cRevMsg.GetSrcSsnId();
			u8 byBrdIdx = *(cRevMsg.GetMsgBody());
			cSendMsg.SetMsgBody((u8*)&byBrdIdx, sizeof(byBrdIdx));
			cSendMsg.CatMsgBody((u8*)&byMcsSsnId, sizeof(byMcsSsnId));
			cSendMsg.CatMsgBody((u8*)(cRevMsg.GetMsgBody()+sizeof(u8)), cRevMsg.GetMsgBodyLen()-sizeof(u8));
			post( MAKEIID( AID_MCU_AGENT, 1 ), SVC_AGT_DSCUPDATEFILE_REQ, cSendMsg.GetServMsg(), cSendMsg.GetServMsgLen() );
		}
		break;
	default:
		break;
	}
    
	return;
}

/*=============================================================================
函 数 名： ProcStartUpdateDscVersionRsp
功    能： 
算法实现： 
全局变量： 
参    数： const CMessage* pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/25   4.0			周广程                  创建
=============================================================================*/
void CMcuCfgInst::ProcStartUpdateDscVersionRsp( const CMessage* pcMsg )
{
	CServMsg cRevMsg(pcMsg->content, pcMsg->length);
	u8 byBrdIdx = *(cRevMsg.GetMsgBody());
	u8 byMcsSsnId = *(cRevMsg.GetMsgBody()+sizeof(u8));
	CServMsg cSendMsg;
	cSendMsg.SetSrcSsnId(byMcsSsnId);
	cSendMsg.SetMsgBody( (u8*)&byBrdIdx, sizeof(byBrdIdx) );
	cSendMsg.CatMsgBody( (u8*)(cRevMsg.GetMsgBody()+sizeof(u8)*2), cRevMsg.GetMsgBodyLen()-sizeof(u8)*2 );
	if ( AGT_SVC_STARTUPDATEDSCVERSION_ACK == pcMsg->event )
	{
		SendMsg2Mcsssn(cSendMsg, MCU_MCS_STARTUPDATEDSCVERSION_ACK);
	}
	else
	{
		SendMsg2Mcsssn(cSendMsg, MCU_MCS_STARTUPDATEDSCVERSION_NACK);
	}
}

/*=============================================================================
函 数 名： ProcDscUpdateFileRsp
功    能： 
算法实现： 
全局变量： 
参    数： const CMessage* pcMsg
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/25   4.0			周广程                  创建
=============================================================================*/
void  CMcuCfgInst::ProcDscUpdateFileRsp( const CMessage* pcMsg )
{
	CServMsg cRevMsg(pcMsg->content, pcMsg->length);
	u8 byBrdIdx = *(cRevMsg.GetMsgBody());
	u8 byMcsSsnId = *(cRevMsg.GetMsgBody()+sizeof(u8));
	CServMsg cSendMsg;
	cSendMsg.SetSrcSsnId(byMcsSsnId);
	cSendMsg.SetMsgBody( (u8*)&byBrdIdx, sizeof(byBrdIdx) );
	cSendMsg.CatMsgBody( (u8*)(cRevMsg.GetMsgBody()+sizeof(u8)*2), cRevMsg.GetMsgBodyLen()-sizeof(u8)*2 );
	if ( AGT_SVC_DSCUPDATEFILE_ACK == pcMsg->event )
	{
		SendMsg2Mcsssn(cSendMsg, MCU_MCS_DSCUPDATEFILE_ACK);
	}
	else
	{
		SendMsg2Mcsssn(cSendMsg, MCU_MCS_DSCUPDATEFILE_NACK);
	}
}

/*=============================================================================
  函 数 名： ProcCpuPercentage
  功    能： mcu 的cpu占有率
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void   
=============================================================================*/
void  CMcuCfgInst::ProcCpuPercentage(const CMessage * pcMsg)
{
	u8 byPercentage = g_cMcuAgent.GetCpuRate();
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	if(MCS_MCU_CPUPERCENTAGE_REQ == pcMsg->event)
	{
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);//ack
	}
	
	CfgLog("cpu percentage :%d%\n", byPercentage);
	
	//notify this mcs
	cServMsg.SetMsgBody(&byPercentage, sizeof(byPercentage));
	SendMsg2Mcsssn(cServMsg, MCU_MCS_CPUPERCENTAGE_NOTIF);
	
	return;
}

/*=============================================================================
  函 数 名： ProcMcuEqpCfg
  功    能： mcu设备配置信息
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void    
=============================================================================*/
void  CMcuCfgInst::ProcMcuEqpCfg(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u16 wRet = SUCCESS_AGENT;

	//get eqp cfg
	if (MCS_MCU_GETMCUEQPCFG_REQ == pcMsg->event)
	{
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);  //ack
	}
	//change eqp cfg
	else
	{
#ifdef _MINIMCU_
		if ( STATE_IDLE == CurState() )
		{
			memcpy( m_abyServMsgHead, &cServMsg, SERV_MSGHEAD_LEN );
			NEXTSTATE(STATE_CFGING);
		}
		else
		{
			CServMsg cTempMsg;
			memcpy( &cTempMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
			if ( cServMsg.GetSrcSsnId() != cTempMsg.GetSrcSsnId() )
			{
				CfgLog("[ProcMcuEqpCfg] Mcu is cfging, waite a moment!\n");
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_CONFLICT_WITHOTHERMCS);
				return;
			}
		}
#endif

		if (sizeof(TMcuEqpCfg) != cServMsg.GetMsgBodyLen())
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MCUEQP)
			CfgLog("[ProcMcuEqpCfg] mcueqp body is invalid!\n");
			return;
		}

		TMcuEqpCfg *ptEqpCfg = (TMcuEqpCfg *)cServMsg.GetMsgBody();
		u8 byLocalLayer = 0;
		u8 byLocalSlot  = 0;
#ifndef _MINIMCU_
		if (0 == ptEqpCfg->GetMcuIpAddr())
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MCUEQP)
			CfgLog("[ProcMcuEqpCfg] mcu ip can't be 0!\n");
			return;
		} 
#endif		

		CfgLog("\n\nThe MpcIp: 0x%xd, McuIp: 0x%xd\n\n", 
			   ntohl(g_cMcuAgent.GetMpcIp()), ptEqpCfg->GetMcuIpAddr());

		g_cMcuAgent.GetLocalLayer(byLocalLayer);
		g_cMcuAgent.GetLocalSlot(byLocalSlot);
		if (g_cMSSsnApp.IsActiveBoard()  && 
			ptEqpCfg->GetLayer() == byLocalLayer && 
			ptEqpCfg->GetSlot() == byLocalSlot)
		{
			if (ntohl(g_cMcuAgent.GetMpcIp()) != ptEqpCfg->GetMcuIpAddr()
				|| g_cMcuAgent.GetInterface() != ptEqpCfg->GetInterface() )
			{
#ifndef _MINIMCU_
				wRet = g_cMcuAgent.SetMpcIp(htonl(ptEqpCfg->GetMcuIpAddr()), ptEqpCfg->GetInterface());
				if (SUCCESS_AGENT != wRet)
				{
					CfgLog("[ProcMcuEqpCfg] set mcu ip addr failed, ret:%d, McuIp:0x%x!\n", 
							wRet, ptEqpCfg->GetMcuIpAddr());
				}
#endif
			}

			if (ntohl(g_cMcuAgent.GetMaskIp()) != ptEqpCfg->GetMcuSubNetMask()
				|| g_cMcuAgent.GetInterface() != ptEqpCfg->GetInterface() )
			{
#ifndef _MINIMCU_
				wRet = g_cMcuAgent.SetMaskIp(htonl(ptEqpCfg->GetMcuSubNetMask()), ptEqpCfg->GetInterface());
				if (SUCCESS_AGENT != wRet)
				{
					CfgLog("[ProcMcuEqpCfg] set mcu mask addr failed, ret:%d, McuMask:0x%x!\n", 
							wRet, ptEqpCfg->GetMcuSubNetMask());
				}
#endif
			}

			if (ntohl(g_cMcuAgent.GetGateway()) != ptEqpCfg->GetGWIpAddr())
			{
				wRet = g_cMcuAgent.SetGateway(htonl(ptEqpCfg->GetGWIpAddr()));
				if (SUCCESS_AGENT != wRet)
				{
					CfgLog("[ProcMcuEqpCfg] set mcu gateway addr failed, ret:%d, McuGW:0x%x!\n", 
							wRet, ptEqpCfg->GetGWIpAddr());
				}
			}

			if( g_cMcuAgent.GetInterface() != ptEqpCfg->GetInterface() )
			{
				wRet = g_cMcuAgent.SetInterface( ptEqpCfg->GetInterface() );
				if (SUCCESS_AGENT != wRet)
				{
					CfgLog("[ProcMcuEqpCfg] set mcu interface failed, ret:%d, Interface: %d!\n", 
							wRet, ptEqpCfg->GetInterface());
				}
			}
		}
		if (g_bPrintCfgMsg)
		{
			CfgLog("[ProcMcuEqpCfg] receive changing config info as follow:\n");
			ptEqpCfg->Print();
		}

		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);
		NEXTSTATE(STATE_IDLE);
	}
     
	//notify all mcs
	NotifyMcsEqpCfg();
	
	return;
}

/*=============================================================================
  函 数 名： ProcBrdCfg
  功    能： 单板配置信息
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void    
=============================================================================*/
void  CMcuCfgInst::ProcBrdCfg(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	if (MCS_MCU_GETBRDCFG_REQ == pcMsg->event)
	{
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);    //ack   
	}
	//change brd cfg info
	else
	{
#ifdef _MINIMCU_
		if ( STATE_IDLE == CurState() )
		{
			memcpy( m_abyServMsgHead, &cServMsg, SERV_MSGHEAD_LEN );
			NEXTSTATE(STATE_CFGING);
		}
		else
		{
			CServMsg cTempMsg;
			memcpy( &cTempMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
			if ( cServMsg.GetSrcSsnId() != cTempMsg.GetSrcSsnId() )
			{
				CfgLog("[ProcBrdCfg] Mcu is cfging, waite a moment!\n");
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_CONFLICT_WITHOTHERMCS);
				return;
			}
		}
#endif

		if ( !BrdCfgProcIntegration( cServMsg ) )
		{
			OspPrintf(TRUE,FALSE, "[ProcBrdCfg] Brd cfg proc error!\n");
			return;
		}
		else
		{
#ifndef _MINIMCU_
		//ack
			SendMsg2Mcsssn(cServMsg, pcMsg->event+1);
#endif
		}
    }
	//notify all mcs
	NotifyMcsBrdCfg();

	return;
}
/*
void  CMcuCfgInst::ProcBrdCfg(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	u8 *pbyMsg = cServMsg.GetMsgBody();
	u16 wMsgLen = cServMsg.GetMsgBodyLen();
	u16 wRet = 0;

	u16 wBrdLen = 0;
	u16 wMixLen = 0;
	u16 wRecLen = 0;
	u16 wBasLen = 0;
	u16 wTWLen = 0;
	u16 wVmpLen = 0;
	u16 wPrsLen = 0;
	u16 wMtwLen = 0;

	u8 byBrdNum = 0;
	u8 byMixNum = 0;
	u8 byRecNum = 0;
	u8 byBasNum = 0;
	u8 byTWNum = 0;
	u8 byVmpNum = 0;
	u8 byPrsNum = 0;
	u8 byMtwNum = 0;

	BOOL32 bIsExistDsc = FALSE;
	u32 dwExpDscIp = 0; // 记录DSC的外部IP, zgc, 2007-07-04

	s32 nIndex = 0;

	//get brd cfg info
	if (MCS_MCU_GETBRDCFG_REQ == pcMsg->event)
	{
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);    //ack   
	}
	//change brd cfg info
	else
	{
#ifdef _MINIMCU_
		// debug, zgc, 2007-07-03
		if ( STATE_IDLE == CurState() )
		{
			memcpy( m_abyServMsgHead, &cServMsg, SERV_MSGHEAD_LEN );
			NEXTSTATE(STATE_CFGING);
		}
		else
		{
			CServMsg cTempMsg;
			memcpy( &cTempMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
			if ( cServMsg.GetSrcSsnId() != cTempMsg.GetSrcSsnId() )
			{
				CfgLog("[ProcBrdCfg] Mcu is cfging, waite a moment!\n");
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_CONFLICT_WITHOTHERMCS);
				return;
			}
		}
#endif

		TBrdCfgInfo *ptBrdCfg = NULL;
		TEqpMixerCfgInfo *ptMixerCfg = NULL;
		TEqpRecCfgInfo *ptRecCfg = NULL;
		TEqpTvWallCfgInfo *ptTWCfg = NULL;
		TEqpBasCfgInfo *ptBasCfg = NULL;
		TEqpVmpCfgInfo * ptVmpCfg = NULL;
		TPrsCfgInfo *ptPrsCfg = NULL;
		TEqpMTvwallCfgInfo *ptMtwCfg = NULL;

        
        //mpc板索引
        u8 byMpcIndex = 0;

		if (g_bPrintCfgMsg)
		{
			CfgLog("[ProcBrdCfg] receive changing config info as follow:\n");
		}

		//brd info
		byBrdNum = *pbyMsg;
		wBrdLen = sizeof(u8) +byBrdNum*sizeof(TBrdCfgInfo);
		if (0 == byBrdNum || byBrdNum > MAXNUM_BOARD || wMsgLen < wBrdLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
			return;
		}
		else
		{
			ptBrdCfg = (TBrdCfgInfo *)(pbyMsg + sizeof(u8));
			for (nIndex = 0; nIndex < byBrdNum; nIndex++)            
			{
				if (g_bPrintCfgMsg)
				{
					ptBrdCfg[nIndex].Print();
				}  
                
                if(BRD_TYPE_MPC == ptBrdCfg[nIndex].GetType())
                {
                    byMpcIndex = ptBrdCfg[nIndex].GetIndex();
                }

				u8 bySlot, bySlotType;
				ptBrdCfg[nIndex].GetSlot(bySlot, bySlotType);
				if (ptBrdCfg[nIndex].GetLayer() > MAXNUM_LAYER || 
					bySlot > MAXNUM_SLOT || 
					0 == ptBrdCfg[nIndex].GetIpAddr())
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					CfgLog("[ProcBrdCfg] brd info is invalid!\n");
					return;
				}

#ifdef _MINIMCU_
				// 通知MCU代理 DSC的IP, zgc, 2007-03-09
				u8 byBrdType = ptBrdCfg[nIndex].GetType();
				if( BRD_TYPE_DSC == byBrdType ||
					BRD_TYPE_MDSC == byBrdType ||
					BRD_TYPE_HDSC == byBrdType )
				{
					bIsExistDsc = TRUE;
					dwExpDscIp = ptBrdCfg[nIndex].GetIpAddr();
				}
#endif
			}
		}

		//mixer info
		byMixNum = *(pbyMsg + wBrdLen);
		wMixLen = sizeof(u8) + byMixNum*sizeof(TEqpMixerCfgInfo);
		if (byMixNum > MAXNUM_PERIEQP || wMsgLen-wBrdLen < wMixLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MIXER)
			CfgLog("[ProcBrdCfg] mixer body is invalid!\n");
			return;
		}
		else if (byMixNum > 0)
		{
			ptMixerCfg = (TEqpMixerCfgInfo *)(pbyMsg + wBrdLen + sizeof(u8));
			for (nIndex = 0; nIndex < byMixNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptMixerCfg[nIndex].Print();
				}                

				if (ptMixerCfg[nIndex].GetEqpId() < MIXERID_MIN || 
					ptMixerCfg[nIndex].GetEqpId() > MIXERID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMixerCfg[nIndex].GetSwitchBrdIndex()) || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMixerCfg[nIndex].GetRunningBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MIXER)
					CfgLog("[ProcBrdCfg] mixer info is invalid!\n");
					return;
				}

                //只有8000B的mcu才能在mpc板上配置mixer
#ifndef _MINIMCU_
                if(ptMixerCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
                {
                    MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MIXER)
                    CfgLog("[ProcBrdCfg] only 8000B mcu can run mixer on mpc, mpc Index :%d!\n", byMpcIndex);
                    return;
                }
#endif
				//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptMixerCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MIXER)
                    CfgLog("[ProcBrdCfg] mix%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptMixerCfg[nIndex].GetEqpId(), byMpcIndex);
                    return;
				}
#endif
			}
		}

		//rec info
		byRecNum = *(pbyMsg + wBrdLen + wMixLen);
		wRecLen = sizeof(u8) + byRecNum*sizeof(TEqpRecCfgInfo);
		if (byRecNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLen-wMixLen < wRecLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_REC)
			CfgLog("[ProcBrdCfg] rec body is invalid!\n");
			return;
		}
		else if (byRecNum > 0)
		{
			ptRecCfg = (TEqpRecCfgInfo *)(pbyMsg + wBrdLen + wMixLen + sizeof(u8));
			for (nIndex = 0; nIndex < byRecNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptRecCfg[nIndex].Print();
				}                

				if (ptRecCfg[nIndex].GetEqpId() < RECORDERID_MIN || 
					ptRecCfg[nIndex].GetEqpId() > RECORDERID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptRecCfg[nIndex].GetSwitchBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_REC)
					CfgLog("[ProcBrdCfg] rec info is invalid!\n");
					return;
				}
				//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptRecCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_REC)
                    CfgLog("[ProcBrdCfg] rec%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptRecCfg[nIndex].GetEqpId(), byMpcIndex);
                    return;
				}
#endif
			}
		}

		//tvwall info
		byTWNum = *(pbyMsg + wBrdLen + wMixLen + wRecLen);
		wTWLen = sizeof(u8) + byTWNum*sizeof(TEqpTvWallCfgInfo);
		if (byTWNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLen-wMixLen-wRecLen < wTWLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_TVWALL)
			CfgLog("[ProcBrdCfg] tvwall body is invalid!\n");
			return;
		}
		else if (byTWNum > 0)
		{
			ptTWCfg = (TEqpTvWallCfgInfo *)(pbyMsg + wBrdLen + wMixLen + wRecLen + sizeof(u8));
			for (nIndex = 0; nIndex < byTWNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptTWCfg[nIndex].Print();
				}                

				if (ptTWCfg[nIndex].GetEqpId() < TVWALLID_MIN || 
					ptTWCfg[nIndex].GetEqpId() > TVWALLID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptTWCfg[nIndex].GetRunningBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_TVWALL)
					CfgLog("[ProcBrdCfg] tvwall info is invalid!\n");
					return;
				}
				//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptTWCfg[nIndex].GetRunningBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_TVWALL)
                    CfgLog("[ProcBrdCfg] tvwall%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptTWCfg[nIndex].GetEqpId(), byMpcIndex);
                    return;
				}
#endif
			}
		}

		//bas info
		byBasNum = *(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen);
		wBasLen = sizeof(u8) + byBasNum*sizeof(TEqpBasCfgInfo);
		if (byBasNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLen-wMixLen-wRecLen-wTWLen < wBasLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BAS)
			CfgLog("[ProcBrdCfg] bas body is invalid!\n");
			return;
		}
		else if (byBasNum > 0)
		{
			ptBasCfg = (TEqpBasCfgInfo *)(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + sizeof(u8));
			for (nIndex = 0; nIndex < byBasNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptBasCfg[nIndex].Print();
				}

				if (ptBasCfg[nIndex].GetEqpId() < BASID_MIN || 
					ptBasCfg[nIndex].GetEqpId() > BASID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptBasCfg[nIndex].GetRunningBrdIndex()) || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptBasCfg[nIndex].GetSwitchBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BAS)
					CfgLog("[ProcBrdCfg] bas info is invalid!\n");
					return;
				}

                //只有8000B的mcu才能在mpc板上配置bas
#ifndef _MINIMCU_
                if(ptBasCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
                {
                    MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BAS)
                    CfgLog("[ProcBrdCfg] only 8000B mcu can run bas on mpc, mpc Index :%d!\n", byMpcIndex);
                    return;
                }
#endif
				//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptBasCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BAS)
                    CfgLog("[ProcBrdCfg] bas%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptBasCfg[nIndex].GetEqpId(), byMpcIndex);
                    return;
				}
#endif
			}
		}

		//vmp info
		byVmpNum = *(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + wBasLen);
		wVmpLen = sizeof(u8) + byVmpNum*sizeof(TEqpVmpCfgInfo);
		if (byVmpNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLen-wMixLen-wRecLen-wTWLen-wBasLen < wVmpLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_VMP)
			CfgLog("[ProcBrdCfg] vmp body is invalid!\n");
			return;
		}
		else if (byVmpNum > 0)
		{
			ptVmpCfg = (TEqpVmpCfgInfo *)(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + wBasLen + sizeof(u8));
			for (nIndex = 0; nIndex < byVmpNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptVmpCfg[nIndex].Print();
				}

				if (ptVmpCfg[nIndex].GetEqpId() < VMPID_MIN || 
					ptVmpCfg[nIndex].GetEqpId() > VMPID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptVmpCfg[nIndex].GetRunningBrdIndex()) || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptVmpCfg[nIndex].GetSwitchBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_VMP)
					CfgLog("[ProcBrdCfg] vmp info is invalid!\n");
					return;
				}

                //只有8000B的mcu才能在mpc板上配置vmp
#ifndef _MINIMCU_
                if(ptVmpCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
                {
                    MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_VMP)
                    CfgLog("[ProcBrdCfg] only 8000B mcu can run vmp on mpc, mpc Index :%d!\n", byMpcIndex);
                    return;
                }
#endif
				//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptVmpCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_VMP)
                    CfgLog("[ProcBrdCfg] vmp%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptVmpCfg[nIndex].GetEqpId(), byMpcIndex);
                    return;
				}
#endif
			}
		}

		//prs info
		byPrsNum = *(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + wBasLen + wVmpLen);
		wPrsLen = sizeof(u8) + byPrsNum*sizeof(TPrsCfgInfo);
		if (byPrsNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLen-wMixLen-wRecLen-wTWLen-wBasLen-wVmpLen < wPrsLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_PRS)
			CfgLog("[ProcBrdCfg] prs body is invalid!\n");
			return;
		}
		else if (byPrsNum > 0)
		{
			ptPrsCfg = (TPrsCfgInfo *)(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + wBasLen + wVmpLen + sizeof(u8));
			for (nIndex = 0; nIndex < byPrsNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptPrsCfg[nIndex].Print();
				}

				if (ptPrsCfg[nIndex].GetEqpId() < PRSID_MIN || 
					ptPrsCfg[nIndex].GetEqpId() > PRSID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptPrsCfg[nIndex].GetRunningBrdIndex()) || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptPrsCfg[nIndex].GetSwitchBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_PRS)
					CfgLog("[ProcBrdCfg] prs info is invalid!\n");
					return;
				}

                //只有8000B的mcu才能在mpc板上配置prs
#ifndef _MINIMCU_
                if(ptPrsCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
                {
                    MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_PRS)
                    CfgLog("[ProcBrdCfg] only 8000B mcu can run prs on mpc, mpc Index :%d!\n", byMpcIndex);
                    return;
                }
#endif
				//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptPrsCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_PRS)
                    CfgLog("[ProcBrdCfg] prs%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptPrsCfg[nIndex].GetEqpId(), byMpcIndex);
                    return;
				}
#endif
			}
		}

		byMtwNum = *(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + wBasLen + wVmpLen + wPrsLen);
		wMtwLen = sizeof(u8) + byMtwNum*sizeof(TEqpMTvwallCfgInfo);
		if (byMtwNum > MAXNUM_PERIEQP || wMsgLen-wBrdLen-wMixLen-wRecLen-wTWLen-wBasLen-wVmpLen-wPrsLen < wMtwLen)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MTVWALL)
			CfgLog("[ProcBrdCfg] mtw body is invalid!\n");
			return;
		}   
		else if (byMtwNum > 0)
		{
			ptMtwCfg = (TEqpMTvwallCfgInfo *)(pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + wBasLen + wVmpLen + wPrsLen + sizeof(u8));
			for (nIndex = 0; nIndex < byMtwNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptMtwCfg[nIndex].Print();
				}

				if (ptMtwCfg[nIndex].GetEqpId() < VMPTWID_MIN || 
					ptMtwCfg[nIndex].GetEqpId() > VMPTWID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMtwCfg[nIndex].GetRunningBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MTVWALL)
					CfgLog("[ProcBrdCfg] mtw info is invalid!\n");
					return;
				}
				//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptMtwCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MTVWALL)
                    CfgLog("[ProcBrdCfg] mtw%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptMtwCfg[nIndex].GetEqpId(), byMpcIndex);
                    return;
				}
#endif
			}
		}

// 变更消息体，TDSCModuleInfo结构从MCS_MCU_GETMCUGENERALCFG_REQ消息中转移到MCS_MCU_GETBRDCFG_REQ, zgc, 2007-03-15
#ifdef _MINIMCU_	
		T8000BDscMod tDscInfo = *(T8000BDscMod*)( pbyMsg + wBrdLen + wMixLen + wRecLen + wTWLen + wBasLen + wVmpLen + wPrsLen + wMtwLen );	
		TDSCModuleInfo tAgentDscInfo;
		DscInfoIn2Out( tDscInfo, tAgentDscInfo );
		if( g_bPrintCfgMsg )
		{
			OspPrintf(TRUE, FALSE, "T8000BDscMod:\n");
			tDscInfo.Print();
			OspPrintf(TRUE, FALSE, "TDSCModuleInfo:\n");
			tAgentDscInfo.Print();
		}
		BOOL32 bIsRegDsc = FALSE;
		if ( g_cMcuAgent.IsRegedBoard( g_cMcuAgent.GetConfigedDscType(), 0, 0 ) )
		{
			bIsRegDsc = TRUE;
		}
#endif

		//change brd info        
		TBoardInfo atBrdInfo[MAXNUM_BOARD]; 
		BrdCfgIn2Out(ptBrdCfg, atBrdInfo, byBrdNum);
		wRet = g_cMcuAgent.WriteBrdTable(byBrdNum, atBrdInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_BRD)
			CfgLog("[ProcBrdCfg] write brd info failed, ret:%d!\n", wRet);
			return;
		}

		//change mixer info   
		TEqpMixerInfo atMixerInfo[MAXNUM_PERIEQP];
		if (NULL != ptMixerCfg)
		{
			MixerCfgIn2Out(ptMixerCfg, atMixerInfo, byMixNum);            
		}
		else
		{
			byMixNum = 0;
		}
		wRet = g_cMcuAgent.WriteMixerTable(byMixNum, atMixerInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_MIXER)
			CfgLog("[ProcBrdCfg] write mixer info failed, ret:%d!\n", wRet);
			return;
		}

		//change rec info
		TEqpRecInfo atRecInfo[MAXNUM_PERIEQP];
		if (NULL != ptRecCfg)
		{
			RecCfgIn2Out(ptRecCfg, atRecInfo, byRecNum);            
		}     
		else
		{
			byRecNum = 0;
		}
		wRet = g_cMcuAgent.WriteRecTable(byRecNum, atRecInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_REC)
			CfgLog("[ProcBrdCfg] write rec info failed, ret:%d!\n", wRet);
			return;
		}

		//change tvwall info
		TEqpTVWallInfo atTWInfo[MAXNUM_PERIEQP];
		if (NULL != ptTWCfg)
		{
			TWCfgIn2Out(ptTWCfg, atTWInfo, byTWNum);            
		}
		else
		{
			byTWNum = 0;
		}
		wRet = g_cMcuAgent.WriteTvTable(byTWNum, atTWInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_TVWALL)
			CfgLog("[ProcBrdCfg] write tvwall info failed, ret:%d!\n", wRet);
			return;
		}

		//change bas info
		TEqpBasInfo atBasInfo[MAXNUM_PERIEQP];
		if (NULL != ptBasCfg)
		{
			BasCfgIn2Out(ptBasCfg, atBasInfo, byBasNum);            
		}
		else
		{
			byBasNum = 0;
		}
		wRet = g_cMcuAgent.WriteBasTable(byBasNum, atBasInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_BAS)
			CfgLog("[ProcBrdCfg] write bas info failed, ret:%d!\n", wRet);
			return;
		}

		//change vmp info
		TEqpVMPInfo atVmpInfo[MAXNUM_PERIEQP];
		if (NULL != ptVmpCfg)
		{
			VmpCfgIn2Out(ptVmpCfg, atVmpInfo, byVmpNum);            
		}
		else
		{
			byVmpNum = 0;
		}
		wRet = g_cMcuAgent.WriteVmpTable(byVmpNum, atVmpInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_VMP)

			CfgLog("[ProcBrdCfg] write vmp info failed, ret:%d!\n", wRet);
			return;
		}

		//change prs info
		TEqpPrsInfo atPrsInfo[MAXNUM_PERIEQP];
		if (NULL != ptPrsCfg)
		{
			PrsCfgIn2Out(ptPrsCfg, atPrsInfo, byPrsNum);            
		}    
		else
		{
			byPrsNum = 0;
		}

        wRet = g_cMcuAgent.WritePrsTable(byPrsNum, atPrsInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_PRS)
			CfgLog("[ProcBrdCfg] write prs info failed, ret:%d!\n", wRet);
			return;
		}

		//change mtw info
		TEqpMpwInfo atMtwInfo[MAXNUM_PERIEQP];
		if (NULL != ptMtwCfg)
		{              
			MtwCfgIn2Out(ptMtwCfg, atMtwInfo, byMtwNum);            
		}
		else
		{
			byMtwNum = 0;
		}
		wRet = g_cMcuAgent.WriteMpwTable(byMtwNum, atMtwInfo);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_MTVWALL)

			CfgLog("[ProcBrdCfg] write mtw info failed, ret:%d!\n", wRet);
			return;
		}

// 变更消息体，TDSCModuleInfo结构从MCS_MCU_GETMCUGENERALCFG_REQ消息中转移到MCS_MCU_GETBRDCFG_REQ, zgc, 2007-03-15
#ifdef _MINIMCU_
		if ( bIsExistDsc )
		{
			TDSCModuleInfo tLocalDscInfo;
			g_cMcuAgent.GetDscInfo( &tLocalDscInfo );
			u32 dwDscInnerIp = tLocalDscInfo.GetDscInnerIp();
			u32 dwMcuInnerIp = tLocalDscInfo.GetMcuInnerIp();
			u32 dwInnerIpMask = tLocalDscInfo.GetInnerIpMask();
			u32 dwNewCfgIp1 = 0;
			u32 dwNewCfgIp2 = 0;
			u32 dwNewCfgIpMask1 = 0;
			u32 dwNewCfgIpMask2 = 0;
			u32 dwNewCfgGWIp1 = 0;
			u8 byIpMaskBitNum = 0;
			u8 byCallAddrNum = tAgentDscInfo.GetCallAddrNum();
			u8 byMcsAccessAddrNum = tAgentDscInfo.GetMcsAccessAddrNum();
			u8 byLop = 0;
			u8 byLop1 = 0;
			u8 byLop2 = 0;

			TMINIMCUNetParamAll tNetParamAll;
			TMINIMCUNetParam tNetParam;
			
			if ( tAgentDscInfo.GetNetType() == NETTYPE_INVALID )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
				CfgLog("[ProcBrdCfg] NetType INVALID!\n");
				return;
			}
			
			tAgentDscInfo.GetCallAddrAll( tNetParamAll );
			if ( !tNetParamAll.IsValid() )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
				CfgLog("[ProcBrdCfg] DSC module ip num error! McsAccessAddrNum.%d, CallAddrNum.%d\n", byMcsAccessAddrNum, byCallAddrNum);
				return;
			}
			memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
			tAgentDscInfo.GetMcsAccessAddrAll( tNetParamAll );
			if ( !tNetParamAll.IsValid() )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
				CfgLog("[ProcBrdCfg] DSC module ip num error! McsAccessAddrNum.%d, CallAddrNum.%d\n", byMcsAccessAddrNum, byCallAddrNum);
				return;
			}
			// IP数不能为0
			if ( 0 == byMcsAccessAddrNum || MAXNUM_MCSACCESSADDR < byMcsAccessAddrNum 
				|| 0 == byCallAddrNum || ETH_IP_MAXNUM-1 < byCallAddrNum )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
				CfgLog("[ProcBrdCfg] DSC module ip num error! McsAccessAddrNum.%d, CallAddrNum.%d\n", byMcsAccessAddrNum, byCallAddrNum);
				return;
			}

			tAgentDscInfo.GetMcsAccessAddrAll( tNetParamAll );
			// 检验会控连接地址是否彼此冲突, zgc, 2007-07-17
			// 检验会控连接地址和网关地址是否在同一个网段, zgc, 2007-08-23
			for ( byLop1 = 0; byLop1 < byMcsAccessAddrNum; byLop1++ )
			{
				if ( !tNetParamAll.GetNetParambyIdx( byLop1, tNetParam ) )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					CfgLog("[ProcBrdCfg] Get McsAccessAddr.%d from new DSC module cfg error!\n", byLop1);
					return;
				}
				if ( !tNetParam.IsValid() )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					CfgLog("[ProcBrdCfg] McsAccessAddr.%d is invalid!\n", byLop1);
					return;
				}
				
				dwNewCfgIp1 = tNetParam.GetIpAddr();
				dwNewCfgIpMask1 = tNetParam.GetIpMask();
				dwNewCfgGWIp1 = tNetParam.GetGatewayIp();
				// IPMASK不能大于30位,不能全0,且不能0,1间隔
				if ( !IsIpMaskValid(dwNewCfgIpMask1) )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					CfgLog("[ProcBrdCfg] Ip0x%x: IpMask is invalid!\n", dwNewCfgIp1);
					return;
				}
				// 子网IP不能全0或全1
				dwNewCfgIpMask1 = ~dwNewCfgIpMask1;
				if ( (dwNewCfgIp1&dwNewCfgIpMask1) == 0 
					|| (dwNewCfgIp1&dwNewCfgIpMask1) == dwNewCfgIpMask1 )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					CfgLog("[ProcBrdCfg] subnet ip.0x%x can not be network or broadcast!\n", dwNewCfgIp1);
					return;
				}
				dwNewCfgIpMask1 = ~dwNewCfgIpMask1;
				//公网IP和网关应该在同一网段
				if ( tNetParam.IsWan()
					&& (dwNewCfgIp1&dwNewCfgIpMask1) != (dwNewCfgGWIp1&dwNewCfgIpMask1)
				   )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					CfgLog("[ProcBrdCfg]WAN ip.0x%x should be in same subnet with GWIp!\n", dwNewCfgIp1);
					return;
				}

				for ( byLop2 = byLop1+1; byLop2 < byMcsAccessAddrNum; byLop2++ )
				{
					if ( !tNetParamAll.GetNetParambyIdx( byLop2, tNetParam ) )
					{
						MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
						CfgLog("[ProcBrdCfg] Get McsAccessAddr.%d from new DSC module cfg error!\n", byLop2);
						return;
					}
					dwNewCfgIp2 = tNetParam.GetIpAddr();
					dwNewCfgIpMask2 = tNetParam.GetIpMask();
					if ( 
						(dwNewCfgIp1&dwNewCfgIpMask1) == (dwNewCfgIp2&dwNewCfgIpMask1) 
						|| (dwNewCfgIp1&dwNewCfgIpMask2) == (dwNewCfgIp2&dwNewCfgIpMask2) 
					   )
					{
						MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
						CfgLog("[ProcBrdCfg] Mcs access address ip.0x%x conflict to ip.0x%x!\n ",
								dwNewCfgIp1, dwNewCfgIp2);
						return;
					}
				}
			}

			// 检验呼叫地址是否彼此冲突(包括内口IP), zgc, 2007-07-17
			memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
			tAgentDscInfo.GetCallAddrAll(tNetParamAll);
			for ( byLop1 = 0; byLop1 < byCallAddrNum; byLop1++ )
			{
				if ( !tNetParamAll.GetNetParambyIdx(byLop1, tNetParam) )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					CfgLog("[ProcBrdCfg] Get CallAddr.%d from new DSC module cfg error!\n", byLop1);
					return;
				}
				if ( !tNetParam.IsValid() )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					CfgLog("[ProcBrdCfg] Ip.%d can not be 0!\n",byLop1);
					return;
				}

				dwNewCfgIp1 = tNetParam.GetIpAddr();
				dwNewCfgIpMask1 = tNetParam.GetIpMask();
				dwNewCfgGWIp1 = tNetParam.GetGatewayIp();			
				// IPMASK不能大于30位,不能全0,且不能0,1间隔
				if ( !IsIpMaskValid(dwNewCfgIpMask1) )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					CfgLog("[ProcBrdCfg] Ip0x%x: IpMask is invalid!\n", dwNewCfgIp1);
					return;
				}
				// 子网IP不能全0或全1
				dwNewCfgIpMask1 = ~dwNewCfgIpMask1;
				if ( (dwNewCfgIp1&dwNewCfgIpMask1) == 0 
					|| (dwNewCfgIp1&dwNewCfgIpMask1) == dwNewCfgIpMask1 )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					CfgLog("[ProcBrdCfg] subnet ip.0x%x can not be network or broadcast!\n", dwNewCfgIp1);
					return;
				}
				dwNewCfgIpMask1 = ~dwNewCfgIpMask1;
				//公网IP和网关应该在同一网段
				if ( tNetParam.IsWan()
					&& (dwNewCfgIp1&dwNewCfgIpMask1) != (dwNewCfgGWIp1&dwNewCfgIpMask1)
				   )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					CfgLog("[ProcBrdCfg]WAN ip.0x%x should be in same subnet with GWIp!\n", dwNewCfgIp1);
					return;
				}

				for ( byLop2 = byLop1+1; byLop2 < byCallAddrNum; byLop2++ )
				{
					if ( !tNetParamAll.GetNetParambyIdx(byLop2, tNetParam) )
					{
						MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
						CfgLog("[ProcBrdCfg]Get CallAddr.%d from new DSC module cfg error!\n",byLop2);
						return;
					}
					dwNewCfgIp2 = tNetParam.GetIpAddr();
					dwNewCfgIpMask2 = tNetParam.GetIpMask();
					if ( 
						(dwNewCfgIp1&dwNewCfgIpMask1) == (dwNewCfgIp2&dwNewCfgIpMask1) 
						|| (dwNewCfgIp1&dwNewCfgIpMask2) == (dwNewCfgIp2&dwNewCfgIpMask2) 
					   )
					{
						MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
						CfgLog("[ProcBrdCfg] Call address ip.0x%x conflict to ip.0x%x!\n ",
								dwNewCfgIp1, dwNewCfgIp2);
						return;
					}
				}
				if ( 
					(dwNewCfgIp1&dwNewCfgIpMask1) == (dwDscInnerIp&dwNewCfgIpMask1) 
					|| (dwNewCfgIp1&dwInnerIpMask) == (dwDscInnerIp&dwInnerIpMask) 
				   )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_BRD)
					CfgLog("[ProcBrdCfg] Call address ip.0x%x conflict to ip.0x%x!\n ",
							dwNewCfgIp1, dwDscInnerIp);
					return;
				}
			}
		}
		else
		{
			if ( bIsRegDsc )
			{
				CServMsg cDisMsg;
				u8 byBrdLayer = 0;
				u8 byBrdSlot = 0;
				cDisMsg.SetMsgBody((u8*)&byBrdLayer, sizeof(byBrdLayer));
				cDisMsg.CatMsgBody((u8*)&byBrdSlot, sizeof(byBrdSlot));
				post( MAKEIID(AID_MCU_AGENT,1), SVC_AGT_DISCONNECTBRD_CMD, cDisMsg.GetServMsg(), cDisMsg.GetServMsgLen()  );
			}
		}
		
		CServMsg cDscInfoMsg;
		u8 byBrdLayer = 0;
		u8 byBrdSlot = 0;
		cDscInfoMsg.SetMsgBody((u8*)&byBrdLayer, sizeof(byBrdLayer));
		cDscInfoMsg.CatMsgBody((u8*)&byBrdSlot, sizeof(byBrdSlot));
		cDscInfoMsg.CatMsgBody((u8*)&tAgentDscInfo, sizeof(tAgentDscInfo));
		post( MAKEIID(AID_MCU_AGENT,1), SVC_AGT_SETDSCINFO_REQ, cDscInfoMsg.GetServMsg(), cDscInfoMsg.GetServMsgLen()  );

		SetTimer(MCUCFG_DSCCFG_WAITINGCHANGE_OVER_TIMER, 10000 );
		g_cMcuAgent.SetDscInfo( &tAgentDscInfo, FALSE );  //用于反馈给MCS，不修改文件

#endif
#ifndef _MINIMCU_
		//ack
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);
#endif
    }

	//notify all mcs
	NotifyMcsBrdCfg();

	return;
}
*/
/*=============================================================================
  函 数 名： CheckBrdIndex
  功    能： 检查外设运行板或交换板索引合法性
  算法实现： 
  全局变量： 
  参    数： TBrdCfgInfo * ptBrdCfg
             u8 byBrdNum
             u8 byBrdIndex  单板索引
  返 回 值： BOOL32  
=============================================================================*/
BOOL32 CMcuCfgInst::CheckBrdIndex(TBrdCfgInfo * ptBrdCfg, u8 byBrdNum, u8 byBrdIndex)
{
    if (NULL == ptBrdCfg)
    {
        return FALSE;
    }

    for (u8 byIndex = 0; byIndex < byBrdNum; byIndex++)
    {
        if (ptBrdCfg[byIndex].GetIndex() == byBrdIndex)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
函 数 名： IsValidRRQMtadpIp
功    能： 是否合法的主协议适配板
算法实现： 
全局变量： 
参    数： TMcuNetCfgInfo &tNetCfgInfo
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/14  4.0			许世林                  创建
=============================================================================*/
BOOL32  CMcuCfgInst::IsValidRRQMtadpIp(TMcuNetCfgInfo &tNetCfgInfo)
{   
    if ( (0 != tNetCfgInfo.GetGkIpAddr() && 0 == tNetCfgInfo.GetRRQMtadpIp()) || 
         (0 == tNetCfgInfo.GetGkIpAddr() && 0 != tNetCfgInfo.GetRRQMtadpIp()) )
    {
        CfgLog("[IsValidRRQMtadpIp] GKIpAddr :0x%x, RRQMtadpIpAddr :0x%x\n", 
            tNetCfgInfo.GetGkIpAddr(), tNetCfgInfo.GetRRQMtadpIp());
        return FALSE;
    }

    if (0 == tNetCfgInfo.GetRRQMtadpIp())
    {
        return TRUE;
    }

    u8 byBrdNum = 0;
    TBoardInfo atBrdInfo[MAXNUM_BOARD];
    u16 wRet = g_cMcuAgent.ReadBrdTable(&byBrdNum, atBrdInfo);
    if (SUCCESS_AGENT != wRet || 0 == byBrdNum || byBrdNum > MAXNUM_BOARD)
    {
        CfgLog("[IsValidRRQMtadpIp] read brd info failed! brd num :%d, wRet :%d\n", byBrdNum, wRet);
        return FALSE;
    }   

    for (u8 byIdx = 0; byIdx < byBrdNum; byIdx++)
    {
        switch(atBrdInfo[byIdx].GetType())
        {
        case DSL8000_BRD_MPC:
        case DSL8000_BRD_CRI:
        case DSL8000_BRD_DRI:
        case DSL8000_BRD_MDSC:
        case DSL8000_BRD_HDSC:
            if ( atBrdInfo[byIdx].GetBrdIp() == tNetCfgInfo.GetRRQMtadpIp() )
            {
                if (DSL8000_BRD_MPC == atBrdInfo[byIdx].GetType())
                {
                    if (0 != tNetCfgInfo.GetIsUseMpcStack())
                    {
                        return TRUE;
                    }                    
                }
                else
                {
                    return TRUE;
                }                
            }
            break;

        default:
            break;
        }        
    }

    return FALSE;
}

/*=============================================================================
  函 数 名： ProcMcuGeneralCfg
  功    能： mcu配置信息基本设置
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void   
=============================================================================*/
void  CMcuCfgInst::ProcMcuGeneralCfg(const CMessage * pcMsg)
{

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u8 *pbyMsg  = cServMsg.GetMsgBody();
	u16 wMsgLen = cServMsg.GetMsgBodyLen();
	u16 wRet = 0;

	//get mcu general cfg
	if (MCS_MCU_GETMCUGENERALCFG_REQ == pcMsg->event)
	{        
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);   //ack    
	}
	//change mcu general cfg
	else
	{
#ifdef _MINIMCU_
		if ( STATE_IDLE == CurState() )
		{
			memcpy( m_abyServMsgHead, &cServMsg, SERV_MSGHEAD_LEN );
			NEXTSTATE(STATE_CFGING);
		}
		else
		{
			CServMsg cTempMsg;
			memcpy( &cTempMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
			if ( cServMsg.GetSrcSsnId() != cTempMsg.GetSrcSsnId() )
			{
				CfgLog("[ProcMcuGeneralCfg] Mcu is cfging, waite a moment!\n");
				MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_CONFLICT_WITHOTHERMCS);
				return;
			}
		}
#endif

		if (wMsgLen < (sizeof(TMcuGeneralCfg)))
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MCUGEN)
			CfgLog("[ProcMcuGeneralCfg] mcu general body is invalid!\n");
			return;
		}        

		//vmp style,mcu local cfg,net cfg,qos cfg,net syn cfg,dcs cfg
		TMcuGeneralCfg tGenCfg = *(TMcuGeneralCfg *)pbyMsg;   
		
		//whether the changed cfg be in effect immediately
		u8 byInEffectNow = 0;
		byInEffectNow = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMcuGeneralCfg) );

		if (g_bPrintCfgMsg)
		{
			CfgLog("[ProcMcuGeneralCfg] receive changing config info as follow:\n");
			tGenCfg.Print();
		}

		TAgentMcuGenCfg tAgentGenCfg;
		if (!GeneralCfgIn2Out(tGenCfg, tAgentGenCfg))
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MCUGEN)
			CfgLog("[ProcMcuGeneralCfg] mcu general param is invalid!\n");
			return;
		}
		
        //需要在此消息前先处理单板配置消息!
        if (!IsValidRRQMtadpIp(tGenCfg.m_tNet))
        {
            MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_RRQMTADPIP)
            CfgLog("[ProcMcuGeneralCfg] mcu rrq mtadp addr is invalid!\n");
            return;
        }

		wRet = g_cMcuAgent.WriteVmpAttachTable(tAgentGenCfg.m_byVmpStyleNum, tAgentGenCfg.m_atVmpStyle);
		if (SUCCESS_AGENT != wRet)            
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_VMPSTYLE)
			CfgLog("[ProcMcuGeneralCfg] mcu general write vmp style failed. ret:%d!\n", wRet);
			return;
		}

		//notify vmp
		//CServMsg cVmpMsg;
		//cVmpMsg.SetEventId(MCU_VMP_SETSTYLE_CMD);
		//cVmpMsg.SetMsgBody((u8*)&tGenCfg.m_tVmpStyle, sizeof(tGenCfg.m_tVmpStyle));
		//g_cEqpSsnApp.BroadcastToAllPeriEqpSsn(MCU_VMP_SETSTYLE_CMD, cVmpMsg.GetServMsg(), cVmpMsg.GetServMsgLen());         

		//是否向gk注册
		BOOL32 bRegGk = FALSE;
		BOOL32 bNotifyMtadp = FALSE;
		TLocalInfo tOldLocal;
		wRet = g_cMcuAgent.GetLocalInfo(&tOldLocal);
		if (SUCCESS_AGENT != wRet)
		{
			OspPrintf(TRUE, FALSE, "[ProcMcuGeneralCfg] get old mcu local info failed, ret:%d!\n", wRet);
		}
		else
		{
			if (0 != strcmp(tOldLocal.GetAlias(), tAgentGenCfg.m_tLocal.GetAlias()) || 
				0 != strcmp(tOldLocal.GetE164Num(), tAgentGenCfg.m_tLocal.GetE164Num()))
			{
				bRegGk = TRUE;
			}

			if (tOldLocal.GetAudioRefreshTime() != tAgentGenCfg.m_tLocal.GetAudioRefreshTime() || 
				tOldLocal.GetVideoRefreshTime() != tAgentGenCfg.m_tLocal.GetVideoRefreshTime() || 
				tOldLocal.GetListRefreshTime()  != tAgentGenCfg.m_tLocal.GetListRefreshTime() )
			{
				bNotifyMtadp = TRUE;
			}
		}

		wRet = g_cMcuAgent.SetLocalInfo(tAgentGenCfg.m_tLocal);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_LOCALINFO)
			CfgLog("[ProcMcuGeneralCfg] mcu general write mcu local failed. ret:%d!\n", wRet);
			return;
		}

		//更新适配对终端信息请求时间间隔
		if (bNotifyMtadp)
		{
			TMtAdpCfg tCfg;
			tCfg.SetAudInfoRefreshTime(tAgentGenCfg.m_tLocal.GetAudioRefreshTime());
			tCfg.SetVidInfoRefreshTime(tAgentGenCfg.m_tLocal.GetVideoRefreshTime());
			tCfg.SetPartListRefreshTime(tAgentGenCfg.m_tLocal.GetListRefreshTime());
			CServMsg cCfgMsg;
			cCfgMsg.SetEventId(MCU_MT_MTADPCFG_CMD);
			cCfgMsg.SetMsgBody((u8*)&tCfg, sizeof(tCfg));
			g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn(MCU_MT_MTADPCFG_CMD, cCfgMsg);  
		}
        
		//更新别名信息(重新注册gk)
		if (bRegGk)
		{
			g_cMcuVcApp.RegisterConfToGK( 0, g_cMcuVcApp.GetRegGKDriId() );
            OspPost( MAKEIID(AID_MCU_VC, CInstance::DAEMON), MCU_MCUREREGISTERGK_NOITF );
		}            
		
		//GK地址在线更新策略：
		//	  (1) 新旧GK地址不相同，且需要立即生效，则发起GK地址更新命令
		//	  (2) 辅mtadp在收到新的地址后，写入本地信息中，并清空本地状态
		//	  (3) 主mtadp在收到新的地址后，注销本地的所有信息，并向MCU发起重新注册请求
		u32 dwOldGkIp = g_cMcuAgent.GetGkIpAddr();
        u32 dwRRQMtadpIp = g_cMcuAgent.GetRRQMtadpIp();

        // guzh [9/8/2006] 
		// 通知MTADP之前一定写入更新后的配置
		// 保护端口不小于终端MCU侧起始端口, zgc, 2007-06-05
		if ( MT_MCU_STARTPORT > tAgentGenCfg.m_tNet.GetRecvStartPort() )
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_NET)
			CfgLog("[ProcMcuGeneralCfg] RecvStartPort value(%d) is error!\n", tAgentGenCfg.m_tNet.GetRecvStartPort() );
			return;
		}
		wRet = g_cMcuAgent.SetNetWorkInfo(tAgentGenCfg.m_tNet);
        
		if ( dwOldGkIp != htonl(tAgentGenCfg.m_tNet.GetGkIp()) ||
             dwRRQMtadpIp != tAgentGenCfg.m_tNet.GetRRQMtadpIp() )
		{
			//是否立即生效	
			u32 dwGKIp = 0;
			if ( 1 == byInEffectNow )
			{
				//告诉所有的适配板，更新GK地址
				CServMsg cServMsg;
				cServMsg.SetEventId( MCU_MTADP_GKADDR_UPDATE_CMD );
				dwGKIp = tAgentGenCfg.m_tNet.GetGkIp();
				cServMsg.SetMsgBody( (u8*)&dwGKIp, sizeof(u32) );
                
                //zbq[07/17/2009] N+1主端为保证终端倒换，放弃实时刷新的失效性；退而依赖重启
                if (MCU_NPLUS_MASTER_CONNECTED != g_cNPlusApp.GetLocalNPlusState())
                {
                    g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn( MCU_MTADP_GKADDR_UPDATE_CMD, cServMsg );
                }
				
				//清空MCU的注册状态
				g_cMcuVcApp.SetConfRegState( 0, 0 );
				g_cMcuVcApp.SetRegGKDriId( 0 );
            }

            // guzh [7/28/2007] 无论是否立即生效都要写DSC代理
			// 让代理去修改DSC的GK info, zgc, 2007-07-21
#ifdef _MINIMCU_
			cServMsg.SetEventId( SVC_AGT_DSCGKINFO_UPDATE_CMD );
			dwGKIp = htonl(tAgentGenCfg.m_tNet.GetGkIp());
			u8 byLayer = 0;
			u8 bySlot = 0;
			cServMsg.SetMsgBody((u8*)&byLayer, sizeof(byLayer));
			cServMsg.CatMsgBody((u8*)&bySlot, sizeof(bySlot));
			cServMsg.CatMsgBody((u8*)&dwGKIp, sizeof(u32));
			post( MAKEIID(AID_MCU_AGENT,1), SVC_AGT_DSCGKINFO_UPDATE_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()  );
#endif
		}

		wRet |= g_cMcuAgent.WriteTrapTable(tAgentGenCfg.m_byTrapListNum, tAgentGenCfg.m_atTrapList);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_NET) 
			CfgLog("[ProcMcuGeneralCfg] mcu general write net failed. ret:%d!\n", wRet);
			return;
		}

		//是否更新Qos信息
		BOOL32 bUpdateQos = FALSE;
		TQosInfo tOldQos;
		wRet = g_cMcuAgent.GetQosInfo(&tOldQos);
		if (SUCCESS_AGENT != wRet)
		{
			OspPrintf(TRUE, FALSE, "[ProcMcuGeneralCfg] get old qos info failed, ret:%d!\n", wRet);
		}
		else
		{
			if (0 != memcmp(&tOldQos, &tAgentGenCfg.m_tQos, sizeof(TQosInfo)))
			{
				bUpdateQos = TRUE;
			}
		}

		wRet = g_cMcuAgent.SetQosInfo(tAgentGenCfg.m_tQos);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_QOS)
			CfgLog("[ProcMcuGeneralCfg] mcu general write qos failed. ret:%d!\n", wRet);
			return;
		}

		//notify mtadp and eqp 
		if (bUpdateQos)
		{
			CServMsg cQosMsg;
			cQosMsg.SetEventId(MCU_MT_SETQOS_CMD);
			cQosMsg.SetMsgBody((u8*)&tGenCfg.m_tQos, sizeof(tGenCfg.m_tQos));
			g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn(MCU_MT_SETQOS_CMD, cQosMsg);    

			cQosMsg.SetEventId(MCU_EQP_SETQOS_CMD);
			g_cEqpSsnApp.BroadcastToAllPeriEqpSsn(MCU_EQP_SETQOS_CMD, cQosMsg.GetServMsg(), cQosMsg.GetServMsgLen()); 
		}
        

		wRet = g_cMcuAgent.SetNetSyncInfo(tAgentGenCfg.m_tNetSyn);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_NETSYN)
			CfgLog("[ProcMcuGeneralCfg] mcu general write netsyn failed. ret:%d!\n", wRet);
			return;
		}

		wRet = g_cMcuAgent.SetDcsIp(tAgentGenCfg.m_dwDcsIp);
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_DCS)
			CfgLog("[ProcMcuGeneralCfg] mcu general write dcs failed. ret:%d!\n", wRet);
			return;
		}

/*
#ifdef _MINIMCU_
        // 保存DSC配置信息
        u32 dwDscInnerIp = tAgentGenCfg.m_tDscInfo.GetDscIp();
        u32 dwMcuInnerIp = tAgentGenCfg.m_tDscInfo.GetMcuIp();
        u32 dwIpMask = tAgentGenCfg.m_tDscInfo.GetIpMask();
		// Bug00007792, 判断是否配置了DSC, 防止被错误NACK, zgc, 2007-02-28
		u8 byCfgDscType = g_cMcuAgent.GetConfigedDscType();
		if( byCfgDscType != 0 &&  // Bug00007792, 判断是否配置了DSC, 防止被错误NACK, zgc, 2007-02-28
			(
			 0 == dwDscInnerIp ||
			 0 == dwMcuInnerIp ||
             0 == dwIpMask ||
             dwDscInnerIp == dwMcuInnerIp || 
             (dwDscInnerIp&dwIpMask) != (dwMcuInnerIp&dwIpMask)
			)
          )
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_INVALID_MCUGEN)
                CfgLog("[ProcMcuGeneralCfg] DCC module cfg error!\n");
            return;
		}
        wRet = g_cMcuAgent.SetDscInfo( &tAgentGenCfg.m_tDscInfo );
        if (SUCCESS_AGENT != wRet)
        {
            MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_DCS)
                CfgLog("[ProcMcuGeneralCfg] mcu general write dsc module failed. ret:%d!\n", wRet);
            return;
		}
#endif
*/
		wRet = g_cMcuAgent.SetIsMcuConfiged( TRUE );
		if (SUCCESS_AGENT != wRet)
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_ISCONFIGED);
			CfgLog("[ProcMcuGeneralCfg] mcu general write configflag failed. ret:%d!\n", wRet);
			return;
		}
/*
		// 处理LoginInfo, zgc, 2007-10-12
		// 设置本地LoginInfo
#ifndef WIN32
		wRet = g_cMcuAgent.SetLoginInfo( &tAgentGenCfg.m_tLoginInfo );
		if ( SUCCESS_AGENT != wRet )
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, pcMsg->event+2, ERR_MCU_CFG_WRITE_LOGININFO);
			CfgLog("[ProcMcuGeneralCfg] mcu general write logininfo failed. ret:%d!\n", wRet);
			return;
		}
#endif
		// 发送给8000B/C
#ifdef _MINIMCU_
		cServMsg.SetEventId( SVC_AGT_DSCTELNETLOGININFO_UPDATE_CMD );
		u8 bySlot = 0;
		u8 byLayer = 0;
		cServMsg.SetMsgBody( (u8*)&byLayer, sizeof(byLayer) );
		cServMsg.CatMsgBody( (u8*)&bySlot, sizeof(bySlot) );
		cServMsg.CatMsgBody( (u8*)&tAgentGenCfg.m_tLoginInfo, sizeof(tAgentGenCfg.m_tLoginInfo) );
		post( MAKEIID(AID_MCU_AGENT,1), SVC_AGT_DSCTELNETLOGININFO_UPDATE_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()  );
#endif
*/
		//ack
		SendMsg2Mcsssn(cServMsg, pcMsg->event+1);
	}

	//notify all mcs
	NotifyMcsGeneralCfg();

	return;
}

/*=============================================================================
函 数 名： ProcUpdateTelnetLoginInfo
功    能： 
算法实现： 
全局变量： 
参    数： const CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/10/19   4.0		周广程                  创建
=============================================================================*/
void CMcuCfgInst::ProcUpdateTelnetLoginInfo(const CMessage * const pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TLoginInfo tLoginInfo;
	memcpy( (u8*)&tLoginInfo, cServMsg.GetMsgBody(), sizeof(TLoginInfo) );

	// 目前写死用户名为admin
	tLoginInfo.SetUser("admin");

	if ( g_bPrintCfgMsg )
	{
		tLoginInfo.Print();
	}

	// 处理LoginInfo, zgc, 2007-10-19
	// 设置本地LoginInfo
#ifndef WIN32
	STATUS wRet = g_cMcuAgent.SetLoginInfo( &tLoginInfo );
	if ( SUCCESS_AGENT != wRet )
	{
		CfgLog("[ProcMcuGeneralCfg] mcu write logininfo failed. ret:%d!\n", wRet);
		SendMsg2Mcsssn( cServMsg, MCU_MCS_UPDATETELNETLOGININFO_NACK );
		return;
	}
#endif

	SendMsg2Mcsssn( cServMsg, MCU_MCS_UPDATETELNETLOGININFO_ACK );

#ifdef _MINIMCU_
	// 发送给8000B/C
	cServMsg.SetEventId( SVC_AGT_DSCTELNETLOGININFO_UPDATE_CMD );
	u8 bySlot = 0;
	u8 byLayer = 0;
	cServMsg.SetMsgBody( (u8*)&byLayer, sizeof(byLayer) );
	cServMsg.CatMsgBody( (u8*)&bySlot, sizeof(bySlot) );
	cServMsg.CatMsgBody( (u8*)&tLoginInfo, sizeof(tLoginInfo) );
	post( MAKEIID(AID_MCU_AGENT,1), SVC_AGT_DSCTELNETLOGININFO_UPDATE_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()  );
#endif

	return;
}

/*=============================================================================
  函 数 名： BrdCfgOut2In
  功    能： 单板信息格式转换
  算法实现： 
  全局变量： 
  参    数： TBoardInfo *ptIn
             TBrdCfgInfo *ptOut
             u8 byNum
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::BrdCfgOut2In(TBoardInfo *ptIn, TBrdCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut || byNum > MAXNUM_BOARD)
	{
		OspPrintf(TRUE, FALSE, "[BrdCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetIndex(ptIn[byIndex].GetBrdId());
		ptOut[byIndex].SetLayer(ptIn[byIndex].GetLayer());
		ptOut[byIndex].SetSlot(ptIn[byIndex].GetSlot(), GetSlotType(ptIn[byIndex].GetType()));
		ptOut[byIndex].SetType(BrdTypeOut2In(ptIn[byIndex].GetType()));
		ptOut[byIndex].SetIpAddr(ptIn[byIndex].GetBrdIp()); 
		ptOut[byIndex].SetSelNetPort(ptIn[byIndex].GetPortChoice());
		ptOut[byIndex].SetSelMulticast(ptIn[byIndex].GetCastChoice());
        ptOut[byIndex].SetBrdState( BrdStatusOut2In(ptIn[byIndex].GetState()) );
        ptOut[byIndex].SetOsType(ptIn[byIndex].GetOSType());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： BrdCfgIn2Out
  功    能： 单板信息格式转换
  算法实现： 
  全局变量： 
  参    数： TBrdCfgInfo *ptIn
             TBoardInfo *ptOut
             u8 byNum
  返 回 值： BOOL32   
=============================================================================*/
BOOL32  CMcuCfgInst::BrdCfgIn2Out(TBrdCfgInfo *ptIn, TBoardInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut || byNum > MAXNUM_BOARD)
	{
		OspPrintf(TRUE, FALSE, "[BrdCfgIn2Out] invalid input param!\n");
		return FALSE;
	}

	u8 bySlotType = SLOTTYPE_EX;
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		if ( BRD_TYPE_HDU == ptIn[byIndex].GetType() )     //4.6  jlb
		{
		ptOut[byIndex].SetBrdId(ptIn[byIndex].GetIndex());
		}
		else
		{
			ptOut[byIndex].SetBrdId(ptIn[byIndex].GetIndex());
		}
		ptOut[byIndex].SetLayer(ptIn[byIndex].GetLayer());

        u8 bySlot = 0;
		ptIn[byIndex].GetSlot(bySlot, bySlotType);
        ptOut[byIndex].SetSlot(bySlot);
        
		ptOut[byIndex].SetType(BrdTypeIn2Out(ptIn[byIndex].GetType()));
		ptOut[byIndex].SetBrdIp(ptIn[byIndex].GetIpAddr()); 
		ptOut[byIndex].SetPortChoice(ptIn[byIndex].GetSelNetPort());
		ptOut[byIndex].SetCastChoice(ptIn[byIndex].GetSelMulticast());
        
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());

		CfgLog("[BrdCfgIn2Out] Index: %d, Layer: %d, Slot: %d, Type: %d, Alias: %s, Ip: %0x\n", 
			    ptOut[byIndex].GetBrdId(), ptIn[byIndex].GetLayer(), ptOut[byIndex].GetSlot(), ptIn[byIndex].GetType(), ptIn[byIndex].GetAlias(), ptIn[byIndex].GetIpAddr());
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： MixerCfgOut2In
  功    能： 混音器信息转换
  算法实现： 
  全局变量： 
  参    数： TEqpMixerInfo *ptIn
             TEqpMixerCfgInfo *ptOut
             u8 byNum
  返 回 值： BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::MixerCfgOut2In(TEqpMixerInfo *ptIn, TEqpMixerCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[MixerCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		ptOut[byIndex].SetMaxChnnlInGrp(MAXNUM_MIXER_CHNNL);
        
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetUsedMap( abyMapId, byMapNum );
        ptOut[byIndex].SetMapId( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： MixerCfgIn2Out
  功    能： 混音器信息转换
  算法实现： 
  全局变量： 
  参    数： TEqpMixerCfgInfo *ptIn
             TEqpMixerInfo *ptOut
             u8 byNum
  返 回 值： BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::MixerCfgIn2Out(TEqpMixerCfgInfo *ptIn, TEqpMixerInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[MixerCfgIn2Out] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId( ptIn[byIndex].GetEqpId() );
        ptOut[byIndex].SetAlias( ptIn[byIndex].GetAlias() );
        ptOut[byIndex].SetMcuRecvPort( ptIn[byIndex].GetMcuStartPort() );
    	ptOut[byIndex].SetEqpRecvPort( ptIn[byIndex].GetEqpStartPort() );
		ptOut[byIndex].SetSwitchBrdId( ptIn[byIndex].GetSwitchBrdIndex() );
		ptOut[byIndex].SetRunBrdId( ptIn[byIndex].GetRunningBrdIndex() );
		ptOut[byIndex].SetMaxChnInGrp( MAXNUM_MIXER_CHNNL );

        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetMapId( abyMapId, byMapNum );
        ptOut[byIndex].SetUsedMap( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： RecCfgOut2In
  功    能： 录像机信息转换
  算法实现： 
  全局变量： 
  参    数： TEqpRecInfo *ptIn
             TEqpRecCfgInfo *ptOut
             u8 byNum
  返 回 值： BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::RecCfgOut2In(TEqpRecInfo *ptIn, TEqpRecCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[RecCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());        
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
        // guzh [7/11/2007]  不需要此配置
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetIpAddr(ptIn[byIndex].GetIpAddr());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： RecCfgIn2Out
  功    能： 录像机信息转换
  算法实现： 
  全局变量： 
  参    数： TEqpRecCfgInfo *ptIn
             TEqpRecInfo *ptOut
             u8 byNum
  返 回 值： BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::RecCfgIn2Out(TEqpRecCfgInfo *ptIn, TEqpRecInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[RecCfgIn2Out] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{        
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
        // guzh [7/11/2007]  不需要此配置
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetIpAddr(ptIn[byIndex].GetIpAddr());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： TWCfgOut2In
  功    能： 电视墙信息转换
  算法实现： 
  全局变量： 
  参    数： TEqpTVWallInfo *ptIn
             TEqpTvWallCfgInfo *ptOut
             u8 byNum
  返 回 值： BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::TWCfgOut2In(TEqpTVWallInfo *ptIn, TEqpTvWallCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[TWCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex <byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());

        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetUsedMap( abyMapId, byMapNum );
        ptOut[byIndex].SetMapId( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： TWCfgIn2Out
  功    能： 电视墙信息转换
  算法实现： 
  全局变量： 
  参    数： TEqpTvWallCfgInfo *ptIn
             TEqpTVWallInfo *ptOut
             u8 byNum
  返 回 值： BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::TWCfgIn2Out(TEqpTvWallCfgInfo *ptIn, TEqpTVWallInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[TWCfgIn2Out] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex <byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());

        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetMapId( abyMapId, byMapNum );
        ptOut[byIndex].SetUsedMap( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： BasCfgOut2In
  功    能： bas信息转换
  算法实现： 
  全局变量： 
  参    数： TEqpBasInfo *ptIn
             TEqpBasCfgInfo *ptOut
             u8 byNum
  返 回 值： BOOL32   
=============================================================================*/
BOOL32  CMcuCfgInst::BasCfgOut2In(TEqpBasInfo *ptIn, TEqpBasCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[BasCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());

        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetUsedMap( abyMapId, byMapNum );
        ptOut[byIndex].SetMapId( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： BasHDCfgOut2In
  功    能： basHD信息转换
  算法实现： 
  全局变量： 
  参    数： TEqpBasHDInfo *ptIn
             TEqpBasHDCfgInfo *ptOut
             u8 byNum
  返 回 值： BOOL32   
=============================================================================*/
BOOL32  CMcuCfgInst::BasHDCfgOut2In(TEqpBasHDInfo *ptIn, TEqpBasHDCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[BasHDCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());        
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetIpAddr(ptIn[byIndex].GetIpAddr());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
        ptOut[byIndex].SetType(ptIn[byIndex].GetHDBasType());
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： BasCfgIn2Out
  功    能： bas信息转换
  算法实现： 
  全局变量： 
  参    数： TEqpBasCfgInfo *ptIn
             TEqpBasInfo *ptOut
             u8 byNum
  返 回 值： BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::BasCfgIn2Out(TEqpBasCfgInfo *ptIn, TEqpBasInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[BasCfgIn2Out] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId( ptIn[byIndex].GetEqpId() );
        ptOut[byIndex].SetAlias( ptIn[byIndex].GetAlias() );

		ptOut[byIndex].SetMcuRecvPort( ptIn[byIndex].GetMcuStartPort() );
		ptOut[byIndex].SetEqpRecvPort( ptIn[byIndex].GetEqpStartPort() );
		ptOut[byIndex].SetSwitchBrdId( ptIn[byIndex].GetSwitchBrdIndex() );
		ptOut[byIndex].SetRunBrdId( ptIn[byIndex].GetRunningBrdIndex() );
        
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetMapId( abyMapId, byMapNum );
        ptOut[byIndex].SetUsedMap( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： BasHDCfgIn2Out
  功    能： basHD信息转换
  算法实现： 
  全局变量： 
  参    数： TEqpBasHDCfgInfo *ptIn
             TEqpBasHDInfo *ptOut
             u8 byNum
  返 回 值： BOOL32    
=============================================================================*/
BOOL32  CMcuCfgInst::BasHDCfgIn2Out(TEqpBasHDCfgInfo *ptIn, TEqpBasHDInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[BasHDCfgIn2Out] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetIpAddr(ptIn[byIndex].GetIpAddr());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
        ptOut[byIndex].SetHDBasType(ptIn[byIndex].GetType());
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： VmpCfgOut2In
  功    能： vmp信息转换
  算法实现： 
  全局变量： 
  参    数： TEqpVMPInfo *ptIn
             TEqpVmpCfgInfo *ptOut
             u8 byNum
  返 回 值： BOOL32   
=============================================================================*/
BOOL32  CMcuCfgInst::VmpCfgOut2In(TEqpVMPInfo *ptIn, TEqpVmpCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[VmpCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		ptOut[byIndex].SetEncodeNum(ptIn[byIndex].GetEncodeNum());

        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetUsedMap( abyMapId, byMapNum );
        ptOut[byIndex].SetMapId( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： VmpCfgIn2Out
  功    能： vmp信息转换
  算法实现： 
  全局变量： 
  参    数： TEqpVmpCfgInfo *ptIn
             TEqpVMPInfo *ptOut
             u8 byNum
  返 回 值： BOOL32   
=============================================================================*/
BOOL32  CMcuCfgInst::VmpCfgIn2Out(TEqpVmpCfgInfo *ptIn, TEqpVMPInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[VmpCfgIn2Out] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		ptOut[byIndex].SetEncodeNum(ptIn[byIndex].GetEncodeNum());

        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetMapId( abyMapId, byMapNum );
        ptOut[byIndex].SetUsedMap( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： PrsCfgOut2In
  功    能： prs信息转换
  算法实现： 
  全局变量： 
  参    数： TEqpPrsInfo *ptIn
             TPrsCfgInfo *ptOut
             u8 byNum
  返 回 值： BOOL32   
=============================================================================*/
BOOL32  CMcuCfgInst::PrsCfgOut2In(TEqpPrsInfo *ptIn, TPrsCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[PrsCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		ptOut[byIndex].SetFirstTimeSpan(ptIn[byIndex].GetFirstTimeSpan());
		ptOut[byIndex].SetSecondTimeSpan(ptIn[byIndex].GetSecondTimeSpan());
		ptOut[byIndex].SetThirdTimeSpan(ptIn[byIndex].GetThirdTimeSpan());
		ptOut[byIndex].SetLoseTimeSpan(ptIn[byIndex].GetRejectTimeSpan());
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： PrsCfgIn2Out
  功    能： prs信息转换
  算法实现： 
  全局变量： 
  参    数： TPrsCfgInfo *ptIn
             TEqpPrsInfo *ptOut
             u8 byNum
  返 回 值： BOOL32   
=============================================================================*/
BOOL32  CMcuCfgInst::PrsCfgIn2Out(TPrsCfgInfo *ptIn, TEqpPrsInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
	OspPrintf(TRUE, FALSE, "[PrsCfgIn2Out] invalid input param!\n");
	return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		ptOut[byIndex].SetFirstTimeSpan(ptIn[byIndex].GetFirstTimeSpan());
		ptOut[byIndex].SetSecondTimeSpan(ptIn[byIndex].GetSecondTimeSpan());
		ptOut[byIndex].SetThirdTimeSpan(ptIn[byIndex].GetThirdTimeSpan());
		ptOut[byIndex].SetRejectTimeSpan(ptIn[byIndex].GetLoseTimeSpan());
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： MtwCfgOut2In
  功    能： 复合电视墙结构转换
  算法实现： 
  全局变量： 
  参    数： TEqpMpwInfo *ptIn
             TMTvwallInfo *ptOut
             u8 byNum = 1
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::MtwCfgOut2In(TEqpMpwInfo *ptIn, TEqpMTvwallCfgInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[MtwCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());

        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetUsedMap( abyMapId, byMapNum );
        ptOut[byIndex].SetMapId( abyMapId, byMapNum );
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： MtwCfgIn2Out
  功    能： 复合电视墙结构转换
  算法实现： 
  全局变量： 
  参    数： TMTvwallInfo *ptIn
             TEqpMpwInfo *ptOut
             u8 byNum = 1
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::MtwCfgIn2Out(TEqpMTvwallCfgInfo *ptIn, TEqpMpwInfo *ptOut, u8 byNum)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[MtwCfgOut2In] invalid input param!\n");
		return FALSE;
	}

	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
        
        u8 abyMapId[MAXNUM_MAP];
        memset(abyMapId, 0, sizeof(abyMapId));
        u8 byMapNum = 0;
        ptIn[byIndex].GetMapId( abyMapId, byMapNum );
        ptOut[byIndex].SetUsedMap( abyMapId, byMapNum );
	}

	return TRUE;
}

//4.6新加外设 jlb
/*=============================================================================
  函 数 名： HduCfgOut2In
  功    能： 高清解码卡结构转换
  算法实现： 
  全局变量： 
  参    数： TEqpHduInfo *ptIn
             TEqpHduCfgInfo *ptOut
             u8 byNum = 1
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::HduCfgOut2In(TEqpHduInfo *ptIn, TEqpHduCfgInfo *ptOut, u8 byNum/* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[TWCfgOut2In] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex <byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());

        THduChnlModePortAgt tHduChnModePortAgt;
        THduChnlModePort tHduChnModePort;
        memset(&tHduChnModePortAgt, 0x0, sizeof(tHduChnModePortAgt));
		memset(&tHduChnModePort, 0x0, sizeof(tHduChnModePort));
		ptIn[byIndex].GetHduChnlModePort(0, tHduChnModePortAgt);
        tHduChnModePort.SetOutModeType(tHduChnModePortAgt.GetOutModeType());
		tHduChnModePort.SetOutPortType(tHduChnModePortAgt.GetOutPortType());
        tHduChnModePort.SetZoomRate( tHduChnModePortAgt.GetZoomRate() );
		ptOut[byIndex].SetHduChnlModePort(0, tHduChnModePort);
		ptIn[byIndex].GetHduChnlModePort(1, tHduChnModePortAgt);
		tHduChnModePort.SetOutModeType(tHduChnModePortAgt.GetOutModeType());
		tHduChnModePort.SetOutPortType(tHduChnModePortAgt.GetOutPortType());
        tHduChnModePort.SetZoomRate( tHduChnModePortAgt.GetZoomRate() );
		ptOut[byIndex].SetHduChnlModePort(1, tHduChnModePort);

//		ptOut[byIndex].SetOutModeType(ptIn[byIndex].GetOutModeType());
//		ptOut[byIndex].SetOutPortType(ptIn[byIndex].GetOutPortType());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());    //???
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： HduCfgIn2Out
  功    能： 高清解码卡结构转换
  算法实现： 
  全局变量： 
  参    数： TEqpHduCfgInfo *ptIn
             TEqpHduInfo *ptOut
             u8 byNum = 1
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::HduCfgIn2Out(TEqpHduCfgInfo *ptIn, TEqpHduInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[HduCfgIn2Out] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex <byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		
		THduChnlModePort tHduChnModePort;
		THduChnlModePortAgt tHduChnModePortAgt;
		memset(&tHduChnModePortAgt, 0x0, sizeof(tHduChnModePortAgt));
		memset(&tHduChnModePort, 0x0, sizeof(tHduChnModePort));
		ptIn[byIndex].GetHduChnlModePort(0, tHduChnModePort);
		tHduChnModePortAgt.SetZoomRate(tHduChnModePort.GetZoomRate());
		tHduChnModePortAgt.SetOutModeType(tHduChnModePort.GetOutModeType());
		tHduChnModePortAgt.SetOutPortType(tHduChnModePort.GetOutPortType());
		ptOut[byIndex].SetHduChnlModePort(0, tHduChnModePortAgt);
		ptIn[byIndex].GetHduChnlModePort(1, tHduChnModePort);
        tHduChnModePortAgt.SetZoomRate( tHduChnModePort.GetZoomRate() );
		tHduChnModePortAgt.SetOutModeType(tHduChnModePort.GetOutModeType());
		tHduChnModePortAgt.SetOutPortType(tHduChnModePort.GetOutPortType());
		ptOut[byIndex].SetHduChnlModePort(1, tHduChnModePortAgt);


// 		
// 		ptOut[byIndex].SetOutModeType(ptIn[byIndex].GetOutModeType());
// 		ptOut[byIndex].SetOutPortType(ptIn[byIndex].GetOutPortType());

	}

	return TRUE;
}

/*=============================================================================
  函 数 名： SvmpCfgOut2In
  功    能： 单Vmp结构转换
  算法实现： 
  全局变量： 
  参    数： TEqpSvmpInfo *ptIn
             TEqpSvmpCfgInfo *ptOut
             u8 byNum = 1
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::SvmpCfgOut2In(TEqpSvmpInfo *ptIn, TEqpSvmpCfgInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[SvmpCfgOut2In] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： SvmpCfgOut2In
  功    能： 单Vmp结构转换
  算法实现： 
  全局变量： 
  参    数： TEqpSvmpCfgInfo *ptIn
             TEqpSvmpInfo *ptOut
             u8 byNum = 1
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::SvmpCfgIn2Out(TEqpSvmpCfgInfo *ptIn, TEqpSvmpInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[SvmpCfgIn2Out] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： DvmpCfgOut2In
  功    能： 双Vmp结构转换
  算法实现： 
  全局变量： 
  参    数： TEqpDvmpBasicInfo *ptIn
             TEqpDvmpCfgBasicInfo *ptOut
             u8 byNum = 1
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::DvmpCfgOut2In(TEqpDvmpBasicInfo *ptIn, TEqpDvmpCfgBasicInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[DvmpCfgOut2In] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： DvmpCfgIn2Out
  功    能： 双Vmp结构转换
  算法实现： 
  全局变量： 
  参    数： TEqpDvmpCfgBasicInfo *ptIn
             TEqpDvmpBasicInfo *ptOut
             u8 byNum = 1
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::DvmpCfgIn2Out(TEqpDvmpCfgBasicInfo *ptIn, TEqpDvmpBasicInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[DvmpCfgIn2Out] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： MpuBasCfgOut2In
  功    能： 双Vmp结构中单个vmp转换
  算法实现： 
  全局变量： 
  参    数： TEqpMpuBasInfo *ptIn
             TEqpMpuBasCfgInfo *ptOut
             u8 byNum = 1
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::MpuBasCfgOut2In(TEqpMpuBasInfo *ptIn, TEqpMpuBasCfgInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[MpuBasCfgOut2In] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		
	}
	return TRUE;
}

/*=============================================================================
  函 数 名： MpuBasCfgOut2In
  功    能： 双Vmp结构中单个vmp转换
  算法实现： 
  全局变量： 
  参    数： TEqpMpuBasInfo *ptIn
             TEqpMpuBasCfgInfo *ptOut
             u8 byNum = 1
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::MpuBasCfgIn2Out(TEqpMpuBasCfgInfo *ptIn, TEqpMpuBasInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[MpuBasCfgIn2Out] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： EbapCfgOut2In
  功    能： Ebap结构转换
  算法实现： 
  全局变量： 
  参    数： TEqpEbapInfo *ptIn
             TEqpEbapCfgInfo *ptOut
             u8 byNum = 1
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::EbapCfgOut2In(TEqpEbapInfo *ptIn, TEqpEbapCfgInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[EbapCfgOut2In] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
	}

	return TRUE;
}
/*=============================================================================
  函 数 名： EbapCfgIn2Out
  功    能： Ebap结构转换
  算法实现： 
  全局变量： 
  参    数： TEqpEbapCfgInfo *ptIn
             TEqpEbapInfo *ptOut
             u8 byNum = 1
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::EbapCfgIn2Out(TEqpEbapCfgInfo *ptIn, TEqpEbapInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[EbapCfgIn2Out] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： EvpuCfgOut2In
  功    能： Ebap结构转换
  算法实现： 
  全局变量： 
  参    数： TEqpEvpuInfo *ptIn
             TEqpEvpuCfgInfo *ptOut
             u8 byNum = 1
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::EvpuCfgOut2In(TEqpEvpuInfo *ptIn, TEqpEvpuCfgInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[EvpuCfgOut2In] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
		ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuStartPort(ptIn[byIndex].GetMcuRecvPort());
		ptOut[byIndex].SetEqpStartPort(ptIn[byIndex].GetEqpRecvPort());
		ptOut[byIndex].SetSwitchBrdIndex(ptIn[byIndex].GetSwitchBrdId());
		ptOut[byIndex].SetRunningBrdIndex(ptIn[byIndex].GetRunBrdId());
		
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： EvpuCfgIn2Out
  功    能： Ebap结构转换
  算法实现： 
  全局变量： 
  参    数： TEqpEvpuCfgInfo *ptIn
             TEqpEvpuInfo *ptOut
             u8 byNum = 1
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::EvpuCfgIn2Out(TEqpEvpuCfgInfo *ptIn, TEqpEvpuInfo *ptOut, u8 byNum /* = 1 */)
{
	if (NULL == ptIn || NULL == ptOut)
	{
		OspPrintf(TRUE, FALSE, "[EvpuCfgIn2Out] invalid input param!\n");
		return FALSE;
	}
	
	for (u8 byIndex = 0; byIndex < byNum; byIndex++)
	{
		ptOut[byIndex].SetEqpId(ptIn[byIndex].GetEqpId());
        ptOut[byIndex].SetAlias(ptIn[byIndex].GetAlias());
		ptOut[byIndex].SetMcuRecvPort(ptIn[byIndex].GetMcuStartPort());
		ptOut[byIndex].SetEqpRecvPort(ptIn[byIndex].GetEqpStartPort());
		ptOut[byIndex].SetSwitchBrdId(ptIn[byIndex].GetSwitchBrdIndex());
		ptOut[byIndex].SetRunBrdId(ptIn[byIndex].GetRunningBrdIndex());
		
	}

	return TRUE;
}

/*=============================================================================
函 数 名： DscInfoOut2In
功    能： 
算法实现： 
全局变量： 
参    数：  TDSCModuleInfo &tIn
           T8000BDscMod &tOut
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/15  4.0			周广程                  创建
=============================================================================*/
BOOL32  CMcuCfgInst::DscInfoOut2In( TDSCModuleInfo &tIn, T8000BDscMod &tOut )
{
	tOut.SetDscInnerIp( tIn.GetDscInnerIp() );
	tOut.SetMcuInnerIp( tIn.GetMcuInnerIp() );
	tOut.SetInnerIpMask( tIn.GetInnerIpMask() );
	tOut.SetMcuPort( tIn.GetMcuPort() );
	tOut.SetStartDcs( tIn.IsStartDcs() );
	tOut.SetStartGk( tIn.IsStartGk() );
	tOut.SetStartMp( tIn.IsStartMp() );
	tOut.SetStartMtAdp( tIn.IsStartMtAdp() );
	tOut.SetStartProxy( tIn.IsStartProxy() );
	tOut.SetNetType( tIn.GetNetType() );
	tOut.SetLanMtProxyIpPos( tIn.GetLanMtProxyIpPos() );

	TMINIMCUNetParamAll tNetParamAll;
	tIn.GetCallAddrAll( tNetParamAll );
	tOut.SetCallAddrAll( tNetParamAll );

	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	tIn.GetMcsAccessAddrAll( tNetParamAll );
	tOut.SetMcsAccessAddrAll( tNetParamAll );

	return TRUE;
}

/*=============================================================================
函 数 名： DscInfoIn2Out
功    能： 
算法实现： 
全局变量： 
参    数：  T8000BDscMod &tIn
           TDSCModuleInfo &tOut
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/3/15  4.0			周广程                  创建
=============================================================================*/
BOOL32  CMcuCfgInst::DscInfoIn2Out( T8000BDscMod &tIn, TDSCModuleInfo &tOut )
{
	tOut.SetDscInnerIp( tIn.GetDscInnerIp() );
	tOut.SetMcuInnerIp( tIn.GetMcuInnerIp() );
	tOut.SetInnerIpMask( tIn.GetInnerIpMask() );
	tOut.SetMcuPort( tIn.GetMcuPort() );
	tOut.SetStartDcs( tIn.IsStartDcs() );
	tOut.SetStartGk( tIn.IsStartGk() );
	tOut.SetStartMp( tIn.IsStartMp() );
	tOut.SetStartMtAdp( tIn.IsStartMtAdp() );
	tOut.SetStartProxy( tIn.IsStartProxy() );
	tOut.SetNetType( tIn.GetNetType() );
	tOut.SetLanMtProxyIpPos( tIn.GetLanMtProxyIpPos() );

	TMINIMCUNetParamAll tNetParamAll;
	tIn.GetCallAddrAll( tNetParamAll );
	tOut.SetCallAddrAll( tNetParamAll );

	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	tIn.GetMcsAccessAddrAll( tNetParamAll );
	tOut.SetMcsAccessAddrAll( tNetParamAll );

	return TRUE;
}

/*=============================================================================
  函 数 名： GeneralCfgOut2In
  功    能： GeneralCfg转换
  算法实现： 
  全局变量： 
  参    数： TAgentMcuGenCfg &tIn
             TMcuGeneralCfg &tOut
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::GeneralCfgOut2In(TAgentMcuGenCfg &tIn, TMcuGeneralCfg &tOut)
{
    TTrapCfgInfo atTrapInfoList[MAXNUM_TRAP_LIST];
    
	//vmp style
	if (tIn.m_byVmpStyleNum > MAX_VMPSTYLE_NUM)
	{
		CfgLog("[GeneralCfgOut2In] vmp style num %d overflow!\n", tIn.m_byVmpStyleNum);
		return FALSE;
	}
	for (u8 byIndex = 0; byIndex < tIn.m_byVmpStyleNum; byIndex++)
	{
		tOut.m_atVmpStyle[byIndex].SetBackgroundColor(tIn.m_atVmpStyle[byIndex].GetBGDColor());
		tOut.m_atVmpStyle[byIndex].SetFrameColor(tIn.m_atVmpStyle[byIndex].GetFrameColor());
		tOut.m_atVmpStyle[byIndex].SetChairFrameColor(tIn.m_atVmpStyle[byIndex].GetChairFrameColor());
		tOut.m_atVmpStyle[byIndex].SetSpeakerFrameColor(tIn.m_atVmpStyle[byIndex].GetSpeakerFrameColor());  
		tOut.m_atVmpStyle[byIndex].SetSchemeId(tIn.m_atVmpStyle[byIndex].GetIndex());
		tOut.m_atVmpStyle[byIndex].SetFontType(tIn.m_atVmpStyle[byIndex].GetFontType());
		tOut.m_atVmpStyle[byIndex].SetFontSize(tIn.m_atVmpStyle[byIndex].GetFontSize());
		tOut.m_atVmpStyle[byIndex].SetAlignment(tIn.m_atVmpStyle[byIndex].GetAlignType());
		tOut.m_atVmpStyle[byIndex].SetTextColor(tIn.m_atVmpStyle[byIndex].GetTextColor());
		tOut.m_atVmpStyle[byIndex].SetTopicBkColor(tIn.m_atVmpStyle[byIndex].GetTopicBGDColor());
		tOut.m_atVmpStyle[byIndex].SetDiaphaneity(tIn.m_atVmpStyle[byIndex].GetDiaphaneity());
		tOut.m_atVmpStyle[byIndex].SetSchemeAlias((s8*)tIn.m_atVmpStyle[byIndex].GetAlias());
	}    
	tOut.m_byVmpStyleNum = tIn.m_byVmpStyleNum;

	//mcu local cfg
	tOut.m_tLocal.SetMcuAlias(tIn.m_tLocal.GetAlias());
	tOut.m_tLocal.SetMcuE164(tIn.m_tLocal.GetE164Num());
	tOut.m_tLocal.SetAudInfoRefreshTime(tIn.m_tLocal.GetAudioRefreshTime());
	tOut.m_tLocal.SetVidInfoRefreshTime(tIn.m_tLocal.GetVideoRefreshTime());
	tOut.m_tLocal.SetPartListRefreshTime(tIn.m_tLocal.GetListRefreshTime());
	tOut.m_tLocal.SetDiscCheckTime(tIn.m_tLocal.GetCheckTime());
	tOut.m_tLocal.SetDiscCheckTimes(tIn.m_tLocal.GetCheckTimes());
	tOut.m_tLocal.SetIsSaveBandWidth(tIn.m_tLocal.GetIsSaveBand());
	tOut.m_tLocal.SetMcuSoftVer(GetMcuVersion());
    tOut.m_tLocal.SetIsNPlusMode(tIn.m_tLocal.IsNPlusMode());
    tOut.m_tLocal.SetIsNPlusBackupMode(tIn.m_tLocal.IsNPlusBackupMode());
    tOut.m_tLocal.SetNPlusMcuIp(tIn.m_tLocal.GetNPlusMcuIp());
    tOut.m_tLocal.SetNPlusRtdTime(tIn.m_tLocal.GetNPlusRtdTime());
    tOut.m_tLocal.SetNPlusRtdNum(tIn.m_tLocal.GetNPlusRtdNum());
    tOut.m_tLocal.SetIsNPlusRollBack(tIn.m_tLocal.GetIsNPlusRollBack());
	s8  szVer[MAXLEN_ALIAS];
	sprintf(szVer, "Hardware Version: V%d", g_cMcuAgent.GetHWVersion());
	tOut.m_tLocal.SetMcuHardVer(szVer);

	//net cfg
	tOut.m_tNet.SetGkIpAddr(tIn.m_tNet.GetGkIp());
    tOut.m_tNet.SetIsGKCharge(tIn.m_tNet.GetIsGKCharge());
    tOut.m_tNet.SetRRQMtadpIp(tIn.m_tNet.GetRRQMtadpIp());
	tOut.m_tNet.SetMultiIpAddr(tIn.m_tNet.GetCastIp());
	tOut.m_tNet.SetMultiPort(tIn.m_tNet.GetCastPort());
	tOut.m_tNet.SetRcvStartPort(tIn.m_tNet.GetRecvStartPort());
	tOut.m_tNet.Set225245StartPort(tIn.m_tNet.Get225245StartPort());
	tOut.m_tNet.SetMaxMtNum(tIn.m_tNet.Get225245MtNum());
	tOut.m_tNet.SetIsUseMpcTransData(tIn.m_tNet.GetMpcTransData());
	tOut.m_tNet.SetIsUseMpcStack(tIn.m_tNet.GetMpcStack());
	tOut.m_tNet.SetMTUSize( tIn.m_tNet.GetMTUSize() ); //设置MTU的大小, zgc, 2007-04-02
	tOut.m_tNet.SetMTUSetupMode( tIn.m_tNet.GetMTUSetupMode() ); // MTU设置模式, zgc, 2007-04-05
    	
	if (tIn.m_byTrapListNum > MAXNUM_TRAP_LIST)
	{
		OspPrintf(TRUE, FALSE, "[GeneralCfgOut2In] too large traplist num!\n");
		return FALSE;
	}
	for (s32 nIndex = 0; nIndex < tIn.m_byTrapListNum; nIndex++)
	{
		atTrapInfoList[ nIndex].SetTrapIp(tIn.m_atTrapList[nIndex].GetTrapIp());
		atTrapInfoList[nIndex].SetReadWritePort(tIn.m_atTrapList[nIndex].GetGSPort());
		atTrapInfoList[nIndex].SetSendTrapPort(tIn.m_atTrapList[nIndex].GetTrapPort());
		atTrapInfoList[nIndex].SetReadCom(tIn.m_atTrapList[nIndex].GetReadCom());
		atTrapInfoList[nIndex].SetWriteCom(tIn.m_atTrapList[nIndex].GetWriteCom());
	}
	tOut.m_tNet.SetTrapList(atTrapInfoList, tIn.m_byTrapListNum);

	//qos
	tOut.m_tQos.SetQosType(tIn.m_tQos.GetQosType());
	tOut.m_tQos.SetAudLevel(tIn.m_tQos.GetAudioLevel());
	tOut.m_tQos.SetVidLevel(tIn.m_tQos.GetVideoLevel());
	tOut.m_tQos.SetDataLevel(tIn.m_tQos.GetDataLevel());
	tOut.m_tQos.SetSignalLevel(tIn.m_tQos.GetSignalLevel());
	tOut.m_tQos.SetIpServiceType(tIn.m_tQos.GetIpServiceType());

	//net syn
	tOut.m_tNetSyn.SetNetSynMode(tIn.m_tNetSyn.GetMode());
	tOut.m_tNetSyn.SetNetSynDTSlot(tIn.m_tNetSyn.GetDTSlot());
	tOut.m_tNetSyn.SetNetSynE1Index(tIn.m_tNetSyn.GetE1Index());

	//dcs 
	tOut.m_tDcs.SetDcsEqpIp(ntohl(tIn.m_dwDcsIp));

    // guzh [1/12/2007] 8000B 的DSC启动的模块和信息
    tOut.m_tDscInfo.SetMcuInnerIp( tIn.m_tDscInfo.GetMcuInnerIp() );
    tOut.m_tDscInfo.SetDscInnerIp( tIn.m_tDscInfo.GetDscInnerIp() );
    tOut.m_tDscInfo.SetMcuPort( tIn.m_tDscInfo.GetMcuPort() );
    tOut.m_tDscInfo.SetInnerIpMask( tIn.m_tDscInfo.GetInnerIpMask() );
	tOut.m_tDscInfo.SetNetType( tIn.m_tDscInfo.GetNetType() );
	tOut.m_tDscInfo.SetLanMtProxyIpPos( tIn.m_tDscInfo.GetLanMtProxyIpPos() );

    tOut.m_tDscInfo.SetStartMp(tIn.m_tDscInfo.IsStartMp());
    tOut.m_tDscInfo.SetStartMtAdp(tIn.m_tDscInfo.IsStartMtAdp());
    tOut.m_tDscInfo.SetStartGk(tIn.m_tDscInfo.IsStartGk());
    tOut.m_tDscInfo.SetStartProxy(tIn.m_tDscInfo.IsStartProxy());
    tOut.m_tDscInfo.SetStartDcs(tIn.m_tDscInfo.IsStartDcs());

	TMINIMCUNetParamAll tNetParamAll;
	tIn.m_tDscInfo.GetCallAddrAll( tNetParamAll );
	tOut.m_tDscInfo.SetCallAddrAll( tNetParamAll );

	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	tIn.m_tDscInfo.GetMcsAccessAddrAll( tNetParamAll );
	tOut.m_tDscInfo.SetMcsAccessAddrAll( tNetParamAll );

	tOut.m_tLoginInfo.SetUser( tIn.m_tLoginInfo.GetUser() );
	s8 achPwd[MAXLEN_PWD+1];
	memset(achPwd, 0, sizeof(achPwd));
	tIn.m_tLoginInfo.GetPwd(achPwd, sizeof(achPwd));
	tOut.m_tLoginInfo.SetPwd( (LPCSTR)achPwd );

	return TRUE;
}

/*=============================================================================
  函 数 名： GeneralCfgIn2Out
  功    能： GeneralCfg转换
  算法实现： 
  全局变量： 
  参    数： TMcuGeneralCfg &tIn
             TAgentMcuGenCfg &tOut
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuCfgInst::GeneralCfgIn2Out(TMcuGeneralCfg &tIn, TAgentMcuGenCfg &tOut)
{   
    TTrapCfgInfo atTrapInfoList[MAXNUM_TRAP_LIST];
    
	//vmp style  
	if (tIn.m_byVmpStyleNum > MAX_VMPSTYLE_NUM)
	{
		CfgLog("[GeneralCfgIn2Out] vmp style num %d overflow!\n", tIn.m_byVmpStyleNum);
		return FALSE;
	}
	for (u8 byIndex = 0; byIndex < tIn.m_byVmpStyleNum; byIndex++)
	{
		tOut.m_atVmpStyle[byIndex].SetBGDColor(tIn.m_atVmpStyle[byIndex].GetBackgroundColor());
		tOut.m_atVmpStyle[byIndex].SetFrameColor(tIn.m_atVmpStyle[byIndex].GetFrameColor());
		tOut.m_atVmpStyle[byIndex].SetChairFrameColor(tIn.m_atVmpStyle[byIndex].GetChairFrameColor());
		tOut.m_atVmpStyle[byIndex].SetSpeakerFrameColor(tIn.m_atVmpStyle[byIndex].GetSpeakerFrameColor());
		tOut.m_atVmpStyle[byIndex].SetIndex(tIn.m_atVmpStyle[byIndex].GetSchemeId());
		tOut.m_atVmpStyle[byIndex].SetFontType(tIn.m_atVmpStyle[byIndex].GetFontType());
		tOut.m_atVmpStyle[byIndex].SetFontSize(tIn.m_atVmpStyle[byIndex].GetFontSize());
		tOut.m_atVmpStyle[byIndex].SetAlignType(tIn.m_atVmpStyle[byIndex].GetAlignment());
		tOut.m_atVmpStyle[byIndex].SetTextColor(tIn.m_atVmpStyle[byIndex].GetTextColor());
		tOut.m_atVmpStyle[byIndex].SetTopicBGDColor(tIn.m_atVmpStyle[byIndex].GetTopicBkColor());
		tOut.m_atVmpStyle[byIndex].SetDiaphaneity(tIn.m_atVmpStyle[byIndex].GetDiaphaneity());
        
        s8 achAlias[MAX_VMPSTYLE_ALIASLEN];
        memset( achAlias, 0, sizeof(achAlias) );
        tIn.m_atVmpStyle[byIndex].GetSchemeAlias( achAlias, sizeof(achAlias) );
        tOut.m_atVmpStyle[byIndex].SetAlias(achAlias);
	}
	tOut.m_byVmpStyleNum = tIn.m_byVmpStyleNum;

	//mcu local cfg
	if (0 == strlen(tIn.m_tLocal.GetMcuAlias()) || 0 == strlen(tIn.m_tLocal.GetMcuE164()))
	{
		CfgLog("[GeneralCfgIn2Out] mcu alias or E164 is empty!\n");
		return FALSE;
	}

    // N+1模式参数有效性检测
    if (tIn.m_tLocal.IsNPlusMode())
    {
        if (!tIn.m_tLocal.IsNPlusBackupMode() && tIn.m_tLocal.GetNPlusMcuIp() == 0)
        {
            CfgLog("[GeneralCfgIn2Out] NPlusMcuIp can't be empty in not NPlusBackupMode!\n");
		    return FALSE;
        }

        //rtd 有效性检测
        if ( tIn.m_tLocal.IsNPlusBackupMode() && 
             ( tIn.m_tLocal.GetNPlusRtdTime() == 0 || tIn.m_tLocal.GetNPlusRtdNum() == 0 ) )
        {
            CfgLog("[GeneralCfgIn2Out] RtdTime or RtdTimes can't be 0 in NPlusBackupMode!\n");
		    return FALSE;
        }
    }

    tOut.m_tLocal.SetAlias(tIn.m_tLocal.GetMcuAlias());
    tOut.m_tLocal.SetE164Num(tIn.m_tLocal.GetMcuE164());
	tOut.m_tLocal.SetAudioRefreshTime(tIn.m_tLocal.GetAudInfoRefreshTime());
	tOut.m_tLocal.SetVideoRefreshTime(tIn.m_tLocal.GetVidInfoRefreshTime());
	tOut.m_tLocal.SetListRefreshTime(tIn.m_tLocal.GetPartListRefreshTime());
	tOut.m_tLocal.SetCheckTime(tIn.m_tLocal.GetDiscCheckTime());
	tOut.m_tLocal.SetCheckTimes(tIn.m_tLocal.GetDiscCheckTimes());
	tOut.m_tLocal.SetIsSaveBand(tIn.m_tLocal.GetIsSaveBandWidth());
	tOut.m_tLocal.SetMcuId(LOCAL_MCUID);
    tOut.m_tLocal.SetIsNPlusMode(tIn.m_tLocal.IsNPlusMode());
    tOut.m_tLocal.SetIsNPlusBackupMode(tIn.m_tLocal.IsNPlusBackupMode());
    tOut.m_tLocal.SetNPlusMcuIp(tIn.m_tLocal.GetNPlusMcuIp());
    tOut.m_tLocal.SetNPlusRtdTime(tIn.m_tLocal.GetNPlusRtdTime());
    tOut.m_tLocal.SetNPlusRtdNum(tIn.m_tLocal.GetNPlusRtdNum());
    tOut.m_tLocal.SetIsNPlusRollBack(tIn.m_tLocal.GetIsNPlusRollBack());

	//net cfg
	tOut.m_tNet.SetGKIp(tIn.m_tNet.GetGkIpAddr());
    tOut.m_tNet.SetIsGKCharge(tIn.m_tNet.GetIsGKCharge());
    tOut.m_tNet.SetRRQMtadpIp(tIn.m_tNet.GetRRQMtadpIp());
	tOut.m_tNet.SetCastIp(tIn.m_tNet.GetMultiIpAddr());
	tOut.m_tNet.SetCastPort(tIn.m_tNet.GetMultiPort());
	tOut.m_tNet.SetRecvStartPort(tIn.m_tNet.GetRcvStartPort());
	tOut.m_tNet.Set225245StartPort(tIn.m_tNet.Get225245StartPort());
	tOut.m_tNet.SetMTUSize( tIn.m_tNet.GetMTUSize() ); //设置MTU的大小, zgc, 2007-04-02
	tOut.m_tNet.SetMTUSetupMode( tIn.m_tNet.GetMTUSetupMode() ); // MTU设置模式, zgc, 2007-04-05
	
	if (tOut.m_tNet.GetRecvStartPort() % PORTSPAN > 0)
	{
		CfgLog("[GeneralCfgIn2Out] mcu receive start port 应该为%d的倍数.\n", PORTSPAN);
		u16 wRecvStartPort = tOut.m_tNet.GetRecvStartPort();
		tOut.m_tNet.SetRecvStartPort( wRecvStartPort - wRecvStartPort % PORTSPAN);
	}
	
    tOut.m_tNet.Set225245MtNum( g_cMcuAgent.Get225245MtNum() );
    if(0 == tOut.m_tNet.Get225245MtNum())
    {
        tOut.m_tNet.Set225245MtNum( MAXNUM_H225H245_MT );
    }
    
	tOut.m_tNet.SetMpcTransData( tIn.m_tNet.GetIsUseMpcTransData() );
	tOut.m_tNet.SetMpcStack( tIn.m_tNet.GetIsUseMpcStack() );
	    
	tIn.m_tNet.GetTrapList(atTrapInfoList, tOut.m_byTrapListNum);    
	for (s32 nIndex = 0; nIndex < tOut.m_byTrapListNum; nIndex++)
	{
		tOut.m_atTrapList[nIndex].SetTrapIp(atTrapInfoList[nIndex].GetTrapIp());
		tOut.m_atTrapList[nIndex].SetGSPort(atTrapInfoList[nIndex].GetReadWritePort());
		tOut.m_atTrapList[nIndex].SetTrapPort(atTrapInfoList[nIndex].GetSendTrapPort());
		tOut.m_atTrapList[nIndex].SetReadCom(atTrapInfoList[nIndex].GetReadCom());
		tOut.m_atTrapList[nIndex].SetWriteCom(atTrapInfoList[nIndex].GetWriteCom());
	}

	//qos
	if (QOSTYPE_DIFFERENCE_SERVICE == tIn.m_tQos.GetQosType())
	{
		if (tIn.m_tQos.GetAudLevel() > QOS_DIFF_MAXLEVEL || 
			tIn.m_tQos.GetVidLevel() > QOS_DIFF_MAXLEVEL || 
			tIn.m_tQos.GetDataLevel() > QOS_DIFF_MAXLEVEL || 
			tIn.m_tQos.GetSignalLevel() > QOS_DIFF_MAXLEVEL)
		{
			CfgLog("[GeneralCfgIn2Out] qos level aud<%d> vid<%d> data<%d> sign<%d> is invalid!\n", 
				   tIn.m_tQos.GetAudLevel(), tIn.m_tQos.GetVidLevel(), tIn.m_tQos.GetDataLevel(), 
				   tIn.m_tQos.GetSignalLevel());

			return FALSE;
		}
	}
	else if (QOSTYPE_IP_PRIORITY == tIn.m_tQos.GetQosType())
	{
		if (tIn.m_tQos.GetAudLevel() > QOS_IP_MAXLEVEL || 
			tIn.m_tQos.GetVidLevel() > QOS_IP_MAXLEVEL || 
			tIn.m_tQos.GetDataLevel() > QOS_IP_MAXLEVEL || 
			tIn.m_tQos.GetSignalLevel() > QOS_IP_MAXLEVEL)
		{
			CfgLog("[GeneralCfgIn2Out] qos level aud<%d> vid<%d> data<%d> sign<%d>, or service type<%d> is invalid!\n", 
				   tIn.m_tQos.GetAudLevel(), tIn.m_tQos.GetVidLevel(), tIn.m_tQos.GetDataLevel(), 
				   tIn.m_tQos.GetSignalLevel(), tIn.m_tQos.GetIpServiceType());
			return FALSE;
		}
	}
	else
	{
		CfgLog("[GeneralCfgIn2Out] qos type<%d> is invalid!\n", tIn.m_tQos.GetQosType());
		return FALSE;
	}

	tOut.m_tQos.SetQosType( tIn.m_tQos.GetQosType() );
	tOut.m_tQos.SetAudioLevel( tIn.m_tQos.GetAudLevel() );
	tOut.m_tQos.SetVideoLevel( tIn.m_tQos.GetVidLevel() );
	tOut.m_tQos.SetDataLevel( tIn.m_tQos.GetDataLevel() );
	tOut.m_tQos.SetSignalLevel( tIn.m_tQos.GetSignalLevel() );
	tOut.m_tQos.SetIpServiceType( tIn.m_tQos.GetIpServiceType() );

	//net syn
	if (NETSYNTYPE_VIBRATION != tIn.m_tNetSyn.GetNetSynMode() && 
		NETSYNTYPE_TRACE != tIn.m_tNetSyn.GetNetSynMode())
	{
		CfgLog("[GeneralCfgIn2Out] net syn mode<%d> is invalid!\n", tIn.m_tNetSyn.GetNetSynMode());
		return FALSE;
	}
	if (tIn.m_tNetSyn.GetNetSynDTSlot() > NETSYN_MAX_DTNUM || 
		tIn.m_tNetSyn.GetNetSynE1Index() > NETSYN_MAX_E1NUM)
	{
		CfgLog("[GeneralCfgIn2Out] net syn DT num<%d> or E1 num<%d> is invalid!\n", 
			   tIn.m_tNetSyn.GetNetSynDTSlot(), tIn.m_tNetSyn.GetNetSynE1Index());
		return FALSE;
	}

	tOut.m_tNetSyn.SetMode( tIn.m_tNetSyn.GetNetSynMode());
	tOut.m_tNetSyn.SetDTSlot( tIn.m_tNetSyn.GetNetSynDTSlot());
	tOut.m_tNetSyn.SetE1Index( tIn.m_tNetSyn.GetNetSynE1Index());

	//dcs 
	tOut.m_dwDcsIp = htonl(tIn.m_tDcs.GetDcsEqpIp());

    // guzh [1/12/2007] 8000B 的DSC启动的模块和连接信息
    tOut.m_tDscInfo.SetMcuInnerIp( tIn.m_tDscInfo.GetMcuInnerIp() );
    tOut.m_tDscInfo.SetDscInnerIp( tIn.m_tDscInfo.GetDscInnerIp() );
    tOut.m_tDscInfo.SetMcuPort( tIn.m_tDscInfo.GetMcuPort() );
    tOut.m_tDscInfo.SetInnerIpMask( tIn.m_tDscInfo.GetInnerIpMask() );
	tOut.m_tDscInfo.SetNetType( tIn.m_tDscInfo.GetNetType() );
	tOut.m_tDscInfo.SetLanMtProxyIpPos( tIn.m_tDscInfo.GetLanMtProxyIpPos() );

    tOut.m_tDscInfo.SetStartMp(tIn.m_tDscInfo.IsStartMp());
    tOut.m_tDscInfo.SetStartMtAdp(tIn.m_tDscInfo.IsStartMtAdp());
    tOut.m_tDscInfo.SetStartGk(tIn.m_tDscInfo.IsStartGk());
    tOut.m_tDscInfo.SetStartProxy(tIn.m_tDscInfo.IsStartProxy());
    tOut.m_tDscInfo.SetStartDcs(tIn.m_tDscInfo.IsStartDcs());

	TMINIMCUNetParamAll tNetParamAll;
	tIn.m_tDscInfo.GetCallAddrAll( tNetParamAll );
	tOut.m_tDscInfo.SetCallAddrAll( tNetParamAll );

	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	tIn.m_tDscInfo.GetMcsAccessAddrAll( tNetParamAll );
	tOut.m_tDscInfo.SetMcsAccessAddrAll( tNetParamAll );

	//Login info
	// 写死为 admin, zgc, 2007-10-16
	//tOut.m_tLoginInfo.SetUser( tIn.m_tLoginInfo.GetUser() );
	tOut.m_tLoginInfo.SetUser( (LPCSTR)"admin" );
	s8 achPwd[MAXLEN_PWD+1];
	memset(achPwd, 0, sizeof(achPwd));
	tIn.m_tLoginInfo.GetPwd(achPwd, sizeof(achPwd));
	tOut.m_tLoginInfo.SetPwd( (LPCSTR)achPwd );

	return TRUE;
}

/*=============================================================================
  函 数 名： BrdTypeOut2In
  功    能： 板类型转换
  算法实现： 
  全局变量： 
  参    数： u8 byType
  返 回 值： u8   
=============================================================================*/
u8  CMcuCfgInst::BrdTypeOut2In(u8 byType)
{
    u8 byInType = 0;
    switch(byType)
    {
    case DSL8000_BRD_MPC:
        byInType = BRD_TYPE_MPC;
        break;
    case DSL8000_BRD_CRI:
        byInType = BRD_TYPE_CRI;
        break;
    case DSL8000_BRD_DRI:
        byInType = BRD_TYPE_DRI;
        break;
    case DSL8000_BRD_APU:
        byInType = BRD_TYPE_APU;
        break;
    case DSL8000_BRD_VPU:
        byInType = BRD_TYPE_VPU;
        break;
    case DSL8000_BRD_DEC5:
        byInType = BRD_TYPE_DEC5;
        break;
    case DSL8000_BRD_MMP:
        byInType = BRD_TYPE_MMP;
        break;
    case DSL8000_BRD_DTI:
        byInType = BRD_TYPE_DTI;
        break;
    case DSL8000_BRD_DIC:
        byInType = BRD_TYPE_DIC;
        break;
    case DSL8000_BRD_VAS:
        byInType = BRD_TYPE_VAS;
        break;
    case DSL8000_BRD_IMT:
        byInType = BRD_TYPE_IMT;
        break;
    case DSL8000_BRD_DSI:
        byInType = BRD_TYPE_DSI;
        break;
	case KDV8000B_MODULE:
		byInType = BRD_TYPE_DSC;
		break;
	case DSL8000_BRD_MDSC:
		byInType = BRD_TYPE_MDSC;
		break;
    case DSL8000_BRD_HDSC:
        byInType = BRD_TYPE_HDSC;
        break;
	//4.6 版本 新加  jlb
	case BRD_HWID_DSL8000_MPU:
		byInType = BRD_TYPE_MPU;
		break;
	case BRD_HWID_EBAP:
		byInType = BRD_TYPE_EBAP;
		break;
	case BRD_HWID_EVPU:
		byInType = BRD_TYPE_EVPU;
		break;
	case BRD_HWID_KDM200_HDU:
		byInType = BRD_TYPE_HDU;
		break;
    default:
        OspPrintf(TRUE, FALSE, "[BrdTypeOut2In] invalid brd type:%d\n", byType);
        break;
    }
    
    return byInType;
}

/*=============================================================================
  函 数 名： BrdTypeIn2Out
  功    能： 板类型转换
  算法实现： 
  全局变量： 
  参    数： u8 byType
  返 回 值： u8  
=============================================================================*/
u8  CMcuCfgInst::BrdTypeIn2Out(u8 byType)
{
    u8 byOutType = 0;
    switch(byType)
    {
    case BRD_TYPE_MPC:
        byOutType = DSL8000_BRD_MPC;
        break;
    case BRD_TYPE_CRI:
        byOutType = DSL8000_BRD_CRI;
        break;
    case BRD_TYPE_DRI:
        byOutType = DSL8000_BRD_DRI;
        break;
    case BRD_TYPE_APU:
        byOutType = DSL8000_BRD_APU;
        break;
    case BRD_TYPE_VPU:
        byOutType = DSL8000_BRD_VPU;
        break;
    case BRD_TYPE_DEC5:
        byOutType = DSL8000_BRD_DEC5;
        break;
    case BRD_TYPE_MMP:
        byOutType = DSL8000_BRD_MMP;
        break;
    case BRD_TYPE_DTI:
        byOutType = DSL8000_BRD_DTI;
        break;
    case BRD_TYPE_DIC:
        byOutType = DSL8000_BRD_DIC;
        break;
    case BRD_TYPE_VAS:
        byOutType = DSL8000_BRD_VAS;
        break;
    case BRD_TYPE_IMT:
        byOutType = DSL8000_BRD_IMT;
        break;
    case BRD_TYPE_DSI:
        byOutType = DSL8000_BRD_DSI;
        break;
	case BRD_TYPE_DSC:
		byOutType = KDV8000B_MODULE;
		break;
	case BRD_TYPE_MDSC:
		byOutType = DSL8000_BRD_MDSC;
		break;
    case BRD_TYPE_HDSC:
        byOutType = DSL8000_BRD_HDSC;
        break;
	//4.6新加版本类型 jlb
	case BRD_TYPE_HDU:
		byOutType = BRD_HWID_KDM200_HDU;
		break;
	case BRD_TYPE_MPU:
		byOutType = BRD_HWID_DSL8000_MPU;
        break;
	case BRD_TYPE_EBAP: 
		byOutType = BRD_HWID_EBAP;
		break;
	case BRD_TYPE_EVPU:
		byOutType = BRD_HWID_EVPU;
        break;
    default:
        OspPrintf(TRUE, FALSE, "[BrdTypeIn2Out] invalid brd type:%d\n", byType);
        break;
    }

    return byOutType;
}

/*=============================================================================
  函 数 名： BrdStatusIn2Out
  功    能： 单板在线状态从VC转到代理库
  算法实现： 
  全局变量： 
  参    数： u8 byStatus
  返 回 值： u8  
=============================================================================*/
u8  CMcuCfgInst::BrdStatusIn2Out( u8 byStatus )
{
    u8 byStatusOut = 0;
    switch( byStatus )
    {
    case BOARD_INLINE:      byStatusOut = BOARD_STATUS_INLINE;      break;
    case BOARD_OUTLINE:     byStatusOut = BOARD_STATUS_DISCONNECT;  break;
    case BOARD_UNKNOW:      byStatusOut = BOARD_STATUS_UNKNOW;      break;

    // FIXME: 本转换目前不会用到. 姑且起用，等待功能扩展.
    case BOARD_CFGCONFLICT: byStatusOut = BOARD_STATUS_CFGCONFLICT; break;
    default:                byStatusOut = BOARD_STATUS_UNKNOW;      break;
    }
    return byStatusOut;
}

/*=============================================================================
  函 数 名： BrdStatusOut2In
  功    能： 单板在线状态从代理库转到VC
  算法实现： 
  全局变量： 
  参    数： u8 byStatus
  返 回 值： u8  
=============================================================================*/
u8  CMcuCfgInst::BrdStatusOut2In( u8 byStatus )
{
    u8 byStatusOut = 0;
    switch( byStatus )
    {
    case BOARD_STATUS_INLINE:       byStatusOut = BOARD_INLINE;    break;
    case BOARD_STATUS_DISCONNECT:   byStatusOut = BOARD_OUTLINE;   break;
        
    // FIXME: 本转换目前已经跑到. 本该转为 BOARD_CFGCONFLICT 的，
    //        但且遵循以前的逻辑转为 BOARD_UNKNOW，等待功能扩展。
    case BOARD_STATUS_CFGCONFLICT:  byStatusOut = BOARD_UNKNOW;    break;
    case BOARD_STATUS_UNKNOW:       byStatusOut = BOARD_UNKNOW;    break;
    default:                        byStatusOut = BOARD_UNKNOW;    break;
    }
    return byStatusOut;
}

/*=============================================================================
  函 数 名： GetSlotType
  功    能： 从板类型得到槽类型
  算法实现： 
  全局变量： 
  参    数： u8 byBrdType
  返 回 值： u8  
=============================================================================*/
u8  CMcuCfgInst::GetSlotType(u8 byBrdType)
{
    u8 bySlotType = 0;

    switch(byBrdType)
    {
    case DSL8000_BRD_MPC:
        bySlotType = SLOTTYPE_MC;
    	break;

	case BRD_HWID_KDM200_HDU:
		bySlotType = SLOTTYPE_TVSEX;
		break;
    
    default:
        bySlotType = SLOTTYPE_EX;
        break;
    }

    return bySlotType;
}

/*=============================================================================
  函 数 名： ProcMcsGetVmpScheme
  功    能： 取画面合成方案信息
  算法实现： 
  全局变量： 
  参    数： const CMessage* pcMsg
  返 回 值： void 
=============================================================================*/
void CMcuCfgInst::ProcMcsGetVmpScheme(const CMessage* pcMsg)
{
	CServMsg cVmpSchemeMsg(pcMsg->content, pcMsg->length);
	u8 byVmpSchemeNum = 0;
	TVmpAttachCfg atVmpSchemeTable[MAX_VMPSTYLE_NUM];
	u16 wRet = g_cMcuAgent.ReadVmpAttachTable(&byVmpSchemeNum, atVmpSchemeTable);
	if (SUCCESS_AGENT == wRet)
	{
		TVmpStyleCfgInfo atVmpSchemeInfo[MAX_VMPSTYLE_NUM];
		for (u8 byLp = 0; byLp < byVmpSchemeNum; byLp++)
		{
			atVmpSchemeInfo[byLp].SetBackgroundColor(atVmpSchemeTable[byLp].GetBGDColor());
			atVmpSchemeInfo[byLp].SetFrameColor(atVmpSchemeTable[byLp].GetFrameColor());
			atVmpSchemeInfo[byLp].SetChairFrameColor(atVmpSchemeTable[byLp].GetChairFrameColor());
			atVmpSchemeInfo[byLp].SetSpeakerFrameColor(atVmpSchemeTable[byLp].GetSpeakerFrameColor());  
			atVmpSchemeInfo[byLp].SetSchemeId(atVmpSchemeTable[byLp].GetIndex());
			atVmpSchemeInfo[byLp].SetFontType(atVmpSchemeTable[byLp].GetFontType());
			atVmpSchemeInfo[byLp].SetFontSize(atVmpSchemeTable[byLp].GetFontSize());
			atVmpSchemeInfo[byLp].SetTextColor(atVmpSchemeTable[byLp].GetTextColor());
			atVmpSchemeInfo[byLp].SetTopicBkColor(atVmpSchemeTable[byLp].GetTopicBGDColor());
			atVmpSchemeInfo[byLp].SetDiaphaneity(atVmpSchemeTable[byLp].GetDiaphaneity());
			atVmpSchemeInfo[byLp].SetSchemeAlias((s8*)atVmpSchemeTable[byLp].GetAlias());
		}
		cVmpSchemeMsg.SetMsgBody((u8*)&atVmpSchemeInfo, sizeof(atVmpSchemeInfo));
		SendMsg2Mcsssn(cVmpSchemeMsg, pcMsg->event+1);
	}
	else
	{
		cVmpSchemeMsg.SetErrorCode(wRet);
		SendMsg2Mcsssn(cVmpSchemeMsg, pcMsg->event+2);
	}

	return;
}

//4.6版本 新加 jlb
/*=============================================================================
  函 数 名： ProcMcsGetHduScheme
  功    能： 取HDU预案方案信息
  算法实现： 
  全局变量： 
  参    数： const CMessage* pcMsg
  返 回 值： void
------------------------------------------------------------------------------
  日 期      版 本      修改人      走读人       修改内容
 2009/01/10   4.6       江乐斌                    创建
=============================================================================*/
void  CMcuCfgInst::ProcMcsGetHduScheme(const CMessage* pcMsg)
{
	CServMsg cHduSchemeMsg(pcMsg->content,pcMsg->length);
    u8 byHduSchemeNum =0;
    
	THduStyleInfo atHduStyleInfoTable[MAX_HDUSTYLE_NUM];
	u16 wRet = g_cMcuAgent.ReadHduSchemeTable(&byHduSchemeNum, atHduStyleInfoTable);
	if (SUCCESS_AGENT == wRet)
	{
		THduStyleCfgInfo atHduStyleCfgInfo[MAX_HDUSTYLE_NUM];
		for ( u8 byLp = 0; byLp < byHduSchemeNum; byLp++ )
		{
			THduChnlInfo  atHduChnlInfo[MAXNUM_HDUCFG_CHNLNUM];

			atHduStyleInfoTable[byLp].GetHduChnlTable(atHduChnlInfo);

			THduChnlCfgInfo atHduChnlCfgInfo[MAXNUM_HDUCFG_CHNLNUM];
			memset(atHduChnlCfgInfo, 0x0, sizeof(atHduChnlCfgInfo));
			memcpy( (void*)atHduChnlCfgInfo, (void*)atHduChnlInfo, MAXNUM_HDUCFG_CHNLNUM*sizeof(THduChnlInfo));
			atHduStyleCfgInfo[byLp].SetHduChnlCfgTable( atHduChnlCfgInfo);
		    atHduStyleCfgInfo[byLp].SetTotalChnlNum( atHduStyleInfoTable[byLp].GetTotalChnlNum() );
		    atHduStyleCfgInfo[byLp].SetIsBeQuiet( atHduStyleInfoTable[byLp].GetIsBeQuiet() );
			atHduStyleCfgInfo[byLp].SetSchemeAlias( atHduStyleInfoTable[byLp].GetSchemeAlias() );
		    atHduStyleCfgInfo[byLp].SetStyleIdx( atHduStyleInfoTable[byLp].GetStyleIdx() );
		    atHduStyleCfgInfo[byLp].SetVolumn( atHduStyleInfoTable[byLp].GetVolumn() );
			atHduStyleCfgInfo[byLp].SetHeight( atHduStyleInfoTable[byLp].GetHeight() );
            atHduStyleCfgInfo[byLp].SetWidth( atHduStyleInfoTable[byLp].GetWidth() );
		}
		cHduSchemeMsg.SetMsgBody((u8*)atHduStyleCfgInfo, sizeof(atHduStyleCfgInfo));
		SendMsg2Mcsssn(cHduSchemeMsg, pcMsg->event + 1);
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[ProcMcsGetHduScheme]:ReadHduSchemeTable fail! wRet = %d\n", wRet);
		cHduSchemeMsg.SetErrorCode(wRet);
		SendMsg2Mcsssn(cHduSchemeMsg, pcMsg->event + 2);
	}

    NotifyMcsHduSchemeCfg(cHduSchemeMsg);

	return;
}

/*=============================================================================
  函 数 名： ProcMcsChangeHduScheme
  功    能： 修改HDU预案方案信息
  算法实现： 
  全局变量： 
  参    数： const CMessage* pcMsg
  返 回 值： void 
------------------------------------------------------------------------------
  日 期      版 本      修改人      走读人       修改内容
 2009/01/10   4.6       江乐斌                    创建
=============================================================================*/
void  CMcuCfgInst::ProcMcsChangeHduScheme(const CMessage* pcMsg)
{
	CServMsg cHduSchemeMsg(pcMsg->content,pcMsg->length);
    u8 byHduSchemeNum =0;

	THduStyleCfgInfo *ptHduStyleInfoTable = (THduStyleCfgInfo*)cHduSchemeMsg.GetMsgBody();
    byHduSchemeNum = cHduSchemeMsg.GetMsgBodyLen() / sizeof(THduStyleCfgInfo);

    if (byHduSchemeNum > MAX_HDUSTYLE_NUM)
    {
		OspPrintf(TRUE, FALSE, "[CMcuCfgInst]:HduScheme Num is too big!\n");
        MCUCFG_INVALIDPARAM_NACK(cHduSchemeMsg, pcMsg->event + 2, ERR_MCU_CFG_HDUSCHEME_NUM_OVER)
        return;
    }

	THduStyleInfo atHduStyleInfo[MAX_HDUSTYLE_NUM];

	u8 byOldHduSchemeNum = 0;
	THduStyleInfo atOldHduSchemeTab[MAX_HDUSTYLE_NUM];
	u16 wReadRet = g_cMcuAgent.ReadHduSchemeTable( &byOldHduSchemeNum, atOldHduSchemeTab );
	if ( SUCCESS_AGENT != wReadRet)
	{
		CfgLog("[ProcMcsChangeHduScheme]: Read hdu scheme fail!\n");
		cHduSchemeMsg.SetErrorCode(wReadRet);
		SendMsg2Mcsssn(cHduSchemeMsg, pcMsg->event + 2);
	}
    
	// 如果该预案有通道正在被使用，则不能被修改
	THduChnlInfo tOldHduChnlInfo[MAXNUM_HDUCFG_CHNLNUM];
	THduChnlCfgInfo  atHduChnlCfg[MAXNUM_HDUCFG_CHNLNUM];
    for ( u8 bySchemeIdx = 0; bySchemeIdx < byOldHduSchemeNum; bySchemeIdx ++ )
    {
        ptHduStyleInfoTable[bySchemeIdx].GetHduChnlCfgTable(atHduChnlCfg);
		
		TPeriEqpStatus tHduStatus;
		u8 byOldHduEqpId = 0;
 		u8 byOldHduChnlIdx = 0;
		u8 byHduEqpId = 0;
		u8 byHduChnlIdx = 0;
		atOldHduSchemeTab[bySchemeIdx].GetHduChnlTable( tOldHduChnlInfo );
		for ( u8 byIndex = 0; byIndex < MAXNUM_HDUCFG_CHNLNUM; byIndex ++ )
		{
			byOldHduEqpId = tOldHduChnlInfo[byIndex].GetEqpId();
			byOldHduChnlIdx = tOldHduChnlInfo[byIndex].GetChnlIdx();
			byHduEqpId = atHduChnlCfg[byIndex].GetEqpId();
			byHduChnlIdx = atHduChnlCfg[byIndex].GetChnlIdx();
			if ( byOldHduEqpId >= HDUID_MIN && byOldHduEqpId <= HDUID_MAX )
			{
				g_cMcuVcApp.GetPeriEqpStatus( byOldHduEqpId, &tHduStatus );
				if ( !tHduStatus.m_tStatus.tHdu.atVideoMt[byOldHduChnlIdx].IsNull() 
					&&( byOldHduEqpId != byHduEqpId || byOldHduChnlIdx != byHduChnlIdx )
					&& ptHduStyleInfoTable[bySchemeIdx].GetStyleIdx() == \
					tHduStatus.m_tStatus.tHdu.atHduChnStatus[byOldHduChnlIdx].GetSchemeIdx() 
					)
				{
			        CfgLog("[ProcMcsChangeHduScheme] current hdu=%d chnl=%d is runing,can not change scheme!\n", 
						byOldHduChnlIdx, byOldHduChnlIdx);
					cHduSchemeMsg.SetConfIdx( tHduStatus.m_tStatus.tHdu.atVideoMt[byOldHduChnlIdx].GetConfIdx() );
					MCUCFG_INVALIDPARAM_NACK( cHduSchemeMsg, pcMsg->event + 2, ERR_MCU_CFG_CHANGEHDUSCHEME )
					NotifyMcsHduSchemeCfg(cHduSchemeMsg);
				    return;
				}
				// 修改预案的宽高也需NACK
				if ( !tHduStatus.m_tStatus.tHdu.atVideoMt[byOldHduChnlIdx].IsNull() && 
					( ptHduStyleInfoTable[bySchemeIdx].GetHeight() != atOldHduSchemeTab[bySchemeIdx].GetHeight()
					|| ptHduStyleInfoTable[bySchemeIdx].GetWidth() != atOldHduSchemeTab[bySchemeIdx].GetWidth() ) )
				{
					CfgLog("[ProcMcsChangeHduScheme] current hdu scheme:%s is runing, can not change it!\n", 
						atOldHduSchemeTab[bySchemeIdx].GetSchemeAlias());
					cHduSchemeMsg.SetConfIdx( tHduStatus.m_tStatus.tHdu.atVideoMt[byOldHduChnlIdx].GetConfIdx() );
					MCUCFG_INVALIDPARAM_NACK( cHduSchemeMsg, pcMsg->event + 2, ERR_MCU_CFG_CHANGEHDUSCHEME )
						NotifyMcsHduSchemeCfg(cHduSchemeMsg);
					return;
				}

            }
		}

    }
	
    for (u8 byIdx = 0; byIdx < byHduSchemeNum; byIdx ++)
    {
       	THduChnlCfgInfo  atHduChnlCfgInfo[MAXNUM_HDUCFG_CHNLNUM];
        ptHduStyleInfoTable[byIdx].GetHduChnlCfgTable(atHduChnlCfgInfo);
		
        THduChnlInfo atHduChnlInfo[MAXNUM_HDUCFG_CHNLNUM];
        memset(atHduChnlInfo, 0x0, sizeof(atHduChnlInfo));
        memcpy( (void*)atHduChnlInfo, (void*)atHduChnlCfgInfo, MAXNUM_HDUCFG_CHNLNUM*sizeof(THduChnlCfgInfo));
        BOOL32 bVoidScheme = TRUE;  // 是否为空预案
		for ( u8 byIdx2 = 0; byIdx2 < MAXNUM_HDUCFG_CHNLNUM; byIdx2 ++ )
		{
			if ( atHduChnlCfgInfo[byIdx2].GetEqpId() >= HDUID_MIN &&  atHduChnlCfgInfo[byIdx2].GetEqpId() <= HDUID_MAX)
			{
				bVoidScheme = FALSE;
			}			
		}
        //如果所配预案有空预案则NACK
        if ( bVoidScheme )
        {
			CfgLog("[ProcMcsChangeHduScheme] There is void scheme , can not change!\n");
			cHduSchemeMsg.SetConfIdx( g_cMcuVcApp.GetConfIdx( cHduSchemeMsg.GetConfId() ) );
			MCUCFG_INVALIDPARAM_NACK( cHduSchemeMsg, pcMsg->event + 2, ERR_MCU_CFG_HDUSCHEMEVOID )
				NotifyMcsHduSchemeCfg(cHduSchemeMsg);
			return;
        }

        atHduStyleInfo[byIdx].SetHduChnlTable(atHduChnlInfo);
		atHduStyleInfo[byIdx].SetStyleIdx(ptHduStyleInfoTable[byIdx].GetStyleIdx());
        atHduStyleInfo[byIdx].SetTotalChnlNum(ptHduStyleInfoTable[byIdx].GetTotalChnlNum());
        atHduStyleInfo[byIdx].SetHeight(ptHduStyleInfoTable[byIdx].GetHeight());
        atHduStyleInfo[byIdx].SetWidth(ptHduStyleInfoTable[byIdx].GetWidth());
        atHduStyleInfo[byIdx].SetVolumn(ptHduStyleInfoTable[byIdx].GetVolumn());
        atHduStyleInfo[byIdx].SetSchemeAlias(ptHduStyleInfoTable[byIdx].GetSchemeAlias());
        atHduStyleInfo[byIdx].SetIsBeQuiet(ptHduStyleInfoTable[byIdx].GetIsBeQuiet());

    }

	u16 wRet = g_cMcuAgent.WriteHduSchemeTable(byHduSchemeNum, atHduStyleInfo);
	if (SUCCESS_AGENT != wRet)
	{
	    CfgLog("[ProcMcsChangeHduScheme]: write hdu scheme fail!\n");
		cHduSchemeMsg.SetErrorCode(wRet);
	    SendMsg2Mcsssn(cHduSchemeMsg, pcMsg->event + 2);
		return;
	}

    SendMsg2Mcsssn(cHduSchemeMsg, pcMsg->event + 1);

    NotifyMcsHduSchemeCfg(cHduSchemeMsg);
	
	return;
}

/*=============================================================================
    函 数 名： ProcMcuSynMcsCfgNtf
    功    能： 主备发生切换时,新的主用板将界面化配置信息重新通知当前所有会控
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/31  4.0			万春雷                  创建
=============================================================================*/
void CMcuCfgInst::ProcMcuSynMcsCfgNtf(const CMessage* pcMsg)
{
	NotifyMcsCpuPercentage();
	NotifyMcsEqpCfg();
	NotifyMcsBrdCfg();
	NotifyMcsGeneralCfg();

	return;
}

/*=============================================================================
函 数 名： ProcDscCfgWaitingChangeOverTimerOut
功    能： 
算法实现： 
全局变量： 
参    数： const CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/2   4.0			周广程                  创建
=============================================================================*/
void CMcuCfgInst::ProcDscCfgWaitingChangeOverTimerOut( const CMessage * const pcMsg )
{
	KillTimer(MCUCFG_DSCCFG_WAITINGCHANGE_OVER_TIMER);
	CServMsg cServMsg;
	memcpy( &cServMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
	MCUCFG_INVALIDPARAM_NACK(cServMsg, MCU_MCS_CHANGEBRDCFG_NACK, ERR_MCU_CFG_WRITE_DSC);
/*
#ifdef _MINIMCU_
	CfgLog( "[ProcDscCfgWaitingChangeOverTimerOut] Timer over!\n");

	TDSCModuleInfo tLastDscInfo;
	g_cMcuAgent.GetLastDscInfo( &tLastDscInfo );
	g_cMcuAgent.SetDscInfo( &tLastDscInfo, TRUE, FALSE );

	//brd cfg
	u32 dwMpcBrdIp = g_cMcuAgent.GetMpcIp(); // 网络序
	u32 dwDscExtIp = tLastDscInfo.GetDscExtIp(); // 主机序
	TBoardInfo atBrdInfo[MAXNUM_BOARD];
	u8 byBrdNum = 0;
	u16 wRet = g_cMcuAgent.ReadBrdTable(&byBrdNum, atBrdInfo);
	if (SUCCESS_AGENT != wRet || 0 == byBrdNum || byBrdNum > MAXNUM_BOARD)
	{
		CfgLog("[ProcDscCfgWaitingChangeOverTimerOut] read brd info failed! brd num :%d, wRet :%d\n", byBrdNum, wRet);
	}
	for( u8 byLop = 0; byLop < byBrdNum; byLop++ )
	{
		if ( atBrdInfo[byLop].GetType() ==  DSL8000_BRD_MPC )
		{
			atBrdInfo[byLop].SetBrdIp( ntohl(dwMpcBrdIp) );
			continue;
		}
		if ( atBrdInfo[byLop].GetType() == KDV8000B_MODULE 
			|| atBrdInfo[byLop].GetType() == DSL8000_BRD_MDSC 
			|| atBrdInfo[byLop].GetType() == DSL8000_BRD_HDSC
		   )
		{
			atBrdInfo[byLop].SetBrdIp( dwDscExtIp );
			continue;
		}
	}
	wRet = g_cMcuAgent.WriteBrdTable( byBrdNum, atBrdInfo );
	if ( SUCCESS_AGENT != wRet )
	{
		CfgLog("[ProcDscCfgWaitingChangeOverTimerOut] Write brd info failed! \n" );
	}
	
	CServMsg cMsg;
	memcpy( &cMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
	cMsg.SetMsgBody(NULL, 0);
	cMsg.SetErrorCode(ERR_MCU_CFG_WRITE_DSC); 
	SendMsg2Mcsssn(cMsg, MCU_MCS_CHANGEBRDCFG_NACK); 
	memset( m_abyServMsgHead, 0, SERV_MSGHEAD_LEN );
	NEXTSTATE(STATE_IDLE);

	return;
#endif
*/
}

/*=============================================================================
函 数 名： ProcMcsDisconnectNotif
功    能： 
算法实现： 
全局变量： 
参    数： const CMessage * const pcMsg
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/11/9   4.0		周广程                  创建
=============================================================================*/
void  CMcuCfgInst::ProcMcsDisconnectNotif(const CMessage * const pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
#ifdef _MINIMCU_
	if ( STATE_IDLE == CurState() )
	{
		return;
	}
	else
	{
		CServMsg cTempMsg;
		memcpy( &cTempMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
		if ( cServMsg.GetSrcSsnId() != cTempMsg.GetSrcSsnId() )
		{
			return;
		}
		else
		{
			memset( m_abyServMsgHead, 0, sizeof(SERV_MSGHEAD_LEN) );
			NEXTSTATE(STATE_IDLE);
		}
	}
#endif
	return;
}

/*=============================================================================
函 数 名： ProcSetDscInfoRsp
功    能： 
算法实现： 
全局变量： 
参    数： const CMessage * const pcMsg
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/16   4.0			周广程                  创建
=============================================================================*/
void  CMcuCfgInst::ProcSetDscInfoRsp( const CMessage * const pcMsg )
{
	KillTimer(MCUCFG_DSCCFG_WAITINGCHANGE_OVER_TIMER);

	CServMsg cServMsg;
	memcpy( &cServMsg, m_abyServMsgHead, SERV_MSGHEAD_LEN );
	switch(pcMsg->event) 
	{
	case AGT_SVC_SETDSCINFO_ACK:
		SendMsg2Mcsssn( cServMsg, MCU_MCS_CHANGEBRDCFG_ACK);
		break;
	case AGT_SVC_SETDSCINFO_NACK:
		MCUCFG_INVALIDPARAM_NACK(cServMsg, MCU_MCS_CHANGEBRDCFG_NACK, ERR_MCU_CFG_WRITE_DSC);
		break;
	default:
		break;
	}
	return;
}

/*=============================================================================
函 数 名： ProcDscRegSucceedNotif
功    能： 
算法实现： 
全局变量： 
参    数： const CMessage* pcMsg
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/24   4.0			周广程                  创建
=============================================================================*/
void  CMcuCfgInst::ProcDscRegSucceedNotif( const CMessage* pcMsg )
{
	if ( NULL == pcMsg )
	{
		return;
	}
#ifdef _MINIMCU_
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	BOOL32 bIsConfiged;
	memcpy( &bIsConfiged, cServMsg.GetMsgBody(), sizeof(BOOL32) );

	TMcuStatus tMcuStatus;
	g_cMcuVcApp.GetMcuCurStatus( tMcuStatus );
	
	cServMsg.SetMsgBody((u8 *)&tMcuStatus, sizeof(tMcuStatus));
	SendMsg2Mcsssn(cServMsg, MCU_MCS_MCUSTATUS_NOTIF, TRUE);
	NotifyMcsBrdCfg();

	if ( !bIsConfiged )
	{
		cServMsg.SetMsgBody();
		SendMsg2Mcsssn(cServMsg, MCU_MCS_UNCFGDSCREG_NOTIF, TRUE);
	}
#endif
}

/*=============================================================================
    函 数 名： NotifyMcsCpuPercentage
    功    能： 通知当前所有会控 当前mcu的cpu占有率信息
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/31  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuCfgInst::NotifyMcsCpuPercentage( void )
{
	CServMsg cMsg;
	
	u8 byPercentage = g_cMcuAgent.GetCpuRate();
	CfgLog("[NotifyMcsCpuPercentage] cpu percentage :%d%\n", byPercentage);

	//notify all mcs
	cMsg.SetMsgBody(&byPercentage, sizeof(byPercentage));
	SendMsg2Mcsssn(cMsg, MCU_MCS_CPUPERCENTAGE_NOTIF, TRUE);

	return TRUE;
}

/*=============================================================================
    函 数 名： NotifyMcsEqpCfg
    功    能： 通知当前所有会控 当前mcu的设备配置信息
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/31  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuCfgInst::NotifyMcsEqpCfg( void )
{
	CServMsg cMsg;
	
	TMcuEqpCfg tEqpCfg;
	u8 byLocalLayer = 0;
	u8 byLocalSlot  = 0;
	g_cMcuAgent.GetLocalLayer(byLocalLayer);
	g_cMcuAgent.GetLocalSlot(byLocalSlot);
	tEqpCfg.SetMcuIpAddr(ntohl(g_cMcuAgent.GetMpcIp()));
	tEqpCfg.SetMcuSubNetMask(ntohl(g_cMcuAgent.GetMaskIp()));
	tEqpCfg.SetGWIpAddr(ntohl(g_cMcuAgent.GetGateway()));
	tEqpCfg.SetLayer(byLocalLayer);
	tEqpCfg.SetSlot(byLocalSlot);
	tEqpCfg.SetInterface( g_cMcuAgent.GetInterface() );
	
	if (g_bPrintCfgMsg)
	{
		CfgLog("[NotifyMcsEqpCfg] notifying config info as follow:\n");
		tEqpCfg.Print();
	}
	
	//notify all mcs
	cMsg.SetMsgBody((u8*)&tEqpCfg, sizeof(tEqpCfg));
	SendMsg2Mcsssn(cMsg, MCU_MCS_MCUEQPCFG_NOTIF, TRUE);
	
	return TRUE;
}

/*=============================================================================
    函 数 名： NotifyMcsBrdCfg
    功    能： 通知当前所有会控 当前mcu的单板配置信息
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/31  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuCfgInst::NotifyMcsBrdCfg( void )
{
	CServMsg cMsg;
	u16 wRet = 0;

	u16 wBrdLen = 0;
	u16 wMixLen = 0;
	u16 wRecLen = 0;
	u16 wBasLen = 0;
	u16 wTWLen = 0;
	u16 wVmpLen = 0;
	u16 wPrsLen = 0;
	u16 wMtwLen = 0;

	u8 byBrdNum = 0;
	u8 byMixNum = 0;
	u8 byRecNum = 0;
	u8 byBasNum = 0;
    u8 byBasHDNum = 0;
	u8 byTWNum = 0;
	u8 byVmpNum = 0;
	u8 byPrsNum = 0;
	u8 byMtwNum = 0;
	//4.6版本 新加模块 jlb
	u8 byHduNum = 0;
	u8 bySvmpNum = 0;
	u8 byDvmpNum = 0;
	u8 byMpuBasNum = 0;
	u8 byEbapNum = 0;
	u8 byEvpuNum = 0;
	
	s32 nIndex = 0;

	//brd cfg        
	TBoardInfo atBrdInfo[MAXNUM_BOARD];
	wRet = g_cMcuAgent.ReadBrdTable(&byBrdNum, atBrdInfo);
	if (SUCCESS_AGENT != wRet || 0 == byBrdNum || byBrdNum > MAXNUM_BOARD)
	{
		CfgLog("[NotifyMcsBrdCfg] read brd info failed! brd num :%d, wRet :%d\n", byBrdNum, wRet);
		return FALSE;
	}

	TBrdCfgInfo atBrdCfg[MAXNUM_BOARD];
	BrdCfgOut2In(atBrdInfo, atBrdCfg, byBrdNum); 
	cMsg.SetMsgBody(&byBrdNum, sizeof(u8));
	cMsg.CatMsgBody((u8*)atBrdCfg, byBrdNum*sizeof(TBrdCfgInfo));

	if (g_bPrintCfgMsg)
	{
		CfgLog("[NotifyMcsBrdCfg] notifying config info as follow:\n");
		for (nIndex = 0; nIndex < byBrdNum; nIndex++)
		{
			atBrdCfg[nIndex].Print();
		}
	}

	//mixer cfg
	TEqpMixerInfo atMixerInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadMixerTable(&byMixNum, atMixerInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byMixNum <= MAXNUM_PERIEQP)
		{
			TEqpMixerCfgInfo atMixerCfg[MAXNUM_PERIEQP];
			MixerCfgOut2In(atMixerInfo, atMixerCfg, byMixNum);
			cMsg.CatMsgBody(&byMixNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atMixerCfg, byMixNum*sizeof(TEqpMixerCfgInfo));

			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byMixNum; nIndex++)
				{
					atMixerCfg[nIndex].Print();
				}
			}
		}
	}      
	else
	{
		CfgLog("[NotifyMcsBrdCfg] read mixer info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//rec cfg
	TEqpRecInfo atRecInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadRecTable(&byRecNum, atRecInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byRecNum <= MAXNUM_PERIEQP)
		{
			TEqpRecCfgInfo atRecCfg[MAXNUM_PERIEQP];
			RecCfgOut2In(atRecInfo, atRecCfg, byRecNum);
			cMsg.CatMsgBody(&byRecNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atRecCfg, byRecNum*sizeof(TEqpRecCfgInfo));

			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byRecNum; nIndex++)
				{
					atRecCfg[nIndex].Print();
				}
			}
		}
	}   
	else
	{
		CfgLog("[NotifyMcsBrdCfg] read rec info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//tvwall cfg
	TEqpTVWallInfo atTWInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadTvTable(&byTWNum, atTWInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byTWNum <= MAXNUM_PERIEQP)
		{
			TEqpTvWallCfgInfo atTWCfg[MAXNUM_PERIEQP];
			TWCfgOut2In(atTWInfo, atTWCfg, byTWNum);
			cMsg.CatMsgBody(&byTWNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atTWCfg, byTWNum*sizeof(TEqpTvWallCfgInfo));

			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byTWNum; nIndex++)
				{
					atTWCfg[nIndex].Print();
				}
			}
		}
	}  
	else
	{
		CfgLog("[NotifyMcsBrdCfg] read tvwall info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//bas cfg
	TEqpBasInfo atBasInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadBasTable(&byBasNum, atBasInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byBasNum <= MAXNUM_PERIEQP)
		{
			TEqpBasCfgInfo atBasCfg[MAXNUM_PERIEQP];
			BasCfgOut2In(atBasInfo, atBasCfg, byBasNum);
			cMsg.CatMsgBody(&byBasNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atBasCfg, byBasNum*sizeof(TEqpBasCfgInfo));

			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byBasNum; nIndex++)
				{
					atBasCfg[nIndex].Print();
				}
			}
		}
	}        
	else
	{
		CfgLog("[NotifyMcsBrdCfg] read bas info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//vmp cfg
	TEqpVMPInfo atVmpInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadVmpTable(&byVmpNum, atVmpInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byVmpNum <= MAXNUM_PERIEQP)
		{
			TEqpVmpCfgInfo atVmpCfg[MAXNUM_PERIEQP];
			VmpCfgOut2In(atVmpInfo, atVmpCfg, byVmpNum);
			cMsg.CatMsgBody(&byVmpNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atVmpCfg, byVmpNum*sizeof(TEqpVmpCfgInfo));

			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byVmpNum; nIndex++)
				{
					atVmpCfg[nIndex].Print();
				}
			}
		}
	}        
	else
	{
		CfgLog("[NotifyMcsBrdCfg] read vmp info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//prs cfg
	TEqpPrsInfo atPrsInfo[MAXNUM_PERIEQP];
    wRet = g_cMcuAgent.ReadPrsTable(&byPrsNum, atPrsInfo);
	if (SUCCESS_AGENT == wRet)
	{
        if(byPrsNum <= MAXNUM_PERIEQP)
        {
            TPrsCfgInfo atPrsCfg[MAXNUM_PERIEQP];
            PrsCfgOut2In(atPrsInfo, atPrsCfg, byPrsNum);
            cMsg.CatMsgBody(&byPrsNum, sizeof(u8));
            cMsg.CatMsgBody((u8*)&atPrsCfg, byPrsNum*sizeof(TPrsCfgInfo));
            
            if (g_bPrintCfgMsg)
            {
                for(u8 byLp = 0; byLp < byPrsNum; byLp++)
                {
                    atPrsCfg[byLp].Print();
                }
            }
        }		
	}
	else
	{
		CfgLog("[NotifyMcsBrdCfg] read prs info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//mtw cfg
	TEqpMpwInfo atMtwInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadMpwTable(&byMtwNum, atMtwInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byMtwNum <= MAXNUM_PERIEQP)
		{
			TEqpMTvwallCfgInfo atMtwCfg[MAXNUM_PERIEQP];
			MtwCfgOut2In(atMtwInfo, atMtwCfg, byMtwNum);
			cMsg.CatMsgBody(&byMtwNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atMtwCfg, byMtwNum*sizeof(TEqpMTvwallCfgInfo));

			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byMtwNum; nIndex++)
				{
					atMtwCfg[nIndex].Print();
				}
			}
		}
	}
	else
	{
		CfgLog("[NotifyMcsBrdCfg] read mtw info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

    //basHD cfg
	TEqpBasHDInfo atBasHDInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadBasHDTable(&byBasHDNum, atBasHDInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if (byBasHDNum <= MAXNUM_PERIEQP)
		{
			TEqpBasHDCfgInfo atBasHDCfg[MAXNUM_PERIEQP];
			BasHDCfgOut2In(atBasHDInfo, atBasHDCfg, byBasHDNum);
			cMsg.CatMsgBody(&byBasHDNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atBasHDCfg, byBasHDNum*sizeof(TEqpBasHDCfgInfo));

			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byRecNum; nIndex++)
				{
					atBasHDCfg[nIndex].Print();
				}
			}
		}
	}   
	else
	{
		CfgLog("[NotifyMcsBrdCfg] read basHD info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

	//4.6版本 新加模块 jlb
    //svmp  cfg
	TEqpSvmpInfo atSvmpInfo[MAXNUM_PERIEQP];
	wRet = g_cMcuAgent.ReadSvmpTable(&bySvmpNum, atSvmpInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if ( bySvmpNum <= MAXNUM_PERIEQP )
		{
			TEqpSvmpCfgInfo atSvmpCfg[MAXNUM_PERIEQP];
			SvmpCfgOut2In(atSvmpInfo, atSvmpCfg, bySvmpNum);
			
			cMsg.CatMsgBody(&bySvmpNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atSvmpCfg, bySvmpNum*sizeof(TEqpSvmpCfgInfo));
			
			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < bySvmpNum; nIndex++)
				{
					atSvmpCfg[nIndex].Print();
				}
			}
		}
	}
	else
	{
		CfgLog("[NotifyMcsBrdCfg] read Svmp info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

    // MpuBas  cfg
	TEqpMpuBasInfo atMpuBasInfo[MAXNUM_PERIEQP];     
	wRet = g_cMcuAgent.ReadMpuBasTable(&byMpuBasNum, atMpuBasInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if ( byMpuBasNum <= MAXNUM_PERIEQP )
		{
			TEqpMpuBasCfgInfo atMpuBasCfg[MAXNUM_PERIEQP];
			MpuBasCfgOut2In(atMpuBasInfo, atMpuBasCfg, byMpuBasNum);
			cMsg.CatMsgBody(&byMpuBasNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atMpuBasCfg, byMpuBasNum*sizeof(TEqpMpuBasCfgInfo));
			
			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byMpuBasNum; nIndex++)
				{
					atMpuBasCfg[nIndex].Print();
				}
			}
		}
	}
	else
	{
		CfgLog("[NotifyMcsBrdCfg] read MpuBas info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

    // Ebap  cfg
	TEqpEbapInfo atEbapInfo[MAXNUM_PERIEQP];     
	wRet = g_cMcuAgent.ReadEbapTable(&byEbapNum, atEbapInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if ( byEbapNum <= MAXNUM_PERIEQP )
		{
			TEqpEbapCfgInfo atEbapCfg[MAXNUM_PERIEQP];
			EbapCfgOut2In(atEbapInfo, atEbapCfg, byEbapNum);
			cMsg.CatMsgBody(&byEbapNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atEbapCfg, byEbapNum*sizeof(TEqpEbapCfgInfo));
			
			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byEbapNum; nIndex++)
				{
					atEbapCfg[nIndex].Print();
				}
			}
		}
	}
	else
	{
		CfgLog("[NotifyMcsBrdCfg] read Ebap info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

    // Evpu  cfg
	TEqpEvpuInfo atEvpuInfo[MAXNUM_PERIEQP];     
	wRet = g_cMcuAgent.ReadEvpuTable(&byEvpuNum, atEvpuInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if ( byEvpuNum <= MAXNUM_PERIEQP )
		{
			TEqpEvpuCfgInfo atEvpuCfg[MAXNUM_PERIEQP];
			EvpuCfgOut2In(atEvpuInfo, atEvpuCfg, byEvpuNum);
			cMsg.CatMsgBody(&byEvpuNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atEvpuCfg, byEvpuNum*sizeof(TEqpEvpuCfgInfo));
			
			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byEvpuNum; nIndex++)
				{
					atEvpuCfg[nIndex].Print();
				}
			}
		}
	}
	else
	{
		CfgLog("[NotifyMcsBrdCfg] read Evpu info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

    //hdu  cfg
	TEqpHduInfo atHduInfo[MAXNUM_PERIHDU];
	wRet = g_cMcuAgent.ReadHduTable(&byHduNum, atHduInfo);
	if (SUCCESS_AGENT == wRet)
	{
		if ( byHduNum <= MAXNUM_PERIHDU )
		{
			TEqpHduCfgInfo atHduCfg[MAXNUM_PERIHDU];
			HduCfgOut2In(atHduInfo, atHduCfg, byHduNum);
			cMsg.CatMsgBody(&byHduNum, sizeof(u8));
			cMsg.CatMsgBody((u8*)atHduCfg, byHduNum*sizeof(TEqpHduCfgInfo));
			
			if (g_bPrintCfgMsg)
			{
				for (nIndex = 0; nIndex < byHduNum; nIndex++)
				{
					atHduCfg[nIndex].Print();
				}
			}
		}
	}
	else
	{
		CfgLog("[NotifyMcsBrdCfg] read Hdu info failed, wRet:%d!\n", wRet);
		return FALSE;
	}

//     // dvmp  cfg
// 	TEqpDvmpBasicInfo atDvmpInfo[MAXNUM_PERIEQP];      // 读取双VMP时可以一次读16个
// 	wRet = g_cMcuAgent.ReadDvmpTable(&byDvmpNum, atDvmpInfo);
// 	if (SUCCESS_AGENT == wRet)
// 	{
// 		if ( byDvmpNum <= MAXNUM_PERIEQP )
// 		{
// 			TEqpDvmpCfgInfo atDvmpCfgInfo[MAXNUM_PERIDVMP];
// 			TEqpDvmpCfgBasicInfo atDvmpCfg[MAXNUM_PERIEQP];
// 			DvmpCfgOut2In(atDvmpInfo, atDvmpCfg, byDvmpNum);
// 			for ( nIndex=0; nIndex<byDvmpNum; nIndex++ )
// 			{
// 				memcpy( &atDvmpCfgInfo[nIndex/2].tEqpDvmpCfgBasicInfo[0], &atDvmpCfg[nIndex], sizeof(TEqpDvmpCfgBasicInfo) );
//                 memcpy( &atDvmpCfgInfo[nIndex/2].tEqpDvmpCfgBasicInfo[1], &atDvmpCfg[nIndex+1], sizeof(TEqpDvmpCfgBasicInfo) );
// 				nIndex += 2;
// 			}
//             if ( byDvmpNum%2 == 0 )
//             {
//                 byDvmpNum /= 2;
//             }
// 			else                          //当只有奇数个vmp时
// 			{
// 				byDvmpNum = byDvmpNum/2 + 1;
// 			}			
// 			cMsg.CatMsgBody(&byDvmpNum, sizeof(u8));
// 			cMsg.CatMsgBody((u8*)atDvmpCfgInfo, byDvmpNum*sizeof(TEqpDvmpCfgInfo));   
// 			
// 			if (g_bPrintCfgMsg)
// 			{
// 				for ( nIndex=0; nIndex<byDvmpNum; nIndex++ )
// 				{
// 					atDvmpCfgInfo[nIndex].tEqpDvmpCfgBasicInfo[0].Print();
// 					atDvmpCfgInfo[nIndex].tEqpDvmpCfgBasicInfo[1].Print();
// 				}
// 			}
// 		}
// 	}
// 	else
// 	{
// 		CfgLog("[NotifyMcsBrdCfg] read Dvmp info failed, wRet:%d!\n", wRet);
// 		return FALSE;
// 	}

	//notify all mcs
	SendMsg2Mcsssn(cMsg, MCU_MCS_BRDCFG_NOTIF, TRUE);

	return TRUE;
}

/*=============================================================================
    函 数 名： NotifyMcsHduSchemeCfg
    功    能： 通知当前所有会控 当前hdu预案的配置信息基本设置
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
   2009/02/10   4.6			江乐斌                  创建
=============================================================================*/
BOOL32	CMcuCfgInst::NotifyMcsHduSchemeCfg( CServMsg &cMsg )
{
	u16 wRet = 0;
	u8  byHduSchemeNum = 0;
    THduStyleInfo atHduStyleInfo[MAX_HDUSTYLE_NUM];
	
	wRet = g_cMcuAgent.ReadHduSchemeTable(&byHduSchemeNum,atHduStyleInfo);
	if (SUCCESS_AGENT != wRet)
	{
		CfgLog("[NotifyMcsHduSchemeCfg]read hdu Scheme failed!\n");
		return FALSE;
	}

	THduStyleCfgInfo atHduStyleCfgInfo[MAX_HDUSTYLE_NUM];
	memset( atHduStyleCfgInfo, 0x0, sizeof(atHduStyleCfgInfo) );
	for ( s32 nIndex = 0; nIndex < byHduSchemeNum; nIndex++ )
	{
		THduChnlInfo  atHduChnlInfo[MAXNUM_HDUCFG_CHNLNUM];

		atHduStyleInfo[nIndex].GetHduChnlTable(atHduChnlInfo);
			
		THduChnlCfgInfo atHduChnlCfgInfo[MAXNUM_HDUCFG_CHNLNUM];
		memset(atHduChnlCfgInfo, 0x0, sizeof(atHduChnlCfgInfo));
		memcpy( (void*)atHduChnlCfgInfo, (void*)atHduChnlInfo, MAXNUM_HDUCFG_CHNLNUM*sizeof(THduChnlInfo));

		atHduStyleCfgInfo[nIndex].SetHduChnlCfgTable( atHduChnlCfgInfo);
		atHduStyleCfgInfo[nIndex].SetTotalChnlNum( atHduStyleInfo[nIndex].GetTotalChnlNum() );
		atHduStyleCfgInfo[nIndex].SetIsBeQuiet( atHduStyleInfo[nIndex].GetIsBeQuiet() );
		atHduStyleCfgInfo[nIndex].SetSchemeAlias( atHduStyleInfo[nIndex].GetSchemeAlias() );
		atHduStyleCfgInfo[nIndex].SetStyleIdx( atHduStyleInfo[nIndex].GetStyleIdx() );
		atHduStyleCfgInfo[nIndex].SetVolumn( atHduStyleInfo[nIndex].GetVolumn() );
		atHduStyleCfgInfo[nIndex].SetHeight( atHduStyleInfo[nIndex].GetHeight() );
        atHduStyleCfgInfo[nIndex].SetWidth( atHduStyleInfo[nIndex].GetWidth() );				
	}

	cMsg.SetMsgBody((u8*)atHduStyleCfgInfo, byHduSchemeNum*sizeof(THduStyleCfgInfo));

	SendMsg2Mcsssn(cMsg, MCU_MCS_HDUSCHEMEINFO_NOTIF);
		
	return TRUE;

}

/*=============================================================================
    函 数 名： NotifyMcsGeneralCfg
    功    能： 通知当前所有会控 当前mcu的配置信息基本设置
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/31  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuCfgInst::NotifyMcsGeneralCfg( void )
{
	CServMsg cMsg;
	u16 wRet = 0;

	TAgentMcuGenCfg tAgentGenCfg;
	wRet = g_cMcuAgent.ReadVmpAttachTable(&tAgentGenCfg.m_byVmpStyleNum, tAgentGenCfg.m_atVmpStyle);
	if (SUCCESS_AGENT != wRet)
	{
		CfgLog("[NotifyMcsGeneralCfg] read vmp style failed, ret:%d!\n", wRet);
	}

    wRet = g_cMcuAgent.GetLocalInfo(&tAgentGenCfg.m_tLocal);
	if (SUCCESS_AGENT != wRet)
	{
		CfgLog("[NotifyMcsGeneralCfg] read local info failed, ret:%d!\n", wRet);
	}

	wRet = g_cMcuAgent.GetNetWorkInfo(&tAgentGenCfg.m_tNet);
	if (SUCCESS_AGENT != wRet)
	{
		CfgLog("[NotifyMcsGeneralCfg] read net info failed, ret:%d!\n", wRet);
	}

	wRet = g_cMcuAgent.ReadTrapTable(&tAgentGenCfg.m_byTrapListNum ,tAgentGenCfg.m_atTrapList);
	if (SUCCESS_AGENT != wRet)
	{
		CfgLog("[NotifyMcsGeneralCfg] read trap info failed, ret:%d!\n", wRet);
	}

	wRet = g_cMcuAgent.GetQosInfo(&tAgentGenCfg.m_tQos);
	if (SUCCESS_AGENT != wRet)
	{
		CfgLog("[NotifyMcsGeneralCfg] read qos info failed, ret:%d!\n", wRet);
	}

	wRet = g_cMcuAgent.GetNetSyncInfo(&tAgentGenCfg.m_tNetSyn);
	if (SUCCESS_AGENT != wRet)
	{
		CfgLog("[NotifyMcsGeneralCfg] read net syn info failed, ret:%d!\n", wRet);
	}

	tAgentGenCfg.m_dwDcsIp = g_cMcuAgent.GetDcsIp();

#ifdef _MINIMCU_
	g_cMcuAgent.GetDscInfo(&tAgentGenCfg.m_tDscInfo);
#endif

	wRet = g_cMcuAgent.GetLoginInfo(&tAgentGenCfg.m_tLoginInfo);
	if (SUCCESS_AGENT != wRet)
	{
		CfgLog("[NotifyMcsGeneralCfg] read telnet login info failed, ret:%d!\n", wRet);
	}
 
	TMcuGeneralCfg tGenCfg;
	if (!GeneralCfgOut2In(tAgentGenCfg, tGenCfg))
	{
		return FALSE;
	}
	else
	{
		if (g_bPrintCfgMsg)
		{
			CfgLog("[NotifyMcsGeneralCfg] notifying config info as follow:\n");
			tGenCfg.Print();
		}
	}

	//notify to all mcs
	cMsg.SetMsgBody((u8*)&tGenCfg, sizeof(tGenCfg));
	SendMsg2Mcsssn(cMsg, MCU_MCS_MCUGENERALCFG_NOTIF, TRUE);

	return TRUE;
}

/*=============================================================================
  函 数 名： NotifyMcsBoardStatus
  功    能： 通知Mcs单板状态
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
=============================================================================*/
void CMcuCfgInst::NotifyMcsBoardStatus(const CMessage * pcMsg)
{
    CServMsg cServMsg;
    if (AGT_SVC_BOARDSTATUS_NOTIFY == pcMsg->event)
    {
        TBrdStatus *ptBrdStatus = (TBrdStatus*)pcMsg->content;
        TBoardStatusNotify tBrdStatusNotify;
        tBrdStatusNotify.SetBrdState( BrdStatusOut2In(ptBrdStatus->byStatus) );
        tBrdStatusNotify.SetBrdLayer(ptBrdStatus->byLayer);
        tBrdStatusNotify.SetBrdSlot( ptBrdStatus->bySlot );
        tBrdStatusNotify.SetBrdType( BrdTypeOut2In(ptBrdStatus->byType) );
        tBrdStatusNotify.SetBrdOsType( ptBrdStatus->byOsType );

        CfgLog("Board Layer= %d, Slot= %d, Type= %d, state= %d OsType= %d status notify\n", 
                              tBrdStatusNotify.GetBrdLayer(), tBrdStatusNotify.GetBrdSlot(),
                              tBrdStatusNotify.GetBrdType(), tBrdStatusNotify.GetBrdState(),
                              tBrdStatusNotify.GetBrdOsType());
    
        cServMsg.SetMsgBody((u8*)&tBrdStatusNotify, sizeof(tBrdStatusNotify));
        SendMsg2Mcsssn(cServMsg, MCU_MCS_BOARDSTATUS_NOTIFY, TRUE);
    }
    else if (AGT_SVC_UPDATEBRDVERSION_NOTIF == pcMsg->event)
    {        
        cServMsg.SetServMsg(pcMsg->content, pcMsg->length);
		CfgLog("receive AGT_SVC_UPDATEBRDVERSION_NOTIF and begin to send to mcs MCU_MCS_UPDATEBRDVERSION_NOTIF!\n");
        SendMsg2Mcsssn(cServMsg, MCU_MCS_UPDATEBRDVERSION_NOTIF);
    }
	// 有新的DSC注册, zgc, 2007-03-05
//	else if (AGT_SVC_NEWDSCREGREQ_NOTIFY == pcMsg->event)
//	{
//		cServMsg.SetServMsg(pcMsg->content, pcMsg->length);
//		SendMsg2Mcsssn(cServMsg, MCU_MCS_NEWDSCREGREQ_NOTIFY, TRUE);
//	}

    return;
}

/*=============================================================================
  函 数 名： SendMsg2Mcsssn
  功    能： 发送消息到mcs会话
  算法实现： 
  全局变量： 
  参    数： CServMsg &cServMsg
  返 回 值： void  
=============================================================================*/
void CMcuCfgInst::SendMsg2Mcsssn(CServMsg &cServMsg, u16 wEvent, BOOL32 bAllMcsSsn)
{
	cServMsg.SetEventId(wEvent);

	if (bAllMcsSsn)
	{
		CMcsSsn::BroadcastToAllMcsSsn(wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
	}
	else
	{
		CMcsSsn::SendMsgToMcsSsn(cServMsg.GetSrcSsnId(), wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
	}

	CfgLog("send msg: %d<%s> to mcsssn, bAllMcs.%d \n", wEvent, OspEventDesc(wEvent), bAllMcsSsn);

	return;
}

/*=============================================================================
函 数 名： IsIpMaskValid
功    能： 
算法实现： 
全局变量： 
参    数： u32 dwIpMask(主机序)
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/24   4.0			周广程                  创建
=============================================================================*/
BOOL32 CMcuCfgInst::IsIpMaskValid(u32 dwIpMask)
{
	if ( dwIpMask == 0 )
	{
		CfgLog( "[IsIpMaskValid] IpMask = 0\n" );
		return FALSE;
	}
	s32 nLop1 = 0, nLop2 = 0;
	u8 byMaskBitNum = 0;
	for ( nLop1 = 31; nLop1 >= 0; nLop1--)
	{
		if ( ((dwIpMask>>nLop1)&0x00000001) == 1 )
		{
			byMaskBitNum++;
			continue;
		}
		else
		{
			if ( nLop1 == 31 ) //首位即为0
			{
				return FALSE;
			}
			else if( nLop1 == 0 ) //第一个0出现的位置是最后一位
			{
				return TRUE;
			}
			else
			{
				for ( nLop2 = nLop1-1; nLop2 >= 0; nLop2-- )
				{
					if ( ((dwIpMask>>nLop2)&0x00000001) == 1 )
					{
						return FALSE;
					}
				}
			}
			break;
		}
	}
	if ( byMaskBitNum > 30 )
	{
		return FALSE;
	}
	return TRUE;
}

/*=============================================================================
函 数 名： BrdCfgProcIntegration
功    能： 
算法实现： 
全局变量： 
参    数： const CServMsg &cServMsg
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/9/7   4.0			周广程                  创建
=============================================================================*/
BOOL32  CMcuCfgInst::BrdCfgProcIntegration(CServMsg &cServMsg)
{	
	u8 *pbyMsg = cServMsg.GetMsgBody();
	u16 wMsgLen = cServMsg.GetMsgBodyLen();
	u16 wRet = 0;

	u16 wBrdLen =   0;
	u16 wMixLen =   0;
	u16 wRecLen =   0;
	u16 wBasLen =   0;
    u16 wBasHDLen = 0;
	u16 wTWLen =    0;
	u16 wVmpLen =   0;
	u16 wPrsLen =   0;
	u16 wMtwLen =   0;
    //4.6版新加外设配置信息 jlb
    u16 wHdulen =   0;
	u16 wSvmplen =  0;
	u16 wDvmplen =  0;
	u16 wMpuBaslen = 0;
	u16 wEbaplen =  0;
	u16 wEvpulen =  0;
	u16 wTotalLen = 0;   //已解析的包的长度

	u8 byBrdNum =   0;
	u8 byMixNum =   0;
	u8 byRecNum =   0;
	u8 byBasNum =   0;
    u8 byBasHDNum = 0;
	u8 byTWNum =    0;
	u8 byVmpNum =   0;
	u8 byPrsNum =   0;
	u8 byMtwNum =   0;
    //4.6版新加外设配置信息 jlb
    u8 byHduNum =   0;
	u8 bySvmpNum =  0;
	u8 byDvmpNum =  0;
	u8 byMpuBasNum = 0;
	u8 byEbapNum =  0;
	u8 byEvpuNum =  0;

	BOOL32 bIsExistDsc = FALSE;
	u32 dwExpDscIp = 0; // 记录DSC的外部IP, zgc, 2007-07-04

	u32 nIndex = 0;
	u32 dwBrdLoop = 0;

	TBrdCfgInfo *ptBrdCfg =        NULL;
	TEqpMixerCfgInfo *ptMixerCfg = NULL;
	TEqpRecCfgInfo *ptRecCfg =     NULL;
	TEqpTvWallCfgInfo *ptTWCfg =   NULL;
	TEqpBasCfgInfo *ptBasCfg =     NULL;
    TEqpBasHDCfgInfo *ptBasHDCfg = NULL;
	TEqpVmpCfgInfo *ptVmpCfg =    NULL;
	TPrsCfgInfo *ptPrsCfg =        NULL;
	TEqpMTvwallCfgInfo *ptMtwCfg = NULL;
	//4.6版新加外设配置信息 jlb
	TEqpHduCfgInfo     *ptHduCfg =    NULL;
	TEqpSvmpCfgInfo    *ptSvmpCfg =   NULL;
	TEqpDvmpCfgInfo    *ptDvmpCfg =   NULL;
	TEqpMpuBasCfgInfo  *ptMpuBasCfg = NULL;
    TEqpEbapCfgInfo    *ptEbapCfg =   NULL;
	TEqpEvpuCfgInfo    *ptEvpuCfg =   NULL;

    //mpc板索引
    u8 byMpcIndex = 0;

	//brd info
	byBrdNum = *(pbyMsg + wTotalLen);
	wBrdLen = sizeof(u8) +byBrdNum*sizeof(TBrdCfgInfo);
	wTotalLen += wBrdLen;
	if (0 == byBrdNum || byBrdNum > MAXNUM_BOARD || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BRD)
		return FALSE;
	}
	else
	{
		ptBrdCfg = (TBrdCfgInfo *)(pbyMsg + (wTotalLen - wBrdLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byBrdNum; nIndex++)            
		{
			if (g_bPrintCfgMsg)
			{
				ptBrdCfg[nIndex].Print();
			}  
            
            if(BRD_TYPE_MPC == ptBrdCfg[nIndex].GetType())
            {
                byMpcIndex = ptBrdCfg[nIndex].GetIndex();
            }
//4.6 新加  jlb 
			if (BRD_TYPE_HDU == ptBrdCfg[nIndex].GetType() ||
				BRD_TYPE_HDU_L == ptBrdCfg[nIndex].GetType() ||
				BRD_TYPE_HDU2 == ptBrdCfg[nIndex].GetType() )
			{   
				
				u8 bySlot, bySlotType;
				ptBrdCfg[nIndex].GetSlot(bySlot, bySlotType);
				if (ptBrdCfg[nIndex].GetLayer() > MAXNUM_LAYER || 
					bySlot > MAXNUM_TVSSLOT || 
					0 == ptBrdCfg[nIndex].GetIpAddr())
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BRD)
						CfgLog("[ProcBrdCfg] hdu brd info is invalid!\n");
					return FALSE;
				}
			}
			
			u8 bySlot, bySlotType;
			ptBrdCfg[nIndex].GetSlot(bySlot, bySlotType);
			if (ptBrdCfg[nIndex].GetLayer() > MAXNUM_LAYER || 
				bySlot > MAXNUM_SLOT || 
				0 == ptBrdCfg[nIndex].GetIpAddr())
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BRD)
				CfgLog("[ProcBrdCfg] brd info is invalid!\n");
				return FALSE;
			}

#ifdef _MINIMCU_
			u8 byBrdType = ptBrdCfg[nIndex].GetType();
			if( BRD_TYPE_DSC == byBrdType ||
				BRD_TYPE_MDSC == byBrdType ||
				BRD_TYPE_HDSC == byBrdType )
			{
				bIsExistDsc = TRUE;
				dwExpDscIp = ptBrdCfg[nIndex].GetIpAddr();
			}
#endif
		} //for (nIndex = 0; nIndex < byBrdNum...
	} //if (0 == byBrdNum || byBrdNum > MAXNUM_BOARD...else...

	//mixer info
	byMixNum = *(pbyMsg + wTotalLen);
	wMixLen = sizeof(u8) + byMixNum*sizeof(TEqpMixerCfgInfo);
	wTotalLen += wMixLen;
	if (byMixNum > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MIXER)
		CfgLog("[ProcBrdCfg] mixer body is invalid!\n");
		return FALSE;
	}
	else if (byMixNum > 0)
	{
		ptMixerCfg = (TEqpMixerCfgInfo *)(pbyMsg + (wTotalLen - wMixLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byMixNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptMixerCfg[nIndex].Print();
			}                

			if (ptMixerCfg[nIndex].GetEqpId() < MIXERID_MIN || 
				ptMixerCfg[nIndex].GetEqpId() > MIXERID_MAX || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMixerCfg[nIndex].GetSwitchBrdIndex()) || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMixerCfg[nIndex].GetRunningBrdIndex()))
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MIXER)
				CfgLog("[ProcBrdCfg] mixer info is invalid!\n");
				return FALSE;
			}

            //只有8000B的mcu才能在mpc板上配置mixer
#ifndef _MINIMCU_
            if(ptMixerCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
            {
                MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MIXER)
                CfgLog("[ProcBrdCfg] only 8000B mcu can run mixer on mpc, mpc Index :%d!\n", byMpcIndex);
                return FALSE;
            }
#endif
			//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
			TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
			if ( !tPerfLimit.IsMpcRunMp() && ptMixerCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MIXER)
                CfgLog("[ProcBrdCfg] mix%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptMixerCfg[nIndex].GetEqpId(), byMpcIndex);
                return FALSE;
			}
#endif
		} // for (nIndex = 0; nIndex < byMixNum ...
	} // if (byMixNum > MAXNUM_PERIEQP || wMsgLen-wB... else...

	//rec info
	byRecNum = *(pbyMsg + wTotalLen);
	wRecLen = sizeof(u8) + byRecNum*sizeof(TEqpRecCfgInfo);
	wTotalLen += wRecLen;
	if (byRecNum  > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_REC)
		CfgLog("[ProcBrdCfg] rec body is invalid!\n");
		return FALSE;
	}
	else if (byRecNum > 0)
	{
		ptRecCfg = (TEqpRecCfgInfo *)(pbyMsg + (wTotalLen - wRecLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byRecNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptRecCfg[nIndex].Print();
			}                

			if (ptRecCfg[nIndex].GetEqpId() < RECORDERID_MIN || 
				ptRecCfg[nIndex].GetEqpId() > RECORDERID_MAX || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptRecCfg[nIndex].GetSwitchBrdIndex()))
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_REC)
				CfgLog("[ProcBrdCfg] rec info is invalid!\n");
				return FALSE;
			}
			//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
			TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
			if ( !tPerfLimit.IsMpcRunMp() && ptRecCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_REC)
                CfgLog("[ProcBrdCfg] rec%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptRecCfg[nIndex].GetEqpId(), byMpcIndex);
                return FALSE;
			}
#endif
		} // for (nIndex = 0; nIndex < byRe ...
	} //if (byRecNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLen-wMixLen ... else ...

	//tvwall info
	byTWNum = *(pbyMsg + wTotalLen);
	wTWLen = sizeof(u8) + byTWNum*sizeof(TEqpTvWallCfgInfo);
    wTotalLen += wTWLen;
	if (byTWNum  > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_TVWALL)
		CfgLog("[ProcBrdCfg] tvwall body is invalid!\n");
		return FALSE;
	}
	else if (byTWNum > 0)
	{
		ptTWCfg = (TEqpTvWallCfgInfo *)(pbyMsg + (wTotalLen - wTWLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byTWNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptTWCfg[nIndex].Print();
			}                

			if (ptTWCfg[nIndex].GetEqpId() < TVWALLID_MIN || 
				ptTWCfg[nIndex].GetEqpId() > TVWALLID_MAX || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptTWCfg[nIndex].GetRunningBrdIndex()))
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_TVWALL)
				CfgLog("[ProcBrdCfg] tvwall info is invalid!\n");
				return FALSE;
			}
			//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
			TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
			if ( !tPerfLimit.IsMpcRunMp() && ptTWCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_TVWALL)
                CfgLog("[ProcBrdCfg] tvwall%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptTWCfg[nIndex].GetEqpId(), byMpcIndex);
                return FALSE;
			}
#endif
		} // for (nIndex = 0; nIndex < byTW ...
	} //if (byTWNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLe ... else ...

	//bas info
	byBasNum = *(pbyMsg + wTotalLen);
	wBasLen = sizeof(u8) + byBasNum*sizeof(TEqpBasCfgInfo);
	wTotalLen += wBasLen;
	if (byBasNum  > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BAS)
		CfgLog("[ProcBrdCfg] bas body is invalid!\n");
		return FALSE;
	}
	else if (byBasNum > 0)
	{
		ptBasCfg = (TEqpBasCfgInfo *)(pbyMsg + (wTotalLen - wBasLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byBasNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptBasCfg[nIndex].Print();
			}

			if (ptBasCfg[nIndex].GetEqpId() < BASID_MIN || 
				ptBasCfg[nIndex].GetEqpId() > BASID_MAX || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptBasCfg[nIndex].GetRunningBrdIndex()) || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptBasCfg[nIndex].GetSwitchBrdIndex()))
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BAS)
				CfgLog("[ProcBrdCfg] bas info is invalid!\n");
				return FALSE;
			}

            //只有8000B的mcu才能在mpc板上配置bas
#ifndef _MINIMCU_
            if(ptBasCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
            {
                MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BAS)
                CfgLog("[ProcBrdCfg] only 8000B mcu can run bas on mpc, mpc Index :%d!\n", byMpcIndex);
                return FALSE;
            }
#endif
			//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
			TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
			if ( !tPerfLimit.IsMpcRunMp() && ptBasCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BAS)
                CfgLog("[ProcBrdCfg] bas%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptBasCfg[nIndex].GetEqpId(), byMpcIndex);
                return FALSE;
			}
#endif
			
		} // for (nIndex = 0; nIndex < byBasN ...
	} // if (byBasNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLe ... else ...

	//vmp info
	byVmpNum = *(pbyMsg + wTotalLen);
	wVmpLen = sizeof(u8) + byVmpNum*sizeof(TEqpVmpCfgInfo);
	wTotalLen += wVmpLen; 
	if (byVmpNum  > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_VMP)
		CfgLog("[ProcBrdCfg] vmp body is invalid!\n");
		return FALSE;
	}
	else if (byVmpNum > 0)
	{
		ptVmpCfg = (TEqpVmpCfgInfo *)(pbyMsg + (wTotalLen - wVmpLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byVmpNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptVmpCfg[nIndex].Print();
			}

			if (ptVmpCfg[nIndex].GetEqpId() < VMPID_MIN || 
				ptVmpCfg[nIndex].GetEqpId() > VMPID_MAX || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptVmpCfg[nIndex].GetRunningBrdIndex()) || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptVmpCfg[nIndex].GetSwitchBrdIndex()))
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_VMP)
				CfgLog("[ProcBrdCfg] vmp info is invalid!\n");
				return FALSE;
			}

            //只有8000B的mcu才能在mpc板上配置vmp
#ifndef _MINIMCU_
            if(ptVmpCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
            {
                MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_VMP)
                CfgLog("[ProcBrdCfg] only 8000B mcu can run vmp on mpc, mpc Index :%d!\n", byMpcIndex);
                return FALSE;
            }
#endif
			//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
			TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
			if ( !tPerfLimit.IsMpcRunMp() && ptVmpCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_VMP)
                CfgLog( "[ProcBrdCfg] vmp%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", 
				        ptVmpCfg[nIndex].GetEqpId(), byMpcIndex );
                return FALSE;
			}
#endif
		} // for (nIndex = 0; nIndex < byVmpNum; nInde ...
	} // if (byVmpNum  > MAXNUM_PERIEQP || wMsgLen-wBr ... else ...

	//prs info
	byPrsNum = *(pbyMsg + wTotalLen);
	wPrsLen = sizeof(u8) + byPrsNum*sizeof(TPrsCfgInfo);
	wTotalLen += wPrsLen;
	if (byPrsNum  > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_PRS)
		CfgLog("[ProcBrdCfg] prs body is invalid!\n");
		return FALSE;
	}
	else if (byPrsNum > 0)
	{
		ptPrsCfg = (TPrsCfgInfo *)(pbyMsg + (wTotalLen - wPrsLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byPrsNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptPrsCfg[nIndex].Print();
			}

			if (ptPrsCfg[nIndex].GetEqpId() < PRSID_MIN || 
				ptPrsCfg[nIndex].GetEqpId() > PRSID_MAX || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptPrsCfg[nIndex].GetRunningBrdIndex()) || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptPrsCfg[nIndex].GetSwitchBrdIndex()))
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_PRS)
				CfgLog("[ProcBrdCfg] prs info is invalid!\n");
				return FALSE;
			}

            //只有8000B的mcu才能在mpc板上配置prs
#ifndef _MINIMCU_
            if(ptPrsCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
            {
                MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_PRS)
                CfgLog("[ProcBrdCfg] only 8000B mcu can run prs on mpc, mpc Index :%d!\n", byMpcIndex);
                return FALSE;
            }
#endif
			//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
			TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
			if ( !tPerfLimit.IsMpcRunMp() && ptPrsCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_PRS)
                CfgLog("[ProcBrdCfg] prs%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptPrsCfg[nIndex].GetEqpId(), byMpcIndex);
                return FALSE;
			}
#endif
		} // for (nIndex = 0; nIndex < b ...
	} // if (byPrsNum  > MAXNUM_PERIEQP || wMsgL ... else ...

	//mtw信息
	byMtwNum = *(pbyMsg + wTotalLen);
	wMtwLen = sizeof(u8) + byMtwNum*sizeof(TEqpMTvwallCfgInfo);
	wTotalLen += wMtwLen;
	if (byMtwNum > MAXNUM_PERIEQP || wMsgLen < wTotalLen)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MTVWALL)
		CfgLog("[ProcBrdCfg] mtw body is invalid!\n");
		return FALSE;
	}   
	else if (byMtwNum > 0)
	{
		ptMtwCfg = (TEqpMTvwallCfgInfo *)(pbyMsg + (wTotalLen - wMtwLen) + sizeof(u8));
		for (nIndex = 0; nIndex < byMtwNum; nIndex++)
		{
			if (g_bPrintCfgMsg)
			{
				ptMtwCfg[nIndex].Print();
			}

			if (ptMtwCfg[nIndex].GetEqpId() < VMPTWID_MIN || 
				ptMtwCfg[nIndex].GetEqpId() > VMPTWID_MAX || 
				!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMtwCfg[nIndex].GetRunningBrdIndex()))
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MTVWALL)
				CfgLog("[ProcBrdCfg] mtw info is invalid!\n");
				return FALSE;
			}
			//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
			TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
			if ( !tPerfLimit.IsMpcRunMp() && ptMtwCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
			{
				MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MTVWALL)
                CfgLog("[ProcBrdCfg] mtw%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", 
				        ptMtwCfg[nIndex].GetEqpId(), byMpcIndex);
                return FALSE;
			}
#endif
		} // for (nIndex = 0; nIndex < by ...
	} // if (byMtwNum > MAXNUM_PERIEQP || wMsgLen-wBrdLen-w ... else ...



//zw[08/02/2008]
    //basHD info
    if (wMsgLen > wTotalLen)
    {
        byBasHDNum = *(pbyMsg + wTotalLen);
        wBasHDLen = sizeof(u8) + byBasHDNum * sizeof(TEqpBasHDCfgInfo);
        wTotalLen += wBasHDLen;
		if (byBasHDNum  > MAXNUM_PERIEQP || wMsgLen  < wTotalLen)
	    {
		    MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BASHD)
		    CfgLog("[ProcBrdCfg] basHD body is invalid!\n");
		    return FALSE;
	    }
        else if (byBasHDNum > 0)
	    {
		    ptBasHDCfg = (TEqpBasHDCfgInfo*)(pbyMsg + (wTotalLen - wBasHDLen) + sizeof(u8));
		    for (nIndex = 0; nIndex < byBasHDNum; nIndex++)
		    {
			    if (g_bPrintCfgMsg)
			    {
				    ptBasHDCfg[nIndex].Print();
			    }

			    if (ptBasHDCfg[nIndex].GetEqpId() < BASID_MIN || 
				    ptBasHDCfg[nIndex].GetEqpId() > BASID_MAX || 
				    !CheckBrdIndex(ptBrdCfg, byBrdNum, ptBasHDCfg[nIndex].GetSwitchBrdIndex()))
			    {
				    MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BASHD)
				    CfgLog("[ProcBrdCfg] basHD info is invalid!\n");
				    return FALSE;
			    }			    
		    } // for (nIndex = 0; nIndex < byBasHDN ...
	    } // if (byBasHDNum  > MAXNUM_PERIEQP || wMsgLen-wBrdLe ... else ...
    }
    else
    {
		OspPrintf(TRUE, FALSE, "[ProcBrdCfg] have no BasHD info!\n");
    }

	//4.6版本 新加外设 jlb

	//SVMP info jlb
	if (wMsgLen > wTotalLen)
	{
		bySvmpNum = *(pbyMsg + wTotalLen);
		wSvmplen = sizeof(u8) + bySvmpNum * sizeof(TEqpSvmpCfgInfo);
		wTotalLen += wSvmplen;
		if (bySvmpNum  > MAXNUM_PERIEQP || (wMsgLen  < wTotalLen))
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_SVMP)
				CfgLog("[ProcBrdCfg] SVMP body is invalid!\n");
			return FALSE;
		}
		else if (bySvmpNum > 0)
		{
			ptSvmpCfg = (TEqpSvmpCfgInfo*)(pbyMsg + (wTotalLen - wSvmplen) + sizeof(u8));
			for (nIndex = 0; nIndex < bySvmpNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptSvmpCfg[nIndex].Print();
				}				 
				if (ptSvmpCfg[nIndex].GetEqpId() < VMPID_MIN || 
					ptSvmpCfg[nIndex].GetEqpId() > VMPID_MAX ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptSvmpCfg[nIndex].GetRunningBrdIndex()) ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptSvmpCfg[nIndex].GetSwitchBrdIndex()))
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_SVMP)
						CfgLog("[ProcBrdCfg] SVMP info is invalid!\n");
					return FALSE;
				}
				
#ifndef _MINIMCU_
				if(ptSvmpCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_SVMP)
						CfgLog("[ProcBrdCfg] only 8000B mcu can run Svmp on mpc, mpc Index :%d!\n", byMpcIndex);
					return FALSE;
				}
#endif

			} 
		} 
		else
		{

		}
    }
	else
    {
		OspPrintf(TRUE, FALSE, "[ProcBrdCfg] have no SVMP info!\n");
    }

	//MpuBas info  jlb


	if (wMsgLen > wTotalLen)
	{
		byMpuBasNum = *(pbyMsg + wTotalLen);
		wMpuBaslen = sizeof(u8) + byMpuBasNum * sizeof(TEqpMpuBasCfgInfo);
		wTotalLen += wMpuBaslen;
		if (byMpuBasNum  > MAXNUM_PERIEQP || (wMsgLen  < wTotalLen))
		{
			OspPrintf(TRUE, FALSE, "[ProcBrdCfg]MpuBas  1\n");
			MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MPUBAS)
				CfgLog("[ProcBrdCfg] MpuBas body is invalid!\n");
			return FALSE;
		}
		else if (byMpuBasNum > 0)
		{
			ptMpuBasCfg = (TEqpMpuBasCfgInfo*)(pbyMsg + (wTotalLen- wMpuBaslen) + sizeof(u8));
			for (nIndex = 0; nIndex < byMpuBasNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptMpuBasCfg[nIndex].Print();
				}				 
				if (ptMpuBasCfg[nIndex].GetEqpId() < BASID_MIN ||
					ptMpuBasCfg[nIndex].GetEqpId() > BASID_MAX ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMpuBasCfg[nIndex].GetRunningBrdIndex()) ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptMpuBasCfg[nIndex].GetSwitchBrdIndex()) )
				{
					OspPrintf(TRUE, FALSE, "[ProcBrdCfg]MpuBas  2\n");
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MPUBAS)
						CfgLog("[ProcBrdCfg] MpuBas info is invalid!\n");
					return FALSE;
				}	
				
#ifndef _MINIMCU_
				if(ptMpuBasCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
				{
					OspPrintf(TRUE, FALSE, "[ProcBrdCfg]MpuBas  3\n");
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_MPUBAS)
						CfgLog("[ProcBrdCfg] only 8000B mcu can run MpuBas on mpc, mpc Index :%d!\n", byMpcIndex);
					return FALSE;
				}
#endif

			} 
		} 
		else
		{
			
		}
    }
	else
    {
		OspPrintf(TRUE, FALSE, "[ProcBrdCfg] have no MpuBas info!\n");
    }

	//Ebap info  jlb
	if (wMsgLen > wTotalLen)
	{
		byEbapNum = *(pbyMsg + wTotalLen);
		wEbaplen = sizeof(u8) + byEbapNum * sizeof(TEqpEbapCfgInfo);
		wTotalLen += wEbaplen;
		if ( byEbapNum  > MAXNUM_PERIEQP || 
			(wMsgLen < wTotalLen) )
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_EBAP)
				CfgLog("[ProcBrdCfg] Ebap body is invalid!\n");
			return FALSE;
		}
		else if (byEbapNum > 0)
		{
			ptEbapCfg = (TEqpEbapCfgInfo*)(pbyMsg + (wTotalLen - wEbaplen) + sizeof(u8));
			for (nIndex = 0; nIndex < byEbapNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptEbapCfg[nIndex].Print();
				}				 
				if (ptEbapCfg[nIndex].GetEqpId() < BASID_MIN ||
					ptEbapCfg[nIndex].GetEqpId() > BASID_MAX ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptEbapCfg[nIndex].GetRunningBrdIndex()) ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptEbapCfg[nIndex].GetSwitchBrdIndex()) )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_EBAP)
						CfgLog("[ProcBrdCfg] Ebap info is invalid!\n");
					return FALSE;
				}
				
#ifndef _MINIMCU_
				if(ptEbapCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_EBAP)
						CfgLog("[ProcBrdCfg] only 8000B mcu can run ebap on mpc, mpc Index :%d!\n", byMpcIndex);
					return FALSE;
				}
#endif

			} 
		} 
		else
		{
			
		}
    }
	else
    {
		OspPrintf(TRUE, FALSE, "[ProcBrdCfg] have no Ebap info!\n");
    }

	//Evpu info  jlb
	if (wMsgLen > wTotalLen)
	{
		byEvpuNum = *(pbyMsg + wTotalLen);
		wEvpulen = sizeof(u8) + byEvpuNum * sizeof(TEqpEvpuCfgInfo);
		wTotalLen += wEvpulen;
		if ( byEvpuNum  > MAXNUM_PERIEQP || 
			(wMsgLen  < wTotalLen) )
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_EVPU)
				CfgLog("[ProcBrdCfg] Evpu body is invalid!\n");
			return FALSE;
		}
		else if (byEvpuNum > 0)
		{
			ptEvpuCfg = (TEqpEvpuCfgInfo*)(pbyMsg + (wTotalLen - wEvpulen) + sizeof(u8));
			for (nIndex = 0; nIndex < byEvpuNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptEvpuCfg[nIndex].Print();
				}				 
				if (ptEvpuCfg[nIndex].GetEqpId() < VMPID_MIN ||
					ptEvpuCfg[nIndex].GetEqpId() > VMPID_MAX ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptEvpuCfg[nIndex].GetRunningBrdIndex()) ||
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptEvpuCfg[nIndex].GetSwitchBrdIndex()) )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_EVPU)
						CfgLog("[ProcBrdCfg] Evpu info is invalid!\n");
					return FALSE;
				}	
#ifndef _MINIMCU_
				if(ptEvpuCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_EVPU)
						CfgLog("[ProcBrdCfg] only 8000B mcu can run evpu on mpc, mpc Index :%d!\n", byMpcIndex);
					return FALSE;
				}
#endif
			} 
		} 
		else
		{
			
		}
    }
	else
    {
		OspPrintf(TRUE, FALSE, "[ProcBrdCfg] have no Evpu info!\n");
    }

	//HDU info jlb
	if (wMsgLen > wTotalLen)
	{
		byHduNum = *(pbyMsg + wTotalLen);
		wHdulen = sizeof(u8) + byHduNum * sizeof(TEqpHduCfgInfo);
        wTotalLen += wHdulen;
		if ( byHduNum  > MAXNUM_PERIHDU || (wMsgLen  < wTotalLen) )
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_HDU)
				CfgLog("[ProcBrdCfg] Hdu body is invalid!\n");
			return FALSE;
		}
		else if (byHduNum > 0)
		{
			ptHduCfg = (TEqpHduCfgInfo*)(pbyMsg + (wTotalLen - wHdulen) + sizeof(u8));
			for (nIndex = 0; nIndex < byHduNum; nIndex++)
			{
				if (g_bPrintCfgMsg)
				{
					ptHduCfg[nIndex].Print();
				}				 
				if ( ptHduCfg[nIndex].GetEqpId() < HDUID_MIN || 
					ptHduCfg[nIndex].GetEqpId() > HDUID_MAX || 
					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptHduCfg[nIndex].GetRunningBrdIndex()) )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_HDU)
						CfgLog("[ProcBrdCfg] HDU info is invalid!\n");
					return FALSE;
				}
        #ifdef _LINUX_                
            #ifndef _MINIMCU_
				if(ptHduCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_HDU)
						CfgLog("[ProcBrdCfg] only 8000B mcu can run Hdu on mpc, mpc Index :%d!\n", byMpcIndex);
					return FALSE;
				}
            #endif
        #endif
				//只有8000B/C运行了MP，才允许把MPC作为转发板，zgc, 2007-09-06
#ifdef _MINIMCU_
				TMcuPerfLimit tPerfLimit = g_cMcuVcApp.GetPerfLimit();
				if ( !tPerfLimit.IsMpcRunMp() && ptHduCfg[nIndex].GetSwitchBrdIndex() == byMpcIndex )
				{
					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_TVWALL)
						CfgLog("[ProcBrdCfg] tvwall%d can't set MPC as switch board if MPC don't run mp, mpc Index :%d!\n", ptTWCfg[nIndex].GetEqpId(), byMpcIndex);
					return FALSE;
				}
#endif				
			} 
		} 
    }
	else
    {
		OspPrintf(TRUE, FALSE, "[ProcBrdCfg] have no HDU info!\n");
    }

// 		//DVMP info  jlb
//  	if (wMsgLen > wTotalLen)
//  	{
//  		byDvmpNum = *(pbyMsg + wTotalLen);
//  		wDvmplen = sizeof(u8) + byDvmpNum * sizeof(TEqpDvmpCfgInfo);
//  		wTotalLen += wDvmplen;
//  		if (byDvmpNum  > MAXNUM_PERIDVMP || (wMsgLen < wTotalLen))
//  		{
//  			MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_DVMP)
//  				CfgLog("[ProcBrdCfg] DVMP body is invalid!\n");
//  			return FALSE;
//  		}
//  		else if (byDvmpNum > 0)
//  		{
//  			ptDvmpCfg = (TEqpDvmpCfgInfo*)(pbyMsg + (wTotalLen - wDvmplen) + sizeof(u8));
//  			for (nIndex = 0; nIndex < byDvmpNum; nIndex++)
//  			{
//  				if (g_bPrintCfgMsg)
//  				{
//  					ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[0].Print();
//  					ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[1].Print();
//  				}				 
//  				if ( ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[0].GetEqpId() < VMPID_MIN ||
//  					 ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[0].GetEqpId() > VMPID_MAX ||
//  					 !CheckBrdIndex(ptBrdCfg, byBrdNum, ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[0].GetRunningBrdIndex()) ||
// 					 !CheckBrdIndex(ptBrdCfg, byBrdNum, ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[0].GetSwitchBrdIndex()) )
//  				{
//  					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_DVMP)
//  						CfgLog("[ProcBrdCfg] DVMP[0] info is invalid!\n");
//  					return FALSE;
//  				}
//  				if ( ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[1].GetEqpId() < VMPID_MIN ||
//  					 ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[1].GetEqpId() > VMPID_MAX ||
//  					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[1].GetRunningBrdIndex()) ||
//  					!CheckBrdIndex(ptBrdCfg, byBrdNum, ptDvmpCfg[nIndex].tEqpDvmpCfgBasicInfo[1].GetSwitchBrdIndex()) )
//  				{
//  					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_DVMP)
//  						CfgLog("[ProcBrdCfg] DVMP[1] info is invalid!\n");
//  					return FALSE;
//  				}
//  /*
// // #ifndef _MINIMCU_
// // 				if(ptPrsCfg[nIndex].GetRunningBrdIndex() == byMpcIndex)
// // 				{
// // 					MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_PRS)
// // 						CfgLog("[ProcBrdCfg] only 8000B mcu can run Dvmp on mpc, mpc Index :%d!\n", byMpcIndex);
// // 					return FALSE;
// // 				}
// // #endif
// // */
//  			} 
//  		} 
//  		else
//  		{
//  			
//  		}
//      }
//  	else
//      {
//  		OspPrintf(TRUE, FALSE, "[ProcBrdCfg] have no DVMP info!\n");
//      }


	// 变更消息体，TDSCModuleInfo结构从MCS_MCU_GETMCUGENERALCFG_REQ消息中转移到MCS_MCU_GETBRDCFG_REQ, zgc, 2007-03-15
 #ifdef _MINIMCU_	
	T8000BDscMod tDscInfo = *(T8000BDscMod*)( pbyMsg + wTotalLen);	
 	TDSCModuleInfo tAgentDscInfo;
 	DscInfoIn2Out( tDscInfo, tAgentDscInfo );
 	if( g_bPrintCfgMsg )
 	{
 		OspPrintf(TRUE, FALSE, "T8000BDscMod:\n");
 		tDscInfo.Print();
 		OspPrintf(TRUE, FALSE, "TDSCModuleInfo:\n");
 		tAgentDscInfo.Print();
 	}
 	//	BOOL32 bIsRegDsc = FALSE;
 	//	if ( g_cMcuAgent.IsRegedBoard( g_cMcuAgent.GetConfigedDscType(), 0, 0 ) )
 	//	{
 	//		bIsRegDsc = TRUE;
 	//	}
#endif

	//change brd info        
	TBoardInfo atBrdInfo[MAXNUM_BOARD]; 
	BrdCfgIn2Out(ptBrdCfg, atBrdInfo, byBrdNum);		

	//change mixer info   
	TEqpMixerInfo atMixerInfo[MAXNUM_PERIEQP];
	if (NULL != ptMixerCfg)
	{
		MixerCfgIn2Out(ptMixerCfg, atMixerInfo, byMixNum);
		for ( nIndex = 0; nIndex < byMixNum; nIndex++ )
		{
			// 设为合法EQP, zgc, 2007-10-29
			g_cMcuVcApp.SetPeriEqpIsValid( atMixerInfo[nIndex].GetEqpId() );
			for( dwBrdLoop = 0; dwBrdLoop < byBrdNum; dwBrdLoop++)
			{
				if( atBrdInfo[dwBrdLoop].GetBrdId() == atMixerInfo[nIndex].GetRunBrdId() )
				{
					atMixerInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atMixerInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= byBrdNum )
			{
				CfgLog("[BrdCfgProcIntegration] Mixer%d running brd(Id.%d) isn't exist!\n", 
						 atMixerInfo[nIndex].GetEqpId(), atMixerInfo[nIndex].GetRunBrdId());
			}
		}
	}
	else
	{
		byMixNum = 0;
	}		

	//change rec info
	TEqpRecInfo atRecInfo[MAXNUM_PERIEQP];
	if (NULL != ptRecCfg)
	{
		RecCfgIn2Out(ptRecCfg, atRecInfo, byRecNum);
		for ( nIndex = 0; nIndex < byRecNum; nIndex++ )
		{
			// 设为合法EQP, zgc, 2007-10-29
			g_cMcuVcApp.SetPeriEqpIsValid( atRecInfo[nIndex].GetEqpId() );
		}
	}     
	else
	{
		byRecNum = 0;
	}

	//change tvwall info
	TEqpTVWallInfo atTWInfo[MAXNUM_PERIEQP];
	if (NULL != ptTWCfg)
	{
		TWCfgIn2Out(ptTWCfg, atTWInfo, byTWNum);
		for ( nIndex = 0; nIndex < byTWNum; nIndex++ )
		{
			// 设为合法EQP, zgc, 2007-10-29
			g_cMcuVcApp.SetPeriEqpIsValid( atTWInfo[nIndex].GetEqpId() );
			for( dwBrdLoop = 0; dwBrdLoop < byBrdNum; dwBrdLoop++)
			{
				if( atBrdInfo[dwBrdLoop].GetBrdId() == atTWInfo[nIndex].GetRunBrdId() )
				{
					atTWInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atTWInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= byBrdNum )
			{
				CfgLog("[BrdCfgProcIntegration] TVWall%d running brd(Id.%d) isn't exist!\n", 
						 atTWInfo[nIndex].GetEqpId(), atTWInfo[nIndex].GetRunBrdId());
			}
		}
	}
	else
	{
		byTWNum = 0;
	}
	
	//change bas info
	TEqpBasInfo atBasInfo[MAXNUM_PERIEQP];
	if (NULL != ptBasCfg)
	{
		BasCfgIn2Out(ptBasCfg, atBasInfo, byBasNum);
		for ( nIndex = 0; nIndex < byBasNum; nIndex++ )
		{
			// 设为合法EQP, zgc, 2007-10-29
			g_cMcuVcApp.SetPeriEqpIsValid( atBasInfo[nIndex].GetEqpId() );
			for( dwBrdLoop = 0; dwBrdLoop < byBrdNum; dwBrdLoop++)
			{
				if( atBrdInfo[dwBrdLoop].GetBrdId() == atBasInfo[nIndex].GetRunBrdId() )
				{
					atBasInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atBasInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= byBrdNum )
			{
				CfgLog("[BrdCfgProcIntegration] Bas%d running brd(Id.%d) isn't exist!\n", 
						 atBasInfo[nIndex].GetEqpId(), atBasInfo[nIndex].GetRunBrdId());
			}
		}
	}
	else
	{
		byBasNum = 0;
	}

    //zw[08/05/2008]
	//change basHD info
	TEqpBasHDInfo atBasHDInfo[MAXNUM_PERIEQP];
	if (NULL != ptBasHDCfg)
	{
		BasHDCfgIn2Out(ptBasHDCfg, atBasHDInfo, byBasHDNum);
		for ( nIndex = 0; nIndex < byBasHDNum; nIndex++ )
		{
			g_cMcuVcApp.SetPeriEqpIsValid( atBasHDInfo[nIndex].GetEqpId() );
		}
	}
	else
	{
		byBasHDNum = 0;
	}

	//change vmp info
	TEqpVMPInfo atVmpInfo[MAXNUM_PERIEQP];
	if (NULL != ptVmpCfg)
	{
		VmpCfgIn2Out(ptVmpCfg, atVmpInfo, byVmpNum);
		for ( nIndex = 0; nIndex < byVmpNum; nIndex++ )
		{
			// 设为合法EQP, zgc, 2007-10-29
			g_cMcuVcApp.SetPeriEqpIsValid( atVmpInfo[nIndex].GetEqpId() );
			for( dwBrdLoop = 0; dwBrdLoop < byBrdNum; dwBrdLoop++)
			{
				if( atBrdInfo[dwBrdLoop].GetBrdId() == atVmpInfo[nIndex].GetRunBrdId() )
				{
					atVmpInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atVmpInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= byBrdNum )
			{
				CfgLog("[BrdCfgProcIntegration] Vmp%d running brd(Id.%d) isn't exist!\n", 
						 atVmpInfo[nIndex].GetEqpId(), atVmpInfo[nIndex].GetRunBrdId());
			}
		}
	}
	else
	{
		byVmpNum = 0;
	}
	
	//change prs info
	TEqpPrsInfo atPrsInfo[MAXNUM_PERIEQP];
	if (NULL != ptPrsCfg)
	{
		PrsCfgIn2Out(ptPrsCfg, atPrsInfo, byPrsNum); 
		for ( nIndex = 0; nIndex < byPrsNum; nIndex++ )
		{
			// 设为合法EQP, zgc, 2007-10-29
			g_cMcuVcApp.SetPeriEqpIsValid( atPrsInfo[nIndex].GetEqpId() );
			for( dwBrdLoop = 0; dwBrdLoop < byBrdNum; dwBrdLoop++)
			{
				if( atBrdInfo[dwBrdLoop].GetBrdId() == atPrsInfo[nIndex].GetRunBrdId() )
				{
					atPrsInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atPrsInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= byBrdNum )
			{
				CfgLog("[BrdCfgProcIntegration] Prs%d running brd(Id.%d) isn't exist!\n", 
						 atPrsInfo[nIndex].GetEqpId(), atPrsInfo[nIndex].GetRunBrdId());
			}
		}
	}    
	else
	{
		byPrsNum = 0;
	}

	//change mtw info
	TEqpMpwInfo atMtwInfo[MAXNUM_PERIEQP];
	if (NULL != ptMtwCfg)
	{              
		MtwCfgIn2Out(ptMtwCfg, atMtwInfo, byMtwNum); 
		for ( nIndex = 0; nIndex < byMtwNum; nIndex++ )
		{
			// 设为合法EQP, zgc, 2007-10-29
			g_cMcuVcApp.SetPeriEqpIsValid( atMtwInfo[nIndex].GetEqpId() );
			for( dwBrdLoop = 0; dwBrdLoop < byBrdNum; dwBrdLoop++)
			{
				if( atBrdInfo[dwBrdLoop].GetBrdId() == atMtwInfo[nIndex].GetRunBrdId() )
				{
					atMtwInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atMtwInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if( dwBrdLoop >= byBrdNum )
			{
				CfgLog("[BrdCfgProcIntegration] Mtw%d running brd(Id.%d) isn't exist!\n", 
						 atMtwInfo[nIndex].GetEqpId(), atMtwInfo[nIndex].GetRunBrdId());
			}
		}
	}
	else
	{
		byMtwNum = 0;
	}

	//4.6新加外设 jlb
    //change hdu info  
    TEqpHduInfo atHduInfo[MAXNUM_PERIHDU];
	if (NULL != ptHduCfg)
	{
		HduCfgIn2Out(ptHduCfg, atHduInfo, byHduNum);
		for (nIndex=0; nIndex<byHduNum; nIndex++)
		{
			g_cMcuVcApp.SetPeriEqpIsValid( atHduInfo[nIndex].GetEqpId() );
			for (dwBrdLoop=0; dwBrdLoop<byBrdNum; dwBrdLoop++)
			{
				if ( atBrdInfo[dwBrdLoop].GetBrdId() == atHduInfo[nIndex].GetRunBrdId() )
				{
					atHduInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
				    atBrdInfo[dwBrdLoop].SetPeriId( atHduInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if ( dwBrdLoop >= byBrdNum )
			{
				CfgLog("[BrdCfgProcIntegration] Hdu%d running brd(Id.%d) isn't exist!\n",
					    atHduInfo[nIndex].GetEqpId(), atHduInfo[nIndex].GetRunBrdId() );
			}
		}
	}
	else
	{
        byHduNum = 0;
	}

	//change svmp info  
    TEqpSvmpInfo atSvmpInfo[MAXNUM_PERIEQP];
	if (NULL != ptSvmpCfg)
	{
		SvmpCfgIn2Out(ptSvmpCfg, atSvmpInfo, bySvmpNum);
		for (nIndex=0; nIndex<bySvmpNum; nIndex++)
		{
			g_cMcuVcApp.SetPeriEqpIsValid( atSvmpInfo[nIndex].GetEqpId() );
			for (dwBrdLoop=0; dwBrdLoop<byBrdNum; dwBrdLoop++)
			{
				if ( atBrdInfo[dwBrdLoop].GetBrdId() == atSvmpInfo[nIndex].GetRunBrdId() )
				{
					atSvmpInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atSvmpInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if ( dwBrdLoop >= byBrdNum )
			{
				CfgLog("[BrdCfgProcIntegration] Svmp%d running brd(Id.%d) isn't exist!\n",
					atSvmpInfo[nIndex].GetEqpId(), atSvmpInfo[nIndex].GetRunBrdId() );
			}
		}
	}
	else
	{
        bySvmpNum = 0;
	}

	//change dvmp info  
    TEqpDvmpBasicInfo atDvmpInfo1[MAXNUM_PERIDVMP];
    TEqpDvmpBasicInfo atDvmpInfo2[MAXNUM_PERIDVMP];
	if (NULL != ptDvmpCfg)
	{
		DvmpCfgIn2Out(&(ptDvmpCfg->tEqpDvmpCfgBasicInfo[0]), atDvmpInfo1, byDvmpNum);
		DvmpCfgIn2Out(&(ptDvmpCfg->tEqpDvmpCfgBasicInfo[1]), atDvmpInfo2, byHduNum);
		for (nIndex=0; nIndex<byDvmpNum; nIndex++)
		{
			g_cMcuVcApp.SetPeriEqpIsValid( atDvmpInfo1[nIndex].GetEqpId() );
			for (dwBrdLoop=0; dwBrdLoop<byBrdNum; dwBrdLoop++)
			{
				if ( atBrdInfo[dwBrdLoop].GetBrdId() == atDvmpInfo1[nIndex].GetRunBrdId() )
				{
					atDvmpInfo1[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atDvmpInfo1[nIndex].GetEqpId() );
					break;
				}
			}
			if ( dwBrdLoop >= byBrdNum )
			{
				CfgLog("[BrdCfgProcIntegration] DvmpBasicInfo1%d running brd(Id.%d) isn't exist!\n",
					atDvmpInfo1[nIndex].GetEqpId(), atDvmpInfo1[nIndex].GetRunBrdId() );
			}

			g_cMcuVcApp.SetPeriEqpIsValid( atDvmpInfo2[nIndex].GetEqpId() );
			for (dwBrdLoop=0; dwBrdLoop<byBrdNum; dwBrdLoop++)
			{
				if ( atBrdInfo[dwBrdLoop].GetBrdId() == atDvmpInfo2[nIndex].GetRunBrdId() )
				{
					atDvmpInfo2[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atDvmpInfo2[nIndex].GetEqpId() );
					break;
				}
			}
			if ( dwBrdLoop >= byBrdNum )
			{
				CfgLog("[BrdCfgProcIntegration] DvmpBasicInfo2%d running brd(Id.%d) isn't exist!\n",
					atDvmpInfo2[nIndex].GetEqpId(), atDvmpInfo2[nIndex].GetRunBrdId() );
			}
		}
	}
	else
	{
        byDvmpNum = 0;
	}

	//change MpuBas info  
    TEqpMpuBasInfo atMpuBasInfo[MAXNUM_PERIEQP];
	if (NULL != ptMpuBasCfg)
	{
		MpuBasCfgIn2Out(ptMpuBasCfg, atMpuBasInfo, byMpuBasNum);
		for (nIndex=0; nIndex<byMpuBasNum; nIndex++)
		{
			g_cMcuVcApp.SetPeriEqpIsValid( atMpuBasInfo[nIndex].GetEqpId() );
			for (dwBrdLoop=0; dwBrdLoop<byBrdNum; dwBrdLoop++)
			{
				if ( atBrdInfo[dwBrdLoop].GetBrdId() == atMpuBasInfo[nIndex].GetRunBrdId() )
				{
					atMpuBasInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atMpuBasInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if ( dwBrdLoop >= byBrdNum )
			{
				CfgLog("[BrdCfgProcIntegration] MpuBas%d running brd(Id.%d) isn't exist!\n",
					atMpuBasInfo[nIndex].GetEqpId(), atMpuBasInfo[nIndex].GetRunBrdId() );
			}
		}
	}
	else
	{
        byMpuBasNum = 0;
	}

	//change Ebap info  
    TEqpEbapInfo atEbapInfo[MAXNUM_PERIEQP];
	if (NULL != ptEbapCfg)
	{
		EbapCfgIn2Out(ptEbapCfg, atEbapInfo, byEbapNum);
		for (nIndex=0; nIndex<byEbapNum; nIndex++)
		{
			g_cMcuVcApp.SetPeriEqpIsValid( atEbapInfo[nIndex].GetEqpId() );
			for (dwBrdLoop=0; dwBrdLoop<byBrdNum; dwBrdLoop++)
			{
				if ( atBrdInfo[dwBrdLoop].GetBrdId() == atEbapInfo[nIndex].GetRunBrdId() )
				{
					atEbapInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atEbapInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if ( dwBrdLoop >= byBrdNum )
			{
				CfgLog("[BrdCfgProcIntegration] Ebap%d running brd(Id.%d) isn't exist!\n",
					atEbapInfo[nIndex].GetEqpId(), atEbapInfo[nIndex].GetRunBrdId() );
			}
		}
	}
	else
	{
        byEbapNum = 0;
	}

	//change evpu info  
    TEqpEvpuInfo atEvpuInfo[MAXNUM_PERIEQP];
	if (NULL != ptEvpuCfg)
	{
		EvpuCfgIn2Out(ptEvpuCfg, atEvpuInfo, byEvpuNum);
		for (nIndex=0; nIndex<byEvpuNum; nIndex++)
		{
			g_cMcuVcApp.SetPeriEqpIsValid( atEvpuInfo[nIndex].GetEqpId() );
			for (dwBrdLoop=0; dwBrdLoop<byBrdNum; dwBrdLoop++)
			{
				if ( atBrdInfo[dwBrdLoop].GetBrdId() == atEvpuInfo[nIndex].GetRunBrdId() )
				{
					atEvpuInfo[nIndex].SetIpAddr( atBrdInfo[dwBrdLoop].GetBrdIp() );
					atBrdInfo[dwBrdLoop].SetPeriId( atEvpuInfo[nIndex].GetEqpId() );
					break;
				}
			}
			if ( dwBrdLoop >= byBrdNum )
			{
				CfgLog("[BrdCfgProcIntegration] Evpu%d running brd(Id.%d) isn't exist!\n",
					atEvpuInfo[nIndex].GetEqpId(), atEvpuInfo[nIndex].GetRunBrdId() );
			}
		}
	}
	else
	{
        byEvpuNum = 0;
	}

	// 记录当前的单板配置, zgc
	TBoardInfo atCurBrdInfo[MAXNUM_BOARD];
	memset(atCurBrdInfo, 0, sizeof(atCurBrdInfo));
	u8 byCruBrdNum = 0;
	g_cMcuAgent.ReadBrdTable( &byCruBrdNum, atCurBrdInfo );

	// 修改配置信息
	wRet = g_cMcuAgent.WriteBrdTable(byBrdNum, atBrdInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg,cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_BRD)
		CfgLog("[ProcBrdCfg] write brd info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	wRet = g_cMcuAgent.WriteMixerTable(byMixNum, atMixerInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_MIXER)
		CfgLog("[ProcBrdCfg] write mixer info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	wRet = g_cMcuAgent.WriteRecTable(byRecNum, atRecInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_REC)
		CfgLog("[ProcBrdCfg] write rec info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	wRet = g_cMcuAgent.WriteTvTable(byTWNum, atTWInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_TVWALL)
		CfgLog("[ProcBrdCfg] write tvwall info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	wRet = g_cMcuAgent.WriteBasTable(byBasNum, atBasInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_BAS)
		CfgLog("[ProcBrdCfg] write bas info failed, ret:%d!\n", wRet);
		return FALSE;
	}

    //zw[08/05/2008]
    wRet = g_cMcuAgent.WriteBasHDTable(byBasHDNum, atBasHDInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_BASHD)
		CfgLog("[ProcBrdCfg] write basHD info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	wRet = g_cMcuAgent.WriteVmpTable(byVmpNum, atVmpInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_VMP)

		CfgLog("[ProcBrdCfg] write vmp info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	wRet = g_cMcuAgent.WritePrsTable(byPrsNum, atPrsInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_PRS)
		CfgLog("[ProcBrdCfg] write prs info failed, ret:%d!\n", wRet);
		return FALSE;
	}
	
	wRet = g_cMcuAgent.WriteMpwTable(byMtwNum, atMtwInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_MTVWALL)

		CfgLog("[ProcBrdCfg] write mtw info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	//4.6版本 新加外设 jlb
    TEqpHduInfo  atOldEqpHduInfo[MAXNUM_PERIHDU]; 
	memset(atOldEqpHduInfo, 0x0, sizeof(atOldEqpHduInfo));
    u8 byOldHduNum = 0;
	wRet = g_cMcuAgent.ReadHduTable(&byOldHduNum, atOldEqpHduInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_READ_HDU)
			CfgLog("[ProcBrdCfg] write hdu info failed, ret:%d!\n", wRet);
		return FALSE;
	}
    
	// 修改输入输出制式
	u8 byHduId = 0;
	THduChnlModePort atChnModePort[MAXNUM_HDU_CHANNEL];
	memset(atChnModePort, 0x0, sizeof(atChnModePort));
	for(u32 nLoop1=0; nLoop1<byHduNum; nLoop1++)
	{
        for(u32 nLoop2=0; nLoop2<byOldHduNum; nLoop2++)
		{
			byHduId = atHduInfo[nLoop1].GetEqpId();
		    if ( ((byHduId>=HDUID_MIN)&&(byHduId<=HDUID_MAX))&& 
				 (byHduId == atOldEqpHduInfo[nLoop2].GetEqpId()))
			{	
				THduChnlModePortAgt tHduModePortAgt1, tHduModePortAgt2;
				atHduInfo[nLoop1].GetHduChnlModePort(0, tHduModePortAgt1);
				atHduInfo[nLoop1].GetHduChnlModePort(1, tHduModePortAgt2);
				atChnModePort[0].SetZoomRate( tHduModePortAgt1.GetZoomRate() );
				atChnModePort[0].SetOutModeType(tHduModePortAgt1.GetOutModeType());
				atChnModePort[0].SetOutPortType(tHduModePortAgt1.GetOutPortType());
                atChnModePort[1].SetZoomRate( tHduModePortAgt2.GetZoomRate() );
				atChnModePort[1].SetOutModeType(tHduModePortAgt2.GetOutModeType());
				atChnModePort[1].SetOutPortType(tHduModePortAgt2.GetOutPortType());
                OspPost(MAKEIID(AID_MCU_PERIEQPSSN, atHduInfo[nLoop1].GetEqpId()), MCU_HDU_CHANGEMODEPORT_NOTIF, (void*)atChnModePort, sizeof(atChnModePort));
			}
			else
			{
                CfgLog("The Hdu Id is wrong or first register!\n");
			}
		}
	}
	wRet = g_cMcuAgent.WriteHduTable(byHduNum,atHduInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_HDU)
		CfgLog("[ProcBrdCfg] write hdu info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	wRet = g_cMcuAgent.WriteSvmpTable(bySvmpNum,atSvmpInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_SVMP)
		
		CfgLog("[ProcBrdCfg] write svmp info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	//双VMP写表
    TEqpDvmpBasicInfo atDvmpInfo[MAXNUM_PERIEQP];
    memcpy(atDvmpInfo, atDvmpInfo1, byDvmpNum);
	memcpy((atDvmpInfo + byDvmpNum), atDvmpInfo2, byDvmpNum);
	wRet = g_cMcuAgent.WriteDvmpTable(byDvmpNum*2,atDvmpInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_DVMP)
			
			CfgLog("[ProcBrdCfg] write Dvmp info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	wRet = g_cMcuAgent.WriteMpuBasTable(byMpuBasNum,atMpuBasInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_MPUBAS)
			
			CfgLog("[ProcBrdCfg] write mpubas info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	wRet = g_cMcuAgent.WriteEbapTable(byEbapNum,atEbapInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_EBAP)
			
			CfgLog("[ProcBrdCfg] write ebap info failed, ret:%d!\n", wRet);
		return FALSE;
	}

	wRet = g_cMcuAgent.WriteEvpuTable(byEvpuNum,atEvpuInfo);
	if (SUCCESS_AGENT != wRet)
	{
		MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_WRITE_EVPU)
			
			CfgLog("[ProcBrdCfg] write evpu info failed, ret:%d!\n", wRet);
		return FALSE;
	}

#ifndef _MINIMCU_
	byBrdNum = 0;
	memset(atBrdInfo, 0, sizeof(atBrdInfo));
	g_cMcuAgent.ReadBrdTable(&byBrdNum, atBrdInfo);
	u8 byLayer = 0;
	u8 bySlot = 0;
	u8 byType = 0;
	CServMsg cBrdCfgMsg;
	for( dwBrdLoop = 0; dwBrdLoop < byBrdNum; dwBrdLoop++ )
	{
		for ( nIndex = 0; nIndex < byCruBrdNum; nIndex++ )
		{
			if ( atCurBrdInfo[nIndex].GetType() == atBrdInfo[dwBrdLoop].GetType() 
				&& atCurBrdInfo[nIndex].GetLayer() == atBrdInfo[dwBrdLoop].GetLayer()
				&& atCurBrdInfo[nIndex].GetSlot() == atBrdInfo[dwBrdLoop].GetSlot()
				&& atCurBrdInfo[nIndex].GetBrdIp() == atBrdInfo[dwBrdLoop].GetBrdIp()
				&& atBrdInfo[dwBrdLoop].GetType() != DSL8000_BRD_MPC )
			{
//				memset(&cCfgBuf, 0, sizeof(cCfgBuf));
//				g_cMcuAgent.PackEqpConfig( cCfgBuf, atBrdInfo[dwBrdLoop] );
//
//				if ( g_cMcuAgent.IsRegedBoard(atBrdInfo[dwBrdLoop].GetType(), atBrdInfo[dwBrdLoop].GetLayer(), atBrdInfo[dwBrdLoop].GetSlot()) )
//				{
//					byLayer = atBrdInfo[dwBrdLoop].GetLayer();
//					bySlot = atBrdInfo[dwBrdLoop].GetSlot();
//					cBrdCfgMsg.SetMsgBody((u8*)&byLayer, sizeof(byLayer));
//					cBrdCfgMsg.CatMsgBody((u8*)&bySlot,sizeof(bySlot));
//
//					u16 wLen = 0;
//					u8* pbyBuf = cCfgBuf.GetBuffer(wLen);
//					cBrdCfgMsg.CatMsgBody(pbyBuf, wLen);
//					post(MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_BOARDCFGMODIFY_NOTIF, cBrdCfgMsg.GetServMsg(), cBrdCfgMsg.GetServMsgLen());
//				}
				byLayer = atBrdInfo[dwBrdLoop].GetLayer();
				bySlot = atBrdInfo[dwBrdLoop].GetSlot();
				byType = atBrdInfo[dwBrdLoop].GetType();
				cBrdCfgMsg.SetMsgBody((u8*)&byLayer, sizeof(byLayer));
				cBrdCfgMsg.CatMsgBody((u8*)&bySlot,sizeof(bySlot));
				cBrdCfgMsg.CatMsgBody((u8*)&byType,sizeof(byType));
				post(MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_BOARDCFGMODIFY_NOTIF, cBrdCfgMsg.GetServMsg(), cBrdCfgMsg.GetServMsgLen());
				break;
			} //if ( atCurBrdInfo[nIndex].GetType ...
		} //for ( nIndex = 0; nIndex < byCruB ...
	} //for( dwBrdLoop = 0; dwBrdL...
#endif

// 变更消息体，TDSCModuleInfo结构从MCS_MCU_GETMCUGENERALCFG_REQ消息中转移到MCS_MCU_GETBRDCFG_REQ, zgc, 2007-03-15
#ifdef _MINIMCU_
	if ( bIsExistDsc )
	{
		if ( !CheckNewDscInfo( tAgentDscInfo ) )
		{
			MCUCFG_INVALIDPARAM_NACK(cServMsg, cServMsg.GetEventId()+2, ERR_MCU_CFG_INVALID_BRD)
			CfgLog( "[ProcBrdCfg] New dscinfo is error! Nack!\n" );
			return FALSE;
		}
	}
	else
	{
//		if ( bIsRegDsc )
//		{
		u8 byCurDscType = 0;
		for ( nIndex = 0; nIndex < byCruBrdNum; nIndex++ )
		{
			if( BRD_TYPE_DSC == atCurBrdInfo[nIndex].GetType() ||
				BRD_TYPE_MDSC == atCurBrdInfo[nIndex].GetType() ||
				BRD_TYPE_HDSC == atCurBrdInfo[nIndex].GetType() )
			{
				byCurDscType = atCurBrdInfo[nIndex].GetType();
				break;
			}
		}
		if ( 0 != byCurDscType )
		{
			CServMsg cDisMsg;
			u8 byBrdLayer = 0;
			u8 byBrdSlot = 0;
			cDisMsg.SetMsgBody((u8*)&byBrdLayer, sizeof(byBrdLayer));
			cDisMsg.CatMsgBody((u8*)&byBrdSlot, sizeof(byBrdSlot));
			cDisMsg.CatMsgBody((u8*)&byCurDscType, sizeof(byCurDscType));
			post( MAKEIID(AID_MCU_AGENT,1), SVC_AGT_DISCONNECTBRD_CMD, cDisMsg.GetServMsg(), cDisMsg.GetServMsgLen()  );
		}
//		}
	}

	CServMsg cDscInfoMsg;
	u8 byBrdLayer = 0;
	u8 byBrdSlot = 0;
	cDscInfoMsg.SetMsgBody((u8*)&byBrdLayer, sizeof(byBrdLayer));
	cDscInfoMsg.CatMsgBody((u8*)&byBrdSlot, sizeof(byBrdSlot));
	cDscInfoMsg.CatMsgBody((u8*)&tAgentDscInfo, sizeof(tAgentDscInfo));
	post( MAKEIID(AID_MCU_AGENT,1), SVC_AGT_SETDSCINFO_REQ, cDscInfoMsg.GetServMsg(), cDscInfoMsg.GetServMsgLen()  );

	SetTimer(MCUCFG_DSCCFG_WAITINGCHANGE_OVER_TIMER, 10000 );
	g_cMcuAgent.SetDscInfo( &tAgentDscInfo, FALSE );  //用于反馈给MCS，不修改文件
#endif

	return TRUE;
}

/*=============================================================================
函 数 名： CheckNewDscInfo
功    能： 
算法实现： 
全局变量： 
参    数： TDSCModuleInfo tDscInfo
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/9/18   4.0			周广程                  创建
=============================================================================*/
#ifdef _MINIMCU_
BOOL32 CMcuCfgInst::CheckNewDscInfo(TDSCModuleInfo tDscInfo)
{
	TDSCModuleInfo tLocalDscInfo;
	g_cMcuAgent.GetDscInfo( &tLocalDscInfo );
	u32 dwDscInnerIp = tLocalDscInfo.GetDscInnerIp();
	u32 dwMcuInnerIp = tLocalDscInfo.GetMcuInnerIp();
	u32 dwInnerIpMask = tLocalDscInfo.GetInnerIpMask();
	u32 dwNewCfgIp1 = 0;
	u32 dwNewCfgIp2 = 0;
	u32 dwNewCfgIpMask1 = 0;
	u32 dwNewCfgIpMask2 = 0;
	u32 dwNewCfgGWIp1 = 0;
	u8 byIpMaskBitNum = 0;
	u8 byCallAddrNum = tDscInfo.GetCallAddrNum();
	u8 byMcsAccessAddrNum = tDscInfo.GetMcsAccessAddrNum();
	u8 byLop = 0;
	u8 byLop1 = 0;
	u8 byLop2 = 0;

	TMINIMCUNetParamAll tNetParamAll;
	TMINIMCUNetParam tNetParam;
			
	if ( tDscInfo.GetNetType() == NETTYPE_INVALID )
	{
		CfgLog("[CheckNewDscInfo] NetType INVALID!\n");
		return FALSE;
	}
	
	tDscInfo.GetCallAddrAll( tNetParamAll );
	if ( !tNetParamAll.IsValid() )
	{
		CfgLog("[CheckNewDscInfo] DSC module ip num error! McsAccessAddrNum.%d, CallAddrNum.%d\n", byMcsAccessAddrNum, byCallAddrNum);
		return FALSE; 
	}

	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	tDscInfo.GetMcsAccessAddrAll( tNetParamAll );
	if ( !tNetParamAll.IsValid() )
	{
		CfgLog("[CheckNewDscInfo] DSC module ip num error! McsAccessAddrNum.%d, CallAddrNum.%d\n", byMcsAccessAddrNum, byCallAddrNum);
		return FALSE;
	}
	// IP数不能为0
	if ( 0 == byMcsAccessAddrNum || MAXNUM_MCSACCESSADDR < byMcsAccessAddrNum 
		|| 0 == byCallAddrNum || ETH_IP_MAXNUM-1 < byCallAddrNum )
	{
		CfgLog("[CheckNewDscInfo] DSC module ip num error! McsAccessAddrNum.%d, CallAddrNum.%d\n", byMcsAccessAddrNum, byCallAddrNum);
		return FALSE;
	}

	tDscInfo.GetMcsAccessAddrAll( tNetParamAll );
	// 检验会控连接地址是否彼此冲突, zgc, 2007-07-17
	// 检验会控连接地址和网关地址是否在同一个网段, zgc, 2007-08-23
	CfgLog( "[CheckNewDscInfo] Check McsAccessAddr!\n" );
	if ( !IsValidNetParamAll(tNetParamAll) )
	{
		CfgLog( "[CheckNewDscInfo] McsAccessAddr config error!\n");
		return FALSE;
	}
	
	// 检验呼叫地址是否彼此冲突(包括内口IP), zgc, 2007-07-17
	memset( &tNetParamAll, 0, sizeof(tNetParamAll) );
	tDscInfo.GetCallAddrAll(tNetParamAll);
	CfgLog( "[CheckNewDscInfo] Check CallAddr!\n" );
	if ( !IsValidNetParamAll(tNetParamAll, dwDscInnerIp, dwInnerIpMask) )
	{
		CfgLog( "[CheckNewDscInfo] CallAddr config error!\n");
		return FALSE;
	}

	return TRUE;
}
#endif
/*=============================================================================
函 数 名： IsValidNetParamAll
功    能： 
算法实现： 
全局变量： 
参    数： TMINIMCUNetParamAll tNetParamAll
           u32 dwIpAddr
           u32 dwIpMask
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/9/18   4.0			周广程                  创建
=============================================================================*/
BOOL32 CMcuCfgInst::IsValidNetParamAll(TMINIMCUNetParamAll tNetParamAll, u32 dwIpAddr, u32 dwIpMask)
{
	u8 byNetParamNum = tNetParamAll.GetNetParamNum();
	if ( 0 == byNetParamNum )
	{
		CfgLog("[IsValidNetParamAll] The netparam num is zero!\n");
		return FALSE;
	}
	
	u32 dwNewCfgIp1 = 0;
	u32 dwNewCfgIp2 = 0;
	u32 dwNewCfgIpMask1 = 0;
	u32 dwNewCfgIpMask2 = 0;
	u32 dwNewCfgGWIp1 = 0;
	u8 byLop1, byLop2;
	TMINIMCUNetParam tNetParam;
	for ( byLop1 = 0; byLop1 < byNetParamNum; byLop1++ )
	{
		if ( !tNetParamAll.GetNetParambyIdx(byLop1, tNetParam) )
		{
			CfgLog("[IsValidNetParamAll] Get NetParam.%d error!\n", byLop1);
			return FALSE;
		}
		
		dwNewCfgIp1 = tNetParam.GetIpAddr();
		dwNewCfgIpMask1 = tNetParam.GetIpMask();
		dwNewCfgGWIp1 = tNetParam.GetGatewayIp();			
		// IPMASK不能大于30位,不能全0,且不能0,1间隔
		if ( !IsIpMaskValid(dwNewCfgIpMask1) )
		{
			CfgLog("[IsValidNetParamAll] Ip0x%x: IpMask is invalid!\n", dwNewCfgIp1);
			return FALSE;
		}
		// 子网IP不能全0或全1
		dwNewCfgIpMask1 = ~dwNewCfgIpMask1;
		if ( (dwNewCfgIp1&dwNewCfgIpMask1) == 0 
			|| (dwNewCfgIp1&dwNewCfgIpMask1) == dwNewCfgIpMask1 )
		{
			CfgLog("[IsValidNetParamAll] subnet ip.0x%x can not be network or broadcast!\n", dwNewCfgIp1);
			return FALSE;
		}
		dwNewCfgIpMask1 = ~dwNewCfgIpMask1;

		//IP和网关应该在同一网段
		if ( (dwNewCfgIp1&dwNewCfgIpMask1) != (dwNewCfgGWIp1&dwNewCfgIpMask1) )
		{
			CfgLog("[ProcBrdCfg]WAN ip.0x%x should be in same subnet with GWIp!\n", dwNewCfgIp1);
			return FALSE;
		}

		for ( byLop2 = byLop1+1; byLop2 < byNetParamNum; byLop2++ )
		{
			if ( !tNetParamAll.GetNetParambyIdx(byLop2, tNetParam) )
			{
				CfgLog("[IsValidNetParamAll] Get NetParam.%d error!\n", byLop2);
				return FALSE;
			}
			// 检验IP是否在同一子网内
			dwNewCfgIp2 = tNetParam.GetIpAddr();
			dwNewCfgIpMask2 = tNetParam.GetIpMask();
			if ( 
				(dwNewCfgIp1&dwNewCfgIpMask1) == (dwNewCfgIp2&dwNewCfgIpMask1) 
				|| (dwNewCfgIp1&dwNewCfgIpMask2) == (dwNewCfgIp2&dwNewCfgIpMask2) 
			   )
			{
				CfgLog("[IsValidNetParamAll] ip.0x%x conflict to ip.0x%x!\n ",
						dwNewCfgIp1, dwNewCfgIp2);
				return FALSE;
			}
		}

		// 对于另外感兴趣的IP地址的冲突检验
		if ( dwIpAddr != 0)
		{
			if ( 
				(dwNewCfgIp1&dwNewCfgIpMask1) == (dwIpAddr&dwNewCfgIpMask1) 
				|| (dwNewCfgIp1&dwIpMask) == (dwIpAddr&dwIpMask) 
				)
			{
				CfgLog("[IsValidNetParamAll] ip.0x%x conflict to outer ip.0x%x!\n ",
						dwNewCfgIp1, dwIpAddr);
				return FALSE;
			}
		}	
	}	//for ( byLop1 = 0; byLop1 < byNetParamNum ...

	return TRUE;
}

/*=============================================================================
  函 数 名： ProcVCSGeneralCfg
  功    能： VCS配置信息设置
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void   
=============================================================================*/
void  CMcuCfgInst::ProcVCSGeneralCfg(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u8 *pbyMsg  = cServMsg.GetMsgBody();
	u16 wMsgLen = cServMsg.GetMsgBodyLen();

	s8 achSoftName[MAX_VCSSOFTNAME_LEN];
	memset(achSoftName, 0, sizeof(achSoftName));
	if (VCS_MCU_GETSOFTNAME_REQ == pcMsg->event)
	{
		g_cMcuAgent.GetVCSSoftName(achSoftName);
		cServMsg.SetMsgBody((u8*)achSoftName, sizeof(achSoftName));
		SendMsg2Mcsssn(cServMsg, pcMsg->event + 1);
	}
	else if (VCS_MCU_CHGSOFTNAME_REQ == pcMsg->event)
	{
		memcpy(achSoftName, pbyMsg, min(wMsgLen, MAX_VCSSOFTNAME_LEN));

		u16 wRet = g_cMcuAgent.SetVCSSoftName(achSoftName);
		if (SUCCESS_AGENT != wRet)            
		{
			cServMsg.SetMsgBody(NULL, 0);                           
			cServMsg.SetErrorCode(ERR_MCU_VCS_WRITESOTTNAME);                            
			SendMsg2Mcsssn(cServMsg,  pcMsg->event + 2); 
			CfgLog("[ProcVCSGeneralCfg]write softname failed. ret:%d!\n", wRet);
			return;
		}
		else
		{
			SendMsg2Mcsssn(cServMsg, pcMsg->event + 1);
		}

		CMcsSsn::BroadcastToAllMcsSsn(pcMsg->event + 3, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

	}
}
// END OF FILE
