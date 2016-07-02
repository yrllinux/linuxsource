/*****************************************************************************
   模块名      : mcu
   文件名      : dcsssn.cpp
   相关文件    : dcsssn.h
   文件实现功能: MCU的T120集成业务处理
   作者        : 张宝卿
   版本        : V4.0  Copyright(C) 2005-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/11/28  4.0		   张宝卿      创建
******************************************************************************/

#include "evmcudcs.h"
#include "evmcu.h"
#include "evmcueqp.h"
#include "mcuvc.h"
#include "dcsssn.h"

CDcsSsnApp g_cDcsSsnApp;

//////////////////////////////////////////////////////////////////////////
//Construction / Destruction
//////////////////////////////////////////////////////////////////////////
CDcsSsnInst::CDcsSsnInst() : m_dwDcsNode(0),
							 m_dwDcsIId(0),
							 m_dwMcuIId(0),
							 m_dwDcsIp(0),
							 m_wDcsPort(0)
{	
}

CDcsSsnInst::~CDcsSsnInst()
{
    // destructor 不能调用
	// ClearInst();
}

/*=============================================================================
  函 数 名： ClearInst
  功    能： (1) 清空实例状态
			 (2) 删除DCS地址列表中对应项
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/13    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ClearInst()
{
	NEXTSTATE( STATE_T120_IDLE );
	g_cDcsSsnApp.m_adwDcsIp[GetInsID()-1] = 0;
	m_dwDcsNode = 0;
	m_dwDcsIId  = 0;
	m_dwMcuIId  = 0;
	m_dwDcsIp   = 0;
	m_wDcsPort	= 0;
}

/*=============================================================================
  函 数 名： DaemonInstanceEntry
  功    能： 处理多个DCS消息的分发
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
             CApp * pcApp
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/13    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::DaemonInstanceEntry( CMessage *const pcMsg, CApp *pcApp )
{
	//消息检测
	if( NULL == pcMsg )
	{
		DcsLog("Daemon: the pcMsg pointer should not be Null, ignore it !\n");
		return;
	}
	else
	{
		DcsLog("Daemon: message %u<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
	}
	
	switch( pcMsg->event )
	{
	case DCS_MCU_REG_REQ:
		DaemonProcDcsMcuRegReq( pcMsg, pcApp );
		break;
    case MCU_APPTASKTEST_REQ:
        DaemonProcAppTaskRequest(pcMsg);
        break;
	default:
		DcsLog("Daemon: unexcept message %d<%s> received, ignore it !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
    
    return;
}

/*=============================================================================
  函 数 名： InstanceEntry
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/13    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::InstanceEntry( CMessage *const pcMsg )
{
	//消息检测
	if( NULL == pcMsg )
	{
		DcsLog("Inst: the pcMsg pointer should not be Null, ignore it !\n");
		return;
	}
	else
		DcsLog("Inst: message %u<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
	
	switch( pcMsg->event )
	{
	case DCS_MCU_REG_REQ:
		ProcDcsMcuRegisterReq( pcMsg );
		break;

	case MCU_DCSSSN_CREATECONF_REQ:
	case MCU_DCSSSN_ADDMT_REQ:
	case MCU_DCSSSN_DELMT_REQ:
	case MCU_DCSSSN_RELEASECONF_REQ:
		ProcMcuDcsMsg( pcMsg );
		break;

	case DCS_MCU_CREATECONF_ACK:
	case DCS_MCU_CREATECONF_NACK:
	case DCS_MCU_CONFCREATED_NOTIF:
	case DCS_MCU_ADDMT_ACK:
	case DCS_MCU_ADDMT_NACK:
	case DCS_MCU_DELMT_ACK:
	case DCS_MCU_DELMT_NACK:
	case DCS_MCU_MTJOINED_NOTIF:
	case DCS_MCU_MTLEFT_NOTIF:		
	case DCS_MCU_RELEASECONF_ACK:
	case DCS_MCU_RELEASECONF_NACK:
	case DCS_MCU_CONFRELEASED_NOTIF:
		ProcDcsMcuMsg( pcMsg );
		break;
		
	case OSP_DISCONNECT:
		ProcDcsDisconnect( pcMsg );
		break;

    case MCU_MSSTATE_EXCHANGE_NTF:
        ProcMSStateExchageNotify( pcMsg );
        break;
        
    case DCS_MCU_GETMSSTATUS_REQ:
        ProcDcsMcuGetMsStatusReq( pcMsg );
        break;

	default:
		DcsLog("Inst: unexpected msg %u<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*====================================================================
    函数名      ：DaemonProcAppTaskRequest
    功能        ：GUARD模块探测消息处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/29    1.0         
====================================================================*/
void CDcsSsnInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );
}

