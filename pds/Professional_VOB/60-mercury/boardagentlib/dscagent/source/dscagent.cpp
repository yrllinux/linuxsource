/*****************************************************************************
   模块名      : Board Agent
   文件名      : criagent.cpp
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
#include "dscagent.h"

#ifdef _VXWORKS_
#include "brddrvlib.h"
#endif

#define DISCONNECT_MCUA    (u16)1
#define DISCONNECT_MCUB    (u16)2

#define UPDATE_FILEBUF_SIZE	 (u32)(100*1024)

CBrdAgentApp	g_cBrdAgentApp;	//单板代理应用实例

extern SEMHANDLE g_semDSC;
BOOL32 g_bUpdateSoftBoard = TRUE;
BOOL32 g_bPrintDscLog = FALSE;

void Dsclog(char * fmt, ...)
{
    char PrintBuf[255];
    memset(PrintBuf, 0, sizeof(PrintBuf));
    int BufLen = 0;
    va_list argptr;
	if (g_bPrintDscLog)
	{
		BufLen = sprintf(PrintBuf, "[Dsc]:");
		va_start(argptr, fmt);
		BufLen += vsprintf(PrintBuf+BufLen, fmt, argptr);
		va_end(argptr);
		BufLen += sprintf(PrintBuf+BufLen, "\n");
		OspPrintf(TRUE, FALSE, PrintBuf);
	}
    return;
}

//构造函数
CBoardAgent::CBoardAgent()
{
	memset(&m_tBoardPosition, 0, sizeof(m_tBoardPosition) );
	m_bIsTest = FALSE;
    FreeDataA();
    FreeDataB();

	memset(m_achCurUpdateFileName, 0, sizeof(m_achCurUpdateFileName));
	memset(m_abyUpdateResult, 0, sizeof(m_abyUpdateResult));
	m_byUpdateFileNum = 0;
	m_byWaitTimes = 0;
	m_byErrTimes = 0;
	m_byBrdIdx = 0;
	m_byMcsSsnIdx = 0;
	m_hUpdateFile = NULL;
	m_pbyFileBuf = NULL;
	m_dwBufUsedSize = 0;
	return;
}

// 析构函数
CBoardAgent::~CBoardAgent()
{
	m_hUpdateFile = NULL;
	m_pbyFileBuf = NULL;
    return;
}

void CBoardAgent::FreeDataA(void)
{
	m_dwDstNode = INVALID_NODE;
	m_dwDstIId = INVALID_INS;
    return;
}

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
    return;
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
	Dsclog( "Msg %u(%s) received!\n", pcMsg->event, ::OspEventDesc( pcMsg->event ) );

	if( NULL == pcMsg )
	{
		OspPrintf(TRUE, FALSE, "BoardAgent: The received msg's pointer in the msg entry is NULL!");
		return;
	}

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

    case BRDAGENT_GET_CONFIG_TIMER:     //等待配置应答消息超时
        ProcGetConfigTimeOut( pcMsg );
		break;

    case MPC_BOARD_SETDSCINFO_REQ:          // 新配置信息通知
        ProcSetDscInfoReq(pcMsg);
        break;

	case MPC_BOARD_DSCGKINFO_UPDATE_CMD:	// DSC的GK info更新请求, zgc, 2007-07-21
		ProcBoardGkInfoUpdateCmd(pcMsg);
		break;

	case MCU_BOARD_DSCTELNETLOGININFO_UPDATE_CMD:	// DSC的更新LoginInfo命令, zgc, 2007-10-12
		ProcBoardLoginInfoUpdateCmd(pcMsg);
		break;

	case BOARD_MODULE_STATUS:
        ProcBoardModuleStatus(pcMsg);
        break;
        
    case BOARD_LED_STATUS:
        ProcBoardLedStatus(pcMsg);
        break;

    case MPC_BOARD_GET_MODULE_REQ:      //获取单板的模块信息
		ProcGetModuleInfoReq( pcMsg );
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
		ProcRebootCmd( pcMsg );
		break;

	case MPC_BOARD_UPDATE_SOFTWARE_CMD:  //软件更新命令
		ProcUpdateSoftwareCmd( pcMsg );
		break;

	case MPC_DSC_STARTUPDATE_SOFTWARE_REQ:	// 新软件更新命令，zgc, 2007-08-20
		ProcStartDscUpdateSoftwareReq( pcMsg );
		break;

	case MPC_DSC_UPDATEFILE_REQ:			// 接收处理升级文件数据包, zgc, 2007-08-25
		ProcUpdateFileReq(pcMsg);
		break;

	case DSC_UPDATE_SOFTWARE_WAITTIMER:
		ProcUpdateSoftwareWaittimer(pcMsg);
		break;

	case OSP_DISCONNECT:
		ProcOspDisconnect( pcMsg );
		break;

    case BOARD_MPC_ALARM_SYNC_ACK:    
    case BOARD_MPC_LED_STATUS_ACK:
    case BOARD_MPC_GET_VERSION_ACK:
        break;

	default:
        log(LOGLVL_EXCEPTION, "BoardAgent: receive unknown msg %d<%s> in DscAgent InstanceEntry! \n",
                               pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
	
	return;
}

//模拟单板BSP上的函数 测试用
//#ifdef WIN32
//void BrdQueryPosition(TBrdPosition* ptPosition)
//{
//	/*已经从配置文件读出*/
// 	return ;
//}
//#endif

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
			OspCreateTcpNode( 0, 4400 );
        }
	}

#ifdef WIN32
	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "Layer" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdLayer = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Slot", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "Slot" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdSlot = (u8)nValue;

	bResult = GetRegKeyInt( achProfileName, "BoardConfig", "Type", 0, &nValue );
	if( bResult == FALSE )  
	{
		OspPrintf( TRUE, TRUE, "[Agent] Wrong profile while reading %s!\n", "Type" );
		return( FALSE );
	}
	m_tBoardPosition.byBrdID = (u8)nValue;

    memset(achReturn, '\0', sizeof(achReturn));
    bResult = GetRegKeyString( achProfileName, "BoardConfig", "BoardIpAddr", 
                                            achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( FALSE == bResult )  
	{
		OspPrintf( TRUE, TRUE, "[Dri] Wrong profile while reading %s!\n", "MpcIpAddrB" );

	}
    m_dwBoardIpAddr = ntohl( inet_addr( achReturn ) );
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
            OspPrintf( TRUE, FALSE, "[Dsc] CreateTcpNode with Manager Failed: dwNode<%d>, MpcIp<0x%x@%d>!\n",
                                     dwMcuNode, dwMpcIp, wMpcPort );
        }
        else
        {
            OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());
            OspSetHBParam(dwMcuNode, g_cBrdAgentApp.m_wDiscHeartBeatTime, g_cBrdAgentApp.m_byDiscHeartBeatNum);
            
            g_cBrdAgentApp.m_dwLocalInnerIp = OspNodeLocalIpGet( dwMcuNode );   // 网络序

            OspPrintf(TRUE, FALSE, "[Dsc] connect to mcu success, dwMcuNode:%d, Mpc Ip: 0x%x, Local Ip: 0x%x, wDiscHeartBeatTime:%d, byDiscHeartBeatNum:%d\n",
                dwMcuNode, 
                dwMpcIp,
                ntohl(g_cBrdAgentApp.m_dwLocalInnerIp),
                g_cBrdAgentApp.m_wDiscHeartBeatTime, g_cBrdAgentApp.m_byDiscHeartBeatNum);
            

        }
        
    }
	return bRet;
}

