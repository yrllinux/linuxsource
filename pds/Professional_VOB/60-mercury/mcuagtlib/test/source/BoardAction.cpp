

// BoardAction.cpp
//#pragma once

#include "boardaction.h"
extern GetInfo(u16 wAppNum, u16& wEvent);

/*=============================================================================
  函 数 名： GetInfoFromBrd
  功    能： 从单板取消息
  算法实现： 
  全局变量： 
  参    数： u16 &wEvent
  返 回 值： BOOL 
=============================================================================*/
BOOL CBoardActTest::GetInfoFromBrd(u16 &wEvent)
{
    char ackbuf[4096];
	u16 ackbuflen = sizeof(ackbuf);

	if( OSP_OK != OspSend(MAKEIID(APP_SIMUBRDSSN, 1), GETINFO_FROM_BRD, 0, 0, 0, 
		MAKEIID(INVALID_APP, INVALID_INS), INVALID_NODE,
		ackbuf, ackbuflen))
	{
		return FALSE;
	}
		
	wEvent = *(u16 *)ackbuf;
	return TRUE;
}

/*=============================================================================
  函 数 名： PostToBrd
  功    能： 发消息到单板
  算法实现： 
  全局变量： 
  参    数： u16 wEvent
             CServMsg cReportMsg
  返 回 值： void 
=============================================================================*/
void CBoardActTest::PostToBrd(u16 wEvent, CServMsg cReportMsg)
{
    OspPost(MAKEIID(APP_SIMUBRDSSN, 1), wEvent, cReportMsg.GetMsgBody(), cReportMsg.GetServMsgLen());
    return;
}

