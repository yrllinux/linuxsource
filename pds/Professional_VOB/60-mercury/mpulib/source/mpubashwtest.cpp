

#include "mpuautotest.h"
#include "mpuvmphwtest.h"
#include "mpuvmphwtest_evid.h"
#include "mcuver.h"
#include "boardagentbasic.h"

// #ifdef _LINUX_
// #include "nipwrapper.h"
// #include "nipwrapperdef.h"
// #include "brdwrapperdef.h"
// #endif

// #ifdef _LINUX12_
// #include "brdwrapper.h"
// #endif

// #define  SERIAL_FILE_LEN                100
// #define  CONNECT_TIMER_LENGTH           (10*1000)
// #define  VERSION_LEN                    32

CMpuBasTestClientApp g_cMpuBasTestApp;


CMpuBasTestClient::CMpuBasTestClient()
{
//     m_dwMpuTestDstNode = INVALID_NODE;
//     m_dwMpuTestDstInst = INVALID_INS;
}

CMpuBasTestClient::~CMpuBasTestClient()
{
	
}

void CMpuBasTestClient::DaemonInit(CMessage* const pcMsg)
{
	if ( pcMsg->length == sizeof(TMpuCfg) )
    {
        memcpy( &g_cMpuBasTestApp.m_tCfg, pcMsg->content, sizeof(TEqpCfg) );
    }
    g_cMpuBasTestApp.m_byWorkMode = TYPE_MPUBAS;
	
    // 码率作弊
    //sprintf( MPULIB_CFG_FILE, "%s/mcueqp.ini", DIR_CONFIG );
    //g_cMpuBasApp.ReadDebugValues();    
	
    u16 wRet = KdvSocketStartup();
    
    if ( MEDIANET_NO_ERROR != wRet )
    {
        mpulog( MPU_CRIT, "KdvSocketStartup failed, error: %d\n", wRet );
        printf( "KdvSocketStartup failed, error: %d\n", wRet );
        return ;
    }
	
#ifdef _LINUX_
    wRet = HardMPUInit( (u32)INITMODE_HDBAS );
    if ( (u16)Codec_Success != wRet )
    {
        mpulog( MPU_CRIT, "HardMPUInit failed, error: %d\n", wRet );
        printf( "HardMPUInit failed, error: %d\n", wRet );
        return ;
    }
#endif	  
    
	// 建链
	DaemonProcConnectTimeOut();
}

BOOL32 CMpuBasTestClient::ConnectServ()
{
	printf("Begin to connect Test Server!\n");

/*	BOOL bRet = TRUE;
	s8    achProfileName[64] = {0};
    memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	
	s8 achServeIpAddr[16] = {0};
	bRet = GetRegKeyString( achProfileName, SECTION_BoardConfig, KEY_ServerIpAddr, "0.0.0.0", achServeIpAddr, sizeof(achServeIpAddr) );
*/

	u32 dwServerIpAddr = inet_addr(achServIpAddr);
	g_cMpuBasTestApp.m_dwMcuNode = (u32)OspConnectTcpNode(dwServerIpAddr, 60000);
	
	if(INVALID_NODE == g_cMpuBasTestApp.m_dwMcuNode)
	{
		printf("[ConnectServ] connect to Server %s failed!\n",achServIpAddr);
		mpulog(MPU_CRIT, "[ConnectServ] connect to Server %s failed!\n",achServIpAddr);
		
		return FALSE;
	}

	g_cMpuBasTestApp.m_dwMcuRcvIp = dwServerIpAddr;					//对端接收码流的Ip
	g_cMpuBasTestApp.m_wMcuRcvStartPort = MPUTEST_BAS_SENDSVRPORT;	//对端接收码流的Port
	return TRUE;
}

void CMpuBasTestClient::DaemonProcConnectTimeOut(void)
{
	if(!ConnectServ())
	{
		SetTimer( EV_C_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );
	}
	else
	{
		::OspNodeDiscCBRegQ( g_cMpuBasTestApp.m_dwMcuNode, GetAppID(), GetInsID() );
		
		SetTimer( EV_C_REGISTER_TIMER, MPU_REGISTER_TIMEOUT);
	}
}