/*=============================================================================
  函 数 名： RegisterToMcuAgent
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CBoardAgent::RegisterToMcuAgent(u32 dwDstNode)
{
    CServMsg  cReportMsg;
    Dsclog("[BOARD_MPC_REG]byBrdSlot = %d, byBrdLayer = %d, byBrdID = %d\n",
        m_tBoardPosition.byBrdSlot, m_tBoardPosition.byBrdLayer, m_tBoardPosition.byBrdID);
   
    cReportMsg.SetMsgBody((u8*)&m_tBoardPosition, sizeof(TBrdPosition));
    // guzh [1/17/2007] 有关该IP的说明:
    // DSC板 用户配置的IP是 在 show run 里面的主IP, 
    // 内部IP 通过 secondary 方式配置,和MCU eth1 通信
    // 这里上报的主IP,应该是MCU配置文件一致
    cReportMsg.CatMsgBody((u8*)&m_dwBoardIpAddr, sizeof(u32));
    cReportMsg.CatMsgBody(&g_cBrdAgentApp.m_byChoice, sizeof(u8));

    u8  byOsType = 0;
#if defined WIN32
    byOsType = OS_TYPE_WIN32;
#elif defined _LINUX_
    byOsType = OS_TYPE_LINUX;
#else
    byOsType = OS_TYPE_VXWORKS;
#endif
    cReportMsg.CatMsgBody((u8*)&byOsType, sizeof(u8));
	
    post( MAKEIID(AID_MCU_BRDMGR, CInstance::DAEMON), BOARD_MPC_REG, 
        cReportMsg.GetServMsg(), cReportMsg.GetServMsgLen(), dwDstNode );

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
    07/03/27    4.0         顾振华        修改
====================================================================*/
void CBoardAgent::ProcBoardPowerOn( CMessage* const pcMsg )
{
	Dsclog("BoardAgent: Receive Board Power On Message! \n");

	switch( CurState() ) 
	{
	case STATE_IDLE:
		if(0 != g_cBrdAgentApp.m_dwMpcIpAddr)
        {
            SetTimer(BRDAGENT_CONNECT_MANAGERA_TIMER, POWEN_ON_CONNECT);
        }

        if(0 != g_cBrdAgentApp.m_dwMpcIpAddrB )
        {
            SetTimer(BRDAGENT_CONNECT_MANAGERB_TIMER, POWEN_ON_CONNECT);
        }


        BrdQueryPosition(&m_tBoardPosition);
        m_tBoardPosition.byBrdLayer = 0;
        m_tBoardPosition.byBrdSlot = 0;
#ifdef _VXWORKS_	
        TBrdEthParam tBrdEthParam;
        BrdGetEthParam(g_cBrdAgentApp.m_byChoice, &tBrdEthParam);
        m_dwBoardIpAddr = tBrdEthParam.dwIpAdrs;
        printf("[ProcBoardPowerOn] [in] byChoice :%d,  [out] BrdIpAddr :0x%x\n", g_cBrdAgentApp.m_byChoice,
               m_dwBoardIpAddr);
//#endif
//#ifdef _LINUX_
#else
        TBrdEthParamAll tBrdEthParamAll;
        BrdGetEthParamAll(g_cBrdAgentApp.m_byChoice, &tBrdEthParamAll);
        // guzh [1/17/2007] 暂时取第0个用于注册
        m_dwBoardIpAddr = tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs;
        printf("[ProcBoardPowerOn] [in] byChoice :%d,  [out] BrdIpAddr :0x%x\n", g_cBrdAgentApp.m_byChoice,
               m_dwBoardIpAddr);
#endif
//#ifdef WIN32
//        GetBoardCfgInfo();
//#endif
		break;

	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when in (ProcBoardPowerOn)!\n", CurState() );
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
	Dsclog("BoardAgent: Receive Connect Manager Time Out Message! \n");
    BOOL bRet = FALSE;
      
	switch( CurState() ) 
	{
	case STATE_IDLE:
    case STATE_INIT:
    case STATE_NORMAL:
		if(TRUE == bIsConnectA)
        {
            bRet = ConnectManager(m_dwDstNode, 
                    g_cBrdAgentApp.m_dwMpcIpAddr, g_cBrdAgentApp.m_wMpcPort);       //和MPC上的管理程序连接
            if( TRUE == bRet)
            {
                KillTimer(BRDAGENT_CONNECT_MANAGERA_TIMER);
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
                    g_cBrdAgentApp.m_dwMpcIpAddrB, g_cBrdAgentApp.m_wMpcPortB);       //和MPC上的管理程序连接
            if( TRUE == bRet)
            {
                KillTimer(BRDAGENT_CONNECT_MANAGERB_TIMER);
                SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT );
                g_cBrdAgentApp.m_dwDstMcuNodeB = m_dwDstNodeB;
            }
            else
            {
                SetTimer( BRDAGENT_CONNECT_MANAGERB_TIMER, CONNECT_MANAGER_TIMEOUT );
            }
        }

        if(TRUE == bRet && STATE_IDLE == CurState())
        {
            NEXTSTATE( STATE_INIT );
        }
        
		break;

	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when in (ProcBoardConnectManagerTimeOut)!\n", CurState() );
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
	
	Dsclog("[Dsc] Receive Register Manager Time Out Message! \n");

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
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when in (ProcBoardRegisterTimeOut)!\n", CurState() );
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
		log( LOGLVL_EXCEPTION, "BoardAgent: Receive Register NAck Message!  \n");
		break;

	default:
		log( LOGLVL_EXCEPTION, "Wrong state %u when in (ProcBoardRegNAck)!\n", CurState() );
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

	Dsclog("[Dsc] Receive Register Ack Message! \n");

	switch( CurState() ) 
	{
	case STATE_INIT:
    case STATE_NORMAL:
        MsgRegAck(pcMsg);
		break;

	default:
		OspPrintf(TRUE, FALSE, "[Dsc] Wrong state %u when in (ProcBoardRegAck)!\n", CurState() );
		break;
	}
	return;

}

/*=============================================================================
  函 数 名： MsgRegAck
  功    能： 处理注册Ack
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CBoardAgent::MsgRegAck(CMessage* const pcMsg )
{
    if(NULL == pcMsg)
    {
        OspPrintf(TRUE, FALSE, "[Dsc] the pointer can not be Null\n");
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

        
    if(STATE_INIT == CurState()) // 只在INIT状态下才取配置
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

	Dsclog("BoardAgent: Receive Get Config Time Out Message! \n");

	switch( CurState() ) 
	{
	case STATE_INIT:
		// 重新发送取配置信息消息
		PostMsgToManager( BOARD_MPC_GET_CONFIG, (u8*)&m_tBoardPosition, sizeof(m_tBoardPosition) );		
		SetTimer( BRDAGENT_GET_CONFIG_TIMER, GET_CONFIG_TIMEOUT );	
		break;

	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when in (ProcGetConfigTimeOut)!\n", CurState() );
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
		OspPrintf(TRUE, FALSE, "[Dsc] Receive Get Config NAck Message! \n");
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "[Dsc] Wrong state %u! (ProcBoardGetConfigNAck)\n", CurState() );
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
    if(NULL == pcMsg)
    {
        OspPrintf(TRUE, FALSE, "[Dsc] the pointer can not be Null\n");
        return;
    }

    Dsclog("Receive Get Config Ack Message! \n");

	switch( CurState() ) 
	{
	case STATE_INIT:
        MsgGetConfAck(pcMsg);
        break;
        
	default:
		OspPrintf(TRUE, FALSE, "Wrong state %u! (ProcBoardGetConfigAck)\n", CurState() );
		break;
	}
	return;
}

/*=============================================================================
  函 数 名： MsgGetConfAck
  功    能： 处理取配置Ack消息
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CBoardAgent::MsgGetConfAck(CMessage* const pcMsg)
{    
    KillTimer( BRDAGENT_GET_CONFIG_TIMER ); 
    
    /*
    // 增加消息体长度保护, 防止出现异常, zgc, 2008-07-15
    // 外设数量(u8)+外设类型(u8)+PRS配置信息(TEqpPrsEntry)+网同步信息(2*u8)
    // +单板IP(u32)+看门狗标志(u8)+TDSCModuleInfo+GKIP(u32)+TLoginInfo
    // FIXME:这里没有对多外设情况进行处理
    u16 dwLocalMsgLength = ( sizeof(u8)+sizeof(u8)+sizeof(TEqpPrsEntry)+2*sizeof(u8)
        +sizeof(u32)+sizeof(u8)+sizeof(TDSCModuleInfo)+sizeof(u32)
        +sizeof(TLoginInfo) );
    if ( pcMsg->length != dwLocalMsgLength )
    {
        OspPrintf( TRUE, FALSE, "[MsgGetConfAck] Config msg body lenth(%d) is error! Should be %d!\n",
            pcMsg->length, dwLocalMsgLength );
        OspDisconnectTcpNode(m_dwDstNode);
        SetTimer( BRDAGENT_CONNECT_MANAGERA_TIMER, CONNECT_MANAGER_TIMEOUT );
        return;
    }
    */
    
	TEqpPrsEntry*	ptPrsCfg = NULL;	// Prs的配置信息
    u16	wIndex = 0;
    u16 byPeriType = 0;
    u8 byPeriCount = 0;
	u32 dwGKIp = 0;
    u16 wLoop = 0;
    BOOL32 bCfgModified = FALSE;
    u16 wLocalMsgLength = 0;
	CServMsg cServMsg;

	TDSCModuleInfo tOtherModuleInfo, tCurModuleInfo;
	TMINIMCUNetParamAll tNewParamAll, tCurParamAll;

	TLoginInfo tLoginInfo;
#ifdef _LINUX_
	TUserType emUserType;
#endif

	u32 dwGKIpAddr = 0;

    switch(CurState())
    {
    case STATE_INIT:
        byPeriCount = pcMsg->content[wIndex]; // 外设个数
        wIndex ++;

        // 增加消息体长度保护, 防止出现异常, zgc, 2008-07-15
        // 中括号中的内容不一定存在，需要判断是否有外设, zgc, 2008-10-09
        // 外设数量(u8)[+外设类型(u8)+PRS配置信息(TEqpPrsEntry)]+网同步信息(2*u8)
        // +单板IP(u32)+看门狗标志(u8)+TDSCModuleInfo+GKIP(u32)+TLoginInfo
        // FIXME:这里没有对多外设情况进行处理
        if ( byPeriCount == 0 )
        { 
            wLocalMsgLength = ( sizeof(u8)+2*sizeof(u8)+sizeof(u32)+sizeof(u8)
                        +sizeof(TDSCModuleInfo)+sizeof(u32)
                        +sizeof(TLoginInfo) );
        }
        else
        {
            wLocalMsgLength = ( sizeof(u8)+sizeof(u8)+sizeof(TEqpPrsEntry)+2*sizeof(u8)
                        +sizeof(u32)+sizeof(u8)+sizeof(TDSCModuleInfo)+sizeof(u32)
                        +sizeof(TLoginInfo) );
        }
         
        if ( pcMsg->length != wLocalMsgLength )
        {
            OspPrintf( TRUE, FALSE, "[MsgGetConfAck] Config msg body lenth(%d) is error! Should be %d!\n",
                pcMsg->length, wLocalMsgLength );
            OspDisconnectTcpNode(m_dwDstNode);
            SetTimer( BRDAGENT_CONNECT_MANAGERA_TIMER, CONNECT_MANAGER_TIMEOUT );
            return;
        }

	    for( wLoop = 0; wLoop < byPeriCount; wLoop++ )
	    {			
		    byPeriType = pcMsg->content[wIndex++];  //  外设类型
		    
            switch( byPeriType )	//  设置该单板相应的配置信息
		    {
		    case EQP_TYPE_PRS:
			    ptPrsCfg = (TEqpPrsEntry*)(pcMsg->content+wIndex);
                ptPrsCfg->SetMcuIp(g_cBrdAgentApp.m_dwMpcIpAddr);
			    wIndex += sizeof(TEqpPrsEntry);
			    g_cBrdAgentApp.SetPrsConfig(ptPrsCfg);
			    break;

		    default:
			    break;
		    }
	    }
        // 跳过网同步信息
        wIndex += 2*sizeof(u8);

	    // 单板IP        
        printf("BoardAgent: g_cBrdAgentApp.SetBrdIpAddr(0x%x)-wIndex = %d\n",
                                 ntohl(*(u32*)&pcMsg->content[wIndex]), wIndex);        
    
	    g_cBrdAgentApp.SetBrdIpAddr( ntohl(  *(u32*)&pcMsg->content[wIndex] ) );
		
        wIndex += sizeof(u32);
		    
	    if( pcMsg->content[wIndex] == 0 )  // 是否使用看门狗
	    {
    #ifdef _VXWORKS_
		    SysRebootDisable( );
    #endif
    #ifdef _LINUX_
            SysRebootDisable();
    #endif
	    }
	    else
	    {
    #ifdef _VXWORKS_
		    SysRebootEnable( );
    #endif
    #ifdef _LINUX_
            SysRebootEnable();
    #endif
	    }

    #ifdef _VXWORKS_			
	    BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON ); // 将MLINK灯点亮
    #endif
    #ifdef  _LINUX_
        // BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );
    #endif

        wIndex ++;
		
        // guzh [1/12/2007] 增加了其他模块的信息        
        memcpy(&tOtherModuleInfo, &(pcMsg->content[wIndex]), sizeof(TDSCModuleInfo) );

        wIndex += sizeof(TDSCModuleInfo);

		// 保存新DSCinfo, zgc, 2007-07-20	
		tOtherModuleInfo.GetCallAddrAll( tNewParamAll );

		if ( tNewParamAll.IsValid() )
		{
			Dsclog("[MsgGetConfAck] Save dscinfo from mcu to local!\n");
			g_cBrdAgentApp.SaveDscLocalInfoToNip( &tOtherModuleInfo );
			g_cBrdAgentApp.SaveDefaultGWToCfg( tNewParamAll );
		}

		// 取GKIP，zgc, 2007-08-15
		dwGKIp = ntohl(  *(u32*)&pcMsg->content[wIndex] );
		g_cBrdAgentApp.SetGKIpAddr( dwGKIp );
		if ( !g_cBrdAgentApp.SaveGKInfoToCfg( g_cBrdAgentApp.GetGKIpAddr(), MCU_RAS_PORT ) ) // 这里写死RAS端口号为1719, zgc, 2007-07-21
		{
			OspPrintf(TRUE, FALSE, "[MsgGetConfAck] Save GK to cfg file error!\n");
		}
		wIndex += sizeof(u32);

		tLoginInfo = *(TLoginInfo*)&pcMsg->content[wIndex];
		wIndex += sizeof(TLoginInfo);
		// 取本地LoginInfo，等待NIP接口
		#ifndef WIN32
		#endif
		
		// 测试用, zgc, 2007-10-25
		if (g_bPrintDscLog)
		{
			OspPrintf(TRUE, FALSE, "[GetCfgAck] Msg len = %d\n", wIndex+1 );
			for ( s32 n = 0; n < wIndex+1; n++ )
			{
				OspPrintf(TRUE, FALSE, "%x ", pcMsg->content[n] );
				if ( (n+1)%20 == 0 )
				{
					OspPrintf(TRUE, FALSE, "\n" );
				}
			}
		}
		
		/*
		emUserType = USER_TYPE_TELNET_SUPER;
		if ( ERROR == BrdAddUser( (s8*)tLoginInfo.GetUser(), (s8*)tLoginInfo.GetPwd(), emUserType ) )
		{
			Dsclog( "[LoginInfoUpdate] Set login info failed!\n " );
		}
		*/
		
		bCfgModified = g_cBrdAgentApp.SetDscRunningModule( tOtherModuleInfo, FALSE );

//		bCfgModified |= g_cBrdAgentApp.WriteConnectMcuInfo();


		// 修改mtadpport
		if ( tOtherModuleInfo.IsStartGk() )
		{
			//恢复使用1720端口, zgc, 2007-10-11
			g_cBrdAgentApp.ModifyMtadpPort( RASPORT_GK, Q931PORT_NOGK );
		}
		else
		{
			g_cBrdAgentApp.ModifyMtadpPort( RASPORT_NOGK, Q931PORT_NOGK );
		}

		// 修改DCS config, zgc, 2007-09-24
		if ( tOtherModuleInfo.IsStartDcs() )
		{
			TMINIMCUNetParam tNetParam;
			tNewParamAll.GetNetParambyIdx( 0, tNetParam );
			g_cBrdAgentApp.ModifyDcsConfig( tNetParam.GetIpAddr(), ntohl(g_cBrdAgentApp.GetConnectMcuIpAddr()), dwGKIp );
		}

		g_cBrdAgentApp.SetLastRunningModuleInfo( tOtherModuleInfo );
    
	    OspSemGive( g_semDSC );
		   	    
	    NEXTSTATE( STATE_NORMAL );

		printf("Current state.%d\n", CurState());
		
        if (bCfgModified)
        {
    #ifdef _LINUX_        
            OspDelay(1000);
            BrdHwReset();
    #endif
        }

        break;
    default:
        printf("[Dsc] Wrong state: %d when in MsgGetConfAck\n", CurState());
        break;
    }

	return;
}