/*=============================================================================
  函 数 名： DaemonProcDcsMcuRegReq
  功    能： 预处理DCS的注册信息
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
             CApp *pcApp
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/13    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::DaemonProcDcsMcuRegReq( CMessage *const pcMsg, CApp *pcApp )
{
 	CDcsMcuRegReqPdu  cRegPdu;
	CMcuDcsRegNackPdu cRegNackPdu;
	CNetDataCoder	  cRegCoder;
	u8 achRegNackData[sizeof(CMcuDcsRegNackPdu)];

	//解包注册DCS数据
	cRegCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
	cRegPdu.PduDecode( cRegCoder );
	
	//获取当前DCS信息
	m_dwDcsIId  = pcMsg->srcid;
	m_dwDcsNode = pcMsg->srcnode;
	m_dwDcsIp	= cRegPdu.m_cDcsInfor.m_dwDcsIp;

	//未配置, NACK
	if ( FALSE == g_cMcuVcApp.IsPeriDcsConfiged( m_dwDcsIp ) )
	{
		u16 wErrorType = CErrorInfor::e_UnConfig;
		McuDcsRegNack( cRegNackPdu, cRegCoder, achRegNackData, wErrorType, pcMsg->event + 2 );			
		
		DcsLog( "Reg: dcs 0x%x have not been configed, ignore it !\n", m_dwDcsIp );
		return;
	}

	//是否已经注册
	for( s32 nIndex = 0; nIndex < MAXNUM_MCU_DCS; nIndex ++  )
	{
		//已经注册, NACK
		if( m_dwDcsIp == g_cDcsSsnApp.m_adwDcsIp[nIndex] )
		{
			u16 wErrorType = CErrorInfor::e_ConfAlreadyExist;
			McuDcsRegNack( cRegNackPdu, cRegCoder, achRegNackData, wErrorType, pcMsg->event + 2 );			

			DcsLog( "Reg: dcs 0x%x have already registered, ignore it !\n", m_dwDcsIp );
			return;
		}
	}
	
	//是否有空的实例
	for( s32 nInst = 1; nInst < MAXNUM_MCU_DCS + 1; nInst ++ )
	{
		//如果有空实例
		if( pcApp->GetInstance(nInst)->CurState() == STATE_T120_IDLE )
		{
			//转发给该空实例处理
			SendMsgToInst( nInst, pcMsg->event, pcMsg->content, pcMsg->length );
			
			//保存注册端信息到该空实例
			CDcsSsnInst* cDcsSsnInst;
			cDcsSsnInst = (CDcsSsnInst*)pcApp->GetInstance(nInst);
			cDcsSsnInst->m_dwDcsNode = pcMsg->srcnode;
			cDcsSsnInst->m_dwDcsIId  = pcMsg->srcid;
			
			//保存当前DCS地址
			g_cDcsSsnApp.m_adwDcsIp[nInst-1] = m_dwDcsIp;
			return;
		}	
	}

	//没有找到空闲实例, NACK	
	u16 wErrorType = CErrorInfor::e_DcsFull;
	McuDcsRegNack( cRegNackPdu, cRegCoder, achRegNackData, wErrorType, pcMsg->event + 2 );
	
	DcsLog("reg: there is no space for new DCS ! \n");
	return;
}

/*=============================================================================
  函 数 名： ProcDcsMcuGetMsStatusReq
  功    能： DCS取主备状态
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/06/14    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuGetMsStatusReq( CMessage *const pcMsg )
{
    // guzh [4/11/2007] 如果没有通过注册就发送获取请求，很有可能是重新启动前的连接，不理睬
    if ( CurState() == STATE_T120_IDLE )
    {
        OspPrintf(TRUE, FALSE, "[ProcDcsMcuGetMsStatusReq]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        
        return;
    }

    CDcsMcuGetmsstatusReqPdu	cGetMSStatusPdu;
    CNetDataCoder				cMSStatusCoder;
    
    u8 byConfIdx = 0;	//会议索引号
    
    //消息处理
    switch( pcMsg->event )
    {
    case DCS_MCU_GETMSSTATUS_REQ:
        {            
            //解码消息
            cMSStatusCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
            cGetMSStatusPdu.PduDecode( cMSStatusCoder );          

            //直接取主备状态
            if( MCU_MSSTATE_ACTIVE == g_cMSSsnApp.GetCurMSState() )
            {
                //写入数据单元
                CMcuDcsGetmsstatusAckPdu cGetMSStatusPdu;
                CNetDataCoder            cMSStatusCoder;
                u8 achMSStatusData[sizeof(CMcuDcsGetmsstatusAckPdu)];
                
                cGetMSStatusPdu.m_wConfId = byConfIdx;
                
                //写入当前的主备状态
                cGetMSStatusPdu.m_cMcuMsstate.m_bMcuMsstateInfo = g_cMSSsnApp.IsMsSwitchOK();
                
                //编码并发送
                cMSStatusCoder.SetNetDataBuffer( achMSStatusData, sizeof(achMSStatusData) );
                cGetMSStatusPdu.PduEncode( cMSStatusCoder );
                SendMsgToDcs( MCU_DCS_GETMSSTATUS_ACK, achMSStatusData, (u16)cMSStatusCoder.GetCurrentLength() ); 
            }            
        }
        break;
    default:
        DcsLog("unexpected msg %d<%s> received in get ms status req !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
        break;
    }

    return;
}

/*=============================================================================
  函 数 名： ProcDcsMcuRegisterReq
  功    能： 校验注册信息,做相应的应答处理
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/13    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuRegisterReq( CMessage *const pcMsg )
{
	CDcsMcuRegReqPdu	cRegPdu;
	CMcuDcsRegAckPdu	cRegAckPdu;
	CMcuDcsRegNackPdu	cRegNackPdu;
	CNetDataCoder		cRegCoder;

	u8 achRegAckData[sizeof(CMcuDcsRegAckPdu)];
	u8 achRegNackData[sizeof(CMcuDcsRegNackPdu)];

	cRegCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
	cRegPdu.PduDecode( cRegCoder );	

	//获取当前DCS的IP和Port
	m_dwDcsIp  = cRegPdu.m_cDcsInfor.m_dwDcsIp;
	m_wDcsPort = cRegPdu.m_cDcsInfor.m_wDcsPort;
	
	//判断消息发起源
	if( MAKEIID(AID_MCU_DCSSSN, CInstance::DAEMON) != pcMsg->srcid )
	{
		DcsLog("Inst: register req should be send by dcsssn Daemon, ignore it !\n");
		u16 wErrorType = CErrorInfor::e_NotDaemon;
		McuDcsRegNack( cRegNackPdu, cRegCoder, achRegNackData, wErrorType, pcMsg->event + 2 );
		return;
	}
	
	//返回ACK
	McuDcsRegAck( cRegAckPdu, cRegCoder, achRegAckData, pcMsg->event + 1 );
	
	//更新状态
	NEXTSTATE( STATE_T120_NORMAL );
	
	//注册断链
	::OspNodeDiscCBRegQ( m_dwDcsNode, GetAppID(), GetInsID() );

	//DCS信息
	TDcsInfo tDcsInfo;
	tDcsInfo.m_byDcsId	= (u8)GetInsID();
	tDcsInfo.m_dwDcsIp	= cRegPdu.m_cDcsInfor.m_dwDcsIp;
	tDcsInfo.m_wDcsPort	= cRegPdu.m_cDcsInfor.m_wDcsPort;

	//发消息给MCU通知DCS注册成功
	CServMsg cServMsg;
	cServMsg.SetEventId( MCU_DCSCONNCETED_NOTIF );
	cServMsg.SetMsgBody( (u8*)&tDcsInfo, sizeof(tDcsInfo) );
	g_cDcsSsnApp.SendMsgToMcuDaemon( cServMsg );

    return;
}

/*=============================================================================
  函 数 名： ProcDcsDisconnect
  功    能： 断链消息处理:　清空本实例, 通知McuVcDaemon.
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/13    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcDcsDisconnect( CMessage *const pcMsg )
{
    DcsLog("disconnect msg %u<%s> received in inst.%d !\n", pcMsg->event, OspEventDesc(pcMsg->length), GetInsID() );

	//状态检测
	switch( CurState() )
	{
	case STATE_T120_NORMAL:
		{
            if( *( u32 * )pcMsg->content == m_dwDcsNode )
            {               
                if (INVALID_NODE != m_dwDcsNode)
                {
                    OspDisconnectTcpNode(m_dwDcsNode);
                }            
                
                //取当前实例注册的DCS的信息
                TDcsInfo tDcsInfo;
                tDcsInfo.m_byDcsId	= (u8)GetInsID();
                tDcsInfo.m_dwDcsIp	= m_dwDcsIp;
                tDcsInfo.m_wDcsPort	= m_wDcsPort;
                
                //发消息通知McuVcDaemon			
                CServMsg cServMsg;
                cServMsg.SetEventId( MCU_DCSDISCONNECTED_NOTIF );
                cServMsg.SetMsgBody( (u8*)&tDcsInfo, sizeof(tDcsInfo) );
                g_cDcsSsnApp.SendMsgToMcuDaemon( cServMsg );
                
                //清空实例状态
                ClearInst();
            }			
		}
		break;
		
	default:
		DcsLog( "unexpected state<%d> received osp disconnected msg !\n", CurState() );
		break;
	}

    return;
}

/*=============================================================================
  函 数 名： ProcMSStateExchageNotify
  功    能： 处理MCU到DCS主备倒换通知消息
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/11/27    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcMSStateExchageNotify( CMessage *const pcMsg )
{
    //状态检测
    switch( CurState() )
    {
    case STATE_T120_NORMAL:
        {
            // 如果同步失败,断开相应的连接
            u8 byIsSwitchOk = *pcMsg->content;
            if (0 == byIsSwitchOk)
            {
                OspDisconnectTcpNode( m_dwDcsNode );
                return;
            }            
        }
        break;
    default:
        DcsLog( "unexpected state<%d> received StateExchageNotify !\n", CurState() );
        break;
    }
    return;
}

/*=============================================================================
  函 数 名： ProcMcuDcsMsg
  功    能： 处理MCU到DCS的消息
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/13    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcMcuDcsMsg( CMessage *const pcMsg )
{
	//消息检测
	if ( NULL == pcMsg )
	{
		DcsLog( "mcu to dcs msg %d<%s> receive failed !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		return;
	}
	//状态检测
	if ( STATE_T120_NORMAL != CurState() )
	{
		DcsLog( "mcu to dcs msg %d<%s> received in wrong state !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		return;
	}
	//消息处理
	switch( pcMsg->event )
	{
	case MCU_DCSSSN_CREATECONF_REQ:
		ProcMcuDcsCreateConfReq( pcMsg );
		break;
	case MCU_DCSSSN_RELEASECONF_REQ:
		ProcMcuDcsReleaseConfReq( pcMsg );
		break;
	case MCU_DCSSSN_DELMT_REQ:
		ProcMcuDcsDelMtReq( pcMsg );
		break;
	case MCU_DCSSSN_ADDMT_REQ:
		ProcMcuDcsAddMtReq( pcMsg );
		break;
	default:
		DcsLog( "unexpected msg %d<%s>received in mcu to dcs msg !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*=============================================================================
  函 数 名： ProcDcsMcuMsg
  功    能： 处理DCS到MCU的消息
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/13    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuMsg( CMessage *const pcMsg )
{
	//消息检测
	if ( NULL == pcMsg )
	{
		DcsLog( "dcs to mcu msg %d<%s> receive failed !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		return;
	}
	//状态检测
	if ( STATE_T120_NORMAL != CurState() )
	{
		DcsLog( "dcs to mcu msg msg %d<%s> received in wrong state !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		return;
	}
	//消息处理
	switch( pcMsg->event )
	{
	case DCS_MCU_CREATECONF_ACK:
	case DCS_MCU_CREATECONF_NACK:
	case DCS_MCU_CONFCREATED_NOTIF:
		ProcDcsMcuCreateConfRsp( pcMsg );
		break;
	case DCS_MCU_RELEASECONF_ACK:
	case DCS_MCU_RELEASECONF_NACK:
	case DCS_MCU_CONFRELEASED_NOTIF:
		ProcDcsMcuReleaseConfRsp( pcMsg );
		break;
	case DCS_MCU_ADDMT_ACK:
	case DCS_MCU_ADDMT_NACK:
		ProcDcsMcuAddMtRsp( pcMsg );
		break;
	case DCS_MCU_DELMT_ACK:
	case DCS_MCU_DELMT_NACK:
		ProcDcsMcuDelMtRsp( pcMsg );
		break;
	case DCS_MCU_MTJOINED_NOTIF:
		ProcDcsMcuMtOnlineNotify( pcMsg );
		break;
	case DCS_MCU_MTLEFT_NOTIF:
		ProcDcsMcuMtOfflineNotify( pcMsg );
		break;
	default:
		DcsLog( "unexpected msg %d<%s> received in inst.%d \n", pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
		break;
	}

    return;
}

/*=============================================================================
  函 数 名： ProcMcuDcsCreateConfReq
  功    能： 处理MCU发给DCSSSN的创会请求
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/27    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcMcuDcsCreateConfReq( CMessage *const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	//消息体校验
	if ( !MSGBODY_LEN_GE( cServMsg, sizeof(TDcsConfInfo) ) ) 
		return;
	
	//取消息体
	TDcsConfInfo tDcsConfInfo = *(TDcsConfInfo*)( cServMsg.GetMsgBody() );
	
	//以下写入DCS数据单元
	CMcuDcsCreateConfReqPdu cCreateConfPdu;
	CNetDataCoder			cCreateConfCoder;
	u8	achCreateConfData[sizeof(CMcuDcsCreateConfReqPdu)];
	
	//会议ID
	if ( 0 == cServMsg.GetConfIdx() )
	{
		DcsLog( "confidx should not be 0 in mcu to dcs create conf !\n ");
		return;
	}
	cCreateConfPdu.m_wConfId = cServMsg.GetConfIdx();

	//会议名称
	if ( NULL == tDcsConfInfo.GetConfName() ) 
	{
		DcsLog( "conf name should not be NULL in mcu to dcs create conf !\n");
		return;
	}
	LPCSTR lpszConfName = NULL;
	lpszConfName = tDcsConfInfo.GetConfName();
	strncpy( cCreateConfPdu.m_cConfBasicInfor.m_achConfName, lpszConfName, sizeof(cCreateConfPdu.m_cConfBasicInfor.m_achConfName) );

	//会议密码

	//取密码
	LPCSTR lpszDataConfPwd = tDcsConfInfo.GetConfPwd();

	//加密空
	if ( NULL == lpszDataConfPwd ) 
	{
		cCreateConfPdu.m_cConfBasicInfor.m_bHasPassword = FALSE;
	}
	//传密码
	else
	{
		cCreateConfPdu.m_cConfBasicInfor.m_bHasPassword = TRUE;
		strncpy( cCreateConfPdu.m_cConfBasicInfor.m_achConfPassword, lpszDataConfPwd, sizeof(u8) * MAXLEN_PWD );
	}
	
	//会议码率
	u16 wBandwidth = tDcsConfInfo.GetBandwidth();
	if ( 0 != wBandwidth )
	{
		cCreateConfPdu.m_cConfShortInfor.IncludeOption( CConfShortInfor::e_BandWidth );
		cCreateConfPdu.m_cConfShortInfor.m_dwBandWidth = wBandwidth;
	}
	else
	{
		DcsLog( "data conf bandwidth should not be 0 ! \n" );
		return;
	}

	//是否接受终端主动呼入,  默认为FALSE
	if ( TRUE == tDcsConfInfo.IsSupportJoinMt() )
	{
		cCreateConfPdu.m_cConfShortInfor.IncludeOption( CConfShortInfor::e_SupportJoin );
		cCreateConfPdu.m_cConfShortInfor.m_bSupportJoinedMt = TRUE;
	}

	//编码并发送该数据单元
	cCreateConfCoder.SetNetDataBuffer( achCreateConfData, sizeof(achCreateConfData) );
	cCreateConfPdu.PduEncode( cCreateConfCoder );
	SendMsgToDcs( MCU_DCS_CREATECONF_REQ, achCreateConfData, (u16)cCreateConfCoder.GetCurrentLength() );

    return;
}

/*=============================================================================
  函 数 名： ProcDcsMcuCreateConfRsp
  功    能： 处理DCS发给MCU的创会的应答
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/28    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuCreateConfRsp( CMessage *const pcMsg )
{
	CDcsMcuCreateConfAckPdu		cConfAckPdu;
	CDcsMcuCreateConfNackPdu	cConfNackPdu;
	CDcsMcuCreateConfNotifyPdu  cConfNotifyPdu;
	CNetDataCoder				cConfRspCoder;

	u8 byConfIdx = 0;	//会议索引号
	
	//消息处理
	switch( pcMsg->event )
	{
	case DCS_MCU_CREATECONF_ACK:
		{
			//解码消息
			cConfRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cConfAckPdu.PduDecode( cConfRspCoder );
			
			//校验confidx			
			byConfIdx = (u8)cConfAckPdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				DcsLog("wrong confidx received in create conf ack !\n");
				return;
			}
			g_cDcsSsnApp.SendMsgToMcuConf( byConfIdx, DCSSSN_MCU_CREATECONF_ACK );
		}
		break;

	case DCS_MCU_CREATECONF_NACK:
		{
			//解码消息
			cConfRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cConfNackPdu.PduDecode( cConfRspCoder );

			//校验confidx
			byConfIdx = (u8)cConfNackPdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				DcsLog("wrong confidx received in create conf nack !\n");
				return;
			}
			//错误类型
			u8 byErrorId = cConfNackPdu.m_cErrorInfor.m_emErrorType;
			if ( CErrorInfor::e_Succuss == byErrorId )
			{
				DcsLog("wrong error id received in create conf nack !\n" );
				return;
			}
			g_cDcsSsnApp.SendMsgToMcuConf( byConfIdx, DCSSSN_MCU_CREATECONF_NACK, &byErrorId, sizeof(u8) );
		}
		break;

	case DCS_MCU_CONFCREATED_NOTIF:
		{
			//解码消息
			cConfRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cConfNotifyPdu.PduDecode( cConfRspCoder );

			//校验confidx
			byConfIdx = (u8)cConfNotifyPdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				DcsLog("wrong confidx received in create conf notify !\n");
				return;
			}
			
			//填充数据会议信息
			TDcsConfInfo tDcsConfInfo;
			memset( &tDcsConfInfo, 0, sizeof(tDcsConfInfo) );

			//会议名
			s8 *pszConfName = cConfNotifyPdu.m_cConfBasicInfor.m_achConfName;
			if ( NULL == pszConfName )
			{
				DcsLog("conf name shoule not be null in create conf notify !\n");
				return;
			}
			tDcsConfInfo.SetConfName( pszConfName );

			//会议密码
			if ( TRUE == cConfNotifyPdu.m_cConfBasicInfor.m_bHasPassword ) 
			{
				LPCSTR lpszConfPwd = cConfNotifyPdu.m_cConfBasicInfor.m_achConfPassword;
				if ( NULL != lpszConfPwd )
				{
					tDcsConfInfo.SetConfPwd( lpszConfPwd );
				}
				else
				{
					DcsLog("get conf encrypt key in create conf notify failed !\n");
					return;
				}
			}	
			
			//会议码率
			if ( 0 != cConfNotifyPdu.m_cConfShortInfor.m_dwBandWidth )
			{
				u16 wBandwidth = (u16)cConfNotifyPdu.m_cConfShortInfor.m_dwBandWidth;
				tDcsConfInfo.SetBandwidth( wBandwidth );
			}
			else
			{
				DcsLog("get bandwidth in create conf notify failed !\n");
				return;
			}
			
			//是否接受终端呼入
			BOOL32 bSupport = cConfNotifyPdu.m_cConfShortInfor.m_bSupportJoinedMt;
			tDcsConfInfo.SetSupportJoinMt( bSupport );
			
			//发消息给McuVc
			CServMsg cServMsg;
			cServMsg.SetConfIdx( byConfIdx );
			cServMsg.SetEventId( DCSSSN_MCU_CONFCREATED_NOTIF );
			cServMsg.SetMsgBody( (u8*)&tDcsConfInfo, sizeof(tDcsConfInfo) );
			
			g_cDcsSsnApp.SendMsgToMcuConf( cServMsg );

		}
		break;

	default:
		DcsLog("unexpected msg %d<%s> received in create conf rsp !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*=============================================================================
  函 数 名： ProcMcuDcsReleaseConfReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/28    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcMcuDcsReleaseConfReq( CMessage *const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	CMcuDcsReleaseConfReqPdu cReleaseConfReqPdu;
	CNetDataCoder			 cReleaseConfCoder;
	u8	achReleaseConfData[sizeof(CMcuDcsReleaseConfReqPdu)];
	
	//写入数据单元
	cReleaseConfReqPdu.m_wConfId = cServMsg.GetConfIdx();
	
	//编码并发送
	cReleaseConfCoder.SetNetDataBuffer( achReleaseConfData, sizeof(achReleaseConfData) );
	cReleaseConfReqPdu.PduEncode( cReleaseConfCoder );
	SendMsgToDcs( MCU_DCS_RELEASECONF_REQ, achReleaseConfData, (u16)cReleaseConfCoder.GetCurrentLength() );

    return;
}

/*=============================================================================
  函 数 名： ProcDcsMcuReleaseConfRsp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/28    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuReleaseConfRsp( CMessage *const pcMsg )
{
	CDcsMcuReleaseConfAckPdu	cReleaseAckPdu;
	CDcsMcuReleaseConfNackPdu	cReleaseNackPdu;
	CDcsMcuReleaseConfNackPdu	cReleaseNotifyPdu;
	CNetDataCoder				cReleaseRspCoder;

	u8 byConfIdx = 0;	//会议索引号
	
	//消息处理
	switch( pcMsg->event )
	{
	case DCS_MCU_RELEASECONF_ACK:
		{
			cReleaseRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cReleaseAckPdu.PduDecode( cReleaseRspCoder );

			byConfIdx = (u8)cReleaseAckPdu.m_wConfId;
			
			//校验confidx
			if ( 0 == byConfIdx )
			{
				DcsLog("wrong confidx received in release conf ack !\n");
				return;
			}
			CServMsg cServMsg;
			cServMsg.SetEventId( DCSSSN_MCU_RELEASECONF_ACK );
			cServMsg.SetMsgBody( &byConfIdx, sizeof(u8) );
			g_cDcsSsnApp.SendMsgToMcuDaemon( cServMsg );
		}
		break;

	case DCS_MCU_RELEASECONF_NACK:
		{
			cReleaseRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cReleaseNackPdu.PduDecode( cReleaseRspCoder );

			byConfIdx = (u8)cReleaseNackPdu.m_wConfId;

			//校验confidx
			if ( 0 == byConfIdx )
			{
				DcsLog("wrong confidx received in release conf nack !\n");
				return;
			}		
			//错误类型
			u8 byErrorId = cReleaseNackPdu.m_cErrorInfor.m_emErrorType;
			if ( CErrorInfor::e_Succuss == byErrorId )
			{
				DcsLog("wrong error id received in release conf nack !\n");
				return;
			}
			CServMsg cServMsg;
			cServMsg.SetEventId( DCSSSN_MCU_RELEASECONF_NACK );
			cServMsg.SetMsgBody( &byConfIdx, sizeof(u8) );
			cServMsg.CatMsgBody( &byErrorId, sizeof(u8) );
			g_cDcsSsnApp.SendMsgToMcuDaemon( cServMsg );
		}
		break;

	case DCS_MCU_CONFRELEASED_NOTIF:
		{
			cReleaseRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cReleaseNotifyPdu.PduDecode( cReleaseRspCoder );

			byConfIdx = (u8)cReleaseNotifyPdu.m_wConfId;
			
			//校验confidx
			if ( 0 == byConfIdx )
			{
				DcsLog("wrong confidx received in release conf notify !\n");
				return;
			}
			CServMsg cServMsg;
			cServMsg.SetEventId( DCSSSN_MCU_CONFRELEASED_NOTIF );
			cServMsg.SetMsgBody( &byConfIdx, sizeof(u8) );
			g_cDcsSsnApp.SendMsgToMcuDaemon( cServMsg );
		}
		break;
		
	default:
		DcsLog("unexpected msg %d<%s> received in release conf rsp !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*=============================================================================
  函 数 名： ProcMcuDcsAddMtReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/28    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcMcuDcsAddMtReq( CMessage *const pcMsg )
{
	//取消息体
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TDcsMtInfo tDcsMtInfo = *(TDcsMtInfo*)cServMsg.GetMsgBody();
	
	//消息体校验
	if ( !MSGBODY_LEN_GE( cServMsg, sizeof(TDcsMtInfo) ) ) 
		return;

	//写入数据单元
	CMcuDcsAddMtReqPdu cAddMtPdu;
	CNetDataCoder      cAddMtCoder;
	u8 achAddMtData[sizeof(CMcuDcsAddMtReqPdu)];

	//会议索引
	u8 byConfIdx = cServMsg.GetConfIdx();
	if ( 0 == byConfIdx )
	{
		DcsLog("get wrong confidx in add mt req !\n");
		return;
	}
	cAddMtPdu.m_wConfId = byConfIdx;

	//终端ID
	u8 byMtId = tDcsMtInfo.m_byMtId;
	if ( 0 == byMtId )
	{
		DcsLog("get wrong mt id in add mt req !\n");
		return;
	}
	cAddMtPdu.m_wMtId = byMtId;

	//判断增加的终端的类型
	//对端主呼, 等待下级加入
	if ( TDcsMtInfo::emBelowJoin == tDcsMtInfo.m_emJoinedType )
	{
		//终端IP有效
		if ( 0 == tDcsMtInfo.m_dwMtIp )
		{
			DcsLog("get wrong mt ip in add mt req !\n");
			return;
		}
		cAddMtPdu.m_cMtShortInfor.m_emPositionType = CMtShortInfor::e_Below_Joined;
		//终端地址
		cAddMtPdu.m_cMtShortInfor.IncludeOption( CMtShortInfor::e_NodeAddress );		
		cAddMtPdu.m_cMtShortInfor.m_dwMtIp	= tDcsMtInfo.m_dwMtIp;
		cAddMtPdu.m_cMtShortInfor.m_wMtPort = 0; 
		//等待超时
		cAddMtPdu.m_cMtShortInfor.IncludeOption( CMtShortInfor::e_WaitJoinTimeout );
		cAddMtPdu.m_cMtShortInfor.m_wTimeOut = 5; //等待下级加入的超时值(s)
	}
	//本端主呼, 邀请下级
	else if ( TDcsMtInfo::emInviteBelow == tDcsMtInfo.m_emJoinedType )
	{
		//终端IP和端口有效
		if ( 0 == tDcsMtInfo.m_dwMtIp || 0 == tDcsMtInfo.m_wPort )
		{
			DcsLog("get wrong mt ip or port in add mt req !\n");
			return;
		}
		cAddMtPdu.m_cMtShortInfor.m_emPositionType = CMtShortInfor::e_Below_Invite;
		//终端地址
		cAddMtPdu.m_cMtShortInfor.IncludeOption( CMtShortInfor::e_NodeAddress );
		cAddMtPdu.m_cMtShortInfor.m_dwMtIp  = tDcsMtInfo.m_dwMtIp;
		cAddMtPdu.m_cMtShortInfor.m_wMtPort = tDcsMtInfo.m_wPort;
	}
	//对端主呼, 上级MCU邀请本端加入
	else if ( TDcsMtInfo::emAboveInvite == tDcsMtInfo.m_emJoinedType )
	{
		//终端IP有效
		if ( 0 == tDcsMtInfo.m_dwMtIp )
		{
			DcsLog("get wrong mt ip in add mt req !\n");
			return;
		}
		cAddMtPdu.m_cMtShortInfor.m_emPositionType = CMtShortInfor::e_Above_Invited;
		//终端地址
		cAddMtPdu.m_cMtShortInfor.IncludeOption( CMtShortInfor::e_NodeAddress );
		cAddMtPdu.m_cMtShortInfor.m_dwMtIp  = tDcsMtInfo.m_dwMtIp;
		//等待超时
		cAddMtPdu.m_cMtShortInfor.IncludeOption( CMtShortInfor::e_WaitJoinTimeout );
		cAddMtPdu.m_cMtShortInfor.m_wTimeOut = 5; //等待上级邀请的超时值(s)
	}
	else
	{
		DcsLog("unexpected mt Join Type received in add mt req, ignore it !\n");
		return;
	}
	
	//编码并发送
	cAddMtCoder.SetNetDataBuffer( achAddMtData, sizeof(achAddMtData) );
	cAddMtPdu.PduEncode( cAddMtCoder );
	SendMsgToDcs( MCU_DCS_ADDMT_REQ, achAddMtData, (u16)cAddMtCoder.GetCurrentLength() );	

    return;
}

/*=============================================================================
  函 数 名： ProcDcsMcuAddMtRsp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/28    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuAddMtRsp( CMessage *const pcMsg )
{
	CDcsMcuAddMtAckPdu		cAddMtAckPdu;
	CDcsMcuAddMtNackPdu		cAddMtNackPdu;
	CNetDataCoder			cAddMtRspCoder;
	
	u8 byConfIdx = 0;	//会议索引号
	
	//消息处理
	switch( pcMsg->event )
	{
	case DCS_MCU_ADDMT_ACK:
		{
			//解码消息
			cAddMtRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cAddMtAckPdu.PduDecode( cAddMtRspCoder );

			//校验confidx
			byConfIdx = (u8)cAddMtAckPdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				DcsLog("wrong confidx: %d get in add mt ack !\n", byConfIdx );
				return;
			}
			//终端ID
			if ( 0 == cAddMtAckPdu.m_wMtId )
			{
				DcsLog("wrong mt id: %d get in add mt ack !\n", cAddMtAckPdu.m_wMtId );
				return;
			}
			u8 byMtId = (u8)cAddMtAckPdu.m_wMtId; 
			//发送
			g_cDcsSsnApp.SendMsgToMcuConf( byConfIdx, DCSSSN_MCU_ADDMT_ACK, &byMtId, sizeof(u8) );
		}
		break;

	case DCS_MCU_ADDMT_NACK:
		{
			//解码消息
			cAddMtRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cAddMtNackPdu.PduDecode( cAddMtRspCoder );
			
			//校验confidx
			byConfIdx = (u8)cAddMtNackPdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				DcsLog("wrong confidx: %d get in add mt Nack !\n", byConfIdx );
				return;
			}
			//终端ID
			if ( 0 == cAddMtNackPdu.m_wMtId )
			{
				DcsLog("wrong mt id: %d get in add mt nack !\n", cAddMtNackPdu.m_wMtId );
				return;
			}
			u8 byMtId = (u8)cAddMtNackPdu.m_wMtId;

			//错误类型
			u8 byErrorId = cAddMtNackPdu.m_cErrorInfor.m_emErrorType;
			if ( CErrorInfor::e_Succuss == byErrorId )
			{
				DcsLog("wrong error id received in add mt Nack !\n");
				return;
			}
			//发送
			CServMsg cServMsg;
			cServMsg.SetConfIdx( byConfIdx );
			cServMsg.SetEventId( DCSSSN_MCU_ADDMT_NACK );
			cServMsg.SetMsgBody( &byMtId, sizeof(u8) );
			cServMsg.CatMsgBody( &byErrorId, sizeof(u8) );
			g_cDcsSsnApp.SendMsgToMcuConf( cServMsg );
		}
		break;

	default:
		DcsLog("unexpected msg %d<%s> received in add mt rsp !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*=============================================================================
  函 数 名： ProcMcuDcsDelMtReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/28    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcMcuDcsDelMtReq( CMessage *const pcMsg )
{
	//取消息体
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byMtId = *cServMsg.GetMsgBody();
	
	//消息体校验
	if ( !MSGBODY_LEN_GE( cServMsg, sizeof(u8) ) ) 
		return;

	//写入数据单元
	CMcuDcsDelMtReqPdu cDelMtPdu;
	CNetDataCoder      cDelMtCoder;
	u8 achDelMtData[sizeof(CMcuDcsDelMtReqPdu)];
	
	//会议索引
	u8 byConfIdx = cServMsg.GetConfIdx();
	if ( 0 == byConfIdx )
	{
		DcsLog("get wrong confidx in del mt req !\n");
		return;
	}
	cDelMtPdu.m_wConfId = byConfIdx;
	
	//终端ID
	if ( 0 == byMtId )
	{
		DcsLog("get wrong mt id in del mt req !\n");
		return;
	}
	cDelMtPdu.m_wMtId = byMtId;

	//编码并发送
	cDelMtCoder.SetNetDataBuffer( achDelMtData, sizeof(achDelMtData) );
	cDelMtPdu.PduEncode( cDelMtCoder );
	SendMsgToDcs( MCU_DCS_DELMT_REQ, achDelMtData, (u16)cDelMtCoder.GetCurrentLength() );
	
    return;
}

/*=============================================================================
  函 数 名： ProcDcsMcuDelMtRsp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/28    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuDelMtRsp( CMessage *const pcMsg )
{
	CDcsMcuDelMtAckPdu		cDelMtAckPdu;
	CDcsMcuDelMtNackPdu		cDelMtNackPdu;
	CNetDataCoder			cDelMtRspCoder;
	
	u8 byConfIdx = 0;	//会议索引号
	
	//消息处理
	switch( pcMsg->event )
	{
	case DCS_MCU_DELMT_ACK:
		{
			//解码消息
			cDelMtRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cDelMtAckPdu.PduDecode( cDelMtRspCoder );
			
			//校验confidx
			byConfIdx = (u8)cDelMtAckPdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				DcsLog("wrong confidx: %d get in del mt ack !\n", byConfIdx );
				return;
			}
			//终端ID
			if ( 0 == cDelMtAckPdu.m_wMtId )
			{
				DcsLog("wrong mt id: %d get in del mt ack !\n", cDelMtAckPdu.m_wMtId );
				return;
			}
			u8 byMtId = (u8)cDelMtAckPdu.m_wMtId;
			
			//发送
			g_cDcsSsnApp.SendMsgToMcuConf( byConfIdx, DCSSSN_MCU_DELMT_ACK, &byMtId, sizeof(u8) );
			
		}
		break;
		
	case DCS_MCU_DELMT_NACK:
		{
			//解码消息
			cDelMtRspCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cDelMtNackPdu.PduDecode( cDelMtRspCoder );
			
			//校验confidx
			byConfIdx = (u8)cDelMtNackPdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				DcsLog("wrong confidx get in del mt Nack !\n");
				return;
			}
			//终端ID
			if ( 0 == cDelMtNackPdu.m_wMtId )
			{
				DcsLog("wrong mt id: %d get in del mt Nack !\n", cDelMtNackPdu.m_wMtId );
				return;
			}
			u8 byMtId = (u8)cDelMtNackPdu.m_wMtId;

			//错误类型
			u8 byErrorId = cDelMtNackPdu.m_cErrorInfor.m_emErrorType;
			if ( CErrorInfor::e_Succuss == byErrorId )
			{
				DcsLog("wrong error id received in del mt Nack !\n");
				return;
			}
			//发送
			CServMsg cServMsg;
			cServMsg.SetConfIdx( byConfIdx );
			cServMsg.SetEventId( DCSSSN_MCU_DELMT_NACK );
			cServMsg.SetMsgBody( &byMtId, sizeof(u8) );
			cServMsg.CatMsgBody( &byErrorId, sizeof(u8) );
			g_cDcsSsnApp.SendMsgToMcuConf( cServMsg );
		}
		break;
		
	default:
		DcsLog("unexpected msg %d<%s> received in del mt rsp !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*=============================================================================
  函 数 名： ProcDcsMcuMtOnlineNotify
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/28    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuMtOnlineNotify( CMessage *const pcMsg )
{
	CDcsMcuMtMtOnlineNotifyPdu	cMtOnlinePdu;
	CNetDataCoder				cMtOnlineCoder;
	
	u8 byConfIdx = 0;	//会议索引号
	
	//消息处理
	switch( pcMsg->event )
	{
	case DCS_MCU_MTJOINED_NOTIF:
		{
			//解码消息
			cMtOnlineCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cMtOnlinePdu.PduDecode( cMtOnlineCoder );
			
			//校验confidx
			byConfIdx = (u8)cMtOnlinePdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				DcsLog("wrong confidx get in mt online notify !\n");
				return;
			}
			//填充数据终端信息
			TDcsMtInfo tDcsMtInfo;
			memset( &tDcsMtInfo, 0, sizeof(tDcsMtInfo) );

			//终端ID
			if ( 0 == cMtOnlinePdu.m_wMtId )
			{
				DcsLog("wrong mt id get in online notify !\n");
				return;
			}
			tDcsMtInfo.m_byMtId = (u8)cMtOnlinePdu.m_wMtId;

			//上线终端的类型
			if ( CMtShortInfor::e_Mcu == cMtOnlinePdu.m_cMtShortInfor.m_emDeviceType )
			{
				tDcsMtInfo.m_byDeviceType = TDcsMtInfo::emT120Mcu;
			}
			else if ( CMtShortInfor::e_Mt == cMtOnlinePdu.m_cMtShortInfor.m_emDeviceType ) 
			{
				tDcsMtInfo.m_byDeviceType = TDcsMtInfo::emT120Mt;
			}
			else
			{
				DcsLog("mt %d wrong device type get in online notify !\n", cMtOnlinePdu.m_wMtId );
				return;
			}	
			//发给McuVc
			CServMsg cServMsg;
			cServMsg.SetConfIdx( byConfIdx );
			cServMsg.SetEventId( DCSSSN_MCU_MTJOINED_NOTIF );
			cServMsg.SetMsgBody( (u8*)&tDcsMtInfo, sizeof(tDcsMtInfo) );
			g_cDcsSsnApp.SendMsgToMcuConf( cServMsg );
		}
		break;
		
	default:
		DcsLog("unexpected msg %d<%s> received in mt online notify !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*=============================================================================
  函 数 名： ProcDcsMcuMtOfflineNotify
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/28    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::ProcDcsMcuMtOfflineNotify( CMessage *const pcMsg )
{
	CDcsMcuMtMtOfflineNotifyPdu	cMtOfflinePdu;
	CNetDataCoder				cMtOfflineCoder;
	
	u8 byConfIdx = 0;	//会议索引号
	
	//消息处理
	switch( pcMsg->event )
	{
	case DCS_MCU_MTLEFT_NOTIF:
		{
			//解码消息
			cMtOfflineCoder.SetNetDataBuffer( pcMsg->content, pcMsg->length );
			cMtOfflinePdu.PduDecode( cMtOfflineCoder );
			
			//校验confidx
			byConfIdx = (u8)cMtOfflinePdu.m_wConfId;
			if ( 0 == byConfIdx )
			{
				DcsLog("wrong confidx %d get in mt offline notify !\n", byConfIdx );
				return;
			}
			//终端ID
			if ( 0 == cMtOfflinePdu.m_wMtId )
			{
				DcsLog("wrong mt id %d get in online notify !\n", cMtOfflinePdu.m_wMtId );
				return;
			}
			u8 byMtId = (u8)cMtOfflinePdu.m_wMtId;			
	
			//发给McuVc
			g_cDcsSsnApp.SendMsgToMcuConf( byConfIdx, DCSSSN_MCU_MTLEFT_NOTIF, &byMtId, sizeof(u8) );
		}
		break;
		
	default:
		DcsLog("unexpected msg %d<%s> received in mt offline notify !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

    return;
}

/*=============================================================================
  函 数 名： McuDcsRegNack
  功    能： 注册拒绝应答
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/26    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::McuDcsRegNack( CMcuDcsRegNackPdu	cRegNackPdu, 
								 CNetDataCoder		cRegCoder, 
								 u8		achRegNackData[sizeof(CMcuDcsRegNackPdu)], 
								 u16	wErrorType, 
								 u16	wEvent	)
{
	cRegNackPdu.m_cErrorInfor.m_emErrorType = (CErrorInfor::EErrorType)wErrorType;
	cRegCoder.SetNetDataBuffer( achRegNackData, sizeof(achRegNackData) );
	cRegNackPdu.PduEncode( cRegCoder );
	SendMsgToDcs( wEvent, achRegNackData, (u16)cRegCoder.GetCurrentLength() );
    return;
}

/*=============================================================================
  函 数 名： McuDcsRegNack
  功    能： 注册接受应答
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/26    4.0			张宝卿                  创建
=============================================================================*/
void CDcsSsnInst::McuDcsRegAck( CMcuDcsRegAckPdu cRegAckPdu, 
								CNetDataCoder	 cRegCoder, 
								u8	 achRegAckData[sizeof(CMcuDcsRegAckPdu)],
								u16	 wEvent )
{
	cRegCoder.SetNetDataBuffer( achRegAckData, sizeof(achRegAckData) );
	cRegAckPdu.PduEncode( cRegCoder );
	SendMsgToDcs( wEvent, achRegAckData, (u16)cRegCoder.GetCurrentLength() );
    return;
}