void CMpuBasTestClient::DaemonRegisterSrv(void)
{
	//发送登陆消息: ev_MPUTest_REQ 消息体：
	//	  u8 测试的模式（VMP/BAS）  
	//	+ u8 *31  空
	//	+ 用户信息 + 设备信息
    u8 abyTemp[sizeof(u8)*32 + sizeof(CLoginRequest) + sizeof(CDeviceInfo)];
    memset(abyTemp, 0, sizeof(abyTemp));
    
	abyTemp[0] = TYPE_MPUBAS;
    
    CLoginRequest cLoginReq;
    cLoginReq.Empty();
    cLoginReq.SetName("admin");
    cLoginReq.SetPassword("admin");
    memcpy(abyTemp + sizeof(u8)*32, &cLoginReq, sizeof(CLoginRequest));
    
    CDeviceInfo cDeviceinfo;
	memset( &cDeviceinfo, 0x0, sizeof( cDeviceinfo ) );
#ifdef _LINUX_
    SetMpuDeviceInfo(cDeviceinfo);
#endif
    memcpy(abyTemp + sizeof(u8)*32 + sizeof(CLoginRequest), &cDeviceinfo, sizeof(CDeviceInfo));
    post( MAKEIID( MPU_SERVER_APPID, 1 ), 
		C_S_MPULOGIN_REQ, 
		abyTemp, 
        sizeof(u8) * 32  + sizeof(CLoginRequest) + sizeof(CDeviceInfo),
        g_cMpuBasTestApp.m_dwMcuNode
		);
}

void CMpuBasTestClient::DaemonProcRegAck(CMessage * const pcMsg )
{
	OspPrintf(TRUE, FALSE, "[DaemonProcRegAck]Receive McuTest_Ack!\n");
	printf("[DaemonProcRegAck]Receive McuTest_Ack!\n");
	KillTimer(MPU_REGISTER_TIMEOUT);
	g_cMpuBasTestApp.m_dwMcuIId = pcMsg->srcid;

	u8 byLoop;
	for ( byLoop = 1; byLoop <= MAXNUM_MPUBAS_CHANNEL; byLoop++ )
	{
		post( MAKEIID(GetAppID(), byLoop), EV_C_BASCHL_INIT );
    }
}

/*====================================================================
	函数  : DaemonProcOspDisconnect
	功能  : Osp断链消息处理函数
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2009/7/16   4.6         xl				创建
====================================================================*/
void CMpuBasTestClient::DaemonProcOspDisconnect( CMessage* const pMsg )
{
    if ( NULL == pMsg )  
    {
        mpulog( MPU_CRIT, "[DaemonProcOspDisconnect] message's pointer is Null\n" );
        return;
    }

    u32 dwNode = *(u32*)pMsg->content;
	
	if( dwNode != g_cMpuBasTestApp.m_dwMcuNode)
	{
		printf( "[DaemonProcOspDisconnect] the Node.%d isn't equal to m_dwMpuTestDstNode.\n",dwNode );
		return;
	}
	
    if(INVALID_NODE == dwNode)
	{
		OspPrintf( TRUE, FALSE, "[DaemonProcOspDisconnect]: m_dwHduAutoTestDstNode is invalid!\n" );
        return;   
	}
	
    OspDisconnectTcpNode(dwNode);
	
	g_cMpuBasTestApp.FreeStatusDataA(); //把节点清空等
	
// 	u8 byLoop = 1;
// 	for ( ; byLoop <= MAXNUM_MPUBAS_CHANNEL; byLoop++ )
//     {
//         post( MAKEIID(GetAppID(), byLoop), EV_BAS_QUIT );
//     }
	
	//重连
	SetTimer( EV_C_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );

	return;
}

