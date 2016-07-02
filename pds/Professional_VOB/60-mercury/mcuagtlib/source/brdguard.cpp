/*****************************************************************************
   模块名      : Board Guard
   文件名      : BoardGuard.cpp
   相关文件    : brdguard.h
   文件实现功能: 单板守卫基类实现，单板状态监控
   作者        : jianghy
   版本        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人          修改内容
   2003/08/25  1.0         jianghy           创建
   2004/11/10  3.6         libo           新接口修改
   2004/11/29  3.6         libo           移植到Linux
   2005/08/17  4.0         liuhuiyun      4.0
   2006/04/30  4.0		   liuhuiyun      增加对Trap发送的保护
   2011/05/03  4.0         miaoqingsong   添加MPC2告警支持
******************************************************************************/
#include "brdguard.h"
#include "agtcomm.h"
#include "configureagent.h"
#include "agentinterface.h"

// #ifdef _LINUX_
// #include <sys/sysinfo.h>
// #endif

/*lint -save -esym(714,g_bAlarmState)*/
// MCU的守卫应用实例
CBoardGuardApp	g_cBrdGuardApp;
//BOOL32          g_bAlarmState = FALSE;      // 全局变量，用于标记新机框电源和风扇告警状态

// 构造函数
CBoardGuard::CBoardGuard()
{
	m_byPowerStatus = 0;
	m_byNetSyncStatus = 0;
	m_byPowerFanStatus = 0;
	m_byBoxFanStatus = 0;           
	m_byMPCCpuStatus = 0;
	m_byMPC2TempStatus = 0;
	m_byPowerTempStatus = 0;
	m_byMPCMemoryStatus = 0;
	m_dwSDHStatus = 0;
	m_byOldRemoteMpcState = 255;
	m_b8KASlamOn = FALSE;
#ifdef _8KI_
	m_dwCpuFan = 0;
	m_byCpuTemp = 0;
#endif
}

// 析构函数
CBoardGuard::~CBoardGuard()
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
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CBoardGuard::InstanceEntry(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
	{
		OspPrintf(TRUE, FALSE,  "[CBoardGuard]: Error input parameter.!");
		return;
	}

	switch( pcMsg->event )
	{
	case BOARD_GUARD_POWERON:       //启动消息
		ProcGuardPowerOn();
		break;

	case MCUAGENT_SCAN_STATE_TIMER:	//扫描定时器到期
		ProcGuardStateScan();
		break;

    case AGT_SVC_REBOOT:            // 重启由mcu业务层来完成
        ProcReboot(pcMsg);
        break;

	default:
		break;
	}	
	return;
}

