/*****************************************************************************
   模块名      : mtadp
   文件名      : mtadp.cpp
   相关文件    : mtadp.h, evmcumt.h
   文件实现功能: MCU业务消息和H323消息的转化
   作者        : Tan Guang
   版本        : V1.0  Copyright(C) 2003-2004 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/11    1.0         Tan Guang   创建
******************************************************************************/

/******************************************************************************
 
   This libaray module involves 2 tasks/threads: mtadp and tRecvDataTask. 

   1. The mtadp is the core thread, taking care of interactions with MCU vc
   modules and network events. It uses a high rate timer to periodically
   call seliSelect() to pick events from the wire through the Radvision 
   stack. Theses events are fed to mtadp in form of stack callbacks. 
   An illustrative call flow is:

                      |->cmEvCallStateChanged()  
   mtadp->seliSelect()|->........................->h323adapterCallbacks->mtadp
                      |->cmEvChannelStateChanged()

   The single task structure greatly simplify the implementation. All cmXXX
   can be safely called within the main task without concerning all potential
   races. Big locks for stack are avoided and thread-safe functions are not 
   needed anywhere. 

   2. tRecvDataTask is spawned by kdv323adp.lib in its initialization process.
   It's sole function is to receive h.224 fecc packets from remote entities.
   It could also be incorperated in the mtadp thread using the RTP/RTCP 
   interfaces provided by the stack.

*******************************************************************************/

/*lint -save -e765*/ 
#include "osp.h"
#include "cm.h"
#include "mcustruct.h"
#include "mcuver.h"
#include "evmcumt.h"
#include "evmcu.h"
#include "evmcuvcs.h"
//#include "mcuerrcode.h"
//#include "vccommon.h"
//#include "bindmp.h"
#include "mtadp.h"
//#include "mcuconst.h"
#include "bindmtadp.h"
#include "mtadputils.h"
//#include "h323adapter.h"
#include "kdvdatanet.h"
//#include "mastructv4r4b2.h"
#include "h460pinhole.h"
#include "evmcueqp.h"
#include "evvrsterminal.h"
#include "vrsterminaldef.h"

#ifdef _LINUX_
#include "bindwatchdog.h"
#endif

#if defined _LINUX_ && defined _LINUX12_ 
#include "brdwrapper.h"
//#include "brdwrapperdef.h"
#endif

CMtAdpApp	g_cMtAdpApp;

//function wrapper

//send conf control message after h245 connection has been connected
#define CONF_CTRL( ctrlType, buf, bufLen ) \
do{\
	if(kdvSendConfCtrlMsg(m_hsCall, (ctrlType), (buf), (bufLen)) == (s32)act_err)\
	        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "%s failed.\n", #ctrlType);\
}while(0)

//send conf control message after h245 connected and TCS & MSD completed
//Notice: all conferece msg other than TCS & MSD should be sent after 
//TCS&MSD, otherwise the stack may be not able to handle them properly and
//results in Rpool exhaust, and the system would crashes!
//Todo: replace this strange macro with one particular state to the instance's 
//state machine

#define CONF_CTRL_STRICT( ctrlType, buf, bufLen ) \
do{\
	if(m_bHasJoinedConf) \
	{\
		if(kdvSendConfCtrlMsg(m_hsCall, (ctrlType), (buf), (bufLen)) == (s32)act_err)\
			    MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "%s failed.\n", #ctrlType);\
	}\
}while(0)

/*lint -save -e715*/
/*=============================================================================
  函 数 名： CallOutByTransportAddr
  功    能： 根据IP地址呼叫终端 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/4        4.0         顾振华                创建
=============================================================================*/
static s8* GetLevelStr(u8 byLevel)
{
    static s8 g_strLevel[8] = {0};
    switch (byLevel)
    {
    case 1:
        strcpy(g_strLevel, ".一级");
        break;
    case 2:
        strcpy(g_strLevel, ".二级");
        break;
    case 3:
        strcpy(g_strLevel, ".三级");
        break;
    case 4:
        strcpy(g_strLevel, ".四级");
        break;
    default:
        strcpy(g_strLevel, ".四级");
        break;
    }
    return g_strLevel;
}

static u8 GetLevelByStr(const s8* szLevelStr)
{
    u8 byLen = strlen(szLevelStr);
    u8 byDstAdminLevel = 4;
    
    if ( byLen > 5 )
    {                    
        if ( strstr(szLevelStr, ".一级") != NULL )
        {
            byDstAdminLevel = 1;
        }
        else if ( strstr(szLevelStr, ".二级") != NULL )
        {
            byDstAdminLevel = 2;
        }
        else if ( strstr(szLevelStr, ".三级") != NULL )
        {
            byDstAdminLevel = 3;
        }
        else if ( strstr(szLevelStr, ".四级") != NULL )
        {
            byDstAdminLevel = 4;
        }                    
        else
        {
            byDstAdminLevel = 4;
        }
    }
    return byDstAdminLevel;
}

/*----------------------------------------------------------------------*/

static s32	   g_nPrtdid	= -1;
static u32    g_dwSendMMcu	=  1;
u32	   g_dwVidInfoTimeout	   = VIDINFO_REFRESHTIME  * 1000;
u32    g_dwAudInfoTimeout	   = AUDINFO_REFRESHTIME  * 1000;
u32    g_dwPartlistInfoTimeout = PARTLIST_REFRESHTIME * 1000;

//.............................................

CMtAdpInst::CMtAdpInst()
{	
	m_dwLastState = STATE_IDLE;
	m_hsCall      = NULL;	
	m_byDirect    = (u8)CALL_INVALID;
	m_wCallRate   = 0;
	m_wAppliedCallRate    = 0;
	m_byFirstVideoChanIdx = 0;
	m_byMtId = 0;
	m_cConfId.SetNull();
	m_byConfIdx  = 0;
	m_dwMtIpAddr = 0;
	m_wMtPort    = MCU_Q931_PORT;
	m_byVendorId = 0;
	m_bH245Connected = FALSE;
	m_bHasJoinedConf = FALSE;
	m_hsRas          = NULL;
	m_nIrrFrequency  = 0;
	memset(&m_tTerminalLabel, 0, sizeof(m_tTerminalLabel));
	m_byTimeoutTimes = 0;
	//m_bEverGotRTDResponse = FALSE;
	//默认所有厂商均支持roundtrip检测
	m_bEverGotRTDResponse = TRUE;
	m_wSavedEvent      = 0;
	m_bIsRecvDRQFromGK = FALSE;
	m_bIsRecvARJFromGK = FALSE;
	m_ptRemoteCapSet   = new TCapSet;

	m_byEncrypt = 0;
	m_bMaster = FALSE;
    m_bMsdOK = FALSE;
    m_byMsdFailTimes = 0;
	m_byMtVer = (u8)emMtVerBegin;
//	m_pbyNonStandConfInfoData  = new u8[sizeof(TConfInfo)];
//    memset( m_pbyNonStandConfInfoData, 0, sizeof(TConfInfo) );
//	m_nNonStandConfInfoDataLen = 0;
//	m_pbyNonStandConfInfoData36  = new u8[sizeof(TConfInfoVer36)];
//    memset( m_pbyNonStandConfInfoData36, 0, sizeof(TConfInfoVer36) );
//	m_nNonStandConfInfoDataLen36 = 0;
	memset(m_atPartIdTable, 0, sizeof(m_atPartIdTable));
	memset(&m_tMtInfo, 0, sizeof(m_tMtInfo));
	m_dwPartIdNum  = 0;
	m_dwMMcuReqId  = 14828708;//OspTickGet()&0x00ffffff;
	m_dwPeerReqId  = 0;
	m_nPasswordLen = 0;
	memset(m_achConfPwd, 0, sizeof(m_achConfPwd));

	m_wForceCallInfo = 0;

	memset(m_achtMtAlias, 0, sizeof(m_achtMtAlias));
	
	m_emEndpointType = emEndpointTypeUnknown;

    //[4/8/2013 liaokang] 编码方式，默认为GBK
    m_emEndpointEncoding = emenCoding_GBK;

	memset(m_abyPassword, 0, sizeof(m_abyPassword));

	m_byAdminLevel = 0;
	//m_byAudioTrackNum = 0;
	
	m_tLocalCapSupport.Clear();
	m_tCommonCap.Clear();
	m_tLocalCapEx.Clear();
	m_tCommonCapEx.Clear();
	
	for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++)
	{
		m_atMainStreamCapEX[byIdx].Clear();
		m_atDoubleStreamCapEX[byIdx].Clear();
	}
   
    //穿越所用相关信息
    m_bIsNeedPinHold = FALSE;
	memset(&m_atAudSndRtpAddr, 0, sizeof(m_atAudSndRtpAddr));
    memset(&m_atVidSndRtpAddr, 0, sizeof(m_atVidSndRtpAddr));
    memset(&m_atSecSndRtpAddr, 0, sizeof(m_atSecSndRtpAddr));
}

CMtAdpInst::~CMtAdpInst()
{	
    // 在这里Clear有风险，可能会导致退出时异常
	//ClearInst();
	if( m_ptRemoteCapSet )
	{
		delete m_ptRemoteCapSet;
		m_ptRemoteCapSet = NULL;
	}
//     if( m_pbyNonStandConfInfoData != NULL )
//     {
//         delete []m_pbyNonStandConfInfoData; 
//         m_pbyNonStandConfInfoData = NULL;
//     }
// 	if( m_pbyNonStandConfInfoData36 != NULL )
// 	{
// 		delete []m_pbyNonStandConfInfoData36;
// 		m_pbyNonStandConfInfoData36 = NULL;
// 	}

}

/*=============================================================================
  函 数 名： ClearInst
  功    能： 控制台断链通知处理函数
  算法实现： (1)清除定时器
			 (2)发送断链报告(如有需要)
			 (3)清除实例变量
			 (4)将实例状态转变为IDLE
  全局变量： 
  参    数：  u16 wReportEvent
              u8 byCause     MtAdp 内部的错误宏，emDisconnectReason
              u8 byMsgRemote 是否通知对端原因
  返 回 值：  void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0			TanGuang                创建
=============================================================================*/
void CMtAdpInst::ClearInst( u16 wReportEvent, u8 byMtadpCause, u8 byMsgRemote )
{
    //a barrier for function re-entrance
	if( CurState() == STATE_IDLE )
    {
        // zbq [06/13/2007] but some member's value has to be cleared just now
        ClearInstMtFromQueue();
		g_cMtAdpApp.m_Msg2InstMap[m_byConfIdx][m_byMtId] = 0; // clear mapping

        m_tMtAlias.SetNull();
        m_wCallRate  = 0;
        m_byMtId     = 0;
        m_byConfIdx  = 0;
        m_dwMtIpAddr = 0;
        m_byDirect   = (u8)CALL_INVALID;

        m_byVendorId = 0;
        m_wMtPort    = MCU_Q931_PORT;
        m_hsRas      = NULL;	
        m_byMtVer    = (u8)emMtVerBegin;

		m_tRecSrcMt.SetNull();
		m_byRecType = TRecChnnlStatus::TYPE_UNUSE;

//        m_nNonStandConfInfoDataLen = 0;
		
		// [11/21/2011 liuxu] 缓冲发送MtList
		m_bySMcuMtListBufTimerStart = 0;
		m_cSMcuMtListSendBuf.ClearAll();
		KillTimer(TIMER_SEND_SMCU_MTLIST);
		return;
    }

	CHANGE_STATE( STATE_IDLE ); 

	// xliang [1/4/2009] 处理接入mcu或mt列表信息.越早处理越好
	if( g_cMtAdpApp.IsInConnMcuList(GetInsID()) )
	{
		g_cMtAdpApp.m_wCurNumConntMt -= 2;
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "m_wCurNumConntMt -2! Now CurNumConntMt is %d\n",g_cMtAdpApp.m_wCurNumConntMt);
		
	}
	else if(g_cMtAdpApp.IsInConnMtList(GetInsID()))
	{
		g_cMtAdpApp.m_wCurNumConntMt -= 1;
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "m_wCurNumConntMt -1! Now CurNumConntMt is %d\n",g_cMtAdpApp.m_wCurNumConntMt);
	}
	g_cMtAdpApp.ClearConnMcuMtList(GetInsID());

	KillTimer( TIMER_IRR );
	KillTimer( WAIT_PASSWORD_TIMEOUT );
	KillTimer( WAIT_RESPONSE_TIMEOUT );
	KillTimer( TIMER_ROUNDTRIPDELAY_REQ );
	KillTimer( TIMER_MTLIST );
	KillTimer( TIMER_VIDEOINFO );
	KillTimer( TIMER_AUDIOINFO );

	if( wReportEvent )
	{
        u8 byCauseMcu = CMtAdpUtils::DisconnectReasonMtAdp2Vc((emDisconnectReason)byMtadpCause);
		BuildAndSendMsgToMcu( wReportEvent, &byCauseMcu, sizeof(byCauseMcu) );
	}
	g_cMtAdpApp.m_Msg2InstMap[m_byConfIdx][m_byMtId] = 0; // clear mapping

	//clear a fully active call & its associated channels
	if( NULL != m_hsCall )
	{
		if( g_cMtAdpApp.m_tH323Config.IsManualRAS() && g_cMtAdpApp.m_bGkAddrSet ) 
		{
			//没有收到drq arj，发送DRQ
			if( !(m_bIsRecvDRQFromGK == TRUE || m_bIsRecvARJFromGK == TRUE) )
            {
                DaemonProcDisengageOnGK();
            }
		}

		for( s32 nCallChanNum = 0; nCallChanNum < MAXNUM_CALL_CHANNEL; nCallChanNum ++ )
		{		
			if( m_atChannel[nCallChanNum].hsChan != NULL )
			{
				// [2013/05/28 chenbing] 对H224数据信道后向打洞的进行补洞
				if ( MODE_DATA == m_atChannel[nCallChanNum].tLogicChan.GetMediaType()
					&& MEDIA_TYPE_H224 == m_atChannel[nCallChanNum].tLogicChan.GetChannelType()
					&& (u8)CHAN_INCOMING == m_atChannel[nCallChanNum].byDirect
					&& m_bIsNeedPinHold
					)
				{
					BOOL32 bRet = KdvTSDelPinhole(m_atChannel[nCallChanNum].tLogicChan.m_tRcvMediaChannel.GetPort());
					MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ClearInst]:h_chan_close---H460---KdvTSDelPinHole(return: %d) Src(%u.%u.%u.%u:%d) MediaType(%d)!!!\n",
							bRet, QUADADDR(m_atChannel[nCallChanNum].tLogicChan.m_tRcvMediaChannel.m_dwIpAddr),
							m_atChannel[nCallChanNum].tLogicChan.GetChannelType(), m_atChannel[nCallChanNum].tLogicChan.m_tRcvMediaChannel.GetPort());
				}
			}			
			m_atChannel[nCallChanNum].Clear();
		}

        if ( 1 == byMsgRemote )
        {
            CallDisconnectReason emReasonStack = CMtAdpUtils::DisconnectReasonMtAdp2Stack((emDisconnectReason)byMtadpCause);
            u8 byReason = (u8)emReasonStack;
            kdvSendCallCtrlMsg( m_hsCall, (u16)h_call_drop, &byReason, sizeof(byReason) );		
        }
        else
        {
            kdvSendCallCtrlMsg( m_hsCall, (u16)h_call_drop, NULL, 0 );		
        }
		m_hsCall  = NULL;
	}

    // zbq [06/13/2007] 本接入主动呼出，须清队列
    ClearInstMtFromQueue();

    m_tMtAlias.SetNull();
	m_dwLastState = STATE_IDLE;
	m_byDirect    = (u8)CALL_INVALID;
	m_wCallRate   = 0;
	m_wAppliedCallRate    = 0;
	m_byFirstVideoChanIdx = 0;
	m_byMtId     = 0;
	m_cConfId.SetNull();
	m_byConfIdx  = 0;
	m_dwMtIpAddr = 0;
	m_wMtPort    = MCU_Q931_PORT;
	m_byVendorId = 0;
	m_bH245Connected = FALSE;
	m_bHasJoinedConf = FALSE;	
	m_hsRas = NULL;	
	if( 0 != m_nIrrFrequency )
	{
		KillTimer( TIMER_IRR );
		m_nIrrFrequency = 0;
	}

	memset( &m_tTerminalLabel, 0, sizeof(m_tTerminalLabel) );	
	m_byTimeoutTimes = 0;
	
	//m_bEverGotRTDResponse = FALSE;
	//默认所有厂商均支持roundtrip检测
	m_bEverGotRTDResponse = TRUE;
	m_wSavedEvent         = 0;
	
	m_bIsRecvDRQFromGK = FALSE;
	m_bIsRecvARJFromGK = FALSE;

	m_byEncrypt = 0;
	m_bMaster   = FALSE;
    m_bMsdOK = FALSE;
    m_byMsdFailTimes = 0;
	m_byMtVer = (u8)emMtVerBegin;
	
//	m_nNonStandConfInfoDataLen = 0;		
	memset( m_atPartIdTable, 0, sizeof(m_atPartIdTable) );
	memset( &m_tMtInfo, 0, sizeof(m_tMtInfo) );
	memset(&m_tLocalCapSupport,   0, sizeof(m_tLocalCapSupport) );
	m_dwPartIdNum  = 0;
	m_dwMMcuReqId  = 14828708;//OspTickGet()&0x00ffffff;
	m_dwPeerReqId  = 0;
	m_nPasswordLen = 0;
	memset(m_achConfPwd, 0, sizeof(m_achConfPwd));
	
	// [11/21/2011 liuxu] 缓冲发送MtList
	m_bySMcuMtListBufTimerStart = 0;
	m_cSMcuMtListSendBuf.ClearAll();
	KillTimer(TIMER_SEND_SMCU_MTLIST);
	
	m_tLocalCapSupport.Clear();
	m_tCommonCap.Clear();
	m_tLocalCapEx.Clear();
	m_tCommonCapEx.Clear();

	for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++)
	{
		m_atMainStreamCapEX[byIdx].Clear();
		m_atDoubleStreamCapEX[byIdx].Clear();
	}

    m_bIsNeedPinHold = FALSE;
    memset(&m_atAudSndRtpAddr, 0, sizeof(m_atAudSndRtpAddr));
    memset(&m_atVidSndRtpAddr, 0, sizeof(m_atVidSndRtpAddr));
    memset(&m_atSecSndRtpAddr, 0, sizeof(m_atSecSndRtpAddr));

	//KillTimer(TIMER_H245_CAPBILITY_NOTIFY_OVERTIME);

    //[4/8/2013 liaokang] 编码方式，默认为GBK
    m_emEndpointEncoding = emenCoding_GBK;

	g_cMtAdpApp.m_tAllSMcuList.DelMcuList((u8)GetInsID());

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Instance %u cleared.\n\n", GetInsID() );
	
	return;
}

/*=============================================================================
  函 数 名： ClearInstMtFromQueue
  功    能： 清实例时清相关的队列
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值：  void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/06/13    4.0			张宝卿                创建
=============================================================================*/
void CMtAdpInst::ClearInstMtFromQueue()
{
    if ( (u8)CALL_OUTGOING == m_byDirect )
    {
        if ( g_cMtAdpApp.m_tCallQueue.IsMtInQueue(m_byConfIdx, m_byMtId) )
        {
            CallNextFromQueue(m_byConfIdx, m_byMtId);
            MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP,"[ClearInstMt] Conf.%d, Mt.%d in call queue \n", m_byConfIdx, m_byMtId);
        }
        else if ( g_cMtAdpApp.m_tWaitQueue.IsMtInQueue(m_byConfIdx, m_byMtId) )
        {
            TMtCallInfo tCallInfo;
            tCallInfo.m_tMt.SetMtId(m_byMtId);
            tCallInfo.m_byConfIdx = m_byConfIdx;
            g_cMtAdpApp.m_tWaitQueue.ProcQueueInfo(tCallInfo, FALSE);
            MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP,"[ClearInstMt] Conf.%d, Mt.%d in wait queue \n", m_byConfIdx, m_byMtId);
        }
        else
        {
            MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ClearInstMt] Conf.%d, Mt.%d unexist in queue \n", m_byConfIdx, m_byMtId);
        }
    }
    else
    {
        MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ClearInstMt] Conf.%d, Mt.%d's m_byDirect.%d \n", m_byConfIdx, m_byMtId, m_byDirect );
    }

    return;
}

/*=============================================================================
函 数 名： SendSMcuMtListInBuf
功    能： 发送SMcuMtList Buffer
算法实现： 
全局变量： 
参    数： byLastPack : 是否是最后一包
返 回 值：  
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2011/11/21  4.6			刘旭                  新加
=============================================================================*/
void CMtAdpInst::SendSMcuMtListInBuf( const BOOL32 bLastPack )
{
	m_bySMcuMtListBufTimerStart = 0;
	KillTimer(TIMER_SEND_SMCU_MTLIST);
	
	const u32 dwMtListNum = m_cSMcuMtListSendBuf.GetUsedNum();
	if (!dwMtListNum)
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Warn: m_cSMcuMtListSendBuf is Null when buffer send\n");
		return;
	}
	
	CServMsg cMsg;
	u8 byLastPack = bLastPack ? 1 : 0;
	cMsg.SetEventId( MCU_MCU_MTLIST_ACK );
	cMsg.SetMsgBody(&byLastPack, sizeof(byLastPack));
	
	TMcuMcuMtInfo tInfo;
	const u32 dwMaxNumInBuf = m_cSMcuMtListSendBuf.GetCapacity();
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[conf.%d]%d mts in buf\n", m_byConfIdx, dwMtListNum);
	
	u32 dwCatNum = 0;
	for ( u32 dwLoop = 0; dwLoop < dwMaxNumInBuf && dwCatNum < dwMtListNum; dwLoop++ )
	{
		if (m_cSMcuMtListSendBuf.Get(dwLoop, tInfo))
		{
			cMsg.CatMsgBody((u8*)&tInfo, sizeof(tInfo));
			dwCatNum++;
		}
	}
	
	m_cSMcuMtListSendBuf.ClearAll();
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[conf.%d]Send %u smcumt from Mcu(%u,%d) to mcuvc, last pack flag is %d\n", 
		m_byConfIdx, dwCatNum, LOCAL_MCUIDX, m_byMtId, byLastPack );
	
	SendMsgToMcu(cMsg);
}

/*=============================================================================
    函 数 名： SendMsgToMcuInst
    功    能： 
    算法实现： 
    全局变量： 
    参    数： CServMsg & cServMsg
    返 回 值： BOOL 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/9/10   3.6			万春雷                增加双层建链支持
=============================================================================*/
BOOL32 CMtAdpInst::SendMsgToMcuInst(const CServMsg & cServMsg)
{	
	BOOL32 bRet = TRUE;

	if (g_cMtAdpApp.m_bAttachedToVc || OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNode))
	{
		if (post(g_cMtAdpApp.m_dwMcuIId, cServMsg.GetEventId(), cServMsg.GetServMsg(), 
			cServMsg.GetServMsgLen(), g_cMtAdpApp.m_dwMcuNode) < 0)
		{
			bRet = FALSE;
		}
	}
	if (OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNodeB))// && g_cMtAdpApp.m_bDoubleLink)
	{
		if (post(g_cMtAdpApp.m_dwMcuIIdB, cServMsg.GetEventId(), cServMsg.GetServMsg(), 
			cServMsg.GetServMsgLen(), g_cMtAdpApp.m_dwMcuNodeB) < 0)
		{
			bRet = FALSE;
		}
	}
	
	return bRet;
}

/*=============================================================================
    函 数 名： SendMsgToMcuDaemon
    功    能： 
    算法实现： 
    全局变量： 
    参    数： u16 wEvent
               u8 * const pbyMsg
               u16 wLen
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/9/10   3.6			万春雷                增加双层建链支持
=============================================================================*/
void CMtAdpInst::SendMsgToMcuDaemon(u16 wEvent, u8 * const pbyMsg, u16 wLen)
{
	if (g_cMtAdpApp.m_bAttachedToVc)
	{
		post(MAKEIID(AID_MCU_MTADPSSN, CInstance::DAEMON), wEvent, pbyMsg, wLen);
	}
	else	
	{
		if (OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNode))
		{
			post(MAKEIID(AID_MCU_MTADPSSN, CInstance::DAEMON, TCP_GATEWAY), wEvent, 
				 pbyMsg, wLen, g_cMtAdpApp.m_dwMcuNode);
		}
		if (OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuNodeB))// && g_cMtAdpApp.m_bDoubleLink)
		{
			post(MAKEIID(AID_MCU_MTADPSSN, CInstance::DAEMON, TCP_GATEWAY), wEvent, 
				 pbyMsg, wLen, g_cMtAdpApp.m_dwMcuNodeB);
		}
	}
	
	return;	
}

/*=============================================================================
    函 数 名： SendMsgToGK
    功    能： 
    算法实现： 
    全局变量： 
    参    数： u16 wEvent
               u8 * const pbyMsg
               u16 wLen
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/09  4.0			张宝卿                创建
=============================================================================*/
void CMtAdpInst::SendMsgToGK( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
    if ( OspIsValidTcpNode(g_cMtAdpApp.m_dwGKNode) )
    {
        s32 nRet = post( g_cMtAdpApp.m_dwGKIId, wEvent, pbyMsg, wLen, g_cMtAdpApp.m_dwGKNode );
        if ( OSP_OK != nRet )
        {
            MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[SendMsgToGK] msg.%d<%s> send failed !\n", wEvent, OspEventDesc(wEvent) );
        }
    }
    else
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[SendMsgToGK] GK node not exist msg.%d<%s> send failed !\n", wEvent, OspEventDesc(wEvent) );
    }
    return;
}

/*====================================================================
    函数名      ：SendMsgToMcu
    功能        ：向MCU发送消息    
    输入参数说明：CServMsg & cServMsg，待发送的消息
    返回值说明  ：无    
====================================================================*/
void CMtAdpInst::SendMsgToMcu(CServMsg & cServMsg)
{	
	cServMsg.SetMcuId((u8)g_cMtAdpApp.m_wMcuNetId);
	cServMsg.SetSrcDriId(g_cMtAdpApp.m_byDriId);
	cServMsg.SetSrcMtId(m_byMtId);	
	cServMsg.SetConfIdx(m_byConfIdx);
	cServMsg.SetConfId(m_cConfId);
	cServMsg.SetTimer(20);
	
	if (SendMsgToMcuInst(cServMsg))
	{
		MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "mtadp -- > vc : %u: %s (bodyLen=%u)\n", cServMsg.GetEventId(), 
			OspEventDesc(cServMsg.GetEventId()), cServMsg.GetMsgBodyLen());
	}
	else
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Send %u(%s) to Mcu failed!\n", cServMsg.GetEventId(),
			OspEventDesc(cServMsg.GetEventId()));
	}

	return;
}
 
/*====================================================================
    函数名      ：BuildAndSendMsgToMcu
    功能        ：根据事件号和某个消息内容创建一个消息并发送给MCU
    输入参数说明：u16 wEvent 事件
				：u8 * const pbyMsg 消息体，缺省为NULL
				：u16 wLen 消息体长度，缺省为0
    返回值说明  ：无    
====================================================================*/
void CMtAdpInst::BuildAndSendMsgToMcu(u16 wEvent, u8 * const pbyMsg, u16 wLen)
{
	CServMsg cServMsg;
	cServMsg.SetEventId(wEvent);
	cServMsg.SetMsgBody(pbyMsg, wLen);
	SendMsgToMcu(cServMsg);

	return;
}

/*====================================================================
    函数名      ：BuildAndSendMsgToMcu
    功能        ：根据事件号和两项消息内容创建一个CServMsg并发送给MCU
    输入参数说明：u16 wEvent 事件
				：u8 * const pbyMsg1 消息体1，缺省为NULL
				：u16 wLen1 消息体1长度，缺省为0	
				：u8 * const pbyMsg2 消息体2，缺省为NULL
				：u16 wLen2 消息体2长度，缺省为0
    返回值说明  ：无    
====================================================================*/
void CMtAdpInst::BuildAndSendMsgToMcu2( u16 wEvent, u8 * const pbyMsg1, u16 wLen1,
										            u8 * const pbyMsg2, u16 wLen2 )
{
	CServMsg cServMsg;
	cServMsg.SetEventId(wEvent);
	cServMsg.SetMsgBody(pbyMsg1, wLen1);
	cServMsg.CatMsgBody(pbyMsg2, wLen2);
	SendMsgToMcu(cServMsg);

	return;
}


/*=============================================================================
  函 数 名： InstanceDump
  功    能： 重载打印信息
  算法实现： 
  全局变量： 
  参    数： u32 param, 打印状态过滤
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0			TanGuang                创建
=============================================================================*/
void CMtAdpInst::InstanceDump( u32 dwParam )
{
	s8 achState[16];
	s8 achIp[16];

	switch(CurState()) 
	{
	case STATE_IDLE:		sprintf(achState, "Idle");		break;
	case STATE_CALLING:		sprintf(achState, "Calling"); 	break;
	case STATE_GKCALLING:	sprintf(achState, "Gkcalling");	break;
	case STATE_NORMAL:		sprintf(achState, "Normal");	break;
	default:				sprintf(achState, "Unknown"); 
	}

	sprintf( achIp, "%u.%u.%u.%u", QUADADDR(m_dwMtIpAddr) );
	StaticLog( "%5u %6u %4u %16s %9s  %p	  %s\n",
		       GetInsID(), m_byConfIdx, m_byMtId, achIp, achState, m_hsCall,GetPeerProductID (m_hsCall) );

	//print channel information
    if (0 != dwParam)
    {
        for( s32 nCallChanNum = 0; nCallChanNum < MAXNUM_CALL_CHANNEL; nCallChanNum ++ )
        {
            if( m_atChannel[nCallChanNum].hsChan != NULL)
            {				
                if( 0 == nCallChanNum )
                {
                    StaticLog( "\n");				
                }
                s8  achChanDirect[16];			
                switch( m_atChannel[nCallChanNum].byDirect ) 
                {			
                case CHAN_OUTGOING:	sprintf( achChanDirect, "->" );	 break;
                case CHAN_INCOMING: sprintf( achChanDirect, "<-" );	 break;	
				default:break;
                }
                
                s8  achChanName[32];
                CMtAdpUtils::PszGetChannelName( m_atChannel[nCallChanNum].tLogicChan.GetChannelType(), 
                                                achChanName, sizeof(achChanName) );
                
                TTransportAddr &tTransRcv     = m_atChannel[nCallChanNum].tLogicChan.m_tRcvMediaChannel;
                TTransportAddr &tTransRcvCtrl = m_atChannel[nCallChanNum].tLogicChan.m_tRcvMediaCtrlChannel;
                TTransportAddr &tTransSndCtrl = m_atChannel[nCallChanNum].tLogicChan.m_tSndMediaCtrlChannel;
                s32 nRate = m_atChannel[nCallChanNum].tLogicChan.GetFlowControl();
                
                s8  achSubLine[128] = { 0 };
                
                if( m_atChannel[nCallChanNum].byDirect == (u8)CHAN_OUTGOING )
                {
                    sprintf( achSubLine, "%3d. %2s%24s% 4d Kbps %5d->%5d, %5d",
                             nCallChanNum + 1, achChanDirect, achChanName, nRate, tTransSndCtrl.GetPort(),
                             tTransRcv.GetPort(), tTransRcvCtrl.GetPort());
                }
                else
                {
                    sprintf( achSubLine, "%3d. %2s%24s% 4d Kbps %5d, %5d<-%5d",
                             nCallChanNum + 1, achChanDirect, achChanName, nRate, tTransRcv.GetPort(), 
                             tTransRcvCtrl.GetPort(), tTransSndCtrl.GetPort());
                }
                StaticLog( "%s\n", achSubLine );				
            }
        }		
        StaticLog( "\n");
    }	

	return;
}


/*=============================================================================
  函 数 名： InstanceEntry
  功    能： 实例消息处理入口函数
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0			TanGuang                创建
=============================================================================*/
void CMtAdpInst::InstanceEntry( CMessage *const pcMsg )
{	
	if( NULL == pcMsg )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "NULL message received!\n" );
		return;
	}
	
	//standard events from mcu to mt
	if( EV_MCUMT_STD_BGN <= pcMsg->event && pcMsg->event <= EV_MCUMT_STD_END )
	{		
		MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP,"vc -- > mtadp std mcu to mt: %u: %s\n", pcMsg->event, OspEventDesc(pcMsg->event));
		ProcStdEvMcuMt(pcMsg);
		return;
	}
	//standard events from mcu to mmcu
	if( (EV_MCUMCU_STD_BGN <= pcMsg->event && pcMsg->event <= EV_MCUMCU_STD_END ) ||
		 (EV_MCUMCU_KDV_BGN <= pcMsg->event && pcMsg->event <= EV_MCUMCU_KDV_END)
		)
	{
		MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "vc -- > mtadp mcu to mmcu: %u: %s\n", pcMsg->event, OspEventDesc(pcMsg->event));		
		ProcEvInterMcu(pcMsg);
		return;
	}
	//kdv customed events from mcu to mt
	if( EV_MCUMT_KDV_BGN <= pcMsg->event && pcMsg->event <= EV_MCUMT_KDV_END ) 		
	{
		OTHERRETURN
		MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "vc -- > mtadp nostd mcu to mt: %u: %s\n", pcMsg->event, OspEventDesc(pcMsg->event));
		ProcKdvCustomEvMcuMt(pcMsg);
		return;
	}
	//standard RAS events
	if( EV_RAS_BGN <= pcMsg->event && pcMsg->event <= EV_RAS_END )
	{
		//this routine handles all calls & call backs for RAS events
		//ProcRasEvMcuMt( pcMsg );
		return;
	}

	// [9/6/2010 xliang] VC events just pass to remote mcu without modification 
	if( EV_VCSVC_BGN <= pcMsg->event && pcMsg->event <= EV_VCSVS_END || 
		EV_MCSVC_BGN <= pcMsg->event && pcMsg->event <= EV_MCSVC_END )
	{
		ProcVCMsgMcuMcu(pcMsg);
		return;
	}
	switch( pcMsg->event )
	{
	case TIMER_ROUNDTRIPDELAY_START:
		{
			SetTimer( TIMER_ROUNDTRIPDELAY_REQ, g_cMtAdpApp.m_wMaxRTDInterval*1000 );
		}
		break;
	case TIMER_ROUNDTRIPDELAY_REQ:	//send roundTripDelayRequest
		ProcMcuMtRoundTripDelayReq( pcMsg );
		break;    

	case TIMER_IRR:
		ProcSendIRR();
		break;
		
	case EV_RAD_FECCCTRL_NOTIF:
		ProcRadEvFeccCtrl( pcMsg );
		break;
	case EV_RAD_MMCUCTRL_NOTIF:
		ProcRadEvMmcuCtrl( pcMsg );
		break;
		
		//--------vrsrec新录播消息支持-------------
	case MCU_REC_STARTREC_REQ:
		ProcMcuVrsStartRecReq( pcMsg );
		break;
	case MCU_REC_LISTALLRECORD_REQ:
		ProcMcuVrsListAllRecordReq( pcMsg );
		break;
	case MCU_REC_STARTPLAY_REQ:
		ProcMcuVrsStartPlayReq( pcMsg );
		break;
	case MCU_REC_SEEK_REQ:					//放像进度拖动
		ProcMcuVrsSeekReq( pcMsg );
		break;
		
	case WAIT_RESPONSE_TIMEOUT:		//wait for calling  response timeout
	case WAIT_PASSWORD_TIMEOUT:		//wait for password response timeout
	case TIMER_AUDIOINFO:
	case TIMER_VIDEOINFO:
	case TIMER_MTLIST:
	case TIMER_STARTAFTERSTOPSMIX:
	//case TIMER_H245_CAPBILITY_NOTIFY_OVERTIME:
		ProcTimerExpired( pcMsg );
		break;
		
	case TIMER_VRS_REC_RSP:
		ProcVrsRspTimeout();
		break;
		
	case TIMER_SEND_SMCU_MTLIST:
		//(超时处理) 定时器到, 不管有没有收到最后一包, 都当做最后一包发送
		SendSMcuMtListInBuf(TRUE);
		break;

	case CLEAR_INSTANCE:
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "receive msg CLEAR_INSTANCE. ClearInst.\n");
		ClearInst();
		break;

	default:
		UNEXPECTED_MESSAGE(pcMsg->event);
		
	}
}


/*=============================================================================
  函 数 名： ProcRadEvNewCall
  功    能： 处理协议栈的新呼叫到来事件，填充HsCall
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg, 消息体：HCALL，新呼叫协议栈句柄
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0			TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcRadEvNewCall(CMessage * const pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	if(!BODY_LEN_GE(cServMsg, sizeof(HCALL))) return;
	
	if(STATE_IDLE != CurState())
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "ProcRadEvNewCall CurState is not STATE_IDLE!\n");
		return;
	}
	m_hsCall = *(HCALL*)cServMsg.GetMsgBody();	
	m_byDirect = (u8)CALL_INCOMING;

	CHANGE_STATE(STATE_CALLING);

	return;
}


/*=============================================================================
  函 数 名： ProcRadEvNewChannel
  功    能： 处理协议栈的新通道请求到来事件，分配HaChan 
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg, 消息体：HCHAN，新信道协议栈句柄
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0			TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcRadEvNewChannel(CMessage * const pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );	
	if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(HCHAN)) ) 
		return;

	HCALL hsCall = *(HCALL*)cServMsg.GetMsgBody();

	if( m_hsCall != hsCall )
		return;

	s32 nNewChanSlot = GetFreeChannel();
	if( nNewChanSlot < 0 )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Failed to allocate space for new incoming channel!\n");
		return;
	}
	m_atChannel[nNewChanSlot].hsChan = *(HCHAN*)(cServMsg.GetMsgBody() + sizeof(HCALL));
	m_atChannel[nNewChanSlot].byDirect = (u8)CHAN_INCOMING;

	return;
}


/*=============================================================================
  函 数 名： ProcRadEvCallCtrl
  功    能： 处理协议栈传来的的呼叫控制事件
  算法实现： 
  全局变量： 
  参    数： CMessage->CServMsg, 消息体：u16，呼叫控制消息类型＋呼叫控制消息内容
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0			TanGuang              创建
  2005/12/30	4.0			张宝卿				  兼容3.6终端	
=============================================================================*/
void CMtAdpInst::ProcRadEvCallCtrl( CMessage * const pcMsg )
{	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	//消息体校验
	if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(u16)) )
	{
		return;
	}
	//获得呼叫句柄
	HCALL hsCall = *(HCALL*)cServMsg.GetMsgBody();

	//获得呼叫控制消息和类型
	u16 wMsgType = *(u16*)( cServMsg.GetMsgBody() + sizeof(HCALL) );	
	u8 *pbyMsg   = (u8*)( cServMsg.GetMsgBody() + sizeof(HCALL) + sizeof(wMsgType) );
	
	//句柄校验
	if( hsCall != m_hsCall)	
		return;

	//u16 wTimer = 20;
	
	//消息处理
	switch( wMsgType )
	{
	//呼叫到来
	case h_call_incoming:        //call offering	
		{
			if( !BODY_LEN_GE(cServMsg, sizeof(wMsgType) + sizeof(HCALL) + sizeof(TCALLPARAM))) 
			{
				return;
			}
			m_byDirect = (u8)CALL_INCOMING;
			
			//取呼叫参数
			m_tIncomingCallParams = *(TCALLPARAM*)pbyMsg;				
			m_dwMtIpAddr = m_tIncomingCallParams.GetCallingIp();
			m_wCallRate = (u16)m_tIncomingCallParams.GetCallRate();
			
			//厂商ID
			s32 nVendorId = *(s32*)((u8*)pbyMsg + sizeof(TCALLPARAM));

            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "MT Incoming Ip(%u.%u.%u.%u) CallRate.%d VendorIdFromStack.%d\n", QUADADDR(m_dwMtIpAddr), m_wCallRate, nVendorId);

            m_byVendorId = CMtAdpUtils::GetVendorId((u8)nVendorId);

            if (g_cMtAdpApp.IsManuInRestrictTable(m_byVendorId))
            {
                MAPrint( LOG_LVL_WARNING, MID_MCULIB_MTADP, "[h_call_incoming]Manu%d is restrict call!\n", m_byVendorId);
                return;
            }

			//取当前终端的版本类型
            m_byMtVer = (u8)GetPeerMtVer(hsCall);

			m_emEndpointType = (emEndpointType)m_tIncomingCallParams.m_emEndpointType;
			if( m_byVendorId == MT_MANU_KDC && m_emEndpointType == emEndpointTypeMCU )
			{
				m_byVendorId = MT_MANU_KDCMCU;
				//对于科达MCU其发起端的端口设置的为其呼叫使用的随机端口，
				//而非接收呼叫的端口，实际接收呼叫端口均为MCU_Q931_PORT		
				m_tIncomingCallParams.SetCallingAddr(m_tIncomingCallParams.GetCallingIp(),
					                                 MCU_Q931_PORT);
			}
            // guzh [4/4/2007] 对于kedaMCU 获取其行政级别
            BOOL32 bLowerMcuCalledIn = FALSE;
            if ( m_byVendorId == MT_MANU_KDCMCU )
            {
                s8* szDisplayInfo = m_tIncomingCallParams.GetDisplayInfo();
                u8 byDstAdminLevel = GetLevelByStr(szDisplayInfo);

                // 如果对端比我大
                if ( byDstAdminLevel > g_cMtAdpApp.m_byCascadeLevel )
                {
                    // NACK, drop the call                    
                    MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "Lower level mcu(%d) calling to me(%d)!\n", 
                                 byDstAdminLevel,
                                 g_cMtAdpApp.m_byCascadeLevel);		

                    bLowerMcuCalledIn = TRUE;
                }
            }
			// xliang [2/14/2009] 接入板能力过滤
			BOOL32 bAccessFull = FALSE;		//接入能力是否满
			if( m_emEndpointType == emEndpointTypeMCU )
			{
				//当前接入数目+2
				if( !g_cMtAdpApp.IsInConnMcuList(GetInsID()) )//已经计算在接入能力之内，不再处理
				{
					//如把其计数后超过最大能力，则不给起占接入能力计数
					if( g_cMtAdpApp.m_wCurNumConntMt + 2 > g_cMtAdpApp.m_wMaxNumConntMt )
					{
						bAccessFull = TRUE;
					}
					else
					{
						g_cMtAdpApp.m_wCurNumConntMt += 2;
						g_cMtAdpApp.AddToConnMcuList(GetInsID());
						MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "m_wCurNumConntMt+2! Now CurNumConntMt is %d\n",g_cMtAdpApp.m_wCurNumConntMt);
					}
				}
				else
				{
					MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Already in HDI access capability!\n");
				}
			}
			else
			{
				//当前接入数目+1
				if( !g_cMtAdpApp.IsInConnMtList(GetInsID()) )
				{
					if( g_cMtAdpApp.m_wCurNumConntMt + 1 > g_cMtAdpApp.m_wMaxNumConntMt )
					{
						bAccessFull = TRUE;
					}
					else
					{
						g_cMtAdpApp.m_wCurNumConntMt ++;
						g_cMtAdpApp.AddToConnMtList(GetInsID());
						MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "m_wCurNumConntMt+1! Now CurNumConntMt is %d\n",g_cMtAdpApp.m_wCurNumConntMt);
					}
				}
				else
				{
					MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Already in HDI access capability!\n");
				}
			}
			if( bAccessFull )	//上报错误给MCU
			{
				//cServMsg.SetSrcDriId(g_cMtAdpApp.m_byDriId);//封装在SendMsgToMcu中
				cServMsg.SetEventId(MT_MCU_CALLFAIL_HDIFULL_NOTIF);
				cServMsg.SetMsgBody();
				SendMsgToMcu(cServMsg);
				
				//KillTimer(WAIT_RESPONSE_TIMEOUT);
				ClearInst(); 
				
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[h_call_incoming]HDI Access capability is full!\n");
				return;
			}

            // zbq [05/14/2007] 产品类型呼叫限制
            if ( !IsCallingSupported(hsCall) )
            {
                ClearInst( MT_MCU_MTDISCONNECTED_NOTIF, (u8)emDisconnectReasonUnmatched );
                MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Mt with handle.0x%x calling is not supported !\n", hsCall );
                return;
            }

            // zbq [05/06/01] 保存呼叫进来的实体的别名
            TMtAlias tSrcMtAlias;
            tSrcMtAlias.SetNull();
            
            tSrcMtAlias.m_AliasType = (u8)mtAliasTypeTransportAddress;
            tSrcMtAlias.m_tTransportAddr.SetIpAddr(ntohl( m_tIncomingCallParams.GetCallingIp()));
            tSrcMtAlias.m_tTransportAddr.SetPort( m_tIncomingCallParams.GetCallingPort());

            m_tMtAlias = tSrcMtAlias;

			//科达终端, 保存其非标准信息
// 			m_nNonStandConfInfoDataLen = 0;
// 			if( m_emEndpointType == emEndpointTypeMT && m_byVendorId == MT_MANU_KDC )	
// 			{				
// 				u8 *pbyNonData = NULL;
// 				s32 nLen = 0;
// 				m_tIncomingCallParams.GetNonStandData(&pbyNonData, &nLen);
// 				
// 				//3.6终端
// 				if ( m_byMtVer == (u8)emMtVer36 )
// 				{
// 					if ((u32)nLen == sizeof(TConfInfoVer36) && NULL != pbyNonData ) 
// 					{
// 						m_nNonStandConfInfoDataLen = nLen;
// 						memcpy( m_pbyNonStandConfInfoData36, pbyNonData, (u32)m_nNonStandConfInfoDataLen36 );
// 					}
// 				}
// 				else
// 				{
//                     // guzh [7/27/2006] 这里要兼容4.0R2 、R3以及以后版本。只要有消息体就保存下来
// 					if ( NULL != pbyNonData ) 
// 					{
// 						m_nNonStandConfInfoDataLen = min( (u32)nLen, sizeof(TConfInfo));
// 						memcpy( m_pbyNonStandConfInfoData, pbyNonData, (u32)m_nNonStandConfInfoDataLen );
// 					}
// 
// 				}
// 				
// 			}

            if (bLowerMcuCalledIn)
            {
                ProcIncomingCall(TRUE);
                break;
            }

			//call with RAS. first try to get permission from GK
			if( g_cMtAdpApp.m_tH323Config.IsManualRAS() && g_cMtAdpApp.m_bGkAddrSet )
			{
				//3.6终端别名结构MtAlias和4.0的完全一样, 所以此处在进行GK鉴权时
				//没有区分, 都是用4.0的结构进行鉴权请求的. 在严格意义应当进行适配. 
				TMtAlias   tSrcAddr;				
				TMtAlias   tDstAddr;
				memset( &tSrcAddr, 0, sizeof(tSrcAddr) );
				memset( &tDstAddr, 0, sizeof(tDstAddr) );
				
				tSrcAddr.m_AliasType = (u8)mtAliasTypeTransportAddress;
				tSrcAddr.m_tTransportAddr.SetIpAddr(ntohl( m_tIncomingCallParams.GetCallingIp()));
				tSrcAddr.m_tTransportAddr.SetPort( m_tIncomingCallParams.GetCallingPort());
						
				s8  achAliasBuf[128];
				cmAlias tAlias;
				memset(&tAlias, 0, sizeof(tAlias));
				tAlias.string = achAliasBuf;
				tAlias.length = sizeof(achAliasBuf);					
				//s32 nLen = sizeof(tAlias);
					
                //  xsl [6/16/2006] 被叫arq时gk需要ip地址
//				if( cmCallGetParam(m_hsCall, cmParamDestinationAddress, 0, &nLen, (s8 *)&tAlias) >= 0)
//				{
//					CMtAdpUtils::MtAliasOut2In(tAlias, tDstAddr);
//				}
//				else
				{
					tDstAddr.m_AliasType = (u8)mtAliasTypeTransportAddress;
					tDstAddr.m_tTransportAddr.SetIpAddr(ntohl(g_cMtAdpApp.m_dwMtAdpIpAddr));
					tDstAddr.m_tTransportAddr.SetPort(MCU_Q931_PORT);
				}				
					
				CHANGE_STATE(STATE_CALLING);
				if(ProcSendARQ(tSrcAddr, tDstAddr, m_wCallRate))
				{
					CHANGE_STATE(STATE_GKCALLING);	
				}
				else
				{
					MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "ARQ sending failed!\n" );					
					ClearInst();
				}				
			}
			else
			{
				ProcIncomingCall();
			}
		}
		break;

	//呼叫已确认
	case h_call_connected:       //call established.	  1 s32: vendor ID
		{			
			if( CurState() != STATE_CALLING )
			{
				UNEXPECTED_MESSAGE( pcMsg->event );
				return;
			}

			if( !BODY_LEN_GE(cServMsg, sizeof(wMsgType) + sizeof(HCALL) + sizeof(TNODEINFO)))
				return;
            
			TNODEINFO tNode = *(TNODEINFO*)pbyMsg;

			// [3/11/2010 xliang] get productID 
// 			u8 abyBuf[128] = {0};
// 			u8 *pbyProductId = &abyBuf[0];
// 			s32 nProductIDSize = tNode.GetProductIdSize();
// 			tNode.GetProductId(pbyProductId, nProductIDSize);

			m_byVendorId = CMtAdpUtils::GetVendorId( (u8)(tNode.GetVenderId()) );
            if ( g_cMtAdpApp.IsManuInRestrictTable(m_byVendorId) )
            {
                MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[h_call_connected]Manu %d is Restricted call!\n", m_byVendorId );
                return;
            }
			m_emEndpointType = (emEndpointType)tNode.m_emEndpointType;
			// xliang [2/14/2009] 对于Call_INCOMING，在h_call_incoming处理中限，以保证在第一时间进行过滤
			if(m_byDirect == (u8)CALL_OUTGOING)
			{
				// xliang [1/4/2009] 接入板能力过滤
				BOOL32 bAccessFull = FALSE;		//接入能力是否满
				if( m_emEndpointType == emEndpointTypeMCU )
				{
					//当前接入数目+2
					if( !g_cMtAdpApp.IsInConnMcuList(GetInsID()) )//已经计算在接入能力之内，不再处理
					{
						//如把其计数后超过最大能力，则不给起占接入能力计数
						if( g_cMtAdpApp.m_wCurNumConntMt + 2 > g_cMtAdpApp.m_wMaxNumConntMt )
						{
							bAccessFull = TRUE;
						}
						else
						{
							g_cMtAdpApp.m_wCurNumConntMt += 2;
							g_cMtAdpApp.AddToConnMcuList(GetInsID());
							MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "m_wCurNumConntMt+2! Now CurNumConntMt is %d\n",g_cMtAdpApp.m_wCurNumConntMt);
						}
					}
					else
					{
						MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Already in HDI access capability!\n");
					}
				}
				else
				{
					//当前接入数目+1
					if( !g_cMtAdpApp.IsInConnMtList(GetInsID()) )
					{
						if( g_cMtAdpApp.m_wCurNumConntMt + 1 > g_cMtAdpApp.m_wMaxNumConntMt )
						{
							bAccessFull = TRUE;
						}
						else
						{
							g_cMtAdpApp.m_wCurNumConntMt ++;
							g_cMtAdpApp.AddToConnMtList(GetInsID());
							MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "m_wCurNumConntMt+1! Now CurNumConntMt is %d\n",g_cMtAdpApp.m_wCurNumConntMt);
						}
					}
					else
					{
						MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Already in HDI access capability!\n");
					}
				}
				if( bAccessFull )	//上报错误给MCU
				{
					//cServMsg.SetSrcDriId(g_cMtAdpApp.m_byDriId);//封装在SendMsgToMcu中
					cServMsg.SetEventId(MT_MCU_CALLFAIL_HDIFULL_NOTIF);
					cServMsg.SetMsgBody();
					SendMsgToMcu(cServMsg);
					
					KillTimer(WAIT_RESPONSE_TIMEOUT);
					ClearInst(); 
					
					MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[h_call_connected]HDI Access capability is full!\n");
					
					return;
				}
			}

            //获取信息判断是否支持H460
            if ( tNode.m_tH460Features.m_bPeerSupportH460 && 
                 tNode.m_tH460Features.m_bPeerMediaTraversalServer)
            {
                m_bIsNeedPinHold = TRUE;
            }
            else
            {
                m_bIsNeedPinHold = FALSE;
            }
            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "h_call_connected m_bIsNeedPinHold(%d) m_bPeerMediaTraversalServer(%d)\n",
				tNode.m_tH460Features.m_bPeerSupportH460, tNode.m_tH460Features.m_bPeerMediaTraversalServer);

            //取当前终端的版本类型
            m_byMtVer = (u8)GetPeerMtVer(hsCall);

            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "MT Connect Ip=%u.%u.%u.%u CallRate=%d VendorIdFromStack=%d\n", QUADADDR(m_dwMtIpAddr), m_wCallRate, tNode.GetVenderId());

            if( m_byVendorId == MT_MANU_KDC && m_emEndpointType == emEndpointTypeMCU )
            {
                m_byVendorId = MT_MANU_KDCMCU;								
			}

            // zbq [05/14/2007] 产品类型呼叫限制
            if ( !IsCallingSupported(hsCall) )
            {
                ClearInst( MT_MCU_MTDISCONNECTED_NOTIF, (u8)emDisconnectReasonUnmatched );
                MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Mt.%d(%s) with handle.0x%x connected is not supported !\n", m_byMtId, tNode.GetName(), hsCall );
                return;
            }

			if( m_byDirect == (u8)CALL_OUTGOING )				
			{	
				cServMsg.SetEventId( m_wSavedEvent + 1 );
				
				TMtAlias tMtH323Alias;
				TMtAlias tMtAddr;
#ifdef _UTF8
				//1. if calling addr is not null, then use it
				BOOL32 bUseCalled323Alias = FALSE; //  [pengguofeng 5/23/2013]主动使用被叫地址里的323别名，此别名已经是UTF8
				PTAliasAddr ptAliasAddr = NULL;
				for ( u8 byAliasType = 0; byAliasType < type_others +1; byAliasType++)
				{
					ptAliasAddr = tNode.GetCalledAliasAddr(byAliasType);
					if ( !ptAliasAddr )
					{
						continue;
					}
					if ( ptAliasAddr->GetAliasType() == type_h323ID /*type_email*/ )
					{
						tMtH323Alias.SetH323Alias( ptAliasAddr->GetAliasName() );
						bUseCalled323Alias = TRUE;
						break;
					}
				}

				//2. else , set 323 alias with display name
				if ( !bUseCalled323Alias )
				{
					s8 achDispName[LEN_DISPLAY_MAX+1]; // Q931:根据终端发送信息，显示名称需要转换 [liaokang 5/20/2013]
					memset(achDispName, 0, sizeof(achDispName));
					gb2312_to_utf8(tNode.GetName(), achDispName, sizeof(achDispName)-1);
					tMtH323Alias.SetH323Alias( achDispName );
				}
#else
				tMtH323Alias.SetH323Alias( tNode.GetName() );
#endif
				cServMsg.SetMsgBody((u8*)&tMtH323Alias, sizeof(TMtAlias) );

				tMtAddr.m_AliasType = (u8)mtAliasTypeTransportAddress;
				tMtAddr.m_tTransportAddr.SetIpAddr(ntohl(m_dwMtIpAddr));
                //zbq[07/24/2007] 终端别名上报修正
                if ( 0 == m_wMtPort )
                {
                    m_wMtPort = MCU_Q931_PORT;
                }
				tMtAddr.m_tTransportAddr.SetPort(m_wMtPort);
				cServMsg.CatMsgBody( (u8*)&tMtAddr, sizeof(TMtAlias) );	
				
				u8 byType = 0;
				switch(m_emEndpointType) 
				{
				case emEndpointTypeMCU:	 byType = TYPE_MCU;	break;
				case emEndpointTypeMT:	 byType = TYPE_MT;	break;
				default:                 byType = TYPE_MT;  break;
				}
				cServMsg.CatMsgBody( (u8*)&byType, 1 );

				u8 byEncrypt = (u8)tNode.IsEncrypt();
				cServMsg.CatMsgBody( &byEncrypt, sizeof(byEncrypt) );					
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Invite MT and get accepted, Encrypt is %d in call connected\n", byEncrypt);	

				SendMsgToMcu(cServMsg);

				m_wSavedEvent = 0;
				KillTimer( WAIT_RESPONSE_TIMEOUT );	
			}
			
		    CHANGE_STATE( STATE_NORMAL );
			
			if( m_nIrrFrequency != 0 )
			{
				SetTimer( TIMER_IRR, m_nIrrFrequency * 1000 ) ;
			}
		}		
		break;

	case h_call_disconnected:    //call dropped	  1 u8,reason
		{
			if(!BODY_LEN_GE(cServMsg, sizeof(wMsgType) + sizeof(u8))) 
				return;

			u8  byReason = *(u8*)pbyMsg;
			pbyMsg += sizeof(byReason);

			u8 byConfLevel = 0;
			s8 achConfName[MAXLEN_CONFNAME] = {0};
			if ((u8)reason_mcuoccupy == byReason)
			{
				if (BODY_LEN_GE(cServMsg, sizeof(wMsgType) + sizeof(u8) + sizeof(u8) + MAXLEN_CONFNAME))
				{
					byConfLevel = *(u8*)pbyMsg;
					pbyMsg += sizeof(u8);

                    //[5/22/2013 liaokang] 转码
                    if( emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() &&
                        emenCoding_GBK == GetEndPointEncoding() )
                    {
                        gb2312_to_utf8((s8*)pbyMsg, achConfName, sizeof(achConfName) - 1);
                    }
                    else
                    {
                        memcpy(achConfName, pbyMsg, MAXLEN_CONFNAME);
                    }
				}
				else
				{
					MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[ProcRadEvCallCtrl] h_call_disconnected because of reason_mcuoccupy(len:%d) with uncorrect msgbody(len:%d)\n",
								 sizeof(wMsgType) + sizeof(u8) + sizeof(u8) + MAXLEN_CONFNAME, cServMsg.GetMsgBodyLen());
					byReason = (u8)reason_busy;
				}

			}


			u16 wErrorCode = 0;
			u8  byCause = 0;          			

			//strictly, we should wait for the ACK for DRQ (DCF/DRJ) before
			//clearing this instance, for the DRQ may fail and the resoure 
			//engaged on GK cannot be released. For now, we just
			//proceed regardless of the response for simplicity
			static const s8*  const pszReasonStr[] = { " ", "Busy",	"Normal", "Rejected", 
										  "Unreachable", "Local", "Unknown", 
										  "", "", "", "Busy Occupy", "Remote reconnect",
											"Conf Holding" , "Has Cascaded" };
				
			switch( byReason ) 
			{
			case reason_busy:       byCause    = (u8)emDisconnectReasonBusy; 
									wErrorCode = (u16)ERR_MCU_MTBUSY;           break;
			case reason_normal:		byCause    = (u8)emDisconnectReasonNormal; break;
			case reason_rejected:	byCause    = (u8)emDisconnectReasonRejected; 
									wErrorCode = (u16)ERR_MCU_MTREJECT;	       break;
			case reason_unreachable:byCause    = (u8)emDisconnectReasonUnreachable;	
									wErrorCode = (u16)ERR_MCU_MTUNREACHABLE;    break;									
			case reason_local:		byCause    = (u8)emDisconnectReasonLocal;  break;
			case reason_mcuoccupy:  byCause    = (u8)emDisconnectReasonBysyExt;
				                    wErrorCode = (u16)ERR_MCU_VCS_MTBUSY;       break;
            case reason_reconnect:  byCause    = (u8)emDisconnectReasonRemoteReconnect; 
                                    wErrorCode = (u16)ERR_MCU_CALLMMCUFAIL;     break;

			case reason_confholding:byCause    = (u8)emDisconnectReasonRemoteConfHolding; 
                                    wErrorCode = (u16)ERR_MCU_CALLMCUERROR_CONFISHOLDING; break;
			case reason_hascascaded:byCause    = (u8)emDisconnectReasonRemoteHasCascaded; 
                                    wErrorCode = (u16)ERR_MCU_CASADEBYOTHERHIGHLEVELMCU;  break;
									
			case reason_unknown:			
			default:byReason    =    (u8)reason_unknown;
                    byCause     =    (u8)emDisconnectReasonUnknown;break;
			}
			
			//m_hsCall = NULL;// xliang [9/18/2008] 
			for(s32 nLoop = 0; nLoop < MAXNUM_CALL_CHANNEL; nLoop++)
			{			
				m_atChannel[nLoop].Clear();
			}
			
			if( m_byTimeoutTimes && TRUE == m_bEverGotRTDResponse )
				byCause = (u8)emDisconnectReasonRtd;

            //zbq [05/15/2007] 适配到MCU的原因
            u8 byCauseVc = CMtAdpUtils::DisconnectReasonMtAdp2Vc((emDisconnectReason)byCause);

			//inviting failed, need not post disconnect message
			if( byReason == (u8)reason_busy ||
				byReason == (u8)reason_mcuoccupy ||
				byReason == (u8)reason_rejected ||
				byReason == (u8)reason_reconnect ||				
				byReason == (u8)reason_unreachable ||
				byReason == (u8)reason_confholding ||
				byReason == (u8)reason_hascascaded
				)
			{
				if( byReason < (sizeof(pszReasonStr) / sizeof(s8 *)) )
					MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[MtAdpInst %u] Call disconnected due to %s reason\n", 
							   GetInsID(), pszReasonStr[byReason]);

				TMtAlias tMtAlias;				
				tMtAlias.m_AliasType = (u8)mtAliasTypeTransportAddress;
				
                // 防火期代理断链时，挂断后保持重新呼叫
                if(CurState() == STATE_NORMAL && byReason == (u8)reason_unreachable)
                {
                    cServMsg.SetSrcMtId( m_byMtId );
                    cServMsg.SetMsgBody( (u8 *)&byCauseVc, sizeof(byCauseVc) );
                    cServMsg.SetEventId( MT_MCU_MTDISCONNECTED_NOTIF );                    
                }
                else if ((u8)reason_mcuoccupy == byReason)
                {
                    cServMsg.SetMsgBody( (u8*)&tMtAlias, sizeof(TMtAlias) );
				    cServMsg.CatMsgBody( &byCauseVc, sizeof(byCauseVc) );
					cServMsg.CatMsgBody( (u8*)&byConfLevel, sizeof(byConfLevel) );
					cServMsg.CatMsgBody( (u8*)achConfName, sizeof(achConfName) );
				    cServMsg.SetErrorCode( wErrorCode );				
				    cServMsg.SetEventId( m_wSavedEvent + 2 );	
                }
				else
                {
                    cServMsg.SetMsgBody( (u8*)&tMtAlias, sizeof(TMtAlias) );
				    cServMsg.SetErrorCode( wErrorCode );				
				    cServMsg.SetEventId( m_wSavedEvent + 2 );	
				    cServMsg.CatMsgBody( &byCauseVc, sizeof(byCauseVc) );
                }
                SendMsgToMcu(cServMsg);
                
                //呼叫终端被拒，单独发断链通知
                if ( (u8)reason_rejected == byReason ||
					 (u8)reason_reconnect == byReason )
                {
                    cServMsg.SetSrcMtId( m_byMtId );
                    cServMsg.SetMsgBody((u8 *)&byCauseVc, sizeof(byCauseVc));
                    cServMsg.SetEventId( MT_MCU_MTDISCONNECTED_NOTIF );
                    SendMsgToMcu(cServMsg);
                }                
			}
            else if(byReason == (u8)reason_normal || 
                    byReason == (u8)reason_unknown || 
                    byReason == (u8)reason_local)
            {
                cServMsg.SetSrcMtId( m_byMtId );
                cServMsg.SetMsgBody((u8 *)&byCauseVc, sizeof(byCauseVc));
                cServMsg.SetEventId( MT_MCU_MTDISCONNECTED_NOTIF );
                SendMsgToMcu(cServMsg);               
            }           
                        
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "h_call_disconnected with stack reason: %s(%d). ClearInst\n", 
                       byReason < (sizeof(pszReasonStr) / sizeof(s8*)) ? pszReasonStr[byReason] : "Unknown",
                       byReason
                      );
            
			ClearInst();            
		}
		break;

	case h_call_facility: // MMCU-->MCU	
		break;
	default:
		break;

	}

	return;
}

/*=============================================================================
  函 数 名： CallNextFromQueue
  功    能： 踢除当前呼叫记录，继续处理未完成的呼叫
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx：当前完成呼叫的会议索引
             u8 byCurMtId：当前完成呼叫的终端ID
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/04/05	4.0			张宝卿				  创建
=============================================================================*/
void CMtAdpInst::CallNextFromQueue( u8 byConfIdx, u8 byCurMtId )
{
    TMtCallInfo tCallInfo;
    tCallInfo.m_byConfIdx = m_byConfIdx;
    tCallInfo.m_tMt.SetMtId(m_byMtId);

    g_cMtAdpApp.m_tCallQueue.ProcQueueInfo( tCallInfo, FALSE );

    if ( !g_cMtAdpApp.m_tWaitQueue.IsQueueNull() )
    {
        OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), CALLOUT_NEXT_NOTIFY );
    }
    return;
}
/*=============================================================================
  函 数 名： ProcIncomingCall
  功    能： 继续处理到来的呼叫。RAS模式下，则是在ARQ/ACF之后
  算法实现： 
  全局变量： 
  参    数： BOOL32 bLowerMcuCallingIn      是否是行政下级MCU呼叫
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0			TanGuang              创建
  2005/01/03	4.0			张宝卿				  兼容3.6终端
=============================================================================*/
void CMtAdpInst::ProcIncomingCall(BOOL32 bLowerMcuCallingIn)
{
	CServMsg cServMsg;

	if( 0 != m_nIrrFrequency)
	{
		SetTimer( TIMER_IRR, m_nIrrFrequency * 1000 );
	}
	TMtAlias      tMtH323Alias;
	TMtAlias      tMtE164Alias;
	TMtAlias      tMtAddr;
	TMtAlias      tConfAlias;
	u16           wLoop = 0;

	m_wSavedEvent = MT_MCU_CREATECONF_REQ;
		
	//终端别名
	tMtAddr.m_AliasType = (u8)mtAliasTypeTransportAddress;
	tMtAddr.m_tTransportAddr.SetIpAddr( ntohl(m_tIncomingCallParams.GetCallingIp()) );
    //zbq [08/07/2007] 终端用真实的端口呼入
    //tMtAddr.m_tTransportAddr.SetPort(MCU_Q931_PORT);
    tMtAddr.m_tTransportAddr.SetPort( m_tIncomingCallParams.GetCallingPort() );  
	
	for(  wLoop = 0; wLoop < (u16)type_others + 1; wLoop ++ )
	{
		if( m_tIncomingCallParams.GetCalledAliasAddr((u8)wLoop)->GetCallAddrType() == cat_alias )
		{
			if( m_tIncomingCallParams.GetCalledAliasAddr((u8)wLoop)->GetAliasType() == type_e164 || 
				m_tIncomingCallParams.GetCalledAliasAddr((u8)wLoop)->GetAliasType() == type_partyNumber )
			{
				tConfAlias.SetE164Alias( m_tIncomingCallParams.GetCalledAliasAddr((u8)wLoop)->GetAliasName() );
				break;				
			}
			else if ( m_tIncomingCallParams.GetCalledAliasAddr((u8)wLoop)->GetAliasType() == type_h323ID)
			{
				tConfAlias.SetH323Alias( m_tIncomingCallParams.GetCalledAliasAddr((u8)wLoop)->GetAliasName() );
				break;
			}
		}
		else if( m_tIncomingCallParams.GetCalledAliasAddr((u8)wLoop)->GetCallAddrType() == cat_ip )
		{
            tConfAlias.m_AliasType = (u8)mtAliasTypeTransportAddress;
			tConfAlias.m_tTransportAddr.SetIpAddr( ntohl( m_tIncomingCallParams.GetCalledAliasAddr((u8)wLoop)->GetIPAddr()));
            tConfAlias.m_tTransportAddr.SetPort( m_tIncomingCallParams.GetCalledAliasAddr((u8)wLoop)->GetIPPort() );
			break;
		}
	}

	for( wLoop = 0; wLoop < (u16)type_others + 1; wLoop ++ )
	{
		if( m_tIncomingCallParams.GetCallingAliasaddr((u8)wLoop)->GetCallAddrType() == cat_alias )
		{
			if( m_tIncomingCallParams.GetCallingAliasaddr((u8)wLoop)->GetAliasType() == type_h323ID )
				tMtH323Alias.SetH323Alias(m_tIncomingCallParams.GetCallingAliasaddr((u8)wLoop)->GetAliasName() );
			else if( m_tIncomingCallParams.GetCallingAliasaddr((u8)wLoop)->GetAliasType() == type_e164 )
				tMtE164Alias.SetE164Alias(m_tIncomingCallParams.GetCallingAliasaddr((u8)wLoop)->GetAliasName() );
		}				
	}
	
	cServMsg.SetMsgBody( (u8*)&tMtH323Alias, sizeof(tMtH323Alias) );
	cServMsg.CatMsgBody( (u8*)&tMtE164Alias, sizeof(tMtE164Alias) );
	cServMsg.CatMsgBody( (u8*)&tMtAddr, sizeof(tMtAddr) );
	cServMsg.CatMsgBody( (u8*)&tConfAlias, sizeof(tConfAlias) );

	u8 byType = 0;
	switch( m_emEndpointType ) 
	{
		case emEndpointTypeMCU:	byType = TYPE_MCU;	break;
		case emEndpointTypeMT:	byType = TYPE_MT;	break;
		default:                byType = TYPE_MT;   break;
	}
	cServMsg.CatMsgBody( (u8*)&byType, 1 );
	
	u8 byEncrypt = (u8)m_tIncomingCallParams.IsEncrypt();
	cServMsg.CatMsgBody( &byEncrypt, sizeof(byEncrypt) );
	
	//增加呼叫速率上报
	u16 wCallRate = (u16)m_tIncomingCallParams.GetCallRate();
	cServMsg.CatMsgBody( (u8*)&wCallRate, sizeof(u16) );

	//增加厂商ID，使外厂商MCU呼叫kedaMCU时放过VCS(Mcs)会议的校验[4/6/2012 chendaiwei]
	cServMsg.CatMsgBody((u8*)&m_byVendorId,sizeof(m_byVendorId));

    if( m_emEndpointType == emEndpointTypeMT && m_byVendorId == MT_MANU_KDC )
    {
        u8 *pbyNonData = NULL;
        s32 nLen = 0;
        m_tIncomingCallParams.GetNonStandData(&pbyNonData, &nLen);

        //按3.6终端的结构取非标数据
        if ( m_byMtVer == (u8)emMtVer36 )
        {
            if( (u32)nLen == sizeof(TConfInfoVer36) && NULL != pbyNonData)	
            {
				// 多国语言支持 [pengguofeng 4/26/2013]
				TConfInfoVer36 tVcConfInfo;
				memcpy(&tVcConfInfo, pbyNonData, sizeof(TConfInfoVer36));
				s8 achConfName[MAXLEN_CONFNAME_VER36+1];
				memset(achConfName, 0, sizeof(achConfName));
				BOOL32 bNeedTrans = TransEncoding(tVcConfInfo.GetConfName(), achConfName, sizeof(achConfName), FALSE);
				if ( bNeedTrans )
				{
					tVcConfInfo.SetConfName(achConfName);
				}

				s8 achOldKey[MAXLEN_KEY];
				memset(achOldKey, 0, sizeof(achOldKey));
				s32 byOldKeyLen = 0;
				TMediaEncryptVer36 tMediaKey = tVcConfInfo.GetMediaKey();
				tMediaKey.GetEncryptKey((u8*)achOldKey, &byOldKeyLen);
				s8 achNewKey[MAXLEN_KEY];
				memset(achNewKey, 0, sizeof(achNewKey));
// 				u8 byNewKeyLen = 0;
				bNeedTrans = TransEncoding(achOldKey, achNewKey, MAXLEN_KEY, FALSE);
				if ( bNeedTrans )
				{
					tMediaKey.SetEncryptKey((u8*)achNewKey, strlen(achNewKey));
					tVcConfInfo.SetMediaKey(tMediaKey);
				}
                cServMsg.CatMsgBody( (u8*)&tVcConfInfo, sizeof(TConfInfoVer36) );
            }
        }
        else
        {
            // guzh [7/27/2006] 4.0 要同时兼容 R2 和 R3 及以后版本
            // 所以转换为当前版本的TConfInfo结构
            // xliang [2/25/2009] 目前最高4.6，要兼容4.5，4.0
            if (nLen > 0 && NULL != pbyNonData)
            {
                TConfInfo tVcConfInfo;
                if (m_byMtVer == (u8)emMtVer40R5) //4.5->4.6
                {
                    TConfInfoV4R5 tConfInfoV4R5;
                    memcpy( &tConfInfoV4R5, pbyNonData,
                        min((u32)nLen, sizeof(TConfInfoV4R5)) );
                    CStructConvert::ConfInfoV4R5ToV4R6( tVcConfInfo, tConfInfoV4R5 );
                    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "TConfInfo change: 4.5->4.6!\n");
                }
                else if(m_byMtVer == (u8)emMtVer40R4)	//4.0->4.6
                {
                    TConfInfoV4R4B2 tConfInfoV4R4;
                    memcpy( &tConfInfoV4R4, pbyNonData, 
                        min((u32)nLen, sizeof(TConfInfoV4R4B2)) );
                    CStructConvert::ConfInfoV4R4ToV4R6( tVcConfInfo, tConfInfoV4R4 );
                    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "TConfInfo change: 4.0->4.6!\n");
                }
                else //m_byMtVer == emMtVer40R6不转
                {
                    memcpy( &tVcConfInfo, 
                        pbyNonData,
                        min((u32)nLen, sizeof(TConfInfo)) );
                    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "TConfInfo change: No change!\n");
                }
                
				// 从非标内容里解析编码方式 [pengguofeng 4/26/2013]
                // TConfInfo之后数据块格式：
                // u8（TConfInfo后追加数据块个数） + 
                // + u16（第一块长度）+ u8（AduioType）+ u8（声道数） /*需要发送到MCU*/
                // + TNonStandardContentHead + 数据内容 [add by liaokang]
                // + ... ...(依次顺序追加其他数据块)
				u8 byContentNum = 0;
				if ( (u32)nLen > sizeof(TConfInfo))
				{
					byContentNum = *(pbyNonData+sizeof(TConfInfo));
				}

                u8 byEndPointEncoding = emenCoding_GBK; //默认GBK
                s8 achConfName[MAXLEN_CONFNAME+1] = {0};
				s8 achNewKey[MAXLEN_KEY] = {0};
                u8 * pbyContent = pbyNonData+sizeof(TConfInfo)+sizeof(u8)*3+sizeof(u16);
                while( byContentNum > 1 )
                {
                    u16 wLength = 0;
                    TNonStandardContentHead tContentHead = *(TNonStandardContentHead *)(pbyContent);
                    pbyContent += sizeof(TNonStandardContentHead);
                    wLength = tContentHead.m_wContentLen;
                    
                    switch(tContentHead.m_dwContentType)
                    {
                    case TNonStandardContentHead::emEncoding:
                        {
                            byEndPointEncoding = *(u8 *)pbyContent;
                        }
                        break;
                    case TNonStandardContentHead::emConfName:
                        {
                            memcpy(achConfName, pbyContent, min(sizeof(achConfName), tContentHead.m_wContentLen));
                        }
                        break;
                    case TNonStandardContentHead::emEncryptKey:
                        {
                            memcpy(achNewKey, pbyContent, min(sizeof(achNewKey), tContentHead.m_wContentLen));
                        }
                        break;
                    default:
                        break;
                    }
                    pbyContent += wLength;
                    byContentNum--;
                }
                
                SetEndPointEncoding((emenCodingForm)byEndPointEncoding);

                // 转码 [pengguofeng 4/26/2013]
                BOOL32 bNeedTrans = TransEncoding(tVcConfInfo.GetConfName(), achConfName, sizeof(achConfName), FALSE);
                if ( bNeedTrans || 
                    (emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() && emenCoding_Utf8 == GetEndPointEncoding()) )
                {
                    tVcConfInfo.SetConfName(achConfName);
                }
 
                s8 achOldKey[MAXLEN_KEY] = {0};
                s32 byOldKeyLen = 0;
                TMediaEncrypt tMediaEncrypt = tVcConfInfo.GetMediaKey();
                tMediaEncrypt.GetEncryptKey((u8*)achOldKey, &byOldKeyLen);
                bNeedTrans = TransEncoding(achOldKey, achNewKey, MAXLEN_KEY, FALSE);
                if ( bNeedTrans || 
                    (emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() && emenCoding_Utf8 == GetEndPointEncoding()) )
                {
                    tMediaEncrypt.SetEncryptKey((u8*)achNewKey, strlen(achNewKey));
                    tVcConfInfo.SetMediaKey(tMediaEncrypt);
				}
      
                cServMsg.CatMsgBody( (u8*)&tVcConfInfo, sizeof(TConfInfo) );
				if(m_byMtVer != (u8)emMtVer40R5 && 
					m_byMtVer != (u8)emMtVer40R4 &&
					(u32)nLen > sizeof(TConfInfo))
				{
					cServMsg.CatMsgBody( (u8*)pbyNonData+sizeof(TConfInfo), (u32)nLen - sizeof(TConfInfo) );
				}
            }
        }
    }

	//按3.6终端的结构取非标数据
// 	if ( m_byMtVer == (u8)emMtVer36 )
// 	{
// 		if( (u32)m_nNonStandConfInfoDataLen == sizeof(TConfInfoVer36) )	
// 		{
// 			cServMsg.CatMsgBody( m_pbyNonStandConfInfoData36, sizeof(TConfInfoVer36) );
// 		}
// 		m_nNonStandConfInfoDataLen = 0;
// 	}
// 	else
// 	{
//         // guzh [7/27/2006] 4.0 要同时兼容 R2 和 R3 及以后版本
//         // 所以转换为当前版本的TConfInfo结构
// 		// xliang [2/25/2009] 目前最高4.6，要兼容4.5，4.0
//         if (m_nNonStandConfInfoDataLen > 0)
//         {
// 			TConfInfo tVcConfInfo;
//             if (m_byMtVer == (u8)emMtVer40R5) //4.5->4.6
//             {
// 				TConfInfoV4R5 tConfInfoV4R5;
// 				memcpy( &tConfInfoV4R5, m_pbyNonStandConfInfoData,
// 					min((u32)m_nNonStandConfInfoDataLen, sizeof(TConfInfoV4R5)) );
// 				CStructConvert::ConfInfoV4R5ToV4R6( tVcConfInfo, tConfInfoV4R5 );
// 				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "TConfInfo change: 4.5->4.6!\n");
//             }
// 			else if(m_byMtVer == (u8)emMtVer40R4)	//4.0->4.6
// 			{
// 				TConfInfoV4R4B2 tConfInfoV4R4;
// 				memcpy( &tConfInfoV4R4, m_pbyNonStandConfInfoData, 
// 					min((u32)m_nNonStandConfInfoDataLen, sizeof(TConfInfoV4R4B2)) );
// 				CStructConvert::ConfInfoV4R4ToV4R6( tVcConfInfo, tConfInfoV4R4 );
// 				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "TConfInfo change: 4.0->4.6!\n");
// 			}
// 			else //m_byMtVer == emMtVer40R6不转
// 			{
// 				memcpy( &tVcConfInfo, 
//                     m_pbyNonStandConfInfoData,
//                     min((u32)m_nNonStandConfInfoDataLen, sizeof(TConfInfo)) );
// 				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "TConfInfo change: No change!\n");
// 			}
// 
// /*			TConfInfo tVcConfInfo;
//             memcpy( &tVcConfInfo, 
//                     m_pbyNonStandConfInfoData,
//                     min(m_nNonStandConfInfoDataLen, sizeof(TConfInfo)) );
// */
//             cServMsg.CatMsgBody( (u8*)&tVcConfInfo, sizeof(TConfInfo) );
// 
//         }
// 		m_nNonStandConfInfoDataLen = 0;
// 	}


	u8 abyConfId[16];
	m_tIncomingCallParams.GetConfId( abyConfId, sizeof(abyConfId) );
	m_cConfId.SetConfId( abyConfId, sizeof(abyConfId) );

    if (bLowerMcuCallingIn)
    {
        cServMsg.SetEventId( MT_MCU_LOWERCALLEDIN_NTF );	
    }
    else
	{
		cServMsg.SetEventId( m_wSavedEvent );
	}

	cServMsg.SetSrcSsnId( (u8)GetInsID() ); 					
	SendMsgToMcu( cServMsg );

	SetTimer( WAIT_RESPONSE_TIMEOUT, CALLING_TIMEOUT * 1000 );
	CHANGE_STATE( STATE_CALLING );

    // guzh [6/19/2007] 通知后直接挂断，等待本端发起呼叫
    if (bLowerMcuCallingIn)
    {
        ClearInst(0, (u8)emDisconnectReasonRemoteReconnect, 1);
    }	
	return;
}


/*=============================================================================
  函 数 名： ProcRadEvChanCtrl
  功    能： 处理协议栈传来的的信道控制事件
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0			TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcRadEvChanCtrl(CMessage * const pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	if(!BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(HCHAN) + sizeof(u16))) 
		return;

	//HCALL pStackCall = *(HCALL*)cServMsg.GetMsgBody();	
	HCHAN pStackChan = *(HCHAN*)(cServMsg.GetMsgBody() + sizeof(HCALL));	
	u16   wMsgType   = *(u16*)(cServMsg.GetMsgBody() + sizeof(HCALL) + sizeof(HCHAN));		
	u8   *pbyMsg     =  (u8*)(cServMsg.GetMsgBody() + sizeof(HCALL) + sizeof(HCHAN) + sizeof(wMsgType));

	if( CurState() == STATE_IDLE )
		return;

	s32 nChanIdx = FindChannel( pStackChan );

	//this item might be deleted before as a channel drop may
	//produce multiple h_chan_release events
	if( nChanIdx < 0 ) 	
	{
		return;
	}
	
	switch(wMsgType)
	{
	//对端打开逻辑通道参数通知, 在此取得对端的地址和其他通道参数(本通知在h_chan_open之前到来)
	case h_chan_parameter:
		{	
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(HCHAN) + sizeof(wMsgType) + sizeof(TCHANPARAM))) 
				 return;
			
			TCHANPARAM tChan = *(TCHANPARAM*)pbyMsg;

			u8 byPayload, byMediaType;
			
			CMtAdpUtils::PayloadTypeOut2In( (u8)tChan.GetPayloadType(), byPayload, byMediaType );

            s8  achChanName[32] = {0};
            MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[h_chan_parameter] Income chan 0x%p Type: %s(%u)\n", pStackChan,
				          CMtAdpUtils::PszGetChannelName( byPayload, achChanName, sizeof(achChanName) ),
                          byPayload);

			//发现是否已有通道
            BOOL32 bDual = FALSE;
			if( MODE_VIDEO == byMediaType && FindChannelByMediaType( (u8)CHAN_INCOMING, MODE_VIDEO ) >= 0 )
			{
				m_atChannel[nChanIdx].tLogicChan.SetMediaType( MODE_SECVIDEO );
                bDual = TRUE;
			}
			else
			{
				m_atChannel[nChanIdx].tLogicChan.SetMediaType( byMediaType );
			}

            m_atChannel[nChanIdx].tLogicChan.SetChannelType( byPayload );
            m_atChannel[nChanIdx].tLogicChan.SetSupportH239( tChan.m_bIsH239DStream );
            m_atChannel[nChanIdx].tLogicChan.SetActivePayload( tChan.GetDynamicPayloadParam() );
            m_atChannel[nChanIdx].tLogicChan.SetFlowControl( 0 );

            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[h_chan_parameter] Type.%u, H239.%d, ActivePT.%u\n", byPayload, tChan.m_bIsH239DStream, tChan.GetDynamicPayloadParam() );

            //取视频通道分辨率、帧率、码率
            if ( MODE_VIDEO == byMediaType )
            {
                BOOL bAccept = TRUE;
                u8 byRes;
                u8 byFps;
                if ( MEDIA_TYPE_H264 != byPayload )
                {
                    bAccept = GetCapVideoParam( tChan.m_tVideoCap, bDual, byRes, byFps );
                    m_atChannel[nChanIdx].tLogicChan.SetFlowControl( (u16)(tChan.m_tVideoCap.GetBitRate() ) );
                }
                else
                {
                    bAccept = GetCapVideoParam( tChan.m_tH264Cap, bDual, byRes, byFps );
                    m_atChannel[nChanIdx].tLogicChan.SetFlowControl( (u16)(tChan.m_tH264Cap.GetBitRate() ) );
                    //m_atChannel[nChanIdx].tLogicChan.SetProfieAttrb((emProfileAttrb)tChan.m_tH264Cap.GetProfileValue());
					if( tChan.m_tH264Cap.GetProfileValue() & (u8)TH264VideoCap::emProfileHigh )
					{
						m_atChannel[nChanIdx].tLogicChan.SetProfieAttrb( emHpAttrb );
					}
					else
					{
						m_atChannel[nChanIdx].tLogicChan.SetProfieAttrb( emBpAttrb );
					}					
                }

                if ( !bAccept )
                {
                    // just clear
                    m_atChannel[nChanIdx].tLogicChan.SetChannelType( MEDIA_TYPE_NULL );
                    MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[h_chan_parameter] Vid Type.%u not supported!\n", byPayload );
                }
                else
                {
                    m_atChannel[nChanIdx].tLogicChan.SetVideoFormat( byRes );
                    m_atChannel[nChanIdx].tLogicChan.SetChanVidFPS( byFps );                    

                    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[h_chan_parameter] Type.%u Res.%d, Fps.%d (Dual:%d), Bitrate.%u!\n", 
                               byPayload, 
                               byRes, byFps, bDual,
                               m_atChannel[nChanIdx].tLogicChan.GetFlowControl() );
                }                
            }
			else if ( MODE_AUDIO == byMediaType )
			{
				BOOL bAccept = TRUE;
				/*增加对远端参数的合法性检查，看对端将要打开的音频格式及
				声道数本地是否支持，如果不支持，则不予理睬*/
				u8 byAudioTrackNum = 1;//默认单声道
				if ( MEDIA_TYPE_AACLC == byPayload || MEDIA_TYPE_AACLD == byPayload )
				{
					TAACCap tAACCap = tChan.m_tAacCap;
					if (96 != tAACCap.GetBitrate())
					{
						MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[h_chan_parameter] 96 != tAACCap.GetBitrate().%d!\n", tAACCap.GetBitrate());
						bAccept = FALSE;
					}
					else if (TAACCap::emFs32000 != tAACCap.GetSampleFreq())
					{
						MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[h_chan_parameter] emFs32000 != tAACCap.GetSampleFreq().%d!\n", tAACCap.GetSampleFreq());
						bAccept = FALSE;
					}
					else
					{
						TAACCap::emAACChnlCfg eChnnl = CMtAdpUtils::GetAACChnnNum(byAudioTrackNum);
						u8 byAudioIdx = 0;
						for ( byAudioIdx = 0 ;byAudioIdx < MAXNUM_CONF_AUDIOTYPE;++byAudioIdx )
						{
							eChnnl = CMtAdpUtils::GetAACChnnNum(m_atLocalAudioTypeDesc[byAudioIdx].GetAudioTrackNum());
							if (m_atLocalAudioTypeDesc[byAudioIdx].GetAudioMediaType() == byPayload &&
								tAACCap.GetChnl() == eChnnl)
							{
								byAudioTrackNum = m_atLocalAudioTypeDesc[byAudioIdx].GetAudioTrackNum();
								break;
							}
						}
						if (byAudioIdx >= MAXNUM_CONF_AUDIOTYPE)
						{
							MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[h_chan_parameter] byPayload[%d] tAACCap.GetChnl()[%d],No supported!!\n",byPayload, tAACCap.GetChnl());
							bAccept = FALSE;
						}
					}
				}
				if ( !bAccept )
				{
					// just clear
                    m_atChannel[nChanIdx].tLogicChan.SetChannelType( MEDIA_TYPE_NULL );
                    MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[h_chan_parameter] Aud Type.%u not supported!\n", byPayload );
				}
				else
				{
					m_atChannel[nChanIdx].tLogicChan.SetAudioTrackNum(byAudioTrackNum);
				}
			}

			//T120: 取对端的监听地址
			if ( (u16)d_t120 == tChan.GetPayloadType() )
			{
				m_atChannel[nChanIdx].tLogicChan.m_tRcvMediaChannel.SetIpAddr( ntohl( tChan.GetRtp().GetIPAddr() ) );
				m_atChannel[nChanIdx].tLogicChan.m_tRcvMediaChannel.SetPort( tChan.GetRtp().GetIPPort() );

                // 取T120通道的码率
                m_atChannel[nChanIdx].tLogicChan.SetFlowControl( (u16)(tChan.m_tDataCap.GetBitRate() ) );
                MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[h_chan_parameter] Data Cap Bitrate is %d\n", tChan.m_tDataCap.GetBitRate() );
			}

			//加密信息保存，只保存加密类型，其他信息由单独的消息通知
			TMediaEncrypt tEncryptMedia = m_atChannel[nChanIdx].tLogicChan.GetMediaEncrypt();
			tEncryptMedia.SetEncryptMode( CMtAdpUtils::EncryptTypeOut2In(tChan.GetEncryptType()) );
			m_atChannel[nChanIdx].tLogicChan.SetMediaEncrypt( tEncryptMedia );

			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[h_chan_parameter] Encrypt Mode from stack: %d\n", tChan.GetEncryptType() );

            // FECType, zgc, 2007-10-10
            m_atChannel[nChanIdx].tLogicChan.SetFECType( tChan.GetFECType() );
		} 
		break;

	//获得远端的RTP地址, 此处获得对端的ACK消息中的Rtp地址 NETADDR
	case h_chan_remoteRtpAddress: 
		
		if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(HCHAN)  + sizeof(wMsgType) + sizeof(TNETADDR) ) )
			return;
		
		if( m_atChannel[nChanIdx].byDirect == (u8)CHAN_OUTGOING )
		{
			TNETADDR tNetAddr = *(TNETADDR*)pbyMsg;			
			m_atChannel[nChanIdx].tLogicChan.m_tRcvMediaChannel.SetIpAddr( ntohl(tNetAddr.GetIPAddr() ));
			m_atChannel[nChanIdx].tLogicChan.m_tRcvMediaChannel.SetPort( tNetAddr.GetIPPort() );
		}
		
		if( m_atChannel[nChanIdx].tLogicChan.GetChannelType() == MEDIA_TYPE_H224 )
		{	
			TTERLABEL tTerSrc, tTerDst;
			tTerSrc.SetTerminalLabel( 0, 0 );
			tTerDst.SetTerminalLabel( 0, 0 );
					
			if( kdvFeccBind( m_atChannel[nChanIdx].hsChan, tTerSrc, tTerDst) != (s32)annexQOk )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Fecc bind error\n");				
			}
		}
		break;

	//获得远端的RTCP地址, 此处获得对端的ACK消息中的地址 NETADDR
	case h_chan_remoteRtcpAddress://get remote RTCP address	NETADDR				
		{
			if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(HCHAN) + sizeof(wMsgType) + sizeof(TNETADDR) ) ) 
				return;

 			TNETADDR tNetAddr = *(TNETADDR*)pbyMsg;

			if(m_atChannel[nChanIdx].byDirect == (u8)CHAN_OUTGOING)
			{
				m_atChannel[nChanIdx].tLogicChan.m_tRcvMediaCtrlChannel.SetIpAddr( ntohl(tNetAddr.GetIPAddr() ));
				m_atChannel[nChanIdx].tLogicChan.m_tRcvMediaCtrlChannel.SetPort( tNetAddr.GetIPPort() );				
			}
			else
			{
				m_atChannel[nChanIdx].tLogicChan.m_tSndMediaCtrlChannel.SetIpAddr( ntohl(tNetAddr.GetIPAddr() ));
				m_atChannel[nChanIdx].tLogicChan.m_tSndMediaCtrlChannel.SetPort( tNetAddr.GetIPPort() );
			}
		}
		break;

	//获得通道的动态载荷类型
	case h_chan_dynamicpayloadtype:
		{
			if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(HCHAN) + sizeof(wMsgType) + sizeof(u8) ) ) 
				return;  
			
			m_atChannel[nChanIdx].tLogicChan.SetActivePayload( *(u8*)pbyMsg );
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Rcv chanDPT callback, DPT is %d, Encrypt is %d\n", *(u8*)pbyMsg, m_byEncrypt);
		}
		break;

	//打开通道
	case h_chan_open:	
		{
            // guzh [9/4/2007] 在MCU回应后处理
            /*
            // guzh [9/3/2007] 首先检查是否支持该通道
            if ( m_atChannel[nChanIdx].tLogicChan.GetChannelType() == MEDIA_TYPE_G7221C &&
                 !IsChanG7221CSupport() )
            {
                kdvSendChanCtrlMsg(m_atChannel[nChanIdx].hsChan, h_chan_openReject, NULL, 0);
                return;
            }
            */

            //H460打开逻辑通道处理
            TTransportAddr tSndRtpAddr;
            tSndRtpAddr.SetNull();
            switch( m_atChannel[nChanIdx].tLogicChan.m_byMediaType )
            {
            case MODE_AUDIO:
                tSndRtpAddr.SetIpAddr(m_atAudSndRtpAddr.GetIp());
                tSndRtpAddr.SetPort(m_atAudSndRtpAddr.GetPort());
                break;
            case MODE_VIDEO:
                tSndRtpAddr.SetIpAddr(m_atVidSndRtpAddr.GetIp());
                tSndRtpAddr.SetPort(m_atVidSndRtpAddr.GetPort());
                break;
            case MODE_SECVIDEO:
                tSndRtpAddr.SetIpAddr(m_atSecSndRtpAddr.GetIp());
                tSndRtpAddr.SetPort(m_atSecSndRtpAddr.GetPort());
                break;
            default:
                break;
            }           

			m_wSavedEvent = MT_MCU_OPENLOGICCHNNL_REQ;	

			CServMsg cMsg;
			cMsg.SetEventId( MT_MCU_OPENLOGICCHNNL_REQ );
			cMsg.SetMsgBody( (u8*)&m_atChannel[nChanIdx].tLogicChan, sizeof(TLogicalChannel) );
            cMsg.CatMsgBody((u8 *)&m_bIsNeedPinHold, sizeof(BOOL32));
            cMsg.CatMsgBody((u8 *)&tSndRtpAddr, sizeof(TTransportAddr));
            
            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "h_chan_open SendMsgToMcu, SndRtpAddr(0x%x:%d) m_bIsNeedPinHold(%d)\n",
				tSndRtpAddr.m_dwIpAddr, tSndRtpAddr.m_wPort, m_bIsNeedPinHold );
		
			SendMsgToMcu( cMsg );

			m_atChannel[nChanIdx].byState = (u8)CHAN_OPENING;
			SetTimer(WAIT_RESPONSE_TIMEOUT, CALLING_TIMEOUT * 1000);			
		}
		break;
		
	case h_chan_openAck:
		break;
    case h_chan_openReject:			  //拒绝打开逻辑通道
        {
            CServMsg cMsg;
            cMsg.SetEventId( MT_MCU_OPENLOGICCHNNL_NACK );
            cMsg.SetMsgBody( (u8*)&m_atChannel[nChanIdx].tLogicChan, sizeof(TLogicalChannel) );
            
			SendMsgToMcu( cMsg );

        }
        break;
        

	//打开通道成功
	case h_chan_connected:		 
		{
			if( (u8)CHAN_CONNECTED == m_atChannel[nChanIdx].byState )
			{
				return;
			}

			if( m_atChannel[nChanIdx].byDirect == (u8)CHAN_OUTGOING )
			{	
				CServMsg cMsg;
				cMsg.SetEventId(MT_MCU_OPENLOGICCHNNL_ACK);
				cMsg.SetMsgBody((u8*)&m_atChannel[nChanIdx].tLogicChan, sizeof(m_atChannel[nChanIdx].tLogicChan));
				cMsg.CatMsgBody((u8*)&m_bIsNeedPinHold, sizeof(m_bIsNeedPinHold));
				SendMsgToMcu(cMsg);
				//BuildAndSendMsgToMcu( MT_MCU_OPENLOGICCHNNL_ACK, (u8*)&m_atChannel[nChanIdx].tLogicChan, sizeof(TLogicalChannel) );
				m_atChannel[nChanIdx].byState = (u8)CHAN_CONNECTED;
				kdvSendChanCtrlMsg( m_atChannel[nChanIdx].hsChan, (u16)h_chan_on, NULL, 0 );
			}
			else
			{
				m_atChannel[nChanIdx].byState = (u8)CHAN_CONNECTED;
				BuildAndSendMsgToMcu( MT_MCU_LOGICCHNNLOPENED_NTF, (u8*)&m_atChannel[nChanIdx].tLogicChan, sizeof(TLogicalChannel) );
			}

			if( m_atChannel[nChanIdx].tLogicChan.GetChannelType() == MEDIA_TYPE_MMCU )
			{
				// [6/14/2011 xliang] mmcu send H_CASCADE_REGUNREG_REQ when h_chan_connected callback; 
				// while smcu send H_CASCADE_REGUNREG_REQ after receiving H_CASCADE_REGUNREG_REQ from mmcu
				if(m_atChannel[nChanIdx].byDirect == (u8)CHAN_OUTGOING)
				{
					//outgoing means it's mmcu receive h_chan_connected
					TRegUnRegReq tReq;
					memset( &tReq, 0, sizeof(tReq) );
					
					//上级mcu发送注册请求时设置会议密码，以便进行呼叫认证 2005-11-02
					if ( m_bMaster && 0 != strlen(m_achConfPwd) )
					{
						tReq.SetUserNameAndPass("rvmcu", m_achConfPwd);
					}
					tReq.m_nReqID = m_dwMMcuReqId ++ ;
					tReq.m_bReg = TRUE;
					
					//是否支持多级联// [7/9/2010 xliang] 根据具体情况赋值,暂置TRUE
					tReq.m_bIsSupportMuitiCasade = TRUE;
					
					kdvSendMMcuCtrlMsg( m_atChannel[nChanIdx].hsChan, (u16)H_CASCADE_REGUNREG_REQ, &tReq, sizeof(tReq), FALSE );
				}
			}

		
			if ( MEDIA_TYPE_H224 == m_atChannel[nChanIdx].tLogicChan.GetChannelType()
				&& MODE_DATA == m_atChannel[nChanIdx].tLogicChan.GetMediaType())
			{
				// [2013/05/27 chenbing] 判断H224数据信道后向通道是否需要打洞
				if( (u8)CHAN_INCOMING == m_atChannel[nChanIdx].byDirect && m_bIsNeedPinHold )
				{
					TGwPinholeInfo tInfo;

					// 本地地址
					tInfo.m_dwLocalIP     = m_atChannel[nChanIdx].tLogicChan.m_tRcvMediaChannel.m_dwIpAddr;
					// 本地端口
					tInfo.m_wLocalPort    = m_atChannel[nChanIdx].tLogicChan.m_tRcvMediaChannel.GetPort();
					// 远端地址
					tInfo.m_dwDestIP      = m_atH224SndRtpAddr.m_dwIP;
					// 远端端口
					tInfo.m_wDestPort     = m_atH224SndRtpAddr.m_wPort;
					// 动态载荷
					tInfo.m_byPayLoad     = MEDIA_TYPE_H224;
					// RTP打洞类型
					tInfo.m_byPinholeType = pinhole_rtp;
					// 调用打洞接口
					BOOL32 bRet = KdvTSAddPinhole(tInfo, g_cMtAdpApp.m_dwPinHoleInterval, pinhole_rtp);
					MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "\n[h_chan_connected]---H460---KdvTSAddPinhole(return: %d) MediaType(%d) Src(%u.%u.%u.%u:%d) Dst(%u.%u.%u.%u:%d)\n",
						bRet, tInfo.m_byPayLoad, QUADADDR(tInfo.m_dwLocalIP), tInfo.m_wLocalPort, QUADADDR(tInfo.m_dwDestIP), tInfo.m_wDestPort);
				}
			}
		}
		break;

	//通道加密模式
	case h_chan_EncryptSyn:
		{
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(HCHAN) + sizeof(wMsgType) + sizeof(TEncryptSync))) 
				return;
			
			TEncryptSync *ptEncryptSync = (TEncryptSync *)pbyMsg;
			TMediaEncrypt tEncryptMedia = m_atChannel[nChanIdx].tLogicChan.GetMediaEncrypt();
			tEncryptMedia.SetEncryptKey( ptEncryptSync->GetH235Key(), ptEncryptSync->GetH235KeyLen() );
			m_atChannel[nChanIdx].tLogicChan.SetMediaEncrypt( tEncryptMedia );
			m_atChannel[nChanIdx].tLogicChan.SetActivePayload( (u8)ptEncryptSync->GetSynchFlag() );
			
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Rcv ChanEncryptSyn callback, DPT is %d\n", ptEncryptSync->GetSynchFlag());
			
			u8	abyKey[LEN_H235KEY_MAX] = { 0 };
			
			memcpy( abyKey, ptEncryptSync->GetH235Key(), (u32)ptEncryptSync->GetH235KeyLen() );
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Rcv H235Key is %2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x\n", 
				abyKey[0], abyKey[1], abyKey[2],  abyKey[3],  abyKey[4],  abyKey[5],  abyKey[6],  abyKey[7], 
				abyKey[8], abyKey[9], abyKey[10], abyKey[11], abyKey[12], abyKey[13], abyKey[14], abyKey[15]);
		}
		break;

	//冻结图像
	case h_chan_VideoFreezePicture:			  //video command freeze		NULL
		BuildAndSendMsgToMcu( MT_MCU_FREEZEPIC_CMD );		
		break;

	//快速图像更新
	case h_chan_videofastupdatePIC:              //video command update		NULL		
		{
			u8 byMode = m_atChannel[nChanIdx].tLogicChan.GetMediaType();
			MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "Receive key frame REQ,the stackchan is: %x,   the media type is: %d\n",pStackChan,byMode); //test [080730]
			BuildAndSendMsgToMcu( MT_MCU_FASTUPDATEPIC_CMD, &byMode, sizeof(u8) );	
			break;
		}

	//终端要求改变接收码率 - 码流控制命令
	case h_chan_flowControl: 
		{
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(HCHAN) + sizeof(wMsgType) + sizeof(u16)))
				return;

			u16 wRate = *(u16*)pbyMsg;
			TLogicalChannel tChan = m_atChannel[nChanIdx].tLogicChan;			
			tChan.SetFlowControl(wRate);
			
			MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcRadEvChanCtrl] FlowCommand: byMtId.%d MediaType.%d wCallRate.%d \n", m_byMtId, tChan.GetMediaType(), wRate );

			BuildAndSendMsgToMcu( MT_MCU_FLOWCONTROL_CMD, (u8*)&tChan, sizeof(tChan) );
			
			// [12/11/2009 xliang] send flow control indication when receiving flow control cmd due to aethra MT
			if (m_byVendorId == MT_MANU_AETHRA)
			{
				MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcRadEvChanCtrl] send flow control indication when receiving flow control cmd due to aethra MT.%d\n",
					m_byMtId );

				kdvSendChanCtrlMsg(m_atChannel[nChanIdx].hsChan, (u16)h_chan_flowControlIndication, &wRate, sizeof(wRate));
			}
		}
		break;

	//终端要求改变发送码率 - 码流控制指示
	case h_chan_flowControlIndication: 
		{
			if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) +	sizeof(HCHAN) + sizeof(wMsgType) + sizeof(u16) ) ) 
				return;

			u16 wRate = *(u16*)pbyMsg;
			TLogicalChannel tChan = m_atChannel[nChanIdx].tLogicChan;			
			tChan.SetFlowControl( wRate );
			
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcRadEvChanCtrl] FlowIndication: byMtId.%d MediaType.%d wCallRate.%d \n", m_byMtId, tChan.GetMediaType(), wRate );

			BuildAndSendMsgToMcu( MT_MCU_FLOWCONTROLINDICATION_NOTIF, (u8*)&tChan, sizeof(tChan) );
		}
		break;
		
	//释放通道
	case  h_chan_close:		  
		{
			cServMsg.SetEventId( MT_MCU_CLOSELOGICCHNNL_NOTIF );
			cServMsg.SetMsgBody( (u8*)&m_atChannel[nChanIdx].tLogicChan, sizeof(TLogicalChannel) );
			u8 byOut = GETBBYTE( m_atChannel[nChanIdx].byDirect == (u8)CHAN_OUTGOING );
			cServMsg.CatMsgBody( (u8 *)&byOut, sizeof(u8) );
			SendMsgToMcu( cServMsg );

			// [2013/05/28 chenbing] 对H224数据信道后向打洞的进行补洞
			if ( MODE_DATA == m_atChannel[nChanIdx].tLogicChan.GetMediaType()
				&& MEDIA_TYPE_H224 == m_atChannel[nChanIdx].tLogicChan.GetChannelType()
				&& (u8)CHAN_INCOMING == m_atChannel[nChanIdx].byDirect
				&& m_bIsNeedPinHold
			   )
			{
				BOOL32 bRet = KdvTSDelPinhole(m_atChannel[nChanIdx].tLogicChan.m_tRcvMediaChannel.GetPort());
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcRadEvChanCtrl]:h_chan_close---H460---KdvTSDelPinHole(return: %d) Src(%u.%u.%u.%u:%d) MediaType(%d)!!!\n",
					bRet, QUADADDR(m_atChannel[nChanIdx].tLogicChan.m_tRcvMediaChannel.m_dwIpAddr), 
					m_atChannel[nChanIdx].tLogicChan.GetChannelType(), m_atChannel[nChanIdx].tLogicChan.m_tRcvMediaChannel.GetPort());
			}

			FreeChannel(  nChanIdx );

			// xliang [12/19/2008] print
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcRadEvChanCtrl]:h_chan_close---byOut is %u,nChanIdx is %u.\n",byOut,nChanIdx);
		}
		break;

		//终端远端环回请求, 在终端发送通道发送
	case h_chan_MediaLoopRequest:
		if (m_atChannel[nChanIdx].byDirect == (u8)CHAN_INCOMING)
		{
			BuildAndSendMsgToMcu( MT_MCU_MEDIALOOPON_REQ, (u8*)&m_atChannel[nChanIdx].tLogicChan, sizeof(TLogicalChannel) );						
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcRadEvChanCtrl] receive mt<%d> chanidx<%d> medialoop request.\n", 
					   m_byMtId, nChanIdx);
		}
		else
		{			
			kdvSendChanCtrlMsg(m_atChannel[nChanIdx].hsChan, (u16)h_chan_MediaLoopReject, NULL, 0);
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcRadEvChanCtrl] receive mt<%d> chanidx<%d> medialoop, should be outgoing. reject.\n",
					   m_byMtId, nChanIdx);
		}
		break;

		//终端远端环回结束请求, 在终端接收通道发送
	case h_chan_MediaLoopOff:
		if (m_atChannel[nChanIdx].byDirect == (u8)CHAN_INCOMING)
		{
			BuildAndSendMsgToMcu( MT_MCU_MEDIALOOPOFF_CMD, (u8*)&m_atChannel[nChanIdx].tLogicChan, sizeof(TLogicalChannel) );			
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcRadEvChanCtrl] receive mt<%d> chanidx<%d> medialoop off.\n", m_byMtId, nChanIdx);
		}
		else
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcRadEvChanCtrl] receive mt<%d> chanidx<%d> medialoopoff, should be outgoing. ignore.\n", 
						m_byMtId, nChanIdx);
		}
		break;

    case h_chan_keepAliveAddress:   //对端RTP发送地址
        {

            if( m_atChannel[nChanIdx].byDirect == (u8)CHAN_INCOMING )
            {
                TKeepAliveInfo tKeepAliveInfo = *(TKeepAliveInfo*)pbyMsg;
                switch( m_atChannel[nChanIdx].tLogicChan.m_byMediaType )
                {
                case MODE_AUDIO:
                    m_atAudSndRtpAddr.SetIp(ntohl(tKeepAliveInfo.m_tKeepAliveNet.m_dwIP));
                    m_atAudSndRtpAddr.SetPort(ntohs(tKeepAliveInfo.m_tKeepAliveNet.m_wPort));
                    break;
                case MODE_VIDEO:
                    m_atVidSndRtpAddr.SetIp(ntohl(tKeepAliveInfo.m_tKeepAliveNet.m_dwIP));
                    m_atVidSndRtpAddr.SetPort(ntohs(tKeepAliveInfo.m_tKeepAliveNet.m_wPort));
                    break;
                case MODE_SECVIDEO:
                    m_atSecSndRtpAddr.SetIp(ntohl(tKeepAliveInfo.m_tKeepAliveNet.m_dwIP));
                    m_atSecSndRtpAddr.SetPort(ntohs(tKeepAliveInfo.m_tKeepAliveNet.m_wPort));
                    break;
				case MODE_DATA:
					{
						if ( MEDIA_TYPE_H224 == m_atChannel[nChanIdx].tLogicChan.GetChannelType() )
						{
							m_atH224SndRtpAddr.SetIp(ntohl(tKeepAliveInfo.m_tKeepAliveNet.m_dwIP));
							m_atH224SndRtpAddr.SetPort(ntohs(tKeepAliveInfo.m_tKeepAliveNet.m_wPort));
						}
						else
						{
							//非H224的设置
						}
					}
					break;
                default:
                    break;
                } 
                
                m_atSecSndRtpAddr.m_byChlIndex = nChanIdx;
                
                MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, 
                    "[ProcRadEvChanCtrl] h_chan_keepAliveAddress mt:%d, MediaType:%d, SendRtpAddr<%u.%u.%u.%u:%d> ChanlId<%d>\n", 
                    m_byMtId, 
                    m_atChannel[nChanIdx].tLogicChan.m_byMediaType, 
                    QUADADDR(tKeepAliveInfo.m_tKeepAliveNet.m_dwIP), tKeepAliveInfo.m_tKeepAliveNet.m_wPort, 
                nChanIdx);
            }            
        }
		break;

	default:
		break;		
	}

	return;
}

/*=============================================================================
  函 数 名： ProcRadEvConfCtrl
  功    能： 处理协议栈传来的的会议控制事件
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0			TanGuang                创建
  2005/12/28    4.0			张宝卿				  增加终端外置矩阵消息
  2005/12/30	4.0			张宝卿				  调整终端内置矩阵消息
  2006/01/04	4.0			张宝卿				  兼容3.6终端
=============================================================================*/
void CMtAdpInst::ProcRadEvConfCtrl(CMessage * const pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	if (!BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(u16)))
		return;
	HCALL hsCall = *(HCALL*)cServMsg.GetMsgBody(); 
	u16 wMsgType = *(u16*)(cServMsg.GetMsgBody() + sizeof(HCALL));
	u8 *pbyMsg   =  (u8*)(cServMsg.GetMsgBody() + sizeof(HCALL) + sizeof(wMsgType));
	u16 wMsgLen  =  cServMsg.GetMsgBodyLen() - sizeof(HCALL) - sizeof(wMsgType);
	u8  byResult;

	if( CurState() == STATE_IDLE )
		return;


	/*lint -save -e578*/
	switch( wMsgType )
	{
	case h_ctrl_chimeInReq:				  //终端申请插话
		BuildAndSendMsgToMcu( MT_MCU_APPLYMIX_NOTIF );
		break;

	case h_ctrl_makeTerChimeInCmd:		 //主席指定终端插话
		{
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TTERLABEL)) )
				return;

			PTTERLABEL pter = (PTTERLABEL)pbyMsg;
			TMt tMt;
			//tMt.SetMt( pter->GetMcuNo(), pter->GetTerNo(), 0, m_byConfIdx );
			CMtAdpUtils::ConverTTERLABEL2TMt( *pter, tMt, m_byConfIdx );

			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "recved addmixmember -- <%d,%d>\n", pter->GetMcuNo(),pter->GetTerNo() );
			BuildAndSendMsgToMcu( MT_MCU_ADDMIXMEMBER_CMD, (u8*)&tMt, sizeof(tMt) );
		}
		break;
		
	case h_ctrl_removeMixMemberCmd: // [11/25/2010 xliang] 移除混音成员
		{
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TTERLABEL)) )
				return;
			
			PTTERLABEL pter = (PTTERLABEL)pbyMsg;
			TMt tMt;
			//tMt.SetMt( pter->GetMcuNo(), pter->GetTerNo(), 0, m_byConfIdx );
			CMtAdpUtils::ConverTTERLABEL2TMt( *pter, tMt, m_byConfIdx );
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "recved remove mixmember -- <%d,%d>\n", pter->GetMcuNo(),pter->GetTerNo() );
			
			BuildAndSendMsgToMcu( MT_MCU_REMOVEMIXMEMBER_CMD,(u8*)&tMt, sizeof(tMt) );
			break;
		}
	case h_ctrl_h245TransportConnected:					
		{
			m_bH245Connected = TRUE;
            CServMsg cServMsg;
            
            //厂商
            cServMsg.SetMsgBody(&m_byVendorId, sizeof(m_byVendorId));
            
            //VerId
            u8 byVerIdLen = strlen(GetPeerVersionID(hsCall));
            cServMsg.CatMsgBody(&byVerIdLen, sizeof(byVerIdLen));
            cServMsg.CatMsgBody((u8*)GetPeerVersionID(hsCall), byVerIdLen);

            //ProductId
            u8 byProductIdLen = strlen(GetPeerProductID(hsCall));
            cServMsg.CatMsgBody(&byProductIdLen, sizeof(byProductIdLen));
            cServMsg.CatMsgBody((u8*)GetPeerProductID(hsCall), byProductIdLen);

			BuildAndSendMsgToMcu( MT_MCU_MTCONNECTED_NOTIF, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );

			//SetTimer(TIMER_H245_CAPBILITY_NOTIFY_OVERTIME,1000);
            
            //[4/8/2013 liaokang] 编码方式通知
            u8 byMcuEncoding = g_cMtAdpApp.GetMcuEncoding();
            CONF_CTRL( (u16)h_ctrl_CodingFormNtf, &byMcuEncoding, sizeof(u8) );
			
			break;	
		}

	case h_ctrl_h245Established:	//cap exchange and msd completed
		{
			if( m_bHasJoinedConf )
			{
				break;
			}
			m_bHasJoinedConf = TRUE;

			u8 byMaster = m_bMaster ? 1:0;
			if( m_bMaster && MT_MANU_KDC == m_byVendorId )
			{
				TTERLABEL tTerLabel;
				tTerLabel.SetTerminalLabel( (u8)g_cMtAdpApp.m_wMcuNetId, m_byMtId );
				TNETADDR tNetAddr;

				//调整给终端的呼叫地址即是本适配接入板的的地址 2005-11-04
				tNetAddr.SetNetAddress( g_cMtAdpApp.m_dwMtAdpIpAddr, MCU_Q931_PORT, (u8)addr_uca );	
				
				CONF_CTRL_STRICT((u16)h_ctrl_MultipointConference, NULL, 0);	
				//CONF_CTRL_STRICT(h_ctrl_mcLocationIndication, &tNetAddr, sizeof(tNetAddr));			
				CONF_CTRL_STRICT((u16)h_ctrl_terminalNumberAssign, (void *)&tTerLabel, sizeof(tTerLabel));
			}

            BuildAndSendMsgToMcu( MT_MCU_MTJOINCONF_NOTIF, &byMaster, sizeof(byMaster) );

            // zbq [10/13/2007] 前向纠错的能力集上报VC
			u8 byHasVidCap = IsMtHasVidCap()? 1 : 0;
			MAPrint(LOG_LVL_DETAIL,MID_MCULIB_MTADP,"[ProcRadEvConfCtrl] hasVidCap.%d\n",byHasVidCap);

            CServMsg cMsg;
            cMsg.SetEventId(MT_MCU_CAPBILITYSET_NOTIF);
            cMsg.SetMsgBody((u8*)&m_tCommonCap, sizeof(TMultiCapSupport));
            cMsg.CatMsgBody((u8*)&m_tCommonCapEx, sizeof(TCapSupportEx));
			cMsg.CatMsgBody((u8*)&byHasVidCap, sizeof(byHasVidCap));
			//BuildAndSendMsgToMcu( MT_MCU_CAPBILITYSET_NOTIF, (u8*)&m_tCommonCap, sizeof(m_tCommonCap) );
            SendMsgToMcu(cMsg);

			// start ping-pong
			post( MAKEIID(GetAppID(), GetInsID()), TIMER_ROUNDTRIPDELAY_START ); 

            // zbq [04/05/2007] 终端分批呼叫策略
            CallNextFromQueue( m_byConfIdx, m_byMtId );
		}
		break;

	case h_ctrl_capNotifyExt:            //对端能力指示
		if( wMsgLen == sizeof(TCapSet) ) 
		{
			TCapSet *ptCapSet = (TCapSet*)pbyMsg;		
			memcpy( m_ptRemoteCapSet, ptCapSet, sizeof(TCapSet) );
			u8 byAct = 0;
			
			if( g_cMtAdpApp.m_tH323Config.m_bH245_capabilitiesManualOperation )
			{		
				//对接口进行修改，以便支持多音频共同能力集的匹配
				CMtAdpUtils::CapSupportCommon( m_tLocalCapSupport, 
                                               m_tLocalCapEx,
                                               m_ptRemoteCapSet, 
                                               m_tCommonCap,
											   m_atMainStreamCapEX,MAX_CONF_CAP_EX_NUM,
											   m_atDoubleStreamCapEX,MAX_CONF_CAP_EX_NUM,
											   m_atLocalAudioTypeDesc,MAXNUM_CONF_AUDIOTYPE
											   );
// 				TSimCapSet tSimuCapMain, tSimuCapSec;
// 				tSimuCapMain = m_tCommonCap.GetMainSimCapSet();
// 				tSimuCapSec  = m_tCommonCap.GetSecondSimCapSet();

				//zbq[10/30/2008] 取共同能力后对双流能力作可能的特殊处理
                s8 *pszMtProductId = GetPeerProductID(hsCall);
                if (g_cMtAdpApp.m_tDSCaps.IsMtExist(pszMtProductId))
                {
                    u8 byRes = m_tCommonCap.GetDStreamResolution();
                    u8 byFps = 0;
                    if (MEDIA_TYPE_H264 == m_tCommonCap.GetDStreamMediaType())
                    {
                        byFps = m_tCommonCap.GetDStreamUsrDefFPS();
                    }
                    else
                    {
                        byFps = m_tCommonCap.GetDStreamFrameRate();
                    }
					
                    u8 byDbgRes = 0;
                    u8 byDbgFps = 0;
                    g_cMtAdpApp.m_tDSCaps.GetMtCap(pszMtProductId, byDbgRes, byDbgFps);
                    
                    if (byDbgRes != 0 && byDbgRes != byRes)
                    {
                        m_tCommonCap.SetDStreamResolution(byDbgRes);
                    }
                    if (byDbgFps != 0 && byDbgFps != byFps)
                    {
                        if (MEDIA_TYPE_H264 == m_tCommonCap.GetDStreamMediaType())
                        {
                            m_tCommonCap.SetDStreamUsrDefFPS(byDbgFps);
                        }
                        else
                        {
                            m_tCommonCap.SetDStreamFrameRate(byDbgFps);
                        }
                    }
                    StaticLog( "[ProcRadEvConfCtrl] Mt.%d's ds cap<Res.%d, Fps.%d> adjust to<Res.%d, Fps.%d>!\n",
						m_byMtId, byRes, byFps, byDbgRes, byDbgFps);
                }

				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcRadEvConfCtrl] The remote FECType is 0x%x\n",m_ptRemoteCapSet->GetFECType() );
	
                // 计算共同FEC能力, zgc, 2007-09-30
                /*
				emFECType LocalFECType, CommonFECType;
				CMtAdpUtils::FECTypeIn2Out( m_tLocalCapSupportEx.GetVideoFECType(), LocalFECType );
				CMtAdpUtils::FECTypeCommon( LocalFECType, m_ptRemoteCapSet->GetFECType(), CommonFECType);
				u8 byCommonFECType;
				CMtAdpUtils::FECTypeOut2In( CommonFECType, byCommonFECType );
				m_tCommonCapEx.SetFECType( byCommonFECType );
                */


				byAct = (u8)cese_accept;	

				// [pengjie 2010/10/18] 无共同能力也让对端上线支持
// 				if( ( tSimuCapMain.IsNull() && tSimuCapSec.IsNull() )
// 					/*||  
// 					( m_tCommonCapEx.IsNoSupportFEC() 
// 					&& ( !m_tLocalCapSupportEx.IsNoSupportFEC() || 0 != m_ptRemoteCapSet->GetFECType() ) )*/
// 				  )
// 				{
// 					byAct = cese_reject;
// 				}
				// End


				// [pengjie 2010/10/18] 多能力上报支持，这里如果是H245已经OK，则把当前能力直接上报上去。
				//zjj20110319 h_ctrl_capNotifyExt消息收到后多能力上报要在发送h_ctrl_capResponse之前做,
				//	否则发送h_ctrl_capResponse后会进入再次回调处理h_ctrl_h245Established消息,
				//	导致该方法出来后m_bHasJoinedConf为true了再次发送能力级,上级就会发送2次级联通道打开命令到下级,
				//	导致下级级联库初始化,无法发送列表到上级
				if( m_bHasJoinedConf == TRUE )
				{
					u8 byHasVidCap = IsMtHasVidCap()? 1 : 0;
					CServMsg cMsg;
					cMsg.SetEventId(MT_MCU_CAPBILITYSET_NOTIF);
					cMsg.SetMsgBody((u8*)&m_tCommonCap, sizeof(TMultiCapSupport));
					cMsg.CatMsgBody((u8*)&m_tCommonCapEx, sizeof(TCapSupportEx));
					cMsg.CatMsgBody((u8*)&byHasVidCap, sizeof(byHasVidCap));
					SendMsgToMcu(cMsg);
				}
				// End
					
				CONF_CTRL( (u16)h_ctrl_capResponse, (void*)&byAct, sizeof(byAct) );

				
			}
		}		
		break;

	case h_ctrl_capResponse: //cap accepted by peer? 1 u8,1-accept;2-denied;0-error
		{
			if( wMsgLen != sizeof(u8) ) 
				return;

			byResult = *(u8*)pbyMsg;

			if( byResult == (u8)cese_accept )
			{
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "capResponse result accepted.\n");
			}
			else if(byResult == (u8)cese_reject)
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "capResponse result rejected.\n");			
			}
			else
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "capResponse error.\n");
			}			
		}
		break;

	case h_ctrl_masterSlaveDetermine: //s32 + s32
		{
			//u32 dwTerminalType = *(u32*)pbyMsg;
			//u32 dwRandomNumber = *((u32*)pbyMsg + 1);			
			s32 anBuf[1];	
			anBuf[0] = (s32)emMsActiveMC;
			if( !IS_MT&&m_byDirect == (u8)CALL_INCOMING )
			{
				anBuf[0] = (s32)emMsMT;//terminalType
			}; 
			
            //  xsl [6/5/2006] 如果主从决定还没有成功则再发送一次主从决定
            if (!m_bMsdOK)
            {
                if( kdvSendConfCtrlMsg( m_hsCall, (u16)h_ctrl_masterSlaveDetermine, 
                    (void*)anBuf, sizeof(anBuf)) == (s32)act_err )
                {
                    MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Send msd failed.\n");	
                }
                else
                {
                    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Manually MSD sent out \n");
                }
            }			
		}
		break;

	case h_ctrl_masterSlave:     //1 byte
		{
			byResult = *(u8*)pbyMsg;			
			ProcMtMcuMsdRsp( byResult );			
		}
		break;

	case h_ctrl_passwordResponse:
		{			
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(TPassRsp)) ) 
				return;

			TPassRsp *ptPassRsp = (TPassRsp *)pbyMsg;
			KillTimer( WAIT_PASSWORD_TIMEOUT );			
			BuildAndSendMsgToMcu( MT_MCU_ENTERPASSWORD_ACK, ptPassRsp->m_abyPassword, (u16)(ptPassRsp->m_nPassLen) );
		}
		break;

	case h_ctrl_userInput:
		{
			TUserInputInfo *ptUserInput = (TUserInputInfo *)pbyMsg;
			if( ptUserInput->GetSignalType() != '#' )
			{
				m_abyPassword[ m_nPasswordLen ++ ] = (u8)( ptUserInput->GetSignalType() ); 								
			    if( (u32)m_nPasswordLen < sizeof(m_abyPassword) )
				{
					break;
				}
			}
			KillTimer( WAIT_PASSWORD_TIMEOUT );			
			BuildAndSendMsgToMcu( MT_MCU_ENTERPASSWORD_ACK, m_abyPassword, (u16)(m_nPasswordLen) );
			m_nPasswordLen = 0;
			break;
		}

	case h_ctrl_dropTerminal:          //force ter to quit	chair->MC, terLabel
		{
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TTERLABEL))) 
				return;

			PTTERLABEL pter = (PTTERLABEL)pbyMsg;
			TMt tMt;
			//tMt.SetMt( pter->GetMcuNo(), pter->GetTerNo(), 0, m_byConfIdx );	
			CMtAdpUtils::ConverTTERLABEL2TMt( *pter, tMt, m_byConfIdx );
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "recved dropTerminal -- <%d,%d>\n", pter->GetMcuNo(),pter->GetTerNo() );
			BuildAndSendMsgToMcu( MT_MCU_DELMT_REQ, (u8*)&tMt, sizeof(tMt) );
		}			
		break;

	case h_ctrl_dropConference:        //chair->MC,MC->ter, NULL	
		{
			//chair -> MC
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "recved dropConference from Chairman\n" );
			BuildAndSendMsgToMcu( MT_MCU_DROPCONF_CMD );
		}
		break;

	case h_ctrl_makeMeChair:       //ter->MC,   NULL
		
		BuildAndSendMsgToMcu( MT_MCU_APPLYCHAIRMAN_REQ );
		break;

	case h_ctrl_cancelMakeMeChair:  //chair->MC, NULL

		BuildAndSendMsgToMcu( MT_MCU_CANCELCHAIRMAN_REQ );
		break;

    case h_ctrl_makeMeChairResponse:
        break;

	case h_ctrl_requestChairTokenOwner:   //ter->MC,   NULL

 		BuildAndSendMsgToMcu( MT_MCU_GETCHAIRMAN_REQ );
		break;

	case h_ctrl_requestForFloor:   //apply speaker	       ter->MC,   NULL	
		
 		BuildAndSendMsgToMcu( MT_MCU_APPLYSPEAKER_NOTIF );	
		break;

	case h_ctrl_makeTerminalBroadcaster://chair->MC, terLabel + 1u8(1 command 2 request)
		{
			if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TTERLABEL) + 1 ))
				return;

			PTTERLABEL pter = (PTTERLABEL)pbyMsg;

			TMt tMt;
			//tMt.SetMt(pter->GetMcuNo(), pter->GetTerNo(), 0, m_byConfIdx );
			CMtAdpUtils::ConverTTERLABEL2TMt( *pter, tMt, m_byConfIdx );
			
			u8 byMode = *((u8*)pbyMsg + sizeof(TTERLABEL));
			
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "recved makeTerminalBroadcaster <%d,%d>, mode = %d\n", pter->GetMcuNo(), pter->GetTerNo(), byMode );
			
			BuildAndSendMsgToMcu(byMode == (u8)mode_command ? MT_MCU_SPECSPEAKER_CMD : MT_MCU_SPECSPEAKER_REQ,
								(u8*)&tMt, sizeof(tMt));
		}
		break;

	case h_ctrl_cancelMakeTerminalBroadcaster:     //chair->MC, NULL

		BuildAndSendMsgToMcu( MT_MCU_CANCELSPEAKER_CMD );
		break;

	case h_ctrl_chairSelectViewCmd:	//	ter->MC, TerLabel + 1Byte(type_audio/type_video/type_av)
		{
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TTERLABEL) + 1 )) 
				return;
			
			PTTERLABEL pTer = (PTTERLABEL)pbyMsg;
		
			u8 meidaType = *((u8*)pbyMsg + sizeof(TTERLABEL));
			
			//SrcMt is the sender of media data 
			TMt tSrcMt;
			//tSrcMt.SetMt( pTer->GetMcuNo(), pTer->GetTerNo(), 0, m_byConfIdx );	
			CMtAdpUtils::ConverTTERLABEL2TMt( *pTer, tSrcMt, m_byConfIdx );
			
			TMt tDstMt;
			//tDstMt.SetMt( pTer->GetMcuNo(), m_byMtId, g_cMtAdpApp.m_byDriId, m_byConfIdx );
			CMtAdpUtils::ConverTTERLABEL2TMt( *pTer, tDstMt, m_byConfIdx );
			tDstMt.SetMtId( m_byMtId );
			tDstMt.SetDriId( g_cMtAdpApp.m_byDriId );

			TSwitchInfo tSwitchInfo;
			tSwitchInfo.SetDstMt(tDstMt);
			tSwitchInfo.SetSrcMt(tSrcMt);
			if( meidaType == (u8)type_audio )
				tSwitchInfo.SetMode(MODE_AUDIO); 
			else if( meidaType == (u8)type_video )
				tSwitchInfo.SetMode(MODE_VIDEO);
			else if( meidaType == (u8)type_data )
				tSwitchInfo.SetMode(MODE_DATA);
			else
				tSwitchInfo.SetMode(MODE_BOTH);
			
			BuildAndSendMsgToMcu( MT_MCU_STARTSELMT_CMD, (u8*)&tSwitchInfo, sizeof(tSwitchInfo) );

		}
		break;

	case h_ctrl_sendThisSource:      //chair->MC, terLabel + 1u8(1 command 2 request)
		{
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TTERLABEL) + 1) ) 
				return;

			PTTERLABEL pTer = (PTTERLABEL)pbyMsg;
		
			u8 byMode = *((u8*)pbyMsg + sizeof(TTERLABEL));
			
			//SrcMt is the sender of media data 
			TMt tSrcMt;
			//tSrcMt.SetMt( pTer->GetMcuNo(), pTer->GetTerNo(), 0, m_byConfIdx );	
			CMtAdpUtils::ConverTTERLABEL2TMt( *pTer, tSrcMt, m_byConfIdx );
			
			TMt tDstMt;
			//tDstMt.SetMt(pTer->GetMcuNo(), m_byMtId, g_cMtAdpApp.m_byDriId, m_byConfIdx );
			CMtAdpUtils::ConverTTERLABEL2TMt( *pTer, tDstMt, m_byConfIdx );
			tDstMt.SetMtId( m_byMtId );
			tDstMt.SetDriId( g_cMtAdpApp.m_byDriId );

			TSwitchInfo tSwitchInfo;
			tSwitchInfo.SetDstMt(tDstMt);
			tSwitchInfo.SetSrcMt(tSrcMt);
			tSwitchInfo.SetMode(MODE_BOTH);  
			
			BuildAndSendMsgToMcu( byMode == (u8)mode_command ? MT_MCU_STARTSELMT_CMD : MT_MCU_STARTSELMT_REQ,
								(u8*)&tSwitchInfo, sizeof(tSwitchInfo));

			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "recved sendThisSource -- <%d,%d>,mode = %d\n", pTer->GetMcuNo(), pTer->GetTerNo(), byMode );		
		}
		break;

	case h_ctrl_cancelSendThisSource: //cancel selecting   chair->MC, NULL
		{
			TMt tDstMt;
			tDstMt.SetMt( /*(u8)g_cMtAdpApp.m_wMcuNetId*/ LOCAL_MCUIDX, m_byMtId, g_cMtAdpApp.m_byDriId, m_byConfIdx);
			BuildAndSendMsgToMcu( MT_MCU_STOPSELMT_CMD, (u8*)&tDstMt, sizeof(tDstMt) );
		}		
		break;
		
	case h_ctrl_terminalListRequest:  //ter->MC,   NULL	
		
		BuildAndSendMsgToMcu( MT_MCU_JOINEDMTLIST_REQ );			
		break;

	case h_ctrl_requestTerminalID:    //ter->MC,   terLabel
		{
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TTERLABEL)) ) 
				return;

			PTTERLABEL pter = (PTTERLABEL)pbyMsg;
			
			if( pter->IsValid() )
			{
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "recved requestTerminalID -- <%d,%d>\n", pter->GetMcuNo(), pter->GetTerNo() );
				TMt tMt;				
				//tMt.SetMt( pter->GetMcuNo(), pter->GetTerNo(), 0, m_byConfIdx );
				CMtAdpUtils::ConverTTERLABEL2TMt( *pter, tMt, m_byConfIdx );
				m_wSavedEvent = MT_MCU_GETMTALIAS_REQ;
				BuildAndSendMsgToMcu( MT_MCU_GETMTALIAS_REQ, (u8*)&tMt, sizeof(tMt) );			
			}
		}
		break;

	case h_ctrl_terminalIDResponse:   //ter->MC,   terInfo
		{
			if(!BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TTERINFO))) 
				return;
			
			PTTERINFO pTerInfo = (PTTERINFO)pbyMsg;
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "recved terminalIDResponse -- <%d,%d> : %s\n",
					   pTerInfo->GetTerlabel().GetMcuNo(), pTerInfo->GetTerlabel().GetTerNo(), pTerInfo->GetName());

			TMtAlias tMtAlias;
			tMtAlias.m_AliasType = (u8)mtAliasTypeH323ID;
			memcpy( tMtAlias.m_achAlias, pTerInfo->GetName(), sizeof(tMtAlias.m_achAlias) );
			BuildAndSendMsgToMcu( MT_MCU_MTALIAS_NOTIF, (u8*)(&tMtAlias), sizeof(tMtAlias) );	
		}
		break;

	case h_ctrl_requestAllTerminalIDs://ter->MC,   NULL

		BuildAndSendMsgToMcu( MT_MCU_JOINEDMTLISTID_REQ );				
		break;

	case h_ctrl_roundTripDelay:			   //check peer aliveness
		{			
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(wMsgType) + 1) ) 
				return;
			
			s32 nRtdRes = *(s32*)pbyMsg;
			u32 dwRtdTimeStamp = OspTickGet();
			
			if( nRtdRes >= 0 )		
			{
				m_bEverGotRTDResponse = TRUE;	
				m_byTimeoutTimes = 0;				
                
				SetTimer( TIMER_ROUNDTRIPDELAY_REQ, g_cMtAdpApp.m_wMaxRTDInterval * 1000 ); //ping after somt time
                if( g_nPrtdid > 0 )
				{
					if( g_nPrtdid == m_byMtId )
					{
						StaticLog( "RoundTripDelay to MT-%u return %d, curticks:%d\n", m_byMtId, nRtdRes, dwRtdTimeStamp );
					}
				}
				else if( g_nPrtdid == 0 )
				{
					StaticLog( "RoundTripDelay to MT-%u return %d, curticks:%d\n", m_byMtId, nRtdRes, dwRtdTimeStamp );
				}
	
			}
			else //timeout
			{
				m_byTimeoutTimes ++ ;

				if( g_nPrtdid > 0 )
				{
					if( g_nPrtdid == m_byMtId )
					{
						StaticLog( "%d RoundTripDelay to MT-%u timed out! Times = %u,curticks:%d, isSupport:%d \n", nRtdRes, m_byMtId, m_byTimeoutTimes,dwRtdTimeStamp, m_bEverGotRTDResponse );
					}
				}
				else if( g_nPrtdid == 0 )
				{
					StaticLog( "%d RoundTripDelay to MT-%u timed out! Times = %u,curticks:%d, isSupport:%d \n", nRtdRes, m_byMtId, m_byTimeoutTimes,dwRtdTimeStamp, m_bEverGotRTDResponse );
				}
		
				if( m_byTimeoutTimes < g_cMtAdpApp.m_byMaxRTDFailTimes )
				{				
					SetTimer(TIMER_ROUNDTRIPDELAY_REQ, g_cMtAdpApp.m_wMaxRTDInterval * 1000 );
				}
				else if( m_bEverGotRTDResponse && m_byTimeoutTimes >= g_cMtAdpApp.m_byMaxRTDFailTimes )
				{					
                    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "call dropped for roundtrip fails.\n" );
					ClearInst( MT_MCU_MTDISCONNECTED_NOTIF, (u8)emDisconnectReasonRtd );
				}
			}
		}
		break;

	case h_ctrl_enterH243Password: // MMCU requires password		
		{
			CServMsg cMsg;
			cMsg.SetEventId( MCU_MT_ENTERPASSWORD_REQ );
			SendMsgToMcu( cMsg );
		}
		break;

	case h_ctrl_enterH243TerminalID:  //MC requests terminal names	MMCU->MCU,  NULL		
		{	
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "<< OnConfCtrlMsg: enterH243TerminalID \n" );
			TTERINFO terInfo;

			TTERLABEL tTermiLabel;
			memcpy(&tTermiLabel,&m_tTerminalLabel,sizeof(TTERLABEL));
			terInfo.SetLabel(&tTermiLabel);
			terInfo.SetName(g_cMtAdpApp.m_achMcuAlias);	
			
			kdvSendConfCtrlMsg( m_hsCall, (u16)h_ctrl_terminalIDResponse, &terInfo, LEN_TERINFO );						
		}
		break;
		
	case h_ctrl_H239Message :						// h239
		{	
			//if(!BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TTERINFO))) 
			//{
			//	return;
			//}
			PTH239INFO ptH239Info = (PTH239INFO)pbyMsg;
			TH239TokenInfo tH239TokenInfo;
			tH239TokenInfo.Clear();

			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcRadEvConfCtrl] Incoming H239Message(type:%d, isack:%d, chanid:0x%x, breaking:%d)\n",
				       ptH239Info->GetH239InfoType(), ptH239Info->IsResponseAck(), 
					   ptH239Info->GetChannelId(), ptH239Info->GetSymmetryBreaking() );

			switch( ptH239Info->GetH239InfoType() )
			{
			case emH239PresentationTokenRequest :
				tH239TokenInfo.SetChannelId( ptH239Info->GetChannelId() );
				tH239TokenInfo.SetSymmetryBreaking( ptH239Info->GetSymmetryBreaking() );
				BuildAndSendMsgToMcu( MT_MCU_GETH239TOKEN_REQ, (u8*)(&tH239TokenInfo), sizeof(TH239TokenInfo));
				break;
				
			case emH239PresentationTokenResponse :
				// xliang [11/14/2008] 主呼进入的polycomMCU, 这个响应是获取其Token成功的响应
				if ( MT_MANU_POLYCOM == m_byVendorId &&
					emEndpointTypeMCU == m_emEndpointType &&
					(u8)CALL_INCOMING == m_byDirect )
				{
					if ( ptH239Info->IsResponseAck() )
					{
						BuildAndSendMsgToMcu( POLY_MCU_GETH239TOKEN_ACK );
					}
					else
					{
						BuildAndSendMsgToMcu( POLY_MCU_GETH239TOKEN_NACK );
					}
				}
				else
				{
					if (ptH239Info->IsResponseAck())
					{
						BuildAndSendMsgToMcu( MT_MCU_RELEASEH239TOKEN_NOTIF );
					}
				}
				break;
				
			case emH239PresentationTokenRelease :
				BuildAndSendMsgToMcu( MT_MCU_RELEASEH239TOKEN_NOTIF );	
				break;
				
			case emH239PresentationTokenIndicateOwner :
				tH239TokenInfo.SetChannelId( ptH239Info->GetChannelId() );
				tH239TokenInfo.SetSymmetryBreaking( ptH239Info->GetSymmetryBreaking() );
				BuildAndSendMsgToMcu( MT_MCU_OWNH239TOKEN_NOTIF, (u8*)(ptH239Info), sizeof(TH239INFO) );
				break;
				
			case emH239FlowControlReleaseRequest :
			case emH239FlowControlReleaseResponse :
				break;
				
			default :
				break;	
			}
		}
		break;


	//=====================KDV private messages==========================

	case h_ctrl_chairTransferReq:		//	chair->MC, TTERLABEL
		{	
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TTERLABEL))) 
				return;

			PTTERLABEL pter = (PTTERLABEL)pbyMsg;
			TMt tMt;
			//tMt.SetMt(pter->GetMcuNo(), pter->GetTerNo(), 0, m_byConfIdx);
			CMtAdpUtils::ConverTTERLABEL2TMt( *pter, tMt, m_byConfIdx );

			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "chairTransferReq -- <%d,%d>\n", pter->GetMcuNo(), pter->GetTerNo() );
			BuildAndSendMsgToMcu( MT_MCU_SPECCHAIRMAN_REQ, (u8*)&tMt, sizeof(tMt) );	
		}
		break;
		
	case h_ctrl_chairInviteTerminalReq:     //	chair->MC, TTERADDR
		{

			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(wMsgType)) ) 
				return;

			u8 byMtNum = wMsgLen / sizeof(TTERADDR);		
			CServMsg cMsg;
			cMsg.SetEventId(MT_MCU_ADDMT_REQ);	
   
			if( byMtNum == 0 )
			{
				TTERADDR tTERADDR;
				CONF_CTRL_STRICT((u16)h_ctrl_chairInviteFailedInd, (u8*)&tTERADDR, sizeof(tTERADDR));
				break;
			}
            			
			TMcu tMcu;
			tMcu.SetMcu( (u8)g_cMtAdpApp.m_wMcuNetId );
			
			cMsg.SetMsgBody( (u8*)&tMcu, sizeof(tMcu) );
				
            TAddMtInfo tMtInfo;
			for( u8 byLoop = 0; byLoop < byMtNum; byLoop ++ )
			{
				memset(&tMtInfo, 0, sizeof(TAddMtInfo));
				TTERADDR tTERADDR = *(TTERADDR*)(pbyMsg + byLoop * sizeof(TTERADDR));

				if( tTERADDR.GetAddrType() == ADDR_TYPE_IP )
				{
					tMtInfo.m_AliasType = (u8)mtAliasTypeTransportAddress;
					tMtInfo.m_tTransportAddr.SetIpAddr(ntohl(tTERADDR.GetIpAddr() ));
					tMtInfo.m_tTransportAddr.SetPort( tTERADDR.GetIpPort() );
				}
				else
				{
					if( tTERADDR.GetAliasType() == ALIAS_E164 )
					{
						tMtInfo.m_AliasType = (u8)mtAliasTypeE164;
					}
					else if( tTERADDR.GetAliasType() == ALIAS_H323ID )
					{
						tMtInfo.m_AliasType = (u8)mtAliasTypeH323ID;
					}

					// 漏转码 [pengguofeng 7/23/2013]
					s8 achMtAlias[MAXLEN_ALIAS] = {0};
					BOOL32 bNeedTrans = TransEncoding(tTERADDR.GetAliasName(), achMtAlias, sizeof(achMtAlias)-1, FALSE);
					if ( bNeedTrans )
					{
						strncpy(tMtInfo.m_achAlias, achMtAlias , sizeof(achMtAlias) - 1 );
					}
					else
					{
						// 注意：此处mt发过来的时候就已经是16个字节了！ [pengguofeng 7/23/2013]
						strncpy(tMtInfo.m_achAlias, tTERADDR.GetAliasName() , VALIDLEN_ALIAS + 1 );
					}
				}
			
				tMtInfo.m_byCallMode = CONF_CALLMODE_TIMER;
                tMtInfo.SetCallBitRate( 0 );
				cMsg.CatMsgBody((u8*)&tMtInfo, sizeof(tMtInfo));
			}

			SendMsgToMcu( cMsg );
		}
		break;

	case h_ctrl_confInfoReq:		//inquiry about conf info	ter->MC,   NULL

		BuildAndSendMsgToMcu( MT_MCU_GETCONFINFO_REQ );
		break;

	case h_ctrl_makeTerQuietCmd:	//far-end mute	ter->MC,MC->ter,  TTERLABEL(dst) + 1u8(turn_on/turn_off)
		{
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TTERLABEL) + 1  )) 
				return;

			PTTERLABEL pter = (PTTERLABEL)pbyMsg;
			u8 byAction = *(u8*)(pbyMsg + sizeof(TTERLABEL));
			u8 byMode = ( byAction == (u8)turn_on ? 1 : 0);

			TMt tMt;
			//tMt.SetMt( pter->GetMcuNo(), pter->GetTerNo(), 0, m_byConfIdx );
			CMtAdpUtils::ConverTTERLABEL2TMt( *pter, tMt, m_byConfIdx );
			BuildAndSendMsgToMcu2( MT_MCU_MTMUTE_CMD, (u8*)&tMt, sizeof(tMt),
								   &byMode, sizeof(byMode) );
		}
		break;

	case h_ctrl_makeTerMuteCmd:		//far-end dumb	ter->MC,MC->ter,  TTERLABEL(dst) + 1u8(turn_on/turn_off)
		{
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TTERLABEL) + 1)) 
				return;

			PTTERLABEL pter = (PTTERLABEL)pbyMsg;
			u8 byMode = *(u8*)( pbyMsg + sizeof(TTERLABEL) );
			byMode = ( byMode == (u8)turn_on ? 1 : 0); 

			TMt tMt;
			//tMt.SetMt( pter->GetMcuNo(), pter->GetTerNo(), 0, m_byConfIdx );
			CMtAdpUtils::ConverTTERLABEL2TMt( *pter, tMt, m_byConfIdx );
			BuildAndSendMsgToMcu2( MT_MCU_MTDUMB_CMD, (u8*)&tMt, sizeof(tMt),
								   &byMode, sizeof(byMode) );
		}		
		break;

	case h_ctrl_terStatusInd:	//	ter->MC,   __TTERSTATUS ( response h_ctrl_terStatusReq)
		{
			if( !BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(_TTERSTATUS))) 
				return;

			_TTERSTATUS tTerStatus;
			memset( &tTerStatus, 0, sizeof(tTerStatus) );

			//非36终端直接取消息体
			if ( m_byMtVer != (u8)emMtVer36 )
			{
				tTerStatus = *(_TTERSTATUS*)pbyMsg;
			}
			else
			{
				_TTERSTATUS_VER36 tTerStatus36;
				memset( &tTerStatus36, 0, sizeof(tTerStatus36) );
				tTerStatus36 = *(_TTERSTATUS_VER36*)pbyMsg;
				CStructConvert::TERSTATUSVer36ToVer40( tTerStatus, tTerStatus36 );
			}
			TMtStatus tMtStatus = CMtAdpUtils::TMtStatusOut2In(tTerStatus);

			BuildAndSendMsgToMcu( m_wSavedEvent == MCU_MT_GETMTSTATUS_REQ ? 
							MT_MCU_GETMTSTATUS_ACK : MT_MCU_MTSTATUS_NOTIF,
							(u8*)&tMtStatus, sizeof(tMtStatus) );

            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcRadEvConfCtrl] MT(%u.%u.%u.%u) video Lose.%d, BoardType.%d !\n", 
                        QUADADDR(m_dwMtIpAddr), tMtStatus.IsVideoLose(), tMtStatus.GetMtBoardType() );

			m_wSavedEvent = 0;
		}
		break;

	case h_ctrl_smsInd:	 //short msg ind.	ter->MC,MC->ter,TTERLABEL(src) + TTERLABEL(dst) + byte(times) + str('\0'terminated)
		{
			CRollMsg cRollMsg;
			if( cRollMsg.SetRollMsg(pbyMsg, wMsgLen) == FALSE )
				break;

			CServMsg cMsg;			
			cMsg.SetEventId( MT_MCU_SENDMSG_CMD );

			u16 wDstNum = htons( cRollMsg.GetDstMtNum() );
			cMsg.SetMsgBody( (u8*)&wDstNum, sizeof(u16) );
			
			TMt tMt;
			TMTLABEL tMtLabel;
			s32 nIndex = cRollMsg.GetFirstDstMt(&tMtLabel);
			
			while ( nIndex != -1 )
			{
				TTERLABEL tTer;
				tTer.SetTerminalLabel( tMtLabel.GetMcuNo(), tMtLabel.GetMtNo() );
				CMtAdpUtils::ConverTTERLABEL2TMt( tTer, tMt );
				//tMt.SetMt( tMtLabel.GetMcuNo(), tMtLabel.GetMtNo() );
				cMsg.CatMsgBody( (u8*)&tMt, sizeof(tMt) ) ;
				nIndex = cRollMsg.GetNextDstMt( nIndex, &tMtLabel );
			}
			// 多国语言支持 [pengguofeng 4/27/2013]
			s8 achRollMsg[/*0x1000*/ MAX_ROLLMSG_LEN]; // 最大长度已经定好 [pengguofeng 4/27/2013]
			memset(achRollMsg, 0, sizeof(achRollMsg));
			BOOL32 bNeedTrans = TransEncoding((s8*)cRollMsg.GetRollMsgContent(), achRollMsg, sizeof(achRollMsg), FALSE);
			if ( bNeedTrans == TRUE )
			{
				cRollMsg.SetRollMsgContent((u8*)achRollMsg, strlen(achRollMsg));
			}
			else
			{
				MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcRadEvConfCtrl]h_ctrl_smsInd Not need to trans Roll Msg\n");
			}
			cMsg.CatMsgBody( (u8*)&cRollMsg, cRollMsg.GetTotalMsgLen() );
			SendMsgToMcu( cMsg );
		}
		break;

	case h_ctrl_letTerViewMeReq: //forced viewing
		{
			if(!BODY_LEN_GE(cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TTERLABEL) + sizeof(u8))) return;

			TTERLABEL *pter = (PTTERLABEL)pbyMsg;
			u8 mode = *(u8*)(pbyMsg + sizeof(TTERLABEL));

			TMt tDstMt;
			//tDstMt.SetMt(pter->GetMcuNo(), pter->GetTerNo(), 0, m_byConfIdx);
			CMtAdpUtils::ConverTTERLABEL2TMt( *pter, tDstMt, m_byConfIdx );
			
			TMt tSrcMt;
			//tSrcMt.SetMt(pter->GetMcuNo(), m_byMtId, g_cMtAdpApp.m_byDriId, m_byConfIdx);
			CMtAdpUtils::ConverTTERLABEL2TMt( *pter, tSrcMt, m_byConfIdx );
			tSrcMt.SetMtId( m_byMtId );
			tSrcMt.SetDriId( g_cMtAdpApp.m_byDriId );
			
			TSwitchInfo tSwitchInfo;
			tSwitchInfo.SetDstMt(tDstMt);
			tSwitchInfo.SetSrcMt(tSrcMt);

			switch(mode) 
			{
			case type_video: tSwitchInfo.SetMode(MODE_VIDEO); break;
			case type_audio: tSwitchInfo.SetMode(MODE_AUDIO); break;				
			default:         tSwitchInfo.SetMode(MODE_BOTH);  break;
			}
			
			BuildAndSendMsgToMcu( MT_MCU_STARTMTSELME_REQ, (u8*)&tSwitchInfo, sizeof(tSwitchInfo) );
		}		
		break;
	
	case h_ctrl_stopViewMeCmd: //stop forced viewing

		BuildAndSendMsgToMcu( MT_MCU_STOPMTSELME_CMD );
		break;

	case h_ctrl_multicastTerReq:
		{
			if(!BODY_LEN_GE(cServMsg, sizeof(HCALL) + 
				sizeof(wMsgType) + sizeof(TTERLABEL))) 
				return;

			TMt tMt;
			TTERLABEL *pter = (PTTERLABEL)pbyMsg;
			//tMt.SetMt(pter->GetMcuNo(), pter->GetTerNo(), 0, m_byConfIdx);
			CMtAdpUtils::ConverTTERLABEL2TMt( *pter, tMt, m_byConfIdx );
			BuildAndSendMsgToMcu( MT_MCU_STARTBROADCASTMT_REQ, (u8*)&tMt, sizeof(tMt) );
		}
		break;
		
	case h_ctrl_startVACReq: //NULL

		BuildAndSendMsgToMcu( MT_MCU_STARTVAC_REQ );
		break;

	case h_ctrl_stopVACReq: //NULL

		BuildAndSendMsgToMcu( MT_MCU_STOPVAC_REQ );
		break;
		
	case h_ctrl_startDiscussReq: //u8(mt num) + *TMT(MT Sequence)

		if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(wMsgType) + wMsgLen )) 
			return;
		BuildAndSendMsgToMcu( MT_MCU_STARTDISCUSS_REQ, pbyMsg, wMsgLen);
		break;

	case h_ctrl_stopDiscussReq: //NULL

		BuildAndSendMsgToMcu( MT_MCU_STOPDISCUSS_REQ );
		break;

	case h_ctrl_startVMPReq: //	 TVMPParam

		if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TVMPParam) ) ) 
        {
            if ( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TVMPParamV4R5) ) )
            {
                return;
            }
            else
            {
                //zbq [12/25/2009] 老版本的主席终端 尝试控制 会议的vmp
                TVMPParamV4R5 tVmpParamV4R5Before = *(TVMPParamV4R5*)pbyMsg;
                TVMPParam tVmpParam;
                CStructConvert::VMPParamV4R5ToV4R6(tVmpParam, tVmpParamV4R5Before);
                
                BuildAndSendMsgToMcu( MT_MCU_STARTVMP_REQ, (u8*)&tVmpParam, sizeof(TVMPParam) );
                break;
            }
        }

		BuildAndSendMsgToMcu( MT_MCU_STARTVMP_REQ, pbyMsg, sizeof(TVMPParam) );
		break;

	case h_ctrl_stopVMPReq: //NULL

		BuildAndSendMsgToMcu( MT_MCU_STOPVMP_REQ ) ;
		break;

	case h_ctrl_changeVMPParamReq: //TVMPParam

		if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TVMPParam) ) ) 
        {
            if ( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TVMPParamV4R5) ) )
            {
                return;
            }
            else
            {
                //zbq [12/25/2009] 老版本的主席终端 尝试控制 会议的vmp
                TVMPParamV4R5 tVmpParamV4R5Before = *(TVMPParamV4R5*)pbyMsg;
                TVMPParam tVmpParam;
                CStructConvert::VMPParamV4R5ToV4R6(tVmpParam, tVmpParamV4R5Before);
                
                BuildAndSendMsgToMcu( MT_MCU_CHANGEVMPPARAM_REQ, (u8*)&tVmpParam, sizeof(TVMPParam) );
                break;
            }
        }

		BuildAndSendMsgToMcu( MT_MCU_CHANGEVMPPARAM_REQ, pbyMsg, sizeof(TVMPParam) );
		break;

	case h_ctrl_getVMPParamReq: //NULL

		BuildAndSendMsgToMcu( MT_MCU_GETVMPPARAM_REQ );
		break;

	case h_ctrl_pollParamRsp:
	case h_ctrl_pollParamInd:
		{
			//类型(1Byte,PollType) + 模式(1Byte,DataType) + 当前状态(1Byte, PollStatus) + 当前轮询到的终端(TTERLABEL，+ 时间间隔(u16,s). 如果没有则全填0)
			//（如果是rsp + )  终端个数(1Byte) + 列表(TTERLABEL数组 + 时间间隔(u16,s))
			if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(wMsgType) + 5 + sizeof(TTERLABEL) ) ) 
				return;
			
			u8 *pbyBuf = cServMsg.GetMsgBody();
			TMt tMt;
			tMt.SetMt( (u8)g_cMtAdpApp.m_wMcuNetId, m_byMtId, g_cMtAdpApp.m_byDriId, m_byConfIdx );
			TPollInfo tPollInfo;
			
			CServMsg cMsg;			
			cMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
			
			u8 byPollMode = *pbyBuf;
			
			if( byPollMode != (u8)poll_vcs )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "polling mode other than VCS not accepted.\n");
				break;
			}
			
			u8 byPollDataType = *( ++ pbyBuf );			
			switch( byPollDataType ) 
			{
			case type_video:	tPollInfo.SetMediaMode(MODE_VIDEO);	break;
			case type_audio:	tPollInfo.SetMediaMode(MODE_AUDIO);	break;
			case type_data:		tPollInfo.SetMediaMode(MODE_DATA);	break;
			case type_none:		tPollInfo.SetMediaMode(MODE_NONE);	break;
			default: break;
			}

			//u8 byPollStatus = *( ++ pbyBuf);
			switch(byPollDataType) 
			{
			case poll_status_normal:  tPollInfo.SetPollState(POLL_STATE_NORMAL); break;
			case poll_status_pause:   tPollInfo.SetPollState(POLL_STATE_PAUSE);  break;
			case poll_status_none:    tPollInfo.SetPollState(POLL_STATE_NONE);   break;
			default: break;
			}

			pbyBuf ++ ;
			TMtPollParam tPolledMt;
			//tPolledMt.SetMt(((PTTERLABEL)pbyBuf)->GetMcuNo(), ((PTTERLABEL)pbyBuf)->GetTerNo(), 0, m_byConfIdx );
			TMt tTempMt= tPolledMt.GetTMt();
			CMtAdpUtils::ConverTTERLABEL2TMt( *(PTTERLABEL)pbyBuf, tTempMt, m_byConfIdx );
			tPolledMt.SetMt( tTempMt );
			pbyBuf += sizeof(TTERLABEL);
			tPolledMt.SetKeepTime( *(u16*)pbyBuf );			
			tPollInfo.SetMtPollParam( tPolledMt );
			
			cMsg.CatMsgBody((u8*)&tPollInfo, sizeof(tPollInfo));

			pbyBuf += 2;
			u8 byNumMt = *pbyBuf;
			if( byNumMt != ( cServMsg.GetMsgBodyLen() - sizeof(wMsgType) - 4 ) / ( sizeof(TTERLABEL) + 1 ) )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Given number of element (TerLabel + KeepTime(u16)) not"
					"equal to the length acutally found. msg ignored.\n");
				break;
			}

			if( wMsgType == (u16)h_ctrl_pollParamRsp )
			{				
				pbyBuf++;
				for( s32 MtNum = 0; MtNum < byNumMt; MtNum ++ )
				{
					TMtPollParam tPollParam;
					//tPollParam.SetMt(((PTTERLABEL)pbyBuf)->GetMcuNo(), ((PTTERLABEL)pbyBuf)->GetTerNo(), 0, m_byConfIdx);
					TMt tMt = tPollParam.GetTMt();
					CMtAdpUtils::ConverTTERLABEL2TMt( *(PTTERLABEL)pbyBuf, tMt, m_byConfIdx );
					tPollParam.SetTMt( tMt );
					pbyBuf += sizeof(TTERLABEL);
					tPollParam.SetKeepTime(*(u16*)pbyBuf);
					pbyBuf += 2;
					
					cMsg.CatMsgBody((u8*)&tPollParam, sizeof(tPollParam));
				}
				cMsg.SetEventId( MT_MCU_GETMTPOLLPARAM_ACK );
			}
			else
				cMsg.SetEventId( MT_MCU_POLLMTSTATE_NOTIF );

			SendMsgToMcu(cMsg);
		}
		break;

	case h_ctrl_delayConfTimeReq: //delay conference duration
		{			
			if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(u16) ) ) 
				return;
			
			u16 minutes = *(u16*)pbyMsg;			
			BuildAndSendMsgToMcu( MT_MCU_DELAYCONF_REQ, (u8*)&minutes, sizeof(u16) );		
		}
		break;
		
	case h_ctrl_whoViewThisTerReq: //A asks who is watching B（law case system）		
		{
			if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(wMsgType) + sizeof(TTERLABEL) ) )   
				return;

			TTERLABEL *pter = (PTTERLABEL)pbyMsg;

			TMt tMt;
			//tMt.SetMt( pter->GetMcuNo(), pter->GetTerNo(), 0, m_byConfIdx );
			CMtAdpUtils::ConverTTERLABEL2TMt( *pter, tMt, m_byConfIdx );
			
			BuildAndSendMsgToMcu( MT_MCU_GETMTSELSTUTS_REQ, (u8*)&tMt, sizeof(tMt) );
		}
		break;

	case h_ctrl_destterStatusReq:
		{
			TTERLABEL tTer = *(TTERLABEL*)pbyMsg;
			TMt tMt;
			//tMt.SetMt(tTer.GetMcuNo(), tTer.m_byTerNo);
			CMtAdpUtils::ConverTTERLABEL2TMt( tTer, tMt );
			BuildAndSendMsgToMcu( MT_MCU_GETMTSTATUS_REQ, (u8*)&tMt, sizeof(tMt) );
		}
		break;

	//内置矩阵所有方案指示
	case h_ctrl_innerMatrixGetAllSchemesInd:				
		BuildAndSendMsgToMcu( MT_MCU_MATRIX_ALLSCHEMES_NOTIF, pbyMsg, wMsgLen );
		break;
		
	//内置矩阵某一方案指示
	case h_ctrl_innerMatrixGetOneSchemeInd:				
		BuildAndSendMsgToMcu( MT_MCU_MATRIX_ONESCHEME_NOTIF, pbyMsg, wMsgLen );
		break;
		
	//内置矩阵方案保存指示
	case h_ctrl_innerMatrixSaveSchemeInd:				
		BuildAndSendMsgToMcu( MT_MCU_MATRIX_SAVESCHEME_NOTIF, pbyMsg, wMsgLen );
		break;
		
	//内置矩阵设置为当前方案指示
	case h_ctrl_innerMatrixSetCurSchemeInd:				
		BuildAndSendMsgToMcu( MT_MCU_MATRIX_SETCURSCHEME_NOTIF, pbyMsg, wMsgLen );
		break;
		
	//内置矩阵当前方案指示
	case h_ctrl_innerMatrixCurSchemeInd:				
		BuildAndSendMsgToMcu( MT_MCU_MATRIX_CURSCHEME_NOTIF, pbyMsg, wMsgLen );
		break;

	//外置矩阵类型指示
	case h_ctrl_matrixExInfoInd:				
		BuildAndSendMsgToMcu( MT_MCU_EXMATRIXINFO_NOTIFY, pbyMsg, wMsgLen );
		break;

	//外置矩阵端口号指示
	case h_ctrl_matrixExPortInd:				
		BuildAndSendMsgToMcu( MT_MCU_EXMATRIX_GETPORT_NOTIF, pbyMsg, wMsgLen );
		break;

	//外置矩阵端口名指示
	case h_ctrl_matrixExPortNameInd:			
		BuildAndSendMsgToMcu( MT_MCU_EXMATRIX_PORTNAME_NOTIF, pbyMsg, wMsgLen );
		break;
		
	//外置矩阵所有端口名指示
	case h_ctrl_matrixExAllPortNameInd:	
		BuildAndSendMsgToMcu( MT_MCU_EXMATRIX_ALLPORTNAME_NOTIF, pbyMsg, wMsgLen );		
		break;

    //扩展视频源
    case h_ctrl_allVideoSourceInfoInd:
        BuildAndSendMsgToMcu( MT_MCU_ALLVIDEOSOURCEINFO_NOTIF, pbyMsg, wMsgLen );
        break;
        
    case h_ctrl_VideoSourceInfoInd:
        BuildAndSendMsgToMcu( MT_MCU_VIDEOSOURCEINFO_NOTIF, pbyMsg, wMsgLen );
        break;

    //mt接收码率通知, u16               
    case h_ctrl_terBandWidthInd:
        {
            BuildAndSendMsgToMcu(MT_MCU_BANDWIDTH_NOTIF, pbyMsg, wMsgLen);
        }        
        break;

    //mt对终端码率应答 , 消息体：TMtBitrate 
    case h_ctrl_terBitrateInfoRsp:
        {
            _TTERBITRATE tTerBitrate = *(_TTERBITRATE *)pbyMsg;
            TMtBitrate tMtBitrate = CMtAdpUtils::TMtBitrateOut2In(tTerBitrate);
            BuildAndSendMsgToMcu(MT_MCU_GETBITRATEINFO_NOTIF, (u8*)&tMtBitrate, sizeof(tMtBitrate));
        }
        break;  
        
    //主席终端强制广播命令, 消息体：enum OprType
    case h_ctrl_ViewBroadcastingSrcCmd:
        {
            u8 byMode = (*pbyMsg == (u8)opr_start) ? 1 : 0;
            BuildAndSendMsgToMcu(MT_MCU_VIEWBRAODCASTINGSRC_CMD, &byMode, sizeof(byMode));
        }
        break;

    //获取终端版本信息的响应
//     case h_ctrl_mtVerInfoRsp:
//         BuildAndSendMsgToMcu(MT_MCU_GETMTVERID_ACK, pbyMsg, wMsgLen);
//         break;
	// xliang [3/31/2009] 主席选看VMP //FIXME:
    case h_ctrl_mtSelectVmpReq:
		{
			u8 bStartSelVmp = *(u8*)pbyMsg;
			if(bStartSelVmp == 1)
			{
				BuildAndSendMsgToMcu( MT_MCU_STARTSWITCHVMPMT_REQ );
			}
			else
			{
				//取消选看
				TMt tDstMt;
				tDstMt.SetMt(/*(u8)g_cMtAdpApp.m_wMcuNetId*/LOCAL_MCUIDX, m_byMtId, g_cMtAdpApp.m_byDriId, m_byConfIdx);
				BuildAndSendMsgToMcu( MT_MCU_STOPSELMT_CMD, (u8*)&tDstMt, sizeof(tDstMt) );
			}
		}
		break;
	case h_ctrl_askVcsOccupyPermit:
		{
#ifdef _UTF8
			// 允许强拆时，需要对会议名称进行转换 [pengguofeng 5/23/2013]
			s8 achConfName[MAXLEN_CONFNAME];
			memset(achConfName, 0, sizeof(achConfName));
			BOOL32 bTrans = TransEncoding((s8*)pbyMsg, achConfName, sizeof(achConfName), FALSE);
			if ( bTrans )
			{
				BuildAndSendMsgToMcu(MT_MCU_RELEASEMT_REQ, (u8 *)achConfName, strlen(achConfName));
			}
			else
			{
				BuildAndSendMsgToMcu(MT_MCU_RELEASEMT_REQ, pbyMsg, wMsgLen);
			}
#else
			BuildAndSendMsgToMcu(MT_MCU_RELEASEMT_REQ, pbyMsg, wMsgLen);
#endif
		}
		break;

    case h_ctrl_applycancelspeakerReq:
		{
			BuildAndSendMsgToMcu(MT_MCU_APPLYCANCELSPEAKER_REQ, pbyMsg, wMsgLen);
		}
        break;

	// [pengjie 2011/4/12] 终端视频源别名上报支持
	case h_ctrl_videoAliasInd:
		{
#ifdef _UTF8
			// 终端视频源转码。。 [pengguofeng 5/23/2013]　MT_MAX_PORTNAME_LEN：16
			u8 byMtNum = *pbyMsg;
			u8 abyTransMsg[1+ (2+MT_MAX_PORTNAME_LEN)* MT_MAXNUM_VIDSOURCE];
			memset(abyTransMsg, 0, sizeof(abyTransMsg));
			u16 wOffSet = 0;
			abyTransMsg[wOffSet++] = byMtNum;
			pbyMsg++;
			u8 byAliasLen = 0;
			s8 achTempOrg[MT_MAX_PORTNAME_LEN]; //保存转换前字符串
			s8 achTempTrans[MT_MAX_PORTNAME_LEN]; //保存转换后字符串
			BOOL32 bTrans = FALSE;
			for ( u8 byMtLoop =0; byMtLoop < byMtNum;byMtLoop++)
			{
				abyTransMsg[wOffSet++] = *pbyMsg++;
				byAliasLen = *pbyMsg++;
				memset(achTempOrg, 0, sizeof(achTempOrg));
				memset(achTempTrans, 0, sizeof(achTempTrans));
				memcpy(achTempOrg, pbyMsg, byAliasLen);
				pbyMsg += byAliasLen;
				achTempOrg[MT_MAX_PORTNAME_LEN-1] = 0;
				bTrans = TransEncoding(achTempOrg, achTempTrans, sizeof(achTempTrans), FALSE);
				if ( bTrans )
				{
					byAliasLen = strlen(achTempTrans);
					abyTransMsg[wOffSet++] = byAliasLen;
					memcpy(abyTransMsg+wOffSet, achTempTrans, byAliasLen);
				}
				else
				{
					abyTransMsg[wOffSet++] = byAliasLen;
					memcpy(abyTransMsg+wOffSet, achTempOrg, byAliasLen);
				}
				wOffSet += byAliasLen;
			}
			BuildAndSendMsgToMcu(MT_MCU_MTVIDEOALIAS_NOTIF, abyTransMsg, wOffSet);
#else
			BuildAndSendMsgToMcu(MT_MCU_MTVIDEOALIAS_NOTIF, pbyMsg, wMsgLen);
#endif
		}
        break;

    case h_ctrl_transparentMsgNtf:
        {
            BuildAndSendMsgToMcu( MT_MCU_TRANSPARENTMSG_NOTIFY, pbyMsg, wMsgLen );
        }        
        break;

        //[4/8/2013 liaokang] 编码方式
    case h_ctrl_CodingFormNtf:
        {
            u8 byEncoding = *(u8 *)pbyMsg;
            SetEndPointEncoding((emenCodingForm)byEncoding);
            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcRadEvConfCtrl] Mcu Encoding%d, EndPoint Encoding%d,Encoding<1:UTF8,2:GBK>!\n",
                    g_cMtAdpApp.GetMcuEncoding(), GetEndPointEncoding());
        }        
        break;
	case h_ctrl_nonstandardVrsDef:
		{
			// 解析非标消息：消息号(u16)+消息长度+(u16)+消息体内容
			CServMsg cMsg;
			u8 *pbyBuf = pbyMsg;
			u16 wEvId = *(u16*)pbyBuf;
			wEvId = ntohs(wEvId);
			pbyBuf += sizeof(wEvId);
			u16 wLen = *(u16*)pbyBuf;
			wLen = ntohs(wLen);
			pbyBuf += sizeof(wLen);
			// 根据相应消息组织给mcu的相应消息
			ProcMcuGetMsgFormVrs(cMsg, wEvId, wLen, pbyBuf);
			// 发送消息
			SendMsgToMcu(cMsg);
		}
		break;
	default:
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcRadEvConfCtrl] Unexpected rad conf ctrl msg %u received. Ignore it.\n", wMsgType);
		break;
	}
	/*lint -restore*/
	return;
}

/*=============================================================================
  函 数 名： ProcRadEvFeccCtrl
  功    能： 处理协议栈传来的的摄像头远遥事件
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang              创建
=============================================================================*/
void CMtAdpInst::ProcRadEvFeccCtrl(CMessage * const pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	if( !BODY_LEN_GE( cServMsg, sizeof(HCALL) + sizeof(u16)) ) 
		return;

	HCALL hsCall   = *(HCALL*)cServMsg.GetMsgBody();
	u16   wMsgType = *(u16*)(cServMsg.GetMsgBody() + sizeof(HCALL));
	u8   *pbyMsg   =  (u8*)(cServMsg.GetMsgBody() + sizeof(HCALL) + sizeof(wMsgType));
	u16   wMsgLen  =  cServMsg.GetMsgBodyLen() - sizeof(HCALL) - sizeof(wMsgType);
	TMt   tMt      = *(TMt*)pbyMsg;
	u8    byAction = *( pbyMsg + sizeof(TMt) );

	if(CurState() == STATE_IDLE)
		return;
	
	if( m_hsCall != hsCall )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Fecc called back to incorrect instance!\n" );
		return;
	}
	
	//this means the terminal does not support multipoint 
	//conference, even though we sent it an MCC or TIA.
	//we should nullify the dst & src terLabel fields
	//when feccing them
	if( 0 == tMt.GetMcuId() && 0 == tMt.GetMtId() )
	{		
		for( s32 nCallChanNum = 0; nCallChanNum < MAXNUM_CALL_CHANNEL; nCallChanNum++ )
		{
			if( MEDIA_TYPE_H224 == m_atChannel[nCallChanNum].tLogicChan.GetChannelType() && 
				(u8)CHAN_OUTGOING == m_atChannel[nCallChanNum].byDirect )
			{
				TTERLABEL localTer, remoteTer;
				localTer.Clear();
				localTer.SetTerminalLabel( 0, 0 );
				remoteTer.Clear();
				remoteTer.SetTerminalLabel( 0, 0 );
				kdvFeccBind( m_atChannel[nCallChanNum].hsChan, localTer, remoteTer );					
			}
		}							
	}

	switch( wMsgType ) 
	{
	case MT_MCU_MTCAMERA_CTRL_CMD:
		{			
			switch( byAction )
			{
			case CAMERA_CTRL_UP: 			break;
			case CAMERA_CTRL_DOWN: 			break;
			case CAMERA_CTRL_LEFT:			break;
			case CAMERA_CTRL_RIGHT:			break;
			case CAMERA_CTRL_DOWNLEFT:		break;
			case CAMERA_CTRL_DOWNRIGHT:		break;
			case CAMERA_CTRL_ZOOMIN:		break;
			case CAMERA_CTRL_ZOOMOUT:		break;
			case CAMERA_CTRL_FOCUSIN:		break;
			case CAMERA_CTRL_FOCUSOUT:		break;
			case CAMERA_CTRL_BRIGHTUP:		break;
			case CAMERA_CTRL_BRIGHTDOWN:	break;			
			default:
				break;
			}
			break;
		}
		/*break;*/
	case MT_MCU_MTCAMERA_RCENABLE_CMD:		break;
	case MT_MCU_MTCAMERA_SAVETOPOS_CMD:		break;
	case MT_MCU_MTCAMERA_MOVETOPOS_CMD:		break;
	case MT_MCU_VIDEOSOURCESWITCHED_CMD:	break;
	case MT_MCU_SELECTVIDEOSOURCE_CMD:		break;

	default:
		break;
	}

	CServMsg cMsg;
	cMsg.SetEventId( wMsgType );
	cMsg.SetMsgBody( pbyMsg, wMsgLen );
	SendMsgToMcu( cMsg );

	return;
}

/*=============================================================================
  函 数 名： ProcRadEvMmcuCtrl
  功    能： 处理协议栈传来的的级联事件
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcRadEvMmcuCtrl( CMessage * const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	CServMsg cMsg;

	u16     wMsgType = *(u16 *)(cServMsg.GetMsgBody() );
	//HCALL   hsCall   = *(HCALL *)(cServMsg.GetMsgBody() + sizeof(u16) );
	HCHAN   hsChan   = *(HCHAN *)(cServMsg.GetMsgBody() + sizeof(u16) + sizeof(HCALL) );
	u8     *pbyBuf   =  (cServMsg.GetMsgBody() + sizeof(u16) + sizeof(HCALL) + sizeof(HCHAN) );
	s32     nBufLen  =  cServMsg.GetMsgBodyLen() - (sizeof(u16) + sizeof(HCALL) + sizeof(HCHAN) );
	
	cMsg.SetEventId(0);

	if(CurState() == STATE_IDLE)
		return;

	switch ( wMsgType)
	{		
	case H_CASCADE_REGUNREG_REQ://[TRegUnRegReq]	register request
		{			
			TRegUnRegReq *ptReg = (TRegUnRegReq *)pbyBuf;
			SetPeerReqId( (u32)ptReg->m_nReqID );
			BOOL32 bSupportMultiCas = ptReg->m_bIsSupportMuitiCasade;
			//接收到上级mcu注册请求时，提取并上报会议密码，以供上层业务认证 2005-11-02
			cMsg.SetMsgBody( (u8*)ptReg->m_aszUserPass, (u16)strlen(ptReg->m_aszUserPass) );
			cMsg.CatMsgBody((u8 *)&bSupportMultiCas, sizeof(bSupportMultiCas));
			cMsg.SetEventId(MCU_MCU_REGISTER_NOTIF);

			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "send msg MCU_MCU_REGISTER_NOTIF. MtId.%d\n", m_byMtId);

			// [6/14/2011 xliang] smcu send H_CASCADE_REGUNREG_REQ to mmcu here rather than h_chan_connected callback spot
			s32 nChanIdx = FindChannel(hsChan);
			if ( m_atChannel[nChanIdx].byDirect == (u8)CHAN_INCOMING )
			{
				TRegUnRegReq tReq;
				memset( &tReq, 0, sizeof(tReq) );
				
				//上级mcu发送注册请求时设置会议密码，以便进行呼叫认证 2005-11-02
				if ( m_bMaster && 0 != strlen(m_achConfPwd) )
				{
					tReq.SetUserNameAndPass("rvmcu", m_achConfPwd);
				}
				tReq.m_nReqID = m_dwMMcuReqId ++ ;
				tReq.m_bReg = TRUE;
				
				//是否支持多级联// [7/9/2010 xliang] 根据具体情况赋值,暂置TRUE
				tReq.m_bIsSupportMuitiCasade = TRUE;
				
				kdvSendMMcuCtrlMsg( m_atChannel[nChanIdx].hsChan, (u16)H_CASCADE_REGUNREG_REQ, &tReq, sizeof(tReq), FALSE );
			}

			break;
		}
// 	case H_CASCADE_INIT_CASC_CMD: // scopia cascade --xliang
// 		{
// 			
// 			cMsg.SetMsgBody();
// 			cMsg.SetEventId(MCU_MCU_REGISTER_NOTIF);
// 			MtAdpInfo("send msg MCU_MCU_REGISTER_NOTIF. MtId.%d\n", m_byMtId);
// 			break;
// 		}
	case H_CASCADE_REGUNREG_RSP://[IN]&[OUT]:[TRegUnRegRsp]	register response
		{
		//	cMsg.SetEventId(MCU_MCU_REGISTER_NOTIF);
			break;
		}
	case H_CASCADE_NEWROSTER_NTF:  //[IN]&[OUT]:[TRosterList]  roster notify
		{
			ProcNewRosterNotify( cMsg, pbyBuf, nBufLen );
			break;
		}

	case H_CASCADE_PARTLIST_REQ:   //[IN]&[OUT]:[TReq]         request part list 
		{			
			ProcPartListReq( cMsg, pbyBuf, nBufLen );
			MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "Recv H_CASCADE_PARTLIST_REQ From Cascadelib. mtid = %d\n", m_byMtId);
            g_cMtAdpApp.m_tMaStaticInfo.m_dwRcvMtListReqNum++;
			break;
		}

	case H_CASCADE_PARTLIST_RSP: //[IN]&[OUT]:[TPartListRsp] reponse part list (dwpid ==0 表示对方的MCU)
		{
			ProcPartListRsp( cMsg, pbyBuf, nBufLen );
			break;
		}

	case H_CASCADE_INVITEPART_REQ://[IN]&[OUT]:[TInvitePartReq]  invite part request
		{ 
			ProcInviteMtReq( cMsg, pbyBuf, nBufLen );
			break;
		}

	case H_CASCADE_INVITEPART_RSP://[IN]&[OUT]:[Tsp]             invite part response
		{			
			ProcInviteMtRsp( cMsg, pbyBuf, nBufLen );
			break;
		}

	case H_CASCADE_NEWPART_NTF://[IN]&[OUT]:[TPart]           new part notify
		{
			ProcNewMtNotify( cMsg, pbyBuf, nBufLen );
			break;
		}

	case H_CASCADE_CALL_ALERTING_NTF: //[IN]&[OUT]:[TCallNtf]
		{
			ProcCallAlertMtNotify( cMsg, pbyBuf, nBufLen );
			break;
		}

	case H_CASCADE_REINVITEPART_REQ: //[IN]&[OUT]:[TPartReq]    reinvite part request
		{
			ProcReInviteMtReq( cMsg, pbyBuf, nBufLen );
			break;
		}

	case H_CASCADE_REINVITEPART_RSP: //[IN]&[OUT]:[Tsp]        reinvite part response
		{
			TPartRsp *ptRsp = (TPartRsp *)pbyBuf;
			TMsgHeadMsg tHeadMsg;
			TMt tMt = GetMtFromPartId( ptRsp->m_dwPID,FALSE );
			tHeadMsg.m_tMsgSrc.m_byCasLevel = ptRsp->m_tMsgSrc.m_byCasLevel;
			//tHeadMsg.m_tMsgSrc.m_tMt = tMt;

			tHeadMsg.m_tMsgDst.m_byCasLevel = ptRsp->m_tMsgDst.m_byCasLevel;
			
			if( emReturnValue_Ok == ptRsp->m_emReturnVal )
			{
				cMsg.SetEventId( MCU_MCU_REINVITEMT_ACK );
			}
			else
			{
				cMsg.SetEventId( MCU_MCU_REINVITEMT_NACK );
			}
			cMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
			cMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
			break;
		}

	case H_CASCADE_DISCONNPART_REQ: //[IN]&[OUT]:[TPartID]     disconnect part request
		{
			ProcDropMtReq( cMsg, pbyBuf, nBufLen );
			break;
		}

	case H_CASCADE_DISCONNPART_RSP: //[IN]&[OUT]:[Tsp]         disconnect part response
		{
			TPartRsp *ptRsp = (TPartRsp *)pbyBuf;
			TMsgHeadMsg tHeadMsg;
			TMt tMt = GetMtFromPartId( ptRsp->m_dwPID,FALSE );
			tHeadMsg.m_tMsgSrc.m_byCasLevel = ptRsp->m_tMsgSrc.m_byCasLevel;
			//tHeadMsg.m_tMsgSrc.m_tMt = tMt;

			tHeadMsg.m_tMsgDst.m_byCasLevel = ptRsp->m_tMsgDst.m_byCasLevel;
			
			cMsg.SetEventId( emReturnValue_Ok == ptRsp->m_emReturnVal ? MCU_MCU_DROPMT_ACK: MCU_MCU_DROPMT_NACK);
			cMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
			cMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
			break;
		}

	case H_CASCADE_DISCONNPART_NTF: //[IN]&[OUT]:[TDiscPartNF] disconnect part notify
		{
			TDiscPartNF* ptNF = (TDiscPartNF*)pbyBuf;
			PartIdUpdate( ptNF->m_dwPID, ptNF->m_dwNewPID, ptNF->m_bLocal );
			TMt tMt = GetMtFromPartId( ptNF->m_dwNewPID, ptNF->m_bLocal );

			TMsgHeadMsg tHeadMsg;
			tHeadMsg.m_tMsgSrc.m_byCasLevel = ptNF->m_tMsgSrc.m_byCasLevel;
			memcpy( &tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
					&ptNF->m_tMsgSrc.m_abyMtIdentify[0],
					sizeof( tHeadMsg.m_tMsgSrc.m_abyMtIdentify )
					);

			tHeadMsg.m_tMsgDst.m_byCasLevel = ptNF->m_tMsgDst.m_byCasLevel;
			memcpy( &tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
					&ptNF->m_tMsgDst.m_abyMtIdentify[0],
					sizeof( tHeadMsg.m_tMsgDst.m_abyMtIdentify )
					);

			LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcRadEvMmcuCtrl]recv drop mt<%d %d> lvl.%d\n",
				tMt.GetMcuId(), tMt.GetMtId(), ptNF->m_tMsgSrc.m_byCasLevel);
			// 如果挂断的是三级的MCU [pengguofeng 7/4/2013]
			// 先不清，防止挂断再呼时有意外
			if ( ptNF->m_tMsgSrc.m_byCasLevel == 0 )
			{
				TMcuList *ptMcuList = g_cMtAdpApp.m_tAllSMcuList.GetMcuList((u8) GetInsID() );
				if ( ptMcuList )
				{
					TMtList *ptMtList = ptMcuList->GetMtList(tMt.GetMtId());
					if ( ptMtList )
					{
						LogPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcRadEvMmcuCtrl] tMt<%d,%d> is Mcu,clear it\n",
							tMt.GetMcuId(), tMt.GetMtId());
						ptMcuList->DelMtList(tMt.GetMtId());
					}
					else
					{
						LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcRadEvMmcuCtrl] cannot find tMt<%d,%d> as SMCU\n",
							tMt.GetMcuId(), tMt.GetMtId());
					}
				}
			}
			cMsg.SetEventId( MCU_MCU_DROPMT_NOTIF );
			cMsg.SetMsgBody( (u8 *)&tMt, sizeof(tMt) );
			cMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
			break;
		}

	case H_CASCADE_DELETEPART_REQ: //[IN]&[OUT]:[TPartReq]		delete part request
		{
			ProcDelMtReq( cMsg, pbyBuf/*, nBufLen*/ );
			break;
		}

	case H_CASCADE_DELETEPART_RSP: //[IN]&[OUT]:[Tsp]			delete part response
		{
			TPartRsp *ptRsp = (TPartRsp *)pbyBuf;
			TMsgHeadMsg tHeadMsg;
			TMt tMt = GetMtFromPartId( ptRsp->m_dwPID,FALSE );
			tHeadMsg.m_tMsgSrc.m_byCasLevel = ptRsp->m_tMsgSrc.m_byCasLevel;
			//tHeadMsg.m_tMsgSrc.m_tMt = tMt;

			tHeadMsg.m_tMsgDst.m_byCasLevel = ptRsp->m_tMsgDst.m_byCasLevel;
			
			cMsg.SetEventId( emReturnValue_Ok == ptRsp->m_emReturnVal ? MCU_MCU_DELMT_ACK: MCU_MCU_DELMT_NACK );
			cMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
			cMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
			break;
		}

	case H_CASCADE_DELETEPART_NTF: //[IN]&[OUT]:[TPartID]   delete part notify (whether or not local)
		{
			TDelPartNF *ptNF = (TDelPartNF *)pbyBuf;
			TMt tMt = GetMtFromPartId( ptNF->m_dwPID, ptNF->m_bLocal );

			// 收到删除终端的通知后，需要把保存的名字删除 [pengguofeng 7/4/2013]
			u8 byInsId = (u8)GetInsID();
			u8 byMcuMtId = 0xFF;
			u8 byDelMtId = 0;
			if ( ptNF->m_tMsgSrc.m_byCasLevel == 0 )
			{
				//二级删除:0 + tMt.MtId
				byMcuMtId = 0;
				byDelMtId = tMt.GetMtId();
			}
			else
			{
				//三级删除:tMt.MtId + identify[0]
				byMcuMtId = tMt.GetMtId();
				byDelMtId = ptNF->m_tMsgSrc.m_abyMtIdentify[0];
			}

			TMcuList *ptMcuList = g_cMtAdpApp.m_tAllSMcuList.GetMcuList(byInsId);
			if ( ptMcuList )
			{
				TMtList *ptMtList = ptMcuList->GetMtList(byMcuMtId);
				if ( ptMtList )
				{
					LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcRadEvMmcuCtrl]del mt<%d.%d.%d>\n", byInsId, byMcuMtId, byDelMtId);
					ptMtList->DelMtName(byDelMtId);
				}
				else
				{
					LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcRadEvMmcuCtrl]no mt<%d.%d.%d> can be found\n", byInsId, byMcuMtId, byDelMtId);
				}

				//有可能删除三级MCU，操作同删除二级，但是要多加一个清除MCU
				if ( ptNF->m_tMsgSrc.m_byCasLevel == 0 )
				{
					ptMtList = ptMcuList->GetMtList(tMt.GetMtId());
					if ( ptMtList )
					{
						LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcRadEvMmcuCtrl]del smcu.%d\n", tMt.GetMtId());
						ptMcuList->DelMtList(tMt.GetMtId());
					}
					else
					{
						LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcRadEvMmcuCtrl] cannot find tMt<%d,%d> as SMCU\n",
							tMt.GetMcuId(), tMt.GetMtId());
					}
				}
			}
			else
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcRadEvMmcuCtrl]no mt<%d.%d.%d> can be found\n", byInsId, byMcuMtId, byDelMtId);
			}

			if( !ptNF->m_bLocal && ptNF->m_tMsgSrc.m_byCasLevel == 0 )
			{
				DelPartIdFromTable( ptNF->m_dwPID );
			}
			
			TMsgHeadMsg tHeadMsg;
			tHeadMsg.m_tMsgSrc.m_byCasLevel = ptNF->m_tMsgSrc.m_byCasLevel;
			memcpy( &tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
					&ptNF->m_tMsgSrc.m_abyMtIdentify[0],
					sizeof( tHeadMsg.m_tMsgSrc.m_abyMtIdentify )
					);

			tHeadMsg.m_tMsgDst.m_byCasLevel = ptNF->m_tMsgDst.m_byCasLevel;
			memcpy( &tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
					&ptNF->m_tMsgDst.m_abyMtIdentify[0],
					sizeof( tHeadMsg.m_tMsgDst.m_abyMtIdentify )
					);

			cMsg.SetEventId( MCU_MCU_DELMT_NOTIF );
			cMsg.SetMsgBody( (u8 *)&tMt, sizeof(tMt) );	
			cMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
			break;
		}

	case H_CASCADE_SETIN_REQ:      //[IN]&[OUT]:[TSetInReq]    request video in
		{
			ProcSetInReq( cMsg, pbyBuf, nBufLen );
			break;
		}

	case H_CASCADE_SETIN_RSP:     //[IN]&[OUT]:[Tsp]          response msg
		{
			TPartRsp *ptRsp = (TPartRsp *)pbyBuf;
			TMsgHeadMsg tHeadMsg;
			TMt tMt = GetMtFromPartId( ptRsp->m_dwPID,FALSE );
			tHeadMsg.m_tMsgSrc.m_byCasLevel = ptRsp->m_tMsgSrc.m_byCasLevel;
			//tHeadMsg.m_tMsgSrc.m_tMt = tMt;

			tHeadMsg.m_tMsgDst.m_byCasLevel = ptRsp->m_tMsgDst.m_byCasLevel;
			

			cMsg.SetEventId( emReturnValue_Ok == ptRsp->m_emReturnVal ? MCU_MCU_SETIN_ACK: MCU_MCU_SETIN_NACK );
			cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
			cMsg.CatMsgBody( (u8 *)&tMt, sizeof(tMt) );
			//自动请求级联音视频源信息
			TMcuMcuReq tReq;
			memset(&tReq, 0, sizeof(tReq));
			cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));

			cServMsg.SetEventId(MCU_MCU_AUDIOINFO_REQ);
			post(MAKEIID(AID_MCU_MTADP, GetInsID()), cServMsg.GetEventId(),
				 cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

			cServMsg.SetEventId(MCU_MCU_VIDEOINFO_REQ);
			post(MAKEIID(AID_MCU_MTADP, GetInsID()), cServMsg.GetEventId(),
				 cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			
			break;
		}

	case H_CASCADE_SETOUT_REQ:     //[IN]&[OUT]:[TSetOutReq]//视频输出到自己的请求(视频流向:MCU->MT)
		{
			ProcSetOutReq( cMsg, pbyBuf, nBufLen );
			break;
		}

	case H_CASCADE_SETOUT_RSP:    //[IN]&[OUT]:[Tsp]
		{
			TRsp *ptRsp = (TRsp *)pbyBuf;
			cMsg.SetEventId( emReturnValue_Ok == ptRsp->m_emReturnVal ? MCU_MCU_SETOUT_ACK: MCU_MCU_SETOUT_NACK );
			break;
		}
		
	case H_CASCADE_OUTPUT_NTF:     //[IN]&[OUT]:[TPartOutputNtf]      video output notify (dwpid ==0 表示对方的MCU)
		{
			ProcSetOutNtf( cMsg, pbyBuf, nBufLen );
			break;
		}

	case H_CASCADE_NEWSPEAKER_NTF: //[IN]&[OUT]:[TNewSpeakerNtf]      newSpeaker output notify (dwpid ==0 表示对方的MCU)
		{
			break;
		}
	case H_CASCADE_AUDIOINFO_REQ: //[IN]&[OUT]:[TReq]         audio info request 
		{
			TReq *ptReq = (TReq *)pbyBuf;
			SetPeerReqId( (u32)ptReq->m_nReqID );
			TMcuMcuReq tMcuMcuReq;
			// 多国语言支持 [pengguofeng 4/8/2013]
			s8 achUserName[CASCADE_USERNAME_MAXLEN];
			memset(achUserName, 0, sizeof(achUserName));
			BOOL32 bNeedTrans = TransEncoding(ptReq->m_aszUserName, achUserName, sizeof(achUserName), FALSE);
			if ( bNeedTrans == TRUE)
			{
				astrncpy( tMcuMcuReq.m_szUserName, achUserName,
					sizeof(tMcuMcuReq.m_szUserName), sizeof(achUserName) );
			}
			else
			{
				astrncpy( tMcuMcuReq.m_szUserName, ptReq->m_aszUserName,
					sizeof(tMcuMcuReq.m_szUserName), sizeof(ptReq->m_aszUserName) );
			}
			
			astrncpy( tMcuMcuReq.m_szUserPwd, ptReq->m_aszUserPass,
				sizeof(tMcuMcuReq.m_szUserPwd), sizeof(ptReq->m_aszUserPass) );
			cMsg.SetMsgBody( (u8 *)&tMcuMcuReq, sizeof(TMcuMcuReq) );
			cMsg.SetEventId( MCU_MCU_AUDIOINFO_REQ );
            g_cMtAdpApp.m_tMaStaticInfo.m_dwRcvAudInfoReqNum++;
			break;
		}

	case H_CASCADE_AUDIOINFO_RSP:  //[IN]&[OUT]:[TConfAudioInfo]    audio info response
		{
			ProcAudioInfoRsp( cMsg, pbyBuf, nBufLen );
			break;
		}

	case H_CASCADE_VIDEOINFO_REQ: //[IN]&[OUT]:[TReq]          conference videoinfo request 
		{
			TReq *ptReq = (TReq *)pbyBuf;
			SetPeerReqId( (u32)ptReq->m_nReqID );
			TMcuMcuReq tMcuMcuReq;
			// 多国语言 [pengguofeng 4/8/2013]
			s8 achUserName[CASCADE_USERNAME_MAXLEN];
			memset(achUserName, 0, sizeof(achUserName));
			BOOL32 bNeedTrans = TransEncoding(ptReq->m_aszUserName, achUserName, sizeof(achUserName), FALSE);
			if ( bNeedTrans == TRUE)
			{
				astrncpy( tMcuMcuReq.m_szUserName, achUserName,
					sizeof(tMcuMcuReq.m_szUserName), sizeof(achUserName) );
			}
			else
			{
				astrncpy( tMcuMcuReq.m_szUserName, ptReq->m_aszUserName,
					sizeof(tMcuMcuReq.m_szUserName), sizeof(ptReq->m_aszUserName) );
			}

			astrncpy(tMcuMcuReq.m_szUserPwd, ptReq->m_aszUserPass,
				sizeof(tMcuMcuReq.m_szUserPwd), sizeof(ptReq->m_aszUserPass) );
			cMsg.SetMsgBody( (u8 *)&tMcuMcuReq, sizeof(TMcuMcuReq) );

			cMsg.SetEventId( MCU_MCU_VIDEOINFO_REQ );
            g_cMtAdpApp.m_tMaStaticInfo.m_dwRcvVidInfoReqNum++;
			break;
		}

	case H_CASCADE_VIDEOINFO_RSP: //[IN]&[OUT]:[TConfVideoInfo] conference videoinfo response
		{
			ProcVideoInfoRsp( cMsg, pbyBuf, nBufLen );
			break;
		}
		
	case H_CASCADE_CONFVIEW_CHG_NTF://[IN]&[OUT]:[TConfViewChgNtf] conference view format(layout geometry)change notify
		{
			TConfViewChgNtf* ptNF = (TConfViewChgNtf *)pbyBuf;
			TCConfViewChangeNtf tInNtf;
			tInNtf.m_nViewID = ptNF->m_nViewID;
			tInNtf.m_bySubframeCount = ptNF->m_dwSubFrameCount;
			cMsg.SetEventId( MCU_MCU_CONFVIEWCHG_NOTIF );
			cMsg.SetMsgBody( (u8 *)&tInNtf, sizeof(tInNtf) );	
			break;
		}

	case H_CASCADE_PARTMEDIACHAN_REQ:   //[IN]&[OUT]:[TPartMediaChan]
		{
			ProcSetPartChanReq( cMsg, pbyBuf, nBufLen );
			break;
		}

	case H_CASCADE_PARTMEDIACHAN_RSP:  //[IN]&[OUT]:[TRsp]
		{
			TRsp *ptRsp = (TRsp *)pbyBuf;
			cMsg.SetEventId( emReturnValue_Ok == ptRsp->m_emReturnVal ? MCU_MCU_SETMTCHAN_ACK: MCU_MCU_SETMTCHAN_NACK );
			break;
		}
		
	case H_CASCADE_PARTMEDIACHAN_NTF:  //[IN]&[OUT]:[TPartMediaChan]
		{
			TPartMediaChan *ptChan = (TPartMediaChan *)pbyBuf;
			TMtMediaChanStatus tStatus;
			tStatus.m_tMt = GetMtFromPartId(ptChan->m_dwPID, ptChan->m_bLocal);
			tStatus.m_byMediaMode = CMtAdpUtils::MediaModeOut2In(ptChan->m_emMediaType);
			tStatus.m_byIsDirectionIn = GETBBYTE( ptChan->m_emMuteDirection == emDirectionIn );
			tStatus.m_bMute = ptChan->m_bMute;
			cMsg.SetEventId( MCU_MCU_SETMTCHAN_NOTIF );
			cMsg.SetMsgBody( (u8 *)&tStatus, sizeof(tStatus) );
			break;
		}
        // guzh [6/26/2007] Radvision 级联轮询请求/应答
    case H_CASCADE_SET_LAYOUT_AUTOSWITCH_REQUEST:   //[IN]&[OUT]:TAutoswitchReq
        {
            TAutoswitchReq *ptSwitchReq = (TAutoswitchReq*)pbyBuf;
            TCAutoSwitchReq tSwitchReq;
            tSwitchReq.m_bSwitchOn = ptSwitchReq->GetAutoSwitchOn();
            tSwitchReq.m_nSwitchSpaceTime = ptSwitchReq->GetAutoSwitchTime();
            tSwitchReq.m_nAutoSwitchLevel = ptSwitchReq->GetAutoSwitchLevel();
            tSwitchReq.m_nSwitchLayerId = ptSwitchReq->GetLid();
            
            cMsg.SetEventId(MCU_MCU_AUTOSWITCH_REQ);
            cMsg.SetMsgBody((u8*)&tSwitchReq, sizeof(tSwitchReq));
            break;   
        }
    case H_CASCADE_SET_LAYOUT_AUTOSWITCH_RESPONSE:  //TAutoswitchRsp
        {
            TAutoswitchRsp *ptRsp = (TAutoswitchRsp*)pbyBuf;
            cMsg.SetEventId( emReturnValue_Ok == ptRsp->m_emReturnVal ? MCU_MCU_AUTOSWITCH_ACK: MCU_MCU_AUTOSWITCH_NACK );
            break;
        }
		
	case H_CASCADE_NONSTANDARD_REQ:
		{
			TNonStandardReq *ptReq = (TNonStandardReq *)pbyBuf;
			ProcNonStandardMsg( cMsg, ptReq->m_abyMsgBuf, ptReq->m_nMsgLen );
			break;
		}

	case H_CASCADE_NONSTANDARD_RSP:
		{
			TNonStandardRsp *ptRsp = (TNonStandardRsp *)pbyBuf;
			ProcNonStandardMsg( cMsg, ptRsp->m_abyMsgBuf, ptRsp->m_nMsgLen );
			break;
		}

	case H_CASCADE_NONSTANDARD_NTF:
		{
			TNonStandardMsg *ptMsg = (TNonStandardMsg *)pbyBuf;
			ProcNonStandardMsg( cMsg, ptMsg->m_abyMsgBuf, ptMsg->m_nMsgLen );
			break;
		}

	default:return;

	}
	
	if( cMsg.GetEventId() != 0 )
	{
		SendMsgToMcu( cMsg );
	}

	return;
}

/*=============================================================================
  函 数 名： ProcRegunregReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
// void CMtAdpInst::ProcRegunregReq( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
// {
// 	
// }

/*=============================================================================
  函 数 名： ProcNewRosterNotify
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcNewRosterNotify(CServMsg& cMsg, u8* pbyBuf, s32 nBufLen)
{
	if( (u32)nBufLen < sizeof(TRosterList))
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcNewRosterNotify] nBufLen isn't correct ! return !\n");
		return;
	}
	//新列表通知
	TRosterList *ptList = (TRosterList *)pbyBuf; 

	//fxh 新版本仅在收到最后一个空ROSTER再触发mtlist请求发送
	//为兼容老版本，设置定时器，若未收到空ROSTER，由30s的定时器触发mtlist请求
	u8 byRosterOver = (ptList->m_nCount ? FALSE : TRUE);
	if (byRosterOver)
	{
		KillTimer(TIMER_MTLIST);
	}
	else
	{
		KillTimer(TIMER_MTLIST);
		SetTimer(TIMER_MTLIST, 30 * 1000);
	}	

	m_tMtInfo.SetMcuIdx(m_byMtId);
	m_tMtInfo.SetConfIdx(m_byConfIdx);	
    u8 byLoop = 0;

	//花名册可能为增量通知
    for( byLoop = 0; byLoop < ptList->m_nCount && (byLoop + m_dwPartIdNum) < MAXNUM_CONF_MT; byLoop ++ )
    {
        m_atPartIdTable[byLoop+m_dwPartIdNum].byMtId   = (u8)(byLoop + m_dwPartIdNum + 1);

		m_atPartIdTable[byLoop+m_dwPartIdNum].dwPartId = ptList->m_atRoster[byLoop].m_dwPID;
		
		m_tMtInfo.RemoveMt( (u8)(byLoop + m_dwPartIdNum + 1) );
		m_tMtInfo.AddMt( (u8)(byLoop + m_dwPartIdNum + 1) );
		if( emConnStateConnected == ptList->m_atRoster[byLoop].m_emConnState )
		{
			m_tMtInfo.AddJoinedMt( (u8)(byLoop+m_dwPartIdNum + 1) );
		}
	}
	m_dwPartIdNum += byLoop;
	
	cMsg.SetEventId( MCU_MCU_ROSTER_NOTIF );
	cMsg.SetMsgBody( (u8 *)&m_tMtInfo, sizeof(m_tMtInfo) );

	//如果是R2武警版本则直接上报mcu可以获取对端mcu列表
	if( FALSE == byRosterOver && ( strcmp(GetPeerVersionID(m_hsCall), "4.0" ) == 0 ||
				strcmp(GetPeerVersionID(m_hsCall), "5" ) == 0
		)
		)
	{
		byRosterOver = TRUE;
		KillTimer(TIMER_MTLIST);
	}
	
	// fxh增加花名册通告结束信息
	cMsg.CatMsgBody(&byRosterOver, sizeof(byRosterOver));

	return;
}

/*=============================================================================
  函 数 名： ProcPartListReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcPartListReq( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	if( (u32)nBufLen < sizeof(TReq) )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcPartListReq] nBufLen is smaller than TReq size!\n");
	}
	TReq *ptReq = (TReq *)pbyBuf;
	SetPeerReqId( (u32)ptReq->m_nReqID );
	TMcuMcuReq tMcuMcuReq;

	astrncpy( tMcuMcuReq.m_szUserName, ptReq->m_aszUserName,
		      sizeof(tMcuMcuReq.m_szUserName), sizeof(ptReq->m_aszUserName) );
	
	astrncpy( tMcuMcuReq.m_szUserPwd, ptReq->m_aszUserPass,
		      sizeof(tMcuMcuReq.m_szUserPwd), sizeof(ptReq->m_aszUserPass) );
	
	cMsg.SetEventId( MCU_MCU_MTLIST_REQ );
	cMsg.SetMsgBody( (u8 *)&tMcuMcuReq, sizeof(TMcuMcuReq) );

	return;
}

/*=============================================================================
  函 数 名： ProcPartListRsp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
    2005/9/26   3.6			万春雷                  创建
=============================================================================*/
void CMtAdpInst::ProcPartListRsp( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	TPartListMsg *ptRsp = (TPartListMsg *)pbyBuf;
	if( emReturnValue_Ok != ptRsp->m_emReturnVal )
	{
		MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "Recv H_CASCADE_PARTLIST_RSP From Cascadelib. MCU_MCU_MTLIST_NACK 1- mtid = %d\n", m_byMtId);
		cMsg.SetEventId( MCU_MCU_MTLIST_NACK );
		return;
	}
	if( (u32)nBufLen != (sizeof(TPartListMsg) + (u32)ptRsp->m_dwPartCount*sizeof(TPart)))
	{
		MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "Recv H_CASCADE_PARTLIST_RSP From Cascadelib. MCU_MCU_MTLIST_NACK 2- mtid = %d len:%d num:%d tPartU:%d partMsg:%d\n",
			m_byMtId, nBufLen, ptRsp->m_dwPartCount, sizeof(TPart), sizeof(TPartListMsg));
		cMsg.SetEventId(MCU_MCU_MTLIST_NACK);
		return;
	}
	
	TPart *ptPartPrtU = NULL;//(TPartU *)(pbyBuf + sizeof(TPartListMsg));
	u8 byMcuMtId = 0xFF;

	TMt tMt;
	tMt.SetNull();
	{
		ptPartPrtU = (TPart *)(pbyBuf + sizeof(TPartListMsg));

// 		StaticLog("加一段打印吧:\n");
// 		for ( u8 byId = 0; byId < ptRsp->m_dwPartCount; byId++)
// 		{
// 			ptPartPrtU[byId].Print();
// 		}
// 		StaticLog("收尾-------:\n");

		if ( ptPartPrtU->m_byCasLevel == 0 )
		{
			byMcuMtId = 0;  //二级MCU保存在0位置
		}
		else
		{
			for ( u8 byMt = 0; byMt < ptRsp->m_dwPartCount; byMt++ )
			{
				if ( ptPartPrtU[byMt].m_byCasLevel != 0
					&& ptPartPrtU[byMt].m_abyMtIdentify[0] != 0)
				{
					// 三级终端要保存的是三级MCU所在二级的MtId [pengguofeng 7/5/2013]
					tMt = GetMtFromPartId(ptPartPrtU[byMt].m_dwPID, ptPartPrtU[byMt].m_bLocal);
					byMcuMtId = tMt.GetMtId()/*(u8)tMt.GetMcuId()*/;//其他mcu保存在McuId的位置
					break;
				}
				else
				{
					LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcPartListRsp] ptPartPrtU[%d] is Not SubMt\n", byMt);
				}
			}
		}
	}

	TMcuMcuMtInfo tInfo;
	
	// 处理多个MtList [pengguofeng 6/26/2013] 注意：此处是取的第一个，当然目前不可能有多个MCU来报PartList_Rsp的情况
	u8 byInsId = (u8)GetInsID();
	TMcuList *ptMcuList = g_cMtAdpApp.m_tAllSMcuList.GetMcuList(byInsId);
	LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcPartListRsp]Ins:%d McuList.IsNull:%d Add to Mcu.%d\n",
		byInsId, ptMcuList==NULL?1:0, byMcuMtId);

	TMcuList tMcuList; //本mtadp所连的2级MCU
	tMcuList.Init();
	TMtList tMtList;
	tMtList.Init();

	if ( ptMcuList != NULL )
	{
		TMtList *ptMtList = ptMcuList->GetMtList( byMcuMtId);
		if ( ptMtList)
		{
			LogPrint(LOG_LVL_DETAIL,MID_MCULIB_MTADP, "[ProcPartListRsp]get existing MtList of Mcu.%d\n", byMcuMtId);
			memcpy(&tMtList, ptMtList, sizeof(TMtList));
		}
	}

	MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[[ProcPartListRsp]][conf.%d]mcumt.%d recv %d mtlist\n", m_byConfIdx, m_byMtId, ptRsp->m_dwPartCount);
	for( s32 nLoop = 0, nMtLoop = 0; nMtLoop < MAXNUM_CONF_MT && nLoop < ptRsp->m_dwPartCount; nLoop ++, ptPartPrtU++)
	{
		memset(&tInfo, 0, sizeof(tInfo));
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcPartListRsp]add partname:(%s ) to ", ptPartPrtU->m_aszPartName);
			Part2MtInfo(ptPartPrtU, &tInfo);
			if ( ptPartPrtU->m_byCasLevel == 0 )
			{
				//2级加到MtId里去
				LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, " Mt.<%d %d> EpDesc.%s\n",
					tInfo.m_tMt.GetMcuId(), tInfo.m_tMt.GetMtId(), ptPartPrtU->m_aszEPDesc);
				tMtList.AddMtName(tInfo.m_tMt.GetMtId(), tInfo.m_szMtName);

// 				// 如果是第三级的mcu, [pengguofeng 7/11/2013]
 				if ( memcmp(ptPartPrtU->m_aszEPDesc, "kdcmcu", sizeof("kdcmcu")) == 0 )
 				{
 					tMtList.AddMcuName(/*ptPartPrtU->*/tInfo.m_szMtName);
 				}
			}
			else
			{
				//3级加到m_byCasIdentify[0]中去，特殊的0不用加，是三级的二级SMCU自己
				if ( ptPartPrtU->m_abyMtIdentify[0] != 0 )
				{
					LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, " identify Mt.%d\n", ptPartPrtU->m_abyMtIdentify[0]);
					tMtList.AddMtName(ptPartPrtU->m_abyMtIdentify[0], tInfo.m_szMtName);
				}
				else
				{
					LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcPartListRsp]partU--m_byCasLevel:%d m_abyMtIdentify[0]:%d m_aszPartName:(%s ) EpDesc.%s\n",
						ptPartPrtU->m_byCasLevel, ptPartPrtU->m_abyMtIdentify[0], ptPartPrtU->m_aszPartName, ptPartPrtU->m_aszEPDesc);
					if ( memcmp(ptPartPrtU->m_aszEPDesc, "kdcmcu", sizeof("kdcmcu")) == 0 )
					{
						tMtList.AddMcuName(/*ptPartPrtU->m_aszPartName*/tInfo.m_szMtName);
					}
				}
			}
		}
		
		// [11/21/2011 liuxu] 自动往缓存里加, 如果缓存加满了,就自动发送
		if (m_cSMcuMtListSendBuf.IsFull())
		{
			SendSMcuMtListInBuf( FALSE );
		}
		
		m_cSMcuMtListSendBuf.Add(tInfo);
		if (!m_bySMcuMtListBufTimerStart && g_cMtAdpApp.m_byEnableBufSendSMcuMtList)
		{
			// 500毫秒上报
			SetTimer(TIMER_SEND_SMCU_MTLIST, 500);
			m_bySMcuMtListBufTimerStart = 1;
		}
	}


	// 1.添加到内存中，以便mcu来请求
	if ( ptMcuList != NULL )
	{
		ptMcuList->AddMtList(byMcuMtId, tMtList);
	}
	else
	{
		tMcuList.AddMtList(byMcuMtId, tMtList);
		g_cMtAdpApp.m_tAllSMcuList.AddMcuList(byInsId, tMcuList);
	}
	
	// 2.只有最后一包才主动上报mcu，并刷新界面 [pengguofeng 7/1/2013]
	u8 byLastPack = GETBBYTE( ptRsp->m_bLastPack );
/*	
	if ( byLastPack == 1)
	{
		CServMsg cServMsg;
		cServMsg.SetEventId(MTADP_MCU_GETMTLIST_NOTIF);
		u8 bySend2MMcu = 0;
		cServMsg.SetMsgBody(&bySend2MMcu, sizeof(u8));
		cServMsg.CatMsgBody(&byMcuMtId, sizeof(u8));
		u8 byMtNum = tMtList.GetTotalMtNum();
		cServMsg.CatMsgBody(&byMtNum, sizeof(u8));
		u8 abyBuf[10*1024];
		memset(abyBuf, 0, sizeof(abyBuf));
		u16 dwMsgLen = 0;
		tMtList.MakeMtName2Buf(abyBuf, dwMsgLen);
		LogPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcPartListRsp] last pack,smcu.%d send %d Mtlen:%d to MMcu\n", byMcuMtId, byMtNum, dwMsgLen);
		cServMsg.CatMsgBody(abyBuf, dwMsgLen);
		SendMsgToMcu(cServMsg);
	}
*/
	if (byLastPack || !g_cMtAdpApp.m_byEnableBufSendSMcuMtList)
	{
		SendSMcuMtListInBuf( byLastPack ? TRUE : FALSE );
	}

	MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "Recv H_CASCADE_PARTLIST_RSP From Cascadelib. mtid = %d, PartCount = %d, byLastPack = %d\n",
		m_byMtId, ptRsp->m_dwPartCount, byLastPack);	
	
	//激发视频和音频信息定时器
	if(ISTRUE(byLastPack))
	{	KillTimer(TIMER_MTLIST);
		SetTimer(TIMER_MTLIST, g_dwPartlistInfoTimeout);
		KillTimer(TIMER_VIDEOINFO);
		KillTimer(TIMER_AUDIOINFO);
		SetTimer(TIMER_AUDIOINFO, 10);
		SetTimer(TIMER_VIDEOINFO, 10);
	}
	
	return;
}

/*=============================================================================
  函 数 名： ProcInviteMtReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcInviteMtReq( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	if ( (u32)nBufLen < sizeof(TInvitePartReq))
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcInviteMtReq] nBufLen is smaller!\n");
	}
	TInvitePartReq *ptReq = (TInvitePartReq *)pbyBuf;

    //呼叫别名类型
    u8 abyAliasType[PART_NUM] = { 0 };
    memcpy(abyAliasType, pbyBuf + sizeof(TInvitePartReq), sizeof(abyAliasType));

	TAddMtInfo atAddMtInfo[MAXNUM_ADDMT];
	SetPeerReqId( (u32)ptReq->m_nReqID );
	// 多国语言 [pengguofeng 4/8/2013]
	s8 achMtAlias[MAXLEN_ALIAS];
	BOOL32 bNeedTrans = FALSE;

	s32 nMtNum = 0;
	for(nMtNum = 0; nMtNum < ptReq->m_nCount && nMtNum < MAXNUM_ADDMT; nMtNum++ )
	{
		atAddMtInfo[nMtNum].m_byCallMode = CONF_CALLMODE_TIMER;	

        //pu类型别名
        if (abyAliasType[nMtNum] == puAliasTypeIPPlusAlias)
        {
            atAddMtInfo[nMtNum].m_AliasType = abyAliasType[nMtNum];
        }
        else
        {
            atAddMtInfo[nMtNum].m_AliasType  = CMtAdpUtils::GetAliasType( ptReq->m_atPart[nMtNum].m_aszDialStr );
        }
		//atAddMtInfo[nMtNum].m_AliasType  = CMtAdpUtils::GetAliasType( ptReq->m_atPart[nMtNum].m_aszDialStr );
		if(atAddMtInfo[nMtNum].m_AliasType == (u8)mtAliasTypeTransportAddress)
		{
			atAddMtInfo[nMtNum]. m_tTransportAddr.SetNetSeqIpAddr( inet_addr(ptReq->m_atPart[nMtNum].m_aszDialStr) );
			atAddMtInfo[nMtNum]. m_tTransportAddr.SetPort(MCU_Q931_PORT);
		}
		else
		{
			memset(achMtAlias, 0, sizeof(achMtAlias));
			bNeedTrans = TransEncoding(ptReq->m_atPart[nMtNum].m_aszDialStr, achMtAlias, sizeof(achMtAlias), FALSE);
			if ( bNeedTrans == TRUE)
			{
				strncpy( atAddMtInfo[nMtNum].m_achAlias, achMtAlias, MAXLEN_ALIAS );
			}
			else
			{
				strncpy( atAddMtInfo[nMtNum].m_achAlias, ptReq->m_atPart[nMtNum].m_aszDialStr, MAXLEN_ALIAS );
			}
			atAddMtInfo[nMtNum].m_achAlias[MAXLEN_ALIAS - 1] = 0;
		}
	}
	
	TMcuMcuReq tMcuMcuReq;
	//pengguofeng 多国语言
	s8 achUserName[CASCADE_USERNAME_MAXLEN];
	memset(achUserName, 0, sizeof(achUserName));
	bNeedTrans = TransEncoding(ptReq->m_aszUserName, achUserName, sizeof(achUserName), FALSE);
	
	if ( bNeedTrans == TRUE )
	{
		astrncpy( tMcuMcuReq.m_szUserName, achUserName,
			sizeof(tMcuMcuReq.m_szUserName), sizeof(achUserName) );
	}
	else
	{
		astrncpy( tMcuMcuReq.m_szUserName, ptReq->m_aszUserName,
			sizeof(tMcuMcuReq.m_szUserName), sizeof(ptReq->m_aszUserName) );
	}
	
	astrncpy( tMcuMcuReq.m_szUserPwd, ptReq->m_aszUserPass,
		      sizeof(tMcuMcuReq.m_szUserPwd), sizeof(ptReq->m_aszUserPass) );

	TMsgHeadMsg tHeadMsg;
	tHeadMsg.m_tMsgSrc.m_byCasLevel = ptReq->m_tMsgSrc.m_byCasLevel;
	memcpy( &tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		&ptReq->m_tMsgSrc.m_abyMtIdentify[0],
		sizeof(tHeadMsg.m_tMsgSrc.m_abyMtIdentify)
		);

	tHeadMsg.m_tMsgDst.m_byCasLevel = ptReq->m_tMsgDst.m_byCasLevel;
	memcpy( &tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
		&ptReq->m_tMsgDst.m_abyMtIdentify[0],
		sizeof(tHeadMsg.m_tMsgDst.m_abyMtIdentify)
		);

	++tHeadMsg.m_tMsgSrc.m_byCasLevel;

	TMcu tMcu = GetMtFromPartId( ptReq->m_dwPID,TRUE );

	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
		tMcu.SetMcuId( tMcu.GetMtId() );		
	}
	else
	{
		tMcu.SetMcuId( LOCAL_MCUID );
	}	
	
	cMsg.SetEventId( MCU_MCU_INVITEMT_REQ );
	cMsg.SetMsgBody( (u8 *)&tMcuMcuReq, sizeof(tMcuMcuReq) );
	cMsg.CatMsgBody( (u8 *)&tMcu, sizeof(TMcu) );
	cMsg.CatMsgBody( (u8 *)atAddMtInfo, sizeof(TAddMtInfo) * (u32)nMtNum );	
	cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );

	return;
}

/*=============================================================================
  函 数 名： ProcInviteMtRsp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcInviteMtRsp( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	if ( (u32)nBufLen < sizeof(TPartRsp))
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcInviteMtRsp] nBufLen is smaller!\n");
	}
	TPartRsp *ptRsp = (TPartRsp *)pbyBuf;
	TMsgHeadMsg tHeadMsg;
	TMt tMt = GetMtFromPartId( ptRsp->m_dwPID,FALSE );
	tHeadMsg.m_tMsgSrc.m_byCasLevel = ptRsp->m_tMsgSrc.m_byCasLevel;
	//tHeadMsg.m_tMsgSrc.m_tMt = tMt;

	tHeadMsg.m_tMsgDst.m_byCasLevel = ptRsp->m_tMsgDst.m_byCasLevel;
	
	
	if(emReturnValue_Ok == ptRsp->m_emReturnVal)
	{
		cMsg.SetEventId( MCU_MCU_INVITEMT_ACK );
	}
	else
	{
		cMsg.SetEventId( MCU_MCU_INVITEMT_NACK );
	}
	cMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
	cMsg.CatMsgBody( (u8*)&tMt,sizeof(TMt) );
	return;
}

/*=============================================================================
  函 数 名： ProcReInviteMtReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcReInviteMtReq( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	if ( (u32)nBufLen < sizeof(TPartReq))
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcReInviteMtReq]nBufLen is smaller!\n");
	}

	TPartReq *ptPartReq = (TPartReq *)pbyBuf;

	SetPeerReqId( (u32)ptPartReq->m_nReqID );
	TMt tMt = GetMtFromPartId( ptPartReq->m_dwPID, ptPartReq->m_bLocal );

	cMsg.SetEventId( MCU_MCU_REINVITEMT_REQ );
	TMcuMcuReq tReq;
	// 多国语言 [pengguofeng 4/8/2013]
	s8 achUserName[CASCADE_USERNAME_MAXLEN];
	memset(achUserName, 0, sizeof(achUserName));
	BOOL32 bNeedTrans = TransEncoding((ptPartReq->m_aszUserName), achUserName, sizeof(achUserName), FALSE);
	if ( bNeedTrans == TRUE)
	{
		astrncpy( tReq.m_szUserName, achUserName, 
			sizeof(tReq.m_szUserName), sizeof( achUserName) )
	}
	else
	{
		astrncpy( tReq.m_szUserName, ptPartReq->m_aszUserName, 
			sizeof(tReq.m_szUserName), sizeof( ptPartReq->m_aszUserName) )
	}
	astrncpy( tReq.m_szUserPwd, ptPartReq->m_aszUserPass, 
		      sizeof(tReq.m_szUserPwd), sizeof( ptPartReq->m_aszUserPass) )
	TMsgHeadMsg tHeadMsg;
	tHeadMsg.m_tMsgSrc.m_byCasLevel = ptPartReq->m_tMsgSrc.m_byCasLevel;
	memcpy( &tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		&ptPartReq->m_tMsgSrc.m_abyMtIdentify[0],
		sizeof(tHeadMsg.m_tMsgSrc.m_abyMtIdentify)
		);

	tHeadMsg.m_tMsgDst.m_byCasLevel = ptPartReq->m_tMsgDst.m_byCasLevel;
	memcpy( &tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
		&ptPartReq->m_tMsgDst.m_abyMtIdentify[0],
		sizeof(tHeadMsg.m_tMsgDst.m_abyMtIdentify)
		);

	++tHeadMsg.m_tMsgSrc.m_byCasLevel;
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		tMt.SetMcuId( tMt.GetMtId() );
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
		tMt.SetMtId( tHeadMsg.m_tMsgDst.m_abyMtIdentify[tHeadMsg.m_tMsgDst.m_byCasLevel] );		
	}	
	
	cMsg.SetMsgBody( (u8 *)&tReq, sizeof(tReq) );
	cMsg.CatMsgBody( (u8 *)&tMt, sizeof(tMt) );
	cMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(tHeadMsg) );

	return;
}

/*============================================================================
  函 数 名： ProcDropMtReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcDropMtReq( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	if ( (u32)nBufLen < sizeof(TPartReq))
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcDropMtReq] nBufLen is smaller!\n");
	}
	TPartReq *ptPartReq = (TPartReq *)pbyBuf;
	SetPeerReqId( (u32)ptPartReq->m_nReqID );
	TMt tMt = GetMtFromPartId( ptPartReq->m_dwPID, ptPartReq->m_bLocal ) ;
	
	TMcuMcuReq tReq;

	// 多国语言 [pengguofeng 4/8/2013]
	s8 achUserName[CASCADE_USERNAME_MAXLEN];
	memset(achUserName, 0, sizeof(achUserName));
	BOOL32 bNeedTrans = TransEncoding(ptPartReq->m_aszUserName, achUserName, sizeof(achUserName), FALSE);
	if ( bNeedTrans == TRUE)
	{
		astrncpy( tReq.m_szUserName, achUserName, 
			sizeof(tReq.m_szUserName), sizeof(achUserName) )
	}
	else
	{
		astrncpy( tReq.m_szUserName, ptPartReq->m_aszUserName, 
			sizeof(tReq.m_szUserName), sizeof(ptPartReq->m_aszUserName) )
	}
	astrncpy( tReq.m_szUserPwd, ptPartReq->m_aszUserPass, 
		      sizeof(tReq.m_szUserPwd), sizeof(ptPartReq->m_aszUserPass) )

	TMsgHeadMsg tHeadMsg;
	tHeadMsg.m_tMsgSrc.m_byCasLevel = ptPartReq->m_tMsgSrc.m_byCasLevel;
	memcpy( &tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		&ptPartReq->m_tMsgSrc.m_abyMtIdentify[0],
		sizeof(tHeadMsg.m_tMsgSrc.m_abyMtIdentify)
		);

	tHeadMsg.m_tMsgDst.m_byCasLevel = ptPartReq->m_tMsgDst.m_byCasLevel;
	memcpy( &tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
		&ptPartReq->m_tMsgDst.m_abyMtIdentify[0],
		sizeof(tHeadMsg.m_tMsgDst.m_abyMtIdentify)
		);

	++tHeadMsg.m_tMsgSrc.m_byCasLevel;
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		tMt.SetMcuId( tMt.GetMtId() );
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
		tMt.SetMtId( tHeadMsg.m_tMsgDst.m_abyMtIdentify[tHeadMsg.m_tMsgDst.m_byCasLevel] );		
	}

	cMsg.SetEventId(MCU_MCU_DROPMT_REQ);
	cMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
	cMsg.CatMsgBody((u8 *)&tMt, sizeof(tMt));
	cMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(tHeadMsg) );

	return;
}

/*=============================================================================
  函 数 名： ProcDelMtReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcDelMtReq( CServMsg& cMsg, u8* pbyBuf/*, s32 nBufLen*/ )
{
	TPartReq *ptPartReq = (TPartReq *)pbyBuf;

	SetPeerReqId((u32)ptPartReq->m_nReqID);
	TMt tMt = GetMtFromPartId(ptPartReq->m_dwPID, ptPartReq->m_bLocal);

	//  [pengguofeng 7/3/2013]取对应McuList，来删除MtList中的对应Mt
	u8 byInsId = (u8)GetInsID();
	TMcuList *ptMcuList = g_cMtAdpApp.m_tAllSMcuList.GetMcuList(byInsId);
	TMtList *ptMtList = NULL;
	if ( ptMcuList )
	{
		ptMtList = ptMcuList->GetMtList((u8)tMt.GetMcuId());
		if ( ptMtList )
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcDelMtReq]del name of Mt<%d.%d>\n", tMt.GetMcuId(), tMt.GetMtId());
			ptMtList->DelMtName(tMt.GetMtId());
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcDelMtReq]Mt<%d %d>not found\n", tMt.GetMcuId(), tMt.GetMtId());
		}
	}

	TMcuMcuReq tReq;
	// 多国语言 [pengguofeng 4/8/2013]
	s8 achUserName[CASCADE_USERNAME_MAXLEN];
	memset(achUserName, 0, sizeof(achUserName));
	BOOL32 bNeedTrans = TransEncoding(ptPartReq->m_aszUserName, achUserName, sizeof(achUserName), FALSE);
	if ( bNeedTrans == TRUE)
	{
		astrncpy( tReq.m_szUserName, achUserName, 
			sizeof(tReq.m_szUserName), sizeof(achUserName) )
	}
	else
	{
		astrncpy( tReq.m_szUserName, ptPartReq->m_aszUserName, 
			sizeof(tReq.m_szUserName), sizeof(ptPartReq->m_aszUserName) )
	}
	astrncpy( tReq.m_szUserPwd, ptPartReq->m_aszUserPass, 
		      sizeof(tReq.m_szUserPwd), sizeof(ptPartReq->m_aszUserPass) )
	TMsgHeadMsg tHeadMsg;
	tHeadMsg.m_tMsgSrc.m_byCasLevel = ptPartReq->m_tMsgSrc.m_byCasLevel;
	memcpy( &tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		&ptPartReq->m_tMsgSrc.m_abyMtIdentify[0],
		sizeof(tHeadMsg.m_tMsgSrc.m_abyMtIdentify)
		);

	tHeadMsg.m_tMsgDst.m_byCasLevel = ptPartReq->m_tMsgDst.m_byCasLevel;
	memcpy( &tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
		&ptPartReq->m_tMsgDst.m_abyMtIdentify[0],
		sizeof(tHeadMsg.m_tMsgDst.m_abyMtIdentify)
		);

	++tHeadMsg.m_tMsgSrc.m_byCasLevel;
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		tMt.SetMcuId( tMt.GetMtId() );
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
		tMt.SetMtId( tHeadMsg.m_tMsgDst.m_abyMtIdentify[tHeadMsg.m_tMsgDst.m_byCasLevel] );		
	}
		
	cMsg.SetMsgBody( (u8 *)&tReq, sizeof(tReq) );
	cMsg.CatMsgBody( (u8 *)&tMt, sizeof(tMt) );
	cMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(tHeadMsg) );
	cMsg.SetEventId( MCU_MCU_DELMT_REQ );

	return;
}

/*=============================================================================
  函 数 名： ProcSetPartChanReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcSetPartChanReq( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	TPartMediaChanReq *ptChan = (TPartMediaChanReq *)pbyBuf;
	SetPeerReqId( ptChan->m_nReqID );
	
	TMtMediaChanStatus tStatus;
	tStatus.m_tMt             = GetMtFromPartId( ptChan->m_dwPID, ptChan->m_bLocal );
	tStatus.m_byMediaMode     = CMtAdpUtils::MediaModeOut2In( ptChan->m_emMediaType );
	tStatus.m_byIsDirectionIn = GETBBYTE( ptChan->m_emMuteDirection == emDirectionIn );
	tStatus.m_bMute           = ptChan->m_bMute;

	TMcuMcuReq tReq;
	// 多国语言 [pengguofeng 4/8/2013]
	s8 achUserName[CASCADE_USERNAME_MAXLEN];
	memset(achUserName, 0, sizeof(achUserName));
	BOOL32 bNeedTrans = TransEncoding(ptChan->m_aszUserName, achUserName, sizeof(achUserName), FALSE);
	if ( bNeedTrans == TRUE)
	{
		astrncpy( tReq.m_szUserName, achUserName, 
			sizeof(tReq.m_szUserName), sizeof(achUserName))
	}
	else
	{
		astrncpy( tReq.m_szUserName, ptChan->m_aszUserName, 
			sizeof(tReq.m_szUserName), sizeof(ptChan->m_aszUserName))
	}
	astrncpy( tReq.m_szUserPwd, ptChan->m_aszUserPass, 
		      sizeof(tReq.m_szUserPwd), sizeof(ptChan->m_aszUserPass))
	
	cMsg.SetEventId(MCU_MCU_SETMTCHAN_REQ);
	cMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
	cMsg.CatMsgBody((u8 *)&tStatus, sizeof(tStatus));

	return;
}

/*=============================================================================
  函 数 名： ProcSetInReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcSetInReq( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	TSetInReq *ptReq = (TSetInReq *)pbyBuf;
	SetPeerReqId( ptReq->m_nReqID );

	TSetInParam tParam;
	tParam.m_tMt             = GetMtFromPartId(ptReq->m_dwPID, ptReq->m_bLocal);
	tParam.m_nViewId         = ptReq->m_nViewID;
	tParam.m_bySubFrameIndex = ptReq->m_bySubFrameIndex;

	TMcuMcuReq tReq;
	// 多国语言 [pengguofeng 4/8/2013]
	s8 achUserName[CASCADE_USERNAME_MAXLEN];
	memset(achUserName, 0, sizeof(achUserName));
	BOOL32 bNeedTrans = TransEncoding(ptReq->m_aszUserName, achUserName, sizeof(achUserName), FALSE);
	if ( bNeedTrans == TRUE)
	{
		astrncpy( tReq.m_szUserName, achUserName, 
			sizeof(tReq.m_szUserName), sizeof(achUserName));
	}
	else
	{
		astrncpy( tReq.m_szUserName, ptReq->m_aszUserName, 
			sizeof(tReq.m_szUserName), sizeof(ptReq->m_aszUserName));
	}
	astrncpy( tReq.m_szUserPwd, ptReq->m_aszUserPass, 
		      sizeof(tReq.m_szUserPwd), sizeof(ptReq->m_aszUserPass));

	TMsgHeadMsg tHeadMsg;
	tHeadMsg.m_tMsgSrc.m_byCasLevel = ptReq->m_tMsgSrc.m_byCasLevel;
	memcpy( &tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		&ptReq->m_tMsgSrc.m_abyMtIdentify[0],
		sizeof(tHeadMsg.m_tMsgSrc.m_abyMtIdentify)
		);

	tHeadMsg.m_tMsgDst.m_byCasLevel = ptReq->m_tMsgDst.m_byCasLevel;
	memcpy( &tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
		&ptReq->m_tMsgDst.m_abyMtIdentify[0],
		sizeof(tHeadMsg.m_tMsgDst.m_abyMtIdentify)
		);

	++tHeadMsg.m_tMsgSrc.m_byCasLevel;
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		tParam.m_tMt.SetMcuId( tParam.m_tMt.GetMtId() );
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
		tParam.m_tMt.SetMtId( tHeadMsg.m_tMsgDst.m_abyMtIdentify[tHeadMsg.m_tMsgDst.m_byCasLevel] );		
	}

	cMsg.SetEventId( MCU_MCU_SETIN_REQ );
	cMsg.SetMsgBody( (u8 *)&tReq, sizeof(tReq) );
	cMsg.CatMsgBody( (u8 *)&tParam, sizeof(tParam) );
	//判断是否是R6或R6Sp4版本
	LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);
	if( strcmp(lpszPeerVerId, "4.0") != 0 && 
		strcmp(lpszPeerVerId, "3.6") != 0 && 
		strcmp(lpszPeerVerId, "8") != 0 &&
		strcmp(lpszPeerVerId, "9") != 0 &&
		strcmp(lpszPeerVerId, "5") != 0)
	{
		cMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(tHeadMsg) );
	}

	return;
}

/*=============================================================================
  函 数 名： ProcAudioInfoRsp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcAudioInfoRsp( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	TConfAudioInfo *ptOInfo = (TConfAudioInfo *)pbyBuf;
	TCConfAudioInfo  tIInfo;
	TMsgHeadMsg tHeadMsg;
	
	if( emReturnValue_Ok != ptOInfo->m_emReturnVal )
	{
		cMsg.SetEventId(MCU_MCU_AUDIOINFO_NACK);
		return;
	}
	
	AudioInfoOut2In( &tIInfo, ptOInfo );
	tHeadMsg.m_tMsgSrc.m_byCasLevel = ptOInfo->m_tMsgSrc.m_byCasLevel;
	memcpy( &tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
				&ptOInfo->m_tMsgSrc.m_abyMtIdentify[0],
				sizeof(tHeadMsg.m_tMsgSrc.m_abyMtIdentify)
				);
	
	if( tHeadMsg.m_tMsgSrc.m_byCasLevel > 0 )
	{
		--tHeadMsg.m_tMsgSrc.m_byCasLevel;
		if( tIInfo.m_byMixerCount > 0 )
		{
			tIInfo.m_tMixerList[0].m_tSpeaker.SetMcuId( (u8)tIInfo.m_tMixerList[0].m_tSpeaker.GetMtId() );
			tIInfo.m_tMixerList[0].m_tSpeaker.SetMtId( tHeadMsg.m_tMsgSrc.m_abyMtIdentify[tHeadMsg.m_tMsgSrc.m_byCasLevel] );
		}
	}
	cMsg.SetEventId( MCU_MCU_AUDIOINFO_ACK );
	cMsg.SetMsgBody( (u8 *)&(tIInfo), sizeof(tIInfo) );
	cMsg.CatMsgBody( (u8 *)&(tHeadMsg), sizeof(tHeadMsg) );

	return;
}

/*=============================================================================
  函 数 名： ProcVideoInfoRsp
  功    能： 
  算法实现： 
  全局变量： 
  参    数：  CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcVideoInfoRsp( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	TConfVideoInfo *ptOInfo = (TConfVideoInfo *)pbyBuf;
	TCConfViewInfo  tIInfo;
	
	if( emReturnValue_Ok != ptOInfo->m_emReturnVal )
	{
		cMsg.SetEventId( MCU_MCU_VIDEOINFO_NACK );
		return;
	}
	
	VideoInfoOut2In(&tIInfo, ptOInfo);
	
	cMsg.SetEventId( MCU_MCU_VIDEOINFO_ACK );
	cMsg.SetMsgBody( (u8 *)&(tIInfo), sizeof(tIInfo) );

	return;
}

/*=============================================================================
  函 数 名： ProcNewMtNotify
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容 
=============================================================================*/
void CMtAdpInst::ProcNewMtNotify( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	//新列表通知
	TPart *ptPartU =  (TPart *)pbyBuf;

	TMcuMcuMtInfo tInfo;
	AddPartIdTable( ptPartU->m_dwPID );

	// 保存到TMcuList [pengguofeng 6/26/2013]
	// 1.确定SMcu的MtId，本身则为0
	u8 byMcuMtId = 0xFF;
	u8 byAddMtId = 0;
	TMt tMt;
	tMt.SetNull();
	tMt = GetMtFromPartId(ptPartU->m_dwPID, ptPartU->m_bLocal);

	{
		if ( ptPartU->m_byCasLevel == 0 )
		{
			byMcuMtId = 0;
			byAddMtId = tMt.GetMtId();
		}
		else
		{
			byMcuMtId = tMt.GetMtId();/*(u8)tMt.GetMcuId()*/;
			byAddMtId = ptPartU->m_abyMtIdentify[0];
		}
	}

	if ( byMcuMtId == 0xFF )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcNewMtNotify]byMcuMtId == 0xFF\n");
		return;
	}

	// 2.取已经保存的McuLisst，如果没有，则新建一个
	u8 byInsId = (u8)GetInsID();

	LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcNewMtNotify]Add to byMcuMtId:%d\n", byMcuMtId);
	TMcuList *ptMcuList = g_cMtAdpApp.m_tAllSMcuList.GetMcuList( byInsId );
	TMtList tMtList;
	if ( ptMcuList != NULL )
	{
		TMtList *ptMtList = ptMcuList->GetMtList( byMcuMtId);
		if ( !ptMtList )
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcPartListRsp]create new MtList\n");
			tMtList.Init();
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL,MID_MCULIB_MTADP, "[ProcPartListRsp]get existing MtList of Mcu.%d\n", byMcuMtId);
			memcpy(&tMtList, ptMtList, sizeof(TMtList));
		}
	}
	else
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcPartListRsp]create new McuList\n");
		tMtList.Init();
	}

	{
		LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcNewMtNotify]add name(%s ) to ",
			ptPartU->m_aszPartName);

		Part2MtInfo( ptPartU, &(tInfo) );
		if ( ptPartU->m_byCasLevel == 0 )
		{
			tMtList.AddMtName(tInfo.m_tMt.GetMtId(), tInfo.m_szMtName);
			LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "to mt<%d.%d>\n",
				tInfo.m_tMt.GetMcuId(), tInfo.m_tMt.GetMtId() );
		}
		else
		{
			if ( ptPartU->m_abyMtIdentify[0] != 0 )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "to identify mt<%d>\n",
					ptPartU->m_abyMtIdentify[0] );
				tMtList.AddMtName(ptPartU->m_abyMtIdentify[0], tInfo.m_szMtName/*ptPartU->m_aszPartName*/);
			}
			else
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcNewMtNotify]part--m_byCasLevel:%d m_abyMtIdentify[0]:%d m_aszPartName:(%s )\n",
					ptPartU->m_byCasLevel, ptPartU->m_abyMtIdentify[0], ptPartU->m_aszPartName);
			}
		}
	}

	if ( ptMcuList)
	{
		ptMcuList->AddMtList(byMcuMtId, tMtList);
	}
	else
	{
		TMcuList tMcuList;
		tMcuList.Init();
		tMcuList.AddMtList(byMcuMtId, tMtList);
		g_cMtAdpApp.m_tAllSMcuList.AddMcuList( byInsId , tMcuList);
	}

	cMsg.SetEventId( MCU_MCU_NEWMT_NOTIF );
	cMsg.SetMsgBody( (u8 *)&tInfo, sizeof(tInfo) );

	return;
}

/*=============================================================================
  函 数 名： ProcCallAlertMtNotify
  功    能： 
  算法实现： 
  全局变量： 
  参    数：  CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcCallAlertMtNotify( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	//呼叫通知
// 	TCallNtf* ptCallAlertNtf = NULL/*(TCallNtf *)pbyBuf*/; 
	TCallNtf* ptCallAlertNtfU = NULL;
	ptCallAlertNtfU = (TCallNtf *)pbyBuf;

	TMt      tCallMt;
	tCallMt.SetNull();
	TMtAlias tCallMtAlias;
	s8 achDialStr[STR_LEN/*_UTF8*/] = {0};
	u32 dwCopyLen = 0;


	AddPartIdTable( ptCallAlertNtfU->m_dwPID );
	tCallMt = GetMtFromPartId( ptCallAlertNtfU->m_dwPID, ptCallAlertNtfU->m_bLocal/*FALSE*/ );
	tCallMtAlias.m_AliasType = CMtAdpUtils::GetAliasType( ptCallAlertNtfU->m_aszDialStr );
	memcpy(achDialStr, ptCallAlertNtfU->m_aszDialStr, sizeof(achDialStr)); //这两个是一样长
	achDialStr[STR_LEN/*_UTF8*/-1] = 0;
	 
	if( (u8)mtAliasTypeOthers == tCallMtAlias.m_AliasType )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcCallAlertMtNotify]tCallMtAlias.m_AliasType InValid\n");
		return;
	}
	else if( (u8)mtAliasTypeTransportAddress == tCallMtAlias.m_AliasType )
	{
		tCallMtAlias.m_tTransportAddr.SetNetSeqIpAddr( inet_addr( /*ptCallAlertNtf->m_aszDialStr*/achDialStr ) );
		tCallMtAlias.m_tTransportAddr.SetPort( MCU_Q931_PORT );
	}
	else
	{
		s8 achMtAlias[MAXLEN_ALIAS]; // pengguofeng 多国语言
		memset(achMtAlias, 0, sizeof(achMtAlias));
		BOOL32 bNeedTrans = TransEncoding(/*ptCallAlertNtf->m_aszDialStr*/achDialStr, achMtAlias, sizeof(achMtAlias), FALSE);
		if ( bNeedTrans == TRUE )
		{
			strncpy( tCallMtAlias.m_achAlias, achMtAlias, MAXLEN_ALIAS );
		}
		else
		{
			strncpy( tCallMtAlias.m_achAlias, achDialStr/*ptCallAlertNtf->m_aszDialStr*/, MAXLEN_ALIAS );
		}
		tCallMtAlias.m_achAlias[MAXLEN_ALIAS - 1] = 0;
	}
	TMsgHeadMsg tHeadMsg;
	u8 byMcuId = 0xFF;
	u8 byAddMtId = 0;

	{
		tHeadMsg.m_tMsgSrc.m_byCasLevel = ptCallAlertNtfU->m_tMsgSrc.m_byCasLevel;
		memcpy( &tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
			&ptCallAlertNtfU->m_tMsgSrc.m_abyMtIdentify[0],
			sizeof( tHeadMsg.m_tMsgSrc.m_abyMtIdentify )
			);
		
		tHeadMsg.m_tMsgDst.m_byCasLevel = ptCallAlertNtfU->m_tMsgDst.m_byCasLevel;
		memcpy( &tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
			&ptCallAlertNtfU->m_tMsgDst.m_abyMtIdentify[0],
			sizeof( tHeadMsg.m_tMsgDst.m_abyMtIdentify )
			);
		// 由MsgSrc的参数来判断 [pengguofeng 7/4/2013]

		if ( tHeadMsg.m_tMsgSrc.m_byCasLevel == 0 )
		{
			byMcuId = 0;
			byAddMtId = tCallMt.GetMtId();
		}
		else
		{
			byMcuId = tCallMt.GetMtId()/*(u8)tCallMt.GetMcuId()*/;
			byAddMtId = ptCallAlertNtfU->m_tMsgSrc.m_abyMtIdentify[0];
		}
	}

	// 添加到MtList中去 [pengguofeng 7/3/2013]
	LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcCallAlertMtNotify]add name(%s ) to Mcu.%d Mt<%d %d> byAddMtId.%d\n",
		tCallMtAlias.m_achAlias, byMcuId, tCallMt.GetMcuId(), tCallMt.GetMtId(), byAddMtId);
	u8 byInsId = (u8)GetInsID();
	TMcuList *ptMcuList = g_cMtAdpApp.m_tAllSMcuList.GetMcuList(byInsId);
	if ( ptMcuList )
	{
		// 因为终端未上线，所以都认为是本级的即可
		TMtList *ptMtList = ptMcuList->GetMtList( byMcuId/*(u8)tCallMt.GetMcuId()*/);
		if ( ptMtList )
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcCallAlertMtNotify]add Name to Mt<%d.%d>\n", tCallMt.GetMcuId(), tCallMt.GetMtId());
			ptMtList->AddMtName(byAddMtId/*tCallMt.GetMtId()*/, tCallMtAlias.m_achAlias);
		}
	}
	/*tHeadMsg.m_tMsgSrc.m_abyMtIdentify[tHeadMsg.m_tMsgSrc.m_byCasLevel] = tCallMt.GetMtId();
	++tHeadMsg.m_tMsgSrc.m_byCasLevel;
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
	}*/

	MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcCallAlertMtNotify] Recf MCU_MCU_CALLALERTING_NOTIF ptCallAlertNtf->m_dwPID.0x%8x tCallMt(%d.%d)\n",
		/*ptCallAlertNtf?ptCallAlertNtf->m_dwPID:0,*/ ptCallAlertNtfU?ptCallAlertNtfU->m_dwPID:0, tCallMt.GetMcuId(),tCallMt.GetMtId()
		);

	cMsg.SetEventId( MCU_MCU_CALLALERTING_NOTIF );
	cMsg.SetMsgBody( (u8 *)&tCallMt, sizeof(tCallMt) );
	cMsg.CatMsgBody( (u8 *)&tCallMtAlias, sizeof(tCallMtAlias) );
	cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );

	return;
}

/*=============================================================================
  函 数 名： ProcSetOutReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcSetOutReq( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	TSetOutReq *ptReq = (TSetOutReq *)pbyBuf;
	SetPeerReqId( ptReq->m_nReqID );

	TSetOutParam tParam;
	tParam.m_nMtCount = min(ptReq->m_dwPartCount, MAXNUM_CONF_MT);

	for( s32 nLoop = 0; nLoop < tParam.m_nMtCount; nLoop ++ )
	{
		tParam.m_atConfViewOutInfo[nLoop].m_tMt = GetMtFromPartId(ptReq->m_tPartOutInfo[nLoop].m_dwPID, ptReq->m_tPartOutInfo[nLoop].m_bLocal);
	    tParam.m_atConfViewOutInfo[nLoop].m_nOutViewID = ptReq->m_tPartOutInfo[nLoop].m_nOutViewID;
		tParam.m_atConfViewOutInfo[nLoop].m_nOutVideoSchemeID = ptReq->m_tPartOutInfo[nLoop].m_nOutVideoSchemeID;
	}

	TMcuMcuReq tReq;
	// 多国语言 [pengguofeng 4/8/2013]
	s8 achUserName[CASCADE_USERNAME_MAXLEN];
	memset(achUserName, 0, sizeof(achUserName));
	BOOL32 bNeedTrans = TransEncoding(ptReq->m_aszUserName, achUserName, sizeof(achUserName), FALSE);
	if ( bNeedTrans == TRUE)
	{
		astrncpy( tReq.m_szUserName, achUserName, 
			sizeof(tReq.m_szUserName), sizeof(achUserName));
	}
	else
	{
		astrncpy( tReq.m_szUserName, ptReq->m_aszUserName, 
			sizeof(tReq.m_szUserName), sizeof(ptReq->m_aszUserName));
	}
	astrncpy( tReq.m_szUserPwd, ptReq->m_aszUserPass, 
		      sizeof(tReq.m_szUserPwd), sizeof(ptReq->m_aszUserPass));
	
	cMsg.SetMsgBody( (u8 *)&tReq, sizeof(tReq) );
	cMsg.CatMsgBody( (u8 *)&tParam, sizeof(tParam) );
	cMsg.SetEventId( MCU_MCU_SETOUT_REQ );

	return;
}

/*=============================================================================
  函 数 名： ProcSetOutNtf
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcSetOutNtf( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	TPartOutputNtf *ptNtf = (TPartOutputNtf *)pbyBuf;
	TCConfViewOutInfo tParam;
	TMsgHeadMsg tHeadMsg;
	
	tParam.m_tMt = GetMtFromPartId( ptNtf->m_dwPID, ptNtf->m_bLocal );

	//zjj 20101115 判断对方是否是R6版本,如果是判断是否mtid为0就屏蔽一下，因为如果是0在PDS流终端就代表是下级取消了上传终端
	//但是由于R6有Bug，每次收到mcu_mcu_setin_req时都会把上级mcusetout给上级，所以会造成PDS做上级会取消发言人
	LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);	

 	if( strcmp(lpszPeerVerId, "9") == 0 ||
 		strcmp(lpszPeerVerId, "8") == 0 || 
 		strcmp(lpszPeerVerId, "3.6") == 0 || 
 		strcmp(lpszPeerVerId, "4.0") == 0 ||
 		strcmp(lpszPeerVerId, "5") == 0
 		)
	{		
 		if( 0 == tParam.m_tMt.GetMtId() )
 		{
 			LogPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcSetOutNtf] mcu %s version,MtId is 0,so not notify to mcu.\n", lpszPeerVerId);
			return;
 		}	
	}

	//20111019 zjl 下级做取消上传源操作 byCancelSpy = 1
	if( tParam.m_tMt.IsMcuIdLocal() && m_byMtId == tParam.m_tMt.GetMtId() )
	{
		tParam.m_tMt.SetMcuId( m_byMtId );
		tParam.m_tMt.SetMtId( 0 );
	}

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcSetOutNtf]  mcu's version .%s, SetOutNTF Mt<McuId.%d, MtId.%d>\n",
													lpszPeerVerId, tParam.m_tMt.GetMcuId(), tParam.m_tMt.GetMtId());

	tParam.m_nOutViewID = ptNtf->m_nViewID;
	tParam.m_nOutVideoSchemeID = 0;

	tHeadMsg.m_tMsgSrc.m_byCasLevel = ptNtf->m_tMsgSrc.m_byCasLevel;
	memcpy( &tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
			&ptNtf->m_tMsgSrc.m_abyMtIdentify[0],
			sizeof( tHeadMsg.m_tMsgSrc.m_abyMtIdentify )
			);

	tHeadMsg.m_tMsgDst.m_byCasLevel = ptNtf->m_tMsgDst.m_byCasLevel;
	memcpy( &tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
			&ptNtf->m_tMsgDst.m_abyMtIdentify[0],
			sizeof( tHeadMsg.m_tMsgDst.m_abyMtIdentify )
			);

	cMsg.SetEventId( MCU_MCU_SETOUT_NOTIF );
	cMsg.SetMsgBody( (u8 *)&tParam, sizeof(tParam) );
	if( strcmp(lpszPeerVerId, "4.0") != 0 && 
		strcmp(lpszPeerVerId, "3.6") != 0 && 
		strcmp(lpszPeerVerId, "8") != 0 &&
		strcmp(lpszPeerVerId, "9") != 0 &&
		strcmp(lpszPeerVerId, "5") != 0 )
	{
		cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
	}

	return;
}

/*=============================================================================
  函 数 名： ProcNonStandardMsg
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg
             u8* pbyBuf
             s32 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcNonStandardMsg( CServMsg& cMsg, u8* pbyBuf, s32 nBufLen )
{
	CServMsg cServMsg(pbyBuf, (u16)(nBufLen));
    switch( cServMsg.GetEventId() )
	{
    case MCU_MCU_STARTMIXER_CMD:
		{
			ProcMcuMcuStartMixerCmd( cServMsg, cMsg );
			break;
		}
/*
	case MCU_MCU_STARTMIXER_REQ:
		{
			ProcMcuMcuStartMixerReq(cServMsg, cMsg);
			break;
		}
	case MCU_MCU_STARTMIXER_ACK:
		{
			ProcMcuMcuStartMixerAck(cServMsg, cMsg);
			break;
		}
	case MCU_MCU_STARTMIXER_NACK:
		{	
			ProcMcuMcuStartMixerNack(cServMsg, cMsg);
			break;
		}
*/
	case MCU_MCU_STARTMIXER_NOTIF:
		{
			ProcMcuMcuStartMixerNotif( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_STOPMIXER_CMD:
		{
			ProcMcuMcuStopMixerCmd( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_STOPMIXER_NOTIF:
		{
			ProcMcuMcuStopMixerNotif( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_GETMIXERPARAM_REQ:
		{
			ProcMcuMcuGetMixerParamReq( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_GETMIXERPARAM_ACK:
		{
			ProcMcuMcuGetMixerParamAck( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_GETMIXERPARAM_NACK:
		{
			ProcMcuMcuGetMixerParamNack( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_MIXERPARAM_NOTIF:
		{
			ProcMcuMcuGetMixerParamNotif( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_ADDMIXMEMBER_CMD:
		{
			ProcMcuMcuAddMixerMemeberCmd( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_REMOVEMIXMEMBER_CMD:
		{
			ProcMcuMcuRemoveMixerMemeberCmd( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_LOCK_REQ:
		{
			ProcMcuMcuLockReq( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_LOCK_ACK:
		{
			ProcMcuMcuLockAck( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_LOCK_NACK:
		{
			ProcMcuMcuLockNack( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_MTSTATUS_CMD:
		{
			ProcMcuMcuMtStatusCmd( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_MTSTATUS_NOTIF:
		{
			ProcMcuMcuMtStatusNtf( cServMsg, cMsg );
			break;
		}

	case MCU_MCU_SENDMSG_NOTIF:
		{
			ProcMcuMcuMsgNtf( cServMsg, cMsg );
			break;
		}

	case MCU_MCU_MTAUDMUTE_REQ:
		{
			ProcMcuMcuMuteDumbReq( cServMsg, cMsg );
			break;
		}
	case VCS_MCU_MUTE_REQ:
		{
			// no need to modify message body, just go ahead.
			cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			break;
		}
	case MCU_MCU_ADJMTRES_REQ:
		{
			ProcMcuMcuAdjMtResReq( cServMsg, cMsg );
			break;
		}
	case  MCU_MCU_ADJMTBITRATE_CMD:
		{
			ProcMcuMcuAdjMtBitrateCmd( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_ADJMTRES_ACK:
	case MCU_MCU_ADJMTRES_NACK:
		{
			ProcMcuMcuAdjMtResRsp( cServMsg, cMsg );
			break;
		}

	//lukunpeng 2010/07/23 终端申请发言/抢答代码合并
	case MCU_MCU_SPEAKSTATUS_NTF:
		{
			ProcMcuMcuSpeakStatusNtf( cServMsg, cMsg  );
			break;
		}

    case MCU_MCU_APPLYSPEAKER_REQ:
        {
            ProcMcuMcuApplySpeakerReq( cServMsg, cMsg );
            break;
        }
		
    case MCU_MCU_APPLYSPEAKER_ACK:
    case MCU_MCU_APPLYSPEAKER_NACK:
        {
            ProcMcuMcuApplySpeakerRsp( cServMsg, cMsg );
            break;
        }
		
    /*case MCU_MCU_APPLYSPEAKER_NOTIF:
        {
            ProcMcuMcuApplySpeakerNtf( cServMsg, cMsg );
            break;
        }*/
        
    case MCU_MCU_CANCELMESPEAKER_REQ:
        {
            ProcMcuMcuCancelMeSpeakerReq( cServMsg, cMsg );
            break;
        }
    case MCU_MCU_CANCELMESPEAKER_ACK:
    case MCU_MCU_CANCELMESPEAKER_NACK:
        {
            ProcMcuMcuCancelMeSpeakerRsp( cServMsg, cMsg );
            break;
        }
		
	case MCU_MCU_SPEAKERMODE_NOTIFY:
		{
			ProcMcuMcuSpeakModeNotify( cServMsg, cMsg );
			break;
		}


	//zjj20100201 多回传代码合并
	//  pengjie[9/29/2009] 从协议栈上来的级联多回传消息处理
	case MCU_MCU_MULTSPYCAP_NOTIF:
		{
			ProcMcuMcuMultSpyCapNotif( cServMsg, cMsg );
			break;
		}

	case MCU_MCU_PRESETIN_REQ:
		{
			ProcMcuMcuPreSetinReq( cServMsg, cMsg );
			break;
		}

	case MCU_MCU_SPYFASTUPDATEPIC_CMD:
		{
			ProcMcuMcuSpyFastUpdateCmd( cServMsg, cMsg );
			break;
		}

	case MCU_MCU_PRESETIN_ACK:
		{
            ProcMcuMcuPreSetinAck( cServMsg, cMsg );
			break;
		}

	case MCU_MCU_PRESETIN_NACK:
		{
            ProcMcuMcuPreSetinNack( cServMsg, cMsg );
			break;
		}

	case MCU_MCU_SPYCHNNL_NOTIF:
		{
			ProcMcuMcuSpyChnnlNotif( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_SWITCHTOMCUFAIL_NOTIF:
		{
			ProcMcuMcuSwitchToMMcuFailNotif( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_MTEXTINFO_NOTIF:
		{
			ProcMcuMcuMtExtInfoNotify( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_REJECTSPY_NOTIF:
		{
			ProcMcuMcuRejectSpyNotif( cServMsg, cMsg );
			break;
		}
		
	case MCU_MCU_BANISHSPY_CMD:
		{
			ProcMcuMcuBanishSpyCmd( cServMsg, cMsg );
			break;
		}

	//lukunpeng 2010/06/10 由于现在是由上级管理下级带宽占用情况，故不需要再通过下级通知上级
		/*
	case MCU_MCU_BANISHSPY_NOTIF:
		{
			ProcMcuMcuBanishSpyNotif( cServMsg, cMsg );
			break;
		}
		*/

// [pengjie 2010/4/23] 级联将帧率支持
	case MCU_MCU_ADJMTFPS_REQ:
		{
			ProcMcuMcuAdjMtFpsReq( cServMsg, cMsg );
			break;
		}
	case MCU_MCU_ADJMTFPS_ACK:
	case MCU_MCU_ADJMTFPS_NACK:
		{
			ProcMcuMcuAdjMtFpsRsp( cServMsg, cMsg );
			break;
		}
// End

// [pengjie 2010/8/12] 级联远摇
	case MCU_MCU_FECC_CMD:
		{
			ProcMMcuFeccCmd( cServMsg, cMsg );
			break;
		}
//End

    //[5/4/2011 zhushengze]VCS控制发言人发双流
    case MCU_MCU_CHANGEMTSECVIDSEND_CMD:
        {
            ProcMMcuChangeMtSecVidSendCmd( cServMsg, cMsg );
            break;
        }
    case MCU_MCU_TRANSPARENTMSG_NOTIFY:
        {
            ProcMMcuTransparentMsgNtf( cServMsg, cMsg );
            break;
        }
	default:
        UNEXPECTED_MESSAGE( cServMsg.GetEventId() );
        break;
	}
}

/*=============================================================================
    函 数 名： ProcMcuMcuStartMixerCmd
    功    能： 处理 上级MCU开启下级MCU会议讨论命令
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               CServMsg& cMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/5/27   3.6			万春雷                  创建
=============================================================================*/
void CMtAdpInst::ProcMcuMcuStartMixerCmd(const CServMsg& cServMsg, CServMsg& cMsg)
{
/*	cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen());
	TMcu tMcu;
	tMcu.SetMcuId(m_byMtId);
	cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
	cMsg.CatMsgBody( cServMsg.GetMsgBody(), sizeof(u8) );
	s32 nMtNum = ( cServMsg.GetMsgBodyLen() - sizeof(u8)) / sizeof(u32);
	u32 *pdwPartId = (u32 *)( cServMsg.GetMsgBody() + sizeof(u8) );
	TMt tMt;
	for( s32 nIndex=0; nIndex<nMtNum; nIndex ++ )
	{
		tMt = GetMtFromPartId(ntohl(*pdwPartId), TRUE);
		cMsg.CatMsgBody((u8 *)&tMt, sizeof(TMt));
		pdwPartId ++ ;
	}

	return;*/
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen());

	TMcu tMcu;
	tMcu.SetMcuId(LOCAL_MCUID);
	s32  nMtNum = 0;
	u32  *pdwPartId = NULL;
	//u8   byReplace   = 0;
	TMtadpHeadMsg tMtadpHeadMsg;
    LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);

	/*if (strcmp(lpszPeerVerId, "10") == 0)
	{
		nMtNum    = (cServMsg.GetMsgBodyLen() - sizeof(u8) - sizeof(TMtadpHeadMsg)) / sizeof(u32);
		pdwPartId = (u32 *)cServMsg.GetMsgBody();
		tMtadpHeadMsg = *(TMtadpHeadMsg*)(cServMsg.GetMsgBody()+ sizeof(u32) * nMtNum + sizeof(u8));
	} 
	else
	{
		nMtNum = ( cServMsg.GetMsgBodyLen() ) / sizeof(u32);
		pdwPartId = (u32 *)( cServMsg.GetMsgBody() );
	}*/

	if( strcmp(lpszPeerVerId, "3.6") == 0 || strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "8") == 0 || strcmp(lpszPeerVerId, "9") == 0 ||
		strcmp(lpszPeerVerId, "5") == 0
		)
	{
		nMtNum = ( cServMsg.GetMsgBodyLen() ) / sizeof(u32);
		pdwPartId = (u32 *)( cServMsg.GetMsgBody() + sizeof(u8) );
	}
	else
	{
		if( cServMsg.GetMsgBodyLen() < sizeof(u8) + sizeof(TMtadpHeadMsg) )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcMcuMcuStartMixerCmd]cServMsg len is smaller than expected, return\n");
			return;
		}
		nMtNum    = (cServMsg.GetMsgBodyLen() - sizeof(u8) - sizeof(TMtadpHeadMsg)) / sizeof(u32);
		pdwPartId = (u32 *)(cServMsg.GetMsgBody() + sizeof(u8));
		tMtadpHeadMsg = *(TMtadpHeadMsg*)(cServMsg.GetMsgBody()+ sizeof(u32) * (u32)nMtNum + sizeof(u8));
	}

    cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
	u8 byDissMtNum = *(u8*)(cServMsg.GetMsgBody());
	cMsg.CatMsgBody( (u8 *)&byDissMtNum, sizeof(u8) );
	if ( tMtadpHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		TMt tTempMt;
		u32 dwTempPartid = 0;
		BOOL32 bIsLocal = TRUE;
		tMtadpHeadMsg.m_tMsgDst.GetPartGid(&dwTempPartid,&bIsLocal);
		tTempMt = GetMtFromPartId(ntohl(dwTempPartid), TRUE);
		cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
		for( s32 nIndex = 0; nIndex < nMtNum; nIndex ++ )
		{
			TMt tMt;
			tMt.SetMcuId(tTempMt.GetMtId());
			if (pdwPartId == NULL)
			{
				break;
			}
			tMt.SetMtId((u8)(ntohl(*pdwPartId)));
			cMsg.CatMsgBody( (u8 *)&tMt, sizeof(tMt) );
			pdwPartId ++ ;
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuStartMixerCmd]mcuid(%d) mtid(%d)\n",
			tMcu.GetMcuId(),tMcu.GetMtId(),tMt.GetMcuId(),tMt.GetMtId());
		}
	} 
	else
	{
		for(s32 nIndex = 0; nIndex < nMtNum; nIndex ++ )
		{
			TMt tMt;
			if (pdwPartId == NULL)
			{
				break;
			}
			tMt = GetMtFromPartId(ntohl(*pdwPartId), TRUE);
			
			tMt.SetMcuIdx(LOCAL_MCUIDX);
			cMsg.CatMsgBody( (u8 *)&tMt, sizeof(tMt) );
			pdwPartId ++ ;
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuStartMixerCmd]mcuid(%d) mtid(%d)\n",tMt.GetMcuId(),tMt.GetMtId());
		}
	}
	return;
}

/*=============================================================================
  函 数 名： ProcMcuMcuStartMixerNotif
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcMcuMcuStartMixerNotif(const CServMsg& cServMsg, CServMsg& cMsg)
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TMcu tMcu;
	tMcu.SetMcuId( m_byMtId );
	cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
	cMsg.CatMsgBody( cServMsg.GetMsgBody(), sizeof(u8) );

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuStopMixerCmd
    功    能： 处理 上级MCU停止下级MCU会议讨论命令
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               CServMsg& cMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/5/27   3.6			万春雷                  创建
=============================================================================*/
void CMtAdpInst::ProcMcuMcuStopMixerCmd( const CServMsg& cServMsg, CServMsg& cMsg )
{
	cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen());
	TMcu tMcu;
	tMcu.SetMcuId(m_byMtId);
	cMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));

	return;
}

/*=============================================================================
  函 数 名： ProcMcuMcuStopMixerNotif
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容  
=============================================================================*/
void CMtAdpInst::ProcMcuMcuStopMixerNotif(const CServMsg& cServMsg, CServMsg& cMsg)
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TMcu tMcu;
	tMcu.SetMcuId( m_byMtId );
	cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
    LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);
	u8 byNeedClearMtMixing =  0;
	if (0 == strcmp(lpszPeerVerId,"5"))
	{
		byNeedClearMtMixing = 1;
	}
// 	if ( lpszPeerVerId == "5")
// 	{
// 		byNeedClearMtMixing = 1;
// 	}
	cMsg.CatMsgBody( (u8 *)&byNeedClearMtMixing, sizeof(u8) );
	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuGetMixerParamReq
    功    能： 处理 上级MCU获取下级MCU会议讨论参数请求
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               CServMsg& cMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/5/27   3.6			万春雷                  创建
=============================================================================*/
void CMtAdpInst::ProcMcuMcuGetMixerParamReq(const CServMsg& cServMsg, CServMsg& cMsg)
{
	cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen());
	TMcu tMcu;
	tMcu.SetMcuId(m_byMtId);
	cMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));

	return;
}

/*=============================================================================
  函 数 名： ProcMcuMcuGetMixerParamAck
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006.07.28    4.0         许世林                增加保护
=============================================================================*/
void CMtAdpInst::ProcMcuMcuGetMixerParamAck( const CServMsg& cServMsg, CServMsg& cMsg )
{
	cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen());

	TMcu tMcu;
	tMcu.SetMcuId(m_byMtId);
	cMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));

    // FIXME: 此处要适配到 TDiscunssParamV4R4B2，稍后
	TMixParam tParam;
	tParam.SetMode(*((u8 *)cServMsg.GetMsgBody()));

	//s32  nMtNum    = (cServMsg.GetMsgBodyLen()-sizeof(u8))/sizeof(u32);
	//u32 *pdwPartId = (u32 *)(cServMsg.GetMsgBody()+sizeof(u8));
    //s32  nIndex = 0;
    /*
	for( nIndex = 0; nIndex < nMtNum; )
	{
		//过滤掉上级mcu本身在下级mcu的成员表现
		if( REMOTE_MCU_PARTID != *pdwPartId && nIndex < MAXNUM_MIXER_DEPTH )
		{
			tParam.m_atMtMember[nIndex] = GetMtFromPartId( ntohl(*pdwPartId), FALSE );
			nIndex ++ ;
		}
		pdwPartId ++ ;
	}
	tParam.m_byMemberNum = nIndex;
    */
	cMsg.CatMsgBody((u8 *)&tParam, sizeof(tParam));

	return;
}

/*=============================================================================
  函 数 名： ProcMcuMcuGetMixerParamNack
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcMcuMcuGetMixerParamNack( const CServMsg& cServMsg, CServMsg& cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TMcu tMcu;
	tMcu.SetMcuId( m_byMtId );
	cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );	

	return;
}

/*=============================================================================
  函 数 名： ProcMcuMcuGetMixerParamNotif
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006.07.28    4.0         许世林                增加保护
=============================================================================*/
void CMtAdpInst::ProcMcuMcuGetMixerParamNotif( const CServMsg& cServMsg, CServMsg& cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );

	TMcu tMcu;
	tMcu.SetMcuId( m_byMtId );
	cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );

	//由于R6中已经废除MixerParam通知，改由TMtStatus上报混音状态
	//故在R6收到此消息后，直接过滤,
	//当EventID == 0时，不发送。
	cMsg.SetEventId(0);
	
	//故在R6收到R2的此消息后，转化为以TMtStatus状态上报
	
	//匹配R2版本

    // FIXME: 此处要适配到 TDiscunssParamV4R4B2，稍后
	TMixParam tParam;
	tParam.SetMode(*((u8 *)cServMsg.GetMsgBody()));
	
    /*
	s32 nMtNum = ( cServMsg.GetMsgBodyLen() - sizeof(u8) ) / sizeof(u32);
	u32 *pdwPartId = (u32 *)( cServMsg.GetMsgBody() + sizeof(u8) );
    s32 nIndex = 0;
	for( nIndex = 0; nIndex < nMtNum; )
	{
		//过滤掉上级mcu本身在下级mcu的成员表现
		if( REMOTE_MCU_PARTID != *pdwPartId  && nIndex < MAXNUM_MIXER_DEPTH )
		{
			tParam.m_atMtMember[nIndex] = GetMtFromPartId(ntohl(*pdwPartId), FALSE);
			nIndex ++ ;
		}
		pdwPartId ++ ;
	}
	tParam.m_byMemberNum = nIndex;
	cMsg.CatMsgBody((u8 *)&tParam, sizeof(tParam));
    */


	return;
}

/*=============================================================================
  函 数 名： ProcMcuMcuAddMixerMemeberCmd
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcMcuMcuAddMixerMemeberCmd( const CServMsg& cServMsg, CServMsg& cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen());

	LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);
	TMt tTempMt;
	if (atoi(lpszPeerVerId) >= 12)
	{
		u8 byMtNum    = (cServMsg.GetMsgBodyLen() - sizeof(u8) ) / sizeof(TMtadpMultiCascadeMsg);
		u8 byReplace  = *(u8*)(cServMsg.GetMsgBody() + cServMsg.GetMsgBodyLen() - sizeof(u8));
		TMtadpMultiCascadeMsg *ptMtadpMultiCascadeMsg = NULL;
		if (byMtNum == 0)
		{
			return;
		}
		ptMtadpMultiCascadeMsg = (TMtadpMultiCascadeMsg*)(cServMsg.GetMsgBody());
		for(s32 nIndex = 0; nIndex < byMtNum; nIndex ++ ,ptMtadpMultiCascadeMsg++)
		{
			if (ptMtadpMultiCascadeMsg == NULL)
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcMcuMcuAddMixerMemeberCmd]ptMtadpMultiCascadeMsg == NULL nIndex(%d),so return.\n",nIndex);
				return;
			}
			if ( ptMtadpMultiCascadeMsg->m_byCasLevel > MAX_CASCADELEVEL )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcMcuMcuAddMixerMemeberCmd]ptMtadpMultiCascadeMsg->m_byCasLevel(%d) > MAX_CASCADELEVEL(%d).\n",
					ptMtadpMultiCascadeMsg->m_byCasLevel , MAX_CASCADELEVEL);
				return;
			}
			tTempMt.SetNull();
			if (ptMtadpMultiCascadeMsg->m_byCasLevel > 0)
			{
				tTempMt = GetMtFromPartId(ntohl(ptMtadpMultiCascadeMsg->GetPid()), TRUE);
				--ptMtadpMultiCascadeMsg->m_byCasLevel;
				tTempMt.SetMcuId(tTempMt.GetMtId());
				tTempMt.SetMtId(ptMtadpMultiCascadeMsg->m_abyMtIdentify[ptMtadpMultiCascadeMsg->m_byCasLevel]);
			}
			else
			{
				tTempMt = GetMtFromPartId(ntohl(ptMtadpMultiCascadeMsg->GetPid()), ptMtadpMultiCascadeMsg->IsLocalPart());
				tTempMt.SetMcuIdx(LOCAL_MCUIDX);
			}
			cMsg.CatMsgBody( (u8 *)&tTempMt, sizeof(TMt) );
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuAddMixerMemeberCmd]nIndex(%d)Mt(%d,%d)byReplace(%d)\n",nIndex,tTempMt.GetMcuId(),tTempMt.GetMtId(),byReplace);
		}
		cMsg.CatMsgBody( (u8*)&byReplace, sizeof(u8));
		return;
	}

	//TMcu tMcu;
	//tMcu.SetMcuId(LOCAL_MCUID);
	s32  nMtNum = 0;
	u32  *pdwPartId = NULL;
	u8   byReplace   = 0;
	TMtadpHeadMsg tMtadpHeadMsg;
    //LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);

	/*if (strcmp(lpszPeerVerId, "10") == 0)
	{
		nMtNum    = (cServMsg.GetMsgBodyLen() - sizeof(u8) - sizeof(TMtadpHeadMsg)) / sizeof(u32);
		pdwPartId = (u32 *)cServMsg.GetMsgBody();
		tMtadpHeadMsg = *(TMtadpHeadMsg*)(cServMsg.GetMsgBody()+ sizeof(u32) * nMtNum + sizeof(u8));
	} 
	else
	{
		nMtNum = ( cServMsg.GetMsgBodyLen() ) / sizeof(u32);
		pdwPartId = (u32 *)( cServMsg.GetMsgBody() );
	}*/

	if( strcmp(lpszPeerVerId, "3.6") == 0 || strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "8") == 0 || strcmp(lpszPeerVerId, "9") == 0 ||
		strcmp(lpszPeerVerId, "5") == 0 || strcmp(lpszPeerVerId, "mcu4.0 R3") == 0
		)
	{
		nMtNum = ( cServMsg.GetMsgBodyLen() ) / sizeof(u32);
		pdwPartId = (u32 *)( cServMsg.GetMsgBody() );
	}
	else
	{
		nMtNum    = (cServMsg.GetMsgBodyLen() - sizeof(u8) - sizeof(TMtadpHeadMsg)) / sizeof(u32);
		pdwPartId = (u32 *)cServMsg.GetMsgBody();
		tMtadpHeadMsg = *(TMtadpHeadMsg*)(cServMsg.GetMsgBody()+ sizeof(u32) * (u32)nMtNum + sizeof(u8));
	}
	
	byReplace   = *(u8*)(cServMsg.GetMsgBody() + sizeof(u32) * (u32)nMtNum);
    //cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
	if ( tMtadpHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		TMt tTmpMt;
		u32 dwTempPartid = 0;
		BOOL32 bIsLocal = TRUE;
		tMtadpHeadMsg.m_tMsgDst.GetPartGid(&dwTempPartid,&bIsLocal);
		tTmpMt = GetMtFromPartId(ntohl(dwTempPartid), TRUE);
		//cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
		for( s32 nIndex = 0; nIndex < nMtNum; nIndex ++ )
		{
			if (pdwPartId == NULL)
			{
				break;
			}
			TMt tMt;
			tMt.SetMcuId(tTmpMt.GetMtId());
			tMt.SetMtId((u8)(ntohl(*pdwPartId)));
			cMsg.CatMsgBody( (u8 *)&tMt, sizeof(tMt) );
			pdwPartId ++ ;
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuAddMixerMemeberCmd]mcuid(%d) mtid(%d)\n",tMt.GetMcuId(),tMt.GetMtId());
		}
	} 
	else
	{
		for(s32 nIndex = 0; nIndex < nMtNum; nIndex ++ )
		{
			if (pdwPartId == NULL)
			{
				break;
			}
			TMt tMt;
			tMt = GetMtFromPartId(ntohl(*pdwPartId), TRUE);
		
			tMt.SetMcuIdx(LOCAL_MCUIDX);
			cMsg.CatMsgBody( (u8 *)&tMt, sizeof(tMt) );
			pdwPartId ++ ;
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuAddMixerMemeberCmd]mcuid(%d) mtid(%d)\n",tMt.GetMcuId(),tMt.GetMtId());
		}
	}

    cMsg.CatMsgBody( (u8*)&byReplace, sizeof(u8));
	

	s8 achBuff[1024] = {0};
	s8 *pBuff = (s8 *)achBuff;
	u16 wLen = 0;

	wLen += sprintf(pBuff, "ProcMcuMcuAddMixerMemeberCmd MsgLen: %d Bodylen:%d Mtnum:%d Replace:%d",
		cServMsg.GetServMsgLen(),
		cServMsg.GetMsgBodyLen(),
		nMtNum,
		byReplace
		);

	if (nMtNum > 0)
	{
		TMt *ptMt = (TMt *)(cMsg.GetMsgBody() + sizeof(TMcu));

		if (ptMt != NULL)
		{
			wLen += sprintf(pBuff + wLen," MTID: <%d, %d>", ptMt->GetMcuId(), ptMt->GetMtId());
		}
	}

	wLen += sprintf(pBuff + wLen, "\n");

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, achBuff);

	return;
}

/*=============================================================================
  函 数 名： ProcMcuMcuRemoveMixerMemeberCmd
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcMcuMcuRemoveMixerMemeberCmd(const CServMsg& cServMsg, CServMsg& cMsg)
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen());

	TMcu tMcu;
	tMcu.SetMcuId(LOCAL_MCUID);
    /*
	cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );

	s32  nMtNum = ( cServMsg.GetMsgBodyLen() ) / sizeof(u32);
	u32 *pdwPartId = (u32 *)( cServMsg.GetMsgBody() );
    */
	s32  nMtNum = 0;
	u32  *pdwPartId = NULL;
	u8 byNotForceRemove = 0;
	TMtadpHeadMsg tMtadpHeadMsg;
    LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);
	/*if (strcmp(lpszPeerVerId, "10") == 0)
	{
		nMtNum    = (cServMsg.GetMsgBodyLen() - sizeof(u8) - sizeof(TMtadpHeadMsg)) / sizeof(u32);
		pdwPartId = (u32 *)cServMsg.GetMsgBody();
		tMtadpHeadMsg = *(TMtadpHeadMsg*)(cServMsg.GetMsgBody()+ sizeof(u32) * nMtNum + sizeof(u8));
		byNotForceRemove   = *(u8*)(cServMsg.GetMsgBody() + sizeof(u32) * nMtNum);
	} 
	else
	{
		nMtNum = ( cServMsg.GetMsgBodyLen() ) / sizeof(u32);
		pdwPartId = (u32 *)( cServMsg.GetMsgBody() );
		if (cServMsg.GetMsgBodyLen() >= sizeof(u32) * nMtNum + sizeof(u8))
		{
			byNotForceRemove   = *(u8*)(cServMsg.GetMsgBody() + sizeof(u32) * nMtNum);
		}
	}*/

	if( strcmp(lpszPeerVerId, "3.6") == 0 || strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "8") == 0 || strcmp(lpszPeerVerId, "9") == 0 ||
		strcmp(lpszPeerVerId, "5") == 0
		)
	{
		nMtNum = ( cServMsg.GetMsgBodyLen() ) / sizeof(u32);
		pdwPartId = (u32 *)( cServMsg.GetMsgBody() );
// 		if (cServMsg.GetMsgBodyLen() >= sizeof(u32) * (u32)nMtNum + sizeof(u8))
// 		{
// 			byNotForceRemove   = *(u8*)(cServMsg.GetMsgBody() + sizeof(u32) * (u32)nMtNum);
// 		}

		// miaoqingsong [20111011] SP3:MCU_MCU_REMOVEMIXMEMBER_CMD消息体为TMcu + TMt数组 + (u8)byNotStopNoMem + (u8)byNotForceRemove
		//                         Sp1:MCU_MCU_REMOVEMIXMEMBER_CMD消息体为TMcu + TMt数组 + (u8)byNotForceRemove
		// SP3做上级级联SP1在上级删除下级混音成员时消息解析错误(是否取消强制混音应该是第二个(u8)字节)
		if (cServMsg.GetMsgBodyLen() > sizeof(u32) * (u32)nMtNum + sizeof(u8))
		{
			byNotForceRemove   = *(u8*)(cServMsg.GetMsgBody() + sizeof(u32) * (u32)nMtNum + sizeof(u8));
		}
	}
	else
	{
		nMtNum    = (cServMsg.GetMsgBodyLen() - sizeof(u8) - sizeof(TMtadpHeadMsg)) / sizeof(u32);
		pdwPartId = (u32 *)cServMsg.GetMsgBody();
		tMtadpHeadMsg = *(TMtadpHeadMsg*)(cServMsg.GetMsgBody()+ sizeof(u32) * (u32)nMtNum + sizeof(u8));
		byNotForceRemove   = *(u8*)(cServMsg.GetMsgBody() + sizeof(u32) * (u32)nMtNum);
	}


	cMsg.CatMsgBody((u8 *)&byNotForceRemove, sizeof(byNotForceRemove));
    cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
	if ( tMtadpHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		TMt tTempMt;
		u32 dwTempPartid = 0;
		BOOL32 bIsLocal = TRUE;
		tMtadpHeadMsg.m_tMsgDst.GetPartGid(&dwTempPartid,&bIsLocal);
		tTempMt = GetMtFromPartId(ntohl(dwTempPartid), TRUE);
		cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
		for( s32 nIndex = 0; nIndex < nMtNum; nIndex ++ )
		{
			TMt tMt;
			tMt.SetMcuId(tTempMt.GetMtId());
			if (pdwPartId == NULL)
			{
				break;
			}
			tMt.SetMtId((u8)(ntohl(*pdwPartId)));
			cMsg.CatMsgBody( (u8 *)&tMt, sizeof(tMt) );
			pdwPartId ++ ;
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuRemoveMixerMemeberCmd]mcuid(%d) mtid(%d)\n",tMt.GetMcuId(),tMt.GetMtId());
		}
	} 
	else
	{
		for(s32 nIndex = 0; nIndex < nMtNum; nIndex ++ )
		{
			TMt tMt;
			if (pdwPartId == NULL)
			{
				break;
			}
			tMt = GetMtFromPartId(ntohl(*pdwPartId), TRUE);
			
			tMt.SetMcuIdx(LOCAL_MCUIDX);
			cMsg.CatMsgBody( (u8 *)&tMt, sizeof(tMt) );
			pdwPartId ++ ;
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuRemoveMixerMemeberCmd]mcuid(%d) mtid(%d)\n",tMt.GetMcuId(),tMt.GetMtId());
		}
	}

    cMsg.CatMsgBody( (u8*)&byNotForceRemove, sizeof(u8));

	s8 achBuff[1024] = {0};
	s8 *pBuff = (s8 *)achBuff;
	u16 wLen = 0;

	wLen += sprintf(pBuff, "ProcMcuMcuRemoveMixerMemeberCmd MsgLen: %d Bodylen:%d Mtnum:%d ForceRemove:%d",
		cServMsg.GetServMsgLen(),
		cServMsg.GetMsgBodyLen(),
		nMtNum,
		byNotForceRemove == 1 ? 0 : 1);

	if (nMtNum > 0)
	{
		TMt *ptMt = (TMt *)(cMsg.GetMsgBody() + sizeof(TMcu));
		
		if (ptMt != NULL)
		{
			wLen += sprintf(pBuff + wLen, " MTID: <%d, %d>", ptMt->GetMcuId(), ptMt->GetMtId());
		}
	}
	
	wLen += sprintf(pBuff + wLen, "\n");

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, achBuff);

	return;
}

/*=============================================================================
函 数 名： ProcMcuMcuAdjMtFpsReq
功    能： 处理级联调帧率的请求
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
CServMsg& cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20100423      4.6         pengjie                create
=============================================================================*/
void CMtAdpInst::ProcMcuMcuAdjMtFpsReq ( const CServMsg &cServMsg, CServMsg &cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );

	TMtadpHeadMsg tMtadpHeadMsg;
	TMsgHeadMsg tHeadMsg;
 	TPartID tPartID;

	// [pengjie 2010/8/23] 三级级联支持
	BOOL32 bMcuIsSupportMultiCascade = TRUE;
	LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);

	/*if( strcmp(lpszPeerVerId, "10") == 0 )
	{		
		bMcuIsSupportMultiCascade = TRUE;
		MtAdpInfo("[ProcMcuMcuAdjMtResReq] mcu %s version,IsSupportMultiCascade\n", lpszPeerVerId);		
	}*/

	if( strcmp(lpszPeerVerId, "3.6") == 0 || strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "8") == 0 || strcmp(lpszPeerVerId, "9") == 0 ||
		strcmp(lpszPeerVerId, "5") == 0
		)
	{
		bMcuIsSupportMultiCascade = FALSE;
	}

	if( TRUE == bMcuIsSupportMultiCascade )
	{
		tMtadpHeadMsg = *(TMtadpHeadMsg*)( cServMsg.GetMsgBody() + (sizeof(u8) * 3) );
		tPartID.m_dwPID = tMtadpHeadMsg.m_tMsgDst.m_dwPID;	
	}
	else
	{
		tPartID = *(TPartID *)cServMsg.GetMsgBody();
	}

	tPartID.m_dwPID = ntohl( tPartID.m_dwPID );
	TMt tMt = GetMtFromPartId(tPartID.m_dwPID, TRUE);
	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );
	
	++tHeadMsg.m_tMsgSrc.m_byCasLevel;
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		tMt.SetMcuId( tMt.GetMtId() );
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
		tMt.SetMtId( tHeadMsg.m_tMsgDst.m_abyMtIdentify[tHeadMsg.m_tMsgDst.m_byCasLevel] );		
	}

	if( TRUE == bMcuIsSupportMultiCascade )
	{
		cMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
		cMsg.CatMsgBody( (u8 *)cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() - sizeof(TMtadpHeadMsg) );
		cMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	}
	else
	{
		cMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
		cMsg.CatMsgBody( (u8 *)( cServMsg.GetMsgBody() + sizeof(TPartID) ), 
		cServMsg.GetMsgBodyLen() - sizeof(TPartID) );
	}
	// End
		
	return;
}

/*=============================================================================
函 数 名： ProcMcuMcuAdjMtFpsRsp
功    能： 处理级联调帧率的回应
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
CServMsg& cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20100423      4.6         pengjie                create
=============================================================================*/
void CMtAdpInst::ProcMcuMcuAdjMtFpsRsp ( const CServMsg &cServMsg, CServMsg &cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TPartID tPartID = *(TPartID *)cServMsg.GetMsgBody();
	TMt tMt = GetMtFromPartId(tPartID.m_dwPID, FALSE);
	
	cMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
	cMsg.CatMsgBody( (u8 *)( cServMsg.GetMsgBody() + sizeof(TPartID) ), 
		cServMsg.GetMsgBodyLen() - sizeof(TPartID) );
	
	return;
}

/*=============================================================================
函 数 名： ProcMMcuFeccCmd
功    能： 级联远摇处理
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
CServMsg& cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20100812      4.6         pengjie                create
=============================================================================*/
void CMtAdpInst::ProcMMcuFeccCmd( const CServMsg &cServMsg, CServMsg &cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );

	TPartID tPartID;
	TMtadpHeadMsg tMtadpHeadMsg;
	TMsgHeadMsg tHeadMsg;
	tMtadpHeadMsg = *(TMtadpHeadMsg*)( cServMsg.GetMsgBody() + sizeof(u8) + sizeof(u16) );
	tPartID.m_dwPID = tMtadpHeadMsg.m_tMsgDst.m_dwPID;
	tPartID.m_dwPID = ntohl( tPartID.m_dwPID );
	TMt tMt = GetMtFromPartId(tPartID.m_dwPID, TRUE);
	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );

	++tHeadMsg.m_tMsgSrc.m_byCasLevel;
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		tMt.SetMcuId( tMt.GetMtId() );
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
		tMt.SetMtId( tHeadMsg.m_tMsgDst.m_abyMtIdentify[tHeadMsg.m_tMsgDst.m_byCasLevel] );		
	}

	cMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
	cMsg.CatMsgBody( cServMsg.GetMsgBody(), (cServMsg.GetMsgBodyLen() - sizeof(TMtadpHeadMsg)) );
	cMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
//	TPartID tPartID = *(TPartID *)cServMsg.GetMsgBody();
// 	u8 byFeccParam = *(u8 *)( cServMsg.GetMsgBody() + sizeof(tPartID) );
// 	u16 wFeccEventId = *(u16 *)( cServMsg.GetMsgBody() + sizeof(tPartID) + sizeof(byFeccParam) );
// 	cMsg.SetMsgBody( (u8 *)&tMt, sizeof(tMt) );
// 	cMsg.CatMsgBody( (u8 *)&byFeccParam, sizeof(byFeccParam) );
// 	cMsg.CatMsgBody( (u8 *)&wFeccEventId, sizeof(wFeccEventId) );
	
	return;
}
/*=============================================================================
函 数 名： ProcMMcuChangeMtSecVidSendCmd
功    能： VCS控制发言人发双流级联消息处理
算法实现： 
全局变量： 
参    数：  const CServMsg& cServMsg
            CServMsg& cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20110506      4.6         朱胜泽                create
=============================================================================*/
//[5/4/2011 zhushengze]VCS控制发言人发双流
void CMtAdpInst::ProcMMcuChangeMtSecVidSendCmd(const CServMsg &cServMsg, CServMsg &cMsg )
{

    cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );

    TMtadpHeadMsg tMtadpHeadMsg = *(TMtadpHeadMsg*)cServMsg.GetMsgBody();   
    u8 byIsSendDStream = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMtadpHeadMsg));

    tMtadpHeadMsg.m_tMsgDst.m_dwPID = ntohl( tMtadpHeadMsg.m_tMsgDst.m_dwPID );
    TMt tMt = GetMtFromPartId( tMtadpHeadMsg.m_tMsgDst.m_dwPID,TRUE );
    
    if( tMtadpHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
    {
        --tMtadpHeadMsg.m_tMsgDst.m_byCasLevel;
        tMt.SetMcuId( tMt.GetMtId() );
        tMt.SetMtId( tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[tMtadpHeadMsg.m_tMsgDst.m_byCasLevel] );
	}

    TMsgHeadMsg  tMsgHeadMsg;
	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tMsgHeadMsg );

    cMsg.SetMsgBody((u8*)&tMsgHeadMsg, sizeof(TMsgHeadMsg));
    cMsg.CatMsgBody((u8*)&tMt, sizeof(TMt));
    cMsg.CatMsgBody((u8*)&byIsSendDStream, sizeof(u8));

    return;
}
/*=============================================================================
  函 数 名： ProcMcuMcuMuteDumbReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg	[i]
             CServMsg& cMsg		[o]
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  10/08/09		4.6			xl						create
=============================================================================*/
void CMtAdpInst::ProcMcuMcuMuteDumbReq(const CServMsg& cServMsg, CServMsg& cMsg)
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
 	TPartID tPartID = *(TPartID *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));
	tPartID.m_dwPID = ntohl( tPartID.m_dwPID );

	
	TMt tMt = GetMtFromPartId(tPartID.m_dwPID, TRUE);

	StaticLog( "[Mtadp][ProcMcuMcuMuteDumbReq]TpartId info--PID: %d, bLocal: %d\n",
		tPartID.m_dwPID, tPartID.m_bLocal);
	StaticLog( "[Mtadp][ProcMcuMcuMuteDumbReq]PID->tmt--mcuId: %u, mtId: %u\n", tMt.GetMcuId(), tMt.GetMtId() );


	++tHeadMsg.m_tMsgSrc.m_byCasLevel;
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		tMt.SetMcuId( tMt.GetMtId() );
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
		tMt.SetMtId( tHeadMsg.m_tMsgDst.m_abyMtIdentify[tHeadMsg.m_tMsgDst.m_byCasLevel] );		
	}
	else
	{
		tMt.SetMcuId( LOCAL_MCUID );	//强制成本地MCU ID //not neccessarily
	}

	StaticLog( "[Mtadp][ProcMcuMcuMuteDumbReq]tmt restructed--mcuIdx: %d, mcuId: %u, mtId: %u\n",
		tMt.GetMcuIdx(), tMt.GetMcuId(), tMt.GetMtId() );

	cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );
	cMsg.CatMsgBody( (u8 *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TPartID)), 
		cServMsg.GetMsgBodyLen() - sizeof(TPartID) -sizeof(TMsgHeadMsg)
		);

	return;
}

/*=============================================================================
函 数 名： ProcMMcuTransparentMsgNtf
功    能： 界面、终端消息透传处理
算法实现： 
全局变量： 
参    数：  const CServMsg& cServMsg
CServMsg& cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		   版本		修改人		走读人    修改内容
2/23/2012      4.6      朱胜泽                create
=============================================================================*/
void CMtAdpInst::ProcMMcuTransparentMsgNtf( const CServMsg &cServMsg, CServMsg &cMsg )
{
    cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
    
    TMtadpHeadMsg tMtadpHeadMsg = *(TMtadpHeadMsg*)cServMsg.GetMsgBody();   
    u8* pbyMsgContent = (u8*)(cServMsg.GetMsgBody() + sizeof(TMtadpHeadMsg));
    u16 dwMsgLen = cServMsg.GetMsgBodyLen() - sizeof(TMtadpHeadMsg);
    
    tMtadpHeadMsg.m_tMsgDst.m_dwPID = ntohl( tMtadpHeadMsg.m_tMsgDst.m_dwPID );
    TMt tMt = GetMtFromPartId( tMtadpHeadMsg.m_tMsgDst.m_dwPID,TRUE );
    
    if( tMtadpHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
    {
        --tMtadpHeadMsg.m_tMsgDst.m_byCasLevel;
        tMt.SetMcuId( tMt.GetMtId() );
        tMt.SetMtId( tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[tMtadpHeadMsg.m_tMsgDst.m_byCasLevel] );
    }
    
    TMsgHeadMsg  tMsgHeadMsg;
    MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tMsgHeadMsg );
    
    cMsg.SetMsgBody((u8*)&tMsgHeadMsg, sizeof(TMsgHeadMsg));
    cMsg.CatMsgBody((u8*)&tMt, sizeof(TMt));
    cMsg.CatMsgBody((u8*)pbyMsgContent, dwMsgLen);
    
    return;
}

/*=============================================================================
  函 数 名： ProcMcuMcuAdjMtBitrateCmd
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2012/05/12	4.7.1		周晶晶		周晶晶			创建
=============================================================================*/
void CMtAdpInst::ProcMcuMcuAdjMtBitrateCmd(const CServMsg& cServMsg, CServMsg& cMsg)
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TMtadpHeadMsg tMtadpHeadMsg;// = *(TMtadpHeadMsg*)cServMsg.GetMsgBody();
	TMsgHeadMsg tHeadMsg;

	TPartID tPartID;
	tMtadpHeadMsg = *(TMtadpHeadMsg*)( cServMsg.GetMsgBody() );
	tPartID.m_dwPID = tMtadpHeadMsg.m_tMsgDst.m_dwPID;	

	if( 0 == tPartID.m_dwPID )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcMcuMcuAdjMtBitrateCmd] Fail to obtain mt,because of tPartID is null.\n" );
	}


	tPartID.m_dwPID = ntohl( tPartID.m_dwPID );
	
	TMt tMt = GetMtFromPartId(tPartID.m_dwPID, /*tPartID.m_bLocal*/TRUE);

	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );

	++tHeadMsg.m_tMsgSrc.m_byCasLevel;
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		tMt.SetMcuId( tMt.GetMtId() );
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
		tMt.SetMtId( tHeadMsg.m_tMsgDst.m_abyMtIdentify[tHeadMsg.m_tMsgDst.m_byCasLevel] );		
	}
	
	cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );
	
	u8 bIsRecover =  *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMtadpHeadMsg) );
	u16 wBitRate = *(u16*)( cServMsg.GetMsgBody() + sizeof(TMtadpHeadMsg) + sizeof(u8) );
	wBitRate = ntohs( wBitRate );
	cMsg.CatMsgBody( (u8 *)&bIsRecover, sizeof(bIsRecover) );
	wBitRate = htons( wBitRate );
	cMsg.CatMsgBody( (u8 *)&wBitRate, sizeof(wBitRate) );

}

/*=============================================================================
  函 数 名： ProcMcuMcuAdjMtResReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcMcuMcuAdjMtResReq(const CServMsg& cServMsg, CServMsg& cMsg)
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TMtadpHeadMsg tMtadpHeadMsg;// = *(TMtadpHeadMsg*)cServMsg.GetMsgBody();
	TMsgHeadMsg tHeadMsg;
 	TPartID tPartID;// = *(TPartID *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));

	BOOL32 bMcuIsSupportMultiCascade = TRUE;
	LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);

	/*if( strcmp(lpszPeerVerId, "10") == 0 )
	{		
		bMcuIsSupportMultiCascade = TRUE;
		MtAdpInfo("[ProcMcuMcuAdjMtResReq] mcu %s version,IsSupportMultiCascade\n", lpszPeerVerId);		
	}*/

	if( strcmp(lpszPeerVerId, "3.6") == 0 || strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "8") == 0 || strcmp(lpszPeerVerId, "9") == 0 ||
		strcmp(lpszPeerVerId, "5") == 0
		)
	{
		bMcuIsSupportMultiCascade = FALSE;
	}

	if( bMcuIsSupportMultiCascade )
	{
		tMtadpHeadMsg = *(TMtadpHeadMsg*)( cServMsg.GetMsgBody() );
		tPartID.m_dwPID = tMtadpHeadMsg.m_tMsgDst.m_dwPID;		
	}
	else
	{
		tPartID = *(TPartID *)cServMsg.GetMsgBody();		
	}
	
	if( 0 == tPartID.m_dwPID )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcMcuMcuAdjMtResReq] Fail to obtain mt,because of tPartID is null.McuIsSupportMultiCascade(%d)\n", 
			bMcuIsSupportMultiCascade
			);
	}


	tPartID.m_dwPID = ntohl( tPartID.m_dwPID );

	
	TMt tMt = GetMtFromPartId(tPartID.m_dwPID, /*tPartID.m_bLocal*/TRUE);

	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );

	++tHeadMsg.m_tMsgSrc.m_byCasLevel;
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		tMt.SetMcuId( tMt.GetMtId() );
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
		tMt.SetMtId( tHeadMsg.m_tMsgDst.m_abyMtIdentify[tHeadMsg.m_tMsgDst.m_byCasLevel] );		
	}

	//tMt.SetMcuId( LOCAL_MCUID );	//强制成本地MCU ID
	cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );
	if( bMcuIsSupportMultiCascade )
	{
		cMsg.CatMsgBody( (u8 *)( cServMsg.GetMsgBody() + sizeof(TMtadpHeadMsg)), 
			cServMsg.GetMsgBodyLen() - sizeof(TMtadpHeadMsg)
			);
	}
	else
	{
		cMsg.CatMsgBody( (u8 *)( cServMsg.GetMsgBody() + sizeof(TPartID)), 
			cServMsg.GetMsgBodyLen() - sizeof(TPartID)
			);
	}
	/*cMsg.CatMsgBody( (u8 *)( cServMsg.GetMsgBody() + sizeof(TMtadpHeadMsg)), 
		cServMsg.GetMsgBodyLen() - sizeof(TMtadpHeadMsg)
		);*/

	

}




/*=============================================================================
  函 数 名： ProcMcuMcuAdjMtResRsp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcMcuMcuAdjMtResRsp(const CServMsg& cServMsg, CServMsg& cMsg)
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TMtadpHeadMsg tMtadpHeadMsg;// = *(TMtadpHeadMsg*)(cServMsg.GetMsgBody());
 	TPartID tPartID;// = *(TPartID *)(cServMsg.GetMsgBody()+sizeof(TMsgHeadMsg));
	TMsgHeadMsg tHeadMsg;

	BOOL32 bMcuIsSupportMultiCascade = TRUE;
	LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);

	/*if( strcmp(lpszPeerVerId, "10") == 0 )
	{		
		bMcuIsSupportMultiCascade = TRUE;
		MtAdpInfo("[ProcMcuMcuAdjMtResReq] mcu %s version,IsSupportMultiCascade\n", lpszPeerVerId);		
	}*/
	if( strcmp(lpszPeerVerId, "3.6") == 0 || strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "8") == 0 || strcmp(lpszPeerVerId, "9") == 0 ||
		strcmp(lpszPeerVerId, "5") == 0
		)
	{
		bMcuIsSupportMultiCascade = FALSE;
	}

	if( bMcuIsSupportMultiCascade )
	{
		tMtadpHeadMsg = *(TMtadpHeadMsg*)cServMsg.GetMsgBody();
		tPartID.m_dwPID = tMtadpHeadMsg.m_tMsgSrc.m_dwPID;
	}
	else
	{
		tPartID = *(TPartID*)cServMsg.GetMsgBody();
	}

	tPartID.m_dwPID = ntohl( tPartID.m_dwPID );

	TMt tMt = GetMtFromPartId(tPartID.m_dwPID, /*tPartID.m_bLocal*/FALSE);

	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );
	
	//tMt.SetMcuId( LOCAL_MCUID );	//强制成本地MCU ID
	cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );
	if( bMcuIsSupportMultiCascade )
	{
		cMsg.CatMsgBody( (u8 *)( cServMsg.GetMsgBody() + sizeof(TMtadpHeadMsg)), 
			cServMsg.GetMsgBodyLen() - sizeof(TMtadpHeadMsg));
	}
	else
	{
		cMsg.CatMsgBody( (u8 *)( cServMsg.GetMsgBody() + sizeof(TPartID)), 
			cServMsg.GetMsgBodyLen() - sizeof(TPartID));
	}
	/*cMsg.CatMsgBody( (u8 *)( cServMsg.GetMsgBody() + sizeof(TMtadpHeadMsg)), 
		cServMsg.GetMsgBodyLen() - sizeof(TMtadpHeadMsg));*/


	
	return;
}


/*=============================================================================
  函 数 名： ProcMcuMcuLockReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcMcuMcuLockReq(const CServMsg& cServMsg, CServMsg& cMsg)
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );

	TMcu tMcu;
	tMcu.SetMcuId( m_byMtId );
	cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
	cMsg.CatMsgBody( cServMsg.GetMsgBody(), sizeof(u8) );

	return;
}

/*=============================================================================
  函 数 名： ProcMcuMcuLockAck
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcMcuMcuLockAck( const CServMsg& cServMsg, CServMsg& cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );

	TMcu tMcu;
	tMcu.SetMcuId( m_byMtId );
	cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
	cMsg.CatMsgBody( cServMsg.GetMsgBody(), sizeof(u8) );

	return;
}

/*=============================================================================
函 数 名： ProcMcuMcuPreSetinReq
功    能： 进行多级联准备（双方握手）
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           CServMsg &cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
2009-11-11				 xl					   modify msgbody
=============================================================================*/
void CMtAdpInst::ProcMcuMcuPreSetinReq( const CServMsg &cServMsg, CServMsg &cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TMsgHeadMsg tHeadMsg;

	//TPartID tPartID = *(TPartID *)cServMsg.GetMsgBody();
	TMtadpHeadMsg tMtadpHeadMsg = *(TMtadpHeadMsg*)(cServMsg.GetMsgBody());

	tMtadpHeadMsg.m_tMsgDst.m_dwPID = ntohl( tMtadpHeadMsg.m_tMsgDst.m_dwPID );
	TMt tMt = GetMtFromPartId(tMtadpHeadMsg.m_tMsgDst.m_dwPID, /*tPartID.m_bLocal*/TRUE);
	
	TPreSetInReq tPreSetInReq = *(TPreSetInReq *)(cServMsg.GetMsgBody() + sizeof(TMtadpHeadMsg));


	//tPreSetInReq.m_dwEvId = ntohl( tPreSetInReq.m_dwEvId );
	//tPreSetInReq.m_tReleaseMtInfo.m_swCount = ntohs( tPreSetInReq.m_tReleaseMtInfo.m_swCount );

	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );

	++tHeadMsg.m_tMsgSrc.m_byCasLevel;
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		tMt.SetMcuId( tMt.GetMtId() );
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
		tMt.SetMtId( tHeadMsg.m_tMsgDst.m_abyMtIdentify[tHeadMsg.m_tMsgDst.m_byCasLevel] );			
	}
	//tHeadMsg.m_tMsgDst.m_tMt = tMt;
	tPreSetInReq.m_tSpyMtInfo.SetSpyMt( tMt );	

	u8 byMsgPartIdLength = 0;
	if( tPreSetInReq.m_tReleaseMtInfo.GetCount() >= 0/* &&
		cServMsg.GetMsgBodyLen() > ( sizeof(TMtadpHeadMsg) + sizeof(TPreSetInReq) */)
	{
		TPartID tPartID = *(TPartID*)(cServMsg.GetMsgBody()+sizeof(TMtadpHeadMsg) + sizeof(TPreSetInReq) );
		tMt = GetMtFromPartId( ntohl(tPartID.m_dwPID), /*tPartID.m_bLocal*/TRUE);
		if( tPreSetInReq.m_tReleaseMtInfo.m_byCasLevel > 0 )
		{
			tMt.SetMcuId( tMt.GetMtId() );
			--tPreSetInReq.m_tReleaseMtInfo.m_byCasLevel;
			tMt.SetMtId( tPreSetInReq.m_tReleaseMtInfo.m_abyMtIdentify[tPreSetInReq.m_tReleaseMtInfo.m_byCasLevel] );				
		}
		tPreSetInReq.m_tReleaseMtInfo.m_tMt = tMt;
		byMsgPartIdLength = sizeof(TPartID);
	}

	cMsg.SetMsgBody( (u8 *)&tPreSetInReq, sizeof(tPreSetInReq) );
	cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg));
	
	u32 dwResWidth = 0;
	u32 dwResHeight = 0;

	if( cServMsg.GetMsgBodyLen() > (sizeof(TPreSetInReq) + sizeof(TMtadpHeadMsg)+byMsgPartIdLength) )
	{
		dwResWidth = *(u32 *)( cServMsg.GetMsgBody() + sizeof(TPreSetInReq)+sizeof(TMtadpHeadMsg)+byMsgPartIdLength);
		dwResHeight = *(u32 *)( cServMsg.GetMsgBody() + sizeof(TPreSetInReq) +sizeof(u32)+sizeof(TMtadpHeadMsg)+byMsgPartIdLength);
		cMsg.CatMsgBody((u8*)&dwResWidth,sizeof(dwResWidth));
		cMsg.CatMsgBody((u8*)&dwResHeight,sizeof(dwResHeight));
	}

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuPreSetinReq] Recv PreSetin tSrc(%d.%d.%d) level.%d ReleaseMt(%d.%d.%d) level.%d byCount.%d!\n",
		tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
		tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMtId(),
		tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
		tMtadpHeadMsg.m_tMsgDst.m_byCasLevel,
		tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMcuId(),
		tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMtId(),
		tPreSetInReq.m_tReleaseMtInfo.m_abyMtIdentify[0],
		tPreSetInReq.m_tReleaseMtInfo.m_byCasLevel,
		tPreSetInReq.m_tReleaseMtInfo.GetCount()
		);

	return;
}


/*=============================================================================
函 数 名： ProcMcuMcuPreSetinAck
功    能： 进行多级联准备（双方握手）
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           CServMsg &cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
20091111				xl						modify msg body
=============================================================================*/
void CMtAdpInst::ProcMcuMcuPreSetinAck( const CServMsg &cServMsg, CServMsg &cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
 	
	//TPartID tPartID = *(TPartID *)cServMsg.GetMsgBody();
	TMsgHeadMsg tHeadMsg;
	
	TPreSetInRsp tPreSetInRsp = *(TPreSetInRsp *)cServMsg.GetMsgBody();
	TMtadpHeadMsg tMtadpHeadMsg = *(TMtadpHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TPreSetInRsp));

// 	tPreSetInRsp.m_dwVidSpyBW = ntohl( tPreSetInRsp.m_dwVidSpyBW );
// 	tPreSetInRsp.m_dwAudSpyBW = ntohl( tPreSetInRsp.m_dwAudSpyBW );
//	tPreSetInRsp.m_tSetInReqInfo.m_dwEvId = ntohl(tPreSetInRsp.m_tSetInReqInfo.m_dwEvId);
//	tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_swCount = ntohs(tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_swCount);

	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = ntohl( tMtadpHeadMsg.m_tMsgSrc.m_dwPID );

	
	TMt tMt = GetMtFromPartId(tMtadpHeadMsg.m_tMsgSrc.m_dwPID, /*tPartID.m_bLocal*/FALSE);
	
	tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt( tMt );
	//tHeadMsg.m_tMsgSrc.m_tMt = tMt;

	if( tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.GetCount() >= 0 &&
		cServMsg.GetMsgBodyLen() > (sizeof(TPreSetInRsp)+sizeof(TMtadpHeadMsg))
		)
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuPreSetinAck] Recv PreSetin Ack  m_tReleaseMtInfo.m_byCount is large than 0(%d)\n",
			tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.GetCount() );
		
		TPartID tPartID = *(TPartID *)( cServMsg.GetMsgBody()+sizeof(TPreSetInRsp)+sizeof(TMtadpHeadMsg) );
		tPartID.m_dwPID = ntohl( tPartID.m_dwPID );
		tMt = GetMtFromPartId(tPartID.m_dwPID, /*tPartID.m_bLocal*/FALSE);
		tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt = tMt;
	}
	else
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuPreSetinAck] Recv PreSetin Ack  m_tReleaseMtInfo.m_byCount is small than 0(%d)\n",
			tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.GetCount() );
			
		tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.SetCount(tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.GetCount()+1);
		//++tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_swCount;		
	}

	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );

	cMsg.SetMsgBody((u8 *)&tPreSetInRsp, sizeof(tPreSetInRsp));
	cMsg.CatMsgBody((u8 *)&tHeadMsg, sizeof(TMsgHeadMsg));

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuPreSetinAck] Recv PreSetin Ack tSrc(%d.%d.%d) level.%d ReleaseMt(%d.%d.%d) level.%d byCount.%d!\n",
		tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
		tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMtId(),
		tMtadpHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		tMtadpHeadMsg.m_tMsgSrc.m_byCasLevel,
		tMt.GetMcuId(),
		tMt.GetMtId(),
		tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_abyMtIdentify[0],
		tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCasLevel,
		tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.GetCount()
		);

	return;
}

/*=============================================================================
函 数 名： ProcMcuMcuPreSetinNack
功    能： 进行多级联准备（双方握手）
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           CServMsg &cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
=============================================================================*/
void CMtAdpInst::ProcMcuMcuPreSetinNack( const CServMsg &cServMsg, CServMsg &cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	
	//TPartID tPartID = *(TPartID *)cServMsg.GetMsgBody();
	TMsgHeadMsg tHeadMsg;
	
	TPreSetInRsp tPreSetInRsp = *(TPreSetInRsp *)(cServMsg.GetMsgBody());
	TMtadpHeadMsg tMtadpHeadMsg = *(TMtadpHeadMsg*)(cServMsg.GetMsgBody()+ sizeof(TPreSetInRsp));

	//tPreSetInRsp.m_tSetInReqInfo.m_dwEvId = ntohl( tPreSetInRsp.m_tSetInReqInfo.m_dwEvId );
	//tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_swCount = ntohs(tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_swCount);

	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = ntohl(tMtadpHeadMsg.m_tMsgSrc.m_dwPID);
	TMt tMt = GetMtFromPartId(tMtadpHeadMsg.m_tMsgSrc.m_dwPID, /*tPartID.m_bLocal*/FALSE);

	tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt( tMt );
	//tHeadMsg.m_tMsgSrc.m_tMt = tMt;

	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );
	
	cMsg.SetMsgBody((u8 *)&tPreSetInRsp, sizeof(tPreSetInRsp));
	cMsg.CatMsgBody((u8 *)&tHeadMsg, sizeof(TMsgHeadMsg));

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuPreSetinNack] Recv PreSetin Nack tSrc(%d.%d.%d) level.%d!\n",
		tMt.GetMcuId(),tMt.GetMtId(),tMtadpHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		tMtadpHeadMsg.m_tMsgSrc.m_byCasLevel
		);
	return;
}

/*=============================================================================
函 数 名： ProcMcuMcuSpyChnnlNotif
功    能： 通知下级mcu，上级可以接受其回传码流的带宽
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           CServMsg &cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
=============================================================================*/
void CMtAdpInst::ProcMcuMcuSpyChnnlNotif( const CServMsg &cServMsg, CServMsg &cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TMsgHeadMsg tHeadMsg;
	//TPartID tPartID = *(TPartID *)cServMsg.GetMsgBody();
	
	
	TSpyResource tSpyResource = *(TSpyResource *)cServMsg.GetMsgBody();
	u32 dwEvId = *(u32 *)( cServMsg.GetMsgBody() + sizeof(TSpyResource) );
	
	//u8 byRes   = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TSpyResource) + sizeof(u32) );
	TMtadpHeadMsg tMtadpHeadMsg = *(TMtadpHeadMsg*)( cServMsg.GetMsgBody() + sizeof(TSpyResource) + sizeof(u32) /*+ sizeof(u8)*/ );

	BOOL32 bSupportExData = FALSE; // 标识是否支持分辨率宽，高的额外数据.上级v4r7支持[9/14/2012 chendaiwei]
	u32 dwResWidth = 0;
	u32 dwResHeight = 0;
	if( cServMsg.GetMsgBodyLen() > (sizeof(TSpyResource) +sizeof(u32) + sizeof(TMtadpHeadMsg)) )
	{
		bSupportExData = TRUE;
		dwResWidth = *(u32 *)( cServMsg.GetMsgBody() + sizeof(TSpyResource) +sizeof(u32)+sizeof(TMtadpHeadMsg));
		dwResHeight = *(u32 *)( cServMsg.GetMsgBody() + sizeof(TSpyResource) +2*sizeof(u32)+sizeof(TMtadpHeadMsg));
	}
	
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = ntohl(tMtadpHeadMsg.m_tMsgDst.m_dwPID);
	TMt tMt = GetMtFromPartId(tMtadpHeadMsg.m_tMsgDst.m_dwPID, /*tPartID.m_bLocal*/TRUE);

	++tMtadpHeadMsg.m_tMsgSrc.m_byCasLevel;
	if( tMtadpHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{		
		tMt.SetMcuId( tMt.GetMtId() );
		--tMtadpHeadMsg.m_tMsgDst.m_byCasLevel;
		tMt.SetMtId( tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[tMtadpHeadMsg.m_tMsgDst.m_byCasLevel] );	
	}

	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );

	tSpyResource.m_tSpy = tMt;

	cMsg.SetMsgBody((u8 *)&tSpyResource, sizeof(TSpyResource));
	cMsg.CatMsgBody((u8 *)&dwEvId, sizeof(u32));
	cMsg.CatMsgBody((u8 *)&tHeadMsg, sizeof(TMsgHeadMsg));

	if(bSupportExData)
	{
		cMsg.CatMsgBody((u8 *)&dwResWidth, sizeof(u32));
		cMsg.CatMsgBody((u8 *)&dwResHeight, sizeof(u32));
	}

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendSpyChnnlNotif] Recv SpyChnnlNotif tSrc(%d.%d.%d) level.%d !\n",
		tMt.GetMcuId(),tMt.GetMtId(),tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
		tMtadpHeadMsg.m_tMsgDst.m_byCasLevel		
		);
	return;
}

/*=============================================================================
函 数 名： ProcMcuMcuSpyChnnlNotif
功    能： 通知上级mcu，下级建交换到上级mcu失败
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           CServMsg &cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2011-09-08 4.6         zhangli               create
=============================================================================*/
void CMtAdpInst::ProcMcuMcuSwitchToMMcuFailNotif(const CServMsg &cServMsg, CServMsg &cMsg)
{
	cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen()-cServMsg.GetMsgBodyLen());
	TMtadpHeadMsg tMtadpHeadMsg;
 	TPartID tPartID;
	TMsgHeadMsg tHeadMsg;

	tMtadpHeadMsg = *(TMtadpHeadMsg*)cServMsg.GetMsgBody();
	tPartID.m_dwPID = tMtadpHeadMsg.m_tMsgSrc.m_dwPID;
	tPartID.m_dwPID = ntohl(tPartID.m_dwPID);

	TMt tMt = GetMtFromPartId(tPartID.m_dwPID, /*tPartID.m_bLocal*/FALSE);

	MtadpHeadMsgToHeadMsg(tMtadpHeadMsg,tHeadMsg);
	
	cMsg.SetMsgBody((u8 *)&tHeadMsg, sizeof(TMsgHeadMsg));
	cMsg.CatMsgBody((u8 *)&tMt, sizeof(TMt));
	cMsg.CatMsgBody((u8 *)(cServMsg.GetMsgBody()+sizeof(TMtadpHeadMsg)), cServMsg.GetMsgBodyLen()-sizeof(TMtadpHeadMsg));

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuSwitchToMMcuFailNotif] Switch To MMcu fail---tSrc(%d.%d.%d) level.%d!\n",
		tMt.GetMcuId(), tMt.GetMtId(), tMtadpHeadMsg.m_tMsgSrc.m_abyMtIdentify[0], tMtadpHeadMsg.m_tMsgSrc.m_byCasLevel);
}

/*=============================================================================
函 数 名： ProcMcuMcuMtExtInfoNotify
功    能： 下级通知上级RTCP信息
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
CServMsg &cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2010-12-10              倪志俊                   create
=============================================================================*/
void CMtAdpInst::ProcMcuMcuMtExtInfoNotify( const CServMsg &cServMsg, CServMsg &cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	
	//TPartID tPartID = *(TPartID *)cServMsg.GetMsgBody();
	TMsgHeadMsg tHeadMsg;
	
	TMultiRtcpInfo tMultiRtcpInfo = *(TMultiRtcpInfo *)(cServMsg.GetMsgBody());
	TMtadpHeadMsg tMtadpHeadMsg = *(TMtadpHeadMsg*)(cServMsg.GetMsgBody()+ sizeof(TMultiRtcpInfo));
	
	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = ntohl(tMtadpHeadMsg.m_tMsgSrc.m_dwPID);
	TMt tMt = GetMtFromPartId(tMtadpHeadMsg.m_tMsgSrc.m_dwPID, /*tPartID.m_bLocal*/FALSE);
	
	tMultiRtcpInfo.m_tSrcMt = tMt;
	
	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );
	
	cMsg.SetMsgBody((u8 *)&tMultiRtcpInfo, sizeof(TMultiRtcpInfo));
	cMsg.CatMsgBody((u8 *)&tHeadMsg, sizeof(TMsgHeadMsg));
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuMtExtInfoNotify] Recv Mt RTCP ExtInfo----tSrc(%d.%d.%d) level.%d!\n",
		tMt.GetMcuId(),tMt.GetMtId(),tMtadpHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		tMtadpHeadMsg.m_tMsgSrc.m_byCasLevel
		);
	return;
}

/*=============================================================================
函 数 名： ProcMcuMcuSpyBWFullNotif
功    能： 通知下级mcu，上级mcu带宽不足，不能进行级联多回传
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           CServMsg &cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
=============================================================================*/
void CMtAdpInst::ProcMcuMcuRejectSpyNotif( const CServMsg &cServMsg, CServMsg &cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TPartID tPartID = *(TPartID *)cServMsg.GetMsgBody();
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	tPartID.m_dwPID = ntohl( tPartID.m_dwPID );
#endif
	u32 dwSpyBW = *(u32 *)( cServMsg.GetMsgBody() + sizeof(tPartID) );
	TMt tMt = GetMtFromPartId(tPartID.m_dwPID, /*tPartID.m_bLocal*/TRUE);
	
	cMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
	cMsg.CatMsgBody( (u8 *)&dwSpyBW, sizeof(u32) );

	return;
}

/*=============================================================================
函 数 名： ProcMcuMcuBanishSpyCmd
功    能： 上级mcu发给下级的拆除相应的交换，回收回传通道
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           CServMsg &cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
=============================================================================*/
void CMtAdpInst::ProcMcuMcuBanishSpyCmd( const CServMsg &cServMsg, CServMsg &cMsg)
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TMsgHeadMsg tHeadMsg;
	//TPartID tPartID = *(TPartID *)cServMsg.GetMsgBody();
	u8 byMode = *(u8 *)cServMsg.GetMsgBody();
	TMtadpHeadMsg tMtadpHeadMsg = *(TMtadpHeadMsg*)(cServMsg.GetMsgBody() + sizeof(u8) );
	u8 bySpyNoUse = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TMtadpHeadMsg));
	
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = ntohl( tMtadpHeadMsg.m_tMsgDst.m_dwPID );
	
	TMt tMt = GetMtFromPartId( tMtadpHeadMsg.m_tMsgDst.m_dwPID, /*tPartID.m_bLocal*/TRUE );
	
	++tMtadpHeadMsg.m_tMsgSrc.m_byCasLevel;
	if( tMtadpHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{		
		tMt.SetMcuId( tMt.GetMtId() );
		--tMtadpHeadMsg.m_tMsgDst.m_byCasLevel;
		tMt.SetMtId( tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[tMtadpHeadMsg.m_tMsgDst.m_byCasLevel] );	
	}
	
	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );
	
	//tHeadMsg.m_tMsgDst.m_tMt = tMt;	
	cMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
	cMsg.CatMsgBody( (u8 *)&byMode, sizeof(u8) );
	cMsg.CatMsgBody((u8 *)&tHeadMsg, sizeof(TMsgHeadMsg));
	cMsg.CatMsgBody((u8 *)&bySpyNoUse, sizeof(bySpyNoUse));
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendBanishSpyCmd] Recv BanishSpyCmd tSrc(%d.%d.%d) level.%d !\n",
		tMt.GetMcuId(),tMt.GetMtId(),tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
		tMtadpHeadMsg.m_tMsgDst.m_byCasLevel		
		);
	return;
}

/*=============================================================================
函 数 名： ProcMcuMcuBanishSpyNotif
功    能： 上级mcu发给下级的拆除相应的交换，回收回传通道
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           CServMsg &cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
=============================================================================*/
void CMtAdpInst::ProcMcuMcuBanishSpyNotif( const CServMsg &cServMsg, CServMsg &cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TPartID tPartID = *(TPartID *)cServMsg.GetMsgBody();
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	tPartID.m_dwPID = ntohl( tPartID.m_dwPID );
#endif
	u8 byMode = *(u8 *)( cServMsg.GetMsgBody() + sizeof(tPartID) );
	u32 dwSpyBW = *(u32 *)( cServMsg.GetMsgBody() + sizeof(tPartID) + sizeof( u8) );
	TMt tMt = GetMtFromPartId(tPartID.m_dwPID, /*tPartID.m_bLocal*/FALSE);
	
	cMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
	cMsg.CatMsgBody( (u8 *)&byMode, sizeof(u8) );
	cMsg.CatMsgBody( (u8 *)&dwSpyBW, sizeof(u32) );

	return;
}


/*=============================================================================
  函 数 名： ProcMcuMcuLockNack
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcMcuMcuLockNack( const CServMsg& cServMsg, CServMsg& cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );

	TMcu tMcu;
	tMcu.SetMcuId( m_byMtId);
	cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
	cMsg.CatMsgBody( cServMsg.GetMsgBody(), sizeof(u8) );

	return;
}


/*=============================================================================
函 数 名： ProcMcuMcuMultSpyCapNotif
功    能： mcu是否都支持多回传能力互探处理
算法实现： 
全局变量： 
参    数： CServMsg &cServMsg
           CServMsg &cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
=============================================================================*/
void CMtAdpInst::ProcMcuMcuMultSpyCapNotif( const CServMsg &cServMsg, CServMsg &cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TMcu tMcu;
	tMcu.SetMcuId( m_byMtId );
	cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
	cMsg.CatMsgBody(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());

	return;
}

/*=============================================================================
  函 数 名： ProcMcuMcuMtStatusCmd
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcMcuMcuMtStatusCmd(const CServMsg& cServMsg, CServMsg& cMsg)
{
	cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen());

	TMcu tMcu;
	tMcu.SetMcuId( m_byMtId );
	cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );

	return;
}

/*=============================================================================
  函 数 名： ProcMcuMcuMtStatusNtf
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcMcuMcuMtStatusNtf( const CServMsg& cServMsg, CServMsg& cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );

	BOOL32 bWJMcuVer = FALSE;
	BOOL32 bR3FullVer = FALSE;
    LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);

	BOOL32 bMcuIsSupportDisConnectReason = TRUE;
	BOOL32 bMcuIsSupportMultiCascade = TRUE;	

    if ( strcmp(lpszPeerVerId, "4.0") == 0 )
    {
        bWJMcuVer = TRUE;
    }
	
	if( strcmp(lpszPeerVerId, "5") == 0 )
	{
		bR3FullVer = TRUE;
	}
	
	// miaoqingsong [20110526] 修改是否支持“终端呼叫断链原因”和“多回传”条件判断
	// 判断对方是否是4.0R5以前的版本, 4.0R5以前的版本不支持终端呼叫断链原因
	if( strcmp(lpszPeerVerId, "3.6") == 0 || 
		strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "5") == 0   ||
		strcmp(lpszPeerVerId, "6") == 0   ||
		strcmp(lpszPeerVerId, "7") == 0 )
	{
		bMcuIsSupportDisConnectReason = FALSE;
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuMtStatusNtf] mcu %s version,Is not Support MT Disconnect Reason\n", lpszPeerVerId);
	}

	// 版本号"10"之前版本均不支持多回传
	if( strcmp(lpszPeerVerId, "3.6") == 0 || 
		strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "5") == 0   ||
		strcmp(lpszPeerVerId, "6") == 0   ||
		strcmp(lpszPeerVerId, "7") == 0   ||
		strcmp(lpszPeerVerId, "8") == 0   ||
		strcmp(lpszPeerVerId, "9") == 0 )
	{
		bMcuIsSupportMultiCascade = FALSE;
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuMtStatusNtf] mcu %s version,Is not Support MultiCascade.\n", lpszPeerVerId);
	}

	//判断是否是R6或R6Sp4版本
	/*if( strcmp(lpszPeerVerId, "3.6") == 0 || strcmp(lpszPeerVerId, "8") == 0 || strcmp(lpszPeerVerId, "9") == 0  )
	{
		bMcuIsSupportDisConnectReason = FALSE;	
		bMcuIsSupportMultiCascade = FALSE;
		MtAdpInfo("[ProcMcuMcuMtStatusNtf] mcu %s version,Is Not Support MultiCascade and DisConnectReason \n", lpszPeerVerId);
	}*/

	/*if( strcmp(lpszPeerVerId, "10") == 0 )
	{
		bMcuIsSupportDisConnectReason = TRUE;
		bMcuIsSupportMultiCascade = TRUE;
		MtAdpInfo("[ProcMcuMcuMtStatusNtf] mcu %s version,IsSupportMultiCascade\n", lpszPeerVerId);
	}*/

	TMcu tMcu;
	tMcu.SetMcuId( m_byMtId );
	cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );

	u8 byMtNum = 0;
	
	TSMcuMtStatus tSMcuMtStatus;
	TMt tMt;
	if( bMcuIsSupportDisConnectReason )
	{
		if( bMcuIsSupportMultiCascade )
		{
			TMcuToMcuMtStatus *ptMcuToMcuMtStatus = (TMcuToMcuMtStatus*)(cServMsg.GetMsgBody() + sizeof(u8));
			byMtNum = *(u8*)(cServMsg.GetMsgBody());
			
			if( byMtNum > 0 && ptMcuToMcuMtStatus->m_byCasLevel > 0 )
			{				
				tMt = GetMtFromPartId( ptMcuToMcuMtStatus->GetPartId(), FALSE );
				tMcu.SetMtId( tMt.GetMtId() );
				cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );				
			}
			
			cMsg.CatMsgBody( (u8*)&byMtNum,sizeof(byMtNum) );
			
			for( u8 byIndex = 0; byIndex < byMtNum; byIndex ++ )
			{
				tMt = GetMtFromPartId( ptMcuToMcuMtStatus->GetPartId(), FALSE );
				memcpy( (void*)&tSMcuMtStatus, (void*)&tMt, sizeof(tMt) );
				tSMcuMtStatus.SetIsEnableFECC( ptMcuToMcuMtStatus->IsEnableFECC() );
				tSMcuMtStatus.SetCurVideo( ptMcuToMcuMtStatus->GetCurVideo() );
				tSMcuMtStatus.SetCurAudio( ptMcuToMcuMtStatus->GetCurAudio() );
				tSMcuMtStatus.SetMtBoardType( ptMcuToMcuMtStatus->GetMtBoardType() );
				if (bWJMcuVer)
				{
					tSMcuMtStatus.SetIsMixing( ptMcuToMcuMtStatus->IsMixing() );
					tSMcuMtStatus.SetSendVideo( ptMcuToMcuMtStatus->IsVideoLose() );
					tSMcuMtStatus.SetSendAudio( ptMcuToMcuMtStatus->IsAutoCallMode() );
					tSMcuMtStatus.SetRecvVideo( ptMcuToMcuMtStatus->IsSendVideo() );
					tSMcuMtStatus.SetRecvAudio( ptMcuToMcuMtStatus->IsSendAudio() );
				}
				else
				{
					tSMcuMtStatus.SetIsMixing( ptMcuToMcuMtStatus->IsMixing() );
					tSMcuMtStatus.SetVideoLose( ptMcuToMcuMtStatus->IsVideoLose() );
					tSMcuMtStatus.SetSendVideo( ptMcuToMcuMtStatus->IsSendVideo() );
					tSMcuMtStatus.SetSendAudio( ptMcuToMcuMtStatus->IsSendAudio() );
					tSMcuMtStatus.SetRecvVideo( ptMcuToMcuMtStatus->IsRecvVideo() );
					tSMcuMtStatus.SetRecvAudio( ptMcuToMcuMtStatus->IsRecvAudio() );
					tSMcuMtStatus.SetMtExInfo( ptMcuToMcuMtStatus->GetMtExInfo() );
					//sgx20100703[Bug00033516]添加呼叫方式信息  
					tSMcuMtStatus.SetIsAutoCallMode(ptMcuToMcuMtStatus->IsAutoCallMode());
						tSMcuMtStatus.m_byCasLevel = ptMcuToMcuMtStatus->m_byCasLevel;
						memcpy( &tSMcuMtStatus.m_abyMtIdentify[0],
									&ptMcuToMcuMtStatus->m_abyMtIdentify[0],
									sizeof(tSMcuMtStatus.m_abyMtIdentify)
									);
				}
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "ProcMcuMcuMtStatusNtf MtId.%d Mix:%d\n", tMt.GetMtId(), ptMcuToMcuMtStatus->IsMixing() );
				cMsg.CatMsgBody( (u8 *)&tSMcuMtStatus, sizeof(tSMcuMtStatus) );
				++ptMcuToMcuMtStatus;
			}
		}
		else
		{
			byMtNum = (cServMsg.GetMsgBodyLen()) / (sizeof(TMcuToMcuMtStatusBeforeV4R6NoMultiCascade));
			
			cMsg.CatMsgBody( (u8*)&byMtNum,sizeof(byMtNum) );
			
			TMcuToMcuMtStatusBeforeV4R6NoMultiCascade *ptMcuToMcuMtStatusNoMultiCascad = (TMcuToMcuMtStatusBeforeV4R6NoMultiCascade*)(cServMsg.GetMsgBody());
			for( u8 byIndex = 0; byIndex < byMtNum; byIndex ++ )
			{
				tMt = GetMtFromPartId( ptMcuToMcuMtStatusNoMultiCascad->GetPartId(), FALSE );
				memcpy( (void*)&tSMcuMtStatus, (void*)&tMt, sizeof(tMt) );
				tSMcuMtStatus.SetIsEnableFECC( ptMcuToMcuMtStatusNoMultiCascad->IsEnableFECC() );
				tSMcuMtStatus.SetCurVideo( ptMcuToMcuMtStatusNoMultiCascad->GetCurVideo() );
				tSMcuMtStatus.SetCurAudio( ptMcuToMcuMtStatusNoMultiCascad->GetCurAudio() );
				tSMcuMtStatus.SetMtBoardType( ptMcuToMcuMtStatusNoMultiCascad->GetMtBoardType() );
				if (bWJMcuVer)
				{
					tSMcuMtStatus.SetIsMixing( ptMcuToMcuMtStatusNoMultiCascad->IsMixing() );
					tSMcuMtStatus.SetSendVideo( ptMcuToMcuMtStatusNoMultiCascad->IsVideoLose() );
					tSMcuMtStatus.SetSendAudio( ptMcuToMcuMtStatusNoMultiCascad->IsAutoCallMode() );
					tSMcuMtStatus.SetRecvVideo( ptMcuToMcuMtStatusNoMultiCascad->IsSendVideo() );
					tSMcuMtStatus.SetRecvAudio( ptMcuToMcuMtStatusNoMultiCascad->IsSendAudio() );
				}
				else
				{
					tSMcuMtStatus.SetIsMixing( ptMcuToMcuMtStatusNoMultiCascad->IsMixing() );
					tSMcuMtStatus.SetVideoLose( ptMcuToMcuMtStatusNoMultiCascad->IsVideoLose() );
					tSMcuMtStatus.SetSendVideo( ptMcuToMcuMtStatusNoMultiCascad->IsSendVideo() );
					tSMcuMtStatus.SetSendAudio( ptMcuToMcuMtStatusNoMultiCascad->IsSendAudio() );
					tSMcuMtStatus.SetRecvVideo( ptMcuToMcuMtStatusNoMultiCascad->IsRecvVideo() );
					tSMcuMtStatus.SetRecvAudio( ptMcuToMcuMtStatusNoMultiCascad->IsRecvAudio() );
					tSMcuMtStatus.SetMtExInfo( ptMcuToMcuMtStatusNoMultiCascad->GetMtExtInfo() );
					//sgx20100703[Bug00033516]添加呼叫方式信息  
					tSMcuMtStatus.SetIsAutoCallMode(ptMcuToMcuMtStatusNoMultiCascad->IsAutoCallMode());				
				}
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "ProcMcuMcuMtStatusNtf MtId.%d Mix:%d\n", tMt.GetMtId(), ptMcuToMcuMtStatusNoMultiCascad->IsMixing() );
				cMsg.CatMsgBody( (u8 *)&tSMcuMtStatus, sizeof(tSMcuMtStatus) );
				++ptMcuToMcuMtStatusNoMultiCascad;
			}
		}
	}
	else
	{
		byMtNum = (cServMsg.GetMsgBodyLen()) / (sizeof(TMcuToMcuMtStatusBeforeV4R5));
		
		cMsg.CatMsgBody( (u8*)&byMtNum,sizeof(byMtNum) );
		
		TMcuToMcuMtStatusBeforeV4R5 *ptMcuToMcuMtStatusBeforeV4R5 = (TMcuToMcuMtStatusBeforeV4R5*)(cServMsg.GetMsgBody());

		for( u8 byIndex = 0; byIndex < byMtNum; byIndex ++ )
		{
			tMt = GetMtFromPartId( ptMcuToMcuMtStatusBeforeV4R5->GetPartId(), FALSE );
			memcpy( (void*)&tSMcuMtStatus, (void*)&tMt, sizeof(tMt) );
			tSMcuMtStatus.SetIsEnableFECC( ptMcuToMcuMtStatusBeforeV4R5->IsEnableFECC() );
			tSMcuMtStatus.SetCurVideo( ptMcuToMcuMtStatusBeforeV4R5->GetCurVideo() );
			tSMcuMtStatus.SetCurAudio( ptMcuToMcuMtStatusBeforeV4R5->GetCurAudio() );
			tSMcuMtStatus.SetMtBoardType( ptMcuToMcuMtStatusBeforeV4R5->GetMtBoardType() );
			if (bWJMcuVer || bR3FullVer)
			{
				tSMcuMtStatus.SetIsMixing( ptMcuToMcuMtStatusBeforeV4R5->IsMixing() );
				tSMcuMtStatus.SetSendVideo( ptMcuToMcuMtStatusBeforeV4R5->IsVideoLose() );
				tSMcuMtStatus.SetSendAudio( ptMcuToMcuMtStatusBeforeV4R5->IsAutoCallMode() );
				tSMcuMtStatus.SetRecvVideo( ptMcuToMcuMtStatusBeforeV4R5->IsSendVideo() );
				tSMcuMtStatus.SetRecvAudio( ptMcuToMcuMtStatusBeforeV4R5->IsSendAudio() );
				//zjj20101201 由于R3full版本没有发送videolose信息，所以为版本兼容暂时写死成视频源未丢失
				if( bR3FullVer )
				{
					tSMcuMtStatus.SetVideoLose( FALSE );
				}
			}
			else
			{
				tSMcuMtStatus.SetIsMixing( ptMcuToMcuMtStatusBeforeV4R5->IsMixing() );
				tSMcuMtStatus.SetVideoLose( ptMcuToMcuMtStatusBeforeV4R5->IsVideoLose() );
				tSMcuMtStatus.SetSendVideo( ptMcuToMcuMtStatusBeforeV4R5->IsSendVideo() );
				tSMcuMtStatus.SetSendAudio( ptMcuToMcuMtStatusBeforeV4R5->IsSendAudio() );
				tSMcuMtStatus.SetRecvVideo( ptMcuToMcuMtStatusBeforeV4R5->IsRecvVideo() );
				tSMcuMtStatus.SetRecvAudio( ptMcuToMcuMtStatusBeforeV4R5->IsRecvAudio() );	
				//sgx20100703[Bug00033516]添加呼叫方式信息
                tSMcuMtStatus.SetIsAutoCallMode(ptMcuToMcuMtStatusBeforeV4R5->IsAutoCallMode());
			}
			cMsg.CatMsgBody( (u8 *)&tSMcuMtStatus, sizeof(tSMcuMtStatus) );
			ptMcuToMcuMtStatusBeforeV4R5 ++ ;
		}		
	}

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "ProcMcuMcuMtStatusNtf byMtId.%d nMtNum.%d\n", m_byMtId, byMtNum );
	return;
}

/*=============================================================================
  函 数 名： ProcMcuMcuMsgNtf
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
  10/08/13		4.6			xl						multicascade support
=============================================================================*/
void CMtAdpInst::ProcMcuMcuMsgNtf( const CServMsg& cServMsg, CServMsg& cMsg )
{
	TPartID tPartId;
	
	tPartId.m_dwPID = ntohl(*(u32 *)(cServMsg.GetMsgBody()));
	CRollMsg* ptROLLMSG = (CRollMsg*)( cServMsg.GetMsgBody() + sizeof(u32));
	
	TMt tMt = GetMtFromPartId(tPartId.m_dwPID, TRUE);
	
	StaticLog("[ProcMcuMcuMsgNtf] partId->tMt----mcuId: %d, mtId: %d\n", tMt.GetMcuId(), tMt.GetMtId());

	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );

	//compatible process
	TMsgHeadMsg tHeadMsg;
	BOOL32 bMultiCasVer = FALSE;
	if( cServMsg.GetMsgBodyLen() > (sizeof(u32) + ptROLLMSG->GetTotalMsgLen()) )
	{
		bMultiCasVer = TRUE;
		tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody() + sizeof(u32) + ptROLLMSG->GetTotalMsgLen() );
		++tHeadMsg.m_tMsgSrc.m_byCasLevel;
		if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
		{
			StaticLog("[ProcMcuMcuMsgNtf] tHeadMsg.m_tMsgDst.m_byCasLevel = %u, need --!\n", tHeadMsg.m_tMsgDst.m_byCasLevel);
			tMt.SetMcuId( tMt.GetMtId() );
			--tHeadMsg.m_tMsgDst.m_byCasLevel;
			tMt.SetMtId( tHeadMsg.m_tMsgDst.m_abyMtIdentify[tHeadMsg.m_tMsgDst.m_byCasLevel] );		
				
			StaticLog("[ProcMcuMcuMsgNtf] Rebuild TMT----mcuId: %d, mtId: %d\n", tMt.GetMcuId(), tMt.GetMtId());

		}
		
	}

	cMsg.SetMsgBody( (u8 *)&tMt, sizeof(tMt) );

	// 多国语言 [pengguofeng 4/8/2013]
	s8 achRollMsg[/*0x1000*/ MAX_ROLLMSG_LEN]; // 最大长度已经定好 [pengguofeng 4/27/2013]
	memset(achRollMsg, 0, sizeof(achRollMsg));
	// 收到消息发给MCU，最后一个参数应该填FALSE [pengguofeng 7/23/2013]
	BOOL32 bNeedTrans = TransEncoding((s8*)ptROLLMSG->GetRollMsgContent(), achRollMsg, sizeof(achRollMsg), FALSE);
	if ( bNeedTrans == TRUE )
	{
		// 		cMsg.CatMsgBody((u8*)achRollMsg, strlen(achRollMsg));
		ptROLLMSG->SetRollMsgContent((u8*)achRollMsg, strlen(achRollMsg));
	}
	else
	{
		MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMcuMsgNtf]Not need to trans Roll Msg\n");
	}
	
	MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMcuMsgNtf]Roll Msg final len:%d\n", ptROLLMSG->GetRollMsgContentLen());

	cMsg.CatMsgBody(ptROLLMSG->GetRollMsg(),ptROLLMSG->GetTotalMsgLen());
	if( bMultiCasVer )
	{
		cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
	}

	return;
}

/*=============================================================================
  函 数 名： SetPeerReqId
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u32 dwPeerReqId
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::SetPeerReqId(u32 dwPeerReqId)
{
	if(m_dwPeerReqId != 0)
	{
		MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[MTADAP]peer request Id is not empty(%d)\n", m_dwPeerReqId);
	}
	m_dwPeerReqId = dwPeerReqId;

	return;
}

/*=============================================================================
  函 数 名： GetPeerReqId
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： u32  
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
u32  CMtAdpInst::GetPeerReqId()
{
	u32 dwTmp = m_dwPeerReqId;
	m_dwPeerReqId = 0;

	return dwTmp;
}
 

/*=============================================================================
  函 数 名： ProcStdEvMcuMt
  功    能： 处理Mcu外发的标准消息
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcStdEvMcuMt(CMessage * const pcMsg)
{	
	switch( pcMsg->event )
	{	
	//------creating/droping conference-------
		
	case MCU_MT_CREATECONF_ACK:		
	case MCU_MT_CREATECONF_NACK:	
		ProcMcuCreateConfRsp( pcMsg );
		break;
	//-----joining/leaving conference -------

	case MCU_MT_INVITEMT_REQ:		
		ProcMcuMtInviteMtReq( pcMsg );
		break;
	case MCU_MT_DELMT_CMD:	//force MT to quit conference
		ProcMcuMtGeneralCmd( pcMsg );
		break;

	case MCU_MT_DELMT_NACK:
		ProcMcuMtGeneralRsp( pcMsg );
		break;		
		
	case MCU_MT_MTJOINED_NOTIF:
	case MCU_MT_MTLEFT_NOTIF:
		ProcMcuMtGeneralInd( pcMsg );
		break;
	//---------authentication --------

	case MCU_MT_ENTERPASSWORD_REQ:
		ProcMcuMtEnterPasswordReq(pcMsg);
		break;		
		
	//---- h239 token operations(h.239) -----
	case  MCU_MT_GETH239TOKEN_ACK:
	case  MCU_MT_GETH239TOKEN_NACK:
	case  MCU_MT_RELEASEH239TOKEN_CMD:
	case  MCU_MT_OWNH239TOKEN_NOTIF:
	
	case MCU_POLY_GETH239TOKEN_REQ:			//KEDAMCU与PolyMCU级联，获取PolyMCU的H239TOKEN请求
	case MCU_POLY_OWNH239TOKEN_NOTIF:		//KEDAMCU与PolyMCU级联，通知PolyMCU H239TOKEN为当前MCU所有
	case MCU_POLY_RELEASEH239TOKEN_CMD:		//KEDAMCU与PolyMCU级联，KEDAMCU停止双流的 命令

		ProcMcuMtH239TokenGeneralInd( pcMsg );
		break;

	//----logical channel operations--------

	case MCU_MT_OPENLOGICCHNNL_REQ:
		ProcMcuMtOpenLogicChannelReq(pcMsg);
		break;

	case MCU_MT_CLOSELOGICCHNNL_CMD:
		ProcMcuMtCloseLogicChannelCmd(pcMsg);
		break;

	case MCU_MT_OPENLOGICCHNNL_ACK:	
	case MCU_MT_OPENLOGICCHNNL_NACK:
		ProcMcuMtOpenLogicChannelRsp( pcMsg );
		break;

	case MCU_MT_FLOWCONTROL_CMD:
	case MCU_MT_FLOWCONTROLINDICATION_NOTIF:
	case MCU_MT_FREEZEPIC_CMD:			//freeze picture
	case MCU_MT_FASTUPDATEPIC_CMD:		//fast update picture
	case MCU_MT_CHANNELON_CMD:
	case MCU_MT_CHANNELOFF_CMD:
		ProcMcuMtGeneralCmd( pcMsg );
		break;
		
	// -----chairman related operations-------

	case MCU_MT_CANCELCHAIRMAN_NOTIF:
		ProcMcuMtGeneralInd( pcMsg );
		break;	

	case MCU_MT_APPLYCHAIRMAN_ACK:
	case MCU_MT_APPLYCHAIRMAN_NACK:
		ProcMcuMtGeneralRsp( pcMsg );
		break;

	case MCU_MT_CANCELCHAIRMAN_ACK:
	case MCU_MT_CANCELCHAIRMAN_NACK:
		ProcMcuMtGeneralRsp( pcMsg );
		break;

	case MCU_MT_GETCHAIRMAN_ACK:
	case MCU_MT_GETCHAIRMAN_NACK:
		ProcMcuMtGeneralRsp( pcMsg );
		break;

	//------speaker related operations----

	case MCU_MT_SPECSPEAKER_NOTIF:    
	case MCU_MT_CANCELSPEAKER_NOTIF:
	case MCU_MT_APPLYSPEAKER_NOTIF:
		ProcMcuMtGeneralInd( pcMsg );
		break;	

	case MCU_MT_SPECSPEAKER_ACK:
	case MCU_MT_SPECSPEAKER_NACK:
		ProcMcuMtGeneralRsp( pcMsg );
		break;

	//------selective watching related operations--------

	case MCU_MT_STARTSELMT_ACK:
	case MCU_MT_STARTSELMT_NACK:
		ProcMcuMtGeneralRsp(pcMsg);
		break;

	//--------miscellaneous operations-------------

	case MCU_MT_JOINEDMTLIST_ACK:	//terminalListResponse
	case MCU_MT_JOINEDMTLIST_NACK:	
	case MCU_MT_JOINEDMTLISTID_ACK: //allTerminalIDResponse
	case MCU_MT_JOINEDMTLISTID_NACK: 
		ProcMcuMtJoinedMtInfoRsp(pcMsg);
		break;

	case MCU_MT_CAPBILITYSET_NOTIF:
		ProcMcuMtGeneralInd(pcMsg);
		break;

	case MCU_MT_GETMTALIAS_ACK:
	case MCU_MT_GETMTALIAS_NACK:
		ProcMcuMtGeneralRsp(pcMsg);
		break;

	case MCU_MT_MEDIALOOPON_ACK:
	case MCU_MT_MEDIALOOPON_NACK:
        ProcMcuMtMediaLoopOnRsp(pcMsg);
		break;
	
	case MCU_MT_YOUARESEEING_NOTIF:
	case MCU_MT_SEENBYOTHER_NOTIF:
		ProcMcuMtGeneralInd( pcMsg );
		break;
		
	//------- camera control (h.224 & h.281) ----------
	case  MCU_MT_MTCAMERA_CTRL_CMD:
	case  MCU_MT_MTCAMERA_CTRL_STOP:
	case  MCU_MT_MTCAMERA_RCENABLE_CMD:
	case  MCU_MT_MTCAMERA_SAVETOPOS_CMD:
	case  MCU_MT_MTCAMERA_MOVETOPOS_CMD:
	case  MCU_MT_VIDEOSOURCESWITCHED_CMD:
	case  MCU_MT_SETMTVIDSRC_CMD:
		ProcMcuMtFeccCmd(pcMsg);
		break;
		
	//----------- ENTERPASSWORD  ----------
	case MT_MCU_ENTERPASSWORD_NACK: //currently undefined
		break;
		
	case MT_MCU_ENTERPASSWORD_ACK:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			s8 *pszPassword = (s8*)cServMsg.GetMsgBody();
			
			TTERLABEL tTer;
			tTer.SetTerminalLabel( (u8)g_cMtAdpApp.m_wMcuNetId, m_byMtId );
			
			TPassRsp tPassRsp;
			tPassRsp.SetTerLabel( tTer );
			tPassRsp.m_nPassLen = strlen(pszPassword);
			tPassRsp.SetPassword( (u8*)pszPassword, (u16)(tPassRsp.m_nPassLen) );
			
			kdvSendConfCtrlMsg(m_hsCall, (u16)h_ctrl_passwordResponse, (u8*)&tPassRsp, sizeof(tPassRsp));				
		}
		break;	

	//----------- MCU同步终端适配层该会议的会议密码，用于级联请求的认证校验，2005-11-02
	case MCU_MT_CONFPWD_NTF:
		{
			CServMsg cServMsg(pcMsg->content, pcMsg->length);
			s8 *pszPassword = (char*)cServMsg.GetMsgBody();
			strncpy(m_achConfPwd, pszPassword, (LEN_H243PWD-1));
			m_achConfPwd[LEN_H243PWD-1] = '\0';
			break;
		}		
	
    //适应对中兴远遥的特殊处理，申请主席
    case MCU_MT_APPLYCHAIRMAN_CMD:
        if ( (s32)act_ok != kdvSendConfCtrlMsg(m_hsCall, (u16)h_ctrl_makeMeChair, NULL, 0) )
        {
            MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "h_ctrl_makeMeChair send to mt<%d> failed !\n", m_byMtId );
        }
		else
		{
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "h_ctrl_makeMeChair send to mt<%d> success !\n", m_byMtId );
		}
        break;
		
		//取消主席申请
	case MCU_MT_CANCELCHAIRMAN_CMD:
		if ( (s32)act_ok != kdvSendConfCtrlMsg(m_hsCall, (u16)h_ctrl_cancelMakeMeChair, NULL, 0) )
        {
            MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "h_ctrl_cancelMakeMeChair send to mt<%d> failed !\n", m_byMtId );
        }
		else
		{
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "h_ctrl_cancelMakeMeChair send to mt<%d> success !\n", m_byMtId );
		}
		break;
	default:
		UNEXPECTED_MESSAGE(pcMsg->event);
		break;
	}

	return;
}

/*=============================================================================
  函 数 名： ProcVCMsgMcuMcu
  功    能： 处理业务透传给远端Mcu的消息
  算法实现： 业务消息直接透传，走级联非标消息
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2010/09/06	4.6			xueliang				create
=============================================================================*/
void CMtAdpInst::ProcVCMsgMcuMcu(CMessage * const pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	HCHAN hsChan = GetMMcuChannel();
	if( hsChan  == 0 )
	{
		return;
	}
	
	TNonStandardReq tReq;
	tReq.m_aszUserName[0] = 0;
	tReq.m_aszUserPass[0] = 0;
	tReq.m_nReqID = m_dwMMcuReqId++;
	
	tReq.m_nMsgLen = cServMsg.GetServMsgLen();
	if((u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][EXP]ProcVCMsgMcuMcu message is too large(len:%d)\n", tReq.m_nMsgLen);
		return;
	}
	
	memcpy(tReq.m_abyMsgBuf, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq));

	return;

}
/*=============================================================================
  函 数 名： ProcEvInterMcu
  功    能： 处理从MCU到MMCU的消息
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcEvInterMcu( CMessage * const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	HCHAN hsChan = GetMMcuChannel();
	if( hsChan  == 0 )
	{
		return;
	}

	TRsp tRsp;
	TReq tReq;
	
	/*lint -save -e578*/
	switch(pcMsg->event)
	{		
	//3.6版本合并级连消息处理
	case MCU_MCU_ROSTER_NOTIF:
		{
			OnSendRosterNotify( cServMsg, hsChan );

			TRegUnRegRsp tRsp;
			tRsp.m_nReqID = GetPeerReqId();
			tRsp.m_emReturnVal = emReturnValue_Ok;
			tRsp.m_bReg = TRUE;
			kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_REGUNREG_RSP, &tRsp, sizeof(tRsp) );

			break;
		}

	case MCU_MCU_MTLIST_REQ:
		{	
			TMcuMcuReq *ptReq = (TMcuMcuReq *)cServMsg.GetMsgBody();

			tReq.m_nReqID = m_dwMMcuReqId ++ ;
			astrncpy(tReq.m_aszUserName, ptReq->m_szUserName, 
				sizeof(tReq.m_aszUserName), sizeof(ptReq->m_szUserName));
			astrncpy(tReq.m_aszUserPass, ptReq->m_szUserPwd, 
				sizeof(tReq.m_aszUserPass), sizeof(ptReq->m_szUserPwd));
			s32 nRet = kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_PARTLIST_REQ, &tReq, sizeof(tReq));
            
			MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "Send H_CASCADE_PARTLIST_REQ to Cascadelib. mtid = %d, nRet = %d\n",
                                     m_byMtId, nRet);
            g_cMtAdpApp.m_tMaStaticInfo.m_dwSndMtListReqNum++;
			m_bySMcuMtListBufTimerStart = 0;
			KillTimer(TIMER_SEND_SMCU_MTLIST);
			m_cSMcuMtListSendBuf.ClearAll();
			break;
		}

	case MCU_MCU_MTLIST_ACK:
		{
			OnSendMtListAck( cServMsg, hsChan );
			break;
		}

	case MCU_MCU_SPEAKSTATUS_NTF:
		{
			OnSendSpeakStatusNotify( cServMsg, hsChan );
			break;
		}

	case MCU_MCU_SPEAKERMODE_NOTIFY:
		{
			OnSendMcuMcuSpeakModeNotify( cServMsg, hsChan );
			break;
		}
		
	case MCU_MCU_CANCELMESPEAKER_REQ:
		{
			OnSendCancelMeSpeakerReq( cServMsg, hsChan );
			break;
		}
	case MCU_MCU_CANCELMESPEAKER_ACK:
		{
			OnSendCancelMeSpeakerAck( cServMsg, hsChan );
			break;
		}
	case MCU_MCU_CANCELMESPEAKER_NACK:
		{
			OnSendCancelMeSpeakerNack( cServMsg, hsChan );
			break;
		}

	case MCU_MCU_NEWMT_NOTIF:
		{
			OnSendNewMtNotify(cServMsg, hsChan);
			break;
		}


	case MCU_MCU_SETOUT_REQ:
		{
			OnSendSetOutReq(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_SETOUT_ACK:
		{
			tRsp.m_emReturnVal = emReturnValue_Ok;
			tRsp.m_nReqID = GetPeerReqId();

			kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_SETOUT_RSP, &tRsp, sizeof(tRsp) );
			break;
		}

	case MCU_MCU_SETOUT_NACK:
		{
			tRsp.m_emReturnVal = emReturnValue_Error;

			kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_SETOUT_RSP, &tRsp, sizeof(tRsp) );
			break;
		}
		
	case MCU_MCU_VIDEOINFO_REQ:
		{
			TMcuMcuReq *ptReq = (TMcuMcuReq *)cServMsg.GetMsgBody();
			TConfVideInfoReq tVidReq;
			tVidReq.m_nReqID = m_dwMMcuReqId++;
			// 多国语言 [pengguofeng 4/8/2013]
			s8 achUserName[CASCADE_USERNAME_MAXLEN];
			memset(achUserName, 0, sizeof(achUserName));
			BOOL32 bNeedTrans = TransEncoding(ptReq->m_szUserName, achUserName, sizeof(achUserName));
			if ( bNeedTrans == TRUE)
			{
				astrncpy(tVidReq.m_aszUserName, achUserName, 
					sizeof(tVidReq.m_aszUserName), sizeof(achUserName));
			}
			else
			{
				astrncpy(tVidReq.m_aszUserName, ptReq->m_szUserName, 
					sizeof(tVidReq.m_aszUserName), sizeof(ptReq->m_szUserName));
			}
			astrncpy(tVidReq.m_aszUserPass, ptReq->m_szUserPwd, 
				sizeof(tVidReq.m_aszUserPass), sizeof(ptReq->m_szUserPwd));
			kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_VIDEOINFO_REQ, &tVidReq, sizeof(tVidReq));
            g_cMtAdpApp.m_tMaStaticInfo.m_dwSndVidInfoReqNum++;
			break;
		}

	case MCU_MCU_VIDEOINFO_ACK:
		{
			OnSendVideoInfoAck( cServMsg, hsChan );
			break;
		}
	case MCU_MCU_AUDIOINFO_REQ:
		{
			TMcuMcuReq *ptReq = (TMcuMcuReq *)cServMsg.GetMsgBody();
			
			tReq.m_nReqID = m_dwMMcuReqId ++ ;
			// 多国语言 [pengguofeng 4/8/2013]
			s8 achUserName[CASCADE_USERNAME_MAXLEN];
			memset(achUserName, 0, sizeof(achUserName));
			BOOL32 bNeedTrans = TransEncoding(ptReq->m_szUserName, achUserName, sizeof(achUserName));
			if ( bNeedTrans == TRUE)
			{
				astrncpy(tReq.m_aszUserName, achUserName, 
					sizeof(tReq.m_aszUserName), sizeof(achUserName));
			}
			else
			{
				astrncpy(tReq.m_aszUserName, ptReq->m_szUserName, 
					sizeof(tReq.m_aszUserName), sizeof(ptReq->m_szUserName));
			}
			astrncpy(tReq.m_aszUserPass, ptReq->m_szUserPwd, 
				sizeof(tReq.m_aszUserPass), sizeof(ptReq->m_szUserPwd));

			kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_AUDIOINFO_REQ, &tReq, sizeof(tReq) );
            g_cMtAdpApp.m_tMaStaticInfo.m_dwSndAudInfoReqNum++;
			break;
		}
		
	case MCU_MCU_AUDIOINFO_ACK:
		{
			OnSendAudioInfoAck( cServMsg, hsChan );
			break;
		}

	 case MCU_MCU_AUTOSWITCH_REQ:
        {
            OnSendAutoSwitchReq(cServMsg, hsChan);
            break;
        }

    case MCU_MCU_AUTOSWITCH_ACK:
        {   
            tRsp.m_emReturnVal = emReturnValue_Ok;
            tRsp.m_nReqID = GetPeerReqId();
            
            kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_SET_LAYOUT_AUTOSWITCH_RESPONSE, &tRsp, sizeof(tRsp) );

            break;
        }
    case MCU_MCU_AUTOSWITCH_NACK:
        {
            tRsp.m_emReturnVal = emReturnValue_Error;
            tRsp.m_nReqID = GetPeerReqId();
            
            kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_SET_LAYOUT_AUTOSWITCH_RESPONSE, &tRsp, sizeof(tRsp) );
            break;
        }

	case MCU_MCU_SETMTCHAN_REQ:
		{
			OnSendSetMtChanReq(cServMsg, hsChan);
            break;
		}

	case MCU_MCU_SETMTCHAN_ACK:
		{
			tRsp.m_emReturnVal = emReturnValue_Ok;
			tRsp.m_nReqID = GetPeerReqId();

			kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_PARTMEDIACHAN_RSP, &tRsp, sizeof(tRsp));
			break;
		}

	case MCU_MCU_SETMTCHAN_NACK:
		{
			tRsp.m_emReturnVal = emReturnValue_Error;
			tRsp.m_nReqID = GetPeerReqId();

			kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_PARTMEDIACHAN_RSP, &tRsp, sizeof(tRsp));
			break;
		}

	//  pengjie[9/29/2009] 级联多回传消息处理
	case MCU_MCU_MULTSPYCAP_NOTIF:
		{
			OnSendMultSpyCapNotif( cServMsg, hsChan );
			break;
		}

	case MCU_MCU_PRESETIN_REQ:
		{
			OnSendPreSetinReq( cServMsg, hsChan );
			break;
		}

	case MCU_MCU_SPYFASTUPDATEPIC_CMD:
		{
			OnSendSpyFastUpdateCmd( cServMsg, hsChan );
			break;
		}

	case MCU_MCU_PRESETIN_ACK:
		{
			OnSendPreSetinAck( cServMsg, hsChan );
			break;
		}

	case MCU_MCU_PRESETIN_NACK:
		{
			OnSendPreSetinNack( cServMsg, hsChan );
			break;
		}

	case MCU_MCU_SPYCHNNL_NOTIF:
		{
			OnSendSpyChnnlNotif( cServMsg, hsChan );
			break;
		}
	case MCU_MCU_SWITCHTOMCUFAIL_NOTIF:
		{
			OnSendSwitchToMMcuFailNotif(cServMsg, hsChan);
			break;
		}
	case MCU_MCU_MTEXTINFO_NOTIF:
		{
			OnSendMtExtInfoNotify( cServMsg, hsChan );
			break;
		}

	case MCU_MCU_REJECTSPY_NOTIF:
		{
			OnSendRejectSpyNotif( cServMsg, hsChan );
			break;
		}

	case MCU_MCU_BANISHSPY_CMD:
		{
			OnSendBanishSpyCmd( cServMsg, hsChan );
			break;
		}

		//lukunpeng 2010/06/10 由于现在是由上级管理下级带宽占用情况，故不需要再通过下级通知上级
		/*
	case MCU_MCU_BANISHSPY_NOTIF:
		{
			OnSendBanishSpyNotif( cServMsg, hsChan );
			break;
		}
		*/
	case MCU_MCU_INVITEMT_REQ:
	case MCU_MCU_REINVITEMT_REQ:
	case MCU_MCU_DROPMT_REQ:
	case MCU_MCU_DELMT_REQ:
	case MCU_MCU_SETIN_REQ:	
		{
			cServMsg.SetEventId( pcMsg->event );
			OnSendMultiCascadeReqMsg(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_INVITEMT_ACK:
	case MCU_MCU_INVITEMT_NACK:
	case MCU_MCU_REINVITEMT_ACK:
	case MCU_MCU_REINVITEMT_NACK:
	case MCU_MCU_DROPMT_ACK:
	case MCU_MCU_DROPMT_NACK:
	case MCU_MCU_DELMT_ACK:
	case MCU_MCU_DELMT_NACK:
	case MCU_MCU_SETIN_ACK:
	case MCU_MCU_SETIN_NACK:
		{
			cServMsg.SetEventId( pcMsg->event );
			OnSendMultiCascadeRspMsg(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_CALLALERTING_NOTIF:
	case MCU_MCU_DROPMT_NOTIF:
	case MCU_MCU_DELMT_NOTIF:
	case MCU_MCU_SETOUT_NOTIF:
		{
			cServMsg.SetEventId( pcMsg->event );
			OnSendMultiCascadeNtfMsg(cServMsg, hsChan);
			break;
		}
	/*case MCU_MCU_INVITEMT_REQ:
		{
			OnSendInviteMtReq(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_INVITEMT_ACK:
		{
			tRsp.m_emReturnVal = emReturnValue_Ok;
			tRsp.m_nReqID      = GetPeerReqId();

			kdvSendMMcuCtrlMsg( hsChan, H_CASCADE_INVITEPART_RSP, &tRsp, sizeof(tRsp) );
			break;
		}

	case MCU_MCU_INVITEMT_NACK:
		{
			tRsp.m_emReturnVal = emReturnValue_Error;
			tRsp.m_nReqID      = GetPeerReqId();

			kdvSendMMcuCtrlMsg( hsChan, H_CASCADE_INVITEPART_RSP, &tRsp, sizeof(tRsp) );
			break;
		}*/

	/*case MCU_MCU_REINVITEMT_REQ:
		{
			OnSendReInviteMtReq(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_REINVITEMT_ACK:
		{
			tRsp.m_emReturnVal = emReturnValue_Ok;
			tRsp.m_nReqID      = GetPeerReqId();

			kdvSendMMcuCtrlMsg( hsChan, H_CASCADE_REINVITEPART_RSP, &tRsp, sizeof(tRsp) );
			break;
		}

	case MCU_MCU_REINVITEMT_NACK:
		{
			tRsp.m_emReturnVal = emReturnValue_Error;
			tRsp.m_nReqID      = GetPeerReqId();

			kdvSendMMcuCtrlMsg(hsChan, H_CASCADE_REINVITEPART_RSP, &tRsp, sizeof(tRsp));
			break;
		}

	case MCU_MCU_CALLALERTING_NOTIF:
		{
			OnSendCallAlertMtNotify(cServMsg, hsChan);
			break;
		}*/
		
		//挂断、删除终端部分
	/*case MCU_MCU_DROPMT_REQ:
		{
			OnSendDropMtReq(cServMsg, hsChan);
			break;
		}
		

	case MCU_MCU_DROPMT_ACK:
		{
			tRsp.m_emReturnVal = emReturnValue_Ok;
			tRsp.m_nReqID      = GetPeerReqId();

			kdvSendMMcuCtrlMsg(hsChan, H_CASCADE_DISCONNPART_RSP, &tRsp, sizeof(tRsp));
			break;
		}

	case MCU_MCU_DROPMT_NACK:
		{
			tRsp.m_emReturnVal = emReturnValue_Error;
			tRsp.m_nReqID      = GetPeerReqId();
			
			kdvSendMMcuCtrlMsg(hsChan, H_CASCADE_DISCONNPART_RSP, &tRsp, sizeof(tRsp));
			break;
		}

	case MCU_MCU_DROPMT_NOTIF:
		{
			TMt *ptMt = (TMt* )cServMsg.GetMsgBody();
			 
			TDiscPartNF tNF;
			tNF.m_dwPID = GetPartIdFromMt(*ptMt, &(tNF.m_bLocal));
			tNF.m_dwNewPID = tNF.m_dwPID;

			kdvSendMMcuCtrlMsg(hsChan, H_CASCADE_DISCONNPART_NTF, &tNF, sizeof(tNF));
			
			break;
		}*/

	/*case MCU_MCU_DELMT_REQ:
		{
			OnSendDelMtReq(cServMsg, hsChan);
			break;
		}
		

	case MCU_MCU_DELMT_ACK:
		{
			tRsp.m_emReturnVal = emReturnValue_Ok;
			tRsp.m_nReqID = GetPeerReqId();

			kdvSendMMcuCtrlMsg( hsChan, H_CASCADE_DELETEPART_RSP, &tRsp, sizeof(tRsp) );
			break;
		}
		
	case MCU_MCU_DELMT_NACK:
		{
			tRsp.m_emReturnVal = emReturnValue_Error;
			tRsp.m_nReqID = GetPeerReqId();

			kdvSendMMcuCtrlMsg( hsChan, H_CASCADE_DELETEPART_RSP, &tRsp, sizeof(tRsp) );
			break;
		}

	case MCU_MCU_DELMT_NOTIF:
		{
			TMt *ptMt = (TMt* )cServMsg.GetMsgBody();

			TPartID tPartId;
			tPartId.m_dwPID = GetPartIdFromMt(*ptMt, &(tPartId.m_bLocal));

			kdvSendMMcuCtrlMsg( hsChan, H_CASCADE_DELETEPART_NTF, &tPartId, sizeof(tPartId) );

			
		
			break;
		}*/

	case MCU_MCU_CONFVIEWCHG_NOTIF:
		{
			TCConfViewChangeNtf *ptNF = (TCConfViewChangeNtf* )cServMsg.GetMsgBody();

			TConfViewChgNtf tOutNF;
			tOutNF.m_nViewID = ptNF->m_nViewID;
			tOutNF.m_dwSubFrameCount = ptNF->m_bySubframeCount;

			kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_CONFVIEW_CHG_NTF, &tOutNF, sizeof(tOutNF));
			break;
		}

	

	case MCU_MCU_SETMTCHAN_NOTIF:
		{
			TMtMediaChanStatus *ptStatus = (TMtMediaChanStatus *)cServMsg.GetMsgBody();

			TPartMediaChan tChan;
			tChan.m_dwPID           = GetPartIdFromMt( ptStatus->m_tMt, &(tChan.m_bLocal) );
			tChan.m_emMediaType     = CMtAdpUtils::MediaModeIn2Out( ptStatus->m_byMediaMode );
			tChan.m_emMuteDirection = ISTRUE( ptStatus->m_byIsDirectionIn ) ? emDirectionIn:emDirectionOut;
			tChan.m_bMute           = ptStatus->m_bMute;

			kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_PARTMEDIACHAN_NTF, &tChan, sizeof(tChan) );
			break;
		}

	/*case MCU_MCU_SETIN_REQ:
		{	
			OnSendSetInReq( cServMsg, hsChan );
			break;
		}
		
		
	case MCU_MCU_SETIN_ACK:
		{
			tRsp.m_emReturnVal = emReturnValue_Ok;
			tRsp.m_nReqID = GetPeerReqId();

			kdvSendMMcuCtrlMsg( hsChan, H_CASCADE_SETIN_RSP, &tRsp, sizeof(tRsp) );
			break;
		}

	case MCU_MCU_SETIN_NACK:
		{
			tRsp.m_emReturnVal = emReturnValue_Error;

			kdvSendMMcuCtrlMsg( hsChan, H_CASCADE_SETIN_RSP, &tRsp, sizeof(tRsp) );
			break;
		}

	

	case MCU_MCU_SETOUT_NOTIF:
		{
			OnSendSetOutNotify(cServMsg, hsChan);
			break;
		}*/

   

		//摄像头远遥信令
	case MCU_MCU_FECC_CMD:
		{
			OnSendMMcuFeccCmd( cServMsg, hsChan );
			break;
		}

	/********************************
	/***   nonstandard message    ***
	/********************************/
	case MCU_MCU_STARTMIXER_CMD:
		{
			cServMsg.SetEventId( pcMsg->event );
			OnSendStartMixerCmd( cServMsg, hsChan );
			break;
		}
	case MCU_MCU_STARTMIXER_NOTIF:
		{
			cServMsg.SetEventId( pcMsg->event );
			OnSendStartMixerNotif( cServMsg, hsChan );
			break;
		}

	case MCU_MCU_STOPMIXER_CMD:
		{
			cServMsg.SetEventId( pcMsg->event );
			OnSendStopMixerCmd( cServMsg, hsChan );
			break;
		}
	case MCU_MCU_STOPMIXER_NOTIF:
		{
			cServMsg.SetEventId( pcMsg->event );
			OnSendStopMixerNotif(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_GETMIXERPARAM_REQ:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendGetMixerParamReq(cServMsg, hsChan);
			break;
		}
	case MCU_MCU_MTAUDMUTE_REQ:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendAudMuteReq(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_ADJMTRES_REQ:
		{
			//对端MCU是不支持级联调分辨率的版本(R5,daily 4.6以前的版本)，直接回Ack给本端MCU
			s8* pszPeerVerId = GetPeerVersionID( m_hsCall);
			s32 nPeerId = atoi( pszPeerVerId );
			if (nPeerId > 6 || nPeerId == 6)
			{
				OnSendAdjustMtResReq(cServMsg, hsChan);
			}
			else
			{
				CServMsg cMsg = cServMsg;//老版本兼容，在原有消息基础上回复，保证信息不丢失
				cMsg.SetEventId(MCU_MCU_ADJMTRES_ACK);
				TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg *)cServMsg.GetMsgBody();

				// 2011-9-13 add by pgf: 返回给MMCU，需要把HeadMsg的内容调换一下
				TMsgHeadMsg tHeadMsg2MMcu;
				tHeadMsg2MMcu.m_tMsgSrc = tHeadMsg.m_tMsgDst;
				tHeadMsg2MMcu.m_tMsgDst = tHeadMsg.m_tMsgSrc;

				TMt tMt = *(TMt *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
				u8  byStart = *(u8 *)(cServMsg.GetMsgBody()+sizeof(TMt)+ sizeof(TMsgHeadMsg));
				//zhouyiliang 20101124对老版本级联直接回ack的时候还要加个byPos的保护
				u8  byPos = ~0;
				if ( cServMsg.GetMsgBodyLen() > sizeof(TMt) + sizeof(u8) + sizeof(u8) + sizeof(u8)  +sizeof(TMsgHeadMsg) ) 
				{
					byPos = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg) + sizeof(u8) * 3);
				}
				cMsg.SetMsgBody((u8 *)&tHeadMsg2MMcu/*tHeadMsg*/, sizeof(TMsgHeadMsg));
				cMsg.CatMsgBody((u8 *)&tMt, sizeof(TMt));
				cMsg.CatMsgBody((u8 *)&byStart, sizeof(byStart));
				cMsg.CatMsgBody((u8 *)&byPos, sizeof(byPos));

				SendMsgToMcu(cMsg);
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "The remote MCU doesn't support cascade adjusting resolution, so send MCU_MCU_ADJMTRES_ACK\
					to local mcu. Mt id: %u, byStart: %u, byPos：%u", tMt.GetMtId(), byStart, byPos );
			}
			break;
		}
	case MCU_MCU_ADJMTRES_ACK:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendAdjustMtResAck(cServMsg, hsChan);
			break;
		}
	case MCU_MCU_ADJMTRES_NACK:
		{	
			cServMsg.SetEventId(pcMsg->event);
			OnSendAdjustMtResNack(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_ADJMTBITRATE_CMD:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendAdjustMtBitrateCmd( cServMsg, hsChan);
			break;
		}

		// [pengjie 2010/4/23] 级联将帧率支持
	case MCU_MCU_ADJMTFPS_REQ:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendAdjustMtFpsReq(cServMsg, hsChan);
			break;
		}
	case MCU_MCU_ADJMTFPS_ACK:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendAdjustMtFpsAck(cServMsg, hsChan);
			break;
		}
	case MCU_MCU_ADJMTFPS_NACK:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendAdjustMtFpsNack(cServMsg, hsChan);
			break;
		}
		// End

	case MCU_MCU_GETMIXERPARAM_ACK:
		{
			cServMsg.SetEventId(pcMsg->event);
			//OnSendGetMixerParamAck(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_GETMIXERPARAM_NACK:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendGetMixerParamNack(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_MIXERPARAM_NOTIF:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendMixerParamNotif(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_ADDMIXMEMBER_CMD:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendAddMixerMemberCmd(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_REMOVEMIXMEMBER_CMD:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendRemoveMixerMemberCmd(cServMsg, hsChan);
			break;
		}

		
    case MCU_MCU_APPLYSPEAKER_REQ:
        {
            cServMsg.SetEventId(pcMsg->event);
            OnSendApplySpeakerReq(cServMsg, hsChan);
            break;
        }
		
    case MCU_MCU_APPLYSPEAKER_ACK:
        {
            cServMsg.SetEventId(pcMsg->event);
            OnSendApplySpeakerAck(cServMsg, hsChan);
            break;
        }
		
    case MCU_MCU_APPLYSPEAKER_NACK:
        {
            cServMsg.SetEventId(pcMsg->event);
            OnSendApplySpeakerNack(cServMsg, hsChan);
            break;
        }
		
    /*case MCU_MCU_APPLYSPEAKER_NOTIF:
        {
            cServMsg.SetEventId(pcMsg->event);
            OnSendApplySpeakerNotif(cServMsg, hsChan);
            break;
        }*/

	case MCU_MCU_LOCK_REQ:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendLockMcuReq(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_LOCK_ACK:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendLockMcuAck(cServMsg, hsChan);
			break;
        }		
        
    case MCU_MCU_LOCK_NACK:
        {
            cServMsg.SetEventId(pcMsg->event);
			OnSendLockMcuNack(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_MTSTATUS_CMD:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendMtStatusCmd(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_MTSTATUS_NOTIF:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendMtStatusNtf(cServMsg, hsChan);
			break;
		}

	case MCU_MCU_SENDMSG_NOTIF:
		{
			cServMsg.SetEventId(pcMsg->event);
			OnSendMsgNtf(cServMsg, hsChan);
			break;
		}
    //[5/4/2011 zhushengze]VCS控制发言人发双流
    case MCU_MCU_CHANGEMTSECVIDSEND_CMD:
        {
            cServMsg.SetEventId(pcMsg->event);
            OnSendMsgChangeMtSecVidSendCmd(cServMsg, hsChan);
			break;
        }

		//[2/23/2012 zhushengze]界面、终端消息透传
    case MCU_MCU_TRANSPARENTMSG_NOTIFY:
        {
            cServMsg.SetEventId(pcMsg->event);
            OnSendMsgTransparentMsgNtf(cServMsg, hsChan);
            break;
        }

	case MCU_MTADP_GETMTLIST_CMD: // MCU请求MTLIST [pengguofeng 6/7/2013]
		{
			u8 byMMcuReq = *(u8 *)cServMsg.GetMsgBody();
			u8 bySMcuNum = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));
			if ( bySMcuNum == 0xFF )
			{
				LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "Mcu request All sub mcu byMMcuReq:%d\n", byMMcuReq);
				ProcMcuMtadpRequestMtList(byMMcuReq);
			}
			else
			{
				u8 bySMcuId = 0;
				u8 bySMcuMtId = 0;
				while(bySMcuId < bySMcuNum)
				{
					bySMcuMtId = *(u8 *)(cServMsg.GetMsgBody() + (bySMcuId + 2) * sizeof(u8));
					LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "Mcu request sub mcu.%d byMMcuReq.%d\n", bySMcuMtId, byMMcuReq);
					ProcMcuMtadpRequestMtList(byMMcuReq, bySMcuMtId);
					bySMcuId++;
				}
			}
		}
		break;
		
	case MCU_MTADP_GETSMCUMTALIAS_CMD: // MCU请求指定下级终端别名信息 [yanghuaizhi 8/7/2013]
		{
            cServMsg.SetEventId(pcMsg->event);
			ProcMcuMtadpRequestMtAlias(cServMsg);
		}
		break;

	default:
		UNEXPECTED_MESSAGE( pcMsg->event );
		break;
	}
	/*lint -restore*/
}

/*====================================================================
    函数名      ：ProcMcuMtadpRequestMtList
    功能        ：处理MCU发过来的CMD，并发NOTIF回应之
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 bMMcuReq, 是否上报MMCU
				　u8 byMcuMtId 某MCU的MtId
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013/6/27   4.7.2       彭国锋          创建
====================================================================*/
void CMtAdpInst::ProcMcuMtadpRequestMtList(u8 bMMcuReq,
										   u8 byMcuMtId /* = 0xFF */)
{
	TMcuList *ptMcuList = g_cMtAdpApp.m_tAllSMcuList.GetMcuList((u8) GetInsID());
	if ( !ptMcuList )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Mcu.%d has no mcu list\n", byMcuMtId);
		return;
	}

	u8 abyBuf[10*1024];
	u16 dwBufLen = 0;
	u8 byMtNum = 0;
	TMtList *ptMtList = NULL;

	CServMsg cServMsg;
	cServMsg.SetEventId(MTADP_MCU_GETMTLIST_NOTIF);

	LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMtadpRequestMtList] %s(%d) to Mcu.%d\n",
		OspEventDesc(cServMsg.GetEventId()), cServMsg.GetEventId(), byMcuMtId );

	if ( byMcuMtId != 0xFF ) //指定某个SMCU的McuId
	{
		ptMtList = ptMcuList->GetMtList(byMcuMtId);
		if ( !ptMtList )
		{
			LogPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Mcu.%d has no mtlist\n", byMcuMtId);
			return;
		}

		cServMsg.SetMsgBody(&bMMcuReq, sizeof(u8));
		cServMsg.CatMsgBody(&byMcuMtId, sizeof(u8));

		byMtNum = ptMtList->GetTotalMtNum();
		cServMsg.CatMsgBody(&byMtNum, sizeof(u8));

		memset(abyBuf, 0, sizeof(abyBuf));
		ptMtList->MakeMtName2Buf(abyBuf, dwBufLen);
		//for test
// 		ptMtList->Print();
		LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMtadpRequestMtList]Pack McuLen:%d mtNum:%d\n", dwBufLen, byMtNum);
		cServMsg.CatMsgBody((u8*)abyBuf, dwBufLen);

		SendMsgToMcu(cServMsg);
	}
	else  //要求发所有的
	{
		u8 byMtListIdx = 0xFF;
		s32 dwCSerMsgLen = s32(SERV_MSG_LEN-SERV_MSGHEAD_LEN); //消息的最大长度
		BOOL32 bFirst = TRUE; //是否第一个消息
		BOOL32 bNeedSend = FALSE; //是否需要发送消息，如果所有的McuId都没有内容，则不必发

		//此处从0开始,因为0是他自己
		u32 dwSMcuMsgLen = 0;
		u8 byMtNum = 0;
		for ( u8 byMcuId = 0;byMcuId <= MAXNUM_CONF_MT; byMcuId++)
		{
// 			if ( ptMcuList->IsValidMcu(byMcuId, byMtListIdx)) //重复循环了，没必要
			{
				ptMtList = ptMcuList->GetMtList(byMcuId);
				if ( !ptMtList )
				{
 					LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "Mcu.%d has no mtlist, just continue\n", byMcuId);
					continue;
				}

				if ( !bNeedSend )
				{
					bNeedSend = TRUE;
				}

				dwSMcuMsgLen = ptMtList->GetTotalMsgLen();
				LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMtadpRequestMtList]McuId.%d TotalMsgLen:%d\n",
					byMcuId, dwSMcuMsgLen);
				if ( dwCSerMsgLen > 0
					&& dwCSerMsgLen > (s32)dwSMcuMsgLen + sizeof(u8)*3 ) //放下这一整个MCU也有余，就放到一起
				{
					//足够放下此MtList的长度，拼接
					if ( bFirst )
					{
						cServMsg.SetMsgBody(&bMMcuReq, sizeof(u8));
						bFirst = FALSE;
					}
					else
					{
						cServMsg.CatMsgBody(&bMMcuReq, sizeof(u8));
					}

					cServMsg.CatMsgBody(&byMcuId, sizeof(u8));
					byMtNum = ptMtList->GetTotalMtNum();
					cServMsg.CatMsgBody(&byMtNum, sizeof(u8));
					memset(abyBuf, 0, sizeof(abyBuf));
					ptMtList->MakeMtName2Buf(abyBuf, dwBufLen);
					LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMtadpRequestMtList]Pack McuLen:%d of Mt.%d\n", dwBufLen, byMcuId);
					cServMsg.CatMsgBody((u8*)abyBuf, dwBufLen);
					dwCSerMsgLen -= (dwSMcuMsgLen + sizeof(u8)*3);
				}
				else
				{
					//不够放了，已经保存的Msg先发送
					SendMsgToMcu(cServMsg);
/*
					bFirst = TRUE; // 这里会重来 [pengguofeng 7/1/2013]
					dwCSerMsgLen = u32(SERV_MSG_LEN-SERV_MSGHEAD_LEN);
*/
					// 当前的加到包里 [pengguofeng 7/15/2013]
					cServMsg.SetMsgBody(&bMMcuReq, sizeof(u8));
					cServMsg.CatMsgBody(&byMcuId, sizeof(u8));
					byMtNum = ptMtList->GetTotalMtNum();
					cServMsg.CatMsgBody(&byMtNum, sizeof(u8));
					memset(abyBuf, 0, sizeof(abyBuf));
					ptMtList->MakeMtName2Buf(abyBuf, dwBufLen);
					LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMtadpRequestMtList]Pack McuLen:%d of Mt.%d\n", dwBufLen, byMcuId);
					cServMsg.CatMsgBody((u8*)abyBuf, dwBufLen);
					dwCSerMsgLen = SERV_MSG_LEN - SERV_MSGHEAD_LEN - (dwSMcuMsgLen + sizeof(u8)*3);
				}
			}
		}

		// 最后如果还有打包到cServMsg的内容，则发出去 [pengguofeng 7/1/2013]
		if ( bNeedSend && cServMsg.GetMsgBodyLen() > 0)
		{
			SendMsgToMcu(cServMsg);
		}
	}
	return;
}

/*====================================================================
    函数名      ：ProcMcuMtadpRequestMtAlias
    功能        ：处理MCU发过来的CMD，并发NOTIF回应之
    算法实现    ：
    引用全局变量：
    输入参数说明：const CServMsg &cServMsg
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013/8/7   4.7.2     yanghuaizhi        创建
====================================================================*/
void CMtAdpInst::ProcMcuMtadpRequestMtAlias(const CServMsg &cServMsg)
{
	// 消息内容：byEqpid + byMcuId + byMtNum + byMtNum*[ byMtId + byChlNum + byChlNum*[byChlIdx]]
	u8 *pbyBuf = (u8 *)cServMsg.GetMsgBody();
	u8 byEqpid = *pbyBuf;// byEqpid
	pbyBuf++;
	u8 byMcuMtId = *pbyBuf;
	pbyBuf++;
	u8 byMtNum = *pbyBuf;
	pbyBuf++;
	
	TMcuList *ptMcuList = g_cMtAdpApp.m_tAllSMcuList.GetMcuList((u8) GetInsID());
	if ( !ptMcuList )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Mcu.%d has no mcu list\n", byMcuMtId);
		return;
	}

	TMtList *ptMtList = ptMcuList->GetMtList(byMcuMtId);
	if ( !ptMtList )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Mcu.%d has no mtlist\n", byMcuMtId);
		return;
	}

	CServMsg cNotfServMsg;
	cNotfServMsg.SetEventId(MTADP_MCU_GETSMCUMTALIAS_NOTIF);
	// 消息内容：byEqpid + byMcuId + byMtNum + byMtNum*[ byMtId + byChlNum + byChlNum*[byChlIdx] + byNameLen + pName]
	cNotfServMsg.SetMsgBody(&byEqpid, sizeof(u8));
	cNotfServMsg.CatMsgBody(&byMcuMtId, sizeof(u8));
	cNotfServMsg.CatMsgBody(&byMtNum, sizeof(u8));
	
	LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMtadpRequestMtAlias] %s(%d) to Mcu.%d, eqpid.%d, MtNum.%d\n",
		OspEventDesc(cNotfServMsg.GetEventId()), cNotfServMsg.GetEventId(), byMcuMtId, byEqpid, byMtNum);
	
	u8 byMtId;
	u8 byChlNum;
	s8 achMtName[STR_LEN/*_UTF8*/];
	memset(achMtName, 0, sizeof(achMtName));
	u8 byNameLen = 0;
	for (u8 byIdx=0; byIdx<byMtNum;byIdx++)
	{
		memset(achMtName, 0, sizeof(achMtName));
		//获得Mtid
		byMtId = *pbyBuf;
		pbyBuf++;
		byChlNum = *pbyBuf;
		pbyBuf++;
		// 获得该终端别名
		if (ptMtList->GetMtName(byMtId))
		{
			memcpy(achMtName, ptMtList->GetMtName(byMtId), STR_LEN);
			byNameLen = strlen(achMtName);
			cNotfServMsg.CatMsgBody(&byMtId, sizeof(u8));
			cNotfServMsg.CatMsgBody(&byChlNum, sizeof(u8));
			cNotfServMsg.CatMsgBody((u8*)pbyBuf, byChlNum);//通道数直接写入
			cNotfServMsg.CatMsgBody(&byNameLen, sizeof(u8));
			cNotfServMsg.CatMsgBody((u8*)achMtName, byNameLen);
		}
		pbyBuf += byChlNum; // 偏移跳过通道数组
	}

	SendMsgToMcu(cNotfServMsg);
}

/*=============================================================================
  函 数 名： OnSendRosterNotify
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendRosterNotify(const CServMsg& cServMsg, HCHAN hsChan)
{
	TRosterList tList;
	tList.m_nCount = 0;
    BOOL32 bSend = FALSE;    

	TConfMtInfo *ptMTInfo = (TConfMtInfo *)(cServMsg.GetMsgBody());
	
	for( u8 byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop ++ )
	{
		if( byLoop + 1 == m_byMtId)  //自己
			continue;
			
		if( ptMTInfo->MtJoinedConf( byLoop + 1 ) )
		{
			tList.m_atRoster[tList.m_nCount].m_bLocal = TRUE;
			tList.m_atRoster[tList.m_nCount].m_dwPID = MAKEIID( ptMTInfo->GetMcuIdx(), (byLoop + 1) );//0x961aca4c;//
			
			tList.m_atRoster[tList.m_nCount].m_emConnState = emConnStateConnected;
			tList.m_nCount ++ ;		
		}
		else if( ptMTInfo->MtInConf( byLoop + 1 ))
		{
			tList.m_atRoster[tList.m_nCount].m_bLocal = TRUE;
			tList.m_atRoster[tList.m_nCount].m_dwPID = MAKEIID( ptMTInfo->GetMcuIdx(),(byLoop + 1) );//0x961aca4c;//
			
			tList.m_atRoster[tList.m_nCount].m_emConnState = emConnStateDisconnected;
		    tList.m_nCount ++ ;		
		}

		//每个花名册通知包 以ROSTER_PARTLIST_NUM 为最大part数，分多次进行通知
		if( ROSTER_PARTLIST_NUM == tList.m_nCount )
		{
			kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NEWROSTER_NTF, &tList, sizeof(tList) );
			tList.m_nCount = 0;
            bSend = TRUE;

		}				
	}
	
	//每个花名册通知包 以ROSTER_PARTLIST_NUM 为最大part数，分多次进行通知
	if( 0 != tList.m_nCount || bSend )
	{
		if( 0 != tList.m_nCount )
		{
			s32 nRet = kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NEWROSTER_NTF, &tList, sizeof(tList) );
			if ((s32)act_ok != nRet)
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[OnSendRosterNotify] send roster notif to remote failed!\n");
			}
		}
		
		//fxh 花名册通告结束以发送空的roster通告，收到该通告后对端再进行mtlist请求(避免重复请求)
//      bSend = TRUE;
		tList.m_nCount = 0;
		bSend = FALSE;
	}

    // xsl [11/3/2006] 如果没有终端，则发送空的roster通知，保证对端请求mtlist，并在mtlist ack里启动定时器
    if (!bSend)
    {
        s32 nRet = kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NEWROSTER_NTF, &tList, sizeof(tList) );
        if ((s32)act_ok != nRet)
        {
            MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[OnSendRosterNotify] send empty roster notif to remote failed!\n");
        }
    }

	return;
}

/*=============================================================================
  函 数 名： OnSendMtListAck
    功    能： 回应级联mcu的终端列表请求
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
    2005/9/26   3.6			万春雷                  创建
=============================================================================*/
void CMtAdpInst::OnSendMtListAck( const CServMsg& cServMsg, HCHAN hsChan )
{
	TMcuMcuMtInfo *ptInfo = (TMcuMcuMtInfo *)(cServMsg.GetMsgBody() + sizeof(u8));
	s32 nNum = ( cServMsg.GetMsgBodyLen() - sizeof(u8) ) / sizeof(TMcuMcuMtInfo);
	if( nNum > PART_NUM )   // xsl [8/11/2006] 底层不支持超过100个终端的列表发送
	{
		MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "Send H_CASCADE_PARTLIST_RSP to Cascadelib. mtid = %d, nRet = -1, PartCount = %d\n",
			                     m_byMtId, nNum);
		return;
	}

// 	u8  abyRspBuf[sizeof(TPartListMsg)+PART_NUM*sizeof(TPart)];
	u8  abyRspUBuf[sizeof(TPartListMsg)+PART_NUM*sizeof(TPart)] = {0};

// 	s32 nRealRspBufLen = sizeof(TPartListMsg)+(u32)nNum*sizeof(TPart);
	s32 nRealRspUBufLen = sizeof(TPartListMsg)+(u32)nNum*sizeof(TPart);

// 	TPartListMsg *ptRspPtr = (TPartListMsg*)abyRspBuf;
	TPartListMsg *ptRspPtrU = (TPartListMsg*)abyRspUBuf;
// 	TPart *ptPartPtr = (TPart *)(abyRspBuf + sizeof(TPartListMsg));
	TPart *ptPartPtrU = (TPart *)(abyRspUBuf + sizeof(TPartListMsg));
	
// 	ptRspPtr->m_nReqID      = GetPeerReqId();
// 	ptRspPtr->m_dwPartCount = nNum;
// 	ptRspPtr->m_bLastPack   = ISTRUE(*(cServMsg.GetMsgBody())); // xsl [8/11/2006] 仅用于接收，发送时此字段没用
// 	ptRspPtr->m_emReturnVal = emReturnValue_Ok;
// 	ptRspPtr->m_nSnapShot   = 1663;

	ptRspPtrU->m_nReqID      = GetPeerReqId();
	ptRspPtrU->m_dwPartCount = nNum;
	ptRspPtrU->m_bLastPack   = ISTRUE(*(cServMsg.GetMsgBody())); // xsl [8/11/2006] 仅用于接收，发送时此字段没用
	ptRspPtrU->m_emReturnVal = emReturnValue_Ok;
	ptRspPtrU->m_nSnapShot   = 1663;

	s32 nLoop = 0;
	
	MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[OnSendMtListAck]Send PartList.%d to Mt.%d\n", nNum, m_byMtId);
	for( nLoop=0; nLoop<nNum; nLoop++)
	{
// 		if ( GetEndPointEncoding() == emenCoding_GBK )
// 		{
// 			MtInfo2Part(ptPartPtr, (ptInfo+nLoop));
// 			ptPartPtr++;
// 		}
// 		else if ( GetEndPointEncoding() == emenCoding_Utf8 )
		{
			MtInfo2Part(ptPartPtrU, (ptInfo+nLoop));
			ptPartPtrU++;
		}
	}

	if(g_dwSendMMcu > 0)
	{
		s32 nRet = 0;
// 		if ( GetEndPointEncoding() == emenCoding_GBK )
// 		{
// 			nRet = kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_PARTLIST_RSP, abyRspBuf, nRealRspBufLen);
// 		}
// 		else if ( GetEndPointEncoding() == emenCoding_Utf8 )
		{
			nRet = kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_PARTLIST_RSP, abyRspUBuf, nRealRspUBufLen);
		}

		MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "Send H_CASCADE_PARTLIST_RSP to Cascadelib. mtid = %d, nRet = %d, PartCount = %d, bLastPack = %d\n",
			m_byMtId, nRet, ptRspPtrU->m_dwPartCount, ptRspPtrU->m_bLastPack);
		MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "TPart: %d  RealBufLen:%d\n",
			sizeof(TPart), /*sizeof(TPartU),*/ /*nRealRspBufLen*/ nRealRspUBufLen);
	}
    
	return;
}

/*=============================================================================
函 数 名： OnSendCancelMeSpeakerReq
功    能： 
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
5/13/2010   4.6			张宝卿                  创建
=============================================================================*/
void CMtAdpInst::OnSendCancelMeSpeakerReq(const CServMsg &cServMsg, HCHAN hsChan)
{
    CServMsg cMsg;
    cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
    TNonStandardReq tReq;
    tReq.m_aszUserName[0] = 0;
    tReq.m_aszUserPass[0] = 0;
    tReq.m_nReqID = m_dwMMcuReqId++;
    
    //modify msg body
    //BOOL32 bLocal = FALSE;
    //TPartID tPartID;
	TMsgHeadMsg tHeadMsg;
	TMtadpHeadMsg tMtadpHeadMsg;
	tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
    TMt tMt = *(TMt *)( cServMsg.GetMsgBody() +sizeof(TMsgHeadMsg) );
	if( !tMt.IsMcuIdLocal() )
	{
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[tHeadMsg.m_tMsgSrc.m_byCasLevel] = tMt.GetMtId();
		++tHeadMsg.m_tMsgSrc.m_byCasLevel;
		tMt.SetMtId( (u8)tMt.GetMcuId() );
		tMt.SetMcuId( LOCAL_MCUID );
	}
    
    tMtadpHeadMsg.m_tMsgSrc.m_dwPID = GetPartIdFromMt( tMt, &tMtadpHeadMsg.m_tMsgSrc.m_bLocal );
	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = htonl( tMtadpHeadMsg.m_tMsgSrc.m_dwPID );

	HeadMsgToMtadpHeadMsg( tHeadMsg,tMtadpHeadMsg );
    
    cMsg.SetMsgBody( (u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg) );
    cMsg.CatMsgBody( (u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg)),
						cServMsg.GetMsgBodyLen() - sizeof(TMt) - sizeof(TMsgHeadMsg)
						);
    
    tReq.m_nMsgLen = cMsg.GetServMsgLen();
    if((u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf))
    {
        StaticLog( "[MTADAP][EXP]OnSendCancelMeSpeakerReq message is too large(len:%d)\n", tReq.m_nMsgLen);
        return;
    }
    
    memcpy(tReq.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());
    kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq));
    
	return;
}



/*=============================================================================
函 数 名： OnSendCancelMeSpeakerAck
功    能： 
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
5/13/2010   4.6			张宝卿                  创建
=============================================================================*/
void CMtAdpInst::OnSendCancelMeSpeakerAck(const CServMsg &cServMsg, HCHAN hsChan)
{
    CServMsg cMsg;
    cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
    
    TNonStandardRsp tRsp;
    tRsp.m_nReqID      = 0;
    tRsp.m_emReturnVal = emReturnValue_Ok;
    
    //modify msg body
    //BOOL32 bLocal = FALSE;
    
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TMtadpHeadMsg tMtadpHeadMsg;
    TMt tMt = *(TMt *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
    
    tMtadpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tMt, &tMtadpHeadMsg.m_tMsgDst.m_bLocal );
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = htonl(tMtadpHeadMsg.m_tMsgDst.m_dwPID);

	HeadMsgToMtadpHeadMsg( tHeadMsg,tMtadpHeadMsg );
    
    cMsg.SetMsgBody( (u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg) );
    cMsg.CatMsgBody( (u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg)),
						cServMsg.GetMsgBodyLen() - sizeof(TMt) - sizeof(TMsgHeadMsg)
						);
    
    tRsp.m_nMsgLen = cMsg.GetServMsgLen();
    if( (u32)tRsp.m_nMsgLen > sizeof(tRsp.m_abyMsgBuf) )
    {
        StaticLog( "[MTADAP][EXP]OnSendApplySpeakerAck message is too large(len:%d)\n", tRsp.m_nMsgLen );
        return;
    }
    
    memcpy(tRsp.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());
    
    kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_RSP, &tRsp, sizeof(tRsp) );
	
    return;
}

/*=============================================================================
函 数 名： OnSendCancelMeSpeakerNack
功    能： 
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
5/13/2010   4.6			张宝卿                  创建
=============================================================================*/
void CMtAdpInst::OnSendCancelMeSpeakerNack(const CServMsg &cServMsg, HCHAN hsChan)
{
    CServMsg cMsg;
    cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
    
    TNonStandardRsp tRsp;
    tRsp.m_nReqID      = 0;
    tRsp.m_emReturnVal = emReturnValue_Error;
    
    //modify msg body
    //BOOL32 bLocal = FALSE;    
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TMtadpHeadMsg tMtadpHeadMsg;
    TMt tMt = *(TMt *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
    
    tMtadpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tMt, &tMtadpHeadMsg.m_tMsgDst.m_bLocal );
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = htonl(tMtadpHeadMsg.m_tMsgDst.m_dwPID);

	HeadMsgToMtadpHeadMsg( tHeadMsg,tMtadpHeadMsg );
    
    cMsg.SetMsgBody( (u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg) );
    cMsg.CatMsgBody( (u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg)),
						cServMsg.GetMsgBodyLen() - sizeof(TMt) - sizeof(TMsgHeadMsg)
						);


    tRsp.m_nMsgLen = cMsg.GetServMsgLen();
    if( (u32)tRsp.m_nMsgLen > sizeof(tRsp.m_abyMsgBuf) )
    {
        StaticLog( "[MTADAP][EXP]OnSendCancelMeSpeakerNack message is too large(len:%d)\n", tRsp.m_nMsgLen );
        return;
    }
    
    memcpy(tRsp.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());
    
    kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_RSP, &tRsp, sizeof(tRsp) );
    
	return;
}

/*=============================================================================
  函 数 名： OnSendVideoInfoAck
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendVideoInfoAck( const CServMsg& cServMsg, HCHAN hsChan )
{
	TCConfViewInfo *ptInInfo = (TCConfViewInfo *)(cServMsg.GetMsgBody());

	TConfVideoInfo  tOutInfo;
	VideoInfoIn2Out(ptInInfo, &tOutInfo);
	tOutInfo.m_nReqID    = GetPeerReqId();
	tOutInfo.m_nSnapShot = 1663;
	tOutInfo.m_bFullInfo = FALSE;
	if(g_dwSendMMcu > 0)
	{		
		kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_VIDEOINFO_RSP, &tOutInfo, sizeof(tOutInfo));
	}
	return;
}

/*=============================================================================
  函 数 名： OnSendAudioInfoAck
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendAudioInfoAck( const CServMsg& cServMsg, HCHAN hsChan )
{
	TCConfAudioInfo *ptInInfo = (TCConfAudioInfo *)(cServMsg.GetMsgBody());
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TCConfAudioInfo));
	TConfAudioInfo   tOutInfo;
	AudioInfoIn2Out(ptInInfo, &tOutInfo);
	tOutInfo.m_tMsgSrc.m_byCasLevel = tHeadMsg.m_tMsgSrc.m_byCasLevel;
	memcpy( &tOutInfo.m_tMsgSrc.m_abyMtIdentify[0],
			&tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
			sizeof( tOutInfo.m_tMsgSrc.m_abyMtIdentify )
			);
	tOutInfo.m_nReqID    = GetPeerReqId();
	tOutInfo.m_nSnapShot = 1663;
	if(g_dwSendMMcu > 0)
	{		
		kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_AUDIOINFO_RSP, &tOutInfo, sizeof(tOutInfo));
	}
	return;
}

/*=============================================================================
  函 数 名： OnSendInviteMtReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendInviteMtReq( const CServMsg& cServMsg, HCHAN hsChan, const TReq &tReq, const TPartID &tPardId)
{
	//TMcuMcuReq *ptReq = (TMcuMcuReq *)cServMsg.GetMsgBody();
	//TMcu tMcu = *(TMcu*)cServMsg.GetMsgBody();
	TAddMtInfo* ptAddMtInfo = (TAddMtInfo *)( cServMsg.GetMsgBody() + sizeof(TMcu) );
	s32 nMtNum = ( cServMsg.GetMsgBodyLen() - sizeof(TMcu) ) / sizeof(TAddMtInfo);
	TInvitePartReq tInvitePartReq;

    //终端呼叫别名类型
    u8 abyAliasType[PART_NUM] = { 0 };
	
	tInvitePartReq.m_nReqID = tReq.m_nReqID;
	// 多国语言 [pengguofeng 4/8/2013]
	s8 achUserName[CASCADE_USERNAME_MAXLEN];
	memset(achUserName, 0, sizeof(achUserName));
	BOOL32 bNeedTrans = TransEncoding((s8*)tReq.m_aszUserName, achUserName, sizeof(achUserName));
	if ( bNeedTrans == TRUE)
	{
		astrncpy(tInvitePartReq.m_aszUserName, achUserName, 
			sizeof(tInvitePartReq.m_aszUserName), sizeof(achUserName));
	}
	else
	{
		astrncpy(tInvitePartReq.m_aszUserName, tReq.m_aszUserName, 
			sizeof(tInvitePartReq.m_aszUserName), sizeof(tReq.m_aszUserName));
	}
	astrncpy(tInvitePartReq.m_aszUserPass, tReq.m_aszUserPass, 
		     sizeof(tInvitePartReq.m_aszUserPass), sizeof(tReq.m_aszUserPass));

	tInvitePartReq.m_nCount = min( nMtNum, PART_NUM ); 
	s8 achMtAlias[STR_LEN]; //pengguofeng 多国语言
	for(s32 nIndex = 0; nIndex < tInvitePartReq.m_nCount; nIndex ++ )
	{
		tInvitePartReq.m_atPart[nIndex].m_dwCallRate = 0;
		if( ptAddMtInfo[nIndex].m_AliasType == (u8)mtAliasTypeTransportAddress )
		{
			CMtAdpUtils::Ipdw2Str( ptAddMtInfo[nIndex].m_tTransportAddr.GetNetSeqIpAddr(), tInvitePartReq.m_atPart[nIndex].m_aszDialStr); 
		}
		else
		{
			memset(achMtAlias, 0, sizeof(achMtAlias));
			bNeedTrans = TransEncoding(ptAddMtInfo[nIndex].m_achAlias, achMtAlias, sizeof(achMtAlias));
			if ( bNeedTrans == TRUE )
			{
				strncpy( tInvitePartReq.m_atPart[nIndex].m_aszDialStr, achMtAlias, sizeof(tInvitePartReq.m_atPart[nIndex].m_aszDialStr) ); 
			}
			else
			{
				strncpy( tInvitePartReq.m_atPart[nIndex].m_aszDialStr, ptAddMtInfo[nIndex].m_achAlias, sizeof(tInvitePartReq.m_atPart[nIndex].m_aszDialStr) ); 
			}
		}
		tInvitePartReq.m_atPart[nIndex].m_aszDialStr[sizeof(tInvitePartReq.m_atPart[nIndex].m_aszDialStr) - 1] = 0;
		abyAliasType[nIndex] = ptAddMtInfo[nIndex].m_AliasType;
		memset( &(tInvitePartReq.m_atPart[nIndex].m_tPartVideoInfo), 0, sizeof(tInvitePartReq.m_atPart[nIndex].m_tPartVideoInfo) );
	}

	tInvitePartReq.m_tMsgDst.m_byCasLevel = tReq.m_tMsgDst.m_byCasLevel;
	memcpy( &tInvitePartReq.m_tMsgDst.m_abyMtIdentify[0],
			&tReq.m_tMsgDst.m_abyMtIdentify[0],
			sizeof( tInvitePartReq.m_tMsgDst.m_abyMtIdentify )
			);

	tInvitePartReq.m_tMsgSrc.m_byCasLevel = tReq.m_tMsgSrc.m_byCasLevel;
	memcpy( &tInvitePartReq.m_tMsgSrc.m_abyMtIdentify[0],
			&tReq.m_tMsgSrc.m_abyMtIdentify[0],
			sizeof( tInvitePartReq.m_tMsgSrc.m_abyMtIdentify )
			);

	tInvitePartReq.m_dwPID = tPardId.m_dwPID;
	tInvitePartReq.m_bLocal = tPardId.m_bLocal;

    s8 achBuf[sizeof(tInvitePartReq) + sizeof(abyAliasType)] = { 0 };
    memcpy( achBuf, &tInvitePartReq, sizeof(tInvitePartReq));
    memcpy( achBuf + sizeof(tInvitePartReq), abyAliasType, sizeof(abyAliasType));

	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_INVITEPART_REQ, achBuf/*&tInvitePartReq*/, sizeof(achBuf)/*sizeof(tInvitePartReq)*/ );

	return;
}

/*=============================================================================
  函 数 名： OnSendReInviteMtReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendReInviteMtReq(const CServMsg& cServMsg, HCHAN hsChan, TReq tReq, TPartID tPardId )
{
	//TMcuMcuReq *ptReq = (TMcuMcuReq *)cServMsg.GetMsgBody();
	//TMt* ptMt = (TMt* )(cServMsg.GetMsgBody() + sizeof(TMcuMcuReq));

	TPartReq tPartReq;
	tPartReq.m_dwPID  = tPardId.m_dwPID;
	tPartReq.m_bLocal = tPardId.m_bLocal;
	tPartReq.m_nReqID = tReq.m_nReqID;

	// 多国语言 [pengguofeng 4/8/2013]
	s8 achUserName[CASCADE_USERNAME_MAXLEN];
	memset(achUserName, 0, sizeof(achUserName));
	BOOL32 bNeedTrans = TransEncoding(tReq.m_aszUserName, achUserName, sizeof(achUserName));
	if ( bNeedTrans == TRUE)
	{
		astrncpy(tPartReq.m_aszUserName, achUserName, 
			sizeof(tPartReq.m_aszUserName), sizeof(achUserName));
	}
	else
	{
		astrncpy(tPartReq.m_aszUserName, tReq.m_aszUserName, 
			sizeof(tPartReq.m_aszUserName), sizeof(tReq.m_aszUserName));
	}
	astrncpy(tPartReq.m_aszUserPass, tReq.m_aszUserPass, 
		     sizeof(tPartReq.m_aszUserPass), sizeof(tReq.m_aszUserPass));

	tPartReq.m_tMsgDst.m_byCasLevel = tReq.m_tMsgDst.m_byCasLevel;
	memcpy( &tPartReq.m_tMsgDst.m_abyMtIdentify[0],
			&tReq.m_tMsgDst.m_abyMtIdentify[0],
			sizeof( tPartReq.m_tMsgDst.m_abyMtIdentify )
			);

	tPartReq.m_tMsgSrc.m_byCasLevel = tReq.m_tMsgSrc.m_byCasLevel;
	memcpy( &tPartReq.m_tMsgSrc.m_abyMtIdentify[0],
			&tReq.m_tMsgSrc.m_abyMtIdentify[0],
			sizeof( tPartReq.m_tMsgSrc.m_abyMtIdentify )
			);

	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_REINVITEPART_REQ, &tPartReq, sizeof(tPartReq) );

	return;
}

/*=============================================================================
  函 数 名： OnSendCallAlertMtNotify
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendCallAlertMtNotify( const CServMsg& cServMsg, HCHAN hsChan )
{
	TMt      *ptMt      = (TMt *)(cServMsg.GetMsgBody());
	TMtAlias *ptMtAlias = (TMtAlias *)(cServMsg.GetMsgBody()+sizeof(TMt));
	BOOL32    bLocal    = TRUE;
	u32       dwPID     = GetPartIdFromMt(*ptMt, &bLocal);
// 	TCallNtf  tCallAlertNtf;
	TCallNtf tCallAlertNtfU;
	
	if( (u8)mtAliasTypeTransportAddress != ptMtAlias->m_AliasType )
	{
		s8 achMtAlias[MAXLEN_ALIAS]; // pengguofeng 多国语言
		memset(achMtAlias, 0, sizeof(achMtAlias));
		BOOL32 bNeedTrans = TransEncoding(ptMtAlias->m_achAlias, achMtAlias, sizeof(achMtAlias));
		if ( bNeedTrans == TRUE )
		{
			tCallAlertNtfU.SetPartCallingParam( dwPID, bLocal, achMtAlias, sizeof(achMtAlias) );
		}
		else
		{
			tCallAlertNtfU.SetPartCallingParam( dwPID, bLocal, ptMtAlias->m_achAlias, sizeof(ptMtAlias->m_achAlias) );
		}
	}
	else
	{
		s8   szDialIP[16];
		CMtAdpUtils::Ipdw2Str( ptMtAlias->m_tTransportAddr.GetNetSeqIpAddr(), szDialIP );
		tCallAlertNtfU.SetPartCallingParam( dwPID, bLocal, szDialIP, sizeof(szDialIP) );
	}
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_CALL_ALERTING_NTF, &tCallAlertNtfU, sizeof(tCallAlertNtfU) );
	
	return;
}

/*=============================================================================
  函 数 名： OnSendNewMtNotify
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容

=============================================================================*/
void CMtAdpInst::OnSendNewMtNotify(CServMsg& cServMsg, HCHAN hsChan)
{
	TMcuMcuMtInfo *ptInfo = (TMcuMcuMtInfo *)(cServMsg.GetMsgBody());

// 	if ( GetEndPointEncoding() == emenCoding_GBK )
// 	{
// 		TPart tPart;
// 		MtInfo2Part( &tPart, ptInfo );
// 		kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NEWPART_NTF, &tPart, sizeof(tPart) );
// 	}
// 	else if ( GetEndPointEncoding() == emenCoding_Utf8 )
	{
		TPart tPartU;
		MtInfo2Part( &tPartU, ptInfo );
		kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NEWPART_NTF, &tPartU, sizeof(tPartU) );
	}

	return;
}

/*=============================================================================
  函 数 名： OnSendDropMtReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendDropMtReq( CServMsg& cServMsg, HCHAN hsChan, TReq tReq, TPartID tPardId )
{
	//TMcuMcuReq *ptReq = (TMcuMcuReq * )cServMsg.GetMsgBody();
	//TMt *ptMt = (TMt *)(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));

	TPartReq tPartReq;
	tPartReq.m_dwPID  = tPardId.m_dwPID;
	tPartReq.m_bLocal = tPardId.m_bLocal;
	tPartReq.m_nReqID = tReq.m_nReqID;

	// 多国语言 [pengguofeng 4/8/2013]
	s8 achUserName[CASCADE_USERNAME_MAXLEN];
	memset(achUserName, 0, sizeof(achUserName));
	BOOL32 bNeedTrans = TransEncoding(tReq.m_aszUserName, achUserName, sizeof(achUserName));
	if ( bNeedTrans == TRUE)
	{
		astrncpy(tPartReq.m_aszUserName, achUserName, 
			sizeof(tPartReq.m_aszUserName), sizeof(achUserName));
	}
	else
	{
		astrncpy(tPartReq.m_aszUserName, tReq.m_aszUserName, 
			sizeof(tPartReq.m_aszUserName), sizeof(tReq.m_aszUserName));
	}
	astrncpy(tPartReq.m_aszUserPass, tReq.m_aszUserPass, 
		     sizeof(tPartReq.m_aszUserPass), sizeof(tReq.m_aszUserPass));

	tPartReq.m_tMsgDst.m_byCasLevel = tReq.m_tMsgDst.m_byCasLevel;
	memcpy( &tPartReq.m_tMsgDst.m_abyMtIdentify[0],
			&tReq.m_tMsgDst.m_abyMtIdentify[0],
			sizeof( tPartReq.m_tMsgDst.m_abyMtIdentify )
			);

	tPartReq.m_tMsgSrc.m_byCasLevel = tReq.m_tMsgSrc.m_byCasLevel;
	memcpy( &tPartReq.m_tMsgSrc.m_abyMtIdentify[0],
			&tReq.m_tMsgSrc.m_abyMtIdentify[0],
			sizeof( tPartReq.m_tMsgSrc.m_abyMtIdentify )
			);
	
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_DISCONNPART_REQ, &tPartReq, sizeof(tPartReq) );

	return;
}

/*=============================================================================
  函 数 名： OnSendDelMtReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendDelMtReq( CServMsg& cServMsg, HCHAN hsChan, TReq tReq, TPartID tPardId )
{
	//TMcuMcuReq *ptReq = (TMcuMcuReq * )cServMsg.GetMsgBody();
	TMt *ptMt = (TMt *)(cServMsg.GetMsgBody() + sizeof(TMcuMcuReq));
	
	u8 byInsId = (u8)GetInsID();
	TMcuList *ptMcuList = g_cMtAdpApp.m_tAllSMcuList.GetMcuList(byInsId);
	if ( ptMt->GetMtType() == MT_TYPE_SMCU
		|| ptMt->GetMtType() == MT_TYPE_MMCU )
	{
		u8 byDelMcuId = ptMt->GetMtId();
		if ( ptMcuList )
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[OnSendDelMtReq]delete Mcu.%d\n", byDelMcuId);
			ptMcuList->DelMtList(byDelMcuId);
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[OnSendDelMtReq]can not find Mcu.%d \n", byDelMcuId);
		}
	}

	TPartReq tPartReq;
	tPartReq.m_dwPID  = tPardId.m_dwPID;
	tPartReq.m_bLocal = tPardId.m_bLocal;
	tPartReq.m_nReqID = tReq.m_nReqID;

	// 多国语言 [pengguofeng 4/8/2013]
	s8 achUserName[CASCADE_USERNAME_MAXLEN];
	memset(achUserName, 0, sizeof(achUserName));
	BOOL32 bNeedTrans = TransEncoding(tReq.m_aszUserName, achUserName, sizeof(achUserName));
	if ( bNeedTrans == TRUE)
	{
		astrncpy(tPartReq.m_aszUserName, achUserName, 
			sizeof(tPartReq.m_aszUserName), sizeof(achUserName));
	}
	else
	{
		astrncpy(tPartReq.m_aszUserName, tReq.m_aszUserName, 
			sizeof(tPartReq.m_aszUserName), sizeof(tReq.m_aszUserName));
	}
	astrncpy(tPartReq.m_aszUserPass, tReq.m_aszUserPass, 
		     sizeof(tPartReq.m_aszUserPass), sizeof(tReq.m_aszUserPass));

	tPartReq.m_tMsgDst.m_byCasLevel = tReq.m_tMsgDst.m_byCasLevel;
	memcpy( &tPartReq.m_tMsgDst.m_abyMtIdentify[0],
			&tReq.m_tMsgDst.m_abyMtIdentify[0],
			sizeof( tPartReq.m_tMsgDst.m_abyMtIdentify )
			);

	tPartReq.m_tMsgSrc.m_byCasLevel = tReq.m_tMsgSrc.m_byCasLevel;
	memcpy( &tPartReq.m_tMsgSrc.m_abyMtIdentify[0],
			&tReq.m_tMsgSrc.m_abyMtIdentify[0],
			sizeof( tPartReq.m_tMsgSrc.m_abyMtIdentify )
			);

	u8 byMcuId = 0xFF;
	u8 byDelMtId = 0;
	TMt tMt;
	tMt.SetNull();
	tMt = GetMtFromPartId(tPartReq.m_dwPID, tPartReq.m_bLocal);
	if ( tPartReq.m_tMsgDst.m_byCasLevel == 0 )
	{
		//本级删除0 + tMt.MtId
		byMcuId = 0;
		byDelMtId = tMt.GetMtId();
	}
	else
	{
		//下级删除tMt.McuId + identify[0]
		byMcuId = tMt.GetMtId()/*(u8)tMt.GetMcuId()*/;
		byDelMtId = tPartReq.m_tMsgDst.m_abyMtIdentify[0];
	}
	// 删除下级终端，需要把对应的列表清空 [pengguofeng 7/3/2013]
	LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[OnSendDelMtReq]delete name of mcu.%d mt<%d %d> delmtid.%d\n",
		byMcuId, tMt.GetMcuId(), tMt.GetMtId(), byDelMtId);

	if ( ptMcuList )
	{
		TMtList *ptMtList = ptMcuList->GetMtList( byMcuId/*(u8)tMt.GetMcuId()*/);
		if ( ptMtList )
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[OnSendDelMtReq]delete OK\n");
			ptMtList->DelMtName(/*tMt.GetMtId()*/byDelMtId);
		}
	}
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_DELETEPART_REQ, &tPartReq, sizeof(tPartReq) );

	return;
	
}

/*=============================================================================
  函 数 名： OnSendSetMtChanReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendSetMtChanReq( CServMsg& cServMsg, HCHAN hsChan )
{
	TMcuMcuReq *ptReq = (TMcuMcuReq *)cServMsg.GetMsgBody();
	TMtMediaChanStatus *ptStatus = (TMtMediaChanStatus *)( cServMsg.GetMsgBody() + sizeof(TMcuMcuReq) );

	TPartMediaChanReq tReq;
	tReq.m_dwPID           = GetPartIdFromMt( ptStatus->m_tMt, &(tReq.m_bLocal) );
	tReq.m_emMediaType     = CMtAdpUtils::MediaModeIn2Out( ptStatus->m_byMediaMode );
	tReq.m_emMuteDirection = ISTRUE(ptStatus->m_byIsDirectionIn) ? emDirectionIn:emDirectionOut;
	tReq.m_bMute           = ptStatus->m_bMute;
	tReq.m_nReqID          = m_dwMMcuReqId ++ ;
	
	// 多国语言 [pengguofeng 4/8/2013]
	s8 achUserName[CASCADE_USERNAME_MAXLEN];
	memset(achUserName, 0, sizeof(achUserName));
	BOOL32 bNeedTrans = TransEncoding(ptReq->m_szUserName, achUserName, sizeof(achUserName));
	if ( bNeedTrans == TRUE)
	{
		astrncpy(tReq.m_aszUserName, achUserName, 
			sizeof(tReq.m_aszUserName), sizeof(achUserName));
	}
	else
	{
		astrncpy(tReq.m_aszUserName, ptReq->m_szUserName, 
			sizeof(tReq.m_aszUserName), sizeof(ptReq->m_szUserName));
	}
	astrncpy(tReq.m_aszUserPass, ptReq->m_szUserPwd, 
		     sizeof(tReq.m_aszUserPass), sizeof(ptReq->m_szUserPwd));

	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_PARTMEDIACHAN_REQ, &tReq, sizeof(tReq) );

	return;
}

/*=============================================================================
  函 数 名： OnSendSetInReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendSetInReq( CServMsg& cServMsg, HCHAN hsChan, TReq tReq, TPartID tPardId )
{
	//TMcuMcuReq  *ptReq   = (TMcuMcuReq *)cServMsg.GetMsgBody();
	TSetInParam *ptParam = (TSetInParam *)( cServMsg.GetMsgBody() );
	
	TSetInReq tSetInReq;
	tSetInReq.m_nReqID  = tReq.m_nReqID;
	tSetInReq.m_dwPID   = tPardId.m_dwPID;
	tSetInReq.m_bLocal  = tPardId.m_bLocal;
	tSetInReq.m_nViewID = ptParam->m_nViewId;
	tSetInReq.m_bySubFrameIndex = ptParam->m_bySubFrameIndex;

	// 多国语言 [pengguofeng 4/8/2013]
	s8 achUserName[CASCADE_USERNAME_MAXLEN];
	memset(achUserName, 0, sizeof(achUserName));
	BOOL32 bNeedTrans = TransEncoding(tReq.m_aszUserName, achUserName, sizeof(achUserName));
	if ( bNeedTrans == TRUE)
	{
		astrncpy(tSetInReq.m_aszUserName, achUserName, 
			sizeof(tSetInReq.m_aszUserName), sizeof(achUserName));
	}
	else
	{
		astrncpy(tSetInReq.m_aszUserName, tReq.m_aszUserName, 
			sizeof(tSetInReq.m_aszUserName), sizeof(tReq.m_aszUserName));
	}
	astrncpy(tSetInReq.m_aszUserPass, tReq.m_aszUserPass, 
		     sizeof(tSetInReq.m_aszUserPass), sizeof(tReq.m_aszUserPass));

	tSetInReq.m_tMsgDst.m_byCasLevel = tReq.m_tMsgDst.m_byCasLevel;
	memcpy( &tSetInReq.m_tMsgDst.m_abyMtIdentify[0],
			&tReq.m_tMsgDst.m_abyMtIdentify[0],
			sizeof( tSetInReq.m_tMsgDst.m_abyMtIdentify )
			);

	tSetInReq.m_tMsgSrc.m_byCasLevel = tReq.m_tMsgSrc.m_byCasLevel;
	memcpy( &tSetInReq.m_tMsgSrc.m_abyMtIdentify[0],
			&tReq.m_tMsgSrc.m_abyMtIdentify[0],
			sizeof( tSetInReq.m_tMsgSrc.m_abyMtIdentify )
			);
	
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_SETIN_REQ, &tSetInReq, sizeof(tSetInReq) );
	

	//针对R2等版本坐下级，在ProcSetOutNotif由于由于兼容问题，会忽略掉传给上级MCU的set_out_notify消息
	//故此处针对R2等版本，直接恢复setout_notify[10/19/2012 chendaiwei]
	LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);	
	if( strcmp(lpszPeerVerId, "4.0") == 0 ||
		strcmp(lpszPeerVerId, "3.6") == 0 || 
		strcmp(lpszPeerVerId, "8") == 0 ||
		strcmp(lpszPeerVerId, "9") == 0 ||
		strcmp(lpszPeerVerId, "5") == 0)
	{

		CServMsg cMsg;
		TCConfViewOutInfo tParam;
		tParam.m_tMt = ptParam->m_tMt;
		tParam.m_nOutViewID = ptParam->m_nViewId;
		tParam.m_nOutVideoSchemeID = 0;
		cMsg.SetEventId( MCU_MCU_SETOUT_NOTIF );
		cMsg.SetMsgBody((u8*)&tParam,sizeof(tParam));

		TMsgHeadMsg tHeadMsg;
		tHeadMsg.m_tMsgDst.m_byCasLevel = tReq.m_tMsgSrc.m_byCasLevel;
		tHeadMsg.m_tMsgSrc.m_byCasLevel = tReq.m_tMsgDst.m_byCasLevel;
		
		memcpy( &tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
			&tReq.m_tMsgDst.m_abyMtIdentify[0],
			sizeof( tSetInReq.m_tMsgDst.m_abyMtIdentify )
			);
		
		memcpy( &tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
			&tReq.m_tMsgSrc.m_abyMtIdentify[0],
			sizeof( tSetInReq.m_tMsgSrc.m_abyMtIdentify )
			);

		cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );

		SendMsgToMcu( cMsg );
	}
	
	return;
}

/*=============================================================================
  函 数 名： OnSendSetOutReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendSetOutReq(CServMsg& cServMsg, HCHAN hsChan)
{
	TMcuMcuReq   *ptReq   = (TMcuMcuReq *)cServMsg.GetMsgBody();
	TSetOutParam *ptParam = (TSetOutParam *)( cServMsg.GetMsgBody() + sizeof(TMcuMcuReq) );
	TSetOutReq    tReq;
	tReq.m_nReqID = m_dwMMcuReqId ++;

	// 多国语言 [pengguofeng 4/8/2013]
	s8 achUserName[CASCADE_USERNAME_MAXLEN];
	memset(achUserName, 0, sizeof(achUserName));
	BOOL32 bNeedTrans = TransEncoding(ptReq->m_szUserName, achUserName, sizeof(achUserName));
	if ( bNeedTrans == TRUE)
	{
		astrncpy(tReq.m_aszUserName, achUserName, 
			sizeof(tReq.m_aszUserName), sizeof(achUserName));
	}
	else
	{
		astrncpy(tReq.m_aszUserName, ptReq->m_szUserName, 
			sizeof(tReq.m_aszUserName), sizeof(ptReq->m_szUserName));
	}
	astrncpy(tReq.m_aszUserPass, ptReq->m_szUserPwd, 
		     sizeof(tReq.m_aszUserPass), sizeof(ptReq->m_szUserPwd)); 
	
	tReq.m_dwPartCount = min( ptParam->m_nMtCount, PART_NUM );
	for( s32 nLoop = 0; nLoop < tReq.m_dwPartCount; nLoop ++ )
	{
		tReq.m_tPartOutInfo[nLoop].m_dwPID = GetPartIdFromMt(ptParam->m_atConfViewOutInfo[nLoop].m_tMt, &(tReq.m_tPartOutInfo[nLoop].m_bLocal));
		tReq.m_tPartOutInfo[nLoop].m_nOutViewID = ptParam->m_atConfViewOutInfo[nLoop].m_nOutViewID;
		tReq.m_tPartOutInfo[nLoop].m_nOutVideoSchemeID = ptParam->m_atConfViewOutInfo[nLoop].m_nOutVideoSchemeID;
	}
    kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_SETOUT_REQ, &tReq, sizeof(tReq) );

	return;
}

/*=============================================================================
  函 数 名： OnSendSetOutNotify
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendSetOutNotify( CServMsg& cServMsg, HCHAN hsChan )
{
	TSetOutParam   *ptParam = (TSetOutParam *)(cServMsg.GetMsgBody());
	TPartOutputNtf  tNtf;
	
	if( ptParam->m_nMtCount == 1 )
	{
		tNtf.m_dwPID   = GetPartIdFromMt( ptParam->m_atConfViewOutInfo[0].m_tMt, &(tNtf.m_bLocal) );
		tNtf.m_nViewID = ptParam->m_atConfViewOutInfo[0].m_nOutViewID;
	}
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_OUTPUT_NTF, &tNtf, sizeof(tNtf) );

	return;
}

/*=============================================================================
  函 数 名： OnSendAutoSwitchReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2007/07/19  4.0  		顾振华                  创建
=============================================================================*/
void CMtAdpInst::OnSendAutoSwitchReq( CServMsg& cServMsg, HCHAN hsChan )
{
    TCAutoSwitchReq *ptASReq = (TCAutoSwitchReq *)(cServMsg.GetMsgBody());
    TAutoswitchReq tSwitchReq;

    tSwitchReq.SetAutoSwitchOn(ptASReq->m_bSwitchOn);
    tSwitchReq.SetAutoSwitchTime(ptASReq->m_nSwitchSpaceTime);
    tSwitchReq.SetAutoSwitchLevel(ptASReq->m_nAutoSwitchLevel);
    tSwitchReq.SetLid(ptASReq->m_nSwitchLayerId);
    
    kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_SET_LAYOUT_AUTOSWITCH_REQUEST, 
                        &tSwitchReq, sizeof(tSwitchReq) ); 
}

/*=============================================================================
    函 数 名： OnSendStartMixerCmd
    功    能： 发送 上级MCU开启下级MCU会议讨论命令
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/5/27   3.6			万春雷                  创建
=============================================================================*/
void CMtAdpInst::OnSendStartMixerCmd( CServMsg& cServMsg, HCHAN hsChan )
{
	//20101217_tzy 开启级联定制混音用MCU_MCU_ADDMIXMEMBER_CMD命令
	//该命令用于开启下级会议讨论
	CServMsg cMsg;
	cMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
	u8 byDepth = *(cServMsg.GetMsgBody() + sizeof(TMcu));
	cMsg.SetMsgBody((u8*)&byDepth, sizeof(u8));
	s32 nMtNum = ( cServMsg.GetMsgBodyLen() - sizeof(TMcu) - sizeof(u8) ) / sizeof(TMt);
	if( 0 == byDepth || nMtNum > MAXNUM_CONF_MT )
	{
		return;
	}
	TMt *ptMt  = (TMt *)(cServMsg.GetMsgBody() + sizeof(TMcu) + sizeof(u8));
	u32 dwPartId  = 0;
	BOOL32 bLocal = FALSE;
	
	for( s32 nIndex = 0; nIndex < nMtNum; nIndex ++ )
	{
		if (ptMt == NULL)
		{
			break;
		}
		dwPartId = GetPartIdFromMt( *ptMt, &bLocal );	
		dwPartId = htonl( dwPartId );
		cMsg.CatMsgBody( (u8 *)&dwPartId, sizeof(dwPartId) );
		ptMt ++;
	}
	LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);
	TMtadpHeadMsg tMtadpHeadMsg;
	if( strcmp(lpszPeerVerId, "3.6") == 0 || strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "8") == 0 || strcmp(lpszPeerVerId, "9") == 0 ||
		strcmp(lpszPeerVerId, "5") == 0
		)
	{
		//cMsg.CatMsgBody((u8*)&tMtadpHeadMsg, sizeof(TMtadpHeadMsg));
	}
	else
	{
		cMsg.CatMsgBody((u8*)&tMtadpHeadMsg, sizeof(TMtadpHeadMsg));
	}
	TNonStandardReq tReq;
	tReq.m_aszUserName[0] = 0;
	tReq.m_aszUserPass[0] = 0;
	tReq.m_nReqID = m_dwMMcuReqId++;
	tReq.m_nMsgLen = cMsg.GetServMsgLen();

	if( (u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf) )
	{
		StaticLog( "[MTADAP][EXP]StartMixerCmd message is too large(len:%d)\n", tReq.m_nMsgLen);
		return;
	}

	memcpy( tReq.m_abyMsgBuf, cMsg.GetServMsg(), tReq.m_nMsgLen );
    
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq) );

	return;
}

/*=============================================================================
  函 数 名： OnSendStartMixerNotif
  功    能： 
  算法实现： 
  全局变量： 
  参    数：  CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendStartMixerNotif( CServMsg& cServMsg, HCHAN hsChan )
{
	CServMsg cMsg;
	cMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
	u8 byDepth = *(cServMsg.GetMsgBody()+sizeof(TMcu));
	cMsg.SetMsgBody((u8*)&byDepth, sizeof(u8));

	TNonStandardMsg tMsg;
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][EXP]OnSendStartMixerNotif message is too large(len:%d)\n", tMsg.m_nMsgLen);
		return;
	}
	memcpy(tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen);
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg));
	
	return;
}

/*=============================================================================
    函 数 名： OnSendStopMixerCmd
    功    能： 发送 上级MCU停止下级MCU会议讨论命令
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/5/27   3.6			万春雷                  创建
=============================================================================*/
void CMtAdpInst::OnSendStopMixerCmd(CServMsg& cServMsg, HCHAN hsChan)
{
	CServMsg cMsg;
	cMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
	TNonStandardReq tReq;
	tReq.m_aszUserName[0] = 0;
	tReq.m_aszUserPass[0] = 0;
	tReq.m_nReqID = m_dwMMcuReqId ++;
	tReq.m_nMsgLen = cMsg.GetServMsgLen();
	if((u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][EXP]OnSendStopMixerCmd message is too large(len:%d)\n", tReq.m_nMsgLen);
		return;
	}
	memcpy(tReq.m_abyMsgBuf, cMsg.GetServMsg(), tReq.m_nMsgLen);
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq));

	return;
}

/*=============================================================================
  函 数 名： OnSendStopMixerNotif
  功    能： 
  算法实现： 
  全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendStopMixerNotif(CServMsg& cServMsg, HCHAN hsChan)
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );

	TNonStandardMsg tMsg;
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][EXP]OnSendStopMixerNotif message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}

	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );
	
	return;
}
/*=============================================================================
    函 数 名： OnSendAdjustMtResAck
    功    能： 发送 上级MCU调整下级MCU下直连MT分辨率
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    4/10/2009   4.6			薛亮                  创建
=============================================================================*/
void CMtAdpInst::OnSendAdjustMtResAck(CServMsg& cServMsg, HCHAN hsChan)
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );

	TNonStandardRsp tRsp;
	tRsp.m_nReqID      = 0;
	tRsp.m_emReturnVal = emReturnValue_Ok;

	//modify msg body
	//BOOL32 bLocal = FALSE;
	TPartID tPartID;
	TMtadpHeadMsg tMtadpHeadMsg;
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody());
	TMt tMt = *(TMt *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));
	
	BOOL32 bMcuIsSupportMultiCascade = TRUE;
	LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);


	/*if( strcmp(lpszPeerVerId, "10") == 0 )
	{		
		bMcuIsSupportMultiCascade = TRUE;
		MtAdpInfo("[ProcMcuMcuAdjMtResReq] mcu %s version,IsSupportMultiCascade\n", lpszPeerVerId);		
	}*/
	if( strcmp(lpszPeerVerId, "3.6") == 0 || strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "8") == 0 || strcmp(lpszPeerVerId, "9") == 0 ||
		strcmp(lpszPeerVerId, "5") == 0
		)
	{
		bMcuIsSupportMultiCascade = FALSE;
	}
	

	if( LOCAL_MCUID != tMt.GetMcuId() )
	{
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[tHeadMsg.m_tMsgSrc.m_byCasLevel] = tMt.GetMtId();
		++tHeadMsg.m_tMsgSrc.m_byCasLevel;
		tMt.SetMtId( (u8)tMt.GetMcuId() );
		tMt.SetMcuId( LOCAL_MCUID );
	}
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
	}
	
	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = GetPartIdFromMt( tMt, &tMtadpHeadMsg.m_tMsgSrc.m_bLocal );

	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = htonl( tMtadpHeadMsg.m_tMsgSrc.m_dwPID );


	HeadMsgToMtadpHeadMsg( tHeadMsg,tMtadpHeadMsg );

	
	//cMsg.CatMsgBody( (u8 *)&tPartID, sizeof(TPartID) );
	if( bMcuIsSupportMultiCascade )
	{
		cMsg.SetMsgBody( (u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg) );
	}
	else
	{
		tPartID.m_dwPID = tMtadpHeadMsg.m_tMsgSrc.m_dwPID;
		tPartID.m_bLocal = tMtadpHeadMsg.m_tMsgSrc.m_bLocal;
		cMsg.SetMsgBody( (u8 *)&tPartID, sizeof(tPartID) );
	}
		
	cMsg.CatMsgBody( (u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg)), cServMsg.GetMsgBodyLen() - sizeof(TMt) - sizeof(TMsgHeadMsg));
	

	tRsp.m_nMsgLen = cMsg.GetServMsgLen();
	if( (u32)tRsp.m_nMsgLen > sizeof(tRsp.m_abyMsgBuf) )
	{
		StaticLog( "[MTADAP][EXP]OnSendAdjustMtResAck message is too large(len:%d)\n", tRsp.m_nMsgLen );
		return;
	}
	
	memcpy(tRsp.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());
	
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_RSP, &tRsp, sizeof(tRsp) );
	
	return;

}

/*=============================================================================
    函 数 名： OnSendAdjustMtResNack
    功    能： 发送 上级MCU调整下级MCU下直连MT分辨率
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    4/10/2009   4.6			薛亮                  创建
=============================================================================*/
void CMtAdpInst::OnSendAdjustMtResNack(CServMsg& cServMsg, HCHAN hsChan)
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );

	TNonStandardRsp tRsp;
	tRsp.m_nReqID      = 0;
	tRsp.m_emReturnVal = emReturnValue_Error;

	BOOL32 bMcuIsSupportMultiCascade = TRUE;
	LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);

	/*if( strcmp(lpszPeerVerId, "10") == 0 )
	{		
		bMcuIsSupportMultiCascade = TRUE;
		MtAdpInfo("[ProcMcuMcuAdjMtResReq] mcu %s version,IsSupportMultiCascade\n", lpszPeerVerId);		
	}*/

	if( strcmp(lpszPeerVerId, "3.6") == 0 || strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "8") == 0 || strcmp(lpszPeerVerId, "9") == 0 ||
		strcmp(lpszPeerVerId, "5") == 0
		)
	{
		bMcuIsSupportMultiCascade = FALSE;
	}


	//modify msg body
	//BOOL32 bLocal = FALSE;
	TPartID tPartID;
	TMtadpHeadMsg tMtadpHeadMsg;
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody());
	TMt tMt = *(TMt *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
	
	

	if( LOCAL_MCUID != tMt.GetMcuId() )
	{
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[tHeadMsg.m_tMsgSrc.m_byCasLevel] = tMt.GetMtId();
		++tHeadMsg.m_tMsgSrc.m_byCasLevel;
		tMt.SetMtId( (u8)tMt.GetMcuId() );
		tMt.SetMcuId( LOCAL_MCUID );
	}
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
	}

	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = GetPartIdFromMt( tMt, &tMtadpHeadMsg.m_tMsgSrc.m_bLocal );

	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = htonl( tMtadpHeadMsg.m_tMsgSrc.m_dwPID );

	HeadMsgToMtadpHeadMsg( tHeadMsg,tMtadpHeadMsg );

	//cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	if( bMcuIsSupportMultiCascade )
	{
		cMsg.SetMsgBody( (u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg) );
	}
	else
	{
		tPartID.m_dwPID = tMtadpHeadMsg.m_tMsgSrc.m_dwPID;
		tPartID.m_bLocal = tMtadpHeadMsg.m_tMsgSrc.m_bLocal;
		cMsg.SetMsgBody( (u8 *)&tPartID, sizeof(tPartID) );
	}
	
	cMsg.CatMsgBody( (u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg) ), cServMsg.GetMsgBodyLen() - sizeof(TMt) - sizeof(TMsgHeadMsg));
	//cMsg.CatMsgBody( (u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg) );
	
	tRsp.m_nMsgLen = cMsg.GetServMsgLen();
	if( (u32)tRsp.m_nMsgLen > sizeof(tRsp.m_abyMsgBuf) )
	{
		StaticLog( "[MTADAP][EXP]OnSendAdjustMtResNack message is too large(len:%d)\n", tRsp.m_nMsgLen );
		return;
	}
	
	memcpy(tRsp.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());

	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_RSP, &tRsp, sizeof(tRsp) );

	return;
}

/*=============================================================================
    函 数 名： OnSendAdjustMtFpsReq
    功    能： 发送 上级MCU调整下级MCU下直连MT帧率
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    20100423    4.6			pengjie                create
=============================================================================*/
void CMtAdpInst::OnSendAdjustMtFpsReq( CServMsg &cServMsg, HCHAN hsChan )
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	TNonStandardReq tReq;
	tReq.m_aszUserName[0] = 0;
	tReq.m_aszUserPass[0] = 0;
	tReq.m_nReqID = m_dwMMcuReqId++;

	// [pengjie 2010/8/23] 调分辨率，三级级联支持
	BOOL32 bMcuIsSupportMultiCascade = TRUE;
	LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);

	/*if( strcmp(lpszPeerVerId, "10") == 0 )
	{		
		bMcuIsSupportMultiCascade = TRUE;
		MtAdpInfo("[ProcMcuMcuAdjMtResReq] mcu %s version,IsSupportMultiCascade\n", lpszPeerVerId);		
	}*/

	if( strcmp(lpszPeerVerId, "3.6") == 0 || strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "8") == 0 || strcmp(lpszPeerVerId, "9") == 0 ||
		strcmp(lpszPeerVerId, "5") == 0
		)
	{
		bMcuIsSupportMultiCascade = FALSE;
	}
	
	//BOOL32 bLocal = FALSE;
	TPartID tPartID;
	TMt tMt = *(TMt *)cServMsg.GetMsgBody();

	if( TRUE == bMcuIsSupportMultiCascade)
	{
		TMtadpHeadMsg tMdadpHeadMsg;
		TMsgHeadMsg tHeadMsg;

		tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody() + sizeof(TMt) + (sizeof(u8) * 3) );
		tMdadpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tMt, &tMdadpHeadMsg.m_tMsgDst.m_bLocal );
		tMdadpHeadMsg.m_tMsgDst.m_dwPID = htonl( tMdadpHeadMsg.m_tMsgDst.m_dwPID );
		HeadMsgToMtadpHeadMsg( tHeadMsg,tMdadpHeadMsg );

		cMsg.SetMsgBody();
		cMsg.SetMsgBody( (u8 *)(cServMsg.GetMsgBody() + sizeof(TMt)), (sizeof(u8) * 3) );
		cMsg.CatMsgBody( (u8 *)&tMdadpHeadMsg, sizeof(TMtadpHeadMsg) );
	}
	else
	{
		tPartID.m_dwPID = GetPartIdFromMt( tMt, &tPartID.m_bLocal );
		cMsg.SetMsgBody();
		cMsg.SetMsgBody( (u8 *)&tPartID, sizeof(TPartID) );
	    cMsg.CatMsgBody( (u8 *)(cServMsg.GetMsgBody() + sizeof(TMt)), cServMsg.GetMsgBodyLen() - sizeof(TMt) );
	}
	// End


// 	tPartID.m_dwPID = GetPartIdFromMt( tMt, &tPartID.m_bLocal );
// 	cMsg.SetMsgBody();
// 	cMsg.SetMsgBody( (u8 *)&tPartID, sizeof(TPartID) );
// 	cMsg.CatMsgBody( (u8 *)(cServMsg.GetMsgBody() + sizeof(TMt)), cServMsg.GetMsgBodyLen() - sizeof(TMt) );
	
	tReq.m_nMsgLen = cMsg.GetServMsgLen();
	if((u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][OnSendAdjustMtFpsReq] message is too large(len:%d)\n", tReq.m_nMsgLen);
		return;
	}
	
	memcpy(tReq.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq));
	StaticLog( " [MTADAP][OnSendAdjustMtFpsReq] change Fps mcuid: %d,mtid: %d\n",
		           tMt.GetMcuId(), tMt.GetMtId());
	
	return;
}

/*=============================================================================
    函 数 名： OnSendAdjustMtFpsAck
    功    能： 发送 上级MCU调整下级MCU下直连MT帧率的ack
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    20100423    4.6			pengjie                create
=============================================================================*/
void CMtAdpInst::OnSendAdjustMtFpsAck( CServMsg &cServMsg, HCHAN hsChan )
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );

	TNonStandardRsp tRsp;
	tRsp.m_nReqID      = 0;
	tRsp.m_emReturnVal = emReturnValue_Ok;

	//modify msg body
	//BOOL32 bLocal = FALSE;
	TPartID tPartID;
	TMt tMt = *(TMt *)cServMsg.GetMsgBody();
	
	tPartID.m_dwPID = GetPartIdFromMt( tMt, &tPartID.m_bLocal );
	cMsg.SetMsgBody();
	cMsg.SetMsgBody( (u8 *)&tPartID, sizeof(TPartID) );
	cMsg.CatMsgBody( (u8 *)(cServMsg.GetMsgBody() + sizeof(TMt)), cServMsg.GetMsgBodyLen() - sizeof(TMt) );
	
	tRsp.m_nMsgLen = cMsg.GetServMsgLen();
	if( (u32)tRsp.m_nMsgLen > sizeof(tRsp.m_abyMsgBuf) )
	{
		StaticLog( "[MTADAP][OnSendAdjustMtFpsAeq] message is too large(len:%d)\n", tRsp.m_nMsgLen );
		return;
	}
	
	memcpy(tRsp.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_RSP, &tRsp, sizeof(tRsp) );
	StaticLog( " [MTADAP][OnSendAdjustMtFpsAck] change Fps mcuid: %d,mtid: %d\n",
		           tMt.GetMcuId(), tMt.GetMtId());
	
	return;
}

/*=============================================================================
    函 数 名： OnSendAdjustMtFpsNack
    功    能： 发送 上级MCU调整下级MCU下直连MT帧率的nack
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    20100423    4.6			pengjie                create
=============================================================================*/
void CMtAdpInst::OnSendAdjustMtFpsNack( CServMsg &cServMsg, HCHAN hsChan )
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	
	TNonStandardRsp tRsp;
	tRsp.m_nReqID      = 0;
	tRsp.m_emReturnVal = emReturnValue_Error;
	
	//modify msg body
	//BOOL32 bLocal = FALSE;
	TPartID tPartID;
	TMt tMt = *(TMt *)cServMsg.GetMsgBody();
	
	tPartID.m_dwPID = GetPartIdFromMt( tMt, &tPartID.m_bLocal );
	cMsg.SetMsgBody();
	cMsg.SetMsgBody( (u8 *)&tPartID, sizeof(TPartID) );
	cMsg.CatMsgBody( (u8 *)(cServMsg.GetMsgBody() + sizeof(TMt)), cServMsg.GetMsgBodyLen() - sizeof(TMt) );
	
	tRsp.m_nMsgLen = cMsg.GetServMsgLen();
	if( (u32)tRsp.m_nMsgLen > sizeof(tRsp.m_abyMsgBuf) )
	{
		StaticLog( "[MTADAP][OnSendAdjustMtFpsNack] message is too large(len:%d)\n", tRsp.m_nMsgLen );
		return;
	}
	
	memcpy(tRsp.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_RSP, &tRsp, sizeof(tRsp) );
	StaticLog( " [MTADAP][OnSendAdjustMtFpsNack] change Fps mcuid: %d,mtid: %d\n",
		           tMt.GetMcuId(), tMt.GetMtId());
	
	return;
}

/*=============================================================================
    函 数 名： OnSendMMcuFeccCmd
    功    能： 级联远摇
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    20100812    4.6			pengjie                create
=============================================================================*/
void CMtAdpInst::OnSendMMcuFeccCmd( CServMsg &cServMsg, HCHAN hsChan )
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );

	TMtadpHeadMsg tMdadpHeadMsg;	
	TMsgHeadMsg tHeadMsg;

	TNonStandardMsg tCmd;
	//TPartID tPartID;
	TMt tMt = *(TMt *)cServMsg.GetMsgBody();
	u8 byFeccParam =  *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt));
	u16 wFeccEvenId = *(u16 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8));
	tHeadMsg = *(TMsgHeadMsg *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u16));
//	tPartID.m_dwPID = GetPartIdFromMt( tMt, &tPartID.m_bLocal );

	tMdadpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tMt, &tMdadpHeadMsg.m_tMsgDst.m_bLocal );
	tMdadpHeadMsg.m_tMsgDst.m_dwPID = htonl( tMdadpHeadMsg.m_tMsgDst.m_dwPID );
	HeadMsgToMtadpHeadMsg( tHeadMsg,tMdadpHeadMsg );

	cMsg.SetMsgBody();
//	cMsg.SetMsgBody( (u8 *)&tPartID, sizeof(tPartID) );
//	cMsg.CatMsgBody( (u8 *)&byFeccParam, sizeof(byFeccParam) );
//	cMsg.CatMsgBody( (u8 *)&wFeccEvenId, sizeof(wFeccEvenId) );
	cMsg.SetMsgBody( (u8 *)&byFeccParam, sizeof(byFeccParam) );
	cMsg.CatMsgBody( (u8 *)&wFeccEvenId, sizeof(wFeccEvenId) );
	cMsg.CatMsgBody( (u8 *)&tMdadpHeadMsg, sizeof(tMdadpHeadMsg) );

	tCmd.m_nMsgLen = cMsg.GetServMsgLen();
	if( (u32)tCmd.m_nMsgLen > sizeof(tCmd.m_abyMsgBuf) )
	{
		StaticLog( "[MTADAP][OnSendMMcuFeccCmd] message is too large(len:%d)\n", tCmd.m_nMsgLen );
		return;
	}

	memcpy(tCmd.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tCmd, sizeof(tCmd) );
	StaticLog( " [MTADAP][OnSendMMcuFeccCmd] change Fps mcuid: %d,mtid: %d\n",
		           tMt.GetMcuId(), tMt.GetMtId());

	return;
}

/*=============================================================================
    函 数 名： OnSendAdjustMtBitrateCmd
    功    能： 发送 上级MCU调整下级MCU下直连MT码率
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2012/05/12   4.7.1		周晶晶                 创建
=============================================================================*/
void CMtAdpInst::OnSendAdjustMtBitrateCmd(CServMsg& cServMsg, HCHAN hsChan)
{	
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	TNonStandardReq tReq;
	tReq.m_aszUserName[0] = 0;
	tReq.m_aszUserPass[0] = 0;
	tReq.m_nReqID = m_dwMMcuReqId++;
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody() );
	TMt tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
	
	TMtadpHeadMsg tMdadpHeadMsg;
	tMdadpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tMt, &tMdadpHeadMsg.m_tMsgDst.m_bLocal );

	tMdadpHeadMsg.m_tMsgDst.m_dwPID = htonl( tMdadpHeadMsg.m_tMsgDst.m_dwPID );

	HeadMsgToMtadpHeadMsg( tHeadMsg,tMdadpHeadMsg );
	

	cMsg.SetMsgBody( (u8 *)&tMdadpHeadMsg, sizeof(tMdadpHeadMsg) );

	u8 bIsRecover = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg) );
	u16 wBitRate = *(u16 *)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg) +sizeof(u8) );
	wBitRate = ntohs(wBitRate);
	cMsg.CatMsgBody( (u8 *)&bIsRecover, sizeof(bIsRecover) );
	wBitRate = htons(wBitRate);
	cMsg.CatMsgBody( (u8 *)&wBitRate, sizeof(wBitRate) );

	tReq.m_nMsgLen = cMsg.GetServMsgLen();
	if((u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][EXP]OnSendAdjustMtResReq message is too large(len:%d)\n", tReq.m_nMsgLen);
		return;
	}

	memcpy(tReq.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq));
	
	
}

/*=============================================================================
    函 数 名： OnSendAdjustMtResReq
    功    能： 发送 上级MCU调整下级MCU下直连MT分辨率
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    4/10/2009   4.6			薛亮                  创建
=============================================================================*/
void CMtAdpInst::OnSendAdjustMtResReq(CServMsg& cServMsg, HCHAN hsChan)
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	TNonStandardReq tReq;
	tReq.m_aszUserName[0] = 0;
	tReq.m_aszUserPass[0] = 0;
	tReq.m_nReqID = m_dwMMcuReqId++;

	BOOL32 bMcuIsSupportMultiCascade = TRUE;
	LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);

	/*if( strcmp(lpszPeerVerId, "10") == 0 )
	{		
		bMcuIsSupportMultiCascade = TRUE;
		MtAdpInfo("[ProcMcuMcuAdjMtResReq] mcu %s version,IsSupportMultiCascade\n", lpszPeerVerId);		
	}*/

	if( strcmp(lpszPeerVerId, "3.6") == 0 || strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "8") == 0 || strcmp(lpszPeerVerId, "9") == 0 ||
		strcmp(lpszPeerVerId, "5") == 0
		)
	{
		bMcuIsSupportMultiCascade = FALSE;
	}
	
	//modify msg body
	//BOOL32 bLocal = FALSE;
	//TPartID tPartID;
	TMtadpHeadMsg tMdadpHeadMsg;
	
	TMsgHeadMsg tHeadMsg;
	//if( cServMsg.GetMsgBodyLen() > ( sizeof( TMt ) + 4 * sizeof(u8) ) )
	//{
		tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody() );//+ sizeof(TMt) + 4 * sizeof(u8) );
	//}
	/*if( cServMsg.GetMsgBodyLen() > ( sizeof( TMt ) + 2 * sizeof(u8) ) &&
		cServMsg.GetMsgBodyLen() == ( sizeof( TMt ) + 2 * sizeof(u8)  + sizeof(TMsgHeadMsg) ) )
	{
		tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody() + sizeof(TMt) + 2 * sizeof(u8) );
	}*/
	
	TMt tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );//tHeadMsg.m_tMsgDst.m_tMt;//*(TMt *)cServMsg.GetMsgBody();
	
	tMdadpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tMt, &tMdadpHeadMsg.m_tMsgDst.m_bLocal );

	tMdadpHeadMsg.m_tMsgDst.m_dwPID = htonl( tMdadpHeadMsg.m_tMsgDst.m_dwPID );


	StaticLog( "[Mtadp][OnSendAdjustMtResReq][mmcu]tmt info--mcuId: %u, mtId: %u\n", tMt.GetMcuId(), tMt.GetMtId() );
	StaticLog( "[Mtadp][OnSendAdjustMtResReq][mmcu]TpartId info--PID: %d, bLocal: %d\n",
		tMdadpHeadMsg.m_tMsgDst.m_dwPID, tMdadpHeadMsg.m_tMsgDst.m_bLocal);

	HeadMsgToMtadpHeadMsg( tHeadMsg,tMdadpHeadMsg );
	cMsg.SetMsgBody();
	
	if( bMcuIsSupportMultiCascade )
	{
		cMsg.SetMsgBody( (u8 *)&tMdadpHeadMsg, sizeof(tMdadpHeadMsg) );		
	}
	else
	{
		TPartID tPartID;
		tPartID.m_dwPID = tMdadpHeadMsg.m_tMsgDst.m_dwPID;
		tPartID.m_bLocal = tMdadpHeadMsg.m_tMsgDst.m_bLocal;
		cMsg.SetMsgBody( (u8 *)&tPartID, sizeof(TPartID) );	
	}
	cMsg.CatMsgBody( (u8 *)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg) ), 
			cServMsg.GetMsgBodyLen() - sizeof(TMt) - sizeof(TMsgHeadMsg)  
			);	
		

	tReq.m_nMsgLen = cMsg.GetServMsgLen();
	if((u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][EXP]OnSendAdjustMtResReq message is too large(len:%d)\n", tReq.m_nMsgLen);
		return;
	}

	memcpy(tReq.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq));
	
	return;
}

/*=============================================================================
    函 数 名： OnSendAudMuteReq
    功    能： 发送 上级MCU调整下级MCU下直连MT静音哑音
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    10/08/13   4.6			薛亮                  创建
=============================================================================*/
void CMtAdpInst::OnSendAudMuteReq( CServMsg &cServMsg, HCHAN hsChan )
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );

	TNonStandardReq tReq;
	tReq.m_aszUserName[0] = 0;
	tReq.m_aszUserPass[0] = 0;
	tReq.m_nReqID = m_dwMMcuReqId++;
	
	//modify msg body
	//BOOL32 bLocal = FALSE;
	TPartID tPartID;
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TMt tMt = *(TMt *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));
	
	tPartID.m_dwPID = GetPartIdFromMt( tMt, &tPartID.m_bLocal );

	tPartID.m_dwPID = htonl( tPartID.m_dwPID );


	StaticLog( "[Mtadp][OnSendAudMuteReq][mmcu]tmt info--mcuId: %u, mtId: %u\n", tMt.GetMcuId(), tMt.GetMtId() );
	StaticLog( "[Mtadp][OnSendAudMuteReq][mmcu]TpartId info--PID: %d, bLocal: %d\n",
		tPartID.m_dwPID, tPartID.m_bLocal);

	cMsg.SetMsgBody();
	cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cMsg.CatMsgBody( (u8 *)&tPartID, sizeof(TPartID) );
	cMsg.CatMsgBody( (u8 *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof( TMt)),
		cServMsg.GetMsgBodyLen() - sizeof(TMt) - sizeof(TMsgHeadMsg) );

	tReq.m_nMsgLen = cMsg.GetServMsgLen();
	if((u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][EXP]OnSendAudMuteReq message is too large(len:%d)\n", tReq.m_nMsgLen);
		return;
	}

	memcpy(tReq.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq));
	
	return;
}

/*=============================================================================
    函 数 名： OnSendGetMixerParamReq
    功    能： 发送 上级MCU获取下级MCU会议讨论参数请求
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/5/27   3.6			万春雷                  创建
=============================================================================*/
void CMtAdpInst::OnSendGetMixerParamReq(CServMsg& cServMsg, HCHAN hsChan)
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	TNonStandardReq tReq;
	tReq.m_aszUserName[0] = 0;
	tReq.m_aszUserPass[0] = 0;
	tReq.m_nReqID = m_dwMMcuReqId++;
	tReq.m_nMsgLen = cMsg.GetServMsgLen();
	if((u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][EXP]OnSendGetMixerParamReq message is too large(len:%d)\n", tReq.m_nMsgLen);
		return;
	}
	memcpy(tReq.m_abyMsgBuf, cMsg.GetServMsg(), tReq.m_nMsgLen);
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq));
	
	return;
}

/*=============================================================================
  函 数 名： OnSendGetMixerParamAck
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容

=============================================================================*/
// void CMtAdpInst::OnSendGetMixerParamAck(CServMsg& cServMsg, HCHAN hsChan)
// {
// 	CServMsg cMsg;
// 	cMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
// 	TMixParam *ptParam = (TMixParam *)( cServMsg.GetMsgBody() + sizeof(TMcu) );
// 	u32    dwPartId = 0 ;
// 	BOOL32 bLocal   = TRUE;
// 
//     // FIXME: 本部分逻辑已经取消了
//     /*
// 	cMsg.SetMsgBody((u8 *)&ptParam->m_byMixMode, sizeof(u8));
// 
// 	for( s32 nIndex = 0; nIndex < ptParam->m_byMemberNum; nIndex ++ )
// 	{
// 		dwPartId = GetPartIdFromMt( ptParam->m_atMtMember[nIndex], &bLocal );
// 		dwPartId = htonl(dwPartId);
// 		cMsg.CatMsgBody( (u8 *)&dwPartId, sizeof(u32) );
// 	}
// 
// 	TNonStandardRsp tRsp;
// 	tRsp.m_nReqID      = 0;
// 	tRsp.m_emReturnVal = emReturnValue_Ok;
// 	tRsp.m_nMsgLen     = cMsg.GetServMsgLen();
// 
// 	if( tRsp.m_nMsgLen > sizeof(tRsp.m_abyMsgBuf) )
// 	{
// 		OspPrintf( TRUE, FALSE, "[MTADAP][EXP]OnSendGetMixerParamAck message is too large(len:%d)\n", tRsp.m_nMsgLen);
// 		return;
// 	}
// 	memcpy( tRsp.m_abyMsgBuf, cMsg.GetServMsg(), tRsp.m_nMsgLen );
// 	kdvSendMMcuCtrlMsg(hsChan, H_CASCADE_NONSTANDARD_RSP, &tRsp, sizeof(tRsp));
// 	*/
// 
// 	return;
// }

/*=============================================================================
  函 数 名： OnSendGetMixerParamNack
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendGetMixerParamNack(CServMsg& cServMsg, HCHAN hsChan)
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );

	TNonStandardRsp tRsp;
	tRsp.m_nReqID      = 0;
	tRsp.m_emReturnVal = emReturnValue_Error;
	tRsp.m_nMsgLen     = cMsg.GetServMsgLen();

	if( (u32)tRsp.m_nMsgLen > sizeof(tRsp.m_abyMsgBuf) )
	{
		StaticLog( "[MTADAP][EXP]OnSendGetMixerParamNack message is too large(len:%d)\n", tRsp.m_nMsgLen );
		return;
	}
	memcpy( tRsp.m_abyMsgBuf, cMsg.GetServMsg(), tRsp.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_RSP, &tRsp, sizeof(tRsp) );
	
	return;
}

/*=============================================================================
  函 数 名： OnSendMixerParamNotif
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendMixerParamNotif( CServMsg& cServMsg, HCHAN hsChan )
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );

	TMixParam *ptParam = (TMixParam *)( cServMsg.GetMsgBody() + sizeof(TMcu) );
	//u32    dwPartId = 0 ;
	//BOOL32 bLocal   = TRUE;
	
	BOOL32 bMcuAfterR3Ver = TRUE;
    LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);
    if (strcmp(lpszPeerVerId, "3.6") == 0 )
    {
        bMcuAfterR3Ver = FALSE;
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendMtStatusNtf] mcu %s version not support mixing status\n", lpszPeerVerId);
    }
	
	// [miaoqingsong 20111028] 对于R3及以前的版本，是通过上报TDiscussParam给MCS来刷新混音状态的。
	// 这里添加对R3版本TDiscussParam状态的支持，以解决R3与R6版本的兼容问题
	if ( strcmp(lpszPeerVerId, "5") == 0 || strcmp(lpszPeerVerId, "4.0") == 0 )
	{
		u8 byMode = ptParam->GetMode();
		cMsg.SetMsgBody( (u8 *)&byMode, sizeof(u8) );
		u8 byMemberNum = ptParam->GetMemberNum();
		
		for( u8 byIdx = 0; byIdx < min(byMemberNum, MAXNUM_MIXER_DEPTH); byIdx++ )
		{
			u32 dwPartId = 0;
			TMt tMt;
			BOOL32 bLocal = TRUE;
			tMt.SetMcuEqp(LOCAL_MCUID,ptParam->GetMemberByIdx(byIdx),TYPE_MT);
			tMt.SetConfIdx(m_byConfIdx);
			dwPartId = GetPartIdFromMt( tMt, &bLocal );
			dwPartId = htonl(dwPartId);
			cMsg.CatMsgBody( (u8 *)&dwPartId, sizeof(u32) );
		}
		
		TNonStandardMsg tMsg;
		tMsg.m_nMsgLen = cMsg.GetServMsgLen();
		if( (u32) tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
		{
			StaticLog( "[MTADAP][EXP]OnSendMixerParamNotif message is too large(len:%d)\n", tMsg.m_nMsgLen );
			return;
		}
		
		memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
		kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );
		
		return;
	}

	if (!bMcuAfterR3Ver)
	{
		//对于R3以前的版本，是通过上报的TDiscussParam来刷新混音状态的。
		//此处兼容上级是r2mcu,需要上报TDiscussParamV4R4B2
		TDiscussParamV4R4B2 tParam;
		tParam.m_byMixMode = ptParam->GetMode();
		tParam.m_byMemberNum = ptParam->GetMemberNum();
		for( s32 nIndex = 0; nIndex < min(tParam.m_byMemberNum/*ptParam->GetMemberNum()*/,MAXNUM_MIXER_DEPTH); nIndex ++ )
		{
			TMt tMt;
			tMt.SetMcuEqp(LOCAL_MCUID,ptParam->GetMemberByIdx(nIndex),TYPE_MT);
			tMt.SetConfIdx(m_byConfIdx);
			tParam.m_atMtMember[nIndex] = tMt;
		}
		cMsg.SetMsgBody((u8 *)&tParam, sizeof(tParam));
		
		TNonStandardMsg tMsg;
		tMsg.m_nMsgLen = cMsg.GetServMsgLen();
		
		if( (u32) tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
		{
			StaticLog( "[MTADAP][EXP]OnSendMixerParamNotif message is too large(len:%d)\n", tMsg.m_nMsgLen );
			return;
		}
		memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
		
		kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );

		return;
	}
	else
	{
		//对于R3之后版本，是通过TMtStatus来上报状态的。
		//此mixparam状态已经无效
		//直接忽略上报
		
		TMixParam tParam;
		cMsg.SetEventId(0);
		
		/*
		TMixParam tParam;
		tParam.SetMode(*((u8 *)cServMsg.GetMsgBody()));
		
		  s32 nMtNum = ( cServMsg.GetMsgBodyLen() - sizeof(u8) ) / sizeof(u32);
		  u32 *pdwPartId = (u32 *)( cServMsg.GetMsgBody() + sizeof(u8) );
		  s32 nIndex = 0;
		  for( nIndex = 0; nIndex < nMtNum; )
		  {
		  //过滤掉上级mcu本身在下级mcu的成员表现
		  if( REMOTE_MCU_PARTID != *pdwPartId  && nIndex < MAXNUM_MIXER_DEPTH )
		  {
		  tParam.m_atMtMember[nIndex] = GetMtFromPartId(ntohl(*pdwPartId), FALSE);
		  nIndex ++ ;
		  }
		  pdwPartId ++ ;
		  }
		  tParam.m_byMemberNum = nIndex;
		*/
		
		cMsg.CatMsgBody((u8 *)&tParam, sizeof(tParam));
	}


	return;
}

/*=============================================================================
  函 数 名： OnSendAddMixerMemberCmd
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendAddMixerMemberCmd(CServMsg& cServMsg, HCHAN hsChan)
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	TMtadpHeadMsg tMtadpHeadMsg;

	u8 byReplace = 0;
	if (cServMsg.GetMsgBodyLen() <= sizeof(u8))//以免消息体中只有一个byReplace字段,而被解析成非跨级终端个数
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[OnSendAddMixerMemberCmd]warning cServMsg.GetMsgBodyLen(%d)\n", cServMsg.GetMsgBodyLen() );
		return;
	}
	else
	{
		byReplace = *(u8*)(cServMsg.GetMsgBody() + cServMsg.GetMsgBodyLen() - 1 );
	}
    
    // zbq [08/30/2007] 增机替换逻辑。保证兼容性。
	//TMt *ptMtStart     = (TMt *)(cServMsg.GetMsgBody()+sizeof(TMcu));
	//s32 nMtNum    = (cServMsg.GetMsgBodyLen()-sizeof(TMcu) - sizeof(u8) - sizeof(TMsgHeadMsg))/sizeof(TMt);

	TMtadpMultiCascadeMsg tMtadpMultiCascadeMsg;
	u32 dwPartId  = 0 ;
	BOOL32 bLocal = FALSE;
	u8  *pbyCurMsgPos = (u8*)cServMsg.GetMsgBody();  // 非跨级终端个数
	u8  bySecMcuLocalMtNum = (u8)(*pbyCurMsgPos);
	LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);
	pbyCurMsgPos += sizeof(u8);
	TMt *ptMt = NULL;

	if ( bySecMcuLocalMtNum > 0 )
	{
		ptMt = (TMt *)(pbyCurMsgPos);
		for ( u8 byMtIdx = 0; byMtIdx < bySecMcuLocalMtNum; byMtIdx ++, ptMt ++ )
		{
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendAddMixerMemberCmd]MtIdx(%d) SecMt(%d,%d)\n",byMtIdx + 1,ptMt->GetMcuId(),ptMt->GetMtId());
			
			//非跨级终端
			memset((u8*)&tMtadpMultiCascadeMsg,0,sizeof(TMtadpMultiCascadeMsg));
			if (ptMt == NULL)
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[OnSendAddMixerMemberCmd]SecMt warning ptMt == NULL!so return.\n");
				return;
			}

			dwPartId = GetPartIdFromMt(*ptMt, &bLocal);
			dwPartId = htonl(dwPartId);
			tMtadpMultiCascadeMsg.SetPartGid(dwPartId,TRUE);
			if ( atoi(lpszPeerVerId) >= 12 )
			{
				cMsg.CatMsgBody((u8*)&tMtadpMultiCascadeMsg, sizeof(TMtadpMultiCascadeMsg));
			}
			else//老版本MCU按照老的消息体组织
			{
				cMsg.CatMsgBody( (u8 *)&dwPartId, sizeof(u32) );
				//20101209_tzy  对下级是R3_FULL是开启定制混音要转化为MCU_MCU_STARTMIX_CMD消息，如果此时下级开启了混音器要先停掉
				//下级混音，因此发送完停下级混音消息后设置一个定时器，定时器到时再让下级开启级联定制，定时器避免时序问题，否则级联定制失败。
				if ((strcmp(lpszPeerVerId, "5") == 0 || strcmp(lpszPeerVerId, "mcu4.0 R3") == 0) && byReplace)
				{
					u16 dwMcuId = ptMt->GetMcuId();
					u8 byMmbId = ptMt->GetMtId();		
					u8   byByteIdx ,byBitIdx,byMask;
					if( byMmbId > MAXNUM_CONF_MT ||byMmbId==0)
						return;
					byMmbId--;
					byByteIdx = byMmbId>>3;
					byBitIdx  = byMmbId%8;
					byMask    = (unsigned int)1<<byBitIdx; 
					m_dwSmcuMixMemBeforeStart[byByteIdx] |= byMask;
					if (byMtIdx + 1 == bySecMcuLocalMtNum)
					{
						CServMsg cStopServMsg = cServMsg;
						cStopServMsg.SetEventId(MCU_MCU_STOPMIXER_CMD);
						TNonStandardReq tReq;
						tReq.m_aszUserName[0] = 0;
						tReq.m_aszUserPass[0] = 0;
						tReq.m_nReqID = m_dwMMcuReqId++;
						tReq.m_nMsgLen = cStopServMsg.GetServMsgLen();
						if( (u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf) )
						{
							StaticLog( "[MTADAP][EXP]StartMixerCmd message is too large(len:%d)\n", tReq.m_nMsgLen);
							return;
						}
						memcpy( tReq.m_abyMsgBuf, cStopServMsg.GetServMsg(), tReq.m_nMsgLen );
						kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq) );
						SetTimer( TIMER_STARTAFTERSTOPSMIX, 200 ,dwMcuId);
						return;
					}
				}
				if (byMtIdx + 1 == bySecMcuLocalMtNum)
				{
					cMsg.CatMsgBody((u8*)&byReplace, sizeof(u8));
					cMsg.CatMsgBody((u8*)&tMtadpHeadMsg, sizeof(TMtadpHeadMsg));
					tMtadpHeadMsg.m_tMsgDst.SetPartGid(0,TRUE);
					TNonStandardMsg tMsg;
					tMsg.m_nMsgLen = cMsg.GetServMsgLen();
					if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
					{
						StaticLog( "[MTADAP][EXP]OnSendAddMixerMemberCmd message is too large(len:%d)\n", tMsg.m_nMsgLen );
						return;
					}
					memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
					kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg));
				}
			}
		}
		pbyCurMsgPos = (u8*)ptMt;
	}

	if (cServMsg.GetMsgBodyLen() <= bySecMcuLocalMtNum * sizeof(TMt) + sizeof(u8)) // 以免取到byReplace字段
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[OnSendAddMixerMemberCmd]warning cServMsg.GetMsgBodyLen(%d) bySecMcuLocalMtNum(%d)\n",cServMsg.GetMsgBodyLen(),bySecMcuLocalMtNum);
		return;
	}

	s32 nMtNum = bySecMcuLocalMtNum;
	u8  byThdMcuNum = *pbyCurMsgPos; // 第三级MCU个数
	pbyCurMsgPos += sizeof(u8);
	for (u8 byThdMcuIdx = 0 ; byThdMcuIdx < byThdMcuNum;byThdMcuIdx ++)
	{
		//跨级终端
		memset((u8*)&tMtadpMultiCascadeMsg,0,sizeof(TMtadpMultiCascadeMsg));
		ptMt = (TMt *)pbyCurMsgPos;
		pbyCurMsgPos += sizeof(TMt);

		if ( ptMt == NULL )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[OnSendAddMixerMemberCmd]ThdMt warning ptMt == NULL!so return.\n");
			return;
		}
		
		u8  byThdMcuMtNum = *pbyCurMsgPos;
		pbyCurMsgPos += sizeof(u8);
		tMtadpMultiCascadeMsg.SetPartGid(htonl(GetPartIdFromMt(*ptMt, &bLocal)),bLocal);
		TMultiCacMtInfo *ptTempMultiCacMtInfo = (TMultiCacMtInfo*)pbyCurMsgPos;
		for (u8 byMtIdx = 0 ; byMtIdx < byThdMcuMtNum;byMtIdx ++,ptTempMultiCacMtInfo++)
		{	
			if (ptTempMultiCacMtInfo == NULL)
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[OnSendAddMixerMemberCmd]warning ptTempMultiCacMtInfo == NULL!so return.\n");
				return;
			}
			if (ptTempMultiCacMtInfo->m_byCasLevel < MAX_CASCADELEVEL)
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[OnSendAddMixerMemberCmd]warning ptTempMultiCacMtInfo->m_byCasLevel(%d) < MAX_CASCADELEVEL(%d)!so return.\n",
					ptTempMultiCacMtInfo->m_byCasLevel , MAX_CASCADELEVEL);
				return;
			}
			tMtadpMultiCascadeMsg.m_byCasLevel = tMtadpHeadMsg.m_tMsgDst.m_byCasLevel = ptTempMultiCacMtInfo->m_byCasLevel;
			ptTempMultiCacMtInfo->m_byCasLevel--;
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendAddMixerMemberCmd]MtIdx(%d) ThdMcu(%d,%d)Identify(%d)\n",nMtNum,ptMt->GetMcuId(),ptMt->GetMtId(),
				ptTempMultiCacMtInfo->m_abyMtIdentify[ptTempMultiCacMtInfo->m_byCasLevel]);
			if ( atoi(lpszPeerVerId) >= 12 )
			{	
				tMtadpMultiCascadeMsg.m_abyMtIdentify[ptTempMultiCacMtInfo->m_byCasLevel] 
					= ptTempMultiCacMtInfo->m_abyMtIdentify[ptTempMultiCacMtInfo->m_byCasLevel];
				cMsg.CatMsgBody((u8*)&tMtadpMultiCascadeMsg, sizeof(TMtadpMultiCascadeMsg));
			}
			else//老版本MCU按照老的消息体组织
			{	
				dwPartId = ptTempMultiCacMtInfo->m_abyMtIdentify[ptTempMultiCacMtInfo->m_byCasLevel];
				dwPartId = htonl(dwPartId);
				if (byMtIdx == 0)
				{
					cMsg.SetMsgBody( (u8 *)&dwPartId, sizeof(u32) );
				}
				else
				{
					cMsg.CatMsgBody( (u8 *)&dwPartId, sizeof(u32) );
				}
				if (byMtIdx + 1 == byThdMcuMtNum)
				{
					cMsg.CatMsgBody((u8*)&byReplace, sizeof(u8));
					tMtadpMultiCascadeMsg.GetPartGid(&dwPartId,&bLocal);
					tMtadpHeadMsg.m_tMsgDst.SetPartGid(dwPartId,bLocal);
					cMsg.CatMsgBody((u8*)&tMtadpHeadMsg, sizeof(TMtadpHeadMsg));	
					TNonStandardMsg tMsg;
					tMsg.m_nMsgLen = cMsg.GetServMsgLen();
					if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
					{
						StaticLog( "[MTADAP][EXP]OnSendAddMixerMemberCmd message is too large(len:%d)\n", tMsg.m_nMsgLen );
						return;
					}
					memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
					kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg));
					if ((byThdMcuIdx + 1) == byThdMcuNum && (byMtIdx + 1) == byThdMcuMtNum)
					{
						return;
					}
				}
			}
			nMtNum++;
		}
		pbyCurMsgPos = (u8*)ptTempMultiCacMtInfo;
	}

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendAddMixerMemberCmd]byReplace(%d)\n",byReplace);
	cMsg.CatMsgBody((u8*)&byReplace, sizeof(u8));

	TNonStandardMsg tMsg;
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();

	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][EXP]OnSendAddMixerMemberCmd message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg));

	s8 achBuff[1024] = {0};
	s8 *pBuff = (s8 *)achBuff;
	u16 wLen = 0;

	wLen += sprintf(pBuff, "OnSendAddMixerMemberCmd MsgLen: %d Bodylen:%d Mtnum:%d Replace:%d",
		cMsg.GetServMsgLen(),
		cMsg.GetMsgBodyLen(),
		nMtNum,
		byReplace
		);

	wLen += sprintf(pBuff + wLen, "\n");
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, achBuff);
	
	return;

	// miaoqingsong 级联定制混音新策略
	/*TMt *ptMt = ptMtStart;
	TMsgHeadMsg tMsgHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+ sizeof(TMt) * (u32)nMtNum + sizeof(TMcu) + sizeof(u8));
	TMtadpHeadMsg tMtadpHeadMsg;
	TMcu tMcu = *(TMcu *)cServMsg.GetMsgBody();
	if ( tMsgHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		for(s32 nIndex = 0; nIndex < nMtNum; nIndex ++ )
		{
			if (ptMt == NULL)
			{
				break;
			}
			dwPartId = ptMt->GetMtId();
			MtAdpInfo("[OnSendAddMixerMemberCmd] MMCU ADD Mt(%d)\n",dwPartId);
			dwPartId = htonl(dwPartId);
			cMsg.CatMsgBody( (u8 *)&dwPartId, sizeof(u32) );
			ptMt ++;
		}
		u32 dwPaId = GetPartIdFromMt(tMcu,FALSE);
		tMtadpHeadMsg.m_tMsgDst.SetPartGid(htonl(dwPaId),FALSE);
		tMtadpHeadMsg.m_tMsgDst.m_byCasLevel = tMsgHeadMsg.m_tMsgDst.m_byCasLevel;	
		//dwPaId = ntohl(dwPaId);
	} 
	else
	{
		memset( m_dwSmcuMixMemBeforeStart ,0 ,sizeof(m_dwSmcuMixMemBeforeStart) );
		for(s32 nIndex = 0; nIndex < nMtNum; nIndex ++ )
		{	
			if (ptMt == NULL)
			{
				break;
			}
			u16 wMcuId = ptMt->GetMcuId();		
			dwPartId = GetPartIdFromMt(*ptMt, &bLocal);
			MtAdpInfo("[OnSendAddMixerMemberCmd] PARTID(%d)\n",dwPartId);
			dwPartId = htonl(dwPartId);
			cMsg.CatMsgBody( (u8 *)&dwPartId, sizeof(u32) );
			//20101209_tzy  对下级是R3_FULL是开启定制混音要转化为MCU_MCU_STARTMIX_CMD消息，如果此时下级开启了混音器要先停掉
			//下级混音，因此发送完停下级混音消息后设置一个定时器，定时器到时再让下级开启级联定制，定时器避免时序问题，否则级联定制失败。
			LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);
			if (strcmp(lpszPeerVerId, "5") == 0 && byReplace)
			{
				u8 byMmbId = ptMt->GetMtId();		
				u8   byByteIdx ,byBitIdx,byMask;
				if( byMmbId > MAXNUM_CONF_MT ||byMmbId==0)
					return;
				byMmbId--;
				byByteIdx = byMmbId>>3;
				byBitIdx  = byMmbId%8;
				byMask    = 1 << byBitIdx; 
				m_dwSmcuMixMemBeforeStart[byByteIdx] |= byMask;
				if (nIndex + 1 == nMtNum)
				{
					CServMsg cStopServMsg = cServMsg;
					cStopServMsg.SetEventId(MCU_MCU_STOPMIXER_CMD);
					TNonStandardReq tReq;
					tReq.m_aszUserName[0] = 0;
					tReq.m_aszUserPass[0] = 0;
					tReq.m_nReqID = m_dwMMcuReqId++;
					tReq.m_nMsgLen = cStopServMsg.GetServMsgLen();
					if( (u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf) )
					{
						OspPrintf(TRUE, FALSE, "[MTADAP][EXP]StartMixerCmd message is too large(len:%d)\n", tReq.m_nMsgLen);
						return;
					}
					memcpy( tReq.m_abyMsgBuf, cStopServMsg.GetServMsg(), tReq.m_nMsgLen );
    
					kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq) );
					SetTimer( TIMER_STARTAFTERSTOPSMIX, 200 ,wMcuId);
					return;
				}
			}
			ptMt ++;
		}
		tMtadpHeadMsg.m_tMsgDst.SetPartGid(0,TRUE);
	}



// 	for(s32 nIndex = 0; nIndex < nMtNum; nIndex ++ )
// 	{
// 		dwPartId = GetPartIdFromMt(*ptMt, &bLocal);
// 		dwPartId = htonl(dwPartId);
// 		cMsg.CatMsgBody( (u8 *)&dwPartId, sizeof(u32) );
// 		ptMt ++;
// 	}

    cMsg.CatMsgBody((u8*)&byReplace, sizeof(u8));
//	cMsg.CatMsgBody((u8*)&byMtAutoInSpec, sizeof(u8));
    LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);


	/*if (strcmp(lpszPeerVerId, "10") == 0)
	{
		cMsg.CatMsgBody((u8*)&tMtadpHeadMsg, sizeof(TMtadpHeadMsg));
	} 
	else
	{
		cMsg.CatMsgBody((u8*)&tMtadpHeadMsg, sizeof(TMtadpHeadMsg));
	}*/

	/*if( strcmp(lpszPeerVerId, "3.6") == 0 || strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "8") == 0 || strcmp(lpszPeerVerId, "9") == 0 ||
		strcmp(lpszPeerVerId, "5") == 0
		)
	{
		//cMsg.CatMsgBody((u8*)&tMtadpHeadMsg, sizeof(TMtadpHeadMsg));
	}
	else
	{
		cMsg.CatMsgBody((u8*)&tMtadpHeadMsg, sizeof(TMtadpHeadMsg));
	}

	TNonStandardMsg tMsg;
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		OspPrintf( TRUE, FALSE, "[MTADAP][EXP]OnSendAddMixerMemberCmd message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg));

	s8 achBuff[1024] = {0};
	s8 *pBuff = (s8 *)achBuff;
	u16 wLen = 0;

	wLen += sprintf(pBuff, "OnSendAddMixerMemberCmd MsgLen: %d Bodylen:%d Mtnum:%d Replace:%d",
		cMsg.GetServMsgLen(),
		cMsg.GetMsgBodyLen(),
		nMtNum,
		byReplace
		);
	
	if (nMtNum > 0 && ptMtStart != NULL)
	{
		wLen += sprintf(pBuff + wLen, " MTID: <%d, %d>", ptMtStart->GetMcuId(), ptMtStart->GetMtId());
	}
	
	wLen += sprintf(pBuff + wLen, "\n");

	MtAdpInfo(achBuff);
	
	return;*/
}

/*=============================================================================
  函 数 名： OnSendRemoveMixerMemberCmd
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendRemoveMixerMemberCmd( CServMsg& cServMsg, HCHAN hsChan )
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	TMt *ptMtStart     = (TMt *)(cServMsg.GetMsgBody()+sizeof(TMcu));
	s32 nMtNum    = (cServMsg.GetMsgBodyLen()-sizeof(TMcu) - sizeof(u8) - sizeof(TMsgHeadMsg))/sizeof(TMt);

	u8 byNotForceRemove = *(cServMsg.GetMsgBody() + sizeof(TMcu) + (u32)nMtNum * sizeof(TMt));

	u32 dwPartId  = 0 ;
	BOOL32 bLocal = FALSE;

	TMt *ptMt = ptMtStart;
	TMsgHeadMsg tMsgHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+ sizeof(TMt) * (u32)nMtNum + sizeof(TMcu) + sizeof(u8));
	TMtadpHeadMsg tMtadpHeadMsg;
	TMcu tMcu = *(TMcu *)cServMsg.GetMsgBody();
	if ( tMsgHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		for(s32 nIndex = 0; nIndex < nMtNum; nIndex ++ )
		{
			if (ptMt == NULL)
			{
				break;
			}
			dwPartId = ptMt->GetMtId();
			dwPartId = htonl(dwPartId);
			cMsg.CatMsgBody( (u8 *)&dwPartId, sizeof(u32) );
			ptMt ++;
		}
		u32 dwPaId = GetPartIdFromMt(tMcu,FALSE);
		tMtadpHeadMsg.m_tMsgDst.SetPartGid(htonl(dwPaId),FALSE);
		tMtadpHeadMsg.m_tMsgDst.m_byCasLevel = tMsgHeadMsg.m_tMsgDst.m_byCasLevel;	
		dwPaId = ntohl(dwPaId);
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendRemoveMixerMemberCmd] PARTID(%d) LEVEL(%d)\n",dwPaId,tMtadpHeadMsg.m_tMsgDst.m_byCasLevel);
	} 
	else
	{
		for(s32 nIndex = 0; nIndex < nMtNum; nIndex ++ )
		{
			if (ptMt == NULL)
			{
				break;
			}
			dwPartId = GetPartIdFromMt(*ptMt, &bLocal);
			dwPartId = htonl(dwPartId);
			cMsg.CatMsgBody( (u8 *)&dwPartId, sizeof(u32) );
			ptMt ++;
		}
		tMtadpHeadMsg.m_tMsgDst.SetPartGid(0,TRUE);
	}

    LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);

	/*if (strcmp(lpszPeerVerId, "10") == 0)
	{
		cMsg.CatMsgBody((u8*)&tMtadpHeadMsg, sizeof(TMtadpHeadMsg));
	} 
	else
	{
		cMsg.CatMsgBody((u8*)&tMtadpHeadMsg, sizeof(TMtadpHeadMsg));
	}*/

	// [20111201_miaoqingsong] 20101116_V4R6B2以上版本消息体中才保存byNotForceRemove和tMtadpHeadMsg两个字段
	if ( atoi(lpszPeerVerId) >= 10 )
	{
		cMsg.CatMsgBody((u8 *)&byNotForceRemove, sizeof(byNotForceRemove));
		cMsg.CatMsgBody((u8*)&tMtadpHeadMsg, sizeof(TMtadpHeadMsg));
	}

	TNonStandardMsg tMsg;
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	if( (u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf) )
	{
		StaticLog( "[MTADAP][EXP]OnSendRemoveMixerMemberCmd message is too large(len:%d)\n", tMsg.m_nMsgLen);
		return;
	}
	memcpy(tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen);
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );

	s8 achBuff[1024] = {0};
	s8 *pBuff = (s8 *)achBuff;
	u16 wLen = 0;

	wLen += sprintf(pBuff, "OnSendRemoveMixerMemberCmd MsgLen: %d Bodylen:%d Mtnum:%d FroceRemove:%d",
		cMsg.GetServMsgLen(),
		cMsg.GetMsgBodyLen(),
		nMtNum,
		byNotForceRemove == 1 ? 0 : 1
		);

	if (nMtNum > 0 && ptMtStart != NULL)
	{
		wLen += sprintf(pBuff + wLen, " MTID: <%d, %d>", ptMtStart->GetMcuId(), ptMtStart->GetMtId());
	}

	wLen += sprintf(pBuff + wLen, "\n");

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, achBuff);
	
	return;
}

/*=============================================================================
  函 数 名： OnSendLockMcuReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendLockMcuReq(CServMsg& cServMsg, HCHAN hsChan)
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	cMsg.SetMsgBody( cServMsg.GetMsgBody() + sizeof(TMcu), sizeof(u8) );
	
	TNonStandardReq tReq;
	tReq.m_aszUserName[0] = 0;
	tReq.m_aszUserPass[0] = 0;
	tReq.m_nReqID         = m_dwMMcuReqId ++ ;
	tReq.m_nMsgLen        = cMsg.GetServMsgLen();
	
	if((u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][EXP]OnSendLockMcuReq message is too large(len:%d)\n", tReq.m_nMsgLen );
		return;
	}

	memcpy(tReq.m_abyMsgBuf, cMsg.GetServMsg(), tReq.m_nMsgLen);
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq));

	return;
}

/*=============================================================================
  函 数 名： OnSendLockMcuAck
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendLockMcuAck( CServMsg& cServMsg, HCHAN hsChan )
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	cMsg.SetMsgBody( cServMsg.GetMsgBody() + sizeof(TMcu), sizeof(u8) );
	
	TNonStandardRsp tRsp;
	tRsp.m_nReqID = 0;
	tRsp.m_emReturnVal = emReturnValue_Ok;
	tRsp.m_nMsgLen = cMsg.GetServMsgLen();

	if( (u32)tRsp.m_nMsgLen > sizeof(tRsp.m_abyMsgBuf) )
	{
		StaticLog( "[MTADAP][EXP]OnSendLockMcuAck message is too large(len:%d)\n", tRsp.m_nMsgLen );
		return;
	}
	memcpy( tRsp.m_abyMsgBuf, cMsg.GetServMsg(), tRsp.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_RSP, &tRsp, sizeof(tRsp) );

	return;
}

/*=============================================================================
  函 数 名： OnSendLockMcuNack
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendLockMcuNack(CServMsg& cServMsg, HCHAN hsChan)
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	cMsg.SetMsgBody( cServMsg.GetMsgBody() + sizeof(TMcu), sizeof(u8) );
	TNonStandardRsp tRsp;
	tRsp.m_nReqID      = 0;
	tRsp.m_emReturnVal = emReturnValue_Error;
	tRsp.m_nMsgLen     = cMsg.GetServMsgLen();
	if( (u32)tRsp.m_nMsgLen > sizeof(tRsp.m_abyMsgBuf) )
	{
		StaticLog( "[MTADAP][EXP]OnSendLockMcuNack message is too large(len:%d)\n", tRsp.m_nMsgLen);
		return;
	}

	memcpy( tRsp.m_abyMsgBuf, cMsg.GetServMsg(), tRsp.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_RSP, &tRsp, sizeof(tRsp) );

	return;
}

/*=============================================================================
    函 数 名： OnSendMtStatusCmd
    功    能： 向下级mcu发送请求其直连终端的状态报告
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/11   3.6			万春雷                  创建
=============================================================================*/
void CMtAdpInst::OnSendMtStatusCmd( CServMsg& cServMsg, HCHAN hsChan )
{
	CServMsg cMsg;
	cMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);

	TNonStandardReq tReq;
	tReq.m_aszUserName[0] = 0;
	tReq.m_aszUserPass[0] = 0;
	tReq.m_nReqID = m_dwMMcuReqId ++;
	tReq.m_nMsgLen = cMsg.GetServMsgLen();
	if( (u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf) )
	{
		StaticLog( "[MTADAP][EXP]OnSendMtStatusCmd message is too large(len:%d)\n", tReq.m_nMsgLen);
		return;
	}

	memcpy( tReq.m_abyMsgBuf, cMsg.GetServMsg(), tReq.m_nMsgLen );
    kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq) );

	return;
}

/*=============================================================================
    函 数 名： OnSendMtStatusNtf
    功    能： 向上级mcu发送本级直连终端的状态报告
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/11   3.6			万春雷                  创建
=============================================================================*/
void CMtAdpInst::OnSendMtStatusNtf(CServMsg& cServMsg, HCHAN hsChan)
{	
    // xsl [8/1/2006] 判断对方是否3.6和4.0R2版本，即4.0R3以前版本
    BOOL32 bMcuAfterR3Ver = TRUE;
    LPCSTR lpszPeerVerId = GetPeerVersionID( m_hsCall);


	BOOL32 bMcuIsSupportDisConnectReason = TRUE;
	BOOL32 bMcuIsSupportMultiCascade = TRUE;

	if (strcmp(lpszPeerVerId, "3.6") == 0 || strcmp(lpszPeerVerId, "4.0") == 0 )
    {
        bMcuAfterR3Ver = FALSE;
    }

	// miaoqingsong [20110526] 修改是否支持“终端呼叫断链原因”和“多回传”条件判断
	// 判断对方是否是4.0R5以前的版本, 4.0R5以前的版本不支持终端呼叫断链原因
	if( strcmp(lpszPeerVerId, "3.6") == 0 || 
		strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "5") == 0   ||
		strcmp(lpszPeerVerId, "6") == 0   ||
		strcmp(lpszPeerVerId, "7") == 0 )
	{
		bMcuIsSupportDisConnectReason = FALSE;
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendMtStatusNtf] mcu %s version,Is not Support MT Disconnect Reason\n", lpszPeerVerId);
	}

	// 版本号"10"之前版本均不支持多回传
	if( strcmp(lpszPeerVerId, "3.6") == 0 || 
		strcmp(lpszPeerVerId, "4.0") == 0 || 
		strcmp(lpszPeerVerId, "5") == 0   ||
		strcmp(lpszPeerVerId, "6") == 0   ||
		strcmp(lpszPeerVerId, "7") == 0   ||
		strcmp(lpszPeerVerId, "8") == 0   ||
		strcmp(lpszPeerVerId, "9") == 0 )
	{
		bMcuIsSupportMultiCascade = FALSE;
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendMtStatusNtf] mcu %s version,Is not Support MultiCascade.\n", lpszPeerVerId);
	}
	
	// 判断对方是否是4.0R5或以后版本
	/*if( strcmp(lpszPeerVerId, "8") == 0 || strcmp(lpszPeerVerId, "9") == 0 ||
		strcmp(lpszPeerVerId, "5") == 0
		)
	{
		bMcuIsSupportDisConnectReason = TRUE;
		bMcuIsSupportMultiCascade = FALSE;
		MtAdpInfo("[OnSendMtStatusNtf] mcu %s version,IsSupport MT Disconnect Reason,but not support MultiCascade.\n", lpszPeerVerId);
	}*/

	/*if( strcmp(lpszPeerVerId, "10") == 0 )
	{
		bMcuIsSupportDisConnectReason = TRUE;
		bMcuIsSupportMultiCascade = TRUE;
		MtAdpInfo("[OnSendMtStatusNtf] mcu %s version,IsSupportMultiCascade\n", lpszPeerVerId);		
	}*/

	TNonStandardMsg tMsg;
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );

	u8 byMtNum = *(u8*)cServMsg.GetMsgBody();	
	TSMcuMtStatus *ptSMcuMtStatus = (TSMcuMtStatus *)( cServMsg.GetMsgBody()+sizeof(u8) );
	TMcuToMcuMtStatus tMcuToMcuMtStatus;
	u32 dwPartId  = 0;
	BOOL32 bLocal = TRUE;
	TMcuToMcuMtStatusBeforeV4R5 tMcuToMcuStatusBeforeV4R5;
	TMcuToMcuMtStatusBeforeV4R6NoMultiCascade tMcuToMcuStatusBeforeV4R6NoMultiCascade;
	
	if( bMcuIsSupportMultiCascade )
	{
		cMsg.SetMsgBody( (u8*)&byMtNum,sizeof(u8) );
	}
	else
	{
		cMsg.SetMsgBody(NULL,0);
	}
		
	for( u8 byIndex = 0; byIndex < byMtNum; byIndex ++ )
	{
		memset( &tMcuToMcuMtStatus,0,sizeof(tMcuToMcuMtStatus) );
		if( ptSMcuMtStatus->GetMcuId() != LOCAL_MCUID )
		{
			tMcuToMcuMtStatus.m_abyMtIdentify[tMcuToMcuMtStatus.m_byCasLevel] = ptSMcuMtStatus->GetMtId();
			++tMcuToMcuMtStatus.m_byCasLevel;
			ptSMcuMtStatus->SetMtId( (u8)ptSMcuMtStatus->GetMcuId() );
			ptSMcuMtStatus->SetMcuIdx( LOCAL_MCUID );
		}		
		dwPartId = GetPartIdFromMt(*((TMt*)ptSMcuMtStatus), &bLocal);	
		tMcuToMcuMtStatus.SetPartId( dwPartId );
		tMcuToMcuMtStatus.SetIsEnableFECC( ptSMcuMtStatus->IsEnableFECC() );
		tMcuToMcuMtStatus.SetCurVideo( ptSMcuMtStatus->GetCurVideo() );
		tMcuToMcuMtStatus.SetCurAudio( ptSMcuMtStatus->GetCurAudio() );
		tMcuToMcuMtStatus.SetMtBoardType( ptSMcuMtStatus->GetMtBoardType() );
        if (bMcuAfterR3Ver)
        {
            tMcuToMcuMtStatus.SetIsMixing( ptSMcuMtStatus->IsMixing() );
            tMcuToMcuMtStatus.SetIsVideoLose( ptSMcuMtStatus->IsVideoLose() );
			tMcuToMcuMtStatus.SetIsAutoCallMode( ptSMcuMtStatus->IsAutoCallMode() );
			tMcuToMcuMtStatus.SetRecvVideo( ptSMcuMtStatus->IsRecvVideo() );
			tMcuToMcuMtStatus.SetRecvAudio( ptSMcuMtStatus->IsRecvAudio() );
			tMcuToMcuMtStatus.SetSendAudio( ptSMcuMtStatus->IsSendAudio() );
			tMcuToMcuMtStatus.SetSendVideo( ptSMcuMtStatus->IsSendVideo() );			
        }
        else
        {
            tMcuToMcuMtStatus.SetIsMixing( ptSMcuMtStatus->IsMixing() );
            tMcuToMcuMtStatus.SetIsVideoLose( FALSE );
        }

		//R2 WJ级联终端状态上报结构存在差异, 其不包含vidlose autocallmode信息
		if (IsR2WJMCU())
		{
			tMcuToMcuMtStatus.SetIsVideoLose( ptSMcuMtStatus->IsSendVideo() );
			tMcuToMcuMtStatus.SetIsAutoCallMode( ptSMcuMtStatus->IsSendAudio() );
			tMcuToMcuMtStatus.SetSendVideo( ptSMcuMtStatus->IsRecvVideo() );
			tMcuToMcuMtStatus.SetSendAudio( ptSMcuMtStatus->IsRecvAudio() );
		}

		//非V4R5和V4R5以前的版本没有不在线原因
		if( !bMcuIsSupportDisConnectReason )
		{			
			memset( &tMcuToMcuStatusBeforeV4R5,0,sizeof(TMcuToMcuMtStatusBeforeV4R5) );
			CMtAdpUtils::McuToMcuMtStatusVerR4V5ToVer40( tMcuToMcuMtStatus,tMcuToMcuStatusBeforeV4R5 );
			/*if( 0 == nIndex )
			{
				cMsg.SetMsgBody((u8 *)&tMcuToMcuStatusBeforeV4R5, sizeof(tMcuToMcuStatusBeforeV4R5));
			}
			else*/
			{
				cMsg.CatMsgBody((u8 *)&tMcuToMcuStatusBeforeV4R5, sizeof(tMcuToMcuStatusBeforeV4R5));
			}
		}
		else
		{			
			tMcuToMcuMtStatus.SetMtExInfo( ptSMcuMtStatus->GetMtExInfo() );
			if( !bMcuIsSupportMultiCascade )
			{
				memcpy( &tMcuToMcuStatusBeforeV4R6NoMultiCascade,
							&tMcuToMcuMtStatus,
							sizeof(tMcuToMcuStatusBeforeV4R6NoMultiCascade)
							);
				/*if( 0 == nIndex )
				{
					cMsg.SetMsgBody((u8 *)&tMcuToMcuStatusBeforeV4R6NoMultiCascade, sizeof(tMcuToMcuStatusBeforeV4R6NoMultiCascade));
				}
				else*/
				{
					cMsg.CatMsgBody((u8 *)&tMcuToMcuStatusBeforeV4R6NoMultiCascade, sizeof(tMcuToMcuStatusBeforeV4R6NoMultiCascade));
				}
			}
			else
			{				
				/*if( 0 == nIndex )
				{
					cMsg.SetMsgBody((u8 *)&tMcuToMcuMtStatus, sizeof(tMcuToMcuMtStatus));
				}
				else*/
				{
					cMsg.CatMsgBody((u8 *)&tMcuToMcuMtStatus, sizeof(tMcuToMcuMtStatus));
				}
			}
		}		

		ptSMcuMtStatus++;
	}

	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][EXP]OnSendMtStatusNtf message is too large(len:%d)\n", tMsg.m_nMsgLen);
		return;
	}
	memcpy(tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen);
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg));
	
	return;
}

/*=============================================================================
  函 数 名： OnSendMsgNtf
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::OnSendMsgNtf(CServMsg& cServMsg, HCHAN hsChan)
{
	TNonStandardMsg tMsg;
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TMt tMt = *(TMt *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
	CRollMsg* ptROLLMSG = (CRollMsg*)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg) );
	
	TPartID tPartId;
	tPartId.m_dwPID = htonl( GetPartIdFromMt( tMt, &(tPartId.m_bLocal) ) );

	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	cMsg.SetMsgBody( (u8 *)&(tPartId.m_dwPID), sizeof(tPartId.m_dwPID) );

	// 多国语言 [pengguofeng 4/8/2013]
	s8 achRollMsg[MAX_ROLLMSG_LEN];
	memset(achRollMsg, 0, sizeof(achRollMsg));
	BOOL32 bNeedTrans = TransEncoding((s8*)ptROLLMSG->GetRollMsgContent(), achRollMsg, sizeof(achRollMsg));
	if ( bNeedTrans == TRUE )
	{
		ptROLLMSG->SetRollMsgContent((u8*)achRollMsg, strlen(achRollMsg));
	}
	else
	{
		MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[OnSendMsgNtf]Not need to trans Roll Msg\n");
	}
	
	MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[OnSendMsgNtf]Roll Msg final len:%d\n", ptROLLMSG->GetRollMsgContentLen());

	cMsg.CatMsgBody( ptROLLMSG->GetRollMsg(), ptROLLMSG->GetTotalMsgLen() );
	cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
	
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	if( (u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf) )
	{
		StaticLog( "[MTADAP][EXP]OnSendMsgNtf message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );

	return;
}

/*=============================================================================
  函 数 名： OnSendMsgChangeMtSecVidSendCmd
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
//[5/4/2011 zhushengze]VCS控制发言人发双流
void CMtAdpInst::OnSendMsgChangeMtSecVidSendCmd(const CServMsg& cServMsg, HCHAN hsChan)
{
    //解析消息体
    TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
    TMt tMt = *(TMt*)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));
    u8 byIsSendDStream = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TMt));

    TMtadpHeadMsg tMdadpHeadMsg;
    tMdadpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tMt, &tMdadpHeadMsg.m_tMsgDst.m_bLocal );
    tMdadpHeadMsg.m_tMsgDst.m_dwPID = htonl( tMdadpHeadMsg.m_tMsgDst.m_dwPID );
	HeadMsgToMtadpHeadMsg( tHeadMsg,tMdadpHeadMsg );


    //组织消息体
    CServMsg cMsg;
    cMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
    cMsg.CatMsgBody((u8*)&tMdadpHeadMsg, sizeof(TMtadpHeadMsg));
    cMsg.CatMsgBody((u8*)&byIsSendDStream, sizeof(u8));

    //组织非标消息体
    TNonStandardMsg tMsg;
    tMsg.m_nMsgLen = cMsg.GetServMsgLen();
    memcpy(tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen);
    
    //发送非标消息
    kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );
    
    return;
}

/*=============================================================================
函 数 名： OnSendMsgChangeMtSecVidSendCmd
功    能： 
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2/23/2012                 朱胜泽                  创建
=============================================================================*/
void CMtAdpInst::OnSendMsgTransparentMsgNtf( const CServMsg& cServMsg, HCHAN hsChan )
{
    
    TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
    TMt tMt = *(TMt*)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));
    u8* pbyMsgContent = (u8*)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TMt));
    u16 dwMsgLen = cServMsg.GetMsgBodyLen() - sizeof(TMsgHeadMsg) - sizeof(TMt);
    
    TMtadpHeadMsg tMdadpHeadMsg;
    tMdadpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tMt, &tMdadpHeadMsg.m_tMsgDst.m_bLocal );
    tMdadpHeadMsg.m_tMsgDst.m_dwPID = htonl( tMdadpHeadMsg.m_tMsgDst.m_dwPID );
    HeadMsgToMtadpHeadMsg( tHeadMsg,tMdadpHeadMsg );
    
    
    //组织消息体
    CServMsg cMsg;
    cMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
    cMsg.CatMsgBody((u8*)&tMdadpHeadMsg, sizeof(TMtadpHeadMsg));
    cMsg.CatMsgBody((u8*)pbyMsgContent, dwMsgLen);
    
    //组织非标消息体
    TNonStandardMsg tMsg;
    tMsg.m_nMsgLen = cMsg.GetServMsgLen();
    memcpy(tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen);
    
    //发送非标消息
    kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );
    
    return;
    
}

/*=============================================================================
  函 数 名： ProcKdvCustomEvMcuMt
  功    能： 处理MCU到MT的自定义消息 
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
  2005/12/28	4.0			张宝卿					增加终端外置矩阵处理
  2005/12/30	4.0			张宝卿					终端内置矩阵消息调整
  2006/01/04	4.0			张宝卿					兼容3.6终端
=============================================================================*/
void CMtAdpInst::ProcKdvCustomEvMcuMt( CMessage * const pcMsg )
{
	m_wSavedEvent = pcMsg->event;

	if(m_byVendorId != MT_MANU_KDC)	
	{
		return ;
	}

	switch(pcMsg->event)
	{					
	case MCU_MT_MTAPPLYMIX_NOTIF:           //通知主席终端其它终端申请参加混音请求
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			
			if(!BODY_LEN_GE(cServMsg, sizeof(TMt))) 
				return;

			TMt tMt = *(TMt*)cServMsg.GetMsgBody();	
			
			TTERLABEL tTer;
			//tTer.SetTerminalLabel( (u8)g_cMtAdpApp.m_wMcuNetId, tMt.GetMtId() );
			CMtAdpUtils::ConverTMt2TTERLABEL( tMt, tTer );

			CONF_CTRL_STRICT( (u16)h_ctrl_otherTerChimeInInd, &tTer, sizeof(tTer) );
		}
		break;
	case MCU_MT_DISCUSSPARAM_NOTIF:			//通知终端定制混音状态// [11/25/2010 xliang]  
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			if( !BODY_LEN_GE(cServMsg, sizeof(TMixParam)) ) 
				return;
			
			// tramslate struct TMixParam according to MT Ver 
			if ( m_byMtVer == (u8)emMtVer40R4 
				||m_byMtVer == (u8)emMtVer36 
				)
            {
				
				TMixParam tMixParam = *(TMixParam *)cServMsg.GetMsgBody();
				TDiscussParamV4R4B2 tDiscussParam;
				
				tDiscussParam.m_byMixMode = tMixParam.GetMode();
				
                //[20110729 zhushz]pclnt err:666
                u8 byMixMemberNum = tMixParam.GetMemberNum();
				u8 byNum = min(MAXNUM_MIXER_DEPTH, byMixMemberNum/*tMixParam.GetMemberNum()*/);
				tDiscussParam.m_byMemberNum = byNum;
				for( u8 byIdx = 0; byIdx < byNum; byIdx ++)
				{
					tDiscussParam.m_atMtMember[byIdx].SetMt(LOCAL_MCUID, tMixParam.GetMemberByIdx(byIdx));
				}
                
                CONF_CTRL_STRICT( (u16)h_ctrl_discussParamInd, (u8*)&tDiscussParam, sizeof(tDiscussParam) );
			}
			else
			{
				CONF_CTRL_STRICT( (u16)h_ctrl_discussParamInd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
			}
			
			break;
		}
	case MCU_MT_MTADDMIX_NOTIF:             //通知终端已成功参加混音
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			if( !BODY_LEN_GE(cServMsg, sizeof(u8)) ) 
				return;

			u8 byMixStat = *(u8*)cServMsg.GetMsgBody();

			//如果是3.6终端, 直接写入; 其他, 进行以下转换
			if ( m_byMtVer != (u8)emMtVer36 )
			{
				if(1 == byMixStat)
				{
					byMixStat = (u8)ctrl_granted;
				}
				else
				{
					byMixStat = (u8)ctrl_denied;
				}
			}
			CONF_CTRL_STRICT( (u16)h_ctrl_chimeInInd, &byMixStat, sizeof(byMixStat) );			
		}
		break;
		
	case MCU_MT_CHANGESPEAKER_NOTIF: //TMt
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			if(!BODY_LEN_GE(cServMsg, sizeof(TMt))) return;

			TMt tMt = *(TMt*)cServMsg.GetMsgBody();			
			TTERLABEL tTer;
//			tTer.SetTerminalLabel( (u8)g_cMtAdpApp.m_wMcuNetId, tMt.GetMtId() );
			CMtAdpUtils::ConverTMt2TTERLABEL( tMt, tTer );
			CONF_CTRL_STRICT( (u16)h_ctrl_broadcasterTokenOwnerInd, &tTer, sizeof(tTer) );
		}
		break;

	case MCU_MT_CHANGECHAIRMAN_NOTIF: //TMt
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			if( !BODY_LEN_GE(cServMsg, sizeof(TMt))) 
				return;

			TMt tMt = *(TMt*)cServMsg.GetMsgBody();
			
			TTERINFO tTerInfo;

			tTerInfo.SetLabel((u8)g_cMtAdpApp.m_wMcuNetId, tMt.GetMtId());
			tTerInfo.SetName("Null");
			CONF_CTRL_STRICT( (u16)h_ctrl_chairTokenOwnerResponse, (void*)&tTerInfo, sizeof(tTerInfo) );
		}
		break;

	case MCU_MT_GETMTSTATUS_REQ:
		{		
			//help decide whether or not to convert the 
			//following status indiction to response
			m_wSavedEvent = pcMsg->event;  
			CONF_CTRL_STRICT( (u16)h_ctrl_terStatusReq, NULL, 0 );
		}
		break;

	case MCU_MT_STARTMTPOLL_CMD: //TMt +TPollInfo + TMtPollParam[]
		break;

	case MCU_MT_STOPMTPOLL_CMD: //TMt
		CONF_CTRL_STRICT( (u16)h_ctrl_stopPollCmd,  NULL, 0 );		
		break;

	case MCU_MT_PAUSEMTPOLL_CMD: //TMt
		CONF_CTRL_STRICT( (u16)h_ctrl_pausePollCmd, NULL, 0 );		
		break;

	case MCU_MT_RESUMEMTPOLL_CMD: //TMt
		CONF_CTRL_STRICT( (u16)h_ctrl_resumePollCmd, NULL, 0 );	
		break;

	case MCU_MT_GETMTPOLLPARAM_REQ:
		CONF_CTRL_STRICT( (u16)h_ctrl_pollParamReq, NULL, 0 );
		break;

	case MCU_MT_APPLYCHAIRMAN_NOTIF://new application for chairman from terminal
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			if(!BODY_LEN_GE(cServMsg, sizeof(TMt))) 
				return;

			TMt *pTMt = (TMt*)cServMsg.GetMsgBody();

			TTERLABEL tTer;
			//tTer.SetTerminalLabel( (u8)pTMt->GetMcuId(), pTMt->GetMtId() );
			CMtAdpUtils::ConverTMt2TTERLABEL( *pTMt, tTer );
			
			CONF_CTRL_STRICT( (u16)h_ctrl_terApplyChairInd, (void*)&tTer, sizeof(tTer) );
		}
		break;

	case MCU_MT_SPECCHAIRMAN_NOTIF:	//notify mt it is designated chair. makeMeChairRsp
		{			
			u8 byResult = (u8)ctrl_granted;				
			CONF_CTRL_STRICT( (u16)h_ctrl_makeMeChairResponse, &byResult, 1 );							
		}		
		break;

	case MCU_MT_SPECCHAIRMAN_ACK: //designating new chair approved. chairTokenOwnerResponse
		{
			//The TMt info of orignally designated chairman
			//is not stored in mtadp, so use the requestChairTokenOwner
			//to elicit chairTokenOwner from MCU to the terminal
			BuildAndSendMsgToMcu( MT_MCU_GETCHAIRMAN_REQ );
		}
		break;

	case MCU_MT_SPECCHAIRMAN_NACK:	//designating new chair rejected
		{			
			TTERINFO tTerInfo;
			tTerInfo.SetLabel((u8)g_cMtAdpApp.m_wMcuNetId, m_byMtId);
            tTerInfo.SetName(m_tMtAlias.m_achAlias);

			CONF_CTRL_STRICT( (u16)h_ctrl_chairTokenOwnerResponse, &tTerInfo, sizeof(TTERINFO) );
		}
		break;

	case MCU_MT_SENDMSG_NOTIF:		//short message
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			
			if(!BODY_LEN_GE(cServMsg, sizeof(TMt))) 
				return;

			CRollMsg cRollMsg;
			cRollMsg.SetRollMsg(cServMsg.GetMsgBody() + sizeof(TMt), cServMsg.GetMsgBodyLen() - sizeof(TMt) );	
//			TMt tMt = *(TMt*)cServMsg.GetMsgBody();
			
			CRollMsg cRollMsgToSend;
			cRollMsgToSend.SetType(cRollMsg.GetType());
			cRollMsgToSend.SetRollTimes(cRollMsg.GetRollTimes());
			cRollMsgToSend.SetRollRate(cRollMsg.GetRollRate());			
			cRollMsgToSend.AddDstMt((u8)g_cMtAdpApp.m_wMcuNetId, m_byMtId);
			cRollMsgToSend.SetMsgSrcMtId(cRollMsg.GetMsgSrcMtId());
			// 多国语言 [pengguofeng 4/8/2013]
			s8 achRollMsg[MAX_ROLLMSG_LEN];
			memset(achRollMsg, 0, sizeof(achRollMsg));
			BOOL32 bNeedTrans = TransEncoding((s8*)cRollMsg.GetRollMsgContent(), achRollMsg, sizeof(achRollMsg));
			if ( bNeedTrans == TRUE )
			{
				cRollMsgToSend.SetRollMsgContent((u8*)achRollMsg, strlen(achRollMsg));
			}
			else
			{
				MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcKdvCustomEvMcuMt] MCU_MT_SENDMSG_NOTIF Not need to trans Roll Msg\n");
				cRollMsgToSend.SetRollMsgContent(cRollMsg.GetRollMsgContent(), cRollMsg.GetRollMsgContentLen());
			}
			cRollMsgToSend.SetMsgId(cRollMsg.GetMsgId());

			CONF_CTRL_STRICT( (u16)h_ctrl_smsInd, &cRollMsgToSend, cRollMsgToSend.GetTotalMsgLen() );
		}
		break;

	case MCU_MT_MTSOURCENAME_NOTIF:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			CONF_CTRL_STRICT( (u16)h_ctrl_termSourceNameInd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		}
		break;

	case MCU_MT_ADDMT_ACK:	//do nothing for this ack.
							//the later MCU_MT_MTJOINED_NOTIF will notify mt about this
		break;

	case MCU_MT_ADDMT_NACK:	//need to notify mt if failed
		{
			CServMsg cServMsg(pcMsg->content, pcMsg->length);
			
			if( BODY_LEN_GE(cServMsg, sizeof(TMtAlias)) )
			{
                TTERADDR tTerAddr;
                //TCALLADDR tCallAddr;
                TMtAlias *ptAlias = (TMtAlias *)cServMsg.GetMsgBody();
                
                if( ptAlias->m_AliasType == (u8)mtAliasTypeTransportAddress )
                {
                    tTerAddr.SetAddrType(ADDR_TYPE_IP);
                    tTerAddr.SetIpAddr(htonl(ptAlias->m_tTransportAddr.GetIpAddr()), ptAlias->m_tTransportAddr.GetPort());
                }
                else
                {
                    tTerAddr.SetAddrType( ADDR_TYPE_ALIAS );

				    if(ptAlias->m_AliasType == (u8)mtAliasTypeH323ID)
                    {                        
                        tTerAddr.SetAlias(ALIAS_H323ID, ptAlias->m_achAlias, sizeof(ptAlias->m_achAlias));
                    }
                    else                    
                    {                        
                        tTerAddr.SetAlias(ALIAS_E164, ptAlias->m_achAlias, sizeof(ptAlias->m_achAlias));
                    }
                }
				
				CONF_CTRL_STRICT( (u16)h_ctrl_chairInviteFailedInd, (u8*)&tTerAddr, sizeof(TTERADDR) );
			}			
		}
		break;

	case MCU_MT_CONF_NOTIF:			//conf info notif, TConfInfo
	case MCU_MT_GETCONFINFO_ACK:	//conf info response, TConfInfo
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			if( !BODY_LEN_GE(cServMsg, sizeof(TConfInfo)) )
				return;
			
			TConfInfo *ptConfInfo = (TConfInfo *)cServMsg.GetMsgBody();
#ifdef _UTF8
			// 会议别名需要转换 [pengguofeng 5/20/2013]
			s8 achConfName[MAXLEN_CONFNAME+1];
			memset(achConfName, 0, sizeof(achConfName));
			BOOL32 bTrans = TransEncoding(ptConfInfo->GetConfName(), achConfName, MAXLEN_CONFNAME+1);
			if ( bTrans == TRUE )
			{
				ptConfInfo->SetConfName(achConfName);
			}
#endif
			if( !ptConfInfo->m_tStatus.m_tChairman.IsNull() )
			{
				ptConfInfo->m_tStatus.m_tChairman.SetMcuId( (u8)g_cMtAdpApp.m_wMcuNetId );
			}
			else
			{
				ptConfInfo->m_tStatus.m_tChairman.SetMcuId( 0 );
			}

			if( !ptConfInfo->m_tStatus.m_tSpeaker.IsNull() )
			{
				ptConfInfo->m_tStatus.m_tSpeaker.SetMcuId( (u8)g_cMtAdpApp.m_wMcuNetId );
			}
			else
			{
				ptConfInfo->m_tStatus.m_tSpeaker.SetMcuId( 0 );
			}
			// xliang [2/24/2009] 升至4.6
			//非3.6终端按4.0结构发
			if ( m_byMtVer == (u8)emMtVer40R6 ) //4.6
			{
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Ver40R6 the mix mode is %d", ((TConfInfo *)cServMsg.GetMsgBody())->GetStatus().GetMixerMode());
					
				CONF_CTRL_STRICT( (u16)h_ctrl_confInfoRsp, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
			}
			if ( m_byMtVer == (u8)emMtVer40R5 )	//4.6-->4.5
			{
				TConfInfo tConfInfo;
                memset( &tConfInfo, 0, sizeof(tConfInfo) );
                tConfInfo = *(TConfInfo*)cServMsg.GetMsgBody();
                
                TConfInfoV4R5 tConfInfoV4R5;
                memset( &tConfInfoV4R5, 0, sizeof(tConfInfoV4R5) );
                CStructConvert::ConfInfoV4R6ToV4R5( tConfInfoV4R5, tConfInfo );
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Ver40R5 the mix mode is %d", tConfInfoV4R5.GetStatus().GetMixerMode());
				
				CONF_CTRL_STRICT( (u16)h_ctrl_confInfoRsp, (u8*)&tConfInfoV4R5, sizeof(tConfInfoV4R5) );
			}
			else if ( m_byMtVer == (u8)emMtVer40R4 )//4.6-->4.0
            {
                TConfInfo tConfInfo;
                memset( &tConfInfo, 0, sizeof(tConfInfo) );
                tConfInfo = *(TConfInfo*)cServMsg.GetMsgBody();
                
                TConfInfoV4R4B2 tConfInfoV4R4B2;
                memset( &tConfInfoV4R4B2, 0, sizeof(tConfInfoV4R4B2) );
                //CMtAdpUtils::ConfInfoVer40V4R5ToVer40V4R4B2( tConfInfoV4R4B2, tConfInfo );
                CStructConvert::ConfInfoV4R6ToV4R4( tConfInfoV4R4B2, tConfInfo );
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Ver40R4 the mix mode is %d", tConfInfoV4R4B2.GetStatus().GetMixModeParam());
				
                CONF_CTRL_STRICT( (u16)h_ctrl_confInfoRsp, (u8*)&tConfInfoV4R4B2, sizeof(tConfInfoV4R4B2) );
			}
            else if ( m_byMtVer == (u8)emMtVer36 )//4.6-->3.6
			{
				TConfInfo tConfInfo;
				memset( &tConfInfo, 0, sizeof(tConfInfo) );
				tConfInfo = *(TConfInfo*)cServMsg.GetMsgBody();

				TConfInfoVer36 tConfInfo36;
				memset( &tConfInfo36, 0, sizeof(tConfInfo36) );
				//CMtAdpUtils::ConfInfoVer40DailyToVer36( tConfInfo36, tConfInfo );
				CStructConvert::ConfInfoV4R6ToV36(tConfInfo36, tConfInfo);
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Ver36 the mix mode is %d", tConfInfo36.GetStatus().GetDiscussParam().m_byMixMode);
				
				CONF_CTRL_STRICT( (u16)h_ctrl_confInfoRsp, (u8*)&tConfInfo36, sizeof(tConfInfo36) );
			}
		}
		break;
		
	case MCU_MT_SIMPLECONF_NOTIF:	//simple conf info notif, TSimConfInfo 2005-10-11
		{
			CServMsg cServMsg(pcMsg->content, pcMsg->length);
			if(!BODY_LEN_GE(cServMsg, sizeof(TSimConfInfo))) return;

			TSimConfInfo *ptSimConfInfo = (TSimConfInfo *)cServMsg.GetMsgBody();
			if( !ptSimConfInfo->m_tChairMan.IsNull() )
			{
				ptSimConfInfo->m_tChairMan.SetMcuId( (u8)g_cMtAdpApp.m_wMcuNetId );
			}
			else
			{
				ptSimConfInfo->m_tChairMan.SetMcuId( 0 );
			}

			if( !ptSimConfInfo->m_tSpeaker.IsNull() )
			{
				ptSimConfInfo->m_tSpeaker.SetMcuId( (u8)g_cMtAdpApp.m_wMcuNetId );
			}
			else
			{
				ptSimConfInfo->m_tSpeaker.SetMcuId( 0 );
			}
			
			CONF_CTRL_STRICT((u16)h_ctrl_SimpleConfInfoInd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
		}
		break;
		
	case MCU_MT_GETCONFINFO_NACK:			
		break;
		
	case MCU_MT_MTMUTE_CMD:	//far site mute: makeTerQuiet
	case MCU_MT_MTDUMB_CMD:	//near site mute: makeTerMute
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );

			if(!BODY_LEN_GE( cServMsg, sizeof(TMt) + sizeof(u8)) )
				return;

			u8 byMode = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMt));
			
			TTERLABEL tTer;
			tTer.SetTerminalLabel( (u8)g_cMtAdpApp.m_wMcuNetId, m_byMtId );
			
			u8 byAction = (u8)( byMode == 1 ? turn_on : turn_off );
			u8 byMsgBuf[sizeof(TTERLABEL) + 1];
			memcpy( byMsgBuf, (void*)&tTer, sizeof(tTer) );
			memcpy( byMsgBuf + sizeof(tTer), &byAction, 1 );

			if( pcMsg->event == MCU_MT_MTMUTE_CMD )
				CONF_CTRL_STRICT((u16)h_ctrl_makeTerQuietCmd, byMsgBuf, sizeof(byMsgBuf) );
			else
				CONF_CTRL_STRICT((u16)h_ctrl_makeTerMuteCmd, byMsgBuf, sizeof(byMsgBuf) );	
		}
		break;
/*
	case MCU_MT_SETMTVIDSRC_CMD:
		{
			CServMsg cServMsg(pcMsg->content, pcMsg->length);
			if(!BODY_LEN_GE(cServMsg, sizeof(TMt) + sizeof(u8))) return;
			
			u8 byVidSrc = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMt));
			CONF_CTRL_STRICT(h_ctrl_termVideoSrcCmd, &byVidSrc, sizeof(u8));
		}
	    break;
*/
	case MCU_MT_STARTVAC_NOTIF:
	case MCU_MT_STARTVAC_ACK:	//voice activation control , granted/denied
	case MCU_MT_STARTVAC_NACK: 
		{
			u8 byResult;

			if( pcMsg->event == MCU_MT_STARTVAC_NACK )
				byResult = (u8)ctrl_denied ;	
			else
				byResult = (u8)ctrl_granted;
			
			CONF_CTRL_STRICT( (u16)h_ctrl_startVACRsp, &byResult, sizeof(u8) );
		}
		break;

	case MCU_MT_STOPVAC_NOTIF:
	case MCU_MT_STOPVAC_ACK: //NULL
	case MCU_MT_STOPVAC_NACK: //NULL
		{
			u8 byResult;

			if( pcMsg->event == MCU_MT_STOPVAC_NACK )
				byResult = (u8)ctrl_denied;
			else
				byResult = (u8)ctrl_granted;

			CONF_CTRL_STRICT((u16)h_ctrl_stopVACRsp, &byResult, sizeof(u8));
		}
		break;

	case MCU_MT_STARTDISCUSS_NOTIF: 
	case MCU_MT_STARTDISCUSS_ACK:	//start discuss
	case MCU_MT_STARTDISCUSS_NACK:  //
		{
			//由于MT侧分定制混音及会议讨论，而两功能共用这一消息。
			//故把Notify单独提出来处理

			CServMsg cServMsg( pcMsg->content, pcMsg->length );

			u8 byMixMode = *(u8 *)cServMsg.GetMsgBody();

			u8 byResult;
			
			if( pcMsg->event == MCU_MT_STARTDISCUSS_NACK )
				byResult = (u8)ctrl_denied;	
			else
				byResult = (u8)ctrl_granted;

			// [12/1/2010 xliang] no send to T3,R3 mt as it ignore the mode in the msgbody
			if(pcMsg->event == MCU_MT_STARTDISCUSS_NOTIF
				&& ( m_byMtVer == (u8)emMtVer40R4 
				||m_byMtVer == (u8)emMtVer36 
				) 
				//&& ( byMixMode == mcuVacMix )
				)
			{
				//u8 byVacResult = ctrl_granted;
				
				//CONF_CTRL_STRICT( h_ctrl_startVACRsp, &byVacResult, sizeof(u8) );
				
				break;
			}

			cServMsg.SetMsgBody(&byResult, sizeof(byResult));
			cServMsg.CatMsgBody(&byMixMode, sizeof(byMixMode));

			CONF_CTRL_STRICT((u16)h_ctrl_startDiscussRsp, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
		}
		break;

	case MCU_MT_STOPDISCUSS_NOTIF: //NULL	
	case MCU_MT_STOPDISCUSS_ACK:	//stop discuss
	case MCU_MT_STOPDISCUSS_NACK:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );

			u8 byMixMode = *(u8 *)cServMsg.GetMsgBody();
			u8 byResult;

			if( pcMsg->event == MCU_MT_STOPDISCUSS_NACK )
				byResult = (u8)ctrl_denied;	
			else
				byResult = (u8)ctrl_granted;

			//20101213_tzy 增加混音模式字段
			cServMsg.SetMsgBody(&byResult, sizeof(byResult));
			cServMsg.CatMsgBody(&byMixMode, sizeof(byMixMode));

			CONF_CTRL_STRICT((u16)h_ctrl_stopDiscussRsp, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
		}
		break;

	case MCU_MT_STARTVMP_ACK:	//start VMP, TVMPParam
	case MCU_MT_STARTVMP_NACK:	//NULL
	case MCU_MT_STARTVMP_NOTIF:	//NULL
		{
			u8 byResult;

			if( pcMsg->event == MCU_MT_STARTVMP_NACK )
				byResult = (u8)ctrl_denied;	
			else
				byResult = (u8)ctrl_granted;

			CONF_CTRL_STRICT( (u16)h_ctrl_startVMPRsp, &byResult, sizeof(u8) );
		}
		break;

	case MCU_MT_STOPVMP_ACK:	//stop VMP, NULL
		//zbq[06/24/2008] ack不回granted，等ntf才回，防止时序问题
		break;

	case MCU_MT_STOPVMP_NOTIF:
	case MCU_MT_STOPVMP_NACK:	//NULL
		{
			u8 byResult;

			if( pcMsg->event == MCU_MT_STOPVMP_NACK )
				byResult = (u8)ctrl_denied;	
			else
				byResult = (u8)ctrl_granted;

			CONF_CTRL_STRICT( (u16)h_ctrl_stopVMPRsp, &byResult, sizeof(u8) );
		}
		break;
		
	case MCU_MT_CHANGEVMPPARAM_ACK:	//change VMP parameters, TVMPParam
	case MCU_MT_CHANGEVMPPARAM_NACK://NULL
		{
			u8 byResult;

			if( pcMsg->event == MCU_MT_CHANGEVMPPARAM_ACK )
				byResult = (u8)ctrl_granted;	
			else
				byResult = (u8)ctrl_denied;

			CONF_CTRL_STRICT( (u16)h_ctrl_changeVMPParamRsp, &byResult, sizeof(u8) );
		}
		break;

	case MCU_MT_GETVMPPARAM_ACK:	//inquiry about VMP params, TVMPParam
	case MCU_MT_VMPPARAM_NOTIF:		//TVMPParam
		{
			CServMsg cServMsg(pcMsg->content, pcMsg->length);

			if(!BODY_LEN_GE(cServMsg, sizeof(TVMPParam))) 
				return;			

			TVMPParam *ptVmpParam = (TVMPParam *)cServMsg.GetMsgBody();
			for( u8 byLoop = 0; byLoop < ptVmpParam->GetMaxMemberNum(); byLoop++)
			{
				if( ptVmpParam->GetVmpMember(byLoop) == NULL )
				{
					continue;
				}
				//if( ptVmpParam->GetVmpMember(byLoop)->GetMcuId() == 0 && ptVmpParam->GetVmpMember(byLoop)->GetMtId() == 0)
				// 直接判成员为空
				if (ptVmpParam->GetVmpMember(byLoop)->IsNull())
				{
					// 通知终端为空
					// 加保护,考虑终控台兼容,mcuid与mtid都设为0
					ptVmpParam->GetVmpMember(byLoop)->SetMcuId( 0 );
					ptVmpParam->GetVmpMember(byLoop)->SetMtId( 0 );
				}
				else
				{
					ptVmpParam->GetVmpMember(byLoop)->SetMcuId( (u8)g_cMtAdpApp.m_wMcuNetId );
				}
			}

			CONF_CTRL_STRICT( (u16)h_ctrl_getVMPParamRsp, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		}
		break;
		
	case MCU_MT_GETVMPPARAM_NACK:
		{
			TVMPParam tVMPParam;
			memset( &tVMPParam, 0, sizeof(tVMPParam) );
			CONF_CTRL_STRICT( (u16)h_ctrl_getVMPParamRsp, (u8*)&tVMPParam, sizeof(tVMPParam) );
		}
		break;

	case MCU_MT_STARTBROADCASTMT_ACK:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			if( !BODY_LEN_GE(cServMsg, sizeof(TMt) + sizeof(TTransportAddr) * 2 + 
				             sizeof(TMediaEncrypt) * 2 + sizeof(TDoublePayload) * 2 ) ) 
				return;
			
			TMt *ptMt = (TMt*)cServMsg.GetMsgBody();

			TTERLABEL tTer;
			TTransportAddr *ptTransAddrVideo = (TTransportAddr*)(cServMsg.GetMsgBody()+sizeof(TMt));
			TTransportAddr *ptTransAddrAudio = (TTransportAddr*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TTransportAddr));
			TMediaEncrypt  *ptEncryptVidio   = (TMediaEncrypt *)(cServMsg.GetMsgBody() + sizeof(TMt) + 2*sizeof(TTransportAddr));
			TDoublePayload *ptDVPayload      = (TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(TMt) + 2*sizeof(TTransportAddr) + sizeof(TMediaEncrypt));
			TMediaEncrypt  *ptEncryptAudio   = (TMediaEncrypt *)(cServMsg.GetMsgBody() + sizeof(TMt) + 2*sizeof(TTransportAddr) + sizeof(TMediaEncrypt)+sizeof(TDoublePayload));
			TDoublePayload *ptDAPayload      = (TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(TMt) + 2*sizeof(TTransportAddr) + 2*sizeof(TMediaEncrypt)+sizeof(TDoublePayload));

			u8 abyBuf[sizeof(TTERLABEL) + sizeof(u32) + sizeof(u16) * 2 + sizeof(u8) * 4 + sizeof(TEncryptSync) * 2];

			u32 dwRcvIP   = htonl(ptTransAddrVideo->GetIpAddr());
			u16 wVidPort  = htons(ptTransAddrVideo->GetPort());
			u16 wAudPort  = htons(ptTransAddrAudio->GetPort());
			//tTer.SetTerminalLabel( (u8)ptMt->GetMcuId(), ptMt->GetMtId() );
			CMtAdpUtils::ConverTMt2TTERLABEL( *ptMt, tTer);

			TEncryptSync tVidEncryptSync;
			TEncryptSync tAudEncryptSync;
			u8 byAudRealPT = ptDAPayload->GetRealPayLoad();
			u8 byVidRealPT = ptDVPayload->GetRealPayLoad();

			u8 byVidEncryptType = CMtAdpUtils::EncryptTypeIn2Out(ptEncryptVidio->GetEncryptMode());
			if( (u8)emEncryptTypeNone != byVidEncryptType )
			{				
				s32 nKeyLen = 0;
				u8  abyKey[MAXLEN_KEY] = {0};
				ptEncryptVidio->GetEncryptKey(abyKey, &nKeyLen);				
				tVidEncryptSync.SetH235Key(abyKey, nKeyLen);			
				tVidEncryptSync.SetSynchFlag(ptDVPayload->GetActivePayload());
				tVidEncryptSync.host2net(TRUE);
			}
			
			u8 byAudbyEncryptType = CMtAdpUtils::EncryptTypeIn2Out(ptEncryptAudio->GetEncryptMode());
			if((u8)emEncryptTypeNone != byAudbyEncryptType)
			{				
				s32 nKeyLen = 0;
				u8  abyKey[MAXLEN_KEY] = {0};
				ptEncryptAudio->GetEncryptKey(abyKey, &nKeyLen);				
				tAudEncryptSync.SetH235Key(abyKey, nKeyLen);			
				tAudEncryptSync.SetSynchFlag(ptDAPayload->GetActivePayload());
				tAudEncryptSync.host2net(TRUE);
			}

			//组播终端应答   MC->ter,  TTERLABEL(目的终端) + u32(ip,如果失败ip为0) 
			//                       + u16(音频端口号)     + u16(视频端口号)
			//                       + u8(音频真实载荷值)导用苄畔? + &tTer, sizeof(tTer) );
            //组播终端应答   MC->ter,  TTERLABEL(目的终端) + u32(ip,如果失败ip为0) 
            //                       + u16(音频端口号)     + u16(视频端口号)
			//                       + u8(音频真实载荷值)  + u8(视频真实载荷值)
			//                       + u8(音频加密类型)    + u8(视频加密类型)
			//                       + TEncryptSync(音频加密信息) + TEncryptSync(视频加密信息)
			memcpy( abyBuf, &tTer, sizeof(tTer) );
			memcpy( abyBuf + sizeof(tTer), &dwRcvIP, sizeof(dwRcvIP) );
			memcpy( abyBuf + sizeof(tTer) + sizeof(u32), &wAudPort, sizeof(wAudPort) );
			memcpy( abyBuf + sizeof(tTer) + sizeof(u32) + sizeof(u16), &wVidPort, sizeof(wVidPort) );
			memcpy( abyBuf + sizeof(tTer) + sizeof(u32) + sizeof(u16) + sizeof(u16), &byAudRealPT, sizeof(u8) );
			memcpy( abyBuf + sizeof(tTer) + sizeof(u32) + sizeof(u16) + sizeof(u16) + sizeof(u8), &byVidRealPT, sizeof(u8) );
			memcpy( abyBuf + sizeof(tTer) + sizeof(u32) + sizeof(u16) + sizeof(u16) + sizeof(u8) + sizeof(u8), &byAudbyEncryptType, sizeof(u8) );
			memcpy( abyBuf + sizeof(tTer) + sizeof(u32) + sizeof(u16) + sizeof(u16) + sizeof(u8) + sizeof(u8) + sizeof(u8), &byVidEncryptType, sizeof(u8) );
			memcpy( abyBuf + sizeof(tTer) + sizeof(u32) + sizeof(u16) + sizeof(u16) + sizeof(u8) + sizeof(u8) + sizeof(u8) + sizeof(u8), &tAudEncryptSync, sizeof(TEncryptSync) );
			memcpy( abyBuf + sizeof(tTer) + sizeof(u32) + sizeof(u16) + sizeof(u16) + sizeof(u8) + sizeof(u8) + sizeof(u8) + sizeof(u8) + sizeof(TEncryptSync), &tVidEncryptSync, sizeof(TEncryptSync) );
			
			CONF_CTRL_STRICT( (u16)h_ctrl_multicastTerRsp, abyBuf, sizeof(abyBuf) );
		}
		break;

	case MCU_MT_STARTBROADCASTMT_NACK:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );

			if( !BODY_LEN_GE( cServMsg, sizeof(TMt) ) )
				return;
			
			TMt *ptMt = (TMt*)cServMsg.GetMsgBody();
			TTERLABEL tTer;

			u8 byBuf[sizeof(TTERLABEL) + sizeof(u32) + sizeof(u16) * 2];
			
			u32 dwIp = 0;
			u16 wPortVideo = 0;
			u16 wPortAudio = 0;
            //tTer.SetTerminalLabel( (u8)ptMt->GetMcuId(), ptMt->GetMtId() );
			CMtAdpUtils::ConverTMt2TTERLABEL( *ptMt, tTer );

			memcpy( byBuf, &tTer, sizeof(tTer) );
			memcpy( byBuf + sizeof(tTer), &dwIp, sizeof(dwIp) );
			memcpy( byBuf + sizeof(tTer) + sizeof(u32), &wPortVideo, sizeof(wPortVideo) );
			memcpy( byBuf + sizeof(tTer) + sizeof(u32) + sizeof(u16), &wPortAudio, sizeof(wPortAudio) );

			CONF_CTRL_STRICT( (u16)h_ctrl_multicastTerRsp, byBuf, sizeof(byBuf) );
		}
		break;

	//law case system specific msgs
	case MCU_MT_STARTMTSELME_ACK:  //ack for forcing other mt to view me
	case MCU_MT_STARTMTSELME_NACK: 
		{
			u8 byResult = (u8)( pcMsg->event == MCU_MT_STARTMTSELME_ACK ? ctrl_granted : ctrl_denied );
			CONF_CTRL_STRICT( (u16)h_ctrl_letTerViewMeRsp, &byResult, sizeof(u8) );
		}
		break;
	
	case MCU_MT_GETMTSELSTUTS_NACK:
		{
			u8 byTerNum = 0;
			CONF_CTRL_STRICT( (u16)h_ctrl_whoViewThisTerRsp, &byTerNum, sizeof(u8) );
		}
		break;

	case MCU_MT_GETMTSELSTUTS_ACK:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );

			if( !BODY_LEN_GE(cServMsg, sizeof(TTERLABEL) * 2) ) 
				return;
			
			TMt *pTMtViewed  = (TMt*)cServMsg.GetMsgBody();
			TMt *pTMtViewing = (TMt*)( cServMsg.GetMsgBody() + sizeof(TMt) );

			u8 byBuf[sizeof(TTERLABEL) + sizeof(u8) + sizeof(TTERLABEL)];
			
			((PTTERLABEL)byBuf)->SetTerminalLabel( (u8)g_cMtAdpApp.m_wMcuNetId, pTMtViewed->GetMtId() );

			byBuf[sizeof(TTERLABEL)] = 1;
			((PTTERLABEL)(byBuf + sizeof(TTERLABEL) + 1))->SetTerminalLabel( (u8)g_cMtAdpApp.m_wMcuNetId,pTMtViewing->GetMtId() );

            //[20110729 zhushz]pc-lint err:545
			CONF_CTRL_STRICT((u16)h_ctrl_whoViewThisTerRsp, byBuf, sizeof(byBuf));
		}
		break;

	case MCU_MT_DELAYCONF_NOTIF:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			if(!BODY_LEN_GE(cServMsg, sizeof(u16))) return;
			
			u16 wMinutes = *(u16*)cServMsg.GetMsgBody();
			CONF_CTRL_STRICT((u16)h_ctrl_delayConfTimeInd, &wMinutes, sizeof(wMinutes));
		}
		break;

	case MCU_MT_DELAYCONF_ACK:
	case MCU_MT_DELAYCONF_NACK:	
		{
			u8 byResult;
			if(pcMsg->event == MCU_MT_DELAYCONF_ACK)
				byResult = (u8)ctrl_granted;	
			else
				byResult = (u8)ctrl_denied;

			CONF_CTRL_STRICT((u16)h_ctrl_delayConfTimeRsp, &byResult, sizeof(u8));
		}
		break;
		
	case MCU_MT_CONFWILLEND_NOTIF:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			if(!BODY_LEN_GE(cServMsg, sizeof(u16))) return;

			u16 leftTime = *(u16*)cServMsg.GetMsgBody();			
			CONF_CTRL_STRICT( (u16)h_ctrl_confWillEndInd, (u8*)&leftTime, sizeof(leftTime) );			
		}
		break;

	case MCU_MT_GETMTSTATUS_ACK:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length ) ;

			TMtStatus tMtStatus = *(TMtStatus*)(cServMsg.GetMsgBody());
			
			TTERLABEL tTer;
			tMtStatus.SetMcuId( (u8)g_cMtAdpApp.m_wMcuNetId );
			tTer.SetTerminalLabel( (u8)tMtStatus.GetMcuId(), tMtStatus.GetMtId() );

			_TTERSTATUS tTerStatus = CMtAdpUtils::TMtStatusIn2Out(tMtStatus);
			
			//非3.6终端直接发出
			if ( (u8)emMtVer36 != m_byMtVer )
			{
				s8    achBuf[sizeof(TTERLABEL) + sizeof(_TTERSTATUS)];
				memcpy(achBuf, &tTer, sizeof(tTer));
				memcpy(achBuf + sizeof(tTer), &tTerStatus, sizeof(tTerStatus));
				CONF_CTRL_STRICT((u16)h_ctrl_destterStatusRsp, achBuf, sizeof(achBuf));
			}
			else
			{
				_TTERSTATUS_VER36 tTerStatus36;
				memset( &tTerStatus36, 0, sizeof(tTerStatus36) );
				CStructConvert::TERSTATUSVer40ToVer36( tTerStatus36, tTerStatus );

				s8    achBuf[sizeof(TTERLABEL) + sizeof(_TTERSTATUS_VER36)];
				memcpy(achBuf, &tTer, sizeof(tTer));
				memcpy(achBuf + sizeof(tTer), &tTerStatus36, sizeof(tTerStatus36) );
				CONF_CTRL_STRICT((u16)h_ctrl_destterStatusRsp, achBuf, sizeof(achBuf));
			}
		}
		break;

	case MCU_MT_GETMTSTATUS_NACK:
		break;

	//获得终端外置矩阵类型
	case MCU_MT_EXMATRIX_GETINFO_CMD:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			CONF_CTRL_STRICT( (u16)h_ctrl_matrixExGetInfoCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		}
		break;

	//设置终端外置矩阵端口号
	case MCU_MT_EXMATRIX_SETPORT_CMD:
		{
			//将消息下放到终端层
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			CONF_CTRL_STRICT( (u16)h_ctrl_matrixExSetPortCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );						
		}
		break;

	//获得终端外置矩阵端口号
	case MCU_MT_EXMATRIX_GETPORT_REQ:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			CONF_CTRL_STRICT( (u16)h_ctrl_matrixExGetPortReq, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
			
            //先直接回MCU一个ACK
            BuildAndSendMsgToMcu( MT_MCU_EXMATRIX_GETPORT_ACK );
		}
		break;
		
	//设置终端外置矩阵端口名
	case MCU_MT_EXMATRIX_SETPORTNAME_CMD:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			CONF_CTRL_STRICT( (u16)h_ctrl_matrixExSetPortNameCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		}
		break;

	//获得所有的终端外置矩阵的端口名(字符串数组)
	case MCU_MT_EXMATRIX_GETALLPORTNAME_CMD:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			CONF_CTRL_STRICT( (u16)h_ctrl_matrixExGetAllPortNameCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		}
		break;

	//获取内置矩阵方案所有内容
	case MCU_MT_MATRIX_GETALLSCHEMES_CMD:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			CONF_CTRL_STRICT( (u16)h_ctrl_innerMatrixGetAllSchemesCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		}
		break;

	//获得内置矩阵方案一个内容
	case MCU_MT_MATRIX_GETONESCHEME_CMD:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			CONF_CTRL_STRICT( (u16)h_ctrl_innerMatrixGetOneSchemeCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		}
		break;

	//保存内置矩阵方案
	case MCU_MT_MATRIX_SAVESCHEME_CMD:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			CONF_CTRL_STRICT( (u16)h_ctrl_innerMatrixSaveSchemeCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		}
		break;
		
	//调用指定内置矩阵方案为当前方案
	case MCU_MT_MATRIX_SETCURSCHEME_CMD:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			CONF_CTRL_STRICT( (u16)h_ctrl_innerMatrixSetCurSchemeCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		}
		break;

	//获取内置矩阵当前方案
	case MCU_MT_MATRIX_GETCURSCHEME_CMD:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			CONF_CTRL_STRICT( (u16)h_ctrl_innerMatrixCurSchemeCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		}
		break;

    //扩展视频源
    case MCU_MT_GETVIDEOSOURCEINFO_CMD:
        {
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
            CONF_CTRL_STRICT( (u16)h_ctrl_getVideoSourceInfoCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
        }
        break;

    case MCU_MT_SETVIDEOSOURCEINFO_CMD:
        {
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
            CONF_CTRL_STRICT( (u16)h_ctrl_setVideoSourceInfoCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
        }
        break;
        
    //视频源切换
    case MCU_MT_SELECTEXVIDEOSRC_CMD:
        {
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
            CONF_CTRL_STRICT( (u16)h_ctrl_selectExVideoSourceCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
        }
        break;

    //获取终端码率请求
    case MCU_MT_GETBITRATEINFO_REQ:
        CONF_CTRL_STRICT( (u16)h_ctrl_terBitrateInfoReq, NULL, 0 );
        BuildAndSendMsgToMcu(MT_MCU_GETBITRATEINFO_ACK);
        break;

    //卫星会议组播地址通知
/*    case MCU_MT_SATDCONFADDR_NOTIF:
        {
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
            CONF_CTRL_STRICT( h_ctrl_SatDConfAddrInd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
        }
        break;*/

    case MCU_MT_SATDCONFCHGADDR_CMD:
        {
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
            u8 byChnnlType = *(u8*)cServMsg.GetMsgBody();
            TTransportAddr tAddr = *(TTransportAddr *)(cServMsg.GetMsgBody() + sizeof(u8));
            TCHANMULTICAST tMultiAddr;
            if (LOGCHL_VIDEO == byChnnlType)
            {
                tMultiAddr.SetChanID((u8)emChanVideo1);
            }
            else if (LOGCHL_AUDIO == byChnnlType)
            {
                tMultiAddr.SetChanID((u8)emChanAudio);
            }
            else if (LOGCHL_SECVIDEO == byChnnlType)
            {
                tMultiAddr.SetChanID((u8)emChanVideo2);
            }
            tMultiAddr.SetChanAddr(tAddr.m_dwIpAddr, tAddr.m_wPort);
            
            if((s32)act_err == kdvSendConfCtrlMsg(m_hsCall, (u16)h_ctrl_setChanMultiCastAddr, (void*)&tMultiAddr, sizeof(tMultiAddr)) )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Send h_ctrl_setChanMultiCastAddr sent to mt.%d failed.\n", m_byMtId);	
			}
			else
			{
				MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "h_ctrl_setChanMultiCastAddr sent to mt.%d \n", m_byMtId);
			}
    
            u32 dwChanIp = tMultiAddr.GetChanIp();        
            MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[SATDCONFCHGADDR] ChanId=%d Ip=%u.%u.%u.%u Port=0x%x\n", 
                    tMultiAddr.GetChanID(), QUADADDR(dwChanIp), tMultiAddr.GeChantPort());
        }
        break;

	case MCU_MT_VIDEOFPSCHANGE_CMD:
		{
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
			CONF_CTRL_STRICT( (u16)h_ctrl_videoFpsChangeCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		}
		break;
        
    //终端视频参数改变命令
    case MCU_MT_VIDEOPARAMCHANGE_CMD: //xliang [080731] 
        {
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
			CONF_CTRL_STRICT( (u16)h_ctrl_videoFormatChangeCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );

// 			u8 byChnnlNo = *(cServMsg.GetMsgBody());     //第一路通道or第2路通道
// 			u8 byVideoResolutionType = *(cServMsg.GetMsgBody()+2);  //分辨率
// 			CServMsg cServFormatChangeMsg;
// 			cServFormatChangeMsg.SetMsgBody(&byChnnlNo,sizeof(u8));
// 			cServFormatChangeMsg.CatMsgBody(&byVideoResolutionType,sizeof(u8));
// 			//兼容老MT
			//CONF_CTRL_STRICT( h_ctrl_videoFormatChangeCmd, cServFormatChangeMsg.GetMsgBody(), cServFormatChangeMsg.GetMsgBodyLen() );
            
//             /*xliang [080731] 
//             **如果信道类型非h264,则消息体同Mcuvc传过来的，直接发往MT
//             **如果信道类型是h264,则消息体最后2个值要改变成h264能力集中的信息*/
// 
//             u8 byChannlType = *(cServMsg.GetMsgBody()+1);//信道类型(H264,MPEG4等)
//             if ( MEDIA_TYPE_H264 != byChannlType)
//             {
//                 CONF_CTRL_STRICT( h_ctrl_videoParamChangeCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
//             }
//             else 
//             {
//                 
//                 u8 byChannlVidFPS = *(cServMsg.GetMsgBody()+3);         //帧率
// 
//                 TH264VideoCap tH264VCap;
//                 tH264VCap.SetResolution(byVideoResolutionType,byChannlVidFPS);
//                 s32 nH264MaxtMBPS = tH264VCap.GetMaxMBPS();
//                 s32 nH264MaxFS = tH264VCap.GetMaxFS();
//                 
//                 CServMsg cServH264Msg;
//                 cServH264Msg.SetMsgBody(&byChnnlNo, sizeof(u8));
//                 cServH264Msg.CatMsgBody(&byChannlType,sizeof(u8));
//                 cServH264Msg.CatMsgBody((u8*)&nH264MaxtMBPS,sizeof(s32));
//                 cServH264Msg.CatMsgBody((u8*)&nH264MaxFS,sizeof(s32));
// 
//                 CONF_CTRL_STRICT( h_ctrl_videoParamChangeCmd, cServH264Msg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
//             
//             }
        }
        break;
        
    /* add by wangliang 2006-12-26 for MT volume change begin */
    //终端音量改变命令
    case MCU_MT_SETMTVOLUME_CMD:
        {
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
            CONF_CTRL_STRICT( (u16)h_ctrl_setAudioVolumeCmd, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
        }
        break;
    /* add by wangliang 2006-12-26 for MT volume change end */

    case MCU_MT_SOMEOFFERRING_NOTIF:
        {
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
            CONF_CTRL_STRICT( (u16)h_ctrl_someOfferingNtf, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
        }
        break;

	case MCU_MT_MTSPEAKSTATUS_NTF:
        {
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
            CONF_CTRL_STRICT( (u16)h_ctrl_mtSpeakStatusNtf, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
        }
        break;
		
    case MCU_MT_CONFSPEAKMODE_NTF:
        {
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
            CONF_CTRL_STRICT( (u16)h_ctrl_confSpeakModeNtf, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
        }
        break;

//     case MCU_MT_GETMTVERID_REQ:
//         {
//             CServMsg cServMsg( pcMsg->content, pcMsg->length );
//             if ( m_byMtVer < emMtVer40R5 )
//             {
//                 cServMsg.SetErrorCode(ERR_MCU_MT_NOVERINFO);
//                 cServMsg.SetEventId( MT_MCU_GETMTVERID_NACK );
//                 SendMsgToMcu( cServMsg );
//             }
//             else
//             {
//                 CONF_CTRL_STRICT( h_ctrl_mtVerInfoReq, NULL, 0 );
//             }
//         }
// 		break;

	case MCU_MT_STARTSWITCHVMPMT_ACK:
	case MCU_MT_STARTSWITCHVMPMT_NACK:
		{
			u8 bSuccess = 0;
			if( MCU_MT_STARTSWITCHVMPMT_ACK == pcMsg->event)
			{
				bSuccess = 1;
			}
			CONF_CTRL_STRICT( (u16)h_ctrl_mtSelectVmpRsp, &bSuccess, sizeof(u8) );
		}
		break;
	case MCU_MT_RELEASEMT_ACK:
	case MCU_MT_RELEASEMT_NACK:
		{
			CServMsg cServMsg( pcMsg->content, pcMsg->length ); 
			CONF_CTRL_STRICT( (u16)h_ctrl_askVcsOccupyPermitResp, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
		}
		break;
    case MCU_MT_APPLYCANCELSPEAKER_ACK:
        {
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
            
            u8 byResult = 1;
            cServMsg.CatMsgBody((u8*)&byResult, sizeof(u8));
            CONF_CTRL_STRICT( (u16)h_ctrl_applycancelspeakerRsp, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
        }
        break;
    case MCU_MT_APPLYCANCELSPEAKER_NACK:
        {
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
            
            u8 byResult = 0;
            cServMsg.CatMsgBody((u8*)&byResult, sizeof(u8));
            CONF_CTRL_STRICT( (u16)h_ctrl_applycancelspeakerRsp, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
        }
        break;
	// [pengjie 2011/4/12] 终端视频源别名上报支持
	case MCU_MT_GETMTVIDEOALIAS_CMD:
		{		
			CONF_CTRL_STRICT( (u16)h_ctrl_videoAliasCmd, NULL, 0 );
		}
		break;
    ////[5/4/2011 zhushengze]VCS控制发言人发双流
    case MCU_MT_CHANGEMTSECVIDSEND_CMD:
        {
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
            u8 byIsSendDStream = *(u8*)cServMsg.GetMsgBody();
            CONF_CTRL_STRICT((u16)h_ctrl_ChgSecVidSendCmd, &byIsSendDStream, sizeof(u8));
        }
        break;

		//[2/23/2012 zhushengze]界面、终端消息透传
    case MCU_MT_TRANSPARENTMSG_NOTIFY:
        {
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
            OspPrintf( TRUE, FALSE, "[>>MCU_MT_TRANSPARENTMSG_NOTIFY]h_ctrl_transparentMsgNtf msg len is %d\n", cServMsg.GetMsgBodyLen());
            CONF_CTRL_STRICT((u16)h_ctrl_transparentMsgNtf, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
        }
        break;

	default:
		UNEXPECTED_MESSAGE(pcMsg->event);
		break;
	}
	return;
}

/*=============================================================================
函 数 名： ProcMcuMcuSpeakStatusNtf
功    能： 
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
CServMsg& cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMtAdpInst::ProcMcuMcuSpeakStatusNtf( CServMsg &cServMsg, CServMsg &cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	
	TMsgHeadMsg tHeadMsg;
	TMtadpHeadMsg tMtadpHeadMsg = *(TMtadpHeadMsg* )cServMsg.GetMsgBody();
	u32 dwStatus = *( u32* )( cServMsg.GetMsgBody() + sizeof(TMtadpHeadMsg) );
	
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = ntohl( tMtadpHeadMsg.m_tMsgDst.m_dwPID );
	TMt tMt = GetMtFromPartId( tMtadpHeadMsg.m_tMsgDst.m_dwPID,TRUE );

	if( tMtadpHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		--tMtadpHeadMsg.m_tMsgDst.m_byCasLevel;
		tMt.SetMcuId( tMt.GetMtId() );
		tMt.SetMtId( tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[tMtadpHeadMsg.m_tMsgDst.m_byCasLevel] );
	}
	
	dwStatus = ntohl( dwStatus );

	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );
	
	cMsg.SetMsgBody( (u8 *)&tHeadMsg,sizeof(tHeadMsg) );	
	cMsg.CatMsgBody( (u8 *)&tMt,sizeof(TMt) );	
	cMsg.CatMsgBody( (u8 *)&dwStatus,sizeof(u32) );	
	
	MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "Recv ProcMcuMcuSpeakStatusNtf From Cascadelib.tMt(%d.%d) dwStatus.%d\n",                 
		tMt.GetMcuId(), tMt.GetMtId(), dwStatus);	
}

/*=============================================================================
  函 数 名： ProcMcuMcuApplySpeakerReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  05/13/2010    4.6         张宝卿                  创建
=============================================================================*/
void CMtAdpInst::ProcMcuMcuApplySpeakerReq(CServMsg& cServMsg, CServMsg& cMsg)
{
    cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );

	TMsgHeadMsg tHeadMsg;
    TMtadpHeadMsg tMtadpHeadMsg = *(TMtadpHeadMsg *)cServMsg.GetMsgBody();

	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = ntohl( tMtadpHeadMsg.m_tMsgSrc.m_dwPID );
    TMt tMt = GetMtFromPartId(tMtadpHeadMsg.m_tMsgSrc.m_dwPID, FALSE);

	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );

	cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
    cMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );
    cMsg.CatMsgBody( (u8*)(cServMsg.GetMsgBody() + sizeof(TMtadpHeadMsg) ), 
		              cServMsg.GetMsgBodyLen() - sizeof(TMtadpHeadMsg) 
					  );
}

/*=============================================================================
  函 数 名： ProcMcuMcuApplySpeakerRsp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  05/13/2010    4.6         张宝卿                  创建
=============================================================================*/
void CMtAdpInst::ProcMcuMcuApplySpeakerRsp(CServMsg& cServMsg, CServMsg& cMsg)
{
    cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );

	TMsgHeadMsg tHeadMsg;
    TMtadpHeadMsg tMtadpHeadMsg = *(TMtadpHeadMsg *)cServMsg.GetMsgBody();

	tMtadpHeadMsg.m_tMsgDst.m_dwPID = ntohl( tMtadpHeadMsg.m_tMsgDst.m_dwPID );
    TMt tMt = GetMtFromPartId( tMtadpHeadMsg.m_tMsgDst.m_dwPID, TRUE );

	if( tMtadpHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		--tMtadpHeadMsg.m_tMsgDst.m_byCasLevel;
		tMt.SetMcuId( tMt.GetMtId() );
		tMt.SetMtId( tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[tMtadpHeadMsg.m_tMsgDst.m_byCasLevel] );
	}

	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );
    
	cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(tHeadMsg) );
    cMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );
    cMsg.CatMsgBody( (u8*)(cServMsg.GetMsgBody() + sizeof(TMtadpHeadMsg) ), 
		              cServMsg.GetMsgBodyLen() - sizeof(TMtadpHeadMsg) );
}

/*=============================================================================
  函 数 名： ProcMcuMcuSpeakModeNotify
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  05/13/2010    4.6         张宝卿                  创建
=============================================================================*/
void CMtAdpInst::ProcMcuMcuSpeakModeNotify( CServMsg &cServMsg, CServMsg &cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
    u8 byMode = *(u8 *)cServMsg.GetMsgBody();
    

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMcuSpeakModeNotify] dwMode Value Is %d\n",byMode );
	
    
    cMsg.SetMsgBody( (u8 *)&byMode, sizeof(u8) );
    cMsg.CatMsgBody( (u8*)(cServMsg.GetMsgBody() + sizeof(u8) ), 
		              cServMsg.GetMsgBodyLen() - sizeof(u8) );
}

/*=============================================================================
  函 数 名： ProcMcuMcuCancelMeSpeakerReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  05/13/2010    4.6         张宝卿                  创建
=============================================================================*/
void CMtAdpInst::ProcMcuMcuCancelMeSpeakerReq(CServMsg &cServMsg, CServMsg &cMsg)
{
    cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	TMsgHeadMsg tHeadMsg;
    TMtadpHeadMsg tMtadpHeadMsg = *(TMtadpHeadMsg *)cServMsg.GetMsgBody();

	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = ntohl( tMtadpHeadMsg.m_tMsgSrc.m_dwPID );
    TMt tMt = GetMtFromPartId(tMtadpHeadMsg.m_tMsgSrc.m_dwPID, FALSE);
	
	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );
    
	cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(tHeadMsg) );
    cMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );
    cMsg.CatMsgBody( (u8*)(cServMsg.GetMsgBody() + sizeof(tMtadpHeadMsg) ), 
		              cServMsg.GetMsgBodyLen() - sizeof(tMtadpHeadMsg) );
}

/*=============================================================================
  函 数 名： ProcMcuMcuCancelMeSpeakerRsp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  05/13/2010    4.6         张宝卿                  创建
=============================================================================*/
void CMtAdpInst::ProcMcuMcuCancelMeSpeakerRsp(CServMsg &cServMsg, CServMsg &cMsg)
{
    cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );

	TMsgHeadMsg tHeadMsg;
    TMtadpHeadMsg tMtadpHeadMsg = *(TMtadpHeadMsg *)cServMsg.GetMsgBody();

	tMtadpHeadMsg.m_tMsgDst.m_dwPID = ntohl( tMtadpHeadMsg.m_tMsgDst.m_dwPID );
    TMt tMt = GetMtFromPartId( tMtadpHeadMsg.m_tMsgDst.m_dwPID, TRUE );

	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );
    
	cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(tHeadMsg) );
    cMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );
    cMsg.CatMsgBody( (u8*)(cServMsg.GetMsgBody() + sizeof(TMtadpHeadMsg) ), 
		              cServMsg.GetMsgBodyLen() - sizeof(TMtadpHeadMsg)
					  );
}

/*=============================================================================
  函 数 名： ProcMcuMcuApplySpeakerReq
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CServMsg& cServMsg
             CServMsg& cMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  05/13/2010    4.6         张宝卿                  创建
=============================================================================*/
void CMtAdpInst::ProcMcuMcuApplySpeakerNtf(CServMsg& cServMsg, CServMsg& cMsg)
{
    cMsg.SetServMsg(cServMsg.GetServMsg(),
                    cServMsg.GetServMsgLen()-cServMsg.GetMsgBodyLen());

	TMt tNtfMt = *(TMt*)cServMsg.GetMsgBody();
	cMsg.SetMsgBody( (u8 *)&tNtfMt, sizeof(tNtfMt) );
}


/*=============================================================================
函 数 名： OnSendSpeakStatusNotify
功    能： 通知发言人的状态
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2010/5/12   3.6			周晶晶                  创建
=============================================================================*/
void CMtAdpInst::OnSendSpeakStatusNotify( CServMsg &cServMsg,HCHAN hsChan )
{
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TMt tMt = *(TMt * )( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );	
	u32 dwStatus = *(u32 *)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg) );
	
	TMtadpHeadMsg tMtadpHeadMsg;


	HeadMsgToMtadpHeadMsg( tHeadMsg,tMtadpHeadMsg );	
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tMt, &tMtadpHeadMsg.m_tMsgDst.m_bLocal );
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = htonl( tMtadpHeadMsg.m_tMsgDst.m_dwPID );

	dwStatus = htonl( dwStatus );
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	cMsg.SetMsgBody();
	//cMsg.CatMsgBody( (u8*)&tPartID,sizeof( TPartID ) );
	cMsg.CatMsgBody( (u8*)&tMtadpHeadMsg,sizeof( TMtadpHeadMsg ) );
	cMsg.CatMsgBody( (u8*)&dwStatus,sizeof( u32 ) );
	
	TNonStandardMsg tMsg;
	
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][OnSendSpeakStatusNotify]OnSendSpeakStatusNotify message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendSpeakStatusNotify] tMt(%d.%d.%d) dwPID:%08x level.%d \n",tMt.GetMcuId(),
		tMt.GetMtId(),tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
		ntohl( tMtadpHeadMsg.m_tMsgDst.m_dwPID ),tMtadpHeadMsg.m_tMsgDst.m_byCasLevel
		);
}


/*=============================================================================
函 数 名： OnSendMcuMcuSpeakModeNotify
功    能： 通知非直连终端加入混音
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2010/5/12   3.6			周晶晶                  创建
=============================================================================*/
void CMtAdpInst::OnSendMcuMcuSpeakModeNotify( CServMsg &cServMsg,HCHAN hsChan )
{
	u8 byMode = *(u8 * )cServMsg.GetMsgBody();
	
	
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	cMsg.SetMsgBody();
	cMsg.CatMsgBody( (u8*)&byMode,sizeof( u8 ) );
	
	TNonStandardMsg tMsg;
	
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][OnSendMcuMcuSpeakModeNotify]OnSendMcuMcuSpeakModeNotify message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendMcuMcuSpeakModeNotify] byMode Value Is %d\n",byMode );
	
	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );
	
}

/*=============================================================================
  函 数 名： ProcMcuMtInviteMtReq
  功    能： MCU发到MT的邀请入会处理函数
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg，消息体TMt+TMtAlias
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcMcuMtInviteMtReq( CMessage * const  pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	if( !BODY_LEN_GE(cServMsg, sizeof(TMt) + sizeof(TMtAlias)) ) 
		return;

	u8* pbyMsgBody = cServMsg.GetMsgBody();
	TMt tMt = *(TMt*)pbyMsgBody;
	pbyMsgBody += sizeof(tMt);

	TMtAlias tMtAlias  = *(TMtAlias*)pbyMsgBody;
	pbyMsgBody += sizeof(tMtAlias);

	TMtAlias tConfName = *(TMtAlias*)pbyMsgBody;
	pbyMsgBody += sizeof(tConfName);
	
	//  xsl [12/29/2004] 是否加密
	m_byEncrypt = *pbyMsgBody;
	pbyMsgBody += sizeof(m_byEncrypt);

	m_wCallRate = ntohs(*(u16*)pbyMsgBody);
	pbyMsgBody += sizeof(m_wCallRate);

	m_tLocalCapSupport = *((TCapSupport *)pbyMsgBody);

	pbyMsgBody += sizeof(m_tLocalCapSupport);
	// end modify by xsl

    m_wForceCallInfo = ntohs(*(u16*)pbyMsgBody);
	pbyMsgBody += sizeof(u16);

	m_byAdminLevel = *((u8*)pbyMsgBody);
    pbyMsgBody += sizeof(m_byAdminLevel);

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[InviteUnjoinedMt] Mcu AdminLevel.%d\n",m_byAdminLevel );

    m_tLocalCapEx = *((TCapSupportEx*)pbyMsgBody);
	pbyMsgBody += sizeof(TCapSupportEx);

	// 保存扩展能力集勾选 [12/8/2011 chendaiwei]
	memcpy(m_atMainStreamCapEX,pbyMsgBody,sizeof(m_atMainStreamCapEX));
	pbyMsgBody += sizeof(m_atMainStreamCapEX);

	memcpy(m_atDoubleStreamCapEX,pbyMsgBody,sizeof(m_atDoubleStreamCapEX));
	pbyMsgBody += sizeof(m_atMainStreamCapEX);

	//记录单双声道，目前用于AACLC,AACLD[3/16/2012 chendaiwei]
	//m_byAudioTrackNum = *pbyMsgBody;
	//pbyMsgBody++;

	TMtAlias tConfE164Alias = *((TMtAlias *)pbyMsgBody);
	pbyMsgBody+=sizeof(TMtAlias);

	BOOL32 bNPlusIPCallWithE164 = ((u8)(*pbyMsgBody) != 0);
	pbyMsgBody+=sizeof(u8);

	//保存会议音频能力集
	memcpy(m_atLocalAudioTypeDesc,pbyMsgBody,sizeof(m_atLocalAudioTypeDesc));

	if(STATE_IDLE != CurState() )
	{
		UNEXPECTED_MESSAGE(pcMsg->event);
		return;
	}
	
	//在Deamon实例将邀请消息发给空闲实例时做删除动作，邀请消息还是回到空闲实例，这时忽略邀请
	if( g_cMtAdpApp.IsMtInviteMsgInOsp(cServMsg.GetConfIdx(),tMt.GetMtId()) &&
		g_cMtAdpApp.IsNeglectInviteMsg( cServMsg.GetConfIdx(),tMt.GetMtId() ) )
	{
		g_cMtAdpApp.SetMtInviteMsgInOsp( cServMsg.GetConfIdx(),tMt.GetMtId(),FALSE );
		g_cMtAdpApp.SetNeglectInviteMsg( cServMsg.GetConfIdx(),tMt.GetMtId(),FALSE );
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[InviteUnjoinedMt] Fail to invite tMt.%d,because it's Neglect Invite\n",tMt.GetMtId() );
		return;
	}
	
	g_cMtAdpApp.SetMtInviteMsgInOsp( cServMsg.GetConfIdx(),tMt.GetMtId(),FALSE );
	g_cMtAdpApp.SetNeglectInviteMsg( cServMsg.GetConfIdx(),tMt.GetMtId(),FALSE );
	m_byDirect    = (u8)CALL_OUTGOING;
	m_byMtId	  = tMt.GetMtId();	
	m_cConfId     = cServMsg.GetConfId();
	m_byConfIdx	  = cServMsg.GetConfIdx();
	m_wSavedEvent = pcMsg->event;
	
	memcpy( &m_tConfAlias, &tConfName, sizeof(TMtAlias) );
	memcpy( &m_tConfE164Alias, &tConfE164Alias, sizeof(TMtAlias) );

    cServMsg.SetMsgBody();

	//establish mapping for subsequent messages
	g_cMtAdpApp.m_Msg2InstMap[m_byConfIdx][m_byMtId] = (u8)GetInsID();

	//zjj20100617 PU呼叫为包含323ID或E164号的IP呼叫
	BOOL32 bIsPuCall = FALSE;
	if ((u8)puAliasTypeIPPlusAlias == tMtAlias.m_AliasType)
	{
		bIsPuCall = TRUE;
		TMtAlias tPUAdpAlias;
		s8* pachAlias = (s8*)(tMtAlias.m_achAlias);
		tPUAdpAlias.m_tTransportAddr.SetIpAddr( tMtAlias.m_tTransportAddr.GetIpAddr() );
 		tPUAdpAlias.m_tTransportAddr.SetPort( tMtAlias.m_tTransportAddr.GetPort() );
		//pachAlias += sizeof(u32);
 		//tPUAdpAlias.SetH323Alias(pachAlias);
		if( (u8)mtAliasTypeE164 == CMtAdpUtils::GetMtAliasTypeFromString(pachAlias) )
		{
			tPUAdpAlias.SetE164Alias(pachAlias);
			tPUAdpAlias.m_AliasType = (u8)mtAliasTypeE164;
		}
		else
		{
			tPUAdpAlias.SetH323Alias(pachAlias);
			tPUAdpAlias.m_AliasType = (u8)mtAliasTypeTransportAddress;
		}		

		memcpy(&tMtAlias, &tPUAdpAlias, sizeof(TMtAlias));
	}


	//call by ip addr directly without GK (include PU as add)
	if (!g_cMtAdpApp.m_bGkAddrSet || 0 == g_cMtAdpApp.m_tGKAddr.ip)
	{
		if( (u8)mtAliasTypeTransportAddress == tMtAlias.m_AliasType || bIsPuCall || bNPlusIPCallWithE164)
		{
			m_tMtAlias = tMtAlias;

			m_dwMtIpAddr = htonl( tMtAlias.m_tTransportAddr.GetIpAddr() );
			m_wMtPort    = tMtAlias.m_tTransportAddr.GetPort(); // in host order
			if( 0 == m_wMtPort ) 
			{
				m_wMtPort = MCU_Q931_PORT;
			}
			if( 0 != m_dwMtIpAddr )
			{
				if( (s32)act_err == kdvCreateNewCall( HAPPCALL(GetInsID()), &m_hsCall ) )
				{
					MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "call MtId.%d by ip addr directly, Create new call failed!\n", m_byMtId );
					cServMsg.SetErrorCode((u16)ERR_MCU_RAS_INTERNALERR);
					cServMsg.SetEventId(m_wSavedEvent + 2);
					SendMsgToMcu(cServMsg);
					ClearInst();
					return;
				}
#if defined(_8KH_) || defined(_8KE_) || defined(_8KI_)
				CallOutByTransportAddr( bIsPuCall );
#else
				CallOutByTransportAddr();
#endif
				return;
			}
			else
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "call MtId.%d by ip addr directly, ip is NULL\n", m_byMtId );
				cServMsg.SetErrorCode((u16)ERR_MCU_RAS_INTERNALERR);
				cServMsg.SetEventId(m_wSavedEvent + 2);
				SendMsgToMcu(cServMsg);
				ClearInst();
				return;
			}
		}
	}
	// call by manual ras
	else if( g_cMtAdpApp.m_tH323Config.IsManualRAS() && g_cMtAdpApp.m_bGkAddrSet ) 
	{
		m_tMtAlias = tMtAlias; 
		//the latest RRQ for conference succeeded, we can proceed.
		//first issue the ARQ, and then switch to state STATE_GKCALLING.

		if( TRUE == bIsPuCall )
		{
			tMtAlias.m_AliasType = (u8)mtAliasTypeTransportAddress;
		}
		
		//手动RAS，呼叫终端时判断整体GK注册有效同时所在会议实体注册成功即可
		if( STATE_NULL != g_cMtAdpApp.m_gkRegState )
		{
			//手动RAS，呼叫终端时判断整体GK注册有效同时所在会议实体注册成功即可
			TMtAlias tSrcMcuAlias = g_cMtAdpApp.m_atMcuAlias[0];
			if( STATE_REG_OK == g_cMtAdpApp.m_gkConfRegState[m_byConfIdx] )
			{
				tSrcMcuAlias = g_cMtAdpApp.m_atMcuAlias[m_byConfIdx];
			}
			else
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "call MtId.%d failed! mcu & conf alias not registered on GK!\n", m_byMtId );                
                cServMsg.SetErrorCode((u16)ERR_MCU_RAS_GKUNREG);
                cServMsg.SetEventId(m_wSavedEvent + 2);
		        SendMsgToMcu(cServMsg);
                ClearInst();
				return;
			}

			if( NULL != m_hsCall )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "call MtId.%d by manual ras, m_hsCall=%0x before Create new call!\n", m_byMtId, m_hsCall );
                cServMsg.SetErrorCode((u16)ERR_MCU_RAS_INTERNALERR);
                cServMsg.SetEventId(m_wSavedEvent + 2);
                SendMsgToMcu(cServMsg);
				ClearInst();
				return;
			}

			if( (s32)act_err == kdvCreateNewCall(HAPPCALL(GetInsID()), &m_hsCall) )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "call MtId.%d by manual ras, Create new call failed!\n", m_byMtId );
                cServMsg.SetErrorCode((u16)ERR_MCU_RAS_INTERNALERR);
                cServMsg.SetEventId(m_wSavedEvent + 2);
                SendMsgToMcu(cServMsg);
				ClearInst();
				return;
			}

			if( (u8)mtAliasTypeTransportAddress == tSrcMcuAlias.m_AliasType && 
				0 == tSrcMcuAlias.m_tTransportAddr.GetPort() )
			{	
				tSrcMcuAlias.m_tTransportAddr.SetPort( MCU_Q931_PORT );
				m_tMtAlias = tMtAlias;
			}

			//let previous state be STATE_CALLING, and current
			//state be STATE_GKCALLING
			//之所以要保证previous state为：STATE_CALLING，是因为在GK鉴权同意后转到
			//CallProceed函数中，会对之前的状态进行判断过滤
			CHANGE_STATE(STATE_CALLING);
			if( ProcSendARQ(tSrcMcuAlias, tMtAlias, m_wCallRate) )
			{
				CHANGE_STATE(STATE_GKCALLING);	
			}
			else
			{
				MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "call MtId.%d failed! ARQ sending failed!\n", m_byMtId );
                cServMsg.SetErrorCode((u16)ERR_MCU_RAS_INTERNALERR);
                cServMsg.SetEventId(m_wSavedEvent + 2);
                SendMsgToMcu(cServMsg);
				//如果为IDLE强制状态为CALLING，以便ClearInst进行对称清空，如对kdvCreateNewCall的drop清空
				if( STATE_IDLE == CurState() )
				{
					CHANGE_STATE(STATE_CALLING);
				}
				ClearInst();
				return;
			}
		}
		else
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "call MtId.%d failed! GK not registered. Call aborted...\n", m_byMtId );
            cServMsg.SetErrorCode((u16)ERR_MCU_RAS_GKUNREG);
            cServMsg.SetEventId(m_wSavedEvent + 2);
            SendMsgToMcu(cServMsg);
			ClearInst();
			return;
		}
	}
	//call by auto-alias...a straightforward procedure
	else if( !g_cMtAdpApp.m_tH323Config.IsManualRAS() && g_cMtAdpApp.m_bGkAddrSet ) 
	{
		m_tMtAlias = tMtAlias;
		if( (s32)act_err == kdvCreateNewCall( HAPPCALL(GetInsID()), &m_hsCall) )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "call MtId.%d by auto ras, Create new call failed!\n", m_byMtId );
            cServMsg.SetErrorCode((u16)ERR_MCU_RAS_INTERNALERR);
            cServMsg.SetEventId(m_wSavedEvent + 2);
            SendMsgToMcu(cServMsg);
			ClearInst();
			return;
		}	

		TCALLPARAM tCall;
		tCall.m_emConfGoalType = cmCreate;
		tCall.m_emEndpointType = emEndpointTypeMCU;
#ifdef _UTF8
		//[4/8/2013 liaokang] 编码方式 兼容老版本,DisplayInfo仍采用GBK编码
        /*tCall.SetDisplayInfo(g_cMtAdpApp.m_achMcuAlias);*/
        if( emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() )
        {
            s8 achGBK[LEN_DISPLAY_MAX] = {0};
            utf8_to_gb2312(g_cMtAdpApp.m_achMcuAlias, achGBK, LEN_DISPLAY_MAX-1);
            tCall.SetDisplayInfo(achGBK);
        }
        else if(  emenCoding_GBK == g_cMtAdpApp.GetMcuEncoding() )
        {
            tCall.SetDisplayInfo(g_cMtAdpApp.m_achMcuAlias);
        }        
#else
		tCall.SetDisplayInfo(g_cMtAdpApp.m_achMcuAlias);
#endif
			
		AliasType aliasType = type_aliastype;
		switch(tMtAlias.m_AliasType) 
		{
			case (u8)mtAliasTypeE164:	aliasType = type_e164;		break;
			case (u8)mtAliasTypeH323ID:	aliasType = type_h323ID;	break;			
			default:	                                        break;
		}
		
		tCall.SetCalledAddr(aliasType, tMtAlias.m_achAlias);
		tCall.SetCallingAddr(g_cMtAdpApp.m_dwMtAdpIpAddr, g_cMtAdpApp.m_wQ931Port ); //不填入主叫IP地址.协议栈会默认填入Eth0的IP地址[12/6/2012 chendaiwei]
		m_cConfId.GetConfId((u8*)tCall.m_abyConfId, sizeof(tCall.m_abyConfId));

		if( (s32)act_err == kdvSendCallCtrlMsg( m_hsCall, (u16)h_call_make, (void*)&tCall, sizeof(tCall) ) )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "call MtId.%d Making call out failed!\n", m_byMtId );
            cServMsg.SetErrorCode((u16)ERR_MCU_RAS_INTERNALERR);
            cServMsg.SetEventId(m_wSavedEvent + 2);
            SendMsgToMcu(cServMsg);

			//如果为IDLE强制状态为CALLING，以便ClearInst进行对称清空，如对kdvCreateNewCall的drop清空
			if( STATE_IDLE == CurState() )
			{
				CHANGE_STATE(STATE_CALLING);
			}
			ClearInst();
			return;
		}
		else
		{
			CHANGE_STATE(STATE_CALLING);
			SetTimer( WAIT_RESPONSE_TIMEOUT, CALLING_TIMEOUT * 1000 );
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "call MtId.%d OK! Calling to %s by alias...\n", m_byMtId, tCall.GetCalledAddr().GetAliasName() );
		}	
	}	
	else
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "call MtId.%d failed! GK address not set. Call aborted...\n", m_byMtId );
        cServMsg.SetErrorCode((u16)ERR_MCU_RAS_GKUNREG);
        cServMsg.SetEventId(m_wSavedEvent + 2);
        SendMsgToMcu(cServMsg);
		ClearInst();
	}

	return;
}


/*=============================================================================
  函 数 名： CallProceed
  功    能： 网守鉴权同意后，继续（根据ip地址）呼叫终端或继续处理到来呼叫
  算法实现： 
  全局变量： 
  参    数： cmTransportAddress *ta
             s32 approvedBandwidth
             s32 nIrrFrequency, 发送IRR的时间间隔(second)
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::CallProceed( cmTransportAddress *ptTransportAddr, s32 nApprovedBandwidth, s32 nIrrFrequency )
{
	if( STATE_GKCALLING != CurState() )
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[CallProceed] MtId.%d failed: CurState is not STATE_GKCALLING\n", m_byMtId );
		return;
	}

	m_nIrrFrequency = nIrrFrequency;
		
	if( (u8)CALL_OUTGOING == m_byDirect )
	{		
		if(!ptTransportAddr)
		{
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[CallProceed] Call Out MtId.%d failed: ras is rejected by GK\n", m_byMtId );

			CServMsg cServMsg;
			cServMsg.SetErrorCode( (u16)ERR_MCU_RAS_GK_REJECT );
			cServMsg.SetEventId( MT_MCU_CALLMTFAILURE_NOTIFY );
			cServMsg.SetMsgBody( NULL, 0 );
			SendMsgToMcu( cServMsg );

			m_bIsRecvARJFromGK = TRUE;
			ClearInst();
		}
		else if( nApprovedBandwidth < 2 * m_wCallRate)
		{
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[CallProceed] Call Out MtId.%d failed: requested BW %d(kbps) not allowed by GK(max %d kbps)\n", 
				        m_byMtId, m_wCallRate, nApprovedBandwidth );
			
			CServMsg cServMsg;
			cServMsg.SetErrorCode( (u16)ERR_MCU_RAS_INSUFFICIENT_RESOURCE );
			cServMsg.SetEventId( MT_MCU_CALLMTFAILURE_NOTIFY );
			cServMsg.SetMsgBody( NULL, 0 );
			SendMsgToMcu( cServMsg );
			
			ClearInst();
		}
		else
		{
			m_dwMtIpAddr = ptTransportAddr->ip;
			m_wMtPort    = ptTransportAddr->port;
			if( 0 == m_wMtPort )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[CallProceed] MtId.%d: GK return ACF but port is zero!\n", m_byMtId );
				m_wMtPort = MCU_Q931_PORT;
			}
            else
            {
                MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[CallProceed] MtId.%d: GK return ACF and port is %d\n", m_byMtId, m_wMtPort );
            }
			
			if( STATE_CALLING == m_dwLastState )
			{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
				CallOutByTransportAddr(TRUE);
#else
				CallOutByTransportAddr();
#endif
			}
			else
			{
				//MtAdpWarning( "[CallProceed] CallOutByTransportAddr MtId.%d canceled: LastState is not STATE_CALLING\n", m_byMtId );
				//CHANGE_STATE(m_dwLastState);

				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[CallProceed] CallOutByTransportAddr MtId.%d canceled: LastState is not STATE_CALLING\n", m_byMtId );
				ClearInst();
			}			
		}
	}
	else
	{
		if(!ptTransportAddr)
		{
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[CallProceed] Call InComing MtId.%d failed: ras is rejected by GK\n", m_byMtId );

			m_bIsRecvARJFromGK = TRUE;
			ClearInst();
		}
		else
		{
			ProcIncomingCall();
		}
	}

	return;
}


/*=============================================================================
  函 数 名： CallOutByTransportAddr
  功    能： 根据IP地址呼叫终端 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::CallOutByTransportAddr( BOOL32 bIsIgnoreVcIpCheck /*= FALSE*/ )
{
    CServMsg cServMsg;
    if( 0 == m_dwMtIpAddr )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[CallOutByTransportAddr] MtId.%d ip is NULL.\n", m_byMtId );
        cServMsg.SetErrorCode((u16)ERR_MCU_RAS_INTERNALERR);
        cServMsg.SetEventId(m_wSavedEvent + 2);
        SendMsgToMcu(cServMsg);
        
        //如果为IDLE强制状态为CALLING，以便ClearInst进行对称清空，如对kdvCreateNewCall的drop清空
        if( STATE_IDLE == CurState() )
        {
            CHANGE_STATE(STATE_CALLING);
        }
        ClearInst();
        return;
    }
    
    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[CallOutByTransportAddr] MtIp= %u.%u.%u.%u, port= %d, vcIp= %u.%u.%u.%u, encrypt=%d bIsIgnoreVcIpCheck=%d.\n",
        QUADADDR(m_dwMtIpAddr), m_wMtPort, QUADADDR(g_cMtAdpApp.m_dwVcIpAddr), m_byEncrypt,bIsIgnoreVcIpCheck );
    //zjj20130305 呼叫sip终端又不注册gk时，8kx系列的mcu的sip网关和mcu的ip一致，所以就无法呼叫此时放过该判断
	if( !bIsIgnoreVcIpCheck )
	{
		if( ( OspIsLocalHost(m_dwMtIpAddr) || m_dwMtIpAddr == g_cMtAdpApp.m_dwVcIpAddr) && m_wMtPort == MCU_Q931_PORT )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[CallOutByTransportAddr] MtId.%d Cannot call localhost.\n", m_byMtId );
			BuildAndSendMsgToMcu(m_wSavedEvent + 2);
        
			//如果为IDLE强制状态为CALLING，以便ClearInst进行对称清空，如对kdvCreateNewCall的drop清空
			if( STATE_IDLE == CurState() )
			{
				CHANGE_STATE(STATE_CALLING);
			}
			ClearInst();
			return;
		}
	}
    
    TCALLPARAM tCall;
    tCall.m_emConfGoalType = cmCreate;	
    // 代理呼叫端口可配置
    tCall.SetCalledAddr( m_dwMtIpAddr, m_wMtPort/*MCU_Q931_PORT*/ );
	//默认端口应该填0.协议栈检测端口为0进行动态绑定12/10/2012 chendaiwei]
	tCall.SetCallingAddr(g_cMtAdpApp.m_dwMtAdpIpAddr); //不填入主叫IP地址.协议栈会默认填入Eth0的IP地址[12/6/2012 chendaiwei]
    
    TAliasAddr tAliasAddr;
    TALIASADDR tAddr;
    if( !m_tMtAlias.IsNull() &&
        // PU实际按IP呼叫，但同时附加了323别名信息
        !m_tMtAlias.IsAliasNull())
    {
        
        if( m_tMtAlias.m_AliasType == (u8)mtAliasTypeE164 )
        {
            tAddr.SetAliasAddr( type_e164, m_tMtAlias.m_achAlias);
            tAliasAddr.SetIPCallAddr(&tAddr);
            tCall.SetCalledAliasAddr(tAliasAddr, 0);
        }
        // PU需要IP+323ID
        else/* if( m_tMtAlias.m_AliasType == mtAliasTypeH323ID )*/
        {
            tAddr.SetAliasAddr( type_h323ID, m_tMtAlias.m_achAlias);
            tAliasAddr.SetIPCallAddr(&tAddr);
            tCall.SetCalledAliasAddr(tAliasAddr, 0);
        }
    }
    
    // guzh [4/4/2007] 设置呼叫别名，增加行政级别    
    s8 szDispAlias[MAX_ALIAS_LEN] = {0};
#ifdef _UTF8
    //[4/8/2013 liaokang] 编码方式 兼容老版本,DisplayInfo仍采用GBK编码   
    if( emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() )
    {        
        utf8_to_gb2312(g_cMtAdpApp.m_achMcuAlias,szDispAlias, sizeof(szDispAlias)-1);
    }
    else if( emenCoding_GBK == g_cMtAdpApp.GetMcuEncoding() )
    {
        strncpy(szDispAlias, g_cMtAdpApp.m_achMcuAlias, sizeof(szDispAlias)-1);
    }   
#else
    strncpy(szDispAlias, g_cMtAdpApp.m_achMcuAlias, sizeof(szDispAlias)-1);
#endif
    sprintf(szDispAlias, "%s", GetLevelStr(m_byAdminLevel/*g_cMtAdpApp.m_byCascadeLevel*/) );
    tCall.SetDisplayInfo( szDispAlias );
    
    if( m_tConfAlias.m_AliasType == (u8)mtAliasTypeTransportAddress)
    {
        TNETADDR tNetAddr;
        tNetAddr.SetNetAddress(m_tConfAlias.m_tTransportAddr.GetNetSeqIpAddr(),
            m_tConfAlias.m_tTransportAddr.GetPort());
        tAliasAddr.SetIPCallAddr(&tNetAddr);
        tCall.SetCallingAliasaddr( tAliasAddr, 0 );
    }
    else if( m_tConfAlias.m_AliasType == (u8)mtAliasTypeE164 )
    {
        tAddr.SetAliasAddr( type_e164, m_tConfAlias.m_achAlias);
        tAliasAddr.SetIPCallAddr(&tAddr);
        tCall.SetCallingAliasaddr( tAliasAddr, 0 );
    }
    else if( m_tConfAlias.m_AliasType == (u8)mtAliasTypeH323ID )
    {
        s8 achDisplay[MAXLEN_ALIAS+2] = {0};
        switch (g_cMtAdpApp.m_byCasAliasType)
        {
        case 1: // Mcu Alias Only
            strncpy(achDisplay, g_cMtAdpApp.m_achMcuAlias, MAXLEN_ALIAS);
            break;
        case 2: // Conf Alias Only
            strncpy(achDisplay, m_tConfAlias.m_achAlias, MAXLEN_ALIAS);
            break;
        default:// Mcu + Conf 
            strncpy(achDisplay, g_cMtAdpApp.m_achMcuAlias, MAXLEN_ALIAS);
            strcat(achDisplay, " ");
            s32 nStrLen = strlen(achDisplay);
            if ( MAXLEN_ALIAS - nStrLen >0)
            {
                strncpy((achDisplay+nStrLen), m_tConfAlias.m_achAlias, (MAXLEN_ALIAS - nStrLen)); 
            }
            //            strncpy((achDisplay+nStrLen), m_tConfAlias.m_achAlias, (MAXLEN_ALIAS - nStrLen));  
            break;
        }
        
		//pengguofeng 2013-7-12 超过协议栈的大小则截断
		if ( strlen(achDisplay) > LEN_ALIAS )
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[CallOutByTransportAddr]strlen(achDisplay) is .%d over LEN_ALIAS.%d, so check it\n",
				strlen(achDisplay), LEN_ALIAS);
			achDisplay[LEN_ALIAS] = 0;
			CorrectUtf8Str(achDisplay, strlen(achDisplay));
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[CallOutByTransportAddr]strlen now is %d\n", strlen(achDisplay));
		}
        tAddr.SetAliasAddr( type_h323ID, achDisplay);
        tAliasAddr.SetIPCallAddr(&tAddr);
        tCall.SetCallingAliasaddr( tAliasAddr, 0 );
        
        // guzh [4/4/2007] 只设置DisplayInfo，不设置Alias
#ifdef _UTF8
        //[4/8/2013 liaokang] 编码方式 兼容老版本,DisplayInfo仍采用GBK编码
        /*tCall.SetDisplayInfo(achDisplay);*/        
        if( emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() )
        {
            memset(szDispAlias,0,sizeof(szDispAlias));
            utf8_to_gb2312(achDisplay,szDispAlias, sizeof(szDispAlias)-1);
            strncpy(achDisplay, szDispAlias, sizeof(achDisplay)-1 );
        }        
#endif  
        s8* pszSrc = GetLevelStr(m_byAdminLevel);
        s32 nStrLen = strlen(achDisplay);
        s32 nRemain = (s32)sizeof(achDisplay) - nStrLen;
        s32 nSrcLen = strlen(pszSrc) + 1;
        astrncpy( (achDisplay + nStrLen), pszSrc, nRemain, nSrcLen/*(sizeof(achDisplay) - nStrLen), (strlen(pszSrc) + 1)*/ );  
        tCall.SetDisplayInfo(achDisplay);
        
    }
    //KEDA呼叫华为MCU级联时，需填充E164信息，华为MCU将对该信息进行验证
    if( STATE_REG_OK == g_cMtAdpApp.m_gkConfRegState[m_byConfIdx] )
    {
        TMtAlias tSrcMcuAlias = g_cMtAdpApp.m_atMcuAlias[m_byConfIdx];
        tAddr.SetAliasAddr( type_e164, tSrcMcuAlias.m_achAlias);
        tAliasAddr.SetIPCallAddr(&tAddr);
        tCall.SetCallingAliasaddr( tAliasAddr, 1 );
    }
	else
	{
		tAddr.SetAliasAddr( type_e164, m_tConfE164Alias.m_achAlias );
		tAliasAddr.SetIPCallAddr(&tAddr);
		tCall.SetCallingAliasaddr( tAliasAddr, 1 );
	}
    
    tCall.SetCallRate( m_wCallRate );
    tCall.m_emEndpointType = emEndpointTypeMCU;
    m_cConfId.GetConfId( (u8*)tCall.m_abyConfId, sizeof(tCall.m_abyConfId) );
    
    //  xsl [12/29/2004] 增加是否加密字段
    tCall.SetIsEncrypt( (BOOL32)m_byEncrypt );
    // end modify by xsl
    
    // fxh 增加非标信息
    u16 wForceCallInfo = htons(m_wForceCallInfo) ;
    tCall.SetNonStandData((u8*)(&wForceCallInfo), sizeof(wForceCallInfo));
    
    if( (s32)act_err == kdvSendCallCtrlMsg( m_hsCall, (u16)h_call_make, (void*)&tCall, sizeof(tCall) ) )
    {
        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[CallOutByTransportAddr] MtId.%d Making call out failed!\n", m_byMtId );
        cServMsg.SetErrorCode((u16)ERR_MCU_RAS_INTERNALERR);
        cServMsg.SetEventId(m_wSavedEvent + 2);
        SendMsgToMcu(cServMsg);
        
        //如果为IDLE强制状态为CALLING，以便ClearInst进行对称清空，如对kdvCreateNewCall的drop清空
        if( STATE_IDLE == CurState() )
        {
            CHANGE_STATE(STATE_CALLING);
        }
        ClearInst();
        return;
    }
    else
    {
        CHANGE_STATE(STATE_CALLING);
        SetTimer( WAIT_RESPONSE_TIMEOUT, CALLING_TIMEOUT * 1000);
        
        u32 dwTmpIp = tCall.GetCalledIp();		
        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[CallOutByTransportAddr] MtId.%d Calling to %u.%u.%u.%u:%u...\n", 
            m_byMtId, QUADADDR(dwTmpIp), tCall.GetCalledPort() );
    }
    
    return;
} 


/*=============================================================================
函 数 名： OnSendApplySpeakerReq
功    能： 发送 下级MCU的终端申请成为上级MCU的发言人
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
5/13/2010   4.6			张宝卿                  创建
=============================================================================*/
void CMtAdpInst::OnSendApplySpeakerReq(CServMsg &cServMsg, HCHAN hsChan)
{
    CServMsg cMsg;
    cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
    TNonStandardReq tReq;
    tReq.m_aszUserName[0] = 0;
    tReq.m_aszUserPass[0] = 0;
    tReq.m_nReqID = m_dwMMcuReqId++;
    
    //modify msg body
//    BOOL32 bLocal = FALSE;
	TMtadpHeadMsg tMtadpHeadMsg;
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
    TMt tMt = *(TMt *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
	if( !tMt.IsMcuIdLocal() )
	{
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[tHeadMsg.m_tMsgSrc.m_byCasLevel] = tMt.GetMtId();
		++tHeadMsg.m_tMsgSrc.m_byCasLevel;
		tMt.SetMtId( (u8)tMt.GetMcuId() );
		tMt.SetMcuId( LOCAL_MCUID );
	}
    
    tMtadpHeadMsg.m_tMsgSrc.m_dwPID = GetPartIdFromMt( tMt, &tMtadpHeadMsg.m_tMsgSrc.m_bLocal );
	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = htonl( tMtadpHeadMsg.m_tMsgSrc.m_dwPID );

	HeadMsgToMtadpHeadMsg( tHeadMsg,tMtadpHeadMsg );
    
    cMsg.SetMsgBody( (u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg) );
    cMsg.CatMsgBody( (u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg)), 
							cServMsg.GetMsgBodyLen() - sizeof(TMt) - sizeof(TMsgHeadMsg) 
							);
    
    tReq.m_nMsgLen = cMsg.GetServMsgLen();
    if((u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf))
    {
        StaticLog( "[MTADAP][EXP]OnSendApplySpeakerReq message is too large(len:%d)\n", tReq.m_nMsgLen);
        return;
    }
    
    memcpy(tReq.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());
    kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq));
    
	return;
}


/*=============================================================================
    函 数 名： OnSendApplySpeakerAck
    功    能： 发送 下级MCU的终端申请成为上级MCU的发言人 同意应答
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    5/13/2010   4.6			张宝卿                  创建
=============================================================================*/
void CMtAdpInst::OnSendApplySpeakerAck(CServMsg &cServMsg, HCHAN hsChan)
{
    CServMsg cMsg;
    cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
    
    TNonStandardRsp tRsp;
    tRsp.m_nReqID      = 0;
    tRsp.m_emReturnVal = emReturnValue_Ok;
    
    //modify msg body
    
    TMtadpHeadMsg tMtadpHeadMsg;
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
    TMt tMt = *(TMt *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );

    HeadMsgToMtadpHeadMsg( tHeadMsg,tMtadpHeadMsg );

	tMtadpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tMt, &tMtadpHeadMsg.m_tMsgDst.m_bLocal );
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = htonl( tMtadpHeadMsg.m_tMsgDst.m_dwPID );

    cMsg.SetMsgBody( (u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg) );
    cMsg.CatMsgBody( (u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg)), 
			cServMsg.GetMsgBodyLen() - sizeof(TMt) - sizeof(TMsgHeadMsg)
			);
    
    tRsp.m_nMsgLen = cMsg.GetServMsgLen();
    if( (u32)tRsp.m_nMsgLen > sizeof(tRsp.m_abyMsgBuf) )
    {
        StaticLog( "[MTADAP][EXP]OnSendApplySpeakerAck message is too large(len:%d)\n", tRsp.m_nMsgLen );
        return;
    }
    
    memcpy(tRsp.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());
    
    kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_RSP, &tRsp, sizeof(tRsp) );
    
	return;
}


/*=============================================================================
    函 数 名： OnSendApplySpeakerNack
    功    能： 发送 下级MCU的终端申请成为上级MCU的发言人 拒绝应答
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    5/13/2010   4.6			张宝卿                  创建
=============================================================================*/
void CMtAdpInst::OnSendApplySpeakerNack(CServMsg &cServMsg, HCHAN hsChan)
{
    CServMsg cMsg;
    cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
    
    TNonStandardRsp tRsp;
    tRsp.m_nReqID      = 0;
    tRsp.m_emReturnVal = emReturnValue_Error;
    
    //modify msg body
    TMtadpHeadMsg tMtadpHeadMsg;
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
    TMt tMt = *(TMt *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );

    HeadMsgToMtadpHeadMsg( tHeadMsg,tMtadpHeadMsg );

	tMtadpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tMt, &tMtadpHeadMsg.m_tMsgDst.m_bLocal );
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = htonl( tMtadpHeadMsg.m_tMsgDst.m_dwPID );

    cMsg.SetMsgBody( (u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg) );
    cMsg.CatMsgBody( (u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg)), 
			cServMsg.GetMsgBodyLen() - sizeof(TMt) - sizeof(TMsgHeadMsg)
			);


    tRsp.m_nMsgLen = cMsg.GetServMsgLen();
    if( (u32)tRsp.m_nMsgLen > sizeof(tRsp.m_abyMsgBuf) )
    {
        StaticLog( "[MTADAP][EXP]OnSendApplySpeakerNack message is too large(len:%d)\n", tRsp.m_nMsgLen );
        return;
    }
    
    memcpy(tRsp.m_abyMsgBuf, cMsg.GetServMsg(), cMsg.GetServMsgLen());
    
    kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_RSP, &tRsp, sizeof(tRsp) );
    
	return;
}

/*=============================================================================
    函 数 名： OnSendApplySpeakerNotif
    功    能： 发送 下级MCU的终端申请成为上级MCU的发言人 状态通知
    算法实现： 
    全局变量： 
    参    数： CServMsg& cServMsg
               HCHAN hsChan
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    5/13/2010   4.6			张宝卿                  创建
=============================================================================*/
void CMtAdpInst::OnSendApplySpeakerNotif(CServMsg &cServMsg, HCHAN hsChan)
{
    CServMsg cMsg;
    cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
    
    TNonStandardMsg tMsg;
    tMsg.m_nMsgLen = cMsg.GetServMsgLen();

    if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
    {
        StaticLog( "[MTADAP][EXP]OnSendAddMixerMemberCmd message is too large(len:%d)\n", tMsg.m_nMsgLen );
        return;
    }
    memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg));
    
    return;
}


/*=============================================================================
  函 数 名： FlowControlProceed
  功    能： 
  算法实现： 
  全局变量： 
  参    数： s32 nRate
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::FlowControlProceed( s32 nRate )
{
	if( nRate >= m_wAppliedCallRate )
	{
		u16 wCallRate = nRate;
		s32 nCallChanNum = 0;
		
		//find the first video channel
		for( nCallChanNum = 0; nCallChanNum < MAXNUM_CALL_CHANNEL; nCallChanNum ++ )
		{
			if( (u8)CHAN_INCOMING == m_atChannel[nCallChanNum].byDirect && 
				MODE_VIDEO    == m_atChannel[nCallChanNum].tLogicChan.GetMediaType() ) //&& 
				//!m_atChannel[nCallChanNum].tLogicChan.IsSupportH239() )
			{
				break;
			}
		}

		if( nCallChanNum < MAXNUM_CALL_CHANNEL )
		{
			kdvSendChanCtrlMsg(m_atChannel[nCallChanNum].hsChan, (u16)h_chan_flowControl, &wCallRate, sizeof(wCallRate));
			m_atChannel[nCallChanNum].wCallRate = wCallRate;

			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[FlowControlProceed] byMtId.%d MediaType.%d wCallRate.%d \n", m_byMtId, m_atChannel[nCallChanNum].tLogicChan.GetMediaType(), wCallRate );
			m_wCallRate = wCallRate;
		}
		else
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Channel to control flow not found!\n");
	}	
	else 
	{
		MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "FlowControl failed: requested %d kbps"
			" not allowed by GK (max: %d kbps)\n", m_wAppliedCallRate, nRate);
	}

	return;
}


/*=============================================================================
  函 数 名： ProcMcuCreateConfRsp
  功    能： 对创建会议的应答
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang              创建
  2006/01/03	4.0			张宝卿				  兼容3.6终端
=============================================================================*/
void CMtAdpInst::ProcMcuCreateConfRsp( CMessage * const  pcMsg )
{		
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	if( STATE_IDLE == CurState() )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcMcuCreateConfRsp] CurState is IDLE.So Notify DisConnect to Mcu.Mt(%d)\n",cServMsg.GetDstMtId() );
		cServMsg.SetMcuId((u8)g_cMtAdpApp.m_wMcuNetId);
		cServMsg.SetSrcDriId(g_cMtAdpApp.m_byDriId);
		cServMsg.SetSrcMtId(cServMsg.GetDstMtId());	
		cServMsg.SetErrorCode( (u16)ERR_MCU_MTREJECT );				
		cServMsg.SetEventId( MT_MCU_MTDISCONNECTED_NOTIF );	
		u8 byCauseVc = MTLEFT_REASON_REJECTED;
		cServMsg.SetMsgBody( &byCauseVc, sizeof(byCauseVc) );
		cServMsg.SetTimer(20);

		if (SendMsgToMcuInst(cServMsg))
		{
			MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "mtadp -- > vc : %u: %s (bodyLen=%u)\n", cServMsg.GetEventId(), 
				OspEventDesc(cServMsg.GetEventId()), cServMsg.GetMsgBodyLen());
		}
		else
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Send %u(%s) to Mcu failed!\n", cServMsg.GetEventId(),
				OspEventDesc(cServMsg.GetEventId()));
		}		
	}
	
	switch ( CurState() ) 
	{
	case STATE_CALLING:

		KillTimer( WAIT_RESPONSE_TIMEOUT );

		if( pcMsg->event == MCU_MT_CREATECONF_ACK )
		{
			m_byConfIdx = cServMsg.GetConfIdx();
			m_cConfId   = cServMsg.GetConfId();
			m_byMtId    = cServMsg.GetDstMtId(); //save the Mt ID assigned by MCU
			g_cMtAdpApp.m_Msg2InstMap[m_byConfIdx][m_byMtId] = (u8)GetInsID();

			u8 byEncrypt = 0;
			TMtAlias *ptConfAlias = NULL;

			byEncrypt = *(cServMsg.GetMsgBody());
			m_tLocalCapSupport = *(TCapSupport *)( cServMsg.GetMsgBody() + sizeof(u8) );

			m_tLocalCapEx = *(TCapSupportEx *)(cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TCapSupport) + sizeof(TMtAlias));
			ptConfAlias = (TMtAlias *)( cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TCapSupport) );
			
			if( cServMsg.GetMsgBodyLen() > sizeof(u8)+sizeof(TCapSupport)+sizeof(TCapSupportEx)+sizeof(TMtAlias))
			{
				u8 *pbyMsgBody = cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TCapSupport) + sizeof(TMtAlias) + sizeof(TCapSupportEx);
				// 保存扩展能力集勾选 [12/8/2011 chendaiwei]
				memcpy(m_atMainStreamCapEX,pbyMsgBody,sizeof(m_atMainStreamCapEX));
				pbyMsgBody += sizeof(m_atMainStreamCapEX);
				
				memcpy(m_atDoubleStreamCapEX,pbyMsgBody,sizeof(m_atDoubleStreamCapEX));
				pbyMsgBody += sizeof(m_atMainStreamCapEX);
				
				//记录单双声道，目前用于AACLC,AACLD[3/16/2012 chendaiwei]
				//m_byAudioTrackNum = *pbyMsgBody;
				memcpy(m_atLocalAudioTypeDesc,pbyMsgBody,sizeof(m_atLocalAudioTypeDesc));
			}
			else
			{
				MAPrint(LOG_LVL_ERROR,MID_MCULIB_MTADP,"[ProcMcuCreateConfRsp]msg body len error! no cap ex!\n");
			}

			TNODEINFO tNode;
			tNode.SetEndpointType(emEndpointTypeMCU);
			BOOL32 bIsEncrpt = ( byEncrypt != 0 ) ? TRUE:FALSE;
			tNode.SetIsEncrypt(bIsEncrpt);
			
			//对常量进行过滤, 保护字符串边界
			if ( (u8)emMtVer36 != m_byMtVer )
			{
                s8 achDisplay[MAXLEN_ALIAS+2] = {0};
                switch (g_cMtAdpApp.m_byCasAliasType)
                {
                case 1: // Mcu Alias Only
                    strncpy(achDisplay, g_cMtAdpApp.m_achMcuAlias, MAXLEN_ALIAS);
                    break;
                case 2: // Conf Alias Only
                    strncpy(achDisplay, ptConfAlias->m_achAlias, MAXLEN_ALIAS);
                    break;
                default:// Mcu + Conf 
                    strncpy(achDisplay, g_cMtAdpApp.m_achMcuAlias, MAXLEN_ALIAS);
                    strcat(achDisplay, " ");
                    s32 nStrLen = strlen(achDisplay);
                    if ( MAXLEN_ALIAS - nStrLen > 0)
                    {
                        strncpy((achDisplay+nStrLen), ptConfAlias->m_achAlias, (MAXLEN_ALIAS - nStrLen));  
                    }
//                    strncpy((achDisplay+nStrLen), ptConfAlias->m_achAlias, (MAXLEN_ALIAS - nStrLen));  
                    break;
                }
                
				tNode.SetName(achDisplay, strlen(achDisplay));				
			}
			else
			{
                s8 achDisplay[MAXLEN_ALIAS_VER36 +2] = {0};
                switch (g_cMtAdpApp.m_byCasAliasType)
                {
                case 1: // Mcu Alias Only
                    strncpy(achDisplay, g_cMtAdpApp.m_achMcuAlias, MAXLEN_ALIAS_VER36);
                    break;
                case 2: // Conf Alias Only
                    strncpy(achDisplay, ptConfAlias->m_achAlias, MAXLEN_ALIAS_VER36);
                    break;
                default:// Mcu + Conf 
                    strncpy(achDisplay, g_cMtAdpApp.m_achMcuAlias, MAXLEN_ALIAS_VER36);
                    strcat(achDisplay, " ");
                    s32 nStrLen = strlen(achDisplay);
                    if ( MAXLEN_ALIAS_VER36 - nStrLen > 0 )
                    {
                        strncpy((achDisplay+nStrLen), ptConfAlias->m_achAlias, (MAXLEN_ALIAS_VER36 - nStrLen)); 
                    }
//                    strncpy((achDisplay+nStrLen), ptConfAlias->m_achAlias, (MAXLEN_ALIAS_VER36 - nStrLen));  
                    break;
                }

				tNode.SetName(achDisplay, strlen(achDisplay));
			}
#ifdef _UTF8
			// 被叫地址填未转换的display name [pengguofeng 5/21/2013]
			TALIASADDR t323AliasAddr;
			memset(&t323AliasAddr, 0, sizeof(t323AliasAddr));
			// 截断处理 [pengguofeng 7/12/2013]
			if ( strlen(tNode.m_chName) > LEN_ALIAS )
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuCreateConfRsp]tNode.m_chName len > LEN_ALIAS.%d,so check it\n", LEN_ALIAS);
				tNode.m_chName[LEN_ALIAS] = 0;
				CorrectUtf8Str(tNode.m_chName, strlen(tNode.m_chName));
			}
			t323AliasAddr.SetAliasAddr(type_h323ID /*type_email*/, tNode.GetName());
			TAliasAddr tAliasAddr;
			memset(&tAliasAddr, 0, sizeof(tAliasAddr));
			tAliasAddr.SetIPCallAddr(&t323AliasAddr);
			tNode.SetCalledAliasAddr(tAliasAddr, 0);

			// Q931 Name要转成GBK [pengguofeng 5/20/2013]
			if ( g_cMtAdpApp.GetMcuEncoding() == emenCoding_Utf8 )
			{
				s8 achName[LEN_DISPLAY_MAX+1] = {0};
				utf8_to_gb2312(tNode.GetName(), achName, sizeof(achName)-1);
				tNode.SetName(achName, strlen(achName)+1);
			}
#endif
            tNode.SetVenderId((s32)vendor_KEDA);
		
			if( kdvSendCallCtrlMsg( m_hsCall, (u16)h_call_answer, &tNode, sizeof(tNode), FALSE) == (s32)act_err )
			{
				MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "answering call %p failed\n", m_hsCall );
				ClearInst();
			}
		}
		else
		{
            BOOL32 bConfFull = FALSE;
			BOOL32 bCallError = FALSE;
			if( (u16)ERR_MCU_NULLCID == cServMsg.GetErrorCode() )
			{
				TFACILITYINFO tFacilityInfo;
				memset( (void*)&tFacilityInfo, 0, sizeof(tFacilityInfo) );
				CMtAdpUtils::TConfList2FacilityInfo( cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen(), tFacilityInfo );
				kdvSendCallCtrlMsg( m_hsCall, (u16)h_call_facility, &tFacilityInfo, sizeof(tFacilityInfo), FALSE );
			}
            // guzh [5/9/2007] 支持错误原因上报
            else if ( (u16)ERR_MCU_CONFNUM_EXCEED == cServMsg.GetErrorCode() )
            {
                bConfFull = TRUE;
            }
			else if( (u16)ERR_MCU_CALLMCUERROR_CONFISHOLDING == cServMsg.GetErrorCode() ||
				(u16)ERR_MCU_CASADEBYOTHERHIGHLEVELMCU == cServMsg.GetErrorCode() 
				)
			{
				bCallError = TRUE;
			}
    // [pengjie 2010/6/3] 逻辑调整
// 			else if( bCallError )
// 			{
// 				if( ERR_MCU_CALLMCUERROR_CONFISHOLDING == cServMsg.GetErrorCode() )
// 				{
// 					ClearInst( 0, emDisconnectReasonRemoteConfHolding, 1 );
// 				}
// 				else if( ERR_MCU_CASADEBYOTHERHIGHLEVELMCU == cServMsg.GetErrorCode() )
// 				{
// 					ClearInst( 0, emDisconnectReasonRemoteHasCascaded, 1 );
// 				}
// 			}
            if (bConfFull)
            {
                ClearInst( 0, (u8)emDisconnectReasonBusy, 1 );
            }
			else if( bCallError )
			{
				if( (u16)ERR_MCU_CALLMCUERROR_CONFISHOLDING == cServMsg.GetErrorCode() )
				{
					ClearInst( 0, (u8)emDisconnectReasonRemoteConfHolding, 1 );
				}
				else if( (u16)ERR_MCU_CASADEBYOTHERHIGHLEVELMCU == cServMsg.GetErrorCode() )
				{
					ClearInst( 0, (u8)emDisconnectReasonRemoteHasCascaded, 1 );
				}
			}
            else
            {
                ClearInst();
            }
			ClearInst();
		}
		break;

	default:
		UNEXPECTED_MESSAGE( pcMsg->event );
	}

	return;
}


/*=============================================================================
  函 数 名： ProcMcuMtOpenLogicChannelReq
  功    能： MCU向终端打开逻辑信道请求
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0         TanGuang              创建
  2004/12/30                xsl                   加密模式下，设置载荷类型和密码等信息
  2005/11/11    4.0         wancl                 代码优化
=============================================================================*/
void CMtAdpInst::ProcMcuMtOpenLogicChannelReq( CMessage * const  pcMsg )
{		
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	if(!BODY_LEN_GE(cServMsg, sizeof(TLogicalChannel))) 
		return;

	if( !m_bHasJoinedConf )
	{
		return;
	}	
	if( STATE_NORMAL != CurState() )
	{
		UNEXPECTED_MESSAGE( pcMsg->event );
		return;
	}
		
	TLogicalChannel tLogicChan = *(TLogicalChannel*)(cServMsg.GetMsgBody());

	u8 byRes = 0;         //分辨率
	if( cServMsg.GetMsgBodyLen() >= sizeof(TLogicalChannel) + 1 )
	{
		byRes = *( cServMsg.GetMsgBody() + sizeof( TLogicalChannel ) ); 
	}

	//双流支持，由于MCU未做对端能力集判别，故在此进行过滤，对于对端能力集不支持的，直接拒绝
	if( MODE_SECVIDEO == tLogicChan.GetMediaType() && 
		( ( m_tCommonCap.GetDStreamMediaType()  != tLogicChan.GetChannelType() &&
            m_tCommonCap.GetSecComDStreamCapSet().GetMediaType() != tLogicChan.GetChannelType() )
			/*|| m_tCommonCap.IsDStreamSupportH239() != tLogicChan.IsSupportH239()*/ ))
	{
		BuildAndSendMsgToMcu( MT_MCU_OPENLOGICCHNNL_NACK, (u8*)&tLogicChan, sizeof(TLogicalChannel));
		return;
	}

	TCHANPARAM  tChanParam;
	s32         nNewSlot;
	u8          byPayload;
	HCHAN       hsNewChan;
	s8          achChanName[32];

	m_wSavedEvent = MCU_MT_OPENLOGICCHNNL_REQ;

	// [pengjie 2010/10/18] 支持对端报多能力级，重新尝试打开逻辑通道
	nNewSlot = FindChannelByMediaType( (u8)CHAN_OUTGOING, tLogicChan.GetMediaType() );
	if( nNewSlot >= 0 && tLogicChan.GetChannelType() == m_atChannel[nNewSlot].tLogicChan.GetChannelType() )
	{
		if( m_atChannel[nNewSlot].tLogicChan.IsOpened() )
		{
			BuildAndSendMsgToMcu( MT_MCU_OPENLOGICCHNNL_NACK, (u8*)&tLogicChan, sizeof(TLogicalChannel));
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "The Channel.%d is opend don't need open again !\n", tLogicChan.GetMediaType() );
			return;
		}
	}

	nNewSlot = GetFreeChannel();		
	if(nNewSlot < 0)
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Cannot get free channel for new outgoing channel!\n");
		return;
	}

	if( (s32)act_err == kdvCreateNewChan(m_hsCall, NULL, &hsNewChan) )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Failed creating new channel!\n");
		FreeChannel(nNewSlot);
		return;
	}
	m_atChannel[nNewSlot].hsChan = hsNewChan;

	//Udp 通道设置Rtcp反馈地址
	tChanParam.m_tRtcp.SetNetAddress( htonl(tLogicChan.m_tSndMediaCtrlChannel.GetIpAddr()), tLogicChan.m_tSndMediaCtrlChannel.GetPort() );	
	//Tcp 通道需设置ip、port
	tChanParam.m_tRtp.SetNetAddress( htonl(tLogicChan.m_tRcvMediaChannel.GetIpAddr()), tLogicChan.m_tRcvMediaChannel.GetPort() );
	
	byPayload = tLogicChan.GetChannelType();
	if( MEDIA_TYPE_H224 == byPayload )
	{
		tChanParam.SetDynamicPayloadParam(MEDIA_TYPE_H224);
	}
    //xl [08/05/20] netmeeting limit (maybe in fact, byPayload here can never be G7221C)
	//zjj new polycom mt can recv g7221c,but function IsChanG7221CSupport return false,so delete the function
    if ( MEDIA_TYPE_G7221C == byPayload &&
		(/*!IsChanG7221CSupport() ||*/ MT_MANU_NETMEETING == m_byVendorId) )    
    {
        FreeChannel(nNewSlot);
        BuildAndSendMsgToMcu( MT_MCU_OPENLOGICCHNNL_NACK, (u8*)&tLogicChan, sizeof(TLogicalChannel));
        return;
    }

	// 设置FECType, zgc, 2007-09-30
	switch( tLogicChan.GetMediaType() )
	{
	case MODE_VIDEO:
		tChanParam.SetFECType( m_tLocalCapEx.GetVideoFECType() );
		break;
	case MODE_SECVIDEO:
		tChanParam.SetFECType( m_tLocalCapEx.GetDVideoFECType() );
		break;
	case MODE_AUDIO:
		tChanParam.SetFECType( m_tLocalCapEx.GetAudioFECType() );
		break;
	default:
		tChanParam.SetFECType( FECTYPE_NONE );
		break;
	}
	
	tChanParam.m_wPayloadType = CMtAdpUtils::PayloadTypeIn2Out(byPayload);		

	CMtAdpUtils::PszGetChannelName(byPayload, achChanName, sizeof(achChanName));

	//设置视频通道传输码率、分辨率、帧率等信息（若为polycom，与其能力集比较，取较小的码率）	
	if( MODE_VIDEO == tLogicChan.GetMediaType() || MODE_SECVIDEO == tLogicChan.GetMediaType() )
	{
		TVideoCap tVideoCap;
        TH264VideoCap tH264VideoCap;

        u16 wBitrate = tLogicChan.GetFlowControl();
                		
        if ( MT_MANU_POLYCOM == m_byVendorId )
        {                    
            u16 wVidBitrate = CMtAdpUtils::GetVideoBitrate(m_ptRemoteCapSet, byPayload);
            if(wVidBitrate != 0 && wBitrate > wVidBitrate)
			{
                wBitrate = wVidBitrate;
			}
        }

		// xliang [9/19/2008] set h264 NAl mode
		// xliang [10/21/2008] 进一步细分，大于4cif的切包，否则不切
		if (MEDIA_TYPE_H264 == byPayload)
		{
			if(( MT_MANU_KDCMCU == m_byVendorId || MT_MANU_KDC == m_byVendorId)
				&& g_cMtAdpApp.m_bSupportSlice	//当前配置是否支持切包
				)
			{
				s8 chResCmp = CMtAdpUtils::ResCmp( VIDEO_FORMAT_4CIF,tLogicChan.GetVideoFormat() );
				if( 0 == u8(chResCmp +1) ) // xliang [11/14/2008] 判chResCmp为-1，考虑某些硬件不支持s8，会将-1看成255.
				{
					tChanParam.SetH264NALMode(emNALNoneInterleaved);
					MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Set H264NalMode: None Interleaved!\n");
				}
				else
				{
					tChanParam.SetH264NALMode(emNALSignal);
					MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Set H264NalMode: emNALSignal!\n");
				}
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMtOpenLogicChannelReq]H264NAL mode is %u\n",tChanParam.GetH264NALMode());
			}
			else 
			{
				tChanParam.SetH264NALMode(emNALSignal);
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Set H264NalMode: emNALSignal!\n");
			}
		}
		
        // xsl [8/11/2006] 打开通道时根据会议帧率填帧率信息
		// [pengjie 2010/6/2] 打开对端通道的帧率信息现在统一在mcu侧填入，这里不用再填
		u8 byMPI = tLogicChan.GetChanVidFPS();
		if( byMPI == 0 )
		{
			StaticLog( "[ProcMcuMtOpenLogicChannelReq] ChanType: %d GetChanVidFPS <= 0,Chan open might fail !\n", tLogicChan.GetChannelType() );
		}
		else
		{
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMtOpenLogicChannelReq] open ChanType: %d, GetChanVidFPS: %d !\n", \
				tLogicChan.GetChannelType(), tLogicChan.GetChanVidFPS() );
		}
		// End
/*
        u8 byMPI;   
        if (MODE_VIDEO == tLogicChan.GetMediaType())
        {
            if (byPayload == m_tLocalCapSupport.GetSecondSimCapSet().GetVideoMediaType())
            {
                if (MEDIA_TYPE_H264 == byPayload)
                {
                    byMPI = m_tLocalCapSupport.GetSecVidUsrDefFPS();
                }
                else
                {
                    byMPI = m_tLocalCapSupport.GetSecVidFrmRate();
                }
            }
            else
            {
                if (MEDIA_TYPE_H264 == byPayload)
                {
                    byMPI = m_tLocalCapSupport.GetMainVidUsrDefFPS();
                }
                else
                {
                    byMPI = m_tLocalCapSupport.GetMainVidFrmRate();
                }
            }
        }
        else
        {
            if (byPayload == m_tLocalCapSupport.GetDStreamMediaType())
            {
                if (MEDIA_TYPE_H264 == byPayload)
                {
                    byMPI = m_tLocalCapSupport.GetDStreamUsrDefFPS();
                }
                else
                {
                    byMPI = m_tLocalCapSupport.GetDStreamFrmRate();
                }
            }
            //双 双流能力集 取帧率
            else if (byPayload == m_tLocalCapEx.GetSecDSType())
            {
                byMPI = m_tLocalCapEx.GetSecDSFrmRate();
            }
            else
            {
                MtAdpException("[ProcMcuMtOpenLogicChannelReq] unexpected ds PL.%d, check it\n", byPayload);
            }
        }
*/
        if (MEDIA_TYPE_H264 != byPayload)
        {
            // guzh [2008/02/20]
            byMPI = CMtAdpUtils::FrameRateIn2Out(byMPI);

            tVideoCap.SetResolution( (emResolution)CMtAdpUtils::ResIn2Out(byRes), byMPI );
            tVideoCap.SetBitRate( wBitrate );
            tChanParam.SetPayloadParam( tChanParam.m_wPayloadType, tVideoCap );            
        }
        else
        {
			// [pengjie 2010/6/2] 打开对端通道的帧率信息现在统一
            // zbq [05/08/2009] 会议如果已经处理了有效帧率，尊重会议处理(临时只处理 双流多帧率情况)
//             if (MEDIA_TYPE_H264 == tLogicChan.GetChannelType() &&
//                 MODE_SECVIDEO == tLogicChan.GetMediaType() &&
//                 0 != tLogicChan.GetChanVidFPS() &&
//                 tLogicChan.GetChanVidFPS() <= byMPI)
//             {
//                 byMPI = tLogicChan.GetChanVidFPS();
//             }
            tH264VideoCap.SetResolution( CMtAdpUtils::ResIn2Out(byRes), byMPI );
            tH264VideoCap.SetBitRate( wBitrate );
			//设置HP/BP属性[12/9/2011 chendaiwei]
			TH264VideoCap::emH264ProfileMask emProfile =  (tLogicChan.GetProfileAttrb() == emHpAttrb ? TH264VideoCap::emProfileHigh : TH264VideoCap::emProfileBaseline );
			tH264VideoCap.SetSupportProfile((u8)emProfile);
            tChanParam.SetPayloadParam( tChanParam.m_wPayloadType, tH264VideoCap );
        }

        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMtOpenLogicChannelReq] open chan bitrate<%u>, CapBitrate<%u>, vendor<%d>, Res<%d>, MPI<%d>, HP<%d>\n", 
                  tLogicChan.GetFlowControl(), wBitrate, m_byVendorId, byRes, byMPI,(u8)tLogicChan.GetProfileAttrb());
	}
	else if ( MODE_AUDIO == tLogicChan.GetMediaType() )
	{
		//zbq[06/17/2008] AAC支持
		if ( MEDIA_TYPE_AACLC == tLogicChan.GetChannelType() || MEDIA_TYPE_AACLD == tLogicChan.GetChannelType())
		{
			TAACCap tAACCap;
			tAACCap.SetCapDirection(cmCapReceiveAndTransmit);
			//音频声道数从逻辑通道中取
			tAACCap.SetChnl(CMtAdpUtils::GetAACChnnNum(tLogicChan.GetAudioTrackNum()));
			tAACCap.SetSampleFreq(TAACCap::emFs32000);
			tChanParam.SetPayloadParam(MEDIA_TYPE_AACLC == tLogicChan.GetChannelType()?(u16)a_mpegaaclc:(u16)a_mpegaacld, tAACCap);
		}
	}

	//加密模式下，设置动态载荷类型和密码等信息
	TMediaEncrypt tEncryptMedia = tLogicChan.GetMediaEncrypt();
	u8 byEncryptType = CMtAdpUtils::EncryptTypeIn2Out(tEncryptMedia.GetEncryptMode());

	//设置加密算法
	tChanParam.SetEncryptType((emEncryptType)byEncryptType);

	//设置加密密码信息
	if((u8)emEncryptTypeNone != byEncryptType)
	{
		u8 abyKey[MAXLEN_KEY] = {0};
		s32 nKeyLen = 0;
		tEncryptMedia.GetEncryptKey(abyKey, &nKeyLen);
		
		TEncryptSync tEncryptSync;
		tEncryptSync.SetH235Key  (abyKey, nKeyLen);			
		tEncryptSync.SetSynchFlag(tLogicChan.GetActivePayload());

		tChanParam.SetEncryptSync(tEncryptSync);
	}
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Encrypt is %d, DPT is %d in mcu2mt openlogicchnnlreq\n",
		      tEncryptMedia.GetEncryptMode(), tLogicChan.GetActivePayload());
	
	//设置动态载荷
	//不加密模式下,h263plus、H239(H263＋/H264)和h264要设置动态载荷; 加密模式下都需要动态载荷
    // guzh [9/26/2007] g7221.c
    // guzh [2008/02/20] aac
	if( (MEDIA_TYPE_H263PLUS == byPayload || 
		 MEDIA_TYPE_H264     == byPayload || 
         MEDIA_TYPE_G7221C   == byPayload || 
         MEDIA_TYPE_AACLC    == byPayload || 
         MEDIA_TYPE_AACLD    == byPayload ||
		 MEDIA_TYPE_G719     == byPayload ||
		 (u8)emEncryptTypeNone   != byEncryptType) && 
		 0 != tLogicChan.GetActivePayload() )
	{
		if(!tChanParam.SetDynamicPayloadParam(tLogicChan.GetActivePayload()))
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Invalid DPT param from mcu in MCU_MT_OPENLOGICCHNNL_REQ!\n");
			FreeChannel(nNewSlot);
			return;
		}		
	}

	//设置H239的通道扩展信息
	if( tLogicChan.IsSupportH239() )
	{
		tChanParam.m_bIsH239DStream = TRUE;

		//sony 在支持 H263－H239，设置 role==2，以适应sony动态双流
		if( MT_MANU_SONY == m_byVendorId && MEDIA_TYPE_H263 == byPayload )
		{
			tChanParam.m_bIsH239DStream = 2;	
		}
	}

	if( (s32)act_err == kdvSendChanCtrlMsg(hsNewChan, (u16)h_chan_open, (void*)&tChanParam, sizeof(tChanParam)) )
	{ 			
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Failed opening channel %p %s!\n", hsNewChan, achChanName);
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "TCHANPARAM: activepayload:%d,encrypt:%d,payload:%d,ip:%d,port:%d,rtcp:%d:%d,%d,%d,%d,%d,RSVP:%d,Zero:%d\n", 
			            tChanParam.GetDynamicPayloadParam(), 
			            tChanParam.GetEncryptType(), 
			            tChanParam.GetPayloadType(), 
			            tChanParam.GetRtp().GetIPAddr(),
			            tChanParam.GetRtp().GetIPPort(),
			            tChanParam.GetRtcp().GetIPAddr(),
			            tChanParam.GetRtcp().GetIPPort(),
			            tChanParam.GetSrcTerLabel().GetMcuNo(), tChanParam.GetSrcTerLabel().GetTerNo(),
			            tChanParam.GetDstTerLabel().GetMcuNo(), tChanParam.GetDstTerLabel().GetTerNo(),
			            tChanParam.IsSupportRSVP(),
			            tChanParam.IsFlowToZero()
			            );
		
		FreeChannel(nNewSlot);
		return;
	}
	else
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "To open logical channel %p: %s...\n", hsNewChan, achChanName);			
	}

	m_atChannel[nNewSlot].tLogicChan = tLogicChan;	//save it
	m_atChannel[nNewSlot].byState    = (u8)CHAN_OPENING;
	m_atChannel[nNewSlot].byDirect   = (u8)CHAN_OUTGOING;
	m_wSavedEvent = pcMsg->event;
	
	return;
}


/*=============================================================================
  函 数 名： ProcMcuMtCloseLogicChannelCmd
  功    能： MCU关闭逻辑信道
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0         TanGuang              创建
  2005/11/11    4.0         wancl                 代码优化
=============================================================================*/
void CMtAdpInst::ProcMcuMtCloseLogicChannelCmd( CMessage * const  pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	if(!BODY_LEN_GE(cServMsg, sizeof(TLogicalChannel))) 
		return;

	if( !m_bHasJoinedConf )
	{
		return;
	}	
	if( STATE_NORMAL != CurState() )
	{
		UNEXPECTED_MESSAGE( pcMsg->event );
		return;
	}
	
	TLogicalChannel tLogicChan = *(TLogicalChannel *)cServMsg.GetMsgBody();
	BOOL32 bOut  = ISTRUE( *( cServMsg.GetMsgBody() + sizeof(TLogicalChannel) ) );
	s32 nChanIdx = FindChannelByPayloadType( (u8)(bOut?CHAN_OUTGOING:CHAN_INCOMING), 
		                                     tLogicChan.GetChannelType(), tLogicChan.GetMediaType() );

	//failed to find the stack handle of channel to close
	if( nChanIdx < 0 )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Cannot find the chan to close, MediaType=%u \n", tLogicChan.GetChannelType());
		return;
	}
	
	if( (s32)act_err == kdvSendChanCtrlMsg( m_atChannel[nChanIdx].hsChan, (u16)h_chan_close, NULL, 0) )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Channel close failed!\n");
		return;
	}

	// xliang [12/19/2008] FreeChannel modify
	if (MODE_SECVIDEO == m_atChannel[nChanIdx].tLogicChan.GetMediaType() &&
		!bOut)
	{
		//add time out --free
	}
	else
	{
		FreeChannel( nChanIdx );
		// xliang [12/19/2008] print
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMtCloseLogicChannelCmd] Free chanIdx.%u,bOut is %u\n",nChanIdx,bOut );
		
	}		

	return;
}

/*=============================================================================
  函 数 名： ProcMcuMtOpenLogicChannelRsp
  功    能： MCU应答终端打开逻辑信道请求
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11 /11   1.0         TanGuang              创建
  2004/12/30                xsl                   加密模式下算法设置
  2005/11/11    4.0         wancl                 代码优化
=============================================================================*/
void CMtAdpInst::ProcMcuMtOpenLogicChannelRsp( CMessage * const  pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	if(!BODY_LEN_GE(cServMsg, sizeof(TLogicalChannel))) 
		return;
	
	if( !m_bHasJoinedConf )
	{
		return;
	}	
	if( STATE_NORMAL != CurState() )
	{
		UNEXPECTED_MESSAGE( pcMsg->event );
		return;
	}

	TLogicalChannel tLogicChan = *(TLogicalChannel *)cServMsg.GetMsgBody();
	TCHANPARAM tChanParam;
	s32 nChanIdx;
	
	KillTimer( WAIT_RESPONSE_TIMEOUT );
	m_wSavedEvent = 0;

	nChanIdx = FindChannelByPayloadType((u8)CHAN_INCOMING, tLogicChan.GetChannelType(), tLogicChan.GetMediaType());
	//failed to find the stack handle of channel to answer
	if( nChanIdx < 0 )	
	{
		s8  achChanName[32];
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Cannot find the chan to answer, MediaType=%u MediaName=%s\n", 
			           tLogicChan.m_byMediaType, 
			           CMtAdpUtils::PszGetChannelName(tLogicChan.GetChannelType(), achChanName, sizeof(achChanName)));	
		return;
	}
    
    // guzh [9/3/2007] G.722.1.C 过滤
    if ( MEDIA_TYPE_G7221C == tLogicChan.GetChannelType() &&
         MCU_MT_OPENLOGICCHNNL_ACK == pcMsg->event )
    {
        if (!IsChanG7221CSupport())
        {
            kdvSendChanCtrlMsg(m_atChannel[nChanIdx].hsChan, (u16)h_chan_openReject, NULL, 0);
            return;
        }
    }

	if( MCU_MT_OPENLOGICCHNNL_ACK == pcMsg->event )
	{
		// [12/13/2010 xliang] active payload modify for sony (for reference)
		// present way: mt--105-->mcu; 
		//				mt<--106--mt
		// following:   mt--105-->mcu
		//				mt<--105--mcu
		/*
		u8 byActPt = m_atChannel[nChanIdx].tLogicChan.GetActivePayload();
		m_atChannel[nChanIdx].tLogicChan = tLogicChan;
		if(  MT_MANU_SONY == m_byVendorId )
		{
			MtAdpInfo("for sony, modify its ActivePT to %d from tLogicChan's %d\n", byActPt, tLogicChan.GetActivePayload());
			m_atChannel[nChanIdx].tLogicChan.SetActivePayload(byActPt);
			tLogicChan.SetActivePayload(byActPt);
		}
		*/

		m_atChannel[nChanIdx].tLogicChan = tLogicChan;
		tChanParam.m_tRtp.SetNetAddress( htonl(tLogicChan.m_tRcvMediaChannel.GetIpAddr()), tLogicChan.m_tRcvMediaChannel.GetPort() );
		
		if( MEDIA_TYPE_MMCU == tLogicChan.GetChannelType() )
		{
			tChanParam.m_tRtp.SetNetAddress( htonl(tLogicChan.m_tRcvMediaChannel.GetIpAddr()), CASCADE_PORT );
		}
		
		tChanParam.m_tRtcp.SetNetAddress( htonl(tLogicChan.m_tRcvMediaCtrlChannel.GetIpAddr()), tLogicChan.m_tRcvMediaCtrlChannel.GetPort() );
		tChanParam.m_wPayloadType = CMtAdpUtils::PayloadTypeIn2Out( tLogicChan.GetChannelType() );

		//加密模式下，设置动态载荷类型和密码等信息
		TMediaEncrypt tEncryptMedia = tLogicChan.GetMediaEncrypt();
		u8 byEncryptType = CMtAdpUtils::EncryptTypeIn2Out(tEncryptMedia.GetEncryptMode());					
		
		//设置加密算法
		tChanParam.SetEncryptType((emEncryptType)byEncryptType);
		
		//设置加密密码信息
		if((u8)emEncryptTypeNone != byEncryptType)
		{
			u8 abyKey[MAXLEN_KEY] = {0};
			s32 nKeyLen = 0;
			tEncryptMedia.GetEncryptKey(abyKey, &nKeyLen);
			
			TEncryptSync tEncryptSync;
			tEncryptSync.SetH235Key(abyKey, nKeyLen);			
			tEncryptSync.SetSynchFlag(tLogicChan.GetActivePayload());
			
			tChanParam.SetEncryptSync(tEncryptSync);
		}

		//设置动态载荷
		//不加密模式下,h263plus、H239(H263＋/H264)和h264要设置动态载荷; 加密模式下都需要动态载荷
        // guzh [9/26/2007] g7221.C
		if( (MEDIA_TYPE_H263PLUS == tLogicChan.GetChannelType() ||
			 MEDIA_TYPE_H264     == tLogicChan.GetChannelType() ||
             MEDIA_TYPE_G7221C   == tLogicChan.GetChannelType() ||
			 MEDIA_TYPE_AACLC	 == tLogicChan.GetChannelType() ||
			 MEDIA_TYPE_AACLD    == tLogicChan.GetChannelType() ||
			 (u8)emEncryptTypeNone   != byEncryptType) && 
			 0 != tLogicChan.GetActivePayload() )
		{
			if(!tChanParam.SetDynamicPayloadParam(tLogicChan.GetActivePayload()))
			{
				MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "Invalid DPT param from mcu in MCU_MT_OPENLOGICCHNNL_ACK!\n");
			}
		}		

		if( (s32)act_err == kdvSendChanCtrlMsg(m_atChannel[nChanIdx].hsChan, (u16)h_chan_openAck, 
			                              (void*)&tChanParam, sizeof(tChanParam)) )
		{
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Channel answer failed!\n");
			return;
		}
		else
		{
			//通道状态统一由协议栈状态通知时进行变更 2005-9-27
			//m_atChannel[iChanIdx].byState = CHAN_CONNECTED;
			//MtAdpInfo("[Ntf OPENA_CK] MtId=%d iChanIdx=%d\n", m_byMtId, iChanIdx, tLogicChan.GetChannelType());
		}			
	}
	else
	{
		kdvSendChanCtrlMsg(m_atChannel[nChanIdx].hsChan, (u16)h_chan_openReject, NULL, 0);
	}

	return;
}


/*=============================================================================
  函 数 名： ProcMcuMtEnterPasswordReq
  功    能： MCU向终端要求输入会议密码
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcMcuMtEnterPasswordReq(CMessage * const  pcMsg )
{	
	if( CurState() != STATE_NORMAL ) 
	{
		UNEXPECTED_MESSAGE( pcMsg->event );
	}
	CONF_CTRL( (u16)h_ctrl_enterH243Password, NULL, 0 );

	SetTimer( WAIT_PASSWORD_TIMEOUT, WAITING_PWD_TIMEOUT * 1000 );

	return;
}


/*=============================================================================
  函 数 名： ProcMcuMtFeccCmd
  功    能： 摄像头远端遥控
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcMcuMtFeccCmd( CMessage * const  pcMsg )
{	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	if( !BODY_LEN_GE(cServMsg, sizeof(TMt) + 1)) 
		return;
	
	cServMsg.SetEventId(pcMsg->event);

	if( !m_bHasJoinedConf )
		return;
	
	TFeccStruct tFeccCmd;
	memset( &tFeccCmd, 0, sizeof(tFeccCmd) );
	CMtAdpUtils::CameraCommandIn2Out( cServMsg, tFeccCmd );
	
	for( s32 nCallChanNum = 0; nCallChanNum < MAXNUM_CALL_CHANNEL; nCallChanNum ++ )
	{
		if( MEDIA_TYPE_H224 == m_atChannel[nCallChanNum].tLogicChan.GetChannelType() && 
			(u8)CHAN_OUTGOING   == m_atChannel[nCallChanNum].byDirect ) 
		{				
			if( kdvSendFeccMsg(m_atChannel[nCallChanNum].hsChan, tFeccCmd) != (s32)annexQOk )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "send fecc error!\n");
			}
			break;
		}
	}

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMtH239TokenGeneralInd
    功    能： 处理由MCU向终端发送的 H239令牌命令
    算法实现： 
    全局变量： 
    参    数： CMessage * const  pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/6/9    3.6			万春雷                  创建
=============================================================================*/
void CMtAdpInst::ProcMcuMtH239TokenGeneralInd( CMessage * const  pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	if(!BODY_LEN_GE( cServMsg, sizeof(TH239TokenInfo)+sizeof(TMt) )) 
		return;

	TMt *ptMt = (TMt *)cServMsg.GetMsgBody();
	PTH239TokenInfo ptH239TokenInfo = (PTH239TokenInfo)(cServMsg.GetMsgBody()+sizeof(TMt));
	
	TH239INFO tH239Info;
	TTERLABEL tMtLabel;
//	s32 nChanIdx  = 0;
	tH239Info.Clear();
	//tMtLabel.SetTerminalLabel( (u8)ptMt->GetMcuId(), (u8)ptMt->GetMtId() );
	CMtAdpUtils::ConverTMt2TTERLABEL( *ptMt, tMtLabel );
	tH239Info.SetTerLabel(tMtLabel);

	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMtH239TokenGeneralInd] Outgoing H239Message(event:%s, isack:%d, chanid:0x%x, breaking:%d)\n",
			    OspEventDesc(cServMsg.GetEventId()), ptH239TokenInfo->IsResponseAck(), 
				ptH239TokenInfo->GetChannelId(), ptH239TokenInfo->GetSymmetryBreaking() );

	switch( pcMsg->event ) 
	{
	case MCU_MT_GETH239TOKEN_ACK:
		{
			tH239Info.SetH239InfoType( emH239PresentationTokenResponse );
			tH239Info.SetChannelId( ptH239TokenInfo->GetChannelId() );
			tH239Info.SetResponseAck( TRUE );
			//tH239Info.host2net( TRUE );	
			kdvSendConfCtrlMsg( m_hsCall, (u16)h_ctrl_H239Message, (void*)&tH239Info, sizeof( TH239INFO ) );
		}
		break;

	case MCU_MT_GETH239TOKEN_NACK:
		{
			tH239Info.SetH239InfoType( emH239PresentationTokenResponse );
			tH239Info.SetChannelId( ptH239TokenInfo->GetChannelId() );
			tH239Info.SetResponseAck( FALSE );
			//tH239Info.host2net( TRUE );
			kdvSendConfCtrlMsg( m_hsCall, (u16)h_ctrl_H239Message, (void*)&tH239Info, sizeof( TH239INFO ) );
		}
		break;

	case MCU_MT_RELEASEH239TOKEN_CMD:
		{
			tH239Info.SetH239InfoType( emH239PresentationTokenRequest );
			tH239Info.SetChannelId( ptH239TokenInfo->GetChannelId() );
			tH239Info.SetSymmetryBreaking( 0 );
			//tH239Info.host2net( TRUE );
			kdvSendConfCtrlMsg( m_hsCall, (u16)h_ctrl_H239Message, (void*)&tH239Info, sizeof( TH239INFO ) );
		}
		break;
		
	case MCU_MT_OWNH239TOKEN_NOTIF:
		{
			tH239Info.SetH239InfoType( emH239PresentationTokenIndicateOwner );
			tH239Info.SetChannelId( ptH239TokenInfo->GetChannelId() );
			tH239Info.SetChannelId( 8 );
			//tH239Info.host2net( TRUE );

			kdvSendConfCtrlMsg( m_hsCall, (u16)h_ctrl_H239Message, (void*)&tH239Info, sizeof( TH239INFO ) );
		}
		break;
	case MCU_POLY_GETH239TOKEN_REQ:
		{
			//FIXME: MCUID和MTID分别是这样处理，断点跟踪看看效果
			tMtLabel.SetTerminalLabel( 0, 0 );
			tH239Info.SetTerLabel(tMtLabel);
			
			tH239Info.SetH239InfoType( emH239PresentationTokenRequest );
			
			//FIXME: 这个ChannelId 应该是本实例对应的MT的chnnlId.
			//for switch 查到支持H239的那个通道
//			u8 byH239Chn = 0;
			u8 byChnnl = 0;
			for( byChnnl = 0; byChnnl < MAXNUM_CALL_CHANNEL; byChnnl ++)
			{
				if ( m_atChannel[byChnnl].tLogicChan.GetMediaType() == MODE_SECVIDEO )
				{
//					byH239Chn = byChnnl;
                    tH239Info.SetChannelId(m_atChannel[byChnnl].hsChan);
					break;
				}
			}
//			tH239Info.SetChannelId(m_atChannel[byH239Chn].hsChan);
			tH239Info.SetSymmetryBreaking(26);
			
			kdvSendConfCtrlMsg( m_hsCall, (u16)h_ctrl_H239Message, (void*)&tH239Info, sizeof(TH239INFO));
		}
		break;
	case MCU_POLY_RELEASEH239TOKEN_CMD:
		{
			//FIXME: MCUID和MTID分别是这样处理，断点跟踪看看效果
			tMtLabel.SetTerminalLabel( 0, 0 );
			tH239Info.SetTerLabel(tMtLabel);
			
			//FIXME: 这个ChannelId 应该是本实例对应的MT的chnnlId.
			//for switch 查到支持H239的那个通道
//			u8 byH239Chn = 0;
			u8 byChnnl = 0;
			for( byChnnl = 0; byChnnl < MAXNUM_CALL_CHANNEL; byChnnl ++)
			{
				if ( m_atChannel[byChnnl].tLogicChan.GetMediaType() == MODE_SECVIDEO )
				{
//					byH239Chn = byChnnl;
                    tH239Info.SetChannelId(m_atChannel[byChnnl].hsChan);
					break;
				}
			}
//			tH239Info.SetChannelId(m_atChannel[byH239Chn].hsChan);
			
			tH239Info.SetH239InfoType( emH239PresentationTokenRelease );
			tH239Info.SetChannelId( ptH239TokenInfo->GetChannelId() );
			
			kdvSendConfCtrlMsg( m_hsCall, (u16)h_ctrl_H239Message, (void*)&tH239Info, sizeof( TH239INFO ) );
			
		}
		break;
	default:
		UNEXPECTED_MESSAGE( pcMsg->event );
        break;
	}

	return;
}


/*=============================================================================
  函 数 名： ProcMcuMtGeneralCmd
  功    能： 处理由MCU向终端发送的会议命令，信道控制命令
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcMcuMtGeneralCmd( CMessage * const  pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	if( pcMsg->event == MCU_MT_DELMT_CMD )
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMtGeneralCmd] receive msg MCU_MT_DELMT_CMD. ClearInst() !!!\n");
		ClearInst();
		return;
	}

	switch ( CurState() ) 
	{
	case STATE_NORMAL:
	case STATE_CALLING:

		switch( pcMsg->event ) 
		{
		case MCU_MT_FLOWCONTROL_CMD:
			{
				if( !BODY_LEN_GE( cServMsg, sizeof(TLogicalChannel) ) ) 
					return;
				
				TLogicalChannel *ptLogicChan = (TLogicalChannel*)cServMsg.GetMsgBody();				
				u16 wCallRate  = ptLogicChan->GetFlowControl();
				
				// [12/11/2009 xliang] Not send flowcontrol cmd 0 to Aethra
				if( MT_MANU_AETHRA == m_byVendorId && wCallRate == 0)
				{
					MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "No flowcontrol 0 to MT_MANU_AETHRA!\n");
					return;
				}
				
				if( MT_MANU_SONY == m_byVendorId && wCallRate == 0)
				{
					MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "No flowcontrol 0 to MT_MANU_SONY!\n");
					return;
				}
				u8 byChanDir = (u8)CHAN_INCOMING;
// 				if(BODY_LEN_GE( cServMsg, sizeof(TLogicalChannel) ) 
// 					&& MT_MANU_OTHER == m_byVendorId
// 					)
// 				{
// 					byChanDir = *( cServMsg.GetMsgBody() + sizeof(TLogicalChannel) );
// 					MtAdpInfo("[ProcMcuMtGeneralCmd][MCU_MT_FLOWCONTROL_CMD] change dir to %u!\n", byChanDir);
// 				}

				s32 nChanIdx   = FindChannelByPayloadType( byChanDir, ptLogicChan->GetChannelType(), ptLogicChan->GetMediaType());
				

				if( nChanIdx >= 0 )
				{
					if(m_byVendorId == MT_MANU_AETHRA && ptLogicChan->GetMediaType() != MODE_AUDIO)
					{
						if( m_atChannel[nChanIdx].CheckFCTimeInterval( wCallRate ) )
						{
							MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMtFlowControlCmd-aethra] byMtId.%d MediaType.%d wCallRate.%d \n", m_byMtId, ptLogicChan->GetMediaType(), wCallRate );
							
							kdvSendChanCtrlMsg(m_atChannel[nChanIdx].hsChan, (u16)h_chan_flowControl, &wCallRate, sizeof(wCallRate));
						}
						else
						{
							MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMtFlowControlCmd-aethra] byMtId.%d MediaType.%d wCallRate.%d no change and TimeInterval<=1s, not send\n", m_byMtId, ptLogicChan->GetMediaType(), wCallRate );
						}
						return;
					}

					if( 0 == g_cMtAdpApp.m_tGKAddr.ip || 
						0 == wCallRate || 					             
						MODE_AUDIO == ptLogicChan->GetMediaType() )
					{
                        if( m_atChannel[nChanIdx].CheckFCTimeInterval( wCallRate ) )
                        {
                            kdvSendChanCtrlMsg( m_atChannel[nChanIdx].hsChan, (u16)h_chan_flowControl,	&wCallRate, sizeof(wCallRate) );
                        }						    
						MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMtGeneralCmd] byMtId.%d MediaType.%d wCallRate.%d \n", m_byMtId, ptLogicChan->GetMediaType(), wCallRate );
					}
					//call via GK, first should get permission from GK
					else if(g_cMtAdpApp.m_bGkAddrSet && g_cMtAdpApp.m_tH323Config.IsManualRAS() )
					{
						if( MT_MANU_KDCMCU == m_byVendorId && wCallRate > m_atChannel[nChanIdx].tLogicChan.GetFlowControl() && ptLogicChan->GetMediaType() == MODE_SECVIDEO)
						{
							MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP,"h_chan_flowControl adust rate.%d to %d\n",wCallRate,m_atChannel[nChanIdx].tLogicChan.GetFlowControl() );
							wCallRate = m_atChannel[nChanIdx].tLogicChan.GetFlowControl();
						}
						//jianghy add. 如果不增加信道的码率 不发brq
						if( wCallRate > m_atChannel[nChanIdx].tLogicChan.GetFlowControl() )
						{
							ProcSendBRQ(wCallRate, m_byDirect);	
							m_wAppliedCallRate = wCallRate; //keep it
						}
						else
						{
							if( m_atChannel[nChanIdx].CheckFCTimeInterval( wCallRate ) )
							{
								MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMtFlowControlCmd] byMtId.%d MediaType.%d wCallRate.%d \n", m_byMtId, ptLogicChan->GetMediaType(), wCallRate );
								
								kdvSendChanCtrlMsg(m_atChannel[nChanIdx].hsChan, (u16)h_chan_flowControl, &wCallRate, sizeof(wCallRate));
							}
							else
							{
								MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMtFlowControlCmd] byMtId.%d MediaType.%d wCallRate.%d no change and TimeInterval<=1s, not send\n", m_byMtId, ptLogicChan->GetMediaType(), wCallRate );
							}
						}
					}
				}
			}
			break;

		case MCU_MT_FLOWCONTROLINDICATION_NOTIF:
			{
				if(!BODY_LEN_GE(cServMsg, sizeof(TLogicalChannel))) return;

				TLogicalChannel *ptLogicChan = (TLogicalChannel*)cServMsg.GetMsgBody();				
				u16 wCallRate = ptLogicChan->GetFlowControl();
				s32 nChanIdx  = FindChannelByPayloadType((u8)CHAN_OUTGOING/*CHAN_INCOMING*/, ptLogicChan->GetChannelType(), ptLogicChan->GetMediaType());
				
				if(nChanIdx >= 0)
				{
					if(m_byVendorId == MT_MANU_AETHRA && ptLogicChan->GetMediaType() != MODE_AUDIO)
					{
						MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMtFlowControlInd-aethra] byMtId.%d MediaType.%d wCallRate.%d \n", m_byMtId, ptLogicChan->GetMediaType(), wCallRate );
						
						kdvSendChanCtrlMsg(m_atChannel[nChanIdx].hsChan, (u16)h_chan_flowControlIndication, &wCallRate, sizeof(wCallRate));
						
						return;
					}
					if( g_cMtAdpApp.m_tGKAddr.ip == 0 || wCallRate == 0 || 
						ptLogicChan->GetMediaType() == MODE_AUDIO)
					{
						kdvSendChanCtrlMsg(m_atChannel[nChanIdx].hsChan, 
							(u16)h_chan_flowControlIndication,	&wCallRate, sizeof(wCallRate));

						MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMtGeneralCmd] byMtId.%d MediaType.%d wCallRate.%d \n", m_byMtId, ptLogicChan->GetMediaType(), wCallRate );
					}
					//call via GK, first should get permission from GK
					else if(g_cMtAdpApp.m_bGkAddrSet && g_cMtAdpApp.m_tH323Config.IsManualRAS() )
					{
						/*
						//jianghy add. 如果不增加信道的码率 不发brq
						if( wCallRate > m_atChannel[nChanIdx].tLogicChan.GetFlowControl() )
						{
							ProcSendBRQ(wCallRate, m_byDirect);	
							m_wAppliedCallRate = wCallRate; //keep it
						}
						else
						*/
						{
							MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuMtFlowControlInd] byMtId.%d MediaType.%d wCallRate.%d \n", m_byMtId, ptLogicChan->GetMediaType(), wCallRate );

							kdvSendChanCtrlMsg(m_atChannel[nChanIdx].hsChan, (u16)h_chan_flowControlIndication, &wCallRate, sizeof(wCallRate));
						}
					}
				}
			}
			break;

		case MCU_MT_FREEZEPIC_CMD:		// vcf
			{
				s32 nVChanIdx = FindChannelByMediaType( (u8)CHAN_INCOMING, MODE_VIDEO ); 
				if( nVChanIdx >= 0 )
				{
					kdvSendChanCtrlMsg( m_atChannel[nVChanIdx].hsChan, (u16)h_chan_VideoFreezePicture, NULL, 0 );
				}
			}
			break;
		
		case MCU_MT_FASTUPDATEPIC_CMD:	// vcu
			{
				u8 byMode = *(cServMsg.GetMsgBody());
				s32 nVChanIdx = FindChannelByMediaType( (u8)CHAN_INCOMING, byMode ); 
				if(nVChanIdx >= 0)
				{
					if( m_atChannel[nVChanIdx].CheckVCUTimeInterval() )
					{
					    kdvSendChanCtrlMsg( m_atChannel[nVChanIdx].hsChan, (u16)h_chan_videofastupdatePIC, NULL, 0 );
                        MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[Proc MCU_MT_FASTUPDATEPIC_CMD] send h_chan_videofastupdatePIC to mtid :%d, ChanType :%d\n", 
                                      m_byMtId, m_atChannel[nVChanIdx].tLogicChan.GetMediaType() );
					}
					else
					{
						MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[Proc MCU_MT_FASTUPDATEPIC_CMD] byMtId.%d MediaType.%d no change and TimeInterval<=1s, not send\n", 
							        m_byMtId, m_atChannel[nVChanIdx].tLogicChan.GetMediaType() );
					}
				}
                else
                {
                    MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[Proc MCU_MT_FASTUPDATEPIC_CMD] find Mt.%d chnnl.%d for mode.%d err\n", m_byMtId, nVChanIdx, byMode);

                    for( u8 byChIdx = 0; byChIdx < MAXNUM_CALL_CHANNEL; byChIdx++)
                    {
                        if ( MODE_NONE != m_atChannel[byChIdx].tLogicChan.GetMediaType())
                        {
                            StaticLog( "\t\t ChnNum.%d: mode.%d, direct.%d\n",
                                                     byChIdx,
                                                     m_atChannel[byChIdx].tLogicChan.GetMediaType(),
                                                     m_atChannel[byChIdx].byDirect);
                        }
                    }
                }
			}
			break;
		
		case MCU_MT_CHANNELON_CMD:
			{
				s32 nAChanIdx = FindChannelByMediaType( (u8)CHAN_INCOMING, MODE_AUDIO ); 
				s32 nVChanIdx = FindChannelByMediaType( (u8)CHAN_INCOMING, MODE_VIDEO ); 
				
				if( nAChanIdx >= 0 )
				{
					kdvSendChanCtrlMsg( m_atChannel[nAChanIdx].hsChan, (u16)h_chan_on, NULL, 0 );
				}
				if( nVChanIdx >= 0 )
				{
					kdvSendChanCtrlMsg( m_atChannel[nVChanIdx].hsChan, (u16)h_chan_on, NULL, 0 );
				}
			}
			break;

		case MCU_MT_CHANNELOFF_CMD:
			{
				s32 nAChanIdx = FindChannelByMediaType( (u8)CHAN_INCOMING, MODE_AUDIO ); 
				s32 nVChanIdx = FindChannelByMediaType( (u8)CHAN_INCOMING, MODE_VIDEO ); 
				if( nAChanIdx >= 0 )
				{
					kdvSendChanCtrlMsg( m_atChannel[nAChanIdx].hsChan, (u16)h_chan_off, NULL, 0 );
				}
				if( nVChanIdx >= 0 )
				{
					kdvSendChanCtrlMsg( m_atChannel[nVChanIdx].hsChan, (u16)h_chan_off, NULL, 0 );
				}
			}
			break;

		default:
			UNEXPECTED_MESSAGE( pcMsg->event );
            break;
		}		
		break;

	default:
		UNEXPECTED_MESSAGE( pcMsg->event );
        break;
	}

	return;
}

/*=============================================================================
  函 数 名： ProcMcuMtGeneralInd
  功    能： 处理由MCU向终端发送的会议通知 
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcMcuMtGeneralInd( CMessage * const  pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );	
	TTERLABEL tMtLabel;

	switch ( CurState() ) 
	{
	case STATE_NORMAL:
		switch( pcMsg->event )
		{
		case MCU_MT_CAPBILITYSET_NOTIF:
			{
				//KillTimer(TIMER_H245_CAPBILITY_NOTIFY_OVERTIME);
				//only after h245 conn has been established can we begin 
				//TCS exchange. Otherwise, later TCS after h_conf_h245Connected
				//may not be able to reset the previous TCS' timer(for reasons
				//not known yet), whose associated handler may be called in a
				//null context and results in stack exception !!
				
				if(!m_bH245Connected)
					break;

				if( !BODY_LEN_GE( cServMsg, sizeof(TCapSupport) ) ) 
					return;
				
				TCapSupport tCapSupport = *(TCapSupport*)cServMsg.GetMsgBody();
				u8 bySupportActiveH263 = *(cServMsg.GetMsgBody() + sizeof(TCapSupport));
				u8 bySendMSDetermine   = *(cServMsg.GetMsgBody() + sizeof(TCapSupport) + sizeof(u8));
				TCapSupportEx tCapEx= *(TCapSupportEx*)(cServMsg.GetMsgBody() + sizeof(TCapSupport) + sizeof(u8) + sizeof(u8) );
				//u8 byFpsAdp = *(cServMsg.GetMsgBody() + sizeof(TCapSupport) + sizeof(u8) + sizeof(u8) + sizeof(TCapSupportEx));

				// 保存扩展能力集勾选 [12/8/2011 chendaiwei]
				memcpy(m_atMainStreamCapEX,cServMsg.GetMsgBody() + sizeof(TCapSupport) + sizeof(u8) + sizeof(u8) + sizeof(TCapSupportEx),sizeof(m_atMainStreamCapEX));
				memcpy(m_atDoubleStreamCapEX,cServMsg.GetMsgBody() + sizeof(TCapSupport) + sizeof(u8) + sizeof(u8) + sizeof(TCapSupportEx)+sizeof(m_atMainStreamCapEX),sizeof(m_atDoubleStreamCapEX));

				memcpy(m_atLocalAudioTypeDesc,cServMsg.GetMsgBody() + sizeof(TCapSupport) + sizeof(u8) + sizeof(u8) + 
					sizeof(TCapSupportEx) + sizeof(m_atMainStreamCapEX) + sizeof(m_atDoubleStreamCapEX),sizeof(m_atLocalAudioTypeDesc));
				
				//m_byAudioTrackNum = *(cServMsg.GetMsgBody() + sizeof(TCapSupport) + sizeof(u8) + sizeof(u8) + sizeof(TCapSupportEx)+sizeof(m_atMainStreamCapEX)+sizeof(m_atDoubleStreamCapEX));

				if ( g_cMtAdpApp.m_byDebugLevel >= DEBUG_INFO )
				{
					MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMtGeneralInd] CapSupportEx: \n");
					tCapEx.Print();

					// 主流扩展勾选打印 [12/12/2011 chendaiwei]
					if(m_atMainStreamCapEX[0].GetMediaType()!= MEDIA_TYPE_NULL)
					{
						MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMtGeneralInd] MainSCapEx: \n");
						for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++)
						{
							if(MEDIA_TYPE_NULL == m_atMainStreamCapEX[byIdx].GetMediaType())
							{
								break;
							}
							MAPrint(LOG_LVL_KEYSTATUS,MID_MCULIB_MTADP,"[ProcMcuMtGeneralInd] Index:%d ",byIdx);
							m_atMainStreamCapEX[byIdx].Print();
						}
					}
					
					// 双流扩展勾选打印 [12/12/2011 chendaiwei]
					if(m_atDoubleStreamCapEX[0].GetMediaType()!= MEDIA_TYPE_NULL)
					{
						MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMtGeneralInd] DoubleSreamCapEx: \n");
						for(u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++)
						{
							if(MEDIA_TYPE_NULL == m_atDoubleStreamCapEX[byIdx].GetMediaType())
							{
								break;
							}
							
							MAPrint(LOG_LVL_KEYSTATUS,MID_MCULIB_MTADP,"[ProcMcuMtGeneralInd] Index:%d ",byIdx);
							m_atDoubleStreamCapEX[byIdx].Print();
						}
					}
					//音频能力打印
					if(m_atLocalAudioTypeDesc[0].GetAudioMediaType() != MEDIA_TYPE_NULL)
					{
						MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMtGeneralInd] Audio Cap: \n");
						for(u8 byIdx = 0; byIdx < MAXNUM_CONF_AUDIOTYPE; byIdx++)
						{
							if(MEDIA_TYPE_NULL == m_atLocalAudioTypeDesc[byIdx].GetAudioMediaType())
							{
								break;
							}
							
							MAPrint(LOG_LVL_KEYSTATUS,MID_MCULIB_MTADP,"[ProcMcuMtGeneralInd] Index:%d ",byIdx);
							m_atLocalAudioTypeDesc[byIdx].Print();
							OspPrintf(TRUE,FALSE,"**************(%d,%d)**************\n",
								m_atLocalAudioTypeDesc[byIdx].GetAudioMediaType(),m_atLocalAudioTypeDesc[byIdx].GetAudioTrackNum());
						}
					}
					//MAPrint(LOG_LVL_KEYSTATUS,MID_MCULIB_MTADP,"[ProcMcuMtGeneralInd] AudioTrackNum [%d]\n",m_byAudioTrackNum);

				}
				
				m_tLocalCapSupport = tCapSupport;
				m_tLocalCapEx = tCapEx;
				//m_byAdpFps = byFpsAdp;	
				if(g_cMtAdpApp.m_tH323Config.m_bH245_capabilitiesManualOperation )
				{
					TCapSet tCapSet;
					
					TVideoStreamCap tDSVideoCapEx;
					if( tCapSupport.GetDStreamResolution() == VIDEO_FORMAT_UXGA
						&& tCapSupport.GetDStreamUsrDefFPS() > 30 )
					{
						if( MT_MANU_KDCMCU == m_byVendorId || MT_MANU_KDC == m_byVendorId)
						{
							TDStreamCap tMainDsStreamCap = tCapSupport.GetDStreamCapSet();
							memcpy(&tDSVideoCapEx,&tMainDsStreamCap,sizeof(tDSVideoCapEx));
							tDSVideoCapEx.SetResolution(VIDEO_FORMAT_HD1080);
							tDSVideoCapEx.SetUserDefFrameRate(30);
						}
						else
						{
							for( u8 byIdx = 0; byIdx < sizeof(m_atDoubleStreamCapEX)/sizeof(m_atDoubleStreamCapEX[0]);byIdx++)
							{
								if( m_atDoubleStreamCapEX[byIdx].GetResolution() == VIDEO_FORMAT_HD1080)
								{
									memcpy(&tDSVideoCapEx,&m_atDoubleStreamCapEX[byIdx],sizeof(tDSVideoCapEx));
									
									break;
								}
							}
						}
					}

					//对该接口进行修改，追加会议支持的音频能力个数参数以及能力列表参数。
					CMtAdpUtils::CapabilitiesSetIn2Out( tCapSupport, tCapEx, &tCapSet, m_byVendorId,
														m_atLocalAudioTypeDesc,MAXNUM_CONF_AUDIOTYPE,tDSVideoCapEx);

					// xliang [9/19/2008] set H264 NAL mode 
					// xliang [10/21/2008] 进一步细分，大于4cif的切包，否则不切
					if( (MT_MANU_KDCMCU == m_byVendorId || MT_MANU_KDC == m_byVendorId)
						&& g_cMtAdpApp.m_bSupportSlice	//当前配置是否支持切包
						)
					{ 
						s8 chResCmp = CMtAdpUtils::ResCmp( VIDEO_FORMAT_4CIF,tCapSupport.GetMainVideoResolution());

						if( 0 == u8(chResCmp+1) )
						{
							tCapSet.SetH264NALMode(emNALNoneInterleaved);
							MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[CapOut]Set H264NalMode: emNALNoneInterleaved!\n");
						}
						else
						{
							tCapSet.SetH264NALMode(emNALSignal);
							MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[CapOut]Set H264NalMode: emNALSignal!\n");
						}
					}

                    if ( g_cMtAdpApp.m_byDebugLevel >= DEBUG_INFO )
                    {
                        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMtGeneralInd] LocalCapInfo:\n");
                        tCapSupport.Print();
                    }

					tCapSet.SetSupportActiveH263(bySupportActiveH263);

					CMtAdpUtils::CapabilityExSetIn2Out( tCapEx, &tCapSet );
					
					if( (s32)act_err == kdvSendConfCtrlMsg(m_hsCall, (u16)h_ctrl_capNotifyExt, &tCapSet, sizeof(TCapSet)) )
					{						
						MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Send CapabilitySet failed.\n");							
					}
					else
					{
						MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "Manually CapSet send out \n");
					}
					
				}
				
				if( g_cMtAdpApp.m_tH323Config.m_bH245_masterSlaveManualOperation && 
					1 == bySendMSDetermine )
				{
					s32 nBuf[1];	
					nBuf[0] = (s32)emMsActiveMC;
					if(!IS_MT && m_byDirect == (u8)CALL_INCOMING)
					{
						nBuf[0] = (s32)emMsMT;//terminalType
					}; 
					
					if (!m_bMsdOK)
					{
						if((s32)act_err == kdvSendConfCtrlMsg(m_hsCall, (u16)h_ctrl_masterSlaveDetermine, (void*)nBuf, sizeof(nBuf)) )
						{
							MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Send msd failed.\n");	
						}
						else
						{
							MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "Manually MSD sent out \n");
						}
					}
				}
			}
			break;

		case MCU_MT_YOUARESEEING_NOTIF:
			{
				OTHERBREAK;				
				if ( (u8)emMtVer36 != m_byMtVer )
				{
					if(!BODY_LEN_GE(cServMsg, sizeof(TMt))) return;
					
					TMt tMt = *(TMt *)cServMsg.GetMsgBody();
					//if( !tMtLabel.SetTerminalLabel( (u8)tMt.GetMcuId(), (u8)tMt.GetMtId() ) )
                    if( !CMtAdpUtils::ConverTMt2TTERLABEL( tMt, tMtLabel ) )
					{
						break;
					}

					CONF_CTRL_STRICT((u16)h_ctrl_terminalYouAreSeeing, (void*)&tMtLabel, sizeof(tMtLabel));
				}
				else
				{
					if(!BODY_LEN_GE(cServMsg, sizeof(TMtVer36))) return;

					TMtVer36 tMt36 = *(TMtVer36 *)cServMsg.GetMsgBody();
					if( !tMtLabel.SetTerminalLabel( (u8)g_cMtAdpApp.m_wMcuNetId, (u8)tMt36.GetMtId() ) )
					{
						break;
					}

					CONF_CTRL_STRICT((u16)h_ctrl_terminalYouAreSeeing, (void*)&tMtLabel, sizeof(tMtLabel));
				}	
			}
			break;

		case MCU_MT_SEENBYOTHER_NOTIF:
			break;
			
		case MCU_MT_CANCELSEENBYOTHER_NOTIF:
			break;		
			
		case MCU_MT_SPECSPEAKER_NOTIF:	
			OTHERBREAK
			CONF_CTRL_STRICT( (u16)h_ctrl_seenByAll, NULL, 0 );			
			break;	

		case MCU_MT_CANCELSPEAKER_NOTIF:	//cancel speaking
			OTHERBREAK
			CONF_CTRL_STRICT( (u16)h_ctrl_cancelSeenByAll, NULL, 0 );			
			break;

		case MCU_MT_APPLYSPEAKER_NOTIF:		//terminal applys for speaking
			{
				OTHERBREAK
				if( !BODY_LEN_GE( cServMsg, sizeof(TMt) ))
					return;

				TMt tMt = *(TMt *)cServMsg.GetMsgBody();
				//tMtLabel.SetTerminalLabel( (u8)tMt.GetMcuId(), (u8)tMt.GetMtId() );
				CMtAdpUtils::ConverTMt2TTERLABEL( tMt, tMtLabel );
				CONF_CTRL_STRICT( (u16)h_ctrl_floorRequested, (void*)&tMtLabel, sizeof(tMtLabel) );
			}
			break;

		case MCU_MT_MTJOINED_NOTIF:			//new terminal joins conference
			{
				OTHERBREAK
				if(!BODY_LEN_GE(cServMsg, sizeof(TMt))) 
					return;

				TMt tMt = *(TMt *)cServMsg.GetMsgBody();
				//tMtLabel.SetTerminalLabel( (u8)tMt.GetMcuId(), (u8)tMt.GetMtId() );	
				CMtAdpUtils::ConverTMt2TTERLABEL( tMt, tMtLabel );
				if(m_bMaster)
				{
					CONF_CTRL_STRICT( (u16)h_ctrl_terminalJoinedConference, (void*)&tMtLabel, sizeof(tMtLabel));
				}
			}
			break;
			
		case MCU_MT_MTLEFT_NOTIF:			//terminal left conference
			{
				OTHERBREAK
				if(!BODY_LEN_GE(cServMsg, sizeof(TMt))) 
					return;

				TMt tMt = *(TMt *)cServMsg.GetMsgBody();
				//tMtLabel.SetTerminalLabel( (u8)tMt.GetMcuId(), (u8)tMt.GetMtId() );
				CMtAdpUtils::ConverTMt2TTERLABEL( tMt, tMtLabel );
				if(m_bMaster)
				{
					CONF_CTRL_STRICT( (u16)h_ctrl_terminalLeftConference, (void*)&tMtLabel, sizeof(tMtLabel));
				}
			}
			break;

		case MCU_MT_CANCELCHAIRMAN_NOTIF:
			OTHERBREAK
			CONF_CTRL_STRICT((u16)h_ctrl_withdrawChairToken, NULL, 0);
			break;
		default:
			break;
		}
		break;
	default:
		UNEXPECTED_MESSAGE(pcMsg->event);	
        break;
	}

	return;
}

/*=============================================================================
  函 数 名： ProcMcuMtMediaLoopOnRsp
  功    能： mcu对终端远端环回响应处理
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::ProcMcuMtMediaLoopOnRsp( CMessage * const pcMsg )
{
    s32 nChanIdx;
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TLogicalChannel *ptChan = (TLogicalChannel *)cServMsg.GetMsgBody();    

    switch( CurState() )
    {
    case STATE_NORMAL:
        nChanIdx = FindChannelByMediaType((u8)CHAN_INCOMING, ptChan->GetMediaType());
        if (nChanIdx >= 0)
        {
            if (pcMsg->event == MCU_MT_MEDIALOOPON_ACK)
            {            
                kdvSendChanCtrlMsg(m_atChannel[nChanIdx].hsChan, (u16)h_chan_MediaLoopConfirm, NULL, 0);
			    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMtMediaLoopOnRsp] send mt<%d> chanidx<%d> medialoop. confirm.\n", 
					       m_byMtId, nChanIdx);
            }            
            else
            {
                kdvSendChanCtrlMsg(m_atChannel[nChanIdx].hsChan, (u16)h_chan_MediaLoopReject, NULL, 0);
			    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMcuMtMediaLoopOnRsp] send mt<%d> chanidx<%d> medialoop. reject.\n", 
					       m_byMtId, nChanIdx);
            }
        }        
        break;

    default:
        UNEXPECTED_MESSAGE(pcMsg->event);
        break;
    }

    return;
}

/*=============================================================================
  函 数 名： ProcMcuMtGeneralRsp
  功    能： 处理由MCU向终端发送的请求响应
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcMcuMtGeneralRsp(CMessage * const pcMsg)
{
	u8 byResult;

	switch( CurState() ) 
	{
	case STATE_NORMAL:
		{
			switch( pcMsg->event ) 
			{
			case MCU_MT_GETMTALIAS_NACK:
			case MCU_MT_GETMTALIAS_ACK:			
				{
					CServMsg cServMsg( pcMsg->content, pcMsg->length );
					TTERINFO tTerInfo;					
								
					if( pcMsg->event == MCU_MT_GETMTALIAS_ACK )
					{
						if( !BODY_LEN_GE(cServMsg, sizeof(TMt) + sizeof(TMtAlias)) )
							return;

						TMt  *pTMt = (TMt*)cServMsg.GetMsgBody();

						TTERLABEL tTerLabel;
						CMtAdpUtils::ConverTMt2TTERLABEL( *pTMt, tTerLabel );

						//tTerInfo.SetLabel( (u8)pTMt->GetMcuId(), pTMt->GetMtId() );
						tTerInfo.SetLabel( &tTerLabel );

						TMtAlias *pAlias = (TMtAlias*)(cServMsg.GetMsgBody() + sizeof(TMt));	
						if( pAlias->m_achAlias[0] )
#ifdef _UTF8
						{
							// 报给终端自己的名字，需要转 [pengguofeng 5/20/2013]
							s8 achMtAlias[MAXLEN_ALIAS];
							memset(achMtAlias, 0, sizeof(achMtAlias));
							BOOL32 bTrans = TransEncoding(pAlias->m_achAlias, achMtAlias, sizeof(achMtAlias));
							if ( bTrans )
							{
								tTerInfo.SetName(achMtAlias);
							}
							else
							{
								tTerInfo.SetName(pAlias->m_achAlias);
							}
						}
#else
							tTerInfo.SetName(pAlias->m_achAlias);
#endif
						else
						{
							s8  achIpStr[20];
							u32 dwIp = htonl(pAlias->m_tTransportAddr.GetIpAddr());
							sprintf(achIpStr, "%u.%u.%u.%u", QUADADDR(dwIp) );
							tTerInfo.SetName(achIpStr);
						}
					}
					else
					{
						tTerInfo.SetLabel( 0, 0 );
						tTerInfo.SetName(" ");
					}					
					CONF_CTRL_STRICT((u16)h_ctrl_mCTerminalIDResponse,(u8*)&tTerInfo, sizeof(tTerInfo));
				}						
				break;

			case MCU_MT_APPLYCHAIRMAN_ACK:
				byResult = (u8)ctrl_granted;				
				CONF_CTRL_STRICT((u16)h_ctrl_makeMeChairResponse, &byResult, sizeof(u8));
				break;
				
			case MCU_MT_APPLYCHAIRMAN_NACK:
				byResult = (u8)ctrl_denied;
				CONF_CTRL_STRICT((u16)h_ctrl_makeMeChairResponse, &byResult, sizeof(u8));				
				break;

			case MCU_MT_SPECSPEAKER_ACK:
				byResult = (u8)ctrl_granted;						
				CONF_CTRL_STRICT((u16)h_ctrl_makeTerminalBroadcasterResponse, &byResult, sizeof(u8));
				break;
				
			case MCU_MT_SPECSPEAKER_NACK:
				byResult = (u8)ctrl_denied;
				CONF_CTRL_STRICT((u16)h_ctrl_makeTerminalBroadcasterResponse, &byResult, sizeof(u8));	
				break;
				
			case MCU_MT_CANCELCHAIRMAN_ACK:
				CONF_CTRL_STRICT((u16)h_ctrl_withdrawChairToken, NULL, 0);
				break;

			case MCU_MT_CANCELCHAIRMAN_NACK: //currently meaningless, should by notif
				break;

			case MCU_MT_DELMT_NACK:
				CONF_CTRL_STRICT((u16)h_ctrl_terminalDropReject, NULL, 0);				
				break;
				
			case MCU_MT_GETCHAIRMAN_ACK:
			case MCU_MT_GETCHAIRMAN_NACK:
				{
					CServMsg cServMsg( pcMsg->content, pcMsg->length );
					TTERINFO tTerInfo;

					if( pcMsg->event == MCU_MT_GETCHAIRMAN_ACK )
					{
						if(!BODY_LEN_GE(cServMsg, sizeof(TMt) + sizeof(TMtAlias))) return;

						TMt  *pTMt = (TMt*)cServMsg.GetMsgBody();	
						TTERLABEL tTerLabel;
						CMtAdpUtils::ConverTMt2TTERLABEL( *pTMt, tTerLabel );
						tTerInfo.SetLabel( &tTerLabel );

						//tTerInfo.SetLabel((u8)pTMt->GetMcuId(), pTMt->GetMtId());
						TMtAlias  *pAlias = (TMtAlias*)(cServMsg.GetMsgBody() + sizeof(TMt));
						if(pAlias->m_AliasType   != (u8)mtAliasTypeTransportAddress &&
						   pAlias->m_achAlias[0] != 0)
						{
							tTerInfo.SetName(pAlias->m_achAlias); 
						}
						else
						{
							s8  aliasBuf[16];
							u32 ip = htonl(pAlias->m_tTransportAddr.GetIpAddr());
							sprintf(aliasBuf, "%u.%u.%u.%u", QUADADDR(ip));
							tTerInfo.SetName(aliasBuf);
						}
					}
					else
					{
						tTerInfo.SetLabel(0, 0);
						tTerInfo.SetName("Unknown");//must set a non-null string
					}
					CONF_CTRL_STRICT((u16)h_ctrl_chairTokenOwnerResponse,(u8*)&tTerInfo, sizeof(tTerInfo));						
				}
				break;
				
			case MCU_MT_STARTSELMT_ACK:

				byResult = (u8)ctrl_granted;
				CONF_CTRL_STRICT((u16)h_ctrl_sendThisSourceResponse, &byResult, sizeof(u8));
				break;

			case MCU_MT_STARTSELMT_NACK:
				
				byResult = (u8)ctrl_denied;
				CONF_CTRL_STRICT((u16)h_ctrl_sendThisSourceResponse, &byResult, sizeof(u8));
				break;

			default:
				UNEXPECTED_MESSAGE(pcMsg->event);
                break;
			}
		}		
		break;
	default:
		UNEXPECTED_MESSAGE(pcMsg->event);
        break;
	}
	return;
}

/*=============================================================================
  函 数 名： ProcMcuMtJoinedMtInfoRsp
  功    能： MCU到MT与会终端列表
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcMcuMtJoinedMtInfoRsp( CMessage * const  pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u16 wMtNum;

	switch ( CurState() ) 
	{
	case STATE_NORMAL:
		if(pcMsg->event == MCU_MT_JOINEDMTLIST_ACK) //terminal label list response
		{
			TTERLABEL atTerLabel[MAXNUM_CONF_MT];
			wMtNum = cServMsg.GetMsgBodyLen() / sizeof(TMt);
			if( wMtNum == 0 || wMtNum > MAXNUM_CONF_MT )
			{
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "Message with bad body length %u received.\n", wMtNum );
				return;
			}
			TMt *pMt = NULL;			
			for( s32 nMtNum = 0; nMtNum < wMtNum; nMtNum ++ )
			{
				pMt = (TMt*)(cServMsg.GetMsgBody() + sizeof(TMt) * (u32)nMtNum );
				//atTerLabel[nMtNum].SetTerminalLabel( cServMsg.GetMcuId(), pMt->GetMtId() );
				CMtAdpUtils::ConverTMt2TTERLABEL( *pMt, atTerLabel[nMtNum] );
			}
			CONF_CTRL_STRICT((u16)h_ctrl_terminalListResponse,(u8*)atTerLabel, sizeof(TTERLABEL) * wMtNum);
		}
		else if( pcMsg->event == MCU_MT_JOINEDMTLIST_NACK )
		{
			//CONF_CTRL(h_ctrl_terminalListResponse, NULL, 0);
		}
		else if(pcMsg->event == MCU_MT_JOINEDMTLISTID_ACK)//terminal label+ID list response
		{
			TMt atMt[MAXNUM_CONF_MT];
			TMtAlias atMtAlias[MAXNUM_CONF_MT];
			u16 wRealMtItems = CMtAdpUtils::UnpackMtInfoListFromBuf(cServMsg.GetMsgBody(), 
								cServMsg.GetMsgBodyLen(), atMt, atMtAlias, MAXNUM_CONF_MT );

			if(wRealMtItems == 0) return;
						
			TTERINFO pTerInfo[MAXNUM_CONF_MT];
#ifdef _UTF8
			s8 achMtAlias[MAXLEN_ALIAS];
			BOOL32 bTrans = FALSE;
#endif
			for( s32 nMtItems = 0; nMtItems < wRealMtItems; nMtItems ++ )
			{
				if( atMt[nMtItems].GetMtId() == 0 || 
					atMt[nMtItems].GetMtId() > 192 )									
					continue;				
				
				pTerInfo[nMtItems].SetLabel((u8)g_cMtAdpApp.m_wMcuNetId, atMt[nMtItems].GetMtId());
				if( atMtAlias[nMtItems].m_AliasType != (u8)mtAliasTypeTransportAddress && 
					atMtAlias[nMtItems].m_achAlias[0] != 0 )
				{
#ifdef _UTF8
					memset(achMtAlias,0, sizeof(achMtAlias)); // 报给MT的名称也需要转换 [pengguofeng 5/20/2013]
					bTrans = TransEncoding(atMtAlias[nMtItems].m_achAlias, achMtAlias, sizeof(achMtAlias));
					if ( bTrans )
					{
						pTerInfo[nMtItems].SetName(achMtAlias);	//must be a non-null string					
					}
					else
					{
						pTerInfo[nMtItems].SetName(atMtAlias[nMtItems].m_achAlias);	//must be a non-null string					
					}
#else
					pTerInfo[nMtItems].SetName(atMtAlias[nMtItems].m_achAlias);	//must be a non-null string	
#endif
				}
				else
				{
					u32 dwIp = htonl(atMtAlias[nMtItems].m_tTransportAddr.GetIpAddr());
					sprintf(pTerInfo[nMtItems].m_achTerID, "%u.%u.%u.%u:%u", 
						    QUADADDR(dwIp), atMtAlias[nMtItems].m_tTransportAddr.GetPort());
				}
			}
			
			CONF_CTRL_STRICT( (u16)h_ctrl_requestAllTerminalIDsResponse, 
				              (u8*)pTerInfo, sizeof(TTERINFO) * wRealMtItems );			
		}
		else if( pcMsg->event == MCU_MT_JOINEDMTLISTID_NACK )
		{
			//CONF_CTRL(h_ctrl_requestAllTerminalIDsResponse, NULL, 0);
		}
		else
		{
			UNEXPECTED_MESSAGE( pcMsg->event );
			return;
		}		
		break;
		
	default:
		UNEXPECTED_MESSAGE( pcMsg->event );	
        break;
	}

	return;
}

/*=============================================================================
  函 数 名： ProcMcuVrsStartRecReq
  功    能： 开启录像请求
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2013/10       1.0      yanghuaizhi                创建
=============================================================================*/
void CMtAdpInst::ProcMcuVrsStartRecReq(CMessage * const pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );	
	
	if (CurState() != STATE_NORMAL)
	{
		UNEXPECTED_MESSAGE(pcMsg->event);
		return;
	}

	//MCU_REC_STARTREC_REQ消息内容：TMt(标识是否会议录像)+TEqp(标识录像设备)+ 录像文件名（129*s8）+ 发布方式（u8）+ 是否直播（u8）
	u8* pbyBuf = cServMsg.GetMsgBody();
	// 消息内容解析
	TMt tSrc = *(TMt*)pbyBuf;
	pbyBuf += sizeof(TMt);
	TEqp tEqp = *(TEqp*)pbyBuf;
	pbyBuf += sizeof(TEqp);
	s8 abyFileAlias[KDV_NAME_MAX_LENGTH+1];
	memcpy(abyFileAlias, pbyBuf, sizeof(abyFileAlias));
	pbyBuf += sizeof(abyFileAlias);
	u8 byIsPublishMode = *pbyBuf;	//是否发布
	pbyBuf++;
	u8 byIsLiveMode = *pbyBuf;	//是否直播
	pbyBuf++;

	u32 nBufLen = sizeof(TMt) + sizeof(TEqp) + sizeof(abyFileAlias) + sizeof(byIsPublishMode) + sizeof(byIsLiveMode);

	if (!tEqp.IsNull())
	{
		// 非vrs新录播消息，回复Nack
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "The tSrc(id.%d) is not null, the vrs recorder cannot start video!\n", tEqp.GetEqpId() );
		BuildAndSendMsgToMcu( pcMsg->event + 2, cServMsg.GetMsgBody(), nBufLen);
		return;
	}
	
	MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuVrsStartRecReq]start vrsrec req: tSrc(%d,%d) tEqp(%d)\n FileAlias:%s\n IsPublish.%d IsLive.%d\n",
			tSrc.GetMcuId(), tSrc.GetMtId(), tEqp.GetEqpId(), abyFileAlias, byIsPublishMode, byIsLiveMode);

	m_tRecSrcMt = tSrc;
	m_byRecType = TRecChnnlStatus::TYPE_RECORD;
	
	//TERMINAL_VRS_SETRECPARAM_REQ参数类型(u8)+参数长度(u16)+参数(不定长)
	//参数类型=1: 文件名长度(u16)+录像文件名(不定长)
	//参数类型=2: 发布方式(u8)
	//参数类型=3: 是否直播(u8)
	u8 byBufType = 0;
	u16 wBufLen = 0;
	// 文件名
	byBufType = 1;
	wBufLen = sizeof(abyFileAlias);
	wBufLen = htons(wBufLen);
	cServMsg.SetMsgBody((u8*)&byBufType, sizeof(byBufType));
	cServMsg.CatMsgBody((u8*)&wBufLen, sizeof(wBufLen));
	cServMsg.CatMsgBody((u8*)abyFileAlias, sizeof(abyFileAlias));
	// 发布方式
	byBufType = 2;
	wBufLen = sizeof(byIsPublishMode);
	wBufLen = htons(wBufLen);
	cServMsg.CatMsgBody((u8*)&byBufType, sizeof(byBufType));
	cServMsg.CatMsgBody((u8*)&wBufLen, sizeof(wBufLen));
	cServMsg.CatMsgBody(&byIsPublishMode, sizeof(byIsPublishMode));
	// 是否直播
	byBufType = 3;
	wBufLen = sizeof(byIsLiveMode);
	wBufLen = htons(wBufLen);
	cServMsg.CatMsgBody((u8*)&byBufType, sizeof(byBufType));
	cServMsg.CatMsgBody((u8*)&wBufLen, sizeof(wBufLen));
	cServMsg.CatMsgBody(&byIsLiveMode, sizeof(byIsLiveMode));

	// 发送开启消息
	ProcMcuSendMsgtoVrs(TERMINAL_VRS_SETRECPARAM_REQ, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
	// 设置两秒等待回复时间
	SetTimer(TIMER_VRS_REC_RSP, 3000);

	return;
}

/*=============================================================================
  函 数 名： ProcMcuVrsListAllRecordReq
  功    能： 获取文件列表
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2013/10       1.0      yanghuaizhi                创建
=============================================================================*/
void CMtAdpInst::ProcMcuVrsListAllRecordReq(CMessage * const pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );	
	
	if (CurState() != STATE_NORMAL)
	{
		UNEXPECTED_MESSAGE(pcMsg->event);
		return;
	}
	
	// MCU_REC_LISTALLRECORD_REQ消息内容：TEqp + 文件组Id（u32） + u32第几页 + u32单页个数
	u8* pbyBuf = cServMsg.GetMsgBody();
	// 消息内容解析
	TEqp tEqp = *(TEqp*)pbyBuf;
	pbyBuf += sizeof(TEqp);
	u32 dwGroupId = *(u32*)pbyBuf;//直接取网络须，不转主机序
	pbyBuf += sizeof(dwGroupId);
	u32 dwPageNo = *(u32*)pbyBuf;//直接取网络须，不转主机序
	pbyBuf += sizeof(dwPageNo);
	u32 dwMemNum = *(u32*)pbyBuf;//直接取网络须，不转主机序
	pbyBuf += sizeof(dwMemNum);

	
	if (!tEqp.IsNull())
	{
		// 非vrs新录播消息，回复Nack
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "The tSrc(id.%d) is not null, the vrs recorder cannot get filelist!\n", tEqp.GetEqpId() );
		BuildAndSendMsgToMcu( pcMsg->event + 2);
		return;
	}
	// 回复Ack
	BuildAndSendMsgToMcu( pcMsg->event + 1);
	
	m_byRecType = TRecChnnlStatus::TYPE_PLAY;
	
	//TERMINAL_VRS_GETFILELIST_REQ消息内容：文件组Id(u32)+起始条目(u32)+获取条目数(u32)

	// 请求分组内文件列表
	cServMsg.SetMsgBody((u8 *)&dwGroupId, sizeof(dwGroupId));
	cServMsg.CatMsgBody((u8 *)&dwPageNo, sizeof(dwPageNo));
	cServMsg.CatMsgBody((u8 *)&dwMemNum, sizeof(dwMemNum));

	ProcMcuSendMsgtoVrs(TERMINAL_VRS_GETFILELIST_CMD, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
	// 设置两秒等待回复时间
	SetTimer(TIMER_VRS_REC_RSP, 3000);

	return;
}

/*=============================================================================
  函 数 名： ProcMcuVrsStartPlayReq
  功    能： 开启放像请求
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2013/10       1.0      yanghuaizhi                创建
=============================================================================*/
void CMtAdpInst::ProcMcuVrsStartPlayReq(CMessage * const pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );	
	
	if (CurState() != STATE_NORMAL)
	{
		UNEXPECTED_MESSAGE(pcMsg->event);
		return;
	}
	
	//MCU_REC_STARTPLAY_REQ消息内容：TEqp(标识录像设备)+ 播放文件id(u32)
	u8* pbyBuf = cServMsg.GetMsgBody();
	// 消息内容解析
	TEqp tEqp = *(TEqp*)pbyBuf;
	pbyBuf += sizeof(TEqp);
	u32 dwFileId = *(u32*)pbyBuf;//不转主机序直接发送
	pbyBuf += sizeof(u32);
	
	if (!tEqp.IsNull())
	{
		// 非vrs新录播消息，回复Nack
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "The tSrc(id.%d) is not null, the vrs recorder cannot get filelist!\n", tEqp.GetEqpId() );
		BuildAndSendMsgToMcu( pcMsg->event + 2);
		return;
	}
	
	// 回复Ack
	BuildAndSendMsgToMcu( pcMsg->event + 1);

	m_byRecType = TRecChnnlStatus::TYPE_PLAY;

	//TERMINAL_VRS_SETPLYPARAM_REQ消息内容：参数类型(u8)+参数长度(u16)+参数(不定长)
	//参数类型=1：播放文件的文件ID(u32)
	//参数类型=2：播放时间(u32)
	u8 byBufType = 1;
	u16 wBufLen = sizeof(dwFileId);
	wBufLen = htons(wBufLen);
	cServMsg.SetMsgBody((u8 *)&byBufType, sizeof(byBufType));
	cServMsg.CatMsgBody((u8 *)&wBufLen, sizeof(wBufLen));
	cServMsg.CatMsgBody((u8 *)&dwFileId, sizeof(dwFileId));
	ProcMcuSendMsgtoVrs(TERMINAL_VRS_SETPLYPARAM_CMD, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());

	return;
}

/*=============================================================================
  函 数 名： ProcMcuVrsSeekReq
  功    能： 放像进度拖动
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2013/10       1.0      yanghuaizhi                创建
=============================================================================*/
void CMtAdpInst::ProcMcuVrsSeekReq(CMessage * const pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );	
	
	if (CurState() != STATE_NORMAL)
	{
		UNEXPECTED_MESSAGE(pcMsg->event);
		return;
	}

	//MCU_REC_SEEK_REQ消息体内容：TEqp+TRecProg
	u8* pbyBuf = cServMsg.GetMsgBody();
	// 消息内容解析
	TEqp tEqp = *(TEqp*)pbyBuf;
	pbyBuf += sizeof(TEqp);
	TRecProg tRecProg = *(TRecProg*)pbyBuf;
	pbyBuf += sizeof(TRecProg);
	
	if (!tEqp.IsNull())
	{
		// 非vrs新录播消息，回复Nack
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "The tSrc(id.%d) is not null, the vrs recorder cannot get filelist!\n", tEqp.GetEqpId() );
		BuildAndSendMsgToMcu( pcMsg->event + 2);
		return;
	}
	
	// 回复Ack
	BuildAndSendMsgToMcu( pcMsg->event + 1);
	MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuVrsSeekReq]SeekProg is %6d/%-6d.\n",
			tRecProg.GetCurProg(), tRecProg.GetTotalTime());

	u32 wSeekProg = tRecProg.GetCurProg();
	wSeekProg = htonl( wSeekProg );//转为网络序
	//TERMINAL_VRS_SETPLYPARAM_REQ消息内容：参数类型(u8)+参数长度(u16)+参数(不定长)
	//参数类型=1：播放文件的文件ID(u32)
	//参数类型=2：播放时间(u32)
	u8 byBufType = 1;
	u16 wBufLen = sizeof(wSeekProg);
	wBufLen = htons(wBufLen);
	cServMsg.SetMsgBody((u8 *)&byBufType, sizeof(byBufType));
	cServMsg.CatMsgBody((u8 *)&wBufLen, sizeof(wBufLen));
	cServMsg.CatMsgBody((u8 *)&wSeekProg, sizeof(wSeekProg));
	ProcMcuSendMsgtoVrs(TERMINAL_VRS_SETPLYPARAM_CMD, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());

	return;
}

/*=============================================================================
  函 数 名： ProcMcuVrsGetVrsInfo
  功    能： 获得Vrs相关信息，状态或进度等
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2013/10       1.0      yanghuaizhi                创建
=============================================================================*
void CMtAdpInst::ProcMcuVrsGetVrsInfo(CMessage * const pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );	
	
	if (CurState() != STATE_NORMAL)
	{
		UNEXPECTED_MESSAGE(pcMsg->event);
		return;
	}
	
	MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[ProcMcuVrsGetVrsInfo]pcMsg->event.%d.\n", pcMsg->event );

	switch (pcMsg->event)
	{
	case MCU_REC_GETRECORDCHNSTATUS_REQ://MCU请求获得录像机录像状态请求
		BuildAndSendMsgToMcu( pcMsg->event + 1);
		ProcMcuSendMsgtoVrs(MCU_VRS_GETRECSTATUS_CMD);
		break;
	case MCU_REC_GETPLAYCHNSTATUS_REQ://MCU请求获得录像机放像状态请求
		BuildAndSendMsgToMcu( pcMsg->event + 1);
		ProcMcuSendMsgtoVrs(MCU_VRS_GETPLYSTATUS_CMD);
		break;
	case MCU_REC_GETRECPROG_CMD://获取录像进度
		ProcMcuSendMsgtoVrs(MCU_VRS_GETRECPROG_CMD);
		break;
	case MCU_REC_GETPLAYPROG_CMD://获取放像进度
		ProcMcuSendMsgtoVrs(MCU_VRS_GETPLYPROG_CMD);
		break;
	default:
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcMcuVrsGetVrsInfo]pcMsg->event.%d is not correct, return!\n", pcMsg->event );
		break;
	}

	return;
}*/

/*=============================================================================
  函 数 名： ProcMcuSendMsgtoVrs
  功    能： 给Vrs发非标消息
  算法实现： 
  全局变量： 
  参    数： u16 wEventId, LPCSTR pBuf, u16 nBufLen
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2013/10       1.0      yanghuaizhi                创建
=============================================================================*/
void CMtAdpInst::ProcMcuSendMsgtoVrs(u16 wEventId, u8* pBuf, u16 nBufLen)
{
	if (pBuf == NULL && nBufLen > 0)
	{
		nBufLen = 0;
	}

	u8 *pbyBuf;
	u8	abyMsgBuf[CASCADE_NONSTANDARDMSG_MAXLEN];//消息体
	u16 wMsgBodyLen = nBufLen;
	// 消息内容按 eventid + bodyLen + body 方式存储和解析
	pbyBuf = abyMsgBuf;
	// 添加eventid
	wEventId = htons(wEventId);
	memcpy(pbyBuf, &wEventId, sizeof(wEventId));
	pbyBuf += sizeof(wEventId);
	// 添加msgbody长度
	wMsgBodyLen = htons(wMsgBodyLen);
	memcpy(pbyBuf, &wMsgBodyLen, sizeof(wMsgBodyLen));
	pbyBuf += sizeof(wMsgBodyLen);
	// 添加消息内容
	memcpy(pbyBuf, pBuf, nBufLen);
	
	CONF_CTRL_STRICT((u16)h_ctrl_nonstandardVrsDef, abyMsgBuf, nBufLen + sizeof(wEventId) + sizeof(wMsgBodyLen));
}

/*=============================================================================
  函 数 名： ProcMcuGetMsgFormVrs
  功    能： 收到Vrs发来的消息
  算法实现： 
  全局变量： 
  参    数： CServMsg& cMsg, u16 wEventId, u16 wBufLen, u8* pbyBuf
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2013/10       1.0      yanghuaizhi                创建
=============================================================================*/
void CMtAdpInst::ProcMcuGetMsgFormVrs(CServMsg& cMsg, u16 wEventId, u16 wBufLen, u8* pbyBuf)
{
	if (pbyBuf == NULL)
	{
		wBufLen = 0;
	}

	TEqp tRecEqp;//用于区分新旧录播，vrs新录播一律填空
	u8 byRspType = 0;
	u16 wErrorCode = 0;
	switch( wEventId )
	{
	case VRS_TERMINAL_SETRECPARAM_RSP://开始录像下参响应内容:回应类型(u8)+错误号(u16)[回应类型=0: ack,回应类型=1: nack]
		{
			if (wBufLen < sizeof(byRspType) + sizeof(wErrorCode))
			{
				MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[ProcMcuGetMsgFormVrs]Msg:VRS_TERMINAL_SETRECPARAM_RSP BufLen.%d is not correct, return!\n", wBufLen );
				break;
			}
			KillTimer(TIMER_VRS_REC_RSP);
			byRspType = *pbyBuf;
			wErrorCode = *(u16*)(pbyBuf + sizeof(byRspType));
			wErrorCode = ntohs(wErrorCode);//转为主机序
			if (byRspType == 0)//开始录像Ack
			{
				cMsg.SetEventId(REC_MCU_STARTREC_ACK);
				cMsg.SetMsgBody((u8 *)&tRecEqp, sizeof(tRecEqp));
			}
			else//开始录像Nack
			{
				cMsg.SetEventId(REC_MCU_STARTREC_NACK);
				cMsg.SetMsgBody((u8 *)&tRecEqp, sizeof(tRecEqp));
				cMsg.SetErrorCode(wErrorCode);
			}
		}
		break;
	case VRS_TERMINAL_STATUS_NTF:
		{
			// 录像状态上报，消息体：录像状态(u8)+发布方式(u8)+是否直播(u8)+文件名长度(u16)+录像文件名(不定长)(录像状态: 0 未录像, 1 正在录像, 2 暂停录像)
			// 放像状态上报，消息体：放像状态(u8)+文件名长度(u16)+放像文件显示名(不定长) (放像状态: 0 未录像, 1 正在放像, 2 暂停放像)
			TRecChnnlStatus tRecChlStatus;
			if (m_byRecType == TRecChnnlStatus::TYPE_RECORD)
			{
				tRecChlStatus.m_byType = TRecChnnlStatus::TYPE_RECORD;
				tRecChlStatus.m_byRecMode = TRecChnnlStatus::MODE_REALTIME;//实时录像
			}
			else if (m_byRecType == TRecChnnlStatus::TYPE_PLAY)
			{
				tRecChlStatus.m_byType = TRecChnnlStatus::TYPE_PLAY;
			}
			else
			{
				MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[ProcMcuGetMsgFormVrs]Msg:VRS_TERMINAL_STATUS_NTF m_byRecType.%d is not correct, return!\n", m_byRecType );
				break;
			}
			//消息解析
			u8 *pTmpBuf = pbyBuf;
			u8 byCurVrsState = *pTmpBuf;//考虑转换vrs状态为mcu能识别的rec状态
			tRecChlStatus.m_byState = GetRecStateByVrsState(byCurVrsState, m_byRecType);
			pTmpBuf += sizeof(u8);
			if (m_byRecType == TRecChnnlStatus::TYPE_RECORD)
			{
				pTmpBuf += sizeof(u8);//跳过发布方式
				pTmpBuf += sizeof(u8);//跳过是否直播
			}
			u16 wFileNameLen = *(u16 *)pTmpBuf;//文件名长度
			wFileNameLen = ntohs(wFileNameLen);
			pTmpBuf += sizeof(u16);//跳过文件名长度
			tRecChlStatus.SetRecordName((s8*)pTmpBuf);
			pTmpBuf += wFileNameLen;

			if (m_byRecType == TRecChnnlStatus::TYPE_RECORD)
			{
				cMsg.SetEventId(REC_MCU_RECORDCHNSTATUS_NOTIF);//消息体为TMt+TEqp+TRecChnnlStatus
				cMsg.SetMsgBody((u8 *)&m_tRecSrcMt, sizeof(m_tRecSrcMt));
				cMsg.CatMsgBody((u8 *)&tRecEqp, sizeof(tRecEqp));
				cMsg.CatMsgBody((u8 *)&tRecChlStatus, sizeof(tRecChlStatus));
			}
			else
			{
				cMsg.SetEventId(REC_MCU_PLAYCHNSTATUS_NOTIF);//消息体为TEqp+TRecChnnlStatus
				cMsg.SetMsgBody((u8 *)&tRecEqp, sizeof(tRecEqp));
				cMsg.CatMsgBody((u8 *)&tRecChlStatus, sizeof(tRecChlStatus));
			}
		}
		break;
	case VRS_TERMINAL_PROG_NTF:				//放像进度上报，消息体：当前进度时间（u32）+总进度时间（u32）
		{
			if (wBufLen < sizeof(TRecProg))
			{
				MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[ProcMcuGetMsgFormVrs]Msg:VRS_TERMINAL_PROG_NTF BufLen.%d is not correct, return!\n", wBufLen );
				break;
			}
			
			TRecProg tRecProg;
			memcpy(&tRecProg, pbyBuf, sizeof(tRecProg));//直接复制进度，无需再转主机序
			
			if (m_byRecType == TRecChnnlStatus::TYPE_RECORD)
			{
				cMsg.SetEventId(REC_MCU_RECORDPROG_NOTIF);//消息体为Tmt+TEqp+TRecProg
				cMsg.SetMsgBody((u8 *)&m_tRecSrcMt, sizeof(m_tRecSrcMt));
				cMsg.CatMsgBody((u8 *)&tRecEqp, sizeof(tRecEqp));
			}
			else if (m_byRecType == TRecChnnlStatus::TYPE_PLAY)
			{
				cMsg.SetEventId(REC_MCU_PLAYPROG_NOTIF);//消息体为TEqp+TRecProg
				cMsg.SetMsgBody((u8 *)&tRecEqp, sizeof(tRecEqp));
			}
			cMsg.CatMsgBody((u8 *)&tRecProg, sizeof(tRecProg));
		}
		break;
	case VRS_TERMINAL_FILELIST_NTF:
		{
			KillTimer(TIMER_VRS_REC_RSP);
			// 文件名列表上报，消息体：文件类型(u8)+ID(u32)+名字(s8*129)+number(u32)
			// 文件类型：0=普通视频文件，1=文件组
			// 如果文件类型=0，文件信息=文件类型+文件ID+文件名字+文件所属组ID
			// 如果文件类型=1，文件信息=文件类型+文件组ID+组名+组内文件个数
			// 不解析消息内容，直接透传给mcu，再由mcu透传给mcs

			//REC_MCU_LISTALLRECORD_NOTIF消息内容：
			//TEqp + TMtAlias（新录播）+ 文件类型(u8)+ID(u32)+名字(s8*129)+number(u32)
			cMsg.SetEventId(REC_MCU_LISTALLRECORD_NOTIF);
			cMsg.SetMsgBody((u8 *)&tRecEqp, sizeof(tRecEqp));
			cMsg.CatMsgBody((u8 *)&m_tMtAlias, sizeof(m_tMtAlias));
			cMsg.CatMsgBody(pbyBuf, wBufLen);
		}
		break;
	default:
		MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[ProcMcuGetMsgFormVrs]Msg:Event id.%d is not correct, return!\n", wEventId );
		break;
	}

	return;
}

/*=============================================================================
  函 数 名： ProcVrsRspTimeout
  功    能： 收到Vrs发来的消息
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2013/10       1.0      yanghuaizhi                创建
=============================================================================*/
void CMtAdpInst::ProcVrsRspTimeout(void)
{
	KillTimer(TIMER_VRS_REC_RSP);
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcVrsRspTimeout]receive Vrs Rsp msg timeout. ClearInst.\n");
	ClearInst();

	return;
}

/*=============================================================================
  函 数 名： GetRecStateByVrsState
  功    能： 收到Vrs发来的消息
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2013/11       1.0      yanghuaizhi                创建
=============================================================================*/
u8 CMtAdpInst::GetRecStateByVrsState(u8 byVrsState, u8 byRecType)
{
	u8 byRecState = TRecChnnlStatus::STATE_IDLE;
	
	//录像状态：1 未录像，2 正在录像，3 暂停录像
	//放像状态：1 未放像，2 正在播放，3 暂停播放
	switch (byVrsState)
	{
	case RP_STATE_WORKING:
		{
			if (byRecType == TRecChnnlStatus::TYPE_RECORD)
			{
				byRecState = TRecChnnlStatus::STATE_RECORDING;
			}
			else
			{
				byRecState = TRecChnnlStatus::STATE_PLAYING;
			}
		}
		break;
	case RP_STATE_PAUSE:
		{
			if (byRecType == TRecChnnlStatus::TYPE_RECORD)
			{
				byRecState = TRecChnnlStatus::STATE_RECPAUSE;
			}
			else
			{
				byRecState = TRecChnnlStatus::STATE_PLAYPAUSE;
			}
		}
		break;
	default:
		break;
	}

	return byRecState;
}

/*=============================================================================
  函 数 名： DaemonProcMtMcuConfMsg
  功    能： 处理终端到业务Daemon实例的入口
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::DaemonProcMtMcuConfMsg( CMessage * const  pcMsg )
{
	if( NULL == pcMsg )
	{
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "The received msg's pointer in the msg entry is NULL!");
		return;
	}

	if( CurState() == STATE_NORMAL )
	{	
		SendMsgToMcuDaemon( pcMsg->event, pcMsg->content, pcMsg->length );	
	}

	return;
}


/*=============================================================================
  函 数 名： ProcTimerExpired
  功    能： 定时器超时处理函数
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcTimerExpired( CMessage * const  pcMsg )
{
	CServMsg cServMsg;
	
	switch( CurState() )
	{
	case STATE_CALLING:
	case STATE_GKCALLING:

		if( pcMsg->event == WAIT_RESPONSE_TIMEOUT )
		{			
			KillTimer(pcMsg->event);
			
			if( m_wSavedEvent != 0 )
			{
				cServMsg.SetErrorCode((u16)ERR_MCU_TIMEROUT);
				cServMsg.SetEventId(m_wSavedEvent + 2);
				SendMsgToMcu(cServMsg);												
			}
			
			if(CurState() == STATE_CALLING )
			{
				MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcTimerExpired]. ClearInst.\n");
				ClearInst();
			}
		}
		else
        {
            UNEXPECTED_MESSAGE(pcMsg->event);
        }			
		break;	

	case STATE_NORMAL:
		if(WAIT_PASSWORD_TIMEOUT == pcMsg->event)
		{			
			KillTimer( pcMsg->event );
			cServMsg.SetErrorCode( (u16)ERR_MCU_TIMEROUT );
			cServMsg.SetEventId( MT_MCU_ENTERPASSWORD_NACK );
			SendMsgToMcu( cServMsg );
		}

		else if(TIMER_VIDEOINFO == pcMsg->event)
		{
			KillTimer( TIMER_VIDEOINFO );
			TMcuMcuReq tReq;
			memset( &tReq, 0, sizeof(tReq) );
			cServMsg.SetMsgBody( (u8 *)&tReq, sizeof(tReq) );
			cServMsg.SetEventId(MCU_MCU_VIDEOINFO_REQ);
			post( MAKEIID(AID_MCU_MTADP, GetInsID()), cServMsg.GetEventId(),
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			
			SetTimer(TIMER_VIDEOINFO, g_dwVidInfoTimeout);
		}
		else if(TIMER_AUDIOINFO == pcMsg->event)
		{
			KillTimer( TIMER_AUDIOINFO );
			TMcuMcuReq tReq;
			memset(&tReq, 0, sizeof(tReq));
			cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
			cServMsg.SetEventId(MCU_MCU_AUDIOINFO_REQ);
			post( MAKEIID(AID_MCU_MTADP, GetInsID()), cServMsg.GetEventId(),
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

			SetTimer(TIMER_AUDIOINFO, g_dwAudInfoTimeout);
		}
		else if(TIMER_MTLIST == pcMsg->event)
		{
			KillTimer(TIMER_MTLIST);
/*			TMcuMcuReq tReq;
			memset( &tReq, 0, sizeof(tReq) );
			cServMsg.SetEventId( MCU_MCU_MTLIST_REQ );
			cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
			post( MAKEIID(AID_MCU_MTADP, GetInsID()), cServMsg.GetEventId(),
				  cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
*/
			ProcMcuMtadpRequestMtList(0xff); // 上报本Mcu下挂的所有SMcu列表 [pengguofeng 7/1/2013]

			SetTimer(TIMER_MTLIST, g_dwPartlistInfoTimeout);
		}
		//20101209_tzy 针对R3_FULL级联定制，定时器到时向下级发送开启级联定制混音命令
		else if (TIMER_STARTAFTERSTOPSMIX == pcMsg->event)
		{
			KillTimer(TIMER_STARTAFTERSTOPSMIX);
			u32 dwMcuId  = (u32)(*((u32*)pcMsg->content));
			CServMsg cStartServMsg;
			u8 byDepth = MAXNUM_EMIXER_DEPTH ;
			cStartServMsg.SetMsgBody(&byDepth,sizeof(u8));

			u32 dwTmpPartId = REMOTE_MCU_PARTID;
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcTimerExpired] R3full add mmcu ID to smcu PARTID(%d)\n",dwTmpPartId);
			dwTmpPartId = htonl(dwTmpPartId);
			cStartServMsg.CatMsgBody( (u8 *)&dwTmpPartId, sizeof(u32) );

			for( u8   byByteIdx=0 ;byByteIdx<(MAXNUM_CONF_MT>>3) ;byByteIdx++ )
			{
				for( u8   byBitIdx = 0 ;byBitIdx < 8 ;byBitIdx++ )
				{
					if((m_dwSmcuMixMemBeforeStart[byByteIdx] >> byBitIdx) & 0x1 )
					{
						u8 byMtId = byByteIdx*8 + byBitIdx + 1;
						TMt tTempMt ;
						tTempMt.SetMcuId((u16)dwMcuId);
						tTempMt.SetMtId(byMtId);
						BOOL32 bLocal = FALSE;
						u32 dwPartId = GetPartIdFromMt(tTempMt, &bLocal);
						MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcTimerExpired] PARTID(%d)\n",dwPartId);
						dwPartId = htonl(dwPartId);
						cStartServMsg.CatMsgBody( (u8 *)&dwPartId, sizeof(u32) );
					}
				}

				// [miaoqingsong 20111020] 将保存的终端加进MCU_MCU_STARTMIXER_CMD消息体后要清空数组，以免有残留导致后续操作有问题
				m_dwSmcuMixMemBeforeStart[byByteIdx] = 0;
			}
			cStartServMsg.SetEventId(MCU_MCU_STARTMIXER_CMD);
			TNonStandardReq tReq;
			tReq.m_aszUserName[0] = 0;
			tReq.m_aszUserPass[0] = 0;
			tReq.m_nReqID = m_dwMMcuReqId++;
			tReq.m_nMsgLen = cStartServMsg.GetServMsgLen();
			if( (u32)tReq.m_nMsgLen > sizeof(tReq.m_abyMsgBuf) )
			{
				StaticLog( "[MTADAP][EXP]StartMixerCmd message is too large(len:%d)\n", tReq.m_nMsgLen);
				return;
			}
			memcpy( tReq.m_abyMsgBuf, cStartServMsg.GetServMsg(), tReq.m_nMsgLen );
			HCHAN hsChan = GetMMcuChannel();
			kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_REQ, &tReq, sizeof(tReq) );
		}
// 		else if (pcMsg->event == TIMER_H245_CAPBILITY_NOTIFY_OVERTIME)
// 		{
// 			MAPrint(LOG_LVL_ERROR,MID_MCULIB_MTADP,"[ProcTimerExpired] TIMER_H245_CAPBILITY_NOTIFY OVERTIME!clearInst!\n");
// 			ClearInst();
// 		}
		else
		{
			UNEXPECTED_MESSAGE( pcMsg->event );
		}
		break;

	default:
		UNEXPECTED_MESSAGE( pcMsg->event );
		break;
	}

	return;
}

/*=============================================================================
  函 数 名： ProcMcuMtRoundTripDelayReq
  功    能： 定时发送roundTripDelayReq
  算法实现： 
  全局变量： 
  参    数： CMessage * const  pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcMcuMtRoundTripDelayReq( CMessage * const  pcMsg )
{
	s32 nDelay = ROUNDTRIP_TIMEOUT;

	switch( CurState() )
	{
	case STATE_NORMAL:
		CONF_CTRL_STRICT((u16)h_ctrl_roundTripDelay, (void*)&nDelay, sizeof(s32));
        if( g_nPrtdid > 0 )
		{
            if( g_nPrtdid == m_byMtId )
			{
				StaticLog( "RoundTripDelay send to mt:%d, curticks:%d\n",m_byMtId, OspTickGet() );
			}
		}
		else if( g_nPrtdid == 0 )
		{
			StaticLog( "RoundTripDelay send to mt:%d, curticks:%d\n", m_byMtId, OspTickGet() );
		}	

		break;

	default:
		UNEXPECTED_MESSAGE( pcMsg->event);
        break;
	}
	return;
}


/*=============================================================================
  函 数 名： ProcMtMcuMsdRsp
  功    能： 处理主从决定
  算法实现： 
  全局变量： 
  参    数： u8 byMsdResult
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::ProcMtMcuMsdRsp( u8 byMsdResult )
{
	u8 byStatus = byMsdResult;

	switch ( CurState() ) 
	{
	case STATE_NORMAL:
		if( (u8)msdse_master == byStatus )
		{			
			m_bMaster = TRUE;
            m_bMsdOK = TRUE;
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "MSD on Call status is MASTER.\n" );	
		}
		else if( (u8)msdse_slave == byStatus )
		{
			m_bMaster = FALSE;
            m_bMsdOK = TRUE;
			MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "Slave role. Wrong MSD Result.\n" );			
		}
		else
		{
            //第三次挂断
            if (m_byMsdFailTimes < 2)
            {
                m_byMsdFailTimes++;

                if (!m_bMsdOK)
                {
                    s32 anBuf[1];	
                    anBuf[0] = (s32)emMsActiveMC;
                    if( !IS_MT&&m_byDirect == (u8)CALL_INCOMING )
                    {
                        anBuf[0] = (s32)emMsMT;//terminalType
                    }; 
                    if( kdvSendConfCtrlMsg( m_hsCall, (u16)h_ctrl_masterSlaveDetermine, 
                        (void*)anBuf, sizeof(anBuf)) == (s32)act_err )
                    {
                        MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[ProcMtMcuMsdRsp] Send msd failed.\n");	
                    }
                    else
                    {
                        MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[ProcMtMcuMsdRsp] Manually MSD sent out \n");
                    }
                }                
            }
            else
            {
                ClearInst();
            }
			MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "MSD Result Error. FailTimes :%d\n", m_byMsdFailTimes);			
		}	
		break;
	
	default:
		MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "MSD message received in wrong state %u!\n", CurState());
		break;
	}
	return;
}


/*=============================================================================
  函 数 名： DoStatistics
  功    能： 统计函数
  算法实现： 
  全局变量： 
  参    数： u32 dwNextState，本实例准备跳转的下一个状态
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::DoStatistics(u32 dwNextState)
{
	switch(CurState())
	{
	case STATE_IDLE:
		
		switch(dwNextState) 
		{
		case STATE_CALLING:
			g_cMtAdpApp.m_tMaStaticInfo.m_byCurNumCalling ++ ;

			if(g_cMtAdpApp.m_tMaStaticInfo.m_byCurNumCalling > g_cMtAdpApp.m_tMaStaticInfo.m_byMaxNumCalling)
            {
                g_cMtAdpApp.m_tMaStaticInfo.m_byMaxNumCalling = g_cMtAdpApp.m_tMaStaticInfo.m_byCurNumCalling;
            }
            
			if((u8)CALL_INCOMING == m_byDirect)
            {
                g_cMtAdpApp.m_tMaStaticInfo.m_dwIncomingCalls ++ ;
            }
            else if((u8)CALL_OUTGOING == m_byDirect)
            {
                g_cMtAdpApp.m_tMaStaticInfo.m_dwOutgoingCalls ++ ;
            }
			break;
		default:
			break;
		}
		break;

	case STATE_CALLING:	

		switch(dwNextState)
		{
		case STATE_IDLE:			
			g_cMtAdpApp.m_tMaStaticInfo.m_dwFailedCalls ++ ;
			g_cMtAdpApp.m_tMaStaticInfo.m_byCurNumCalling -- ;
			break;

		case STATE_NORMAL:	
			g_cMtAdpApp.m_tMaStaticInfo.m_dwSuccessfulCalls ++ ;
			g_cMtAdpApp.m_tMaStaticInfo.m_byCurNumOnline ++ ;	
			g_cMtAdpApp.m_tMaStaticInfo.m_byCurNumCalling -- ;

			if(g_cMtAdpApp.m_tMaStaticInfo.m_byCurNumOnline > g_cMtAdpApp.m_tMaStaticInfo.m_byMaxNumOnline)	
            {
                g_cMtAdpApp.m_tMaStaticInfo.m_byMaxNumOnline = g_cMtAdpApp.m_tMaStaticInfo.m_byCurNumOnline;
            }				
			break;
		default:
			break;
		}
		break;

	case STATE_GKCALLING:	
		switch(dwNextState)
		{
		case STATE_IDLE:			
			g_cMtAdpApp.m_tMaStaticInfo.m_dwFailedCalls++;
			g_cMtAdpApp.m_tMaStaticInfo.m_byCurNumCalling--;
			break;
		default:
			break;
		}
		break;

	case STATE_NORMAL:
		switch ( dwNextState )
		{
		case STATE_IDLE:
			g_cMtAdpApp.m_tMaStaticInfo.m_byCurNumOnline -- ;				
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return;
}


/*=============================================================================
  函 数 名： FindChannel
  功    能： 根据协议栈句柄查找信道结构 
  算法实现： 
  全局变量： 
  参    数： HCHAN hsChan
  返 回 值： s32 信道索引
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
s32 CMtAdpInst::FindChannel( HCHAN hsChan )
{
	for(s32 nCallChanNum = 0; nCallChanNum < MAXNUM_CALL_CHANNEL; nCallChanNum ++ )
	{
		if( m_atChannel[nCallChanNum].hsChan == hsChan )
        {
    		return nCallChanNum;
        }
	}
	return -1;
}


/*=============================================================================
  函 数 名： FindChannelByPayloadType
  功    能： 根据信道方向和媒体类型查找信道结构 
  算法实现： 
  全局变量： 
  参    数： u8 byChanDirect
             u8 byPayloadType
             u8 byMode
  返 回 值： 信道索引
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
s32	CMtAdpInst::FindChannelByPayloadType( u8 byChanDirect, u8 byPayloadType, u8 byMode )
{
	for(s32 nCallChanNum = 0; nCallChanNum < MAXNUM_CALL_CHANNEL; nCallChanNum ++ )
	{
		if( m_atChannel[nCallChanNum].byDirect == byChanDirect && 
            m_atChannel[nCallChanNum].tLogicChan.GetMediaType() == byMode &&
			m_atChannel[nCallChanNum].tLogicChan.GetChannelType() == byPayloadType )
		{
			return nCallChanNum;
		}
	}
	return -1;
}

/*=============================================================================
  函 数 名： FindChannelByMediaType
  功    能： 根据信道方向和媒体类型查找信道结构
  算法实现： 
  全局变量： 
  参    数：  u8 byChanDirect
             u8 byMediaType
  返 回 值： 信道索引
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
s32	CMtAdpInst::FindChannelByMediaType( u8 byChanDirect, u8 byMediaType )
{
	for( s32 nCallChanNum = 0; nCallChanNum < MAXNUM_CALL_CHANNEL; nCallChanNum ++ )
	{
		if( m_atChannel[nCallChanNum].byDirect == byChanDirect && 
			m_atChannel[nCallChanNum].tLogicChan.GetMediaType() == byMediaType)
			
			return nCallChanNum;
	}
	return -1;
}


/*=============================================================================
  函 数 名： GetFreeChannel
  功    能： 分配空闲信道 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： s32 信道索引 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
s32 CMtAdpInst::GetFreeChannel()
{
	for( s32 nCallChanNum = 0; nCallChanNum < MAXNUM_CALL_CHANNEL; nCallChanNum ++ )
	{
		if( NULL == m_atChannel[nCallChanNum].hsChan )
        {
            return nCallChanNum ;
        }			
	}
	return -1;
}

/*=============================================================================
  函 数 名： GetMMcuChannel
  功    能： 得到级联通道
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： HCHAN 通道值 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
HCHAN CMtAdpInst::GetMMcuChannel()
{
	for ( s32 nCallChanNum = 0; nCallChanNum < MAXNUM_CALL_CHANNEL; nCallChanNum++ )
	{
		if( MEDIA_TYPE_MMCU == m_atChannel[nCallChanNum].tLogicChan.GetChannelType() )
        {
            return m_atChannel[nCallChanNum].hsChan;
        }      
	}	
	return NULL;
}

/*=============================================================================
  函 数 名： FreeChannel
  功    能： 释放信道结构
  算法实现： 
  全局变量： 
  参    数： s32 nChanIdx
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::FreeChannel( s32 nChanIdx )
{
	if( 0 <= nChanIdx && nChanIdx < MAXNUM_CALL_CHANNEL )
	{
        m_atChannel[nChanIdx].Clear();
	}
	return;
}


/*=============================================================================
  函 数 名： FreeChannel
  功    能： 释放信道结构 
  算法实现： 
  全局变量： 
  参    数： HCHAN hsChan
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::FreeChannel(HCHAN hsChan)
{
	s32 nChanIdx = FindChannel(hsChan);

	if( 0 <= nChanIdx && nChanIdx < MAXNUM_CALL_CHANNEL )
	{
        m_atChannel[nChanIdx].Clear();
	}

	return;
}

/*=============================================================================
  函 数 名： MtInfo2Part
  功    能： 
  算法实现： 
  全局变量： 
  参    数： TPart *ptPart
             TMcuMcuMtInfo *ptInfo
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2013/5/24     2.0         彭国锋　　　　　　　　发给对方mtadp前，将EpDesc/PartName/DialString进行转码
=============================================================================*/
void CMtAdpInst::MtInfo2Part( TPart *ptPart, TMcuMcuMtInfo *ptInfo )
{
	if( !ptInfo->m_tMt.IsMcuIdLocal() )
	{
		ptInfo->m_abyMtIdentify[ptInfo->m_byCasLevel] = ptInfo->m_tMt.GetMtId();
		++ptInfo->m_byCasLevel;	
		ptInfo->m_tMt.SetMtId( (u8)ptInfo->m_tMt.GetMcuId() );
		ptInfo->m_tMt.SetMcuId( LOCAL_MCUID );
	}
	ptPart->m_bAudioMuteIn  = ISTRUE(ptInfo->m_byIsAudioMuteIn);
	ptPart->m_bAudioMuteOut = ISTRUE(ptInfo->m_byIsAudioMuteOut);
	ptPart->m_bDataMeeting  = ISTRUE(ptInfo->m_byIsDataMeeting);
	ptPart->m_bFECCEnable   = ISTRUE(ptInfo->m_byIsFECCEnable);
	ptPart->m_bInvited      = TRUE;
	ptPart->m_bLocal        = TRUE;
	ptPart->m_bMaster       = FALSE;
	ptPart->m_bVideoMuteIn  = ISTRUE(ptInfo->m_byIsVideoMuteIn);
	ptPart->m_bVideoMuteOut = ISTRUE(ptInfo->m_byIsVideoMuteOut);
	ptPart->m_dwIP          = ptInfo->m_dwMtIp;
	ptPart->m_dwPID         = MAKEIID(ptInfo->m_tMt.GetMcuId(), ptInfo->m_tMt.GetMtId());//0x961aca4c;//
	ptPart->m_emAudioIn     = (PayloadType)CMtAdpUtils::PayloadTypeIn2Out(ptInfo->m_byAudioIn);
	ptPart->m_emAudioOut    = (PayloadType)CMtAdpUtils::PayloadTypeIn2Out(ptInfo->m_byAudioOut);
	ptPart->m_emConnState   = (ISTRUE(ptInfo->m_byIsConnected)) ? emConnStateConnected : emConnStateDisconnected;
	ptPart->m_emEPType      = (ptInfo->m_byMtType != MT_TYPE_SMCU && ptInfo->m_byMtType != MT_TYPE_MMCU) ? emEPTypeTerminal : emEPTypeMCU;
	ptPart->m_emVideo2In    = (PayloadType)CMtAdpUtils::PayloadTypeIn2Out(ptInfo->m_byVideo2In); 
	ptPart->m_emVideo2Out   = (PayloadType)CMtAdpUtils::PayloadTypeIn2Out(ptInfo->m_byVideo2Out);
	ptPart->m_emVideoIn     = (PayloadType)CMtAdpUtils::PayloadTypeIn2Out(ptInfo->m_byVideoIn); 
	ptPart->m_emVideoOut    = (PayloadType)CMtAdpUtils::PayloadTypeIn2Out(ptInfo->m_byVideoOut);
	s8 achMtAlias[STR_LEN]; //pengguofeng　多国语言
	memset(achMtAlias, 0, sizeof(achMtAlias));
	MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[MtInfo2Part]Mt. %x name:(%s ) STR_LEN:%d OnLine:%d\n",
		ptPart->m_dwPID, ptInfo->m_szMtName, STR_LEN, ptInfo->m_byIsConnected);
	BOOL32 bNeedTrans = TransEncoding(ptInfo->m_szMtName, achMtAlias, sizeof(achMtAlias));
	if ( bNeedTrans == TRUE )
	{
		strncpy(ptPart->m_aszDialStr, achMtAlias, sizeof(ptPart->m_aszDialStr));
	}
	else
	{
		strncpy(ptPart->m_aszDialStr, ptInfo->m_szMtName, sizeof(ptPart->m_aszDialStr));
	}
	ptPart->m_aszDialStr[sizeof(ptPart->m_aszDialStr)-1] = 0;
	ptPart->m_byCasLevel = ptInfo->m_byCasLevel;
	memcpy( &ptPart->m_abyMtIdentify[0], &ptInfo->m_abyMtIdentify[0], sizeof( ptPart->m_abyMtIdentify ) );
	
	memset( ptPart->m_aszEPDesc,0,sizeof(ptPart->m_aszEPDesc) );
	if( MT_MANU_KDC == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "kdcmt", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_KDCMCU == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "kdcmcu", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_NETMEETING == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "netmeetingmt", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_POLYCOM == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "polycommt", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_HUAWEI == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "huaweimt", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_TAIDE == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "taidemt", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_SONY == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "sonymt", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_VCON == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "vconmt", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_RADVISION == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "radvisionmcu", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_PENTEVIEW == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "pentviewmt", sizeof(ptPart->m_aszEPDesc));
	}
	ptPart->m_aszEPDesc[sizeof(ptPart->m_aszEPDesc)-1] = 0;
    
	// MtName也需要转换 [pengguofeng 5/24/2013]
	if ( bNeedTrans == TRUE )
	{
		strncpy( ptPart->m_aszPartName, achMtAlias, sizeof(ptPart->m_aszPartName) );
	}
	else
	{
		strncpy( ptPart->m_aszPartName, ptInfo->m_szMtName, sizeof(ptPart->m_aszPartName) );
	}
    ptPart->m_aszPartName[sizeof(ptPart->m_aszPartName)-1] = 0;

    //若对端为keda mcu填写本端终端协议类型字段
    if( MT_MANU_KDC == ptInfo->m_byManuId && MT_MANU_KDCMCU == m_byVendorId )
    {
        if(strlen(ptPart->m_aszPartName) >= sizeof(ptPart->m_aszEPDesc)-1)
        {
            ptPart->m_aszPartName[sizeof(ptPart->m_aszEPDesc)-2] = 0; //clear one char for protocol position
        }
        sprintf(ptPart->m_aszPartName, "%s%d", ptPart->m_aszPartName, ptInfo->m_byProtocolType);
    }

	//若对端为keda mcu填写本端终端协议类型字段(终端别名扩容代码，这里注掉留作参考)
// 	if( MT_MANU_KDC == ptInfo->m_byManuId && MT_MANU_KDCMCU == m_byVendorId )
//     {
//         if(strlen(ptPart->m_aszPartName) >= sizeof(ptPart->m_aszEPDesc)-1)
//         {			
// 			ptPart->m_aszPartName[sizeof(ptPart->m_aszPartName)-2] = 0;   //clear one char for protocol position
//         }
// 		sprintf(ptPart->m_aszPartName, "%s%d", ptPart->m_aszPartName, ptInfo->m_byProtocolType);
//     }
	
	memset(&(ptPart->m_tAVStatis), 0, sizeof(ptPart->m_tAVStatis));

	ptPart->m_tPartVideoInfo.m_nViewCount        = ptInfo->m_tPartVideoInfo.m_nViewCount;
	ptPart->m_tPartVideoInfo.m_nOutputViewID     = ptInfo->m_tPartVideoInfo.m_nOutputLID;
	ptPart->m_tPartVideoInfo.m_nOutVideoSchemeID = ptInfo->m_tPartVideoInfo.m_nOutVideoSchemeID;

	for( s32 nLoop = 0; nLoop < ptPart->m_tPartVideoInfo.m_nViewCount && nLoop < MAX_VIEWS_PER_CONF; nLoop ++ )
	{
		ptPart->m_tPartVideoInfo.m_anViewPos[nLoop].m_nViewID         = ptInfo->m_tPartVideoInfo.m_atViewPos[nLoop].m_nViewID; 
		ptPart->m_tPartVideoInfo.m_anViewPos[nLoop].m_bySubframeIndex = ptInfo->m_tPartVideoInfo.m_atViewPos[nLoop].m_bySubframeIndex;
	}
	ptPart->m_tStartTime.year   = 2005;
	ptPart->m_tStartTime.month  = 1;
	ptPart->m_tStartTime.day    = 1;
	ptPart->m_tStartTime.hour   = 0;
	ptPart->m_tStartTime.minute = 0;
	ptPart->m_tStartTime.second = 0;
	if( ptInfo->m_tMt.GetMtId() == m_byMtId )
	{
		ptPart->m_dwPID    = REMOTE_MCU_PARTID; //对方在本MCU上的条目
		ptPart->m_bLocal   = FALSE;
		ptPart->m_emEPType = emEPTypeCascConf;
		ptPart->m_bMaster  = !m_bMaster;
		if(ptPart->m_bMaster) ptPart->m_bInvited = FALSE;
	}

	return;
}

// 参数带UTF8的扩容版本 [pengguofeng 5/31/2013]
/*void CMtAdpInst::MtInfo2Part( TPartU *ptPart, TMcuMcuMtInfo *ptInfo )
{

	if( !ptInfo->m_tMt.IsMcuIdLocal() )
	{
		OspPrintf( TRUE, FALSE, "Mt<%d | %d > is not local\n", 	ptInfo->m_tMt.GetMcuId(), ptInfo->m_tMt.GetMtId());
		ptInfo->m_abyMtIdentify[ptInfo->m_byCasLevel] = ptInfo->m_tMt.GetMtId();
		++ptInfo->m_byCasLevel;	
		ptInfo->m_tMt.SetMtId( (u8)ptInfo->m_tMt.GetMcuId() );
		ptInfo->m_tMt.SetMcuId( LOCAL_MCUID );
	}
	ptPart->m_bAudioMuteIn  = ISTRUE(ptInfo->m_byIsAudioMuteIn);
	ptPart->m_bAudioMuteOut = ISTRUE(ptInfo->m_byIsAudioMuteOut);
	ptPart->m_bDataMeeting  = ISTRUE(ptInfo->m_byIsDataMeeting);
	ptPart->m_bFECCEnable   = ISTRUE(ptInfo->m_byIsFECCEnable);
	ptPart->m_bInvited      = TRUE;
	ptPart->m_bLocal        = TRUE;
	ptPart->m_bMaster       = FALSE;
	ptPart->m_bVideoMuteIn  = ISTRUE(ptInfo->m_byIsVideoMuteIn);
	ptPart->m_bVideoMuteOut = ISTRUE(ptInfo->m_byIsVideoMuteOut);
	ptPart->m_dwIP          = ptInfo->m_dwMtIp;
	ptPart->m_dwPID         = MAKEIID(ptInfo->m_tMt.GetMcuId(), ptInfo->m_tMt.GetMtId());//0x961aca4c;//
	OspPrintf(TRUE,FALSE,"来而[MtInfo2Part(U)]Mt(%d.%d) IsMcuIdLocal.%d Name:(%s ) makePID:%x\n",
		ptInfo->m_tMt.GetMcuId(), ptInfo->m_tMt.GetMtId(), ptInfo->m_tMt.IsMcuIdLocal(),
		ptInfo->m_szMtName, ptPart->m_dwPID);
	ptPart->m_emAudioIn     = (PayloadType)CMtAdpUtils::PayloadTypeIn2Out(ptInfo->m_byAudioIn);
	ptPart->m_emAudioOut    = (PayloadType)CMtAdpUtils::PayloadTypeIn2Out(ptInfo->m_byAudioOut);
	ptPart->m_emConnState   = (ISTRUE(ptInfo->m_byIsConnected)) ? emConnStateConnected : emConnStateDisconnected;
	ptPart->m_emEPType      = (ptInfo->m_byMtType != MT_TYPE_SMCU && ptInfo->m_byMtType != MT_TYPE_MMCU) ? emEPTypeTerminal : emEPTypeMCU;
	ptPart->m_emVideo2In    = (PayloadType)CMtAdpUtils::PayloadTypeIn2Out(ptInfo->m_byVideo2In); 
	ptPart->m_emVideo2Out   = (PayloadType)CMtAdpUtils::PayloadTypeIn2Out(ptInfo->m_byVideo2Out);
	ptPart->m_emVideoIn     = (PayloadType)CMtAdpUtils::PayloadTypeIn2Out(ptInfo->m_byVideoIn); 
	ptPart->m_emVideoOut    = (PayloadType)CMtAdpUtils::PayloadTypeIn2Out(ptInfo->m_byVideoOut);
	s8 achMtAlias[STR_LEN_UTF8]; //pengguofeng　多国语言
	memset(achMtAlias, 0, sizeof(achMtAlias));
	BOOL32 bNeedTrans = TransEncoding(ptInfo->m_szMtName, achMtAlias, sizeof(achMtAlias));
	if ( bNeedTrans == TRUE )
	{
		strncpy(ptPart->m_aszDialStr, achMtAlias, sizeof(ptPart->m_aszDialStr));
	}
	else
	{
		strncpy(ptPart->m_aszDialStr, ptInfo->m_szMtName, sizeof(ptPart->m_aszDialStr));
	}
	ptPart->m_aszDialStr[sizeof(ptPart->m_aszDialStr)-1] = 0;
	ptPart->m_byCasLevel = ptInfo->m_byCasLevel;
	memcpy( &ptPart->m_abyMtIdentify[0], &ptInfo->m_abyMtIdentify[0], sizeof( ptPart->m_abyMtIdentify ) );
	
	memset( ptPart->m_aszEPDesc,0,sizeof(ptPart->m_aszEPDesc) );
	if( MT_MANU_KDC == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "kdcmt", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_KDCMCU == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "kdcmcu", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_NETMEETING == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "netmeetingmt", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_POLYCOM == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "polycommt", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_HUAWEI == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "huaweimt", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_TAIDE == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "taidemt", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_SONY == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "sonymt", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_VCON == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "vconmt", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_RADVISION == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "radvisionmcu", sizeof(ptPart->m_aszEPDesc));
	}
	if( MT_MANU_PENTEVIEW == ptInfo->m_byManuId )
	{
		strncpy(ptPart->m_aszEPDesc, "pentviewmt", sizeof(ptPart->m_aszEPDesc));
	}
	ptPart->m_aszEPDesc[sizeof(ptPart->m_aszEPDesc)-1] = 0;
    
	// MtName也需要转换 [pengguofeng 5/24/2013]
	if ( bNeedTrans == TRUE )
	{
		strncpy( ptPart->m_aszPartName, achMtAlias, sizeof(ptPart->m_aszPartName) );
	}
	else
	{
		strncpy( ptPart->m_aszPartName, ptInfo->m_szMtName, sizeof(ptPart->m_aszPartName) );
	}
    ptPart->m_aszPartName[sizeof(ptPart->m_aszPartName)-1] = 0;

    //若对端为keda mcu填写本端终端协议类型字段
    if( MT_MANU_KDC == ptInfo->m_byManuId && MT_MANU_KDCMCU == m_byVendorId )
    {

        sprintf(ptPart->m_aszPartName, "%s%d", ptPart->m_aszPartName, ptInfo->m_byProtocolType);
    }

	//若对端为keda mcu填写本端终端协议类型字段(终端别名扩容代码，这里注掉留作参考)
// 	if( MT_MANU_KDC == ptInfo->m_byManuId && MT_MANU_KDCMCU == m_byVendorId )
//     {
//         if(strlen(ptPart->m_aszPartName) >= sizeof(ptPart->m_aszEPDesc)-1)
//         {			
// 			ptPart->m_aszPartName[sizeof(ptPart->m_aszPartName)-2] = 0;   //clear one char for protocol position
//         }
// 		sprintf(ptPart->m_aszPartName, "%s%d", ptPart->m_aszPartName, ptInfo->m_byProtocolType);
//     }
	
	memset(&(ptPart->m_tAVStatis), 0, sizeof(ptPart->m_tAVStatis));

	ptPart->m_tPartVideoInfo.m_nViewCount        = ptInfo->m_tPartVideoInfo.m_nViewCount;
	ptPart->m_tPartVideoInfo.m_nOutputViewID     = ptInfo->m_tPartVideoInfo.m_nOutputLID;
	ptPart->m_tPartVideoInfo.m_nOutVideoSchemeID = ptInfo->m_tPartVideoInfo.m_nOutVideoSchemeID;

	for( s32 nLoop = 0; nLoop < ptPart->m_tPartVideoInfo.m_nViewCount && nLoop < MAX_VIEWS_PER_CONF; nLoop ++ )
	{
		ptPart->m_tPartVideoInfo.m_anViewPos[nLoop].m_nViewID         = ptInfo->m_tPartVideoInfo.m_atViewPos[nLoop].m_nViewID; 
		ptPart->m_tPartVideoInfo.m_anViewPos[nLoop].m_bySubframeIndex = ptInfo->m_tPartVideoInfo.m_atViewPos[nLoop].m_bySubframeIndex;
	}
	ptPart->m_tStartTime.year   = 2005;
	ptPart->m_tStartTime.month  = 1;
	ptPart->m_tStartTime.day    = 1;
	ptPart->m_tStartTime.hour   = 0;
	ptPart->m_tStartTime.minute = 0;
	ptPart->m_tStartTime.second = 0;
	if( ptInfo->m_tMt.GetMtId() == m_byMtId )
	{
		ptPart->m_dwPID    = REMOTE_MCU_PARTID; //对方在本MCU上的条目
		ptPart->m_bLocal   = FALSE;
		ptPart->m_emEPType = emEPTypeCascConf;
		ptPart->m_bMaster  = !m_bMaster;
		if(ptPart->m_bMaster) ptPart->m_bInvited = FALSE;
	}

	return;
}
*/
/*=============================================================================
  函 数 名： Part2MtInfo
  功    能： 
  算法实现： 
  全局变量： 
  参    数： TPart *ptPart
             TMcuMcuMtInfo *ptInfo
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
  2013/5/24     2.0         彭国锋　　　　　　　　　发给本MCU前，将EpDesc/PartName/DialString进行转码
=============================================================================*/
void CMtAdpInst::Part2MtInfo(TPart *ptPart, TMcuMcuMtInfo *ptInfo)
{
	ptInfo->m_byAudioIn  = CMtAdpUtils::PayloadTypeOut2In( (u8)(ptPart->m_emAudioIn) );
	ptInfo->m_byAudioOut = CMtAdpUtils::PayloadTypeOut2In( (u8)(ptPart->m_emAudioOut) );
	ptInfo->m_byIsAudioMuteIn  = GETBBYTE( ptPart->m_bAudioMuteIn );
	ptInfo->m_byIsAudioMuteOut = GETBBYTE( ptPart->m_bAudioMuteOut );
	ptInfo->m_byIsConnected    = GETBBYTE( ptPart->m_emConnState == emConnStateConnected );
	ptInfo->m_byIsDataMeeting  = GETBBYTE( ptPart->m_bDataMeeting );
	ptInfo->m_byIsFECCEnable   = GETBBYTE( ptPart->m_bFECCEnable );
	ptInfo->m_byIsVideoMuteIn  = GETBBYTE( ptPart->m_bVideoMuteIn );
	ptInfo->m_byIsVideoMuteOut = GETBBYTE( ptPart->m_bVideoMuteOut );
	ptInfo->m_byMtType = ( ptPart->m_emEPType == emEPTypeMCU) ? MT_TYPE_SMCU:MT_TYPE_MT;
	ptInfo->m_byVideo2In  = CMtAdpUtils::PayloadTypeOut2In( (u8)(ptPart->m_emVideo2In) );
	ptInfo->m_byVideo2Out = CMtAdpUtils::PayloadTypeOut2In( (u8)(ptPart->m_emVideo2Out) );
	ptInfo->m_byVideoIn   = CMtAdpUtils::PayloadTypeOut2In( (u8)(ptPart->m_emVideoIn) );
	ptInfo->m_byVideoOut  = CMtAdpUtils::PayloadTypeOut2In( (u8)(ptPart->m_emVideoOut) );
	ptInfo->m_dwMtIp =  ptPart->m_dwIP;
	ptInfo->m_byCasLevel = ptPart->m_byCasLevel;
	memcpy( &ptInfo->m_abyMtIdentify[0],&ptPart->m_abyMtIdentify[0],sizeof(ptInfo->m_abyMtIdentify) );
	
	// 多国语言：EP描述 [pengguofeng 4/8/2013]
	s8 achMtDesc[VALIDLEN_ALIAS_UTF8];
	memset(achMtDesc, 0, sizeof(achMtDesc));
	BOOL32 bNeedTrans = TransEncoding(ptPart->m_aszEPDesc, achMtDesc, sizeof(achMtDesc), FALSE);
	if ( bNeedTrans == TRUE)
	{
		strncpy( ptInfo->m_szMtDesc, achMtDesc, sizeof( ptInfo->m_szMtDesc) );
	}
	else
	{
		strncpy( ptInfo->m_szMtDesc, ptPart->m_aszEPDesc, sizeof( ptInfo->m_szMtDesc) );
	}
	ptInfo->m_szMtDesc[sizeof(ptInfo->m_szMtDesc)-1] = 0;

	if( 0 == strcmp( ptInfo->m_szMtDesc, "kdcmt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_KDC;
        ptInfo->m_byProtocolType = ptPart->m_aszPartName[strlen(ptPart->m_aszPartName)-1]-'0'; // 取协议类型
        ptPart->m_aszPartName[strlen(ptPart->m_aszPartName)-1] = 0;
	}

	if( 0 == strcmp( ptInfo->m_szMtDesc, "kdcmcu" ) )
	{
		ptInfo->m_byManuId = MT_MANU_KDCMCU;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "netmeetingmt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_NETMEETING;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "polycommt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_POLYCOM;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "huaweimt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_HUAWEI;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "taidemt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_TAIDE;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "sonymt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_SONY;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "vconmt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_VCON;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "radvisionmcu" ) )
	{
		ptInfo->m_byManuId = MT_MANU_RADVISION;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "pentviewmt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_PENTEVIEW;
	}

	// 多国语言 [pengguofeng 4/8/2013]
	memset(achMtDesc, 0, sizeof(achMtDesc));
	bNeedTrans = TransEncoding(ptPart->m_aszPartName, achMtDesc, sizeof(achMtDesc), FALSE);
	MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[Part2MtInfo]Mt. %x name:(%s ) VALIDLEN_ALIAS_UTF8:%d OnLine:%d\n",
		ptPart->m_dwPID, ptPart->m_aszPartName, VALIDLEN_ALIAS_UTF8, ptPart->m_emConnState == emConnStateConnected);
	if ( bNeedTrans == TRUE)
	{
		strncpy( ptInfo->m_szMtName, achMtDesc, sizeof(ptInfo->m_szMtName) );
	}
	else
	{
		strncpy( ptInfo->m_szMtName, ptPart->m_aszPartName, sizeof(ptInfo->m_szMtName) );
	}
	ptInfo->m_szMtName[sizeof(ptInfo->m_szMtName) - 1] = 0;

	if( strncmp( ptInfo->m_szMtName, "null", strlen("null") ) == 0 )
	{
		memset(achMtDesc, 0, sizeof(achMtDesc));
		bNeedTrans = TransEncoding(ptPart->m_aszDialStr, achMtDesc, sizeof(achMtDesc), FALSE);
		if ( bNeedTrans == TRUE)
		{
			strncpy(ptInfo->m_szMtName, achMtDesc, sizeof(ptInfo->m_szMtName));
		}
		else
		{
			strncpy(ptInfo->m_szMtName, ptPart->m_aszDialStr, sizeof(ptInfo->m_szMtName));
		}
		ptInfo->m_szMtName[sizeof(ptInfo->m_szMtName) - 1] = 0;
	}
	
	ptInfo->m_tMt = GetMtFromPartId(ptPart->m_dwPID, ptPart->m_bLocal);

	ptInfo->m_tPartVideoInfo.m_nOutputLID        = ptPart->m_tPartVideoInfo.m_nOutputViewID;
	ptInfo->m_tPartVideoInfo.m_nOutVideoSchemeID = ptPart->m_tPartVideoInfo.m_nOutVideoSchemeID;
	ptInfo->m_tPartVideoInfo.m_nViewCount        = ptPart->m_tPartVideoInfo.m_nViewCount;
	
	for(s32 nIndex=0; nIndex<ptInfo->m_tPartVideoInfo.m_nViewCount; nIndex++)
	{
		ptInfo->m_tPartVideoInfo.m_atViewPos[nIndex].m_nViewID         = ptPart->m_tPartVideoInfo.m_anViewPos[nIndex].m_nViewID;
		ptInfo->m_tPartVideoInfo.m_atViewPos[nIndex].m_bySubframeIndex = ptPart->m_tPartVideoInfo.m_anViewPos[nIndex].m_bySubframeIndex;
	}

	return;
}

// 带U的为扩容版本 [pengguofeng 5/31/2013]
/*void CMtAdpInst::Part2MtInfo(TPartU *ptPart, TMcuMcuMtInfo *ptInfo)
{
	ptInfo->m_byAudioIn  = CMtAdpUtils::PayloadTypeOut2In( (u8)(ptPart->m_emAudioIn) );
	ptInfo->m_byAudioOut = CMtAdpUtils::PayloadTypeOut2In( (u8)(ptPart->m_emAudioOut) );
	ptInfo->m_byIsAudioMuteIn  = GETBBYTE( ptPart->m_bAudioMuteIn );
	ptInfo->m_byIsAudioMuteOut = GETBBYTE( ptPart->m_bAudioMuteOut );
	ptInfo->m_byIsConnected    = GETBBYTE( ptPart->m_emConnState == emConnStateConnected );
	ptInfo->m_byIsDataMeeting  = GETBBYTE( ptPart->m_bDataMeeting );
	ptInfo->m_byIsFECCEnable   = GETBBYTE( ptPart->m_bFECCEnable );
	ptInfo->m_byIsVideoMuteIn  = GETBBYTE( ptPart->m_bVideoMuteIn );
	ptInfo->m_byIsVideoMuteOut = GETBBYTE( ptPart->m_bVideoMuteOut );
	ptInfo->m_byMtType = ( ptPart->m_emEPType == emEPTypeMCU) ? MT_TYPE_SMCU:MT_TYPE_MT;
	ptInfo->m_byVideo2In  = CMtAdpUtils::PayloadTypeOut2In( (u8)(ptPart->m_emVideo2In) );
	ptInfo->m_byVideo2Out = CMtAdpUtils::PayloadTypeOut2In( (u8)(ptPart->m_emVideo2Out) );
	ptInfo->m_byVideoIn   = CMtAdpUtils::PayloadTypeOut2In( (u8)(ptPart->m_emVideoIn) );
	ptInfo->m_byVideoOut  = CMtAdpUtils::PayloadTypeOut2In( (u8)(ptPart->m_emVideoOut) );
	ptInfo->m_dwMtIp =  ptPart->m_dwIP;
	ptInfo->m_byCasLevel = ptPart->m_byCasLevel;
	memcpy( &ptInfo->m_abyMtIdentify[0],&ptPart->m_abyMtIdentify[0],sizeof(ptInfo->m_abyMtIdentify) );
	
	// 多国语言：EP描述 [pengguofeng 4/8/2013]
	s8 achMtDesc[VALIDLEN_ALIAS];
	memset(achMtDesc, 0, sizeof(achMtDesc));
	BOOL32 bNeedTrans = TransEncoding(ptPart->m_aszEPDesc, achMtDesc, sizeof(achMtDesc), FALSE);
	if ( bNeedTrans == TRUE)
	{
		strncpy( ptInfo->m_szMtDesc, achMtDesc, sizeof( ptInfo->m_szMtDesc) );
	}
	else
	{
		strncpy( ptInfo->m_szMtDesc, ptPart->m_aszEPDesc, sizeof( ptInfo->m_szMtDesc) );
	}
	ptInfo->m_szMtDesc[sizeof(ptInfo->m_szMtDesc)-1] = 0;

	if( 0 == strcmp( ptInfo->m_szMtDesc, "kdcmt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_KDC;
        ptInfo->m_byProtocolType = ptPart->m_aszPartName[strlen(ptPart->m_aszPartName)-1]-'0'; // 取协议类型
        ptPart->m_aszPartName[strlen(ptPart->m_aszPartName)-1] = 0;
	}

	if( 0 == strcmp( ptInfo->m_szMtDesc, "kdcmcu" ) )
	{
		ptInfo->m_byManuId = MT_MANU_KDCMCU;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "netmeetingmt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_NETMEETING;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "polycommt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_POLYCOM;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "huaweimt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_HUAWEI;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "taidemt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_TAIDE;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "sonymt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_SONY;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "vconmt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_VCON;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "radvisionmcu" ) )
	{
		ptInfo->m_byManuId = MT_MANU_RADVISION;
	}
	if( 0 == strcmp( ptInfo->m_szMtDesc, "pentviewmt" ) )
	{
		ptInfo->m_byManuId = MT_MANU_PENTEVIEW;
	}

	// 多国语言 [pengguofeng 4/8/2013]
	memset(achMtDesc, 0, sizeof(achMtDesc));
	bNeedTrans = TransEncoding(ptPart->m_aszPartName, achMtDesc, sizeof(achMtDesc), FALSE);
	if ( bNeedTrans == TRUE)
	{
		strncpy( ptInfo->m_szMtName, achMtDesc, sizeof(ptInfo->m_szMtName) );
	}
	else
	{
		strncpy( ptInfo->m_szMtName, ptPart->m_aszPartName, sizeof(ptInfo->m_szMtName) );
	}
	ptInfo->m_szMtName[sizeof(ptInfo->m_szMtName) - 1] = 0;

	if( strncmp( ptInfo->m_szMtName, "null", strlen("null") ) == 0 )
	{
		memset(achMtDesc, 0, sizeof(achMtDesc));
		bNeedTrans = TransEncoding(ptPart->m_aszDialStr, achMtDesc, sizeof(achMtDesc), FALSE);
		if ( bNeedTrans == TRUE)
		{
			strncpy(ptInfo->m_szMtName, achMtDesc, sizeof(ptInfo->m_szMtName));
		}
		else
		{
			strncpy(ptInfo->m_szMtName, ptPart->m_aszDialStr, sizeof(ptInfo->m_szMtName));
		}
		ptInfo->m_szMtName[sizeof(ptInfo->m_szMtName) - 1] = 0;
	}
	
	ptInfo->m_tMt = GetMtFromPartId(ptPart->m_dwPID, ptPart->m_bLocal);

	ptInfo->m_tPartVideoInfo.m_nOutputLID        = ptPart->m_tPartVideoInfo.m_nOutputViewID;
	ptInfo->m_tPartVideoInfo.m_nOutVideoSchemeID = ptPart->m_tPartVideoInfo.m_nOutVideoSchemeID;
	ptInfo->m_tPartVideoInfo.m_nViewCount        = ptPart->m_tPartVideoInfo.m_nViewCount;
	
	for(s32 nIndex=0; nIndex<ptInfo->m_tPartVideoInfo.m_nViewCount; nIndex++)
	{
		ptInfo->m_tPartVideoInfo.m_atViewPos[nIndex].m_nViewID         = ptPart->m_tPartVideoInfo.m_anViewPos[nIndex].m_nViewID;
		ptInfo->m_tPartVideoInfo.m_atViewPos[nIndex].m_bySubframeIndex = ptPart->m_tPartVideoInfo.m_anViewPos[nIndex].m_bySubframeIndex;
	}

	return;
}
*/
/*=============================================================================
  函 数 名： VideoInfoIn2Out
  功    能： 
  算法实现： 
  全局变量： 
  参    数： TCConfViewInfo*  ptInInfo
             TConfVideoInfo* ptOutInfo
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::VideoInfoIn2Out(TCConfViewInfo* ptInInfo, TConfVideoInfo* ptOutInfo)
{
	ptOutInfo->m_emReturnVal        = emReturnValue_Ok;
	ptOutInfo->m_byDefaultViewIndex = ptInInfo->m_byDefViewIndex;
	ptOutInfo->m_byConfViewCount    = (u8)( min( ptInInfo->m_byViewCount, MAX_VIEWS_PER_CONF ) );

	// 以下版本仅支持两个视图
	LPCSTR pszVersionInfo = GetPeerVersionID(m_hsCall);
	if (strcmp(pszVersionInfo, "3.6") == 0 ||
		strcmp(pszVersionInfo, "4.0") == 0 ||
		strcmp(pszVersionInfo, "mcu4.0 R3") == 0 ||
		strcmp(pszVersionInfo, "5") == 0)
	{
		ptOutInfo->m_byConfViewCount = min(ptInInfo->m_byViewCount, 2);
	}

	TView *ptOView;
	TCViewInfo *ptIView;
	for( s32 nLoop = 0; nLoop < ptOutInfo->m_byConfViewCount; nLoop ++ )
	{
		ptOView = &(ptOutInfo->m_atViewInfo[nLoop]);
		ptIView = &(ptInInfo->m_atViewInfo[nLoop]);
		ptOView->m_nViewId = ptIView->m_nViewId;
        // guzh [2008/03/27]
		s32 emRes = CMtAdpUtils::ResIn2Out(ptIView->m_byRes);
        memcpy(&ptOView->m_emRes, &emRes, sizeof(s32));
		ptOView->m_byCurrentGeometryIndex = ptIView->m_byCurGeoIndex;
		ptOView->m_byGeometryCount        = ptIView->m_byGeoCount;
        const u8 byLen = min(MAXNUM_GEOINVIEW, MAX_GEOMETRYS_PER_VIEW);
		memcpy( ptOView->m_abyGeometryInfo, ptIView->m_abyGeoInfo, byLen); 
	

		ptOView->m_byContributePIDCount = (u8)( min( ptIView->m_byMtCount, MAX_SUBFRAMES_PER_GEOMETRY));   
        s32 nPicNum = 0;
		for( nPicNum = 0; nPicNum < ptOView->m_byContributePIDCount; nPicNum ++ )
		{
			ptOView->m_atContributePIDs[nPicNum].m_dwPID = GetPartIdFromMt(ptIView->m_atMts[nPicNum], &(ptOView->m_atContributePIDs[nPicNum].m_bLocal));
		}
		if( ptOView->m_byContributePIDCount == 0 )
		{
			ptOView->m_byContributePIDCount= 1;
			ptOView->m_atContributePIDs[0].m_dwPID = 0;
			ptOView->m_atContributePIDs[nPicNum].m_bLocal = TRUE;
		}

		ptOView->m_byVideoSchemeCount = (u8)(min(ptIView->m_byVSchemeCount, MAX_VIDEOSCHEMES_PER_VIEW));
		for(s32 nVideoSchNum = 0; nVideoSchNum < ptOView->m_byVideoSchemeCount; nVideoSchNum ++ )
		{
			VideoSchemeIn2Out( &(ptOView->m_atVideoSchemeList[nVideoSchNum]),  &(ptIView->m_atVSchemes[nVideoSchNum]));
		}

        // guzh [7/19/2007] 轮询回传切换参数
       ptOView->m_bAutoSwitchStatus = ISTRUE(ptIView->m_byAutoSwitchStatus);
       ptOView->m_nAutoSwitchTime = ptIView->m_nAutoSwitchTime;
	}

	return;
}

/*=============================================================================
  函 数 名： VideoInfoOut2In
  功    能： 
  算法实现： 
  全局变量： 
  参    数： TCConfViewInfo*  ptInInfo
             TConfVideoInfo* ptOutInfo
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::VideoInfoOut2In(TCConfViewInfo*  ptInInfo, TConfVideoInfo* ptOutInfo)
{
	ptInInfo->m_byDefViewIndex = ptOutInfo->m_byDefaultViewIndex;
	ptInInfo->m_byViewCount    = min(ptOutInfo->m_byConfViewCount, MAXNUM_VIEWINCONF); 

	TView *ptOView;
	TCViewInfo *ptIView;

	for(s32 nLoop = 0; nLoop < ptInInfo->m_byViewCount; nLoop ++ )
	{
		ptOView = &(ptOutInfo->m_atViewInfo[nLoop]);
		ptIView = &(ptInInfo->m_atViewInfo[nLoop]);

		ptIView->m_nViewId = ptOView->m_nViewId;
		ptIView->m_byRes   = CMtAdpUtils::ResOut2In((s32)ptOView->m_emRes);
		ptIView->m_byCurGeoIndex = ptOView->m_byCurrentGeometryIndex;
		ptIView->m_byGeoCount    = ptOView->m_byGeometryCount;
        const u8 byLen = min(MAXNUM_GEOINVIEW, MAX_GEOMETRYS_PER_VIEW);
		memcpy( ptIView->m_abyGeoInfo, ptOView->m_abyGeometryInfo, byLen); 
		ptIView->m_byMtCount = (u8)(min(ptOView->m_byContributePIDCount, MAX_SUBFRAMES_PER_GEOMETRY)); 
		
		for( s32 nMtNum = 0; nMtNum < ptIView->m_byMtCount; nMtNum ++ )
		{
			ptIView->m_atMts[nMtNum] = GetMtFromPartId( ptOView->m_atContributePIDs[nMtNum].m_dwPID, ptOView->m_atContributePIDs[nMtNum].m_bLocal);
		}
		ptIView->m_byVSchemeCount = (u8)(min(ptOView->m_byVideoSchemeCount, MAX_VIDEOSCHEMES_PER_VIEW));
		
		for(s32 nSchemeNum = 0; nSchemeNum < ptIView->m_byVSchemeCount; nSchemeNum ++ )
		{
			VideoSchemeOut2In( &(ptOView->m_atVideoSchemeList[nSchemeNum]),  &(ptIView->m_atVSchemes[nSchemeNum]));
		}

        // guzh [7/19/2007] 轮询回传切换参数
       ptIView->m_byAutoSwitchStatus = ptOView->m_bAutoSwitchStatus ? 1 : 0;
       ptIView->m_nAutoSwitchTime = ptOView->m_nAutoSwitchTime;
	}
	return;
}

/*=============================================================================
  函 数 名： AudioInfoIn2Out
  功    能： 
  算法实现： 
  全局变量： 
  参    数： TCConfAudioInfo*  ptInInfo
             TConfAudioInfo* ptOutInfo
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
  2010/08/24	4.6.2		周晶晶					修改(考虑到上级没发言人没混音时也要设置一个特殊值到下级,下级依此判断,补建到老发言人的音频交换)
=============================================================================*/
void CMtAdpInst::AudioInfoIn2Out( TCConfAudioInfo*  ptInInfo, TConfAudioInfo* ptOutInfo )
{
	ptOutInfo->m_emReturnVal = emReturnValue_Ok;
	ptOutInfo->m_byDefaultMixerIndex = ptInInfo->m_byDefMixerIndex;
	ptOutInfo->m_byMixerCount = (u8)(min(ptInInfo->m_byMixerCount, MAX_MIXERS_PER_CONF));

	if( 0 == ptOutInfo->m_byMixerCount ||
		ptInInfo->m_tMixerList[0].m_tSpeaker.GetMcuId() != m_byMtId 
		)
	{
		ptInInfo->m_tMixerList[0].m_tSpeaker.SetMcuId( LOCAL_MCUID );
		ptInInfo->m_tMixerList[0].m_tSpeaker.SetMtId( m_byMtId );
		++ptOutInfo->m_byMixerCount;
	}
	
	for( s32 nLoop = 0; nLoop < ptOutInfo->m_byMixerCount; nLoop ++ )
	{
		ptOutInfo->m_tMixerList[nLoop].m_nMixerID     = ptInInfo->m_tMixerList[nLoop].m_nMixerID;
		ptOutInfo->m_tMixerList[nLoop].m_dwSpeakerPID = GetPartIdFromMt(ptInInfo->m_tMixerList[nLoop].m_tSpeaker, &(ptOutInfo->m_tMixerList[nLoop].m_bLocal));
	}

	return;
}

/*=============================================================================
  函 数 名： AudioInfoOut2In
  功    能： 
  算法实现： 
  全局变量： 
  参    数： TCConfAudioInfo*  ptInInfo
             TConfAudioInfo* ptOutInfo
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::AudioInfoOut2In( TCConfAudioInfo*  ptInInfo, TConfAudioInfo* ptOutInfo )
{
	ptInInfo->m_byDefMixerIndex = ptOutInfo->m_byDefaultMixerIndex;
	ptInInfo->m_byMixerCount = (u8)(min(ptOutInfo->m_byMixerCount, MAX_MIXERS_PER_CONF));

	for( s32 nLoop = 0; nLoop < ptInInfo->m_byMixerCount; nLoop ++ )
	{
		ptInInfo->m_tMixerList[nLoop].m_nMixerID = ptOutInfo->m_tMixerList[nLoop].m_nMixerID;
		ptInInfo->m_tMixerList[nLoop].m_tSpeaker = GetMtFromPartId(ptOutInfo->m_tMixerList[nLoop].m_dwSpeakerPID, ptOutInfo->m_tMixerList[nLoop].m_bLocal);
	}

	return;
}

/*=============================================================================
  函 数 名： GetMtFromPartId
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u32 dwPartId
             BOOL32 bLocal
  返 回 值： TMt 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
TMt CMtAdpInst::GetMtFromPartId(u32 dwPartId, BOOL32 bLocal)
{
	TMt tMt;
	tMt.SetConfIdx(m_byConfIdx);
	if(REMOTE_MCU_PARTID == dwPartId) //自己
	{
		if(bLocal)
		{
			tMt.SetMcuId(m_byMtId);
			tMt.SetMtId(0);
		}
		else
		{
		   tMt.SetMcuId(LOCAL_MCUID);
		   tMt.SetMtId(m_byMtId);
		}
		return tMt;
	}

	if(bLocal)
	{
		tMt.SetMcuId((u8)GETAPP(dwPartId));
		tMt.SetMtId((u8)GETINS(dwPartId));
		return tMt;
	}

	tMt.SetMcuId(m_byMtId);
	for(s32 nLoop=0; nLoop<MAXNUM_CONF_MT; nLoop++)
	{
		if(m_atPartIdTable[nLoop].dwPartId == dwPartId)
		{
			tMt.SetMtId(m_atPartIdTable[nLoop].byMtId);
			return tMt;
		}
	}
	
	tMt.SetNull();
	if(AddPartIdTable(dwPartId))//加入列表
	{
		return GetMtFromPartId(dwPartId, FALSE);		
	}
	
	return tMt;
}

/*=============================================================================
  函 数 名： DelPartIdFromTable
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u32 dwPartId
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::DelPartIdFromTable(u32 dwPartId )
{
	for( s32 nLoop = 0; nLoop < MAXNUM_CONF_MT; nLoop ++ )
	{
		if(m_atPartIdTable[nLoop].dwPartId == dwPartId)
		{
			m_atPartIdTable[nLoop].byMtId   = 0;
			m_atPartIdTable[nLoop].dwPartId = 0;
			break;
		}
	}

	return;
}

/*=============================================================================
  函 数 名： GetPartIdFromMt
  功    能： 
  算法实现： 
  全局变量： 
  参    数： TMt tMt
             BOOL32 *pbLocal
  返 回 值： u32  
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
u32  CMtAdpInst::GetPartIdFromMt(TMt tMt, BOOL32 *pbLocal)
{
	if(tMt.GetMcuId() == LOCAL_MCUID)
	{
		if(pbLocal != NULL)
		{
			*pbLocal = TRUE;
		}

		if(tMt.GetMtId() == m_byMtId)
		{
            if(pbLocal != NULL)
            {
                *pbLocal = FALSE;
		    }			
			return REMOTE_MCU_PARTID; //对方在本MCU上的条目;
		}
		else
		{
			return MAKEIID(tMt.GetMcuId(), tMt.GetMtId());//0x961aca4c;//
		}
	}
	
	if(tMt.GetMcuId() != m_byMtId)
	{
//		ASSERT("invalid TMt!");
        ASSERT(tMt.GetMcuId() != m_byMtId);
		return 0;
	}

	if(pbLocal != NULL)
	{
		*pbLocal = FALSE;
	}

	if(tMt.GetMtId() == 0)
	{
		if(pbLocal != NULL)
		{
			*pbLocal = TRUE;
		}
		return REMOTE_MCU_PARTID;
	}

	for( s32 nLoop = 0; nLoop < MAXNUM_CONF_MT; nLoop ++ )
	{
		if( m_atPartIdTable[nLoop].byMtId == tMt.GetMtId() )
		{
			return m_atPartIdTable[nLoop].dwPartId;
		}
	}
    
//	ASSERT("invalid TMt!");
    printf("invalid TMt!\n");
	return 0;
}

/*=============================================================================
  函 数 名： VideoSchemeIn2Out
  功    能： 
  算法实现： 
  全局变量： 
  参    数： TOutPutVideoSchemeInfo* ptOInfo
             TCSchemeInfo* ptIInfo
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::VideoSchemeIn2Out( TOutPutVideoSchemeInfo* ptOInfo, TCSchemeInfo* ptIInfo )
{
	ptOInfo->m_bCanUpdateRate = ptIInfo->m_bCanUpdateRate;
	ptOInfo->m_dwMaxRate  = ptIInfo->m_dwMaxRate;
	ptOInfo->m_dwMinRate  = ptIInfo->m_dwMinRate;
	ptOInfo->m_nFrameRate = ptIInfo->m_nFrameRate;
	ptOInfo->m_nOutputVideoSchemeID = ptIInfo->m_nVideoSchemeID;
	ptOInfo->m_emPayLoad = (PayloadType)CMtAdpUtils::PayloadTypeIn2Out(ptIInfo->m_byMediaType);

	return;
}

/*=============================================================================
  函 数 名： VideoSchemeOut2In
  功    能： 
  算法实现： 
  全局变量： 
  参    数： TOutPutVideoSchemeInfo* ptOInfo
             TCSchemeInfo* ptIInfo
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::VideoSchemeOut2In(TOutPutVideoSchemeInfo* ptOInfo, TCSchemeInfo* ptIInfo)
{
	ptIInfo->m_bCanUpdateRate = ptOInfo->m_bCanUpdateRate;
	ptIInfo->m_dwMaxRate  = ptOInfo->m_dwMaxRate;
	ptIInfo->m_dwMinRate  = ptOInfo->m_dwMinRate;
	ptIInfo->m_nFrameRate = ptOInfo->m_nFrameRate;
	ptIInfo->m_nVideoSchemeID = ptOInfo->m_nOutputVideoSchemeID;
    ptIInfo->m_byMediaType = CMtAdpUtils::PayloadTypeOut2In((u8)(ptOInfo->m_emPayLoad));

	return;
}

/*=============================================================================
  函 数 名： AddPartIdTable
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u32 dwPartId
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
BOOL32 CMtAdpInst::AddPartIdTable( u32 dwPartId )
{
	s32 nIdle = MAXNUM_CONF_MT;
	BOOL32 bFind = FALSE;
	for( s32 nLoop = 0; nLoop < MAXNUM_CONF_MT; nLoop ++ )
	{
		if( nIdle == MAXNUM_CONF_MT && m_atPartIdTable[nLoop].byMtId == 0 )
		{
			nIdle = nLoop;			
		}
		if( m_atPartIdTable[nLoop].dwPartId == dwPartId )
		{
			bFind = TRUE;
			break;
		}
	}
	if(bFind)
	{
		return TRUE; 
	}
	if(nIdle != MAXNUM_CONF_MT)
	{
		m_atPartIdTable[nIdle].byMtId   = nIdle + 1;
		m_atPartIdTable[nIdle].dwPartId = dwPartId;
		return TRUE;
	}

	return FALSE;
}

/*=============================================================================
  函 数 名： PartIdUpdate
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u32 dwOldPartId
             u32 dwNewPartId
             BOOL32 bLocal
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
void CMtAdpInst::PartIdUpdate( u32 dwOldPartId, u32 dwNewPartId, BOOL32 bLocal )
{
	if(bLocal)
	{
		return;
	}

	for( s32 nLoop = 0; nLoop < MAXNUM_CONF_MT; nLoop ++ )
	{
		if(m_atPartIdTable[nLoop].dwPartId == dwOldPartId)
		{
			m_atPartIdTable[nLoop].dwPartId = dwNewPartId;
			break;
		}
	}

	return;
}

/*=============================================================================
  函 数 名： GetPeerMtVer
  功    能： 获取终端的版本
  算法实现： 1、3.6终端标识ID的字符串的最后两个字符为"36"
             2、3.6之后，4.0R5以前的终端 标识符比较乱, 行业有"4.0"打尾，基本上认为是R3以前，也有“4.4.1 ...”打头,伴随T2规范出现；
                                                       企业有"1.1.1.0.2"等
             3、4.0R5开始，行业标识为"4.5....",企业标识为"1.2...."。
  全局变量： 
  参    数： HCALL hscall, 到来的呼叫句柄
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/11/10    4.0         张宝卿                创建
=============================================================================*/
emMtVer CMtAdpInst::GetPeerMtVer(HCALL hsCall)
{
    //zbq[12/07/2007] 非科达终端给最新版本号
    if ( m_byVendorId != MT_MANU_KDC )
    {
		return emMtVer40R6;
        //return emMtVer40R5;
    }

    //3.6标识
    s32 nStep = 0;
	LPCSTR lpszVerIdInFact = GetPeerVersionID( hsCall);
    MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[GetPeerMtVer] version: %s\n", lpszVerIdInFact);

    LPCSTR lpszVer36Id = "36";
	nStep = strlen(lpszVerIdInFact) - strlen(lpszVer36Id);
    if ( 0 == strncmp( lpszVer36Id, lpszVerIdInFact + nStep, strlen(lpszVer36Id) ) )
    {
        return emMtVer36;
    }

    //4.0R3以前的标识也统一到R4版本里
    LPCSTR lpszVer40R3Id = "4.0";
	LPCSTR lpszVer40R3IdNew = "7";
	LPCSTR lpszVer40R3IdNewR3full = "5";
    nStep = strlen(lpszVerIdInFact) - strlen(lpszVer40R3Id);
    if ( 0 == strncmp( lpszVer40R3Id, lpszVerIdInFact + nStep, strlen(lpszVer40R3Id) ) 
		 || 0 == strncmp( lpszVer40R3IdNew, lpszVerIdInFact , strlen(lpszVer40R3IdNew ) )
		 || 0 == strncmp( lpszVer40R3IdNewR3full, lpszVerIdInFact , strlen(lpszVer40R3IdNewR3full ) )
		)
    {
        return emMtVer40R4;
    }

    //4.0R4标识
    LPCSTR lpszVer40R4 = "4.4";
    if ( 0 == strncmp(lpszVerIdInFact, lpszVer40R4, strlen(lpszVer40R4) ))
    {
        return emMtVer40R4;
    }

    //4.0R5标识
    LPCSTR lpszVer40R5 = "4.5";
    LPCSTR lpszVer40R5New = "8";
    if ( 0 == strncmp(lpszVerIdInFact, lpszVer40R5, strlen(lpszVer40R5) )
		|| 0 == strncmp(lpszVerIdInFact, lpszVer40R5New, strlen(lpszVer40R5)))
    {
        return emMtVer40R5;
    }

	// xliang [2/24/2009] V4R6
// 	LPCSTR lpszVer40R6 = "7";
// 	if ( 0 == strncmp(lpszVerIdInFact, lpszVer40R6, strlen(lpszVer40R6)) )
// 	{
// 		return emMtVer40R6;
// 	}

    //return emMtVer40R4;
	//return emMtVer40R5;
	return emMtVer40R6;
}

/*=============================================================================
  函 数 名： IsCallingSupported
  功    能： 终端接入模式是否支持
  算法实现： 
  全局变量： 
  参    数： HCALL hscall, 到来的呼叫句柄
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/05/14    4.0         张宝卿                创建
=============================================================================*/
BOOL32 CMtAdpInst::IsCallingSupported( HCALL hsCall )
{
    if ( !g_cMtAdpApp.m_bUseCallingMatch )
    {
        return TRUE;
    }
    // guzh [6/15/2007] PCMT 都支持 
    if ( IsPeerMtPcmt(hsCall) )
    {
        return TRUE;
    }

    if ( IsLocalMcu8000C() )
    {
        if ( !IS_MT || 
             m_byVendorId == MT_MANU_KDC || 
             IsPeerMtSerialTS(hsCall) )
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
		return TRUE;
    }

//    return FALSE;
}

/*=============================================================================
  函 数 名： IsPeerMtSerialTS
  功    能： 接入终端是否为 TS 系列的终端
  算法实现： 
  全局变量： 
  参    数： HCALL hscall, 到来的呼叫句柄
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/05/14    4.0         张宝卿                创建
=============================================================================*/
BOOL32 CMtAdpInst::IsPeerMtSerialTS( HCALL hsCall )
{
    BOOL32 bSerialTS = FALSE;

    if ( !IS_MT )
    {
        MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[IsPeerMtSerialTS] bSerialTS.%d due to not MT!\n", bSerialTS);
        return FALSE;
    }

    if ( m_byVendorId != MT_MANU_KDC )
    {
        MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[IsPeerMtSerialTS] bSerialTS.%d due to not KedaMT!\n", bSerialTS);
        return FALSE;
    }

    s8 *pszMtProductId = GetPeerProductID(hsCall);

    if ( NULL == pszMtProductId )
    {
        MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[IsPeerMtSerialTS] bSerialTS.%d due to pszMtProductId is NULL!\n", bSerialTS);
        return bSerialTS;
    }
    if ( 0 == strncmp( (s8*)pszMtProductId, "ts", strlen("ts") ) )
    {
        bSerialTS = TRUE;
    }
    // 现在V系列只有v5
    else if ( 0 == strncmp( pszMtProductId, "v", strlen("v") ) )
    {
        bSerialTS = TRUE;
    }
    MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[IsPeerMtSerialTS] bSerialTS.%d due to pszMtProductId.%s !\n", bSerialTS, pszMtProductId);

    return bSerialTS;
}

/*=============================================================================
  函 数 名： IsPeerMtPcmt
  功    能： 接入终端是否为 Pcmt
  算法实现： 
  全局变量： 
  参    数： HCALL hscall, 到来的呼叫句柄
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/06/15    4.0         顾振华                创建
=============================================================================*/
BOOL32 CMtAdpInst::IsPeerMtPcmt( HCALL hsCall )
{
    BOOL32 bPcmt = FALSE;

    if ( !IS_MT )
    {
        MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[IsPeerMtPcmt] bPcmt.%d due to not MT!\n", bPcmt);
        return FALSE;
    }

    if ( m_byVendorId != MT_MANU_KDC )
    {
        MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[IsPeerMtPcmt] bPcmt.%d due to not KedaMT!\n", bPcmt);
        return FALSE;
    }

    s8 *pszMtProductId = GetPeerProductID(hsCall);

    if ( NULL == pszMtProductId )
    {
        MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[IsPeerMtSerialTS] bPcmt.%d due to pszMtProductId is NULL!\n", bPcmt);
        return bPcmt;
    }
    if ( 0 == strncmp( pszMtProductId, "Kdvpcmt", strlen("Kdvpcmt") ) )
    {
        bPcmt = TRUE;
    }

    MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[IsPeerMtSerialTS] bPcmt.%d due to pszMtProductId.%s !\n", bPcmt, pszMtProductId);

    return bPcmt;
}

/*=============================================================================
  函 数 名： IsLocalMcu8000C
  功    能： 本MCU是否8000C
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/05/14    4.0         张宝卿                创建
=============================================================================*/
BOOL32 CMtAdpInst::IsLocalMcu8000C()
{
    u8 abyBuf[128] = {0};
    u8 *pbyProductId = &abyBuf[0];
    u8 byLen = g_cMtAdpApp.m_tH323Config.GetProductIdSize();
    g_cMtAdpApp.m_tH323Config.GetProductId(pbyProductId, byLen);

    u8 dw8000CLen =  strlen(PRODUCT_MCU_8000C);
    byLen = max( byLen, dw8000CLen/*strlen(PRODUCT_MCU_8000C)*/ );

    BOOL32 bMcu8000C = FALSE;
    if ( 0 == memcmp((s8*)pbyProductId, PRODUCT_MCU_8000C, byLen) )
    {
        bMcu8000C = TRUE;
    }
    MAPrint(LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[IsLocalMcu8000C] bMcu8000C.%d !\n", bMcu8000C);

    return bMcu8000C;
}

/*=============================================================================
  函 数 名： IsChanG7221CSupport
  功    能： 尝试打开的通道是否支持，目前用于判断对端是否支持G.722.1.C的最新版本
  算法实现： 
  全局变量： 
  参    数： HCALL hscall, 呼叫句柄
  返 回 值： BOOL
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/09/03    4.0         顾振华                创建
=============================================================================*/
BOOL CMtAdpInst::IsChanG7221CSupport( )
{
    
    s8* pszPeerPdtId = GetPeerProductID(m_hsCall);
    s8* pszPeerVerId = GetPeerVersionID(m_hsCall);

    if ( !IsPeerSupportNewG7221c(m_hsCall) )
    {
        MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "Not support G.7221.C Audio for MT vendor.%d %s %s\n",
            m_byVendorId, pszPeerPdtId, pszPeerVerId);
        return FALSE;
    }
    return TRUE;
}


/*=============================================================================
  函 数 名： GetCapVideoParam
  功    能： 根据协议栈参数及本地能力集，获取视频的分辨率、帧率等参数
  算法实现： 
  全局变量： 
  参    数： u8        byPayload
             TVideoCap &tVidCap 对于H.264 是TH264VideoCap           
             BOOL32    bDual：  是否双流
             u8 &byRes  [out]   返回的对端的分辨率
             u8 &byFps  [out]   返回的对端的帧率(对于非H.264是MPI)
  返 回 值： BOOL 是否能取得共同能力，如果返回FALSE说明该视频通道无法被接收
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/09/04    4.0         张宝卿                创建
  2008/02/20    4.5         顾振华                高清支持，以及高清标清对通限制
=============================================================================*/
BOOL CMtAdpInst::GetCapVideoParam(TVideoCap &tVidCap, BOOL32 bDual, u8 &byRemoteRes, u8 &byRemoteFps)
{
    byRemoteRes = VIDEO_FORMAT_CIF;
    byRemoteFps = 1;
    
    //非H.264 视频参数处理
    //[4/24/2012 zhushengze]优选分辨率大的
    for(u8 byLp = (u8)emResolutionEnd - 1; byLp > (u8)emResolutionBegin; byLp-- )
    {
        //  第一路: fixme: 暂时只支持对方填写 mpi=1。这里企业版本是比较正确的
		//zjj20130619 只认为1有问题，会导致30或25帧一下帧率的终端(外厂商)的无法正确开mcu的通道
        if( !bDual && 0 != tVidCap.m_abyResolution[byLp] )
        {
            byRemoteRes = CMtAdpUtils::ResOut2In(/*(emResolution)*/byLp);
			//zjj20130619 现在界面非264的帧率都统一成25帧了,25和30的标准上都是用29.97除的,
			//所以都是1,如果对端的能力都大于1,也就表示帧率都小于30或25帧,
			//为了在上层不导致过适配统一都认为是1,这样就没有问题了
			//因为版本已经封闭,双流先不改,在4.7.2修改
            byRemoteFps = 1;//tVidCap.m_abyResolution[byLp];
            return TRUE;
        }
        // 双流的帧率非空即认为能力支持 
		// [12/11/2009 xliang] FIXME: 若有几个能力帧率都非空，是否应找Res最大的
        else if ( bDual &&                
                0 != tVidCap.m_abyResolution[byLp] )
        {
            byRemoteRes = CMtAdpUtils::ResOut2In(/*(emResolution)*/byLp);
            //byRemoteFps = tVidCap.m_abyResolution[byLp];	
			if( m_byVendorId == MT_MANU_KDC || m_byVendorId == MT_MANU_KDCMCU )
			{
				byRemoteFps = CMtAdpUtils::FrameRateOut2In(tVidCap.m_abyResolution[byLp]);
				if( byRemoteRes == VIDEO_FORMAT_VGA || byRemoteRes == VIDEO_FORMAT_SVGA )
				{
					byRemoteFps = VIDEO_FPS_2997_6;
				}
			}
			else
			{
				byRemoteFps = tVidCap.m_abyResolution[byLp];
			}
            return TRUE;
        }
    }
    return FALSE;
}


BOOL CMtAdpInst::GetCapVideoParam(TH264VideoCap &tVidCap, BOOL32 bDual, u8 &byRemoteRes, u8 &byRemoteFps)
{
    byRemoteRes = VIDEO_FORMAT_CIF;
    byRemoteFps = 1; 
    
    //H.264 视频参数处理
    u8 byLocalRes;
    u8 byLocalFps;    
    if (!bDual)
    {
        //FIXME:目前双格式只支持一路H.264
        if ( MEDIA_TYPE_H264 == m_tLocalCapSupport.GetMainVideoType() )
        {
            byLocalRes = m_tLocalCapSupport.GetMainVideoResolution();
            byLocalFps = m_tLocalCapSupport.GetMainVidUsrDefFPS();
        }
        else if ( MEDIA_TYPE_H264 == m_tLocalCapSupport.GetSecVideoType() )
        {
            byLocalRes = m_tLocalCapSupport.GetSecVideoResolution();
            byLocalFps = m_tLocalCapSupport.GetSecVidUsrDefFPS();
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        // 双流
        byLocalRes = m_tLocalCapSupport.GetDStreamResolution();
        byLocalFps = m_tLocalCapSupport.GetDStreamUsrDefFPS();
    }
    if (VIDEO_FORMAT_AUTO == byLocalRes)
    {
        byLocalRes = VIDEO_FORMAT_CIF;
    }
    // 保护25-->30
    if (!bDual && VIDEO_FORMAT_CIF == byLocalRes && 25 == byLocalFps)
    {
        byLocalFps = 30;
    }
    s32 byStackLocalRes = CMtAdpUtils::ResIn2Out(byLocalRes);

    //构造协议栈能力，如果对方要求打开的比本端最大值小，就可以pass
    TH264VideoCap tStackLocalCap;
    tStackLocalCap.SetResolution(byStackLocalRes, byLocalFps);


	//TODO H264能力集比较是否要加入HP/BP 设置HP/BP属性[12/9/2011 chendaiwei]
	u8 byMSProfile  = (m_tLocalCapSupport.GetMainStreamProfileAttrb() == emHpAttrb ? (u8)TH264VideoCap::emProfileHigh | (u8)TH264VideoCap::emProfileBaseline : (u8)TH264VideoCap::emProfileBaseline); 
	u8 byDSProfile  = (m_tLocalCapSupport.GetDStreamCapSet().GetH264ProfileAttrb() == emHpAttrb ? (u8)TH264VideoCap::emProfileHigh | (u8)TH264VideoCap::emProfileBaseline : (u8)TH264VideoCap::emProfileBaseline ); 
	if(!bDual)
	{
		tStackLocalCap.SetSupportProfile(byMSProfile );
	}
	else
	{
		tStackLocalCap.SetSupportProfile( byDSProfile );		
	}

    if( !(tVidCap <= tStackLocalCap) )
    {
		// 双流UXGA60视作有1080/30双流能力 [6/4/2013 chendaiwei]
		if( byLocalFps == 60 && byLocalRes == VIDEO_FORMAT_UXGA)
		{
			byStackLocalRes = CMtAdpUtils::ResIn2Out(VIDEO_FORMAT_HD1080);
			byLocalFps = 30;
			tStackLocalCap.SetResolution(byStackLocalRes, byLocalFps);

			if(!(tVidCap <= tStackLocalCap))
			{
				MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[GetCapVideoParam]1 Remote H.264 Cap GREATER than Local, NOT Support.\n");
				MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[GetCapVideoParam]1 Remote <MBPS.%d, FS.%d, HpProfile.%d>, Local <MBPS.%d, FS.%d, HpProfile%d>!\n",
					tVidCap.GetMaxMBPS(), tVidCap.GetMaxFS(),tVidCap.GetProfileValue(),
					tStackLocalCap.GetMaxMBPS(), tStackLocalCap.GetMaxFS(),tStackLocalCap.GetProfileValue());

				return FALSE;
			}
		}
		else
		{
			MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[GetCapVideoParam] Remote H.264 Cap GREATER than Local, NOT Support.\n");
			MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[GetCapVideoParam] Remote <MBPS.%d, FS.%d, HpProfile.%d>, Local <MBPS.%d, FS.%d, HpProfile%d>!\n",
						  tVidCap.GetMaxMBPS(), tVidCap.GetMaxFS(),tVidCap.GetProfileValue(),
						  tStackLocalCap.GetMaxMBPS(), tStackLocalCap.GetMaxFS(),tStackLocalCap.GetProfileValue());
			return FALSE;
		}
    }

    // 额外的高清标清终端主视频对通限制(仅对KedaMT处理, KedaMCU及其他厂商均不考虑)
    //guzh 2008/08/07 BAS HD支持，放开限制
    /*
    if ( m_byVendorId == MT_MANU_KDC && !bDual)
    {
        // 1. 1080/720 的会议，必须4CIF及以上发送能力
        if ( tStackLocalCap.IsSupport(emHD720, 1) )
        {
            if ( !tVidCap.IsSupport(emCIF4, 25) &&
                !tVidCap.IsSupport(em704480, 25) )
            {
                MtAdpWarning("[GetCapVideoParam] HD conf but remote cap only CIF, NOT Support.\n");
                return FALSE;
            }
        }
       
        // 2. 4CIF 的会议
        // 暂时不做限制，允许单通(可能会造成转发CIF码流到HD终端)
        
        // 3. CIF 的会议，不允许高清能力终端(根据对端能力集判断)发送码流
        if ( byLocalRes == VIDEO_FORMAT_CIF )
        {
            if ( tVidCap.IsSupport(emHD720, 25) )
            {
                MtAdpWarning("[GetCapVideoParam] CIF Conf but MT is HD-cap, NOT Support.\n");
                return FALSE;
            }
        }
    }
    */
    
    // 分析计算对方希望打开的通道分辨率
    //const s32 s_anRes[] = { (s32)emHD1080, (s32)emSXGA, (s32)emHD720, (s32)em1024768, (s32)em800600, (s32)emCIF4, (s32)em704480, (s32)em640480, (s32)emCIF, (s32)em352240 };
	const s32 s_anRes[] = { (s32)emHD1080, (s32)emUXGA, (s32)emSXGA, (s32)em1280x800, (s32)emHD720, (s32)em1024768, (s32)emW4CIF, (s32)em800600, (s32)emCIF4, (s32)em704480, (s32)em640480, (s32)emCIF2, (s32)emCIF, (s32)em352240};
    BOOL bLocalStart = FALSE;
    for(u8 byLoop = 0; byLoop < sizeof(s_anRes)/sizeof(s32); byLoop++ )
    {
        if ( !bLocalStart && byStackLocalRes == s_anRes[byLoop] )
        {
            bLocalStart = TRUE;
        }
        if (!bLocalStart)
        {
            continue;
        }

        if ( tVidCap.GetResolution(s_anRes[byLoop]) != 0 )
        {
            byRemoteRes = CMtAdpUtils::ResOut2In(/*(emResolution)*/s_anRes[byLoop]);
            //byRemoteFps = tVidCap.GetResolution(byRemoteRes);
			byRemoteFps = tVidCap.GetResolution((u8)s_anRes[byLoop]);
            break;
        }
    }

    if (bDual)
    {
        if (byLocalFps != 0)
        {
            byRemoteFps = min(byRemoteFps, byLocalFps);
        }
    }

    /*     
    s32 byStackRemoteRes;

    TConfInfo tTmpCalcInfo;
    u16 wHeightL, wHeightR;
    u16 wWidthL, wWidthR;

    // 计算本地分辨率图像宽高
    tTmpCalcInfo.GetVideoScale(MEDIA_TYPE_H264, wWidthL, wHeightL, &byLocalRes);

    u8 byFirstAcceptIdx = 0xFF;
    for(u8 byLoop = 0; byLoop < sizeof(s_anRes)/sizeof(s32); byLoop++ )
    {
        byStackRemoteRes = s_anRes[byLoop];        
        byRemoteFps = tVidCap.GetResolution(byStackRemoteRes);

        byRemoteRes = CMtAdpUtils::ResOut2In(byStackRemoteRes);

        // 跳过不支持的分辨率
        if (byRemoteFps == 0)
            continue;

        // 计算对方分辨率宽高            
        tTmpCalcInfo.GetVideoScale(MEDIA_TYPE_H264, wWidthR, wHeightR, &byRemoteRes);

        // 请求的分辨率超出本端能力
        // 第一层if是为了防止对方打开过大，第二层if是为了防止能力类似情况下误杀
        
        if ( ((s32)wWidthR)*((s32)wHeightR) > ((s32)wWidthL)*((s32)wHeightL) )
        {                
            if (g_bChkRemoteGeCap)
            {
                //FIXME1: 这里采用这样的判断也是不得已。有可能对方会发送一个比较大的帧*小的帧率，但是我们无法限制, 因为总的处理能力是类似的
                //这里的核心原因是因为我们的双流和第一路能力合并共同发送，会导致对方对我们帧大小能力的高估
                //FIXME2: 这里可能有舍入误差，就是说还可能误杀。此时通过调整外部的 g_bChkRemoteGeCap 变量保护
                if ( ((s32)wWidthR)*((s32)wHeightR)*((s32)byRemoteFps) >
                    ((s32)wWidthL)*((s32)wHeightL)*((s32)byLocalFps) )
                {
                    MtAdpWarning("[GetCapVideoParam][H.264] Remote request open %d*%d*%dfps > Local %d*%d*%dfps, Match FAILED!\n",
                                 wWidthR, wHeightR, byRemoteFps,
                                 wWidthL, wHeightL, byLocalFps);
                    return FALSE;
                }
            }
        }
        // 分辨率相等，模糊比较帧率
        else if ( ((s32)wWidthR)*((s32)wHeightR) == ((s32)wWidthL)*((s32)wHeightL) )
        {
            if ( bDual )
            {
                // 双流不比较帧率，支持该分辨率即可
                return TRUE;
            }

            if ( CMtAdpUtils::IsSrcFpsAcptable(byRemoteFps, byLocalFps) )
            {
                byFirstAcceptIdx = byLoop;
                break;
            }
        }
        // 对端分辨率小，比较总处理能力
        else
        {
            if ( bDual )
            {
                // 双流不比较帧率，支持即可
                return TRUE;
            }

            if ( ((s32)wWidthR)*((s32)wHeightR)*((s32)byRemoteFps) <=
                 ((s32)wWidthL)*((s32)wHeightL)*((s32)byLocalFps) )
            {
                byFirstAcceptIdx = byLoop;
                break;
            }                
        }
    }

    if (byFirstAcceptIdx == 0xFF)
        return FALSE;

    byStackRemoteRes = s_anRes[byFirstAcceptIdx];        
    byRemoteFps = tVidCap.GetResolution(byStackRemoteRes);
    byRemoteRes = CMtAdpUtils::ResOut2In(byStackRemoteRes);
    

    // 额外的高清标清终端对通限制(仅对KedaMT处理, KedaMCU及其他厂商均不考虑)
    if ( m_byVendorId == MT_MANU_KDC )
    {
        // 1. 1080p/i/720p 的会议，必须CIF以上发送能力
        if ( byLocalRes == VIDEO_FORMAT_HD1080 ||
             byLocalRes == VIDEO_FORMAT_HD720 )
        {
            if (byRemoteRes == VIDEO_FORMAT_CIF || byRemoteRes == VIDEO_FORMAT_SIF)
                return FALSE;
        }
        
        // 2. 4CIF 的会议，暂时不做限制，允许单通(可能会造成转发CIF码流到HD终端)
        
        // 3. CIF 的会议，不允许高清能力终端(根据对端能力集判断)发送码流
        if ( byLocalRes == VIDEO_FORMAT_CIF )
        {
            TH264VideoCap* ptH264Cap = m_ptRemoteCapSet->GetH264VideoCap();
            if (ptH264Cap == NULL)
                return FALSE;

            if (ptH264Cap->GetResolution(emHD720p) > 0)
                return FALSE;
        }
    }
    */

// 	if( byRemoteFps > byLocalFps && byRemoteRes != VIDEO_FORMAT_CIF )
// 	{
//         MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "[GetCapVideoParam] Remote H.264 Cap Res<%d> FS<%d> more than Local Res<%d> FS<%d>, NOT Support.\n",byRemoteRes,
// 			byRemoteFps,byLocalRes,byLocalFps);
// 
//         return FALSE;
// 	}

    return TRUE;
}

/*=============================================================================
  函 数 名： MtAdpPrintf
  功    能： 打印普通信息
  算法实现： 
  全局变量： 
  参    数： u8 level
             BOOL32 toScreen
             BOOL32 toFile
             s8*  printStr
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
// void CMtAdpInst::MtAdpPrintf( u8 byLevel, BOOL32 bToScreen, BOOL32 bToFile, s8* pszPrintStr )
// {
// 	static const s8 * pchLevelName[] = 
// 	{(s8 *)"[CRITICAL]", (s8 *)"[ERROR]", (s8 *)"[WARNING]", "", "",""};
// 
// 	s8  achPrintBuf[256];
// 	s32 nLen = 0;
// 
// 	if( byLevel > g_cMtAdpApp.m_byDebugLevel )
// 	{
// 		return;
// 	}
//     
//     // guzh [5/19/2007] 系统DEBUG > WARNING 才打印时间
//     if ( g_cMtAdpApp.m_byDebugLevel > DEBUG_WARNING )
//     {
//         // zbq [04/25/2007]
//         TIME_SHOW
//     }
// 
// 	if( byLevel > DEBUG_DETAIL ) 
// 	{
// 		byLevel = DEBUG_DETAIL;
// 	}
// 	if( GetInsID() != CInstance::DAEMON )
// 	{
// 		nLen = sprintf( achPrintBuf, "[MtAdpInst %u] %s ", GetInsID(), pchLevelName[byLevel] );	
// 	}
// 	else
// 	{
// 		nLen = sprintf( achPrintBuf, "[MtAdpDaemon]  %s ", pchLevelName[byLevel] );
// 	}
// 	strncat( achPrintBuf + nLen, pszPrintStr, sizeof(achPrintBuf) - nLen );
// 
// 	OspPrintf( bToScreen, bToFile, achPrintBuf );	 
// 	
// 	return;
// }
/*lint -save -esym(530, argptr)*/
void CMtAdpInst::MAPrint( const u8 byLevel, const u16 wModule, const s8* pszFormat, ...)
{
    s8 achPrintBuf[1024];
    
    va_list argptr;
    s32 nLen = 0;
    if( GetInsID() != CInstance::DAEMON )
    {
        nLen = sprintf( achPrintBuf, "[MtAdpInst%u(Mt%u)]", GetInsID(), m_byMtId );	
    }
    else
    {
        nLen = sprintf( achPrintBuf, "[MtAdpDaemon]");
    }
    va_start( argptr, pszFormat );    
    vsnprintf(achPrintBuf + nLen, 1024 - nLen - 1, pszFormat, argptr );
    LogPrint( byLevel, wModule, achPrintBuf );
    va_end(argptr);    
}
/*lint -restore*/

/*=============================================================================
  函 数 名： MtAdpDetail
  功    能： 打印调试细节信息
  算法实现： 
  全局变量： 
  参    数：  s8*  pszFmt
             ...
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
/*lint -save -esym(530, tArgptr)*/
// void CMtAdpInst::MtAdpDetail( s8*  pszFmt, ... )
// {
// 	s8      achPrintBuf[512];
// 	s32     nBufLen;		
// 	va_list tArgptr;
// 
// 	va_start( tArgptr, pszFmt );
// 	nBufLen = vsprintf( achPrintBuf, pszFmt, tArgptr );	
// 	va_end( tArgptr );
// 
//     if(nBufLen <= 0 )
//     {
//         return;
// 	}
// 
// 	MtAdpPrintf( DEBUG_DETAIL, TRUE, FALSE, achPrintBuf );
// 
// 	return;
// }
/*lint -restore*/
/*=============================================================================
  函 数 名： MtAdpVerbose
  功    能： 打印调试详细信息
  算法实现： 
  全局变量： 
  参    数：  s8*  pszFmt
             ...
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
/*lint -save -esym(530, tArgptr)*/
// void CMtAdpInst::MtAdpVerbose( s8*  pszFmt, ...)
// {
// 	s8      achPrintBuf[512];
// 	s32     nBufLen;		
// 	va_list tArgptr;
// 
// 	va_start( tArgptr, pszFmt );
// 	nBufLen = vsprintf(achPrintBuf, pszFmt, tArgptr );	
// 	va_end( tArgptr );
// 
//     if(nBufLen <= 0 )
//     {
//         return;
// 	}
// 
// 	MtAdpPrintf(DEBUG_VERBOSE, TRUE, FALSE, achPrintBuf);
// 
// 	return;
// }
/*lint -restore*/
/*=============================================================================
  函 数 名： MtAdpInfo
  功    能： 打印调试普通信息
  算法实现： 
  全局变量： 
  参    数： s8*  pszFmt
             ...
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
/*lint -save -esym(530, tArgptr)*/
// void CMtAdpInst::MtAdpInfo(s8*  pszFmt, ...)
// {
// 	s8      achPrintBuf[512];
// 	s32     nBufLen;		
// 	va_list tArgptr;
// 	
// 	va_start( tArgptr, pszFmt );
// 	nBufLen = vsprintf(achPrintBuf, pszFmt, tArgptr );	
// 	va_end( tArgptr );
// 
//     if( nBufLen <= 0 )
//     {
//         return;
// 	}
// 	
// 	MtAdpPrintf(DEBUG_INFO, TRUE, FALSE, achPrintBuf);
// 
// 	return;
// }
/*lint -restore*/
/*=============================================================================
  函 数 名： MtAdpWarning
  功    能： 打印告警信息
  算法实现： 
  全局变量： 
  参    数： s8*  pszFmt
             ...
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
/*lint -save -esym(530, tArgptr)*/
// void CMtAdpInst::MtAdpWarning(s8*  pszFmt, ...)
// {
// 	s8      achPrintBuf[512];
// 	s32     nBufLen;		
// 	va_list tArgptr;
// 	
// 	va_start( tArgptr, pszFmt );
// 	nBufLen = vsprintf(achPrintBuf, pszFmt, tArgptr );	
// 	va_end( tArgptr );
// 
//     if(nBufLen <= 0 )
//     {
//         return;
// 	}
// 	
// 	MtAdpPrintf(DEBUG_WARNING, TRUE, FALSE, achPrintBuf);
// 
// 	return;
// }
/*lint -restore*/
/*=============================================================================
  函 数 名： MtAdpException
  功    能： 打印错误信息
  算法实现： 
  全局变量： 
  参    数：  s8*  pszFmt
             ...
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
/*lint -save -esym(530, tArgptr)*/
// void CMtAdpInst::MtAdpException( s8*  pszFmt, ... )
// {
// 	s8      achPrintBuf[512];
// 	s32     nBufLen;		
// 	va_list tArgptr;
// 	
// 	va_start( tArgptr, pszFmt );
// 	nBufLen = vsprintf(achPrintBuf, pszFmt, tArgptr );	
// 	va_end( tArgptr );
// 
//     if(nBufLen <= 0 )
//     {
//         return;
// 	}
// 	
// 	MtAdpPrintf(DEBUG_ERROR, TRUE, FALSE, achPrintBuf);
// 
// 	return;
// }
/*lint -restore*/
/*=============================================================================
  函 数 名： MtAdpCritical
  功    能： 打印危险信息
  算法实现： 
  全局变量： 
  参    数： s8*  pszFmt
             ...
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
/*lint -save -esym(530, tArgptr)*/
// void CMtAdpInst::MtAdpCritical(s8*  pszFmt, ...)
// {
// 	s8      achPrintBuf[512];
// 	s32     nBufLen;	
// 	va_list tArgptr;		
// 	va_start( tArgptr, pszFmt );
// 	nBufLen = vsprintf(achPrintBuf, pszFmt, tArgptr );	
// 	va_end( tArgptr );
// 
//     if(nBufLen <= 0 )
//     {
//         return;
// 	}
// 	
// 	MtAdpPrintf(DEBUG_CRITICAL, TRUE, FALSE, achPrintBuf);
// 	
// 	return;
// }
/*lint -restore*/
/*=============================================================================
函 数 名： SendMultSpyCapNotif
功    能： mcu是否都支持多回传能力互探
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
=============================================================================*/
void CMtAdpInst::OnSendMultSpyCapNotif( CServMsg &cServMsg, HCHAN hsChan )
{
//     CServMsg cMsg;
// //    cMsg.SetServMsg( 0, SERV_MSGHEAD_LEN );
//     memset( &cMsg, 0, sizeof(cMsg) );
//     cMsg.SetEventId( MCU_MCU_MULTSPYCAP_NOTIF );
    //memset( &cServMsg, 0, sizeof(cServMsg) );

	cServMsg.SetEventId( MCU_MCU_MULTSPYCAP_NOTIF );


	TNonStandardMsg tMsg;
	memset( &tMsg, 0, sizeof(tMsg) );
	tMsg.m_nMsgLen = cServMsg.GetServMsgLen();
	
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		StaticLog( "[MTADAP][SendMultSpyCapNotif]SendMultSpyCapNotif message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy( tMsg.m_abyMsgBuf, cServMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );

	StaticLog( "[MTadp][SendMultSpyCapNotif] SendMultSpyCapNotif OK !\n" );
	
	return;
}


/*=============================================================================
函 数 名： OnSendSpyFastUpdateCmd
功    能： 发送级联多回传请求关键帧消息
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20100402                 pj                    create
=============================================================================*/
void CMtAdpInst::OnSendSpyFastUpdateCmd( CServMsg &cServMsg, HCHAN hsChan )
{
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TMt tSrc   = *(TMt *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
	u8  byMode = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMsgHeadMsg) );
//	BOOL32 bLocal = FALSE;

	TMtadpHeadMsg tMtadpHeadMsg;
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tSrc, &tMtadpHeadMsg.m_tMsgDst.m_bLocal );
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = htonl( tMtadpHeadMsg.m_tMsgDst.m_dwPID );	

	HeadMsgToMtadpHeadMsg( tHeadMsg,tMtadpHeadMsg );

	//creat new msg body
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	//cMsg.SetMsgBody((u8 *)&tPartID, sizeof(tPartID));
	cMsg.SetMsgBody((u8 *)&byMode, sizeof(u8));
	cMsg.CatMsgBody((u8 *)&tMtadpHeadMsg, sizeof(TMtadpHeadMsg));
	
	TNonStandardMsg tMsg;
	
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendSpyFastUpdateCmd]OnSendSpyFastUpdateCmd message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "SendSpyFastUpdateCmd OK\n" );
	return;
}


/*=============================================================================
函 数 名： ProcMcuMcuSpyFastUpdateCmd
功    能： 处理级联多回传请求关键帧消息
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20100402                 pj                    create
=============================================================================*/
void CMtAdpInst::ProcMcuMcuSpyFastUpdateCmd( CServMsg &cServMsg, CServMsg &cMsg )
{
	cMsg.SetServMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() - cServMsg.GetMsgBodyLen() );
	//TPartID tPartID = *(TPartID *)cServMsg.GetMsgBody();
	
	TMsgHeadMsg tHeadMsg;
	u8 byMode = *(u8 *)(cServMsg.GetMsgBody());
	TMtadpHeadMsg tMtadpHeadMsg = *(TMtadpHeadMsg*)(cServMsg.GetMsgBody() + sizeof(u8));
	


	TMt tMt = GetMtFromPartId( ntohl(tMtadpHeadMsg.m_tMsgDst.m_dwPID), /*tPartID.m_bLocal*/TRUE);

	++tMtadpHeadMsg.m_tMsgSrc.m_byCasLevel;
	if( tMtadpHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		tMt.SetMcuId( tMt.GetMtId() );
		--tMtadpHeadMsg.m_tMsgDst.m_byCasLevel;
		tMt.SetMtId( tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[tMtadpHeadMsg.m_tMsgDst.m_byCasLevel] );		
	}
	//tHeadMsg.m_tMsgDst.m_tMt = tMt;


	MtadpHeadMsgToHeadMsg( tMtadpHeadMsg,tHeadMsg );
	
	cMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
	cMsg.CatMsgBody( (u8 *)&byMode, sizeof(u8) );	
	cMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );

	return;
}

/*=============================================================================
函 数 名： OnSendPreSetinReq
功    能： 进行多级联准备（双方握手）
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
2009-11-11				 xl					   modify msg body
=============================================================================*/
void CMtAdpInst::OnSendPreSetinReq( CServMsg &cServMsg, HCHAN hsChan )
{	
	TMsgHeadMsg tHeadMdg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TPreSetInReq tPreSetInReq = *(TPreSetInReq *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));
//	TSimCapSet   tSimCapSet   = *(TSimCapSet *)( cServMsg.GetMsgBody() + sizeof(TPreSetInReq) );

	TMtadpHeadMsg tMtadpHeadMsg;
	
	// modify msg body (tMt -> partId)
	
	TMt tSrc = tPreSetInReq.m_tSpyMtInfo.GetSpyMt();
	s16 swCount = tPreSetInReq.m_tReleaseMtInfo.GetCount();
	//TPartID tPartID;
	//tPartID.m_dwPID = GetPartIdFromMt( tSrc, &tPartID.m_bLocal );
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tSrc, &tMtadpHeadMsg.m_tMsgDst.m_bLocal );
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = htonl( tMtadpHeadMsg.m_tMsgDst.m_dwPID );
	//tPreSetInReq.m_dwEvId = htonl( tPreSetInReq.m_dwEvId );
	//tPreSetInReq.m_tReleaseMtInfo.m_swCount = htons( tPreSetInReq.m_tReleaseMtInfo.m_swCount );
	HeadMsgToMtadpHeadMsg( tHeadMdg,tMtadpHeadMsg );

	//creat new msg body
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	//cMsg.SetMsgBody((u8 *)&tPartID, sizeof(tPartID));
	cMsg.SetMsgBody( (u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg) );
	cMsg.CatMsgBody((u8 *)&tPreSetInReq, sizeof(tPreSetInReq));
	//cMsg.CatMsgBody((u8 *)&tHeadMdg, sizeof(TMsgHeadMsg));

	if( swCount >= 0 )
	{
		TPartID tPartID;
		memset( &tPartID,0,sizeof(tPartID) );
		tPartID.m_dwPID = GetPartIdFromMt( tPreSetInReq.m_tReleaseMtInfo.m_tMt, &tPartID.m_bLocal );
		tPartID.m_dwPID = htonl( tPartID.m_dwPID );
		cMsg.CatMsgBody((u8 *)&tPartID, sizeof(tPartID));
	}

	u32 dwResWidth = *(u32 *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg)+sizeof(TPreSetInReq));
	u32 dwResHeight = *(u32 *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg)+sizeof(TPreSetInReq)+sizeof(u32));
	cMsg.CatMsgBody((u8 *)&dwResWidth, sizeof(dwResWidth));
	cMsg.CatMsgBody((u8 *)&dwResHeight, sizeof(dwResHeight));

	TNonStandardMsg tMsg;	
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendPreSetinReq]OnSendPreSetinReq message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendPreSetinReq] Send PreSetin tSrc(%d.%d.%d) level.%d ReleaseMt(%d.%d.%d) level.%d byCount.%d!\n",
		tSrc.GetMcuId(),tSrc.GetMtId(),tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
		tMtadpHeadMsg.m_tMsgDst.m_byCasLevel,
		tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMcuId(),
		tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMtId(),
		tPreSetInReq.m_tReleaseMtInfo.m_abyMtIdentify[0],
		tPreSetInReq.m_tReleaseMtInfo.m_byCasLevel,
		tPreSetInReq.m_tReleaseMtInfo.GetCount()
		);
	return;
}

/*=============================================================================
函 数 名： OnSendPreSetinAck
功    能： 进行多级联准备（双方握手）
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
20091111				 xl						modify msg body
=============================================================================*/
void CMtAdpInst::OnSendPreSetinAck( CServMsg &cServMsg, HCHAN hsChan )
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	TMtadpHeadMsg tMtadpHeadMsg;
	
	//modify msg body
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TPreSetInRsp tPreSetInRsp = *(TPreSetInRsp *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));
	TMt tMt = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	if( LOCAL_MCUID != tMt.GetMcuId() )
	{		
		tMt.SetMtId( (u8)tMt.GetMcuId() );
		tMt.SetMcuId( LOCAL_MCUID );
	}	
	
	s16 swCount = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.GetCount();
	//tPreSetInRsp.m_tSetInReqInfo.m_dwEvId = htonl( tPreSetInRsp.m_tSetInReqInfo.m_dwEvId );
	//tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_swCount = htons(tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_swCount);
// 	tPreSetInRsp.m_dwVidSpyBW = htonl( tPreSetInRsp.m_dwVidSpyBW );
// 	tPreSetInRsp.m_dwAudSpyBW = htonl( tPreSetInRsp.m_dwAudSpyBW );

	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = GetPartIdFromMt( tMt, &tMtadpHeadMsg.m_tMsgSrc.m_bLocal );
	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = htonl(tMtadpHeadMsg.m_tMsgSrc.m_dwPID);

	if( LOCAL_MCUID != tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId() )
	{
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[tHeadMsg.m_tMsgSrc.m_byCasLevel] = \
			tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMtId();
		++tHeadMsg.m_tMsgSrc.m_byCasLevel;
	}
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
	}

	tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt( tMt );
	//cMsg.SetMsgBody((u8 *)&tPartID, sizeof(tPartID));

	HeadMsgToMtadpHeadMsg( tHeadMsg,tMtadpHeadMsg );	

	if( swCount >= 0 )
	{
		TPartID tPartID;
		tMt = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt;
		if( LOCAL_MCUID != tMt.GetMcuId() )
		{		
			tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_abyMtIdentify[tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCasLevel] = tMt.GetMtId();
			++tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCasLevel;
			
			tMt.SetMtId( (u8)tMt.GetMcuId() );
			tMt.SetMcuId( LOCAL_MCUID );			
		}
		memset( &tPartID,0,sizeof(tPartID) );
		tPartID.m_dwPID = GetPartIdFromMt( tMt, &tPartID.m_bLocal );
		tPartID.m_dwPID = htonl( tPartID.m_dwPID );

		cMsg.SetMsgBody((u8 *)&tPreSetInRsp, sizeof(tPreSetInRsp) );
		cMsg.CatMsgBody((u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg) );
		cMsg.CatMsgBody((u8 *)&tPartID, sizeof(tPartID));					
	}
	else
	{
		cMsg.SetMsgBody((u8 *)&tPreSetInRsp, sizeof(tPreSetInRsp) );
		cMsg.CatMsgBody((u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg) );
	}
	
	TNonStandardMsg tMsg;
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendPreSetinAck]OnSendPreSetinAck message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );	
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendPreSetinAck] Send PreSetin Ack tSrc(%d.%d.%d) level.%d ReleaseMt(%d.%d.%d) level.%d byCount.%d!\n",
		tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
		tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMtId(),
		tMtadpHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		tMtadpHeadMsg.m_tMsgSrc.m_byCasLevel,
		tMt.GetMcuId(),
		tMt.GetMtId(),
		tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_abyMtIdentify[0],
		tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCasLevel,
		tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.GetCount()
		);
	return;
}

/*=============================================================================
函 数 名： OnSendPreSetinNack
功    能： 进行多级联准备（双方握手）
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
=============================================================================*/
void CMtAdpInst::OnSendPreSetinNack( CServMsg &cServMsg, HCHAN hsChan )
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	
	TMtadpHeadMsg tMtadpHeadMsg;
	//modify msg body
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TPreSetInRsp tPreSetInRsp = *(TPreSetInRsp *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));
	TMt tMt = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	if( LOCAL_MCUID != tMt.GetMcuId() )
	{		
		tMt.SetMtId( (u8)tMt.GetMcuId() );
		tMt.SetMcuId( LOCAL_MCUID );
	}
	
//	BOOL32 bLocal = FALSE;
	
	//tPreSetInRsp.m_tSetInReqInfo.m_dwEvId = htonl(tPreSetInRsp.m_tSetInReqInfo.m_dwEvId);
	//tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_swCount = htons(tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_swCount);

	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = GetPartIdFromMt( tMt, &tMtadpHeadMsg.m_tMsgSrc.m_bLocal );
	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = htonl(tMtadpHeadMsg.m_tMsgSrc.m_dwPID);
    

	if( LOCAL_MCUID != tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId() )
	{
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[tHeadMsg.m_tMsgSrc.m_byCasLevel] = \
			tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMtId();
		++tHeadMsg.m_tMsgSrc.m_byCasLevel;
	}
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
	}

	
	//cMsg.SetMsgBody((u8 *)&tPartID, sizeof(tPartID));
	HeadMsgToMtadpHeadMsg( tHeadMsg,tMtadpHeadMsg );

	cMsg.SetMsgBody((u8 *)&tPreSetInRsp, sizeof(tPreSetInRsp) );
	cMsg.CatMsgBody((u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg) );

	TNonStandardMsg tMsg;
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendPreSetinNack]OnSendPreSetinNack message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendPreSetinNack] Send PreSetin Nack tSrc(%d.%d.%d) level.%d \n",
		tMt.GetMcuId(),tMt.GetMtId(),tMtadpHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		tMtadpHeadMsg.m_tMsgSrc.m_byCasLevel
		);
	return;
}
 
/*=============================================================================
函 数 名： OnSendSpyChnnlNotif
功    能： 通知下级mcu，上级可以接受其回传码流的带宽
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
=============================================================================*/
void CMtAdpInst::OnSendSpyChnnlNotif( CServMsg &cServMsg, HCHAN hsChan )
{
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TSpyResource tSpyResource = *(TSpyResource *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));
	u32 dwEvId = *(u32 *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) +sizeof(TSpyResource));
	/*u8 byRes = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TSpyResource) + sizeof(u32) );*/
	
	u32 dwResWidth = *(u32 *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) +sizeof(TSpyResource)+sizeof(u32));
	u32 dwResHeight = *(u32 *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) +sizeof(TSpyResource)+sizeof(u32)+sizeof(u32));

	// modify msg body (tMt -> partId)
	TMt tSrc = tSpyResource.m_tSpy;
	//BOOL32 bLocal = FALSE;
	TMtadpHeadMsg tMtAdpHeadMsg;
	//TPartID tPartID;


	tMtAdpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tSrc, &tMtAdpHeadMsg.m_tMsgDst.m_bLocal );	
	tMtAdpHeadMsg.m_tMsgDst.m_dwPID = htonl( tMtAdpHeadMsg.m_tMsgDst.m_dwPID );
	HeadMsgToMtadpHeadMsg( tHeadMsg,tMtAdpHeadMsg );
	
	//creat new msg body
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	//cMsg.SetMsgBody((u8 *)&tPartID, sizeof(tPartID));
	cMsg.SetMsgBody((u8 *)&tSpyResource, sizeof(TSpyResource));
	cMsg.CatMsgBody((u8 *)&dwEvId, sizeof(u32) );
	/*cMsg.CatMsgBody((u8 *)&byRes, sizeof(u8) );*/
	cMsg.CatMsgBody((u8 *)&tMtAdpHeadMsg, sizeof(TMtadpHeadMsg) );
	cMsg.CatMsgBody((u8*)&dwResWidth,sizeof(u32));
	cMsg.CatMsgBody((u8*)&dwResHeight,sizeof(u32));

	TNonStandardMsg tMsg;
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendSpyChnnlNotif]OnSendSpyChnnlNotif message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendSpyChnnlNotif] Send SpyChnnlNotif tSrc(%d.%d.%d) level.%d !\n",
		tSrc.GetMcuId(),tSrc.GetMtId(),tMtAdpHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
		tMtAdpHeadMsg.m_tMsgDst.m_byCasLevel		
		);

	return;
}

/*=============================================================================
函 数 名： OnSendSwitchToMMcuFailNotif
功    能： 下级通知上级建交换到上级mcu失败
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
		   CServMsg &cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2011-09-08             zhangli                   create
=============================================================================*/
void CMtAdpInst::OnSendSwitchToMMcuFailNotif(CServMsg &cServMsg, HCHAN hsChan)
{
	CServMsg cMsg;
	cMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
	
	TMtadpHeadMsg tMtadpHeadMsg;
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody());
	TMt tMt = *(TMt *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
	
	if(LOCAL_MCUID != tMt.GetMcuId())
	{
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[tHeadMsg.m_tMsgSrc.m_byCasLevel] = tMt.GetMtId();
		++tHeadMsg.m_tMsgSrc.m_byCasLevel;
		tMt.SetMtId((u8)tMt.GetMcuId());
		tMt.SetMcuId(LOCAL_MCUID);
	}
	
	if(tHeadMsg.m_tMsgDst.m_byCasLevel > 0)
	{
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
	}
	
	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = GetPartIdFromMt(tMt, &tMtadpHeadMsg.m_tMsgSrc.m_bLocal);
	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = htonl(tMtadpHeadMsg.m_tMsgSrc.m_dwPID);
	
	HeadMsgToMtadpHeadMsg(tHeadMsg, tMtadpHeadMsg);
	
	cMsg.SetMsgBody((u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg));	
	cMsg.CatMsgBody((u8 *)&tMt, sizeof(TMt) );
	
	TNonStandardMsg tMsg;
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendSwitchToMMcuFailNotif]OnSendSwitchToMMcuFailNotif message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy(tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen);
	kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg));
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendSwitchToMMcuFailNotif] OnSendSwitchToMMcuFailNotif----tSrc(%d.%d.%d) level.%d \n",
		tMt.GetMcuId(),tMt.GetMtId(),tMtadpHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		tMtadpHeadMsg.m_tMsgSrc.m_byCasLevel);
}

/*=============================================================================
函 数 名： OnSendMtExtInfoNotify
功    能： 下级通知上级RTCP信息
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
CServMsg &cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2010-12-15             倪志俊                   create
=============================================================================*/
void CMtAdpInst::OnSendMtExtInfoNotify( CServMsg &cServMsg, HCHAN hsChan )
{
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	
	TMtadpHeadMsg tMtadpHeadMsg;
	//modify msg body
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TMultiRtcpInfo tMultiRtcpInfo = *(TMultiRtcpInfo *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));
	TMt tMt = tMultiRtcpInfo.m_tSrcMt;
	if( LOCAL_MCUID != tMt.GetMcuId() )
	{		
		tMt.SetMtId( (u8)tMt.GetMcuId() );
		tMt.SetMcuId( LOCAL_MCUID );
	}
	
//	BOOL32 bLocal = FALSE;
	
	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = GetPartIdFromMt( tMt, &tMtadpHeadMsg.m_tMsgSrc.m_bLocal );
	tMtadpHeadMsg.m_tMsgSrc.m_dwPID = htonl(tMtadpHeadMsg.m_tMsgSrc.m_dwPID);
	
	if( LOCAL_MCUID != tMultiRtcpInfo.m_tSrcMt.GetMcuId() )
	{
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[tHeadMsg.m_tMsgSrc.m_byCasLevel] = tMultiRtcpInfo.m_tSrcMt.GetMtId();
		++tHeadMsg.m_tMsgSrc.m_byCasLevel;
	}
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		--tHeadMsg.m_tMsgDst.m_byCasLevel;
	}
	
	HeadMsgToMtadpHeadMsg( tHeadMsg,tMtadpHeadMsg );
	
	cMsg.SetMsgBody((u8 *)&tMultiRtcpInfo, sizeof(tMultiRtcpInfo) );
	cMsg.CatMsgBody((u8 *)&tMtadpHeadMsg, sizeof(tMtadpHeadMsg) );
	
	TNonStandardMsg tMsg;
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendMtExtInfoNotify]OnSendMtExtInfoNotify message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendMtExtInfoNotify] Send Mt RTCP ExtInfo----tSrc(%d.%d.%d) level.%d \n",
		tMt.GetMcuId(),tMt.GetMtId(),tMtadpHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		tMtadpHeadMsg.m_tMsgSrc.m_byCasLevel
		);
	return;
}


/*=============================================================================
函 数 名： OnSendSpyBWFullNotif
功    能： 通知下级mcu，上级mcu带宽不足，不能进行级联多回传
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
=============================================================================*/
void CMtAdpInst::OnSendRejectSpyNotif( CServMsg &cServMsg, HCHAN hsChan )
{
	TMt tSrc = *(TMt *)cServMsg.GetMsgBody();
	u32 dwSpyBW = *(u32 *)( cServMsg.GetMsgBody() + sizeof(TMt) );
	
	// modify msg body (tMt -> partId)
//	BOOL32 bLocal = FALSE;
	TPartID tPartID;
	tPartID.m_dwPID = GetPartIdFromMt( tSrc, &tPartID.m_bLocal );
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	tPartID.m_dwPID = htonl( tPartID.m_dwPID );
#endif
	//creat new msg body
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	cMsg.SetMsgBody((u8 *)&tPartID, sizeof(tPartID));
	cMsg.CatMsgBody((u8 *)&dwSpyBW, sizeof(u32));
	
	TNonStandardMsg tMsg;
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();

	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendSpyBWFullNotif]OnSendSpyBWFullNotif message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendRejectSpyNotif] OnSendRejectSpyNotif OK !\n" );
	return;
}

/*=============================================================================
函 数 名： OnSendBanishSpyCmd
功    能： 上级mcu发给下级的拆除相应的交换，回收回传通道
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
=============================================================================*/
void CMtAdpInst::OnSendBanishSpyCmd( CServMsg &cServMsg, HCHAN hsChan )
{
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TMt tSrc = *(TMt *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
	u8 byMode = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TMt) );
	u8 bySpyNoUse = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TMt) + sizeof(u8));
	
	TMtadpHeadMsg tMtadpHeadMsg;
	//tSrc = tHeadMsg.m_tMsgDst.m_tMt;
	// modify msg body (tMt -> partId)
//	BOOL32 bLocal = FALSE;
	//TPartID tPartID;
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = GetPartIdFromMt( tSrc, &tMtadpHeadMsg.m_tMsgDst.m_bLocal );
	tMtadpHeadMsg.m_tMsgDst.m_dwPID = htonl( tMtadpHeadMsg.m_tMsgDst.m_dwPID );
	
	HeadMsgToMtadpHeadMsg( tHeadMsg,tMtadpHeadMsg );
	//creat new msg body
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	//cMsg.SetMsgBody((u8 *)&tPartID, sizeof(tPartID));
	cMsg.SetMsgBody((u8 *)&byMode, sizeof(u8));
	cMsg.CatMsgBody((u8 *)&tMtadpHeadMsg, sizeof(TMtadpHeadMsg));
	cMsg.CatMsgBody((u8 *)&bySpyNoUse, sizeof(bySpyNoUse));
	
	TNonStandardMsg tMsg;
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendBanishSpyCmd]OnSendBanishSpyCmd message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendBanishSpyCmd] Send BanishSpyCmd tSrc(%d.%d.%d) level.%d !\n",
		tSrc.GetMcuId(),tSrc.GetMtId(),tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
		tMtadpHeadMsg.m_tMsgDst.m_byCasLevel		
		);

	return;
}

/*=============================================================================
函 数 名： OnSendBanishSpyNotif
功    能： 上级mcu发给下级的拆除相应的交换，回收回传通道
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           CServMsg &cMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20090928                 pj                    create
=============================================================================*/
void CMtAdpInst::OnSendBanishSpyNotif( CServMsg &cServMsg, HCHAN hsChan )
{
	TMt tSrc = *(TMt *)cServMsg.GetMsgBody();
	u8  byMode = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMt) );
	u32 dwSpyBW = *(u32 *)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) );
	
	// modify msg body (tMt -> partId)
//	BOOL32 bLocal = FALSE;
	TPartID tPartID;
	tPartID.m_dwPID = GetPartIdFromMt( tSrc, &tPartID.m_bLocal );

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	tPartID.m_dwPID = htonl( tPartID.m_dwPID );
#endif

	//creat new msg body
	CServMsg cMsg;
	cMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
	cMsg.SetMsgBody((u8 *)&tPartID, sizeof(tPartID));
	cMsg.CatMsgBody((u8 *)&byMode, sizeof(u8));
	cMsg.CatMsgBody((u8 *)&dwSpyBW, sizeof(u32));
	
	TNonStandardMsg tMsg;
	tMsg.m_nMsgLen = cMsg.GetServMsgLen();
	
	if((u32)tMsg.m_nMsgLen > sizeof(tMsg.m_abyMsgBuf))
	{
		MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendBanishSpyCmd]OnSendBanishSpyCmd message is too large(len:%d)\n", tMsg.m_nMsgLen );
		return;
	}
	memcpy( tMsg.m_abyMsgBuf, cMsg.GetServMsg(), tMsg.m_nMsgLen );
	kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_NONSTANDARD_NTF, &tMsg, sizeof(tMsg) );
	
	MAPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[OnSendBanishSpyNotif] OnSendBanishSpyNotif OK !\n" );
}

/*=============================================================================
函 数 名： OnSendMultiCascadeReqMsg
功    能： 预处理多级联请求信息
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20100715                周晶晶                   create
=============================================================================*/
void CMtAdpInst::OnSendMultiCascadeReqMsg( CServMsg &cServMsg, HCHAN hsChan )
{
	TMsgHeadMsg *ptHeadMsg = NULL;
	TMt tMt;
	if( cServMsg.GetEventId() == MCU_MCU_SETIN_REQ )
	{
		ptHeadMsg = (TMsgHeadMsg *)( cServMsg.GetMsgBody() + sizeof(TSetInParam) + sizeof(TMcuMcuReq) );
		TSetInParam *ptParam = (TSetInParam *)( cServMsg.GetMsgBody() + sizeof(TMcuMcuReq) );
		tMt = ptParam->m_tMt;
	}
	else if( cServMsg.GetEventId() == MCU_MCU_INVITEMT_REQ )
	{
		u8 byMtNum = ( cServMsg.GetMsgBodyLen() - sizeof(TMcuMcuReq) - sizeof(TMt) - sizeof(TMsgHeadMsg) ) / sizeof(TAddMtInfo);
		ptHeadMsg = (TMsgHeadMsg *)( cServMsg.GetMsgBody() + sizeof(TMcuMcuReq) + sizeof(TMt) + sizeof(TAddMtInfo) * byMtNum );
		tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMcuMcuReq) );
	}
	else
	{
		ptHeadMsg = (TMsgHeadMsg *)( cServMsg.GetMsgBody() + sizeof(TMcuMcuReq) + sizeof(TMt) );
		tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMcuMcuReq) );
	}
	
	TMcuMcuReq *ptReq = (TMcuMcuReq *)cServMsg.GetMsgBody();

	TReq tReq;
	tReq.m_nReqID = m_dwMMcuReqId ++;
	astrncpy(tReq.m_aszUserName, ptReq->m_szUserName, 
			 sizeof(tReq.m_aszUserName), sizeof(ptReq->m_szUserName));
	astrncpy(tReq.m_aszUserPass, ptReq->m_szUserPwd, 
		     sizeof(tReq.m_aszUserPass), sizeof(ptReq->m_szUserPwd));
	tReq.m_tMsgDst.m_byCasLevel = ptHeadMsg->m_tMsgDst.m_byCasLevel;
	memcpy( &tReq.m_tMsgDst.m_abyMtIdentify[0],
			&ptHeadMsg->m_tMsgDst.m_abyMtIdentify[0],
			sizeof(tReq.m_tMsgDst.m_abyMtIdentify)
			);
	tReq.m_tMsgSrc.m_byCasLevel = ptHeadMsg->m_tMsgSrc.m_byCasLevel;
	memcpy( &tReq.m_tMsgSrc.m_abyMtIdentify[0],
			&ptHeadMsg->m_tMsgSrc.m_abyMtIdentify[0],
			sizeof(tReq.m_tMsgSrc.m_abyMtIdentify)
			);

	TPartID tPartId;
	if( !/*ptHeadMsg->m_tMsgDst.m_*/tMt.IsNull() )
	{
		tPartId.m_dwPID = GetPartIdFromMt( /*ptHeadMsg->m_tMsgDst.m_*/tMt,&tPartId.m_bLocal );
	}

	u8 *pData = cServMsg.GetMsgBody() + sizeof(TMcuMcuReq);
	cServMsg.SetMsgBody( pData,cServMsg.GetMsgBodyLen() - sizeof(TMcuMcuReq) );

	switch( cServMsg.GetEventId() )
	{
		case MCU_MCU_INVITEMT_REQ:
			OnSendInviteMtReq( cServMsg, hsChan, tReq, tPartId );
			break;
		case MCU_MCU_REINVITEMT_REQ:
			OnSendReInviteMtReq( cServMsg, hsChan, tReq, tPartId );
			break;
		case MCU_MCU_DROPMT_REQ:
			OnSendDropMtReq( cServMsg, hsChan, tReq, tPartId );
			break;
		case MCU_MCU_DELMT_REQ:
			OnSendDelMtReq(cServMsg, hsChan, tReq, tPartId );
			break;
		case MCU_MCU_SETIN_REQ:
			OnSendSetInReq( cServMsg, hsChan, tReq, tPartId );
			break;
		default:
			MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "Handle Unknow Request Message. EventId(%d)(%d)\n",
				cServMsg.GetEventId(),::OspEventDesc(cServMsg.GetEventId())
				);	
			break;
	}
}
/*=============================================================================
函 数 名： OnSendMultiCascadeRspMsg
功    能： 预处理多级联响应信息
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20100715                周晶晶                   create
=============================================================================*/
void CMtAdpInst::OnSendMultiCascadeRspMsg( CServMsg &cServMsg, HCHAN hsChan )
{
	TMsgHeadMsg *ptHeadMsg = (TMsgHeadMsg*)cServMsg.GetMsgBody();
	TMt tMt = *(TMt*)( cServMsg.GetMsgBody()+sizeof(TMsgHeadMsg) );
	TPartRsp tRsp;	

	//TMt tMt = ptHeadMsg->m_tMsgSrc.m_tMt;
	if( LOCAL_MCUID != tMt.GetMcuId() )
	{		
		tRsp.m_tMsgSrc.m_abyMtIdentify[tRsp.m_tMsgSrc.m_byCasLevel] = tMt.GetMtId();
		++tRsp.m_tMsgSrc.m_byCasLevel;

		tMt.SetMtId( (u8)tMt.GetMcuId() );
		tMt.SetMcuId( LOCAL_MCUID );
	}

	tRsp.m_dwPID = GetPartIdFromMt( tMt,&tRsp.m_bLocal );
	
	tRsp.m_nReqID      = GetPeerReqId();
	switch( cServMsg.GetEventId() )
	{
		case MCU_MCU_INVITEMT_ACK:		
		case MCU_MCU_REINVITEMT_ACK:		
		case MCU_MCU_DROPMT_ACK:		
		case MCU_MCU_DELMT_ACK:		
		case MCU_MCU_SETIN_ACK:
			tRsp.m_emReturnVal = emReturnValue_Ok;
			break;
		case MCU_MCU_INVITEMT_NACK:
		case MCU_MCU_REINVITEMT_NACK:
		case MCU_MCU_DROPMT_NACK:
		case MCU_MCU_DELMT_NACK:
		case MCU_MCU_SETIN_NACK:
			tRsp.m_emReturnVal = emReturnValue_Error;
			break;
		default:
			MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "Handle Unknow Response Message. EventId(%d)(%d)\n",
				cServMsg.GetEventId(),::OspEventDesc(cServMsg.GetEventId())
				);	
			break;
	}

	tRsp.m_tMsgDst.m_byCasLevel = ptHeadMsg->m_tMsgDst.m_byCasLevel;
	memcpy( &tRsp.m_tMsgDst.m_abyMtIdentify[0],
			&ptHeadMsg->m_tMsgDst.m_abyMtIdentify[0],
			sizeof(tRsp.m_tMsgDst.m_abyMtIdentify)
			);

	tRsp.m_tMsgSrc.m_byCasLevel = ptHeadMsg->m_tMsgSrc.m_byCasLevel;
	memcpy( &tRsp.m_tMsgSrc.m_abyMtIdentify[0],
			&ptHeadMsg->m_tMsgSrc.m_abyMtIdentify[0],
			sizeof(tRsp.m_tMsgSrc.m_abyMtIdentify)
			);

	
	if( tRsp.m_tMsgDst.m_byCasLevel > 0 )
	{
		--tRsp.m_tMsgDst.m_byCasLevel;
	}

	u16 wMsgType = 0;
	
	switch( cServMsg.GetEventId() )
	{
		case MCU_MCU_INVITEMT_ACK:	
		case MCU_MCU_INVITEMT_NACK:
			wMsgType = (u16)H_CASCADE_INVITEPART_RSP;
			break;
		case MCU_MCU_REINVITEMT_ACK:
		case MCU_MCU_REINVITEMT_NACK:
			wMsgType = (u16)H_CASCADE_REINVITEPART_RSP;
			break;
		case MCU_MCU_DROPMT_ACK:
		case MCU_MCU_DROPMT_NACK:
			wMsgType = (u16)H_CASCADE_DISCONNPART_RSP;
			break;
		case MCU_MCU_DELMT_ACK:	
		case MCU_MCU_DELMT_NACK:
			wMsgType = (u16)H_CASCADE_DELETEPART_RSP;
			break;
		case MCU_MCU_SETIN_ACK:
		case MCU_MCU_SETIN_NACK:
			wMsgType = (u16)H_CASCADE_SETIN_RSP;
			break;
		default:
			break;
	}

	if( 0 != wMsgType )
	{
		kdvSendMMcuCtrlMsg( hsChan, wMsgType, &tRsp, sizeof(tRsp) );
	}
	
}
/*=============================================================================
函 数 名： OnSendMultiCascadeNtfMsg
功    能： 预处理多级联通知信息
算法实现： 
全局变量： 
参    数： CServMsg& cServMsg
           HCHAN hsChan
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20100715                周晶晶                   create
=============================================================================*/
void CMtAdpInst::OnSendMultiCascadeNtfMsg( CServMsg &cServMsg, HCHAN hsChan )
{
	TMsgHeadMsg *ptHeadMsg = NULL;
	TMt tMt;
	tMt.SetNull();
	if( MCU_MCU_CALLALERTING_NOTIF == cServMsg.GetEventId() )
	{
		ptHeadMsg = (TMsgHeadMsg*)( cServMsg.GetMsgBody() + sizeof(TMtAlias) + sizeof(TMt) );
		tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMtAlias) );
	}
	else if( MCU_MCU_SETOUT_NOTIF == cServMsg.GetEventId() )
	{
		ptHeadMsg = (TMsgHeadMsg*)( cServMsg.GetMsgBody() + sizeof(TSetOutParam) );
		TSetOutParam *ptSetOutParam = (TSetOutParam*)cServMsg.GetMsgBody();
		ptHeadMsg->m_tMsgDst.m_byCasLevel++;
		tMt = ptSetOutParam->m_atConfViewOutInfo[0].m_tMt;
	}
	else
	{
		ptHeadMsg = (TMsgHeadMsg*)( cServMsg.GetMsgBody() + sizeof(TMt) );
		tMt = *(TMt*)cServMsg.GetMsgBody();
	}
	
	TMt tOrgMt = tMt;
	TPartID tPartId;

	//tMt = ptHeadMsg->m_tMsgSrc.m_tMt;
	if( !tMt.IsMcuIdLocal() && tMt.GetMtId() != 0 )	
	{
		tMt.SetMtId( (u8)tMt.GetMcuId() );
		tMt.SetMcuId( LOCAL_MCUID );
	}
	tPartId.m_dwPID = GetPartIdFromMt( tMt,&tPartId.m_bLocal );
	
	
//	s32 nViewID = 0;
	switch( cServMsg.GetEventId() )
	{		
		case MCU_MCU_DROPMT_NOTIF:
			{
				TDiscPartNF tNF;
				tNF.m_dwPID = tPartId.m_dwPID;
				tNF.m_bLocal = tPartId.m_bLocal;
				tNF.m_dwNewPID = tNF.m_dwPID;

				tNF.m_tMsgDst.m_byCasLevel = ptHeadMsg->m_tMsgDst.m_byCasLevel;
				memcpy( &tNF.m_tMsgDst.m_abyMtIdentify[0],
					&ptHeadMsg->m_tMsgDst.m_abyMtIdentify[0],
					sizeof(tNF.m_tMsgDst.m_abyMtIdentify)
					);

				tNF.m_tMsgSrc.m_byCasLevel = ptHeadMsg->m_tMsgSrc.m_byCasLevel;
				memcpy( &tNF.m_tMsgSrc.m_abyMtIdentify[0],
					&ptHeadMsg->m_tMsgSrc.m_abyMtIdentify[0],
					sizeof(tNF.m_tMsgSrc.m_abyMtIdentify)
					);

				if( LOCAL_MCUID != tOrgMt.GetMcuId() )
				{
					tNF.m_tMsgSrc.m_abyMtIdentify[tNF.m_tMsgSrc.m_byCasLevel] = tOrgMt.GetMtId();
					++tNF.m_tMsgSrc.m_byCasLevel;
				}
				if( tNF.m_tMsgDst.m_byCasLevel > 0 )
				{
					--tNF.m_tMsgDst.m_byCasLevel;
				}

				LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[OnSendMultiCascadeNtfMsg]drop mt<%d,%d> lvl.%d\n",
					tOrgMt.GetMtId(), tOrgMt.GetMtId(), tNF.m_tMsgSrc.m_byCasLevel);
				kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_DISCONNPART_NTF, &tNF, sizeof(tNF));
			}
			break;
		case MCU_MCU_DELMT_NOTIF:
			{
				TDelPartNF tNF;
				tNF.m_dwPID = tPartId.m_dwPID;
				tNF.m_bLocal = tPartId.m_bLocal;
				tNF.m_tMsgDst.m_byCasLevel = ptHeadMsg->m_tMsgDst.m_byCasLevel;
				memcpy( &tNF.m_tMsgDst.m_abyMtIdentify[0],
					&ptHeadMsg->m_tMsgDst.m_abyMtIdentify[0],
					sizeof(tNF.m_tMsgDst.m_abyMtIdentify)
					);

				tNF.m_tMsgSrc.m_byCasLevel = ptHeadMsg->m_tMsgSrc.m_byCasLevel;
				memcpy( &tNF.m_tMsgSrc.m_abyMtIdentify[0],
					&ptHeadMsg->m_tMsgSrc.m_abyMtIdentify[0],
					sizeof(tNF.m_tMsgSrc.m_abyMtIdentify)
					);

				if( LOCAL_MCUID != tOrgMt.GetMcuId() )
				{
					tNF.m_tMsgSrc.m_abyMtIdentify[tNF.m_tMsgSrc.m_byCasLevel] = tOrgMt.GetMtId();
					++tNF.m_tMsgSrc.m_byCasLevel;
				}
				if( tNF.m_tMsgDst.m_byCasLevel > 0 )
				{
					--tNF.m_tMsgDst.m_byCasLevel;
				}

				LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[OnSendMultiCascadeNtfMsg]drop mt<%d,%d> lvl.%d\n",
					tOrgMt.GetMtId(), tOrgMt.GetMtId(), tNF.m_tMsgSrc.m_byCasLevel);

				kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_DELETEPART_NTF, &tNF, sizeof(tNF) );
			}
			break;
		case MCU_MCU_CALLALERTING_NOTIF:
			{
				TMtAlias *ptMtAlias = (TMtAlias *)cServMsg.GetMsgBody();
				TCallNtf  tNF;
				tNF.m_dwPID = tPartId.m_dwPID;
				tNF.m_bLocal = tPartId.m_bLocal;				
				
				MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "[OnSendMultiCascadeNtfMsg] send MCU_MCU_CALLALERTING_NOTIF 0x%08X\n",
					tNF.m_dwPID
					);

				if( (u8)mtAliasTypeTransportAddress != ptMtAlias->m_AliasType )
				{
					s8 achMtAlias[MAXLEN_ALIAS]; //pengguofeng 多国语言
					memset(achMtAlias, 0, sizeof(achMtAlias));
					BOOL32 bNeedTrans = TransEncoding(ptMtAlias->m_achAlias, achMtAlias, sizeof(achMtAlias));
					if ( bNeedTrans == TRUE )
					{
						tNF.SetPartCallingParam( tNF.m_dwPID, tNF.m_bLocal, achMtAlias, sizeof(achMtAlias) );
					}
					else
					{
						tNF.SetPartCallingParam( tNF.m_dwPID, tNF.m_bLocal, ptMtAlias->m_achAlias, sizeof(ptMtAlias->m_achAlias) );
					}
				}
				else
				{
					s8   szDialIP[16];
					CMtAdpUtils::Ipdw2Str( ptMtAlias->m_tTransportAddr.GetNetSeqIpAddr(), szDialIP );
					tNF.SetPartCallingParam( tNF.m_dwPID, tNF.m_bLocal, szDialIP, sizeof(szDialIP) );
				}
				tNF.m_tMsgDst.m_byCasLevel = ptHeadMsg->m_tMsgDst.m_byCasLevel;
				memcpy( &tNF.m_tMsgDst.m_abyMtIdentify[0],
					&ptHeadMsg->m_tMsgDst.m_abyMtIdentify[0],
					sizeof(tNF.m_tMsgDst.m_abyMtIdentify)
					);

				tNF.m_tMsgSrc.m_byCasLevel = ptHeadMsg->m_tMsgSrc.m_byCasLevel;
				memcpy( &tNF.m_tMsgSrc.m_abyMtIdentify[0],
					&ptHeadMsg->m_tMsgSrc.m_abyMtIdentify[0],
					sizeof(tNF.m_tMsgSrc.m_abyMtIdentify)
					);

				if( LOCAL_MCUID != tOrgMt.GetMcuId() )
				{
					tNF.m_tMsgSrc.m_abyMtIdentify[tNF.m_tMsgSrc.m_byCasLevel] = tOrgMt.GetMtId();
					++tNF.m_tMsgSrc.m_byCasLevel;
				}
				if( tNF.m_tMsgDst.m_byCasLevel > 0 )
				{
					--tNF.m_tMsgDst.m_byCasLevel;
				}

				kdvSendMMcuCtrlMsg(hsChan, (u16)H_CASCADE_CALL_ALERTING_NTF, &tNF, sizeof(tNF) );
			}
			break;
		case MCU_MCU_SETOUT_NOTIF:
			{
				TSetOutParam   *ptParam = (TSetOutParam *)cServMsg.GetMsgBody();				
				TPartOutputNtf  tNF;	
				if( ptParam->m_nMtCount == 1 )
				{
					tNF.m_dwPID   = tPartId.m_dwPID;
					tNF.m_bLocal  = tPartId.m_bLocal;
					tNF.m_nViewID = ptParam->m_atConfViewOutInfo[0].m_nOutViewID;
					tNF.m_tMsgDst.m_byCasLevel = ptHeadMsg->m_tMsgDst.m_byCasLevel;
					memcpy( &tNF.m_tMsgDst.m_abyMtIdentify[0],
						&ptHeadMsg->m_tMsgDst.m_abyMtIdentify[0],
						sizeof(tNF.m_tMsgDst.m_abyMtIdentify)
						);

					tNF.m_tMsgSrc.m_byCasLevel = ptHeadMsg->m_tMsgSrc.m_byCasLevel;
					memcpy( &tNF.m_tMsgSrc.m_abyMtIdentify[0],
						&ptHeadMsg->m_tMsgSrc.m_abyMtIdentify[0],
						sizeof(tNF.m_tMsgSrc.m_abyMtIdentify)
						);
					tNF.m_tMsgDst.m_byCasLevel = ptHeadMsg->m_tMsgDst.m_byCasLevel;
					if( LOCAL_MCUID != tOrgMt.GetMcuId() && tMt.GetMtId() != 0 )
					{
						tNF.m_tMsgSrc.m_abyMtIdentify[tNF.m_tMsgSrc.m_byCasLevel] = tOrgMt.GetMtId();
						++tNF.m_tMsgSrc.m_byCasLevel;
					}
					kdvSendMMcuCtrlMsg( hsChan, (u16)H_CASCADE_OUTPUT_NTF, &tNF, sizeof(tNF) );
				}				
			}
			break;
		default:
			break;
	}


}


void CMtAdpInst::HeadMsgToMtadpHeadMsg( const TMsgHeadMsg &tHeadMsg ,TMtadpHeadMsg &tMtadpHeadMsg )
{
	memcpy( &tMtadpHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
			&tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
			sizeof(tMtadpHeadMsg.m_tMsgSrc.m_abyMtIdentify)
			);
	memcpy( &tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
			&tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
			sizeof(tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify)
			);

	tMtadpHeadMsg.m_tMsgSrc.m_byCasLevel = tHeadMsg.m_tMsgSrc.m_byCasLevel;
	tMtadpHeadMsg.m_tMsgDst.m_byCasLevel = tHeadMsg.m_tMsgDst.m_byCasLevel;
}

void CMtAdpInst::MtadpHeadMsgToHeadMsg( const TMtadpHeadMsg &tMtadpHeadMsg ,TMsgHeadMsg &tHeadMsg )
{
	memcpy( &tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
			&tMtadpHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
			sizeof(tHeadMsg.m_tMsgSrc.m_abyMtIdentify)
			);
	memcpy( &tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
			&tMtadpHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
			sizeof(tHeadMsg.m_tMsgDst.m_abyMtIdentify)
			);

	tHeadMsg.m_tMsgSrc.m_byCasLevel = tMtadpHeadMsg.m_tMsgSrc.m_byCasLevel;
	tHeadMsg.m_tMsgDst.m_byCasLevel = tMtadpHeadMsg.m_tMsgDst.m_byCasLevel;
}

/*====================================================================
    函数名      : SetEndPointEncoding
    功能        : 存储对端实体编码方式
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  :
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2013/04/08              liaokang      创建
====================================================================*/
void CMtAdpInst::SetEndPointEncoding(emenCodingForm emEPCodingForm)
{
    m_emEndpointEncoding = emEPCodingForm;
}

/*====================================================================
    函数名      : GetEndPointEncoding
    功能        : 获取对端实体编码方式
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  : emCodingForm
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2013/04/08              liaokang      创建
====================================================================*/
emenCodingForm CMtAdpInst::GetEndPointEncoding()
{
    return m_emEndpointEncoding;
}

/*----------Global function-------------*/

/*=============================================================================
    函 数 名： MtAdpStart
    功    能： 开启MtAdp
    算法实现： 
    全局变量： 
    参    数： TMtAdpConnectParam *ptConnectParam
               TMtAdp *ptMtadp
               TMtAdpConnectParam *ptConnectParam2 = NULL（when doublelinking is used）
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2003/11     1.0         TanGuang              创建
    2005/9/10   4.0			万春雷                增加双层建链支持
=============================================================================*/
BOOL32 MtAdpStart( TMtAdpConnectParam *ptConnectParam, 
				   TMtAdp *ptMtadp, 
				   TMtAdpConnectParam *ptConnectParam2 /*= NULL*/,u8 *pabyBrdEthMac/*= NULL*/)
{
	if (NULL == ptConnectParam || NULL == ptMtadp)
	{
		StaticLog( "Param is invalid, MtAdpStart failed.\n" );
		return FALSE;
	}
	if (!IsOspInitd())
	{
		if (!OspInit())
		{
			StaticLog( "Osp starting up failed.\n" );
			return FALSE;
		}
	}
	if (TRUE == g_cMtAdpApp.m_bMtAdpInited)
	{
		StaticLog( "MtAdapter already initialized before.\n" );
		return TRUE;
	}
	
	//保存MAC地址信息 [5/16/2012 chendaiwei]
	if(pabyBrdEthMac != NULL)
	{
		memcpy(g_cMtAdpApp.m_abyCRIMacAddr,pabyBrdEthMac,sizeof(g_cMtAdpApp.m_abyCRIMacAddr));
	}
    
//     #ifdef OSPEVENT
//     #undef OSPEVENT
//     #endif
//     
//     #define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
 
//[20110801 zhushz]pc-lint err:537    
//     #ifdef _EV_MCUMT_H_
//     #undef _EV_MCUMT_H_
//     #include "evmcumt.h"
//     #else
//     #include "evmcumt.h"
//     #endif
    
//     #ifdef _EV_MCU_H_
//     #undef _EV_MCU_H_
//     #include "evmcu.h"
//     #else
//     #include "evmcu.h"
//     #endif

	//MtAdp base info
	g_cMtAdpApp.m_bAttachedToVc = ( ptMtadp->GetAttachMode() == 1 ? TRUE : FALSE );
	g_cMtAdpApp.m_byDriId       = ( ptMtadp->GetMtadpId() != 0 ? ptMtadp->GetMtadpId() : 1 );
	g_cMtAdpApp.m_dwMtAdpIpAddr = htonl(ptMtadp->GetIpAddr());
	sprintf( g_cMtAdpApp.m_achMtAdpAlias, "MtAdp%d", g_cMtAdpApp.m_byDriId );

	//MtAdp单链时(包括attach方式)的mcu连接信息 或者双链连接时其中一个mcu连接信息
	g_cMtAdpApp.m_dwMcuIpAddr = ptConnectParam->GetMcuIpAddr();
	g_cMtAdpApp.m_wMcuPort    = ptConnectParam->m_wMcuPort;
	if (OspIsValidTcpNode(ptConnectParam->m_dwMcuTcpNode))
	{
		g_cMtAdpApp.m_dwMcuNode = ptConnectParam->m_dwMcuTcpNode;
	}


	//双链连接时其中一个mcu连接信息
	if (!g_cMtAdpApp.m_bAttachedToVc && NULL != ptConnectParam2)
	{
		g_cMtAdpApp.m_bDoubleLink  = TRUE;
		g_cMtAdpApp.m_dwMcuIpAddrB = ptConnectParam2->GetMcuIpAddr();
		g_cMtAdpApp.m_wMcuPortB    = ptConnectParam2->m_wMcuPort;
		if (OspIsValidTcpNode(ptConnectParam2->m_dwMcuTcpNode))
		{
			g_cMtAdpApp.m_dwMcuNodeB = ptConnectParam2->m_dwMcuTcpNode;
		}

	}

    g_cMtAdpApp.GetCallDataFromDebugFile( g_cMtAdpApp.m_wRasPort, 
                                          g_cMtAdpApp.m_wQ931Port,
                                          g_cMtAdpApp.m_byMaxCallNum );
	
	//zbq[10/30/2008] 获取高清双流dbg能力
	g_cMtAdpApp.GetMtDSDbgInfoFromDbgFile();

	// xliang [3/9/2009] 获取是否采取切包
	g_cMtAdpApp.GetMtSliceInfoFromDbgFile();

	//zjj [08/27/2009] 获取泰德终端无法取得双流能力时是否作弊
	g_cMtAdpApp.GetTaideDsSupport();
	//  [12/11/2009 pengjie] 是否响应GK发来的DRQ
	g_cMtAdpApp.GetIsRespDRQFormGK();
    
    //[20120208 zhushz]获取呼叫受限制的厂商列表
    g_cMtAdpApp.GetRestrictManu();

	g_cMtAdpApp.ReadFakedProductId();

	//获取H224打洞时间
	g_cMtAdpApp.GetPinHoleTime();
	
#ifdef _IS22_
	g_cMtAdpApp.ReadDiscHeartBeatParam();
#endif

#ifndef WIN32

	s8 achKeyFileName[MAX_PATH] = {0};
    sprintf(achKeyFileName, "%s/%s", DIR_DATA, KEY_FILENAME);

	
	//  [8/4/2011 chendaiwei]MPC内嵌的接入，高清接入能力为2
#ifdef _MPC_EMBEDDED_MTADP_
	g_cMtAdpApp.m_byMaxHDMtNum = MAXHDLIMIT_MPC_MTADP;
#else

	BOOL32 bReadLicense = FALSE;
	bReadLicense = g_cMtAdpApp.GetLicenseDataFromFile( achKeyFileName );

	// [miaoqingsong 2011/08/16] 添加点CRI2板FUN1灯和FUN7灯，点灯触发条件如下：
	// 1、CRI2检测到高清接入的License授权文件并且License授权数大于0时，点亮FUN7灯；
    // 2、CRI2未检测到高清接入的License授权文件或License授权数等于0时，点亮FUN1灯。
#if defined _LINUX_ && defined _LINUX12_ && !defined(_IS22_)
	if( bReadLicense == FALSE || g_cMtAdpApp.m_byMaxHDMtNum == 0 )
	{
		BrdLedStatusSet(LED_FUN_ID1, BRD_LED_ON);
		BrdLedStatusSet(LED_FUN_ID7, BRD_LED_OFF);
	}
	
	if( g_cMtAdpApp.m_byMaxHDMtNum > 0 )
	{
		BrdLedStatusSet(LED_FUN_ID7, BRD_LED_ON);
		BrdLedStatusSet(LED_FUN_ID1, BRD_LED_OFF);
	}
#endif

#endif

#ifdef _IS22_
	//  [7/29/2011 chendaiwei]IS2.2的接入能力为192个终端
	g_cMtAdpApp.m_wMaxNumConntMt = MAXNUM_H225H245_MT;
#else
	//  [7/29/2011 chendaiwei]CRI,CRI2的接入能力为112个终端
	g_cMtAdpApp.m_wMaxNumConntMt = MAXLIMIT_HDI_MTADP;
#endif
	
	
#else
	//  [7/28/2011 chendaiwei]win32的高清接入数应该和接入数一致
	g_cMtAdpApp.m_byMaxHDMtNum = MAXNUM_H225H245_MT;
	g_cMtAdpApp.m_wMaxNumConntMt = MAXNUM_H225H245_MT;
#endif

#if defined (WIN32) || defined(_8KH_) || defined(_8KI_)
	g_cMtAdpApp.m_wMaxNumConnAudMt = MAXNUM_CONNTAUD_MT;
#endif

    // zbq [04/05/2007] 分批呼叫策略：同时可呼叫点数
    if ( g_cMtAdpApp.m_byMaxCallNum > MAX_NUM_CALLOUT ) 
    {
        g_cMtAdpApp.m_byMaxCallNum = MAX_NUM_CALLOUT;
    }
    else if ( g_cMtAdpApp.m_byMaxCallNum < DEFAULT_NUM_CALLOUT) 
    {
        g_cMtAdpApp.m_byMaxCallNum = DEFAULT_NUM_CALLOUT;
    }

    // 终端呼叫队列和等待队列空间分配
    g_cMtAdpApp.m_tCallQueue.Init( g_cMtAdpApp.m_byMaxCallNum );
    g_cMtAdpApp.m_tWaitQueue.Init( MAXNUM_DRI_MT );

    u32 dwMcuAIp = ntohl(g_cMtAdpApp.m_dwMcuIpAddr);
    u32 dwMcuBIp = ntohl(g_cMtAdpApp.m_dwMcuIpAddrB);
    StaticLog("[MtAdpStart] bDoubleLink.%d IPA(%u.%u.%u.%u) IPB(%u.%u.%u.%u), Port RAS= %d, Q931= %d, byMaxCallNum= %d.\n", 
                            g_cMtAdpApp.m_bDoubleLink, QUADADDR(dwMcuAIp), QUADADDR(dwMcuBIp),
                            g_cMtAdpApp.m_wRasPort, g_cMtAdpApp.m_wQ931Port, g_cMtAdpApp.m_byMaxCallNum);
	
	//enlarge the stack size to accomodate more levels of function calls on real-address systems.
	g_cMtAdpApp.CreateApp( "MtAdp", AID_MCU_MTADP, APPPRI_MTADP, 5000, 2<<20);
	
#if !defined(WIN32)
	g_cMtAdpApp.m_byDebugLevel = DEBUG_ERROR;
#else
	g_cMtAdpApp.m_byDebugLevel = g_cMtAdpApp.m_bAttachedToVc ? DEBUG_ERROR : DEBUG_INFO;
#endif

#ifdef _LINUX_
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
	WDStartWatchDog( emMTADP );
#endif
#endif
	
    //LINUX下需将API注册
    MtAdpAPIEnableInLinux();
    
	OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), OSP_POWERON );    
	
    	
	return TRUE;
}

/*=============================================================================
    函 数 名： MtAdpStop
    功    能： 停止MtAdp
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void  
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/9/10   4.0			万春雷                  创建
=============================================================================*/
void MtAdpStop()
{
	g_cMtAdpApp.m_bMtAdpInited = FALSE;
	g_cMtAdpApp.m_hApp = NULL;
	g_cMtAdpApp.ClearMtAdpData();
	kdvCloseH323Adapter();
#ifndef _8KH_
	OspQuit();
#endif
}

/*=============================================================================
  函 数 名： BODY_LEN_EQ
  功    能： check if body len of CServMsg is equal to len
  算法实现： 
  全局变量： 
  参    数： CServMsg &cServMsg
             u16 wLen
  返 回 值： inline BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         谭光                    创建
=============================================================================*/
BOOL32 BODY_LEN_EQ( CServMsg &cServMsg, u16 wLen )
{
	if(cServMsg.GetMsgBodyLen() != wLen)
	{
		const s8* pszMsgStr = OspEventDesc(cServMsg.GetEventId());		
		OspLog(LOGLVL_EXCEPTION, "Error body len %u for msg %u(%s) detected: "
				"Should be %u bytes. IGNORE it!\n", cServMsg.GetMsgBodyLen(),
				cServMsg.GetEventId(),	pszMsgStr ? pszMsgStr : "null",	wLen);						
		return FALSE;
	}
	return TRUE;
}

/*=============================================================================
  函 数 名： BODY_LEN_GE
  功    能： check if the body len of CServMsg >= len
  算法实现： 
  全局变量： 
  参    数： CServMsg &cServMsg
             u16 wLen
  返 回 值： inline BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         谭光                    创建
=============================================================================*/
BOOL32 BODY_LEN_GE(CServMsg &cServMsg, u16 wLen)
{
	if(cServMsg.GetMsgBodyLen() < wLen)
	{
		const s8* pszMsgStr = OspEventDesc(cServMsg.GetEventId());			
		OspLog(LOGLVL_EXCEPTION, "Error body len %u for msg %u(%s) detected:  "
				"Should >= %u. IGNORE it!\n", cServMsg.GetMsgBodyLen(), 
				cServMsg.GetEventId(),	pszMsgStr ? pszMsgStr : "null",	wLen);
									
		return FALSE;
	}
	return TRUE;
}

/*************** API ****************/

/*=============================================================================
  函 数 名： mtadphelp
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
API void mtadphelp()
{	
	StaticLog( "\tmtadpver : show version info.\n");	
// 	StaticLog( "\tmadebug  : set debug level(0 - 5).\n");
	StaticLog( "\tmacall   : show calls info.\n");
	StaticLog( "\tmaras    : show RAS info.\n");
    StaticLog( "\tmastatus : show mtadp inner status \n" );
    StaticLog( "\tmacharge : show conf charge info \n" );
    StaticLog( "\tmaclear  : inner command....\n");
    StaticLog( "\tmasize   : inner command....\n");
    StaticLog( "\tmatau(UsrName,Pwd): mtadp telnet author.\n");
	StaticLog( "\tsetautosend: enable send partlist req when setin.\n");
	StaticLog( "\tsetsendmmcu: enable send (partlist audioin videoin)rsp \n");
	StaticLog( "\tsettimeoutspan: set send partlist span \n");
    StaticLog( "\tsq       : show current call queue and wait queue.\n" );
    StaticLog( "\tpq       : show real time call queue and wait queue info.\n" );
	StaticLog( "\tmamtcall : show mt is or not neglect invite msg.\n" );
    StaticLog( "\tpmamsg   : open mtadp print.\n");
    StaticLog( "\tnpmsmsg  : close mtadp print.\n");
}

/*=============================================================================
  函 数 名： mtadpver
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
API void mtadpver() 
{
	static s8 gs_VersionBuf[128] = {0};
	
    strcpy(gs_VersionBuf, KDV_MCU_PREFIX);
    
    s8 achMon[16] = {0};
    u32 byDay = 0;
    u32 byMonth = 0;
    u32 wYear = 0;
    static s8 achFullDate[24] = {0};
    
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
        sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
    else
    {
        // for debug information
        sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
	StaticLog( "MtAdp Version: %s\n", gs_VersionBuf  );
	StaticLog( "MtAdp Module Version: %s. Compile Time: %s, %s\n", VER_MTADP, __TIME__, __DATE__);
	kdv323adptver();}

/*=============================================================================
  函 数 名： madebug
  功    能： 
  算法实现： 
  全局变量： 
  参    数： s32 nDbgLvl
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
API void madebug( s32 nDbgLvl )
{
// 	if( nDbgLvl < DEBUG_CRITICAL )
// 	{
// 		nDbgLvl = DEBUG_CRITICAL;
// 	}
// 	if( nDbgLvl > DEBUG_DETAIL )
// 	{
// 		nDbgLvl = DEBUG_DETAIL;
// 	}
 	g_cMtAdpApp.m_byDebugLevel = nDbgLvl;

    StaticLog(" This command was invalid!\n");
    StaticLog(" please use <pmamsg/npmamsg> to open/close mtadp log!\n");
}

/*=============================================================================
函 数 名： pmamsg
功    能： 放开mtadp模块打印
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2011/12       1.0         zhushz                创建
=============================================================================*/
API void pmamsg( s32 nDbgLvl )
{
    logenablemod(MID_MCULIB_MTADP);
}
/*=============================================================================
函 数 名： npmamsg
功    能： 关闭mtadp模块打印 
算法实现： 
全局变量： 
参    数： void
返 回 值： API void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2011/12       1.0         zhushz                创建
=============================================================================*/
API void npmamsg( void )
{
    logdisablemod(MID_MCULIB_MTADP);
}

/*=============================================================================
  函 数 名： macall
  功    能： 打印所有呼叫信息
  算法实现： 
  全局变量： 
  参    数： s8 * param，打印参数，
					C 打印所有呼叫及信道信息
					c 打印所有呼叫信息
					b 打印呼叫统计信息
					无 打印帮助信息
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
API void macall( u8 byChanInfo )
{	
    StaticLog( "\n" );
    StaticLog( "InsId ConfIdx MtId        MtIpAddr   ConfState HsCall      ProductId\n" );
    StaticLog( "--------------------------------------------------------------------\n" );

	OspInstDump( AID_MCU_MTADP, CInstance::EACH, byChanInfo );
    
    return;
}

/*=============================================================================
  函 数 名： maclear
  功    能： drop call and clear instance
  算法实现： 
  全局变量： 
  参    数： s32 inst
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
API void maclear( s32 nInst )
{
	if( nInst > 0 && nInst < MAXNUM_DRI_MT )
	{
		OspPost( MAKEIID(AID_MCU_MTADP, nInst), CLEAR_INSTANCE );	
	}
	if( nInst == 0 )
	{
		OspPost( MAKEIID(AID_MCU_MTADP, CInstance::EACH), CLEAR_INSTANCE );
	}
}

/*=============================================================================
  函 数 名： maras
  功    能： print ras information
  算法实现： 
  全局变量： 
  参    数： s8 * pchIpStr
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
API void maras( s8 * pchIpStr)
{
	u32 dwIp = g_cMtAdpApp.m_tGKAddr.ip;

	if( dwIp == 0 || !g_cMtAdpApp.m_bGkAddrSet )
	{
		StaticLog( "\nRAS Disabled.ip = %u.%u.%u.%u, GkAddrSet:%d\n", QUADADDR(dwIp), g_cMtAdpApp.m_bGkAddrSet);
	}
	else if( !g_cMtAdpApp.m_bGkRegistered )
	{
		StaticLog( "\nRAS Enabled, but GK(%u.%u.%u.%u) not registered bMasterMtAdp.%d\n",
				   QUADADDR(dwIp), g_cMtAdpApp.m_bMasterMtAdp );
	}
	else
	{
		StaticLog( "\nRAS Enabled, and GK(%u.%u.%u.%u) registered bMasterMtAdp.%d\n", 
				   QUADADDR(dwIp), g_cMtAdpApp.m_bMasterMtAdp );
        const s8 * pchStateName[] = {"state null", "register proceeding", "register succeeded"};

        GKREG_STATE emState = g_cMtAdpApp.m_gkConfRegState[0];
        
        //zbq [11/20/2007] 辅接入板没有获取MCU的H323ID的资格
        if ( g_cMtAdpApp.m_bMasterMtAdp )
        {
            if( emState >= STATE_NULL && emState <= STATE_REG_OK )
            {
                StaticLog( "0. alias %s: \t%s !\n", 
                                         g_cMtAdpApp.m_tMcuH323Id.m_achAlias, pchStateName[emState]);
            }
        }

		for( s32 nAlias = 0/*, nAliasIdx = 0*/; nAlias < MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE + 1; nAlias ++ )
		{	
			if( g_cMtAdpApp.m_atMcuAlias[nAlias].m_AliasType == 0 )
			{
				continue;
			}
//            nAliasIdx ++;
			// STATE_NULL,  STATE_REG_INPROGRESS, 
			// STATE_UNREG_INPROGRESS, STATE_REG_OK

			emState = g_cMtAdpApp.m_gkConfRegState[nAlias];
			if( emState >= STATE_NULL && emState <= STATE_REG_OK )
			{
				StaticLog( "%d. alias %s: \t%s !\n", nAlias, 
						   g_cMtAdpApp.m_atMcuAlias[nAlias].m_achAlias, pchStateName[emState]);
			}
		}
	}
	//  [12/11/2009 pengjie] Modify 打印是否响应GK的DRQ消息，1：相应，0，不响应
	StaticLog( "Mcu Set IsRespDRQFromGK = %d\n", g_cMtAdpApp.m_bIsRespDRQFromGK );
}

/*=============================================================================
  函 数 名： masize
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
API void masize()
{
	StaticLog( "MtAdp Instance size = %d Bytes, Application size = %d KBytes\n",
	           sizeof(CMtAdpInst), sizeof(CMtAdpApp) / 1024 ); 
}


/*=============================================================================
  函 数 名： sendev
  功    能： send an event in form of u16 -- For manual testing
  算法实现： 
  全局变量： 
  参    数： u16 wEvent
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
API void sendev( u16 wEvent )
{
	StaticLog( "Send message %u to MCU..\n", wEvent);

	TMtExt tMtExt;
	tMtExt.SetIPAddr( g_cMtAdpApp.m_dwMtAdpIpAddr );
	tMtExt.SetMt( 1, 1 );
	
	CServMsg cServMsg;
	cServMsg.SetEventId(wEvent);
	cServMsg.SetConfIdx(1);
	cServMsg.SetDstMtId(1);
	cServMsg.SetMsgBody((u8*)&tMtExt, sizeof(tMtExt));
	
	OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
}

/*=============================================================================
  函 数 名： sendevmsg
  功    能： send a message in form of string -- For manual testing
  算法实现： 
  全局变量： 
  参    数： s8* pchMsg
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
API void sendevmsg( s8* pszMsg )
{	
	if( NULL == pszMsg ) 
		return;
	
	for( s32 nEvent = EV_MCUMT_BGN; nEvent < EV_MCUMT_END; nEvent ++ )
	{
		if( OspEventDesc(nEvent) && strcmp(OspEventDesc(nEvent), pszMsg) == 0 )
		{
			sendev(nEvent);
			return;
		}
	}	
	StaticLog( "%s Unknow message.\n", pszMsg );
}
 
/*=============================================================================
  函 数 名： listmsg
  功    能： list messages
  算法实现： 
  全局变量： 
  参    数：  s32 nStartMsg = 0
              s32 nLines = 0
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
API void listmsg( s32 nStartMsg = 0, s32 nLines = 0 )
{
	static u16 wBase = EV_MCUMT_BGN;
	static u16 wItem = 1;
	s32 nItemNum;
		
	if( 0 != nStartMsg )
	{
		wBase = nStartMsg;
		wItem = 1;
	}

	if( 0 != nLines )
	{
		nItemNum = nLines;
	}
	else 
	{
		nItemNum = 20;
	}
	s32 nCount = 0;
	
	for(; wBase < EV_MCUMT_END; wBase ++ )
	{
		if( OspEventDesc(wBase) != NULL )
		{
			StaticLog( "%u.\t%u\t(%s)\n", wItem, wBase, OspEventDesc(wBase) );
			wItem  ++;
			nCount ++;
		}
		if( nCount == nItemNum )
		{
			break;
		}
	}	
}

/*=============================================================================
  函 数 名： findmsg
  功    能： find messages containing specified substring 
  算法实现： 
  全局变量： 
  参    数： const s8* pchCondition
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
API void findmsg(const s8* pchCondition)
{
	u16        wBase    = EV_MCUMT_BGN;
	static u16 wItem    = 1;
	s32        nCount   = 0;
	s32        nItemNum = 40;

	if(!pchCondition)
	{
		listmsg();
		return;
	}
	
	for( ; wBase < EV_MCUMT_END; wBase++ )
	{
		const s8* pchMsg = OspEventDesc( wBase );
		if( pchMsg != NULL && strstr( pchMsg, pchCondition ) != NULL)
		{
			StaticLog( "%u.\t%u\t(%s)\n", wItem, wBase, pchMsg);
			wItem  ++ ;
			nCount ++ ;
		}
		
		if( nCount == nItemNum )
			break;
	}
}

/********************************************************************
	created:	2013/06/25
	created:	25:6:2013   11:21
	name: 		showsmcu
	author:		pengguofeng
	
	purpose:	display all smcu's mtlist, including itself
*********************************************************************/
API void showsmcu()
{
	// 打印部分结体中名称的长度 [pengguofeng 7/23/2013]
	/*	
	 *	STR_LEN  TGetConfIDByNameReq  TPart  TInvitePart   TCallNtf  TInvitePartReq
	 *    50           172             308    128            68         24660           当前值
	 *    16           104             204    128            32         24660           4.7.2以前的版本值
	 */
	StaticLog("Length: STR_LEN.%d  TGetConfIDByNameReq.%d  TPart.%d  TInvitePart.%d  TCallNtf.%d  TInvitePartReq.%d\n",
		STR_LEN, sizeof(TGetConfIDByNameReq), sizeof(TPart), sizeof(TInvitePart), sizeof(TCallNtf), sizeof(TInvitePartReq));
	// 打印下级列表
	g_cMtAdpApp.m_tAllSMcuList.Print();
}

/*=============================================================================
  函 数 名： mtgk
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11       1.0         TanGuang                创建
=============================================================================*/
API void mtgk()
{
	StaticLog( "\n    g_cMtAdpApp.m_bGkRegistered: %d\n", g_cMtAdpApp.m_bGkRegistered);
	StaticLog( "\n    g_cMtAdpApp.m_gkRegState: %d\n",    g_cMtAdpApp.m_gkRegState);
	StaticLog( "\n    g_cMtAdpApp.m_emMcuEncoding: %d\n",    g_cMtAdpApp.m_emMcuEncoding);
	StaticLog( "\n    g_cMtAdpApp.m_emGkEncoding: %d\n",    g_cMtAdpApp.m_emGkEncoding);
	StaticLog( "\n    g_cMtAdpApp.m_byStackInitUtf8: %d\n",    g_cMtAdpApp.m_byStackInitUtf8);
}

/*=============================================================================
  函 数 名： settimeoutspan
  功    能： 设置请求终端列表, 视频信息, 音频信息的时间间隔
  算法实现： 
  全局变量： 
  参    数：  u32 dwTimeout
             u8 byInfoType
  返 回 值： API 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
API void settimeoutspan( u32 dwTimeout, u8 byInfoType )
{
	if( dwTimeout < 1 )
	{
		dwTimeout = 1;
	}
	if( TYPE_PARTLISTINFO == byInfoType )
	{
		g_dwPartlistInfoTimeout = dwTimeout * 1000;
	}
	if( TYPE_AUDINFO == byInfoType )
	{
		g_dwAudInfoTimeout = dwTimeout * 1000;
	}
	if( TYPE_VIDINFO == byInfoType )
	{
		g_dwVidInfoTimeout = dwTimeout * 1000;
	}	
	StaticLog( "CurTimeoutInfo: Partlist(0).%d Aud(1).%d Vid(2).%d\n", 
		       g_dwPartlistInfoTimeout, g_dwAudInfoTimeout, g_dwVidInfoTimeout );
}

/*=============================================================================
  函 数 名： mastatus
  功    能： print ma status
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
=============================================================================*/
API void mastatus( void )
{

    u32 dwMcuIp = ntohl(g_cMtAdpApp.m_dwMcuIpAddr);
    u32 dwMcuIpB = ntohl(g_cMtAdpApp.m_dwMcuIpAddrB);

    StaticLog("MtAdp inner status listed fallowed:\n");
    StaticLog("m_bMtAdpInited :\t%d\n",       g_cMtAdpApp.m_bMtAdpInited);
    StaticLog("m_bH323InitFailed :\t%d\n",    g_cMtAdpApp.m_tMaStaticInfo.m_bH323InitFailed);
    StaticLog("m_bH323PollWatchdog :\t%d\n",  g_cMtAdpApp.m_bH323PollWatchdog);
    StaticLog("m_hApp :\t\t0x%x\n",           g_cMtAdpApp.m_hApp);
    StaticLog("m_wQ931Port :\t\t%d\n",        g_cMtAdpApp.m_wQ931Port);
    StaticLog("m_wRasPort :\t\t%d\n",         g_cMtAdpApp.m_wRasPort);
    StaticLog("m_wH225H245Port :\t%d\n",      g_cMtAdpApp.m_wH225H245Port);
    StaticLog("m_dwMcuIpAddr :\t\t%u.%u.%u.%u\n",    QUADADDR(dwMcuIp));
    StaticLog("m_wMcuPort :\t\t%d\n",         g_cMtAdpApp.m_wMcuPort);
    StaticLog("m_dwMcuIpAddrB :\t%u.%u.%u.%u\n",     QUADADDR(dwMcuIpB));
    StaticLog("m_wMcuPortB :\t\t%d\n",        g_cMtAdpApp.m_wMcuPortB);
    StaticLog("m_bDoubleLink :\t\t%d\n",      g_cMtAdpApp.m_bDoubleLink);
    StaticLog("m_bMasterMtAdp :\t%d\n",       g_cMtAdpApp.m_bMasterMtAdp);
    StaticLog("m_dwMtAdpAddr :\t\t%u.%u.%u.%u\n",    QUADADDR(g_cMtAdpApp.m_dwMtAdpIpAddr));
    StaticLog("m_dwSndMtListReqNum :\t%u\n",  g_cMtAdpApp.m_tMaStaticInfo.m_dwSndMtListReqNum);
    StaticLog("m_dwRcvMtListReqNum :\t%u\n",  g_cMtAdpApp.m_tMaStaticInfo.m_dwRcvMtListReqNum);
    StaticLog("m_dwSndAudInfoReqNum :\t%u\n", g_cMtAdpApp.m_tMaStaticInfo.m_dwSndAudInfoReqNum);
    StaticLog("m_dwRcvAudInfoReqNum :\t%u\n", g_cMtAdpApp.m_tMaStaticInfo.m_dwRcvAudInfoReqNum);
    StaticLog("m_dwSndVidInfoReqNum :\t%u\n", g_cMtAdpApp.m_tMaStaticInfo.m_dwSndVidInfoReqNum);
    StaticLog("m_dwRcvVidInfoReqNum :\t%u\n", g_cMtAdpApp.m_tMaStaticInfo.m_dwRcvVidInfoReqNum);
    StaticLog("m_wMaxRTDInterval :\t%d\n",    g_cMtAdpApp.m_wMaxRTDInterval);
    StaticLog("m_byMaxRTDFailTimes :\t%d\n",  g_cMtAdpApp.m_byMaxRTDFailTimes);
    StaticLog("m_byMaxCallNum :\t%d\n",       g_cMtAdpApp.m_byMaxCallNum);
    StaticLog("m_byCascadeLevel :\t%d\n",     g_cMtAdpApp.m_byCascadeLevel);
	StaticLog("m_byCasAliasType :\t%d\n",     g_cMtAdpApp.m_byCasAliasType);
	StaticLog("m_byMaxHDMtNum :\t%d\n",	      g_cMtAdpApp.m_byMaxHDMtNum);
	StaticLog("m_tGKAddr:%08x\n",			  g_cMtAdpApp.m_tGKAddr.ip);
#ifdef _IS22_
	StaticLog("m_wDiscHeartBeatTime:%d\n",	  g_cMtAdpApp.GetDiscHeartBeatTime());
	StaticLog("m_byDiscHeartBeatNum:%d\n",	  g_cMtAdpApp.GetDiscHeartBeatNum());
#endif
	
	if(g_cMtAdpApp.GetLicenseErrorCode() == 0)
	{
		// xliang [10/31/2008]  显示最大接入MT能力及目前用了多少接入能力
#ifndef WIN32
		u16 wMaxHDIConntMt = g_cMtAdpApp.m_wMaxNumConntMt;//有license后放开
#else
		u16 wMaxHDIConntMt = MAXNUM_CONF_MT;
#endif
//		u16 wCurNumConntMt = 0;
		StaticLog("MaxConntMt :\t%d\n",		   wMaxHDIConntMt);
		//显示目前用了多少接入能力
// 		CApp * pcApp = (CApp*)&g_cMtAdpApp;
// 		CInstance *pcInst = NULL;
// 		for( s32 nDriMtNum = 1; nDriMtNum <= wMaxHDIConntMt; nDriMtNum ++ )
// 		{
// 			pcInst = pcApp->GetInstance(nDriMtNum);
// 			if( pcInst && pcInst->CurState() != CMtAdpInst::STATE_IDLE )
// 			{
// 				wCurNumConntMt++;
// 			}				
// 		}
//		OspPrintf(TRUE, FALSE, "CurHDIConntMt :\t%d\n",		   wCurNumConntMt);
		StaticLog("CurConntMt :\t%d\n",	g_cMtAdpApp.m_wCurNumConntMt);
	}
	else
	{
		//read license error
		StaticLog("Read mtadp license error, error code is :\t%u\n",g_cMtAdpApp.GetLicenseErrorCode());
	}

    g_cMtAdpApp.m_tDSCaps.Print();

	StaticLog("m_byIsGkUseRRQPwd :\t%d\n",	      g_cMtAdpApp.m_byIsGkUseRRQPwd);
	StaticLog("m_achRRQPassword :\t%s\n",	      g_cMtAdpApp.m_achRRQPassword);
}

/*=============================================================================
  函 数 名： macharge
  功    能： print all the conf charge info
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/11       4.0         张宝卿                创建
=============================================================================*/
API void macharge( void )
{
    if ( !g_cMtAdpApp.m_bMasterMtAdp ) 
    {
        return;
    }
    TConfChargeInfo *ptChargeInfo = &g_cMtAdpApp.m_atChargeInfo[0];
    StaticLog( "ConfIdx\tBitRate\tTerNum\tMixNum\tVMPNum\tBasNum\tIsCase\tIsEnc\tIsDual\tIsData\tIsBroad\n");
    for( u8 byConfIdx = 1; byConfIdx <= MAX_CONFIDX; byConfIdx ++ )
    {
        if ( !ptChargeInfo[byConfIdx-1].IsNull() )
        {
            StaticLog( "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                                    byConfIdx,
                                    ptChargeInfo[byConfIdx-1].GetBitRate(),
                                    ptChargeInfo[byConfIdx-1].GetTerNum(),
                                    ptChargeInfo[byConfIdx-1].GetMixerNum(),
                                    ptChargeInfo[byConfIdx-1].GetVMPNum(),
                                    ptChargeInfo[byConfIdx-1].GetBasNum(),
                                    ptChargeInfo[byConfIdx-1].GetIsUseCascade(),
                                    ptChargeInfo[byConfIdx-1].GetIsUseEncrypt(),
                                    ptChargeInfo[byConfIdx-1].GetIsUseDualVideo(),
                                    ptChargeInfo[byConfIdx-1].GetIsUseDataConf(),
                                    ptChargeInfo[byConfIdx-1].GetIsUseStreamBroadCast() );
        }
    }
}

/*=============================================================================
  函 数 名： matau
  功    能： telnet 授权
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/11       4.0         张宝卿                创建
=============================================================================*/
API void matau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
}

/*=============================================================================
  函 数 名： sq
  功    能： 打印当前的呼叫队列和等待队列
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/04/05    4.0         张宝卿                创建
=============================================================================*/
API void sq( void )
{
    g_cMtAdpApp.m_tCallQueue.ShowQueue();
    g_cMtAdpApp.m_tWaitQueue.ShowQueue();
}

/*=============================================================================
  函 数 名： pq
  功    能： 实时打印呼叫队列和等待队列的出入列信息
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： API void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/04/06    4.0         张宝卿                创建
=============================================================================*/
API void pq( void )
{
    g_cMtAdpApp.m_tCallQueue.PrtQueueInfo();
    g_cMtAdpApp.m_tWaitQueue.PrtQueueInfo();
}

API void setsendmmcu( u32 dw )
{
	g_dwSendMMcu = dw;
}

API void prtd( u8 byMtId )
{
	g_nPrtdid = byMtId;
}

API void nprtd( void )
{
	g_nPrtdid = -1;
}

API void mamtcall( void )
{
	StaticLog( "Nelgect Mt Invite Msg List:\n" );
	for( u8 byConfIdx = 0;byConfIdx<MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE;++byConfIdx )
	{
		StaticLog( "ConfIdx.%d\n",byConfIdx );
		for( u8 byLoop = 1;byLoop < MAXNUM_CONF_MT; ++byLoop )
		{
			if( g_cMtAdpApp.IsMtInviteMsgInOsp(byConfIdx,byLoop) && 
				g_cMtAdpApp.IsNeglectInviteMsg(byConfIdx,byLoop)
				)
			{
				StaticLog( "Mt.%d is neglect invite msg\n",byLoop );
			}
		}
	}
}
/*lint -save -e611*/
void MtAdpAPIEnableInLinux()
{       
#ifdef _LINUX_
    OspRegCommand("mtadphelp",  (void*)mtadphelp,   "mtadp help command");
    OspRegCommand("mtadpver",   (void*)mtadpver,    "mtadp version command");
    OspRegCommand("madebug",    (void*)madebug,     "mtadp debug command");
    OspRegCommand("macall",     (void*)macall,      "mtadp call command");
    OspRegCommand("maclear",    (void*)maclear,     "mtadp clear command");
    OspRegCommand("maras",      (void*)maras,       "mtadp ras command");
    OspRegCommand("masize",     (void*)masize,      "mtadp size command");
    OspRegCommand("sendev",     (void*)sendev,      "mtadp send event command");
    OspRegCommand("sendevmsg",  (void*)sendevmsg,   "mtadp send event msg command");
    OspRegCommand("listmsg",    (void*)listmsg,     "mtadp listmsg command");
    OspRegCommand("findmsg",    (void*)findmsg,     "mtadp findmsg command");
    OspRegCommand("mtgk",       (void*)mtgk,        "mtadp gk info command");    
    OspRegCommand("prtd",       (void*)prtd,        "mtadp prtd command");
    OspRegCommand("nprtd",      (void*)nprtd,       "mtadp nprtd command");
    OspRegCommand("mastatus",   (void*)mastatus,    "mtadp status command");
    OspRegCommand("macharge",   (void*)macharge,    "mtadp conf charge info command");
    OspRegCommand("matau",      (void*)matau,       "mtadp telnet author command");
    OspRegCommand("setsendmmcu",(void*)setsendmmcu, "mtadp etsendmmcu command");
    OspRegCommand("settimeoutspan", (void*)settimeoutspan, "mtadp settimeoutspan command");
    OspRegCommand("sq",         (void*)sq,          "mtadp show call queue and wait queue");
    OspRegCommand("pq",         (void*)pq,          "mtadp show call queue and wait queue real time info");
	OspRegCommand("mamtcall",   (void*)mamtcall,    "mtadp show mt is or not neglect invite msg");
    OspRegCommand("pmamsg",     (void*)pmamsg,      "mtadp open log print command");
    OspRegCommand("npmamsg",    (void*)npmamsg,     "mtadp close log print command");
	OspRegCommand("showsmcu",   (void*)showsmcu,    "mtadp pint sub mcu list");
#endif

    return;
}

/*  //  [pengguofeng 4/9/2013]
*	将pSrc的内容，转成pDst的格式
*	从Utf-8 -> GBK
*	return False以方便外部转换，nlen为Dst的数组长度(即sizeof)
*   bOut是限制所在MCU使用GBK编码时，往外部发送时不作转换
*/
BOOL32 CMtAdpInst::TransEncoding(const s8 *pSrc, s8 *pDst, s32 nlen, BOOL32 bOut /*= TRUE*/ )
{
#ifndef _UTF8
	return FALSE; // 老版本返回FALSE，不进行转码操作 [pengguofeng 4/26/2013]
#else
	if ( !pSrc ||!pDst)
	{
		return FALSE;
	}
	
	u32 dwDstLen = 0;
	
	emenCodingForm emMcuType = g_cMtAdpApp.GetMcuEncoding();
	emenCodingForm emMtType = GetEndPointEncoding();
	
	//for test
	MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "UTF:1 GBK:2\n\tMcuType:%d MtType:%d bOut:%d\n", emMcuType, emMtType, bOut);
	if (  emMcuType != emMtType )
	{
		if ( emMcuType == emenCoding_Utf8
			&& emMtType == emenCoding_GBK )
		{
			if ( bOut ) //发送
			{
				dwDstLen = utf8_to_gb2312(pSrc, pDst, nlen-1);
			}
			else //接收
			{
				LogPrint( LOG_LVL_DETAIL, MID_MCULIB_MTADP, "[TransEncoding]recv GBK string,correct it\n");
				s8 achStr[MAXLEN_ALIAS] = {0};
				strncpy(achStr, pSrc, sizeof(achStr)-1);
				CorrectGBKStr(achStr, strlen(achStr));
				dwDstLen = gb2312_to_utf8(/*pSrc*/achStr, pDst, nlen-1);
			}
		}
		else if ( emMcuType == emenCoding_GBK
			&& emMtType == emenCoding_Utf8 )
		{
			if ( bOut ) //发送
			{
				MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "Mcu:GBK,no need to trans because needing to compate old MT\n");
				return FALSE;
			}
			else  //接收
			{
				dwDstLen = utf8_to_gb2312(pSrc, pDst, nlen-1);
			}
		}
		else
		{
			MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "not support trans\n");
			return FALSE;
		}
	}
	else
	{
		MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "not need to trans\n");
		return FALSE;
	}

	//for test
	MAPrint(LOG_LVL_ERROR, MID_MCULIB_MTADP, "trans length:from %d to %d\n", strlen(pSrc), dwDstLen);
	if ( dwDstLen == 0 )
	{
		MAPrint(LOG_LVL_WARNING, MID_MCULIB_MTADP, "dwDstLen == 0\n");
		return FALSE;
	}
	else
	{
		return TRUE;
	}
#endif
}

/*lint -restore*/
/*lint -restore*/