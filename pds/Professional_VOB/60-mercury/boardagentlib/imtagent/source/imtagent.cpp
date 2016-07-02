 /*****************************************************************************
   模块名      : Board Agent
   文件名      : imtagent.cpp
   相关文件    : 
   文件实现功能: 单板代理实现，完成告警处理和与MANAGER的交互
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/25  1.0         jianghy       创建
   2004/12/05  3.5         李 博        新接口的修改
   2006/04/18  4.0         john         增加软件升级功能
******************************************************************************/
#include "osp.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "imtagent.h"
#include "mcuagtstruct.h"
#include "mcustruct.h"

#ifdef _VXWORKS_
#include "brddrvLib.h"
#include "timers.h"
#include <dirent.h>
#endif

// [pengjie 2010/3/9] CRI2/MPC2 支持
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

extern SEMHANDLE g_semImt;

CBrdAgentApp	g_cBrdAgentApp;	//单板代理应用实例
BOOL32 g_bUpdateSoftBoard = TRUE;   // 升级软件开关

s32 g_nLogLevel = 0;
/*=============================================================================
  函 数 名： brdagtlog
  功    能： 
  算法实现： 
  全局变量： 
  参    数： char* fmt
             ...
  返 回 值： void 
=============================================================================*/
void brdagtlog(char* fmt, ...)
{
    char PrintBuf[255];
    int BufLen = 0;
    va_list argptr;
	if( 1 == g_nLogLevel )
	{		  
		BufLen = sprintf(PrintBuf,"[Imt]:"); 
		va_start(argptr, fmt);
		BufLen += vsprintf(PrintBuf+BufLen, fmt, argptr); 
		va_end(argptr); 
		BufLen += sprintf(PrintBuf+BufLen, "\n"); 
		OspPrintf(1, 0, PrintBuf); 
	}
    return;
}

// 构造函数
CBoardAgent::CBoardAgent()
{
	memset(&m_tBoardPosition, 0, sizeof(m_tBoardPosition) );
    FreeDataA();
    FreeDataB();

	m_bIsTest = FALSE;
	memset( m_atBoardAlarmTable, 0, sizeof(m_atBoardAlarmTable) );
	m_dwBoardAlarmTableMaxNo = 0;

	// 测试用
	m_wLastEvent = 0;
	memset( m_abyLastEventPara, 0, sizeof(m_abyLastEventPara) );
	return;
}

//析构函数
CBoardAgent::~CBoardAgent()
{
	return;
}

/*=============================================================================
  函 数 名： FreeDataA
  功    能： 清空A板状态数据
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CBoardAgent::FreeDataA(void)
{
	m_dwDstNode = INVALID_NODE;
	m_dwDstIId = INVALID_INS;
    return;
}

/*=============================================================================
  函 数 名： FreeDataB
  功    能： 清空B板状态数据
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CBoardAgent::FreeDataB(void)
{
    m_dwDstIIdB = INVALID_INS;
    m_dwDstNodeB = INVALID_NODE;
    return;
}

/*====================================================================
    函数名      ：InstanceExit
    功能        ：实例退出函数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/20    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::InstanceExit()
{
#ifndef WIN32
#ifdef IMT
	BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF );
#endif
#endif
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
void CBoardAgent::InstanceEntry(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
	{
		OspPrintf( TRUE, FALSE, "[Imt]: The msg's pointer is NULL(InstanceEntry)!");
		return;
	}

	brdagtlog("[Imt]: %u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch( pcMsg->event )
	{
	case BOARD_AGENT_POWERON:            //单板启动
        ProcBoardPowerOn( pcMsg ) ;
		break;

	case BRDAGENT_CONNECT_MANAGERA_TIMER:          //连接管理程序超时
		ProcBoardConnectManagerTimeOut( TRUE );
		break;

    case BRDAGENT_CONNECT_MANAGERB_TIMER:          //连接管理程序超时
		ProcBoardConnectManagerTimeOut( FALSE );
		break;

	case BRDAGENT_REGISTERA_TIMER:		         //注册时间超时
		ProcBoardRegisterTimeOut( TRUE );
		break;

    case BRDAGENT_REGISTERB_TIMER:		         //注册时间超时
		ProcBoardRegisterTimeOut( FALSE );
		break;

	case MPC_BOARD_REG_ACK:              //注册应答消息
		ProcBoardRegAck( pcMsg );
		break;
		
	case MPC_BOARD_REG_NACK:			 //注册否定应答消息
		ProcBoardRegNAck( pcMsg );
		break;

	case MPC_BOARD_GET_CONFIG_ACK:       //取配置信息应答消息
		ProcBoardGetConfigAck( pcMsg );
		break;
		
	case MPC_BOARD_GET_CONFIG_NACK:      //取配置信息否定应答
		ProcBoardGetConfigNAck( pcMsg );
		break;

	case BRDAGENT_GET_CONFIG_TIMER:               //等待配置应答消息超时
		ProcGetConfigTimeOut( pcMsg );
		break;

	case MPC_BOARD_ALARM_SYNC_REQ:       //管理程序的告警同步请求
		ProcAlarmSyncReq( pcMsg );
		break;

	case MPC_BOARD_LED_STATUS_REQ:
		ProcLedStatusReq( pcMsg );
		break;

	case MPC_BOARD_BIT_ERROR_TEST_CMD:   //单板误码测试命令
		ProcBitErrorTestCmd( pcMsg );
		break;

	case MPC_BOARD_TIME_SYNC_CMD:        //单板时间同步命令
		ProcTimeSyncCmd( pcMsg );
		break;

	case MPC_BOARD_SELF_TEST_CMD:        //单板自测命令
		ProcBoardSelfTestCmd( pcMsg );
		break;

	case MPC_BOARD_RESET_CMD:            //单板重启动命令
		ProcBoardResetCmd( pcMsg );
		break;

	case MPC_BOARD_UPDATE_SOFTWARE_CMD:  //软件更新命令
		ProcUpdateSoftwareCmd( pcMsg );
		break;

	case MPC_BOARD_GET_STATISTICS_REQ:   //获取单板的统计信息
		ProcGetStatisticsReq( pcMsg );
		break;

	case MPC_BOARD_GET_VERSION_REQ:      //获取单板的版本信息
		ProcGetVersionReq( pcMsg );
		break;

	case MPC_BOARD_GET_MODULE_REQ:      //获取单板的模块信息
		ProcGetModuleInfoReq( pcMsg );
		break;

	case OSP_DISCONNECT:
		ProcOspDisconnect( pcMsg );
		break;

	//以下是告警处理的相关消息
	case SVC_AGT_MEMORY_STATUS:          //内存状态改变
		ProcBoardMemeryStatus( pcMsg );
		break;

	case SVC_AGT_FILESYSTEM_STATUS:      //文件系统状态改变
		ProcBoardFileSystemStatus( pcMsg );
		break;

	case BOARD_LED_STATUS:
		ProcBoardLedStatus( pcMsg );
		break;

	case MCU_BRD_FAN_STATUS:
		ProcImtFanStatus( pcMsg );
		break;

	//以下是测试用消息
	case BOARD_MPC_CFG_TEST:
		ProcBoardGetCfgTest( pcMsg );
		break;

	case BOARD_MPC_GET_ALARM:
		ProcBoardGetAlarm( pcMsg );
		break;

	case BOARD_MPC_MANAGERCMD_TEST:
		ProcBoardGetLastManagerCmd( pcMsg );
		break;

	default:
        OspPrintf(TRUE, FALSE, "BoardAgent: receive unknown msg %d<%s> in ImtAgent InstanceEntry! \n",
                               pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
	
	return;
}

//模拟单板BSP上的函数 测试用
#ifdef WIN32
void BrdQueryPosition(TBrdPosition* ptPosition)
{
	return ;
}
#else
#ifndef IMT
void BrdQueryPosition(TBrdPosition* ptPosition)
{
	return ;
}
#endif
#endif


/*====================================================================
    函数名      ：GetBoardCfgInfo
    功能        ：获取要连接的MPC的IP地址和PORT
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：成功返回TRUE，反之返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/20    1.0         jianghy       创建
====================================================================*/
BOOL CBoardAgent::GetBoardCfgInfo()
{
    s8   achProfileName[32];
    BOOL bResult;
    s8   achDefStr[] = "Cannot find the section or key";
    s8   achReturn[MAX_VALUE_LEN + 1];
    s32  nValue;

	// 在测试时要建立节点
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");
	bResult = GetRegKeyInt( achProfileName, "BoardDebug", "IsTest", 0, &nValue );
	if( bResult == TRUE )  
	{
		m_bIsTest = (nValue != 0);
		if(m_bIsTest)
        {
		OspCreateTcpNode( 0, 4500 );
        }
	}

#ifdef WIN32
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &nValue );
	if( FALSE == bResult )
	{
		OspPrintf( TRUE, TRUE, "[Imt] Wrong profile while reading %s!\n", "Layer" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Slot", 0, &nValue );
	if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[Imt] Wrong profile while reading %s!\n", "Slot" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdSlot = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Type", 0, &nValue );
	if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[Imt] Wrong profile while reading %s!\n", "Type" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdID = (u8)nValue;
    
    memset(achReturn, '\0', sizeof(achReturn));
    bResult = GetRegKeyString( achProfileName, "BoardConfig", "BoardIpAddr", 
                                            achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[Mmp] Wrong profile while reading %s!\n", "BoardIpAddr" );

	}
    m_dwBoardIpAddr = ntohl( inet_addr( achReturn ) );

#else
#ifndef IMT
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "brdcfgdebug.ini");

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &nValue );
	if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[Imt] Wrong profile while reading %s!\n", "Layer" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Slot", 0, &nValue );
	if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[Imt] Wrong profile while reading %s!\n", "Slot" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdSlot = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Type", 0, &nValue );
	if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[Imt] Wrong profile while reading %s!\n", "Type" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdID = (u8)nValue;

    memset(achReturn, '\0', sizeof(achReturn));
    bResult = GetRegKeyString( achProfileName, "BoardConfig", "BoardIpAddr", 
                                            achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[Imt] Wrong profile while reading %s!\n", "BoardIpAddr" );

	}
    m_dwBoardIpAddr = ntohl( INET_ADDR( achReturn ) );

