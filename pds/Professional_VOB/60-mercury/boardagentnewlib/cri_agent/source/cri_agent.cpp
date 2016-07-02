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
#include "eqplib.h"
#include "cri_agent.h"

#ifdef _VXWORKS_
#include "timers.h"
#include <dirent.h>
#endif

CBrdAgentApp	g_cBrdAgentApp;	//单板代理应用实例

extern SEMHANDLE g_semCRI;

//构造函数
CBoardAgent::CBoardAgent()
{
    m_dwBrdSlaveSysNode = INVALID_NODE;
    m_dwBrdSlaveSysIId = INVALID_INS;
    memset( &m_tBrdSlaveSysLedState, 2, sizeof(TBrdLedState) );         // 灯状态 灭
#ifdef _LINUX12_   // 目前仅brdwrapper.h中有SWEthInfo
    memset( &m_tBrdSlaveSysEthPortState, 3, sizeof( SWEthInfo ) ); // 网口状态 未知
#endif //end _LINUX12_
	return;
}

//析构函数
CBoardAgent::~CBoardAgent()
{
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
		OspPrintf( TRUE, FALSE,  "[InstanceEntry] The received msg's pointer in the msg entry is NULL!");
		return;
	}

/*	CriLog("%u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event));*/    
    LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, 
        "[InstanceEntry] %u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch( pcMsg->event )
	{
	// 启动流程消息
// 	case OSP_POWERON:		                    //单板启动
// 	case BRDAGENT_CONNECT_MANAGERA_TIMER:       // 连接管理程序超时
//     case BRDAGENT_CONNECT_MANAGERB_TIMER:
// 	case BRDAGENT_REGISTERA_TIMER:		        // 注册时间超时
//     case BRDAGENT_REGISTERB_TIMER:		        // 注册时间超时
// 	case MPC_BOARD_REG_ACK:                     //注册应答消息
// 	case MPC_BOARD_REG_NACK:			        //注册否定应答消息
// 	case BRDAGENT_GET_CONFIG_TIMER:             //等待配置应答消息超时
// 	// MPC管理消息
// 	case MPC_BOARD_ALARM_SYNC_REQ:              //管理程序的告警同步请求
// 	case MPC_BOARD_BIT_ERROR_TEST_CMD:          //单板误码测试命令
// 	case MPC_BOARD_TIME_SYNC_CMD:               //单板时间同步命令
// 	case MPC_BOARD_SELF_TEST_CMD:               //单板自测命令
// 	case MPC_BOARD_RESET_CMD:                   //单板重启动命令
// 	case MPC_BOARD_UPDATE_SOFTWARE_CMD:         //软件更新命令
// 	case MPC_BOARD_GET_STATISTICS_REQ:          //获取单板的统计信息
// 	case MPC_BOARD_GET_VERSION_REQ:             //获取单板的版本信息
// 	case MPC_BOARD_GET_MODULE_REQ:              //获取单板的模块信息	
// 	// 以下是告警处理的相关消息
// 	case SVC_AGT_MEMORY_STATUS:                 //内存状态改变
// 	case SVC_AGT_FILESYSTEM_STATUS:             //文件系统状态改变
// 	// 以下是测试用消息
// 	case BOARD_MPC_CFG_TEST:
// 	case BOARD_MPC_GET_ALARM:
// 	case BOARD_MPC_MANAGERCMD_TEST:
// 	// 断链
// 	case OSP_DISCONNECT:
// 		CBBoardAgent::InstanceEntry(pcMsg);
// 		break;

	case MPC_BOARD_GET_CONFIG_ACK:              //取配置信息应答消息
		ProcBoardGetConfigAck( pcMsg );
		break;
		
	case MPC_BOARD_GET_CONFIG_NACK:             //取配置信息否定应答
		ProcBoardGetConfigNAck( pcMsg );
		break;

	case MPC_BOARD_LED_STATUS_REQ:
		ProcLedStatusReq( pcMsg );              // 取面板灯状态 [05/04/2012 liaokang]
		break;
        
    case MPC_BOARD_ETHPORT_STATUS_REQ:
        ProcEthPortStatusReq( pcMsg );          // 取网口状态 [05/04/2012 liaokang]
        break;

	case BOARD_LED_STATUS:
		ProcBoardLedStatus( pcMsg );            // 主系统led状态
		break;

    case BOARD_ETHPORT_STATUS:
        ProcBoardEthPortStatus( pcMsg );        // 主系统网口状态
		break;        

	case BOARD_MODULE_STATUS:
		ProcBoardModuleStatus( pcMsg );
		break;

	case MCU_BOARD_CONFIGMODIFY_NOTIF:
		ProcBoardConfigModify( pcMsg );
		break;

     // 支持从系统（IS2.2 8313）[05/04/2012 liaokang]
    case BRDSYS_SLAVE_MASTER_REG_REQ:
        ProcBrdSlaveSysRegReq( pcMsg );         // 从系统（IS2.2 8313） 注册
        break;

    case BRDSYS_SLAVE_MASTER_LED_STATUS_NOTIFY:
        ProcBrdSlaveSysLedStatus( pcMsg );      // 从系统（IS2.2 8313） led状态
        break;
    
    case BRDSYS_SLAVE_MASTER_ETHPORT_STATUS_NOTIFY:
        ProcBrdSlaveSysEthPortStatus( pcMsg );  // 从系统（IS2.2 8313） 网口状态
        break;

    case OSP_DISCONNECT: 
        ProcDisconnect( pcMsg );                // 断链处理
		break;

		//Is2.2须在子类中重启8313单板[6/8/2013 chendaiwei]
#if defined(_IS22_)
	case MPC_BOARD_RESET_CMD:                   //单板重启动命令
		ProcBoardIS22ResetCmd( pcMsg );
		break;
#endif

	default:
		CBBoardAgent::InstanceEntry(pcMsg);
		break;
// 		OspPrintf( TRUE, FALSE,  "[InstanceEntry] receive unknown msg %d<%s> criAgent! \n",
//                                   pcMsg->event, OspEventDesc(pcMsg->event) );
	
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
		OspPrintf(TRUE, FALSE,  "BoardAgent: Wrong state %u when receive get config Nack!srcNode.%d\n", 
			CurState(),pcMsg!=NULL?pcMsg->srcnode:0 );
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
	u8	byPeriCount;
	u8	byPeriType;
	u16	wLoop, wIndex;
	TEqpPrsEntry*			ptPrsCfg;		// Prs的配置信息

	switch( CurState() ) 
	{
	case STATE_INIT:
		NEXTSTATE( STATE_NORMAL );
		byPeriCount = pcMsg->content[0]; // 先是外设个数
		wIndex = 1;

		for( wLoop = 0; wLoop < byPeriCount; wLoop++ )
		{			
			byPeriType = pcMsg->content[wIndex++]; // 先是外设类型
			
			switch( byPeriType ) // 设置该单板相应的配置信息
			{
			case EQP_TYPE_PRS:
				ptPrsCfg = (TEqpPrsEntry*)(pcMsg->content+wIndex);
                ptPrsCfg->SetMcuIp(g_cBrdAgentApp.GetMpcIpA());
				wIndex += sizeof(TEqpPrsEntry);
				g_cBrdAgentApp.SetPrsConfig(ptPrsCfg);
                CriLog( " The Cri board run the Prs module\n");
				break;

			default:
				break;
			}
		}
		
		// 以后是单板IP        
        CriLog( "g_cBrdAgentApp.SetBrdIpAddr(0x%x)-wIndex = %d\n",
                            ntohl(*(u32*)&pcMsg->content[wIndex+2]), wIndex);

		g_cBrdAgentApp.SetBrdIpAddr( ntohl(  *(u32*)&pcMsg->content[wIndex+2] ) );
		
		if( pcMsg->content[ wIndex + 6 ] == 0 )// 以后是是否使用看门狗
		{
			SysRebootDisable( );
		}
		else
		{
			SysRebootEnable( );
		}

		BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );// 将MLINK灯点亮
		OspSemGive( g_semCRI );
		
		KillTimer( BRDAGENT_GET_CONFIG_TIMER );
		
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong state %u when receive get config Ack!\n", 
			CurState() );
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
    u8  abyLedBuf[ MAX_BOARD_LED_NUM + 1 ]={0};    // led BUF  最大32个灯
    u8  byLedCount = 0;                        // led个数
    // abyBuf结构：TBrdPos + byLedCount + abyLedBuf
    u8  abyBuf[ sizeof(TBrdPos) + sizeof(u8) + MAX_BOARD_LED_NUM + 1 ] = {0};  // 消息BUF
    u16 dwMsgLen = 0;                         // 消息长度

    // 获取主系统led状态
    TBrdLedState tBrdLedState;
    BrdQueryLedState( &tBrdLedState ); 
    memcpy( &m_tBrdMasterSysLedState, &tBrdLedState, sizeof(TBrdLedState) );
    
    // 单板身份结构
    TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );                       

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
        {

#if (defined _LINUX_ && defined _LINUX12_) // CRI2 / IS2.2
        
        if ( BRD_TYPE_IS22 == tBrdPos.byBrdID ) // IS2.2
        {
            // led count 及 BUF （主系统8548 及 从系统8313）
            byLedCount = sizeof(TBrdIS3MPC8548LedDesc) + sizeof(TBrdIS3MPC8313LedDesc);
            memcpy(abyLedBuf, &m_tBrdMasterSysLedState.nlunion.tBrdIS2XMpc8548LedState, sizeof(TBrdIS3MPC8548LedDesc));
            memcpy(abyLedBuf + sizeof(TBrdIS3MPC8548LedDesc), &m_tBrdSlaveSysLedState.nlunion.tBrdIS2XMpc8313LedState, sizeof(TBrdIS3MPC8313LedDesc));
        }
        else //CRI2
        {
            // led count 及 BUF
            byLedCount = sizeof(TBrdCRI2LedDesc);
            memcpy(abyLedBuf, &m_tBrdMasterSysLedState.nlunion.tBrdCRI2LedState, byLedCount);
        }

#else // CRI
        
        // led count 及 BUF
        byLedCount = sizeof(TBrdCRILedStateDesc);
        memcpy(abyLedBuf, &m_tBrdMasterSysLedState.nlunion.tBrdCRILedState, byLedCount);

#endif

        memcpy(abyBuf+sizeof(TBrdPos), &byLedCount, sizeof(u8) );         // led个数
        memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), abyLedBuf, byLedCount); // led BUF
        dwMsgLen = sizeof(TBrdPos) + sizeof(u8) + byLedCount;             // 消息长度
        PostMsgToManager( BOARD_MPC_LED_STATUS_ACK, abyBuf, dwMsgLen);    // ACK

        // 打印led状态
        s8 abyPrintLedState[256] = {0};        
        abyPrintLedState[255] = '\0';
        s8 *pBuff = abyPrintLedState;        
        pBuff += sprintf(pBuff, "[ProcLedStatusReq] Led Num:%d [", byLedCount);
        u32 dwIndex = 0;        
        for (dwIndex = 0; dwIndex < byLedCount; ++dwIndex)
        {
            pBuff += sprintf(pBuff, " %d ", *(abyLedBuf + dwIndex));
        }
        pBuff += sprintf(pBuff, "]\n");
        LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, abyPrintLedState );
   
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
    函数名      ：ProcBoardLedStatus
    功能        ：Led状态改变消息处理(主系统)
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
    12/05/04    4.7         liaokang      支持IS2.2从系统