CDcsConfig::CDcsConfig()
{
	memset( (void*)&m_adwDcsIp, 0, sizeof(m_adwDcsIp) );
}

CDcsConfig::~CDcsConfig()
{
	
}

/*=============================================================================
  函 数 名： SendMsgToDcs
  功    能： 发消息给DCS
  算法实现： 
  全局变量： 
  参    数：  u16 wEvent
             u8 *const pbyMsg
             u16 wLen
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/14    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CDcsSsnInst::SendMsgToDcs( u16 wEvent, u8 *const pbyMsg, u16 wLen )
{
	if( OspIsValidTcpNode( m_dwDcsNode ) )
	{
		u32 dwRet = OspPost( m_dwDcsIId, wEvent, pbyMsg, wLen, m_dwDcsNode, MAKEIID( GetAppID(), GetInsID()) );
		if( OSP_OK == dwRet )
		{
			return TRUE;
		}
		else
		{
			DcsLog( "Send msg %d<%s> to Dcs failed !\n", wEvent, OspEventDesc(wEvent) );
		}
	}
	DcsLog("Send: dcs node %d invalid, check it !\n", m_dwDcsNode);
	return FALSE;
}

/*=============================================================================
  函 数 名： SendMsgToInst
  功    能： DCS会话分发消息给普通实例
  算法实现： 
  全局变量： 
  参    数：  s32 nInst
             u16 wEvent
             u8 *const pbyMsg
             u16 wLen
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/13    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CDcsSsnInst::SendMsgToInst( s32 nInst, u16 wEvent, u8 *const pbyMsg, u16 wLen )
{
	u32 dwRet = OspPost( MAKEIID(AID_MCU_DCSSSN, nInst), wEvent, pbyMsg, wLen, 0, MAKEIID(AID_MCU_DCSSSN, CInstance::DAEMON) );
	if ( OSP_OK == dwRet )
	{
		return TRUE;
	}
	DcsLog("send msg: %d<%s> to dcsssn inst failed !\n", wEvent, OspEventDesc(wEvent) );
	return FALSE;
}


/*=============================================================================
  函 数 名： SendMsgToMcuConf
  功    能： 发消息给MCU会议
  算法实现： 
  全局变量： 
  参    数：  
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/13    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CDcsConfig::SendMsgToMcuConf( u8 byConfIdx, u16 wEvent, u8 *const pbyMsg, u16 wLen )
{
	if( 0 != byConfIdx )
	{	
		CServMsg cServMsg;
		cServMsg.SetConfIdx( byConfIdx );
		cServMsg.SetEventId( wEvent );
		cServMsg.SetMsgBody( pbyMsg, wLen );

		u32 dwRet = OspPost( MAKEIID( AID_MCU_VC, g_cMcuVcApp.GetConfMapInsId(byConfIdx) ), 
					wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		return TRUE ;
	}
	DcsLog( "send msg: %d<%s> to mcu conf failed !\n", wEvent, OspEventDesc(wEvent) );
	return FALSE;
}

/*=============================================================================
  函 数 名： SendMsgToMcuConf
  功    能： 发消息给MCU会议
  算法实现： 
  全局变量： 
  参    数： CServMsg &cServMsg
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/28    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CDcsConfig::SendMsgToMcuConf( CServMsg &cServMsg )
{
	u32 dwRet = OspPost( MAKEIID( AID_MCU_VC, g_cMcuVcApp.GetConfMapInsId(cServMsg.GetConfIdx())), 
					     cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	if ( 0 == dwRet )
	{
		return TRUE;
	}
	DcsLog( "send msg: %d<%s> to mcu conf failed !\n", cServMsg.GetEventId(), OspEventDesc(cServMsg.GetEventId()) );
	return FALSE;
}

/*=============================================================================
  函 数 名： SendMsgToMcuDaemon
  功    能： 发消息给MCU daemon实例
  算法实现： 
  全局变量： 
  参    数：  u16 wEvent
             u8 *const pbyMsg
             u16 wLen
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/13    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CDcsConfig::SendMsgToMcuDaemon( CServMsg &cServMsg )
{
	u32 dwRet = OspPost( MAKEIID(AID_MCU_VC, CInstance::DAEMON), cServMsg.GetEventId(), 
						 cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	if ( OSP_OK == dwRet )
	{
		return TRUE;
	}
	DcsLog("Send msg: %d<%s> to mcuvc daemon failed !\n", cServMsg.GetEventId(), OspEventDesc(cServMsg.GetEventId()) );
	return FALSE;
}

/*=============================================================================
  函 数 名： BroadcastToAllDcsSsn
  功    能： 发消息给所有的DCS会话实例
  算法实现： 
  全局变量： 
  参    数： u16 wEvent
             u8 *const pbyMsg
             u16 wLen
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/11/27    4.0			张宝卿                  创建
=============================================================================*/
void CDcsConfig::BroadcastToAllDcsSsn( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
    //本端主用则允许向外部各App投递消息，否则丢弃
    if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
    {
        return;
    }
    
    ::OspPost(MAKEIID( AID_MCU_DCSSSN, CInstance::EACH ), wEvent, pbyMsg, wLen);
    
    return;
}