#endif
#endif
    
	return TRUE;
}

/*====================================================================
    函数名      ：ConnectManager
    功能        ：和MPC上的管理程序连接
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：成功返回TRUE，反之返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/20    1.0         jianghy       创建
====================================================================*/
BOOL CBoardAgent::ConnectManager(u32& dwMcuNode, u32 dwMpcIp, u16 wMpcPort)
{
    BOOL bRet = TRUE;
    if( !OspIsValidTcpNode(dwMcuNode) )  // 未建立了连接
    {
        dwMcuNode = OspConnectTcpNode( htonl(dwMpcIp), wMpcPort, 10, 3, 100);
        if( !OspIsValidTcpNode(dwMcuNode) )
        {
            bRet = FALSE;
            brdagtlog("CreateTcpNode with Manager Failed!\n");
        }
        else
        {
            OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());
            OspSetHBParam(dwMcuNode, g_cBrdAgentApp.m_wDiscHeartBeatTime, g_cBrdAgentApp.m_byDiscHeartBeatNum);
            brdagtlog( " connect to mcu success, dwMcuNode:%d, wDiscHeartBeatTime:%d, byDiscHeartBeatNum:%d\n",
                dwMcuNode, g_cBrdAgentApp.m_wDiscHeartBeatTime, g_cBrdAgentApp.m_byDiscHeartBeatNum);
        }
    }
    return bRet;
}

/*=============================================================================
  函 数 名： RegisterToMcuAgent
  功    能： 向Mcu代理注册
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CBoardAgent::RegisterToMcuAgent(u32 dwDstNode)
{
    CServMsg  cReportMsg;
    cReportMsg.SetMsgBody((u8*)&m_tBoardPosition, sizeof(TBrdPosition));
    cReportMsg.CatMsgBody((u8*)&m_dwBoardIpAddr, sizeof(u32));
    cReportMsg.CatMsgBody(&g_cBrdAgentApp.m_byChoice, sizeof(u8));

    // xsl [8/14/2006] 增加版本类型上报
    u8  byOsType = 0;
#if defined WIN32
    byOsType = OS_TYPE_WIN32;
#elif defined _LINUX_
    byOsType = OS_TYPE_LINUX;
#else
    byOsType = OS_TYPE_VXWORKS;
#endif
    cReportMsg.CatMsgBody((u8*)&byOsType, sizeof(u8));

	// 向管理程序注册
    post( MAKEIID(AID_MCU_BRDMGR, CInstance::DAEMON), BOARD_MPC_REG, 
            cReportMsg.GetServMsg(), cReportMsg.GetServMsgLen(), dwDstNode);
    return;
}

/*====================================================================
    函数名      ：ProcBoardPowerOn
    功能        ：单板启动消息
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardPowerOn( CMessage* const pcMsg )
{

    u8 byBrdId = 0;
    u8 byHWVer = 0;

	switch( CurState() ) 
	{
	case STATE_IDLE:
		if(0 != g_cBrdAgentApp.dwMpcIpAddr)
        {
            SetTimer(BRDAGENT_CONNECT_MANAGERA_TIMER, POWEN_ON_CONNECT);
        }
        if( 0 != g_cBrdAgentApp.dwMpcIpAddrB)
        {
            SetTimer(BRDAGENT_CONNECT_MANAGERB_TIMER, POWEN_ON_CONNECT);
        }
     
#ifndef WIN32

        BrdQueryPosition( &m_tBoardPosition ); 

        TBrdEthParam tBrdEthParam;     
        BrdGetEthParam(g_cBrdAgentApp.m_byChoice, &tBrdEthParam);
        m_dwBoardIpAddr = tBrdEthParam.dwIpAdrs;

        byBrdId = BrdGetBoardID();
        byHWVer = BrdQueryHWVersion();
        if((BRD_TYPE_DEC5/*DSL8000_BRD_DEC5*/ == byBrdId) && (byHWVer >= 7))
        {
            g_cBrdAgentApp.m_byTvWallModel = TW_OUTPUTMODE_BOTH;
        }
        else if((BRD_TYPE_DEC5/*DSL8000_BRD_DEC5*/ == byBrdId) && (byHWVer < 7))
        {
            g_cBrdAgentApp.m_byTvWallModel = TW_OUTPUTMODE_VIDEO;
        }
		
		// APU支持单板搜索 [4/19/2013 chendaiwei]
		if( m_tBoardPosition.byBrdID == BRD_TYPE_APU)
		{
			s8 achDeviceName[64] = {0};
			s8 achDeviceType[64] ={0};
			strcpy(achDeviceType,"APU");

			u8 byBrdTypeStrLen = strlen(achDeviceType);
			memcpy(&achDeviceName[0],achDeviceType,byBrdTypeStrLen);	
			sprintf(&achDeviceName[byBrdTypeStrLen],"(%d层%d槽)",m_tBoardPosition.byBrdLayer,m_tBoardPosition.byBrdSlot);
			
			printf("[imtagent] device name:%s\n",achDeviceName);
			
			s8 achConfigFilePath[64]={0};
			//方案1：
			// 	sprintf(achConfigFilePath,"%s/%s",DIR_CONFIG,"detectee.ini");
			//  
			// 	FILE *hFile = fopen(achConfigFilePath,"w+");
			// 	if( hFile == NULL)
			// 	{
			// 	 	printf("[WriteBoardInfoToDetecteeFile]open or create detectee.ini failed!\n");
			// 
			// 		return FALSE;
			// 	}
			// 
			// 	if(fputs(achDeviceType,hFile) == EOF )
			// 	{
			// 		printf("[WriteBoardInfoToDetecteeFile] write Device Type Failed!\n");
			// 
			// 		fclose(hFile);
			// 		
			// 		return FALSE;
			// 	}
			// 
			// 	if(fputs(achDeviceName,hFile) == EOF )
			// 	{
			// 		printf("[WriteBoardInfoToDetecteeFile] write Device name Failed!\n");
			// 		
			// 		fclose(hFile);
			// 		
			// 		return FALSE;
			// 	}
			// 
			// 	fclose(hFile);
			
			//方案二：
			
			sprintf(achConfigFilePath,"%s/%s",DIR_CONFIG,"kdvipdt.ini");
			
			FILE *hFile = fopen(achConfigFilePath,"a+");
			if( hFile == NULL)
			{
				printf("[WriteBoardInfoToDetecteeFile]open or create kdvipdt.ini failed!\n");
				
				return;
			}
			else
			{
				fclose(hFile);
			}
			
			if(!SetRegKeyString(achConfigFilePath,"Dev","TypeAlias",achDeviceType))
			{
				printf("[WriteBoardInfoToDetecteeFile]set type alis failed!\n");
				
				return;
			}
			
			if(!SetRegKeyString(achConfigFilePath,"Dev","Alias",achDeviceName))
			{
				printf("[WriteBoardInfoToDetecteeFile]set device alis failed!\n");
				
				return;
			}
		}//end:方案二