====================================================================*/
void CBoardAgent::ProcBoardLedStatus( CMessage* const pcMsg )
{
    // 比较 维护主系统led状态
	TBrdLedState tLedState;
	memcpy( &tLedState, pcMsg->content, sizeof(TBrdLedState) );
    if( memcmp(&m_tBrdMasterSysLedState, &tLedState, sizeof(TBrdLedState)) == 0 )
    {
        return; 
    }
    memcpy( &m_tBrdMasterSysLedState, &tLedState, sizeof(TBrdLedState) );

    u8  abyLedBuf[ MAX_BOARD_LED_NUM + 1 ]={0};    // led BUF  最大32个灯
    u8  byLedCount = 0;                        // led个数
    // abyBuf结构：TBrdPos + byLedCount + abyLedBuf
    u8  abyBuf[ sizeof(TBrdPos) + sizeof(u8) + MAX_BOARD_LED_NUM + 1 ] = {0};  // 消息BUF
    u16 dwMsgLen = 0;                        // 消息长度

    // 单板身份结构
    TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) ); 

#if (defined _LINUX_ && defined _LINUX12_) // CRI2 / IS2.2

    if ( BRD_TYPE_IS22 == tBrdPos.byBrdID ) // IS2.2
    {
        // led count 及 BUF （主系统8548 及 从系统8313）
        byLedCount = sizeof(TBrdIS3MPC8548LedDesc) + sizeof(TBrdIS3MPC8313LedDesc);
        memcpy(abyLedBuf, &m_tBrdMasterSysLedState.nlunion.tBrdIS2XMpc8548LedState, sizeof(TBrdIS3MPC8548LedDesc));
        memcpy(abyLedBuf + sizeof(TBrdIS3MPC8548LedDesc), &m_tBrdSlaveSysLedState.nlunion.tBrdIS2XMpc8313LedState, sizeof(TBrdIS3MPC8313LedDesc));
    }
    else //CRI2
    {
        // led count 及 BUF
        byLedCount = sizeof(TBrdCRI2LedDesc);
        memcpy(abyLedBuf, &m_tBrdMasterSysLedState.nlunion.tBrdCRI2LedState, byLedCount);
    }