/*====================================================================
    函数名      ：ProcGuardPowerOn
    功能        ：GUARD模块启动消息
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CBoardGuard::ProcGuardPowerOn()
{
	SetTimer( MCUAGENT_SCAN_STATE_TIMER, SCAN_STATE_TIME_OUT ); // 等待系统稳态

	return;
}

/*=============================================================================
  函 数 名： ProcReboot
  功    能： 重启
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
=============================================================================*/
void CBoardGuard::ProcReboot(CMessage* const pMsg)
{
    if(NULL != pMsg)
    {

#if ( defined(_8KE_) || defined(_8KH_) || defined(_8KI_)) && defined(_LINUX_)
		//清业务正常启动标记
		McuAgentQuit(FALSE);

	#ifndef _8KH_
		OspQuit();
	#endif
		
		OspDelay(100);     

//		s8    achProfileName[MAXLEN_MCU_FILEPATH] = {0};
//		sprintf(achProfileName, "%s/%s", DIR_DATA, RUNSTATUS_8KE_CHKFILENAME);
//		s32 dwRunSt = 0;
//		SetRegKeyInt( achProfileName, SECTION_RUNSTATUS, KEY_MCU8KE, dwRunSt );
		//execute reboot
		s8   chCmdline[256] =  {0};   
		sprintf(chCmdline,  "reboot");
		system(chCmdline);

#elif defined(_LINUX_)	
        printf("[ProcReboot] Reboot the mcu\n");

        OspQuit();
        
        OspDelay( 1000 );

        BrdHwReset(); // restart
#else
#endif
#ifdef WIN32
        OspPrintf(TRUE, FALSE, "[ProcReboot] Recv reboot message\n");
#endif
    }
    return;
}
/*==============================================================================================
    函数名      ：ProcGuardStateScan
    功能        ：单板状态的定时扫描
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
------------------------------------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
    04/11/10    3.6         libo          新接口的修改
	06/04/28    4.0         john          增加对是否有网管服务器的判断
	10/12/15    4.0         苗庆松        增加机箱风扇、MPC板内存和CPU使用率、MPC2板温度异常告警
	11/05/03    4.0         苗庆松        增加电源和电源风扇异常告警
================================================================================================*/
void CBoardGuard::ProcGuardStateScan(void)
{

#ifndef WIN32
    TBrdLedState tBrdLedState;

#ifndef _MINIMCU_

    TBrdAlarmState      tBrdAlarm;
    TBrdMPCAlarmVeneer* ptMpcAlarm = NULL;
	TBrdMPCAlarmSDH*    ptSDHAlarm = NULL;
	TPowerStatus        tPowerStatus;
	TPowerFanStatus     tPowerFanStatus;
	
	u8 byNetSyncMode;
	u32 dwOldSDHStatus;
		
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );

    u8 byRet = BrdMPCQueryAnotherMPCState();

	TBrdStatus tBrdStatus;
	if( tMPCInfo.GetIsHaveOtherMpc() )  // 防止远端Mpc不存在时，第一次误告警
	{
		if( byRet != m_byOldRemoteMpcState ) // 状态改变
		{
			//  [1/21/2011 chendaiwei]支持MPC2
#ifdef _LINUX12_
			tBrdStatus.byType = BRD_TYPE_MPC2;
#else
            tBrdStatus.byType   = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
#endif
			tBrdStatus.byLayer  = tMPCInfo.GetOtherMpcLayer();
			tBrdStatus.bySlot   = tMPCInfo.GetOtherMpcSlot(); 
			tBrdStatus.byStatus = g_cCfgParse.BrdStatusHW2Agt(byRet);
            tBrdStatus.byOsType = tMPCInfo.GetOSType();

			post( MAKEIID(AID_MCU_AGENT, 1), MCU_AGT_BOARD_STATUSCHANGE, 
				                 (u8*)&tBrdStatus, sizeof(tBrdStatus) );

            tMPCInfo.SetOtherMpcStatus( BOARD_STATUS_INLINE );
            g_cCfgParse.SetMPCInfo( tMPCInfo );
			m_byOldRemoteMpcState = g_cCfgParse.BrdStatusHW2Agt(byRet);
			Agtlog(LOG_VERBOSE, "[ProcGuardStateScan] The Remote Mpc(%d, %d)'s state is changed (%d ---0: In, 1: Out).\n", 
                                                       tMPCInfo.GetOtherMpcLayer(), tMPCInfo.GetOtherMpcSlot(), byRet );
		}
	}