/*====================================================================
    函数名      ：ProcSetDscInfoReq
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/01/24    4.0         顾振华        创建
====================================================================*/
void CBoardAgent::ProcSetDscInfoReq( CMessage* const pcMsg )
{
	if(NULL == pcMsg)
    {
        OspPrintf(TRUE, FALSE, "[DSC][ProcSetDscInfoReq]The pointer can not be Null\n");
        return;
    }

    if (CurState() != STATE_NORMAL)
    {
        OspPrintf(TRUE, FALSE, "Wrong state %u! (ProcSetDscInfoReq)\n", CurState() );
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TDSCModuleInfo tNewInfo = *(TDSCModuleInfo*)(cServMsg.GetMsgBody());
    if (g_bPrintDscLog)
    {
        tNewInfo.Print();
    }
	TDSCModuleInfo tOldInfo = g_cBrdAgentApp.GetRunningModuleInfo();
	if (g_bPrintDscLog)
    {
        tOldInfo.Print();
    }
	
	u8 byCallAddrNum = tNewInfo.GetCallAddrNum();
    // 需要保留1个地址
	if ( 0 == byCallAddrNum || ETH_IP_MAXNUM-1 <= byCallAddrNum )
	{
		OspPrintf(TRUE, FALSE, "[ProcSetDscInfoReq] The call addr num.%d error!\n", byCallAddrNum );
		PostMsgToManager( BOARD_MPC_SETDSCINFO_NACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		return;
	}

	TMINIMCUNetParamAll tNewParamAll;
	tNewInfo.GetCallAddrAll( tNewParamAll );
	TMINIMCUNetParamAll tOldParamAll;
	tOldInfo.GetCallAddrAll( tOldParamAll );

	BOOL32 bCfgModified = FALSE;
	TMINIMCUNetParam tNetParam;
	tNewParamAll.GetNetParambyIdx( 0, tNetParam );
//#ifdef _LINUX_
	TBrdEthParam tEthParam;
	BrdGetEthParam( 0, &tEthParam );
	// 如果首地址改变，需要重启, zgc, 2007-09-24
	bCfgModified |= ( ntohl(tEthParam.dwIpAdrs) != tNetParam.GetIpAddr() ) ? TRUE : FALSE;
//#endif
	// 和m_dwBoardIpAddr不同，需要重启, zgc, 2007-09-24
	bCfgModified |= ( ntohl(m_dwBoardIpAddr) != tNetParam.GetIpAddr() ) ? TRUE : FALSE;
	

	BOOL32 bSaveCallAddr = TRUE;
	if ( !tOldParamAll.IsEqualTo( tNewParamAll ) )	
	{
		bSaveCallAddr &= g_cBrdAgentApp.SaveDscLocalInfoToNip( &tNewInfo );
		bSaveCallAddr &= g_cBrdAgentApp.SaveDefaultGWToCfg( tNewParamAll );
		if ( !bSaveCallAddr )
		{
			OspPrintf( TRUE, FALSE, "[ProcSetDscInfoReq] Set new dscinfo error! Roll back!\n");
			bSaveCallAddr = TRUE;
			bSaveCallAddr &= g_cBrdAgentApp.SaveDscLocalInfoToNip( &tOldInfo );
			bSaveCallAddr &= g_cBrdAgentApp.SaveDefaultGWToCfg( tOldParamAll );
			if ( !bSaveCallAddr )
			{
				OspPrintf( TRUE, FALSE, "[ProcSetDscInfoReq] Roll back ERROR!\n");
			}
			PostMsgToManager( BOARD_MPC_SETDSCINFO_NACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			return;
		}
	}

	PostMsgToManager( BOARD_MPC_SETDSCINFO_ACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

	bCfgModified |= g_cBrdAgentApp.SetDscRunningModule( tNewInfo );
//	bCfgModified |= g_cBrdAgentApp.WriteConnectMcuInfo();	
	g_cBrdAgentApp.SetLastRunningModuleInfo( tNewInfo );

	// 修改mtadpport
	if ( tNewInfo.IsStartGk() )
	{
		//恢复使用1720端口, zgc, 2007-10-11
		g_cBrdAgentApp.ModifyMtadpPort( RASPORT_GK, Q931PORT_NOGK );
	}
	else
	{
		g_cBrdAgentApp.ModifyMtadpPort( RASPORT_NOGK, Q931PORT_NOGK );
	}
	
	// 修改DCS config
	if ( tNewInfo.IsStartDcs() )
	{
		TMINIMCUNetParam tNetParam;
		tNewParamAll.GetNetParambyIdx( 0, tNetParam );
		g_cBrdAgentApp.ModifyDcsConfig( tNetParam.GetIpAddr() );
	}

	 if (bCfgModified)
	{
	#ifdef _LINUX_        
		OspDelay(1000);
		BrdHwReset();
	#endif
	}

}

/*=============================================================================
函 数 名： ProcBoardGkInfoUpdateCmd
功    能： 
算法实现： 
全局变量： 
参    数： CMessage* const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/21   4.0			周广程                  创建
=============================================================================*/
void CBoardAgent::ProcBoardGkInfoUpdateCmd( CMessage* const pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u32 dwGKIp = *(u32*)(cServMsg.GetMsgBody() + sizeof(u8)*2);
	dwGKIp = ntohl(dwGKIp);
	g_cBrdAgentApp.SetGKIpAddr( dwGKIp );
	if ( !g_cBrdAgentApp.SaveGKInfoToCfg( g_cBrdAgentApp.GetGKIpAddr(), MCU_RAS_PORT ) ) // 这里写死RAS端口号为1719, zgc, 2007-07-21
	{
		OspPrintf(TRUE, FALSE, "[ProcBoardGkInfoUpdateCmd] Save GK to cfg file error!\n");
	}
	return;
}

/*=============================================================================
函 数 名： ProcBoardGkInfoUpdateCmd
功    能： 
算法实现： 
全局变量： 
参    数： CMessage* const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/21   4.0			周广程                  创建
=============================================================================*/
void CBoardAgent::ProcBoardLoginInfoUpdateCmd( CMessage* const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TLoginInfo tLoginInfo = *(TLoginInfo*)( cServMsg.GetMsgBody() + sizeof(u8)*2 );

	s8 achPwd[MAXLEN_PWD+1];
	memset(achPwd, 0, sizeof(achPwd));
	tLoginInfo.GetPwd(achPwd, sizeof(achPwd));
	if ( NULL == tLoginInfo.GetUser() || 0 == strlen(achPwd) )
	{
		printf( "[ProcBoardLoginInfoUpdateCmd] Login name or password is null! Error!\n" );
		return ;
	}
	
#ifdef _LINUX_
	TUserType emUserType = USER_TYPE_TELNET_SUPER;
	STATUS wRet = BrdAddUser( (s8*)tLoginInfo.GetUser(), achPwd, emUserType );
	if ( ERROR == wRet )
	{
		Dsclog( "[LoginInfoUpdate] Set login info failed!\n " );
	}
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
		Dsclog("[PostMsgToManager] MSG<%s> send!\n", ::OspEventDesc(wEvent) );
		post( m_dwDstIId, wEvent, pbyContent, wLen, m_dwDstNode );
	}
	else
	{
		Dsclog("[Dsc] the A's node is invalid\n");
        Dsclog("Board [%d,%d,%d] is offline now: Node=%u\n", 
			m_tBoardPosition.byBrdID, m_tBoardPosition.byBrdLayer, 
            m_tBoardPosition.byBrdSlot, m_dwDstNode);
	}
    
    if(OspIsValidTcpNode(m_dwDstNodeB))
    {
        post(m_dwDstIIdB, wEvent, pbyContent, wLen, m_dwDstNodeB);
    }
    else
    {
        Dsclog("[Dsc] the B's node is invalid\n");
        Dsclog("BoardAgent: Board [%d,%d,%d] is offline now: Node=%u\n", 
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

	Dsclog("BoardAgent: Receive Osp Disconnect Message! \n");

    u32 dwNode = 0;
	switch( CurState() ) 
	{
    case STATE_IDLE:
	case STATE_INIT:
	case STATE_NORMAL:
        MsgDisconnectInfo(pcMsg);
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u! (ProcOspDisconnect)\n", CurState() );
		break;
	}
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
        OspPrintf(TRUE, FALSE, "[Dsc] the pointer can not be Null.(MsgDisconnectInfo)\n");
        return;
    }

    u16 wRet = 0;

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
        wRet = DISCONNECT_MCUB;
    }

    if(INVALID_NODE == m_dwDstNode && INVALID_NODE == m_dwDstNodeB)
    {

    #ifdef _VXWORKS_
		BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF );
    #endif

    #ifdef _LINUX_
        /* add the linux interface */
        BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF );
    #endif
        NEXTSTATE( STATE_IDLE );
        OspDelay(5000);		
    }
    return;
}
/*=============================================================================
  函 数 名： ProcBoardModuleStatus
  功    能： 处理模块告警
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CBoardAgent::ProcBoardModuleStatus( CMessage* const pcMsg )
{
    return;
}

/*=============================================================================
  函 数 名： ProcBoardLedStatus
  功    能： 处理灯告警
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CBoardAgent::ProcBoardLedStatus( CMessage* const pcMsg )
{
    return;
}

/*=============================================================================
  函 数 名： ProcGetModuleInfoReq
  功    能： 取单板模块信息
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CBoardAgent::ProcGetModuleInfoReq( CMessage* const pcMsg )
{
    switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:		
		// 发送给MANAGER响应
		PostMsgToManager( BOARD_MPC_GET_MODULE_ACK, (u8*)&m_tBoardPosition, sizeof(m_tBoardPosition) );
		break;
		
	default:
		log( LOGLVL_EXCEPTION, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*=============================================================================
  函 数 名： ProcRebootCmd
  功    能： reboot 
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CBoardAgent::ProcRebootCmd(CMessage* const pcMsg)
{	
	OspPrintf(TRUE, FALSE, "[Dsc] Receive reboot command.\n");
#ifndef WIN32	
	switch( CurState() )
	{
	case STATE_INIT:
	case STATE_NORMAL:
		
        OspDelay(2000);		
        BrdHwReset();
	
		break;
	default:
		break;
	}
#endif
	return;
}
/*=============================================================================
  函 数 名： ProcBoardSelfTestCmd
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CBoardAgent::ProcBoardSelfTestCmd(CMessage* const pcMsg)
{
	return;
}
/*=============================================================================
  函 数 名： ProcBitErrorTestCmd
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CBoardAgent::ProcBitErrorTestCmd(CMessage* const pcMsg )
{
	return;
}
/*=============================================================================
  函 数 名： ProcTimeSyncCmd
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CBoardAgent::ProcTimeSyncCmd(CMessage* const pcMsg )
{
#ifndef WIN32	    
	s8 achTemp[50];
    struct tm tmTime;

    switch( CurState() ) 
    {
    case STATE_INIT:
    case STATE_NORMAL:			
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
        if( ERROR == BrdTimeSet( &tmTime ) )
        {
            printf( "BrdTimeSet failed!\n" );
            return;
        }
	}
#endif
}
/*=============================================================================
  函 数 名： ProcUpdateSoftwareCmd
  功    能： 软件升级
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CBoardAgent::ProcUpdateSoftwareCmd(CMessage* const pcMsg )
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
	OspPrintf(TRUE, FALSE, "BoardAgent: Rcv Update Software Msg: byBrdId.%d byNum.%d\n", byBrdId, byNum);
	
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
						OspPrintf(TRUE, FALSE, "[ProcUpdateSoftwareCmd] fail to rename file %s to %s.\n", achFileName, achOldFile);
					}
					else// 新文件改名
					{
						nRet = rename(achTmpName, achFileName);
						if( ERROR == nRet )
						{
                            abyRet[byFileNum] = 0;
							OspPrintf(TRUE, FALSE, "[ProcUpdateSoftwareCmd] fail to rename file %s to %s.\n", achTmpName, achFileName);						
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
                        OspPrintf(TRUE, FALSE, "[ProcUpdateBrdVersion] BrdUpdateAppImage failed.\n");
                    }
                    else
                    {
                        OspPrintf(TRUE, FALSE, "[ProcUpdateBrdVersion] BrdUpdateAppImage success.\n");
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
                            OspPrintf(TRUE, FALSE, "[ProcUpdateBrdVersion] BrdFpUpdateAuxData failed.\n");
                        }
                        else
                        {
                            OspPrintf(TRUE, FALSE, "[ProcUpdateBrdVersion] BrdFpUpdateAuxData success.\n");
                        }
                    }   
                    else
                    {
                        OspPrintf(TRUE, FALSE, "[ProcUpdateBrdVersion] not supported file type.\n");
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
		log( LOGLVL_EXCEPTION, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			 pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*=============================================================================
函 数 名： ProcStartDscUpdateSoftwareReq
功    能： 
算法实现： 
全局变量： 
参    数： CMessage* const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/20   4.0			周广程                  创建
=============================================================================*/
void CBoardAgent::ProcStartDscUpdateSoftwareReq( CMessage* const pcMsg )
{
	if ( pcMsg == NULL )
	{
		Dsclog( "[ProcStartDscUpdateSoftwareReq] The msg is NULL! Error!\n" );
		return;
	}

	CServMsg cMsg( pcMsg->content, pcMsg->length );
	if ( m_byMcsSsnIdx == 0 )
	{
		m_byBrdIdx = *cMsg.GetMsgBody();
		m_byMcsSsnIdx = *( cMsg.GetMsgBody()+sizeof(u8) );
		m_byUpdateFileNum = *( cMsg.GetMsgBody()+sizeof(u8)+sizeof(u8) );
		Dsclog("[ProcStartDscUpdateSoftwareReq] BrdIdx = %d, McsSsnIdx = %d, UpdateFileNum = %d\n",
			m_byBrdIdx, m_byMcsSsnIdx, m_byUpdateFileNum);
	}
	else
	{
		Dsclog("[ProcStartDscUpdateSoftwareReq] Other Mcs<McsSsn.%d> is updating now!\n", m_byMcsSsnIdx);
		PostMsgToManager(DSC_MPC_STARTUPDATE_SOFTWARE_NACK, pcMsg->content, pcMsg->length);
		return;
	}
	
	u8 *pMsgBuf = cMsg.GetMsgBody() + sizeof(u8)*3;
	u8 byFileNameLen = 0;
	memset( m_achCurUpdateFileName, 0, sizeof(m_achCurUpdateFileName) );
	memset(m_abyUpdateResult, 0, sizeof(m_abyUpdateResult));
	for ( u8 byLop = 0; byLop < m_byUpdateFileNum; byLop++ )
	{
		byFileNameLen = *pMsgBuf;
		pMsgBuf++;
		strncpy( m_achCurUpdateFileName[byLop], (s8*)pMsgBuf, byFileNameLen );
		m_achCurUpdateFileName[byLop][MAXLEN_MCU_FILEPATH-1] = '\0';
		pMsgBuf += byFileNameLen;
	}

	if ( !g_bUpdateSoftBoard )
	{
		PostMsgToManager(DSC_MPC_STARTUPDATE_SOFTWARE_NACK, pcMsg->content, pcMsg->length);
	}
	else
	{
		PostMsgToManager(DSC_MPC_STARTUPDATE_SOFTWARE_ACK, pcMsg->content, pcMsg->length);
		SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 1000);
	}

	return;
}

