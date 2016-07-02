/*****************************************************************************
   模块名      : MCU主控热备份的通讯及实现会话模块
   文件名      : msmanagerssn.cpp
   相关文件    : msmanagerssn.h
   文件实现功能: MCU主控热备份的通讯及实现会话模块实现
   作者        : 万春雷
   版本        : V4.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/10/18  4.0         万春雷        创建
-----------------------------------------------------------------------------
遗留问题：
0。主备发生切换时,新的主用板需将相关信息重新通知会控,以便保持切换过程中可能的变化,以保持一致
1。备用板主备数据倒换时，缺少 [MSMag <->McuVc]模块互锁；
   部分定时器未恢复
2。稳态的相关公用变量读写访问操作缺少互斥保护
3。当前发送出去的同步数据的状态 SendReq 后等待相应Ack: 
   需超时或失败时的重传机制：尝试重传2次，仍失败等待下次同步，注意解锁 
4。需同步的数据的简化处理
5。同步前的稳态：未考虑Osp消息接收点状态，存在堆积时也需同步
6。非稳态的备用升为主用的处理措施: 无法回滚，重启
7。数据结构紧凑：由于同步的主从MCU应用环境一致，暂不考虑紧凑问题：Pack Ingore
8。IncompletePro 标记的代码部分需进一步
******************************************************************************/

#include "evmcu.h"
#include "agentinterface.h"
#include "mcuvc.h"
//#include "msmanagerssn.h"
#include "vcsssn.h"

#ifdef _VXWORKS_
#include <inetLib.h>
#endif

// MPC2 支持
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#else
    #include "brddrvlib.h"
#endif

#include "brdwrapperdef.h"

#include "mcuver.h"

CMSManagerSsnApp  g_cMSSsnApp;

u32 g_dwMsCheckTime = MS_CEHCK_MSSTATE_TIMEOUT;

//构造
CMSManagerSsnInst::CMSManagerSsnInst()
{
	m_dwMcuAppIId   = 0;
	m_dwMcuNode     = INVALID_NODE;
	m_dwRcvFrmLen   = 0;
	m_bRemoteInited = FALSE;
	m_byCheckTimes  = 0;
	memset((void*)&m_tSndFrmHead, 0, sizeof(m_tSndFrmHead));
	memset((void*)&m_tRcvFrmHead, 0, sizeof(m_tRcvFrmHead));
	memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));	
}

//析构
CMSManagerSsnInst::~CMSManagerSsnInst()
{
    // destructor 不能调用 
	// ClearInst();
	if (NULL != m_tSndFrmHead.m_pbyFrmBuf)
	{
/*lint -save -e424*/	
		u8* pbyFrmBuf = m_tSndFrmHead.m_pbyFrmBuf-sizeof(TMSSynDataReqHead);
		MCU_SAFE_DELETE(pbyFrmBuf)
/*lint -restore*/
	}
	if (NULL != m_tRcvFrmHead.m_pbyFrmBuf)
	{
		MCU_SAFE_DELETE(m_tRcvFrmHead.m_pbyFrmBuf)
	}
	memset((void*)&m_tSndFrmHead, 0, sizeof(m_tSndFrmHead));
	memset((void*)&m_tRcvFrmHead, 0, sizeof(m_tRcvFrmHead));
}

/*=============================================================================
    函 数 名： InstanceEntry
    功    能： 普通实例入口
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg 消息
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		PrintMSWarning("InstanceEntry The received msg's pointer in the msg entry is NULL!\n" );
		return;
	}

	switch( pcMsg->event ) 
	{
	case OSP_OVERFLOW:   //实例已满
		break;

	case OSP_DISCONNECT: //上电通知
		ProcMcuMcuDisconnectNtf();
		break;

	case MCU_MSEXCHANGER_POWERON_CMD:
		ProcPowerOnCmd();
		break;

	//备位板MCU向主位板MCU 的连接定时器
	case MCU_MCU_CONNECT_TIMER:
		ProcMcuMcuConnectTimeOut();
		break;
	//备位板MCU向主位板MCU 的注册定时器
	case MCU_MCU_REGISTER_TIMER:
		ProcMcuMcuRegisterTimeOut();
		break;

	//vxworks下检测主备用状态的定时器
	case MCU_MCU_CEHCK_MSSTATE_TIMER:
		ProcMcuMcuCheckMSStateTimeOut();
		break;

	//主用板MCU向备用板MCU 的数据倒换定时器
	case MCU_MCU_SYN_TIMER:
		ProcMcuMcuSynTimeOut();
		break;
		
	//MCU之间 等待对端的请求应答 的超时定时器
	case MCU_MCU_WAITFORRSP_TIMER:
		ProcMcuMcuWaitforRspTimeOut( pcMsg );
		break;

	//处理备位板MCU发过来的注册请求
	case MCU_MCU_REGISTER_REQ:
		ProcMcuMcuRegisterReq( pcMsg );
        break;
	//处理主位板MCU发过来的注册请求的 应答
	case MCU_MCU_REGISTER_ACK:
	case MCU_MCU_REGISTER_NACK:
		ProcMcuMcuRegisterRsp( pcMsg );		
		break;

	//处理对端MCU的主备位类型及当前的主备使用状态通知，进行主备用协商
	case MCU_MCU_MS_DETERMINE:
		ProcMcuMcuMSDetermine( pcMsg );
		break;
	//处理主备用协商结果通知
	case MCU_MCU_MS_RESULT:
		ProcMcuMcuMSResult( pcMsg );
		break;
		
	//处理主备用板MCU之间 开始数据倒换 交互
	case MCU_MCU_START_SYN_REQ:
		ProcMcuMcuStartSynReq( pcMsg );
		break;
	case MCU_MCU_START_SYN_ACK:
	case MCU_MCU_START_SYN_NACK:
		ProcMcuMcuStartSynRsp( pcMsg );
		break;	
	//处理主用板MCU向备用板MCU 结束数据倒换 通知
	case MCU_MCU_END_SYN_NTF:
		ProcMcuMcuEndSynNtf();
		break;

	//处理主备用板MCU之间 实际切包组包的数据倒换 交互
	case MCU_MCU_SYNNING_DATA_REQ:
		ProcMcuMcuSynningDataReq( pcMsg );
		break;
	case MCU_MCU_SYNNING_DATA_RSP:
		ProcMcuMcuSynningDataRsp( pcMsg );
		break;

    case MCU_MCU_PROBE_REQ:
        ProcMcuMcuProbeReq();
        break;
        
    case MCU_MCU_PROBE_ACK:
        ProcMcuMcuProbeAck();
        break;
 
    // [zw] [09/11/2008] 主mcu让备mcu重启或者升级
    case EV_AGENT_MCU_MCU_RESTART:
        PostMsgToOtherMcuDaemon( MCU_MCU_REBOOT_CMD, NULL, 0 );
        break;
    case MCU_MCU_REBOOT_CMD:
        OspPost( MAKEIID(AID_MCU_BRDGUARD, 1), AGT_SVC_REBOOT );
		printf("[CMSManagerSsnInst::InstanceEntry]MCU_MCU_REBOOT_CMD req reboot\n");
        break;
    case EV_AGENT_MCU_MCU_UPDATE:        
        PostMsgToOtherMcuDaemon( MCU_MCU_UPDATE_CMD, (u8*)pcMsg->content, pcMsg->length );
        break;
    case MCU_MCU_UPDATE_CMD:
        {
            CServMsg cMsg(pcMsg->content, pcMsg->length);
            OspPost( MAKEIID(AID_MCU_AGENT, 1), EV_MSGCENTER_NMS_UPDATEDMPC_CMD, cMsg.GetMsgBody(), cMsg.GetMsgBodyLen() );
            break;
        }

	default:
		PrintMSCritical("[MSManager] Wrong message %u(%s) received in InstanceEntry()!\n", 
			             pcMsg->event, ::OspEventDesc(pcMsg->event));
		break;
	}

	return;
}

/*=============================================================================
    函 数 名： ProcPowerOnCmd
    功    能： 上电通知，通过主从通讯节点完成主从协商及相应的配置信息同步处理
	           主备决定后，完成系统时间及配置文件同步，并释放同步信号
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcPowerOnCmd( void )
{
/*lint -save -esym(429, pbyDataBuf)*/
	if (MS_STATE_IDLE != CurState())
	{
		printf("[ProcPowerOnCmd] MS_STATE_IDLE != CurState().%d!!!\n", CurState());
		return;
	}

	u8 *pbyDataBuf = new u8[MS_MAX_FRAME_LEN + sizeof(TMSSynDataReqHead)];
	if (NULL == pbyDataBuf)
	{
		ClearInst();
		printf("[ProcPowerOnCmd] allocate memory failed for SndFrmBuf!!!\n");
		return;
	}
	m_tSndFrmHead.m_pbyFrmBuf = pbyDataBuf + sizeof(TMSSynDataReqHead);
	m_tRcvFrmHead.m_pbyFrmBuf = new u8[MS_MAX_FRAME_LEN];
	if (NULL == m_tRcvFrmHead.m_pbyFrmBuf)
	{
		ClearInst();
		printf("[ProcPowerOnCmd] allocate memory failed for RcvFrmBuf!!!\n");
		return;
	}

	//主备用状态获取
	if (g_cMSSsnApp.GetMSDetermineType())
	{
		emMCUMSState emLocalMSState = MCU_MSSTATE_STANDBY;
		if (g_cMSSsnApp.IsActiveBoard())
		{
			emLocalMSState = MCU_MSSTATE_ACTIVE;
		}
		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[ProcPowerOnCmd] SetCurMSState.%d!\n", emLocalMSState);
		g_cMSSsnApp.SetCurMSState( emLocalMSState );
		SetTimer(MCU_MCU_CEHCK_MSSTATE_TIMER, g_dwMsCheckTime);
	}

	PrintMSInfo("[ProcPowerOnCmd] ready to com: LocalMSInfo Type.%d State.%d\n", 
		         g_cMSSsnApp.GetLocalMSType(), g_cMSSsnApp.GetCurMSState());

	//备位板 连接 主位板，并进行相应注册
	if (MCU_MSTYPE_SLAVE == g_cMSSsnApp.GetLocalMSType())
	{
        SetTimer( MCU_MCU_CONNECT_TIMER, 10 );//立即开始建链	
        LogPrint( LOG_LVL_ERROR, MID_MCU_MSMGR, "[ProcPowerOnCmd] start timer connect to another mcu.\n");
	}

	return;

/*lint -restore*/
}