#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)

	// 扫描单板的状态
    if( g_cCfgParse.IsMpcActive() )
    {   
	    // MPC告警
        BrdAlarmStateScan(&tBrdAlarm);

	    // 左48V电源状态反转
        ptMpcAlarm = &tBrdAlarm.nlunion.tBrdMPCAlarmAll.tBrdMPCAlarmVeneer;
		Agtlog(LOG_INFORM,"[ProcGuardStateScan]MPCBrdState:Power.bAlarmPowerDC48VLDown:%d,\
			Power.bAlarmPowerDC5VLDown:%d,Power.bAlarmPowerDC48VRDown:%d,Power.bAlarmPowerDC5VRDown:%d\n",
			ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC48VLDown,
			ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC5VLDown,
			ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC48VRDown,
			ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC5VRDown);
		Agtlog(LOG_INFORM,"[ProcGuardStateScan]MPCBrdState:PowerFan.bAlarmPowerFanLLDown:%d,\
			PowerFan.bAlarmPowerFanLRDown:%d,PowerFan.bAlarmPowerFanRLDown:%d,PowerFan.bAlarmPowerFanRRDown:%d\n",
			ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanLLDown,
			ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanLRDown,
			ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanRLDown,
			ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanRRDown);

	    if (ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC48VLDown != (BOOL32)((m_byPowerStatus & 0x08) == 0x08))
	    {
		    m_byPowerStatus ^= 0x08;

		    tPowerStatus.bySlot = POWER_LEFT;
		    tPowerStatus.byType = POWER_48V;
		    tPowerStatus.byStatus = m_byPowerStatus & 0x08;
			post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_STATUS, &tPowerStatus, sizeof(tPowerStatus) );
	    }

        // 左5v
	    if (ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC5VLDown != (BOOL32)((m_byPowerStatus & 0x04) == 0x04))
	    {
		    m_byPowerStatus ^= 0x04;

		    tPowerStatus.bySlot = POWER_LEFT;
		    tPowerStatus.byType = POWER_5V;
		    tPowerStatus.byStatus = m_byPowerStatus & 0x04;
			post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_STATUS, &tPowerStatus, sizeof(tPowerStatus) );
	    }

	    //右48v
	    if (ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC48VRDown != (BOOL32)((m_byPowerStatus & 0x02) == 0x02))
	    {
		    //状态反转
		    m_byPowerStatus ^= 0x02;

		    tPowerStatus.bySlot = POWER_RIGHT;
		    tPowerStatus.byType = POWER_48V;
		    tPowerStatus.byStatus = m_byPowerStatus & 0x02;
			post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_STATUS, &tPowerStatus, sizeof(tPowerStatus) );
	    }

	    // 右5v
	    if (ptMpcAlarm->tBrdMPCAlarmPower.bAlarmPowerDC5VRDown != (BOOL32)((m_byPowerStatus & 0x01) == 0x01)) 
	    {
		    //状态反转
		    m_byPowerStatus ^= 0x01;

		    tPowerStatus.bySlot = POWER_RIGHT;
		    tPowerStatus.byType = POWER_5V;
		    tPowerStatus.byStatus = m_byPowerStatus & 0x01;
			post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_STATUS, &tPowerStatus, sizeof(tPowerStatus) );
	    }

	    // 电源风扇状态
	    // 左电源左风扇停转
	    if (ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanLLDown != (BOOL32)((m_byPowerFanStatus & 0x08) == 0x08))
	    {
		    m_byPowerFanStatus ^= 0x08;

		    tPowerFanStatus.bySlot = POWER_LEFT;
		    tPowerFanStatus.byFanPos = FAN_LEFT;
		    tPowerFanStatus.byStatus = m_byPowerFanStatus & 0x08;
			post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_FAN_STATUS, &tPowerFanStatus, sizeof(tPowerFanStatus) );
	    }

	    // 左电源右风扇停转
	    if (ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanLRDown != (BOOL32)((m_byPowerFanStatus & 0x04) == 0x04))
	    {
		    m_byPowerFanStatus ^= 0x04;

		    tPowerFanStatus.bySlot = POWER_LEFT;
		    tPowerFanStatus.byFanPos = FAN_RIGHT;
		    tPowerFanStatus.byStatus = m_byPowerFanStatus & 0x04;
            post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_FAN_STATUS, &tPowerFanStatus, sizeof(tPowerFanStatus) );
	    }

	    // 右电源左风扇停转
	    if (ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanRLDown != (BOOL32)((m_byPowerFanStatus & 0x02) == 0x02))
	    {
		    // 状态反转
		    m_byPowerFanStatus ^= 0x02;

		    tPowerFanStatus.bySlot = POWER_RIGHT;
		    tPowerFanStatus.byFanPos = FAN_LEFT;
		    tPowerFanStatus.byStatus = m_byPowerFanStatus & 0x02;
            post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_FAN_STATUS, &tPowerFanStatus, sizeof(tPowerFanStatus) );
	    }

	    // 右电源右风扇停转
	    if (ptMpcAlarm->tBrdMPCAlarmPowerFan.bAlarmPowerFanRRDown != (BOOL32)((m_byPowerFanStatus & 0x01) == 0x01)) 
	    {
		    // 状态反转
		    m_byPowerFanStatus ^= 0x01;

		    tPowerFanStatus.bySlot = POWER_RIGHT;
		    tPowerFanStatus.byFanPos = FAN_RIGHT;
		    tPowerFanStatus.byStatus = m_byPowerFanStatus & 0x01;
            post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_POWER_FAN_STATUS, &tPowerFanStatus, sizeof(tPowerFanStatus) );
	    }

		BOOL32 bCur8KASlamOn = FALSE;
		//判断当前电源或电风扇是否存在异常
		if ( (BOOL32)(m_byPowerStatus & 0x0F)  || (BOOL32)(m_byPowerFanStatus & 0x0F) )
		{
			bCur8KASlamOn = TRUE;
		}
		//要点亮alarm，且当前灯没点亮
		if ( TRUE == bCur8KASlamOn && FALSE == m_b8KASlamOn )
		{
// 			BrdLedStatusSet(LED_BOARD_LED_SALM, BRD_LED_ON);   // 点亮机箱告警灯
// 			BrdMPCLedBoardSpeakerSet(LED_BOARD_SPK_ON);        // 响起机箱蜂鸣器
			m_b8KASlamOn = TRUE;
			Agtlog(LOG_INFORM,"[ProcGuardStateScan] Turn On  SAlarm !\n");
		}
		else if ( FALSE == bCur8KASlamOn && TRUE == m_b8KASlamOn )//要灭alarm，且当前灯是亮着的
		{
			BrdLedStatusSet(LED_BOARD_LED_SALM, BRD_LED_OFF);      // 灭掉机箱告警灯
 			BrdMPCLedBoardSpeakerSet(LED_BOARD_SPK_OFF);           // 关闭机箱蜂鸣器
			m_b8KASlamOn = FALSE;
			Agtlog(LOG_INFORM,"[ProcGuardStateScan] Turn Off  SAlarm !\n");
		}


	    
       // 网同步, 在跟踪的时候才有
	    byNetSyncMode = BrdMPCQueryNetSyncMode();
	    if( byNetSyncMode != SYNC_MODE_FREERUN )
	    {
		    if (ptMpcAlarm->bAlarmNetClkLockFailed != (BOOL32)((m_byNetSyncStatus & 0x01) == 0x01))
		    {
			    // 反转
			    m_byNetSyncStatus ^= 0x01;
				// 通知代理
				post( MAKEIID(AID_MCU_AGENT, 1), MCU_AGT_SYNCSOURCE_STATUSCHANGE, 
						         &m_byNetSyncStatus, sizeof(m_byNetSyncStatus) );
		    }
	    }

	    // SDH告警
        ptSDHAlarm = &tBrdAlarm.nlunion.tBrdMPCAlarmAll.tBrdMPCAlarmSDH;
	    dwOldSDHStatus = m_dwSDHStatus;

	    // 光信号丢失
	    if (ptSDHAlarm->bAlarmLOS != (BOOL32)((m_dwSDHStatus & 0x00000001) == 0x00000001))
		    m_dwSDHStatus ^= 0x00000001;

	    // 帧丢失
	    if (ptSDHAlarm->bAlarmLOF != (BOOL32)((m_dwSDHStatus & 0x00000002) == 0x00000002))
		    m_dwSDHStatus ^= 0x00000002;

	    // 帧失步
	    if (ptSDHAlarm->bAlarmOOF != (BOOL32)((m_dwSDHStatus & 0x00000004) == 0x00000004))
		    m_dwSDHStatus ^= 0x00000004;

	    // 复帧丢失
	    if (ptSDHAlarm->bAlarmLOM != (BOOL32)((m_dwSDHStatus & 0x00000008) == 0x00000008))
		    m_dwSDHStatus ^= 0x00000008;

	    // 管理单元指针丢失
	    if (ptSDHAlarm->bAlarmAU_LOP != (BOOL32)((m_dwSDHStatus & 0x00000010) == 0x00000010))
		    m_dwSDHStatus ^= 0x00000010;

	    // 复用段远端缺陷指示
	    if (ptSDHAlarm->bAlarmMS_RDI != (BOOL32)((m_dwSDHStatus & 0x00000020) == 0x00000020))
		    m_dwSDHStatus ^= 0x00000020;

	    // 复用段报警指示
	    if (ptSDHAlarm->bAlarmMS_AIS != (BOOL32)((m_dwSDHStatus & 0x00000040) == 0x00000040))
		    m_dwSDHStatus ^= 0x00000040;

	    // 高阶通道远端缺陷指示
	    if (ptSDHAlarm->bAlarmHP_RDI != (BOOL32)((m_dwSDHStatus & 0x00000080) == 0x00000080))
		    m_dwSDHStatus ^= 0x00000080;

	    // 高阶通道报警指示
	    if (ptSDHAlarm->bAlarmHP_AIS != (BOOL32)((m_dwSDHStatus & 0x00000100) == 0x00000100))
		    m_dwSDHStatus ^= 0x00000100;

	    // 再生段通道踪迹字节不匹配
	    if (ptSDHAlarm->bAlarmRS_TIM != (BOOL32)((m_dwSDHStatus & 0x00000200) == 0x00000200))
		    m_dwSDHStatus ^= 0x00000200;

	    // 高阶通道踪迹字节不匹配
	    if (ptSDHAlarm->bAlarmHP_TIM != (BOOL32)((m_dwSDHStatus & 0x00000400) == 0x00000400))
		    m_dwSDHStatus ^= 0x00000400;

	    // 高阶通道信号标记字节未装载
	    if (ptSDHAlarm->bAlarmHP_UNEQ != (BOOL32)((m_dwSDHStatus & 0x00000800) == 0x00000800))
		    m_dwSDHStatus ^= 0x00000800;

	    // 高阶通道信号标记字节不匹配
	    if (ptSDHAlarm->bAlarmHP_PLSM != (BOOL32)((m_dwSDHStatus & 0x00001000) == 0x00001000))
		    m_dwSDHStatus ^= 0x00001000;

	    // 支路单元指针丢失
	    if (ptSDHAlarm->bAlarmTU_LOP != (BOOL32)((m_dwSDHStatus & 0x00002000) == 0x00002000))
		    m_dwSDHStatus ^= 0x00002000;

	    // 低阶通道远端缺陷指示
	    if (ptSDHAlarm->bAlarmLP_RDI != (BOOL32)((m_dwSDHStatus & 0x00004000) == 0x00004000))
		    m_dwSDHStatus ^= 0x00004000;

	    // 低阶通道报警指示
	    if (ptSDHAlarm->bAlarmLP_AIS != (BOOL32)((m_dwSDHStatus & 0x00008000) == 0x00008000))
		    m_dwSDHStatus ^= 0x00008000;

	    // 低阶通道踪迹字节不匹配
	    if (ptSDHAlarm->bAlarmLP_TIM != (BOOL32)((m_dwSDHStatus & 0x00010000) == 0x00010000))
		    m_dwSDHStatus ^= 0x00010000;

	    // 低阶通道信号标记字节不匹配
	    if (ptSDHAlarm->bAlarmLP_PLSM != (BOOL32)((m_dwSDHStatus & 0x00020000) == 0x00020000))
		    m_dwSDHStatus ^= 0x00020000;
	    // 有状态改变
	    if (dwOldSDHStatus != m_dwSDHStatus)
	    {		
			post( MAKEIID(AID_MCU_AGENT), SVC_AGT_SDH_STATUS, &m_dwSDHStatus, sizeof(m_dwSDHStatus) );
	    }
    }