/*lint -save -e715*/
void CMpuBasTestClient::DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp )
{
	switch (pMsg->event)
	{
	case EV_C_INIT:
		DaemonInit(pMsg);
		break;

	case EV_C_CONNECT_TIMER:
		DaemonProcConnectTimeOut();
		break;
	
	case EV_C_REGISTER_TIMER:
		DaemonRegisterSrv();
		break;

	case S_C_MPULOGIN_ACK:
		DaemonProcRegAck(pMsg);
		break;

	case S_C_MPULOGIN_NACK:
		break;

	case OSP_DISCONNECT:
        DaemonProcOspDisconnect( pMsg );
        break;

	case EV_C_CHANGEAUTOTEST_CMD:			//API 开启/取消测试模式
#ifdef _LINUX_
		DaemonProcChangeAutoTestCmd( pMsg );
#endif
		break;
	
	case S_C_CHANGETESTMODE_CMD:			//改变测试模式
		{
			CServMsg cServMsg( pMsg->content, pMsg->length );
			u8 byMode = *cServMsg.GetMsgBody();
			ChangeTestMode(byMode);
			break;
		}
	case S_C_MPURESTORE_CMD:			    //恢复测试前默认值
		{
			RestoreDefault();
			break;
		}
	case S_C_STARTBASTEST_REQ:
		{
			printf("receive S_C_STARTBASTEST_REQ !\n");
			mpulog( MPU_CRIT, "receive S_C_STARTBASTEST_REQ !\n");
            
			//CServMsg cServMsg( pMsg->content, pMsg->length );
            u8 byChnId = 1;
			for (; byChnId <= MAXNUM_MPUBAS_CHANNEL; byChnId ++)
			{
				printf("post S_C_STARTBASTEST_REQ to InstId : %d\n", byChnId);
				mpulog( MPU_CRIT, "post S_C_STARTBASTEST_REQ to InstId : %d\n", byChnId);
				post( MAKEIID( GetAppID(), byChnId ), pMsg->event, pMsg->content, pMsg->length );
			}
            break;        
        }
	case S_C_FASTUPDATEPIC_CMD:
		{
			CServMsg cServMsg( pMsg->content, pMsg->length );
			u8 byChnIdx = cServMsg.GetChnIndex();
			u8 byChnId = byChnIdx + 1;
			post( MAKEIID( GetAppID(), byChnId ), pMsg->event, pMsg->content, pMsg->length );
            break;
		}
	default:
		break;
	}
}
/*lint -restore */
/*====================================================================
	函数  : InstanceEntry
	功能  : 普通实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
    日  期      版本        修改人      修改内容
====================================================================*/
void CMpuBasTestClient::InstanceEntry( CMessage* const pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    cServMsg.SetEventId(pcMsg->event);
    
    switch ( pcMsg->event )    
    {
    // 初始化通道
    case EV_C_BASCHL_INIT:
        {
			ProcInitBas();
			break;
		}
	case S_C_STARTBASTEST_REQ:
		{
			ProcStartAdpReq();
			break;
		}
	case S_C_FASTUPDATEPIC_CMD:
		{
			ProcFastUpdatePicCmd(/*cServMsg*/);
			break;
		}
	default:
		{
			break;
		}
		
	}
}

void CMpuBasTestClient::ProcInitBas()
{
	OspPrintf(TRUE, FALSE, "[ProcInitBas]\n");
	BOOL bRet = TRUE;
    if ( m_pAdpCroup == NULL )
    {
        // start new       
        m_pAdpCroup = new CMpuBasAdpGroup;
		
		// port mpu receive stream
        u16 wRecvPort = MPUTEST_BAS_RECVBASEPORT + ( GetInsID() - 1 ) * PORTSPAN * 2;
		
		// port mpu send stream to
        u16 wDstPort = g_cMpuBasTestApp.m_wMcuRcvStartPort + ( GetInsID() - 1 ) * PORTSPAN * 2;
		
		bRet = m_pAdpCroup->Create( /*MAX_VIDEO_FRAME_SIZE, */
									wRecvPort + 1,
									wRecvPort,
									g_cMpuBasTestApp.m_dwMcuRcvIp,
									wDstPort,
								    (u32)this,
                                    GetInsID()-1 );	
		if( !bRet )
		{
			OspPrintf(TRUE, FALSE, "ProcInitBas: Create failed!\n");
		}
//     if (bRet)
//     {
//         NextState( READY );
//         SetTimer( EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL );   // 定时查询适配组是否需要关键帧
//     }

	}
	NextState((u32)READY);
	CServMsg cServMsg;
	cServMsg.SetChnIndex( GetInsID() -1 );
	post( MAKEIID( MPU_SERVER_APPID, 1 ), 
		C_S_BASCHNL_NOTIF, 
		cServMsg.GetServMsg(), cServMsg.GetServMsgLen(),
		g_cMpuBasTestApp.m_dwMcuNode
		);
}