/*=============================================================================
    函 数 名： ClearInst
    功    能： 清空实例 数据（m_pbySndDataBuf、m_pbyRcvDataBuf除外）
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ClearInst( void )
{
	m_dwMcuAppIId   = 0;
	m_dwMcuNode     = INVALID_NODE;
	m_dwRcvFrmLen   = 0;
	m_bRemoteInited = FALSE;
	memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));
    g_cMSSsnApp.SetRemoteMpcConnected(FALSE);

	KillTimer(MCU_MCU_CONNECT_TIMER);
	KillTimer(MCU_MCU_REGISTER_TIMER);
	KillTimer(MCU_MCU_SYN_TIMER);
	KillTimer(MCU_MCU_WAITFORRSP_TIMER);
	//KillTimer(MCU_MCU_CEHCK_MSSTATE_TIMER);
	//2010102122_tzy 如果数据同步未完成或失败，此时other mcu断链，解锁
	g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);

	NEXTSTATE(MS_STATE_IDLE);

	LogPrint( LOG_LVL_ERROR, MID_MCU_MSMGR, "[ClearInst] LocalMSInfo Type.%d State.%d\n", 
		         g_cMSSsnApp.GetLocalMSType(), g_cMSSsnApp.GetCurMSState());

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuDisconnectNtf
    功    能： MCU之间 断链通知
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuDisconnectNtf( void )
{
	PrintMSException("[ProcMcuMcuDisconnectNtf] LocalMSInfo Type.%d State.%d\n", 
		             g_cMSSsnApp.GetLocalMSType(), g_cMSSsnApp.GetCurMSState());

    //  xsl [2/25/2006]断链消息有时会晚于注册过程
    if (INVALID_NODE != m_dwMcuNode)
    {
        OspDisconnectTcpNode( m_dwMcuNode );
    }    
    
	ClearInst();
	
	//软件实现主备用倒换：
	if (FALSE == g_cMSSsnApp.GetMSDetermineType())
	{
		BOOL32 bNtfMSExchange = FALSE;
		if (MCU_MSSTATE_STANDBY == g_cMSSsnApp.GetCurMSState())
		{
			//备用板升级为主用板
			g_cMSSsnApp.SetCurMSState(MCU_MSSTATE_ACTIVE);
			bNtfMSExchange = TRUE;

            g_cMSSsnApp.SetMsSwitchOK(FALSE);
            // guzh [9/12/2006] 如果目前是同步成功状态(本板拥有最新数据)，则认为切换成功
            if(g_cMSSsnApp.IsMSSynOK())
            {
                g_cMSSsnApp.SetMsSwitchOK(TRUE);
            }
            
			LogPrint( LOG_LVL_ERROR, MID_MCU_MSMGR, "[McuMcuDisconnectNtf] STANDBY->ACTIVE, IsMsSwitchOK :%d\n",
                      g_cMSSsnApp.IsMsSwitchOK());            
		}
        
		if (MCU_MSSTATE_ACTIVE == g_cMSSsnApp.GetCurMSState())
		{
            u8 byIsSwitchOk = g_cMSSsnApp.IsMsSwitchOK() ? 1 : 0;

			g_cMSSsnApp.SetMSSynOKFlag(FALSE);
			PrintMSInfo("[ProcMcuMcuDisconnectNtf] Unlock, byIsSwitchOk.%d\n", byIsSwitchOk);

			//备用板升级为主用板，[MSMag <->McuVc]模块互锁，解锁 2005-12-15
			g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);

            if (FALSE == g_cMSSsnApp.IsMSConfigInited())
            {
                // guzh [4/17/2007] 切换后同时结束初始化锁
                g_cMSSsnApp.LeaveMSInitLock();	
            }
            
            // guzh [4/17/2007] 灭out灯
            g_cMSSsnApp.SetMpcOusLed(FALSE);

			//通知[McuVc]模块主备用状态变更
			if (bNtfMSExchange)
			{
				::OspPost(MAKEIID(AID_MCU_VC, CInstance::DAEMON), MCU_MSSTATE_EXCHANGE_NTF, &byIsSwitchOk, sizeof(u8));
			}
		}
	}
	
    u32 dwTimeOut = 10;
    //断链后本端应处于 Active 状态
	if (MCU_MSSTATE_ACTIVE != g_cMSSsnApp.GetCurMSState())
	{
		PrintMSException("[ProcMcuMcuDisconnectNtf] CurMSState.%d != MCU_MSSTATE_ACTIVE\n", 
			              g_cMSSsnApp.GetCurMSState());

        // guzh [9/12/2006] 如果此时硬件还没有翻转，多等待一会儿
        dwTimeOut = CONNECT_MASTERMCU_TIMEOUT*3;
	}

	//备位板 重新尝试连接 主位板
	if (MCU_MSTYPE_SLAVE == g_cMSSsnApp.GetLocalMSType())
	{
        SetTimer( MCU_MCU_CONNECT_TIMER, dwTimeOut );//开始建链
        LogPrint( LOG_LVL_ERROR, MID_MCU_MSMGR, "[ProcMcuMcuDisconnectNtf] start timer connect to another mcu.\n");
    }

    // guzh [9/15/2006] 通知MCU代理,MPC下线
    u8 byState = BOARD_OUTLINE;
	post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_STANDBYMPCSTATUS_NOTIFY, 
	      &byState, sizeof(byState) );

	PrintMSInfo("[ProcMcuMcuDisconnectNtf] Notify Other MPC Status.%d\n", 
		         byState);
    
	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuConnectTimeOut
    功    能： 备位板MCU连接主位板mcu连接定时请求
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuConnectTimeOut( void )
{
    PrintMSWarning("[ProcMcuMcuConnectTimeOut] Trying Connect to Master(MC1) MPC...\n");
	//printf("[ProcMcuMcuConnectTimeOut] Trying Connect to Master(MC1) MPC...\n");

	if (MS_STATE_IDLE != CurState())
	{
		return;
	}
	
	//备位板 连接 主位板，并进行相应注册
	if (TRUE == ConnectMasterMcu())
	{
		NEXTSTATE(MS_STATE_INIT);
	}
	
	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuRegisterTimeOut
    功    能： 备位板MCU连接主位板mcu注册定时请求
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuRegisterTimeOut( void )
{
	if (MS_STATE_INIT != CurState())
	{
		return;
	}
	
	//备位板 连接 主位板，并进行相应注册
	if (FALSE == ConnectMasterMcu())
	{
		NEXTSTATE(MS_STATE_IDLE);
	}

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuCheckMSStateTimeOut
    功    能： vxworks下检测主备用状态的定时器
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
    2007/04/18  4.0         顾振华                支持在完成初始化之前就进行检测
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuCheckMSStateTimeOut( void )
{
	//主备用倒换：
	if (FALSE == g_cMSSsnApp.GetMSDetermineType())
	{
		SetTimer(MCU_MCU_CEHCK_MSSTATE_TIMER, g_dwMsCheckTime);
		return;
	}

	BOOL32 bMpcEnable = g_cMSSsnApp.IsActiveBoard();
	emMCUMSState emLocalMSState = MCU_MSSTATE_STANDBY;
	if (bMpcEnable)
	{
		emLocalMSState = MCU_MSSTATE_ACTIVE;
	}

	PrintMSVerbose("[ProcMcuMcuCheckMSStateTimeOut] LocalMSInfo Type.%d OldState.%d NewState.%d CheckTime.%u\n", 
				   g_cMSSsnApp.GetLocalMSType(), g_cMSSsnApp.GetCurMSState(), emLocalMSState, g_dwMsCheckTime);

	//主备用状态变更，
	if (emLocalMSState != g_cMSSsnApp.GetCurMSState())
	{
		//增加主备用状态变更时的抖动处理:即连续三次状态变更判别正常才进行切换,防止状态快速变更的抖动
		m_byCheckTimes++;
        g_cMSSsnApp.IncDitheringTimes();
        
		if (m_byCheckTimes < MS_CHECK_MSSTATE_TIMES)
		{			
			PrintMSException("[ProcMcuMcuCheckMSStateTimeOut] CheckTimes.%d LocalMSInfo Type.%d OldState.%d NewState.%d\n", 
					  m_byCheckTimes, g_cMSSsnApp.GetLocalMSType(), g_cMSSsnApp.GetCurMSState(), emLocalMSState);                       
		}
		else
		{
			m_byCheckTimes = 0;
			g_cMSSsnApp.SetCurMSState(emLocalMSState);

            g_cMSSsnApp.SetMsSwitchOK(FALSE);
             // guzh [9/12/2006] 如果目前已经是同步完全状态，则认为切换成功
            if(MCU_MSSTATE_ACTIVE == emLocalMSState && g_cMSSsnApp.IsMSSynOK() )
            {
                g_cMSSsnApp.SetMsSwitchOK(TRUE);
                printf("[ProcMcuMcuCheckMSStateTimeOut] MCU MS Switched to ACTIVE success!\n");
            }
            // guzh [2/11/2007] 这里点灭ous灯，防止误显示
            g_cMSSsnApp.SetMpcOusLed(FALSE);

			PrintMSException("[ProcMcuMcuCheckMSStateTimeOut] New State.%d IsSynOK.%d IsSwitchOK.%d\n", 
					         emLocalMSState, 
                             g_cMSSsnApp.IsMSSynOK(),
                             g_cMSSsnApp.IsMsSwitchOK());

			KillTimer(MCU_MCU_SYN_TIMER);
			KillTimer(MCU_MCU_WAITFORRSP_TIMER);	

            // guzh [9/14/2006] 如果目前是同步成功状态(本板拥有最新数据)，则认为切换成功
            u8 bySwitchOk = g_cMSSsnApp.IsMsSwitchOK() ? 1 : 0;

			g_cMSSsnApp.SetMSSynOKFlag(FALSE);

			//清空当前倒换的数据信息记录
			memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));

			//[MSMag <->McuVc]模块互锁，解锁 2005-12-15
			g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);
            
            // guzh [4/17/2007] 切换成功后同时结束初始化锁
            if (FALSE == g_cMSSsnApp.IsMSConfigInited())
            {
                PrintMSException("[CheckMSStateTimeOut] STANDBY->ACTIVE Unlock & LeaveMSInitLock: LocalMSInfo Type.%d State.%d\n", 
						  g_cMSSsnApp.GetLocalMSType(), emLocalMSState);

                g_cMSSsnApp.LeaveMSInitLock();	
            }
            else
            {
                if (bMpcEnable)
                {
                    //备用板升级为主用板
                    //通知[McuVc]模块主备用状态变更
                    u8 byIsSwitchOk = g_cMSSsnApp.IsMsSwitchOK() ? 1 : 0;
                    ::OspPost(MAKEIID(AID_MCU_VC, CInstance::DAEMON), MCU_MSSTATE_EXCHANGE_NTF, &byIsSwitchOk, sizeof(u8));

                    //此时对端需进行完全同步，以恢复性间隔请求同步
                    SetTimer(MCU_MCU_SYN_TIMER, MS_SYN_MIN_SPAN_TIMEOUT);
                    
                    PrintMSException("[CheckMSStateTimeOut] STANDBY->ACTIVE Unlock: LocalMSInfo Type.%d State.%d\n", 
						  g_cMSSsnApp.GetLocalMSType(), emLocalMSState);
                }
                else
                {
                    PrintMSException("[CheckMSStateTimeOut] ACTIVE->STANDBY Lock: LocalMSInfo Type.%d State.%d\n", 
                        g_cMSSsnApp.GetLocalMSType(), emLocalMSState);
			    }
            }
		}
	}
	else
	{
		m_byCheckTimes = 0;
	}

	SetTimer(MCU_MCU_CEHCK_MSSTATE_TIMER, g_dwMsCheckTime);
	return;
}


/*=============================================================================
    函 数 名： PostMsgToOtherMcuDaemon
    功    能： 向另一块MCU-Daemon发送消息
    算法实现： 
    全局变量： 
    参    数： u16 wEvent, 
	           u8 *const pbyMsg, 
			   u16 wLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::PostMsgToOtherMcuDaemon(u16 wEvent, u8 *const pbyMsg, u16 wLen)
{	
	BOOL32 bRet = FALSE;
	if (INVALID_NODE == m_dwMcuNode)
	{
		PrintMSWarning("PostMsgToOtherMcuDaemon post wEvent.%d Failed, Invalid Node\n", wEvent);
		return bRet;
	}
	
	CServMsg cMsg;
	cMsg.SetEventId(wEvent);
	if (NULL != pbyMsg)
	{
		cMsg.SetMsgBody(pbyMsg, wLen);
	}

	if (post(MAKEIID(AID_MCU_MSMANAGERSSN, 1), wEvent, 
		cMsg.GetServMsg(), cMsg.GetServMsgLen(), m_dwMcuNode) < 0)
	{
		PrintMSWarning("PostMsgToOtherMcuDaemon post wEvent.%d Failed\n", wEvent);
	}
	else
	{
		bRet = TRUE;
	}
	
	return bRet;
}

/*=============================================================================
    函 数 名： PostMsgToOtherMcuInst
    功    能： 向另一块MCU-Inst发送消息
    算法实现： 
    全局变量： 
    参    数： u16 wEvent, 
	           u8 *const pbyMsg, 
			   u16 wLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::PostMsgToOtherMcuInst(u16 wEvent, u8 *const pbyMsg, u16 wLen)
{
	BOOL32 bRet = FALSE;
	if (0 == m_dwMcuAppIId || INVALID_NODE == m_dwMcuNode)
	{
		PrintMSWarning("PostMsgToOtherMcuInst post wEvent.%d Failed, Invalid Node\n", wEvent);
		//printf("PostMsgToOtherMcuInst post wEvent.%d Failed, Invalid Node\n", wEvent);
		return bRet;
	}

	CServMsg cMsg;
	cMsg.SetEventId(wEvent);
	if (NULL != pbyMsg)
	{
		cMsg.SetMsgBody(pbyMsg, wLen);
	}

	if (post(m_dwMcuAppIId, wEvent, cMsg.GetServMsg(), cMsg.GetServMsgLen(), m_dwMcuNode) < 0)
	{
		PrintMSWarning("PostMsgToOtherMcuInst post wEvent.%d Failed\n", wEvent);
		//printf("PostMsgToOtherMcuInst post wEvent.%d Failed\n", wEvent);
	}
	else
	{
		bRet = TRUE;
	}
	
	return bRet;
}

/*=============================================================================
    函 数 名： ConnectMasterMcu
    功    能： 备位板MCU连接主位板mcu请求
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::ConnectMasterMcu( void )
{
	if (FALSE == OspIsValidTcpNode(m_dwMcuNode))
	{
		m_dwMcuNode = OspConnectTcpNode( htonl(g_cMSSsnApp.GetAnotherMcuIP()), 
										 g_cMSSsnApp.GetMcuConnectPort(),
                                         5,
                                         3,
                                         1000   // guzh [4/27/2007] 尝试连接1秒，以免堵住业务
                                        );
		if (FALSE == OspIsValidTcpNode(m_dwMcuNode))
		{
			m_dwMcuNode = INVALID_NODE;
			PrintMSException("OspConnectTcpNode Another_MCU Failed!\n");
			SetTimer( MCU_MCU_CONNECT_TIMER, CONNECT_MASTERMCU_TIMEOUT );
			return FALSE;
		}	
		OspNodeDiscCBRegQ(m_dwMcuNode, GetAppID(), GetInsID());

        // guzh [4/27/2007] 如果采用软件检测，缩短检测时间(3*3)
        if (FALSE == g_cMSSsnApp.GetMSDetermineType())
        {
            OspSetHBParam(m_dwMcuNode, 3, 3);
        }
	}

	//向 Master_MCU 管理程序 注册
	u32 dwLocalIp = htonl(g_cMSSsnApp.GetLocalMcuIP());

    //[5/7/2013 liaokang] 追加消息 u16 (版本号长度) + 版本号字符串
	//PostMsgToOtherMcuDaemon(MCU_MCU_REGISTER_REQ, (u8*)&dwLocalIp, sizeof(dwLocalIp));
    CServMsg cSrvMsg;
    cSrvMsg.SetMsgBody((u8*)&dwLocalIp, sizeof(u32));
    u16 wVerLen = strlen(VER_MCU);
    cSrvMsg.CatMsgBody((u8*)&wVerLen,sizeof(u16));
    cSrvMsg.CatMsgBody((u8*)VER_MCU,wVerLen);

    PostMsgToOtherMcuDaemon(MCU_MCU_REGISTER_REQ, cSrvMsg.GetMsgBody(), cSrvMsg.GetMsgBodyLen());

	PrintMSCritical("Send Regiester Request(LocalIp0x%0x) To Another_MCU!\n", dwLocalIp);
	//printf("Send Regiester Request(LocalIp0x%x) To Another_MCU!\n", dwLocalIp);

	SetTimer( MCU_MCU_REGISTER_TIMER, REGISTER_MASTERMCU_TIMEOUT );

	return TRUE;
}


/*=============================================================================
    函 数 名： ProcMcuMcuRegisterReq
    功    能： 处理备位板MCU发过来的注册请求
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuRegisterReq( CMessage * const pcMsg )
{
	if (MS_STATE_IDLE != CurState())
	{
		post( pcMsg->srcid, MCU_MCU_REGISTER_NACK, NULL, 0, pcMsg->srcnode );
		OspDisconnectTcpNode( pcMsg->srcnode );
		ClearInst();
        PrintMSException("ProcMcuMcuRegisterReq invalid ins state :%d.\n", CurState());
		return;
	}

	if (MCU_MSTYPE_SLAVE == g_cMSSsnApp.GetLocalMSType())
	{	
		post( pcMsg->srcid, MCU_MCU_REGISTER_NACK, NULL, 0, pcMsg->srcnode );
		OspDisconnectTcpNode( pcMsg->srcnode );
		ClearInst();
        PrintMSException("ProcMcuMcuRegisterReq invalid local MSType :%d.\n", g_cMSSsnApp.GetLocalMSType());
		return;
	}
	
	CServMsg cMsg( pcMsg->content, pcMsg->length );
	u32 dwRemoteIp = *(u32*)cMsg.GetMsgBody();	
	PrintMSInfo("ProcMcuMcuRegisterReq from ip.0x%0x Slave_MCU!\n", dwRemoteIp);

    //[5/7/2013 liaokang] 版本校验
    s8 achRemoteVerInfo[MAX_SOFT_VER_LEN + 1] = {0};
    u16 wVerLen = 0;
    if(cMsg.GetMsgBodyLen() > sizeof(u32) )
    {
        wVerLen = *(u16*)(cMsg.GetMsgBody() + sizeof(u32));
        memcpy(achRemoteVerInfo, (cMsg.GetMsgBody() + sizeof(u32) + sizeof(u16)), min(wVerLen, MAX_SOFT_VER_LEN));
    }

    if( FALSE == CompareStringElements(achRemoteVerInfo, VER_MCU) )
    {
        post( pcMsg->srcid, MCU_MCU_REGISTER_NACK, NULL, 0, pcMsg->srcnode );
        OspDisconnectTcpNode( pcMsg->srcnode );
        ClearInst();
		PrintMSException("ProcMcuMcuRegisterReq local mcu ver: %s, remote mcu ver %s, disconnect!\n", VER_MCU, achRemoteVerInfo);
        return;
    }

	//认证
	if (dwRemoteIp != ntohl(g_cMSSsnApp.GetAnotherMcuIP()))
	{
		post( pcMsg->srcid, MCU_MCU_REGISTER_NACK, NULL, 0, pcMsg->srcnode );
		OspDisconnectTcpNode( pcMsg->srcnode );
		ClearInst();
		PrintMSException("ProcMcuMcuRegisterReq RemoteIp.0x%0x Failed\n", dwRemoteIp);
	}
	else
	{
		m_dwMcuNode   = pcMsg->srcnode;
		m_dwMcuAppIId = pcMsg->srcid;
		::OspNodeDiscCBRegQ( pcMsg->srcnode, GetAppID(), GetInsID() );
        g_cMSSsnApp.SetRemoteMpcConnected(TRUE);
		NEXTSTATE(MS_STATE_NORMAL);

        //[5/7/2013 liaokang] 追加版本号
		//PostMsgToOtherMcuInst(MCU_MCU_REGISTER_ACK, NULL, 0);        
        wVerLen = strlen(VER_MCU);
        cMsg.SetMsgBody((u8*)&wVerLen,sizeof(u16));
        cMsg.CatMsgBody((u8*)VER_MCU,wVerLen);
		PostMsgToOtherMcuInst(MCU_MCU_REGISTER_ACK, cMsg.GetMsgBody(), cMsg.GetMsgBodyLen());
		
		//通知 对方 本地MCU的主备位类型及当前的主备使用状态，进行主备用协商
		u8 abyMSDNtf[2];
		abyMSDNtf[0] = (u8)g_cMSSsnApp.GetLocalMSType();
		abyMSDNtf[1] = (u8)g_cMSSsnApp.GetCurMSState();
		PostMsgToOtherMcuInst(MCU_MCU_MS_DETERMINE, abyMSDNtf, sizeof(abyMSDNtf));
	}

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuRegisterRsp
    功    能： 处理主位板MCU发过来的注册请求的 应答
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuRegisterRsp( CMessage * const pcMsg )
{
	if (MS_STATE_INIT != CurState())
	{
		return;
	}
	if (MCU_MSTYPE_SLAVE != g_cMSSsnApp.GetLocalMSType())
	{
		return;
	}

	CServMsg cMsg( pcMsg->content, pcMsg->length );

    //[5/7/2013 liaokang] 版本校验
    s8 achRemoteVerInfo[MAX_SOFT_VER_LEN + 1] = {0};
    if(cMsg.GetMsgBodyLen() > 0)
    {
        u16 wVerLen = *(u16*)(cMsg.GetMsgBody());
        memcpy(achRemoteVerInfo, (cMsg.GetMsgBody() + sizeof(u16)), min(wVerLen, MAX_SOFT_VER_LEN));
    }
    
    if( FALSE == CompareStringElements(achRemoteVerInfo, VER_MCU) )
    {
        OspDisconnectTcpNode( pcMsg->srcnode );
        ClearInst();
        PrintMSException("ProcMcuMcuRegisterRsp local mcu ver: %s, remote mcu ver %s, disconnect!\n", VER_MCU, achRemoteVerInfo);
        return;
    }

	if (MCU_MCU_REGISTER_ACK == pcMsg->event)
	{
		KillTimer(MCU_MCU_REGISTER_TIMER);

		m_dwMcuAppIId = pcMsg->srcid;
        g_cMSSsnApp.SetRemoteMpcConnected(TRUE);
		NEXTSTATE(MS_STATE_NORMAL);

		//通知 对方 本地MCU的主备位类型及当前的主备使用状态，进行主备用协商
		u8 abyMSDNtf[2];
		abyMSDNtf[0] = g_cMSSsnApp.GetLocalMSType();
		abyMSDNtf[1] = g_cMSSsnApp.GetCurMSState();
		PostMsgToOtherMcuInst(MCU_MCU_MS_DETERMINE, abyMSDNtf, sizeof(abyMSDNtf));

		PrintMSInfo("ProcMcuMcuRegisterRsp Ack From Another_MCU!\n");
	}
	else
	{
		PrintMSWarning("ProcMcuMcuRegisterRsp Nack From Another_MCU!\n");
	}
	//否则不作处理，等待远端断开此连接，收到断链通知后重新尝试

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuMSDetermine
    功    能： 对端MCU的主备位类型及当前的主备使用状态通知，进行主备用协商
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuMSDetermine( CMessage * const pcMsg )
{
	if (MS_STATE_NORMAL != CurState())
	{
		return;
	}

	CServMsg cMsg( pcMsg->content, pcMsg->length );
	emMCUMSType  emRemoteMSType  = (emMCUMSType)(*cMsg.GetMsgBody());
	emMCUMSState emRemoteMSState = (emMCUMSState)(*(cMsg.GetMsgBody()+1));
	emMCUMSType  emLocalMSType   = g_cMSSsnApp.GetLocalMSType();
	emMCUMSState emLocalMSState  = g_cMSSsnApp.GetCurMSState();
	BOOL32 bDetermineok = TRUE;
	
	LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[ProcMcuMcuMSDetermine] Local<Type.%d, State.%d>, Remote<Type.%d, State.%d>!\n",
								emLocalMSType, emLocalMSState, emRemoteMSType, emRemoteMSState);
	//主备用协商
	bDetermineok = g_cMSSsnApp.DetermineMSlave(emLocalMSType, emLocalMSState, 
		                                       emRemoteMSType, emRemoteMSState);

	if (bDetermineok)
	{
		u8 byRemoteMSState = (u8)emRemoteMSState;
		PostMsgToOtherMcuInst(MCU_MCU_MS_RESULT, &byRemoteMSState, sizeof(byRemoteMSState));
		g_cMSSsnApp.SetCurMSState(emLocalMSState);

		//主备用协商成功
		KillTimer(MCU_MCU_SYN_TIMER);
		g_cMSSsnApp.SetMSSynOKFlag(FALSE);

		PrintMSInfo("[ProcMcuMcuMSDetermine] Determineok Unlock: LocalMSInfo Type.%d State.%d RemoteMSState.%d\n", 
				     g_cMSSsnApp.GetLocalMSType(), emLocalMSState, emRemoteMSState);

		//printf("[ProcMcuMcuMSDetermine] Determineok Unlock: LocalMSInfo Type.%d State.%d RemoteMSState.%d\n", 
		//		     g_cMSSsnApp.GetLocalMSType(), emLocalMSState, emRemoteMSState);
	}
	else
	{
		PrintMSException("[ProcMcuMcuMSDetermine] Determine Err: LocalMSType.%d LocalMSState.%d RemoteMSType.%d RemoteMSState.%d\n", 
			             g_cMSSsnApp.GetLocalMSType(), emLocalMSState, emRemoteMSType, emRemoteMSState);

		//printf("[ProcMcuMcuMSDetermine] Determine Err: LocalMSType.%d LocalMSState.%d RemoteMSType.%d RemoteMSState.%d\n", 
		//	             g_cMSSsnApp.GetLocalMSType(), emLocalMSState, emRemoteMSType, emRemoteMSState);

		//主备用协商异常，存在误配，已初始化则不予理睬，否则置为Standby......
		if (FALSE == g_cMSSsnApp.IsMSConfigInited())
		{
			g_cMSSsnApp.SetCurMSState( MCU_MSSTATE_STANDBY );
		}
		OspDisconnectTcpNode(m_dwMcuNode);
		ClearInst();
	}

	if (MCU_MSSTATE_ACTIVE == emLocalMSState)
	{
		//此时对端需进行完全同步，即刻请求同步
		SetTimer(MCU_MCU_SYN_TIMER, MS_SYN_NOW_SPAN_TIMEOUT);
	}
	
	PrintMSInfo("ProcMcuMcuMSDetermine bDetermineok.%d LocalMSState.%d RemoteMSState.%d\n", 
		         bDetermineok, emLocalMSState, emRemoteMSState);

    //printf("ProcMcuMcuMSDetermine bDetermineok.%d LocalMSState.%d RemoteMSState.%d\n", 
	//	         bDetermineok, emLocalMSState, emRemoteMSState);

    // guzh [9/15/2006] 主备协商成功后通知MCU代理,备用MPC上线
    if (bDetermineok)
    {
        u8 byState = BOARD_INLINE;
		post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_STANDBYMPCSTATUS_NOTIFY, 
			&byState, sizeof(byState) );

	    PrintMSInfo("[ProcMcuMcuMSDetermine] Notify Other MPC Status.%d\n", 
		             byState);
    }

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuMSResult
    功    能： MCU之间 主备用协商结果通知（即协商后的本端主备用状态）
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuMSResult( CMessage * const pcMsg )
{
	if (MS_STATE_NORMAL != CurState())
	{
		return;
	}
	
	CServMsg cMsg( pcMsg->content, pcMsg->length );
	emMCUMSState emLocalMSState = (emMCUMSState)(*cMsg.GetMsgBody());
	
	if (emLocalMSState != g_cMSSsnApp.GetCurMSState())
	{
		OspDisconnectTcpNode(m_dwMcuNode);
		ClearInst();
		PrintMSException("[ProcMcuMcuMSResult] LocalMSType.%d ResultMSType.%d\n", 
		                 g_cMSSsnApp.GetCurMSState(), emLocalMSState);
	}
	
	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuSynTimeOut
    功    能： 处理 主用板MCU向备用板MCU 数据倒换定时
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuSynTimeOut( void )
{
	if (MS_STATE_NORMAL != CurState() || 
		MCU_MSSTATE_ACTIVE != g_cMSSsnApp.GetCurMSState())
	{
        PrintMSException("[ProcMcuMcuSynTimeOut] invalid ins state :%d, CurMsState :%d.\n",
                         CurState(), g_cMSSsnApp.GetCurMSState());

		//printf("[ProcMcuMcuSynTimeOut] invalid ins state :%d, CurMsState :%d.\n",
         //               CurState(), g_cMSSsnApp.GetCurMSState());
		return;
	}
	
	//关闭请求超时检测
	KillTimer(MCU_MCU_WAITFORRSP_TIMER);

    //主用板同步请求超时，[MSMag <->McuVc]模块互锁，解锁 2005-12-15
    g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);
    
    PrintMSWarning("[ProcMcuMcuSynTimeOut] Waitfor_SynOpr_Rsp_Timeout: UnLock\n");
    //printf("[ProcMcuMcuSynTimeOut] Waitfor_SynOpr_Rsp_Timeout: UnLock\n");

	if (m_tMSCurSynState.m_emSynType > emSynDataStart)
	{
		g_cMSSsnApp.SetMSSynOKFlag(FALSE);
		//清空当前倒换的数据信息记录
		memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));
	}
	
    //探测对端是否存在，若存在则开始数据同步请求
	if (!PostMsgToOtherMcuInst(MCU_MCU_PROBE_REQ, NULL, 0))
    {
        PrintMSCritical("[ProcMcuMcuSynTimeOut] Post_Probe_Req failed m_dwMcuNode.%u\n", m_dwMcuNode);
		//printf("[ProcMcuMcuSynTimeOut] Post_Probe_Req failed m_dwMcuNode.%u\n", m_dwMcuNode);
    }

	//若对端未完成完全同步，以恢复性间隔请求同步，否则以正常性间隔请求同步
	if (g_cMSSsnApp.IsMSSynOK())
	{
		SetTimer(MCU_MCU_SYN_TIMER, g_cMcuVcApp.GetMsSynTime()*1000 );
	}
	else
	{
		SetTimer(MCU_MCU_SYN_TIMER, MS_SYN_MIN_SPAN_TIMEOUT);
	}

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuWaitforRspTimeOut
    功    能： 处理 MCU之间 等待对端的请求应答 的超时定时
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuWaitforRspTimeOut( CMessage * const pcMsg )
{	
	switch( CurState() )
	{
	case MS_STATE_NORMAL:
		
		KillTimer(pcMsg->event);

		if (MCU_MCU_WAITFORRSP_TIMER == pcMsg->event && 
			MCU_MSSTATE_ACTIVE == g_cMSSsnApp.GetCurMSState())
		{
			//MCU_MCU_START_SYN_REQ
			//MCU_MCU_SYNNING_DATA_REQ
			//此时对端需进行完全同步，以恢复性间隔请求同步
			g_cMSSsnApp.SetMSSynOKFlag(FALSE);
			KillTimer(MCU_MCU_SYN_TIMER);
			SetTimer(MCU_MCU_SYN_TIMER, MS_SYN_MIN_SPAN_TIMEOUT);

			//清空当前倒换的数据信息记录
			memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));

			//主用板同步请求超时，[MSMag <->McuVc]模块互锁，解锁 2005-12-15
			g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);
			
			PrintMSWarning("[ProcMcuMcuWaitforRspTimeOut] Waitfor_SynOpr_Rsp_Timeout: UnLock\n");
			//printf("[ProcMcuMcuWaitforRspTimeOut] Waitfor_SynOpr_Rsp_Timeout: UnLock\n");
		}
        else if (MCU_MCU_WAITFORRSP_TIMER == pcMsg->event && 
			MCU_MSSTATE_STANDBY == g_cMSSsnApp.GetCurMSState())
        {
			//备板的timeout，解锁
			g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);
			PrintMSWarning("[ProcMcuMcuWaitforRspTimeOut] StandBy board Waitfor_SynOpr_Rsp_Timeout: UnLock\n");

		}
		else
		{
     
            PrintMSException("[ProcMcuMcuWaitforRspTimeOut] Waitfor_SynOpr_Rsp_Timeout, CurMSState:%d\n", 
                             g_cMSSsnApp.GetCurMSState());

			//printf("[ProcMcuMcuWaitforRspTimeOut] Waitfor_SynOpr_Rsp_Timeout, CurMSState:%d\n", 
            //                 g_cMSSsnApp.GetCurMSState());
        }
		return;
	default:
		break;
	}

	return;
}


/*=============================================================================
函 数 名： ProcMcuMcuProbeReq
功    能： 主板向备板探测消息
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/4/4  4.0			许世林                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuProbeReq( void )
{
    if (g_cMSSsnApp.GetCurMSState() == MCU_MSSTATE_STANDBY)
    {
        PostMsgToOtherMcuInst(MCU_MCU_PROBE_ACK, NULL, 0);
		//备板在主备备数据的时候也应该锁住，否则可能出现主备数据不一致的情况（备板vc处理，主板vc不处理）
		//10s timeout,10s内主备没同步完解 vc和ms的锁
		SetTimer(MCU_MCU_WAITFORRSP_TIMER,10000);
		//考虑到网络问题，可能出现前一个timer没到，后一个probe又倒锁死情况，先解锁保护
		g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);
		g_cMSSsnApp.EnterMSSynLock(AID_MCU_MSMANAGERSSN);
		PrintMSInfo("[ProcMcuMcuProbeReq] Rcv_SynOpr_Req: Lock\n");
    }    
    return;
}

/*=============================================================================
函 数 名： ProcMcuMcuProbeAck
功    能： 备板工作正常发起同步数据请求
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/4/4  4.0			许世林                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuProbeAck( void )
{
    //发送同步请求，通知对端【本端当前环境状态】，以保证同步前的稳态：
    TMSSynInfoReq tMSSynInfoReq;
    g_cMcuVcApp.GetVcDeamonEnvState(tMSSynInfoReq.m_tEnvState);
    tMSSynInfoReq.m_byInited = g_cMSSsnApp.IsMSConfigInited() ? 1 : 0;
    if (PostMsgToOtherMcuInst(MCU_MCU_START_SYN_REQ, (u8*)&tMSSynInfoReq, sizeof(tMSSynInfoReq)))
    {
        //[MSMag <->McuVc]模块互锁，锁定 2005-12-15
		g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);//如果备板连续回两条ack过来会导致锁住
        g_cMSSsnApp.EnterMSSynLock(AID_MCU_MSMANAGERSSN);
        
        PrintMSInfo("[ProcMcuMcuProbeAck] Post_SynOpr_Req: Lock\n");
        
        //开启请求超时检测       
        //缩短定时器，防止锁定过长时间无法检测到对端断链
        SetTimer(MCU_MCU_WAITFORRSP_TIMER, MS_WAITFOTRSP_TIMEOUT, (u32)MCU_MCU_START_SYN_REQ);
    }
    else
    {
        PrintMSCritical("[ProcMcuMcuProbeAck] Post_SynOpr_Req failed m_dwMcuNode.%u\n", m_dwMcuNode);
    }

    return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuStartSynReq
    功    能： 处理 主用板MCU向备用板MCU 开始数据倒换 请求
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/12  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuStartSynReq( CMessage * const pcMsg )
{
	if (MS_STATE_NORMAL != CurState() || 
		MCU_MSSTATE_STANDBY != g_cMSSsnApp.GetCurMSState())
	{
		PrintMSWarning("[ProcMcuMcuStartSynReq] Post_SynOpr_Nack: CurInstState.%d CurMSState.%d\n", 
			            CurState(), g_cMSSsnApp.GetCurMSState());

		//printf("[ProcMcuMcuStartSynReq] Post_SynOpr_Nack: CurInstState.%d CurMSState.%d\n", 
		//	            CurState(), g_cMSSsnApp.GetCurMSState());

		PostMsgToOtherMcuInst(MCU_MCU_START_SYN_NACK, NULL, 0);
		return;
	}

	CServMsg cMsg( pcMsg->content, pcMsg->length );
	if (cMsg.GetMsgBodyLen() != sizeof(TMSSynInfoReq)) 
	{
		PrintMSException("[ProcMcuMcuStartSynReq] Invalid msgLen != sizeof(TMSSynInfoReq).%d\n", sizeof(TMSSynInfoReq));
	
		//printf("[ProcMcuMcuStartSynReq] Invalid msgLen != sizeof(TMSSynInfoReq).%d\n", sizeof(TMSSynInfoReq));
		return;
	}

	TMSSynInfoReq *ptMSSynInfoReq = (TMSSynInfoReq*)cMsg.GetMsgBody();
	m_bRemoteInited = (1 == ptMSSynInfoReq->m_byInited) ? TRUE : FALSE;

	//收到同步请求，若本端未完成初始化，则允许同步，
	//              若本端已完成初始化，对照【本端当前环境状态】，一致则允许同步，以保证同步前的稳态
	u8 byInited    = g_cMSSsnApp.IsMSConfigInited() ? 1 : 0;
	BOOL32 bPrintErr = (MS_DEBUG_INFO <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
    TMSSynState tMSSynState;
	if (1 == byInited && 
		FALSE == g_cMcuVcApp.IsEqualToVcDeamonEnvState(&ptMSSynInfoReq->m_tEnvState, bPrintErr, &tMSSynState))
	{
        //主备环境信息不一致
        if (!tMSSynState.IsSynSucceed())
        {
			//NACK主板的请求
			g_cMSSsnApp.SetMSSynOKFlag(FALSE);

			PostMsgToOtherMcuInst(MCU_MCU_START_SYN_NACK, (u8*)&tMSSynState, sizeof(TMSSynState) );
           
			PrintMSInfo( "[ProcMcuMcuStartSynReq] EntityType:%d, EntityId:%d state conflict!\n",
			          tMSSynState.GetEntityType(), tMSSynState.GetEntityId() );

			//printf("[ProcMcuMcuStartSynReq] EntityType:%d, EntityId:%d state conflict!\n",
			 //         tMSSynState.GetEntityType(), tMSSynState.GetEntityId());

		}

        //备板稳态丢失后点亮ous灯
        if (g_cMSSsnApp.GetCurMSState() == MCU_MSSTATE_STANDBY)
        {
            g_cMSSsnApp.SetMpcOusLed(TRUE);
        }
		//g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);
		PrintMSInfo("[ProcMcuMcuStartSynReq] IsEqualToVcDeamonEnvState Failed, byInited.%d, Post_SynOpr_Nack\n", byInited);
		//printf("[ProcMcuMcuStartSynReq] IsEqualToVcDeamonEnvState Failed, byInited.%d, Post_SynOpr_Nack\n", byInited);
	}
	else
	{
        TMSSynInfoRsp tRsp;
        tRsp.m_byInited = byInited;
		PostMsgToOtherMcuInst(MCU_MCU_START_SYN_ACK, (u8*)&tRsp, sizeof(tRsp));
		
		// 记录下主板的MCS连接状态, zgc, 2008-03-25
		g_cMSSsnApp.SetMSVcMCState( ptMSSynInfoReq->m_tEnvState.m_tMCState );
		
		//[MSMag <->McuVc]模块互锁，锁定 2005-12-15

		//IncompletePro: 备用板的锁定处理
		//g_cMSSsnApp.EnterMSSynLock(AID_MCU_MSMANAGERSSN);
		
		//清空当前倒换的数据信息记录
		memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));

		PrintMSInfo("[ProcMcuMcuStartSynReq] IsEqualToVcDeamonEnvState OK, byInited.%d, Post_SynOpr_Ack, CurTick :%u\n", 
            byInited, OspTickGet());

		//printf("[ProcMcuMcuStartSynReq] IsEqualToVcDeamonEnvState OK, byInited.%d, Post_SynOpr_Ack, CurTick :%u\n", 
         //   byInited, OspTickGet());
	}
	
	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuStartSynRsp
    功    能： 处理 备用板MCU向主用板MCU 开始数据倒换 请求回应
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuStartSynRsp( CMessage * const pcMsg )
{
	if (MS_STATE_NORMAL != CurState() || 
		MCU_MSSTATE_ACTIVE != g_cMSSsnApp.GetCurMSState())
	{
		PrintMSWarning("[ProcMcuMcuStartSynRsp] Ingore: CurInstState.%d CurMSState.%d\n", 
			            CurState(), g_cMSSsnApp.GetCurMSState());

		//printf("[ProcMcuMcuStartSynRsp] Ingore: CurInstState.%d CurMSState.%d\n", 
		//	            CurState(), g_cMSSsnApp.GetCurMSState());
		return;
	}
	
	//关闭请求超时检测
	KillTimer(MCU_MCU_WAITFORRSP_TIMER);
    PrintMSInfo("[ProcMcuMcuStartSynRsp] kill timer MCU_MCU_WAITFORRSP_TIMER.\n");
	//printf("[ProcMcuMcuStartSynRsp] kill timer MCU_MCU_WAITFORRSP_TIMER.\n");

	CServMsg cMsg( pcMsg->content, pcMsg->length );

	//主用板同步请求被拒，
	if (MCU_MCU_START_SYN_NACK == cMsg.GetEventId())
	{	
		//此时对端需重新进行完全同步，以恢复性间隔请求同步
		g_cMSSsnApp.SetMSSynOKFlag(FALSE);
		KillTimer(MCU_MCU_SYN_TIMER);
		SetTimer(MCU_MCU_SYN_TIMER, MS_SYN_MIN_SPAN_TIMEOUT);

        //取同步失败的状态信息
        g_cMSSsnApp.SetCurMSSynState( *(TMSSynState*)cMsg.GetMsgBody() );
        
		//[MSMag <->McuVc]模块互锁，解锁 2005-12-15
		g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);

		PrintMSException("[ProcMcuMcuStartSynRsp] Rcv_SynOpr_Nack: Unlock\n");
		//printf("[ProcMcuMcuStartSynRsp] Rcv_SynOpr_Nack: Unlock\n");
		return;
	}
	else
	{
		if (cMsg.GetMsgBodyLen() != sizeof(TMSSynInfoRsp)) 
		{
			PrintMSException("[ProcMcuMcuStartSynRsp] Invalid msgLen != sizeof(TMSSynInfoRsp).%d\n", sizeof(TMSSynInfoRsp));
			//printf("[ProcMcuMcuStartSynRsp] Invalid msgLen != sizeof(TMSSynInfoRsp).%d\n", sizeof(TMSSynInfoRsp));
			return;
		}
		TMSSynInfoRsp *ptSynInfoRsp = (TMSSynInfoRsp*)cMsg.GetMsgBody();
		m_bRemoteInited = (1 == ptSynInfoRsp->m_byInited) ? TRUE : FALSE;
	}

	PrintMSInfo("[ProcMcuMcuStartSynRsp] Rcv_SynOpr_Ack StartSyn, RemoteInited :%d, CurTicks :%u\n", 
                m_bRemoteInited, OspTickGet());

	//printf("[ProcMcuMcuStartSynRsp] Rcv_SynOpr_Ack StartSyn, RemoteInited :%d, CurTicks :%u\n", 
     //           m_bRemoteInited, OspTickGet());

	//开始发送需同步数据
	SendSynData(TRUE);
	
	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuEndSynNtf
    功    能： 处理 主用板MCU向备用板MCU 结束数据倒换 通知
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuEndSynNtf( void )
{
	if (MS_STATE_NORMAL != CurState() || 
		MCU_MSSTATE_STANDBY != g_cMSSsnApp.GetCurMSState())
	{
		PrintMSWarning("[ProcMcuMcuEndSynNtf] Ingore: CurInstState.%d CurMSState.%d\n", 
			            CurState(), g_cMSSsnApp.GetCurMSState());

		//printf("[ProcMcuMcuEndSynNtf] Ingore: CurInstState.%d CurMSState.%d\n", 
		//	            CurState(), g_cMSSsnApp.GetCurMSState());
		return;
	}
	
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		//通知外层App 解锁主备模块初始化锁定
		g_cMSSsnApp.LeaveMSInitLock();
	}
	else
	{
		g_cMSSsnApp.SetMSSynOKFlag(TRUE);

        //备板同步成功点灭ous灯
        // zw [2008/10/13] 以及做一些信息同步的事情
        if (g_cMSSsnApp.GetCurMSState() == MCU_MSSTATE_STANDBY)
        {
            g_cMSSsnApp.SetMpcOusLed(FALSE);
        }     
	}

	//[MSMag <->McuVc]模块互锁，解锁 2005-12-15
	//IncompletePro: 备用板的解锁处理
	//g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);

	//清空当前倒换的数据信息记录
	memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));
	//备板的timeout，解锁
	g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);
	PrintMSInfo("[ProcMcuMcuEndSynNtf] EndSyn,Standby unlock, CurTicks :%u\n", OspTickGet());
	//printf("[ProcMcuMcuEndSynNtf] EndSyn, CurTicks :%u\n", OspTickGet());

	return;
}


/*=============================================================================
    函 数 名： ProcMcuMcuSynningDataReq
    功    能： 处理 主位板MCU向备位板MCU 实际切包组包的数据倒换 同步请求
	           包括1.系统时间、mcu代理配置信息、会议信息、用户信息、地址簿、debug文件 以及
			       2.业务内存数据
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuSynningDataReq( CMessage * const pcMsg )
{
	if (MS_STATE_NORMAL != CurState() || 
		MCU_MSSTATE_STANDBY != g_cMSSsnApp.GetCurMSState())
	{
		PrintMSWarning("[ProcMcuMcuSynningDataReq] Ingore: CurInstState.%d CurMSState.%d\n", 
			            CurState(), g_cMSSsnApp.GetCurMSState());
		return;
	}
	
	if( NULL == pcMsg )
	{
		PrintMSWarning("ProcMcuMcuSynningDataReq The received msg's pointer is NULL!\n");
		return;
	}

	//组帧
	if (FALSE == RecvCustomMsgFromOtherMcu(pcMsg))
	{
		return;
	}

	PrintMSInfo("[ProcMcuMcuSynningDataReq] RecvCustomMsgFromOtherMcu OK: MsgID.%d FrmLen.%u FrmSN.%d\n", 
		            m_tRcvFrmHead.m_byFrmType, m_tRcvFrmHead.m_dwFrmLen, m_tRcvFrmHead.m_wFrmSN);

	
	//同步并回应同步结果
	TMSSynDataRsp tMSDataRsp;
	tMSDataRsp.m_byMsgID  = m_tRcvFrmHead.m_byFrmType;
	tMSDataRsp.m_dwFrmLen = htonl(m_tRcvFrmHead.m_dwFrmLen);
	tMSDataRsp.m_wFrmSN   = htons(m_tRcvFrmHead.m_wFrmSN);
	if (DealOneCustomSynMsg(&m_tRcvFrmHead) )
	{
		tMSDataRsp.m_byRetVal = (u8)emMSReturnValue_Ok;

		PrintMSInfo("[ProcMcuMcuSynningDataReq] DealOneCustomSynMsg OK: MsgID.%d FrmLen.%u FrmSN.%d\n", 
			         m_tRcvFrmHead.m_byFrmType, m_tRcvFrmHead.m_dwFrmLen, m_tRcvFrmHead.m_wFrmSN);
	}
	else
	{
		tMSDataRsp.m_byRetVal = (u8)emMSReturnValue_Error;
		
		//IncompletePro: 备用板的解锁处理
		//g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);

		//清空当前倒换的数据信息记录
		//memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));

        //同步失败后点亮ous灯
        if (g_cMSSsnApp.GetCurMSState() == MCU_MSSTATE_STANDBY)
        {
            g_cMSSsnApp.SetMpcOusLed(TRUE);
        }
		
		PrintMSWarning("[ProcMcuMcuSynningDataReq] DealOneCustomSynMsg Failed!\n");
	}
	PostMsgToOtherMcuInst(MCU_MCU_SYNNING_DATA_RSP, (u8*)&tMSDataRsp, sizeof(tMSDataRsp));

	//清空处理后的完整接收帧信息
	m_dwRcvFrmLen = 0;
	m_tRcvFrmHead.m_byFrmType = emSynDataStart;
	m_tRcvFrmHead.m_dwFrmLen  = 0;
	
	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuSynningDataRsp
    功    能： 备位板MCU向主位板MCU 实际切包组包的数据倒换 同步结果 回应
	           包括1.系统时间、mcu代理配置信息、会议信息、用户信息、地址簿、debug文件 以及
			       2.业务内存数据
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::ProcMcuMcuSynningDataRsp( CMessage * const pcMsg )
{
	if (MS_STATE_NORMAL != CurState() || 
		MCU_MSSTATE_ACTIVE != g_cMSSsnApp.GetCurMSState())
	{
		return;
	}

	CServMsg cMsg( pcMsg->content, pcMsg->length );
	if (cMsg.GetMsgBodyLen() < sizeof(TMSSynDataRsp)) 
	{
		PrintMSException("[ProcMcuMcuSynningDataRsp] Invalid msgLen < sizeof(TMSSynDataRsp).%d\n", sizeof(TMSSynDataRsp));
		//printf("[ProcMcuMcuSynningDataRsp] Invalid msgLen < sizeof(TMSSynDataRsp).%d\n", sizeof(TMSSynDataRsp));
		return;
	}

	TMSSynDataRsp *ptMSDataRsp = (TMSSynDataRsp *)cMsg.GetMsgBody();
	ptMSDataRsp->m_dwFrmLen    = ntohl(ptMSDataRsp->m_dwFrmLen);
	ptMSDataRsp->m_wFrmSN      = ntohs(ptMSDataRsp->m_wFrmSN);
	
	PrintMSVerbose("SynnData: RetVal.%d MsgID.%d FrmLen.%u FrmSN.%d\n", 
		         ptMSDataRsp->m_byRetVal, ptMSDataRsp->m_byMsgID, ptMSDataRsp->m_dwFrmLen, ptMSDataRsp->m_wFrmSN);
	
	//printf("[ProcMcuMcuSynningDataRsp] RetVal.%d MsgID.%d FrmLen.%u FrmSN.%d\n", 
	//	         ptMSDataRsp->m_byRetVal, ptMSDataRsp->m_byMsgID, ptMSDataRsp->m_dwFrmLen, ptMSDataRsp->m_wFrmSN);
	
	//进行过期校验，过期则丢弃
	if (m_tSndFrmHead.m_byFrmType != ptMSDataRsp->m_byMsgID || 
		m_tSndFrmHead.m_wFrmSN != ptMSDataRsp->m_wFrmSN)
	{
		PrintMSException("[ProcMcuMcuSynningDataRsp] DataRsp Overdue! ReqMsgID.%d ReqFrmSN.%d RspMsgID.%d RspFrmSN.%d!\n", 
			              m_tSndFrmHead.m_byFrmType, m_tSndFrmHead.m_wFrmSN, 
						  ptMSDataRsp->m_byMsgID, ptMSDataRsp->m_wFrmSN);

		//printf("[ProcMcuMcuSynningDataRsp] DataRsp Overdue! ReqMsgID.%d ReqFrmSN.%d RspMsgID.%d RspFrmSN.%d!\n", 
		//			   	  m_tSndFrmHead.m_byFrmType, m_tSndFrmHead.m_wFrmSN, 
		//				  ptMSDataRsp->m_byMsgID, ptMSDataRsp->m_wFrmSN);
		return;
	}

	//关闭请求超时检测
	KillTimer(MCU_MCU_WAITFORRSP_TIMER);

	//主用板数据同步请求失败，暂不支持重传，等待下次同步
	if ((u8)emMSReturnValue_Ok != ptMSDataRsp->m_byRetVal)
	{
		//此时对端需重新进行完全同步，以恢复性间隔请求同步
		g_cMSSsnApp.SetMSSynOKFlag(FALSE);
		KillTimer(MCU_MCU_SYN_TIMER);
		SetTimer(MCU_MCU_SYN_TIMER, MS_SYN_MIN_SPAN_TIMEOUT);
		
		//[MSMag <->McuVc]模块互锁，解锁 2005-12-15
		g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);

		PrintMSException("SynnData:RetVal.%d!\n", ptMSDataRsp->m_byRetVal);
		//printf("[ProcMcuMcuSynningDataRsp] RetVal.%d!\n", ptMSDataRsp->m_byRetVal);
		return;
	}

	//继续发送其他的同步请求, 判别是否已经完成全部数据同步
	if (SendSynData(FALSE))
	{
		PostMsgToOtherMcuInst(MCU_MCU_END_SYN_NTF, NULL, 0);
		
		if (FALSE == g_cMSSsnApp.IsMSConfigInited())
		{
			//通知外层App 解锁主备模块初始化锁定
			g_cMSSsnApp.LeaveMSInitLock();
		}
		else
		{
			//此时对端已完全同步，以正常性间隔请求同步
			if (m_bRemoteInited)
			{
				g_cMSSsnApp.SetMSSynOKFlag(TRUE); 
                
                if ( !g_cMSSsnApp.IsMsSwitchOK() )
                {
                    // guzh [9/14/2006] 到此时，认为该主板也已经成功切换
                    g_cMSSsnApp.SetMsSwitchOK(TRUE);
                }

                //主板清除同步失败状态信息
                TMSSynState tMSSynState;                
                if ( !g_cMSSsnApp.GetCurMSSynState().IsSynSucceed() )
                {
                    g_cMSSsnApp.SetCurMSSynState(tMSSynState);
                }
			}
		}

		//主用板完成同步，[MSMag <->McuVc]模块互锁，解锁 2005-12-15
		g_cMSSsnApp.LeaveMSSynLock(AID_MCU_MSMANAGERSSN);

		PrintMSInfo("[ProcMcuMcuSynningDataRsp] Finish_SynOpr: Unlock, CurTick :%u\n", OspTickGet());
		//printf("[ProcMcuMcuSynningDataRsp] Finish_SynOpr: Unlock, CurTick :%u\n", OspTickGet());
	}
	
	return;
}


/*=============================================================================
    函 数 名： DealOneCustomSynMsg
    功    能： 处理数据倒换时的 一条组包完整的自定义主备同步消息
    算法实现： 
    全局变量： 
    参    数： TMSFrmHead *ptMSFrmHead 自定义消息体的消息头
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::DealOneCustomSynMsg( TMSFrmHead *ptMSFrmHead )
{
	BOOL32 bRet = FALSE;
	if( NULL == ptMSFrmHead )
	{
		PrintMSWarning("[DealOneCustomSynMsg] The received msg's pointer is NULL!\n");
		return bRet;
	}

	switch( ptMSFrmHead->m_byFrmType ) 
	{
	case emSysTime:
		bRet = SaveSynDataOfSysTime(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emSysCritData:
        bRet = SaveSynDataOfSysCritData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
        break;
	case emCfgFile:
		bRet = SaveSynDataOfCfgFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emDebugFile:
		bRet = SaveSynDataOfDebugFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
    case emAddrbookFile_Utf8:
        bRet = SaveSynDataOfAddrbookFileUtf8(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emAddrbookFile:
		bRet = SaveSynDataOfAddrbookFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emConfinfoFile:
		bRet = SaveSynDataOfConfinfoFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emUnProcConfinfoFile:
		bRet = SaveSynDataOfUnProcConfinfoFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emLoguserFile:
		bRet = SaveSynDataOfLoguserFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emVCSLoguserFile:
		bRet = SaveSynDataOfVCSLoguserFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emVCSUserTaskFile:
		bRet = SaveSynDataOfVCSUserTaskFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
    case emUserExFile:  // 用户组扩展信息
        bRet = SaveSynDataOfUserExFile(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
        break;

        
	case emVcDeamonPeriEqpData:
		bRet = SaveSynDataOfVcDeamonPeriEqpData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emVcDeamonMCData:
		bRet = SaveSynDataOfVcDeamonMCData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emVcDeamonMpData:
		bRet = SaveSynDataOfVcDeamonMpData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emVcDeamonMtadpData:
		bRet = SaveSynDataOfVcDeamonMtadpData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emVcDeamonTemplateData:
		bRet = SaveSynDataOfVcDeamonTemplateData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emVcDeamonOtherData:
		bRet = SaveSynDataOfVcDeamonOtherData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emAllVcInstState:
		bRet = SaveSynDataOfAllVcInstState(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emOneVcInstConfMtTableData:
		bRet = SaveSynDataOfOneVcInstConfMtTableData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emOneVcInstConfSwitchTableData:
		bRet = SaveSynDataOfOneVcInstConfSwitchTableData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emOneVcInstConfOtherMcuTableData:
		bRet = SaveSynDataOfOneVcInstConfOtherMcTableData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
	case emOneVcInstOtherData:
		bRet = SaveSynDataOfOneVcInstOtherData(ptMSFrmHead->m_pbyFrmBuf, ptMSFrmHead->m_dwFrmLen);
		break;
		
	default:
		PrintMSCritical("[MSManager] Wrong Custom MsgId%u(%s) received in DealOneCustomSynMsg()!\n", 
			             ptMSFrmHead->m_byFrmType, ::OspEventDesc(ptMSFrmHead->m_byFrmType));
		break;
	}

	return bRet;
}

/*=============================================================================
    函 数 名： SendCustomMsgToOtherMcu
    功    能： 向另一块MCU-Inst发送需数据倒换的消息，
	           若消息体长度超过 MS_MAX_PACKET_LEN ，则进行切包处理
    算法实现： 
    全局变量： 
    参    数： TMSFrmHead *ptSndFrmHead
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendCustomMsgToOtherMcu(TMSFrmHead *ptSndFrmHead)
{	
	BOOL32 bRet = TRUE;
	if (NULL == ptSndFrmHead)
	{
		PrintMSWarning("[SendCustomMsgToOtherMcu] invalid msg, Failed\n");
		//printf("[SendCustomMsgToOtherMcu] invalid msg, Failed\n");
		return bRet;
	}

	u8  byPackNum  = (u8)((ptSndFrmHead->m_dwFrmLen+MS_MAX_PACKET_LEN-1)/MS_MAX_PACKET_LEN);
	u8 *pbyPackBuf = ptSndFrmHead->m_pbyFrmBuf;
	u16 wPackLen   = MS_MAX_PACKET_LEN;
	TMSSynDataReqHead *ptMSDataHead = NULL;
	if (0 == byPackNum)
	{
		byPackNum = 1;
	}
	
	//n-1包
	for (u8 byLoop = 1; byLoop < byPackNum; byLoop++)
	{
		ptMSDataHead = (TMSSynDataReqHead *)(pbyPackBuf-sizeof(TMSSynDataReqHead));
		ptMSDataHead->m_byMsgID   = ptSndFrmHead->m_byFrmType;
		ptMSDataHead->m_dwFrmLen  = htonl(ptSndFrmHead->m_dwFrmLen);
		ptMSDataHead->m_wFrmSN    = htons(ptSndFrmHead->m_wFrmSN);
		ptMSDataHead->m_byPackIdx = byLoop-1;
		ptMSDataHead->m_byMark    = 0;
		if (FALSE == PostMsgToOtherMcuInst(MCU_MCU_SYNNING_DATA_REQ, 
			                               (u8*)ptMSDataHead, wPackLen+sizeof(TMSSynDataReqHead)))
		{
			PrintMSException("[SendCustomMsgToOtherMcu] post err: MsgID.%d TotalLen.%u CurPackLen.%d CurPackIdx.%d Failed\n", 
				              ptMSDataHead->m_byMsgID, ptMSDataHead->m_dwFrmLen, wPackLen, ptMSDataHead->m_byPackIdx);

			//printf("[SendCustomMsgToOtherMcu] post err: MsgID.%d TotalLen.%u CurPackLen.%d CurPackIdx.%d Failed\n", 
			//	              ptMSDataHead->m_byMsgID, ptMSDataHead->m_dwFrmLen, wPackLen, ptMSDataHead->m_byPackIdx);
			return bRet;
		}
		pbyPackBuf += MS_MAX_PACKET_LEN;
	}
	
	//最后一包
	//允许整帧长度为0
	ptMSDataHead = (TMSSynDataReqHead *)(pbyPackBuf-sizeof(TMSSynDataReqHead));
	wPackLen    = (u16)(ptSndFrmHead->m_dwFrmLen + ptSndFrmHead->m_pbyFrmBuf - pbyPackBuf);
	ptMSDataHead->m_byMsgID   = ptSndFrmHead->m_byFrmType;
	ptMSDataHead->m_dwFrmLen  = htonl(ptSndFrmHead->m_dwFrmLen);
	ptMSDataHead->m_wFrmSN    = htons(ptSndFrmHead->m_wFrmSN);
	ptMSDataHead->m_byPackIdx = byPackNum-1;
	ptMSDataHead->m_byMark    = 1;
	if (FALSE == PostMsgToOtherMcuInst(MCU_MCU_SYNNING_DATA_REQ, 
		                               (u8*)ptMSDataHead, wPackLen+sizeof(TMSSynDataReqHead)))
	{
		PrintMSException("[SendCustomMsgToOtherMcu] post err: MsgID.%d TotalLen.%u CurPackLen.%d CurPackIdx.%d Failed\n", 
			              ptMSDataHead->m_byMsgID, ptMSDataHead->m_dwFrmLen, wPackLen, ptMSDataHead->m_byPackIdx);

		//printf("[SendCustomMsgToOtherMcu] post err: MsgID.%d TotalLen.%u CurPackLen.%d CurPackIdx.%d Failed\n", 
		//	              ptMSDataHead->m_byMsgID, ptMSDataHead->m_dwFrmLen, wPackLen, ptMSDataHead->m_byPackIdx);
		return bRet;
	}
	
	bRet = TRUE;
	return bRet;
}

/*=============================================================================
    函 数 名： RecvCustomMsgFromOtherMcu
    功    能： 接收另一块MCU-Inst的消息（消息体完整重组）
	           不处理乱序、丢包等情况，一律做接收失败处理
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： TMSFrmHead *
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
TMSFrmHead *CMSManagerSsnInst::RecvCustomMsgFromOtherMcu( CMessage * const pcMsg )
{
	CServMsg cMsg( pcMsg->content ,pcMsg->length );
	TMSFrmHead *ptMSFrmHead = NULL;

	//允许整帧长度为0
	if (cMsg.GetMsgBodyLen() < sizeof(TMSSynDataReqHead)) 
	{
		PrintMSException("[RecvCustomMsgFromOtherMcu] Invalid msgLen < sizeof(TMSSynDataReqHead).%d\n", sizeof(TMSSynDataReqHead));
		return ptMSFrmHead;
	}

	u16 wPackLen   = cMsg.GetMsgBodyLen() - sizeof(TMSSynDataReqHead);
	u8 *pbyPackBuf = cMsg.GetMsgBody() + sizeof(TMSSynDataReqHead);
	TMSSynDataReqHead *ptMSDataHead = (TMSSynDataReqHead *)cMsg.GetMsgBody();
	ptMSDataHead->m_dwFrmLen        = ntohl(ptMSDataHead->m_dwFrmLen);
	ptMSDataHead->m_wFrmSN          = ntohs(ptMSDataHead->m_wFrmSN);

	if (ptMSDataHead->m_dwFrmLen > MS_MAX_FRAME_LEN || 
		ptMSDataHead->m_byPackIdx >= MS_MAX_PACKET_NUM || 
		ptMSDataHead->m_byMsgID == (u8)emSynDataStart || 
		ptMSDataHead->m_byMsgID >= (u8)emSynDataEnd)
	{
		PrintMSException("[RecvCustomMsgFromOtherMcu] Invalid msgInfo FrmLen.%u PackIdx.%d MsgID.%d\n", 
			              ptMSDataHead->m_dwFrmLen, ptMSDataHead->m_byPackIdx, ptMSDataHead->m_byMsgID);
		return ptMSFrmHead;
	}
	if ((0 == ptMSDataHead->m_byMark && wPackLen != MS_MAX_PACKET_LEN) || 
		(1 == ptMSDataHead->m_byMark && wPackLen >  MS_MAX_PACKET_LEN))
	{
		PrintMSException("[RecvCustomMsgFromOtherMcu] Invalid msgLen FrmLen.%u PackLen.%d PackIdx.%d Mark.%d MsgID.%d\n", 
			              ptMSDataHead->m_dwFrmLen, wPackLen, ptMSDataHead->m_byPackIdx, 
					      ptMSDataHead->m_byMark, ptMSDataHead->m_byMsgID);
		return ptMSFrmHead;
	}

	if (m_tRcvFrmHead.m_byFrmType != ptMSDataHead->m_byMsgID || 
		m_tRcvFrmHead.m_wFrmSN != ptMSDataHead->m_wFrmSN || 
		0 == ptMSDataHead->m_byPackIdx)
	{
		if (emSynDataStart != m_tRcvFrmHead.m_byFrmType)
		{
			PrintMSException("[RecvCustomMsgFromOtherMcu] Lost One Incomplete Msg : FrmLen.%u RcvFrmLen.%u MsgID.%d\n", 
				              m_tRcvFrmHead.m_dwFrmLen, m_dwRcvFrmLen, m_tRcvFrmHead.m_byFrmType);
		}
		m_dwRcvFrmLen = 0;
		m_tRcvFrmHead.m_byFrmType = ptMSDataHead->m_byMsgID;
		m_tRcvFrmHead.m_dwFrmLen  = ptMSDataHead->m_dwFrmLen;
		m_tRcvFrmHead.m_wFrmSN    = ptMSDataHead->m_wFrmSN;
	}
	u32 dwOffset = MS_MAX_PACKET_LEN*ptMSDataHead->m_byPackIdx;
	if ((dwOffset+wPackLen) > m_tRcvFrmHead.m_dwFrmLen || 
		(m_dwRcvFrmLen+wPackLen) > m_tRcvFrmHead.m_dwFrmLen)
	{
		PrintMSException("[RecvCustomMsgFromOtherMcu] Lost One Msg FrmLen.%u RcvFrmLen.%u PackLen.%d PackIdx.%d MsgID.%d\n", 
			              m_tRcvFrmHead.m_dwFrmLen, m_dwRcvFrmLen, 
					      wPackLen, ptMSDataHead->m_byPackIdx, m_tRcvFrmHead.m_byFrmType);
		return ptMSFrmHead;
	}

	memcpy((m_tRcvFrmHead.m_pbyFrmBuf+dwOffset), pbyPackBuf, wPackLen);
	m_dwRcvFrmLen += wPackLen;
	if (m_dwRcvFrmLen == m_tRcvFrmHead.m_dwFrmLen)
	{
		ptMSFrmHead = &m_tRcvFrmHead;
	}
	
	return ptMSFrmHead;
}

/*=============================================================================
    函 数 名： SendSynData
    功    能： 发送 同步mcu各类数据 通知（全体数据同步倒换）
    算法实现： 
    全局变量： 
    参    数： BOOL32 bStart TRUE-开始发送，初始化状态 FALSE-继续发送
    返 回 值： BOOL32 TRUE-所有同步均完成，FALSE-未全部完成
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynData( BOOL32 bStart /*= FALSE*/ )
{
	BOOL32 bEndFlag = FALSE;

	//同步起始点：若对端需进行完全同步，同步配置信息及会议公用区业务数据，
	//                            否则  直接同步会议公用区业务数据
	//2005-12-27 仍旧改为全体同步
	if (bStart)
	{
		memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));

		/*
		if (g_cMSSsnApp.IsMSConfigInited() && 
			g_cMSSsnApp.IsMSSynOK())
		{
			m_tMSCurSynState.m_emSynType = emVcDeamonPeriEqpData;
		}
		else
		*/
		{
			m_tMSCurSynState.m_emSynType = emSysTime;
		}
	}
	else
	{
        // guzh [4/18/2007] 如果正在倒换预约会议，则跳过交换表、MC表
        if ( emOneVcInstConfMtTableData == m_tMSCurSynState.m_emSynType && 
            CONF_TAKEMODE_ONGOING != g_cMcuVcApp.GetTakeModeOfInstId(m_tMSCurSynState.m_byInstID) ) 
        {
            m_tMSCurSynState.m_emSynType = emOneVcInstOtherData;
        }
        else
        {
            m_tMSCurSynState.m_emSynType = (emgMSSynDataType)(m_tMSCurSynState.m_emSynType+1);
        }
	}

	if (m_tMSCurSynState.m_emSynType <= emAllVcInstState)
	{
		//发送配置信息及会议公用区业务数据
		m_tMSCurSynState.m_byInstID  = 0;

		if (FALSE == g_cMSSsnApp.IsMSConfigInited() && 
			emVcDeamonPeriEqpData == m_tMSCurSynState.m_emSynType)
		{
			//同步结束点：若本端未完成初始化，业务App尚未创建，结束同步
			memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));
			bEndFlag = TRUE;
		}
	}
	else
	{
		//发送会议实例区业务数据（存在多个需同步的会议实例）
		u8 byInstID = m_tMSCurSynState.m_byInstID;
		if (emSynDataEnd == m_tMSCurSynState.m_emSynType || 
			emOneVcInstConfMtTableData == m_tMSCurSynState.m_emSynType)
		{
			m_tMSCurSynState.m_emSynType = emOneVcInstConfMtTableData;
			//Find Next No-Idle-Inst ID
			byInstID = g_cMcuVcApp.FindNextConfInstIDOfNotIdle( byInstID );
		}

		if (0 != byInstID)
		{
			m_tMSCurSynState.m_byInstID = byInstID;
		}
		else
		{
			//同步结束点：若本端已完成初始化，完成所有需同步的会议实例，结束同步
			memset((void*)&m_tMSCurSynState, 0, sizeof(m_tMSCurSynState));
			bEndFlag = TRUE;
		}
	}

	if (FALSE == bEndFlag)
	{
		if (SendSynData(m_tMSCurSynState))
		{
			PrintMSInfo("[SendSynData] OK: EndFlag.%d Start.%d SynTypeID.%u InstID.%d!\n", 
				         bEndFlag, bStart, m_tMSCurSynState.m_emSynType, m_tMSCurSynState.m_byInstID);
		}
		else
		{
			PrintMSWarning("[SendSynData] Failed: bStart.%d SynTypeID.%u InstID.%d\n", 
				           bStart, m_tMSCurSynState.m_emSynType, m_tMSCurSynState.m_byInstID);
		}
	}
	
	return bEndFlag;
}