#endif // end !_8KE_
#endif // !_MINIMCU_


#if defined(_8KI_) && defined(_LINUX_)
	TBrdCtrlInfo tBreCtrlInfo;
	memset( &tBreCtrlInfo,0,sizeof(tBreCtrlInfo) );
	u8 byOldCpuFanState = m_dwCpuFan >= 100 ? 0 : 1;//风扇转速大于100为正常值
	u8 byOldCpuTempState = m_byCpuTemp <= 85 ? 0 : 1;//cpu温度小于等于85度为正常值
	if( OK != GetBrdCtrlInfo(&tBreCtrlInfo) )
	{
		Agtlog(LOG_ERROR, "[ProcGuardStateScan] BrdQueryLedState failed !\n");
	}

	
#else
	// led status
    if (OK != BrdQueryLedState(&tBrdLedState)) 
    {
        Agtlog(LOG_ERROR, "[ProcGuardStateScan] BrdQueryLedState failed !\n");
    }
#endif//end defined(_8KI_)
    else
    {
#if defined(_8KI_) && defined(_LINUX_)
		// 初始化tBrdLedState信息
		memset(&tBrdLedState, 0, sizeof(tBrdLedState));
		tBrdLedState.dwLedNum = sizeof(TBrdMPCLedStateDesc);
		memset(&tBrdLedState.nlunion.tBrdMPCLedState, BRD_LED_OFF, sizeof(tBrdLedState.nlunion.tBrdMPCLedState));//初始化为灯灭
	
		tBrdLedState.nlunion.tBrdMPCLedState.byLedRUN = ( tBreCtrlInfo.dwRUNLedStat <= 0 ) ? 0xff : (u8)tBreCtrlInfo.dwRUNLedStat;		
		tBrdLedState.nlunion.tBrdMPCLedState.byLedALM = ( tBreCtrlInfo.dwALMLedStat <= 0 ) ? 0xff : (u8)tBreCtrlInfo.dwALMLedStat;		
		u8 byCurCpuFanState = tBreCtrlInfo.dwCpuFanRPM >= 100 ? 0 : 1;		
		if( byOldCpuFanState != byCurCpuFanState )
		{
			post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_CPU_FAN_STATUS, &byCurCpuFanState, sizeof(byCurCpuFanState));
		}
		m_dwCpuFan = tBreCtrlInfo.dwCpuFanRPM;
		u8 byCurCpuTempState = tBreCtrlInfo.dwCpuTemp <= 85 ? 0 : 1;	
		if( byOldCpuTempState != byCurCpuTempState )
		{
			post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_CPUTEMP_STATUS, &byCurCpuTempState, sizeof(byCurCpuTempState));
		}
		m_byCpuTemp = tBreCtrlInfo.dwCpuTemp;