/*=============================================================================
  函 数 名： MSGBODY_LEN_GE
  功    能： 校验消息体的长度
  算法实现： 
  全局变量： 
  参    数： CServMsg &cServMsg
             u16 wLen
  返 回 值： inline BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/27    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 MSGBODY_LEN_GE( CServMsg &cServMsg, u32 wLen )
{
	if(cServMsg.GetMsgBodyLen() < wLen)
	{
		const s8* pszMsgStr = OspEventDesc( cServMsg.GetEventId() );
		
		DcsLog("Error body len %u for msg %u(%s) detected:  "
				"Should >= %u. Ignore it.\n", cServMsg.GetMsgBodyLen(), 
				cServMsg.GetEventId(),	pszMsgStr ? pszMsgStr : "null",	wLen);									
		return FALSE;
	}	
	return TRUE;
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
    06/05/19    4.0         张宝卿        创建
====================================================================*/
void CDcsSsnInst::InstanceDump( u32 param )
{
	s8 achType[32];
	memset( achType, 0, sizeof(achType) );
    strncpy( achType, "DCS", sizeof(achType) );
	OspPrintf( TRUE, FALSE, "%3u %5u %5u %6s   %s\n", GetInsID(), GetInsID(), m_dwDcsNode, achType, g_cMcuVcApp.GetDcsAlias((u8)GetInsID()) );
    return;
}


// END OF FILE