#endif

#ifdef WIN32
        GetBoardCfgInfo();
#endif
		break;

	default:
		OspPrintf(TRUE, FALSE, "[Imt] Wrong state %u when in (ProcBoardPowerOn)!\n", 
			CurState() );
		break;
	}
	return;
}

/*====================================================================
    函数名      ：ProcBoardConnectManagerTimeOut
    功能        ：连接MANAGER超时
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardConnectManagerTimeOut( BOOL32 bIsConnectA )
{
	
    BOOL bRet = FALSE;

	switch( CurState() ) 
	{
	case STATE_IDLE:
    case STATE_INIT:
    case STATE_NORMAL:
		if(TRUE == bIsConnectA)
        {
            bRet = ConnectManager(m_dwDstNode, 
                    g_cBrdAgentApp.dwMpcIpAddr, g_cBrdAgentApp.wMpcPort);       //和MPC上的管理程序连接
            if( TRUE == bRet)
            {
                SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT );
                g_cBrdAgentApp.m_dwDstMcuNode = m_dwDstNode;
            }
            else
            {
                SetTimer( BRDAGENT_CONNECT_MANAGERA_TIMER, CONNECT_MANAGER_TIMEOUT );
            }
        }
		else
        {
            bRet = ConnectManager(m_dwDstNodeB, 
                    g_cBrdAgentApp.dwMpcIpAddrB, g_cBrdAgentApp.wMpcPortB);       //和MPC上的管理程序连接
            if( TRUE == bRet)
            {
                SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT );
                g_cBrdAgentApp.m_dwDstMcuNodeB = m_dwDstNodeB;
            }
            else
            {
                SetTimer( BRDAGENT_CONNECT_MANAGERB_TIMER, CONNECT_MANAGER_TIMEOUT );
            }
        }
        
		if( TRUE == bRet && STATE_IDLE == CurState())
		{
            NEXTSTATE( STATE_INIT );
        }

        break;

	default:
		OspPrintf(TRUE, FALSE, "[Imt] Wrong state %u when in (ProcBoardConnectManagerTimeOut)!\n", 
			CurState() );
		break;
	}
	return;
}

/*====================================================================
    函数名      ：ProcBoardRegisterTimeOut
    功能        ：等待注册超时
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardRegisterTimeOut( BOOL32 bIsConnectA )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
    case STATE_NORMAL:
		if(TRUE == bIsConnectA)
        {
            RegisterToMcuAgent(m_dwDstNode);
            SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT );
        }
        else
        {
            RegisterToMcuAgent(m_dwDstNodeB);
            SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT );
        }      
		break;

	default:
		OspPrintf(TRUE, FALSE, "[Imt] Wrong state %u when in (ProcBoardRegisterTimeOut)!\n", 
			CurState() );
		break;
	}
	return;
}

/*====================================================================
    函数名      ：ProcBoardRegNAck
    功能        ：单板注册消息的否定应答
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardRegNAck( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
    case STATE_NORMAL:

		break;

	default:
		OspPrintf(TRUE, FALSE, "[Imt] Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;

}

/*====================================================================
    函数名      ：ProcBoardRegAck
    功能        ：单板注册消息的应答
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardRegAck( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
    case STATE_NORMAL:
        MsgRegAck(pcMsg);
		break;
        
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;

}

/*=============================================================================
  函 数 名： MsgRegAck
  功    能： 处理注册成功
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CBoardAgent::MsgRegAck(CMessage* const pcMsg )
{
    if(NULL == pcMsg)
    {
        OspPrintf(TRUE, FALSE, "[Imt] the pointer can not be Null\n");
        return;
    }

    if(pcMsg->srcnode == m_dwDstNode)
    {
        m_dwDstIId = pcMsg->srcid;
        KillTimer(BRDAGENT_REGISTERA_TIMER);
    }
    else if(pcMsg->srcnode == m_dwDstNodeB)
    {
        m_dwDstIIdB = pcMsg->srcid;
        KillTimer(BRDAGENT_REGISTERB_TIMER);
    }

    if(STATE_INIT == CurState())
    {
        PostMsgToManager( BOARD_MPC_GET_CONFIG, 
            (u8*)&m_tBoardPosition, sizeof(m_tBoardPosition) );		
        SetTimer( BRDAGENT_GET_CONFIG_TIMER, GET_CONFIG_TIMEOUT );
    }

    return;
}

/*====================================================================
    函数名      ：ProcGetConfigTimeOut
    功能        ：取配置文件定时器超时
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcGetConfigTimeOut( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
		// 重新发送取配置信息消息
		PostMsgToManager( BOARD_MPC_GET_CONFIG, (u8*)&m_tBoardPosition, sizeof(m_tBoardPosition) );		
		SetTimer( BRDAGENT_GET_CONFIG_TIMER, GET_CONFIG_TIMEOUT );	
		break;

	default:
		OspPrintf(TRUE, FALSE, "[Imt] Wrong state %u when in (ProcGetConfigTimeOut)!\n", 
			CurState() );
		break;
	}
	return;
}


/*====================================================================
    函数名      ：ProcBoardGetConfigNAck
    功能        ：单板取配置信息否定应答消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardGetConfigNAck( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
    case STATE_NORMAL:

		break;
		
	default:
		OspPrintf(TRUE, FALSE, "[Imt] Wrong state %u when in (ProcBoardGetConfigNAck)!\n", 
			CurState() );
		break;
	}
	return;
}

/*====================================================================
    函数名      ：ProcBoardGetConfigAck
    功能        ：单板取配置信息应答消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
    04/11/11    3.5         李 博         新接口的修改
====================================================================*/
void CBoardAgent::ProcBoardGetConfigAck( CMessage* const pcMsg )
{
    switch(CurState()) 
    {
    case STATE_INIT:
        MsgGetConfAck(pcMsg);
    	break;

    default:
        OspPrintf(TRUE, FALSE, "[Imt] Wrong state: %d when in (ProcBoardGetConfigAck)\n");
        break;
    }
    
    return;
}

