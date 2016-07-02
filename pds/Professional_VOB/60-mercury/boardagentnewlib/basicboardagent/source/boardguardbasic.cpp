/*****************************************************************************
   模块名      : Board Agent Basic
   文件名      : boardguardbasic.cpp
   相关文件    : 
   文件实现功能: 单板守卫基类实现
   作者        : 周广程
   版本        : V4.0  Copyright(C) 2001-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2007/08/20  4.0         周广程       创建
******************************************************************************/
#include "boardguardbasic.h"

CBoardGuardApp	g_cBrdGuardApp;

//构造函数
CBBoardGuard::CBBoardGuard()
{
	memset( &m_tBrdAlarmState, 0, sizeof(m_tBrdAlarmState) );
	memset( &m_tBrdLedState, 0, sizeof(m_tBrdLedState) );
	m_byBrdTempStatus = 0;
	m_byBrdCpuStatus = 0;
	m_byBrdId = 0;

	return;
}

//析构函数
CBBoardGuard::~CBBoardGuard()
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
    2007/08/20  4.0         周广程       创建
====================================================================*/
void CBBoardGuard::InstanceEntry(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
	{
		OspPrintf(TRUE, FALSE, "CBoardGuard: The received msg's pointer in the msg entry is NULL!");
		return;
	}

	switch( pcMsg->event )
	{
	case BOARD_GUARD_POWERON:    //启动消息
		ProcGuardPowerOn( pcMsg );
		break;

	case BRDAGENT_SCAN_STATE_TIMER:		 //扫描定时器到期
		ProcGuardStateScan( pcMsg );
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
    2007/08/20  4.0         周广程       创建
====================================================================*/
void CBBoardGuard::ProcGuardPowerOn(CMessage* const pcMsg)
{
	m_byBrdId = *(u8*)pcMsg->content;
	SetTimer( BRDAGENT_SCAN_STATE_TIMER, SCAN_STATE_TIME_OUT );
	return;
}


/*====================================================================
    函数名      ：ProcGuardStateScan
    功能        ：单板状态的定时扫描
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2007/08/20  4.0         周广程       创建
====================================================================*/
void CBBoardGuard::ProcGuardStateScan(CMessage* const pcMsg)
{
    TBrdAlarmState tBrdAlarmState;
    TBrdLedState tBrdLedState;

	log( LOGLVL_DEBUG2, "Receive Scan Board Message \n");

    if( OK == BrdAlarmStateScan( &tBrdAlarmState ))
	{
		if( memcmp( &tBrdAlarmState, &m_tBrdAlarmState, sizeof(TBrdAlarmState)) != 0 )
		{
			memcpy( &m_tBrdAlarmState, &tBrdAlarmState, sizeof(TBrdAlarmState) );
			post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_MODULE_STATUS, &m_tBrdAlarmState, sizeof(TBrdAlarmState) );
		}
	}
	else
	{
		OspPrintf(TRUE,FALSE,"[ProcGuardStateScan] Error Get board alarm state.srcnode.%d\n",pcMsg!=NULL?pcMsg->srcnode:0);
	}

    // 扫描led状态
    BrdQueryLedState( &tBrdLedState );
	if( memcmp( &tBrdLedState, &m_tBrdLedState, sizeof(TBrdLedState)) != 0 )
	{
		memcpy( &m_tBrdLedState, &tBrdLedState, sizeof(TBrdLedState) );
		post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_LED_STATUS, &m_tBrdLedState, sizeof(TBrdLedState) );
	}


    // 以下板卡类型需上报网口状态，以后会逐步放开。[2012/05/30 liaokang]
    if( m_byBrdId == BRD_TYPE_IS22 )
	{
        // 取前网口：0 的网口状态  
        u8  byEthId = 0;       // 网口
        u8  byLink =0;         // link
        u8  byEthAutoNeg = 0;  // 协商状态
        u8  byEthDuplex = 0;   // 双工状态
        u32 dwEthSpeed = 0;    // 接口速率Mbps
#ifdef _LINUX12_   // 目前仅brdwrapper.h中有TBrdEthInfo
        TBrdEthInfo  tBrdFrontEthPortState;
        // byLink: 0-link down, 1-link up 有链接再调用 BrdGetEthNegStat，否则一直刷错误打印
        BrdGetEthLinkStat( byEthId, &byLink );
        if ( 1 == byLink )
        {
            BrdGetEthNegStat( byEthId, &byEthAutoNeg, &byEthDuplex, &dwEthSpeed);
        }
        tBrdFrontEthPortState.Link = byLink;
        tBrdFrontEthPortState.AutoNeg = byEthAutoNeg;
        tBrdFrontEthPortState.Duplex = byEthDuplex;
        tBrdFrontEthPortState.Speed = dwEthSpeed;
        if( memcmp( &tBrdFrontEthPortState, &m_tBrdFrontEthPortState, sizeof(TBrdEthInfo)) != 0 )
        {
            memcpy( &m_tBrdFrontEthPortState, &tBrdFrontEthPortState, sizeof(TBrdEthInfo) );
            post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_ETHPORT_STATUS, &m_tBrdFrontEthPortState, sizeof(TBrdEthInfo) );
        }
#endif
    }

	/*以下板卡类型,需要上报温度和CPU异常。以后会逐步放开。[10/27/2011 chendaiwei]	*/
	if(m_byBrdId == BRD_TYPE_DRI || 
		m_byBrdId == BRD_TYPE_DRI2 || 
		m_byBrdId == BRD_TYPE_MPU2 ||
		m_byBrdId == BRD_TYPE_MPU2ECARD ||
		m_byBrdId == BRD_TYPE_HDU2 ||
		m_byBrdId == BRD_TYPE_HDU2_L ||
		m_byBrdId == BRD_TYPE_HDU2_S)
	{
	#ifdef _LINUX12_
		tempalarm tSensoralarm;
		memset(&tSensoralarm,0,sizeof(tSensoralarm));
		if ( OK == get_lm75temp_alarm(&tSensoralarm) )
		{
			if ( (tSensoralarm.temp_alarm == BRD_STATUS_ABNORMAL) && (m_byBrdTempStatus == BRD_STATUS_NORMAL) )    
			{
				OspPrintf(TRUE,FALSE,"[ProcGuardStateScan] The board's temperature is high enough\n");
				
				m_byBrdTempStatus = BRD_STATUS_ABNORMAL;
				post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_TEMPERATURE_STATUS_NOTIF, &m_byBrdTempStatus, sizeof(m_byBrdTempStatus) );
			}
			
			if ( (tSensoralarm.temp_alarm == BRD_STATUS_NORMAL) && (m_byBrdTempStatus == BRD_STATUS_ABNORMAL))
			{
				m_byBrdTempStatus = BRD_STATUS_NORMAL;
				post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_TEMPERATURE_STATUS_NOTIF, &m_byBrdTempStatus, sizeof(m_byBrdTempStatus) );
			}
		}
		else
		{
			OspPrintf(TRUE,FALSE,"[ProcGuardStateScan] Error Get board temperature.\n");
		}
	#endif
		
		// 获取CPU占用率信息 [10/25/2011 chendaiwei]
		u8			   byCpuAllocRate = 0;
		TOspCpuInfo    tCpuInfo;
		
		if ( OspGetCpuInfo(&tCpuInfo) )
		{
			byCpuAllocRate = 100 - tCpuInfo.m_byIdlePercent;
			
			if ( byCpuAllocRate > BRD_CPU_THRESHOLD && m_byBrdCpuStatus == BRD_STATUS_NORMAL)    // Cpu占用率超过85%
			{
				OspPrintf(TRUE,FALSE,"[ProcGuardStateScan] The Board's cpu is not enough: %d\n", byCpuAllocRate);
				m_byBrdCpuStatus = BRD_STATUS_ABNORMAL;
				post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_CPU_STATUS_NOTIF, &m_byBrdCpuStatus, sizeof(m_byBrdCpuStatus) );
			}
        
			if ( (byCpuAllocRate <= BRD_CPU_THRESHOLD) && ( m_byBrdCpuStatus == BRD_STATUS_ABNORMAL) )
			{
				m_byBrdCpuStatus = BRD_STATUS_NORMAL;
				post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_CPU_STATUS_NOTIF, &m_byBrdCpuStatus, sizeof(m_byBrdCpuStatus) );
			}       
		}
		else
		{
			OspPrintf(TRUE,FALSE,"[ProcGuardStateScan] Error Get Board Cpu Percentage.\n");
		}	
	}

	//重新设置定时器
	SetTimer( BRDAGENT_SCAN_STATE_TIMER, SCAN_STATE_TIME_OUT );

	return;
}
