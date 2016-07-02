/*****************************************************************************
   模块名      : Board Agent
   文件名      : bapagent.cpp
   相关文件    : 
   文件实现功能: 单板代理
   作者        : guzh
   版本        : V4.5  Copyright(C) 2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/

#include "mpuagent.h"
//#include "agentcommon.h"

#include "boardguardbasic.h"

CBrdAgentApp	g_cBrdAgentApp;

SEMHANDLE       g_semMPU;

BOOL32 CBoardConfig::ReadConfig()
{
    BOOL32 bRet = CBBoardConfig::ReadConnectMcuInfo();
    if ( !bRet )
    {
        return bRet;
    }
	
#ifdef _WIN32  //win32 下的层槽号取配置文件
    s8    achProfileName[64] = {0};
    
    memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
		
	
	//Layer
	s32  sdwLayer = 0;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Layer, 0, &sdwLayer );
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_Layer );
		return FALSE;
	}
	
    //Slot
    s32 sdwSlot = 0;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Slot, 0, &sdwSlot );
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_Slot );
		return FALSE;
	}
	
    //Type
    s32 sdwType = 0;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_Type, 0, &sdwType );
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_Type );
		return FALSE;
	}
	
	TBrdPos tBrdPos;
	tBrdPos.byBrdID = (u8)sdwType;
	tBrdPos.byBrdLayer = (u8)sdwLayer;   	
	tBrdPos.byBrdSlot = (u8)sdwSlot;
	g_cBrdAgentApp.SetBrdPosition(tBrdPos);
	
	//BrdIp
	u32 dwBrdIpAddr = 0;
	
	s8 achBrdIpAddr[64] = {0};
	
	bRet = GetRegKeyString( achProfileName, SECTION_BoardConfig, KEY_BoardIpAddr, "0.0.0.0", achBrdIpAddr, 64 );
	
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_BoardIpAddr );
		return FALSE;
	}
	
	dwBrdIpAddr = ntohl(inet_addr(achBrdIpAddr));
	
	g_cBrdAgentApp.SetBrdIpAddr(dwBrdIpAddr);
    
	
	s32 sdwPID = 0;
	bRet = GetRegKeyInt( achProfileName, SECTION_BoardConfig, KEY_BrdType, 0, &sdwPID );
	if( !bRet )  
	{
		printf( "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardConfig, KEY_BrdType );
		return FALSE;
	}
	//保存brdtype
    TBrdPos tBdPos = g_cBrdAgentApp.GetBrdPosition();
	tBdPos.byBrdID = (u8)sdwPID;
	g_cBrdAgentApp.SetBrdPosition(tBdPos);
#endif

// 	s8    achProfileName[64] = {0};
//     memset((void*)achProfileName, 0x0, sizeof(achProfileName));
// 	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	
	
// 	s32  dwTest = 0;
// 	bRet = GetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_IsAutoTest, 0, &dwTest );
// 	if( !bRet )  
// 	{
// 		printf( "[BrdAgent] Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_IsAutoTest );
// 		//return FALSE;
// 	}
// 	g_cBrdAgentApp.SetIsTest( dwTest );

//IsAutoTest
// 	BOOL32 bAutoTest = FALSE;
// #ifdef _LINUX_
// 	bAutoTest = BrdGetE2promTestFlag();
// #endif
// 	g_cBrdAgentApp.SetIsTest( bAutoTest );


// 	if(dwTest == 1)			//VMP 生产测试
// 	{
// 		g_cBrdAgentApp.SetEqpRcvStartPort( 40700 );
// 		printf( "[BrdAgent] auto test-- SetEqpRcvStartPort.%d\n",g_cBrdAgentApp.GetEqpRcvStartPort());
// 	}
// 	else if(dwTest == 2)	//BAS 生产测试
// 	{
// 		g_cBrdAgentApp.SetEqpRcvStartPort( 40600 );
// 	}
    return TRUE;
}


// void AgentAPIEnableInLinux()
// {
// #ifdef _LUNUX_
//     
// #endif
// }

API BOOL InitBrdAgent()
{
    BOOL bResult;

	static u16 wFlag = 0;
	if ( wFlag != 0 )
	{
		printf( "[mpu] Already Inited!\n" );
		return TRUE;
	}

    printf( "[mpu] mpu BrdInit() here!\n" );
    if ( BrdInit() )
    {
        printf( "[mpu][error] mpu BrdInit() failed!\n" );
        return FALSE;
    }
    
    ::OspInit( TRUE, BRD_TELNET_PORT );
    
    //Osp telnet 初始授权 [11/28/2006-zbq]
#ifndef WIN32
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
#endif

// #undef OSPEVENT
// #define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
//     
// #ifdef _EV_AGTSVC_H_
// #undef _EV_AGTSVC_H_
// #include "evagtsvc.h"
// #define _EV_AGTSVC_H_
// #else
// #include "evagtsvc.h"
// #undef _EV_AGTSVC_H_
// #endif    

//	AgentAPIEnableInLinux();

	wFlag++;

    if ( !OspSemBCreate(&g_semMPU) )
	{
        printf( "[mpu][error] OspSemBCreate failed\n" );
		return FALSE;
	}

	bResult = OspSemTake( g_semMPU );
    if ( bResult == FALSE )
    {
        OspPrintf( TRUE, FALSE, "[mpu][error] OspSemTake(g_semMMP) error!\n" );
    }
	
	BOOL32 bAutoTest = FALSE;
#ifdef _LINUX_
	bAutoTest = BrdGetE2promTestFlag();
#else
	s8    achProfileName[64] = {0};
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	
	s32  dwTest = 0;
	BOOL32 bRet = GetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_IsAutoTest, 0, &dwTest );
	if( !bRet )  
	{
		printf( "Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_IsAutoTest );
	}
	bAutoTest = (dwTest != 0) ? TRUE: FALSE;
#endif
	printf("bAutoTest is: %d\n", bAutoTest);
	OspPrintf(TRUE, FALSE, "bAutoTest is: %d\n", bAutoTest);
	g_cBrdAgentApp.SetIsTest( bAutoTest );
	
	if(g_cBrdAgentApp.GetIsTest())		//走生产测试路线
	{
		u32 dwBrdIpAddr = 0;
#ifdef _LINUX_
		BrdGetE2promIpAddr(&dwBrdIpAddr);//测试期间IP从E2PROM中读取
		
		if(dwBrdIpAddr == 0)
		{
			printf( "Read Board Ip from E2PROM error!\n" );
			return FALSE;
		}
		
		printf( "Board Ip(network sequence) while in AutoTest is: %x\n", dwBrdIpAddr);
		
		//设置ip
		//判断设E0 or E1. (两口都连的话，以0口为准)
//		u8 byId = 0;
		u8 byState = 0;
		u8 byEthId = ~0;
		for(u8 byId = 0; byId < 2; byId ++)
		{
			if( ERROR == BrdGetEthLinkStat(byId, &byState) )
			{
				printf(" Call BrdGetEthLinkStat(%u) ERROR!\n", byId);
				byEthId = 0;
				break;
			}
			if( byState == 1 && byEthId == (u8)~0)
			{
				byEthId = byId;
			}
			BrdDelEthParam(byId);	//先全清掉，以防重ip设不了
		}
		
        TBrdEthParam tEtherPara;
        tEtherPara.dwIpAdrs = dwBrdIpAddr;
        tEtherPara.dwIpMask = inet_addr("255.0.0.0");
        memset( tEtherPara.byMacAdrs, 0, sizeof(tEtherPara.byMacAdrs) );
        if( ERROR == BrdSetEthParam( byEthId, Brd_SET_IP_AND_MASK, &tEtherPara ) )
        {
            printf("Set Eth %u IP failed\n",byEthId);
            //return FALSE;
        }
		else
		{
			printf("Set Eth %u Ip successfully!\n", byEthId);
		}
#else
		s8    achDefStr[] = "10.1.1.2";
		s8    achReturn[MAX_VALUE_LEN + 1] ;
		memset(achReturn, '\0', sizeof(achReturn));
		bResult = GetRegKeyString( achProfileName, SECTION_BoardDebug, KEY_BoardIpAddr,
			achDefStr, achReturn, MAX_VALUE_LEN + 1 );
		if ( !bResult )
		{
			printf("Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_BoardIpAddr );
			//memcpy(achReturn, achDefStr, min(sizeof(achDefStr), sizeof(achReturn)) );
			memcpy(achReturn, achDefStr, sizeof(achDefStr)) ;
		}
		dwBrdIpAddr = ntohl(INET_ADDR(achReturn));
#endif
		g_cBrdAgentApp.SetBrdIpAddr(dwBrdIpAddr);
		
		printf("return true dircectly here due to the purpose of autotest!!!!!\n");
		return TRUE;
	}

	//得到配置信息
    bResult = g_cBrdAgentApp.ReadConfig();
    if( bResult != TRUE )
    {
        printf( "[mpu][error] ReadConfig failed.\n" );
        return FALSE;
    }
	
// 	if(g_cBrdAgentApp.GetIsTest())
// 	{
// 		printf("[InitBrdAgent]avert the process of BoardAgent due to mpu auto test, return TRUE here\n");
// 		return TRUE;
// 	}
	//创建单板代理应用
	g_cBrdAgentApp.CreateApp( "MpuAgent", AID_MCU_BRDAGENT, APPPRI_BRDAGENT );
	//创建单板守卫应用
	g_cBrdGuardApp.CreateApp( "MpuGuard", AID_MCU_BRDGUARD, APPPRI_BRDGUARD);
    /*
#ifdef _WIN32
    if ( 0 == g_cBrdAgentApp.GetBrdIpAddr() )
    {
        OspPrintf( TRUE,FALSE, "[mpu] write ip here!\n" );
        g_cBrdAgentApp.SetBrdIpAddr( 0x81ac10ac );
    }
#else
    if ( 0 == g_cBrdAgentApp.GetBrdIpAddr() )
    {
        OspPrintf( TRUE,FALSE, "[mpu] write ip here!\n" );
        g_cBrdAgentApp.SetBrdIpAddr( 0xac10ac81 );
    }
#endif*/