/*=============================================================================
  函 数 名： MsgGetConfAck
  功    能： 处理取配置信息Ack消息
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CBoardAgent::MsgGetConfAck(CMessage* const pcMsg)
{
    u8	byPeriCount;
	u8	byPeriType;
	u16	wLoop, wIndex;
	TEqpMixerEntry*	ptMixerCfg;		//MIXER的配置信息
	TEqpTVWallEntry*	ptTVWallCfg;	//TVWALL的配置信息
	TEqpBasEntry*		ptBasCfg;		//BAS的配置信息
	TEqpVMPEntry*		ptVMPCfg;		//VMP的配置信息
	TEqpPrsEntry*			ptPrsCfg;		//Prs的配置信息
    TEqpMPWEntry*    ptMpwCfg;
    
    switch( CurState() ) 
	{
	case STATE_INIT:
		
		byPeriCount = pcMsg->content[0]; // 外设个数
		wIndex = 1;
		for( wLoop = 0; wLoop < byPeriCount; wLoop++ )
		{			
			byPeriType = pcMsg->content[wIndex++];  // 外设类型
			switch( byPeriType )                    // 设置该单板相应的配置信息
			{
			case EQP_TYPE_TVWALL:
				ptTVWallCfg = (TEqpTVWallEntry*)(pcMsg->content+wIndex);
				wIndex += sizeof(TEqpTVWallEntry);
				g_cBrdAgentApp.SetTVWallConfig(ptTVWallCfg);
				break;
				
			case EQP_TYPE_MIXER:
				ptMixerCfg = (TEqpMixerEntry*)(pcMsg->content+wIndex);
				wIndex += sizeof(TEqpMixerEntry);
				g_cBrdAgentApp.SetMixerConfig(ptMixerCfg);
				break;
				
			case EQP_TYPE_BAS:
				ptBasCfg = (TEqpBasEntry*)(pcMsg->content+wIndex);
				wIndex += sizeof(TEqpBasEntry);
				g_cBrdAgentApp.SetBasConfig(ptBasCfg);
				break;
				
			case EQP_TYPE_VMP:
				ptVMPCfg = (TEqpVMPEntry*)(pcMsg->content+wIndex);
				wIndex += sizeof(TEqpVMPEntry);
				g_cBrdAgentApp.SetVMPConfig(ptVMPCfg);
				break;
                
           case EQP_TYPE_VMPTW:
                ptMpwCfg = (TEqpMPWEntry*)(pcMsg->content+wIndex);
                wIndex += sizeof(TEqpMPWEntry);
                g_cBrdAgentApp.SetMpwConfig(ptMpwCfg);
                break;

			case EQP_TYPE_PRS:
				ptPrsCfg = (TEqpPrsEntry*)(pcMsg->content+wIndex);
                ptPrsCfg->SetMcuIp(g_cBrdAgentApp.dwMpcIpAddr); // Prs 没有取的McuIp
				wIndex += sizeof(TEqpPrsEntry);
				g_cBrdAgentApp.SetPrsConfig(ptPrsCfg);
				break;
			}
		}
		
#ifndef WIN32
#ifdef IMT
		BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );
#endif
#endif
		
		if( pcMsg->content[ wIndex + 6 ] == 0 ) // 是否使用看门狗
		{
#ifndef WIN32
			SysRebootDisable( );
#endif
		}
		else
		{
#ifndef WIN32
			SysRebootEnable( );
#endif
		}

		OspSemGive( g_semImt );

		KillTimer( BRDAGENT_GET_CONFIG_TIMER );
		NEXTSTATE( STATE_NORMAL );
        break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
    return;
}

/*====================================================================
    函数名      ：ProcAlarmSyncReq
    功能        ：MPC发送给单板的告警同步请求
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcAlarmSyncReq( CMessage* const pcMsg )
{
	u16 wLoop;
	u16 wCount=0, wLen;
	u8 abyAlarmBuf[ MAXNUM_BOARD_ALARM*sizeof(TBoardAlarmMsgInfo) + 10 ];
	TBoardAlarmMsgInfo* pAlarmMsgInfo;

	memset(abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	switch( CurState() ) 
	{
    case STATE_INIT:
	case STATE_NORMAL:
		//先是自己的身份信息
		memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );

		//扫描自己的告警表
		pAlarmMsgInfo = (TBoardAlarmMsgInfo*)(abyAlarmBuf + sizeof(m_tBoardPosition) + sizeof(u16));
		for( wLoop=0; wLoop<m_dwBoardAlarmTableMaxNo; wLoop++ )
		{
			if( m_atBoardAlarmTable[wLoop].bExist )
			{
				memcpy(pAlarmMsgInfo, &m_atBoardAlarmTable[wLoop].tBoardAlarm, sizeof(TBoardAlarmMsgInfo) );
				pAlarmMsgInfo++;
				wCount++;
			}
		}

		//告警个数
		*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(wCount); 
		wLen = sizeof(m_tBoardPosition) + sizeof(u16) + wCount*sizeof(TBoardAlarmMsgInfo);

		PostMsgToManager( BOARD_MPC_ALARM_SYNC_ACK, abyAlarmBuf, wLen );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

#ifdef WIN32
//模拟BSP函数
u8 BrdQueryHWVersion()
{
	return 1;
}
u8 BrdQueryFPGAVersion()
{
	return 1;
}

#endif

/*====================================================================
    函数名      ：ProcGetVersionReq
    功能        ：MPC发送给单板的获取版本信息消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcGetVersionReq( CMessage* const pcMsg )
{
	s8 abyBuf[128];
	u16 wLen;

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
		//取版本信息
		memset(abyBuf, 0, sizeof(abyBuf) );

		memcpy(abyBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );

		wLen = sprintf(abyBuf+sizeof(m_tBoardPosition),"Hardware %d| FPGA %d",
			BrdQueryHWVersion(), BrdQueryFPGAVersion() );

		//发送给MANAGER响应
		PostMsgToManager( BOARD_MPC_GET_VERSION_ACK, (u8*)&abyBuf, wLen+sizeof(m_tBoardPosition)+1 );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}


/*====================================================================
    函数名      ：ProcGetModuleInfoReq
    功能        ：MPC发送给单板的取单板的模块信息消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcGetModuleInfoReq( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:		
		//发送给MANAGER响应
		PostMsgToManager( BOARD_MPC_GET_MODULE_ACK, (u8*)&m_tBoardPosition, sizeof(m_tBoardPosition) );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    函数名      ：ProcBoardResetCmd
    功能        ：MPC发送给单板的重启动命令处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::TestingProcess( CMessage* const pcMsg )
{
	if( m_bIsTest == FALSE )
    {
		return ;
    }
	
	m_wLastEvent = pcMsg->event;
	if(pcMsg->content != NULL)
	{
		if( pcMsg->length < sizeof(m_abyLastEventPara) )
        {
			memcpy(m_abyLastEventPara, pcMsg->content, pcMsg->length );
        }
		else
        {
			memcpy(m_abyLastEventPara, pcMsg->content, sizeof(m_abyLastEventPara) );
        }
	}
	else
	{
		memset(m_abyLastEventPara, 0, sizeof(m_abyLastEventPara) );
	} 
}

/*====================================================================
    函数名      ：ProcBoardResetCmd
    功能        ：MPC发送给单板的重启动命令处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardResetCmd( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
#ifndef WIN32        
        OspDelay(2000);		
        BrdHwReset();
#endif
	  //测试用
		TestingProcess( pcMsg );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    函数名      ：MoveFilesToTarget
    功能        ：将FTP目录中的文件根据类型移动到不同的文件夹
    算法实现    ：
    引用全局变量：
    输入参数说明：s8* pbyFileName:待移动的文件名列表
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
/*
void MoveFilesToTarget( s8* pbyFileName )
{
#ifndef WIN32
	DIR *ptDir;
#endif
	BOOL bNext;
	s8 *lpszBgn, *lpszEnd;
	s8 achFileName[256];
	s8 achSrcFullName[256];
	s8 achDstFullName[256];
	
	memset( achFileName, 0, sizeof(achFileName) );
	memset( achSrcFullName, 0, sizeof(achSrcFullName) );
	memset( achDstFullName, 0, sizeof(achDstFullName) );
	
	// move files in sequence
	bNext = TRUE;
	lpszBgn = pbyFileName;
	while( bNext )
	{
		// find the first end 
		lpszEnd = strchr( lpszBgn, (int)CHAR_FTPFILE_SEPARATOR );
		if( lpszEnd == NULL )		// end
		{
			bNext = FALSE;
			lpszEnd = lpszBgn + strlen( lpszBgn ) - 1;
		}
		else
		{
			lpszEnd = lpszEnd - 1;
		}
		
		// find the first type separator
		lpszEnd = (s8*)memchr( lpszBgn, (int)CHAR_FTPFILETYPE_SEPARATOR, lpszEnd - lpszBgn );
		if( lpszEnd == NULL )
			return;
		memcpy( achFileName, lpszBgn, lpszEnd - lpszBgn );
		achFileName[lpszEnd - lpszBgn] = '\0';
		sprintf( achSrcFullName, "%s/%s", DIR_FTP, achFileName );
		
		// analyze the file type
		switch( lpszEnd[1] )
		{
		case CHAR_EXE_TYPE:
		case CHAR_EXE_TYPE + 'a' - 'A':
#ifndef WIN32
			if( ( ptDir = opendir( ( s8* )PATH_BIN ) ) == NULL )
				mkdir( ( s8* )PATH_BIN );
			else
				closedir( ptDir );
#endif
			sprintf( achDstFullName, "%s/%s", PATH_BIN, achFileName );
			break;
		case CHAR_RES_TYPE:
		case CHAR_RES_TYPE + 'a' - 'A':
#ifndef WIN32
			if( ( ptDir = opendir( ( s8* )PATH_RES ) ) == NULL )
				mkdir( ( s8* )PATH_RES );
			else
				closedir( ptDir );
#endif
			sprintf( achDstFullName, "%s/%s", PATH_RES, achFileName );
			break;
		case CHAR_CFG_TYPE:
		case CHAR_CFG_TYPE + 'a' - 'A':
		default:
#ifndef WIN32
			if( ( ptDir = opendir( ( s8* )DIR_CONFIG ) ) == NULL )
				mkdir( ( s8* )DIR_CONFIG );
			else
				closedir( ptDir );
#endif

			sprintf( achDstFullName, "%s/%s", DIR_CONFIG, achFileName );
			break;
		}
		
		// copy the file from ftp directory to specified directory
#ifndef WIN32
		if( mv( (s8*)achSrcFullName, (s8*)achDstFullName ) == ERROR )
		{
			OspPrintf( TRUE, FALSE, "move file from %s to %s fail!\n", achSrcFullName, achDstFullName );
			return;
		}
#endif		
		lpszBgn = lpszEnd + 3;
	}
	return;
}
*/

