/*****************************************************************************
   模块名      : Board Agent Basic
   文件名      : boardagentbasic.cpp
   相关文件    : 
   文件实现功能: 单板代理实现，完成告警处理和与MANAGER的交互
   作者        : 周广程
   版本        : V4.0  Copyright(C) 2001-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2007/08/20  4.0         周广程       创建
******************************************************************************/

#include "boardagentbasic.h"
#include "mcuver.h"

#ifdef _VXWORKS_
#include "timers.h"
#include <dirent.h>
#endif


//构造函数
CBBoardAgent::CBBoardAgent()
{
	m_pBBoardConfig = NULL;
	m_byIsMpu2SimuMpu = 0;
	m_byIsAnyBrdRegSuccess = 0;
	m_byRegBrdTypeFlagA = 0;
	m_byRegBrdTypeFlagB = 0;
	return;
}

//析构函数
CBBoardAgent::~CBBoardAgent()
{
	m_pBBoardConfig = NULL;
	m_byIsMpu2SimuMpu = 0;
	m_byIsAnyBrdRegSuccess = 0;
	m_byRegBrdTypeFlagA = 0;
	m_byRegBrdTypeFlagB = 0;
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
    2007/08/20  4.0         周广程       创建
====================================================================*/
void CBBoardAgent::InstanceExit()
{
	BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF );
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
    2007/08/20  4.0         周广程       创建
====================================================================*/
void CBBoardAgent::InstanceEntry(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
	{
		OspPrintf( TRUE, FALSE,  "[InstanceEntry] The received msg's pointer in the msg entry is NULL!");
		return;
	}

	if ( pcMsg->event != OSP_POWERON )
	{
		brdagtlog( "[InstanceEntry] Msg.%d<%s> received!\n", pcMsg->event, ::OspEventDesc(pcMsg->event) );
	}
	
	switch( pcMsg->event )
	{
	case OSP_POWERON:                   //单板启动
        ProcBoardPowerOn( pcMsg ) ;
		break;
	case BRDAGENT_CONNECT_MANAGER_TIMER:
		ProcBoardConnectManagerTimeOut();
		break;


	case BRDAGENT_REGISTERA_TIMER:		        // 注册时间超时
		ProcBoardRegisterTimeOut( TRUE );
		break;

    case BRDAGENT_REGISTERB_TIMER:		        // 注册时间超时
		ProcBoardRegisterTimeOut( FALSE );
		break;

	case MPC_BOARD_REG_ACK:                     //注册应答消息
		ProcBoardRegAck( pcMsg );
		break;
		
	case MPC_BOARD_REG_NACK:			        //注册否定应答消息
		ProcBoardRegNAck( pcMsg );
		break;

	case BRDAGENT_GET_CONFIG_TIMER:             //等待配置应答消息超时
		ProcGetConfigTimeOut( pcMsg );
		break;

	case MPC_BOARD_ALARM_SYNC_REQ:              //管理程序的告警同步请求
		ProcAlarmSyncReq( pcMsg );
		break;

	case MPC_BOARD_GET_VERSION_REQ:             //获取单板的版本信息
		ProcGetVersionReq( pcMsg );
		break;

	case MPC_BOARD_GET_MODULE_REQ:              //获取单板的模块信息
		ProcGetModuleInfoReq( pcMsg );
		break;

	case MPC_BOARD_TIME_SYNC_CMD:               //单板时间同步命令
		ProcTimeSyncCmd( pcMsg );
		break;

	case MPC_BOARD_SELF_TEST_CMD:               //单板自测命令
		ProcBoardSelfTestCmd( pcMsg );
		break;

	case MPC_BOARD_BIT_ERROR_TEST_CMD:          //单板误码测试命令
		ProcBitErrorTestCmd( pcMsg );
		break;

	case MPC_BOARD_GET_STATISTICS_REQ:          //获取单板的统计信息
		ProcGetStatisticsReq( pcMsg );
		break;

	case MPC_BOARD_UPDATE_SOFTWARE_CMD:			//单板升级命令
		ProcUpdateSoftwareCmd( pcMsg );
		break;

	// 以下是告警处理的相关消息
	case SVC_AGT_MEMORY_STATUS:                 //内存状态改变
		ProcBoardMemeryStatus( pcMsg );
		break;

	case SVC_AGT_FILESYSTEM_STATUS:             //文件系统状态改变
		ProcBoardFileSystemStatus( pcMsg );
		break;

	// 以下是测试用消息
	case BOARD_MPC_CFG_TEST:
		ProcBoardGetCfgTest( pcMsg );
		break;

	case BOARD_MPC_GET_ALARM:
		ProcBoardGetAlarm( pcMsg );
		break;

	case BOARD_MPC_MANAGERCMD_TEST:
		ProcBoardGetLastManagerCmd( pcMsg );
		break;

	case MPC_BOARD_RESET_CMD:                   //单板重启动命令
		ProcBoardResetCmd( pcMsg );
		break;

	case OSP_DISCONNECT:
		ProcOspDisconnect( pcMsg );
		break;

	case BOARD_TEMPERATURE_STATUS_NOTIF:
		ProcBoardTempStatusNotif(pcMsg);
		break;
		
	case BOARD_CPU_STATUS_NOTIF:
		ProcBoardCpuStatusNotif(pcMsg);
		break;

	default:
		OspPrintf( TRUE, FALSE,  "[InstanceEntry] receive unknown msg %d<%s>! \n",
                                  pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
	return;
}

/*====================================================================
    函数名      ：ConnectManager
    功能        ：和MPC上的管理程序连接
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bIsNodeA 连接mpc A/B, TRUE:A, FALSE:B
    返回值说明  ：连接成功返回TRUE，连接失败或该连接已经存在则返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2007/08/20  4.0         周广程        创建
	2010/06/02	4.6         xueliang	  add filter
	20110105    4.6         pengjie       接口参数调整
====================================================================*/
BOOL32 CBBoardAgent::ConnectManager(BOOL32 bIsNodeA)
{
	if( !IsNeedConnectNode(bIsNodeA) )
	{
		return FALSE;
	}

	u32 dwMpcIp = 0;
	u16 wMpcPort = 0;
	if( bIsNodeA )
	{
		dwMpcIp = m_pBBoardConfig->GetMpcIpA();
		wMpcPort = m_pBBoardConfig->GetMpcPortA();
	}
	else
	{
		dwMpcIp = m_pBBoardConfig->GetMpcIpB();
		wMpcPort = m_pBBoardConfig->GetMpcPortB();
	}
	
	u32 dwMcuNode = OspConnectTcpNode( htonl(dwMpcIp), wMpcPort, 10, 3, 100);
	if( !OspIsValidTcpNode(dwMcuNode) )
	{	
		OspPrintf(TRUE, FALSE, "[BoardAgent] CreateTcpNode Failed MpcIp.%x,MpcPort.%d!\n", dwMpcIp, wMpcPort);
	}
	else
	{
		OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());
		OspSetHBParam(dwMcuNode, m_pBBoardConfig->m_wDiscHeartBeatTime, m_pBBoardConfig->m_byDiscHeartBeatNum);
		if( bIsNodeA )
		{
			m_pBBoardConfig->m_dwDstMcuNodeA = dwMcuNode;
		}
		else
		{
			m_pBBoardConfig->m_dwDstMcuNodeB = dwMcuNode;
		}
		brdagtlog( "[BoardAgent] connect Success MpcIp.%x, Port.%d, Node:%d, wDiscHeartBeatTime:%d, byDiscHeartBeatNum:%d\n",
			dwMpcIp, wMpcPort, dwMcuNode, m_pBBoardConfig->m_wDiscHeartBeatTime, m_pBBoardConfig->m_byDiscHeartBeatNum);
		return TRUE;
	} 

	return FALSE;
}

/*====================================================================
    函数名      IsNeedConnectNode
    功能        ：判断是否需要连接的MPC A/B
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bIsNodeA  TRUE:Mpc A; FLASE:Mpc B
    返回值说明  ：False:不需要连接;True:需要尝试连接
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	20110105    4.6         pengjie       create
====================================================================*/
BOOL32 CBBoardAgent::IsNeedConnectNode( BOOL32 bIsNodeA )
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(IsNeedConnectNode)\n");
		return FALSE;
	}

	if( bIsNodeA == TRUE ) // Mpc A
	{
		if( m_pBBoardConfig->GetMpcIpA() != 0 && m_pBBoardConfig->GetMpcPortA() != 0 &&
			!OspIsValidTcpNode(m_pBBoardConfig->m_dwDstMcuNodeA) )
		{
			return TRUE;
		}
	}
	else // Mpc B
	{
		if( m_pBBoardConfig->GetMpcIpB() != 0 && m_pBBoardConfig->GetMpcPortB() != 0 &&
			!OspIsValidTcpNode(m_pBBoardConfig->m_dwDstMcuNodeB) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*=============================================================================
  函 数 名： RegisterToMcuAgent
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CBBoardAgent::RegisterToMcuAgent(u32 dwDstNode)
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(RegisterToMcuAgent)\n");
		return;
	}

	OspPrintf(TRUE, FALSE, "[RegisterToMcuAgent] Register to Node.%d\n", dwDstNode );

    CServMsg  cReportMsg;
    u32 dwBrdIpAddr = htonl(m_pBBoardConfig->m_dwBrdIpAddr);

	TBrdPos tTmpPos;
	memcpy(&tTmpPos,&m_pBBoardConfig->m_tBoardPosition,sizeof(tTmpPos));
	
	OspPrintf(TRUE, FALSE, "[RegisterToMcuAgent] before adjust,brdId.0x%x\n",tTmpPos.byBrdID);
	printf("[RegisterToMcuAgent]before adjust,brdId.0x%x\n",tTmpPos.byBrdID);
	
	//只在MPC A链时切换board type[2/1/2013 chendaiwei]
	if(m_byIsMpu2SimuMpu && !m_byIsAnyBrdRegSuccess)
	{
		if(dwDstNode == m_pBBoardConfig->m_dwDstMcuNodeA)
		{
			if(m_byRegBrdTypeFlagA== 0)
			{
				tTmpPos.byBrdID = BRD_TYPE_MPU;
				m_byRegBrdTypeFlagA = 1;
				printf("[RegisterToMcuAgent](1)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
				OspPrintf(TRUE, FALSE, "[RegisterToMcuAgent](1)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
			}
			else if(m_byRegBrdTypeFlagA == 1)
			{
				tTmpPos.byBrdID = 0x42;/*BRD_HWID_DSL8000_MPU*/
				m_byRegBrdTypeFlagA = 0;
				printf("[RegisterToMcuAgent](2)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
				OspPrintf(TRUE, FALSE, "[RegisterToMcuAgent](2)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
			}
		}
		else if(dwDstNode == m_pBBoardConfig->m_dwDstMcuNodeB)
		{
			if(m_byRegBrdTypeFlagB== 0)
			{
				tTmpPos.byBrdID = BRD_TYPE_MPU;
				m_byRegBrdTypeFlagB = 1; 
				printf("[RegisterToMcuAgent](3)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
				OspPrintf(TRUE, FALSE, "[RegisterToMcuAgent](3)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
			}
			else if(m_byRegBrdTypeFlagB == 1)
			{
				tTmpPos.byBrdID = 0x42;/*BRD_HWID_DSL8000_MPU*/
				m_byRegBrdTypeFlagB = 0;
				printf("[RegisterToMcuAgent](4)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
				OspPrintf(TRUE, FALSE, "[RegisterToMcuAgent](4)adjust BrdId to 0x%x\n",tTmpPos.byBrdID);
			}
		}
	}
	
	OspPrintf(TRUE, FALSE, "[RegisterToMcuAgent] after adjust,brdId.0x%x,m_byIsMpu2SimuMpu<%d>,m_byIsAnyBrdRegSuccess<%d>,m_byRegBrdTypeFlagA<%d>,m_byRegBrdTypeFlagB<%d>\n",
		tTmpPos.byBrdID,m_byIsMpu2SimuMpu,m_byIsAnyBrdRegSuccess,m_byRegBrdTypeFlagA,m_byRegBrdTypeFlagB);
	printf("[RegisterToMcuAgent] after adjust,brdId.0x%x,m_byIsMpu2SimuMpu<%d>,m_byIsAnyBrdRegSuccess<%d>,m_byRegBrdTypeFlagA<%d>,m_byRegBrdTypeFlagB<%d>\n",
		tTmpPos.byBrdID,m_byIsMpu2SimuMpu,m_byIsAnyBrdRegSuccess,m_byRegBrdTypeFlagA,m_byRegBrdTypeFlagB);
	
    cReportMsg.SetMsgBody((u8*)&tTmpPos, sizeof(TBrdPos));
    cReportMsg.CatMsgBody((u8*)&dwBrdIpAddr, sizeof(u32));
    cReportMsg.CatMsgBody(&m_pBBoardConfig->m_byBrdEthChoice, sizeof(u8));
	  
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
	10/11/25	4.6			xliang	      recode
====================================================================*/
void CBBoardAgent::ProcBoardPowerOn( CMessage* const pcMsg )
{
	if ( NULL == pcMsg )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(ProcBoardPowerOn)\n");
		return;
	}
	
	memcpy( &m_pBBoardConfig, pcMsg->content, sizeof(CBBoardConfig*));
	if ( 0 == m_pBBoardConfig )
	{
		printf("[PowerOn] Config class point is zero!\n");
		return;
	}

	//判断是否是MPU2模拟MPU[2/5/2013 chendaiwei]
	if(m_pBBoardConfig->m_tBoardPosition.byBrdID == BRD_TYPE_MPU)
	{
#ifdef _LINUX12_

		TBrdE2PromInfo tBrdE2PromInfo;
		memset( &tBrdE2PromInfo, 0x0, sizeof(tBrdE2PromInfo) );
		s32 nRet = BrdGetE2PromInfo(&tBrdE2PromInfo); 
		if (ERROR == nRet)
		{
			printf("[BoardAgent][ProcBoardPowerOn] BrdGetE2PromInfo error!\n");
			return;
		}

		if(BRD_PID_KDV8000A_MPU2 == tBrdE2PromInfo.dwProductId)
		{
			m_byIsMpu2SimuMpu = 1;
			m_byIsAnyBrdRegSuccess = 0;
			printf("[ProcBoardPowerOn] Mpu2 simu MPU\n");
			OspPrintf(TRUE,FALSE,"[ProcBoardPowerOn] Mpu2 simu MPU\n");
		}
#endif
	}
	
    // zw 20081114 改下面的TRUE为FALSE
	BOOL32 bRet = FALSE;
	switch( CurState() ) 
	{
	case STATE_IDLE:
		bRet = ProcConnect();
		if ( bRet == TRUE )
		{
			NEXTSTATE( STATE_INIT );
		}
		break;

	default:
		printf( "[PowerOn] Wrong state %u when in (ProcBoardPowerOn)!\n", CurState() );
		break;
	}
	return;
}

/*====================================================================
    函数名      ProcConnect
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/06/01    4.6         xueliang       创建
	20110106    4.6         pengjie        逻辑调整
====================================================================*/
BOOL32 CBBoardAgent::ProcConnect()
{
	if( ConnectManager(TRUE) ) // 尝试连mpc A
	{
		brdagtlog("SetTimer BRDAGENT_REGISTERB_TIMER after connect node A ok!\n");
		SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT);
		return TRUE;
	}

	if( ConnectManager(FALSE) ) // 尝试连mpc B
	{
		brdagtlog("SetTimer BRDAGENT_REGISTERB_TIMER after connect node B ok!\n");
		SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT);
		return TRUE;
	}

	if( IsNeedConnectNode(TRUE) || IsNeedConnectNode(FALSE) )
	{
		SetTimer( BRDAGENT_CONNECT_MANAGER_TIMER, CONNECT_MANAGER_TIMEOUT );
	}

	return FALSE;
}

/*====================================================================
    函数名      ：ProcBoardConnectManagerTimeOut
    功能        ：连接MANAGER超时
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/06/01    4.6         xueliang       创建
====================================================================*/
void CBBoardAgent::ProcBoardConnectManagerTimeOut( void )
{
    BOOL bRet = FALSE;
    
	switch( CurState() ) 
	{
	case STATE_IDLE:
    case STATE_INIT:
    case STATE_NORMAL:
		bRet =  ProcConnect();
		/*
		if ( IsConnectA )
        {
			bRet = ConnectManager(m_pBBoardConfig->m_dwDstMcuNodeA, 
                m_pBBoardConfig->GetMpcIpA(), m_pBBoardConfig->GetMpcPortA());       //和MPC上的管理程序连接
			if( TRUE == bRet)
			{
				SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT );
			}
			else 
			{
				//尝试用IpB建链
				bRet = ConnectManager(m_pBBoardConfig->m_dwDstMcuNodeA, 
					m_pBBoardConfig->GetMpcIpB(), m_pBBoardConfig->GetMpcPortB());

				if( bRet )
				{	
					// [5/26/2010 xliang] ipA，ipB互换角色
					u32 dwTmpIp = m_pBBoardConfig->GetMpcIpA();
					u16 wTmpPort = m_pBBoardConfig->GetMpcPortA();
					m_pBBoardConfig->SetMpcIpA(m_pBBoardConfig->GetMpcIpB());
					m_pBBoardConfig->SetMpcPortA(m_pBBoardConfig->GetMpcPortB());
					m_pBBoardConfig->SetMpcIpB(dwTmpIp);
					m_pBBoardConfig->SetMpcPortB(wTmpPort);

					SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT );
				}
				else
				{
					SetTimer( BRDAGENT_CONNECT_MANAGERA_TIMER, CONNECT_MANAGER_TIMEOUT );
				}
			}
			
		}
		else
		{
			bRet = ConnectManager(m_pBBoardConfig->m_dwDstMcuNodeB, 
                m_pBBoardConfig->GetMpcIpB(), m_pBBoardConfig->GetMpcPortB());       //和MPC上的管理程序连接
			if( TRUE == bRet)
			{
				SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT );
			}
			else
			{
				SetTimer( BRDAGENT_CONNECT_MANAGERB_TIMER, CONNECT_MANAGER_TIMEOUT );
			}
		}
		*/
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
    函数名      ：ProcBoardConnectManagerTimeOut
    功能        ：连接MANAGER超时
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         jianghy       创建
====================================================================*/
// void CBBoardAgent::ProcBoardConnectManagerTimeOut( BOOL32 IsConnectA )
// {
//     BOOL bRet = FALSE;
//     
// 	switch( CurState() ) 
// 	{
// 	case STATE_IDLE:
//     case STATE_INIT:
//     case STATE_NORMAL:
// 		if ( IsConnectA )
//         {
// 			bRet = ConnectManager(m_pBBoardConfig->m_dwDstMcuNodeA, 
//                 m_pBBoardConfig->GetMpcIpA(), m_pBBoardConfig->GetMpcPortA());       //和MPC上的管理程序连接
// 			if( TRUE == bRet)
// 			{
// 				SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT );
// 			}
// 			else 
// 			{
// 				//尝试用IpB建链
// 				bRet = ConnectManager(m_pBBoardConfig->m_dwDstMcuNodeA, 
// 					m_pBBoardConfig->GetMpcIpB(), m_pBBoardConfig->GetMpcPortB());
// 
// 				if( bRet )
// 				{	
// 					// [5/26/2010 xliang] ipA，ipB互换角色
// 					u32 dwTmpIp = m_pBBoardConfig->GetMpcIpA();
// 					u16 wTmpPort = m_pBBoardConfig->GetMpcPortA();
// 					m_pBBoardConfig->SetMpcIpA(m_pBBoardConfig->GetMpcIpB());
// 					m_pBBoardConfig->SetMpcPortA(m_pBBoardConfig->GetMpcPortB());
// 					m_pBBoardConfig->SetMpcIpB(dwTmpIp);
// 					m_pBBoardConfig->SetMpcPortB(wTmpPort);
// 
// 					SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT );
// 				}
// 				else
// 				{
// 					SetTimer( BRDAGENT_CONNECT_MANAGERA_TIMER, CONNECT_MANAGER_TIMEOUT );
// 				}
// 			}
// 		}
// 		else
// 		{
// 			bRet = ConnectManager(m_pBBoardConfig->m_dwDstMcuNodeB, 
//                 m_pBBoardConfig->GetMpcIpB(), m_pBBoardConfig->GetMpcPortB());       //和MPC上的管理程序连接
// 			if( TRUE == bRet)
// 			{
// 				SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT );
// 			}
// 			else
// 			{
// 				SetTimer( BRDAGENT_CONNECT_MANAGERB_TIMER, CONNECT_MANAGER_TIMEOUT );
// 			}
// 		}
//  
//         if(TRUE == bRet && STATE_IDLE == CurState())
//         {
//             NEXTSTATE( STATE_INIT );
//         }
// 		break;
// 
// 	default:
// 		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when in (ProcBoardConnectManagerTimeOut)!\n", CurState() );
// 		break;
// 	}
// 	return;
// }



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
void CBBoardAgent::ProcBoardRegisterTimeOut( BOOL32 bIsConnectA )
{

	switch( CurState() )
	{
	case STATE_INIT:
    case STATE_NORMAL:
        if(TRUE == bIsConnectA)
        {
            RegisterToMcuAgent(m_pBBoardConfig->m_dwDstMcuNodeA);
            SetTimer( BRDAGENT_REGISTERA_TIMER, REGISTER_TIMEOUT );
        }
        else
        {
            RegisterToMcuAgent(m_pBBoardConfig->m_dwDstMcuNodeB);
            SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT );
        }
		break;

	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when in (ProcBoardRegisterTimeOut)!\n", 
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
void CBBoardAgent::ProcBoardRegNAck( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
    case STATE_NORMAL:
		break;

	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %d when receive RegNack!srcNodeId.%d\n", 
			CurState(),(pcMsg != NULL)?pcMsg->srcnode:0 );
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
void CBBoardAgent::ProcBoardRegAck( CMessage* const pcMsg )
{

	if(NULL == pcMsg)
    {
        OspPrintf(TRUE, FALSE, "[Cri] The pointer cannot be Null. (ProcBoardRegAck)\n");
        return;
    }

	switch( CurState() ) 
	{
	case STATE_INIT:
    case STATE_NORMAL:
        MsgRegAck(pcMsg );	
		break;
       
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when receive Reg ack!\n", 
			CurState() );
		break;
	}
	return;

}

/*=============================================================================
  函 数 名： UpdateIpPort
  功    能： 更新IP，port
  算法实现： 
  全局变量： 
  参    数： u32 dwIp (host sequence)
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  10/06/02    4.6         xueliang      重整
==============================================================================*/
void CBBoardAgent::UpdateIpPort(u32 dwIp, u16 wPort, BOOL32 bUpdateA)
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(UpdateIpPort)\n");
		return;
	}

	if( bUpdateA )
	{
		m_pBBoardConfig->SetMpcIpA(dwIp);
		m_pBBoardConfig->SetMpcPortA(wPort);
	}
	else
	{
		m_pBBoardConfig->SetMpcIpB(dwIp);
		m_pBBoardConfig->SetMpcPortB(wPort);
	}
	
	return;
}

/*=============================================================================
函 数 名： ProcBoardTempStatusNotif
功    能： 单板温度状态变化时，发送单板温度状态给MPC
算法实现： 
全局变量： 
参    数： CMessage* const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/10/25   4.0		chendaiwei            创建
=============================================================================*/
void CBBoardAgent::ProcBoardTempStatusNotif(CMessage* const pcMsg)
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];
	
	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );
	
	TBrdPos tBrdPos;
	memset(&tBrdPos,0,sizeof(tBrdPos));
	if( m_pBBoardConfig != NULL )
	{
		tBrdPos = m_pBBoardConfig->GetBrdPosition();
	}
		
    if( FindAlarm( ALARM_MCU_BRD_TEMP_ABNORMAL, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) )
    {
        if( pcMsg->content[0] == BRD_STATUS_NORMAL )    // normal
        {
            if( !DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ))
			{
                brdagtlog("brdAgent: DeleteAlarm( AlarmCode = %lu) failed!\n", tAlarmData.dwBoardAlarmCode);
			}
            else
			{
				tAlarmMsg.wEventId = SVC_AGT_BRD_TEMPERATURE_STATUS_NOTIFY;
				tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = pcMsg->content[0];
				tAlarmMsg.abyAlarmContent[4] = 0;
				
				memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );
				
				// 告警表个数 [10/25/2011 chendaiwei]
				*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 
				
				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );				
			}
        }
    }
    else //no such alarm
    {
        if(  pcMsg->content[1] == BRD_STATUS_ABNORMAL )    //abnormal
        {
            if( !AddAlarm( ALARM_MCU_BRD_TEMP_ABNORMAL, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ))
			{
                brdagtlog("brdAgent: AddAlarm( AlarmCode = %lu) failed!\n", ALARM_MCU_BRD_TEMP_ABNORMAL);
			}
            else
			{						
				tAlarmMsg.wEventId = SVC_AGT_BRD_TEMPERATURE_STATUS_NOTIFY;
				tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = pcMsg->content[0];
				tAlarmMsg.abyAlarmContent[4] = 0;
				
				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg);
				
				memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );
				
				// 告警表个数 [10/25/2011 chendaiwei]
				*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 
				
				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
        }
    }

	return;
}