/*=============================================================================
    函 数 名： SendSynData
    功    能： 发送 指定类型的同步数据 通知
    算法实现： 
    全局变量： 
    参    数： TMSCurSynState tMSCurSynState
    返 回 值： BOOL32 TRUE-发送成功
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynData( TMSCurSynState tMSCurSynState )
{
	BOOL32 bRet = FALSE;

	//check valid
	if (tMSCurSynState.m_emSynType <= emSynDataStart || tMSCurSynState.m_emSynType >= emSynDataEnd)
	{
		PrintMSWarning("[SendSynData] Invalid argument SynTypeID.%u InstID.%d failed!\n", 
			            tMSCurSynState.m_emSynType, tMSCurSynState.m_byInstID);
		return bRet;
	}
	
	switch(tMSCurSynState.m_emSynType)
	{
	//发送配置信息数据
	case emSysTime:
		bRet = SendSynDataOfSysTime();
		break;
	case emSysCritData:
        bRet = SendSynDataOfSysCritData();
        break;
	case emCfgFile:
		bRet = SendSynDataOfCfgFile();
		break;
	case emDebugFile:
		bRet = SendSynDataOfDebugFile();
		break;
    case emAddrbookFile_Utf8:
        bRet = SendSynDataOfAddrbookFileUtf8();
		break;
	case emAddrbookFile:
		bRet = SendSynDataOfAddrbookFile();
		break;
	case emConfinfoFile:
		bRet = SendSynDataOfConfinfoFile();
		break;
	case emUnProcConfinfoFile:
		bRet = SendSynDataOfUnProcConfinfoFile();
		break;
	case emLoguserFile:
		bRet = SendSynDataOfLoguserFile();
		break;
	case emVCSLoguserFile:
		bRet = SendSynDataOfVCSLoguserFile();
		break;
	case emVCSUserTaskFile:
		bRet = SendSynDataOfVCSUserTaskFile();
		break;
    case emUserExFile:
        bRet = SendSynDataOfUserExFile();
        break;

	//发送会议公用区业务数据
	case emVcDeamonPeriEqpData:
		bRet = SendSynDataOfVcDeamonPeriEqpData();
		break;
	case emVcDeamonMCData:
		bRet = SendSynDataOfVcDeamonMCData();
		break;
	case emVcDeamonMpData:
		bRet = SendSynDataOfVcDeamonMpData();
		break;
	case emVcDeamonMtadpData:
		bRet = SendSynDataOfVcDeamonMtadpData();
		break;
	case emVcDeamonTemplateData:
		bRet = SendSynDataOfVcDeamonTemplateData();
		break;		
	case emVcDeamonOtherData:
		bRet = SendSynDataOfVcDeamonOtherData();
		break;
	case emAllVcInstState:
		bRet = SendSynDataOfAllVcInstState();
		break;

	//发送指定会议实例区业务数据
	case emOneVcInstConfMtTableData:
		bRet = SendSynDataOfOneVcInstConfMtTableData(tMSCurSynState.m_byInstID);
		break;
	case emOneVcInstConfSwitchTableData:
		bRet = SendSynDataOfOneVcInstConfSwitchTableData(tMSCurSynState.m_byInstID);
		break;
	case emOneVcInstConfOtherMcuTableData:
		bRet = SendSynDataOfOneVcInstConfOtherMcTableData(tMSCurSynState.m_byInstID);
		break;
	case emOneVcInstOtherData:
		bRet = SendSynDataOfOneVcInstOtherData(tMSCurSynState.m_byInstID);
		break;

	default:
		PrintMSCritical("[SendSynData] Wrong Custom SynTypeID.%u InstID.%d failed!\n", 
			             tMSCurSynState.m_emSynType, tMSCurSynState.m_byInstID);
		break;
	}
	
	if (bRet)
	{
        // guzh [9/25/2006] 同步过程中缩短等待回应超时，
        // 防止锁定过长时间无法检测到对端断链
		SetTimer(MCU_MCU_WAITFORRSP_TIMER, MS_WAITFOTRSP_TIMEOUT, (u32)MCU_MCU_SYNNING_DATA_REQ);
	}
	
	return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfSysTime
    功    能： 发送 同步mcu系统时间 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfSysTime( void )
{
	TKdvTime tCurMcuTime;
	time_t tMcuTime;
	
	::time( &tMcuTime );
	tCurMcuTime.SetTime( &tMcuTime );
	
	m_tSndFrmHead.m_byFrmType = emSysTime;
	m_tSndFrmHead.m_dwFrmLen  = sizeof(tCurMcuTime);
	m_tSndFrmHead.m_wFrmSN   += 1;
	memcpy(m_tSndFrmHead.m_pbyFrmBuf, (u8*)&tCurMcuTime, sizeof(tCurMcuTime) );
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfSysTime
    功    能： 处理 同步mcu系统时间 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfSysTime( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	//check valid
	if (NULL == pbyBuf || sizeof(TKdvTime) != dwBufLen)
	{
		PrintMSWarning("[SaveSynDataOfSysTime] invalid argument BufLen.%d\n", dwBufLen);
		return bRet;
	}
	
    TKdvTime *ptTime = (TKdvTime *)pbyBuf;
    time_t tTime;
    ptTime->GetTime(tTime);
	bRet = g_cMSSsnApp.SetLocalMcuTime(tTime);
    if (FALSE == bRet)
    {
        PrintMSWarning("[SaveSynDataOfSysTime] change system time failed\n");
    }
	
	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emSysTime;
		m_tMSCurSynState.m_byInstID  = 0;
	}
		
    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfSysTime
    功    能： 发送 mcu系统关键数据 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2007/6/11  4.0			顾振华                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfSysCritData( void )
{
    u32 dwSysSSrc = htonl(g_cMSSsnApp.GetMSSsrc());
    
    m_tSndFrmHead.m_byFrmType = emSysCritData;
    m_tSndFrmHead.m_dwFrmLen  = sizeof(dwSysSSrc);
    m_tSndFrmHead.m_wFrmSN   += 1;
    memcpy(m_tSndFrmHead.m_pbyFrmBuf, (u8*)&dwSysSSrc, sizeof(dwSysSSrc) );
    
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfSysCritData
    功    能： 处理 同步mcu系统关键数据
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfSysCritData( u8 *pbyBuf, u32 dwBufLen )
{
    BOOL32 bRet = TRUE;
    //check valid
    if (NULL == pbyBuf || sizeof(u32) != dwBufLen)
    {
        PrintMSWarning("[SaveSynDataOfSysCritData] invalid argument BufLen.%d\n", dwBufLen);
        return bRet;
    }
    
    u32 dwSysSSrc = ntohl(*(u32*)pbyBuf);
    g_cMSSsnApp.SetMSSsrc(dwSysSSrc);
    
    if (bRet)
    {
        m_tMSCurSynState.m_emSynType = emSysCritData;
        m_tMSCurSynState.m_byInstID  = 0;
    }
    
    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfCfgFile
    功    能： 发送 同步mcu/conf下的 mcucfg.ini 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfCfgFile( void )
{
	u32 dwOutBufLen = 0;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;

	if (FALSE == g_cMcuVcApp.GetCfgFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                    MS_MAX_FRAME_LEN, dwOutBufLen, bPrintErr))
	{
		PrintMSWarning("[SendSynDataOfCfgFile] GetCfgFileData failed\n");
		return FALSE;
	}
	
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emCfgFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfCfgFile
    功    能： 处理 同步mcu/conf下的 mcucfg.ini 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfCfgFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
	
	bRet = g_cMcuVcApp.SetCfgFileData(pbyBuf, dwBufLen, bPrintErr);
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfCfgFile] SetCfgFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emCfgFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfDebugFile
    功    能： 发送 同步mcu/conf下的 mcudebug.ini 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfDebugFile( void )
{
	u32 dwOutBufLen = 0;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;

	if (FALSE == g_cMcuVcApp.GetDebugFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                      MS_MAX_FRAME_LEN, dwOutBufLen, bPrintErr))
	{
		PrintMSWarning("[SendSynDataOfDebugFile] GetDebugFileData failed\n");
		return FALSE;
	}
	
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emDebugFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfDebugFile
    功    能： 处理 同步mcu/conf下的 mcudebug.ini 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfDebugFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
	
	bRet = g_cMcuVcApp.SetDebugFileData(pbyBuf, dwBufLen, bPrintErr);
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfDebugFile] SetDebugFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emDebugFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfAddrbookFileUtf8
    功    能： 发送 同步mcu/conf下的 kdvaddrbook.kdv 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2013/07/31  4.7			liaokang                创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfAddrbookFileUtf8( void )
{
    u32 dwOutBufLen = 0;
    BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
    
    if (FALSE == g_cMcuVcApp.GetAddrbookFileDataUtf8(m_tSndFrmHead.m_pbyFrmBuf, 
        MS_MAX_FRAME_LEN, dwOutBufLen, bPrintErr))
    {
        PrintMSWarning("[SendSynDataOfAddrbookFile] GetAddrbookFileData failed\n");
        return FALSE;
    }
    
    m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
    m_tSndFrmHead.m_byFrmType = emAddrbookFile_Utf8;
    m_tSndFrmHead.m_wFrmSN   += 1;
    
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SendSynDataOfAddrbookFile
    功    能： 发送 同步mcu/conf下的 addrbook.kdv 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfAddrbookFile( void )
{
	u32 dwOutBufLen = 0;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
	
	if (FALSE == g_cMcuVcApp.GetAddrbookFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                         MS_MAX_FRAME_LEN, dwOutBufLen, bPrintErr))
	{
		PrintMSWarning("[SendSynDataOfAddrbookFile] GetAddrbookFileData failed\n");
		return FALSE;
	}
	
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emAddrbookFile;
	m_tSndFrmHead.m_wFrmSN   += 1;

	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfAddrbookFileUtf8
    功    能： 处理 同步mcu/conf下的 kdvaddrbook.kdv 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
               u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2013/07/31  4.7			liaokang                创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfAddrbookFileUtf8( u8 *pbyBuf, u32 dwBufLen )
{
    BOOL32 bRet = FALSE;
    BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
    
    bRet = g_cMcuVcApp.SetAddrbookFileDataUtf8(pbyBuf, dwBufLen, bPrintErr);
    if (FALSE == bRet)
    {
        PrintMSWarning("[SaveSynDataOfAddrbookFile] SetAddrbookFileData BufLen.%d failed\n", dwBufLen);
    }
    else
    {
        m_tMSCurSynState.m_emSynType = emAddrbookFile_Utf8;
        m_tMSCurSynState.m_byInstID  = 0;
    }
    
    return bRet;
}

/*=============================================================================
    函 数 名： SaveSynDataOfAddrbookFile
    功    能： 处理 同步mcu/conf下的 addrbook.kdv 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfAddrbookFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
	
	bRet = g_cMcuVcApp.SetAddrbookFileData(pbyBuf, dwBufLen, bPrintErr);
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfAddrbookFile] SetAddrbookFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emAddrbookFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}

    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfConfinfoFile
    功    能： 发送 同步mcu/data下的 confinfo.dat 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfConfinfoFile( void )
{
	u32 dwOutBufLen = 0;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
	
	if (FALSE == g_cMcuVcApp.GetConfinfoFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                         MS_MAX_FRAME_LEN, dwOutBufLen, bPrintErr))
	{
		PrintMSWarning("[SendSynDataOfConfinfoFile] GetConfinfoFileData failed\n");
		return FALSE;
	}
	
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emConfinfoFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfConfinfoFile
    功    能： 处理 同步mcu/data下的 confinfo.dat 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfConfinfoFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;
	   
	bRet = g_cMcuVcApp.SetConfinfoFileData(pbyBuf, dwBufLen, bPrintErr);    
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfConfinfoFile] SetConfinfoFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emConfinfoFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}
/*=============================================================================
    函 数 名： SendSynDataOfUnProcConfinfoFile
    功    能： 发送 同步mcu/data下的 unprocconfinfo.dat信息
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2009/04/03  4.0			付秀华                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfUnProcConfinfoFile( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetUnProcConfinfoFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                               MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfUnProcConfinfoFile] GetUnProcConfinfoFileData failed\n");
		return FALSE;
	}

	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emUnProcConfinfoFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}
/*=============================================================================
    函 数 名： SaveSynDataOfUnProcConfinfoFile
    功    能： 接收 同步mcu/data下的 unprocconfinfo.dat信息
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2009/04/03  4.0			付秀华                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfUnProcConfinfoFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;	   
	bRet = g_cMcuVcApp.SetUnProcConfinfoFileData(pbyBuf, dwBufLen);    
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfConfinfoFile] SetUnProcConfinfoFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emUnProcConfinfoFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}
/*=============================================================================
    函 数 名： SendSynDataOfVCSLoguserFile
    功    能： 发送 同步mcu/data下的 vcslogin.usr信息
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2009/04/03  4.0			付秀华                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVCSLoguserFile( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVCSLoguserFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                           MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVCSLoguserFile] GetVCSLoguserFileData failed\n");
		return FALSE;
	}

	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emVCSLoguserFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}
/*=============================================================================
    函 数 名： SaveSynDataOfVCSLoguserFile
    功    能： 接收 同步mcu/data下的 vcslogin.usr信息
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2009/04/03  4.0			付秀华                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVCSLoguserFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;	   
	bRet = g_cMcuVcApp.SetVCSLoguserFileData(pbyBuf, dwBufLen);    
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfVCSLoguserFile] SetVCSLoguserFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emVCSLoguserFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}
/*=============================================================================
    函 数 名： SendSynDataOfVCSUserTaskFile
    功    能： 发送 同步mcu/data下的 usrtask.dat 信息
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2009/04/03  4.0			付秀华                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVCSUserTaskFile( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVCSUserTaskFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                            MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVCSUserTaskFile] GetVCSUserTaskFileData failed\n");
		return FALSE;
	}

	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emVCSUserTaskFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}
/*=============================================================================
    函 数 名： SaveSynDataOfVCSUserTaskFile
    功    能： 接收 同步mcu/data下的 usrtask.dat 信息
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2009/04/03  4.0			付秀华                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVCSUserTaskFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;	   
	bRet = g_cMcuVcApp.SetVCSUserTaskFileData(pbyBuf, dwBufLen);    
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfVCSUserTaskFile] SetVCSUserTaskFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emVCSUserTaskFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}
/*=============================================================================
    函 数 名： SendSynDataOfLoguserFile
    功    能： 发送 同步mcu/data下的 login.usr 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfLoguserFile( void )
{
	u32 dwOutBufLen = 0;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;

	if (FALSE == g_cMcuVcApp.GetLoguserFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                        MS_MAX_FRAME_LEN, dwOutBufLen, bPrintErr))
	{
		PrintMSWarning("[SendSynDataOfLoguserFile] GetLoguserFileData failed\n");
		return FALSE;
	}

	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emLoguserFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfLoguserFile
    功    能： 处理 同步mcu/data下的 login.usr 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pSetVcDeamonPeriEqpDatabyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfLoguserFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;

	bRet = g_cMcuVcApp.SetLoguserFileData(pbyBuf, dwBufLen, bPrintErr);
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfLoguserFile] SetLoguserFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emLoguserFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}


/*=============================================================================
    函 数 名： SendSynDataOfUserExFile
    功    能： 发送 同步mcu/data下的 usrgrp.usr 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/06/22  4.0			顾振华                创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfUserExFile( void )
{
	u32 dwOutBufLen = 0;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;

	if (FALSE == g_cMcuVcApp.GetUserExFileData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                        MS_MAX_FRAME_LEN, dwOutBufLen, bPrintErr))
	{
		PrintMSWarning("[SendSynDataOfLoguserFile] GetLoguserFileData failed\n");
		return FALSE;
	}

	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_byFrmType = emUserExFile;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfUserExFile
    功    能： 处理 同步mcu/data下的 usrgrp.usr 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pSetVcDeamonPeriEqpDatabyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/06/22  4.0			顾振华                创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfUserExFile( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	BOOL32 bPrintErr = (MS_DEBUG_WARNING <= g_cMSSsnApp.GetDebugLevel()) ? TRUE : FALSE;

	bRet = g_cMcuVcApp.SetUserExFileData(pbyBuf, dwBufLen, bPrintErr);
	if (FALSE == bRet)
	{
		PrintMSWarning("[SaveSynDataOfLoguserFile] SetLoguserFileData BufLen.%d failed\n", dwBufLen);
	}
	else
	{
		m_tMSCurSynState.m_emSynType = emUserExFile;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}


/*=============================================================================
    函 数 名： SendSynDataOfVcDeamonPeriEqpData
    功    能： 发送 同步mcu业务Deamon_TPeriEqpData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVcDeamonPeriEqpData( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVcDeamonPeriEqpData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                            MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVcDeamonPeriEqpData] GetVcDeamonPeriEqpData failed\n");
		return FALSE;
	}
	m_tSndFrmHead.m_byFrmType = emVcDeamonPeriEqpData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfVcDeamonPeriEqpData
    功    能： 处理 同步mcu业务Deamon_TPeriEqpData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVcDeamonPeriEqpData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	
	//若对端主用，本端备用，同步时尚未完成初始化，直接返回，后续同步
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfVcDeamonPeriEqpData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		bRet = g_cMcuVcApp.SetVcDeamonPeriEqpData( pbyBuf, dwBufLen );
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfVcDeamonPeriEqpData] SetVcDeamonPeriEqpData BufLen.%d failed\n", dwBufLen);
		}
	}

	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emVcDeamonPeriEqpData;
		m_tMSCurSynState.m_byInstID  = 0;
	}

    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfVcDeamonMCData
    功    能： 发送 同步mcu业务Deamon_TMCData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVcDeamonMCData( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVcDeamonMCData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                       MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVcDeamonMCData] GetVcDeamonMCData failed\n");
		return FALSE;
	}
	m_tSndFrmHead.m_byFrmType = emVcDeamonMCData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfVcDeamonMCData
    功    能： 处理 同步mcu业务Deamon_TMCData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVcDeamonMCData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	
	//若对端主用，本端备用，同步时尚未完成初始化，直接返回，后续同步
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfVcDeamonMCData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		bRet = g_cMcuVcApp.SetVcDeamonMCData( pbyBuf, dwBufLen );
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfVcDeamonMCData] SetVcDeamonMCData BufLen.%d failed\n", dwBufLen);
		}
	}

	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emVcDeamonMCData;
		m_tMSCurSynState.m_byInstID  = 0;
	}

    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfVcDeamonMpData
    功    能： 发送 同步mcu业务Deamon_TMpData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVcDeamonMpData( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVcDeamonMpData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                       MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVcDeamonMpData] GetVcDeamonMpData failed\n");
		return FALSE;
	}
	m_tSndFrmHead.m_byFrmType = emVcDeamonMpData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfVcDeamonMpData
    功    能： 处理 同步mcu业务Deamon_TMpData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVcDeamonMpData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	
	//若对端主用，本端备用，同步时尚未完成初始化，直接返回，后续同步
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfVcDeamonMpData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		bRet = g_cMcuVcApp.SetVcDeamonMpData( pbyBuf, dwBufLen );
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfVcDeamonMpData] SetVcDeamonMpData BufLen.%d failed\n", dwBufLen);
		}
	}

	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emVcDeamonMpData;
		m_tMSCurSynState.m_byInstID  = 0;
	}

    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfVcDeamonMtadpData
    功    能： 发送 同步mcu业务Deamon_TMtadpData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVcDeamonMtadpData( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVcDeamonMtadpData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                          MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVcDeamonMtadpData] GetVcDeamonMtadpData failed\n");
		return FALSE;
	}
	m_tSndFrmHead.m_byFrmType = emVcDeamonMtadpData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfVcDeamonMtadpData
    功    能： 处理 同步mcu业务Deamon_TMtadpData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVcDeamonMtadpData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	
	//若对端主用，本端备用，同步时尚未完成初始化，直接返回，后续同步
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfVcDeamonMtadpData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		bRet = g_cMcuVcApp.SetVcDeamonMtadpData( pbyBuf, dwBufLen );
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfVcDeamonMtadpData] SetVcDeamonMtadpData BufLen.%d failed\n", dwBufLen);
		}
	}

	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emVcDeamonMtadpData;
		m_tMSCurSynState.m_byInstID  = 0;
	}

    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfVcDeamonTemplateData
    功    能： 发送 同步mcu业务Deamon_TTemplateData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVcDeamonTemplateData( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVcDeamonTemplateData(m_tSndFrmHead.m_pbyFrmBuf, 
		                                             MS_MAX_FRAME_LEN, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVcDeamonTemplateData] GetVcDeamonTemplateData failed\n");
		return FALSE;
	}
	m_tSndFrmHead.m_byFrmType = emVcDeamonTemplateData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfVcDeamonTemplateData
    功    能： 处理 同步mcu业务Deamon_TTemplateData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVcDeamonTemplateData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfVcDeamonTemplateData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		bRet = g_cMcuVcApp.SetVcDeamonTemplateData( pbyBuf, dwBufLen );
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfVcDeamonTemplateData] SetVcDeamonTemplateData BufLen.%d failed\n", dwBufLen);
		}
	}
	
	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emVcDeamonTemplateData;
		m_tMSCurSynState.m_byInstID  = 0;
	}
	
    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfVcDeamonOtherData
    功    能： 发送 同步mcu业务VcDeamon_OtherData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfVcDeamonOtherData( void )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetVcDeamonOtherData(m_tSndFrmHead.m_pbyFrmBuf, dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfVcDeamonOtherData] GetVcDeamonOtherData failed\n");
		return FALSE;
	}
	m_tSndFrmHead.m_byFrmType = emVcDeamonOtherData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen;
	m_tSndFrmHead.m_wFrmSN   += 1;
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfVcDeamonOtherData
    功    能： 处理 同步mcu业务VcDeamon_OtherData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfVcDeamonOtherData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfVcDeamonOtherData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		bRet = g_cMcuVcApp.SetVcDeamonOtherData( pbyBuf );
		if (FALSE == bRet)
		{
			PrintMSException("[SaveSynDataOfVcDeamonOtherData] SetVcDeamonOtherData BufLen.%d failed\n", dwBufLen);
		}
	}
	
	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emVcDeamonOtherData;
		m_tMSCurSynState.m_byInstID  = 0;
	}

    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfAllVcInstState
    功    能： 发送 同步mcu业务All_VcInst_State内存数据 通知
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfAllVcInstState( void )
{
	TMSVcInstState tMSVcInstState;
	if (FALSE == g_cMcuVcApp.GetAllVcInstState(tMSVcInstState))
	{
		PrintMSWarning("[SendSynDataOfAllVcInstState] GetAllVcInstState failed\n");
		return FALSE;
	}
	m_tSndFrmHead.m_byFrmType = emAllVcInstState;
	m_tSndFrmHead.m_dwFrmLen  = sizeof(TMSVcInstState);
	m_tSndFrmHead.m_wFrmSN   += 1;
	memcpy(m_tSndFrmHead.m_pbyFrmBuf, (s8*)&tMSVcInstState, sizeof(tMSVcInstState));
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfAllVcInstState
    功    能： 处理 同步mcu业务All_VcInst_State内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfAllVcInstState( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	TMSVcInstState tMSVcInstState;
	
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfAllVcInstState] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		//check valid
		if (NULL == pbyBuf || sizeof(TMSVcInstState) != dwBufLen)
		{
			PrintMSWarning("[SaveSynDataOfAllVcInstState] invalid argument BufLen.%d\n", dwBufLen);
			return bRet;
		}
		memcpy((s8*)&tMSVcInstState, pbyBuf, sizeof(tMSVcInstState));

		bRet = g_cMcuVcApp.SetAllVcInstState( tMSVcInstState );
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfAllVcInstState] SetAllVcInstState BufLen.%d failed\n", dwBufLen);
		}
	}
	
	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emAllVcInstState;
		m_tMSCurSynState.m_byInstID  = 0;
	}

    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfOneVcInstConfMtTableData
    功    能： 发送 同步mcu业务One_VcInst_TConfMtTableData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 byInstID
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfOneVcInstConfMtTableData( u8 byInstID )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetOneVcInstConfMtTableData(byInstID, 
		                                                 m_tSndFrmHead.m_pbyFrmBuf+sizeof(TMSVcInstDataHead), 
		                                                 MS_MAX_FRAME_LEN-sizeof(TMSVcInstDataHead), dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfOneVcInstConfMtTableData] GetOneVcInstConfMtTableData failed\n");
		return FALSE;
	}
	TMSVcInstDataHead tMSVcInstHead;
	tMSVcInstHead.m_byInstID  = byInstID;
	m_tSndFrmHead.m_byFrmType = emOneVcInstConfMtTableData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen+sizeof(tMSVcInstHead);
	m_tSndFrmHead.m_wFrmSN   += 1;
	memcpy(m_tSndFrmHead.m_pbyFrmBuf, (u8*)&tMSVcInstHead, sizeof(tMSVcInstHead));
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfOneVcInstConfMtTableData
    功    能： 处理 同步mcu业务One_VcInst_TConfMtTableData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfOneVcInstConfMtTableData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	TMSVcInstDataHead tMSVcInstHead = {0};
	
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfOneVcInstConfMtTableData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
	}
	else
	{
		//check valid
		if (NULL == pbyBuf || dwBufLen <= sizeof(TMSVcInstDataHead))
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstConfMtTableData] invalid argument BufLen.%d\n", dwBufLen);
			return bRet;
		}
		memcpy((s8*)&tMSVcInstHead, pbyBuf, sizeof(tMSVcInstHead));

		bRet = g_cMcuVcApp.SetOneVcInstConfMtTableData(tMSVcInstHead.m_byInstID, 
			                                           pbyBuf+sizeof(tMSVcInstHead), 
													   dwBufLen-sizeof(tMSVcInstHead));
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstConfMtTableData] SetOneVcInstConfMtTableData InstID.%d BufLen.%d failed\n", 
				            tMSVcInstHead.m_byInstID, dwBufLen);
		}
	}

	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emOneVcInstConfMtTableData;
		m_tMSCurSynState.m_byInstID  = tMSVcInstHead.m_byInstID;
	}

    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfOneVcInstConfSwitchTableData
    功    能： 发送 同步mcu业务One_VcInst_TConfSwitchTable内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 byInstID
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfOneVcInstConfSwitchTableData( u8 byInstID )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetOneVcInstConfSwitchTableData(byInstID, 
		                                                     m_tSndFrmHead.m_pbyFrmBuf+sizeof(TMSVcInstDataHead), 
		                                                     MS_MAX_FRAME_LEN-sizeof(TMSVcInstDataHead), dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfOneVcInstConfSwitchTableData] GetOneVcInstConfSwitchTableData failed\n");
		return FALSE;
	}
	TMSVcInstDataHead tMSVcInstHead;
	tMSVcInstHead.m_byInstID  = byInstID;
	m_tSndFrmHead.m_byFrmType = emOneVcInstConfSwitchTableData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen+sizeof(tMSVcInstHead);
	m_tSndFrmHead.m_wFrmSN   += 1;
	memcpy(m_tSndFrmHead.m_pbyFrmBuf, (u8*)&tMSVcInstHead, sizeof(tMSVcInstHead));
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfOneVcInstConfSwitchTableData
    功    能： 处理 同步mcu业务One_VcInst_TConfSwitchTable内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfOneVcInstConfSwitchTableData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	TMSVcInstDataHead tMSVcInstHead;
	
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfOneVcInstConfSwitchTableData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
		return bRet;
	}
	else
	{
		//check valid
		if (NULL == pbyBuf || dwBufLen <= sizeof(TMSVcInstDataHead))
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstConfSwitchTableData] invalid argument BufLen.%d\n", dwBufLen);
			return bRet;
		}
		memcpy((s8*)&tMSVcInstHead, pbyBuf, sizeof(tMSVcInstHead));

		bRet = g_cMcuVcApp.SetOneVcInstConfSwitchTableData(tMSVcInstHead.m_byInstID, 
			                                               pbyBuf+sizeof(tMSVcInstHead), 
														   dwBufLen-sizeof(tMSVcInstHead));
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstConfSwitchTableData] SetOneVcInstConfSwitchTableData InstID.%d BufLen.%d failed\n", 
				            tMSVcInstHead.m_byInstID, dwBufLen);
		}
	}
	
	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emOneVcInstConfSwitchTableData;
		m_tMSCurSynState.m_byInstID  = tMSVcInstHead.m_byInstID;
	}
	
    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfOneVcInstConfOtherMcTableData
    功    能： 发送 同步mcu业务One_VcInst_TConfOtherMcTable内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 byInstID
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfOneVcInstConfOtherMcTableData( u8 byInstID )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetOneVcInstConfOtherMcTableData(byInstID, 
		                                                      m_tSndFrmHead.m_pbyFrmBuf+sizeof(TMSVcInstDataHead), 
		                                                      MS_MAX_FRAME_LEN-sizeof(TMSVcInstDataHead), dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfOneVcInstConfOtherMcTableData] GetOneVcInstConfOtherMcTableData failed\n");
		return FALSE;
	}
	TMSVcInstDataHead tMSVcInstHead;
	tMSVcInstHead.m_byInstID  = byInstID;
	m_tSndFrmHead.m_byFrmType = emOneVcInstConfOtherMcuTableData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen+sizeof(tMSVcInstHead);
	m_tSndFrmHead.m_wFrmSN   += 1;
	memcpy(m_tSndFrmHead.m_pbyFrmBuf, (s8*)&tMSVcInstHead, sizeof(tMSVcInstHead));
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfOneVcInstConfOtherMcTableData
    功    能： 处理 同步mcu业务One_VcInst_TConfOtherMcTable内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfOneVcInstConfOtherMcTableData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	TMSVcInstDataHead tMSVcInstHead;
	
	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfOneVcInstConfOtherMcTableData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
		return bRet;
	}
	else
	{
		//check valid
		if (NULL == pbyBuf || dwBufLen <= sizeof(TMSVcInstDataHead))
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstConfOtherMcTableData] invalid argument BufLen.%d\n", dwBufLen);
			return bRet;
		}
		memcpy((s8*)&tMSVcInstHead, pbyBuf, sizeof(tMSVcInstHead));

		bRet = g_cMcuVcApp.SetOneVcInstConfOtherMcTableData(tMSVcInstHead.m_byInstID, 
			                                                pbyBuf+sizeof(tMSVcInstHead), 
															dwBufLen-sizeof(tMSVcInstHead));
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstConfOtherMcTableData] SetOneVcInstConfOtherMcTableData InstID.%d BufLen.%d failed\n", 
				            tMSVcInstHead.m_byInstID, dwBufLen);
		}
	}
	
	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emOneVcInstConfOtherMcuTableData;
		m_tMSCurSynState.m_byInstID  = tMSVcInstHead.m_byInstID;
	}
	
    return bRet;
}

/*=============================================================================
    函 数 名： SendSynDataOfOneVcInstOtherData
    功    能： 发送 同步mcu业务One_VcInst_OtherData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 byInstID
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SendSynDataOfOneVcInstOtherData( u8 byInstID )
{
	u32 dwOutBufLen = 0;
	if (FALSE == g_cMcuVcApp.GetOneVcInstOtherData(byInstID, 
		                                           m_tSndFrmHead.m_pbyFrmBuf+sizeof(TMSVcInstDataHead), 
		                                           MS_MAX_FRAME_LEN-sizeof(TMSVcInstDataHead), dwOutBufLen))
	{
		PrintMSWarning("[SendSynDataOfOneVcInstOtherData] GetOneVcInstOtherData failed\n");
		return FALSE;
	}
	TMSVcInstDataHead tMSVcInstHead;
	tMSVcInstHead.m_byInstID  = byInstID;
	m_tSndFrmHead.m_byFrmType = emOneVcInstOtherData;
	m_tSndFrmHead.m_dwFrmLen  = dwOutBufLen+sizeof(tMSVcInstHead);
	m_tSndFrmHead.m_wFrmSN   += 1;
	memcpy(m_tSndFrmHead.m_pbyFrmBuf, (u8*)&tMSVcInstHead, sizeof(tMSVcInstHead));
	
	return SendCustomMsgToOtherMcu( &m_tSndFrmHead );
}

/*=============================================================================
    函 数 名： SaveSynDataOfOneVcInstOtherData
    功    能： 处理 同步mcu业务One_VcInst_OtherData内存数据 通知
    算法实现： 
    全局变量： 
    参    数： u8 *pbyBuf, 
	           u32 dwBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerSsnInst::SaveSynDataOfOneVcInstOtherData( u8 *pbyBuf, u32 dwBufLen )
{
	BOOL32 bRet = FALSE;
	TMSVcInstDataHead tMSVcInstHead;
	BOOL32 bResumeTimer = FALSE;

	if (FALSE == g_cMSSsnApp.IsMSConfigInited())
	{
		PrintMSInfo("[SaveSynDataOfOneVcInstOtherData] McuVcApp Not Inited, return\n");
		bRet = TRUE;
		return bRet;
	}
	else
	{
		//check valid
		if (NULL == pbyBuf || dwBufLen <= sizeof(TMSVcInstDataHead))
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstOtherData] invalid argument BufLen.%d\n", dwBufLen);
			return bRet;
		}
		
		memcpy((s8*)&tMSVcInstHead, pbyBuf, sizeof(tMSVcInstHead));
		if (FALSE == g_cMSSsnApp.IsMSSynOK())
		{
			bResumeTimer = TRUE;
		}

		bRet = g_cMcuVcApp.SetOneVcInstOtherData(tMSVcInstHead.m_byInstID, 
			                                     pbyBuf+sizeof(tMSVcInstHead), 
												 dwBufLen-sizeof(tMSVcInstHead), bResumeTimer);
		if (FALSE == bRet)
		{
			PrintMSWarning("[SaveSynDataOfOneVcInstOtherData] SetOneVcInstOtherData InstID.%d BufLen.%d ResumeTimer.%d failed\n", 
				            tMSVcInstHead.m_byInstID, dwBufLen, bResumeTimer);
		}
	}

	if (bRet)
	{
		m_tMSCurSynState.m_emSynType = emOneVcInstOtherData;
		m_tMSCurSynState.m_byInstID  = tMSVcInstHead.m_byInstID;
	}

    return bRet;
}

/*=============================================================================
    函 数 名： PrintMSVerbose
    功    能： 主备交换打印调试详细信息
    算法实现： 
    全局变量： 
    参    数： s8 * pszFmt
               ...
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
/*lint -save -e628*/
void CMSManagerSsnInst::PrintMSVerbose(s8*  pszFmt, ...)
{
	s8  achPrintBuf[1024];
	s32 nBufLen = sprintf( achPrintBuf, "[MS_VERBOSE]: " );

	va_list tArgptr;
	va_start(tArgptr, pszFmt);    
	vsnprintf(achPrintBuf + nBufLen, 1024 - nBufLen - 1, pszFmt, tArgptr );
	//vsprintf( achPrintBuf+nBufLen, pszFmt, tArgptr );   
	va_end(tArgptr);

	MSManagerPrint(MS_DEBUG_VERBOSE, achPrintBuf);
	return;
}
/*lint -restore*/