void CMpuBasTestClient::ProcStartAdpReq( void)
{
	u8 byChnIdx = (u8)GetInsID() - 1;

    mpulog( MPU_INFO, "[ProcStartAdptReq] Start adp, The channel is %d\n", byChnIdx );

	if( NULL == m_pAdpCroup )
	{
		mpulog( MPU_CRIT, "[ProcStartAdptReq] Channel.%d create failed !\n", GetInsID() );	
		return;
	}

    TMediaEncrypt  tMediaEncryptVideo;
	TDoublePayload tDoublePayloadVideo;
	
	tDoublePayloadVideo.SetRealPayLoad( MEDIA_TYPE_H264 );
	tDoublePayloadVideo.SetActivePayload( MEDIA_TYPE_H264);

//     THDAdaptParam  atParm[2];
// 	atParm[0]  = *(THDAdaptParam*)cServMsg.GetMsgBody();
// 	atParm[1] = *(THDAdaptParam*)( cServMsg.GetMsgBody() + sizeof(THDAdaptParam) );
    
//     if ( g_cMpuBasApp.IsEnableCheat() )
//     {
//         u16 wBitrate = atParm[0].GetBitrate();
//         wBitrate = g_cMpuBasApp.GetDecRateDebug( wBitrate );
//         atParm[0].SetBitRate( wBitrate );
// 
//         wBitrate = atParm[1].GetBitrate();
//         wBitrate = g_cMpuBasApp.GetDecRateDebug( wBitrate );
//         atParm[1].SetBitRate( wBitrate );
//     }

//     tMediaEncryptVideo = *(TMediaEncrypt*)( cServMsg.GetMsgBody()  + 2 * sizeof(THDAdaptParam) );
//     tDoublePayloadVideo= *(TDoublePayload*)( cServMsg.GetMsgBody() + 2 * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt) );
	
	switch( CurState() )
	{
	case READY:  
	case NORMAL: //重入临时支持
		{
            BOOL32 bRet = FALSE;
			TVideoEncParam atVidEncPar[2];
			u8 byOutPutNum = 2;
			for(u8 byOutChnlIdx = 0; byOutChnlIdx < 2; byOutChnlIdx++)
			{
			    g_cMpuBasTestApp.GetDefaultParam( MEDIA_TYPE_H264, atVidEncPar[byOutChnlIdx] );

				//设置编码参数
				atVidEncPar[byOutChnlIdx].m_wVideoHeight			= 720;
				atVidEncPar[byOutChnlIdx].m_wVideoWidth				= 1280;
				atVidEncPar[byOutChnlIdx].m_wBitRate				= 2000; 
				atVidEncPar[byOutChnlIdx].m_byFrameRate				= 30;
				atVidEncPar[byOutChnlIdx].m_byMaxKeyFrameInterval	= 3000;
				atVidEncPar[byOutChnlIdx].m_byMaxQuant				= 51;
				atVidEncPar[byOutChnlIdx].m_byMinQuant				= 10;
				atVidEncPar[byOutChnlIdx].m_byEncType				= MEDIA_TYPE_H264;
				if(byOutChnlIdx == 1)
				{
					atVidEncPar[byOutChnlIdx].m_wBitRate				= 0;
				}
				mpulog( MPU_INFO, "Start Video Enc Param W*H: %d x %d, Bitrate.%dKbps, FrmRate.%d, MaxKeyFrmInt.%d\n",
								   atVidEncPar[byOutChnlIdx].m_wVideoWidth, 
								   atVidEncPar[byOutChnlIdx].m_wVideoHeight,
								   atVidEncPar[byOutChnlIdx].m_wBitRate,
								   atVidEncPar[byOutChnlIdx].m_byFrameRate,
								   atVidEncPar[byOutChnlIdx].m_byMaxKeyFrameInterval );
			}
	
			bRet = m_pAdpCroup->SetVideoEncParam(atVidEncPar, byChnIdx, byOutPutNum);
			if( !bRet )
			{
				printf( "[ProcStartAdpReq] AdpGroup SetVideoEncParam Error!\n ");
				return;
			}
// 			if ( TRUE == m_pAdpCroup->SetVideoEncParam(atVidEncPar, byChnIdx) )
// 			{
// 				// 记录通道状态
// 	    		THDAdaptParam* ptAdpParam = m_tChnInfo.m_tChnStatus.GetOutputVidParam(0);
// 
// 	    		if ( NULL != ptAdpParam )
// 				{
// 		    		ptAdpParam->SetAudType( atParm[0].GetAudType() );
// 		    		ptAdpParam->SetVidType( atParm[0].GetVidType() );
// 		    		ptAdpParam->SetBitRate( atParm[0].GetBitrate() );
// 			    	ptAdpParam->SetResolution( atParm[0].GetWidth(), atParm[0].GetHeight() );
// 					ptAdpParam->SetFrameRate( atParm[0].GetFrameRate() );
// 				}
// 
//                 THDAdaptParam* ptAdpParam1 = m_tChnInfo.m_tChnStatus.GetOutputVidParam(1);
//                 
//                 if ( NULL != ptAdpParam1 )
//                 {
//                     ptAdpParam1->SetAudType( atParm[1].GetAudType() );
//                     ptAdpParam1->SetVidType( atParm[1].GetVidType() );
//                     ptAdpParam1->SetBitRate( atParm[1].GetBitrate() );
//                     ptAdpParam1->SetResolution( atParm[1].GetWidth(), atParm[1].GetHeight() );
// 					ptAdpParam1->SetFrameRate( atParm[1].GetFrameRate() );
// 				}
// 			}

            //u8 byEncryptMode = tMediaEncryptVideo.GetEncryptMode();
			u8 abySndActPt[MAXNUM_MPU_H_VOUTPUT] = { MEDIA_TYPE_H264, MEDIA_TYPE_H264, MEDIA_TYPE_H264,
													 MEDIA_TYPE_H264, MEDIA_TYPE_H264, MEDIA_TYPE_H264 };

			m_pAdpCroup->SetKeyandPT( NULL, 0, 0,
									  abySndActPt, byOutPutNum,
									  tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad());

// 			m_pAdpCroup->SetKeyandPT( NULL, 0, 0,
// 				tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad(),
//                                           abySndActPt[0], abySndActPt[1] );

			//设置加密
			m_pAdpCroup->SetVidEncryptKey( tMediaEncryptVideo , byOutPutNum);

			//设置加密
			//m_pAdpCroup->SetSecVidEncryptKey( tMediaEncryptVideo );

			//设置动态载荷(
			m_pAdpCroup->SetVideoActivePT( tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad() );           

            //保存ConfId
			//m_tChnInfo.m_cChnConfId = cServMsg.GetConfId();

			// 设丢包重传参数
			/*
			TRSParam tNetRSParam;
            if ( atParm[0].IsNeedbyPrs() )		//是否需要包重传？ 1: 重传, 0: 不重传
			{		
				mpulog( MPU_INFO, "[ProcStartAdptReq] Bas is needed by Prs!\n" );
                tNetRSParam.m_wFirstTimeSpan  = g_cMpuBasApp.m_tPrsTimeSpan.m_wFirstTimeSpan;
				tNetRSParam.m_wSecondTimeSpan = g_cMpuBasApp.m_tPrsTimeSpan.m_wSecondTimeSpan;
				tNetRSParam.m_wThirdTimeSpan  = g_cMpuBasApp.m_tPrsTimeSpan.m_wThirdTimeSpan;
				tNetRSParam.m_wRejectTimeSpan = g_cMpuBasApp.m_tPrsTimeSpan.m_wRejectTimeSpan;
				m_pAdpCroup->SetNetSendFeedbackVideoParam( 2000, TRUE );
				m_pAdpCroup->SetNetSecSendFeedbackVideoParam( 2000, TRUE );
				m_pAdpCroup->SetNetRecvFeedbackVideoParam( tNetRSParam, TRUE );
			}
			else
			{
				mpulog( MPU_INFO, "Bas is not needed by Prs!\n" );
                tNetRSParam.m_wFirstTimeSpan  = 0;
				tNetRSParam.m_wSecondTimeSpan = 0;
				tNetRSParam.m_wThirdTimeSpan  = 0;
				tNetRSParam.m_wRejectTimeSpan = 0;
				m_pAdpCroup->SetNetSendFeedbackVideoParam( 2000, FALSE );
				m_pAdpCroup->SetNetSecSendFeedbackVideoParam( 2000, FALSE );
				m_pAdpCroup->SetNetRecvFeedbackVideoParam( tNetRSParam, FALSE );
			}
			*/
			//临时兼容重入
			if( !m_pAdpCroup->IsStart() )
			{
				BOOL bStartOk;
                bStartOk = m_pAdpCroup->StartAdapter( TRUE );
				if ( !bStartOk )
				{
					mpulog( MPU_CRIT, "channel. %u start adapter failed!\n", byChnIdx);
					return;
				}
			}

            // 平滑发送
            m_pAdpCroup->SetSmoothSendRule( g_cMpuBasApp.m_bIsUseSmoothSend );		
			
            CServMsg cServMsg;
			cServMsg.SetChnIndex( byChnIdx );
			post( MAKEIID( MPU_SERVER_APPID, 1 ), 
				C_S_STARTBASTEST_ACK, 
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen(),
				g_cMpuBasTestApp.m_dwMcuNode
				);
            
            NextState( (u32)NORMAL );            
            
		}
		break;

	default:
		mpulog( MPU_CRIT, "the chn.%d in unknown state !\n", byChnIdx );
		break;
	}

    return;
}

