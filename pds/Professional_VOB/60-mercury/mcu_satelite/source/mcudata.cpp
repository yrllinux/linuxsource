/*****************************************************************************
   模块名      : mcu
   文件名      : mcudata.cpp
   相关文件    : mcudata.h
   文件实现功能: MCU数据模块应用类普通函数定义
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/05/29  0.9         李屹        创建
   2005/02/19  3.6         万春雷      级联修改、与3.5版本合并
******************************************************************************/

#include "evmcumcs.h"
#include "evmcumt.h"
#include "evmcueqp.h"
#include "evmcu.h"
#include "evmp.h"
#include "mcuvc.h"
#include "mcudata.h"
#include "mcuerrcode.h"
#include "mcuutility.h"
#include "mtadpssn.h"
#include "mcsssn.h"
#include "vcsssn.h"

#if defined(_LINUX_)
#include "boardwrapper.h"
#else
#include "brddrvlib.h"
#endif

#define CHECK_CONFIDX(byConfIdx)                                    \
    if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)          \
    {                                                               \
        OspPrintf(TRUE, FALSE, "[CheckConfIdx] invalid confidx :%d\n", byConfIdx); \
        return FALSE;                                               \
    }

u32  g_dwVCUInterval;

/*--------------------------------------------------------------------------*/
/*                                CMcuVcData                                */
/*--------------------------------------------------------------------------*/

CMcuVcData::CMcuVcData()
{
	//清零
	ClearVcData();
    g_dwVCUInterval = 3*OspClkRateGet();    //3秒的tick数
}

CMcuVcData::~CMcuVcData()
{
    MCU_SAFE_DELETE(m_ptTemplateInfo);
}

void CMcuVcData::ClearVcData()
{
	memset( m_apConfInst, 0, sizeof( m_apConfInst ) );
	memset( m_atPeriEqpTable, 0, sizeof( m_atPeriEqpTable ) );
	memset( m_atMcTable, 0, sizeof( m_atMcTable ) );
	memset( m_atMpData, 0, sizeof(m_atMpData) );
	memset( m_atMtAdpData, 0, sizeof(m_atMtAdpData) );
	memset( m_atRecvMtPort, 0, sizeof(m_atRecvMtPort) );
	memset( m_atMulticastPort, 0, sizeof(m_atMulticastPort) );
	memset( m_abyConfStoreInfo, 0, sizeof(m_abyConfStoreInfo) );
	memset( m_abyConfRegState, 0, sizeof(m_abyConfRegState) );
	memset( (void*)&m_tGKID, 0, sizeof(m_tGKID) );
	memset( (void*)&m_tEPID, 0, sizeof(m_tEPID) );
	// memset( (void*)&m_tMcuDebugVal, 0, sizeof(m_tMcuDebugVal) ); // guzh 有构造函数,no memeset
	memset( m_atMtCallInterface, 0, sizeof(m_atMtCallInterface) );
	memset( m_atPeriDcsTable, 0, sizeof(m_atPeriDcsTable) );
	memset( m_abyConfRefreshTimes, 0, sizeof(m_abyConfRefreshTimes) );
	
	m_dwMakeConfIdTimes = 0;
	m_byRegGKDriId  = 0;
    m_byChargeRegOK = 0;
	m_dwMtCallInterfaceNum = 0;

	m_ptTemplateInfo = NULL;

	m_cVmpMemVidLmt.Init(); //VMP 各通道成员能力限制 
 	memset( m_adwEqpIp, 0, sizeof(m_adwEqpIp) );
   	
	return;
}

/*====================================================================
    函数名      ：IsPeriEqpConnected
    功能        ：获取外设连接状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId, 外设 ID
    返回值说明  ：TRUE: connected FALSE: unnnected
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/06    1.0         杨皞昀         创建
====================================================================*/
BOOL32 CMcuVcData::IsPeriEqpConnected( u8 byEqpId )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP )
	{
		return FALSE;
	}

	return( m_atPeriEqpTable[byEqpId - 1].m_tPeriEqpStatus.m_byOnline );
}

/*====================================================================
    函数名      ：GetPeriEqpLogicChnnl
    功能        ：获取外设的逻辑通道配置
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId, 外设 ID
				  u8 byMediaType, 媒体类型
				  TLogicalChannel * ptChnnl, 返回的逻辑通道配置
				  u8 * pbyChnnlNum, 返回的MCU至外设通道数
				  BOOL32 bForwardChnnl, 流向
    返回值说明  ：TRUE: success	FALSE: fail
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/06    1.0         杨皞昀         创建
====================================================================*/
BOOL32 CMcuVcData::GetPeriEqpLogicChnnl( u8 byEqpId, u8 byMediaType, u8 * pbyChnnlNum, TLogicalChannel * ptStartChnnl, BOOL32 bForwardChnnl )
{
	if( 0 == byEqpId || byEqpId > MAXNUM_MCU_PERIEQP || NULL == pbyChnnlNum || NULL == ptStartChnnl )
	{
		return FALSE;
	}

	switch( byMediaType )
	{
	case MODE_VIDEO:
		if( bForwardChnnl )
		{
			*pbyChnnlNum = m_atPeriEqpTable[byEqpId - 1].m_byFwdChannelNum;
			*ptStartChnnl = m_atPeriEqpTable[byEqpId - 1].m_tFwdVideoChannel;
		}
		else
		{
			*pbyChnnlNum = m_atPeriEqpTable[byEqpId - 1].m_byRvsChannelNum;
			*ptStartChnnl = m_atPeriEqpTable[byEqpId - 1].m_tRvsVideoChannel;
		}
		break;
	case MODE_AUDIO:
		if( bForwardChnnl )		
		{
			*pbyChnnlNum = m_atPeriEqpTable[byEqpId - 1].m_byFwdChannelNum;
			*ptStartChnnl = m_atPeriEqpTable[byEqpId - 1].m_tFwdAudioChannel;
		}
		else
		{
			*pbyChnnlNum = m_atPeriEqpTable[byEqpId - 1].m_byRvsChannelNum;
			*ptStartChnnl = m_atPeriEqpTable[byEqpId - 1].m_tRvsAudioChannel;
		}
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::GetPeriEqpLogicChnnl(): wrong Mode %u!\n", byMediaType );
		return( FALSE );
	}

	return( TRUE );
}

/*====================================================================
    函数名      ：SetEqpRtcpDstAddr
    功能        ：设置外设的通道目的Rtcp地址
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId, 外设 ID
				  u8 byChnnl, 通道号
				  u32 dwDstIp, 目的Ip
				  u16 wDstPort, 目的端口
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/10/19    3.0         胡昌威         创建
====================================================================*/
void CMcuVcData::SetEqpRtcpDstAddr(u8 byEqpId, u8 byChnnl, u32 dwDstIp, u16 wDstPort, u8 byMode)
{
    if (byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || byChnnl > MAXNUM_PERIEQP_CHNNL)
    {
        return;
    }

    if (MODE_VIDEO == byMode)
    {
        m_atPeriEqpTable[byEqpId - 1].m_tVideoRtcpDstAddr[byChnnl].SetIpAddr(dwDstIp);
        m_atPeriEqpTable[byEqpId - 1].m_tVideoRtcpDstAddr[byChnnl].SetPort(wDstPort);
    }
    else if (MODE_AUDIO == byMode)
    {
        m_atPeriEqpTable[byEqpId - 1].m_tAudioRtcpDstAddr[byChnnl].SetIpAddr(dwDstIp);
        m_atPeriEqpTable[byEqpId - 1].m_tAudioRtcpDstAddr[byChnnl].SetPort(wDstPort);
    }
}

/*====================================================================
    函数名      ：GetEqpRtcpDstAddr
    功能        ：得到外设的通道目的Rtcp地址
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId, 外设 ID
				  u8 byChnnl, 通道号
				  u32 dwDstIp, 目的Ip
				  u16 wDstPort, 目的端口
    返回值说明  ：TRUE: success	FALSE: fail
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/10/19    3.0         胡昌威         创建
====================================================================*/
BOOL32 CMcuVcData::GetEqpRtcpDstAddr(u8 byEqpId, u8 byChnnl, u32 &dwDstIp, u16 &wDstPort, u8 byMode)
{
    if (byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || byChnnl > MAXNUM_PERIEQP_CHNNL)
    {
        return FALSE;
    }

    if (MODE_VIDEO == byMode)
    {
        dwDstIp = m_atPeriEqpTable[byEqpId - 1].m_tVideoRtcpDstAddr[byChnnl].GetIpAddr();
        wDstPort = m_atPeriEqpTable[byEqpId - 1].m_tVideoRtcpDstAddr[byChnnl].GetPort();
    }
    else if (MODE_AUDIO == byMode)
    {
        dwDstIp = m_atPeriEqpTable[byEqpId - 1].m_tAudioRtcpDstAddr[byChnnl].GetIpAddr();
        wDstPort = m_atPeriEqpTable[byEqpId - 1].m_tAudioRtcpDstAddr[byChnnl].GetPort();
    }

    return ( dwDstIp == 0 || wDstPort == 0 ) ? FALSE : TRUE;
}

/*====================================================================
    函数名      ：GetPeriEqpStatus
    功能        ：获得外设状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId, 外设 ID
				  TPeriEqpStatus * ptStatus, 返回的外设状态
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/10    1.0         杨皞昀         创建
====================================================================*/
BOOL32 CMcuVcData::GetPeriEqpStatus( u8 byEqpId, TPeriEqpStatus * ptStatus )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || NULL == ptStatus )
	{
		return( FALSE );
	}

	*ptStatus = m_atPeriEqpTable[byEqpId - 1].m_tPeriEqpStatus;

	return( TRUE );
}
/*====================================================================
    函数名      ：SendPeriEqpStatusToMcs
    功能        ：发送指定外设状态给所有界面
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId:外设 ID
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/06/05                付秀华         创建
====================================================================*/
void CMcuVcData::SendPeriEqpStatusToMcs(u8 byEqpId)
{
	TPeriEqpStatus tEqpStatus;
	if (GetPeriEqpStatus(byEqpId, &tEqpStatus))
	{
		CServMsg cServMsg;
		cServMsg.SetMsgBody((u8 *)&tEqpStatus, sizeof(tEqpStatus));
		CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, 
			                          cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
	}

}
/*====================================================================
    函数名      ：IsMcConnected
    功能        ：获取MC连接状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wMcInstId, MC对应实例
    返回值说明  ：TRUE: connected FALSE: unnnected
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/06    1.0         杨皞昀         创建
====================================================================*/
BOOL32 CMcuVcData::IsMcConnected( u16 wMcInstId )
{
	if( 0 == wMcInstId || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) )
	{
		return FALSE;
	}

	return m_atMcTable[wMcInstId-1].m_bConnected;
}

/*====================================================================
    函数名      ：GetMcLogicChnnl
    功能        ：获取MC的逻辑通道配置
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wMcInstId, MC ID
				  u8 byMediaType, 媒体类型
				  TLogicalChannel * ptStartChnnl, 返回的逻辑通道配置
				   u8 * pbyChnnlNum, 返回的MCU至会控通道数
    返回值说明  ：TRUE: success FALSE: fail
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/06    1.0         杨皞昀         创建
====================================================================*/
BOOL32 CMcuVcData::GetMcLogicChnnl( u16 wMcInstId, u8 byMediaType, u8 * pbyChnnlNum, TLogicalChannel * ptStartChnnl )
{
	if( wMcInstId == 0 || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) || NULL == ptStartChnnl || NULL == pbyChnnlNum )
	{
		return FALSE;
	}

	*pbyChnnlNum = m_atMcTable[ wMcInstId-1 ].m_byFwdChannelNum;
	
	switch( byMediaType )
	{
	case MODE_VIDEO:
		*ptStartChnnl = m_atMcTable[ wMcInstId-1 ].m_tFwdVideoChannel;
		break;
	case MODE_AUDIO:
		*ptStartChnnl = m_atMcTable[ wMcInstId-1 ].m_tFwdAudioChannel;
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::GetMcLogicChnnl(): wrong Mode %u!\n", byMediaType );
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
    函数名      ：GetMcSrc
    功能        ：获取MC的发送/接收视频或音频源
    算法实现    ：
    引用全局变量：
    输入参数说明：uu16 wMcInstId, MC号
				  TMt * ptSrc, 返回的终端源
				  u8 byChnnlNo, 通道号
				  u8 byMode, 流模式MODE_VIDEO/MODE_AUDIO
    返回值说明  ：TRUE: success FALSE: fail
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/06    1.0         杨皞昀         创建
====================================================================*/
BOOL32 CMcuVcData::GetMcSrc( u16 wMcInstId, TMt * ptSrc, u8 byChnnlNo, u8 byMode )
{
	if( wMcInstId == 0 || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) || NULL == ptSrc || byChnnlNo >= MAXNUM_MC_CHANNL )
	{
		return FALSE;
	}

	switch( byMode )
	{
	case MODE_VIDEO:
		*ptSrc = m_atMcTable[ wMcInstId-1 ].m_atVidSrc[byChnnlNo];
		break;
	case MODE_AUDIO:
		*ptSrc = m_atMcTable[ wMcInstId-1 ].m_atAudSrc[byChnnlNo];
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::GetMcSrc(): wrong Mode %u!\n", byMode );
		return FALSE;	
	}

	return TRUE;
}

/*====================================================================
    函数名      ：GetPeriEqpSrc
    功能        ：获取外设的接收视频或音频源
    算法实现    ：
    引用全局变量：
    输入参数说明：u8  periEqp, 外设号
				  TMt * ptSrc, 返回的终端源
				  u8 byChnnlNo, 通道号
				  u8 byMode, 流模式MODE_VIDEO/MODE_AUDIO
    返回值说明  ：TRUE: success FALSE: fail
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/030   1.0         JQL         创建
====================================================================*/
BOOL32 CMcuVcData::GetPeriEqpSrc(u8 byEqpId, TMt * ptSrc, u8 byChnnlNo, u8 byMode )
{
	
	if( byEqpId == 0 || byEqpId > MAXNUM_PERIEQP_CHNNL 
		|| NULL == ptSrc || byChnnlNo >= MAXNUM_PERIEQP_CHNNL )
	{
		return FALSE;
	}

	switch( byMode )
	{
	case MODE_VIDEO:
		*ptSrc = m_atPeriEqpTable[ byEqpId - 1 ].m_atVidSrc[byChnnlNo];
		break;
	case MODE_AUDIO:
		*ptSrc = m_atPeriEqpTable[ byEqpId - 1 ].m_atAudSrc[byChnnlNo];
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::GetMcSrc(): wrong Mode %u!\n", byMode );
		return FALSE;	
	}
	return TRUE;
}

/*====================================================================
    函数名      ：MsgPassCheck
    功能        ：检查消息是否可以发给会议
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/20    3.0         胡昌威         创建
====================================================================*/
BOOL32 CMcuVcData::MsgPassCheck(u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	BOOL32 bReturn = TRUE;
	CServMsg cServMsg( pbyMsg, wLen );
	u8 byConfIdx, bySrcSsnId, byConfProtectMode;
	CConfId cConfId;
	TConfProtectInfo tProtectInfo;
    TConfMapData tMapData;
 
    switch(wEvent)
	{
	//任何情况下都可通过的消息
    case MCS_MCU_GETMCUSTATUS_CMD:
	case MCS_MCU_GETMCUSTATUS_REQ:	        //查询MCU状态请求 
	case MCS_MCU_LISTALLCONF_REQ:	        //列出该MCU上所有会议信息
	case MCS_MCU_STOPSWITCHMC_REQ:	        //会议控制台停止播放请求
	case MCS_MCU_STOP_SWITCH_TW_REQ:	    //会议控制台向外设停止播放请求
	case MCS_MCU_GETMCUPERIEQPSTATUS_REQ:	//查询MCU外设状态
	case MCS_MCU_GETRECSTATUS_REQ:	        //查询录像机状态请求
	case MCS_MCU_GETMIXERSTATUS_REQ:	    //查询混音器状态请求
	case MCS_MCU_CONNECT_REQ:		        //控制台通知MCU请求准入
	case MCS_MCU_CURRTIME_REQ :		        //控制台查询mcu当前时间
	case MCS_MCU_CREATECONF_REQ:            //会议控制台在MCU上创建一个会议 
	case MCS_MCU_ENTERCONFPWD_ACK:          //会议控制台回应MCU的密码请求   
	case MCS_MCU_ENTERCONFPWD_NACK:         //会议控制台拒绝MCU的密码请求
	case MCU_MCSCONNECTED_NOTIF:            //会议控制台建链
	case MCU_MCSDISCONNECTED_NOTIF:         //会议控制台断链
    case MCU_CREATECONF_FROMFILE:           //从文件创建会议
	case MCS_MCU_GETMTLIST_REQ:             //会议控制台发给MCU的得到终端列表请求
	case MCS_MCU_REFRESHMCU_CMD:            //会议控制台发给MCU的刷新其他MCU的命令
	case MCS_MCU_GETCONFINFO_REQ:           //会议控制台向MCU查询会议信息  
    case MCS_MCU_GETMAUSTATUS_REQ:          //会议控制台向MCU查询MAU信息  
	case MCS_MCU_GETPOLLPARAM_REQ:          //会议控制台向MCU查询会议轮询参数
    case MCS_MCU_GETTWPOLLPARAM_REQ:
	//case MCS_MCU_GETDISCUSSPARAM_REQ:     //查询讨论参数请求
    case MCS_MCU_GETMIXPARAM_REQ:           //查询讨论参数请求
	case MCS_MCU_GETVMPPARAM_REQ:			//查询视频复合成员请求
	case MCS_MCU_GETCONFSTATUS_REQ:         //会议控制台向MCU查询会议状态
	case MCS_MCU_GETMTSTATUS_REQ:           //会控向MCU查询终端状态
	case MCS_MCU_GETALLMTSTATUS_REQ:		//会控向MCU查询所有终端状态
	case MCS_MCU_GETMTALIAS_REQ:            //会控向MCU查询终端别名
	case MCS_MCU_GETALLMTALIAS_REQ:			//会控向MCU查询所有终端别名
	case MCS_MCU_GETLOCKINFO_REQ:           //得到会议独享消息
	case MCS_MCU_MCUMEDIASRC_REQ:
	case MCS_MCU_GETMCULOCKSTATUS_REQ:        
    case MCS_MCU_SETCHAIRMODE_CMD:          //会议控制台设置会议的主席方式  // guzh [7/25/2006] 即使锁定或者需要密码等也允许直接操作，因为这只是一个内部操作，非用户操作
	case MCS_MCU_SAVECONFTOTEMPLATE_REQ:	//会议控制台请求保存当前会议为会议模板(预留)  FIXME: 权限保护未考虑, zgc, 2007/04/20
    case MCS_MCU_GETMTEXTINFO_REQ:          //获取终端的二次扩展信息
	case MCS_MCU_STOP_SWITCH_HDU_REQ:       //停止HDU交换   4.6.1 新加版本  jlb
	case MCS_MCU_VMPPRISEIZE_ACK:			// xliang [12/12/2008] 
	case MCS_MCU_VMPPRISEIZE_NACK:
		break;
		
    //需要判断的消息
	case MCS_MCU_SENDRUNMSG_CMD:            //会议控制台命令MCU向终端发送短消息，终端号为0表示发给所有终端
	case MCS_MCU_STARTPOLL_CMD:             //会议控制台命令该会议开始轮询广播
	case MCS_MCU_STOPPOLL_CMD:              //会议控制台命令该会议停止轮询广播  
	case MCS_MCU_PAUSEPOLL_CMD:             //会议控制台命令该会议暂停轮询广播   
	case MCS_MCU_RESUMEPOLL_CMD:            //会议控制台命令该会议继续轮询广播
    case MCS_MCU_CHANGEPOLLPARAM_CMD:       //会议控制台命令该会议更新轮询列表
	case MCS_MCU_SPECPOLLPOS_REQ:			//会议控制台指定轮询位置
    case MCS_MCU_STARTTWPOLL_CMD:           //开始电视墙轮询
    case MCS_MCU_STOPTWPOLL_CMD:
    case MCS_MCU_PAUSETWPOLL_CMD:
    case MCS_MCU_RESUMETWPOLL_CMD:

    case MCS_MCU_STARTHDUPOLL_CMD:           //开始hdu轮询
    case MCS_MCU_STOPHDUPOLL_CMD:
    case MCS_MCU_PAUSEHDUPOLL_CMD:
    case MCS_MCU_RESUMEHDUPOLL_CMD:

	case MCS_MCU_SETMTBITRATE_CMD:          //会议控制台命令MCU调节终端码率
	case MCS_MCU_MTCAMERA_CTRL_CMD:         //会议控制台命令终端摄像机镜头运动
	case MCS_MCU_MTCAMERA_CTRL_STOP:        //会议控制台命令终端摄像机镜头停止运动
	case MCS_MCU_MTCAMERA_RCENABLE_CMD:     //会议控制台命令终端摄像机遥控使能	
	case MCS_MCU_MTCAMERA_SAVETOPOS_CMD:    //会议控制台命令终端摄像机将当前位置信息存入指定位置	
	case MCS_MCU_MTCAMERA_MOVETOPOS_CMD:    //会议控制台命令终端摄像机调整到指定位置
	
	case MCS_MCU_MTSEESPEAKER_CMD:          //强制广播发言人
    case MCS_MCU_ADDMIXMEMBER_CMD:          //改变混音成员
    case MCS_MCU_REMOVEMIXMEMBER_CMD:
	case MCS_MCU_SETMTVOLUME_CMD:			//会控实时设置终端音量,zgc,2006-12-26
	case MCS_MCU_REPLACEMIXMEMBER_CMD:      //替换混音成员
        cConfId = cServMsg.GetConfId();
        byConfIdx = GetConfIdx( cConfId );
        if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
        {
            OspPrintf(TRUE, FALSE, "[MsgPassCheck] invalid cConfId :");
            cConfId.Print();
            return FALSE;
        }	    
        tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
        if(!tMapData.IsValidConf()) 
        {
            OspPrintf(TRUE, FALSE, "[MsgPassCheck] invalid instance id :%d\n", tMapData.GetInsId());
            return FALSE;
        }
        GetConfProtectInfo( byConfIdx, &tProtectInfo );
        bySrcSsnId = cServMsg.GetSrcSsnId();
		byConfProtectMode = m_apConfInst[tMapData.GetInsId()-1]->m_tConf.m_tStatus.GetProtectMode();
		if( ( byConfProtectMode == CONF_LOCKMODE_LOCK && 
              (tProtectInfo.IsLockByMcs() && tProtectInfo.GetLockedMcSsnId() != bySrcSsnId || tProtectInfo.IsLockByMcu()) 
             ) || 
            ( byConfProtectMode == CONF_LOCKMODE_NEEDPWD && 
              !tProtectInfo.IsMcsPwdPassed(bySrcSsnId) 
            ) 
          )
		{         
            bReturn = FALSE;
			if(byConfProtectMode == CONF_LOCKMODE_NEEDPWD)
			{
				//问密码
				cServMsg.SetMsgBody();
                CMcsSsn::SendMsgToMcsSsn( bySrcSsnId, MCU_MCS_ENTERCONFPWD_REQ, 
					                          cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			}
			else
			{
                if ( tProtectInfo.IsLockByMcu() )				
                {
                    u8 byLockedMcuId = tProtectInfo.GetLockedMcuId();
                    TMtAlias tMtAlias;
                    m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable->GetMtAlias(byLockedMcuId, 
                        mtAliasTypeTransportAddress,
                        &tMtAlias);
                    
                    cServMsg.SetMsgBody( (u8*)&tMtAlias.m_tTransportAddr.m_dwIpAddr, sizeof(u32) );
                    if(!m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable->GetMtAlias(byLockedMcuId, 
                        mtAliasTypeH323ID,
                        &tMtAlias))
                    {
                        if(!m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable->GetMtAlias(byLockedMcuId, 
                            mtAliasTypeE164,
                            &tMtAlias))
                        {
                            strncpy( tMtAlias.m_achAlias, "mmcu", sizeof(tMtAlias.m_achAlias) );
                        }
                    }
                    tMtAlias.m_achAlias[MAXLEN_PWD-1] = 0;
                    cServMsg.CatMsgBody( (u8*)tMtAlias.m_achAlias, MAXLEN_PWD );										
				}
				else if( tProtectInfo.IsLockByMcs() && tProtectInfo.GetLockedMcSsnId() != bySrcSsnId )
                {
                    TMcsRegInfo tMcsRegInfo;
                    GetMcsRegInfo( tProtectInfo.GetLockedMcSsnId(), &tMcsRegInfo );
                    cServMsg.SetMsgBody( (u8*)&tMcsRegInfo.m_dwMcsIpAddr, sizeof(u32) );
                    cServMsg.CatMsgBody( (u8*)tMcsRegInfo.m_achUser, MAXLEN_PWD );
				}

				CMcsSsn::SendMsgToMcsSsn( bySrcSsnId, MCU_MCS_LOCKUSERINFO_NOTIFY, cServMsg.GetServMsg(), 
					            cServMsg.GetServMsgLen() );				
			}            
		}
        break;

	//需要判断并有可能要回应的消息
	case MCS_MCU_RELEASECONF_REQ:           //会议控制台请求MCU结束一个会议
	case MCS_MCU_CHANGECONFPWD_REQ:         //会议控制台请求MCU更改会议密码
	case MCS_MCU_SAVECONF_REQ:              //会议控制台请求MCU保存会议
	case MCS_MCU_MODIFYCONF_REQ:	        //会控修改预约会议
	case MCS_MCU_DELAYCONF_REQ:             //会议控制台请求MCU延长会议
	case MCS_MCU_CHANGEVACHOLDTIME_REQ:		//会议控制台请求MCU改变语音激励切换时间
	case MCS_MCU_CHANGECONFLOCKMODE_REQ:    //会议控制台请求MCU锁定会议
	case MCS_MCU_SPECCHAIRMAN_REQ:          //会议控制台指定一台终端为主席
	case MCS_MCU_CANCELCHAIRMAN_REQ:        //会议控制台取消当前会议主席
	case MCS_MCU_SPECSPEAKER_REQ:           //会议控制台指定一台终端发言
	case MCS_MCU_CANCELSPEAKER_REQ:         //会议控制台取消会议Speaker
    case MCS_MCU_SPECOUTVIEW_REQ:           //会议控制台指定回传通道
	case MCS_MCU_ADDMT_REQ:                 //会议控制台邀请终端入会	
	case MCS_MCU_DELMT_REQ:                 //会议控制台驱逐终端离会
	case MCS_MCU_STARTSWITCHMC_REQ:         //会控选看终端
	case MCS_MCU_STARTVAC_REQ:              //会议控制台请求MCU开始语音激励控制发言		
	case MCS_MCU_STOPVAC_REQ:               //会议控制台请求MCU停止语音激励控制发言
	case MCS_MCU_STARTDISCUSS_REQ:          //开始会议讨论请求 - 用于本级开始讨论操作
	case MCS_MCU_STOPDISCUSS_REQ:           //结束会议讨论请求 - 用于本级开始讨论操作

    //会议控制---点名
    case MCS_MCU_STARTROLLCALL_REQ:
    case MCS_MCU_STOPROLLCALL_REQ:
    case MCS_MCU_CHANGEROLLCALL_REQ:

    //vmp
	case MCS_MCU_STARTVMP_REQ:              //开始视频复合请求
	case MCS_MCU_STOPVMP_REQ:               //结束视频复合请求
	case MCS_MCU_CHANGEVMPPARAM_REQ:        //会议控制台请求MCU改变视频复合参数
    //vmptw
	case MCS_MCU_STARTVMPTW_REQ:            //开始复合电视墙请求
	case MCS_MCU_STOPVMPTW_REQ:             //结束复合电视墙请求
	case MCS_MCU_CHANGEVMPTWPARAM_REQ:      //会议控制台请求MCU改变复合电视墙参数
    //
	case MCS_MCU_CALLMT_REQ:                //会议控制台请求MCU呼叫终端
	case MCS_MCU_SETCALLMTMODE_REQ:         //会议控制台设置MCU呼叫终端方式
	case MCS_MCU_DROPMT_REQ:                //会议控制台挂断终端请求
	case MCS_MCU_STARTSWITCHMT_REQ:         //会控强制目的终端选看源终端
	case MCS_MCU_STOPSWITCHMT_REQ:          //会控取消目的终端选看源终端
	case MCS_MCU_SETMTVIDSRC_CMD:           //会控要求MCU设置终端视频源
	case MCS_MCU_MTAUDMUTE_REQ:             //会控要求MCU设置终端静音
	case MCS_MCU_MATRIX_SAVESCHEME_CMD:     //保存矩阵方案请求        
    case MCS_MCU_MATRIX_GETALLSCHEMES_CMD:  //终端内置矩阵控制
    case MCS_MCU_MATRIX_GETONESCHEME_CMD:
    case MCS_MCU_MATRIX_SETCURSCHEME_CMD:
    case MCS_MCU_MATRIX_GETCURSCHEME_CMD:
    case MCS_MCU_EXMATRIX_GETINFO_CMD:      //获取终端外置矩阵类型
    case MCS_MCU_EXMATRIX_SETPORT_CMD:      //设置外置矩阵连接端口号
    case MCS_MCU_EXMATRIX_GETPORT_REQ:      //请求获取外置矩阵连接端口
    case MCS_MCU_EXMATRIX_SETPORTNAME_CMD:  //设置外置矩阵连接端口名
    case MCS_MCU_EXMATRIX_GETALLPORTNAME_CMD://请求获取外置矩阵的所有端口名        
    case MCS_MCU_GETVIDEOSOURCEINFO_CMD://终端扩展视频源
    case MCS_MCU_SETVIDEOSOURCEINFO_CMD:
    case MCS_MCU_SELECTEXVIDEOSRC_CMD:
	case MCS_MCU_STARTREC_REQ:              //会控向MCU请求录像
	case MCS_MCU_PAUSEREC_REQ:              //会控向MCU暂停录像
	case MCS_MCU_RESUMEREC_REQ:             //会控向MCU恢复录像
	case MCS_MCU_STOPREC_REQ:               //会控向MCU停止录像
	case MCS_MCU_STARTPLAY_REQ:             //会控开始放像请求
	case MCS_MCU_PAUSEPLAY_REQ:             //会控暂停放像请求
	case MCS_MCU_RESUMEPLAY_REQ:            //会控恢复放像请求
	case MCS_MCU_STOPPLAY_REQ:              //会控停止放像请求
	case MCS_MCU_FFPLAY_REQ:                //会控快进放像请求
	case MCS_MCU_FBPLAY_REQ:                //会控快退放像请求
	case MCS_MCU_SEEK_REQ:                  //会控调整放像进度请求
    case MCS_MCU_GETRECPROG_CMD:            //会控查询录相进度
    case MCS_MCU_GETPLAYPROG_CMD:           //会控查询放相进度
	case MCS_MCU_START_SWITCH_TW_REQ:       //会议控制台请求将指定Mt的图像交换到指定外设的指定索引的通道上
	case MCS_MCU_STARTVMPBRDST_REQ:         //会议控制台命令MCU开始把画面合成图像广播到终端
	case MCS_MCU_STOPVMPBRDST_REQ:          //会议控制台命令MCU停止把画面合成图像广播到终端
	case MCS_MCU_LOCKSMCU_REQ:		
    case MCS_MCU_CHANGEMIXDELAY_REQ:
	case MCS_MCU_START_VMPBATCHPOLL_REQ:
	case MCS_MCU_PAUSE_VMPBATCHPOLL_CMD:
	case MCS_MCU_STOP_VMPBATCHPOLL_CMD:
	case MCS_MCU_STARTSWITCHVMPMT_REQ:
	case MCS_MCU_START_SWITCH_HDU_REQ:      //4.6.1  新加版本 jlb
		cConfId = cServMsg.GetConfId();
        byConfIdx = GetConfIdx( cConfId );
        if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
        {
            OspPrintf(TRUE, FALSE, "[MsgPassCheck] invalid cConfId :");
            cConfId.Print();
            return FALSE;
        }
        tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
        if(!tMapData.IsValidConf())
        {
            OspPrintf(TRUE, FALSE, "[MsgPassCheck] invalid instance id :%d\n", tMapData.GetInsId());
            return FALSE;
        }
	    GetConfProtectInfo( byConfIdx, &tProtectInfo );
        bySrcSsnId = cServMsg.GetSrcSsnId();
		byConfProtectMode = m_apConfInst[tMapData.GetInsId()-1]->m_tConf.m_tStatus.GetProtectMode();
		if( ( byConfProtectMode == CONF_LOCKMODE_LOCK && 
			  ( tProtectInfo.IsLockByMcs() && tProtectInfo.GetLockedMcSsnId() != bySrcSsnId || tProtectInfo.IsLockByMcu() ) 
             ) || 
		    ( byConfProtectMode == CONF_LOCKMODE_NEEDPWD && 
			  !tProtectInfo.IsMcsPwdPassed(bySrcSsnId)
            ) 
           )
        { 
            bReturn = FALSE;
			if( byConfProtectMode == CONF_LOCKMODE_NEEDPWD )
			{
				//问密码
				cServMsg.SetMsgBody();
                CMcsSsn::SendMsgToMcsSsn( bySrcSsnId, MCU_MCS_ENTERCONFPWD_REQ, 
					                          cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			}
			else
			{
				//Nack, 不发给会议
                cServMsg.SetErrorCode( ERR_MCU_CTRLBYOTHER );
				CMcsSsn::SendMsgToMcsSsn( bySrcSsnId, wEvent+2, cServMsg.GetServMsg(), 
					                          cServMsg.GetServMsgLen() );

				 if (tProtectInfo.IsLockByMcu())
                 {
					TMtAlias tMtAlias;
                    u8 byLockedMcuId = tProtectInfo.GetLockedMcuId();
					m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable->GetMtAlias(byLockedMcuId, 
																		mtAliasTypeTransportAddress,
																		&tMtAlias);

					cServMsg.SetMsgBody( (u8*)&tMtAlias.m_tTransportAddr.m_dwIpAddr, sizeof(u32) );
					if(!m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable->GetMtAlias(byLockedMcuId, 
																			mtAliasTypeH323ID,
																			&tMtAlias))
					{
						if(!m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable->GetMtAlias(byLockedMcuId, 
																				mtAliasTypeE164,
																				&tMtAlias))
						{
							strncpy( tMtAlias.m_achAlias, "mmcu", sizeof(tMtAlias.m_achAlias) );
						}
					}
					tMtAlias.m_achAlias[MAXLEN_PWD-1] = 0;
					cServMsg.CatMsgBody( (u8*)tMtAlias.m_achAlias, MAXLEN_PWD );										
                 }
                 else if(tProtectInfo.IsLockByMcs() && tProtectInfo.GetLockedMcSsnId() != bySrcSsnId )
                 {
                     TMcsRegInfo tMcsRegInfo;
                     GetMcsRegInfo( tProtectInfo.GetLockedMcSsnId(), &tMcsRegInfo );
                     cServMsg.SetMsgBody( (u8*)&tMcsRegInfo.m_dwMcsIpAddr, sizeof(u32) );
                     cServMsg.CatMsgBody( (u8*)tMcsRegInfo.m_achUser, MAXLEN_PWD );
                 }

				CMcsSsn::SendMsgToMcsSsn( bySrcSsnId, MCU_MCS_LOCKUSERINFO_NOTIFY, cServMsg.GetServMsg(), 
					                          cServMsg.GetServMsgLen() );
			}

		}
	 break;

	default:
		break;
	}
	return bReturn;
}

/*====================================================================
    函数名      ：BroadcastToAllConf
    功能        ：发消息给所有会议实例
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
	05/12/20	4.0			张宝卿		  增加T120消息
====================================================================*/
void CMcuVcData::BroadcastToAllConf( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//本端离线或者备用且未完全同步则丢弃(MTADP/MP/MC/外设 离线消息除外)，
	//否则允许向业务模块投递消息
	if ( MTADP_MCU_REGISTER_REQ != wEvent && 
		 MCU_MTADP_DISCONNECTED_NOTIFY != wEvent && 
		 MP_MCU_REG_REQ != wEvent && 
		 MCU_MP_DISCONNECTED_NOTIFY != wEvent && 
		 MCU_MCSCONNECTED_NOTIF != wEvent && 
		 MCU_MCSDISCONNECTED_NOTIF != wEvent && 
	 	 MCU_EQPCONNECTED_NOTIF != wEvent && 
 		 MCU_EQPDISCONNECTED_NOTIF != wEvent && 
		 MCU_RECCONNECTED_NOTIF != wEvent && 
		 MCU_RECDISCONNECTED_NOTIF != wEvent && 
		 MCU_BASCONNECTED_NOTIF != wEvent && 
		 MCU_BASDISCONNECTED_NOTIF != wEvent && 
		 MCU_MIXERCONNECTED_NOTIF != wEvent && 
		 MCU_MIXERDISCONNECTED_NOTIF != wEvent && 
		 MCU_VMPCONNECTED_NOTIF != wEvent && 
		 MCU_VMPDISCONNECTED_NOTIF != wEvent && 
		 MCU_VMPTWCONNECTED_NOTIF != wEvent && 
		 MCU_VMPTWDISCONNECTED_NOTIF != wEvent && 
		 MCU_PRSCONNECTED_NOTIF != wEvent && 
		 MCU_PRSDISCONNECTED_NOTIF != wEvent && 
		 MCU_TVWALLCONNECTED_NOTIF != wEvent && 
		 MCU_TVWALLDISCONNECTED_NOTIF != wEvent && 
		 MCU_DCSCONNCETED_NOTIF != wEvent &&
		 MCU_DCSDISCONNECTED_NOTIF != wEvent &&
		 MCU_HDUCONNECTED_NOTIF != wEvent &&         //4.6 新加  jlb
		 MCU_HDUDISCONNECTED_NOTIF!= wEvent &&      //4.6 新加  jlb
		 FALSE == g_cMSSsnApp.JudgeRcvMsgPass() )
	{
		return;
	}

	if (MsgPassCheck(wEvent, pbyMsg, wLen))
	{
		if (0 != ::OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), wEvent, pbyMsg, wLen))
		{
			OspPrintf(TRUE, FALSE, "[BroadcastToAllConf] msg.%d<%s> broad failed!\n", wEvent, OspEventDesc(wEvent));
		}
	}

	return;
}

/*====================================================================
    函数名      ：SendMsgToConf
    功能        ：发消息给指定的会议对应的会议实例
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx, 会议索引号
				  u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：成功返回TRUE，若无该会议返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/28    1.0         LI Yi         创建
	05/12/20	4.0			张宝卿		  增加T120消息
====================================================================*/
BOOL32 CMcuVcData::SendMsgToConf( u8 byConfIdx, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//本端离线或者备用且未完全同步则丢弃(MTADP/MP/MC/外设 离线消息除外)，
	//否则允许向业务模块投递消息
	if (MTADP_MCU_REGISTER_REQ != wEvent && 
		MCU_MTADP_DISCONNECTED_NOTIFY != wEvent && 
		MP_MCU_REG_REQ != wEvent && 
		MCU_MP_DISCONNECTED_NOTIFY != wEvent && 
		MCU_MCSCONNECTED_NOTIF != wEvent && 
		MCU_MCSDISCONNECTED_NOTIF != wEvent && 
		MCU_EQPCONNECTED_NOTIF != wEvent && 
		MCU_EQPDISCONNECTED_NOTIF != wEvent && 
		MCU_RECCONNECTED_NOTIF != wEvent && 
		MCU_RECDISCONNECTED_NOTIF != wEvent && 
		MCU_BASCONNECTED_NOTIF != wEvent && 
		MCU_BASDISCONNECTED_NOTIF != wEvent && 
		MCU_MIXERCONNECTED_NOTIF != wEvent && 
		MCU_MIXERDISCONNECTED_NOTIF != wEvent && 
		MCU_VMPCONNECTED_NOTIF != wEvent && 
		MCU_VMPDISCONNECTED_NOTIF != wEvent && 
		MCU_VMPTWCONNECTED_NOTIF != wEvent && 
		MCU_VMPTWDISCONNECTED_NOTIF != wEvent && 
		MCU_PRSCONNECTED_NOTIF != wEvent && 
		MCU_PRSDISCONNECTED_NOTIF != wEvent && 
		MCU_TVWALLCONNECTED_NOTIF != wEvent && 
		MCU_TVWALLDISCONNECTED_NOTIF != wEvent && 
		MCU_DCSCONNCETED_NOTIF != wEvent &&
		MCU_DCSDISCONNECTED_NOTIF != wEvent &&
		MCU_HDUCONNECTED_NOTIF != wEvent &&          //4.6  新加  jlb 
		MCU_HDUDISCONNECTED_NOTIF != wEvent &&
		FALSE == g_cMSSsnApp.JudgeRcvMsgPass())
	{
		return TRUE;
	}
	
	if (0 != byConfIdx)
	{
		if (MsgPassCheck(wEvent, pbyMsg, wLen))
		{		
		    ::OspPost(MAKEIID(AID_MCU_VC, GetConfMapInsId(byConfIdx)), wEvent, pbyMsg, wLen);
		}
		return TRUE;
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[SendMsgToConf] ConfIdx is 0! event:%u(%s)\n", wEvent, OspEventDesc(wEvent));
	}
	
    return FALSE;
}

/*====================================================================
    函数名      ：SendMsgToConf
    功能        ：发消息给指定的会议对应的会议实例
    算法实现    ：
    引用全局变量：
    输入参数说明：const CConfId & cConfId, 会议号，0表示任意IDLE实例
				  u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：成功返回TRUE，若无该会议返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/28    1.0         LI Yi         创建
	05/12/20	4.0			张宝卿		  增加T120消息
====================================================================*/
BOOL32 CMcuVcData::SendMsgToConf( const CConfId & cConfId, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//本端离线或者备用且未完全同步则丢弃(MTADP/MP/MC/外设 离线消息除外)，
	//否则允许向业务模块投递消息
	if (MTADP_MCU_REGISTER_REQ != wEvent && 
		MCU_MTADP_DISCONNECTED_NOTIFY != wEvent && 
		MP_MCU_REG_REQ != wEvent && 
		MCU_MP_DISCONNECTED_NOTIFY != wEvent && 
		MCU_MCSCONNECTED_NOTIF != wEvent && 
		MCU_MCSDISCONNECTED_NOTIF != wEvent && 
		MCU_EQPCONNECTED_NOTIF != wEvent && 
		MCU_EQPDISCONNECTED_NOTIF != wEvent && 
		MCU_RECCONNECTED_NOTIF != wEvent && 
		MCU_RECDISCONNECTED_NOTIF != wEvent && 
		MCU_BASCONNECTED_NOTIF != wEvent && 
		MCU_BASDISCONNECTED_NOTIF != wEvent && 
		MCU_MIXERCONNECTED_NOTIF != wEvent && 
		MCU_MIXERDISCONNECTED_NOTIF != wEvent && 
		MCU_VMPCONNECTED_NOTIF != wEvent && 
		MCU_VMPDISCONNECTED_NOTIF != wEvent && 
		MCU_VMPTWCONNECTED_NOTIF != wEvent && 
		MCU_VMPTWDISCONNECTED_NOTIF != wEvent && 
		MCU_PRSCONNECTED_NOTIF != wEvent && 
		MCU_PRSDISCONNECTED_NOTIF != wEvent && 
		MCU_TVWALLCONNECTED_NOTIF != wEvent && 
		MCU_TVWALLDISCONNECTED_NOTIF != wEvent && 
		MCU_DCSCONNCETED_NOTIF != wEvent &&
		MCU_DCSDISCONNECTED_NOTIF != wEvent &&
		MCU_HDUCONNECTED_NOTIF != wEvent && 
		MCU_HDUDISCONNECTED_NOTIF != wEvent && 
		FALSE == g_cMSSsnApp.JudgeRcvMsgPass())
	{
		return TRUE;
	}

	if (cConfId.IsNull())
	{
//		if (MsgPassCheck(wEvent, pbyMsg, wLen))
//		{		
//		    ::OspPost(MAKEIID(AID_MCU_VC, CInstance::PENDING), wEvent, pbyMsg, wLen);
//		}

        OspPrintf(TRUE, FALSE, "[SendMsgToConf] ConfId is Null! event:%u(%s)\n", wEvent, OspEventDesc(wEvent));

		return TRUE;
	}
    
    if (MsgPassCheck(wEvent, pbyMsg, wLen))
	{
		u8 byConfIdx = GetConfIdx(cConfId);
		if (0 != byConfIdx)
		{
            u8 byInsId = GetConfMapInsId(byConfIdx);
            if (0 == byInsId || byInsId > MAXNUM_MCU_CONF)
            {
                OspPrintf(TRUE, FALSE, "[SendMsgToConf] Send msg to conf failure, invalid confidx %d!\n", byConfIdx);
            }
			else
			{
                if (OSP_OK != ::OspPost( MAKEIID( AID_MCU_VC, byInsId), wEvent, pbyMsg, wLen ))
                {
                    OspPrintf(TRUE, FALSE, "[SendMsgToConf] Send msg to conf by confIdx failure!\n");
                }
			}
		}
        else
        {
            s8 achBuf[MAXLEN_CONFID*2+1] = {0};
            cConfId.GetConfIdString( achBuf, sizeof(achBuf) );
            OspPrintf(TRUE, FALSE, "[SendMsgToConf] cConfId %s invalid, Event = %s(%d)!\n", 
                      achBuf, 
                      OspEventDesc(wEvent),
                      wEvent);            
        }
	}
	
    return TRUE;
}

/*====================================================================
    函数名      ：SendMsgToDaemonConf
    功能        ：发消息给业务Daemon实例
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：成功返回TRUE，若无该会议返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/30    1.0         JQL           创建
	05/12/20	4.0			张宝卿		  增加T120消息
====================================================================*/
BOOL32 CMcuVcData::SendMsgToDaemonConf( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//本端离线或者备用且未完全同步则丢弃(MTADP/MP/MC/外设 离线消息除外)，
	//否则允许向业务模块投递消息
	if (MTADP_MCU_REGISTER_REQ != wEvent && 
		MCU_MTADP_DISCONNECTED_NOTIFY != wEvent && 
		MP_MCU_REG_REQ != wEvent && 
		MCU_MP_DISCONNECTED_NOTIFY != wEvent && 
		MCU_MCSCONNECTED_NOTIF != wEvent && 
		MCU_MCSDISCONNECTED_NOTIF != wEvent && 
		MCU_EQPCONNECTED_NOTIF != wEvent && 
		MCU_EQPDISCONNECTED_NOTIF != wEvent && 
		MCU_RECCONNECTED_NOTIF != wEvent && 
		MCU_RECDISCONNECTED_NOTIF != wEvent && 
		MCU_BASCONNECTED_NOTIF != wEvent && 
		MCU_BASDISCONNECTED_NOTIF != wEvent && 
		MCU_MIXERCONNECTED_NOTIF != wEvent && 
		MCU_MIXERDISCONNECTED_NOTIF != wEvent && 
		MCU_VMPCONNECTED_NOTIF != wEvent && 
		MCU_VMPDISCONNECTED_NOTIF != wEvent && 
		MCU_VMPTWCONNECTED_NOTIF != wEvent && 
		MCU_VMPTWDISCONNECTED_NOTIF != wEvent && 
		MCU_PRSCONNECTED_NOTIF != wEvent && 
		MCU_PRSDISCONNECTED_NOTIF != wEvent && 
		MCU_TVWALLCONNECTED_NOTIF != wEvent && 
		MCU_TVWALLDISCONNECTED_NOTIF != wEvent && 
		MCU_DCSCONNCETED_NOTIF != wEvent &&
		MCU_DCSDISCONNECTED_NOTIF != wEvent &&
		MCU_HDUCONNECTED_NOTIF != wEvent && 
		MCU_HDUDISCONNECTED_NOTIF != wEvent && 
		FALSE == g_cMSSsnApp.JudgeRcvMsgPass())
	{
		return TRUE;
	}

    if (MsgPassCheck( wEvent, pbyMsg, wLen ))
	{
        ::OspPost(MAKEIID( AID_MCU_VC, CInstance::DAEMON ), wEvent, pbyMsg, wLen);
	}

    return TRUE;
}

/*====================================================================
    函数名      ：RefreshHtml
    功能        ：刷新Html页面
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/04/19    3.0         胡昌威        修改
    06/04/19    4.0         顾振华        NMS1写文件要求传入主机序地址
    08/06/27    5.0         周文          WriteHtml要求tStreamChannel包含AAC相关内容
====================================================================*/
void CMcuVcData::RefreshHtml( void )
{
	TStreamChannel tStreamChannel[64];
	memset( tStreamChannel, 0, sizeof(tStreamChannel) );
	u8  byChlNum = 0;
	TCapSupport tCapSupport;
    TMediaEncrypt tMediaEncrypt;
    u16 wMultiCastPort;

	for( u8 byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++ )
	{
		if( m_apConfInst[byLoop] != NULL && 
			m_apConfInst[byLoop]->m_tConf.GetConfAttrb().IsMulticastMode() && 
			m_apConfInst[byLoop]->m_tConf.m_tStatus.IsOngoing() )
		{
            tMediaEncrypt = m_apConfInst[byLoop]->m_tConf.GetMediaKey();
			tCapSupport = m_apConfInst[byLoop]->m_tConf.GetCapSupport();

			strncpy( tStreamChannel[byChlNum].m_aszChannelName, m_apConfInst[byLoop]->m_tConf.GetConfName(), 254 );
			strncpy( tStreamChannel[byChlNum].m_aszPassWord, m_apConfInst[byLoop]->m_tConf.GetConfPwd(), 32 );
            
            // 采用主机序地址
			tStreamChannel[byChlNum].m_dwIp  = AssignMulticastIp(m_apConfInst[byLoop]->m_byConfIdx);
            wMultiCastPort = AssignMulticastPort( (u8)m_apConfInst[byLoop]->m_byConfIdx, 0 );
            tStreamChannel[byChlNum].m_wPort[0] = wMultiCastPort;

            //设置加密密钥
            s32 nTmpLen;
            //视频
            tStreamChannel[byChlNum].m_tEncrypt[0].m_tEncryptKey[0].m_byMode = tMediaEncrypt.GetEncryptMode();
            tMediaEncrypt.GetEncryptKey(tStreamChannel[byChlNum].m_tEncrypt[0].m_tEncryptKey[0].m_abyKey, &nTmpLen);
            tStreamChannel[byChlNum].m_tEncrypt[0].m_tEncryptKey[0].m_byKeyLen = (u8)nTmpLen;

            //音频
            tStreamChannel[byChlNum].m_tEncrypt[0].m_tEncryptKey[1].m_byMode = tMediaEncrypt.GetEncryptMode();
            tMediaEncrypt.GetEncryptKey(tStreamChannel[byChlNum].m_tEncrypt[0].m_tEncryptKey[1].m_abyKey, &nTmpLen);
            tStreamChannel[byChlNum].m_tEncrypt[0].m_tEncryptKey[1].m_byKeyLen = (u8)nTmpLen;
            
            //默认写入双流信息
            tStreamChannel[byChlNum].m_wPort[1] = wMultiCastPort + 4;
            tStreamChannel[byChlNum].m_dwStreamNum = 2; 
            //视频
            tStreamChannel[byChlNum].m_tEncrypt[1].m_tEncryptKey[0].m_byMode = tMediaEncrypt.GetEncryptMode();
            tMediaEncrypt.GetEncryptKey(tStreamChannel[byChlNum].m_tEncrypt[1].m_tEncryptKey[0].m_abyKey, &nTmpLen);
            tStreamChannel[byChlNum].m_tEncrypt[1].m_tEncryptKey[0].m_byKeyLen = (u8)nTmpLen;

            //双流没有音频，不进行设置

			//第一路视频为H264时存在动态载荷支持，目前暂不支持加密流媒体
			if( MEDIA_TYPE_H264 == tCapSupport.GetMainVideoType() || 
				( !tCapSupport.GetSecondSimCapSet().IsNull() && 
				  MEDIA_TYPE_H264 == tCapSupport.GetSecVideoType() ) )
			{
                tStreamChannel[byChlNum].m_tEncrypt[0].m_byRealVideoPT = MEDIA_TYPE_H264;
				tStreamChannel[byChlNum].m_tEncrypt[0].m_byVideoEncType = 
                    GetActivePayload(m_apConfInst[byLoop]->m_tConf, MEDIA_TYPE_H264);
			}
			else
			{
				tStreamChannel[byChlNum].m_tEncrypt[0].m_byRealVideoPT  = tCapSupport.GetMainVideoType();
				tStreamChannel[byChlNum].m_tEncrypt[0].m_byVideoEncType = tCapSupport.GetMainVideoType();
			}
			tStreamChannel[byChlNum].m_tEncrypt[0].m_byRealAudioPT  = tCapSupport.GetMainAudioType();
			tStreamChannel[byChlNum].m_tEncrypt[0].m_byAudioEncType = tCapSupport.GetMainAudioType();

            //第二路视频
            tStreamChannel[byChlNum].m_tEncrypt[1].m_byRealVideoPT  = tCapSupport.GetDStreamMediaType();
            tStreamChannel[byChlNum].m_tEncrypt[1].m_byVideoEncType =
                GetActivePayload(m_apConfInst[byLoop]->m_tConf, tCapSupport.GetDStreamMediaType());

            tStreamChannel[byChlNum].m_tEncrypt[1].m_byRealAudioPT  = MEDIA_TYPE_NULL;
			tStreamChannel[byChlNum].m_tEncrypt[1].m_byAudioEncType = MEDIA_TYPE_NULL;

            // zw [06/27/2008] 如果是AAC会议，添加AAC需要的内容
            if ( MEDIA_TYPE_AACLC == m_apConfInst[byLoop]->m_tConf.GetMainAudioMediaType() )
            {
                tStreamChannel[byChlNum].m_tEncrypt[0].m_byAACSamplePerSecond = AAC_SAMPLE_FRQ_32;
                tStreamChannel[byChlNum].m_tEncrypt[0].m_byAACChannels        = AAC_CHNL_TYPE_SINGLE; 
            }

			byChlNum++;
		}
	}

	s8	   achWebFileName[KDV_MAX_PATH];
	sprintf( achWebFileName, "%s/%s", DIR_WEB, MCUHTMLFILENAME );
    McsLog("[RefreshHtml] Writing VOD Channel info(%d chnl) into file %s.\n", byChlNum, achWebFileName);
	WriteHtml( achWebFileName, tStreamChannel, byChlNum );

	return;
}

/*====================================================================
    函数名      ：AddConf
    功能        ：向会议表中增加一个会议
    算法实现    ：
    引用全局变量：
    输入参数说明：TConfFullInfo	*ptConf, 会议完整信息
	              BOOL32 bRefreshHtml =TRUE 是否刷新流媒体文件
    返回值说明  ：成功返回TRUE，失败返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
	03/05/29	1.2			Jiaql         修改
	03/11/08    3.0         胡昌威        修改
====================================================================*/
BOOL32 CMcuVcData::AddConf( CMcuVcInst *pConfInst, BOOL32 bRefreshHtml /*=TRUE*/ )
{
	if( pConfInst->GetInsID() > MAXNUM_MCU_CONF )
	{
		return FALSE;
	}

    m_apConfInst[(u8)pConfInst->GetInsID()-1] = pConfInst;

	if (bRefreshHtml)
	{
		RefreshHtml();
	}

	return TRUE;
}

/*====================================================================
    函数名      ：GetConfInstHandle
    功能        ：根据会议号索引号 查询会议实例指针
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx, 会议号索引号
    返回值说明  ：成功返回TRUE，失败返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
	03/11/08    3.0         胡昌威        修改
====================================================================*/
CMcuVcInst *CMcuVcData::GetConfInstHandle( u8 byConfIdx )
{
	if (byConfIdx > MAX_CONFIDX || byConfIdx < MIN_CONFIDX)
	{
		return NULL;
	}
	
    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if (!tMapData.IsValidConf() || NULL == m_apConfInst[tMapData.GetInsId()-1])
    {
        return NULL;
    }
    
	return (CMcuVcInst *)m_apConfInst[tMapData.GetInsId()-1];

}

/*====================================================================
    函数名      ：RemoveConf
    功能        ：向会议表中删除一个会议
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx, 会议号索引号
	              BOOL32 bRefreshHtml =TRUE 是否刷新流媒体文件
    返回值说明  ：成功返回TRUE，失败返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
	03/11/08    3.0         胡昌威        修改
====================================================================*/
BOOL32 CMcuVcData::RemoveConf( u8 byConfIdx, BOOL32 bRefreshHtml /*=TRUE*/ )
{
    CHECK_CONFIDX(byConfIdx)	
	
    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if(!tMapData.IsValidConf())
    {
        return FALSE;
    }
    m_apConfInst[tMapData.GetInsId()-1] = NULL;

	if (bRefreshHtml)
	{
		RefreshHtml();
	}

	return TRUE;
}

/*====================================================================
    函数名      ：GetConfIdx
    功能        ：获得会议号在会议表中的序号
    算法实现    ：
    引用全局变量：
    输入参数说明：const CConfId& cConfId 会议号    
    返回值说明  ：0 没有找到会议号对应的序号，表示会议不存在
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/24    1.0         LI Yi         创建
	03/11/08    3.0         胡昌威        修改
====================================================================*/
u8 CMcuVcData::GetConfIdx( const CConfId& cConfId ) const
{	
	u8 byLoop;	    
    for( byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++ )
    {
        if( m_apConfInst[byLoop] != NULL &&
            m_apConfInst[byLoop]->m_tConf.GetConfId() == cConfId )
        {
            return m_apConfInst[byLoop]->m_byConfIdx;
        }
    }

    for( byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++ )
    {
        if (!m_ptTemplateInfo[byLoop].IsEmpty() &&
            m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfId() == cConfId)
        {
            return m_ptTemplateInfo[byLoop].m_byConfIdx;
        }
    }

   return 0;
}

/*====================================================================
    函数名      ：GetConfId
    功能        ：根据在会议表中的序号获得会议号
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/24    1.0         LI Yi         创建
	03/11/08    3.0         胡昌威        修改
====================================================================*/
CConfId CMcuVcData::GetConfId(const u8 &byConfIdx) const
{
    CConfId cConfId;
    cConfId.SetNull();

    u8 byLoop;	    
    for( byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++ )
    {
        if( m_apConfInst[byLoop] != NULL &&
            m_apConfInst[byLoop]->m_byConfIdx == byConfIdx )
        {
            return m_apConfInst[byLoop]->m_tConf.GetConfId();
        }
    }
    return cConfId;
}

/*====================================================================
    函数名      ：MakeConfId
    功能        ：根据本MCU号/会议类型/会议索引值/会议号产生次数 产生会议号
                  支持用户组信息
				  支持会议创始者信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：会议号
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/01    1.0         LI Yi         创建
    06/06/21    4.0         顾振华        支持用户组信息
====================================================================*/
CConfId CMcuVcData::MakeConfId(u8 byConfIdx, u8 byTemplate, u8 byUsrGrpId, u8 byConfSource /*= MCS_CONF*/ )
{
	CConfId	cConfId;
	u8	abyTConfId[8];
	u16	wTemp;

	//会议号产生次数进行累计,保证前后产生的会议号的唯一性
	m_dwMakeConfIdTimes++;

	//2 bytes
	wTemp = htons( LOCAL_MCUID );
	memcpy( abyTConfId, &wTemp, 2 );
	//2 bytes
	abyTConfId[2] = byTemplate;
	abyTConfId[3] = byConfIdx;
	//4 bytes
    u32 dwTimes = htonl(m_dwMakeConfIdTimes);
	memcpy(abyTConfId+4, &dwTimes, sizeof(dwTimes));
	
	cConfId.SetConfId(abyTConfId, sizeof(abyTConfId));
    cConfId.SetUsrGrpId( byUsrGrpId );
	cConfId.SetConfSource( byConfSource );

	return( cConfId );
}

/*====================================================================
    函数名      ：GetMakeTimesFromConfId
    功能        ：从CConfId取得已经进行了多少次的MakeConfId
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/04/10    4.0         顾振华          创建
====================================================================*/
u32 CMcuVcData::GetMakeTimesFromConfId(const CConfId& cConfId) const
{
    u32 dwTimes;
    u8	abyConfId[sizeof(CConfId)];
    cConfId.GetConfId( abyConfId, sizeof(abyConfId) );
    memcpy(&dwTimes, abyConfId+4, sizeof(u32));
    dwTimes = ntohl(dwTimes);
    return dwTimes;
}

/*====================================================================
    函数名      ：IsConfNameRepeat
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszConfName 新的会议名    
    返回值说明  ：TRUE-会议名重复 FALSE-不重复
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/08    3.0         胡昌威        创建
====================================================================*/
BOOL32 CMcuVcData::IsConfNameRepeat( LPCSTR lpszConfName, BOOL32 IsTemple )
{
	u8 byLoop;
	
    if(IsTemple)
    {
        for(byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
        {
            if(!m_ptTemplateInfo[byLoop].IsEmpty() && 
                0 == memcmp(m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfName(), lpszConfName, MAXLEN_CONFNAME ))
            {
                return TRUE;
            }
        }
    }
    else
    {
        for( byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++ )
        {
            if( m_apConfInst[byLoop] != NULL )
            {
                if (0 == memcmp( m_apConfInst[byLoop]->m_tConf.GetConfName(), lpszConfName, MAXLEN_CONFNAME ))                    
                {
                    return TRUE;
                }
            }
        }
    }	

	return FALSE;
}

/*====================================================================
    函数名      ：IsConfNameRepeat
    功能        ：是否会议E164号码重复
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszConfE164 会议的E164号   
    返回值说明  ：TRUE-会议名重复 FALSE-不重复
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/07/25    3.0         胡昌威        创建
====================================================================*/
BOOL32 CMcuVcData::IsConfE164Repeat( LPCSTR lpszConfE164, BOOL32 IsTemple )
{
	u8 byLoop;
	char abyMcuE164[MAXLEN_E164];
	g_cMcuAgent.GetE164Number( abyMcuE164, MAXLEN_E164 );
	if( memcmp( abyMcuE164, lpszConfE164, MAXLEN_E164 ) == 0 )
	{
		return TRUE;
	}
	
    if(IsTemple)
    {
        for(byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
        {
            if(!m_ptTemplateInfo[byLoop].IsEmpty() && 
               0 == memcmp(m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfE164(), lpszConfE164, MAXLEN_E164 ))
            {
                return TRUE;
            }
        }
    }
    else
    {
        for( byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++ )
        {
            if ( m_apConfInst[byLoop] != NULL )
            {
                if(0 == memcmp( m_apConfInst[byLoop]->m_tConf.GetConfE164(), lpszConfE164, MAXLEN_E164 ))
                {
                    return TRUE;	
                }
            }
        }
	}

	return FALSE;	
}

/*====================================================================
    函数名      ：GetConfIdxByE164
    功能        ：从会议的E164号码得到会议的索引号
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszConfE164 会议的E164号 
				  pbyOngoningConfIdx 返回的ongonig conf index
				  pbyTemplateConfIdx 返回的template conf idex

    返回值说明  ：TRUE-会议名重复 FALSE-不重复
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/07/25    3.0         胡昌威        创建
====================================================================*/
u8  CMcuVcData::GetOngoingConfIdxByE164( LPCSTR lpszConfE164 )
{
    if( NULL == lpszConfE164 )
    {
        return 0;
    }

    for (u8 byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++)
    {
        if (m_apConfInst[byLoop] == NULL)
        {
            continue;
        }
        
        if (0 == memcmp(m_apConfInst[byLoop]->m_tConf.GetConfE164(), lpszConfE164, MAXLEN_E164)) 
        {
            return m_apConfInst[byLoop]->m_byConfIdx;
        }
    }
    
    return 0;
}

u8  CMcuVcData::GetTemConfIdxByE164( LPCSTR lpszConfE164 )
{
    if( NULL == lpszConfE164 )
    {
        return 0;
    }

    for( u8 byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
    {
        if (m_ptTemplateInfo[byLoop].IsEmpty())
        {
            continue;
        }
        
        if (0 == memcmp(m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfE164(), lpszConfE164, MAXLEN_E164))
        {
            return m_ptTemplateInfo[byLoop].m_byConfIdx;
        } 
    }

    return 0;
}

/*=============================================================================
  函 数 名： GetConfRateByConfIdx
  功    能： 由会议序号取会议速率
  算法实现： 
  全局变量： 
  参    数：  u16 wConfIdx
             u16& wFirstRate
             u16& wSecondRate
             BOOL32 bTemplate /* = FALSE  */
//  返 回 值： u16 
// =============================================================================*/
u16 CMcuVcData::GetConfRateByConfIdx( u16 wConfIdx, u16& wFirstRate, u16& wSecondRate, BOOL32 bTemplate /* = FALSE  */)
{
    if( 0 == wConfIdx)
    {
        return 0;
    }
    u16 wAudRate = 0;
    if( FALSE == bTemplate )
    {
        for (u8 byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++)
        {
            if (NULL == m_apConfInst[byLoop] )
            {
                continue;
            }
    
            if ( m_apConfInst[byLoop]->m_byConfIdx == (u8)wConfIdx ) 
            {
                wFirstRate = m_apConfInst[byLoop]->m_tConf.GetBitRate();
                wSecondRate = m_apConfInst[byLoop]->m_tConf.GetSecBitRate();
                TCapSupport m_tCapSupport = m_apConfInst[byLoop]->m_tConf.GetCapSupport();
                wAudRate = GetAudioBitrate(m_tCapSupport.GetMainAudioType() );
                wFirstRate += wAudRate;
                wSecondRate = ( 0 != wSecondRate ) ? (wSecondRate+wAudRate): 0;
                
                break;
            }
        }
    }
    else
    {
        for( u8 byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
        {
            if (m_ptTemplateInfo[byLoop].IsEmpty())
            {
                continue;
            }
    
            if ( m_ptTemplateInfo[byLoop].m_byConfIdx == wConfIdx )
            {
                wFirstRate = m_ptTemplateInfo[byLoop].m_tConfInfo.GetBitRate();
                wSecondRate = m_ptTemplateInfo[byLoop].m_tConfInfo.GetSecBitRate();
                wAudRate = GetAudioBitrate( m_ptTemplateInfo[byLoop].m_tConfInfo.GetMainAudioMediaType() );
                wFirstRate += wAudRate;
                wSecondRate = ( 0 != wSecondRate ) ? (wSecondRate+wAudRate): 0;
            } 
        }
    }
    return 0;
}

/*=============================================================================
  函 数 名： GetConfNameByConfId
  功    能： 根据会议ID取会议名称
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： u8 ConfIdx, 0 - 没有找到
 =============================================================================*/
u8 CMcuVcData::GetConfNameByConfId( const CConfId cConfId, LPCSTR &lpszConfName )
{
    lpszConfName = NULL;
    for (u8 byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++)
    {
        if (NULL == m_apConfInst[byLoop] )
        {
            continue;
        }

        if ( m_apConfInst[byLoop]->m_tConf.GetConfId() == cConfId ) 
        {
            lpszConfName = m_apConfInst[byLoop]->m_tConf.GetConfName();
            return m_apConfInst[byLoop]->m_byConfIdx;
        }
    }    
    return 0;
}


/*=============================================================================
  函 数 名： GetConfIdByName
  功    能： 根据会议名称取CConfId
  算法实现： 
  全局变量： 
  参    数：  
  返 回 值： u8 ConfIdx, 0 - 没有找到
 =============================================================================*/
CConfId CMcuVcData::GetConfIdByName(LPCSTR lpszConfName, BOOL32 bTemplate )
{
    CConfId cConfId;
    cConfId.SetNull();
    u8 byLoop = 0;

    if (!bTemplate)
    {
        for (byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++)
        {
            if (NULL == m_apConfInst[byLoop] )
            {
                continue;
            }

            if ( strncmp(m_apConfInst[byLoop]->m_tConf.GetConfName(), lpszConfName, MAXLEN_CONFNAME) == 0) 
            {
                cConfId = m_apConfInst[byLoop]->m_tConf.GetConfId();
                break;
            }
        }  
    }
    else
    {
        for( byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
        {
            if (m_ptTemplateInfo[byLoop].IsEmpty())
            {
                continue;
            }
    
            if ( strncmp(m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfName(), lpszConfName, MAXLEN_CONFNAME) == 0) 
            {
                cConfId = m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfId();
                break;                
            } 
        }        
    }
    
    return cConfId;
}

/*=============================================================================
  函 数 名： GetConfIdByE164
  功    能： 根据会议名称取CConfId
  算法实现： 
  全局变量： 
  参    数：  
  返 回 值： u8 ConfIdx, 0 - 没有找到
 =============================================================================*/
CConfId CMcuVcData::GetConfIdByE164( LPCSTR lpszConfE164, BOOL32 bTemplate )
{
    CConfId cConfId;
    cConfId.SetNull();
    u8 byLoop = 0;

    if (!bTemplate)
    {
        for (byLoop = 0; byLoop < MAXNUM_MCU_CONF; byLoop++)
        {
            if (NULL == m_apConfInst[byLoop] )
            {
                continue;
            }

            if ( strncmp(m_apConfInst[byLoop]->m_tConf.GetConfE164(), lpszConfE164, MAXLEN_E164) == 0) 
            {
                cConfId = m_apConfInst[byLoop]->m_tConf.GetConfId();
                break;
            }
        }  
    }
    else
    {
        for( byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
        {
            if (m_ptTemplateInfo[byLoop].IsEmpty())
            {
                continue;
            }
    
            if ( strncmp(m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfE164(), lpszConfE164, MAXLEN_E164) == 0) 
            {
                cConfId = m_ptTemplateInfo[byLoop].m_tConfInfo.GetConfId();
                break;                
            } 
        }        
    }
    
    return cConfId;
}

/*====================================================================
    函数名      ：GetConfNum
    功能        ：查询当前会议个数
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bIncOngoing,           含即时会议
                  BOOL32 bIncSched,             含预约会议
                  BOOL32 bIncTempl,             含模板  
				  u8 byConfSource               针对某一发起方的信息统计
    返回值说明  ：实际返回的会议数目
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/03/19    1.0         Qzj           创建
	03/11/08    3.0         胡昌威        修改
    07/01/19    4.0         顾振华        修改
	08/11/28                付秀华        修改
====================================================================*/
u8 CMcuVcData::GetConfNum( BOOL32 bIncOngoing, BOOL32 bIncSched, BOOL32 bIncTempl, u8 byConfSource /*= MCS_CONF*/ ) const
{
	u8		byLoop;
	u8		byNum = 0;
	for( byLoop = 0; byLoop < MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE; byLoop++ )
	{
        TConfMapData tMapData = m_atConfMapData[byLoop];
        if ( (tMapData.IsValidConf() && NULL != m_apConfInst[tMapData.GetInsId()-1] ) )
        {
			if (byConfSource != ALL_CONF && m_apConfInst[tMapData.GetInsId()-1]->m_tConf.GetConfSource() != byConfSource)
			{
				break;
			}

            if ( bIncOngoing && m_apConfInst[tMapData.GetInsId()-1]->m_tConf.m_tStatus.IsOngoing() )
            {
                byNum ++;
            }
            if ( bIncSched && m_apConfInst[tMapData.GetInsId()-1]->m_tConf.m_tStatus.IsScheduled() )
            {
                byNum ++;
            }
        }
        if ( bIncTempl && tMapData.IsTemUsed() )
		{
			if (byConfSource != ALL_CONF && m_ptTemplateInfo[tMapData.GetTemIndex()].m_tConfInfo.GetConfSource() != byConfSource)
			{
				break;
			}

			byNum ++;
		}            
	}

	return( byNum );
}

/*====================================================================
    函数名      ：GetConfAllMtInfo
    功能        ：得到指定索引位置的一个完整会议信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx：索引位置
				  TConfAllMtInfo * pConfAllMtInfo：传入的数据空间
    返回值说明  ：TRUE - 成功 FALSE - 不存在
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/08/12    3.0         胡昌威        创建
====================================================================*/
BOOL32 CMcuVcData::GetConfAllMtInfo( u8 byConfIdx, TConfAllMtInfo * pConfAllMtInfo ) const
{
    CHECK_CONFIDX(byConfIdx)

	if(pConfAllMtInfo == NULL )
	{
		return FALSE;
	}

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
	if( tMapData.IsValidConf() && m_apConfInst[tMapData.GetInsId() - 1] != NULL )
	{
		memcpy( pConfAllMtInfo, &m_apConfInst[tMapData.GetInsId() - 1]->m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
		return TRUE;
	}

	return FALSE;
}

/*====================================================================
    函数名      ：GetConfMtTable
    功能        ：得到指定索引位置的一个会议终端表
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx：索引位置
				  TConfMtTable * ptMtTable：传入的数据空间
    返回值说明  ：TRUE - 成功 FALSE - 不存在
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/08    3.0         胡昌威        创建
====================================================================*/
BOOL32 CMcuVcData::GetConfMtTable( u8 byConfIdx, TConfMtTable * ptMtTable ) const
{
    CHECK_CONFIDX(byConfIdx)

	if( ptMtTable == NULL )
	{
		return FALSE;
	}

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
	if( tMapData.IsValidConf() && m_apConfInst[tMapData.GetInsId() - 1] != NULL )
	{
		memcpy( ptMtTable, m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable, sizeof( TConfMtTable ) );
		return TRUE;
	}

    return FALSE;
}

/*====================================================================
    函数名      ：GetConfProtectInfo
    功能        ：得到指定索引位置的一个会议保护信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx：索引位置，为避免操作失误
				  TConfProtectInfo *tConfProtectInfo：传入的数据空间
    返回值说明  ：TRUE - 成功 FALSE - 不存在
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/08    3.0         胡昌威        创建
====================================================================*/
BOOL32 CMcuVcData::GetConfProtectInfo( u8 byConfIdx, TConfProtectInfo *ptConfProtectInfo ) const
{
    CHECK_CONFIDX(byConfIdx)
	
	if(ptConfProtectInfo == NULL )
	{
		return FALSE;
	}

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
	if( tMapData.IsValidConf() && m_apConfInst[tMapData.GetInsId() - 1] != NULL )
	{
		memcpy( ptConfProtectInfo, &m_apConfInst[tMapData.GetInsId() - 1]->m_tConfProtectInfo , sizeof( TConfProtectInfo ) );
		return TRUE;
	}

    return FALSE;
}

/*====================================================================
    函数名      ：GetConfMtTable
    功能        ：得到指定索引位置的一个会议终端表
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx：索引位置
    返回值说明  ：会议存在 - 会议终端表指针 会议不存在 - NULL
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/03/19    1.0         Qzj           创建
	03/11/08    3.0         胡昌威        修改
====================================================================*/
TConfMtTable * CMcuVcData::GetConfMtTable( u8 byConfIdx ) const
{
	if ( byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX )
	{
		OspPrintf( TRUE, FALSE, "[DATA]: serious error occur in GetConfMtTable(), ConfIdx: %d \n", byConfIdx );
		return NULL;
	}

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
	if( tMapData.IsValidConf() && NULL != m_apConfInst[tMapData.GetInsId() - 1] )
	{
		return m_apConfInst[tMapData.GetInsId() - 1]->m_ptMtTable;
	}
	
	return NULL;

}

/*====================================================================
    函数名      ：GetConfSwitchTable
    功能        ：得到指定索引位置的一个会议交换信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx：索引位置，为避免操作失误
    返回值说明  ：会议存在 - 会议交换表指针 会议不存在 - NULL
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/08    3.0         胡昌威        创建
====================================================================*/
TConfSwitchTable * CMcuVcData::GetConfSwitchTable( u8 byConfIdx ) const
{
	if( byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX )
	{
		return NULL;
	}

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if( tMapData.IsValidConf() && NULL != m_apConfInst[tMapData.GetInsId() - 1] )
    {
        return m_apConfInst[tMapData.GetInsId() - 1]->m_ptSwitchTable;        
    }

	return NULL;
}

/*====================================================================
    函数名      ：GetConfEqpModule
    功能        ：得到指定索引位置的一个会议设备模板
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx：索引位置，为避免操作失误
    返回值说明  ：会议存在 - 会议交换表指针 会议不存在 - NULL
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/08    3.0         胡昌威        创建
====================================================================*/
TConfEqpModule *CMcuVcData::GetConfEqpModule( u8 byConfIdx ) const
{
    CHECK_CONFIDX(byConfIdx)
	
    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if( tMapData.IsValidConf() && m_apConfInst[tMapData.GetInsId() - 1]!=NULL )
	{
		return &m_apConfInst[tMapData.GetInsId() - 1]->m_tConfEqpModule;
	}

	return NULL;
}

/*====================================================================
    函数名      ：GetConfAllMtInfo
    功能        ：得到指定索引位置的一个会议所有终端信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx：索引位置，为避免操作失误
    返回值说明  ：会议存在 - 会议交换表指针 会议不存在 - NULL
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/08    3.0         胡昌威        创建
====================================================================*/
TConfAllMtInfo *CMcuVcData::GetConfAllMtInfo( u8 byConfIdx ) const
{
    CHECK_CONFIDX(byConfIdx)

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
	if( tMapData.IsValidConf() &&  m_apConfInst[tMapData.GetInsId() - 1]!=NULL )
	{
		return &m_apConfInst[tMapData.GetInsId() - 1]->m_tConfAllMtInfo;
	}

	return NULL;
}

/*====================================================================
    函数名      ：GetConfProtectInfo
    功能        ：得到指定索引位置的一个会议保护信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx：索引位置，为避免操作失误
    返回值说明  ：会议存在 - 会议交换表指针 会议不存在 - NULL
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/08    3.0         胡昌威        创建
====================================================================*/
TConfProtectInfo *CMcuVcData::GetConfProtectInfo( u8 byConfIdx ) const
{
    CHECK_CONFIDX(byConfIdx)

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
	if( tMapData.IsValidConf() && m_apConfInst[tMapData.GetInsId() - 1]!=NULL )
	{
		return &m_apConfInst[tMapData.GetInsId() - 1]->m_tConfProtectInfo;
	}

	return NULL;
}
	
/*====================================================================
    函数名      ：SaveConfToFile
    功能        ：把会议存储到文件 
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx：索引位置
    返回值说明  ：操作成功 - TRUE  操作失败 - FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/08    3.0         胡昌威        修改
====================================================================*/
BOOL32 CMcuVcData::SaveConfToFile( u8 byConfIdx, BOOL32 bTemplate, BOOL32 bDefaultConf )
{
	CHECK_CONFIDX(byConfIdx)

	TConfStore tConfStore;

	if (!bTemplate)
	{        
		if (m_atConfMapData[byConfIdx-MIN_CONFIDX].IsValidConf())
		{
			//get tConfInfo
			if (NULL == GetConfInstHandle(byConfIdx))
			{
				OspPrintf(TRUE, FALSE, "[CMcuVcData]: GetConfInstHandle NULL! -- %d\n", byConfIdx);
				return FALSE;
			}
			TConfInfo *ptConfFullInfo = &GetConfInstHandle(byConfIdx)->m_tConf;
			tConfStore.m_tConfInfo = *ptConfFullInfo;
			if (CONF_LOCKMODE_LOCK == tConfStore.m_tConfInfo.m_tStatus.GetProtectMode())
			{
				tConfStore.m_tConfInfo.m_tStatus.SetProtectMode(CONF_LOCKMODE_NONE);
			}

            //保存即时会议时，清空当前会议状态中的外设状态
            tConfStore.m_tConfInfo.m_tStatus.SetNoMixing();
            tConfStore.m_tConfInfo.m_tStatus.SetVACing(FALSE);
            tConfStore.m_tConfInfo.m_tStatus.SetNoPlaying();
            tConfStore.m_tConfInfo.m_tStatus.SetNoRecording();
            tConfStore.m_tConfInfo.m_tStatus.SetVMPMode(CONF_VMPMODE_NONE);
            tConfStore.m_tConfInfo.m_tStatus.SetVmpTwMode(CONF_VMPTWMODE_NONE);
            tConfStore.m_tConfInfo.m_tStatus.SetPollMode(CONF_POLLMODE_NONE);
            tConfStore.m_tConfInfo.m_tStatus.SetTvWallPollState(POLL_STATE_NONE);
            
            //zbq [09/24/2007] 保存即时会议，清空当前的BAS状态
            if ( tConfStore.m_tConfInfo.m_tStatus.IsAudAdapting() )
            {
                tConfStore.m_tConfInfo.m_tStatus.SetAdaptMode(CONF_BASMODE_AUD, FALSE);
            }
            if ( tConfStore.m_tConfInfo.m_tStatus.IsVidAdapting() )
            {
                tConfStore.m_tConfInfo.m_tStatus.SetAdaptMode(CONF_BASMODE_VID, FALSE);
            }
            if ( tConfStore.m_tConfInfo.m_tStatus.IsBrAdapting() )
            {
                tConfStore.m_tConfInfo.m_tStatus.SetAdaptMode(CONF_BASMODE_BR, FALSE);
            }
            if ( tConfStore.m_tConfInfo.m_tStatus.IsCasdAudAdapting() )
            {
                tConfStore.m_tConfInfo.m_tStatus.SetAdaptMode(CONF_BASMODE_CASDAUD, FALSE);
            }
            if ( tConfStore.m_tConfInfo.m_tStatus.IsCasdVidAdapting() )
            {
                tConfStore.m_tConfInfo.m_tStatus.SetAdaptMode(CONF_BASMODE_CASDVID, FALSE);
            }
            // 清除高清适配状态, zgc, 2008-08-09
            if ( tConfStore.m_tConfInfo.m_tStatus.IsHdVidAdapting() )
            {
                tConfStore.m_tConfInfo.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_VID, FALSE);
            }
            
			TConfEqpModule tConfEqpModule = *GetConfEqpModule(byConfIdx);
			tConfStore.m_tMultiTvWallModule = tConfEqpModule.m_tMultiTvWallModule; 
			tConfStore.m_atVmpModule = tConfEqpModule.GetVmpModule();
            
            tConfStore.EmptyAllTvMember();
            tConfStore.EmptyAllVmpMember();

			//get alias array
            u8 byMemberType = 0;
			TMt tMt;
			TMtAlias tMtAlias;
			TConfMtTable *ptMtTable = GetConfMtTable(byConfIdx);
            // guzh [5/31/2007] 当级联时有跳过终端，需要另外保存索引
            u8 byStoreIdx = 0;
			for (u8 byLoop = 0; byLoop < ptMtTable->m_byMaxNumInUse; byLoop++)
			{
				tMt = ptMtTable->GetMt(byLoop+1);
				if (!tMt.IsNull())
				{
					if (MT_TYPE_MMCU == tMt.GetMtType())
					{
						continue;
					}

                    //zbq [08/09/2007] 优先保存终端的呼叫别名；然后依次保存E164/H323ID，IP.
                    if (ptMtTable->GetDialAlias((byLoop+1), &tMtAlias))
                    {
                        tConfStore.m_atMtAlias[byStoreIdx] = tMtAlias;
                    }
                    else
                    {
                        //由于mtAliasTypeH320Alias类型不能用作呼叫信息，所以不进行保存
					    if (ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeH320ID, &tMtAlias))
					    {
						    tConfStore.m_atMtAlias[byStoreIdx] = tMtAlias;
					    }
					    else if (ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeE164, &tMtAlias))
					    {
						    tConfStore.m_atMtAlias[byStoreIdx] = tMtAlias;			
					    }
					    else if (ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeH323ID, &tMtAlias))
                        {
                            tConfStore.m_atMtAlias[byStoreIdx] = tMtAlias;
					    }
					    else if (ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeTransportAddress, &tMtAlias))
					    {
						    tConfStore.m_atMtAlias[byStoreIdx] = tMtAlias;
					    }
                    }

                    tConfStore.m_awMtDialBitRate[byStoreIdx] = ptMtTable->GetDialBitrate(byLoop+1);
            
					//更新模板映射关系
					if (tConfStore.m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
					{
						for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL; byTvLp++)
                        {                            
                            u8 byTvId = tConfEqpModule.m_tTvWallInfo[byTvLp].m_tTvWallEqp.GetEqpId();
                            for(u8 byTvChlLp = 0; byTvChlLp < MAXNUM_PERIEQP_CHNNL; byTvChlLp++)
                            {
                                if(TRUE == tConfEqpModule.IsMtInTvWallChannel(byTvId, byTvChlLp, tMt, byMemberType) )
                                {
                                    tConfStore.SetMtInTvChannel(byTvId, byTvChlLp, byStoreIdx+1, byMemberType);
                                }
                            }
                        }
					}
					if (tConfStore.m_tConfInfo.GetConfAttrb().IsHasVmpModule())
					{
                        for (u8 byVmpIdx = 0; byVmpIdx < MAXNUM_MPUSVMP_MEMBER; byVmpIdx++)
                        {
                            if( tConfEqpModule.IsMtAtVmpChannel(byVmpIdx, tMt, byMemberType) )
						    {
                                tConfStore.m_atVmpModule.SetVmpMember(byVmpIdx, byStoreIdx+1, byMemberType);
						    }
                        }
					}            
                    byStoreIdx ++;
				}                
			}
            tConfStore.m_byMtNum = byStoreIdx;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if(!m_atConfMapData[byConfIdx-MIN_CONFIDX].IsTemUsed())
		{
			return FALSE;
		}
        // zbq [09/06/2007] 所有模板信息保存清空bFromFile
        u8 bySaveTmpIdx = m_atConfMapData[byConfIdx-MIN_CONFIDX].GetTemIndex();
        m_ptTemplateInfo[bySaveTmpIdx].m_tConfInfo.m_tStatus.SetTakeFromFile(FALSE);
		memcpy(&tConfStore, &m_ptTemplateInfo[bySaveTmpIdx], sizeof(TConfStore));
	}

	return AddConfToFile(tConfStore, bDefaultConf);
}

/*====================================================================
    函数名      ：RemoveConfFromFile
    功能        ：从文件删除会议
    算法实现    ：
    引用全局变量：
    输入参数说明：CConfId cConfId 要删除的会议号
    返回值说明  ：操作成功 - TRUE  操作失败 - FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/08    3.0         胡昌威        修改
====================================================================*/
BOOL32 CMcuVcData::RemoveConfFromFile( CConfId cConfId )
{
	if (cConfId.IsNull())
	{
		return FALSE;
	}

	return DeleteConfFromFile(cConfId);
}

/*====================================================================
    函数名      ：GetConfFullInfo
    功能        ：得到指定索引位置的一个完整会议信息
    算法实现    ：
    引用全局变量：
    输入参数说明：[IN]  u8 byConfIdx：索引位置
	              [IN]  TPackConfStore *ptPackConfStore 大小为预留的 sizeof(TConfStore)
    返回值说明  ：操作成功 - TRUE  操作失败 - FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/08    3.0         胡昌威        修改
====================================================================*/
BOOL32 CMcuVcData::GetConfFromFile( u8 byConfIdx, TPackConfStore *ptPackConfStore )
{
    return ::GetConfFromFile((byConfIdx-MIN_CONFIDX), ptPackConfStore);
}

/*=============================================================================
  函 数 名： InitPeriDcsList
  功    能： 初始化DCS列表：代理支持不够，目前直接构造初始结构
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/15    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcData::InitPeriDcsList()
{
	u8  byDcsId = 0;
    u32 dwDcsIp = 0;
	memset( m_atPeriDcsTable, 0, sizeof(m_atPeriDcsTable) );
	
    for ( byDcsId = 1; byDcsId <= MAXNUM_MCU_DCS; byDcsId ++ )
    {
        if ( 0 != g_cMcuAgent.GetDcsIp() )
        {
            m_atPeriDcsTable[byDcsId-1].m_bIsValid = TRUE;
            m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.SetType( TYPE_MCUPERI );
            m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.SetEqpType( EQP_TYPE_DCS );
            m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.SetEqpId(byDcsId);
            m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.SetMcuId(LOCAL_MCUID);        
            m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.m_byOnline = FALSE;
            
            //MCU给DCS主动构造别名
            g_cMcuVcApp.SetDcsAlias( byDcsId );
            m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.SetDcsAlias(g_cMcuVcApp.GetDcsAlias(byDcsId));
        }        
    }
    return;
}

/*=============================================================================
  函 数 名： SetPeriDcsConnected
  功    能： 设置DCS的连接状态
  算法实现： 
  全局变量： 
  参    数：  u8 byDcsId
             BOOL32 bConnected
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/15    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcData::SetPeriDcsConnected( u8 byDcsId, BOOL32 bConnected )
{
	if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS )
	{
		return;
	}	
	m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.m_byOnline = bConnected;
	
	return;
}

/*=============================================================================
  函 数 名： IsPeriDcsConnected
  功    能： 判断DCS是否连接
  算法实现： 
  全局变量： 
  参    数： u8 byDcsId
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/15    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsPeriDcsConnected( u8 byDcsId )
{
	if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS )
	{
		return FALSE;
	}
	return( m_atPeriDcsTable[byDcsId - 1].m_tDcsStatus.m_byOnline );
}

/*=============================================================================
  函 数 名： SetPeriDcsValid
  功    能： 设置DCS配置状态
  算法实现： 
  全局变量： 
  参    数： u8 byDcsId
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/26    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcData::SetPeriDcsValid( u8 byDcsId )
{
    if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS )
	{
		return;
	}

	m_atPeriDcsTable[byDcsId-1].m_bIsValid = TRUE;
}

/*=============================================================================
  函 数 名： IsPeriDcsValid
  功    能： 判断DCS是否配置
  算法实现： 
  全局变量： 
  参    数： u8 byDcsId
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/15    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsPeriDcsValid( u8 byDcsId )
{
    if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS )
	{
		return FALSE;
	}

	return m_atPeriDcsTable[byDcsId-1].m_bIsValid;
}

/*=============================================================================
  函 数 名： SetDcsAlias
  功    能： 设置DCS的别名
  算法实现： 
  全局变量： 
  参    数：  u8 byDcsId
             LPCSTR lpszDcsAlias
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/15    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcData::SetDcsAlias( u8 byDcsId )
{
	if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS )
	{
		return;
	}
	s8 achDcsName[MAXLEN_EQP_ALIAS];
	sprintf( achDcsName, "%s%d", "dcs", byDcsId );
	m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.SetDcsAlias( achDcsName );
}

/*=============================================================================
  函 数 名： GetDcsAlias
  功    能： 获得DCS的别名
  算法实现： 
  全局变量： 
  参    数： u8 byDcsId
  返 回 值： LPCSTR 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/15    4.0			张宝卿                  创建
=============================================================================*/
LPCSTR CMcuVcData::GetDcsAlias( u8 byDcsId )
{
	if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS )
	{
		return NULL;
	}
	return m_atPeriDcsTable[byDcsId-1].m_tDcsStatus.GetDcsAlias();
}

/*=============================================================================
  函 数 名： IsDcsConfiged
  功    能： DCS是否配置了
  算法实现： 目前代理只支持一个DCS
  全局变量： 
  参    数： u32 dwDcsIp
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/1/10    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsPeriDcsConfiged( u32 dwDcsIp )
{
	for( s32 nIndex = 0; nIndex < MAXNUM_MCU_DCS; nIndex ++ )
	{
		if ( dwDcsIp ==  ntohl(g_cMcuAgent.GetDcsIp()) )
		{
			return TRUE;
		}
	}
	return FALSE;
}


/*=============================================================================
    函数名      ：GetPeriDcsStatus
    功能        ：获得DCS的状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byDcsId：DCS ID
				  TPeriDcsStatus * ptStatus, 返回的DCS状态
    返回值说明  ：无
-------------------------------------------------------------------------------
    修改记录    ：
    日  期       版本           修改人                修改内容
    2006/6/12    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetPeriDcsStatus( u8 byDcsId, TPeriDcsStatus * ptStatus )
{
	if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS || NULL == ptStatus )
	{
		return FALSE;
	}

	*ptStatus = m_atPeriDcsTable[byDcsId - 1].m_tDcsStatus;

	return TRUE;
}

/*=============================================================================
    函数名      ：SetPeriDcsStatus
    功能        ：设置DCS的状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byDcsId：DCS ID
				  TPeriDcsStatus * ptStatus
    返回值说明  ：无
-------------------------------------------------------------------------------
    修改记录    ：
    日  期       版本           修改人                修改内容
    2006/6/12    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetPeriDcsStatus( u8 byDcsId, const TPeriDcsStatus * ptStatus )
{
	if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS || NULL == ptStatus )
	{
		return FALSE;
	}

	m_atPeriDcsTable[byDcsId - 1].m_tDcsStatus = *ptStatus;
    return TRUE;
}

/*=============================================================================
    函数名      ：GetDcs
    功能        ：获得DCS结构体
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byDcsId：DCS ID
    返回值说明  ：TEqp
-------------------------------------------------------------------------------
    修改记录    ：
    日  期       版本           修改人                修改内容
    2006/6/12    4.0			张宝卿                  创建
=============================================================================*/
TEqp CMcuVcData::GetDcs( u8 byDcsId )
{
    TEqp tEqp;
    tEqp.SetNull();
    if( byDcsId == 0 || byDcsId > MAXNUM_MCU_DCS || !IsPeriDcsValid( byDcsId ) )
    {
        return tEqp;
    }
    
    return (TEqp)m_atPeriDcsTable[byDcsId-1].m_tDcsStatus;
}

/*====================================================================
    函数名      ：SetPeriEqpConnected
    功能        ：设置外设连接状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId, 外设 ID
				  BOOL32 bConnected, 是否连接
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/06    1.0         杨皞昀         创建
====================================================================*/
void CMcuVcData::SetPeriEqpConnected( u8 byEqpId, BOOL32 bConnected )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP )
	{
		return;
	}

	m_atPeriEqpTable[byEqpId - 1].m_tPeriEqpStatus.m_byOnline = bConnected;

	return;
}

/*====================================================================
    函数名      ：SetPeriEqpLogicChnnl
    功能        ：设置外设的逻辑通道
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId, 外设 ID
				  u8 byMediaType, 媒体类型
				  u8 byChnnlNum, MCU至外设通道数
				  TLogicalChannel * ptChnnl, 逻辑通道配置
				  BOOL32 bForwardChnnl, 流向
    输出参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/06    1.0         杨皞昀         创建
====================================================================*/
void CMcuVcData::SetPeriEqpLogicChnnl( u8 byEqpId, u8 byMediaType, u8 byChnnlNum, const TLogicalChannel * ptStartChnnl, BOOL32 bForwardChnnl )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || NULL == ptStartChnnl )
	{
		return ;
	}

	switch( byMediaType )
	{
	case MODE_VIDEO:
		if( bForwardChnnl )
		{
			m_atPeriEqpTable[byEqpId - 1].m_byFwdChannelNum = byChnnlNum;
			m_atPeriEqpTable[byEqpId - 1].m_tFwdVideoChannel = *ptStartChnnl;
		}
		else
		{
			m_atPeriEqpTable[byEqpId - 1].m_byRvsChannelNum = byChnnlNum;
			m_atPeriEqpTable[byEqpId - 1].m_tRvsVideoChannel = *ptStartChnnl;
		}
		break;
	case MODE_AUDIO:
		if( bForwardChnnl )		
		{
			m_atPeriEqpTable[byEqpId - 1].m_byFwdChannelNum = byChnnlNum;
			m_atPeriEqpTable[byEqpId - 1].m_tFwdAudioChannel = *ptStartChnnl;
		}
		else
		{
			m_atPeriEqpTable[byEqpId - 1].m_byRvsChannelNum = byChnnlNum;
			m_atPeriEqpTable[byEqpId - 1].m_tRvsAudioChannel = *ptStartChnnl;
		}
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::SetPeriEqpLogicChnnl(): wrong Mode %u!\n", byMediaType );
		break;
	}

	return;

}

/*====================================================================
    函数名      ：SetPeriEqpStatus
    功能        ：设置外设状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId, 外设 ID
				  const TPeriEqpStatus * ptStatus, 外设状态
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/10    1.0         杨皞昀         创建
====================================================================*/
void CMcuVcData::SetPeriEqpStatus( u8 byEqpId, const TPeriEqpStatus * ptStatus )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || NULL == ptStatus )
	{
		return ;
	}

	m_atPeriEqpTable[byEqpId - 1].m_tPeriEqpStatus = *ptStatus;
	
}

/************************************************************************/
/* 适配器状态信息的读写处理                                             */
/************************************************************************/
/*====================================================================
    函数名      ：SetBasChanStatus
    功能        ：设置BAS通道状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId, 外设 ID
				  u8 byChanNo, EBasStatus eStatus
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/09/07    4.0         libo          创建
====================================================================*/
//modify bas 2
void CMcuVcData::SetBasChanStatus(u8 byEqpId, u8 byChanNo, u8 byStatus)
{
	if (0 == byEqpId || byEqpId > MAXNUM_MCU_PERIEQP || byChanNo > MAXNUM_BAS_CHNNL)
	{
		return;
	}

    TBasStatus *ptBasStatus;
    ptBasStatus = &m_atPeriEqpTable[byEqpId - 1].m_tPeriEqpStatus.m_tStatus.tBas;
    ptBasStatus->tChnnl[byChanNo].SetStatus(byStatus);
}

/*====================================================================
    函数名      ：SetBasChanReserved
    功能        ：设置BAS通道预留情况
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId, 外设 ID
				  u8 byChanNo, EBasStatus eStatus
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/09/07    4.0         libo          创建
====================================================================*/
void CMcuVcData::SetBasChanReserved(u8 byEqpId, u8 byChanNo, BOOL32 bReserved)
{
    if (0 == byEqpId || byEqpId > MAXNUM_MCU_PERIEQP || byChanNo > MAXNUM_BAS_CHNNL)
	{
		return;
	}

    TBasStatus *ptBasStatus;
    ptBasStatus = &m_atPeriEqpTable[byEqpId - 1].m_tPeriEqpStatus.m_tStatus.tBas;
    ptBasStatus->tChnnl[byChanNo].SetReserved(bReserved);
}

/*====================================================================
    函数名      ：SetMcConnected
    功能        ：设置MC连接状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wMcInstId, 会议控制台实例号
				  BOOL32 bConnected, 是否连接
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/06    1.0         杨皞昀         创建
====================================================================*/
void CMcuVcData::SetMcConnected( u16 wMcInstId, BOOL32 bConnected )
{
	if( wMcInstId == 0 || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) )
	{
		return;
	}

	m_atMcTable[wMcInstId-1].m_bConnected = bConnected;

	return;
}

/*====================================================================
    函数名      ：SetMcConnected
    功能        ：设置MC连接状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wMcInstId, 会议控制台实例号
				  BOOL32 bConnected, 是否连接
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/06    1.0         杨皞昀         创建
====================================================================*/
void CMcuVcData::SetMcsRegInfo( u16 wMcInstId, TMcsRegInfo tMcsRegInfo )
{
	if( wMcInstId == 0 || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) )
	{
		return;
	}

	m_atMcTable[wMcInstId-1].m_tMcsRegInfo = tMcsRegInfo;
}

/*====================================================================
    函数名      ：SetMcConnected
    功能        ：设置MC连接状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wMcInstId, 会议控制台实例号
				  BOOL32 bConnected, 是否连接
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/06    1.0         杨皞昀         创建
====================================================================*/
void CMcuVcData::GetMcsRegInfo( u16 wMcInstId, TMcsRegInfo *ptMcsRegInfo )
{
	if( wMcInstId == 0 || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) || ptMcsRegInfo == NULL )
	{
		return;
	}

	*ptMcsRegInfo = m_atMcTable[wMcInstId-1].m_tMcsRegInfo;
}

/*====================================================================
    函数名      ：SetMcLogicChnnl
    功能        ：设置MC的逻辑通道
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wMcInstId, MC号
				  u8 byMediaType, 媒体类型
				  u8 byChnnlNum, MCU至MC通道数
				  TLogicalChannel * ptStartChnnl, 逻辑通道配置
    输出参数说明:		无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/06    1.0         杨皞昀         创建
====================================================================*/
void CMcuVcData::SetMcLogicChnnl( u16 wMcInstId, u8 byMediaType, u8 byChnnlNum, const TLogicalChannel * ptStartChnnl )
{
	if( wMcInstId == 0 || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) || NULL == ptStartChnnl )
	{
		return ;
	}

	switch( byMediaType )
	{
	case MODE_VIDEO:
		m_atMcTable[ wMcInstId-1 ].m_tFwdVideoChannel = *ptStartChnnl;
		break;
	case MODE_AUDIO:
		m_atMcTable[ wMcInstId-1 ].m_tFwdAudioChannel = *ptStartChnnl;
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::SetMcLogicChnnl(): wrong Mode %u!\n", byMediaType );
		break;
	}
	m_atMcTable[ wMcInstId-1 ].m_byFwdChannelNum = byChnnlNum;

	return;	
}


/*====================================================================
    函数名      ：SetMcSrc
    功能        ：设置MC的发送/接收视频或音频源
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wMcInstId, MC号
				  TMt * ptSrc, 终端源
				  u8 byChnnlNo, 通道号
				  u8 byMode, 流模式MODE_VIDEO/MODE_AUDIO
    返回值说明  ：none
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/06    1.0         杨皞昀         创建
====================================================================*/
void CMcuVcData::SetMcSrc( u16 wMcInstId, const TMt * ptSrc, u8 byChnnlNo, u8 byMode )
{
	if( wMcInstId == 0 || wMcInstId > (MAXNUM_MCU_MC + MAXNUM_MCU_VC) || NULL == ptSrc || byChnnlNo >= MAXNUM_MC_CHANNL )
	{
		return ;
	}

	switch( byMode )
	{
	case MODE_VIDEO:
		m_atMcTable[ wMcInstId-1 ].m_atVidSrc[byChnnlNo] = *ptSrc;
		break;
	case MODE_AUDIO:
		m_atMcTable[ wMcInstId-1 ].m_atAudSrc[byChnnlNo] = *ptSrc;
		break;
	case MODE_BOTH:
		m_atMcTable[ wMcInstId-1 ].m_atVidSrc[byChnnlNo] = *ptSrc;	
		m_atMcTable[ wMcInstId-1 ].m_atAudSrc[byChnnlNo] = *ptSrc;
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::SetMcSrc(): wrong Mode %u!\n", byMode );
		break;
	}

	return;
}

/*====================================================================
    函数名      ：SetPeriEqpSrc
    功能        ：设置PeriEqp的发送/接收视频或音频源
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 periEqpId, Eqp号
				  TMt * ptSrc, 终端源
				  u8 byChnnlNo, 通道号
				  u8 byMode, 流模式MODE_VIDEO/MODE_AUDIO
    返回值说明  ：none
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/30    1.0         JQL           创建
====================================================================*/
void CMcuVcData::SetPeriEqpSrc( u8 byEqpId, const TMt * ptSrc, u8 byChnnlNo, u8 byMode )
{
	
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP 
		|| NULL == ptSrc || byChnnlNo >= MAXNUM_PERIEQP_CHNNL )
	{
		return ;
	}

	switch( byMode )
	{
	case MODE_VIDEO:
		m_atPeriEqpTable[ byEqpId - 1 ].m_atVidSrc[byChnnlNo] = *ptSrc;
		break;
	case MODE_AUDIO:
		m_atPeriEqpTable[ byEqpId - 1 ].m_atAudSrc[byChnnlNo] = *ptSrc;
		break;
	case MODE_BOTH:
		m_atPeriEqpTable[ byEqpId - 1 ].m_atVidSrc[byChnnlNo] = *ptSrc;
		m_atPeriEqpTable[ byEqpId - 1 ].m_atAudSrc[byChnnlNo] = *ptSrc;
		break;
	default:
		OspPrintf( TRUE, FALSE, "Exception - CMcuVcData::SetMcSrc(): wrong Mode %u!\n", byMode );
		break;
	}

	return;
}


/*====================================================================
    函数名      ：AddMp
    功能        ：增加一个Mp
    算法实现    ：
    引用全局变量：
    输入参数说明：tMp Mp信息
    返回值说明  ：成功TRUE,失败FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
BOOL32 CMcuVcData::AddMp( TMp tMp )
{
    if (0 == tMp.GetMpId() || tMp.GetMpId() > MAXNUM_DRI)
    {
        return FALSE;
    }

    m_atMpData[tMp.GetMpId() - 1].m_bConnected = TRUE;
	m_atMpData[tMp.GetMpId() - 1].m_tMp = tMp; 
    
    // guzh [1/17/2007] 根据MP类型确定其码流限制的能力
    u16 wMaxBand  = 0;
    u32 dwMaxPkNum = 0;
#ifdef _MINIMCU_
    // 对于8000B，要区分 MPC/MDSC/HDSC
    if ( tMp.GetAttachMode() == 1 )
    {
        wMaxBand = g_cMcuVcApp.GetPerfLimit().m_wMpcMaxMpAbility;
        dwMaxPkNum = g_cMcuVcApp.GetPerfLimit().m_dwMpcMaxPkNum;
    }
    else
    {
        // 为了防止用户配置错误，直接采用内部默认值
        u8 byType = g_cMcuAgent.GetRunBrdTypeByIdx( tMp.GetMpId() );
        if ( byType == 0xFF )
        {
            wMaxBand = 0;
            dwMaxPkNum = 0;
        }
        else if ( byType == DSL8000_BRD_HDSC )
        {
            wMaxBand = MAXLIMIT_CRI_MP_HDSC;
            dwMaxPkNum = MAXLIMIT_CRI_MP_HDSC_PMNUM * 1024;
        }
        else
        {
            wMaxBand = MAXLIMIT_CRI_MP_MDSC;
            dwMaxPkNum = MAXLIMIT_CRI_MP_MDSC_PMNUM * 1024;
        }
    }
#else
    if ( tMp.GetAttachMode() == 1 )
    {
        wMaxBand = g_cMcuVcApp.GetPerfLimit().m_wMpcMaxMpAbility;
        dwMaxPkNum = g_cMcuVcApp.GetPerfLimit().m_dwMpcMaxPkNum;
    }
    else
    {
        wMaxBand = g_cMcuVcApp.GetPerfLimit().m_wCriMaxMpAbility;
        dwMaxPkNum = g_cMcuVcApp.GetPerfLimit().m_dwCriMaxPkNum;
    }
#endif
    m_atMpData[tMp.GetMpId()-1].m_wNetBandAllowed = wMaxBand;
    m_atMpData[tMp.GetMpId()-1].m_dwPkNumAllowed = dwMaxPkNum;

	return TRUE;
}

/*====================================================================
    函数名      ：RemoveMp
    功能        ：移除Mp
    算法实现    ：
    引用全局变量：
    输入参数说明：byMpId Mp编号	          
    返回值说明  ：成功TRUE,失败FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
BOOL32 CMcuVcData::RemoveMp( u8 byMpId )
{
	if(!IsMpConnected(byMpId))
	{
		return FALSE;
	}

    m_atMpData[byMpId - 1].m_bConnected = FALSE;
	memset( &m_atMpData[byMpId - 1].m_tMp, 0, sizeof(TMp) );
    m_atMpData[byMpId - 1].m_wMtNum = 0;
    memset( m_atMpData[byMpId - 1].m_abyMtId, 0, sizeof(m_atMpData[byMpId - 1].m_abyMtId) );
    m_atMpData[byMpId - 1].m_wNetBandAllowed = 0;

	return TRUE;
}

/*====================================================================
    函数名      ：IsMpConnected
    功能        ：判断Mp是否已连接
    算法实现    ：
    引用全局变量：
    输入参数说明：byMpId Mp编号
    返回值说明  ：TRUE - 连接, FALSE - 未连接
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
BOOL32 CMcuVcData::IsMpConnected( u8 byMpId )
{
    if (0 == byMpId || byMpId > MAXNUM_DRI)
    {
        return FALSE;
    }

    return m_atMpData[byMpId - 1].m_bConnected;
}

/*====================================================================
    函数名      ：GetMpIpAddr
    功能        ：得到Mp的Ip地址 
    算法实现    ：
    引用全局变量：
    输入参数说明：byMpId Mp编号
	              byIpNO 第几个IP
    返回值说明  ：成功返回IP地址, 失败0
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
u32  CMcuVcData::GetMpIpAddr( u8 byMpId )
{
    if(!IsMpConnected(byMpId))
	{
		return 0;
	}

	return m_atMpData[byMpId - 1].m_tMp.GetIpAddr( );
}

/*====================================================================
    函数名      ：GetMpNum
    功能        ：得到Mp的数目
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：Mp的数量
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
u8   CMcuVcData::GetMpNum( void )
{
	u8 byMpNum = 0;
    for(u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++ )
	{
        if(m_atMpData[byLoop].m_bConnected)
		{
		    byMpNum++;
		}
	}
	return byMpNum;
}

/*====================================================================
    函数名      ：FindMp
    功能        ：查找Mp
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 dwIp   ip地址，主机序
    返回值说明  ：查找Mp的Id
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
u8  CMcuVcData::FindMp( u32 dwIp )
{
	u8 byMpId = 0;
	int nMpId = 0;
	while( nMpId < MAXNUM_DRI && byMpId == 0)
	{
		//跳过未连接的
		if(!m_atMpData[nMpId].m_bConnected)
		{
			nMpId++;
			continue;
		}

		//查找是否是此Mp
		if( m_atMpData[nMpId].m_tMp.GetIpAddr( ) == dwIp )
		{
			byMpId = nMpId+1;
			break;
		}
		nMpId++;
	}

	return byMpId;
}

/*====================================================================
    函数名      ：GetValidMp
    功能        ：得到一个Mp
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：查找Mp的Id
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/10/21    3.0         胡昌威        创建
====================================================================*/
u32  CMcuVcData::GetAnyValidMp( void )
{
	int nMpId = 0;
	while( nMpId < MAXNUM_DRI )
	{
		//跳过未连接的
		if(!m_atMpData[nMpId].m_bConnected)
		{
			nMpId++;
			continue;
		}

		return m_atMpData[nMpId].m_tMp.GetIpAddr( );

	}

	return 0;	
}

u8   CMcuVcData::GetMpMulticast(u8 byMpId)
{
    if (byMpId == 0 || byMpId > MAXNUM_DRI || !m_atMpData[byMpId - 1].m_bConnected)
    {
        return 0;
    }

    return m_atMpData[byMpId - 1].m_tMp.GetMulticast();
}

/*====================================================================
    函数名      ：ShowDri
    功能        ：显示DRI信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：u8 byDriId DRI编号
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/04/02    3.0         胡昌威        创建
    06/11/09    4.0         许世林        修改
====================================================================*/
void  CMcuVcData::ShowDri( u8 byDriId )
{	
	if( byDriId == 0 )
	{
		for( u8 byLoop = 1; byLoop <= MAXNUM_DRI; byLoop++ )
		{	        
			if( m_atMpData[byLoop-1].m_bConnected )
			{
				OspPrintf( TRUE, FALSE, "\n============= MP%d: TotalMtNum:%d ===========\n", 
                    byLoop, m_atMpData[byLoop-1].m_wMtNum );

                for( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
	            { 		
		            if( NULL != g_cMcuVcApp.GetConfInstHandle( byConfIdx ))
		            { 
                        TConfInfo  *ptConfFullInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
			            OspPrintf( TRUE, FALSE, "Conf: %s \n", ptConfFullInfo->GetConfName( ) );

                        for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
                        {
                            if (m_atMpData[byLoop-1].m_abyMtId[byConfIdx-1][byIdx] > 0)
                            {
                                OspPrintf(TRUE, FALSE, "Mt.%d ", m_atMpData[byLoop-1].m_abyMtId[byConfIdx-1][byIdx]);
                            }
                        }
                        OspPrintf( TRUE, FALSE, "\n");
                    }
                }                
			}
			else if( m_atMpData[byLoop-1].m_wMtNum )
			{
				OspPrintf( TRUE, FALSE, "MP%d: %d TotalMtNum:%d (Not Online)\n", byLoop, m_atMpData[byLoop-1].m_wMtNum );
			}
			
			if( m_atMtAdpData[byLoop-1].m_bConnected )
			{
				OspPrintf( TRUE, FALSE, "============== MtAdp%d: TotalMtNum:%d  ProtocolID:%d ==========\n", 
                            byLoop, m_atMtAdpData[byLoop-1].m_wMtNum, 
						    m_atMtAdpData[byLoop-1].m_byProtocolType );	

                for( u8 byConfIdx = MIN_CONFIDX; byConfIdx < MAX_CONFIDX; byConfIdx++ )
	            { 		
		            if( NULL != g_cMcuVcApp.GetConfInstHandle( byConfIdx ))
		            { 
                        TConfInfo  *ptConfFullInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
			            OspPrintf( TRUE, FALSE, "Conf: %s \n", ptConfFullInfo->GetConfName( ) );

                        for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
                        {
                            if (m_atMtAdpData[byLoop-1].m_abyMtId[byConfIdx-1][byIdx] > 0)
                            {
                                OspPrintf(TRUE, FALSE, "Mt.%d ", m_atMtAdpData[byLoop-1].m_abyMtId[byConfIdx-1][byIdx]);
                            }
                        }
                        OspPrintf( TRUE, FALSE, "\n");
                    }
                }                
			}
			else if( m_atMtAdpData[byLoop-1].m_wMtNum )
			{
				OspPrintf( TRUE, FALSE, "MtAdp%d: TotalMtNum:%d ProtocolID:%d (Not Online)\n", byLoop, 
						   m_atMtAdpData[byLoop-1].m_wMtNum, 
						   m_atMtAdpData[byLoop-1].m_byProtocolType );	
			}
		}
	}
	else if (byDriId <= MAXNUM_DRI)
	{
		if( m_atMpData[byDriId-1].m_bConnected )
		{
			OspPrintf( TRUE, FALSE, "\n\n=========== MP%d: TotalMtNum:%d =============\n", 
                        byDriId, m_atMpData[byDriId-1].m_wMtNum );
            
            for( u8 byConfIdx = MIN_CONFIDX; byConfIdx < MAX_CONFIDX; byConfIdx++ )
	        { 		
		        if( NULL != g_cMcuVcApp.GetConfInstHandle( byConfIdx ))
		        { 
                    TConfInfo  *ptConfFullInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
			        OspPrintf( TRUE, FALSE, "Conf: %s \n", ptConfFullInfo->GetConfName( ) );

                    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
                    {
                        if (m_atMpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx] > 0)
                        {
                            OspPrintf(TRUE, FALSE, "Mt.%d ", m_atMpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx]);
                        }
                    }
                    OspPrintf( TRUE, FALSE, "\n");
                }
            }            
		}
		else if( m_atMpData[byDriId-1].m_wMtNum )
		{
			OspPrintf( TRUE, FALSE, "MP%d: TotalMtNum:%d (Not Online)\n", byDriId, m_atMpData[byDriId-1].m_wMtNum );
		}
		
		if( m_atMtAdpData[byDriId-1].m_bConnected )
		{
			OspPrintf( TRUE, FALSE, "============ MtAdp%d: TotalMtNum:%d ProtocolID:%d ============\n", 
                        byDriId, m_atMtAdpData[byDriId-1].m_wMtNum, 
					    m_atMtAdpData[byDriId-1].m_byProtocolType );	

            for( u8 byConfIdx = MIN_CONFIDX; byConfIdx < MAX_CONFIDX; byConfIdx++ )
	        { 		
		        if( NULL != g_cMcuVcApp.GetConfInstHandle( byConfIdx ))
		        { 
                    TConfInfo  *ptConfFullInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
			        OspPrintf( TRUE, FALSE, "Conf: %s \n", ptConfFullInfo->GetConfName( ) );

                    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
                    {
                        if (m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx] > 0)
                        {
                            OspPrintf(TRUE, FALSE, "Mt.%d ", m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx]);
                        }
                    }
                    OspPrintf( TRUE, FALSE, "\n");
                }
            }            
		}
		else if( m_atMtAdpData[byDriId-1].m_wMtNum )
		{
			OspPrintf( TRUE, FALSE, "MtAdp%d: TotalMtNum:%d ProtocolID:%d (Not Online)\n", byDriId, 
					   m_atMtAdpData[byDriId-1].m_wMtNum, 
					   m_atMtAdpData[byDriId-1].m_byProtocolType );	
		}
	}
}

/*====================================================================
    函数名      ：IncMtAdpMtNum
    功能        ：增加DRI终端数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：u8 byDriId DRI编号
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/04/02    3.0         胡昌威        创建
====================================================================*/
void CMcuVcData::IncMtAdpMtNum( u8 byDriId, u8 byConfIdx, u8 byMtId )
{
	if( 0 == byDriId || byDriId > MAXNUM_DRI || 0 == byMtId || 0 == byConfIdx || byConfIdx > MAX_CONFIDX )	
	{
        return;		
	}
	// xliang [1/4/2009] 上限非定值 MAXNUM_CONF_MT
	u8 byDriMaxNum = m_atMtAdpData[byDriId-1].m_byMaxMtNum;
	
    //先查找是否已经存在，并记录空位置
    u8 byEmptyIdx = byDriMaxNum/*MAXNUM_CONF_MT*/;
    for(u8 byIdx = 0; byIdx < byDriMaxNum; byIdx++)
    {      
        if ( m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx] == byMtId )
        {
            return;
        }

        if (0 == m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx] && byDriMaxNum == byEmptyIdx)
        {
            byEmptyIdx = byIdx;
        }
    }

    //不存在, 增加mtid记录并增加计数
    if (byEmptyIdx < byDriMaxNum)
    {
        m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byEmptyIdx] = byMtId;
        m_atMtAdpData[byDriId-1].m_wMtNum++;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[IncMtAdpMtNum] ConfIdx.%d MtNum.%d is full.\n", 
            byConfIdx, m_atMtAdpData[byDriId-1].m_wMtNum);
    }
    return;
}

/*====================================================================
    函数名      ：DecMtAdpMtNum
    功能        ：减少DRI终端数
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bHDMt			[in] 是HD终端 
				  BOOL32 bOnlyAlterNum	[in] 仅调整接入数目
    返回值说明  ：u8 byDriId DRI编号
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/04/02    3.0         胡昌威        创建
	1/4/2009				薛亮		  修改	 
====================================================================*/
void CMcuVcData::DecMtAdpMtNum( u8 byDriId, u8 byConfIdx, u8 byMtId, BOOL32 bHDMt, BOOL32 bOnlyAlterNum)
{
    if( 0 == byDriId || byDriId > MAXNUM_DRI || 0 == byMtId || 0 == byConfIdx || byConfIdx > MAX_CONFIDX ||
        0 == m_atMtAdpData[byDriId-1].m_wMtNum)	
	{
        return;		
	}    
    if(bOnlyAlterNum)
	{
		m_atMtAdpData[byDriId-1].m_wMtNum--;
		return;
	}
    //先查找是否已经存在
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
    {      
        if ( m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx] == byMtId )
        {
            m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx] = 0;
            m_atMtAdpData[byDriId-1].m_wMtNum--;
#ifdef _LINUX_
			// xliang [10/27/2008] 如果是HD的MT，且mcu内置接入板
			if(bHDMt)
			{
				TMtAdpHDChnnlInfo tHdChnnlInfo;
				tHdChnnlInfo.SetConfIdx(byConfIdx);
				tHdChnnlInfo.SetHDMtId(byMtId);

				for(u8 byIndex=0; byIndex<MAXHDLIMIT_MPC_MTADP; byIndex ++)
				{
					if(m_atMtAdpData[byDriId-1].m_atHdChnnlInfo[byIndex] == tHdChnnlInfo)
					{
						m_atMtAdpData[byDriId-1].m_atHdChnnlInfo[byIndex].SetNull();
						break;
					}
				}
			}
#endif
            return;
        }        
    }   
    return;	
}

/*====================================================================
    函数名      ：IncMpMtNum
    功能        ：增加MP终端数
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wConfBR: 会议码率
    返回值说明  ：BOOL32
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/04/02    3.0         胡昌威        创建
    07/02/09    4.0         张宝卿        增加MP的Pk限制策略  
====================================================================*/
BOOL32 CMcuVcData::IncMpMtNum( u8 byMpId, u8 byConfIdx, u8 byMtId, u16 wConfBR )
{
    // guzh [6/14/2007] 纯音频会议会议码率为0，也应该能分配
    if( 0 == byMpId || byMpId > MAXNUM_DRI || 0 == byMtId || 0 == byConfIdx || byConfIdx > MAX_CONFIDX )	
	{
        OspPrintf( TRUE, FALSE, "[IncMpMtNum] param err: byMpId.%d, byMtId.%d, byConfIdx.%d, wConfBR.%d !\n",
                                 byMpId, byMtId, byConfIdx, wConfBR );
        return FALSE;		
	}

    //先查找是否已经存在，并记录空位置
    u8 byEmptyIdx = MAXNUM_CONF_MT;
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
    {      
        if ( m_atMpData[byMpId-1].m_abyMtId[byConfIdx-1][byIdx] == byMtId )
        {
			//zbq[09/11/2008]对已经存在的重复添加，应该认为OK
            //return FALSE;
			return TRUE;
        }

        if (0 == m_atMpData[byMpId-1].m_abyMtId[byConfIdx-1][byIdx] && MAXNUM_CONF_MT == byEmptyIdx)
        {
            byEmptyIdx = byIdx;
        }
    }

    //不存在, 增加mtid记录并增加计数
    if (byEmptyIdx < MAXNUM_CONF_MT)
    {
        // 增加该MP上负载的Pk数, 1M以下负载按1M计算 [07/02/09-zbq]
        m_atMpData[byMpId-1].m_dwPkNumReal += wConfBR >= 1024 ? wConfBR : 1024;
        m_atMpData[byMpId-1].m_abyMtId[byConfIdx-1][byEmptyIdx] = byMtId;
        m_atMpData[byMpId-1].m_wMtNum++;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[IncMpMtNum] ConfIdx.%d MtNum.%d is full.\n", 
                                byConfIdx, m_atMpData[byMpId-1].m_wMtNum);
        return FALSE;
    }    
    return TRUE;
}


/*====================================================================
    函数名      ：DecMpMtNum
    功能        ：减少MP终端数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：u8 byMpId MP编号
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/04/02    3.0         胡昌威        创建
====================================================================*/
void CMcuVcData::DecMpMtNum( u8 byMpId, u8 byConfIdx, u8 byMtId, u16 wConfBR )
{
	if( 0 == byMpId || byMpId > MAXNUM_DRI || 0 == byMtId || 0 == byConfIdx || byConfIdx > MAX_CONFIDX ||
        0 == m_atMpData[byMpId-1].m_wMtNum)	
	{
        return;		
	}

    //先查找是否已经存在
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
    {      
        if ( m_atMpData[byMpId-1].m_abyMtId[byConfIdx-1][byIdx] == byMtId )
        {
            m_atMpData[byMpId-1].m_abyMtId[byConfIdx-1][byIdx] = 0;
            m_atMpData[byMpId-1].m_wMtNum--;
            m_atMpData[byMpId-1].m_dwPkNumReal -= wConfBR >= 1024 ? wConfBR : 1024;
            return;
        }        
    }   
    return;	
}

/*====================================================================
    函数名      ：AssignMpIdByBurden
    功能        ：根据负载分配MpId，首先查指定配置表
    算法实现    ：
    引用全局变量：
    输入参数说明：u8        byConfIdx:
                  TMtAlias &tMtAlias : 别名可能是IP或E164+IP
                  u8        byMtId   :
                  u16       wConfBR : 会议码率
    返回值说明  ：Mp的Id 
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
    06/12/29    4.0         guzh          增加别名传入，别名可能是IP或E164+IP
    07/02/12    4.0         张宝卿        用 繁忙指数 重新规划负载均衡
====================================================================*/
u8 CMcuVcData::AssignMpIdByBurden( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId, u16 wConfBR )
{
    // guzh [12/29/2006] 首先尝试从配置文件里面读取
    u32 dwMtadpIpAddr = 0;
    u32 dwMpIpAddr = 0;
    
    u8 byMpId = 0;

    BOOL32 bFoundInTable = FALSE;
    if ( tMtAlias.m_AliasType == mtAliasTypeE164 ) 
    {
        // 首先匹配E164
        if ( g_cMcuVcApp.GetMpIdAndH323MtDriIdFromMtAlias(tMtAlias, dwMtadpIpAddr, dwMpIpAddr) )
        {
            bFoundInTable = TRUE;

            byMpId = g_cMcuVcApp.FindMp( dwMpIpAddr );             
        }
        else
        {
            // 用IP尝试
            tMtAlias.m_AliasType = mtAliasTypeTransportAddress;
        }
    }
    if (!bFoundInTable)
    {
        // 再匹配IP        
        if ( g_cMcuVcApp.GetMpIdAndH323MtDriIdFromMtAlias(tMtAlias, dwMtadpIpAddr, dwMpIpAddr) )
        {
            bFoundInTable = TRUE;

            byMpId = g_cMcuVcApp.FindMp( dwMpIpAddr );             
        }
    }   
    
    // 用户没有指定, 寻找 当前最轻闲的MP
    if (!bFoundInTable)
    {
        // 繁忙指数可能超过100%(超负荷运转)，这里给一个安全上限
        u8  byLeastBusyPercent = 200;
        u8  byMaxPkLeftNO = 0;
        
        for(u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++ )
        {
            if(m_atMpData[byLoop].m_bConnected)
            {
                // 繁忙指数最小 且 在Pk承受范围之内者 居之
                if ( m_atMpData[byLoop].GetCurBusyPercent() < byLeastBusyPercent &&
                     m_atMpData[byLoop].IsThePkSupport( wConfBR ) ) 
                {
                    byMaxPkLeftNO = byLoop;
                    byLeastBusyPercent = m_atMpData[byLoop].GetCurBusyPercent();
                }
            }
        }
        byMpId = byMaxPkLeftNO+1;
    }	

    if ( byMpId != 0 && m_atMpData[byMpId-1].m_bConnected )
    {
        if ( !IncMpMtNum(byMpId, byConfIdx, byMtId, wConfBR) ) 
        {
            byMpId = 0;
        }
    }
    else
    {
        byMpId = 0;
    }

	return byMpId;
}

/*====================================================================
    函数名      ：AssignH323MtDriId
    功能        ：根据终端地址分配H323适配用的DRI板 
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId
    返回值说明  ：返回的适配用的DRI板ID 
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
	07/01/08    4.0         周广程		  修改
====================================================================*/
u8 CMcuVcData::AssignH323MtDriId(u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId)
{
	u8 byDriId = 0;	
    
    u32 dwMtadpIpAddr = 0;
    u32 dwMpIpAddr = 0;

    if( 0 == byMtId || 0 == byConfIdx || byConfIdx > MAX_CONFIDX )
    {
        byDriId = 0;
        OspPrintf( TRUE, FALSE, "[AssignH323MtDriId] assign mtadp id, param byConfIdx.%d error! \n", byConfIdx );
        return byDriId;
    }

    // find the conf
    TConfInfo *ptConf = NULL;
    if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return FALSE;
    }
    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if(tMapData.IsValidConf())
    {            
        ptConf = &m_apConfInst[tMapData.GetInsId()-1]->m_tConf;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[AssignH323MtDriId] can't get confinfo.%d\n", byConfIdx);
        return FALSE;
    }
    BOOL bConfHD = CMcuPfmLmt::IsConfFormatHD(*ptConf);

    u8 byMtNum = 0;
    u8 bySMcuNum = 0;

	// xliang [12/17/2008] 计算有HD接入能力的接入板数目。
	// 即HDI板的数目。内嵌接入板，CRI板均属非SupportHD类
	// xliang [2/27/2009] 另外如果配了HDI板，但授权数是0，则当该HDI板不存在

	s32 nHdMtAdpNum = 0;
	for( u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if( g_cMcuVcApp.m_atMtAdpData[byLoop].m_bConnected 
			&& g_cMcuVcApp.m_atMtAdpData[byLoop].m_byIsSupportHD
			&& g_cMcuVcApp.m_atMtAdpData[byLoop].m_byMaxMtNum >0)
		{
			nHdMtAdpNum++;
		}		
	}
	CallLog("The num of access boards supported HD is %d\n", nHdMtAdpNum);
	
	BOOL32 bNeglectedForLowPri = FALSE; // 暂时忽略内嵌接入板的分配，优先分配到HDI上

    if (GetMpIdAndH323MtDriIdFromMtAlias(tMtAlias, dwMtadpIpAddr, dwMpIpAddr))
    {
        byDriId = FindMtAdp( dwMtadpIpAddr, PROTOCOL_TYPE_H323 );

        // 分配时,检测所有终端,不区分是否在线.
        if ( //GetPerfLimit().IsLimitMtNum() &&
             //zbq[01/09/2008]优先判断终端分布
             GetMtNumOnDri(byDriId, FALSE, byMtNum, bySMcuNum) &&
             GetPerfLimit().IsLimitMtNum() )
        {
            byDriId = 0;
        }

        //guzh [2008/03/27] 高清接入限制。8000B通过BrdID限制，非8000B-HD不允许开高清会       
#ifndef _MINIMCU_
        if ( bConfHD && !m_atMtAdpData[byDriId-1].m_byIsSupportHD )
        {
            // xliang [12/16/2008] 考虑内嵌接入HD终端能力，进行过滤
			BOOL32 bEmbededAccessHdMtFull = TRUE; //内嵌接入HD终端能力是否已满
			u8 byIdx = 0;
			for(byIdx = 0; byIdx < MAXHDLIMIT_MPC_MTADP; byIdx ++)
			{
				if(g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_atHdChnnlInfo[byIdx].GetHDMtId() == 0)
				{
					bEmbededAccessHdMtFull = FALSE;
					break;
				}
			}

			//zbq[10/28/2008] 高清终端接入能力放开
            if ((byDriId == MCU_BOARD_MPC ||
				byDriId == MCU_BOARD_MPCD) &&
				!bEmbededAccessHdMtFull)// xliang [12/16/2008] 内嵌接入HD终端能力未满才放过
            {
                CallLog("[AssignH323MtDriId] User specified dri.%d is mpc!\n", byDriId);
            }
			else if(bEmbededAccessHdMtFull && nHdMtAdpNum == 0)
			{
				CallLog("[AssignH323MtDriId] No HDI，Auto specified dri.%d is mpc!\n",byDriId);
			}
            else
            {
                CallLog("[AssignH323MtDriId] User specified dri.%d is not HDI.", byDriId);
                byDriId = 0;
            }
        }            
#endif

    }
    else
    {
        u8 byMinDriNO = 0;
        u8 byMinMtNum = MAXNUM_DRI_MT;
		u8 byMaxMtNumLeft = 0;	// xliang [2/16/2009] 接入能力最大剩余数
        for(u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++ )
        {
            if ( //GetPerfLimit().IsLimitMtNum() &&
                 //zbq[01/09/2008]优先判断终端分布
                 GetMtNumOnDri(byLoop+1, FALSE, byMtNum, bySMcuNum) &&
                 GetPerfLimit().IsLimitMtNum() )
            {
                // 跳过超容量的
                continue;
            }

            if( m_atMtAdpData[byLoop].m_bConnected && 
                PROTOCOL_TYPE_H323 == m_atMtAdpData[byLoop].m_byProtocolType )
            {
                //guzh [2008/03/27] 高清接入限制。8000B通过BrdID限制，非8000B-HD不允许开高清会
#ifndef _MINIMCU_
                if ( bConfHD && !m_atMtAdpData[byLoop].m_byIsSupportHD )
                {
					// xliang [2/27/2009] 只要是高清会议就先找HDI板（如果有HDI板的话），
					//不管内嵌HD接入能力是否已满
                    if ((byLoop+1 == MCU_BOARD_MPC 
                        || byLoop+1 == MCU_BOARD_MPCD) 
						&& nHdMtAdpNum > 0 
						) 
					{
						//标记此处"暂时跳过"
						bNeglectedForLowPri = TRUE;
						continue;
					} 
                }
#endif
                /*if( byMtNum < byMinMtNum )
                {
                    byMinDriNO = byLoop;
                    byMinMtNum = byMtNum;
                }*/
				// xliang [2/16/2009] 选剩余接入能力最多的一个
				u8 byMtNumLeft = m_atMtAdpData[byLoop].m_byMaxMtNum - byMtNum;
				if( byMtNumLeft > byMaxMtNumLeft)
				{
					byMaxMtNumLeft = byMtNumLeft;
					byMinDriNO = byLoop;
				}
            }
        }
        
        //if( byMinMtNum == MAXNUM_DRI_MT )
        if( byMaxMtNumLeft == 0 ) // xliang [2/16/2009] 
        {
            if(bNeglectedForLowPri) // xliang [1/15/2009] 之前忽略内嵌接入板，现在再回过头尝试分配
			{
				CallLog("[AssignH323MtDriId]Try to assign mt to embeded dri which was neglected before!\n");
				
				if ( !GetMtNumOnDri(MCU_BOARD_MPC, FALSE, byMtNum, bySMcuNum) 
					||!GetPerfLimit().IsLimitMtNum() )
				{
					byDriId = MCU_BOARD_MPC;
				}
				else
				{
					OspPrintf( TRUE, FALSE, "[AssignH323MtDriId] fail to assign mtadp id, All DRI full! \n" );
					byDriId = 0;
				}
			}
			else
			{
				OspPrintf( TRUE, FALSE, "[AssignH323MtDriId] fail to assign mtadp id, All DRI full! \n" );
				byDriId = 0;
			}
        }   
        else
        {
            byDriId = byMinDriNO + 1;            
        }
    }
    

    if ( byDriId > 0 )
    {
        IncMtAdpMtNum(byDriId, byConfIdx, byMtId);
    }
   
	return byDriId;
}

/*====================================================================
    函数名      ：AssignMpIdAndH320MtDriId
    功能        ：根据终端Ip地址分配交换数据的mp板以及H320适配用的DRI板 
    算法实现    ：
    引用全局变量：
    输入参数说明：byMpId 返回的mp板ID
    返回值说明  ：返回的适配用的DRI板ID 
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
u8 CMcuVcData::AssignH320MtDriId( u8 byConfIdx, TMtAlias &tMtAlias, u8 byMtId )
{
	u8 byDriId = 0;    
	u8 byLayer   = 0; 
	u8 bySlot    = 0;
	u8 byChannel = 0;
	u32 dwDriNo  = 0;
	tMtAlias.GetH320Alias( byLayer, bySlot, byChannel );
	dwDriNo  = byLayer << 4;
	dwDriNo += bySlot;
	if( dwDriNo <= MAXNUM_DRI_MT )
	{
		byDriId = (u8)(dwDriNo+1);
		if( m_atMtAdpData[byDriId-1].m_bConnected && 
			PROTOCOL_TYPE_H320 == m_atMtAdpData[byDriId-1].m_byProtocolType)
		{
			if( (byChannel+1) > m_atMtAdpData[byDriId-1].m_byMaxMtNum )
			{
				byDriId = 0;
				OspPrintf( TRUE, FALSE, "[AssignMpIdAndH320MtDriId] invalid Channel! \n" );	
			}
			else
			{
				IncMtAdpMtNum(byDriId, byConfIdx, byMtId);
			}
		}
	}
	else
	{
		OspPrintf( TRUE, FALSE, "[AssignH320MtDriId] invalid dri! \n" );
	}	
    
	return byDriId;
}

/*=============================================================================
函 数 名： IsMtAssignInDri
功    能： 终端是否已经分配在此mtadp上
算法实现： 
全局变量： 
参    数：  u8 byDriId
           u8 byConfIdx
           u8 byMtId
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/13  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsMtAssignInDri( u8 byDriId, u8 byConfIdx, u8 byMtId )
{
    if (0 == byDriId || byDriId > MAXNUM_DRI || byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return FALSE;
    }

    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
    {
        if (m_atMtAdpData[byDriId-1].m_abyMtId[byConfIdx-1][byIdx] == byMtId)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
函 数 名： IsMtAssignInMp
功    能： 终端是否已经分配在此mp上
算法实现： 
全局变量： 
参    数：  u8 byMpId
           u8 byConfIdx
           u8 byMtId
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/13  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsMtAssignInMp( u8 byMpId, u8 byConfIdx, u8 byMtId )
{
    if (0 == byMpId || byMpId > MAXNUM_DRI || byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return FALSE;
    }

    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
    {
        if (m_atMpData[byMpId-1].m_abyMtId[byConfIdx-1][byIdx] == byMtId)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*====================================================================
    函数名      ：GetRegConfDriId
    功能        ：得到注册会议的MtAdp号
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：MtAdpId ID
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
u8 CMcuVcData::GetRegConfDriId( void )
{
	u8  byDriId  = 0;
	int nMtAdpId = 0;
	u8  byRegNum = MAXNUM_MCU_CONF;
	while( nMtAdpId < MAXNUM_DRI )
	{
		//跳过未连接的
		if( !m_atMtAdpData[nMtAdpId].m_bConnected || 
			PROTOCOL_TYPE_H323 == m_atMtAdpData[nMtAdpId].m_byProtocolType )
		{
			nMtAdpId++;
		}
		else
		{
			if( m_atMtAdpData[nMtAdpId].m_byRegGKConfNum < byRegNum )
			{
				byRegNum  = m_atMtAdpData[nMtAdpId].m_byRegGKConfNum;
				byDriId = nMtAdpId+1;
			}
			nMtAdpId++;
		}
	}

	return byDriId<=MAXNUM_DRI ? byDriId : 0;	
}

/*====================================================================
    函数名      ChangeDriRegConfNum
    功能        ：改变Dri上注册GK的会议数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：Mp的数量
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
void CMcuVcData::ChangeDriRegConfNum( u8 byDriId, BOOL32 bInc  )
{
    if( byDriId >= MAXNUM_DRI )
	{
		return;
	}

	if( bInc )
	{
		m_atMtAdpData[byDriId].m_byRegGKConfNum++;
	}
	else
	{
		m_atMtAdpData[byDriId].m_byRegGKConfNum--;
	}
}

/*=============================================================================
    函 数 名： RegisterConfToGK
    功    能： 把MCU或者会议实体注册到GK上
    算法实现： 
    全局变量： 
    参    数： u8 byConfIdx
               u8 byDriId
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/6    3.6			万春雷                  创建
=============================================================================*/
void CMcuVcData::RegisterConfToGK(u8 byConfIdx, u8 byDriId, BOOL32 bTemplate, BOOL32 bUnReg)
{
    TMtAlias tMcuH323Id;
    TMtAlias tMcuE164;
    CServMsg cServMsg;

    if (0 == g_cMcuAgent.GetGkIpAddr() || 
        0 == byDriId || 
        byDriId > MAXNUM_DRI)
    {
        return;
    }
    if (0 == byConfIdx)
    {
        s8 achMcuE164[MAXLEN_ALIAS];
        s8 achMcuH323Id[MAXLEN_ALIAS];
        memset(achMcuE164, 0, MAXLEN_ALIAS);
        memset(achMcuH323Id, 0, MAXLEN_ALIAS);
        g_cMcuAgent.GetE164Number(achMcuE164, MAXLEN_ALIAS);
        g_cMcuAgent.GetMcuAlias(achMcuH323Id, MAXLEN_ALIAS);
        tMcuE164.SetE164Alias(achMcuE164);
        tMcuH323Id.SetH323Alias(achMcuH323Id);
        cServMsg.SetMcuId(LOCAL_MCUID);
        cServMsg.SetConfIdx(0);
        cServMsg.SetDstDriId(byDriId);
        cServMsg.SetMsgBody((u8*)&tMcuE164, sizeof(TMtAlias));
        cServMsg.CatMsgBody((u8*)&tMcuH323Id, sizeof(TMtAlias));
    }
    else
    {		
        TConfInfo *ptConf = NULL;
        if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
        {
            return;
        }
        TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
        if(bTemplate && tMapData.IsTemUsed())
        {           
            ptConf = &m_ptTemplateInfo[tMapData.GetTemIndex()].m_tConfInfo;
        }
        else if(tMapData.IsValidConf())
        {            
            ptConf = &m_apConfInst[tMapData.GetInsId()-1]->m_tConf;
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[RegisterConfToGK] can't get confinfo for register\n");
            return;
        }
        
        tMcuE164.SetE164Alias(ptConf->GetConfE164());
        cServMsg.SetConfId(ptConf->GetConfId());
        cServMsg.SetConfIdx(byConfIdx);
        cServMsg.SetDstDriId(byDriId);
        cServMsg.SetMsgBody((u8*)&tMcuE164, sizeof(TMtAlias));
        cServMsg.CatMsgBody((u8*)&tMcuH323Id, sizeof(TMtAlias));
    }

    //协议接入板地址
    BOOL32 bHasMtadp = FALSE;
    TTransportAddr tMtadpAddr;
    for (u8 byIdx = 0; byIdx < MAXNUM_DRI; byIdx++)
    {
        if (m_atMtAdpData[byIdx].m_bConnected && m_atMtAdpData[byIdx].m_dwIpAddr != 0)
        {
            tMtadpAddr.SetNetSeqIpAddr(m_atMtAdpData[byIdx].m_dwIpAddr);            
            tMtadpAddr.SetPort(m_atMtAdpData[byIdx].m_wQ931Port);
            cServMsg.CatMsgBody((u8*)&tMtadpAddr, sizeof(tMtadpAddr));
            bHasMtadp = TRUE;
        }
    }
    if (!bHasMtadp)
    {
        OspPrintf(TRUE, FALSE, "[RegisterConfToGK] no online mtadp for register\n");
        return;
    }
    
    if(bUnReg)
    {
        g_cMtAdpSsnApp.SendMsgToMtAdpSsn(byDriId, MCU_MT_UNREGISTERGK_REQ, cServMsg);
    }
    else
    {
        g_cMtAdpSsnApp.SendMsgToMtAdpSsn(byDriId, MCU_MT_REGISTERGK_REQ, cServMsg);
    }    

    return;
}


/*=============================================================================
    函 数 名： ConfChargeByGK
    功    能： 会议计费请求
    算法实现： 
    全局变量： 
    参    数： u8 byConfIdx
               u8 byDriId
               BOOL32 bStopCharge = FALSE
               u8 byCreateBy = CONF_CREATE_MCS
               u8 byMtNum
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/11/09  4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcData::ConfChargeByGK( u8 byConfIdx, u8 byDriId, BOOL32 bStopCharge, u8 byCreateBy, u8 byMtNum )
{
    TConfInfo *ptConf = NULL;
    if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return;
    }
    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if(tMapData.IsValidConf())
    {            
        ptConf = &m_apConfInst[tMapData.GetInsId()-1]->m_tConf;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[ConfChargeByGK] can't get confinfo for charge\n");
        return;
    }

    CServMsg cServMsg;
    cServMsg.SetConfId(ptConf->GetConfId());
    cServMsg.SetConfIdx(byConfIdx);
    cServMsg.SetDstDriId(byDriId);

    if ( !bStopCharge )
    {
        TNetWorkInfo tNetInfo;
        g_cMcuAgent.GetNetWorkInfo( &tNetInfo );

        TConfChargeInfo tChargeInfo;
        tChargeInfo.SetGKUsrName( ptConf->GetConfAttrbEx().GetGKUsrName() );
        tChargeInfo.SetGKPwd( ptConf->GetConfAttrbEx().GetGKPwd() );

        if ( CONF_CREATE_NPLUS == byCreateBy )
        {
            tChargeInfo.SetConfName( ptConf->GetConfName() );
            tChargeInfo.SetConfE164( ptConf->GetConfE164() );
            // GK计费依照会议全码率（VID+AUD）, zgc, 2008-09-23
            //tChargeInfo.SetBitRate( ptConf->GetBitRate() );
            tChargeInfo.SetBitRate( ptConf->GetBitRate() + GetAudioBitrate(ptConf->GetMainAudioMediaType()) );
            
            BOOL32 bDiscussConf = FALSE;
            bDiscussConf = ptConf->GetConfAttrb().IsDiscussConf();
            tChargeInfo.SetMixerNum( bDiscussConf ? 1 : 0 );
            
            tChargeInfo.SetTerNum( byMtNum );
            
            BOOL32 bVMPAuto = ptConf->m_tStatus.GetVmpParam().IsVMPAuto();
            tChargeInfo.SetVMPNum( bVMPAuto ? 1 : min(16, byMtNum) );
            tChargeInfo.SetBasNum( 0 != ptConf->GetSecBitRate() ? 1 : 0 );  //码率适配数
            
            BOOL32 bDataConf = FALSE;
            u8 byMode = ptConf->GetConfAttrb().GetDataMode();
            if ( CONF_DATAMODE_VAANDDATA == byMode ||
                CONF_DATAMODE_DATAONLY  == byMode )
            {
                bDataConf = TRUE;
            }
            tChargeInfo.SetIsUseDataConf( bDataConf );
            
            BOOL32 bDualVideo = FALSE;
            byMode = ptConf->GetConfAttrb().GetDualMode();
            if ( CONF_DUALMODE_SPEAKERONLY  == byMode ||
                CONF_DUALMODE_EACHMTATWILL == byMode )
            {
                bDualVideo = TRUE;
            }
            tChargeInfo.SetIsUseDualVideo( bDualVideo );
            
            BOOL32 bEncrypt = FALSE;
            byMode = ptConf->GetConfAttrb().GetEncryptMode();
            if ( CONF_ENCRYPTMODE_NONE != byMode )
            {
                bEncrypt = TRUE;
            }
            tChargeInfo.SetIsUseEncrypt( bEncrypt );
            tChargeInfo.SetIsUseCascade( ptConf->GetConfAttrb().IsSupportCascade() );
            tChargeInfo.SetIsUseStreamBroadCast( ptConf->GetConfAttrb().IsMulticastMode() );
        }
        else
        {            
            TTemplateInfo tTemInfo;
            GetTemplate( byConfIdx, tTemInfo );
       
            tChargeInfo.SetConfName( tTemInfo.m_tConfInfo.GetConfName() );
            tChargeInfo.SetConfE164( tTemInfo.m_tConfInfo.GetConfE164() );
            // GK计费依照会议全码率（VID+AUD）, zgc, 2008-09-23
            //tChargeInfo.SetBitRate( tTemInfo.m_tConfInfo.GetBitRate() );
            tChargeInfo.SetBitRate( tTemInfo.m_tConfInfo.GetBitRate() + GetAudioBitrate(tTemInfo.m_tConfInfo.GetMainAudioMediaType()) );
            
            BOOL32 bDiscussConf = FALSE;
            bDiscussConf = tTemInfo.m_tConfInfo.GetConfAttrb().IsDiscussConf();
            tChargeInfo.SetMixerNum( bDiscussConf ? 1 : 0 );
            
            tChargeInfo.SetTerNum( tTemInfo.m_byMtNum );
            
            BOOL32 bVMPAuto = tTemInfo.m_atVmpModule.GetVmpParam().IsVMPAuto();
            tChargeInfo.SetVMPNum( bVMPAuto ? 1 : min(16, tTemInfo.m_byMtNum) );
            tChargeInfo.SetBasNum( 0 != tTemInfo.m_tConfInfo.GetSecBitRate() ? 1 : 0 );  //码率适配数
            
            BOOL32 bDataConf = FALSE;
            u8 byMode = tTemInfo.m_tConfInfo.GetConfAttrb().GetDataMode();
            if ( CONF_DATAMODE_VAANDDATA == byMode ||
                CONF_DATAMODE_DATAONLY  == byMode )
            {
                bDataConf = TRUE;
            }
            tChargeInfo.SetIsUseDataConf( bDataConf );
            
            BOOL32 bDualVideo = FALSE;
            byMode = tTemInfo.m_tConfInfo.GetConfAttrb().GetDualMode();
            if ( CONF_DUALMODE_SPEAKERONLY  == byMode ||
                CONF_DUALMODE_EACHMTATWILL == byMode )
            {
                bDualVideo = TRUE;
            }
            tChargeInfo.SetIsUseDualVideo( bDualVideo );
            
            BOOL32 bEncrypt = FALSE;
            byMode = tTemInfo.m_tConfInfo.GetConfAttrb().GetEncryptMode();
            if ( CONF_ENCRYPTMODE_NONE != byMode )
            {
                bEncrypt = TRUE;
            }
            tChargeInfo.SetIsUseEncrypt( bEncrypt );
            tChargeInfo.SetIsUseCascade( tTemInfo.m_tConfInfo.GetConfAttrb().IsSupportCascade() );
            tChargeInfo.SetIsUseStreamBroadCast( tTemInfo.m_tConfInfo.GetConfAttrb().IsMulticastMode() );
        }
        cServMsg.SetMsgBody( (u8*)&tChargeInfo, sizeof(tChargeInfo) );
        g_cMtAdpSsnApp.SendMsgToMtAdpSsn(byDriId, MCU_MT_CONF_STARTCHARGE_REQ, cServMsg);
    }
    else
    {
        g_cMtAdpSsnApp.SendMsgToMtAdpSsn(byDriId, MCU_MT_CONF_STOPCHARGE_REQ, cServMsg);
    }

    return;
}

/*====================================================================
    函数名      ：AddMtAdp
    功能        ：增加一个MtAdp
    算法实现    ：
    引用全局变量：
    输入参数说明：tMtAdpReg MtAdp信息
    返回值说明  ：成功TRUE,失败FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
    08/03/27    4.5         顾振华        高清HDI支持
====================================================================*/
void CMcuVcData::AddMtAdp( TMtAdpReg &tMtAdpReg )
{
    m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_bConnected     = TRUE;
	m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_byMtAdpId      = tMtAdpReg.GetDriId(); 
	m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_dwIpAddr       = htonl( tMtAdpReg.GetIpAddr() );
	m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_byProtocolType = tMtAdpReg.GetProtocolType(); 	
    m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_wQ931Port      = tMtAdpReg.GetQ931Port();
    m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_wRasPort       = tMtAdpReg.GetRasPort();
    m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_byIsSupportHD  = tMtAdpReg.IsSupportHD() ? 1: 0;

    // guzh [1/17/2007] 综合考量MtAdp能力与业务限制
    u8 byMaxMtNum = tMtAdpReg.GetMaxMtNum();
    u8 byMaxSMcuNum = MAXLIMIT_CRI_MTADP_SMCU;
    TMcuPerfLimit tLimit = GetPerfLimit ();
    if ( tLimit.IsLimitMtNum() )
    {
        if ( tMtAdpReg.GetAttachMode() == 1)
        {
            byMaxMtNum = min ( byMaxMtNum, tLimit.m_byMpcMaxMtAdpConnMtAbility );
            byMaxSMcuNum = tLimit.m_byMpcMaxMtAdpConnSMcuAbility;
        }
        else
        {
#ifndef _MINIMCU_
//             if (tMtAdpReg.IsSupportHD())
//             {
//                 byMaxMtNum = min ( byMaxMtNum, tLimit.m_byHdiMaxMtAdpConnMtAbility );
//             }
//             else
//             {
//                 byMaxMtNum = min ( byMaxMtNum, tLimit.m_byTuiMaxMtAdpConnMtAbility );
//             }
			byMaxMtNum = min ( byMaxMtNum, MAXNUM_DRI_MT);
#else
            byMaxMtNum = min ( byMaxMtNum, tLimit.m_byTuiMaxMtAdpConnMtAbility );
#endif            
            byMaxSMcuNum = tLimit.m_byTuiMaxMtAdpConnSMcuAbility;
        }
    }
    m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_byMaxMtNum     = byMaxMtNum;
    m_atMtAdpData[tMtAdpReg.GetDriId() - 1].m_byMaxSMcuNum   = byMaxSMcuNum;
}

/*====================================================================
    函数名      ：RemoveMtAdp
    功能        ：移除MtAdp
    算法实现    ：
    引用全局变量：
    输入参数说明：byMtAdpId MtAdp编号	          
    返回值说明  ：成功TRUE,失败FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
void CMcuVcData::RemoveMtAdp( u8 byMtAdpId )
{
    if (!IsMtAdpConnected(byMtAdpId))
    {
        return;
    }
    
    m_atMtAdpData[byMtAdpId - 1].m_bConnected     = FALSE;
	m_atMtAdpData[byMtAdpId - 1].m_byMtAdpId      = 0;
	m_atMtAdpData[byMtAdpId - 1].m_dwIpAddr       = 0;
	m_atMtAdpData[byMtAdpId - 1].m_byProtocolType = 0;
	m_atMtAdpData[byMtAdpId - 1].m_byMaxMtNum     = 0;
    m_atMtAdpData[byMtAdpId - 1].m_byMaxSMcuNum   = 0;
    m_atMtAdpData[byMtAdpId - 1].m_wQ931Port      = 0;
    m_atMtAdpData[byMtAdpId - 1].m_wRasPort       = 0;
    m_atMtAdpData[byMtAdpId - 1].m_byRegGKConfNum = 0;
    m_atMtAdpData[byMtAdpId - 1].m_wMtNum         = 0;
    memset(m_atMtAdpData[byMtAdpId - 1].m_abyMtId, 0, sizeof(m_atMtAdpData[byMtAdpId - 1].m_abyMtId));
}

/*====================================================================
    函数名      ：IsMtAdpConnected
    功能        ：判断MtAdp是否已连接
    算法实现    ：
    引用全局变量：
    输入参数说明：byMtAdpId MtAdp编号
    返回值说明  ：TRUE - 连接, FALSE - 未连接
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
BOOL32 CMcuVcData::IsMtAdpConnected( u8 byMtAdpId )
{
    if (0 == byMtAdpId || byMtAdpId > MAXNUM_DRI)
    {
        return FALSE;
    }

    return m_atMtAdpData[byMtAdpId - 1].m_bConnected;
}

/*=============================================================================
    函 数 名： GetMtAdpProtocalType
    功    能： 获取适配板支持的协议类型
    算法实现： 
    全局变量： 
    参    数： u8 byMtAdpId
    返 回 值： u8 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/1    3.5			万春雷                  创建
=============================================================================*/
u8 CMcuVcData::GetMtAdpProtocalType( u8 byMtAdpId )
{
    if (!IsMtAdpConnected(byMtAdpId))
    {
        return 0;
    }

    return m_atMtAdpData[byMtAdpId - 1].m_byProtocolType;
}

/*=============================================================================
    函 数 名： GetMtAdpSupportMtNum
    功    能： 获取适配板支持的最大呼叫通道数
    算法实现： 
    全局变量： 
    参    数： u8 byMtAdpId
    返 回 值： u8 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/1    3.5			万春雷                  创建
=============================================================================*/
u8 CMcuVcData::GetMtAdpSupportMtNum( u8 byMtAdpId )
{
    if (!IsMtAdpConnected(byMtAdpId))
    {
        return 0;
    }

    return m_atMtAdpData[byMtAdpId - 1].m_byMaxMtNum;
}

/*=============================================================================
    函 数 名： GetMtAdpSupportSMcuNum
    功    能： 获取适配板支持的最大下级MCU数
    算法实现： 
    全局变量： 
    参    数： u8 byMtAdpId
    返 回 值： u8 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2007/1/17   4.0			顾振华                  创建
=============================================================================*/
u8 CMcuVcData::GetMtAdpSupportSMcuNum( u8 byMtAdpId )
{
    if (!IsMtAdpConnected(byMtAdpId))
    {
        return 0;
    }

    return m_atMtAdpData[byMtAdpId - 1].m_byMaxSMcuNum;
}

/*====================================================================
    函数名      ：GetMtAdpIpAddr
    功能        ：得到MtAdp的Ip地址 
    算法实现    ：
    引用全局变量：
    输入参数说明：byMtAdpId MtAdp编号
    返回值说明  ：成功返回IP地址, 失败0
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
u32 CMcuVcData::GetMtAdpIpAddr( u8 byMtAdpId )
{
    if (!IsMtAdpConnected(byMtAdpId))
    {
        return 0;
    }

	return ntohl( m_atMtAdpData[byMtAdpId - 1].m_dwIpAddr );
}

/*====================================================================
    函数名      ：GetMtAdpNum
    功能        ：得到MtAdp的数目
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byProtocolType = PROTOCOL_TYPE_H323 协议类型
    返回值说明  ：MtAdp的数量
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
u8 CMcuVcData::GetMtAdpNum( u8 byProtocolType )
{
	u8 byMpNum = 0;
    for( u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++ )
	{
        if( m_atMtAdpData[byLoop].m_bConnected && 
			m_atMtAdpData[byLoop].m_byProtocolType == byProtocolType )
		{
		    byMpNum++;
		}
	}
	return byMpNum;
}

/*====================================================================
    函数名      ：FindMtAdp
    功能        ：查找MtAdp
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 dwIp ip地址，主机序
	              u8 byProtocolType = PROTOCOL_TYPE_H323 协议类型
    返回值说明  ：查找MtAdp的Id
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
u8 CMcuVcData::FindMtAdp( u32 dwIp, u8 byProtocolType )
{
   u8 byMtAdpId = 0;
   int nMtAdpId = 0;
   while( nMtAdpId < MAXNUM_DRI && byMtAdpId == 0)
   {
	   //跳过未连接的
	   if(!m_atMtAdpData[nMtAdpId].m_bConnected)
	   {
		   nMtAdpId++;
		   continue;
	   }

	  //查找是否是此MtAdp
      if( m_atMtAdpData[nMtAdpId].m_dwIpAddr == htonl(dwIp) && 
		  m_atMtAdpData[nMtAdpId].m_byProtocolType == byProtocolType )
	  {
         byMtAdpId = nMtAdpId+1;
		 break;
	  }

	   nMtAdpId++;
   }

   return byMtAdpId;
}

/*====================================================================
    函数名      ：InitPeriEqpList
    功能        ：初始化外设列表
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/21    3.0         zmy           创建
====================================================================*/
void CMcuVcData::InitPeriEqpList()
{
    u8  byEqpId;
    u32 dwIP;
    u8  byType;

    memset(m_atPeriEqpTable, 0, sizeof(m_atPeriEqpTable));

    for (byEqpId = 1; byEqpId < MAXNUM_MCU_PERIEQP; byEqpId++)
    {
        m_atPeriEqpTable[byEqpId-1].m_bIsValid = (SUCCESS_AGENT == g_cMcuAgent.GetPeriInfo(byEqpId, &dwIP, &byType));
        m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.SetMcuId(LOCAL_MCUID);
        m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.SetEqpId(byEqpId);
        m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.SetEqpType(byType);
        m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_byOnline = FALSE;
    }
}

/*====================================================================
    函数名      ：IsPeriEqpValid
    功能        ：外设是否在配置中
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/21    3.0         zmy           创建
====================================================================*/
BOOL32 CMcuVcData::IsPeriEqpValid( u8   byEqpId )
{
	return m_atPeriEqpTable[byEqpId-1].m_bIsValid;
}

/*=============================================================================
函 数 名： SetPeriEqpIsValid
功    能： 设置指定外设有效
算法实现： 
全局变量： 
参    数： u8 byEqpId
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/10/29   4.0		周广程                  创建
=============================================================================*/
void CMcuVcData::SetPeriEqpIsValid( u8 byEqpId )
{
	m_atPeriEqpTable[byEqpId-1].m_bIsValid = TRUE;
}

/*====================================================================
    函数名      ：GetEqp
    功能        ：得到外设结构
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/12/04    3.0         胡昌威           创建
====================================================================*/
TEqp CMcuVcData::GetEqp( u8 byEqpId )
{
	TEqp tEqp;
	tEqp.SetNull();
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || !IsPeriEqpValid( byEqpId ) )
	{
		return tEqp;
	}

	return (TEqp)m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus;
}

/*====================================================================
    函数名      ：GetEqpType
    功能        ：得到外设类型
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/12/04    3.0         胡昌威           创建
====================================================================*/
u8   CMcuVcData::GetEqpType( u8 byEqpId )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || !IsPeriEqpValid( byEqpId ) )
	{
		return 0;
	}

	return m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.GetEqpType();
}
/*====================================================================
    函数名      :GetEqpData
    功能        :得到外设数据
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/12/04    3.0         zhangsh           创建
====================================================================*/
TPeriEqpData *CMcuVcData::GetEqpData( u8 byEqpId )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP || !IsPeriEqpValid( byEqpId ) )
	{
		return NULL;
	}
	return &m_atPeriEqpTable[byEqpId-1];
}

/*====================================================================
    函数名      ：GetIdleVMP
    功能        ：得到空闲的画面合成器
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * pbyVMPBuf IN OUT : IDLE VMP id buf
                  u8 & byNum    OUT : IDLE VMP num
                  u8   byVMPLen IN  : buf len
    返回值说明  ：BOOL32
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/12/03    3.0         胡昌威        创建
    06/12/27    4.0         张宝卿        获取所有的空闲VMP  
====================================================================*/
BOOL32 CMcuVcData::GetIdleVMP( u8 * pbyVMPBuf, u8 &byNum, u8 byVMPLen )
{
    if ( NULL == pbyVMPBuf || byVMPLen < MAXNUM_PERIEQP )
    {
        OspPrintf( TRUE, FALSE, "[GetIdleVMP] param err: pbyBuf.0x%x, byVMPLen.%d !\n", pbyVMPBuf, byVMPLen );
        return FALSE;
    }
    byNum = 0;
	u8    byEqpId = VMPID_MIN;
    while( byEqpId <= VMPID_MAX )
	{
		if( IsPeriEqpValid( byEqpId ) &&
			m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_byOnline && 
			m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::IDLE)
		{
            pbyVMPBuf[byNum] = byEqpId;
            byNum ++;
		}
        byEqpId++;
	}

	if (g_bPrintEqpMsg)
	{
		OspPrintf(TRUE, FALSE, "[GetIdleVMP]idle vmp:\n");
		for (u8 byIdx = 0; byIdx < byNum; byIdx++)
		{
			OspPrintf(TRUE, FALSE, "%d.eqpid%d\n", pbyVMPBuf[byIdx]);
		}
		OspPrintf(TRUE, FALSE, "[GetIdleVMP]no other idle vmp\n");

	}

	return TRUE;
}

/*====================================================================
    函数名      ：GetIdleVmpTw
    功能        ：得到空闲的复合电视墙
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * pbyMPWBuf IN OUT : IDLE VMP id buf
                  u8 & byNum    OUT : IDLE VMP num
                  u8   byMPWLen IN  : buf len
    返回值说明  ：BOOL32
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/10/08    4.0         libo          创建
    06/12/27    4.0         张宝卿        获取所有的空闲MPW
====================================================================*/
BOOL32 CMcuVcData::GetIdleVmpTw( u8 * pbyMPWBuf, u8 &byNum, u8 byMPWLen )
{
    if ( NULL == pbyMPWBuf || byMPWLen < MAXNUM_PERIEQP )
    {
        OspPrintf( TRUE, FALSE, "[GetIdleVMP] param err: pbyBuf.0x%x, byVMPLen.%d !\n", pbyMPWBuf, byMPWLen );
        return FALSE;
    }
    byNum = 0;
    u8    byEqpId = VMPTWID_MIN;
    while( byEqpId <= VMPTWID_MAX )
    {
        if( IsPeriEqpValid( byEqpId ) &&
            m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_byOnline && 
            m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::IDLE)
        {
            pbyMPWBuf[byNum] = byEqpId;
            byNum ++;
        }
        byEqpId++;
    }
    return TRUE;
}

/*====================================================================
    函数名      ：GetIdlePrsChl
    功能        ：得到空闲的包重传通道
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：0-未找到 非0-EqpId
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/03/26    3.0         胡昌威           创建
====================================================================*/
u8   CMcuVcData::GetIdlePrsChl(u8 &byEqpId, u8 &byChlId, u8 &byChlId2, u8 &byChlIdAud)
{
    u8  byLoopEqpId = 1;
    u8  byLoopChlId = 0; 
    TPrsStatus tPrsStatus;

    while (byLoopEqpId < MAXNUM_MCU_PERIEQP)
    {
        if (IsPeriEqpValid(byLoopEqpId))
        {
            if (EQP_TYPE_PRS == GetEqpType(byLoopEqpId))
            {			
                if (m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus.m_byOnline)
                {
                    byLoopChlId = 0;
                    tPrsStatus = m_atPeriEqpTable[byLoopEqpId - 1].m_tPeriEqpStatus.m_tStatus.tPrs;
                    while (byLoopChlId < MAXNUM_PRS_CHNNL)
                    {
                        if (!tPrsStatus.m_tPerChStatus[byLoopChlId].IsReserved() &&
                            !tPrsStatus.m_tPerChStatus[byLoopChlId + 1].IsReserved() &&
                            !tPrsStatus.m_tPerChStatus[byLoopChlId + 2].IsReserved())
                        {
                            byEqpId = byLoopEqpId;
                            byChlId = byLoopChlId;
                            byChlId2 = byLoopChlId + 1;
                            byChlIdAud = byLoopChlId + 2;
                            return byEqpId;
                        }
                        byLoopChlId++;
                    }
                }
            }
        }
        byLoopEqpId++;
    }

    OspPrintf(TRUE, FALSE, "[GetIdlePrsChl]: No Idle prs chl can get!\n");

    return 0;
}

/*=============================================================================
  函 数 名： GetIdlePrsChl
  功    能： 获取一个空闲的包重传通道
  算法实现： 
  全局变量： 
  参    数： u8 byEqpId
  返 回 值： u8  
=============================================================================*/
u8   CMcuVcData::GetIdlePrsChl(u8 &byEqpId, u8 &byChlId)
{
    u8  byLoopEqpId = 1;
    u8  byLoopChlId = 0; 
    TPrsStatus tPrsStatus;
    
    while (byLoopEqpId < MAXNUM_MCU_PERIEQP)
    {
        if (IsPeriEqpValid(byLoopEqpId))
        {
            if (EQP_TYPE_PRS == GetEqpType(byLoopEqpId))
            {			
                if (m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus.m_byOnline)
                {
                    byLoopChlId = 0;
                    tPrsStatus = m_atPeriEqpTable[byLoopEqpId - 1].m_tPeriEqpStatus.m_tStatus.tPrs;
                    while (byLoopChlId < MAXNUM_PRS_CHNNL)
                    {
                        if (!tPrsStatus.m_tPerChStatus[byLoopChlId].IsReserved())                            
                        {
                            byEqpId = byLoopEqpId;
                            byChlId = byLoopChlId;                            
                            return byEqpId;
                        }
                        byLoopChlId++;
                    }
                }
            }
        }
        byLoopEqpId++;
    }
    
    OspPrintf(TRUE, FALSE, "[GetIdlePrsChl]: No Idle prs chl can get!\n");
    
    return 0;
}
/*====================================================================
    函数名      : GetIdlePrsChls
    功能        ：从所有在线Prs中得到空闲的通道数
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byChannelNum 需要的通道数
				  TPrsChannel& tPrsChannel 通道结构
    返回值说明  ：FALSE-未找到 TRUE-找到
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/02/22    4.0         john           创建
====================================================================*/
BOOL32 CMcuVcData::GetIdlePrsChls(u8 byChannelNum, TPrsChannel& tPrsChannel)
{
	u8  byEqpPrsId;
    TPrsStatus tPrsStatus;
    BOOL32 bRet = FALSE;
	u8 byIdleNum = GetMaxIdleChlsPrsId( byEqpPrsId ); // 得到在线Prs中最大空闲通道数
	if( byChannelNum <= byIdleNum ) // 有足够的通道数并且在线
	{
		u8 byChlIdx = 0;
		tPrsStatus = m_atPeriEqpTable[byEqpPrsId - 1].m_tPeriEqpStatus.m_tStatus.tPrs;
		for(u8 byChlLp = 0; byChlLp < MAXNUM_PRS_CHNNL; byChlLp++)
		{
			if (!tPrsStatus.m_tPerChStatus[byChlLp].IsReserved())
			{
				tPrsChannel.m_abyPrsChannels[byChlIdx++] = byChlLp;
				if ( byChlIdx == byChannelNum ) // 得到足够的通道数
				{
					break;
				}
			}
		}
		tPrsChannel.SetChannelNum( byChannelNum );
		tPrsChannel.SetPrsId( byEqpPrsId );
		bRet = TRUE;
	}
	return bRet;
}
/*====================================================================
    函数名      : GetIdlePrsChls
    功能        ：得到空闲的混音组
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byPrsId      指定PrsId
				  u8 byChannelNum 需要的通道数
				  TPrsChannel& tPrsChannel 通道结构
    返回值说明  ：FALSE-未找到 TRUE-找到
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/02/22    4.0         john           创建
====================================================================*/
BOOL32 CMcuVcData::GetIdlePrsChls(u8 byPrsId, u8 byChannelNum, TPrsChannel& tPrsChannel)
{
	BOOL32 bRet = FALSE;
	TPrsStatus tPrsStatus = m_atPeriEqpTable[byPrsId - 1].m_tPeriEqpStatus.m_tStatus.tPrs;
	if ( 1 == m_atPeriEqpTable[byPrsId-1].m_tPeriEqpStatus.m_byOnline ) // 在线
	{
		if( byChannelNum <= tPrsStatus.GetIdleChlNum() )
		{
			u8 byChlIdx = 0;
			for(u8 byChlLp = 0; byChlLp < MAXNUM_PRS_CHNNL; byChlLp++)
			{
				if (!tPrsStatus.m_tPerChStatus[byChlLp].IsReserved())
				{
					tPrsChannel.m_abyPrsChannels[byChlIdx++] = byChlLp;
					if ( byChlIdx == byChannelNum ) // 得到足够的通道数
					{
						break;
					}
				}
			}
			bRet = TRUE;
		}
	}
	return bRet;
}
/*====================================================================
    函数名      GetMaxIdleChlsPrsId
    功能        ：得到所有Prs中空闲通道数最多的PrsId
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpPrsId PrsId
    返回值说明  ：空闲通道数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/02/22    4.0         john           创建
====================================================================*/
u8 CMcuVcData::GetMaxIdleChlsPrsId(u8& byEqpPrsId )
{
    u8  byMaxChls = 0; 
    TPrsStatus tPrsStatus;
    u8  byLoopEqpId = PRSID_MIN;
    while (byLoopEqpId >= PRSID_MIN && byLoopEqpId <= PRSID_MAX)
    {
        if ( 1 == m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus.m_byOnline ) // 在线
		{
			if (IsPeriEqpValid(byLoopEqpId) 
				&& EQP_TYPE_PRS == GetEqpType(byLoopEqpId) ) // 合法性检测
			{
				tPrsStatus = m_atPeriEqpTable[byLoopEqpId - 1].m_tPeriEqpStatus.m_tStatus.tPrs;
				if( byMaxChls <= tPrsStatus.GetIdleChlNum() ) // 相同空闲数时先取后面的。
				{
					byEqpPrsId = byLoopEqpId;
					byMaxChls = tPrsStatus.GetIdleChlNum();
				}
            }
        }
		byLoopEqpId++;
    }
	return byMaxChls;
}
/*====================================================================
    函数名      ：GetIdleMixGroup
    功能        ：得到空闲的混音组
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 &byEqpId 返回的外设号
	              u8 &byGrpId 返回的混音组ID
    返回值说明  ：0-未找到 非0-EqpId
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/12/03    3.0         胡昌威           创建
====================================================================*/
u8   CMcuVcData::GetIdleMixGroup( u8 &byEqpId, u8 &byGrpId )
{
	u8  byLoopEqpId = 1;
	u8  byLoopGrpId = 0;
    TMixerStatus tMixerStatus;

    while( byLoopEqpId <= MAXNUM_MCU_PERIEQP )
	{
		if( IsPeriEqpValid( byLoopEqpId ) )
		{
			if( GetEqpType( byLoopEqpId ) == EQP_TYPE_MIXER )
			{
			    if( m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus.m_byOnline )
				{
					byLoopGrpId = 0;
					tMixerStatus = m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer;
					while( byLoopGrpId < tMixerStatus.m_byGrpNum )
					{
						if( tMixerStatus.m_atGrpStatus[byLoopGrpId].m_byGrpState == TMixerGrpStatus::READY )
						{
							byEqpId = byLoopEqpId;
							byGrpId = byLoopGrpId;
							return byEqpId;
						}

						byLoopGrpId++;				
					}
			
				}
			}
		}
		
        byLoopEqpId++;
	}

	return 0;	
}

/*====================================================================
    函数名      ：GetIdleBasChl
    功能        ：得到空闲的适配器通道
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 &byEqpId 返回的外设号
	              u8 &byChlId 返回的通道ID
    返回值说明  ：0-未找到 非0-EqpId
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/03/20    3.0         胡昌威           创建
====================================================================*/
u8 CMcuVcData::GetIdleBasChl(u8 byAdaptType, u8 &byEqpId, u8 &byChlId)
{
    u8 byLoopGrpId = 0;
    u8 byMinChnl;
    u8 byMaxChnl;
    TPeriEqpStatus *ptStatus;

    for (u8 byLoopEqpId = 1; byLoopEqpId < MAXNUM_MCU_PERIEQP; byLoopEqpId++)
    {
        if (!IsPeriEqpValid(byLoopEqpId))
        {
            continue;
        }
        if (EQP_TYPE_BAS != GetEqpType(byLoopEqpId))
        {
            continue;
        }

        if ( g_cMcuAgent.IsEqpBasHD(byLoopEqpId) )
        {
            continue;   
        }

        ptStatus = &(m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus);
        if (!ISTRUE(ptStatus->m_byOnline))
        {
            continue;
        }

        byMinChnl = 0;
        byMaxChnl = ptStatus->m_tStatus.tBas.byChnNum;
        
        //查找空闲通道
        for (u8 byChnIdx =byMinChnl; byChnIdx < byMaxChnl; byChnIdx++)
        {            
            if (!ptStatus->m_tStatus.tBas.tChnnl[byChnIdx].IsReserved())
            {                
                BOOL32 bFlag = FALSE;
                u8 byBasChnnlType = ptStatus->m_tStatus.tBas.tChnnl[byChnIdx].GetChannelType();
                if (BAS_CHAN_AUDIO == byBasChnnlType)
                {
                    if (ADAPT_TYPE_AUD == byAdaptType)
                    {
                        bFlag = TRUE;
                    }                    
                }
                else if (BAS_CHAN_VIDEO == byBasChnnlType)
                {
                    if (ADAPT_TYPE_VID == byAdaptType || ADAPT_TYPE_BR == byAdaptType)
                    {
                        bFlag = TRUE;
                    }                    
                }

                if (bFlag)
                {                    
                    ptStatus->m_tStatus.tBas.tChnnl[byChnIdx].SetReserved(TRUE);
                    byEqpId = byLoopEqpId;
                    byChlId = byChnIdx;
                    return byEqpId;
                }
            }
        }			
    }

	return 0;
}
/*=============================================================================
函 数 名： IsIdleHDBasVidChlExist
功    能： 查找是否有空闲的MAU主视频适配通道
算法实现： 
全局变量： 
参    数：
返 回 值： BOOL32
----------------------------------------------------------------------
修改记录    ：
日  期		版本		创建人		走读人    修改内容
09/06/03                付秀华             
=============================================================================*/
BOOL32 CMcuVcData::IsIdleHDBasVidChlExist(u8* pbyEqpId /*= NULL*/, u8* pbyChlIdx /*= NULL*/)
{
    TPeriEqpStatus *ptStatus;
    for (u8 byLoopEqpId = BASID_MIN; byLoopEqpId <= BASID_MAX; byLoopEqpId++)
    {
        if (!IsPeriEqpValid(byLoopEqpId))
        {
            continue;
        }
        if ( !g_cMcuAgent.IsEqpBasHD(byLoopEqpId) )
        {
            continue;   
        }

        ptStatus = &(m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus);
        if (!ISTRUE(ptStatus->m_byOnline))
        {
            continue;
        }

        u8 byEqpType = ptStatus->m_tStatus.tHdBas.GetEqpType();
        if (TYPE_MAU_H263PLUS == byEqpType)
        {
            continue;
		}

        u8 byChnIdx = 0;
        THDBasVidChnStatus tVidChn;

        //查找空闲通道
        switch (byEqpType)
        {
        case TYPE_MPU:
            for (byChnIdx = 0; byChnIdx < MAXNUM_MPU_CHN; byChnIdx++)
            {            
                memset( &tVidChn, 0, sizeof(tVidChn) );
                memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx), sizeof(tVidChn) );
                if (!tVidChn.IsReserved() &&
                    !tVidChn.IsTempReserved() &&
                    tVidChn.GetStatus() != THDBasChnStatus::IDLE )
                {
                    if (pbyEqpId != NULL)
                    {
                        *pbyEqpId = byLoopEqpId;
                    }
                    if (pbyChlIdx != NULL)
                    {
                        *pbyChlIdx = byChnIdx;
                    }
                    return TRUE;
                }
            }

        	break;

        case TYPE_MAU_NORMAL:
            for (byChnIdx = 0; byChnIdx < MAXNUM_MAU_VCHN; byChnIdx++)
            {            
                memset( &tVidChn, 0, sizeof(tVidChn) );
                memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(byChnIdx), sizeof(tVidChn) );
                if (!tVidChn.IsReserved() &&
                    !tVidChn.IsTempReserved() &&
                    tVidChn.GetStatus() != THDBasChnStatus::IDLE )
                {
                    if (pbyEqpId != NULL)
                    {
                        *pbyEqpId = byLoopEqpId;
                    }
                    if (pbyChlIdx != NULL)
                    {
                        *pbyChlIdx = byChnIdx;
                    }
                    return TRUE;
                }
            }

            break;
        default:
            OspPrintf(TRUE, FALSE, "[IsIdleHDBasVidChlExist] unexpected type.%d for eqp.%d!\n", byEqpType, byLoopEqpId);
            break;
        }		
	}
	return FALSE;
}
/*=============================================================================
函 数 名： GetIdleHDBasVidChl
功    能： 为第一路高清主视频获取空闲的MAU适配通道
算法实现： 
全局变量： 
参    数： 
           u8 &byEqpId
           u8 &byChlIdx
           BOOL32 bIsTemp
返 回 值： BOOL32
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/8/4   4.0		    周广程                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetIdleHDBasVidChl( u8 &byEqpId, u8 &byChlIdx )
{
    TPeriEqpStatus *ptStatus;
    
    for (u8 byLoopEqpId = BASID_MIN; byLoopEqpId <= BASID_MAX; byLoopEqpId++)
    {
        if (!IsPeriEqpValid(byLoopEqpId))
        {
            continue;
        }
        if ( !g_cMcuAgent.IsEqpBasHD(byLoopEqpId) )
        {
            continue;   
        }
       
        ptStatus = &(m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus);
		if (TYPE_MAU_H263PLUS == ptStatus->m_tStatus.tHdBas.GetEqpType())
		{
			continue;
		}

        if (!ISTRUE(ptStatus->m_byOnline))
        {
            continue;
        }
        
        u8 byEqpType = ptStatus->m_tStatus.tHdBas.GetEqpType();

        u8 byChnIdx = 0;
        THDBasVidChnStatus tVidChn;

        //查找空闲通道
        switch (byEqpType)
        {
        case TYPE_MPU:

            for (byChnIdx = 0; byChnIdx < MAXNUM_MPU_CHN; byChnIdx++)
            {            
                memset( &tVidChn, 0, sizeof(tVidChn) );
                memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx), sizeof(tVidChn) );
                if (!tVidChn.IsReserved() &&
                    !tVidChn.IsTempReserved() &&
                    tVidChn.GetStatus() != THDBasChnStatus::IDLE )
                {          
                    tVidChn.SetIsReserved( TRUE );
                    ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus( tVidChn, byChnIdx );
                    byEqpId = byLoopEqpId;
                    byChlIdx = byChnIdx;
                    return TRUE;
                }
            }	
        	break;

        case TYPE_MAU_NORMAL:

            for (byChnIdx = 0; byChnIdx < MAXNUM_MAU_VCHN; byChnIdx++)
            {            
                memset( &tVidChn, 0, sizeof(tVidChn) );
                memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(), sizeof(tVidChn) );
                if (!tVidChn.IsReserved() &&
                    !tVidChn.IsTempReserved() &&
                    tVidChn.GetStatus() != THDBasChnStatus::IDLE )
                {          
                    tVidChn.SetIsReserved( TRUE );
                    ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus( tVidChn );
                    byEqpId = byLoopEqpId;
                    byChlIdx = byChnIdx;
                    return TRUE;
                }
            }	
            break;
        default:
            OspPrintf(TRUE, FALSE, "[GetIdleHDBasVidChl] unexpected eqp type.%d for eqp.%d!\n", byEqpType, byLoopEqpId);
            break;
        }
    }
    
	return FALSE;
}

/*====================================================================
    函数名      ：GetIdleMauChn
    功能        ：获取空闲的MAU通道
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿        创建
====================================================================*/
BOOL32 CMcuVcData::GetIdleMauChn(u8 byChnType, u8 &byMauId, u8 &byChnId)
{
    byMauId = 0xff;
    byChnId = 0xff;

    TPeriEqpStatus *ptStatus;

    BOOL32 bMVChn = FALSE;
    BOOL32 bDSChn = FALSE;

    switch (byChnType)
    {
    case MAU_CHN_NORMAL:
    case MAU_CHN_263TO264:
        bMVChn = TRUE;
        break;
    case MAU_CHN_VGA:
    case MAU_CHN_264TO263:
        bDSChn = TRUE;
    	break;
    default:
        OspPrintf(TRUE, FALSE, "[GetIdleMauChn] unexpected chn type.%d\n", byChnType);
        return FALSE;
    }

    for (u8 byEqpId = BASID_MIN; byEqpId <= BASID_MAX; byEqpId++)
    {
        if (!IsPeriEqpValid(byEqpId))
        {
            continue;
        }
        if ( !g_cMcuAgent.IsEqpBasHD(byEqpId) )
        {
            continue;   
        }
        
        ptStatus = &(m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus);
        if (!ISTRUE(ptStatus->m_byOnline))
        {
            continue;
        }
        
        if ((MAU_CHN_VGA == byChnType || MAU_CHN_NORMAL == byChnType) &&
            ptStatus->m_tStatus.tHdBas.GetEqpType() != TYPE_MAU_NORMAL)
        {
            continue;
        }
        if ((MAU_CHN_263TO264 == byChnType || MAU_CHN_264TO263 == byChnType) &&
            ptStatus->m_tStatus.tHdBas.GetEqpType() != TYPE_MAU_H263PLUS)
        {
            continue;
        }

        if (bMVChn)
        {
            THDBasVidChnStatus tVidChn;
            u8 byChnIdx = 0;
            for (; byChnIdx < MAXNUM_MAU_VCHN; byChnIdx++)
            {            
                memset( &tVidChn, 0, sizeof(tVidChn) );
                memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(byChnIdx), sizeof(tVidChn) );
                if (!tVidChn.IsReserved() &&
                    !tVidChn.IsTempReserved() &&
                    tVidChn.GetStatus() == THDBasChnStatus::READY )
                {
                    tVidChn.SetIsReserved(TRUE);
                    ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tVidChn, byChnIdx );
                    byMauId = byEqpId;
                    byChnId = 0;
                    break;
                }
                else
                {
                    EqpLog("[GetIdleMau] Eqp.%d mv failed due to<ChnIdx.%d, Res.%d, Status.%d>\n",
                        byEqpId, byChnIdx, tVidChn.IsReserved(), tVidChn.GetStatus());
                    continue;
                }
            }
        }
        else if (bDSChn)
        {
            THDBasDVidChnStatus tDVidChn;
            u8 byChnIdx = 0;
            for (; byChnIdx < MAXNUM_MAU_VCHN; byChnIdx++)
            {            
                memset( &tDVidChn, 0, sizeof(tDVidChn) );
                memcpy( &tDVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(byChnIdx), sizeof(tDVidChn) );
                if (!tDVidChn.IsReserved() &&
                    !tDVidChn.IsTempReserved() &&
                    tDVidChn.GetStatus() == THDBasChnStatus::READY )
                {
                    tDVidChn.SetIsReserved(TRUE);
                    ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tDVidChn, byChnIdx );
                    byMauId = byEqpId;
                    byChnId = 1;
                    break;
                }
                else
                {
                    EqpLog("[GetIdleMau] Eqp.%d mv failed due to<ChnIdx.%d, Res.%d, Status.%d>\n",
                        byEqpId, byChnIdx, tDVidChn.IsReserved(), tDVidChn.GetStatus());
                    continue;
                }
            }
        }

        if (0xff != byMauId)
        {
            break;
        }
    }

    if (0xff != byMauId && 0xff != byChnId)
    {
        return TRUE;
    }
    return FALSE;
}

/*====================================================================
    函数名      ：GetIdleMpuChn
    功能        ：获取空闲的Mpu通道
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/05/19    4.6         张宝卿        创建
====================================================================*/
BOOL32 CMcuVcData::GetIdleMpuChn(u8 &byEqpId, u8 &byChnId)
{
    byEqpId = 0xff;
    byChnId = 0xff;

    TPeriEqpStatus *ptStatus;

    for (u8 byIdx = BASID_MIN; byIdx <= BASID_MAX; byIdx++)
    {
        if (!IsPeriEqpValid(byIdx))
        {
            continue;
        }
        if ( !g_cMcuAgent.IsEqpBasHD(byIdx) )
        {
            continue;   
        }
        
        ptStatus = &(m_atPeriEqpTable[byIdx-1].m_tPeriEqpStatus);
        if (!ISTRUE(ptStatus->m_byOnline))
        {
            continue;
        }
        if (ptStatus->m_tStatus.tHdBas.GetEqpType() != TYPE_MPU)
        {
            continue;
        }

        THDBasVidChnStatus tVidChn;
        u8 byChnIdx = 0;
        for (; byChnIdx < MAXNUM_MPU_CHN; byChnIdx++)
        {
            memset( &tVidChn, 0, sizeof(tVidChn) );
            memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx), sizeof(tVidChn) );
            if (!tVidChn.IsReserved() &&
                !tVidChn.IsTempReserved() &&
                tVidChn.GetStatus() == THDBasChnStatus::READY )
            {
                tVidChn.SetIsReserved(TRUE);
                ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tVidChn, byChnIdx );
                byEqpId = byIdx;
                byChnId = byChnIdx;
                break;
            }
            else
            {
                EqpLog("[GetIdleMpuChn] Eqp.%d failed due to<ChnIdx.%d, Res.%d, Status.%d>\n",
                    byIdx, byChnIdx, tVidChn.IsReserved(), tVidChn.GetStatus());
                continue;
            }
        }
        if (0xff != byEqpId && 0xff != byChnId)
        {
            break;
        }
    }

    if (0xff != byEqpId && 0xff != byChnId)
    {
        return TRUE;
    }
    return FALSE;
}

/*====================================================================
    函数名      ：ReleaseHDBasChn
    功能        ：释放占用的HD-BAS通道
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿        创建
====================================================================*/
void CMcuVcData::ReleaseHDBasChn(u8 byEqpId, u8 byChnId)
{
    TPeriEqpStatus *ptStatus;
    
    if (!IsPeriEqpValid(byEqpId))
    {
        return;
    }
    if ( !g_cMcuAgent.IsEqpBasHD(byEqpId) )
    {
        return;   
    }
    
    ptStatus = &(m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus);
    
    //zbq[11/27/2008] 不在线的EQP也要清理数据区
    //if (!ISTRUE(ptStatus->m_byOnline))
    //{
    //    return;
    //}

    u8 byAdpType = ptStatus->m_tStatus.tHdBas.GetEqpType();

    switch (byAdpType)
    {
    case TYPE_MAU_NORMAL:
    case TYPE_MAU_H263PLUS:
        {
            if (byChnId >= MAXNUM_MAU_VCHN + MAXNUM_MAU_DVCHN)
            {
                OspPrintf(TRUE, FALSE, "[ReleaseHDBasChn] unexpected mau chnId.%d\n", byChnId);
                return;
            }
            BOOL32 bReleaseMV = byChnId == 0 ? TRUE : FALSE;
         
            if (bReleaseMV)
            {
                THDBasVidChnStatus tVidChn;
                memset( &tVidChn, 0, sizeof(tVidChn) );
                memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(), sizeof(tVidChn) );          

                tVidChn.SetIsReserved(FALSE);
                tVidChn.SetStatus(THDBasChnStatus::READY);
				tVidChn.ClrOutputVidParam();
                ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tVidChn);
            }
            else
            {
                THDBasVidChnStatus tVidChn;
                memset( &tVidChn, 0, sizeof(tVidChn) );
                memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(), sizeof(tVidChn) );
                
                tVidChn.SetIsReserved(FALSE);
                tVidChn.SetStatus(THDBasChnStatus::READY);
				tVidChn.ClrOutputVidParam();
                ptStatus->m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tVidChn);
            }
        }
        break;

    case TYPE_MPU:
        {
            if (byChnId >= MAXNUM_MPU_VCHN)
            {
                OspPrintf(TRUE, FALSE, "[ReleaseHDBasChn] unexpected mpu chnId.%d\n", byChnId);
                return;
            }
            THDBasVidChnStatus tVidChn;
            memset( &tVidChn, 0, sizeof(tVidChn) );
            memcpy( &tVidChn, ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnId), sizeof(tVidChn) );
            
            tVidChn.SetIsReserved(FALSE);
            tVidChn.SetStatus(THDBasChnStatus::READY);
			tVidChn.ClrOutputVidParam();
            ptStatus->m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tVidChn, byChnId );  
        }
    	break;
    default:
        OspPrintf(TRUE, FALSE, "[ReleaseHDBasChn] unexpected EqpType.%d!\n", byAdpType);
        break;
    }
	SendPeriEqpStatusToMcs(byEqpId);
    return;
}

/*====================================================================
    函数名      ：ResetHDBasChn
    功能        ：清空通道内适配参数：防止残留
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/05/20    4.6         张宝卿        创建
====================================================================*/
void CMcuVcData::ResetHDBasChn(u8 byEqpId, u8 byChnId)
{
    TPeriEqpStatus tEqpStatus;
    if (!g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tEqpStatus))
    {
        OspPrintf(TRUE, FALSE, "[ResetHDBasChn] get Eqp.%d status failed!\n", byEqpId);
        return;
    }
    
    THDBasVidChnStatus tVidChn;
    memset(&tVidChn, 0, sizeof(tVidChn));
    
    u8 byBasType = tEqpStatus.m_tStatus.tHdBas.GetEqpType();
    switch (byBasType)
    {
    case TYPE_MAU_NORMAL:
    case TYPE_MAU_H263PLUS:
        {
            if (byChnId >= MAXNUM_MAU_VCHN + MAXNUM_MAU_DVCHN)
            {
                OspPrintf(TRUE, FALSE, "[TMVChnGrp::AddChn] unexpected mau chnId.%d!\n", byChnId);
                return;
            }
            
            BOOL32 bChnMV = byChnId == 0 ? TRUE : FALSE;
            
            if (bChnMV)
            {
                tVidChn = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus();
                
                for(u8 byIdx = 0; byIdx < MAXNUM_VOUTPUT; byIdx++)
                {
                    THDAdaptParam tParam = *tVidChn.GetOutputVidParam(byIdx);
                    tParam.Reset();
                    tVidChn.SetOutputVidParam(tParam, byIdx);
                }
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tVidChn);
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tEqpStatus);
            }
            else
            {
                tVidChn = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus();
                
                for(u8 byIdx = 0; byIdx < MAXNUM_VOUTPUT; byIdx++)
                {
                    THDAdaptParam tParam = *tVidChn.GetOutputVidParam(byIdx);
                    tParam.Reset();
                    tVidChn.SetOutputVidParam(tParam, byIdx);
                }
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tVidChn);
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tEqpStatus);
            }
        }
        break;
        
    case TYPE_MPU:
        {
            if (byChnId >= MAXNUM_MPU_CHN)
            {
                OspPrintf(TRUE, FALSE, "[TMVChnGrp::AddChn] unexpected mpu chnId.%d!\n", byChnId);
                return;
            }
            tVidChn = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnId);
            
            for(u8 byIdx = 0; byIdx < MAXNUM_VOUTPUT; byIdx++)
            {
                THDAdaptParam tParam = *tVidChn.GetOutputVidParam(byIdx);
                tParam.Reset();
                tVidChn.SetOutputVidParam(tParam, byIdx);
            }
            tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tVidChn, byChnId);
            g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tEqpStatus);
        }
        break;
        
    default:
        break;
    }

    g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tEqpStatus);
    
    return;
}

/*====================================================================
    函数名      ：GetIdleRecorder
    功能        ：查找是否有可用的录像机
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/07/18    4.0         顾振华        创建
====================================================================*/
BOOL32 CMcuVcData::IsRecorderOnline (u8 byEqpId)
{
    if( byEqpId < MAXNUM_MCU_PERIEQP )
	{
		if( GetEqpType( byEqpId ) == EQP_TYPE_RECORDER &&
            m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_byOnline )
		{
            // 目前就简单了事
			return TRUE;
		}
	}    
    return FALSE;
}

/*====================================================================
    函数名      ：SetEqpAlias
    功能        ：设置外设别名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR strAlias 外设别名	           
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/02/25    3.0         胡昌威           创建
====================================================================*/
void   CMcuVcData::SetEqpAlias( u8 byEqpId, LPCSTR lpszAlias )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP )
	{
		return;
	}

	if( lpszAlias != NULL )
	{
		strncpy( m_atPeriEqpTable[byEqpId-1].m_achAliase, lpszAlias, MAXLEN_EQP_ALIAS );
		m_atPeriEqpTable[byEqpId-1].m_achAliase[MAXLEN_EQP_ALIAS - 1] = '\0';
	}
	else
	{
		memset( m_atPeriEqpTable[byEqpId-1].m_achAliase, 0, MAXLEN_EQP_ALIAS );
	}
}

/*====================================================================
    函数名      ：GetEqpAlias
    功能        ：得到外设别名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR strAlias 外设别名	           
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/02/25    3.0         胡昌威           创建
====================================================================*/
LPCSTR CMcuVcData::GetEqpAlias( u8 byEqpId )
{
	if( byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP )
	{
		return NULL;
	}

	return m_atPeriEqpTable[byEqpId-1].m_achAliase;
}

/*====================================================================
    函数名      ：IsEqpH263pMau
    功能        ：得到mau 是否为H263p
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/01/08    4.5         张宝卿           创建
====================================================================*/
BOOL32 CMcuVcData::IsEqpH263pMau(u8 byEqpId)
{
    if (!g_cMcuAgent.IsEqpBasHD(byEqpId))
    {
        return FALSE;
    }

    TPeriEqpStatus tStatus;
    GetPeriEqpStatus(byEqpId, &tStatus);

    if (TYPE_MAU_H263PLUS == tStatus.m_tStatus.tHdBas.GetEqpType())
    {
        return TRUE;
    }
    return FALSE;
}

/*====================================================================
    函数名      ：GetIdleMau
    功能        ：得到mau 是否为H263p
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/01/08    4.5         张宝卿           创建
====================================================================*/
void CMcuVcData::GetIdleMau(u8 &byNVChn, u8 &byVGAChn, u8 &byH263pChn)
{
    BOOL32 bNVReserved = FALSE;
    BOOL32 bVGAReserved = FALSE;
    BOOL32 bH263p = FALSE;

    for(u8 byEqpId = BASID_MIN; byEqpId <= BASID_MAX; byEqpId++)
    {
        TPeriEqpStatus tStatus;
        if (GetPeriEqpStatus(byEqpId, &tStatus))
        {
            //hd only
            if (g_cMcuVcApp.IsPeriEqpConnected(byEqpId) &&
                g_cMcuAgent.IsEqpBasHD(byEqpId) &&
                !tStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->IsNull() &&
                !tStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->IsNull())
            {
                u8 byEqpType = tStatus.m_tStatus.tHdBas.GetEqpType();
                if (TYPE_MPU == byEqpType)
                {
                    continue;
                }

                u8 byConfIdx = tStatus.GetConfIdx();
                
                //one chnnl only
                bNVReserved = tStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->IsReserved();
                bVGAReserved = tStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus()->IsReserved();
                
                //h263p
                bH263p = byEqpType == TYPE_MAU_H263PLUS ? TRUE : FALSE;
                
                if (bH263p)
                {
                    if (!bNVReserved)
                    {
                        byH263pChn++;
                    }
                }
                else
                {
                    if (!bNVReserved)
                    {
                        byNVChn ++;
                    }
                    if (!bVGAReserved)
                    {
                        byVGAChn++;
                    }
                }
            }
        }
    }

    return;
}

/*====================================================================
    函数名      ：GetIdleMpu
    功能        ：得到空闲的mpu通道
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/05/19    4.6         张宝卿           创建
====================================================================*/
void CMcuVcData::GetIdleMpu(u8 &byChnNum)
{
    byChnNum = 0;
    BOOL32 bReserved = FALSE;
    
    for(u8 byEqpId = BASID_MIN; byEqpId <= BASID_MAX; byEqpId++)
    {
        TPeriEqpStatus tStatus;
        memset(&tStatus, 0, sizeof(tStatus));
        
        if (!GetPeriEqpStatus(byEqpId, &tStatus))
        {
            continue;
        }

        if (!g_cMcuVcApp.IsPeriEqpConnected(byEqpId) ||
            !g_cMcuAgent.IsEqpBasHD(byEqpId))
        {
            continue;
        }
        u8 byEqpType = tStatus.m_tStatus.tHdBas.GetEqpType();
        if (TYPE_MPU != byEqpType)
        {
            continue;
        }

        for(u8 byChnIdx = 0; byChnIdx < MAXNUM_MPU_CHN; byChnIdx ++)
        {
            THDBasVidChnStatus tVidChn;
            memset(&tVidChn, 0, sizeof(tVidChn));
            tVidChn = *tStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx);
            bReserved = tVidChn.IsReserved();
            
            if (!bReserved)
            {
                byChnNum ++;
            }
        }
    }
    
    return;
}

/*====================================================================
    函数名      ：GetMpuChn
    功能        ：得到mpu所有通道
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/05/19    4.6         张宝卿           创建
====================================================================*/
void CMcuVcData::GetMpuNum(u8 &byMpuNum)
{
    byMpuNum = 0;
    BOOL32 bReserved = FALSE;
    
    for(u8 byEqpId = BASID_MIN; byEqpId <= BASID_MAX; byEqpId++)
    {
        TPeriEqpStatus tStatus;
        memset(&tStatus, 0, sizeof(tStatus));
        
        if (!GetPeriEqpStatus(byEqpId, &tStatus))
        {
            continue;
        }
        
        if (!g_cMcuVcApp.IsPeriEqpConnected(byEqpId) ||
            !g_cMcuAgent.IsEqpBasHD(byEqpId))
        {
            continue;
        }
        u8 byEqpType = tStatus.m_tStatus.tHdBas.GetEqpType();
        if (TYPE_MPU != byEqpType)
        {
            continue;
        }
        byMpuNum += 1;
    }

    return;
}

/*====================================================================
    函数名      ：ShowBasInfo
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/04/11    4.5         张宝卿           创建
====================================================================*/
void CMcuVcData::ShowBasInfo()
{
    for(u8 byEqpId = BASID_MIN; byEqpId < BASID_MAX; byEqpId++)
    {
        if (!IsPeriEqpValid(byEqpId))
        {
            continue;
        }
        if (!IsPeriEqpConnected(byEqpId))
        {
            continue;
        }
        //暂时不支持非高清bas
        if (!g_cMcuAgent.IsEqpBasHD(byEqpId))
        {
            continue;
        }
        
        u8 byBasType = m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.GetEqpType();

        if (TYPE_MPU == byBasType)
        {
            OspPrintf(TRUE, FALSE, "\n---- MPU-BAS.%d info is as follows: ----\n\n", byEqpId);
            m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.Print();
            OspPrintf(TRUE, FALSE, "\n---- MPU-BAS.%d info is end         ----\n", byEqpId);
        }
        else
        {
            OspPrintf(TRUE, FALSE, "\n---- MAU-BAS.%d info is as follows: ----\n\n", byEqpId);
            m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.Print();
            OspPrintf(TRUE, FALSE, "\n---- MAU-BAS.%d info is end         ----\n", byEqpId);
        }
    }
}

/*====================================================================
    函数名      ：GetTvWallOutputMode
    功能        ：得到电视墙输出模式
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId 外设号	           
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/10/18    3.6         libo           创建
====================================================================*/
u8 CMcuVcData::GetTvWallOutputMode(u8 byEqpId)
{
    if (byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP)
    {
        return TW_OUTPUTMODE_NONE;
    }

    if (EQP_TYPE_TVWALL != m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.GetEqpType())
    {
        return TW_OUTPUTMODE_NONE;
    }

    return m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tTvWall.byOutputMode;
}

//4.6.1  新加  jlb
/*====================================================================
    函数名      : GetHduOutputMode
    功能        : 得到Hdu输出模式
    算法实现    :
    引用全局变量:
    输入参数说明: u8 byEqpId 外设号	           
    返回值说明  :
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
	09/01/19    4.6         江乐斌           创建
====================================================================*/
u8 CMcuVcData::GetHduOutputMode( u8 byEqpId )    
{
    if (byEqpId == 0 || byEqpId > MAXNUM_MCU_PERIEQP)
    {
        return HDU_OUTPUTMODE_NONE;
    }
	
    if (EQP_TYPE_HDU != m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.GetEqpType())
    {
        return HDU_OUTPUTMODE_NONE;
    }
	
    return m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdu.byOutputMode;
}

/*====================================================================
    函数名      ：EqpStatusShow
    功能        ：显示外设的状态信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId 返回的外设号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/01/09    3.0         zmy            创建
====================================================================*/
void CMcuVcData::EqpStatusShow( u8   byEqpId )
{
    s32 nChnnlNum;
    s32 nChnIdx, nGrpIdx;
    s8  achEqpType[][12]={"UNKNOW","MIXER","VMP","RECORDER","BAS","TVWALL","DCS","PRS","FILEENC","VMPTW","HDU" };
    OspPrintf( TRUE, FALSE, "_____________________________________________________________\n");
    if (byEqpId < 1) 
    {
        OspPrintf( TRUE, FALSE, "Eqp Id must from 1 - 140\n");
        OspPrintf( TRUE, FALSE, "Dcs Id must from 1 - 16 ( support only one now )\n");
        OspPrintf( TRUE, FALSE, "<<<<<<<<<<<<<    当前配置外设信息    >>>>>>>>>>>>>\n");
        for(s32 nIdx =1; nIdx <= MAXNUM_MCU_PERIEQP; nIdx++)
        {
            if (m_atPeriEqpTable[nIdx-1].m_bIsValid)
            {
                OspPrintf(TRUE, FALSE, " %s (Eqp.%2d) %10s %s \n",
                          m_atPeriEqpTable[nIdx-1].m_tPeriEqpStatus.m_byOnline ? "√":"×",
                          nIdx,
                          achEqpType[min(GetEqpType(nIdx), sizeof(achEqpType)/sizeof(achEqpType[0]))],
                          m_atPeriEqpTable[nIdx-1].m_tPeriEqpStatus.m_byOnline ? "在线":"掉线");
            }
        }
        u8 byDcsNum = 0;
        for(u8 byIdx = 1; byIdx <= MAXNUM_MCU_DCS; byIdx ++ )
        {            
            if (m_atPeriDcsTable[byIdx-1].m_bIsValid) 
            {
                byDcsNum ++ ;
                if ( 1 == byDcsNum )
                {
                    OspPrintf( TRUE, FALSE, "\n" );
                }
                OspPrintf(TRUE, FALSE, " %s (Dcs.%2d) %10s %s \n",
                          m_atPeriDcsTable[byIdx-1].m_tDcsStatus.m_byOnline ? "√":"×",
                          byIdx,
                          achEqpType[EQP_TYPE_DCS],
                          m_atPeriDcsTable[byIdx-1].m_tDcsStatus.m_byOnline ? "在线":"掉线");
            }
        }
        OspPrintf(TRUE, FALSE, "_____________________________________________________________\n\n");
        return;
    }

	OspPrintf(TRUE, FALSE, "\nEqpId = %d    ", byEqpId);
	if (!m_atPeriEqpTable[byEqpId-1].m_bIsValid)
	{
		OspPrintf(TRUE, FALSE, "NOT CONFIG\n");
		return;
	}

	if( !m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_byOnline )
	{
		OspPrintf( TRUE ,FALSE ,"    Offline\n");
		return;
	}
    
	OspPrintf(TRUE ,FALSE, "\n");
	switch( GetEqpType( byEqpId) ) 
	{
	case EQP_TYPE_BAS://码流适配器
		{
            if ( !g_cMcuAgent.IsEqpBasHD(byEqpId) )
            {
			    nChnnlNum = m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tBas.byChnNum;
			    OspPrintf( TRUE ,FALSE ," Status:  IDLE(0),READY(1),RUNING(2)\n");
			    OspPrintf( TRUE ,FALSE ," BAS.%d has %d channels\n\n", byEqpId, nChnnlNum);
			    OspPrintf( TRUE ,FALSE ,"%3s%7s%10s%14s%10s\n","CH","Status","Type A/V","  Resolution","Bitrate");
			    for(nChnIdx =0 ;nChnIdx <  min(MAXNUM_BAS_CHNNL,nChnnlNum);nChnIdx++)
			    {
				    OspPrintf( TRUE ,FALSE ,"%3d %6d   %2d/%2d   %5d X %-5d%10d\n",nChnIdx,
					    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tBas.tChnnl[nChnIdx].GetStatus(),
					    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tBas.tChnnl[nChnIdx].GetAudType(),
					    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tBas.tChnnl[nChnIdx].GetVidType(),
					    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tBas.tChnnl[nChnIdx].GetWidth(),
					    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tBas.tChnnl[nChnIdx].GetHeight(),
					    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tBas.tChnnl[nChnIdx].GetBitrate());
			    }
            }
            else
            {
                OspPrintf( TRUE ,FALSE ," Status:  IDLE(0),READY(1),RUNING(2)\n");
                OspPrintf( TRUE ,FALSE ," Reserve type:  IDLE(0),Reserved(1),Temp reserved(2)\n");
                OspPrintf( TRUE ,FALSE ," %-6s %-3s %-5s %-10s %-14s%\n","EQPID","CH","Type","Status","Reservetype");
                for ( nChnIdx = 0; nChnIdx < MAXNUM_MAU_VCHN; nChnIdx++ )
                {
                    OspPrintf( TRUE ,FALSE ," %-6d %-3d %-5s %-10d %-4d\n",
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(nChnIdx)->GetEqpId(),
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(nChnIdx)->GetChnIdx(),
                    "VID",
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(nChnIdx)->GetStatus(),
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(nChnIdx)->GetReservedType());
                }
                for ( nChnIdx = 0; nChnIdx < MAXNUM_MAU_DVCHN; nChnIdx++ )
                {
                    OspPrintf( TRUE ,FALSE ," %-6d %-3d %-5s %-10d %-4d\n",
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(nChnIdx)->GetEqpId(),
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(nChnIdx)->GetChnIdx(),
                    "DVID",
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(nChnIdx)->GetStatus(),
                    m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus(nChnIdx)->GetReservedType());
                }
            }
		}
		break;
	case EQP_TYPE_MIXER://混音器
		{
			u8   byIdx;
			u8   byMmbNum;
			u8   abyActiveMmb[MAXNUM_MIXER_DEPTH];
			u8   abyMixMmb[256];
			static char status[3][7]={"  IDLE"," READY","MIXING"};
			OspPrintf( TRUE ,FALSE ,"Mixer.%d has %d Groups \n",byEqpId,m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_byGrpNum);

			for( nGrpIdx=0 ;
			nGrpIdx< m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_byGrpNum;
			nGrpIdx++)
			{
				
				OspPrintf( TRUE ,FALSE ,"\n--------------------  [ Grp.%d ]--------------------\n",m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[nGrpIdx].m_byGrpId );
				OspPrintf( TRUE ,FALSE ,"Status : %s               Depth  : %d\n",
					status[m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[nGrpIdx].m_byGrpState],
					m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[nGrpIdx].m_byGrpMixDepth);
				OspPrintf( TRUE ,FALSE ,"Active Mmb:");
				
				byMmbNum =m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[nGrpIdx].GetActiveMmb( abyActiveMmb ,MAXNUM_MIXER_DEPTH);
				for( byIdx=0 ;byIdx< byMmbNum ;byIdx++)
				{
					OspPrintf( TRUE ,FALSE ," %d",abyActiveMmb[byIdx]);		
				}
				OspPrintf( TRUE ,FALSE ,"\n");
				OspPrintf( TRUE ,FALSE ,"Mixing Member Id\n");
				
				byMmbNum = m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[nGrpIdx].GetMixMmb( abyMixMmb ,200);
				for( byIdx=0 ;byIdx< byMmbNum ;byIdx++)
				{
					OspPrintf( TRUE ,FALSE ," %3d",abyActiveMmb[byIdx]);
					if( ((byIdx+1)%10)==0 )
						OspPrintf( TRUE ,FALSE ,"\n");
				}
			}
		}

		break;
	case EQP_TYPE_RECORDER://录像机
		{
			TRecChnnlStatus tChnStatus;
			u8 byRecChnNum ,byPlayChnNum ,byChnIdx;
			TRecStatus * ptRecStatus = &m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tRecorder;
			byRecChnNum  = ptRecStatus->GetRecChnnlNum();
			byPlayChnNum = ptRecStatus->GetPlayChnnlNum();

			OspPrintf(TRUE,FALSE ,"_____________________ Recorder.%d ___________________\n",byEqpId);
			OspPrintf(TRUE,FALSE,"%-6s %-12s %-10s %6s/%-6s %s\n","  CH","    Status","    Mode","Cur(S)","Tot(S)","Recording Name ");
			for( byChnIdx = 0 ;byChnIdx < byRecChnNum ;byChnIdx++ )
			{
				ptRecStatus->GetChnnlStatus( byChnIdx ,TRecChnnlStatus::TYPE_RECORD ,&tChnStatus);
				OspPrintf( TRUE ,FALSE ,"CH.%2d  %12s %10s %6d/%-6d %s\n",byChnIdx 
					,tChnStatus.GetStatusStr( tChnStatus.m_byState)
					,tChnStatus.m_byRecMode == TRecChnnlStatus::MODE_REALTIME ?"NORMAL":"SKIP"
					,tChnStatus.m_tProg.GetCurProg(),tChnStatus.m_tProg.GetTotalTime() 
					,tChnStatus.GetRecordName());
			}

			OspPrintf(TRUE,FALSE,"\t__________________________________________\n");
			OspPrintf(TRUE,FALSE,"%-6s %-12s %6s/%-6s %s\n","  CH","    Status","Cur(S)","Tot(S)","Playing Name ");
			for( byChnIdx = 0 ;byChnIdx < byRecChnNum ;byChnIdx++ )
			{
				ptRecStatus->GetChnnlStatus( byChnIdx ,TRecChnnlStatus::TYPE_PLAY ,&tChnStatus);
				OspPrintf( TRUE ,FALSE ,"CH.%2d  %12s %6d/%-6d %s\n",byChnIdx 
					,tChnStatus.GetStatusStr( tChnStatus.m_byState) 
					,tChnStatus.m_tProg.GetCurProg(),tChnStatus.m_tProg.GetTotalTime()
					,tChnStatus.GetRecordName());
			}
		}
		break;
	case EQP_TYPE_TVWALL:
		{
			TTvWallStatus *ptTWStatus;
			ptTWStatus = &m_atPeriEqpTable[byEqpId-1].m_tPeriEqpStatus.m_tStatus.tTvWall;
			OspPrintf(TRUE,FALSE,"_________________%d channels TV Wall ________________\n",ptTWStatus->byChnnlNum);
			for( u8 byChnIdx=0 ;byChnIdx < ptTWStatus->byChnnlNum ;byChnIdx++)
			{
				OspPrintf(TRUE,FALSE,"Chn.%d Playing Mcu.%d MT.%d\n",byChnIdx
					,ptTWStatus->atVideoMt[byChnIdx].GetMcuId()
					,ptTWStatus->atVideoMt[byChnIdx].GetMtId());
			}
		}
		break;

	default:
		break;
	};

}

/*====================================================================
    函数名      ：AssignMtPort
    功能        ：分配终端端口
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx 会议索引号
				  u8 byMtId 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/03/02    3.0         胡昌威          创建
====================================================================*/
u16  CMcuVcData::AssignMtPort( u8 byConfIdx, u8 byMtId )
{
	u16 wPort = 0;
	u16 wPortIndex = 0;
	while( wPortIndex < MAXNUM_MCU_MT )
	{
		if( m_atRecvMtPort[wPortIndex].m_byPortInUse )
		{
			wPortIndex++;
		}
		else
		{
			break;
		}
	}
	
	if( wPortIndex < MAXNUM_MCU_MT )
	{
		wPort = g_cMcuAgent.GetRecvStartPort() + PORTSPAN * wPortIndex;
		m_atRecvMtPort[wPortIndex].m_byPortInUse = TRUE;
		m_atRecvMtPort[wPortIndex].m_byConfIdx = byConfIdx;
		m_atRecvMtPort[wPortIndex].m_byMtId = byMtId;
	}
	else
	{
		wPort = 0;
	}

	return wPort;
}

/*====================================================================
    函数名      ：ReleaseMtPort
    功能        ：释放终端端口
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx 会议索引号
				  u8 byMtId 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/03/02    3.0         胡昌威          创建
====================================================================*/
void CMcuVcData::ReleaseMtPort( u8 byConfIdx, u8 byMtId )
{
	u16 wPortIndex = 0;
	while( wPortIndex < MAXNUM_MCU_MT )
	{
		if( m_atRecvMtPort[wPortIndex].m_byConfIdx == byConfIdx 
			&& m_atRecvMtPort[wPortIndex].m_byMtId == byMtId )
		{
			m_atRecvMtPort[wPortIndex].m_byPortInUse = FALSE;
			m_atRecvMtPort[wPortIndex].m_byConfIdx = 0;
			m_atRecvMtPort[wPortIndex].m_byMtId = 0;
			break;
		}
		else
		{
			wPortIndex++;
		}
	}
}

/*====================================================================
    函数名      ：AssignMulticastIp
    功能        ：分配组播Ip地址
    算法实现    ：231.255.255.255 --- 233.0.0.0为保留范围
    引用全局变量：
    输入参数说明：无
    返回值说明  ：分配的组播Ip地址 - 主机序
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/09/27    4.0         libo          创建
    06/04/06    4.0         顾振华        根据会议模版，而不再自动生成
====================================================================*/
u32  CMcuVcData::AssignMulticastIp(u8 byConfIdx)
{
    u32 dwMultiCastAddr = 0;
    CMcuVcInst* pVcInst = GetConfInstHandle(byConfIdx);
    if ( NULL != pVcInst )
    {
        // 顾振华@2006.4.8 直接从会议里面获取
        dwMultiCastAddr = pVcInst->m_tConf.GetConfAttrb().GetMulticastIp();
    }
    else
    {
        // Fatal error!
        OspPrintf(TRUE, FALSE, "Cannot get VC Instance(conf idx: %d) while AssignMulticastIp!\n", 
                            byConfIdx);
    }

    return dwMultiCastAddr;
}

/*====================================================================
    函数名      ：AssignMulticastPort
    功能        ：分配组播端口
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx 会议索引号
				  u8 byMtId 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/03/02    3.0         胡昌威        创建
    06/04/06    4.0         顾振华        根据会议模版，而不再自动生成
====================================================================*/
u16  CMcuVcData::AssignMulticastPort( u8 byConfIdx, u8 byMtId )
{
	u16 wMultiCastPort;
	u16 wPort = 0;
	u16 wPortIndex = 0;

    CMcuVcInst* pVcInst = GetConfInstHandle(byConfIdx);
    if ( NULL == pVcInst )
    {
        OspPrintf(TRUE, FALSE, "Cannot get VC Instance(conf idx: %d) while AssignMulticastPort!\n", 
                            byConfIdx);
        return 0;
    }

    // 顾振华@2006.4.8 直接从会议里面获取
    wMultiCastPort = pVcInst->m_tConf.GetConfAttrb().GetMulticastPort();    
    
    // xsl [9/25/2006] 会议组播不分配port
    if (0 == byMtId)
    {
        return wMultiCastPort;
    }
    
	//该端口是否已分配好
	while( wPortIndex < MAXNUM_MCU_MT )
	{
		if( m_atMulticastPort[wPortIndex].m_byConfIdx == byConfIdx 
			&& m_atMulticastPort[wPortIndex].m_byMtId == byMtId )
		{           
			wPort = wMultiCastPort + 4 * wPortIndex;
			return wPort;
		}
		else
		{
			wPortIndex++;
		}
	}

    //查找空闲端口
	wPortIndex = 0;
	while( wPortIndex < MAXNUM_MCU_MT )
	{
		if( m_atMulticastPort[wPortIndex].m_byPortInUse )
		{
			wPortIndex++;
		}
		else
		{
			break;
		}
	}
	
	//分配端口
	if( wPortIndex < MAXNUM_MCU_MT )
	{
		wPort = wMultiCastPort + 4 * wPortIndex;
		m_atMulticastPort[wPortIndex].m_byPortInUse = TRUE;
		m_atMulticastPort[wPortIndex].m_byConfIdx = byConfIdx;
		m_atMulticastPort[wPortIndex].m_byMtId = byMtId;
	}
	else
	{
		wPort = 0;
	}

	return wPort;
}

/*====================================================================
    函数名      ：IsMulticastAddrOccupied
    功能        ：查找所有的即时会议，是否启用了相应的地址
    算法实现    ：暂时一个会议占用一个IP地址，不允许多会议占用1个
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    06/04/06    4.0         顾振华        创建
====================================================================*/
BOOL32 CMcuVcData::IsMulticastAddrOccupied(u32 dwCastIp, u16 wCastPort)
{
    // 暂时不考虑端口
	CMcuVcInst* pcVcInst = NULL;
	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_CONF; byInstID++)
	{
		pcVcInst = m_apConfInst[byInstID-1];
		if (NULL != pcVcInst)
		{
            if (pcVcInst->m_tConf.m_tStatus.IsOngoing() &&
                pcVcInst->m_tConf.GetConfAttrb().GetMulticastIp() == dwCastIp )
            {
                // 发现已经被占用
                return TRUE;
            }
		}	
	}    
    return FALSE;
}

/*====================================================================
    函数名      ：ReleaseMulticastPort
    功能        ：释放组播端口
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx 会议索引号
				  u8 byMtId 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/03/02    3.0         胡昌威          创建
====================================================================*/
void CMcuVcData::ReleaseMulticastPort( u8 byConfIdx, u8 byMtId )
{
	u16 wPortIndex = 0;
	while( wPortIndex < MAXNUM_MCU_MT )
	{
		if( m_atMulticastPort[wPortIndex].m_byConfIdx == byConfIdx 
			&& m_atMulticastPort[wPortIndex].m_byMtId == byMtId )
		{
			m_atMulticastPort[wPortIndex].m_byPortInUse = FALSE;
			m_atMulticastPort[wPortIndex].m_byConfIdx = 0;
			m_atMulticastPort[wPortIndex].m_byMtId = 0;
			break;
		}
		else
		{
			wPortIndex++;
		}
	}
}

/*====================================================================
    函数名      ：IsDistrConfCastAddrOccupied
    功能        ：查找所有的即时会议，是否启用了相应的分散会议组播地址
    算法实现    ：只有当地址和端口都被占用才认为被占用
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/04/12    4.0         顾振华        创建
====================================================================*/
BOOL32 CMcuVcData::IsDistrConfCastAddrOccupied( u32 dwCastIp, u16 wCastPort) 
{
	CMcuVcInst* pcVcInst = NULL;
	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_CONF; byInstID++)
	{
		pcVcInst = m_apConfInst[byInstID-1];
		if (NULL != pcVcInst)
		{
            if (pcVcInst->m_tConf.m_tStatus.IsOngoing() &&
                pcVcInst->m_tConf.GetConfAttrb().GetSatDCastIp() == dwCastIp &&
                pcVcInst->m_tConf.GetConfAttrb().GetSatDCastPort() == wCastPort)
            {
                // 发现已经被占用
                return TRUE;
            }
		}	
	}    
    return FALSE;
}

/*=============================================================================
    函 数 名： GetBaseInfoFromDebugFile
    功    能： 获取指定MCU Debug文件中基本调试信息设置
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/22  4.0			万春雷                  创建
=============================================================================*/
void CMcuVcData::GetBaseInfoFromDebugFile( )
{
	McuGetDebugKeyValue(m_tMcuDebugVal);

	return;
}

/*=============================================================================
    函 数 名： GetSatInfoFromDebugFile
    功    能： 获取卫星会议的相关配置信息
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2009/9/11   4.6			张宝卿                  创建
=============================================================================*/
void CMcuVcData::GetSatInfoFromDebugFile()
{
    McuGetSateliteConfigValue(m_tSatInfo);
}

/*=============================================================================
    函 数 名： GetMcuTelnetPort
    功    能： 获取指定MCU Debug文件中MCU_TELNET端口设置
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： u16 主机序 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/22  4.0			万春雷                  创建
=============================================================================*/
u16 CMcuVcData::GetMcuTelnetPort( )
{
	return m_tMcuDebugVal.m_wMcuTelnetPort;
}

/*=============================================================================
    函 数 名： GetBaseInfoFromDebugFile
    功    能： 获取指定MCU Debug文件中MCU_LISTEN端口设置
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： u16 主机序 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/22  4.0			万春雷                  创建
=============================================================================*/
u16 CMcuVcData::GetMcuListenPort( )
{
	return m_tMcuDebugVal.m_wMcuListenPort;
}

/*=============================================================================
    函 数 名： GetMcsRefreshInterval
    功    能： 获取指定MCU Debug文件中MCS的终端转台刷新间隔设置
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： u32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/22  4.0			万春雷                  创建
=============================================================================*/
u32 CMcuVcData::GetMcsRefreshInterval( )
{
	return m_tMcuDebugVal.m_dwMcsRefreshInterval;
}

/*=============================================================================
    函 数 名： IsWatchDogEnable
    功    能： 获取指定MCU Debug文件中看门狗是否激活设置
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/22  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsWatchDogEnable( )
{
	return m_tMcuDebugVal.m_bWDEnable;
}

/*=============================================================================
函 数 名： GetMsSynTime
功    能： 获得主备同步时间
算法实现： 
全局变量： 
参    数： void
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/4/2  4.0			周广程                  创建
=============================================================================*/
u16	 CMcuVcData::GetMsSynTime(void)
{
	return m_tMcuDebugVal.m_wMsSynTime;
}

/*=============================================================================
    函 数 名： GetMtCallInterfaceInfoFromDebugFile
    功    能： 获取指定MCU Debug文件关于指定终端的协议适配板资源及码流转发板资源的设置
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/25   3.6			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetMtCallInterfaceInfoFromDebugFile( )
{
	m_dwMtCallInterfaceNum = MAXNUM_MCU_MT;
	if( FALSE == McuGetMtCallInterfaceInfo(m_atMtCallInterface, m_dwMtCallInterfaceNum) )
	{
		memset(m_atMtCallInterface, 0, sizeof(m_atMtCallInterface));
		m_dwMtCallInterfaceNum = 0;

		return FALSE;
	}

	return TRUE;	
}

/*=============================================================================
    函 数 名： GetMpIdAndH323MtDriIdFromMtAlias
    功    能： 根据终端别名 获取 该终端的协议适配板资源及码流转发板资源的设置
    算法实现： 
    全局变量： 
    参    数： u32      [in] tMtAlias       //终端的别名
	           u32      [out]&dwMtadpIpAddr //协议适配板IP地址, 网络序
	           u32      [out]&dwMpIpAddr    //码流转发板IP地址, 网络序
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/25   3.6			万春雷                  创建
    2006/12/20  4.0-R4      王亮                    增加E164号码段支持
=============================================================================*/
BOOL32 CMcuVcData::GetMpIdAndH323MtDriIdFromMtAlias( TMtAlias &tMtAlias, u32 &dwMtadpIpAddr, u32 &dwMpIpAddr )
{
	for( u32 dwLoop = 0; dwLoop < m_dwMtCallInterfaceNum; dwLoop++ )
	{
        switch (m_atMtCallInterface[dwLoop].m_byAliasType)
        {
        case mtAliasTypeTransportAddress:
            if( tMtAlias.m_AliasType == mtAliasTypeTransportAddress &&
                 m_atMtCallInterface[dwLoop].m_tIpSeg.IsIpIn( tMtAlias.m_tTransportAddr.GetNetSeqIpAddr() ) )
            {
			    dwMtadpIpAddr = m_atMtCallInterface[dwLoop].m_dwMtadpIpAddr;
			    dwMpIpAddr    = m_atMtCallInterface[dwLoop].m_dwMpIpAddr;
                return TRUE;
            }
            break;
        case mtAliasTypeE164:
            //判断E164号码是否在号码段内
            if( tMtAlias.m_AliasType == mtAliasTypeE164 &&
                m_atMtCallInterface[dwLoop].IsE164Same( tMtAlias.m_achAlias ) )
             {
                MtLog("[GetMpIdAndH323MtDriIdFromMtAlias]: E164=%s in E164 seg=%s.\n", tMtAlias.m_achAlias, m_atMtCallInterface[dwLoop].m_szE164);
			    dwMtadpIpAddr = m_atMtCallInterface[dwLoop].m_dwMtadpIpAddr;
			    dwMpIpAddr    = m_atMtCallInterface[dwLoop].m_dwMpIpAddr;
                return TRUE;
            }
            break;
        default:
            break;
        }
	}
	return FALSE;
}

/*=============================================================================
    函 数 名： GetMSDetermineType
    功    能： 获取指定MCU Debug文件中 主备用结果提取方式 设置
	           vxworks下硬件平台支持主备用功能，则直接由os获取主备用结果，默认不支持
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/26  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetMSDetermineType( )
{
	return m_tMcuDebugVal.m_bMSDetermineType;
}


/*=============================================================================
函 数 名： GetBitrateScale
功    能： 获取适配码率比例
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/4/11  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetBitrateScale()
{
    return m_tMcuDebugVal.m_byBitrateScale;
}


/*=============================================================================
函 数 名： IsMMcuSpeaker
功    能： 是否指定上级作发言人
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/4/11  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsMMcuSpeaker() const
{
    return (0 != m_tMcuDebugVal.m_byIsMMcuSpeaker);
}

/*=============================================================================
函 数 名： IsShowMMcuMtList
功    能： 级联时是否显示上级MCU的终端列表
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/04/30  4.0			顾振华                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsShowMMcuMtList() const
{
    return (0 != m_tMcuDebugVal.m_byShowMMcuMtList);
}

/*=============================================================================
函 数 名： SetSMcuCasPort
功    能： 设置欲连接的下级MCU的侦听端口。
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/6/15  4.0			顾振华                  创建
=============================================================================*/
void CMcuVcData::SetSMcuCasPort(u16 wPort)
{
    m_tMcuDebugVal.m_wSMcuCasPort = wPort;
}

/*=============================================================================
函 数 名： GetSMcuCasPort
功    能： 返回欲连接的下级MCU的侦听端口。本值在telnet里面通过SetSMcuCasPort设置
算法实现： 
全局变量： 
参    数： 
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/6/15  4.0			顾振华                  创建
=============================================================================*/
u16 CMcuVcData::GetSMcuCasPort() const
{
    return m_tMcuDebugVal.m_wSMcuCasPort;
}

/*=============================================================================
函 数 名： IsAutoDetectMMcuDupCall
功    能： 当上级MCU重新呼叫时，是否检测是否是同一个上级MCU
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/6/22  4.0			顾振华                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsAutoDetectMMcuDupCall() const
{
    return (0 != m_tMcuDebugVal.m_byIsAutoDetectMMcuDupCall);
}

/*=============================================================================
函 数 名： GetCascadeAliasType
功    能： 获取级联时MCU在会议中别名的显示方式
算法实现： 
全局变量： 
参    数： 
返 回 值： 0 - MCU Alias+Conf H.323ID,  1 - Mcu Alias Only, 2 - Conf H.323ID Only
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/4/30   4.0			顾振华                  创建
=============================================================================*/
u8 CMcuVcData::GetCascadeAliasType() const
{
    return m_tMcuDebugVal.m_byCascadeAliasType;
}

/*=============================================================================
函 数 名： IsApplyChairToZxMcu
功    能： 是否向中兴mcu申请主席
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/6/22  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsApplyChairToZxMcu() const
{
	return (0 != m_tMcuDebugVal.m_byIsApplyChairToZxMcu);
}

/*=============================================================================
函 数 名： IsTransmitMtShortMsg
功    能： 是否转发终端短消息
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/3  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsTransmitMtShortMsg() const
{
    return (0 != m_tMcuDebugVal.m_byIsTransmitMtShortMsg);
}

/*=============================================================================
函 数 名： IsChairDisplayMtApplyInfo
功    能： 是否将终端申请主席或发言人的提示信息告诉主席终端
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/3  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsChairDisplayMtApplyInfo() const
{
    return (0 != m_tMcuDebugVal.m_byIsChairDisplayMtApplyInfo);
}

/*=============================================================================
函 数 名： IsSelInDoubleMediaConf
功    能： 是否允许双格式会议选看
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/10  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsSelInDoubleMediaConf() const
{
    return (0 != m_tMcuDebugVal.m_byIsSelInDoubleMediaConf);
}

/*=============================================================================
函 数 名： IsLimitAccessByMtModal
功    能： 是否根据终端和MCU的类型限制终端呼入
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/05/14  4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsLimitAccessByMtModal() const
{
    return (0 == m_tMcuDebugVal.m_byIsNotLimitAccessByMtModal);
}

/*=============================================================================
函 数 名： IsSupportSecDSCap
功    能： 是否根据支持第二双流能力
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/15  4.5			张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsSupportSecDSCap() const
{
    return (0 != m_tMcuDebugVal.m_byIsSupportSecDSCap);
}

/*=============================================================================
函 数 名： IsAllowVmpMemRepeated
功    能： 是否允许单通道下VMP成员重复
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：  
日  期		版本		修改人		走读人    修改内容
2009/03/21  4.5			张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsAllowVmpMemRepeated() const
{
    return (0 != m_tMcuDebugVal.m_byIsAllowVmpMemRepeated);
}
/*=============================================================================
函 数 名： IsSendFakeCap2Polycom
功    能： 是否约束面向polycom的能力集
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/05/14  4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsSendFakeCap2Polycom() const
{
    return (1 == m_tMcuDebugVal.m_byIsSendFakeCap2Polycom);
}
/*=============================================================================
函 数 名： IsSendFakeCap2Taide
功    能： 是否约束面向polycom的能力集
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/05/14  4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsSendFakeCap2Taide() const
{
    return (1 == m_tMcuDebugVal.m_byIsSendFakeCap2Taide);
}
/*=============================================================================
函 数 名： IsSendFakeCap2TaideHD
功    能： 是否手动提升面向polycom的能力集
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/08/31  4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsSendFakeCap2TaideHD() const
{
    return (1 == m_tMcuDebugVal.m_byIsSendFakeCap2TaideHD);
}

/*=============================================================================
 函 数 名： IsAdpResourceCompact
 功    能： 是否紧凑使用当前视频资源
 算法实现： 
 全局变量： 
 参    数： 
 返 回 值： BOOL32 
 ----------------------------------------------------------------------
 修改记录    ：
 日  期		版本		修改人		走读人    修改内容
 2009/05/22  4.5		张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsAdpResourceCompact() const
{
    return (0 != m_tMcuDebugVal.m_byIsAdpResourceCompact);
}

/*=============================================================================
 函 数 名： IsSVmpOutput1080i
 功    能： 是否强制SVMP编码输出1080i，即同时放弃720p
 算法实现： 
 全局变量： 
 参    数： 
 返 回 值： BOOL32 
 ----------------------------------------------------------------------
 修改记录    ：
 日  期		    版本		修改人		走读人    修改内容
 2009/07/26     4.5		    张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsSVmpOutput1080i() const
{
    return (0 != m_tMcuDebugVal.m_byIsSVmpOutput1080i);
}


BOOL32 CMcuVcData::IsConfAdpManually() const
{
	return (0 != m_tMcuDebugVal.m_byConfAdpManually);
}

/*=============================================================================
 函 数 名： GetBandWidthReserved4HdBas
 功    能： 获取为HD-BAS预留的带宽
 算法实现： 
 全局变量： 
 参    数： 
 返 回 值： BOOL32 
 ----------------------------------------------------------------------
 修改记录    ：
 日  期		    版本		修改人		走读人    修改内容
 2009/07/26     4.6		    张宝卿                  创建
=============================================================================*/
u8 CMcuVcData::GetBandWidthReserved4HdBas() const
{
    return m_tMcuDebugVal.m_byBandWidthReserved4HdBas;
}

/*=============================================================================
 函 数 名： GetBandWidthReserved4HdVmp
 功    能： 获取为HD-VMP预留的带宽
 算法实现： 
 全局变量： 
 参    数： 
 返 回 值： BOOL32 
 ----------------------------------------------------------------------
 修改记录    ：
 日  期		    版本		修改人		走读人    修改内容
 2009/07/26     4.6		    张宝卿                  创建
=============================================================================*/
u8 CMcuVcData::GetBandWidthReserved4HdVmp() const
{
    return m_tMcuDebugVal.m_byBandWidthReserved4HdVmp;
}


BOOL32 CMcuVcData::IsDistinguishHDSDMt() const
{
	return (1 == m_tMcuDebugVal.m_byIsDistingtishSDHDMt);
}

BOOL32 CMcuVcData::IsVidAdjustless4Polycom() const
{
	return (1 == m_tMcuDebugVal.m_byIsVidAdjustless4Polycom);
}

BOOL32 CMcuVcData::IsSelAccord2Adp() const
{
    return (1 == m_tMcuDebugVal.m_byIsSelAccord2Adp);
}

/*=============================================================================
函 数 名： GetPerfLimit
功    能： 获取性能参数
算法实现： 
全局变量： 
参    数： 
返 回 值： TMcuPerfLimit
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/1/17  4.0			顾振华                  创建
=============================================================================*/
TMcuPerfLimit& CMcuVcData::GetPerfLimit()
{
    return m_tMcuDebugVal.m_tPerfLimit;
}

/*=============================================================================
函 数 名： ShowDebugInfo
功    能： 打印Debug文件的信息
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/9/05   4.0			顾振华                创建
2006/12/20  4.0          王亮                 增加E164号码段输出
2007/01/26  4.0			周广程				  增加读Licnese错误提示输出	
=============================================================================*/
void CMcuVcData::ShowDebugInfo()
{
    m_tMcuDebugVal.Print();

    OspPrintf(TRUE, FALSE, "\n");

#ifdef _MINIMCU_
    TMcu8kbPfmFilter tFilter = CMcuPfmLmt::GetFilter();
    tFilter.Print();
#endif

    OspPrintf(TRUE, FALSE, "\nMt Call Interface Table: %d\n", m_dwMtCallInterfaceNum);
    if ( m_dwMtCallInterfaceNum > 0 )
    {
        OspPrintf(TRUE, FALSE, "IP Start(E164)\tIP End\t\tMTAdp\t\tMP\n");
        for (u32 nLoop = 0; nLoop < m_dwMtCallInterfaceNum; nLoop ++)
        {
            if (m_atMtCallInterface[nLoop].m_byAliasType == mtAliasTypeTransportAddress)
            {
                OspPrintf(TRUE, FALSE, "0x%x\t0x%x\t0x%x\t0x%x\n",
                    ntohl(m_atMtCallInterface[nLoop].m_tIpSeg.dwIpStart),
                    ntohl(m_atMtCallInterface[nLoop].m_tIpSeg.dwIpEnd),
                    (m_atMtCallInterface[nLoop].m_dwMtadpIpAddr),
                    (m_atMtCallInterface[nLoop].m_dwMpIpAddr));
            }
            else if  (m_atMtCallInterface[nLoop].m_byAliasType == mtAliasTypeE164)
            {
                OspPrintf(TRUE, FALSE, "%s\t\t\t0x%x\t0x%x\n",
                    (m_atMtCallInterface[nLoop].m_szE164),
                    (m_atMtCallInterface[nLoop].m_dwMtadpIpAddr),
                    (m_atMtCallInterface[nLoop].m_dwMpIpAddr));
            }
            else
            {
                OspPrintf(TRUE, FALSE, "Unknown Alias: %s\t%s\n",
                    (m_atMtCallInterface[nLoop].m_szE164) );
            }
        }               
    }
    OspPrintf(TRUE, FALSE, "\n\n");

    TMcuStatus tStatus;
    GetMcuCurStatus(tStatus);
    tStatus.Print();   
}

/*=============================================================================
    函 数 名： GetMcuCurStatus
    功    能： 获取当前MCU状态
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/31  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetMcuCurStatus( TMcuStatus &tMcuStatus )
{
	memset( &tMcuStatus, 0 ,sizeof(TMcuStatus) );
	tMcuStatus.SetMcu( LOCAL_MCUID );
	u32 dwEqpIP = 0;
	u8  byEqpType = 0;
	u8  byEqpId = 1;
    
	//外设状态(包括离线外设)
	while (byEqpId <= MAXNUM_MCU_PERIEQP)
	{
		if (IsPeriEqpValid(byEqpId))
		{
			g_cMcuAgent.GetPeriInfo(byEqpId, &dwEqpIP, &byEqpType);
			tMcuStatus.m_atPeriEqp[tMcuStatus.m_byPeriEqpNum] = GetEqp(byEqpId);
			tMcuStatus.m_byEqpOnline[tMcuStatus.m_byPeriEqpNum]  = IsPeriEqpConnected(byEqpId);
            tMcuStatus.m_dwPeriEqpIpAddr[tMcuStatus.m_byPeriEqpNum] = GetEqpIp(byEqpId);
            
			tMcuStatus.m_byPeriEqpNum++;
		}
		byEqpId++;
	}

    u32 dwDcsIP = 0;
    u8  byDcsType = 0;
    u8  byDcsId = 1;

    //DCS状态(包括离线DCS)
    while (byDcsId <= MAXNUM_MCU_DCS) 
    {
        if (IsPeriDcsValid(byDcsId)) 
        {
            TPeriDcsStatus tDcsStatus;
            tMcuStatus.m_atPeriDcs[tMcuStatus.m_byPeriDcsNum] = GetDcs(byDcsId);
            tMcuStatus.m_byDcsOnline[tMcuStatus.m_byPeriDcsNum] = IsPeriDcsConnected(byDcsId);
            //目前只支持一个，后续需 MCU代理扩展
            tMcuStatus.m_dwPeriDcsIpAddr[tMcuStatus.m_byPeriDcsNum] = g_cMcuAgent.GetDcsIp();
            tMcuStatus.m_byPeriDcsNum ++ ;
        }
        byDcsId ++ ;
    }

	//即时会议个数, 预约会议个数, 所有会议与会终端总数
	CApp* pcApp = &g_cMcuVcApp;
	CMcuVcInst* pcVcInst = NULL;
	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_CONF; byInstID++)
	{
		pcVcInst = (CMcuVcInst *)pcApp->GetInstance(byInstID);
		if (NULL != pcVcInst)
		{            
            if ( pcVcInst->CurState() == CMcuVcInst::STATE_ONGOING )
			{
				tMcuStatus.m_byOngoingConfNum++;
				tMcuStatus.m_wAllJoinedMtNum += pcVcInst->m_tConfAllMtInfo.GetLocalJoinedMtNum();
			}
            if ( pcVcInst->CurState() == CMcuVcInst::STATE_SCHEDULED )
			{
				tMcuStatus.m_byScheduleConfNum++;
			}
		}
	}
	tMcuStatus.m_wAllJoinedMtNum = htons(tMcuStatus.m_wAllJoinedMtNum);

    // 顾振华 [5/29/2006] 增加MCU注册GK的情况
    tMcuStatus.m_byRegedGk = GetConfRegState(0);

    if ( g_cMSSsnApp.IsDoubleLink() )
    {
        tMcuStatus.m_tMSSynState.SetEntityType(TMSSynState::emNone);
        if ( g_cMSSsnApp.IsRemoteMpcConnected() )
        {
            if ( !g_cMSSsnApp.IsMSSynOK() ) 
            {
                tMcuStatus.m_tMSSynState = g_cMSSsnApp.GetCurMSSynState();
            }
        }
        else
        {
            // guzh [9/25/2006] 标记为未连接
            tMcuStatus.m_tMSSynState.SetEntityType(TMSSynState::emMpc);
        }
    }
	
    // License 数量
	tMcuStatus.m_wLicenseNum = htons( GetLicenseNum() );

    // 在线的Mp和MtAdp
    tMcuStatus.m_byRegedMpNum = GetMpNum();
    tMcuStatus.m_byRegedMtAdpNum = GetMtAdpNum(PROTOCOL_TYPE_H323);

    //N+1备份数据同步状态
    tMcuStatus.m_dwNPlusReplacedMcuIp = 0;
    switch (g_cNPlusApp.GetLocalNPlusState())
    {
    case MCU_NPLUS_IDLE:
        tMcuStatus.m_byNPlusState = NPLUS_NONE;
        break;
    case MCU_NPLUS_MASTER_IDLE:
        tMcuStatus.m_byNPlusState = NPLUS_BAKCLIENT_IDLE;
        break;
    case MCU_NPLUS_MASTER_CONNECTED:
        tMcuStatus.m_byNPlusState = NPLUS_BAKCLIENT_CONNECT;
        break;
    case MCU_NPLUS_SLAVE_IDLE:
        tMcuStatus.m_byNPlusState = NPLUS_BAKSERVER_IDLE;
        break;
    case MCU_NPLUS_SLAVE_SWITCH:
        tMcuStatus.m_byNPlusState = NPLUS_BAKSERVER_SWITCHED;
        if (!g_cNPlusApp.GetNPlusSynOk())
        {
            tMcuStatus.m_byNPlusState = NPLUS_BAKSERVER_SWITCHERR;
        }
        tMcuStatus.m_dwNPlusReplacedMcuIp = htonl(g_cNPlusApp.GetMcuSwitchedIp());
        break;
    default:
        tMcuStatus.m_byNPlusState = NPLUS_NONE;
        break;
    }
	
	//MCU是否被配置过的标识 zgc [12/21/2006] for支持MCU导航配置需求
	tMcuStatus.m_byMcuIsConfiged = g_cMcuAgent.IsMcuConfiged() ? 1 : 0;
	
	// 是否有MP zgc [07/25/2007]
	if (tMcuStatus.m_byRegedMpNum > 0)
	{
		tMcuStatus.SetIsExistMp( TRUE );
	}
	else
	{
		tMcuStatus.SetIsExistMp( FALSE );
	}
	
	// 是否有mtadp zgc [07/25/2007]
	if ( tMcuStatus.m_byRegedMtAdpNum > 0 )
	{ 
		tMcuStatus.SetIsExistMtadp( TRUE );
	}
	else
	{
		tMcuStatus.SetIsExistMtadp( FALSE );
	}
	
	// mcu配置信息可信级别  zgc [07/25/2007]
	tMcuStatus.m_byMcuCfgLevel = g_cMcuAgent.GetMcuCfgInfoLevel();

#ifdef _MINIMCU_
    // 考虑兼容老版本，三种DSC都设置IsExistDSC, 具体区分由会控侧处理, zgc, 2008-06-27
	u8 byDSCType = BRD_TYPE_UNKNOW;
	if ( g_cMcuAgent.IsDscReged(byDSCType) )
	{
		switch(byDSCType)
		{
		case BRD_TYPE_DSC:
			tMcuStatus.SetIsExistDSC( TRUE );
			break;
		case BRD_TYPE_MDSC:
            tMcuStatus.SetIsExistDSC( TRUE );
			tMcuStatus.SetIsExistMDSC( TRUE );
			break;
		case BRD_TYPE_HDSC:
            tMcuStatus.SetIsExistDSC( TRUE );
			tMcuStatus.SetIsExistHDSC( TRUE );
			break;
		default:
			OspPrintf( TRUE, FALSE, "[GetCurMcuStatus] unexpected reged dsc type.%d\n", byDSCType );
			break;
		}
	}
#endif

    //设置MCU状态位 zgc [07/25/2007]
    // guzh [8/2/2007] 目前的标准是有License，且MP/MtAdp 均在线
    BOOL32 bRunOk = tMcuStatus.IsExistMp() && 
                  tMcuStatus.IsExistMtadp() && 
                  (ntohs(tMcuStatus.m_wLicenseNum) > 0);
#ifdef _MINIMCU_		
#ifndef WIN32
    // 如果是 MINIMCU ，在非启用内置接入和转发的情况下（即商用环境下），则必须有注册的DSC模块
    if ( !g_cMcuAgent.GetIsUseMpcTranData() && !g_cMcuAgent.GetIsUseMpcStack() )
    {
        bRunOk &= tMcuStatus.IsExistDSC();
    }	
#endif
#endif
    tMcuStatus.SetIsMcuRunOk( bRunOk );

    //zbq[12/13/2007] MCU持续运行时间获取
    tMcuStatus.SetPersistantRunningTime(OspTickGet()/OspClkRateGet());

	// xliang [11/20/2008] HDI 接入能力
#ifndef _MINIMCU_
	u16 wAllHdiAccessMtNum = 0;
	for(u8 byHdiId=1; byHdiId <= MAXNUM_DRI; byHdiId++)
	{
		if(!IsMtAdpConnected(byHdiId) || 
			byHdiId == MCU_BOARD_MPC  ||
			byHdiId == MCU_BOARD_MPCD ||
            !g_cMcuVcApp.m_atMtAdpData[byHdiId-1].m_byIsSupportHD)
		{
			continue;
		}
		wAllHdiAccessMtNum += m_atMtAdpData[byHdiId-1].m_byMaxMtNum;
	}
	tMcuStatus.m_wAllHdiAccessMtNum = wAllHdiAccessMtNum;
#else
	tMcuStatus.m_wAllHdiAccessMtNum = ~0;  //用65535表示:  8000b，不用HDI
#endif

	// xliang [11/20/2008] CRI(标清接入板)接入能力 (暂不支持，预留)
	tMcuStatus.m_wStdCriAccessMtNum = ~0;

    if ( g_bPrintCfgMsg )
    {
        tMcuStatus.Print();
    }

	return TRUE;
}

/*=============================================================================
    函 数 名： GetMcuCurUserList
    功    能： 获取当前MCU用户列表
    算法实现： 
    全局变量： 
    参    数： [IN]  CUsrManage& cUsrManageObj, 操作的用户管理对象
			   [IN]  u8 byGrpId,                用户组ID
               [OUT] u8 *pbyBuf,                包缓冲
	           [IN/OUT] u8 &byUsrItr,           数组访问指针
	           [IN/OUT] u8 &byUserNumInPack     此包中的有效用户数

    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/31  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetMcuCurUserList(CUsrManage& cUsrManageObj, u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack )
{    
    //若为N+1备份工作模式（即切换后），取内存中的信息
    if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
    {
        u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
        if (0 == byInsId || byInsId > MAXNUM_NPLUS_MCU)
        {
            OspPrintf(TRUE, FALSE, "[GetMcuCurUserList] McuSwitchedInsId is invalid in NPlus switched mode.\n");
            return FALSE;
        }
        else
        {
            CApp *pcApp = &g_cNPlusApp;
            CNPlusInst *pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL != pcInst)
            {
                return pcInst->GetGrpUserList(byGrpId, pbyBuf, byUsrItr, byUserNumInPack);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[GetMcuCurUserList] GetInstance(%d) failed.\n", byInsId);
                return FALSE;
            }
        }                
    }

    byUserNumInPack = 0;
	if (NULL == pbyBuf)
	{
		return FALSE;
	}

	CExUsrInfo cUserInfo;

    s32 nTotalCount = cUsrManageObj.GetUserNum();
	for (; byUserNumInPack < USERNUM_PERPKT && byUsrItr < nTotalCount; byUsrItr ++ )
	{
		cUserInfo.Empty();
		if ( cUsrManageObj.GetUserFullInfo(&cUserInfo, byUsrItr) && 
			 !cUserInfo.IsEmpty() )
		{

            if ( byGrpId == USRGRPID_SADMIN ||
                 byGrpId == cUserInfo.GetUsrGrpId()  )
            {
			    memcpy(pbyBuf + byUserNumInPack * sizeof(CExUsrInfo), &cUserInfo, sizeof(CExUsrInfo));
			    byUserNumInPack++;
            }
		}
	}

	if (0 == byUserNumInPack)
	{
		return FALSE;
	}
	
	return TRUE;
}
/*=============================================================================
    函 数 名： GetMCSCurUserList
    功    能： 获取当前MCS用户列表
    算法实现： 
    全局变量： 
    参    数：
			   [IN]  u8 byGrpId,                用户组ID
               [OUT] u8 *pbyBuf,                包缓冲
	           [IN/OUT] u8 &byUsrItr,           数组访问指针
	           [IN/OUT] u8 &byUserNumInPack     此包中的有效用户数

    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    08/11/21     		    付秀华                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetMCSCurUserList(u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack )
{
	return GetMcuCurUserList(g_cUsrManage, byGrpId, pbyBuf, byUsrItr, byUserNumInPack);
}

/*=============================================================================
    函 数 名： GetVCSCurUserList
    功    能： 获取当前VCS用户列表
    算法实现： 
    全局变量： 
    参    数：
			   [IN]  u8 byGrpId,                用户组ID
               [OUT] u8 *pbyBuf,                包缓冲
	           [IN/OUT] u8 &byUsrItr,           数组访问指针
	           [IN/OUT] u8 &byUserNumInPack     此包中的有效用户数

    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    08/11/21     		    付秀华                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetVCSCurUserList(u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack )
{
	return GetMcuCurUserList(g_cVCSUsrManage, byGrpId, pbyBuf, byUsrItr, byUserNumInPack);
}

/*=============================================================================
  函 数 名： CreateTemplate
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::CreateTemplate(void)
{
    if(NULL == m_ptTemplateInfo)
    {
        m_ptTemplateInfo = new TTemplateInfo[MAXNUM_MCU_TEMPLATE];
        if(NULL == m_ptTemplateInfo)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*=============================================================================
  函 数 名： AddTemplate
  功    能： 
  算法实现： 
  全局变量： 
  参    数： [IN/OUT]       TTemplateInfo &tTemInfo
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::AddTemplate(TTemplateInfo &tTemInfo)
{
    // guzh [4/11/2007] 检查是否从文件恢复
    BOOL32 bTemplateFromFile =  tTemInfo.m_tConfInfo.m_tStatus.IsTakeFromFile();

    //zbq [09/06/2007] 本字段在SaveConfToFile里统一处理
    //tTemInfo.m_tConfInfo.m_tStatus.SetTakeFromFile(FALSE);

	u8 byIndex = MAXNUM_MCU_TEMPLATE;
	u8 byVCSTempNum = 0;
	u8 byMCSTempNum = 0;
    for(u8 byInd = 0; byInd < MAXNUM_MCU_TEMPLATE; byInd++)
    {

        if(m_ptTemplateInfo[byInd].IsEmpty())
        {
			if (MAXNUM_MCU_TEMPLATE == byIndex)
			{
				byIndex = byInd;
			}
        }
		else
		{
            if (VCS_CONF == m_ptTemplateInfo[byInd].m_tConfInfo.GetConfSource())
            {
				byVCSTempNum++;
            }
			else if (MCS_CONF == m_ptTemplateInfo[byInd].m_tConfInfo.GetConfSource())
			{
				byMCSTempNum++;
			}
		}
    }	

	if (VCS_CONF == tTemInfo.m_tConfInfo.GetConfSource() && byVCSTempNum >= MAXNUM_MCU_VCSTEMPLATE )
	{
		return FALSE;
	}
	else if (MCS_CONF == tTemInfo.m_tConfInfo.GetConfSource() && byMCSTempNum >= MAXNUM_MCU_MCSTEMPLATE )
	{
		return FALSE;
	}

    //template array full
    if(MAXNUM_MCU_TEMPLATE == byIndex)
    {
        return FALSE;
    }

    u8 byConfIdx = GetOngoingConfIdxByE164(tTemInfo.m_tConfInfo.GetConfE164());
    if(0 == byConfIdx)
    {
        byConfIdx = GetIdleConfidx();
        if(0 == byConfIdx)
        {
            return FALSE;
        }
    }    

    //save template info
    if(CONF_LOCKMODE_LOCK == tTemInfo.m_tConfInfo.m_tStatus.GetProtectMode())
    {
        tTemInfo.m_tConfInfo.m_tStatus.SetProtectMode(CONF_LOCKMODE_NONE);
    }    
    tTemInfo.m_tConfInfo.m_tStatus.SetRegToGK(FALSE);
    // guzh [4/11/2007] 如果从文件恢复,则沿用原来的ConfId

    if (!bTemplateFromFile)    
    {
		// 会议ID中保留发起者如MCS设置的配置源信息
		CConfId cConfID = MakeConfId(byConfIdx, 1, tTemInfo.m_tConfInfo.GetUsrGrpId(),
			                         tTemInfo.m_tConfInfo.GetConfSource());
        tTemInfo.m_tConfInfo.SetConfId(cConfID);
    }
    
    tTemInfo.m_byConfIdx = byConfIdx;
    memcpy(&m_ptTemplateInfo[byIndex], &tTemInfo, sizeof(tTemInfo));

    //save map info
    m_atConfMapData[byConfIdx-MIN_CONFIDX].SetTemIndex(byIndex);    

    //save to file
    SaveConfToFile(byConfIdx, TRUE);

    //reg to gk
    RegisterConfToGK(byConfIdx, GetRegGKDriId(), TRUE);

    return TRUE;
}

/*=============================================================================
  函 数 名： ModifyTemplate
  功    能： 
  算法实现： 
  全局变量： 
  参    数： TTemplateInfo &tTemInfo
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::ModifyTemplate(TTemplateInfo &tTemInfo, BOOL32 bSameE164AndName)
{
    if(tTemInfo.m_byConfIdx < MIN_CONFIDX || tTemInfo.m_byConfIdx > MAX_CONFIDX)
    {
        return FALSE;
    }
  
//     if(bSameE164AndName)
//     {
        if(CONF_LOCKMODE_LOCK == tTemInfo.m_tConfInfo.m_tStatus.GetProtectMode())
        {
            tTemInfo.m_tConfInfo.m_tStatus.SetProtectMode(CONF_LOCKMODE_NONE);       
        }        
        tTemInfo.m_tConfInfo.m_tStatus.SetTemplate();

		u8 byConfIdx = tTemInfo.m_byConfIdx;
        TConfMapData tMapData = GetConfMapData(byConfIdx);
        if(!tMapData.IsTemUsed())
        {
            return FALSE;
        }

		// 对于164号改变的会议模板，还需要完成原模板在GK上的解注册及新模板的注册
		// 而不采取删除会议再重新添加的策略，没必要，且导致confid改变影响用户任务信息
		if (bSameE164AndName)
		{
			// 解注册
			CMcuVcInst *pcInst = GetConfInstHandle(byConfIdx);
			if (!m_atConfMapData[byConfIdx-MIN_CONFIDX].IsValidConf() || 
			   (NULL != pcInst && pcInst->m_tConf.m_tStatus.IsScheduled()))
			{
				if ( 0 != g_cMcuVcApp.GetConfRegState( byConfIdx ) )
				{
					RegisterConfToGK(byConfIdx, GetRegGKDriId(), TRUE, TRUE);
				}
			} 
			
		}

		// 新模板注册
		tTemInfo.m_tConfInfo.m_tStatus.SetRegToGK(FALSE);
        memcpy(&m_ptTemplateInfo[tMapData.GetTemIndex()], &tTemInfo, sizeof(tTemInfo));
		RegisterConfToGK(byConfIdx, GetRegGKDriId(), TRUE);
			
        //save to file
        SaveConfToFile(byConfIdx, TRUE);
//     }
//     else
//     {
//         if(!DelTemplate(tTemInfo.m_byConfIdx))
//         {
//             return FALSE;
//         }
//         
//         if(!AddTemplate(tTemInfo))
//         {
//             return FALSE;
//         }   
//     }    

    return TRUE;
}

/*=============================================================================
  函 数 名： DelTemplate
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::DelTemplate(u8 byConfIdx)
{
    if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return FALSE;
    }

    //clear template info
    u8 byIndex = m_atConfMapData[byConfIdx-MIN_CONFIDX].GetTemIndex();
    if(byIndex >= MAXNUM_MCU_TEMPLATE) 
    {
        return FALSE;
    }
            
    //unreg to gk
    CMcuVcInst *pcInst = GetConfInstHandle(byConfIdx);
    if (!m_atConfMapData[byConfIdx-MIN_CONFIDX].IsValidConf() || 
       (NULL != pcInst && pcInst->m_tConf.m_tStatus.IsScheduled()))
    {
        // zbq [03/30/2007] 模板未注册成功，不做注销
        if ( 0 != g_cMcuVcApp.GetConfRegState( byConfIdx ) )
        {
            RegisterConfToGK(byConfIdx, GetRegGKDriId(), TRUE, TRUE);
        }
    }    
    
    //remove to file
    if(!RemoveConfFromFile(m_ptTemplateInfo[byIndex].m_tConfInfo.GetConfId()))
    {
        OspPrintf(TRUE, FALSE, "[DelTemplate] Remove Conf From File failed!\n");
    }

    m_ptTemplateInfo[byIndex].Clear();

    //clear map info
    m_atConfMapData[byConfIdx-MIN_CONFIDX].SetTemIndex(MAXNUM_MCU_TEMPLATE); 

    return TRUE;
}

/*=============================================================================
  函 数 名： GetTemplate
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
             TTemplateInfo &tTemInfo
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::GetTemplate(u8 byConfIdx, TTemplateInfo &tTemInfo)
{
    if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return FALSE;
    }

    u8 byIndex = m_atConfMapData[byConfIdx-MIN_CONFIDX].GetTemIndex();
    if(byIndex >= MAXNUM_MCU_TEMPLATE) 
    {
        return FALSE;
    }
    memcpy(&tTemInfo, &m_ptTemplateInfo[byIndex], sizeof(TTemplateInfo));

    return TRUE;
}

u32 CMcuVcData::GetExistSatCastIp()
{
    u32 dwSatCastIp = 0xffffffff;

    for (u8 byConfIdx = 1; byConfIdx <= MAX_CONFIDX; byConfIdx++)
    {
        if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
        {
            continue;
        }
        
        u8 byIndex = m_atConfMapData[byConfIdx-MIN_CONFIDX].GetTemIndex();
        if(byIndex >= MAXNUM_MCU_TEMPLATE) 
        {
            continue;
        }
        dwSatCastIp = m_ptTemplateInfo[byIndex].m_tConfInfo.GetConfAttrb().GetSatDCastIp();
        if (dwSatCastIp == 0)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    return dwSatCastIp;
}

u16 CMcuVcData::GetExistSatCastPort()
{
    u16 wSatCastPort = 0xffff;
    
    for (u8 byConfIdx = 1; byConfIdx <= MAX_CONFIDX; byConfIdx++)
    {
        if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
        {
            continue;
        }
        
        u8 byIndex = m_atConfMapData[byConfIdx-MIN_CONFIDX].GetTemIndex();
        if(byIndex >= MAXNUM_MCU_TEMPLATE) 
        {
            continue;
        }
        wSatCastPort = m_ptTemplateInfo[byIndex].m_tConfInfo.GetConfAttrb().GetSatDCastPort();
        if (wSatCastPort == 0)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    return wSatCastPort;
}

/*=============================================================================
  函 数 名： SetConfMapInsId
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
             u8 byInsId
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::SetConfMapInsId(u8 byConfIdx, u8 byInsId)
{
    if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return FALSE;
    }

    m_atConfMapData[byConfIdx-MIN_CONFIDX].SetInsId(byInsId);

    return TRUE;
}

/*=============================================================================
  函 数 名： GetConfMapInsId
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
  返 回 值： u8  
=============================================================================*/
u8  CMcuVcData::GetConfMapInsId(u8 byConfIdx)
{
    if(byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX)
    {
        return 0;
    }

    return m_atConfMapData[byConfIdx-MIN_CONFIDX].GetInsId();
}

/*=============================================================================
  函 数 名： GetConfMapData
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
  返 回 值： TConfMapData  
=============================================================================*/
TConfMapData  CMcuVcData::GetConfMapData(u8 byConfIdx)
{
    return m_atConfMapData[byConfIdx-MIN_CONFIDX];
}

/*=============================================================================
  函 数 名： SetTemRegGK
  功    能： 设置模板注册gk状态
  算法实现： 
  全局变量： 
  参    数： BOOL32 bReg
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::SetTemRegGK(u8 byConfIdx, BOOL32 bReg)
{
    CHECK_CONFIDX(byConfIdx)

    u8 byIndex = m_atConfMapData[byConfIdx-MIN_CONFIDX].GetTemIndex();
    if(byIndex >= MAXNUM_MCU_TEMPLATE) 
    {
        return FALSE;
    }
    
    m_ptTemplateInfo[byIndex].m_tConfInfo.m_tStatus.SetRegToGK(bReg);
    return TRUE;
}

/*=============================================================================
  函 数 名： GetTemRegGK
  功    能： 模板注册gk状态
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::IsTemRegGK(u8 byConfIdx)
{
    CHECK_CONFIDX(byConfIdx)

    u8 byIndex = m_atConfMapData[byConfIdx-MIN_CONFIDX].GetTemIndex();
    if(byIndex >= MAXNUM_MCU_TEMPLATE) 
    {
        return FALSE;
    } 

    return m_ptTemplateInfo[byIndex].m_tConfInfo.m_tStatus.IsRegToGK();
}

/*=============================================================================
  函 数 名： GetIdleConfidx
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： u8  
=============================================================================*/
u8    CMcuVcData::GetIdleConfidx(void)
{    
    for(u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
    {
        if (!m_atConfMapData[byConfIdx-MIN_CONFIDX].IsValidConf() && 
            !m_atConfMapData[byConfIdx-MIN_CONFIDX].IsTemUsed())
        {
            return byConfIdx;
        }
    }

    return 0;
}

/*=============================================================================
  函 数 名： SetLicenseNum
  功    能： 设置License
  算法实现： 
  全局变量： 
  参    数： u16 wLicenseValue
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::SetLicenseNum( u16 wLicenseValue )
{
	m_tMcuDebugVal.m_tPerfLimit.m_wMaxConnMtNum = wLicenseValue;
	return TRUE;
}

/*=============================================================================
  函 数 名： GetLicenseNum
  功    能： 取License
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： u16 
=============================================================================*/
u16 CMcuVcData::GetLicenseNum( void )
{
	return GetPerfLimit().m_wMaxConnMtNum;
}

/*=============================================================================
  函 数 名： SetVCSAccessNum
  功    能： 设置License写入的VCS接入数
  算法实现： 
  全局变量： 
  参    数： u16 wAccessNum
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::SetVCSAccessNum( u16 wAccessNum )
{
	m_tMcuDebugVal.m_tPerfLimit.m_wMaxVCSAccessNum = wAccessNum;
	return TRUE;
}

/*=============================================================================
  函 数 名： GetVCSAccessNum
  功    能： 取License写入的VCS接入数
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： u16 
=============================================================================*/
u16 CMcuVcData::GetVCSAccessNum( void )
{
	return GetPerfLimit().m_wMaxVCSAccessNum;
}

/*=============================================================================
函 数 名： SetMcuExpireDate
功    能： 设置mcu license 超时使用日期
算法实现： 
全局变量： 
参    数： TKdvTime &tExpireDate
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/10/20  4.0			许世林                  创建
=============================================================================*/
void CMcuVcData::SetMcuExpireDate( TKdvTime &tExpireDate )
{
    m_tMcuDebugVal.m_tExpireDate = tExpireDate;
}

/*=============================================================================
函 数 名： GetMcuExpireDate
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： TKdvTime 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/10/20  4.0			许世林                  创建
=============================================================================*/
TKdvTime CMcuVcData::GetMcuExpireDate( void )
{
    return m_tMcuDebugVal.m_tExpireDate;
}

/*=============================================================================
函 数 名： IsMcuExpiredDate
功    能： mcu license是否超期
算法实现： 
全局变量： 
参    数： void
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/10/20  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMcuVcData::IsMcuExpiredDate(void)
{
    if (m_tMcuDebugVal.m_tExpireDate.GetYear() == 0)
    {
        return FALSE;
    }

    time_t tExpireDate;
    m_tMcuDebugVal.m_tExpireDate.GetTime(tExpireDate);

    return (time(NULL) > tExpireDate);
}


/*=============================================================================
  函 数 名： IsMtNumOverLicense
  功    能： 是否超过最大终端接入能力
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuVcData::IsMtNumOverLicense(void)
{
    u16 wMtNum = 0;
    for(u8 byIndex = 0; byIndex < MAXNUM_MCU_CONF; byIndex++)
    {
        CMcuVcInst *pcIns = m_apConfInst[byIndex];
        if(NULL != pcIns && pcIns->m_tConf.m_tStatus.IsOngoing())
        {
            wMtNum += pcIns->m_tConfAllMtInfo.GetLocalJoinedMtNum();
        }
    }

    return (wMtNum >= GetLicenseNum());
}

/*=============================================================================
  函 数 名： IsVCSNumOverLicense
  功    能： 是否超过最大VCS接入能力
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： BOOL32  
=============================================================================*/
BOOL32  CMcuVcData::IsVCSNumOverLicense(void)
{
    u16 wVCSAccessNum = 0;
	for (u16 wInstIdx = MAXNUM_MCU_MC + 1; wInstIdx <= (MAXNUM_MCU_VC + MAXNUM_MCU_MC); wInstIdx++)
	{
		if (g_cMcuVcApp.IsMcConnected(wInstIdx))
		{
			wVCSAccessNum++;
		}
	}
	
	CfgLog("[IsVCSNumOverLicense]current vcs access num:%d\n", wVCSAccessNum);

    return (wVCSAccessNum >= GetVCSAccessNum());
}

/*=============================================================================
  函 数 名： IsConfNumOverCap
  功    能： 会议数量是否超限
  算法实现： 
  全局变量： 
  参    数： bOngoing               - 欲创建的会议是即时会议
             byConfSource           - 指定发起方MCS_CONF、VCS_CONF、ALL_CONF
  返 回 值： BOOL32  
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/1/18     4.0			顾振华                  创建
=============================================================================*/
BOOL32  CMcuVcData::IsConfNumOverCap ( BOOL32 bOngoing, u8 byConfSource /*= MCS_CONF*/)
{
	// 检查MCU各发起方即时及预约会议是否超过了mcu会议能力
    u8 byAllConfNum = GetConfNum( TRUE, TRUE, FALSE, ALL_CONF );

    // 超最大会议能力
    if (byAllConfNum >= MAXNUM_MCU_CONF )
    {
        return TRUE;    
    }

    if ( bOngoing )
    {
        // 超最大即时会议能力 
		// 查看指定的发起方会议是否超过分配给各发起方的即时会议的能力
        u8 byConfNum = GetConfNum( TRUE, FALSE, FALSE, byConfSource );

		if (VCS_CONF == byConfSource && byConfNum >= MAXNUM_ONGO_VCSCONF)
		{
			return TRUE;
		}
		else if (MCS_CONF == byConfSource && byConfNum >= (MAXNUM_ONGO_CONF - MAXNUM_ONGO_VCSCONF))
		{
			return TRUE;
		}
		else if (ALL_CONF == byConfSource && byConfNum >= MAXNUM_ONGO_CONF)
		{
			return TRUE;
		}
    }

    return FALSE;
}

/*=============================================================================
  函 数 名： GetMcuCasConfNum
  功    能： 获取级联会议总数
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32  
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/01/19    4.0			顾振华                  创建
=============================================================================*/
u8  CMcuVcData::GetMcuCasConfNum ( )
{
    u8 byCasConfNum = 0;
    for(u8 byIndex = 0; byIndex < MAXNUM_MCU_CONF; byIndex++)
    {
        CMcuVcInst *pcIns = m_apConfInst[byIndex];
        if (NULL == pcIns)
            continue;
        if ( !pcIns->m_tConf.m_tStatus.IsOngoing() )
            continue;
        
        if ( pcIns->m_tConfAllMtInfo.GetCascadeMcuNum() > 0 )
        {
            byCasConfNum ++;
        }            
    }

    return byCasConfNum;
}

/*=============================================================================
  函 数 名： IsCasConfOverCap
  功    能： 级联会议数量是否超限
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32  
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/01/19    4.0			顾振华                  创建
=============================================================================*/
BOOL32  CMcuVcData::IsCasConfOverCap ( )
{
    return ( GetMcuCasConfNum() >= MAXLIMIT_MCU_CASCONF );
}

/*=============================================================================
  函 数 名： GetMtNumOnDri
  功    能： 获得某个DRI MtAdp上在线的终端或者Mcu数量，返回是否超限
  算法实现： 
  全局变量： 
  参    数： byDriId    [in]
             bOnilne    [in]    是否检测Online值
             byMtNum    [out]
             bySMcuNum  [out]
  返 回 值： BOOL32  是否超限(找不到、不在线等都认为超限)
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/1/18     4.0			顾振华                  创建
=============================================================================*/
BOOL32  CMcuVcData::GetMtNumOnDri( u8 byDriId, BOOL32 bOnline, u8 &byMtNum, u8 &byMcuNum )
{
    if ( byDriId == 0 || byDriId > MAXNUM_DRI )
        return TRUE;

    u8 byIdx = byDriId - 1;
    if ( !m_atMtAdpData[byIdx].m_bConnected || 
         PROTOCOL_TYPE_H323 != m_atMtAdpData[byIdx].m_byProtocolType ) 
    {
         return TRUE;
    }

    
    if (!bOnline)
    {
        // Mt数量有统计值
        byMtNum = (u8)m_atMtAdpData[byIdx].m_wMtNum;
    }    
    else
    {
        byMtNum = 0;
    }

    // 遍历计算SMcu数量
    byMcuNum = 0;
    for(u8 byIndex = 0; byIndex < MAXNUM_MCU_CONF; byIndex++)
    {
        CMcuVcInst *pcIns = m_apConfInst[byIndex];
        if (NULL == pcIns)
            continue;
        if ( !pcIns->m_tConf.m_tStatus.IsOngoing() )
            continue;

        if ( !pcIns->m_tConf.GetConfAttrb().IsSupportCascade() )
            continue;

        for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
        {
            if ( pcIns->m_ptMtTable->GetDriId(byMtId) == byDriId )
            {
                if ( bOnline && pcIns->m_tConfAllMtInfo.MtJoinedConf( byMtId ) )
                {
                    byMtNum ++;
                }
                if ( pcIns->m_tConfAllMtInfo.MtJoinedConf( byMtId ) &&
                     ( pcIns->m_ptMtTable->GetMtType( byMtId ) == MT_TYPE_SMCU ) )
                {
                    // MCU 永远检测online
                    byMcuNum ++;
                }
            }
        }
    }

    if ( byMtNum >= m_atMtAdpData[byIdx].m_byMaxMtNum || 
         byMcuNum >= m_atMtAdpData[byIdx].m_byMaxSMcuNum )
    {
         return TRUE;
    }

    return FALSE;
    
}


/*=============================================================================
  函 数 名： IsSavingBandwidth
  功    能： 是否节省带宽
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcData::IsSavingBandwidth(void)         
{
    TLocalInfo tLocalInfo;
    if(SUCCESS_AGENT != g_cMcuAgent.GetLocalInfo(&tLocalInfo))
    {
        return FALSE;
    }

    return (1 == tLocalInfo.GetIsSaveBand());
}

/*=============================================================================
  函 数 名： Msg2TemInfo
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg &cMsg
             TTemplateInfo &tTemInfo
  返 回 值： void  
=============================================================================*/
void CMcuVcData::Msg2TemInfo(CServMsg &cMsg, TTemplateInfo &tTemInfo, 
					    	 s8* *pszUnProcInfoHead /*= NULL*/, u16* pwUnProcLen /*= NULL*/)
{
    // guzh [5/17/2007] 先清空
    tTemInfo.Clear();

	TConfInfo *ptConfInfo = (TConfInfo *)cMsg.GetMsgBody();
	u16 wAliasBufLen = ntohs( *(u16*)(cMsg.GetMsgBody() + sizeof(TConfInfo)) );
	s8 *pszAliaseBuf = (s8*)(cMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16));
	s8 *pszModBuf = (s8*)(cMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16) + wAliasBufLen);

    u8 byMtNum = tTemInfo.m_byMtNum;
	UnPackTMtAliasArray(pszAliaseBuf, wAliasBufLen, ptConfInfo, tTemInfo.m_atMtAlias,
		                tTemInfo.m_awMtDialBitRate, byMtNum);
    tTemInfo.m_byMtNum = byMtNum;
	memcpy(&tTemInfo.m_tConfInfo, ptConfInfo, sizeof(TConfInfo));

	if (ptConfInfo->GetConfAttrb().IsHasTvWallModule())
	{
		TMultiTvWallModule *ptMultiTvWallModule = (TMultiTvWallModule *)pszModBuf;
		tTemInfo.m_tMultiTvWallModule = *ptMultiTvWallModule;
		pszModBuf += sizeof(TMultiTvWallModule);
	}
	if (ptConfInfo->GetConfAttrb().IsHasVmpModule())
	{
		memcpy(&tTemInfo.m_atVmpModule, pszModBuf, sizeof(TVmpModule));
		pszModBuf += sizeof(TVmpModule);
	}

	// 对于VCS创建的会议模板，包含所需要的额外信息 
	//                  + 1byte(u8: 0 1  是否配置了高清电视墙)
	//                  +(可选, THDTvWall)
	//                  + 1byte(u8: 会议配置HDU的通道总数)
	//                  + (可选, THduModChnlInfo+...)	
	//                  + 1byte(是否为级联调度)＋(可选，2byte[u16 网络序，配置打包的总长度]+下级mcu配置[1byte(终端类型)+1byte(别名长度)+xbyte(别名字符串)+2byte(呼叫码率)...)])
	//                  + 1byte(是否支持分组)＋(可选，2byte(u16 网络序，信息总长)+内容(1byte(组数)+n组[1TVCSGroupInfo＋m个TVCSEntryInfo])
	if (VCS_CONF == ptConfInfo->GetConfSource())
	{
		s8* pszVCSInfoEx = pszModBuf;
		// 配置了高清电视墙
		if (*pszVCSInfoEx++)
		{
			memcpy(&tTemInfo.m_tHDTWInfo, pszVCSInfoEx, sizeof(THDTvWall));
			pszVCSInfoEx += sizeof(THDTvWall);			
		}
		else
		{
			tTemInfo.m_tHDTWInfo.SetNull();
		}

		// 配置HDU
		tTemInfo.m_tHduModule.SetNull();
		u8 byChnlNum = *pszVCSInfoEx++;
		if (byChnlNum)
		{
			tTemInfo.m_tHduModule.SetHduModuleInfo(byChnlNum, pszVCSInfoEx);
			pszVCSInfoEx += byChnlNum * sizeof(THduModChnlInfo);
		}

		// 配置了下级mcu
		if (*pszVCSInfoEx++)
		{
			u16 wSMCUInfoLen = ntohs(*(u16*)pszVCSInfoEx);
			pszVCSInfoEx = (s8*)(pszVCSInfoEx + sizeof(u16));
			UnPackTMtAliasArray(pszVCSInfoEx, wSMCUInfoLen, &tTemInfo.m_tConfInfo,
								tTemInfo.m_tVCSSMCUCfg.m_atSMCUAlias, 
								tTemInfo.m_tVCSSMCUCfg.m_awSMCUDialBitRate, 
								tTemInfo.m_tVCSSMCUCfg.m_wSMCUNum);
			pszVCSInfoEx = (s8*)(pszVCSInfoEx + wSMCUInfoLen);
		}
		else
		{
			tTemInfo.m_tVCSSMCUCfg.SetNull();
		}

		// 配置了分组
		if (*pszVCSInfoEx++)
		{
			tTemInfo.m_byMTPackExist = TRUE;
			if (pszUnProcInfoHead != NULL)
			{
				*pszUnProcInfoHead = pszVCSInfoEx;
				*pwUnProcLen = ntohs(*(u16*)pszVCSInfoEx) + sizeof(u16);
			}
	 

		}	
	}

	return;
}

/*=============================================================================
  函 数 名： TemInfo2Msg
  功    能： 
  算法实现： 
  全局变量： 
  参    数： TTemplateInfo &tTemInfo
             CServMsg &cMsg
  返 回 值： void  
=============================================================================*/
void CMcuVcData::TemInfo2Msg(TTemplateInfo &tTemInfo, CServMsg &cMsg)
{
	s8  achAliasBuf[SERV_MSG_LEN];
	u16 wAliasBufLen = 0;

	cMsg.SetMsgBody( (u8*)&tTemInfo.m_tConfInfo, sizeof(TConfInfo) );

	//终端别名数组打包 
	PackTMtAliasArray(tTemInfo.m_atMtAlias, tTemInfo.m_awMtDialBitRate, 
		              tTemInfo.m_byMtNum, achAliasBuf, wAliasBufLen);
	wAliasBufLen = htons(wAliasBufLen);
	cMsg.CatMsgBody((u8*)&wAliasBufLen, sizeof(wAliasBufLen));
	cMsg.CatMsgBody((u8*)achAliasBuf, ntohs(wAliasBufLen));

	//电视墙模板
	if( tTemInfo.m_tConfInfo.GetConfAttrb().IsHasTvWallModule() )
	{			
		cMsg.CatMsgBody( (u8*)&tTemInfo.m_tMultiTvWallModule, sizeof(TMultiTvWallModule) );
	}
	//画面合成模板 
	if( tTemInfo.m_tConfInfo.GetConfAttrb().IsHasVmpModule() )
	{			
		cMsg.CatMsgBody( (u8*)&tTemInfo.m_atVmpModule, sizeof(TVmpModule) );
	}
	// 对于VCS创建的会议模板，包含所需要的额外信息 
	//                  + 1byte(u8: 0 1  是否配置了高清电视墙)
	//                  +(可选, THDTvWall)
	//                  + 1byte(u8: 会议配置HDU的通道总数)
	//                  + (可选, THduModChnlInfo+...)
	//                  + 1byte(是否为级联调度)＋(可选，2byte[u16 网络序，配置打包的总长度]+下级mcu配置[1byte(终端类型)+1byte(别名长度)+xbyte(别名字符串)+2byte(呼叫码率)...)])
	//                  + 1byte(是否支持分组)＋(可选，2byte(u16 网络序，信息总长)+内容(1byte(组数)+n组[1TVCSGroupInfo＋m个TVCSEntryInfo])
	if (VCS_CONF == tTemInfo.m_tConfInfo.GetConfSource())
	{
		u8 byIsHDTWCfg = (u8)tTemInfo.IsHDTWCfg();
		cMsg.CatMsgBody(&byIsHDTWCfg, sizeof(u8));
		// 配置了高清电视墙
		if (byIsHDTWCfg)
		{
			cMsg.CatMsgBody((u8*)&tTemInfo.m_tHDTWInfo, sizeof(THDTvWall));
		}
		// 配置了Hdu
		u8 byHduChnlNum = tTemInfo.m_tHduModule.GetHduChnlNum();
		cMsg.CatMsgBody(&byHduChnlNum, sizeof(u8));
		if (byHduChnlNum)
		{
			cMsg.CatMsgBody(tTemInfo.m_tHduModule.GetHduModuleInfo(), byHduChnlNum * sizeof(THduModChnlInfo));
		}

		// 配置了下级mcu
		u8 byIsVCSSMCUCfg = (u8)tTemInfo.IsVCSMCUCfg();
		cMsg.CatMsgBody(&byIsVCSSMCUCfg, sizeof(u8));
		if (byIsVCSSMCUCfg)
		{
			PackTMtAliasArray(tTemInfo.m_tVCSSMCUCfg.m_atSMCUAlias, tTemInfo.m_tVCSSMCUCfg.m_awSMCUDialBitRate,
							  tTemInfo.m_tVCSSMCUCfg.m_wSMCUNum, achAliasBuf, wAliasBufLen);
			wAliasBufLen = htons(wAliasBufLen);
			cMsg.CatMsgBody((u8*)&wAliasBufLen, sizeof(u16));
			cMsg.CatMsgBody((u8*)achAliasBuf, ntohs(wAliasBufLen));
		}
		// 配置了分组		
		if (tTemInfo.m_byMTPackExist)
		{
			u8 achMTPackInfo[SERV_MSG_LEN - SERV_MSGHEAD_LEN];  //存放分组信息的缓存
			memset(achMTPackInfo, 0, sizeof(achMTPackInfo));
			
			CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
			u8      byConfPos  = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;

			// 在头信息记录中 获取 会议或模板的相对位置
			GetAllConfHeadFromFile(acConfId, sizeof(acConfId));

			//查找已有会议：此会议已保存，覆盖，不包含缺省会议位置
			for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
			{
				if (acConfId[nPos] == tTemInfo.m_tConfInfo.GetConfId())
				{
					byConfPos = (u8)nPos;
					break;
				}
			}
			//一般走不到，该模板不存在未存于文件中
			if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE == byConfPos)
			{
				OspPrintf(TRUE, FALSE, "[TemInfo2Msg] confinfo_head.dat has no temp with specified confid\n");
			}
			else
			{
				if (GetUnProConfDataToFile(byConfPos, (s8*)achMTPackInfo, wAliasBufLen))
				{
					// 成功获取对应分组
					cMsg.CatMsgBody((u8*)&tTemInfo.m_byMTPackExist, sizeof(u8));
					cMsg.CatMsgBody(achMTPackInfo, wAliasBufLen);
				}
				else
				{
					// 文件可能被删除
					tTemInfo.m_byMTPackExist =  FALSE;
					cMsg.CatMsgBody((u8*)&tTemInfo.m_byMTPackExist, sizeof(u8));
				}

			}
		}
		else
		{
			cMsg.CatMsgBody((u8*)&tTemInfo.m_byMTPackExist, sizeof(u8));

		}
			
	}

	cMsg.SetConfId(tTemInfo.m_tConfInfo.GetConfId());

	return;
}

/*=============================================================================
  函 数 名： ConfInfoMsgPack
  功    能： 会议信息消息打包
  算法实现： 
  全局变量： 
  参    数： CMcuVcInst *pcSchInst
             CServMsg &cServMsg
  返 回 值： void 
=============================================================================*/
void CMcuVcData::ConfInfoMsgPack(CMcuVcInst *pcSchInst, CServMsg &cServMsg)
{
    if(NULL == pcSchInst)
    {
        return;
    }

    u8  byLoop;
    TMt tMt;
    TMtAlias  tMtAlias;
    TConfInfo tNewConfInfo;
    TMtAlias  atMtAlias[MAXNUM_CONF_MT];
    u16       awMtDialBitRate[MAXNUM_CONF_MT];
    
    s8  achAliasBuf[SERV_MSG_LEN];
    u16 wAliasBufLen = 0;
    u8  byMtPos = 0;
    
    TMultiTvWallModule tMultiTvWallModule;
    pcSchInst->m_tConfEqpModule.GetMultiTvWallModule(tMultiTvWallModule);
    TVmpModule tVmpModule = pcSchInst->m_tConfEqpModule.GetVmpModule();   

    memset(tVmpModule.m_abyVmpMember, 0, sizeof(tVmpModule.m_abyVmpMember));
    
    //清除GK注册信息，即时模板本身注册成功
    tNewConfInfo = pcSchInst->m_tConf;
    tNewConfInfo.m_tStatus.SetRegToGK( FALSE );
    if( tNewConfInfo.m_tStatus.GetProtectMode() == CONF_LOCKMODE_LOCK )
    {
        tNewConfInfo.m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );
    }    
    
    //终端列表
    BOOL32 bExist = FALSE;
    u8 byMemberType = 0;
    for( byLoop = 0; byLoop < pcSchInst->m_ptMtTable->m_byMaxNumInUse; byLoop++)
    {
        tMt = pcSchInst->m_ptMtTable->GetMt(byLoop+1);
        if(!tMt.IsNull())
        {			
            //由于mtAliasTypeH320Alias类型不能用作呼叫信息，所以不加载
            if( pcSchInst->m_ptMtTable->GetMtAlias( (byLoop+1), mtAliasTypeH320ID, &tMtAlias ) )
            {
                atMtAlias[byLoop] = tMtAlias;
            }
            else if( pcSchInst->m_ptMtTable->GetMtAlias( (byLoop+1), mtAliasTypeTransportAddress, &tMtAlias ) )
            {
                atMtAlias[byLoop] = tMtAlias;			
            }
            else 
            {
                if( pcSchInst->m_ptMtTable->GetMtAlias( (byLoop+1), mtAliasTypeE164, &tMtAlias ) )
                {
                    atMtAlias[byLoop] = tMtAlias;			
                } 
                else 
                {
                    if( pcSchInst->m_ptMtTable->GetMtAlias( (byLoop+1), mtAliasTypeH323ID, &tMtAlias ) )
                    {
                        atMtAlias[byLoop] = tMtAlias;
                    }
                }
            }
            
            awMtDialBitRate[byLoop] = pcSchInst->m_ptMtTable->GetDialBitrate( (byLoop+1) );
            
            //更新模板映射关系
            if(tNewConfInfo.GetConfAttrb().IsHasTvWallModule())
            {
                for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL; byTvLp++)
                {
                    u8 byTvId = pcSchInst->m_tConfEqpModule.m_tTvWallInfo[byTvLp].m_tTvWallEqp.GetEqpId();
                    bExist = pcSchInst->m_tConfEqpModule.GetTvWallMemberByMt(byTvId, tMt, byMtPos, byMemberType);
                    if( bExist )
                    {
                        tMultiTvWallModule.SetTvWallMember(byTvId, byMtPos, byLoop+1, byMemberType);
                    }
                }
            }

            if(tNewConfInfo.GetConfAttrb().IsHasVmpModule())
            {
                for (u8 byVmpIdx = 0; byVmpIdx < MAXNUM_MPUSVMP_MEMBER; byVmpIdx++)
                {
                    if( pcSchInst->m_tConfEqpModule.IsMtAtVmpChannel(byVmpIdx, tMt, byMemberType) )
					{
						tVmpModule.SetVmpMember(byVmpIdx, byLoop+1, byMemberType);
					}
                }                
            }            
        }
    }   
    
    //终端别名数组打包 
    PackTMtAliasArray(atMtAlias, awMtDialBitRate, 
		              pcSchInst->m_ptMtTable->m_byMaxNumInUse, achAliasBuf, wAliasBufLen);	
    wAliasBufLen = htons(wAliasBufLen);
	cServMsg.SetMsgBody((u8 *)&tNewConfInfo, sizeof(TConfInfo));
    cServMsg.CatMsgBody((u8*)&wAliasBufLen, sizeof(wAliasBufLen));
    cServMsg.CatMsgBody((u8*)achAliasBuf, ntohs(wAliasBufLen));
    
    //电视墙模板
    if( tNewConfInfo.GetConfAttrb().IsHasTvWallModule() )
    {			
        cServMsg.CatMsgBody( (u8*)&tMultiTvWallModule, sizeof(TMultiTvWallModule) );
    }
    //画面合成模板 
    if( tNewConfInfo.GetConfAttrb().IsHasVmpModule() )
    {			
        cServMsg.CatMsgBody( (u8*)&tVmpModule, sizeof(TVmpModule) );
    }

    return;
}

/*=============================================================================
  函 数 名： ShowTemplate
  功    能： 打印模板基本信息
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void  
=============================================================================*/
void   CMcuVcData::ShowTemplate(void)
{
    s8 szInfo[255];
    OspPrintf(TRUE, FALSE, "\nconf template info as followed:\n");
    
    for(u8 byIndex = 0; byIndex < MAXNUM_MCU_TEMPLATE; byIndex++)
    {
        if(m_ptTemplateInfo[byIndex].IsEmpty())
        {
            continue;
        }

        TConfInfo *ptConfInfo = &m_ptTemplateInfo[byIndex].m_tConfInfo;
        ptConfInfo->GetConfId().GetConfIdString(szInfo, sizeof(szInfo));
        OspPrintf(TRUE, FALSE, "\nTemplate name: %s, E164: %s\n", 
                  ptConfInfo->GetConfName(), ptConfInfo->GetConfE164());
        OspPrintf(TRUE, FALSE, "temindex: %d, insid: %d confidx: %d, usrgrp: %d\n  confid: %s\n", 
                  byIndex, GetConfMapInsId(m_ptTemplateInfo[byIndex].m_byConfIdx), m_ptTemplateInfo[byIndex].m_byConfIdx, 
                  ptConfInfo->GetUsrGrpId(), szInfo);
        
        OspPrintf(TRUE, FALSE, "MTs(number %d) in template:\n", m_ptTemplateInfo[byIndex].m_byMtNum);
        for(u8 byMtIndex = 0; byMtIndex < MAXNUM_CONF_MT; byMtIndex++)
        {
            if(!m_ptTemplateInfo[byIndex].m_atMtAlias[byMtIndex].IsNull())
            {
                OspPrintf(TRUE, FALSE, "Mt%d :", byMtIndex);
                m_ptTemplateInfo[byIndex].m_atMtAlias[byMtIndex].Print();
            }
        }
        
        if(ptConfInfo->GetConfAttrb().IsHasTvWallModule())
        {
            for(u8 byTvLp = 0; byTvLp < m_ptTemplateInfo->m_tMultiTvWallModule.m_byTvModuleNum; byTvLp++)
            {
                TTvWallModule *ptTW = &m_ptTemplateInfo[byIndex].m_tMultiTvWallModule.m_atTvWallModule[byTvLp];
                OspPrintf(TRUE, FALSE, "TvWall Module <%d, %d>:\n", ptTW->m_tTvWall.GetMcuId(), ptTW->m_tTvWall.GetMtId());
                memset(szInfo, 0, sizeof(szInfo));
                for(u8 byTWIndex = 0; byTWIndex < MAXNUM_PERIEQP_CHNNL; byTWIndex++)
                {
                    sprintf(szInfo, "%s:%d", szInfo, ptTW->m_abyTvWallMember[byTWIndex]);
                }
                OspPrintf(TRUE, FALSE, "TvWall mt index %s", szInfo);
            }
        }

        if(ptConfInfo->GetConfAttrb().IsHasVmpModule())
        {
            TVmpModule *ptVmp = &m_ptTemplateInfo[byIndex].m_atVmpModule;
            OspPrintf(TRUE, FALSE, "Vmp Module:\n");
            memset(szInfo, 0, sizeof(szInfo));
            for(u8 byVmpIndex = 0; byVmpIndex < MAXNUM_MPUSVMP_MEMBER; byVmpIndex++)
            {
                sprintf(szInfo, "%s:%d", szInfo, ptVmp->m_abyVmpMember[byVmpIndex]);
            }
            OspPrintf(TRUE, FALSE, "Vmp mt index %s", szInfo);
        }
    }

    return;
}

/*=============================================================================
  函 数 名： ShowConfMap
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void  
=============================================================================*/
void   CMcuVcData::ShowConfMap(void)
{
    OspPrintf(TRUE, FALSE, "\n---------VC Map Data---------\n");

    s8 szConfId[64] = {0};
    TConfInfo *ptConf;
    for(u8 byIndex = 0; byIndex < MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE; byIndex++)
    {
        if ( !m_atConfMapData[byIndex].IsTemUsed() && 
             !m_atConfMapData[byIndex].IsValidConf() )
             continue;

        OspPrintf(TRUE, FALSE, "Map Index: %3d\tTemplateIdx: %2d\tVCInstID: %2d\n", 
                  byIndex+1,
                  m_atConfMapData[byIndex].GetTemIndex(), 
                  m_atConfMapData[byIndex].GetInsId() );

        if ( m_atConfMapData[byIndex].IsTemUsed() )
        {
            u8 byTemIdx = m_atConfMapData[byIndex].GetTemIndex();
            m_ptTemplateInfo[byTemIdx].m_tConfInfo.GetConfId().GetConfIdString(szConfId, 63);
            OspPrintf(TRUE, FALSE, "\tTmpt Name: %s\tConfID:%s\n", 
                      m_ptTemplateInfo[byTemIdx].m_tConfInfo.GetConfName(),
                      szConfId);
        }

        if ( m_atConfMapData[byIndex].IsValidConf() )
        {
            u8 byInsId =  m_atConfMapData[byIndex].GetInsId()-MIN_CONFIDX;
            ptConf = &m_apConfInst[byInsId]->m_tConf;
            ptConf->GetConfId().GetConfIdString(szConfId, 63);
            OspPrintf(TRUE, FALSE, "\tConf Name: %s\tConfID:%s\n", 
                      ptConf->GetConfName(),
                      szConfId);
        }
    }
}


/*=============================================================================
    函 数 名： GetVcDeamonEnvState
    功    能： 主备数据倒换时 获取 同步前的MTADP/MP/外设/会控 等环境信息
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  TMSSynEnvState &tMSSynEnvState
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/12  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetVcDeamonEnvState( TMSSynEnvState &tMSSynEnvState )
{
	u8 byLoop = 0;
	memset(&tMSSynEnvState, 0, sizeof(tMSSynEnvState));

	//会控实际连接状态
	for (byLoop = 0; byLoop < (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byLoop++)
	{
		if (m_atMcTable[byLoop].m_bConnected)
		{
			tMSSynEnvState.m_tMCState.m_abyOnline[byLoop] = 1;
			memcpy((s8*)&tMSSynEnvState.m_tMCState.m_tMcsRegInfo[byLoop], 
				   (s8*)&m_atMcTable[byLoop].m_tMcsRegInfo, sizeof(TMcsRegInfo));
		}
	}
	for (byLoop = 0; byLoop < MAXNUM_MCU_PERIEQP; byLoop++)
	{	
		if (m_atPeriEqpTable[byLoop].m_tPeriEqpStatus.m_byOnline)
		{
			tMSSynEnvState.m_tEqpState.m_abyOnline[byLoop] = 1;
		}
	}
	//Mp实际连接状态
	for (byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if (m_atMpData[byLoop].m_bConnected)
		{
			tMSSynEnvState.m_tMpState.m_abyOnline[byLoop] = 1;
		}	
	}
	//MTADP实际连接状态
	for (byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if (m_atMtAdpData[byLoop].m_bConnected)
		{
			tMSSynEnvState.m_tMtAdpState.m_abyOnline[byLoop] = 1;
		}	
	}
    //DCS实际连接状态
	for (byLoop = 0; byLoop < MAXNUM_MCU_DCS; byLoop++)
	{
		if (m_atPeriDcsTable[byLoop].m_tDcsStatus.m_byOnline)
		{
			tMSSynEnvState.m_tDcsState.m_abyOnline[byLoop] = 1;
		}	
	}
	return TRUE;
}

/*=============================================================================
    函 数 名： IsEqualToVcDeamonEnvState
    功    能： 主备数据倒换时 比较是否一致 同步前的MTADP/MP/外设/会控 等环境信息
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  TMSSynEnvState *ptMSSynEnvState 待比较的外部环境信息               
	           [IN]      BOOL32 bPrintErr                  是否打印失败信息
               [OUT]     TMSEnvConflict &tMSEnvConflict  详细失败信息
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                创建
    2006/06/01  4.0         张宝卿                增加失败信息上报会控处理
=============================================================================*/
BOOL32 CMcuVcData::IsEqualToVcDeamonEnvState( TMSSynEnvState *ptMSSynEnvState, BOOL32 bPrintErr, TMSSynState *ptMSSynState )
{
	if (NULL == ptMSSynEnvState)
	{
		return FALSE;
	}

	//MTADP/Mp/外设 EqpID 由MCU指定，对应InstID唯一，所以可采用InstID唯一标识，进行连接状态校验；
	//会控 无EqpID等ID值指定，对应InstID也不唯一，由会控的提供连接随机值dwMcsSSRC唯一标识，组成TMcsRegInfo进行连接状态校验；

	u8 byLoop  = 0;
	u8 byState = 0;

	//会控实际连接状态
	//会控有效连接数 校验
	u8 byLocalNum  = 0;  //本端会控有效连接数
	u8 byRemoteNum = 0;  //远端会控有效连接数
	for (byLoop = 0; byLoop < (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byLoop++)
	{
		if (m_atMcTable[byLoop].m_bConnected)
		{			
			byLocalNum++; 
		}
		if (1 == ptMSSynEnvState->m_tMCState.m_abyOnline[byLoop])
		{			
			byRemoteNum++; 
		}
	}
	if (byLocalNum != byRemoteNum)
	{
        //设置冲突类型
        ptMSSynState->SetEntityType( TMSSynState::emMC );
        
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "Mcs Online_Num is not Equal: LocalNum.%d RemoteNum.%d\n", 
				      byLocalNum, byRemoteNum);
		}
		return FALSE;
	}

	//有效会控的连接参数 校验
	if (byRemoteNum > 0)
	{
		for (byLoop = 0; byLoop < (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byLoop++)
		{
			if (1 != ptMSSynEnvState->m_tMCState.m_abyOnline[byLoop])
			{
				continue;
			}
			u8 byLocalPos = 0;
			for (byLocalPos = 0; byLocalPos < (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byLocalPos++)
			{
				if (m_atMcTable[byLocalPos].m_bConnected && 
					m_atMcTable[byLocalPos].m_tMcsRegInfo == ptMSSynEnvState->m_tMCState.m_tMcsRegInfo[byLoop])
				{
					break;
				}
			}
			if (MAXNUM_MCU_MC + MAXNUM_MCU_VC == byLocalPos)
			{
                //设置冲突信息
                ptMSSynState->SetEntityType( TMSSynState::emMC );

				if (bPrintErr)
				{
					OspPrintf(TRUE, FALSE, "McsInstId.%d McsRegInfo no match with LocalMC's: RemoteMcsIp.0x%0x McsSSRC.0x%0x\n", 
						      (byLoop+1), ptMSSynEnvState->m_tMCState.m_tMcsRegInfo[byLoop].GetMcsIpAddr(), 
						      ptMSSynEnvState->m_tMCState.m_tMcsRegInfo[byLoop].GetMcsSSRC());
				}
				return FALSE;
			}
		}
	}

	//外设实际连接状态
	for (byLoop = 0; byLoop < MAXNUM_MCU_PERIEQP; byLoop++)
	{
		if (m_atPeriEqpTable[byLoop].m_tPeriEqpStatus.m_byOnline != 
			ptMSSynEnvState->m_tEqpState.m_abyOnline[byLoop])
		{
            //设置冲突信息
            ptMSSynState->SetEntityType( TMSSynState::emPeriEqp );
            ptMSSynState->SetEntityId( byLoop + 1 ); 
            
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "EqpId.%d Online_State is not Equal:Local.%d Remote.%d\n", 
					      (byLoop+1), m_atPeriEqpTable[byLoop].m_tPeriEqpStatus.m_byOnline, 
						  ptMSSynEnvState->m_tEqpState.m_abyOnline[byLoop]);
			}
			return FALSE;
		}
	}
	//Mp实际连接状态
	for (byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		byState = m_atMpData[byLoop].m_bConnected ? 1 : 0;
		if (byState != ptMSSynEnvState->m_tMpState.m_abyOnline[byLoop])
		{
            //设置冲突信息
            ptMSSynState->SetEntityType( TMSSynState::emMp );
            
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "MpId.%d Online_State is not Equal:Local.%d Remote.%d\n", 
					      (byLoop+1), byState, ptMSSynEnvState->m_tMpState.m_abyOnline[byLoop]);
			}
			return FALSE;
		}	
	}
	//MTADP实际连接状态
	for (byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		byState = m_atMtAdpData[byLoop].m_bConnected ? 1 : 0;
		if (byState != ptMSSynEnvState->m_tMtAdpState.m_abyOnline[byLoop])
		{
            //设置冲突信息
            ptMSSynState->SetEntityType( TMSSynState::emMtAdp );

			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "MtadpId.%d Online_State is not Equal:Local.%d Remote.%d\n", 
					      (byLoop+1), byState, ptMSSynEnvState->m_tMtAdpState.m_abyOnline[byLoop]);
			}
			return FALSE;
		}	
	}
	//DCS实际连接状态
	for (byLoop = 0; byLoop < MAXNUM_MCU_DCS; byLoop++)
	{
		if (m_atPeriDcsTable[byLoop].m_tDcsStatus.m_byOnline != 
			ptMSSynEnvState->m_tDcsState.m_abyOnline[byLoop])
		{
            //设置冲突信息
            ptMSSynState->SetEntityType( TMSSynState::emDcs );
            ptMSSynState->SetEntityId( byLoop + 1 );            

			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "DcsId.%d Online_State is not Equal:Local.%d Remote.%d\n", 
					      (byLoop+1), m_atPeriDcsTable[byLoop].m_tDcsStatus.m_byOnline, 
						  ptMSSynEnvState->m_tDcsState.m_abyOnline[byLoop]);
			}
			return FALSE;
		}
	}	
	return TRUE;
}

/*=============================================================================
    函 数 名： GetVcDeamonPeriEqpData
    功    能： 主备数据倒换时 获取 会议应用的公用数据区的外设信息表(包括离线外设) 信息
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
			   [OUT]     u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetVcDeamonPeriEqpData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < (sizeof(TMSVcPeriEqpStateHead)+sizeof(m_atPeriEqpTable)))
	{
		return FALSE;
	}
	
	TMSVcPeriEqpStateHead *ptPeriEqpState = (TMSVcPeriEqpStateHead*)pbyBuf;
	u8 *pbyEqpDataBuf = pbyBuf+sizeof(TMSVcPeriEqpStateHead);
	u8  byPeriEqpNum  = 0;
	memset(pbyBuf, 0, sizeof(TMSVcPeriEqpState));
		
	for (u8 byLoop = 0; byLoop < MAXNUM_MCU_PERIEQP; byLoop++)
	{
		if (m_atPeriEqpTable[byLoop].m_bIsValid) //包括离线外设
		{
			ptPeriEqpState->m_abyValid[byLoop] = 1;
			memcpy(pbyEqpDataBuf, (s8*)(&m_atPeriEqpTable[byLoop]), sizeof(TPeriEqpData));
			pbyEqpDataBuf += sizeof(TPeriEqpData);
			
			byPeriEqpNum += 1;
		}
		else
		{
			ptPeriEqpState->m_abyValid[byLoop] = 0;
		}
	}
	
	dwOutBufLen = sizeof(TMSVcPeriEqpStateHead) + byPeriEqpNum*sizeof(TPeriEqpData);
	
	return TRUE;
}

/*=============================================================================
    函 数 名： SetVcDeamonPeriEqpData
    功    能： 主备数据倒换时 恢复 会议应用的公用数据区的外设信息表(包括离线外设) 信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetVcDeamonPeriEqpData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcPeriEqpStateHead))
	{
		return FALSE;
	}
	
	TMSVcPeriEqpStateHead *ptPeriEqpState = (TMSVcPeriEqpStateHead*)pbyBuf;
	u8 *pbyEqpDataBuf = pbyBuf+sizeof(TMSVcPeriEqpStateHead);
	
	//zbq[06/05/2008] 连接状态主备独立修正
	for (u8 byLoop = 0; byLoop < MAXNUM_MCU_PERIEQP; byLoop++)
	{
		if (1 == ptPeriEqpState->m_abyValid[byLoop])
		{
            //zbq[2006/10/11]外设的连接状态在物理上认为是主备独立，不进行倒换
            u8 byOnline = m_atPeriEqpTable[byLoop].m_tPeriEqpStatus.m_byOnline;
			
			//zbq[2008/07/24]混音器的设备状态半独立(其他外设暂时不作出处理)
			u8 byMixerStatus = TMixerGrpStatus::IDLE;
			if (byLoop <= MIXERID_MAX)
			{
				byMixerStatus = m_atPeriEqpTable[byLoop].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[0].m_byGrpState;
			}
			
			memcpy((s8*)(&m_atPeriEqpTable[byLoop]), pbyEqpDataBuf, sizeof(TPeriEqpData));
			
            m_atPeriEqpTable[byLoop].m_tPeriEqpStatus.m_byOnline = byOnline;
			if (byLoop <= MIXERID_MAX &&
				byMixerStatus == TMixerGrpStatus::WAIT_BEGIN &&
				byMixerStatus == TMixerGrpStatus::WAIT_START_SPECMIX &&
				byMixerStatus == TMixerGrpStatus::WAIT_START_AUTOMIX &&
				byMixerStatus == TMixerGrpStatus::WAIT_START_VAC &&
				byMixerStatus == TMixerGrpStatus::WAIT_STOP )
			{
				m_atPeriEqpTable[byLoop].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[0].m_byGrpState = byMixerStatus;
			}
			
			pbyEqpDataBuf += sizeof(TPeriEqpData);	
		}
		else
		{
			memset((s8*)(&m_atPeriEqpTable[byLoop]), 0, sizeof(TPeriEqpData));
		}
	}

	return TRUE;
}

/*=============================================================================
    函 数 名： GetVcDeamonMCData
    功    能： 主备数据倒换时 获取 会议应用的公用数据区的会控信息表 信息
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
			   [OUT]     u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetVcDeamonMCData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < (sizeof(TMSVcMCStateHead)+sizeof(m_atMcTable)))
	{
		return FALSE;
	}
	
	TMSVcMCStateHead *ptMCStateHead = (TMSVcMCStateHead*)pbyBuf;
	u8 *pbyMCDataBuf = pbyBuf+sizeof(TMSVcMCStateHead);
	u8  byMCNum = 0;
	memset(pbyBuf, 0, sizeof(TMSVcMCStateHead));
		
	for (u8 byLoop = 0; byLoop < (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byLoop++)
	{
		if (m_atMcTable[byLoop].m_bConnected)
		{
			ptMCStateHead->m_abyValid[byLoop] = 1;
			memcpy(pbyMCDataBuf, (s8*)(&m_atMcTable[byLoop]), sizeof(TMcData));
			pbyMCDataBuf += sizeof(TMcData);
			
			byMCNum += 1;
		}
		else
		{
			ptMCStateHead->m_abyValid[byLoop] = 0;
		}
	}
	
	dwOutBufLen = sizeof(TMSVcMCStateHead) + byMCNum*sizeof(TMcData);
	
	return TRUE;
}

/*=============================================================================
    函 数 名： SetVcDeamonMCData
    功    能： 主备数据倒换时 恢复 会议应用的公用数据区的会控信息表 信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetVcDeamonMCData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcMCStateHead))
	{
		return FALSE;
	}
	
	BOOL32 bRet = TRUE;
	TMSVcMCStateHead *ptMCStateHead = (TMSVcMCStateHead*)pbyBuf;
	TMcData *ptMCData = (TMcData *)(pbyBuf+sizeof(TMSVcMCStateHead));

	//MTADP/Mp/外设 EqpID 由MCU指定，对应InstID唯一，所以可采用InstID唯一标识，进行连接状态校验；
	//会控 无EqpID等ID值指定，对应InstID也不唯一，由会控的提供连接随机值dwMcsSSRC唯一标识，组成TMcsRegInfo进行连接状态校验；
	
	for (u8 byLoop = 0; byLoop < MAXNUM_MCU_MC + MAXNUM_MCU_VC; byLoop++)
	{
		if (1 != ptMCStateHead->m_abyValid[byLoop])
		{
			continue;
		}
		u8 byLocalPos = 0;
		for (byLocalPos = 0; byLocalPos < (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byLocalPos++)
		{
			if (m_atMcTable[byLocalPos].m_bConnected && ptMCData->m_bConnected && 
				m_atMcTable[byLocalPos].m_tMcsRegInfo == ptMCData->m_tMcsRegInfo)
			{
				break;
			}
		}
		if ((MAXNUM_MCU_MC + MAXNUM_MCU_VC) == byLocalPos)
		{
			OspPrintf(TRUE, FALSE, "McsInstId.%d McsRegInfo no match with LocalMC's: Connected.%d RemoteMcsIp.0x%0x McsSSRC.0x%0x\n", 
					  (byLoop+1), ptMCData->m_bConnected, 
					  ptMCData->m_tMcsRegInfo.GetMcsIpAddr(), ptMCData->m_tMcsRegInfo.GetMcsSSRC());
			bRet = FALSE;
		}
		else
		{
            //MC的连接状态在物理上认为是主备独立，不进行倒换 2006-10-11
            BOOL32 bConnected = m_atMcTable[byLocalPos].m_bConnected;
			memcpy((s8*)(&m_atMcTable[byLocalPos]), (s8*)ptMCData, sizeof(TMcData));
            m_atMcTable[byLocalPos].m_bConnected = bConnected;
		}
		ptMCData += 1;
	}
	
	return bRet;
}

/*=============================================================================
    函 数 名： GetVcDeamonMpData
    功    能： 主备数据倒换时 获取 会议应用的公用数据区的MP信息表 信息
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
			   [OUT]     u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetVcDeamonMpData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < (sizeof(TMSVcMpState)+sizeof(m_atMpData)))
	{
		return FALSE;
	}
	
	TMSVcMpState *ptMpState = (TMSVcMpState*)pbyBuf;
	u8 *pbyMpDataBuf = pbyBuf+sizeof(TMSVcMpState);
	u8  byMpNum = 0;
	memset(pbyBuf, 0, sizeof(TMSVcMpState));
		
	for (u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if (m_atMpData[byLoop].m_bConnected)
		{
			ptMpState->m_abyOnline[byLoop] = 1;
			memcpy(pbyMpDataBuf, (s8*)(&m_atMpData[byLoop]), sizeof(TMpData));
			pbyMpDataBuf += sizeof(TMpData);
			
			byMpNum += 1;
		}
		else
		{
			ptMpState->m_abyOnline[byLoop] = 0;
		}
	}
	
	dwOutBufLen = sizeof(TMSVcMpState) + byMpNum*sizeof(TMpData);
	
	return TRUE;
}

/*=============================================================================
    函 数 名： SetVcDeamonMpData
    功    能： 主备数据倒换时 恢复 会议应用的公用数据区的MP信息表 信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetVcDeamonMpData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcMpState))
	{
		return FALSE;
	}
	
	TMSVcMpState *ptMpState = (TMSVcMpState*)pbyBuf;
	u8 *pbyMpDataBuf = pbyBuf+sizeof(TMSVcMpState);
	
	//zbq[06/05/2008] 连接状态主备独立修正
	for (u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if (1 == ptMpState->m_abyOnline[byLoop])
		{ 
            //MP的连接状态在物理上认为是主备独立，不进行倒换 2006-10-11
            BOOL32 bConnected = m_atMpData[byLoop].m_bConnected;
			memcpy((s8*)(&m_atMpData[byLoop]), pbyMpDataBuf, sizeof(TMpData)); 
            m_atMpData[byLoop].m_bConnected = bConnected;

			pbyMpDataBuf += sizeof(TMpData);	
		}
		else
		{
			memset((s8*)(&m_atMpData[byLoop]), 0, sizeof(TMpData));
		}
	}

	return TRUE;
}

/*=============================================================================
    函 数 名： GetVcDeamonMtadpData
    功    能： 主备数据倒换时 获取 会议应用的公用数据区的MTADP信息表 信息
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
			   [OUT]     u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetVcDeamonMtadpData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < (sizeof(TMSVcMtAdpState)+sizeof(m_atMtAdpData)))
	{
		return FALSE;
	}
	
	TMSVcMtAdpState *ptMtadpState = (TMSVcMtAdpState*)pbyBuf;
	u8 *pbyMtadpDataBuf = pbyBuf+sizeof(TMSVcMtAdpState);
	u8  byMtadpNum = 0;
	memset(pbyBuf, 0, sizeof(TMSVcMtAdpState));
		
	for (u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if (m_atMtAdpData[byLoop].m_bConnected)
		{
			ptMtadpState->m_abyOnline[byLoop] = 1;
			memcpy(pbyMtadpDataBuf, (s8*)(&m_atMtAdpData[byLoop]), sizeof(TMtAdpData));
			pbyMtadpDataBuf += sizeof(TMtAdpData);
			
			byMtadpNum += 1;
		}
		else
		{
			ptMtadpState->m_abyOnline[byLoop] = 0;
		}
	}
	
	dwOutBufLen = sizeof(TMSVcMtAdpState) + byMtadpNum*sizeof(TMtAdpData);
	
	return TRUE;
}

/*=============================================================================
    函 数 名： SetVcDeamonMtadpData
    功    能： 主备数据倒换时 恢复 会议应用的公用数据区的MTADP信息表 信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetVcDeamonMtadpData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcMtAdpState))
	{
		return FALSE;
	}
	
	TMSVcMtAdpState *ptMtadpState = (TMSVcMtAdpState*)pbyBuf;
	u8 *pbyMtadpDataBuf = pbyBuf+sizeof(TMSVcMtAdpState);
	
	//zbq[06/05/2008] 连接状态主备独立修正
	for (u8 byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if (1 == ptMtadpState->m_abyOnline[byLoop])
		{ 
            //MtAdp的连接状态在物理上认为是主备独立，不进行倒换 2006-10-11
            BOOL32 bConnected = m_atMtAdpData[byLoop].m_bConnected;
			memcpy((s8*)(&m_atMtAdpData[byLoop]), pbyMtadpDataBuf, sizeof(TMtAdpData));
            m_atMtAdpData[byLoop].m_bConnected = bConnected;

			pbyMtadpDataBuf += sizeof(TMtAdpData);	
		}
		else
		{
			memset((s8*)(&m_atMtAdpData[byLoop]), 0, sizeof(TMtAdpData));
		}
	}

	return TRUE;
}

/*=============================================================================
    函 数 名： GetVcDeamonTemplateData
    功    能： 主备数据倒换时 获取 会议应用的公用数据区的模板信息表 信息
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
			   [OUT]     u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetVcDeamonTemplateData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < (sizeof(TMSVcTemplateState)+MAXNUM_MCU_TEMPLATE*sizeof(TTemplateInfo)))
	{
		return FALSE;
	}
	
	TMSVcTemplateState *ptTemplateState = (TMSVcTemplateState*)pbyBuf;
	u8 *pbyTemplateDataBuf = pbyBuf+sizeof(TMSVcTemplateState);
	u8  byTemplateNum = 0;
	memset(pbyBuf, 0, sizeof(TMSVcTemplateState));
	
	for (u8 byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
	{
		if (m_ptTemplateInfo[byLoop].IsEmpty())
		{
			ptTemplateState->m_abyValid[byLoop] = 0;
		}
		else
		{
			ptTemplateState->m_abyValid[byLoop] = 1;
			memcpy(pbyTemplateDataBuf, (s8*)(&m_ptTemplateInfo[byLoop]), sizeof(TTemplateInfo));
			pbyTemplateDataBuf += sizeof(TTemplateInfo);
			
			byTemplateNum += 1;
		}
	}
	
	dwOutBufLen = sizeof(TMSVcTemplateState) + byTemplateNum*sizeof(TTemplateInfo);
	
	return TRUE;
}

/*=============================================================================
    函 数 名： SetVcDeamonTemplateData
    功    能： 主备数据倒换时 恢复 会议应用的公用数据区的模板信息表 信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetVcDeamonTemplateData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcTemplateState))
	{
		return FALSE;
	}
	
	TMSVcTemplateState *ptTemplateState = (TMSVcTemplateState*)pbyBuf;
	u8 *ptTemplateData = pbyBuf+sizeof(TMSVcTemplateState);
	
	for (u8 byLoop = 0; byLoop < MAXNUM_MCU_TEMPLATE; byLoop++)
	{
		if (1 == ptTemplateState->m_abyValid[byLoop])
		{
			memcpy((s8*)(&m_ptTemplateInfo[byLoop]), ptTemplateData, sizeof(TTemplateInfo));
			ptTemplateData += sizeof(TTemplateInfo);	
		}
		else
		{
			m_ptTemplateInfo[byLoop].Clear();
		}
	}
	
	return TRUE;
}

/*=============================================================================
    函 数 名： GetVcDeamonOtherData
    功    能： 主备数据倒换时 获取 会议应用的公用数据区的除EQP/MC/MP/MTADP信息表的其他需同步 信息
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
			   [OUT]     u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetVcDeamonOtherData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	dwOutBufLen  = 0;
	u32 dwOutLen = OprVcDeamonOtherData(pbyBuf, dwInBufLen, TRUE);
	if (0 == dwOutLen)
	{
		return FALSE;
	}
	
	dwOutBufLen = dwOutLen;
	return TRUE;
}

/*=============================================================================
    函 数 名： SetVcDeamonOtherData
    功    能： 主备数据倒换时 恢复 会议应用的公用数据区的除EQP/MC/MP/MTADP信息表的其他需同步 信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 

-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetVcDeamonOtherData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (0 == OprVcDeamonOtherData(pbyBuf, dwInBufLen, FALSE))
	{
		return FALSE;
	}
	
	return TRUE;
}

/*=============================================================================
    函 数 名： OprVcDataOtherData
    功    能： 主备数据倒换时 同步 会议应用的公用数据区的除EQP/MC/MP/MTADP信息表的其他需同步 数据
	           由于同步的主从MCU应用环境一致，同步的数据结构暂不考虑紧凑压栈问题：Pack Ingore
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [IN]      BOOL32 bGet TRUE －获取 FALSE - 恢复 
    返 回 值： u32  同步的数据总长度
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                 创建
    2006/08/07  4.0         顾振华                 增加同步License数据
=============================================================================*/
u32 CMcuVcData::OprVcDeamonOtherData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bGet )
{
	u32 dwOtherDataLen = GetVcDeamonOtherDataLen();
	//允许大于---
	if (NULL == pbyBuf || dwInBufLen < dwOtherDataLen)
	{
		return 0;
	}

	if (bGet)
	{
		memcpy(pbyBuf, (s8*)&m_atRecvMtPort, sizeof(m_atRecvMtPort));
		pbyBuf += sizeof(m_atRecvMtPort);
//		memcpy(pbyBuf, (s8*)&m_atMulticastIp, sizeof(m_atMulticastIp));
//		pbyBuf += sizeof(m_atMulticastIp);
		memcpy(pbyBuf, (s8*)&m_atMulticastPort, sizeof(m_atMulticastPort));
		pbyBuf += sizeof(m_atMulticastPort);
		//不需同步这个，防止误操作
		//memcpy(pbyBuf, (s8*)&m_abyConfStoreInfo, sizeof(m_abyConfStoreInfo));
		//pbyBuf += sizeof(m_abyConfStoreInfo);
		memcpy(pbyBuf, (s8*)&m_abyConfRegState, sizeof(m_abyConfRegState));
		pbyBuf += sizeof(m_abyConfRegState);
		memcpy(pbyBuf, (s8*)&m_byRegGKDriId, sizeof(m_byRegGKDriId));
		pbyBuf += sizeof(m_byRegGKDriId);
		memcpy(pbyBuf, (s8*)&m_tGKID, sizeof(m_tGKID));
		pbyBuf += sizeof(m_tGKID);
		memcpy(pbyBuf, (s8*)&m_tEPID, sizeof(m_tEPID));
		pbyBuf += sizeof(m_tEPID);
		memcpy(pbyBuf, (s8*)&m_dwMakeConfIdTimes, sizeof(m_dwMakeConfIdTimes));
		pbyBuf += sizeof(m_dwMakeConfIdTimes);
		memcpy(pbyBuf, (s8*)&m_tMcuDebugVal, sizeof(m_tMcuDebugVal));
		pbyBuf += sizeof(m_tMcuDebugVal);
		memcpy(pbyBuf, (s8*)&m_dwMtCallInterfaceNum, sizeof(m_dwMtCallInterfaceNum));
		pbyBuf += sizeof(m_dwMtCallInterfaceNum);
		memcpy(pbyBuf, (s8*)m_atMtCallInterface, m_dwMtCallInterfaceNum*sizeof(TMtCallInterface));
		pbyBuf += m_dwMtCallInterfaceNum*sizeof(TMtCallInterface);
		memcpy(pbyBuf, (s8*)m_atConfMapData, sizeof(m_atConfMapData));
		pbyBuf += sizeof(m_atConfMapData);
        memcpy(pbyBuf, (s8*)m_atPeriDcsTable, sizeof(m_atPeriDcsTable));
        pbyBuf += sizeof(m_atPeriDcsTable);
        memcpy(pbyBuf, (s8*)&m_byChargeRegOK, sizeof(m_byChargeRegOK));
        pbyBuf += sizeof(m_byChargeRegOK);
	}
	else
	{
		memcpy((s8*)&m_atRecvMtPort, pbyBuf, sizeof(m_atRecvMtPort));
		pbyBuf += sizeof(m_atRecvMtPort);
//		memcpy((s8*)&m_atMulticastIp, pbyBuf, sizeof(m_atMulticastIp));
//		pbyBuf += sizeof(m_atMulticastIp);
		memcpy((s8*)&m_atMulticastPort, pbyBuf, sizeof(m_atMulticastPort));
		pbyBuf += sizeof(m_atMulticastPort);
		//不需同步这个，防止误操作
		//memcpy((s8*)&m_abyConfStoreInfo, pbyBuf, sizeof(m_abyConfStoreInfo));
		//pbyBuf += sizeof(m_abyConfStoreInfo);
		memcpy((s8*)&m_abyConfRegState, pbyBuf, sizeof(m_abyConfRegState));
		pbyBuf += sizeof(m_abyConfRegState);
		memcpy((s8*)&m_byRegGKDriId, pbyBuf, sizeof(m_byRegGKDriId));
		pbyBuf += sizeof(m_byRegGKDriId);
		memcpy((s8*)&m_tGKID, pbyBuf, sizeof(m_tGKID));
		pbyBuf += sizeof(m_tGKID);
		memcpy((s8*)&m_tEPID, pbyBuf, sizeof(m_tEPID));
		pbyBuf += sizeof(m_tEPID);
		memcpy((s8*)&m_dwMakeConfIdTimes, pbyBuf, sizeof(m_dwMakeConfIdTimes));
		pbyBuf += sizeof(m_dwMakeConfIdTimes);        

		memcpy((s8*)&m_tMcuDebugVal, pbyBuf, sizeof(m_tMcuDebugVal));
		pbyBuf += sizeof(m_tMcuDebugVal);
        
		memcpy((s8*)&m_dwMtCallInterfaceNum, pbyBuf, sizeof(m_dwMtCallInterfaceNum));
		pbyBuf += sizeof(m_dwMtCallInterfaceNum);
		memcpy((s8*)m_atMtCallInterface, pbyBuf, m_dwMtCallInterfaceNum*sizeof(TMtCallInterface));
		pbyBuf += m_dwMtCallInterfaceNum*sizeof(TMtCallInterface);
		memcpy((s8*)m_atConfMapData, pbyBuf, sizeof(m_atConfMapData));
		pbyBuf += sizeof(m_atConfMapData);

		//zbq[06/05/2008] 连接状态主备独立修正
		/*
        //DCS的连接状态在物理上认为是主备独立，不进行倒换 2006-10-11
        u8 byIndex = 0;
        u8 abyDcsOnline[MAXNUM_MCU_DCS] = { 0 }; 
        for( ; byIndex < MAXNUM_MCU_DCS; byIndex ++ )
        {
            abyDcsOnline[byIndex] = m_atPeriDcsTable[byIndex].m_tDcsStatus.m_byOnline;
        }
        memcpy((s8*)&m_atPeriDcsTable, pbyBuf, sizeof(m_atPeriDcsTable));
        for( byIndex = 0; byIndex < MAXNUM_MCU_DCS; byIndex ++ )
        {
            m_atPeriDcsTable[byIndex].m_tDcsStatus.m_byOnline = abyDcsOnline[byIndex];
        }
		*/
        pbyBuf += sizeof(m_atPeriDcsTable);

        memcpy((s8*)&m_byChargeRegOK, pbyBuf, sizeof(m_byChargeRegOK));
        pbyBuf += sizeof(m_byChargeRegOK);
	}

	return dwOtherDataLen;
}

/*=============================================================================
    函 数 名： GetVcDeamonOtherDataLen
    功    能： 主备数据倒换时 获取 会议应用的公用数据区的除EQP/MC/MP/MTADP信息表的其他需同步 数据总长度
	           由于同步的主从MCU应用环境一致，同步的数据结构暂不考虑紧凑压栈问题：Pack Ingore
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： u32  数据总长度
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                 创建
    2006/08/07  4.0         顾振华                 增加同步License数据
=============================================================================*/
u32 CMcuVcData::GetVcDeamonOtherDataLen( void )
{
	u32 dwOtherDataLen = 0;
	
	//12
	dwOtherDataLen += sizeof(m_atRecvMtPort);
//	dwOtherDataLen += sizeof(m_atMulticastIp);
	dwOtherDataLen += sizeof(m_atMulticastPort);
	//dwOtherDataLen += sizeof(m_abyConfStoreInfo);
	dwOtherDataLen += sizeof(m_abyConfRegState);
	dwOtherDataLen += sizeof(m_byRegGKDriId);
	dwOtherDataLen += sizeof(m_tGKID);
	dwOtherDataLen += sizeof(m_tEPID);
	dwOtherDataLen += sizeof(m_dwMakeConfIdTimes);
	dwOtherDataLen += sizeof(m_tMcuDebugVal);
	dwOtherDataLen += sizeof(m_dwMtCallInterfaceNum);
	dwOtherDataLen += m_dwMtCallInterfaceNum*sizeof(TMtCallInterface);
	dwOtherDataLen += sizeof(m_atConfMapData);
    dwOtherDataLen += sizeof(m_atPeriDcsTable);
    dwOtherDataLen += sizeof(m_byChargeRegOK);
	
	return dwOtherDataLen;
}

/*=============================================================================
    函 数 名： HangupAllVcInstTimer
    功    能： 主备数据倒换时 暂停 所有会议实例 轮循、预约会议等会议定时器（非STATE_IDLE实例）
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::HangupAllVcInstTimer( void )
{
	BOOL32 bRet = TRUE;
	
	CMcuVcInst* pcVcInst = NULL;
	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_CONF; byInstID++)
	{
		pcVcInst = m_apConfInst[byInstID-1];
		if (NULL != pcVcInst)
		{
			if (FALSE == pcVcInst->HangupInstTimer())
			{
				bRet = FALSE;
			}
		}	
	}
	
	return bRet;
}

/*=============================================================================
    函 数 名： ResumeAllVcInstTimer
    功    能： 主备数据倒换时 恢复 所有会议实例 轮循、预约会议等会议定时器（非STATE_IDLE实例）
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::ResumeAllVcInstTimer( void )
{
	BOOL32 bRet = TRUE;
	
	CMcuVcInst* pcVcInst = NULL;
	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_CONF; byInstID++)
	{
		pcVcInst = m_apConfInst[byInstID-1];
		if (NULL != pcVcInst)
		{
			if (FALSE == pcVcInst->ResumeInstTimer())
			{
				bRet = FALSE;
			}
		}	
	}
	
	return bRet;
}

/*=============================================================================
    函 数 名： GetAllVcInstState
    功    能： 主备数据倒换时 获取 所有会议实例 状态机 状态
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  TMSVcInstState &tVcInstState 状态数据
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetAllVcInstState( TMSVcInstState &tVcInstState )
{
	BOOL32 bRet = TRUE;
	
	u8 byState  = 0;
	CApp* pcApp = &g_cMcuVcApp;
	CMcuVcInst* pcVcInst = NULL;
	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_CONF; byInstID++)
	{
		pcVcInst = (CMcuVcInst*)pcApp->GetInstance(byInstID);
		if (NULL != pcVcInst)
		{
			pcVcInst->GetInstState(byState);	
		}
		else
		{
			byState = 0;
			bRet = FALSE;
		}
		tVcInstState.m_abyState[byInstID-1] = byState;
	}
	
	return bRet;
}

/*=============================================================================
    函 数 名： SetAllVcInstState
    功    能： 主备数据倒换时 恢复 所有会议实例 状态机 状态
	           同时同步公用区数据
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  TMSVcInstState &tVcInstState 状态数据
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetAllVcInstState( TMSVcInstState &tVcInstState )
{
	BOOL32 bRet = TRUE;

	CApp* pcApp = &g_cMcuVcApp;
	CMcuVcInst* pcVcInst = NULL;

	memset(m_apConfInst, 0, sizeof(m_apConfInst));

	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_CONF; byInstID++)
	{
		pcVcInst = (CMcuVcInst*)pcApp->GetInstance(byInstID);
		if (NULL != pcVcInst)
		{
			pcVcInst->SetInstState(tVcInstState.m_abyState[byInstID-1]);
		}
		else
		{
			bRet = FALSE;
		}
	}

	RefreshHtml();
	
	return bRet;
}

/*=============================================================================
    函 数 名： FindNextConfInstIDOfNotIdle
    功    能： 根据上一个会议实例ID 查询大于此ID的下一个非空闲会议实例ID
    算法实现： 
    全局变量： 
    参    数： [IN] u8  byPreInstID  －Preview No-Idle-Inst ID
    返 回 值： u8 - Next No-Idle-Inst ID  [1, MAXNUM_MCU_CONF]  0－Not Find
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
u8 CMcuVcData::FindNextConfInstIDOfNotIdle( u8 byPreInstID )
{
	u8 byNextInstID = 0;
	u8 byState = 0;
	CMcuVcInst* pcVcInst = NULL;
	
	for (u8 byInstID = (byPreInstID+1); byInstID < MAXNUM_MCU_CONF; byInstID++)
	{	
		pcVcInst = m_apConfInst[byInstID-1];
		if (NULL != pcVcInst)
		{
			pcVcInst->GetInstState(byState);
			if (0 != byState)
			{
				byNextInstID = byInstID;
				break;
			}
		}
		
	}
	
	return byNextInstID;
}


/*=============================================================================
函 数 名： GetTakeModeOfInstId
功    能： 根据会议实例ID 查询会议的召开模式
算法实现： 
全局变量： 
参    数： 
返 回 值：
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2007/04/18  4.0			顾振华                  创建
=============================================================================*/
u8 CMcuVcData::GetTakeModeOfInstId( u8 byInstId )
{
    u8 byNextInstID = 0;
    u8 byState = 0;
    CMcuVcInst* pcVcInst = NULL;
    
    if ( byInstId == 0 || byInstId >= MAXNUM_MCU_CONF )
    {
        return 0;   // 返回0误认为是Schedule没有关系
    }
    
    pcVcInst = m_apConfInst[byInstId-1];
    if (NULL != pcVcInst)
    {
        return pcVcInst->m_tConf.m_tStatus.GetTakeMode();
    }
    else
    {
        return 0;
    }
}

/*=============================================================================
    函 数 名： GetOneVcInstConfMtTableData
    功    能： 主备数据倒换时 获取 指定实例的 会议终端表 信息
    算法实现： 
    全局变量： 
    参    数： [IN]      u8  byInstID
               [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [OUT]     u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetOneVcInstConfMtTableData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}
	
	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}
	
	return pcVcInst->GetConfMtTableData(pbyBuf, dwInBufLen, dwOutBufLen);
}

/*=============================================================================
    函 数 名： SetOneVcInstConfMtTableData
    功    能： 主备数据倒换时 恢复 指定实例的 会议终端表 信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8  byInstID
               [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetOneVcInstConfMtTableData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}
	
	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}
	
	return pcVcInst->SetConfMtTableData(pbyBuf, dwInBufLen);
}

/*=============================================================================
    函 数 名： GetOneVcInstConfSwitchTableData
    功    能： 主备数据倒换时 获取 指定实例的 会议交换表 信息
    算法实现： 
    全局变量： 
    参    数： [IN]      u8  byInstID
               [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [OUT]     u32 &dwOutBufLen
    返 回 值： BOOL32 
	-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetOneVcInstConfSwitchTableData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}
	
	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}
	
	return pcVcInst->GetConfSwitchTableData(pbyBuf, dwInBufLen, dwOutBufLen);
}

/*=============================================================================
    函 数 名： SetOneVcInstConfSwitchTableData
    功    能： 主备数据倒换时 恢复 指定实例的 会议交换表 信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8  byInstID
               [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 
	-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetOneVcInstConfSwitchTableData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}
	
	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}
	
	return pcVcInst->SetConfSwitchTableData(pbyBuf, dwInBufLen);
}

/*=============================================================================
    函 数 名： GetOneVcInstConfOtherMcTableData
    功    能： 主备数据倒换时 获取 指定实例的 其它Mc终端列表 信息
    算法实现： 
    全局变量： 
    参    数： [IN]      u8  byInstID
               [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [OUT]     u32 &dwOutBufLen
    返 回 值： BOOL32 
	-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetOneVcInstConfOtherMcTableData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}
	
	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}
	
	return pcVcInst->GetConfOtherMcTableData(pbyBuf, dwInBufLen, dwOutBufLen);
}

/*=============================================================================
    函 数 名： SetOneVcInstConfOtherMcTableData
    功    能： 主备数据倒换时 恢复 指定实例的 其它Mc终端列表 信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8  byInstID
               [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 
	-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetOneVcInstConfOtherMcTableData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}
	
	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}
	
	return pcVcInst->SetConfOtherMcTableData(pbyBuf, dwInBufLen);
}

/*=============================================================================
    函 数 名： GetOneVcInstOtherData
    功    能： 主备数据倒换时 获取 指定实例的 除上述动态分配的列表信息外的其他需同步 信息
    算法实现： 
    全局变量： 
    参    数： [IN]      u8  byInstID
               [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
			   [OUT]     u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetOneVcInstOtherData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}
	
	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}
	
	return pcVcInst->GetInstOtherData(pbyBuf, dwInBufLen, dwOutBufLen);
}

/*=============================================================================
    函 数 名： SetOneVcInstOtherData
    功    能： 主备数据倒换时 恢复 指定实例的 除上述动态分配的列表信息外的其他需同步 信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8  byInstID
               [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [IN]  BOOL32 bResumeTimer
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetOneVcInstOtherData( u8 byInstID, u8 *pbyBuf, u32 dwInBufLen, BOOL32 bResumeTimer )
{
	if (0 == byInstID || byInstID > MAXNUM_MCU_CONF)
	{
		return FALSE;
	}

	CMcuVcInst* pcVcInst = m_apConfInst[byInstID-1];
	if (NULL == pcVcInst)
	{
		return FALSE;
	}

	return pcVcInst->SetInstOtherData(pbyBuf, dwInBufLen, bResumeTimer);
}

/*=============================================================================
    函 数 名： GetCfgFileData
    功    能： 主备数据倒换时 获取 mcu/conf下的 mcucfg.ini 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
	           [IN]  BOOL32 bPrintErr 是否打印失败信息
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetCfgFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr )
{
	if (NULL == pbyBuf)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[GetCfgFileData] NULL == pbyBuf\n");
		}
		return FALSE;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME);
	
	FILE *hFile  = fopen(achFullName, "rb");
	s32 nFileLen = 0;
	if (NULL != hFile)
	{
		fseek(hFile, 0, SEEK_END);
		nFileLen = ftell(hFile);
		if (nFileLen > (s32)dwInBufLen)
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[GetCfgFileData] mcucfg.ini FileLen(%d)>%d\n", nFileLen, dwInBufLen);			
			}
			return FALSE;
		}
		if (nFileLen > 0)
		{
			fseek(hFile, 0, SEEK_SET);
			fread(pbyBuf, nFileLen, 1, hFile);
		}
		fclose(hFile);
		hFile = NULL;
	}
	
	//允许文件长度为0，文件存在则同步清空该文件
	dwOutBufLen = (u32)nFileLen;

	return TRUE;
}

/*=============================================================================
    函 数 名： SetCfgFileData
    功    能： 主备数据倒换时 恢复 mcu/conf下的 mcucfg.ini 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
	           [IN]  BOOL32 bPrintErr 是否打印失败信息
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetCfgFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr )
{
	BOOL32 bRet = FALSE;
	
	//允许文件长度为0，文件存在则同步清空该文件
	if (NULL == pbyBuf && dwInBufLen > 0)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[SetCfgFileData] NULL == pbyBuf && InBufLen(%d) > 0\n", dwInBufLen);
		}
		return bRet;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME);
	
	BOOL32 bReWrite = TRUE;
	if (0 == dwInBufLen)
	{
		FILE *hFile = fopen(achFullName, "rb");
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
		else
		{
			bReWrite = FALSE;
			bRet = TRUE;
		}
	}
	if (bReWrite)
	{
		FILE *hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(pbyBuf, 1, dwInBufLen, hFile);
#ifdef WIN32    //VX下不作多余的 fflush 操作
				fflush(hFile);
#endif
			}
			fclose(hFile);
			hFile = NULL;
			bRet = TRUE;
		}
		else
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[SetCfgFileData] save mcucfg.ini failed\n");
			}
		}
	}

	return bRet;
}

/*=============================================================================
    函 数 名： GetDebugFileData
    功    能： 主备数据倒换时 获取 mcu/conf下的 mcudebug.ini 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
	           [IN]  BOOL32 bPrintErr 是否打印失败信息
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetDebugFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr )
{
	if (NULL == pbyBuf)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[GetDebugFileData] NULL == pbyBuf\n");
		}
		return FALSE;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUDEBUGFILENAME);
	
	FILE *hFile  = fopen(achFullName, "rb");
	s32 nFileLen = 0;
	if (NULL != hFile)
	{
		fseek(hFile, 0, SEEK_END);
		nFileLen = ftell(hFile);
		if (nFileLen > (s32)dwInBufLen)
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[GetDebugFileData] mcudebug.ini FileLen(%d)>%d\n", nFileLen, dwInBufLen);			
			}
			return FALSE;
		}
		if (nFileLen > 0)
		{
			fseek(hFile, 0, SEEK_SET);
			fread(pbyBuf, nFileLen, 1, hFile);
		}
		fclose(hFile);
		hFile = NULL;
	}
	
	//允许文件长度为0，文件存在则同步清空该文件
	dwOutBufLen = (u32)nFileLen;

	return TRUE;
}

/*=============================================================================
    函 数 名： SetDebugFileData
    功    能： 主备数据倒换时 恢复 mcu/conf下的 mcudebug.ini 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
	           [IN]  BOOL32 bPrintErr 是否打印失败信息
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetDebugFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr )
{
	BOOL32 bRet = FALSE;
	
	//允许文件长度为0，文件存在则同步清空该文件
	if (NULL == pbyBuf && dwInBufLen > 0)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[SetDebugFileData] NULL == pbyBuf && InBufLen(%d) > 0\n", dwInBufLen);
		}
		return bRet;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUDEBUGFILENAME);
	
	BOOL32 bReWrite = TRUE;
	if (0 == dwInBufLen)
	{
		FILE *hFile = fopen(achFullName, "rb");
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
		else
		{
			bReWrite = FALSE;
			bRet = TRUE;
		}
	}
	if (bReWrite)
	{
		FILE *hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(pbyBuf, 1, dwInBufLen, hFile);
#ifdef WIN32    //VX下不作多余的 fflush 操作
				fflush(hFile);
#endif
			}
			fclose(hFile);
			hFile = NULL;
			bRet = TRUE;
		}
		else
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[SetDebugFileData] save mcudebug.ini failed\n");
			}
		}
	}

	return bRet;
}

/*=============================================================================
    函 数 名： GetAddrbookFileData
    功    能： 主备数据倒换时 获取 mcu/conf下的 addrbook.kdv 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
	           [IN]  BOOL32 bPrintErr 是否打印失败信息
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetAddrbookFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr )
{
	if (NULL == pbyBuf)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[GetAddrbookFileData] NULL == pbyBuf\n");
		}
		return FALSE;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, MCUADDRFILENAME);
	
	FILE *hFile  = fopen(achFullName, "rb");
	s32 nFileLen = 0;
	if (NULL != hFile)
	{
		fseek(hFile, 0, SEEK_END);
		nFileLen = ftell(hFile);
		if (nFileLen > (s32)dwInBufLen)
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[GetAddrbookFileData] addrbook.kdv FileLen(%d)>%d\n", nFileLen, dwInBufLen);			
			}
			return FALSE;
		}
		if (nFileLen > 0)
		{
			fseek(hFile, 0, SEEK_SET);
			fread(pbyBuf, nFileLen, 1, hFile);
		}
		fclose(hFile);
		hFile = NULL;
	}
	
	//允许文件长度为0，文件存在则同步清空该文件
	dwOutBufLen = (u32)nFileLen;

	return TRUE;
}

/*=============================================================================
    函 数 名： SetAddrbookFileData
    功    能： 主备数据倒换时 恢复 mcu/conf下的 addrbook.kdv 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
	           [IN]  BOOL32 bPrintErr 是否打印失败信息
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetAddrbookFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr )
{
	BOOL32 bRet = FALSE;

	//允许文件长度为0，文件存在则同步清空该文件
	if (NULL == pbyBuf && dwInBufLen > 0)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[SetAddrbookFileData] NULL == pbyBuf && InBufLen(%d) > 0\n", dwInBufLen);
		}
		return bRet;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, MCUADDRFILENAME);
	
	BOOL32 bReWrite = TRUE;
	if (0 == dwInBufLen)
	{
		FILE *hFile = fopen(achFullName, "rb");
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
		else
		{
			bReWrite = FALSE;
			bRet = TRUE;
		}
	}
	if (bReWrite)
	{
		FILE *hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(pbyBuf, 1, dwInBufLen, hFile);
#ifdef WIN32    //VX下不作多余的 fflush 操作
				fflush(hFile);
#endif
			}
			fclose(hFile);
			hFile = NULL;
			bRet = TRUE;
		}
		else
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[SetAddrbookFileData] save addrbook.kdv failed\n");
			}
		}
	}

	return bRet;
}

/*=============================================================================
    函 数 名： GetConfinfoFileData
    功    能： 主备数据倒换时 获取 mcu/data下的 confinfo.dat 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
	           [IN]  BOOL32 bPrintErr 是否打印失败信息
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr )
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcConfState)) //?? 缓冲大小需要判别
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[GetConfinfoFileData] NULL==pbyBuf InBufLen.%d\n", dwInBufLen);
		}
		return FALSE;
	}

	TMSVcConfState *ptConfState = (TMSVcConfState*)pbyBuf;
	u8 *pbyConfDataBuf = pbyBuf+sizeof(TMSVcConfState);
	u32 dwConfDataTotalLen = 0;
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	memset(pbyBuf, 0, sizeof(TMSVcConfState));	
	memset(acConfId, 0, sizeof(acConfId));
	
	//读取 会议头信息及缺省会议信息_存储文件
	if (TRUE == GetAllConfHeadFromFile(acConfId, sizeof(acConfId)))
	{
		u8  byLoop = 0;
		u16 wConfDataLen = 0;
		
		//包括最后一个位置上的缺省会议信息
		for (byLoop = 0; byLoop < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1); byLoop++)
		{
			if (FALSE == acConfId[byLoop].IsNull())
			{
				if (TRUE == ::GetConfDataFromFile(byLoop, acConfId[byLoop], 
					                              pbyConfDataBuf, sizeof(TConfStore), wConfDataLen)) 
				{
					ptConfState->m_abyValid[byLoop] = 1;
					ptConfState->m_awConfDataLen[byLoop] = htons(wConfDataLen);
					pbyConfDataBuf += wConfDataLen;
					dwConfDataTotalLen += wConfDataLen;

                    // 测试 [4/28/2006]
                    // OspPrintf( TRUE, FALSE, "[GuZhenhua MS Dump] Loaded Confinfo_%d!\n", byLoop );
				}
				else
				{
					OspPrintf(TRUE, FALSE, "[GetConfinfoFileData] GetConfDataFromFile Err: Loop.%d\n", byLoop);
				}
			}
		}
	}
	else
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[GetConfinfoFileData] GetAllConfHeadFromFile Err\n");
		}
	}
	
	//允许文件长度为0，文件存在则同步清空
	dwOutBufLen = sizeof(TMSVcConfState)+dwConfDataTotalLen;

    // 顾振华 [4/28/2006] 这里为什么要写一遍？？？
    // 暂时注释
	// SetConfinfoFileData(pbyBuf, dwOutBufLen, TRUE);

	return TRUE;
}

/*=============================================================================
    函 数 名： SetConfinfoFileData
    功    能： 主备数据倒换时 恢复 mcu/data下的 confinfo.dat 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
	           [IN]  BOOL32 bPrintErr 是否打印失败信息
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr )
{
	//允许文件长度为0，文件存在则同步清空该文件
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcConfState))
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[SetConfinfoFileData] NULL==pbyBuf || InBufLen(%d)>0\n", dwInBufLen);
		}
		return FALSE;
	}
	
	TMSVcConfState *ptConfState = (TMSVcConfState*)pbyBuf;
	u8 *pbyConfDataBuf = pbyBuf+sizeof(TMSVcConfState);
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	memset(acConfId, 0, sizeof(acConfId));
	    
	u8  byLoop = 0;
	u16 wConfDataLen = 0;
	CConfId cConfId;
	cConfId.SetNull();
	for (byLoop = 0; byLoop < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1); byLoop++)
	{
		wConfDataLen = ntohs(ptConfState->m_awConfDataLen[byLoop]);
		if (1 == ptConfState->m_abyValid[byLoop])
		{
			if (TRUE == ::SetConfDataToFile(byLoop, cConfId, pbyConfDataBuf, wConfDataLen))
			{
                // 测试 [4/28/2006]
                // OspPrintf( TRUE, FALSE, "[GuZhenhua MS Dump] Writed Confinfo_%d!\n", byLoop );
				acConfId[byLoop] = cConfId;
			}
			else
			{
				OspPrintf(TRUE, FALSE, "[SetConfinfoFileData] SetConfDataToFile Err: Loop.%d\n", byLoop);
			}
			pbyConfDataBuf += wConfDataLen;
		}
	}

	if (FALSE == SetAllConfHeadToFile(acConfId, sizeof(acConfId)))
	{
		OspPrintf(TRUE, FALSE, "[SetConfinfoFileData] SetAllConfHeadToFile Err\n");
	}

    // 测试 [4/28/2006]
    // OspPrintf( TRUE, FALSE, "[GuZhenhua MS Dump] Writed Confinfo_head!\n" );

	return TRUE;
}

/*=============================================================================
    函 数 名： GetLoguserFileData
    功    能： 主备数据倒换时 获取 mcu/data下的 login.usr 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
	           [IN]  BOOL32 bPrintErr 是否打印失败信息
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::GetLoguserFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr )
{
	if (NULL == pbyBuf)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[GetLoguserFileData] NULL == pbyBuf\n");
		}
		return FALSE;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, USERINFOFILENAME);
	
	FILE *hFile  = fopen(achFullName, "rb");
	s32 nFileLen = 0;
	if (NULL != hFile)
	{
		fseek(hFile, 0, SEEK_END);
		nFileLen = ftell(hFile);
		if (nFileLen > (s32)dwInBufLen)
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[GetLoguserFileData] login.usr FileLen(%d)>%d\n", nFileLen, dwInBufLen);
			}
			return FALSE;
		}
		if (nFileLen > 0)
		{
			fseek(hFile, 0, SEEK_SET);
			fread(pbyBuf, nFileLen, 1, hFile);
		}
		fclose(hFile);
		hFile = NULL;
	}
	
	//允许文件长度为0，文件存在则同步清空该文件
	dwOutBufLen = (u32)nFileLen;

	return TRUE;
}

/*=============================================================================
    函 数 名： SetLoguserFileData
    功    能： 主备数据倒换时 恢复 mcu/data下的 login.usr 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
	           [IN]  BOOL32 bPrintErr 是否打印失败信息
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcData::SetLoguserFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr )
{
	BOOL32 bRet = FALSE;

	//允许文件长度为0，文件存在则同步清空该文件
	if (NULL == pbyBuf && dwInBufLen > 0)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[SetLoguserFileData] NULL == pbyBuf && InBufLen(%d) > 0\n", dwInBufLen);
		}
		return bRet;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, USERINFOFILENAME);
	
	BOOL32 bReWrite = TRUE;
	if (0 == dwInBufLen)
	{
		FILE *hFile = fopen(achFullName, "rb");
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
		else
		{
			bReWrite = FALSE;
			bRet = TRUE;
		}
	}
	if (bReWrite)
	{
		FILE *hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(pbyBuf, 1, dwInBufLen, hFile);
#ifdef WIN32    //VX下不作多余的 fflush 操作
				fflush(hFile);
#endif
			}
			fclose(hFile);
			hFile = NULL;
			bRet = TRUE;
		}
		else
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[SetLoguserFileData] save login.usr failed\n");
			}
		}
	}

	return bRet;
}

/*=============================================================================
    函 数 名： GetUserExFileData
    功    能： 主备数据倒换时 获取 mcu/data下的 usrgrp.usr 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
	           [IN]  BOOL32 bPrintErr 是否打印失败信息
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/06/22  4.0			顾振华                创建
=============================================================================*/
BOOL32 CMcuVcData::GetUserExFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen, BOOL32 bPrintErr )
{
	if (NULL == pbyBuf)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[GetUserExFileData] NULL == pbyBuf\n");
		}
		return FALSE;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, USERGRPINFOFILENAME);
	
	FILE *hFile  = fopen(achFullName, "rb");
	s32 nFileLen = 0;
	if (NULL != hFile)
	{
		fseek(hFile, 0, SEEK_END);
		nFileLen = ftell(hFile);
		if (nFileLen > (s32)dwInBufLen)
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[GetUserExFileData] usrgrp.usr FileLen(%d)>%d\n", nFileLen, dwInBufLen);
			}
			return FALSE;
		}
		if (nFileLen > 0)
		{
			fseek(hFile, 0, SEEK_SET);
			fread(pbyBuf, nFileLen, 1, hFile);
		}
		fclose(hFile);
		hFile = NULL;
	}
	
	//允许文件长度为0，文件存在则同步清空该文件
	dwOutBufLen = (u32)nFileLen;

	return TRUE;
}

/*=============================================================================
    函 数 名： SetUserExFileData
    功    能： 主备数据倒换时 恢复 mcu/data下的 usrgrp.usr 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
	           [IN]  BOOL32 bPrintErr 是否打印失败信息
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/06/22  4.0			顾振华                创建
=============================================================================*/
BOOL32 CMcuVcData::SetUserExFileData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bPrintErr )
{
	BOOL32 bRet = FALSE;

	//允许文件长度为0，文件存在则同步清空该文件
	if (NULL == pbyBuf && dwInBufLen > 0)
	{
		if (bPrintErr)
		{
			OspPrintf(TRUE, FALSE, "[SetUserExFileData] NULL == pbyBuf && InBufLen(%d) > 0\n", dwInBufLen);
		}
		return bRet;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, USERGRPINFOFILENAME);
	
	BOOL32 bReWrite = TRUE;
	if (0 == dwInBufLen)
	{
		FILE *hFile = fopen(achFullName, "rb");
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
		else
		{
			bReWrite = FALSE;
			bRet = TRUE;
		}
	}
	if (bReWrite)
	{
		FILE *hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(pbyBuf, 1, dwInBufLen, hFile);
#ifdef WIN32    //VX下不作多余的 fflush 操作
				fflush(hFile);
#endif
			}
			fclose(hFile);
			hFile = NULL;
			bRet = TRUE;
		}
		else
		{
			if (bPrintErr)
			{
				OspPrintf(TRUE, FALSE, "[SetUserExFileData] save usrgrp.usr failed\n");
			}
		}
	}

	return bRet;
}
/*=============================================================================
    函 数 名： GetUnProcConfinfoFileData
    功    能： 主备数据倒换时 获取 mcu/data下的 unprocconfinfo.dat 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/06/22  4.0			fxh                   创建
=============================================================================*/
BOOL32 CMcuVcData::GetUnProcConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen)
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcConfState)) 
	{
		OspPrintf(TRUE, FALSE, "[GetUnProcConfinfoFileData] NULL==pbyBuf InBufLen.%d\n", dwInBufLen);
		return FALSE;
	}

	TMSVcConfState *ptConfState = (TMSVcConfState*)pbyBuf;
	u8 *pbyConfDataBuf = pbyBuf + sizeof(TMSVcConfState);
	u32 dwConfDataTotalLen = 0;
	CConfId acConfId[MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1];
	memset(pbyBuf, 0, sizeof(TMSVcConfState));	
	memset(acConfId, 0, sizeof(acConfId));
	
	//读取 会议头信息及缺省会议信息_存储文件
	if (TRUE == GetAllConfHeadFromFile(acConfId, sizeof(acConfId)))
	{
		u16 wUnProcConfDataLen = 0;		
		for (u8 byLoop = 0; byLoop < (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE); byLoop++)
		{
			if (FALSE == acConfId[byLoop].IsNull())
			{
				if (TRUE == ::GetUnProConfDataToFile(byLoop, (s8*)pbyConfDataBuf, wUnProcConfDataLen)) 
				{
					ptConfState->m_abyValid[byLoop] = 1;
					ptConfState->m_awConfDataLen[byLoop] = htons(wUnProcConfDataLen);
					pbyConfDataBuf += wUnProcConfDataLen;
					dwConfDataTotalLen += wUnProcConfDataLen;
				}
				else
				{
					OspPrintf(TRUE, FALSE, "[GetUnProcConfinfoFileData] GetUnProConfDataToFile Err: Loop.%d\n", byLoop);
				}
			}
		}
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[GetUnProcConfinfoFileData] GetAllConfHeadFromFile Err\n");
	}
	
	dwOutBufLen = sizeof(TMSVcConfState) + dwConfDataTotalLen;
	return TRUE;
}
/*=============================================================================
    函 数 名： SetUnProcConfinfoFileData
    功    能： 主备数据倒换时 保存mcu/data下的 unprocconfinfo.dat 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/06/22  4.0			fxh                   创建
=============================================================================*/
BOOL32 CMcuVcData::SetUnProcConfinfoFileData( u8 *pbyBuf, u32 dwInBufLen)
{
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcConfState))
	{
		OspPrintf(TRUE, FALSE, "[SetUnProcConfinfoFileData] NULL==pbyBuf || InBufLen(%d)< sizeof(TMSVcConfState)\n", dwInBufLen);
		return FALSE;
	}
	
	TMSVcConfState *ptConfState = (TMSVcConfState*)pbyBuf;
	u8 *pbyConfDataBuf = pbyBuf + sizeof(TMSVcConfState);    
	u16 wConfDataLen = 0;
	for (u8 byLoop = 0; byLoop < (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE); byLoop++)
	{
		wConfDataLen = ntohs(ptConfState->m_awConfDataLen[byLoop]);
		if (1 == ptConfState->m_abyValid[byLoop])
		{
			if (FALSE == ::SetUnProConfDataToFile(byLoop, (s8*)pbyConfDataBuf, wConfDataLen))
			{
				OspPrintf(TRUE, FALSE, "[SetUnProcConfinfoFileData] SetUnProConfDataToFile Err: Loop.%d\n", byLoop);
			}
			pbyConfDataBuf += wConfDataLen;
		}
	}
	return TRUE;
}
/*=============================================================================
    函 数 名： GetVCSLoguserFileData
    功    能： 主备数据倒换时 获取 mcu/data下的 vcslogin.usr 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2009/04/03  4.0			fxh                   创建
=============================================================================*/
BOOL32 CMcuVcData::GetVCSLoguserFileData(u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen)
{
	if (NULL == pbyBuf)
	{
		OspPrintf(TRUE, FALSE, "[GetVCSLoguserFileData] NULL == pbyBuf\n");
		return FALSE;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, MCU_VCSUSER_FILENAME);
	
	FILE *hFile  = fopen(achFullName, "rb");
	s32 nFileLen = 0;
	if (NULL != hFile)
	{
		fseek(hFile, 0, SEEK_END);
		nFileLen = ftell(hFile);
		if (nFileLen > (s32)dwInBufLen)
		{
			OspPrintf(TRUE, FALSE, "[GetVCSLoguserFileData] vcslogin.usr FileLen(%d)>%d\n", nFileLen, dwInBufLen);
			fclose(hFile);
			hFile = NULL;
			return FALSE;
		}

		if (nFileLen > 0)
		{
			fseek(hFile, 0, SEEK_SET);
			fread(pbyBuf, nFileLen, 1, hFile);
		}
		fclose(hFile);
		hFile = NULL;
		dwOutBufLen = (u32)nFileLen;
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[GetVCSLoguserFileData] open vcslogin.usr Fail\n");
		return FALSE;
	}
	return TRUE;
}
/*=============================================================================
    函 数 名： SetVCSLoguserFileData
    功    能： 主备数据倒换时 保存mcu/data下的 vcslogin.usr 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/06/22  4.0			fxh                   创建
=============================================================================*/
BOOL32 CMcuVcData::SetVCSLoguserFileData(u8 *pbyBuf, u32 dwInBufLen)
{
	BOOL32 bRet = FALSE;
	if (NULL == pbyBuf || dwInBufLen < 0)
	{
		OspPrintf(TRUE, FALSE, "[SetVCSLoguserFileData] NULL == pbyBuf || InBufLen(%d) < 0 \n", dwInBufLen);
		return bRet;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, MCU_VCSUSER_FILENAME);
	
	BOOL32 bReWrite = TRUE;
	if (0 == dwInBufLen)
	{
		FILE *hFile = fopen(achFullName, "rb");
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
		else
		{
			bReWrite = FALSE;
			bRet = TRUE;
		}
	}

	if (bReWrite)
	{
		FILE *hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(pbyBuf, 1, dwInBufLen, hFile);
#ifdef WIN32    //VX下不作多余的 fflush 操作
				fflush(hFile);
#endif
			}
			fclose(hFile);
			hFile = NULL;
			bRet = TRUE;
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[SetVCSLoguserFileData] fopen vcslogin.usr failed\n");
		}
	}

	return bRet;
}
/*=============================================================================
    函 数 名： GetVCSUserTaskFileData
    功    能： 主备数据倒换时 获取 mcu/data下的 usrtask.dat 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [OUT] u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2009/04/03  4.0			fxh                   创建
=============================================================================*/
BOOL32 CMcuVcData::GetVCSUserTaskFileData(u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen)
{
	if (NULL == pbyBuf)
	{
		OspPrintf(TRUE, FALSE, "[GetVCSUserTaskFileData] NULL == pbyBuf\n");
		return FALSE;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, USERTASKINFOFILENAME);
	
	FILE *hFile  = fopen(achFullName, "rb");
	s32 nFileLen = 0;
	if (NULL != hFile)
	{
		fseek(hFile, 0, SEEK_END);
		nFileLen = ftell(hFile);
		if (nFileLen > (s32)dwInBufLen)
		{
			OspPrintf(TRUE, FALSE, "[GetVCSUserTaskFileData] usrtask.dat FileLen(%d)>%d\n", nFileLen, dwInBufLen);
			return FALSE;
		}
		if (nFileLen > 0)
		{
			fseek(hFile, 0, SEEK_SET);
			fread(pbyBuf, nFileLen, 1, hFile);
		}
		fclose(hFile);
		hFile = NULL;
		dwOutBufLen = (u32)nFileLen;
	}
	return TRUE;
}
/*=============================================================================
    函 数 名： SetVCSUserTaskFileData
    功    能： 主备数据倒换时 保存mcu/data下的 usrtask.dat 文件信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/06/22  4.0			fxh                   创建
=============================================================================*/
BOOL32 CMcuVcData::SetVCSUserTaskFileData(u8 *pbyBuf, u32 dwInBufLen)
{
	BOOL32 bRet = FALSE;
	if (NULL == pbyBuf || dwInBufLen < 0)
	{
		OspPrintf(TRUE, FALSE, "[SetVCSUserTaskFileData] NULL == pbyBuf || InBufLen(%d) < 0 \n", dwInBufLen);
		return bRet;
	}
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_DATA, USERTASKINFOFILENAME);
	
	BOOL32 bReWrite = TRUE;
	if (0 == dwInBufLen)
	{
		FILE *hFile = fopen(achFullName, "rb");
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
		else
		{
			bReWrite = FALSE;
			bRet = TRUE;
		}
	}

	if (bReWrite)
	{
		FILE *hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			if (0 != dwInBufLen)
			{
				fwrite(pbyBuf, 1, dwInBufLen, hFile);
#ifdef WIN32    //VX下不作多余的 fflush 操作
				fflush(hFile);
#endif
			}
			fclose(hFile);
			hFile = NULL;
			bRet = TRUE;
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[SetVCSUserTaskFileData] save usrtask.dat failed\n");
		}
	}

	return bRet;
}
/*=============================================================================
    函 数 名： ReloadMcuUserList
    功    能： 用户列表模块重新从文件装载用户列表
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/01/05  4.0			万春雷                  创建
    2006/06/22  4.0         顾振华                  增加用户组支持
=============================================================================*/
BOOL32 CMcuVcData::ReloadMcuUserList( void )
{
	BOOL32 bRet = g_cUsrManage.GetAllUserFromFile();    
    g_cUsrManage.SetSingle(FALSE);

    CUserFullInfo cUserInfo;

    s32 nNum = g_cUsrManage.GetUserNum();
    if (nNum == 0)
    {
        printf("[UserMgrLoad]Load User ret: %d. User Number: %d\n", bRet, nNum);
    }    

    // 处理一下admin

    for (s32 nLoop = 0; nLoop < nNum; nLoop ++)
    {        
        g_cUsrManage.GetUserFullInfo( &cUserInfo,  nLoop );

        if ( cUserInfo.IsEqualName("admin") )
        {
            // 如果是admin的话，需要过滤一下。可能是自动生成的
            // 要给它加上组标记
            CExUsrInfo cNewExnfo = cUserInfo;

            char *szDesc = cUserInfo.GetDiscription();
            if ( szDesc !=  NULL &&
                 (u8)szDesc[0] != USRGRPID_SADMIN )
            {
                cNewExnfo.SetDiscription( szDesc );
                cNewExnfo.SetUsrGrpId( USRGRPID_SADMIN );
                if (!g_cUsrManage.ModifyInfo( &cNewExnfo ))
                {
                    OspPrintf(TRUE, FALSE, "[ReloadMcuUserList] Modify Admin error: %d!\n", g_cUsrManage.GetLastError());
                }
            }

            break;
        }
    }

    // 读取用户组信息
    bRet = bRet & m_cUsrGrpInfo.Load();

    return bRet;
}

/*=============================================================================
  函 数 名： GetMcuEqpCapacity
  功    能： 
  算法实现： 
  全局变量： 
  参    数： [OUT] TEqpCapacity& tMcuEqpCap
  返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/02/25  4.0			刘辉云                  创建
=============================================================================*/
void CMcuVcData::GetMcuEqpCapacity(TEqpCapacity& tMcuEqpCap)
{

	u8  byVmpNum = 0;
	u8  byMixNum = 0;
	u8  byTvNum = 0;
	u8  byMpwNum = 0;
	u8 byEqpIdLp = 0;
	
	TBasReqInfo tBasCap;
	GetBasCapacity( tBasCap );  // 取Bas空闲通道数

    // 取高清BAS空闲通道数
    THDBasReqInfo tHDBasCap;
    GetHDBasCapacity( tHDBasCap );

	TEqpReqInfo tPrsCap;
	GetPrsCapacity( tPrsCap ); // 取Prs空闲通道数
	
	// 电视墙空闲通道数 ?
	
    while( byEqpIdLp < MAXNUM_MCU_PERIEQP )
	{
		if( IsPeriEqpValid( byEqpIdLp )
			&& m_atPeriEqpTable[byEqpIdLp-1].m_tPeriEqpStatus.m_byOnline )
		{
			// mix
			if( EQP_TYPE_MIXER == GetEqpType(byEqpIdLp) )
			{
                //zbq[04/10/2008] 混音组没有准备好不认为该混音器可用. 只记录数量会误判.
                TPeriEqpStatus tEqpStatus;
                if (!GetPeriEqpStatus(byEqpIdLp, &tEqpStatus))
                {
                    OspPrintf(TRUE, FALSE, "[GetMcuEqpCapacity] get Mixer.%d's status failed\n", byEqpIdLp);
                    continue;
                }
                u8 byLoopGrpId = 0;
                TMixerStatus tMixerStatus = tEqpStatus.m_tStatus.tMixer;
                while( byLoopGrpId < tMixerStatus.m_byGrpNum )
                {
                    if( tMixerStatus.m_atGrpStatus[byLoopGrpId].m_byGrpState == TMixerGrpStatus::READY )
                    {
                        byMixNum++;
                    }
                    byLoopGrpId++;				
                }

				//byMixNum++; // 只记录了混音器数量
			}

			// recorder 
			if( EQP_TYPE_RECORDER == GetEqpType(byEqpIdLp) )
			{
				// do nothing;
			}
						
			// tv
			if( EQP_TYPE_TVWALL == GetEqpType(byEqpIdLp) )
			{
				byTvNum++;
			}
						
			// bas
			if( EQP_TYPE_BAS == GetEqpType(byEqpIdLp) )
			{
				// do nothing;
			}
			
			// vmp
			if(EQP_TYPE_VMP == GetEqpType( byEqpIdLp )
				&& m_atPeriEqpTable[byEqpIdLp-1].m_tPeriEqpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::IDLE )
			{
				byVmpNum++;
			}

			// mpw 
			if( EQP_TYPE_VMPTW == GetEqpType( byEqpIdLp) )
			{
				byMpwNum++;
			}
			// prs
			if( EQP_TYPE_PRS == GetEqpType( byEqpIdLp ) )
			{
				// get prs channels
			}
		}		
        byEqpIdLp++;
	}

    // mix
	if( 0 != byMixNum )
	{
		tMcuEqpCap.m_tMixCap.SetNeedEqp(TRUE);
		tMcuEqpCap.m_tMixCap.SetNeedChannles( byMixNum );
	}
	else
	{
		tMcuEqpCap.m_tMixCap.SetNeedEqp(FALSE);
	}
	// tvwall
	if( 0 != byTvNum )
	{
		tMcuEqpCap.m_tTvCap.SetNeedEqp(TRUE);
		tMcuEqpCap.m_tTvCap.SetNeedChannles( byTvNum );
	}
	else
	{
		tMcuEqpCap.m_tTvCap.SetNeedEqp(FALSE);
	}
	// vmp
	if( 0!= byVmpNum)
	{
		tMcuEqpCap.m_tVmpCap.SetNeedEqp( TRUE );
		tMcuEqpCap.m_tVmpCap.SetNeedChannles( byVmpNum );
	}
	else
	{
		tMcuEqpCap.m_tVmpCap.SetNeedEqp( FALSE );
	}
	// mpw
	if( 0 != byMpwNum )
	{
		tMcuEqpCap.m_tMpwCap.SetNeedEqp( TRUE );
		tMcuEqpCap.m_tMpwCap.SetNeedChannles( byMpwNum );
	}
	else
	{
		tMcuEqpCap.m_tMpwCap.SetNeedEqp( FALSE );
	}
	
	// bas
	tMcuEqpCap.m_tBasCap = tBasCap;

    // HDBas
    tMcuEqpCap.m_tHDBasCap = tHDBasCap;

	// prs
	tMcuEqpCap.m_tPrsCap = tPrsCap;

	return;
}

/*=============================================================================
  函 数 名： GetConfEqpDemand
  功    能： 
  算法实现： 
  全局变量： 
  参    数： [IN] TConfInfo& tConfInfo
             [OUT] TEqpCapacity& tConfEqpDemand
  返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/02/25  4.0			刘辉云                  创建
=============================================================================*/
void CMcuVcData::GetConfEqpDemand(TConfInfo& tConfInfo, TEqpCapacity& tConfEqpDemand )
{
	TConfAttrb tConfAttribute = tConfInfo.GetConfAttrb();
	tConfEqpDemand.m_tVmpCap.SetNeedEqp( tConfAttribute.IsHasVmpModule() || CONF_VMPMODE_AUTO == tConfInfo.m_tStatus.GetVMPMode() ); 
	tConfEqpDemand.m_tPrsCap.SetNeedEqp( tConfAttribute.IsResendLosePack() );// prs

    // 会议BAS和高清BAS需求, zgc, 2008-08-11
    BOOL32 bIsNeedBas = FALSE;
    BOOL32 bIsNeedHDBas = FALSE;
    if ( IsHDConf(tConfInfo) && tConfAttribute.IsUseAdapter() )
    {   
        bIsNeedHDBas = TRUE;
    }
    tConfEqpDemand.m_tHDBasCap.SetNeedEqp( bIsNeedHDBas );
    
    if ( tConfAttribute.IsUseAdapter() &&
         (( tConfInfo.GetSecAudioMediaType() != MEDIA_TYPE_NULL && 
           tConfInfo.GetMainAudioMediaType() != tConfInfo.GetSecAudioMediaType() ) ||
          ( tConfInfo.GetSecVideoMediaType() != MEDIA_TYPE_NULL &&
            tConfInfo.GetMainVideoMediaType() != tConfInfo.GetSecVideoMediaType() ) ||
          ( tConfInfo.GetSecBitRate() != 0 && 
            tConfInfo.GetBitRate() != tConfInfo.GetSecBitRate())  ) )
    {
        bIsNeedBas = TRUE;
    }
    tConfEqpDemand.m_tBasCap.SetNeedEqp( FALSE );
	//tConfEqpDemand.m_tBasCap.SetNeedEqp( tConfAttribute.IsUseAdapter() ); // bas
	tConfEqpDemand.m_tMixCap.SetNeedEqp( tConfAttribute.IsDiscussConf()  ); // mixer

	tConfEqpDemand.m_tMpwCap.SetNeedEqp( FALSE );
	tConfEqpDemand.m_tRecCap.SetNeedEqp( FALSE );

	// tvwall(多电视墙没有考虑) (电视墙通道数不在会议信息中，所以在外面判断)
	tConfEqpDemand.m_tTvCap.SetNeedEqp( tConfAttribute.IsHasTvWallModule() ); // tv
	
	u8 byNeedPrsChannles = DEFAULT_PRS_CHANNELS;
	u8 byNeedVidBasChannels = 0;
	u8 byNeedAudBasChannels = 0;

	// bas (具体通道数目)
    // guzh [8/28/2007] 如果是 8000B/8000C，则检查是否是双媒体会议（只需要1路视频适配)
#ifdef _MINIMCU_
    BOOL32 bNeedBas = FALSE;
#endif
    // 高清BAS和普通BAS分开, zgc, 2008-08-11
	//if( tConfAttribute.IsUseAdapter() )
    if ( bIsNeedBas )
	{
		u8 byMainMediaType   = tConfInfo.GetMainAudioMediaType();
		u8 bySecondMediaType = tConfInfo.GetSecAudioMediaType();
		if (MEDIA_TYPE_NULL != bySecondMediaType && byMainMediaType != bySecondMediaType)
		{
			byNeedPrsChannles++;
			byNeedAudBasChannels++;
		}

		byMainMediaType = tConfInfo.GetMainVideoMediaType();
		bySecondMediaType = tConfInfo.GetSecVideoMediaType();
		u8 byMainVidFormat = tConfInfo.GetMainVideoFormat();
		u8 bySecVidFormat = tConfInfo.GetSecVideoFormat();
		if ( (MEDIA_TYPE_NULL != bySecondMediaType && byMainMediaType != bySecondMediaType)
			||(byMainMediaType == bySecondMediaType && byMainVidFormat != bySecVidFormat) )
		{
			byNeedPrsChannles++;
#ifdef _MINIMCU_
            bNeedBas = TRUE;
#else
			byNeedVidBasChannels++;
#endif
		}

		if( 0 != tConfInfo.GetSecBitRate() )
		{
			byNeedPrsChannles++;
#ifdef _MINIMCU_
            bNeedBas = TRUE;
#else
			byNeedVidBasChannels++;
#endif
		}

		tConfEqpDemand.m_tBasCap.SetAudChannels( byNeedAudBasChannels );
#ifdef _MINIMCU_
        tConfEqpDemand.m_tBasCap.SetNeedChannles( bNeedBas ? 1: 0 );
#else
		tConfEqpDemand.m_tBasCap.SetNeedChannles( byNeedVidBasChannels );
#endif
	}

    // 高清BAS
    if ( bIsNeedHDBas )
    {
        u8 byNVChn = 0;
        u8 byDSChnVGA = 0;
        u8 byDSChnH263p = 0;

        CBasMgr cBasMgr;
        cBasMgr.GetNeededMau(tConfInfo, byNVChn, byDSChnH263p, byDSChnVGA);

        tConfEqpDemand.m_tHDBasCap.SetNeedChannles( byNVChn );
        tConfEqpDemand.m_tHDBasCap.SetDVidChannels( byDSChnVGA );
        tConfEqpDemand.m_tHDBasCap.SetDVidH263pChn( byDSChnH263p );
    }

	// prs (具体通道数目)
	if(tConfAttribute.IsResendLosePack())
	{
		tConfEqpDemand.m_tPrsCap.SetNeedChannles( byNeedPrsChannles );
	}
	return;
}

/*=============================================================================
  函 数 名： AnalyEqpCapacity
  功    能： 
  算法实现： 
  全局变量： 
  参    数：  [IN] TEqpCapacity& tConfEqpDemand
              [IN] TEqpCapacity& tMcuSupportCap
  返 回 值： BOOL32: TRUE: Mcu支持该会议的外设需求， FALSE；不支持
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/02/25  4.0			刘辉云                  创建
=============================================================================*/
BOOL32 CMcuVcData::AnalyEqpCapacity( TEqpCapacity& tConfEqpDemand, TEqpCapacity& tMcuSupportCap)
{
	BOOL32 bRet = TRUE;
	// bas
	if( tConfEqpDemand.m_tBasCap.IsNeedEqp() ) // need bas
	{
		if( tMcuSupportCap.m_tBasCap.IsNeedEqp() ) // support bas
		{
			if( tMcuSupportCap.m_tBasCap.GetAudChannels() < tConfEqpDemand.m_tBasCap.GetAudChannels() ||
				tMcuSupportCap.m_tBasCap.GetSupportChannels() < tConfEqpDemand.m_tBasCap.GetSupportChannels() )
			{
				bRet = FALSE;
			}
		}
		else
		{
			bRet = FALSE;
		}
	}

    // hdbas, zgc, 2008-08-11
    if( tConfEqpDemand.m_tHDBasCap.IsNeedEqp() ) // need bas
    {
        if( tMcuSupportCap.m_tHDBasCap.IsNeedEqp() ) // support bas
        {
            //优先算MPU是否满足，再算MAU是否满足，和占用的顺序保持对齐
            if (tMcuSupportCap.m_tHDBasCap.GetMpuChn() > 0 &&
                tMcuSupportCap.m_tHDBasCap.GetMpuChn() >= ( tConfEqpDemand.m_tHDBasCap.GetDVidChannels() +
                                                            tConfEqpDemand.m_tHDBasCap.GetSupportChannels() + 
                                                            tConfEqpDemand.m_tHDBasCap.GetDVidH263pChn() )
               )
            {
                bRet = TRUE;
            }
            else if( tMcuSupportCap.m_tHDBasCap.GetDVidChannels() >= tConfEqpDemand.m_tHDBasCap.GetDVidChannels() &&
                     tMcuSupportCap.m_tHDBasCap.GetSupportChannels() >= tConfEqpDemand.m_tHDBasCap.GetSupportChannels() &&
                     tMcuSupportCap.m_tHDBasCap.GetDVidH263pChn() >= tConfEqpDemand.m_tHDBasCap.GetDVidH263pChn())
            {
                bRet = TRUE;
            }
            else
            {
                bRet = FALSE;
            }
        }
        else
        {
            bRet = FALSE;
        }
	}

	// prs
	if( tConfEqpDemand.m_tPrsCap.IsNeedEqp() )
	{
		if( tMcuSupportCap.m_tPrsCap.IsNeedEqp() )
		{
			if( tMcuSupportCap.m_tPrsCap.GetSupportChannels() < tConfEqpDemand.m_tPrsCap.GetSupportChannels() )
			{
				bRet = FALSE;
			}
		}
		else
		{
			bRet = FALSE;
		}
	}
	
	// tv
	if( tConfEqpDemand.m_tTvCap.IsNeedEqp() )
	{
		if( !tMcuSupportCap.m_tTvCap.IsNeedEqp() )
		{
			bRet = FALSE;
		}
	}

	// vmp
	if( tConfEqpDemand.m_tVmpCap.IsNeedEqp() )
	{
		if( !tMcuSupportCap.m_tVmpCap.IsNeedEqp() )
		{
			bRet = FALSE;
		}
	}

	// mixer
	if( tConfEqpDemand.m_tMixCap.IsNeedEqp()  )
	{
		if( !tMcuSupportCap.m_tMixCap.IsNeedEqp() )
		{
			bRet = FALSE;
		}
	}
/*
	// mpw
	if( tConfEqpDemand.m_tMpwCap.IsNeedEqp()  )
	{
		if( !tMcuSupportCap.m_tMpwCap.IsNeedEqp() )
		{
			bRet = FALSE;
		}
	}
*/
	return bRet;
}

/*=============================================================================
  函 数 名： GetBasCapacity
  功    能： 取Bas能力集
  算法实现： 
  全局变量： 
  参    数： TBasReqInfo& tBasCap
  返 回 值： void 
=============================================================================*/
void CMcuVcData::GetBasCapacity(TBasReqInfo& tBasCap )
{
	u8 byAudChannels = 0;
	u8 byVidChannels = 0;
	u8 byBasIdLp = BASID_MIN;
	TBasStatus *ptStatus = NULL;
	
	while( byBasIdLp >= BASID_MIN && byBasIdLp <= BASID_MAX )
	{
		if( IsPeriEqpValid( byBasIdLp )
            && !g_cMcuAgent.IsEqpBasHD(byBasIdLp)
			&& m_atPeriEqpTable[byBasIdLp-1].m_tPeriEqpStatus.m_byOnline )
		{
			ptStatus = &(m_atPeriEqpTable[byBasIdLp-1].m_tPeriEqpStatus.m_tStatus.tBas);
			u8 byMinChnl = 0;
			u8 byMaxChnl = ptStatus->byChnNum;

			//查找空闲通道
			for (u8 byChnIdx = byMinChnl; byChnIdx < byMaxChnl; byChnIdx++)
			{
				if (!ptStatus->tChnnl[byChnIdx].IsReserved())
				{
					u8 byBasChnnlType = ptStatus->tChnnl[byChnIdx].GetChannelType();
					if (BAS_CHAN_AUDIO == byBasChnnlType)
					{
						byAudChannels++;
					}
					else if (BAS_CHAN_VIDEO == byBasChnnlType)
					{
						byVidChannels++;
					}
				}
			}
		}
		byBasIdLp++;
	}
	
	tBasCap.SetNeedEqp(FALSE);
	tBasCap.SetAudChannels( 0 );
	tBasCap.SetNeedChannles( 0 );

	if( 0 != byAudChannels )
	{
		tBasCap.SetNeedEqp(TRUE);
		tBasCap.SetAudChannels( byAudChannels );
	}

	if( 0!= byVidChannels )
	{
		tBasCap.SetNeedEqp( TRUE );
		tBasCap.SetNeedChannles( byVidChannels );
	}

	return;
}

/*=============================================================================
函 数 名： GetHDBasCapacity
功    能： 
算法实现： 
全局变量： 
参    数： THDBasReqInfo& tHDBasCap
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/8/11   4.0		    周广程                  创建
2008/12/12  4.5         张宝卿                  增加H263p处理
=============================================================================*/
void CMcuVcData::GetHDBasCapacity( THDBasReqInfo& tHDBasCap )
{
    u8 byBasIdLp = BASID_MIN;
	THdBasStatus *ptStatus = NULL;

    u8 byHDBasVidChnnls = 0;
    u8 byHDBasDVidChnnls = 0;
    u8 byHDBasDVidH263p = 0;
    u8 byHDBasMpuChns  = 0;
    
    u8 byLoop = 0;
    while( byBasIdLp >= BASID_MIN && byBasIdLp <= BASID_MAX )
    {
        if( IsPeriEqpValid( byBasIdLp ) &&
            g_cMcuAgent.IsEqpBasHD( byBasIdLp ) &&
            m_atPeriEqpTable[byBasIdLp-1].m_tPeriEqpStatus.m_byOnline )
        {
            ptStatus = &(m_atPeriEqpTable[byBasIdLp-1].m_tPeriEqpStatus.m_tStatus.tHdBas);

            if (TYPE_MAU_NORMAL == ptStatus->GetEqpType())
            {
                for ( byLoop = 0; byLoop < MAXNUM_MAU_VCHN; byLoop++ )
                {
                    if (!ptStatus->tStatus.tMauBas.GetVidChnStatus( byLoop )->IsReserved() &&
                        !ptStatus->tStatus.tMauBas.GetVidChnStatus( byLoop )->IsTempReserved() )
                    {
                        byHDBasVidChnnls++;
                    }
                }
                for ( byLoop = 0; byLoop < MAXNUM_MAU_DVCHN; byLoop++ )
                {
                    if (!ptStatus->tStatus.tMauBas.GetDVidChnStatus( byLoop )->IsReserved() &&
                        !ptStatus->tStatus.tMauBas.GetDVidChnStatus( byLoop )->IsTempReserved() )
                    {
                        byHDBasDVidChnnls++;
                    }
                }
            }
            else if(TYPE_MAU_H263PLUS == ptStatus->GetEqpType())
            {
                byHDBasDVidH263p ++;
            }
            else if (TYPE_MPU == ptStatus->GetEqpType())
            {
                for ( byLoop = 0; byLoop < MAXNUM_MPU_VCHN; byLoop++ )
                {
                    if (!ptStatus->tStatus.tMpuBas.GetVidChnStatus( byLoop )->IsReserved() &&
                        !ptStatus->tStatus.tMpuBas.GetVidChnStatus( byLoop )->IsTempReserved() )
                    {
                        byHDBasMpuChns++;
                    }
                }
            }
        }
        byBasIdLp++;
	}

    tHDBasCap.SetNeedEqp( FALSE );
    tHDBasCap.SetNeedChannles( 0 );
    tHDBasCap.SetDVidChannels( 0 );
    tHDBasCap.SetMpuChn(0);

    if ( byHDBasVidChnnls > 0 )
    {
        tHDBasCap.SetNeedEqp( TRUE );
        tHDBasCap.SetNeedChannles( byHDBasVidChnnls );
    }
    if ( byHDBasDVidChnnls > 0 )
    {
        tHDBasCap.SetNeedEqp( TRUE );
        tHDBasCap.SetDVidChannels( byHDBasDVidChnnls );
    }
    if (byHDBasDVidH263p > 0)
    {
        tHDBasCap.SetNeedEqp( TRUE );
        tHDBasCap.SetDVidH263pChn( byHDBasDVidH263p );
    }
    if (byHDBasMpuChns > 0)
    {
        tHDBasCap.SetNeedEqp( TRUE );
        tHDBasCap.SetMpuChn(byHDBasMpuChns);
    }

    return;
}

/*=============================================================================
  函 数 名： GetPrsCapacity
  功    能： 取Prs能力集
  算法实现： 
  全局变量： 
  参    数： TEqpReqInfo& tPrsCap
  返 回 值： void 
=============================================================================*/
void CMcuVcData::GetPrsCapacity(TEqpReqInfo& tPrsCap )
{
	u8 byPrsIdLp = PRSID_MIN;
	u8 byPrsChannels = 0;

	tPrsCap.SetNeedEqp( FALSE );
	tPrsCap.SetNeedChannles( 0 );

	TPrsStatus *ptStatus = NULL;
	while (byPrsIdLp >= PRSID_MIN && byPrsIdLp <= PRSID_MAX)
    {
        if ( 1 == m_atPeriEqpTable[byPrsIdLp-1].m_tPeriEqpStatus.m_byOnline ) // 在线
		{
			if (IsPeriEqpValid(byPrsIdLp) 
				&& EQP_TYPE_PRS == GetEqpType(byPrsIdLp) ) // 合法性检测
			{
				ptStatus = &(m_atPeriEqpTable[byPrsIdLp - 1].m_tPeriEqpStatus.m_tStatus.tPrs);
				byPrsChannels += ptStatus->GetIdleChlNum();
            }
        }
		byPrsIdLp++;
    }

	if( 0 != byPrsChannels )
	{
		tPrsCap.SetNeedEqp(TRUE);
		tPrsCap.SetNeedChannles( byPrsChannels ); 
	}
	return;
}

/*=============================================================================
  函 数 名： GetConfFECType
  功    能： 
  算法实现： 
  全局变量： 
  参    数： [IN] u8 byConfIdx
             [IN] u8 byMode
  返 回 值： u8: 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2007/10/11  4.0			张宝卿                创建
=============================================================================*/
u8 CMcuVcData::GetConfFECType( u8 byConfIdx, u8 byMode )
{
    u8 byFECType = FECTYPE_NONE;

    if ( byConfIdx > MAX_CONFIDX ) 
    {
        OspPrintf( TRUE, FALSE, "[GetConfFECType] ConfIdx.%d, unexpected, ignore it\n", byConfIdx );
        return byFECType;
    }

    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if(!tMapData.IsValidConf()) 
    {
        OspPrintf(TRUE, FALSE, "[GetConfFECType] invalid instance id :%d\n", tMapData.GetInsId());
        return byFECType;
    }
    
    if ( NULL == m_apConfInst[tMapData.GetInsId()-1] )
    {
        OspPrintf(TRUE, FALSE, "[GetConfFECType] Inst.%d has no conf, unexpected\n", tMapData.GetInsId());
        return byFECType;
    }
    switch( byMode )
    {
    case MODE_AUDIO:
        byFECType = m_apConfInst[tMapData.GetInsId()-1]->m_tConf.GetCapSupportEx().GetAudioFECType();
        break;

    case MODE_VIDEO:
        byFECType = m_apConfInst[tMapData.GetInsId()-1]->m_tConf.GetCapSupportEx().GetVideoFECType();
    	break;

    case MODE_BOTH:
        {
        u8 byFECTypeA = m_apConfInst[tMapData.GetInsId()-1]->m_tConf.GetCapSupportEx().GetAudioFECType();
        u8 byFECTypeV = m_apConfInst[tMapData.GetInsId()-1]->m_tConf.GetCapSupportEx().GetVideoFECType();
        byFECType = byFECTypeA == byFECTypeV ? byFECTypeV : FECTYPE_NONE;
        }
        break;

    case MODE_SECVIDEO:
        byFECType = m_apConfInst[tMapData.GetInsId()-1]->m_tConf.GetCapSupportEx().GetDVideoFECType();
        break;

    default:
        OspPrintf( TRUE, FALSE, "[GetConfFECType] unexpected mode.%d, ignore it\n", byMode );
        break;
    }

    return byFECType;
}

/*=============================================================================
  函 数 名： GetConfEncryptMode
  功    能： 
  算法实现： 
  全局变量： 
  参    数： [IN] u8 byConfIdx
  返 回 值： u8: 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2007/10/16  4.0			张宝卿                创建
=============================================================================*/
u8 CMcuVcData::GetConfEncryptMode( u8 byConfIdx )
{
    u8 byEncMode = CONF_ENCRYPTMODE_NONE;

    if ( byConfIdx > MAX_CONFIDX ) 
    {
        OspPrintf( TRUE, FALSE, "[GetConfEncryptMode] ConfIdx.%d, unexpected, ignore it\n", byConfIdx );
        return byEncMode;
    }
    TConfMapData tMapData = m_atConfMapData[byConfIdx-MIN_CONFIDX];
    if(!tMapData.IsValidConf()) 
    {
        OspPrintf(TRUE, FALSE, "[GetConfEncryptMode] invalid instance id :%d\n", tMapData.GetInsId());
        return byEncMode;
    }
    
    if ( NULL == m_apConfInst[tMapData.GetInsId()-1] )
    {
        OspPrintf( TRUE, FALSE, "[GetConfEncryptMode] ConfIdx.%d unexist, ignore it\n", byConfIdx );
        return byEncMode;
    }

    return m_apConfInst[tMapData.GetInsId()-1]->m_tConf.GetMediaKey().GetEncryptMode();
}

/********************************************

        用户组扩展

  *******************************************/


/*=============================================================================
  函 数 名： GetGrpById
  功    能：根据GroupId查找组，没找到返回False
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32       FALSE 没有找到
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
=============================================================================*/
BOOL32 CUsrGrpsInfo::GetGrpById(u8 byGrpId, TUsrGrpInfo &tGrpInfo) const
{
    tGrpInfo.SetNull();
    if (byGrpId == USRGRPID_INVALID ||
        byGrpId > MAXNUM_USRGRP)
    {
        // 非法
        return FALSE;
    }
    tGrpInfo = m_atInfo[byGrpId-1];
    return TRUE;
    
    /*
    for ( s32 nLoop = 0; nLoop < m_nGrpNum; nLoop ++ )
    {
        if ( m_atInfo[ nLoop ].GetUsrGrpId() == byGrpId )
        {
            tGrpInfo = m_atInfo[nLoop];
            return TRUE;
        }
    }    
    */
    
}


/*=============================================================================
  函 数 名： GetGrpByName
  功    能：根据组名查找组，没找到返回False
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32       FALSE 没有找到
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
=============================================================================*/
BOOL32 CUsrGrpsInfo::GetGrpByName(LPCSTR szName,  TUsrGrpInfo &tGrpInfo) const
{
    tGrpInfo.SetNull();
    for ( s32 nLoop = 0; nLoop < MAXNUM_USRGRP; nLoop ++ )
    {
        if ( strncmp( m_atInfo[ nLoop ].GetUsrGrpName(), szName, MAX_CHARLENGTH ) == 0 )
        {
            tGrpInfo = m_atInfo[nLoop];
            return TRUE;
        }
    }    
    return FALSE;    
}


/*=============================================================================
  函 数 名： AddGrp
  功    能：添加组。返回组Id，已满返回 USRGRPID_INVALID
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u8         组Id
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
    06/09/07    4.0         顾振华        调整策略，允许用户自己配制组ID
=============================================================================*/
u8 CUsrGrpsInfo::AddGrp ( const TUsrGrpInfo &tGrpInfo )
{
/*
    if ( m_nGrpNum >= MAXNUM_USRGRP )
    {
        return USRGRPID_INVALID;
    }

    TUsrGrpInfo tTargetInfo = tGrpInfo;
    // 查找第一个可用的组ID
    tTargetInfo.byGrpId = USRGRPID_INVALID;
    
    BOOL32 bIdUsed[MAXNUM_USRGRP + 1];
    memset( &bIdUsed, 0, sizeof(bIdUsed) );
    u8 byLoop = 0;
    for (; byLoop < m_nGrpNum; byLoop ++)
    {
        bIdUsed [ m_atInfo[byLoop].byGrpId ] = TRUE;
    }

    for ( byLoop = 1; byLoop < MAXNUM_USRGRP + 1; byLoop ++)
    {
        if ( !bIdUsed[byLoop] )
        {
            // 第一个未使用的
            break;
        }
    }

    if ( byLoop > MAXNUM_USRGRP )
    {
        // 没有空闲，应该不可能
        return USRGRPID_INVALID;           
    }
    
    tTargetInfo.byGrpId = byLoop;
    m_atInfo[m_nGrpNum] = tTargetInfo;
    m_nGrpNum ++;
    return tTargetInfo.byGrpId;
*/
    
    u8 byTargetId = tGrpInfo.GetUsrGrpId();
    TUsrGrpInfo tOldOne;
    if ( GetGrpById(byTargetId, tOldOne) )
    {
        if (!tOldOne.IsFree())
        {
            // 组ID已用
            return USRGRPID_INVALID;
        }
    }
    else
    {
        // 非法
        return USRGRPID_INVALID;
    }

    // 保存
    m_atInfo[byTargetId-1] = tGrpInfo;
    return byTargetId;
}


/*=============================================================================
  函 数 名： ChgGrp
  功    能：修改组操作，返回是否找到
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
=============================================================================*/
BOOL32 CUsrGrpsInfo::ChgGrp ( const TUsrGrpInfo &tGrpInfo )
{
    TUsrGrpInfo tOldOne;
    if ( !GetGrpById(tGrpInfo.GetUsrGrpId(), tOldOne) ) 
    {
        return FALSE;
    }
    if (tOldOne.IsFree())
    {
        return FALSE;
    }

    m_atInfo[tGrpInfo.GetUsrGrpId()-1] = tGrpInfo;

    return TRUE;
}

/*=============================================================================
  函 数 名： DelGrp
  功    能： 删除组操作。不进行相关其他资源释放操作。返回是否找到
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
=============================================================================*/
BOOL32 CUsrGrpsInfo::DelGrp ( u8 byGrpId )
{
    /*
    for (s32 nLoop = 0; nLoop < m_nGrpNum; nLoop ++)
    {
        if ( m_atInfo[nLoop].byGrpId == byGrpId )
        {
            // 找到则删除
            // 移动后面的
            for ( s32 nLoop2 = nLoop + 1; nLoop2 < m_nGrpNum; nLoop2 ++ )
            {
                m_atInfo[nLoop2 - 1] = m_atInfo[nLoop2];
            }

            m_atInfo[m_nGrpNum-1].SetNull();
            m_nGrpNum --;

            return TRUE;
        }
    }
    return FALSE;    
    */

    TUsrGrpInfo tOldOne;
    if ( !GetGrpById(byGrpId, tOldOne) ) 
    {
        return FALSE;
    }
    if (tOldOne.IsFree())
    {
        return FALSE;
    }

    m_atInfo[byGrpId-1].SetNull();
    return TRUE;
}

/*=============================================================================
  函 数 名： Save
  功    能： 保存到文件
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
=============================================================================*/
BOOL32 CUsrGrpsInfo::Save() const
{
    s8 szPath[KDV_MAX_PATH] = {0};
    
    sprintf(szPath, "%s/%s", DIR_DATA, USERGRPINFOFILENAME);

    FILE *fp;
    fp = fopen( szPath, "wb" );
    if (fp == NULL)
    {
        return FALSE;
    }

    //fwrite( &m_nGrpNum, sizeof(s32), 1, fp );  
    //fwrite( m_atInfo, sizeof( TUsrGrpInfo ), m_nGrpNum, fp );
    s32 nTmp = MAXNUM_USRGRP;
    fwrite( &nTmp, sizeof(s32), 1, fp );  
    fwrite( m_atInfo, sizeof( TUsrGrpInfo ), MAXNUM_USRGRP, fp );

#ifdef WIN32
    fflush(fp);
#endif

    fclose(fp);

    return TRUE;       
}


/*=============================================================================
  函 数 名： Load
  功    能： 用户组信息从文件读取
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
=============================================================================*/
BOOL32 CUsrGrpsInfo::Load()
{
    s8 szPath[KDV_MAX_PATH] = {0};
    
    sprintf(szPath, "%s/%s", DIR_DATA, USERGRPINFOFILENAME);

    FILE *fp;
    fp = fopen( szPath, "rb" );
    if (fp == NULL)
    {
        return FALSE;
    }

    fread( &m_nGrpNum, sizeof(s32), 1, fp );  
    fread( m_atInfo, sizeof( TUsrGrpInfo ), m_nGrpNum, fp );
    m_nGrpNum = MAXNUM_USRGRP;

    fclose(fp);

    return TRUE;
}

/*=============================================================================
  函 数 名： Print
  功    能： 调试打印
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
=============================================================================*/
void CUsrGrpsInfo::Print() const
{
    OspPrintf(TRUE, FALSE, "User Group Info, Total Number: %d\n", m_nGrpNum);
    for (s32 nIndex = 0; nIndex < m_nGrpNum; nIndex ++)
    {
        if (m_atInfo[nIndex].IsFree())
        {
            continue;
        }
        OspPrintf(TRUE, FALSE, "User Group Info Index: %d\n", nIndex);
        m_atInfo[nIndex].Print();
        // 哪些用户属于这个组

        OspPrintf(TRUE, FALSE, "\tUsers in Group:\n");
        CExUsrInfo cInfo;
        for ( s32 nUserLoop = 0; nUserLoop < g_cUsrManage.GetUserNum(); nUserLoop ++ )
        {
            g_cUsrManage.GetUserFullInfo( &cInfo, nUserLoop );
            if ( cInfo.GetUsrGrpId() == m_atInfo[nIndex].GetUsrGrpId() )
            {
                OspPrintf(TRUE, FALSE, "\t\t%s\n", cInfo.GetName());
            }
        }
    }
}

/*=============================================================================
  函 数 名： GetUsrGrpUserCount
  功    能： 获取该组用户当前数量和最大值
  算法实现： 
  全局变量： 
  参    数： cUsrGrpObj: 操作的用户管理对象
			 cUsrGrp:    操作的用户组管理对象             
			 byGrpId：   指定的用户组号
             byMaxNum：  指定组允许加入的最大用户数
			 byNum：     指定组目前的用户数
  返 回 值： BOOL32              FALSE 没有找到
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
	08/11/21                付秀华        对于两类控制台（VCS MCS）采取两类不同的用户管理对象
	                                      各自操作各自的用户对象
=============================================================================*/
BOOL32  CMcuVcData::GetUsrGrpUserCount(CUsrManage& cUsrManageObj, 
									   CUsrGrpsInfo& cUsrGrpObj, u8 byGrpId, 
									   u8 &byMaxNum, u8 &byNum)
{
    //若为N+1备份工作模式（即切换后），取内存中的信息
    if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
    {
        u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
        if (0 == byInsId || byInsId > MAXNUM_NPLUS_MCU)
        {
            OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] McuSwitchedInsId is invalid in NPlus switched mode.\n");
            return FALSE;
        }
        else
        {
            CApp *pcApp = &g_cNPlusApp;
            CNPlusInst *pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL != pcInst)
            {
                return pcInst->GetGrpUsrCount(byGrpId, byMaxNum, byNum);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] GetInstance(%d) failed.\n", byInsId);
                return FALSE;
            }
        }                
    }

    if (byGrpId == USRGRPID_SADMIN)
    {
        // 如果是超级管理员返回系统值
        byMaxNum = MAXNUM_GRPUSRNUM;
        byNum = cUsrManageObj.GetUserNum();
        return TRUE;
    }

    TUsrGrpInfo tGrpInfo;
    BOOL32 bRet = cUsrGrpObj.GetGrpById( byGrpId, tGrpInfo );

    byNum = 0;
    byMaxNum = 0;

    if ( !bRet )
    {
        // 没找到
        return FALSE;
    }

    byMaxNum = tGrpInfo.GetMaxUsrNum();

    s32 nTotalUserNum = cUsrManageObj.GetUserNum();

    CExUsrInfo cUsrInfo;

    for (s32 nUsrLoop = 0; nUsrLoop < nTotalUserNum; nUsrLoop ++)
    {
        cUsrManageObj.GetUserFullInfo( &cUsrInfo, nUsrLoop );
        if ( byGrpId == cUsrInfo.GetUsrGrpId() )
        {
            byNum ++;
        }
    }
    return TRUE;
}

/*=============================================================================
  函 数 名： GetMCSUsrGrpUserCount
  功    能： 获取MCS控制台对应用户组的用户当前数量和最大值
  算法实现： 
  全局变量： 
  参    数：        
			 byGrpId：   指定的用户组号
             byMaxNum：  指定组允许加入的最大用户数
			 byNum：     指定组目前的用户数
  返 回 值： BOOL32              FALSE 没有找到
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/21                付秀华        创建
=============================================================================*/
BOOL32  CMcuVcData::GetMCSUsrGrpUserCount(u8 byGrpId, u8 &byMaxNum, u8 &byNum)
{
	return GetUsrGrpUserCount(g_cUsrManage, m_cUsrGrpInfo, byGrpId, byMaxNum, byNum);
}
/*=============================================================================
  函 数 名： GetVCSUsrGrpUserCount
  功    能： 获取VCS控制台对应用户组的用户当前数量和最大值
  算法实现： 
  全局变量： 
  参    数：        
			 byGrpId：   指定的用户组号
             byMaxNum：  指定组允许加入的最大用户数
			 byNum：     指定组目前的用户数
  返 回 值： BOOL32              FALSE 没有找到
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/21                付秀华        创建
=============================================================================*/
BOOL32  CMcuVcData::GetVCSUsrGrpUserCount(u8 byGrpId, u8 &byMaxNum, u8 &byNum)
{
	return GetUsrGrpUserCount(g_cVCSUsrManage, m_cVCSUsrGrpInfo, byGrpId, byMaxNum, byNum);
}

/*=============================================================================
  函 数 名： AddUserGroup
  功    能： 增加一个用户组
  算法实现： CUsrGrpsInfo: 操作的用户组对象
             TUsrGrpInfo:  添加的用户组信息
			 wErrorNo：    失败原因
  全局变量： 
  参    数： 
  返 回 值： u8              返回组ID，如USRGRPID_INVALID 表示添加失败，填写wErrorNo。
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
    06/09/07    4.0         顾振华        调整策略，允许用户自己配制用户组ID
=============================================================================*/
u8 CMcuVcData::AddUserGroup( CUsrGrpsInfo &cUsrGrpObj, const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo  )
{
    wErrorNo = 0;
    /*
    TUsrGrpInfo tOldOne;
    if ( m_cUsrGrpInfo.GetGrpByName( tGrpInfo.szName, tOldOne ) )
    {
        // 已存在
        wErrorNo = UM_EX_USRGRPDUP;
        return USRGRPID_INVALID;
    }

    u8 byGrpId = m_cUsrGrpInfo.AddGrp( tGrpInfo );

    if (byGrpId == USRGRPID_INVALID)
    {
        // 用户组满
        wErrorNo = UM_EX_USRGRPFULL;
    }
    else
    {
        m_cUsrGrpInfo.Save();
    } 
    */
    
    if (tGrpInfo.GetUsrGrpId() == USRGRPID_INVALID ||
        tGrpInfo.GetUsrGrpId() > MAXNUM_USRGRP)
    {
        // 非法保护
        wErrorNo = UM_EX_USRGRPIDINV;
        return USRGRPID_INVALID;
    }
    TUsrGrpInfo tOldOne;
    if ( cUsrGrpObj.GetGrpByName( tGrpInfo.GetUsrGrpName(), tOldOne ) )
    {
        //  组名已存在
        wErrorNo = UM_EX_USRGRPDUP;
        return USRGRPID_INVALID;
    }

    u8 byGrpId = cUsrGrpObj.AddGrp( tGrpInfo );

    if (byGrpId == USRGRPID_INVALID)
    {
        // 组ID已存在
        wErrorNo = UM_EX_USRGRPIDDUP;
    }
    else
    {
        cUsrGrpObj.Save();
    } 

    return byGrpId;
}

/*=============================================================================
  函 数 名： AddMCSUserGroup
  功    能： 增加一个MCS用户组
  算法实现：
             TUsrGrpInfo:  添加的用户组信息
			 wErrorNo：    失败原因
  全局变量： 
  参    数： 
  返 回 值： u8              返回组ID，如USRGRPID_INVALID 表示添加失败，填写wErrorNo。
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/21                付秀华        创建
=============================================================================*/
u8 CMcuVcData::AddMCSUserGroup( const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo  )
{
    return AddUserGroup(m_cUsrGrpInfo, tGrpInfo, wErrorNo);
}

/*=============================================================================
  函 数 名： AddVCSUserGroup
  功    能： 增加一个VCS用户组
  算法实现：
             TUsrGrpInfo:  添加的用户组信息
			 wErrorNo：    失败原因
  全局变量： 
  参    数： 
  返 回 值： u8              返回组ID，如USRGRPID_INVALID 表示添加失败，填写wErrorNo。
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/21                付秀华        创建
=============================================================================*/
u8 CMcuVcData::AddVCSUserGroup( const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo  )
{
    return AddUserGroup(m_cVCSUsrGrpInfo, tGrpInfo, wErrorNo);
}

/*=============================================================================
  函 数 名： ChgUserGroup
  功    能： 修改一个用户组，如果失败填写 wErrorNo
  算法实现： 
  全局变量： 
  参    数： CUsrGrpsInfo: 操作的用户组对象
             TUsrGrpInfo:  添加的用户组信息
			 wErrorNo：    失败原因
  返 回 值： BOOL32
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
=============================================================================*/
BOOL32 CMcuVcData::ChgUserGroup(CUsrGrpsInfo &cUsrGrpObj, const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo )
{
    wErrorNo = 0;

    if ( !cUsrGrpObj.ChgGrp( tGrpInfo ) )
    {
        // 没有找到
        wErrorNo = UM_EX_USRGRPNOEXIST;
        return FALSE;
    }

    cUsrGrpObj.Save();

    return TRUE;    
}
/*=============================================================================
  函 数 名： ChgMCSUserGroup
  功    能： 修改MCS的一个用户组，如果失败填写 wErrorNo
  算法实现： 
  全局变量： 
  参    数： TUsrGrpInfo:  修改的用户组信息
			 wErrorNo：    失败原因
  返 回 值： BOOL32
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/21                付秀华        创建
=============================================================================*/
BOOL32 CMcuVcData::ChgMCSUserGroup(const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo )
{
    return ChgUserGroup(m_cUsrGrpInfo, tGrpInfo, wErrorNo);  
}

/*=============================================================================
  函 数 名： ChgVCSUserGroup
  功    能： 修改VCS的一个用户组，如果失败填写 wErrorNo
  算法实现： 
  全局变量： 
  参    数： TUsrGrpInfo:  添加的用户组信息
			 wErrorNo：    失败原因
  返 回 值： BOOL32
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/21                付秀华        创建
=============================================================================*/
BOOL32 CMcuVcData::ChgVCSUserGroup(const TUsrGrpInfo &tGrpInfo, u16 &wErrorNo )
{
    return ChgUserGroup(m_cVCSUsrGrpInfo, tGrpInfo, wErrorNo);  
}
/*=============================================================================
  函 数 名： DelMCSUserGroup
  功    能： 删除一个MCS用户组，同时完成相关的资源释放。如果失败填写 wErrorNo
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
=============================================================================*/
BOOL32 CMcuVcData::DelMCSUserGroup( u8 byGrpId, u16 &wErrorNo )
{
    wErrorNo = 0;

    if ( ! m_cUsrGrpInfo.DelGrp( byGrpId ) )
    {
        // 没有找到
        wErrorNo = UM_EX_USRGRPNOEXIST;
        return FALSE;
    }

    // 删除所有相关的信息，包括用户、模版、会议等
    CServMsg cServMsg;
    cServMsg.SetMcuId(LOCAL_MCUID);
    cServMsg.SetSrcMtId(0);
    cServMsg.SetSrcSsnId(0);
    
    cServMsg.SetMsgBody();
    


    s32 nUserNum = g_cUsrManage.GetUserNum();
    CExUsrInfo cInfo;
    for (s32 nLoop = nUserNum -1 ;nLoop >= 0; nLoop --)
    {
        g_cUsrManage.GetUserFullInfo( &cInfo, nLoop);
        if (cInfo.GetUsrGrpId() == byGrpId)
        {
            g_cUsrManage.DelUser( &cInfo );

            // 通知
            cServMsg.SetEventId(MCU_MCS_DELUSER_NOTIF);
            cServMsg.SetMsgBody( (u8*)cInfo.GetName(), MAX_CHARLENGTH );
			g_cMcsSsnApp.Broadcast2SpecGrpMcsSsn( byGrpId, MCU_MCS_DELUSER_NOTIF, 
											      cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
    }


    cServMsg.SetMsgBody();

	for (u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
	{
		TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
		if (tMapData.IsTemUsed())
		{
			TTemplateInfo tTemConf;
			if (g_cMcuVcApp.GetTemplate(byConfIdx, tTemConf))
			{
                if ( byGrpId == tTemConf.m_tConfInfo.GetUsrGrpId() &&
					 MCS_CONF == tTemConf.m_tConfInfo.GetConfSource())
                {
                    // 结束模版                            
                    cServMsg.SetEventId(MCS_MCU_DELTEMPLATE_REQ);
                    cServMsg.SetConfId( tTemConf.m_tConfInfo.GetConfId() );
                    g_cMcuVcApp.SendMsgToDaemonConf(MCS_MCU_DELTEMPLATE_REQ,
                                                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen());                    
                }
			}
		}

        
		if (tMapData.IsValidConf())
		{
			CMcuVcInst *pcMcuVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
			if (NULL == pcMcuVcInst)
			{
				continue;
			}

			TConfInfo* ptConfInfo = &pcMcuVcInst->m_tConf;
            if ( byGrpId == ptConfInfo->GetUsrGrpId() && 
				 MCS_CONF == ptConfInfo->GetConfSource())
            {
                // 结束会议
                cServMsg.SetEventId(MCS_MCU_RELEASECONF_REQ);
                cServMsg.SetConfId( ptConfInfo->GetConfId() );
                g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(), MCS_MCU_RELEASECONF_REQ,
                                          cServMsg.GetServMsg(), cServMsg.GetServMsgLen());  
            }
		}
	}        

    // TODO: 删除用户组暂时不删除相关会议录像

    m_cUsrGrpInfo.Save();

    return TRUE;
}
/*=============================================================================
  函 数 名： DelVCSUserGroup
  功    能： 删除一个VCS用户组，同时完成相关的资源释放。如果失败填写 wErrorNo
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
=============================================================================*/
BOOL32 CMcuVcData::DelVCSUserGroup( u8 byGrpId, u16 &wErrorNo )
{
    wErrorNo = 0;

    if ( ! m_cVCSUsrGrpInfo.DelGrp( byGrpId ) )
    {
        // 没有找到
        wErrorNo = UM_EX_USRGRPNOEXIST;
        return FALSE;
    }

    // 删除所有相关的信息，包括用户、模版、会议等
    CServMsg cServMsg;
    cServMsg.SetMcuId(LOCAL_MCUID);
    cServMsg.SetSrcMtId(0);
    cServMsg.SetSrcSsnId(0);    
    cServMsg.SetMsgBody();
    


    s32 nUserNum = g_cVCSUsrManage.GetUserNum();
    CExUsrInfo cInfo;
    for (s32 nLoop = nUserNum -1 ;nLoop >= 0; nLoop --)
    {
        g_cVCSUsrManage.GetUserFullInfo( &cInfo, nLoop);
        if (cInfo.GetUsrGrpId() == byGrpId)
        {
            g_cVCSUsrManage.DelUser( &cInfo );

            // 通知
            cServMsg.SetEventId(MCU_MCS_DELUSER_NOTIF);
            cServMsg.SetMsgBody( (u8*)cInfo.GetName(), MAX_CHARLENGTH );
			g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn( byGrpId, MCU_MCS_DELUSER_NOTIF, 
											      cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
    }


    cServMsg.SetMsgBody();

	for (u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
	{
		TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
		if (tMapData.IsTemUsed())
		{
			TTemplateInfo tTemConf;
			if (g_cMcuVcApp.GetTemplate(byConfIdx, tTemConf))
			{
                if ( byGrpId == tTemConf.m_tConfInfo.GetUsrGrpId() &&
					 VCS_CONF == tTemConf.m_tConfInfo.GetConfSource())
                {
                    // 结束模版                            
                    cServMsg.SetEventId(MCS_MCU_DELTEMPLATE_REQ);
                    cServMsg.SetConfId(tTemConf.m_tConfInfo.GetConfId());
                    g_cMcuVcApp.SendMsgToDaemonConf(MCS_MCU_DELTEMPLATE_REQ,
                                                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen());                    
                }
			}
		}

        
		if (tMapData.IsValidConf())
		{
			CMcuVcInst *pcMcuVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
			if (NULL == pcMcuVcInst)
			{
				continue;
			}

			TConfInfo* ptConfInfo = &pcMcuVcInst->m_tConf;
            if ( byGrpId == ptConfInfo->GetUsrGrpId() && 
				 VCS_CONF == ptConfInfo->GetConfSource())
            {
                // 结束会议
                cServMsg.SetEventId(MCS_MCU_RELEASECONF_REQ);
                cServMsg.SetConfId( ptConfInfo->GetConfId() );
                g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(), MCS_MCU_RELEASECONF_REQ,
                                          cServMsg.GetServMsg(), cServMsg.GetServMsgLen());  
            }
		}
	}        

    // TODO: 删除用户组暂时不删除相关会议录像

    m_cVCSUsrGrpInfo.Save();

    return TRUE;
}

/*=============================================================================
  函 数 名： GetMCSUserGroupInfo
  功    能： 获取所有用户组信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
=============================================================================*/
void  CMcuVcData::GetMCSUserGroupInfo ( u8 &byNum, TUsrGrpInfo **ptInfo )
{
    //若为N+1备份工作模式（即切换后），取内存中的信息
    if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
    {
        u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
        if (0 == byInsId || byInsId > MAXNUM_NPLUS_MCU)
        {
            OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] McuSwitchedInsId is invalid in NPlus switched mode.\n");
        }
        else
        {
            CApp *pcApp = &g_cNPlusApp;
            CNPlusInst *pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL != pcInst)
            {
                CUsrGrpsInfo* pcUsrGrpsInfo = pcInst->GetUsrGrpInfo();
                byNum = pcUsrGrpsInfo->GetGrpNum();
                *ptInfo = pcUsrGrpsInfo->m_atInfo;                
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] GetInstance(%d) failed.\n", byInsId);
            }
        }                
        return ;
    }

    // 普通模式
    byNum = (u8)m_cUsrGrpInfo.GetGrpNum();

    *ptInfo = m_cUsrGrpInfo.m_atInfo;

}
/*=============================================================================
  函 数 名： GetVCSUserGroupInfo
  功    能： 获取VCS所有用户组信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/21                付秀华        创建
=============================================================================*/
void  CMcuVcData::GetVCSUserGroupInfo ( u8 &byNum, TUsrGrpInfo **ptInfo )
{
    //若为N+1备份工作模式（即切换后），取内存中的信息
//     if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
//     {
//         u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
//         if (0 == byInsId || byInsId > MAXNUM_NPLUS_MCU)
//         {
//             OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] McuSwitchedInsId is invalid in NPlus switched mode.\n");
//         }
//         else
//         {
//             CApp *pcApp = &g_cNPlusApp;
//             CNPlusInst *pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
//             if (NULL != pcInst)
//             {
//                 CUsrGrpsInfo* pcUsrGrpsInfo = pcInst->GetUsrGrpInfo();
//                 byNum = pcUsrGrpsInfo->GetGrpNum();
//                 *ptInfo = pcUsrGrpsInfo->m_atInfo;                
//             }
//             else
//             {
//                 OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] GetInstance(%d) failed.\n", byInsId);
//             }
//         }                
//         return ;
//     }

    // 普通模式
    byNum = (u8)m_cVCSUsrGrpInfo.GetGrpNum();

    *ptInfo = m_cVCSUsrGrpInfo.m_atInfo;

}
/*=============================================================================
函 数 名： GetUserGroupInfo
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： CUsrGrpsInfo * 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/23  4.0			许世林                  创建
=============================================================================*/
CUsrGrpsInfo * CMcuVcData::GetUserGroupInfo( void )
{
    //若为N+1备份工作模式（即切换后），取内存中的信息
    if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
    {
        u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
        if (0 == byInsId || byInsId > MAXNUM_NPLUS_MCU)
        {
            OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] McuSwitchedInsId is invalid in NPlus switched mode.\n");
            return NULL;
        }
        else
        {
            CApp *pcApp = &g_cNPlusApp;
            CNPlusInst *pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL != pcInst)
            {
                return pcInst->GetUsrGrpInfo();
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[GetUsrGrpUserCount] GetInstance(%d) failed.\n", byInsId);
                return NULL;
            }
        }                
    }

    // 普通模式
    return &m_cUsrGrpInfo;
}


/*=============================================================================
  函 数 名： IsMtIpInAllowSeg
  功    能： 指定IP是否在指定用户组的允许范围内
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
=============================================================================*/
BOOL32 CMcuVcData::IsMtIpInAllowSeg ( u8 byGrpId, u32 dwIp )
{
    TUsrGrpInfo tGroup;
    
    if (byGrpId == USRGRPID_SADMIN)
    {
        // guzh [7/26/2006] 如果是超级用户组，直接认为不允许
        return FALSE;
    }
    if (byGrpId == USRGRPID_INVALID)
    {
        return TRUE;
    }

    if ( !m_cUsrGrpInfo.GetGrpById(byGrpId, tGroup) )
    {
        return FALSE;
    }
    if ( tGroup.IsFree() )
    {
        // 用户组空
        return FALSE;
    }

    return tGroup.IsMtIpInAllowSeg(dwIp);
}

/*=============================================================================
  函 数 名： PrtUserGroup
  功    能： 调试打印
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/06/23    4.0         顾振华        创建
=============================================================================*/
void CMcuVcData::PrtUserGroup()
{
    m_cUsrGrpInfo.Print();
}

/*=============================================================================
  函 数 名： AddUserTaskInfo
  功    能： 添加用户分配的任务信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： FALSE:用户已存在/用户数已满
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
BOOL  CUsersTaskInfo::AddUserTaskInfo(TUserTaskInfo& tUserTaskInfo)
{
	u16 wSpacePos = MAXNUM_VCSUSERNUM;
	for (u16 wIndex = 0; wIndex < MAXNUM_VCSUSERNUM; wIndex++)
	{
		if (0 == strcmp(tUserTaskInfo.GetUserName(), m_tUserTaskInfo[wIndex].GetUserName()))
		{
			// 用户已存在
			return FALSE;
		}
		// 找到空闲的位置放新的用户信息，从小存放起
		if (!m_abyUsed[wIndex] && wSpacePos == MAXNUM_VCSUSERNUM)
		{
			wSpacePos = wIndex;
		}
	}

	if (MAXNUM_VCSUSERNUM == wSpacePos)
	{
		return FALSE;
	}
	else
	{
		m_abyUsed[wSpacePos] = 1;
		memcpy(&m_tUserTaskInfo[wSpacePos], &tUserTaskInfo, sizeof(TUserTaskInfo));
		SaveUsersTaskInfo();
		return TRUE;
	}
}

/*=============================================================================
  函 数 名： DelUserTaskInfo
  功    能： 删除用户分配的任务信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： FALSE:指定删除的用户不存在
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
BOOL  CUsersTaskInfo::DelUserTaskInfo(s8* pachUserName)
{
	u16 wUserPos = MAXNUM_VCSUSERNUM;
	for (u16 wIndex = 0; wIndex < MAXNUM_VCSUSERNUM; wIndex++)
	{
		if (0 == strcmp(pachUserName, m_tUserTaskInfo[wIndex].GetUserName()))
		{
			wUserPos = wIndex;
			break;
		}
	}

	if (MAXNUM_VCSUSERNUM == wUserPos)
	{
		return FALSE;
	}
	else
	{
		m_abyUsed[wUserPos] = 0;
		memset(&m_tUserTaskInfo[wUserPos], 0, sizeof(TUserTaskInfo));
		SaveUsersTaskInfo();
		return TRUE;
	}
}
/*=============================================================================
  函 数 名： DelSpecTaskInfo
  功    能： 将所有用户中的指定任务删除
  算法实现： 
  全局变量： 
  参    数： [IN]      cConfId：待删除的任务信息
             [IN/OUT]pdwChgIdx：被删除的用户索引
			 [IN/OUT] dwChgNum：被删除的用户数
  返 回 值： 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
void  CUsersTaskInfo::DelSpecTaskInfo(CConfId cConfId, u16* const pdwChgIdx, u16& dwChgNum)
{
	u16* pdwTemp = pdwChgIdx;
	dwChgNum = 0;
	for (u16 dwIdx = 0; dwIdx < MAXNUM_VCSUSERNUM; dwIdx++)
	{
		if (m_abyUsed[dwIdx] && m_tUserTaskInfo[dwIdx].IsYourTask(cConfId, TRUE) && pdwChgIdx != NULL)
		{
			*pdwTemp++ = dwIdx;
			dwChgNum++;				
		}
	}
	SaveUsersTaskInfo();
}

/*=============================================================================
  函 数 名： ChgUserTaskInfo
  功    能： 修改用户分配的任务信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： FALSE:指定修改的用户不存在
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
BOOL  CUsersTaskInfo::ChgUserTaskInfo(TUserTaskInfo& tUserTaskInfo)
{
	u16 wUserPos = MAXNUM_VCSUSERNUM;
	for (u16 wIndex = 0; wIndex < MAXNUM_VCSUSERNUM; wIndex++)
	{
		if (0 == strcmp(tUserTaskInfo.GetUserName(), m_tUserTaskInfo[wIndex].GetUserName()))
		{
			wUserPos = wIndex;
			break;
		}
	}

	if (MAXNUM_VCSUSERNUM == wUserPos)
	{
		return FALSE;
	}
	else
	{
		memcpy(&m_tUserTaskInfo[wUserPos], &tUserTaskInfo, sizeof(TUserTaskInfo));
		SaveUsersTaskInfo();
		return TRUE;
	}
}
/*=============================================================================
  函 数 名： GetSpecUserTaskInfo
  功    能： 根据用户名获取指定用户的任务信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： FALSE:指定的用户不存在
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
BOOL  CUsersTaskInfo::GetSpecUserTaskInfo(const s8* pachUserName, TUserTaskInfo& tUserTaskInfo) 
{
	u16 wUserPos = MAXNUM_VCSUSERNUM;
	for (u16 wIndex = 0; wIndex < MAXNUM_VCSUSERNUM; wIndex++)
	{
		if (0 == strcmp(m_tUserTaskInfo[wIndex].GetUserName(), pachUserName))
		{
			wUserPos = wIndex;
			break;
		}
	}

	if (MAXNUM_VCSUSERNUM == wUserPos)
	{
		return FALSE;
	}
	else
	{
		tUserTaskInfo = m_tUserTaskInfo[wUserPos];
		return TRUE;
	}
}
/*=============================================================================
  函 数 名： GetSpecUserTaskInfo
  功    能： 根据索引获取指定用户的任务信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： FALSE:指定的用户不存在
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
BOOL  CUsersTaskInfo::GetSpecUserTaskInfo(u16 dwIdx, TUserTaskInfo& tUserTaskInfo)
{
	if (m_abyUsed[dwIdx])
	{
		tUserTaskInfo = m_tUserTaskInfo[dwIdx];
		return TRUE;
	}
	return FALSE;
}

/*=============================================================================
  函 数 名： IsYourTask
  功    能： 判断该用户是否有操作任务的权限
  算法实现： 
  全局变量： 
  参    数： [IN]pachUserName: 用户名
             [IN]cConfId:      会议ID
  返 回 值： FALSE:指定的用户不能操作该会议
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
BOOL  CUsersTaskInfo::IsYourTask(const s8* pachUserName, const CConfId& cConfId)
{
	TUserTaskInfo tTaskInfo;
	if(GetSpecUserTaskInfo(pachUserName, tTaskInfo))
	{
		const CConfId* pcConfId = tTaskInfo.GetUserTaskInfo();
		for (u8 byTaskIndex = 0; byTaskIndex < tTaskInfo.GetUserTaskNum(); byTaskIndex++)
		{
			if (cConfId == *pcConfId)
			{
				return TRUE;
			}
			pcConfId++;
		}
	}
	return FALSE;
}

/*=============================================================================
  函 数 名： SaveUsersTaskInfo
  功    能： 保存所有用户的任务信息到文件
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
BOOL  CUsersTaskInfo::SaveUsersTaskInfo() const
{
   s8 szPath[KDV_MAX_PATH] = {0};
    
    sprintf(szPath, "%s/%s", DIR_DATA, USERTASKINFOFILENAME);

    FILE *fp;
    fp = fopen( szPath, "wb" );
    if (fp == NULL)
    {
        return FALSE;
    }

	fwrite(m_abyUsed, sizeof(u8), MAXNUM_VCSUSERNUM, fp);
	fwrite(m_tUserTaskInfo, sizeof(TUserTaskInfo), MAXNUM_VCSUSERNUM, fp);

#ifdef WIN32
    fflush(fp);
#endif

    fclose(fp);

    return TRUE;    
}
/*=============================================================================
  函 数 名： LoadUsersTaskInfo
  功    能： 下载所有用户的任务信息到文件
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
BOOL  CUsersTaskInfo::LoadUsersTaskInfo()
{
   s8 szPath[KDV_MAX_PATH] = {0};
    
    sprintf(szPath, "%s/%s", DIR_DATA, USERTASKINFOFILENAME);

    FILE *fp;
    fp = fopen( szPath, "rb" );
    if (fp == NULL)
    {
        return FALSE;
    }

 	fread(m_abyUsed, sizeof(u8), MAXNUM_VCSUSERNUM, fp);
	fread(m_tUserTaskInfo, sizeof(TUserTaskInfo), MAXNUM_VCSUSERNUM, fp);

    fclose(fp);
    return TRUE;    
}

/*=============================================================================
  函 数 名： AddVCSUserTaskInfo
  功    能： 添加VCS用户分配的任务信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： FALSE:用户已存在/用户数已满
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
BOOL  CMcuVcData::AddVCSUserTaskInfo(TUserTaskInfo& tUserTaskInfo)
{
	return m_cVCSUsrTaskInfo.AddUserTaskInfo(tUserTaskInfo);
}

/*=============================================================================
  函 数 名： DelVCSUserTaskInfo
  功    能： 删除VCS用户分配的任务信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： FALSE:用户已存在/用户数已满
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
BOOL  CMcuVcData::DelVCSUserTaskInfo(s8* pachUserName)
{
	return m_cVCSUsrTaskInfo.DelUserTaskInfo(pachUserName);
}
/*=============================================================================
  函 数 名： DelVCSSpecTaskInfo
  功    能： 对所有用户，删除该指定任务，并发出消息通告
  算法实现： 
  全局变量： 
  参    数： [IN]      cConfId：待删除的任务信息
  返 回 值： FALSE:用户已存在/用户数已满
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
void  CMcuVcData::DelVCSSpecTaskInfo(CConfId cConfId)
{
	u16 dwChgIdx[MAXNUM_VCSUSERNUM];
	u16 *pdwChgIdx = dwChgIdx;
	memset(dwChgIdx, 0, sizeof(dwChgIdx));
	u16  dwChgNum = 0;


	m_cVCSUsrTaskInfo.DelSpecTaskInfo(cConfId, pdwChgIdx, dwChgNum);
	for (u16 dwIdx = 0; dwIdx <	dwChgNum; dwIdx++)
	{
		TUserTaskInfo tTaskInfo;
		CServMsg      cMsg;
		if(m_cVCSUsrTaskInfo.GetSpecUserTaskInfo(*pdwChgIdx++, tTaskInfo))
		{
			s32 nUsrNum = g_cVCSUsrManage.GetUserNum();
			CVCSUsrInfo cUsrInfo;
			for (s32 nUsrLoop = 0; nUsrLoop < nUsrNum; nUsrLoop ++)
			{
				g_cVCSUsrManage.GetUserFullInfo( &cUsrInfo, nUsrLoop );
				if ( strcmp(cUsrInfo.GetName(), tTaskInfo.GetUserName()))
				{
					cUsrInfo.AssignTask(tTaskInfo.GetUserTaskNum(), tTaskInfo.GetUserTaskInfo());

					cMsg.SetMcuId(LOCAL_MCUID);
					cMsg.SetSrcMtId(0);
					cMsg.SetSrcSsnId(0);
					cMsg.SetMsgBody((u8*)&cUsrInfo, sizeof(CVCSUsrInfo));
					g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn(cUsrInfo.GetUsrGrpId(), MCU_MCS_CHANGEUSER_NOTIF,
						                             cMsg.GetServMsg(), cMsg.GetServMsgLen());
					break;
				}
			}
			

		}


	}
}

/*=============================================================================
  函 数 名： ChgVCSUserTaskInfo
  功    能： 修改VCS用户分配的任务信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： FALSE:用户已存在/用户数已满
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
BOOL  CMcuVcData::ChgVCSUserTaskInfo(TUserTaskInfo& tUserTaskInfo)
{
	return m_cVCSUsrTaskInfo.ChgUserTaskInfo(tUserTaskInfo);
}
/*=============================================================================
  函 数 名： GetSpecVCSUserTaskInfo
  功    能： 获取指定用户的任务信息
  算法实现： 
  全局变量： 
  参    数： [IN]pachUserName:查询的用户名
             [OUT]tUserTaskInfo:接收该用户的任务信息
  返 回 值： 是否为该用户指定任务
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
BOOL  CMcuVcData::GetSpecVCSUserTaskInfo(s8* pachUserName, TUserTaskInfo& tUserTaskInfo)
{
	return m_cVCSUsrTaskInfo.GetSpecUserTaskInfo(pachUserName, tUserTaskInfo);
}
/*=============================================================================
  函 数 名： LoadVCSUsersTaskInfo
  功    能： 文件中读取所有用户的任务信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 是否为该用户指定任务
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/11/24    4.5         付秀华        创建
=============================================================================*/
BOOL  CMcuVcData::LoadVCSUsersTaskInfo()
{
	return m_cVCSUsrTaskInfo.LoadUsersTaskInfo();
}
/*=============================================================================
  函 数 名： IsYourTask
  功    能： 判定指定会议是否为指定用户的任务范围内
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 是否为该用户指定的任务
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  08/12/15    4.5         付秀华        创建
=============================================================================*/
BOOL  CMcuVcData::IsYourTask(const s8* pachUserName, const CConfId& cConfId)
{
	return m_cVCSUsrTaskInfo.IsYourTask(pachUserName, cConfId);
}

/********************************************

        高清适配资源管理实现

  *******************************************/

/*=============================================================================
  函 数 名： CBasMgr
  功    能： 适配管理类 构造
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿        创建
=============================================================================*/
CBasMgr::CBasMgr()
{
    Clear();
}

/*=============================================================================
  函 数 名： CBasMgr
  功    能： 适配管理类 析构
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿        创建
=============================================================================*/
CBasMgr::~CBasMgr()
{
    Clear();
}

/*=============================================================================
  函 数 名： GetNeededMau
  功    能： 适配管理类 获取某会议所需要的bas
  算法实现： 忽略静态双流
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿        创建
=============================================================================*/
void CBasMgr::GetNeededMau(const TConfInfo &tConfInfo, u8 &byNVChn, u8 &byH263Chn, u8 &byVGAChn)
{
    byNVChn = 0;
    byH263Chn = 0;
    byVGAChn = 0;
    
    //zbq[03/24/2009] 双流支持非H264处理
    SetDualDouble(IsConfDoubleDual(tConfInfo));
    
    //VGA双流忽略，只取h263plus(双向适配绑定出现)
    if (IsConfDoubleDual(tConfInfo))
    {
        byH263Chn ++ ;
    }

    if (MEDIA_TYPE_H264 != tConfInfo.GetMainVideoMediaType())
    {
        return;
    }

    u8 byHDDSNeed = 0;    //双流主流适配通道的需求量
    u8 byHDNeeded = 0;    //高清适配群组的需求量
    u8 bySDNeeded = 0;    //标清适配群组的需求量

    //双速
    if (tConfInfo.GetSecBitRate() != 0 &&
        VIDEO_FORMAT_CIF != tConfInfo.GetMainVideoFormat())
    {
        //zbq[05/22/2009] 紧凑适配不为此预留
        if (!g_cMcuVcApp.IsAdpResourceCompact())
        {
            byHDNeeded ++;
        }
    }

    //双格式
    u8 bySecVidType = tConfInfo.GetSecVideoMediaType();
    if (MEDIA_TYPE_MP4 == bySecVidType ||
        MEDIA_TYPE_H263 == bySecVidType )
    {
        bySDNeeded ++;
    }
    else if (MEDIA_TYPE_NULL != bySecVidType)
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::GetNeededMau] unexpected sec type.%d\n", bySecVidType);
    }

    //同主视频双流
    SetDualEqMv(IsConfDualEqMV(tConfInfo));
    SetDualVGA(!IsDualEqMv() && !IsDualDouble());

    if (IsDualEqMv() &&
        VIDEO_FORMAT_CIF != tConfInfo.GetMainVideoFormat()
        /*&&
        //zbq[04/02/2009] 同主视频的双流需要Res支持cif或4cif，720p是最大同主视频，不需要适配；1080p没有；
        (tConfInfo.GetConfAttrbEx().IsResEx4Cif() || tConfInfo.GetConfAttrbEx().IsResExCif())*/)
    {
        byHDDSNeed ++;
    }

    //用户指定
    if (tConfInfo.GetConfAttrbEx().IsResEx1080())
    {
        byHDNeeded ++;
    }
    if (tConfInfo.GetConfAttrbEx().IsResEx4Cif())
    {
        byHDNeeded ++;
    }
    if (tConfInfo.GetConfAttrbEx().IsResEx720())
    {
        byHDNeeded ++;
    }
    if (tConfInfo.GetConfAttrbEx().IsResExCif())
    {
        bySDNeeded ++;
    }

    byNVChn = max(byHDNeeded, bySDNeeded) + byHDDSNeed;

    //主流不支持适配，双流不预留适配资源
    if (!IsDualEqMv() && byNVChn >= 1 &&
        (tConfInfo.GetDStreamUsrDefFPS() > 5 || IsResG(tConfInfo.GetDoubleVideoFormat(), VIDEO_FORMAT_XGA)))
    {
        byVGAChn = 1;
    }

    return;
}

/*=============================================================================
  函 数 名： GetNeededMpu
  功    能： 适配管理类 获取某会议所需要的bas
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/05/19    4.5         张宝卿        创建
=============================================================================*/
void CBasMgr::GetNeededMpu(const TConfInfo &tConfInfo, u8 &byNVChn, u8 &byDSChn)
{
    byNVChn = 0;
    byDSChn = 0;

    u8 byH263Chn = 0;
    u8 byVGAChn = 0;
    
    GetNeededMau(tConfInfo, byNVChn, byH263Chn, byVGAChn);

    //同主视频 双流
    if (IsConfDualEqMV(tConfInfo) &&
        (VIDEO_FORMAT_CIF != tConfInfo.GetMainVideoFormat() &&
         MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType()))
    {
        byNVChn = byNVChn - 1;
        byDSChn = 1;
    }
    else
    {
        byDSChn = max(byH263Chn, byVGAChn);
        byDSChn = min(byDSChn, 1);
    }
    return;
}

/*=============================================================================
  函 数 名： IsMauSufficient
  功    能： 适配管理类 当前MCU空闲的MAU资源是否能满足该会议或模板
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿        创建
=============================================================================*/
BOOL32 CBasMgr::IsMauSufficient(const TConfInfo &tConfInfo)
{
    u8 byNVIdle = 0;
    u8 byH263Idle = 0;
    u8 byVGAIdle = 0;
    g_cMcuVcApp.GetIdleMau(byNVIdle, byVGAIdle, byH263Idle);

    u8 byNVNeeded = 0;
    u8 byH263Needed = 0;
    u8 byVGANeeded = 0;
    GetNeededMau(tConfInfo, byNVNeeded, byH263Needed, byVGANeeded);

    if (byNVIdle >= byNVNeeded && byH263Idle >= byH263Needed && byVGAIdle >= byVGANeeded)
    {
        return TRUE;
    }
    OspPrintf(TRUE, FALSE, "[IsMauSufficient] Idle<nv.%d,h263p.%d,vga.%d>, need<%d,%d,%d>\n",
                            byNVIdle, byH263Idle, byVGAIdle, byNVNeeded, byH263Needed, byVGANeeded);
    
    return FALSE;
}

/*=============================================================================
  函 数 名： IsHdBasSufficient
  功    能： 适配管理类 当前MCU空闲的BAS资源是否能满足该会议或模板
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿        创建
=============================================================================*/
BOOL32 CBasMgr::IsHdBasSufficient(const TConfInfo &tConfInfo)
{
    return IsMpuSufficient(tConfInfo) || IsMauSufficient(tConfInfo) ;
}

/*=============================================================================
  函 数 名： OcuppyHdBasChn
  功    能： 适配管理类
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿        创建
=============================================================================*/
BOOL32 CBasMgr::OcuppyHdBasChn(const TConfInfo &tConfInfo)
{
    if (IsMpuSufficient(tConfInfo))
    {
        return OcuppyMpu(tConfInfo);
    }
    else if (IsMauSufficient(tConfInfo))
    {
        return OcuppyMau(tConfInfo);
    }
    return FALSE;
}

/*=============================================================================
  函 数 名： IsMpuSufficient
  功    能： 适配管理类 当前MCU空闲的Mpu资源是否能满足该会议或模板
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/05/19    4.6         张宝卿        创建
=============================================================================*/
BOOL32 CBasMgr::IsMpuSufficient(const TConfInfo &tConfInfo)
{
    u8 byChnIdle = 0;
    g_cMcuVcApp.GetIdleMpu(byChnIdle);

    u8 byNVChn = 0;
    u8 byDSChn = 0;
    GetNeededMpu(tConfInfo, byNVChn, byDSChn);

    if (byChnIdle >= byNVChn + byDSChn)
    {
        return TRUE;
    }
    OspPrintf(TRUE, FALSE, "[IsMpuSufficient] Idle<nv.%d>, need<NV.%d, DS.%d>\n", byChnIdle, byNVChn, byDSChn);

    return FALSE;
}


/*=============================================================================
  函 数 名： GetHdBasStatus
  功    能： 适配管理类 获取整个mcu已经在用的bas
  算法实现： 忽略静态双流通道. 本接口供上报MCS用.
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿        创建
=============================================================================*/
void CBasMgr::GetHdBasStatus(TMcuHdBasStatus &tHdBasStatus, TConfInfo &tCurConf)
{
    tHdBasStatus.Clear();

    for(u8 byEqpId = BASID_MIN; byEqpId <= BASID_MAX; byEqpId++)
    {
        TPeriEqpStatus tStatus;
        if (!g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus))
        {
            continue;
        }
        //hd only
        if (!g_cMcuAgent.IsEqpBasHD(byEqpId) ||
            !g_cMcuVcApp.IsPeriEqpConnected(byEqpId))
        {
            continue;
        }

        u8 byConfIdx = tStatus.GetConfIdx();
        u8 byEqpType = tStatus.m_tStatus.tHdBas.GetEqpType();
        
        //get ip
        u32 dwEqpIp = 0;
        u8  byType = 0;
        g_cMcuAgent.GetPeriInfo(byEqpId, &dwEqpIp, &byType);

        u8 byChnIdx = 0;

        switch (byEqpType)
        {
        case TYPE_MAU_NORMAL:
        case TYPE_MAU_H263PLUS:
            {
                //ignore vga
                byChnIdx = 0;
                BOOL32 bReserved = tStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus(byChnIdx)->IsReserved();
                BOOL32 bH263p = tStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MAU_H263PLUS ? TRUE : FALSE;
                
                if (bReserved)
                {
                    CConfId cConfId = g_cMcuVcApp.GetConfId(byConfIdx);
                    tHdBasStatus.SetConfMau(cConfId, dwEqpIp, bH263p);
                }
                else
                {
                    tHdBasStatus.SetIdleMau(dwEqpIp, bH263p);
                }
            }
            break;

        case TYPE_MPU:
            for (byChnIdx = 0; byChnIdx < MAXNUM_MPU_CHN; byChnIdx++)
            {
                BOOL32 bReserved = tStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx)->IsReserved();
                
                if (bReserved)
                {
                    CConfId cConfId = g_cMcuVcApp.GetConfId(byConfIdx);
                    tHdBasStatus.SetConfMpuChn(cConfId, dwEqpIp, byChnIdx);
                }
                else
                {
                    tHdBasStatus.SetIdleMpuChn(dwEqpIp, byChnIdx);
                }
            }
            break;

        default:
            break;
        }
    }

    u8 byNormalMau = 0;
    u8 byH263pMau = 0;
    u8 byVGAMau = 0;
    GetNeededMau(tCurConf, byNormalMau, byH263pMau, byVGAMau);
    tHdBasStatus.SetCurMauNeeded(byNormalMau, byH263pMau);

    u8 byNVChn = 0;
    u8 byDSChn = 0;
    GetNeededMpu(tCurConf, byNVChn, byDSChn);
    tHdBasStatus.SetCurMpuChnNeeded(byNVChn + byDSChn);

    return;
}


/*=============================================================================
  函 数 名： OcuppyMau
  功    能： 适配管理类 占用本会议需要的bas（含VGA）
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿        创建
=============================================================================*/
BOOL32 CBasMgr::OcuppyMau(const TConfInfo &tConfInfo)
{
    u8 byNVIdle = 0;
    u8 byH263Idle = 0;
    u8 byVGAIdle = 0;   //目前暂未启用

    g_cMcuVcApp.GetIdleMau(byNVIdle, byVGAIdle, byH263Idle);
    
    u8 byNVNeeded = 0;
    u8 byH263Needed = 0;
    u8 byVGANeeded = 0;

    GetNeededMau(tConfInfo, byNVNeeded, byH263Needed, byVGANeeded);
    
    if (byNVIdle < byNVNeeded || byH263Idle < byH263Needed || byVGAIdle < byVGANeeded)
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::OcuppyMau] failed due to idle<%d,%d,%d>, needed<%d,%d,%d>\n",
                                byNVIdle, byH263Idle, byVGAIdle, byNVNeeded, byH263Needed, byVGANeeded);
        return FALSE;
    }
    
    BOOL32 bH263Needed = byH263Needed != 0 ? TRUE : FALSE;
    BOOL32 bDualEqMV = IsConfDualEqMV(tConfInfo);

    //尝试占用
    u8 byEqpId = 0;
    u8 byChnId = 0;

    TEqp tEqp;
    tEqp.SetNull();

    while (byH263Needed > 0)
    {
        if (g_cMcuVcApp.GetIdleMauChn(MAU_CHN_263TO264, byEqpId, byChnId))
        {
            tEqp = g_cMcuVcApp.GetEqp(byEqpId);
            AddBrdChn(tEqp, byChnId, MAU_CHN_263TO264, TRUE);
        }
        else
        {
            break;
        }
        if (g_cMcuVcApp.GetIdleMauChn(MAU_CHN_264TO263, byEqpId, byChnId))
        {
            tEqp = g_cMcuVcApp.GetEqp(byEqpId);
            AddBrdChn(tEqp, byChnId, MAU_CHN_264TO263, TRUE);
        }
        else
        {
            break;
        }
        byH263Needed--;
    }
    
    while (byNVNeeded > 0)
    {
        tEqp.SetNull();
        byEqpId = 0;
        byChnId = 0;

        if (bDualEqMV)
        {
            if (g_cMcuVcApp.GetIdleMauChn(MAU_CHN_NORMAL, byEqpId, byChnId))
            {
                tEqp = g_cMcuVcApp.GetEqp(byEqpId);
                AddBrdChn(tEqp, byChnId, MAU_CHN_NORMAL, TRUE);
            }
            else
            {
                break;
            }
            bDualEqMV = FALSE;
        }
        else
        {
            if (g_cMcuVcApp.GetIdleMauChn(MAU_CHN_NORMAL, byEqpId, byChnId))
            {
                tEqp = g_cMcuVcApp.GetEqp(byEqpId);
                AddBrdChn(tEqp, byChnId, MAU_CHN_NORMAL);
            }
            else
            {
                break;
            }
        }

        byNVNeeded--;
    }

    while(byVGANeeded > 0)
    {
        tEqp.SetNull();
        byEqpId = 0;
        byChnId = 0;

        if (g_cMcuVcApp.GetIdleMauChn(MAU_CHN_VGA, byEqpId, byChnId))
        {
            tEqp = g_cMcuVcApp.GetEqp(byEqpId);
            AddBrdChn(tEqp, byChnId, MAU_CHN_VGA, TRUE);
        }
        else
        {
            break;
        }
        byVGANeeded --;
    }

    if (byH263Needed != 0 || byNVNeeded != 0 || byVGANeeded != 0)
    {
        ReleaseHdBasChn();
        OspPrintf(TRUE, FALSE, "[CBasMgr::OcuppyMau] failed as h263.%d, nv.%d, vga.%d\n", 
            byH263Needed, byNVNeeded, byNVNeeded);
    }
    else
    {
        AssignBasChn(tConfInfo, MODE_VIDEO);
        AssignBasChn(tConfInfo, MODE_SECVIDEO);
    }

    return byH263Needed == 0 && byNVNeeded == 0 && byVGANeeded == 0;

}

/*=============================================================================
  函 数 名： OcuppyMpu
  功    能： 适配管理类 占用本会议需要的mpu-bas（含VGA）
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/05/19    4.6         张宝卿        创建
=============================================================================*/
BOOL32 CBasMgr::OcuppyMpu(const TConfInfo &tConfInfo)
{
    u8 byChnIdle = 0;    
    g_cMcuVcApp.GetIdleMpu(byChnIdle);
    
    u8 byNVChn = 0;
    u8 byDSChn = 0;
    GetNeededMpu(tConfInfo, byNVChn, byDSChn);
    
    if (byChnIdle < byNVChn + byDSChn)
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::OcuppyMpu] failed due to idle<%d>, needed<%d,%d>\n", byChnIdle, byNVChn, byDSChn);
        return FALSE;
    }

    //尝试占用
    u8 byEqpId = 0;
    u8 byChnId = 0;
    
    TEqp tEqp;
    tEqp.SetNull();
    
    while (byNVChn > 0)
    {
        if (g_cMcuVcApp.GetIdleMpuChn(byEqpId, byChnId))
        {
            tEqp = g_cMcuVcApp.GetEqp(byEqpId);			
            AddBrdChn(tEqp, byChnId, MPU_CHN_NORMAL);
        }
        else
        {
            break;
        }
        byNVChn--;
    }

    while (byDSChn > 0)
    {
        if (g_cMcuVcApp.GetIdleMpuChn(byEqpId, byChnId))
        {
            tEqp = g_cMcuVcApp.GetEqp(byEqpId);			
            AddBrdChn(tEqp, byChnId, MPU_CHN_NORMAL, TRUE);
        }
        else
        {
            break;
        }
        byDSChn--;
    }

    if (byDSChn != 0 || byNVChn != 0)
    {
        ReleaseHdBasChn();
        OspPrintf(TRUE, FALSE, "[CBasMgr::OcuppyMpu] failed lack of NV.%d, DS.%d!\n", byNVChn, byDSChn);
    }
    else
    {
        AssignBasChn(tConfInfo, MODE_VIDEO);
        AssignBasChn(tConfInfo, MODE_SECVIDEO);
    }
    
    return byNVChn == 0 && byDSChn == 0;
}

/*=============================================================================
  函 数 名： AssignBasChn
  功    能： 适配管理类 根据会议情况给已占用的bas通道分配工作（即：给出适配参数）
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿          创建
    09/04/19    4.6         张宝卿          重写，兼容mpu
=============================================================================*/
void CBasMgr::AssignBasChn(const TConfInfo &tConfInfo, u8 byMode)
{
    if (MODE_VIDEO == byMode)
    {
        AssignMVBasChn(tConfInfo);
    }
    else if (MODE_SECVIDEO == byMode)
    {
        AssignDSBasChn(tConfInfo);
    }
    return;
}

/*=============================================================================
  函 数 名： AssignBasChn
  功    能： 适配管理类 主流适配相关通道参数初始分配
  算法实现： 1、主流bas通道依次尝试：高清速率适配、高清分辨率适配（从大到小）、H263适配；
             2、主流适配的帧率和码率需要到会议中微调；
  全局变量： 
  参    数：
  返 回 值： 
  
    Fixme: 遗留问题：
        1、主流适配输出参数在起停双流时的切变处理：码率；
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/04/19    4.6         张宝卿          重写，兼容mpu
=============================================================================*/
void CBasMgr::AssignMVBasChn(const TConfInfo &tConfInfo)
{
    u8 byMVChnIdx = 0;

    THDBasVidChnStatus tStatus;
    memset(&tStatus, 0, sizeof(tStatus));

    BOOL32 bBRAdpIgnored = FALSE;

    //1、双速
    if (0 != tConfInfo.GetSecBitRate())
    {
        //zbq[05/23/2009]视频资源紧凑使用时忽略纯速率适配
        if (g_cMcuVcApp.IsAdpResourceCompact())
        {
            bBRAdpIgnored = TRUE;
        }
        else
        {
	        THDAdaptParam tHDAdpParam;
	        memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));
	        
	        u8 byMediaType = tConfInfo.GetMainVideoMediaType();
	        tHDAdpParam.SetVidType(byMediaType);
	        tHDAdpParam.SetBitRate(tConfInfo.GetSecBitRate());
	        tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, byMediaType));
	        
	        u16 wWidth = 0;
	        u16 wHeight = 0;
	        GetWHByRes(tConfInfo.GetMainVideoFormat(), wWidth, wHeight);
	        tHDAdpParam.SetResolution(wWidth, wHeight);
	        
	        tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
	        tHDAdpParam.SetFrameRate(tConfInfo.GetMainVidUsrDefFPS());


	        m_tMVBrdChn.GetChn(byMVChnIdx, tStatus);
	        if (!tStatus.IsNull())
	        {
	            tStatus.SetOutputVidParam(tHDAdpParam, 0);
	            m_tMVBrdChn.UpdateChn(byMVChnIdx, tStatus);
	        }
	        else
	        {
	            OspPrintf(TRUE, FALSE, "[CBasMgr::AssignMVBasChn] MVChnIdx.%d is not ready \
yet, unexpected(DBR)\n", byMVChnIdx);
	        }

	        byMVChnIdx ++;
        }
    }

    //2、陆续的分辨率适配(1080不考虑)
    if (tConfInfo.GetConfAttrbEx().IsResEx720())
    {
        THDAdaptParam tHDAdpParam;
        memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));

        u8 byMediaType = tConfInfo.GetMainVideoMediaType();
        tHDAdpParam.SetVidType(byMediaType);
        
        //zbq[05/23/2009] 适配资源紧凑后，低分辨率统统降速，适配建立时进行微调
        if (bBRAdpIgnored)
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetSecBitRate());
        }
        else
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate());
        }
        
        tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, byMediaType));

        u16 wWidth = 0;
        u16 wHeight = 0;
        GetWHByRes(VIDEO_FORMAT_HD720, wWidth, wHeight);
        tHDAdpParam.SetResolution(wWidth, wHeight);

        tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
        tHDAdpParam.SetFrameRate(tConfInfo.GetMainVidUsrDefFPS());

        memset(&tStatus, 0, sizeof(tStatus));
        m_tMVBrdChn.GetChn(byMVChnIdx, tStatus);
        if (!tStatus.IsNull())
        {
            tStatus.SetOutputVidParam(tHDAdpParam, 0);
            m_tMVBrdChn.UpdateChn(byMVChnIdx, tStatus);
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[CBasMgr::AssignMVBasChn] MVChnIdx.%d is not ready \
yet, unexpected(720)\n", byMVChnIdx);
        }

        byMVChnIdx ++;
    }
    if (tConfInfo.GetConfAttrbEx().IsResEx4Cif())
    {
        THDAdaptParam tHDAdpParam;
        memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));

        u8 byMediaType = tConfInfo.GetMainVideoMediaType();
        tHDAdpParam.SetVidType(byMediaType);
        
        //zbq[05/23/2009] 适配资源紧凑后，低分辨率统统降速，适配建立时进行微调
        if (bBRAdpIgnored)
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetSecBitRate());
        }
        else
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate());
        }

        tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, byMediaType));
        
        u16 wWidth = 0;
        u16 wHeight = 0;
        GetWHByRes(VIDEO_FORMAT_4CIF, wWidth, wHeight);
        tHDAdpParam.SetResolution(wWidth, wHeight);
        
        tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
        tHDAdpParam.SetFrameRate(tConfInfo.GetMainVidUsrDefFPS());
        
        memset(&tStatus, 0, sizeof(tStatus));
        m_tMVBrdChn.GetChn(byMVChnIdx, tStatus);
        if (!tStatus.IsNull())
        {
            tStatus.SetOutputVidParam(tHDAdpParam, 0);
            m_tMVBrdChn.UpdateChn(byMVChnIdx, tStatus);
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[CBasMgr::AssignMVBasChn] MVChnIdx.%d is not ready \
yet, unexpected(4CIF)\n", byMVChnIdx);
        }

        byMVChnIdx ++;
    }

    //zbq[04/22/2009] 双格式主适配输出出空，填入主流格式和分辨率，对格式反向适配予以支持
    if (0 == byMVChnIdx)
    {
        if (0 != tConfInfo.GetSecVideoMediaType() &&
            MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType())
        {
            THDAdaptParam tHDAdpParam;
            memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));
            
            u8 byMediaType = tConfInfo.GetMainVideoMediaType();
            tHDAdpParam.SetVidType(byMediaType);
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate());
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, byMediaType));
            
            u16 wWidth = 0;
            u16 wHeight = 0;
            GetWHByRes(tConfInfo.GetMainVideoFormat(), wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
            tHDAdpParam.SetFrameRate(tConfInfo.GetMainVidUsrDefFPS());
            
            memset(&tStatus, 0, sizeof(tStatus));
            m_tMVBrdChn.GetChn(byMVChnIdx, tStatus);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 0);
                m_tMVBrdChn.UpdateChn(byMVChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignMVBasChn] MVChnIdx.%d is not ready \
yet, unexpected(MainVid)\n", byMVChnIdx);
            }
            
            byMVChnIdx ++;
        }
    }

    //3、Other和CIF的顺次挂接
    byMVChnIdx = 0;

    // other
    if (MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType())
    {
        THDAdaptParam tHDAdpParam;
        memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));

        u8 byMediaType = tConfInfo.GetSecVideoMediaType();
        
        //zbq[05/23/2009] 适配资源紧凑后，低分辨率统统降速，适配建立时进行微调
        if (bBRAdpIgnored)
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetSecBitRate());
        }
        else
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate());
        }

        tHDAdpParam.SetVidType(byMediaType);
        tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, byMediaType));

        u16 wWidth = 0;
        u16 wHeight = 0;
        u8 bySecVidFormat = tConfInfo.GetSecVideoFormat();
        if (VIDEO_FORMAT_AUTO == bySecVidFormat)
        {
            u16 wSecBR = tConfInfo.GetSecBitRate();
            if (wSecBR > 2048)
            {
                bySecVidFormat = VIDEO_FORMAT_4CIF;
            }
            else
            {
                bySecVidFormat = VIDEO_FORMAT_CIF;
            }
        }
        GetWHByRes(bySecVidFormat, wWidth, wHeight);
        tHDAdpParam.SetResolution(wWidth, wHeight);
        
        tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
        tHDAdpParam.SetFrameRate(tConfInfo.GetSecVidFrameRate());
        
        memset(&tStatus, 0, sizeof(tStatus));
        m_tMVBrdChn.GetChn(byMVChnIdx, tStatus);
        if (!tStatus.IsNull())
        {
            tStatus.SetOutputVidParam(tHDAdpParam, 1);
            m_tMVBrdChn.UpdateChn(byMVChnIdx, tStatus);
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[CBasMgr::AssignMVBasChn] MVChnIdx.%d is not ready \
yet, unexpected(other)\n", byMVChnIdx);
        }

        byMVChnIdx ++;
    }
    // cif
    if (tConfInfo.GetConfAttrbEx().IsResExCif())
    {
        THDAdaptParam tHDAdpParam;
        memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));

        u8 byMediaType = tConfInfo.GetMainVideoMediaType();
        
        //zbq[05/23/2009] 适配资源紧凑后，低分辨率统统降速，适配建立时进行微调
        if (bBRAdpIgnored)
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetSecBitRate());
        }
        else
        {
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate());
        }

        tHDAdpParam.SetVidType(byMediaType);
        tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, byMediaType));
        
        u16 wWidth = 0;
        u16 wHeight = 0;
        GetWHByRes(VIDEO_FORMAT_CIF, wWidth, wHeight);
        tHDAdpParam.SetResolution(wWidth, wHeight);
        
        tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
        tHDAdpParam.SetFrameRate(tConfInfo.GetMainVidUsrDefFPS());

        memset(&tStatus, 0, sizeof(tStatus));
        m_tMVBrdChn.GetChn(byMVChnIdx, tStatus);
        if (!tStatus.IsNull())
        {
            tStatus.SetOutputVidParam(tHDAdpParam, 1);
            m_tMVBrdChn.UpdateChn(byMVChnIdx, tStatus);
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[CBasMgr::AssignMVBasChn] MVChnIdx.%d is not ready \
yet, unexpected(cif)\n", byMVChnIdx);
        }

        byMVChnIdx ++;
    }
    return;
}

/*=============================================================================
  函 数 名： AssignDSBasChn
  功    能： 适配管理类 双流广播适配 初始参数分配
  算法实现： 1、双流bas通道直接给静态双流（如果有静态双流）；
             2、双流适配的帧率和码率也需要到会议中微调；
             3、mau作双流适配资源的时候，平行占用相关的所用通道；
             4、mpu作双流适配资源的时候，所有情况下的双流共享一个mpu通道，实时切换参数
  全局变量： 
  参    数：
  返 回 值： 
  
    Fixme: 遗留问题：
        1、双流h263p[0]: 264的输出参数的调整：分辨率；
        2、双流h263p[1]: 263p的输出参数的调整：分辨率；
        3、同主视频双流: 分辨率微调
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/04/19    4.6         张宝卿          重写，兼容mpu
=============================================================================*/
void CBasMgr::AssignDSBasChn(const TConfInfo &tConfInfo)
{
    THDBasVidChnStatus tStatus;
    memset(&tStatus, 0, sizeof(tStatus));

    u8 byDSChnIdx = 0;

    //MAU适配模式下：
    //  vga双流填辅通道；挂在MVChnIdx下；
    //  h263plus双流填主通道和辅通道；主辅通道均单出；
    //  同主视频双流填主通道，主通道单出；
    
    //MPU适配模式下：
    //  双流只占一个通道，需要根据双流源复用处理(双双流模式下 需要动态切换)

    TEqp tEqp;
	u8 byConfIdx = g_cMcuVcApp.GetConfIdx(tConfInfo.GetConfId());
    if (IsDualDouble())
    {
        if (!m_tDSBrdChn.IsGrpMpu())
        {
            //4.1 H263p适配参数的确认
            THDAdaptParam tHDAdpParam;
            memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));
            
            //输出h264（走主通道）
            tHDAdpParam.SetVidType(MEDIA_TYPE_H264);
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate()*tConfInfo.GetDStreamScale()/100);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H264));
            
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
            u8 byDStreamFPS = 5/*tConfInfo.GetDStreamUsrDefFPS()*/;
            tHDAdpParam.SetFrameRate(byDStreamFPS);
            
            u16 wWidth = 0;
            u16 wHeight = 0;
            GetWHByRes(VIDEO_FORMAT_XGA, wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            byDSChnIdx = 0;
            memset(&tStatus, 0, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 0);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] MVChnIdx.%d is not ready \
yet, unexpected(h263p: h264)\n", byDSChnIdx);
            }
            
            byDSChnIdx ++;
            
            
            //输出h263plus（走辅通道）
            tHDAdpParam.SetVidType(MEDIA_TYPE_H263PLUS);
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate()*tConfInfo.GetDStreamScale()/100);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H263PLUS));
            tHDAdpParam.SetFrameRate(tConfInfo.GetCapSupportEx().GetSecDSFrmRate());
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
            
            GetWHByRes(VIDEO_FORMAT_XGA, wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            memset(&tStatus, 0, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 0);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] DSChnIdx.%d is not ready \
yet, unexpected(h263p: h263p)\n", byDSChnIdx);
            }
            
            byDSChnIdx ++;
            
            //4.2 静态双流适配的确认
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate()*tConfInfo.GetDStreamScale()/100);
            tHDAdpParam.SetVidType(MEDIA_TYPE_H264);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H264));
            
            wWidth = 0;
            wHeight = 0;
            GetWHByRes(VIDEO_FORMAT_XGA, wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            u8 byUsrDefFps = 5;
            tHDAdpParam.SetFrameRate(byUsrDefFps);
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
            
            memset(&tStatus, byDSChnIdx, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 0);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] DSChnIdx.%d is not ready \
yet, unexpected(h263p: vga)\n", byDSChnIdx);
            }
            
            byDSChnIdx ++;
        }
        else
        {
            //mpu模式下只占一个通道，默认出：H264 XGA + H263p XGA
			//					   卫星版本：H264 XGA + H263  SVGA
            //4.1 H263p适配参数的确认
            THDAdaptParam tHDAdpParam;
            memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));
            
            //输出h264（走主通道）
            tHDAdpParam.SetVidType(MEDIA_TYPE_H264);
            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate()*tConfInfo.GetDStreamScale()/100);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H264));
            
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
            u8 byDStreamFPS = 5;
            tHDAdpParam.SetFrameRate(byDStreamFPS);
            
            u16 wWidth = 0;
            u16 wHeight = 0;
            GetWHByRes(VIDEO_FORMAT_XGA, wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            byDSChnIdx = 0;
            memset(&tStatus, 0, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 0);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] MVChnIdx.%d is not ready \
yet, unexpected(h263p: h264-mpu)\n", byDSChnIdx);
            }            
            
#ifndef _SATELITE_
            //输出h263plus（走辅通道）
            tHDAdpParam.SetVidType(MEDIA_TYPE_H263PLUS);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H263PLUS));
			GetWHByRes(VIDEO_FORMAT_XGA, wWidth, wHeight);
#else
            //卫星输出h263（走辅通道）
            tHDAdpParam.SetVidType(MEDIA_TYPE_H263);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H263));
			//GetWHByRes(VIDEO_FORMAT_SVGA, wWidth, wHeight);
			//强制编CIF
			GetWHByRes(VIDEO_FORMAT_CIF, wWidth, wHeight);
#endif
			tHDAdpParam.SetBitRate(tConfInfo.GetBitRate()*tConfInfo.GetDStreamScale()/100);
            tHDAdpParam.SetFrameRate(tConfInfo.GetCapSupportEx().GetSecDSFrmRate());
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());

            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            memset(&tStatus, 0, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 1);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] DSChnIdx.%d is not ready \
yet, unexpected(h263p: h263p-mpu)\n", byDSChnIdx);
            }
        }
    }
    else
    {
        if (IsDualEqMv())
        {
            //4.1 同主视频双流的确认
            byDSChnIdx = 0;
            THDAdaptParam tHDAdpParam;
            memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));

            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate()*tConfInfo.GetDStreamScale()/100);
            tHDAdpParam.SetVidType(MEDIA_TYPE_H264);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H264));
            
            u16 wWidth = 0;
            u16 wHeight = 0;
            GetWHByRes(VIDEO_FORMAT_4CIF, wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
            u8 byDStreamFPS = tConfInfo.GetDStreamUsrDefFPS();
            tHDAdpParam.SetFrameRate(byDStreamFPS);
            
            byDSChnIdx = 0;
            memset(&tStatus, 0, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 0);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] DSChnIdx.%d \
is not ready yet, unexpected(Eq MV-4cif)\n", byDSChnIdx);
            }

            wWidth = 0;
            wHeight = 0;
            GetWHByRes(VIDEO_FORMAT_CIF, wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());
            byDStreamFPS = tConfInfo.GetDStreamUsrDefFPS();
            tHDAdpParam.SetFrameRate(byDStreamFPS);
            
            byDSChnIdx = 0;
            memset(&tStatus, 0, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 1);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
            OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] DSChnIdx.%d \
is not ready yet, unexpected(Eq MV-cif)\n", byDSChnIdx);
            }
        }
        else
        {
            //4.1 静态双流适配的确认
            byDSChnIdx = 0;
            THDAdaptParam tHDAdpParam;
            memset(&tHDAdpParam, 0, sizeof(tHDAdpParam));

            tHDAdpParam.SetBitRate(tConfInfo.GetBitRate()*tConfInfo.GetDStreamScale()/100);
            tHDAdpParam.SetVidType(MEDIA_TYPE_H264);
            tHDAdpParam.SetVidActiveType(GetActivePL(tConfInfo, MEDIA_TYPE_H264));

            u16 wWidth = 0;
            u16 wHeight = 0;
            GetWHByRes(VIDEO_FORMAT_XGA, wWidth, wHeight);
            tHDAdpParam.SetResolution(wWidth, wHeight);
            tHDAdpParam.SetIsNeedByPrs(tConfInfo.GetConfAttrb().IsResendLosePack());

            u8 byUsrDefFps = 5;
            tHDAdpParam.SetFrameRate(byUsrDefFps);
            
            byDSChnIdx = 0;

            memset(&tStatus, 0, sizeof(tStatus));
            m_tDSBrdChn.GetChn(byDSChnIdx, tStatus);
			tEqp = tStatus.GetEqp();
			tEqp.SetConfIdx(byConfIdx);
			tStatus.SetEqp(tEqp);
            if (!tStatus.IsNull())
            {
                tStatus.SetOutputVidParam(tHDAdpParam, 0);
                m_tDSBrdChn.UpdateChn(byDSChnIdx, tStatus);
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBasMgr::AssignDSBasChn] DSChnIdx.%d is not ready \
yet, unexpected(vga)\n", byDSChnIdx);
            }
        }
    }

    return;
}

/*=============================================================================
  函 数 名： GetChnGrp
  功    能： 获取某bas通道的 参数信息
  算法实现： FIXME: 暂未支持选看组的获取(选看组获取没有意义？)
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/04/17    4.6         张宝卿        创建
=============================================================================*/
void CBasMgr::GetChnGrp(u8 &byNum, TBasChn *ptBasChn, u8 byAdpMode)
{
    if (NULL == ptBasChn)
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::GetChnGrp] ptBasChn.0x%x\n", ptBasChn);
        return;
    }

    if (CHN_ADPMODE_MVBRD != byAdpMode && CHN_ADPMODE_DSBRD != byAdpMode)
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::GetChnGrp] unexpected adp mode.%d!\n", byAdpMode);
        return;

    }
    byNum = 0;
    
    if (CHN_ADPMODE_DSBRD == byAdpMode)
    {
        m_tDSBrdChn.GetChn(byNum, ptBasChn);
    }
    else
    {
        m_tMVBrdChn.GetChn(byNum, ptBasChn);
    }

    return;
}

/*=============================================================================
  函 数 名： GetChnGrp
  功    能： 获取某bas通道的 适配模式：MVBRD/DSBRD/MVSEL/DSSEL
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/04/17    4.6         张宝卿        创建
=============================================================================*/
u8 CBasMgr::GetChnMode(const TEqp &tEqp, u8 byChnId)
{
    u8 byChnMode = CHN_ADPMODE_NONE;

    u8 byType = GetChnType(tEqp, byChnId);

    if (m_tMVBrdChn.IsChnExist(tEqp, byChnId, byType))
    {
        byChnMode = CHN_ADPMODE_MVBRD;
    }
    else if (m_tDSBrdChn.IsChnExist(tEqp, byChnId, byType))
    {
        byChnMode = CHN_ADPMODE_DSBRD;
    }
    else if (m_tMVSelChn.IsChnExist(tEqp, byChnId, byType))
    {
        byChnMode = CHN_ADPMODE_MVSEL;
    }
    else if (m_tDSSelChn.IsChnExist(tEqp, byChnId, byType))
    {
        byChnMode = CHN_ADPMODE_DSSEL;
    }

    return byChnMode;
}

/*=============================================================================
  函 数 名： ReleaseHdBasChn
  功    能： 适配管理类 释放本会议已经占用的所有bas
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿        创建
=============================================================================*/
void CBasMgr::ReleaseHdBasChn()
{
    u8 byIdx = 0;
    for(; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        THDBasVidChnStatus tStatus;
        memset(&tStatus, 0, sizeof(tStatus));
        if (m_tMVBrdChn.GetChn(byIdx, tStatus) && tStatus.IsReserved())
        {
            g_cMcuVcApp.ReleaseHDBasChn(tStatus.GetEqpId(), tStatus.GetChnIdx());
        }
    }
    m_tMVBrdChn.Clear();

    for(byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        THDBasVidChnStatus tStatus;
        memset(&tStatus, 0, sizeof(tStatus));
        if (m_tDSBrdChn.GetChn(byIdx, tStatus) && tStatus.IsReserved())
        {
            g_cMcuVcApp.ReleaseHDBasChn(tStatus.GetEqpId(), tStatus.GetChnIdx());
        }
    }

    m_tDSBrdChn.Clear();

    return;
}


/*=============================================================================
  函 数 名： AddBrdChn
  功    能： 适配管理类 增加占用一个Bas, 写入可支配数据区
  算法实现： 
  全局变量： 
  参    数： BOOL32 bH263p
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿        创建
=============================================================================*/
void CBasMgr::AddBrdChn(const TEqp &tEqp, u8 byChnId, u8 byChnType, BOOL32 bDual)
{
    //h263plus不进主流区；同主视频双流也不进主流区
    if (!bDual)
    {
        m_tMVBrdChn.AddChn(tEqp, byChnId, byChnType);
    }
    //双流
    else
    {
        m_tDSBrdChn.AddChn(tEqp, byChnId, byChnType);
    }
    return;
}

/*=============================================================================
  函 数 名： GetChnId
  功    能： 适配管理类 获取其适配组的索引
  算法实现： 广播适配通道 从1开始到6,
             主流选看通道从7开始
             双流选看要跳过主流选看的16×4 和广播适配通道
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/18    4.5         张宝卿        创建
=============================================================================*/
u8 CBasMgr::GetChnId(const TEqp &tEqp, u8 byChId)
{
    u8 byChnIdx = 0;
    u8 byChnType = GetChnType(tEqp, byChId);

    if (m_tMVBrdChn.IsChnExist(tEqp, byChId, byChnType))
    {
        byChnIdx = m_tMVBrdChn.GetChnPos(tEqp, byChId, byChnType);
    }
    else if (m_tDSBrdChn.IsChnExist(tEqp, byChId, byChnType))
    {
        byChnIdx = m_tDSBrdChn.GetChnPos(tEqp, byChId, byChnType) + MAXNUM_CONF_MVCHN;
    }
    else if (m_tMVSelChn.IsChnExist(tEqp, byChId, byChnType))
    {
        byChnIdx = m_tMVSelChn.GetChnPos(tEqp, byChId, byChnType) + MAXNUM_CONF_MVCHN + MAXNUM_CONF_DSCHN;
    }
    else if (m_tDSSelChn.IsChnExist(tEqp, byChId, byChnType))
    {
        byChnIdx = m_tMVSelChn.GetChnPos(tEqp, byChId, byChnType) + MAXNUM_CONF_MVCHN + MAXNUM_CONF_DSCHN + MAXNUM_PERIEQP * MAXNUM_MPU_VCHN;
    }

    return byChnIdx;
}


/*=============================================================================
  函 数 名： GetActivePL
  功    能： 适配管理类 获取动态载荷封装
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/18    4.5         张宝卿        创建
=============================================================================*/
u8 CBasMgr::GetActivePL(const TConfInfo &tConfInfo, u8 byMediaType)
{
    u8 byConfIdx = g_cMcuVcApp.GetConfIdx(tConfInfo.GetConfId());
    return GetActivePayload(tConfInfo, byMediaType);
}

/*=============================================================================
  函 数 名： GetChnStatus
  功    能： 适配管理类 获取通道
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/04/17    4.6         张宝卿        创建
=============================================================================*/
BOOL32 CBasMgr::GetChnStatus(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tStatus)
{
    u8 byType = GetChnType(tEqp, byChnId);
    
    if (MAU_CHN_NONE == byType)
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::GetChnStatus] byType.%d, reject it\n", byType);
        return FALSE;
    }
    
    if (m_tMVBrdChn.IsChnExist(tEqp, byChnId, byType) ||
        m_tDSBrdChn.IsChnExist(tEqp, byChnId, byType) ||
        m_tMVSelChn.IsChnExist(tEqp, byChnId, byType) ||
        m_tDSSelChn.IsChnExist(tEqp, byChnId, byType) )
    {
        TPeriEqpStatus tEqpStatus;
        if (!g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tEqpStatus))
        {
            return FALSE;
        }

        THDBasVidChnStatus *ptVidChn = NULL;

        u8 byType = GetChnType(tEqp, byChnId);
        if (MPU_CHN_NORMAL == byType)
        {
            ptVidChn= tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnId);
        }
        else
        {
            if (0 == byChnId)
            {
                ptVidChn= tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus();
            }
            else if (1 == byChnId)
            {
                ptVidChn= tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus();
            }
        }
        if (ptVidChn->IsNull())
        {
            return FALSE;
        }
        
        memcpy(&tStatus, ptVidChn, sizeof(tStatus));
        return TRUE;
    }

    OspPrintf(TRUE, FALSE, "[CBasMgr::GetChnStatus] Eqp<%d, %d> failed\n", tEqp.GetEqpId(), byChnId);

    return FALSE;
}

/*=============================================================================
  函 数 名： UpdateChn
  功    能： 适配管理类 更新通道状态
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/04/17    4.6         张宝卿        创建
=============================================================================*/
BOOL32 CBasMgr::UpdateChn(const TEqp &tEqp, u8 byChnId, THDBasVidChnStatus &tStatus)
{
    if(tStatus.IsNull())
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::UpdateChn] tStatus.IsNull for Eqp<%d, %d>, reject it\n", tEqp.GetEqpId(), byChnId);
        return FALSE;
    }
    
    u8 byType = GetChnType(tEqp, byChnId);
    if (MAU_CHN_NONE == byType)
    {
        OspPrintf(TRUE, FALSE, "[CBasMgr::UpdateChn] byChnType.%d, reject it\n", byType);
        return FALSE;
    }

    if (m_tMVBrdChn.IsChnExist(tEqp, byChnId, byType) ||
        m_tDSBrdChn.IsChnExist(tEqp, byChnId, byType) ||
        m_tMVSelChn.IsChnExist(tEqp, byChnId, byType) ||
        m_tDSSelChn.IsChnExist(tEqp, byChnId, byType) )
    {
        TPeriEqpStatus tEqpStatus;
        if (!g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tEqpStatus))
        {
            return FALSE;
        }

        u8 byType = GetChnType(tEqp, byChnId);
        if (MPU_CHN_NORMAL == byType)
        {
            tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tStatus, byChnId);
        }
        else
        {
            if (0 == byChnId)
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tStatus);
            }
            else if (1 == byChnId)
            {
                tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tStatus);
            }
        }
        g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tEqpStatus);
        
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
  函 数 名： GetLowBREqp
  功    能： 适配管理类 获取低速通道
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/12/11    4.5         张宝卿        创建
=============================================================================*/
BOOL32 CBasMgr::GetLowBREqp(const TConfInfo &tConfInfo, TEqp &tEqp, u8 &byChnId)
{
    BOOL32 bExist = FALSE;

    if (0 == tConfInfo.GetSecBitRate())
    {
        OspPrintf(TRUE, FALSE, "[GetLowBREqp] conf.%s has no sec bitrate, ignore it!\n", tConfInfo.GetConfName());
        return FALSE;
    }
    for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        THDBasVidChnStatus tVidChn;
        m_tMVBrdChn.GetChn(byIdx, tVidChn);
        if (tVidChn.IsNull())
        {
            continue;
        }
        //低速主出
        u16 wHeight = tVidChn.GetOutputVidParam(0)->GetHeight();
        u16 wWidth = tVidChn.GetOutputVidParam(0)->GetWidth();
        u8 byRes = GetResByWH(wWidth, wHeight);
        if(byRes == tConfInfo.GetMainVideoFormat() &&
           tVidChn.GetOutputVidParam(0)->GetBitrate() < tConfInfo.GetBitRate())
        {
            tEqp = tVidChn.GetEqp();
            byChnId = tVidChn.GetChnIdx();
            bExist = TRUE;
            break;
        }
    }

    return bExist;
}

/*=============================================================================
  函 数 名： GetBasResource
  功    能： 适配管理类
  算法实现： 
  全局变量： 
  参    数：u8 byMediaType
            u8 byRes
            TEqp &tHDBas, u8 &byChnId, u8 &byOutIdx, BOOL32 bDual
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/27    4.5         张宝卿        创建
=============================================================================*/
BOOL32 CBasMgr::GetBasResource(u8 byMediaType, u8 byRes,
                               TEqp &tHDBas, u8 &byChnId,
                               u8 &byOutIdx, BOOL32 bDual, BOOL32 bH263p)
{
    if (!bDual)
        return m_tMVBrdChn.GetBasResource(byMediaType, byRes, tHDBas, byChnId, byOutIdx);
    else
        return m_tDSBrdChn.GetBasResource(byMediaType, byRes, tHDBas, byChnId, byOutIdx, bH263p);
}

/*=============================================================================
  函 数 名： GetChnVcuTick
  功    能： 适配管理类
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/27    4.5         张宝卿        创建
=============================================================================*/
u32 CBasMgr::GetChnVcuTick(const TEqp &tEqp, u8 byChnIdx)
{
    u8 byType = GetChnType(tEqp, byChnIdx);

    if (m_tMVBrdChn.IsChnExist(tEqp, byChnIdx, byType))
    {
        return m_tMVBrdChn.GetChnVcuTick(tEqp, byChnIdx);
    }
    else if (m_tDSBrdChn.IsChnExist(tEqp, byChnIdx, byType))
    {
        return m_tDSBrdChn.GetChnVcuTick(tEqp, byChnIdx);
    }
    OspPrintf(TRUE, FALSE, "[GetChnVcuTick] tEqp<%d, %d> get tick failed!\n", tEqp.GetEqpId(), byChnIdx);
    return 0;
}

/*=============================================================================
  函 数 名： IsBrdGrpMpu
  功    能： 适配管理类
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/27    4.5         张宝卿        创建
=============================================================================*/
BOOL32 CBasMgr::IsBrdGrpMpu()
{
    return m_tDSBrdChn.IsGrpMpu();
}

/*=============================================================================
  函 数 名： SetChnVcuTick
  功    能： 适配管理类
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/27    4.5         张宝卿        创建
=============================================================================*/
BOOL32 CBasMgr::SetChnVcuTick(const TEqp &tEqp, u8 byChnIdx, u32 dwCurTicks)
{
    u8 byType = GetChnType(tEqp, byChnIdx);

    if (m_tMVBrdChn.IsChnExist(tEqp, byChnIdx, byType))
    {
        return m_tMVBrdChn.SetChnVcuTick(tEqp, byChnIdx, dwCurTicks);
    }
    else if (m_tDSBrdChn.IsChnExist(tEqp, byChnIdx, byType))
    {
        return m_tDSBrdChn.SetChnVcuTick(tEqp, byChnIdx, dwCurTicks);
    }

    OspPrintf(TRUE, FALSE, "[SetChnVcuTick] tEqp<%d, %d> set tick failed!\n", tEqp.GetEqpId(), byChnIdx);
    return FALSE;
}

/*=============================================================================
  函 数 名： Clear
  功    能： 适配管理类 清空
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿        创建
=============================================================================*/
void CBasMgr::Clear()
{
    m_emType = emBgin;
}

/*=============================================================================
  函 数 名： Print
  功    能： 适配管理类 打印
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/09    4.5         张宝卿        创建
=============================================================================*/
void CBasMgr::Print()
{
    THDBasVidChnStatus tStatus;

    u8 byIdx = 0;
    for (; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        memset(&tStatus, 0, sizeof(tStatus));

        m_tMVBrdChn.GetChn(byIdx, tStatus);
        if (!tStatus.IsNull())
        {
            OspPrintf(TRUE, FALSE, "[MV-Part]:\nTBasStatus.%d as follows:\n\n", tStatus.GetEqpId());
            tStatus.PrintInfo();
        }
        OspPrintf(TRUE, FALSE, "\n");
    }

    for (byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        memset(&tStatus, 0, sizeof(tStatus));

        m_tDSBrdChn.GetChn(byIdx, tStatus);
        if (!tStatus.IsNull())
        {
            OspPrintf(TRUE, FALSE, "[DS-Part]:\nTBasStatus.%d as follows:\n\n", tStatus.GetEqpId());
            tStatus.PrintInfo();
        }
        OspPrintf(TRUE, FALSE, "\n");
    }
    return;
}


/*=============================================================================
  函 数 名： TMVChnGrp
  功    能： 主流通道 构造
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/19    4.5         张宝卿        创建
=============================================================================*/
TMVChnGrp::TMVChnGrp()
{
    Clear();
}

/*=============================================================================
  函 数 名： Clear
  功    能： 主流通道
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/19    4.5         张宝卿        创建
=============================================================================*/
void TMVChnGrp::Clear()
{
    memset(this, 0, sizeof(TMVChnGrp));
}

/*=============================================================================
  函 数 名： GetBasResource
  功    能： 主流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/19    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TMVChnGrp::GetBasResource(u8 byMediaType, u8 byRes, TEqp &tHDBas, u8 &byChnId, u8 &byOutIdx)
{
    for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        for(u8 byOut = 0; byOut < MAXNUM_VOUTPUT; byOut++)
        {
            TEqp tMau = m_atChn[byIdx].GetEqp();
            if (tMau.IsNull())
            {
                continue;
            }
            TPeriEqpStatus tEqpStatus;
            if (!g_cMcuVcApp.GetPeriEqpStatus(tMau.GetEqpId(), &tEqpStatus))
            {
                continue;
            }

            u8 byEqpType = tEqpStatus.m_tStatus.tHdBas.GetEqpType();
            
            THDBasVidChnStatus *ptVidChn;
            
            switch (byEqpType)
            {
            case TYPE_MAU_NORMAL:
            case TYPE_MAU_H263PLUS:
                if (m_atChn[byIdx].GetChnId() == 0)
                {
                    ptVidChn = tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus();
                }
                else if (m_atChn[byIdx].GetChnId() == 1)
                {
                    ptVidChn = tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus();
                }
                break;
            case TYPE_MPU:
                ptVidChn = tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(m_atChn[byIdx].GetChnId());
                break;
            }

            if (ptVidChn->IsNull())
            {
                continue;
            }

            THDAdaptParam tHDAdpParam = *ptVidChn->GetOutputVidParam(byOut);

            if (!tHDAdpParam.IsNull() &&
                tHDAdpParam.GetVidType() == byMediaType &&
                //zbq[04/29/2009] mp4忽略分辨率，所有分辨率归一到CIF
                (byMediaType == MEDIA_TYPE_MP4 ||
                 GetResByWH(tHDAdpParam.GetWidth(), tHDAdpParam.GetHeight()) == byRes))
            {
                byChnId = m_atChn[byIdx].GetChnId(); 
                tHDBas = m_atChn[byIdx].GetEqp();
                byOutIdx = byOut;
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*=============================================================================
  函 数 名： SetChnVcuTick
  功    能： 主流通道 vcu tick保存
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/12/11    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TMVChnGrp::SetChnVcuTick(const TEqp &tEqp, u8 byChnIdx, u32 dwCurTicks)
{
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        if (tEqp == m_atChn[byIdx].GetEqp() &&
            byChnIdx == m_atChn[byIdx].GetChnId() )
        {
            m_adwLastVCUTick[byIdx] = dwCurTicks;
            return TRUE;
        }
    }
    OspPrintf(TRUE, FALSE, "[SetChnVcuTick] set eqp<%d, %d> vcu tick failed\n", tEqp.GetEqpId(), byChnIdx);
    return FALSE;
}

/*=============================================================================
  函 数 名： GetChnVcuTick
  功    能： 主流通道 vcu tick
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/12/11    4.5         张宝卿        创建
=============================================================================*/
u32 TMVChnGrp::GetChnVcuTick(const TEqp &tEqp, u8 byChnIdx)
{
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        if (tEqp == m_atChn[byIdx].GetEqp() &&
            byChnIdx == m_atChn[byIdx].GetChnId() )
        {
            return m_adwLastVCUTick[byIdx];
        }
    }
    OspPrintf(TRUE, FALSE, "[GetChnVcuTick] get eqp<%d, %d> vcu tick failed\n", tEqp.GetEqpId(), byChnIdx);
    return 0;
}

/*=============================================================================
  函 数 名： IsChnExist
  功    能： 主流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/20    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TMVChnGrp::IsChnExist(const TEqp &tEqp, u8 byChnIdx, u8 byType)
{
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        if (m_atChn[byIdx].GetEqp() == tEqp &&
            m_atChn[byIdx].GetChnId() == byChnIdx &&
            m_atChn[byIdx].GetType() == byType)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
  函 数 名： GetChnPos
  功    能： 主流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/20    4.5         张宝卿        创建
=============================================================================*/
u8 TMVChnGrp::GetChnPos(const TEqp &tEqp, u8 byChnIdx, u8 byType)
{
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        if (m_atChn[byIdx].GetEqp() == tEqp &&
            m_atChn[byIdx].GetChnId() == byChnIdx &&
            m_atChn[byIdx].GetType() == byType)
        {
            return byIdx;
        }
    }
    
    return 0xff;
}

/*=============================================================================
  函 数 名： AddChn
  功    能： 主流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/19    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TMVChnGrp::AddChn(const TEqp &tEqp, u8 byChnId, u8 byType)
{
    if (IsChnExist(tEqp, byChnId, byType))
    {
        return TRUE;
    }
    u8 byPos = 0xff;
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        if (m_atChn[byIdx].IsNull())
        {
            byPos = byIdx;
            break;
        }
    }
    if (byPos == 0xff)
    {
        OspPrintf(TRUE, FALSE, "[TMVChnGrp::AddChn] no pos for new chn<Eqp.%d, Chn.%d>\n",
                                tEqp.GetEqpId(), byChnId);
        return FALSE;
    }
    m_atChn[byPos].SetEqp(tEqp);
    m_atChn[byPos].SetChnId(byChnId);
    m_atChn[byPos].SetType(byType);

    //新通道到来，清空对应适配参数
    g_cMcuVcApp.ResetHDBasChn(tEqp.GetEqpId(), byChnId);

    return TRUE;
}

/*=============================================================================
  函 数 名： GetChn
  功    能： 主流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/19    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TMVChnGrp::GetChn(u8 byIdx, THDBasVidChnStatus &tChn)
{
    if (byIdx >= MAXNUM_CONF_MVCHN)
    {
        OspPrintf(TRUE, FALSE, "[TMVChnGrp::GetChn] unexpected idx.%d\n", byIdx);
        return FALSE;
    }

    TPeriEqpStatus tEqpStatus;
    if (!g_cMcuVcApp.GetPeriEqpStatus(m_atChn[byIdx].GetEqpId(), &tEqpStatus))
    {
        return FALSE;
    }

    u8 byChnId = m_atChn[byIdx].GetChnId();
    THdBasStatus tHdBasStatus = tEqpStatus.m_tStatus.tHdBas;
    u8 byChnType = GetChnType(m_atChn[byIdx].GetEqp(), byChnId);
    
    THDBasVidChnStatus *ptVidStatus = NULL;
    switch (byChnType)
    {
    case MAU_CHN_NORMAL:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetVidChnStatus();
        break;
    case MAU_CHN_VGA:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetDVidChnStatus();
        break;
    case MAU_CHN_264TO263:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetDVidChnStatus();
        break;
    case MAU_CHN_263TO264:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetVidChnStatus();
        break;
    case MPU_CHN_NORMAL:
        if (NULL == ptVidStatus)
        {
            ptVidStatus = tHdBasStatus.tStatus.tMpuBas.GetVidChnStatus(byChnId);
        }
        break;
    }
    
    if (NULL == ptVidStatus || ptVidStatus->IsNull())
    {
        return FALSE;
    }

    memcpy(&tChn, ptVidStatus, sizeof(tChn));
    return TRUE;
}

/*=============================================================================
  函 数 名： GetChn
  功    能： 主流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/19    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TMVChnGrp::GetChn(u8 &byNum, TBasChn *ptBasChn)
{
    if (NULL == ptBasChn)
    {
        OspPrintf(TRUE, FALSE, "[TMVChnGrp::GetChn] NULL == ptBasChn!\n");
        return FALSE;
    }
    byNum = 0;
    
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
    {
        if (m_atChn[byIdx].IsNull())
        {
            continue;
        }
        ptBasChn[byNum] = m_atChn[byIdx];
        byNum++;
    }
    return TRUE;
}

/*=============================================================================
  函 数 名： UpdateChn
  功    能： 主流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/19    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TMVChnGrp::UpdateChn(u8 byIdx, THDBasVidChnStatus &tChn)
{
    u8 byPos = 0xff;
    u8 byCurPos = 0xff;

    THDBasChnStatus tMVChn = (THDBasChnStatus)tChn;
    if (tChn.IsNull())
    {
        return FALSE;
    }

    TPeriEqpStatus tStatus;

    if (!g_cMcuVcApp.GetPeriEqpStatus(m_atChn[byIdx].GetEqpId(), &tStatus))
    {
        return FALSE;
    }
    
    u8 byChnId = m_atChn[byIdx].GetChnId();
    THdBasStatus tHdBasStatus = tStatus.m_tStatus.tHdBas;
    u8 byChnType = GetChnType(m_atChn[byIdx].GetEqp(), byChnId);
    
    switch (byChnType)
    {
    case MAU_CHN_NORMAL:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tChn);
        break;
    case MAU_CHN_VGA:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tChn);
        break;
    case MAU_CHN_264TO263:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tChn);
        break;
    case MAU_CHN_263TO264:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tChn);
        break;
    case MPU_CHN_NORMAL:
        tStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tChn, byChnId);
        break;
    }

    g_cMcuVcApp.SetPeriEqpStatus(m_atChn[byIdx].GetEqpId(), &tStatus);
	g_cMcuVcApp.SendPeriEqpStatusToMcs(m_atChn[byIdx].GetEqpId());

    return TRUE;
}

/*=============================================================================
  函 数 名： TDSChnGrp
  功    能： 双流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/19    4.5         张宝卿        创建
=============================================================================*/
TDSChnGrp::TDSChnGrp()
{
    Clear();
}

/*=============================================================================
  函 数 名： Clear
  功    能： 双流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/19    4.5         张宝卿        创建
=============================================================================*/
void TDSChnGrp::Clear()
{
    memset(this, 0, sizeof(TDSChnGrp));
}

/*=============================================================================
  函 数 名： IsChnExist
  功    能： 双流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/20    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TDSChnGrp::IsChnExist(const TEqp &tEqp, u8 byChnIdx, u8 byType)
{
    u8 byIdx = 0;
    for(byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        if (m_atChn[byIdx].GetEqp() == tEqp &&
            m_atChn[byIdx].GetChnId() == byChnIdx &&
            m_atChn[byIdx].GetType() == byType)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
  函 数 名： GetChnPos
  功    能： 双流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/20    4.5         张宝卿        创建
=============================================================================*/
u8 TDSChnGrp::GetChnPos(const TEqp &tEqp, u8 byChnIdx, u8 byType)
{
    u8 byIdx = 0;
    for(byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        if (m_atChn[byIdx].GetEqp() == tEqp &&
            m_atChn[byIdx].GetChnId() == byChnIdx &&
            m_atChn[byIdx].GetType() == byType)
        {
            return byIdx;
        }
    }
    
    return 0xff;
}

/*=============================================================================
  函 数 名： GetBasResource
  功    能： 双流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/27    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TDSChnGrp::GetBasResource(u8     byMediaType, 
                                 u8     byRes,
                                 TEqp  &tHDBas,
                                 u8    &byChnId,
                                 u8    &byOutIdx,
                                 BOOL32 bH263p)
{
    u8 byIdx = 0;
    u8 byOut = 0;

    for (byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        for(byOut = 0; byOut < MAXNUM_VOUTPUT; byOut++)
        {
            TEqp tMau = m_atChn[byIdx].GetEqp();
            if (tMau.IsNull())
            {
                continue;
            }
            TPeriEqpStatus tEqpStatus;
            if (!g_cMcuVcApp.GetPeriEqpStatus(tMau.GetEqpId(), &tEqpStatus))
            {
                continue;
            }

            u8 byEqpType = tEqpStatus.m_tStatus.tHdBas.GetEqpType();

            THDBasVidChnStatus *ptVidChn;

            switch (byEqpType)
            {
            case TYPE_MAU_NORMAL:
            case TYPE_MAU_H263PLUS:
                if (m_atChn[byIdx].GetChnId() == 0)
                {
                    ptVidChn = tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus();
                }
                else if (m_atChn[byIdx].GetChnId() == 1)
                {
                    ptVidChn = tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus();
                }
            	break;
            case TYPE_MPU:
                ptVidChn = tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(m_atChn[byIdx].GetChnId());
                break;
            }
            
            if (ptVidChn->IsNull())
            {
                continue;
            }

            u8 byChnType = GetChnType(m_atChn[byIdx].GetEqp(), m_atChn[byIdx].GetChnId());

            THDAdaptParam tHDAdpParam = *ptVidChn->GetOutputVidParam(byOut);
            if (!tHDAdpParam.IsNull() &&
                tHDAdpParam.GetVidType() == byMediaType &&
                GetResByWH(tHDAdpParam.GetWidth(), tHDAdpParam.GetHeight()) == byRes)
            {
                if (bH263p && byChnType != MAU_CHN_263TO264 && byChnType != MPU_CHN_NORMAL)
                {
                    continue;
                }
                byChnId = m_atChn[byIdx].GetChnId(); 
                tHDBas = m_atChn[byIdx].GetEqp();
                byOutIdx = byOut;
                return TRUE;
            }
        }
    }

    return FALSE;
}


/*=============================================================================
  函 数 名： IsGrpMpu
  功    能： 双流广播适配组是否基于MPU
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/06/02    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TDSChnGrp::IsGrpMpu()
{
    u8 byIdx = 0;
    
    for (byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        TEqp tMau = m_atChn[byIdx].GetEqp();
        if (tMau.IsNull())
        {
            continue;
        }
        TPeriEqpStatus tEqpStatus;
        if (!g_cMcuVcApp.GetPeriEqpStatus(tMau.GetEqpId(), &tEqpStatus))
        {
            continue;
        }
        
        u8 byEqpType = tEqpStatus.m_tStatus.tHdBas.GetEqpType();
        if (TYPE_MPU == byEqpType)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
  函 数 名： SetChnVcuTick
  功    能： 双流通道 VCU TICK
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/12/12    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TDSChnGrp::SetChnVcuTick(const TEqp &tEqp, u8 byChnIdx, u32 dwCurTicks)
{
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        if (tEqp == m_atChn[byIdx].GetEqp() &&
            byChnIdx == m_atChn[byIdx].GetChnId())
        {
            m_adwLastVCUTick[byIdx] = dwCurTicks;
            return TRUE;
        }
    }
    OspPrintf(TRUE, FALSE, "[SetChnVcuTick] set eqp<%d, %d> vcu tick failed(dual)\n", tEqp.GetEqpId(), byChnIdx);
    return FALSE;
}

/*=============================================================================
  函 数 名： GetChnVcuTick
  功    能： 双流通道 VCU TICK
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/12/12    4.5         张宝卿        创建
=============================================================================*/
u32 TDSChnGrp::GetChnVcuTick(const TEqp &tEqp, u8 byChnIdx)
{
    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        if (tEqp == m_atChn[byIdx].GetEqp() &&
            byChnIdx == m_atChn[byIdx].GetChnId())
        {
            return m_adwLastVCUTick[byIdx];
        }
    }
    OspPrintf(TRUE, FALSE, "[GetChnVcuTick] set eqp<%d, %d> vcu tick failed(dual)\n", tEqp.GetEqpId(), byChnIdx);
    return FALSE;
}


/*=============================================================================
  函 数 名： AddChn
  功    能： 双流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/19    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TDSChnGrp::AddChn(const TEqp &tEqp, u8 byChnId, u8 byType)
{
    if (IsChnExist(tEqp, byChnId, byType))
    {
        return TRUE;
    }
    u8 byPos = 0xff;

    for(u8 byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        if (m_atChn[byIdx].IsNull())
        {
            byPos = byIdx;
            break;
        }
    }
    if (0xff == byPos)
    {
        OspPrintf(TRUE, FALSE, "[TDSChnGrp::AddChn] no pos for new chn<Eqp.%d, Chn.%d>\n",
            tEqp.GetEqpId(), byChnId);
        return FALSE;
    }
    m_atChn[byPos].SetEqp(tEqp);
    m_atChn[byPos].SetChnId(byChnId);
    m_atChn[byPos].SetType(byType);

    //新通道到来，清空对应适配参数
    g_cMcuVcApp.ResetHDBasChn(tEqp.GetEqpId(), byChnId);

    return FALSE;
}

/*=============================================================================
  函 数 名： GetChn
  功    能： 双流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/19    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TDSChnGrp::GetChn(u8 byIdx, THDBasVidChnStatus &tChn)
{
    if (byIdx >= MAXNUM_CONF_DSCHN)
    {
        OspPrintf(TRUE, FALSE, "[TDSChnGrp::GetChn] unexpected idx.%d\n", byIdx);
        return FALSE;
    }
    TPeriEqpStatus tStatus;
    g_cMcuVcApp.GetPeriEqpStatus(m_atChn[byIdx].GetEqpId(), &tStatus);
    
    u8 byChnId = m_atChn[byIdx].GetChnId();
    THdBasStatus tHdBasStatus = tStatus.m_tStatus.tHdBas;
    u8 byChnType = GetChnType(m_atChn[byIdx].GetEqp(), byChnId);

    THDBasVidChnStatus *ptVidStatus = NULL;
    switch (byChnType)
    {
    case MAU_CHN_NORMAL:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetVidChnStatus();
        break;
    case MAU_CHN_VGA:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetDVidChnStatus();
        break;
    case MAU_CHN_264TO263:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetDVidChnStatus();
        break;
    case MAU_CHN_263TO264:
        ptVidStatus = tHdBasStatus.tStatus.tMauBas.GetVidChnStatus();
        break;
    case MPU_CHN_NORMAL:
        if (NULL == ptVidStatus)
        {
            ptVidStatus = tHdBasStatus.tStatus.tMpuBas.GetVidChnStatus(byChnId);
        }
        break;
    }

    if (NULL == ptVidStatus || ptVidStatus->IsNull())
    {
        return FALSE;
    }
    memcpy(&tChn, ptVidStatus, sizeof(tChn));

    return TRUE;
}

/*=============================================================================
  函 数 名： GetChn
  功    能： 双流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/19    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TDSChnGrp::GetChn(u8 &byNum, TBasChn *ptBasChn)
{
    if (NULL == ptBasChn)
    {
        OspPrintf(TRUE, FALSE, "[TDSChnGrp::GetChn] NULL == ptBasChn!\n");
        return FALSE;
    }
    byNum = 0;
    
    u8 byIdx = 0;
    for(byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
    {
        if (m_atChn[byIdx].IsNull())
        {
            continue;
        }
        ptBasChn[byNum] = m_atChn[byIdx];
        byNum++;
    }

    return TRUE;
}

/*=============================================================================
  函 数 名： UpdateChn
  功    能： 双流通道 集成
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/19    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TDSChnGrp::UpdateChn(u8 byIdx, THDBasVidChnStatus &tChn)
{
    u8 byPos = 0xff;
    THDBasChnStatus tDSChn = (THDBasChnStatus)tChn;

    if (tChn.IsNull())
    {
        OspPrintf(TRUE, FALSE, "[TDSChnGrp::UpdateChn] Idx.%d set null rejected!\n", byIdx);
        return FALSE;
    }

    TPeriEqpStatus tStatus;

    if (!g_cMcuVcApp.GetPeriEqpStatus(m_atChn[byIdx].GetEqpId(), &tStatus))
    {
        return FALSE;
    }

    u8 byChnId = m_atChn[byIdx].GetChnId();
    THdBasStatus tHdBasStatus = tStatus.m_tStatus.tHdBas;
    u8 byChnType = GetChnType(m_atChn[byIdx].GetEqp(), byChnId);
    
    switch (byChnType)
    {
    case MAU_CHN_NORMAL:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tChn);
        break;
    case MAU_CHN_VGA:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tChn);
        break;
    case MAU_CHN_264TO263:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetDVidChnStatus(tChn);
        break;
    case MAU_CHN_263TO264:
        tStatus.m_tStatus.tHdBas.tStatus.tMauBas.SetVidChnStatus(tChn);
        break;
    case MPU_CHN_NORMAL:
        tStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tChn, byChnId);
        break;
    }

    g_cMcuVcApp.SetPeriEqpStatus(m_atChn[byIdx].GetEqpId(), &tStatus);
    g_cMcuVcApp.SendPeriEqpStatusToMcs(m_atChn[byIdx].GetEqpId());

    return TRUE;
}


/********************************************

        适配接收群组管理实现

  *******************************************/

/*=============================================================================
  函 数 名： AddExcept
  功    能： 群组管理类 增加免适配成员，群组整理前，优先处理此工作
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void CRcvGrp::AddExcept(u8 byMtId, BOOL32 bDual)
{
    if (!bDual)
        m_tMVGrp.AddExcept(byMtId);
    else
        m_tDSGrp.AddExcept(byMtId);
    return;
}

/*=============================================================================
  函 数 名： AddMem
  功    能： 群组管理类 增加双流成员
  算法实现： 
  全局变量： 
  参    数： bExceptEither：是否同时立即增加到免适配区
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void CRcvGrp::AddMem(u8 byMtId, TDStreamCap &tDCap, BOOL32 bExc)
{
    u8 byRes = GetDSRes(tDCap.GetMediaType(), tDCap.GetResolution());
    m_tDSGrp.AddMem(byMtId, (TDSRcvGrp::emType)byRes, bExc);
    return;
}

/*=============================================================================
  函 数 名： AddMem
  功    能： 群组管理类 增加主流成员
  算法实现： 
  全局变量： 
  参    数： bExceptEither：是否同时立即增加到免适配区
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void CRcvGrp::AddMem(u8 byMtId, TSimCapSet &tSim, BOOL32 bExc)
{
    u8 byRes = GetMVRes(tSim.GetVideoMediaType(), tSim.GetVideoResolution());
    m_tMVGrp.AddMem(byMtId, (TMVRcvGrp::emType)byRes, bExc);
    return;
}

/*=============================================================================
  函 数 名： RemoveMem
  功    能： 群组管理类 增加主流成员
  算法实现： 
  全局变量： 
  参    数： bExceptEither：是否同时立即增加到免适配区
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void CRcvGrp::RemoveMem(u8 byMtId, TDStreamCap &tDCap)
{
    u8 byRes = GetDSRes(tDCap.GetMediaType(), tDCap.GetResolution());
    m_tDSGrp.RemoveMem(byMtId, (TDSRcvGrp::emType)byRes);
    return;
}

/*=============================================================================
  函 数 名： RemoveMem
  功    能： 群组管理类 增加主流成员
  算法实现： 
  全局变量： 
  参    数： bExceptEither：是否同时立即增加到免适配区
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void CRcvGrp::RemoveMem(u8 byMtId, TSimCapSet &tSim)
{
    u8 byRes = GetMVRes(tSim.GetVideoMediaType(), tSim.GetVideoResolution());
    m_tMVGrp.RemoveMem(byMtId, (TMVRcvGrp::emType)byRes);
    return;
}
/*=============================================================================
  函 数 名： ClearExpt
  功    能： 接收群组 清空免适配
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/02/06    4.5         薛亮          创建
=============================================================================*/
void CRcvGrp::ClearExpt(BOOL32 bDual)
{
	m_tMVGrp.ClearExpt();
	if(bDual)
	{
		m_tDSGrp.ClearExpt();
	}
    return;
}
/*=============================================================================
  函 数 名： GetMVRes
  功    能： 群组管理类 获取主流群组位置
  算法实现： 目前只处理格式和分辨率，帧率和码率暂不处理
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
u8 CRcvGrp::GetMVRes(u8 byType, u8 byRes)
{
    u8 byMVRes = TMVRcvGrp::emBegin;

    switch (byType)
    {
    case MEDIA_TYPE_H264:
        switch (byRes)
        {
        case VIDEO_FORMAT_CIF:  byMVRes = TMVRcvGrp::emCif;     break;
        case VIDEO_FORMAT_4CIF: byMVRes = TMVRcvGrp::em4Cif;    break;
        case VIDEO_FORMAT_HD720:    byMVRes = TMVRcvGrp::em720p;    break;
        case VIDEO_FORMAT_HD1080:   byMVRes = TMVRcvGrp::em1080;    break;
        default:    
            OspPrintf(TRUE, FALSE, "[CRcvGrp][GetMVRes] unexpected h264 Res.%d\n", byRes);
            break;
        }
        break;

    case MEDIA_TYPE_H263:
        byMVRes = TMVRcvGrp::emH263;
        break;
    case MEDIA_TYPE_MP4:
        byMVRes = TMVRcvGrp::emMp4;
        break;
    default:
        OspPrintf(TRUE, FALSE, "[CRcvGrp][GetMVRes] unexpected mtype.%d\n", byType);
        break;
    }
    return byMVRes;
}

void TMVRcvGrp::ResIn2Out(u8 byRes, u8 &byConfRes, u8 &byConfType)
{
    switch (byRes)
    {
    case emCif:  byConfRes = VIDEO_FORMAT_CIF; byConfType = MEDIA_TYPE_H264; break;
    case em4Cif: byConfRes = VIDEO_FORMAT_4CIF; byConfType = MEDIA_TYPE_H264; break;
    case em720p: byConfRes = VIDEO_FORMAT_HD720; byConfType = MEDIA_TYPE_H264; break;
    case em1080: byConfRes = VIDEO_FORMAT_HD1080; byConfType = MEDIA_TYPE_H264; break;
    case emH263: byConfRes = VIDEO_FORMAT_CIF; byConfType = MEDIA_TYPE_H263; break;
    case emMp4:  byConfRes = VIDEO_FORMAT_CIF; byConfType = MEDIA_TYPE_MP4; break;
    	break;
    default:
        OspPrintf(TRUE, FALSE, "[CRcvGrp][ResMVIn2Out] unexpected res.%d\n", byRes);
        break;
    }
}

void TDSRcvGrp::ResIn2Out(u8 byRes, u8 &byConfRes, u8 &byConfType)
{
    switch (byRes)
    {
    case emCif:  byConfRes = VIDEO_FORMAT_CIF; byConfType = MEDIA_TYPE_H264; break;
    case em4Cif: byConfRes = VIDEO_FORMAT_4CIF; byConfType = MEDIA_TYPE_H264; break;
    case em720p: byConfRes = VIDEO_FORMAT_HD720; byConfType = MEDIA_TYPE_H264; break;
    case emUXGA: byConfRes = VIDEO_FORMAT_HD1080; byConfType = MEDIA_TYPE_H264; break;
    case emSXGA: byConfRes = VIDEO_FORMAT_SXGA; byConfType = MEDIA_TYPE_H264; break;
    case emXGA:  byConfRes = VIDEO_FORMAT_XGA; byConfType = MEDIA_TYPE_H264; break;
    case emSVGA: byConfRes = VIDEO_FORMAT_SVGA; byConfType = MEDIA_TYPE_H264; break;
    case emVGA:  byConfRes = VIDEO_FORMAT_VGA; byConfType = MEDIA_TYPE_H264; break;
    case emH263plus: byConfRes = VIDEO_FORMAT_XGA; byConfType = MEDIA_TYPE_H263PLUS; break;
    	break;
    default:
        OspPrintf(TRUE, FALSE, "[CRcvGrp][ResDSIn2Out] unexpected res.%d\n", byRes);
        break;
    }
}

/*=============================================================================
  函 数 名： GetDSRes
  功    能： 群组管理类 获取双流群组位置
  算法实现： 目前只处理格式和分辨率，帧率和码率暂不处理
  全局变量： 
  参    数： TSimCapSet &tSim
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
u8 CRcvGrp::GetDSRes(u8 byType, u8 byRes)
{
    u8 byDSRes = TMVRcvGrp::emBegin;
    
    switch (byType)
    {
    case MEDIA_TYPE_H264:
        switch (byRes)
        {
        case VIDEO_FORMAT_CIF:  byDSRes = TDSRcvGrp::emCif;     break;
        case VIDEO_FORMAT_4CIF: byDSRes = TDSRcvGrp::em4Cif;    break;
        case VIDEO_FORMAT_HD720:    byDSRes = TDSRcvGrp::em720p;    break;
        case VIDEO_FORMAT_UXGA:     byDSRes = TDSRcvGrp::emUXGA;    break;
        case VIDEO_FORMAT_SXGA:     byDSRes = TDSRcvGrp::emSXGA;    break;
        case VIDEO_FORMAT_XGA:      byDSRes = TDSRcvGrp::emXGA;     break;
        case VIDEO_FORMAT_SVGA:     byDSRes = TDSRcvGrp::emSVGA;    break;
        case VIDEO_FORMAT_VGA:      byDSRes = TDSRcvGrp::emVGA;     break;
        default:    
            OspPrintf(TRUE, FALSE, "[CRcvGrp][GetDSRes] unexpected h264 Res.%d\n", byRes);
            break;
        }
        break;
	
	case MEDIA_TYPE_H263:	//临时卫星支持Polycom
    case MEDIA_TYPE_H263PLUS:
        byDSRes = TDSRcvGrp::emH263plus;
        break;
    default:
        OspPrintf(TRUE, FALSE, "[CRcvGrp][GetDSRes] unexpected mtype.%d\n", byType);
        break;
    }
    return byDSRes;
}

/*=============================================================================
  函 数 名： GetMVMtList
  功    能： 群组管理类 获取某类型终端的待适配列表（不在免适配区）
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/23    4.5         张宝卿        创建
=============================================================================*/
void CRcvGrp::GetMVMtList(u8 byType, u8 byRes, u8 &byNum, u8 *pbyMt, BOOL32 bForce)
{
    u8 byResNeed = GetMVRes(byType, byRes);
    m_tMVGrp.GetMtList(byResNeed, byNum, pbyMt, bForce);
    return;
}

/*=============================================================================
  函 数 名： GetDSMtList
  功    能： 群组管理类 获取某类型终端的待适配列表（不在免适配区）
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/23    4.5         张宝卿        创建
=============================================================================*/
void CRcvGrp::GetDSMtList(u8 byType, u8 byRes, u8 &byNum, u8 *pbyMt)
{
    u8 byResNeed = GetDSRes(byType, byRes);
    m_tDSGrp.GetMtList(byResNeed, byNum, pbyMt);
    return;
}

/*=============================================================================
  函 数 名： IsMtNeedAdp
  功    能： 群组管理类 获取某终端是否不在免适配区
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/23    4.5         张宝卿        创建
=============================================================================*/
BOOL32 CRcvGrp::IsMtNeedAdp(u8 byMtId, BOOL32 bMV)
{
    if (bMV)
        return !m_tMVGrp.IsMtInExcept(byMtId);
    else
        return !m_tDSGrp.IsMtInExcept(byMtId);
}

/*=============================================================================
  函 数 名： IsMtNeedAdp
  功    能： 群组管理类 获取群组是否需要适配
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/27    4.5         张宝卿        创建
=============================================================================*/
BOOL32 CRcvGrp::IsNeedAdp(BOOL32 bMV)
{
    if (bMV)
        return m_tMVGrp.IsNeedAdp();
    else
        return m_tDSGrp.IsNeedAdp();
}


/*=============================================================================
  函 数 名： GetMtMediaRes
  功    能： 群组管理类
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/23    4.5         张宝卿        创建
=============================================================================*/
BOOL32 CRcvGrp::GetMtMediaRes(u8 byMtId, u8 &byMediaType, u8 &byRes, BOOL32 bDual)
{
    if (!bDual)
        return m_tMVGrp.GetMtMediaRes(byMtId, byMediaType, byRes);
    else
        return m_tDSGrp.GetMtMediaRes(byMtId, byMediaType, byRes);
}

/*=============================================================================
  函 数 名： Clear
  功    能： 群组管理类 Clear
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void CRcvGrp::Clear()
{
    m_tMVGrp.Clear();
    m_tDSGrp.Clear();
}

/*=============================================================================
  函 数 名： Print
  功    能： 群组管理类 Print
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void CRcvGrp::Print()
{
    m_tMVGrp.Print();
    m_tDSGrp.Print();
}

/*=============================================================================
  函 数 名： AddMem
  功    能： 主流接收群组 增加成员
  算法实现： 
  全局变量： 
  参    数： bExceptEither：是否同时立即增加到免适配区
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void TMVRcvGrp::AddMem(u8 byMtId, emType type, BOOL32 bExceptEither)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT ||
        type >= emEnd || type <= emBegin)
    {
        OspPrintf(TRUE, FALSE, "[TMVRcvGrp] [AddMem] unexpected mt.%d or type.%d\n", byMtId, type);
        return;
    }
    m_aabyGrp[type][(byMtId-1)/8] |= 1 << ((byMtId-1) % 8);

    for(u8 byTypeId = emBegin+1; byTypeId < emEnd; byTypeId++)
    {
        if (byTypeId == type)
        {
            continue;
        }
        m_aabyGrp[byTypeId][(byMtId-1)/8] &= ~(1 << ((byMtId-1) % 8));
    }

    if (bExceptEither)
    {
        m_abyExcept[(byMtId-1)/8] |= 1 << ((byMtId-1) % 8);
    }
    else
    {
        m_abyExcept[(byMtId-1)/8] &= ~(1 << ((byMtId-1) % 8));
    }
    return;
}

/*=============================================================================
  函 数 名： AddExcept
  功    能： 主流接收群组 增加免适配成员
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void TMVRcvGrp::AddExcept(u8 byMtId)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT)
    {
        OspPrintf(TRUE, FALSE, "[TMVRcvGrp] [AddExcept] unexpected mt.%d\n", byMtId);
        return;
    }
    m_abyExcept[(byMtId-1)/8] |= 1 << ((byMtId-1) % 8);
    return;
}

/*=============================================================================
  函 数 名： RemoveMem
  功    能： 主流接收群组 删除成员，含免适配
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void TMVRcvGrp::RemoveMem(u8 byMtId, emType type)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT ||
        type >= emEnd || type <= emBegin)
    {
        OspPrintf(TRUE, FALSE, "[TMVRcvGrp] [RemvoeMem] unexpected mt.%d or type.%d\n", byMtId, type);
        return;
    }
    m_aabyGrp[type][(byMtId-1)/8] &= ~(1 << ((byMtId-1)%8));
    m_abyExcept[(byMtId-1)/8] &= ~(1 << ((byMtId-1)%8));
    return;
}

/*=============================================================================
  函 数 名： ClearExpt
  功    能： 主流接收群组 清空免适配
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/02/06    4.5         薛亮          创建
=============================================================================*/
void TMVRcvGrp::ClearExpt()
{
	memset(m_abyExcept,0,sizeof(m_abyExcept));
    return;
}
/*============================================================================
  函 数 名： IsMtInExcept
  功    能： 主流接收群组 判断终端是否在免适配区
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TMVRcvGrp::IsMtInExcept(u8 byMtId)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT)
    {
        OspPrintf(TRUE, FALSE, "[TMVRcvGrp] [IsMtInExcept] unexpected mt.%d\n", byMtId);
        return FALSE;
    }
    return m_abyExcept[(byMtId-1)/8] & (1 << ((byMtId-1)%8));
}

/*=============================================================================
  函 数 名： IsMtInExcept
  功    能： 主流接收群组 判断终端适配类型
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/27    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TMVRcvGrp::GetMtMediaRes(u8 byMtId, u8 &byMediaType, u8 &byRes)
{
    u8 byResId = 0;
    for (byResId = emBegin+1; byResId < emEnd; byResId ++)
    {
        if (m_aabyGrp[byResId][(byMtId-1)/8] & (1 << ((byMtId-1)%8)))
        {
            break;
        }
    }
    if (byResId != emEnd)
    {
        ResIn2Out(byResId, byRes, byMediaType);
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
  函 数 名： IsGrpNull
  功    能： 主流接收群组 某群组是否为空
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/01/05    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TMVRcvGrp::IsGrpNull(emType type)
{
    if(type == emBegin || type > emEnd)
    {
        OspPrintf(TRUE, FALSE, "[TMVRcvGrp::IsGrpNull] unexpected type.%d\n", type);
        return TRUE;
    }

    for(u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
    {
        if ((m_aabyGrp[type][(byMtId-1)/8] & (1 << ((byMtId-1)%8))))
        {
            return FALSE;
        }
    }
    return TRUE;
}

/*=============================================================================
  函 数 名： IsNeedAdp
  功    能： 主流接收群组 判断主流群组是否需要适配
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/27    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TMVRcvGrp::IsNeedAdp(void)
{
    u8 byType = 0;
    u8 byRes = 0;
    for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
    {
        if (GetMtMediaRes(byMtId, byType, byRes) &&
            !IsMtInExcept(byMtId))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
  函 数 名： GetMtList
  功    能： 主流接收群组 获取终端列
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void TMVRcvGrp::GetMtList(u8 byRes, u8 &byNum, u8 *pbyMt, BOOL32 bForce)
{
    if (emEnd <= byRes || emBegin == byRes)
    {
        OspPrintf(TRUE, FALSE, "[TMVRcvGrp::GetMtList] unexpected res.%d\n", byRes);
        return;
    }
    for(u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
    {
        if ((m_aabyGrp[byRes][(byMtId-1)/8] & (1 << ((byMtId-1)%8))) 
			&& (!IsMtInExcept(byMtId) || bForce) )
        {
            pbyMt[byNum] = byMtId;
            byNum++;
        }
    }
    return;
}


/*=============================================================================
  函 数 名： Print
  功    能： 主流接收群组 打印当前群组成员
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void TMVRcvGrp::Print()
{
    u8 byRes = 0;
    OspPrintf(TRUE, FALSE, "MVGrp:\n");
    for(byRes = emBegin+1; byRes < emEnd; byRes ++)
    {
        u8 byNum = 0;
        u8 abyMt[MAXNUM_CONF_MT];
        memset(&abyMt, 0, sizeof(abyMt));
        GetMtList(byRes, byNum, &abyMt[0]);
        if (byNum != 0)
        {
            u8 byConfRes = 0;
            u8 byMediaType = 0;
            ResIn2Out(byRes, byConfRes, byMediaType);
            OspPrintf(TRUE, FALSE, "\tRes.%s, Media.%d\n", GetResStr(byConfRes), byMediaType);
        }
        for(u8 byMtIdx = 0; byMtIdx < byNum; byMtIdx++)
        {
            OspPrintf(TRUE, FALSE, "\t    Mt.%d\n", abyMt[byMtIdx]);
        }
        OspPrintf(TRUE, FALSE, "\n");
    }

    OspPrintf(TRUE, FALSE, "\tExcept:\n");
    for(u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
    {
        if (IsMtInExcept(byMtId))
        {
            OspPrintf(TRUE, FALSE, "\t    Mt.%d\n", byMtId);
        }
    }

    OspPrintf(TRUE, FALSE, "\n");
}

/*=============================================================================
  函 数 名： AddMem
  功    能： 双流接收群组 增加成员
  算法实现： 
  全局变量： 
  参    数： bExceptEither：是否同时立即增加到免适配区
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void TDSRcvGrp::AddMem(u8 byMtId, emType type, BOOL32 bExceptEither)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT ||
        type >= emEnd || type <= emBegin)
    {
        OspPrintf(TRUE, FALSE, "[TDSRcvGrp] [AddMem] unexpected mt.%d or type.%d\n", byMtId, type);
        return;
    }
    m_aabyGrp[type][(byMtId-1)/8] |= 1 << ((byMtId-1) % 8);

    for(u8 byTypeId = emBegin+1; byTypeId < emEnd; byTypeId++)
    {
        if (byTypeId == type)
        {
            continue;
        }
        m_aabyGrp[byTypeId][(byMtId-1)/8] &= ~(1 << ((byMtId-1) % 8));
    }

    if (bExceptEither)
    {
        m_abyExcept[(byMtId-1)/8] |= 1 << ((byMtId-1) % 8);
    }
    else
    {
        m_abyExcept[(byMtId-1)/8] &= ~(1 << ((byMtId-1) % 8));
    }
    return;
}

/*=============================================================================
  函 数 名： AddExcept
  功    能： 双流接收群组 增加免适配区成员
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void TDSRcvGrp::AddExcept(u8 byMtId)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT)
    {
        OspPrintf(TRUE, FALSE, "[TDSRcvGrp] [AddExcept] unexpected mt.%d\n", byMtId);
        return;
    }
    m_abyExcept[(byMtId-1)/8] |= 1 << ((byMtId-1) % 8);
    return;
}

/*=============================================================================
  函 数 名： AddExcept
  功    能： 双流接收群组 删除成员，含免适配区
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void TDSRcvGrp::RemoveMem(u8 byMtId, emType type)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT ||
        type >= emEnd || type <= emBegin)
    {
        OspPrintf(TRUE, FALSE, "[TDSRcvGrp] [RemvoeMem] unexpected mt.%d or type.%d\n", byMtId, type);
        return;
    }
    m_aabyGrp[type][(byMtId-1)/8] &= ~(1 << ((byMtId-1)%8));
    m_abyExcept[(byMtId-1)/8] &= ~(1 << ((byMtId-1)%8));
    return;
}
/*=============================================================================
  函 数 名： ClearExpt
  功    能： 双流接收群组 清空免适配
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/02/06    4.5         薛亮          创建
=============================================================================*/
void TDSRcvGrp::ClearExpt()
{
	memset(m_abyExcept,0,sizeof(m_abyExcept));
    return;
}
/*=============================================================================
  函 数 名： IsMtInExcept
  功    能： 双流接收群组 判断终端是否在免适配区
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TDSRcvGrp::IsMtInExcept(u8 byMtId)
{
    if (byMtId < 1 || byMtId > MAXNUM_CONF_MT)
    {
        OspPrintf(TRUE, FALSE, "[TDSRcvGrp] [IsMtInExcept] unexpected mt.%d\n", byMtId);
        return FALSE;
    }
    return m_abyExcept[(byMtId-1)/8] & (1 << ((byMtId-1)%8));
}

/*=============================================================================
  函 数 名： GetMtMediaRes
  功    能： 双流接收群组 获取终端的群组类型
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TDSRcvGrp::GetMtMediaRes(u8 byMtId, u8 &byMediaType, u8 &byRes)
{
    u8 byResId = 0;
    for (byResId = emBegin+1; byResId < emEnd; byResId ++)
    {
        if (m_aabyGrp[byResId][(byMtId-1)/8] & (1 << ((byMtId-1)%8)))
        {
            break;
        }
    }
    if (byResId != emEnd)
    {
        ResIn2Out(byResId, byRes, byMediaType);
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
  函 数 名： IsNeedAdp
  功    能： 双流接收群组 获取双流适配么
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/27    4.5         张宝卿        创建
=============================================================================*/
BOOL32 TDSRcvGrp::IsNeedAdp()
{
    u8 byType = 0;
    u8 byRes = 0;
    for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
    {
        if (GetMtMediaRes(byMtId, byType, byRes) &&
            !IsMtInExcept(byMtId))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
  函 数 名： GetMtList
  功    能： 双流接收群组 获取终端列表
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void TDSRcvGrp::GetMtList(u8 byRes, u8 &byNum, u8 *pbyMt)
{
    if (emEnd <= byRes || emBegin == byRes)
    {
        OspPrintf(TRUE, FALSE, "[TDSRcvGrp::GetMtList] unexpected res.%d\n", byRes);
        return;
    }
    for(u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
    {
        if ((m_aabyGrp[byRes][(byMtId-1)/8] & (1 << ((byMtId-1)%8))) &&
            !IsMtInExcept(byMtId))
        {
            pbyMt[byNum] = byMtId;
            byNum++;
        }
    }
    return;
}

/*=============================================================================
  函 数 名： IsMtInExcept
  功    能： 双流接收群组 打印适配组成员，含免适配区
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/07    4.5         张宝卿        创建
=============================================================================*/
void TDSRcvGrp::Print()
{
    OspPrintf(TRUE, FALSE, "DSGrp:\n");
    for(u8 byRes = emBegin+1; byRes < emEnd; byRes ++)
    {
        u8 byNum = 0;
        u8 abyMt[MAXNUM_CONF_MT];
        memset(&abyMt, 0, sizeof(abyMt));
        GetMtList(byRes, byNum, &abyMt[0]);
        if (byNum != 0)
        {
            u8 byConfRes = 0;
            u8 byMediaType = 0;
            ResIn2Out(byRes, byConfRes, byMediaType);
            OspPrintf(TRUE, FALSE, "\tRes.%s, Media.%d\n", GetResStr(byConfRes), byMediaType);
        }
        for(u8 byMtIdx = 0; byMtIdx < byNum; byMtIdx++)
        {
            OspPrintf(TRUE, FALSE, "\t    Mt.%d\n", abyMt[byMtIdx]);
        }
    }
    OspPrintf(TRUE, FALSE, "\n");
    OspPrintf(TRUE, FALSE, "\tExcept:\n");
    for(u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
    {
        if (IsMtInExcept(byMtId))
        {
            OspPrintf(TRUE, FALSE, "\t    Mt.%d\n", byMtId);
        }
    }
    OspPrintf(TRUE, FALSE, "\n");
    return;
}

/*=============================================================================
  函 数 名： GetApplyFreqInfo
  功    能： 获取卫星会议的申请频点
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  09/08/28    4.6         张宝卿        创建
=============================================================================*/
void CMcuVcData::GetApplyFreqInfo(u32 &dwSatIp, u16 &wSatPort)
{
    if (0 == m_tSatInfo.GetApplyIp() || 0 == m_tSatInfo.GetApplyPort())
    {
        OspPrintf(TRUE, FALSE, "[GetApplyFreqInfo] sat apply addr unexist!\n");
        return;
    }
    dwSatIp = htonl(m_tSatInfo.GetApplyIp());
    wSatPort = m_tSatInfo.GetApplyPort();
    return;
}


/*====================================================================
    函数名      SetConfAllocFreq
    功能       设置会议分配频率
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
    09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
void CMcuVcData::SetConfGetSendFreq( u8 byConfIdx, BOOL bTrue )
{
	if ( bTrue )
	{
		m_abySendConfFreq[byConfIdx-1] = 1;
	}
	else
	{
		m_abySendConfFreq[byConfIdx-1] = 0;
	}
}

/*====================================================================
    函数名      SetConfAllocFreq
    功能       设置会议分配频率
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
    09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
void CMcuVcData::SetConfGetReceiveFreq( u8 byConfIdx, BOOL bTrue )
{
	if ( bTrue )
	{
		m_abyReceiveConfFreq[byConfIdx-1] = 1;
	}
	else
	{
		m_abyReceiveConfFreq[byConfIdx-1] = 0;
	}
}

/*====================================================================
    函数名      IsConfAllocFreq
    功能       检测会议是否分配频率
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
    09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
BOOL CMcuVcData::IsConfGetAllFreq( u8 byConfIdx )
{
	return ( IsConfGetSendFreq( byConfIdx ) && IsConfGetReceiveFreq( byConfIdx ) );
}

/*====================================================================
    函数名      IsConfAllocFreq
    功能       检测会议是否分配频率
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
    09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
BOOL CMcuVcData::IsConfGetSendFreq( u8 byConfIdx )
{
	return ( m_abySendConfFreq[byConfIdx-1] == 1 );
}
/*====================================================================
    函数名      IsConfAllocFreq
    功能       检测会议是否分配频率
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
    09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
BOOL CMcuVcData::IsConfGetReceiveFreq( u8 byConfIdx )
{
	return ( m_abyReceiveConfFreq[byConfIdx-1] == 1 );
}


/*====================================================================
  函数名      SetConfBitRate
  功能       会议发送/接收码率
  算法实现    ：
  引用全局变量：
  输入参数说明：

  返回值说明  ：无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  04/04/06    1.0         zhangsh         创建
  09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
void CMcuVcData::SetConfBitRate( u8 byConfIdx, u32 dwSend, u32 dwRecv )
{
    for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
    {
        if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
        {
            if ( dwSend != 0 )
			{
                m_atConfFreq[byLoop].dwSendBitRate = dwSend;
			}
            if ( dwRecv != 0 )
			{
                m_atConfFreq[byLoop].dwRevBitRate = dwRecv;
			}
            return;
        }
    }
    return ;
}


/*====================================================================
    函数名      SetConfInfo
    功能       得到会议频率，码率信息的空结构
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
    09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
void CMcuVcData::SetConfInfo( u8 byConfIdx )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == 0 )
		{
			m_atConfFreq[byLoop].byConfIdx = byConfIdx;
			break;
		}
	}
	return;
}

/*====================================================================
    函数名      ReleaseConfInfo
    功能       释放会议频率，码率信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
    09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
void CMcuVcData::ReleaseConfInfo( u8 byConfIdx )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			memset( &m_atConfFreq[byLoop], 0, sizeof( TConfFreqBitRate ) );
			break;
		}
	}
	return;
}

/*====================================================================
    函数名      SetConfFreq
    功能       会议发送/接收频率
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
    09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
void CMcuVcData::SetConfFreq( u8 byConfIdx, u32 dwSend, u8 byPos, u32 dwRecv )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			if ( dwSend != 0 )
			{
				m_atConfFreq[byLoop].dwSendFreq = dwSend;
			}
			if ( dwRecv != 0 )
			{
				m_atConfFreq[byLoop].dwRevFreq[byPos] = dwRecv;
			}
			return;
		}
	}
	return ;
}

/*====================================================================
    函数名      GetConfSndFreq
    功能       得到会议发送频率
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
    09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
u32 CMcuVcData::GetConfSndFreq( u8 byConfIdx )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			return m_atConfFreq[byLoop].dwSendFreq;
		}
	}
	return 0;
}

/*====================================================================
    函数名      GetConfRcvFreq
    功能       得到会议接收频率
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
    09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
u32 CMcuVcData::GetConfRcvFreq( u8 byConfIdx, u8 byPos )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			return m_atConfFreq[byLoop].dwRevFreq[byPos];
		}
	}
	return 0;
}

/*====================================================================
    函数名      GetConfSndBitRate
    功能       得到会议发送码率
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
    09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
u32 CMcuVcData::GetConfSndBitRate( u8 byConfIdx )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			return m_atConfFreq[byLoop].dwSendBitRate;
		}
	}
	return 0;
}
/*====================================================================
    函数名      GetConfRcvBitRate
    功能       得到会议接收码率
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
    09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
u32 CMcuVcData::GetConfRcvBitRate( u8 byConfIdx )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			return m_atConfFreq[byLoop].dwRevBitRate;
		}
	}
	return 0;
}

/*====================================================================
    函数名      GetConfRcvNum
    功能       得到会议接收频点个数
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
    09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
u8 CMcuVcData::GetConfRcvNum( u8 byConfIdx )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			return m_atConfFreq[byLoop].byNum;
		}
	}
	return 0;
}

/*====================================================================
    函数名      SetConfRcvNum
    功能       设置会议接收频点个数
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
    09/08/28    4.6         张宝卿          从卫星移植过来
====================================================================*/
void CMcuVcData::SetConfRcvNum( u8 byConfIdx, u8 byTotal )
{
	for ( u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF ; byLoop ++ )
	{
		if ( m_atConfFreq[byLoop].byConfIdx == byConfIdx )
		{
			m_atConfFreq[byLoop].byNum = byTotal;
		}
	}
	return ;
}

/*====================================================================
    函数名      GetRcvMtSignalPort
    功能        获取来自终端侧卫星协议信令的端口
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/08/28    4.6         张宝卿          创建
====================================================================*/
u16 CMcuVcData::GetRcvMtSignalPort( void ) const
{
    return m_tSatInfo.GetRcvMtSignalPort();
}

u16 CMcuVcData::GetApplyRcvPort( void ) const
{
    return m_tSatInfo.GetApplyRcvPort();
}

u32 CMcuVcData::GetTimeRefreshIpAddr( void ) const
{
    return m_tSatInfo.GetTimeRefreshIpAddr();
}

u16 CMcuVcData::GetTimeRefreshPort( void ) const
{
    return m_tSatInfo.GetTimeRefreshPort();
}


/*====================================================================
    函数名      SetMcuModemConnected
    功能       Mcu Modem进入连接状态
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
====================================================================*/
void CMcuVcData::SetMcuModemConnected( u16 wModemId, BOOL bConnected )
{
	if ( wModemId == 0 || wModemId > MAXNUM_MCU_MODEM )
		return;
	m_atModemData[wModemId - 1 ].bConnected = bConnected;
	return;
}
/*====================================================================
    函数名      GetIdleMcuModemForSnd
    功能       可以发送的
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
====================================================================*/
u8 CMcuVcData::GetIdleMcuModemForSnd()
{
	for ( u8 byLoop = 1 ; byLoop < MAXNUM_MCU_MODEM+1 ; byLoop ++ )
	{
		if ( m_atModemData[byLoop-1].wConfIdxSnd == 0 && m_atModemData[byLoop-1].bConnected == TRUE )
		{
			return byLoop;
		}
	}
	return 0;
}
/*====================================================================
    函数名      ReleaseConfModem
    功能       释放会议中的MODEM
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
====================================================================*/
void CMcuVcData::ReleaseSndMcuModem( u8 byModemId )
{
	if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM )
		return;
	m_atModemData[byModemId-1].wConfIdxSnd = 0;
	m_atModemData[byModemId-1].tMtSnd.SetNull();
	m_atModemData[byModemId-1].bConnected = TRUE;
}


/*====================================================================
    函数名      GetIdleMcuModemForRcv
    功能       得到一个MCU的未使用MODEM来发送
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
====================================================================*/
u8 CMcuVcData::GetIdleMcuModemForRcv()
{
	for ( u8 byLoop = 1 ; byLoop < MAXNUM_MCU_MODEM+1 ; byLoop ++ )
	{
		if ( m_atModemData[byLoop-1].wConfIdxRcv == 0 && m_atModemData[byLoop-1].bConnected == TRUE )
		{
			return byLoop;
		}
	}
	return 0;
}
/*====================================================================
    函数名      ReleaseRcvMcuModem
    功能       释放会议中的MODEM
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
====================================================================*/
void CMcuVcData::ReleaseRcvMcuModem( u8 byModemId )
{
	if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM )
		return;
	m_atModemData[byModemId-1].wConfIdxRcv = 0;
	//m_atModemData[byModemId-1].wMtId = 0;
	m_atModemData[byModemId-1].tMtRcv.SetNull();
	m_atModemData[byModemId-1].bConnected = TRUE;
}

/*====================================================================
    函数名      SetMcuModemSndData
    功能       设置MCU MDOEM的数据
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
====================================================================*/
void CMcuVcData::SetMcuModemSndData( u8 byModemId, u8 byConfIdx, u32 dwSendFreq, u32 dwSendBit)
{
	if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM )
		return;
	m_atModemData[byModemId-1].wConfIdxSnd = byConfIdx;
	m_atModemData[byModemId-1].dwSendBitRate = dwSendBit;
	m_atModemData[byModemId-1].dwSendFrequece = dwSendFreq;
	return;
}

/*====================================================================
    函数名      GetMcuModemRcvData
    功能       得到一个MCU的MODEM的数据
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
====================================================================*/
void CMcuVcData::GetMcuModemSndData( u8 byModemId , u32 &dwRevFreq, u32 &dwRevBit )
{
	if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM )
	{
		dwRevFreq = 0;
		dwRevBit = 0;
		return;
	}
	dwRevBit = m_atModemData[byModemId-1].dwSendBitRate;
	dwRevFreq= m_atModemData[byModemId-1].dwSendFrequece ;
	return;
}
/*====================================================================
    函数名      SetMcuModemRcvData
    功能       设置MCU MDOEM的数据
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
====================================================================*/
void CMcuVcData::SetMcuModemRcvData( u8 byModemId, u8 byConfIdx, u32 dwRevFreq, u32 dwRevBit )
{
	if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM )
    {
		return;
    }
	m_atModemData[byModemId-1].wConfIdxRcv = byConfIdx;
	m_atModemData[byModemId-1].dwRevBitRate = dwRevBit;
	m_atModemData[byModemId-1].dwRevFrequece = dwRevFreq;
	return;
}

/*====================================================================
    函数名      GetMcuModemRcvData
    功能       得到一个MCU的MODEM的数据
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
====================================================================*/
void CMcuVcData::GetMcuModemRcvData( u8 byModemId , u32 &dwRevFreq, u32 &dwRevBit )
{
	if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM )
	{
		dwRevFreq = 0;
		dwRevBit = 0;
		return;
	}
	dwRevBit = m_atModemData[byModemId-1].dwRevBitRate;
	dwRevFreq= m_atModemData[byModemId-1].dwRevFrequece ;
	return;
}

/*====================================================================
    函数名      SetModemSportNum
    功能       :
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
====================================================================*/
void CMcuVcData::SetModemSportNum(u8 byModemId, u8 byNum )
{
    if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM+1 )
	{
        return;
	}
    m_atModemData[byModemId-1].bySPortNum = byNum;
    return ;
}

/*====================================================================
    函数名      GetModemSportNum
    功能       :
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
====================================================================*/
u8 CMcuVcData::GetModemSportNum( u8 byModemId )
{
    if ( byModemId == 0 || byModemId > MAXNUM_MCU_MODEM+1 )
	{
        return 0;
	}
    return m_atModemData[byModemId-1].bySPortNum;
}

/*====================================================================
    函数名      GetConfRcvModem
    功能       :
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
====================================================================*/
u8 CMcuVcData::GetConfRcvModem(u8 byConfIdx )
{
	for ( u8 byLoop = 1; byLoop <= MAXNUM_MCU_MODEM ; byLoop ++ )
	{
		if ( m_atModemData[byLoop-1].wConfIdxRcv == byConfIdx )
		{
			return byLoop;
		}
	}
	return 0;
}

/*====================================================================
    函数名      GetConfSndModem
    功能       :
    算法实现    ：
    引用全局变量：
    输入参数说明：
				  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/06    1.0         zhangsh         创建
====================================================================*/
u8 CMcuVcData::GetConfSndModem(u8 byConfIdx )
{
	for ( u8 byLoop = 1; byLoop <= MAXNUM_MCU_MODEM; byLoop ++ )
	{
		if ( m_atModemData[byLoop-1].wConfIdxSnd == byConfIdx )
		{
			return byLoop;
		}
	}
	return 0;
}

/*====================================================================
  函数名      DisplayModemInfo
  功能       显示MCU MODEM信息
  算法实现    ：
  引用全局变量：
  输入参数说明：

  返回值说明  ：无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  04/04/06    1.0         zhangsh         创建
====================================================================*/
void CMcuVcData::DisplayModemInfo()
{
    OspPrintf( TRUE, FALSE, "ConfIdx(Snd)\tConfIdx(Rcv)\tRcvType\tRcvMt/EqpId\tSndType\tSndMt/EqpId\tSndFrq\t\tRcvFrq\t\tSendBR\t\tRcvBR\t\tSPort\n" );
   
    for ( u8 byLoop = 0; byLoop < MAXNUM_MCU_MODEM ; byLoop ++ )
    {
        OspPrintf( TRUE, FALSE,"%u \t\t%u \t\t%u \t%u \t%9u \t%9u \t%9u \t%9u\n",
                                m_atModemData[byLoop].wConfIdxSnd,
                                m_atModemData[byLoop].wConfIdxRcv,
								m_atModemData[byLoop].tMtRcv.GetType(),
                                m_atModemData[byLoop].tMtRcv.GetMtId(),
								m_atModemData[byLoop].tMtSnd.GetType(),
								m_atModemData[byLoop].tMtSnd.GetMtId(),
                                m_atModemData[byLoop].dwSendFrequece,
                                m_atModemData[byLoop].dwRevFrequece,
                                m_atModemData[byLoop].dwSendBitRate,
                                m_atModemData[byLoop].dwRevBitRate,
                                m_atModemData[byLoop].bySPortNum ); 
    }
    return;
}

void CMcuVcData::ShowConfFreqInfo()
{
	for (u8 byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx ++)
	{
		if (m_atConfFreq[byIdx].byConfIdx != 0)
		{
			m_atConfFreq[byIdx].Print();
		}
	}
}

void CMcuVcData::ShowSatInfo()
{
	m_tSatInfo.Print();
}


/*====================================================================
  函数名      SetMtModemConnected
  功能       设置MODEM连接状态
  算法实现    ：
  引用全局变量：
  输入参数说明：

  返回值说明  ：无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  04/04/06    1.0         zhangsh         创建
====================================================================*/
void CMcuVcData::SetMtModemConnected( u16 wMtId, BOOL bConnected)
{
    if( wMtId == 0 || wMtId > MAXNUM_CONF_MT )
    {
        return;
    }	
    m_abyMtModemConnected[wMtId-1] = bConnected;
    return;
}

/*====================================================================
  函数名      IsMtModemConnected
  功能       MODEM上线
  算法实现    ：
  引用全局变量：
  输入参数说明：

  返回值说明  ：无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  04/04/06    1.0         zhangsh         创建
====================================================================*/
BOOL CMcuVcData::IsMtModemConnected( u16 wMtId )
{
    if( wMtId == 0 || wMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }	
    return m_abyMtModemConnected[wMtId-1];
    
}

/*====================================================================
  函数名      SetMtModemStatus
  功能       设置Modem状态
  算法实现    ：
  引用全局变量：
  输入参数说明：

  返回值说明  ：无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  04/04/06    1.0         zhangsh         创建
====================================================================*/
void CMcuVcData::SetMtModemStatus( u16 wMtId, BOOL bError )
{
    if( wMtId == 0 || wMtId > MAXNUM_CONF_MT )
    {
        return;
    }	
    m_abyMtModemStatus[wMtId-1] = bError;
    return;
}

u32 CMcuVcData::GetMcuMulticastDataIpAddr( void )
{
	return m_tSatInfo.GetMcuMulticastDataIpAddr();
}

u16 CMcuVcData::GetMcuMulticastDataPort(void)
{
	return m_tSatInfo.GetMcuMulticastDataPort();
}

u16 CMcuVcData::GetMcuRcvMtMediaStartPort(void)
{
	return m_tSatInfo.GetMcuRcvMtMediaStartPort();
}

u16 CMcuVcData::GetMtRcvMcuMediaPort(void)
{
	return m_tSatInfo.GetMtRcvMcuMediaPort();
}


u32 CMcuVcData::GetMcuSecMulticastIpAddr( void )
{
	return m_tSatInfo.GetMcuSecMulticastIpAddr();
}


/*====================================================================
  函数名      SetMtOrEqpUseMcuModem
  功能        ：终端使用某个MCU的MODEM进行接收/发送 
  算法实现    ：
  引用全局变量：
  输入参数说明：

  返回值说明  ：无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  04/04/06    1.0         zhangsh         创建
  09/11/18    4.6         zhangbq         增加多发送处理
====================================================================*/
void CMcuVcData::SetMtOrEqpUseMcuModem(TMt tMt, u8 byModem, BOOL bRcv, BOOL bUsed)
{
	if ( byModem == 0 || byModem > MAXNUM_MCU_MODEM )
		return;
	
	if (bRcv)
	{
		if (  bUsed )
		{
			m_atModemData[byModem - 1].tMtRcv = tMt;;
		}
		else
		{
			m_atModemData[byModem - 1].tMtRcv.SetNull();
		}
	}
	else
	{
		if (  bUsed )
		{
			m_atModemData[byModem - 1].tMtSnd = tMt;;
		}
		else
		{
			m_atModemData[byModem - 1].tMtSnd.SetNull();
		}
	}
	return;
}


/*====================================================================
  函数名      GetConfMtUsedMcuModem
  功能       得到一个MCU的MODEM来接收终端
  算法实现    ：
  引用全局变量：
  输入参数说明：

  返回值说明  ：无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  04/04/06    1.0         zhangsh         创建
  09/11/18    4.6         zhangbq         增加多发送处理
====================================================================*/
u8 CMcuVcData::GetConfMtUsedMcuModem(u8 byConfIdx, TMt tMt, BOOL bRcv)
{
	for ( u8 byLoop = 1; byLoop <= MAXNUM_MCU_MODEM ; byLoop ++ )
	{
		if (bRcv)
		{
			if ( m_atModemData[byLoop-1].wConfIdxRcv == byConfIdx &&
				 m_atModemData[byLoop-1].tMtRcv.GetType() == tMt.GetType() &&
				 m_atModemData[byLoop-1].tMtRcv.GetMtId() == tMt.GetMtId() )
			{
				return byLoop;
			}
		}
		else
		{
			if ( m_atModemData[byLoop-1].wConfIdxSnd == byConfIdx &&
				 m_atModemData[byLoop-1].tMtSnd.GetType() == tMt.GetType() &&
				 m_atModemData[byLoop-1].tMtSnd.GetMtId() == tMt.GetMtId() )
			{
				return byLoop;
			}
		}
	}
	return 0;
}

/*====================================================================
  函数名      ReleaseConfUsedMcuModem
  功能       得到一个MCU的MODEM来接收终端
  算法实现    ：
  引用全局变量：
  输入参数说明：

  返回值说明  ：无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  04/04/06    1.0         zhangsh         创建
  09/11/18    4.6         zhangbq         增加多广播处理
====================================================================*/
u8 CMcuVcData::ReleaseConfUsedMcuModem(u8 byConfIdx, TMt tMt, BOOL bRcv)
{
	for ( u8 byLoop = 1; byLoop < MAXNUM_MCU_MODEM+1 ; byLoop ++ )
	{
		if (bRcv)
		{
			if (m_atModemData[byLoop-1].wConfIdxRcv == byConfIdx &&
				m_atModemData[byLoop-1].tMtRcv.GetMtId() == tMt.GetMtId() &&
				m_atModemData[byLoop-1].tMtRcv.GetType() == tMt.GetType() )
			{
				m_atModemData[byLoop-1].tMtRcv.SetNull();
				return byLoop;
			}
		}
		else
		{
			if (m_atModemData[byLoop-1].wConfIdxSnd == byConfIdx &&
				m_atModemData[byLoop-1].tMtSnd.GetMtId() == tMt.GetMtId() &&
				m_atModemData[byLoop-1].tMtSnd.GetType() == tMt.GetType() )
			{
				m_atModemData[byLoop-1].tMtSnd.SetNull();
				return byLoop;
			}
		}
	}
	return 0;
}

/*====================================================================
  函数名      GetConfIdleMcuModem
  功能       得到一个MCU的MODEM来接收/发送码流
  算法实现    ：
  引用全局变量：
  输入参数说明：

  返回值说明  ：无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  04/04/06    1.0         zhangsh         创建
  09/11/18    4.6         zhangbq         增加多广播处理
====================================================================*/
u8 CMcuVcData::GetConfIdleMcuModem(u8 byConfIdx, BOOL bRcv)
{
	for ( u8 byLoop = 1; byLoop <= MAXNUM_MCU_MODEM ; byLoop ++ )
	{
		if (bRcv)
		{
			if (m_atModemData[byLoop-1].wConfIdxRcv == byConfIdx &&
				m_atModemData[byLoop-1].tMtRcv.GetMtId() == 0 )
			{
				return byLoop;
			}
		}
		else
		{
			if (m_atModemData[byLoop-1].wConfIdxSnd == byConfIdx &&
				m_atModemData[byLoop-1].tMtSnd.GetMtId() == 0 )
			{
				return byLoop;
			}
		}
	}
	return 0;
}

void CMcuVcData::SetConfRefreshTimes(u8 byConfIdx, u8 byTimes)
{
	if (byConfIdx == 0 || byConfIdx > MAX_CONFIDX)
	{
		return;
	}
	m_abyConfRefreshTimes[byConfIdx-1] = byTimes;
}

u8 CMcuVcData::GetConfRefreshTimes(u8 byConfIdx)
{
	if (byConfIdx == 0 || byConfIdx > MAX_CONFIDX)
	{
		return 0;
	}
	return m_abyConfRefreshTimes[byConfIdx-1];
}

BOOL CMcuVcData::IsConfRefreshNeeded( void )
{
	for( u8 byConfIdx = 1; byConfIdx <= MAX_CONFIDX; byConfIdx ++)
	{
		if (GetConfRefreshTimes(byConfIdx) > 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//END FILE