/*====================================================================
    函数名      ：ProcUpdateSoftwareCmd
    功能        ：MPC发送给单板的软件更新命令处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
	06/04/19    4.0         john          修改升级方式
====================================================================*/
void CBoardAgent::ProcUpdateSoftwareCmd( CMessage* const pcMsg )
{
	if( NULL == pcMsg->content )
	{
		OspPrintf(TRUE, FALSE, "[ProcUpdateSoftwareCmd] Error input parameter .\n");
		return;
	}	

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 byBrdId = *(u8*)cServMsg.GetMsgBody();
    u8 byNum   = *(u8*)(cServMsg.GetMsgBody()+sizeof(u8));
    u8 byTmpNum = byNum;

    u8 abyRet[MAXNUM_FILE_UPDATE];  // 各升级文件对应的成功情况：1-成功，0-失败 (针对会控)
    memset(abyRet, 1, sizeof(abyRet));

    u8 byFileNum = 0;      // 已经升级的文件数
	brdagtlog( "BoardAgent: Rcv Update Software Msg: byBrdId.%d byNum.%d\n", byBrdId, byNum);
	
	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
		if( g_bUpdateSoftBoard )
		{
			s8 achFileName[256];
            s8 achTmpName[256];
            s8 *lpMsgBody = (s8*)(cServMsg.GetMsgBody()+sizeof(u8)+sizeof(u8));
			u8  byLen = 0;
            u16 wOffSet = 0;

			while( byTmpNum-- > 0 )
			{
                byFileNum ++;
                
				memset( achFileName, 0, sizeof(achFileName) );
                byLen = *lpMsgBody;
                if (byLen + wOffSet > cServMsg.GetMsgBodyLen()-2)
                {
                    OspPrintf(TRUE, FALSE, "[ProcUpdateSoftwareCmd] invalid msgbody!!!\n");
                    abyRet[byFileNum] = 0;
                    break;
                }
                memcpy(achFileName, lpMsgBody+sizeof(u8), byLen);
                wOffSet = sizeof(byLen) + byLen;
                lpMsgBody += wOffSet;
                OspPrintf(TRUE, FALSE, "[ProcUpdateSoftwareCmd] filename: %s\n", achFileName);
				
				// 保存上传文件名
				memcpy(achTmpName, achFileName, sizeof(achFileName));

                // 判断是否有此文件名
				FILE *hR = fopen( achTmpName, "r" );
				if( NULL == hR )
				{
                    abyRet[byFileNum] = 0;
					continue;
				}
				else
				{
					fclose( hR );
				}
				
                //vx 下改名
#ifdef _VXWORKS_    
				//去掉后缀
				achFileName[strlen(achFileName) - strlen("upd")] = 0;
				OspPrintf(TRUE, FALSE, "[ProcUpdateSoftwareCmd] updating file name is :%s\n", achFileName);
				
				s8 achOldFile[KDV_MAX_PATH];
				memset(achOldFile, 0, sizeof(achOldFile));
				memcpy(achOldFile, achFileName, strlen(achFileName));
								
				hR = fopen( achOldFile, "r" );
				if( NULL != hR )
				{
					fclose( hR );
					strcat(achOldFile, "old");
					
					s32 nRet = rename( achFileName, achOldFile );
					if( ERROR == nRet )
					{
                        abyRet[byFileNum] = 0;
						brdagtlog( "[ProcUpdateSoftwareCmd] fail to rename file %s to %s.\n", achFileName, achOldFile);
					}
					else// 新文件改名
					{
						nRet = rename(achTmpName, achFileName);
						if( ERROR == nRet )
						{
                            abyRet[byFileNum] = 0;
							brdagtlog( "[ProcUpdateSoftwareCmd] fail to rename file %s to %s.\n", achTmpName, achFileName);						
						}
						
						remove( achOldFile );
					}
				}
				else
				{
					// 新文件改名
					rename(achTmpName, achFileName);
				}
#elif defined _LINUX_
                //调用底层接口appupdate...
                //判断是否image文件
                s32 nNameOff = strlen(achFileName) - strlen(LINUXAPP_POSTFIX);
                if (nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, LINUXAPP_POSTFIX) )
                {
                    if ( OK != BrdUpdateAppImage(achFileName) )
                    {
                        abyRet[byFileNum] = 0;
                        brdagtlog( "[ProcUpdateBrdVersion] BrdUpdateAppImage failed.\n");
                    }
                    else
                    {
                        brdagtlog( "[ProcUpdateBrdVersion] BrdUpdateAppImage success.\n");
                    }
                }
                else
                {
                    //判断是否os
                    nNameOff = strlen(achFileName) - strlen(LINUXOS_POSTFIX);
                    if (nNameOff > 0 && 0 == strcmp(achFileName+nNameOff, LINUXOS_POSTFIX) )
                    {
                        if ( OK != BrdFpUpdateAuxData(achFileName) )
                        {
                            abyRet[byFileNum] = 0;
                            brdagtlog( "[ProcUpdateBrdVersion] BrdFpUpdateAuxData failed.\n");
                        }
                        else
                        {
                            brdagtlog( "[ProcUpdateBrdVersion] BrdFpUpdateAuxData success.\n");
                        }
                    }   
                    else
                    {
                        brdagtlog( "[ProcUpdateBrdVersion] not supported file type.\n");
                    }
                }
#endif
			}
		}

        // 本代理对升级传入的消息作全部的返回，统一由MCU代理处理
        {
            u8 * pbyMsg   = cServMsg.GetMsgBody() + sizeof(u8) * 2;
            u8   byMsgLen = cServMsg.GetMsgBodyLen() - 2;
            
            CServMsg cServ(pcMsg->content, pcMsg->length);
            cServ.SetMsgBody(&byBrdId,sizeof(u8));
            cServ.CatMsgBody(&byNum,  sizeof(u8));
            cServ.CatMsgBody(abyRet, byNum);
            cServ.CatMsgBody(pbyMsg, byMsgLen);
            
            PostMsgToManager( BOARD_MPC_UPDATE_NOTIFY, cServ.GetServMsg(), cServ.GetServMsgLen());
        }
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			 pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    函数名      ：ProcTimeSyncCmd
    功能        ：MPC发送给单板的时间同步命令处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcTimeSyncCmd( CMessage* const pcMsg )
{
    s8 achTemp[50];
#ifdef WIN32 
    SYSTEMTIME  systemTime;
#else
    struct tm tmTime;
#endif

    switch( CurState() ) 
    {
    case STATE_INIT:
    case STATE_NORMAL:		
    #ifndef WIN32		
        achTemp[4] = '\0';
        memcpy( achTemp, pcMsg->content, 4 );
        tmTime.tm_year = atoi( achTemp ) - 1900;
        achTemp[2] = '\0';
        memcpy( achTemp, pcMsg->content + 4, 2 );
        tmTime.tm_mon = atoi( achTemp ) - 1;
        memcpy( achTemp, pcMsg->content + 6, 2 );
        tmTime.tm_mday = atoi( achTemp );
        memcpy( achTemp, pcMsg->content + 8, 2 );
        tmTime.tm_hour = atoi( achTemp );
        memcpy( achTemp, pcMsg->content + 10, 2 );
        tmTime.tm_min = atoi( achTemp );
        memcpy( achTemp, pcMsg->content + 12, 2 );
        tmTime.tm_sec = atoi( achTemp );

        /* set clock chip time */
        if( BrdTimeSet( &tmTime ) == ERROR )
        {
            printf( "BrdTimeSet failed!\n" );
            return;
        }
    #else
        achTemp[4] = '\0';
        memcpy( achTemp, pcMsg->content, 4 );
        systemTime.wYear = atoi( achTemp );
        achTemp[2] = '\0';
        memcpy( achTemp, pcMsg->content + 4, 2 );
        systemTime.wMonth = atoi( achTemp );
        memcpy( achTemp, pcMsg->content + 6, 2 );
        systemTime.wDay = atoi( achTemp );
        memcpy( achTemp, pcMsg->content + 8, 2 );
        systemTime.wHour = atoi( achTemp );
        memcpy( achTemp, pcMsg->content + 10, 2 );
        systemTime.wMinute = atoi( achTemp );
        memcpy( achTemp, pcMsg->content + 12, 2 );
        systemTime.wSecond = atoi( achTemp );	 

        SetLocalTime( &systemTime );	  
    #endif
        //测试用
        TestingProcess( pcMsg );
        break;

    default:
        OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;
    }
    return;
}