#else // CRI

    // led count 及 BUF
    byLedCount = sizeof(TBrdCRILedStateDesc);
    memcpy(abyLedBuf, &m_tBrdMasterSysLedState.nlunion.tBrdCRILedState, byLedCount);

#endif

    memcpy(abyBuf+sizeof(TBrdPos), &byLedCount, sizeof(u8) );         // led个数
    memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), abyLedBuf, byLedCount); // led BUF
	dwMsgLen = sizeof(TBrdPos) + sizeof(u8) + byLedCount;             // 消息长度
    PostMsgToManager( BOARD_MPC_LED_STATUS_NOTIFY, abyBuf, dwMsgLen);

    // 打印led状态
    s8 abyPrintLedState[256] = {0};        
    abyPrintLedState[255] = '\0';
    s8 *pBuff = abyPrintLedState;        
    pBuff += sprintf(pBuff, "[ProcBoardLedStatus] Led Num:%d [", byLedCount);
    u32 dwIndex = 0;        
    for (dwIndex = 0; dwIndex < byLedCount; ++dwIndex)
    {
        pBuff += sprintf(pBuff, " %d ", *(abyLedBuf + dwIndex));
    }
    pBuff += sprintf(pBuff, "]\n");
    LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, abyPrintLedState );

    return;
}