//     TBrdPos m_tBoardPosition;
//     m_tBoardPosition = g_cBrdAgentApp.GetBrdPosition();
// #ifdef _WIN32
//     m_tBoardPosition.byBrdID    = 66;
//     m_tBoardPosition.byBrdLayer = 3 ;
//     m_tBoardPosition.byBrdSlot  = 6;
//     g_cBrdAgentApp.SetBrdPosition( m_tBoardPosition );
// #else
//     // byBrdID待进一步调试
//     m_tBoardPosition.byBrdID    = 66;
//     g_cBrdAgentApp.SetBrdPosition( m_tBoardPosition );
// #endif

	//单板代理开始工作
	CBBoardConfig *pBBoardConfig = (CBBoardConfig*)&g_cBrdAgentApp;
	OspPost( MAKEIID(AID_MCU_BRDAGENT, 1), OSP_POWERON, &pBBoardConfig, sizeof(pBBoardConfig) );
	// 单板守卫模块开始工作
	// 单板守卫保存单板类型信息，方便根据不同单板类型有不同告警 [12/1/2011 chendaiwei]
	u8 byBrdId = pBBoardConfig->GetBrdPosition().byBrdID;
	OspPost( MAKEIID(AID_MCU_BRDGUARD,0,0), BOARD_GUARD_POWERON,&byBrdId,sizeof(u8));
	//wait here
	bResult = OspSemTake( g_semMPU);
    if ( bResult == FALSE )
    {
        OspPrintf( TRUE, FALSE, "[mpu][error] BrdAgent: OspSemTake(g_semMMP) error!\n" );
    }

	OspSetLogLevel( AID_MCU_BRDAGENT, 0, 0 );	 
	OspSetTrcFlag( AID_MCU_BRDAGENT, 0, 0 );

	return TRUE;
}