/*=============================================================================
  函 数 名： TestBrdReg
  功    能： 测试单板注册
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CBoardActTest::TestBrdReg()
{
    CServMsg cReportMsg;
    u32 m_dwBrdIp = 0x02020202;  // Ip: 2.2.2.2
    u8  m_byChoice = 1;
    u16 wEvent = 0;

    TBrdPosition    m_tBoardPosition;
    m_tBoardPosition.byBrdID = 5;
    m_tBoardPosition.byBrdLayer = 0;
    m_tBoardPosition.byBrdSlot  = 4;

    cReportMsg.SetMsgBody((u8*)&m_tBoardPosition, sizeof(TBrdPosition));
    cReportMsg.CatMsgBody((u8*)&m_dwBrdIp, sizeof(u32));
    cReportMsg.CatMsgBody(&m_byChoice, sizeof(u8));
    PostToBrd(BOARD_MPC_REG, cReportMsg);
//    OspDelay(1000);
    OspSemTake( g_SimuSem );

    GetInfo(APP_SIMUBRDSSN, wEvent);

    CPPUNIT_ASSERT( wEvent == BOARD_MPC_REG_ACK); // 
    return;
}

/*=============================================================================
  函 数 名： TestBrdGetReg
  功    能： 测试单板取配置信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CBoardActTest::TestBrdGetReg()
{
    CServMsg cReportMsg;
    u16 wEvent = 0;
    TBrdPosition    m_tBoardPosition;
    m_tBoardPosition.byBrdID = 5;
    m_tBoardPosition.byBrdLayer = 0;
    m_tBoardPosition.byBrdSlot  = 4;

    cReportMsg.SetMsgBody((u8*)&m_tBoardPosition, sizeof(TBrdPosition));
    PostToBrd(BOARD_MPC_GET_CONFIG, cReportMsg);
//    OspDelay(1000);
    OspSemTake( g_SimuSem );

    GetInfo(APP_SIMUBRDSSN, wEvent);
	()
    CPPUNIT_ASSERT(wEvent == BOARD_MPC_GET_CONFIG+1);
    return;
}

/*=============================================================================
  函 数 名： TestBrdMemAlarm
  功    能： 测试内存告警
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CBoardActTest::TestBrdMemAlarm()
{
    TBoardAlarmInfo tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];
    CServMsg cReportMsg;
    u16 wEvent = 0;

    TBrdPosition    m_tBoardPosition;
    m_tBoardPosition.byBrdID = 5;
    m_tBoardPosition.byBrdLayer = 0;
    m_tBoardPosition.byBrdSlot  = 4;

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	tAlarmMsg.wEventId = SVC_AGT_MEMORY_STATUS;
	tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
	tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
	tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
	memcpy((void*)tAlarmMsg.abyAlarmContent[3], "Error", sizeof("Error"));
			
	memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
	memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16), &tAlarmMsg, sizeof(tAlarmMsg) );

	*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 
    
    cReportMsg.SetMsgBody((u8*)abyAlarmBuf, sizeof(sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg)));

	PostToBrd( SVC_AGT_MEMORY_STATUS, cReportMsg);
//    OspDelay(1000);
    OspSemTake( g_SimuSem );

    GetInfo(APP_SIMUBRDSSN, wEvent);
    CPPUNIT_ASSERT( wEvent == NMS_REV_MEMALARM);
    return;				
}

/*=============================================================================
  函 数 名： TestBrdFileAlarm
  功    能： 测试文件系统告警
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CBoardActTest::TestBrdFileAlarm()
{
    TBoardAlarmInfo tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];
    u16 wEvent;
    CServMsg cReportMsg;

    TBrdPosition    m_tBoardPosition;
    m_tBoardPosition.byBrdID = 5;
    m_tBoardPosition.byBrdLayer = 0;
    m_tBoardPosition.byBrdSlot  = 4;

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	tAlarmMsg.wEventId = SVC_AGT_FILESYSTEM_STATUS;
	tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
	tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
	tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
	memcpy((void*)tAlarmMsg.abyAlarmContent[3], "Error", sizeof("Error"));
			
	memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
	memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16), &tAlarmMsg, sizeof(tAlarmMsg) );

	*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

    cReportMsg.SetMsgBody((u8*)abyAlarmBuf, sizeof(sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg)));
	PostToBrd( SVC_AGT_FILESYSTEM_STATUS, cReportMsg);
    OspSemTake( g_SimuSem );

    GetInfo(APP_SIMUBRDSSN, wEvent);
    CPPUNIT_ASSERT( wEvent == NMS_REV_FILESYSALARM);
    return;
}

/*=============================================================================
  函 数 名： TestBrdModuleAlarm
  功    能： 测试模块告警
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CBoardActTest::TestBrdModuleAlarm()
{
    TBoardAlarmInfo tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];
    u16 wEvent;
    CServMsg cReportMsg;

    TBrdPosition    m_tBoardPosition;
    m_tBoardPosition.byBrdID = 5;
    m_tBoardPosition.byBrdLayer = 0;
    m_tBoardPosition.byBrdSlot  = 4;


	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	tAlarmMsg.wEventId = BOARD_MODULE_STATUS;
	tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
	tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
	tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
	memcpy((void*)tAlarmMsg.abyAlarmContent[3], "Error", sizeof("Error"));
			
	memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
	memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16), &tAlarmMsg, sizeof(tAlarmMsg) );

	*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

    cReportMsg.SetMsgBody((u8*)abyAlarmBuf, sizeof(sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg)));
	PostToBrd( BOARD_MODULE_STATUS, cReportMsg);
    OspSemTake( g_SimuSem );
    
    GetInfo(APP_SIMUBRDSSN, wEvent);
    CPPUNIT_ASSERT( wEvent == NMS_REV_MODULEALARM);
    return;
}

/*=============================================================================
  函 数 名： TestBrdLedAlarm
  功    能： 测试Led告警
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CBoardActTest::TestBrdLedAlarm()
{
    TBoardAlarmInfo tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];
    u16 wEvent;
    CServMsg cReportMsg;

    TBrdPosition    m_tBoardPosition;
    m_tBoardPosition.byBrdID = 5;
    m_tBoardPosition.byBrdLayer = 0;
    m_tBoardPosition.byBrdSlot  = 4;


	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );

	tAlarmMsg.wEventId = BOARD_LED_STATUS;
	tAlarmMsg.abyAlarmContent[0] = m_tBoardPosition.byBrdLayer;
	tAlarmMsg.abyAlarmContent[1] = m_tBoardPosition.byBrdSlot;
	tAlarmMsg.abyAlarmContent[2] = m_tBoardPosition.byBrdID;
	memcpy((void*)tAlarmMsg.abyAlarmContent[3], "Error", sizeof("Error"));
			
	memcpy(abyAlarmBuf, &m_tBoardPosition,sizeof(m_tBoardPosition) );
				
	memcpy(abyAlarmBuf+sizeof(m_tBoardPosition)+sizeof(u16), &tAlarmMsg, sizeof(tAlarmMsg) );

	*(u16*)( abyAlarmBuf + sizeof(m_tBoardPosition) ) = htons(1); 

    cReportMsg.SetMsgBody((u8*)abyAlarmBuf, sizeof(sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg)));
	PostToBrd( BOARD_LED_STATUS, cReportMsg);
    OspDelay(1000);

    GetInfo(APP_SIMUBRDSSN, wEvent);
    CPPUNIT_ASSERT( wEvent == NMS_REV_LEDALARM);
    return;
}