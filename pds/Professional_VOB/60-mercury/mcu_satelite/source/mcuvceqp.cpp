/*****************************************************************************
   模块名      : mcu
   文件名      : mcuvceqp.cpp
   相关文件    : mcuvc.h
   文件实现功能: MCU业务交换函数
   作者        : 胡昌威
   版本        : V2.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/06  2.0         胡昌威      创建
   2003/12/17  3.0         zmy         修该码流适配器部分
   2005/02/19  3.6         万春雷      级联修改、与3.5版本合并                                    
******************************************************************************/

#include "evmcumcs.h"
#include "evmcumt.h"
#include "evmcueqp.h"
#include "evmcu.h"
#include "evmcutest.h"
#include "mcuvc.h"
#include "mcsssn.h"
#include "mcuerrcode.h"
#include "mtadpssn.h"
#include "mpmanager.h"



/*------------------------------------------------------------------*/
/*                                Common                            */
/*------------------------------------------------------------------*/

/*====================================================================
    函数名      ：DaemonProcMcsMcuGetPeriEqpStatusReq
    功能        ：查询外设状态请求处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/20    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::DaemonProcMcsMcuGetPeriEqpStatusReq( const CMessage * pcMsg )
{
    CServMsg	cServMsg( pcMsg->content, pcMsg->length );
    TEqp		*ptEqp = ( TEqp * )cServMsg.GetMsgBody();
    TPeriEqpStatus	tEqpStatus;
    TPeriDcsStatus  tDcsStatus;
    
    /*
    //not belong to this MCU, nack
    if( ptEqp->GetMcuId() != LOCAL_MCUID )
    {
        ConfLog( FALSE, "CMcuVcInst: Wrong! The specified periequipment not belong to this MCU!\n" );
        cServMsg.SetErrorCode( ERR_MCU_VISIT_NONCONNEQP );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }
    */

    //其他外设
    if ( MCS_MCU_GETPERIDCSSTATUS_REQ != pcMsg->event )
    {
        //ERR_MCU_WRONGEQP
        if (ptEqp->GetEqpId() == 0 || ptEqp->GetEqpId() > MAXNUM_MCU_PERIEQP)
        {
            ConfLog(FALSE, "CMcuVcInst: Wrong! The specified periequipment id.%d invaild!\n", ptEqp->GetEqpId());
            cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
        
        //not online
        if( !g_cMcuVcApp.IsPeriEqpConnected( ptEqp->GetEqpId() ) )
        {
            memcpy( &tEqpStatus, ptEqp, sizeof( TEqp ) );
            tEqpStatus.m_byOnline = FALSE;
        }
        else
        {
            g_cMcuVcApp.GetPeriEqpStatus( ptEqp->GetEqpId(), &tEqpStatus );
        }
        
        //send ack
        cServMsg.SetMsgBody( ( u8 * )&tEqpStatus, sizeof( tEqpStatus ) );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
    }
    //DCS
    else
    {
        //ERR_MCU_WRONGDCS
        if (ptEqp->GetEqpId() == 0 || ptEqp->GetEqpId() > MAXNUM_MCU_DCS)
        {
            ConfLog(FALSE, "CMcuVcInst: Wrong! The specified peri dcs id.%d invaild!\n", ptEqp->GetEqpId());
            cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
        
        //not online
        if( !g_cMcuVcApp.IsPeriDcsConnected(ptEqp->GetEqpId()) )
        {
            memcpy( &tDcsStatus, ptEqp, sizeof( TEqp ) );
            tDcsStatus.m_byOnline = FALSE;
        }
        else
        {
            g_cMcuVcApp.GetPeriDcsStatus( ptEqp->GetEqpId(), &tDcsStatus );
        }
        
        //send ack
        cServMsg.SetMsgBody( ( u8 * )&tDcsStatus, sizeof( tDcsStatus ) );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
    }
}

/*====================================================================
    函数名      ：DaemonProcPeriEqpMcuStatusNotif
    功能        ：外设状态上报
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/10    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::DaemonProcPeriEqpMcuStatusNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TPeriEqpStatus tStatus = *(TPeriEqpStatus *)cServMsg.GetMsgBody();

    if (tStatus.GetMcuId() != LOCAL_MCUID ||
        tStatus.GetEqpId() == 0 || tStatus.GetEqpId() > MAXNUM_MCU_PERIEQP)
    {
        OspPrintf(TRUE, FALSE, "[PeriEqpStatusNotif]TPeriEqpStatus content error! GetMcuId() = %d GetEqpId() = %d\n",
        tStatus.GetMcuId(), tStatus.GetEqpId());
        return;
    }

    TPeriEqpStatus tOldStatus;
    memset(&tOldStatus, 0, sizeof(TPeriEqpStatus));
    TPeriEqpStatus tNewHduStatus = tStatus;
	// 通知N+1备份机, zgc, 2008-04-07
	BOOL32 bNotifNPlus = FALSE;

    if (VMP_MCU_VMPSTATUS_NOTIF == pcMsg->event ||
        VMPTW_MCU_VMPTWSTATUS_NOTIF == pcMsg->event)
    {
		//alias //fixme-- zw
		if ( g_cMcuAgent.IsSVmp( tStatus.GetEqpId() ) )
		{				
			TEqpSvmpInfo tEqpSvmpInfo;
			g_cMcuAgent.GetEqpSvmpCfgById( tStatus.GetEqpId(), &tEqpSvmpInfo );
			
			tStatus.SetAlias( tEqpSvmpInfo.GetAlias() );
		}
		
        g_cMcuVcApp.GetPeriEqpStatus(tStatus.GetEqpId(), &tOldStatus);
        if (tOldStatus.IsNull())
        {
            g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tStatus);
			EqpLog( "VMP.%u sub type is:%u\n",tStatus.GetEqpId(),tStatus.m_tStatus.tVmp.m_bySubType);
			EqpLog( "VMP.%u Version is:%u\n", tStatus.GetEqpId(), tStatus.m_tStatus.tVmp.m_byBoardVer);
			// 通知N+1备份机, zgc, 2008-04-07
			bNotifNPlus = TRUE;
        }
        else
        {
			// 通知N+1备份机, zgc, 2008-04-07
			if ( tOldStatus.m_tStatus.tVmp.m_byChlNum != tStatus.m_tStatus.tVmp.m_byChlNum )
			{
				bNotifNPlus = TRUE;
			}
	
            tOldStatus.m_tStatus.tVmp.m_byChlNum = tStatus.m_tStatus.tVmp.m_byChlNum;
            tOldStatus.SetAlias(tStatus.GetAlias());

			tOldStatus.SetEqpType(tStatus.GetEqpType());// xliang [2/12/2009]确保eqptype值统一，解决HDVMP bug
			tOldStatus.m_tStatus.tVmp.m_bySubType = tStatus.m_tStatus.tVmp.m_bySubType;//子类型 
			tOldStatus.m_tStatus.tVmp.m_byBoardVer = tStatus.m_tStatus.tVmp.m_byBoardVer;	//A/B板区分
			tStatus = tOldStatus;
            g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tOldStatus);

			EqpLog( "VMP.%u sub type is:%u\n",tStatus.GetEqpId(),tStatus.m_tStatus.tVmp.m_bySubType);
			EqpLog( "VMP.%u Version is:%u\n", tStatus.GetEqpId(), tStatus.m_tStatus.tVmp.m_byBoardVer);
        }
    }
    else if (TVWALL_MCU_STATUS_NOTIF == pcMsg->event)
    {
        u8 byMtConfIdx;
        g_cMcuVcApp.GetPeriEqpStatus(tStatus.GetEqpId(), &tOldStatus);
        for (u8 byLoop = 0; byLoop < tStatus.m_tStatus.tTvWall.byChnnlNum; byLoop++)
        {
            if (tOldStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType != 0)
            {
                tStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 
                                tOldStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType;

                byMtConfIdx = tOldStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
                tStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(byMtConfIdx);
            }
        }
        g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tStatus);
    }
    else if (HDU_MCU_STATUS_NOTIF == pcMsg->event)    //4.6 jlb
    {
        memset(&tOldStatus, 0, sizeof(tOldStatus));
        g_cMcuVcApp.GetPeriEqpStatus(tStatus.GetEqpId(), &tOldStatus);
        for(u8 byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; byLoop++)
        {
            if (tOldStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType != 0)
            {
                tStatus.m_tStatus.tHdu.atVideoMt[byLoop] =
                    tOldStatus.m_tStatus.tHdu.atVideoMt[byLoop];
            }
			if ( tOldStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].GetIsMute() || 
				  tOldStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].GetVolume() != 0 )
			{
				tStatus.m_tStatus.tHdu.atHduChnStatus[byLoop] = 
					tOldStatus.m_tStatus.tHdu.atHduChnStatus[byLoop];
				tNewHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop] = 
					tOldStatus.m_tStatus.tHdu.atHduChnStatus[byLoop];
			}
			tStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetChnIdx( byLoop );
			tNewHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetChnIdx( byLoop );
        }
		if (tOldStatus.GetEqpType() != EQP_TYPE_HDU)
		{
			tStatus.SetEqpType(EQP_TYPE_HDU);
			tNewHduStatus.SetEqpType( EQP_TYPE_HDU );
		}

        g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tStatus);
    }
    else if (MIXER_MCU_MIXERSTATUS_NOTIF == pcMsg->event)
    {
        TMixerGrpStatus tOldMixerGrpStatus;
        g_cMcuVcApp.GetPeriEqpStatus(tStatus.GetEqpId(), &tOldStatus);

        for (u8 byLoop = 0; byLoop < tStatus.m_tStatus.tMixer.m_byGrpNum; byLoop++)
        {
            tOldMixerGrpStatus = tOldStatus.m_tStatus.tMixer.m_atGrpStatus[byLoop];
            tStatus.m_tStatus.tMixer.m_atGrpStatus[byLoop].m_byGrpId = tOldMixerGrpStatus.m_byGrpId;
            tStatus.m_tStatus.tMixer.m_atGrpStatus[byLoop].m_byGrpMixDepth = tOldMixerGrpStatus.m_byGrpMixDepth;
            //tStatus.m_tStatus.tMixer.m_atGrpStatus[byLoop].m_byGrpState = tOldMixerGrpStatus.m_byGrpState;
            tStatus.m_tStatus.tMixer.m_atGrpStatus[byLoop].m_byConfId = tOldMixerGrpStatus.m_byConfId;
        }

        g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tStatus);
    }
    else if (PRS_MCU_PRSSTATUS_NOTIF == pcMsg->event)
    {
        TPeriEqpStatus tOldPrsStatus;
        g_cMcuVcApp.GetPeriEqpStatus(tStatus.GetEqpId(), &tOldPrsStatus);

        // 顾振华 [6/14/2006] 恢复是否保留 IsReserved 字段。本字段归MCU管理，Prs不知情
        for (u8 byLoop = 0; byLoop < MAXNUM_PRS_CHNNL; byLoop ++)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[byLoop].  \
                SetReserved( tOldPrsStatus.m_tStatus.tPrs.m_tPerChStatus[byLoop].IsReserved() );
			//恢复每个通道服务的会议Idx, zgc, 2007/04/24
			tStatus.m_tStatus.tPrs.SetChnConfIdx( byLoop, tOldPrsStatus.m_tStatus.tPrs.GetChnConfIdx( byLoop ) );
        }

        g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tStatus);
    }
    else
    {
        // 顾振华 [6/14/2006] 目前Bas只有在连接时会发送状态通知，可以认为不会对mcu内部保存的逻辑造成冲击
        // 因此不需要像PRS一样
        if ( !g_cMcuAgent.IsEqpBasHD( tStatus.GetEqpId() ) )
        {
            g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tStatus);
        }
        else
        {
            //高清适配器的状态通知只有EQP,是否在线和别名是有效的, zgc, 2008-09-03
            TPeriEqpStatus tOldStatus;
            g_cMcuVcApp.GetPeriEqpStatus(tStatus.GetEqpId(), &tOldStatus);
            tOldStatus.SetMcuEqp( tStatus.GetMcuId(), tStatus.GetEqpId(), tStatus.GetEqpType() );
            tOldStatus.m_byOnline = tStatus.m_byOnline;
            tOldStatus.SetAlias( tStatus.GetAlias() );
            
            //还有mau类型
            if (tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->IsReserved() ||
                tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->IsReserved())
            {
                if (tOldStatus.m_tStatus.tHdBas.GetEqpType() != tStatus.m_tStatus.tHdBas.GetEqpType())
                {
                    tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->SetIsReserved(FALSE);
                    tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->SetIsReserved(FALSE);
                }
            }
            if (THDBasChnStatus::IDLE == tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->GetStatus())
            {
                tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->SetStatus(THDBasChnStatus::READY);
            }
            if (THDBasChnStatus::IDLE == tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->GetStatus())
            {
                tOldStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->SetStatus(THDBasChnStatus::READY);
            }
            tOldStatus.m_tStatus.tHdBas.SetEqpType(tStatus.m_tStatus.tHdBas.GetEqpType());
            g_cMcuVcApp.SetPeriEqpStatus(tStatus.GetEqpId(), &tOldStatus);
        }
		// 通知N+1备份机
		bNotifNPlus = TRUE;
    }

    //status notification
	if ( EQP_TYPE_HDU == tStatus.GetEqpType())
	{
		cServMsg.SetMsgBody( (u8*)&tNewHduStatus, sizeof(tNewHduStatus) );
	}
	else
	{
        cServMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
    }

	SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);

	// zgc 2008-04-02 N+1模式下通知主mcu外设能力
    // zgc 2008-08-08 增加通知高清适配(目前暂时注销)
    if ( VMP_MCU_VMPSTATUS_NOTIF == pcMsg->event &&
		 BAS_MCU_BASSTATUS_NOTIF == pcMsg->event &&
         HDBAS_MCU_BASSTATUS_NOTIF == pcMsg->event && 
		g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_MASTER_CONNECTED)
    {
		if ( bNotifNPlus )
        {
			TNPlusEqpCap tEqpCap = g_cNPlusApp.GetMcuEqpCap();
			cServMsg.SetEventId(MCU_NPLUS_EQPCAP_NOTIF);
			cServMsg.SetMsgBody((u8 *)&tEqpCap, sizeof(tEqpCap));
			g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
    }

    g_cMcuVcApp.BroadcastToAllConf(pcMsg->event, pcMsg->content, pcMsg->length);

    return;
}

/*====================================================================
    函数名      ：DaemonProcMcuEqpConnectedNotif
    功能        ：外设注册成功处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/10    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::DaemonProcMcuEqpConnectedNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TPeriEqpRegReq tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();
    TLogicalChannel tLogicalChannel;

    //not connected periequipment
    if (LOCAL_MCUID != tRegReq.GetMcuId())
    {
        return;
    }

    //得到接收外设数据的Mp地址
    u32 dwRecvIpAddr;
    u32 dwEqpAddr;
    u16 wRecvPort;
    TEqp tEqp = (TEqp)tRegReq;
    g_cMpManager.GetSwitchInfo(tEqp, dwRecvIpAddr, wRecvPort, dwEqpAddr);
	CallLog("[DaemonProcMcuEqpConnectedNotif] MP RecvPort is %d\n",wRecvPort);

    //设置前向视频通道
    tLogicalChannel.m_tRcvMediaChannel.SetIpAddr(ntohl(tRegReq.GetPeriEqpIpAddr()));
    tLogicalChannel.m_tRcvMediaChannel.SetPort(tRegReq.GetStartPort());
    tLogicalChannel.m_tRcvMediaCtrlChannel.SetIpAddr(ntohl(tRegReq.GetPeriEqpIpAddr()));
    tLogicalChannel.m_tRcvMediaCtrlChannel.SetPort(tRegReq.GetStartPort() + 1);
    tLogicalChannel.m_tSndMediaCtrlChannel.SetIpAddr(dwRecvIpAddr);	//设0交换时绑定用
    tLogicalChannel.m_tSndMediaCtrlChannel.SetPort(tRegReq.GetStartPort() + 1);
    g_cMcuVcApp.SetPeriEqpLogicChnnl(tRegReq.GetEqpId(), MODE_VIDEO,
                                     tRegReq.GetChnnlNum(), &tLogicalChannel, TRUE);

    u32 dwIpAddr = tRegReq.GetPeriEqpIpAddr();
    g_cMcuVcApp.SetEqpIp(tRegReq.GetEqpId(), dwIpAddr);

    //设置前向音频通道
    //对于PRS而言，一个PRS可支持多个音视频通道(默认总共16个),
    //各通道接收端口统一为PRS起始接收端口，以通道号区分实际的接收端口
    if (EQP_TYPE_PRS == tRegReq.GetEqpType())
    {
        tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( ntohl(tRegReq.GetPeriEqpIpAddr()) );
        tLogicalChannel.m_tRcvMediaChannel.SetPort( tRegReq.GetStartPort() );
        tLogicalChannel.m_tRcvMediaCtrlChannel.SetIpAddr( ntohl(tRegReq.GetPeriEqpIpAddr()) );
        tLogicalChannel.m_tRcvMediaCtrlChannel.SetPort( tRegReq.GetStartPort() + 1 );
        tLogicalChannel.m_tSndMediaCtrlChannel.SetIpAddr( dwRecvIpAddr );	//设0交换时绑定用
        tLogicalChannel.m_tSndMediaCtrlChannel.SetPort( tRegReq.GetStartPort() + 1 );
    }
    else
    {
        tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( ntohl(tRegReq.GetPeriEqpIpAddr()) );
        tLogicalChannel.m_tRcvMediaChannel.SetPort( tRegReq.GetStartPort() + 2 );
        tLogicalChannel.m_tRcvMediaCtrlChannel.SetIpAddr( ntohl(tRegReq.GetPeriEqpIpAddr()) );
        tLogicalChannel.m_tRcvMediaCtrlChannel.SetPort( tRegReq.GetStartPort() + 3 );
        tLogicalChannel.m_tSndMediaCtrlChannel.SetIpAddr( dwRecvIpAddr );	//设0交换时绑定用
        tLogicalChannel.m_tSndMediaCtrlChannel.SetPort( tRegReq.GetStartPort() + 3 );
    }       
    g_cMcuVcApp.SetPeriEqpLogicChnnl(tRegReq.GetEqpId(), MODE_AUDIO,
                                     tRegReq.GetChnnlNum(), &tLogicalChannel, TRUE);

    //设置连接状态
    g_cMcuVcApp.SetPeriEqpConnected(tRegReq.GetEqpId(), TRUE);
    g_cMcuVcApp.SetEqpAlias(tRegReq.GetEqpId(), tRegReq.GetEqpAlias());

    //通知所有会议
    switch(tRegReq.GetEqpType())
    {
    case EQP_TYPE_MIXER:
        g_cMcuVcApp.BroadcastToAllConf(MCU_MIXERCONNECTED_NOTIF, pcMsg->content, pcMsg->length);
        break;
    case EQP_TYPE_BAS:
        g_cMcuVcApp.BroadcastToAllConf(MCU_BASCONNECTED_NOTIF, pcMsg->content, pcMsg->length);
        break;
    case EQP_TYPE_PRS:
        g_cMcuVcApp.BroadcastToAllConf(MCU_PRSCONNECTED_NOTIF, pcMsg->content, pcMsg->length);
        break;
    case EQP_TYPE_TVWALL:   //tvwall2
        g_cMcuVcApp.BroadcastToAllConf(MCU_TVWALLCONNECTED_NOTIF, pcMsg->content, pcMsg->length);
        break;
    case EQP_TYPE_RECORDER:
        g_cMcuVcApp.BroadcastToAllConf(MCU_RECCONNECTED_NOTIF, pcMsg->content, pcMsg->length);
        break;
    case EQP_TYPE_VMP:   //vmp
        g_cMcuVcApp.BroadcastToAllConf(MCU_VMPCONNECTED_NOTIF, pcMsg->content, pcMsg->length);
        break;
	case EQP_TYPE_HDU:    //4.6  新加 jlb
		g_cMcuVcApp.BroadcastToAllConf( MCU_HDUCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;
    case EQP_TYPE_VMPTW:   //vmptw
        g_cMcuVcApp.BroadcastToAllConf(MCU_VMPTWCONNECTED_NOTIF, pcMsg->content, pcMsg->length);
        break;
    default:
        break;
    }

    // xsl [11/29/2006] N+1模式下通知主mcu外设能力
    if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_MASTER_CONNECTED)
    {
        TNPlusEqpCap tEqpCap = g_cNPlusApp.GetMcuEqpCap();
        cServMsg.SetEventId(MCU_NPLUS_EQPCAP_NOTIF);
        cServMsg.SetMsgBody((u8 *)&tEqpCap, sizeof(tEqpCap));
        g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }

    // zbq[08/18/2009] 负载均衡的相关处理
    u8 byBandReserved = 0;
    u8 bySwitchMpId = 0;
    if (
        (EQP_TYPE_BAS == tRegReq.GetEqpType() &&
         g_cMcuAgent.IsEqpBasHD(tRegReq.GetEqpId()) &&
         g_cMcuVcApp.GetBandWidthReserved4HdBas())
         ||
        (EQP_TYPE_VMP == tRegReq.GetEqpType() &&
         g_cMcuAgent.IsSVmp(tRegReq.GetEqpId()) &&
         g_cMcuVcApp.GetBandWidthReserved4HdVmp())
       )
    {
        byBandReserved = g_cMcuVcApp.GetBandWidthReserved4HdBas();
        if (0 == byBandReserved)
        {
            return;
        }
        bySwitchMpId = g_cMcuVcApp.FindMp(dwRecvIpAddr);
        g_cMcuVcApp.m_atMpData[bySwitchMpId-1].m_wNetBandReserved += byBandReserved;

        //外设累加预留导致的“超负”，给出提示
        if (byBandReserved <= g_cMcuVcApp.m_atMpData[bySwitchMpId-1].m_wNetBandAllowed &&
            g_cMcuVcApp.m_atMpData[bySwitchMpId-1].m_wNetBandReserved >
            g_cMcuVcApp.m_atMpData[bySwitchMpId-1].m_wNetBandAllowed)
        {
            NotifyMcsAlarmInfo(0, ERR_MCU_MPRESERVEDBANDWIDTH_FULL);
        }
    }
    
    return;
}

/*====================================================================
    函数名      ：DaemonProcMcuEqpDisconnectedNotif
    功能        ：外设断链通知处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/10    1.0         LI Yi         创建
	04/03/27    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::DaemonProcMcuEqpDisconnectedNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TEqp	tEqp = *( TEqp * )cServMsg.GetMsgBody();
	TLogicalChannel tLogicalChannel;
	TPeriEqpStatus	tStatus;

    if (0 == tEqp.GetEqpId() || MAXNUM_MCU_PERIEQP < tEqp.GetEqpId())
    {
        OspPrintf(TRUE, FALSE, "[PeriEqpDisconnectedNotif]TPeriEqpStatus content error! GetEqpId() = %d\n",
                                tStatus.GetEqpId());
        return;
    }

	//清除信息
	g_cMcuVcApp.SetPeriEqpConnected( tEqp.GetEqpId(), FALSE );

	//通知会控
	tStatus.SetMcuEqp( tEqp.GetMcuId(), tEqp.GetEqpId(), tEqp.GetEqpType() );
	tStatus.m_byOnline = FALSE;
	cServMsg.SetMsgBody( ( u8 * )&tStatus, sizeof( tStatus ) );
	SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );

	if( EQP_TYPE_VMP == tEqp.GetEqpType() )
	{
		//tStatus.SetNull();
		//g_cMcuVcApp.SetPeriEqpStatus( tEqp.GetEqpId(), &tStatus );
	}

	//通知所有会议 
	switch( tEqp.GetEqpType() )
	{
	case EQP_TYPE_RECORDER:
		g_cMcuVcApp.BroadcastToAllConf( MCU_RECDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;
	case EQP_TYPE_BAS:
		g_cMcuVcApp.BroadcastToAllConf( MCU_BASDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;
	case EQP_TYPE_MIXER:
		g_cMcuVcApp.BroadcastToAllConf( MCU_MIXERDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;
	case EQP_TYPE_VMP:
	    g_cMcuVcApp.BroadcastToAllConf( MCU_VMPDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;    
	case EQP_TYPE_VMPTW:
		g_cMcuVcApp.BroadcastToAllConf( MCU_VMPTWDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;
	case EQP_TYPE_PRS:
		g_cMcuVcApp.BroadcastToAllConf( MCU_PRSDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;
	case EQP_TYPE_TVWALL:
		g_cMcuVcApp.BroadcastToAllConf( MCU_TVWALLDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
		break;
	case EQP_TYPE_HDU:
		g_cMcuVcApp.BroadcastToAllConf( MCU_HDUDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
	    break;

	default:
		break;
	}
    
    // N+1模式下通知主mcu外设能力变更 [12/21/2006-zbq]
    if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_MASTER_CONNECTED)
    {
        TNPlusEqpCap tEqpCap = g_cNPlusApp.GetMcuEqpCap();
        cServMsg.SetEventId(MCU_NPLUS_EQPCAP_NOTIF);
        cServMsg.SetMsgBody((u8 *)&tEqpCap, sizeof(tEqpCap));
        g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }
}



/*------------------------------------------------------------------*/
/*                              Recorder                            */
/*------------------------------------------------------------------*/

/*====================================================================
    函数名      ：ProcMcsMcuStartRecReq
    功能        ：开始会议录像处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/13    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuStartRecReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8   byEqpType;
    u32  dwIpAddress;
    BOOL32 bRecordConf;
    
    TMt tOldRecordMt = *(TMt *)cServMsg.GetMsgBody();
    TMt tRecordMt = GetLocalMtFromOtherMcuMt(tOldRecordMt);
    
    TEqp tEqp = *(TEqp *)(cServMsg.GetMsgBody()+sizeof(TMt));
    TRecStartPara tRecPara = *(TRecStartPara *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TEqp));
    
    //将会议码率告诉录像机
    tRecPara.SetBitRate(m_tConf.GetBitRate());
    tRecPara.SetSecBitRate(GetDoubleStreamVideoBitrate(m_tConf.GetBitRate(), FALSE));
    // guzh [6/9/2007] 只支持录主格式
    tRecPara.SetIsRecMainVideo(TRUE);
    tRecPara.SetIsRecMainAudio(TRUE);

    s8 aszRecName[KDV_MAX_PATH] = {0};
    strncpy(aszRecName,
            (s8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TEqp) + sizeof(TRecStartPara)),
            KDV_MAX_PATH-1);
    
    aszRecName[KDV_MAX_PATH-1] = 0;
    
    s8 szRecFullName[KDV_MAX_PATH] = {0};
    // 拼上路径给录像机
    //sprintf(szRecFullName, "%s/%s", m_tConf.GetConfName(), aszRecName);
    // guzh [9/7/2006] 不再支持路径
    sprintf(szRecFullName, "%s", aszRecName);

    u8   byNeedPrs;
    TRecRtcpBack tRtcpBack;
    TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
    TDoublePayload tDVPayload;   //视频载荷值
    TDoublePayload tDAPayload;   //音频载荷值
    TDoublePayload tDDVPayload;  //双流视频载荷值
    u8 byConfVideoType;
    u8 byConfAudioType;
	TCapSupportEx tCapSupportEx;

    // 顾振华 [7/18/2006]  注意，其实目前只支持录主格式
    if (tRecPara.IsRecMainVideo())
    {
        byConfVideoType = m_tConf.GetMainVideoMediaType();
    }
    else
    {
        byConfVideoType = m_tConf.GetSecVideoMediaType();
    }

    if (tRecPara.IsRecMainAudio())
    {
        byConfAudioType = m_tConf.GetMainAudioMediaType();
    }
    else
    {
        byConfAudioType = m_tConf.GetSecAudioMediaType();
    }

    //判断是否是对会议录像
    bRecordConf = (tRecordMt.GetMcuId() == 0);    
	
	switch(CurState())
	{	
	case STATE_ONGOING:
        {
		//外设合法性判断
		g_cMcuAgent.GetPeriInfo(tEqp.GetEqpId(), &dwIpAddress, &byEqpType);
		
		if (byEqpType != EQP_TYPE_RECORDER || !g_cMcuVcApp.IsPeriEqpConnected(tEqp.GetEqpId()))
		{
			EqpLog( "CMcuVcInst: This MCU's recorder is invalid or offline now!\n");		
			cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
			cServMsg.SetMsgBody((u8*)&tRecordMt, sizeof(tRecordMt));
			cServMsg.CatMsgBody((u8*)&tEqp, sizeof(tEqp) );
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
		}
		
		if (bRecordConf)
		{
            //recording now, Nack			
			if (!m_tConf.m_tStatus.IsNoRecording())
			{
				McsLog( "CMcuVcInst: This conference is recording now!\n");
				cServMsg.SetErrorCode(ERR_MCU_CONFRECORDING);
				cServMsg.SetMsgBody((u8*)&tRecordMt, sizeof(tRecordMt));
				cServMsg.CatMsgBody((u8*)&tEqp, sizeof(tEqp));
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}

#ifdef _MINIMCU_
            if ( ISTRUE(m_byIsDoubleMediaConf) && tRecPara.IsRecLowStream() )
            {
                McsLog( "CMcuVcInst: This conference not support record low stream!\n");
                cServMsg.SetErrorCode(ERR_MCU_RECLOWNOSUPPORT);
                cServMsg.SetMsgBody((u8*)&tRecordMt, sizeof(tRecordMt));
                cServMsg.CatMsgBody((u8*)&tEqp, sizeof(tEqp));
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
            }
#endif
            // guzh [6/9/2007] 保存会议录像参数
            m_tRecPara = tRecPara;

            //会议录像时，对于双格式的录主格式码流，若存在动态载荷，发生格式切换时可能存在误解码？？
			if (MEDIA_TYPE_H264 == byConfVideoType || 
				MEDIA_TYPE_H263PLUS == byConfVideoType || 
				CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] 适应FEC支持其他格式
                m_tConf.GetCapSupportEx().IsVideoSupportFEC())
			{
				tDVPayload.SetRealPayLoad(byConfVideoType );
				tDVPayload.SetActivePayload(GetActivePayload(m_tConf, byConfVideoType));
			}
			else
			{
				tDVPayload.SetRealPayLoad(byConfVideoType);
				tDVPayload.SetActivePayload(byConfVideoType);
			}
			if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] 适应FEC支持其他格式
                m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
			{
				tDAPayload.SetRealPayLoad(byConfAudioType);
				tDAPayload.SetActivePayload(GetActivePayload(m_tConf, byConfAudioType));
			}
			else
			{
				tDAPayload.SetRealPayLoad(byConfAudioType);
				tDAPayload.SetActivePayload(byConfAudioType);
			}
		}
		else    // 终端录像
		{
			if (!m_tConfAllMtInfo.MtJoinedConf(tRecordMt.GetMtId()))
			{
				ConfLog(FALSE, "[ProcMcsMcuStartRecReq] This mt %d is not joined in conference.\n", tRecordMt.GetMtId() );
				cServMsg.SetErrorCode(ERR_MCU_MT_NOTINCONF);
				cServMsg.SetMsgBody((u8*)&tRecordMt, sizeof(tRecordMt));
				cServMsg.CatMsgBody((u8*)&tEqp, sizeof(tEqp));
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;			
			}

			if (!m_ptMtTable->IsMtNoRecording(tRecordMt.GetMtId()))
			{
				ConfLog(FALSE, "[ProcMcsMcuStartRecReq] This mt %d is recording now.\n", tRecordMt.GetMtId());
				cServMsg.SetErrorCode(ERR_MCU_MTRECORDING);
				cServMsg.SetMsgBody((u8*)&tRecordMt, sizeof(tRecordMt));
				cServMsg.CatMsgBody((u8*)&tEqp, sizeof(tEqp) );
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}

            // xsl [7/20/2006] 卫星分散会议时需要判断回传通道数
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                if (IsOverSatCastChnnlNum(tRecordMt.GetMtId()))
                {
                    ConfLog(FALSE, "[ProcMcsMcuStartRecReq] over max upload mt num. nack!\n");            
                    cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                    cServMsg.SetMsgBody((u8*)&tRecordMt, sizeof(tRecordMt));
				    cServMsg.CatMsgBody((u8*)&tEqp, sizeof(tEqp));
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
            }

			//动态载荷信息
			TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS(tRecordMt.GetMtId());
			if (MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() ||
				MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() ||
				CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] 适应FEC支持其他格式
                m_tConf.GetCapSupportEx().IsVideoSupportFEC() )
			{
				tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType());
				tDVPayload.SetActivePayload(GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType()));
			}
			else
			{
				tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType());
				tDVPayload.SetActivePayload(tSrcSCS.GetVideoMediaType());
			}
			if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] 适应FEC支持其他格式
                m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
			{
				tDAPayload.SetRealPayLoad(tSrcSCS.GetAudioMediaType());
				tDAPayload.SetActivePayload(GetActivePayload(m_tConf,  tSrcSCS.GetAudioMediaType()));
			}
			else
			{
				tDAPayload.SetRealPayLoad(tSrcSCS.GetAudioMediaType());
				tDAPayload.SetActivePayload(tSrcSCS.GetAudioMediaType());
			}
		}
	
        //tDDVPayload 动态载荷信息
        if (tRecPara.IsRecDStream())
        {
            u8 DStreamType = m_tConf.GetCapSupport().GetDStreamMediaType();
            
			if ( m_tConf.GetCapSupportEx().IsDDStreamCap() ) // 双双流时动态载荷设为0; jlb [2009/03/31]
			{
                tDDVPayload.SetActivePayload( 0 );
			}
			else if (MEDIA_TYPE_H264 == DStreamType ||
                MEDIA_TYPE_H263PLUS == DStreamType ||
                CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] 适应FEC支持其他格式
                m_tConf.GetCapSupportEx().IsDVideoSupportFEC())
            {
                tDDVPayload.SetActivePayload(GetActivePayload(m_tConf, DStreamType));
            }
            else
            {
                tDDVPayload.SetActivePayload(DStreamType);
            }
            tDDVPayload.SetRealPayLoad(DStreamType);
            EqpLog("DoubleStreamSrc MediaType = %d\n", DStreamType);
        }

        //prs
        ///FIXME: guzh [6/12/2007] 这里计算Prs源有漏洞，而且没有考虑广播源变化的情况，所以暂时不支持
        /*
        if (m_tConf.GetConfAttrb().IsResendLosePack())
        {
            TMt tRecSrc;
            u32 dwVideoSwitchIp;
            u32 dwAudioSwitchIp;
            u32 dwDStreamSwitchIp;
            u16 wMtSwitchPort;
            u32 dwMtSrcIp;

            if (bRecordConf)
            {                                
                //video
                IsRecordSrcBas(MODE_VIDEO, tRecSrc);
                u8 bySrcChnnl = (tRecSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
                g_cMpManager.GetSwitchInfo(tRecSrc, dwVideoSwitchIp, wMtSwitchPort, dwMtSrcIp);

                //double
                if (!m_tDoubleStreamSrc.IsNull() && m_tRecPara.IsRecDStream())
                {
                    g_cMpManager.GetSwitchInfo(m_tDoubleStreamSrc, dwDStreamSwitchIp, wMtSwitchPort, dwMtSrcIp);
                }
							
				//audio
                IsRecordSrcBas(MODE_AUDIO, tRecSrc);
                g_cMpManager.GetSwitchInfo(tRecSrc, dwAudioSwitchIp, wMtSwitchPort, dwMtSrcIp);
            }
            else
            {
                g_cMpManager.GetSwitchInfo(tRecordMt, dwVideoSwitchIp, wMtSwitchPort, dwMtSrcIp);
                dwAudioSwitchIp = dwVideoSwitchIp;
            }

            tRtcpBack.SetVideoAddr(dwVideoSwitchIp, 0);
            McsLog("[SetVideoAddr]IpAddr(0x%x), Port(%d)\n", dwVideoSwitchIp, 0);
        
            tRtcpBack.SetAudioAddr(dwAudioSwitchIp, 0);
            McsLog("[SetAudioAddr]IpAddr(0x%x), Port(%d)\n", dwAudioSwitchIp, 0);

            if (!m_tDoubleStreamSrc.IsNull() && m_tRecPara.IsRecDStream())
            {
                tRtcpBack.SetDStreamAddr(dwDStreamSwitchIp, 0);
                McsLog("[SetDStreamAddr]IpAddr(0x%x), Port(%d)\n", dwDStreamSwitchIp, 0);
            }

            byNeedPrs = 1;
        }
        else
        */
        {
            byNeedPrs = 0;
            memset(&tRtcpBack, 0, sizeof(tRtcpBack));
        }

        cServMsg.SetMsgBody((u8 *)&tOldRecordMt, sizeof(TMt));
		cServMsg.CatMsgBody((u8 *)&tEqp, sizeof(tEqp));
		cServMsg.CatMsgBody((u8 *)&tRecPara, sizeof(tRecPara));
		cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs));
		cServMsg.CatMsgBody((u8 *)&tRtcpBack, sizeof(tRtcpBack));
		cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //video encrypt
		cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload)); //video payload
		cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //audio encrypt
		cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload)); //audio payload
		cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //double video encrypt
		cServMsg.CatMsgBody((u8 *)&tDDVPayload, sizeof(tDDVPayload)); //double video payload

        // MCU前向纠错, zgc, 2007-09-28
        tCapSupportEx = m_tConf.GetCapSupportEx();
        cServMsg.CatMsgBody((u8 *)&tCapSupportEx, sizeof(tCapSupportEx));

        u16 wRecNameLen = htons(strlen(szRecFullName) + 1);
        cServMsg.CatMsgBody((u8 *)&wRecNameLen, sizeof(u16));
		cServMsg.CatMsgBody((u8 *)szRecFullName, strlen(szRecFullName) + 1);
        
        // zbq[11/12/2007] 高盛OEM录像机扩展支持
        u16 wConfInfoLen = htons(sizeof(TConfInfo));
        cServMsg.CatMsgBody((u8 *)&wConfInfoLen, sizeof(u16));
        cServMsg.CatMsgBody((u8 *)&m_tConf, sizeof(TConfInfo));

		SendMsgToEqp(tEqp.GetEqpId(), MCU_REC_STARTREC_REQ, cServMsg);
		break;
        }
	default:
		ConfLog(FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n",
			    pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
		break;
	}

}

/*====================================================================
    函数名      ：ProcMcsMcuPauseRecReq
    功能        ：暂停会议录像处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/14    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuPauseRecReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = *( TMt* )cServMsg.GetMsgBody();
	TEqp		tRecorder;
	u8			byRecChannl;
	BOOL32		bRecConf = tMt.GetMcuId() == 0;


	switch( CurState() )
	{
	case STATE_ONGOING:
	
		if( bRecConf )
		{	
			EqpLog( "CMcuVcInst: Received request to record conference now!\n" );			
			
			if( !m_tConf.m_tStatus.IsRecording() )
			{
				EqpLog( "CMcuVcInst: This conference is not recording now!\n" );
				cServMsg.SetErrorCode( ERR_MCU_CONFNOTRECORDING );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
			tRecorder = m_tRecEqp;
			byRecChannl = m_byRecChnnl;
		}
		else
		{
			EqpLog( "CMcuVcInst: Receive request to record mt%d-%d.\n", tMt.GetMcuId(), tMt.GetMtId() );
			
			if( !m_ptMtTable->IsMtRecording( tMt.GetMtId() ) )
			{
				EqpLog( "CMcuVcInst: The Mt %d is not recording !\n", tMt.GetMtId() );
				cServMsg.SetErrorCode( ERR_MCU_MTNOTRECORDING );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;				
			}	
			m_ptMtTable->GetMtRecordInfo( tMt.GetMtId(), &tRecorder, &byRecChannl );

			if( 0 == tRecorder.GetEqpId() )
			{
				ConfLog( FALSE, "CMcuVcInst: The Mt %d is use record with Eqp id = 0, check program.\n",  tMt.GetMtId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}		
		}

		//send it to recorder
		cServMsg.SetChnIndex( byRecChannl );
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof( tMt ) );
		cServMsg.CatMsgBody( (u8*)&tRecorder, sizeof( tRecorder ) );
		SendMsgToEqp( tRecorder.GetEqpId(), MCU_REC_PAUSEREC_REQ, cServMsg );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuResumeRecReq
    功能        ：恢复会议录像处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/14    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuResumeRecReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = *( TMt* )cServMsg.GetMsgBody();	
	TEqp		tRecorder;
	u8			byRecChannl;
	BOOL32		bRecConf = tMt.GetMcuId() == 0;


	switch( CurState() )
	{
	case STATE_ONGOING:
	
		if( bRecConf )
		{
			EqpLog( "CMcuVcInst: Received request to resume recording conference now!\n" );
			
			if( !m_tConf.m_tStatus.IsRecPause() )
			{
				EqpLog( "CMcuVcInst: This conference is not recording now!\n" );
				cServMsg.SetErrorCode( ERR_MCU_CONFNOTRECPAUSE );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
			tRecorder   = m_tRecEqp;
			byRecChannl = m_byRecChnnl;
		}
		else
		{
			EqpLog( "CMcuVcInst: Receive request to resume recording mt%d-%d.\n", tMt.GetMcuId(), tMt.GetMtId() );
			
			if( !m_ptMtTable->IsMtRecPause( tMt.GetMtId() ) )
			{
				EqpLog( "CMcuVcInst: The Mt %d is not recording pause !\n", tMt.GetMtId() );
				cServMsg.SetErrorCode( ERR_MCU_MTNOTRECPAUSE );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;				
			}	
			m_ptMtTable->GetMtRecordInfo( tMt.GetMtId(), &tRecorder, &byRecChannl );

			if( 0 == tRecorder.GetEqpId() )
			{
				ConfLog( FALSE, "CMcuVcInst: The Mt %d is use record with Eqp id = 0, check program.\n",  tMt.GetMtId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}		
		}

		//send it to recorder
		cServMsg.SetChnIndex( byRecChannl );
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof( tMt ) );
		cServMsg.CatMsgBody( (u8*)&tRecorder, sizeof( tRecorder ) );
		SendMsgToEqp( tRecorder.GetEqpId(), MCU_REC_RESUMEREC_REQ, cServMsg );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuStopRecReq
    功能        ：停止会议录像处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/16    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuStopRecReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = *( TMt* )cServMsg.GetMsgBody();
	TEqp		tRecorder;
	u8			byRecChannl;
	BOOL32		bRecConf = tMt.GetMcuId() == 0;


	switch( CurState() )
	{
	case STATE_ONGOING:
	
		if( bRecConf )
		{
			EqpLog( "CMcuVcInst: Received request to stop recording conference now!\n" );
			
			if( m_tConf.m_tStatus.IsNoRecording() )
			{
				EqpLog( "CMcuVcInst: This conference is not in recording state!\n" );
				cServMsg.SetErrorCode( ERR_MCU_CONFNOTRECORDING );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
			tRecorder = m_tRecEqp;
			byRecChannl = m_byRecChnnl;
		}
		else
		{
			EqpLog( "CMcuVcInst: Receive request to stop recording mt%d-%d.\n", tMt.GetMcuId(), tMt.GetMtId() );
			
			if( m_ptMtTable->IsMtNoRecording( tMt.GetMtId() ) )
			{
				EqpLog( "CMcuVcInst: The Mt %d is not recording  !\n", tMt.GetMtId() );
				cServMsg.SetErrorCode( ERR_MCU_MTNOTRECORDING );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;				
			}	
			m_ptMtTable->GetMtRecordInfo( tMt.GetMtId(), &tRecorder, &byRecChannl );

			if( 0 == tRecorder.GetEqpId() )
			{
				ConfLog( FALSE, "CMcuVcInst: The Mt %d is use record with Eqp id = 0, check program.\n",  tMt.GetMtId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}		
		}

		//send it to recorder
		cServMsg.SetChnIndex( byRecChannl );
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof( tMt ) );
		cServMsg.CatMsgBody( (u8*)&tRecorder, sizeof( tRecorder ) );
		SendMsgToEqp( tRecorder.GetEqpId(), MCU_REC_STOPREC_REQ, cServMsg );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

}

/*====================================================================
    函数名      ：ProcVcctrlMcuChangeRecModeReq
    功能        ：改变会议录像模式请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/28    1.0         JQL         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuChangeRecModeReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = *( TMt* )cServMsg.GetMsgBody();
	u8			byMode = *( u8* )( cServMsg.GetMsgBody() + sizeof( TMt ) );
	
	TEqp		tRecEqp;
	u8			byRecChannel;
	BOOL32      bRecordConf = tMt.GetMcuId() == 0;
				

	switch( CurState() )
	{
	case STATE_ONGOING:
		if( bRecordConf )
		{
			EqpLog( "CMcuVcInst: Received request to change recording mode conference now! Channel is %u\n", 
				m_byRecChnnl );

			//not recording now, Nack
			if( m_tConf.m_tStatus.IsNoRecording() )
			{
				EqpLog( "CMcuVcInst: This conference is not recording now!\n" );
				cServMsg.SetErrorCode( ERR_MCU_CONFNOTRECORDING );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
			tRecEqp = m_tRecEqp;
			byRecChannel = m_byRecChnnl;

		}
		else
		{
			EqpLog( "CMcuVcInst: Received request to change recording mode Mcu%d-Mt%d now!\n", tMt.GetMcuId(), tMt.GetMtId() );

			//not recording now, Nack
			if( m_ptMtTable->IsMtNoRecording( tMt.GetMtId() ) )
			{
				EqpLog( "CMcuVcInst: The Mt %d is not in recording state!\n", tMt.GetMtId() );
				cServMsg.SetErrorCode( ERR_MCU_MTNOTRECORDING );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}

			m_ptMtTable->GetMtRecordInfo( tMt.GetMtId(), &tRecEqp, &byRecChannel );
			if( 0 == tRecEqp.GetEqpId() )
			{
				ConfLog( FALSE, "CMcuVcInst: The Mt %d is use record with Eqp id = 0, check program.\n",  tMt.GetMtId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}		
			
		}

		cServMsg.SetChnIndex( byRecChannel );
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof( tMt ) );
		cServMsg.CatMsgBody( (u8*)&tRecEqp, sizeof( tRecEqp ) );
		cServMsg.CatMsgBody( (u8*)&byMode, sizeof( byMode ) );
		SendMsgToEqp( tRecEqp.GetEqpId(), MCU_REC_CHANGERECMODE_REQ, cServMsg );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

}

/*====================================================================
    函数名      ：ProcMcsMcuListAllRecordReq
    功能        ：查询录像机记录处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/12    3.0         胡昌威         创建
    06/08/22    4.0         顾振华         调整策略
====================================================================*/
void CMcuVcInst::ProcMcsMcuListAllRecordReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TEqp		tEqp = *( TEqp* )cServMsg.GetMsgBody();
		
	//判断外设是否合法与已连接
	if( !g_cMcuVcApp.IsPeriEqpValid( tEqp.GetEqpId() ) || 
		!g_cMcuVcApp.IsPeriEqpConnected( tEqp.GetEqpId() ) )
	{
		EqpLog( "CMcuVcInst: This MCU's recorder is invalid or offline now!\n" );
		cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//发送给录像机
    cServMsg.SetMsgBody( ( u8 * )&tEqp, sizeof( tEqp ) );

    // 把组ID打包给录像机
    u8 byUsrGrpId = CMcsSsn::GetUserGroup( cServMsg.GetSrcSsnId() );
    cServMsg.CatMsgBody( (u8*)&byUsrGrpId, sizeof(u8) );
    /*
    // 顾振华 [7/20/2006] 支持树形目录
    CConfId cConfId = cServMsg.GetConfId();
    u8 byConfMsg = 0;
    u8 byUsrGrpId = CMcsSsn::GetUserGroup( cServMsg.GetSrcSsnId() );

    if (!cConfId.IsNull())
    {
        // 会议请求
        byConfMsg = 1;
        cServMsg.CatMsgBody( (u8*)&byConfMsg, sizeof(u8) );
        // 把组ID打包给录像机
        cServMsg.CatMsgBody( (u8*)&byUsrGrpId, sizeof(u8) );

        // 如果CConfId 非Null，带上该会议的名字给录像机
        LPCSTR lpszName = NULL;
        if ( g_cMcuVcApp.GetConfNameByConfId(cConfId, lpszName) != 0)
        {
            s8 szConfName[MAXLEN_CONFNAME] = {0};
            strncpy( szConfName, lpszName, MAXLEN_CONFNAME-1 );
            cServMsg.CatMsgBody( (u8*)szConfName, MAXLEN_CONFNAME);
        }
        else
        {
            ConfLog(FALSE, "[ProcMcsMcuListAllRecordReq] Cannot find conf name for confid: ");
            cConfId.Print();
        }
        
    }
    else
    {
        // guzh [8/22/2006] 非会议请求
        byConfMsg = 0;
        cServMsg.CatMsgBody( (u8*)&byConfMsg, sizeof(u8) );
        
        // 把组ID打包给录像机
        cServMsg.CatMsgBody( (u8*)&byUsrGrpId, sizeof(u8) );

        if (byUsrGrpId == USRGRPID_SADMIN)
        {
            // 如果是超级管理员，则不需要包含任何会议名
        }
        else
        {
            // 否则整理打包本用户组的所有会议包括模版(可能为空)
            s8 aszConfNameBuf[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE][MAXLEN_CONFNAME] = {0};
            u8 byConfNum = 0;
    
            TConfMapData tMapData;
            CMcuVcInst* pcInst = NULL;
            TTemplateInfo tTemInfo;
            u8 byIndex;
            for ( byIndex = MIN_CONFIDX; byIndex <= MAX_CONFIDX; byIndex++)
            {
                tMapData = g_cMcuVcApp.GetConfMapData( byIndex );
                if (tMapData.IsValidConf())
                {
                    pcInst = g_cMcuVcApp.GetConfInstHandle(byIndex);
                    if (pcInst == NULL)
                    {
                        continue;
                    }
                    if (pcInst->m_tConf.GetUsrGrpId() == byUsrGrpId ||
                        pcInst->m_tConf.GetUsrGrpId() == USRGRPID_INVALID)  // 终端创会都能看到
                    {
                        strncpy( aszConfNameBuf[byConfNum], pcInst->m_tConf.GetConfName(), MAXLEN_CONFNAME-1 );
                        byConfNum ++;
                    }
                }
                else if (tMapData.IsTemUsed())
                {
                    g_cMcuVcApp.GetTemplate(byIndex, tTemInfo);
                    if (tTemInfo.IsEmpty())
                    {
                        continue;
                    }
                    if (tTemInfo.m_tConfInfo.GetUsrGrpId() == byUsrGrpId ||
                        tTemInfo.m_tConfInfo.GetUsrGrpId() == USRGRPID_INVALID)
                    {
                        // 检查是否已经有同名会议，若无，则添加
                        LPCSTR lpszName = tTemInfo.m_tConfInfo.GetConfName();
                        CConfId cConfId = g_cMcuVcApp.GetConfIdByName( lpszName, FALSE );
                        if (cConfId.IsNull())
                        {
                            strncpy( aszConfNameBuf[byConfNum], lpszName, MAXLEN_CONFNAME-1 );
                            byConfNum ++;
                        }
                    }
                }
            }
            for ( byIndex = 0; byIndex < byConfNum; byIndex ++ )
            {
                cServMsg.CatMsgBody( (u8*)aszConfNameBuf[byIndex], MAXLEN_CONFNAME);    
            }
        }
    }
    */
	
	SendMsgToEqp( tEqp.GetEqpId(), MCU_REC_LISTALLRECORD_REQ, cServMsg );

}

/*====================================================================
    函数名      ：ProcRecMcuListAllRecNotif
    功能        ：录像记录通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/12    3.0         胡昌威         创建
    06/07/24    4.0         顾振华         R3多级目录支持
====================================================================*/
void CMcuVcInst::ProcRecMcuListAllRecNotif( const CMessage * pcMsg )
{
    // guzh [8/22/2006] 改变策略，业务侧不在此进行过滤，而在向录像机请求时

    CServMsg	cServMsg( pcMsg->content, pcMsg->length );
    cServMsg.SetNullConfId();    // 把ConfId清空，让业务通过
    SendMsgToMcs(  cServMsg.GetSrcSsnId(), MCU_MCS_LISTALLRECORD_NOTIF, cServMsg );

/*
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
    CServMsg    newServMsg( pcMsg->content, pcMsg->length );
    
    // 如果用户指定了CConfId
    if ( !cServMsg.GetConfId().IsNull() )
    {
        // 直接发送
    }
    else
    {
        //需要根据用户所属组的信息决定本条消息所包含的会议录像是否要通知MCS
        s8 szConfName [MAXLEN_CONFNAME] = {0};

        strncpy(szConfName, 
                (s8*)(cServMsg.GetMsgBody() + sizeof(TEqp)),
                MAXLEN_CONFNAME);
        
        if (strlen(szConfName) > 0)
        {
            // 分别检查会议和模版里面
            CConfId cConfId = g_cMcuVcApp.GetConfIdByName(szConfName, FALSE);
            CConfId cTemplId = g_cMcuVcApp.GetConfIdByName(szConfName, TRUE);

            if (cConfId.IsNull() && cTemplId.IsNull())
            {
                // 如果已经没有该会议或模版，则只有admin能看到
                cConfId = g_cMcuVcApp.MakeConfId( 0, 0, USRGRPID_SADMIN );
                newServMsg.SetConfId( cConfId );
            }
            else if (!cConfId.IsNull())
            {
                newServMsg.SetConfId( cConfId );
            }
            else if (!cTemplId.IsNull())
            {
                newServMsg.SetConfId( cTemplId );
            }
        }

    }
  
    newServMsg.SetMsgBody( cServMsg.GetMsgBody(), sizeof(TEqp) );
    newServMsg.CatMsgBody( cServMsg.GetMsgBody() + sizeof(TEqp) + MAXLEN_CONFNAME, 
                           sizeof(TRecFileListNotify ) );

	SendMsgToMcs(  newServMsg.GetSrcSsnId(), MCU_MCS_LISTALLRECORD_NOTIF, newServMsg );
*/
}

/*====================================================================
    函数名      ：ProcMcsMcuDeleteRecordReq
    功能        ：处理会控到MCU的删除文件请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/09/22    4.0         libo          创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuDeleteRecordReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TEqp		tEqp = *( TEqp* )cServMsg.GetMsgBody();
		
	//判断外设是否合法与已连接
	if( !g_cMcuVcApp.IsPeriEqpValid( tEqp.GetEqpId() ) || 
		!g_cMcuVcApp.IsPeriEqpConnected( tEqp.GetEqpId() ) )
	{
		EqpLog( "CMcuVcInst: This MCU's recorder is invalid or offline now!\n" );
		cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//发送给录像机
	SendMsgToEqp( tEqp.GetEqpId(), MCU_REC_DELETERECORD_REQ, cServMsg );

}

/*====================================================================
    函数名      ：ProcMcsMcuRenameRecordReq
    功能        ：处理会控到MCU的更改文件名请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/09/22    4.0         libo          创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuRenameRecordReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp     tEqp = *(TEqp *)cServMsg.GetMsgBody();

    //判断外设是否合法与已连接
    if (!g_cMcuVcApp.IsPeriEqpValid(tEqp.GetEqpId()) || 
        !g_cMcuVcApp.IsPeriEqpConnected(tEqp.GetEqpId()))
    {
        EqpLog("CMcuVcInst: This MCU's recorder is invalid or offline now!\n");
        cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
        return;
    }

    //发送给录像机
    SendMsgToEqp(tEqp.GetEqpId(), MCU_REC_RENAMERECORD_REQ, cServMsg);
}

/*====================================================================
    函数名      ：ProcVcctrlMcuPublishRecReq
    功能        ：处理会控到MCU的录像发布处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/04    1.0         JQL         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuPublishRecReq( const CMessage * pcMsg )
{

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TEqp	*	ptEqp      = ( TEqp* )cServMsg.GetMsgBody();
		
	//判断外设是否合法与已连接
	if( !g_cMcuVcApp.IsPeriEqpValid( ptEqp->GetEqpId() ) || 
		!g_cMcuVcApp.IsPeriEqpConnected( ptEqp->GetEqpId() ) )
	{
		EqpLog( "CMcuVcInst: This MCU's recorder is invalid or offline now!\n" );
		cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	if( !g_cMcuVcApp.IsPeriEqpConnected( ptEqp->GetEqpId() ) )
	{
		ConfLog( FALSE, "CMcuVcInst: Eqp %d-%d doesnot connect yet.\n" );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	SendMsgToEqp( ptEqp->GetEqpId(), MCU_REC_PUBLISHREC_REQ, cServMsg );

}

void CMcuVcInst::ProcMcsMcuCancelPublishRecReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TEqp		tEqp = *( TEqp* )cServMsg.GetMsgBody();
		
	//判断外设是否合法与已连接
	if( !g_cMcuVcApp.IsPeriEqpValid( tEqp.GetEqpId() ) || 
		!g_cMcuVcApp.IsPeriEqpConnected( tEqp.GetEqpId() ) )
	{
		EqpLog( "CMcuVcInst: This MCU's recorder is invalid or offline now!\n" );
		cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//发送给录像机
	SendMsgToEqp( tEqp.GetEqpId(), MCU_REC_CANCELPUBLISHREC_REQ, cServMsg );
	
}

/*====================================================================
    函数名      ：ProcMcsMcuStartPlayReq
    功能        ：开始会议放像处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/09/28    4.0         libo          创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuStartPlayReq(const CMessage * pcMsg)
{
    CServMsg cTempServMsg(pcMsg->content, pcMsg->length);
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TRecProg tRecProg;

    memset(&tRecProg, 0, sizeof(TRecProg));

    u8 byIsDStreamPlay = *(u8 *)(cTempServMsg.GetMsgBody() + sizeof(TEqp));

    m_tPlayEqpAttrib.SetDStreamPlay(byIsDStreamPlay);

    cServMsg.SetMsgBody((u8 *)cTempServMsg.GetMsgBody(), sizeof(TEqp));
    cServMsg.CatMsgBody((u8 *)&tRecProg, sizeof(TRecProg));                             //开始进度

    s8 szRecName[KDV_MAX_PATH] = {0};
    //放像文件名
    strncpy(szRecName,
            (s8 *)(cTempServMsg.GetMsgBody() + sizeof(u8) + sizeof(TEqp)),
            KDV_MAX_PATH-1);    
    szRecName[KDV_MAX_PATH-1] = 0;
    
    s8 szRecFullName[KDV_MAX_PATH] = {0};
    // 拼上路径给录像机
    // guzh [8/29/2006] mcs 自己带路径
    //sprintf(szRecFullName, "%s/%s", m_tConf.GetConfName(), szRecName);
    sprintf(szRecFullName, "%s",  szRecName);

    cServMsg.CatMsgBody( (u8*)szRecFullName, strlen(szRecFullName)+1 );


    StartPlayReq(cServMsg);
}

/*====================================================================
    函数名      ：StartPlayReq
    功能        ：开始会议放像处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：CServMsg &cServMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/09/28    4.0         libo          创建
====================================================================*/
void CMcuVcInst::StartPlayReq(CServMsg &cServMsg)
{
    CServMsg cTempMsg;
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();

    TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
    u8 byConfVideoType = m_tConf.GetMainVideoMediaType();
    u8 byConfAudioType = m_tConf.GetMainAudioMediaType();

	// 双流格式
	u8 byDStreamMediaType = m_tConf.GetCapSupport().GetDStreamMediaType();

    TDoublePayload tDVPayload, tSndDVPayload;
    TDoublePayload tDAPayload, tSndDAPayload;

    tSndDVPayload.SetRealPayLoad(m_tConf.GetSecVideoMediaType());
    tSndDAPayload.SetRealPayLoad(m_tConf.GetSecAudioMediaType());

	TCapSupportEx tCapSupportEx;
	u16 wFileInfoLen = 0;
	
    TCapSupport tCapSupport;
    switch (CurState())
    {
    case STATE_ONGOING:
        //强制发言人时不允许放像
        /*        
        if (m_tConf.m_tStatus.IsMustSeeSpeaker())
        {
            ConfLog(FALSE, "CMcuVcInst: Must See Speaker, can't play recorder!\n" );
            cServMsg.SetErrorCode(ERR_MUSTSEESPEAKER_NOTPLAY);
            cServMsg.SetMsgBody((u8*)&tEqp, sizeof(TEqp));
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }*/

        //判断外设是否合法与已连接
        if (!g_cMcuVcApp.IsPeriEqpValid(tEqp.GetEqpId()) || 
            !g_cMcuVcApp.IsPeriEqpConnected(tEqp.GetEqpId()))
        {
            ConfLog(FALSE, "CMcuVcInst: This MCU's recorder is invalid or offline now!\n");
            cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            cServMsg.SetMsgBody((u8*)&tEqp, sizeof(TEqp));
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        //正在录像,NACK
        if (!m_tConf.m_tStatus.IsNoPlaying())
        {
            ConfLog(FALSE, "CMcuVcInst: This conference is playing now!\n");
            cServMsg.SetErrorCode( ERR_MCU_CONFPLAYING);
            cServMsg.SetMsgBody((u8*)&tEqp, sizeof(TEqp));
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        //发往录像机
        cTempMsg.SetMsgBody(cServMsg.GetMsgBody() + sizeof(TEqp),
                            cServMsg.GetMsgBodyLen() - sizeof(TEqp));
        tEqp.SetMcuEqp( (u8)LOCAL_MCUID, tEqp.GetEqpId(), EQP_TYPE_RECORDER);

        if (MEDIA_TYPE_H264 == byConfVideoType || 
            MEDIA_TYPE_H263PLUS == byConfVideoType || 
            CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
            // zbq [10/29/2007] 适应FEC支持其他格式
            m_tConf.GetCapSupportEx().IsVideoSupportFEC())
        {
            tDVPayload.SetRealPayLoad(byConfVideoType);
            tDVPayload.SetActivePayload(GetActivePayload(m_tConf, byConfVideoType));
        }
        else
        {
            tDVPayload.SetRealPayLoad(byConfVideoType);
            tDVPayload.SetActivePayload(byConfVideoType);
        }

        if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
            // zbq [10/29/2007] 适应FEC支持其他格式
            m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
        {
            tDAPayload.SetRealPayLoad(byConfAudioType);
            tDAPayload.SetActivePayload(GetActivePayload(m_tConf, byConfAudioType));
        }
        else
        {
            tDAPayload.SetRealPayLoad(byConfAudioType);
            tDAPayload.SetActivePayload(byConfAudioType);
        }

        //文件格式可能与主格式并不相同，需rplib对其进行格式判别，不符合则拒绝!!!
        cServMsg.SetMsgBody((u8 *)&tEqp, sizeof(tEqp));
        cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //video
        cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload)); //video
        cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //audio
        cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload)); //audio
        cServMsg.CatMsgBody((u8 *)&tSndDVPayload, sizeof(tSndDVPayload));
        cServMsg.CatMsgBody((u8 *)&tSndDAPayload, sizeof(tSndDAPayload));
		cServMsg.CatMsgBody(&byDStreamMediaType, sizeof(byDStreamMediaType)); //DStream, zgc, 2007-08-23

        // MCU前向纠错, zgc, 2007-09-28
        tCapSupportEx = m_tConf.GetCapSupportEx();
        cServMsg.CatMsgBody((u8*)&tCapSupportEx, sizeof(tCapSupportEx));
		
		wFileInfoLen = cTempMsg.GetMsgBodyLen();
		cServMsg.CatMsgBody((u8*)&wFileInfoLen, sizeof(u16));
        cServMsg.CatMsgBody(cTempMsg.GetMsgBody(), cTempMsg.GetMsgBodyLen()); //开始进度＋放像文件名
        tCapSupport = m_tConf.GetCapSupport();
		cServMsg.CatMsgBody((u8*)&tCapSupport, sizeof(TCapSupport));//增加绑定通知会议能力集

        SendMsgToEqp(tEqp.GetEqpId(), MCU_REC_STARTPLAY_REQ, cServMsg);
        break;

    default:
        //ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
        //	pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;
    }
}

/*====================================================================
    函数名      ：ProcMcsMcuPausePlayReq
    功能        ：暂停会议放像处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/14    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuPausePlayReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:
		EqpLog( "CMcuVcInst: Received request to pause playing conference now! Channel is %u\n", 
			m_byPlayChnnl );

		//not playing now, Nack
		if( !m_tConf.m_tStatus.IsPlaying() )
		{
			EqpLog( "CMcuVcInst: This conference is not playing now!\n" );
			cServMsg.SetErrorCode( ERR_MCU_CONFNOTPLAYING );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}

		//send it to recorder
		cServMsg.SetMsgBody( ( u8 * )&m_tPlayEqp, sizeof( m_tPlayEqp ) );	//set TEqp
		cServMsg.SetChnIndex( m_byPlayChnnl );
		SendMsgToEqp( m_tPlayEqp.GetEqpId(), MCU_REC_PAUSEPLAY_REQ, cServMsg );


		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuResumePlayReq
    功能        ：恢复会议放像处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/14    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuResumePlayReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:
		EqpLog( "CMcuVcInst: Received request to resume recording conference now! Channel is %u\n", 
			m_byPlayChnnl );

		//not pausing now, Nack
		if( !m_tConf.m_tStatus.IsPlayPause() )
		{
			EqpLog( "CMcuVcInst: This conference is not play paused now!\n" );
			cServMsg.SetErrorCode( ERR_MCU_CONFNOTPLAYPAUSE );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}

		//send it to recorder
		cServMsg.SetMsgBody( ( u8 * )&m_tPlayEqp, sizeof( m_tPlayEqp ) );	//set TEqp
		cServMsg.SetChnIndex( m_byPlayChnnl );
		SendMsgToEqp( m_tPlayEqp.GetEqpId(), MCU_REC_RESUMEPLAY_REQ, cServMsg );


		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}



/*====================================================================
    函数名      ：ProcMcsMcuStopPlayReq
    功能        ：停止会议放像处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/16    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuStopPlayReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:
		EqpLog( "CMcuVcInst: Received request to stop playing conference now! Channel is %u\n", 
			m_byPlayChnnl );

		//not playing now, Nack
		if( m_tConf.m_tStatus.IsNoPlaying() )
		{
			EqpLog( "CMcuVcInst: This conference is not playing now!\n" );
			cServMsg.SetErrorCode( ERR_MCU_CONFNOTPLAYING );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}

		//send it to recorder
		cServMsg.SetMsgBody( ( u8 * )&m_tPlayEqp, sizeof( m_tPlayEqp ) );	//set TEqp
		cServMsg.SetChnIndex( m_byPlayChnnl );
		SendMsgToEqp( m_tPlayEqp.GetEqpId(), MCU_REC_STOPPLAY_REQ, cServMsg );

		//会议状态改变和改变发言人收到信道改变通知后再处理！！！
        // 清放像文件格式记录, zgc, 2008-08-28
        m_tPlayFileMediaInfo.clear();

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*=============================================================================
  函 数 名： ProcMcsMcuGetRecPlayProgCmd
  功    能： 会控查询录放相进度
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::ProcMcsMcuGetRecPlayProgCmd( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    switch( CurState() )
    {
    case STATE_ONGOING:
        {
            //获取录相进度
            if (MCS_MCU_GETRECPROG_CMD == pcMsg->event)
            {
                TMt tMt = *(TMt *)cServMsg.GetMsgBody();
                TEqp tRecEqp;
                u8  byRecChnnl;
                
                //会议录相
                if(tMt.IsNull())
                {
                    if (m_tConf.m_tStatus.IsNoRecording())
                    {
                        return;
                    }

                    tRecEqp = m_tRecEqp;
                    byRecChnnl = m_byRecChnnl;
                }
                //终端录相
                else
                {
                    if (m_ptMtTable->IsMtNoRecording(tMt.GetMtId()))
                    {
                        return;
                    }

                    if(!m_ptMtTable->GetMtRecordInfo(tMt.GetMtId(), &tRecEqp, &byRecChnnl))
                    {
                        return;
                    }
                }               

                cServMsg.SetChnIndex(byRecChnnl);
                SendMsgToEqp(tRecEqp.GetEqpId(), MCU_REC_GETRECPROG_CMD, cServMsg);
            }
            //获取放相进度
            else
            {
                if (m_tConf.m_tStatus.IsNoPlaying())
                {
                    return;
                }                
                cServMsg.SetChnIndex( m_byPlayChnnl );
                SendMsgToEqp( m_tPlayEqp.GetEqpId(), MCU_REC_GETPLAYPROG_CMD, cServMsg );
            }
        }
        break;

    default:
        ConfLog( FALSE, "[ProcMcsMcuGetRecPlayProgCmd] Wrong message %u(%s) received in state %u!\n", 
                pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;
    }

    return;
}

/*====================================================================
    函数名      ：ProcMcsMcuSeekReq
    功能        ：会议放像调整处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/20    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuSeekReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length ), cTempMsg;
	u16		wEv;

	switch( CurState() )
	{
	case STATE_ONGOING:
		switch( pcMsg->event )
		{
		case MCS_MCU_FFPLAY_REQ:
			EqpLog( "CMcuVcInst: Received request to fast forward now! Channel is %u\n", 
				m_byPlayChnnl );
			wEv = MCU_REC_FFPLAY_REQ;
			break;
		case MCS_MCU_FBPLAY_REQ:
			EqpLog( "CMcuVcInst: Received request to fast backward now! Channel is %u\n", 
				m_byPlayChnnl );
			wEv = MCU_REC_FBPLAY_REQ;
			break;
		case MCS_MCU_SEEK_REQ:
			EqpLog( "CMcuVcInst: Received request to seek now! Channel is %u\n", 
				m_byPlayChnnl );
			wEv = MCU_REC_SEEK_REQ;
			break;
		default:
			break;
		}

		//not playing now, Nack
		if( m_tConf.m_tStatus.IsNoPlaying() )
		{
			EqpLog( "CMcuVcInst: This conference is not playing now!\n" );
			cServMsg.SetErrorCode( ERR_MCU_CONFNOTPLAYING );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}

		//send it to recorder
		cTempMsg.SetMsgBody( cServMsg.GetMsgBody() + sizeof( TEqp ), cServMsg.GetMsgBodyLen() - sizeof( TEqp ) );
		cServMsg.SetMsgBody( ( u8 * )&m_tPlayEqp, sizeof( m_tPlayEqp ) );	//set TEqp
		cServMsg.CatMsgBody( cTempMsg.GetMsgBody(), cTempMsg.GetMsgBodyLen() );	//set record name
		cServMsg.SetChnIndex( m_byPlayChnnl );
		SendMsgToEqp( m_tPlayEqp.GetEqpId(), wEv, cServMsg );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*=============================================================================
函 数 名： ProcRecPlayWaitMpAckTimer
功    能： 等待放像交换建立定时器超时
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/3/26   4.0			周广程                  创建
=============================================================================*/
void CMcuVcInst::ProcRecPlayWaitMpAckTimer(const CMessage * pcMsg)
{
	KillTimer( MCUVC_RECPLAY_WAITMPACK_TIMER );
	CServMsg cServMsg;
	cServMsg.SetChnIndex( m_byPlayChnnl );
	m_tConf.m_tStatus.SetPlaying();
	SendMsgToEqp( m_tPlayEqp.GetEqpId() , MCU_EQP_SWITCHSTART_NOTIF, cServMsg);
}

/*====================================================================
    函数名      ：DaemonProcRecMcuChnnlStatusNotif
    功能        ：录像机信道状态通知处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/18    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::DaemonProcRecMcuChnnlStatusNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp *ptEqp;
    TRecChnnlStatus *ptChnnlStatus;
    CConfId cConfId = cServMsg.GetConfId();
    u8 byChnnl = cServMsg.GetChnIndex();
    TPeriEqpStatus tRecStatus;

    if (pcMsg->event == REC_MCU_RECORDCHNSTATUS_NOTIF)
    {
        ptEqp = (TEqp *)(cServMsg.GetMsgBody() + sizeof(TMt));
        ptChnnlStatus = (TRecChnnlStatus *)(cServMsg.GetMsgBody() + sizeof(TEqp) + sizeof(TMt));	
    }
    else
    {
        ptEqp = (TEqp *)cServMsg.GetMsgBody();
        ptChnnlStatus = (TRecChnnlStatus *)(cServMsg.GetMsgBody() + sizeof(TEqp));
    }

    //save to periequipment status
    if (!g_cMcuVcApp.GetPeriEqpStatus(ptEqp->GetEqpId(), &tRecStatus))
    {
        return;
    }
    tRecStatus.m_tStatus.tRecorder.SetChnnlStatus(byChnnl, ptChnnlStatus->m_byType, ptChnnlStatus);
    g_cMcuVcApp.SetPeriEqpStatus(ptEqp->GetEqpId(), &tRecStatus);

    //analyze conference id and forward it to conference
    if (0 != g_cMcuVcApp.GetConfIdx(cConfId))
    {
        g_cMcuVcApp.SendMsgToConf(cConfId, pcMsg->event, pcMsg->content, pcMsg->length);
    }
}

/*====================================================================
    函数名      ：DaemonProcRecMcuProgNotif
    功能        ：录像机信道进度通知处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/20    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::DaemonProcRecMcuProgNotif( const CMessage * pcMsg )
{
    CServMsg	cServMsg( pcMsg->content, pcMsg->length);
    u8          * pbyMsgBody  =  cServMsg.GetMsgBody();   
    TMt         * ptMt  = NULL;
    TEqp		* ptEqp = NULL;
    TRecProg	* ptRecProg = NULL;
    TRecChnnlStatus		tChnnlStatus;
    CConfId		cConfId = cServMsg.GetConfId();
    u8			byChnnl = cServMsg.GetChnIndex(), byChnnlType;
    TPeriEqpStatus	tRecStatus;

    //save to periequipment status
    if (pcMsg->event == REC_MCU_PLAYPROG_NOTIF)
    {
        byChnnlType = TRecChnnlStatus::TYPE_PLAY;
    }
    else
    {
        byChnnlType = TRecChnnlStatus::TYPE_RECORD;

        ptMt = (TMt*)pbyMsgBody;
        pbyMsgBody  += sizeof(TMt);
    }

    ptEqp       = (TEqp*)pbyMsgBody;
    pbyMsgBody += sizeof(TEqp);

    ptRecProg   = (TRecProg *)pbyMsgBody;

    if (!g_cMcuVcApp.GetPeriEqpStatus(ptEqp->GetEqpId(), &tRecStatus))
    {
        return;
    }

    if (!tRecStatus.m_tStatus.tRecorder.GetChnnlStatus(byChnnl, byChnnlType, &tChnnlStatus))
    {
        return;
    }
    tChnnlStatus.m_tProg = *ptRecProg;
    tRecStatus.m_tStatus.tRecorder.SetChnnlStatus(byChnnl, byChnnlType, &tChnnlStatus);
    g_cMcuVcApp.SetPeriEqpStatus(ptEqp->GetEqpId(), &tRecStatus);

	//analyze conference id and forward it to conference
    if (0 != g_cMcuVcApp.GetConfIdx(cConfId))
    {
        g_cMcuVcApp.SendMsgToConf(cConfId, pcMsg->event, pcMsg->content, pcMsg->length);
    }
}


/*====================================================================
    函数名      ：ProcRecMcuPlayChnnlStatusNotif
    功能        ：录像机放像信道改变处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/19    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcRecMcuPlayChnnlStatusNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TEqp		*ptEqp = ( TEqp * )cServMsg.GetMsgBody();
	TRecChnnlStatus		* ptChnnlStatus = ( TRecChnnlStatus * )( cServMsg.GetMsgBody() + sizeof( TEqp ) );
	CConfId		cConfId = cServMsg.GetConfId();
	u8			byChnnl = cServMsg.GetChnIndex();

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		switch( ptChnnlStatus->m_byState )
		{
		case TRecChnnlStatus::STATE_PLAYREADY:	//停止放像
            //change speaker if it's recorder
            if( m_tConf.GetSpeaker() == *ptEqp )
            {
                //是否为语音激励控制发言，是则恢复
                if (m_tConf.m_tStatus.IsVACing() && !m_tVacLastSpeaker.IsNull())
                {
                    EqpLog("Vac speaker restored, speaker is: Mt%d\n",
                                   m_tVacLastSpeaker.GetMtId());

                    ChangeSpeaker(&m_tVacLastSpeaker);
                }
                else
                {
                    ChangeSpeaker(NULL);
                }

				// xliang [8/19/2008] modify A:此部分逻辑移出该if外，见"modify B"处，
				//放在此处会引起如下bug：即通过拖发言人而触发停放像的情景下，前面这个if是进不来的
				//这就会导致如果是双流放像的话，暂停后拖发言人会停不掉双流
// 				 if (TYPE_MCUPERI == m_tDoubleStreamSrc.GetType())
// 				 {
// 					 StopDoubleStream(FALSE, TRUE);
// 				 }
            }

			// xliang [8/19/2008] modify B 改进判断，更加严格
			//if (TYPE_MCUPERI == m_tDoubleStreamSrc.GetType())
			if(m_tDoubleStreamSrc == *ptEqp) 
			{
				StopDoubleStream(FALSE, TRUE);
			}
			//change status
			m_byPlayChnnl = 0;
			memset( &m_tPlayEqp, 0, sizeof( m_tPlayEqp ) );
            m_tPlayEqpAttrib.Reset();
			m_tConf.m_tStatus.SetNoPlaying();

			break;
		case TRecChnnlStatus::STATE_PLAYING:		//开始放像或恢复放像
		
			//change speaker if it's not speaker now			
//			if( m_tConf.m_tStatus.IsNoPlaying() )
//			{
//				m_byPlayChnnl = byChnnl;
//				m_tPlayEqp = *ptEqp;
//				m_tPlayEqp.SetConfIdx( m_byConfIdx );
//				ChangeSpeaker( &m_tPlayEqp );
//			}
			m_tConf.m_tStatus.SetPlaying();

			break;
		// zgc, 2007-02-27, 会议放像关键帧延迟问题
		case TRecChnnlStatus::STATE_PLAYREADYPLAY:	//准备开始放像
//			m_tConf.m_tStatus.SetPlayReady();
			break;
		// zgc, 2007-02-27, end
			
		case TRecChnnlStatus::STATE_PLAYPAUSE:		//暂停放像
			//change status
			m_tConf.m_tStatus.SetPlayPause();
			break;
		case TRecChnnlStatus::STATE_FF:				//快进
			//change status
			m_tConf.m_tStatus.SetPlayFF();
			break;
		case TRecChnnlStatus::STATE_FB:				//快退
			//change status
			m_tConf.m_tStatus.SetPlayFB();
			break;
		default:
			ConfLog( FALSE, "CMcuVcInst: Wrong channel state %u!\n", ptChnnlStatus->m_byState );
			break;
		}

		ConfModeChange();
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcRecMcuRecChnnlStatusNotif
    功能        ：录像机录像信道改变处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/19    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcRecMcuRecChnnlStatusNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TMt      tRecMt = *(TMt *)cServMsg.GetMsgBody();
    TEqp    *ptEqp  = (TEqp *)(cServMsg.GetMsgBody() + sizeof(TMt));
    TRecChnnlStatus *ptChnnlStatus = (TRecChnnlStatus *)(ptEqp + 1);
    CConfId  cConfId = cServMsg.GetConfId();
    u8       byChnnl = cServMsg.GetChnIndex();
    u8       bySrcChnnl = 0;
    BOOL32   bRecordConf = (tRecMt.GetMcuId() == 0);

	switch (CurState())
	{
	case STATE_ONGOING:
		
		if ( bRecordConf )
		{			
			switch( ptChnnlStatus->m_byState )
			{
			case TRecChnnlStatus::STATE_RECREADY:	//停止录像
            {            
				//stop switch	
				StopSwitchToPeriEqp( m_tRecEqp.GetEqpId(), byChnnl, FALSE );
                // guzh [8/30/2006] 停双流录像
                if ( m_tRecPara.IsRecDStream() && !m_tDoubleStreamSrc.IsNull())
                {
                    StopSwitchToPeriEqp( m_tRecEqp.GetEqpId(), byChnnl, FALSE, MODE_SECVIDEO );
                }

				//change status
				m_byRecChnnl = 0;
				memset( &m_tRecEqp, 0, sizeof( m_tRecEqp ) );
                TRecStartPara tOldRecParam = m_tRecPara;
                m_tRecPara.Reset();
				m_tConf.m_tStatus.SetNoRecording();

                //录低速码流，则向上调整码率
                if (tOldRecParam.IsRecLowStream() && m_tConf.m_tStatus.IsBrAdapting())
                {
                    // guzh [8/29/2007] 向上调整为码率适配（需要计算主格式的最低ReqBitrate）
                    ChangeAdapt(ADAPT_TYPE_BR, GetLeastMtReqBitrate(TRUE, m_tConf.GetMainVideoMediaType()));
                }

                // guzh [6/13/2007]
                TSimCapSet tDstSCS;
                TSimCapSet tSrcSCS;
                u16 wAdaptBitrate;
                if (  m_tConf.m_tStatus.IsVidAdapting() && 
                     (IsNeedVidAdapt(tDstSCS, tSrcSCS,wAdaptBitrate) || 
                      IsNeedCifAdp())
                   )
                {
                    ChangeAdapt(ADAPT_TYPE_VID, wAdaptBitrate, &tDstSCS, &tSrcSCS);
                }
                
                /*
                // zbq [06/07/2007] 无条件恢复一次码率
                if (m_tRecPara.IsRecLowStream() && m_tConf.m_tStatus.IsVidAdapting())
                {
                    u8 byConfMainVType = m_tConf.GetMainVideoMediaType();
                    u8 byConfSecVType  = m_tConf.GetMainAudioMediaType();

                    TSimCapSet tSSim;
                    TMt tSpeaker = GetLocalSpeaker();

                    if (tSpeaker.GetMtType() == MT_TYPE_MT)
                    {
                        tSSim = m_ptMtTable->GetDstSCS(GetLocalSpeaker().GetMtId());
                    }
                    else // 外设统统认为是主格式 FIXME: 录像机双流放象 ?
                    {
                        tSSim.SetVideoMediaType(byConfMainVType);
                    }

                    BOOL32 bPrimary = tSSim.GetVideoMediaType() == byConfMainVType ? TRUE : FALSE;
                    u8 byAdaptDstType = bPrimary ? byConfSecVType : byConfMainVType;
                    
                    ChangeAdapt(ADAPT_TYPE_VID, GetLeastMtReqBitrate(bPrimary, byAdaptDstType));
                }     
                */
            }
			break;
			case TRecChnnlStatus::STATE_RECORDING:	//开始或恢复录像

                //保存参数
                m_byRecChnnl = byChnnl;
                m_tRecEqp = *ptEqp;

                //change status
                if (ptChnnlStatus->m_byRecMode == REC_MODE_SKIPFRAME)
                {
                    m_tConf.m_tStatus.m_tConfMode.SetRecSkipFrame(TRUE);
                }
                else
                {
                    m_tConf.m_tStatus.m_tConfMode.SetRecSkipFrame(FALSE);
                }

				//恢复录像时不需重建交换
				if (m_tConf.m_tStatus.IsNoRecording())
				{
					//开始录像
					//图像
                    AdjustRecordSrcStream( MODE_VIDEO );
                    //语音
                    AdjustRecordSrcStream( MODE_AUDIO );
                    //双流
                    AdjustRecordSrcStream( MODE_SECVIDEO );
                    //请求双流源关键帧
                    if ( m_tRecPara.IsRecDStream() && 
                         !m_tDoubleStreamSrc.IsNull() && m_tDoubleStreamSrc.GetType() == TYPE_MT )
                    {
                        /*
                        if (m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE))
                        {
                            tDStreamAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                        }
                        g_cMpManager.GetSwitchInfo(m_tDoubleStreamSrc, dwDStreamSwitchIp, wMtSwitchPort, dwMtSrcIp);
                        */

                        NotifyFastUpdate(m_tDoubleStreamSrc, MODE_SECVIDEO);
                    }
                    
                    ///FIXME: guzh [6/12/2007] 这里计算Prs源没有考虑广播源变化的情况，暂时不支持
                    // 请注意恢复时下面代码需要修改 tRecSrc
                    /*
                    TMt tRecSrc;                    
                    u32 dwVideoSwitchIp;
                    u32 dwAudioSwitchIp;
                    u32 dwDStreamSwitchIp;
                    u16 wMtSwitchPort;
                    u32 dwMtSrcIp;
                    TTransportAddr tVideoAddr;
                    TTransportAddr tAudioAddr;
                    TTransportAddr tDStreamAddr;
                    TLogicalChannel tLogicalChannel;

                    //视频RTCP
                    // 如果是终端
                    if (tRecSrc.GetType() == TYPE_MT)
                    {                                                        
                        if (m_ptMtTable->GetMtLogicChnnl(tRecSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
                        {
                            tVideoAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                        }
                    }
                    // 否则就是外设(VMP, Player or Bas)
                    else if (tRecSrc.GetEqpType() == EQP_TYPE_BAS ||
                             tRecSrc.GetEqpType() == EQP_TYPE_VMP)
                    {
                        // 如果是从Bas/Vmp
                        u8 byChlNum;
                        g_cMcuVcApp.GetPeriEqpLogicChnnl(tRecSrc.GetEqpId(), MODE_VIDEO, &byChlNum, &tLogicalChannel, FALSE);
                        tVideoAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                    } 

                    if(!g_cMpManager.GetSwitchInfo(tRecSrc, dwVideoSwitchIp, wMtSwitchPort, dwMtSrcIp))
                    {
                        ConfLog(FALSE, "[ProcRecMcuRecChnnlStatusNotif] GetSwitchInfo for video src failed. tRecSrc<%d,%d>\n",
                                        tRecSrc.GetMcuId(), tRecSrc.GetMtId());
                        // 这里不需要return
                    }

                    //音频RTCP
                    if (tRecSrc.GetType() == TYPE_MT)
                    {
                        // 如果从终端交换
                        if (m_ptMtTable->GetMtLogicChnnl(tRecSrc.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE))
                        {
                            tAudioAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                        }                        
                    }
                    else if (tRecSrc.GetEqpType() == EQP_TYPE_BAS)
                    {
                        // 如果从适配器交换, FIXME: 混音器交换出来的不须PRS ?
                        u8 byChlNum;
                        g_cMcuVcApp.GetPeriEqpLogicChnnl(tRecSrc.GetEqpId(), MODE_AUDIO, &byChlNum, &tLogicalChannel, FALSE);
                        tAudioAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                    }
                    g_cMpManager.GetSwitchInfo(tRecSrc, dwAudioSwitchIp, wMtSwitchPort, dwMtSrcIp);
                    
                    //prs
                    if (m_tConf.GetConfAttrb().IsResendLosePack())
                    {
                        u8  byRecorderId = ptEqp->GetEqpId();
                        u32 dwSwitchIpAddr;
                        u16 wSwitchPort;
                        g_cMpManager.GetRecorderSwitchAddr(byRecorderId, dwSwitchIpAddr, wSwitchPort);

                        wSwitchPort = wSwitchPort + PORTSPAN * byChnnl;

                        //Video
                        g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwVideoSwitchIp, 0,
                                                         dwVideoSwitchIp, wSwitchPort + 6,
                                                         tVideoAddr.GetIpAddr(), tVideoAddr.GetPort());
                        //Audio
                        g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwAudioSwitchIp, 0,
                                                         dwAudioSwitchIp, wSwitchPort + 7,
                                                         tAudioAddr.GetIpAddr(), tAudioAddr.GetPort());

                        //DStream
                        if (!m_tDoubleStreamSrc.IsNull() && m_tRecPara.IsRecDStream())
                        {
                            g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwDStreamSwitchIp, 0,
                                                             dwDStreamSwitchIp, wSwitchPort + 8,
                                                             tDStreamAddr.GetIpAddr(), tDStreamAddr.GetPort());
                        }
                    }
                    */
				}
                else
                {
                    //恢复录象向终端请求关键帧, zgc, 2008-07-17
                    TMt tSrc;
                    u8  byRecChnIdx = 0;
                    BOOL32 bNeedBas = IsRecordSrcBas(MODE_VIDEO, tSrc, byRecChnIdx);
                    if ( !tSrc.IsNull() && tSrc.GetType() == TYPE_MT)
                    {                                   
                        NotifyFastUpdate(tSrc, MODE_VIDEO);
                    }
                    if ( m_tRecPara.IsRecDStream() && 
                        !m_tDoubleStreamSrc.IsNull() && 
                        m_tDoubleStreamSrc.GetType() == TYPE_MT )
                    {
                        NotifyFastUpdate(m_tDoubleStreamSrc, MODE_SECVIDEO);
                    }
                }

				m_tConf.m_tStatus.SetRecording();
				break;
			
			case TRecChnnlStatus::STATE_RECPAUSE:	//暂停录像	
				//change status
				m_tConf.m_tStatus.SetRecPause();
				break;	
			}
			ConfModeChange();	

		}
        //终端录像
		else
		{
            TMt tSMcuMt = tRecMt;
            tRecMt = GetLocalMtFromOtherMcuMt(tRecMt);
            
            switch (ptChnnlStatus->m_byState)
            {
            case TRecChnnlStatus::STATE_RECREADY:	//停止录像
                
                //停交换
                if (m_tConfAllMtInfo.MtJoinedConf(tRecMt.GetMtId()))
                {
                    StopSwitchToPeriEqp(ptEqp->GetEqpId(), byChnnl, FALSE);
                }
                //置状态
                m_ptMtTable->SetMtNoRecording(tRecMt.GetMtId());		
                break;

            case TRecChnnlStatus::STATE_RECORDING:	//开始或恢复录像
                
                //开始录像，设置终端状态，向录像机发送码流
                if (m_ptMtTable->IsMtNoRecording(tRecMt.GetMtId()))
                {		
                    //被录像终端已不在会议，停止终端录像
                    if (!m_tConfAllMtInfo.MtJoinedConf(tRecMt.GetMtId()))
                    {
                        CServMsg cServMsg;
                        cServMsg.SetConfId(m_tConf.GetConfId());
                        cServMsg.SetMsgBody((u8 *)ptEqp, sizeof(*ptEqp));	//set TEqp
                        cServMsg.SetChnIndex(byChnnl);
                        SendMsgToEqp(ptEqp->GetEqpId(), MCU_REC_STOPREC_REQ, cServMsg);
                    }
                    StartSwitchToPeriEqp(tRecMt, 0, ptEqp->GetEqpId(), byChnnl, MODE_BOTH);

                    //建立交换桥 [01/25/2007-zbq]
                    // 这里的通道号应该填写源的通道号, zgc, 2008-07-02
                    g_cMpManager.SetSwitchBridge(tRecMt, 0/*byChnnl*/, MODE_BOTH);

                    if (m_tConf.GetConfAttrb().IsResendLosePack())
                    {
                        u8  byRecorderId = ptEqp->GetEqpId();
                        u32 dwSwitchIpAddr;
                        u16 wSwitchPort;
                        u32 dwMtSwitchIp;
                        u16 wMtSwitchPort;
                        u32 dwMtSrcIp;
                        g_cMpManager.GetRecorderSwitchAddr(byRecorderId, dwSwitchIpAddr, wSwitchPort);

                        g_cMpManager.GetSwitchInfo(tRecMt, dwMtSwitchIp, wMtSwitchPort, dwMtSrcIp);

                        wSwitchPort = wSwitchPort + PORTSPAN*byChnnl;

                        TTransportAddr  tAddr;
                        TLogicalChannel tLogicalChannel;

                        //Video
                        if (m_ptMtTable->GetMtLogicChnnl(tRecMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
                        {
                            tAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                            g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwMtSwitchIp, 0,
                                                             dwMtSwitchIp, wSwitchPort + 6,
                                                             tAddr.GetIpAddr(), tAddr.GetPort());
                        }

                        //Audio
                        if (m_ptMtTable->GetMtLogicChnnl(tRecMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE))
                        {
                            tAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                            g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwMtSwitchIp, 0,
                                                             dwMtSwitchIp, wSwitchPort + 7,
                                                             tAddr.GetIpAddr(), tAddr.GetPort());
                        }

                        /*/DStream
                        if (!m_tDoubleStreamSrc.IsNull() &&
                            m_ptMtTable->GetMtLogicChnnl(tRecMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE))
                        {
                            tAddr = tLogicalChannel.GetSndMediaCtrlChannel();
                            g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwMtSwitchIp, 0,
                                                             dwMtSwitchIp, wSwitchPort + 8,
                                                             tAddr.GetIpAddr(), tAddr.GetPort());
                        }*/
                    }
                    if ( tSMcuMt.IsLocal() )
                    {
                        //通知本级终端开始发送
                        NotifyMtSend(tRecMt.GetMtId());
                    }
                    else
                    {
                        //请求下级终端回传码流
                        OnMMcuSetIn( tSMcuMt, 0, SWITCH_MODE_SELECT );
                    }
				}
                else
                {
                    //恢复录象向终端请求关键帧, zgc, 2008-07-17                    
                    if ( tRecMt.GetType() == TYPE_MT)
                    {                                   
                        NotifyFastUpdate(tRecMt, MODE_VIDEO);
                    }
                    if ( m_tRecPara.IsRecDStream() && 
                        !m_tDoubleStreamSrc.IsNull() && 
                        m_tDoubleStreamSrc.GetType() == TYPE_MT )
                    {
                        NotifyFastUpdate(m_tDoubleStreamSrc, MODE_SECVIDEO);
                    }
                }

                //重置为录像状态
                m_ptMtTable->SetMtRecording(tRecMt.GetMtId(), *ptEqp, byChnnl);

                if (ptChnnlStatus->m_byRecMode == REC_MODE_SKIPFRAME)
                {
                    m_ptMtTable->SetMtRecSkipFrame(tRecMt.GetMtId(), TRUE);
                }
                else
                {
                    m_ptMtTable->SetMtRecSkipFrame(tRecMt.GetMtId(), FALSE);
                }
				break;

			case TRecChnnlStatus::STATE_RECPAUSE:	//暂停录像	
				
                //置状态
				m_ptMtTable->SetMtRecPause( tRecMt.GetMtId() );
				break;

			default:
				ConfLog( FALSE, "Unexcept State Recevied In RecordMtNotify.\n" );
				return;
			}

            TMtStatus tMtStatus;
            m_ptMtTable->GetMtStatus(tRecMt.GetMtId(), &tMtStatus);

            MtStatusChange(tRecMt.GetMtId(), TRUE);

			if( tMtStatus.m_tRecState.IsNoRecording() )
			{
				EqpLog( "Mt %d rec state change, new state: IsNoRecording!\n", tRecMt.GetMtId() );
			}
			else if( tMtStatus.m_tRecState.IsRecording() )
			{
				EqpLog( "Mt %d rec state change, new state: IsRecording!\n", tRecMt.GetMtId());
			}
			else if( tMtStatus.m_tRecState.IsRecPause() )
			{
				EqpLog( "Mt %d rec state change, new state: IsRecPause!\n", tRecMt.GetMtId());
			}
			else if( tMtStatus.m_tRecState.IsRecSkipFrame() )
			{
				EqpLog( "Mt %d rec state change, new state: IsRecSkipFrame!\n",tRecMt.GetMtId());
			}

		}
		break;		

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcRecMcuProgNotif
    功能        ：录像机进度通知处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/20    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcRecMcuProgNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TRecProg	* ptRecProg = NULL;
    TEqp        tEqp;
    TMt         tMt;
	u16         wMcEvent = 0;
    
	switch( CurState() )
	{
	case STATE_ONGOING:
		if( pcMsg->event == REC_MCU_PLAYPROG_NOTIF )
		{
			ptRecProg = ( TRecProg * )( cServMsg.GetMsgBody() + sizeof( TEqp ) );
			m_tConf.m_tStatus.m_tPlayProg = *ptRecProg;
			wMcEvent = MCU_MCS_PLAYPROG_NOTIF;
		}
		else
		{
		    tMt  = *( TMt* )cServMsg.GetMsgBody();
            tEqp = *( TMt* )( cServMsg.GetMsgBody() + sizeof( TMt ) );
			ptRecProg = ( TRecProg * )( cServMsg.GetMsgBody() + sizeof( TMt ) + sizeof( TEqp ) );
			if( tMt.GetMcuId() == 0 )
			{
				m_tConf.m_tStatus.m_tRecProg = *ptRecProg;				
			}
			else
			{
                if ( !tMt.IsLocal() )
                {
                    tMt = GetLocalMtFromOtherMcuMt(tMt);

                    //重新构造 cServMsg
                    cServMsg.SetMsgBody( (u8*)&tMt,  sizeof(TMt) );
                    cServMsg.CatMsgBody( (u8*)&tEqp, sizeof(TEqp) );
                    cServMsg.CatMsgBody( (u8*)ptRecProg, sizeof(TRecProg) );
                }
                m_ptMtTable->SetMtRecProg( tMt.GetMtId() , *ptRecProg );
			}
			wMcEvent = MCU_MCS_RECPROG_NOTIF;
		}

		///////////////////
		if( pcMsg->event == REC_MCU_PLAYPROG_NOTIF )
		{
			m_tConf.m_tStatus.m_tPlayProg = *ptRecProg;
		}
		else
		{
			m_tConf.m_tStatus.m_tRecProg = *ptRecProg;
		}

    	SendMsgToAllMcs( wMcEvent, cServMsg );
		break;
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			             pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ProcRecMCUNeedIFrameCmd
    功能        ：录像机请求关键帧
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/12/18    4.0         江乐斌          创建
====================================================================*/
void CMcuVcInst::ProcRecMcuNeedIFrameCmd(const CMessage * pcMsg)
{	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TMt tSrcMt;
    TEqp tBasSrc;
    u8 byChnId = 0;

    switch(CurState())
    {
    case STATE_ONGOING:

        switch( pcMsg->event ) 
        {			
        case REC_MCU_NEEDIFRAME_CMD:
			// xliang [3/6/2009] 增加向VMP请求关键帧，当VMP和speaker并存时，录像录VMP的图像
			if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE
				&& m_tConf.m_tStatus.IsBrdstVMP())
			{
				tSrcMt = m_tVmpEqp;
			
				u8	byMtMediaType = m_tConf.GetMainVideoMediaType();
				u8	byMtRes = m_tConf.GetMainVideoFormat();
		 		byChnId = GetVmpOutChnnlByRes(byMtRes, m_tVmpEqp.GetEqpId(), byMtMediaType);

				NotifyEqpFastUpdate(tSrcMt, byChnId);
			}
            else if (IsRecordSrcBas(MODE_VIDEO, tBasSrc, byChnId))
            {
                //录像机会定时请求三次，此处不定时
                NotifyEqpFastUpdate(tBasSrc, byChnId, FALSE);
            }
            else if(m_tConf.HasSpeaker())
            {
                tSrcMt = GetLocalSpeaker();
                NotifyFastUpdate(tSrcMt, MODE_VIDEO);
            }
            else
            {
                ConfLog(FALSE, "[ProcRecMCUNeedIFrameCmd] Err: No VMP mode or speaker for REC_MCU_NEEDIFRAME_CMD\n");
            }
            break;
        default:
			ConfLog(FALSE, "[ProcRecMCUNeedIFrameCmd]:The Message type %u(%s) is not fit", pcMsg->event, ::OspEventDesc(pcMsg->event) );
            break;
        }
        break;

    default:
        ConfLog(FALSE, "[ProcRecMCUNeedIFrameCmd]Wrong message %u(%s) received in state %u!\n", 
                        pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
    }
	return;
}

/*====================================================================
    函数名      ：ProcMcuRecConnectedNotif
    功能        ：录像机上电通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/09/28    4.0         libo          创建
====================================================================*/
void CMcuVcInst::ProcMcuRecConnectedNotif(const CMessage * pcMsg)
{
    if (STATE_ONGOING != CurState())
    {
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TEqp tEqp;
    TPeriEqpStatus tRecStatus;
    TRecChnnlStatus tRecChanStatus;
    TPeriEqpRegReq tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();

    
    if (tRegReq.GetEqpId() != m_tPlayEqp.GetEqpId() ||
        EQP_CHANNO_INVALID == m_byPlayChnnl)
    {
        /*
        OspPrintf(TRUE, FALSE, "tRegReq.GetEqpId().%d != m_tPlayEqp.GetEqpId().%d\n",
                               tRegReq.GetEqpId(), m_tPlayEqp.GetEqpId());
        */
        return;
    }

    // 放像恢复
    tEqp.SetMcuEqp(tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());

    g_cMcuVcApp.GetPeriEqpStatus(tRegReq.GetEqpId(), &tRecStatus);

    tRecStatus.m_tStatus.tRecorder.GetChnnlStatus(m_byPlayChnnl,
                                                  TRecChnnlStatus::TYPE_PLAY,
                                                  &tRecChanStatus);

    cServMsg.SetMsgBody((u8 *)&tEqp, sizeof(TEqp));
    cServMsg.CatMsgBody((u8 *)&tRecChanStatus.m_tProg, sizeof(TRecProg));
    cServMsg.CatMsgBody((u8 *)tRecChanStatus.GetRecordName(),
                        strlen(tRecChanStatus.GetRecordName()));

    StartPlayReq(cServMsg);

    return;
}

/*====================================================================
    函数名      ：ProcMcuRecDisconnectedNotif
    功能        ：录像机断链处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/26    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMcuRecDisconnectedNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8      byMtId;
	TEqp	tEqp = *( TEqp * )cServMsg.GetMsgBody();

	switch( CurState() )
	{
	case STATE_ONGOING:

		//停止会议录像交换
		if( !m_tConf.m_tStatus.IsNoRecording() && (m_tRecEqp == tEqp) )
        {
			StopSwitchToPeriEqp( m_tRecEqp.GetEqpId() ,m_byRecChnnl);
			m_byRecChnnl = 0;
			memset( &m_tRecEqp, 0, sizeof( m_tRecEqp ) );
			m_tConf.m_tStatus.SetNoRecording();
			ConfModeChange( );
            log( LOGLVL_IMPORTANT, "CMcuVcInst: Conferece %s recording state cancelled due to MCU%u's VOD%u disconnected!\n", 
                m_tConf.GetConfName(), tEqp.GetMcuId(), tEqp.GetEqpId() );
        }

		cServMsg.SetConfId( m_tConf.GetConfId() );

		//判断是否是发言外设
		if( !m_tConf.m_tStatus.IsNoPlaying() && (m_tPlayEqp==tEqp) )
		{
			//change status
			//m_byPlayChnnl = 0;
			//memset( &m_tPlayEqp, 0, sizeof( m_tPlayEqp ) );
			m_tConf.m_tStatus.SetNoPlaying();
			ConfModeChange();
            
            if (TYPE_MCUPERI      == m_tDoubleStreamSrc.GetType() &&
                EQP_TYPE_RECORDER == m_tDoubleStreamSrc.GetEqpType())
            {
                StopDoubleStream(FALSE, TRUE);
            }

			//change speaker
			ChangeSpeaker( NULL );
		}

		//终端录像
		for( byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
		{				
			
			if( m_tConfAllMtInfo.MtJoinedConf( byMtId ) && !m_ptMtTable->IsMtNoRecording( byMtId ) )
			{
				TEqp tRecEqp;
				u8   byRecChn;
				TMtStatus tMtStatus;

				m_ptMtTable->GetMtRecordInfo( byMtId ,&tRecEqp,&byRecChn );

                //终端录像的录像机是断链的录像机
                if ( tRecEqp == tEqp )
                {
                    StopSwitchToPeriEqp( tRecEqp.GetEqpId(),byRecChn);                    
                    m_ptMtTable->SetMtNoRecording(byMtId);
                    MtStatusChange(byMtId, TRUE);
                }				
			}
		}
			
		break;

	case STATE_SCHEDULED:
		break;
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcRecMcuResp
    功能        ：处理录像机给MCU的回应消息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/09/23    4.0         libo          创建
====================================================================*/
void CMcuVcInst::ProcRecMcuResp(const CMessage * pcMsg)
{
    if (STATE_ONGOING != CurState())
    {
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();
	CConfId	cConfId = cServMsg.GetConfId();
	u8 byChnnl = cServMsg.GetChnIndex();

    switch(pcMsg->event)
    {
    case REC_MCU_STARTPLAY_ACK:
        {
            TPlayFileAttrib tPlayFileAttrib;
            tPlayFileAttrib = *(TPlayFileAttrib *)(cServMsg.GetMsgBody() + sizeof(TEqp));
            m_tPlayEqpAttrib.SetFileType(tPlayFileAttrib.GetFileType());
            m_tPlayEqpAttrib.SetAudioType(tPlayFileAttrib.GetAudioType());
            m_tPlayEqpAttrib.SetVideoType(tPlayFileAttrib.GetVideoType());
            m_tPlayEqpAttrib.SetDVideoType(tPlayFileAttrib.GetDVideoType());            
            
            
            // zgc, 2008-08-28, 记录放像文件媒体信息
            if ( cServMsg.GetMsgBodyLen() > sizeof(TEqp) + sizeof(TPlayFileAttrib) )
            {
                m_tPlayFileMediaInfo = *(TPlayFileMediaInfo *)( cServMsg.GetMsgBody() + sizeof(TEqp) + sizeof(TPlayFileAttrib) );
                u8 byType = 0;
                u16 wWidth = 0;
                u16 wHeight = 0;
                m_tPlayFileMediaInfo.GetVideo( byType, wWidth, wHeight );
                EqpLog( "[ProcRecMcuResp]PlayFileMediaInfo: Type.%d, Width.%d, Height.%\n", 
                            byType, wWidth, wHeight );
            }
            else
            {
                EqpLog( "[ProcRecMcuResp] Receive old version recorder REC_MCU_STARTPLAY_ACK\n" );
            }
            
			if (m_tPlayEqpAttrib.GetDVideoType() == m_tConf.GetDStreamMediaType()
				 ||( m_tConf.GetCapSupportEx().IsDDStreamCap()
				     && ( MEDIA_TYPE_H264 == m_tPlayEqpAttrib.GetDVideoType() 
					      || MEDIA_TYPE_H263PLUS == m_tPlayEqpAttrib.GetDVideoType()
						 )
				   )
			   )
            {
                if (m_tPlayEqpAttrib.IsDStreamPlay() && m_tPlayEqpAttrib.IsDStreamFile())
                {
                    TLogicalChannel tLogicChnnl;
                    tLogicChnnl.SetChannelType(m_tPlayEqpAttrib.GetDVideoType());
                    tLogicChnnl.SetSupportH239(m_tConf.GetCapSupport().IsDStreamSupportH239());

                    tEqp.SetConfIdx(m_byConfIdx);
                    StartDoubleStream(tEqp, tLogicChnnl);
                }
            }
			//连续播放刷新交换 临时支持 稍后加入标识恢复状态机完备性			
			//change speaker if it's not speaker now			
			//if( m_tConf.m_tStatus.IsNoPlaying() )
			{
				m_byPlayChnnl = byChnnl;
				m_tPlayEqp = tEqp;
				m_tPlayEqp.SetConfIdx( m_byConfIdx );
				ChangeSpeaker( &m_tPlayEqp );
                
                //zbq [09/05/2007] 录像机双流放象，此处才可以建桥
                if ( m_tDoubleStreamSrc == tEqp )
                {
                    g_cMpManager.SetSwitchBridge(tEqp, byChnnl, MODE_SECVIDEO);
                }

				// 开启等待交换建立定时器, zgc, 2008-03-27
				SetTimer( MCUVC_RECPLAY_WAITMPACK_TIMER, 4*1000 );
			}
        }
	
		//SendMsgToEqp( m_tPlayEqp.GetEqpId() , MCU_EQP_SWITCHSTART_NOTIF, cServMsg);
        //给mcs开始放相的ack
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
    	break;
    case REC_MCU_STARTREC_ACK:
    case REC_MCU_STOPREC_ACK:
        {
            // guzh [9/8/2006] 开始/停止录像成功，需要分析错误码
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

            u16 wError = cServMsg.GetErrorCode();
            if (wError != 0)
            {
                NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), wError );
            }
        }
        break;
    default:
        break;
    }


    return;
}

/*------------------------------------------------------------------*/
/*                                 BAS                              */
/*------------------------------------------------------------------*/

/*====================================================================
    函数名      : ProcBasChnnlStatusNotif
    功能        ：码流适配器通道状态通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/18    3.0         zmy           Create
====================================================================*/
void CMcuVcInst::ProcBasChnnlStatusNotif(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8  byChnIdx;
    u8  byEqpId;
    u8 *pbyMsgBody;
    TBasChnStatus *ptChnStatus;
    TPeriEqpStatus tEqpStatus;
    CConfId cConfId = cServMsg.GetConfId();

    byChnIdx    = cServMsg.GetChnIndex();
    pbyMsgBody  = (u8 *)cServMsg.GetMsgBody();
    byEqpId     = *pbyMsgBody;
    pbyMsgBody += sizeof(u8);
    ptChnStatus = (TBasChnStatus*)pbyMsgBody;

    //save to periequipment status
    if (!g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tEqpStatus))
    {
        return;
    }

    // xsl [9/29/2006] reserved字段为mcu上层概念
    BOOL32 bReserved = tEqpStatus.m_tStatus.tBas.tChnnl[byChnIdx].IsReserved();
    tEqpStatus.m_tStatus.tBas.tChnnl[byChnIdx] = *ptChnStatus;
    tEqpStatus.m_tStatus.tBas.tChnnl[byChnIdx].SetReserved(bReserved);

    EqpLog("[Bas ChanIdx.%d]GetChannelType() = %d\n", byChnIdx, ptChnStatus->GetChannelType());

    g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tEqpStatus);

    //analyze conference id and forward it to conference
    cServMsg.SetMsgBody((u8 *)&tEqpStatus, sizeof(tEqpStatus));
    SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
    
    return;
}

/*=============================================================================
 函 数 名： DaemonProcHDBasChnnlStatusNotif 
 功    能： 
 算法实现： 
 全局变量： 
 参    数： const CMessage * pcMsg
 返 回 值： void 
 ----------------------------------------------------------------------
 修改记录    ：
 日  期		版本		修改人		走读人    修改内容
 2008/8/8   4.0		    周广程                  创建
 2009/5/2   4.6		    张宝卿                  兼容mpu
=============================================================================*/
void CMcuVcInst::DaemonProcHDBasChnnlStatusNotif( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    CConfId cConfId = cServMsg.GetConfId();
    u8  byChnIdx = cServMsg.GetChnIndex();
    u8  byChnType = *(u8*)cServMsg.GetMsgBody();;
    
    THDBasVidChnStatus tVidChnStatus;
    memset(&tVidChnStatus, 0, sizeof(tVidChnStatus));
    memcpy( &tVidChnStatus, cServMsg.GetMsgBody() + sizeof(u8), sizeof(THDBasVidChnStatus) );
    
    THDBasChnStatus tChnStatus = (THDBasChnStatus)tVidChnStatus;
    u8 byEqpId = tVidChnStatus.GetEqpId();
    u8 byStatus = tVidChnStatus.GetStatus();

    TPeriEqpStatus tEqpStatus;
    g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tEqpStatus );

    switch (byChnType)
    {
    case HDBAS_CHNTYPE_MAU_MV:
        {
            THDBasVidChnStatus tMVStatus = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus();
            BOOL32 bIsReserved = tMVStatus.IsReserved();
            BOOL32 bIsTmpReserved = tMVStatus.IsTempReserved();
            
            tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tVidChnStatus);
            
            if ( bIsReserved )
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->SetIsReserved( TRUE );
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tMVStatus);
            }
            else if ( bIsTmpReserved )
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->SetIsTempReserved( TRUE );
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tMVStatus);
            }
            else
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->UnReserved();
            }
            g_cMcuVcApp.SetPeriEqpStatus( byEqpId, &tEqpStatus );
			g_cMcuVcApp.SendPeriEqpStatusToMcs(byEqpId);
        }
    	break;

    case HDBAS_CHNTYPE_MAU_DS:
        {
            THDBasDVidChnStatus tDSStatus = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus();
            BOOL32 bIsReserved = tDSStatus.IsReserved();
            BOOL32 bIsTmpReserved = tDSStatus.IsTempReserved();
            
            tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tVidChnStatus);
            
            if ( bIsReserved )
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->SetIsReserved( TRUE );
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tDSStatus);
            }
            else if ( bIsTmpReserved )
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->SetIsTempReserved( TRUE );
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tDSStatus);
            }
            else
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->UnReserved();
            }
            g_cMcuVcApp.SetPeriEqpStatus( byEqpId, &tEqpStatus );
			g_cMcuVcApp.SendPeriEqpStatusToMcs(byEqpId);
        }
        break;

    case HDBAS_CHNTYPE_MPU:
        {
            if (byChnIdx >= MAXNUM_MPU_CHN)
            {
                break;
            }
            THDBasVidChnStatus tMVStatus = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx);
            BOOL32 bIsReserved = tMVStatus.IsReserved();
            BOOL32 bIsTmpReserved = tMVStatus.IsTempReserved();
            
            tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tVidChnStatus, byChnIdx);
            
            if ( bIsReserved )
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->SetIsReserved( TRUE );
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tMVStatus, byChnIdx);
            }
            else if ( bIsTmpReserved )
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->SetIsTempReserved( TRUE );
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tMVStatus, byChnIdx);
            }
            else
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->UnReserved();
            }
            g_cMcuVcApp.SetPeriEqpStatus( byEqpId, &tEqpStatus );
			g_cMcuVcApp.SendPeriEqpStatusToMcs(byEqpId);
        }
        break;
    default:
        ConfLog( FALSE, "[DaemonProcHDBasChnnlStatusNotif] unexpected chnType.%d!\n", byChnType );
        break;
    }

    //转发给所有会议
    if ( tChnStatus.GetStatus() == THDBasChnStatus::READY )
    {
        g_cMcuVcApp.BroadcastToAllConf(pcMsg->event, pcMsg->content, pcMsg->length);
    }

    EqpLog( "[DaemonProcHDBasChnnlStatusNotif] EqpId.%d, ChnIdx.%d, ChnType.%d, ChnStatus.%d\n" ,
             byEqpId, byChnIdx, byChnType, byStatus );
    return;
}

/*=============================================================================
    函 数 名： DaemonProcHduMcuChnnlStatusNotif
    功    能： 
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/1/13   4.6		    张宝卿                  创建
=============================================================================*/
void CMcuVcInst::DaemonProcHduMcuChnnlStatusNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp tEqp;
    u8 byChnnl;
	u8 bFirstNotif;
	THduChnStatus *ptChnnlStatus;
    CConfId cConfId = cServMsg.GetConfId();
    TPeriEqpStatus tHduStatus;

    u8 bOldMute = FALSE;
	u8 byOldVolume = 0;
    ptChnnlStatus = (THduChnStatus *)cServMsg.GetMsgBody();	
	bFirstNotif = *(u8*)(cServMsg.GetMsgBody() + sizeof(THduChnStatus));

	tEqp = ptChnnlStatus->GetEqp();
    byChnnl = ptChnnlStatus->GetChnIdx();

    if (!g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus))
    {
		EqpLog("[DaemonProcHduMcuChnnlStatusNotif]:GetPeriEqpStatus failure!\n");
        return;
    }
	if ( bFirstNotif )
	{
        bOldMute = tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnnl].GetIsMute();
		byOldVolume = tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnnl].GetVolume();
		memcpy((void*)&tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnnl], (void*)ptChnnlStatus, sizeof(THduChnStatus));
	    tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnnl].SetIsMute( bOldMute );
		tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnnl].SetVolume( byOldVolume );
		THduVolumeInfo tHduVolumeInfo;
		tHduVolumeInfo.SetIsMute( bOldMute );
		tHduVolumeInfo.SetVolume( byOldVolume );
		tHduVolumeInfo.SetChnlIdx( byChnnl );
		cServMsg.SetMsgBody( (u8*)&tHduVolumeInfo, sizeof(tHduVolumeInfo) );
		SendMsgToEqp( tEqp.GetEqpId(), MCU_HDU_CHANGEVOLUME_CMD, cServMsg );
	}
	else
	{
		memcpy((void*)&tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnnl], (void*)ptChnnlStatus, sizeof(THduChnStatus));
	}

	g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);
	g_cMcuVcApp.BroadcastToAllConf( pcMsg->event, pcMsg->content, pcMsg->length );

    return;
}

/*====================================================================
    函数名      ：ProcBasMcuRsp
    功能        ：码率适配器开始适配应答处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/26    1.0         LI Yi         创建
	03/11/25    3.0         zmy           Modify
	04/03/22    3.0         胡昌威        修改
====================================================================*/
//modify bas 2
void CMcuVcInst::ProcBasMcuRsp(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TPeriEqpStatus tStatus;
    TConfMtInfo tConfMtInfo;
    TSimCapSet tSimCapSet;
    //TEqp *ptEqp;
    TMt tAdaptSrc;
    u8 bySrcChnnl;
    u8 byBasMode;
    u8 byAdaptType;
    u8 byMediaMode;

    u8 byEqpChlIdx = cServMsg.GetChnIndex();
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();

    tAdaptSrc.SetNull();

    //确定是哪种适配器的应答
    if (tEqp.GetEqpId() == m_tVidBasEqp.GetEqpId() && byEqpChlIdx == m_byVidBasChnnl && m_tConf.m_tStatus.IsVidAdapting() )
    {
        byMediaMode = MODE_VIDEO;
        byAdaptType = ADAPT_TYPE_VID;
        byBasMode = CONF_BASMODE_VID;
        tEqp = m_tVidBasEqp;
        tAdaptSrc = m_tVidBrdSrc;
    }
    else if (tEqp.GetEqpId() == m_tBrBasEqp.GetEqpId() && byEqpChlIdx == m_byBrBasChnnl  && m_tConf.m_tStatus.IsBrAdapting())
    {
        byMediaMode = MODE_VIDEO;
        byAdaptType = ADAPT_TYPE_BR;
        byBasMode = CONF_BASMODE_BR;
        tEqp = m_tBrBasEqp;
        tAdaptSrc = m_tVidBrdSrc;
    }
    else if (tEqp.GetEqpId() == m_tAudBasEqp.GetEqpId() && byEqpChlIdx == m_byAudBasChnnl && m_tConf.m_tStatus.IsAudAdapting() )
    {
        byMediaMode = MODE_AUDIO;
        byAdaptType = ADAPT_TYPE_AUD;
        byBasMode = CONF_BASMODE_AUD;
        tEqp = m_tAudBasEqp;
        tAdaptSrc = m_tAudBrdSrc;
    }
    else if (tEqp.GetEqpId() == m_tCasdVidBasEqp.GetEqpId() && byEqpChlIdx == m_byCasdVidBasChnnl && m_tConf.m_tStatus.IsCasdVidAdapting() )
    {
        byMediaMode = MODE_VIDEO;
        byAdaptType = ADAPT_TYPE_CASDVID;
        byBasMode = CONF_BASMODE_CASDVID;
        tEqp = m_tCasdVidBasEqp;
        tAdaptSrc = m_tVidBrdSrc;
    }
    else if (tEqp.GetEqpId() == m_tCasdAudBasEqp.GetEqpId() && byEqpChlIdx == m_byCasdAudBasChnnl && m_tConf.m_tStatus.IsCasdAudAdapting() )
    {
        byMediaMode = MODE_AUDIO;
        byAdaptType = ADAPT_TYPE_CASDAUD;
        byBasMode = CONF_BASMODE_CASDAUD;
        tEqp = m_tCasdAudBasEqp;
        tAdaptSrc = m_tAudBrdSrc;
    }
    else
    {
        ConfLog(FALSE, "Unexpected %d(%s) received or audseladp, EqpId: %d!\n",
                pcMsg->event, OspEventDesc(pcMsg->event), tEqp.GetEqpId());
        return;
    }

    switch (pcMsg->event)
    {
    case BAS_MCU_STARTADAPT_ACK:

        //取消适配器应答定时
        KillTimer(MCUVC_WAITBASRRSP_TIMER + byAdaptType - 1);
        EqpLog("[BAS_MCU_STARTADAPT_ACK]KillTimer(%d)\n", MCUVC_WAITBASRRSP_TIMER + byAdaptType - 1);

        if (ADAPT_TYPE_CASDAUD == byAdaptType || ADAPT_TYPE_CASDVID == byAdaptType)
        {
            return;
        }

        //码流适配
        if (tAdaptSrc.IsNull())
        {
            ConfLog(FALSE, "Start media type adapt but has no video broadcast source!\n");
        }
        else
        {
            //图像交换到码流适配器
            if (ADAPT_TYPE_VID == byAdaptType)
            {
                bySrcChnnl = (tAdaptSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
                StartSwitchToPeriEqp(tAdaptSrc, bySrcChnnl, tEqp.GetEqpId(), byEqpChlIdx, MODE_VIDEO);
            }

            //图像交换到码流适配器
            if (ADAPT_TYPE_BR == byAdaptType)
            {
                bySrcChnnl = (tAdaptSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
                StartSwitchToPeriEqp(tAdaptSrc, bySrcChnnl, tEqp.GetEqpId(), byEqpChlIdx, MODE_VIDEO);
            }

            //语音交换到码流适配器
            if (ADAPT_TYPE_AUD == byAdaptType)
            {
                bySrcChnnl = (tAdaptSrc == m_tMixEqp) ? m_byMixGrpId : 0;
                StartSwitchToPeriEqp(tAdaptSrc, bySrcChnnl, tEqp.GetEqpId(), byEqpChlIdx, MODE_AUDIO);
            }

            //byMediaMode = MODE_VIDEO;

            //建立适配器的交换桥
            g_cMpManager.SetSwitchBridge(tEqp, byEqpChlIdx, byMediaMode);
            
            bySrcChnnl = (tAdaptSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
            
            //适配码流交换到prs
            if (m_tConf.GetConfAttrb().IsResendLosePack())
            {               
                if (ADAPT_TYPE_AUD == byAdaptType)
                {
                    StartSwitchToPeriEqp(tEqp, byEqpChlIdx, m_tPrsEqp.GetEqpId(), m_byPrsChnnlAudBas, MODE_AUDIO);

                    ChangePrsSrc(m_tAudBrdSrc, PRSCHANMODE_AUDBAS, TRUE);                    
                }
                else if (ADAPT_TYPE_VID == byAdaptType)
                {
                    StartSwitchToPeriEqp(tEqp, byEqpChlIdx, m_tPrsEqp.GetEqpId(), m_byPrsChnnlVidBas, MODE_VIDEO);

                    ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_VIDBAS, TRUE);                    
                }
                else if (ADAPT_TYPE_BR == byAdaptType)
                {
                    StartSwitchToPeriEqp(tEqp, byEqpChlIdx, m_tPrsEqp.GetEqpId(), m_byPrsChnnlBrBas, MODE_VIDEO);

                    ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_BRBAS, TRUE);                                    }               
            }

            //适配码流交换到各终端            
            StartSwitchToAllSubMtJoinedConfNeedAdapt(tAdaptSrc, bySrcChnnl, byMediaMode, byAdaptType);	
            
            if (ADAPT_TYPE_BR == byAdaptType)
            {
                //组播
                if (m_tConf.GetConfAttrb().IsMulticastMode() &&
                    m_tConf.GetConfAttrb().IsMulcastLowStream())
                {
                    // guzh [8/29/2007] 不支持双媒体会议低速组播
/*
                    // guzh [8/29/2007] 非双媒体会议，从BrBas组播
#ifdef _MINIMCU_
                    if ( !ISTRUE(m_byIsDoubleMediaConf) )
#endif                    
*/
                        g_cMpManager.StartMulticast(tEqp, byEqpChlIdx, byMediaMode);	
                }                

                // 会议录像需要经过BrBas
                TEqp tRecSrc;
                u8 byRecChnlIdx = 0;
                if (!m_tConf.m_tStatus.IsNoRecording() && 
                    IsRecordSrcBas(MODE_VIDEO, tRecSrc, byRecChnlIdx) &&
                    tEqp == tRecSrc )
                {
                    StartSwitchToPeriEqp(tEqp, byEqpChlIdx, m_tRecEqp.GetEqpId(), m_byRecChnnl, byMediaMode);
                }
            }

            else if (ADAPT_TYPE_VID == byAdaptType)
            {
                // 会议录像需要经过VidBas
                TEqp tRecSrc;
                u8   byRecChnlIdx = 0;
                if (!m_tConf.m_tStatus.IsNoRecording() &&
                    IsRecordSrcBas(MODE_VIDEO, tRecSrc, byRecChnlIdx) &&
                    tEqp == tRecSrc )
                {
                    StartSwitchToPeriEqp(tEqp, byEqpChlIdx, m_tRecEqp.GetEqpId(), m_byRecChnnl, byMediaMode);
                }
                // guzh [8/29/2007] 不支持双媒体会议低速组播
/*
#ifdef _MINIMCU_
                // guzh [8/29/2007] 双媒体会议，从VidBas组播
                if ( ISTRUE(m_byIsDoubleMediaConf) )
                {
                    g_cMpManager.StartMulticast(tEqp, byEqpChlIdx, byMediaMode);
                }
#endif                        
*/
            }
            else if (ADAPT_TYPE_AUD == byAdaptType)
            {
                // 会议录像需要经过AudBas
                TEqp tRecSrc;
                u8   byRecChnlIdx = 0;
                if (!m_tConf.m_tStatus.IsNoRecording() &&
                    IsRecordSrcBas(MODE_AUDIO, tRecSrc, byRecChnlIdx) &&
                    tEqp == tRecSrc)
                {
                    StartSwitchToPeriEqp(tEqp, byEqpChlIdx, m_tRecEqp.GetEqpId(), m_byRecChnnl, byMediaMode);
                }
            }    
            // 顾振华@2006.4.12 卫星组播暂时不支持双速会议
        }
        break;

    //释放通道要考虑
    case BAS_MCU_STARTADAPT_NACK:
        
        KillTimer(MCUVC_WAITBASRRSP_TIMER + byAdaptType - 1);
        EqpLog("[BAS_MCU_STARTADAPT_NACK]KillTimer(%d), adapttype :%d\n", 
                MCUVC_WAITBASRRSP_TIMER + byAdaptType - 1, byAdaptType);

        g_cMcuVcApp.SetBasChanStatus(tEqp.GetEqpId(), byEqpChlIdx, TBasChnStatus::READY);

        //改变会议适配状态
        m_tConf.m_tStatus.SetAdaptMode(byBasMode, FALSE);
		ConfModeChange();
        break;

    default:
        break;
    }
}


/*====================================================================
    函数名      ：ProcMcuBasConnectedNotif
    功能        ：码率适配登记处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/26    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMcuBasConnectedNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TPeriEqpRegReq tEqp = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();

    switch(CurState())
    {
    case STATE_ONGOING:

        if (EQP_TYPE_BAS == tEqp.GetEqpType() && m_tConf.GetConfAttrb().IsUseAdapter())
        {
            u16 wAdaptBitRate = 0;
            TSimCapSet tSrcSimCapSet;
            TSimCapSet tDstSimCapSet;

            if (!m_tVidBrdSrc.IsNull())
            {
                //标清恢复先，高清稍后
                if ( IsHDConf( m_tConf ) )
                {
                }
                else
                {
                    if (tEqp == m_tVidBasEqp && !m_tConf.m_tStatus.IsVidAdapting() &&
                        IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
                    {
                        StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }
                    else if(tEqp == m_tVidBasEqp && !m_tConf.m_tStatus.IsVidAdapting()
                        && IsNeedCifAdp() )
                    {
                        IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate);
                        StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }

                    if (tEqp == m_tBrBasEqp && !m_tConf.m_tStatus.IsBrAdapting() &&
                        IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
                    {
                        StartAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);	
                    }
                
                    //zbq[04/08/2008] BAS重连上来，尝试恢复视频相关交换：彻底重刷一次. 下同.
                    if ( (tEqp == m_tVidBasEqp &&
                          (IsNeedCifAdp() ||
                           IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate)))
                         ||
                         (tEqp == m_tBrBasEqp &&
                          IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate)) )
                    {
                        TMt tVidSrc = m_tVidBrdSrc;
                        ChangeVidBrdSrc(&tVidSrc);
                    }
                }
            }

            if (!m_tAudBrdSrc.IsNull())
            {
                if (tEqp == m_tAudBasEqp && !m_tConf.m_tStatus.IsAudAdapting() &&
                    IsNeedAudAdapt(tDstSimCapSet, tSrcSimCapSet))
                {
                    StartAdapt(ADAPT_TYPE_AUD, 0, &tDstSimCapSet, &tSrcSimCapSet);

                    //zbq[04/08/2008] BAS重连上来，尝试恢复音频相关交换
                    TMt tAudSrc = m_tAudBrdSrc;
                    ChangeAudBrdSrc(&tAudSrc);
                }
            }
        }
        break;

    default:
        ConfLog(FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
                pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
    }
}

/*====================================================================
    函数名      ：ProcMcuBasDisconnectedNotif
    功能        ：码率适配断链处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/27    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMcuBasDisconnectedNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();

    switch(CurState())
    {
    case STATE_ONGOING:

        if ( EQP_TYPE_BAS == tEqp.GetEqpType() &&
             m_tConf.m_tStatus.IsConfAdapting() )
        {
            ConfLog(FALSE, "BAS %u disconnected message received in conference %s!\n", 
                            tEqp.GetEqpId(), m_tConf.GetConfName());

            //停止视频格式类型适配
            if (tEqp.GetEqpId() == m_tVidBasEqp.GetEqpId() && m_tConf.m_tStatus.IsVidAdapting() )
            {
                StopBasAdapt(ADAPT_TYPE_VID);
            }

            //停止速率适配
            if (tEqp.GetEqpId() == m_tBrBasEqp.GetEqpId() && m_tConf.m_tStatus.IsBrAdapting())
            {
                StopBasAdapt(ADAPT_TYPE_BR);
            }

            //停止音频格式类型适配
            if (tEqp.GetEqpId() == m_tAudBasEqp.GetEqpId() && m_tConf.m_tStatus.IsAudAdapting())
            {
                StopBasAdapt(ADAPT_TYPE_AUD);
            }

            //停止级联视频格式类型适配
            if (tEqp.GetEqpId() == m_tCasdVidBasEqp.GetEqpId() && m_tConf.m_tStatus.IsCasdVidAdapting())
            {
                StopBasAdapt(ADAPT_TYPE_CASDVID);
            }

            //停止级联音频格式类型适配
            if (tEqp.GetEqpId() == m_tCasdAudBasEqp.GetEqpId() && m_tConf.m_tStatus.IsCasdAudAdapting())
            {
                StopBasAdapt(ADAPT_TYPE_CASDAUD);
            }

        }

        //高清适配: 只停交换；不释放通道；不停止bas
        if ( EQP_TYPE_BAS == tEqp.GetEqpType() &&
             g_cMcuAgent.IsEqpBasHD(tEqp.GetEqpId()))
        {

            if (m_tConf.m_tStatus.IsHdVidAdapting())
            {
				TPeriEqpStatus tStatus;
				g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tStatus);
				u8 byEqpType = tStatus.m_tStatus.tHdBas.GetEqpType();
				u8 tChnlNum;
				if (TYPE_MPU == byEqpType)
				{
					tChnlNum = MAXNUM_MPU_CHN;
				}
				else if (TYPE_MAU_NORMAL == byEqpType ||
					     TYPE_MAU_H263PLUS == byEqpType)
				{
					tChnlNum = 2;
				}
				else
				{
					tChnlNum = 0;
				}

				for (u8 byIdx = 0; byIdx < tChnlNum; byIdx++)
				{
					StopHdBasSwitch(tEqp, byIdx);
				}
//                StopHdBasSwitch(tEqp, FALSE);
            }
//             else
//             {
//                 StopHdBasSwitch(tEqp, TRUE);
//             }
        }
        break;

    default:
        ConfLog(FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
                        pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
    }
}

/*=============================================================================
    函 数 名： StartBasAdapt
    功    能： 启动码率适配
    算法实现： 
    全局变量： 
    参    数： TAdaptParam* ptParam 适配后的码率参数
			   u8  byAdaptType      适配类型
               TSimCapSet *pSrcSCS  适配前的原始的码流能力集
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/03/08  3.6			万春雷                  创建
=============================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::StartBasAdapt(TAdaptParam *ptParam, u8 byAdaptType, TSimCapSet *pSrcSCS)
{
    u8 byEqpId;
    u8 byChnIdx;
    TPeriEqpStatus tStatus;

    if (NULL == ptParam || NULL == pSrcSCS)
    {
        return FALSE;
    }

    //若是媒体类型适配，利用预留的适配器
    if (ADAPT_TYPE_VID == byAdaptType)
    {
        byEqpId  = m_tVidBasEqp.GetEqpId();
        byChnIdx = m_byVidBasChnnl;
    }
    else if (ADAPT_TYPE_BR == byAdaptType)
    {
        byEqpId  = m_tBrBasEqp.GetEqpId();
        byChnIdx = m_byBrBasChnnl;
    }
    else if (ADAPT_TYPE_AUD == byAdaptType)
    {
        byEqpId  = m_tAudBasEqp.GetEqpId();
        byChnIdx = m_byAudBasChnnl;
    }
    else if (ADAPT_TYPE_CASDVID == byAdaptType)
    {
        byEqpId  = m_tCasdVidBasEqp.GetEqpId();
        byChnIdx = m_byCasdVidBasChnnl;
    }
    else if (ADAPT_TYPE_CASDAUD == byAdaptType)
    {
        byEqpId  = m_tCasdAudBasEqp.GetEqpId();
        byChnIdx = m_byCasdAudBasChnnl;
    }
    else
    {
        //查找空闲的适配器 
        if (!g_cMcuVcApp.GetIdleBasChl(byAdaptType, byEqpId, byChnIdx))
        {
            EqpLog("CMcuVcInst: all BAS busy!\n");
            return FALSE;
        }
        else
        {
            //m_tBrBasEqp.SetMcuEqp( LOCAL_MCUID, byEqpId, EQP_TYPE_BAS );
		    //m_tBrBasEqp.SetConfIdx( (u8)GetInsID() );
		    //m_byBrBasChnnl = byChnIdx;
        }
    }

    //先占用
    g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
    tStatus.m_tStatus.tBas.tChnnl[byChnIdx].SetStatus(TBasChnStatus::RUNING);
    g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);		

    //改变适配状态
    m_tConf.m_tStatus.SetAdaptMode(byAdaptType, TRUE);
	// zgc, 2008-01-22, 上报模式
	ConfModeChange();

    TMediaEncrypt  tEncrypt = m_tConf.GetMediaKey();
    TDoublePayload tDVPayload;
    TDoublePayload tDAPayload;

    if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode())
    {
        ptParam->SetAudActiveType(GetActivePayload(m_tConf, ptParam->GetAudType()));
        ptParam->SetVidActiveType(GetActivePayload(m_tConf, ptParam->GetVidType()));
    }
    else
    {
        ptParam->SetAudActiveType(ptParam->GetAudType());
        ptParam->SetVidActiveType(ptParam->GetVidType());
    }

    if (ADAPT_TYPE_AUD != byAdaptType)
    {
        if (MEDIA_TYPE_H264 == pSrcSCS->GetVideoMediaType() || 
            MEDIA_TYPE_H263PLUS == pSrcSCS->GetVideoMediaType() || 
            CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
            // zbq [10/29/2007] 适应FEC支持其他格式
            m_tConf.GetCapSupportEx().IsVideoSupportFEC() )
        {
            tDVPayload.SetActivePayload(GetActivePayload(m_tConf, pSrcSCS->GetVideoMediaType()));
        }
        else
        {
            tDVPayload.SetActivePayload(pSrcSCS->GetVideoMediaType());
        }
        tDVPayload.SetRealPayLoad(pSrcSCS->GetVideoMediaType());
    }

    if (ADAPT_TYPE_AUD == byAdaptType)
    {
        //音频暂时没有不加密下的动态载荷需求
        if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
            // zbq [10/29/2007] 适应FEC支持其他格式
            m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
        {
            tDAPayload.SetActivePayload(GetActivePayload(m_tConf, pSrcSCS->GetAudioMediaType()));
        }
        else
        {
            tDAPayload.SetActivePayload(pSrcSCS->GetAudioMediaType());
        }
        tDAPayload.SetRealPayLoad(pSrcSCS->GetAudioMediaType());
    }

    ptParam->SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());

    CServMsg cServMsg;							
    cServMsg.SetChnIndex(byChnIdx);
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8 *)ptParam, sizeof(TAdaptParam));
    cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //video
    cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload)); //dvideopaload
    cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //audio
    cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload)); //dApayload 

	// MCU前向纠错, zgc, 2007-09-28
	TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
	cServMsg.CatMsgBody((u8 *)&tCapSupportEx, sizeof(tCapSupportEx));	//FECType

    SendMsgToEqp(byEqpId, MCU_BAS_STARTADAPT_REQ, cServMsg);

    EqpLog("CMcuVcInst: BAS.%d Chn.%d is ready, and expropriate now!\n", byEqpId, byChnIdx);

    //设置定时
    SetTimer(MCUVC_WAITBASRRSP_TIMER+byAdaptType-1, 20*1000, byAdaptType);
    EqpLog("CMcuVcInst.%d Set timer %d\n", GetInsID(), MCUVC_WAITBASRRSP_TIMER+byAdaptType-1);

    return TRUE;		

}

/*=============================================================================
    函 数 名： ChangeBasAdaptParam
    功    能： 变更适配参数(目前没有对级联适配支持)
    算法实现： 
    全局变量： 
    参    数： const TAdaptParam* ptParam 适配后的码率参数
			   BOOL32 bRTP                  是否为RTP适配
               TSimCapSet *pSrcSCS        适配前的原始的码流能力集
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/3/9    3.5			万春雷                  创建
=============================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::ChangeBasAdaptParam(TAdaptParam *ptParam, u8 byAdaptType, TSimCapSet *pSrcSCS)
{
	CServMsg cServMsg;

	TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
	TDoublePayload tDVPayload;
	TDoublePayload tDAPayload;
	u8 byChnIdx;
    u8 byEqpId;

    if (ADAPT_TYPE_AUD == byAdaptType)
    {
        byChnIdx = m_byAudBasChnnl;
        byEqpId = m_tAudBasEqp.GetEqpId();
    }
    else if (ADAPT_TYPE_VID == byAdaptType)
    {
        byChnIdx = m_byVidBasChnnl;
        byEqpId = m_tVidBasEqp.GetEqpId();
    }
    else if (ADAPT_TYPE_BR == byAdaptType)
    {
        byChnIdx = m_byBrBasChnnl;
        byEqpId = m_tBrBasEqp.GetEqpId();
    }
    else
    {
    }

    if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode())
    {
        ptParam->SetAudActiveType(GetActivePayload(m_tConf, ptParam->GetAudType()));
        ptParam->SetVidActiveType(GetActivePayload(m_tConf, ptParam->GetVidType()));
    }
    else
    {
        ptParam->SetAudActiveType(ptParam->GetAudType());
        ptParam->SetVidActiveType(ptParam->GetVidType());
    }

    if (NULL != pSrcSCS)
    {
        if (ADAPT_TYPE_AUD != byAdaptType)
        {
            if (MEDIA_TYPE_H264 == pSrcSCS->GetVideoMediaType() ||
                MEDIA_TYPE_H263PLUS == pSrcSCS->GetVideoMediaType() ||
                CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] 适应FEC支持其他格式
                m_tConf.GetCapSupportEx().IsVideoSupportFEC() )
            {
                tDVPayload.SetRealPayLoad(pSrcSCS->GetVideoMediaType());
                tDVPayload.SetActivePayload(GetActivePayload(m_tConf, pSrcSCS->GetVideoMediaType()));
            }
            else
            {
                tDVPayload.SetRealPayLoad(pSrcSCS->GetVideoMediaType());
                tDVPayload.SetActivePayload(pSrcSCS->GetVideoMediaType());
            }
        }

        if (ADAPT_TYPE_AUD == byAdaptType)
        {
            //音频暂时没有不加密下的动态载荷需求
            if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] 适应FEC支持其他格式
                m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
            {
                tDAPayload.SetRealPayLoad(pSrcSCS->GetAudioMediaType());
                tDAPayload.SetActivePayload(GetActivePayload(m_tConf,  pSrcSCS->GetAudioMediaType()));
            }
            else
            {
                tDAPayload.SetRealPayLoad(pSrcSCS->GetAudioMediaType());
                tDAPayload.SetActivePayload(pSrcSCS->GetAudioMediaType());
            }
        }
    }
    else
    {
        if (ADAPT_TYPE_AUD != byAdaptType)
        {
            if (MEDIA_TYPE_H264 == ptParam->GetVidType() || 
                MEDIA_TYPE_H263PLUS == ptParam->GetVidType() || 
                CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] 适应FEC支持其他格式
                m_tConf.GetCapSupportEx().IsVideoSupportFEC())
            {
                tDVPayload.SetRealPayLoad(ptParam->GetVidType());
                tDVPayload.SetActivePayload(GetActivePayload(m_tConf, ptParam->GetVidType()));
            }
            else
            {
                tDVPayload.SetRealPayLoad(ptParam->GetVidType());
                tDVPayload.SetActivePayload(ptParam->GetVidType());
            }
        }

        if (ADAPT_TYPE_AUD == byAdaptType)
        {
            //音频暂时没有不加密下的动态载荷需求
            if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] 适应FEC支持其他格式
                m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
            {
                tDAPayload.SetRealPayLoad(ptParam->GetAudType());
                tDAPayload.SetActivePayload(GetActivePayload(m_tConf, ptParam->GetAudType()));
            }
            else
            {
                tDAPayload.SetRealPayLoad(ptParam->GetAudType());
                tDAPayload.SetActivePayload(ptParam->GetAudType());
            }
        }
    }

    ptParam->SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());

    cServMsg.SetChnIndex(byChnIdx);
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8 *)ptParam, sizeof(TAdaptParam));
    cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //video
    cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload)); //dvideopaload
    cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //audio
    cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload)); //dApayload 

    SendMsgToEqp(byEqpId, MCU_BAS_SETADAPTPARAM_CMD, cServMsg);

    return TRUE;
}

/*====================================================================
    函数名      : StopBasSwitch
    功能        ：停止码流适配器交换
    输入        ：
    返回 ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/22    3.0         zmy           Create
	04/03/20    3.0         胡昌威          修改
====================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::StopBasSwitch(u8 byAdaptType)
{
    TEqp tBasEqp;
    u8 byBasChnnl;
    u8 byMode;

    if (ADAPT_TYPE_AUD == byAdaptType)
    {
        byBasChnnl = m_byAudBasChnnl;
        tBasEqp = m_tAudBasEqp;
        byMode = MODE_AUDIO;
    }
    else if (ADAPT_TYPE_VID == byAdaptType)
    {
        byBasChnnl = m_byVidBasChnnl;
        tBasEqp = m_tVidBasEqp;
        byMode = MODE_VIDEO;
    }
    else if (ADAPT_TYPE_BR == byAdaptType)
    {
        byBasChnnl = m_byBrBasChnnl;
        tBasEqp = m_tBrBasEqp;
        byMode = MODE_VIDEO;
    }
    else if (ADAPT_TYPE_CASDAUD == byAdaptType)
    {
        byBasChnnl = m_byCasdAudBasChnnl;
        tBasEqp = m_tCasdAudBasEqp;
        byMode = MODE_AUDIO;
    }
    else if (ADAPT_TYPE_CASDVID == byAdaptType)
    {
        byBasChnnl = m_byCasdVidBasChnnl;
        tBasEqp = m_tCasdVidBasEqp;
        byMode = MODE_VIDEO;
    }

    //拆除适配器的交换桥
    g_cMpManager.RemoveSwitchBridge(tBasEqp, byBasChnnl, byMode);

    //拆除适配器到prs的交换
    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
        if (ADAPT_TYPE_AUD == byAdaptType)
        {
            StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlAudBas, FALSE, MODE_AUDIO);
        }
        else if (ADAPT_TYPE_VID == byAdaptType)
        {
            StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlVidBas, FALSE, MODE_VIDEO);
        }
        else if (ADAPT_TYPE_BR == byAdaptType)
        {
            StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlBrBas, FALSE, MODE_VIDEO);
        }
    }

    //拆除适配器到终端的交换
    StopSwitchToAllSubMtJoinedConfNeedAdapt(FALSE, byMode, byAdaptType);

    //拆除到适配器的交换
    StopSwitchToPeriEqp(tBasEqp.GetEqpId(), byBasChnnl);

    return TRUE;
}

/*====================================================================
    函数名      : StopBasAdapt
    功能        ：停止码流适配
    输入        ：
    返回 ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/22    3.0         zmy           Create
	04/02/25    3.0         胡昌威         修改
====================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::StopBasAdapt(u8 byAdaptType)
{
    TEqp *ptBasEqp = NULL;
    u8 byBasChnnl = EQP_CHANNO_INVALID;
    u8 byBasMode = CONF_BASMODE_NONE;

    if (ADAPT_TYPE_AUD == byAdaptType)
    {
        byBasMode = CONF_BASMODE_AUD;
        byBasChnnl = m_byAudBasChnnl;
        ptBasEqp = &m_tAudBasEqp;

        //停止需要音频适配的prs,绑定处理        
        if(m_tConfInStatus.IsPrsChnlAudBasStart())
        {
            StopPrs(PRSCHANMODE_AUDBAS);
        }        
    }
    else if (ADAPT_TYPE_VID == byAdaptType)
    {
        byBasMode = CONF_BASMODE_VID;
        byBasChnnl = m_byVidBasChnnl;
        ptBasEqp = &m_tVidBasEqp;

        if(m_tConfInStatus.IsPrsChnlVidBasStart())
        {
            StopPrs(PRSCHANMODE_VIDBAS);
        }    
    }
    else if (ADAPT_TYPE_BR == byAdaptType)
    {
        byBasMode = CONF_BASMODE_BR;
        byBasChnnl = m_byBrBasChnnl;
        ptBasEqp = &m_tBrBasEqp;

        if(m_tConfInStatus.IsPrsChnlBrBasStart())
        {
            StopPrs(PRSCHANMODE_BRBAS);
        }    
    }
    else if (ADAPT_TYPE_CASDAUD == byAdaptType)
    {
        byBasMode = CONF_BASMODE_CASDAUD;
        byBasChnnl = m_byCasdAudBasChnnl;
        ptBasEqp = &m_tCasdAudBasEqp;
    }
    else if (ADAPT_TYPE_CASDVID == byAdaptType)
    {
        byBasMode = CONF_BASMODE_CASDVID;
        byBasChnnl = m_byCasdVidBasChnnl;
        ptBasEqp = &m_tCasdVidBasEqp;
    }

    if (EQP_CHANNO_INVALID == byBasChnnl || NULL == ptBasEqp)
    {
        ConfLog(FALSE, "[StopBasAdapt] invalid byBasChnnl.%d or byAdaptType.%d\n", byBasChnnl, byAdaptType);
        return FALSE;
    }

    CServMsg cServMsg;
    cServMsg.SetChnIndex(byBasChnnl);
    cServMsg.SetConfId(m_tConf.GetConfId());
    TPeriEqpStatus tStatus;

    //拆掉适配器交换
    StopBasSwitch(byAdaptType);

    //向适配器发送停止消息
    SendMsgToEqp(ptBasEqp->GetEqpId(), MCU_BAS_STOPADAPT_REQ, cServMsg);

    //change info
    //u8 byBasMode = byAdaptType;
    m_tConf.m_tStatus.SetAdaptMode(byBasMode, FALSE);
	// zgc, 2008-01-22, 上报模式
	ConfModeChange();

    //释放通道
    if (!g_cMcuVcApp.GetPeriEqpStatus(ptBasEqp->GetEqpId(), &tStatus))
    {
        return FALSE;
    }

    tStatus.m_tStatus.tBas.tChnnl[byBasChnnl].SetStatus(TBasChnStatus::READY);
    g_cMcuVcApp.SetPeriEqpStatus(ptBasEqp->GetEqpId(), &tStatus);

    return TRUE;
}

/*====================================================================
    函数名      : ProcBasStartupTimeout
    功能        ：启动码率适配应答超时消息处理函数
    输入        ：
    返回 ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/24    3.0         zmy           Create
====================================================================*/
//modify bas 2
void CMcuVcInst::ProcBasStartupTimeout(const CMessage *pcMsg)
{
    TPeriEqpStatus tStatus;
    u8 byAdaptType = *(u8 *)pcMsg->content;

    u8 byBasChnnl = EQP_CHANNO_INVALID;
    TEqp *ptBasEqp = NULL;

    switch(byAdaptType)
    {
    case ADAPT_TYPE_AUD:
        byBasChnnl = m_byAudBasChnnl;
        ptBasEqp = &m_tAudBasEqp;
    	break;

    case ADAPT_TYPE_VID:
        byBasChnnl = m_byVidBasChnnl;
        ptBasEqp = &m_tVidBasEqp;
    	break;

    case ADAPT_TYPE_BR:
        byBasChnnl = m_byBrBasChnnl;
        ptBasEqp = &m_tBrBasEqp;
        break;

    case ADAPT_TYPE_CASDAUD:
        byBasChnnl = m_byCasdAudBasChnnl;
        ptBasEqp = &m_tCasdAudBasEqp;
        break;

    case ADAPT_TYPE_CASDVID:
        byBasChnnl = m_byCasdVidBasChnnl;
        ptBasEqp = &m_tCasdVidBasEqp;
        break;

    default:
        ConfLog(FALSE, "[ProcBasStartupTimeout] invalid byAdaptType.%d\n", byAdaptType);
        return;
        break;
    }  

    if (EQP_CHANNO_INVALID == byBasChnnl || ptBasEqp->IsNull())
    {
        ConfLog(FALSE, "[ProcBasStartupTimeout] invalid byBasChnnl.%d\n", byBasChnnl);
        return;
    }

    //释放通道
    if (!g_cMcuVcApp.GetPeriEqpStatus(ptBasEqp->GetEqpId(), &tStatus))
    {
        //ptBasEqp->SetNull();
        return;
    }

    tStatus.m_tStatus.tBas.tChnnl[byBasChnnl].SetStatus(TBasChnStatus::READY);
    g_cMcuVcApp.SetPeriEqpStatus(ptBasEqp->GetEqpId(), &tStatus);

    //改变适配状态
    m_tConf.m_tStatus.SetAdaptMode(byAdaptType, FALSE);
	// zgc, 2008-01-22, 上报模式
	ConfModeChange();

    //发送停止消息，防止应答消息缓存在队列中
    CServMsg cServMsg;
    cServMsg.SetChnIndex(byBasChnnl);
    cServMsg.SetConfId(m_tConf.GetConfId());
    SendMsgToEqp(ptBasEqp->GetEqpId(), MCU_BAS_STOPADAPT_REQ, cServMsg);

    //ptBasEqp->SetNull();

    return;
}

/*=============================================================================
    函 数 名： ProcHduMcuNeedIFrameCmd
    功    能： 
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/3/9    4.6		    江乐斌                  创建
=============================================================================*/
void CMcuVcInst::ProcHduMcuNeedIFrameCmd(const CMessage *pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TMt tSrcMt;

	TPeriEqpStatus tHduInfo;

    switch(CurState())
	{
    case STATE_ONGOING:	
	    switch( pcMsg->event ) 
		{			
	    case HDU_MCU_NEEDIFRAME_CMD:
		{
			u8 byChnlIdx = cServMsg.GetChnIndex();
			g_cMcuVcApp.GetPeriEqpStatus(cServMsg.GetSrcSsnId(), &tHduInfo);
            tSrcMt = (TMt)tHduInfo.m_tStatus.tHdu.atVideoMt[byChnlIdx];
		
			TLogicalChannel tLogicChn;
			if (tSrcMt.IsNull() ||
				(MT_TYPE_MT == tSrcMt.GetMtType() &&
				!m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicChn, FALSE)))
			{ 
				CallLog("[ProcHduMcuNeedIFrameCmd] tMt/tEqp.%d unexist or logicChn unexist!\n", tSrcMt.GetMtId());
			}
			else
			{
				if (TYPE_MT == tSrcMt.GetType())
				{
				    tSrcMt = m_ptMtTable->GetMt(tSrcMt.GetMtId());
					NotifyFastUpdate(tSrcMt, MODE_VIDEO);
				}
				else if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_VMP == tSrcMt.GetEqpType())
				{
					u8 bySrcChnnl = 0;
					u8 byMtMediaType = 0;
					u8 byMtRes = 0;			

					byMtMediaType = m_tConf.GetMainVideoMediaType();

					byMtRes = m_tConf.GetMainVideoFormat();

		 			bySrcChnnl = GetVmpOutChnnlByRes(byMtRes, m_tVmpEqp.GetEqpId(), byMtMediaType);
                    
					EqpLog("[ProcHduMcuNeedIFrameCmd]Req iframe to eqp(eqpid:%d chnlidx:%d)\n", 
						
						   tSrcMt.GetEqpId(), bySrcChnnl);

					NotifyEqpFastUpdate(tSrcMt,bySrcChnnl);
				} 
				else
				{
					EqpLog("[ProcHduMcuNeedIFrameCmd]src(mcuid:%d mtid:%d type:%d) in hdu channel%d has wrong type\n",
						   tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSrcMt.GetType(), byChnlIdx);
				}
			}
		}
		    break;
	    default:
		    ConfLog(FALSE, "[ProcHduMcuNeedIFrameCmd]:The Message type %u(%s) is not fit", pcMsg->event, ::OspEventDesc(pcMsg->event) );
		    break;
		}
	    break;
    default:
        ConfLog(FALSE, "[ProcHduMcuNeedIFrameCmd]Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
	}
	return;
}

/*=============================================================================
    函 数 名： ProcBasMcuCommand
    功    能： 
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/9/18   4.0		    周广程                  创建
=============================================================================*/
void CMcuVcInst::ProcBasMcuCommand(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TMt tSrcMt;

    switch(CurState())
    {
    case STATE_ONGOING:

        switch( pcMsg->event ) 
        {			
        case BAS_MCU_NEEDIFRAME_CMD:

            if ( !IsHDConf(m_tConf) )
            {
                if (m_tConf.HasSpeaker())
                {
                    tSrcMt = GetLocalSpeaker();
                    
                    if (cServMsg.GetChnIndex() != m_byVidBasChnnl &&
                        cServMsg.GetChnIndex() != m_byBrBasChnnl &&
                        cServMsg.GetChnIndex() != m_byCasdVidBasChnnl )
                    {
                        ConfLog(FALSE, "[ProcBasMcuCommand]cServMsg.GetChnIndex()[%d] != m_byAnyBasChnnl\n",
                            cServMsg.GetChnIndex());
                        break;
                    }
                    
                    NotifyFastUpdate(tSrcMt, MODE_VIDEO);
                }
                else
                {
                    EqpLog("[ProcBasMcuCommand] Err: No speaker for VCU from eqp.%d\n", cServMsg.GetSrcSsnId());
                }
            }
            else
            {
				if (VCS_CONF == m_tConf.GetConfSource())
				{
					NotifyFastUpdate(m_tConf.GetChairman(), MODE_VIDEO, TRUE);
					NotifyFastUpdate(m_cVCSConfStatus.GetCurVCMT(), MODE_VIDEO, TRUE);
					return;
				}
                u8 byEqpId = cServMsg.GetSrcSsnId();
                u8 byChnIdx = cServMsg.GetChnIndex();
                EqpLog( "[ProcBasMcuCommand] Receive bas.%d<chn.%d> BAS_MCU_NEEDIFRAME_CMD!\n", byEqpId, byChnIdx );

                if ( !g_cMcuAgent.IsEqpBasHD(byEqpId) )
                {
                    ConfLog( FALSE, "[ProcBasMcuCommand] The bas.%d is common bas, the conf.%s is HD conf, It's impossible!\n",
                                byEqpId, m_tConf.GetConfName() );
                    break;
                }

                TEqp tEqp = g_cMcuVcApp.GetEqp(byEqpId);
                u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnIdx);

                if ( CHN_ADPMODE_MVBRD == byChnMode )
                {
                    if ( m_tConf.HasSpeaker() )
                    {
                        tSrcMt = GetLocalSpeaker();                      
                        NotifyFastUpdate(tSrcMt, MODE_VIDEO);
                    }
                    else
                    {
                        EqpLog("[ProcBasMcuCommand] Err: No speaker for VCU from eqp.%d\n", cServMsg.GetSrcSsnId());
                    }
                    break;
                }
                else if ( CHN_ADPMODE_DSBRD == byChnMode)
                {
                    if ( !m_tDoubleStreamSrc.IsNull() )
                    {
                        tSrcMt = m_tDoubleStreamSrc;
                        NotifyFastUpdate(tSrcMt, MODE_SECVIDEO);
                    }
                    else
                    {
                        EqpLog("[ProcBasMcuCommand] Err: No ds src for VCU from eqp.%d\n", cServMsg.GetSrcSsnId());
                    }
                    break;
                }
                else if ( CHN_ADPMODE_MVSEL == byChnMode )
                {
                    //从m_cBasMgr获取对应的选看源，作对应的关键帧请求
                }
                else if ( CHN_ADPMODE_DSSEL == byChnMode )
                {
                    //从m_cBasMgr获取对应的选看源，作对应的关键帧请求
                }
            }
            break;

        default:
            break;
        }
        break;

    default:
        ConfLog(FALSE, "[ProcBasMcuCommand]Wrong message %u(%s) received in state %u!\n", 
                        pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
    }
}

/*===========================================================================
函 数 名： ProcHduMcuChnnlStatusNotif
功    能： 
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/02/26  4.6.1		江乐斌                  创建

=============================================================================*/
void CMcuVcInst::ProcHduMcuChnnlStatusNotif( const CMessage * pcMsg )
{
	if (STATE_ONGOING != CurState())
    {
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

	THduChnStatus tHduChnStatus = *(THduChnStatus*)cServMsg.GetMsgBody();
	
    TPeriEqpStatus tHduStatus;
	
    THduSwitchInfo tHduSwitchInfo;
    u8 byMemberType;
    u8 bySrcMtConfIdx;
    u8 byChnlIdx = tHduChnStatus.GetChnIdx();
    TEqp tEqp;
	
    tEqp.SetMcuEqp(tHduChnStatus.GetEqp().GetMcuId(), tHduChnStatus.GetEqpId(), tHduChnStatus.GetEqp().GetEqpType() );
	
    g_cMcuVcApp.GetPeriEqpStatus(tHduChnStatus.GetEqpId(), &tHduStatus);
	
	if ( THduChnStatus::eREADY ==  tHduChnStatus.GetStatus() )
	{
		bySrcMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx();
		if (m_byConfIdx == bySrcMtConfIdx)
		{
			byMemberType = tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType;
			if (0 != byMemberType)
			{
				cServMsg.SetMsgBody( (u8*)&tHduStatus, sizeof(tHduStatus) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );

				tHduSwitchInfo.SetMemberType(byMemberType);
				tHduSwitchInfo.SetSrcMt((TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx]);
				tHduSwitchInfo.SetDstMt(tEqp);
				tHduSwitchInfo.SetDstChlIdx(byChnlIdx);
				
				::OspPost(MAKEIID(AID_MCU_VC, GetInsID()), MCUVC_STARTSWITCHHDU_NOTIFY, 
					(u8*)&tHduSwitchInfo, sizeof(tHduSwitchInfo));
			}
		}

	}
	
	cServMsg.SetMsgBody( (u8*)&tHduStatus, sizeof( tHduStatus ) );
	SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );
	
    return;
}

/*------------------------------------------------------------------*/
/*                                 mau                              */
/*------------------------------------------------------------------*/


/*=============================================================================
    函 数 名： ProcHdBasChnnlStatusNotif
    功    能： 高清BAS通道状态通知会议处理函数
    算法实现： 由于在高清BAS连接时进行适配调整存在时序上的问题，可能起适配失败, 因此调整到收到通道状态时进行
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/9/3    4.0		    周广程                  创建
    2008/11/19  4.5         张宝卿                  适配管理类集成调整；H263+适配调整
=============================================================================*/
void CMcuVcInst::ProcHdBasChnnlStatusNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    u8 *pbyMsgBody  = (u8 *)cServMsg.GetMsgBody();
    u8  byChnType = *pbyMsgBody;
    pbyMsgBody += sizeof(u8);

    //只取父类
    THDBasChnStatus tChnStatus;
    memcpy( &tChnStatus, pbyMsgBody, sizeof(THDBasChnStatus) );
    if ( tChnStatus.GetStatus() != THDBasChnStatus::READY )
    {
        return;
    }

    EqpLog( "[ProcHdBasChnnlStatusNotif] EqpId.%d, ChnIdx.%d, ChnType.%d, Status.%d\n",
             tChnStatus.GetEqpId(), 
             tChnStatus.GetChnIdx(), 
             byChnType,
             tChnStatus.GetStatus() );

    u8 byEqpId = tChnStatus.GetEqpId();
    u8 byChnIdx = tChnStatus.GetChnIdx();

    BOOL32 bIsReserved = FALSE;

    TPeriEqpStatus tEqpStatus;
    g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tEqpStatus);

    //获取外设状态
    THDBasVidChnStatus *ptChnStatus = NULL;
    u8 byBasType = tEqpStatus.m_tStatus.tHdBas.GetEqpType();
    if (TYPE_MPU == byBasType)
    {
        ptChnStatus = tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx);
    }
    else if (TYPE_MAU_NORMAL == byBasType ||
             TYPE_MAU_H263PLUS == byBasType )
    {
        BOOL32 bChnMV = 0 == byChnIdx ? TRUE : FALSE;
        if (bChnMV)
        {
            ptChnStatus = tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus();
        }
        else
        {
            ptChnStatus = tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus();
        }
    }
    
    if (NULL == ptChnStatus)
    {
        ConfLog(FALSE, "[ProcHdBasChnnlStatusNotif] ptChnStatus.%d.IsNull, check it!\n", byChnIdx);
        return;
    }
    
    bIsReserved = ptChnStatus->GetReservedType();
    if (!bIsReserved)
    {
        EqpLog("[ProcHdBasChnnlStatusNotif] Eqp<%d,%d> isn't reserved, ignore it\n", byEqpId, byChnIdx);
        return;
    }

    u8 byMediaMode = MODE_NONE;
    u8 bySwitchMode = SWITCH_MODE_NONE;
    u8 byChnMode = m_cBasMgr.GetChnMode(tChnStatus.GetEqp(), byChnIdx);
    if(!CheckHdBasChnMode(byChnMode, byMediaMode, bySwitchMode))
    {
        EqpLog("[ProcHdBasChnnlStatusNotif] HdBas<%d, %d> isn't reseverd for conf.%d!\n", 
            byEqpId, byChnIdx, m_byConfIdx);
        return;
    }

    switch (byChnMode)
    {
    case CHN_ADPMODE_MVBRD:

        if (!m_tConf.m_tStatus.IsHdVidAdapting())
        {
            ConfLog(FALSE, "[ProcHdBasChnnlStatusNotif] HdBas<%d,%d> should be \
restore mv but not due to status changed!\n", byEqpId, byChnIdx);
            return;
        }
        ChangeHDAdapt(tChnStatus.GetEqp(), byChnIdx, TRUE);
        break;

    case CHN_ADPMODE_DSBRD:

        if (!m_tConf.m_tStatus.IsHDDoubleVidAdapting())
        {
            ConfLog(FALSE, "[ProcHdBasChnnlStatusNotif] HdBas<%d,%d> should be \
restore ds but not due to status changed!\n", byEqpId, byChnIdx);
            return;
        }
        ChangeHDAdapt(tChnStatus.GetEqp(), byChnIdx, TRUE);
        break;

    case CHN_ADPMODE_MVSEL:
    case CHN_ADPMODE_DSSEL:
        ChangeHDAdapt(tChnStatus.GetEqp(), byChnIdx, TRUE);
    	break;

    default:
        ConfLog(FALSE, "[ProcHdBasChnnlStatusNotif] unexpected type.%d\n", byChnMode);
        break;
    }

    return;
}

/*=============================================================================
函 数 名： ProcHdBasMcuRsp
功    能： 
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/8/7    4.0			周广程                  创建
2008/11/20  4.5         张宝卿                  高清bas群组集成
2009/04/19  4.6         张宝卿                  重写，兼容mpu和选看适配
=============================================================================*/
void CMcuVcInst::ProcHdBasMcuRsp( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    cServMsg.SetEventId( pcMsg->event );
    
    u8 byChnIdx = cServMsg.GetChnIndex();
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();

    EqpLog( "[ProcHdBasMcuRsp] Receive bas.%d(chn.%d) rsp<msg.%s>!\n", 
             tEqp.GetEqpId(), byChnIdx, ::OspEventDesc( pcMsg->event));

    u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnIdx);
    if (CHN_ADPMODE_NONE == byChnMode)
    {
        ConfLog(FALSE, "[ProcHdBasMcuRsp] HdBas<%d,%d> unexist, check it\n", tEqp.GetEqpId(), byChnIdx);
        return;
    }
    switch (byChnMode)
    {
    case CHN_ADPMODE_MVBRD:
    case CHN_ADPMODE_DSBRD:
        ProcHdBasMcuBrdAdpRsp( cServMsg );
        break;

    case CHN_ADPMODE_MVSEL:
    case CHN_ADPMODE_DSSEL:
        ProcHdBasMcuSelRsp( cServMsg );
    	break;
    default:
        //FIXME: 选看适配未开发完成，临时处理
        ProcHdBasMcuSelRsp( cServMsg );
        break;        
    }
    return;
}

/*=============================================================================
函 数 名： ProcHdBasMcuBrdAdpRsp
功    能： 广播适配通道启动成功响应处理
算法实现： 
全局变量： 
参    数： CServMsg &cServMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/8/21   4.0			周广程                  创建
2008/11/20  4.5         张宝卿                  高清bas群组集成
2009/06/05  4.5         付秀华                  对主流和双流通道启用成功统一响应
=============================================================================*/
void CMcuVcInst::ProcHdBasMcuBrdAdpRsp( CServMsg &cServMsg )
{   
    u8 byChlIdx = cServMsg.GetChnIndex();
    TEqp tEqp = *(TEqp*)cServMsg.GetMsgBody();
    
    tEqp.SetConfIdx(m_byConfIdx);
    u32 dwTimerId = MCUVC_WAITHDVIDBASRSP_TIMER + m_cBasMgr.GetChnId(tEqp, byChlIdx);
    
    //取消适配器应答定时
    KillTimer(dwTimerId);
    EqpLog("[HDBAS_MCU_STARTADAPT_ACK] KillTimer(%d)\n", dwTimerId);

    switch (cServMsg.GetEventId())
    {
    case HDBAS_MCU_STARTADAPT_ACK:
        {
			THDBasVidChnStatus tStatus;
			if (!m_cBasMgr.GetChnStatus(tEqp, byChlIdx, tStatus))
			{
				ConfLog(FALSE, "[ProcHdBasMcuMVRsp] get eqp<%d, %d> status failed\n", 
														  tEqp.GetEqpId(), byChlIdx);
				return;
			}

			//设置通道状态为running      
			tStatus.SetStatus(THDBasChnStatus::RUNING);
			if (!m_cBasMgr.UpdateChn(tEqp, byChlIdx, tStatus))
			{
				ConfLog(FALSE, "[ProcHdBasMcuMVRsp] UpdateMVChn<%d, %d> failed\n", 
													   tEqp.GetEqpId(), byChlIdx);
				return;
			}
			g_cMcuVcApp.SendPeriEqpStatusToMcs(tEqp.GetEqpId());

			//建立相关交换
			StartHdBasSwitch(tEqp, byChlIdx);
        }
        break;

    case HDBAS_MCU_STARTADAPT_NACK:
		{
			//zbq[11/20/2008] 适配会议开启后不再变更，除非结会
			/*
			//改变会议适配状态
			m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_VID, FALSE);
			ConfModeChange();*/
			u8 byChnlMode = m_cBasMgr.GetChnMode(tEqp, byChlIdx); 
			if (byChnlMode != CHN_ADPMODE_NONE)
			{
				TPeriEqpStatus tHDBasStatus;
				g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tHDBasStatus);
				THDBasVidChnStatus * ptChanStatus = NULL;
				if (CHN_ADPMODE_MVBRD == byChnlMode)
				{
					ptChanStatus = tHDBasStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(byChlIdx);
				}
				else if (CHN_ADPMODE_DSBRD == byChnlMode)
				{
					ptChanStatus = tHDBasStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(byChlIdx);
				}

				if ( NULL != ptChanStatus )
				{
					TEqp tHDBas;
					tHDBas = ptChanStatus->GetEqp();
					tHDBas.SetConfIdx(0);
					ptChanStatus->SetEqp(tHDBas);
					g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tHDBasStatus);
					g_cMcuVcApp.SendPeriEqpStatusToMcs(tEqp.GetEqpId());
				}
			}

		}
        break;

    default:
        break;
    }
}

/*=============================================================================
    函 数 名： ProcHdBasMcuSelRsp
    功    能： 高清选看适配通道消息响应处理：含主流选看和双流选看（本版本只支持级联适配）
    算法实现： 
    全局变量： 
    参    数： CServMsg &cServMsg
    返 回 值： void 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/03/26  4.5         张宝卿                  创建
    2009/04/19  4.6         张宝卿                  兼容普通的选看适配逻辑
=============================================================================*/
void CMcuVcInst::ProcHdBasMcuSelRsp(CServMsg &cServMsg)
{
    u8 byChlIdx = cServMsg.GetChnIndex();
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();
    tEqp.SetConfIdx(m_byConfIdx);

    switch (cServMsg.GetEventId())
    {
    case HDBAS_MCU_STARTADAPT_ACK:
        {
            if (tEqp.GetEqpId() != m_tCasdVidBasEqp.GetEqpId())
            {
                ConfLog(FALSE, "[ProcHdBasMcuSelRsp] Eqp.%d doesn't adp for cascade EQP.%d\n", 
                    tEqp.GetEqpId(), m_tCasdVidBasEqp.GetEqpId());
                return;
            }
            
            if( m_tCascadeMMCU.IsNull() )
            {
                 ConfLog(FALSE, "[ProcHdBasMcuSelRsp] m_tCascadeMMCU.IsNull()!\n");
                 return;
            }

            //图像交换到码流适配器
            TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
            if( ptConfMcInfo == NULL || ptConfMcInfo->m_tSpyMt.IsNull() )
            {
                ConfLog(FALSE, "[ProcHdBasMcuSelRsp] ptConfMcInfo.0x%x!\n");
                return;
            }

            TMt tAdaptSrc = ptConfMcInfo->m_tSpyMt;
            u8 bySrcChnnl = (tAdaptSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
            StartSwitchToPeriEqp(tAdaptSrc, bySrcChnnl, 
                                 tEqp.GetEqpId(), byChlIdx, 
                                 MODE_VIDEO, SWITCH_MODE_SELECT, TRUE);
            
            //zbq[05/07/2009]根据上级MCU的能力集确定回传适配的输出
            u8 byOutIdx = 0;
            TSimCapSet tDstSimCap = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());

            if (MEDIA_TYPE_H264 != tDstSimCap.GetVideoMediaType() ||
                (MEDIA_TYPE_H264 == tDstSimCap.GetVideoMediaType() &&
                 VIDEO_FORMAT_CIF == tDstSimCap.GetVideoResolution()))
            {
                byOutIdx = 1;
            }

            //建立适配器的交换桥
            u8 byBasOutputChn = byChlIdx * MAXNUM_VOUTPUT;
            
            BOOL32 bChn = g_cMpManager.SetSwitchBridge(tEqp, byBasOutputChn+byOutIdx, MODE_VIDEO, TRUE);
            
            if ( !bChn )
            {
                ConfLog( FALSE, "[ProcHdBasMcuDualRsp] SetSwitchBridge chn.%d failed!\n", byBasOutputChn );
            }
            
            //适配码流交换到prs
            if (m_tConf.GetConfAttrb().IsResendLosePack())
            {
                // FIXME：高清适配丢包重传未添加
            }

            //适配码流交换到上级
            StartSwitchToSubMt(tEqp, byBasOutputChn + byOutIdx, m_tCascadeMMCU.GetMtId(), MODE_VIDEO);
            
            NotifyEqpFastUpdate(tEqp, m_byCasdVidBasChnnl, TRUE);
        }
        break;
        
    case HDBAS_MCU_STARTADAPT_NACK:
        /*
        KillTimer(dwTimerId);
        EqpLog("[HDBAS_MCU_STARTADAPT_ACK]KillTimer(%d)\n", dwTimerId);
        */
        //改变会议适配状态
        m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_CASDVID, FALSE);
        ConfModeChange();
        
        break;
        
    default:
        break;
    }

    return;
}


/*=============================================================================
    函 数 名： StartHDMVBrdAdapt
    功    能： 开启会议广播适配
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： BOOL32
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/11/18  4.5		    张宝卿                  创建
    2009/04/17  4.6         张宝卿                  层次整理
=============================================================================*/
BOOL32 CMcuVcInst::StartHDMVBrdAdapt()
{
    //0.重整适配源
    RefreshBasParam4MVBrd();

    //1.微调适配参数
    RefreshBasParam4AllMt();

    //2.逐个开启
    TBasChn atBasChn[MAXNUM_CONF_MVCHN];
    u8 byNum = 0;
    m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_MVBRD);

    u8 byIdx = 0;
    for(; byIdx < byNum; byIdx ++)
    {
        ChangeHDAdapt(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId(), TRUE);
    }

    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_VID, TRUE);
    ConfModeChange();

    return TRUE;
}

/*=============================================================================
    函 数 名： StopHDMVBrdAdapt
    功    能： 停止会议广播适配
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： BOOL32
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/04/17  4.6		    张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::StopHDMVBrdAdapt()
{
    if (!m_tConf.m_tStatus.IsHdVidAdapting())
    {
        return FALSE;
    }

    TBasChn atBasChn[MAXNUM_CONF_MVCHN];
    u8 byNum = 0;
    m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_MVBRD);
    
    u8 byIdx = 0;
    for(; byIdx < byNum; byIdx ++)
    {
        StopHDAdapt(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId());
    }
    
    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_VID, TRUE);
    ConfModeChange();
    
    return TRUE;
}

/*=============================================================================
    函 数 名： StartHDDSBrdAdapt
    功    能： 开启会议双流广播适配
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： BOOL32
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/04/17  4.6		    张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::StartHDDSBrdAdapt()
{
    //0.调整适配输入
    RefreshBasParam4DSBrd();

    //1.微调适配参数
    RefreshBasParam4AllMt(FALSE, TRUE);
    
    //2.逐个开启
    TBasChn atBasChn[MAXNUM_CONF_MVCHN];
    u8 byNum = 0;
    m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_DSBRD);
    
    u8 byIdx = 0;
    for(; byIdx < byNum; byIdx ++)
    {
        ChangeHDAdapt(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId(), TRUE);
    }
    
    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_DOUBLEVID, TRUE);
    ConfModeChange();
    
    return TRUE;
}

/*=============================================================================
    函 数 名： StopHDDSBrdAdapt
    功    能： 停止会议双流广播适配
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： BOOL32
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/04/17  4.6		    张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::StopHDDSBrdAdapt()
{
    if (!m_tConf.m_tStatus.IsHDDoubleVidAdapting())
    {
        return FALSE;
    }
    
    TBasChn atBasChn[MAXNUM_CONF_MVCHN];
    u8 byNum = 0;
    m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_DSBRD);
    
    u8 byIdx = 0;
    for(; byIdx < byNum; byIdx ++)
    {
        StopHDAdapt(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId());
    }
    
    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_DOUBLEVID, TRUE);
    ConfModeChange();
    
    return TRUE;
}

/*=============================================================================
    函 数 名： ChangeHDAdapt
    功    能： 
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： BOOL32
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/11/18  4.5		    张宝卿                  创建
    2009/04/17  4.6         张宝卿                  修改
=============================================================================*/
BOOL32 CMcuVcInst::ChangeHDAdapt(const TEqp &tEqp, u8 byChnId, BOOL32 bStart)
{
    CServMsg cServMsg;
    cServMsg.SetMsgBody();

    u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnId);
    
    //选看适配动态获取和调整，需要动态整理通道状态；广播适配适配整体开启刷新，或单个加入
    BOOL32 bRet = TRUE;
    if (CHN_ADPMODE_MVSEL == byChnMode)
    {
        bRet = RefreshBasMVSelAdpParam(tEqp, byChnId);
    }
    else if (CHN_ADPMODE_DSSEL == byChnMode)
    {
        bRet = RefreshBasDSSelAdpParam(tEqp, byChnId);
    }
    if (!bRet)
    {
        ConfLog(FALSE, "[ChangeHDAdapt] tEqp<%d, %d> refresh ds sel param failed\n", 
            tEqp.GetEqpId(), byChnId);
        return FALSE;
    }

    
    THDBasVidChnStatus tVidChn;
    if (!m_cBasMgr.GetChnStatus(tEqp, byChnId, tVidChn))
    {
        return FALSE;
    }
    
    THDAdaptParam tAdaptParam;
    
    u8 byOutIdx = 0;
    for (; byOutIdx < MAXNUM_VOUTPUT; byOutIdx ++)
    {
        tAdaptParam.Reset();
        if (!tVidChn.GetOutputVidParam(byOutIdx)->IsNull())
        {
            memcpy(&tAdaptParam, tVidChn.GetOutputVidParam(byOutIdx), sizeof(tAdaptParam));
        }
        else
        {
            //构造adpparam，保护baphd: 双流保护逻辑亦然
            tAdaptParam.SetVidType(MEDIA_TYPE_H264);
            tAdaptParam.SetVidActiveType(GetActivePayload(m_tConf, MEDIA_TYPE_H264));
            tAdaptParam.SetBitRate(m_tConf.GetBitRate());
            tAdaptParam.SetFrameRate(m_tConf.GetMainVidUsrDefFPS());
            tAdaptParam.SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());

            //会议群组不需要 第1路适配，临时写为4CIF/H264
            if (0 == byOutIdx)
            {
                u16 wHeight = 0;
                u16 wWidth = 0;
                GetWHByRes(VIDEO_FORMAT_4CIF, wWidth, wHeight);
                tAdaptParam.SetResolution(wWidth, wHeight);
            }
            //会议群组不需要 第2路适配，临时写为CIF/H264
            else
            {
                u16 wHeight = 0;
                u16 wWidth = 0;
                GetWHByRes(VIDEO_FORMAT_CIF, wWidth, wHeight);
                tAdaptParam.SetResolution(wWidth, wHeight);
            }
        }
        cServMsg.CatMsgBody((u8*)&tAdaptParam, sizeof(tAdaptParam));
    }
    
    TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
    cServMsg.CatMsgBody( (u8*)&tEncrypt, sizeof(tEncrypt) );
    
    TSimCapSet tSrcSCS;
    TDStreamCap tDSim;
    
    u8 byBasDecPT = MEDIA_TYPE_NULL;
    u8 byBasAudDecPT = MEDIA_TYPE_NULL;
    
    switch (byChnMode)
    {
    case CHN_ADPMODE_MVBRD:
        GetMVBrdSrcSim(tSrcSCS);
        byBasDecPT = tSrcSCS.GetVideoMediaType();
        byBasAudDecPT = tSrcSCS.GetAudioMediaType();
        break;
    case CHN_ADPMODE_DSBRD:
        GetDSBrdSrcSim(tDSim);
        byBasDecPT = tDSim.GetMediaType();
        break;
    case CHN_ADPMODE_MVSEL:
        GetSelSrcSim(tEqp, byChnId, tSrcSCS);
        byBasDecPT = tSrcSCS.GetVideoMediaType();
        byBasAudDecPT = tSrcSCS.GetAudioMediaType();
        break;
    case CHN_ADPMODE_DSSEL:
        GetSelSrcSim(tEqp, byChnId, tDSim);
        byBasDecPT = tDSim.GetMediaType();
        break;
    }
    
    if (MEDIA_TYPE_NULL == byBasDecPT)
    {
        ConfLog(FALSE, "[ChangeHDAdapt] get byBasDecPT.%d failed!\n", byBasDecPT);
        return FALSE;
    }
    
    TDoublePayload tDVPayload;
    if (MEDIA_TYPE_H264 == byBasDecPT || 
        MEDIA_TYPE_H263PLUS == byBasDecPT || 
        CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() )
    {
        tDVPayload.SetActivePayload(GetActivePayload(m_tConf, byBasDecPT));
    }
    else
    {
        tDVPayload.SetActivePayload(byBasDecPT);
    }
    tDVPayload.SetRealPayLoad(byBasDecPT); 
    cServMsg.CatMsgBody( (u8*)&tDVPayload, sizeof(tDVPayload) );
    cServMsg.SetChnIndex( byChnId );

    if (MEDIA_TYPE_NULL != byBasAudDecPT)
    {
        cServMsg.CatMsgBody((u8*)&byBasAudDecPT, sizeof(u8));
    }
    
    if (bStart)
    {
        SendMsgToEqp( tEqp.GetEqpId(), MCU_HDBAS_STARTADAPT_REQ, cServMsg );
        EqpLog("[StartHDAdapt] BAS.%d Chn.%d: MCU_HDBAS_STARTADAPT_REQ!\n", 
                tEqp.GetEqpId(), byChnId);
        
        //设置定时
        u8 byChnIdx = m_cBasMgr.GetChnId(tEqp, byChnId);
        SetTimer(MCUVC_WAITHDVIDBASRSP_TIMER + byChnIdx, 20*1000);
        
        EqpLog("[StartHDAdapt] CMcuVcInst.%d Set timer %d\n", 
                GetInsID(), MCUVC_WAITHDVIDBASRSP_TIMER+byChnIdx);
    }
    else
    {
        SendMsgToEqp( tVidChn.GetEqpId(), MCU_HDBAS_SETADAPTPARAM_CMD, cServMsg );
        EqpLog("[ChangeHDAdapt] BAS.%d Chn.%d: MCU_HDBAS_SETADAPTPARAM_CMD!\n", 
            tEqp.GetEqpId(), byChnId);
    }

    return TRUE;
}

/*=============================================================================
    函 数 名： StopHDAdapt
    功    能： 
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： BOOL32
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/11/18  4.5		    张宝卿                  创建
    2009/04/17  4.6         张宝卿                  层次整理
=============================================================================*/
BOOL32 CMcuVcInst::StopHDAdapt(const TEqp &tEqp, u8 byChnId)
{
    u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnId);

    if (CHN_ADPMODE_NONE == byChnMode)
    {
        ConfLog(FALSE, "[StopHDAdapt] Bas<%d, %d> unexist in Grp!\n", tEqp.GetEqpId(), byChnId);
        return FALSE;
    }
    CServMsg cServMsg;
    cServMsg.SetChnIndex(byChnId);
    cServMsg.SetConfId(m_tConf.GetConfId());

    SendMsgToEqp(tEqp.GetEqpId(), MCU_HDBAS_STOPADAPT_REQ, cServMsg);
    StopHdBasSwitch(tEqp, byChnId);

    return TRUE;
}

/*=============================================================================
    函 数 名： StartHDCascadeAdp
    功    能： 
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： BOOL32
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/04/13  4.5		    张宝卿                级联回传适配支持
=============================================================================*/
BOOL32 CMcuVcInst::StartHDCascadeAdp(TMt *ptSrcMt)
{
    if (m_tCascadeMMCU.IsNull())
    {
        return FALSE;
    }

    TMt tSrcMt;
    tSrcMt.SetNull();
    if (ptSrcMt != NULL && !ptSrcMt->IsNull())
    {
        tSrcMt = *ptSrcMt;
    }
    else
    {
        TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
        if(ptConfMcInfo == NULL)
        {
            ConfLog(FALSE, "[StartHDCascadeAdp] CasMcu.%d's McInfo is Null!\n", m_tCascadeMMCU.GetMtId());
            return FALSE;
        }
        tSrcMt = ptConfMcInfo->m_tSpyMt;
        if (tSrcMt.IsNull())
        {
            ConfLog(FALSE, "[StartHDCascadeAdp] CasMcu.%d's spymt is null!\n", m_tCascadeMMCU.GetMtId());
            return FALSE;
        }
    }


    TSimCapSet tSrcSimCap = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());
    TSimCapSet tDstSimCap = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());

    //构造适配参数
    CServMsg cServMsg;
    cServMsg.SetMsgBody();
    
    THDAdaptParam tAdaptParam;
    tAdaptParam.SetVidType(tDstSimCap.GetVideoMediaType());
    tAdaptParam.SetVidActiveType(GetActivePayload(m_tConf, tDstSimCap.GetVideoMediaType()));
    tAdaptParam.SetBitRate(m_tConf.GetBitRate());
    tAdaptParam.SetFrameRate(30);
    tAdaptParam.SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());
    
    u16 wHeight = 0;
    u16 wWidth = 0;
    GetWHByRes(tDstSimCap.GetVideoResolution(), wWidth, wHeight);
    tAdaptParam.SetResolution(wWidth, wHeight);
    
    THDBasVidChnStatus tVidChn;
    memset(&tVidChn, 0, sizeof(tVidChn));
    if (MEDIA_TYPE_H264 != tDstSimCap.GetVideoMediaType() ||
        (MEDIA_TYPE_H264 == tDstSimCap.GetVideoMediaType() &&
        VIDEO_FORMAT_CIF == tDstSimCap.GetVideoResolution()))
    {
        tVidChn.SetOutputVidParam(tAdaptParam, 1);
    }
    else
    {
        tVidChn.SetOutputVidParam(tAdaptParam, 0);
    }
    
    //构造消息，整理补齐参数
    u8 byOutIdx = 0;
    for (; byOutIdx < MAXNUM_VOUTPUT; byOutIdx ++)
    {
        tAdaptParam.Reset();
        if (!tVidChn.GetOutputVidParam(byOutIdx)->IsNull())
        {
            memcpy(&tAdaptParam, tVidChn.GetOutputVidParam(byOutIdx), sizeof(tAdaptParam));
        }
        else
        {
            //构造adpparam，保护baphd
            tAdaptParam.SetVidType(MEDIA_TYPE_H264);
            tAdaptParam.SetVidActiveType(GetActivePayload(m_tConf, MEDIA_TYPE_H264));
            tAdaptParam.SetBitRate(m_tConf.GetBitRate());
            tAdaptParam.SetFrameRate(m_tConf.GetMainVidUsrDefFPS());
            tAdaptParam.SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());
            
            //会议群组不需要 第2路适配，临时写为CIF/H264
            if (1 == byOutIdx)
            {
                u16 wHeight = 0;
                u16 wWidth = 0;
                GetWHByRes(VIDEO_FORMAT_CIF, wWidth, wHeight);
                tAdaptParam.SetResolution(wWidth, wHeight);
            }
            //会议群组不需要 第1路适配，临时写为4CIF/H264
            else
            {
                u16 wHeight = 0;
                u16 wWidth = 0;
                GetWHByRes(VIDEO_FORMAT_4CIF, wWidth, wHeight);
                tAdaptParam.SetResolution(wWidth, wHeight);
            }
        }
        cServMsg.CatMsgBody((u8*)&tAdaptParam, sizeof(tAdaptParam));
    }
    
    TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
    cServMsg.CatMsgBody( (u8*)&tEncrypt, sizeof(tEncrypt) );
    
    TDoublePayload tDVPayload;
    if (MEDIA_TYPE_H264 == tSrcSimCap.GetVideoMediaType() || 
        MEDIA_TYPE_H263PLUS == tSrcSimCap.GetVideoMediaType() || 
        CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() )
    {
        tDVPayload.SetActivePayload(GetActivePayload(m_tConf, tSrcSimCap.GetVideoMediaType()));
    }
    else
    {
        tDVPayload.SetActivePayload(tSrcSimCap.GetVideoMediaType());
    }
    tDVPayload.SetRealPayLoad(tSrcSimCap.GetVideoMediaType()); 
    cServMsg.CatMsgBody( (u8*)&tDVPayload, sizeof(tDVPayload) );
    cServMsg.SetChnIndex(m_byCasdVidBasChnnl);
    
    //FIXME: 临时处理，不考虑超时定时. 稍后由BasMgr统一解析分配所有的ReservedBas的ID.
    if (m_tConf.m_tStatus.IsCasdVidAdapting())
    {
        SendMsgToEqp( m_tCasdVidBasEqp.GetEqpId(), MCU_HDBAS_CHANGEPARAM_REQ, cServMsg );
    }
    else
    {
        SendMsgToEqp( m_tCasdVidBasEqp.GetEqpId(), MCU_HDBAS_STARTADAPT_REQ, cServMsg );
    }
    
    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_CASDVID, TRUE);
    
    return TRUE;
}

/*=============================================================================
    函 数 名： StopHDCascaseAdp
    功    能： 
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： BOOL32
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/04/13  4.5		    张宝卿                创建
=============================================================================*/
BOOL32 CMcuVcInst::StopHDCascaseAdp()
{
    if (m_tCasdVidBasEqp.IsNull())
    {
        return FALSE;
    }

    CServMsg cServMsg;
    cServMsg.SetChnIndex(0);
    cServMsg.SetConfId(m_tConf.GetConfId());
    
    SendMsgToEqp(m_tCasdVidBasEqp.GetEqpId(), MCU_HDBAS_STOPADAPT_REQ, cServMsg);

    StopSwitchToPeriEqp(m_tCasdVidBasEqp.GetEqpId(), m_byCasdVidBasChnnl, TRUE, MODE_VIDEO);
    g_cMpManager.RemoveSwitchBridge(m_tCasdVidBasEqp, m_byCasdVidBasChnnl*MAXNUM_VOUTPUT, MODE_VIDEO, TRUE);
    g_cMpManager.RemoveSwitchBridge(m_tCasdVidBasEqp, m_byCasdVidBasChnnl*MAXNUM_VOUTPUT+1, MODE_VIDEO, TRUE);

    StopSwitchToSubMt(m_tCascadeMMCU.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);

    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_CASDVID, FALSE);
    ConfModeChange();

    g_cMcuVcApp.ReleaseHDBasChn(m_tCasdVidBasEqp.GetEqpId(), m_byCasdVidBasChnnl);
    m_tCasdVidBasEqp.SetNull();

    return TRUE;
}

/*=============================================================================
    函 数 名： StartHdVidSelAdp
    功    能： 
    算法实现： 
    全局变量： 
    参    数： BOOL32 bIsVGA
                TEqp &tEqp
                u8 &byChnIdx
    返 回 值： BOOL32 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/5/23   4.5		    张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::StartHdVidSelAdp(TMt tSrc, TMt tDst, u8 bySelMode)
{
    //抢占BAS: FIXME. 目前不支持标清适配
    
    //FIXME: 暂时未支持mpu 和 通用管理，稍后
   
	TMt tOrigSrc = tSrc;
	if (!tSrc.IsLocal())
	{
		tSrc = GetLocalMtFromOtherMcuMt(tSrc);
	}
	if (!tDst.IsLocal())
	{
		tDst = GetLocalMtFromOtherMcuMt(tDst);
	}

	u8 byBasId  = 0xff;
	u8 byChnIdx = 0xff;
	// 查询是否之前已有同源终端占用BAS,并且输出的码流可为新加入的终端所接受
	BOOL32 byNeedNewBas = IsNeedNewSelAdp(tSrc.GetMtId(), tDst.GetMtId(), MODE_VIDEO, &byBasId, &byChnIdx);
	if (byNeedNewBas)
	{
		BOOL32 bRet = g_cMcuVcApp.GetIdleHDBasVidChl( byBasId, byChnIdx);
		if ( bRet )
		{
			TPeriEqpStatus tHDBasStatus;
			g_cMcuVcApp.GetPeriEqpStatus( byBasId, &tHDBasStatus );
			THDBasVidChnStatus* ptChanStatus = NULL;
			if (TYPE_MPU == tHDBasStatus.m_tStatus.tHdBas.GetEqpType())
			{
				ptChanStatus = tHDBasStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus( byChnIdx );
			}
			else
			{
				ptChanStatus = tHDBasStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus( byChnIdx );
			}

			if ( NULL != ptChanStatus )
			{
				TEqp tHDBas;
				tHDBas = ptChanStatus->GetEqp();
				tHDBas.SetConfIdx( m_byConfIdx );
				ptChanStatus->SetEqp( tHDBas );
				g_cMcuVcApp.SetPeriEqpStatus(byBasId, &tHDBasStatus);
				g_cMcuVcApp.SendPeriEqpStatusToMcs(byBasId);
				EqpLog( "[StartHdVidSelAdp] Occupy HDBas.%d, chn.%d\n", byBasId, byChnIdx );

				//根据源和目的准备参数，开启适配
				CServMsg cServMsg;
				TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
				TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS(tDst.GetMtId());

				u8 abyMediaType[MAXNUM_VOUTPUT] = {MEDIA_TYPE_NULL, MEDIA_TYPE_NULL};
				u8 abyRes[MAXNUM_VOUTPUT]       = {VIDEO_FORMAT_CIF, VIDEO_FORMAT_CIF};
				if (MEDIA_TYPE_H264 == tDstSCS.GetVideoMediaType())
				{
					abyMediaType[0] = MEDIA_TYPE_H264;
					abyMediaType[1] = MEDIA_TYPE_H264;
					if (VIDEO_FORMAT_CIF == tDstSCS.GetVideoResolution())
					{
						abyRes[0] = VIDEO_FORMAT_4CIF;
						abyRes[1] = VIDEO_FORMAT_CIF;
					}
					else
					{
						abyRes[0] = tDstSCS.GetVideoResolution();
						abyRes[1] = VIDEO_FORMAT_CIF;
					}

				}
				else
				{
					if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() ||
						MEDIA_TYPE_H264 == m_tConf.GetSecVideoMediaType())
					{
						abyMediaType[0] = MEDIA_TYPE_H264;
						abyRes[0]       = VIDEO_FORMAT_4CIF;
					}
					abyMediaType[1] = tDstSCS.GetVideoMediaType();
					abyRes[1]       = tDstSCS.GetVideoResolution();
				}

				u16 wOutBitRate = m_tConf.GetBitRate();
				if (m_tConf.GetSecBitRate() != 0)
				{
					wOutBitRate = min(m_tConf.GetSecBitRate(), wOutBitRate);
				}

				EqpLog("[StartHdVidSelAdp] mtid:%d sel mtid:%d using hdbas(firstout:(%d, %d) secout:(%d, %d) bitrate:(%d)\n", 
						tDst.GetMtId(), tSrc.GetMtId(), abyMediaType[0], abyRes[0], abyMediaType[1], abyRes[1], wOutBitRate);


				//构造适配目的参数
				THDAdaptParam tHDAdpParam;
				u16 wWidth , wHeight = 0;
				u8 byFrameRate = 0;
				for (u8 byIdx = 0; byIdx < MAXNUM_VOUTPUT; byIdx++)
				{
					tHDAdpParam.SetVidType(abyMediaType[byIdx]);
					tHDAdpParam.SetBitRate(wOutBitRate);
					tHDAdpParam.SetVidActiveType(GetActivePayload(m_tConf, abyMediaType[byIdx]));    

					GetWHByRes(abyRes[byIdx], wWidth, wHeight);
					tHDAdpParam.SetResolution(wWidth, wHeight);

					if (MEDIA_TYPE_H264 == abyMediaType[byIdx])
					{
						byFrameRate = ((abyMediaType[byIdx] == m_tConf.GetSecVideoMediaType()) ? m_tConf.GetSecVidUsrDefFPS() : m_tConf.GetMainVidUsrDefFPS());
					}
					else
					{
						byFrameRate = ((abyMediaType[byIdx] == m_tConf.GetSecVideoMediaType()) ? m_tConf.GetSecVidFrameRate() : m_tConf.GetMainVidFrameRate());
					}
					tHDAdpParam.SetFrameRate(byFrameRate);

					tHDAdpParam.SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());    
					cServMsg.CatMsgBody((u8*)&tHDAdpParam, sizeof(tHDAdpParam));
					ptChanStatus->SetOutputVidParam(tHDAdpParam, byIdx);	
				}

				TMediaEncrypt  tEncrypt = m_tConf.GetMediaKey();
				cServMsg.CatMsgBody( (u8*)&tEncrypt, sizeof(tEncrypt) );

				TDoublePayload tDVPayload;
				if (MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() || 
					MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() || 
					CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() )
				{
					tDVPayload.SetActivePayload(GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType()));
				}
				else
				{
					tDVPayload.SetActivePayload(tSrcSCS.GetVideoMediaType());
				}
				tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType()); 
				cServMsg.CatMsgBody( (u8*)&tDVPayload, sizeof(tDVPayload) );
				cServMsg.SetChnIndex( byChnIdx );

				SendMsgToEqp( byBasId, MCU_HDBAS_STARTADAPT_REQ, cServMsg );

				EqpLog("[StartHdVidSelAdp] CMcuVcInst: BAS.%d Chn.%d start adp!\n", byBasId, byChnIdx);


				//简单起见，事先建交换，4.6.1规范处理
				TEqp tHDAdpt = g_cMcuVcApp.GetEqp(byBasId);
				tHDAdpt.SetConfIdx(m_byConfIdx);
				g_cMpManager.SetSwitchBridge(tSrc, 0, MODE_VIDEO);
				g_cMpManager.SetSwitchBridge(tHDAdpt, byChnIdx * 2, MODE_VIDEO, TRUE);
				g_cMpManager.SetSwitchBridge(tHDAdpt, byChnIdx * 2 + 1, MODE_VIDEO, TRUE);
				//选看适配不等启动回应,简单将通道变为正在适配中的状态
				ptChanStatus->SetStatus(TBasChnStatus::RUNING);
				g_cMcuVcApp.SetPeriEqpStatus(byBasId, &tHDBasStatus);
				g_cMcuVcApp.SendPeriEqpStatusToMcs(byBasId);

				g_cMpManager.StartSwitchToPeriEqp(tSrc, 0, tHDAdpt.GetEqpId(), byChnIdx, MODE_VIDEO, SWITCH_MODE_SELECT,
												  FALSE, TRUE, (VCS_CONF == m_tConf.GetConfSource()));
			}
			else
			{
				ConfLog( FALSE, "[StartHdVidSelAdp] Get HDBas.%d Chan.%d status failed!\n", byBasId, byChnIdx );
				return FALSE;
			}
		}
		else
		{
			ConfLog(FALSE, "[StartHdVidSelAdp] no Idle HdBas to ocuppy!\n");
			return FALSE;
		}
	}
    
    //抢到通道，写入SelGrp
    TEqp tAdpBas = g_cMcuVcApp.GetEqp(byBasId);
	tAdpBas.SetConfIdx(m_byConfIdx);
    m_cSelChnGrp.AddSel(tSrc.GetMtId(), tDst.GetMtId(), MODE_VIDEO, tAdpBas, byChnIdx);


    //目的非H264或264 cif，接第二出；其余均走第一出
	TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS(tDst.GetMtId());
    if (MEDIA_TYPE_H264 != tDstSCS.GetVideoMediaType() ||
		(MEDIA_TYPE_H264 == tDstSCS.GetVideoMediaType() && VIDEO_FORMAT_CIF == tDstSCS.GetVideoResolution()))
    {
        g_cMpManager.StartSwitchToSubMt(tAdpBas, byChnIdx * 2 + 1, tDst, MODE_VIDEO);
    }
    else
    {
        g_cMpManager.StartSwitchToSubMt(tAdpBas, byChnIdx * 2, tDst, MODE_VIDEO);
    }
    
    //选看标识置位
    m_ptMtTable->SetMtSrc(tDst.GetMtId(), &tOrigSrc, MODE_VIDEO);

    //通知发送
    NotifyMtSend(tSrc.GetMtId(), MODE_VIDEO);
    NotifyMtReceive(tSrc, tDst.GetMtId());
    NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);

    //非kdc厂商有广播源且节省带宽时，通知mt停止发送码流
//     if(g_cMcuVcApp.IsSavingBandwidth() && 
//         MT_MANU_KDC != m_ptMtTable->GetManuId( tDst.GetMtId()) &&
//         MT_MANU_KDCMCU != m_ptMtTable->GetManuId( tDst.GetMtId()))
//     {
//         NotifyOtherMtSend(tDst.GetMtId(), FALSE);
//     }

    //刷新界面
    TMtStatus tMtStatus;

    //过滤目标终端为上级mcu的情况
    m_ptMtTable->GetMtStatus( tDst.GetMtId(), &tMtStatus );
    if( tSrc.GetType() == TYPE_MT && tSrc.GetMtId() != tDst.GetMtId() && 
        (m_tCascadeMMCU.GetMtId() == 0 || tDst.GetMtId() != m_tCascadeMMCU.GetMtId()) )
    {
        TMtStatus tSrcMtStatus;
        u8 byAddSelByMcsMode = MODE_NONE;
        m_ptMtTable->GetMtStatus( tSrc.GetMtId(), &tSrcMtStatus );
        
        //zbq[12/06/2007] VMP点名时序问题
        if( ((!(tSrc == m_tVidBrdSrc)) || (tSrc == m_tVidBrdSrc && 
            ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() &&
            m_tRollCaller == m_tVidBrdSrc)) &&
            tSrcMtStatus.IsSendVideo() && 
            ( MODE_VIDEO == bySelMode || MODE_BOTH == bySelMode ) )
        {
            byAddSelByMcsMode = MODE_VIDEO;
        }
        if( !(tSrc == m_tAudBrdSrc) && tSrcMtStatus.IsSendAudio() && 
            ( MODE_AUDIO == bySelMode || MODE_BOTH == bySelMode ) )
        {
            if( MODE_VIDEO == byAddSelByMcsMode )
            {
                byAddSelByMcsMode = MODE_BOTH;
            }
            else
            {
                byAddSelByMcsMode = MODE_AUDIO;
            }
        }
        
        if( MODE_NONE != byAddSelByMcsMode )
        {
            tMtStatus.AddSelByMcsMode( bySelMode );
        }
        else
        {
            tMtStatus.RemoveSelByMcsMode( bySelMode );
        }
    }
    else
    {
        tMtStatus.RemoveSelByMcsMode( bySelMode );
    }
	tMtStatus.SetSelectMt(tOrigSrc, bySelMode);
    m_ptMtTable->SetMtStatus( tDst.GetMtId(), &tMtStatus );    
    MtStatusChange(tDst.GetMtId(), TRUE);

    //暂不支持PRS
    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
    }

    return TRUE;
}


/*=============================================================================
    函 数 名： StopHdVidSelAdp
    功    能： 
    算法实现： 
    全局变量： 
    参    数：  TMt &tSrc
                TMt &tDst
                u8 bySelMode
    返 回 值： BOOL32 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/5/23   4.5		    张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::StopHdVidSelAdp(TMt tSrc, TMt tDst, u8 bySelMode)
{
    TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
    TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS(tDst.GetMtId());

	if (!tSrc.IsLocal())
	{
		tSrc = GetLocalMtFromOtherMcuMt(tSrc);
	}
	if (!tDst.IsLocal())
	{
		tDst = GetLocalMtFromOtherMcuMt(tDst);
	}

    //非适配选看
    TEqp tBas;
    tBas.SetNull();
    u8 byChnIdx = 0;
    BOOL32 bRlsChn = FALSE;
    BOOL32 bGetAdpChn = FALSE;
    bGetAdpChn = m_cSelChnGrp.GetSelBasChn(tSrc.GetMtId(), tDst.GetMtId(), MODE_VIDEO, tBas, byChnIdx, bRlsChn);
    if (!bGetAdpChn)
    {
        ConfLog(FALSE, "[StopHdVidSelAdp] get adp chn failed for src.%d, dst.%d, mode.%d!\n",
                        tSrc.GetMtId(), tDst.GetMtId(), bySelMode);
        return FALSE;
    }
    else
    {
        EqpLog("[StopHdVidSelAdp] Chn<%d,%d> for sel<src.%d,dst.%d>, mode.%d, Rls.%d has been checked out!\n",
                tBas.GetEqpId(), byChnIdx, tSrc.GetMtId(), tDst.GetMtId(), bySelMode, bRlsChn);
    }

    //拆交换
    TEqp tHDAdpt = g_cMcuVcApp.GetEqp(tBas.GetEqpId());
  
	if (bRlsChn)
	{
		g_cMpManager.RemoveSwitchBridge(tSrc, 0, MODE_VIDEO);
		g_cMpManager.RemoveSwitchBridge(tHDAdpt, 0, MODE_VIDEO);
		g_cMpManager.RemoveSwitchBridge(tHDAdpt, 1, MODE_VIDEO);    
		g_cMpManager.StopSwitchToPeriEqp(m_byConfIdx, tHDAdpt.GetEqpId(), byChnIdx, MODE_VIDEO);

        g_cMcuVcApp.ReleaseHDBasChn(tBas.GetEqpId(), byChnIdx);
		StopHDAdapt(tBas, byChnIdx);
	}

    g_cMpManager.StopSwitchToSubMt(tDst, MODE_VIDEO);
    
    //选看标识复位
    TMt tMtNull;
    tMtNull.SetNull();

	TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus(tDst.GetMtId(), &tMtStatus);
    tMtStatus.RemoveSelByMcsMode(bySelMode);
	tMtStatus.SetSelectMt(tMtNull, bySelMode);
	m_ptMtTable->SetMtStatus(tDst.GetMtId(), &tMtStatus);

    m_ptMtTable->SetMtSrc(tDst.GetMtId(), &tMtNull, bySelMode);

    //交换恢复
    RestoreRcvMediaBrdSrc(tDst.GetMtId(), bySelMode, TRUE);

    
    //暂不支持PRS
    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
    }

    //清除关联项
    if (!m_cSelChnGrp.RemoveSel(tSrc.GetMtId(), tDst.GetMtId(), MODE_VIDEO))
    {
        ConfLog(FALSE, "[StopHdVidSelAdp] remove sel<src.%d, dst.%d> failed!\n", tSrc.GetMtId(), tDst.GetMtId());
    }
    
    return TRUE;
}
/*=============================================================================
    函 数 名： ReleaseResbySel
    功    能： 
    算法实现： 
    全局变量： 
    参    数： TMt tSelDstMt
    返 回 值： void 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/5/23   4.5		    付秀华                  创建
=============================================================================*/
void CMcuVcInst::ReleaseResbySel(TMt tSelDstMt, u8 bySelMode)
{
	if (!tSelDstMt.IsLocal())
	{
		tSelDstMt = GetLocalMtFromOtherMcuMt(tSelDstMt);
	}
	TMtStatus tStatus;
	m_ptMtTable->GetMtStatus(tSelDstMt.GetMtId(), &tStatus);
	if (MODE_VIDEO == bySelMode || MODE_BOTH == bySelMode)
	{
		TMt tVSelMt = tStatus.GetSelectMt(MODE_VIDEO);
		if (!tVSelMt.IsNull() && !tVSelMt.IsLocal())
		{
			tVSelMt = GetLocalMtFromOtherMcuMt(tVSelMt);
		}
		if (!tVSelMt.IsNull() &&
			IsNeedSelApt(tVSelMt.GetMtId(), tSelDstMt.GetMtId(), MODE_VIDEO))
		{
			StopHdVidSelAdp(tVSelMt, tSelDstMt, MODE_VIDEO);
		}
	}

	if (MODE_AUDIO == bySelMode || MODE_BOTH == bySelMode)
	{
		TMt tASelMt = tStatus.GetSelectMt(MODE_AUDIO);
		if (!tASelMt.IsNull() && !tASelMt.IsLocal())
		{
			tASelMt = GetLocalMtFromOtherMcuMt(tASelMt);
		}

		if (!tASelMt.IsNull() &&
			IsNeedSelApt(tASelMt.GetMtId(), tSelDstMt.GetMtId(), MODE_AUDIO))
		{
			StopAudSelAdp(tASelMt, tSelDstMt);
		}	
	}

}
/*=============================================================================
    函 数 名： StartAudSelAdp
    功    能： 
    算法实现： 
    全局变量： 
    参    数：
    返 回 值： BOOL32 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/5/23   4.5		    付秀华                  创建
=============================================================================*/
BOOL32 CMcuVcInst::StartAudSelAdp(TMt tSrc, TMt tDst)
{  
	TMt tOrigSrc = tSrc;
	if (!tSrc.IsLocal())
	{
		tSrc = GetLocalMtFromOtherMcuMt(tSrc);
	}
	if (!tDst.IsLocal())
	{
		tDst = GetLocalMtFromOtherMcuMt(tDst);
	}

	u8 byBasId  = 0xff;
	u8 byChnIdx = 0xff;
	// 查询是否之前已有同源终端占用BAS,并且输出的码流可为新加入的终端所接受
	BOOL32 byNeedNewBas = IsNeedNewSelAdp(tSrc.GetMtId(), tDst.GetMtId(), MODE_AUDIO, &byBasId, &byChnIdx);
	if (byNeedNewBas)
	{
		// 目前音频适配只使用标清适配器
		BOOL32 bRet = g_cMcuVcApp.GetIdleBasChl(ADAPT_TYPE_AUD, byBasId, byChnIdx);
		if ( bRet )
		{
			TPeriEqpStatus tBasStatus;
			g_cMcuVcApp.GetPeriEqpStatus( byBasId, &tBasStatus );
			TBasChnStatus tBasChnStatus = tBasStatus.m_tStatus.tBas.tChnnl[byChnIdx];
			//占用
			tBasChnStatus.SetStatus(TBasChnStatus::RUNING);
			g_cMcuVcApp.SetPeriEqpStatus(byBasId, &tBasStatus);	
			g_cMcuVcApp.SendPeriEqpStatusToMcs(byBasId);
			EqpLog( "[StartAudSelAdp] Occupy HDBas.%d, chn.%d\n", byBasId, byChnIdx );

			//根据源和目的准备参数，开启适配
			TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
			TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS(tDst.GetMtId());

			TAdaptParam tAdaptParam;
			tAdaptParam.Clear();
			tAdaptParam.SetVidType(MEDIA_TYPE_NULL);
		    tAdaptParam.SetAudType(tDstSCS.GetAudioMediaType());
			TMediaEncrypt  tEncrypt = m_tConf.GetMediaKey();
			TDoublePayload tDVPayload;
			TDoublePayload tDAPayload;
			if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode())
			{
				tAdaptParam.SetAudActiveType(GetActivePayload(m_tConf, tDstSCS.GetAudioMediaType()));
				tDAPayload.SetActivePayload(GetActivePayload(m_tConf, tDstSCS.GetAudioMediaType()));
			}
			else
			{
				tAdaptParam.SetAudActiveType(tDstSCS.GetAudioMediaType());
				tDAPayload.SetActivePayload(tDstSCS.GetAudioMediaType());
			}
            tDAPayload.SetRealPayLoad(tDstSCS.GetAudioMediaType());
			tAdaptParam.SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());

			CServMsg cServMsg;							
			cServMsg.SetChnIndex(byChnIdx);
			cServMsg.SetConfId(m_tConf.GetConfId());
			cServMsg.SetMsgBody((u8 *)&tAdaptParam, sizeof(TAdaptParam));
			cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //video
			cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload)); //dvideopaload
			cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));     //audio
			cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload)); //dApayload 
    		TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
			cServMsg.CatMsgBody((u8 *)&tCapSupportEx, sizeof(tCapSupportEx));	//FECType
			SendMsgToEqp(byBasId, MCU_BAS_STARTADAPT_REQ, cServMsg);
			EqpLog("[StartAudSelAdp] CMcuVcInst: BAS.%d Chn.%d start adp!\n", byBasId, byChnIdx);


			//简单起见，事先建交换，4.6.1规范处理
			TEqp tHDAdpt = g_cMcuVcApp.GetEqp(byBasId);
			tHDAdpt.SetConfIdx(m_byConfIdx);
			g_cMpManager.SetSwitchBridge(tSrc, 0, MODE_AUDIO);
			g_cMpManager.SetSwitchBridge(tHDAdpt, byChnIdx, MODE_AUDIO);
			g_cMpManager.StartSwitchToPeriEqp(tSrc, 0, tHDAdpt.GetEqpId(), byChnIdx, MODE_AUDIO, SWITCH_MODE_SELECT,
											  FALSE, TRUE, (VCS_CONF == m_tConf.GetConfSource()));
		}
		else
		{
			ConfLog(FALSE, "[StartAudSelAdp] no Idle HdBas to ocuppy!\n");
			return FALSE;
		}
	}
    
    //抢到通道，写入SelGrp
    TEqp tAdpBas = g_cMcuVcApp.GetEqp(byBasId);
	tAdpBas.SetConfIdx(m_byConfIdx);
    m_cSelChnGrp.AddSel(tSrc.GetMtId(), tDst.GetMtId(), MODE_AUDIO, tAdpBas, byChnIdx);

    g_cMpManager.StartSwitchToSubMt(tAdpBas, byChnIdx, tDst, MODE_AUDIO);
    
    //选看标识置位
    m_ptMtTable->SetMtSrc(tDst.GetMtId(), &tOrigSrc, MODE_AUDIO);

    //通知发送
    NotifyMtSend(tSrc.GetMtId(), MODE_AUDIO);
    NotifyMtReceive(tSrc, tDst.GetMtId());
    NotifyFastUpdate(tSrc, MODE_AUDIO, TRUE);

    TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus( tDst.GetMtId(), &tMtStatus );      
	tMtStatus.SetSelectMt(tOrigSrc, MODE_AUDIO);
    m_ptMtTable->SetMtStatus( tDst.GetMtId(), &tMtStatus );    
    MtStatusChange(tDst.GetMtId(), TRUE);

    return TRUE;
}
/*=============================================================================
    函 数 名： StopAudSelAdp
    功    能： 
    算法实现： 
    全局变量： 
    参    数：
    返 回 值： BOOL32 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/5/23   4.5		    付秀华                  创建
=============================================================================*/
BOOL32 CMcuVcInst::StopAudSelAdp(TMt tSrc, TMt tDst)
{
    TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
    TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS(tDst.GetMtId());

	if (!tSrc.IsLocal())
	{
		tSrc = GetLocalMtFromOtherMcuMt(tSrc);
	}
	if (!tDst.IsLocal())
	{
		tDst = GetLocalMtFromOtherMcuMt(tDst);
	}

    //非适配选看
    TEqp tBas;
    tBas.SetNull();
    u8 byChnIdx = 0;
    BOOL32 bRlsChn = FALSE;
    BOOL32 bGetAdpChn = FALSE;
    bGetAdpChn = m_cSelChnGrp.GetSelBasChn(tSrc.GetMtId(), tDst.GetMtId(), MODE_AUDIO, tBas, byChnIdx, bRlsChn);
    if (!bGetAdpChn)
    {
        ConfLog(FALSE, "[StopAudSelAdp] get adp chn failed for src.%d, dst.%d!\n",
                        tSrc.GetMtId(), tDst.GetMtId());
        return FALSE;
    }
    else
    {
        EqpLog("[StopAudSelAdp] Chn<%d,%d> for sel<src.%d,dst.%d>, Rls.%d has been checked out!\n",
                tBas.GetEqpId(), byChnIdx, tSrc.GetMtId(), tDst.GetMtId(), bRlsChn);
    }

    //拆交换
    TEqp tHDAdpt = g_cMcuVcApp.GetEqp(tBas.GetEqpId());
  
	if (bRlsChn)
	{
		g_cMpManager.RemoveSwitchBridge(tSrc, 0, MODE_AUDIO);
		g_cMpManager.RemoveSwitchBridge(tHDAdpt, byChnIdx, MODE_AUDIO);
		g_cMpManager.StopSwitchToPeriEqp(m_byConfIdx, tHDAdpt.GetEqpId(), byChnIdx, MODE_AUDIO);

		TPeriEqpStatus tBasStatus;
		g_cMcuVcApp.GetPeriEqpStatus( tHDAdpt.GetEqpId(), &tBasStatus );
		TBasChnStatus tBasChnStatus = tBasStatus.m_tStatus.tBas.tChnnl[byChnIdx];
		tBasChnStatus.SetStatus(TBasChnStatus::READY);
		tBasChnStatus.SetReserved(FALSE);
		tBasStatus.m_tStatus.tBas.tChnnl[byChnIdx] = tBasChnStatus;
		g_cMcuVcApp.SetPeriEqpStatus(tHDAdpt.GetEqpId(), &tBasStatus);	
		g_cMcuVcApp.SendPeriEqpStatusToMcs(tHDAdpt.GetEqpId());

		CServMsg cServMsg;
		cServMsg.SetChnIndex(byChnIdx);
		cServMsg.SetConfId(m_tConf.GetConfId());
		SendMsgToEqp(tHDAdpt.GetEqpId(), MCU_BAS_STOPADAPT_REQ, cServMsg);
	}

    g_cMpManager.StopSwitchToSubMt(tDst, MODE_AUDIO);
    
    //选看标识复位
    TMt tMtNull;
    tMtNull.SetNull();

	TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus(tDst.GetMtId(), &tMtStatus);
    tMtStatus.RemoveSelByMcsMode(MODE_AUDIO);
	tMtStatus.SetSelectMt(tMtNull, MODE_AUDIO);
	m_ptMtTable->SetMtStatus(tDst.GetMtId(), &tMtStatus);

    m_ptMtTable->SetMtSrc(tDst.GetMtId(), &tMtNull, MODE_AUDIO);

    //交换恢复
    RestoreRcvMediaBrdSrc(tDst.GetMtId(), MODE_AUDIO, TRUE);
    
   //清除关联项
    if (!m_cSelChnGrp.RemoveSel(tSrc.GetMtId(), tDst.GetMtId(), MODE_AUDIO))
    {
        ConfLog(FALSE, "[StopAudSelAdp] remove sel<src.%d, dst.%d> failed!\n", tSrc.GetMtId(), tDst.GetMtId());
    }
    
    return TRUE;
}
/*------------------------------------------------------------------*/
/*                                 Mixer                            */
/*------------------------------------------------------------------*/


/*====================================================================
    函数名      ：StartMixing
    功能        ：开始混音
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMixMode 混音模式
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         胡昌威         创建
====================================================================*/
BOOL32 CMcuVcInst::StartMixing( u8 byMixMode )
{
    u8 byEqpId = 0;
	u8 byGrpId = 0;
	u8 byAudioType = m_tConf.GetMainAudioMediaType();
    u8 byAudioType2 = m_tConf.GetSecAudioMediaType();
	TPeriEqpStatus tPeriEqpStatus;

	//得到空闲的混音器
    byEqpId = g_cMcuVcApp.GetIdleMixGroup( byEqpId, byGrpId );
	if( byEqpId == 0 )
	{
		return FALSE;//无可用的混音器
	}
	m_tMixEqp = g_cMcuVcApp.GetEqp( byEqpId );
	m_tMixEqp.SetConfIdx( m_byConfIdx );
	m_byMixGrpId = byGrpId;

	//占用混音器
	g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tPeriEqpStatus );
    // guzh [11/9/2007] 
	tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[byGrpId].m_byGrpMixDepth = GetMaxMixerDepth();
    u8 byMixerGrpState = TMixerGrpStatus::WAIT_BEGIN;
    switch (byMixMode)
    {
    case mcuPartMix:
        byMixerGrpState = TMixerGrpStatus::WAIT_START_SPECMIX;
        break;
    case mcuWholeMix:
        byMixerGrpState = TMixerGrpStatus::WAIT_START_AUTOMIX;
        break;
    case mcuVacMix:
        byMixerGrpState = TMixerGrpStatus::WAIT_START_VAC;
        break;
    default:
        break;
    }

    //先占用,超时后未成功再放弃
	tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[byGrpId].m_byGrpState = byMixerGrpState;
    EqpLog("[StartMixing] Eqp.%d ConfIdx.%d GrpState.%d\n",
            m_tMixEqp.GetEqpId(), m_byConfIdx, byMixerGrpState);

	g_cMcuVcApp.SetPeriEqpStatus( byEqpId, &tPeriEqpStatus );

	TMixerStart tMixer;
	tMixer.SetAudioEncrypt(m_tConf.GetMediaKey());
	tMixer.SetAudioMode(byAudioType);
    tMixer.SetSecAudioMode(byAudioType2);
//	tMixer.SetIsAllMix(!m_tConf.m_tStatus.IsMixSpecMt());
    tMixer.SetIsAllMix(TRUE); // xsl [8/24/2006] 新的定制混音方案不用再通知当前参与混音的通道
	tMixer.SetMixGroupId(byGrpId);
    //zbq [11/22/2007] 给当前的非空混音深度
    tMixer.SetMixDepth(GetMaxMixerDepth());

    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
        tMixer.SetIsNeedByPrs(TRUE);
    }
    else
    {
        tMixer.SetIsNeedByPrs(FALSE);
    }

	TDoublePayload tDPayload;
	tDPayload.SetRealPayLoad(byAudioType);
	tDPayload.SetActivePayload(GetActivePayload(m_tConf, byAudioType));

	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetMsgBody( (u8*)&tMixer, sizeof(tMixer) );
	cServMsg.CatMsgBody( (u8*)&tDPayload, sizeof(tDPayload));

	// MCU前向纠错, zgc, 2007-09-27
	TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
	cServMsg.CatMsgBody( (u8*)&tCapSupportEx, sizeof(tCapSupportEx) );

	//给混音器发开始消息
	SendMsgToEqp( byEqpId, MCU_MIXER_STARTMIX_REQ, cServMsg );

	EqpLog( "[MCU_MIXER_STARTMIX_REQ] AudioType.%d!\n", byAudioType );

	//设置等待应答时钟
	SetTimer( MCUVC_MIX_WAITMIXERRSP_TIMER, 6*1000 );

	return TRUE;
}

/*====================================================================
    函数名      ：StopMixing
    功能        ：停止混音
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::StopMixing(void)
{
	CServMsg cServMsg;
	cServMsg.SetConfId(m_tConf.GetConfId());

	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (m_tConfAllMtInfo.MtJoinedConf(byMtId))            
		{
            // guzh [11/7/2007] 
			// fxh[09/06/04]等待混音器响应到来后再清空状态
			// 否则响应未来前,挂断参与混音的终端,将导致终端到混音器的交换未拆
//             m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);
// 			if (m_ptMtTable->IsMtInMixing(byMtId))
// 			{
// 				m_ptMtTable->SetMtInMixing(byMtId, FALSE);
// 				u8 byMix = 0;
// 				cServMsg.SetMsgBody((u8*)&byMix, 1);
// 				SendMsgToMt(byMtId, MCU_MT_MTADDMIX_NOTIF, cServMsg);
// 			}

			if (m_tConf.GetConfAttrb().IsResendLosePack())
			{
				u8  byChlNum;
				u16 wRtcpSwitchPort;
				TTransportAddr  tAddr;
				TTransportAddr  tEqpAddr;
				TLogicalChannel tLogicalChannel;
				TLogicalChannel tEqpLogicalChannel;

				m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_AUDIO, &tLogicalChannel, FALSE);

				g_cMcuVcApp.GetPeriEqpLogicChnnl(m_tMixEqp.GetEqpId(), MODE_AUDIO, &byChlNum, &tEqpLogicalChannel, TRUE);

				//移除下行Rtcp反馈交换
				
				tAddr = tLogicalChannel.GetRcvMediaCtrlChannel();
				
				tEqpAddr = tEqpLogicalChannel.GetRcvMediaCtrlChannel();
				
				wRtcpSwitchPort = (tEqpAddr.GetPort() - 2) + 
					              MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId + PORTSPAN*GetMixChnPos(byMtId) + 4;

				g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, tEqpAddr.GetIpAddr(), wRtcpSwitchPort);


				//移除上行Rtcp反馈交换
				tAddr = tLogicalChannel.GetSndMediaCtrlChannel();

				g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, tAddr.GetIpAddr(), tAddr.GetPort());

			}
		}
	}

    // guzh [11/9/2007]  设置等待
    u8 byEqpId = m_tMixEqp.GetEqpId();    
    TPeriEqpStatus tPeriEqpStatus;
    g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tPeriEqpStatus );
    tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = TMixerGrpStatus::WAIT_STOP;
    g_cMcuVcApp.SetPeriEqpStatus( byEqpId, &tPeriEqpStatus );

	cServMsg.SetMsgBody((u8 *)&m_byMixGrpId, 1);
	SendMsgToEqp(m_tMixEqp.GetEqpId(), MCU_MIXER_STOPMIX_REQ, cServMsg);

    // guzh [11/9/2007] 
    SetTimer(MCUVC_MIX_WAITMIXERRSP_TIMER, 6*1000);

	EqpLog("Mixer stop working\n");
	return;
}

/*====================================================================
    函数名      ：SwitchMixMember
    功能        ：混音器交换码流给终端
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt *ptMt 成员终端
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/05/31    3.6         LIBO          创建
    06/08/04    4.0         xsl           增加N模式交换
====================================================================*/
void CMcuVcInst::SwitchMixMember(const TMt *ptMt, BOOL32 bNMode/*=false*/)
{
    //是否在音频环回
    TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus(ptMt->GetMtId(), &tMtStatus);
    if (tMtStatus.IsMediaLoop(MODE_AUDIO))
    {
        return;
    }

    // xsl [8/19/2006] N模式分散会议通知终端改变接收地址和端口为组播
    if (bNMode && m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        ChangeSatDConfMtRcvAddr(ptMt->GetMtId(), LOGCHL_AUDIO);
        return;
    }
     
	TTransportAddr  tAddr;
	TLogicalChannel tLogicalChannel;
	if (m_ptMtTable->GetMtLogicChnnl(ptMt->GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE))
	{
		tAddr = tLogicalChannel.GetRcvMediaChannel();
	}
	else
	{
		return;
	}

	u8  byMixerId = m_tMixEqp.GetEqpId();
	u32 dwSwitchIpAddr;
	u16 wSwitchPort;
	u32 dwMtSwitchIp;
	u16 wMtSwitchPort;
	u32 dwMtSrcIp;

	g_cMpManager.GetMixerSwitchAddr(byMixerId, dwSwitchIpAddr, wSwitchPort);
	g_cMpManager.GetSwitchInfo(*ptMt, dwMtSwitchIp, wMtSwitchPort, dwMtSrcIp);

	//这里应考虑同一混音器存在多个混音组的情况，源接收端口加以区分
    if (bNMode)
    {
        wSwitchPort = wSwitchPort + MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId + 2;
    }
    else
    {
        wSwitchPort = wSwitchPort + MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId + PORTSPAN*GetMixChnPos(ptMt->GetMtId()) + 2;
    }
	
	//语音激励时，不用把码流交换回来
    // xsl [7/29/2006] N-1模式只将加入混音组的终端交换回来
	if (m_tConf.m_tStatus.IsMixing() && 
        (m_ptMtTable->IsMtInMixGrp(ptMt->GetMtId())&&!bNMode || bNMode))
	{
        u32 dwRcvIp, dwSrcIp;
        u16 wRcvPort;        
        //得到交换源地址
        if( !g_cMpManager.GetSwitchInfo( m_tMixEqp, dwRcvIp, wRcvPort, dwSrcIp ) )
        {
            EqpLog("SwitchMixMember() get switch info failed!");
        }
        
		TMt tSrcMt;
		tSrcMt.SetNull();
		g_cMpManager.StartSwitch(tSrcMt, m_byConfIdx, dwSrcIp, 0, 
								 dwMtSwitchIp, wSwitchPort, tAddr.GetIpAddr(), tAddr.GetPort());

        // guzh [8/31/2006] 设置终端的音频源是混音器
        m_ptMtTable->SetMtSrc( ptMt->GetMtId(), &m_tMixEqp, MODE_AUDIO );
        // 清除选看状态
        m_ptMtTable->GetMtStatus(ptMt->GetMtId(), &tMtStatus);
        tMtStatus.RemoveSelByMcsMode(MODE_AUDIO);
        m_ptMtTable->SetMtStatus(ptMt->GetMtId(), &tMtStatus);
	}

	return;
}

/*====================================================================
    函数名      ：AddMixMember
    功能        ：增加混音成员
    算法实现    ：通知mixer增加通道，非vac模式将mixer码流交换给mt，建上下行交换
    引用全局变量：
    输入参数说明：const TMt *ptMt 成员终端
                  u8 byVolume 成员音量
                  BOOL32 bForce 是否是强制 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         胡昌威         创建
====================================================================*/
BOOL32 CMcuVcInst::AddMixMember( const TMt *ptMt, u8 byVolume, BOOL32 bForce )
{
    //是否在音频环回
    TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus(ptMt->GetMtId(), &tMtStatus);
    if (tMtStatus.IsMediaLoop(MODE_AUDIO))
    {
        return FALSE;
    }

    // xsl [8/19/2006] 分散会议通知终端改变接收地址和端口为单播
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        ChangeSatDConfMtRcvAddr(ptMt->GetMtId(), LOGCHL_AUDIO, FALSE);
    }

	TMixMember tMixMember;
	memset( &tMixMember, 0, sizeof(tMixMember) );
	tMixMember.m_byVolume = byVolume;
	tMixMember.m_tMember  = *ptMt;

	TTransportAddr  tAddr;
	TLogicalChannel tLogicalChannel;
	if (m_ptMtTable->GetMtLogicChnnl(ptMt->GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE))
	{
		tAddr = tLogicalChannel.GetRcvMediaChannel();
	}
	else
	{
		return FALSE;
	}

    tMixMember.m_byAudioType = tLogicalChannel.GetChannelType();

	u8  byMixerId = m_tMixEqp.GetEqpId();
	u32 dwSwitchIpAddr;
	u16 wSwitchPort;
	u32 dwMtSwitchIp;
	u16 wMtSwitchPort;
	u32 dwMtSrcIp;
	g_cMpManager.GetMixerSwitchAddr(byMixerId, dwSwitchIpAddr, wSwitchPort);

	g_cMpManager.GetSwitchInfo(*ptMt, dwMtSwitchIp, wMtSwitchPort, dwMtSrcIp);

	//这里应考虑同一混音器存在多个混音组的情况，源接收端口加以区分
	wSwitchPort = wSwitchPort + 
		          MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId + PORTSPAN * GetMixChnPos(ptMt->GetMtId()) + 2;

	tMixMember.m_tAddr.SetIpAddr(dwMtSwitchIp);
	tMixMember.m_tAddr.SetPort(wSwitchPort);
	tMixMember.m_tRtcpBackAddr.SetIpAddr(dwMtSwitchIp);
	tMixMember.m_tRtcpBackAddr.SetPort(wSwitchPort + 1);

	CServMsg cServMsg;
	cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetChnIndex(GetMixChnPos(ptMt->GetMtId()));
	cServMsg.SetMsgBody((u8*)&m_byMixGrpId, 1);
	cServMsg.CatMsgBody((u8*)&tMixMember, sizeof(TMixMember));
	cServMsg.CatMsgBody((u8*)&tLogicalChannel.m_byChannelType, sizeof(u8));

	if (bForce)
	{
		SendMsgToEqp(m_tMixEqp.GetEqpId(), MCU_MIXER_FORCEACTIVE_REQ, cServMsg);
        // xsl [8/4/2006] 直接返回即可
        return TRUE;
	}

    // xsl [7/29/2006] 不在混音组内的mt才加入混音组
    if (!m_ptMtTable->IsMtInMixGrp(ptMt->GetMtId()))
    {
        SendMsgToEqp(m_tMixEqp.GetEqpId(), MCU_MIXER_ADDMEMBER_REQ, cServMsg);
        
        m_ptMtTable->SetMtInMixGrp(ptMt->GetMtId(), TRUE);     
    }

	//语音激励时，不用把码流交换回来。
	//讨论模式下，混音后的码流区分交换回来后再交换给各终端
	if (m_tConf.m_tStatus.IsMixing())
	{
        u32 dwRcvIp, dwSrcIp;
        u16 wRcvPort;        
        //得到交换源地址
        if( !g_cMpManager.GetSwitchInfo( m_tMixEqp, dwRcvIp, wRcvPort, dwSrcIp ) )
        {
            EqpLog("AddMixMember() get switch info failed!");
        }

		TMt tSrcMt;
		tSrcMt.SetNull();
		g_cMpManager.StartSwitch(tSrcMt, m_byConfIdx, 
								 dwSrcIp, 0, 
								 tMixMember.m_tAddr.GetIpAddr(), tMixMember.m_tAddr.GetPort(), 
								 tAddr.GetIpAddr(), tAddr.GetPort());

        // guzh [8/31/2006] 设置终端的音频源是混音器
        m_ptMtTable->SetMtSrc( ptMt->GetMtId(), &m_tMixEqp, MODE_AUDIO );

        // 清除选看状态
        m_ptMtTable->GetMtStatus(ptMt->GetMtId(), &tMtStatus);
        tMtStatus.RemoveSelByMcsMode(MODE_AUDIO);
        m_ptMtTable->SetMtStatus(ptMt->GetMtId(), &tMtStatus);
	}

	if (m_tConf.GetConfAttrb().IsResendLosePack())
	{
		u8  byChlNum = 0;
		u16 wRtcpSwitchPort = 0;
		TTransportAddr  tEqpAddr;
		TLogicalChannel tEqpLogicalChannel;

		m_ptMtTable->GetMtLogicChnnl(ptMt->GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE);

		g_cMcuVcApp.GetPeriEqpLogicChnnl(m_tMixEqp.GetEqpId(), MODE_AUDIO, &byChlNum, &tEqpLogicalChannel, TRUE);

		//添加下行Rtcp反馈交换
		tAddr    = tLogicalChannel.GetRcvMediaCtrlChannel();
		tEqpAddr = tEqpLogicalChannel.GetRcvMediaCtrlChannel();		
		
        // xsl [7/24/2006] +2 为混音器接收对象端口，+4为混音器发送对象端口
		wRtcpSwitchPort = (tEqpAddr.GetPort() - 2) + 
			              MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId + PORTSPAN*GetMixChnPos(ptMt->GetMtId()) + 4;

		g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, 
										 tAddr.GetIpAddr(), 0, 
										 tAddr.GetIpAddr(), tAddr.GetPort(), 
										 tEqpAddr.GetIpAddr(), wRtcpSwitchPort);

		//添加上行Rtcp反馈交换
		tAddr = tLogicalChannel.GetSndMediaCtrlChannel();

		g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, 
										 tMixMember.m_tRtcpBackAddr.GetIpAddr(), 0, 
										 tMixMember.m_tRtcpBackAddr.GetIpAddr(), 
										 tMixMember.m_tRtcpBackAddr.GetPort(), 
										 tAddr.GetIpAddr(), tAddr.GetPort());
	}

	return TRUE;
}

/*====================================================================
    函数名      ：RemoveMixMember
    功能        ：移除混音成员
    算法实现    ：通知mixer移除通道
    引用全局变量：
    输入参数说明：const TMt *ptMt 成员终端
                  BOOL32 bForce 是否是强制 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::RemoveMixMember( const TMt *ptMt, BOOL32 bForce )
{
	TMixMember tMixMember;
	tMixMember.m_byVolume = DEFAULT_MIXER_VOLUME;
	tMixMember.m_tMember  = *ptMt;

	TLogicalChannel tLogicalChannel;
	if( m_ptMtTable->GetMtLogicChnnl( ptMt->GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )
	{
		tMixMember.m_tAddr = tLogicalChannel.GetRcvMediaChannel();	
	}
	tMixMember.m_byAudioType = tLogicalChannel.GetChannelType();

	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );
    cServMsg.SetChnIndex( GetMixChnPos(ptMt->GetMtId()) );
	cServMsg.SetMsgBody( (u8*)&m_byMixGrpId, 1 );
	cServMsg.CatMsgBody( (u8*)&tMixMember, sizeof(TMixMember) );

	if( bForce )
	{
		SendMsgToEqp( m_tMixEqp.GetEqpId(), MCU_MIXER_CANCELFORCEACTIVE_REQ, cServMsg );
	}
	else
	{
        // xsl [7/29/2006] 只有在混音组内才移除通道
        if (m_ptMtTable->IsMtInMixGrp(ptMt->GetMtId()))
        {
            SendMsgToEqp( m_tMixEqp.GetEqpId(), MCU_MIXER_REMOVEMEMBER_REQ, cServMsg );
            m_ptMtTable->SetMtInMixGrp(ptMt->GetMtId(), FALSE);
        }        
	}

	//在停止混音时统一移除上下行Rtcp反馈交换

	return;
}


/*====================================================================
    函数名      ：GetMixChnPos
    功能        ：获取混音成员通道位置，用于区分交换端口号
    算法实现    ：
    引用全局变量：
    输入参数说明：u8     byMtId
                  BOOL32 bRemove
    返回值说明  ：u8: pos
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/03/19    4.0         张宝卿         创建
====================================================================*/
u8 CMcuVcInst::GetMixChnPos( u8 byMtId, BOOL32 bRemove )
{
    u8 byDstPos = 0;
    if ( byMtId == 0 )
    {
        ConfLog( FALSE, "[GetMixChnPos] param err: byMtId.%d \n", byMtId );
        return byDstPos;
    }
    
    if ( !bRemove )
    {
        BOOL32 bExist = FALSE;
        u8     byFirstIdle = 0xFF;

        u8 byPos = 0;
        
        for( byPos = 1; byPos < MAXNUM_MIXER_CHNNL+1; byPos++ )
        {
            // guzh [5/12/2007] Find the old one
            if ( byMtId == m_abyMixMtId[byPos])
            {
                bExist = TRUE;
                MpManagerLog( "[GetMixChnPos] Mt.%d already exist at byPos.%d !\n", byMtId, byPos );
                byDstPos = byPos;
                break;
            }
            else if ( 0 == m_abyMixMtId[byPos] && 0xFF == byFirstIdle ) 
            {
                byFirstIdle = byPos;
            }
        }
        if ( !bExist )
        {
            if ( 0xFF != byFirstIdle )
            {
                m_abyMixMtId[byFirstIdle] = byMtId;
                MpManagerLog( "[GetMixChnPos] Mt.%d set succeed, byPos.%d !\n", byMtId, byFirstIdle );
                byDstPos = byFirstIdle;
            }
            else
            {
                ConfLog(FALSE, "[GetMixChnPos] Mt.%d set to m_abyMixMtId failed !\n", byMtId );
            }
        }
    }
    else
    {
        BOOL32 bExist = FALSE;
        u8 byPos = 1;
        for( ; byPos < MAXNUM_MIXER_CHNNL+1; byPos++ )
        {
            if ( byMtId == m_abyMixMtId[byPos] )
            {
                m_abyMixMtId[byPos] = 0;
                bExist = TRUE;
                MpManagerLog( "[GetMixChnPos] Mt.%d remve m_abyMixMtId succeed, byPos.%d !\n", byMtId, byPos );
                break;
            }
        }
        if ( !bExist )
        {
            ConfLog( FALSE, "[GetMixChnPos] Mt.%d remve m_abyMixMtId failed !\n", byMtId );
        }
    }
    return byDstPos;
}

/*====================================================================
    函数名      ：NotifyMixerSendToMt
    功能        ：通知混音器往终端发数据
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bSend 是否发送
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         胡昌威         创建
====================================================================*/
BOOL32 CMcuVcInst::NotifyMixerSendToMt( BOOL32 bSend )
{
	u8 bySend = ( bSend == FALSE ) ? 0 : 1;

	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetMsgBody( (u8*)&m_byMixGrpId, 1 );
	cServMsg.CatMsgBody( (u8*)&bySend, 1 );

	//给混音器发开始消息
	SendMsgToEqp( m_tMixEqp.GetEqpId() , MCU_MIXER_SEND_NOTIFY, cServMsg );

	return TRUE;
}

/*====================================================================
    函数名      ：NotifyMixerSendToMt
    功能        ：通知混音器往终端发数据
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bSend 是否发送
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::SetMixerSensitivity( u8 bySensVal )
{
	u32 dwSens = bySensVal;
	dwSens = htonl( dwSens );

	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetMsgBody( (u8*)&m_byMixGrpId, 1 );
	cServMsg.CatMsgBody( (u8*)&dwSens, sizeof(u32) );

	//给混音器发开始消息
	SendMsgToEqp( m_tMixEqp.GetEqpId() , MCU_MIXER_VACKEEPTIME_CMD, cServMsg );

	return;
}

/*=============================================================================
    函 数 名： SetMixDelayTime
    功    能： 设置混音延时时间间隔
    算法实现： 
    全局变量： 
    参    数： u16 wDelayTime
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/1/10    3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::SetMixDelayTime(u16 wDelayTime)
{
	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetMsgBody(&m_byMixGrpId, sizeof(u8));
	cServMsg.CatMsgBody((u8*)&wDelayTime, sizeof(u16));

	//给混音器发消息
	SendMsgToEqp( m_tMixEqp.GetEqpId() , MCU_MIXER_CHANGEMIXDELAY_CMD, cServMsg );

	EqpLog("[SetMixDelayTime] MixEqpId :%d, MixGrpId :%d, DelayTime :%u\n", 
		   m_tMixEqp.GetEqpId(), m_byMixGrpId, wDelayTime);

	return;
}

/*====================================================================
    函数名      ：SetMemberVolume
    功能        ：设置成员音量
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt *ptMt 混音成员
	              u8 byVolume 音量
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::SetMemberVolume( const TMt *ptMt, u8 byVolume )
{
	TMixMember  tMixMember;
	tMixMember.m_byVolume = byVolume;
	tMixMember.m_tMember  = *ptMt;

	TLogicalChannel tLogicalChannel;
	if ( m_ptMtTable->GetMtLogicChnnl( ptMt->GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )
	{
		tMixMember.m_tAddr = tLogicalChannel.GetRcvMediaChannel();	
	}
	tMixMember.m_byAudioType = tLogicalChannel.GetChannelType();

	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );
    cServMsg.SetChnIndex( GetMixChnPos(ptMt->GetMtId()) );
	cServMsg.SetMsgBody( (u8*)&m_byMixGrpId, 1 );
	cServMsg.CatMsgBody( (u8*)&tMixMember, sizeof(TMixMember) );
	SendMsgToEqp( m_tMixEqp.GetEqpId(), MCU_MIXER_SETCHNNLVOL_CMD, cServMsg );

	return;
}

/*====================================================================
    函数名      ：ProcMixerRspWaitTimer
    功能        ：MCU等待混音器应答超时处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/08    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcMixerRspWaitTimer( const CMessage * pcMsg )
{
	// fxh
	KillTimer(MCUVC_MIX_WAITMIXERRSP_TIMER);
	if (m_tMixEqp.IsNull())
	{
		EqpLog("[ProcMixerRspWaitTimer]m_tMixEqp Is Null!\n");
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	TPeriEqpStatus tPeriEqpStatus;
	u8 byMixGrpState;

	switch (CurState())
	{
	case STATE_ONGOING:

		//放弃混音组
		g_cMcuVcApp.GetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
        byMixGrpState = tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState;
		tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = TMixerGrpStatus::READY;
		g_cMcuVcApp.SetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );

		switch(byMixGrpState)
		{
		case TMixerGrpStatus::WAIT_START_VAC:
			// guzh [11/9/2007]
			//NACK
			cServMsg.SetErrorCode( ERR_MCU_TIMEROUT );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			break;
		case TMixerGrpStatus::WAIT_STOP:
			ProcMixStopSucRsp();			
		    break;
		default:
		    break;
		}

        m_tMixEqp.SetNull();
        m_byMixGrpId = 0;

		break;

	default:
		ConfLog(FALSE, "Wrong message %u(%s) received in state %u in ProcMixerRspWaitTimer!\n", 
				pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState());
		break;
	}

	return;
}

/*====================================================================
    函数名      ：ProcMixerMcuRsp
    功能        ：混音器回应MCU消息处理函数
    算法实现    ：.
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         胡昌威        创建
	06/02/17	4.0			张宝卿		  混音状态恢复保护
    06/04/20	4.0			张宝卿		  停止混音后恢复选看状态
====================================================================*/
void CMcuVcInst::ProcMixerMcuRsp(const CMessage * pcMsg)
{
	CServMsg       cServMsg(pcMsg->content, pcMsg->length);	
	TPeriEqpStatus tPeriEqpStatus;
	TConfMtInfo	   tConfMtInfo;
	TMt            tMt;
	u8             byMixGrpState;
	u8             byMtId;
	TMixMember     tMixMember;
    u8             byMixMtNum;

	switch (CurState())
	{
	case STATE_ONGOING:

		switch (pcMsg->event)
		{
		case MIXER_MCU_STARTMIX_ACK:        //同意开始使用混音器应答

			//清除定时
			KillTimer(MCUVC_MIX_WAITMIXERRSP_TIMER);

			//设置参数
			g_cMcuVcApp.GetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);
			byMixGrpState = tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState;

			EqpLog("[MIXER_MCU_STARTMIX_ACK] Eqp.%d MixGrpState.%d\n", 
        		   m_tMixEqp.GetEqpId(), byMixGrpState);
        
            tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = TMixerGrpStatus::MIXING;
            g_cMcuVcApp.SetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);
    
			//改变会议状态
            if (byMixGrpState == TMixerGrpStatus::WAIT_START_VAC)
			{
                m_tConf.m_tStatus.SetVACing();
				//通知主席及所有会控
				SendMsgToAllMcs(MCU_MCS_STARTVAC_NOTIF, cServMsg);
				if (HasJoinedChairman())
				{
					SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STARTVAC_NOTIF, cServMsg);
				}

				//通知混音器停止向终端发码流
				NotifyMixerSendToMt(FALSE);

			}
			else//混音及讨论
			{
                if (byMixGrpState == TMixerGrpStatus::WAIT_START_SPECMIX)
                {
                    m_tConf.m_tStatus.SetSpecMixing();
                }
                else
                {
                    m_tConf.m_tStatus.SetAutoMixing();
                }

				TMcu tMcu;
				tMcu.SetMcuId(LOCAL_MCUID);
				cServMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) ); 
                u8 byMixDepth = GetMaxMixNum();
				cServMsg.CatMsgBody( (u8*)&byMixDepth, 1 );
				if (!m_tCascadeMMCU.IsNull())
				{
					cServMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
					SendMsgToMt(m_tCascadeMMCU.GetMtId(),  MCU_MCU_STARTMIXER_NOTIF, cServMsg);
				}
				if (HasJoinedChairman())
				{
					cServMsg.SetMsgBody((u8*)&byMixDepth, 1);
					SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STARTDISCUSS_NOTIF, cServMsg);
				}

                ChangeAudBrdSrc(&m_tMixEqp);                
			}

            //混音状态通知,通知主席及所有会控
            MixerStatusChange();            

            SetMixerSensitivity(m_tConf.GetTalkHoldTime());
            
			// xsl [8/4/2006] 添加混音成员, 交换码流给混音器
            byMixMtNum = 0;
			for (byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
            {
                if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
                {
                    tMt = m_ptMtTable->GetMt(byMtId);    

                    if ((!m_tConf.m_tStatus.IsSpecMixing() ||
                        (m_tConf.m_tStatus.IsSpecMixing() && m_ptMtTable->IsMtInMixing(byMtId)))
                        && byMixMtNum < GetMaxMixNum())
                    {
                        //通知mixer增加通道，非vac模式将mixer码流交换给mt，建上下行交换                
                        AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE);

                        StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
								             (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())), 
								             MODE_AUDIO, SWITCH_MODE_SELECT);
                        byMixMtNum++;
                    }  
           
                    //定制混音不在混音列表中听N模式声音
                    if (m_tConf.m_tStatus.IsSpecMixing() && !m_ptMtTable->IsMtInMixing(byMtId))
                    {
                        SwitchMixMember(&tMt, TRUE);
                    }
                }           
            }  

            // xsl [8/19/2006] 如果是n模式分散会议需要交换到组播地址
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                g_cMpManager.StartDistrConfCast(m_tMixEqp, MODE_AUDIO);
            }

            // guzh [11/7/2007] 
			//在混音状态置位后再发起语音激励
			if ( m_tLastMixParam.GetMode() == mcuVacMix )
			{
				CServMsg cServMsg;				
                cServMsg.SetEventId(MCS_MCU_STARTVAC_REQ);				
                MixerVACReq(cServMsg);
                //上次混音器的工作情况恢复到此为止
				m_tLastMixParam.SetMode(mcuNoMix);
			}

			if (m_tConf.m_tStatus.IsMixing())
			{
				//通知混音器开始向终端发码流
				NotifyMixerSendToMt(TRUE);
			}

			if (m_tConf.m_tStatus.IsSpecMixing())
			{
				AddRemoveSpecMixMember();         
			}
			else 
			{
				MtStatusChange();
			}
			break;

		case MIXER_MCU_STOPMIX_ACK:         //同意停止混音应答	
			
			// fxh 清除定时器
			KillTimer(MCUVC_MIX_WAITMIXERRSP_TIMER);
			// 检测状态机
			if ((m_tConf.m_tStatus.IsVACing() || m_tConf.m_tStatus.IsMixing()) &&
				!m_tMixEqp.IsNull())
			{
				TPeriEqpStatus tPeriStatus;
				g_cMcuVcApp.GetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriStatus);
				if (tPeriStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState != TMixerGrpStatus::WAIT_STOP)
				{
					EqpLog("[CMcuVcInst%d][ProcMixerMcuRsp]stop mix(%d) ack recv at wrong state(%s)\n", 
						   GetInsID(), m_tMixEqp.GetMtId(), tPeriStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState);
					return;
				}

			}
            
            ProcMixStopSucRsp();
			break;

		case MIXER_MCU_ADDMEMBER_ACK:       //加入成员应答消息

			tMixMember =  *((TMixMember*)(cServMsg.GetMsgBody() + sizeof(u8)));
			if (tMixMember.m_tMember == GetLocalSpeaker())//强制混音
			{
				AddMixMember(&tMixMember.m_tMember, DEFAULT_MIXER_VOLUME, TRUE);
			}

			break;

		case MIXER_MCU_REMOVEMEMBER_ACK:    //删除成员应答消息

			tMixMember =  *((TMixMember*)(cServMsg.GetMsgBody() + sizeof(u8)));
			if (tMixMember.m_tMember == GetLocalSpeaker())// xsl [8/4/2006]强制混音(此时终端可能已经不是发言人)
			{
				RemoveMixMember(&tMixMember.m_tMember, TRUE);
			}

			break;

		case MIXER_MCU_FORCEACTIVE_ACK:     //强制成员混音应答

			break;

		case MIXER_MCU_CANCELFORCEACTIVE_ACK:    //取消成员强制混音应答

			break;

		case MIXER_MCU_ADDMEMBER_NACK:      //加入成员应答消息
		case MIXER_MCU_REMOVEMEMBER_NACK:   //删除成员应答消息
		case MIXER_MCU_FORCEACTIVE_NACK:    //强制成员混音拒绝
		case MIXER_MCU_CANCELFORCEACTIVE_NACK:   //取消成员强制混音拒绝
		case MIXER_MCU_STARTMIX_NACK:       //拒绝开始混音应答
		case MIXER_MCU_STOPMIX_NACK:        //拒绝停止混音应答
		case MIXER_MCU_SETMIXDEPTH_NACK:    //设置混音深度拒绝消息
			break;

		default:
			ConfLog(FALSE, "Wrong message %u(%s) received in state %u in ProcMixerMcuRsp!\n", 
					pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState());
			break;
		}

		break;

	default:
		ConfLog(FALSE, "Wrong message %u(%s) received in state %u in ProcMixerMcuRsp!\n", 
				pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState());
		break;
	}

	return;
}

/*=============================================================================
  函 数 名： MixerStatusChange
  功    能： 混音状态通知, 通知所有会控和主席
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  07/11/07	  4.0		  顾振华		调整为所有混音器状态的变更通知
=============================================================================*/
void CMcuVcInst::MixerStatusChange(void)
{    
    //TMcu tMcu;
    //tMcu.SetMcuId(LOCAL_MCUID);
    
    CServMsg cServMsg;
    //cServMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) ); 
    u8 byMixerMode = m_tConf.m_tStatus.GetMixerMode();
    cServMsg.SetMsgBody( &byMixerMode, sizeof(u8));
    SendMsgToAllMcs(MCU_MCS_MIXPARAM_NOTIF, cServMsg);
    
    // guzh [11/7/2007] 是否可以省略？
    //SendConfInfoToChairMt();

    EqpLog("[MixerStatusChange] MixerMode: %d\n", byMixerMode );
    return;
}

/*====================================================================
    函数名      ：ProcMixerMcuRsp
    功能        ：混音器给MCU通知消息处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcMixerMcuNotif(const CMessage * pcMsg)
{
    if (STATE_ONGOING != CurState())
    {
        ConfLog(FALSE, "[ProcMixerMcuNotif] Wrong message %u(%s) received in state %u!\n", 
                pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState());
        return;
    }
    
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 *pbyVACMember;
    u8 byMtId;
    TPeriEqpStatus tPeriEqpStatus;
    TMixerGrpStatus tMixerGrpStatus;
    TEqp tEqp;
    u8   byExciteChn;
    TPeriEqpRegReq tRegReq;
    
    switch (pcMsg->event)
    {
    case MIXER_MCU_ACTIVEMMBCHANGE_NOTIF:  //混音激励成员改变通知

        //得到讨论成员
        pbyVACMember = (u8 *)cServMsg.GetMsgBody();       

        byExciteChn = *(pbyVACMember + MAXNUM_MIXER_DEPTH);
        if (byExciteChn > 0 && byExciteChn <= MAXNUM_CONF_MT)
        {
            m_tVacLastSpeaker = m_ptMtTable->GetMt(byExciteChn);
        }
       
        //语音激励控制改变
        VACChange(m_tConf.m_tStatus.m_tMixParam, byExciteChn);

        break;

    case MIXER_MCU_CHNNLVOL_NOTIF:      //某通道音量通知消息   			
        break;

    case MIXER_MCU_GRPSTATUS_NOTIF:     //混音组状态通知
        tMixerGrpStatus = *(TMixerGrpStatus *)cServMsg.GetMsgBody();
        if (tMixerGrpStatus.m_byGrpId == m_byMixGrpId)
        {
            TMixerGrpStatus tOldMixerGrpStatus;
            g_cMcuVcApp.GetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);
            tOldMixerGrpStatus = tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId];
            tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId] = tMixerGrpStatus;
            tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpId = tOldMixerGrpStatus.m_byGrpId;
            tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpMixDepth = tOldMixerGrpStatus.m_byGrpMixDepth;
            tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byConfId = m_byConfIdx;
            // guzh [11/9/2007] 业务负责切换
            tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = tOldMixerGrpStatus.m_byGrpState;

            g_cMcuVcApp.SetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);

            cServMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
            SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
        }

        break;

    case MIXER_MCU_MIXERSTATUS_NOTIF:
        {
            // guzh [11/6/2007] 根据混音器断链前的工作状态，依次恢复
            // 是否先 混音，然后是VAC
            EqpLog("Last mixer mode = %d\n", m_tLastMixParam.GetMode());        
            if (mcuNoMix == m_tLastMixParam.GetMode())
            {
                return;
            }
            
            if (m_tLastMixParam.GetMode() == mcuWholeMix ||
                m_tLastMixParam.GetMode() == mcuVacWholeMix)
            {
                CServMsg cServMsg;
                cServMsg.SetEventId(MCS_MCU_STARTDISCUSS_REQ);
                ProcMixStart(cServMsg);
                
                // 标记接下来开VAC
                if ( m_tLastMixParam.GetMode() == mcuVacWholeMix ) 
                {
                    m_tLastMixParam.SetMode(mcuVacMix);
                }
            }
            else if (m_tLastMixParam.GetMode() == mcuPartMix ||
                     m_tLastMixParam.GetMode() == mcuVacPartMix)
            {
                CServMsg cServMsg;

                //从保存的列表中获取
                for(u8 byLoop = 0; byLoop < m_tLastMixParam.GetMemberNum(); byLoop++)
                {
                    u8 byLastMixMtId = m_tLastMixParam.GetMemberByIdx(byLoop);
                    if ( byLastMixMtId != 0 &&
                         m_tConfAllMtInfo.MtJoinedConf(byLastMixMtId) )
                    {
                        TMt tMixMt;
                        tMixMt = m_ptMtTable->GetMt(m_tLastMixParam.GetMemberByIdx(byLoop));
                        cServMsg.CatMsgBody((u8 *)&tMixMt, sizeof(TMt));
                    }
                }
                m_tLastMixParam.ClearAllMembers();
                //m_tConf.m_tStatus.SetMixerParam(m_tLastMixParam);
                
                ProcMixStart(cServMsg);
                
                // 标记接下来开VAC
                if ( m_tLastMixParam.GetMode() == mcuVacPartMix ) 
                {
                    m_tLastMixParam.SetMode(mcuVacMix);
                }            
            }
            else if (m_tLastMixParam.GetMode() == mcuVacMix )
            {
                CServMsg cServMsg;
                cServMsg.SetEventId(MCS_MCU_STARTVAC_REQ);
                MixerVACReq(cServMsg);
            }
            else
            {
                ConfLog(FALSE, "[MCU_MIXERCONNECTED_NOTIF] - ERROR mixer mode: %d\n\n", m_tLastMixParam.GetMode());
            }
            break;
        }

    case MCU_MIXERCONNECTED_NOTIF:   //混音器建链成功通知
        tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();

        tEqp.SetMcuEqp(tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());

        EqpLog("m_tMixEqp.GetMtId(%d) m_tMixEqp.GetMcuId(%d) m_tMixEqp.GetType(%d)\n",
                                m_tMixEqp.GetMtId(), m_tMixEqp.GetMcuId(), m_tMixEqp.GetType());
        EqpLog("tEqp.GetMtId(%d) tEqp.GetMcuId(%d) tEqp.GetType(%d)\n",
                                tEqp.GetMtId(),tEqp.GetMcuId(), tEqp.GetType());        
        
        if (m_tMixEqp.GetEqpId() != tEqp.GetEqpId() ||
            m_tMixEqp.GetMcuId() != tEqp.GetMcuId() ||
            m_tMixEqp.GetType() != tEqp.GetType())
        {
            return;
        }

        /*
        // guzh [11/6/2007] 根据混音器断链前的工作状态，依次恢复
        // 是否先 混音，然后是VAC
        EqpLog("Last mixer mode = %d\n", m_byLastMixerMode);        
        if (mcuNoMix == m_byLastMixerMode)
        {
            return;
        }

        if (m_byLastMixerMode == mcuWholeMix ||
            m_byLastMixerMode == mcuVacWholeMix)
        {
            CServMsg cServMsg;
            cServMsg.SetEventId(MCS_MCU_STARTDISCUSS_REQ);
            ProcMixStart(cServMsg);

            // 标记接下来开VAC
			if ( m_byLastMixerMode == mcuVacWholeMix ) 
			{
				m_byLastMixerMode = mcuVacMix;
			}
        }
        else if (m_byLastMixerMode == mcuPartMix ||
                 m_byLastMixerMode == mcuVacPartMix)
        {
            CServMsg cServMsg;
            //从保存的列表中获取
            TMixParam tLastParam = m_tConf.m_tStatus.GetMixerParam();
            for(u8 byLoop = 0; byLoop < tLastParam.GetMemberNum(); byLoop++)
            {
                if ( tLastParam.GetMemberByIdx(byLoop) != 0 )
                {
                    TMt tMixMt;
                    tMixMt = m_ptMtTable->GetMt(tLastParam.GetMemberByIdx(byLoop));
                    cServMsg.CatMsgBody((u8 *)&tMixMt, sizeof(TMt));
                }
            }
            tLastParam.ClearAllMembers();
            m_tConf.m_tStatus.SetMixerParam(tLastParam);

            ProcMixStart(cServMsg);
            
            // 标记接下来开VAC
            if ( m_byLastMixerMode == mcuVacPartMix ) 
            {
                m_byLastMixerMode = mcuVacMix;
			}            
        }
        else if (m_byLastMixerMode == mcuVacMix )
        {
            CServMsg cServMsg;
            cServMsg.SetEventId(MCS_MCU_STARTVAC_REQ);
            MixerVACReq(cServMsg);
        }
        else
        {
            ConfLog(FALSE, "[MCU_MIXERCONNECTED_NOTIF] - ERROR mixer mode: %d\n\n", m_byLastMixerMode);
        }
        */
        break;

    case MCU_MIXERDISCONNECTED_NOTIF:   //混音器断链通知

        tEqp = *(TEqp *)cServMsg.GetMsgBody();

        //否是本会议的混音器断链
        if (tEqp.GetEqpId() != m_tMixEqp.GetEqpId())
        {
            return;
        }

        // guzh [11/7/2007] 保存混音器工作参数，混音终端列表则不清空，仅清TMtStatus
        m_tLastMixParam.SetMode(m_tConf.m_tStatus.GetMixerMode());

        //改变会议状态
        if (m_tConf.m_tStatus.IsVACing())
        {
            //通知主席及所有会控
            SendMsgToAllMcs(MCU_MCS_STOPVAC_NOTIF, cServMsg);
            if (HasJoinedChairman())
            {
                SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cServMsg);
            }
            //停止向混音器交换数据，并恢复收听广播源
            for (byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
            {
                if(m_tConfAllMtInfo.MtJoinedConf(byMtId))            
                {
                    StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
							            (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(byMtId)), FALSE, MODE_AUDIO);
                    
                    // xsl [7/29/2006]
                    m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);
                }
            }
        }
		
        if(m_tConf.m_tStatus.IsMixing())//混音(讨论)
        {
            //通知主席及所有会控 
            TMcu tMcu;
            tMcu.SetMcuId(LOCAL_MCUID);
            cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
            if (!m_tCascadeMMCU.IsNull())
            {
                cServMsg.SetDstMtId(m_tCascadeMMCU.GetMtId());
                SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_STOPMIXER_NOTIF, cServMsg);
            }
            if (HasJoinedChairman())
            {
                SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STOPDISCUSS_NOTIF, cServMsg);
            }

            //停止向混音器交换数据，并恢复收听广播源
            TMixParam tMixParam = m_tConf.m_tStatus.GetMixerParam();
            
            //zbq[04/10/2008] 定制混音保存混音终端列表
            if (mcuPartMix == tMixParam.GetMode() ||
                mcuVacPartMix == tMixParam.GetMode() )
            {
                for(u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
                {
                    if (m_tConfAllMtInfo.MtJoinedConf(byMtIdx) &&
                        m_ptMtTable->IsMtInMixing(byMtIdx))
                    {
                        m_tLastMixParam.AddMember(byMtIdx);
                    }
                }
            }

            tMixParam.ClearAllMembers();
            for (byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
            {
                if (m_tConfAllMtInfo.MtJoinedConf(byMtId))            
                {
                    StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
							            (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(byMtId)), FALSE, MODE_AUDIO );

                    StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);	

                    // xsl [9/21/2006] 恢复从组播地址接收
                    if (m_tConf.GetConfAttrb().IsSatDCastMode() && m_ptMtTable->IsMtInMixGrp(byMtId))
                    {
                        ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_AUDIO);
                    }

                    // xsl [7/29/2006]
                    m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);

                    if (m_ptMtTable->IsMtInMixing(byMtId))            
                    {
                        m_ptMtTable->SetMtInMixing(byMtId, FALSE);

                        // guzh [11/7/2007] 保存到临时列表中，等混音器重新上线
                        tMixParam.AddMember(byMtId);
                        
                        u8 byMix = 0;
                        cServMsg.SetMsgBody((u8*)&byMix, 1);
                        SendMsgToMt(byMtId, MCU_MT_MTADDMIX_NOTIF, cServMsg);
                    }
                }                
            }

            MtStatusChange();

            //改变视频源
            if (HasJoinedSpeaker())
            {
                TMt tSpeakerMt = GetLocalSpeaker();
                ChangeAudBrdSrc(&tSpeakerMt);
            }
            else
            {
                ChangeAudBrdSrc(NULL);
            }

            //向上级MCU发终端声音
            if (!m_tCascadeMMCU.IsNull())
            {
                TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
                if (ptInfo != NULL&& 
                    m_tConfAllMtInfo.MtJoinedConf(ptInfo->m_tSpyMt.GetMcuId(), ptInfo->m_tSpyMt.GetMtId()))
                {
                    StartSwitchToSubMt(ptInfo->m_tSpyMt, 0, m_tCascadeMMCU.GetMtId(), MODE_AUDIO, SWITCH_MODE_SELECT);
                }
            }
        }

        //zbq[12/05/2007] 停点名
        if ( ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
        {
            CServMsg cMsg;
            RollCallStop(cMsg);
        }
        
        //放弃混音组
        g_cMcuVcApp.GetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);
        tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = TMixerGrpStatus::READY;
        g_cMcuVcApp.SetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);

        // 清除混音器工作状态
        m_tConf.m_tStatus.SetVACing(FALSE);
        m_tConf.m_tStatus.SetNoMixing();
        MixerStatusChange();

        break; 

    default:
        break;
    }

#ifdef _SATELITE_
    RefreshConfState();
#endif

    return;
}
		
/*====================================================================
    函数名      ：ProcMcsMcuAddMixMemberCmd
    功能        ：处理会控要求增加混音成员命令
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/10/14    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuAddMixMemberCmd( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    
    STATECHECK;

    if ( m_tConf.m_tStatus.IsSpecMixing() )
    {
        TMt  * ptMt  = (TMt *)(cServMsg.GetMsgBody());;
        u8     byMtNum = (cServMsg.GetMsgBodyLen()) / sizeof(TMt);
        AddRemoveSpecMixMember( ptMt, byMtNum );
    }
    else
    {
        ProcMixStart(cServMsg);
    }
	return;
}

/*====================================================================
    函数名      ：ProcMcsMcuRemoveMixMemberCmd
    功能        ：处理会控要求移除混音成员命令
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/10/14    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuRemoveMixMemberCmd( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    
    STATECHECK;
    
    TMt  *ptMt  = (TMt*)(cServMsg.GetMsgBody());
    u8 byMtNum  = (cServMsg.GetMsgBodyLen())/sizeof(TMt);
    AddRemoveSpecMixMember( ptMt, byMtNum, FALSE );

    return;
}

/*====================================================================
    函数名      ：ProcMcsMcuReplaceMixMemberCmd
    功能        ：处理会控要求替换混音成员命令
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/11/17    4.0         张宝卿         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuReplaceMixMemberCmd( const CMessage * pcMsg )
{
    STATECHECK;

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TMt tOldMt = *(TMt*)cServMsg.GetMsgBody();
    TMt tNewMt = *(TMt*)(cServMsg.GetMsgBody() + sizeof(TMt));

    if ( tNewMt.IsNull() )
    {
        ConfLog(FALSE, "[ReplaceMixMemberCmd] tNewMt.IsNull, ignore it\n");
        return;
    }
    if ( tOldMt.IsNull() )
    {
        ConfLog(FALSE, "[ReplaceMixMemberCmd] tOldMt.IsNull, ignore it\n");
        return;
    }
    if (!m_tConf.m_tStatus.IsSpecMixing())
    {
        ConfLog(FALSE, "[ReplaceMixMemberCmd] conf mode.%d, unexpected opr, ignore it\n", m_tConf.m_tStatus.GetMixerMode());
        return;
    }
    if (tOldMt.IsLocal() &&
        !m_ptMtTable->IsMtInMixing(tOldMt.GetMtId()))
    {
        ConfLog(FALSE, "[ReplaceMixMemberCmd] tOldMt.%d isn't in mixing, ignore it\n", tOldMt.GetMtId());
        return;
    }

    AddRemoveSpecMixMember(&tOldMt, 1, FALSE, TRUE);
    AddRemoveSpecMixMember(&tNewMt, 1, TRUE);
    
    return;
}

/*====================================================================
    函数名      ：AddRemoveSpecMixMember
    功能        ：添加移除混音成员
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt *ptMt 成员终端
                  u8 byMtNum 成员个数
				  BOOL32 bAdd TRUE-添加 FALSE-移除
                  BOOL32 bStopMixNonMem: 移除终端 移空是否停混音
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/09    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::AddRemoveSpecMixMember(TMt *ptMt, u8 byMtNum, BOOL32 bAdd, BOOL32 bStopMixNonMem)
{
    u8  byLoop;
    CServMsg cServMsg;
    
    if (!m_tConf.m_tStatus.IsSpecMixing())
    {
        ConfLog(FALSE, "[AddRemoveSpecMix] conf mode.%d, unexpected opr, ignore it\n", m_tConf.m_tStatus.GetMixerMode());
        return;
    }
    
    for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && m_ptMtTable->IsMtInMixing(byLoop))            
        {
            if (NULL == ptMt)
            {
                u8 byMix = 1;
                cServMsg.SetMsgBody((u8 *)&byMix, sizeof(u8));
                SendMsgToMt(byLoop, MCU_MT_MTADDMIX_NOTIF, cServMsg);
            }
        }
    }

    if (NULL != ptMt)
    {
        // xsl [8/4/2006] 若为add则判断当前是否超过最大混音通道数
        if (bAdd && GetMixMtNumInGrp() >= GetMaxMixNum())
        {
            ConfLog(FALSE, "[AddRemoveSpecMixMember] current mixing mt num exceed %d, ingnore mt.%d\n", 
                    GetMaxMixNum(), ptMt->GetMtId());
            NotifyMcsAlarmInfo(0, ERR_MCU_OVERMAXMIXERCHNNL);
            return;
        }
        // guzh [1/30/2007] 8000B 性能限制
        u16 wError = 0;
        if ( bAdd && 
             !CMcuPfmLmt::IsMixOprSupported(m_tConf, GetMixMtNumInGrp(), byMtNum, wError) )
        {
            NotifyMcsAlarmInfo(0, wError);
            return;
        }

        TMt atAllMixMt[MAXNUM_MIXING_MEMBER];
        TMt * ptAllMt = &atAllMixMt[0];
        memcpy( ptAllMt, ptMt, byMtNum * sizeof(TMt) );
        
        TMtListDiscard tOtherMcuMtList;

        for (byLoop = 0; byLoop < byMtNum; byLoop++)
        {
            EqpLog("[AddRemoveSpecMixMember] nIndex.%d Mt<%d,%d> bAdd.%d\n",
                    byLoop+1, ptAllMt->GetMcuId(), ptAllMt->GetMtId(), bAdd);

            if (ptAllMt->IsLocal())
            {
                TMt tMt = GetLocalMtFromOtherMcuMt(*ptAllMt);
                u8 byMix = bAdd ? 1 : 0; 
                m_ptMtTable->SetMtInMixing(tMt.GetMtId(), bAdd);

                ptAllMt++;            
                
                cServMsg.SetMsgBody((u8 *)&byMix, 1);
                SendMsgToMt(tMt.GetMtId(), MCU_MT_MTADDMIX_NOTIF, cServMsg);
                
                // xsl [8/4/2006] 用于改变混音成员时处理
                if (bAdd)
                {
                    if (!m_ptMtTable->IsMtInMixGrp(tMt.GetMtId()))
                    {
                        // zbq [05/14/2007] 非发言人切换导致的混音成员增加，须判断其
                        // 成员有效性，否则将建错交换。现已不会这样，保护起见，防止误建。
                        TLogicalChannel tLogicChan;
                        if ( m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tLogicChan, TRUE) )
                        {
                            AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE);
                            StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
                                                 (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())),
                                                 MODE_AUDIO, SWITCH_MODE_SELECT);
                        }
                    }
                }
                else 
                {
                    if (m_ptMtTable->IsMtInMixGrp(tMt.GetMtId()))
                    {
                        RemoveMixMember(&tMt, FALSE);
                        StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
                                            (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())),
                                            FALSE, MODE_AUDIO );
                        g_cMpManager.StopSwitchToSubMt(tMt, MODE_AUDIO);    // xsl [8/19/2006] 停止到终端的交换
                        SwitchMixMember(&tMt, TRUE);
                    }                
                }
            }
            else
            {   
                tOtherMcuMtList.SetNewMt(*ptAllMt);
            }
        }

        u8  byGrpNum = 0;
        TMt atMt[MAXNUM_SUB_MCU * MAXNUM_MIXING_MEMBER];
        u8  abyMixNum[MAXNUM_SUB_MCU] = { 0 };
        TMt *ptMixMt = &atMt[0];
        u8  *pbyMixNum = &abyMixNum[0];

        tOtherMcuMtList.GetMtList( sizeof(atMt), byGrpNum, pbyMixNum, ptMixMt );

        if ( 0 != byGrpNum )
        {
            for( u8 byGrpIdx = 0; byGrpIdx < byGrpNum; byGrpIdx ++)
            {
                TMcu tMcu;
                tMcu.SetMcu(ptMixMt->GetMcuId());
                
                CServMsg cMsg;
                cMsg.SetMsgBody((u8*)&tMcu, sizeof(TMcu));
                cMsg.CatMsgBody((u8*)ptMixMt, (sizeof(TMt) * *pbyMixNum));

                //zbq[11/29/2007] 增加标识本次增加是否为替换增加；非级联则替换.
                u8 byReplace = 0;
                if ( !m_ptMtTable->IsMtInMixing(ptMixMt->GetMcuId()) )
                {
                    byReplace = 1;
                }
                cMsg.CatMsgBody((u8*)&byReplace, sizeof(u8));

                if ( *pbyMixNum > 0 )
                {
                    OnAddRemoveMixToSMcu(&cMsg, bAdd);
                }
                ptMixMt += *pbyMixNum;
                pbyMixNum ++ ;
            }
        }
        else
        {
            EqpLog("[AddRemoveSpecMix] No other mcu mem exist, ignore\n");
        }

        // guzh [11/7/2007] 
        /*
        TMcu tMcu;
        tMcu.SetMcuId(LOCAL_MCUID);
        cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
        cServMsg.CatMsgBody((u8*)&tMixParam, sizeof(TMixParam));
        SendMsgToAllMcs(MCU_MCS_MIXPARAM_NOTIF, cServMsg);
        if (!m_tCascadeMMCU.IsNull())
        {
            cServMsg.SetDstMtId(m_tCascadeMMCU.GetMtId());            
            SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_MIXERPARAM_NOTIF, cServMsg);
        }
        */
    }

    // xsl [7/31/2006] 终端混音状态通知上级mcu
    if (!m_tCascadeMMCU.IsNull())
    {
        OnNtfMtStatusToMMcu( m_tCascadeMMCU.GetMtId());
    }

    /*
    // xsl [8/24/2006] 新的定制混音方案不用再通知当前参与混音的通道
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8*)&m_byMixGrpId, 1);
    cServMsg.CatMsgBody(abyMtInMixing, sizeof(abyMtInMixing));
    SendMsgToEqp(m_tMixEqp.GetEqpId(), MCU_MIXER_SETMEMBER_CMD, cServMsg);
    */

    u8 byMtIdx = 0;

    // zbq [11/22/2007] 单纯刷状态的，遵循定时刷新间隔；其他情况实时上报
    if ( NULL == ptMt )
    {
        MtStatusChange( 0, TRUE );
    }
    else
    {
        for( byMtIdx = 0; byMtIdx < byMtNum; byMtIdx++ )
        {
            TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptMt);
            MtStatusChange( tLocalMt.GetMtId(), TRUE );
            ptMt ++;
        }
    }
    
    // zbq [11/01/2007] 恢复支持 无终端停混音策略
    // zbq [06/27/2007] 无成员停混音策略 暂不支持
    // guzh [4/19/2007] 如果当前没有终端参与混音，则停止混音
    u8 byCurMixNum = 0;
    for( byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
    {
        if ( m_ptMtTable->IsMtInMixing(byMtIdx) )
        {
            byCurMixNum ++;
        }
    }
    if ( 0 == byCurMixNum && !bStopMixNonMem )
    {
        CServMsg cServMsg;
        ProcMixStop(cServMsg);
    }

    if ( 0 == byMtNum && !bAdd )
    {
        CServMsg cServMsg;
        ProcMixStop(cServMsg);        
    }
    return;
}

/*====================================================================
    函数名      ：ProcMtMcuApplyMixNotify
    功能        ：处理终端要求参加混音请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/10/14    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcMtMcuApplyMixNotify( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	TMt			tChairman;

	switch( CurState() )
	{
	case STATE_ONGOING:

		//未与会
		//[8/6/2008 xueliang] 增加过滤，即若未开启音频发送通道的与会终端申请插话，拒之
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) || 
			m_tConf.m_tStatus.IsNoMixing() ||
			!m_ptMtTable->IsLogicChnnlOpen(tMt.GetMtId(), LOGCHL_AUDIO, FALSE)
			 
			)
		{
			return;
		}

		//未处在定制混音模式下
		if( !m_tConf.m_tStatus.IsSpecMixing() )
		{
			return;
		}	
		
		// 查询是否已经处于混音组中
		if( LOCAL_MCUID != tMt.GetMcuId() || 
			TRUE == m_ptMtTable->IsMtInMixing( tMt.GetMtId() ) )
		{
			return;
		}

		//有主席
		if( HasJoinedChairman() )
		{
			tChairman = m_tConf.GetChairman();
			
    
			if( tMt.GetMtId() == tChairman.GetMtId() )
			{
				//主席申请插话,同意
				AddRemoveSpecMixMember( &tChairman, 1, TRUE );
				return;
			}
			else
			{
				//通知主席
				cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
				cServMsg.SetDstMtId( tChairman.GetMtId() );			
				SendMsgToMt( tChairman.GetMtId(), MCU_MT_MTAPPLYMIX_NOTIF, cServMsg );			
			}
		}

		//通知会控
		cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
		SendMsgToAllMcs( MCU_MCS_MTAPPLYMIX_NOTIF, cServMsg );

		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMixStart
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/11/09    4.0         张宝卿          创建
====================================================================*/
void CMcuVcInst::ProcMixStart( CServMsg &cServMsg )
{
#define REJECT_MIXER_START( wError )	\
	{ \
		if ( cServMsg.GetEventId() != MCU_MCU_STARTMIXER_CMD && \
			 cServMsg.GetEventId() != MCS_MCU_ADDMIXMEMBER_CMD ) \
		{ \
			cServMsg.SetErrorCode( wError );	\
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2); \
		} \
		else \
		{ \
			NotifyMcsAlarmInfo( 0, wError ); \
		} \
	}

    u8   byMixDepth;
    u8   byLoop;
    TMt *ptMt = NULL;
    u8   byMtIdx = 0;

    //已处于强制广播状态，NACK
    if ( m_tConf.m_tStatus.IsMustSeeSpeaker() ) 
    {
        ConfLog(FALSE, "[MixerCommonReq] IsMustSeeSpeaker is true. nack\n");
        REJECT_MIXER_START(0);
        return;
    }

    s8 *lpMsgBody = NULL;
    u8  byMtNum = 0;
    
    //zbq[11/26/2007] 级联定制、智能混音无条件刷新下级支持
    BOOL32 bMMcuMix = FALSE;

    if (MCU_MCU_STARTMIXER_CMD == cServMsg.GetEventId())
    {
        lpMsgBody = (s8*)(cServMsg.GetMsgBody() + sizeof(TMcu) + sizeof(u8));
        byMtNum = (cServMsg.GetMsgBodyLen()-sizeof(TMcu)-sizeof(u8))/sizeof(TMt);
        bMMcuMix = TRUE;
    }
    else
    {
        lpMsgBody = (s8*)cServMsg.GetMsgBody();
        byMtNum = cServMsg.GetMsgBodyLen()/sizeof(TMt);
    }

    //zbq[11/29/2007] 适应智能混音和定制混音相互切换支持
    BOOL32 bReplaceMixMode = FALSE;
    if ( byMtNum == 0 && m_tConf.m_tStatus.IsSpecMixing() )
    {
        NotifyMcsAlarmInfo( 0, ERR_MCU_SPECMIX_CANCEL_NTF );
        bReplaceMixMode = TRUE;
    }
    else if ( byMtNum > 0 && m_tConf.m_tStatus.IsAutoMixing() )
    {
        NotifyMcsAlarmInfo( 0, ERR_MCU_AUTOMIX_CANCEL_NTF );
        bReplaceMixMode = TRUE;
    }

    ptMt = (TMt*)lpMsgBody;

    if (MCU_MCU_STARTMIXER_CMD == cServMsg.GetEventId())
    {
        //讨论的混音深度必须大于1
        byMixDepth = *(u8*)(cServMsg.GetMsgBody()+sizeof(TMcu));
        if (byMixDepth < 1)
        {
            //FIXME：寻找新的错误码
            //cServMsg.SetErrorCode( ERR_MCU_POOLMEMBERNUM );
            //SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            return;
        }
        else
        {
            if( byMixDepth == 1)
            {
                byMixDepth = 2;
            }
        }
    }
    // guzh [11/7/2007] 忽略混音深度设置
    byMixDepth = GetMaxMixerDepth();

	u8 byEqpId = 0;
	u8 byGrpId = 0;
	if( !bReplaceMixMode &&
        !m_tConf.m_tStatus.IsVACing() && 
		0 == g_cMcuVcApp.GetIdleMixGroup( byEqpId, byGrpId ) )
	{
		REJECT_MIXER_START( ERR_MCU_NOIDLEMIXER_INCONF )
		return;
	}

	//处理指定成员混音时的混音成员
    u8  byJoinedMtNum = m_tConfAllMtInfo.GetLocalJoinedMtNum();

    u16 wError = 0;
    if ( ! CMcuPfmLmt::IsMixOprSupported( m_tConf, GetMixMtNumInGrp(), 
                                          ( byMtNum == 0 ? byJoinedMtNum : byMtNum ) , wError ) )
    {
        REJECT_MIXER_START(wError);
        return;
    }

    TMtListDiscard tOtherMcuMtList;
    
	if( byMtNum > 0 )
	{
        // xsl [7/24/2006] 本地操作：语音激励与定制混音互斥; 级联给上级高优先级.
        if (m_tConf.m_tStatus.IsVACing())
        {
            if ( bMMcuMix )
            {
                NotifyMcsAlarmInfo(0, ERR_MCU_MMCUSPECMIX_VAC);
                ConfLog( FALSE, "[MixerCommonReq] Local VAC mode has been canceled due to cas specmixing\n" );
                
                //FIXME：MixParamNtf
                CServMsg cMsg;
                cMsg.SetEventId(MCU_MCS_STOPVAC_NOTIF);
                SendMsgToAllMcs(MCU_MCS_STOPVAC_NOTIF, cMsg);
                if ( HasJoinedChairman() )
                {
                    cMsg.SetEventId(MCU_MT_STOPVAC_NOTIF);
                    SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cMsg);
                }
            }
            else
            {
                ConfLog(FALSE, "[MixerCommonReq] IsVACMode is true. nack\n"); 
                REJECT_MIXER_START(ERR_MCU_VACMUTEXSPECMIX)
                return;
            }
        }

        //zbq[11/29/2007] 先增加上级MCU进本地混音组
        if( byMtNum > 0 && bMMcuMix && !m_ptMtTable->IsMtInMixing(m_tCascadeMMCU.GetMtId()) )
        {
            m_ptMtTable->SetMtInMixing(m_tCascadeMMCU.GetMtId(), TRUE);
        }

        // xsl [8/4/2006] 定制混音最大为56个
        if (byMtNum > GetMaxMixNum())
        {
            ConfLog(FALSE, "[MixerCommonReq] spec MtNum(%d) can't exceed %d. nack\n", 
                            byMtNum, GetMaxMixNum());
            REJECT_MIXER_START(ERR_MCU_OVERMAXMIXERCHNNL)
			return;
        }

        // zbq [11/26/2007] 级联定制混音冲本地定制混音
        if ( bMMcuMix &&
             m_tConf.m_tStatus.IsSpecMixing() )
        {
            for( byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
            {
                if ( m_tConfAllMtInfo.MtInConf(byMtIdx) &&
                     m_ptMtTable->IsMtInMixing(byMtIdx))
                {
                    TMt tMt = m_ptMtTable->GetMt(byMtIdx);
                    AddRemoveSpecMixMember(&tMt, 1, FALSE, TRUE);
                }
            }
        }
            
		for( byLoop = 0; byLoop < byMtNum; byLoop++ )
		{
            TMt tMt = *ptMt;
            if ( tMt.IsLocal() )
            {
                //直接增加本级终端
                if ( !m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
                {
                    continue;
                }
                // zbq [11/26/2007] 级联定制冲本级定制，此处完成本级终端的增加
                if ( bMMcuMix &&
                     m_tConf.m_tStatus.IsSpecMixing() )
                {
                    AddRemoveSpecMixMember( &tMt, 1 );
                }
                else
                {
                    m_ptMtTable->SetMtInMixing( tMt.GetMtId(), TRUE );
                }
            }
            else
            {
                //增加回传通道
                tMt = GetLocalMtFromOtherMcuMt( *ptMt );
                if ( !m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
                {
                    continue;
                }
                m_ptMtTable->SetMtInMixing( tMt.GetMtId(), TRUE );

                //开启对应下级MCU的混音
                TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMt->GetMcuId());
                if(ptMcInfo == NULL)
                {
                    ConfLog(FALSE, "[ProcMixStart] SMcu.%d unexist, impossible\n", ptMt->GetMcuId());
                    continue;
                }
                tOtherMcuMtList.SetNewMt(*ptMt);
            }
			ptMt++;
		}

        u8  byGrpNum = 0;
        TMt atMt[MAXNUM_SUB_MCU * MAXNUM_MIXING_MEMBER];
        u8  abyMixNum[MAXNUM_SUB_MCU] = { 0 };
        TMt *ptMt = &atMt[0];
        u8  *pbyMixNum = &abyMixNum[0];
        
        tOtherMcuMtList.GetMtList( sizeof(atMt), byGrpNum, pbyMixNum, ptMt );
        
        if ( 0 != byGrpNum )
        {
            for( u8 byGrpIdx = 0; byGrpIdx < byGrpNum; byGrpIdx ++)
            {
                TMcu tMcu;
                tMcu.SetMcu(ptMt->GetMcuId());
                
                CServMsg cMsg;
                cMsg.SetEventId(MCU_MCU_ADDMIXMEMBER_CMD);
                cMsg.SetMsgBody((u8*)&tMcu, sizeof(TMcu));
                cMsg.CatMsgBody((u8*)ptMt, (sizeof(TMt) * *pbyMixNum));

                //zbq[11/29/2007] 增加标识本次增加是否为替换增加；非级联则替换.
                u8 byReplace = 1;
                cMsg.CatMsgBody((u8*)&byReplace, sizeof(u8));
                
                if ( cMsg.GetMsgBodyLen() > sizeof(TMcu)+sizeof(u8) )
                {
                    OnAddRemoveMixToSMcu(&cMsg, TRUE);
                    //OnStartMixToSMcu(&cMsg);
                }
                ptMt += *pbyMixNum;
                pbyMixNum ++ ;
            }
        }
        else
        {
            EqpLog("[AddRemoveSpecMix] No other mcu mem exist, ignore\n");
        }
	}
	else
	{
        // xsl [8/19/2006] 卫星分散会议不支持会议讨论
        if (m_tConf.GetConfAttrb().IsSatDCastMode())
        {
		    REJECT_MIXER_START( ERR_MCU_DCAST_NOOP )            
            ConfLog( FALSE, "[MixerCommonReq] distributed conf not supported discuss\n");
            return;
        }

        // xsl [8/4/2006] 会议讨论不能超过56个终端                
        if (byJoinedMtNum > GetMaxMixNum())
        {
            ConfLog(FALSE, "[MixerCommonReq] discuss MtNum(%d) can't exceed %d. nack\n", 
                    byJoinedMtNum, GetMaxMixNum());
            REJECT_MIXER_START(ERR_MCU_OVERMAXMIXERCHNNL)
			return;
        }

        // xsl [1/10/2006] 请求开启所有直联下级MCU全体混音                 
        TMcu tMcu;
        tMcu.SetMcuId(LOCAL_MCUID);	
        CServMsg cMsg;
        cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) ); 
        cMsg.CatMsgBody( (u8*)&byMixDepth, 1 );
        
        OnStartDiscussToAllSMcu( &cMsg );
	}

    // zbq [06/27/2007] 开讨论或定制混音停当前所有与会终端的音频选看
    for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
    {
        if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) )
        {
            TMtStatus tMtStatus;
            if ( m_ptMtTable->GetMtStatus(byMtId, &tMtStatus) &&
                 !tMtStatus.GetSelectMt(MODE_AUDIO).IsNull() )
            {
                TMt tMt = m_ptMtTable->GetMt(byMtId);
                StopSelectSrc(tMt, MODE_AUDIO);
            }
        }
    }

    //ACK
    if ( cServMsg.GetEventId() != MCU_MCU_STARTMIXER_CMD &&
         cServMsg.GetEventId() != MCS_MCU_ADDMIXMEMBER_CMD )
    {
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
    }

	//开始混音            
	if (m_tConf.m_tStatus.IsVACing() &&
        m_tConf.m_tStatus.IsNoMixing())
	{
        //通知主席及所有会控
        if (byMtNum > 0)
        {
            m_tConf.m_tStatus.SetSpecMixing();
            m_tConf.m_tStatus.SetVACing(FALSE);
        }
        else
        {
            m_tConf.m_tStatus.SetAutoMixing();
        }

        TMcu tMcu;
        tMcu.SetMcu(LOCAL_MCUID);					
        cServMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) ); 
        cServMsg.CatMsgBody( (u8*)&byMixDepth, 1 );
        if(!m_tCascadeMMCU.IsNull())
        {
			cServMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
            SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_STARTMIXER_NOTIF, cServMsg);						
        }
        if (HasJoinedChairman())
        {
            cServMsg.SetMsgBody((u8*)&byMixDepth, 1);
            SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STARTDISCUSS_NOTIF, cServMsg);
        }

        //增加定制混音成员
        for( byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
        {
            if ( m_tConfAllMtInfo.MtJoinedConf(byMtIdx) &&
                 m_ptMtTable->IsMtInMixing(byMtIdx))
            {
                TMt tMt = m_ptMtTable->GetMt(byMtIdx);
                AddRemoveSpecMixMember(&tMt, 1);
            }
        }

        //添加混音成员, 混音器交换码流给mt
        for(u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
        {
            if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
            {
                TMt tMt = m_ptMtTable->GetMt(byMtId);
                StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);	
                SwitchMixMember(&tMt);
            }
        }

        //通知混音器开始向终端发码流
        NotifyMixerSendToMt(TRUE);

        MixerStatusChange();
	}
    else if (m_tConf.m_tStatus.IsSpecMixing())
    {
        //zbq[11/26/2007] 本地或级联智能混音 冲 定制
        if ( byMtNum == 0 )
        {
            //移除当前所有的定制混音成员
            for( byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
            {
                if ( m_tConfAllMtInfo.MtInConf(byMtIdx) &&
                     m_ptMtTable->IsMtInMixing(byMtIdx))
                {
                    TMt tMt = m_ptMtTable->GetMt(byMtIdx);
                    AddRemoveSpecMixMember(&tMt, 1, FALSE, TRUE);
                }
            }
            
            m_tConf.m_tStatus.SetAutoMixing(TRUE);

            //添加混音成员, 交换码流给混音器
            u8 byMixMtNum = 0;
            for (byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
            {
                if (m_tConfAllMtInfo.MtJoinedConf(byMtIdx))
                {
                    TMt tMt = m_ptMtTable->GetMt(byMtIdx);    
                    
                    if (byMixMtNum < GetMaxMixNum())
                    {
                        //通知mixer增加通道，非vac模式将mixer码流交换给mt，建上下行交换                
                        AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE);
                        
                        StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
                                             (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())), 
                                             MODE_AUDIO, SWITCH_MODE_SELECT);
                        byMixMtNum++;
                    }  
                }
            }
            
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                g_cMpManager.StartDistrConfCast(m_tMixEqp, MODE_AUDIO);
            }

            //整理N模式的交换
            for (byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
            {
                if (m_tConfAllMtInfo.MtJoinedConf(byMtIdx) &&
                    !m_ptMtTable->IsMtInMixGrp(byMtIdx))
                {
                    TMt tMt = m_ptMtTable->GetMt(byMtIdx);
                    SwitchMixMember(&tMt, TRUE);
                }           
            }
        }
        else
        {
            // 本地或级联定制混音冲定制，上面已经完成.
        }
        MixerStatusChange();
    }
    else if(m_tConf.m_tStatus.IsAutoMixing())
    {
        //zbq[11/26/2007] 本地或级联定制混音 冲 智能
        if ( byMtNum > 0 )
        {
            m_tConf.m_tStatus.SetSpecMixing(TRUE);

            //讨论模式，须先整理恢复当前的交换
            for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
            {
                if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
                {
                    StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
                                        (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+byMtId), FALSE, MODE_AUDIO);
                    
                    StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);                       
                    
                    // 恢复从组播地址接收
                    if (m_tConf.GetConfAttrb().IsSatDCastMode() && m_ptMtTable->IsMtInMixGrp(byMtId))
                    {
                        ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_AUDIO);
                    }
                    m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);
                }
            }

            //停级联混音
            //zbq[12/06/2007] 针对不同的情况决定是否停下级的混音
            CServMsg cMsg;
            cMsg.SetEventId(MCU_MCU_STOPMIXER_CMD);
            
            TConfMcInfo* ptMcInfo = NULL;
            for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
            {
                ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
                if( ( 0 == ptMcInfo->m_byMcuId ) || 
                    ( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) ||
                    tOtherMcuMtList.IsMcuMixing(ptMcInfo->m_byMcuId) )
                {
                    continue;
                }
                cMsg.SetDstMtId( ptMcInfo->m_byMcuId );
                SendMsgToMt(ptMcInfo->m_byMcuId, MCU_MCU_STOPMIXER_CMD, cMsg);
            }

            // 增加定制混音成员
            for( byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
            {
                if ( m_tConfAllMtInfo.MtJoinedConf(byMtIdx) &&
                     m_ptMtTable->IsMtInMixing(byMtIdx))
                {
                    TMt tMt = m_ptMtTable->GetMt(byMtIdx);
                    AddRemoveSpecMixMember(&tMt, 1);
                }
            }
            // FIXME: 如果没有增加上级MCU为混音成员，在这里手动增加之

            //整理N模式的交换
            for (byMtIdx = 1; byMtIdx < MAXNUM_CONF_MT; byMtIdx++)
            {
                if (m_tConfAllMtInfo.MtJoinedConf(byMtIdx))
                {
                    TMt tMt = m_ptMtTable->GetMt(byMtIdx);
                    if (!m_ptMtTable->IsMtInMixing(byMtIdx))
                    {
                        SwitchMixMember(&tMt, TRUE);
                    }
                }           
            }
        }
        else
        {
            // 上下级智能混音，自然成立. 且级联透传上面已经完成
        }
        MixerStatusChange();
    }
	else
	{
        StartMixing( byMtNum > 0 ? mcuPartMix : mcuWholeMix );    //开始混音
	}

    return;
#undef REJECT_MIXER_START
}

/*====================================================================
    函数名      ：ProcMixStop
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/11/09    4.0         张宝卿          创建
====================================================================*/
void CMcuVcInst::ProcMixStop( CServMsg &cServMsg, BOOL32 bRCStop )
{
    //尚未处于混音状态, NACK 
    if ( m_tConf.m_tStatus.IsNoMixing() )
    {
        cServMsg.SetErrorCode( ERR_MCU_CONFNOTINDISCUSSCMODE );
        if ( cServMsg.GetEventId() != MCU_MCU_STOPMIXER_CMD )
        {
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        }
        return;
    }
    
    //ACK
    if ( MCU_MCU_STOPMIXER_CMD != cServMsg.GetEventId() )
    {
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
    }

    if ( m_tConf.m_tStatus.IsAutoMixing() )
    {
        OnStopDiscussToAllSMcu(&cServMsg);        
    }
    else if ( m_tConf.m_tStatus.IsSpecMixing() )
    {
        for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
        {
            TMt tMt = m_ptMtTable->GetMt(byMtId);
            if ( MT_TYPE_SMCU == tMt.GetMtType() &&
                 ( m_ptMtTable->IsMtInMixing(byMtId) || bRCStop ))
            {
                TMt tSMcu;
                tSMcu.SetMcu(byMtId);
                CServMsg cMsg;
                cMsg.SetMsgBody((u8*)&tSMcu, sizeof(TMt));
                OnStopMixToSMcu( &cMsg );
            }
        }
    }
    
    if (m_tConf.m_tStatus.IsVACing())
    {
        // guzh [11/7/2007] 只设置状态
        m_tConf.m_tStatus.SetNoMixing();
        
        //通知混音器停止向终端发码流
        NotifyMixerSendToMt(FALSE);
        
        //停止混音器向终端的交换
        for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
        {
            if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
            {                        
                StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);
            }
        }
        
        //通知主席及所有会控
        MixerStatusChange();
        
        TMt tMcu;
        tMcu.SetMcu(LOCAL_MCUID);
        cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
        if(!m_tCascadeMMCU.IsNull())
        {
            cServMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
            SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_STOPMIXER_NOTIF, cServMsg);
        }
        if( HasJoinedChairman() )
        {
            SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STOPDISCUSS_NOTIF, cServMsg );
        }
        
        //改变视频源
        if( HasJoinedSpeaker() )
        {
            TMt tSpeakerMt = GetLocalSpeaker();
            ChangeAudBrdSrc( &tSpeakerMt );
        }
        else
        {
            ChangeAudBrdSrc( NULL );
        }                                            
    }
    else
    {
        StopMixing();//停止混音，等ACK回来设置状态
    }

    return;
}


/*------------------------------------------------------------------*/
/*                                 VMP                              */
/*------------------------------------------------------------------*/

/*====================================================================
    函数名      ：IsDynamicVmp
    功能        ：画面合成是否是动态分屏
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/01    3.0         胡昌威         创建
====================================================================*/
BOOL32 CMcuVcInst::IsDynamicVmp( void )
{
//	TPeriEqpStatus tPeriEqpStatus;
//	TVMPParam tVMPParam;

	if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_AUTO )
	{
		return FALSE;
	}

/*
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );  
	tVMPParam = tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam;	
	if( tVMPParam.GetVMPStyle() != VMP_STYLE_DYNAMIC )
	{
		return FALSE;
	}
*/

	return TRUE;
}

/*====================================================================
    函数名      ：SetVmpChnnl
    功能        ：设置画面合成器通道 
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/04    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::SetVmpChnnl(TMt tMt, u8 byChnnl, u8 byMemberType, BOOL32 bMsgMcs)
{
    CServMsg cServMsg;

    TMt tLocalMt = GetLocalMtFromOtherMcuMt(tMt);

    // guzh [5/23/2007] 这里检测终端条件是否在线作为最后保护
    if ( tMt.IsNull() ||
         !m_tConfAllMtInfo.MtJoinedConf(tMt.GetMcuId(), tMt.GetMtId()) || 
         !m_ptMtTable->IsLogicChnnlOpen(tLocalMt.GetMtId(), LOGCHL_VIDEO, FALSE) )
    {
        EqpLog("[SetVmpChnnl] Mt.%d not available for vmp chnl.%d.", tLocalMt.GetMtId(), byChnnl );
        return;
    }

    //开始交换码流
    // guzh [4/24/2007] 照理目前不支持放像进入vmp
    //u8 bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
    u8 bySrcChnnl = (tLocalMt == m_tPlayEqp) ? m_byPlayChnnl : 0;
		 
    // xliang [4/14/2009] 根据具体情况建交换. 
	// 该判断对于模板启画面合成的情况不适用，所以交换还是得建，只是修改bStopBeforeStart值
	BOOL32 bStopBeforeStart = TRUE;
	if (m_tLastVmpParam.IsMtInMember(tMt) 
		&& m_tLastVmpParam.GetChlOfMtInMember(tMt) == byChnnl)
	{
		bStopBeforeStart = FALSE;
		EqpLog("Mt.%u in the channel.%u has no change, so no bStopBeforeStart!\n",
			tMt.GetMtId(), byChnnl);
	}

	StartSwitchToPeriEqp(tLocalMt, bySrcChnnl, m_tVmpEqp.GetEqpId(),
		byChnnl, MODE_VIDEO, SWITCH_MODE_SELECT, FALSE, 
		bStopBeforeStart);

    if (!tMt.IsLocal())
    {
        OnMMcuSetIn(tMt, 0, SWITCH_MODE_SELECT);
    }

    //改变状态
    TVMPMember tVMPMember;
    tVMPMember.SetMemberTMt(tMt);
    tVMPMember.SetMemberType(byMemberType);
    m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byChnnl, tVMPMember);

    //通知会控及主席
    if (bMsgMcs)
    {
        cServMsg.SetMsgBody((u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam));
        SendMsgToAllMcs(MCU_MCS_VMPPARAM_NOTIF, cServMsg);
        if( HasJoinedChairman() )
        {
            SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_VMPPARAM_NOTIF, cServMsg );
        }	
    }

    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
        //把目的的Rtcp交换给源
        TLogicalChannel tLogicalChannel;
        m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);

        u32 dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
        u16 wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();

        //记录交换目的
        SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tVmpEqp, byChnnl, MODE_VIDEO, SWITCH_MODE_SELECT);
    }
}

/*====================================================================
    函数名      ：GetVmpChnnl
    功能        ：得到画面合成器通道
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/04    3.0         胡昌威         创建
====================================================================*/
u8 CMcuVcInst::GetVmpChnnl( void )
{
	u8 byChnnl = 0;
	TVMPMember tVMPMember;
	u8 byMemberNum = m_tConf.m_tStatus.GetVmpParam().GetMaxMemberNum();

    //有空闲通道,返回空闲通道
	while( byChnnl < byMemberNum )
	{
        tVMPMember = *m_tConf.m_tStatus.GetVmpParam().GetVmpMember( byChnnl );
		if( tVMPMember.IsNull() )
		{
			return byChnnl;
		}

        byChnnl ++;
	}

	//没有空闲通道,返回一个未在语音激励结果中的通道
	u8 byChl = 0;
	byChnnl = 0;
	while( byChnnl < byMemberNum )
	{
        tVMPMember = *m_tConf.m_tStatus.GetVmpParam().GetVmpMember( byChl );
		//if( !m_tConf.m_tStatus.m_tDiscussParam.IsMtInMember( (TMt)tVMPMember ) )
        if ( m_ptMtTable->IsMtInMixing(tVMPMember.GetMtId()))
		{
			return byChl;
		}

		byChnnl++;
        byChl ++;
		if( byChl == byMemberNum )
		{
            byChl = 0;
		}
	}
	

	return byChl;
}


/*====================================================================
    函数名      ：ChangeVmpChannelParam
    功能        ：改变会议画面合成方式
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/05/28    3.6         libo          创建    
	7/8/2009    4.6         xliang		  修正+优化（含FIXME）
====================================================================*/
void CMcuVcInst::ChangeVmpChannelParam(TMt * ptNewSrc, u8 byMemberType, TMt * ptOldSrc)
{
    TVMPMember tTmpVmpMember;
    TVMPMember tVmpMember;
    TVMPParam  tVmpParam;    
    TMt        tMt;
    u8         byMaxMemberNum;
    u8         byNewMemberChnl;
    u8         byMemberChnl;
    u8         byLoop;
    memset( &tVmpParam, 0, sizeof(TVMPParam) );
    tVmpParam      = m_tConf.m_tStatus.GetVmpParam();
    byMaxMemberNum = tVmpParam.GetMaxMemberNum();

    // zbq [06/01/2007] 是否真正调整了通道参数
    BOOL32 bVmpParamChged = FALSE;

    if (!m_tConf.m_tStatus.GetVmpParam().IsVMPAuto())
    {
		// xliang [3/20/2009] 为保证高清会议下，MPU没有同一个MT进多个通道，对发言人跟随做策略调整
		TPeriEqpStatus tPeriEqpStatus; 
		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
		u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
		BOOL bConfHD = CMcuPfmLmt::IsConfFormatHD(m_tConf);
		if(	(byMemberType == VMP_MEMBERTYPE_SPEAKER 
			/*|| byMemberType == VMP_MEMBERTYPE_POLL*/)
			&& (byVmpSubType != VMP)
			//&& bConfHD // 可去掉该条件求统一
			)
		{
			AdjustVmpParambyMemberType(ptNewSrc, tVmpParam, byMemberType, ptOldSrc);
			bVmpParamChged = TRUE;
		}
		else
		{
			for (byLoop = 0; byLoop < byMaxMemberNum; byLoop++)
			{
				if ( tVmpParam.GetVmpMember(byLoop)->GetMemberType() == byMemberType )
				{
					StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
					tVmpParam.ClearVmpMember(byLoop);

					// 顾振华 [5/23/2006] 只允许终端进入通道，不允许放像设备
					if (ptNewSrc == NULL || ptNewSrc->GetType() == TYPE_MCUPERI)
					{ 
						tMt.SetNull();
					}
					else
					{
						tMt = *ptNewSrc;
					}

					tVmpMember.SetMemberTMt(tMt);
					tVmpMember.SetMemberType(byMemberType);
					tVmpParam.SetVmpMember(byLoop, tVmpMember);

					bVmpParamChged = TRUE;
				}

				//zgc, 2007-12-24, 没有跟随模式时拖合成终端为发言人或主席
				if ( byMemberType == VMP_MEMBERTYPE_SPEAKER || byMemberType == VMP_MEMBERTYPE_CHAIRMAN )
				{
					if ( ( ptNewSrc != NULL && !ptNewSrc->IsNull() && ptNewSrc->GetType() != TYPE_MCUPERI && tVmpParam.IsMtInMember( *ptNewSrc ) )
						|| ( ptOldSrc != NULL && !ptOldSrc->IsNull() && ptOldSrc->GetType() != TYPE_MCUPERI && tVmpParam.IsMtInMember( *ptOldSrc ) )
						)
					{
						bVmpParamChged = TRUE;
					}
				}
			} 
		}
    }        
    else
    {     
        if (byMemberType == VMP_MEMBERTYPE_SPEAKER)
        {
            byNewMemberChnl = 0;
        }
        else if (byMemberType == VMP_MEMBERTYPE_CHAIRMAN)
        {
            if(HasJoinedSpeaker() && GetLocalSpeaker().GetType() != TYPE_MCUPERI)
			{
				byNewMemberChnl = 1;
			}
			else
			{
				byNewMemberChnl = 0;
			}
			
        }
        else if (byMemberType == VMP_MEMBERTYPE_POLL)
        {
            byNewMemberChnl = 2;
        }

        if (ptNewSrc != NULL)
        {
            if (!ptNewSrc->IsNull())
			{
				if (tVmpParam.IsMtInMember(*ptNewSrc))
				{
					if (byMemberType == VMP_MEMBERTYPE_CHAIRMAN 
						&&
						( (!ptNewSrc->IsNull() && ptNewSrc->GetMtId() == GetLocalSpeaker().GetMtId())
						|| byMaxMemberNum < 2 )
						)
					{
						if(byMaxMemberNum >= 2)
						{
							ConfLog( FALSE, "[ChangeVmpChannelParam] Mt.%u is both speaker and chairman, ignore change!\n", ptNewSrc->GetMtId());
						}
						return;
					}
					
					tVmpMember    = *(tVmpParam.GetVmpMember(byNewMemberChnl)); 
					byMemberChnl  = tVmpParam.GetChlOfMtInMember(*ptNewSrc);
					tTmpVmpMember = *(tVmpParam.GetVmpMember(byMemberChnl));	
					
					if(tVmpMember.GetMtId() == tTmpVmpMember.GetMtId())
					{
						ConfLog( FALSE, "[ChangeVmpChannelParam] vmp member has no change at all, so no need to change vmp param!\n");
						return;	//成员实质无变化，直接return
					}
					
					//清空必要的交换（比如原先1，2，3，4；3-->chairman; 顺序变为3，1，2，4；
					//这里只需拆前3个通道，最后一个通道不用拆）
					for(byLoop = 0; byLoop <= byMemberChnl; byLoop++)
					{
						StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
					}
				}
				else
				{
					//清空当前所有成员交换
					for(byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop ++)
					{
						if( !((tVmpParam.GetVmpMember(byLoop))->IsNull()))
						{
							StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
						}
						
					}
				}
				//这里设成员意义不大.

				bVmpParamChged = TRUE;
			}
			else if(ptOldSrc != NULL && !ptOldSrc->IsNull())	//cancel chairman or speaker. 
			{
				//FIXME：目前无条件调参数, 最终在MPU中过滤
				bVmpParamChged = TRUE;
			}
			else
			{
				//Do nothing
			}
			
			/*
			if (tVmpParam.IsMtInMember(*ptNewSrc))
            {
                if (byMemberType == VMP_MEMBERTYPE_CHAIRMAN 
					&&
                    ( (!ptNewSrc->IsNull() && ptNewSrc->GetMtId() == GetLocalSpeaker().GetMtId())
						|| byMaxMemberNum < 2 )
					)
                {
					if(byMaxMemberNum >= 2)
					{
						ConfLog( FALSE, "[ChangeVmpChannelParam] Mt.%u is both speaker and chairman, ignore change!\n", ptNewSrc->GetMtId());
					}
                    return;
                }
                
                tVmpMember    = *(tVmpParam.GetVmpMember(byNewMemberChnl)); //mt1
                byMemberChnl  = tVmpParam.GetChlOfMtInMember(*ptNewSrc);	//3
                tTmpVmpMember = *(tVmpParam.GetVmpMember(byMemberChnl));	//mt3

                StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byNewMemberChnl, TRUE, MODE_VIDEO);
                StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byMemberChnl, TRUE, MODE_VIDEO);
                
                tVmpParam.SetVmpMember(byMemberChnl, tVmpMember);
                
                tMt = *ptNewSrc;
            }
            else
            {
                StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byNewMemberChnl, TRUE, MODE_VIDEO);
                tVmpParam.ClearVmpMember(byNewMemberChnl);
                
                tMt = *ptNewSrc;
            }

            // 顾振华 [5/23/2006] 只允许终端进入通道，不允许放像设备
            if (tMt.GetType() == TYPE_MCUPERI)
            {
                tMt.SetNull();
            }
            tVmpMember.SetMemberTMt(tMt);
            tVmpMember.SetMemberType(byMemberType);
            tVmpParam.SetVmpMember(byNewMemberChnl, tVmpMember);

            bVmpParamChged = TRUE;
			ConfLog(FALSE, "[ChangeVmpChannelParam] bVmpParamChged is 1!\n");
		*/	
        }
    }

    // zbq [06/01/2007] 通道参数均未调整，则不告知VMP修改参数。否则，引发的此次
    // 无谓的修改将导致交换重新调整，对8000C来说，由于性能限制，合成画面将不能平滑过渡
    if ( bVmpParamChged )
    {
        m_tConf.m_tStatus.SetVmpParam(tVmpParam);
        
        TPeriEqpStatus tPeriEqpStatus;
        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
        tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
        
		AdjustVmpParam(&tVmpParam);
    }

    return;
}

/*====================================================================
    函数名      ：ChangeVmpStyle
    功能        ：改变会议动态画面合成方式
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt tMt 增加或移除的终端
				  BOOL32 bAddMt TRUE-增加 FALSE-移除
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/31    3.0         胡昌威          创建
====================================================================*/
void CMcuVcInst::ChangeVmpStyle(TMt tMt, BOOL32 bAddMt, BOOL32 bChangeVmpParam)
{
	//检查状态是否是动态分屏				
	if( !IsDynamicVmp() )
	{
		return;
	}
	//临时 VCS动态画面合成根据当前所有在线终端进行合成
	//若移除终端,但该终端状态仍为在线,则临时更改该状态,待更改画面合成风格后还原
	u8 byTempChgMtStat = FALSE;

	TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpParam();

	// 对于vcs的vmp动态合成成员不在此处获取
	if (m_tConf.GetConfSource() != VCS_CONF)
	{
		BOOL32 bNeedRemove = FALSE;
		u8 byMemberChl;
		
		TPeriEqpStatus tPeriEqpStatus;
		TVMPMember tVMPMember;
		
		if( tVMPParam.IsMtInMember( tMt ) )
		{
			bNeedRemove = TRUE;
			byMemberChl = tVMPParam.GetChlOfMtInMember( tMt );
		}
		
		//得到当前终端数
		u8 byMtNum = m_tConfAllMtInfo.GetLocalJoinedMtNum();	//对于删除终端进入该函数，此处统计的终端数目是将要被删的终端计算在内的
		
		if( !bAddMt )
		{
			byMtNum--;
			m_tConfAllMtInfo.RemoveJoinedMt(tMt.GetMtId());
		}
		
		u8 byVmpStyle = GetVmpDynStyle( byMtNum ); 
		if (VMP_STYLE_NONE == byVmpStyle)
		{
			EqpLog("[ChangeVmpStyle] GetVmpDynStyle failed, byMtNum :%d\n", byMtNum);
			return;
		}    
		
		//判断是否需要改变Style
		if( byVmpStyle != m_tConf.m_tStatus.GetVmpStyle() )
		{
			tVMPParam.SetVMPStyle( byVmpStyle );
			u8 byMaxMemberNum = tVMPParam.GetMaxMemberNum();
			if( bAddMt )
			{
				u8 byLoop = 0;
				for( byLoop = 0; byLoop < byMaxMemberNum; byLoop++ )
				{
					tVMPMember = *tVMPParam.GetVmpMember( byLoop );
					if( tVMPMember.IsNull() )
					{
						tVMPMember.SetMemberTMt( tMt );
						tVMPMember.SetMemberType( VMP_MEMBERTYPE_VAC );
						tVMPParam.SetVmpMember( byLoop, tVMPMember );
						break;
					}			
				}
				
				if (byLoop >= byMaxMemberNum && bNeedRemove)
				{               
					tVMPParam.ClearVmpMember(byMemberChl);
					
					if (byMemberChl < byMaxMemberNum)
					{
						tVMPMember.SetMemberTMt( tMt );
						tVMPMember.SetMemberType( VMP_MEMBERTYPE_VAC );
						tVMPParam.SetVmpMember( byMemberChl, tVMPMember );
						m_tConf.m_tStatus.SetVmpParam( tVMPParam );                   
					}
					bNeedRemove = FALSE;
				}
			}
			else
			{
				if( bNeedRemove && byMemberChl <= byMaxMemberNum )
				{
					//移除当前通道后，把后面通道前移
					tVMPParam.ClearVmpMember( byMemberChl );
					if( byMemberChl < byMaxMemberNum )
					{
						for( u8 byLoop = byMaxMemberNum; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
						{
							tVMPMember = *tVMPParam.GetVmpMember( byLoop );
							if( !tVMPMember.IsNull() )
							{
								tVMPParam.SetVmpMember( byMemberChl, tVMPMember );
								break;
							}
						}
					}
					
				}
			}
			m_tConf.m_tStatus.SetVmpParam( tVMPParam );
		}
		else
		{
			if( bAddMt )
			{
				u8 byMaxMemberNum = tVMPParam.GetMaxMemberNum();
				u8 byLoop = 0;
				for( ; byLoop < tVMPParam.GetMaxMemberNum(); byLoop++ )
				{
					tVMPMember = *tVMPParam.GetVmpMember( byLoop );
					if( tVMPMember.IsNull() )
					{
						tVMPMember.SetMemberTMt( tMt );
						tVMPMember.SetMemberType( VMP_MEMBERTYPE_VAC );
						tVMPParam.SetVmpMember( byLoop, tVMPMember );
						m_tConf.m_tStatus.SetVmpParam( tVMPParam );
						break;
					}			
				}
				
				if (byLoop >= byMaxMemberNum && bNeedRemove)
				{               
					tVMPParam.ClearVmpMember(byMemberChl);
					
					if (byMemberChl < byMaxMemberNum)
					{
						tVMPMember.SetMemberTMt( tMt );
						tVMPMember.SetMemberType( VMP_MEMBERTYPE_VAC );
						tVMPParam.SetVmpMember( byMemberChl, tVMPMember );
						m_tConf.m_tStatus.SetVmpParam( tVMPParam );                   
					}
					bNeedRemove = FALSE;
				}
			}
			else
			{
				if( bNeedRemove )
				{
					u8 byMaxMemberNum = tVMPParam.GetMaxMemberNum();
					u8 byLoop = byMaxMemberNum-1;
					
					tVMPMember = *tVMPParam.GetVmpMember(byMemberChl);
					
					tVMPParam.ClearVmpMember( byMemberChl );				
					
					if (byMemberChl < byMaxMemberNum)
					{
						for(; byLoop > byMemberChl; byLoop--)
						{
							tVMPMember = *tVMPParam.GetVmpMember(byLoop);
							if (!tVMPMember.IsNull())
							{
								tVMPParam.SetVmpMember(byMemberChl, tVMPMember);
								//同时删除原有交换
								StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
								tVMPParam.ClearVmpMember(byLoop);
								break;
							}
						}
					}
					
					if (byMtNum >= byMaxMemberNum)
					{
						for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
						{
							if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
							{
								continue;
							}
							TMt tMtTmp = m_ptMtTable->GetMt(byMtId);
							if (byMtId != tMt.GetMtId() && !tVMPParam.IsMtInMember(tMtTmp))
							{
								tVMPMember.SetMemberTMt(tMtTmp);
								tVMPMember.SetMemberType(VMP_MEMBERTYPE_VAC);
								tVMPParam.SetVmpMember(byLoop, tVMPMember);
								break;
							}
						}
					}
					m_tConf.m_tStatus.SetVmpParam( tVMPParam );
				}
			}
		}
		
		if (bAddMt && bNeedRemove)
		{
			u8 byMaxMemberNum = tVMPParam.GetMaxMemberNum();
			
			tVMPParam.ClearVmpMember(byMemberChl);
			
			if (byMemberChl < byMaxMemberNum)
			{
				for(u8 byLoop = byMaxMemberNum-1; byLoop >= byMemberChl+1; byLoop--)
				{
					tVMPMember = *tVMPParam.GetVmpMember(byLoop);
					if (!tVMPMember.IsNull())
					{
						tVMPParam.SetVmpMember(byMemberChl, tVMPMember);
						//同时删除原有交换
						StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
						tVMPParam.ClearVmpMember(byLoop);
						break;
					}
				}
			}
			m_tConf.m_tStatus.SetVmpParam(tVMPParam);
        }
    }
	else
	{
		// 临时 VCS动态画面合成根据当前所有在线终端进行合成
		if (FALSE == bAddMt && m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
		{
			m_tConfAllMtInfo.RemoveJoinedMt(tMt.GetMtId());
			byTempChgMtStat = TRUE;
		}
	}

    if (bChangeVmpParam)
    {
	    //ChangeVmpParam(&tVMPParam);
		
		// xliang [1/6/2009] 区分新老VMP设VMP param
		AdjustVmpParam(&tVMPParam);
    }
    else
    {
        SetTimer(MCUVC_CHANGE_VMPPARAM_TIMER, TIMESPACE_CHANGEVMPPARAM);
    }

	if (byTempChgMtStat)
	{
		m_tConfAllMtInfo.AddJoinedMt(tMt.GetMtId());
	}
}

/*====================================================================
    函数名      ：ChangeVmpSwitch
    功能        ：改变画面合成器交换参数
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt * ptNewSrc：源,NULL为所有通道
	              u8 bySrcType        ：成员类型
                  u8 byState          ：VMP目的状态
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/04    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ChangeVmpSwitch( u8 bySrcType, u8 byState)
{
	u8         byLoop;
	TVMPMember tVMPMember;
	TVMPParam  tVMPParam   = m_tConf.m_tStatus.GetVmpParam();    
	u8         byMemberNum = tVMPParam.GetMaxMemberNum();
	u8		   byVmpStyle  = tVMPParam.GetVMPStyle();// xliang [2/20/2009] 取style

	// xliang [12/10/2008] 取vmp子类型
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
	
	BOOL32 bNoneKeda = FALSE; // xliang [12/10/2008] vmp成员是否为非keda的
	
	// xliang [12/17/2008] 清空VMP抢占通道信息（vmp成员发生变化时，所有成员都会重新遍历）
	m_tVmpChnnlInfo.clear();
//	memset(m_abyMtVmpChnnl,0,sizeof(m_abyMtVmpChnnl));
	
	//所有通道停止向VMP交换
	if (byState == VMP_STATE_STOP)
	{
        for (byLoop = 0; byLoop < byMemberNum; byLoop++)
		{
            //停止向VMP的交换
			StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
            
            //取画面合成成员
            tVMPMember = *(tVMPParam.GetVmpMember(byLoop));

            //[1]. 停智能丢包恢复 
            if (m_tConf.GetConfAttrb().IsResendLosePack())
            {
                //把目的的Rtcp交换给源
                TLogicalChannel tLogicalChannel;
                m_ptMtTable->GetMtLogicChnnl(tVMPMember.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);

                u32 dwDstIp  = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
                u16 wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();

                g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIp, wDstPort);
            }
            
            //[2].恢复终端分辨率
			// xliang [12/16/2008] 增加针对新 vmp的恢复终端分辨率处理
			ChangeMtVideoFormat( (TMt)tVMPMember, &m_tLastVmpParam, FALSE, FALSE );
		}

		//若是VMP广播，则把VMP到当前广播终端的交换也停掉。有发言人存在的话，发言人开始广播
		if (tVMPParam.IsVMPBrdst())	
		{
			if (HasJoinedSpeaker())
			{
				TMt tMt = GetLocalSpeaker();
				ChangeVidBrdSrc(&tMt);
			}
			else
			{
				ChangeVidBrdSrc(NULL);
			}
		}

		// xliang [4/14/2009] 主席选看VMP的交换停掉(要单独清)
		if (tVMPParam.IsVMPSeeByChairman())
		{
			EqpLog("[ChangeVmpSwitch] cancel VMP -> chairman!\n");
			StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO);
		}
		return;
	}

	//会控控制模式 
	if (CONF_VMPMODE_CTRL == m_tConf.m_tStatus.GetVMPMode())
	{
		for (byLoop = 0; byLoop < byMemberNum; byLoop++)
		{
			tVMPMember = *tVMPParam.GetVmpMember(byLoop);
			
			if (0 == tVMPMember.GetMemberType() && tVMPMember.IsNull())
			{
				StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
				// xliang [12/15/2008] 清该Mt对应的HD通道(如果存在对应通道).在前面已清掉，此处不需
			}
			else
			{
				// xliang [12/10/2008] 
				u8 byManuId = m_ptMtTable->GetManuId(tVMPMember.GetMtId());
				if( (MT_MANU_KDC != byManuId) 
					&& (MT_MANU_KDCMCU != byManuId) )
				{
					bNoneKeda = TRUE;
				}
				switch (tVMPMember.GetMemberType()) 
				{
				case VMP_MEMBERTYPE_MCSSPEC:
					{
						if (m_tConfAllMtInfo.MtJoinedConf(tVMPMember.GetMtId()))
						{
							// xliang [12/10/2008] 用老vmp时，非科达的不参加画面合成。evpu往后拒
							if( (byVmpSubType == VMP /*|| byVmpSubType == MPU_EVPU*/) 
								&& bNoneKeda == TRUE )
							{
								EqpLog("Old Vmp,not support None Keda Mt!\n");
								
								StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
								TMt tMt;
								tMt.SetNull();
								tVMPMember.SetMemberTMt(tMt);
								tVMPMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
								m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byLoop, tVMPMember);
								
								// xliang [12/16/2008] 上报错误给mcs
								NotifyMcsAlarmInfo(0, ERR_MCU_NONKEDAMT_JOINVMP);
								
							}
							else
							{
								if(byVmpSubType == VMP)
								{
									SetVmpChnnl((TMt)tVMPMember, byLoop, VMP_MEMBERTYPE_MCSSPEC);
									
									//调整终端的分辨率
									ChangeMtVideoFormat( (TMt)tVMPMember, &tVMPParam );
								}
								else
								{
									BOOL32 bRet = FALSE;
									bRet = ChangeMtVideoFormat((TMt)tVMPMember, &tVMPParam);
									
									if(bRet)
									{
										SetVmpChnnl((TMt)tVMPMember, byLoop, VMP_MEMBERTYPE_MCSSPEC);
									}
									else
									{
										//记录tVMPMember对应的chnnl，以便抢占成功后,有通道可依。
										//目前对于非抢占相关的其他原因而导致没有SetVmpChnnl的情形 
										//(如非科达的HD终端使用evpu等)，也一并记录其Chnnl.
										//对于抢占失败这种情况，这里byLoop不可能为0； 为0则肯定用的是无HD前适配通道的VMP
										//m_abyMtVmpChnnl[tVMPMember.GetMtId()-1] = byLoop;
									}
								}
							}
						}
						break;
					}
				case VMP_MEMBERTYPE_SPEAKER://注意改变发言人时改变交换
                    // 顾振华 [5/23/2006] 只允许终端进入通道，不允许放像设备
					if (HasJoinedSpeaker() && GetLocalSpeaker().GetType() != TYPE_MCUPERI)
					{
						// xliang [12/10/2008] 用老vmp时，非科达的不参加画面合成
						if( (byVmpSubType == VMP /*|| byVmpSubType == MPU_EVPU*/) 
							&& bNoneKeda == TRUE )
						{
							ConfLog(FALSE,"Old Vmp not support None Keda Mt!\n");
							StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
							TMt tMt;
							tMt.SetNull();
							tVMPMember.SetMemberTMt(tMt);
							tVMPMember.SetMemberType(VMP_MEMBERTYPE_SPEAKER);
							m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byLoop, tVMPMember);
						}
						else
						{
							// xliang [12/11/2008] 
							if(byVmpSubType == VMP)
							{
								SetVmpChnnl(GetLocalSpeaker(), byLoop, VMP_MEMBERTYPE_SPEAKER);
								
								//调整终端的分辨率
								ChangeMtVideoFormat( (TMt)tVMPMember, &tVMPParam );
							}
							else
							{
								BOOL32 bRet = ChangeMtVideoFormat((TMt)tVMPMember, &tVMPParam);
								if(bRet)
								{
									SetVmpChnnl(GetLocalSpeaker(), byLoop, VMP_MEMBERTYPE_SPEAKER);
								}
								else
								{
									//记录tVMPMember对应的chnnl，以便抢占成功后,有通道可依
									//m_abyMtVmpChnnl[tVMPMember.GetMtId()-1] = byLoop;
								}
							}
						}
					}
					else
					{
						StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
						TMt tMt;
						tMt.SetNull();
						tVMPMember.SetMemberTMt(tMt);
						tVMPMember.SetMemberType(VMP_MEMBERTYPE_SPEAKER);
						m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byLoop, tVMPMember);
					}
					break;

				case VMP_MEMBERTYPE_CHAIRMAN://注意改变主席时改变交换
					if (HasJoinedChairman())
					{
						// xliang [12/10/2008] 用老vmp和evpu时，非科达的不参加画面合成
						if( (byVmpSubType == VMP ) 
							&& bNoneKeda == TRUE )
						{
							ConfLog(FALSE,"Using old Vmp or EVPU, doesn't support None Keda Mt!\n");
							
							StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
							TMt tMt;
							tMt.SetNull();
							tVMPMember.SetMemberTMt(tMt);
							tVMPMember.SetMemberType(VMP_MEMBERTYPE_CHAIRMAN);
							m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byLoop, tVMPMember);
						}
						else
						{
							if(byVmpSubType == VMP)
							{
								SetVmpChnnl(m_tConf.GetChairman(), byLoop, VMP_MEMBERTYPE_CHAIRMAN);
								
								//调整终端的分辨率
								ChangeMtVideoFormat( (TMt)tVMPMember, &tVMPParam );
							}
							else
							{
								BOOL32 bRet = ChangeMtVideoFormat((TMt)tVMPMember, &tVMPParam);
								if(bRet)
								{
									SetVmpChnnl(m_tConf.GetChairman(), byLoop, VMP_MEMBERTYPE_CHAIRMAN);
								}
								else
								{
									//记录tVMPMember对应的chnnl，以便抢占成功后,有通道可依
									//m_abyMtVmpChnnl[tVMPMember.GetMtId()-1] = byLoop;
								}
							}
						}
					}
					else
					{
						StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
						TMt tMt;
						tMt.SetNull();
						tVMPMember.SetMemberTMt(tMt);
						tVMPMember.SetMemberType(VMP_MEMBERTYPE_CHAIRMAN);
						m_tConf.m_tStatus.m_tVMPParam.SetVmpMember( byLoop, tVMPMember);
					}
					break;

				case VMP_MEMBERTYPE_POLL://注意改变轮询广播源时改变交换
					if (m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO || 
						m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_SPEAKER)
					{	
						// xliang [12/10/2008] 用老vmp，非科达的不参加画面合成
						if( (byVmpSubType == VMP ) 
							&& bNoneKeda == TRUE )
						{
							ConfLog(FALSE,"Using old Vmp or EVPU, doesn't support None Keda Mt!\n");
							
							StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
							TMt tMt;
							tMt.SetNull();
							tVMPMember.SetMemberTMt(tMt);
							tVMPMember.SetMemberType(VMP_MEMBERTYPE_POLL);
							m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byLoop, tVMPMember);
						}
						else
						{
							TMt tMt = (TMt)m_tConf.m_tStatus.GetMtPollParam();
							if(byVmpSubType == VMP)
							{
								SetVmpChnnl(tMt, byLoop, VMP_MEMBERTYPE_POLL);								
								//调整终端的分辨率
								ChangeMtVideoFormat( (TMt)tVMPMember, &tVMPParam );
							}
							else
							{
								BOOL32 bRet = ChangeMtVideoFormat((TMt)tVMPMember, &tVMPParam);
								if(bRet)
								{
									SetVmpChnnl(tMt, byLoop, VMP_MEMBERTYPE_POLL);
								}
								else
								{
									//FIXME：暂不做事情
								}
							}
						}
					}
					else
					{
						StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);

						TMt tMt;
						tMt.SetNull();
						tVMPMember.SetMemberTMt(tMt);
						tVMPMember.SetMemberType(VMP_MEMBERTYPE_POLL);
						m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byLoop, tVMPMember);
					}
					break;
					
				default:
					break;
				}
			}
		}	
		
		// xliang [12/22/2008] FIXME:新VMP的会议模板处理
		//有会议设备模板时的处理, 只在会议开始时使用，停止VMP后再次复合，则不再使用
		if( byState == VMP_STATE_START && 
			m_tConf.GetConfAttrb().IsHasVmpModule() && 
            !m_tConfInStatus.IsVmpModuleUsed() )
		{
            u8 byMemberType = 0;			
			for( u8 byLoop = 0; byLoop < byMemberNum; byLoop++ )
			{
                // 顾振华 [5/11/2006] 这里支持指定通道类型
				TMt tMt = m_tConfEqpModule.m_atVmpMember[byLoop];
                byMemberType = m_tConfEqpModule.m_tVmpModule.m_abyMemberType[byLoop];
				if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) ||
                    byMemberType != VMP_MEMBERTYPE_MCSSPEC)
				{
                    // 如果与会
                    if (m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ))
                    {
                        SetVmpChnnl( tMt, byLoop, byMemberType, TRUE );

                        //调整终端的分辨率
                        ChangeMtVideoFormat( (TMt)tVMPMember, &tVMPParam );
                    }
                    else
                    {
                        // 否则设置状态
						tMt.SetNull();
						tVMPMember.SetMemberTMt(tMt);
						tVMPMember.SetMemberType(byMemberType);
						m_tConf.m_tStatus.m_tVMPParam.SetVmpMember( byLoop, tVMPMember);
                    }					
				}
			}			
		}

        if (VMP_STATE_START == byState && m_tConf.m_tStatus.IsBrdstVMP())
        {
            ChangeVidBrdSrc(&m_tVmpEqp);
        }
	}
  
	//自动设置成员模式
	// xliang [7/3/2009] 简化重整
	if( m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_AUTO )
	{
		u8 byMember = 0;
		if( VMP_STATE_START  == byState ||
            VMP_STATE_CHANGE == byState )
		{                
			//清空成员
			for( byLoop = 0; byLoop < byMemberNum; byLoop++ )
			{
				if (m_tConf.GetConfAttrb().IsSatDCastMode())
				{
					StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
				}
                
				m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );			
			} 

			for (byLoop = 0; byLoop < byMemberNum; byLoop++)
			{
				tVMPMember = *tVMPParam.GetVmpMember(byLoop);
				
				if (tVMPMember.IsNull())
				{
					StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
				}
				else
				{
					if (m_tConfAllMtInfo.MtJoinedConf(tVMPMember.GetMtId()))
					{
						BOOL32 bRet = FALSE;
						bRet = ChangeMtVideoFormat((TMt)tVMPMember, &tVMPParam);
						if(bRet)
						{
							SetVmpChnnl((TMt)tVMPMember, byLoop, tVMPMember.GetMemberType());
						}
					}
				}
			}
			if (VCS_CONF == m_tConf.GetConfSource())
			{
				// 挂断被踢出画面合成器的成员
				CServMsg cServMsg;
				if ( CONF_VMPMODE_NONE != m_tLastVmpParam.GetVMPMode() )
				{
					for( s32 nIndex = 0; nIndex < m_tLastVmpParam.GetMaxMemberNum(); nIndex ++ )
					{
						TMt tVMPMemberOld = *(TMt *)m_tLastVmpParam.GetVmpMember(nIndex);
        
						if ( !m_tConf.m_tStatus.m_tVMPParam.IsMtInMember(tVMPMemberOld) &&
							m_tConfAllMtInfo.MtJoinedConf(tVMPMemberOld.GetMcuId(), tVMPMemberOld.GetMtId())) 
						{
							if (!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
							{
								VCSDropMT(tVMPMemberOld);
							}
							m_cVCSConfStatus.SetCurUseVMPChanInd(nIndex + 1);
						}
					}             
				}
			}
			/*
			if (VCS_CONF == m_tConf.GetConfSource())
			{
				TMt atMtMember[MAXNUM_VMP_MEMBER];
				u8 byStyle = GetVCSAutoVmpMember(atMtMember);
				if ( byStyle != VMP_STYLE_NONE)
				{
					SetVCSVmpMemInChanl(atMtMember, byStyle);
				}
			}
			else
			{
				//清空成员
				for( byLoop = 0; byLoop < byMemberNum; byLoop++ )
				{
					if (m_tConf.GetConfAttrb().IsSatDCastMode())
					{
						StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
					}
                
					m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );			
				} 
				// xliang [12/16/2008] 对于老vmp保持原有策略
				if(byVmpSubType == VMP) 
				{
					//若有发言人,占用当前最显著空通道,即第0通道
					// guzh [8/25/2006] 这里也要判断放像不进入通道
					if (HasJoinedSpeaker() && GetLocalSpeaker().GetType() != TYPE_MCUPERI)
					{
						SetVmpChnnl(GetLocalSpeaker(), byMember, VMP_MEMBERTYPE_SPEAKER);
						byMember++;
						
						//调整发言人的分辨率
						ChangeMtVideoFormat( GetLocalSpeaker(), &tVMPParam );
					}
					
					//若有主席,占用当前最显著空通道(无发言人为0通道,有则为1通道)
					if (HasJoinedChairman() &&
						m_tConf.GetChairman().GetMtId() != GetLocalSpeaker().GetMtId())
					{
						SetVmpChnnl(m_tConf.GetChairman(), byMember, VMP_MEMBERTYPE_CHAIRMAN);
						byMember++;
						
						//调整主席的分辨率
						ChangeMtVideoFormat( m_tConf.GetChairman(), &tVMPParam );
					}               
					
					//其他终端依次占后续空通道
					for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
					{		
						if (byMember >= byMemberNum)
						{
							break;
						}
						
						if (m_tConfAllMtInfo.MtJoinedConf(byLoop))
						{
							if (byLoop == m_tConf.GetChairman().GetMtId())
							{
								// 主席在前面
								continue;
							}
							if (byLoop == GetLocalSpeaker().GetMtId() &&
								GetLocalSpeaker().GetType() != TYPE_MCUPERI)
							{
								// 发言人在前面
								continue;
							}
							
							TMt tMt = m_ptMtTable->GetMt(byLoop);
							SetVmpChnnl(tMt, byMember, VMP_MEMBERTYPE_VAC);
							byMember++;
							
							//调整终端的分辨率
							ChangeMtVideoFormat( tMt, &tVMPParam );
						}				
					}
				}
				else // xliang [12/16/2008] 针对evpu, mpu的处理
				{
					/*
					//若有发言人,占用当前最显著空通道,即第0通道
					// guzh [8/25/2006] 这里也要判断放像不进入通道
					BOOL32 bRet = TRUE;
					if (HasJoinedSpeaker() && GetLocalSpeaker().GetType() != TYPE_MCUPERI)
					{
						// xliang [12/17/2008] 不考虑在VMP成员忽略列表中的终端
						if(!IsMtNeglectedbyVmp(GetLocalSpeaker().GetMtId()) )
						{
							bRet = ChangeMtVideoFormat(GetLocalSpeaker(), &tVMPParam);
							
							if(bRet)
							{
								SetVmpChnnl(GetLocalSpeaker(), byMember, VMP_MEMBERTYPE_SPEAKER);
								byMember++;
							}
							else
							{
								//m_abyMtVmpChnnl[GetLocalSpeaker().GetMtId()-1] = byMember;
								byMember++;
							}
						}
					}
					
					//若有主席,占用当前最显著空通道(无发言人为0通道,有则为1通道)
					if (HasJoinedChairman() &&
						m_tConf.GetChairman().GetMtId() != GetLocalSpeaker().GetMtId())
					{
						// xliang [12/17/2008] 不考虑在VMP成员忽略列表中的终端
						if(!IsMtNeglectedbyVmp(m_tConf.GetChairman().GetMtId()) )
						{
							bRet = ChangeMtVideoFormat(m_tConf.GetChairman(), &tVMPParam);
							if(bRet)
							{
								SetVmpChnnl(m_tConf.GetChairman(), byMember, VMP_MEMBERTYPE_CHAIRMAN);
								byMember++;
							}
							else
							{
								//m_abyMtVmpChnnl[m_tConf.GetChairman().GetMtId()-1] = byMember;
								byMember++;
							}

						}
					}               
					
					//其他终端依次占后续空通道
					for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
					{		
						if (byMember >= byMemberNum)
						{
							break;
						}
						
						if (m_tConfAllMtInfo.MtJoinedConf(byLoop))
						{
							if (byLoop == m_tConf.GetChairman().GetMtId())
							{
								// 主席在前面
								continue;
							}
							if (byLoop == GetLocalSpeaker().GetMtId() &&
								GetLocalSpeaker().GetType() != TYPE_MCUPERI)
							{
								// 发言人在前面
								continue;
							}
							
							// xliang [12/17/2008] 不考虑在VMP成员忽略列表中的终端
							if (IsMtNeglectedbyVmp(byLoop))
							{
								continue;
							}
							TMt tMt = m_ptMtTable->GetMt(byLoop);
							bRet = ChangeMtVideoFormat(tMt, &tVMPParam);
							if(bRet)
							{
								SetVmpChnnl(tMt, byMember, VMP_MEMBERTYPE_VAC);
								byMember++;
							}
							else
							{
								//m_abyMtVmpChnnl[tMt.GetMtId()-1] = byMember;
								byMember++;
							}
						}		
					}
				}
			}*/

		}
		else //停止自动画面合成
		{
			for( byLoop = 0; byLoop < byMemberNum; byLoop++ )
			{
				tVMPMember = *tVMPParam.GetVmpMember( byLoop );
				if( !tVMPMember.IsNull() && m_tConfAllMtInfo.MtJoinedConf( tVMPMember.GetMtId() ) )
				{
					SetVmpChnnl( (TMt)tVMPMember, byLoop, tVMPMember.GetMemberType() );

                    //恢复终端的分辨率
					// xliang [12/16/2008] 增加针对新 vmp的恢复终端分辨率处理
					ChangeMtVideoFormat((TMt)tVMPMember,&m_tLastVmpParam, FALSE);
				}			
			} 				
		}
        
        if (VMP_STATE_START == byState && m_tConf.m_tStatus.IsBrdstVMP())
        {
            ChangeVidBrdSrc(&m_tVmpEqp); 
        }
	}
	// xliang [4/14/2009] 主席选看VMP处理
	if ( tVMPParam.IsVMPSeeByChairman() )
	{
		CServMsg cMsg;
		cMsg.SetNoSrc();
		cMsg.SetEventId(MT_MCU_STARTSWITCHVMPMT_REQ);
		ChairmanSeeVmp(m_tConf.GetChairman(), cMsg);
	}

    //节省带宽时，改变画面合成成员时，需要控制非keda厂商mt码流是否发送...
	
	CServMsg cServMsg;
	cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam) );
	SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg ); //告诉会控刷界面
    
	// 更改合成分辨率参数
	if (VMP_STATE_CHANGE == byState)
    {
        if ( CONF_VMPMODE_NONE != m_tLastVmpParam.GetVMPMode() )
        {
            // xliang [3/13/2009] 对上次在通道内的Mt，而本次由于HD适配通道被占的情况，要清该MT到VMP的交换
// 			for( u8 byIdx = 1; byIdx <= MAXNUM_CONF_MT; byIdx ++)
// 			{
// 				if( m_abyMtVmpChnnl[byIdx-1] !=0 )
// 				{
// 					StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), m_abyMtVmpChnnl[byIdx-1], FALSE, MODE_VIDEO);
// 				}
// 			}
			for( s32 nIndex = 0; nIndex < m_tLastVmpParam.GetMaxMemberNum(); nIndex ++ )
            {
                TMt tVMPMemberOld = *(TMt *)m_tLastVmpParam.GetVmpMember(nIndex);
                
                //对删除了的合成成员恢复其发送分辨率
                if ( !m_tConf.m_tStatus.m_tVMPParam.IsMtInMember(tVMPMemberOld) &&
                    m_tConfAllMtInfo.MtJoinedConf(tVMPMemberOld.GetMtId()) ) 
                {
                    // xliang [12/16/2008] 增加针对新 vmp的恢复终端分辨率处理
					ChangeMtVideoFormat(tVMPMemberOld, &m_tLastVmpParam, FALSE, FALSE);
				}
            }             
        }   
    }

	//保存本次画面合成参数，用于h264动态分辨率恢复
	memcpy(&m_tLastVmpParam, &m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam));

    if(g_bPrintEqpMsg)
    {
        OspPrintf(TRUE, FALSE, "[ChangeVmpSwitch] The result vmp param:");
        m_tConf.m_tStatus.m_tVMPParam.Print();
    }
    
    if( HasJoinedChairman() )
    {
        SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_VMPPARAM_NOTIF, cServMsg );        
    }
	
	// xliang [4/20/2009] 在一次处理改变VMP param过程中，用户快速又请求另一种change vmp param
	// (实际场景主要是切换风格后，立马勾选/取消"广播码流"按钮), 可能会导致实际效果与操作不符。
	// 为解决该问题，严格来讲应把VMP从改变参数到建完交换(广播/非广播)封装成原子操作，期间相关的请求不做响应。[FIXME]
	// 这里退一步, 把VMP从改变参数到发出交换信令完成 作为原子
	m_byVmpOperating = 0;

}

/*=============================================================================
    函 数 名： CancelOneVmpFollowSwitch
    功    能： 清空会控模式下的指定跟随类型的相应复合交换及状态
    算法实现： 
    全局变量： 
    参    数： u8 byMemberType
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/03/21  3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::CancelOneVmpFollowSwitch( u8 byMemberType, BOOL32 bNotify )
{
	//会控控制模式 
	if( CONF_VMPMODE_CTRL != m_tConf.m_tStatus.GetVMPMode() || 
		VMP_MEMBERTYPE_MCSSPEC == byMemberType )
	{
		return;
	}

	u8 byLoop;
	TVMPMember tVMPMember;
	TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpParam();
	u8 byMemberNum = tVMPParam.GetMaxMemberNum();

	for( byLoop = 0; byLoop < byMemberNum; byLoop++ )
	{
		tVMPMember = *m_tConf.m_tStatus.GetVmpParam().GetVmpMember( byLoop );
		if( !tVMPMember.IsNull() && 
			byMemberType == tVMPMember.GetMemberType() )
		{
			StopSwitchToPeriEqp( m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO );
			
			TMt tMt;
			tMt.SetNull();
			tVMPMember.SetMemberTMt( tMt );
			tVMPMember.SetMemberType( byMemberType );
			m_tConf.m_tStatus.m_tVMPParam.SetVmpMember( byLoop, tVMPMember);

			//同步外设复合状态
			TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
			tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
			g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
		}
	}

	if( bNotify )
	{
		CServMsg cServMsg;
		cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam) );

		SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
		
		TPeriEqpStatus tVmpEqpStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tVmpEqpStatus );
		cServMsg.SetMsgBody( ( u8 * )&tVmpEqpStatus, sizeof( tVmpEqpStatus ) );
		SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );
	}

	return;
}

/*====================================================================
    函数名      ：ChangeVmpBitRate
    功能        ：改变画面合成的码率 
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wBitRate, 画面合成的码率
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/05/21    3.0         胡昌威         创建
====================================================================*/
//modify bas 2
void CMcuVcInst::ChangeVmpBitRate(u16 wBitRate, u8 byVmpChanNo/* = 1*/)
{
    TPeriEqpStatus tVmpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpStatus);
    u8 byVmpSubType = tVmpStatus.m_tStatus.tVmp.m_bySubType;
	if( MPU_SVMP == byVmpSubType)
	{
		AdjustVmpBrdBitRate();
		return;
	}

	u16 wVmpBitRate = htons(wBitRate);

    CServMsg cServMsg;
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetChnIndex(byVmpChanNo);
    cServMsg.SetMsgBody((u8*)&wVmpBitRate, sizeof(u16));	
    SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_SETCHANNELBITRATE_REQ, cServMsg);

    u16 wOriBitrate = 0;

    if (1 == byVmpChanNo)
    {
        wOriBitrate = m_wVidBasBitrate;
        m_wVidBasBitrate = wBitRate;
    }
    else if (2 == byVmpChanNo)
    {
        wOriBitrate = m_wBasBitrate;
        m_wBasBitrate = wBitRate;
    }

    EqpLog("[ChangeVmpBitrate] ChanNo.%d be changed from BR.%d to BR.%d\n", byVmpChanNo, wOriBitrate, wBitRate);

    return;
}

/*====================================================================
    函数名      ：AdjustVmpBrdBitRate
    功能        ：调整画面合成的当前输出码率
    算法实现    ：兼容新老VMP，调整某通道的最低接收能力
    引用全局变量：
    输入参数说明：TMt *		[in]	老VMP用作调通道号的参考，对于MPU无意义
    返回值说明  ：u8		对于老VMP返回调整的实际通道号，对于MPU无意义
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/08/04    4.6         薛亮          创建
====================================================================*/
u8 CMcuVcInst::AdjustVmpBrdBitRate( TMt *ptMt /*= NULL*/)
{
    //本调整只针对单速会议VMP的升降速, byVmpChanNo也是为后续扩展预留的.
//     if ( 0 != m_tConf.GetSecBitRate() )
//     {
//         return;
//     }
    
	u16 wMinMtReqBitRate = 0;
	u8  byMediaType = 0;
	TPeriEqpStatus tVmpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpStatus);
    u8 byVmpSubType = tVmpStatus.m_tStatus.tVmp.m_bySubType;
	if( VMP == byVmpSubType)
	{
		if( NULL == ptMt || ptMt->IsNull() )
		{
			EqpLog("[AdjustVmpBrdBitRate] ptMt is NULL as using old VMP!\n ");
			return 0;
		}
		u8 byVmpChanNo = 0;
		TSimCapSet tDstSimCapSet = m_ptMtTable->GetDstSCS(ptMt->GetMtId());
		byMediaType = m_tConf.GetMainVideoMediaType();
		//双速会议要考虑入会时的呼叫码率
        if ( 0 != m_tConf.GetSecBitRate() && 
            MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
        {
            if (m_ptMtTable->GetDialBitrate(ptMt->GetMtId()) == m_tConf.GetBitRate())
            {
                byVmpChanNo = 1;
            }
            else
            {
                byVmpChanNo = 2;
            }
        }
        //单速或双格式会议
        else
        {
            if (tDstSimCapSet.GetVideoMediaType() == byMediaType)
            {
                byVmpChanNo = 1;
            }
            else
            {
                byVmpChanNo = 2;
            }
        }

		u8 byEncType = m_tConf.GetCapSupport().GetMainSimCapSet().GetVideoMediaType();
		wMinMtReqBitRate = GetLeastMtReqBitrate( TRUE, byEncType );
		
		u16 wOriBitrate = 0;
		if ( wMinMtReqBitRate != m_awVMPBrdBitrate[byVmpChanNo-1] )
		{
			wOriBitrate = m_awVMPBrdBitrate[byVmpChanNo-1];
			m_awVMPBrdBitrate[byVmpChanNo-1] = wMinMtReqBitRate;
			u16 wVmpBitRate = htons(wMinMtReqBitRate);
			
			CServMsg cServMsg;
			cServMsg.SetConfId(m_tConf.GetConfId());
			cServMsg.SetChnIndex(byVmpChanNo);
			cServMsg.SetMsgBody((u8*)&wVmpBitRate, sizeof(u16));	
			SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_SETCHANNELBITRATE_REQ, cServMsg);
			
			EqpLog("[AdjustVmpBrdBitrate] ChanNo.%d be changed from BR.%d to BR.%d\n", byVmpChanNo, wOriBitrate, wMinMtReqBitRate);
		}

		return (byVmpChanNo - 1);
	}

	u8 byBoardVer	= tVmpStatus.m_tStatus.tVmp.m_byBoardVer;
	u8 byVmpOutChnnl = 0;
	u8 byChnnlRes	= 0;

	for (; byVmpOutChnnl < MAXNUM_MPU_OUTCHNNL; byVmpOutChnnl ++)
	{
		
		if (byBoardVer == MPU_BOARD_A128 && byVmpOutChnnl == 3) //A板只有3路
		{
			ConfLog( FALSE, "MPU A version, only 3 out channel!\n" );
			break;
		}
		byChnnlRes = GetResByVmpOutChnnl(byMediaType, byVmpOutChnnl, m_tConf, m_tVmpEqp.GetEqpId());
		
		wMinMtReqBitRate = GetLstRcvGrpMtBr(TRUE, byMediaType, byChnnlRes);

		u16 awOriBitrate[MAXNUM_MPU_OUTCHNNL] = {0};
		if ( wMinMtReqBitRate != m_awVMPBrdBitrate[byVmpOutChnnl] )
		{
			awOriBitrate[byVmpOutChnnl] = m_awVMPBrdBitrate[byVmpOutChnnl];
			m_awVMPBrdBitrate[byVmpOutChnnl] = wMinMtReqBitRate;
			u16 wVmpBitRate = htons(wMinMtReqBitRate);
			
			CServMsg cServMsg;
			cServMsg.SetConfId(m_tConf.GetConfId());
			cServMsg.SetChnIndex(byVmpOutChnnl);
			cServMsg.SetMsgBody((u8*)&wVmpBitRate, sizeof(u16));	
			SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_SETCHANNELBITRATE_REQ, cServMsg);
			
			EqpLog("[AdjustVmpBrdBitrate] ChanNo.%d be changed from BR.%d to BR.%d\n", 
				byVmpOutChnnl, awOriBitrate[byVmpOutChnnl], wMinMtReqBitRate);
		}
	}
    return 1;
}

/*====================================================================
    函数名      ：UpdataVmpEncryptParam
    功能        ：改变画面合成的编解码参数
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt tMt
                  u8  byChnlNo
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/04/18    3.6         libo          创建
====================================================================*/
void CMcuVcInst::UpdataVmpEncryptParam(TMt tMt, u8 byChnlNo)
{
    TVMPParam tConfVMPParam;
    TMediaEncrypt  tVideoEncrypt;
    TDoublePayload tDoublePayload;

    tConfVMPParam = m_tConfEqpModule.m_tVmpModule.m_tVMPParam;

	if (m_tConfEqpModule.m_atVmpMember[byChnlNo].GetMtId() != tMt.GetMtId())
    {
        return;
    }

    BOOL32 bRet = FALSE;
    TLogicalChannel tChnnl;

    bRet = m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tChnnl, FALSE);
    if (bRet == TRUE)
    {
        tVideoEncrypt = tChnnl.GetMediaEncrypt();
        tDoublePayload.SetRealPayLoad(tChnnl.GetChannelType());
	    tDoublePayload.SetActivePayload(tChnnl.GetActivePayload());
    }

    CServMsg cServMsg;
    cServMsg.SetConfId( m_tConf.GetConfId() );
    cServMsg.SetMsgBody( (u8*)&byChnlNo, sizeof(u8) );
    cServMsg.CatMsgBody( (u8*)&tVideoEncrypt, sizeof(TMediaEncrypt) );
    cServMsg.CatMsgBody( (u8*)&tDoublePayload, sizeof(TDoublePayload) );

    SendMsgToEqp( m_tVmpEqp.GetEqpId(), MCU_VMP_UPDATAVMPENCRYPTPARAM_REQ, cServMsg );

}

/*====================================================================
    函数名      ：ChangeVmpParam
    功能        ：画面合成参数改变
    算法实现    ：
    引用全局变量：
    输入参数说明：TVMPParam *ptVMPParam, 画面合成参数
				  BOOL32 bStart 是否是开始画面合成
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/04    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ChangeVmpParam(TVMPParam *ptVMPParam, BOOL32 bStart)
{
	if (ptVMPParam == NULL)
	{
		return;
	}

    // guzh [7/25/2007] 当前会议的合成参数，本函数不会保存风格等参数，而是等VMP回应再保存
    // 在此之前会 SetPeriEqpStatus 中临时保存
    // 在VMP回应之前的GetVmpParam会返回以前的Param    
	TVMPParam tConfVMPParam = m_tConf.m_tStatus.GetVmpParam();
    
    u16 wMinMtReqBitrate = 0;
    u8  byRes;

    //设置参数
    CKDVVMPParam cKDVVMPParam;
    memset(&cKDVVMPParam, 0, sizeof(cKDVVMPParam));
    cKDVVMPParam.m_byVMPStyle = ptVMPParam->GetVMPStyle();
    cKDVVMPParam.m_byEncType = m_tConf.GetMainVideoMediaType();
    
    //先判断双速单格式会议的情况：第一通道为会议码率，第二通道为非会议码率的最低码率（>=会议辅助码率）
    if (0 != m_tConf.GetSecBitRate() && 
        MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
    {
        wMinMtReqBitrate = m_tConf.GetBitRate();
    }
    else
    {
        wMinMtReqBitrate = GetLeastMtReqBitrate(TRUE, cKDVVMPParam.m_byEncType);
        //m_awVMPBrdBitrate[0] = wMinMtReqBitrate;
    }
    m_awVMPBrdBitrate[0] = wMinMtReqBitrate;
//     if (wMinMtReqBitrate < m_wVidBasBitrate || 0 == m_wVidBasBitrate)
//     {
//         m_wVidBasBitrate = wMinMtReqBitrate;
//     }
    cKDVVMPParam.m_wBitRate = htons(wMinMtReqBitrate);

    
    u16 wWidth = 0;
    u16 wHeight = 0;
    m_tConf.GetVideoScale(cKDVVMPParam.m_byEncType,
                          wWidth,
                          wHeight);
    cKDVVMPParam.m_wVideoWidth = wWidth;
    cKDVVMPParam.m_wVideoHeight = wHeight;


    //现在约定16CIF即是2CIF,规则按照AUTO处理, xsl [8/11/2006] mpeg2 4cif按2cif处理
    byRes = m_tConf.GetVideoFormat(cKDVVMPParam.m_byEncType);
    if( VIDEO_FORMAT_16CIF == byRes ||
        (MEDIA_TYPE_H262 == cKDVVMPParam.m_byEncType && VIDEO_FORMAT_4CIF == byRes))
    {
        cKDVVMPParam.m_wVideoWidth  = 352;
        cKDVVMPParam.m_wVideoHeight = 576;
    }
    
	cKDVVMPParam.m_wVideoWidth  = htons(cKDVVMPParam.m_wVideoWidth);
	cKDVVMPParam.m_wVideoHeight = htons(cKDVVMPParam.m_wVideoHeight);
	cKDVVMPParam.m_byMemberNum  = ptVMPParam->GetMaxMemberNum();

    TMt tSpeakerMt = GetLocalSpeaker();
    // 顾振华 [5/23/2006] 只允许终端进入通道，不允许放像设备
    if (tSpeakerMt.GetType() == TYPE_MCUPERI)
    {
        memset(&tSpeakerMt, 0, sizeof(TMt));
    }
    TMt tChairMt = m_tConf.GetChairman();
    
    u8 byMemberNum = ptVMPParam->GetMaxMemberNum();
    u8 byMtId = 1;
    u8 byMember = 0;
    u8 byLoop = 0;
	// 对于VCS会议画面合成成员选定
	if (VCS_CONF == m_tConf.GetConfSource())
	{
		TMt tVmpMember[MAXNUM_VMP_MEMBER];
		memset(tVmpMember, 0, sizeof(TMt) * MAXNUM_VMP_MEMBER);
		if (ptVMPParam->IsVMPAuto())
		{
			u8 byStyle = GetVCSAutoVmpMember(tVmpMember);
			if (byStyle != VMP_STYLE_NONE)
			{
				ptVMPParam->SetVMPStyle(byStyle);
				cKDVVMPParam.m_byVMPStyle = byStyle;
			}
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[ChangeVmpParam]only auto vmp to vcs currently");
			return;
		}

		for (u8 byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++)
		{
			if (!tVmpMember[byLoop].IsNull())
			{
				cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(tVmpMember[byLoop].GetMtId()) );
				cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(tVmpMember[byLoop]);
				cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_AUDIENCE);				
				ptVMPParam->SetVmpMember(byMember, cKDVVMPParam.m_atMtMember[byMember]);
				byMember ++;
			}
		}
		cKDVVMPParam.m_byMemberNum = byMember;

	}
	else
	{
		if(ptVMPParam->IsVMPAuto())
		{        
			//先填发言人，再填主席，再填观众
			if(!tSpeakerMt.IsNull() && m_tConfAllMtInfo.MtJoinedConf(tSpeakerMt.GetMtId()))
			{
				// 终端别名
				cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(tSpeakerMt.GetMtId()) );
				cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(tSpeakerMt);
				cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_SPEAKER);
				byMember ++;
			}
			if(!tChairMt.IsNull() &&
			   !(tChairMt == tSpeakerMt) &&
			   m_tConfAllMtInfo.MtJoinedConf(tChairMt.GetMtId()))
			{
				// 终端别名
				cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(tChairMt.GetMtId()) );
				cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(tChairMt);
				cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_CHAIRMAN);
				byMember ++;
			}
			for (byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
			{
				if (byMember >= byMemberNum)
				{
					// 数量满
					break;
				}
				if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
				{
					if (byMtId == m_tConf.GetChairman().GetMtId())
					{
						// 主席在前面
						continue;
					}
					if (byMtId == GetLocalSpeaker().GetMtId() &&
						GetLocalSpeaker().GetType() != TYPE_MCUPERI)
					{
						// 发言人在前面
						continue;
					}

					// 终端别名
					cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(byMtId) );
					cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(m_ptMtTable->GetMt(byMtId));
					cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_AUDIENCE);
					byMember ++;
				}
			}

			for(byLoop = 0; byLoop < cKDVVMPParam.m_byMemberNum; byLoop ++)
			{
				ptVMPParam->SetVmpMember(byLoop, cKDVVMPParam.m_atMtMember[byLoop]);
			}
		}
		else
		{
			for (byLoop = 0; byLoop < ptVMPParam->GetMaxMemberNum(); byLoop++)
			{
				cKDVVMPParam.m_atMtMember[byLoop].SetMember(*ptVMPParam->GetVmpMember(byLoop)); 

				TMt tTempMt = *(TMt *)ptVMPParam->GetVmpMember(byLoop);    
				if( tTempMt.GetMtId() != 0 )
				{
					byMember ++;
				}
				if (tChairMt == tTempMt && ptVMPParam->GetVmpMember(byLoop)->GetMemberType() != VMP_MEMBERTYPE_SPEAKER )
				{
					cKDVVMPParam.m_atMtMember[byLoop].SetMemStatus(MT_STATUS_CHAIRMAN);
				}
				else if (tSpeakerMt == tTempMt)
				{
					cKDVVMPParam.m_atMtMember[byLoop].SetMemStatus(MT_STATUS_SPEAKER);
				}
				else
				{
					cKDVVMPParam.m_atMtMember[byLoop].SetMemStatus(MT_STATUS_AUDIENCE);
				}
				// 终端别名
				cKDVVMPParam.m_atMtMember[byLoop].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(tTempMt.GetMtId()) );
			}
    
			//清掉不与会终端
			if( !m_tConfAllMtInfo.MtJoinedConf( cKDVVMPParam.m_atMtMember[byLoop].GetMtId()))
			{
				cKDVVMPParam.m_atMtMember[byLoop].SetNull();
			}
		}
	}

	//清掉其余通道的交换
	u8 byMaxVMPMember = MAXNUM_MPUSVMP_MEMBER;
	if (ptVMPParam->IsVMPAuto() || VCS_CONF == m_tConf.GetConfSource())
	{
		byLoop = byMember;	
	}
	else
	{
		byLoop = ptVMPParam->GetMaxMemberNum();
	}
	
	for (; byLoop < byMaxVMPMember; byLoop++)
	{
		StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
		ptVMPParam->ClearVmpMember(byLoop);
		//tConfVMPParam.ClearVmpMember(byLoop);// meaning?
	}

	//将合成成员更新到TPeriEqpStatus中
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = *ptVMPParam;
	g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );

    //填载荷类型等参数
    TLogicalChannel tChnnl;
    BOOL32 bRet = FALSE;
    for (byLoop = 0; byLoop < /*ptVMPParam->GetMaxMemberNum()*/MAXNUM_SDVMP_MEMBER; byLoop++)
    {
	     //zbq[03/03/2009] 跨级终端的载荷取对应的mcu
        TMt tTmpMt = (TMt)cKDVVMPParam.m_atMtMember[byLoop];
		if (tTmpMt.IsNull())
		{
			continue;
		}

        if (!tTmpMt.IsLocal())
        {
            tTmpMt = GetLocalMtFromOtherMcuMt(tTmpMt);
        }	

        bRet = m_ptMtTable->GetMtLogicChnnl(tTmpMt.GetMtId(), LOGCHL_VIDEO, &tChnnl, FALSE);
        if (bRet)
        {
            cKDVVMPParam.m_tVideoEncrypt[byLoop] = tChnnl.GetMediaEncrypt();
            cKDVVMPParam.m_tDoublePayload[byLoop].SetRealPayLoad(tChnnl.GetChannelType());
            cKDVVMPParam.m_tDoublePayload[byLoop].SetActivePayload(tChnnl.GetActivePayload());
        }
    }
	
	u8 byNeedPrs = m_tConf.GetConfAttrb().IsResendLosePack();
    
    if(g_bPrintEqpMsg)
    {
        OspPrintf(TRUE, FALSE, "[ChangeVmpParam] vmp param is followed:\n");
        cKDVVMPParam.Print();
    }    

	CServMsg cServMsg;
	cServMsg.SetConfId(m_tConf.GetConfId());
	cServMsg.SetMsgBody((u8*)&cKDVVMPParam, sizeof(cKDVVMPParam));
	
    //双速单格式会议时第二通道填主媒体格式
    if (0 != m_tConf.GetSecBitRate() && 
        MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
    {
        cKDVVMPParam.m_byEncType = m_tConf.GetMainVideoMediaType();
    }
    else
    {
        cKDVVMPParam.m_byEncType = m_tConf.GetSecVideoMediaType();
        
        //xsl [8/11/2006] mpeg2 4cif按2cif处理
        m_tConf.GetVideoScale(cKDVVMPParam.m_byEncType, wWidth, wHeight);
        cKDVVMPParam.m_wVideoWidth = wWidth;
        cKDVVMPParam.m_wVideoHeight = wHeight;
        byRes = m_tConf.GetVideoFormat(cKDVVMPParam.m_byEncType);
        if( VIDEO_FORMAT_16CIF == byRes ||
            (MEDIA_TYPE_H262 == cKDVVMPParam.m_byEncType && VIDEO_FORMAT_4CIF == byRes))
        {
            cKDVVMPParam.m_wVideoWidth  = 352;
            cKDVVMPParam.m_wVideoHeight = 576;
        }        
        cKDVVMPParam.m_wVideoWidth  = htons(cKDVVMPParam.m_wVideoWidth);
        cKDVVMPParam.m_wVideoHeight = htons(cKDVVMPParam.m_wVideoHeight);
    }
    
    //modify bas 2
    // 单速单格式会议，码率清0, zgc, 2008-03-19
    if ( MEDIA_TYPE_NULL == cKDVVMPParam.m_byEncType )
    {
        wMinMtReqBitrate = 0;
    }
    else
    {
        wMinMtReqBitrate = GetLeastMtReqBitrate(TRUE, cKDVVMPParam.m_byEncType);
        if (wMinMtReqBitrate < m_wBasBitrate || 0 == m_wBasBitrate)
        {
            m_wBasBitrate = wMinMtReqBitrate;
        }
    }
    cKDVVMPParam.m_wBitRate = htons(wMinMtReqBitrate);

	cServMsg.CatMsgBody((u8 *)&cKDVVMPParam, sizeof(cKDVVMPParam));
	cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs)); 

    //画面合成风格
    u8 bySchemeId = ptVMPParam->GetVMPSchemeId();
    TVmpStyleCfgInfo tMcuVmpStyle;    
    if(0 == bySchemeId)
    {
        EqpLog("[ChangeVmpParam] SchemeId :%d, use default\n", bySchemeId);
        tMcuVmpStyle.ResetDefaultColor();		
    }  
    else
    {		
        u8  byVmpStyleNum = 0;
        TVmpAttachCfg atVmpStyle[MAX_VMPSTYLE_NUM];
        if (SUCCESS_AGENT == g_cMcuAgent.ReadVmpAttachTable(&byVmpStyleNum, atVmpStyle) && 
            bySchemeId <= MAX_VMPSTYLE_NUM) // 顾振华 [4/30/2006] 这里应该检查是否越界即可
        {
            //zbq[04/02/2008] 取对应方案号的方案，而不是直接取索引
            u8 byStyleNo = 0;
            for( ; byStyleNo < byVmpStyleNum && byStyleNo < MAX_VMPSTYLE_NUM; byStyleNo ++ )
            {
                if ( bySchemeId == atVmpStyle[byStyleNo].GetIndex() )
                {
                    break;
                }
            }
            if ( byVmpStyleNum == byStyleNo ||
                 MAX_VMPSTYLE_NUM == byStyleNo )
            {
                tMcuVmpStyle.ResetDefaultColor();
                ConfLog( FALSE, "[ChangeVmpParam] Get vmp cfg failed! use default(NoIdx)\n");                
            }
            else
            {
                tMcuVmpStyle.SetBackgroundColor(atVmpStyle[byStyleNo].GetBGDColor());
                tMcuVmpStyle.SetFrameColor(atVmpStyle[byStyleNo].GetFrameColor());
                tMcuVmpStyle.SetChairFrameColor(atVmpStyle[byStyleNo].GetChairFrameColor());
                tMcuVmpStyle.SetSpeakerFrameColor(atVmpStyle[byStyleNo].GetSpeakerFrameColor());
                tMcuVmpStyle.SetSchemeId(atVmpStyle[byStyleNo].GetIndex());
                tMcuVmpStyle.SetFontType( atVmpStyle[byStyleNo].GetFontType() );
                tMcuVmpStyle.SetFontSize( atVmpStyle[byStyleNo].GetFontSize() );
                tMcuVmpStyle.SetTextColor( atVmpStyle[byStyleNo].GetTextColor() );
                tMcuVmpStyle.SetTopicBkColor( atVmpStyle[byStyleNo].GetTopicBGDColor() );
                tMcuVmpStyle.SetDiaphaneity( atVmpStyle[byStyleNo].GetDiaphaneity() );
                
                EqpLog("[ChangeVmpParam] get vmpstyle info success, SchemeId :%d\n", bySchemeId);
                EqpLog("[ChangeVmpParam] GRDColor.0x%x, AudFrmColor.0x%x, ChairFrmColor.0x%x, SpeakerFrmColor.0x%x\n",
                        tMcuVmpStyle.GetBackgroundColor(),
                        tMcuVmpStyle.GetFrameColor(),
                        tMcuVmpStyle.GetChairFrameColor(),
                        tMcuVmpStyle.GetSpeakerFrameColor() );
            }
        }
        else
        {
            tMcuVmpStyle.ResetDefaultColor();
            OspPrintf(TRUE, FALSE, "[ChangeVmpParam] Get vmp cfg failed! use default\n");
        }
    }
    tMcuVmpStyle.SetIsRimEnabled( ptVMPParam->GetIsRimEnabled() );

    tConfVMPParam.SetVMPSchemeId(bySchemeId);
	// 保存是否有边框的标志, zgc, 2007-05-16
	tConfVMPParam.SetIsRimEnabled( ptVMPParam->GetIsRimEnabled() );

    m_tConf.m_tStatus.SetVmpParam(tConfVMPParam);

    cServMsg.CatMsgBody((u8*)&tMcuVmpStyle, sizeof(tMcuVmpStyle));

	//发消息
	if (bStart)
	{
		// MCU前向纠错修改, zgc, 2007-09-27
		TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
		cServMsg.CatMsgBody((u8*)&tCapSupportEx, sizeof(tCapSupportEx));

		SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, 6*1000);
		SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_STARTVIDMIX_REQ, cServMsg);
	}
	else
	{
		SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_CHANGEVIDMIXPARAM_REQ, cServMsg);
	}

    m_tConfInStatus.SetVmpNotify(FALSE);
}

/*====================================================================
    函数名      ：ProcVmpMcuMsg
    功能        ：画面合成器消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/04    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcVmpMcuRsp( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:

        switch( pcMsg->event ) 
		{		
        case VMP_MCU_STARTVIDMIX_ACK:       //VMP给MCU开始工作确认
        case VMP_MCU_STARTVIDMIX_NACK:      //VMP给MCU开始工作拒绝
		case VMP_MCU_STOPVIDMIX_ACK:        //VMP给MCU停止工作确认
		case VMP_MCU_STOPVIDMIX_NACK:       //VMP给MCU停止工作拒绝
		case VMP_MCU_CHANGEVIDMIXPARAM_ACK: //VMP给MCU改变复合参数确认
		case VMP_MCU_CHANGEVIDMIXPARAM_NACK://VMP给MCU改变复合参数拒绝			
		case VMP_MCU_GETVIDMIXPARAM_ACK:    //VMP给MCU回送复合参数确认			
		case VMP_MCU_GETVIDMIXPARAM_NACK:   //VMP给MCU回送复合参数拒绝
			break;

        default:
			break;
        }

		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}	
}

/*====================================================================
    函数名      ：ProcVmpRspWaitTimer
    功能        ：MCU等待VMP应答超时处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/08    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcVmpRspWaitTimer( const CMessage * pcMsg )
{
	KillTimer(MCUVC_VMP_WAITVMPRSP_TIMER);

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TPeriEqpStatus tPeriEqpStatus;

	switch( CurState() )
	{
	case STATE_ONGOING:

		if (!m_tVmpEqp.IsNull() &&
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
			tPeriEqpStatus.GetConfIdx() == m_byConfIdx)
		{
			if (TVmpStatus::WAIT_START== tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
			{
				tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
				g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
				// xliang [5/4/2009] 上报会控
				cServMsg.SetMsgBody();
				cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );

				cServMsg.SetMsgBody();
				SendMsgToEqp( m_tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cServMsg );

				m_tVmpEqp.SetNull();
			}
			else if (TVmpStatus::WAIT_STOP== tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
			{
				ProcVMPStopSucRsp();
			}
			else
			{
				EqpLog("[ProcVmpRspWaitTimer] no steps needed in state(%d) for vmp(%d) in conf(%d)\n",
					   tPeriEqpStatus.m_tStatus.tVmp.m_byUseState, m_tVmpEqp.GetEqpId(), m_byConfIdx);

			}
		}
		break;

	default:
		ConfLog( FALSE, "[VmpRspWaitTimer]Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcVmpMcuNotif
    功能        ：处理Vmp给Mcu的通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/10/31    4.0         libo          创建
====================================================================*/
void CMcuVcInst::ProcVmpMcuNotif(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	TVMPParam tVMPParam;
	memset( &tVMPParam, 0, sizeof(TVMPParam) );

    TEqp tEqp;
	TPeriEqpStatus tPeriEqpStatus;
	TMt tSrcMt;
	u8 byChlIdx;
    TPeriEqpRegReq tRegReq;

	switch( CurState() )
	{
	case STATE_ONGOING:

        switch( pcMsg->event ) 
		{
		case VMP_MCU_STARTVIDMIX_NOTIF:     //VMP给MCU开始工作通知
			{
			//清除定时
			KillTimer( MCUVC_VMP_WAITVMPRSP_TIMER );
			if (!m_tVmpEqp.IsNull() &&
				g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
				tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
				TVmpStatus::WAIT_START == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
			{
				tVMPParam = tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam;
				m_tConf.m_tStatus.SetVmpParam( tVMPParam );
				m_tConf.m_tStatus.SetVMPMode( tVMPParam.IsVMPAuto() ? CONF_VMPMODE_AUTO : CONF_VMPMODE_CTRL );

				//开始交换数据 
				ChangeVmpSwitch();

				// xliang [12/22/2008] 批量轮询定时
				if (m_tConf.m_tStatus.m_tVMPParam.IsVMPBatchPoll() 
					//! 
					)
				{
					u32 dwTimerT = m_tVmpBatchPollInfo.GetFirstPollT();
					
					if(dwTimerT < VMP_BATCHPOLL_TMIN)
					{
						dwTimerT = VMP_BATCHPOLL_TMIN;
					}
					else if(dwTimerT > VMP_BATCHPOLL_TMAX)
					{
						dwTimerT = VMP_BATCHPOLL_TMAX;
					}
					
					SetTimer(MCUVC_VMPBATCHPOLL_TIMER,dwTimerT);
					
					EqpLog("SetTimer MCUVC_VMPBATCHPOLL_TIMER for T:%d s\n",dwTimerT/1000);
					
				}
				//同步外设复合状态
				tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
				tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::START;
				g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );

				
				//通知主席及所有会控
				cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam) );
				SendMsgToAllMcs( MCU_MCS_STARTVMP_NOTIF, cServMsg );

				if( HasJoinedChairman() )
				{
					SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STARTVMP_NOTIF, cServMsg );
				}

				m_tConfInStatus.SetVmpNotify(TRUE);       
    
				//FastUpdate to Vmp
				NotifyFastUpdate(m_tVmpEqp, 0);
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				u8 byMpuBoardVer = tPeriEqpStatus.m_tStatus.tVmp.m_byBoardVer;
				if (MPU_SVMP == byVmpSubType && MPU_BOARD_B256 == byMpuBoardVer)
				{
					NotifyFastUpdate(m_tVmpEqp, 1);
					NotifyFastUpdate(m_tVmpEqp, 2);
					NotifyFastUpdate(m_tVmpEqp, 3);
				}

				//n+1备份更新vmp信息
				if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
				{
					ProcNPlusVmpUpdate();
				}
			}
			else
			{
				OspPrintf(TRUE, FALSE, "[ProcVmpMcuNotif]Recv VMP_MCU_STARTVIDMIX_NOTIF in wrong state\n");
			}
            break;
			}

		case VMP_MCU_STOPVIDMIX_NOTIF:      //VMP给MCU停止工作通知
			KillTimer(MCUVC_VMP_WAITVMPRSP_TIMER);
			if (!m_tVmpEqp.IsNull() &&
				g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
				tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
				TVmpStatus::WAIT_STOP == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
			{
				ProcVMPStopSucRsp();
			}
			
			break;
			
		case VMP_MCU_CHANGESTATUS_NOTIF:    //VMP给MCU改变复合参数通知 
			{
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
				tVMPParam = tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam;
				m_tConf.m_tStatus.SetVmpParam( tVMPParam );
				m_tConf.m_tStatus.SetVMPMode( tVMPParam.IsVMPAuto() ? CONF_VMPMODE_AUTO : CONF_VMPMODE_CTRL );
				
				
				//改变数据交换
				ChangeVmpSwitch( 0, VMP_STATE_CHANGE );            
				
				// xliang [12/23/2008] 批量轮询定时t.
				if(m_tConf.m_tStatus.m_tVMPParam.IsVMPBatchPoll())
				{
					u32 dwTimert = 0;
					if(m_tVmpBatchPollInfo.GetTmpt() != 0)//非与会终端基本定时为0
					{
						dwTimert = m_tVmpBatchPollInfo.GetTmpt();
					}
					else
					{
						dwTimert = m_tVmpBatchPollInfo.GetPollt();
					}
					SetTimer(MCUVC_VMPBATCHPOLL_TIMER,dwTimert);
				}
				//同步外设复合状态
				tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
				g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );     
				
				//通知mcs
				cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(m_tConf.m_tStatus.m_tVMPParam) );
				SendMsgToAllMcs(MCU_MCS_CHANGEVMPPARAM_NOTIF, cServMsg );
				
				m_tConfInStatus.SetVmpNotify(TRUE);
				
				//FastUpdate to Vmp
				NotifyFastUpdate(m_tVmpEqp, 0);
				u8 byVmpSubType	= tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				u8 byMpuBoardVer = tPeriEqpStatus.m_tStatus.tVmp.m_byBoardVer;
				if (MPU_SVMP == byVmpSubType && MPU_BOARD_B256 == byMpuBoardVer)
				{
					NotifyFastUpdate(m_tVmpEqp, 1);
					NotifyFastUpdate(m_tVmpEqp, 2);
					NotifyFastUpdate(m_tVmpEqp, 3);
				}
				
				// 对于VCS会议在组呼画面合成的模式下,混音成员与画面合成成员绑定
				if (VCS_CONF == m_tConf.GetConfSource() && 
					VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode() &&
					m_tConf.m_tStatus.IsMixing())
				{
					TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
					u8 byMemNum = tVmpParam.GetMaxMemberNum();
					TMt tMt;
					TMt atMixMember[MAXNUM_CONF_MT];
					u8  byMixMemNum = 0;
					for (u8 byVMPMemIdx = 0; byVMPMemIdx < byMemNum; byVMPMemIdx++)
					{
						tMt = *(TMt*)(tVmpParam.GetVmpMember(byVMPMemIdx));
						if (!tMt.IsNull())
						{
							atMixMember[byMixMemNum++] = tMt;				
						}
						
					}
					ChangeSpecMixMember(atMixMember, byMixMemNum);
					
				}
				
				//n+1备份更新vmp信息
				if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
				{
					ProcNPlusVmpUpdate();
				}
				break;
			}

        case MCU_VMPCONNECTED_NOTIF:
            tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();

            tEqp.SetMcuEqp(tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());

            if (m_tVmpEqp.GetMtId() != tEqp.GetMtId() ||
                m_tVmpEqp.GetMcuId() != tEqp.GetMcuId() ||
                m_tVmpEqp.GetType() != tEqp.GetType())
            {
				// xliang [4/17/2009] 在异常掉线的VMP板恢复前，用户用了其他VMP板，之后
				// 先前掉线的VMP板上线将作为空闲VMP板备用。此时要将其状态置为"未使用"，否则泄漏
				/*
				if ( !m_tLastVmpEqp.IsNull() )
				{
					g_cMcuVcApp.GetPeriEqpStatus(m_tLastVmpEqp.GetEqpId(), &tPeriEqpStatus);
					tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = FALSE;
					g_cMcuVcApp.SetPeriEqpStatus( m_tLastVmpEqp.GetEqpId() , &tPeriEqpStatus );
				}*/
                return;
            }

            //设置状态			
            g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tPeriEqpStatus);  
            tVMPParam = tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam;

            if ( g_bPrintEqpMsg )
            {
                OspPrintf(TRUE, FALSE, "[Vmp Connected] tVMPParam is follows:\n");
                tVMPParam.Print();
            }
			
            cServMsg.SetEventId(MCS_MCU_STARTVMP_REQ);
            cServMsg.SetMsgBody((u8 *)&tVMPParam, sizeof(TVMPParam));
            VmpCommonReq(cServMsg);
            break;

		case MCU_VMPDISCONNECTED_NOTIF:
			{
	           tEqp = *(TEqp *)cServMsg.GetMsgBody();

				if (m_tVmpEqp.GetMtId() != tEqp.GetMtId() ||
					m_tVmpEqp.GetMcuId() != tEqp.GetMcuId() ||
					m_tVmpEqp.GetType() != tEqp.GetType())
				{
					return;
				}

				// xliang [4/17/2009] 记录异常掉线的VMP
				// m_tLastVmpEqp = m_tVmpEqp;

				//停止画面复合后，VMP模板不再生效
				m_tConfInStatus.SetVmpModuleUsed(TRUE);
				m_tConfInStatus.SetVmpNotify(FALSE);

				//zbq[12/05/2007] 停对应模式下的点名
				if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
				{
					CServMsg cMsg;
					RollCallStop(cMsg);
				}

				m_tConf.m_tStatus.SetVMPMode( CONF_VMPMODE_NONE );	// xliang [4/17/2009] 移至ChangeVmpSwitch之前
				
				//停止交换数据
				ChangeVmpSwitch( 0, VMP_STATE_STOP );

				//主席选看VMP停掉
	// 			if(m_tConf.m_tStatus.GetVmpParam().IsVMPSeeByChairman())
	// 			{
	// 				OspPrintf(1,0,"cancel vmp to mt!\n");
	// 				StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO);
	// 			}

				//设置外设状态：tPeriEqpStatus保留大部分数据，以备恢复用		
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
				tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
				g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
				
				//外设状态上报会控
				cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );
				m_tConf.m_tStatus.SetVmpParam( tVMPParam );
				m_tConf.m_tStatus.SetVMPMode( CONF_VMPMODE_NONE );

				//通知主席及所有会控 
				cServMsg.SetMsgBody();
				cServMsg.SetMsgBody( (u8*)&tVMPParam, sizeof(TVMPParam) );
				SendMsgToAllMcs( MCU_MCS_STOPVMP_NOTIF, cServMsg );
				if( HasJoinedChairman() )
				{
					SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVMP_NOTIF, cServMsg );
				}
				
				SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
            
				if ( g_bPrintEqpMsg )
				{
					OspPrintf(TRUE, FALSE, "[Vmp Disconnected] m_tVMPParam is follows:\n");
					m_tConf.m_tStatus.m_tVMPParam.Print();
				}

				//对于VCS会议，若在画面合成状态则恢复到单方调度
				// 若在组呼画面合成状态则恢复到发言人模式
				if (VCS_CONF == m_tConf.GetConfSource() &&
					(VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() ||
					 VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode()))
				{
					if (VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode())
					{
						RestoreVCConf(VCS_SINGLE_MODE);
					}
					else if (VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode())
					{
						RestoreVCConf(VCS_GROUPSPEAK_MODE);
					}

					// fxh 释放画面合成器,该合成器重启后不自动启动画面合成
					m_tVmpEqp.SetNull();
				}
				
				//n+1备份更新vmp信息
				if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
				{
					ProcNPlusVmpUpdate();
				}

				//停止选看该vmp的hdu
				TPeriEqpStatus tHduStatus;
				u8 byHduId = HDUID_MIN;
				while( byHduId >= HDUID_MIN && byHduId <= HDUID_MAX )
				{
					if (g_cMcuVcApp.IsPeriEqpValid(byHduId))
					{
						g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
						if (tHduStatus.m_byOnline == 1)
						{
							u8 byTmpMtId = 0;
							u8 byMtConfIdx = 0;
							u8 byMemberNum = tHduStatus.m_tStatus.tHdu.byChnnlNum;
							for(u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
							{
								byTmpMtId = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetMtId();
								byMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
								if (m_tVmpEqp.GetEqpId() == byTmpMtId && m_byConfIdx == byMtConfIdx)
								{
									StopSwitchToPeriEqp(byHduId, byLoop);
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = 0;
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
                    			}
               				}
							g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);
					
							cServMsg.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
							SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
						}
					}
					byHduId++;
				}	
				
				ConfStatusChange();	// xliang [5/4/2009] 会议状态上报

			}


				
			break;
			
		case VMP_MCU_NEEDIFRAME_CMD:

            byChlIdx = cServMsg.GetChnIndex();
            tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
            tSrcMt = *(TMt*)tVMPParam.GetVmpMember( byChlIdx );
            //必须是终端
            NotifyMtFastUpdate(tSrcMt.GetMtId(), MODE_VIDEO);
            break;
				
        default:
			break;
        }

		break;

	default:
		ConfLog( FALSE, "[VmpMcuNotif]Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

#ifdef _SATELITE_
    RefreshConfState();

#endif

}

/*------------------------------------------------------------------*/
/*                          VMP TVWall                              */
/*------------------------------------------------------------------*/

/*====================================================================
    函数名      ：SetVmpChnnl
    功能        ：设置画面合成器通道 
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/10/08    4.0         libo          创建
====================================================================*/
void CMcuVcInst::SetVmpTwChnnl(TMt tMt, u8 byChnnl, u8 byMemberType, BOOL32 bMsgMcs)
{
    CServMsg cServMsg;

    TMt tLocalMt = GetLocalMtFromOtherMcuMt(tMt);

    //开始交换码流
    StartSwitchToPeriEqp(tLocalMt, 0, m_tVmpTwEqp.GetEqpId(),
                         byChnnl, MODE_VIDEO, SWITCH_MODE_SELECT);

    if (!tMt.IsLocal())
    {
        OnMMcuSetIn(tMt, 0, SWITCH_MODE_SELECT);
    }

    //改变状态
    TVMPMember tVMPMember;
    tVMPMember.SetMemberTMt(tMt);
    tVMPMember.SetMemberType(byMemberType);
    m_tConf.m_tStatus.m_tVmpTwParam.SetVmpMember(byChnnl, tVMPMember);

    //通知会控
    if (bMsgMcs)
    {
        cServMsg.SetMsgBody((u8*)&m_tConf.m_tStatus.m_tVmpTwParam, sizeof(TVMPParam));
        SendMsgToAllMcs(MCU_MCS_VMPTWPARAM_NOTIF, cServMsg);

        SendConfInfoToChairMt();	
    }

    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
        //把目的的Rtcp交换给源
        TLogicalChannel tLogicalChannel;
        m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);

        u32 dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
        u16 wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();

        //记录交换目的
        SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tVmpTwEqp, byChnnl, SWITCH_MODE_SELECT);
    }
}

/*====================================================================
    函数名      ：ChangeVmpTwSwitch
    功能        ：改变复合电视墙交换参数
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt * ptNewSrc, 源,NULL为所有通道
	              u8 bySrcType 成员类型
                  u8 byState 是否是停止交换
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/10/08    4.0         libo          创建
====================================================================*/
void CMcuVcInst::ChangeVmpTwSwitch(u8 bySrcType, u8 byState)
{
    u8 byLoop;
    TVMPMember tVMPMember;
    TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpTwParam();
    u8 byMemberNum = tVMPParam.GetMaxMemberNum();

    //所有通道停止向VMP交换
    if (VMPTW_STATE_STOP == byState)
    {
        for (byLoop = 0; byLoop < byMemberNum; byLoop++)
        {
            StopSwitchToPeriEqp(m_tVmpTwEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);

			tVMPMember = *(tVMPParam.GetVmpMember(byLoop));

            //Stop Resend Lose Pack
            if (m_tConf.GetConfAttrb().IsResendLosePack())
            {                
                //把目的的Rtcp交换给源
                TLogicalChannel tLogicalChannel;
                m_ptMtTable->GetMtLogicChnnl(tVMPMember.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);

                u32 dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
                u16 wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();

                g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIp, wDstPort);
            }

			ChangeMtVideoFormat( tVMPMember, &m_tLastVmpTwParam, FALSE );
        }
        return;
    }

    //会控控制模式 
    TMt tMt;
    memset(&tMt, 0, sizeof(TMt));
    if (CONF_VMPTWMODE_CTRL == m_tConf.m_tStatus.GetVmpTwMode())
    {
        for(byLoop = 0; byLoop < byMemberNum; byLoop++)
        {
            tVMPMember = *tVMPParam.GetVmpMember(byLoop);
            // 顾振华 [5/23/2006] 参考Vmp部分,增加 MemberType的判断
            if (0 == tVMPMember.GetMemberType() && tVMPMember.IsNull())
            {
                StopSwitchToPeriEqp(m_tVmpTwEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
            }
            else
            {
                switch(tVMPMember.GetMemberType()) 
                {
                case VMPTW_MEMBERTYPE_MCSSPEC:
                    if (m_tConfAllMtInfo.MtJoinedConf(tVMPMember.GetMtId()))
                    {
                        SetVmpTwChnnl((TMt)tVMPMember, byLoop, VMPTW_MEMBERTYPE_MCSSPEC);

						ChangeMtVideoFormat( tVMPMember, &tVMPParam );
                    }
                    break;

                case VMPTW_MEMBERTYPE_SPEAKER://注意改变发言人时改变交换
                    // 顾振华 [5/23/2006] 只允许终端进入通道，不允许放像设备
					if( HasJoinedSpeaker()  && GetLocalSpeaker().GetType() != TYPE_MCUPERI )
					{
						SetVmpTwChnnl( GetLocalSpeaker(), byLoop, VMPTW_MEMBERTYPE_SPEAKER );

						ChangeMtVideoFormat( tVMPMember, &tVMPParam );
					}
					else
					{
						StopSwitchToPeriEqp(m_tVmpTwEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
						
						tMt.SetNull();
						tVMPMember.SetMemberTMt(tMt);
						tVMPMember.SetMemberType(VMPTW_MEMBERTYPE_SPEAKER);
						m_tConf.m_tStatus.m_tVmpTwParam.SetVmpMember(byLoop, tVMPMember);
					}
					break;

				case VMPTW_MEMBERTYPE_CHAIRMAN://注意改变主席时改变交换
					if( HasJoinedChairman() )
					{
						SetVmpTwChnnl( m_tConf.GetChairman(), byLoop, VMPTW_MEMBERTYPE_CHAIRMAN );

						ChangeMtVideoFormat( tVMPMember, &tVMPParam );
					}
					else
					{
						StopSwitchToPeriEqp( m_tVmpTwEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO );
						tMt.SetNull();
						tVMPMember.SetMemberTMt( tMt );
						tVMPMember.SetMemberType( VMPTW_MEMBERTYPE_CHAIRMAN );
						m_tConf.m_tStatus.m_tVmpTwParam.SetVmpMember( byLoop, tVMPMember);
					}
					break;

				case VMPTW_MEMBERTYPE_POLL://注意改变轮询广播源时改变交换
					if( m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO ||
						m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_SPEAKER)
					{	
						tMt = (TMt)m_tConf.m_tStatus.GetMtPollParam();
						
						SetVmpTwChnnl( tMt, byLoop, VMPTW_MEMBERTYPE_POLL );

						ChangeMtVideoFormat( tVMPMember, &tVMPParam );
					}
					else
					{
						StopSwitchToPeriEqp( m_tVmpTwEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO );

						tMt.SetNull();
						tVMPMember.SetMemberTMt( tMt );
						tVMPMember.SetMemberType( VMPTW_MEMBERTYPE_POLL );
						m_tConf.m_tStatus.m_tVmpTwParam.SetVmpMember( byLoop, tVMPMember);
					}
					break;

                default:
                    break;
                }
            }
        }
    }
	
	//更改合成分辨率参数
	if (VMP_STATE_CHANGE == byState)
    {
        if ( CONF_VMPMODE_NONE != m_tLastVmpTwParam.GetVMPMode() )
        {
            for( s32 nIndex = 0; nIndex < m_tLastVmpTwParam.GetMaxMemberNum(); nIndex ++ )
			{
				TMt tVMPMemberOld = *(TMt *)m_tLastVmpTwParam.GetVmpMember(nIndex);
            
				//对删除了的合成成员恢复其发送分辨率
				if ( !m_tConf.m_tStatus.m_tVmpTwParam.IsMtInMember(tVMPMemberOld) &&
					 m_tConfAllMtInfo.MtJoinedConf(tVMPMemberOld.GetMtId()) ) 
				{
					ChangeMtVideoFormat( tVMPMemberOld, &m_tLastVmpTwParam, FALSE );
				}
			}                          
        }
    }

	//保存本次画面合成参数，用于动态分辨率恢复
	memcpy(&m_tLastVmpTwParam, &m_tConf.m_tStatus.m_tVmpTwParam, sizeof(TVMPParam));
	
    if(g_bPrintEqpMsg)
    {
        m_tConf.m_tStatus.m_tVmpTwParam.Print();
    }

	CServMsg cServMsg;
	cServMsg.SetMsgBody((u8 *)&m_tConf.m_tStatus.m_tVmpTwParam, sizeof(TVMPParam));
	SendMsgToAllMcs(MCU_MCS_VMPTWPARAM_NOTIF, cServMsg);

    SendConfInfoToChairMt();

	return;
}


/*====================================================================
    函数名      ：ChangeVmpTwChannelParam
    功能        ：改变会议画面合成方式
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/10/27    4.0         libo          创建
====================================================================*/
void CMcuVcInst::ChangeVmpTwChannelParam(TMt * ptNewSrc, u8 byMemberType)
{    
    //TVMPMember tTmpVmpMember;
    TVMPMember tVmpMember;
    TVMPParam  tVmpTwParam;    
    TMt        tMt;
    u8         byMaxMemberNum;
    //u8         byNewMemberChnl;
    //u8         byMemberChnl;

    BOOL32 bVmpTwParamChged = FALSE;

    memset(&tMt, 0, sizeof(TMt));
    tVmpTwParam = m_tConf.m_tStatus.GetVmpTwParam();
    byMaxMemberNum = tVmpTwParam.GetMaxMemberNum();

    for (u8 byLoop = 0; byLoop < byMaxMemberNum; byLoop++)
    {
        if (tVmpTwParam.GetVmpMember(byLoop)->GetMemberType() == byMemberType)
        {
            StopSwitchToPeriEqp(m_tVmpTwEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
            tVmpTwParam.ClearVmpMember(byLoop);

            // 顾振华 [5/23/2006] 只允许终端进入通道，不允许放像设备
            if (ptNewSrc == NULL || ptNewSrc->GetType() == TYPE_MCUPERI)
            {
                tMt.SetNull();
            }
            else
            {
                tMt = *ptNewSrc;
            }

            tVmpMember.SetMemberTMt(tMt);
            tVmpMember.SetMemberType(byMemberType);
            tVmpTwParam.SetVmpMember(byLoop, tVmpMember);

            bVmpTwParamChged = TRUE;
        }
    }

    // zbq [06/05/2007] 通道参数发生了实际更改才修改通道参数
    if ( bVmpTwParamChged )
    {
        m_tConf.m_tStatus.SetVmpTwParam(tVmpTwParam);
        
        TPeriEqpStatus tPeriEqpStatus;
        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId() , &tPeriEqpStatus);  
        tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpTwParam();
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId() , &tPeriEqpStatus);  
        
        ChangeVmpTwParam(&tVmpTwParam);
    }

    return;
}

/*====================================================================
    函数名      ：ChangeVmpTwParam
    功能        ：复合电视墙参数改变
    算法实现    ：
    引用全局变量：
    输入参数说明：TVMPParam *ptVMPParam, 复合电视墙合成参数
				  BOOL32 bStart 是否是开始画面合成
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/10/08    4.0         libo          创建
====================================================================*/
void CMcuVcInst::ChangeVmpTwParam(TVMPParam *ptVmpParam, BOOL32 bStart)
{
	if (ptVmpParam == NULL)
	{
		return;
	}

	TVMPParam tConfVmpParam;

    //设置参数
    CKDVVMPParam cVmpParam;
    memset(&cVmpParam, 0, sizeof(CKDVVMPParam));
    cVmpParam.m_byVMPStyle = ptVmpParam->GetVMPStyle();
    cVmpParam.m_byEncType = MEDIA_TYPE_H261;

    cVmpParam.m_wBitRate = htons(768);
    //CIF
    cVmpParam.m_wVideoWidth  = htons(352);
	cVmpParam.m_wVideoHeight = htons(288);
	cVmpParam.m_byMemberNum  = ptVmpParam->GetMaxMemberNum();

	for (u8 byLoop = 0; byLoop < ptVmpParam->GetMaxMemberNum(); byLoop++)
	{
		cVmpParam.m_atMtMember[byLoop].SetMember(*ptVmpParam->GetVmpMember(byLoop)) ;
		//清掉不与会终端
		if (!m_tConfAllMtInfo.MtJoinedConf(cVmpParam.m_atMtMember[byLoop].GetMtId()))
		{
			cVmpParam.m_atMtMember[byLoop].SetNull();
		}
	}

	//清掉其余通道的交换
	tConfVmpParam = m_tConf.m_tStatus.GetVmpTwParam();
	for (u8 byLoop1 = ptVmpParam->GetMaxMemberNum(); byLoop1 < MAXNUM_SDVMP_MEMBER; byLoop1++)
	{
		StopSwitchToPeriEqp(m_tVmpTwEqp.GetEqpId(), byLoop1, TRUE, MODE_VIDEO);
		tConfVmpParam.ClearVmpMember(byLoop1);
	}
	m_tConf.m_tStatus.SetVmpTwParam(tConfVmpParam);

	u8 byNeedPrs = m_tConf.GetConfAttrb().IsResendLosePack();

    u8 byMtId = 1;
    for(u8 byIdx = 0; byIdx < ptVmpParam->GetMaxMemberNum(); byIdx++)
	{
        BOOL32 bRet = FALSE;
        TLogicalChannel tChnnl;

		TVMPMember tVMPMember = *ptVmpParam->GetVmpMember(byIdx);
		if (tVMPMember.GetMtId() != 0)
		{
            bRet = m_ptMtTable->GetMtLogicChnnl(tVMPMember.GetMtId(), LOGCHL_VIDEO, &tChnnl, FALSE);
            if (TRUE == bRet)
            {
                cVmpParam.m_tVideoEncrypt[byIdx] = tChnnl.GetMediaEncrypt();
                cVmpParam.m_tDoublePayload[byIdx].SetRealPayLoad(tChnnl.GetChannelType());
	            cVmpParam.m_tDoublePayload[byIdx].SetActivePayload(tChnnl.GetActivePayload());
            }
            byMtId++;   
		}      
	}

	CServMsg cServMsg;
	cServMsg.SetConfId(m_tConf.GetConfId());
	cServMsg.SetMsgBody((u8*)&cVmpParam, sizeof(cVmpParam));
	cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs));

	//发消息
	if (bStart)
	{
		// MCU前向纠错, zgc, 2007-09-27
		TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
		cServMsg.CatMsgBody((u8*)&tCapSupportEx, sizeof(tCapSupportEx));

		//SetTimer(MCU_VMP_WAITVMPRSP_TIMER, 6*1000);
		SendMsgToEqp(m_tVmpTwEqp.GetEqpId(), MCU_VMPTW_STARTVIDMIX_REQ, cServMsg);
	}
	else
	{
		SendMsgToEqp(m_tVmpTwEqp.GetEqpId(), MCU_VMPTW_CHANGEVIDMIXPARAM_REQ, cServMsg);
	}
}


/*====================================================================
    函数名      ：ProcVmpTwMcuRsp
    功能        ：复合电视墙消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/10/08    4.0         libo          创建
====================================================================*/
void CMcuVcInst::ProcVmpTwMcuRsp(const CMessage * pcMsg)
{
    if (STATE_ONGOING != CurState())
    {
        ConfLog(FALSE, "Wrong message %u(%s) received in state %u!\n", 
                       pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    switch( pcMsg->event ) 
    {
    case VMPTW_MCU_STARTVIDMIX_ACK:       //VMPTW给MCU开始工作确认
    case VMPTW_MCU_STARTVIDMIX_NACK:      //VMPTW给MCU开始工作拒绝
    case VMPTW_MCU_STOPVIDMIX_ACK:        //VMPTW给MCU停止工作确认
    case VMPTW_MCU_STOPVIDMIX_NACK:       //VMPTW给MCU停止工作拒绝
    case VMPTW_MCU_CHANGEVIDMIXPARAM_ACK: //VMPTW给MCU改变复合参数确认
    case VMPTW_MCU_CHANGEVIDMIXPARAM_NACK://VMPTW给MCU改变复合参数拒绝			
    case VMPTW_MCU_GETVIDMIXPARAM_ACK:    //VMPTW给MCU回送复合参数确认			
    case VMPTW_MCU_GETVIDMIXPARAM_NACK:   //VMPTW给MCU回送复合参数拒绝
        break;

    default:
        break;
    }
}


/*====================================================================
    函数名      ：ProcVmpTwMcuNotif
    功能        ：处理Vmp给Mcu的通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/10/08    4.0         libo          创建
====================================================================*/
void CMcuVcInst::ProcVmpTwMcuNotif(const CMessage * pcMsg)
{
    if (STATE_ONGOING != CurState())
    {
        ConfLog(FALSE, "Wrong message %u(%s) received in state %u!\n", 
                       pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TVMPParam tVmpTwParam;
    memset(&tVmpTwParam, 0, sizeof(TVMPParam));

    TEqp tEqp;
    TPeriEqpStatus tPeriEqpStatus;
    TMt tSrcMt;
    u8  byChlIdx;
    TPeriEqpRegReq tRegReq;

    switch(pcMsg->event) 
    {
    case VMPTW_MCU_STARTVIDMIX_NOTIF:     //VMP给MCU开始工作通知
        //清除定时
        //KillTimer(MCU_VMP_WAITVMPRSP_TIMER);

        //设置状态			
        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);  
        tVmpTwParam = tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam;
        tVmpTwParam.SetVMPStyle(m_tConf.m_tStatus.GetVmpTwStyle());
        m_tConf.m_tStatus.SetVmpTwParam(tVmpTwParam);
        m_tConf.m_tStatus.SetVmpTwMode(CONF_VMPTWMODE_CTRL);

        //开始交换数据 
        ChangeVmpTwSwitch();

        //同步外设复合状态
        tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVmpTwParam;
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);

        //通知主席及所有会控
        cServMsg.SetMsgBody((u8*)&m_tConf.m_tStatus.m_tVmpTwParam, sizeof(TVMPParam));
        SendMsgToAllMcs(MCU_MCS_STARTVMPTW_NOTIF, cServMsg);  
        
        SendConfInfoToChairMt();

        NotifyFastUpdate(m_tVmpTwEqp, MODE_VIDEO);
        break;

    case VMPTW_MCU_STOPVIDMIX_NOTIF:      //VMP给MCU停止工作通知
        //设置状态			
        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);
        tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);

        m_tConf.m_tStatus.SetVmpTwMode(CONF_VMPTWMODE_NONE);

        //停止交换数据
        ChangeVmpTwSwitch(0, VMP_STATE_STOP);

        m_tConf.m_tStatus.SetVmpTwParam(tVmpTwParam);

        //同步外设复合状态
        tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVmpTwParam;
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);

        //通知主席及所有会控
        cServMsg.SetMsgBody((u8 *)&tVmpTwParam, sizeof(TVMPParam));
        SendMsgToAllMcs(MCU_MCS_STOPVMPTW_NOTIF, cServMsg); 
        
        SendMsgToAllMcs(MCU_MCS_VMPTWPARAM_NOTIF, cServMsg);
        
        SendConfInfoToChairMt();

        // guzh [6/9/2007] 清除记录
        memcpy(&m_tLastVmpTwParam, &tVmpTwParam, sizeof(TVMPParam));
        break;

    case VMPTW_MCU_CHANGESTATUS_NOTIF:    //VMP给MCU改变复合参数通知 

        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);
        tVmpTwParam = tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam;
        m_tConf.m_tStatus.SetVmpTwParam(tVmpTwParam);
        m_tConf.m_tStatus.SetVmpTwMode(CONF_VMPTWMODE_CTRL);

        //改变数据交换
        ChangeVmpTwSwitch( 0, VMP_STATE_CHANGE );

        //同步外设复合状态
        tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVmpTwParam;
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);        
        break;

    case MCU_VMPTWCONNECTED_NOTIF:
        tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();

        tEqp.SetMcuEqp(tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());

        //g_cMcuVcApp.GetPeriEqpStatus(tRegReq.GetEqpId(), &tTvWallStatus);
        if (m_tVmpTwEqp.GetMtId() != tEqp.GetMtId() ||
            m_tVmpTwEqp.GetMcuId() != tEqp.GetMcuId() ||
            m_tVmpTwEqp.GetType() != tEqp.GetType())
        {
            return;
        }

        //设置状态			
        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);  
        tVmpTwParam = tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam;

        cServMsg.SetEventId(MCS_MCU_STARTVMPTW_REQ);
        cServMsg.SetMsgBody((u8 *)&tVmpTwParam, sizeof(TVMPParam));
        VmpTwCommonReq(cServMsg);
        break;

    case MCU_VMPTWDISCONNECTED_NOTIF: //需要修改的

        tEqp = *(TEqp *)cServMsg.GetMsgBody();

        if (m_tVmpTwEqp.GetMtId() != tEqp.GetMtId() ||
            m_tVmpTwEqp.GetMcuId() != tEqp.GetMcuId() ||
            m_tVmpTwEqp.GetType() != tEqp.GetType())
        {
            return;
        }

        //停止交换数据
        ChangeVmpTwSwitch(0, VMP_STATE_STOP);

        //设置状态			
        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);
        tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);
        //m_tConf.m_tStatus.SetVmpTwParam(tVmpTwParam);
        m_tConf.m_tStatus.SetVmpTwMode(CONF_VMPTWMODE_NONE);

        //通知主席及所有会控 
        //cServMsg.SetMsgBody((u8*)&tVmpTwParam, sizeof(TVMPParam));
        cServMsg.SetMsgBody();
        SendMsgToAllMcs(MCU_MCS_STOPVMPTW_NOTIF, cServMsg);   
        
        memset(&tVmpTwParam, 0, sizeof(TVMPParam));
        tVmpTwParam.SetVMPMode(CONF_VMPTWMODE_NONE);
        cServMsg.SetMsgBody((u8*)&tVmpTwParam, sizeof(TVMPParam));
        SendMsgToAllMcs( MCU_MCS_VMPTWPARAM_NOTIF, cServMsg );
        
        SendConfInfoToChairMt();
        break;

    case VMPTW_MCU_NEEDIFRAME_CMD:

        byChlIdx = cServMsg.GetChnIndex();
        tVmpTwParam = m_tConf.m_tStatus.m_tVmpTwParam;
        tSrcMt = *(TMt *)tVmpTwParam.GetVmpMember(byChlIdx);
        //必须是终端
        NotifyMtFastUpdate(tSrcMt.GetMtId(), MODE_VIDEO);
        break;

    default:
        break;
    }
	
}

/*------------------------------------------------------------------*/
/*                           TV Wall                                */
/*------------------------------------------------------------------*/

/*====================================================================
    函数名      ：ChangeTvWallSwitch
    功能        ：开始向电视墙交换参数
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt * ptSrc, 源
                  u8 byEqpId,
                  u8 byChnlIdx, 
	              u8 bySrcType 成员类型
                  u8 byState 
                  BOOL32 bNotify :是否向电视墙外设发送停止消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/05/23    3.6         libo          创建
====================================================================*/
void CMcuVcInst::ChangeTvWallSwitch(TMt * ptSrc, u8 byTvWallId, u8 byChnlIdx,u8 byTWMemberType, u8 byState, BOOL32 bEqpStop)
{
    CServMsg      cServMsg;
    TTWSwitchInfo tTWSwitchInfo;
    TPeriEqpStatus tTWStatus;    
    BOOL32 bSndMtStatus = FALSE; 
    BOOL32 bMultiInTv = FALSE;    
    
    u8 byMode;
    u8 byTvWallOutputMode = g_cMcuVcApp.GetTvWallOutputMode(byTvWallId);

    if (TW_OUTPUTMODE_AUDIO == byTvWallOutputMode)
    {
        byMode = MODE_AUDIO;
    }
    else if (TW_OUTPUTMODE_VIDEO == byTvWallOutputMode)
    {
        byMode = MODE_VIDEO;
    }
    else if (TW_OUTPUTMODE_BOTH == byTvWallOutputMode)
    {
        byMode = MODE_BOTH;
    }
    else
    {
        byMode = MODE_VIDEO;
    }

    StopSwitchToPeriEqp(byTvWallId, byChnlIdx, FALSE, byMode);
        
    g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTWStatus);
    
    TMt tOldMt = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx];        
    if(!tOldMt.IsNull() && IsMtNotInOtherTvWallChnnl(tOldMt.GetMtId(), byTvWallId, byChnlIdx))
    {
        m_ptMtTable->SetMtInTvWall(tOldMt.GetMtId(), FALSE);
        bSndMtStatus = TRUE;
    }    
    // 顾振华 [5/23/2006] 成员必须是终端，而不能是外设
    if(NULL != ptSrc && !ptSrc->IsNull() && ptSrc->GetType() != TYPE_MCUPERI)
    {
        //在ack中更新比较好
        m_ptMtTable->SetMtInTvWall(ptSrc->GetMtId()); 
        bSndMtStatus = TRUE;
    }
    if(bSndMtStatus)
    {
        MtStatusChange();
    }
        
    //更新新通道信息
    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType = byTWMemberType;
    // 顾振华 [5/23/2006] 成员必须是终端，而不能是外设
    if (ptSrc != NULL && ptSrc->GetType() == TYPE_MCUPERI)
    {
        ptSrc->SetNull();
    }
    if (ptSrc == NULL || ptSrc->IsNull())
    {
        tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].SetNull();
        tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].SetConfIdx(m_byConfIdx);
    }
    else
    {
        tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].SetMt(*ptSrc);
    }
    g_cMcuVcApp.SetPeriEqpStatus(byTvWallId, &tTWStatus);

    //status notification
    cServMsg.SetMsgBody((u8 *)&tTWStatus, sizeof(tTWStatus));
    SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);

    if ((TW_STATE_START == byState || TW_STATE_CHANGE == byState) && ptSrc != NULL)
    {
        TEqp tEqp = g_cMcuVcApp.GetEqp(byTvWallId);

        tTWSwitchInfo.SetSrcMt(*ptSrc);
        tTWSwitchInfo.SetDstMt(tEqp);
        tTWSwitchInfo.SetMode(byMode);

        tTWSwitchInfo.SetDstChlIdx(byChnlIdx);
        cServMsg.SetEventId(MCS_MCU_START_SWITCH_TW_REQ);
        SwitchSrcToDst(tTWSwitchInfo, cServMsg);
    }
    
    // 顾振华 [5/25/2006] 这里当传入的终端是NULL的情况下,请求停止本电视墙通道
    // 否则业务和电视墙内部记录的通道终端不一致
    // 当电视墙再次开始其他通道解码时,会给MCU发送错误的电视墙成员数组
    // 这个成员数组包含部分业务上已经置空但是电视并不知道停止的通道
    // FIXME: 现在的问题是请求Stop的开销是否很大, 为什么原来的代码
    //        只有在 bEqpStop 的情况下才去停止
    if ( (TW_STATE_STOP == byState ) || ptSrc == NULL )
    {
        cServMsg.SetChnIndex(byChnlIdx);
        cServMsg.SetEventId(MCU_TVWALL_STOP_PLAY_REQ);
        SendMsgToEqp(byTvWallId, MCU_TVWALL_STOP_PLAY_REQ, cServMsg);        
    }

    return;
}

/*------------------------------------------------------------------*/
/*                              HDU                                 */
/*------------------------------------------------------------------*/
/*====================================================================
    函数名      ：StopHduBatchPollSwitch
    功能        ：一个版面的轮询结束后拆交换
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bStopPlay: 是否在拆交换的同时停止HDU解码
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/04/13    4.6.1         江乐斌        创建
====================================================================*/
void   CMcuVcInst::StopHduBatchPollSwitch( BOOL32 bStopPlay )
{
	u8 byHduChlNum = 0;
	u8 byHduEqpId = 0;
	u8 byChnlIdx = 0;
	u8 byChnlPollNum = m_tHduBatchPollInfo.GetChnlPollNum();
	TPeriEqpStatus tHduStatus;
	CServMsg cServMsg;
	for ( ; byHduChlNum < byChnlPollNum; byHduChlNum ++ )
	{
		byHduEqpId = m_tHduBatchPollInfo.m_tChnlBatchPollInfo[byHduChlNum].GetEqpId();
		byChnlIdx = m_tHduBatchPollInfo.m_tChnlBatchPollInfo[byHduChlNum].GetChnlIdx();
		if ( byHduEqpId >= HDUID_MIN && byHduEqpId <= HDUID_MAX )
		{
			g_cMcuVcApp.GetPeriEqpStatus( byHduEqpId, &tHduStatus );
			tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnlIdx].SetSchemeIdx( 0 );
			g_cMcuVcApp.SetPeriEqpStatus( byHduEqpId, &tHduStatus );

			if ( 0 == tHduStatus.m_byOnline )    // hdu不在线
			{
				tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetNull();
				g_cMcuVcApp.SetPeriEqpStatus( byHduEqpId, &tHduStatus );
				cServMsg.SetMsgBody( (u8*)&tHduStatus, sizeof(tHduStatus) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF,  cServMsg);
			}
			else
			{
				TMt tMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx];
				tMt = GetLocalMtFromOtherMcuMt( tMt );
                
                if (bStopPlay)
                {
                    ChangeHduSwitch( &tMt, byHduEqpId, byChnlIdx, TW_MEMBERTYPE_BATCHPOLL, TW_STATE_STOP );	
                }
                else
                {
                    ChangeHduSwitch( &tMt, byHduEqpId, byChnlIdx, TW_MEMBERTYPE_BATCHPOLL, TW_STATE_STOP, MODE_BOTH, TRUE );	
                }
			}
		}
		else
		{
			ConfLog( FALSE, "[StopHduBatchPollSwitch] stop hdu(%d) batch poll error!\n", byHduEqpId);
			return;
		}
	}

}

/*====================================================================
    函数名      ：HduBatchPollOfOneCycle
    功能        ：hdu一个版面的批量轮询
    算法实现    ：
    引用全局变量：
    输入参数说明：bFirstCycle --> 是否是第一个版面的轮询
    返回值说明  ：TRUE --> 成功
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/04/13    4.6.1         江乐斌        创建
====================================================================*/
BOOL32 CMcuVcInst::HduBatchPollOfOneCycle( BOOL32 bFirstCycle )
{	
	CServMsg cServMsg;
	THduChnlPollInfo *ptHduChnlPollInfo = (THduChnlPollInfo*)m_tHduBatchPollInfo.m_tChnlBatchPollInfo;
	u8 byChnlPollNum = m_tHduBatchPollInfo.GetChnlPollNum();
	u8 byMtId = m_tHduBatchPollInfo.GetCurrentMtId() + 1;
	TPeriEqpStatus tHduStatus;
	u8 byLoop = 0;
	u8 byIndex = 0;
	u8 byEqpIdTemp = 0;
    u8 byChnlIdxTemp = 0;

	//如果当前会议里已无任何与会终端，则批量轮询结束
	for (byLoop = 1; byLoop<= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtInConf( byLoop ) )
		{
			break;
		}
	}
	if( MAXNUM_CONF_MT < byLoop )
	{
		return FALSE;
	}

	while(  byIndex < byChnlPollNum  )
	{

		for ( byLoop = byMtId; byLoop <= MAXNUM_CONF_MT; byLoop ++ )
		{
			if ( !m_tConfAllMtInfo.MtInConf( byLoop ) )
			{
				continue;
			}
			else
			{
				byMtId = byLoop;
				break;
			}
		}
		
// 		if ( 0 == byIndex && byLoop == MAXNUM_CONF_MT + 1)   //没有受邀终端
// 		{
//             return FALSE;
// 		}

		// 一个周期结束，该轮下一个周期了
		if ( byMtId == m_tHduBatchPollInfo.GetCurrentMtId() + 1 && byMtId != byLoop)
		{
            if ( 0 != m_tHduBatchPollInfo.GetCycles() 
				 && m_tHduBatchPollInfo.GetCurrentCycle() < m_tHduBatchPollInfo.GetCycles()
				)
            {
				m_tHduBatchPollInfo.SetNextCycle();
				u8 byKeepCycles = m_tHduBatchPollInfo.GetCycles() - m_tHduBatchPollInfo.GetCurrentCycle();
                m_tHduPollSchemeInfo.SetCycles( byKeepCycles );
				m_tHduBatchPollInfo.SetCurrentMtId( 0 );
				byMtId = m_tHduBatchPollInfo.GetCurrentMtId() + 1;
				byIndex = 0;
				byLoop = 0;
				m_tConf.m_tStatus.GetHduPollInfo()->SetPollNum( byKeepCycles );
				ConfStatusChange();
                // 告诉会控轮询次数递减
				cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof(m_tHduPollSchemeInfo) );
		        SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );
            }
			else if ( 0 == m_tHduBatchPollInfo.GetCycles() )      // 无限轮询
			{
                m_tHduBatchPollInfo.SetCurrentMtId( 0 );
                byMtId = m_tHduBatchPollInfo.GetCurrentMtId() + 1;
				byIndex = 0;
				byLoop = 0;
			}
			else if ( m_tHduBatchPollInfo.GetCurrentCycle() == m_tHduBatchPollInfo.GetCycles() )   // 轮询结束
			{
				m_tHduPollSchemeInfo.SetCycles( 0 );
				m_tConf.m_tStatus.GetHduPollInfo()->SetPollNum( 0 );
				ConfStatusChange();
				cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof(m_tHduPollSchemeInfo) );
		        SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );
				return TRUE;
			}

			continue;
		}

		//  整个受邀终端不够一个版面
		if ( byLoop > MAXNUM_CONF_MT )
		{
			while( byIndex < byChnlPollNum )
			{
				byEqpIdTemp = ptHduChnlPollInfo[byIndex].GetEqpId();
				byChnlIdxTemp = ptHduChnlPollInfo[byIndex].GetChnlIdx();

           		g_cMcuVcApp.GetPeriEqpStatus( byEqpIdTemp, &tHduStatus );
                TMt  tSrcMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdxTemp];
				tSrcMt = GetLocalMtFromOtherMcuMt( tSrcMt ); 

                ChangeHduSwitch( &tSrcMt, byEqpIdTemp, \
						byChnlIdxTemp, TW_MEMBERTYPE_BATCHPOLL, TW_STATE_STOP, MODE_BOTH, TRUE );

                byIndex ++;				
			}

			break;
		}
		
		THduMember tHduMember;     //  保存不能建交换的终端
		memset(&tHduMember, 0x0, sizeof(THduMember));
		tHduMember.byMemberType = TW_MEMBERTYPE_BATCHPOLL;
		
		BOOL32 bSendMtStatus = FALSE;
		TMt tOldMt;
		memset( &tOldMt, 0x0, sizeof( TMt ) );
		byEqpIdTemp = ptHduChnlPollInfo[byIndex].GetEqpId();
		byChnlIdxTemp = ptHduChnlPollInfo[byIndex].GetChnlIdx();
		g_cMcuVcApp.GetPeriEqpStatus( byEqpIdTemp, &tHduStatus );
		if ( 0 == tHduStatus.m_byOnline )  // 为该hdu通道预留对应终端，待hdu上线后自动恢复正常状态
		{
			tOldMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdxTemp];        
			if(!tOldMt.IsNull() 
				&& IsMtNotInOtherHduChnnl(tOldMt.GetMtId(), byEqpIdTemp, byChnlIdxTemp)
				&& TYPE_MT == tOldMt.GetType())
			{
				m_ptMtTable->SetMtInHdu(tOldMt.GetMtId(), FALSE);
				bSendMtStatus = TRUE;
			}    
			
			if ( bSendMtStatus )
			{
			    MtStatusChange(0, TRUE);
			}
			
			memcpy( &tHduMember, &m_ptMtTable->GetMt( byMtId ), sizeof(TMt) );
			tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdxTemp] = tHduMember;               
			g_cMcuVcApp.SetPeriEqpStatus( byEqpIdTemp, &tHduStatus );

		}
		else
		{
			if ( m_tConfAllMtInfo.MtJoinedConf( byMtId ) )   // 与会终端直接建交换
			{
				TMt tLocalMt = GetLocalMtFromOtherMcuMt( m_ptMtTable->GetMt( byMtId ) ); 
				if ( bFirstCycle)
				{
					ChangeHduSwitch( &tLocalMt, byEqpIdTemp, \
						byChnlIdxTemp, TW_MEMBERTYPE_BATCHPOLL, TW_STATE_START, MODE_BOTH, TRUE);
				}
				else
				{
					ChangeHduSwitch( &tLocalMt, byEqpIdTemp, \
						byChnlIdxTemp, TW_MEMBERTYPE_BATCHPOLL, TW_STATE_CHANGE, MODE_BOTH, TRUE);
				}
			}
			else    
			{
				tOldMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdxTemp];        
				if(!tOldMt.IsNull() 
					&& IsMtNotInOtherHduChnnl(tOldMt.GetMtId(), byEqpIdTemp, byChnlIdxTemp)
					&& TYPE_MT == tOldMt.GetType())
				{					
					m_ptMtTable->SetMtInHdu(tOldMt.GetMtId(), FALSE);
					bSendMtStatus = TRUE;
				}    
                
				if ( bSendMtStatus )
				{
					MtStatusChange(0, TRUE);
				}

				memcpy( &tHduMember, &(m_ptMtTable->GetMt( byMtId )), sizeof(TMt) );
				tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdxTemp] = tHduMember;
				
				g_cMcuVcApp.SetPeriEqpStatus( byEqpIdTemp, &tHduStatus );

                cServMsg.SetMsgBody( (u8*)&tHduStatus, sizeof(tHduStatus) );
                SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );
				
			}
		}

        byMtId ++;
		byIndex ++;
	}

    m_tHduBatchPollInfo.SetCurrentMtId( (byMtId - 1) % MAXNUM_CONF_MT );
	
	return TRUE;	

}

/*====================================================================
    函数名      ：ChangeHduSwitch
    功能        ：开始向Hdu交换参数
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt * ptSrc, 源
                  u8 byEqpId,
                  u8 byChnlIdx, 
				  u8 byHduMemberType 成员类型
                  u8 byState 
                  BOOL32 bNotify :是否向电视墙外设发送停止消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/01/19    4.6.1         江乐斌        创建
====================================================================*/
void CMcuVcInst::ChangeHduSwitch(TMt * ptSrc,
                                 u8    byHduId,
                                 u8    byChnlIdx,
                                 u8    byHduMemberType,
                                 u8    byState,
                                 u8    byMode/* = MODE_BOTH*/,
                                 BOOL32 bBatchPoll/* = FALSE*/ )
{
	if ( NULL == ptSrc )
	{
		ConfLog( FALSE, "[CMcuVcInst] ptSrc is Null!\n");
	}

    CServMsg      cServMsg;
    THduSwitchInfo tHduSwitchInfo;
    TPeriEqpStatus tHduStatus;    
    BOOL32 bSndMtStatus = FALSE; 

	g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
    
    u8 byHduOutputMode = tHduStatus.m_tStatus.tHdu.byOutputMode;
	u8 byOldMode;

    if (HDU_OUTPUTMODE_BOTH == byHduOutputMode)
    {
        byOldMode = MODE_BOTH;
    }
    else if (HDU_OUTPUTMODE_VIDEO == byHduOutputMode)
    {
        byOldMode = MODE_VIDEO;
    }
    else if (HDU_OUTPUTMODE_AUDIO == byHduOutputMode)
    {
        byOldMode = MODE_AUDIO;
    }
    else
    {
        byOldMode = MODE_VIDEO;
    }
		
    StopSwitchToPeriEqp(byHduId, byChnlIdx, FALSE, byOldMode);
	
	if ( MODE_BOTH == byMode )
	{
		byHduOutputMode = HDU_OUTPUTMODE_BOTH;
	}
	else if ( MODE_VIDEO == byMode )
	{
		byHduOutputMode = HDU_OUTPUTMODE_VIDEO;
	}
	else if ( MODE_AUDIO == byMode )
	{
		byHduOutputMode = HDU_OUTPUTMODE_AUDIO;
	}
	else
	{
		byHduOutputMode = HDU_OUTPUTMODE_VIDEO;
	}
	
    tHduStatus.m_tStatus.tHdu.byOutputMode = byHduOutputMode;

    TMt tOldMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx];        
    if(!tOldMt.IsNull() 
		&& IsMtNotInOtherHduChnnl(tOldMt.GetMtId(), byHduId, byChnlIdx)
		&& TYPE_MT == tOldMt.GetType())
    {
        m_ptMtTable->SetMtInHdu(tOldMt.GetMtId(), FALSE);
        bSndMtStatus = TRUE;
    }    

    //在ack中更新比较好
	if (NULL!=ptSrc 
		&& MT_TYPE_MT == ptSrc->GetMtType()
		&& TW_STATE_STOP != byState)
	{
		m_ptMtTable->SetMtInHdu(ptSrc->GetMtId()); 
		bSndMtStatus = TRUE;
	}
    
	if(bSndMtStatus)
    {
		if ( bBatchPoll )
		{
            MtStatusChange(0, TRUE);
		}
		else
		{
			MtStatusChange();
		}
    }
	
    //更新通道信息
    tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType = byHduMemberType;
	tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnlIdx].SetChnIdx(byChnlIdx);
	if ( tHduStatus.m_tStatus.tHdu.byChnnlNum < MAXNUM_HDU_CHANNEL)
	{
	    tHduStatus.m_tStatus.tHdu.byChnnlNum++;
	}
    // 成员必须是终端，而不能是外设, VMP 除外
    if (ptSrc != NULL && ptSrc->GetType() == TYPE_MCUPERI && EQP_TYPE_VMP != ptSrc->GetEqpType())
    {
        ptSrc->SetNull();
    }
    if (ptSrc == NULL || ptSrc->IsNull())
    {
        tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetNull();
		tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetEqpType( 0 );
		tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetType( 0 );
        tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetConfIdx(m_byConfIdx);
    }
    else
    {
		if ( ptSrc->GetType() == TYPE_MCUPERI)
		{
            tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetMcuEqp(ptSrc->GetMcuId(), ptSrc->GetEqpId(), ptSrc->GetEqpType());
            tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetConfIdx( ptSrc->GetConfIdx() );
		}
		else
		{
			TMt tSrct = m_ptMtTable->GetMt( ptSrc->GetMtId() );
            memcpy( &tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx], &tSrct, sizeof(TMt));
		    tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType = byHduMemberType;
		}
	}
    
	if ( ( TW_STATE_STOP == byState || ptSrc == NULL )
		 &&(TW_MEMBERTYPE_MCSSPEC == byHduMemberType 
		    || TW_MEMBERTYPE_SWITCHVMP == byHduMemberType
			|| TW_MEMBERTYPE_TWPOLL == byHduMemberType
			)
		)
	{
		tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType = 0;
		tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetNull();
        tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetConfIdx( (u8)0 );
	}

	if ( TW_STATE_STOP == byState 
		&& ( TW_MEMBERTYPE_CHAIRMAN == byHduMemberType  
		     || TW_MEMBERTYPE_SPEAKER == byHduMemberType
			 || TW_MEMBERTYPE_BATCHPOLL == byHduMemberType
			)
		)
	{
		tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetNull();
	}

    g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);

    //status notification
    cServMsg.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
    SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
	
	if ((TW_STATE_START == byState || TW_STATE_CHANGE == byState) && ptSrc != NULL)
    {
        TEqp tEqp = g_cMcuVcApp.GetEqp(byHduId);
		
        tHduSwitchInfo.SetSrcMt(*ptSrc);
        tHduSwitchInfo.SetDstMt(tEqp);
        tHduSwitchInfo.SetMode(byMode);
		
        tHduSwitchInfo.SetDstChlIdx(byChnlIdx);
		u8 bySrcChnnl = 0; 
		if(TW_MEMBERTYPE_SWITCHVMP == byHduMemberType)	// xliang [6/23/2009] 选看VMP
		{	
			u8 byMediaType = m_tConf.GetMainVideoMediaType();
			u8 byRes = m_tConf.GetMainVideoFormat();
			bySrcChnnl = GetVmpOutChnnlByRes(byRes, m_tVmpEqp.GetEqpId(), byMediaType);
		}
		tHduSwitchInfo.SetSrcChlIdx(bySrcChnnl);

		cServMsg.SetEventId(MCS_MCU_START_SWITCH_HDU_REQ);

        SwitchSrcToDst(tHduSwitchInfo, cServMsg, (TW_STATE_CHANGE == byState) & bBatchPoll);
    }
    

    if (!bBatchPoll && 
        (TW_STATE_STOP == byState || ptSrc == NULL))
    {
        cServMsg.SetChnIndex(byChnlIdx);
        cServMsg.SetEventId(MCU_HDU_STOP_PLAY_REQ);
        SendMsgToEqp(byHduId, MCU_HDU_STOP_PLAY_REQ, cServMsg); 
    }
	
    return;
}

/*====================================================================
    函数名      : IsMtNotInOtherHduChnnl
    功能        是否需要清除终端在hdu通道的状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId   终端ID
	              u8 byHduId    HDUId
				  u8 byChnlId   通道ID
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/01/19    4.6.1         江乐斌        创建
====================================================================*/
BOOL32 CMcuVcInst::IsMtNotInOtherHduChnnl(u8 byMtId, u8 byHduId, u8 byChnlId)
{
	TPeriEqpStatus tHduStatus;
    g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
	
    //是否在本电视墙的其他通道里
    for(u8 byLp = 0; byLp < tHduStatus.m_tStatus.tHdu.byChnnlNum; byLp++)
    {
        if(byLp != byChnlId)
        {
            TMt tMtInTv = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byLp];
			// 增加判断终端所在会议号，否则会将其他会议的终端误判
            if(tMtInTv.GetMtId() == byMtId && tMtInTv.GetConfIdx() == m_byConfIdx)
            {
                return FALSE;
            }
        }
    }
	
    //是否在其他电视墙的通道里
    for(u8 byEqpHduId = HDUID_MIN; byEqpHduId <= HDUID_MAX; byEqpHduId++)
    {
        if(byHduId == byEqpHduId)
            continue;
        
        g_cMcuVcApp.GetPeriEqpStatus(byEqpHduId, &tHduStatus);
        if( tHduStatus.m_byOnline )
        {
            for(u8 byLp = 0; byLp < tHduStatus.m_tStatus.tHdu.byChnnlNum; byLp++)
            {
                TMt tMtInTv = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byLp];
				// 增加判断终端所在会议号，否则会将其他会议的终端误判
                if(tMtInTv.GetMtId() == byMtId && tMtInTv.GetConfIdx() == m_byConfIdx)
                {
                    return FALSE;
                }
            }
        }
    }

	return TRUE;
}

/*------------------------------------------------------------------*/
/*                              PRS                                 */
/*------------------------------------------------------------------*/
/*====================================================================
    函数名      ：OccupyPrsChnl
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/07/15    4.0         付秀华        创建
====================================================================*/
void CMcuVcInst::OccupyPrsChnl(u8 byPrsId, u8 byPrsChnl)
{
	TPeriEqpStatus tPrsStaus;
	g_cMcuVcApp.GetPeriEqpStatus(byPrsId, &tPrsStaus);
	tPrsStaus.m_tStatus.tPrs.SetChnConfIdx(byPrsChnl, m_byConfIdx);
	tPrsStaus.m_tStatus.tPrs.m_tPerChStatus[byPrsChnl].SetReserved(TRUE);
	g_cMcuVcApp.SetPeriEqpStatus(byPrsId, &tPrsStaus);

	CServMsg cServMsg;
	cServMsg.SetMsgBody((u8*)&tPrsStaus, sizeof(tPrsStaus));
	SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
}
/*====================================================================
    函数名      ：RlsPrsChnl
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/07/15    4.0         付秀华        创建
====================================================================*/
void CMcuVcInst::RlsPrsChnl(u8 byPrsId, u8 byPrsChnl)
{
	TPeriEqpStatus tPrsStaus;
	g_cMcuVcApp.GetPeriEqpStatus(byPrsId, &tPrsStaus);
	tPrsStaus.m_tStatus.tPrs.SetChnConfIdx(byPrsChnl, 0);
	tPrsStaus.m_tStatus.tPrs.m_tPerChStatus[byPrsChnl].SetReserved(FALSE);
	g_cMcuVcApp.SetPeriEqpStatus(byPrsId, &tPrsStaus);

	CServMsg cServMsg;
	cServMsg.SetMsgBody((u8*)&tPrsStaus, sizeof(tPrsStaus));
	SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);

	m_tConfPrsInfo.RemovePrsChnl(byPrsId, byPrsChnl);
}
/*====================================================================
    函数名      ：ChangePrsSrc
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：[IN] u8 byPrsId:      待更改的PRS设备号
	              [IN] u8 byPrsChnlPos: 待更改的PRS通道号
				  [IN] TMt& tEqp:       对应接收的信号源ID
				  [IN] u8 byEqpOutChnl: 对应信号源输出通道号
				  [IN] u8 byEqpInChnl:  对应信号源输入通道号
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/07/15    4.0         付秀华        创建
====================================================================*/
BOOL CMcuVcInst::ChangePrsSrc(u8 byPrsId, u8 byPrsChnlPos, 
							  const TMt& tEqp,  u8 byEqpOutChnl /*= 0*/, u8 byEqpInChnl /*= 0*/)
{
	u8 byChlNum = 0;
	TLogicalChannel tEqpLogChnl;
    if (!g_cMcuVcApp.GetPeriEqpLogicChnnl(byPrsId, MODE_VIDEO, &byChlNum, &tEqpLogChnl, TRUE))
    {
        ConfLog(FALSE, "[ChangePrsSrc] Can't get prs(%d) logic channel, ChangePrsSrc failure!\n",
			    byPrsId);
        return FALSE;
    }

	TPrsParam tPrsParam;
	TTransportAddr tLocAddr;
	TTransportAddr tRemAddr;
	u32 dwRtcpSwitchIp;
	u16 wRtcpSwitchPort;
	switch(tEqp.GetEqpType())
	{
	// 目前只针对HDbas
	case EQP_TYPE_BAS:
        //bas 接收RTCP回馈包地址
        g_cMpManager.GetBasSwitchAddr(tEqp.GetEqpId(), dwRtcpSwitchIp, wRtcpSwitchPort);
        tRemAddr.SetIpAddr(dwRtcpSwitchIp);
        tRemAddr.SetPort(wRtcpSwitchPort + PORTSPAN*(byEqpInChnl * MAXNUM_VOUTPUT + byEqpOutChnl) + 1);
        tPrsParam.SetRemoteAddr(tRemAddr);
        
        //设置丢包重传器接收RTP包的地址
        tLocAddr.SetIpAddr(tEqpLogChnl.GetRcvMediaChannel().GetIpAddr());
        tLocAddr.SetPort(tEqpLogChnl.GetRcvMediaChannel().GetPort() + PORTSPAN * byPrsChnlPos);
        tPrsParam.SetLocalAddr(tLocAddr);
		break;
	default:
		ConfLog(FALSE,  "[ChangePrsSrc] unknown type(%d) of prssrc\n", tEqp.GetEqpType());
		return FALSE;
	    break;
	}

	EqpLog("[ChangePrsSrc] Set src(id:%d, type:%d) for prs(id:%d, chnl:%d)\n", 
		   tEqp.GetEqpId(), tEqp.GetEqpType(), byPrsId, byPrsChnlPos);
	CServMsg cServMsg;
    cServMsg.SetChnIndex(byPrsChnlPos);
    cServMsg.SetMsgBody((u8*)&tPrsParam, sizeof(tPrsParam));
    SendMsgToEqp(byPrsId, MCU_PRS_SETSRC_REQ, cServMsg);
	
	return TRUE;
}
/*====================================================================
    函数名      ：ChangePrsSrc
    功能        ：改变丢包重传的接收源(终端接收RTCP回馈包的地址, 丢包重传器接收RTP包的地址)
    算法实现    ：在ack中将广播源rtp -> prs, 各与会MT的RTCP -> PRS,这里只改变不需bas适配的prs源信息，
                  其他prs源信息与bas绑定处理,及开始bas时改变prs源，停止bas时
    引用全局变量：
    输入参数说明： TMt tMt 广播源终端
                    BOOL32 bSrcBas  源是否为bas
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/03/27    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ChangePrsSrc(TMt tMt, u8 byPrsChanMode, BOOL32 bSrcBas)
{
    EqpLog("[ChangePrsSrc] Start MtId.%d PrsChanMode.%d\n", tMt.GetMtId(), byPrsChanMode);

    CServMsg  cServMsg;
    TPrsParam tPrsParam;
    TPrsParam tPrsParam2;
    TPrsParam tPrsParamAud;
    TPrsParam tPrsParamAudBas;
    TPrsParam tPrsParamVidBas;
    TPrsParam tPrsParamBrBas;
    //TPrsParam tPrsParamHDVidBas;    // 高清适配主视频丢包重传参数, zgc, 2008-08-13
    TLogicalChannel tMtLogicalChannel;
    TLogicalChannel tMtLogicalChannel2;
    TLogicalChannel tMtLogicalChannelAud;
    TLogicalChannel tEqpLogicalChannel;

    u8  byEqpType;
    u8  byChlNum;
    u32 dwEqpIP;
    TTransportAddr tLocalAddr;
    TTransportAddr tDstAddr;
    u32 dwRtcpSwitchIp;
    u16 wRtcpSwitchPort;

    //是否有预留资源
    if (m_tPrsEqp.IsNull())
    {
        ConfLog(FALSE, "[ChangePrsSrc] End1 MtId.%d PrsChanMode.%d\n", tMt.GetMtId(), byPrsChanMode);

        return;
    }

    if (!g_cMcuVcApp.GetPeriEqpLogicChnnl(m_tPrsEqp.GetEqpId(), MODE_VIDEO, &byChlNum, &tEqpLogicalChannel, TRUE))
    {
        ConfLog(FALSE, "[ChangePrsSrc] Can't get mt logic channel, ChangePrsSrc failure!\n");
        return;
    }

    //填IP地址
    if (SUCCESS_AGENT == g_cMcuAgent.GetPeriInfo(m_tPrsEqp.GetEqpId(), &dwEqpIP, &byEqpType))
    {
        tLocalAddr.SetNetSeqIpAddr(dwEqpIP);
        tLocalAddr.SetPort(0 );
        tPrsParam.SetLocalAddr(tLocalAddr);
        tPrsParam2.SetLocalAddr(tLocalAddr);
        tPrsParamAud.SetLocalAddr(tLocalAddr);
        tPrsParamAudBas.SetLocalAddr(tLocalAddr);
        tPrsParamVidBas.SetLocalAddr(tLocalAddr);
        tPrsParamBrBas.SetLocalAddr(tLocalAddr);
    }

    //数据源是否为bas
    if(bSrcBas)
    {
        TLogicalChannel tSrcLogicalChannel;
        
        if(EQP_CHANNO_INVALID != m_byPrsChnnlVidBas && !m_tVidBasEqp.IsNull())
        {                        
            //bas 接收RTCP回馈包地址
            g_cMpManager.GetBasSwitchAddr(m_tVidBasEqp.GetEqpId(), dwRtcpSwitchIp, wRtcpSwitchPort);
            tDstAddr.SetIpAddr(dwRtcpSwitchIp);
            tDstAddr.SetPort(wRtcpSwitchPort + PORTSPAN*m_byVidBasChnnl + 1);
            tPrsParamVidBas.SetRemoteAddr(tDstAddr);
            
            //设置丢包重传器接收RTP包的地址
            tLocalAddr = tPrsParam.GetLocalAddr();
            tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVidBas);
            tPrsParamVidBas.SetLocalAddr(tLocalAddr);
        }
        
        if(EQP_CHANNO_INVALID != m_byPrsChnnlBrBas && !m_tBrBasEqp.IsNull())
        {
            //bas 接收RTCP回馈包地址
            g_cMpManager.GetBasSwitchAddr(m_tBrBasEqp.GetEqpId(), dwRtcpSwitchIp, wRtcpSwitchPort);
            tDstAddr.SetIpAddr(dwRtcpSwitchIp);
            tDstAddr.SetPort(wRtcpSwitchPort + PORTSPAN*m_byBrBasChnnl + 1);
            tPrsParamBrBas.SetRemoteAddr(tDstAddr);
            
            //设置丢包重传器接收RTP包的地址
            tLocalAddr = tPrsParam.GetLocalAddr();
            tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlBrBas);
            tPrsParamBrBas.SetLocalAddr(tLocalAddr);
        }
        
        if(EQP_CHANNO_INVALID != m_byPrsChnnlAudBas && !m_tAudBasEqp.IsNull())
        {
            //bas 接收RTCP回馈包地址
            g_cMpManager.GetBasSwitchAddr(m_tAudBasEqp.GetEqpId(), dwRtcpSwitchIp, wRtcpSwitchPort);
            tDstAddr.SetIpAddr(dwRtcpSwitchIp);
            tDstAddr.SetPort(wRtcpSwitchPort + PORTSPAN*m_byAudBasChnnl + 3);
            tPrsParamAudBas.SetRemoteAddr(tDstAddr);
            
            //设置丢包重传器接收RTP包的地址
            tLocalAddr = tPrsParam.GetLocalAddr();
            tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlAudBas);
            tPrsParamAudBas.SetLocalAddr(tLocalAddr);
        }

        //给丢包重传器发消息
        if (PRSCHANMODE_AUDBAS == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlAudBas)
        {
            if (TYPE_MT == tMt.GetType())
            {
                cServMsg.SetSrcMtId(tMt.GetMtId());
                cServMsg.SetChnIndex(m_byPrsChnnlAudBas);
                cServMsg.SetMsgBody((u8*)&tPrsParamAudBas, sizeof(tPrsParamAudBas));
                SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_SETSRC_REQ, cServMsg);
                m_tConfInStatus.SetPrsChnlAudBasStart(TRUE);

                //把bas的Rtcp交换给源mt
                m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tSrcLogicalChannel, FALSE);                    
                u32 dwDstIp = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
                u16 wDstPort = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetPort();
                
                SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tAudBasEqp, m_byAudBasChnnl, MODE_AUDIO, SWITCH_MODE_BROADCAST);
            }
        }
        
        //给丢包重传器发消息
        if (PRSCHANMODE_VIDBAS == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlVidBas)
        {
            if(TYPE_MT == tMt.GetType())
            {
                cServMsg.SetSrcMtId(tMt.GetMtId());
                cServMsg.SetChnIndex(m_byPrsChnnlVidBas);
                cServMsg.SetMsgBody((u8*)&tPrsParamVidBas, sizeof(tPrsParamVidBas));
                SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_SETSRC_REQ, cServMsg);
                m_tConfInStatus.SetPrsChnlVidBasStart(TRUE);

                //把bas的Rtcp交换给源mt
                m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tSrcLogicalChannel, FALSE);                    
                u32 dwDstIp = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
                u16 wDstPort = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetPort();
                
                SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tVidBasEqp, m_byVidBasChnnl, MODE_VIDEO, SWITCH_MODE_BROADCAST);
            }
        }
        
        //给丢包重传器发消息
        if(PRSCHANMODE_BRBAS == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlBrBas)
        {
            if(TYPE_MT == tMt.GetType())
            {
                cServMsg.SetSrcMtId(tMt.GetMtId());
                cServMsg.SetChnIndex(m_byPrsChnnlBrBas);
                cServMsg.SetMsgBody((u8*)&tPrsParamBrBas, sizeof(tPrsParamBrBas));
                SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_SETSRC_REQ, cServMsg);
                m_tConfInStatus.SetPrsChnlBrBasStart(TRUE);

                //把bas的Rtcp交换给源mt
                m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tSrcLogicalChannel, FALSE);                    
                u32 dwDstIp = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
                u16 wDstPort = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetPort();
                
                SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tBrBasEqp, m_byBrBasChnnl, MODE_VIDEO, SWITCH_MODE_BROADCAST);
            }
        }

        // FIXME：高清适配丢包重传未添加,zgc
        if ( PRSCHANMODE_HDBAS_VID == byPrsChanMode /*&& m_cConfBasChnMgr.GetVidChnUseNum() > 0*/ )
        {
            if(TYPE_MT == tMt.GetType())
            {
                cServMsg.SetSrcMtId(tMt.GetMtId());
                cServMsg.SetChnIndex(m_byPrsChnnlBrBas);
                cServMsg.SetMsgBody((u8*)&tPrsParamBrBas, sizeof(tPrsParamBrBas));
                SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_SETSRC_REQ, cServMsg);
                m_tConfInStatus.SetPrsChnlBrBasStart(TRUE);
                
                //把bas的Rtcp交换给源mt
                m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tSrcLogicalChannel, FALSE);                    
                u32 dwDstIp = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
                u16 wDstPort = tSrcLogicalChannel.GetSndMediaCtrlChannel().GetPort();
                
                SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tBrBasEqp, m_byBrBasChnnl, MODE_VIDEO, SWITCH_MODE_BROADCAST);
            }
        }
        
        return;
    }

    //vmp第二路码流prs设置
    if (PRSCHANMODE_VMP2 == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlBrBas)
    {
        //vmp 接收RTCP回馈包地址            
        u32 dwEqpIP,dwRcvIp;
        u16 wRcvPort;
        g_cMpManager.GetSwitchInfo(tMt, dwRcvIp, wRcvPort, dwEqpIP);
        
        TEqpVMPInfo tVmpInfo;
        g_cMcuAgent.GetEqpVMPCfg( tMt.GetEqpId(), &tVmpInfo);

        tDstAddr.SetIpAddr(dwEqpIP);          
        tDstAddr.SetPort(tVmpInfo.GetEqpRecvPort() - PORTSPAN*2 + 1);          
        
        tPrsParam.SetRemoteAddr(tDstAddr);
        
        //设置丢包重传器接收RTP包的地址
        tLocalAddr = tPrsParam.GetLocalAddr();
        tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlBrBas);
        tPrsParam.SetLocalAddr(tLocalAddr);

        cServMsg.SetSrcMtId(0);
        cServMsg.SetChnIndex(m_byPrsChnnlBrBas);
        cServMsg.SetMsgBody((u8*)&tPrsParam, sizeof(tPrsParam));
        SendMsgToEqp(m_tPrsEqp.GetEqpId() , MCU_PRS_SETSRC_REQ, cServMsg);
        m_tConfInStatus.SetPrsChnlBrBasStart(TRUE);
        EqpLog("[ChangePrsSrc] Msg MCU_PRS_SETSRC_REQ send to prs eqp %d chl %d!\n", m_tPrsEqp.GetEqpId(), m_byPrsChnnlBrBas);
    }
	
	// xliang [4/30/2009] 多路输出类广播源4出码流prs设置
    if (PRSCHANMODE_VMPOUT1 == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut1)
    {
        //广播源接收RTCP回馈包地址            
        u32 dwEqpIP,dwRcvIp;
        u16 wRcvPort;
        g_cMpManager.GetSwitchInfo(tMt, dwRcvIp, wRcvPort, dwEqpIP);
        
        TEqpVMPInfo tVmpInfo;
        g_cMcuAgent.GetEqpVMPCfg( tMt.GetEqpId(), &tVmpInfo);
		
        tDstAddr.SetIpAddr(dwEqpIP);				
		//tDstAddr.SetIpAddr(tVmpInfo.GetIpAddr());	
        tDstAddr.SetPort(tVmpInfo.GetEqpRecvPort() + 1);          
        
        tPrsParam.SetRemoteAddr(tDstAddr);
        
        //设置丢包重传器接收RTP包的地址
        tLocalAddr = tPrsParam.GetLocalAddr();
        tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut1);
        tPrsParam.SetLocalAddr(tLocalAddr);
		
        cServMsg.SetSrcMtId(0);
        cServMsg.SetChnIndex(m_byPrsChnnlVmpOut1);
        cServMsg.SetMsgBody((u8*)&tPrsParam, sizeof(tPrsParam));
        SendMsgToEqp(m_tPrsEqp.GetEqpId() , MCU_PRS_SETSRC_REQ, cServMsg);
        m_tConfInStatus.SetPrsChnlVmpOut1Start(TRUE); 
        EqpLog("[ChangePrsSrc] Msg MCU_PRS_SETSRC_REQ send to prs eqp %d chl %d(m_byPrsChnnlVmpOut1)!\n", m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut1);
    }

	if (PRSCHANMODE_VMPOUT2 == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut2)
    {
        //广播源接收RTCP回馈包地址            
        u32 dwEqpIP,dwRcvIp;
        u16 wRcvPort;
        g_cMpManager.GetSwitchInfo(tMt, dwRcvIp, wRcvPort, dwEqpIP);
        
        TEqpVMPInfo tVmpInfo;
        g_cMcuAgent.GetEqpVMPCfg( tMt.GetEqpId(), &tVmpInfo);
		
        tDstAddr.SetIpAddr(dwEqpIP);          
        tDstAddr.SetPort(tVmpInfo.GetEqpRecvPort() + PORTSPAN*1 + 1);          
        
        tPrsParam.SetRemoteAddr(tDstAddr);
        
        //设置丢包重传器接收RTP包的地址
        tLocalAddr = tPrsParam.GetLocalAddr();
        tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut2);
        tPrsParam.SetLocalAddr(tLocalAddr);
		
        cServMsg.SetSrcMtId(0);
        cServMsg.SetChnIndex(m_byPrsChnnlVmpOut2);
        cServMsg.SetMsgBody((u8*)&tPrsParam, sizeof(tPrsParam));
        SendMsgToEqp(m_tPrsEqp.GetEqpId() , MCU_PRS_SETSRC_REQ, cServMsg);
		m_tConfInStatus.SetPrsChnlVmpOut2Start(TRUE);
        EqpLog("[ChangePrsSrc] Msg MCU_PRS_SETSRC_REQ send to prs eqp %d chl %d(m_byPrsChnnlVmpOut2)!\n", m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut2);
    }

	if (PRSCHANMODE_VMPOUT3 == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut3)
    {
        //广播源接收RTCP回馈包地址            
        u32 dwEqpIP,dwRcvIp;
        u16 wRcvPort;
        g_cMpManager.GetSwitchInfo(tMt, dwRcvIp, wRcvPort, dwEqpIP);
        
        TEqpVMPInfo tVmpInfo;
        g_cMcuAgent.GetEqpVMPCfg( tMt.GetEqpId(), &tVmpInfo);
		
        tDstAddr.SetIpAddr(dwEqpIP);          
        tDstAddr.SetPort(tVmpInfo.GetEqpRecvPort() + PORTSPAN*2 + 1);          
        
        tPrsParam.SetRemoteAddr(tDstAddr);
        
        //设置丢包重传器接收RTP包的地址
        tLocalAddr = tPrsParam.GetLocalAddr();
        tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut3);
        tPrsParam.SetLocalAddr(tLocalAddr);
		
        cServMsg.SetSrcMtId(0);
        cServMsg.SetChnIndex(m_byPrsChnnlVmpOut3);
        cServMsg.SetMsgBody((u8*)&tPrsParam, sizeof(tPrsParam));
        SendMsgToEqp(m_tPrsEqp.GetEqpId() , MCU_PRS_SETSRC_REQ, cServMsg);
        m_tConfInStatus.SetPrsChnlVmpOut3Start(TRUE); 
        EqpLog("[ChangePrsSrc] Msg MCU_PRS_SETSRC_REQ send to prs eqp %d chl %d(m_byPrsChnnlVmpOut3)!\n", m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut3);
    }
	
	if (PRSCHANMODE_VMPOUT4 == byPrsChanMode && EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut4)
    {
        //广播源接收RTCP回馈包地址            
        u32 dwEqpIP,dwRcvIp;
        u16 wRcvPort;
        g_cMpManager.GetSwitchInfo(tMt, dwRcvIp, wRcvPort, dwEqpIP);
        
        TEqpVMPInfo tVmpInfo;
        g_cMcuAgent.GetEqpVMPCfg( tMt.GetEqpId(), &tVmpInfo);
		
        tDstAddr.SetIpAddr(dwEqpIP);          
        tDstAddr.SetPort(tVmpInfo.GetEqpRecvPort() + PORTSPAN*3 + 1);          
        
        tPrsParam.SetRemoteAddr(tDstAddr);
        
        //设置丢包重传器接收RTP包的地址
        tLocalAddr = tPrsParam.GetLocalAddr();
        tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut4);
        tPrsParam.SetLocalAddr(tLocalAddr);
		
        cServMsg.SetSrcMtId(0);
        cServMsg.SetChnIndex(m_byPrsChnnlVmpOut4);
        cServMsg.SetMsgBody((u8*)&tPrsParam, sizeof(tPrsParam));
        SendMsgToEqp(m_tPrsEqp.GetEqpId() , MCU_PRS_SETSRC_REQ, cServMsg);
        m_tConfInStatus.SetPrsChnlVmpOut4Start(TRUE); 
        EqpLog("[ChangePrsSrc] Msg MCU_PRS_SETSRC_REQ send to prs eqp %d chl %d(m_byPrsChnnlVmpOut4)!\n", m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut4);
    }

    if (!tMt.IsNull())
    {	
        //仅对科达终端有效
        if (tMt.GetType() == TYPE_MT)
        {
            if (m_ptMtTable->GetManuId(tMt.GetMtId()) != MT_MANU_KDC && 
                m_ptMtTable->GetManuId(tMt.GetMtId()) != MT_MANU_KDCMCU)
            {
                EqpLog("[ChangePrsSrc] End2 MtId.%d PrsChanMode.%d\n", tMt.GetMtId(), byPrsChanMode);

                return;
            }

            //第一路视频
            if (m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, FALSE))
            {

                //设置终端接收RTCP回馈包的地址
                tDstAddr = tMtLogicalChannel.GetSndMediaCtrlChannel();
                tPrsParam.SetRemoteAddr(tDstAddr);

                //设置丢包重传器接收RTP包的地址(与MCU接收该终端的RTP包地址一致)
                tLocalAddr = tPrsParam.GetLocalAddr();
                tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnl);
                tPrsParam.SetLocalAddr(tLocalAddr);                    
            }

            //第二路视频
            if (m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_SECVIDEO, &tMtLogicalChannel2, FALSE))
            {
                //设置终端接收RTCP回馈包的地址
                tDstAddr = tMtLogicalChannel2.GetSndMediaCtrlChannel();
                tPrsParam2.SetRemoteAddr(tDstAddr);

                //设置丢包重传器接收RTP包的地址(与MCU接收该终端的RTP包地址一致)
                tLocalAddr = tPrsParam2.GetLocalAddr();
                tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnl2);
                tPrsParam2.SetLocalAddr(tLocalAddr);
            }

            //音频
            if (m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tMtLogicalChannelAud, FALSE))
            {
                //设置终端接收RTCP回馈包的地址
                tDstAddr = tMtLogicalChannelAud.GetSndMediaCtrlChannel();
                tPrsParamAud.SetRemoteAddr(tDstAddr);

                //设置丢包重传器接收RTP包的地址(与MCU接收该终端的RTP包地址一致)
                tLocalAddr = tPrsParamAud.GetLocalAddr();
                tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlAud);
                tPrsParamAud.SetLocalAddr(tLocalAddr);                    
            }              
        }
        else
        {
            //设置终端接收RTCP回馈包的地址
            u32 dwEqpIP,dwRcvIp;
            u16 wRcvPort;
            g_cMpManager.GetSwitchInfo(tMt, dwRcvIp, wRcvPort, dwEqpIP);
            tDstAddr.SetIpAddr(dwEqpIP);            
            if (tMt.GetEqpType() == EQP_TYPE_VMP)
            {               
                TEqpVMPInfo tVmpInfo;
                g_cMcuAgent.GetEqpVMPCfg( tMt.GetEqpId(), &tVmpInfo);
                    
                tDstAddr.SetPort(tVmpInfo.GetEqpRecvPort() - PORTSPAN + 1);
            }
            else
            {
                tDstAddr.SetPort(wRcvPort + 1);
            }            
            tPrsParam.SetRemoteAddr(tDstAddr);

            //设置丢包重传器接收RTP包的地址(与MCU接收该终端的RTP包地址一致)
            tLocalAddr = tPrsParam.GetLocalAddr();
            tLocalAddr.SetPort(tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnl);
            tPrsParam.SetLocalAddr(tLocalAddr);			
        }
    }
    else
    {
        memset(&tPrsParam, 0, sizeof(tPrsParam));
        memset(&tPrsParam2, 0, sizeof(tPrsParam2));
        memset(&tPrsParamAud, 0, sizeof(tPrsParamAud));
    }
	
    //给丢包重传器发消息
    if ((PRSCHANMODE_FIRST == byPrsChanMode || PRSCHANMODE_BOTH == byPrsChanMode) &&
        (m_byPrsChnnl != EQP_CHANNO_INVALID))
    {
        if(tMt.GetType() == TYPE_MT)
        {
            cServMsg.SetSrcMtId(tMt.GetMtId());
        }
        else
        {
            cServMsg.SetSrcMtId(0);
        }
        cServMsg.SetChnIndex(m_byPrsChnnl);
        cServMsg.SetMsgBody((u8*)&tPrsParam, sizeof(tPrsParam));
        SendMsgToEqp(m_tPrsEqp.GetEqpId() , MCU_PRS_SETSRC_REQ, cServMsg);
        m_tConfInStatus.SetPrsChnl1Start(TRUE);
        EqpLog("[ChangePrsSrc] Msg MCU_PRS_SETSRC_REQ send to prs eqp %d chl %d!\n", m_tPrsEqp.GetEqpId(), m_byPrsChnnl);
    }

    //给丢包重传器发消息
    if ((PRSCHANMODE_SECOND == byPrsChanMode || PRSCHANMODE_BOTH == byPrsChanMode) &&
        (m_byPrsChnnl2 != EQP_CHANNO_INVALID))
    {
        //第二路视频
        if (TYPE_MT == tMt.GetType())
        {
            if (m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_SECVIDEO, &tMtLogicalChannel2, FALSE))
            {
                //给丢包重传器发消息
                cServMsg.SetSrcMtId(tMt.GetMtId());
                cServMsg.SetChnIndex(m_byPrsChnnl2);
                cServMsg.SetMsgBody((u8*)&tPrsParam2, sizeof(tPrsParam2));
                SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_SETSRC_REQ, cServMsg);
                m_tConfInStatus.SetPrsChnl2Start(TRUE);
            }
        }
    }

    //给丢包重传器发消息
    if ((PRSCHANMODE_AUDIO == byPrsChanMode) && EQP_CHANNO_INVALID != m_byPrsChnnlAud)
    {
        //音频
        if (TYPE_MT == tMt.GetType())
        {
            if (m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tMtLogicalChannelAud, FALSE))
            {
                //给丢包重传器发消息
                cServMsg.SetSrcMtId(tMt.GetMtId());
                cServMsg.SetChnIndex(m_byPrsChnnlAud);
                cServMsg.SetMsgBody((u8*)&tPrsParamAud, sizeof(tPrsParamAud));
                SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_SETSRC_REQ, cServMsg);
                m_tConfInStatus.SetPrsChnlAudStart(TRUE);
            }
        }
    }    
}
/*====================================================================
    函数名      ：AddRemovePrsMember
    功能        ：增加移除丢包重传的成员(MT.RTCP -> PRS)
    算法实现    ：
    引用全局变量：
    输入参数说明
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/07/17    4.0         付秀华        创建
====================================================================*/
void CMcuVcInst::AddRemovePrsMember(u8 byMemId, u8 byPrsId, u8 byPrsChl, u8 byPrsMode, u8 bAdd)
{
	u32 dwSrcRtcpIP, dwPrsRtcpIP, dwMapIpAddr = 0;
	u16 wSrcRtcpPort, wPrsRtcpPort, wMapPort  = 0;

	u8 byChlNum = 0;
	TLogicalChannel tPrsLogChnl;
	if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byPrsId, MODE_VIDEO, &byChlNum, &tPrsLogChnl, TRUE))
	{
		dwPrsRtcpIP  = tPrsLogChnl.GetRcvMediaChannel().GetIpAddr();
		wPrsRtcpPort = tPrsLogChnl.GetRcvMediaChannel().GetPort() + PORTSPAN*byPrsChl + 2;
	}
	else
	{
		EqpLog("[AddRemovePrsMember] Fail to find prs(%d) logchnl\n", byPrsId);
		return;
	}

	TLogicalChannel tLogChnl;
	u8  byFindMemRtcpChnl = FALSE;
	if (MODE_VIDEO == byPrsMode &&
		m_ptMtTable->GetMtLogicChnnl(byMemId, LOGCHL_VIDEO, &tLogChnl, TRUE))
	{
		byFindMemRtcpChnl = TRUE;
	}
	else if (MODE_SECVIDEO == byPrsMode &&
		     m_ptMtTable->GetMtLogicChnnl(byMemId, LOGCHL_SECVIDEO, &tLogChnl, TRUE))
	{
		byFindMemRtcpChnl = TRUE;
	}
	else if (MODE_AUDIO == byPrsMode &&
		     m_ptMtTable->GetMtLogicChnnl(byMemId, LOGCHL_AUDIO, &tLogChnl, TRUE))
	{
		byFindMemRtcpChnl = TRUE;
	}

	if (byFindMemRtcpChnl)
	{
		dwSrcRtcpIP  = tLogChnl.GetSndMediaCtrlChannel().GetIpAddr();
		wSrcRtcpPort = tLogChnl.GetSndMediaCtrlChannel().GetPort();
        dwMapIpAddr  = tLogChnl.GetRcvMediaChannel().GetIpAddr();
        wMapPort     = tLogChnl.GetRcvMediaChannel().GetPort();
	}
	else
	{
		EqpLog("[AddRemovePrsMember] Fail to find the logchnl(%d) for mt(%d)\n",
			    byPrsMode, byMemId);
		return;
	}

	// 移除原交换
    g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwSrcRtcpIP, wSrcRtcpPort, dwPrsRtcpIP, wPrsRtcpPort);

	// 按源先清空已有多对一交换交换

    if (bAdd)
    {
        g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwSrcRtcpIP, 0, dwSrcRtcpIP,wSrcRtcpPort, 
					                     dwPrsRtcpIP, wPrsRtcpPort, 0, 0, dwMapIpAddr, wMapPort);
		EqpLog("[AddRemovePrsMember](%s, %d)--->", 
			   StrOfIP(dwSrcRtcpIP), wSrcRtcpPort);
		EqpLog("(%s, %d)\n", StrOfIP(dwPrsRtcpIP), wPrsRtcpPort);
    }

}
/*====================================================================
    函数名      ：AddRemovePrsMember
    功能        ：增加移除丢包重传的成员(MT.RTCP -> PRS)
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt tMt 广播源终端
				  BOOL32 bAdd TRUE-增加 FALSE-移除
				  BOOL32 bOnly2 是否为第二路
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/03/27    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::AddRemovePrsMember( TMt tMt, BOOL32 bAdd, u8 byPrsChanMode )
{
    //是否有预留资源
    if (m_tPrsEqp.IsNull() || tMt.IsNull()/* || !m_tConf.m_tStatus.IsPrsing()*/)
    {
        return;
    }

    u8  byChlNum  = 0;
    u16 wRecvPort = 0;
    u16 wMapPort  = 0;
    u16 wDstPort  = 0;
    u32 dwSrcIpAddr  = 0;
    u32 dwRecvIpAddr = 0;
    u32 dwDstIpAddr  = 0;
    TLogicalChannel tMtLogicalChannel;
    TLogicalChannel tMtLogicalChannel2;
    TLogicalChannel tMtLogicalChannelAud;
    TLogicalChannel tEqpLogicalChannel;

    //仅对科达终端有效
    u8 byManuID = m_ptMtTable->GetManuId(tMt.GetMtId());
    if (byManuID != MT_MANU_KDC && byManuID != MT_MANU_KDCMCU)
    {
        return;
    }

    //对于PRS而言，一个PRS可支持多个音视频通道(默认总共16个),
    //各通道接收端口统一为PRS起始接收端口，以通道号区分实际的接收端口
    BOOL32 bPrsChan = g_cMcuVcApp.GetPeriEqpLogicChnnl(m_tPrsEqp.GetEqpId(), MODE_VIDEO, &byChlNum, &tEqpLogicalChannel, TRUE);

    if (PRSCHANMODE_FIRST == byPrsChanMode || PRSCHANMODE_BOTH == byPrsChanMode)
    {
        if (m_tConfInStatus.IsPrsChnl1Start() && bPrsChan &&
            m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, TRUE))
        {
            dwSrcIpAddr  = tMtLogicalChannel.GetRcvMediaChannel().GetIpAddr();      //终端的rtp接收地址与端口
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //转发板rtcp接收地址与端口
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs的rtcp接收地址与端口
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnl+2; //此端口分配与设置prs源时传入的端口有关（prs模块中:传入端口＋2）

            if (bAdd)
            {
                //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

    if (PRSCHANMODE_SECOND == byPrsChanMode || PRSCHANMODE_BOTH == byPrsChanMode)
    {
        //第二路视频
        if (m_tConfInStatus.IsPrsChnl2Start() && bPrsChan && 
            m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_SECVIDEO, &tMtLogicalChannel2, TRUE))
        {
            dwSrcIpAddr  = tMtLogicalChannel2.GetRcvMediaChannel().GetIpAddr();
            wMapPort     = tMtLogicalChannel2.GetRcvMediaChannel().GetPort();
            dwRecvIpAddr = tMtLogicalChannel2.GetSndMediaCtrlChannel().GetIpAddr();
            wRecvPort    = tMtLogicalChannel2.GetSndMediaCtrlChannel().GetPort();            
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnl2+2;

            if (bAdd)
            {
                //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
                g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwSrcIpAddr, 0, dwRecvIpAddr,wRecvPort, 
					                             dwDstIpAddr, wDstPort, 0, 0, dwSrcIpAddr, wMapPort);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

    if (PRSCHANMODE_AUDIO == byPrsChanMode)
    {
        //音频
        if (m_tConfInStatus.IsPrsChnlAudStart() && bPrsChan && 
            m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tMtLogicalChannelAud, TRUE))
        {
            dwSrcIpAddr  = tMtLogicalChannelAud.GetRcvMediaChannel().GetIpAddr();
            wMapPort     = tMtLogicalChannelAud.GetRcvMediaChannel().GetPort();
            dwRecvIpAddr = tMtLogicalChannelAud.GetSndMediaCtrlChannel().GetIpAddr();
            wRecvPort    = tMtLogicalChannelAud.GetSndMediaCtrlChannel().GetPort();            
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlAud+2;

            if (bAdd)
            {
                //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
                g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwSrcIpAddr, 0, dwRecvIpAddr,wRecvPort, 
					                             dwDstIpAddr, wDstPort, 0, 0, dwSrcIpAddr, wMapPort);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

    if (PRSCHANMODE_VIDBAS == byPrsChanMode)
    {
        if (m_tConfInStatus.IsPrsChnlVidBasStart() && bPrsChan &&
            m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, TRUE))
        {
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //转发板rtcp接收地址与端口
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs的rtcp接收地址与端口
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVidBas+2; //此端口分配与设置prs源时传入的端口有关（prs模块中:传入端口＋2）
            
            if (bAdd)
            {
                //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

    //vmp.2
    if (PRSCHANMODE_BRBAS == byPrsChanMode || PRSCHANMODE_VMP2 == byPrsChanMode)
    {
        if (m_tConfInStatus.IsPrsChnlBrBasStart() && bPrsChan &&
            m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, TRUE))
        {            
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //转发板rtcp接收地址与端口
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs的rtcp接收地址与端口
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlBrBas+2; //此端口分配与设置prs源时传入的端口有关（prs模块中:传入端口＋2）
            
            if (bAdd)
            {
                //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

    if (PRSCHANMODE_AUDBAS == byPrsChanMode)
    {
        if (m_tConfInStatus.IsPrsChnlAudBasStart() && bPrsChan &&
            m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tMtLogicalChannel, TRUE))
        {            
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //转发板rtcp接收地址与端口
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs的rtcp接收地址与端口
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlAudBas+2; //此端口分配与设置prs源时传入的端口有关（prs模块中:传入端口＋2）
            
            if (bAdd)
            {
                //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_AUDIO, SWITCH_MODE_BROADCAST, TRUE);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

    // FIXME：高清适配丢包重传未添加,zgc

	// xliang [4/30/2009] 多输出类的广播源丢包重传
	if (PRSCHANMODE_VMPOUT1 == byPrsChanMode)
    {
        if ( m_tConfInStatus.IsPrsChnlVmpOut1Start() 
			&& bPrsChan 
			&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, TRUE))
        {            
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //转发板rtcp接收地址与端口
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs的rtcp接收地址与端口
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut1+2; //此端口分配与设置prs源时传入的端口有关（prs模块中:传入端口＋2）
            
            if (bAdd)
            {
				EqpLog("begin to RTCP switch Mt.%u to PRS(Mode:%u, ip:%d, port:%d)!\n",tMt.GetMtId(), byPrsChanMode, dwDstIpAddr, wDstPort );
                //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);

            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

	if (PRSCHANMODE_VMPOUT2 == byPrsChanMode)
    {
        if ( m_tConfInStatus.IsPrsChnlVmpOut2Start() 
			&& bPrsChan 
			&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, TRUE))
        {            
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //转发板rtcp接收地址与端口
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs的rtcp接收地址与端口
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut2+2; //此端口分配与设置prs源时传入的端口有关（prs模块中:传入端口＋2）
            
            if (bAdd)
            {
				EqpLog("begin to RTCP switch Mt.%u to PRS(Mode:%u, ip:%d, port:%d)!\n",tMt.GetMtId(), byPrsChanMode, dwDstIpAddr, wDstPort );
                //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

	if (PRSCHANMODE_VMPOUT3 == byPrsChanMode)
    {
        if ( m_tConfInStatus.IsPrsChnlVmpOut3Start() 
			&& bPrsChan 
			&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, TRUE))
        {            
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //转发板rtcp接收地址与端口
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs的rtcp接收地址与端口
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut3+2; //此端口分配与设置prs源时传入的端口有关（prs模块中:传入端口＋2）
            
            if (bAdd)
            {
				EqpLog("begin to RTCP switch Mt.%u to PRS(Mode:%u, ip:%d, port:%d)!\n",tMt.GetMtId(), byPrsChanMode, dwDstIpAddr, wDstPort );
                //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }
	if (PRSCHANMODE_VMPOUT4 == byPrsChanMode)
    {
        if ( m_tConfInStatus.IsPrsChnlVmpOut4Start() 
			&& bPrsChan 
			&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicalChannel, TRUE))
        {            
            dwRecvIpAddr = tMtLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();  //转发板rtcp接收地址与端口
            wRecvPort    = tMtLogicalChannel.GetSndMediaCtrlChannel().GetPort();
            dwDstIpAddr  = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();     //prs的rtcp接收地址与端口
            wDstPort     = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnlVmpOut4+2; //此端口分配与设置prs源时传入的端口有关（prs模块中:传入端口＋2）
            
            if (bAdd)
            {
				EqpLog("begin to RTCP switch Mt.%u to PRS(Mode:%u, ip:%d, port:%d)!\n",tMt.GetMtId(), byPrsChanMode, dwDstIpAddr, wDstPort );
                //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
                SwitchVideoRtcpToDst(dwDstIpAddr, wDstPort, tMt, 0, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
            }
            else
            {
                g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIpAddr, wDstPort);
            }
        }
    }

    return;
}
/*====================================================================
    函数名      ：ReRtcpSwithForPrs
    功能        ：更新到PRS的RTCP交换
    算法实现    ：
    引用全局变量：
    输入参数说明
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/07/17    4.0         付秀华        创建
====================================================================*/
void CMcuVcInst::BuildRtcpAndRtpSwithForPrs(u8 byPrsId, u8 byPrsChl)
{
	TMt tPrsSrc;
	u8  byPrsSrcOutChnl = 0;
	u8  byPrsMode       = MODE_NONE;
	if (m_tConfPrsInfo.FindPrsChnlSrc(byPrsId, byPrsChl, byPrsMode, tPrsSrc, byPrsSrcOutChnl))
	{
		StartSwitchToPeriEqp(tPrsSrc, byPrsSrcOutChnl, 
								 byPrsId, byPrsChl, byPrsMode, SWITCH_MODE_SELECT);

		TMt tRealSrc;
		u8  byRealSrcOChnl = 0;
		for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{                
			if(m_tConfAllMtInfo.MtJoinedConf( byLoop ) && 
			   GetMtRealSrc(byLoop, byPrsMode, tRealSrc, byRealSrcOChnl) &&
			   tRealSrc == tPrsSrc && byRealSrcOChnl == byPrsSrcOutChnl)
			{
				AddRemovePrsMember( byLoop, byPrsId, byPrsChl, byPrsMode, TRUE);
			}
// 			else
// 			{
// 				AddRemovePrsMember( byLoop, byPrsId, byPrsChl, byPrsMode, FALSE);
// 			}
		}
	}
}
/*====================================================================
    函数名      ：StopPrs
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/07/17    4.0         付秀华        创建
====================================================================*/
void CMcuVcInst::StopPrs(u8 byPrsId, u8 byPrsChnl)
{
	CServMsg cServMsg;
    cServMsg.SetChnIndex(byPrsChnl);
    SendMsgToEqp(byPrsId, MCU_PRS_REMOVEALL_REQ, cServMsg);
     

    StopSwitchToPeriEqp(byPrsId, byPrsChnl, FALSE, MODE_VIDEO, SWITCH_MODE_SELECT);

	u32 dwPrsRtcpIP  = 0;
	u16 wPrsRtcpPort = 0;
	u8 byChlNum = 0;
	TLogicalChannel tPrsLogChnl;
	if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byPrsId, MODE_VIDEO,
		                                 &byChlNum, &tPrsLogChnl, TRUE))
	{
        dwPrsRtcpIP  = tPrsLogChnl.GetRcvMediaChannel().GetIpAddr();
        wPrsRtcpPort = tPrsLogChnl.GetRcvMediaChannel().GetPort() + PORTSPAN * byPrsChnl + 2;
        g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwPrsRtcpIP, wPrsRtcpPort);
        EqpLog("[StopPrs] Stop rtcp switch to (ip:%d port:%d) for prs(eqpid:%d, chnlid:%d)\n", 
			   StrOfIP(dwPrsRtcpIP), wPrsRtcpPort, byPrsId, byPrsChnl);

	}

}
/*====================================================================
    函数名      ：StopPrs
    功能        ：停止丢包重传
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/03/27    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::StopPrs(u8 byPrsChanMode, BOOL32 bNotify)
{
    u8  byChlNum;
    u16 wDstPort;
    u32 dwDstIpAddr;
    TLogicalChannel tEqpLogicalChannel;

    //是否有预留资源
    if (m_tPrsEqp.IsNull() || !m_tConf.m_tStatus.IsPrsing())
    {
        return;
    }

    //给丢包重传器发消息
    CServMsg cServMsg;
    TPeriEqpStatus tStatus;
    BOOL32 bLogicalChan = g_cMcuVcApp.GetPeriEqpLogicChnnl(m_tPrsEqp.GetEqpId(), MODE_VIDEO, &byChlNum, &tEqpLogicalChannel, TRUE);
    g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );

    //第一路视频
    if ((byPrsChanMode == PRSCHANMODE_FIRST || byPrsChanMode == PRSCHANMODE_BOTH) &&
        m_tConfInStatus.IsPrsChnl1Start() && m_byPrsChnnl != EQP_CHANNO_INVALID)
    {		
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnl);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }        

        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnl, FALSE, MODE_VIDEO);

        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnl+2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlVid] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnl1Start(FALSE);
    }

    //第二路视频
    if ((byPrsChanMode == PRSCHANMODE_SECOND || byPrsChanMode == PRSCHANMODE_BOTH ) &&
        m_tConfInStatus.IsPrsChnl2Start() && m_byPrsChnnl2 != EQP_CHANNO_INVALID)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnl2);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }

        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnl2, FALSE, MODE_VIDEO);

        if (bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN*m_byPrsChnnl2+2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlVid2] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnl2Start(FALSE);
    }

    //音频
    if (byPrsChanMode == PRSCHANMODE_AUDIO && 
        m_tConfInStatus.IsPrsChnlAudStart() && EQP_CHANNO_INVALID != m_byPrsChnnlAud)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlAud);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }

        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlAud, FALSE, MODE_AUDIO);

        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlAud + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlAud] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlAudStart(FALSE);
    }

    //需适配的音频重传
    if (PRSCHANMODE_AUDBAS == byPrsChanMode && 
        m_tConfInStatus.IsPrsChnlAudBasStart() && EQP_CHANNO_INVALID != m_byPrsChnnlAudBas)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlAudBas);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }
        
        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlAudBas, FALSE, MODE_AUDIO);
        
        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlAudBas + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlAudBas] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlAudBasStart(FALSE);
    }

    //需媒体适配的视频重传
    if (PRSCHANMODE_VIDBAS == byPrsChanMode && 
        m_tConfInStatus.IsPrsChnlVidBasStart() && EQP_CHANNO_INVALID != m_byPrsChnnlVidBas)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlVidBas);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }
        
        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlVidBas, FALSE, MODE_VIDEO);
        
        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlVidBas + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlVidBas] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlVidBasStart(FALSE);
    }

    //需码率适配的视频重传 or vmp.2
    if ((PRSCHANMODE_BRBAS == byPrsChanMode || PRSCHANMODE_VMP2 == byPrsChanMode) &&
        m_tConfInStatus.IsPrsChnlBrBasStart() && EQP_CHANNO_INVALID != m_byPrsChnnlBrBas)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlBrBas);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }
        
        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlBrBas, FALSE, MODE_VIDEO);
        
        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlBrBas + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlBrBas] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlBrBasStart(FALSE);
    }

    // FIXME：高清适配丢包重传未添加,zgc
	
	// xliang [4/30/2009] 多路输出类广播源视频重传
    if (PRSCHANMODE_VMPOUT1 == byPrsChanMode 
		&& m_tConfInStatus.IsPrsChnlVmpOut1Start()  
		&& EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut1
		)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlVmpOut1);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }
        
        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut1, FALSE, MODE_VIDEO);
        
        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlVmpOut1 + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlBrd1080] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlVmpOut1Start(FALSE);
    }

	if (PRSCHANMODE_VMPOUT2 == byPrsChanMode 
		&& m_tConfInStatus.IsPrsChnlVmpOut2Start()  
		&& EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut2
		)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlVmpOut2);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }
        
        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut2, FALSE, MODE_VIDEO);
        
        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlVmpOut2 + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlBrd720] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlVmpOut2Start(FALSE);
    }

	if (PRSCHANMODE_VMPOUT3 == byPrsChanMode 
		&& m_tConfInStatus.IsPrsChnlVmpOut3Start()  
		&& EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut3
		)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlVmpOut3);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }
        
        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut3, FALSE, MODE_VIDEO);
        
        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlVmpOut3 + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlBrd4Cif] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlVmpOut3Start(FALSE);
    }

	if (PRSCHANMODE_VMPOUT4 == byPrsChanMode 
		&& m_tConfInStatus.IsPrsChnlVmpOut4Start()  
		&& EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut4
		)
    {
        if(bNotify)
        {
            cServMsg.SetChnIndex(m_byPrsChnnlVmpOut4);
            cServMsg.SetMsgBody();
            SendMsgToEqp(m_tPrsEqp.GetEqpId(), MCU_PRS_REMOVEALL_REQ, cServMsg);
        }
        
        StopSwitchToPeriEqp(m_tPrsEqp.GetEqpId(), m_byPrsChnnlVmpOut4, FALSE, MODE_VIDEO);
        
        if(bLogicalChan)
        {
            dwDstIpAddr = tEqpLogicalChannel.GetRcvMediaChannel().GetIpAddr();
            wDstPort = tEqpLogicalChannel.GetRcvMediaChannel().GetPort() + PORTSPAN * m_byPrsChnnlVmpOut4 + 2;
            g_cMpManager.StopMultiToOneSwitch(m_byConfIdx, dwDstIpAddr, wDstPort);
            EqpLog("[StopPrs-PrsChnnlBrdCifOrOther] dwDstIpAddr = %d, wDstPort = %d\n", dwDstIpAddr, wDstPort);
        }
        m_tConfInStatus.SetPrsChnlVmpOut4Start(FALSE);
    }

    g_cMcuVcApp.SetPeriEqpStatus(m_tPrsEqp.GetEqpId(), &tStatus);

    
	if ( (IsHDConf(m_tConf) && !m_tConfInStatus.IsHDPrsChnlAnyStart() )
		|| ( !IsHDConf(m_tConf) && !m_tConfInStatus.IsPrsChnlAnyStart() )
		)
    {
        m_tConf.m_tStatus.SetPrsing(FALSE);
    }

    //移除所有数据交换

}

/*====================================================================
    函数名      ：ProcPrsMcuRsp
    功能        ：丢包重传器应答处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/03/27    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcPrsConnectedNotif(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TPeriEqpRegReq tEqp = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();

    u8 byEqpId;
    u8 byLoop;
    TPeriEqpStatus tStatus;

	switch(CurState())
	{
	case STATE_ONGOING:

		//清空状态
		g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tStatus);
		tStatus.SetConfIdx(m_byConfIdx);
		for(byLoop=0; byLoop<MAXNUM_PRS_CHNNL; byLoop++)
		{
			tStatus.m_tStatus.tPrs.m_tPerChStatus[byLoop].SetReserved(FALSE);
		}
		g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tStatus);
		
		//开始根据需要适配
		if (m_tConf.GetConfAttrb().IsResendLosePack() && !m_tConf.m_tStatus.IsPrsing())
		{
			// if (g_cMcuVcApp.GetIdlePrsChl(byEqpId, byChnIdx, byChnIdx2, byChnIdxAud))
			TPrsChannel tPrsChannel;
			if( g_cMcuVcApp.GetIdlePrsChls( DEFAULT_PRS_CHANNELS, tPrsChannel) ) // 
			{
				byEqpId = tPrsChannel.GetPrsId();
				m_tPrsEqp.SetMcuEqp(LOCAL_MCUID, tPrsChannel.GetPrsId(), EQP_TYPE_PRS);
				m_tPrsEqp.SetConfIdx(m_byConfIdx);
				u8 byChnIdx = 0;
				m_byPrsChnnl  = tPrsChannel.m_abyPrsChannels[byChnIdx++];
				m_byPrsChnnl2 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
				m_byPrsChnnlAud = tPrsChannel.m_abyPrsChannels[byChnIdx];

				g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
				//目前一个PRS可以为多个会议服务，因此修改为在每个PRS通道中记录该通道服务的会议索引
				//zgc, 2007/04/24
				tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnl, m_byConfIdx );
				tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnl2, m_byConfIdx );
				tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlAud, m_byConfIdx );

				tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl].SetReserved(TRUE);
				tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl2].SetReserved(TRUE);
				tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAud].SetReserved(TRUE);
				g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
				
				if (!m_tVidBrdSrc.IsNull())
				{
					ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_FIRST);
				}
				if (!m_tDoubleStreamSrc.IsNull())
				{
					ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_SECOND);					
				}

                if (!m_tAudBrdSrc.IsNull())
				{
					ChangePrsSrc(m_tAudBrdSrc, PRSCHANMODE_AUDIO);
				}

				ConfLog(FALSE, "Idle prs channel %d %d %d get success!\n",
                                m_byPrsChnnl, m_byPrsChnnl2, m_byPrsChnnlAud);
				
			}
		}
				
		break;

	default:
		ConfLog(FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState());
		break;
	}
}


/*====================================================================
    函数名      ：ProcPrsMcuRsp
    功能        ：丢包重传器应答处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/03/27    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcPrsDisconnectedNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		if( m_tConf.m_tStatus.IsPrsing() )
		{
			StopPrs( PRSCHANMODE_BOTH );
			StopPrs(PRSCHANMODE_AUDIO);
            StopPrs(PRSCHANMODE_VIDBAS);
            StopPrs(PRSCHANMODE_AUDBAS);
            StopPrs(PRSCHANMODE_BRBAS);
            //zbq [07/18/2007]
            StopPrs(PRSCHANMODE_VMP2);
			m_tConfInStatus.SetPrsChnlAllStop();
			
			// xliang [5/6/2009] 
			StopPrs(PRSCHANMODE_VMPOUT1);
			StopPrs(PRSCHANMODE_VMPOUT2);
			StopPrs(PRSCHANMODE_VMPOUT3);
			StopPrs(PRSCHANMODE_VMPOUT4);
			m_tConfInStatus.SetHDPrsChnlAllStop();
            
			m_tConf.m_tStatus.SetPrsing( FALSE );

            // FIXME：高清适配丢包重传未添加,zgc
		}

		if( m_tConf.GetConfAttrb().IsResendLosePack() )
		{
			TPeriEqpStatus tStatus;
			g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl].SetReserved( FALSE );
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl2].SetReserved( FALSE );
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAud].SetReserved( FALSE );
            if(EQP_CHANNO_INVALID != m_byPrsChnnlAudBas && m_byPrsChnnlAudBas < MAXNUM_PRS_CHNNL)
            {
                tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAudBas].SetReserved(FALSE);
            }
            if(EQP_CHANNO_INVALID != m_byPrsChnnlVidBas && m_byPrsChnnlVidBas < MAXNUM_PRS_CHNNL)
            {
                tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVidBas].SetReserved(FALSE);
            }
            if(EQP_CHANNO_INVALID != m_byPrsChnnlBrBas && m_byPrsChnnlBrBas < MAXNUM_PRS_CHNNL)
            {
                tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlBrBas].SetReserved(FALSE);
            }
			// xliang [4/30/2009] 释放广播源4出对应的PRS通道
			if(EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut1 && m_byPrsChnnlVmpOut1 < MAXNUM_PRS_CHNNL)
			{
				tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut1].SetReserved(FALSE);
			}
			if(EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut2 && m_byPrsChnnlVmpOut2 < MAXNUM_PRS_CHNNL)
			{
				tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut2].SetReserved(FALSE);
			}
			if(EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut3 && m_byPrsChnnlVmpOut3 < MAXNUM_PRS_CHNNL)
			{
				tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut3].SetReserved(FALSE);
			}
			if(EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut4 && m_byPrsChnnlVmpOut4 < MAXNUM_PRS_CHNNL)
			{
				tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut4].SetReserved(FALSE);
			}
			g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
		}
		
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcPrsMcuRsp
    功能        ：丢包重传器应答处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/03/27    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcPrsMcuRsp( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TLogicalChannel tMtLogicalChannel, tMtLogicalChannel2, tEqpLogicalChannel;
	u8  byMtId, byLoop;
	TMt tMt;
	u8  byPrsChl = cServMsg.GetChnIndex();
    u8  byAdpType;

	switch( pcMsg->event ) 
	{
	case PRS_MCU_SETSRC_ACK:			//PRS给MCU保存信息源确认
		{
			u8  byPrsId  = *cServMsg.GetMsgBody();

			if( !m_tConf.m_tStatus.IsPrsing() )
			{
				m_tConf.m_tStatus.SetPrsing( TRUE );
			}
			//改变会议状态
			if( byPrsChl == m_byPrsChnnl )
			{				
				for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
				{                
					if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) &&
						!IsMtSrcBas(byLoop, MODE_VIDEO, byAdpType) &&
						!IsMtSrcVmp2(byLoop))
					{
						tMt = m_ptMtTable->GetMt( byLoop );
						AddRemovePrsMember( tMt, TRUE, PRSCHANMODE_FIRST);
					}
				}
			}
			if( byPrsChl == m_byPrsChnnl2 )
			{
				for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
				{
					if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
					{
						tMt = m_ptMtTable->GetMt( byLoop );
						AddRemovePrsMember( tMt, TRUE, PRSCHANMODE_SECOND);
					}
				}
			}
			if (byPrsChl == m_byPrsChnnlAud)
			{
				for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
				{
					if (m_tConfAllMtInfo.MtJoinedConf(byLoop) &&
						!IsMtSrcBas(byLoop, MODE_AUDIO, byAdpType))
					{
						tMt = m_ptMtTable->GetMt(byLoop);
						AddRemovePrsMember(tMt, TRUE, PRSCHANMODE_AUDIO);
					}
				}
			}
			
			byMtId = cServMsg.GetSrcMtId();
			if (byMtId > 0)
			{
				tMt = m_ptMtTable->GetMt(byMtId);
			}
			
			//建立交换
			if (!m_tVidBrdSrc.IsNull())
			{
				//源与PRS: 只交换源到PRS的RTP包,PRS给源的反馈RTCP包暂不交换
				if (byPrsChl == m_byPrsChnnl)
				{
					StartSwitchToPeriEqp( m_tVidBrdSrc, 0, m_tPrsEqp.GetEqpId(), m_byPrsChnnl, MODE_VIDEO );
				}
			}
			if (!m_tDoubleStreamSrc.IsNull())
			{
				if (byMtId > 0 && m_tDoubleStreamSrc.GetMtId() == byMtId)
				{
					if( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tMtLogicalChannel2, FALSE ) )
					{
						//StartSwitchToPeriEqp( m_tDoubleStreamSrc, 1, m_tPrsEqp.GetEqpId(), m_byPrsChnnl2, MODE_VIDEO );
						StartSwitchToPeriEqp(m_tDoubleStreamSrc, 0, m_tPrsEqp.GetEqpId(), m_byPrsChnnl2, MODE_SECVIDEO);
					}
				}
			}
			
			if (!m_tAudBrdSrc.IsNull())
			{
				//源与PRS: 只交换源到PRS的RTP包,PRS给源的反馈RTCP包暂不交换
				if (byPrsChl == m_byPrsChnnlAud)
				{
					StartSwitchToPeriEqp(m_tAudBrdSrc, 0, m_tPrsEqp.GetEqpId(), m_byPrsChnnlAud, MODE_AUDIO);
				}
			}
			
			//vmp.2 第二路码流交换到prs
			if (0 != m_tConf.GetSecBitRate() && 
				MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType() &&
				m_tVidBrdSrc == m_tVmpEqp)
			{
				if (byPrsChl == m_byPrsChnnlBrBas)
				{
					StartSwitchToPeriEqp( m_tVidBrdSrc, 1, m_tPrsEqp.GetEqpId(), m_byPrsChnnlBrBas, MODE_VIDEO );
				}            
			}
			
			// xliang [4/30/2009] 处理VMP 4出
			u8 abyPrsChnnl[MAXNUM_MPU_OUTCHNNL] = {m_byPrsChnnlVmpOut1, m_byPrsChnnlVmpOut2, m_byPrsChnnlVmpOut3, m_byPrsChnnlVmpOut4};
			for( byLoop = 0; byLoop < MAXNUM_MPU_OUTCHNNL; byLoop++)
			{
				if(byPrsChl == abyPrsChnnl[byLoop])
				{
					StartSwitchToPeriEqp( m_tVidBrdSrc, 0+byLoop, m_tPrsEqp.GetEqpId(), abyPrsChnnl[byLoop], MODE_VIDEO );
					break;
				}
			}

			BuildRtcpAndRtpSwithForPrs(byPrsId, byPrsChl);

			break;
		}
	case PRS_MCU_REMOVEALL_ACK:			//PRS给MCU改变复合参数确认	
		break;

	case PRS_MCU_SETSRC_NACK:			//PRS给MCU保存信息源拒绝
//	case PRS_MCU_ADDRESENDCH_NACK:		//PRS给MCU停止工作拒绝
//	case PRS_MCU_REMOVERESENDCH_NACK:	//PRS给MCU改变复合参数拒绝
	case PRS_MCU_REMOVEALL_NACK:		//PRS给MCU改变复合参数拒绝
		break;

	default:
		break;
	}
}

/*====================================================================
    函数名      ：RestoreMtSelectStatus
    功能        ：选看状态的恢复
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId   : 要恢复的终端ID
                  u8 bySelMode: 选看模式 MODE_AUDIO, MODE_VIDEO
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/04/20    4.0         张宝卿          创建
====================================================================*/
void CMcuVcInst::RestoreMtSelectStatus( u8 byMtId, u8 bySelMode )
{
/*    TSwitchInfo tSwitchInfo;
    TMtStatus tMtStatus;
    
    //判断恢复原选看交换
    if ( m_ptMtTable->GetMtStatus(byMtId, &tMtStatus) )
    {        
        TMt tMtSelect;
        tMtSelect = tMtStatus.GetSelectMt(bySelMode);

        //选看信息非空
        if ( !tMtSelect.IsNull() )
        {
            //被选看终端在线，把交换恢复到选看模式
            if (m_tConfAllMtInfo.MtJoinedConf(tMtSelect.GetMtId()))
            {
                tSwitchInfo.SetSrcMt( tMtSelect );
                tSwitchInfo.SetDstMt( m_ptMtTable->GetMt(byMtId) );
                tSwitchInfo.SetMode( bySelMode );
                
                CServMsg cServMsg;
                
                //这里将 EventId 设为 MCS_MCU_STARTSWITCHMT_REQ 没有实际意义，
                //  只是为了适应 SwitchSrcToDst() 中交换的正常开启，也可以设为
                //  MT_MCU_STARTSELMT_CMD 或 MCS_MCU_STARTSWITCHMT_REQ. 
                cServMsg.SetEventId( MCS_MCU_STARTSWITCHMT_REQ );
                
                SwitchSrcToDst( tSwitchInfo, cServMsg );
            }
            //被选看终端掉线，清空选看信息
            else
            {
                tMtSelect.SetNull();
                tMtStatus.SetSelectMt(tMtSelect, bySelMode);
                m_ptMtTable->SetMtStatus(byMtId, &tMtStatus);
            }
        }                        
    }*/

    return;
}
/*====================================================================
    函数名      ：ChangeMtVideoFormat
    功能        ：H264会议画面合成，分辨率的调整
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt&              tMt: 要调整的终端
                  TVMPParam* ptVMPParam: 调整后的分辨率
				  BOOL32         bStart: 调整或恢复
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/04/29    4.0         付秀华        创建
====================================================================*/
BOOL32 CMcuVcInst::ChangeMtVideoFormat( TMt tMt, TVMPParam * ptVMPParam, BOOL32 bStart/* = TRUE */,
									 BOOL32 bSeize/* = TRUE */, BOOL32 bConsiderVmpBrd /* = TRUE */)
{
	u8 byMtId = (tMt.IsLocal() ? tMt.GetMtId() : tMt.GetMcuId());

	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

	if(byVmpSubType == VMP)
	{
		ChgMtFormatInSdVmp(byMtId, ptVMPParam, bStart);
		return TRUE;
	}
	else
	{
		return ChgMtFormatInMpu(byMtId, ptVMPParam, bStart, bSeize, bConsiderVmpBrd);
	}
}


/*=============================================================================
函 数 名： IsRecordSrcBas
功    能： 获取会议录像真正的源，判断是否是适配器
算法实现： 缺省认为会议录像永远录主格式，通过比较广播源的能力集及录像的参数计算
全局变量： 
参    数： u8 byType        [In]  MODE_VIDEO, MODE_AUDIO
           TMt &tRetRecSrc  [Out]
           //u8  &byBasType   [Out] ADAPT_TYPE_VID, ADAPT_TYPE_BR 等/
           [IN/OUT]u8&   byRecChnlIdx:录像机实际接收通道 

返 回 值： BOOL32 是否从Bas录像
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
07/06/12    4.0         顾振华         重写
09/07/13    4.0         付秀华         兼容HD bas
=============================================================================*/
BOOL32 CMcuVcInst::IsRecordSrcBas(  u8 byType, TMt &tRetRecSrc, /*u8 &byBasType*/ u8 byRecChnlIdx)
{
    BOOL32 bNeedBas = FALSE;
    TSimCapSet tConfMainSCS = m_tConf.GetCapSupport().GetMainSimCapSet();    
    TSimCapSet tSrcMtSCS;
    TMt tRecSrc;

#define END_RET_VALUE( bVal ) \
    {   \
        bNeedBas = bVal; \
        goto lbl_EndIsRecordSrcBas;    \
    }

	byRecChnlIdx = 0;
    if ( m_tConf.m_tStatus.IsBrdstVMP() && byType == MODE_VIDEO)
    {
        // 画面合成广播不需要
        tRetRecSrc = m_tVmpEqp;
        END_RET_VALUE(FALSE);
    }
    if ( m_tConf.m_tStatus.IsMixing() && byType == MODE_AUDIO)
    {
        //在混音不需要
        tRetRecSrc = m_tMixEqp;
        END_RET_VALUE(FALSE);
    }
    if (byType == MODE_SECVIDEO)
    {
        //双流不需要
        tRetRecSrc = m_tDoubleStreamSrc;
        END_RET_VALUE(FALSE);
    }
    
    // 默认是会议广播源（可能是终端或者会议放像）
	// zgc, 2008-04-28, 会议视频轮询录象失败
    //tRecSrc = GetLocalSpeaker();
	if ( byType == MODE_VIDEO )
	{
		tRecSrc = m_tVidBrdSrc;
	}
	else if ( byType == MODE_AUDIO )
	{
		tRecSrc = m_tAudBrdSrc;
	}

    tRetRecSrc = tRecSrc;

    if ( !m_tConf.GetConfAttrb().IsUseAdapter() )
    {
        //没有启用适配（单速单格式）不需要
        END_RET_VALUE(FALSE);
    }

    if ( tRecSrc.GetType() == TYPE_MT )
    {
        // 终端
        tSrcMtSCS = m_ptMtTable->GetSrcSCS(tRecSrc.GetMtId());
    }
    else
    {
        // 放像器(放像器直接默认主格式、主码率)
        tSrcMtSCS.SetAudioMediaType( tConfMainSCS.GetAudioMediaType() );
        tSrcMtSCS.SetVideoMediaType( tConfMainSCS.GetVideoMediaType() );
        tSrcMtSCS.SetVideoResolution( tConfMainSCS.GetVideoResolution() );
    }   

    switch (byType)
    {
    case MODE_AUDIO:
        {
            if (MEDIA_TYPE_NULL == m_tConf.GetSecAudioMediaType()) 
            {
                END_RET_VALUE(FALSE);
            }
            
            if ( tConfMainSCS.GetAudioMediaType() != tSrcMtSCS.GetAudioMediaType() )
            {
                tRetRecSrc   = m_tAudBasEqp;
				byRecChnlIdx = m_byAudBasChnnl;
                END_RET_VALUE(TRUE);
            }
            else
            {
                END_RET_VALUE(FALSE);
            }
        }
        break;
    case MODE_VIDEO:
        {
			if (IsHDConf(m_tConf))
			{
				TMt tHdBas;
				u8  byInChnlIdx, byOutChnIdx;
				u8  byMediaType = tConfMainSCS.GetVideoMediaType();
				u8  byRes       = tConfMainSCS.GetVideoResolution();
				if (tConfMainSCS.GetVideoMediaType() != tSrcMtSCS.GetVideoMediaType() ||
					m_tRecPara.IsRecLowStream())
				{
					do 
					{
						if (m_cBasMgr.GetBasResource(byMediaType, byRes,
													 tHdBas, byInChnlIdx, byOutChnIdx))
						{
							tRetRecSrc   = tHdBas;
							byRecChnlIdx = byInChnlIdx * MAXNUM_VOUTPUT + byOutChnIdx;
							END_RET_VALUE(TRUE);
						}
					} while(GetProximalGrp(byMediaType, byRes, byMediaType, byRes));
				}

				END_RET_VALUE(FALSE);
			}
			else
			{
				// 单速
				if (m_tConf.GetSecBitRate() == 0)
				{
					if (tConfMainSCS.GetVideoMediaType() != tSrcMtSCS.GetVideoMediaType()   ||
						tConfMainSCS.GetVideoResolution() != tSrcMtSCS.GetVideoResolution())
					{
						tRetRecSrc   = m_tVidBasEqp;
						byRecChnlIdx = m_byVidBasChnnl; 
						END_RET_VALUE(TRUE);
					}    
					else
					{
						END_RET_VALUE(FALSE);                     
					}
				}
            
				// 双速单格式
				if (MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType()) 
				{
					if ( !m_tRecPara.IsRecLowStream() )
					{
						END_RET_VALUE(FALSE);
					}
					// zgc, 2008-01-30, 如果源的发送码率等于会议第二速率，则不需要适配
					else if ( tRecSrc.GetType() == TYPE_MT && m_ptMtTable->GetMtSndBitrate(tRecSrc.GetMtId()) == m_tConf.GetSecBitRate() )
					{
						END_RET_VALUE(FALSE);
					}
					else
					{
						tRetRecSrc   = m_tBrBasEqp;
						byRecChnlIdx = m_byBrBasChnnl; 
						END_RET_VALUE(TRUE);
					}
				}

				// 双速双格式
				if (tConfMainSCS.GetVideoMediaType() == tSrcMtSCS.GetVideoMediaType() &&
					tConfMainSCS.GetVideoResolution() == tSrcMtSCS.GetVideoResolution() &&
					!m_tRecPara.IsRecLowStream())
				{
					// 格式相同、且录高速，则不需要适配
					END_RET_VALUE(FALSE);
				}
				else
				{
					// 录高速，需要格式适配
					if (!m_tRecPara.IsRecLowStream())
					{
						tRetRecSrc   = m_tVidBasEqp;
						byRecChnlIdx = m_byVidBasChnnl;
						END_RET_VALUE(TRUE);
					}
					// 录低速，区分广播源适配的情况
					if ( tConfMainSCS.GetVideoMediaType() == tSrcMtSCS.GetVideoMediaType() &&
						 tConfMainSCS.GetVideoResolution() == tSrcMtSCS.GetVideoResolution() )
					{
						// zgc, 2008-01-30, 如果源的发送码率等于会议第二速率，则不需要适配
						if ( tRecSrc.GetType() == TYPE_MT && m_ptMtTable->GetMtSndBitrate(tRecSrc.GetMtId()) == m_tConf.GetSecBitRate() )
						{
							END_RET_VALUE(FALSE);
						}
						else
						{
							tRetRecSrc   = m_tBrBasEqp;
							byRecChnlIdx = m_byBrBasChnnl;                   
						}
					}
					else
					{
						tRetRecSrc   = m_tVidBasEqp;
						byRecChnlIdx = m_byVidBasChnnl;
					}

					END_RET_VALUE(TRUE);
				}
			}
		}
        break;
    default:
        break;
    }

lbl_EndIsRecordSrcBas:

    EqpLog("[IsRecordSrcBas] byType.%d IsNeed.%d, RecSrc.(Type.%d, Id.%d), RecChnlIdx.%d\n",
           byType, bNeedBas, tRetRecSrc.GetType(), tRetRecSrc.GetEqpId(), byRecChnlIdx);

    return bNeedBas;

#undef END_RET_VALUE
}

/*=============================================================================
函 数 名： IsRecordSrcHDBas
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/8/27   4.0		    周广程                  创建
=============================================================================*/
BOOL32 CMcuVcInst::IsRecordSrcHDBas( void )
{
    BOOL32 bNeedBas = FALSE;
    TSimCapSet tConfMainSCS = m_tConf.GetCapSupport().GetMainSimCapSet();    
    TSimCapSet tSrcMtSCS;
    TMt tRecSrc;

#define END_RET_VALUE( bVal ) \
    {   \
        bNeedBas = bVal; \
        goto lbl_EndIsRecordSrcBas;    \
    }

    if ( !IsHDConf(m_tConf) )
    {
        // 非高清会议不需要
        END_RET_VALUE(FALSE);
    }

    if ( m_tConf.m_tStatus.IsBrdstVMP() )
    {
        // 画面合成广播不需要
        END_RET_VALUE(FALSE);
    }

    if ( !m_tConf.GetConfAttrb().IsUseAdapter() )
    {
        //没有启用适配（单速单格式）不需要
        END_RET_VALUE(FALSE);
    }
    
    // 默认是会议视频广播源（可能是终端或者会议放像）
    tRecSrc = m_tVidBrdSrc;
    
    if ( tRecSrc.GetType() == TYPE_MT )
    {
        // 终端
        tSrcMtSCS = m_ptMtTable->GetSrcSCS(tRecSrc.GetMtId());
    }
    else
    {
        // 放像器(放像器直接默认主格式、主码率)
        tSrcMtSCS.SetAudioMediaType( tConfMainSCS.GetAudioMediaType() );
        tSrcMtSCS.SetVideoMediaType( tConfMainSCS.GetVideoMediaType() );
        tSrcMtSCS.SetVideoResolution( tConfMainSCS.GetVideoResolution() );
    }   

    // 单速
    if (m_tConf.GetSecBitRate() == 0)
    {
        END_RET_VALUE(FALSE);                     
    }
    
    // 双速单格式
    if (MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType()) 
    {
        if ( !m_tRecPara.IsRecLowStream() )
        {
            END_RET_VALUE(FALSE);
        }
		// 如果源的发送码率等于会议第二速率，则不需要适配
		else if ( tRecSrc.GetType() == TYPE_MT && m_ptMtTable->GetMtSndBitrate(tRecSrc.GetMtId()) == m_tConf.GetSecBitRate() )
		{
			END_RET_VALUE(FALSE);
		}
        else
        {
            END_RET_VALUE(TRUE);
        }
    }

    // 双速双格式
    // FIXME：目前高清会议不支持存在非H264双格式的会议, zgc, 2008-08-27


lbl_EndIsRecordSrcBas:

    return bNeedBas;

#undef END_RET_VALUE
}

/*=============================================================================
    函 数 名： IsDSSrcH263p
    功    能： 双流源是否为H263p
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/12/12  4.5		    张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::IsDSSrcH263p(void)
{
    TDStreamCap tDSrcCap;
    GetDSBrdSrcSim(tDSrcCap);
    return tDSrcCap.GetMediaType() == MEDIA_TYPE_H263PLUS ? TRUE : FALSE;
}

/*=============================================================================
函 数 名： GetBasLastVCUTick
功    能： 根据通道号获得上一次发送关键帧请求的TICK数
算法实现： 
全局变量： 
参    数： u8 byBasChnnl
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/21   4.0		周广程                  创建
=============================================================================*/
u32	CMcuVcInst::GetBasLastVCUTick( u8 byBasChnnl )
{
	u32 dwLastTick = 0;

	if ( byBasChnnl == m_byAudBasChnnl)
	{
		dwLastTick = m_dwAudBasChnnlLastVCUTick;
	}
	else if ( byBasChnnl == m_byVidBasChnnl )
	{
		dwLastTick = m_dwVidBasChnnlLastVCUTick;
	}
	else if ( byBasChnnl == m_byBrBasChnnl )
	{
		dwLastTick = m_dwBrBasChnnlLastVCUTick;
	}
	else if ( byBasChnnl == m_byCasdAudBasChnnl )
	{
		dwLastTick = m_dwCasdAudBasChnnlLastVCUTick;
	}
	else if ( byBasChnnl == m_byCasdVidBasChnnl )
	{
		dwLastTick = m_dwCasdVidBasChnnlLastVCUTick;
	}
	else
	{
		ConfLog( FALSE, "[SetBasLastVCUTick] Input bas channal(%d) is error! Last tick = 0\n", byBasChnnl );
		dwLastTick = 0;
	}

	return dwLastTick;
}

/*=============================================================================
函 数 名： SetBasLastVCUTick
功    能： 根据通道号获得最后一次发送关键帧请求的TICK数
算法实现： 
全局变量： 
参    数：  u8 byBasChnnl
           u32 dwTick
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/21   4.0		周广程                  创建
=============================================================================*/
void CMcuVcInst::SetBasLastVCUTick( u8 byBasChnnl, u32 dwTick )
{
	if ( byBasChnnl == m_byAudBasChnnl)
	{
		m_dwAudBasChnnlLastVCUTick = dwTick;
	}
	else if ( byBasChnnl == m_byVidBasChnnl )
	{
		m_dwVidBasChnnlLastVCUTick = dwTick;
	}
	else if ( byBasChnnl == m_byBrBasChnnl )
	{
		m_dwBrBasChnnlLastVCUTick = dwTick;
	}
	else if ( byBasChnnl == m_byCasdAudBasChnnl )
	{
		m_dwCasdAudBasChnnlLastVCUTick = dwTick;
	}
	else if ( byBasChnnl == m_byCasdVidBasChnnl )
	{
		m_dwCasdVidBasChnnlLastVCUTick = dwTick;
	}
	else
	{
		ConfLog( FALSE, "[SetBasLastVCUTick] Input bas channal(%d) is error!\n", byBasChnnl );
	}

	return;
}


/*====================================================================
    函数名      ：ShowConfEqp
    功能        ：打印会议外设
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/11    3.0         胡昌威          创建
====================================================================*/
void CMcuVcInst::ShowConfEqp(void)
{
    //画面合成
    if (CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode())
    {
        ConfLog(FALSE, "  VMP:   Vmp eqp id %d\n", m_tVmpEqp.GetEqpId());        
    }

    //多画面电视墙
    if (CONF_VMPTWMODE_NONE != m_tConf.m_tStatus.GetVmpTwMode())
    {
        ConfLog(FALSE, "  VMPTW: VmpTw eqp id %d\n", m_tVmpTwEqp.GetEqpId());
    }

    //混音
    if (m_tConf.m_tStatus.IsMixing() || m_tConf.m_tStatus.IsVACing())
    {
        ConfLog(FALSE, "  MIXER: Mixer eqp id %d, Mixer group id %d, MixMode %d \n",
                m_tMixEqp.GetEqpId(), m_byMixGrpId, m_tConf.m_tStatus.GetMixerMode());
        
        // guzh [6/8/2007]         
        if ( m_tConf.m_tStatus.IsSpecMixing() )
        {
            ConfLog(FALSE, "  MIXER: Specified Mix Member List: \n");
            for (u8 byLoop = 0; byLoop < MAXNUM_MIXER_CHNNL+1; byLoop ++)
            {
                if ( m_abyMixMtId[byLoop] != 0)
                {
                    OspPrintf(TRUE, FALSE, "\t\tIndex.%d Mt.%d\n",
                                            byLoop, m_abyMixMtId[byLoop]);
                }
            }
        }
    }

    //录像
    if (!m_tConf.m_tStatus.IsNoRecording())
    {
        ConfLog(FALSE, "  REC:   Recorder eqp id %d, recorder channel %d \n  REC:   Recorder param:\n",
                m_tRecEqp.GetEqpId(), m_byRecChnnl);
        m_tRecPara.Print();
    }

    //放像
    if (!m_tConf.m_tStatus.IsNoPlaying())
    {
        ConfLog(FALSE, "  REC:   Player eqp id %d, player channel %d \n  REC:   Player Attrib:\n",
                m_tPlayEqp.GetEqpId(), m_byPlayChnnl);
        m_tPlayEqpAttrib.Print();
    }

    //音频适配
    if (m_tConf.m_tStatus.IsAudAdapting())
    {
        ConfLog(FALSE, "  BAS:   Audio adapter eqp id %d, Audio adapter channel %d \n",
                m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl);
    }

    //视频适配
    if (m_tConf.m_tStatus.IsVidAdapting())
    {
        ConfLog(FALSE, "  BAS:   Video adapter eqp id %d, Video adapter channel %d, bitrate %d \n",
                m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, m_wVidBasBitrate);
    }

    //码率适配
    if (m_tConf.m_tStatus.IsBrAdapting())
    {
        ConfLog(FALSE, "  BAS:   BitRate adapter eqp id %d, BitRate adapter channel %d, bitrate %d \n",
                m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, m_wBasBitrate);
    }

    //级联音频适配
    if (m_tConf.m_tStatus.IsCasdAudAdapting())
    {
        ConfLog(FALSE, "  BAS:   Cascade Audio adapter eqp id %d, Audio adapter channel %d\n",
            m_tCasdAudBasEqp.GetEqpId(), m_byCasdAudBasChnnl);
    }
    
    //级联视频适配
    if (m_tConf.m_tStatus.IsCasdVidAdapting())
    {
        ConfLog(FALSE, "  BAS:   Cascade Video adapter eqp id %d, Video adapter channel %d\n",
            m_tCasdVidBasEqp.GetEqpId(), m_byCasdVidBasChnnl);
    }

    //丢包重传
    if( m_tConf.m_tStatus.IsPrsing())
    {
        ConfLog(FALSE, "  PRS:   Prs eqp id %d, VidChnnl %d, VidChnnl2 %d, AudChnnl %d, AudBasChnnl %d, VidBasChnnl %d, BrBasChnnl %d \n",
                m_tPrsEqp.GetEqpId(), m_byPrsChnnl, m_byPrsChnnl2, m_byPrsChnnlAud, 
                m_byPrsChnnlAudBas, m_byPrsChnnlVidBas, m_byPrsChnnlBrBas);
    }

    if ( m_tConf.GetConfAttrb().GetDataMode() != CONF_DATAMODE_VAONLY )
    {
        ConfLog(FALSE, "  DCS:   DcsIdx.%d, DataConfOnGoing.%d, DataConfReCreated.%d\n",
                m_byDcsIdx, m_tConfInStatus.IsDataConfOngoing(), m_tConfInStatus.IsDataConfRecreated());        
    }

    if (m_tLastVmpParam.GetVMPMode() != CONF_VMPMODE_NONE )
    {
        ConfLog(FALSE, "        m_tLastVmpParam:\n");  
        m_tLastVmpParam.Print();
    }   
    
    if (m_tLastVmpTwParam.GetVMPMode() != CONF_VMPMODE_NONE )
    {
        ConfLog(FALSE, "        m_tLastVmpTwParam:\n");  
        m_tLastVmpTwParam.Print();
    }      
}

/*====================================================================
    函数名      ：SwitchDiscuss2SpecMix
    功能        ：自动把会议讨论(或者VAC)切换成定制混音(或者停止VAC)
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/01/31    4.0         顾振华          创建
====================================================================*/
void CMcuVcInst::SwitchDiscuss2SpecMix()
{
#ifdef _MINIMCU_
    if ( !m_tConfInStatus.IsSwitchDiscuss2Mix() )
    {
        // Step.1 先停止智能混音
        if ( !m_tConf.m_tStatus.IsMixing() )
        {
            return;
        }
         
        BOOL32 bWholeDiscuss = m_tConf.m_tStatus.IsAutoMixing();
        if ( bWholeDiscuss && !m_tConf.IsDiscussAutoStop())
        {
            // 标记需要自动切换
            m_tConfInStatus.SetSwitchDiscuss2Mix(TRUE);
        } 
        
        // guzh [11/7/2007] 即使有VAC也只能被停止
        StopMixing();       
    }
    else
    {
        // Step.2 切换到定制混音,自动选择前 n 个终端
        m_tConfInStatus.SetSwitchDiscuss2Mix(FALSE);
        
        CServMsg cServMsg;

        // 增加终端
        TMt tMt;
        u8 byCount = 0;
        u16 wError = 0;
        u8 byMaxCount = CMcuPfmLmt::GetMaxOprMixNum( m_tConf, wError );
        if (byMaxCount == 0)
        {
            NotifyMcsAlarmInfo(0, wError);
            return;
        }

        for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
        {
            if ( !m_tConfAllMtInfo.MtJoinedConf(byMtId) )
                continue;
            tMt = m_ptMtTable->GetMt(byMtId);
            cServMsg.CatMsgBody( (u8*)&tMt, sizeof(TMt) );
            byCount ++ ;

            if ( byCount >= byMaxCount )
                break;
        }
        if ( byCount > 0 )
        {
            AddRemoveSpecMixMember((TMt*)cServMsg.GetMsgBody(), byCount);
        }
    }
#endif    
}

/*====================================================================
    函数名      ：GetMaxMixerDepth
    功能        ：获取混音器的最大混音深度
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/01/31    4.0         顾振华          创建
====================================================================*/
u8 CMcuVcInst::GetMaxMixerDepth() const
{
#ifdef _MINIMCU_
    if ( MEDIA_TYPE_G729 == m_tConf.GetMainAudioMediaType() )
        return MAXNUM_MIXER_DEPTH_8000B_G729;
    else
        return MAXNUM_MIXER_DEPTH_8000B;
#else
    return MAXNUM_MIXER_DEPTH;
#endif
}

/*====================================================================
    函数名      ：ChgMtFormatInSdVmp
    功能        ：H264会议画面合成，分辨率的调整
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId    : 要调整的终端ID
                  u8 byNewFormt: 调整后的分辨率
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/05    4.0         张宝卿          创建
	09/04/29    4.0         付秀华          支持调整下级MCU中回传终端的分辨率
====================================================================*/
void CMcuVcInst::ChgMtFormatInSdVmp(	u8 byMtId,
										TVMPParam *ptVmpParam,
										BOOL32 bStart
										)
{
	if ( NULL == ptVmpParam )
    {
        return;
    }
    
    // guzh [9/1/2006] 会议不要求自适应分辨率
    if ( !m_tConf.IsVmpAutoAdapt() )
    {
        return;
    }

    TMt tMt = m_ptMtTable->GetMt(byMtId);
    if (tMt.IsNull()) 
    {
        return;
    }

    if ((tMt.GetType() != TYPE_MT && tMt.GetType() != TYPE_MCU) ||
		(m_ptMtTable->GetManuId(tMt.GetMtId()) != MT_MANU_KDC && m_ptMtTable->GetManuId(tMt.GetMtId()) != MT_MANU_KDCMCU))
    {
        ConfLog(FALSE, "[ChangeMtVideoFormat] dstmt.%d is not mt(mcu) or kdc mt(mcu). type.%d manu.%d\n",
                tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return;
    }

    //目前只改变第一路视频通道的分辨率
    u8 byChnnlType = LOGCHL_VIDEO;   
   
    TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( byMtId ) ||
         !m_ptMtTable->GetMtLogicChnnl( byMtId, byChnnlType, &tLogicChannel, FALSE ) )
    {
        return;
    }

    u8 byMtStandardFormat = tLogicChannel.GetVideoFormat(); // 查找原来建立的分辨率

    u8 byLogicChannlType = tLogicChannel.GetChannelType(); // 信道类型 xliang [080731] 下面判断多次用到
    u8 byChannlVidFPS = tLogicChannel.GetChanVidFPS();// 通道码流帧率 xliang [080731]

    u8 byNewFormat; //分辨率类型
    u8 byVmpType = ptVmpParam->GetVMPStyle();   
    
    //若终端还在多画面电视墙里或vmp里且不是发言人，则不恢复分辨率
    BOOL32 bSpeaker = (GetLocalSpeaker() == tMt);
    
    // 8000B 
    // H264:          sqcif<-->原来(cif)
    // MPEG4, MPEG2:  四画面以上qcif<-->原来
    // 一画面不改，1－3画面发言人不改
#ifdef _MINIMCU_
    //FIXME: MINIMCU暂未处理高清VMP
    if (MEDIA_TYPE_H264 == byLogicChannlType)
    {     
        if (VMP_STYLE_ONE == byVmpType)
        {
            byNewFormat = bStart ? VIDEO_FORMAT_CIF:byMtStandardFormat;
        }
        else if (VMP_STYLE_VTWO == byVmpType || VMP_STYLE_HTWO == byVmpType || 
                VMP_STYLE_THREE == byVmpType || VMP_STYLE_FOUR == byVmpType || 
                VMP_STYLE_SPECFOUR == byVmpType)
        {
                //zbq[12/27/2007]通用恢复到原分辨率
                byNewFormat = bStart ? VIDEO_FORMAT_QCIF:byMtStandardFormat;
        }
        else
        {
            u8 byResSqcif = (VMP_STYLE_NINE == byVmpType ? VIDEO_FORMAT_SQCIF_112x96 : VIDEO_FORMAT_SQCIF_96x80);            

            //zbq[12/27/2007]通用恢复到原分辨率
            byNewFormat = bStart ? byResSqcif : byMtStandardFormat;
        }       

        // xsl [11/1/2006] 8000b mcu h264会议四画面及以上发言人也要改变分辨率
        if (VMP_STYLE_ONE == byVmpType || VMP_STYLE_VTWO == byVmpType || VMP_STYLE_THREE == byVmpType)
        {
            //zbq[04/07/2008] 发言人统一调整 分辨率
            //byNewFormat = bSpeaker ? byMtStandardFormat : byNewFormat;
        }        
    }
    else if (MEDIA_TYPE_MP4  == byLogicChannlType ||
             MEDIA_TYPE_H262 == byLogicChannlType )
    {
        if (VMP_STYLE_ONE == byVmpType)
        {
            byNewFormat = byMtStandardFormat;
        }
        else if (VMP_STYLE_VTWO == byVmpType || VMP_STYLE_HTWO == byVmpType || 
                VMP_STYLE_THREE == byVmpType || VMP_STYLE_FOUR == byVmpType || 
                VMP_STYLE_SPECFOUR == byVmpType)
        {
            byNewFormat = bStart ? VIDEO_FORMAT_CIF:byMtStandardFormat;
        }
        else
        {
            byNewFormat = bStart ? VIDEO_FORMAT_QCIF:byMtStandardFormat;
        }

        //zbq[04/07/2008] 发言人统一调整 分辨率
        //发言人保持原来的分辨率
        //byNewFormat = bSpeaker ? byMtStandardFormat : byNewFormat;
    }
    else
    {
        return;
    }
#else
    // 8000 
    // 四画面: CIF及以下不调，H264的HD/4CIF调到CIF
    // H264:         qcif<-->原来(cif)
    // MPEG4, MPEG2: cif<-->原来

    if (MEDIA_TYPE_H264 == byLogicChannlType)
    {        
        if (VMP_STYLE_ONE == byVmpType || VMP_STYLE_VTWO == byVmpType ||
            VMP_STYLE_HTWO == byVmpType || VMP_STYLE_THREE == byVmpType ||
            VMP_STYLE_FOUR == byVmpType || VMP_STYLE_SPECFOUR == byVmpType ||
			//fxh [02/05/2009] 针对VCS特殊动态VMP的临时调整 
			(VCS_CONF == m_tConf.GetConfSource() &&
			 VMP_STYLE_DYNAMIC == byVmpType && IsHDConf(m_tConf)) )
        {

            //zbq[12/27/2007] 降到CIF
            if (VIDEO_FORMAT_CIF != m_tConf.GetMainVideoFormat())
            {
                byNewFormat = bStart ? VIDEO_FORMAT_CIF : byMtStandardFormat;
            }
            else
            {
                byNewFormat = byMtStandardFormat;
            }                
        }
        else
        {
            //zbq[12/27/2007]通用恢复到原分辨率
            byNewFormat = bStart ? VIDEO_FORMAT_QCIF:byMtStandardFormat;
        }        
    }
    else if (MEDIA_TYPE_MP4  == byLogicChannlType ||
             MEDIA_TYPE_H262 == byLogicChannlType )
    {       
        if (VMP_STYLE_ONE == byVmpType || VMP_STYLE_VTWO == byVmpType ||
            VMP_STYLE_HTWO == byVmpType || VMP_STYLE_THREE == byVmpType ||
            VMP_STYLE_FOUR == byVmpType || VMP_STYLE_SPECFOUR == byVmpType)
        {
            byNewFormat = byMtStandardFormat;
        }
        else
        {
            byNewFormat = bStart ? VIDEO_FORMAT_CIF:byMtStandardFormat;
        }        
    }
    else
    {
        return;   
    }  

    //发言人保持原来的分辨率
    //byNewFormat = bSpeaker ? byMtStandardFormat : byNewFormat;
#endif
    
	if ( !bStart )    
	{
		if ( ((m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE && m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tMt)) ||
			 (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE && m_tConf.m_tStatus.GetVmpTwParam().IsMtInMember(tMt))) )
		{
			EqpLog("[ChangeMtVideoFormat] mt.%d is still in VMP or MPW, keep its format !\n", tMt.GetMtId());
			return;
		}
	}   

    // 调节高清主视频适配, zgc, 2008-08-21
    if ( m_tVidBrdSrc == tMt )
    {
        //zbq[12/11/2008] 视频源分辨率切换后，适配不作调整
        //AdjustHDVidAdapt();
    }
    
	// fxh 针对下级MCU调整分辨率,实际为对其回传通道中的终端调整
	if (MT_TYPE_SMCU == tMt.GetMtType())
	{
		TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMtId());
		if (ptMcInfo != NULL)
		{
			EqpLog("[ChangeMtVideoFormat]change mt(mcuid:%d, mtid:%d) in sub mcu%d\n", 
				   ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId(), tMt.GetMtId());
		}

		if (ptMcInfo != NULL &&
			!ptMcInfo->m_tMMcuViewMt.IsNull())
		{
			CascadeAdjMtRes(ptMcInfo->m_tMMcuViewMt, byNewFormat, bStart);			
		}
	}
	else
	{
		CServMsg cServMsg;
		//xliang [080731] modify msgbody
		cServMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
		cServMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
		//cServMsg.CatMsgBody( &byLogicChannlType,sizeof(u8) );
		cServMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
		//cServMsg.CatMsgBody( &byChannlVidFPS, sizeof(u8) );
		SendMsgToMt( byMtId, cServMsg.GetEventId(), cServMsg );
	}

    EqpLog( "[ChangeMtVideoFormat] chnnl type: %d, send videoformat<%d> change msg to mt<mcuid:%d, mtid:%d, mttype:%d>!\n",
            tLogicChannel.GetChannelType(), byNewFormat, tMt.GetMcuId(), tMt.GetMtId(), tMt.GetMtType() );

    return;
}

/*====================================================================
    函数名      ChgMtFormatInMpu
    功能        ：分辨率的调整
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId				:[in] 要调整的终端ID
                  TVMPParam &tVmpParam	:[in] VMP Param
				  BOOL32 bStart			:[in] 是否开始调（区别与恢复分辨率）

    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	12/10/2008				薛亮	        创建          
====================================================================*/
BOOL32 CMcuVcInst::ChgMtFormatInMpu( u8 byMtId,
									 TVMPParam *ptVmpParam,
									 BOOL32 bStart,
									 BOOL32 bSeize,
                                     BOOL32 bConsiderVmpBrd
									 )
{
	
	if ( NULL == ptVmpParam )
    {
        return FALSE;
    }

	TMt tMt = m_ptMtTable->GetMt(byMtId);
    if (tMt.IsNull()) 
    {
        return FALSE;
    }

    if ( !m_tConf.IsVmpAutoAdapt() )	//当前会议不是画面合成自适应
    {
        return TRUE;					//无过滤操作，返回之后照进通道，所以返回TURE
    }

	if (tMt.GetType() != TYPE_MT)
	{
		ConfLog(FALSE, "[ChgMtFormatInMpu] dstmt.%d is not mt. type.%d manu.%d\n",
			tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return FALSE;
	}
	
	TMtStatus tMtStatus;
	m_ptMtTable->GetMtStatus( tMt.GetMtId(), &tMtStatus );
	if( tMtStatus.IsInTvWall() )
	{
		return TRUE;					//Mt已在DEC5中，则不调分辨率，返回TRUE表之后照进通道
	}

	//目前只改变第一路视频通道的分辨率
    u8 byChnnlType = LOGCHL_VIDEO;   

    TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( byMtId ) ||
		!m_ptMtTable->GetMtLogicChnnl( byMtId, byChnnlType, &tLogicChannel, FALSE ) )
    {
        return FALSE;
    }

	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
	u8 byMpuBoardVer = tPeriEqpStatus.m_tStatus.tVmp.m_byBoardVer;
	u8 byPos = ptVmpParam->GetChlOfMtInMember( tMt );
	u8 byVmpStyle = ptVmpParam->GetVMPStyle();
    u8 byMtStandardFormat = tLogicChannel.GetVideoFormat(); // 查找原来建立的分辨率
    u8 byLogicChannlType = tLogicChannel.GetChannelType();	// 信道类型 
    u8 byChannlVidFPS = tLogicChannel.GetChanVidFPS();		// 通道码流帧率 
    u8 byNewFormat = 0;										// 调整后的分辨率
	BOOL32 bNeedAdjustRes = FALSE;							// 是否真的需要调整分辨率
    
	if(!bStart) //恢复分辨率
	{
		if ( ((m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE && m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tMt)) ||
			(m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE && m_tConf.m_tStatus.GetVmpTwParam().IsMtInMember(tMt))) )
		{
			EqpLog("[ChgMtFormatInMpu] mt.%d is still in VMP or MPW, keep its format !\n", tMt.GetMtId());
			return FALSE; //FIXME:return TRUE会怎么样
		}
		else
		{
			byNewFormat = byMtStandardFormat;
			EqpLog("[ChgMtFormatInMpu] mt.%u recover it's format!\n",tMt.GetMtId());
			//Send Extra NoneStandard Message to Mt //目前MT还不支持
// 			CServMsg cServMsg;
// 			SendMsgToMt(byMtId, MCU_MT_VIDEOPARAMRECOVER_CMD, cServMsg);
		}
	}
	else //调整MT分辨率
	{
		if( byVmpStyle == VMP_STYLE_ONE)	//1画面不调分辨率
		{
			// xliang [7/14/2009] 因为终端当前实际编的分辨率MCU并不知道，所以还是得调一下
			if( MEDIA_TYPE_H264 != byLogicChannlType ) 
			{
				byNewFormat = VIDEO_FORMAT_CIF;	//mp4 auto 的直接调成CIF
				EqpLog("[ChgMtFormatInMpu] 1 vmp adjust resolution CIF for none h264!\n");
			}
			else
			{
				byNewFormat = byMtStandardFormat;
				EqpLog("[ChgMtFormatInMpu] 1 vmp need not adjust resolution!\n");
			}
		}	
		else
		{
			u8 byReqRes	= 0;										//某风格中某位置所允许的最大分辨率	
			u16 wBR		= 0;										//码率，不由mcu触发调整码率
			s32 nResCmpRst = 3;										//分辨率比较结果，初始化3,表征比较无意义
			
			//特殊身份判断
			BOOL32 bSpeaker = (GetLocalSpeaker() == tMt);									//是否是发言人
			BOOL32 bNoneKeda = ( MT_MANU_KDC != m_ptMtTable->GetManuId(tMt.GetMtId())  
				&& MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tMt.GetMtId()) );	//是否非keda终端
			BOOL32 bSelected = IsSelectedbyOtherMt(tMt.GetMtId());							//是否被选看
			
			BOOL32 bSelectedByRoll = FALSE;		// xliang [5/7/2009] 点名造成的选看。因为时序问题，这里特殊处理下
			if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
				&& ( m_tRollCaller == tMt 
				|| m_tRollCallee == tMt ) )
			{
				bSelectedByRoll = TRUE;
			}
			
			//各种VMP前适配通道能力及非h264CIF通道能力
			u8 byMaxHdChnnlNum	= 0;
			u8 byMaxCifChnnlNum  = 0;
			
			switch(byVmpSubType)
			{
			case MPU_DVMP:
			case EVPU_DVMP:
				{
					byMaxHdChnnlNum	= MAXNUM_DVMP_HDCHNNL;
					break;
				}
			case MPU_SVMP:
			case EVPU_SVMP:
				{
					if(byMpuBoardVer == MPU_BOARD_B256)
					{
						byMaxHdChnnlNum	= MAXNUM_SVMPB_HDCHNNL;
						byMaxCifChnnlNum = MAXNUM_SVMP_NON264CIFCHNNL;
					}
					else
					{
						byMaxHdChnnlNum	= MAXNUM_SVMP_HDCHNNL;
						byMaxCifChnnlNum = MAXNUM_SVMP_NON264CIFCHNNL;
					}
					
					break;
				}
			default:
				break;
			}
			/*
			if(m_tVmpChnnlInfo.m_byAllChnnlNum > byMaxAllChnnlNum)
			{
			EqpLog("[ChangeMtVideoFormat]all channel has been occupied!\n");
			return FALSE;
		}*/
			
			
			if(byLogicChannlType != MEDIA_TYPE_H264) 
			{
				u8 byNon264CifChnnlNum = m_tVmpChnnlInfo.GetCifChnnlNum();
				if (byNon264CifChnnlNum >= byMaxCifChnnlNum)
				{
				EqpLog("[ChgMtFormatInMpu] VMP channels for none h264 Cif reach the maximum(%u),\
					so Mt.%u won't enter VMP\n", byMaxCifChnnlNum, byMtId );
				return FALSE;
				}
				
				if (byMtStandardFormat == VIDEO_FORMAT_4CIF && bNoneKeda)
				{
					EqpLog("[ChgMtFormatInMpu] NoneKeda Mt.%u which is MPEG4 with 4CIF in format is not support by VMP!\n",
						byMtId );
					return FALSE;
				}
				
				byNewFormat = VIDEO_FORMAT_CIF;
				m_tVmpChnnlInfo.SetCifChnnlNum(++byNon264CifChnnlNum);
				EqpLog("[ChgMtFormatInMpu] Force to change Mt.%u 's format to CIF as media type other than H264!\n", byMtId);
			}
			else // H264格式的处理
			{
				//判断是否要调整分辨率以及给出要调整到的分辨率大小
				bNeedAdjustRes = VidResAdjust( byVmpStyle, byPos, byMtStandardFormat, byReqRes);
				BOOL32 bNeedAdjust = bNeedAdjustRes;	//不考虑进前适配通道，分辨率是否需要调整
				
				// xliang [3/24/2009] 占前适配通道 不再局限于HD能力，对与EVPU，4CIF也可进前适配通道
				
				//在vmp批量轮询中，非科达的MT若其需要调整分辨率，则黑屏 
				// 			if(m_tConf.m_tStatus.m_tVMPParam.IsVMPBatchPoll() && bNoneKeda && bNeedAdjust) 
				// 			{
				// 				EqpLog("[ChangeMtVideoFormat] Batch polling: The Mt is none keda's!\n");
				// 				return FALSE;
				// 			}
				
				
				// xliang [4/7/2009] Res 比较下来没必要调分辨率的MT，即使是特殊身份的也不占前适配通道
				//但切风格可能会造成频繁抢占，有风险
				if( bNeedAdjust 
					&&
					(  ( bSpeaker && !m_tConf.m_tStatus.IsBrdstVMP() )
					|| ( bSelected && (!m_tConf.m_tStatus.IsBrdstVMP() || !bConsiderVmpBrd) )
					|| bNoneKeda 
					|| bSelectedByRoll )
					)
				{
					TChnnlMemberInfo tChnnlMemInfo;
					
					u8 byHdChnnlNum = m_tVmpChnnlInfo.m_byHDChnnlNum;
					//tChnnlMemInfo.m_byMtId = byMtId;
					tChnnlMemInfo.SetMt(tMt);
					if(bSpeaker && !m_tConf.m_tStatus.IsBrdstVMP())
					{
						tChnnlMemInfo.SetAttrSpeaker();
					}
					if(bNoneKeda)
					{
						tChnnlMemInfo.SetAttrNoneKeda();
					}
					if( ( bSelected 
						&& ( (!m_tConf.m_tStatus.IsBrdstVMP() && !bSpeaker)	//非VMP广播下发言人会冲走被选看
						|| !bConsiderVmpBrd ) )
						|| bSelectedByRoll									//点名造成的选看，同VMP广播与否无关
						)
					{
						tChnnlMemInfo.SetAttrSelected();
					}
					
					if ( byHdChnnlNum < byMaxHdChnnlNum					//有剩余HD前适配通道可用则占之
						|| ( m_tVmpChnnlInfo.GetChnnlIdxCtnMt(tMt) != -1 )	//成员已经在前适配通道中，可能会重刷成员属性
						)
					{
						//不允许多个channel中有相同的MT
						s32 nRet = m_tVmpChnnlInfo.AddInfoToHdChnnl(&tChnnlMemInfo);
						if(nRet == -1) //该HD MT已经进入其他HD通道，不允许再进。之前在对vmpParam已有过滤，这里再保护一下
						{
							//虽然已经在通道内了，但成员属性可能发生变化(如：从被选看的变为发言人)，所以这里刷新一下通道成员属性
							//在AddInfoToHdChnnl接口里增加该处理
							EqpLog("[ChgMtFormatInMpu]The same Hd Mt.%u is not allowed to enter multiple vmp channel!\n",byMtId);
							return FALSE;
						}
						//进vmp高清前适配通道，分辨率不用调整
						bNeedAdjustRes = FALSE;
						EqpLog("[ChgMtFormatInMpu]Mt.%u -> HD adapter channel, no need to change it's video format.\n",
							byMtId);
					}
					else
					{
						if(m_tConf.m_tStatus.m_tVMPParam.IsVMPBatchPoll())
						{
							//vmp轮询永远不进行抢占
							EqpLog("[ChgMtFormatInMpu]no seize operation in VMP Poll mode!\n");
							if (bNoneKeda)
							{
							EqpLog("[ChgMtFormatInMpu]The Mt.%u is none keda and VMP VIP channel is full! \
								Since VMP mode is [Batch Poll] mode, the Mt will not enter any channel.\n",
								byMtId );
							return FALSE;	
							}
						}
						else if(m_tConf.m_tStatus.m_tVMPParam.IsVMPAuto())
						{
							//自动画面合成不抢占
							EqpLog("[ChgMtFormatInMpu]no seize operation in VMP Auto mode!\n");
							if (bNoneKeda)
							{
							EqpLog("[ChgMtFormatInMpu]The Mt.%u is none keda and VMP VIP channel is full! \
								Since VMP mode is [AUTO VMP] mode, the Mt will not enter any channel.\n",
								byMtId );
							return FALSE;	
							}
						}
						else
						{	
							//抢占提示
							if (bSeize)
							{
								CServMsg cServMsg;
								//遍历所有占用HD的通道
								cServMsg.SetMsgBody((u8*)&tMt,sizeof(tMt));//需要抢占高清适配通道的终端ID
								for(u8 byChnnlIdx = 0; byChnnlIdx<byHdChnnlNum; byChnnlIdx++)
								{
									m_tVmpChnnlInfo.GetHdChnnlInfo(byChnnlIdx,&tChnnlMemInfo);
									cServMsg.CatMsgBody((u8 *)&tChnnlMemInfo,sizeof(tChnnlMemInfo));
								}
								SendMsgToAllMcs(MCU_MCS_VMPPRISEIZE_REQ, cServMsg);
								
								return FALSE;//require Hd channel but fail to occupy it now, so enter no channel at all.
							}
							else
							{
								//抢占hd通道还未到抢占时机(即HD通道释放未处理完)，
								//会有相关定时器再次触发抢占，不要再次出现抢占提示。
								//当定时次数达到一定次数时，才再次出现抢占提示(应对连续2次抢占，
								//被抢的是同一个MT，则后一次始终没法抢成功的情况)
								
								// xliang [4/21/2009] 该策略由于在最初启VMP时有另一段保护，所以出现连续抢占的概率相当低
								// ( 对于增加一个成员的changevmpparam操作而言，不可能有连续抢占发生；
								// 仅仅在切风格操作中很小概率发生连续抢占。若去除对于进前适配的优化，可以使概率降低到0 )
								
								// xliang [7/14/2009] 目前该else的存在的意义（bSeize存在的意义）
								// 仅供程序在其他处调用时，不希望抢占发生，而是直接无条件调分辨率。
								// 一个场景就是在DEC5中的MT退出DEC5时要恢复（调整）其分辨率
								EqpLog("[ChgMtFormatInMpu]Make Mt.%u seize no adapter channel on purpose!\n",byMtId);
							}
							//这里不设超时
							
							//return FALSE;//require Hd channel but fail to occupy it now, so enter no channel at all.
						}
					}
				}
				else //非特殊身份的高清MT，则根据合成风格及通道位置调整分辨率
				{
					//原来是特殊身份，被抢占的情况：释放原来占用的前向适配通道
					BOOL32 nRet = m_tVmpChnnlInfo.ClearOneChnnl(m_ptMtTable->GetMt(byMtId));
					if(nRet)
					{
						EqpLog("[ChgMtFormatInMpu]Mt.%u's format change: get out of the Hd Channel.\n",byMtId);
					}
					else
					{
						// Do nothing
					}
				}
				
				if(bNeedAdjustRes)
				{
					byNewFormat = byReqRes;
				}
				else
				{
					byNewFormat = byMtStandardFormat;
				}
				
				//GetResStr()不能直接连续调用，内部维护了一个static varible.
				EqpLog("--------【Mt.%u (Channel.%u): Resolution compare】------\n",byMtId, byPos);
				EqpLog("Original Res: \t%s\n",GetResStr(byMtStandardFormat));
				EqpLog("Require Res: \t%s\n",GetResStr(byReqRes));
				EqpLog("======================================================\n");
				if( bNeedAdjust && !bNeedAdjustRes )
				{
					EqpLog("Need Adjust: \t0 (Enter HD adapter channel)\n");
				}
				else if(!bNeedAdjust)
				{
					EqpLog("Need Adjust: \t0 \n");
				}
				else
				{
					EqpLog( "Need Adjust: \t1 \n");
				}	
			}
		}	
    }
	
	// xliang [4/11/2009] 分辨率都要调一下，否则对于中途切风格的情况会有问题
	//因为比较是以最初的Res为基准的
	if (MT_TYPE_SMCU == tMt.GetMtType())
	{
		TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMtId());
		if (ptMcInfo != NULL)
		{
			EqpLog("[ChgMtFormatInMpu]change mt(mcuid:%d, mtid:%d) in sub mcu%d\n", 
				ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId(), tMt.GetMtId());
		}
		
		if (ptMcInfo != NULL &&
			!ptMcInfo->m_tMMcuViewMt.IsNull())
		{
			// xliang [4/29/2009] FIXME目前只有一个回传通道,所以先(2,2)在VMP，后上级选看下级(2,1) 会有问题. 
			// 可用新加的回传源变量标示tSrcMt. (FIXME:参VCS流,变量保持统一)
			// 回传源变化不仅要降(2,1)分辨率，还得恢复(2,2)分辨率。
			// FIXME: 回传源调整处即可进行恢复操作
			TMt tSrcMt = ptMcInfo->m_tMMcuViewMt;
			TMt tLastSrcMt = ptMcInfo->m_tLastMMcuViewMt;
			if (!tLastSrcMt.IsNull())
			{
				//恢复上次回传源Mt的分辨率
				CascadeAdjMtRes( tLastSrcMt, byNewFormat, FALSE);
			}
			
			CascadeAdjMtRes( tSrcMt, byNewFormat, bStart, byVmpStyle, byPos);
			
			return FALSE;	//返回FALSE意味不建交换，等下级MCU调Res返回Ok后再补建交换		
		}
	}
	else
	{
		CServMsg cServMsg;
		cServMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
		cServMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
		cServMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
		SendMsgToMt( byMtId, cServMsg.GetEventId(), cServMsg );
		
		EqpLog( "[ChgMtFormatInMpu] chnnl type: %d, send videoformat<%d>(%s) change msg to mt<%d>!\n",
			tLogicChannel.GetChannelType(), byNewFormat, GetResStr(byNewFormat), byMtId );
	}
	
    return TRUE;
}

/*==============================================================================
函数名    :  RecoverMtResInMpu
功能      :  恢复终端的分辨率，同时考虑该Mt在画面合成器中的情况
算法实现  :  
参数说明  :  TMt	tMt	[in]: 要恢复分辨率的终端
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-7-13	 4.6           薛亮                             create
==============================================================================*/
BOOL32 CMcuVcInst::RecoverMtResInMpu(TMt tMt)
{
	if( tMt.IsNull())
	{
		return FALSE;
	}

	if (tMt.GetType() != TYPE_MT)
	{
		ConfLog(FALSE, "[RecoverMtResInMpu] dstmt.%d is not mt. type.%d manu.%d\n",
			tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return FALSE;
	}

	u8 byMtId = tMt.GetMtId();
	u8 byChnnlType = LOGCHL_VIDEO;   
	
    TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( byMtId )
		|| !m_ptMtTable->GetMtLogicChnnl( byMtId, byChnnlType, &tLogicChannel, FALSE )
		|| MEDIA_TYPE_H264 != tLogicChannel.GetChannelType()	//目前的调整分辨率均是针对h264
		)
    {
        return FALSE;
    }

	u8 byLogicChannlType = tLogicChannel.GetChannelType();	// 信道类型
    u8 byMtStandardFormat = tLogicChannel.GetVideoFormat(); // 原始分辨率
    u8 byNewFormat = 0;										// 要恢复到的分辨率
	
	//原始分辨率就是CIF，则不用恢复了
	if( VIDEO_FORMAT_CIF == byMtStandardFormat )
	{
		ConfLog(FALSE, "[RecoverMtResInMpu] The original resolution is Cif, so no need to recover resolution!\n");
		return FALSE;
	}
	
	BOOL32 bRecoverToOrg = FALSE;
	TVMPParam tVmpParam;
	//该终端是否是在VMP中
	if(m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE && m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tMt))
	{
		tVmpParam = m_tConf.m_tStatus.GetVmpParam();
		u8 byVmpStyle = tVmpParam.GetVMPStyle();
		if( byVmpStyle == VMP_STYLE_ONE)	//1画面不调分辨率
		{
			EqpLog("[RecoverMtResInMpu] 1 vmp need not adjust resolution further, so recover to its original resolution!\n");
			bRecoverToOrg = TRUE;
		}
	}
	else
	{
		bRecoverToOrg = TRUE;
	}

	if( bRecoverToOrg )
	{
		byNewFormat = byMtStandardFormat;
		CServMsg cServMsg;
		cServMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
		cServMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
		cServMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
		SendMsgToMt( byMtId, cServMsg.GetEventId(), cServMsg );
		
		EqpLog( "[ChgMtFormatInMpu] chnnl type: %d, send videoformat<%d>(%s) change msg to mt<%d>!\n",
			tLogicChannel.GetChannelType(), byNewFormat, GetResStr(byNewFormat), byMtId );
	}
	else
	{
		ChangeMtVideoFormat( tMt, &tVmpParam, TRUE, FALSE );
	}

	return TRUE;
}

/*==============================================================================
函数名    :  AddToVmpNeglected
功能      :  把某终端ID加入到VMP成员忽略列表
算法实现  :  
参数说明  :  u8 byMtId	[in]
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2008-12-17					薛亮							创建
==============================================================================*/
void CMcuVcInst::AddToVmpNeglected( u8 byMtId)
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)return;
	m_abyMtNeglectedByVmp[(byMtId-1) / 8] |= 1 << ( (byMtId-1) % 8 );
}

/*==============================================================================
函数名    :  IsMtNeglectedbyVmp
功能      :  判断某个终端是否在VMP成员忽略列表中
算法实现  :  
参数说明  :  u8 byMtId	[in]
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2008-12-17					薛亮							create
==============================================================================*/
BOOL32 CMcuVcInst::IsMtNeglectedbyVmp( u8 byMtId)
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return FALSE;
	}
	return ( ( m_abyMtNeglectedByVmp[(byMtId-1) / 8] & ( 1 << ( (byMtId-1) % 8 ) ) ) != 0 );
}

/*====================================================================
    函数名      AdjustVmpParam
    功能        ：画面合成参数改变
    算法实现    ：
    引用全局变量：
    输入参数说明：TVMPParam *ptVMPParam	[in]画面合成参数
				  BOOL32 bStart			[in]是否是开始画面合成, default: FALSE
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
05/18/2009		4.6			薛亮			create				
====================================================================*/
void CMcuVcInst::AdjustVmpParam(TVMPParam *ptVMPParam, BOOL32 bStart)
{
	if (m_tVmpEqp.GetEqpId() < VMPID_MIN || m_tVmpEqp.GetEqpId() > VMPID_MAX)
	{
		CfgLog(FALSE, "[AdjustVmpParam] it hasn't assign correct vmp(id:%d) yet\n", m_tVmpEqp.GetEqpId());
		return;
	}

	TPeriEqpStatus tPeriEqpStatus;	
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
	if(byVmpSubType == VMP)
	{
		ChangeVmpParam(ptVMPParam, bStart);
	}
	else
	{
		ChangeNewVmpParam(ptVMPParam, bStart);
	}
}

/*====================================================================
    函数名      ：ChangeNewVmpParam
    功能        ：画面合成参数改变
    算法实现    ：
    引用全局变量：
    输入参数说明：TVMPParam *ptVMPParam, 画面合成参数
				  BOOL32 bStart 是否是开始画面合成
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
12/31/2008						
====================================================================*/
void CMcuVcInst::ChangeNewVmpParam(TVMPParam *ptVMPParam, BOOL32 bStart)
{
	if (ptVMPParam == NULL)
	{
		return;
	}
   
	//tConfVMPParam只记录SchemeId, RimEnable等值, 对于成员状态没有参考价值。可考虑废除
	TVMPParam tConfVMPParam = m_tConf.m_tStatus.GetVmpParam();	

	CServMsg cServMsg;
	cServMsg.SetConfId(m_tConf.GetConfId());

    //设置CKDVNewVMPParam[x]参数
    CKDVNewVMPParam cKDVVMPParam;
    memset(&cKDVVMPParam, 0, sizeof(cKDVVMPParam));
    cKDVVMPParam.m_byVMPStyle = ptVMPParam->GetVMPStyle();
    cKDVVMPParam.m_byEncType = m_tConf.GetMainVideoMediaType();
    
	// xliang [2/25/2009] 老vmp该参数填GetMaxMemberNum，新的填实际参加VMP的成员数
	//cKDVVMPParam.m_byMemberNum  = ptVMPParam->GetMaxMemberNum(); 
	
	TMt tSpeakerMt = GetLocalSpeaker();
	//只允许终端进入通道，不允许放像设备
	if (tSpeakerMt.GetType() == TYPE_MCUPERI)
	{
		memset(&tSpeakerMt, 0, sizeof(TMt));
	}
	TMt tChairMt = m_tConf.GetChairman();
	
	u8 byMemberNum = ptVMPParam->GetMaxMemberNum();	//当前风格下能够参加的最大成员数
	u8 byMtId = 1;
	u8 byMember = 0;								//实际参加VMP的成员数
	u8 byLoop = 0;

	// 对于VCS会议画面合成成员选定
	if (VCS_CONF == m_tConf.GetConfSource())
	{
		TMt atVmpMember[MAXNUM_VMP_MEMBER];
		memset(atVmpMember, 0, sizeof(TMt) * MAXNUM_VMP_MEMBER);
		if (ptVMPParam->IsVMPAuto())
		{
			u8 byStyle = GetVCSAutoVmpMember(atVmpMember);
			if (byStyle != VMP_STYLE_NONE)
			{
				ptVMPParam->SetVMPStyle(byStyle);
				cKDVVMPParam.m_byVMPStyle = byStyle;
			}
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[ChangeVmpParam]only auto vmp to vcs currently");
			return;
		}
		
		for (; byLoop < ptVMPParam->GetMaxMemberNum(); byLoop++)
		{
			if (!atVmpMember[byLoop].IsNull() && m_tConfAllMtInfo.MtJoinedConf(atVmpMember[byLoop].GetMtId()))
			{
				cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(atVmpMember[byLoop].GetMtId()) );
				cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(atVmpMember[byLoop]);
				cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_AUDIENCE);
				ptVMPParam->SetVmpMember(byMember, cKDVVMPParam.m_atMtMember[byMember]);
				byMember ++;
			}
		}
		cKDVVMPParam.m_byMemberNum  = byMember;		
	}
	else
	{
		if(ptVMPParam->IsVMPAuto())
		{        
			//先填发言人，再填主席，再填观众
			if(!tSpeakerMt.IsNull() && m_tConfAllMtInfo.MtJoinedConf(tSpeakerMt.GetMtId()))
			{
				// 终端别名
				cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(tSpeakerMt.GetMtId()) );
				cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(tSpeakerMt);
				cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_SPEAKER);
				cKDVVMPParam.m_atMtMember[byMember].SetMemberType(VMP_MEMBERTYPE_SPEAKER);
				byMember ++;
			}
			if(!tChairMt.IsNull() &&
				!(tChairMt == tSpeakerMt) &&
				m_tConfAllMtInfo.MtJoinedConf(tChairMt.GetMtId()))
			{
				// 终端别名
				cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(tChairMt.GetMtId()) );
				cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(tChairMt);
				cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_CHAIRMAN);
				cKDVVMPParam.m_atMtMember[byMember].SetMemberType(VMP_MEMBERTYPE_CHAIRMAN);
				byMember ++;
			}
			for (byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
			{
				if (byMember >= byMemberNum)
				{
					// 数量满
					break;
				}
				if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
				{
					if (byMtId == m_tConf.GetChairman().GetMtId())
					{
						// 主席在前面
						continue;
					}
					if (byMtId == GetLocalSpeaker().GetMtId() &&
						GetLocalSpeaker().GetType() != TYPE_MCUPERI)
					{
						// 发言人在前面
						continue;
					}
					
					// 终端别名
					cKDVVMPParam.m_atMtMember[byMember].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(byMtId) );
					cKDVVMPParam.m_atMtMember[byMember].SetMemberTMt(m_ptMtTable->GetMt(byMtId));
					cKDVVMPParam.m_atMtMember[byMember].SetMemStatus(MT_STATUS_AUDIENCE);
					cKDVVMPParam.m_atMtMember[byMember].SetMemberType(VMP_MEMBERTYPE_VAC);
					byMember ++;
				}
			}
			cKDVVMPParam.m_byMemberNum = byMember; //自动VMP时，byMemberNum应该与终端数一致
			
			for(u8 byLp = 0; byLp < cKDVVMPParam.m_byMemberNum; byLp ++)
			{
				ptVMPParam->SetVmpMember(byLp, cKDVVMPParam.m_atMtMember[byLp]);
			}
		}
		else
		{
			for (byLoop = 0; byLoop < ptVMPParam->GetMaxMemberNum(); byLoop++)
			{
				cKDVVMPParam.m_atMtMember[byLoop].SetMember(*ptVMPParam->GetVmpMember(byLoop)); 
				
				TMt tTempMt = *(TMt *)ptVMPParam->GetVmpMember(byLoop);
				if( tTempMt.GetMtId() != 0 )
				{
					byMember ++;
				}
				if (tChairMt == tTempMt && ptVMPParam->GetVmpMember(byLoop)->GetMemberType() != VMP_MEMBERTYPE_SPEAKER )
				{
					cKDVVMPParam.m_atMtMember[byLoop].SetMemStatus(MT_STATUS_CHAIRMAN);
				}
				else if (tSpeakerMt == tTempMt)
				{
					cKDVVMPParam.m_atMtMember[byLoop].SetMemStatus(MT_STATUS_SPEAKER);
				}
				else
				{
					cKDVVMPParam.m_atMtMember[byLoop].SetMemStatus(MT_STATUS_AUDIENCE);
				}
				// 终端别名
				cKDVVMPParam.m_atMtMember[byLoop].SetMbAlias( m_ptMtTable->GetMtAliasFromExt(tTempMt.GetMtId()) );
			}
			
			//清掉不与会终端  // xliang [12/24/2008] FIXME?写在这何用意
			if( !m_tConfAllMtInfo.MtJoinedConf( cKDVVMPParam.m_atMtMember[byLoop].GetMtId()))
			{
				cKDVVMPParam.m_atMtMember[byLoop].SetNull();
			}
			cKDVVMPParam.m_byMemberNum = byMember;
		} 
	}

	//清掉其余通道的交换
	u8 byMaxVMPMember = MAXNUM_MPUSVMP_MEMBER;
	if (ptVMPParam->IsVMPAuto() || VCS_CONF == m_tConf.GetConfSource())
	{
		byLoop = byMember;	
	}
	else
	{
		byLoop = ptVMPParam->GetMaxMemberNum();
	}

	for (; byLoop < byMaxVMPMember; byLoop++)
	{
		StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
		ptVMPParam->ClearVmpMember(byLoop);
		//tConfVMPParam.ClearVmpMember(byLoop);// meaning?
	}

	//将合成成员更新到TPeriEqpStatus中
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = *ptVMPParam;
	g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	

	//填载荷类型等参数
	TLogicalChannel tChnnl;
	BOOL32 bRet = FALSE;
	for (byLoop = 0; byLoop < ptVMPParam->GetMaxMemberNum(); byLoop++)
	{
		bRet = m_ptMtTable->GetMtLogicChnnl(cKDVVMPParam.m_atMtMember[byLoop].GetMtId(), LOGCHL_VIDEO, &tChnnl, FALSE);
		if (bRet)
		{
			cKDVVMPParam.m_tVideoEncrypt[byLoop] = tChnnl.GetMediaEncrypt();
			cKDVVMPParam.m_tDoublePayload[byLoop].SetRealPayLoad(tChnnl.GetChannelType());
			cKDVVMPParam.m_tDoublePayload[byLoop].SetActivePayload(tChnnl.GetActivePayload());
		}
	}
	

	//填格式，码率，分辨率信息 
	u16 wMinMtReqBitrate = 0;
	if (0 != m_tConf.GetSecBitRate() && 
		MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
	{
		wMinMtReqBitrate = m_tConf.GetBitRate();
	}
	else
	{
		wMinMtReqBitrate = GetLeastMtReqBitrate(TRUE, cKDVVMPParam.m_byEncType);
//		m_wVMPBrdBitrate = wMinMtReqBitrate;
	}
	
	if (wMinMtReqBitrate < m_wVidBasBitrate || 0 == m_wVidBasBitrate)
	{
		m_wVidBasBitrate = wMinMtReqBitrate;
	}
	//cKDVVMPParam.SetBitRate(wMinMtReqBitrate);

	// xliang [5/8/2009] 填多出RES
	u16 wWidth = 0;
	u16 wHeight = 0;
	u8 byChnnlRes  = 0;
	u8 byMediaType = 0;
	u8 byVmpOutChnnl = 0;
	TPeriEqpStatus tVmpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpStatus);
    u8 byVmpSubType = tVmpStatus.m_tStatus.tVmp.m_bySubType;	
	u8 byBoardVer	= tVmpStatus.m_tStatus.tVmp.m_byBoardVer;

	for (byVmpOutChnnl; byVmpOutChnnl < MAXNUM_MPU_OUTCHNNL; byVmpOutChnnl ++)
	{
		
		if (byBoardVer == MPU_BOARD_A128 && byVmpOutChnnl == 3) //A板只填3路
		{
			ConfLog( FALSE, "MPU A version, only 3 out channel!\n" );
			break;
		}
		byChnnlRes = GetResByVmpOutChnnl(byMediaType, byVmpOutChnnl, m_tConf, m_tVmpEqp.GetEqpId());

		GetWHByRes(byChnnlRes, wWidth, wHeight);
		cKDVVMPParam.SetVideoWidth(wWidth);
		cKDVVMPParam.SetVideoHeight(wHeight);
		cKDVVMPParam.m_byEncType = byMediaType;
		
		wMinMtReqBitrate = GetLstRcvGrpMtBr(TRUE, byMediaType, byChnnlRes);
		//wMinMtReqBitrate = GetLeastMtReqBitrate(TRUE, cKDVVMPParam.m_byEncType);
		m_awVMPBrdBitrate[byVmpOutChnnl] = wMinMtReqBitrate;

		cKDVVMPParam.SetBitRate(wMinMtReqBitrate);

// 		if(g_bPrintEqpMsg)
// 		{
// 			OspPrintf(TRUE, FALSE, "[ChangeNewVmpParam] vmp param[%u] is followed:\n",byVmpOutChnnl);
// 			cKDVVMPParam.Print();
// 		} 

        //zbq[07/27/2009] 是否强制编码输出1080i, 放弃720p
        if (g_cMcuVcApp.IsSVmpOutput1080i())
        {
            if (VIDEO_FORMAT_HD1080 == byChnnlRes)
            {
                cKDVVMPParam.SetVideoHeight(544);
            }
            else if (VIDEO_FORMAT_HD720 == byChnnlRes)
            {
                cKDVVMPParam.SetBitRate(0);
            }
        }
		
		// xliang [7/28/2009] 会议如果没有高清能力，则把SVMP当EVPU用
		if( VIDEO_FORMAT_HD1080 != m_tConf.GetMainVideoFormat() 
			&& VIDEO_FORMAT_HD720 != m_tConf.GetMainVideoFormat() )
		{
			if( VIDEO_FORMAT_HD1080 == byChnnlRes || VIDEO_FORMAT_HD720 == byChnnlRes )
			{
				cKDVVMPParam.SetBitRate(0);
			}
		}
		cServMsg.CatMsgBody((u8 *)&cKDVVMPParam, sizeof(cKDVVMPParam));
	}
    
	//是否需要PRS
	u8 byNeedPrs = m_tConf.GetConfAttrb().IsResendLosePack();
	cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs));

	//画面合成风格
    u8 bySchemeId = ptVMPParam->GetVMPSchemeId();
    TVmpStyleCfgInfo tMcuVmpStyle;    
    if(0 == bySchemeId)
    {
        EqpLog("[ChangeVmpParam] SchemeId :%d, use default\n", bySchemeId);
        tMcuVmpStyle.ResetDefaultColor();
        
        //zbq[05/08/2009] 高清VMP 背景默认为黑
        if (VMP != byVmpSubType)
        {
            tMcuVmpStyle.SetBackgroundColor(0);
        }
    }  
    else
    {		
        u8  byVmpStyleNum = 0;
        TVmpAttachCfg atVmpStyle[MAX_VMPSTYLE_NUM];
        if (SUCCESS_AGENT == g_cMcuAgent.ReadVmpAttachTable(&byVmpStyleNum, atVmpStyle) && 
            bySchemeId <= MAX_VMPSTYLE_NUM) // 顾振华 [4/30/2006] 这里应该检查是否越界即可
        {
            //zbq[04/02/2008] 取对应方案号的方案，而不是直接取索引
            u8 byStyleNo = 0;
            for( ; byStyleNo < byVmpStyleNum && byStyleNo < MAX_VMPSTYLE_NUM; byStyleNo ++ )
            {
                if ( bySchemeId == atVmpStyle[byStyleNo].GetIndex() )
                {
                    break;
                }
            }
            if ( byVmpStyleNum == byStyleNo ||
                 MAX_VMPSTYLE_NUM == byStyleNo )
            {
                tMcuVmpStyle.ResetDefaultColor();
                ConfLog( FALSE, "[ChangeVmpParam] Get vmp cfg failed! use default(NoIdx)\n");                
            }
            else
            {
                tMcuVmpStyle.SetBackgroundColor(atVmpStyle[byStyleNo].GetBGDColor());
                tMcuVmpStyle.SetFrameColor(atVmpStyle[byStyleNo].GetFrameColor());
                tMcuVmpStyle.SetChairFrameColor(atVmpStyle[byStyleNo].GetChairFrameColor());
                tMcuVmpStyle.SetSpeakerFrameColor(atVmpStyle[byStyleNo].GetSpeakerFrameColor());
                tMcuVmpStyle.SetSchemeId(atVmpStyle[byStyleNo].GetIndex());
                tMcuVmpStyle.SetFontType( atVmpStyle[byStyleNo].GetFontType() );
                tMcuVmpStyle.SetFontSize( atVmpStyle[byStyleNo].GetFontSize() );
                tMcuVmpStyle.SetTextColor( atVmpStyle[byStyleNo].GetTextColor() );
                tMcuVmpStyle.SetTopicBkColor( atVmpStyle[byStyleNo].GetTopicBGDColor() );
                tMcuVmpStyle.SetDiaphaneity( atVmpStyle[byStyleNo].GetDiaphaneity() );
                
                EqpLog("[ChangeVmpParam] get vmpstyle info success, SchemeId :%d\n", bySchemeId);

            }
        }
        else
        {
            tMcuVmpStyle.ResetDefaultColor();
            OspPrintf(TRUE, FALSE, "[ChangeVmpParam] Get vmp cfg failed! use default\n");
        }
    }
    EqpLog("[ChangeVmpParam] GRDColor.0x%x, AudFrmColor.0x%x, ChairFrmColor.0x%x, SpeakerFrmColor.0x%x\n",
            tMcuVmpStyle.GetBackgroundColor(),
            tMcuVmpStyle.GetFrameColor(),
			
            tMcuVmpStyle.GetChairFrameColor(),
            tMcuVmpStyle.GetSpeakerFrameColor() );
	
    tMcuVmpStyle.SetIsRimEnabled( ptVMPParam->GetIsRimEnabled() );

    tConfVMPParam.SetVMPSchemeId(bySchemeId);
	tConfVMPParam.SetIsRimEnabled( ptVMPParam->GetIsRimEnabled() );

    m_tConf.m_tStatus.SetVmpParam(tConfVMPParam);

    cServMsg.CatMsgBody((u8*)&tMcuVmpStyle, sizeof(tMcuVmpStyle));


	//print:
	if(g_bPrintEqpMsg)
	{
		EqpLog("PeriEqpStatus vmpParam is follow:\n===========\n");
		ptVMPParam->Print();
	}


// 	EqpLog("ConfVmpParam is follow:\n===============\n");
// 	tConfVMPParam.Print();

	//发消息
	if (bStart)
	{
		SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, 6*1000);
		SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_STARTVIDMIX_REQ, cServMsg);
	}
	else
	{
		SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_CHANGEVIDMIXPARAM_REQ, cServMsg);
	}

    m_tConfInStatus.SetVmpNotify(FALSE);
}

BOOL32 CMcuVcInst::VidResAdjust(u8 byVmpStyle, u8 byPos, u8 byMtStandardFormat, u8 &byReqRes)
{
	u16 wBR = 0;		//bitrate
	s32 nResCmpRst = 3;	//3 indicate no meaning for comparing resolution
	BOOL32 bNeedAdjRst = FALSE;
	
	/*
	if(byVmpSubType == MPU_SVMP)
	{
		if( (byMtStandardFormat == VIDEO_FORMAT_HD1080 || byMtStandardFormat == VIDEO_FORMAT_HD720)
			&& (byVmpStyle == VMP_STYLE_HTWO 
			|| byVmpStyle == VMP_STYLE_VTWO
			|| byVmpStyle == VMP_STYLE_THREE
			|| byVmpStyle == VMP_STYLE_FOUR 
			|| byVmpStyle == VMP_STYLE_SPECFOUR )
			)
		{
			return FALSE;
		}
	}
	else if(byVmpSubType == MPU_DVMP)
	{
		//FIXME
	}
	else //EVPU
	{
		//FIXME
	}
	*/

	if((byMtStandardFormat == VIDEO_FORMAT_HD1080 || byMtStandardFormat == VIDEO_FORMAT_HD720)
		/*&& byVmpSubType != MPU_EVPU*/ )
	{
		g_cMcuVcApp.m_cVmpMemVidLmt.GetMtVidLmtForHd(byVmpStyle, byPos, byReqRes, wBR);
	}
	else if( byMtStandardFormat == VIDEO_FORMAT_4CIF || byMtStandardFormat == VIDEO_FORMAT_CIF
		/*|| byVmpSubType == MPU_EVPU*/ )
	{
		g_cMcuVcApp.m_cVmpMemVidLmt.GetMtVidLmtForStd(byVmpStyle, byPos, byReqRes, wBR);	
	}
	
	nResCmpRst = ResWHCmp(byMtStandardFormat,byReqRes);
	
	if( nResCmpRst == 2 //value 2 indicates the former resolution is bigger than the latter both in width and height.
		|| nResCmpRst == -1 //value -1 indicates the former resolution is only bigger in height
		|| nResCmpRst == 1	//1 indicates the former is only bigger in width
		)
	{
		bNeedAdjRst = TRUE;
	}

	EqpLog( "Original Res :%s,",GetResStr(byMtStandardFormat));
	EqpLog( "require Res: %s,", GetResStr(byReqRes));
	EqpLog("Compare result is: %d \n", nResCmpRst);

	return bNeedAdjRst;
}
/*==============================================================================
函数名    :  AdjustVmpParambyMemberType
功能      :  VMP发言人跟随，轮询跟随调整
算法实现  :  新的发言人已经在vmp某个不是发言人跟随的通道里，则将其挪位到发言人跟随通道，
		     原来占的通道位置清空,并做历史记录，待下次又有新发言人做恢复
参数说明  :  
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-3-20					薛亮							create
==============================================================================*/
void CMcuVcInst::AdjustVmpParambyMemberType(TMt *ptSpeaker, TVMPParam &tVmpParam, u8 byMemberType ,TMt *ptOldSrc)
{
	TMt tMt;
	TVMPMember tVmpMember;
	u8 byLoop = 0;
	u8 bytmpLastSpeakerVmpChnnl = ~0; //初始化为无意义值
	u8 bytmpLastVmpMemberType	= 0;
	u8 byMaxMemberNum			= tVmpParam.GetMaxMemberNum();

	//只允许终端进入通道，不允许放像设备
	if (ptSpeaker == NULL || ptSpeaker->GetType() == TYPE_MCUPERI)
	{ 
		tMt.SetNull();
	}
	else
	{
		tMt = *ptSpeaker;
	}
	for (byLoop = 0; byLoop < byMaxMemberNum; byLoop++)
	{
		//新的发言人已经在vmp某个不是发言人跟随的通道里，则将其挪位到发言人跟随通道
		//原来占的通道位置清空,并做历史记录，待下次又有新发言人做恢复
		tVmpMember = *tVmpParam.GetVmpMember(byLoop);
		if( !tMt.IsNull()
			&& (tVmpMember.GetMtId() == tMt.GetMtId())
			&& (tVmpMember.GetMemberType() != byMemberType)
			)
		{
			
			StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
			tVmpParam.ClearVmpMember(byLoop);
			bytmpLastSpeakerVmpChnnl = byLoop;	//因为时序不定，先存于临时变量
			bytmpLastVmpMemberType = tVmpMember.GetMemberType(); //membertype 也保留
		}
		
		if ( tVmpMember.GetMemberType() == byMemberType )
		{
			//发言人跟随通道先清空
			StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO);
			tVmpParam.ClearVmpMember(byLoop);
			
			//之前存在该让位MT的历史通道记录，则让该MT恢复到历史通道中
			if(m_tLastVmpChnnlMemInfo.GetLastVmpMemChnnl() < MAXNUM_MPUSVMP_MEMBER) 
			{
				//历史通道中成员为空才恢复
				if( (tVmpParam.GetVmpMember(m_tLastVmpChnnlMemInfo.GetLastVmpMemChnnl())->IsNull())
					&& (ptOldSrc != NULL && !ptOldSrc->IsNull())
					&& (!tVmpParam.IsMtInMember(*ptOldSrc)) //也不能与其他通道里的重复
					)
				{
					tVmpMember.SetMemberTMt(*ptOldSrc); //恢复的就只可能是旧发言人
					tVmpMember.SetMemberType(m_tLastVmpChnnlMemInfo.GetLastVmpMemType());
					tVmpParam.SetVmpMember(m_tLastVmpChnnlMemInfo.GetLastVmpMemChnnl(), tVmpMember);
				}
			}
			
			// 新发言人放到发言人跟随通道中
			tVmpMember.SetMemberTMt(tMt);
			tVmpMember.SetMemberType(byMemberType);
			tVmpParam.SetVmpMember(byLoop, tVmpMember);

		}
	}

	m_tLastVmpChnnlMemInfo.SetLastVmpMemChnnl(bytmpLastSpeakerVmpChnnl);
	m_tLastVmpChnnlMemInfo.SetLastVmpMemType(bytmpLastVmpMemberType);

	EqpLog("[AdjustVmpParambyMemberType] m_byLastVmpMemInChnnl:%u\n",bytmpLastSpeakerVmpChnnl);

}

/*==============================================================================
函数名    :  AssignPrsChnnl4HdConf
功能      :  分配PRS资源 （高清会议用）
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-4-30	4.6				薛亮							create
==============================================================================*/
BOOL32 CMcuVcInst::AssignPrsChnnl4HdConf()
{
	u8 byEqpId;
	u8 byChnIdx;
	TPeriEqpStatus tStatus;
	u8 byNeedPrsChls = DEFAULT_PRS_CHANNELS;

	// HD会议MAX预留PRS的通道数：缺省的3路 + 广播源MAX 4路 + 适配音频1路 + 适配双流1路  （Max总通道占9路）

	//1, 确定PRS通道预留数目, VMP 4出均预留
/*	//广播源MAX 4出[1080][720][4cif][cif/other],根据会议格式分辨率确定具体数目
	u8 byBrd1080Chnnl	= EQP_CHANNO_INVALID;
	u8 byBrd720Chnnl	= EQP_CHANNO_INVALID;
	u8 byBrd4CifChnnl	= EQP_CHANNO_INVALID;
	u8 byBrdCifOthChnnl	= EQP_CHANNO_INVALID;
	u8 byBrdOtherChnnl	= EQP_CHANNO_INVALID;
	
	if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType())
	{
		if( VIDEO_FORMAT_HD1080 == m_tConf.GetMainVideoFormat())
		{
			byNeedPrsChls += 1;
			byBrd1080Chnnl = 1;
			
			if (m_tConf.GetConfAttrbEx().IsResEx720())
			{	
				byNeedPrsChls += 1;
				byBrd720Chnnl = 1;
			}
			if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
			{
				byNeedPrsChls += 1;
				byBrd4CifChnnl = 1;
			}
			if (m_tConf.GetConfAttrbEx().IsResExCif())
			{
				byNeedPrsChls += 1;
				byBrdCifOthChnnl = 1;
			}
		}
		else if (VIDEO_FORMAT_HD720 == m_tConf.GetMainVideoFormat())
		{
			byNeedPrsChls += 1;
			byBrd720Chnnl = 1;
			
			if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
			{
				byNeedPrsChls += 1;
				byBrd4CifChnnl = 1;
			}
			if (m_tConf.GetConfAttrbEx().IsResExCif())
			{
				byNeedPrsChls += 1;
				byBrdCifOthChnnl = 1;
			}
		}
		else if ( VIDEO_FORMAT_4CIF == m_tConf.GetMainVideoFormat())
		{
			byNeedPrsChls += 1;
			byBrd4CifChnnl = 1;
			
			if (m_tConf.GetConfAttrbEx().IsResExCif())
			{
				byNeedPrsChls += 1;
				byBrdCifOthChnnl = 1;
			}
		}
		else if (VIDEO_FORMAT_CIF == m_tConf.GetMainVideoFormat())
		{
			byNeedPrsChls += 1;
			byBrdCifOthChnnl = 1;
		}
		else
		{
			//do nothing
		}
	}
	else //非h264格式 
	{
		if( EQP_CHANNO_INVALID == byBrdCifOthChnnl ) //other格式的和CIF不并存，所以复用
		{
			byNeedPrsChls += 1;
			byBrdCifOthChnnl = 1;
		}
	}
*/	
	byNeedPrsChls += MAXNUM_MPU_OUTCHNNL;
	//适配音频 FIXME
	
	
	//适配双流 FIXME
	
	//2, PRS通道数目确定后，进行各通道分配
	TPrsChannel tPrsChannel;
	if( g_cMcuVcApp.GetIdlePrsChls( byNeedPrsChls, tPrsChannel) )
	{
		byEqpId = tPrsChannel.GetPrsId();
		m_tPrsEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_PRS);
		m_tPrsEqp.SetConfIdx(m_byConfIdx);
		byChnIdx = 0;
		m_byPrsChnnl = tPrsChannel.m_abyPrsChannels[byChnIdx++];
		m_byPrsChnnl2 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
		m_byPrsChnnlAud = tPrsChannel.m_abyPrsChannels[byChnIdx++];
		
		g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
		//目前一个PRS可以为多个会议服务，因此修改为在每个PRS通道中记录该通道服务的会议索引
		//zgc, 2007/04/24
		tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnl, m_byConfIdx );
		tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnl2, m_byConfIdx );
		tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlAud, m_byConfIdx );
		
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl].SetReserved(TRUE);
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl2].SetReserved(TRUE);
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAud].SetReserved(TRUE);
		
		m_byPrsChnnlVmpOut1 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut1].SetReserved(TRUE);
		tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut1, m_byConfIdx );

		m_byPrsChnnlVmpOut2 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut2].SetReserved(TRUE);
		tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut2, m_byConfIdx );

		m_byPrsChnnlVmpOut3 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut3].SetReserved(TRUE); 
		tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut3, m_byConfIdx );

		m_byPrsChnnlVmpOut4 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut4].SetReserved(TRUE);
		tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut4, m_byConfIdx );

		/*
		if(EQP_CHANNO_INVALID != byBrd1080Chnnl)
		{
			m_byPrsChnnlVmpOut1 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut1].SetReserved(TRUE);
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut1, m_byConfIdx );
		}                  
		
		if(EQP_CHANNO_INVALID != byBrd720Chnnl)
		{
			m_byPrsChnnlVmpOut2 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut2].SetReserved(TRUE);
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut2, m_byConfIdx );
		}       
		
		if(EQP_CHANNO_INVALID != byBrd4CifChnnl)
		{
			m_byPrsChnnlVmpOut3 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut3].SetReserved(TRUE); 
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut3, m_byConfIdx );
		}
		
		if(EQP_CHANNO_INVALID != byBrdCifOthChnnl)
		{
			m_byPrsChnnlVmpOut4 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut4].SetReserved(TRUE);
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVmpOut4, m_byConfIdx );
		}
		*/
		// 适配的2路 FIXME
		//  ...
		
		g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
		
		ConfLog(FALSE, "Prs eqp %d chl %d, chl2 %d, chlaud %d, chl1080 %d, chl720 %d, ch4Cif %d, chCifOther %d,\
			chlAudBas %d, chlDsBas %d reserved for conf %s\n",
			byEqpId, 
			m_byPrsChnnl, m_byPrsChnnl2, m_byPrsChnnlAud, 
			m_byPrsChnnlVmpOut1, m_byPrsChnnlVmpOut2, m_byPrsChnnlVmpOut3, m_byPrsChnnlVmpOut4,
			m_byPrsChnnlAudBas, m_byPrsChnnlDsVidBas, 
			m_tConf.GetConfName());

		return TRUE;
	}	
	else
	{
		return FALSE;
	}

}

/*==============================================================================
函数名    :  AssignPrsChnnl4SdConf
功能      :  分配PRS资源 （标清会议用）
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-4-30	4.6				薛亮							create
==============================================================================*/
BOOL32 CMcuVcInst::AssignPrsChnnl4SdConf( void )
{
	u8 byEqpId;
	u8 byChnIdx;
	TPeriEqpStatus tStatus;

	u8 byNeedPrsChls = DEFAULT_PRS_CHANNELS;
	
	if(EQP_CHANNO_INVALID != m_byVidBasChnnl)
	{
		byNeedPrsChls += 1;
	}
	if(EQP_CHANNO_INVALID != m_byBrBasChnnl)
	{
		byNeedPrsChls += 1;
	}
	if(EQP_CHANNO_INVALID != m_byAudBasChnnl)
	{
		byNeedPrsChls += 1;
	}
	
	// if (g_cMcuVcApp.GetIdlePrsChl(byEqpId, byChnIdx, byChnIdx2, byChnIdxAud))
	TPrsChannel tPrsChannel;
	if( g_cMcuVcApp.GetIdlePrsChls( byNeedPrsChls, tPrsChannel) )
	{
		byEqpId = tPrsChannel.GetPrsId();
		m_tPrsEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_PRS);
		m_tPrsEqp.SetConfIdx(m_byConfIdx);
		byChnIdx = 0;
		m_byPrsChnnl = tPrsChannel.m_abyPrsChannels[byChnIdx++];
		m_byPrsChnnl2 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
		m_byPrsChnnlAud = tPrsChannel.m_abyPrsChannels[byChnIdx++];
		
		g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
		//目前一个PRS可以为多个会议服务，因此修改为在每个PRS通道中记录该通道服务的会议索引
		//zgc, 2007/04/24
		tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnl, m_byConfIdx );
		tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnl2, m_byConfIdx );
		tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlAud, m_byConfIdx );
		
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl].SetReserved(TRUE);
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl2].SetReserved(TRUE);
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAud].SetReserved(TRUE);
		//g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
		
		if(EQP_CHANNO_INVALID != m_byVidBasChnnl)
		{
			m_byPrsChnnlVidBas = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			//g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVidBas].SetReserved(TRUE);
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlVidBas, m_byConfIdx );
			//tStatus.SetConfIdx(m_byConfIdx);                        
			//g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
		}                  
		
		if(EQP_CHANNO_INVALID != m_byBrBasChnnl)
		{
			m_byPrsChnnlBrBas = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			//g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlBrBas].SetReserved(TRUE);
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlBrBas, m_byConfIdx );
			//tStatus.SetConfIdx(m_byConfIdx);                        
			//g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
		}   
		
		if(EQP_CHANNO_INVALID != m_byAudBasChnnl)
		{
			m_byPrsChnnlAudBas = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			//g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAudBas].SetReserved(TRUE);
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlAudBas, m_byConfIdx );
			//tStatus.SetConfIdx(m_byConfIdx);                        
			//g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
		}                                        
		g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
		
		ConfLog(FALSE, "Prs eqp %d chl %d, chl2 %d, chlaud %d, chlVidBas %d, chlBrBas %d, chlAudBas %d reserved for conf %s\n",
			byEqpId, m_byPrsChnnl,m_byPrsChnnl2,m_byPrsChnnlAud, m_byPrsChnnlVidBas, 
			m_byPrsChnnlBrBas, m_byPrsChnnlAudBas, m_tConf.GetConfName());

		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

/*==============================================================================
函数名    :  AssignPrsChnnl
功能      :  分配PRS资源
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-4-30	4.6				薛亮							create
==============================================================================*/
BOOL32 CMcuVcInst::AssignPrsChnnl( void )
{
	
	// HD会议MAX预留PRS的通道数：缺省的3路 + 广播源MAX 4路 + 适配音频1路 + 适配双流1路  （Max总通道占9路）
	if ( IsHDConf(m_tConf) )
	{
		return AssignPrsChnnl4HdConf();
	}
	else	//标清会议的Prs资源预留
	{
		return AssignPrsChnnl4SdConf();
	}
	
}
/*=============================================================================
函 数 名： GetMtRealSrc
功    能： 获取指定终端的实际接收源
算法实现： 
全局变量： 
参    数：
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
090423      4.0	        fxh					  创建
=============================================================================*/
BOOL32 CMcuVcInst::GetMtRealSrc(u8 byMtId, u8 byMode, TMt& tMtSrc, u8& byOutChnl)
{
	tMtSrc.SetNull();
	byOutChnl = 0;
	if (m_ptMtTable->GetMtSrc(byMtId, &tMtSrc, byMode) &&
		(tMtSrc.IsLocal() ? (tMtSrc.GetMtId() != byMtId) : (tMtSrc.GetMcuId() != byMtId)))
	{
		u8 byType = tMtSrc.GetType();
		if (TYPE_MT == byType)
		{
			TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(tMtSrc.GetMtId());
			TSimCapSet tDstCap = m_ptMtTable->GetDstSCS(byMtId);
			if (MODE_AUDIO == byMode)
			{
				if (tSrcCap.GetAudioMediaType() != tDstCap.GetAudioMediaType() &&
					!m_tAudBasEqp.IsNull())
				{
					tMtSrc = m_tAudBasEqp;
					byOutChnl = m_byAudBasChnnl;
					return TRUE;
				}
			}else if (MODE_VIDEO == byMode)
			{
				if (tSrcCap != tDstCap)
				{
					if (tMtSrc == m_tVidBrdSrc)
					{
					    TEqp tBas;
						u8 byFindSrc   = FALSE;
						u8 byInChnlId  = 0;
						u8 byOutChnlId = 0;
						u8 byAdpType   = ADAPT_TYPE_NONE;
						if (m_cMtRcvGrp.IsMtNeedAdp(byMtId))
						{
							u8 byRes = 0;
							u8 byMediaType = MEDIA_TYPE_NULL;
							if (m_cMtRcvGrp.GetMtMediaRes(byMtId, byMediaType, byRes) &&
								m_cBasMgr.GetBasResource(byMediaType, byRes, tBas, byInChnlId, byOutChnlId))
							{
								byFindSrc = TRUE;
							}
							else
							{
								u8 byDstType = tDstCap.GetVideoMediaType();
								u8 byDstRes  = tDstCap.GetVideoResolution();
								if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
									MEDIA_TYPE_H264 == byDstType)
								{
									while (GetProximalGrp(byDstType, byDstRes, byDstType, byDstRes))
									{
										if (m_cBasMgr.GetBasResource(byDstType, byDstRes,
											                         tBas, byInChnlId, byOutChnlId))
										{
											byFindSrc = TRUE;
											break;
										}
										
									}
								}
							}

							if (byFindSrc)
							{
								tMtSrc = tBas;
								byOutChnl = byInChnlId * MAXNUM_VOUTPUT + byOutChnlId;
								EqpLog("[GetMtRealSrc] hdbas(id:%d, outchnl:%d)--->dstmt(%d)\n",
								        tMtSrc.GetEqpId(), byOutChnl, byMtId);
							    return TRUE;	
							}
						}
						// 请求终端是否在标清适配中
						else if (IsMtSrcBas(byMtId, MODE_VIDEO, byAdpType))
						{
							if (ADAPT_TYPE_VID == byAdpType)
							{
								tMtSrc = m_tVidBasEqp;
								byOutChnl = m_byVidBasChnnl;
							}
							else if (ADAPT_TYPE_BR == byAdpType)
							{
								tMtSrc = m_tBrBasEqp;
								byOutChnl = m_byBrBasChnnl;
							}            
							else if (ADAPT_TYPE_CASDVID == byAdpType)
							{
								tMtSrc = m_tCasdVidBasEqp;
								byOutChnl = m_byCasdVidBasChnnl;
							}
							EqpLog("[GetMtRealSrc] sdbas(id:%d, outchnl:%d)--->dstmt(%d)\n",
								   tMtSrc.GetEqpId(), byOutChnl, byMtId);
							return TRUE;	
						}
						else
						{
							EqpLog("[GetMtRealSrc] Fail to find actual broadbassrc for mt%d\n",
								   byMtId);
						}
					}
					else
					{
						u8 bySelSrcId = (tMtSrc.IsLocal() ? tMtSrc.GetMtId() : tMtSrc.GetMcuId());
						TEqp tBasEqp;
						u8 byChnl;
						if (m_cSelChnGrp.IsMtInSelAdpGroup(bySelSrcId, byMtId, &tBasEqp, &byChnl))
						{
							return TRUE;
						}						
					}

				}
			}
			else if (MODE_SECVIDEO == byMode)
			{
			}
			else
			{
				
			}
		}
		else if (TYPE_MCUPERI == byType)
		{
		}
		else
		{
			ConfLog(FALSE, "[GetMtRealSrc]unproc src type(%d)\n", byType);
		}
	}
	return FALSE;

}
/*=============================================================================
函 数 名： ProcMixStopSucRsp
功    能： 
算法实现： 
全局变量： 
参    数：
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
090423      4.0	        fxh					  提取停止混音成功业务响应函数
=============================================================================*/
void CMcuVcInst::ProcMixStopSucRsp(void)
{
	CServMsg cServMsg;
	if (m_tConf.m_tStatus.IsVACing())//激励
	{
        m_tConf.m_tStatus.SetVACing(FALSE);            
		//通知主席及所有会控
		SendMsgToAllMcs( MCU_MCS_STOPVAC_NOTIF, cServMsg );
		if (HasJoinedChairman())
		{
			SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cServMsg );
		}

		//停止向混音器交换数据，并恢复收听广播源
		for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
		{
			if (m_tConfAllMtInfo.MtJoinedConf(byMtId))            
			{
				StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
							        (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(byMtId)), FALSE, MODE_AUDIO );
			
                // xsl [7/29/2006]
                m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);
            }
		}
	}
	
	if(m_tConf.m_tStatus.IsMixing())//混音(讨论)
	{
        m_tConf.m_tStatus.SetNoMixing();

		TMcu tMcu;
		tMcu.SetMcuId(LOCAL_MCUID);
		cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
		if (!m_tCascadeMMCU.IsNull())
		{
			cServMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
			SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_STOPMIXER_NOTIF, cServMsg);
		}
		if (HasJoinedChairman())
		{
			SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STOPDISCUSS_NOTIF, cServMsg );
		}
        
		//停止向混音器交换数据，并恢复收听广播源
		for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
		{
			if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
			{
				StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
							        (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(byMtId)), FALSE, MODE_AUDIO);

				StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);                       
                
                // xsl [9/21/2006] 恢复从组播地址接收
                if (m_tConf.GetConfAttrb().IsSatDCastMode() && m_ptMtTable->IsMtInMixGrp(byMtId))
                {
                    ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_AUDIO);
                }

                // xsl [7/29/2006]
                m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);

                // guzh [11/7/2007]
                m_ptMtTable->SetMtInMixing(byMtId, FALSE);

                // zbq [11/22/2007] 实时刷终端状态
                MtStatusChange(byMtId, TRUE);
			}
		}
        
        // guzh [5/12/2007] 清除映射表(防止残留)
        memset( m_abyMixMtId, 0, sizeof(m_abyMixMtId) );

		//改变视频源
		if (HasJoinedSpeaker())
		{
			TMt tSpeakerMt = GetLocalSpeaker();
			ChangeAudBrdSrc(&tSpeakerMt);
		}
		else
		{
			ChangeAudBrdSrc( NULL );
		}

		//通知混音器停止向终端发码流
		NotifyMixerSendToMt( FALSE );

		//向上级MCU发终端声音
		if (!m_tCascadeMMCU.IsNull())
		{
			TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
			if (NULL != ptInfo && 
				m_tConfAllMtInfo.MtJoinedConf(ptInfo->m_tSpyMt.GetMcuId(), ptInfo->m_tSpyMt.GetMtId()))
			{
				StartSwitchToSubMt( ptInfo->m_tSpyMt, 0, m_tCascadeMMCU.GetMtId(), MODE_AUDIO, SWITCH_MODE_SELECT);
			}
		}
	}
    //通知主席及所有会控 
    MixerStatusChange();

    //zbq[11/29/2007]通知上级MCU当前终端状态
    if ( !m_tCascadeMMCU.IsNull() )
    {
        OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId());
    }

	//放弃混音组
	ConfLog(FALSE, "give up mixer group.%u!\n",m_byMixGrpId);
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
	tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = TMixerGrpStatus::READY;
	g_cMcuVcApp.SetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
	cServMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
    SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
    
	m_tVacLastSpeaker.SetNull();

    // 如果是自动切换讨论-->混音
    if ( !m_tConf.IsDiscussAutoStop() && m_tConfInStatus.IsSwitchDiscuss2Mix() )
    {
        SwitchDiscuss2SpecMix();
    }
}
/*=============================================================================
函 数 名： ProcVMPStopSucRsp
功    能： 
算法实现： 
全局变量： 
参    数：
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
090423      4.0	        fxh					  提取停止画面合成业务处理
=============================================================================*/
void CMcuVcInst::ProcVMPStopSucRsp(void)
{
	//设置状态
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tPeriEqpStatus);
	tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;

	m_tConf.m_tStatus.SetVMPMode(CONF_VMPMODE_NONE);

	//停止画面复合后，VMP模板不再生效
	m_tConfInStatus.SetVmpModuleUsed(TRUE);
	// xliang [12/23/2008] 显式设置一下
	//m_tConf.m_tStatus.m_tVMPParam.SetVMPBatchPoll(0);

	//停止交换数据
	ChangeVmpSwitch( 0, VMP_STATE_STOP );

	TVMPParam tVMPParam;
	memset(&tVMPParam, 0, sizeof(tVMPParam));
	m_tConf.m_tStatus.SetVmpParam( tVMPParam );

	//同步外设复合状态
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
	g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );

	// xliang [5/4/2009] 外设复合状态上报会控
	CServMsg cServMsg;
	cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
	SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );

	//通知主席及所有会控
	cServMsg.SetMsgBody();
	cServMsg.SetMsgBody( (u8*)&tVMPParam, sizeof(TVMPParam) );
	SendMsgToAllMcs( MCU_MCS_STOPVMP_NOTIF, cServMsg );
	if( HasJoinedChairman() )
	{
		SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVMP_NOTIF, cServMsg );
	}

    SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
	
    m_tConfInStatus.SetVmpNotify(FALSE);

    // guzh [6/9/2007] 清除记录
    memcpy(&m_tLastVmpParam, &tVMPParam, sizeof(TVMPParam));

	// xliang [12/16/2008] 清空vmp channel所有信息。(对于老vmp，没有这个channel概念，这里一并清空)
	m_tVmpChnnlInfo.clear();
//			memset(m_abyMtVmpChnnl,0,sizeof(m_abyMtVmpChnnl));
	memset(m_abyMtNeglectedByVmp,0,sizeof(m_abyMtNeglectedByVmp));
	memset(&m_tVmpPollParam,0,sizeof(m_tVmpPollParam));
	memset(&m_tVmpBatchPollInfo,0,sizeof(m_tVmpBatchPollInfo));
	m_tLastVmpChnnlMemInfo.Init();
	//m_byLastVmpMemInChnnl = ~0;
	memset(m_atVMPTmpMember,0,sizeof(m_atVMPTmpMember));

	//m_tVmpCasMemInfo.Init();

    m_byVmpOperating = 0;

	memset(&m_tVmpEqp, 0, sizeof(m_tVmpEqp));

    //n+1备份更新vmp信息
    if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
    {
        ProcNPlusVmpUpdate();
    }

	//关闭选看该vmp的hdu通道
    TPeriEqpStatus tHduStatus;
    u8 byHduId = HDUID_MIN;
    while( byHduId >= HDUID_MIN && byHduId <= HDUID_MAX )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byHduId))
		{
			g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
            if (tHduStatus.m_byOnline == 1)
			{
                u8 byTmpMtId = 0;
                u8 byMtConfIdx = 0;
                u8 byMemberNum = tHduStatus.m_tStatus.tHdu.byChnnlNum;
				for(u8 byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; byLoop++)
				{
                    byTmpMtId = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetMtId();
                    byMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
					if (m_tVmpEqp.GetEqpId() == byTmpMtId && m_byConfIdx == byMtConfIdx)
					{
						StopSwitchToPeriEqp(byHduId, byLoop);
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = 0;
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
                    }
               	}
                g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);
		
                cServMsg.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
			}
		}
        byHduId++;
	}	
}
			
// END OF FILE

