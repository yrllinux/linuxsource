/*****************************************************************************
   模块名      : 高清解码卡
   文件名      : hdu2test.cpp
   相关文件    : hdu2test.h
   文件实现功能: CHdu2TestClient类实现
   作者        : 田志勇
   版本        : V1.0  Copyright(C) 2011-2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2011/11/28  4.7         田志勇       创建
******************************************************************************/
#include "hdu2eventid.h"
#include "hduautotest.h"
#include "hdu2test.h"

CHdu2TestClientApp g_cHdu2TestApp;
/*=============================================================================
  函 数 名： CHdu2TestClient    
  功    能： 构造函数
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
CHdu2TestClient::CHdu2TestClient()
{
    m_dwHduAutoTestDstNode = INVALID_NODE;
}
/*=============================================================================
  函 数 名： ~CHdu2TestClient    
  功    能： 析构函数
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
CHdu2TestClient::~CHdu2TestClient()
{
}
/*=============================================================================
  函 数 名： DaemonInstanceEntry    
  功    能： 管理实例消息主入口
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg, CApp* pcApp
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
void CHdu2TestClient::DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp )
{
	if (pMsg == NULL)
	{
		hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[DaemonInstanceEntry]pMsg == NULL!\n");
		return;
	}
	hdu2log(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[DaemonInstanceEntry]Receive msg.%d<%s>\n", pMsg->event, ::OspEventDesc(pMsg->event) );
	if ( pMsg->event == EV_HDU_STATUSSHOW )
    {
		StatusShow();
		return;
	}
	switch(CurState()) 
	{
		case emIDLE:
			switch(pMsg->event)
			{
				case OSP_POWERON:
					DaemonProcPowerOn();
					break;
				default:
					hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[DaemonInstanceEntry]Rcv Wrong Msg(%d)(%s) IN STATE:emIDLE!\n" ,
						pMsg->event,OspEventDesc(pMsg->event),CurState());
					break;
			}
			break;
		case emINIT:
			switch(pMsg->event)
			{
				case EV_HDU_CONNECT_TIMER:
				case S_C_REG_NACK:
					DaemonProcConnectTimerOut();
					break;
				case S_C_REG_ACK:
					KillTimer(EV_HDU_REGISTER_TIMER);
					NextState((u32)emREADY);
					break;
				case OSP_DISCONNECT:
					DaemonProcOspDisconnect();
					break;
				case EV_HDU_REGISTER_TIMER:
					LoginTestServer();
					break;
				default:
					hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[DaemonInstanceEntry]Rcv Wrong Msg(%d)(%s) IN STATE:emINIT!\n" ,
						pMsg->event,OspEventDesc(pMsg->event),CurState());
					break;
			}
			break;
		case emREADY:
			switch(pMsg->event)
			{
				case ev_HDUAUTOTEST_CMD:
					{
						u8 byChlIndex;
						THDUAutoTestMsg tHduAutoTestMsg = *(THDUAutoTestMsg*)pMsg->content;
						tHduAutoTestMsg.Print();
						if ( 0 != tHduAutoTestMsg.GetAutoTestStart() )
						{
							hdu2log(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[DaemonInstanceEntry]enter start!\n");
							for ( byChlIndex = 0; byChlIndex < MAXNUM_HDU_CHANNEL; byChlIndex ++ )
							{
								OspPost( MAKEIID( AID_HDUAUTO_TEST, byChlIndex + 1 ), EV_C_STARTSWITCH_CMD, pMsg->content, pMsg->length);
							}
						}
						else
						{
							hdu2log(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[DaemonInstanceEntry]enter stop!\n");
							for ( byChlIndex = 0; byChlIndex < MAXNUM_HDU_CHANNEL; byChlIndex ++ )
							{
								OspPost( MAKEIID( AID_HDUAUTO_TEST, byChlIndex + 1 ), EV_C_STOPSWITCH_CMD, pMsg->content, pMsg->length);
							}
						}
					}
					break;
				case ev_HDURESTORE_CMD:
					DaemonRestoreDefault(pMsg);
					break;
				case OSP_DISCONNECT:
					DaemonProcOspDisconnect();
					break;
				default:
					hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[DaemonInstanceEntry]Rcv Wrong Msg(%d)(%s) IN STATE:emREADY!\n" ,
						pMsg->event,OspEventDesc(pMsg->event),CurState());
					break;
			}
			break;
		default:
			hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[DaemonInstanceEntry]WRONG STATE(%d)! (%d)\n" ,CurState(), pcApp!=NULL?pcApp->appId:0);
			break;
	}
}
/*=============================================================================
  函 数 名： InstanceEntry    
  功    能： 普通实例消息主入口
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
void CHdu2TestClient::InstanceEntry( CMessage* const pMsg )
{
	if (pMsg == NULL)
	{
		hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[DaemonInstanceEntry]pMsg == NULL!\n");
		return;
	}
	hdu2log(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[InstanceEntry]Chn(%d) Receive msg.%d<%s>\n",GetInsID() - 1, pMsg->event, ::OspEventDesc(pMsg->event) );
	if ( pMsg->event == EV_HDU_STATUSSHOW )
    {
		StatusShow();
		return;
	}
	switch(CurState()) 
	{
		case emIDLE:
			switch ( pMsg->event )
			{
				case EV_HDUCHNMGRGRP_CREATE:
					ProcHduChnGrpCreate( pMsg );
					break;
				default:
					hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[InstanceEntry]CHN(%d) Rcv Wrong Msg(%d)(%s) IN STATE:emIDLE!\n" ,
						GetInsID() - 1,pMsg->event,OspEventDesc(pMsg->event),CurState());
					break;
			}
			break;
		case emREADY:
			switch ( pMsg->event )
			{
				case EV_C_STARTSWITCH_CMD:
					ProcStartSwitchReq(pMsg);
					break;
				default:
					hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[InstanceEntry]CHN(%d) Rcv Wrong Msg(%d)(%s) IN STATE:emREADY!\n" ,
						GetInsID() - 1,pMsg->event,OspEventDesc(pMsg->event),CurState());
					break;
			}
			break;
		case emRUNNING:
			switch ( pMsg->event )
			{
				case OSP_DISCONNECT:
				case EV_C_STOPSWITCH_CMD:
					ProcStopSwitchReq(pMsg);
					break;
				default:
					hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[InstanceEntry]CHN(%d) Rcv Wrong Msg(%d)(%s) IN STATE:emRUNNING!\n" ,
						GetInsID() - 1,pMsg->event,OspEventDesc(pMsg->event),CurState());
					break;
			}
			break;
		default:
			hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[InstanceEntry]WRONG STATE(%d)!\n" ,CurState());
			break;
	}
}
/*=============================================================================
  函 数 名： DaemonInit    
  功    能： 板初始化
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
void CHdu2TestClient::DaemonProcPowerOn()
{
	OspAddEventDesc("C_S_REG_REQ", C_S_REG_REQ);
	OspAddEventDesc("S_C_REG_ACK", S_C_REG_ACK);
	OspAddEventDesc("S_C_REG_NACK", S_C_REG_NACK);
	OspAddEventDesc("ev_HDUAUTOTEST_CMD", ev_HDUAUTOTEST_CMD);
	OspAddEventDesc("ev_HDURESTORE_CMD", ev_HDURESTORE_CMD);
	OspAddEventDesc("ev_HDURESTORE_NOTIFY", ev_HDURESTORE_NOTIFY);
	s32 nRet = 0;
	nRet = HardMPUInit((u32)en_HDU2_Board);
	if ( CODEC_NO_ERROR != nRet )
	{
        hdu2log(LOG_LVL_ERROR, MID_MCU_EQP,"[DaemonInit]HardMPUInit Failed!error=[%d]\n", nRet);
		return;
	}
	else
	{
        hdu2log(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[DaemonInit]HardMPUInit Successed!\n");
	}
    u16 wRet = KdvSocketStartup();
    if ( MEDIANET_NO_ERROR != wRet)
    {
        hdu2log(LOG_LVL_ERROR, MID_MCU_EQP,"[DaemonInit]KdvSocketStartup Failed!error=[%d]\n", nRet);
		return;
	}
	else
	{
        hdu2log(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[DaemonInit]KdvSocketStartup Successed!\n");
	}
	NextState((u32)emINIT);
	SetTimer( EV_HDU_CONNECT_TIMER, CONNECT_TIMER_LENGTH );
	for (u8 byChlIndex = 0; byChlIndex < MAXNUM_HDU_CHANNEL; byChlIndex ++ )
	{
		wRet = OspPost( MAKEIID(AID_HDUAUTO_TEST, byChlIndex + 1), EV_HDUCHNMGRGRP_CREATE );
		if (OSP_OK == wRet)
		{
			hdu2log(LOG_LVL_DETAIL, MID_MCU_EQP,"[DaemonInit]OspPost EV_HDUCHNMGRGRP_CREATE Sucessful!\n");
		}
		else
		{
            hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[DaemonInit]OspPost EV_HDUCHNMGRGRP_CREATE Failed!\n");
		}
	}
	return;
}
/*=============================================================================
  函 数 名： DaemonProcConnectServerCmd    
  功    能： 定时建链处理
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
void CHdu2TestClient::DaemonProcConnectTimerOut()
{
	KillTimer( EV_HDU_CONNECT_TIMER );
    if( !Connect() )
    {
        u16 wRet = SetTimer( EV_HDU_CONNECT_TIMER, CONNECT_TIMER_LENGTH );
		if (OSP_OK == wRet)
		{
			hdu2log(LOG_LVL_DETAIL, MID_MCU_EQP,"[DaemonProcConnectTimerOut]SetTimer Sucessful!\n");
		}
		else
		{
            hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[DaemonProcConnectTimerOut]SetTimer Failed!\n");
		}
    }
	else
	{
		LoginTestServer();
	}
    return;
}
/*=============================================================================
  函 数 名： DaemonProcOspDisconnect    
  功    能： 断链处理
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
void CHdu2TestClient::DaemonProcOspDisconnect()
{	
	m_dwHduAutoTestDstNode = INVALID_NODE;
	for (u8 byLoop = 1; byLoop <= MAXNUM_HDU_CHANNEL; byLoop++ )
	{
		post(MAKEIID(GetAppID(), byLoop), OSP_DISCONNECT);
	}
	NextState((u32)emINIT);
	u16 wRet = SetTimer( EV_HDU_CONNECT_TIMER, CONNECT_TIMER_LENGTH );
	if (OSP_OK == wRet)
	{
		hdu2log(LOG_LVL_DETAIL, MID_MCU_EQP,"[DaemonProcOspDisconnect]SetTimer Sucessful!\n");
	}
	else
	{
		hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[DaemonProcOspDisconnect]SetTimer Failed!\n");
	}
	hdu2log(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[DaemonProcOspDisconnect]DAEMON Change Status READY-->INIT!\n");
}
/*=============================================================================
  函 数 名： DaemonRestoreDefault
  功    能： 恢复默认值
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
void CHdu2TestClient::DaemonRestoreDefault( CMessage* const pMsg )
{
#ifndef WIN32
	BOOL32 bRet = FALSE;
    bRet = BrdClearE2promTestFlag();	//清测试标志位.返回0表成功
	u8 bySucessRestore = bRet ? 0 : 1;
	s32 sdwRet;
    sdwRet = post(pMsg->srcid, ev_HDURESTORE_NOTIFY, &bySucessRestore, sizeof(u8), pMsg->srcnode);
    if ( sdwRet != OSP_OK )
    {
        hdu2log(LOG_LVL_ERROR, MID_MCU_EQP,"[DaemonRestoreDefault]post ev_HDURESTORE_NOTIFY Failed!\n" );
    }
	else
	{
		hdu2log(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[DaemonRestoreDefault]post ev_HDURESTORE_NOTIFY sucessful!\n" );
	}
	OspDelay(2000);
	BrdHwReset();
#endif
	hdu2log(LOG_LVL_ERROR, MID_MCU_EQP,"[DaemonRestoreDefault]MsgNode (%d)!\n",pMsg != NULL ? pMsg->srcnode : 0);
}
/*=============================================================================
  函 数 名： ProcStartSwitchReq
  功    能： 开始交换
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
void CHdu2TestClient::ProcStartSwitchReq( CMessage* const pMsg )
{
    THDUAutoTestMsg tHduAutoMsg = *(THDUAutoTestMsg*)pMsg->content;
    u32 dwHduTestItem = (u32)tHduAutoMsg.GetAutoTestType();
	TVidSrcInfo tVidSrcInfo;
	memset( &tVidSrcInfo, 0x0, sizeof(tVidSrcInfo) );
	u16 wChnIdx = GetInsID() - 1;
	TDoublePayload tVidPayload, tAudPayload;
	if (g_cBrdAgentApp.GetBrdPosition().byBrdID == BRD_TYPE_HDU2)
	{
		switch ( dwHduTestItem )
		{
		case 	emDviToHdmi:
			tVidSrcInfo.m_wWidth = 1280;
			tVidSrcInfo.m_wHeight = 720;
			tVidSrcInfo.m_bProgressive = 1;
			tVidSrcInfo.m_dwFrameRate = 60;
			m_tHduModePort.SetOutModeType(HDU_YPbPr_720P_60fps);
			m_tHduModePort.SetOutPortType(HDU_OUTPUT_HDMI);
			break;
			
		case	emDviToVga:
			tVidSrcInfo.m_wWidth = 800;
			tVidSrcInfo.m_wHeight = 600;
			tVidSrcInfo.m_bProgressive = 1;
			tVidSrcInfo.m_dwFrameRate = 60;
			m_tHduModePort.SetOutModeType(HDU_VGA_SVGA_60HZ);
			m_tHduModePort.SetOutPortType(HDU_OUTPUT_VGA);
			break;
		default:
			hdu2log(LOG_LVL_WARNING, MID_MCU_EQP, "[ProcStartSwitchReq]Wrong HduTestItem(%d) is wrong!\n",dwHduTestItem);
			return;
		}
	}
	else if (g_cBrdAgentApp.GetBrdPosition().byBrdID == BRD_TYPE_HDU2_S)
	{
		switch ( dwHduTestItem )
		{
		case 	emYPbPr1080P60fps:
			tVidSrcInfo.m_wWidth = 1920;
			tVidSrcInfo.m_wHeight = 1080;
			tVidSrcInfo.m_bProgressive = 1;
			tVidSrcInfo.m_dwFrameRate = 60;
			m_tHduModePort.SetOutModeType(HDU_YPbPr_1080P_60fps);
			m_tHduModePort.SetOutPortType(HDU_OUTPUT_SDI);
			break;
			
		case	emYPbPr576i50Hz:
			tVidSrcInfo.m_wWidth = 720;
			tVidSrcInfo.m_wHeight = 576;
			tVidSrcInfo.m_bProgressive = 0;
			tVidSrcInfo.m_dwFrameRate = 25;
			m_tHduModePort.SetOutModeType(HDU_C_576i_50HZ);
			m_tHduModePort.SetOutPortType(HDU_OUTPUT_C);
			break;
		default:
			hdu2log(LOG_LVL_WARNING, MID_MCU_EQP, "[ProcStartSwitchReq]Wrong HduTestItem(%d) is wrong!\n",dwHduTestItem);
			return;
		}
	}
	else if(g_cBrdAgentApp.GetBrdPosition().byBrdID == BRD_TYPE_HDU2_L)
	{
		tVidSrcInfo.m_wWidth = 720;
		tVidSrcInfo.m_wHeight = 576;
		tVidSrcInfo.m_bProgressive = 0;
		tVidSrcInfo.m_dwFrameRate = 25;
		m_tHduModePort.SetOutModeType(HDU_C_576i_50HZ);
		m_tHduModePort.SetOutPortType(HDU_OUTPUT_C);
		hdu2log(LOG_LVL_WARNING, MID_MCU_EQP, "[ProcStartSwitchReq]hdu2_l  start producttest!\n");
	}
	else
	{
		hdu2log(LOG_LVL_WARNING, MID_MCU_EQP, "[ProcStartSwitchReq]Unknown BrdType(%d)!\n",g_cBrdAgentApp.GetBrdPosition().byBrdID);
	}

	if (!m_cHdu2ChnMgrGrp.SetVidPlyPortType(GetVidPlyPortTypeByChnAndType(wChnIdx,m_tHduModePort.GetOutPortType())))
	{
		hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[ProcStartSwitchReq]SetVidPlyPortType Failed!\n");
	}
	u16 wRecvPort = PRODUCETEST_RECVBASEPORT + wChnIdx * 4;
	u32 dwIpAddr = inet_addr(Host_IpAddr);
	if (!m_cHdu2ChnMgrGrp.SetVideoPlyInfo(&tVidSrcInfo))
	{
		hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[ProcStartSwitchReq]SetVideoPlyInfo Failed!\n");
	}
	m_tHduModePort.SetZoomRate(1);
	if (!m_cHdu2ChnMgrGrp.SetPlayScales(16,9))
	{
		hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[ProcStartSwitchReq]SetPlayScales Failed!\n");
	}
	m_tHduModePort.SetScalingMode(1);
	if (!m_cHdu2ChnMgrGrp.SetVidDecResizeMode(m_tHduModePort.GetScalingMode()))
	{
		hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[ProcStartSwitchReq]SetScalingMode Failed!\n");
	}
	if (!m_cHdu2ChnMgrGrp.SetVidDecResizeMode(m_tHduModePort.GetScalingMode()))
	{
		hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[ProcStartSwitchReq]Chn(%d) SetVidDecResizeMode() Failed!\n",wChnIdx);
	}
	if (!m_cHdu2ChnMgrGrp.SetVolume( HDU_VOLUME_DEFAULT ))
	{
		hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[ProcStartSwitchReq]Chn(%d) SetVolume() Failed!\n",wChnIdx);
	}
	TLocalNetParam tLocalNetParm;
	memset(&tLocalNetParm, 0, sizeof(TLocalNetParam));
	tLocalNetParm.m_tLocalNet.m_wRTPPort  = wRecvPort;
	tLocalNetParm.m_tLocalNet.m_wRTCPPort = wRecvPort + 1;
	tLocalNetParm.m_dwRtcpBackAddr        = dwIpAddr;
	tLocalNetParm.m_wRtcpBackPort         = wRecvPort + 1;
	if (m_cHdu2ChnMgrGrp.SetVidLocalNetParam(&tLocalNetParm))
	{
		hdu2log(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[ProcStartSwitchReq]Chn(%d) SetVidLocalNetParam() LocalRcvPort(%d) Successed!\n",wChnIdx,tLocalNetParm.m_tLocalNet.m_wRTPPort);
	} 
	else
	{
		hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[ProcStartSwitchReq]Chn(%d) SetVidLocalNetParam() LocalRcvPort(%d) Failed!\n",wChnIdx,tLocalNetParm.m_tLocalNet.m_wRTPPort);
	}
    memset(&tLocalNetParm, 0, sizeof(TLocalNetParam));
    tLocalNetParm.m_tLocalNet.m_wRTPPort  = wRecvPort + 2;
    tLocalNetParm.m_tLocalNet.m_wRTCPPort = wRecvPort + 3;
    tLocalNetParm.m_dwRtcpBackAddr        = dwIpAddr;
    tLocalNetParm.m_wRtcpBackPort         = wRecvPort + 3;
	if (m_cHdu2ChnMgrGrp.SetAudLocalNetParam(&tLocalNetParm))
	{
		hdu2log(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[ProcStartSwitchReq]Chn(%d) SetAudLocalNetParam() LocalRcvPort(%d) Successed!\n",wChnIdx,tLocalNetParm.m_tLocalNet.m_wRTPPort);
	} 
	else
	{
		hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[ProcStartSwitchReq]Chn(%d) SetAudLocalNetParam() LocalRcvPort(%d) Failed!\n",wChnIdx,tLocalNetParm.m_tLocalNet.m_wRTPPort);
	}
	if(!m_cHdu2ChnMgrGrp.StartNetRecv(MODE_BOTH, 0/*chenbing*/))
    {
        hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[ProcStartSwitchReq]StartNetRecv Faileded!\n");
    }
	tVidPayload.SetActivePayload( tHduAutoMsg.GetVideoType() );
	tVidPayload.SetRealPayLoad( tHduAutoMsg.GetVideoType() );
	tAudPayload.SetActivePayload( tHduAutoMsg.GetAudioType() );
	tAudPayload.SetRealPayLoad( tHduAutoMsg.GetAudioType() );
	m_cHdu2ChnMgrGrp.SetVidDoublePayload(tVidPayload, 0/*chenbing*/);
	m_cHdu2ChnMgrGrp.SetAudDoublePayload(tAudPayload);
	if (!m_cHdu2ChnMgrGrp.SetActivePT(MODE_BOTH, 0/*chenbing*/))
	{
		hdu2log(LOG_LVL_WARNING, MID_MCU_EQP,"[ProcStartSwitchReq]SetActivePT Faileded!\n");
	}
	
	if(!m_cHdu2ChnMgrGrp.StartDecode(MODE_BOTH))
	{
		hdu2log(LOG_LVL_ERROR, MID_MCU_EQP,"[ProcStartSwitchReq] Chn.%d Play failed as mode.%d \n",
			GetInsID()-1, MODE_BOTH);
		return;
	}
	NextState((u32)emRUNNING);
}
/*=============================================================================
  函 数 名： ProcStopSwitchReq
  功    能： 停止交换
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
void CHdu2TestClient::ProcStopSwitchReq( CMessage* const pMsg )
{
	if(!m_cHdu2ChnMgrGrp.StopDecode(m_cHdu2ChnMgrGrp.GetMode()))
	{
		hdu2log(LOG_LVL_ERROR, MID_MCU_EQP,"[ProcStopSwitchReq] Chn.%d Play Failed as mode.%d \n",
			GetInsID()-1, MODE_BOTH);
	}
	NextState((u32)emREADY);
	hdu2log(LOG_LVL_ERROR, MID_MCU_EQP,"[DaemonRestoreDefault]MsgNode (%d)!\n",pMsg != NULL ? pMsg->srcnode : 0);
	return;
}
/*=============================================================================
  函 数 名： ProcHduChnGrpCreate
  功    能： 初始化通道
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
void CHdu2TestClient::ProcHduChnGrpCreate( CMessage* const pMsg )
{
	if ( !m_cHdu2ChnMgrGrp.Create(GetInsID() - 1,g_cBrdAgentApp.GetBrdPosition().byBrdID == BRD_TYPE_HDU2_S) )
	{
		hdu2log(LOG_LVL_ERROR, MID_MCU_EQP,"[ProcHduChnGrpCreate]m_cHdu2ChnMgrGrp.Create failed!\n");
	}
	else
	{
		NextState((u32)emREADY);
		hdu2log(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[ProcHduChnGrpCreate]Chn(%d) Change Status INIT-->READY!\n",GetInsID() - 1);
	}
	hdu2log(LOG_LVL_ERROR, MID_MCU_EQP,"[DaemonRestoreDefault]MsgNode (%d)!\n",pMsg != NULL ? pMsg->srcnode : 0);
}
/*=============================================================================
  函 数 名： Connect
  功    能： 连接生产测试服务器
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
BOOL CHdu2TestClient::Connect()
{
    u32 dwServerIpAddr = inet_addr(Host_IpAddr);
    m_dwHduAutoTestDstNode = OspConnectTcpNode(dwServerIpAddr, 60000, 10, 3);
    if(INVALID_NODE == m_dwHduAutoTestDstNode)
	{		
		hdu2log(LOG_LVL_ERROR, MID_MCU_EQP,"[ConnectAndLoginTestServer]OspConnectTcpNode Failed!ProductTestIp:[%s]\n",Host_IpAddr);

        return FALSE;
    }
	else
	{
		hdu2log(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[ConnectAndLoginTestServer]OspConnectTcpNode Successed!\n");
	}
    u16 wRet = OspNodeDiscCBReg( m_dwHduAutoTestDstNode, AID_HDUAUTO_TEST, CHdu2TestClient::DAEMON );
	if (OSP_OK != wRet)
    {
        hdu2log(LOG_LVL_ERROR, MID_MCU_EQP,"[ConnectAndLoginTestServer]OspNodeDiscCBReg Failed!\n");
		return FALSE;
    }
	else
	{
		hdu2log(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[ConnectAndLoginTestServer]OspNodeDiscCBReg Successed!\n");
	}

    return TRUE;
}
/*=============================================================================
  函 数 名： LoginTestServer
  功    能： 连接生产测试服务器
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2013/02/20  4.7         田志勇       创建
=============================================================================*/
void CHdu2TestClient::LoginTestServer()
{
	//发送登陆消息: ev_MTAutoTestLoginReq 消息体：版本号 + 用户信息
	u8 abyTemp[sizeof(u8)*32 + sizeof(CLoginRequest) + sizeof(CDeviceInfo)];
	memset(abyTemp, 0, sizeof(abyTemp));
	abyTemp[0] = 15;//EQP_TYPE_HDU2原定义
	CLoginRequest cLoginReq;
	cLoginReq.Empty();
	cLoginReq.SetName("admin");
	cLoginReq.SetPassword("admin");
	memcpy(abyTemp + sizeof(u8)*32, &cLoginReq, sizeof(CLoginRequest));

	CDeviceInfo cDeviceinfo;
	memset( &cDeviceinfo, 0x0, sizeof( cDeviceinfo ) );
	SetHduDeviceInfo(cDeviceinfo);
	memcpy(abyTemp + sizeof(u8)*32 + sizeof(CLoginRequest), &cDeviceinfo, sizeof(CDeviceInfo));
	u16 wRet = post( MAKEIID( HDU_SERVER_APPID, 1 ), C_S_REG_REQ, abyTemp, sizeof(u8) * 32  + sizeof(CLoginRequest) + sizeof(CDeviceInfo),
		 m_dwHduAutoTestDstNode);
	if (OSP_OK != wRet)
	{
		hdu2log(LOG_LVL_ERROR, MID_MCU_EQP,"[ConnectAndLoginTestServer]post C_S_REG_REQ Failed!\n");
	}
	else
	{
		hdu2log(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[ConnectAndLoginTestServer]post C_S_REG_REQ Sucessed!\n");
	}
	SetTimer(EV_HDU_REGISTER_TIMER, HDU_REGISTER_TIMEOUT);
}
/*=============================================================================
  函 数 名： SetHduDeviceInfo
  功    能： 设置设备信息
  算法实现： 
  全局变量： 
  参    数： CDeviceInfo &cDeviceInfo
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
void CHdu2TestClient::SetHduDeviceInfo(CDeviceInfo &cDeviceInfo)
{
    s8* pSoftVer = (s8*)VER_HDU2;
    TBrdEthParam tBrdEthParam;
	memset( &tBrdEthParam, 0x0, sizeof(tBrdEthParam) );
    BrdGetEthParam( 0, &tBrdEthParam );
    
	u32 dwIp = tBrdEthParam.dwIpAdrs;
    u32 dwMask = tBrdEthParam.dwIpMask; 

    cDeviceInfo.setIp( dwIp );
    cDeviceInfo.setSubMask( dwMask );    
    cDeviceInfo.setSoftVersion( pSoftVer );

#ifndef WIN32
    s8 achSerial[12] = {0};
	TBrdE2PromInfo tBrdE2PromInfo;
	s8 achHWversion[VERSION_LEN];
	s8 achMac[MAX_MAC_LENGTH];
	memset( achMac, 0x0, sizeof(achMac) );
	memset( achHWversion, 0x0, sizeof(achHWversion) );
	memset( &tBrdE2PromInfo, 0x0, sizeof(tBrdE2PromInfo) );
    BrdGetE2PromInfo( &tBrdE2PromInfo );

	sprintf(achMac,"%02X-%02X-%02X-%02X-%02X-%02X",tBrdEthParam.byMacAdrs[0],tBrdEthParam.byMacAdrs[1],
		tBrdEthParam.byMacAdrs[2],tBrdEthParam.byMacAdrs[3],tBrdEthParam.byMacAdrs[4],tBrdEthParam.byMacAdrs[5]);  
    sprintf( achHWversion, "%d", tBrdE2PromInfo.dwHardwareVersion );
    memcpy( achSerial, tBrdE2PromInfo.chDeviceSerial, sizeof(tBrdE2PromInfo.chDeviceSerial) );
	cDeviceInfo.setHardVersion( achHWversion );
    cDeviceInfo.setMac( achMac );
    cDeviceInfo.setSerial( achSerial ); 
#endif
}
/*====================================================================
函数  : StatusShow
功能  : 状态显示，显示复合解码器状态
输入  : void
输出  : 无
返回  : void
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
====================================================================*/
void   CHdu2TestClient::StatusShow(void)                                    
{
	static char status[4][10] = {"IDLE", "INIT","READY","RUNNING"};
	u16 wChnId = GetInsID();
	if (wChnId == CInstance::DAEMON)
	{
		hdu2log(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,"DAEMONINST Status:[%s] TestServerNode:[%d]\n",status[CurState()],m_dwHduAutoTestDstNode);
	}
	else if ( wChnId <= MAXNUM_HDU_CHANNEL )
    {
		hdu2log(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,"\n**********CHN(%d) INFO Status:[%s]**********\n",wChnId,status[CurState()]);
		hdu2log(LOG_LVL_ERROR, MID_PUB_ALWAYS,"Status:%s Param(OutType:%d(%s),OutMode:%d(%s),ZoomRate:%d(%s),ScalingMode:%d(%s))\n",
			status[CurState()],m_tHduModePort.GetOutPortType(),m_tHduModePort.GetOutTypeStr(),m_tHduModePort.GetOutModeType(),m_tHduModePort.GetOutModeStr(),
			m_tHduModePort.GetZoomRate(),m_tHduModePort.GetZoomRateStr(),m_tHduModePort.GetScalingMode(),m_tHduModePort.GetScalingModeStr());
        m_cHdu2ChnMgrGrp.ShowInfo(wChnId - 1);
    }
	else
	{
		hdu2log(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS,"------------------INVALID CHN(%d)----------------\n",wChnId);
	}
	return;
}
//END OF FILE