#endif
        if (0 != memcmp(&tBrdLedState, &m_tBrdLedState, sizeof(TBrdLedState))  )
        {
            Agtlog(LOG_INFORM, "[ProcGuardStateScan] tBrdLedState not equal to m_tBrdLedState !\n");
            
            memcpy( &m_tBrdLedState, &tBrdLedState, sizeof(TBrdLedState) );
		    post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_LED_STATUS, &m_tBrdLedState, sizeof(m_tBrdLedState));
        }
        else
        {
            Agtlog(LOG_VERBOSE, "[ProcGuardStateScan] tBrdLedState equal to m_tBrdLedState !\n");
		} 

        s8  abyBuf[256] = {'\0'};
        s8 *pBuff = abyBuf;
        u8  abyLedBuf[ MAX_BOARD_LED_NUM + 1 ]={0};    // led BUF  最大32个灯
        u8  byLedCount = 0;                            // led个数
#ifdef _LINUX12_
        byLedCount = sizeof(TBrdMPC2LedDesc);
        memcpy( abyLedBuf, &m_tBrdLedState.nlunion.tBrdMPC2LedState, byLedCount);
#else
        byLedCount = sizeof(TBrdMPCLedStateDesc);
        memcpy( abyLedBuf, &m_tBrdLedState.nlunion.tBrdMPCLedState, byLedCount);