/*====================================================================
函数名      ：ProcEthPortStatusReq
功能        ：取网口状态（目前支持IS2.2，后续优化需修改）
算法实现    ：
引用全局变量：
输入参数说明：CMessage * const pcMsg, 传入的消息
返回值说明  ：void
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
12/05/04    4.7         liaokang       创建
====================================================================*/
void CBoardAgent::ProcEthPortStatusReq( CMessage* const pcMsg )
{
    /*
    根据需求，暂时只传IS2.2 双工状态给MCU，后续修改需与mcuagent一起修改
    NMS侧 双工 表示：1 半双工 2 全双工 3 未知
    接口  双工表示： 0 半双工 1 全双工
    */
    
    u8  abyEthPortBuf[ MAX_BOARD_ETHPORT_NUM + 1 ]={0};    // EthPort BUF
    u8  byEthPortCount = 0;                            // EthPort个数
    // abyBuf结构：TBrdPos + byEthPortCount + EthPortBuf
    u8  abyBuf[ sizeof(TBrdPos) + sizeof(u8) + MAX_BOARD_ETHPORT_NUM + 1 ] = {0};  // 消息BUF
    u16 dwMsgLen = 0;                              // 消息长度  
   
    // 取前网口：0 的网口状态
    u8  byEthId = 0;       // 网口
    u8  byLink =0;         // link
    u8  byEthAutoNeg = 0;  // 协商状态
    u8  byEthDuplex = 0;   // 双工状态
    u32 dwEthSpeed = 0;    // 接口速率Mbps
#ifdef _LINUX12_   // 目前仅brdwrapper.h中有TBrdEthInfo
    TBrdEthInfo  tBrdFrontEthPortState;
    // byLink: 0-link down, 1-link up 有链接再调用 BrdGetEthNegStat，否则刷错误打印
    BrdGetEthLinkStat( byEthId, &byLink );
    if ( 1 == byLink )
    {
        BrdGetEthNegStat( byEthId, &byEthAutoNeg, &byEthDuplex, &dwEthSpeed);
    }
    tBrdFrontEthPortState.Link = byLink;
    tBrdFrontEthPortState.AutoNeg = byEthAutoNeg;
    tBrdFrontEthPortState.Duplex = byEthDuplex;
    tBrdFrontEthPortState.Speed = dwEthSpeed;    
    memcpy( &m_tBrdMasterSysFrontEthPortState, &tBrdFrontEthPortState, sizeof(TBrdEthInfo) );
#endif //end _LINUX12_

    // 单板身份结构
    TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) ); 

    switch( CurState() ) 
    {
    case STATE_INIT:
    case STATE_NORMAL:
        {
#if (defined _LINUX_ && defined _LINUX12_) // CRI2 / IS2.2

            if ( BRD_TYPE_IS22 == tBrdPos.byBrdID ) // IS2.2
            {
                // 8548 前网口 1个 + 8313 前网口 2个 + 8313 后网口 8个
                byEthPortCount = 1 + FRONT_PANE_ETH_MAXNUM + BACK_PANE_ETH_MAXNUM;
                u8 byDuplexToNms = 0; // 驱动组提供的双工状态转化为网管组的双工状态
                u8 byLoop = 0;
                // EthPort BUF
                // 前面板前面板第一个网口 G2
                if ( 0 == m_tBrdMasterSysFrontEthPortState.Link )
                {
                    byDuplexToNms = 3;
                }
                else
                {
                    byDuplexToNms = m_tBrdMasterSysFrontEthPortState.Duplex + 1;
                }
                memcpy( abyEthPortBuf, &byDuplexToNms, sizeof(u8)); 
                
                // 前面板第二个第三个网口 G0 G1
                for ( byLoop = 0; byLoop< FRONT_PANE_ETH_MAXNUM; byLoop++ )
                {
                    if ( 0 == m_tBrdSlaveSysEthPortState.front_eth_info[byLoop].Link )
                    {
                        byDuplexToNms = 3;
                    }
                    else
                    {
                        byDuplexToNms = m_tBrdSlaveSysEthPortState.front_eth_info[byLoop].Duplex + 1;
                    }        
                    memcpy( abyEthPortBuf + sizeof(u8) * ( 1 + byLoop ), &byDuplexToNms, sizeof(u8));
                }
                
                // 后面板link0-7
                for ( byLoop = 0; byLoop< BACK_PANE_ETH_MAXNUM; byLoop++ )
                {
                    if ( 0 == m_tBrdSlaveSysEthPortState.back_eth_info[byLoop].Link )
                    {
                        byDuplexToNms = 3;
                    }
                    else
                    {
                        byDuplexToNms = m_tBrdSlaveSysEthPortState.back_eth_info[byLoop].Duplex + 1;
                    }        
                    memcpy( abyEthPortBuf + sizeof(u8) * ( FRONT_PANE_ETH_MAXNUM + 1 + byLoop), &byDuplexToNms, sizeof(u8));
                } 
            }
            else //CRI2
            {
                // 预留、暂时不做处理
            }

#else // CRI
            
            // 预留、暂时不做处理

#endif

            memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );                       // 单板身份结构
            memcpy(abyBuf+sizeof(TBrdPos), &byEthPortCount, sizeof(u8) );     // EthPort个数
            memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), abyEthPortBuf, byEthPortCount); // EthPort BUF
            dwMsgLen = sizeof(TBrdPos) + sizeof(u8) + byEthPortCount;         // 消息长度 
            PostMsgToManager( BOARD_MPC_ETHPORT_STATUS_ACK, abyBuf, dwMsgLen);// ACK

            // 打印EthPort状态
            s8 abyPrintEthPortState[256] = {0};        
            abyPrintEthPortState[255] = '\0';
            s8 *pBuff = abyPrintEthPortState;        
            pBuff += sprintf(pBuff, "[ProcEthPortStatusReq] EthPort Num:%d [", byEthPortCount);
            u32 dwIndex = 0;        
            for (dwIndex = 0; dwIndex < byEthPortCount; ++dwIndex)
            {
                pBuff += sprintf(pBuff, " %d ", *(abyEthPortBuf + dwIndex));
            }
            pBuff += sprintf(pBuff, "]\n");
            LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, abyPrintEthPortState );

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
函数名      ：ProcBoardEthPortStatus
功能        ：网口状态改变（主系统）
算法实现    ：
引用全局变量：
输入参数说明：CMessage * const pcMsg, 传入的消息
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
12/05/04    4.7         liaokang       创建
====================================================================*/
void CBoardAgent::ProcBoardEthPortStatus( CMessage* const pcMsg )
{
    /*
    根据需求，暂时只传IS2.2 双工状态给MCU，后续修改需与mcuagent一起修改
    NMS侧 双工 表示：1 半双工 2 全双工 3 未知
    接口  双工表示： 0 半双工 1 全双工
    */

#ifdef _LINUX12_   // 目前仅brdwrapper.h中有TBrdEthInfo
    // 比较 维护主系统网口状态
    TBrdEthInfo tBrdFrontEthPortState;
    memcpy( &tBrdFrontEthPortState, pcMsg->content, sizeof(TBrdEthInfo) );
    if( memcmp(&m_tBrdMasterSysFrontEthPortState, &tBrdFrontEthPortState, sizeof(TBrdEthInfo)) == 0 )
    {
        return; 
    }
    memcpy( &m_tBrdMasterSysFrontEthPortState, &tBrdFrontEthPortState, sizeof(TBrdEthInfo) );
#endif //end _LINUX12_
    
    u8  abyEthPortBuf[ MAX_BOARD_ETHPORT_NUM + 1 ]={0};    // EthPort BUF
    u8  byEthPortCount = 0;                            // EthPort个数
    // abyBuf结构：TBrdPos + byEthPortCount + EthPortBuf
    u8  abyBuf[ sizeof(TBrdPos) + sizeof(u8) + MAX_BOARD_ETHPORT_NUM + 1 ] = {0};  // 消息BUF
    u16 dwMsgLen = 0;                              // 消息长度

    // 单板身份结构
    TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
    memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) ); 
    