/*====================================================================
    函数名      ：ProcBoardSelfTestCmd
    功能        ：MPC发送给单板的自测命令处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardSelfTestCmd( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_NORMAL:
		//测试用
		TestingProcess( pcMsg );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    函数名      ：ProcBitErrorTestCmd
    功能        ：MPC发送给单板的误码测试命令处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBitErrorTestCmd( CMessage* const pcMsg )
{
	switch( CurState() ) 
	{
	case STATE_NORMAL:
		//测试用
		TestingProcess( pcMsg );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    函数名      ：ProcGetStatisticsReq
    功能        ：MPC发送给单板的取单板的统计信息消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcGetStatisticsReq( CMessage* const pcMsg )
{
	switch( CurState() ) 
	{
	case STATE_NORMAL:
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}


/*====================================================================
    函数名      ：ProcLedStatusReq
    功能        ：MPC发送给单板的取单板的面板灯状态消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建

    04/11/11    3.5         李 博         新接口的修改
====================================================================*/
void CBoardAgent::ProcLedStatusReq( CMessage* const pcMsg )
{
#ifndef WIN32
#ifdef IMT
	u8 abyBuf[32];
	u8  byCount;
    TBrdLedState tBrdLedState;

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
		//BrdIMTQueryLedState( &m_tLedState );
        BrdQueryLedState( &tBrdLedState );
        memcpy(&m_tLedState, &tBrdLedState.nlunion.tBrdIMTLedState, sizeof(TBrdIMTLedStateDesc));
		
		memcpy(abyBuf, &m_tBoardPosition, sizeof(m_tBoardPosition) );
		byCount = sizeof(m_tLedState);
		memcpy(abyBuf+sizeof(m_tBoardPosition), &byCount, sizeof(byCount) ); 
		memcpy(abyBuf+sizeof(m_tBoardPosition)+sizeof(byCount), &m_tLedState, sizeof(m_tLedState));
		
		PostMsgToManager( BOARD_MPC_LED_STATUS_ACK, abyBuf, 
			sizeof(m_tBoardPosition)+sizeof(u8)+sizeof(m_tLedState) );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
#endif
#endif
	return;
}


/*====================================================================
    函数名      ：PostMsgToManager
    功能        ：发送消息给MPC的单板管理程序
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wEvent: 消息号
				  u8 * const pbyContent: 消息内容 
				  u16 wLen: 消息内容长度
    返回值说明  ：成功返回TRUE，反之返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CBoardAgent::PostMsgToManager( u16 wEvent, u8 * const pbyContent, u16 wLen )
{
	if( OspIsValidTcpNode( m_dwDstNode ) )
	{
		post( m_dwDstIId, wEvent, pbyContent, wLen, m_dwDstNode );
		brdagtlog(  "BoardAgent: Post message %u(%s) to BoardManager. [%d,%d,%d]!\n", wEvent, 
			    ::OspEventDesc( wEvent ), m_tBoardPosition.byBrdID, 
                m_tBoardPosition.byBrdLayer, m_tBoardPosition.byBrdSlot );
	}
	else
	{
		brdagtlog( "[Imt] the A's node is invalide\n");
        brdagtlog( "BoardAgent: Board [%d,%d,%d] is offline now: Node=%u\n", 
			        m_tBoardPosition.byBrdID, m_tBoardPosition.byBrdLayer, 
                    m_tBoardPosition.byBrdSlot, m_dwDstNode);
	}
    if(OspIsValidTcpNode(m_dwDstNodeB))
    {
        post(m_dwDstIIdB, wEvent, pbyContent, wLen, m_dwDstNodeB);
        brdagtlog(  "BoardAgent: Post message %u(%s) to BoardManager. [%d,%d,%d]!\n", wEvent, 
			    ::OspEventDesc( wEvent ), m_tBoardPosition.byBrdID, 
                m_tBoardPosition.byBrdLayer, m_tBoardPosition.byBrdSlot );
    }
    else
    {
        brdagtlog( "[Imt] the B's node is invalide\n");
        brdagtlog( "BoardAgent: Board [%d,%d,%d] is offline now: Node=%u\n", 
			        m_tBoardPosition.byBrdID, m_tBoardPosition.byBrdLayer, 
                    m_tBoardPosition.byBrdSlot, m_dwDstNodeB);
    }
    return TRUE;
}



/*====================================================================
    函数名      ：ProcOspDisconnect
    功能        ：OSP和单板断开连接
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建

    04/11/11    3.5         李 博         新接口的修改
====================================================================*/
void CBoardAgent::ProcOspDisconnect( CMessage* const pcMsg )
{
   
    MsgDisconnectInfo(pcMsg);

    return;
    
}