/*=============================================================================
    函 数 名： PrintMSInfo
    功    能： 主备交换打印调试普通信息
    算法实现： 
    全局变量： 
    参    数： s8 * pszFmt
               ...
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::PrintMSInfo(s8*  pszFmt, ...)
{
	s8  achPrintBuf[1024];
	s32 nBufLen = sprintf( achPrintBuf, "[MS_INFO]: " );
	
	va_list tArgptr;
	va_start(tArgptr, pszFmt);    
	vsnprintf(achPrintBuf + nBufLen, 1024 - nBufLen - 1, pszFmt, tArgptr );
	//vsprintf( achPrintBuf+nBufLen, pszFmt, tArgptr );   
	va_end(tArgptr);
	
	MSManagerPrint(MS_DEBUG_INFO, achPrintBuf);
	
	return;
}

/*=============================================================================
    函 数 名： PrintMSWarning
    功    能： 主备交换打印告警信息
    算法实现： 
    全局变量： 
    参    数： s8 * pszFmt
               ...
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::PrintMSWarning(s8*  pszFmt, ...)
{
	s8  achPrintBuf[1024];
	s32 nBufLen = sprintf( achPrintBuf, "[MS_WARNING]: " );
	
	va_list tArgptr;
	va_start(tArgptr, pszFmt);    
	vsnprintf(achPrintBuf + nBufLen, 1024 - nBufLen - 1, pszFmt, tArgptr );
	//vsprintf( achPrintBuf+nBufLen, pszFmt, tArgptr );   
	va_end(tArgptr);
		
	MSManagerPrint(MS_DEBUG_WARNING, achPrintBuf);
	
	return;
}

/*=============================================================================
    函 数 名： PrintMSException
    功    能： 主备交换打印错误信息
    算法实现： 
    全局变量： 
    参    数： s8 * pszFmt
               ...
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::PrintMSException( s8*  pszFmt, ... )
{
	s8  achPrintBuf[1024];
	s32 nBufLen = sprintf( achPrintBuf, "[MS_ERROR]: " );
	
    va_list tArgptr;
    va_start(tArgptr, pszFmt);    
	vsnprintf(achPrintBuf + nBufLen, 1024 - nBufLen - 1, pszFmt, tArgptr );
    //vsprintf( achPrintBuf+nBufLen, pszFmt, tArgptr );   
    va_end(tArgptr);
	
	MSManagerPrint(MS_DEBUG_ERROR, achPrintBuf);
	
	return;
}

/*=============================================================================
    函 数 名： PrintMSCritical
    功    能： 主备交换打印异常信息
    算法实现： 
    全局变量： 
    参    数： s8 * pszFmt
               ...
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::PrintMSCritical( s8*  pszFmt, ... )
{
	s8  achPrintBuf[1024];
	s32 nBufLen = sprintf( achPrintBuf, "[MS_CRITICAL]: " );

	va_list tArgptr;
	va_start(tArgptr, pszFmt);    
	vsnprintf(achPrintBuf + nBufLen, 1024 - nBufLen - 1, pszFmt, tArgptr );
	//vsprintf( achPrintBuf+nBufLen, pszFmt, tArgptr );   
	va_end(tArgptr);

	MSManagerPrint(MS_DEBUG_ERROR, achPrintBuf);

	return;
}

/*=============================================================================
    函 数 名： MSManagerPrint
    功    能： 主备交换打印
    算法实现： 
    全局变量： 
    参    数： u8 byLevel
               s8 * pszPrintStr
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerSsnInst::MSManagerPrint( u8 byLevel, s8 *pszPrintStr )
{
	s8  achPrintBuf[256];
	s32 nLen = 0;
		
	if (CInstance::DAEMON != GetInsID())
	{
		nLen = sprintf( achPrintBuf, "[MS]" );
	}
	else
	{
		nLen = sprintf( achPrintBuf, "[MSDaemon]" );
	}
	strncat( achPrintBuf+nLen, pszPrintStr, sizeof(achPrintBuf)-(u32)nLen );
	
	LogPrint( byLevel, MID_MCU_MSMGR, achPrintBuf);
	
	return;
}


//构造
CMSManagerConfig::CMSManagerConfig()
{
	m_dwLocalMcuIP  = 0;
	m_dwRemoteMcuIP = 0;
	m_wMcuListenPort= 0;
	m_byLocalMcuType= 0;
	m_bDoubleLink   = FALSE;
	m_bInited       = FALSE;
    m_bRemoteConnected = FALSE;
	m_emLocalMSType = MCU_MSTYPE_INVALID;
	m_emCurMSState  = MCU_MSSTATE_OFFLINE;
	m_bSynOK        = FALSE;
    m_bMsSwitchOK   = FALSE;
	m_hSemInitLock  = NULL;
	m_hSemSynLock   = NULL;
	m_wLockedAppId  = 0;
    
	//vxworks下默认采用定时器获取硬件主备用状态，
	//vxworks下不支持由硬件平台直接获取主备用结果，则按照WIN32方式操作
	//WIN32下固定采用Osp消息协商主备用状态，若断链，则备用升为主用
#ifdef WIN32
	m_bMSDetermineType = FALSE;
	m_byDebugLevel  = MS_DEBUG_INFO;
#else
	m_bMSDetermineType = TRUE;
	m_byDebugLevel  = MS_DEBUG_CRITICAL;
#endif

	m_dwDitheringTimes = 0;

	// guzh [6/12/2007] 初始化系统会话值
    srand(time(NULL));
    SetMSSsrc(rand()+1);
}

//析构
CMSManagerConfig::~CMSManagerConfig()
{
	FreeMSConfig();
}

/*=============================================================================
    函 数 名： InitMSConfig
    功    能： 获取主备板倒换业务设置，并根据需要创建MSManager应用，主备协商后返回
    算法实现： 
    全局变量： 
    参    数： TMSStartParam tMSStartParam
    返 回 值： BOOL32 返回当前MCU主备用状态 TRUE-主用，FALSE-备用
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::InitMSConfig( TMSStartParam tMSStartParam )
{
	BOOL32 bMpcEnable = TRUE;
	
	FreeMSConfig();

#if defined(_MINIMCU_) || defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	SetCurMSState(MCU_MSSTATE_ACTIVE);
	SetMSSynOKFlag(TRUE);
	bMpcEnable = TRUE;
	m_bDoubleLink = FALSE;
	return bMpcEnable;
#endif

	if (FALSE == InitLocalCfgInfo(tMSStartParam.m_byMcuType))
	{		
        //printf("[InitMSConfig] InitLocalCfgInfo Failed!\n");
		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] InitLocalCfgInfo Failed!\n");
		FreeMSConfig();
		//RebootMCU();
		return bMpcEnable;
	}
	
	m_wMcuListenPort = tMSStartParam.m_wMcuListenPort;
	m_bMSDetermineType = tMSStartParam.m_bMSDetermineType;

	//主备业务 - 
	if (m_bDoubleLink)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] m_bDoubleLink is true!\n");
		//双层建链支持，初始化MSManager应用，等待并返回主备协商结果

		//分配资源失败做重启
		if (FALSE == OspSemBCreate(&m_hSemInitLock))
		{
			//printf("[InitMSConfig] OspSemBCreate m_hSemInitLock Failed, DoubleLink->SingleLink!\n");
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] OspSemBCreate m_hSemInitLock Failed, DoubleLink->SingleLink!\n");
			FreeMSConfig();
			RebootMCU();
			return bMpcEnable;
		}
		if (FALSE == OspSemBCreate(&m_hSemSynLock))
		{
			//printf("[InitMSConfig] OspSemBCreate m_hSemSynLock Failed, DoubleLink->SingleLink!\n");
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] OspSemBCreate m_hSemSynLock Failed, DoubleLink->SingleLink!\n");
			FreeMSConfig();
			RebootMCU();
			return bMpcEnable;
		}

		//开始有信号，释放
		OspSemTake(m_hSemInitLock);

		//任务等级与 McuVc－Task 保持一致，
		s32 nRet = g_cMSSsnApp.CreateApp( "McuMSMag", AID_MCU_MSMANAGERSSN, APPPRI_MCUMSMGR, 1000, 4<<20 );
		if ( OSP_OK != nRet )
		{
			StaticLog("[InitMSConfig]g_cMSSsnApp.CreateApp McuMSMag failed!\n ");
		}
		OspPost(MAKEIID(AID_MCU_MSMANAGERSSN, 1), MCU_MSEXCHANGER_POWERON_CMD);

		//若为备板，同步等待主备协商并按照主备协商结果进行下一步处理
        if(!g_cMSSsnApp.IsActiveBoard())
        {
            if (!EnterMSInitLock())
            {
                //等待超时，
                //若为WIN32方式，则自行升为主用板，否则VxWorks沿用硬件检测的主备用状态
                if (FALSE == GetMSDetermineType())
                {
                    SetCurMSState(MCU_MSSTATE_ACTIVE);
                    SetMSSynOKFlag(TRUE);
                    //printf("[InitMSConfig] EnterMSInitLock TimeOut CurMSState.%d Unlock\n", m_emCurMSState);
					LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] EnterMSInitLock TimeOut CurMSState.%d Unlock\n", m_emCurMSState);
                }
                else
                {
                    if (MCU_MSSTATE_ACTIVE == m_emCurMSState)
                    {
                        //printf("[InitMSConfig] EnterMSInitLock TimeOut CurMSState.%d Unlock\n", m_emCurMSState);
						LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] EnterMSInitLock TimeOut CurMSState.%d Unlock\n", m_emCurMSState);
                    }
                    else
                    {
                        //printf("[InitMSConfig] EnterMSInitLock TimeOut CurMSState.%d Reboot MCU\n", m_emCurMSState);
						LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] EnterMSInitLock TimeOut CurMSState.%d Reboot MCU\n", m_emCurMSState);
						RebootMCU();
                        return bMpcEnable;
                    }
                }
            }
        }		
		else
		{
			SetCurMSState(MCU_MSSTATE_ACTIVE);
		}
		bMpcEnable = (MCU_MSSTATE_ACTIVE == m_emCurMSState) ? TRUE : FALSE;
	}
	else
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitMSConfig] m_bDoubleLink is false!\n");
		//单层建链，直接做为主用板
		SetCurMSState(MCU_MSSTATE_ACTIVE);
		SetMSSynOKFlag(TRUE);
		bMpcEnable = TRUE;
	}

	//printf("[Mcu] MS Init OK. MpcEnable.%d DoubleLink.%d CurMSState.%d MSDetermineType.%d\n", 
	//	    bMpcEnable, m_bDoubleLink, m_emCurMSState, m_bMSDetermineType);

	LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[Mcu] MS Init OK. MpcEnable.%d DoubleLink.%d CurMSState.%d MSDetermineType.%d\n", 
		    bMpcEnable, m_bDoubleLink, m_emCurMSState, m_bMSDetermineType);

    //若为主板则将ous灯点灭
    if (GetCurMSState() == MCU_MSSTATE_ACTIVE)
    {
        SetMpcOusLed(FALSE);
    }
    
	m_bInited = TRUE;
	return bMpcEnable;
}

/*=============================================================================
    函 数 名： FreeMSConfig
    功    能： 释放主从通讯节点 及 相关数据区
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
void CMSManagerConfig::FreeMSConfig( void )
{
	m_dwLocalMcuIP  = 0;
	m_dwRemoteMcuIP = 0;
	m_wMcuListenPort= 0;
	m_byLocalMcuType= 0;
	m_bDoubleLink   = FALSE;
	m_bInited       = FALSE;
	m_emLocalMSType = MCU_MSTYPE_INVALID;
	m_emCurMSState  = MCU_MSSTATE_OFFLINE;
	m_bSynOK        = FALSE;
	if (NULL != m_hSemInitLock)
	{
		OspSemDelete(m_hSemInitLock);
		m_hSemInitLock = NULL;
	}
	if (NULL != m_hSemSynLock)
	{
		OspSemDelete(m_hSemSynLock);
		m_hSemSynLock = NULL;
	}
	m_wLockedAppId = 0;

	return;
}

/*=============================================================================
    函 数 名： InitLocalCfgInfo
    功    能： 初始化 本地配置信息
    算法实现： 
    全局变量： 
    参    数： u8 byLocalMcuType
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/18  4.0		    万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::InitLocalCfgInfo( u8 byLocalMcuType )
{
	/*lint -save -esym(550, byInterface, byEntLayer)*/
	/*lint -save -esym(429, ppszTable)*/
	
	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUCFGFILENAME);
	
	s8* *ppszTable = NULL;
	s32  nEntryNum = 0;
	s32  nLoop = 0;
	
	// 先判断配置文件是否存在, 如不存在则直接按单链启动, zgc, 2007-05-23
	FILE* hOpen = NULL;
    if(NULL != (hOpen = fopen(achFullName, "r"))) // exist
    {
        fclose(hOpen); 
    }
	else
	{
		m_bDoubleLink = FALSE;
		m_dwRemoteMcuIP = 0;
		return TRUE;
	}
	
	//get the number of entry 
	if (FALSE == GetRegKeyInt(achFullName, SECTION_mcueqpBoardConfig, STR_ENTRY_NUM, 0, &nEntryNum)) 
	{
		//printf("[InitLocalCfgInfo] Wrong profile while reading %s %s, SingleLink Start!\n", 
		//	SECTION_mcueqpBoardConfig, STR_ENTRY_NUM );  

		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while reading %s %s, SingleLink Start!\n", 
			SECTION_mcueqpBoardConfig, STR_ENTRY_NUM );  

		//读取单板配置字段失败，则按照默认的单链启动，由代理自动生成缺省的配置文件
		m_bDoubleLink   = FALSE;
		m_dwRemoteMcuIP = 0;
		return TRUE;
	}
	if (0 == nEntryNum)
	{
		//printf("[InitLocalCfgInfo] The Board Num is 0, SingleLink Start\n");

		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] The Board Num is 0, SingleLink Start\n");
		//读取单板配置字段失败，则按照默认的单链启动，由代理自动生成缺省的配置文件
		m_bDoubleLink   = FALSE;
		m_dwRemoteMcuIP = 0;
		return TRUE;
	}
	
	//alloc memory
	ppszTable = new s8*[(u32)nEntryNum];
	if (NULL == ppszTable)
	{
		//printf("[InitLocalCfgInfo] Fail to malloc memory for board table\n");

		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Fail to malloc memory for board table\n");
		return FALSE;
	}
	memset(ppszTable, 0, (u32)nEntryNum*(u32)sizeof(s8*));
	for (nLoop = 0; nLoop < nEntryNum; nLoop++)
	{
		ppszTable[nLoop] = new s8[MS_MAX_STRING_LEN+1];
		if (NULL == ppszTable[nLoop])
		{
			
			TableMemoryFree(ppszTable, nEntryNum);
			//printf("[InitLocalCfgInfo] Fail to malloc memory for board table\n");
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Fail to malloc memory for board table\n");
			return FALSE;
		}
	}
	
	//get the board config table
	u32 dwEntryNum = (u32)nEntryNum;
	// 因为GetRegKeyStringTable只有在读错STR_ENTRY_NUM时才会返回FALSE，而其他情况下
	// 都返回TRUE，因此这里修改判断标准, zgc, 2007-03-21
	BOOL32 bRet = GetRegKeyStringTable(achFullName, SECTION_mcueqpBoardConfig, "fail", 
		ppszTable, &dwEntryNum, (MS_MAX_STRING_LEN+1));
	bRet &= ( (u32)nEntryNum == dwEntryNum ) ? TRUE : FALSE;
	if (FALSE == bRet )
	{
		TableMemoryFree(ppszTable, nEntryNum);

		//printf("[InitLocalCfgInfo] Wrong profile while reading %s table!\n", 
		//	SECTION_mcueqpBoardConfig);

		LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while reading %s table!\n", 
			SECTION_mcueqpBoardConfig);

		return FALSE;
	}
	
	s8  achSeps[] = " \t";  //分隔符
	s8 *pchToken  = NULL;
	u8  byEntLayer = 0;
	u8  byEntSlot  = 0;
	u8  byEntType  = 0;
	u32 dwEntIpAddr = 0;    //net order
	u8  byInterface = 1;    // 前后网口选择
	
