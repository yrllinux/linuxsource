#include "eapuagent.h"

#include "boardguardbasic.h"

CBrdAgentApp	g_cBrdAgentApp;

static SEMHANDLE       g_semEAPU;

BOOL32 CBoardConfig::ReadConfig()
{
	// 读取连接的MCU信息
    BOOL32 bRet = CBBoardConfig::ReadConnectMcuInfo();
    if ( !bRet )
    {
        return bRet;
    }

	s32 sdwDefault = 0;
	s32 sdwReturnValue = 0;
	s8  achProfileName[64] = {0};
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, "mcueqp.ini");
	bRet = GetRegKeyInt( achProfileName, SECTION_EqpMixer, KEY_IsSimuApu, sdwDefault, &sdwReturnValue);
	if( bRet && sdwReturnValue != 0 )  
	{
		m_bIsSimuApu = TRUE;
	}
	else
	{
		m_bIsSimuApu = FALSE;
	}
    return TRUE;
}
#ifdef _LUNUX_
void AgentAPIEnableInLinux()
{
}
#endif
API BOOL InitBrdAgent()
{
	static BOOL32 bIsBrdInitSuccessFul = FALSE;//板初始化是否成功,防止多次对板进行初始化操作,第一次时默认不成功
	if ( bIsBrdInitSuccessFul )
	{
		printf( "[InitBrdAgent] Already Inited Successfully,So Return!\n" );
		return TRUE;
	}
    if ( ::BrdInit() == ERROR )
    {
        printf( "[InitBrdAgent]BrdInit() failed,so return!\n" );
        return FALSE;
    }
    else
    {
		bIsBrdInitSuccessFul = TRUE;
        printf( "[InitBrdAgent]BrdInit() succeed!\n" );
    }
    if ( ::IsOspInitd() )
    {
		printf( "[InitBrdAgent]Osp Has Been Inited!\n" );
    }
	else
	{
		::OspInit( TRUE, BRD_TELNET_PORT );
	}
	
#ifndef WIN32
	OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );

    // 判断是否进行生产测试
	if( BrdGetE2promTestFlag() )		
	{
		u32 dwBrdIpAddr = 0;
		if( ::BrdGetE2promIpAddr(&dwBrdIpAddr) == ERROR)//测试期间IP从E2PROM中读取
		{
			printf( "[IsProductTest]Read Board Ip from E2PROM error! so do'not do product test\n" );
			return FALSE;
		}
		printf( "[IsProductTest]Board Ip(network sequence) while in ProductTest is: %x\n", dwBrdIpAddr);
		
		//设置ip
		//判断设E0 or E1. (两口都连的话，以0口为准)
		u8 byId = 0;
		u8 byState = 0;
		u8 byEthId = ~0;
		for(; byId < 2; byId ++)
		{
			if( OK != ::BrdGetEthLinkStat(byId, &byState) )
			{
				printf(" Call BrdGetEthLinkStat(%u) ERROR!\n", byId);
				byEthId = 0;
				break;
			}
			if( byState == 1 && byEthId == (u8)~0)
			{
				byEthId = byId;
			}
			::BrdDelEthParam(byId);	//先全清掉，以防重ip设不了
		}
		
		TBrdEthParam tEtherPara;
		tEtherPara.dwIpAdrs = dwBrdIpAddr;
		tEtherPara.dwIpMask = inet_addr("255.0.0.0");
		memset( tEtherPara.byMacAdrs, 0, sizeof(tEtherPara.byMacAdrs) );
		printf("[BrdSetEthParam]Set Eth(%d)  IP(%d)(%x) \n",byEthId,dwBrdIpAddr,dwBrdIpAddr);
		if( ERROR == ::BrdSetEthParam( byEthId, Brd_SET_IP_AND_MASK, &tEtherPara ) )
		{
			printf("[BrdSetEthParam]Set Eth %u IP failed\n",byEthId);
		}
		else
		{
			printf("[BrdSetEthParam]Set Eth %u Ip successfully!\n", byEthId);
		}

		g_cBrdAgentApp.SetProductTest(TRUE);
		OspPrintf(TRUE,FALSE,"starting product test  ......\n");
		printf("starting product test  ......\n");
		return TRUE;
	}