/*=============================================================================
函 数 名： ProcUpdateFileReq
功    能： 
算法实现： 
全局变量： 
参    数： CMessage* const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/25   4.0			周广程                  创建
=============================================================================*/
void CBoardAgent::ProcUpdateFileReq( CMessage* const pcMsg )
{
	if ( pcMsg == NULL )
	{
		Dsclog( "[ProcUpdateFileReq] The msg is NULL! Error!\n" );
		return;
	}

//	FILE *hFile = NULL;

	KillTimer(DSC_UPDATE_SOFTWARE_WAITTIMER);
	m_byWaitTimes = 0;

	CServMsg cMsg( pcMsg->content, pcMsg->length );
	u8 byBrdIdx = *( cMsg.GetMsgBody() );
	u8 byMcsSsnIdx = *( cMsg.GetMsgBody()+sizeof(u8) );
	TDscUpdateReqHead tDscUpdateReqHead = *(TDscUpdateReqHead*)( cMsg.GetMsgBody() + 2*sizeof(u8) );
	if ( strlen(m_tCurUpdateReq.m_acFileName) == 0 )
	{
		strncpy( m_tCurUpdateReq.m_acFileName, tDscUpdateReqHead.m_acFileName, MAXLEN_MCU_FILEPATH-1);
		m_tCurUpdateReq.m_acFileName[MAXLEN_MCU_FILEPATH-1] = 0;
	}
	//如果与当前请求的文件和帧不一致，则直接丢弃，重发请求
	if ( (strcmp(tDscUpdateReqHead.m_acFileName, m_tCurUpdateReq.m_acFileName ) != 0
		|| tDscUpdateReqHead.GetReqFrmSN() != m_tCurUpdateReq.GetRspFrmSN()
		|| m_byBrdIdx != byBrdIdx
		|| m_byMcsSsnIdx != byMcsSsnIdx)
		&& 
		m_byErrTimes <= 3 )
	{
		Dsclog("[ProcUpdateFileReq] Req update pack is not according with waitting pack! NACK!\n");
		Dsclog("ReqFile : %s, WaitFile : %s\n",tDscUpdateReqHead.m_acFileName, m_tCurUpdateReq.m_acFileName);
		Dsclog("ReqFrmSN : %d, WaitFrmSN : %d\n", tDscUpdateReqHead.GetReqFrmSN(), m_tCurUpdateReq.GetRspFrmSN());
		Dsclog("ReqBrdIdx : %d, WaitBrdIdx : %d\n", byBrdIdx, m_byBrdIdx);
		Dsclog("ReqMcsSsnIdx : %d, WaitMcsSsnIdx : %d\n", byMcsSsnIdx, m_byMcsSsnIdx);
		NackUpdateFileReq();
		SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
		return;
	}
	else
	{
		if ( tDscUpdateReqHead.GetReqFrmSN() == 0 )
		{
			Dsclog("[ProcUpdateFileReq]ReqFileSize: %d\n", tDscUpdateReqHead.GetFileSize());
			// 申请缓存, zgc, 2007-10-18
			if ( NULL == m_pbyFileBuf )
			{
				m_pbyFileBuf = new u8[UPDATE_FILEBUF_SIZE];
				if ( NULL == m_pbyFileBuf )
				{
					Dsclog( "[ProcUpdateFileReq] Alloc mem failed! NACK!\n" );
					NackUpdateFileReq();
					SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
					return;
				}
				memset( m_pbyFileBuf, 0, UPDATE_FILEBUF_SIZE );
				m_dwBufUsedSize = 0;
			}
		}
		Dsclog("ReqFrmSize: %d,\tReqFrmSN: %d,\tMark: %d\n", tDscUpdateReqHead.GetFrmSize(), tDscUpdateReqHead.GetReqFrmSN(), tDscUpdateReqHead.m_byMark);
	}

	if ( m_byErrTimes > 3)
	{
		if ( strlen(m_tCurUpdateReq.m_acFileName) != 0 )
		{
		//	FILE *hFile = fopen( m_tCurUpdateReq.m_acFileName, "r" );
			if ( NULL == m_hUpdateFile )
			{
				m_hUpdateFile = fopen( m_tCurUpdateReq.m_acFileName, "r" );
			}
			//if ( hFile != NULL )
			if ( m_hUpdateFile != NULL )
			{
				//fclose(hFile);
				fclose(m_hUpdateFile);
				remove(m_tCurUpdateReq.m_acFileName);
			}
		}
		memset( &m_tCurUpdateReq, 0, sizeof(m_tCurUpdateReq) );
		NackUpdateFileReq();
		m_byErrTimes = 0;
		m_byMcsSsnIdx = 0;
		m_byBrdIdx = 0;
		m_byUpdateFileNum = 0;
		if ( NULL != m_hUpdateFile )
		{
			fclose( m_hUpdateFile );
			m_hUpdateFile = NULL;
		}
		if ( NULL != m_pbyFileBuf )
		{
			delete [] m_pbyFileBuf;
			m_pbyFileBuf = NULL;
		}
		m_dwBufUsedSize = 0;
		return;
	}

	// 清空原有文件或创建新空文件
	m_byErrTimes = 0;
	if ( tDscUpdateReqHead.GetReqFrmSN() == 0 )
	{
		//hFile = fopen( tDscUpdateReqHead.m_acFileName, "w" );
		if ( NULL != m_hUpdateFile )
		{
			fclose(m_hUpdateFile);
			m_hUpdateFile = NULL;
		}
		m_hUpdateFile = fopen( tDscUpdateReqHead.m_acFileName, "w" );
		//if ( hFile == NULL )
		if ( NULL == m_hUpdateFile )
		{
			Dsclog( "[ProcUpdateFileReq] Open or creat file<%s> failed!\n", tDscUpdateReqHead.m_acFileName );
		//	memset( &m_tCurUpdateReq, 0, sizeof(m_tCurUpdateReq) );
			NackUpdateFileReq();
//			m_byMcsSsnIdx = 0;
//			m_byBrdIdx = 0;
//			m_byUpdateFileNum = 0;
			SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
			return;
		}
		//fclose( hFile );
		fclose( m_hUpdateFile );
		m_hUpdateFile = NULL;
	}

	//hFile = fopen( tDscUpdateReqHead.m_acFileName, "ab" );
	if ( NULL == m_hUpdateFile )
	{
		m_hUpdateFile = fopen( tDscUpdateReqHead.m_acFileName, "ab" );
		if ( NULL == m_hUpdateFile )
		{
			Dsclog( "[ProcUpdateFileReq] Open file<%s> failed! NACK!\n", tDscUpdateReqHead.m_acFileName );
			NackUpdateFileReq();
			SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
			return;
		}
	}
	u8 *pFileContent = cMsg.GetMsgBody() + 2*sizeof(u8) + sizeof(TDscUpdateReqHead);
	u32 dwFileContentLen = cMsg.GetMsgBodyLen() - 2*sizeof(u8) - sizeof(TDscUpdateReqHead);
	//fwrite( pFileContent, sizeof(u8), dwFileContentLen, hFile );
	//fclose( hFile );
	// 先写缓存，写满后再写入文件中，zgc, 2007-10-18
	if ( m_dwBufUsedSize + dwFileContentLen > UPDATE_FILEBUF_SIZE )
	{
		if ( m_dwBufUsedSize == fwrite( m_pbyFileBuf, sizeof(u8), m_dwBufUsedSize, m_hUpdateFile ) )
		{
			memset( m_pbyFileBuf, 0, UPDATE_FILEBUF_SIZE );
			m_dwBufUsedSize = 0;
		}
		else
		{
			Dsclog("[ProcUpdateFileReq] Write File<%s> failed! NACK!\n", tDscUpdateReqHead.m_acFileName );
			NackUpdateFileReq();
			SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
			return;
		}
	}
	memcpy( m_pbyFileBuf+m_dwBufUsedSize, pFileContent, dwFileContentLen );
	m_dwBufUsedSize += dwFileContentLen;

	if ( tDscUpdateReqHead.m_byMark == 0 )
	{
		// 请求下一帧
		u32 dwRspFrmSN = m_tCurUpdateReq.GetRspFrmSN() + 1;
		m_tCurUpdateReq.SetRspFrmSN(dwRspFrmSN);
		cMsg.SetMsgBody( (u8*)&byBrdIdx, sizeof(byBrdIdx) );
		cMsg.CatMsgBody( (u8*)&byMcsSsnIdx, sizeof(byMcsSsnIdx) );
		cMsg.CatMsgBody( (u8*)&m_tCurUpdateReq, sizeof(m_tCurUpdateReq) );
		PostMsgToManager( DSC_MPC_UPDATEFILE_ACK, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
		SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
	}
	else
	{
		// 最后一部分写入, zgc, 2008-03-18
		if ( 0 != m_dwBufUsedSize )
		{
			if ( m_dwBufUsedSize == fwrite( m_pbyFileBuf, sizeof(u8), m_dwBufUsedSize, m_hUpdateFile ) )
			{
				memset( m_pbyFileBuf, 0, UPDATE_FILEBUF_SIZE );
				m_dwBufUsedSize = 0;
			}
			else
			{
				m_dwBufUsedSize -= dwFileContentLen;

				Dsclog("[ProcUpdateFileReq] Write File<%s> failed! NACK!\n", tDscUpdateReqHead.m_acFileName );
				NackUpdateFileReq();
				SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
				return;
			}
		}

		// 长度校验
		FILE *hp = m_hUpdateFile;
		fseek( hp, 0, SEEK_END );
		u32 dwFileLen = ftell(hp);
		if ( tDscUpdateReqHead.GetFileSize() != dwFileLen )
		{
			Dsclog("[ProcUpdateFileReq] File(%s) recv filelen(%d) not equal to reqlen(%d)! NACK!\n",
				tDscUpdateReqHead.m_acFileName, dwFileLen, tDscUpdateReqHead.GetFileSize() );
			NackUpdateFileReq();
			SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
			return;
		}

		u8 byLop = 0;
		for ( byLop = 0; byLop < m_byUpdateFileNum; byLop++ )
		{
			if ( 0 == strcmp(tDscUpdateReqHead.m_acFileName, m_achCurUpdateFileName[byLop] ) )
			{
				m_abyUpdateResult[byLop] = 1;
				break;
			}
		}
		m_tCurUpdateReq.SetRspFrmSN(0);
		memset ( m_tCurUpdateReq.m_acFileName, 0, sizeof(m_tCurUpdateReq.m_acFileName) );
		if ( NULL != m_hUpdateFile )
		{
			fclose( m_hUpdateFile );
			m_hUpdateFile = NULL;
		}
		if ( NULL != m_pbyFileBuf )
		{
			delete [] m_pbyFileBuf;
			m_pbyFileBuf = NULL;
		}
		m_dwBufUsedSize = 0;
		cMsg.SetMsgBody( (u8*)&byBrdIdx, sizeof(byBrdIdx) );
		cMsg.CatMsgBody( (u8*)&byMcsSsnIdx, sizeof(byMcsSsnIdx) );
		cMsg.CatMsgBody( (u8*)&m_tCurUpdateReq, sizeof(m_tCurUpdateReq) );
		PostMsgToManager( DSC_MPC_UPDATEFILE_ACK, cMsg.GetServMsg(), cMsg.GetServMsgLen() );		
		for ( byLop = 0; byLop < m_byUpdateFileNum; byLop++ )
		{
			if ( m_abyUpdateResult[byLop] != 1 )
			{
				break;
			}
		}
		if ( byLop == m_byUpdateFileNum )
		{
			m_byMcsSsnIdx = 0;
			m_byBrdIdx = 0;
			m_byUpdateFileNum = 0;
			memset(m_abyUpdateResult, 0 , sizeof(m_abyUpdateResult));
			Dsclog("[ProcUpdateFileReq] Update complete successfully!\n");
		}
		else
		{
			SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
		}
		return;
	}
}

/*=============================================================================
函 数 名： ProcUpdateSoftwareWaittimer
功    能： 
算法实现： 
全局变量： 
参    数： CMessage* const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/21   4.0			周广程                  创建
=============================================================================*/
void CBoardAgent::ProcUpdateSoftwareWaittimer( CMessage* const pcMsg )
{
	KillTimer(DSC_UPDATE_SOFTWARE_WAITTIMER);
	m_byWaitTimes++;
	CServMsg cMsg;
	if ( m_byWaitTimes <= 3 && strlen(m_tCurUpdateReq.m_acFileName) != 0 )
	{
		NackUpdateFileReq();
		SetTimer(DSC_UPDATE_SOFTWARE_WAITTIMER, 3000);
	}
	else
	{
		m_byWaitTimes = 0;
		m_byErrTimes = 0;
		Dsclog( "[ProcDscUpDateRsp] Lost connect during update file<%s>!\n", m_tCurUpdateReq.m_acFileName );
		if ( strlen(m_tCurUpdateReq.m_acFileName) != 0 )
		{
			//FILE *hFile = fopen( m_tCurUpdateReq.m_acFileName, "r" );
			if ( NULL == m_hUpdateFile )
			{
				m_hUpdateFile = fopen( m_tCurUpdateReq.m_acFileName, "r" );
			}
			//if ( hFile != NULL )
			if ( m_hUpdateFile != NULL )
			{
				//fclose(hFile);
				fclose( m_hUpdateFile );
				remove(m_tCurUpdateReq.m_acFileName);
				m_hUpdateFile = NULL;
			}
		}
		memset( &m_tCurUpdateReq, 0, sizeof(m_tCurUpdateReq) );		

		NackUpdateFileReq();

		m_byMcsSsnIdx = 0;
		m_byBrdIdx = 0;
		m_byUpdateFileNum = 0;	
		memset( m_achCurUpdateFileName, 0, sizeof(m_achCurUpdateFileName) );
		memset( m_abyUpdateResult, 0, sizeof(m_abyUpdateResult) );
		if ( m_hUpdateFile != NULL )
		{
			fclose( m_hUpdateFile );
			m_hUpdateFile = NULL;
		}
		if ( m_pbyFileBuf != NULL )
		{
			delete [] m_pbyFileBuf;
			m_pbyFileBuf = NULL;
		}
		m_dwBufUsedSize = 0;
		return;
	}
}

/*=============================================================================
函 数 名： NackUpdateFileReq
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/21   4.0			周广程                  创建
=============================================================================*/
void CBoardAgent::NackUpdateFileReq(void)
{
	CServMsg cMsg;
	cMsg.SetMsgBody( (u8*)&m_byBrdIdx, sizeof(m_byBrdIdx) );
	cMsg.CatMsgBody( (u8*)&m_byMcsSsnIdx, sizeof(m_byMcsSsnIdx) );
	cMsg.CatMsgBody( (u8*)&m_tCurUpdateReq, sizeof(m_tCurUpdateReq) );
	PostMsgToManager( DSC_MPC_UPDATEFILE_NACK, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
	m_byErrTimes++;
}


//////////////////////////////////////////////////////////////////////////

API u32 BrdGetDstMcuNode( void )
{
    return g_cBrdAgentApp.m_dwDstMcuNode;
}

API u32 BrdGetDstMcuNodeB(void)
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}

API void noupdatesoft( void )
{
	g_bUpdateSoftBoard = FALSE;
}

API void updatesoft( void )
{
	g_bUpdateSoftBoard = TRUE;
}





