#ifndef WIN32
	TBrdPosition tCurrentPos;
	BrdQueryPosition( &tCurrentPos ); // 只关心槽号/ *Mpc为备板时,取层号失败，忽略 */
	//printf( "[InitLocalCfgInfo] BrdQueryPosition[BrdId.%d, Layer.%d, Slot.%d]!\n", 
	//											tCurrentPos.byBrdID,
	//											tCurrentPos.byBrdLayer,
	//											tCurrentPos.byBrdSlot);

	LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] BrdQueryPosition[BrdId.%d, Layer.%d, Slot.%d]!\n", 
												tCurrentPos.byBrdID,
												tCurrentPos.byBrdLayer,
												tCurrentPos.byBrdSlot);
#endif	
	
	// analyze entry strings 
	for (nLoop = 0; nLoop < nEntryNum; nLoop++ )
	{
		// BoardId 
		pchToken = strtok( ppszTable[nLoop], achSeps );
		if (NULL == pchToken)
		{
			//printf("[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardId );
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardId );
			continue;
		}
		
		//Layer
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
		{
			//printf("[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardLayer );

			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardLayer );
			continue;
		}
		else
		{
			byEntLayer = atoi(pchToken);
		}
		
		//Slot 
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
		{
			//printf("[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardSlot );
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardSlot );
			continue;
		}
		else
		{
			StrUpper( pchToken );
			if (FALSE == GetSlotThroughName(pchToken, &byEntSlot))
			{
				//printf("[InitLocalCfgInfo] Wrong Value %s = %s !\n", FIELD_mcueqpBoardSlot, pchToken );
				LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong Value %s = %s !\n", FIELD_mcueqpBoardSlot, pchToken );
				continue;
			}
		}
		
		//Type 
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
		{
			//printf("[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardType );
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardType );
			continue;
		}
		else
		{
			StrUpper( pchToken );
			if (FALSE == GetTypeThroughName(pchToken, &byEntType))
			{
				//printf("[InitLocalCfgInfo] Wrong Value %s = %s !\n", FIELD_mcueqpBoardType, pchToken );
				LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong Value %s = %s !\n", FIELD_mcueqpBoardType, pchToken );
				continue;
			}
		}
		
		//IpAddr
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
		{
			//printf("[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardIpAddr );
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while reading %s!\n", FIELD_mcueqpBoardIpAddr );
			continue;
		}
		else
		{
			dwEntIpAddr = INET_ADDR(pchToken);
		}
		
		pchToken = strtok(NULL, achSeps);
		if( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == byEntType || BRD_TYPE_MPC2 == byEntType )
		{
#ifdef _MINIMCU_               
			byInterface = 0;
#else
#ifdef _LINUX_
			// 先查eth1
			STATUS nRet = 0;
			TBrdEthParamAll tBrdEthParamAll;
			u8 byLop = 0;
			nRet = BrdGetEthParamAll( 1, &tBrdEthParamAll ); 
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Get eth1 params -> IpNum.%d!\n", tBrdEthParamAll.dwIpNum);
			if (tBrdEthParamAll.dwIpNum > 0)
			{
				for (u8 byIdx = 0; byIdx < tBrdEthParamAll.dwIpNum; byIdx++)
				{
					LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] eth1 Ip.%d -> 0x%x!\n", 
									byIdx, tBrdEthParamAll.atBrdEthParam[byIdx].dwIpAdrs);
				}
			}

			if ( nRet == ERROR )
			{
				//printf( "[InitLocalCfgInfo] Get eth1 params failed!\n" );
				LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Get eth1 params failed!\n" );
			}
			else if( tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs != 0 )
			{
				for ( byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
				{
					if ( dwEntIpAddr == tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs )
					{
						LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Get eth1 params success!\n" );
						byInterface = 1;
						break;
					}
				}
			}
			
			//再查eth0
			if ( nRet == ERROR 
				|| tBrdEthParamAll.dwIpNum == 0 
				|| (tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs == 0)
				|| byLop == tBrdEthParamAll.dwIpNum )
			{
				memset( &tBrdEthParamAll, 0, sizeof(tBrdEthParamAll) );
				nRet = BrdGetEthParamAll( 0, &tBrdEthParamAll ); 

				LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Get eth0 params -> IpNum.%d!\n", tBrdEthParamAll.dwIpNum);
				
				if (tBrdEthParamAll.dwIpNum > 0)
				{
					for (u8 byIdx = 0; byIdx < tBrdEthParamAll.dwIpNum; byIdx++)
					{
						LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] eth0 Ip.%d -> 0x%x!\n", 
													byIdx, tBrdEthParamAll.atBrdEthParam[byIdx].dwIpAdrs);
					}
				}

				if ( nRet == ERROR )
				{
					//printf( "[InitLocalCfgInfo] Get eth0 params failed!\n" );
					LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Get eth0 params failed!\n" );
				}
				else if ( tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs != 0 )
				{
					for ( byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
					{
						if ( dwEntIpAddr == tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs )
						{
							byInterface = 0;
							break;
						}
					}
				}
				if ( nRet == ERROR 
					|| tBrdEthParamAll.dwIpNum == 0 
					|| (tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs == 0)
					|| byLop == tBrdEthParamAll.dwIpNum )
				{
					//printf( "[InitLocalCfgInfo] MPC ip is not correct!\n" );
					LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] MPC ip is not correct!\n" );
					byInterface = 1;
				}
			}
			