#else
	s32 sdwDefault = 0;
	s32 sdwReturnValue = 0;
	s8  achProfileName[64] = {0};
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");

	// 判断是否进行生产测试
	if( ::GetRegKeyInt( achProfileName, "BoardDebug", "IsTest", sdwDefault, &sdwReturnValue ) 
		&& sdwReturnValue != 0 )  
	{
		g_cBrdAgentApp.SetProductTest(TRUE);
		OspPrintf(TRUE,FALSE,"starting product test  ......\n");
		printf("starting product test  ......\n");

		return TRUE;
	}
#endif

    if ( !::OspSemBCreate(&g_semEAPU) )
	{
		OspPrintf( TRUE, FALSE,"[InitBrdAgent]OspSemBCreate() failed\n" );
        printf( "[InitBrdAgent]OspSemBCreate() failed\n" );
		return FALSE;
	}
    if ( !::OspSemTake( g_semEAPU ) )
    {
        OspPrintf( TRUE, FALSE, "[InitBrdAgent]OspSemTake(g_semEAPU) error!\n" );
		printf( "[InitBrdAgent]OspSemTake(g_semEAPU) error!\n" );
		return FALSE;
    }

	// 读取配置信息
	if( !g_cBrdAgentApp.ReadConfig() )  
	{
		OspPrintf( TRUE, FALSE, "[InitBrdAgent]ReadConfig error!\n" );
		printf( "[InitBrdAgent]ReadConfig() failed.!\n" );
		return FALSE;
	}

	// 创建单板代理和单板守卫应用
	g_cBrdAgentApp.CreateApp( "EapuAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );
	g_cBrdGuardApp.CreateApp( "EapuGuard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);

    TBrdPos tBoardPosition;
    tBoardPosition = g_cBrdAgentApp.GetBrdPosition();
	
	// 是否模拟APU信息
	if ( g_cBrdAgentApp.IsSimuApu() )
	{
		OspPrintf( TRUE, FALSE, "[InitBrdAgent]EAPU SIMU APU!\n" );
		printf( "[InitBrdAgent]EAPU SIMU APU!\n" );
		tBoardPosition.byBrdID = BRD_TYPE_APU;
	}

	OspPrintf(TRUE,FALSE,"**************info:byBrdID=[%d]byBrdLayer=[%d]byBrdSlot=[%d]**************\n",tBoardPosition.byBrdID,
		tBoardPosition.byBrdLayer,tBoardPosition.byBrdSlot);
	printf("**************info:byBrdID=[%d]byBrdLayer=[%d]byBrdSlot=[%d]**************\n",tBoardPosition.byBrdID,
		tBoardPosition.byBrdLayer,tBoardPosition.byBrdSlot);
    g_cBrdAgentApp.SetBrdPosition( tBoardPosition );

	CBBoardConfig *pBBoardConfig = (CBBoardConfig*)&g_cBrdAgentApp;
	// 单板代理开始工作
	OspPost( MAKEIID(AID_MCU_BRDAGENT, 1), OSP_POWERON, &pBBoardConfig, sizeof(pBBoardConfig) );
	// 单板守卫模块开始工作
	// 单板守卫保存单板类型信息，方便根据不同单板类型有不同告警 [12/1/2011 chendaiwei]
	u8 byBrdId = pBBoardConfig->GetBrdPosition().byBrdID;
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON,&byBrdId,sizeof(u8));
    
	if ( !OspSemTake( g_semEAPU) )
    {
        OspPrintf( TRUE, FALSE, "[InitBrdAgent]OspSemTake(g_semEAPU) error!\n" );
    }
	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0 );	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0 );
	return TRUE;
}


API u32 BrdGetDstMcuNode()
{
    return g_cBrdAgentApp.m_dwDstMcuNodeA;
}