#endif
        pBuff += sprintf(pBuff, "[ProcGuardStateScan] Led Num:%d [", byLedCount );
        u8 byIdx = 0;    
        for( byIdx = 0; byIdx < byLedCount; byIdx++)
        {
            if ( byIdx >= MAX_BOARD_LED_NUM )
            {
                break;
            }        
            pBuff += sprintf(pBuff, " %d ", abyLedBuf[byIdx] );
        } 
        pBuff += sprintf(pBuff, "]\n");
        Agtlog(LOG_VERBOSE, abyBuf);
    }


// #endif // !_MINIMCU_

// #ifdef _MINIMCU_
//     // led status
//     if (OK != BrdQueryLedState(&tBrdLedState)) 
//     {
//         Agtlog(LOG_VERBOSE, "[ProcGuardStateScan] BrdQueryLedState failed !\n");
//     }
//     else
//     {        	
//         if (0 != memcmp(&tBrdLedState, &m_tBrdLedState, sizeof(TBrdLedState))  )
//         {
//             Agtlog(LOG_WARN, "[ProcGuardStateScan] tBrdLedState not equal to m_tBrsLedState !\n");
// 
//             memcpy( &m_tBrdLedState, &tBrdLedState, sizeof(TBrdLedState) );
// 		    post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_LED_STATUS, &m_tBrdLedState, sizeof(m_tBrdLedState));
//         }
//         else
//         {
// 			Agtlog(LOG_VERBOSE, "[ProcGuardStateScan] tBrdLedState equal to m_tBrsLedState !\n");
//         } 
//     }
// #endif // _MINIMCU_
//目前风扇状态获取暂不支持，避免刷过多打印[1/25/2013 chendaiwei]
// #ifdef _LINUX12_
// 	
// 	// [miaoqingsong 20101214 add] MCU8000A机箱风扇异常告警上报  
// 	TBoxFanStatus       tBoxFanStatus;              
// 	u8                  byFanId;                                     
// 	u16                 wFanSpeed;
// 	
// 	for ( byFanId = 0; byFanId < MCU8000ABOXFAN_NUM; byFanId++ )
// 	{
// 		u32 dwRet = BrdMPCFanBrdGetFanSpeed( byFanId, &wFanSpeed );
// 		
// 		if ( OK == dwRet )
// 		{
// 			if ( (wFanSpeed < MIN_MCU8000ABOXFAN_SPEED) && ((((unsigned int)1<<byFanId) & m_byBoxFanStatus) == 0) )
// 			{
// 				tBoxFanStatus.bySlot = byFanId / 2;
// 				tBoxFanStatus.byFanPos = byFanId % 2 + 1;
// 				tBoxFanStatus.byStatus = 1;
// 				
// 				post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_BOX_FAN_STATUS, &tBoxFanStatus, sizeof(tBoxFanStatus) );
// 				
// 				m_byBoxFanStatus |= (unsigned int)1 << byFanId;
// 			}
// 			else
// 			{
// 				if ( 0 < (((unsigned int)1<<byFanId) & m_byBoxFanStatus) ) 
// 					
// 				{
// 					tBoxFanStatus.bySlot = byFanId / 2;
// 					tBoxFanStatus.byFanPos = byFanId % 2 + 1;
// 					tBoxFanStatus.byStatus = 0;
// 					
// 					post( MAKEIID(AID_MCU_AGENT, 1), SVC_AGT_BOX_FAN_STATUS, &tBoxFanStatus, sizeof(tBoxFanStatus) );
// 					
// 					m_byBoxFanStatus ^= (unsigned int)1 << byFanId;      
// 				}
// 			}
// 		}
// 		else
// 		{
// 			Agtlog( LOG_ERROR, "[BrdMPCFanBrdGetFanSpeed] get BoxFan's speed failed!\n");
// 		}
// 	}
// #endif  // _LINUX12_

