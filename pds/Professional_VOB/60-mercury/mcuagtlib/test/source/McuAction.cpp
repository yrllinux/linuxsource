
// McuAction.cpp

#include "McuAction.h"
#include "mcuagtstruct.h"
#include "AgentInterface.h" // 接口类
#include "Testevent.h"

CAgentInterface    g_cInterface;
extern SEMHANDLE g_SimuSem;

/*=============================================================================
  函 数 名： GetInfo
  功    能： 从指定App取信息
  算法实现： 
  全局变量： 
  参    数： u16 wAppNum
             u16& wEvent
  返 回 值： BOOL 
=============================================================================*/
BOOL GetInfo(u16 wAppNum, u16& wEvent)
{
    char ackbuf[4096];
	u16 ackbuflen = sizeof(ackbuf);

	if( OSP_OK != OspSend(MAKEIID(wAppNum, 1), GETINFO_FROM_MCU, 0, 0, 0, 
		MAKEIID(INVALID_APP, INVALID_INS), INVALID_NODE,
		ackbuf, ackbuflen))
	{
		return FALSE;
	}
		
	wEvent = *(u16 *)ackbuf;
	return TRUE;
    
}

/*=============================================================================
  函 数 名： PostMsg
  功    能： 发送消息到指定App
  算法实现： 
  全局变量： 
  参    数： u16 wAppNum
             u16& wEvent
  返 回 值： void 
=============================================================================*/
void PostMsg(u16 wAppNum, u16& wEvent)
{
    OspPost(MAKEIID(wAppNum, 1), wEvent);
    return;
}

/*=============================================================================
  函 数 名： GetInfoFromMcu
  功    能： 从模拟Mcu取消息
  算法实现： 
  全局变量： 
  参    数： u16& wEvent
  返 回 值： BOOL 
=============================================================================*/
BOOL CMcuActTest::GetInfoFromMcu(u16& wEvent)
{
    char ackbuf[4096];
	u16 ackbuflen = sizeof(ackbuf);

	if( OSP_OK != OspSend(MAKEIID(APP_SIMUMCUSSN, 1), GETINFO_FROM_MCU, 0, 0, 0, 
		MAKEIID(INVALID_APP, INVALID_INS), INVALID_NODE,
		ackbuf, ackbuflen))
	{
		return FALSE;
	}
		
	wEvent = *(u16 *)ackbuf;
	return TRUE;
}


/*=============================================================================
  函 数 名： PostToMcu
  功    能： 发消息给模拟Mcu
  算法实现： 
  全局变量： 
  参    数： u16 wEvent
  返 回 值： void 
=============================================================================*/
void CMcuActTest::PostToMcu(u16 wEvent)
{
    OspPost(MAKEIID(APP_SIMUMCUSSN, 1), wEvent);
    return;
}