#if (defined _LINUX_ && defined _LINUX12_) // CRI2 / IS2.2
    
    if ( BRD_TYPE_IS22 == tBrdPos.byBrdID ) // IS2.2
    {
        // 8548 前网口 1个 + 8313 前网口 2个 + 8313 后网口 8个
        byEthPortCount = 1 + FRONT_PANE_ETH_MAXNUM + BACK_PANE_ETH_MAXNUM;
        u8 byDuplexToNms = 0; // 驱动组提供的双工状态转化为网管组的双工状态 
        u8 byLoop = 0;
        // EthPort BUF
        // 前面板前面板第一个网口 G2
        if ( 0 == m_tBrdMasterSysFrontEthPortState.Link )
        {
            byDuplexToNms = 3;
        }
        else
        {
            byDuplexToNms = m_tBrdMasterSysFrontEthPortState.Duplex + 1;
        }
        memcpy( abyEthPortBuf, &byDuplexToNms, sizeof(u8)); 
        
        // 前面板第二个第三个网口 G0 G1
        for ( byLoop = 0; byLoop< FRONT_PANE_ETH_MAXNUM; byLoop++ )
        {
            if ( 0 == m_tBrdSlaveSysEthPortState.front_eth_info[byLoop].Link )
            {
                byDuplexToNms = 3;
            }
            else
            {
                byDuplexToNms = m_tBrdSlaveSysEthPortState.front_eth_info[byLoop].Duplex + 1;
            }        
            memcpy( abyEthPortBuf + sizeof(u8) * ( 1 + byLoop ), &byDuplexToNms, sizeof(u8));
        }
        
        // 后面板link0-7
        for ( byLoop = 0; byLoop< BACK_PANE_ETH_MAXNUM; byLoop++ )
        {
            if ( 0 == m_tBrdSlaveSysEthPortState.back_eth_info[byLoop].Link )
            {
                byDuplexToNms = 3;
            }
            else
            {
                byDuplexToNms = m_tBrdSlaveSysEthPortState.back_eth_info[byLoop].Duplex + 1;
            }        
            memcpy( abyEthPortBuf + sizeof(u8) * ( FRONT_PANE_ETH_MAXNUM + 1 + byLoop), &byDuplexToNms, sizeof(u8));
        } 

        memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );                       // 单板身份结构
        memcpy(abyBuf+sizeof(TBrdPos), &byEthPortCount, sizeof(u8) );     // EthPort个数
        memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), abyEthPortBuf, byEthPortCount); // EthPort BUF
        dwMsgLen = sizeof(TBrdPos) + sizeof(u8) + byEthPortCount;         // 消息长度 
        PostMsgToManager( BOARD_MPC_ETHPORT_STATUS_NOTIFY, abyBuf, dwMsgLen);// ACK

        // 打印EthPort状态
        s8 abyPrintEthPortState[256] = {0};        
        abyPrintEthPortState[255] = '\0';
        s8 *pBuff = abyPrintEthPortState;        
        pBuff += sprintf(pBuff, "[ProcBoardEthPortStatus] EthPort Num:%d [", byEthPortCount);
        u32 dwIndex = 0;        
        for (dwIndex = 0; dwIndex < byEthPortCount; ++dwIndex)
        {
            pBuff += sprintf(pBuff, " %d ", *(abyEthPortBuf + dwIndex));
        }
        pBuff += sprintf(pBuff, "]\n");
        LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, abyPrintEthPortState );

    }
    else //CRI2
    {
        // 预留、暂时不做处理
    }
    
#else // CRI

    // 预留、暂时不做处理

