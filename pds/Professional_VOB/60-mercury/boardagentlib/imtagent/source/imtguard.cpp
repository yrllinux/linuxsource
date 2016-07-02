/*****************************************************************************
   模块名      : Board Agent
   文件名      : imtguard.cpp
   相关文件    : 
   文件实现功能: 单板状态监控
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/25  1.0         jianghy       创建
   2004/12/05  3.5         李 博        新接口的修改
******************************************************************************/

#include "imtguard.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "imtagent.h"

CBoardGuardApp	g_cBrdGuardApp;   //单板的守卫应用实例

//构造函数
CBoardGuard::CBoardGuard()
{

#ifndef WIN32
#ifdef IMT
	memset( &m_tLedState, 0, sizeof(m_tLedState) );
	memset( &m_tImtBrdAlarm, 0, sizeof(m_tImtBrdAlarm) );
#endif
#endif

	return;
}

//析构函数
CBoardGuard::~CBoardGuard()
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
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CBoardGuard::InstanceEntry(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
	{
		log(LOGLVL_EXCEPTION, "CBoardGuard: The received msg's pointer in the msg entry is NULL!");
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
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CBoardGuard::ProcGuardPowerOn(CMessage* const pcMsg)
{
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
    03/08/19    1.0         jianghy       创建
    04/12/05    3.5         李 博        新接口的修改
====================================================================*/
void CBoardGuard::ProcGuardStateScan(CMessage* const pcMsg)
{
#ifndef WIN32
#ifdef IMT
	/*风扇告警*/
	//TBrdIMTAlarmAll tBrdAlarm;
    TBrdIMTAlarmAll * ptBrdAlarm;
	u8 byBuf[2];
	
    TBrdAlarmState tBrdAlarmState;

	//BrdIMTAlarmStateScan( &tBrdAlarm );
    BrdAlarmStateScan( &tBrdAlarmState );
    ptBrdAlarm = &tBrdAlarmState.nlunion.tBrdIMTAlarmAll;

	if( ptBrdAlarm->bAlarmDSP1FanStop != m_tImtBrdAlarm.bAlarmDSP1FanStop )
	{
		m_tImtBrdAlarm.bAlarmDSP1FanStop = ptBrdAlarm->bAlarmDSP1FanStop;

		byBuf[0] = 0;
		if( m_tImtBrdAlarm.bAlarmDSP1FanStop == TRUE )
		{
			byBuf[1] = 1;
		}
		else
		{
			byBuf[1] = 0;
		}
		post( MAKEIID(AID_MCU_BRDAGENT, 1), MCU_BRD_FAN_STATUS, byBuf, sizeof(byBuf) );
	}

	if( ptBrdAlarm->bAlarmDSP2FanStop != m_tImtBrdAlarm.bAlarmDSP2FanStop )
	{
		m_tImtBrdAlarm.bAlarmDSP2FanStop = ptBrdAlarm->bAlarmDSP2FanStop;

		byBuf[0] = 1;
		if( m_tImtBrdAlarm.bAlarmDSP2FanStop == TRUE )
		{
			byBuf[1] = 1;
		}
		else
		{
			byBuf[1] = 0;
		}
		post( MAKEIID(AID_MCU_BRDAGENT, 1), MCU_BRD_FAN_STATUS, byBuf, sizeof(byBuf) );
	}
	
	if( ptBrdAlarm->bAlarmDSP3FanStop != m_tImtBrdAlarm.bAlarmDSP3FanStop )
	{
		m_tImtBrdAlarm.bAlarmDSP3FanStop = ptBrdAlarm->bAlarmDSP3FanStop;

		byBuf[0] = 1;
		if( m_tImtBrdAlarm.bAlarmDSP3FanStop == TRUE )
		{
			byBuf[1] = 1;
		}
		else
		{
			byBuf[1] = 0;
		}
		post( MAKEIID(AID_MCU_BRDAGENT, 1), MCU_BRD_FAN_STATUS, byBuf, sizeof(byBuf) );
	}
	
	/*灯状态*/    
    TBrdLedState tBrdLedState;
	TBrdIMTLedStateDesc * ptLedState;

	//BrdIMTQueryLedState( &tLedState );
    BrdQueryLedState( &tBrdLedState );
    ptLedState = &tBrdLedState.nlunion.tBrdIMTLedState;
	if( memcmp( ptLedState, &m_tLedState, sizeof(TBrdIMTLedStateDesc)) != 0 )
	{
		memcpy( &m_tLedState, ptLedState, sizeof(TBrdIMTLedStateDesc) );
		post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_LED_STATUS, &m_tLedState, sizeof(TBrdIMTLedStateDesc) );
	}
#endif
#endif	
	//重新设置定时器
	SetTimer( BRDAGENT_SCAN_STATE_TIMER, SCAN_STATE_TIME_OUT );

	return;
}




