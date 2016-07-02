/*==============================================================================
    文件名：abnormitytest.cpp
    
===============================================================================*/
// AbnormityTest.cpp 
#include "Testevent.h"
#include "AbnormityTest.h"
#include "boardaction.h"

/*=============================================================================
  函 数 名： UndefinedAlarmTest
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CAbnormityTest::UndefinedAlarmTest()
{

}

/*=============================================================================
  函 数 名： PostToBrd
  功    能： 发消息到模拟单板 
  算法实现： 
  全局变量： 
  参    数： u16 wEvent
             CServMsg cReportMsg
  返 回 值： void 
=============================================================================*/
void CAbnormityTest::PostToBrd(u16 wEvent, CServMsg cReportMsg)
{
    OspPost(MAKEIID(APP_SIMUBRDSSN, 1), wEvent, cReportMsg.GetMsgBody(), cReportMsg.GetServMsgLen());
    return;
}

/*=============================================================================
  函 数 名： GetInfoFromBrd
  功    能： 从单板取消息
  算法实现： 
  全局变量： 
  参    数： u16 &wEvent
  返 回 值： BOOL 
=============================================================================*/
BOOL CAbnormityTest::GetInfoFromBrd(u16 &wEvent)
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
  函 数 名： HiFrequencyTest
  功    能： 测试频凡发告警到Agent
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CAbnormityTest::HiFrequencyTest()
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

    cReportMsg.SetMsgBody((u8*)abyAlarmBuf, sizeof(sizeof(m_tBoardPosition)+sizeof(u16)+sizeof(tAlarmMsg)));  ()
	
    for(u32 dwLp = 0; dwLp < 100; dwLp++)   
    {
        PostToBrd( SVC_AGT_FILESYSTEM_STATUS, cReportMsg);
        OspDelay(1000);

        GetInfoFromBrd(wEvent);
        CPPUNIT_ASSERT( wEvent == NMS_REV_FILESYSALARM);
    }

    return;

}