#endif

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
void CBoardAgent::ProcBoardModuleStatus( CMessage* const pcMsg )
{

	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	if( FindAlarm( ALARM_MCU_MODULE_OFFLINE, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) )
	{
		if( 0 == *( pcMsg->content ))      //normal
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
				tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );
			
				memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( 1 == *( pcMsg->content ) )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_MODULE_OFFLINE, ALARMOBJ_MCU, byAlarmObj, &tAlarmData ) == FALSE )
			{
				brdagtlog( "BoardAgent: AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
				tAlarmMsg.abyAlarmContent[3] = *( pcMsg->content );

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg );
			
				memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}

	return;
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
2007/9/17   4.0			周广程                  创建
=============================================================================*/
void CBoardAgent::ProcBoardConfigModify( CMessage* const pcMsg )
{
	if ( NULL == pcMsg )
	{
		OspPrintf(TRUE, FALSE, "[ProcBoardConfigModify] Receive null msg!\n");
		return;
	}

	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byLayer = 0;
	memcpy( &byLayer, cServMsg.GetMsgBody(), sizeof(u8) );
	u8 bySlot = 0;
	memcpy( &bySlot, cServMsg.GetMsgBody()+sizeof(u8), sizeof(u8) );

	if ( byLayer != g_cBrdAgentApp.GetBrdPosition().byBrdLayer
		|| bySlot != g_cBrdAgentApp.GetBrdPosition().byBrdSlot )
	{
		CriLog( "[ProcBoardConfigModify] Receive msg is not local board msg! Layer = %d, Slot = %d\n", byLayer, bySlot );
		return;
	}

	u8	byPeriCount;
	u8	byPeriType;
	u16	wLoop, wIndex;
	TEqpPrsEntry* ptPrsCfg;		// Prs的配置信息
	BOOL32 bIsCfgPrs = FALSE;
	
	u8 *pMsg = cServMsg.GetMsgBody()+sizeof(u8)+sizeof(u8);

	byPeriCount = pMsg[0]; // 先是外设个数
	wIndex = 1;

	for( wLoop = 0; wLoop < byPeriCount; wLoop++ )
	{			
		byPeriType = pMsg[wIndex++]; // 先是外设类型
		
		switch( byPeriType ) // 设置该单板相应的配置信息
		{
		case EQP_TYPE_PRS:
			bIsCfgPrs = TRUE;
			if ( !g_cBrdAgentApp.IsRunPrs() )
			{
				ptPrsCfg = (TEqpPrsEntry*)(pMsg+wIndex);
				ptPrsCfg->SetMcuIp(g_cBrdAgentApp.GetMpcIpA());
				wIndex += sizeof(TEqpPrsEntry);
				g_cBrdAgentApp.SetPrsConfig(ptPrsCfg);
				TPrsCfg tPrsCfg;
				g_cBrdAgentApp.GetPrsCfg(&tPrsCfg);
				if( !prsinit( &tPrsCfg ) )
				{
					CriLog( " [ProcBoardConfigModify]Start the prs failed!\n");
				}	
				else
				{
					CriLog( " [ProcBoardConfigModify]Start the prs OK!\n");
				}			
			}
			else
			{
				CriLog( "The board have started the Prs module, need reboot!\n" );
				return;
			}
			break;

		default:
			break;
		}
	}
	
	if ( !bIsCfgPrs && g_cBrdAgentApp.IsRunPrs() )
	{
		CriLog("[ProcBoardConfigModify] New cfg delete the prs, need reboot!\n");
		return;
	}

	// 以后是单板IP        
    CriLog( "g_cBrdAgentApp.SetBrdIpAddr(0x%x)-wIndex = %d\n",
                        ntohl(*(u32*)&pMsg[wIndex+2]), wIndex);

	if ( g_cBrdAgentApp.GetBrdIpAddr() != ntohl(  *(u32*)&pMsg[wIndex+2] ) )
	{
		CriLog( "The Cri ip changed, need recfg!\n" );
		return;
	}
			
	if( pMsg[ wIndex + 6 ] == 0 )// 以后是是否使用看门狗
	{
		SysRebootDisable( );
	}
	else
	{
		SysRebootEnable( );
	}
		
	return;
}

/*====================================================================
函数名      ：ProcBrdSlaveSysRegReq
功能        ：从系统（IS2.2 8313） 注册请求
算法实现    ：
引用全局变量：
输入参数说明：CMessage * const pcMsg, 传入的消息
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
12/05/04    4.7         liaokang       创建
====================================================================*/
void CBoardAgent::ProcBrdSlaveSysRegReq( CMessage* const pcMsg )
{  
    m_dwBrdSlaveSysNode = pcMsg->srcnode;
    m_dwBrdSlaveSysIId = pcMsg->srcid;

    // 设置断链检测参数
	OspSetHBParam( m_dwBrdSlaveSysNode, 10, 3 );
    // 设置node连接中断时需通知的AppId和InstId
    OspNodeDiscCBRegQ( m_dwBrdSlaveSysNode, GetAppID(), GetInsID() );

    PostMsgToBrdSlaveSys( BRDSYS_MASTER_SLAVE_REG_ACK );
}


/*====================================================================
函数名      ：ProcBrdSlaveSysLedStatus
功能        ：led状态处理（从系统 IS2.2 8313）
算法实现    ：
引用全局变量：
输入参数说明：CMessage * const pcMsg, 传入的消息
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
12/05/04    4.7         liaokang       创建
====================================================================*/
void CBoardAgent::ProcBrdSlaveSysLedStatus( CMessage* const pcMsg )
{
    // 比较 维护从系统led状态
    TBrdLedState tLedState;
    memcpy( &tLedState, pcMsg->content, sizeof(TBrdLedState) );
    if( memcmp(&m_tBrdSlaveSysLedState, &tLedState, sizeof(TBrdLedState)) == 0 )
    {
        return; 
    }
    memcpy( &m_tBrdSlaveSysLedState, &tLedState, sizeof(TBrdLedState) );  
    
    u8  abyLedBuf[ MAX_BOARD_LED_NUM + 1 ]={0};    // led BUF  最大32个灯
    u8  byLedCount = 0;                            // led个数
    // abyBuf结构：TBrdPos + byLedCount + abyLedBuf
    u8  abyBuf[ sizeof(TBrdPos) + sizeof(u8) + MAX_BOARD_LED_NUM + 1 ] = {0};  // 消息BUF
    u16 dwMsgLen = 0;                        // 消息长度
    
#if (defined _LINUX_ && defined _LINUX12_)
    // IS2.2
    TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
    memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );                       // 单板身份结构
    byLedCount = sizeof(TBrdIS3MPC8548LedDesc) + sizeof(TBrdIS3MPC8313LedDesc);
    memcpy(abyBuf+sizeof(TBrdPos), &byLedCount, sizeof(u8) );         // led个数
    memcpy(abyLedBuf, &m_tBrdMasterSysLedState.nlunion.tBrdIS2XMpc8548LedState, sizeof(TBrdIS3MPC8548LedDesc));
    memcpy(abyLedBuf + sizeof(TBrdIS3MPC8548LedDesc), &m_tBrdSlaveSysLedState.nlunion.tBrdIS2XMpc8313LedState, sizeof(TBrdIS3MPC8313LedDesc));
    memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), abyLedBuf, byLedCount); // led BUF
    dwMsgLen = sizeof(TBrdPos) + sizeof(u8) + byLedCount;             // 消息长度
