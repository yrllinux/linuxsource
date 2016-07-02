/*****************************************************************************
   模块名      : Board Agent
   文件名      : dsiguard.cpp
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

#include "dsiguard.h"
#include "evagtsvc.h"
#include "mcuconst.h"
#include "dsiagent.h"


CBoardGuardApp	g_cBrdGuardApp;   //单板的守卫应用实例

//构造函数
CBoardGuard::CBoardGuard()
{
	memset(m_byE1Alarm, 0, sizeof(m_byE1Alarm));
#ifndef WIN32
	memset(&m_tLedState, 0, sizeof(m_tLedState));
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

    04/11/11    3.5         李 博          新接口的修改
====================================================================*/
void CBoardGuard::ProcGuardStateScan(CMessage* const pcMsg)
{
#ifndef WIN32
	u8 byLoop;
    TBrdDSIAlarmAll * ptBrdE1Alarm;
	u8 byOldE1Alarm;
	u8 byBuf[2];
	TBrdDSILedStateDesc * ptLedState;

    TBrdAlarmState tBrdAlarmState;
    TBrdLedState tBrdLedState;

	log( LOGLVL_DEBUG2, "Receive Scan Board Message \n");

    BrdAlarmStateScan( &tBrdAlarmState );
    ptBrdE1Alarm = &tBrdAlarmState.nlunion.tBrdDSIAlarmAll;

	for( byLoop=0; byLoop<4; byLoop++)
	{
		byOldE1Alarm = m_byE1Alarm[byLoop];
		
		//E1失同步
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RLOS != ((m_byE1Alarm[byLoop] & 0x01)==0x01) )
			m_byE1Alarm[byLoop] ^= 0x01;
		//E1失载波
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RCL != ((m_byE1Alarm[byLoop] & 0x02)==0x02) )
			m_byE1Alarm[byLoop] ^= 0x02;
		//E1远端告警
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RRA != ((m_byE1Alarm[byLoop] & 0x04)==0x04) )
			m_byE1Alarm[byLoop] ^= 0x04;
		//E1全1
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RUA1 != ((m_byE1Alarm[byLoop] & 0x08)==0x08) )
			m_byE1Alarm[byLoop] ^= 0x08;
		//CRC复帧错误
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RCMF != ((m_byE1Alarm[byLoop] & 0x10)==0x10) )
			m_byE1Alarm[byLoop] ^= 0x10;
		//随路复帧错误
		if( ptBrdE1Alarm->tBrdE1AlarmDesc[byLoop].bAlarmE1RMF != ((m_byE1Alarm[byLoop] & 0x20)==0x20) )
			m_byE1Alarm[byLoop] ^= 0x20;
		
		if( byOldE1Alarm != m_byE1Alarm[byLoop] )
		{
			byBuf[0] = byLoop;
			byBuf[1] = m_byE1Alarm[byLoop];
			post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_LINK_STATUS, byBuf, sizeof(byBuf) );
		}

        BrdQueryLedState( &tBrdLedState );
        ptLedState = &tBrdLedState.nlunion.tBrdDSILedState;
		if( memcmp( ptLedState, &m_tLedState, sizeof(TBrdDRILedStateDesc)) != 0 )
		{
			memcpy( &m_tLedState, ptLedState, sizeof(TBrdDRILedStateDesc) );
			post( MAKEIID(AID_MCU_BRDAGENT, 1), BOARD_LED_STATUS, &m_tLedState, sizeof(TBrdDRILedStateDesc) );
		}
	}
#endif	
	//重新设置定时器
	SetTimer( BRDAGENT_SCAN_STATE_TIMER, SCAN_STATE_TIME_OUT );

	return;
}