#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
	
	// [2010/12/15 miaoqingsong add] MPC板内存使用率告警上报网管，当内存使用率大于等于85%时上报告警
    u32 dwByteFree = 0;
    u32 dwBytesAlloc = 1;
	TMPCMemoryStatus tMpcMemoryStatus;
    memset( &tMpcMemoryStatus, 0, sizeof(tMpcMemoryStatus) );
    if(OK == BrdGetMemInfo( &dwByteFree, &dwBytesAlloc) )
	{   
        // [20120718 liaokang] 防止溢出
        dwByteFree = dwByteFree>>20;       // byte ----> MB   防止溢出
        dwBytesAlloc = dwBytesAlloc>>20;   // byte ----> MB   防止溢出

		tMpcMemoryStatus.byMemoryAllocRate = dwBytesAlloc * 100 / (dwBytesAlloc + dwByteFree);
		
		if ( (tMpcMemoryStatus.byMemoryAllocRate > 85) && (m_byMPCMemoryStatus == 0) )    // 内存使用率超过85%
		{
			Agtlog( LOG_WARN,"[ProcGuardStateScan] The Mpc's Memory is not enough: %d\n", tMpcMemoryStatus.byMemoryAllocRate);
			
			tMpcMemoryStatus.byMemoryStatus = 1;
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_MPCMEMORY_STATUS, &tMpcMemoryStatus, sizeof(tMpcMemoryStatus));
			m_byMPCMemoryStatus = 1;
		}
		
		if ( (tMpcMemoryStatus.byMemoryAllocRate <= 85) && (m_byMPCMemoryStatus == 1) )
		{
			tMpcMemoryStatus.byMemoryStatus = 0;
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_MPCMEMORY_STATUS, &tMpcMemoryStatus, sizeof(tMpcMemoryStatus));
			m_byMPCMemoryStatus = 0;
		}
        Agtlog( LOG_VERBOSE,"[ProcGuardStateScan] Free Mem:%u(MB), Alloc Mem:%u(MB), Rate:%u, Status:%u!\n", 
        dwByteFree, dwBytesAlloc, tMpcMemoryStatus.byMemoryAllocRate, m_byMPCMemoryStatus );
	}
	else
	{
		Agtlog( LOG_ERROR, "[BrdGetMemInfo] Error Get Memory Percentage.\n");
	}

	// [2010/12/15 miaoqingsong add] MPC板CPU占用率告警上报网管，当Cpu占用率大于等于85%时上报告警
	u8             byCpuIdleRate = 0;
	TOspCpuInfo    tCpuInfo;
	TMPCCpuStatus  tMpcCpuStatus;
    memset(&tMpcCpuStatus, 0, sizeof(tMpcCpuStatus) );
	
	if ( OspGetCpuInfo(&tCpuInfo) )
    {
		byCpuIdleRate = tCpuInfo.m_byIdlePercent;
        tMpcCpuStatus.byCpuAllocRate = 100 - byCpuIdleRate;
		       
		if ( (tMpcCpuStatus.byCpuAllocRate > 85) && (BOOL32)(m_byMPCCpuStatus == 0) )    // Cpu占用率超过85%
        {           
			tMpcCpuStatus.byCpuStatus = 1;
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_CPU_STATUS, &tMpcCpuStatus, sizeof(tMpcCpuStatus));
			m_byMPCCpuStatus = 1;
        }
        
		if ( (tMpcCpuStatus.byCpuAllocRate <= 85) && (BOOL32)( m_byMPCCpuStatus == 1) )
		{
			tMpcCpuStatus.byCpuStatus = 0;
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_CPU_STATUS, &tMpcCpuStatus,sizeof(tMpcCpuStatus));
			m_byMPCCpuStatus = 0;
		} 
        Agtlog( LOG_VERBOSE,"[ProcGuardStateScan] Cpu Rate:%u, Status:%u!\n", 
        tMpcCpuStatus.byCpuAllocRate, m_byMPCCpuStatus);
    }
	else
	{
		Agtlog( LOG_ERROR, "[OspGetCpuInfo] Error Get Cpu Percentage.\n");
	}