#endif
    PostMsgToManager( BOARD_MPC_LED_STATUS_NOTIFY, abyBuf, dwMsgLen);

    // 打印led状态
    s8 abyPrintLedState[256] = {0};        
    abyPrintLedState[255] = '\0';
    s8 *pBuff = abyPrintLedState;        
    pBuff += sprintf(pBuff, "[ProcBrdSlaveSysLedStatus] Led Num:%d [", byLedCount);
    u32 dwIndex = 0;        
    for (dwIndex = 0; dwIndex < byLedCount; ++dwIndex)
    {
        pBuff += sprintf(pBuff, " %d ", *(abyLedBuf + dwIndex));
    }
    pBuff += sprintf(pBuff, "]\n");
    LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, abyPrintLedState );
    
    return;
}

/*====================================================================
函数名      ：ProcBrdSlaveSysEthPortStatus
功能        ：从系统（IS2.2 8313）网口状态
算法实现    ：
引用全局变量：
输入参数说明：CMessage * const pcMsg, 传入的消息
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
12/05/04    4.7         liaokang       创建
====================================================================*/
void CBoardAgent::ProcBrdSlaveSysEthPortStatus( CMessage* const pcMsg )
{
#ifdef _LINUX12_   // 目前仅brdwrapper.h中有SWEthInfo

    /*
    根据需求，暂时只传IS2.2 双工状态给MCU，后续修改需与mcuagent一起修改
    NMS侧 双工 表示：1 半双工 2 全双工 3 未知
    接口  双工表示： 0 半双工 1 全双工
    */

    // 比较 维护从系统网口状态
    SWEthInfo tEthPortState;
    memcpy( &tEthPortState, pcMsg->content, sizeof(SWEthInfo) );
    if( memcmp(&m_tBrdSlaveSysEthPortState, &tEthPortState, sizeof(SWEthInfo)) == 0 )
    {
        return; 
    }    
    memcpy( &m_tBrdSlaveSysEthPortState, &tEthPortState, sizeof(SWEthInfo) );

    u8  abyEthPortBuf[ MAX_BOARD_ETHPORT_NUM + 1 ]={0};    // EthPort BUF
    u8  byEthPortCount = 0;                            // EthPort个数
    // abyBuf结构：TBrdPos + byEthPortCount + EthPortBuf
    u8  abyBuf[ sizeof(TBrdPos) + sizeof(u8) + MAX_BOARD_ETHPORT_NUM + 1 ] = {0};  // 消息BUF
    u16 dwMsgLen = 0;                              // 消息长度
    
    // 单板身份结构
    TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
    memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) ); 
    
    // 8548 前网口 1个 + 8313 前网口 2个 + 8313 后网口 8个
    byEthPortCount = 1 + FRONT_PANE_ETH_MAXNUM + BACK_PANE_ETH_MAXNUM;
    u8 byDuplexToNms = 0; // 驱动组提供的双工状态转化为网管组的双工状态
    u8 byLoop = 0;
    // EthPort BUF
    // 前面板前面板第一个网口 G2
    if ( 0 == m_tBrdMasterSysFrontEthPortState.Link )
    {
        byDuplexToNms = 3;
    }
    else
    {
        byDuplexToNms = m_tBrdMasterSysFrontEthPortState.Duplex + 1;
    }
    memcpy( abyEthPortBuf, &byDuplexToNms, sizeof(u8)); 
    
    // 前面板第二个第三个网口 G0 G1
    for ( byLoop = 0; byLoop< FRONT_PANE_ETH_MAXNUM; byLoop++ )
    {
        if ( 0 == m_tBrdSlaveSysEthPortState.front_eth_info[byLoop].Link )
        {
            byDuplexToNms = 3;
        }
        else
        {
            byDuplexToNms = m_tBrdSlaveSysEthPortState.front_eth_info[byLoop].Duplex + 1;
        }        
        memcpy( abyEthPortBuf + sizeof(u8) * ( 1 + byLoop ), &byDuplexToNms, sizeof(u8));
    }

    // 后面板link0-7
    for ( byLoop = 0; byLoop< BACK_PANE_ETH_MAXNUM; byLoop++ )
    {
        if ( 0 == m_tBrdSlaveSysEthPortState.back_eth_info[byLoop].Link )
        {
            byDuplexToNms = 3;
        }
        else
        {
            byDuplexToNms = m_tBrdSlaveSysEthPortState.back_eth_info[byLoop].Duplex + 1;
        }        
        memcpy( abyEthPortBuf + sizeof(u8) * ( FRONT_PANE_ETH_MAXNUM + 1 + byLoop), &byDuplexToNms, sizeof(u8));
    } 
    
    memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );                       // 单板身份结构
    memcpy(abyBuf+sizeof(TBrdPos), &byEthPortCount, sizeof(u8) );     // EthPort个数
    memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), abyEthPortBuf, byEthPortCount); // EthPort BUF
    dwMsgLen = sizeof(TBrdPos) + sizeof(u8) + byEthPortCount;         // 消息长度 
    PostMsgToManager( BOARD_MPC_ETHPORT_STATUS_NOTIFY, abyBuf, dwMsgLen);

    // 打印EthPort状态
    s8 abyPrintEthPortState[256] = {0};        
    abyPrintEthPortState[255] = '\0';
    s8 *pBuff = abyPrintEthPortState;        
    pBuff += sprintf(pBuff, "[ProcBrdSlaveSysEthPortStatus] EthPort Num:%d [", byEthPortCount);
    u32 dwIndex = 0;        
    for (dwIndex = 0; dwIndex < byEthPortCount; ++dwIndex)
    {
        pBuff += sprintf(pBuff, " %d ", *(abyEthPortBuf + dwIndex));
    }
    pBuff += sprintf(pBuff, "]\n");
    LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, abyPrintEthPortState ); 
    