/*=============================================================================
函 数 名： ProcBoardCpuStatusNotif
功    能： 单板CPU占有率异常正常切换时，发送告警信息给MPC
算法实现： 
全局变量： 
参    数： CMessage* const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/10/25   4.0		chendaiwei            创建
=============================================================================*/
void CBBoardAgent::ProcBoardCpuStatusNotif(CMessage* const pcMsg)
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];
	
	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );
	
	TBrdPos tBrdPos;
	memset(&tBrdPos,0,sizeof(tBrdPos));
	if( m_pBBoardConfig != NULL )
	{
		tBrdPos = m_pBBoardConfig->GetBrdPosition();
	}
		
    if( FindAlarm( ALARM_MCU_BRD_CPU_ABNORMAL, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) )
    {
        if( pcMsg->content[0] == BRD_STATUS_NORMAL )    // normal
        {
            if( !DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ))
			{
                brdagtlog("brdAgent: DeleteAlarm( AlarmCode = %lu) failed!\n", tAlarmData.dwBoardAlarmCode);
			}
            else
			{
				tAlarmMsg.wEventId = SVC_AGT_BRD_CPU_STATUS_NOTIF;
				tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = pcMsg->content[0];
				tAlarmMsg.abyAlarmContent[4] = 0;
				
				memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );
				
				// 告警表个数 [10/25/2011 chendaiwei]
				*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 
				
				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );				
			}
        }
    }
    else //no such alarm
    {
        if(  pcMsg->content[1] == BRD_STATUS_ABNORMAL )    //abnormal
        {
            if( !AddAlarm( ALARM_MCU_BRD_CPU_ABNORMAL, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ))
			{
				brdagtlog("brdAgent: AddAlarm( AlarmCode = %lu) failed!\n", ALARM_MCU_BRD_CPU_ABNORMAL);
			}
            else
			{						
				tAlarmMsg.wEventId = SVC_AGT_BRD_CPU_STATUS_NOTIF;
				tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = pcMsg->content[0];
				tAlarmMsg.abyAlarmContent[4] = 0;
				
				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg);
				
				memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );
				
				// 告警表个数 [10/25/2011 chendaiwei]
				*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 
				
				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
        }
    }
	
	return;
}