API u32 BrdGetDstMcuNodeB()
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}
void CBoardAgent::InstanceEntry( CMessage * const pcMsg )
{
	if ( NULL == pcMsg )
	{
		OspPrintf( TRUE, FALSE, "[InstanceEntry]the pointer can not be NULL!\n" );
		return;
	}

	OspPrintf( TRUE, FALSE, "[InstanceEntry] %u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event) );

	switch( pcMsg->event )
	{
// 	case OSP_POWERON:                      //单板启动
// 	case BRDAGENT_CONNECT_MANAGERA_TIMER:  //连接管理程序超时
//     case BRDAGENT_CONNECT_MANAGERB_TIMER:  //连接管理程序超时
// 	case BRDAGENT_REGISTERA_TIMER:		   //注册时间超时
//     case BRDAGENT_REGISTERB_TIMER:		   //注册时间超时
// 	case MPC_BOARD_REG_ACK:                //注册应答消息
// 	case MPC_BOARD_REG_NACK:			   //注册否定应答消息
// 	case BRDAGENT_GET_CONFIG_TIMER:        //等待配置应答消息超时
// 	case MPC_BOARD_ALARM_SYNC_REQ:         //管理程序的告警同步请求
// 	case MPC_BOARD_BIT_ERROR_TEST_CMD:     //单板误码测试命令
// 	case MPC_BOARD_TIME_SYNC_CMD:          //单板时间同步命令
// 	case MPC_BOARD_SELF_TEST_CMD:          //单板自测命令
// 	case MPC_BOARD_RESET_CMD:              //单板重启动命令
// 	case MPC_BOARD_UPDATE_SOFTWARE_CMD:    //软件更新命令
// 	case MPC_BOARD_GET_STATISTICS_REQ:     //获取单板的统计信息
// 	case MPC_BOARD_GET_VERSION_REQ:        //获取单板的版本信息
// 	case OSP_DISCONNECT:
// 	//以下是告警处理的相关消息
// 	case SVC_AGT_MEMORY_STATUS:            //内存状态改变
// 	case SVC_AGT_FILESYSTEM_STATUS:        //文件系统状态改变
// 	//以下是测试用消息
// 	case BOARD_MPC_CFG_TEST:
// 	case BOARD_MPC_GET_ALARM:
// 	case BOARD_MPC_MANAGERCMD_TEST:
// 	case MPC_BOARD_GET_MODULE_REQ:         //获取单板的模块信息
// 		CBBoardAgent::InstanceEntry( pcMsg );
// 		break;

	case MPC_BOARD_GET_CONFIG_ACK:         //取配置信息应答消息
		ProcBoardGetConfigAck( pcMsg );
		break;
		
	case MPC_BOARD_GET_CONFIG_NACK:        //取配置信息否定应答
		ProcBoardGetConfigNAck( pcMsg );
		break;

	case MPC_BOARD_LED_STATUS_REQ:
		ProcLedStatusReq( pcMsg );
		break;

	case BOARD_LED_STATUS:
		ProcBoardLedStatus( pcMsg );
		break;

	case MCU_BOARD_CONFIGMODIFY_NOTIF:
		ProcBoardConfigModify();
		break;

	default:
		CBBoardAgent::InstanceEntry( pcMsg );
		break;
//         OspPrintf( TRUE, FALSE, "[InstanceEntry][error] BoardAgent: receive unknown msg %d<%s> in mmpAgent InstanceEntry! \n",
//                               pcMsg->event, OspEventDesc(pcMsg->event) );
// 		break;
	}
	
	return;
}

void CBoardAgent::ProcBoardGetConfigAck( CMessage * const pcMsg )
{
    if( NULL == pcMsg )
    {
        OspPrintf( TRUE, FALSE, "[ProcBoardGetConfigAck][error] ProcBoardGetConfigAck pointer is Null\n" );
        return;
    }
    u8	byPeriCount;
	u8	byPeriType;
	u16	wLoop, wIndex;
	TEqpMixerEntry*	ptMixerCfg;		//MIXER的配置信息
	if (pcMsg->content == NULL)
	{
		OspPrintf( TRUE, FALSE, "[ProcBoardGetConfigAck][error] pcMsg->content == NULL\n" );
		return;
	}
	byPeriCount = pcMsg->content[0]; // 外设个数
	g_cBrdAgentApp.SetEMixerNum(byPeriCount);
	wIndex = 1;
	for( wLoop = 0; wLoop < min(MAXNUM_EAPU_MIXER,byPeriCount); wLoop++ )
	{			
		byPeriType = pcMsg->content[wIndex++];  // 外设类型
		switch( byPeriType )                    // 设置该单板相应的配置信息
		{
			case EQP_TYPE_MIXER: //保留此项源于EAPU有可能模拟APU[2/14/2012 chendaiwei]
			case 12: //  [2/14/2012 chendaiwei]原EQP_TYPE_EMIXER值为12,V4R7 MCU兼容老的EAPU，仍会发送值12给EAPU板。并不区分老EAPU板和新EAPU板。
				ptMixerCfg = (TEqpMixerEntry*)(pcMsg->content + wIndex);
				wIndex += sizeof(TEqpMixerEntry);
				g_cBrdAgentApp.SetEqpEMixerEntry(wLoop,*ptMixerCfg);
				break;
			default:
				OspPrintf( TRUE, FALSE, "*******unknown type[%d]*****\n",byPeriType);
		}
	}
    BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );// 将MLINK灯点亮
    // 释放信号量
    OspSemGive( g_semEAPU );
	KillTimer( BRDAGENT_GET_CONFIG_TIMER );
	NEXTSTATE( STATE_NORMAL );
	return;
}