#endif
#ifdef _VXWORKS_
			// 先查eth1
			s32 nRet;
			TBrdEthParam tOneParam;
			memset(&tOneParam, 0, sizeof(tOneParam));
			nRet = BrdGetEthParam( 1, &tOneParam );
			if ( nRet == ERROR )
			{
				printf( "[InitLocalCfgInfo] Get eth1 param failed!\n" );
			}
			else if ( dwEntIpAddr == tOneParam.dwIpAdrs )
			{
				byInterface = 1;
			}
			
			if ( nRet == ERROR || dwEntIpAddr != tOneParam.dwIpAdrs ) 
			{
				//再查eth0
				memset(&tOneParam, 0, sizeof(tOneParam));
				nRet = BrdGetEthParam( 0, &tOneParam );
				if ( nRet == ERROR )
				{
					printf( "[InitLocalCfgInfo] Get eth0 param failed!\n" );
				}
				else if ( dwEntIpAddr == tOneParam.dwIpAdrs )
				{
					byInterface = 0;
				}
				
				if ( nRet == ERROR || dwEntIpAddr != tOneParam.dwIpAdrs ) 
				{
					printf( "[InitLocalCfgInfo] MPC ip is not correct!\n" );
					byInterface = 1;
				}
			}
#endif
#ifdef WIN32
			byInterface = 1;