/*=============================================================================
  函 数 名： MsgRegAck
  功    能： 注册成功
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  10/06/02    4.6         xueliang      重整
==============================================================================*/
void CBBoardAgent::MsgRegAck(CMessage* const pcMsg )
{
    if(NULL == pcMsg || NULL == m_pBBoardConfig)
    {
        OspPrintf(TRUE, FALSE, "The pointer can not be Null.(MsgRegAck)\n");
		return;
    }

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	BOOL32 bIsMpcActive = TRUE;
	TBrdRegAck tBrdRegAck;
	memcpy( &bIsMpcActive, cServMsg.GetMsgBody(), sizeof(BOOL32));
	memcpy( &tBrdRegAck, cServMsg.GetMsgBody()+sizeof(BOOL32), sizeof(TBrdRegAck) );

	printf("The other MPC is 0x%x:%d\n", tBrdRegAck.GetOtherMpcIp(), tBrdRegAck.GetOtherMpcPort());

	// 双链标志
	BOOL32 bIsDoubleLink = ( 0 != tBrdRegAck.GetOtherMpcIp() ) ? TRUE : FALSE;
	BOOL32 bSrcNodeA = TRUE;
	BOOL32 bNetParamChanged = FALSE;

	if(m_byIsMpu2SimuMpu)
	{
		printf("[MsgRegAck]m_byIsAnyBrdRegSuccess.%d m_byRegBrdTypeFlagA.%d m_byRegBrdTypeFlagB.%d\n",m_byIsAnyBrdRegSuccess,
			m_byRegBrdTypeFlagA,m_byRegBrdTypeFlagB);
		OspPrintf(TRUE,FALSE,"[MsgRegAck]m_byIsAnyBrdRegSuccess.%d m_byRegBrdTypeFlagA.%d m_byRegBrdTypeFlagB.%d\n",m_byIsAnyBrdRegSuccess,
			m_byRegBrdTypeFlagA,m_byRegBrdTypeFlagB);
	}

	// record IId
	if(pcMsg->srcnode == m_pBBoardConfig->m_dwDstMcuNodeA)
    {
		m_pBBoardConfig->m_dwDstMcuIIdA = pcMsg->srcid;

		if(m_byIsMpu2SimuMpu && !m_byIsAnyBrdRegSuccess)
		{
			m_byIsAnyBrdRegSuccess = 1;
			if(m_byRegBrdTypeFlagA == 0)
			{
				m_pBBoardConfig->m_tBoardPosition.byBrdID = 0x42;
			}
			else
			{
				m_pBBoardConfig->m_tBoardPosition.byBrdID = BRD_TYPE_MPU;
			}
			
			printf("[MsgRegAck]m_pBBoardConfig->m_tBoardPosition.byBrdId adjust to 0x%x\n",m_pBBoardConfig->m_tBoardPosition.byBrdID);
			OspPrintf(TRUE,FALSE,"[MsgRegAck]m_pBBoardConfig->m_tBoardPosition.byBrdId adjust to 0x%x\n",m_pBBoardConfig->m_tBoardPosition.byBrdID);
		}
	}
	else if(pcMsg->srcnode == m_pBBoardConfig->m_dwDstMcuNodeB)
    {
        m_pBBoardConfig->m_dwDstMcuIIdB = pcMsg->srcid;
		bSrcNodeA = FALSE;
		if(m_byIsMpu2SimuMpu && !m_byIsAnyBrdRegSuccess)
		{
			m_byIsAnyBrdRegSuccess = 1;
			if(m_byRegBrdTypeFlagB == 0)
			{
				m_pBBoardConfig->m_tBoardPosition.byBrdID = 0x42;
			}
			else
			{
				m_pBBoardConfig->m_tBoardPosition.byBrdID = BRD_TYPE_MPU;
			}
			
			printf("[MsgRegAck]m_pBBoardConfig->m_tBoardPosition.byBrdId adjust to 0x%x\n",m_pBBoardConfig->m_tBoardPosition.byBrdID);
			OspPrintf(TRUE,FALSE,"[MsgRegAck]m_pBBoardConfig->m_tBoardPosition.byBrdId adjust to 0x%x\n",m_pBBoardConfig->m_tBoardPosition.byBrdID);
		}
    }
	
	u16 wRegEvent = (bSrcNodeA ? BRDAGENT_REGISTERA_TIMER: BRDAGENT_REGISTERB_TIMER);
	KillTimer(wRegEvent);

	if( bIsDoubleLink )
	{
		u32 dwAlternativeNode	= bSrcNodeA ? m_pBBoardConfig->m_dwDstMcuNodeB: m_pBBoardConfig->m_dwDstMcuNodeA;
		u32 dwAlternativeIp		= bSrcNodeA ? m_pBBoardConfig->GetMpcIpB(): m_pBBoardConfig->GetMpcIpA();
		u16 wAlternativePort	= bSrcNodeA ? m_pBBoardConfig->GetMpcPortB(): m_pBBoardConfig->GetMpcPortA();

		if ( dwAlternativeIp != tBrdRegAck.GetOtherMpcIp() 
			|| wAlternativePort != tBrdRegAck.GetOtherMpcPort() )
		{
			OspPrintf(TRUE,FALSE,"[MsgRegAck]localothermpcip = %d,regack othermpcip:%d localothermpcport:%d,regack othermpcport:%d\n",dwAlternativeIp,tBrdRegAck.GetOtherMpcIp(),
				wAlternativePort,tBrdRegAck.GetOtherMpcPort());
			BOOL32 bNoSrcNodeA = !bSrcNodeA;
			UpdateIpPort(tBrdRegAck.GetOtherMpcIp(), tBrdRegAck.GetOtherMpcPort(), bNoSrcNodeA);
			bNetParamChanged = TRUE;
		}

		if( !OspIsValidTcpNode(dwAlternativeNode))
		{
			//if the alternative Node is invalid, then connect it using the Ip passed by Mcu
			ProcConnect();
		}
		else
		{	
			if(bNetParamChanged)
			{
				//if the alternative node is valid but the ip info doesn't match the ip passed by Mcu,
				//we disconnect it and reconnect using new ip.
				if ( !OspDisconnectTcpNode( dwAlternativeNode ) )
				{
					brdagtlog("[MsgRegAck] Disconnect alternative node.%d failed!\n", dwAlternativeNode);
					return;
				}
				OspPrintf(TRUE,FALSE,"[MsgRegAck]NetParamChanged,Disconnect!\n");
				ProcConnect();
			}
			else
			{
				//do nothing
			}
		}
	}

/*	
    if(pcMsg->srcnode == m_pBBoardConfig->m_dwDstMcuNodeA)
    {
		m_pBBoardConfig->m_dwDstMcuIIdA = pcMsg->srcid;
		KillTimer(BRDAGENT_REGISTERA_TIMER);
		
		if ( bIsDoubleLink )
		{
			// 记录另一块MPC的IP、PORT
			// [5/26/2010 xliang] 均以消息中的数据为准
			if ( m_pBBoardConfig->GetMpcIpB() != tBrdRegAck.GetOtherMpcIp() 
				|| m_pBBoardConfig->GetMpcPortB() != tBrdRegAck.GetOtherMpcPort() )
			{
				m_pBBoardConfig->SetMpcIpB(tBrdRegAck.GetOtherMpcIp());
				m_pBBoardConfig->SetMpcPortB(tBrdRegAck.GetOtherMpcPort());
				bNetParamChanged = TRUE;
			}
			// 开始向另一块板建链
			if ( ( bNetParamChanged 
				|| m_pBBoardConfig->m_dwDstMcuNodeB == INVALID_NODE
				|| m_pBBoardConfig->m_dwDstMcuIIdB == INVALID_INS)
				&& m_pBBoardConfig->GetMpcIpB() != 0
				)
			{
				// 先断开老的TCP链
				if ( OspIsValidTcpNode(m_pBBoardConfig->m_dwDstMcuNodeB) )
				{
					if ( !OspDisconnectTcpNode( m_pBBoardConfig->m_dwDstMcuNodeB ) )
					{
						brdagtlog("[MsgRegAck] Disconnect old nodeB is failed!\n");
						return;
					}
				}
				
				bRet = ConnectManager(m_pBBoardConfig->m_dwDstMcuNodeB, 
						m_pBBoardConfig->GetMpcIpB(), m_pBBoardConfig->GetMpcPortB());       //和MPC上的管理程序连接
				if( TRUE == bRet)
				{
					SetTimer( BRDAGENT_REGISTERB_TIMER, REGISTER_TIMEOUT );
				}
				else
				{
					SetTimer( BRDAGENT_CONNECT_MANAGERB_TIMER, CONNECT_MANAGER_TIMEOUT );
				}
			}
		} // if ( bIsDoubleLink )
    }
    else if(pcMsg->srcnode == m_pBBoardConfig->m_dwDstMcuNodeB)
    {
        m_pBBoardConfig->m_dwDstMcuIIdB = pcMsg->srcid;
        KillTimer(BRDAGENT_REGISTERB_TIMER);
    }
*/ 
	if( STATE_INIT == CurState() )
	{
        //guzh [2008/08/01] 非标准单板注册过程结束
        if (m_pBBoardConfig->m_tBoardPosition.byBrdLayer == BRD_LAYER_CUSTOM)
        {
            NEXTSTATE(STATE_NORMAL);
        }
        else
        {
            PostMsgToManager( BOARD_MPC_GET_CONFIG, (u8*)&m_pBBoardConfig->m_tBoardPosition, sizeof(TBrdPos) );		
            SetTimer( BRDAGENT_GET_CONFIG_TIMER, GET_CONFIG_TIMEOUT );	
        }
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
void CBBoardAgent::ProcGetConfigTimeOut( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
		// 重新发送取配置信息消息
		PostMsgToManager( BOARD_MPC_GET_CONFIG, (u8*)&m_pBBoardConfig->m_tBoardPosition, sizeof(TBrdPos) );		
		SetTimer( BRDAGENT_GET_CONFIG_TIMER, GET_CONFIG_TIMEOUT );	
		break;

	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when receive get configure!srcnode.%d\n", 
			CurState(), pcMsg != NULL?pcMsg->srcnode:0 );
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
void CBBoardAgent::ProcAlarmSyncReq( CMessage* const pcMsg )
{
	if (NULL == m_pBBoardConfig)
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(ProcAlarmSyncReq)\n");
		return;
	}

	u16 wLoop;
	u16 wCount=0, wLen;
	u8 abyAlarmBuf[ MAXNUM_BOARD_ALARM*sizeof(TBoardAlarmMsgInfo) + 10 ];
	TBoardAlarmMsgInfo* pAlarmMsgInfo;

	memset(abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	switch( CurState() ) 
	{
    case STATE_INIT:
	case STATE_NORMAL:
		// 先是自己的身份信息
		memcpy(abyAlarmBuf, &m_pBBoardConfig->m_tBoardPosition, sizeof(TBrdPos) );

		// 扫描自己的告警表
		pAlarmMsgInfo = (TBoardAlarmMsgInfo*)(abyAlarmBuf + sizeof(TBrdPos) + sizeof(u16));
		for( wLoop = 0; wLoop < m_dwBoardAlarmTableMaxNo; wLoop++ )
		{
			if( m_atBoardAlarmTable[wLoop].bExist )
			{
				memcpy(pAlarmMsgInfo, &m_atBoardAlarmTable[wLoop].tBoardAlarm, sizeof(TBoardAlarmMsgInfo) );
				pAlarmMsgInfo++;
				wCount++;
			}
		}

		// 告警个数
		*(u16*)( abyAlarmBuf + sizeof(TBrdPos) ) = htons(wCount); 
		wLen = sizeof(TBrdPos) + sizeof(u16) + wCount*sizeof(TBoardAlarmMsgInfo);

		PostMsgToManager( BOARD_MPC_ALARM_SYNC_ACK, abyAlarmBuf, wLen );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

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
void CBBoardAgent::ProcGetVersionReq( CMessage* const pcMsg )
{
	if (NULL == m_pBBoardConfig)
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(ProcGetVersionReq)\n");
		return;
	}

	s8 abyBuf[128];
	u16 wLen;

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
		//取版本信息
		{
		memset(abyBuf, 0, sizeof(abyBuf) );

		TBrdPos tBrdPosition = m_pBBoardConfig->GetBrdPosition();
		memcpy(abyBuf, &tBrdPosition,sizeof(tBrdPosition) );
		
		s8 achVersionBuf[128] = {0};
		strcpy(achVersionBuf, KDV_MCU_PREFIX);
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
			sprintf(achVersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
		}
		else
		{
			// for debug information
			sprintf(achVersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
		}

		wLen = sprintf(abyBuf+sizeof(tBrdPosition),achVersionBuf);
   
		/*
		switch(tBrdPosition.byBrdID)
		{
			//bas vmp
		case BRD_TYPE_MPU:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_MPU);
			break;
		case BRD_TYPE_MPU2:
		case BRD_TYPE_MPU2ECARD:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_MPU2);
			break;
		case BRD_TYPE_APU:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_MIXER);
			break;
			
			//mixer
		case BRD_TYPE_APU2:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_APU2);
			break;
		case BRD_TYPE_EAPU:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_EMIXER);
			break;
			
			//tvwall
		case BRD_TYPE_DEC5:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_TW);
			break;
		case BRD_TYPE_HDU:
		case BRD_TYPE_HDU_L:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_HDU);
			break;
		case BRD_TYPE_HDU2:
		//case BRD_TYPE_HDU2_L:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_HDU2);
			break;
			
			//cri,dri..
		case BRD_TYPE_CRI:
		case BRD_TYPE_CRI2:
		case BRD_TYPE_IS21:
		case BRD_TYPE_IS22:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_CRIAGENT);
			break;
		case BRD_TYPE_DRI:
		case BRD_TYPE_DRI2:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),VER_DRIAGENT);
			break;
			
		default:
			wLen = sprintf(abyBuf+sizeof(tBrdPosition),"Hardware %d| FPGA %d",
				BrdQueryHWVersion(), BrdQueryFPGAVersion() );
			break;
		}*/

		// [pengjie 2010/3/25]
		OspPrintf( TRUE, FALSE, "[Brd_Debug][ProcGetModuleInfoReq] id: %d, Layer: %d, Slot: %d \n",
			tBrdPosition.byBrdID,  tBrdPosition.byBrdLayer, tBrdPosition.byBrdSlot );
		printf( "[Brd_Debug][ProcGetModuleInfoReq] id: %d, Layer: %d, Slot: %d \n",
			tBrdPosition.byBrdID,  tBrdPosition.byBrdLayer, tBrdPosition.byBrdSlot );

		//发送给MANAGER响应
		PostMsgToManager( BOARD_MPC_GET_VERSION_ACK, (u8*)abyBuf, wLen+sizeof(TBrdPos)+1 );
		break;
		}
		
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
void CBBoardAgent::ProcGetModuleInfoReq( CMessage* const pcMsg )
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(ProcGetModuleInfoReq)!\n");
		return;
	}

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:		
		//发送给MANAGER响应
		{
		TBrdPos tBrdPosition = m_pBBoardConfig->GetBrdPosition();
		PostMsgToManager( BOARD_MPC_GET_MODULE_ACK, (u8*)&tBrdPosition, sizeof(tBrdPosition) );
		// [pengjie 2010/3/25]
		OspPrintf( TRUE, FALSE, "[Brd_Debug][ProcGetModuleInfoReq] id: %d, Layer: %d, Slot: %d \n",
			tBrdPosition.byBrdID,  tBrdPosition.byBrdLayer, tBrdPosition.byBrdSlot );
		printf( "[Brd_Debug][ProcGetModuleInfoReq] id: %d, Layer: %d, Slot: %d \n",
			tBrdPosition.byBrdID,  tBrdPosition.byBrdLayer, tBrdPosition.byBrdSlot );

		break;
		}
		
	default:
		log( LOGLVL_EXCEPTION, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
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
void CBBoardAgent::ProcBoardResetCmd( CMessage* const pcMsg )
{

    switch(CurState())
    {
    case STATE_IDLE:
    case STATE_INIT:
    case STATE_NORMAL:
        OspDelay(1000);		
        BrdHwReset();
        OspPrintf(TRUE, FALSE, "[ProcBoardResetCmd] Reboot local brd...\n");
        break;

    default:
        OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n",
                                pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState());
        break;
    }
	return;
}

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
====================================================================*/
void CBBoardAgent::ProcUpdateSoftwareCmd( CMessage* const pcMsg )
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
		if( m_pBBoardConfig->m_bUpdateSoftBoard )
		{
			s8 achFileName[256];
            s8 achTmpName[256];
            s8 *lpMsgBody = (s8*)(cServMsg.GetMsgBody()+sizeof(u8)+sizeof(u8));
			u8  byLen = 0;
            u16 wOffSet = 0;

			while( byTmpNum-- > 0 )
			{
                
                
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
                brdagtlog( "[ProcUpdateSoftwareCmd] filename: %s\n", achFileName);
				
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
				brdagtlog( "[ProcUpdateSoftwareCmd] updating file name is :%s\n", achFileName);
				
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
                s32 nNameOffBin = strlen(achFileName) - strlen(LINUXAPP_POSTFIX_BIN);
                s32 nNameOffImage = strlen(achFileName) - strlen(LINUXAPP_POSTFIX);
				s32 nNameOffLinux = strlen(achFileName) - strlen(LINUXOS_POSTFIX);
                
				printf("[ProcUpdateSoftwareCmd] start update, file.%s\n", achFileName+nNameOffBin);
				brdagtlog("[ProcUpdateSoftwareCmd] start update, file.%s\n", achFileName+nNameOffBin);
		#ifdef _LINUX12_				
				//判断是否bin文件
				if ( nNameOffBin > 0 && 0 == strcmp(achFileName+nNameOffBin, LINUXAPP_POSTFIX_BIN) )
				{
					printf("[ProcUpdateSoftwareCmd] update ram file.%s as bin\n", achFileName);
					brdagtlog("[ProcUpdateSoftwareCmd] update ram file.%s as bin\n", achFileName);
					
					s32 nRetCode = BrdCheckUpdatePackage(achFileName);
					if( nRetCode != OK )
					{
						LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,"[ProcUpdateSoftwareCmd] Board<T:%d,L:%d,S:%d>check package[%s] failed,error.%d!\n",
							m_pBBoardConfig->GetBrdPosition().byBrdID,
							m_pBBoardConfig->GetBrdPosition().byBrdLayer,
							m_pBBoardConfig->GetBrdPosition().byBrdSlot,
							achFileName, nRetCode);

						abyRet[byFileNum] = 0;
						if( CHK_PID_FAILURE == nRetCode )
						{
							abyRet[byFileNum] = 2;
						}

						//return;
						continue;
					}

					TBrdPos tBrdPosion = m_pBBoardConfig->GetBrdPosition();					
					if (BRD_TYPE_IS22 == tBrdPosion.byBrdID)
					{//IS2.2用BrdAllSysUpdate升级
						if ( OK != BrdAllSysUpdate(achFileName, 1) )
						{
							abyRet[byFileNum] = 0;
							printf("[ProcUpdateBrdVersion] BrdAllSysUpdate failed.\n");
							brdagtlog( "[ProcUpdateBrdVersion] BrdUpdateAppBin failed.\n");
						}
						else
						{
							printf("[ProcUpdateBrdVersion] BrdAllSysUpdate success.\n");
							brdagtlog( "[ProcUpdateBrdVersion] BrdUpdateAppBin success.\n");
						}
					}
					else
					{//其它板用BrdSysUpdate()升级
						if ( SYSTEM_UPDATE_SUCCESS != BrdSysUpdate(achFileName) )
						{
							abyRet[byFileNum] = 0;
							printf("[ProcUpdateBrdVersion] BrdUpdateAppBin failed.\n");
							brdagtlog( "[ProcUpdateBrdVersion] BrdUpdateAppBin failed.\n");
						}
						else
						{
							printf( "[ProcUpdateBrdVersion] BrdUpdateAppBin success.\n");
							brdagtlog( "[ProcUpdateBrdVersion] BrdUpdateAppBin success.\n");
						}
					}					
				}

				//判断是否image文件
                else 
		#endif					
				if (nNameOffImage > 0 && 0 == strcmp(achFileName+nNameOffImage, LINUXAPP_POSTFIX) )
                {
					printf("[ProcUpdateSoftwareCmd] update ram file.%s as image\n", achFileName);
					brdagtlog("[ProcUpdateSoftwareCmd] update ram file.%s as image\n", achFileName);

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
                else if (nNameOffLinux > 0 && 0 == strcmp(achFileName+nNameOffLinux, LINUXOS_POSTFIX) )
                {
					printf("[ProcUpdateSoftwareCmd] update ram file.%s as os\n", achFileName);
					brdagtlog("[ProcUpdateSoftwareCmd] update ram file.%s as os\n", achFileName);

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
              
#endif
				byFileNum ++;
///FIXME: WIndows处理                
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
            
            // [zw] [09/10/2008] 升级成功才重启。
            if ( UPDATE_NMS == cServMsg.GetChnIndex() )
            {
                s32 nIndex = 0;
                for( ; nIndex < byNum; nIndex ++ )
                {
                    if ( abyRet[nIndex] == 0 )
                    {
                        brdagtlog("[ProcUpdateBrdVersion] file.%d update failed !\n", nIndex);
                        return;
                    }
                }
                BrdHwReset();
            }
        }
		break;
		
	default:
		OspPrintf( TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
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
void CBBoardAgent::ProcTimeSyncCmd( CMessage* const pcMsg )
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
void CBBoardAgent::ProcBoardSelfTestCmd( CMessage* const pcMsg )
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

/*=============================================================================
函 数 名： TestingProcess
功    能： 
算法实现： 
全局变量： 
参    数： CMessage* const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/27   4.0			周广程                  创建
=============================================================================*/
void CBBoardAgent::TestingProcess( CMessage* const pcMsg )
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(TestingProcess)\n");
		return;
	}

	if( m_pBBoardConfig->m_bIsTest == FALSE )
		return ;
	
	m_pBBoardConfig->m_wLastEvent = pcMsg->event;
	if(pcMsg->content != NULL)
	{
		if( pcMsg->length < sizeof(m_pBBoardConfig->m_abyLastEventPara) )
        {
			memcpy(m_pBBoardConfig->m_abyLastEventPara, pcMsg->content, pcMsg->length );
        }
		else
        {
			memcpy(m_pBBoardConfig->m_abyLastEventPara, pcMsg->content, sizeof(m_pBBoardConfig->m_abyLastEventPara) );
        }
	}
	else
	{
		memset(m_pBBoardConfig->m_abyLastEventPara, 0, sizeof(m_pBBoardConfig->m_abyLastEventPara) );
	} 
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
void CBBoardAgent::ProcBitErrorTestCmd( CMessage* const pcMsg )
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
void CBBoardAgent::ProcGetStatisticsReq( CMessage* const pcMsg )
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
BOOL32 CBBoardAgent::PostMsgToManager( u16 wEvent, u8 * const pbyContent, u16 wLen )
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(PostMsgToManager)!\n");
		return FALSE;
	}

	if( OspIsValidTcpNode( m_pBBoardConfig->m_dwDstMcuNodeA ) )
	{
		post( m_pBBoardConfig->m_dwDstMcuIIdA, wEvent, pbyContent, wLen, m_pBBoardConfig->m_dwDstMcuNodeA );
		brdagtlog("Post message %u(%s) to Board manager A!\n", wEvent, ::OspEventDesc( wEvent ));
	}
	else
	{
		brdagtlog("BoardAgent: Post message %u(%s) to Board manager A ERROR!\n", wEvent, ::OspEventDesc( wEvent ));
	}

    if(OspIsValidTcpNode(m_pBBoardConfig->m_dwDstMcuNodeB))
    {
        post( m_pBBoardConfig->m_dwDstMcuIIdB, wEvent, pbyContent, wLen, m_pBBoardConfig->m_dwDstMcuNodeB );
		brdagtlog(" Post message %u(%s) to Board manager B!\n", wEvent, ::OspEventDesc( wEvent ));
    }
    else
    {
		brdagtlog( "BoardAgent: Post message %u(%s) to Board manager B ERROR!\n", wEvent, ::OspEventDesc( wEvent ));
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
	10/06/01	4.6			xueliang	  重整
====================================================================*/
void CBBoardAgent::ProcOspDisconnect( CMessage* const pcMsg )
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(ProcOspDisconnect)!\n");
		return;
	}

    u32 dwNode = *(u32*)pcMsg->content;
    if ( INVALID_NODE != dwNode )
    {
        OspDisconnectTcpNode(dwNode);
    }
    
    if(dwNode == m_pBBoardConfig->m_dwDstMcuNodeA)
    {
        FreeDataA();
    }
    else if(dwNode == m_pBBoardConfig->m_dwDstMcuNodeB)
    {
        FreeDataB();
    }

	printf("[ProcOspDisconnect]m_pBBoardConfig->m_dwDstMcuNodeA.%d m_pBBoardConfig->m_dwDstMcuNodeB.%d brdId.%d\n",m_pBBoardConfig->m_dwDstMcuNodeA,
		m_pBBoardConfig->m_dwDstMcuNodeB,m_pBBoardConfig->m_tBoardPosition.byBrdID);
	OspPrintf(TRUE,FALSE,"[ProcOspDisconnect]m_pBBoardConfig->m_dwDstMcuNodeA.%d m_pBBoardConfig->m_dwDstMcuNodeB.%d brdId.%d\n",m_pBBoardConfig->m_dwDstMcuNodeA,
		m_pBBoardConfig->m_dwDstMcuNodeB,m_pBBoardConfig->m_tBoardPosition.byBrdID);
	
	if(m_pBBoardConfig->m_dwDstMcuNodeA == INVALID_NODE
		&& m_pBBoardConfig->m_dwDstMcuNodeB == INVALID_NODE
		&& m_byIsAnyBrdRegSuccess)
	{
		printf("[ProcOspDisconnect]m_byIsAnyBrdRegSuccess set to 0!\n");
		OspPrintf(TRUE,FALSE,"[ProcOspDisconnect]m_byIsAnyBrdRegSuccess set to 0!\n");
		m_byIsAnyBrdRegSuccess = 0;
	}

	SetTimer( BRDAGENT_CONNECT_MANAGER_TIMER, CONNECT_MANAGER_TIMEOUT );
	
	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:

        if(INVALID_NODE == m_pBBoardConfig->m_dwDstMcuNodeA && INVALID_NODE == m_pBBoardConfig->m_dwDstMcuNodeB )
        {

		    BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF );
            NEXTSTATE( STATE_IDLE );
            OspDelay(5*1000);
        }
        break;
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when in (ProcOspDisconnect)!\n", CurState() );
		break;
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
void CBBoardAgent::ProcBoardMemeryStatus( CMessage* const pcMsg )
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[ALARM_MCU_OBJECT_LENGTH];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	if( FindAlarm( ALARM_MCU_MEMORYERROR, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) )
	{
		if( *( pcMsg->content ) == 0 )      //normal
		{
			if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
			{
				brdagtlog( "DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
				tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0], 
				tAlarmData.byBoardAlarmObject[1] );
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_pBBoardConfig->m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_pBBoardConfig->m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_pBBoardConfig->m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );
			
				memcpy(abyAlarmBuf, &m_pBBoardConfig->m_tBoardPosition,sizeof(TBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(TBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(TBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(TBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( *( pcMsg->content ) == 1 )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_MEMORYERROR, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog( "AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_pBBoardConfig->m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_pBBoardConfig->m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_pBBoardConfig->m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg);

				memcpy(abyAlarmBuf, &m_pBBoardConfig->m_tBoardPosition,sizeof(TBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(TBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(TBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(TBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	return;
}

/*====================================================================
    函数名      ：ProcBoardModuleStatus
    功能        ：单板模块状态告警
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CBBoardAgent::ProcBoardModuleStatus( CMessage* const pcMsg )
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[ALARM_MCU_OBJECT_LENGTH];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	if( FindAlarm( ALARM_MCU_MODULE_OFFLINE, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) )
	{
		if( 0 == *( pcMsg->content ))      //normal
		{
			if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
			{
				brdagtlog( "DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
				tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0], 
				tAlarmData.byBoardAlarmObject[1] );
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_pBBoardConfig->m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_pBBoardConfig->m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_pBBoardConfig->m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );
			
				memcpy(abyAlarmBuf, &m_pBBoardConfig->m_tBoardPosition,sizeof(TBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(TBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(TBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(TBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( 1 == *( pcMsg->content ) )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_MODULE_OFFLINE, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog( "AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_pBBoardConfig->m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_pBBoardConfig->m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_pBBoardConfig->m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg );
			
				memcpy(abyAlarmBuf, &m_pBBoardConfig->m_tBoardPosition,sizeof(TBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(TBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(TBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(TBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
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
void CBBoardAgent::ProcBoardFileSystemStatus( CMessage* const pcMsg )
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[ALARM_MCU_OBJECT_LENGTH];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	if( FindAlarm( ALARM_MCU_FILESYSTEMERROR, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) )
	{
		if( *( pcMsg->content ) == 0 )      //normal
		{
			if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
			{
				brdagtlog( "DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
				tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0], 
				tAlarmData.byBoardAlarmObject[1] );
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_pBBoardConfig->m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_pBBoardConfig->m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_pBBoardConfig->m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );
			
				memcpy(abyAlarmBuf, &m_pBBoardConfig->m_tBoardPosition,sizeof(TBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(TBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(TBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(TBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( *( pcMsg->content ) == 1 )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_FILESYSTEMERROR, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog( " AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = m_pBBoardConfig->m_tBoardPosition.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = m_pBBoardConfig->m_tBoardPosition.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = m_pBBoardConfig->m_tBoardPosition.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg );
			
				memcpy(abyAlarmBuf, &m_pBBoardConfig->m_tBoardPosition,sizeof(TBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(TBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(TBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(TBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
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
BOOL32 CBBoardAgent::AddAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData )
{
    u16    wLoop;

	/*找到一个空表项*/
    for( wLoop = 0; wLoop < m_dwBoardAlarmTableMaxNo; wLoop++ )
    {
        if( !m_atBoardAlarmTable[wLoop].bExist )
        {
            break;
        }
    }
    
    if( wLoop >= sizeof( m_atBoardAlarmTable ) / sizeof( m_atBoardAlarmTable[0] ) )
    {
        return( FALSE );
    }

	/*填写该告警表项*/
	u8 byLen1 = ALARM_MCU_OBJECT_LENGTH;
	u8 bylen2 = sizeof( m_atBoardAlarmTable[0].byBoardAlarmObject );
    m_atBoardAlarmTable[wLoop].dwBoardAlarmCode = dwAlarmCode;
    m_atBoardAlarmTable[wLoop].byBoardAlarmObjType = byObjType;
    memcpy( m_atBoardAlarmTable[wLoop].byBoardAlarmObject, byObject, min(byLen1, bylen2) );
    m_atBoardAlarmTable[wLoop].bExist = TRUE;
	
    // copy
    ptData->dwBoardAlarmSerialNo = wLoop + 1;
    ptData->dwBoardAlarmCode = m_atBoardAlarmTable[wLoop].dwBoardAlarmCode;
    ptData->byBoardAlarmObjType = m_atBoardAlarmTable[wLoop].byBoardAlarmObjType;
    memcpy( ptData->byBoardAlarmObject, m_atBoardAlarmTable[wLoop].byBoardAlarmObject, sizeof( ptData->byBoardAlarmObject ) );
	
    // increase maxno
    if( wLoop == m_dwBoardAlarmTableMaxNo )
    {
        m_dwBoardAlarmTableMaxNo++;
    }

	return TRUE;
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
BOOL32 CBBoardAgent::SetAlarmMsgInfo( u32 dwSerialNo, TBoardAlarmMsgInfo* const ptMsg )
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
BOOL32 CBBoardAgent::DeleteAlarm( u32 dwSerialNo )
{	
    s32   dwLoop;

    m_atBoardAlarmTable[dwSerialNo - 1].bExist = FALSE;
        
    if( dwSerialNo == m_dwBoardAlarmTableMaxNo )     //the last one
    {
        for( dwLoop = m_dwBoardAlarmTableMaxNo - 1; dwLoop >= 0; dwLoop-- )
        {
            if( m_atBoardAlarmTable[dwLoop].bExist )
            {
                break;
            }
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
BOOL32 CBBoardAgent::FindAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData )
{
    u16    wLoop;
    BOOL    bFind = FALSE;

	/*查找告警表中的每一项*/
    for( wLoop = 0; wLoop < m_dwBoardAlarmTableMaxNo; wLoop++ )
    {
        if( !m_atBoardAlarmTable[wLoop].bExist ) /*该项不存在，继续找下一项*/
        {
            continue;
        }

		/*告警码和告警对象类型要一致*/
        if( dwAlarmCode == m_atBoardAlarmTable[wLoop].dwBoardAlarmCode 
            && byObjType == m_atBoardAlarmTable[wLoop].byBoardAlarmObjType )
        {
			/*告警对象要一致*/
            switch( byObjType )
            {
			case ALARMOBJ_MCU:
				bFind = TRUE;
				break;

            case ALARMOBJ_MCU_LINK:
                if( byObject[0] == m_atBoardAlarmTable[wLoop].byBoardAlarmObject[0] 
                    && byObject[1] == m_atBoardAlarmTable[wLoop].byBoardAlarmObject[1] )
                {
                    bFind = TRUE;
                }
                break;

            default:
                brdagtlog( " Wrong AlarmObjType in FindAlarm()!\n" );
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
void CBBoardAgent::ProcBoardGetCfgTest( CMessage* const pcMsg )
{
/*	STRUCT_mcueqpMpEntry tMpCfg;

	tMpCfg = g_cBrdAgentApp.m_tMpConfig;

	tMpCfg.mcueqpMpEntConnMcuPort = htons(tMpCfg.mcueqpMpEntConnMcuPort);
	tMpCfg.mcueqpMpEntConnMcuIp = htonl(tMpCfg.mcueqpMpEntConnMcuIp);
	tMpCfg.mcueqpMpEntIpAddr = htonl(tMpCfg.mcueqpMpEntIpAddr);
	
	SetSyncAck( &tMpCfg, sizeof(tMpCfg) );*/
	OspPrintf(TRUE,FALSE,"[ProcBoardGetCfgTest] srcnode.%d\n",pcMsg!=NULL ? pcMsg->srcnode:0);
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
void CBBoardAgent::ProcBoardGetAlarm( CMessage* const pcMsg )
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(ProcBoardGetAlarm).srcnode.%d\n",pcMsg!=NULL?pcMsg->srcnode:0);
		return;
	}

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
			*pbyBuf = m_pBBoardConfig->m_tBoardPosition.byBrdLayer;
			pbyBuf++;
			*pbyBuf = m_pBBoardConfig->m_tBoardPosition.byBrdSlot;
			pbyBuf++;
			*pbyBuf = m_pBBoardConfig->m_tBoardPosition.byBrdID;
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
    函数名      ：ProcBoardGetLastManagerCmd
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
void CBBoardAgent::ProcBoardGetLastManagerCmd( CMessage* const pcMsg )
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(ProcBoardGetLastManagerCmd).srcnode.%d\n",pcMsg!=NULL?pcMsg->srcnode:0);
		return;
	}

	u8 abyBuf[128];

	//直接返回最后一条消息的相关信息
	*(u16*)abyBuf = htons(m_pBBoardConfig->m_wLastEvent);
	memcpy(abyBuf+2, m_pBBoardConfig->m_abyLastEventPara, sizeof(m_pBBoardConfig->m_abyLastEventPara) );
	SetSyncAck( abyBuf, sizeof(u16)+sizeof(m_pBBoardConfig->m_abyLastEventPara) );
	return;
}

/*=============================================================================
函 数 名： FreeDataA
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/28   4.0			周广程                  创建
=============================================================================*/
void CBBoardAgent::FreeDataA(void)
{
	if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(FreeDataA)\n");
		return;
	}

	m_pBBoardConfig->m_dwDstMcuNodeA = INVALID_NODE;
	m_pBBoardConfig->m_dwDstMcuIIdA  = INVALID_INS;
    return;
}

/*=============================================================================
函 数 名： FreeDataB
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/28   4.0			周广程                  创建
=============================================================================*/
void CBBoardAgent::FreeDataB(void)
{
    if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(FreeDataB)\n");
		return;
	}

    m_pBBoardConfig->m_dwDstMcuNodeB = INVALID_NODE;
    m_pBBoardConfig->m_dwDstMcuIIdB  = INVALID_INS;
    return;
}

/*=============================================================================
函 数 名： brdagtlog
功    能： 
算法实现： 
全局变量： 
参    数： char * fmt
           ...
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/28   4.0			周广程                  创建
=============================================================================*/
/*lint -save -e438 -e530*/
void CBBoardAgent::brdagtlog(char * fmt, ...)
{
    if ( NULL == m_pBBoardConfig )
	{
		OspPrintf(TRUE, FALSE, "The pointer can not be Null.(brdagtlog)\n");
		return;
	}

	va_list argptr;
	va_start(argptr, fmt);
    m_pBBoardConfig->brdlog(fmt, argptr);
	va_end(argptr);
}
/*lint -restore*/

/*====================================================================
函数名      : AddBrdAgtEventStr
功能        : 添加单板代理消息字符串
算法实现    : 
引用全局变量: 无
输入参数说明: 
返回值说明  : 无
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人      修改内容
2012/05/05  4.7         liaokang      创建
====================================================================*/
void AddBrdAgtEventStr()
{    
/*lint -save -e537*/
#ifdef OSPEVENT
#undef OSPEVENT
#endif
#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )

#ifdef _EV_AGTSVC_H_
    #undef _EV_AGTSVC_H_
    #include "evagtsvc.h"
#else
#include "evagtsvc.h"
#endif
/*lint -restore*/
}

// API void byebye()
// {
//     OspPost( MAKEIID(AID_MCU_BRDAGENT,1), MPC_BOARD_RESET_CMD );
//     
// }