#endif //end _LINUX12_

	return;
}

/*====================================================================
函数名      ：ProcDisconnect
功能        ：断链处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage * const pcMsg, 传入的消息
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
12/05/04    4.7         liaokang       创建
====================================================================*/
void CBoardAgent::ProcDisconnect( CMessage* const pcMsg )
{
    u32 dwNode = *(u32*)pcMsg->content;
    if ( INVALID_NODE != dwNode )
    {
        OspDisconnectTcpNode(dwNode);
    }
    // 从系统（IS2.2 8313）断链处理
    if( dwNode == m_dwBrdSlaveSysNode )
    {
        // 清空从系统相关数据
        m_dwBrdSlaveSysNode = INVALID_NODE;
        m_dwBrdSlaveSysIId = INVALID_INS;
        memset( &m_tBrdSlaveSysLedState, 2, sizeof(TBrdLedState) );         // 灯状态 灭
#ifdef _LINUX12_   // 目前仅brdwrapper.h中有SWEthInfo
        memset( &m_tBrdSlaveSysEthPortState, 0, sizeof(SWEthInfo) );        // 网口状态
#endif //end _LINUX12_
        // 模拟mcu agent模块brdmanager发送请求，通知mcu agent模块brdmanager
        SetTimer( MPC_BOARD_LED_STATUS_REQ, 10 );    // 更新led状态
        SetTimer( MPC_BOARD_ETHPORT_STATUS_REQ, 10 );// 更新网口状态
        LogPrint( LOG_LVL_KEYSTATUS, MID_MCULIB_MCUAGT, "[ProcDisconnect] Board Slave System disconnect!\n" );
    }
    else
    {
        CBBoardAgent::InstanceEntry( pcMsg );
    }
}

/*====================================================================
函数名      ：PostMsgToBrdSlaveSys
功能        ：发消息给从系统（IS2.2 8313）
算法实现    ：
引用全局变量：
输入参数说明：u16 wEvent:   消息号
              void * pMsg   消息内容 
			  u16  wLen:    消息内容长度
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
12/05/04    4.7         liaokang       创建
====================================================================*/
void CBoardAgent::PostMsgToBrdSlaveSys( u16 wEvent, void * pMsg, u16 wLen)
{
    if( OspIsValidTcpNode( m_dwBrdSlaveSysNode ) )
    {
        post( m_dwBrdSlaveSysIId, wEvent, pMsg, wLen, m_dwBrdSlaveSysNode );
        LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MCUAGT, 
            "[PostMsgToBrdSlaveSys]Post message %u(%s) to slave system!\n", wEvent, ::OspEventDesc( wEvent ));
    }
    else
    {
        LogPrint( LOG_LVL_ERROR, MID_MCULIB_MCUAGT,
            "[PostMsgToBrdSlaveSys]Post message %u(%s) to slave system ERROR!\n", wEvent, ::OspEventDesc( wEvent ));
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
void CBoardAgent::ProcBoardIS22ResetCmd( CMessage* const pcMsg )
{
    switch(CurState())
    {
    case STATE_IDLE:
    case STATE_INIT:
    case STATE_NORMAL:
		PostMsgToBrdSlaveSys(pcMsg->event);
        OspPrintf(TRUE, FALSE, "[ProcBoardIS22ResetCmd] send reboot cmd to 8313,Reboot 8548!\n");
        OspDelay(1000);		
        BrdHwReset();

        break;

    default:
        OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n",
                                pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState());
        break;
    }
	return;
}


/*lint -save -e765*/
API u32 BrdGetDstMcuNode( void )
{
    return g_cBrdAgentApp.m_dwDstMcuNodeA;
}

API u32 BrdGetDstMcuNodeB(void)
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}

/*lint -save -esym(530,argptr)*/
API void CriLog(char * fmt, ...)
{
	if ( g_cBrdAgentApp.m_bPrintBrdLog )
	{
		OspPrintf(TRUE, FALSE, "[Cri]");
	}
    va_list argptr;
	va_start(argptr, fmt);
	g_cBrdAgentApp.brdlog(fmt, argptr);
	va_end(argptr);
    return;
}

API void pcrimsg(void)
{
	g_cBrdAgentApp.m_bPrintBrdLog = TRUE;
}

API void npcrimsg( void )
{
	g_cBrdAgentApp.m_bPrintBrdLog = FALSE;
}

// API void nupdatesoft( void )
// {
// 	g_cBrdAgentApp.m_bUpdateSoftBoard = FALSE;
// }
// API void updatesoft( void )
// {
// 	g_cBrdAgentApp.m_bUpdateSoftBoard = TRUE;
// }
// API u8  GetBoardSlot()
// {
// 	return g_cBrdAgentApp.GetBrdPosition().byBrdSlot;
// }

/*lint -restore*/