#endif
#endif		
		}
		else
		{
			if (NULL == pchToken)
			{
				//            if( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == byEntType )
				//            {
				// #ifdef _MINIMCU_               
				// 				byInterface = 0;
				// #else
				// 			#ifdef _LINUX_
				// 				// 先查eth1
				// 				STATUS nRet = 0;
				// 				TBrdEthParamAll tBrdEthParamAll;
				// 				u8 byLop = 0;
				// 				nRet = BrdGetEthParamAll( 1, &tBrdEthParamAll ); 
				// 				if ( nRet == ERROR )
				// 				{
				// 					printf( "[InitLocalCfgInfo] Get eth1 params failed!\n" );
				// 				}
				// 				else if( tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs != 0 )
				// 				{
				// 					for ( byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
				// 					{
				// 						if ( dwEntIpAddr == tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs )
				// 						{
				// 							byInterface = 1;
				// 							break;
				// 						}
				// 					}
				// 				}
				// 				
				// 				//再查eth0
				// 				if ( nRet == ERROR 
				// 					|| tBrdEthParamAll.dwIpNum <= 0 
				// 					|| (tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs == 0)
				// 					|| byLop == tBrdEthParamAll.dwIpNum )
				// 				{
				// 					memset( &tBrdEthParamAll, 0, sizeof(tBrdEthParamAll) );
				// 					nRet = BrdGetEthParamAll( 0, &tBrdEthParamAll ); 
				// 					if ( nRet == ERROR )
				// 					{
				// 						printf( "[InitLocalCfgInfo] Get eth0 params failed!\n" );
				// 					}
				// 					else if ( tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs != 0 )
				// 					{
				// 						for ( byLop = 0; byLop < tBrdEthParamAll.dwIpNum; byLop++ )
				// 						{
				// 							if ( dwEntIpAddr == tBrdEthParamAll.atBrdEthParam[byLop].dwIpAdrs )
				// 							{
				// 								byInterface = 0;
				// 								break;
				// 							}
				// 						}
				// 					}
				// 					if ( nRet == ERROR 
				// 					|| tBrdEthParamAll.dwIpNum <= 0 
				// 					|| (tBrdEthParamAll.dwIpNum > 0 && tBrdEthParamAll.atBrdEthParam[0].dwIpAdrs == 0)
				// 					|| byLop == tBrdEthParamAll.dwIpNum )
				// 					{
				// 						printf( "[InitLocalCfgInfo] MPC ip is not correct!\n" );
				// 						byInterface = 1;
				// 					}
				// 				}
				// 				
				// 			#endif
				// 			#ifdef _VXWORKS_
				// 				// 先查eth1
				// 				s32 nRet;
				// 				TBrdEthParam tOneParam;
				// 				memset(&tOneParam, 0, sizeof(tOneParam));
				// 				nRet = BrdGetEthParam( 1, &tOneParam );
				// 				if ( nRet == ERROR )
				// 				{
				// 					printf( "[InitLocalCfgInfo] Get eth1 param failed!\n" );
				// 				}
				// 				else if ( dwEntIpAddr == tOneParam.dwIpAdrs )
				// 				{
				// 					byInterface = 1;
				// 				}
				// 				
				// 				if ( nRet == ERROR || dwEntIpAddr != tOneParam.dwIpAdrs ) 
				// 				{
				// 					//再查eth0
				// 					memset(&tOneParam, 0, sizeof(tOneParam));
				// 					nRet = BrdGetEthParam( 0, &tOneParam );
				// 					if ( nRet == ERROR )
				// 					{
				// 						printf( "[InitLocalCfgInfo] Get eth0 param failed!\n" );
				// 					}
				// 					else if ( dwEntIpAddr == tOneParam.dwIpAdrs )
				// 					{
				// 						byInterface = 0;
				// 					}
				// 
				// 					if ( nRet == ERROR || dwEntIpAddr != tOneParam.dwIpAdrs ) 
				// 					{
				// 						printf( "[InitLocalCfgInfo] MPC ip is not correct!\n" );
				// 						byInterface = 1;
				// 					}
				// 				}
				// 			#endif
				// 			#ifdef WIN32
				// 				byInterface = 1;
				// 			#endif
// #endif		
//            }
// 		   else
			   {
				   byInterface = 1;
			   }
			}
			else
			{
				byInterface = atoi( pchToken );
			}
		}
		
		//判断配置和实际是否一致, 现在只能有一块MPC板
		//  [1/21/2011 chendaiwei]支持MPC2
		if (BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == byEntType || BRD_TYPE_MPC2 == byEntType)
		{
#ifndef WIN32			
			TBrdEthParam tMpcEthParam;
			BrdGetEthParam( byInterface, &tMpcEthParam);
			LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[InitLocalCfgInfo] BrdGetEthParam <Interface.%d, Ip.0x%x)\n", 
										byInterface, tMpcEthParam.dwIpAdrs);

			if ( ntohl(dwEntIpAddr) == tMpcEthParam.dwIpAdrs )
#else
				if( OspIsLocalHost( dwEntIpAddr) )
#endif
				{
					m_dwLocalMcuIP = dwEntIpAddr;
					if (1 == byEntSlot) //mc1
					{
						m_emLocalMSType = MCU_MSTYPE_MASTER;
					}
					else                //mc0
					{
						m_emLocalMSType = MCU_MSTYPE_SLAVE;
					}
					LogPrint(LOG_LVL_KEYSTATUS,  MID_PUB_ALWAYS, "[InitLocalCfgInfo] m_emLocalMSType.%d!\n", m_emLocalMSType);
				}
#ifndef WIN32
				else if( tCurrentPos.byBrdSlot == byEntSlot ) // 配置文件中MpcIp有误
				{
					m_dwLocalMcuIP = htonl( tMpcEthParam.dwIpAdrs );
					LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[!!Debug!!] the mpc's ip %0x.\n", m_dwLocalMcuIP);
				}
#endif
				else
				{
					m_dwRemoteMcuIP = dwEntIpAddr;
				}
		}
	}
	
	//drawback memory
	TableMemoryFree(ppszTable, nEntryNum);
    
	m_byLocalMcuType = byLocalMcuType;
	
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
	if (0 == m_dwLocalMcuIP)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[InitLocalCfgInfo] Wrong profile while LocalMcuIP!\n");
		m_bDoubleLink   = FALSE;
		m_dwRemoteMcuIP = 0;
		return FALSE;
	}
	
	if (MCU_TYPE_KDV8000B != m_byLocalMcuType && MCU_TYPE_KDV8000C != m_byLocalMcuType && 
		0 != m_dwRemoteMcuIP && 0xFFFFFFFF != m_dwRemoteMcuIP && m_dwLocalMcuIP != m_dwRemoteMcuIP)
	{
		m_bDoubleLink = TRUE;
	}
	else
	{
		m_bDoubleLink   = FALSE;
		m_dwRemoteMcuIP = 0;
	}
#endif	
	return TRUE;
	
	/*lint -restore*/
}

/*=============================================================================
    函 数 名： GetLocalMcuIP
    功    能： 获取主备倒换时本地mcu的IP
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： u32 主机序
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
u32 CMSManagerConfig::GetLocalMcuIP( void )
{
	return ntohl(m_dwLocalMcuIP);
}

/*=============================================================================
    函 数 名： SetLocalMcuIP
    功    能： 设置主备倒换时本地mcu的IP
    算法实现： 
    全局变量： 
    参    数： dwLocalMcuIP 主机序 
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerConfig::SetLocalMcuIP( u32 dwLocalMcuIP )
{
	m_dwLocalMcuIP = htonl(dwLocalMcuIP);

	return;
}

/*=============================================================================
    函 数 名： GetAnotherMcuIP
    功    能： 获取主备倒换时另一块mcu的IP
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： u32 主机序
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
u32 CMSManagerConfig::GetAnotherMcuIP( void )
{
	return ntohl(m_dwRemoteMcuIP);
}

/*=============================================================================
    函 数 名： SetAnotherMcuIP
    功    能： 设置主备倒换时另一块mcu的IP
    算法实现： 
    全局变量： 
    参    数： u32 dwAnotherMcuIP 主机序
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerConfig::SetAnotherMcuIP( u32 dwAnotherMcuIP )
{
	m_dwRemoteMcuIP = htonl(dwAnotherMcuIP);
	
	return;
}

/*=============================================================================
    函 数 名： GetLocalMcuType
    功    能： 获取MCU连接端口
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： u16 主机序
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
u16 CMSManagerConfig::GetMcuConnectPort( void )
{
	return m_wMcuListenPort;
}

/*=============================================================================
    函 数 名： SetMcuConnectPort
    功    能： 设置MCU连接端口
    算法实现： 
    全局变量： 
    参    数： u16 wConnectPort 主机序
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerConfig::SetMcuConnectPort( u16 wConnectPort )
{
	m_wMcuListenPort = wConnectPort;
	
	return;
}

/*=============================================================================
    函 数 名： GetLocalMcuType
    功    能： 获取本地MCU类型 -- 8000A、8000B、WIN32
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： u8
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
u8 CMSManagerConfig::GetLocalMcuType( void )
{
	return m_byLocalMcuType;
}

/*=============================================================================
    函 数 名： SetLocalMcuType
    功    能： 设置本地MCU类型 -- 8000A、8000B、WIN32
    算法实现： 
    全局变量： 
    参    数： u8 byLocalMcuType
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerConfig::SetLocalMcuType( u8 byLocalMcuType )
{
	m_byLocalMcuType = byLocalMcuType;

	return;
}

/*=============================================================================
    函 数 名： IsDoubleLink
    功    能： 获取是否进行双层的主备倒换
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::IsDoubleLink( void )
{
	return m_bDoubleLink;
}

/*=============================================================================
    函 数 名： SetDoubleLink
    功    能： 设置是否进行双层的主备倒换
    算法实现： 
    全局变量： 
    参    数： BOOL32 bDoubleLink
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerConfig::SetDoubleLink( BOOL32 bDoubleLink )
{
	m_bDoubleLink = bDoubleLink;

	return;
}

/*=============================================================================
    函 数 名： GetLocalMSType
    功    能： 获取主备倒换时本地MCU的物理主备类型
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： emMCUMSType 本地MCU的物理主备类型
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
emMCUMSType CMSManagerConfig::GetLocalMSType( void )
{
	return m_emLocalMSType;
}

/*=============================================================================
    函 数 名： SetLocalMSType
    功    能： 设置主备倒换时本地MCU的物理主备类型
    算法实现： 
    全局变量： 
    参    数： emMCUMSType emLocalMSType
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerConfig::SetLocalMSType( emMCUMSType emLocalMSType )
{
	m_emLocalMSType = emLocalMSType;

	return;
}

/*=============================================================================
    函 数 名： SetRemoteMpcConnected
    功    能： 设置对端是否已经连接
    算法实现： 
    全局变量： 
    参    数： BOOL32
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/09/25  4.0			顾振华                  创建
=============================================================================*/
void CMSManagerConfig::SetRemoteMpcConnected( BOOL32 bConnect )
{
	m_bRemoteConnected = bConnect;
}

/*=============================================================================
    函 数 名： IsRemoteMpcConnected
    功    能： 返回对端是否已经连接
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/09/25  4.0			顾振华                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::IsRemoteMpcConnected() const
{
	return m_bRemoteConnected;
}

/*=============================================================================
    函 数 名： GetCurMSState
    功    能： 获取主备倒换时本地MCU当前的主备用状态
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： emMCUMSState 本地MCU当前的主备用状态
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
emMCUMSState CMSManagerConfig::GetCurMSState( void )
{
	return m_emCurMSState;
}

/*=============================================================================
    函 数 名： SetCurMSState
    功    能： 设置主备倒换时本地MCU当前的主备用状态，并同步代理相应状态
    算法实现： 
    全局变量： 
    参    数： emMCUMSState emCurMSState MCU_MSSTATE_ACTIVE/MCU_MSSTATE_STANDBY
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerConfig::SetCurMSState(emMCUMSState emCurMSState)
{
	if (emCurMSState <= MCU_MSSTATE_OFFLINE || emCurMSState > MCU_MSSTATE_ACTIVE)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MSMGR, "[SetCurMSState] invalid argument emCurMSState(%d), Failed!\n", emCurMSState);
		return;
	}
	if (emCurMSState != m_emCurMSState)
	{
		//vxworks下不支持由硬件平台直接获取主备用结果时需os接口同步硬件主备用状态
#ifndef WIN32
		if (FALSE == m_bMSDetermineType && TRUE == m_bDoubleLink)
		{
			u8 byLocalMSState = BRD_MPC_RUN_SLAVE;
			if (MCU_MSSTATE_ACTIVE == emCurMSState)
			{
				byLocalMSState = BRD_MPC_RUN_MASTER;
			}
			if (OK != BrdMPCSetLocalMSState(byLocalMSState))			
			{
				LogPrint( LOG_LVL_ERROR, MID_MCU_MSMGR, "[SetCurMSState] BrdMPCSetLocalMSState Failed Old_CurMSState.%d New_CurMSState.%d!\n", 
						  m_emCurMSState, emCurMSState);
			}
		}
#endif
		m_emCurMSState = emCurMSState;
	}

	//同步 代理模块 主备用状态
	if (m_bInited)
	{
		BOOL32 bMpcEnabled = FALSE;

		if( MCU_MSSTATE_ACTIVE == m_emCurMSState )
		{
			bMpcEnabled = TRUE;
		}

		g_cMcuAgent.SetMpcEnabled(bMpcEnabled);
	}

	return;
}

/*=============================================================================
    函 数 名： DetermineMSlave
    功    能： 主从协商
    算法实现： 
    全局变量： 
    参    数： [IN]     emMCUMSType   emLocalMSType, 
	           [IN/OUT] emMCUMSState  &emLocalMSState, 
	           [IN]     emMCUMSType   emRemoteMSType,
	           [IN/OUT] emMCUMSState  &emRemoteMSState
    返 回 值： BOOL32 TRUE-主从协商成功
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/22  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::DetermineMSlave( emMCUMSType emLocalMSType, emMCUMSState &emLocalMSState, 
										  emMCUMSType emRemoteMSType, emMCUMSState &emRemoteMSState )
{
	BOOL32 bDetermineok = TRUE;

	//主备用协商：vxworks下由硬件平台直接获取主备用结果，不进行协商
	//            vxworks下不支持由硬件平台直接获取主备用结果，则按照WIN32方式操作
	//            WIN32下进行采用Osp消息协商主备用状态
	
	if (m_bMSDetermineType)
	{
		if (IsActiveBoard())
		{
			emLocalMSState  = MCU_MSSTATE_ACTIVE;
			emRemoteMSState = MCU_MSSTATE_STANDBY;
		}
		else
		{
			emLocalMSState  = MCU_MSSTATE_STANDBY;
			emRemoteMSState = MCU_MSSTATE_ACTIVE;
		}
	}
	else
	{
		if (MCU_MSSTATE_ACTIVE == emRemoteMSState)
		{
			if (MCU_MSSTATE_ACTIVE == emLocalMSState)
			{
				bDetermineok = FALSE;
			}
			else
			{
				emLocalMSState  = MCU_MSSTATE_STANDBY;
			}
		}
		else if (MCU_MSSTATE_ACTIVE == emLocalMSState)
		{
			if (MCU_MSSTATE_ACTIVE == emRemoteMSState)
			{
				bDetermineok = FALSE;
			}
			else
			{
				emRemoteMSState  = MCU_MSSTATE_STANDBY;
			}
		}
		else
		{
			if (emRemoteMSType == emLocalMSType)
			{			
				bDetermineok = FALSE;
			}
			else
			{
				emRemoteMSState = (emRemoteMSType>emLocalMSType) ? MCU_MSSTATE_ACTIVE : MCU_MSSTATE_STANDBY;
				emLocalMSState  = (emRemoteMSType>emLocalMSType) ? MCU_MSSTATE_STANDBY : MCU_MSSTATE_ACTIVE;
			}
		}
	}

	return bDetermineok;
}

/*=============================================================================
    函 数 名： IsMSConfigInited
    功    能： 是否已经初始化
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 TRUE-已初始化
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::IsMSConfigInited( void )
{
	return m_bInited;
}

/*=============================================================================
函 数 名： SetMpcOusLed
功    能： 设置8000 mpc板ous灯是否点亮
算法实现： 
全局变量： 
参    数： BOOL32 bOpen
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/27  4.0			许世林                  创建
=============================================================================*/
void CMSManagerConfig::SetMpcOusLed(BOOL32 bOpen)
{
#ifndef WIN32
#ifndef _MINIMCU_    
    if (bOpen)
    {
        BrdLedStatusSet(LED_MPC_OUS, BRD_LED_ON);            
    }
    else
    {
        BrdLedStatusSet(LED_MPC_OUS, BRD_LED_OFF);  
    }    
#endif
#endif

    return;
}