void CMpuBasTestClient::RestoreDefault(void)
{
#ifndef WIN32
	
	BOOL32 bRet = FALSE;
    bRet = BrdClearE2promTestFlag();	//清测试标志位
	u8 bySucessRestore = bRet?0:1;
	s32 sdwRet;
    sdwRet = post(g_cMpuBasTestApp.m_dwMcuIId, C_S_MPURESTORE_NOTIF, &bySucessRestore, sizeof(u8), g_cMpuBasTestApp.m_dwMcuNode);
    if ( sdwRet != OSP_OK )
    {
        printf( "[RestoreDefault] post failed!\n" );
    }
	else
	{
		printf( "[RestoreDefault] post sucessful!\n" );
	}
	//reset
	OspDelay(2000);
	BrdHwReset();
#endif
    return;
}
#ifdef _LINUX_
void CMpuBasTestClient::DaemonProcChangeAutoTestCmd( CMessage* const pMsg )
{
//#ifndef WIN32

	if ( NULL == pMsg)
	{
		OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] pMsg is Null!\n" );
		return;
	}
	s32 sdwAutoTest = *(s32*)pMsg->content;
	if( sdwAutoTest != 0)
	{
		BrdSetE2promTestFlag();
		OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] Set e2prom test flag 1\n" );
	}
	else 
	{
		BrdClearE2promTestFlag();
		OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] Set e2prom test flag 0\n" );
	}