/*=============================================================================
  函 数 名： TestE1Loop
  功    能： 测试E1自环
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestE1Loop()
{
    CServMsg cReport;
    u16 wEvent = 0;
    PostMsg( APP_SIMUMCUSSN, BRDTO_E1LOOP_TEST);
//    OspDelay(1000);
    OspSemTake( g_SimuSem );
    
    GetInfo( APP_SIMUMCUSSN, wEvent);
    CPPUNIT_ASSERT(wEvent = BRDTO_E1LOOP_TEST + 1);
    return;
}

/*=============================================================================
  函 数 名： TestErrorTest
  功    能： 测试Erroe消息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestErrorTest()
{
    CServMsg cReport;
    u16 wEvent = 0;
    PostMsg(APP_SIMUMCUSSN, BRDTO_ERROR_TEST);
//    OspDelay(1000);
    OspSemTake( g_SimuSem );

    GetInfo(APP_SIMUMCUSSN, wEvent);
    CPPUNIT_ASSERT(wEvent == BRDTO_ERROR_TEST+1);
    return;
}

/*=============================================================================
  函 数 名： TestTimeSync
  功    能： 测试时间同步
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestTimeSync()
{
    CServMsg cReport;
    u16 wEvent = 0;
    PostMsg(APP_SIMUMCUSSN, BRDTO_TIMESYNC_TEST);
//    OspDelay(1000);
    OspSemTake( g_SimuSem );

    GetInfo(APP_SIMUMCUSSN, wEvent);
    CPPUNIT_ASSERT(wEvent == BRDTO_TIMESYNC_TEST+1);
    return;
}


/*=============================================================================
  函 数 名： TestGetEqpRecorderCfg
  功    能： 测试取录像机配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestGetEqpRecorderCfg()
{
    TmcueqpRecorderEntry    tMcuRecorderParam;
    u8  byRecId = 17;

    g_cInterface.GetEqpRecorderCfg(byRecId, &tMcuRecorderParam);

    CPPUNIT_ASSERT(tMcuRecorderParam.mcueqpRecorderEntId == 17);
    CPPUNIT_ASSERT(tMcuRecorderParam.mcueqpRecorderEntPort == 60000);
    CPPUNIT_ASSERT(tMcuRecorderParam.mcueqpRecorderEntSwitchBrdId == 2);
    CPPUNIT_ASSERT(0 == strcmp(tMcuRecorderParam.mcueqpRecorderEntAlias, "Recorder1"));
    CPPUNIT_ASSERT(tMcuRecorderParam.mcueqpRecorderEntIpAddr == 0xac10050f); // 172.16.5.15
    CPPUNIT_ASSERT(tMcuRecorderParam.mcueqpRecorderEntRecvStartPort == 60000);

    return;

}

/*=============================================================================
  函 数 名： TestSetRecorderCfg
  功    能： 测试置录像机配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestSetRecorderCfg()
{
    TmcueqpRecorderEntry tMcuRecorderParam;
    TmcueqpRecorderEntry tDstRecorderParam;
    CServMsg cGetMsg;
    u8 byRecId = 17;
    
    tMcuRecorderParam.mcueqpRecorderEntId = 20;
    tMcuRecorderParam.mcueqpRecorderEntPort  = 60000;
    tMcuRecorderParam.mcueqpRecorderEntSwitchBrdId = 2;
    memcpy(tMcuRecorderParam.mcueqpRecorderEntAlias, "Recorder3", sizeof("Recorder3"));
    tMcuRecorderParam.mcueqpRecorderEntIpAddr = 0xac100510; // 172.16.5.16
    tMcuRecorderParam.mcueqpRecorderEntRecvStartPort =60000;

    g_cInterface.SetEqpRecorderCfg(byRecId, &tMcuRecorderParam);
    OspDelay(1000);

    byRecId = 20;
    g_cInterface.GetEqpRecorderCfg(byRecId, &tDstRecorderParam);
    OspDelay(1000);

//    cGetMsg.SetMsgBody(&byRecId, sizeof(u8));
//    PostToMcu(MCU_NMS_SETRECCFG, cGetMsg);
//    OspDelay(1000);
//    
//    cGetMsg.Init();
//    GetInfoFromMcu(cGetMsg);
//    memcpy(&tMcuRecorderParam, cGetMsg.GetServMsg(), sizeof(TmcueqpRecorderEntry));

    CPPUNIT_ASSERT(tDstRecorderParam.mcueqpRecorderEntId == 20);
    CPPUNIT_ASSERT(tDstRecorderParam.mcueqpRecorderEntPort == 60000);
    CPPUNIT_ASSERT(tDstRecorderParam.mcueqpRecorderEntSwitchBrdId == 2);
    CPPUNIT_ASSERT(0 == strcmp(tDstRecorderParam.mcueqpRecorderEntAlias, "Recorder3"));
    CPPUNIT_ASSERT(tDstRecorderParam.mcueqpRecorderEntIpAddr == 0xac100510); // 172.16.5.16
    CPPUNIT_ASSERT(tDstRecorderParam.mcueqpRecorderEntRecvStartPort == 60000);

    return;
}

/*=============================================================================
  函 数 名： TestGetEqpBasCfg
  功    能： 测试取Bas配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestGetEqpBasCfg()
{
    TmcueqpBasEntry     tSrcBasParam;
    
    u8 byBasId = 49;

    g_cInterface.GetEqpBasCfg(byBasId, &tSrcBasParam);
    
    CPPUNIT_ASSERT( tSrcBasParam.mcueqpBasEntPort == 13000);
    CPPUNIT_ASSERT( tSrcBasParam.mcueqpBasEntSwitchBrdId == 2);
    CPPUNIT_ASSERT( tSrcBasParam.mcueqpBasEntRunningBrdId == 6);
    CPPUNIT_ASSERT( tSrcBasParam.mcueqpBasEntRecvStartPort == 42000);
    CPPUNIT_ASSERT( tSrcBasParam.mcueqpBasEntUsedMapCount == 3); // 3个Map
    CPPUNIT_ASSERT( 0 == strcmp(tSrcBasParam.mcueqpBasEntAlias, "Bas1"));

    return;
}

/*=============================================================================
  函 数 名： TestSetBasCfg
  功    能： 测试设置Bas配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestSetBasCfg()
{
    TmcueqpBasEntry     tSrcBasParam;
    TmcueqpBasEntry     tDstBasParam;
    u8 byBasId = 49;
    CServMsg cGetMsg;

    tSrcBasParam.mcueqpBasEntId = 51;
    tSrcBasParam.mcueqpBasEntPort = 13000;
    tSrcBasParam.mcueqpBasEntSwitchBrdId = 2;
    tSrcBasParam.mcueqpBasEntRunningBrdId = 3;
    tSrcBasParam.mcueqpBasEntRecvStartPort = 42000;
    tSrcBasParam.mcueqpBasEntUsedMapCount = 4; // 3个Map
    memcpy(tSrcBasParam.mcueqpBasEntAlias, "Bas3", sizeof("Bas1"));
    
    g_cInterface.SetEqpBasCfg(byBasId, &tSrcBasParam);
    byBasId = 51;
    OspDelay(1000);

    g_cInterface.GetEqpBasCfg(byBasId, &tDstBasParam);
    OspDelay(1000);

//    cGetMsg.SetMsgBody(&byBasId, sizeof(u8));
//    PostToMcu(MCU_NMS_SETBASCFG, cGetMsg);
//    OspDelay(1000);
//
//    cGetMsg.Init();
//    GetInfoFromMcu(cGetMsg);
//    memcpy(&tSrcBasParam, cGetMsg.GetServMsg(), sizeof(TmcueqpBasEntry));
    
    CPPUNIT_ASSERT( tDstBasParam.mcueqpBasEntId == 51);
    CPPUNIT_ASSERT( tDstBasParam.mcueqpBasEntPort == 13000);
    CPPUNIT_ASSERT( tDstBasParam.mcueqpBasEntSwitchBrdId == 2);
    CPPUNIT_ASSERT( tDstBasParam.mcueqpBasEntRunningBrdId == 3);
    CPPUNIT_ASSERT( tDstBasParam.mcueqpBasEntRecvStartPort == 42000);
    CPPUNIT_ASSERT( tDstBasParam.mcueqpBasEntUsedMapCount == 4); // 3个Map
    CPPUNIT_ASSERT( 0 == strcmp(tDstBasParam.mcueqpBasEntAlias, "Bas3"));

    return;
}

/*=============================================================================
  函 数 名： TestGetEqpMixerCfg
  功    能： 测试取Mixer配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestGetEqpMixerCfg()
{
    TmcueqpMixerEntry   tMixerParam;
    u8  byMixId = 1;

    g_cInterface.GetEqpMixerCfg(byMixId, &tMixerParam);

    CPPUNIT_ASSERT( tMixerParam.mcueqpMixerEntPort == 60000);
    CPPUNIT_ASSERT( tMixerParam.mcueqpMixerEntSwitchBrdId == 2);
    CPPUNIT_ASSERT( tMixerParam.mcueqpMixerEntRunningBrdId == 3);
    CPPUNIT_ASSERT( tMixerParam.mcueqpMixerEntMixStartPort == 45000);
    CPPUNIT_ASSERT( tMixerParam.mcueqpMixerEntMaxChannelInGrp == 10);
    CPPUNIT_ASSERT( 0 == strcmp(tMixerParam.mcueqpMixerEntAlias, "Mixer1"));
    return;
}

/*=============================================================================
  函 数 名： TestSetMixerCfg
  功    能： 测试设置Mixer配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestSetMixerCfg()
{
    TmcueqpMixerEntry   tMixerParam;
    TmcueqpMixerEntry   tDstParam;
    u8  byMixId = 1;
    CServMsg cTmpMsg;

    tMixerParam.mcueqpMixerEntId = 3;
    tMixerParam.mcueqpMixerEntPort = 60000;
    tMixerParam.mcueqpMixerEntSwitchBrdId = 3;
    tMixerParam.mcueqpMixerEntRunningBrdId = 2;
    tMixerParam.mcueqpMixerEntMixStartPort = 45001;
    tMixerParam.mcueqpMixerEntMaxChannelInGrp = 11;
    memcpy(tMixerParam.mcueqpMixerEntAlias, "Mixer3", sizeof("Mixer3"));

    g_cInterface.SetEqpMixerCfg(byMixId, &tMixerParam);
    byMixId = 3;
    OspDelay(1000);
    
    g_cInterface.GetEqpMixerCfg(byMixId, &tDstParam);
    OspDelay(1000);

//    cTmpMsg.SetMsgBody(&byMixId, sizeof(u8));
//    PostToMcu(MCU_NMS_SETMIXCFG, cTmpMsg);
//    OspDelay(1000);
//    
//    cTmpMsg.Init();
//    GetInfoFromMcu(cTmpMsg);
//    memcpy(&tMixerParam, cTmpMsg.GetServMsg(), sizeof(TmcueqpMixerEntry));
    
    CPPUNIT_ASSERT( tDstParam.mcueqpMixerEntId == 3);
    CPPUNIT_ASSERT( tDstParam.mcueqpMixerEntPort == 60000);
    CPPUNIT_ASSERT( tDstParam.mcueqpMixerEntSwitchBrdId == 2);
    CPPUNIT_ASSERT( tDstParam.mcueqpMixerEntRunningBrdId == 3);
    CPPUNIT_ASSERT( tDstParam.mcueqpMixerEntMixStartPort == 45000);
    CPPUNIT_ASSERT( tDstParam.mcueqpMixerEntMaxChannelInGrp == 10);
    CPPUNIT_ASSERT( 0 == strcmp(tDstParam.mcueqpMixerEntAlias, "Mixer1"));
    return;

}

/*=============================================================================
  函 数 名： TestGetEqpPrsCfg
  功    能： 测试取Prs配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestGetEqpPrsCfg()
{
    TmcueqpPrs  tPrsInfo;
    
    g_cInterface.GetEqpPrsCfg(&tPrsInfo);

    CPPUNIT_ASSERT( tPrsInfo.mcueqpPrsEntId == 92);
    CPPUNIT_ASSERT( tPrsInfo.mcueqpPrsEntPort == 18000);
    CPPUNIT_ASSERT( tPrsInfo.mcueqpPrsEntSwitchBrdId == 2);
    CPPUNIT_ASSERT( tPrsInfo.mcueqpPrsEntRunningBrdId == 8);
    CPPUNIT_ASSERT( tPrsInfo.mcueqpPrsEntRecvStartPort == 44000);
    CPPUNIT_ASSERT( tPrsInfo.mcueqpPrsFirstTimeSpan == 40);
    CPPUNIT_ASSERT( tPrsInfo.mcueqpPrsSecondTimeSpan == 80);
    CPPUNIT_ASSERT( tPrsInfo.mcueqpPrsThirdTimeSpan == 120);
    CPPUNIT_ASSERT( tPrsInfo.mcueqpPrsRejectTimeSpan == 160);
    CPPUNIT_ASSERT( 0 == strcmp(tPrsInfo.mcueqpPrsEntAlias, "Prs1"));
    return;
}

/*=============================================================================
  函 数 名： TestSetPrsCfg
  功    能： 测试设置Prs配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestSetPrsCfg()
{
    TmcueqpPrs tPrsInfo;
    TmcueqpPrs tDstPrsInfo;

    CServMsg cGetmsg;
    tPrsInfo.mcueqpPrsEntId = 93;
    tPrsInfo.mcueqpPrsEntPort = 18000;
    tPrsInfo.mcueqpPrsEntSwitchBrdId = 3;
    tPrsInfo.mcueqpPrsEntRunningBrdId = 4;
    tPrsInfo.mcueqpPrsEntRecvStartPort = 44000;
    tPrsInfo.mcueqpPrsFirstTimeSpan = 60;
    tPrsInfo.mcueqpPrsSecondTimeSpan = 1000;
    tPrsInfo.mcueqpPrsThirdTimeSpan = 140;
    tPrsInfo.mcueqpPrsRejectTimeSpan = 180;
    memcpy(tPrsInfo.mcueqpPrsEntAlias, "Prs1", sizeof("Prs2"));

    g_cInterface.SetEqpPrsCfg(&tPrsInfo);
    OspDelay(1000);

    g_cInterface.GetEqpPrsCfg( &tDstPrsInfo );
    OspDelay(1000);

//    PostToMcu(MCU_NMS_SETPRSCFG);
//    OspDelay(1000);
//
//    GetInfoFromMcu(cGetmsg);
//    memcpy(&tPrsInfo, cGetmsg.GetServMsg(), sizeof(TmcueqpPrs));

    CPPUNIT_ASSERT( tDstPrsInfo.mcueqpPrsEntId == 93);
    CPPUNIT_ASSERT( tDstPrsInfo.mcueqpPrsEntPort == 18000);
    CPPUNIT_ASSERT( tDstPrsInfo.mcueqpPrsEntSwitchBrdId == 3);
    CPPUNIT_ASSERT( tDstPrsInfo.mcueqpPrsEntRunningBrdId == 4);
    CPPUNIT_ASSERT( tDstPrsInfo.mcueqpPrsEntRecvStartPort == 44000);
    CPPUNIT_ASSERT( tDstPrsInfo.mcueqpPrsFirstTimeSpan == 60);
    CPPUNIT_ASSERT( tDstPrsInfo.mcueqpPrsSecondTimeSpan == 100);
    CPPUNIT_ASSERT( tDstPrsInfo.mcueqpPrsThirdTimeSpan == 140);
    CPPUNIT_ASSERT( tDstPrsInfo.mcueqpPrsRejectTimeSpan == 180);
    CPPUNIT_ASSERT( 0 == strcmp(tDstPrsInfo.mcueqpPrsEntAlias, "Prs2"));
    return;

}

/*=============================================================================
  函 数 名： TestGetEqpTVWallCfg
  功    能： 测试取TvWall配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestGetEqpTVWallCfg()
{
    TmcueqpTVWallEntry  tTvWallInfo;
    u8 byTvId = 33;

    g_cInterface.GetEqpTVWallCfg(byTvId, &tTvWallInfo);

    CPPUNIT_ASSERT( tTvWallInfo.mcueqpTVWallEntId == 33);
    CPPUNIT_ASSERT( tTvWallInfo.mcueqpTVWallEntRunningBrdId == 4);
    CPPUNIT_ASSERT( tTvWallInfo.mcueqpTVWallEntVideoStartRecvPort == 41000);
    CPPUNIT_ASSERT( 0 == strcmp(tTvWallInfo.mcueqpTVWallEntAlias, "TVWall1"));
    return;
}

/*=============================================================================
  函 数 名： TestSetTvWallCfg
  功    能： 测试设置TvWall配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestSetTvWallCfg()
{
    TmcueqpTVWallEntry tTvWallInfo;
    TmcueqpTVWallEntry tDstTvWallInfo;

    u8 byTvId = 33;
    CServMsg cGetMsg;

    tTvWallInfo.mcueqpTVWallEntId = 35;
    tTvWallInfo.mcueqpTVWallEntRunningBrdId = 3;
    tTvWallInfo.mcueqpTVWallEntVideoStartRecvPort = 41000;
    memcpy(tTvWallInfo.mcueqpTVWallEntAlias, "TVWall2", sizeof("TVWall2"));

    byTvId = 35;
    g_cInterface.SetEqpTVWallCfg(byTvId, &tTvWallInfo);
    OspDelay(1000);

    g_cInterface.GetEqpTVWallCfg(byTvId, &tDstTvWallInfo);
    OspDelay(1000);

//    cGetMsg.SetMsgBody(&byTvId, sizeof(u8));
//    PostToMcu(MCU_NMS_SETTVWALLCFG, cGetMsg);
//    OspDelay(1000);
//    
//    cGetMsg.Init();
//    GetInfoFromMcu(cGetMsg);
//    memcpy(&tTvWallInfo, cGetMsg.GetServMsg(), sizeof(TmcueqpTVWallEntry));

    CPPUNIT_ASSERT( tDstTvWallInfo.mcueqpTVWallEntId == 35);
    CPPUNIT_ASSERT( tDstTvWallInfo.mcueqpTVWallEntRunningBrdId == 3);
    CPPUNIT_ASSERT( tDstTvWallInfo.mcueqpTVWallEntVideoStartRecvPort == 41000);
    CPPUNIT_ASSERT( 0 == strcmp(tDstTvWallInfo.mcueqpTVWallEntAlias, "TVWall2"));
    return;
}

/*=============================================================================
  函 数 名： TestGetEqpVMPCfg
  功    能： 测试取Vmp配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestGetEqpVMPCfg()
{
    TmcueqpVMPEntry  tVmpInfo;
    u8 byVmpId = 65;

    g_cInterface.GetEqpVMPCfg(byVmpId, &tVmpInfo);

    CPPUNIT_ASSERT( tVmpInfo.mcueqpVMPEntId == 65);
    CPPUNIT_ASSERT( tVmpInfo.mcueqpVMPEntPort == 60000);
    CPPUNIT_ASSERT( tVmpInfo.mcueqpVMPEntSwitchBrdId == 2);
    CPPUNIT_ASSERT( tVmpInfo.mcueqpVMPEntRunningBrdId == 6);
    CPPUNIT_ASSERT( tVmpInfo.mcueqpVMPEntRecvStartPort == 43000);
    CPPUNIT_ASSERT( tVmpInfo.mcueqpVMPEntEncNumber == 1);
    return;
}

/*=============================================================================
  函 数 名： TestSetVMPCfg
  功    能： 测试设置Vmp配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestSetVMPCfg()
{
    TmcueqpVMPEntry  tVmpInfo;
    TmcueqpVMPEntry  tDstVmpInfo;

    u8 byVmpId = 65;
    CServMsg cGetMsg;

    tVmpInfo.mcueqpVMPEntId = 67;
    tVmpInfo.mcueqpVMPEntPort = 60000;
    tVmpInfo.mcueqpVMPEntSwitchBrdId = 3;
    tVmpInfo.mcueqpVMPEntRunningBrdId = 4;
    tVmpInfo.mcueqpVMPEntRecvStartPort = 43000;
    tVmpInfo.mcueqpVMPEntEncNumber = 2;
    memcpy(tVmpInfo.mcueqpVMPEntAlias, "Vmp3", sizeof("Vmp3"));

    g_cInterface.SetEqpVMPCfg(byVmpId, &tVmpInfo);
    byVmpId = 67;
    OspDelay(1000);

    g_cInterface.GetEqpVMPCfg(byVmpId, &tDstVmpInfo);
    OspDelay(1000);
    
//    cGetMsg.SetMsgBody(&byVmpId);
//    PostToMcu(MCU_NMS_SETVMPCFG, cGetMsg);
//    OspDelay(1000);
//
//    cGetMsg.Init();
//    GetInfoFromMcu(cGetMsg);
//    memcpy(&tVmpInfo, cGetMsg.GetServMsg(), sizeof(TmcueqpVMPEntry));

    CPPUNIT_ASSERT( tDstVmpInfo.mcueqpVMPEntId == 67);
    CPPUNIT_ASSERT( tDstVmpInfo.mcueqpVMPEntSwitchBrdId == 3);
    CPPUNIT_ASSERT( tDstVmpInfo.mcueqpVMPEntRunningBrdId == 4);
    CPPUNIT_ASSERT( tDstVmpInfo.mcueqpVMPEntEncNumber == 2);
    CPPUNIT_ASSERT( 0 == strcmp(tDstVmpInfo.mcueqpVMPEntAlias, "Vmp3"));
    return;

}

/*=============================================================================
  函 数 名： TestGetBrdCfg
  功    能： 测试取单板配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestGetBrdCfg()
{
    TBoardInfo  tBrdInfo;
    u8 byBrdIdx = 1;

    g_cInterface.GetBrdCfg(byBrdIdx, &tBrdInfo);

    CPPUNIT_ASSERT( tBrdInfo.byLayer = 0);
    CPPUNIT_ASSERT( tBrdInfo.bySlot = 0);
    CPPUNIT_ASSERT( tBrdInfo.byType = 1);
    CPPUNIT_ASSERT( tBrdInfo.dwBrdIp = 0xac10050f);
}

/*=============================================================================
  函 数 名： TestSetBrdCfg
  功    能： 测试设置单板配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestSetBrdCfg()
{
    TBoardInfo  tBrdInfo;
    TBoardInfo  tDstBrdInfo;
    u8  byBrdIdx = 1;

    tBrdInfo.byLayer = 1;
    tBrdInfo.bySlot = 1;
    tBrdInfo.byType = 2;
    tBrdInfo.dwBrdIp = 0xac100510;
    tBrdInfo.byPortChoice = 1;

    g_cInterface.SetBrdCfg(byBrdIdx, &tBrdInfo);
    OspDelay(1000);

    g_cInterface.GetBrdCfg(byBrdIdx, &tDstBrdInfo);

    CPPUNIT_ASSERT( tDstBrdInfo.byLayer == tBrdInfo.byLayer);
    CPPUNIT_ASSERT( tDstBrdInfo.bySlot == tBrdInfo.bySlot);
    CPPUNIT_ASSERT( tDstBrdInfo.byType == tBrdInfo.byType);
    CPPUNIT_ASSERT( tDstBrdInfo.dwBrdIp == tBrdInfo.dwBrdIp);
    CPPUNIT_ASSERT( tDstBrdInfo.byPortChoice == tBrdInfo.byPortChoice);
    return;
}

/*=============================================================================
  函 数 名： TestGetQosInfo
  功    能： 测试取Qos配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestGetQosInfo()
{
    TQosInfo    tQosInfo;

    g_cInterface.GetQosInfo(&tQosInfo);
    OspDelay(1000);

    CPPUNIT_ASSERT( tQosInfo.mcuAudioLevel == 4);
    CPPUNIT_ASSERT( tQosInfo.mcuVideoLevel == 3);
    CPPUNIT_ASSERT( tQosInfo.mcuDataLevel == 0);
    CPPUNIT_ASSERT( tQosInfo.mcuSignalLevel == 7);
    CPPUNIT_ASSERT( tQosInfo.mcuQosType == 2);
    CPPUNIT_ASSERT( tQosInfo.mcuIpServiceType == 1);
    return;
}

/*=============================================================================
  函 数 名： TestSetQosInfo
  功    能： 测试设置Qos配置
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestSetQosInfo()
{
    TQosInfo  tQosInfo;
    TQosInfo  tDstQosInfo;

    tQosInfo.mcuAudioLevel = 5;
    tQosInfo.mcuVideoLevel = 4;
    tQosInfo.mcuDataLevel = 1;
    tQosInfo.mcuSignalLevel = 6;
    tQosInfo.mcuQosType = 1;

    g_cInterface.SetQosInfo( &tQosInfo );
    OspDelay(1000);

    g_cInterface.GetQosInfo( &tDstQosInfo );

    CPPUNIT_ASSERT( tQosInfo.mcuAudioLevel == tDstQosInfo.mcuAudioLevel);
    CPPUNIT_ASSERT( tQosInfo.mcuVideoLevel == tDstQosInfo.mcuVideoLevel);
    CPPUNIT_ASSERT( tQosInfo.mcuDataLevel == tDstQosInfo.mcuDataLevel);
    CPPUNIT_ASSERT( tQosInfo.mcuSignalLevel == tDstQosInfo.mcuSignalLevel);
    CPPUNIT_ASSERT( tQosInfo.mcuQosType == tDstQosInfo.mcuQosType);
    return;
}

/*=============================================================================
  函 数 名： TestGetLocalInfo
  功    能： 测试取本地信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestGetLocalInfo()
{
    TLocalInfo  tLocalInfo;

    g_cInterface.GetLocalInfo( &tLocalInfo );
    OspDelay(1000);

    CPPUNIT_ASSERT( tLocalInfo.mcuId == 192);
    CPPUNIT_ASSERT( tLocalInfo.mcuMmcuNeedBAS == 1);
    CPPUNIT_ASSERT( 0 == strcmp(tLocalInfo.mcuE164Number, "123456789"));
    CPPUNIT_ASSERT( 0 == strcmp(tLocalInfo.mcuAlias , "MCU15"));
    return;
}

/*=============================================================================
  函 数 名： TestSetLocalInfo
  功    能： 设置本地信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestSetLocalInfo()
{
    TLocalInfo  tLocalInfo;
    TLocalInfo  tDstLocalInfo;

    tLocalInfo.mcuId = 193;
    tLocalInfo.mcuMmcuNeedBAS = 0;
    memcpy(tLocalInfo.mcuE164Number, "99999999", sizeof("99999999"));
    memcpy(tLocalInfo.mcuAlias, "MCU16", sizeof("MCU16"));

    g_cInterface.SetLocalInfo( &tLocalInfo );
    OspDelay(1000);

    g_cInterface.GetLocalInfo( &tDstLocalInfo );

    CPPUNIT_ASSERT( tLocalInfo.mcuId == tDstLocalInfo.mcuId);
    CPPUNIT_ASSERT( tLocalInfo.mcuMmcuNeedBAS == tDstLocalInfo.mcuMmcuNeedBAS);
    CPPUNIT_ASSERT( 0 == strcmp(tLocalInfo.mcuE164Number, tDstLocalInfo.mcuE164Number));
    CPPUNIT_ASSERT( 0 == strcmp(tLocalInfo.mcuAlias , tDstLocalInfo.mcuAlias));
    return;

}

/*=============================================================================
  函 数 名： TestGetNetWorkInfo
  功    能： 取网络信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestGetNetWorkInfo()
{
    TNetWorkInfo    tNetWorkInfo;

    g_cInterface.GetNetWorkInfo( &tNetWorkInfo );
    OspDelay(1000);

    CPPUNIT_ASSERT( tNetWorkInfo.mcunetGKIpAddr == 0);
    CPPUNIT_ASSERT( tNetWorkInfo.mcunetMulticastIpAddr == 0xff0a0a0a);
    CPPUNIT_ASSERT( tNetWorkInfo.mcunetMulticastPort == 60100);
    CPPUNIT_ASSERT( tNetWorkInfo.mcunetRecvStartPort == 61000);
    CPPUNIT_ASSERT( tNetWorkInfo.mcunet225245StartPort == 60002);
    CPPUNIT_ASSERT( tNetWorkInfo.mcunet225245MtNum == 192);
    CPPUNIT_ASSERT( tNetWorkInfo.mcunetUseMPCTransData == 0);
    CPPUNIT_ASSERT( tNetWorkInfo.mcunetUseMPCStack == 1);
    return;
    
}

/*=============================================================================
  函 数 名： TestSetNetWorkInfo
  功    能： 设置网络信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestSetNetWorkInfo()
{
   TNetWorkInfo    tNetWorkInfo;
   TNetWorkInfo    tDstNetWorkInfo;

   tNetWorkInfo.mcunetGKIpAddr = 0;
   tNetWorkInfo.mcunetMulticastIpAddr = 0xff0a0a0b;
   tNetWorkInfo.mcunetMulticastPort = 60102;
   tNetWorkInfo.mcunetRecvStartPort = 60202;
   tNetWorkInfo.mcunet225245StartPort = 60004;
   tNetWorkInfo.mcunet225245MtNum = 180;
   tNetWorkInfo.mcunetUseMPCTransData = 1;
   tNetWorkInfo.mcunetUseMPCStack = 0;

   g_cInterface.SetNetWorkInfo(&tNetWorkInfo);
   OspDelay(1000);

   g_cInterface.GetNetWorkInfo( &tDstNetWorkInfo );

   CPPUNIT_ASSERT( tNetWorkInfo.mcunetGKIpAddr == tDstNetWorkInfo.mcunetGKIpAddr);
   CPPUNIT_ASSERT( tNetWorkInfo.mcunetMulticastIpAddr == tDstNetWorkInfo.mcunetMulticastIpAddr);
   CPPUNIT_ASSERT( tNetWorkInfo.mcunetMulticastPort == tDstNetWorkInfo.mcunetMulticastPort);
   CPPUNIT_ASSERT( tNetWorkInfo.mcunetRecvStartPort == tDstNetWorkInfo.mcunetRecvStartPort);
   CPPUNIT_ASSERT( tNetWorkInfo.mcunet225245StartPort == tDstNetWorkInfo.mcunet225245StartPort);
   CPPUNIT_ASSERT( tNetWorkInfo.mcunet225245MtNum == tDstNetWorkInfo.mcunet225245MtNum);
   CPPUNIT_ASSERT( tNetWorkInfo.mcunetUseMPCTransData == tDstNetWorkInfo.mcunetUseMPCTransData);
   CPPUNIT_ASSERT( tNetWorkInfo.mcunetUseMPCStack == tDstNetWorkInfo.mcunetUseMPCStack);
   return;
}

/*=============================================================================
  函 数 名： TestGetTrapInfo
  功    能： 取Trap信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestGetTrapInfo()
{
    TTrapInfo tTrapInfo;
    u8  byTrapIdx = 1;

    g_cInterface.GetTrapInfo(byTrapIdx, &tTrapInfo);

    CPPUNIT_ASSERT( tTrapInfo.dwTrapIp == 0x640401fe);
    CPPUNIT_ASSERT( 0 == strcmp(tTrapInfo.achReadCom, "public9"));
    CPPUNIT_ASSERT( 0 == strcmp(tTrapInfo.achWriteCom, "private9"));
    CPPUNIT_ASSERT( tTrapInfo.byGetPort == 161);
    CPPUNIT_ASSERT( tTrapInfo.byTrapPort == 162);
    return;
}

/*=============================================================================
  函 数 名： TestSetTrapInfo
  功    能： 设置Trap信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuActTest::TestSetTrapInfo()
{
    TTrapInfo tTrapInfo;
    TTrapInfo tDstTrapInfo;
    u8 byTrapIdx = 1;
    u16 byFlag1 = 0;
    u16 byFlag2 = 0;

    tTrapInfo.dwTrapIp = 0x640401fe;
    memcpy( tTrapInfo.achReadCom, "public8", sizeof("public8"));
    memcpy( tTrapInfo.achWriteCom, "private8", sizeof("private8"));
    tTrapInfo.byGetPort = 163;
    tTrapInfo.byTrapPort = 164;

    g_cInterface.SetTrapInfo(byTrapIdx, &tTrapInfo);
    OspDelay(1000);

    g_cInterface.GetTrapInfo(byTrapIdx, &tDstTrapInfo);
    
    byFlag1 = strcmp(tTrapInfo.achReadCom, tDstTrapInfo.achReadCom);
    byFlag2 = strcmp(tTrapInfo.achWriteCom , tDstTrapInfo.achWriteCom);

    CPPUNIT_ASSERT( tTrapInfo.dwTrapIp == tDstTrapInfo.dwTrapIp);
    CPPUNIT_ASSERT( tTrapInfo.byGetPort == tDstTrapInfo.byGetPort);
    CPPUNIT_ASSERT( tTrapInfo.byTrapPort == tDstTrapInfo.byTrapPort);
    CPPUNIT_ASSERT( 0 == byFlag1);
    CPPUNIT_ASSERT( 0 == byFlag2);
    return;
}