/*=============================================================================
  函 数 名： MsgDisconnectInfo
  功    能： 处理断链消息
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CBoardAgent::MsgDisconnectInfo(CMessage* const pcMsg)
{
    if(NULL == pcMsg)
    {
        OspPrintf(TRUE, FALSE, "[Imt] the pointer can not be Null\n");
        return;
    }

    u32 dwNode = *(u32*)pcMsg->content;
    if ( INVALID_NODE != dwNode )
    {
        OspDisconnectTcpNode(dwNode);
    }
    
    if(dwNode == m_dwDstNode)
    {
        FreeDataA();
        g_cBrdAgentApp.m_dwDstMcuNode = INVALID_NODE;
        SetTimer( BRDAGENT_CONNECT_MANAGERA_TIMER, CONNECT_MANAGER_TIMEOUT );
    }
    else if(dwNode == m_dwDstNodeB)
    {
        FreeDataB();
        g_cBrdAgentApp.m_dwDstMcuNodeB = INVALID_NODE;
        SetTimer( BRDAGENT_CONNECT_MANAGERB_TIMER, CONNECT_MANAGER_TIMEOUT );
    }

    if( INVALID_NODE == m_dwDstNode && INVALID_NODE == m_dwDstNodeB )
    {
        switch( CurState() ) 
	    {
	    case STATE_INIT:
	    case STATE_NORMAL:
    #ifndef WIN32
    #ifdef IMT
		    BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF );
    #endif
    #endif
            OspDelay(5*1000);
	        NEXTSTATE( STATE_IDLE );
		    break;
		
	    default:
		    OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			        pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		    break;
	    }
		
    }

    return;
}

/*====================================================================
    函数名      ：ProcBoardMemeryStatus
    功能        ：内存状态改变消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardMemeryStatus( CMessage* const pcMsg )
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	if( FindAlarm( ALARM_MCU_MEMORYERROR, ALARMOBJ_MCU_SOFTWARE, byAlarmObj, &tAlarmData ) )
	{
		if( *( pcMsg->content ) == 0 )      //normal
		{
			if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
			{
				brdagtlog( "BoardAgent: DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
				tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0], 
				tAlarmData.byBoardAlarmObject[1] );
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );
			
				memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
				memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( *( pcMsg->content ) == 1 )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_MEMORYERROR, ALARMOBJ_MCU_SOFTWARE, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog( "BoardAgent: AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg);

				memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
				memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	return;
}


/*====================================================================
    函数名      ：ProcBoardFileSystemStatus
    功能        ：文件系统状态改变消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardModuleStatus( CMessage* const pcMsg )
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	if( FindAlarm( ALARM_MCU_MODULE_OFFLINE, ALARMOBJ_MCU_MODULE, byAlarmObj, &tAlarmData ) )
	{
		if( *( pcMsg->content ) == 0 )      //normal
		{
			if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
			{
				brdagtlog("BoardAgent: DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
				tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0], 
				tAlarmData.byBoardAlarmObject[1] );
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );
			
				memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
				memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( *( pcMsg->content ) == 1 )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_MODULE_OFFLINE, ALARMOBJ_MCU_MODULE, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog( "BoardAgent: AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg );
			
				memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
				memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	return;
}

/*====================================================================
    函数名      ：ProcImtFanStatus
    功能        ：MAP风扇状态改变消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcImtFanStatus( CMessage* const pcMsg )
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );
	byAlarmObj[0] = pcMsg->content[0];
	byAlarmObj[1] = 0;

	if( FindAlarm( ALARM_MCU_BOARD_FAN_STOP, ALARMOBJ_MCU_BRD_FAN, byAlarmObj, &tAlarmData ) )
	{
		if(  pcMsg->content[1] == 0 )      //normal
		{
			if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
			{
				brdagtlog( "BoardAgent: DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
				tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0], 
				tAlarmData.byBoardAlarmObject[1] );
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] =  pcMsg->content[0];
				tAlarmMsg.abyAlarmContent[4] =  pcMsg->content[1];
			
				memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
				memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( pcMsg->content[1] == 1 )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_BOARD_FAN_STOP, ALARMOBJ_MCU_BRD_FAN, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog("BoardAgent: AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] =  pcMsg->content[0];
				tAlarmMsg.abyAlarmContent[4] =  pcMsg->content[1];

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg );
			
				memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
				memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
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
#ifdef IMT
	u8 abyBuf[32];
	u8  byCount;
	TBrdIMTLedStateDesc* ptLedState;
	ptLedState = (TBrdIMTLedStateDesc*)pcMsg->content;

	if( memcmp(&m_tLedState, ptLedState, sizeof(*ptLedState)) == 0 )
    {
		return; 
    }

	memcpy(abyBuf, &m_tBoardPosition, sizeof(m_tBoardPosition) );
	byCount = sizeof(*ptLedState);
	memcpy(abyBuf+sizeof(m_tBoardPosition), &byCount, sizeof(byCount) ); 
	memcpy(abyBuf+sizeof(m_tBoardPosition)+sizeof(byCount), ptLedState, sizeof(*ptLedState));
					
	PostMsgToManager( BOARD_MPC_LED_STATUS_NOTIFY, abyBuf, 
		sizeof(m_tBoardPosition)+sizeof(u8)+sizeof(*ptLedState) );
#endif
#endif
}

/*====================================================================
    函数名      ：ProcBoardFileSystemStatus
    功能        ：文件系统状态改变消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardFileSystemStatus( CMessage* const pcMsg )
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	if( FindAlarm( ALARM_MCU_FILESYSTEMERROR, ALARMOBJ_MCU_SOFTWARE, byAlarmObj, &tAlarmData ) )
	{
		if( *( pcMsg->content ) == 0 )      //normal
		{
			if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
			{
				brdagtlog( "BoardAgent: DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
				tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0], 
				tAlarmData.byBoardAlarmObject[1] );
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );
			
				memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
				memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( *( pcMsg->content ) == 1 )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_FILESYSTEMERROR, ALARMOBJ_MCU_SOFTWARE, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog( "BoardAgent: AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg );
			
				memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
				memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	return;
}


/*====================================================================
    函数名      ：AddAlarm
    功能        ：向告警表中增加告警
    算法实现    ：
    引用全局变量：
    输入参数说明：DWORD dwAlarmCode, 告警码
                  u8 byObjType, 告警对象类型
                  u8 byObject[], 告警对象
                  TBoardAlarm *ptData, 返回的告警信息
    返回值说明  ：成功返回TRUE，否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CBoardAgent::AddAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData )
{
    u16    wLoop;

	/*找到一个空表项*/
    for( wLoop = 0; wLoop < m_dwBoardAlarmTableMaxNo; wLoop++ )
    {
        if( !m_atBoardAlarmTable[wLoop].bExist )
            break;
    }
    
    if( wLoop >= sizeof( m_atBoardAlarmTable ) / sizeof( m_atBoardAlarmTable[0] ) )
    {
        return( FALSE );
    }

	/*填写该告警表项*/
    m_atBoardAlarmTable[wLoop].dwBoardAlarmCode = dwAlarmCode;
    m_atBoardAlarmTable[wLoop].byBoardAlarmObjType = byObjType;
    memcpy( m_atBoardAlarmTable[wLoop].byBoardAlarmObject, byObject, 
        sizeof( m_atBoardAlarmTable[0].byBoardAlarmObject ) );
    m_atBoardAlarmTable[wLoop].bExist = TRUE;
	
    // copy
    ptData->dwBoardAlarmSerialNo = wLoop + 1;
    ptData->dwBoardAlarmCode = m_atBoardAlarmTable[wLoop].dwBoardAlarmCode;
    ptData->byBoardAlarmObjType = m_atBoardAlarmTable[wLoop].byBoardAlarmObjType;
    memcpy( ptData->byBoardAlarmObject, m_atBoardAlarmTable[wLoop].byBoardAlarmObject, 
        sizeof( ptData->byBoardAlarmObject ) );
	
    // increase maxno
    if( wLoop == m_dwBoardAlarmTableMaxNo )
        m_dwBoardAlarmTableMaxNo++;

     return( TRUE );
}

/*====================================================================
    函数名      ：SetAlarmMsgInfo
    功能        ：设置引起该告警的消息
    算法实现    ：
    引用全局变量：
    输入参数说明：DWORD dwSerialNo: 告警的序列号
	              CMessage* const pcMsg: 引起该告警的消息
    返回值说明  ：成功返回TRUE，否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CBoardAgent::SetAlarmMsgInfo( u32 dwSerialNo, TBoardAlarmMsgInfo* const ptMsg )
{
	m_atBoardAlarmTable[dwSerialNo - 1].tBoardAlarm = *ptMsg;
	return ( TRUE );
}

/*====================================================================
    函数名      ：DeleteAlarm
    功能        ：从告警表中删除告警
    算法实现    ：
    引用全局变量：
    输入参数说明：DWORD dwSerialNo: 告警的序列号
    返回值说明  ：成功返回TRUE，否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CBoardAgent::DeleteAlarm( u32 dwSerialNo )
{	
    u32   dwLoop;

    m_atBoardAlarmTable[dwSerialNo - 1].bExist = FALSE;
        
    if( dwSerialNo == m_dwBoardAlarmTableMaxNo )     //the last one
    {
        for( dwLoop = m_dwBoardAlarmTableMaxNo - 1; dwLoop >= 0; dwLoop-- )
        {
            if( m_atBoardAlarmTable[dwLoop].bExist )
                break;
        }
        m_dwBoardAlarmTableMaxNo = dwLoop + 1;  //reset MaxNo
    }

    return( TRUE );
}

/*====================================================================
    函数名      ：FindAlarm
    功能        ：从告警表查找告警
    算法实现    ：
    引用全局变量：
    输入参数说明：DWORD dwAlarmCode:  告警码
				  u8 byObjType: 告警对象类型
				  u8 byObject[]: 告警对象
				  TBoardAlarm *ptData: 返回的告警信息
    返回值说明  ：成功返回TRUE，否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
BOOL CBoardAgent::FindAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData )
{
    u16    wLoop;
    BOOL    bFind = FALSE;

	/*查找告警表中的每一项*/
    for( wLoop = 0; wLoop < m_dwBoardAlarmTableMaxNo; wLoop++ )
    {
		/*该项不存在，继续找下一项*/
        if( !m_atBoardAlarmTable[wLoop].bExist )
            continue;

		/*告警码和告警对象类型要一致*/
        if( dwAlarmCode == m_atBoardAlarmTable[wLoop].dwBoardAlarmCode 
            && byObjType == m_atBoardAlarmTable[wLoop].byBoardAlarmObjType )
        {
			/*告警对象要一致*/
            switch( byObjType )
            {
			case ALARMOBJ_MCU_SOFTWARE:
			case ALARMOBJ_MCU_MODULE:
				bFind = TRUE;
				break;

            case ALARMOBJ_MCU_LINK:
                if( byObject[0] == m_atBoardAlarmTable[wLoop].byBoardAlarmObject[0] 
                    && byObject[1] == m_atBoardAlarmTable[wLoop].byBoardAlarmObject[1] )
                {
                    bFind = TRUE;
                }
                break;

			case ALARMOBJ_MCU_BRD_FAN:
                if( byObject[0] == m_atBoardAlarmTable[wLoop].byBoardAlarmObject[0] )
				{
					bFind = TRUE;
				}
				break;

            default:
#ifdef _DEBUG
                brdagtlog( "BoardAgent: Wrong AlarmObjType in FindAlarm()!\n" );
#endif
                break;
            }  //end switch
        
            if( bFind ) 
				break;
        }   //end if
    }

    if( bFind )
    {
        //copy
		ptData->dwBoardAlarmSerialNo = wLoop+1;
        ptData->dwBoardAlarmCode = m_atBoardAlarmTable[wLoop].dwBoardAlarmCode;
        ptData->byBoardAlarmObjType = m_atBoardAlarmTable[wLoop].byBoardAlarmObjType;
        memcpy( ptData->byBoardAlarmObject, m_atBoardAlarmTable[wLoop].byBoardAlarmObject, 
            sizeof( ptData->byBoardAlarmObject ) );
    }

    return( bFind );
}