//#endif

// 	BrdHwReset();
}
#endif

void CMpuBasTestClient::ChangeTestMode(u8 byMode)
{
	if( byMode != TYPE_MPUSVMP && byMode != TYPE_MPUBAS)
	{
		printf("[ChangeTestMode] Unexpected Mode.%d!\n", byMode);
		return;
	}
	printf("Change to Test Mode: %u(Vmp:11  --- bas:13)\n wait for rebooting...\n", byMode);
	
	//转模式测试
	s8   achProfileName[64] = {0};
    memset((void*)achProfileName, 0x0, sizeof(achProfileName));
    sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	
	BOOL32 bRet;
    s32  sdwTest = byMode;	
    bRet = SetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_TestMode, sdwTest );
    if( !bRet )  
	{
		printf( "[RestoreDefault] Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_TestMode );
		return;
	}
	//reset
#ifndef WIN32
	OspDelay(2000);
	BrdHwReset();
#endif
    return;
	
}

#ifdef _LINUX_
/*====================================================================
  函 数 名： SetMpuDeviceInfo
  功    能： 设置设备信息
  算法实现： 
  全局变量： 
  参    数： CDeviceInfo &cDeviceInfo
  返 回 值： 
  --------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  6/3/2009		1.0			薛亮					create
======================================================================*/
void CMpuBasTestClient::SetMpuDeviceInfo(CDeviceInfo &cDeviceInfo)
{

	s8 gs_VersionBuf[128] = {0};
    {
        strcpy(gs_VersionBuf, KDV_MCU_PREFIX);
		
        s8 achMon[16] = {0};
        u32 byDay = 0;
        u32 byMonth = 0;
        u32 wYear = 0;
        s8 achFullDate[24] = {0};
		
        s8 achDate[32] = {0};
        sprintf(achDate, "%s", __DATE__);
        StrUpper(achDate);
		
        sscanf(achDate, "%s %d %d", achMon, &byDay, &wYear );
		
        if ( 0 == strcmp( achMon, "JAN") )		 
            byMonth = 1;
        else if ( 0 == strcmp( achMon, "FEB") )
            byMonth = 2;
        else if ( 0 == strcmp( achMon, "MAR") )
            byMonth = 3;
        else if ( 0 == strcmp( achMon, "APR") )		 
            byMonth = 4;
        else if ( 0 == strcmp( achMon, "MAY") )
            byMonth = 5;
        else if ( 0 == strcmp( achMon, "JUN") )
            byMonth = 6;
        else if ( 0 == strcmp( achMon, "JUL") )
            byMonth = 7;
        else if ( 0 == strcmp( achMon, "AUG") )
            byMonth = 8;
        else if ( 0 == strcmp( achMon, "SEP") )		 
            byMonth = 9;
        else if ( 0 == strcmp( achMon, "OCT") )
            byMonth = 10;
        else if ( 0 == strcmp( achMon, "NOV") )
            byMonth = 11;
        else if ( 0 == strcmp( achMon, "DEC") )
            byMonth = 12;
        else
            byMonth = 0;
		
        if ( byMonth != 0 )
        {
            sprintf(achFullDate, "%04d%02d%02d", wYear, byMonth, byDay);
            sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
        }
        else
        {
            // for debug information
            sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
        }
    }
	
    s8* pSoftVer = (s8*)gs_VersionBuf;
//     s8* pSoftVer = (s8*)VER_MPU;
	cDeviceInfo.setSoftVersion( pSoftVer );
	
    //MAC, IP, SubMask
	s8 achMac[VERSION_LEN];
	memset( achMac, 0x0, sizeof(achMac) );
	u8 byEthId = 0;
	u8 byState = 0;
	//判断用前(外/0)网口还是后(内/1)网口，默认用外网口
	for(; byEthId < 2; byEthId ++)
	{
		if( ERROR == BrdGetEthLinkStat(byEthId, &byState) )
		{
			printf("[SetMpuDeviceInfo] Call BrdGetEthLinkStat(%u) ERROR!\n", byEthId);
			break;
		}
		if( byState == 1 )
		{
			break;
		}
	}
	
	TBrdEthParam tBrdEthParam;
	memset( &tBrdEthParam, 0x0, sizeof(tBrdEthParam) );
    BrdGetEthParam( byEthId, &tBrdEthParam );
    
	u32 dwIp = tBrdEthParam.dwIpAdrs;
    u32 dwMask = tBrdEthParam.dwIpMask; 
	sprintf(achMac,
        "%02X-%02X-%02X-%02X-%02X-%02X",
        tBrdEthParam.byMacAdrs[0],
        tBrdEthParam.byMacAdrs[1],
        tBrdEthParam.byMacAdrs[2],
        tBrdEthParam.byMacAdrs[3],
        tBrdEthParam.byMacAdrs[4],
        tBrdEthParam.byMacAdrs[5]
        ); 
	cDeviceInfo.setMac( achMac );
    cDeviceInfo.setIp( dwIp );
    cDeviceInfo.setSubMask( dwMask ); 
	
	// hw version, serialNo
	TBrdE2PromInfo tBrdE2PromInfo;
	s8 achSerial[12] = { 0 };
	s8 achHWversion[VERSION_LEN];
	
	memset( achHWversion, 0x0, sizeof(achHWversion) );
	memset( &tBrdE2PromInfo, 0x0, sizeof(tBrdE2PromInfo) );
    BrdGetE2PromInfo( &tBrdE2PromInfo );
	
    sprintf( achHWversion, "%d", tBrdE2PromInfo.dwHardwareVersion );
    memcpy( achSerial, tBrdE2PromInfo.chDeviceSerial, sizeof(tBrdE2PromInfo.chDeviceSerial) );
	cDeviceInfo.setHardVersion( achHWversion );
    cDeviceInfo.setSerial( achSerial ); 
	

	return;
}
#endif