void CBoardAgent::ProcBoardGetConfigNAck(CMessage* const pcMsg)
{
    OspPrintf( TRUE, FALSE, "[eapu][error] ProcBoardGetConfigNAck(%d)\n",pcMsg != NULL ? pcMsg->srcnode : 0 );
    return;
}

void CBoardAgent::ProcLedStatusReq( CMessage* const pcMsg )
{
#ifndef WIN32
	u8 abyBuf[32];
	u8 byCount;
    TBrdLedState tBrdLedState;

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
        BrdQueryLedState( &tBrdLedState );
        memcpy(&m_tLedState, &tBrdLedState, sizeof(tBrdLedState));
		
        TBrdPos tBrdPos;
        tBrdPos = g_cBrdAgentApp.GetBrdPosition();

		memcpy(abyBuf, &tBrdPos, sizeof(tBrdPos) );
		byCount = (u8)m_tLedState.dwLedNum;
		memcpy(abyBuf+sizeof(tBrdPos), &byCount, sizeof(byCount) ); 
		memcpy(abyBuf+sizeof(tBrdPos)+sizeof(byCount), m_tLedState.nlunion.byLedNo, byCount);
		
		PostMsgToManager( BOARD_MPC_LED_STATUS_ACK, abyBuf, 
			              sizeof(tBrdPos)+sizeof(u8)+ byCount * sizeof(u8) );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
#endif
	OspPrintf(TRUE, FALSE, "BoardAgent:message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
	return;
}


/*====================================================================
    函数名      ：ProcBoardLedStatus
    功能        ：面板灯状态改变消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardLedStatus( CMessage* const pcMsg )
{
#ifndef WIN32
	u8 abyBuf[32];
	u8 byCount;

	TBrdLedState tLedState;
	memcpy( &tLedState, pcMsg->content, sizeof(TBrdLedState) );

	if( memcmp(m_tLedState.nlunion.byLedNo, tLedState.nlunion.byLedNo, tLedState.dwLedNum) == 0 )
		return;

	memcpy(&m_tLedState, &tLedState, sizeof(m_tLedState));

	TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );
	byCount = (u8)tLedState.dwLedNum;
	memcpy(abyBuf+sizeof(TBrdPos), &byCount, sizeof(u8) ); 
	memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), tLedState.nlunion.byLedNo, byCount);

	if (g_cBrdAgentApp.m_bPrintBrdLog)
	{
		OspPrintf(TRUE, FALSE, "CBoardAgent::ProcLedStatusReq: Led:\n");
		
		for (u32 dwIndex = 0; dwIndex < m_tLedState.dwLedNum; ++dwIndex)
		{
			OspPrintf(TRUE, FALSE," %d ", m_tLedState.nlunion.byLedNo[dwIndex]);
		}
		OspPrintf(TRUE, FALSE, "\n");
	}
					
	PostMsgToManager( BOARD_MPC_LED_STATUS_NOTIFY, abyBuf, 
		sizeof(TBrdPos)+sizeof(u8)+byCount * sizeof(u8) );
#endif
	OspPrintf(TRUE, FALSE, "BoardAgent:message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
}

/*=============================================================================
函 数 名： ProcBoardConfigModify
功    能： 
算法实现： 
全局变量： 
参    数： CMessage* const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/9/19   4.0			周广程                  创建
=============================================================================*/
void CBoardAgent::ProcBoardConfigModify()
{

}