/*lint -save -e714*/
API u32 BrdGetDstMcuNode()
{
    return g_cBrdAgentApp.m_dwDstMcuNodeA;
}

API u32 BrdGetDstMcuNodeB()
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
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
    03/08/20    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::InstanceEntry( CMessage * const pcMsg )
{
	if ( NULL == pcMsg )
	{
		OspPrintf( TRUE, FALSE, "[Mpu][error] the pointer can not be NULL!\n" );
		return;
	}

	OspPrintf( TRUE, FALSE, "[mpu] %u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event) );

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
		// ProcBoardConfigModify( pcMsg );
		break;

	default:
		CBBoardAgent::InstanceEntry( pcMsg );
		break;
	}
	
	return;
}

/*====================================================================
    函数名      ：ProcBoardGetConfigAck
    功能        ：实例消息处理入口函数，必须override
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/20    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardGetConfigAck( CMessage * const pcMsg )
{
    if( NULL == pcMsg )
    {
        OspPrintf( TRUE, FALSE, "[mpu][error] ProcBoardGetConfigAck pointer is Null\n" );
        return;
    }
    TMpuEBapEVpuConfigAck tMpuEBapEVpuConfigAck;
    memcpy( &tMpuEBapEVpuConfigAck, pcMsg->content, sizeof(TMpuEBapEVpuConfigAck) );

	if ( 0 != g_cBrdAgentApp.GetEqpId() && 
		 (g_cBrdAgentApp.GetEqpId() != tMpuEBapEVpuConfigAck.GetEqpId()) )
	{
		BrdHwReset();
	}

	printf("[ProcBoardGetConfigAck] Mpu's WorkMode is %d!\n ", tMpuEBapEVpuConfigAck.GetEqpType());
    // 保存EqpType（Workmode）和EqpId
    g_cBrdAgentApp.SetEqpId( tMpuEBapEVpuConfigAck.GetEqpId() );
    g_cBrdAgentApp.SetEqpIdB( tMpuEBapEVpuConfigAck.GetEqpId2() );
    g_cBrdAgentApp.SetWorkMode( tMpuEBapEVpuConfigAck.GetEqpType() );
    g_cBrdAgentApp.SetEqpRcvStartPort( tMpuEBapEVpuConfigAck.GetEqpRcvStartPort() );
    g_cBrdAgentApp.SetEqpRcvStartPort2( tMpuEBapEVpuConfigAck.GetEqpRcvStartPort2() );

#ifndef _WIN32
	BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );// 将MLINK灯点亮
#endif

    // 释放信号量
    OspSemGive( g_semMPU );

	KillTimer( BRDAGENT_GET_CONFIG_TIMER );
	NEXTSTATE( STATE_NORMAL );

	return;
}

/*lint -save -e715*/
/*====================================================================
    函数名      ：ProcBoardGetConfigNAck
    功能        ：实例消息处理入口函数，必须override
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/20    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardGetConfigNAck( CMessage* const pcMsg )
{
    OspPrintf( TRUE, FALSE, "[mpu][error] ProcBoardGetConfigNAck\n" );
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

		if (g_cBrdAgentApp.m_bPrintBrdLog)
		{
			OspPrintf(TRUE, FALSE, "CBoardAgent::ProcLedStatusReq: Led:\n");
			
			for (u32 dwIndex = 0; dwIndex < m_tLedState.dwLedNum; ++dwIndex)
			{
				OspPrintf(TRUE, FALSE," %d ", m_tLedState.nlunion.byLedNo[dwIndex]);
			}
			OspPrintf(TRUE, FALSE, "\n");
		}	
		PostMsgToManager( BOARD_MPC_LED_STATUS_ACK, abyBuf, 
			              sizeof(tBrdPos)+sizeof(u8)+ byCount * sizeof(u8) );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
#endif

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
    10/05/13    4.6         lukunpeng       修改
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
					
	PostMsgToManager( BOARD_MPC_LED_STATUS_NOTIFY, abyBuf, 
		sizeof(TBrdPos)+sizeof(u8)+byCount * sizeof(u8) );
#endif
	
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
void CBoardAgent::ProcBoardConfigModify( CMessage* const pcMsg )
{

}
/*lint -restore*/