/*====================================================================
    函数名      ：ProcBoardGetCfgTest
    功能        ：测试时取配置的消息
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage* const pcMsg 
    返回值说明  ：成功返回TRUE，否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardGetCfgTest( CMessage* const pcMsg )
{
/*	STRUCT_mcueqpTVWallEntry_mem tTVWallCfg;
	STRUCT_mcueqpMixerEntry_mem		tMixerCfg;
	STRUCT_mcueqpBasEntry_mem		tBasCfg;
	STRUCT_mcueqpVMPEntry_mem		tVMPCfg;

	switch(  g_cBrdAgentApp.byPeriType )
	{
	case EQP_TYPE_TVWALL:
		tTVWallCfg = g_cBrdAgentApp.m_tTVWallCfg;

		tTVWallCfg.mcueqpTVWallEntIpAddr = htonl(tTVWallCfg.mcueqpTVWallEntIpAddr);
		tTVWallCfg.mcueqpTVWallEntConnMcuIp  = htonl(tTVWallCfg.mcueqpTVWallEntConnMcuIp);
		tTVWallCfg.mcueqpTVWallEntConnMcuPort = htons(tTVWallCfg.mcueqpTVWallEntConnMcuPort);
		tTVWallCfg.mcueqpTVWallEntVideoStartRecvPort = htons(tTVWallCfg.mcueqpTVWallEntVideoStartRecvPort);

		SetSyncAck(&tTVWallCfg, sizeof(tTVWallCfg));
		break;

	case EQP_TYPE_MIXER:
		tMixerCfg = g_cBrdAgentApp.m_tMixerCfg;
		tMixerCfg.mcueqpMixerEntPort = htons(tMixerCfg.mcueqpMixerEntPort);
		tMixerCfg.mcueqpMixerEntIpAddr = htonl(tMixerCfg.mcueqpMixerEntIpAddr);
		tMixerCfg.mcueqpMixerEntMixStartPort = htons(tMixerCfg.mcueqpMixerEntMixStartPort);
		tMixerCfg.mcueqpMixerEntConnMcuPort = htons(tMixerCfg.mcueqpMixerEntConnMcuPort);
		tMixerCfg.mcueqpMixerEntConnMcuIp = htonl(tMixerCfg.mcueqpMixerEntConnMcuIp);
		SetSyncAck(&tMixerCfg, sizeof(tMixerCfg));

		break;

	case EQP_TYPE_VMP:
		tVMPCfg = g_cBrdAgentApp.m_tVMPCfg;
		tVMPCfg.mcueqpVMPEntPort = htons(tVMPCfg.mcueqpVMPEntPort);
		tVMPCfg.mcueqpVMPEntIpAddr = htonl(tVMPCfg.mcueqpVMPEntIpAddr);
		tVMPCfg.mcueqpVMPEntRecvStartPort = htons(tVMPCfg.mcueqpVMPEntRecvStartPort);
		tVMPCfg.mcueqpVMPEntConnMcuPort = htons(tVMPCfg.mcueqpVMPEntConnMcuPort);
		tVMPCfg.mcueqpVMPEntConnMcuIp = htonl(tVMPCfg.mcueqpVMPEntConnMcuIp);
		SetSyncAck(&tVMPCfg, sizeof(tVMPCfg));

		break;

	case EQP_TYPE_BAS:
		tBasCfg = g_cBrdAgentApp.m_tBasCfg;
		tBasCfg.mcueqpBasEntPort = htons(tBasCfg.mcueqpBasEntPort);
		tBasCfg.mcueqpBasEntIpAddr = htonl(tBasCfg.mcueqpBasEntIpAddr);
		tBasCfg.mcueqpBasEntRecvStartPort = htons(tBasCfg.mcueqpBasEntRecvStartPort);
		tBasCfg.mcueqpBasEntConnMcuPort = htons(tBasCfg.mcueqpBasEntConnMcuPort);
		tBasCfg.mcueqpBasEntConnMcuIp = htonl(tBasCfg.mcueqpBasEntConnMcuIp);
		SetSyncAck(&tBasCfg, sizeof(tBasCfg));
		break;

	default:
		break;
	}*/
	return ;
}
/*====================================================================
    函数名      ：ProcBoardGetAlarm
    功能        ：测试时取告警信息
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage* const pcMsg 
    返回值说明  ：成功返回TRUE，否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardGetAlarm( CMessage* const pcMsg )
{
	u16 wLoop;
	u16 wCount=0;
	u8 abyAlarmBuf[ 1024];
	u8* pbyBuf;

	//扫描自己的告警表
	pbyBuf = abyAlarmBuf + sizeof(u16);
	for( wLoop=0; wLoop<m_dwBoardAlarmTableMaxNo; wLoop++ )
	{
		if( m_atBoardAlarmTable[wLoop].bExist )
		{
			*(u32*)pbyBuf =  htonl( m_atBoardAlarmTable[wLoop].dwBoardAlarmCode );
			pbyBuf += sizeof(m_atBoardAlarmTable[wLoop].dwBoardAlarmCode);
			*pbyBuf = m_atBoardAlarmTable[wLoop].byBoardAlarmObjType;
			pbyBuf ++;
			*pbyBuf = m_tBoardPosition.byBrdLayer;
			pbyBuf++;
			*pbyBuf = m_tBoardPosition.byBrdSlot;
			pbyBuf++;
			*pbyBuf = m_tBoardPosition.byBrdID;
			pbyBuf++;
			memcpy( pbyBuf, m_atBoardAlarmTable[wLoop].byBoardAlarmObject, 2 );
			pbyBuf += 2;

			wCount++;
		}
	}
	
	//告警个数
	*(u16*)abyAlarmBuf = htons(wCount); 

	SetSyncAck( abyAlarmBuf, pbyBuf - abyAlarmBuf );
	return ;

}

/*====================================================================
    函数名      ：ProcBoardGetAlarm
    功能        ：测试时取告警信息
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage* const pcMsg 
    返回值说明  ：成功返回TRUE，否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CBoardAgent::ProcBoardGetLastManagerCmd( CMessage* const pcMsg )
{
	u8 abyBuf[128];

	//直接返回最后一条消息的相关信息
	*(u16*)abyBuf = htons(m_wLastEvent);
	memcpy(abyBuf+2, m_abyLastEventPara, sizeof(m_abyLastEventPara) );
	SetSyncAck( abyBuf, sizeof(u16)+sizeof(m_abyLastEventPara) );
	return;
}

API u32 BrdGetDstMcuNode( void )
{
    return g_cBrdAgentApp.m_dwDstMcuNode;
}

API u32 BrdGetDstMcuNodeB(void)
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}

API u8 BrdGetTvwallModel(void)
{
    return g_cBrdAgentApp.m_byTvWallModel;
}

API void noupdatesoft( void )
{
	g_bUpdateSoftBoard = FALSE;
}

API void updatesoft( void )
{
	g_bUpdateSoftBoard = TRUE;
}