#ifdef _LINUX12_
	// [2011/01/13 miaoqingsong add] MPC2板温度异常告警上报网管
    lm75 tSensor;
	TMPC2TempStatus  tMpc2TempStatus;

    u32 dwReturn = BrdGetSensorStat( &tSensor );
    tMpc2TempStatus.dwMpc2Temp = tSensor.temp;

	if ( OK == dwReturn )
	{
		if ( (tMpc2TempStatus.dwMpc2Temp > 70) && (BOOL32)(m_byMPC2TempStatus == 0) )    // MPC2板温度超过70度
		{
			Agtlog(LOG_WARN,"[ProcGuardStateScan] The Mpc2's temperature is high enough: %d\n", tMpc2TempStatus.dwMpc2Temp);

            tMpc2TempStatus.byMpc2TempStatus = 1;
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_MPC2TEMP_STATUS, &tMpc2TempStatus, sizeof(tMpc2TempStatus));
			m_byMPC2TempStatus = 1;
		}

		if ( (tMpc2TempStatus.dwMpc2Temp <= 70) && (BOOL32)(m_byMPC2TempStatus == 1) )
		{
			tMpc2TempStatus.byMpc2TempStatus = 0;
			OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_MPC2TEMP_STATUS, &tMpc2TempStatus, sizeof(tMpc2TempStatus));
			m_byMPC2TempStatus = 0;
		}
	}
	else
	{
		Agtlog( LOG_ERROR, "[BrdGetSensorStat] Error Get Mpc2 temperature.\n");
	}
	
	// [2010/12/16 miaoqingsong add] 电源板温度异常告警上报网管
	TPowerBrdTempStatus    tPowerBrdTempStatus;
	u8                     byPowerBrdTemp = 0;
	
	u32 dwRet = BrdMPCLedBrdTempCheck( &byPowerBrdTemp );

	if ( OK == dwRet )
	{
		if( 1 == (byPowerBrdTemp & 0x01) )              // byPowerBrdTemp: 0x01位表示左电源状态，0，温度正常；1，温度异常
		{
			if ( m_byPowerTempStatus == 0 )
			{
				tPowerBrdTempStatus.bySlot = POWER_LEFT;
				tPowerBrdTempStatus.byPowerBrdTempStatus = 1;
				OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_POWERTEMP_STATUS, &tPowerBrdTempStatus, sizeof(tPowerBrdTempStatus) );
			    m_byPowerTempStatus = 1;
			}
			
		}
		else
		{
			if ( m_byPowerTempStatus == 1 )
			{
				tPowerBrdTempStatus.bySlot = POWER_LEFT;
				tPowerBrdTempStatus.byPowerBrdTempStatus = 0;
				OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_POWERTEMP_STATUS, &tPowerBrdTempStatus, sizeof(tPowerBrdTempStatus) );
			    m_byPowerTempStatus = 0;
			}
		}

		if ( 2 == (byPowerBrdTemp & 0x02) )              // byPowerBrdTemp: 0x02位表示右电源状态，0，温度正常；1，温度异常
		{
			if ( m_byPowerTempStatus == 0 )
			{
				tPowerBrdTempStatus.bySlot = POWER_RIGHT;
				tPowerBrdTempStatus.byPowerBrdTempStatus = 1;
				OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_POWERTEMP_STATUS, &tPowerBrdTempStatus, sizeof(tPowerBrdTempStatus) );
			    m_byPowerTempStatus = 1;
			}
			
		}
		else
		{
			if ( m_byPowerTempStatus == 1 )
			{
				tPowerBrdTempStatus.bySlot = POWER_RIGHT;
				tPowerBrdTempStatus.byPowerBrdTempStatus = 0;
				OspPost( MAKEIID(AID_MCU_AGENT, 1 ), SVC_AGT_POWERTEMP_STATUS, &tPowerBrdTempStatus, sizeof(tPowerBrdTempStatus) );
			    m_byPowerTempStatus = 0;
			}
		}
	}
	else
	{
		Agtlog( LOG_ERROR, "[BrdMPCLedBrdTempCheck] Error Get Power's board temperature!\n");
	}
#endif // end _LINUX12_
#endif // end !_8KE_
	
	// 重新设置定时器
	SetTimer( MCUAGENT_SCAN_STATE_TIMER, SCAN_STATE_TIME_OUT );

#endif // !(_VXWORKS_ || _LINUX_)

    return;
}

/*lint -restore*/
// END OF FILE