/*=============================================================================
    函 数 名： SetMSSynOKFlag
    功    能： 设置 是否已完成主备数据倒换的同步操作
    算法实现： 
    全局变量： 
    参    数： BOOL32 bSynOK
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/12  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerConfig::SetMSSynOKFlag(BOOL32 bSynOK)
{
	//FALSE-备用板未完成同步则丢弃接受到的消息，防止误判
	m_bSynOK = bSynOK;

	return;
}

/*=============================================================================
    函 数 名： IsMSSynOK
    功    能： 获取 是否已完成主备数据倒换的同步操作
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 TRUE-备用板非锁定状态下可接收消息
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/12  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::IsMSSynOK(void)
{
	return m_bSynOK;
}

/*=============================================================================
函 数 名： SetMsSwitchOK
功    能： 设置主备切换是否成功
算法实现： 
全局变量： 
参    数： BOOL32 bSwitchOK
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/2/21  4.0			许世林                  创建
=============================================================================*/
void   CMSManagerConfig::SetMsSwitchOK(BOOL32 bSwitchOK)
{
    m_bMsSwitchOK = bSwitchOK;
    return;
}

/*=============================================================================
函 数 名： IsMsSwitchOK
功    能： 主备切换是否成功
算法实现： 
全局变量： 
参    数： void
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/2/21  4.0			许世林                创建
=============================================================================*/
BOOL32 CMSManagerConfig::IsMsSwitchOK(void)
{
    return m_bMsSwitchOK;
}


/*=============================================================================
    函 数 名： JudgeSndMsgPass
    功    能： 获取 是否允许业务模块向外部各App投递消息
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 FALSE- 丢弃 TRUE-允许
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/12  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::JudgeSndMsgPass(void)
{
	BOOL32 bRet = FALSE;
	
	//本端离线或者备用且则丢弃，否则允许向外部各App投递消息
	if (MCU_MSSTATE_ACTIVE == m_emCurMSState )
	{
		bRet = TRUE;
	}
	
	return bRet;
}

/*=============================================================================
    函 数 名： JudgeRcvMsgPass
    功    能： 获取 是否允许业务模块接收各外部App消息
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 FALSE- 丢弃 TRUE-允许
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/12  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::JudgeRcvMsgPass(void)
{	
	//本端离线或者备用且未完全同步则丢弃，否则允许向业务模块投递消息
	if (MCU_MSSTATE_OFFLINE == m_emCurMSState || 
		(MCU_MSSTATE_STANDBY == m_emCurMSState && FALSE == m_bSynOK))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/*=============================================================================
    函 数 名： EnterMSInitLock
    功    能： 进入 主备模块初始化锁定 处理 （只用一次）
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 锁定结果（无信号量时返回失败）
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/12  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::EnterMSInitLock( void )
{
	BOOL32 bRet = FALSE;

	if (NULL != m_hSemInitLock)
	{
		bRet = OspSemTakeByTime(m_hSemInitLock, MS_INIT_LOCK_SPAN_TIMEOUT);
	}

	return bRet;
}

/*=============================================================================
    函 数 名： LeaveMSInitLock
    功    能： 退出 主备模块初始化锁定 处理 （只用一次）
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： void（无信号量时直接返回）
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/12  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerConfig::LeaveMSInitLock( void )
{	
	if (NULL != m_hSemInitLock)
	{
		OspSemGive(m_hSemInitLock);
	}

	return;
}

/*=============================================================================
    函 数 名： EnterMSSynLock
    功    能： 进入 主备模块App与外部业务模块App操作竞争同步锁定 处理
    算法实现： 
    全局变量： 
    参    数： u16 wAppId App应用ID
    返 回 值： void（无信号量时直接返回）
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/15  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerConfig::EnterMSSynLock( u16 wAppId )
{
	if (NULL == m_hSemSynLock)
	{
		return;
	}

    OspSemTakeByTime(m_hSemSynLock, MS_SYN_LOCK_SPAN_TIMEOUT);
//	OspSemTake(m_hSemSynLock);
	m_wLockedAppId = wAppId;

	return;
}

/*=============================================================================
    函 数 名： LeaveMSSynLock
    功    能： 退出 主备模块App与外部业务模块App操作竞争同步锁定 处理
    算法实现： 
    全局变量： 
    参    数： u16 wAppId App应用ID
    返 回 值： void（无信号量时直接返回）
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/15  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerConfig::LeaveMSSynLock( u16 wAppId )
{
	if (NULL == m_hSemSynLock)
	{
		return;
	}
	
	//过滤可能的多次LeaveMSSynLock
	if (m_wLockedAppId == wAppId)
	{
		m_wLockedAppId = 0;
		OspSemGive(m_hSemSynLock);
	}

	return;
}


/*=============================================================================
函 数 名： IncDitheringTimes
功    能： 硬件抖动次数加1
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/2/18  4.0			许世林                  创建
=============================================================================*/
void CMSManagerConfig::IncDitheringTimes( void )
{
    m_dwDitheringTimes++;
    return;
}


/*=============================================================================
函 数 名： GetDitheringTimes
功    能： 获取系统运行至今硬件总抖动次数
算法实现： 
全局变量： 
参    数： void
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/2/18  4.0			许世林                  创建
=============================================================================*/
u32 CMSManagerConfig::GetDitheringTimes( void )
{
    return m_dwDitheringTimes;
}

/*=============================================================================
函 数 名： SetMSSsrc
功    能： 设置系统会话校验值
算法实现： 
全局变量： 
参    数： void
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/6/11   4.0			顾振华                  创建
=============================================================================*/
void CMSManagerConfig::SetMSSsrc( u32 dwSSrc )
{
    m_dwSysSSrc = htonl(dwSSrc);
}

/*=============================================================================
函 数 名： GetMSSsrc
功    能： 获取系统会话校验值
算法实现： 
全局变量： 
参    数： void
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/6/11   4.0			顾振华                  创建
=============================================================================*/
u32 CMSManagerConfig::GetMSSsrc( void ) const
{
    return ntohl(m_dwSysSSrc);
}

/*=============================================================================
函 数 名： SetCurMSSynState
功    能： 设置主备同步失败状态
算法实现： 
全局变量： 
参    数： TMSSynState *ptMSSynState：失败状态记录
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/06/02  4.0			张宝卿                  创建
=============================================================================*/
void CMSManagerConfig::SetCurMSSynState( TMSSynState &tMSSynState )
{
    m_tMSSynState = tMSSynState;
    return;
}

/*=============================================================================
函 数 名： GetCurMSSynState
功    能： 获取主备同步失败状态
算法实现： 
全局变量： 
参    数： void
返 回 值： TMSSynState
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/06/02  4.0			张宝卿                  创建
=============================================================================*/
TMSSynState CMSManagerConfig::GetCurMSSynState()
{
    return m_tMSSynState;
}

/*=============================================================================
    函 数 名： GetDebugLevel
    功    能： 获取调试打印等级
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： u8
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
u8 CMSManagerConfig::GetDebugLevel( void )
{
	return m_byDebugLevel;
}

/*=============================================================================
    函 数 名： SetDebugLevel
    功    能： 设置调试打印等级
    算法实现： 
    全局变量： 
    参    数： u8 byDebugLevel
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerConfig::SetDebugLevel( u8 byDebugLevel )
{
	if (byDebugLevel > MS_DEBUG_VERBOSE)
	{
		byDebugLevel = MS_DEBUG_VERBOSE;
	}
	m_byDebugLevel = byDebugLevel;

	return;
}

/*=============================================================================
    函 数 名： GetMSDetermineType
    功    能： 获取 是否采用定时器获取硬件主备用状态
	           vxworks下默认采用定时器获取硬件主备用状态，
	           vxworks下不支持由硬件平台直接获取主备用结果，则按照WIN32方式操作
	           WIN32下固定采用Osp消息协商主备用状态，若断链，则备用升为主用
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 TRUE－采用定时器获取硬件主备用状态；FALSE－采用Osp消息协商主备用状态
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::GetMSDetermineType( void )
{
	return m_bMSDetermineType;
}

/*=============================================================================
    函 数 名： SetMSDetermineType
    功    能： 设置 是否采用定时器获取硬件主备用状态
	           vxworks下默认采用定时器获取硬件主备用状态，
	           vxworks下不支持由硬件平台直接获取主备用结果，则按照WIN32方式操作
	           WIN32下固定采用Osp消息协商主备用状态，若断链，则备用升为主用
    算法实现： 
    全局变量： 
    参    数： BOOL32 bMSDetermineType TRUE－采用定时器获取硬件主备用状态；FALSE－采用Osp消息协商主备用状态
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerConfig::SetMSDetermineType( BOOL32 bMSDetermineType )
{
#ifdef WIN32
	m_bMSDetermineType = FALSE;
#else
	m_bMSDetermineType = bMSDetermineType;
#endif

	return;
}

/*=============================================================================
    函 数 名： RebootMCU
    功    能： 重启MCU
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/22  4.0			万春雷                  创建
=============================================================================*/
void CMSManagerConfig::RebootMCU( void )
{
#ifndef _8KH_
	OspQuit();
	OspDelay(1000);
#endif

#ifndef WIN32
	BrdHwReset();
	printf("[CMSManagerConfig]ms reboot\n");	
#endif

	return;
}

/*=============================================================================
    函 数 名： SetLocalMcuTime
    功    能： 设置本地系统时间
    算法实现： 
    全局变量： 
    参    数： time_t tTime
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/08  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::SetLocalMcuTime(time_t tTime)
{
	tTime += 0;  //传输消耗
    u16 wRet = g_cMcuAgent.SetSystemTime(tTime);
	return ( SUCCESS_AGENT == wRet);
}

/*=============================================================================
    函 数 名： IsActiveBoard
    功    能： 获取 当前是否为主用板
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 TRUE - 主用 FALSE - 备用
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/08  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::IsActiveBoard(void)
{
#ifndef WIN32  
#ifndef _MINIMCU_
    u8 byLocalMSState = BRD_MPC_RUN_SLAVE;
    byLocalMSState = BrdMPCQueryLocalMSState();
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MSMGR, "[IsActiveBoard] BrdMPCQueryLocalMSState->%d!\n", byLocalMSState);
    return (BRD_MPC_RUN_MASTER == byLocalMSState);
#else
    return TRUE;
#endif
#else   // WIN32
    return ( MCU_MSSTATE_ACTIVE == m_emCurMSState );
#endif    
}

/*=============================================================================
    函 数 名： GetSlotThroughName
    功    能： 由单板槽号得到单板名称
    算法实现： 
    全局变量： 
    参    数： const s8* pszName
	           u8* pbySlot
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/08  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::GetSlotThroughName(const s8* pszName, u8* pbySlot)
{
	if (0 == memcmp(pszName, "EX", 2) || 0 == memcmp(pszName, "MC", 2))
	{
		*pbySlot = atoi(pszName+2);
		return TRUE;
	}
	else if ( 0 == memcmp(pszName, "TVSEX", 5) )
	{
		*pbySlot = atoi(pszName+5);
		return TRUE;
	}
	else
	{
		*pbySlot = 0;
		return FALSE;
	}
}

/*=============================================================================
    函 数 名： GetTypeThroughName
    功    能： 由单板类型得到单板名称
    算法实现： 
    全局变量： 
    参    数： const s8* pszName
	           u8* pbyType
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/08  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMSManagerConfig::GetTypeThroughName( const s8* pszName, u8* pbyType )
{
	if (0 == strcmp(pszName, "MPC"))
	{
		*pbyType = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
		return TRUE;
	}
	//  [1/21/2011 chendaiwei]支持MPC2
	if (0 == strcmp(pszName, "MPC2"))
	{
		*pbyType = BRD_TYPE_MPC2/*DSL8000_BRD_MPC2*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "CRI"))
	{
		*pbyType = BRD_TYPE_CRI/*DSL8000_BRD_CRI*/;
		return TRUE;
	}
	//  [1/21/2011 chendaiwei] CRI2支持
	else if (0 == strcmp(pszName, "CRI2"))
	{
		*pbyType = BRD_TYPE_CRI2/*DSL8000_BRD_CRI*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "DTI"))
	{
		*pbyType = BRD_TYPE_DTI/*DSL8000_BRD_DTI*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "DIC"))
	{
		*pbyType = BRD_TYPE_DIC/*DSL8000_BRD_DIC*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "DRI"))
	{
		*pbyType = BRD_TYPE_DRI/*DSL8000_BRD_DRI*/;
		return TRUE;
	}
	//  [1/21/2011 chendaiwei]DRI2支持
	else if (0 == strcmp(pszName, "DRI2"))
	{
		*pbyType = BRD_TYPE_DRI2/*DSL8000_BRD_DRI2*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "MMP"))
	{
		*pbyType = BRD_TYPE_MMP/*DSL8000_BRD_MMP*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "VPU"))
	{
		*pbyType = BRD_TYPE_VPU/*DSL8000_BRD_VPU*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "DEC5"))
	{
		*pbyType = BRD_TYPE_DEC5/*DSL8000_BRD_DEC5*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "VAS"))
	{
		*pbyType = BRD_TYPE_VAS/*DSL8000_BRD_VAS*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "IMT"))
	{
		*pbyType = BRD_TYPE_IMT/*DSL8000_BRD_IMT*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "APU"))
	{
		*pbyType = BRD_TYPE_APU/*DSL8000_BRD_APU*/;
		return TRUE;
	}
	else if (0 == strcmp(pszName, "DSI"))
	{
		*pbyType = BRD_TYPE_DSI/*DSL8000_BRD_DSI*/;
		return TRUE;
	}
	else if( 0 == strcmp(pszName, "DSC") )
	{
		*pbyType = BRD_TYPE_DSC/*KDV8000B_MODULE*/;
		return TRUE;
	}
	else if( 0 == strcmp( pszName, "MDSC") )
	{
		*pbyType = BRD_TYPE_MDSC/*DSL8000_BRD_MDSC*/;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, "HDSC") )
	{
		*pbyType = BRD_TYPE_HDSC;
		return TRUE;
	}
	//4.6 新加版本 jlb  20090105
	else if ( 0 == strcmp( pszName, "HDU") )
	{
		*pbyType = BRD_TYPE_HDU/*BRD_HWID_KDM200_HDU*/;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, "HDU_L") )
	{
		*pbyType = BRD_TYPE_HDU_L;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, "MPU") )
	{
		*pbyType = BRD_TYPE_MPU/*BRD_HWID_DSL8000_MPU*/;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, "EBAP") )
	{
		*pbyType = BRD_HWID_EBAP;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, "EVPU") )
	{
		*pbyType = BRD_HWID_EVPU;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, "EAPU") )
	{
		*pbyType = BRD_TYPE_EAPU;
		return TRUE;
	}
	//[2011/01/25 zhushz]IS2.x单板支持
	else if ( 0 == strcmp( pszName, "IS21") )
	{
		*pbyType = BRD_TYPE_IS21;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, "IS22") )
	{
		*pbyType = BRD_TYPE_IS22;
		return TRUE;
	}
	// 4.7版本新板卡 [1/31/2012 chendaiwei]
	else if ( 0 == strcmp( pszName, BOARD_TYPE_HDU2) )
	{
		*pbyType = BRD_TYPE_HDU2;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, BOARD_TYPE_HDU2_S) )
	{
		*pbyType = BRD_TYPE_HDU2_S;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, BOARD_TYPE_HDU2_L) )
	{
		*pbyType = BRD_TYPE_HDU2_L;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, BOARD_TYPE_MPU2) )
	{
		*pbyType = BRD_TYPE_MPU2;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, BOARD_TYPE_MPU2ECARD) )
	{
		*pbyType = BRD_TYPE_MPU2ECARD;
		return TRUE;
	}
	else if ( 0 == strcmp( pszName, BOARD_TYPE_APU2) )
	{
		*pbyType = BRD_TYPE_MPU2;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*=============================================================================
    函 数 名： msdebug
    功    能： 设置主控热备份模块调试打印等级
    算法实现： 
    全局变量： 
    参    数： s32 nDbgLvl
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
API void msdebug( s32 nDbgLvl )
{
	if( nDbgLvl > 0 ) 
	{
		logenablemod(MID_MCU_MSMGR);
	}
	else
	{
		logdisablemod(MID_MCU_MSMGR);
	}

	if( nDbgLvl < MS_DEBUG_CRITICAL ) 
		nDbgLvl = MS_DEBUG_CRITICAL;
	if( nDbgLvl > MS_DEBUG_VERBOSE )
		nDbgLvl = MS_DEBUG_VERBOSE;
	
	g_cMSSsnApp.SetDebugLevel((u8)nDbgLvl);	

	return;
}

/*=============================================================================
函 数 名： setmschecktime
功    能： 设置主备状态检测时间间隔(调试接口)
算法实现： 
全局变量： 
参    数： u32 dwTime (单位ms)
返 回 值： API void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/2/18  4.0			许世林                  创建
=============================================================================*/
API void setmschecktime(u32 dwTime) 
{
    if(dwTime >= 50) //最小50ms
    {
        g_dwMsCheckTime = dwTime;
    }    
    
    return;
}

/*=============================================================================
    函 数 名： showmsstate
    功    能： 打印主备状态
    算法实现： 
    全局变量： 
    参    数： s32 nDbgLvl
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/24  4.0			万春雷                  创建
=============================================================================*/
API void showmsstate()
{
	s8  achPrt[MS_MAX_STRING_LEN];
	s8  achPrt2[MS_MAX_STRING_LEN];
	u32 dwLocalMcuIP   = g_cMSSsnApp.GetLocalMcuIP();
	u32 dwRemoteMcuIP  = g_cMSSsnApp.GetAnotherMcuIP();
	u8  byLocalMcuType = g_cMSSsnApp.GetLocalMcuType();
	BOOL32 bDoubleLink = g_cMSSsnApp.IsDoubleLink();
	
	BOOL32 bInited   = g_cMSSsnApp.IsMSConfigInited();
    BOOL32 bConnected= g_cMSSsnApp.IsRemoteMpcConnected();
	BOOL32 bSynOK    = g_cMSSsnApp.IsMSSynOK();	
    BOOL32 bSwitchOk = g_cMSSsnApp.IsMsSwitchOK();	
	BOOL32 bMSDetermineType    = g_cMSSsnApp.GetMSDetermineType();
	emMCUMSType  emLocalMSType = g_cMSSsnApp.GetLocalMSType();
	emMCUMSState emCurMSState  = g_cMSSsnApp.GetCurMSState();
    u32 dwDitheringTimes = g_cMSSsnApp.GetDitheringTimes();

	u32 dwSysSSrc = g_cMSSsnApp.GetMSSsrc();
	
	memset(achPrt, 0, sizeof(achPrt));
	if (MCU_TYPE_KDV8000 == byLocalMcuType)
	{
		const s8* pszName = "KDV8000A";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt[MS_MAX_STRING_LEN-1] = '\0';
	}
	else if (MCU_TYPE_KDV8000B == byLocalMcuType)
	{
		const s8* pszName = "KDV8000B";
		memcpy(achPrt, "KDV8000B", strlen(pszName) + 1);
		achPrt[MS_MAX_STRING_LEN-1] = '\0';
	}
	else if (MCU_TYPE_WIN32 == byLocalMcuType)
	{
		const s8* pszName = "KDVWIN32";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt[MS_MAX_STRING_LEN-1] = '\0';
	}
    else if (MCU_TYPE_KDV8000C == byLocalMcuType)
    {
		const s8* pszName = "8000C";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
        achPrt[MS_MAX_STRING_LEN-1] = '\0';
    }    
	else
	{
		const s8* pszName = "UNKNOWN";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt[MS_MAX_STRING_LEN-1] = '\0';
	}
	
	StaticLog("MS Base State: LocalMcuIP:0x%0x RemoteMcuIP:0x%0x McuType:%s DoubleLink:%d Connected: %d SysSSRC:%u\n", 
		      dwLocalMcuIP, dwRemoteMcuIP, achPrt, bDoubleLink, bConnected, dwSysSSrc);

	memset(achPrt, 0, sizeof(achPrt));
	if (MCU_MSTYPE_MASTER == emLocalMSType)
	{
		const s8* pszName = "MC1(MST)";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt[MS_MAX_STRING_LEN-1] = '\0';
	}
	else if (MCU_MSTYPE_SLAVE == emLocalMSType)
	{
		const s8* pszName = "MC0(SLV)";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt[MS_MAX_STRING_LEN-1] = '\0';
	}
	else
	{
		const s8* pszName = "INVALID";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt[MS_MAX_STRING_LEN-1] = '\0';
	}
	
	memset(achPrt2, 0, sizeof(achPrt2));
	if (MCU_MSSTATE_ACTIVE == emCurMSState)
	{
		const s8* pszName = "ACTIVE";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt2[MS_MAX_STRING_LEN-1] = '\0';
	}
	else if (MCU_MSSTATE_STANDBY == emCurMSState)
	{
		const s8* pszName = "STANDBY";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt2[MS_MAX_STRING_LEN-1] = '\0';
	}
	else
	{
		const s8* pszName = "OFFLINE";
		const s32 nDstBufLen = sizeof(achPrt);
		const s32 nSrcNameLen = strlen(pszName) + 1;
		const s32 nMaxCpyLen = min(nSrcNameLen, nDstBufLen);
		memcpy(achPrt, pszName, nMaxCpyLen);
		achPrt2[MS_MAX_STRING_LEN-1] = '\0';
	}
	
	StaticLog("MS Comm State: MSType:%s MSState:%s Inited:%d SynOK:%d SwitchOK:%d MSDetermineType:%d HWDitheringTimes:%u\n", 
		      achPrt, achPrt2, bInited, bSynOK, bSwitchOk, bMSDetermineType, dwDitheringTimes);

	return;
}

//END OF FILE
