/*****************************************************************************
   模块名      : Recorder
   文件名      : chnInst.cpp
   相关文件    : RecApp.cpp
   文件实现功能: Recorder channel基类实现
   作者        : 
   版本        : V3.5  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/03/21  4.0         张宝卿      代码优化
   2006/07/20  4.0         顾振华      代码调整、增加树形支持(同时支持R2和R3)
******************************************************************************/
#include "ChnInst.h"
#include "RecApp.h"
#include "evrec.h"
#include "RecError.h"
#include "evmcueqp.h"
#include "errno.h"
#include "mcuver.h"
#include "mcuinnerstruct.h"
#include <atlbase.h>
#include "nserror.h"

IWMSServer *g_pServer;
IWMSPublishingPoints *g_pPubPoints;

#define IS_CONF_RECORD(tMt) ((tMt.GetEqpId() == 0) ? TRUE : FALSE)  
CRecApp g_cRecApp;

u8		g_byRecChnNum;
u8		g_byPlayChnNum;
BOOL32	g_bPrintRecLog = FALSE;


#ifdef _RECSERV_
#include "RecServ_Def.h"

/*#define SendMsgToMcu CChnInst::SendMsgToRecSevr*/

struct TMtInfo
{
    TMtRegReq m_tRegReq;
    u32 m_dwMtNodeId;
    u32 m_dwMtAppId;
};

// #else
// #define SendMsgToMcu CChnInst::SendMsgToMcuA;
#endif


/*=============================================================================
  函 数 名： ReadFileProcessCallBack
  功    能： 放像进度告警
  算法实现： 
  全局变量： 
  参    数： u8  byPlayerID       : 
             u32 dwCurrentPlayTime: 当前放象的绝对时间
             u32 dwContext        : 录像机号
  返 回 值： void 
=============================================================================*/
void ReadFileProcessCallBack( u32 byPlayerID, u32 dwCurrentPlayTime, u32 dwContext )
{
    CServMsg    cServMsg;
    TNotifyInfo tNotifyInfo;
    u8 byReadFileRate = 0;
    
    u16 wChnIdx = GetRecChnIndex( dwContext ); // 录像设备号和放像设备号是一样的
    if( wChnIdx <= g_byRecChnNum )
    {
        wChnIdx = wChnIdx + g_byRecChnNum;
    }
	// 实际返回的是实例号，应该从 1 --> MAXNUM_RECORDER_CHNNL, zgc, 2008-03-18
    //if( wChnIdx >= 0 && wChnIdx < MAXNUM_RECORDER_CHNNL)
	if( wChnIdx >= 1 && wChnIdx <= MAXNUM_RECORDER_CHNNL)
    {
        TPlayerStatis tPlayerStatis;   
        u16 wRet = RPGetPlayerStatis( byPlayerID, &tPlayerStatis );
        if ( RP_OK != wRet )
        {
            OspPrintf( TRUE, FALSE, "[ReadFileProcessCallBack] RPGetPlayerStatis failed !\n");
            return;
        }
        
		//byReadFileRate = (u8)((tPlayerStatis.m_dwPlyTime / tPlayerStatis.m_dwTotalPlyTime) * 100);

        if( 0 == tPlayerStatis.m_dwTotalPlyTime )
        {
			byReadFileRate = 100;	// 空文件, 认为播放完毕, zgc, 20070613
            tNotifyInfo.wAlarmType = emPLAYEREMPTYFILE; // 空文件
        }
        else
        {
			byReadFileRate = (u8)((tPlayerStatis.m_dwPlyTime / tPlayerStatis.m_dwTotalPlyTime) * 100);  // 防止除0, 从判断之前移到判断内, zgc, 20070613
            tNotifyInfo.wAlarmType = emPLAYERPROCESS;   // 放像进度通知
        }
        tNotifyInfo.dwPlayTime = byReadFileRate;

		if( 0xffffffff == dwCurrentPlayTime )	// 录象播放完毕, zgc, 20070613
		{
			tNotifyInfo.dwPlayTime = FINISH_PLAY;
		}

        tNotifyInfo.wChnIndex  = wChnIdx;
        
        cServMsg.SetChnIndex( (u8)wChnIdx ); 
        cServMsg.SetMsgBody( (u8*)&tNotifyInfo, sizeof(tNotifyInfo) ); 
        ::OspPost( MAKEIID(AID_RECORDER, wChnIdx),
                   EV_REC_DEVNOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }
    return;
}

/*=============================================================================
  函 数 名： ProcSpaceAlarmCallBack
  功    能： 剩余存储空间告警回调函数
  算法实现： 
  全局变量： 
  参    数： u8  byAlarmLevel  : 告警级别
             s8* pszPartionName: 文件录像录像路径(磁盘分区)         
                                  -- 目的是使得再次再本分区录像时，清空本分区。暂时没有用到
             u32 dwRecorderId  : 录像机号(只有在二级告警的时候有效) 
                                  -- 目的是停掉相应的录像机,与 dwContext 重复。暂时没有用到
             u32 dwContext     : 录像机号
  返 回 值： void 
=============================================================================*/
void ProcSpaceAlarmCallBack( u8 byAlarmLevel, s8 *pszPartionName, u32 byRecorderId, u32 dwContext )
{
    CServMsg cServMsg;
    TNotifyInfo tNotifyInfo;


    if( RP_DISK_ALARM_LEVEL_ONE == byAlarmLevel 
		|| RP_DISK_ALARM_LEVEL_TWO == byAlarmLevel
		|| RP_WRITE_FILE_ERROR == byAlarmLevel)
    {
        if( RP_DISK_ALARM_LEVEL_TWO == byAlarmLevel )
        {
            tNotifyInfo.wAlarmType = emRECORDER_FULLDISK;
        }
		else if( RP_WRITE_FILE_ERROR == byAlarmLevel )
		{
			tNotifyInfo.wAlarmType = emRECORDER_WRITE_FILE_ERR;
			// 取原始文件的发布模式和发布级别
			u8 byChnIdx  = u8(dwContext >> 16);
			tNotifyInfo.wChnIndex = byChnIdx;
			
			if ( byChnIdx > g_byRecChnNum )
			{
				LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "unexpected channle index<%d> !\n", byChnIdx );
				return;
			}
			
			LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_REC, "[CallBack](chnnl.%d) An Error occured on writing file\n", byChnIdx);   
		}
        else
        {
            tNotifyInfo.wAlarmType = emRECORDER_SMALL_FREESPACE;
            return;
        }        
        tNotifyInfo.wRecId = (u16)dwContext;                
        cServMsg.SetMsgBody((u8*)&tNotifyInfo, sizeof(tNotifyInfo) ); 
        ::OspPost( MAKEIID(AID_RECORDER, CInstance::DAEMON),
			       EV_REC_DEVNOTIF,
				   cServMsg.GetServMsg(),
				   cServMsg.GetServMsgLen());
    }
    return;
}

/*=============================================================================
  函 数 名： RecFileSizeOverDefCallBack
  功    能： 录像的文件超过了最大文件定义的限制回调, 即大文件切换回调
  算法实现： 回调获得新文件的文件名，作如下处理：
             1、若原始文件不发布，则不做处理
             2、若原始文件为立即发布，则将回调来的文件名拼接为MMS格式后直接发布
             3、原始文件为录完发布，则将回调来的文件名解析后，拼接出跳转前的上一个文件名，将之发布。
             4、并置当前的录像文件名为回调获得的文件名，使得在该文件停止录像时能正常发布最后一个未跳转文件。
  全局变量： 
  参    数： u8  byPlayerID : 
             u32 pchFileName: 新的文件名指针
             u32 dwContext  : 回调上下文，这里使用了3个低位u8，依次为 Chnlidx + 发布级别＋发布模式(由高到低)
  返 回 值： void 
-------------------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2006/11/15    4.0     张宝卿    创建
=============================================================================*/
void RecFileSizeOverDefCallBack( u32 dwRecorderID, s8* pszFileName, u32 dwContext )
{
    // 取原始文件的发布模式和发布级别
    u8 byPubMode = u8(dwContext);
    u8 byPubLvl  = u8(dwContext >> 8);
    u8 byChnIdx  = u8(dwContext >> 16);

    if ( byChnIdx > g_byRecChnNum )
    {
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "unexpected channle index<%d> !\n", byChnIdx );
        return;
    }

	LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_REC, "[CallBack] Changed to a new filename: %s with (recID,chnl,pub_mode,pub_lvl. %d,%d,%d,%d)\n\n", 
              pszFileName, dwRecorderID, byChnIdx, byPubMode, byPubLvl  );
    
    CServMsg cServMsg;
    cServMsg.SetMsgBody( (u8*)pszFileName, strlen(pszFileName) );
    //通知对应录像通道更新当前文件名
    OspPost( MAKEIID(AID_RECORDER, byChnIdx), EV_REC_REFRESH_FILENAME, 
             cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
}

/*====================================================================
    函数名	     ：CChnInst
	功能		 ：CChnInst 构造函数
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
CChnInst::CChnInst()
{
    m_dwDevice       = NOT_FIND_RECID;	
    m_dwPlayIpAddr  = 0;
    m_dwRecIpAddr   = 0;
    m_wPlayPort     = 0;
    m_wRecPort      = 0;
    m_byChnIdx      = 0;
    m_wLocalPort    = 0;
    m_byPublishMode  = PUBLISH_MODE_NONE;
    m_byPublishLevel = 0;

    m_cConfId.SetNull();
    m_tChnnlStatus.SetRecordName("");
	m_tChnnlStatus.m_tSrc.SetNull();
    m_tChnnlStatus.m_byType  = TRecChnnlStatus::TYPE_UNUSE;
    m_tChnnlStatus.m_byState = TRecChnnlStatus::STATE_IDLE;

	// 修改放像处理流程, zgc, 07/02/14
	m_tVideoEncrypt.Reset();
	m_tAudioEncrypt.Reset();
	memset( &m_tMediaInfo, 0 ,sizeof(TMediaInfo) );	
    memset( m_achOrignalFullName, 0, sizeof(m_achOrignalFullName) );  //add by jlb 081027
	m_byVideoNum = 0;

    memset( &m_tSrvTmt, 0, sizeof(m_tSrvTmt) );
    memset( m_achChnInfo, 0, sizeof(m_achChnInfo) );
    memset( m_achFileName, 0, sizeof(m_achFileName) );
    memset( m_achFullName, 0, sizeof(m_achFullName) );   

    m_curState = STATE_IDLE;
    m_lastState = STATE_IDLE;
	m_byConfPlaySrcSsnId = 0;
	m_bNeedCapChk = FALSE;

	m_pPubPoint = NULL;
	m_pBCPubPoint = NULL;

	m_bIsNodeAConnect = FALSE;
	m_bIsNodeBConnect = FALSE;
}

/*====================================================================
    函数名	     ：CChnInst
	功能		 ：CChnInst 析构函数
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
CChnInst::~CChnInst()
{
	if (m_pPubPoint != NULL)
	{
		m_pPubPoint->Release();
	}

	if (m_pBCPubPoint != NULL)
	{
		m_pBCPubPoint->Release();
	}
}

/*====================================================================
    函数名	     ：InitRecorder
	功能		 ：初始化录放器
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：成功返回 TRUE，否则返回FALSE
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2004/11/24  3.5         libo        分离单元测试部分代码
====================================================================*/
BOOL CChnInst::InitRecorder( )
{
	s16 wChannelNum;
	s8 achUserPass[MAX_FILE_PATH_LEN] = {0};

    //建立存放录像文件的目录
	::CreateDirectory( g_tCfg.m_szRecordPath, NULL );

	g_cRecApp.m_tEqp.SetMcuEqp( (u8)g_tCfg.m_wMcuId,
		                        g_tCfg.m_byEqpId,
					            g_tCfg.m_byEqpType );

	if( 0 != strlen(g_tCfg.m_achFTPUsername) )
	{
		sprintf(achUserPass, "%s:%s@", g_tCfg.m_achFTPUsername, g_tCfg.m_achFTPPassword);
	}
	sprintf( g_cRecApp.m_achPublishPath,"mms://%s%s/%s",
		     achUserPass,
             //g_tCfg.m_achLocalIP,
             g_tCfg.m_szMachineName,
		     g_tCfg.m_achVodFtpPatch);	
	
    // 初始化设备号
    for(u8 byLop = 0; byLop < MAXNUM_RECORDER_CHNNL; byLop ++ )
    {
        g_adwDevice[byLop] = NOT_FIND_RECID;
    }

    for( wChannelNum = 0; wChannelNum < MAXNUM_RECORDER_CHNNL; wChannelNum++ )
	{
		//向各通道发送上电消息
		post( MAKEIID(GetAppID(), (wChannelNum+1)), EV_REC_CHNINIT);
	}

#ifdef REC_CPPUNIT_TEST
	if( g_tCfg.m_bCppUnitTest )
	{
        ::OspPrintf(TRUE, TRUE, "[Recorder]CPPUNIT---OspCreateTcpNode(0x%x, 0x%x)\n",
                                 g_tCfg.m_dwCppUnitIpAddr,g_tCfg.m_wCppUnitPort);
		::OspCreateTcpNode( g_tCfg.m_dwCppUnitIpAddr,g_tCfg.m_wCppUnitPort );
	}
#endif

	return TRUE;
}

/*====================================================================
    函数名	     ：RestartRecorderProc
	功能		 ：重启录像机处理函数
	引用全局变量 ：无
    输入参数说明 ：TCfgAgent * ptConf - 配置信息结构指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    04/12/31    3.6         libo          create
	05/04/12	3.6			liuhuiyun	  增加双链

    FIXME:
    已知问题: 这里没有对通道等进行重新初始化,
    所以内部的通道数、MMS等各个字段可能都是错误的
====================================================================*/
void CChnInst::RestartRecorderProc()
{
    // 结束录像库
    // ::RPRelease();
    // 再重新初始化
    // InitalData();
    
    g_cRecApp.m_wRegTimes = 0;
    memset( g_cRecApp.m_achUsingFile,  0, sizeof(g_cRecApp.m_achUsingFile) );

    if (OspIsValidTcpNode(g_cRecApp.m_dwMcuNode))
	{
        ::OspDisconnectTcpNode(g_cRecApp.m_dwMcuNode);
    }
    else
    {        
        if( 0 != g_tCfg.m_dwMcuIpAddr )
        {
            // 如果配置了，则设置时间去连接
            SetTimer(EV_REC_CONNECTTIMER, REC_CONNECT_TIMEOUT);
        }
    }

    if(OspIsValidTcpNode(g_cRecApp.m_dwMcuNodeB))
    {
        ::OspDisconnectTcpNode(g_cRecApp.m_dwMcuNodeB);
    }
    else
    {
        if( 0 != g_tCfg.m_dwMcuIpAddrB )
        {
            // 如果配置了，则设置时间去连接
            SetTimer(EV_REC_CONNECTTIMERB, REC_CONNECT_TIMEOUT);
        }
    }

	LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC,  "\nEV_REC_RESTART!!\n\n");
    
}

/*====================================================================
    函数名	     ：DaemonInstanceEntry
	功能		 ：DAEMON 实例消息处理入口函数
	引用全局变量 ：无
    输入参数说明 ：pMsg　　- 消息指针
                   pcApp   - 应用指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/12	3.6			liuhuiyun	  增加消息显示
====================================================================*/
void CChnInst::DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp)
{
	s16 wLoop;
	CServMsg cServMsg(pMsg->content, pMsg->length);
    s32 nChIdx=cServMsg.GetChnIndex();  // 取通道数

    Reclog("Message %u(%s).\n", pMsg->event,::OspEventDesc( pMsg->event ));

	switch(pMsg->event) 
	{
    case EV_REC_RESTART:
        RestartRecorderProc();
        break;

	// 断链检测消息
	case MCU_EQP_ROUNDTRIP_REQ:
		SendMsgToMcu(EQP_MCU_ROUNDTRIP_ACK, &cServMsg);
		break;

	// 录像机上电
    case EV_REC_POWERON:
		// [8/25/2010 liuxu] 添加了参数
        InitalData(pMsg, pcApp);
		break;
    
	// 录像机断链
	case OSP_DISCONNECT:
		Disconnect(pMsg);//断链维护
		break;

	//MCU 通知会话 重联MPCB 相应实例, zgc, 2007/04/30
	case MCU_EQP_RECONNECTB_CMD:
		//DaemonProcReconnectBCmd( pMsg );
		break;
	
	// 定时建链消息
	case EV_REC_CONNECTTIMER:
        ProcConnectTimeOut(TRUE);
        break;
   
    case EV_REC_CONNECTTIMERB:
        ProcConnectTimeOut(FALSE);
        break;

	// 定时注册消息,等待注册应答消息
	case EV_REC_REGTIMER:
		ProcRegisterTimeOut(pMsg, TRUE);//注册
		break;

    case EV_REC_REGTIMERB:
		ProcRegisterTimeOut(pMsg, FALSE);//注册
		break;

	// 注册应答
	case MCU_REC_REG_ACK:
		MsgRegAckProc(pMsg, pcApp);     // ***
		break;

	//注册拒绝
	case MCU_REC_REG_NACK:
		MsgRegNackProc(pMsg);
		break;

	// 录像设备通知
	case EV_REC_DEVNOTIF:
		MsgDeviceNotifyProc(pMsg);
		break;

	//记录列表
	case MCU_REC_LISTALLRECORD_REQ:
		MsgListAllRecordProc(pMsg);     // ***
		break;
        
    // 修改文件名
    case MCU_REC_RENAMERECORD_REQ:
        MsgReNameFile(pMsg);
        break;

	//MCU请求获得录像机状态
	case MCU_REC_GETRECSTATUS_REQ:
		MsgGetRecStateProc(pMsg, pcApp);
		break;

	//录像机请求获得录像通道状态
	case MCU_REC_GETRECORDCHNSTATUS_REQ:
		MsgGetRecChnStatusProc(pMsg, pcApp);
		break;

	//录像机请求获得放像通道状态
	case MCU_REC_GETPLAYCHNSTATUS_REQ:
		MsgGetPlayChnStatusProc(pMsg, pcApp);
		break;
	
	//录像机状态定时上报
	case EV_REC_STATUSNOTYTIME:
		SendRecStatusNotify(pcApp);
		break;

	//请求发布消息
	case MCU_REC_PUBLISHREC_REQ:
		MsgDaemonPublishFileReq(pMsg);		
		break;

    //取消发布
	case MCU_REC_CANCELPUBLISHREC_REQ:
		MsgDaemonCancelPublishFileReq(pMsg);		
		break;
        
	//删除记录
	case MCU_REC_DELETERECORD_REQ:
		MsgDeleteRecordProc(pMsg);
		break;

    //请求录像
	case MCU_REC_STARTREC_REQ:
		OnDaemStartRecReq(pMsg, pcApp);
		break;

    //请求放像
	case MCU_REC_STARTPLAY_REQ:
		for(wLoop=0; wLoop< g_byPlayChnNum; wLoop++)
		{
			CChnInst * pInst;
			pInst = (CChnInst*)pcApp->GetInstance(g_byRecChnNum + wLoop+1);
			if (TRecChnnlStatus::STATE_PLAYREADY == pInst->CurState())
			{
				post( MAKEIID(GetAppID(), (g_byRecChnNum + wLoop+1)), pMsg->event,
					  pMsg->content, pMsg->length);
				return;
			}
		}
		LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_PLAY,  "[Rec] The Play Channels are not enough\n");
		
		cServMsg.SetErrorCode(ERR_REC_VALIDCHN);
		cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp) );
		SendMsgToMcu(REC_MCU_STARTPLAY_NACK, &cServMsg);
		break;
        
	//录像通道消息
	case MCU_REC_PAUSEREC_REQ:
	case MCU_REC_RESUMEREC_REQ:
	case MCU_REC_STOPREC_REQ:
	case MCU_REC_CHANGERECMODE_REQ:
    case MCU_REC_GETRECPROG_CMD:
	case MCU_REC_SETRTCP_NFY:
	case MCU_REC_SETRECV_NTF:
		if (nChIdx < g_byRecChnNum)
		{
			post(MAKEIID(GetAppID(), (nChIdx+1)), pMsg->event, pMsg->content, pMsg->length);
		}
		else
		{
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC,  "Mcu Send %u(%s) Chn.%d is not record Channel\n",
				                   pMsg->event, ::OspEventDesc(pMsg->event), nChIdx);
			
		}

		break;

	//播放通道消息
	case MCU_REC_PAUSEPLAY_REQ:
	case MCU_REC_RESUMEPLAY_REQ:
	case MCU_REC_FFPLAY_REQ:
	case MCU_REC_FBPLAY_REQ:
	case MCU_REC_STOPPLAY_REQ:
	case MCU_REC_SEEK_REQ:
    case MCU_REC_GETPLAYPROG_CMD:
	case MCU_EQP_SWITCHSTART_NOTIF:
		if( (nChIdx < g_byPlayChnNum) )
		{
			post( MAKEIID(GetAppID(), (nChIdx + g_byRecChnNum +1 )),
				  pMsg->event, pMsg->content, pMsg->length);
		}
		else
		{
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC,  "Mcu Send %u(%s) Chn.%d is not player Chnnel.\n",
				 pMsg->event,::OspEventDesc( pMsg->event ),nChIdx);
		}
		break;

    // 设置Qos值
    case MCU_EQP_SETQOS_CMD:
        ProcSetQosInfo(pMsg);
        break;
        
	// 显示录像及状态信息
	case EV_REC_STATUSSHOW:
		StatusShow( pcApp);
		break;

	// 主备状态
	case EV_REC_GETMSSTATUS_TIMER:
	case MCU_EQP_GETMSSTATUS_ACK:
		MsgDaemonGetMsStatusRsp(pMsg);
		break;

// 	//退出消息
// 	case EV_REC_QUIT:
// 		SetSyncAck( NULL,0);
// 		break;

#ifdef REC_CPPUNIT_TEST
    /////////////////////////////////
    //为CPPUnit测试保留的消息
	//获取录像机状态
	case REC_TEST_GETEQPSTATUS:
		if( g_tCfg.m_bCppUnitTest )
		{
			MsgCppUnitGetEqpStatus(pMsg,pcApp);
		}
		break;
#endif

	// 录像机服务器消息
    case MT_RECSERV_REG_REQ:
        OnMtRegReq(pMsg, pcApp);
        break;
	case EV_REC_QUIT:
		{
			for(wLoop=0; wLoop< g_byRecChnNum; wLoop++)
			{
				CChnInst * pInst;
				pInst = (CChnInst*)pcApp->GetInstance(wLoop+1);
				if (TRecChnnlStatus::STATE_RECREADY == pInst->CurState()
					|| TRecChnnlStatus::STATE_RECORDING == pInst->CurState() 
					|| TRecChnnlStatus::STATE_RECPAUSE == pInst->CurState())
				{
					post(MAKEIID(GetAppID(), (wLoop+1)), pMsg->event, pMsg->content, pMsg->length);
				}
			}
		}
		break;

    //未定义消息
	default:
		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC,  "[Rec] DAEMON :invalid message %u(%s).\n",
			 pMsg->event, ::OspEventDesc(pMsg->event));
		break;
	}

    return;
}

/*====================================================================
    函数名	     ：InstanceEntry
	功能		 ：实例消息处理入口函数
	引用全局变量 ：无
    输入参数说明 ：pMsg　　- 消息指针
	返回值说明   ：无
    注：所有的消息都是通过DAEMON分发而来，其消息的有效性由其保证。
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::InstanceEntry(CMessage * const pMsg)
{
	switch(pMsg->event) 
	{
	case EV_REC_CHNINIT:
		MsgChnInitProc( pMsg ); 
		break;

	case EV_REC_STOPDEVICE:
		MsgForceStopProc( pMsg);
		break;
	///////////////////////
	//
	// 录像消息
	
	//开始录像
	case MCU_REC_STARTREC_REQ:
		MsgStartRecordProc(pMsg);
		break;

	//暂停录像
	case MCU_REC_PAUSEREC_REQ:
		MsgPauseRecordProc(pMsg);
		break;

	//恢复录像
	case MCU_REC_RESUMEREC_REQ:
		MsgResumeRecordProc(pMsg);
		break;

	//停止录像
	case MCU_REC_STOPREC_REQ:
		MsgStopRecordProc(pMsg);
		break;

	//改变录像方式
	case MCU_REC_CHANGERECMODE_REQ:
		MsgChangeRecModeProc( pMsg );
		break;

	case MCU_REC_GETRECPROG_CMD:
        SendRecProgNotify(FALSE);
        break;

	//录像进程通知(每分钟通知1次)
	case EV_REC_RECPROGTIME:
		SendRecProgNotify();
		break;
	//设置录像RTCP参数
	case MCU_REC_SETRTCP_NFY:
		MsgResetRtcpParam(pMsg);
		break;
	case MCU_REC_SETRECV_NTF:
		MsgResetRcvParam(pMsg);
		break;
	////////////////////////////////////
	//
	//放像消息

	//开始播放
	case MCU_REC_STARTPLAY_REQ:
		MsgStartPlayProc(pMsg);
		break;
	
	//交换建立
	case MCU_EQP_SWITCHSTART_NOTIF:
		MsgSwitchStartNotifProc(pMsg);
		break;
	
	//暂停播放
	case MCU_REC_PAUSEPLAY_REQ:
		MsgPausePlayProc(pMsg);
		break;
	
	//恢复播放
	case MCU_REC_RESUMEPLAY_REQ:
		MsgResumePlayProc(pMsg);
		break;

	//快进
	case MCU_REC_FFPLAY_REQ:
		MsgFFPlayProc(pMsg);
		break;

	//快退
	case MCU_REC_FBPLAY_REQ:
		MsgFBPlayProc(pMsg);
		break;

	//停止播放
	case MCU_REC_STOPPLAY_REQ:
		MsgStopPlayProc(pMsg);
		break;

		
//	//调整backrtcp
//	case MCU_REC_ADJUST_BACKRTCP_CMD:
//		MsgAjustBackRtcp(pMsg);

    // 搜索
	case MCU_REC_SEEK_REQ:
		MsgSeekPlayProc(pMsg);
		break;

	case MCU_REC_GETPLAYPROG_CMD:
        SendPlayProgNotify();
        break;

	case EV_REC_SWITCHSTART_WAITTIMER:
		SwitchStartWaittimerOver(pMsg);
		break;

	case EV_REC_MCU_NEEDIFRAME_TIMER:    //录像机请求关键帧
		ProcTimerNeedIFrame(pMsg);
		break;
        
	// 录像设备通知
	case EV_REC_DEVNOTIF:
		MsgDeviceNotifyProc(pMsg);
		break;

    // 更新当前录像通道文件名通知
    case EV_REC_REFRESH_FILENAME:
        MsgRefreshCurChnRecFileName(pMsg);
        break;

    case EV_MT_DISCONNECTED:
        OnMtDisconnect(pMsg);
        break;

	case EV_REC_PUBLISHPOINT_TIMER:
		ProcTimerPpFailed(pMsg);
		break;
	case EV_REC_QUIT:
		ProcRecorderQuit(pMsg);
		break;

	default:
		log( LOGLVL_EXCEPTION,"[Rec] Inst%d invalid message %u(%s).\n",GetInsID(),pMsg->event,::OspEventDesc( pMsg->event ));
		break;
	}
	return;
}

/*=============================================================================
  函 数 名： ProcConnectTimeOut
  功    能： 处理连接超时
  算法实现： 
  全局变量： 
  参    数： BOOL32 bIsConnectA
  返 回 值： void 
 ----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	03/03/06    4.0         john          新开一个线程来连接
=============================================================================*/
void CChnInst::ProcConnectTimeOut(BOOL32 bIsConnectA)
{
	BOOL32 bRet = FALSE;
    if( bIsConnectA )
    {
        if(0 != g_tCfg.m_dwMcuIpAddr)
        {
            bRet = ConnectMcu(g_tCfg.m_dwMcuIpAddr, g_tCfg.m_wMcuConnPort, g_cRecApp.m_dwMcuNode);
            if(TRUE == bRet)
            {
                SetTimer(EV_REC_REGTIMER, REC_REGISTER_TIMEOUT);
            }
            else
            {
                SetTimer(EV_REC_CONNECTTIMER, REC_CONNECT_TIMEOUT);
            }
        }
    }
    else
    {
        if( 0 != g_tCfg.m_dwMcuIpAddrB )
        {
            bRet = ConnectMcu(g_tCfg.m_dwMcuIpAddrB, g_tCfg.m_wMcuConnPortB, g_cRecApp.m_dwMcuNodeB);
            if(TRUE == bRet)
            {
                SetTimer(EV_REC_REGTIMERB, REC_REGISTER_TIMEOUT);
            }
            else
            {
                // zbq [06/07/08] 保护起见，保持 McuA 的主导地位。否则，将建链错误
                if ( OspIsValidTcpNode(g_cRecApp.m_dwMcuNode) )
                {
                    SetTimer(EV_REC_CONNECTTIMERB, REC_CONNECT_TIMEOUT);
                }
                else
                {
					LogPrint(LOG_LVL_WARNING, MID_RECEQP_COMMON, "[ProcConnectTimeOut] McuA's node is invalid, almost impossible \n" );
                }
            }
        }
    }

	LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_COMMON, "[ProcConnectTimeOut] bMcuA.%d, McuIpA.0x%x, McuIpB.0x%x \n", 
             bIsConnectA, g_tCfg.m_dwMcuIpAddr, g_tCfg.m_dwMcuIpAddrB );

    return;
}
/*====================================================================
    函数名	     ：ConnectMcu
	功能		 ：与MCU建链函数
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 CChnInst::ConnectMcu(u32 dwMcuIp, u16 wMcuPort, u32& dwMcuNode)
{
    BOOL32 bRet = TRUE;
    if(!OspIsValidTcpNode(dwMcuNode))
    {
        dwMcuNode = ::OspConnectTcpNode(dwMcuIp, wMcuPort, 10, 3, 3000);
	    if (OspIsValidTcpNode(dwMcuNode))
	    {
		    // 由DAEMON实例处理断链消息
		    ::OspNodeDiscCBReg(dwMcuNode, GetAppID(), CInstance::DAEMON);		
		    			
			LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_COMMON, "[Rec] Connect MCU.0x%x Success!\n", htonl(dwMcuIp) );
	    }
        else
        {
			LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_COMMON, "[Rec] Fail to Connect MCU.0x%x !\n", htonl(dwMcuIp) );
            bRet = FALSE;
        }
    }
    else
    {
		LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_COMMON, "[Rec] Node.%d for MCU.0x%x is valid, ignore it !\n", dwMcuNode, dwMcuIp );
        
    }
    return bRet;
}

/*=============================================================================
  函 数 名： InitalData
  功    能： 初始化数据
  算法实现： 
  全局变量： 
  参    数： pMsg(消息体)
			 pcApp(CChnInst的App)
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CChnInst::InitalData(CMessage * const pMsg, CApp* pcApp)
{

    if( !g_tCfg.IsInited() )
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_COMMON, "[Rec] Error to read configure file\n");
		
		return FALSE;
	}
    
    if( 0 == g_tCfg.m_dwMcuIpAddr && 0 == g_tCfg.m_dwMcuIpAddrB)
    {
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_COMMON, "[Rec] The McuA and McuB's ip are all 0\n");
        
        return FALSE;
    }

    if(g_tCfg.m_dwMcuIpAddr == g_tCfg.m_dwMcuIpAddrB)
    {
        g_tCfg.m_dwMcuIpAddrB = 0;
        g_tCfg.m_wMcuConnPortB = 0;
    }

    Reclog("[Rec] Success to read configure file\n");
    
    // 重新设置录像机的提示
    s8 szPrompt[KDV_MAX_PATH] = {0};
    sprintf(szPrompt, "REC%d(%s)", g_tCfg.m_byEqpId, g_tCfg.m_szAlias);
    ::OspSetPrompt(szPrompt);
    
    // 存储快捷
    g_byPlayChnNum = g_tCfg.m_byPlayChnNum;
    g_byRecChnNum  = g_tCfg.m_byRecChnNum;

    TRPInitParam tRPInitParam;
    tRPInitParam.m_dwInitialPlayerNum = g_byPlayChnNum;
    tRPInitParam.m_dwInitialRecorderNum = g_byRecChnNum;
    tRPInitParam.m_dwRemainSpaceAlarmLevelOne = DISCSPACE_LEVEL1 ;  // 这里设置是无效的
    tRPInitParam.m_dwRemainSpaceAlarmLevelTwo = DISCSPACE_LEVEL2 ;  // 这里设置是无效的
    tRPInitParam.m_pCallback = ProcSpaceAlarmCallBack;
	
    //创建录放像设备
    u16 wRet = ::RPInit( &tRPInitParam );
	if( RP_OK != wRet )
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_RPCTRL, "[Rec] Fail to inital to RPLib, Error: %d\n", wRet);
        
		return FALSE;
	}

    // 设置监控的磁盘
    s8 szDrvName[3] = {0};
    strncpy(szDrvName, g_tCfg.m_szRecordPath, 2 );
    ::RPSetCheckPartion( szDrvName, DISCSPACE_LEVEL1, DISCSPACE_LEVEL2);  // 这里设置才有效

    BOOL32 bRet = InitRecorder();
    
	// [8/25/2010 liuxu] 录像通道的配置处理从向mcu注册的ack中提到此处。
	// 原因：
	//       (1)录像机服务器即使不连接mcu，也需要完成录像通道的配置的,否则终端录像无法进行
	//       (2)录像通道的初始化本应放在系统初始化的时候进行
	// 目的：配置录像通道    
	CChnInst * pInst;
	for( u8 bynChnnlIdx = 0; bynChnnlIdx < g_byRecChnNum; bynChnnlIdx++ )
	{
		pInst = ( CChnInst * )pcApp->GetInstance(bynChnnlIdx+1);			// 获得通道实例
		pInst->m_dwRecIpAddr = g_tCfg.m_dwRecIpAddr;						// 配置该通道的录像接收地址
		pInst->m_wRecPort    = g_tCfg.m_wRecStartPort + (PORTSPAN)*bynChnnlIdx; // 通道接收端口号
	}// [8/25/2010 liuxu] 修改结束

    if( bRet )
    {
        if(0 != g_tCfg.m_dwMcuIpAddr)
        {
            SetTimer( EV_REC_CONNECTTIMER, 2*1000 );//2 秒后进行建链
        }
		/*
        if(0 != g_tCfg.m_dwMcuIpAddrB)
        {
            SetTimer( EV_REC_CONNECTTIMERB, 2*1000 );//2 秒后进行建链
        }*/
    }
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_COMMON,  "[Rec] Fail to InitRecorder");
        
		return FALSE;		
	}

	// [pengjie 2010/9/26] 设置录像机的最大录像文件大小（超过则会切文件）
	::RPSetMaxAsfFileLen( g_tCfg.m_dwMaxRecFileLen );

	InitCOMServer();
    return TRUE;
}

/*=============================================================================
  函 数 名： MsgReNameFile
  功    能： 更改文件名
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
=============================================================================*/
void CChnInst::MsgReNameFile(CMessage* const pMsg )
{
    if(NULL == pMsg)
    {
        OspPrintf(TRUE, FALSE, "[MsgReNameFile] the pointer can not be Null\n");
        return;
    }
    
    u16 wRet = 0;
    s8 achSrcName[MAX_FILE_NAME_LEN] = {0};
    s8 achDstName[MAX_FILE_NAME_LEN] = {0};
    
	s8 achFullName[MAX_FILE_NAME_LEN] ={0};
    s8 szMMSName[MAX_FILE_NAME_LEN]   ={0};
    
	s8* lpMsgBody = NULL;

	CServMsg cServMsg(pMsg->content,pMsg->length);
	lpMsgBody = (s8*)cServMsg.GetMsgBody();
    lpMsgBody = lpMsgBody + sizeof(TEqp);
	u8 byOldNameLen = *(u8*)lpMsgBody;
    u8 byNewNameLen = *(u8*)(lpMsgBody + sizeof(u8) + byOldNameLen);
    memcpy(achSrcName, (lpMsgBody + sizeof(u8)), byOldNameLen);
    memcpy(achDstName, (lpMsgBody + sizeof(u8) + byOldNameLen + sizeof(u8)), byNewNameLen);

    if (0 == g_cRecApp.PreProcFileame(achFullName, achSrcName, NULL))
    {
		cServMsg.SetErrorCode(ERR_REC_FILENAMETOOLONG);
		cServMsg.SetMsgBody((u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp));
		SendMsgToMcu(REC_MCU_RENAMERECORD_NACK, &cServMsg);

		LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "Failed to rename %s! because file name too long.\n", achSrcName);
		
		return;
    }

	strcpy(achSrcName, achFullName);

    memset(achFullName, 0, sizeof(achFullName));
    if (0 == g_cRecApp.PreProcFileame(achFullName, achDstName, NULL))
    {
		cServMsg.SetErrorCode(ERR_REC_FILENAMETOOLONG);
		cServMsg.SetMsgBody((u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp));
		SendMsgToMcu(REC_MCU_RENAMERECORD_NACK, &cServMsg);

		LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "Failed to rename %s! because file name too long.\n", achDstName);
		return;
    }

    strcpy(achDstName, achFullName);    

    Reclog("Source Filename: %s\n", achSrcName);
    Reclog("Destination Filename: %s\n", achDstName);

    s32 nLen = strlen(achSrcName);
	for( s32 nLoop = 1; nLoop <= MAXNUM_RECORDER_CHNNL; nLoop++)// 判断指定的文件是否正在使用
	{		
		if( 0 ==_strnicmp(g_cRecApp.m_achUsingFile[nLoop], achSrcName, nLen) ) // 该文件正在被操作
		{
			cServMsg.SetErrorCode(ERR_REC_FILEUSING);
			cServMsg.SetMsgBody((u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp));
			
			SendMsgToMcu(REC_MCU_RENAMERECORD_NACK, &cServMsg);
			Reclog("Failed to rename %s! because now using.\n", achSrcName);
			return;
		}
	}

	// 判断文件是否发布
	BOOL bPublic = FALSE;
	if( g_tCfg.m_bOpenDBSucceed ) // 如果数据库服务器没有开，此时不理
	{
        g_cRecApp.Path2MMS( achSrcName, szMMSName );
        if( VOD_OK != g_cTDBOperate.QueryOneFileStauts(g_cRecApp.m_achPublishPath, szMMSName, bPublic))
		{
			Reclog("Query Public Lib Fail\n");
		}
		if(TRUE == bPublic )
		{
            wRet = g_cTDBOperate.CancelOnePublishedFile(g_cRecApp.m_achPublishPath, szMMSName);
			if(VOD_OK != wRet)
			{
				LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "Cancel Public File Fail\n");
			}
			else
			{
				LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "Cancel Public File Success\n");
			}
		}
	}

    wRet = ::rename(achSrcName, achDstName);

    if(RENAME_OK != wRet)
	{
		if(EACCES == wRet)
        {
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "Rename failed: The path question\n");
        }
        else if(ENOENT == wRet)
        {
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "Rename failed: File or path specified by oldname not found\n");            
        }
        else if(EINVAL == wRet)
        {
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "Rename failed: Name contains invalid characters\n");
        }
        
        cServMsg.SetErrorCode(ERR_REC_RENAMEFILE);
		cServMsg.SetMsgBody((u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp));
		SendMsgToMcu(REC_MCU_RENAMERECORD_NACK, &cServMsg);
        return;
	}
	else
	{
		cServMsg.SetMsgBody((u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp));
        SendMsgToMcu( REC_MCU_RENAMERECORD_ACK, &cServMsg);
		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "Success rename file: %s to %s\n", achSrcName, achDstName);

        if( bPublic && g_tCfg.m_bOpenDBSucceed )
        {
			wRet = PublishOneFile(achDstName);
        }
	}
}

/*====================================================================
    函数名	     ：MsgRegAckProc
	功能		 ：MCU注册应答消息处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg　　- 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgRegAckProc(CMessage * const pMsg, CApp* pcApp)
{	
	TPeriEqpRegAck tRegAck;
    u8 bynChnnlIdx;	
    CChnInst * pInst;

    // 是否是从界面配置地址返回的注册响应

    BOOL32 bRspFromMcuA = FALSE;

	CServMsg cServMsg(pMsg->content, pMsg->length);
    if(pMsg->srcnode == g_cRecApp.m_dwMcuNode)
    {
        g_cRecApp.m_dwMcuIId = pMsg->srcid;
        KillTimer(EV_REC_REGTIMER);
        g_cRecApp.m_byRegAckNum++;

        bRspFromMcuA = TRUE;
		m_bIsNodeAConnect = TRUE;
    }
    else if(pMsg->srcnode == g_cRecApp.m_dwMcuNodeB)
    {
        g_cRecApp.m_dwMcuIIdB = pMsg->srcid;
        KillTimer(EV_REC_REGTIMERB);
        g_cRecApp.m_byRegAckNum++;
		m_bIsNodeBConnect = TRUE;
    }
    
    // 处理回复消息体
	memcpy( &tRegAck, cServMsg.GetMsgBody(), sizeof(tRegAck) );

	// guzh [6/12/2007] 校验会话参数
    if ( g_cRecApp.m_dwMpcSSrc == 0 )
    {
        g_cRecApp.m_dwMpcSSrc = tRegAck.GetMSSsrc();
    }
    else
    {
        // 异常情况，断开两个节点
        if ( g_cRecApp.m_dwMpcSSrc != tRegAck.GetMSSsrc() )
        {
			LogPrint(LOG_LVL_ERROR, MID_RECEQP_COMMON, "[MsgRegAckProc] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
                      g_cRecApp.m_dwMpcSSrc, tRegAck.GetMSSsrc());

            if ( OspIsValidTcpNode(g_cRecApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cRecApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cRecApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cRecApp.m_dwMcuNodeB);
            }               
            return;
        }
    }

    if(FIRST_REGISTERACK == g_cRecApp.m_byRegAckNum)
    {
		u32 dwAnotherMpcIp = tRegAck.GetAnotherMpcIp();
        
        // zbq [06/08/2007] 保护起见，防止误刷了 MCU.B 的地址
		if ( bRspFromMcuA && 0 != dwAnotherMpcIp )
		{
			g_tCfg.m_wMcuIdB = g_tCfg.m_wMcuId;
			g_tCfg.m_dwMcuIpAddrB = ntohl(dwAnotherMpcIp);
			g_tCfg.m_wMcuConnPortB = g_tCfg.m_wMcuConnPort;
			SetTimer( EV_REC_CONNECTTIMERB, 10 ); //立即建链
		}
        else
        {
            // 这里不需要起A的Timer。A的Timer马上就到 ...
        }
		LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_COMMON, "[MsgRegAckProc] dwAnotherMpcIp.0x%x, bRspA.%d \n", ntohl(dwAnotherMpcIp), bRspFromMcuA );

        NEXTSTATE(STATE_NORMAL);
        memset(&g_cRecApp.m_PrsParam, 0, sizeof(g_cRecApp.m_PrsParam));
        g_cRecApp.m_PrsParam = *(TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));

        // 顾振华 [7/19/2006] 记录Mcu别名
        memset( g_cRecApp.m_szMcuAlias, 0, MAX_ALIAS_LEN );
        if ( cServMsg.GetMsgBodyLen() > sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) )
        {
            const s8* pszAlias = (s8*)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan));
            strncpy( g_cRecApp.m_szMcuAlias, pszAlias, MAX_ALIAS_LEN - 1 ); 
        }
        else
        {
            // 默认MCU名
            strcpy( g_cRecApp.m_szMcuAlias, "LocalMcu");
        }
        // 生成工作路径，目前都在本路径下操作各个会议下vod。可以通过 GetWorkingPath() 来获取
        sprintf( g_cRecApp.m_szWorkPath, "%s\\%s", 
                 g_tCfg.m_szRecordPath, 
                 g_cRecApp.m_szMcuAlias );

        ::CreateDirectory( g_cRecApp.GetWorkingPath(), NULL );             
    }
    g_cRecApp.m_dwMcuStartRcvPort = tRegAck.GetMcuStartPort();

	// [8/25/2010 liuxu] 将录像通道的配置信息提到了InitData中去。

	// 放像通道
	for(bynChnnlIdx = 0; bynChnnlIdx < g_byPlayChnNum; bynChnnlIdx++)
	{						
		pInst = (CChnInst*)pcApp->GetInstance(g_byRecChnNum + bynChnnlIdx + 1); // 获得播放通道实例
		pInst->m_dwPlayIpAddr	= htonl(tRegAck.GetMcuIpAddr());                     // 获得对端播放IP地址
		pInst->m_wPlayPort		= tRegAck.GetMcuStartPort() + (PORTSPAN)*bynChnnlIdx;
	}    

    // 上报状态
	SetTimer(EV_REC_STATUSNOTYTIME, 10);	   

    return;
}

/*====================================================================
    函数名	     ：MsgRegNackProc
	功能		 ：MCU注册拒绝消息处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg　　- 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgRegNackProc(CMessage * const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    if(pMsg->srcnode == g_cRecApp.m_dwMcuNode)
    {
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_COMMON, "[Rec] Register to Mcu(0x%x) refused with err.%d. Retry...\n", g_tCfg.m_dwMcuIpAddr, cServMsg.GetErrorCode());
		
    }

    if(pMsg->srcnode == g_cRecApp.m_dwMcuNodeB)
    {
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_COMMON, "[Rec] Register to BakMcu(0x%x) refused with err.%d. Retry...\n", g_tCfg.m_dwMcuIpAddrB, cServMsg.GetErrorCode());
        
    }

    return;
}

/*====================================================================
    函数名	     ：Disconnect
	功能		 ：拆链
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::Disconnect(CMessage* const pMsg)
{
    if(NULL == pMsg)
    {
        OspPrintf(TRUE, FALSE, "[Disconnect] The pointer can not be Null\n");
        return;
    }

    u32 dwNode = *(u32*)pMsg->content;
    
    if ( INVALID_NODE != dwNode )
    {
        OspDisconnectTcpNode( dwNode );
    }
    if(dwNode == g_cRecApp.m_dwMcuNode)
    {
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_COMMON, "[Rec] Disconnect with  Mcu.0x%x.\n", g_tCfg.m_dwMcuIpAddr);
        g_cRecApp.m_dwMcuNode = INVALID_NODE;
        g_cRecApp.m_dwMcuIId = INVALID_INS;

		m_bIsNodeAConnect = FALSE;

        SetTimer(EV_REC_CONNECTTIMER, REC_CONNECT_TIMEOUT);

        // zbq [06/07/08] 保护起见，保持 McuA 的主导地位。否则，将错误建链
        if ( !OspIsValidTcpNode(g_cRecApp.m_dwMcuNodeB) )
        {            
            KillTimer(EV_REC_CONNECTTIMERB);
        }
    }
    else if(dwNode == g_cRecApp.m_dwMcuNodeB)
    {
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_COMMON, "[Rec] Disconnect with  Mcu.0x%x.\n", g_tCfg.m_dwMcuIpAddrB);
        g_cRecApp.m_dwMcuNodeB = INVALID_NODE;
        g_cRecApp.m_dwMcuIIdB = INVALID_INS;

		m_bIsNodeBConnect = FALSE;

        // zbq [06/07/08] 保护起见，保持 McuA 的主导地位。否则，将错误建链
        if ( OspIsValidTcpNode(g_cRecApp.m_dwMcuNode) )
        {            
            SetTimer(EV_REC_CONNECTTIMERB, REC_CONNECT_TIMEOUT);
        }
    }
	// 与其中一个断链后，向Mcu取主备倒换状态，判断是否成功
	if( INVALID_NODE != g_cRecApp.m_dwMcuNode || INVALID_NODE != g_cRecApp.m_dwMcuNodeB )
	{
		if (OspIsValidTcpNode(g_cRecApp.m_dwMcuNode))
		{
			// 是否考虑延时一会待Mcu稳定后再发
			post( g_cRecApp.m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cRecApp.m_dwMcuNode );     
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_COMMON, "[Disconnect] GetMsStatusReq. McuNode.A\n");
		}
		else if (OspIsValidTcpNode(g_cRecApp.m_dwMcuNodeB))
		{
			post( g_cRecApp.m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cRecApp.m_dwMcuNodeB );        
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_COMMON, "[Disconnect] GetMsStatusReq. McuNode.B\n");
		}
		// 等待指定时间
		SetTimer(EV_REC_GETMSSTATUS_TIMER, WAITING_MSSTATUS_TIMEOUT);
		return;
	}
	
    if(INVALID_NODE == g_cRecApp.m_dwMcuNode && INVALID_NODE == g_cRecApp.m_dwMcuNodeB)
    {
	    StopAllChannel(TRUE);
		KillTimer(EV_REC_STATUSNOTYTIME);
        
        // zbq [06/07/2007] 清空注册标识
        g_cRecApp.m_byRegAckNum = 0;
		g_cRecApp.m_dwMpcSSrc = 0;
        LogPrint(LOG_LVL_WARNING, MID_RECEQP_COMMON, "[Disconnect] Node(s) is(are) all invalid, reconnect ... \n");
    }
    return;
}

/*=============================================================================
函 数 名： DaemonProcReconnectBCmd
功    能： 
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/4/30   4.0			周广程                  创建
=============================================================================*/
void CChnInst::DaemonProcReconnectBCmd( CMessage * const pcMsg )
{
	if( pcMsg == NULL )
	{
		return;
	}
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u32 dwMpcBIp = *(u32*)cServMsg.GetMsgBody();
	dwMpcBIp = ntohl(dwMpcBIp);
	g_tCfg.m_dwMcuIpAddrB = dwMpcBIp;
	
	// 如果已连接, 先断开
	if( OspIsValidTcpNode( g_cRecApp.m_dwMcuNodeB ) )
	{
		OspDisconnectTcpNode( g_cRecApp.m_dwMcuNodeB );
	}

	g_cRecApp.m_dwMcuNodeB = INVALID_NODE;
    g_cRecApp.m_dwMcuIIdB = INVALID_INS;
    SetTimer(EV_REC_CONNECTTIMERB, REC_CONNECT_TIMEOUT);
}
/*=============================================================================
  函 数 名： MsgDaemonGetMsStatusRsp
  功    能： 取主备响应
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
=============================================================================*/
void CChnInst::MsgDaemonGetMsStatusRsp(CMessage* const pMsg)
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg(pMsg->content, pMsg->length);
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event )
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer(EV_REC_GETMSSTATUS_TIMER);

		LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_COMMON, "[MsgDaemonGetMsStatusRsp]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                  ptMsStatus->IsMsSwitchOK());

        if(ptMsStatus->IsMsSwitchOK()) // 倒换成功
        {
            bSwitchOk = TRUE;
        }
	}

	// 倒换失败或者超时
	if( !bSwitchOk )
	{
		StopAllChannel(TRUE);
		KillTimer(EV_REC_STATUSNOTYTIME);

        // zbq [06/07/2007] 情况注册标识
        g_cRecApp.m_byRegAckNum = 0;

		g_cRecApp.m_dwMpcSSrc = 0;

		m_bIsNodeAConnect = FALSE;
		m_bIsNodeBConnect = FALSE;

		if( INVALID_NODE == g_cRecApp.m_dwMcuNode)// 有可能产生重连，在Connect里面过滤
		{
			SetTimer(EV_REC_CONNECTTIMER, REC_CONNECT_TIMEOUT);
		}

        // zbq [06/08/2007] 主备状态不同步，应只向 McuA 发起连接！否则，如果同时也
        // 向McuB发起了连接，且该连接导致的注册先返回(一般都会)，则全局配置 McuB的
        // 地址被冲掉且变成了McuA的地址，此后的动作只会向McuA建链和注册。进而导致Mcu主备不同步 ...        
        /*
		if( INVALID_NODE == g_cRecApp.m_dwMcuNodeB)
		{
			SetTimer(EV_REC_CONNECTTIMERB, REC_CONNECT_TIMEOUT);
		}*/
	}
	return;
}
/*=============================================================================
  函 数 名： StopAllChannel
  功    能： 停止所有通道
  算法实现： 
  全局变量： 
  参    数： BOOL32 bIncludePlayChnl    是否包括放像通道
  返 回 值： void 
=============================================================================*/
void CChnInst::StopAllChannel(BOOL32 bIncludePlayChnl)
{
    // guzh [10/9/2006] 只停录像即可
    s32 nMax = bIncludePlayChnl ? (g_byRecChnNum+g_byPlayChnNum) : g_byRecChnNum;
	for(s32 nChNum = 0; nChNum < nMax; nChNum++)
	{
		post(MAKEIID(GetAppID(), (nChNum+1)), EV_REC_STOPDEVICE);
	}
	return;
}
/*=============================================================================
  函 数 名： ProcRegisterTimeOut
  功    能： 处理注册超时
  算法实现： 
  全局变量： 
  参    数： BOOL32 bIsRegisterA
  返 回 值： void 
=============================================================================*/
void CChnInst::ProcRegisterTimeOut(CMessage* const pMsg, BOOL32 bIsRegisterA)
{
	if(TRUE == bIsRegisterA)
	{
		if(!m_bIsNodeAConnect)
		{
			Register(g_cRecApp.m_dwMcuNode, TRUE);
			SetTimer(EV_REC_REGTIMER, REC_REGISTER_TIMEOUT);
		}
	}
	else 
	{
		if(!m_bIsNodeBConnect)
		{
			Register(g_cRecApp.m_dwMcuNodeB, FALSE);
			SetTimer(EV_REC_REGTIMERB, REC_REGISTER_TIMEOUT);
		}
	}

    return;
}
/*====================================================================
    函数名	     ：Register
	功能		 ：向MCU注册函数
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2004/11/24  3.5         libo        删除多余时钟EV_REC_REGWAIT
	2005/03/29  3.6         liuhuiyun	删除多余置别名语句
====================================================================*/
void CChnInst::Register(u32 dwMcuNode, BOOL32 bIsRegisterA)
{
	CServMsg cSvrMsg;
	TPeriEqpRegReq tReg;

	tReg.SetEqpAlias( g_tCfg.m_szAlias );
	tReg.SetChnnlNum( g_byRecChnNum ); // 录像通道数
	tReg.SetPeriEqpIpAddr( g_tCfg.m_dwRecIpAddr );
	tReg.SetStartPort( g_tCfg.m_wRecStartPort );
    tReg.SetVersion(DEVVER_RECORDER);
    if(TRUE == bIsRegisterA)
    {
        tReg.SetMcuEqp( (u8)g_tCfg.m_wMcuId,
		                g_tCfg.m_byEqpId,
				        g_tCfg.m_byEqpType );
    }
    else
    {
        tReg.SetMcuEqp( (u8)g_tCfg.m_wMcuIdB,
		                g_tCfg.m_byEqpId,
				        g_tCfg.m_byEqpType );
    }
	
	cSvrMsg.SetMsgBody( (u8*)&tReg, sizeof(tReg) );
       
    post( MAKEIID( AID_MCU_PERIEQPSSN, g_tCfg.m_byEqpId ), REC_MCU_REG_REQ,
		      cSvrMsg.GetServMsg(), cSvrMsg.GetServMsgLen(), dwMcuNode );
	return ;
}

/*====================================================================
    函数名	     ：SendMsgToMcu
	功能		 ：向MCU发送消息
	引用全局变量 ：无
    输入参数说明 ：wEvent - 事件
	返回值说明   ：成功返回 TRUE，否则返回FALSE
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL CChnInst::SendMsgToMcu(u16 wEvent, CServMsg* const pcServMsg)
{
	// 录像机服务器
#ifdef _RECSERV_
	// [9/28/2010 liuxu] Daemon实例的消息也需要发给Mt
	if (DAEMON == GetInsID())
	{
		switch (wEvent)
		{
		case REC_MCU_EXCPT_NOTIF:
		case REC_MCU_RECSTATUS_NOTIF:
			SendMsgToRecSevrDaem(wEvent, pcServMsg);
		}
	}else
	{
		// 如果终端连接上了,则将消息发给终端
		if (IsMtConnect())
		{
			SendMsgToRecSevr(wEvent, pcServMsg);
			
			// 其它实例中,Mt申请录像通道失败或者释放录像通道时, CChnInst会释放与Mt的链接
			switch (wEvent)
			{
			case REC_MCU_STARTREC_NACK:					// 开始录像失败
			case REC_MCU_STOPREC_ACK:					// 结束录像成功
				RecServLevelLog(RECSERV_LEVEL_LOG_COMMON, "Close chanel[%d], Recording finished\n", GetInsID());
				SetMtConnect(FALSE);					// 关闭连接
				SetMTNodeInfo(0, 0);					// 终端节点信息清零
				break;

			case REC_MCU_RECORDCHNSTATUS_NOTIF:
				//录像通道已经被释放时需要更改终端连接的状态
				if (TRecChnnlStatus::STATE_RECREADY == CurState())	
				{
					SetMtConnect(FALSE);					// 关闭连接
					SetMTNodeInfo(0, 0);					// 终端节点信息清零
				}
				break;
			}
			
			return TRUE;
		}
	}

#endif	// end of _RECSERV_
	
	// 非终端消息，给mcu回复消息
	if ( OspIsValidTcpNode( g_cRecApp.m_dwMcuNode ) )
	{
        post( g_cRecApp.m_dwMcuIId, wEvent, pcServMsg->GetServMsg(), pcServMsg->GetServMsgLen(), g_cRecApp.m_dwMcuNode );
	}
	else
	{
        // do nothing
	}
    
    if(OspIsValidTcpNode(g_cRecApp.m_dwMcuNodeB))
    {
		post( g_cRecApp.m_dwMcuIIdB, wEvent, pcServMsg->GetServMsg(), pcServMsg->GetServMsgLen(), g_cRecApp.m_dwMcuNodeB );
    }
	else
	{
        // do nothing
	}

    return TRUE;
}



/*====================================================================
    函数名	     ：SendChnNotify
	功能		 ：通道状态上报
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::SendChnNotify()
{
	u16 wEvent;
	CServMsg cServMsg;
	s32 nChn = GetInsID();

	if( nChn <= g_byRecChnNum )
	{
		wEvent = REC_MCU_RECORDCHNSTATUS_NOTIF;
		cServMsg.SetMsgBody((u8*)&m_tSrvTmt,sizeof(m_tSrvTmt));
		cServMsg.CatMsgBody((u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp));
	}
	else if( (nChn <= (g_byRecChnNum + g_byPlayChnNum))
        && (  nChn >  g_byRecChnNum ) )
	{
		wEvent = REC_MCU_PLAYCHNSTATUS_NOTIF;
		cServMsg.SetMsgBody((u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp));
	}
	else
	{
		Reclog("A unused channel(%) try to send notify.\n", nChn-1);
		return;
	}
	cServMsg.SetChnIndex(m_byChnIdx);
	cServMsg.SetConfId(m_cConfId);
	cServMsg.CatMsgBody((u8*)&m_tChnnlStatus,sizeof(m_tChnnlStatus));
	
	SendMsgToMcu(wEvent, &cServMsg);
    return;
}

/*====================================================================
    函数名	     ：MsgDeleteRecordProc
	功能		 ：删除所有录像记录
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/25	3.6			liuhuiyun	  增加判断文件是否发布
====================================================================*/
void CChnInst::MsgDeleteRecordProc(CMessage * const pMsg)
{
	u16 wRet;

	s8  szInputName[MAX_FILE_NAME_LEN]   = {0};
	s8  achFullName[MAX_FILE_NAME_LEN]   = {0};
    s8  szMMSName [MAX_FILE_NAME_LEN]    = {0};
    s8  szRecPath [MAX_FILE_PATH_LEN]    = {0};
    s8  szRecName [MAX_FILE_NAME_LEN]    = {0};
	s8* lpMsgBody;	
		
	CServMsg cServMsg(pMsg->content,pMsg->length);
	lpMsgBody	   = ( s8 * ) cServMsg.GetMsgBody();
	
    // 格式： abc.asf
	strncpy( szInputName, lpMsgBody + sizeof( TEqp ), sizeof( szInputName) - 1 );
	u16 wFullNameSize;  // add by jlb  081026
    wFullNameSize = g_cRecApp.PreProcFileame(achFullName, szInputName, NULL);   
	if (0 == wFullNameSize)
    {
		cServMsg.SetErrorCode(ERR_REC_FILENAMETOOLONG);
		cServMsg.SetMsgBody((u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp));
		SendMsgToMcu(REC_MCU_DELETERECORD_NACK, &cServMsg);

		LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_COMMON, "Failed to delete %s! because file name too long.\n", szInputName);
		return;
    }

    g_cRecApp.Path2MMS( achFullName, szMMSName );
    g_cRecApp.SplitPath(achFullName, szRecPath, szRecName, TRUE);

    LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "The Delete full file name: %s\n", achFullName);

	// 判断制定的文件是否已使用
    s32 nLen = strlen(achFullName);
	for( s32 wLoop=1 ;wLoop< MAXNUM_RECORDER_CHNNL ;wLoop++)
	{		
		if( 0 ==_strnicmp(g_cRecApp.m_achUsingFile[wLoop], achFullName, nLen) )// 该文件正在被操作
		{
			cServMsg.SetErrorCode(ERR_REC_FILEUSING);
			cServMsg.SetMsgBody((u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp));
			
			SendMsgToMcu(REC_MCU_DELETERECORD_NACK, &cServMsg);
			return;
		}
	}

	// 判断文件是否发布
	if( g_tCfg.m_bOpenDBSucceed )
	{        
        BOOL bPublic = IsPublicFile(szMMSName); 
		if(TRUE == bPublic )
		{
            wRet = g_cTDBOperate.CancelOnePublishedFile(g_cRecApp.m_achPublishPath, szMMSName);
			if(VOD_OK != wRet)
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_COMMON, "[Rec] Cancel Public File Fail\n");
			}
			else
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_COMMON, "Cancel Public File Success\n");
			}
		}
	}

    wRet = RPDeleteFile( szRecPath, szRecName, TRUE);
	if( RP_OK == wRet )
	{
		cServMsg.SetMsgBody((u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp));
        SendMsgToMcu( REC_MCU_DELETERECORD_ACK, &cServMsg);

		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "[Rec] Successfully delete %s\n", achFullName);
    }
	else
	{
		cServMsg.SetErrorCode(ERR_REC_FILEUSING);
		cServMsg.SetMsgBody((u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp));
		SendMsgToMcu(REC_MCU_DELETERECORD_NACK, &cServMsg);

		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "Failed to delete %s(%d).\n", achFullName, wRet);
	}
    return;
}

/*====================================================================
    函数名	     ：MsgDaemonPublishFileReq
	功能		 ：请求发布文件
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgDaemonPublishFileReq(CMessage * const pMsg)
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
	
	if(!g_tCfg.m_bOpenDBSucceed)
	{
		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "[Rec] Public NOT support !");
		log( LOGLVL_DEBUG2,"[Rec] Public NOT support !");

		cServMsg.SetErrorCode(ERR_REC_PUBLICFAILED);
		SendMsgToMcu(REC_MCU_PUBLISHREC_NACK, &cServMsg);

		return;
	}
	
	u16 wErrCode = 0;
	u16 wAckEvent = REC_MCU_PUBLISHREC_ACK;
	TEqp  tEqp;
	u8  byPublicLevel;

	s8* lpMsgBody = (s8*)cServMsg.GetMsgBody();


	//save tEqp;
	memcpy( &tEqp, lpMsgBody, sizeof(tEqp) );
	lpMsgBody += sizeof(tEqp);

	byPublicLevel = (u8)*lpMsgBody; 
	lpMsgBody += sizeof(u8);

    s8 szFullName[MAX_FILE_NAME_LEN] = {0};
	if (0 == g_cRecApp.PreProcFileame(szFullName, lpMsgBody, NULL))
    {
		cServMsg.SetErrorCode(ERR_REC_FILENAMETOOLONG);
		SendMsgToMcu(REC_MCU_PUBLISHREC_NACK, &cServMsg);

		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "Failed to publish %s! because file name too long.\n", lpMsgBody);
		
		return;
    }

	if(VOD_OK != PublishOneFile(szFullName))
	{
		wAckEvent = REC_MCU_PUBLISHREC_NACK;
		wErrCode = ERR_REC_PUBLICFAILED;
	}

	cServMsg.SetErrorCode(wErrCode);
	SendMsgToMcu(wAckEvent, &cServMsg);
}

/*====================================================================
    函数名	     ：MsgDaemonCancelPublishFileReq
	功能		 ：取消发布文件
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/14	3.6			liuhuiyun	  修正发布功能
====================================================================*/
void CChnInst::MsgDaemonCancelPublishFileReq(CMessage* const pMsg)
{
	u16 wErrCode,wAckEvent,wRet;
	
	TEqp  tEqp;
	CServMsg cServMsg(pMsg->content,pMsg->length);
	s8* lpMsgBody = (s8*)cServMsg.GetMsgBody();
	
	//save tEqp;
	memcpy( &tEqp, lpMsgBody, sizeof(tEqp) );
	lpMsgBody += sizeof(tEqp);

    s8 szFullName[MAX_FILE_NAME_LEN] = {0};
    s8 szRecPath[MAX_FILE_PATH_LEN] = {0};
    s8 szRecName[MAX_FILE_NAME_LEN] = {0};
    s8 szMMSName[MAX_FILE_NAME_LEN] = {0};
	u16 wFullNameSize;  // add by jlb 081026
    wFullNameSize = g_cRecApp.PreProcFileame(szFullName, lpMsgBody, NULL);
	if (0 == wFullNameSize)
    {
		cServMsg.SetErrorCode(ERR_REC_FILENAMETOOLONG);
		SendMsgToMcu(REC_MCU_CANCELPUBLISHREC_ACK, &cServMsg);
		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "Failed to cancel publish %s! because file name too long.\n", lpMsgBody);
		
		return;
    }	
    g_cRecApp.SplitPath(szFullName, szRecPath, szRecName, FALSE);
    g_cRecApp.Path2MMS( szFullName, szMMSName );
    
	LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "ancel public mms name:  %s\n", szMMSName);

	wAckEvent = REC_MCU_CANCELPUBLISHREC_ACK;
	wErrCode  = 0;

	if( g_tCfg.m_bOpenDBSucceed )
	{
        wRet = g_cTDBOperate.CancelOnePublishedFile(g_cRecApp.m_achPublishPath, szMMSName);
		if( VOD_OK != wRet )
		{
			wAckEvent = REC_MCU_CANCELPUBLISHREC_NACK;
			wErrCode  = ERR_REC_PUBLICFAILED;
			
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "Cancel Public %s %s Failed!", g_cRecApp.m_achPublishPath, szMMSName);
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "Cancel Public Success\n");
			
		}
	}
	else
	{
		wAckEvent = REC_MCU_CANCELPUBLISHREC_ACK;
		wErrCode = ERR_REC_PUBLICFAILED;
		
		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "public not support !");
	}
	cServMsg.SetErrorCode( wErrCode );
	SendMsgToMcu( wAckEvent,&cServMsg);
    return;
}

/*
u16 CChnInst::ListAllFile( s8* pchDirStr, TListInfo& tFilesList )
{
      
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    s8 achFileName[KDV_MAX_PATH];
    memset(achFileName, 0, sizeof(achFileName));
    strncpy(achFileName, pchDirStr, strlen(pchDirStr));
    strcat(achFileName, "\\*.*");
    
    tFilesList.m_dwNum = 0;
    hFind = FindFirstFile(achFileName, &FindFileData);
    while ( INVALID_HANDLE_VALUE != hFind )
    {
        if( FILE_ATTRIBUTE_DIRECTORY != FindFileData.dwFileAttributes )
        {
            strncpy(tFilesList.m_astrFileName[tFilesList.m_dwNum], 
                FindFileData.cFileName, strlen(FindFileData.cFileName));

            tFilesList.m_dwNum++;
        }
        
        BOOL bFind = FindNextFile(hFind, &FindFileData);
        if(FALSE == bFind)
        {
            DWORD wRet = GetLastError();
            if( ERROR_NO_MORE_FILES == wRet)
            {
                break;
            }
        }        
    } 
        
    FindClose(hFind);
    return (0);

}
*/

// 统计该MCU下录像的总数
// guzh [9/7/2006] 废弃
/*lint -save -e560*/
// sprintf(szTmpName, "%s\\*.asf", *citr); argument no. 3 should be a pointer)

u16 CChnInst::GetRecordCount(vector<CString> &vtDirName)
{
    u16 wGlobal = 0;
    u16 wFolder = 0;

    s8 szTmpName [MAX_FILE_NAME_LEN] = {0};

    vector<CString>::const_iterator citr = vtDirName.begin();
    vector<CString>::const_iterator citrEnd = vtDirName.end();

    CFileFind cffRecFile;
    while (citr != citrEnd)
    {

        // 查找下一个会议目录
        // 开始在目录里面查找ASF记录文件
        BOOL32 bRetFile;
        sprintf(szTmpName, "%s\\*.asf", *citr);
        bRetFile = cffRecFile.FindFile( szTmpName );

        while (bRetFile)
        {
            bRetFile = cffRecFile.FindNextFile();
            wFolder ++;
        }
        cffRecFile.Close();

        wGlobal += (wFolder > (u16)MAX_FILE_NUMBER ? (u16)MAX_FILE_NUMBER : wFolder);
        wFolder = 0;
        citr ++;
    }

    return wGlobal;
}
/*lint -restore*/

/*
void CChnInst::SendRecordList(CServMsg &cServMsg, u16 dwTotalCount, u16 dwStartIdx, TListInfo& tListInfo, 
                              BOOL32 bConfReq, LPCSTR lpszConfName)
*/
void CChnInst::SendRecordList(CServMsg &cServMsg, vector<CString> &vtFiles)
{
	TRecFileListNotify tRecNotify;	
	tRecNotify.Reset();    
    
	s32 nSum = vtFiles.size();  // 取得文件数目
    s32 nLoop;
    s8 achTmpName [MAX_FILE_NAME_LEN] = {0};

	if( nSum > 0 )
	{
        /*
        // 调整成全局数量和索引
		tRecNotify.SetListSize(dwTotalCount);
        tRecNotify.SetStartIdx(dwStartIdx);
        tRecNotify.SetEndIdx(dwStartIdx);
        */
        tRecNotify.SetListSize(nSum);
        tRecNotify.SetStartIdx( 0 );
        tRecNotify.SetEndIdx( 0 );    

        s8 szFullName[MAX_FILE_NAME_LEN] = {0};
        s8 szMMSName [MAX_FILE_NAME_LEN] = {0};       

		for( nLoop = 0; nLoop < nSum; nLoop ++)
		{
            strcpy( achTmpName, vtFiles[nLoop] );

			// 检测该文件是否已发布
			BOOL bPublic = FALSE;
            memset(szFullName, 0, sizeof(szFullName) );
            memset(szMMSName, 0, sizeof(szMMSName) );
			u16 wFullNameSize;    //add by jlb 081026
            wFullNameSize = g_cRecApp.PreProcFileame(szFullName, achTmpName, NULL);    
			
			//  [5/17/2013 guodawei] 万一有个文件名字不长，那么该文件后的都不会刷新到界面
			// 另外由于endidx永远不会达到listsize，所以界面空白
// 			if (0 == wFullNameSize)
// 			{
// 				cServMsg.SetErrorCode(ERR_REC_FILENAMETOOLONG);
// 				cServMsg.SetEventId(REC_MCU_LISTALLRECORD_NOTIF);
// 				cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp) );
// 				SendMsgToMcu(REC_MCU_LISTALLRECORD_NOTIF, &cServMsg);
// 
// 				LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "Failed to send recorder list %s  nLoop = %d! because file name too long.\n", achTmpName, nLoop);
// 				
// 				break;
// 			}          
			g_cRecApp.Path2MMS( szFullName, szMMSName );

			if( g_tCfg.m_bOpenDBSucceed )
			{
                bPublic = IsPublicFile( szMMSName );   // 单纯文件名
			}

            tRecNotify.AddFile( achTmpName, bPublic );
			if( tRecNotify.IsFull() ) 
			{
                // 本包满，发送Nofity      
                // 最后要减去1， 变成 0~~15
                tRecNotify.SetEndIdx( tRecNotify.GetEndIdx() - 1 );

                cServMsg.SetEventId(REC_MCU_LISTALLRECORD_NOTIF);
				cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );

                cServMsg.CatMsgBody( (u8*)&tRecNotify, sizeof(tRecNotify) );
				SendMsgToMcu( REC_MCU_LISTALLRECORD_NOTIF, &cServMsg);
                
				LogPrint(LOG_LVL_DETAIL, 
					MID_RECEQP_COMMON,
					"Sending Record List Nofity(%d, %d-%d)\n", 
					tRecNotify.GetListSize(), 
					tRecNotify.GetStartIdx(),
                        tRecNotify.GetEndIdx());

                // 重新开始计数
                tRecNotify.Reset();
                tRecNotify.SetListSize(nSum);
                tRecNotify.SetStartIdx( nLoop + 1 );
                tRecNotify.SetEndIdx( nLoop + 1 );                
			}
		}

        // 发送最后一包
        if (!tRecNotify.IsEmpty())
        {
            tRecNotify.SetEndIdx( tRecNotify.GetEndIdx() - 1 );
        
	        cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp) );

	        cServMsg.CatMsgBody( (u8*)&tRecNotify, sizeof(tRecNotify) );
	        SendMsgToMcu( REC_MCU_LISTALLRECORD_NOTIF, &cServMsg);

            LogPrint(LOG_LVL_DETAIL, 
				MID_RECEQP_COMMON,
				"Sending Left Record List Nofity(%d: %d-%d)\n", 
                    tRecNotify.GetListSize(), 
                    tRecNotify.GetStartIdx(),
                    tRecNotify.GetEndIdx());

        } 
	}
    else
    {
        // 则即使没有任何数据，发一个空包给mcs，因为mcs需要这个notify来打开对话框        
	    cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp) );

	    cServMsg.CatMsgBody( (u8*)&tRecNotify, sizeof(tRecNotify) );
	    SendMsgToMcu( REC_MCU_LISTALLRECORD_NOTIF, &cServMsg);

		LogPrint(LOG_LVL_DETAIL, 
			MID_RECEQP_COMMON,
			"Sending Empty Record List Nofity (%d: %d-%d)\n", 
                tRecNotify.GetListSize(), 
                tRecNotify.GetStartIdx(),
                tRecNotify.GetEndIdx());

    }   
}

/*====================================================================
    函数名	     ：MsgListAllRecordProc
	功能		 ：列出所有录像记录
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/03/25	3.6			刘辉云		  缓冲区满时直接退出
====================================================================*/
void CChnInst::MsgListAllRecordProc(CMessage * const pMsg)
{
    s8 szTmpName[MAX_FILE_NAME_LEN] = {0};
    //s8 szConfDir[MAX_FILE_NAME_LEN] = {0};
	
	//TListInfo tRecInfoList;
    //memset(&tRecInfoList, 0, sizeof(tRecInfoList));
	
	CServMsg cServMsg(pMsg->content,pMsg->length);
    CServMsg cReplyMsg(pMsg->content,pMsg->length);

    cReplyMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp) );

    if ( cServMsg.GetMsgBodyLen() < sizeof(TEqp) + sizeof(u8) )
    {
        // Nack
        SendMsgToMcu( REC_MCU_LISTALLRECORD_NACK, &cReplyMsg);    
    }

    // Ack	
	SendMsgToMcu( REC_MCU_LISTALLRECORD_ACK, &cReplyMsg);

    // 记录下目标用户组
    u8 byUsrGrpId = *(cServMsg.GetMsgBody() + sizeof(TEqp));

    CFileFind cffRecFile;   // 负责遍历文件
    if ( !cffRecFile.FindFile( g_cRecApp.GetWorkingPath() ) )
    {
        // 如果没找到文件，看是否是没有建目录
        s32 nError  = GetLastError();
        if (ENOENT == nError ||
            ESRCH  == nError )
        {
            //MCU路径不存在，尝试重新创建
            ::CreateDirectory(g_cRecApp.GetWorkingPath(), NULL);        
        }  
    }
    cffRecFile.Close();

    // 开始在目录里面查找ASF记录文件
    BOOL32 bRetFile;
    sprintf(szTmpName, "%s\\*.asf", g_cRecApp.GetWorkingPath());

    bRetFile = cffRecFile.FindFile( szTmpName );
    BOOL32 bAdd;
    u8 byFileOwnerGrp;
    u16 wFileBitrate;

    vector<CString> vtFiles;
    CString cstrTmpName;
    while (bRetFile)
    {
        bRetFile = cffRecFile.FindNextFile();

        bAdd = g_cRecApp.GetParamFromFilename(cffRecFile.GetFileName(), byFileOwnerGrp, wFileBitrate, cstrTmpName);
        if (bAdd && 
            (byFileOwnerGrp == byUsrGrpId || USRGRPID_SADMIN == byUsrGrpId) )
        {
            vtFiles.push_back(cffRecFile.GetFileName());
            /*
            sprintf( tRecInfoList.m_astrFileName[tRecInfoList.m_dwNum], 
                     "%s", 
                     cffRecFile.GetFileName()) ;
            tRecInfoList.m_dwNum ++;
            */
        }

        /*
        if ( tRecInfoList.m_dwNum < MAX_FILE_NUMBER ) 
        {            
        }
        else
        {
            // 单个文件夹最多支持1000个文件
            break;
        }
        */
    }
    cffRecFile.Close();
    SendRecordList( cReplyMsg, vtFiles);
/*
    CFileFind cffDir;       // 负责遍历路径
    CFileFind cffRecFile;   // 负责遍历文件

    if ( !cffDir.FindFile( g_cRecApp.GetWorkingPath() ) )
    {
        // 如果没找到文件，看是否是没有建目录
        s32 nError  = GetLastError();
        if (ENOENT == nError ||
            ESRCH  == nError )
        {
            //MCU路径不存在，尝试重新创建
            ::CreateDirectory(g_cRecApp.GetWorkingPath(), NULL);        
        }  
    }
    cffDir.Close();

    // guzh [8/22/2006] MCU增加了目录列表
    vector<CString> vtStrDirName;    

    // 记录是否会议中请求单个目录下的录像文件
    u8 byConfReq = *(cServMsg.GetMsgBody() + sizeof(TEqp));
    // guzh [8/29/2006] 新需要要求所有列表都带路径。mcs修改，rec侧暂时不修改
    // 如果需要放开，直接采用下面的语句即可
    // byConfReq = 0;
    u8 byUsrGrpId = *(cServMsg.GetMsgBody() + sizeof(TEqp) + sizeof(u8));
    

    if (cServMsg.GetMsgBodyLen() > sizeof(TEqp) + 2*sizeof(u8) )
    {
        u8 byConfNum = (cServMsg.GetMsgBodyLen() - sizeof(TEqp) - 2*sizeof(u8)) / MAXLEN_CONFNAME;
        s8* pszConfName = (s8*)(cServMsg.GetMsgBody()+sizeof(TEqp)+2*sizeof(u8));
        for (u8 byConfIdx = 0; byConfIdx < byConfNum; byConfIdx ++)
        {
            strncpy( szTmpName, pszConfName, MAXLEN_CONFNAME );
            pszConfName += MAXLEN_CONFNAME;
            g_cRecApp.PreProcDirName( szConfDir, szTmpName, FALSE );

            if ( !cffDir.FindFile( szConfDir ) )
            {
                // 如果没找到，建目录
                s32 nError  = GetLastError();
                if (ENOENT == nError ||
                    ESRCH  == nError)
                {
                    // 尝试创建目录，供后面录放像使用
                    ::CreateDirectory(szConfDir, NULL);
                }   
            }
            cffDir.Close();
            vtStrDirName.push_back( CString(szConfDir) );            
        }
    }
    else if (byUsrGrpId == USRGRPID_SADMIN)
    {
        // 如果没给出且是超级管理员，说明请求所有目录的录像文件，遍历生成所有目录
        sprintf(szTmpName, "%s\\*.*", g_cRecApp.GetWorkingPath());
        BOOL32 bRet = cffDir.FindFile( szTmpName );
        while (bRet)
        {
            // 查找下一个会议目录
            bRet = cffDir.FindNextFile();
            if (!cffDir.IsDirectory())
            {
                continue;
            }
            if (cffDir.IsDots())
            {
                continue;
            }
            vtStrDirName.push_back( cffDir.GetFilePath() );
        }
        cffDir.Close();
    }
    else 
    {
        // 什么都不请求,保护性发送一个空列表
        SendRecordList( cServMsg, 0, 0, tRecInfoList, TRUE, NULL ); // 发送文件列表
        return;
    }

    // 计算总的文件数量
    u16 wFileGlobalCount = GetRecordCount(vtStrDirName);   
    u16 wFileGlobalIdx = 0;
    BOOL32 bSendEmpy = (wFileGlobalCount==0);

    vector<CString>::const_iterator citr = vtStrDirName.begin();
    vector<CString>::const_iterator citrEnd = vtStrDirName.end();

    while (citr != citrEnd)
    {
        // 查找下一个会议目录
        // 开始在目录里面查找ASF记录文件
        BOOL32 bRetFile;
        sprintf(szTmpName, "%s\\*.asf", *citr);
        sprintf(szConfDir, "%s", citr->Right(strlen(*citr)-citr->ReverseFind('\\')-1));

        bRetFile = cffRecFile.FindFile( szTmpName );
        while (bRetFile)
        {
            bRetFile = cffRecFile.FindNextFile();
            if ( tRecInfoList.m_dwNum < MAX_FILE_NUMBER ) 
            {
                if (byConfReq)
                {
                    // 单个会议不带路径
                    sprintf( tRecInfoList.m_astrFileName[tRecInfoList.m_dwNum], 
                             "%s",                          
                             cffRecFile.GetFileName()) ;
                }
                else
                {
                    sprintf( tRecInfoList.m_astrFileName[tRecInfoList.m_dwNum], 
                             "%s\\%s", 
                             szConfDir, 
                             cffRecFile.GetFileName()) ;
                }
                tRecInfoList.m_dwNum ++;
            }
            else
            {
                // 单个文件夹最多支持1000个文件
                break;
            }
        }
        cffRecFile.Close();
        
        if (byConfReq)
        {
            // 如果会议请求是单个文件夹
            wFileGlobalCount = (u16)tRecInfoList.m_dwNum;
            // 发送文件列表，即使空也发送
            SendRecordList( cReplyMsg, wFileGlobalCount, 0, tRecInfoList, TRUE, szConfDir ); 

            // 找完一个目录就over
            break;
        }
        else
        {
            // 发送文件列表，空则不发送
            // guzh [8/31/2006] 由于策略修改，修改发送空列表条件
            SendRecordList( cReplyMsg, wFileGlobalCount, wFileGlobalIdx, tRecInfoList, bSendEmpy, szConfDir ); 

            wFileGlobalIdx += (u16)tRecInfoList.m_dwNum;    // 发送的全局索引号
            memset(&tRecInfoList, 0, sizeof(TListInfo));
        }

        citr ++;
    }


#if 0
	// 这里使用底层的取放象文件的接口，R2 
    g_cRecApp.PreProcFileame( szConfDir, NULL, NULL );
    if ( RP_OK != RPListFile( szConfDir, &tRecInfoList) )
	{
		log( LOGLVL_EXCEPTION, "Read asf file failed due to File NULL or Read ERROR !\n" );
	}

    SendRecordList( cServMsg, tRecInfoList.m_dwNum, 0, tRecInfoList, TRUE, NULL ); // 发送文件列表
#endif 
*/
    
    return;		
}


/*====================================================================
    函数名	     ：MsgChangeRecModeProc
	功能		 ：修改录像方式处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgChangeRecModeProc(CMessage * const pMsg)
{
	u8  byMode;
	TEqp  tEqp;
	TMt   tMt;
	u16  wErrCode =0;
			
	CServMsg cServMsg(pMsg->content,pMsg->length);
	s8* lpMsgBody	    = ( s8 * ) cServMsg.GetMsgBody();
	
	//save TMt
	memcpy( &tMt,lpMsgBody,sizeof(tMt) );
	lpMsgBody += sizeof(TMt);
	
	//save tEqp;
	memcpy( &tEqp,lpMsgBody,sizeof(tEqp) );
	lpMsgBody += sizeof(tEqp);

	byMode = (u8)*lpMsgBody;
	lpMsgBody += sizeof(u8);

	//回送应答 并进行状态上报
	cServMsg.SetErrorCode( wErrCode);
	cServMsg.SetChnIndex( m_byChnIdx );
	cServMsg.SetMsgBody( (u8*)&m_tSrvTmt, sizeof(m_tSrvTmt) );
	cServMsg.CatMsgBody( (u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp) );

	if( byMode != m_tChnnlStatus.m_byRecMode )
	{
		BOOL32 bSkip = (byMode == REC_MODE_SKIPFRAME) ? TRUE:FALSE;
        u32  dwSkipTimes = 1;       // 跳帧数目

        u16 wRet = RPSetRecorderSaveMethod(m_dwDevice, bSkip, dwSkipTimes);
		if(wRet != RP_OK )
		{
			cServMsg.SetErrorCode(ERR_REC_SETMOTHODFAILED);
			SendMsgToMcu( REC_MCU_CHANGERECMODE_NACK,&cServMsg );
			return;
		}
	}

	m_tChnnlStatus.m_byRecMode = byMode;
	SendMsgToMcu( REC_MCU_CHANGERECMODE_ACK,&cServMsg );
	SendChnNotify();	
}

/*=============================================================================
函 数 名： MsgResetRtcpParam
功    能： 重新设置录像机RTCP参数
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： 无 
修改记录    ：
日  期      版本        修改人        修改内容
2012/05/25  4.7         zhangli        create
=============================================================================*/
void CChnInst::MsgResetRtcpParam(CMessage * const pMsg)
{
	if (pMsg == NULL)
	{
		return;
	}
	
	TRecorderStatus tRecorderStaus;
	
	if(RP_OK != RPGetRecorderStatus( m_dwDevice, &tRecorderStaus))
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "Get Rec Status Error!\n");
		return;
	}
	
	CServMsg cServMsg(pMsg->content, pMsg->length);
	s8* lpMsgBody = (s8 *) cServMsg.GetMsgBody();
	
	//tEqp;
	TEqp tEqp;
	memcpy(&tEqp, lpMsgBody, sizeof(TEqp));
	lpMsgBody += sizeof(TEqp);
	
	//byIdx = 0双流；=1第一路音视频；=2第二路音视频。。。
	u8 byIdx = ~0;
	byIdx = (u8)*lpMsgBody;
	lpMsgBody += sizeof(u8);
	
	//rtcp参数
	TRecRtcpBack tRecRtcpBack;
	memcpy(&tRecRtcpBack, lpMsgBody, sizeof(TRecRtcpBack));
	lpMsgBody += sizeof(TRecRtcpBack);
	
	//取原参数
	TRPNetRcvParam tRPNetRcvParam = tRecorderStaus.m_tNetparam;

	u32 dwIp = 0;
	u16 wPort = 0;
	BOOL32 bIsParamChange = FALSE;

	if (0 == byIdx)			//双流
	{
		tRecRtcpBack.GetDStreamAddr(dwIp, wPort);
		
		if (dwIp != 0 && wPort != 0
			&& (dwIp != tRPNetRcvParam.m_atBackAddr[2].m_dwIp || wPort != tRPNetRcvParam.m_atBackAddr[2].m_wRtcpPort))
		{
			tRPNetRcvParam.m_atBackAddr[2].m_dwIp = htonl(dwIp);
			tRPNetRcvParam.m_atBackAddr[2].m_wRtcpPort = wPort;
			bIsParamChange = TRUE;
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "byIndex:%d,old param(%0x,%d), new param(%0x,%d)!\n",
				byIdx, tRPNetRcvParam.m_atBackAddr[2].m_dwIp, tRPNetRcvParam.m_atBackAddr[2].m_wRtcpPort, dwIp, wPort);
		}
	}
	else if (1 == byIdx)	//第一路音视频
	{
		tRecRtcpBack.GetAudioAddr(dwIp, wPort);
		if (dwIp != 0 && wPort != 0
			&& (dwIp != tRPNetRcvParam.m_atBackAddr[0].m_dwIp || wPort!= tRPNetRcvParam.m_atBackAddr[0].m_wRtcpPort))
		{
			tRPNetRcvParam.m_atBackAddr[0].m_dwIp = htonl(dwIp);
			tRPNetRcvParam.m_atBackAddr[0].m_wRtcpPort = wPort;
			bIsParamChange = TRUE;
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "byIndex:%d,old param(%0x,%d), new param(%0x,%d)!\n",
				byIdx, tRPNetRcvParam.m_atBackAddr[0].m_dwIp, tRPNetRcvParam.m_atBackAddr[0].m_wRtcpPort, dwIp, wPort);
		}

		tRecRtcpBack.GetVideoAddr(dwIp, wPort);	
		if (dwIp != 0 && wPort != 0
			&& (tRPNetRcvParam.m_atBackAddr[1].m_dwIp != dwIp || tRPNetRcvParam.m_atBackAddr[1].m_wRtcpPort != wPort))
		{
			tRPNetRcvParam.m_atBackAddr[1].m_dwIp = htonl(dwIp);
			tRPNetRcvParam.m_atBackAddr[1].m_wRtcpPort = wPort;
			bIsParamChange = TRUE;
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "byIndex:%d,old param(%0x,%d), new param(%0x,%d)!\n",
				byIdx, tRPNetRcvParam.m_atBackAddr[1].m_dwIp, tRPNetRcvParam.m_atBackAddr[1].m_wRtcpPort, dwIp, wPort);
		}
	}
	else if (2 == byIdx)	//第二路音视频
	{
		tRecRtcpBack.GetAudioAddr(dwIp, wPort);
		if (dwIp != 0 && wPort != 0
			&& (tRPNetRcvParam.m_atBackAddr[3].m_dwIp != dwIp || tRPNetRcvParam.m_atBackAddr[3].m_wRtcpPort != wPort))
		{
			tRPNetRcvParam.m_atBackAddr[3].m_dwIp = htonl(dwIp);
			tRPNetRcvParam.m_atBackAddr[3].m_wRtcpPort = wPort;
			bIsParamChange = TRUE;
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "byIndex:%d,old param(%0x,%d), new param(%0x,%d)!\n",
				byIdx, tRPNetRcvParam.m_atBackAddr[3].m_dwIp, tRPNetRcvParam.m_atBackAddr[3].m_wRtcpPort, dwIp, wPort);
		}
		
		tRecRtcpBack.GetVideoAddr(dwIp, wPort);	
		if (dwIp != 0 && wPort != 0
			&& (tRPNetRcvParam.m_atBackAddr[4].m_dwIp != dwIp || tRPNetRcvParam.m_atBackAddr[4].m_wRtcpPort != wPort))
		{
			tRPNetRcvParam.m_atBackAddr[4].m_dwIp = htonl(dwIp);
			tRPNetRcvParam.m_atBackAddr[4].m_wRtcpPort = wPort;
			bIsParamChange = TRUE;
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "byIndex:%d,old param(%0x,%d), new param(%0x,%d)!\n",
				byIdx, tRPNetRcvParam.m_atBackAddr[4].m_dwIp, tRPNetRcvParam.m_atBackAddr[4].m_wRtcpPort, dwIp, wPort);
		}
	}
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "byIdx is not 0 or 1!\n");
		return;
	}
	
	if (!bIsParamChange)
	{
		return;
	}

	RPSetRecNetRcvParam(m_dwDevice, &tRPNetRcvParam);
}

/*=============================================================================
函 数 名： MsgResetRcvParam
功    能： 重新设置录像机接收参数
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： 无 
修改记录    ：
日  期      版本        修改人        修改内容
2013/09/05  4.7         zhangli        create
=============================================================================*/
void CChnInst::MsgResetRcvParam(CMessage * const pMsg)
{
	if (pMsg == NULL)
	{
		return;
	}
	
	CServMsg cServMsg(pMsg->content, pMsg->length);
	s8* lpMsgBody = (s8 *) cServMsg.GetMsgBody();

	//1:audio,2:video,3:double stream,4:audio,5:video
	//如果没有录双流，第二路audio=3，类推
	u8 byStreamIdx = (u8)*lpMsgBody;
    lpMsgBody += sizeof(u8);

	if (byStreamIdx == 0 || byStreamIdx > MAX_STREAM_NUM)
	{
		return;
	}

	byStreamIdx = byStreamIdx-1;

	TDoublePayload tDoublePayload;
	memcpy(&tDoublePayload, lpMsgBody, sizeof(TDoublePayload));
	lpMsgBody += sizeof(TDoublePayload);
	memcpy(m_ptDoublePayload+byStreamIdx, &tDoublePayload, sizeof(TDoublePayload));

	TMediaEncrypt tMediaEncrypt;
	memcpy(&tMediaEncrypt, lpMsgBody, sizeof(TMediaEncrypt));
	lpMsgBody += sizeof(TMediaEncrypt);
	memcpy(m_ptMediaEncrypt+byStreamIdx, &tMediaEncrypt, sizeof(TMediaEncrypt));

	TRecorderStatus tRecorderStaus;
	
	if(RP_OK != RPGetRecorderStatus( m_dwDevice, &tRecorderStaus))
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "Get Rec Status Error!\n");
		return;
	}

	TRPNetRcvParam tNetRcvParam = tRecorderStaus.m_tNetparam;
	BOOL32 bIsDStream = tRecorderStaus.m_tRecParam.m_bLocalRec;

	// 载荷参数
    TRemotePTParam tPTParam;
    memset(&tPTParam, 0, sizeof(tPTParam));

    TEncDecParam tDecryptParam;
    memset(&tDecryptParam, 0, sizeof(tDecryptParam) );
	tDecryptParam.m_byNum = tRecorderStaus.m_tNetparam.m_byNum;

	// [5/17/2012 zhangli]五路加密参数 
	u8 abyEncKey[MAX_STREAM_NUM][MAXLEN_KEY];
	memset(abyEncKey, '\0', sizeof(abyEncKey));

	//填视频参数
	u8 byIndex = 0;
	for (byIndex = 0; byIndex < tNetRcvParam.m_byNum; ++byIndex)
	{
		if ((byIndex == 0 || byIndex == m_byAudIndex)			//音频
			|| (byIndex == m_byDsIndex && !m_byIsDstream)		//不支持双流
			|| (byIndex == m_byVidIndex && m_byVidIndex == (u8)~0))	//不支持第二路视频
		{
			continue;
		}
		
		if (m_ptMediaEncrypt[byIndex].GetEncryptMode() == CONF_ENCRYPTMODE_NONE)
		{
			// u16 wRetval;
			u8 byPayLoadValue;
			
			// zbq [10/18/2007] FEC支持
			if (m_ptDoublePayload[byIndex].GetActivePayload() == MEDIA_TYPE_FEC)
			{
				byPayLoadValue = MEDIA_TYPE_FEC;
			}            
			else if (m_ptDoublePayload[byIndex].GetRealPayLoad() == MEDIA_TYPE_H264)
			{
				byPayLoadValue = MEDIA_TYPE_H264;
			}
			else if (m_ptDoublePayload[byIndex].GetRealPayLoad() == MEDIA_TYPE_H263PLUS)
			{
				byPayLoadValue = MEDIA_TYPE_H263PLUS;
			}
			else
			{
				byPayLoadValue = 0;
			}
			
			// 第一路视频载荷
			tPTParam.m_atPTInfo[byIndex].m_byRealPT = byPayLoadValue;
			tPTParam.m_atPTInfo[byIndex].m_byRmtActivePT = byPayLoadValue;
			
			// 第一路加密参数
			tDecryptParam.m_atEncDecInfo[byIndex].m_byMode = m_ptMediaEncrypt[byIndex].GetEncryptMode();
			tDecryptParam.m_atEncDecInfo[byIndex].m_pszKeyBuf = NULL;
			tDecryptParam.m_atEncDecInfo[byIndex].m_wKeySize = 0;
		}
		else   //need encrypt
		{
			s32 nKeyLen;
			u8  byEncryptMode;
			
			// 第一路视频载荷
			// zbq [10/18/2007] FEC支持
			u8 byRealPayload = m_ptDoublePayload[byIndex].GetRealPayLoad();
			u8 byActivePayload = m_ptDoublePayload[byIndex].GetActivePayload();
			
			if ( MEDIA_TYPE_FEC == byActivePayload )
			{
				byRealPayload = MEDIA_TYPE_FEC;
			}
			tPTParam.m_atPTInfo[byIndex].m_byRealPT = byRealPayload;
			tPTParam.m_atPTInfo[byIndex].m_byRmtActivePT = byActivePayload;
			
			// 第一路加密参数
			m_ptMediaEncrypt[byIndex].GetEncryptKey(abyEncKey[byIndex], &nKeyLen); // 取加密字符串
			byEncryptMode = m_ptMediaEncrypt[byIndex].GetEncryptMode();   // 取加密模式
			if (byEncryptMode == CONF_ENCRYPTMODE_DES)
			{
				byEncryptMode = DES_ENCRYPT_MODE;
			}
			else if (byEncryptMode == CONF_ENCRYPTMODE_AES)
			{
				byEncryptMode = AES_ENCRYPT_MODE;
			}
			tDecryptParam.m_atEncDecInfo[byIndex].m_byMode = byEncryptMode;
			tDecryptParam.m_atEncDecInfo[byIndex].m_pszKeyBuf = (s8*)abyEncKey[byIndex];
			tDecryptParam.m_atEncDecInfo[byIndex].m_wKeySize = nKeyLen;
		}
	}
	
	//填音频参数
	for (byIndex = 0; byIndex < tNetRcvParam.m_byNum; ++byIndex)
	{
		if ((byIndex == 1 || byIndex == m_byDsIndex || byIndex == m_byVidIndex)		//视频
			|| (m_byAudIndex == u8(~0)))						//第二路音频为0
		{
			continue;
		}
		
		if (m_ptMediaEncrypt[byIndex].GetEncryptMode() == CONF_ENCRYPTMODE_NONE)   //not need encrypt
		{
			// zbq [10/18/2007] FEC支持
			u8 byPayLoadValue = m_ptDoublePayload[byIndex].GetRealPayLoad();
			if ( MEDIA_TYPE_FEC == m_ptDoublePayload[byIndex].GetActivePayload() )
			{
				byPayLoadValue = MEDIA_TYPE_FEC;
			}
			else if ( MEDIA_TYPE_G7221C == m_ptDoublePayload[byIndex].GetRealPayLoad() )
			{
				byPayLoadValue = MEDIA_TYPE_G7221C;
			}
			else if ( MEDIA_TYPE_AACLC == m_ptDoublePayload[byIndex].GetRealPayLoad() )
			{
				byPayLoadValue = MEDIA_TYPE_AACLC;
			}
			else if ( MEDIA_TYPE_AACLD == m_ptDoublePayload[byIndex].GetRealPayLoad() )
			{
				byPayLoadValue = MEDIA_TYPE_AACLD;
			}
			Reclog("!!!audiopayload is :%u\n", byPayLoadValue);
			
			// u16 wRetval;
			tPTParam.m_atPTInfo[byIndex].m_byRealPT = byPayLoadValue;
			tPTParam.m_atPTInfo[byIndex].m_byRmtActivePT = byPayLoadValue;
			
			tDecryptParam.m_atEncDecInfo[byIndex].m_byMode    = m_ptMediaEncrypt[byIndex].GetEncryptMode();
			tDecryptParam.m_atEncDecInfo[byIndex].m_pszKeyBuf = NULL;
			tDecryptParam.m_atEncDecInfo[byIndex].m_wKeySize  = 0;
		}
		else
		{
			s32 nKeyLen;
			u8  byEncryptMode;
			m_ptMediaEncrypt[byIndex].GetEncryptKey(abyEncKey[byIndex], &nKeyLen);
			
			// zbq [10/18/2007] FEC支持
			u8 byRealPayload = m_ptDoublePayload[byIndex].GetRealPayLoad();
			u8 byActivePayload = m_ptDoublePayload[byIndex].GetActivePayload();
			
			if ( MEDIA_TYPE_FEC == m_ptDoublePayload[byIndex].GetActivePayload() )
			{
				byRealPayload = MEDIA_TYPE_FEC;
			}
			else if ( MEDIA_TYPE_G7221C == byRealPayload )
			{
				byActivePayload = ACTIVE_TYPE_G7221C;
			}
			else if ( MEDIA_TYPE_AACLC == byRealPayload )
			{
				byActivePayload = MEDIA_TYPE_AACLC;
			}
			else if ( MEDIA_TYPE_AACLD == byRealPayload )
			{
				byActivePayload = MEDIA_TYPE_AACLD;
			}
			
			tPTParam.m_atPTInfo[byIndex].m_byRealPT = byRealPayload;
			tPTParam.m_atPTInfo[byIndex].m_byRmtActivePT = byActivePayload;
			
			byEncryptMode = m_ptMediaEncrypt[byIndex].GetEncryptMode();
			if (byEncryptMode == CONF_ENCRYPTMODE_DES)
			{
				byEncryptMode = DES_ENCRYPT_MODE;
			}
			else if (byEncryptMode == CONF_ENCRYPTMODE_AES)
			{
				byEncryptMode = AES_ENCRYPT_MODE;
			}
			tDecryptParam.m_atEncDecInfo[byIndex].m_byMode    = byEncryptMode;
			tDecryptParam.m_atEncDecInfo[byIndex].m_wKeySize  = nKeyLen;
			tDecryptParam.m_atEncDecInfo[byIndex].m_pszKeyBuf = (s8*)abyEncKey[byIndex];
		}
	}
	
	if(RP_OK != ::RPSetRemotePT( m_dwDevice, &tPTParam))
	{
		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "Fail to set remote payload\n");
		
	}
	
	if (RP_OK != ::RPSetDecryptKey( m_dwDevice, &tDecryptParam))
	{
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "Fail to set decrypt key\n");
	}
}

/*=============================================================================
  函 数 名： IsExistFile
  功    能： 文件是否存在
  算法实现： 
  全局变量： 
  参    数： s8* pchFileName
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CChnInst::IsExistFile( s8* pchFileName )
{
    BOOL32 bRet = FALSE;
    WIN32_FIND_DATA fd; 
    HANDLE hFind = FindFirstFile(pchFileName, &fd); 
    if ( INVALID_HANDLE_VALUE != hFind )
    { 
        bRet = TRUE;// exist
    }
    FindClose(hFind);
    return bRet;
}

/*====================================================================
    函数名	     ：MsgStartRecordProc
	功能		 ：开始录像消息处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    04/12/28    3.6         libo          add encrypt
    05/05/11    3.6         liuhuiyun      修改显示信息
====================================================================*/
void CChnInst::MsgStartRecordProc(CMessage * const pMsg)
{
	if (m_dwDevice <= MAXNUM_RECORDER_CHNNL)
	{
		g_adwRecDuration[m_dwDevice] = 0;
	}
	
	u16  wErrCode = ERR_REC_NOERROR;
    u16  wAckEvent;
   
//     u8  abyEncKey[MAXLEN_KEY];  // 第一路视频加密字符串
//     memset(abyEncKey, '\0', sizeof(abyEncKey));
//     u8  abyEncKey2[MAXLEN_KEY]; // 第二路视频加密字符串
//     memset(abyEncKey2, '\0', sizeof(abyEncKey2));
//     u8  abyEncKey1[MAXLEN_KEY];  // 音频加密字符串
//     memset(abyEncKey1, '\0', sizeof(abyEncKey1));

	// [5/17/2012 zhangli]五路加密参数 
	u8 abyEncKey[MAX_STREAM_NUM][MAXLEN_KEY];
	memset(abyEncKey, '\0', sizeof(abyEncKey));

    s8   achPatternName[MAX_FILE_NAME_LEN] = {0};
    s8   szMMSName[MAX_FILE_NAME_LEN]     = {0};

	TEqp tEqp;
    TRecStartPara tRecStartPara;
	/* [2012/4/5 zhangli] 0：第一路音频；1：第一路视频；2：双流；3：第二路音频；4：第二路视频，
	如果没有双流，则2为第二路音频，3为第二路视频*/
	TMediaEncrypt pMediaEncrypt[MAX_STREAM_NUM];
	TDoublePayload pDoublePayload[MAX_STREAM_NUM];

	memset(pMediaEncrypt, 0, sizeof(pMediaEncrypt));
	memset(pDoublePayload, 0, sizeof(pDoublePayload));

//    TMediaEncrypt tVideoEncrypt, tAudioEncrypt, tDoubleVideoEncrypt;
//    TDoublePayload tVideoDoublePayload, tAudioDoublePayload, tDoubleVideoPayload;
    TCapSupportEx tCapSupportEx;
    u16 wRecNameLen = 0;
    //u16 wConfInfoLen = 0;

	m_byPublishMode = PUBLISH_MODE_NONE;

	TRPNetRcvParam tNetRcvParam; // 网络接收参数
	memset(&tNetRcvParam, 0, sizeof(TRPNetRcvParam));

	CServMsg cServMsg(pMsg->content, pMsg->length);

	s8* lpMsgBody = (s8 *)cServMsg.GetMsgBody();
    u16 wMsgBodyLen = 0;
	BOOL32 bIsDStream = FALSE;		//是否双流
	u8   byNeedPrs = 0;				//是否重传
	TRecRtcpBack tRtcpBack ;		//RTCP回馈地址

	u8 byVideoNum = 0;
	u8 byAudioNum = 0;

	TAudioTypeDesc tAudioType;

	// 终端将此封装成了一个结构体
	if (IsMtConnect())
	{
		TRecStartParam tRecStartParam;
	    memcpy((void *)&tRecStartParam, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
	
		// //save tEqp
		memset(&tEqp, 0 ,sizeof(tEqp));

		//TRecStartPara
		memcpy((void *)&tRecStartPara, (void *)&(tRecStartParam.tRecStartPara), sizeof(TRecStartPara));
		
		//  [7/28/2010 liuxu]
		//m_byPublishMode    = tRecStartPara.GetPublishMode();
		//m_byPublishLevel   = tRecStartPara.GetPublishLevel();
		
		//  [7/28/2010 liuxu]
		m_byPublishMode = PUBLISH_MODE_IMMED;
		m_byPublishLevel = PUBLISH_LEVEL_NORMAL;
		
		bIsDStream = tRecStartPara.IsRecDStream(); // 是否双流选项,TRUE：双流，FALSE：普通
		
		// tRecStartPara是否要加此变量
		byNeedPrs = tRecStartParam.byNeedPrs;         //是否重传
		
		memcpy(&tRtcpBack, &(tRecStartParam.tRecRtcpBack), sizeof(tRtcpBack));
		
		// 视频
		memcpy(&pMediaEncrypt[1], &(tRecStartParam.tPriVideoEncrypt), sizeof(TMediaEncrypt));
		
		// 视频载荷
		memcpy(&pDoublePayload[1], &(tRecStartParam.tPriPayload), sizeof(TDoublePayload));
		
		// 音频
		memcpy(&pMediaEncrypt[0], &(tRecStartParam.tAudVideoEncrypt), sizeof(TMediaEncrypt));
		
		// 音频载荷
		memcpy(&pDoublePayload[0], &(tRecStartParam.tAudPayload), sizeof(TDoublePayload));
		
		if (bIsDStream)
		{
			// 第二路视频
			memcpy(&pMediaEncrypt[2], &(tRecStartParam.tSecVideoEncrypt), sizeof(TMediaEncrypt));
			
			// 第二路视频载荷
			memcpy(&pDoublePayload[2], &(tRecStartParam.tSecPayload), sizeof(TDoublePayload));
		}
		
		// zbq [10/11/12] 前向纠错. FIXME: 现在暂时用不到结构变量
		memcpy(&tCapSupportEx, &(tRecStartParam.tCapSupportEx), sizeof(tCapSupportEx));
		
 		// 录像名， 格式： abc_255_384K
		memcpy((void *)m_achFileName, tRecStartParam.achFileName, strlen(tRecStartParam.achFileName) + 1);
	}
	else
	{
		//save TMt
		memcpy(&m_tSrvTmt, lpMsgBody, sizeof(m_tSrvTmt));
		lpMsgBody += sizeof(TMt);
		wMsgBodyLen += sizeof(TMt);
		
		//save tEqp
		memcpy(&tEqp, lpMsgBody, sizeof(tEqp));
		lpMsgBody += sizeof(tEqp);
		wMsgBodyLen += sizeof(tEqp);
		
		//TRecStartPara
		memcpy(&tRecStartPara, lpMsgBody, sizeof(TRecStartPara));
		lpMsgBody += sizeof(TRecStartPara);
		wMsgBodyLen += sizeof(TRecStartPara);
		
		m_byPublishMode    = tRecStartPara.GetPublishMode();
		m_byPublishLevel   = tRecStartPara.GetPublishLevel();
		bIsDStream = tRecStartPara.IsRecDStream(); // 是否双流选项,TRUE：双流，FALSE：普通
		
		byNeedPrs = *(lpMsgBody);         //是否重传
		lpMsgBody += sizeof(u8);
		wMsgBodyLen += sizeof(u8);
		
		tRtcpBack = *(TRecRtcpBack*)lpMsgBody; //RTCP回馈地址
		lpMsgBody += sizeof(TRecRtcpBack);
		wMsgBodyLen += sizeof(TRecRtcpBack);
		
		// 视频
		pMediaEncrypt[1] = *(TMediaEncrypt *)lpMsgBody;
		lpMsgBody += sizeof(TMediaEncrypt);
		wMsgBodyLen += sizeof(TMediaEncrypt);
		
		// 视频载荷
		pDoublePayload[1] = *(TDoublePayload *)lpMsgBody;
		lpMsgBody += sizeof(TDoublePayload);
		wMsgBodyLen += sizeof(TDoublePayload);
		
		// 音频
		pMediaEncrypt[0] = *(TMediaEncrypt *)lpMsgBody;
		lpMsgBody += sizeof(TMediaEncrypt);
		wMsgBodyLen += sizeof(TMediaEncrypt);
		
		// 音频载荷
		pDoublePayload[0] = *(TDoublePayload *)lpMsgBody;
		lpMsgBody += sizeof(TDoublePayload);
		wMsgBodyLen += sizeof(TDoublePayload);
		

		// 第二路视频
		//只有支持双流才存到pMediaEncrypt数组中
		if (bIsDStream)
		{
			pMediaEncrypt[2] = *(TMediaEncrypt*)lpMsgBody;
		}
		lpMsgBody += sizeof(TMediaEncrypt);
		wMsgBodyLen += sizeof(TMediaEncrypt);
		
		// 第二路视频载荷
		if (bIsDStream)
		{
			pDoublePayload[2] = *(TDoublePayload*)lpMsgBody;
		}
		lpMsgBody += sizeof(TDoublePayload);
		wMsgBodyLen += sizeof(TDoublePayload);
		
		// zbq [10/11/12] 前向纠错. FIXME: 现在暂时用不到结构变量
		tCapSupportEx = *(TCapSupportEx*)lpMsgBody;
		lpMsgBody += sizeof(TCapSupportEx);
		wMsgBodyLen += sizeof(TCapSupportEx);
		
		wRecNameLen = ntohs(*(u16*)lpMsgBody);
		lpMsgBody += sizeof(u16);
		wMsgBodyLen += sizeof(u16);
		
		// 录像名， 格式： abc_255_384K
		memcpy( m_achFileName, lpMsgBody, wRecNameLen );

		lpMsgBody += wRecNameLen;
		wMsgBodyLen += wRecNameLen;

		if (wMsgBodyLen < cServMsg.GetMsgBodyLen())
		{
			//会议信息，TConfInfo，未用，不解析，加上长度
			u16 wConfInfoLen = ntohs(*(u16*)lpMsgBody);
			lpMsgBody += sizeof(u16);
			wMsgBodyLen += sizeof(u16);
			
			//
			lpMsgBody += wConfInfoLen;
			wMsgBodyLen += wConfInfoLen;
		}

		//录像机视频扩展信息
		if (wMsgBodyLen < cServMsg.GetMsgBodyLen())
		{
			byVideoNum = *(u8*)lpMsgBody;
			lpMsgBody += sizeof(u8);
			wMsgBodyLen += sizeof(u8);
			
			if (byVideoNum > 0)
			{
				u8 byIdx = 4;
				//不支持双流，向前挪一位
				if (!bIsDStream)
				{
					byIdx = 3;
				}
				
				pMediaEncrypt[byIdx] = *(TMediaEncrypt *)lpMsgBody;
				lpMsgBody += sizeof(TMediaEncrypt);
				wMsgBodyLen += sizeof(TMediaEncrypt);
				
				pDoublePayload[byIdx] = *(TDoublePayload *)lpMsgBody;
				lpMsgBody += sizeof(TDoublePayload);
				wMsgBodyLen += sizeof(TDoublePayload);
				
				if (byNeedPrs)
				{
					u32 dwIp = ntohs(*(u32*)lpMsgBody);
					lpMsgBody += sizeof(u32);
					wMsgBodyLen += sizeof(u32);
					tNetRcvParam.m_atBackAddr[byIdx].m_dwIp = htonl(dwIp);

					u16 wPort = ntohs(*(u16*)lpMsgBody);
					lpMsgBody += sizeof(u16);
					wMsgBodyLen += sizeof(u16);
					tNetRcvParam.m_atBackAddr[byIdx].m_wRtcpPort = wPort;
				}
			}
		}
		
		//录像机音频扩展信息
		if (wMsgBodyLen < cServMsg.GetMsgBodyLen())
		{
			byAudioNum = *(u8*)lpMsgBody;
			lpMsgBody += sizeof(u8);
			wMsgBodyLen += sizeof(u8);

			if (byAudioNum > 0)
			{
				u8 byIdx = 3;
				//不支持双流，向前挪一位
				if (!bIsDStream)
				{
					byIdx = 2;
				}
				
				pMediaEncrypt[byIdx] = *(TMediaEncrypt *)lpMsgBody;
				lpMsgBody += sizeof(TMediaEncrypt);
				wMsgBodyLen += sizeof(TMediaEncrypt);
				
				pDoublePayload[byIdx] = *(TDoublePayload *)lpMsgBody;
				
				lpMsgBody += sizeof(TDoublePayload);
				wMsgBodyLen += sizeof(TDoublePayload);
				
				if (byNeedPrs)
				{
					u32 dwIp = ntohs(*(u32*)lpMsgBody);
					lpMsgBody += sizeof(u32);
					wMsgBodyLen += sizeof(u32);
					tNetRcvParam.m_atBackAddr[byIdx].m_dwIp = htonl(dwIp);
					
					u16 wPort = ntohs(*(u16*)lpMsgBody);
					lpMsgBody += sizeof(u16);
					wMsgBodyLen += sizeof(u16);
					tNetRcvParam.m_atBackAddr[byIdx].m_wRtcpPort = wPort;
				}
			}
		}

		//音频双声道信息
		if (wMsgBodyLen < cServMsg.GetMsgBodyLen())
		{
			u8 byAudioTypeLen = *(u8*)lpMsgBody;
			lpMsgBody += sizeof(u8);
			wMsgBodyLen += sizeof(u8);
			
			if (byAudioTypeLen > 0)
			{
				tAudioType = *(TAudioTypeDesc *)lpMsgBody;
				lpMsgBody += sizeof(TAudioTypeDesc);
				wMsgBodyLen += sizeof(TAudioTypeDesc);
			}
		}
	}

    strcat( m_achFileName, ".asf" );
    u8 byFileOwnerGrp;
    u16 wFileBitrate;
    CString cstrPureName;   // 格式 abc
    g_cRecApp.GetParamFromFilename(m_achFileName, byFileOwnerGrp, wFileBitrate, cstrPureName);
	
	wErrCode = g_cRecApp.CheckPureName(cstrPureName);
	if ( ERR_REC_NOERROR != wErrCode )
	{
		cServMsg.SetChnIndex( m_byChnIdx );
		cServMsg.SetErrorCode( wErrCode );
		cServMsg.SetMsgBody( (u8*)&m_tSrvTmt, sizeof(TMt) );
		cServMsg.CatMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
		SendMsgToMcu( REC_MCU_STARTREC_NACK, &cServMsg);
		
		LogPrint(LOG_LVL_DETAIL, 
			MID_RECEQP_REC,
			"Pure file name check failed\n", m_achFileName);
		
		return;
	}


    g_cRecApp.PreProcPatternName(achPatternName, cstrPureName, byFileOwnerGrp, wFileBitrate );    
    u16 wFullNameSize;     //add by jlb  081026
	// [8/31/2010 liuxu] 添加了终端和mcu的识别
	if (IsMtConnect())
	{
		wFullNameSize = g_cRecApp.PreProcMtFileName(m_achFullName, m_achFileName, NULL, GETINS(m_dwMtInstId));  // 格式  e:\vod\mcuname/abc.asf
	}else
	{
		wFullNameSize = g_cRecApp.PreProcFileame(m_achFullName, m_achFileName, NULL);  // 格式  e:\vod\mcuname/abc.asf
	}
    

    if (0 == wFullNameSize)
    { 
		cServMsg.SetChnIndex( m_byChnIdx );
	    cServMsg.SetErrorCode( ERR_REC_FILENAMETOOLONG );
	    cServMsg.SetMsgBody( (u8*)&m_tSrvTmt, sizeof(TMt) );
	    cServMsg.CatMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
	    SendMsgToMcu( REC_MCU_STARTREC_NACK, &cServMsg);

		LogPrint(LOG_LVL_DETAIL, 
			MID_RECEQP_REC,
			"Failed to start record  %s! because file name too long.\n", m_achFileName);
		
		return;
    }
	g_cRecApp.Path2MMS( m_achFullName, szMMSName );
    lpMsgBody += wRecNameLen;
    wMsgBodyLen += wRecNameLen;


    if (g_bPrintRecLog)
    {
        Reclog("Start Record Parametres: \n");  
        tRecStartPara.Print();
        OspPrintf(TRUE, FALSE, "Need Prs?: %d\n", byNeedPrs);        
        OspPrintf(TRUE, FALSE, "First Channel Video Payload: %d/%d(Act/Real)\n", 
                                pDoublePayload[1].GetActivePayload(), 
                                pDoublePayload[1].GetRealPayLoad());
        OspPrintf(TRUE, FALSE, "Second Channel Video Payload: %d/%d(Act/Real)\n", 
                                pDoublePayload[2].GetActivePayload(), 
                                pDoublePayload[2].GetRealPayLoad());
        OspPrintf(TRUE, FALSE, "Audio Payload: %d/%d(Act/Real)\n", 
                                pDoublePayload[0].GetActivePayload(), 
                                pDoublePayload[0].GetRealPayLoad());
        OspPrintf(TRUE, FALSE, "Filename: %s\n", m_achFileName);
        OspPrintf(TRUE, FALSE, "Full filename: %s\n", m_achFullName);
        OspPrintf(TRUE, FALSE, "Pattern name: %s\n", achPatternName);
        OspPrintf(TRUE, FALSE, "MMS name: %s\n", szMMSName);

    }

    BOOL32 bIsExistFile = IsExistFile( m_achFullName );
    if( TRUE == bIsExistFile )
    {
        cServMsg.SetChnIndex( m_byChnIdx );
		cServMsg.SetErrorCode( ERR_REC_FILEEXISTED );
        cServMsg.SetMsgBody( (u8*)&m_tSrvTmt, sizeof(TMt) );
		cServMsg.CatMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
		
		SendMsgToMcu( REC_MCU_STARTREC_NACK, &cServMsg);

		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "The file has existed\n");
        
        return;
    }


    //[11/10/2006-zbq] 文件名的判断处理已经由会控完成，这里不必作多余校验
	// 判断制定的文件是否已使用
    s32 nLen = strlen(m_achFullName);
	for( s32 nLop = 1; nLop <= MAXNUM_RECORDER_CHNNL; nLop++)
	{
		if( 0 == _strnicmp(g_cRecApp.m_achUsingFile[nLop], m_achFullName, nLen) )
		{
            // 该文件正在被操作
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "The file: %s is operating by another channel.\n", m_achFileName);

            cServMsg.SetChnIndex( m_byChnIdx );
			cServMsg.SetErrorCode( ERR_REC_FILEUSING );
			cServMsg.SetMsgBody( (u8*)&m_tSrvTmt, sizeof(TMt) );
		    cServMsg.CatMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
			
			SendMsgToMcu( REC_MCU_STARTREC_NACK, &cServMsg);
			return;
		}
	}
    
    // 录像参数
    TRecParam tRecParam;
    memset( &tRecParam, 0, sizeof(TRecParam) );
	tRecParam.m_dwStartTime = 1;

	//第一个文件以视频关键帧开始存, zgc, 2007-10-16
	//zhouyiliang 20120322 如果会议录像只有混音没有视频源，也要让其开始录
	tRecParam.m_bVideoFirst = FALSE;
    
    //  形成录像库要求的文件
    g_cRecApp.SplitPath( m_achFullName, tRecParam.m_achRecFilePath, tRecParam.m_achRecFileName, TRUE );
    
	//网络接收参数
	tNetRcvParam.m_byNum = 2;

    tNetRcvParam.m_atAddr[0].m_dwIp  = m_dwRecIpAddr;  // 音频
    tNetRcvParam.m_atAddr[0].m_wPort = m_wRecPort+2;

    tNetRcvParam.m_atAddr[1].m_dwIp  = m_dwRecIpAddr;  // 第一视频
    tNetRcvParam.m_atAddr[1].m_wPort = m_wRecPort;

	//双流在数组中的序号 
	u8 byDsIndex = ~0;
	if (bIsDStream)
	{
		byDsIndex = tNetRcvParam.m_byNum;
		tNetRcvParam.m_atAddr[byDsIndex].m_dwIp = m_dwRecIpAddr; // 第二路视频
		tNetRcvParam.m_atAddr[byDsIndex].m_wPort = m_wRecPort + 4;
		tNetRcvParam.m_byNum++;
	}
    
	u8 byAudIndex = ~0;
	if (byAudioNum > 0)
	{
		byAudIndex = tNetRcvParam.m_byNum;
		tNetRcvParam.m_atAddr[byAudIndex].m_dwIp  = m_dwRecIpAddr;  // 第二路音频
		tNetRcvParam.m_atAddr[byAudIndex].m_wPort = m_wRecPort+8;
		tNetRcvParam.m_byNum++;
	}

	u8 byVidIndex = ~0;
	if (byVideoNum > 0)
	{
		byVidIndex = tNetRcvParam.m_byNum;
		tNetRcvParam.m_atAddr[byVidIndex].m_dwIp  = m_dwRecIpAddr;  // 第二路视频
		tNetRcvParam.m_atAddr[byVidIndex].m_wPort = m_wRecPort+6;
		tNetRcvParam.m_byNum++;
	}

	m_byDsIndex = byDsIndex;
	m_byAudIndex = byAudIndex;
	m_byVidIndex = byVidIndex;
	m_byIsDstream = bIsDStream;

	LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_REC, "byNeedPrs: %d", byNeedPrs);

	TRPRecRSParam tRPRecRsParam;
	memset(&tRPRecRsParam, 0, sizeof(tRPRecRsParam));
    if(0 != byNeedPrs)
    {
        u32 dwIp = 0;
        u16 wPort = 0;
		//音频RTCP
        tRtcpBack.GetAudioAddr(dwIp, wPort);
        tNetRcvParam.m_atBackAddr[0].m_dwIp = htonl(dwIp);		
		if ( IS_CONF_RECORD(m_tSrvTmt) )
		{
			tNetRcvParam.m_atBackAddr[0].m_wRtcpPort = wPort+m_byChnIdx*PORTSPAN;
		}
		else
		{
			tNetRcvParam.m_atBackAddr[0].m_wRtcpPort = wPort;	
		}       
        Reclog("The Audio: %0x, %d\n", dwIp, tNetRcvParam.m_atBackAddr[0].m_wRtcpPort);

		//视频RTCP
        dwIp = 0;
        wPort = 0;
        tRtcpBack.GetVideoAddr(dwIp, wPort);
        tNetRcvParam.m_atBackAddr[1].m_dwIp = htonl(dwIp);
		if ( IS_CONF_RECORD(m_tSrvTmt) )
		{
			tNetRcvParam.m_atBackAddr[1].m_wRtcpPort = wPort+m_byChnIdx*PORTSPAN;
		}
		else
		{
		    tNetRcvParam.m_atBackAddr[1].m_wRtcpPort = wPort;
		} 
        Reclog("The Video: %0x, %d\n", dwIp, tNetRcvParam.m_atBackAddr[1].m_wRtcpPort);
		
		//双流RTCP
        if( TRUE == bIsDStream )
        {
            dwIp = 0;
            wPort = 0;
            tRtcpBack.GetDStreamAddr(dwIp, wPort);
            
            tNetRcvParam.m_atBackAddr[2].m_dwIp = htonl(dwIp);
			if ( IS_CONF_RECORD(m_tSrvTmt) )
			{
				 tNetRcvParam.m_atBackAddr[2].m_wRtcpPort = wPort+m_byChnIdx*PORTSPAN;
			}
			else
			{
				tNetRcvParam.m_atBackAddr[2].m_wRtcpPort = wPort;
			}
            Reclog("The DVideo: %0x, %d\n", dwIp, tNetRcvParam.m_atBackAddr[2].m_wRtcpPort);
        }
		//第二路音视频已经在解析时存入相应tNetRcvParam.m_atBackAddr

        for (u8 byIndex = 0; byIndex < tNetRcvParam.m_byNum; ++byIndex)
        {
			if ((byIndex == byDsIndex && !bIsDStream)
				|| (byIndex == byAudIndex && byAudioNum == 0)
				|| (byIndex == byVidIndex && byVideoNum == 0))
			{
				continue;
			}

			tRPRecRsParam.m_abRepeatSnd[byIndex] = TRUE;

			tRPRecRsParam.m_atRsParam[byIndex].m_wFirstTimeSpan = ntohs(g_cRecApp.m_PrsParam.m_wFirstTimeSpan);
			tRPRecRsParam.m_atRsParam[byIndex].m_wSecondTimeSpan = ntohs(g_cRecApp.m_PrsParam.m_wSecondTimeSpan);
			tRPRecRsParam.m_atRsParam[byIndex].m_wThirdTimeSpan = ntohs(g_cRecApp.m_PrsParam.m_wThirdTimeSpan);
			tRPRecRsParam.m_atRsParam[byIndex].m_wRejectTimeSpan = ntohs(g_cRecApp.m_PrsParam.m_wRejectTimeSpan);
        }
    }

// 	memcpy(&m_tNetRcvParam, &tNetRcvParam, sizeof(tNetRcvParam));
    u16 wRet = ::RPSetRecNetRcvParam( m_dwDevice, &tNetRcvParam);  // 设置网络接受参数
    if(RP_OK != wRet)
    {
        
		LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_REC, "Fail to set recorder network receive param, Error:%d\n", wRet);
		// Nack给mcu
		cServMsg.SetChnIndex( m_byChnIdx );
		cServMsg.SetErrorCode( ERR_REC_CALLLIBFAILED );
		cServMsg.SetMsgBody( (u8*)&m_tSrvTmt, sizeof(TMt) );
		cServMsg.CatMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
		SendMsgToMcu( REC_MCU_STARTREC_NACK, &cServMsg);
        return;
    }
    else
    {
        Reclog("Ok to set network param, called RPSetRecNetRcvParam\n");
    }
	

	if(0 != byNeedPrs)
	{
		wRet = RPSetRecRsParam( m_dwDevice, &tRPRecRsParam);	
		if(RP_OK != wRet)
		{
			Reclog("Fail to call RPSetRecRsParam, error: %d\n", wRet);
		}
		else
		{
			Reclog("Ok to set Prs Param, called RPSetRecRsParam\n");
		}		
	}



    // 载荷参数
    TRemotePTParam tPTParam;
    memset(&tPTParam, 0, sizeof(tPTParam));

    TEncDecParam tDecryptParam;
    memset(&tDecryptParam, 0, sizeof(tDecryptParam) );
	tDecryptParam.m_byNum = tNetRcvParam.m_byNum;
    
    //码率参数
    TStreamAverageBitrate tStreamBitRate;
    memset( &tStreamBitRate, 0, sizeof(tStreamBitRate) );

	tStreamBitRate.m_adwStreamAverageBitrate[1] = GetRPBitRate(tRecStartPara.GetBitRate(), pDoublePayload[1].GetRealPayLoad());

    //双流情况
    if (bIsDStream)
    {
        tStreamBitRate.m_adwStreamAverageBitrate[byDsIndex] 
			= GetRPBitRate(tRecStartPara.GetSecBitRate(), pDoublePayload[byDsIndex].GetRealPayLoad());
    }

	//第二路视频
    if (byVideoNum > 0)
    {
		tStreamBitRate.m_adwStreamAverageBitrate[byVidIndex] 
			= GetRPBitRate( tRecStartPara.GetBitRate(), pDoublePayload[byVidIndex].GetRealPayLoad());
    }

	cServMsg.SetMsgBody();
	// 初始化为成功状态
	wErrCode       = 0;
	wAckEvent      = REC_MCU_STARTREC_ACK;
	m_tChnnlStatus.m_byRecMode = REC_MODE_NORMAL;
    u16 wInstId    = 0;

	switch( CurState() )
	{
	case TRecChnnlStatus::STATE_RECREADY:
		{
			//填视频参数
			u8 byIndex = 0;
			for (byIndex = 0; byIndex < tNetRcvParam.m_byNum; ++byIndex)
			{
				if ((byIndex == 0 || byIndex == byAudIndex)			//音频
					|| (byIndex == byDsIndex && !bIsDStream)		//不支持双流
					|| (byIndex == byVidIndex && byVideoNum == 0))	//不支持第二路视频
				{
					continue;
				}
				
				if (pMediaEncrypt[byIndex].GetEncryptMode() == CONF_ENCRYPTMODE_NONE)
				{
					// u16 wRetval;
					u8 byPayLoadValue;
					
					// zbq [10/18/2007] FEC支持
					if (pDoublePayload[byIndex].GetActivePayload() == MEDIA_TYPE_FEC)
					{
						byPayLoadValue = MEDIA_TYPE_FEC;
					}            
					else if (pDoublePayload[byIndex].GetRealPayLoad() == MEDIA_TYPE_H264)
					{
						byPayLoadValue = MEDIA_TYPE_H264;
					}
					else if (pDoublePayload[byIndex].GetRealPayLoad() == MEDIA_TYPE_H263PLUS)
					{
						byPayLoadValue = MEDIA_TYPE_H263PLUS;
					}
					else
					{
						byPayLoadValue = 0;
					}
					
					// 第一路视频载荷
					tPTParam.m_atPTInfo[byIndex].m_byRealPT = byPayLoadValue;
					tPTParam.m_atPTInfo[byIndex].m_byRmtActivePT = byPayLoadValue;
					
					// 第一路加密参数
					tDecryptParam.m_atEncDecInfo[byIndex].m_byMode = pMediaEncrypt[byIndex].GetEncryptMode();
					tDecryptParam.m_atEncDecInfo[byIndex].m_pszKeyBuf = NULL;
					tDecryptParam.m_atEncDecInfo[byIndex].m_wKeySize = 0;
				}
				else   //need encrypt
				{
					s32 nKeyLen;
					u8  byEncryptMode;
					
					// 第一路视频载荷
					// zbq [10/18/2007] FEC支持
					u8 byRealPayload = pDoublePayload[byIndex].GetRealPayLoad();
					u8 byActivePayload = pDoublePayload[byIndex].GetActivePayload();
					
					if ( MEDIA_TYPE_FEC == byActivePayload )
					{
						byRealPayload = MEDIA_TYPE_FEC;
					}
					tPTParam.m_atPTInfo[byIndex].m_byRealPT = byRealPayload;
					tPTParam.m_atPTInfo[byIndex].m_byRmtActivePT = byActivePayload;
					
					// 第一路加密参数
					pMediaEncrypt[byIndex].GetEncryptKey(abyEncKey[byIndex], &nKeyLen); // 取加密字符串
					byEncryptMode = pMediaEncrypt[byIndex].GetEncryptMode();   // 取加密模式
					if (byEncryptMode == CONF_ENCRYPTMODE_DES)
					{
						byEncryptMode = DES_ENCRYPT_MODE;
					}
					else if (byEncryptMode == CONF_ENCRYPTMODE_AES)
					{
						byEncryptMode = AES_ENCRYPT_MODE;
					}
					tDecryptParam.m_atEncDecInfo[byIndex].m_byMode = byEncryptMode;
					tDecryptParam.m_atEncDecInfo[byIndex].m_pszKeyBuf = (s8*)abyEncKey[byIndex];
					tDecryptParam.m_atEncDecInfo[byIndex].m_wKeySize = nKeyLen;
				}
			}
			
			//填音频参数
			for (byIndex = 0; byIndex < tNetRcvParam.m_byNum; ++byIndex)
			{
				if ((byIndex == 1 || byIndex == byDsIndex || byIndex == byVidIndex)		//视频
					|| (byIndex == byAudIndex && byAudioNum == 0))						//第二路音频为0
				{
					continue;
				}
				
				if (pMediaEncrypt[byIndex].GetEncryptMode() == CONF_ENCRYPTMODE_NONE)   //not need encrypt
				{
					// zbq [10/18/2007] FEC支持
					u8 byPayLoadValue = pDoublePayload[byIndex].GetRealPayLoad();
					if ( MEDIA_TYPE_FEC == pDoublePayload[byIndex].GetActivePayload() )
					{
						byPayLoadValue = MEDIA_TYPE_FEC;
					}
					else if ( MEDIA_TYPE_G7221C == pDoublePayload[byIndex].GetRealPayLoad() )
					{
						byPayLoadValue = MEDIA_TYPE_G7221C;
					}
					else if ( MEDIA_TYPE_AACLC == pDoublePayload[byIndex].GetRealPayLoad() )
					{
						byPayLoadValue = MEDIA_TYPE_AACLC;
					}
					else if ( MEDIA_TYPE_AACLD == pDoublePayload[byIndex].GetRealPayLoad() )
					{
						byPayLoadValue = MEDIA_TYPE_AACLD;
					}
					Reclog("!!!audiopayload is :%u\n", byPayLoadValue);
					
					// u16 wRetval;
					tPTParam.m_atPTInfo[byIndex].m_byRealPT = byPayLoadValue;
					tPTParam.m_atPTInfo[byIndex].m_byRmtActivePT = byPayLoadValue;
					
					tDecryptParam.m_atEncDecInfo[byIndex].m_byMode    = pMediaEncrypt[byIndex].GetEncryptMode();
					tDecryptParam.m_atEncDecInfo[byIndex].m_pszKeyBuf = NULL;
					tDecryptParam.m_atEncDecInfo[byIndex].m_wKeySize  = 0;
				}
				else
				{
					s32 nKeyLen;
					u8  byEncryptMode;
					pMediaEncrypt[byIndex].GetEncryptKey(abyEncKey[byIndex], &nKeyLen);
					
					// zbq [10/18/2007] FEC支持
					u8 byRealPayload = pDoublePayload[byIndex].GetRealPayLoad();
					u8 byActivePayload = pDoublePayload[byIndex].GetActivePayload();
					
					if ( MEDIA_TYPE_FEC == pDoublePayload[byIndex].GetActivePayload() )
					{
						byRealPayload = MEDIA_TYPE_FEC;
					}
					else if ( MEDIA_TYPE_G7221C == byRealPayload )
					{
						byActivePayload = ACTIVE_TYPE_G7221C;
					}
					else if ( MEDIA_TYPE_AACLC == byRealPayload )
					{
						byActivePayload = MEDIA_TYPE_AACLC;
					}
					else if ( MEDIA_TYPE_AACLD == byRealPayload )
					{
						byActivePayload = MEDIA_TYPE_AACLD;
					}
					
					tPTParam.m_atPTInfo[byIndex].m_byRealPT = byRealPayload;
					tPTParam.m_atPTInfo[byIndex].m_byRmtActivePT = byActivePayload;
					
					byEncryptMode = pMediaEncrypt[byIndex].GetEncryptMode();
					if (byEncryptMode == CONF_ENCRYPTMODE_DES)
					{
						byEncryptMode = DES_ENCRYPT_MODE;
					}
					else if (byEncryptMode == CONF_ENCRYPTMODE_AES)
					{
						byEncryptMode = AES_ENCRYPT_MODE;
					}
					tDecryptParam.m_atEncDecInfo[byIndex].m_byMode    = byEncryptMode;
					tDecryptParam.m_atEncDecInfo[byIndex].m_wKeySize  = nKeyLen;
					tDecryptParam.m_atEncDecInfo[byIndex].m_pszKeyBuf = (s8*)abyEncKey[byIndex];
				}
			}
			
			wRet = ::RPSetRemotePT( m_dwDevice, &tPTParam);
			if(RP_OK != wRet)
			{
				LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "Fail to set remote payload\n");
				
			}
			else
			{
				Reclog("Ok to set remote payload, called RPSetRemotePT\n");
			}
			
			wRet = ::RPSetDecryptKey( m_dwDevice, &tDecryptParam);

			if(RP_OK != wRet)
			{
				LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "Fail to set decrypt key\n");
				
			}
			else
			{
				Reclog("Ok to set decrypt key, called RPSetDecryptKey\n");
			}
			
			wRet = ::RPSetAverageBitrate( m_dwDevice, tStreamBitRate );
			if ( RP_OK != wRet )
			{
				LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "Fail to set stream bitrate<%d:%d:%d>.", 
					tStreamBitRate.m_adwStreamAverageBitrate[0],
					tStreamBitRate.m_adwStreamAverageBitrate[1],
					tStreamBitRate.m_adwStreamAverageBitrate[2] );
				
			}
			else
			{
				LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "OK to set stream bitrate<%d:%d:%d>. \n", 
					tStreamBitRate.m_adwStreamAverageBitrate[0],
					tStreamBitRate.m_adwStreamAverageBitrate[1],
					tStreamBitRate.m_adwStreamAverageBitrate[2] );
			}
			
			//设置音频通道类型
			RPSetAudioTypeDesc(m_dwDevice, tAudioType.GetAudioTrackNum());
			
			// 注册文件大小跳转回调，回调上下文注册为回调模式和回调级别[11/15/2006-zbq]
			wInstId = GetInsID();
			tRecParam.m_dwContext = (wInstId << 16) | (m_byPublishLevel << 8) | m_byPublishMode;
			tRecParam.m_pRecBigFileSwCB = RecFileSizeOverDefCallBack;
			
			// guzh [7/17/2007] 设置Pattern
			::RPSetSwitchFileNamePattern(m_dwDevice, achPatternName);
			//SetTimer(EV_REC_MCU_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);   //jlb 090223
			wRet = ::RPStartRec( m_dwDevice, &tRecParam);
			Reclog("called RPStartRec, the m_dwDevice: %d, m_bVidFirst.%d\n", m_dwDevice, tRecParam.m_bVideoFirst);
			
			/************************************************************************/
			/* u16 rpStartRecord(u16 wRecorderID, s8 *pszFileName, 
			u32 dwVideoIPAddress, u16 wVideoPort, u32 dwAudioIPAddress, u16 wAudioPort, 
			EmRecordMode emMode);                                                                      */
			/************************************************************************/
			struct in_addr in;
			in.s_addr = m_dwRecIpAddr;
			Reclog("The Record Ip: %s, VedioPort: %d, AudioPort: %d, VideoPort 2: %d\n", 
				inet_ntoa(in), m_wRecPort, m_wRecPort+2, m_wRecPort+4);
			
			if (wRet != RP_OK)
			{
				wAckEvent = REC_MCU_STARTREC_NACK;
				wErrCode  = ERR_REC_CALLLIBFAILED;
				
				LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "Device.%d Record %s ( %p@%d )failed.\n", m_dwDevice,
					m_achFileName, m_dwRecIpAddr, m_wRecPort );
				
				LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "The ERROR code: %d\n", wRet);
			}
			else
			{
				m_tChnnlStatus.SetRecordName(m_achFileName);
				
				m_cConfId	= cServMsg.GetConfId();
				Reclog("Device.%d Record %s ( %p@%d ) Success.\n", m_dwDevice,
					m_achFileName, m_dwRecIpAddr, m_wRecPort );
				
				m_tChnnlStatus.m_byRecMode = tRecStartPara.GetRecMode();
				BOOL32 bSkipMode = REC_MODE_SKIPFRAME == tRecStartPara.GetRecMode();
				wRet = ::RPSetRecorderSaveMethod(m_dwDevice, bSkipMode, (u32)1);
				if (wRet != RP_OK)
				{
					// guzh [9/8/2006] 带错误码回Ack
					wAckEvent = REC_MCU_STARTREC_ACK;
					wErrCode  = ERR_REC_SETMOTHODFAILED;
					
					LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "Rec_ch.%d set skip failed(%d)!", m_dwDevice, wRet);	
				}

				memset(m_achOrignalFileName, 0, sizeof(m_achOrignalFileName));
				memcpy(m_achOrignalFileName, m_achFileName, strlen(m_achFileName));
				if (PUBLISH_MODE_IMMED == m_byPublishMode && g_tCfg.m_bOpenDBSucceed)
				{
					//这两个值为NULL，重新连接media server
					if (NULL == g_pServer || NULL == g_pPubPoints)
					{
						LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "meida server is invalid, init com again.\n");
						InitCOMServer();
					}
					
					//这两个值为NULL，则按照即录即播处理
					if (NULL == g_pServer || NULL == g_pPubPoints)
					{
						wRet = PublishOneFile(m_achFullName, FALSE);
						
						LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "the recorder isn't support broadcast point.\n");
					}
					else
					{
						if (CreateBrdPubPoint())
						{
							wRet = AddOneLiveSource();
						}
						else
						{
							LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "CreateBrdPubPoint failed, meida server is invalid, init com again.\n");
							
							if (InitCOMServer())
							{
								if (CreateBrdPubPoint())
								{
									wRet = AddOneLiveSource();
								}
								else
								{
									LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "CreateBrdPubPoint failed again.\n");
								}
							}
							else
							{
								LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "InitCOMServer failed!!!\n");
							}
						}
					}
					
					if (VOD_OK != wRet)
					{
						// guzh [9/8/2006] 带错误码回Ack
						wAckEvent = REC_MCU_STARTREC_ACK;
						wErrCode  = ERR_REC_PUBLICFAILED;
					}
				}
			}
		}
		break;
	case TRecChnnlStatus::STATE_RECORDING:
		wErrCode  = ERR_REC_CURDOING;
	    wAckEvent = REC_MCU_STARTREC_NACK;
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "Refuse Mcu Start Recording Req,since Recording.\n");
		break;
	case TRecChnnlStatus::STATE_RECPAUSE:
        wRet = ::RPResumeRec( m_dwDevice );
	    if( wRet != RP_OK )
		{
			wAckEvent = REC_MCU_STARTREC_NACK;
			wErrCode  = ERR_REC_ACTFAIL;
 
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "Device.%d Resume Record failed.", m_dwDevice);
			
		}
		break;
	default:
		log(LOGLVL_EXCEPTION, "[Rec] Inst.%d at a undefine state .\n", GetInsID());
		break;
	}

	memcpy(m_ptMediaEncrypt, pMediaEncrypt, sizeof(m_ptMediaEncrypt));
	memcpy(m_ptDoublePayload, pDoublePayload, sizeof(m_ptDoublePayload));

	// 回送应答 并进行状态上报
	cServMsg.SetErrorCode(wErrCode);
	cServMsg.SetChnIndex(m_byChnIdx);
	cServMsg.SetMsgBody((u8 *)&m_tSrvTmt, sizeof(m_tSrvTmt));
	cServMsg.CatMsgBody((u8 *)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp));

	SendMsgToMcu(wAckEvent, &cServMsg);
    
    // guzh [9/7/2006] ACK
	if (wAckEvent == REC_MCU_STARTREC_ACK)
	{
		s32 InstId = GetInsID();
        // 记录正在使用的文件
		strncpy(g_cRecApp.m_achUsingFile[InstId], m_achFullName, MAX_FILE_NAME_LEN); 

		m_tChnnlStatus.m_byState = TRecChnnlStatus::STATE_RECORDING;
		
		NEXTSTATE(TRecChnnlStatus::STATE_RECORDING);

		m_tChnnlStatus.m_tSrc = m_tSrvTmt;

        SendChnNotify();    // 通道状态上报

		SendRecProgNotify(); // 录像进度通知
		
		SetTimer(EV_REC_MCU_NEEDIFRAME_TIMER, 1000, 3);

		// SetTimer( EV_REC_RECPROGTIME, RECPROG_PERIOD); // 开始录像,启动录像通知定时器
	}

    return;
}

/*====================================================================
    函数名	     ：MsgPauseRecordProc
	功能		 ：暂停录像消息处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgPauseRecordProc(CMessage * const pMsg)
{
	u16 wErrCode,wAckEvent;
	u16 wRet;
	CServMsg cServMsg(pMsg->content, pMsg->length);

#ifdef _RECSERV_
	//  [8/24/2010 liuxu]只有mcu连接上时才走以下逻辑
	if (!IsMtConnect())
	{
		if( !(m_cConfId == cServMsg.GetConfId()) )
		{
			//会议号不正确
			cServMsg.SetMsgBody( (u8*)&m_tSrvTmt, sizeof(m_tSrvTmt) );
			cServMsg.CatMsgBody( (u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp) );
			SendMsgToMcu( pMsg->event +2,&cServMsg );
			
			Reclog("[Rec] Refuse Pause Rec since different ConfID:\n");
			
			s8 achConfId[80];
			m_cConfId.GetConfIdString( achConfId, sizeof(achConfId) );
			Reclog("Current:\t%s\n",achConfId);
			
			cServMsg.GetConfId().GetConfIdString(achConfId, sizeof(achConfId) );
			Reclog("Recv %s\n",achConfId);
			
			return;
		}
	}
#endif

	wErrCode   = 0;
	wAckEvent  = REC_MCU_PAUSEREC_ACK;

	switch( CurState() )
	{
	case TRecChnnlStatus::STATE_RECORDING:
		wRet = RPPauseRec( m_dwDevice );
        if( wRet != RP_OK )
		{
			wErrCode  = ERR_REC_ACTFAIL;
			wAckEvent = REC_MCU_PAUSEREC_NACK;
			
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "Call Device.%d rpPauseRecord failed.\n", m_dwDevice);
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "Device.%d call \"rpPauseRecord\" success.\n", m_dwDevice);
			
		}
		break;
	case TRecChnnlStatus::STATE_RECPAUSE:
		wErrCode  = ERR_REC_CURDOING;
	    wAckEvent = REC_MCU_PAUSEREC_NACK;
		break;
	case TRecChnnlStatus::STATE_RECREADY:
		wErrCode  = ERR_REC_NOTRECORD;
	    wAckEvent = REC_MCU_PAUSEREC_NACK;
		break;
	default:
		Reclog("[Rec] (MsgPauseRecordProc)Inst.%d at a undefine state(%d) .\n",GetInsID(),CurState());
		return;
	}

	cServMsg.SetErrorCode( wErrCode );
	cServMsg.SetMsgBody( (u8*)&m_tSrvTmt, sizeof(m_tSrvTmt) );
    cServMsg.CatMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
	SendMsgToMcu( wAckEvent, &cServMsg );

	if( wAckEvent == REC_MCU_PAUSEREC_ACK )
	{
		KillTimer(EV_REC_MCU_NEEDIFRAME_TIMER);
		KillTimer( EV_REC_RECPROGTIME );
		TRecorderStatus tRecorderStaus;
        TRecorderStatis tRecorderStatis;

        wRet = RPGetRecorderStatis(m_dwDevice, &tRecorderStatis );
        if( RP_OK == wRet)
        {
            if( RP_OK != RPGetRecorderStatus( m_dwDevice, &tRecorderStaus))
		    {
				LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_REC, "call \"rpGetRecorderStatus\" failed.\n");
			    
		    }
		    else
		    {
			    m_tChnnlStatus.m_tProg.SetCurProg(tRecorderStatis.m_dwRecTime);
		    }
        }
        else
        {
			LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_REC, "Fail to call the RPGetRecorderStatis() \n" );
        }
		m_tChnnlStatus.m_byState = TRecChnnlStatus::STATE_RECPAUSE;
		NEXTSTATE(TRecChnnlStatus::STATE_RECPAUSE);
		SendChnNotify();
	}

    return;
}

/*====================================================================
    函数名	     ：MsgResumeRecordProc
	功能		 ：恢复录像消息处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgResumeRecordProc(CMessage * const pMsg)
{
	u16 wErrCode,wAckEvent,wRet;
	
	CServMsg cServMsg(pMsg->content, pMsg->length);

#ifdef _RECSERV_
	//  [8/24/2010 liuxu]只有mcu连接上时才走以下逻辑
	if (!IsMtConnect())
	{
		if( !(m_cConfId == cServMsg.GetConfId()) )
		{
			//会议号不正确
			cServMsg.SetMsgBody( (u8*)&m_tSrvTmt, sizeof(m_tSrvTmt) );
			cServMsg.CatMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
			SendMsgToMcu( pMsg->event +2,&cServMsg );
			log( LOGLVL_IMPORTANT,"[Rec] refuse Resume Rec,since diff confID:\n ");
			
			s8 achConfId[80];
			m_cConfId.GetConfIdString( achConfId,sizeof(achConfId) );
			log( LOGLVL_IMPORTANT,"\t%s\n",achConfId);
			
			cServMsg.GetConfId().GetConfIdString(achConfId,sizeof(achConfId) );
			log( LOGLVL_IMPORTANT,"req %s\n",achConfId);
			return;
		}
	}
#endif

	wErrCode  = 0;
	wAckEvent = REC_MCU_RESUMEREC_ACK;

	switch( CurState() )
	{
	case TRecChnnlStatus::STATE_RECPAUSE:
        wRet = RPResumeRec( m_dwDevice );
        if(wRet != RP_OK )
        {
			wErrCode  = ERR_REC_ACTFAIL;
			wAckEvent = REC_MCU_RESUMEREC_NACK;
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "call Device.%d \"rpResumeRecord\" failed \n", m_dwDevice);
        }
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, " Device.%d call \"rpResumeRecord\" success.\n", m_dwDevice);
			
		}
		break;
	case TRecChnnlStatus::STATE_RECORDING:
		wErrCode  = ERR_REC_CURDOING;//当前通道正在录像
        wAckEvent = REC_MCU_RESUMEREC_NACK;
        break;
	case TRecChnnlStatus::STATE_RECREADY:
		wErrCode  = ERR_REC_NOTRECORD;//当前通道没在录像
        wAckEvent = REC_MCU_RESUMEREC_NACK;
		break;
	default:
    	log( LOGLVL_EXCEPTION,"[Rec] ( MsgPauseRecordProc )Inst.%d at a undefine state(%d) .\n",GetInsID(),CurState());
		return;
	}

	cServMsg.SetErrorCode( wErrCode );
    cServMsg.SetMsgBody( (u8*)&m_tSrvTmt, sizeof(m_tSrvTmt) );
	cServMsg.CatMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
	SendMsgToMcu( wAckEvent,&cServMsg);

	if( wAckEvent ==  REC_MCU_RESUMEREC_ACK )
	{
		
		m_tChnnlStatus.m_byState = TRecChnnlStatus::STATE_RECORDING ;
		
		NEXTSTATE( TRecChnnlStatus::STATE_RECORDING );
		
		SendChnNotify();   // 通道状态上报

		SendRecProgNotify();// 录像进度通知

		SetTimer( EV_REC_RECPROGTIME, RECPROG_PERIOD); // 恢复定时通知录像进程

        //  [7/26/2010 zhouxingxing] xxxx
        SetTimer(EV_REC_MCU_NEEDIFRAME_TIMER, 1000, 3);
	}
}

/*====================================================================
    函数名	     ：MsgStopRecordProc
	功能		 ：停止录像消息处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgStopRecordProc(CMessage * const pMsg)
{	
	u16 wErrCode,wAckEvent,wRet = RP_OK;
	CServMsg cServMsg(pMsg->content, pMsg->length);

#ifdef _RECSERV_
	//  [8/24/2010 liuxu]只有mcu连接上时才走以下逻辑
	if (!IsMtConnect())
	{
		if( !(m_cConfId == cServMsg.GetConfId()) )
		{
			//会议号不正确
			cServMsg.SetMsgBody( (u8*)&m_tSrvTmt, sizeof(m_tSrvTmt) );
			cServMsg.CatMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
			SendMsgToMcu( pMsg->event +2,&cServMsg );
			Reclog("Refuse Stop Rec req,since dif confID:\n");
			
			s8 achConfId[80];
			m_cConfId.GetConfIdString( achConfId,sizeof(achConfId) );
			Reclog("\t%s  ",achConfId);
			
			memset(achConfId, '\0', sizeof(achConfId));
			cServMsg.GetConfId().GetConfIdString(achConfId,sizeof(achConfId) );
			Reclog("Req %s\n",achConfId);
			return;
		}
	}
#endif

	wErrCode  = 0 ;
	wAckEvent = REC_MCU_STOPREC_ACK;
    
	LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "Stop Record eqp m_dwDevice: %d\n", m_dwDevice);
	switch( CurState() )
	{
	case TRecChnnlStatus::STATE_RECORDING:
	case TRecChnnlStatus::STATE_RECPAUSE:
        wRet = RPStopRec( m_dwDevice, 0 );
		if( RP_OK != wRet)
		{
			wErrCode  = ERR_REC_ACTFAIL ;
			wAckEvent = REC_MCU_STOPREC_NACK;
			
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "[MsgStopRecordProc]call device%d \"rpStopRecord\" failed.\n", m_dwDevice );
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "[MsgStopRecordProc]call device%d \"rpStopRecord\" success.\n", m_dwDevice );
			
		}
		break;
	case TRecChnnlStatus::STATE_RECREADY:
		wErrCode  = ERR_REC_CURDOING;       // 当前通道正在录像
	    wAckEvent = REC_MCU_STOPREC_NACK;
		break;
	case TRecChnnlStatus::STATE_IDLE:
		log( LOGLVL_EXCEPTION,"[Rec] Channel %d receiv msg at IDLE.\n",GetInsID());
		return;

	default:
		log( LOGLVL_EXCEPTION,"[Rec] (MsgStopRecordProc) Inst.%d at undefine state .\n",GetInsID() );
		return;

	}
	
	// 立即发布
	if (PUBLISH_MODE_IMMED == m_byPublishMode)
	{
		DeleteBrdPubPoint();
		DeleteOneLiveSource();
		DeletePlayList();
	}

    // 录完发布和立即发布
	if(PUBLISH_MODE_FIN == m_byPublishMode || PUBLISH_MODE_IMMED == m_byPublishMode)
	{
		if (!g_tCfg.m_bOpenDBSucceed)
		{
			// [7/12/2011 liuxu]
			wAckEvent = REC_MCU_STOPREC_ACK;
			wErrCode  = ERR_REC_PUBLICFAILED;
			
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "DB is not opened, publish file failed!(%d)");
		}
		else
		{
			wRet = PublishOneFile(m_achFullName);

			if( VOD_OK != wRet )
			{
				wAckEvent = REC_MCU_STOPREC_ACK;
				wErrCode  = ERR_REC_PUBLICFAILED;
			}
		}
	}

	m_byPublishMode  = PUBLISH_MODE_NONE;    // 清空发布模式

	cServMsg.SetErrorCode(wErrCode);
	cServMsg.SetMsgBody( (u8*)&m_tSrvTmt, sizeof(m_tSrvTmt) );
	cServMsg.CatMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
	SendMsgToMcu( wAckEvent,&cServMsg );

	if( wAckEvent == REC_MCU_STOPREC_ACK )
	{
		KillTimer(EV_REC_MCU_NEEDIFRAME_TIMER);
		KillTimer( EV_REC_RECPROGTIME );  // 关闭通知录像进程定时器
		m_tChnnlStatus.m_tSrc.SetNull();
        m_tChnnlStatus.SetRecordName( "" );
		m_tChnnlStatus.m_tProg.SetTotalTime( 0 );
		m_tChnnlStatus.m_tProg.SetCurProg( 0 );
		m_tChnnlStatus.m_byState = TRecChnnlStatus::STATE_RECREADY ;
		NEXTSTATE( TRecChnnlStatus::STATE_RECREADY );
		SendChnNotify();

		//停止录像，ADDVIDFile通知
		SendAddVidFileNotify();

		m_cConfId.SetNull();

		// 释放使用文件
		s32 InstId = GetInsID();
        Reclog("The recorder cancel using file: %s\n", g_cRecApp.m_achUsingFile[InstId]);
		memset( g_cRecApp.m_achUsingFile[InstId], 0, MAX_FILE_PATH_LEN );
	}
    return;
}

/*====================================================================
函数名	     ：MsgAddVidFileProc
功能		 ：停止录像或大文件切换时，文件属性通知到mcu
引用全局变量 ：无
输入参数说明 ：无
返回值说明   ：无
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
13/08/27		        党剑锋        创建
====================================================================*/
void CChnInst::SendAddVidFileNotify(void)
{
	u16 wRet = RP_OK;
	s8 achNullString[MAX_TCNAME_LEN+1];
	CServMsg cServMsg;
	
	//设置会议号
	cServMsg.SetConfId(m_cConfId);

	memset(achNullString, 0, MAX_TCNAME_LEN+1);
	cServMsg.SetMsgBody((u8*)achNullString, MAX_TCNAME_LEN+1);
	cServMsg.CatMsgBody((u8*)achNullString, MAX_TCNAME_LEN+1);

	//视频设备ID
	sprintf(achNullString, "%d" ,g_tCfg.m_byEqpId);
	cServMsg.CatMsgBody((u8*)achNullString, MAX_TCNAME_LEN+1);
	
	//视频设备名称
	cServMsg.CatMsgBody((u8*)g_tCfg.m_szAlias, MAX_TCNAME_LEN+1);
	
	memset(achNullString, 0, MAX_TCNAME_LEN+1);
	cServMsg.CatMsgBody((u8*)achNullString, MAX_TCNAME_LEN+1);
	cServMsg.CatMsgBody((u8*)achNullString, MAX_TCNAME_LEN+1);

	//录像文件名称
	cServMsg.CatMsgBody((u8*)m_achFileName, MAX_TCNAME_LEN+1);
	
	//录像文件地址：mms://125.64.11.105/KEDA_MCU/评审室1_768k_2.asf
	s8 achFileAddr[MAX_PATH+1];
	memset(achFileAddr, 0, MAX_PATH+1);
	strncpy(achFileAddr, "mms://", strlen("mms://"));
	strncat(achFileAddr, g_tCfg.m_szMachineName, 32);
	strncat(achFileAddr, "/", strlen("/"));
	s8 *pHeadTemp = m_achFullName;
	while( NULL != strstr(pHeadTemp, "\\") )
	{
		pHeadTemp = strstr(pHeadTemp, "\\");
		pHeadTemp++;
	}
	strncat(achFileAddr, pHeadTemp, MAX_PATH + 1 - strlen("mms://") - 32);
	cServMsg.CatMsgBody((u8*)achFileAddr, MAX_PATH+1);
	
	TFileInfo tFileInfo;
	RPGetFileInfo(MAXNUM_RECORDER_CHNNL ,m_achFullName , &tFileInfo);
	
	s8 achFullName[MAX_FILE_NAME_LEN];
	memset(achFullName, 0, MAX_FILE_NAME_LEN);
	memcpy(achFullName, m_achFullName, strlen(m_achFullName)-strlen(m_achFileName)-1);
	achFullName[strlen(achFullName)] = '\\';
	memcpy(&achFullName[strlen(achFullName)], m_achFileName, strlen(m_achFileName));

	//录制的时间点(秒)，其实就是文件的创建时间
	u64 qwCreateDate = 0;
	wRet = RPGetFileCreateTime(MAXNUM_RECORDER_CHNNL, &qwCreateDate, achFullName);
	if(RP_OK != wRet)
	{
		Reclog("Fail to call RPGetFileCreateTime, error: %d\n", wRet);
	}

	time_t cTime;
	FILETIME cFileTime;
	memcpy(&cFileTime, &qwCreateDate, sizeof(u64));
	g_cRecApp.FileTimeToTime_t(cFileTime, &cTime);
	cServMsg.CatMsgBody((u8*)&cTime, sizeof(s32));
	
	//录像文件的时长
	cServMsg.CatMsgBody((u8*)&tFileInfo.m_dwDuration, sizeof(u32));

	//录像文件大小
	cServMsg.CatMsgBody((u8*)&tFileInfo.m_dwSize, sizeof(u32));

	SendMsgToMcu(REC_MCU_ADDVIDFILE_NTF, &cServMsg);

	return;
}


/*====================================================================
    函数名	     ：MsgStartPlayProc
	功能		 ：开始播放消息处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    04/12/28    3.6         libo          add encrypt
====================================================================*/
void CChnInst::MsgStartPlayProc(CMessage * const pMsg)
{    
    u16 wErrCode, wAckEvent;
	s8 * lpMsgBody = NULL;

	CConfId cConfId;

    TMediaEncrypt tVideoEncrypt;
    TDoublePayload tVideoDoublePayload;

    TMediaEncrypt tAudioEncrypt;
    TDoublePayload tAudioDoublePayload;
    TDoublePayload tSndVideoPayload, tSndAudioPayload;

	TRecProg tRecProg;

	u8 byVideoNum = 0;
	u8 byIsNeedPrs = 0;
	u8 byIsDstream = 0;
	//[20121119 guodawei] 会议声道数
	u8 byConfAudChnlNum = 0;

	s8 achFileName[MAX_FILE_NAME_LEN];  // 当前录放像文件名，格式为 abc_255_384K.asf 
	memset( achFileName, 0, sizeof(achFileName) );
	s8 achFullName[MAX_FILE_NAME_LEN];  // 文件全文件名，格式为 E:\vod\mcuname/abc_255_384K.asf 
	memset( achFullName, 0, sizeof(achFullName) );

	//u8 byDStreamMediaType;
	u16 wMsgLen = 0;
	CServMsg cServMsg( pMsg->content,pMsg->length);
	cConfId = cServMsg.GetConfId();

	lpMsgBody = (s8 *)cServMsg.GetMsgBody();
    lpMsgBody += sizeof(TEqp);
	wMsgLen += sizeof(TEqp); 

    // 视频
    tVideoEncrypt = *(TMediaEncrypt *)lpMsgBody;
    lpMsgBody += sizeof(TMediaEncrypt);
	wMsgLen += sizeof(TMediaEncrypt);

    // 视频载荷
    tVideoDoublePayload = *(TDoublePayload *)lpMsgBody;
    lpMsgBody += sizeof(TDoublePayload);
	wMsgLen += sizeof(TDoublePayload);

    // 音频
    tAudioEncrypt = *(TMediaEncrypt *)lpMsgBody;
    lpMsgBody += sizeof(TMediaEncrypt);
	wMsgLen += sizeof(TMediaEncrypt);

    // 音频载荷
    tAudioDoublePayload = *(TDoublePayload *)lpMsgBody;
    lpMsgBody += sizeof(TDoublePayload);
	wMsgLen += sizeof(TDoublePayload);

    //辅助视频格式
    tSndVideoPayload = *(TDoublePayload *)lpMsgBody;
    lpMsgBody += sizeof(TDoublePayload);
	wMsgLen += sizeof(TDoublePayload);

    //辅助音频格式
    tSndAudioPayload = *(TDoublePayload *)lpMsgBody;
    lpMsgBody += sizeof(TDoublePayload);
	wMsgLen += sizeof(TDoublePayload);

	//会议双流格式
	//byDStreamMediaType = *(u8*)lpMsgBody;
	lpMsgBody += sizeof(u8);
	wMsgLen += sizeof(u8);

    // MCU前向纠错
    TCapSupportEx tCapSupportEx = *(TCapSupportEx*)lpMsgBody;
    lpMsgBody += sizeof(TCapSupportEx);
	wMsgLen += sizeof(TCapSupportEx);
    if( g_bPrintRecLog )
    {
        tCapSupportEx.Print();
    }
    
	//文件信息总总长
	u16 wFileInfoLen = *(u16*)lpMsgBody;
	wFileInfoLen = ntohs(wFileInfoLen);

	lpMsgBody += sizeof(u16);
	wMsgLen += sizeof(u16);

    //放像开始进度
    tRecProg = *(TRecProg*)lpMsgBody;
    lpMsgBody += sizeof(TRecProg);
    
    //放像名,格式： abc_255_384K.asf    
	sprintf( achFileName, "%s", lpMsgBody);   //  m_achFileName 格式 abc_255_384K.asf    
    u16 wFullNameSize;    // add by jlb 081026
	wFullNameSize = g_cRecApp.PreProcFileame(achFullName, achFileName, NULL);  // 格式  e:\vod\mcuname/abc_255_384K.asf
    if (0 == wFullNameSize)
    {
		cServMsg.SetChnIndex( m_byChnIdx );
		cServMsg.SetErrorCode( ERR_REC_FILENAMETOOLONG );
		cServMsg.SetMsgBody((u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp));
		SendMsgToMcu( REC_MCU_STARTPLAY_NACK, &cServMsg);
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "Failed to play record  %s! because file name too long.\n", achFileName);
		return;
    }
    
	LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "Play Filename = %s\n", achFullName);
	
	lpMsgBody += wFileInfoLen - sizeof(TRecProg);
	wMsgLen += wFileInfoLen;

	//是否有能力信息并获取
	m_bNeedCapChk = FALSE;
	TCapSupport tCap;
	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		tCap = *(TCapSupport*)lpMsgBody;
		wMsgLen += sizeof(TCapSupport);
		lpMsgBody += sizeof(TCapSupport);
		m_tConfCapSupport = tCap;
		m_bNeedCapChk = TRUE;
	}

	wErrCode  =  0;
	wAckEvent =  REC_MCU_STARTPLAY_ACK;

    Reclog("conf main video :%d, main audio :%d, snd video :%d, snd audio %d\n",
            tVideoDoublePayload.GetRealPayLoad(), tAudioDoublePayload.GetRealPayLoad(),
            tSndVideoPayload.GetRealPayLoad(), tSndAudioPayload.GetRealPayLoad());

	//PRS支持以及是否播放双流
	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		byIsNeedPrs = *(u8*)lpMsgBody;
		wMsgLen += sizeof(u8);
		lpMsgBody += sizeof(u8);
		
		byIsDstream = *(u8*)lpMsgBody;
		wMsgLen += sizeof(u8);
		lpMsgBody += sizeof(u8);
	}
	
	//[20121119 guodawei] 获取会议声道数
	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		byConfAudChnlNum = *(u8*)lpMsgBody;
		wMsgLen += sizeof(u8);
		lpMsgBody += sizeof(u8);
	}
	
	//[2013/5/10 zhangli]多音频会议支持的音频
	memset(m_patAudDesc, 0, sizeof(m_patAudDesc));
	m_byAudNumber = 0;
	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		m_byAudNumber = *(u8*)lpMsgBody;
		wMsgLen += sizeof(u8);
		lpMsgBody += sizeof(u8);
		
		m_byAudNumber = min(m_byAudNumber, MAXNUM_CONF_AUDIOTYPE);
		
		TAudioTypeDesc tAudioType;
		for (u8 byLoop = 0; byLoop < m_byAudNumber; ++byLoop)
		{
			tAudioType = *(TAudioTypeDesc *)lpMsgBody;
			m_patAudDesc[byLoop]  = tAudioType;
			
			Reclog("conf Audio index:%d, mediatype:%d, Channels:%d\n", byLoop, 
				tAudioType.GetAudioMediaType(), tAudioType.GetAudioTrackNum());
			
			wMsgLen += sizeof(TAudioTypeDesc);
			lpMsgBody += sizeof(TAudioTypeDesc);
		}
	}
	
	Reclog("conf Audio number:%d\n", m_byAudNumber);

	if(byIsNeedPrs != 0)
	{ 
		m_byIsNeedPrs = byIsNeedPrs;
		
		if(byIsDstream)
		{
			m_byIsDstream = byIsDstream;
		}
	}
	
	// 保存放象信息
	m_cConfId = cConfId;
	m_byConfPlaySrcSsnId = cServMsg.GetSrcSsnId();
	
	memset( m_achFileName, 0x0, sizeof(m_achFileName) );
	strncpy( m_achFileName, achFileName, strlen(achFileName) );
	m_achFileName[MAX_FILE_NAME_LEN-1] = '\0';
    memset( m_achFullName, 0x0, sizeof(m_achFullName));
	strncpy( m_achFullName, achFullName, strlen(achFullName) );
	m_achFullName[MAX_FILE_NAME_LEN-1] = '\0';
    memset( m_achOrignalFullName, 0x0, sizeof(m_achOrignalFullName));
    strncpy( m_achOrignalFullName, achFullName, strlen(achFullName) );  //add by jlb 081027
    m_achOrignalFullName[MAX_FILE_NAME_LEN-1] = '\0';      //add by jlb 08127

	m_tVideoEncrypt = tVideoEncrypt;
	m_tAudioEncrypt = tAudioEncrypt;
	m_tChnnlStatus.m_tProg = tRecProg;
    m_tCapSupportEx = tCapSupportEx;
	m_byAudioTrackNum = byConfAudChnlNum;
	m_byAudioMediaType = tAudioDoublePayload.GetRealPayLoad();
	m_byConfVideoMediaType = tVideoDoublePayload.GetRealPayLoad();
	m_byConfSecVidMediaType = tSndVideoPayload.GetRealPayLoad();

	wErrCode = CheckFileCanPlay(achFullName);
	if (wErrCode != 0)
	{
		cServMsg.SetChnIndex(m_byChnIdx);
		cServMsg.SetErrorCode(wErrCode);
		cServMsg.SetMsgBody((u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp));
		SendMsgToMcu(REC_MCU_STARTPLAY_NACK, &cServMsg);
		return;
	}

	byVideoNum = 0;
    if( MEDIA_TYPE_H261 <= m_tMediaInfo.m_abyMediaType[1] && 
        MEDIA_TYPE_H264 >= m_tMediaInfo.m_abyMediaType[1] )
    {
        byVideoNum++;
    }
	
    if( MEDIA_TYPE_H261 <= m_tMediaInfo.m_abyMediaType[2] && 
        MEDIA_TYPE_H264 >= m_tMediaInfo.m_abyMediaType[2] )
    {
        byVideoNum++;
    }

	m_byVideoNum = byVideoNum;
	
    Reclog("The video channle num in file: %d\n", byVideoNum);

	switch(CurState())
	{
	case TRecChnnlStatus::STATE_PLAYING:
	case TRecChnnlStatus::STATE_PLAYREADYPLAY:
		wErrCode = ERR_REC_CURDOING;//当前通道正在放像
		wAckEvent = REC_MCU_STARTPLAY_NACK;
		cServMsg.SetChnIndex( m_byChnIdx );
        cServMsg.SetErrorCode( wErrCode );
        cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp) );
        SendMsgToMcu( wAckEvent, &cServMsg );
        return;

	case TRecChnnlStatus::STATE_PLAYPAUSE:
	case TRecChnnlStatus::STATE_FB:
	case TRecChnnlStatus::STATE_FF:
	case TRecChnnlStatus::STATE_PLAYREADY:  
		break;
	default:
		log( LOGLVL_EXCEPTION,"[Rec] (MsgStartPlayProc) Inst.%d at undefine state .\n",GetInsID() );
		return ;
	}

	TPlayFileAttrib tPlayFileAttrib;
    
    if(VIDEO_TWO == byVideoNum 
		|| MEDIA_TYPE_NULL != m_tMediaInfo.m_abyMediaType[2])
    {
        tPlayFileAttrib.SetFileType(RECFILE_DSTREAM);
        tPlayFileAttrib.SetDVideoType(m_tMediaInfo.m_abyMediaType[2]);
        Reclog("Set double stream :%d\n", m_tMediaInfo.m_abyMediaType[2]);
    }
	
    tPlayFileAttrib.SetAudioType(m_tMediaInfo.m_abyMediaType[0]);
    tPlayFileAttrib.SetVideoType(m_tMediaInfo.m_abyMediaType[1]); 

	// 修改状态，回ACK
	s32 InstId = GetInsID();
	strcpy(g_cRecApp.m_achUsingFile[InstId], m_achFullName);
    m_tChnnlStatus.SetRecordName(m_achFileName);
	m_tChnnlStatus.m_byState = TRecChnnlStatus::STATE_PLAYREADYPLAY;

	NEXTSTATE( TRecChnnlStatus::STATE_PLAYREADYPLAY );
	cServMsg.SetChnIndex( m_byChnIdx );
	cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp) );
	cServMsg.CatMsgBody( (u8*)&tPlayFileAttrib, sizeof(tPlayFileAttrib));
    
    // zgc, 2008-08-28, 增加放象文件媒体信息
    TPlayFileMediaInfo tPlayFileMediaInfo;
    MediaInfoRec2Mcu( m_tMediaInfo, tPlayFileMediaInfo );
    cServMsg.CatMsgBody( (u8*)&tPlayFileMediaInfo, sizeof(tPlayFileMediaInfo) );

	//[liu lijiu][2010/11/08]增加放像机的起始端口信息
	TTransportAddr tPlayRtcpAddr;
	tPlayRtcpAddr.SetIpAddr(ntohl(g_tCfg.m_dwRecIpAddr));
	tPlayRtcpAddr.SetPort(g_tCfg.m_wPlayStartPort);
	cServMsg.CatMsgBody( (u8*)&tPlayRtcpAddr, sizeof(TTransportAddr) );
	Reclog( "[MsgStartPlayProc] player local start port: %d\n", tPlayRtcpAddr.GetPort());
	
	//zhouyiliang 20120224 录像机放像带上HP/BP信息
	TPlayFileHpAttrib tPlayFileHpAtAtrib;
	if (m_bFileFirstVidHp)
	{
		tPlayFileHpAtAtrib.SetFirstVideHpAttrib(emHpAttrb);
	}//无需else，TPlayFileHpAttrib构造函数默认就是bp

	if (m_bFileSecVidHp)
	{
		tPlayFileHpAtAtrib.SetSecVideHpAttrib(emHpAttrb);
	}//无需else，TPlayFileHpAttrib构造函数默认就是bp

	cServMsg.CatMsgBody((u8*)&tPlayFileHpAtAtrib,sizeof(TPlayFileHpAttrib));    
	// [2013/3/21 zhangli]音频声道数
	cServMsg.CatMsgBody(&m_byFileAudioTrackNum, sizeof(u8));

	SendMsgToMcu( wAckEvent, &cServMsg);
	SendChnNotify();

	// 设置定时器，防止交换一直未建立
	SetTimer(EV_REC_SWITCHSTART_WAITTIMER, 5*1000);

    return;
}

/*=============================================================================
  函 数 名： CheckFileCanPlay
  功    能： 根据会议属性判断是否够播放
  算法实现： 
  全局变量： 
  参    数： s8 *pFileFullName 文件全路径
  返 回 值： 无
  ----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2012/05/10  4.7         zhangli        create
=============================================================================*/
u16 CChnInst::CheckFileCanPlay(s8 *pFileFullName)
{
	u16 wErroeCode = 0;
	//判断指定的文件是否已使用
	for(s32 nLop = 1; nLop <= MAXNUM_RECORDER_CHNNL; nLop++)
	{
		s32 nLen = strlen(pFileFullName);

		if((nLen == 0) || (0 ==_strnicmp(g_cRecApp.m_achUsingFile[nLop], pFileFullName, nLen)))//该文件正在被操作
		{
			if(nLen > 0)
            {
				wErroeCode = ERR_REC_FILEUSING;
            }
			else
            {
				wErroeCode = ERR_REC_FILENOTEXIST;
            }
			return wErroeCode;
		}
	}
    
	TMediaInfo tMediaInfo;
    memset(&tMediaInfo, 0, sizeof(tMediaInfo));
    u16 wRet = RPGetFileMediaInfo(m_dwDevice, pFileFullName, &tMediaInfo);
    if(RP_OK != wRet)
    {
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_PLAY, "Fail to get file media info, error code: %d\n", wRet);
        wErroeCode = ERR_REC_FILEDAMAGE;
        return wErroeCode;
    }

	m_tMediaInfo = tMediaInfo;

	Reclog("file media info: main video: %d, main audio: %d, sec video: %d\n", 
		m_tMediaInfo.m_abyMediaType[1], m_tMediaInfo.m_abyMediaType[0], m_tMediaInfo.m_abyMediaType[2]);

    Reclog("video<W=%d,H=%d>, dual<W=%d,H=%d>\n",
		m_tMediaInfo.m_awWidth[1], m_tMediaInfo.m_awHeight[1], m_tMediaInfo.m_awWidth[2], m_tMediaInfo.m_awHeight[2]);

    //判断视频文件载荷类型与会议是否匹配   
	if ((m_byConfVideoMediaType != tMediaInfo.m_abyMediaType[1]	//video
		&& m_byConfSecVidMediaType != tMediaInfo.m_abyMediaType[1]
		&& MEDIA_TYPE_NULL != tMediaInfo.m_abyMediaType[1])
		|| (MEDIA_TYPE_NULL != tMediaInfo.m_abyMediaType[2]					//second video 
		&& MEDIA_TYPE_H264 != tMediaInfo.m_abyMediaType[2] 
		&& MEDIA_TYPE_H263PLUS != tMediaInfo.m_abyMediaType[2]))
	{
		Reclog("File video payload type is not matched with conf. conf video :%d, sec video\n", 
			m_byConfVideoMediaType, m_byConfSecVidMediaType);
		 wErroeCode = ERR_REC_PLAYERRPAYLOAD;
		return wErroeCode;
	}

	//判断文件分辨率大小和HP与会议是否匹配.(目前只针对H264)
	if (m_bNeedCapChk)
	{
		//1、check res
		BOOL32 bVidMatch = TRUE;
		BOOL32 bDSMatch = TRUE;
		
		if (MEDIA_TYPE_H264 == tMediaInfo.m_abyMediaType[1])
		{
			u8 byConfVidRes = m_tConfCapSupport.GetMainVideoResolution();
			u16 wConfVidW = 0;
			u16 wConfVidH = 0; 
			g_cRecApp.GetWHByRes(byConfVidRes, wConfVidW, wConfVidH);
			if (tMediaInfo.m_awWidth[1] > wConfVidW 
				|| tMediaInfo.m_awHeight[1] > wConfVidH)
			{
				bVidMatch = FALSE;
				Reclog("File vid res<w.%d, h.%d> is greater than conf res<w.%d, h.%d>\n", 
					tMediaInfo.m_awWidth[1], tMediaInfo.m_awHeight[1], wConfVidW, wConfVidH);
			}
		}

		if (MEDIA_TYPE_H264 == tMediaInfo.m_abyMediaType[2])
		{
			u8 byConfDSRes = m_tConfCapSupport.GetDStreamResolution();
			u16 wConfDSW = 0;
			u16 wConfDSH = 0; 
			g_cRecApp.GetWHByRes(byConfDSRes, wConfDSW, wConfDSH);

			if (tMediaInfo.m_awWidth[2] * tMediaInfo.m_awHeight[2] > wConfDSW *wConfDSH )
			{
				bDSMatch = FALSE;
				Reclog("File DS res<w.%d, h.%d> is greater than conf res<w.%d, h.%d>\n", 
					tMediaInfo.m_awWidth[2], tMediaInfo.m_awHeight[2], wConfDSW, wConfDSH);
			}
		}
		
		if (!bVidMatch || !bDSMatch)
		{
			wErroeCode = ERR_REC_PALYFILERES;
			return wErroeCode;
		}
		
		//2、check hp/bp
		m_bFileFirstVidHp = FALSE;
		m_bFileSecVidHp = FALSE;
		RPGetFileHpAttrib(m_dwDevice, m_achFullName, m_bFileFirstVidHp, m_bFileSecVidHp);
		Reclog("firstVideois :%s,second is:%s\n", m_bFileFirstVidHp ? "HP" : "BP", m_bFileSecVidHp ? "HP" : "BP");

		BOOL32 bVidHpMatch = TRUE;
		BOOL32 bDSHpMatch = TRUE;
		
		if (MEDIA_TYPE_H264 == tMediaInfo.m_abyMediaType[1])
		{	
			emProfileAttrb eMainPa = m_tConfCapSupport.GetMainStreamProfileAttrb();
			if (m_bFileFirstVidHp && emHpAttrb != eMainPa)
			{
				bVidHpMatch = FALSE;
				Reclog("play File\'s first video stream is hp,but conf is bp\n ");
			}
		}

		if (MEDIA_TYPE_H264 == tMediaInfo.m_abyMediaType[2])
		{
			emProfileAttrb eDsPa = m_tConfCapSupport.GetDStreamCapSet().GetH264ProfileAttrb();
			if (m_bFileSecVidHp && emHpAttrb != eDsPa)
			{
				bDSHpMatch = FALSE;
				Reclog("play File\'s second video stream is hp,but conf\'s second stream is bp\n ");
			}
		}
		
		if (!bVidHpMatch || !bDSHpMatch)
		{
			wErroeCode = ERR_REC_PLAYFILE_HPDISMATCH;
			return wErroeCode;
		}
	}
    
	// 判断音频是否与会议匹配
	u8 abyFileAudChnlNum[MAX_AUDIOSTREAM_NUM] = {0};
	wRet = RPGetAudioChnlNum(m_dwDevice, pFileFullName, abyFileAudChnlNum);
	if(RP_OK != wRet)
	{
		wErroeCode = ERR_REC_FILEDAMAGE;
		return wErroeCode;
    }

	m_byFileAudioTrackNum = abyFileAudChnlNum[0];

	Reclog("file audio channels:%d \n", m_byFileAudioTrackNum);

	BOOL32 bIsAudioMatch = FALSE;
	if (MEDIA_TYPE_NULL == tMediaInfo.m_abyMediaType[0])
	{
		bIsAudioMatch = TRUE;
	}

	if (!bIsAudioMatch)
	{
		if (m_byAudNumber > 0)
		{
			for (u8 byLoop = 0; byLoop < m_byAudNumber; ++byLoop)
			{
				if (tMediaInfo.m_abyMediaType[0] == m_patAudDesc[byLoop].GetAudioMediaType()
					&& m_patAudDesc[byLoop].GetAudioTrackNum() == m_byFileAudioTrackNum)
				{
					bIsAudioMatch = TRUE;
					break;
				}
			}
		}
		else
		{
			bIsAudioMatch = TRUE;
			
			if (MEDIA_TYPE_NULL != m_byAudioMediaType
				&& MEDIA_TYPE_NULL != tMediaInfo.m_abyMediaType[0]
				&& m_byAudioMediaType != tMediaInfo.m_abyMediaType[0])
			{
				bIsAudioMatch = FALSE;
			}
			if (bIsAudioMatch && m_byAudioTrackNum != 0 && m_byAudioTrackNum != m_byFileAudioTrackNum)
			{
				bIsAudioMatch = FALSE;
			}
		}
	}

	if (!bIsAudioMatch)
	{
		Reclog("File audio payload(type:%d,chnl:%d) type is not matched with conf(type:%d,chnl:%d)\n", 
			tMediaInfo.m_abyMediaType[0], m_byFileAudioTrackNum, m_byAudioMediaType, m_byAudioTrackNum);

		wErroeCode = ERR_REC_PLAYERRPAYLOAD;
		return wErroeCode;
	}

	return wErroeCode;
}
/*=============================================================================
  函 数 名： MsgSwitchStartNotifProc
  功    能： MCU放像会议状态通知
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： 无
  ----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    07/02/14    4.0         周广程        创建
=============================================================================*/
void CChnInst::MsgSwitchStartNotifProc( CMessage * const pcMsg, BOOL32 bSwitchedFile )
{
	CServMsg cServMsg( pcMsg->content,pcMsg->length );

	KillTimer(EV_REC_SWITCHSTART_WAITTIMER);

	u8  abyEncKey[MAXLEN_KEY];  // 第一路视频
    memset(abyEncKey, '\0', sizeof(abyEncKey));
    u8  abyEncKey1[MAXLEN_KEY]; // 音频
    memset(abyEncKey, '\0', sizeof(abyEncKey1));
    u8  abyEncKey2[MAXLEN_KEY]; // 第二路视频
    memset(abyEncKey2, '\0', sizeof(abyEncKey2));

	// 本地载荷
    TLocalPTParam tLocalPTParam;
    memset(&tLocalPTParam, 0, sizeof(tLocalPTParam));

	// 解密参数
    TEncDecParam tEncDecParam;
    memset(&tEncDecParam, 0, sizeof(tEncDecParam) );
    if( VIDEO_TWO == m_byVideoNum ||
        // zbq [02/06/2007] 单放 第二路码流文件 支持
        MEDIA_TYPE_NULL != m_tMediaInfo.m_abyMediaType[2] )
    {
        tEncDecParam.m_byNum = 3;
    }
    else
    {
        tEncDecParam.m_byNum = 2;
    }
      
    if (m_tVideoEncrypt.GetEncryptMode() == CONF_ENCRYPTMODE_NONE)   // need encrypt
    {
        //zbq[10/18/2007] FEC支持
        u8 byPayloadType = 0;
        if ( FECTYPE_NONE != m_tCapSupportEx.GetVideoFECType() )
        {
            byPayloadType = MEDIA_TYPE_FEC;
        }
	    tLocalPTParam.m_abyLocalPt[1] = byPayloadType;  // 视频载荷值

        tEncDecParam.m_atEncDecInfo[1].m_byMode = m_tVideoEncrypt.GetEncryptMode();
        tEncDecParam.m_atEncDecInfo[1].m_pszKeyBuf = NULL;
        tEncDecParam.m_atEncDecInfo[1].m_wKeySize = 0;
    }
    else
    {
        s32 nKeyLen;
        u8  byEncryptMode;
        m_tVideoEncrypt.GetEncryptKey(abyEncKey, &nKeyLen);
        tLocalPTParam.m_abyLocalPt[1] = GetActivePayload(m_tMediaInfo.m_abyMediaType[1]);
        byEncryptMode = m_tVideoEncrypt.GetEncryptMode();
        if (byEncryptMode == CONF_ENCRYPTMODE_DES)
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else if (byEncryptMode == CONF_ENCRYPTMODE_AES)
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }
        tEncDecParam.m_atEncDecInfo[1].m_byMode = byEncryptMode;
        tEncDecParam.m_atEncDecInfo[1].m_wKeySize = (u16)nKeyLen;
        tEncDecParam.m_atEncDecInfo[1].m_pszKeyBuf = (s8*)abyEncKey;
    }
    // 存在两路视频
    if( VIDEO_TWO == m_byVideoNum ||
        // zbq [02/06/2007] 单放 第二路码流文件 支持
        MEDIA_TYPE_NULL != m_tMediaInfo.m_abyMediaType[2] ) 
    {
        if (m_tVideoEncrypt.GetEncryptMode() == CONF_ENCRYPTMODE_NONE)   // 加密参数跟第一路视频一致
        {
            tLocalPTParam.m_abyLocalPt[2] = 0;  // 视频载荷值
        
            tEncDecParam.m_atEncDecInfo[2].m_byMode = m_tVideoEncrypt.GetEncryptMode();
            tEncDecParam.m_atEncDecInfo[2].m_pszKeyBuf = NULL;
            tEncDecParam.m_atEncDecInfo[2].m_wKeySize = 0;
        }
        else
        {
            s32 nKeyLen;
            u8  byEncryptMode;
            m_tVideoEncrypt.GetEncryptKey(abyEncKey2, &nKeyLen);
            tLocalPTParam.m_abyLocalPt[2] = GetActivePayload(m_tMediaInfo.m_abyMediaType[2],
                                                             m_tVideoEncrypt.GetEncryptMode(), TRUE);
            byEncryptMode = m_tVideoEncrypt.GetEncryptMode();
            if (byEncryptMode == CONF_ENCRYPTMODE_DES)
            {
                byEncryptMode = DES_ENCRYPT_MODE;
            }
            else if (byEncryptMode == CONF_ENCRYPTMODE_AES)
            {
                byEncryptMode = AES_ENCRYPT_MODE;
            }
            tEncDecParam.m_atEncDecInfo[2].m_byMode = byEncryptMode;
            tEncDecParam.m_atEncDecInfo[2].m_wKeySize = (u16)nKeyLen;
            tEncDecParam.m_atEncDecInfo[2].m_pszKeyBuf = (s8*)abyEncKey2;
        }
    }

    if (m_tAudioEncrypt.GetEncryptMode() == CONF_ENCRYPTMODE_NONE)   // not need encrypt
    {
        tLocalPTParam.m_abyLocalPt[0] = 0;
        tEncDecParam.m_atEncDecInfo[0].m_byMode = m_tAudioEncrypt.GetEncryptMode();
        tEncDecParam.m_atEncDecInfo[0].m_pszKeyBuf = NULL;
        tEncDecParam.m_atEncDecInfo[0].m_wKeySize = 0;
        
    }
    else
    {
        s32 nKeyLen;
        u8  byEncryptMode;
        m_tAudioEncrypt.GetEncryptKey(abyEncKey1, &nKeyLen);
        tLocalPTParam.m_abyLocalPt[0] = GetActivePayload(m_tMediaInfo.m_abyMediaType[0],
                                                         m_tAudioEncrypt.GetEncryptMode());
        byEncryptMode = m_tVideoEncrypt.GetEncryptMode();
        if (byEncryptMode == CONF_ENCRYPTMODE_DES)
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else if (byEncryptMode == CONF_ENCRYPTMODE_AES)
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }
        tEncDecParam.m_atEncDecInfo[0].m_byMode = byEncryptMode;
        tEncDecParam.m_atEncDecInfo[0].m_wKeySize = (u16)nKeyLen;
        tEncDecParam.m_atEncDecInfo[0].m_pszKeyBuf = (s8*)abyEncKey1;
    }
		
	TPlayParam tPlayParam;
    memset( &tPlayParam, 0, sizeof(TPlayParam) );

	if( VIDEO_TWO == m_byVideoNum ||
        // zbq [02/07/2007] 单放 第二路码流文件 支持
        MEDIA_TYPE_NULL != m_tMediaInfo.m_abyMediaType[2] )
    {
		tPlayParam.m_tNetSndParam.m_byNum = 3;
	}
	else
	{
		tPlayParam.m_tNetSndParam.m_byNum = 2;
	}

	if (!bSwitchedFile)
	{
        //zbq[05/06/2009] 初始播放和连续播放 均从0 开始
		tPlayParam.m_dwStartTime = 0; //m_tChnnlStatus.m_tProg.GetCurProg();
	}
	else
	{
		// jlb 08/10/28 指定开始时间，连续播放时每次都从1开始
		tPlayParam.m_dwStartTime = 1;  //m_tChnnlStatus.m_tProg.GetCurProg();
	}
    tPlayParam.m_dwContext   = m_dwDevice;
    tPlayParam.m_pCallBack   = ReadFileProcessCallBack;
    tPlayParam.m_byRateGran  = PLAY_RATE_GRAN;

    // 本地音频
    tPlayParam.m_tNetSndParam.m_atLocalAddr[0].m_dwIp = g_tCfg.m_dwRecIpAddr; // 本地Ip地址
    tPlayParam.m_tNetSndParam.m_atLocalAddr[0].m_wPort = m_wLocalPort + 2;
    Reclog( "[StartPlay] m_atLocalAddr[0].m_dwIp.0x%x, m_atLocalAddr[0].m_wPort.%d\n", 
             g_tCfg.m_dwRecIpAddr, m_wLocalPort + 2 );

    // 本地第一路视频
    tPlayParam.m_tNetSndParam.m_atLocalAddr[1].m_dwIp = g_tCfg.m_dwRecIpAddr;
    tPlayParam.m_tNetSndParam.m_atLocalAddr[1].m_wPort = m_wLocalPort;
    Reclog( "[StartPlay] m_atLocalAddr[1].m_dwIp.0x%x, m_atLocalAddr[1].m_wPort.%d\n", 
             g_tCfg.m_dwRecIpAddr, m_wLocalPort );

    if( VIDEO_TWO == m_byVideoNum ||
        // zbq [02/07/2007] 单放 第二路码流文件 支持
        MEDIA_TYPE_NULL != m_tMediaInfo.m_abyMediaType[2] )
    {
        // 本地第二路视频
        tPlayParam.m_tNetSndParam.m_atLocalAddr[2].m_dwIp = g_tCfg.m_dwRecIpAddr;
        tPlayParam.m_tNetSndParam.m_atLocalAddr[2].m_wPort = m_wLocalPort + 4;
        Reclog( "[StartPlay] m_atLocalAddr[2].m_dwIp.0x%x, m_atLocalAddr[2].m_wPort.%d\n", 
                 g_tCfg.m_dwRecIpAddr, m_wLocalPort + 4);
    }
    else
    {
        tPlayParam.m_tNetSndParam.m_atLocalAddr[2].m_dwIp = 0;
        tPlayParam.m_tNetSndParam.m_atLocalAddr[2].m_wPort = 0;
    }

    // 远端音频
    tPlayParam.m_tNetSndParam.m_atRemoteAddr[0].m_dwIp = m_dwPlayIpAddr; // Mcu 地址
    tPlayParam.m_tNetSndParam.m_atRemoteAddr[0].m_wPort = m_wPlayPort + 2;
    // 远端第一路视频
    tPlayParam.m_tNetSndParam.m_atRemoteAddr[1].m_dwIp = m_dwPlayIpAddr;
    tPlayParam.m_tNetSndParam.m_atRemoteAddr[1].m_wPort = m_wPlayPort; 
    if( VIDEO_TWO == m_byVideoNum ||
        // zbq [02/07/2007] 单放 第二路码流文件 支持
        MEDIA_TYPE_NULL != m_tMediaInfo.m_abyMediaType[2] )
    {
        // 远端第二路视频
        tPlayParam.m_tNetSndParam.m_atRemoteAddr[2].m_dwIp = m_dwPlayIpAddr;
        tPlayParam.m_tNetSndParam.m_atRemoteAddr[2].m_wPort = m_wPlayPort + 4; 
    }
    else
    {
        tPlayParam.m_tNetSndParam.m_atRemoteAddr[2].m_dwIp = 0;
        tPlayParam.m_tNetSndParam.m_atRemoteAddr[2].m_wPort = 0;
    }

    g_cRecApp.SplitPath( m_achFullName,tPlayParam.m_achPlayFilePath, tPlayParam.m_achPlayFileName, TRUE );
///////////////////////////////////////////////////////////////////////////
/// u16 rpStartPlay(u16 wPlayerID, s8 *pszFileName, 
//		u16 wLocalAudioPort, u16 wLocalVideoPort, 
//		u32 dwAudioIpAddress, u16 wAudioPort, 
//		u32 dwVideoIpAddress, u16 wVideoPort,
//		EmRecordMode emMode, u32 dwFileSeconds = 0xffffffff);                                                                     */
///////////////////////////////////////////////////////////////////////////

	u16 wRet = RP_OK;
	u16 wPlaySucceed = RP_OK;
	u16 wErrCode = 0;
	u16 wAckEvent = REC_MCU_STARTPLAY_ACK;

    wRet = RPSetReadCallBackGran( m_dwDevice, 5);
	wPlaySucceed |= wRet;
    if(RP_OK != wRet)
    {
		LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_PLAY, "Fail to RPSetReadCallBackGran, code: %d\n", wRet);
        
    }
    wRet = RPSetLocalPT(m_dwDevice, &tLocalPTParam);
	wPlaySucceed |= wRet;
    if(RP_OK != wRet)
    {
		LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_PLAY, "Fail to RPSetLocalPT, code: %d\n", wRet);
        
    }
    wRet = RPSetEncryptKey(m_dwDevice, &tEncDecParam);  // 解密参数
	wPlaySucceed |= wRet;
    if(RP_OK != wRet)
    {
		LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_PLAY, "Fail to RPSetEncryptKey, code: %d\n", wRet);
        
    }
    wRet = RPStartPlay(m_dwDevice, &tPlayParam);
	wPlaySucceed |= wRet;
    if(RP_OK != wRet)
    {
		LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_PLAY, "Fail to RPStartPlay, code: %d\n", wRet);
        
    }

    //zbq[10/16/2007] FEC 支持.
    BOOL32 bAudioFEC = FECTYPE_NONE != m_tCapSupportEx.GetAudioFECType() ? TRUE : FALSE;
    wRet = RPSetFECEnable( m_dwDevice, bAudioFEC, 0 );
    if(RP_OK != wRet)
    {
        Reclog("Fail to RPSetFECEnable, Stream.0, code: %d\n", wRet);
    }
    
    BOOL32 bVideoFEC = FECTYPE_NONE != m_tCapSupportEx.GetVideoFECType() ? TRUE : FALSE;
    wRet = RPSetFECEnable( m_dwDevice, bVideoFEC, 1 );
    if(RP_OK != wRet)
    {
        Reclog("Fail to RPSetFECEnable, Stream.1, code: %d\n", wRet);
    }

    BOOL32 bDVideoFEC = FECTYPE_NONE != m_tCapSupportEx.GetDVideoFECType() ? TRUE : FALSE;
    if ( VIDEO_TWO == m_byVideoNum )
    {
        wRet = RPSetFECEnable( m_dwDevice, bDVideoFEC, 2 );
        if(RP_OK != wRet)
        {
            Reclog("Fail to RPSetFECEnable, Stream.2, code: %d\n", wRet);
        }
    }
    Reclog("[SwitchStartNotif] FECType.<%d, %d, %d> \n", 
            m_tCapSupportEx.GetAudioFECType(), 
            m_tCapSupportEx.GetVideoFECType(), m_tCapSupportEx.GetDVideoFECType() );
    
//
//        if( 0 != byNeedPrs)
//        {
//            TRPPlyRSParam tPlyRSParam;
//            tPlyRSParam.m_abRepeatSnd[0] = TRUE;
//            tPlyRSParam.m_abRepeatSnd[1] = TRUE;
//            tPlyRSParam.m_awBufTimeSpan[0] = REC_PLAY_RSTIME;  // 500ms
//            tPlyRSParam.m_awBufTimeSpan[1] = REC_PLAY_RSTIME;
//            if(VIDEO_TWO == byVideoNum)
//            {
//                tPlyRSParam.m_abRepeatSnd[2] = TRUE;
//                tPlyRSParam.m_awBufTimeSpan[2] = REC_PLAY_RSTIME;
//            }
//            else
//            {
//                tPlyRSParam.m_abRepeatSnd[2] = FALSE;
//                tPlyRSParam.m_awBufTimeSpan[2] = 0;
//            }
//            wRet = RPSetPlyRsParam( m_dwDevice, &tPlyRSParam);
//            if(RP_OK != wRet)
//            {
//                Reclog("Fail to call RPSetPlyRsParam. error: %d\n", wRet);
//            }
//        }
//
	//[liu lijiu][2010/11/04]放像机PRS支持
	if(m_byIsNeedPrs != 0)
	{
		TRPPlyRSParam tRPPlyRsParam;
		tRPPlyRsParam.m_awBufTimeSpan[0] = 2000;//设置缓存时间
		tRPPlyRsParam.m_awBufTimeSpan[1] = 2000;
		
		tRPPlyRsParam.m_abRepeatSnd[0] = TRUE;//设置是否需要重传
		tRPPlyRsParam.m_abRepeatSnd[1] = TRUE;

		if(m_byIsDstream != 0)
		{
			tRPPlyRsParam.m_awBufTimeSpan[2] = 2000;
			tRPPlyRsParam.m_abRepeatSnd[2] = TRUE;
		}
		
		wRet = ::RPSetPlyRsParam(m_dwDevice, &tRPPlyRsParam);
		if(RP_OK != wRet)
		{
			Reclog("Fail to call RPSetPlyRsParam, error: %d\n", wRet);
		}
		else
		{
			Reclog("Ok to set Prs Param, called RPSetPlyRsParam");
		}   
		
	}

    Reclog("[play]wLocalAudioPort = %d, wLocalVideoPort = %d\n",
                            m_wLocalPort + 2, m_wLocalPort);
    Reclog("dwAudioIpAddress = %x, wAudioPort = %d, wVideoPort = %d\n",
                            m_dwPlayIpAddr, m_wPlayPort+2, m_wPlayPort);
	if (RP_OK != wPlaySucceed )
	{
		if( wPlaySucceed == ERROR_SND_CREATE_SOCK )
		{
			wErrCode = ERR_REC_CREATE_SNDSOCK;
		}
		else
		{
			wErrCode  = ERR_REC_ACTFAIL;
		}
		
		cServMsg.SetErrorCode( wErrCode );
		wAckEvent = REC_MCU_STARTPLAY_NACK;
		RPStopPlay( m_dwDevice );
		SendMsgToMcu( wAckEvent, &cServMsg);
		log( LOGLVL_EXCEPTION,"[Rec] Inst.%d Play %s failed.\n",GetInsID(),tPlayParam.m_achPlayFileName );
		ReleaseChannel();
		SendChnNotify();
		return;
	}
	else
	{
		// TPlayerStatus   tPlayerStatus;
        TPlayerStatis   tPlayerStatis;
        if(RP_OK == RPGetPlayerStatis(m_dwDevice,  &tPlayerStatis))
        {
		    if( tPlayerStatis.m_dwTotalPlyTime > 0 )
		    {
			    m_tChnnlStatus.m_tProg.SetTotalTime( tPlayerStatis.m_dwTotalPlyTime );
			    m_tChnnlStatus.m_tProg.SetCurProg( tPlayerStatis.m_dwPlyTime);
		    }
		    else
            {   // 该文件长度为空
                Reclog("The length of file %s is Null\n", tPlayParam.m_achPlayFileName);
			    wErrCode  = ERR_REC_FILEDAMAGE ;
			    wAckEvent = REC_MCU_STARTPLAY_NACK;
                RPStopPlay( m_dwDevice );
				SendMsgToMcu( wAckEvent, &cServMsg);
				ReleaseChannel();
				SendChnNotify();
				return;
		    }
        }
        else
        {
            Reclog("Fail to call RPGetPlayerStatis\n");
			wErrCode  = ERR_REC_FILEDAMAGE ;
			wAckEvent = REC_MCU_STARTPLAY_NACK;
            RPStopPlay( m_dwDevice );
			SendMsgToMcu( wAckEvent, &cServMsg);
			ReleaseChannel();
			SendChnNotify();
			return;
        }
            
		log( LOGLVL_DEBUG2,"[Rec] Inst.%d Start Play %s success(%ds).\n",GetInsID(),tPlayParam.m_achPlayFileName, tPlayerStatis.m_dwTotalPlyTime);
	}
	m_tChnnlStatus.m_byState = TRecChnnlStatus::STATE_PLAYING ;
	NEXTSTATE( TRecChnnlStatus::STATE_PLAYING );
	
	SendChnNotify();
	SendPlayProgNotify();

    return;
}

/*=============================================================================
函 数 名： SwitchStartWaittimerOver
功    能： 
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/23   4.0			周广程                  创建
=============================================================================*/
void CChnInst::SwitchStartWaittimerOver( CMessage * const pcMsg )
{
	Reclog( "[SwitchStartWaittimerOver] Start switch error! Time out!\n" );
	KillTimer(EV_REC_SWITCHSTART_WAITTIMER);
	ReleaseChannel();
	SendChnNotify();
}

/*=============================================================================
  函 数 名： GetActivePayload
  功    能： 取动态载荷
  算法实现： 
  全局变量： 
  参    数： u8 byRealPayloadType
  返 回 值： u8 
=============================================================================*/
u8 CChnInst::GetActivePayload(u8 byRealPayloadType, u8 byEncryptMode, BOOL32 bDStream)
{
    u8 byAPayload = MEDIA_TYPE_NULL;
    u8 byFECType  = FECTYPE_NONE;
    
    switch(byRealPayloadType)
    {
    case MEDIA_TYPE_PCMA:
    case MEDIA_TYPE_PCMU:
    case MEDIA_TYPE_G721:
    case MEDIA_TYPE_G722:
    case MEDIA_TYPE_G7231:
    case MEDIA_TYPE_G728:
    case MEDIA_TYPE_G729:
    case MEDIA_TYPE_MP3:
    case MEDIA_TYPE_G7221C:
    case MEDIA_TYPE_G7221:
    case MEDIA_TYPE_ADPCM:
        
        byFECType = m_tCapSupportEx.GetAudioFECType();
        break;
        
    case MEDIA_TYPE_H261:
    case MEDIA_TYPE_H262:
    case MEDIA_TYPE_H263:
    case MEDIA_TYPE_MP4:
    case MEDIA_TYPE_H263PLUS:
    case MEDIA_TYPE_H264:
        
        if ( !bDStream )
        {
            byFECType = m_tCapSupportEx.GetVideoFECType();
        }
        else
        {
            byFECType = m_tCapSupportEx.GetDVideoFECType();
        }
        break;

    default:
        OspPrintf( TRUE, FALSE, "[GetActivePayload] unexpected media type.%d, ignore it\n", byRealPayloadType );
        break;
    }
    
    if ( FECTYPE_NONE != byFECType )
    {
        byAPayload = MEDIA_TYPE_FEC;
    }
    else
    {
        BOOL32 bNewActivePT = CONF_ENCRYPTMODE_NONE != byEncryptMode ? TRUE : FALSE;
        switch(byRealPayloadType)
        {
        case MEDIA_TYPE_PCMA:   byAPayload = ACTIVE_TYPE_PCMA;  break;
        case MEDIA_TYPE_PCMU:   byAPayload = ACTIVE_TYPE_PCMU;  break;
        case MEDIA_TYPE_G721:   byAPayload = ACTIVE_TYPE_G721;  break;
        case MEDIA_TYPE_G722:   byAPayload = ACTIVE_TYPE_G722;  break;
        case MEDIA_TYPE_G7231:  byAPayload = ACTIVE_TYPE_G7231; break;
        case MEDIA_TYPE_G728:   byAPayload = ACTIVE_TYPE_G728;  break;
        case MEDIA_TYPE_G729:   byAPayload = ACTIVE_TYPE_G729;  break;
        case MEDIA_TYPE_H261:   byAPayload = ACTIVE_TYPE_H261;  break;
        case MEDIA_TYPE_H262:   byAPayload = ACTIVE_TYPE_H262;  break;
        case MEDIA_TYPE_H263:   byAPayload = ACTIVE_TYPE_H263;  break;
        case MEDIA_TYPE_ADPCM:  byAPayload = bNewActivePT ? ACTIVE_TYPE_ADPCM : byRealPayloadType;  break;
        case MEDIA_TYPE_G7221C: byAPayload = bNewActivePT ? ACTIVE_TYPE_G7221C : byRealPayloadType; break;
        default:                byAPayload = byRealPayloadType; break;
        }        
    }
	return byAPayload;
}

/*====================================================================
    函数名	     ：MsgPausePlayProc
	功能		 ：暂停播放消息处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgPausePlayProc(CMessage * const pMsg)
{
	u16 wErrCode, wAckEvent,wRet;
	
	CServMsg cServMsg( pMsg->content,pMsg->length);

	if( !(m_cConfId == cServMsg.GetConfId()) )
	{
		//会议号不正确
		cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
		SendMsgToMcu( pMsg->event +2,&cServMsg );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "refuse Pause Play req,since dif confID:\n");

		s8 achConfId[80];
		m_cConfId.GetConfIdString( achConfId,sizeof(achConfId) );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "\t%s\n",achConfId);

		cServMsg.GetConfId().GetConfIdString(achConfId,sizeof(achConfId) );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "req %s\n",achConfId);
		return;
	}
	
	wErrCode  = 0;
	wAckEvent = REC_MCU_PAUSEPLAY_ACK;

	switch( CurState() )
	{
	case TRecChnnlStatus::STATE_PLAYING:
	case TRecChnnlStatus::STATE_FB:
	case TRecChnnlStatus::STATE_FF:
        wRet = RPPausePlay( m_dwDevice );
		if( wRet != RP_OK )
		{
			wErrCode  = ERR_REC_ACTFAIL;
			wAckEvent = REC_MCU_PAUSEPLAY_NACK;
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "call Device.%d rpPausePlay failed.\n", m_dwDevice);
		}
		else
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_PLAY,"[Rec] call Device.%d \"rpPausePlay\" Success.\n", m_dwDevice);

		}
		break;
	case TRecChnnlStatus::STATE_PLAYPAUSE:
		wErrCode  = ERR_REC_CURDOING;
		wAckEvent = REC_MCU_PAUSEPLAY_NACK;
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "refuse Pause play req,since pausing.\n");
		break;

	case TRecChnnlStatus::STATE_PLAYREADY:
	case TRecChnnlStatus::STATE_PLAYREADYPLAY:
		wErrCode  = ERR_REC_NOTPLAY;
		wAckEvent = REC_MCU_PAUSEPLAY_NACK;
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "refuse Pause play req,since ready play.\n");
		break;

	default:
		log( LOGLVL_EXCEPTION,"[Rec] (MsgPausePlayProc)Inst.%d at undefine state.\n",GetInsID());
    	return;
	}
	cServMsg.SetErrorCode( wErrCode );
	cServMsg.SetChnIndex( m_byChnIdx );
    cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );	
	SendMsgToMcu( wAckEvent, &cServMsg );

	if( wAckEvent ==  REC_MCU_PAUSEPLAY_ACK)
	{
        TPlayerStatis   tPlayerStatis;
        if( RP_OK == RPGetPlayerStatis(m_dwDevice, &tPlayerStatis))
        {

        // if( RP_OK !=::rpGetPlayerStatus( m_dwDevice,tPlayerStatus ) )
//            if( RP_OK != RPGetPlayerStatus(m_dwDevice, &tPlayerStatus))
//		    {
//			    log( LOGLVL_EXCEPTION,"[Rec] (MsgPausePlayProc)Inst.%d call \"rpGetPalyerStatus\" failed.\n",GetInsID());
//                OspPrintf(TRUE, FALSE, "[Rec] Fail to call RPGetPlayerStatus\n");
//		    }
        }
        else
        {
            LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "Fail to call RPGetPlayerStatis\n");
        }

		m_tChnnlStatus.m_byState	= TRecChnnlStatus::STATE_PLAYPAUSE ;
		m_tChnnlStatus.m_tProg.SetCurProg( tPlayerStatis.m_dwPlyTime );
		m_tChnnlStatus.m_tProg.SetTotalTime( tPlayerStatis.m_dwTotalPlyTime );
		
		NEXTSTATE( TRecChnnlStatus::STATE_PLAYPAUSE );

		SendChnNotify();
	}
    return;
}

/*====================================================================
    函数名	     ：MsgResumePlayProc
	功能		 ：恢复播放消息处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgResumePlayProc(CMessage * const pMsg)
{
	u16 wErrCode,wAckEvent,wRet;
	
	CServMsg cServMsg( pMsg->content,pMsg->length);
	
	if( !(m_cConfId == cServMsg.GetConfId()) )
	{
		//会议号不正确
		cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
		SendMsgToMcu( pMsg->event +2,&cServMsg );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "refuse Resume Play req,since dif confID:\n");

		s8 achConfId[80];
		m_cConfId.GetConfIdString( achConfId,sizeof(achConfId) );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "\t%s\n",achConfId);

		cServMsg.GetConfId().GetConfIdString(achConfId,sizeof(achConfId) );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "req %s\n",achConfId);
		return;
	}

	wErrCode  = 0;
	wAckEvent = REC_MCU_RESUMEPLAY_ACK;
	
	switch( CurState() )
	{

	case TRecChnnlStatus::STATE_PLAYPAUSE:
            wRet = RPResumePlay( m_dwDevice );
			if( wRet != RP_OK )
			{
				wErrCode  = ERR_REC_ACTFAIL;
				wAckEvent = REC_MCU_RESUMEPLAY_NACK;

				LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "[Rec] call Device.%d \"rpContinuePlay\" failed.\n", m_dwDevice);
			}
			else
			{
				LogPrint(LOG_LVL_DETAIL, MID_RECEQP_PLAY, "call Device.%d \"rpContinuePlay\" success.\n", m_dwDevice);
			}
		break;
        
	case TRecChnnlStatus::STATE_PLAYING:
	case TRecChnnlStatus::STATE_FB:
	case TRecChnnlStatus::STATE_FF:
		wErrCode  =  ERR_REC_PLAYING ;
		wAckEvent =  REC_MCU_RESUMEPLAY_NACK;
		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_PLAY, "refuse Resume play req,since not pause.\n");
			
		break;

	case TRecChnnlStatus::STATE_PLAYREADY:
	case TRecChnnlStatus::STATE_PLAYREADYPLAY:
		wErrCode  =  ERR_REC_NOTPLAY ;
		wAckEvent =  REC_MCU_RESUMEPLAY_NACK;
		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_PLAY, "refuse Resume play req,since not play.\n");

		break;

	default:
		log( LOGLVL_EXCEPTION,"[Rec] (MsgResumePlayProc)Inst.%d at undefine state.\n",GetInsID());
		return ;
	}

	cServMsg.SetErrorCode( wErrCode );
	cServMsg.SetChnIndex( m_byChnIdx );
	cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
	SendMsgToMcu( wAckEvent, &cServMsg);

	if( wAckEvent == REC_MCU_RESUMEPLAY_ACK)
	{
        TPlayerStatis   tPlayerStatis;

        if( RP_OK == RPGetPlayerStatis( m_dwDevice, &tPlayerStatis) )
        {
            // if( RP_OK !=::rpGetPlayerStatus( m_dwDevice,tPlayerStatus ) )
//            if( RP_OK != RPGetPlayerStatus( m_dwDevice, &tPlayerStatus))
//		    {
//			    log( LOGLVL_EXCEPTION,"[Rec] (MsgResumePlayProc)Inst.%d call \"rpGetPalyerStatus\" failed.\n",GetInsID());
//                OspPrintf(TRUE, FALSE, "[Rec] Fail to call RPGetPlayerStatus\n");
//		    }
        }
        else
        {
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_PLAY, "Fail to call RPGetPlayerStatis\n");
        }

		m_tChnnlStatus.m_byState	= TRecChnnlStatus::STATE_PLAYING ;
		m_tChnnlStatus.m_tProg.SetCurProg( tPlayerStatis.m_dwPlyTime );
		m_tChnnlStatus.m_tProg.SetTotalTime( tPlayerStatis.m_dwTotalPlyTime );

		NEXTSTATE( TRecChnnlStatus::STATE_PLAYING );

		SendChnNotify();

		SendPlayProgNotify(); // 放像进度通知
	}
    return;
}

/*====================================================================
    函数名	     ：MsgFFPlayProc
	功能		 ：快进消息处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgFFPlayProc(CMessage* const pMsg)
{
	u16 wErrCode,wAckEvent,wRet;
	
	CServMsg cServMsg( pMsg->content,pMsg->length);
	
	if( !(m_cConfId == cServMsg.GetConfId()) )
	{
		//会议号不正确
		cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
		SendMsgToMcu( pMsg->event +2,&cServMsg );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "[Rec] refuse FF req,since dif confID:\n");

		s8 achConfId[80];
		m_cConfId.GetConfIdString( achConfId,sizeof(achConfId) );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "\t%s\n",achConfId);

		cServMsg.GetConfId().GetConfIdString(achConfId,sizeof(achConfId) );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "req %s\n",achConfId);

		return;
	}

	wErrCode  = 0;
	wAckEvent = REC_MCU_FFPLAY_ACK;
    u8 byQuickTime = 4;     // 块进倍数
	switch( CurState() )
	{
	case TRecChnnlStatus::STATE_PLAYING:
	case TRecChnnlStatus::STATE_PLAYPAUSE:
	case TRecChnnlStatus::STATE_FB:
        wRet = RPQuickPlay(m_dwDevice, byQuickTime);
		if( wRet != RP_OK)
		{
			wErrCode  = ERR_REC_ACTFAIL;
			wAckEvent = REC_MCU_FFPLAY_NACK;
			
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "call Device.%d rpFastForword failed.\n", m_dwDevice);
		}
		else
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_PLAY, "[Rec] call Device.%d \"rpFastForword\" Success.\n", m_dwDevice);
		}
		break;

	case TRecChnnlStatus::STATE_FF:
		wErrCode  = ERR_REC_CURDOING;
		wAckEvent = REC_MCU_FFPLAY_NACK;
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "refuse FF req,since fast forwording.\n");

		break;

	case TRecChnnlStatus::STATE_PLAYREADY:
		wErrCode  = ERR_REC_NOTPLAY;
		wAckEvent = REC_MCU_FFPLAY_NACK;
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "refuse FF req,since not play.\n");

		break;

	default:
		log( LOGLVL_EXCEPTION,"[Rec] (MsgFFPlayProc)Inst.%d at undefine state(%d).\n",GetInsID(),CurState());
		return;
	}

	u8 bySpeed =1;
	cServMsg.SetErrorCode( wErrCode );
	cServMsg.SetChnIndex( m_byChnIdx );
	cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
	cServMsg.CatMsgBody( &bySpeed,sizeof(bySpeed) );
	SendMsgToMcu( wAckEvent,&cServMsg);

	if(wAckEvent == REC_MCU_FFPLAY_ACK)
	{
        TPlayerStatis   tPlayerStatis;

        if( RP_OK == RPGetPlayerStatis( m_dwDevice, &tPlayerStatis) )
        {
            // if( RP_OK !=::rpGetPlayerStatus( m_dwDevice,tPlayerStatus ) )
//            if( RP_OK != RPGetPlayerStatus( m_dwDevice, &tPlayerStatus))
//		    {
//			    log( LOGLVL_EXCEPTION,"[Rec] (MsgFFPlayProc)Inst.%d call \"rpGetPalyerStatus\" failed.\n",GetInsID());
//                OspPrintf(TRUE, FALSE, "[Rec] Fail to  call RPGetPlayerStatus\n");
//		    }
        }
        else
        {
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "Fail to  call RPGetPlayerStatis\n");
        }
		m_tChnnlStatus.m_byState	= TRecChnnlStatus::STATE_FF ;
		m_tChnnlStatus.m_tProg.SetCurProg( tPlayerStatis.m_dwPlyTime );
		m_tChnnlStatus.m_tProg.SetTotalTime( tPlayerStatis.m_dwTotalPlyTime );

		NEXTSTATE( TRecChnnlStatus::STATE_FF );
		
		SendChnNotify();
	}
    return;
}

/*====================================================================
    函数名	     ：MsgFBPlayProc
	功能		 ：快退消息处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgFBPlayProc(CMessage* const pMsg)
{
	u16 wErrCode,wAckEvent,wRet;
	
	CServMsg cServMsg( pMsg->content,pMsg->length);

	if( !(m_cConfId == cServMsg.GetConfId()) )
	{
		//会议号不正确
		cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
		SendMsgToMcu( REC_MCU_FBPLAY_NACK, &cServMsg );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "refuse FB req,since dif confID:\n");

		s8 achConfId[80];
		m_cConfId.GetConfIdString( achConfId,sizeof(achConfId) );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "\t%s\n",achConfId);


		cServMsg.GetConfId().GetConfIdString(achConfId,sizeof(achConfId) );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "req %s\n",achConfId);

		return;
	}

	// 不支持快退
	wErrCode  = 0;
	wAckEvent = REC_MCU_FBPLAY_ACK;

	switch( CurState() )
	{
	case TRecChnnlStatus::STATE_PLAYING:
	case TRecChnnlStatus::STATE_PLAYPAUSE:
	case TRecChnnlStatus::STATE_FF:
        wRet = RPSlowPlay( m_dwDevice, 1 );   // 不支持快退
		if( wRet != RP_OK)
		{
			wErrCode  = ERR_REC_ACTFAIL;
			wAckEvent = REC_MCU_FBPLAY_NACK;

			LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "call Device.%d \"rpFastBack\" failed.\n", m_dwDevice);
		}
		else
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_PLAY, "[Rec] call Device.%d \"rpFastBack\" Success.\n", m_dwDevice);
		}
		break;

	case TRecChnnlStatus::STATE_FB:
		wErrCode  = ERR_REC_CURDOING;
		wAckEvent = REC_MCU_FBPLAY_NACK;
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "refuse FB req,since fast back playing .\n");

		break;

	case TRecChnnlStatus::STATE_PLAYREADY:
		wErrCode  = ERR_REC_NOTPLAY;
		wAckEvent = REC_MCU_FBPLAY_NACK;
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "refuse FB req,since not playing.\n");

		break;

	default:
		log( LOGLVL_EXCEPTION,"[Rec] (MsgFBPlayProc)Inst.%d at undefine state(%d).\n",GetInsID(),CurState());
		return;
	}
	
	u8 bySpeed =1;
	cServMsg.SetErrorCode( wErrCode );
	cServMsg.SetChnIndex( m_byChnIdx );
	cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
	cServMsg.CatMsgBody( &bySpeed,sizeof(bySpeed) );
	SendMsgToMcu( wAckEvent,&cServMsg);

	if(wAckEvent == REC_MCU_FBPLAY_ACK)
	{
		TPlayerStatis   tPlayerStatis;
        if( RP_OK == RPGetPlayerStatis( m_dwDevice, &tPlayerStatis) )
        {
            // if( RP_OK !=::rpGetPlayerStatus( m_dwDevice,tPlayerStatus ) )
//            if( RP_OK != RPGetPlayerStatus( m_dwDevice, &tPlayerStatus))
//		    {
//			    // log( LOGLVL_EXCEPTION,"[Rec] (MsgFBPlayProc)Inst.%d call \"rpGetPalyerStatus\" failed.\n",GetInsID());
//                OspPrintf(TRUE, FALSE, "[Rec] Fail to call RPGetPlayerStatus\n");
//		    }
        }
        else
        {
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "Fail to call RPGetPlayerStatis\n");
        }

		m_tChnnlStatus.m_byState	= TRecChnnlStatus::STATE_FB ;
		m_tChnnlStatus.m_tProg.SetCurProg( tPlayerStatis.m_dwPlyTime );
		m_tChnnlStatus.m_tProg.SetTotalTime( tPlayerStatis.m_dwTotalPlyTime );

		NEXTSTATE( TRecChnnlStatus::STATE_FB );
		
		SendChnNotify();
	}
    return;
}

/*====================================================================
    函数名	     ：MsgStopPlayProc
	功能		 ：停止播放消息处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgStopPlayProc(CMessage* const pMsg)
{
	u16 wErrCode,wAckEvent,wRet;
	
	CServMsg cServMsg( pMsg->content,pMsg->length);
	
	if( !(m_cConfId == cServMsg.GetConfId()) )
	{
		//会议号不正确
		cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
		SendMsgToMcu( pMsg->event +2,&cServMsg );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "Refuse Stop Play,since dif confID:\n");

		s8 achConfId[80];
		m_cConfId.GetConfIdString( achConfId,sizeof(achConfId) );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "\t%s\t",achConfId);

		cServMsg.GetConfId().GetConfIdString(achConfId,sizeof(achConfId) );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "Req: %s\n",achConfId);
		return;
	}

	wErrCode  = 0;
	wAckEvent = REC_MCU_STOPPLAY_ACK;
	cServMsg.SetMsgBody( NULL, 0);
    
    //s32 InstId = GetInsID();
//    TMediaInfo tMediaInfo;
//    memset(&tMediaInfo, 0, sizeof(TMediaInfo));
//    
//    wRet = RPGetFileMediaInfo(m_dwDevice, g_cRecApp.m_achUsingFile[InstId], &tMediaInfo);
//    if(RP_OK != wRet)
//    {
//        Reclog("Fail to get %s media info, error code: %d\n",g_cRecApp.m_achUsingFile[InstId], wRet);
//        tMediaInfo.m_byNum = 3; // 写死为3，测试
//    }
//    else
//    {
//    }

	// 不能在播放媒体文件的时候调用RPGetFileMediaInfo, 该为使用在开始播放文件时保存下来的meidainfo, zgc, 2007-03-05
    u8 byVideoNum = 0;
    if( MEDIA_TYPE_H262 <= m_tMediaInfo.m_abyMediaType[1] && 
        MEDIA_TYPE_H264 >= m_tMediaInfo.m_abyMediaType[1])
    {
        byVideoNum++;
    }

    if( MEDIA_TYPE_H262 <= m_tMediaInfo.m_abyMediaType[2] && 
        MEDIA_TYPE_H264 >= m_tMediaInfo.m_abyMediaType[2])
    {
        byVideoNum++;
    }
    u8 byDoubleStream = ( VIDEO_TWO == byVideoNum ||
                          // zbq [02/06/2007] 单放 第二路码流文件 支持
                          MEDIA_TYPE_NULL != m_tMediaInfo.m_abyMediaType[2]) ? 1 : 0;


	switch( CurState() )
	{
	case TRecChnnlStatus::STATE_PLAYING:
	case TRecChnnlStatus::STATE_PLAYPAUSE:
	case TRecChnnlStatus::STATE_FB:
	case TRecChnnlStatus::STATE_FF:
        wRet = RPStopPlay( m_dwDevice );
		if( wRet != RP_OK )
		{
			wErrCode  = ERR_REC_ACTFAIL;
			wAckEvent = REC_MCU_STOPPLAY_NACK;
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "Call RPStopPlay fail (Device: %d).\n", m_dwDevice);

		}
		else
		{
		}
		break;

	case TRecChnnlStatus::STATE_PLAYREADY:
		wErrCode  = ERR_REC_CURDOING;
		wAckEvent = REC_MCU_STOPPLAY_NACK;
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "Inst.%d refuse Mcu stop play REQ,since already stop.\n",GetInsID());

		break;

	case TRecChnnlStatus::STATE_PLAYREADYPLAY:
		KillTimer(EV_REC_SWITCHSTART_WAITTIMER);
		break;

	default:
		Reclog("(MsgStopPlayProc)Instance%d at undefine state(%d).\n",GetInsID(),CurState());
		return;
	}

	
	cServMsg.SetErrorCode( wErrCode );
	cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
    cServMsg.CatMsgBody(&byDoubleStream, sizeof(u8));
	SendMsgToMcu( wAckEvent,&cServMsg);
//	cServMsg.SetMsgBody( (u8*)&m_tSrvTmt,sizeof(m_tSrvTmt) );
//	cServMsg.CatMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );


	if( wAckEvent == REC_MCU_STOPPLAY_ACK)
	{
		//释放使用文件
		ReleaseChannel();

		SendChnNotify();

		m_cConfId.SetNull();
	}
    return;
}

/*====================================================================
    函数名	     ：MsgSeekPlayProc
	功能		 ：选择指定位置播放消息处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgSeekPlayProc(CMessage* const pMsg)
{
	u16 wErrCode, wAckEvent, wRet = 0;
	
	TRecProg* ptProg = NULL;
    Reclog("Process Seek Play\n");

	CServMsg cServMsg( pMsg->content,pMsg->length);

	if( !(m_cConfId == cServMsg.GetConfId()) )
	{
		//会议号不正确
		cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
		SendMsgToMcu( pMsg->event +2,&cServMsg );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "refuse Seek Play req,since dif confID:\n");

		s8 achConfId[80];
		m_cConfId.GetConfIdString( achConfId,sizeof(achConfId) );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "\t%s\n",achConfId);

		cServMsg.GetConfId().GetConfIdString(achConfId,sizeof(achConfId) );
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "req %s\n",achConfId);

		return;
	}


	ptProg	= (TRecProg*)( (s8*)cServMsg.GetMsgBody() + sizeof(TEqp) );

	wErrCode  = 0;
	wAckEvent = REC_MCU_SEEK_ACK;
	
	switch( CurState() ) 
	{
	case TRecChnnlStatus::STATE_PLAYING:
	case TRecChnnlStatus::STATE_FF:
	case TRecChnnlStatus::STATE_FB:
		{
			// wRet = ::rpSetPlayPosition( m_dwDevice, (u16)ptProg->GetCurProg());
            // 不支持
            wRet = RPDragDropPlay( m_dwDevice, (u32)ptProg->GetCurProg());
			if( wRet != RP_OK )
			{
				wErrCode  = ERR_REC_ACTFAIL;
				wAckEvent = REC_MCU_SEEK_NACK;
				LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "Call Device.%d RPDragDropPlay failed.\n", m_dwDevice);

			}
			else
			{
				m_tChnnlStatus.m_tProg.SetCurProg( ptProg->GetCurProg() );
				LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_PLAY, "Call Device.%d RPDragDropPlay Success. SetPos: %d, total: %d\n", m_dwDevice,ptProg->GetCurProg(),
					ptProg->GetTotalTime());
			}

		}
		break;
	case TRecChnnlStatus::STATE_PLAYPAUSE:
	case TRecChnnlStatus::STATE_RECPAUSE:
		wErrCode  = ERR_REC_NOTPLAY;
		wAckEvent = REC_MCU_SEEK_NACK;
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "[Rec] refuse Seek play req,since not play.\n");

		break;

	default:
		log( LOGLVL_EXCEPTION,"[Rec] (MsgStopPlayProc)Inst.%d at undefine state(%d).\n",GetInsID(),CurState());
		return;
	}

	cServMsg.SetErrorCode( wErrCode );
	cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
	SendMsgToMcu( wAckEvent,&cServMsg);

	if(  REC_MCU_SEEK_ACK == wAckEvent )
	{
		SendPlayProgNotify();
	}
	return;
}

/*====================================================================
    函数名	     ：MsgGetRecChnStatusProc
	功能		 ：MCU获取去录像通道状态处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgGetRecChnStatusProc( CMessage * const pMsg,CApp * pcApp )
{
	CChnInst* pInst;
	CServMsg   cServMsg(pMsg->content,pMsg->length);
	u8 byChnIdx	= cServMsg.GetChnIndex();

	if( byChnIdx < g_byRecChnNum )
	{
		pInst	= ( CChnInst* )pcApp->GetInstance( byChnIdx+1 );

		cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );	
		cServMsg.CatMsgBody( (u8*)&pInst->m_tChnnlStatus,sizeof(TRecChnnlStatus) );

		SendMsgToMcu( REC_MCU_GETRECORDCHNSTATUS_ACK,&cServMsg );
	}
	else
	{
		cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
		cServMsg.SetErrorCode( ERR_REC_UMMATCHCMD );
		SendMsgToMcu( REC_MCU_GETRECORDCHNSTATUS_NACK,&cServMsg );
	}
    return;
}

/*====================================================================
    函数名	     ：MsgGetPlayChnStatusProc
	功能		 ：MCU获取去放像通道状态处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgGetPlayChnStatusProc( CMessage* const pMsg,CApp* pcApp )
{
	CChnInst* pInst;
	CServMsg   cServMsg(pMsg->content,pMsg->length);
	u8 byChnIdx	= cServMsg.GetChnIndex();

	if( byChnIdx < g_byPlayChnNum )
	{
		pInst	= ( CChnInst* )pcApp->GetInstance( byChnIdx + g_byRecChnNum +1 );
		
		cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );	
		cServMsg.CatMsgBody( (u8*)&pInst->m_tChnnlStatus,sizeof(TRecChnnlStatus) );

		SendMsgToMcu( REC_MCU_GETRECORDCHNSTATUS_ACK,&cServMsg );
	}
	else
	{
		cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
		cServMsg.SetErrorCode( ERR_REC_UMMATCHCMD );
		SendMsgToMcu( REC_MCU_GETRECORDCHNSTATUS_NACK,&cServMsg );
	}
    return;
}

/*====================================================================
    函数名	     ：MsgGetRecStateProc
	功能		 ：MCU获取去录像机状态处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/03/29	3.6			liuhuiyun	  注释内容
	05/04/13	3.6			liuhuiyun	  代码规范化
====================================================================*/
void CChnInst::MsgGetRecStateProc(CMessage* const pMsg,CApp* pcApp)
{
	s32 nLp;
	CChnInst      *pInst;
	TPeriEqpStatus tEqpStatus;

	tEqpStatus.m_byOnline	= 1;
	tEqpStatus.m_tStatus.tRecorder.SetPublicAttribute(g_tCfg.m_bOpenDBSucceed);
	tEqpStatus.SetMcuEqp( (u8)g_tCfg.m_wMcuId,
		                  g_tCfg.m_byEqpId,
						  g_tCfg.m_byEqpType);

	CServMsg   cServMsg(pMsg->content,pMsg->length);


	tEqpStatus.m_tStatus.tRecorder.SetChnnlNum( g_byRecChnNum,
												g_byPlayChnNum );
	for(nLp=0 ;nLp< ( g_byRecChnNum )  ;nLp++)
	{
		pInst     = (CChnInst*)pcApp->GetInstance(nLp+1);

		tEqpStatus.m_tStatus.tRecorder.SetChnnlStatus( nLp,
								  pInst->m_tChnnlStatus.m_byType,
								  &pInst->m_tChnnlStatus );
	}

	for(nLp=0 ;nLp< ( g_byPlayChnNum )  ;nLp++)
	{
		pInst     = (CChnInst*)pcApp->GetInstance( g_byRecChnNum +nLp+1);

		tEqpStatus.m_tStatus.tRecorder.SetChnnlStatus( nLp,
								  pInst->m_tChnnlStatus.m_byType,
								  &pInst->m_tChnnlStatus );
	}
	 
	cServMsg.SetMsgBody( (u8 *)&tEqpStatus,sizeof(tEqpStatus) );

	SendMsgToMcu( REC_MCU_GETRECSTATUS_ACK,&cServMsg );
}

/////////////////////////////////////
/*void CChnInst::MsgGetChnStateProc(CMessage* const pMsg,CApp* pcApp)
{
	CServMsg cServMsg( pMsg->content,pMsg->length );

	cServMsg.SetMsgBody( (u8*)&m_tChnnlStatus,sizeof(m_tChnnlStatus) );

	SendMsgToMcu( REC_MCU_GETCHNSTATE_ACK,&cServMsg );

}*/

/*====================================================================
    函数名	     ：MsgChnInitProc
	功能		 ：信道初始化处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgChnInitProc( CMessage* const pMsg )
{
    u16 wInstId = GetInsID();
	
	m_tChnnlStatus.m_tProg.SetCurProg( 0 );
	m_tChnnlStatus.m_tProg.SetTotalTime( 0 );
	
	if( wInstId <= g_byRecChnNum )
	{
		m_byChnIdx = wInstId-1;

        u16 wRet = ::RPCreateRec( &m_dwDevice );
        if( RP_OK != wRet )
        {
			LogPrint(LOG_LVL_ERROR, MID_RECEQP_COMMON, "Fail to create recorder Device.%d\n due to reason: %d !\n", wInstId, wRet);

            ::RPRelease();
            AfxMessageBox("Fail to create Device Recorder");
            exit(0);
            //return;
        }

		//2012/[6/11 zhangli] 设置录像机丢帧是否写文件 
		if (RP_OK != ::RPSetRecDebugInfo(m_dwDevice, g_tCfg.m_bIsWriteAsfWhenLoseFrame, g_tCfg.m_byAudioBuffSize,
			g_tCfg.m_byVideoBuffSize, g_tCfg.m_byDsBuffSize, g_tCfg.m_byLeftBuffSize))
		{
			LogPrint(LOG_LVL_ERROR, MID_RECEQP_COMMON, "Fail to set rec param isWriteAsfWhenLoseFrame due to reason: %d !\n", wRet);
			
            ::RPRelease();
			exit(0);
		}

        g_adwDevice[wInstId] = m_dwDevice;   // 记录每个通道的设备号
		g_cRecApp.m_wDevCount++;
		m_tChnnlStatus.m_byType	  = TRecChnnlStatus::TYPE_RECORD ;
		m_tChnnlStatus.m_byState  = TRecChnnlStatus::STATE_RECREADY ;

		NEXTSTATE( TRecChnnlStatus::STATE_RECREADY );
	}
	else if( ( wInstId <= (g_byRecChnNum + g_byPlayChnNum ) )
		     && ( wInstId > g_byRecChnNum ) )
	{
		m_byChnIdx	 = wInstId - g_byRecChnNum - 1;
        m_wLocalPort = g_tCfg.m_wPlayStartPort + m_byChnIdx*(PORTSPAN);

        if(RP_OK != RPCreatePlay( &m_dwDevice ))
        {
			LogPrint(LOG_LVL_ERROR, MID_RECEQP_COMMON, "Fail to create Player Device.%d\n", wInstId);
            
            ::RPRelease();
            AfxMessageBox("Fail to create Player");
            exit(0);
            //return;
        }

        //2012/[6/11 zhangli] 设置放像非标帧长 
		if (RP_OK != ::RPSetFrameSizeInfo(m_dwDevice, g_tCfg.GetFrameSizeInfo()))
		{
			LogPrint(LOG_LVL_ERROR, MID_RECEQP_COMMON, "Fail to SetFrameSizeInfo Device.%d\n", m_dwDevice);
            
            ::RPRelease();
            AfxMessageBox("Fail to create Player");
            exit(0);
		}

        g_adwDevice[wInstId] = m_dwDevice;        // 记录每个通道的设备号
        g_cRecApp.m_wDevCount++;
		m_tChnnlStatus.m_byType	  = TRecChnnlStatus::TYPE_PLAY ;
		m_tChnnlStatus.m_byState  = TRecChnnlStatus::STATE_PLAYREADY ;

		NEXTSTATE( TRecChnnlStatus::STATE_PLAYREADY );
	}
	else
	{
		m_byChnIdx   	= 0xff;
		m_tChnnlStatus.m_byType		= TRecChnnlStatus::TYPE_UNUSE ;
		m_tChnnlStatus.m_byState	= TRecChnnlStatus::STATE_IDLE ;
	}
    return;
}

/*====================================================================
    函数名	     ：MsgDeviceNotifyProc
	功能		 ：底层录像库通知消息处理函数
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgDeviceNotifyProc(CMessage * const pMsg)
{
    TNotifyInfo tNotifyInfo;
	CServMsg cServMsg( pMsg->content, pMsg->length );	

	cServMsg.GetMsgBody((u8*)&tNotifyInfo, sizeof(tNotifyInfo));

	cServMsg.SetChnIndex( m_byChnIdx);
	cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp) );
	cServMsg.SetConfId( m_cConfId );

    //u16 wChannelNum = 0;
	switch( tNotifyInfo.wAlarmType )
	{
    // 磁盘满, 50M以下
	case emRECORDER_FULLDISK: 
        StopAllChannel(FALSE); 
        OspPrintf( TRUE, FALSE, "[REC][Error] emRECORDER_FULLDISK!\n" );
		       
        // 没有 break;

    case emRECORDER_SMALL_FREESPACE:
		
        // 同时提高录像机的空间上报频率，方便MCS从报警状态恢复
        KillTimer( EV_REC_STATUSNOTYTIME );
        SetTimer( EV_REC_STATUSNOTYTIME, 10 );

        // 发送告警通知
		cServMsg.SetErrorCode( ERR_REC_DISKFULL );
		SendMsgToMcu( REC_MCU_EXCPT_NOTIF, &cServMsg );

		break;

		// [8/11/2011 liuxu] 录像写文件出现异常的处理
	case emRECORDER_WRITE_FILE_ERR:
		cServMsg.SetErrorCode( ERR_REC_WRITE_FILE_ERR );
		SendMsgToMcu( REC_MCU_EXCPT_NOTIF, &cServMsg );
		
		post( MAKEIID(GetAppID(), (u8)tNotifyInfo.wChnIndex), 
			EV_REC_STOPDEVICE,
			cServMsg.GetMsgBody(),
			cServMsg.GetMsgBodyLen() ); // 通知关闭
		
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_COMMON, "Write file error!\n");
		
		break;

    // 空文件, 异常
	case emPLAYEREMPTYFILE:   
		cServMsg.SetErrorCode( ERR_REC_FILEEMPTY );
		SendMsgToMcu( REC_MCU_EXCPT_NOTIF, &cServMsg );

		post( MAKEIID(GetAppID(),GetInsID()), 
			  EV_REC_STOPDEVICE,
			  cServMsg.GetMsgBody(),
			  cServMsg.GetMsgBodyLen() ); // 通知关闭

		LogPrint(LOG_LVL_WARNING, MID_RECEQP_COMMON, "Empty File !.\n");

		break;

	// 完成任务
	case emPLAYFINISHTASK:	
		post( MAKEIID( GetAppID(), GetInsID()), 
		      EV_REC_STOPDEVICE,
		      cServMsg.GetMsgBody(),
		      cServMsg.GetMsgBodyLen() ); // 通知关闭

		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "Receive finish notify from rpLib.\n");
		break;
		
	// 放像进程时间
	case emPLAYERPROCESS:
        {
			/*u8 byRet =*/ SendPlayProgNotify(); // 放像进度通知
            // if( FINISH_PLAY == byRet)
            if( FINISH_PLAY == tNotifyInfo.dwPlayTime )
            {
				
				s8 achNextFile[MAX_FILE_NAME_LEN];    // add by jlb 081027
                memset(achNextFile, 0, sizeof(achNextFile));    // add by jlb 081027
                //if (!GetNextSwitchFile(m_achFullName, achNextFile))
                {

                    u8 byInst = (u8)GetRecChnIndex(m_dwDevice);
                    if(byInst <= g_byRecChnNum) // 跳过录像通道
					{
                        byInst = byInst + g_byRecChnNum;
					}

                    Reclog("Finish to play .....the chn: %d, GetInsID = %d\n", byInst, GetInsID());
                    // u8 byInst = GetRecChnIndex(m_dwDevice);
                    post( MAKEIID(GetAppID(), GetInsID()), 
			              EV_REC_STOPDEVICE,
			              cServMsg.GetMsgBody(),
			              cServMsg.GetMsgBodyLen() ); // 通知关闭
				}
            }
        }
		break;
		
	default:
		break;
	}
    return;
}

/*====================================================================
    函数名	     ：GetCurChnInfoStr
	功能		 ：得到当前通道信息（序号、类型、状态）
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
LPCSTR CChnInst::GetCurChnInfoStr()
{
	s8 achType[16]  = {0};
	s8 achState[16] = {0};

	u16 wInstId;
	wInstId = GetInsID();
	if( wInstId == CInstance::DAEMON )
	{
		strcpy( m_achChnInfo," DAEMON ");
	}
	else
	{
		switch( m_tChnnlStatus.m_byType ) 
		{
		case TRecChnnlStatus::TYPE_PLAY:
			strcpy( achType,"TYPE_PLAY");
			break;
		case TRecChnnlStatus::TYPE_RECORD:
			strcpy( achType,"TYPE_RECORD");
			break;
		default:
			strcpy( achType,"Invalid type");
			break;
		}

		switch( CurState() )
		{
		case TRecChnnlStatus::STATE_PLAYREADY:
			strcpy( achState,"STATE_PLAYREADY");
			break;
		case TRecChnnlStatus::STATE_PLAYING:
			strcpy( achState,"STATE_PLAYING");
			break;
		case TRecChnnlStatus::STATE_FF:
			strcpy( achState,"STATE_FF");
			break;
		case TRecChnnlStatus::STATE_FB:
			strcpy( achState,"STATE_FB");
			break;
		case TRecChnnlStatus::STATE_PLAYPAUSE:
			strcpy( achState,"STATE_PLAYPAUSE");
			break;
		case TRecChnnlStatus::STATE_RECREADY:
			strcpy( achState,"STATE_RECREADY");
			break;
		case TRecChnnlStatus::STATE_RECORDING:
			strcpy( achState,"STATE_RECORDING");
			break;
		case TRecChnnlStatus::STATE_RECPAUSE:
			strcpy( achState,"STATE_RECPAUSE");
			break;
		default:
			strcpy( achState,"Invalid State");
			break;
		}
		sprintf( m_achChnInfo,"Ch.%d %s %s",wInstId,achType,achState);
	}
	return m_achChnInfo;
}

/*====================================================================
    函数名	     ：MsgForceStopProc
	功能		 ：强行关闭设备
	引用全局变量 ：无
    输入参数说明 ：pMsg - 消息指针
                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/12	3.6			liuhuiyun	  增加对未定义的实例号的消息提示
====================================================================*/
void CChnInst::MsgForceStopProc( CMessage *const pMsg  )
{
	u16 wRet;
	u16 wInstId;
//	BOOL bAutoPlay;   //是否自动播放
    
    wInstId = GetInsID();

	if( wInstId <= g_byRecChnNum )
	{
        // 录像通道
		if( TRecChnnlStatus::STATE_RECREADY != CurState() )
		{
            wRet = ::RPStopRec( m_dwDevice, 0 );
			if( RP_OK != wRet  )
			{
				Reclog("Fail to force stop device %d, error code: %d\n", m_dwDevice, wRet);
			}
			else
			{
				LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "[MsgForceStopProc]call device%d \"rpStopRecord\" success.\n", m_dwDevice );
			}
			
			if (PUBLISH_MODE_IMMED == m_byPublishMode)
			{
				DeleteBrdPubPoint();
				DeleteOneLiveSource();
				DeletePlayList();
			}

			if (m_byPublishMode != PUBLISH_MODE_NONE)
			{
				PublishOneFile(m_achFullName);
			}

			m_tChnnlStatus.m_byState = TRecChnnlStatus::STATE_RECREADY;
			m_tChnnlStatus.m_tProg.SetCurProg( 0 );
			m_tChnnlStatus.m_tProg.SetTotalTime( 0 );
			m_tChnnlStatus.SetRecordName("");
			m_tChnnlStatus.m_tSrc.SetNull();
			NEXTSTATE( TRecChnnlStatus::STATE_RECREADY );
			SendChnNotify();
			m_cConfId.SetNull();
            KillTimer(EV_REC_RECPROGTIME);
		}
		
	}
	else if( (wInstId <= g_byRecChnNum + g_byPlayChnNum)&&
		     ( wInstId > g_byRecChnNum ))
	{
        // 放像通道
		if( TRecChnnlStatus::STATE_PLAYREADY != CurState() )
		{
			KillTimer(EV_REC_SWITCHSTART_WAITTIMER);
            wRet = ::RPStopPlay( m_dwDevice );
            if( RP_OK != wRet  )
			{
				LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "Fail to force stop device %d, error code: %d\n", m_dwDevice);
				return;
			}
			if ( !AutoPlayNextSwitchFile() )    //add by jlb 081027
            {
//				bAutoPlay = TRUE;
			    ReleaseChannel();
		     	SendChnNotify();
			    m_cConfId.SetNull();
			}
		}
	}
	else
	{
		// 未定义的实例号
		OspPrintf(TRUE, FALSE, "[MsgForceStopProc]Undefined Instance ID: %d\n", wInstId);
		return;
	}

	//释放使用文件
	memset( g_cRecApp.m_achUsingFile[wInstId], 0, MAX_FILE_NAME_LEN );
    return;

}


/*====================================================================
    函数名	     ：AutoPlayNextSwitchFile
	功能		 ：自动播放切换产生的下一个文件
	引用全局变量 ：
    输入参数说明 ：
	返回值说明   ：B0OL： TRUE.需要，FALSE.不需要
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    01/11/2008  4.0         张宝卿          创建
====================================================================*/
BOOL CChnInst::AutoPlayNextSwitchFile( void )
{
    s8 achNextFileName[MAX_FILE_NAME_LEN];
    memset( achNextFileName, 0, sizeof(achNextFileName) );
    if ( !GetNextSwitchFile(m_achFullName, achNextFileName) )
    {
		LogPrint(LOG_LVL_KEYSTATUS, MID_RECEQP_COMMON, "[PlayNextSwitchFile] non next switch file\n");
        return FALSE;
    }

    s32 nLen = max(strlen(achNextFileName), strlen(m_achFullName));
    nLen = nLen > MAX_FILE_NAME_LEN-1 ? MAX_FILE_NAME_LEN-1 : nLen;
    memcpy( m_achFullName, achNextFileName, nLen);

	u16 wAckEvent = REC_MCU_STARTPLAY_ACK;
    CServMsg cServMsg;

	u16 wErrCode = CheckFileCanPlay(achNextFileName);
	if (wErrCode != 0)
	{
		cServMsg.SetChnIndex(m_byChnIdx);
		cServMsg.SetErrorCode(wErrCode);
		cServMsg.SetMsgBody((u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp));
		SendMsgToMcu(REC_MCU_STARTPLAY_NACK, &cServMsg);
		return FALSE;
	}
    
    //处理后续
	cServMsg.SetChnIndex( m_byChnIdx );
	cServMsg.SetConfId(m_cConfId);
	cServMsg.SetMcuId(g_tCfg.m_wMcuId);
	cServMsg.SetSrcSsnId(m_byConfPlaySrcSsnId);
	cServMsg.SetEventId( 22531 );//MCS_MCU_STARTPLAY_REQ
	
    u8 byVideoNum = 0;
    if( MEDIA_TYPE_H261 <= m_tMediaInfo.m_abyMediaType[1] && 
        MEDIA_TYPE_H264 >= m_tMediaInfo.m_abyMediaType[1] )
    {
        byVideoNum++;
    }
	
    if( MEDIA_TYPE_H261 <= m_tMediaInfo.m_abyMediaType[2] && 
        MEDIA_TYPE_H264 >= m_tMediaInfo.m_abyMediaType[2] )
    {
        byVideoNum++;
    }
	
    LogPrint(LOG_LVL_DETAIL, MID_RECEQP_PLAY, "The video num in file: %d\n", byVideoNum);
    
    TPlayFileAttrib tPlayFileAttrib;
    
    if( VIDEO_TWO == byVideoNum ||
        MEDIA_TYPE_NULL != m_tMediaInfo.m_abyMediaType[2] )
    {
        tPlayFileAttrib.SetFileType(RECFILE_DSTREAM);
        tPlayFileAttrib.SetDVideoType(m_tMediaInfo.m_abyMediaType[2]);
        Reclog("Set double stream :%d\n", m_tMediaInfo.m_abyMediaType[2]);
    }
	
    tPlayFileAttrib.SetAudioType(m_tMediaInfo.m_abyMediaType[0]);
    tPlayFileAttrib.SetVideoType(m_tMediaInfo.m_abyMediaType[1]);
	
    TPlayFileMediaInfo tPlayFileMediaInfo;
    MediaInfoRec2Mcu(m_tMediaInfo, tPlayFileMediaInfo);
	
	//[liu lijiu][2010/11/08]增加放像机的起始端口信息
	TTransportAddr tPlayRtcpAddr;
	tPlayRtcpAddr.SetIpAddr(ntohl(g_tCfg.m_dwRecIpAddr));
	tPlayRtcpAddr.SetPort(g_tCfg.m_wPlayStartPort);

	Reclog( "[MsgStartPlayProc] player local start port: %d\n", tPlayRtcpAddr.GetPort());

	//zhouyiliang 20120224 录像机放像带上HP/BP信息
	TPlayFileHpAttrib tPlayFileHpAtAtrib;
	if (m_bFileFirstVidHp)
	{
		tPlayFileHpAtAtrib.SetFirstVideHpAttrib(emHpAttrb);
	}//无需else，TPlayFileHpAttrib构造函数默认就是bp
	
	if (m_bFileSecVidHp)
	{
		tPlayFileHpAtAtrib.SetSecVideHpAttrib(emHpAttrb);
	}//无需else，TPlayFileHpAttrib构造函数默认就是bp

	cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp) );
	cServMsg.CatMsgBody( (u8*)&tPlayFileAttrib, sizeof(tPlayFileAttrib));
    cServMsg.CatMsgBody( (u8*)&tPlayFileMediaInfo, sizeof(tPlayFileMediaInfo) );
	cServMsg.CatMsgBody( (u8*)&tPlayRtcpAddr, sizeof(TTransportAddr) );
	cServMsg.CatMsgBody((u8*)&tPlayFileHpAtAtrib,sizeof(TPlayFileHpAttrib));
	// [2013/3/21 zhangli]音频声道数
	cServMsg.CatMsgBody(&m_byFileAudioTrackNum, sizeof(u8));

	SendMsgToMcu( wAckEvent, &cServMsg);
	
    return TRUE;
}



/*=============================================================================
  函 数 名: GetNextSwitchFile
  功    能: 由当前文件名尝试获取下一个切换文件
  参    数: LPCSTR lpszCurFile
            s8 * pszNextFile
  注    意: 
  返 回 值:  切换后的文件名
-------------------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人      修改内容
  2008/01/11    4.0     张宝卿      创建
  2008/10/24    4.5     张宝卿      尝试获取跳过文件
=============================================================================*/
BOOL32 CChnInst::GetNextSwitchFile( LPCSTR lpszCurFile, s8 * pszNextFile )
{
    if ( NULL == lpszCurFile || NULL == pszNextFile )
    {
        OspPrintf(TRUE, FALSE, "[GetNextSwitchFile] param err: lpCur.0x%x, lpNext.0x%x\n", lpszCurFile, pszNextFile);
        return FALSE;
    }

    u8 byMaxGap = 5; //最大尝试5个后续文件(后续提供宏定义)
    u8 byCurGap = 0;

    u8 byFileOwnerGrp;
    u16 wFileBitrate;
    s8 achPatternName[MAX_FILE_NAME_LEN] = {0};

//     CString cStrCurPureName;
//     g_cRecApp.GetParamFromFilename(lpszCurFile, byFileOwnerGrp, wFileBitrate, cStrCurPureName);
// 
     CString cStrOriPureName;
     g_cRecApp.GetParamFromFilename(m_achOrignalFullName, byFileOwnerGrp, wFileBitrate, cStrOriPureName);

    for (; byCurGap <= byMaxGap; byCurGap ++)
    {
		
		CString cStrCurPureName;
		g_cRecApp.GetParamFromFilename(lpszCurFile, byFileOwnerGrp, wFileBitrate, cStrCurPureName);
		
        //第一次，切pattern，补1
        if ( 0 == cStrCurPureName.Compare(cStrOriPureName) )
        {
            g_cRecApp.PreProcPatternName(achPatternName, cStrCurPureName, byFileOwnerGrp, wFileBitrate );
            sprintf( pszNextFile, achPatternName, 1 );
        }
        else
        {
            s8 achCurStr[MAX_FILE_NAME_LEN];
            memset(achCurStr, 0, sizeof(achCurStr));
            memcpy(achCurStr, cStrCurPureName, cStrCurPureName.GetLength());
            achCurStr[MAX_FILE_NAME_LEN-1] = '\0';
            
            //最大支持9999个跳转文件的联播
            s8 achToken[5];
            memset(achToken, 0, sizeof(achToken));
            
            u8 byPos = strlen(achCurStr)-1;
            for( ; byPos > 0; byPos-- )
            {
                if ( 0 == strncmp("_", &achCurStr[byPos], sizeof(u8)) )
                {
                    break;
                }
            }
            if ( strlen(achCurStr)-byPos-1 > 4 )
            {
                OspPrintf( TRUE, FALSE, "[GetNextSwitchFile] SwitchFile.%s over play\n", &achCurStr[byPos+1] );
                return FALSE;    
            }
            else
            {				
                memcpy( achToken, &achCurStr[byPos+1], strlen(achCurStr)-byPos-1);
                achToken[4] = '\0';
            }
            
            s32 nIdx = atoi(achToken);
            nIdx ++;
            
            g_cRecApp.PreProcPatternName(achPatternName, cStrOriPureName, byFileOwnerGrp, wFileBitrate );
            sprintf( pszNextFile, achPatternName, nIdx );        
        }

        //文件有效性校验
        
        //判断获取的下个文件是否在使用
        BOOL32 bFileInUse = FALSE;
        for(s32 nLop = 1; nLop <= MAXNUM_RECORDER_CHNNL; nLop++)
        {
            s32 nLen = strlen(pszNextFile);
            
            if((nLen == 0)||(0 ==_strnicmp(g_cRecApp.m_achUsingFile[nLop], pszNextFile, nLen)))//该文件正在被操作
            {
                bFileInUse = TRUE;
            }
        }
        
        if ( bFileInUse )
        {
            Reclog("[GetNextSwitchFile] nextFile.%s has been in use, try again and left time(s).%d\n", pszNextFile, byMaxGap-byCurGap);
			memcpy((void*)lpszCurFile, pszNextFile, sizeof(m_achFullName));
            continue;
        }

        
        TMediaInfo tMediaInfo;
        memset(&tMediaInfo, 0, sizeof(TMediaInfo));
        u16 wRet = RPGetFileMediaInfo((u8)m_dwDevice, pszNextFile, &tMediaInfo);
        if(RP_OK != wRet)
        {
            Reclog("[GetNextSwitchFile] nextFile.%s unexist, try again and left time(s).%d\n", pszNextFile, byMaxGap-byCurGap);
			memcpy((void*)lpszCurFile, pszNextFile, sizeof(m_achFullName));
            continue;
        }

        break;
    }

    if ( byCurGap > byMaxGap )
    {
        return FALSE;
    }
    Reclog("[GetNextSwitchFile] nextFile.%s has been detected successfully!\n", pszNextFile);
    
    return TRUE;
}

/*====================================================================
    函数名	     ProcTimerNeedIFrame
	功能		 ：向MCU请求关键帧
	引用全局变量 ：无
    输入参数说明 ：无                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void  CChnInst::ProcTimerNeedIFrame( const CMessage *pMsg )
{
	// xliang [6/25/2009] 录像时开头请求3次关键帧，以便放像时，接收方错过第一个关键帧
	u8 byIFrameTimes  = (u8)(*((u32*)pMsg->content));	//请求关键帧次数
	BOOL32 bNeedIFrame = FALSE;

	if(/*byIFrameTimes <= 3 && */byIFrameTimes > 0)
	{
		bNeedIFrame = TRUE;
		byIFrameTimes--;
	}
	u8 abyNeedIframeStream[MAX_STREAM_NUM] = {0};
	TRecorderStatis tRecorderStatis;
	//录像开始,恢复时，三次请求关键帧要发
	if ( bNeedIFrame )
	{
		//主流，双流对应的index 1， 2 要发
		abyNeedIframeStream[1] = 1;
		abyNeedIframeStream[2] = 1;
	}
	//录像过程中丢帧导致的请求关键帧，哪路缺了，哪路发
	if (!bNeedIFrame 
		&& g_tCfg.m_bIsNeedKeyFrameWhenLoseFrame
		&& RP_OK == ::RPGetRecorderStatis(m_dwDevice, &tRecorderStatis))
	{
		if (tRecorderStatis.m_atMediaRecStatis[1].m_bWaitForKeyFrame)
		{
			abyNeedIframeStream[1] = 1;
			bNeedIFrame = TRUE;
		}

		if (tRecorderStatis.m_atMediaRecStatis[2].m_bWaitForKeyFrame)
		{
			abyNeedIframeStream[2] = 1;
			bNeedIFrame = TRUE;
		}

		if (tRecorderStatis.m_atMediaRecStatis[4].m_bWaitForKeyFrame)
		{
			abyNeedIframeStream[4] = 1;
			bNeedIFrame = TRUE;
		}
	}
	
	if (bNeedIFrame)
	{
		CServMsg cServMsg;
		cServMsg.SetConfId(m_cConfId);
		cServMsg.SetChnIndex(GetInsID()-1);
		cServMsg.SetSrcSsnId(g_cRecApp.m_tEqp.GetEqpId());
		cServMsg.SetMsgBody(abyNeedIframeStream,sizeof(abyNeedIframeStream));
		SendMsgToMcu(REC_MCU_NEEDIFRAME_CMD, &cServMsg);
		
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC, "REC channel:%d request iframe, IFrameTimes.%d!\n", GetInsID() - 1, byIFrameTimes);
	}
	
	//请求关键帧剩余次数大于0或者配置起了请求关键帧项才起定时器
	if (byIFrameTimes > 0 || g_tCfg.m_bIsNeedKeyFrameWhenLoseFrame)
	{
		SetTimer(EV_REC_MCU_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL, byIFrameTimes); //定时检测
	}	
}
/*====================================================================
    函数名	     ：SendRecStatusNotify
	功能		 ：录像机状态处理函数
	引用全局变量 ：无
    输入参数说明 ：pcApp - 应用指针                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::SendRecStatusNotify( CApp* pcApp)
{
	CServMsg cServMsg;
	s32 nChnnlIdx;
	CChnInst* pInst;
	TRecStatus tRecStatus;
	TPeriEqpStatus tEqpStatus;

	tEqpStatus.m_tStatus.tRecorder.SetPublicAttribute(g_tCfg.m_bOpenDBSucceed);
	tRecStatus.SetChnnlNum( g_byRecChnNum,g_byPlayChnNum );

	// 配置录像通道
	for( nChnnlIdx = 0 ; nChnnlIdx < g_byRecChnNum ; nChnnlIdx++ )
	{
        // 获得通道实例
		pInst = ( CChnInst* )pcApp->GetInstance( nChnnlIdx + 1 );
		tRecStatus.SetChnnlStatus( nChnnlIdx,
								   TRecChnnlStatus::TYPE_RECORD,
								   &pInst->m_tChnnlStatus );
	}
	

	// 放像通道
	for( nChnnlIdx = 0 ; nChnnlIdx < g_byPlayChnNum; nChnnlIdx ++ )
	{
        //获得播放通道实例
		pInst = (CChnInst*)pcApp->GetInstance( g_byRecChnNum + nChnnlIdx + 1 );
		tRecStatus.SetChnnlStatus( nChnnlIdx,
								   TRecChnnlStatus::TYPE_PLAY,
								   &pInst->m_tChnnlStatus );
	}
	
	s32 nFreeSpace;
	s32 nTotalSpace;
    BOOL32 bRet = GetSpaceInfo(nFreeSpace, nTotalSpace);
	tRecStatus.SetFreeSpaceSize( nFreeSpace );
	tRecStatus.SetTotalSpaceSize( nTotalSpace );
	tRecStatus.SetPublicAttribute(g_tCfg.m_bOpenDBSucceed);
    
    // 获取磁盘空间信息失败
    if ( !bRet )
    {
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "Get disc space failed !\n");
    }

	// 将录像机状态上报
	tEqpStatus.m_byOnline = 1;
	tEqpStatus.SetMcuEqp( (u8)g_tCfg.m_wMcuId,
		                  g_tCfg.m_byEqpId,
			    	      g_tCfg.m_byEqpType);
	
	tEqpStatus.m_tStatus.tRecorder = tRecStatus;
	tEqpStatus.SetAlias( g_tCfg.m_szAlias );
	 
	cServMsg.SetMsgBody((u8*)&tEqpStatus,sizeof(tEqpStatus));
	SendMsgToMcu( REC_MCU_RECSTATUS_NOTIF, &cServMsg );

    // 根据当前的磁盘状态，确定上报间隔
    BOOL32 bAbnormal = (nFreeSpace < DISCSPACE_LEVEL1);
    if (bAbnormal)
    {
        SetTimer(EV_REC_STATUSNOTYTIME, RECSTATUS_CRINOTIPERIOD);
    }
    else
    {
        SetTimer(EV_REC_STATUSNOTYTIME, RECSTATUS_NOTIPERIOD);
    }
    
    
    return;
}

/*=============================================================================
  函 数 名： GetSpaceInfo
  功    能： 获取磁盘空间
  算法实现： 
  全局变量： 
  参    数： s32& nFreeSpace：未使用空间
             s32& nTotalSpace：总空间
  返 回 值： void 
=============================================================================*/
BOOL32 CChnInst::GetSpaceInfo(s32& nFreeSpace, s32& nTotalSpace)
{
    LPCSTR pszDirve = g_tCfg.m_szRecordPath;
    s64 i64FreeBytesToCaller = 0;
    s64 i64TotalBytes = 0; 
    s64 i64FreeBytes = 0;

    s32 nRet = GetDiskFreeSpaceEx(pszDirve, (PULARGE_INTEGER)&i64FreeBytesToCaller, 
                                            (PULARGE_INTEGER)&i64TotalBytes,
                                            (PULARGE_INTEGER)&i64FreeBytes );
    if ( 0 == nRet )
    {
        nFreeSpace  = 0;
        nTotalSpace = 0;
        return FALSE;
    }
    else
    {
        nFreeSpace  = (s32)(i64FreeBytes / (1024*1024));
        nTotalSpace = (s32)(i64TotalBytes / (1024*1024));        
    }
    return TRUE;
}

/*====================================================================
    函数名	     ：SendRecProgNotify
	功能		 ：底层录像库通知消息函数
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::SendRecProgNotify(BOOL32 bSetTimer)
{
	TRecProg tRecProg;
	TRecorderStatus tRecorderStatus;
    TRecorderStatis tRecorderStatis;

    u16 wRet = RPGetRecorderStatis( m_dwDevice, &tRecorderStatis );
    if( RP_OK == wRet )
    {
	    if( RP_OK != RPGetRecorderStatus( m_dwDevice, &tRecorderStatus ))
	    {
		    LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "m_dwDevice: %d Call \"rpGetRecorderStatus\" failed.\n", m_dwDevice);
		    return;
	    }
	    else
	    {
		    tRecProg.SetCurProg( tRecorderStatis.m_dwRecTime );
		    m_tChnnlStatus.m_tProg = tRecProg; // 进度存入通道状态
		    
		    Reclog("Record Progress %d s.\n",m_tChnnlStatus.m_tProg.GetCurProg());
	    
	    }
    }
	CServMsg cServMsg;
	cServMsg.SetChnIndex( m_byChnIdx );
	cServMsg.SetConfId( m_cConfId );
	
	cServMsg.SetMsgBody( (u8*)&m_tSrvTmt,sizeof(m_tSrvTmt) );
	cServMsg.CatMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
	cServMsg.CatMsgBody( (u8*)&tRecProg,sizeof(tRecProg));
	
	SendMsgToMcu( REC_MCU_RECORDPROG_NOTIF, &cServMsg);
    
    if(bSetTimer)
    {
        SetTimer( EV_REC_RECPROGTIME, RECPROG_PERIOD); // 恢复定时通知录像进程
    }    
    return;
}

/*====================================================================
    函数名	     ：SendPlayProgNotify
	功能		 ：底层放像库通知消息函数
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
u8 CChnInst::SendPlayProgNotify()
{
	TRecProg tProg;
	TPlayerStatus tPlayerStatus;
    TPlayerStatis   tPlayerStatis;

    if( RP_OK == RPGetPlayerStatis( m_dwDevice, &tPlayerStatis) )
    {
        if( RP_OK != RPGetPlayerStatus( m_dwDevice, &tPlayerStatus))
	    {		    
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "Fail to call RPGetPlayerStatis\n");
            return 0;
	    }
	    else
	    {
		    tProg.SetCurProg( tPlayerStatis.m_dwPlyTime );
		    tProg.SetTotalTime( tPlayerStatis.m_dwTotalPlyTime );

		    m_tChnnlStatus.m_tProg = tProg;//进度存入通道状态

		    LogPrint(LOG_LVL_DETAIL, MID_RECEQP_PLAY, "Play Process %u/%u.\n",tPlayerStatis.m_dwPlyTime, 
			    m_tChnnlStatus.m_tProg.GetTotalTime());
            if(tPlayerStatis.m_dwPlyTime == tPlayerStatis.m_dwTotalPlyTime)
            {
			    // return FINISH_PLAY;
                // 退出由上面完成
            }
	    }
    }
    else
    {
		LogPrint(LOG_LVL_WARNING, MID_RECEQP_PLAY, "Fail to call RPGetPlayerStatis\n");
    }
	CServMsg cServMsg;
	
	cServMsg.SetChnIndex( m_byChnIdx);
	cServMsg.SetConfId( m_cConfId );

	cServMsg.SetMsgBody( (u8*)&g_cRecApp.m_tEqp,sizeof(g_cRecApp.m_tEqp) );
	cServMsg.CatMsgBody( (u8*)&tProg,sizeof(tProg) );
	
	SendMsgToMcu( REC_MCU_PLAYPROG_NOTIF,&cServMsg );
	return 0;
	
}

/*====================================================================
    函数名	     ：MsgRefreshCurChnRecFileName
	功能		 ：录像文件跳转后通知刷新当前通道的录像文件名
	引用全局变量 ：无
    输入参数说明 ：
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2006/11/15  4.0         张宝卿          创建
====================================================================*/
void CChnInst::MsgRefreshCurChnRecFileName(CMessage * const pMsg)
{
	// 删除广播发布点、取消发布条目,必须在更新m_achFileName之前操作
//	if (PUBLISH_MODE_IMMED == m_byPublishMode)
//	{
//		DeleteBrdPubPoint();
//		DeleteOneLiveSource();
//	}

	//大文件切换，ADDVIDFile通知
	SendAddVidFileNotify();

    CServMsg cServMsg( pMsg->content, pMsg->length );
    s8 * pszNewFileName = (s8*)cServMsg.GetMsgBody();

    s8  achOldFullName[MAX_FILE_NAME_LEN]  = {0};
    strcpy( achOldFullName, m_achFullName );
    
	TFileInfo tFileInfo;
	if (m_dwDevice <= MAXNUM_RECORDER_CHNNL && RP_OK == ::RPGetFileInfo(MAXNUM_RECORDER_CHNNL, achOldFullName, &tFileInfo))
	{
		g_adwRecDuration[m_dwDevice] += tFileInfo.m_dwDuration;
	}

	strncpy(m_achFileName, pszNewFileName, cServMsg.GetMsgBodyLen());
	m_achFileName[strlen(pszNewFileName)] = 0;

    m_tChnnlStatus.SetRecordName(m_achFileName);
 
	u16 wFullNameSize;    //add by jlb 081026
    wFullNameSize = g_cRecApp.PreProcFileame(m_achFullName, m_achFileName, NULL); 
    if (0 == wFullNameSize)
    { 
		Reclog("[REC][MsgRefreshCurChnRecFileName]Failed to refresh current rec  %s! because file name too long.\n", m_achFileName);
		return;
    }
    //更新占用的文件
    strcpy(g_cRecApp.m_achUsingFile[GetInsID()], m_achFullName);

    Reclog( "RecChn.%d record file name change to: %s !\n", GetInsID(), m_achFileName );

	// [pengjie 2010/2/5] Modify 录像文件改变，需要在这里请求关键
	SetTimer(EV_REC_MCU_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL, 3);
	// End

	post(MAKEIID(GetAppID(), CInstance::DAEMON), EV_REC_STATUSNOTYTIME, NULL, 0, 0);
    
    //原始文件不发布
    if (PUBLISH_MODE_NONE == m_byPublishMode)
    {
        return;
    }
    
	// [9/29/2010 liuxu][添加] 如果数据库没有连接上,则不能发布
	if(!g_tCfg.m_bOpenDBSucceed)
	{
		return;
	}
	// [9/29/2010 liuxu][添加结束]

	//发布上个文件
	if (m_byPublishMode != PUBLISH_MODE_NONE)
	{
		PublishOneFile(achOldFullName);
	}
	
	if (PUBLISH_MODE_IMMED == m_byPublishMode)
	{
		AddToPlayList();
//		//创建切文件之后的发布点
//		CreateBrdPubPoint();	
//		//添加新的直播
//		AddOneLiveSource();
	}
}

/*====================================================================
    函数名	     ：StatusShow
	功能		 ：录像机当前状态函数
	引用全局变量 ：无
    输入参数说明 ：pcApp - 应用指针
                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::StatusShow( CApp* const pcApp)
{
	s32 nLop;
	TRecChnnlStatus tChnStatus;
	s8 achConfId[40];
	CChnInst* pInst;

	::OspPrintf( TRUE,FALSE,"\n================ Recorder Status ========================\n");
	::OspPrintf( TRUE,FALSE,"= IDLE rREADY rING rPAUSE pREADY pREADYPLAY pING pPAUSE pFF pFB    =\n");
	::OspPrintf( TRUE,FALSE,"= %-4d %-6d %-4d %-6d %-6d %-10d %-4d %-6d %-3d %-3d    =\n",
    	TRecChnnlStatus::STATE_IDLE,
		TRecChnnlStatus::STATE_RECREADY, 
		TRecChnnlStatus::STATE_RECORDING,
		TRecChnnlStatus::STATE_RECPAUSE,
		TRecChnnlStatus::STATE_PLAYREADY,
		TRecChnnlStatus::STATE_PLAYREADYPLAY,
		TRecChnnlStatus::STATE_PLAYING,
		TRecChnnlStatus::STATE_PLAYPAUSE,
		TRecChnnlStatus::STATE_FF,
		TRecChnnlStatus::STATE_FB );

	for( nLop = 0; nLop< (g_byRecChnNum); nLop++)
	{
		pInst = (CChnInst*)pcApp->GetInstance( nLop+1 );
		pInst->m_cConfId.GetConfIdString( achConfId,sizeof(achConfId) );
		tChnStatus = pInst->m_tChnnlStatus;
		
		::OspPrintf( TRUE,FALSE,"-\n Record Channel.%d  STATE= %d Progress: %d\n", nLop
			            ,tChnStatus.m_byState,tChnStatus.m_tProg.GetCurProg() );
		::OspPrintf( TRUE,FALSE," Serving ConfId   : %s\n",achConfId);
		::OspPrintf( TRUE,FALSE," Chnl Record Name : %s\n",tChnStatus.GetRecordName() );
        ::OspPrintf( TRUE,FALSE," Record FileName  : %s\n",pInst->m_achFileName );
        ::OspPrintf( TRUE,FALSE," Record Full Name : %s\n",pInst->m_achFullName );
		::OspPrintf( TRUE,FALSE," Record Mode      : %s\n",
			tChnStatus.m_byRecMode == REC_MODE_NORMAL ? "Real time":"Skip Frame");
        ::OspPrintf( TRUE,FALSE," Public Mode      : %d(Lvl.%d)\n",
            pInst->m_byPublishMode, pInst->m_byPublishLevel);
	}

	for( nLop=0; nLop< g_byPlayChnNum; nLop++)
	{
		pInst = (CChnInst*)pcApp->GetInstance( g_byRecChnNum + nLop+1 );
		pInst->m_cConfId.GetConfIdString( achConfId,sizeof(achConfId) );
		tChnStatus = pInst->m_tChnnlStatus;

		::OspPrintf( TRUE,FALSE,"-\n Play Channel.%d STATE= %d Progress %d/%d\n", nLop,
			tChnStatus.m_byState,tChnStatus.m_tProg.GetCurProg(),tChnStatus.m_tProg.GetTotalTime());
		::OspPrintf( TRUE,FALSE," Serving ConfId : %s\n",achConfId);
		::OspPrintf( TRUE,FALSE," Play Name      : %s\n",tChnStatus.GetRecordName() );
        ::OspPrintf( TRUE,FALSE," Play FileName  : %s\n",pInst->m_achFileName );
        ::OspPrintf( TRUE,FALSE," Play FullName  : %s\n",pInst->m_achFullName );
	}
    return;
}

/*====================================================================
    函数名	     ：MsgCppUnitGetEqpStatus
	功能		 ：测试代理函数，获得录像机外设状态
	引用全局变量 ：无
    输入参数说明 ：pMsg  - 消息指针
                   pcApp - 应用指针
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CChnInst::MsgCppUnitGetEqpStatus(CMessage* const pMsg,CApp* pcApp)
{
	s32 nLop;
	CChnInst      *pInst;
	TPeriEqpStatus tEqpStatus;
	
	tEqpStatus.m_tStatus.tRecorder.SetPublicAttribute(g_tCfg.m_bOpenDBSucceed);
	tEqpStatus.m_byOnline	= 1;
	tEqpStatus.SetMcuEqp( (u8)g_tCfg.m_wMcuId,
		                  g_tCfg.m_byEqpId,
						  g_tCfg.m_byEqpType);

	CServMsg   cServMsg(pMsg->content,pMsg->length);

	tEqpStatus.m_tStatus.tRecorder.SetChnnlNum( g_byRecChnNum,
												g_byPlayChnNum );
	for(nLop=0 ;nLop< ( g_byRecChnNum )  ;nLop++)
	{
		pInst     = (CChnInst*)pcApp->GetInstance(nLop+1);

		tEqpStatus.m_tStatus.tRecorder.SetChnnlStatus( nLop,
								  pInst->m_tChnnlStatus.m_byType,
								  &pInst->m_tChnnlStatus );
	}

	for(nLop=0; nLop< ( g_byPlayChnNum ); nLop++)
	{
		pInst = (CChnInst*)pcApp->GetInstance( g_byRecChnNum + nLop + 1);

		tEqpStatus.m_tStatus.tRecorder.SetChnnlStatus( nLop,
								  pInst->m_tChnnlStatus.m_byType,
								  &pInst->m_tChnnlStatus );
	}

	SetSyncAck( &tEqpStatus,sizeof(tEqpStatus) );
    return;
}


/*====================================================================
    函数名	     ：InitPlayList
	功能		 ：初始化播放列表
	引用全局变量 ：无
    输入参数说明 ：
                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/25	3.6			liuhuiyun	  Create
====================================================================
void CChnInst::InitPlayList()
{
	s32 nSum = -1;			// the number of files in the PlayList
	s8  achTmpName[66]={0};

	u16 wIdx = 0;
	s32 nSize = -1;

	g_cRecApp.m_tRecNotify.Reset();	// important
    s8 aszRecordePath[64];
    memset(aszRecordePath, '\0', sizeof(aszRecordePath) );
    sprintf(aszRecordePath, "%s/", g_tCfg.m_szRecordPath);
	// if( RP_OK == ::rpListTapes( g_tCfg.m_szRecordPath, g_cRecApp.m_tTapeInfoList ) )
    if( RP_OK == RPListFile( aszRecordePath, &g_cRecApp.m_tTapeInfoList) )
	{
#if 0		
		if(m_bLookMsg == TRUE)
		{
			log(LOGLVL_EXCEPTION, "[Rec] Get the PlayList Success\n");
		}
#endif
		nSum = g_cRecApp.m_tTapeInfoList.m_dwNum;

		// 文件名按时间排序
		// IndexTapeList( g_cRecApp.m_tTapeInfoList ); // *********need to change

		if(nSum > 0)
		{
			g_cRecApp.m_tRecNotify.SetListSize(nSum);

			for( s32 nLp =0; nLp< nSum; nLp++)
			{
				nSize = strlen(g_cRecApp.m_tTapeInfoList.m_astrFileName[nLp]);
				
				if(nSize >64)
				{
					nSize = 64;//记录名截断
				}

				memcpy(achTmpName, g_cRecApp.m_tTapeInfoList.m_astrFileName[nLp], nSize);
				achTmpName[nSize] = '\0';

				//检测该文件是否已发布
				BOOL bPublic  = FALSE;
				bPublic = IsPublicFile( achTmpName );
				
				if( g_cRecApp.m_tRecNotify .IsFull() ) 
				{
					g_cRecApp.m_tRecNotify.SetEndIdx( wIdx );
					break;		// 记录缓冲区已经满，退出
				}
				else
				{
					g_cRecApp.m_tRecNotify.SetFileName( wIdx, achTmpName, bPublic );  // 追加的方式
					wIdx++;
					g_cRecApp.m_tRecNotify.SetEndIdx(wIdx);  // 必须要，因为在SetFileName中要用到EndIdx
				}

			}

		}
	}
	else
	{
		log(LOGLVL_EXCEPTION, "[Rec] Get the PlayList Fail(In InitPlayList)\n");
	}

    return;
}


/*====================================================================
    函数名	     ：ComparePlayList
	功能		 ：比较列表信息
	引用全局变量 ：无
    输入参数说明 ：tTapeInfoList
                   
	返回值说明   ：TRUE: 两个播放列表不同 FALSE: 两个播放列表相同
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/25	3.6			liuhuiyun	  Create
====================================================================
BOOL CChnInst::ComparePlayList(TListInfo tTapeInfoList )
{
	s32 nSrcFileSum = g_cRecApp.m_tTapeInfoList.m_dwNum;
	s32 nDstFileSum = tTapeInfoList.m_dwNum;
	
	s32 nLoop, nLoop1;
	s8 achTmpNameSrc[64] = {0};
	s8 achTmpNameDst[64] = {0};
	
	s32 nFlag = -1;

	if(m_bLookMsg == TRUE)
	{
		log(LOGLVL_EXCEPTION, "[Rec] Compare the PlayList(In ComparePlayList)\n");

	}
	if( nSrcFileSum != nDstFileSum)
    {
        return TRUE;    // 两个播放列表不同
    }

	// 比较两个播放列表
	for(nLoop = 0; nLoop < nSrcFileSum; nLoop++)
	{
		memset(achTmpNameSrc, '\0', 64);	// 置空
		
		sprintf(achTmpNameSrc, "%s", g_cRecApp.m_tTapeInfoList.m_astrFileName[nLoop]);

		for(nLoop1 = 0; nLoop1 < nDstFileSum; nLoop1++)
		{
			
			memset(achTmpNameDst, '\0', 64);	// 置空

			sprintf(achTmpNameDst, "%s", tTapeInfoList.m_astrFileName[nLoop1]);
			nFlag = strcmp(achTmpNameSrc, achTmpNameDst);
			if( nFlag == 0)
			{
				break;
			}
			
		}
		if( 0 != nFlag )
		{
			if(m_bLookMsg == TRUE)
				log(LOGLVL_EXCEPTION, "[Rec] Exist the different Files in the PlayList(In ComparePlayList)\n");
			return TRUE;
		}
	}

	if(m_bLookMsg == TRUE)
	{
		log(LOGLVL_EXCEPTION, "[Rec] No Exist the different Files in the PlayList\n");
	}

	return FALSE;
}

/*====================================================================
    函数名	     ：MaintenanceList
	功能		 ：维护列表
	引用全局变量 ：无
    输入参数说明 ：tRecNotify
                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/25	3.6			liuhuiyun	  Create
====================================================================
void CChnInst::MaintenanceList(TRecFileListNotify tRecNotify )
{
	s32 nSrcSum = ntohs(g_cRecApp.m_tRecNotify.wListSize);
	
	s32 nSum = ntohs(tRecNotify.wListSize);
	
	s32 nLp = 0;
	s32 nLoop = 0;

	u32 wRet;
	u8  uPublic;
	s32 nFlag = -1; 
	
	if(m_bLookMsg == TRUE)
	{
		log(LOGLVL_EXCEPTION, "[Rec] Maintenanec the PlayList\n");
	}

	// Search the different in the RecNotify
	for(nLoop = 0; nLoop < nSrcSum; nLoop++)
	{
		for(nLp = 0; nLp < nSum; nLp++)
		{

			nFlag = strcmp(g_cRecApp.m_tRecNotify.achFileName[nLoop], tRecNotify.achFileName[nLp]);
			if(nFlag == 0)
			{
				g_cRecApp.m_tRecNotify.abyStatus[nLoop] = tRecNotify.abyStatus[nLp]; // 是否发布项
				break;
			}

		}
		if(nFlag != 0)
		{
			uPublic = g_cRecApp.m_tRecNotify.abyStatus[nLoop];
			if(uPublic == 1)  // publiced
			{
				s8 achFullname[128];

				memset(achFullname, '\0', 128);	// 置空
				sprintf( achFullname, "%s.asf", g_cRecApp.m_tRecNotify.achFileName[nLoop]);

				// wRet = ::rpCancelOnePublishedFile(g_cRecApp.m_achPublishPath, sFullname);
                wRet = g_cTDBOperate.CancelOnePublishedFile(g_cRecApp.m_achPublishPath, achFullname);
				if(VOD_OK != wRet)
				{
					if(m_bLookMsg == TRUE)
						log(LOGLVL_EXCEPTION, " [Rec] Cancel Public File:%s Fail(In MaintenanceList)\n", achFullname);
				}
				else
				{
					if(m_bLookMsg == TRUE)
						log(LOGLVL_EXCEPTION, " [Rec] Cancel Public File:%s Success(In MaintenanceList)\n", achFullname);
				}
			}				
		}
	}
    return;
}
/*====================================================================
    函数名	     ：MaintenanceList
	功能		 ：维护列表消息处理
	引用全局变量 ：无
    输入参数说明 ：
                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/25	3.6			liuhuiyun	  Create
====================================================================
void CChnInst::MsgMaintainPlayList()
{
	s8 achList[4096]={0};
	s8 achName[66]={0};
	s8 achTmpName[66]={0};
	s32  nLp1, nSum, nSize ;

	TListInfo tTapeInfoList;

	TRecFileListNotify tRecNotify;			
	u16 wIdx=0;
	tRecNotify.Reset();
	
	nSum = g_cRecApp.m_tTapeInfoList.m_dwNum;

	// 原来的列表为空,则初始化
	if(nSum == 0)
	{
		InitPlayList();
		return;
	}

	nSum = 0;
    s8 aszRecordePath[64];
    memset(aszRecordePath, '\0', sizeof(aszRecordePath) );
    sprintf(aszRecordePath, "%s", g_tCfg.m_szRecordPath);
	// if( RP_OK == ::rpListTapes( g_tCfg.m_szRecordPath, tTapeInfoList ) )
   if( RP_OK == RPListFile( aszRecordePath, &tTapeInfoList) )
	{
		
		nSum = tTapeInfoList.m_dwNum;  // 取得文件数目

		// 对文件名按时间排序	
		// IndexTapeList(tTapeInfoList);
		
		BOOL bPublic = FALSE;
		
		if( nSum > 0 )
		{
			
			tRecNotify.SetListSize(nSum);
			// 取目前的播放列表状态
			for( nLp1 =0; nLp1< nSum; nLp1++)
			{
				nSize = strlen(tTapeInfoList.m_astrFileName[nLp1]);
				if(nSize >64)
				{
					nSize = 64;   //记录名截断
				}

				memset(achTmpName, '\0', 66);	// 置空
				memcpy(achTmpName, tTapeInfoList.m_astrFileName[nLp1], nSize);
				achTmpName[nSize] = '\0';

				//检测该文件是否已发布
				bPublic = IsPublicFile( achTmpName );
				Sleep(10);

				if( tRecNotify .IsFull() ) 
				{
					tRecNotify.SetEndIdx( wIdx );
					break;		// 记录缓冲区已经满，退出
				}
				else
				{
					tRecNotify.SetFileName( wIdx, achTmpName, bPublic );
					wIdx++;
					tRecNotify.SetEndIdx(wIdx);  // 必须要，因为在SetFileName中要用到俄EndIdx
				}
			}

			BOOL bFlag = ComparePlayList(tTapeInfoList);
			if(TRUE == bFlag)	// 两个播放列表存在不同
			{
				MaintenanceList(tRecNotify);	// 维护列表
				InitPlayList();					// 更新播放列表
			}
			else	// 相同								
			{
				InitPlayList();					// 更新播放列表
			}
			
		}
		else	// 当前列表为空
		{
			CancelAllPublicFile();				// 取消播放列表中所有已发布文件
			InitPlayList();						// 更新播放列表
		}
	}
	else
	{
		if( m_bLookMsg == TRUE)			
		{
			log( LOGLVL_EXCEPTION," [Rec] call \"rpListTapes\" failed.(In MaintainPlayList)\n" ); 
		}
	}
	
//	if(m_bLookMsg == TRUE)
//	{
//		PrintPlayList();	// 打印播放列表 
//	}

}
*/

/*====================================================================
    函数名	     ：IndexTapeList
	功能		 ：按时间对文件排序
	引用全局变量 ：无
    输入参数说明 ：tTapeInfoList
                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/25	3.6			liuhuiyun	  Create
====================================================================
void CChnInst::IndexTapeList(TListInfo& tTapeInfoList)
{
	s32 nLp1;
	s32 nSum;

	nSum = tTapeInfoList.m_dwNum;

	if(nSum == 0)
	{
		if(m_bLookMsg == TRUE)
		{
			log(LOGLVL_EXCEPTION, "[Rec] The TapeList is empty\n");
		}

		return;
	}
	for( nLp1=1; nLp1 < nSum; nLp1++)
	{
//		for( nLp2=0; nLp2 < nSum-nLp1; nLp2++)
//		{
//			if( tTapeInfoList.m_tTapeIfo[nLp2].m_ttFileTime < tTapeInfoList.m_tTapeIfo[nLp2+1].m_ttFileTime)
//			{
//				s8 achTmpStr[MAX_FILE_NAME_LEN];
//                memset(achTmpStr, '\0', sizeof(achTmpStr));
//                
//				strncpy( achTmpStr, tTapeInfoList.m_astrFileName[nLp2], sizeof(tTapeInfoList.m_astrFileName[nLp2]));
//
//				strncpy(tTapeInfoList.m_astrFileName[nLp2], tTapeInfoList.m_astrFileName[nLp2+1],
//                                                       sizeof(tTapeInfoList.m_astrFileName[nLp2]));
//
//                strncpy(tTapeInfoList.m_astrFileName[nLp2+1], achTmpStr, sizeof(achTmpStr));
//			}
//		}
	} 

	return;
}
*/

/*====================================================================
    函数名	     ：IsPublicFile
	功能		 ：查询指定文件是否发布
	引用全局变量 ：无
    输入参数说明 ：achFileName 这里的文件名是带路径的，如 mcuName/Conf1/a.asf
                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/25	3.6			liuhuiyun	  Create
====================================================================*/
BOOL CChnInst::IsPublicFile(s8* achFileName)
{
	BOOL bPublic = FALSE;
	if( g_tCfg.m_bOpenDBSucceed )
	{
        if( VOD_OK != g_cTDBOperate.QueryOneFileStauts(g_cRecApp.m_achPublishPath, achFileName, bPublic))
		{
			bPublic = FALSE;
		}
	}
	return bPublic;
}

/*====================================================================
    函数名	     ：DelFileTapeList
	功能		 ：在播放列表中删除指定文件
	引用全局变量 ：无
    输入参数说明 ：DelFileName
                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/25	3.6			liuhuiyun	  Create
====================================================================
void CChnInst::DelFileTapeList(s8* DelFileName )
{
	s32 nLop = 0;
	s32 nSum = g_cRecApp.m_tTapeInfoList.m_dwNum;
	s32 nLen = -1;
	
	if( m_bLookMsg == TRUE )
	{
		log(LOGLVL_EXCEPTION, " [Rec] Delete File in the TapeList\n");
	}
	
	if( DelFileName == NULL)
	{
		log(LOGLVL_EXCEPTION, " [Rec] Error File Name (In DelFileTapeList)\n");
		return;
	}
	
	s8 achDstFileName[KDV_MAX_PATH] = {0};
	strncpy(achDstFileName, DelFileName, strlen(DelFileName));
	
	s8 achSrcFileName[KDV_MAX_PATH] = {0};

	s32 nFlag = -1;
	for(nLop = 0; nLop < nSum; nLop++)
	{
		nLen = strlen(g_cRecApp.m_tTapeInfoList.m_astrFileName[nLop]);
		strncpy(achSrcFileName, g_cRecApp.m_tTapeInfoList.m_astrFileName[nLop], nLen);

		nFlag = strcmp(achDstFileName, achSrcFileName);
		if(nFlag == 0)
		{	
			// 删除列表中的文件
//			g_cRecApp.m_tTapeInfoList.m_tTapeIfo[nLop].m_dwLength = 0;
//			g_cRecApp.m_tTapeInfoList.m_tTapeIfo[nLop].m_StreamNumber = 0;		
//			g_cRecApp.m_tTapeInfoList.m_tTapeIfo[nLop].m_ttFileTime = 0;
			//memset(g_cRecApp.m_tTapeInfoList.m_tTapeIfo[nLop], 1, '\0');

            memset(g_cRecApp.m_tTapeInfoList.m_astrFileName[nLop], '\0', MAX_FILE_NAME_LEN);
			g_cRecApp.m_tTapeInfoList.m_dwNum = g_cRecApp.m_tTapeInfoList.m_dwNum - 1; // 文件数减1
		
			break;
		}
		
	}

	return;	
}


/*====================================================================
    函数名	     ：CancelAllPublicFile
	功能		 ：取消播放列表中所有以发布文件
	引用全局变量 ：无
    输入参数说明 ：
                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/25	3.6			liuhuiyun	  Create
====================================================================
void CChnInst::CancelAllPublicFile()
{
	s32 nSum = ntohs(g_cRecApp.m_tRecNotify.wListSize);
	s32 nLp = 0;
	s32 nSize = 0;
	
	s32 wRet = 0;

	s8 achTmpFile[KDV_MAX_PATH] = {'\0'};
	BOOL bPublic = FALSE;

	for(nLp = 0; nLp < nSum; nLp++)
	{
		memset(achTmpFile, '\0', KDV_MAX_PATH);

		if(g_cRecApp.m_tRecNotify.abyStatus[nLp] == 1) // public
		{
			nSize = strlen(g_cRecApp.m_tRecNotify.achFileName[nLp]);
			strncpy(achTmpFile, g_cRecApp.m_tRecNotify.achFileName[nLp], nSize);
			
			sprintf(achTmpFile, "%s.asf", achTmpFile);

			// wRet = ::rpCancelOnePublishedFile(g_cRecApp.m_achPublishPath, achTmpFile);
            wRet = g_cTDBOperate.CancelOnePublishedFile(g_cRecApp.m_achPublishPath, achTmpFile);
			if(VOD_OK != wRet)
			{
				log(LOGLVL_EXCEPTION, "[Rec] Cancel Public File:%s Fail(In CancelAllPublic)\n", achTmpFile);
			}
			else
			{
				log(LOGLVL_EXCEPTION, "[Rec] Cancel Public File:%s Success(In CancelAllPublic)\n", achTmpFile);
			}
			Sleep(100);	// 过于频繁的访问数据库，系统可能无法响应
		}
	
	}
	return;
}

/*====================================================================
    函数名	     ：PrintPlayList
	功能		 ：打印播放列表
	引用全局变量 ：无
    输入参数说明 ：
                   
	返回值说明   ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/25	3.6			liuhuiyun	  Create
====================================================================
void CChnInst::PrintPlayList()
{
	s32 nSum = ntohs(g_cRecApp.m_tRecNotify.wListSize);
	s8  achTmpName[64] = {0};

	OspPrintf(TRUE, FALSE, "\tThe Play List (0: not Publiced 1: publiced)\n");

	for(s32 nLp = 0; nLp < nSum; nLp++)
	{
		memset(achTmpName, '\0', 64);	// 置空
		sprintf(achTmpName, "%s", g_cRecApp.m_tRecNotify.achFileName[nLp]);
		OspPrintf(TRUE, FALSE, "The No %d  File:%s \t%d\n", nLp+1, achTmpName, g_cRecApp.m_tRecNotify.abyStatus[nLp]);

	}
	return;
}
*/

/*=============================================================================
  函 数 名： ProcSetQosInfo
  功    能： 设置Qos值
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
=============================================================================*/
void CChnInst::ProcSetQosInfo(CMessage* const pMsg)
{
    if(NULL == pMsg)
    {
        OspPrintf(TRUE, FALSE, "[Rec] The Rec Qos infomation is Null\n");
        return;
    }
    TMcuQosCfgInfo  tQosInfo;
    CServMsg cServMsg(pMsg->content, pMsg->length);
    tQosInfo = *(TMcuQosCfgInfo*)cServMsg.GetMsgBody();

    u8 byQosType    = tQosInfo.GetQosType();
    u8 byAudioValue = tQosInfo.GetAudLevel();
    u8 byVideoValue = tQosInfo.GetVidLevel();
    u8 byDataValue  = tQosInfo.GetDataLevel();
    u8 byIpPriorValue = tQosInfo.GetIpServiceType();
    if(QOSTYPE_IP_PRIORITY == byQosType)
    {
        byAudioValue = (byAudioValue << 5);
        byVideoValue = (byVideoValue << 5);
        byDataValue  = (byDataValue  << 5);
        ComplexQos(byAudioValue, byIpPriorValue);
        ComplexQos(byVideoValue, byIpPriorValue);
        ComplexQos(byDataValue,  byIpPriorValue);
    }
    else
    {
        byAudioValue = (byAudioValue << 2);
        byVideoValue = (byVideoValue << 2);
        byDataValue  = (byDataValue  << 2);
    }
    s32 nRet = RP_OK;
    nRet = SetRPMediaTOS((s32)byAudioValue, QOS_AUDIO);
    if ( RP_OK != nRet )
    {
        Reclog( "[ProcSetQosInfo] set qos type: %d failed !\n", QOS_AUDIO );
    }
	nRet = SetRPMediaTOS((s32)byVideoValue, QOS_VIDEO);
    if ( RP_OK != nRet )
    {
        Reclog( "[ProcSetQosInfo] set qos type: %d failed !\n", QOS_VIDEO );
    }
	nRet = SetRPMediaTOS((s32)byDataValue, QOS_DATA);
    if ( RP_OK != nRet )
    {
        Reclog( "[ProcSetQosInfo] set qos type: %d failed !\n", QOS_DATA );
    }

    return;
}

/*=============================================================================
  函 数 名： ComplexQos
  功    能： 求复合Qos值
  算法实现： 
  全局变量： 
  参    数： u8& byValue
             u8 byPrior
  返 回 值： void 
=============================================================================*/
void CChnInst::ComplexQos(u8& byValue, u8 byPrior)
{
    u8 byLBit = ((byPrior & 0x08) >> 3);
    u8 byRBit = ((byPrior & 0x04) >> 1);
    u8 byTBit = ((byPrior & 0x02) << 1);
    u8 byDBit = ((byPrior & 0x01) << 3);
    
    byValue = byValue + ((byDBit + byTBit + byRBit + byLBit)*2);
    return;
}

/*=============================================================================
  函 数 名： GetRPBitRate
  功    能： 会议码率转成rp库的码率
  算法实现： 
  全局变量： 
  参    数： u16 wConfBitRate
  返 回 值： u32 
=============================================================================*/
u32 CChnInst::GetRPBitRate( u16 wConfBitRate, u16 wPayLoadType )
{
    u32 dwRPBitRate = 0;

    switch(wPayLoadType) 
    {
    case MEDIA_TYPE_MP4:
    case MEDIA_TYPE_H262:
    case MEDIA_TYPE_H261:
    case MEDIA_TYPE_H263:
    case MEDIA_TYPE_H263PLUS:
    case MEDIA_TYPE_H264:
        dwRPBitRate = wConfBitRate * 1024;
    	break;
    default:
        OspPrintf( TRUE, FALSE, "[GetRPBitRate] unexpected media type<%d> failed !\n", wPayLoadType );
        break;
    }
    return dwRPBitRate;
}

/*=============================================================================
函 数 名： ReleaseChannel
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/23   4.0			周广程                  创建
=============================================================================*/
void CChnInst::ReleaseChannel()
{	
	u16 wInstId = GetInsID();
	m_tChnnlStatus.m_tProg.SetCurProg( 0 );
	m_tChnnlStatus.m_tProg.SetTotalTime( 0 );
	m_tChnnlStatus.SetRecordName( "" );
	m_tChnnlStatus.m_tSrc.SetNull();
	memset(g_cRecApp.m_achUsingFile[wInstId], 0, MAX_FILE_NAME_LEN );
	memset(m_achFileName, 0x0, sizeof(m_achFileName));
	memset(m_achFullName, 0x0, sizeof(m_achFullName));
	memset( m_achOrignalFullName, 0x0, sizeof(m_achOrignalFullName));

    // zbq [10/16/2007] FEC 支持
    m_byVideoNum = 0;
    m_tVideoEncrypt.Reset();
    m_tAudioEncrypt.Reset();
    memset( &m_tMediaInfo, 0 ,sizeof(TMediaInfo) );
    memset( &m_tCapSupportEx, 0, sizeof(m_tCapSupportEx));

	if( wInstId <= g_byRecChnNum )
	{
		m_tChnnlStatus.m_byState  = TRecChnnlStatus::STATE_RECREADY ;
		NEXTSTATE( TRecChnnlStatus::STATE_RECREADY );
	}
	else if( ( wInstId <= (g_byRecChnNum + g_byPlayChnNum ) )
		     && ( wInstId > g_byRecChnNum ) )
	{    
		m_tChnnlStatus.m_byState  = TRecChnnlStatus::STATE_PLAYREADY ;
		NEXTSTATE( TRecChnnlStatus::STATE_PLAYREADY );
	}
    return;
}

CRecCfg::CRecCfg()
{
    m_wRegTimes   = 0;
    m_wDevCount   = 0;
    m_byRegAckNum = 0;
	m_dwMpcSSrc   = 0;
    //m_bySetTimer  = 0;
    m_dwMcuStartRcvPort = 0;

    m_dwMcuNode   = INVALID_NODE;
    m_dwMcuIId    = INVALID_INS;
    m_dwMcuNodeB  = INVALID_NODE;
    m_dwMcuIIdB   = INVALID_INS;    

    memset( &m_tEqp, 0, sizeof(TMt) );
    memset( &m_PrsParam, 0, sizeof(m_PrsParam) );
    
    memset( m_achUsingFile,  0, sizeof(m_achUsingFile) );
    memset( m_achPublishPath, 0, sizeof(m_achPublishPath) );
    memset( m_szMcuAlias, 0, sizeof(m_szMcuAlias));
    memset( m_szWorkPath, 0, sizeof(m_szWorkPath));

}

CRecCfg::~CRecCfg()
{
}

// 返回工作路径
LPCSTR CRecCfg::GetWorkingPath() 
{ 
    return m_szWorkPath; 
}

/*====================================================================
函数名	     ：FileTimeToTime_t
功能		 ：FILETIME格式的时间转换成TIME_T格式
引用全局变量 ：无
输入参数说明 ：FILETIME cFileTime,FILETIME格式的文件创建时间
输出参数说明 ：time_t *pcTime,TIME_T格式的时间
返回值说明   ：无
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
13/08/27		        党剑锋        创建
====================================================================*/
void CRecCfg::FileTimeToTime_t(FILETIME cFileTime, time_t *pcTime) 
{ 
	LONGLONG cLongLong; 
	ULARGE_INTEGER cUlarge_Integer; 
	
	cUlarge_Integer.LowPart = cFileTime.dwLowDateTime; 
	cUlarge_Integer.HighPart = cFileTime.dwHighDateTime; 
	
	cLongLong = (cFileTime.dwHighDateTime << 32) + cFileTime.dwLowDateTime; 
	
	*pcTime = ((LONGLONG)(cUlarge_Integer.QuadPart - 116444736000000000) / 10000000);
}

/*=============================================================================
  函 数 名： GetMtWorkingPath
  功    能： 返回每个终端对应的工作路径
  算法实现： 
  全局变量： 
  参    数： dwInstid : 终端注册录像机服务器的Instance号
  返 回 值： 返回每个终端对应的工作路径
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2010/09/1   1.0     刘旭    创建
=============================================================================*/
LPCSTR  CRecCfg::GetMtWorkingPath(const u32& dwInstid)
{
	// 数组索引不能越界
	if (dwInstid > MAXNUM_RECORDER_CHNNL || 0 == dwInstid)
	{
		return NULL;
	}

	return m_szMTWorkPath[dwInstid - 1];
}

/*=============================================================================
  函 数 名： PreProcFileame
  功    能： 预处理文件名，拼接上VOD路径和MCU名称信息，形成带完整路径的文件全名
             如把 a.asf 替换成 e:\vod\mcuname/a.asf
  算法实现： 
  全局变量： 
  参    数： lpszFullName [out] 需要自己预分配空间
             最新需要要求不包含会议名，最后一个参数无效
  返 回 值： 文件名FullName的长度，当文件名加路径超过258时，返回值为0    //add by jlb  081026
=============================================================================*/
u16 CRecCfg::PreProcFileame(LPSTR lpszFullName, LPSTR lpszOldName, LPSTR lpszDirname)
{
	u16 wFileNameSize = strlen(GetWorkingPath()) + strlen(lpszOldName);
	if (wFileNameSize > MAX_FILE_NAME_LEN || strlen(lpszOldName) > KDV_NAME_MAX_LENGTH)  
	{
		wFileNameSize = 0;
        OspPrintf(TRUE, FALSE, "[REC] [PreProcFileame] file name is too long\n");
	}
	else
	{
        sprintf(lpszFullName, "%s/%s", 
                GetWorkingPath(),
                lpszOldName == NULL ? "\0" : lpszOldName
                );
	}
    return wFileNameSize;
}    


/**
/*  预处理文件名，拼接上VOD路径和MCU名称信息
/*  lpszFullName [out] 需要自己预分配空间
/*	如把 conf1 和 a.asf 替换成 e:\vod\mcuname\conf1/a.asf
*/
u16 CRecCfg::PreProcMtFileName(LPSTR lpszFullName, LPSTR lpszOldName, LPSTR lpszDirname, u32 dwMtId)
{
	u16 wFileNameSize = strlen(GetMtWorkingPath(dwMtId)) + strlen(lpszOldName);
	if (wFileNameSize > MAX_FILE_NAME_LEN)  
	{
		wFileNameSize = 0;
        OspPrintf(TRUE, FALSE, "[REC] [PreProcMtFileName] file name is too long\n");
	}
	else
	{
        sprintf(lpszFullName, "%s/%s", 
			GetMtWorkingPath(dwMtId),
			lpszOldName == NULL ? "\0" : lpszOldName
			);
	}
    return wFileNameSize;
}

/*=============================================================================
  函 数 名： PreProcPatternName
  功    能： 预处理文件名，形成模板名
             如把 a 替换成 e:\vod\mcuname/a_%d_255_384K.asf
  算法实现： 
  全局变量： 
  参    数： lpszFullName [out] 需要自己预分配空间
             lpszOldNameWithoutDotAsf [in] 不带路径，不带.asf，不带码率、用户组信息
  返 回 值：  
=============================================================================*/
void CRecCfg::PreProcPatternName(LPSTR lpszPatternName, 
                                 LPCSTR lpszOldNameWithoutDotAsf,
                                 u8 byOwnerGrp, u16 wBitrate)
{
    sprintf(lpszPatternName, "%s_%%d_%d_%dK.asf", 
            lpszOldNameWithoutDotAsf,
            byOwnerGrp, wBitrate
            );
}


/*=============================================================================
  函 数 名： SplitPath
  功    能： 反处理， 从完整文件名中分割目录和文件名，要求以 / 区分
             如 e:\vod\mcuname/abc.asf
             分割为 e:\vod\mcuname/ 和 abc.asf
  算法实现： 
  全局变量： 
  参    数： 用户自己需要给出完整的 memset 0 过的空间             
  返 回 值：  
=============================================================================*/
void CRecCfg::SplitPath(LPCSTR lpszFull, LPSTR lpszDir, LPSTR lpszName, BOOL32 bWithSlash)
{
    LPCSTR lpDest = strchr(lpszFull, '/');
    if ( lpDest != NULL)
    {
        strncpy( lpszDir, lpszFull, (lpDest - lpszFull + (bWithSlash ? 1 : 0) ) );
        strcpy( lpszName, lpDest + 1);
    }
}

/*=============================================================================
  函 数 名： Path2MMS
  功    能： 把完整路径切割成符合MMS要求的路径和名称
  算法实现： 
  全局变量： 
  参    数： 用户自己需要给出完整的 memset 0 过的空间             
  返 回 值：  
=============================================================================*/
void CRecCfg::Path2MMS(LPCSTR lpszFull, LPSTR lpszMMS)
{
    if (lpszFull != NULL)
    {
        strcpy( lpszMMS, lpszFull + strlen(g_tCfg.m_szRecordPath) + 1 );
        s8 *tok = NULL;
        while ( (tok = strchr( lpszMMS, '\\' )) != NULL )
        {
            *tok = '/';
            tok ++;
        }
    }
}


/*=============================================================================
函 数 名:GetRecShowFileName
功    能:获取界面显示的录像文件名
参    数:LPCTSTR lptrNameWithRate       [in]    含有用户组ID和会议码率的文件名
注    意:无
返 回 值:去除.asf, 用户组ID和会议码率的文件名
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2006/09/06  4.0     王昊    创建
=============================================================================*/
BOOL32 CRecCfg::GetParamFromFilename(LPCSTR lpszFullname, u8 &byGrpId, u16 &wBitrate, CString& cstrPureName)
{
    byGrpId = USRGRPID_INVALID;
    wBitrate = 0;

    CString cstrFileName(lpszFullname);
    cstrFileName.MakeLower();
    
    if ( cstrFileName.Right( 4 ) == _T(".asf") )
    {
        cstrFileName = cstrFileName.Left( cstrFileName.GetLength() - 4 );
    }

    if ( cstrFileName[cstrFileName.GetLength() - 1] != 'k' )
    {
        return FALSE;
    }

    CString cstrTempName = cstrFileName.Left( cstrFileName.GetLength() - 1 );
    int nFind = cstrTempName.ReverseFind( '_' );
    if ( nFind == 0 || nFind == -1 || cstrTempName.GetLength() == nFind + 1 )
    {
        return FALSE;
    }

    CString cstrNum = cstrTempName.Right( cstrTempName.GetLength() - nFind - 1 );
    for ( int nIndex = 0; nIndex < cstrNum.GetLength(); nIndex ++ )
    {
        if ( ! ::isdigit( cstrNum[nIndex] ) )
        {
            return FALSE;
        }
    }
    
    if ( _ttoi( cstrNum ) > 0x0FFFF )
    {
        return FALSE;
    }
    wBitrate = _ttoi( cstrNum );

    cstrTempName = cstrTempName.Left( nFind );
    nFind = cstrTempName.ReverseFind( '_' );
    if ( nFind == 0 || nFind == -1 || cstrTempName.GetLength() == nFind + 1 )
    {
        return FALSE;
    }
    cstrNum = cstrTempName.Right( cstrTempName.GetLength() - nFind - 1 );
    for ( nIndex = 0; nIndex < cstrNum.GetLength(); nIndex ++ )
    {
        if ( ! ::isdigit( cstrNum[nIndex] ) )
        {
            return FALSE;
        }
    }
    if ( _ttoi( cstrNum ) > 0x0FF )
    {
        return FALSE;
    }
    byGrpId = _ttoi( cstrNum );

    cstrTempName = cstrTempName.Left( nFind );
    if ( cstrTempName[cstrTempName.GetLength() - 1] == '\\' )
    {
        return FALSE;
    }

    cstrPureName = lpszFullname;
    cstrPureName = cstrPureName.Left( cstrTempName.GetLength() );

    return TRUE;
}

/*=============================================================================
  函 数 名: CheckPureName
  功    能: 用户自定义名称检验
  参    数: 
  注    意: 无
  返 回 值: bool
-------------------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人      修改内容
  2011/08/01    4.6     刘旭        创建
=============================================================================*/
u16 CRecCfg::CheckPureName( const s8* pszFileName )
{
#define CHECK_INVALID_CHAR(a, b, err)	if((a) == (b)) return (err);

	// 名称中不能有下划线
	s8* pszTemp = const_cast<s8*>( pszFileName);
	while (pszTemp && *pszTemp != '\0' )
	{
		// 文件名纯名字中不能有下划线
		CHECK_INVALID_CHAR( *pszTemp, '_', ERR_REC_HAS_UNDERLINE)	
	
		++pszTemp;
	}

	return ERR_REC_NOERROR;
}

/*=============================================================================
  函 数 名: GetWHByRes 
  功    能: 通过分辨率获取宽高
  参    数: 
  注    意: 无
  返 回 值: void
-------------------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人      修改内容
  2008/06/13    4.0     张宝卿      创建
=============================================================================*/
void CRecCfg::GetWHByRes(u8 byRes, u16 &wWidth, u16 &wHeight)
{
	wWidth = 0;
	wHeight = 0;

	switch(byRes)
	{
    case VIDEO_FORMAT_SQCIF_112x96:
		wWidth = 112;
		wHeight = 96;
		break;

    case VIDEO_FORMAT_SQCIF_96x80:
		wWidth = 96;
		wHeight = 80;
		break;
	
    case VIDEO_FORMAT_SQCIF:
		wWidth = 128;
		wHeight = 96;
		break;

    case VIDEO_FORMAT_QCIF:
		wWidth = 176;
		wHeight = 144;
		break;

    case VIDEO_FORMAT_CIF:
		wWidth = 352;
		wHeight = 288;
		break;

    case VIDEO_FORMAT_2CIF:
		wWidth = 352;
		wHeight = 576;
		break;

    case VIDEO_FORMAT_4CIF:
		
		//基于尽可能播放考虑，这里取大值
		wWidth = 704;
		wHeight = 576;
		break;

    // 增加高清分辨率, zgc, 2008-08-28
	case VIDEO_FORMAT_W4CIF:
		wWidth = 1024;
		wHeight = 576;
		break;

	case VIDEO_FORMAT_SXGA:
		wWidth = 1280;
		wHeight = 1024;
		break;

	case VIDEO_FORMAT_UXGA:
		wWidth = 1600;
		wHeight = 1200;
		break;

    case VIDEO_FORMAT_HD720:
        wWidth = 1280;
        wHeight = 720;
        break;

    case VIDEO_FORMAT_HD1080:
        wWidth = 1920;
        wHeight = 1088;
        break;

    case VIDEO_FORMAT_16CIF:
		wWidth = 1048;
		wHeight = 1152;
		break;
		
    case VIDEO_FORMAT_AUTO:
		//不解析
		break;
	
    case VIDEO_FORMAT_SIF:
		wWidth = 352;
		wHeight = 240;
		break;

    case VIDEO_FORMAT_2SIF:
		wWidth = 352;
		wHeight = 480;
		break;

    case VIDEO_FORMAT_4SIF:
		wWidth = 704;
		wHeight = 480;
		break;

    case VIDEO_FORMAT_VGA:
		wWidth = 640;
		wHeight = 480;
		break;

    case VIDEO_FORMAT_SVGA:
		wWidth = 800;
		wHeight = 600;
		break;

    case VIDEO_FORMAT_XGA:
		wWidth = 1024;
		wHeight = 768;
		break;
	default:
		break;
	}
	if ( 0 == wHeight || 0 == wWidth )
	{
		OspPrintf( TRUE, FALSE, "[GetWHByRes] unexpected res.%d, ignore it\n", byRes );
	}
	return;
}

/*=============================================================================
  函 数 名: GetResByWH 
  功    能: 通过宽高获取分辨率
  参    数: 
  注    意: 无
  返 回 值: void
-------------------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人      修改内容
  2013/08/19    4.7     zhangli      create
=============================================================================*/
void CRecCfg::GetResByWH(u16 wWidth, u16 wHeight, u8 &byRes)
{
	byRes = VIDEO_FORMAT_INVALID;

	if (wWidth == 112 && wHeight == 96)
	{
		byRes = VIDEO_FORMAT_SQCIF_112x96;
	}
	else if (wWidth == 96 && wHeight == 80)
	{
		byRes = VIDEO_FORMAT_SQCIF_96x80;
	}
	else if (wWidth == 128 && wHeight == 96)
	{
		byRes = VIDEO_FORMAT_SQCIF;
	}
	else if (wWidth == 176 && wHeight == 144)
	{
		byRes = VIDEO_FORMAT_QCIF;
	}
	else if (wWidth == 352 && wHeight == 288)
	{
		byRes = VIDEO_FORMAT_CIF;
	}
	else if (wWidth == 352 && wHeight == 576)
	{
		byRes = VIDEO_FORMAT_2CIF;
	}
	else if (wWidth == 704 && wHeight == 576)
	{
		byRes = VIDEO_FORMAT_4CIF;
	}
	else if (wWidth == 1024 && wHeight == 576)
	{
		byRes = VIDEO_FORMAT_W4CIF;
	}
	else if (wWidth == 1280 && wHeight == 1024)
	{
		byRes = VIDEO_FORMAT_SXGA;
	}
	else if (wWidth == 1600 && wHeight == 1200)
	{
		byRes = VIDEO_FORMAT_UXGA;
	}
	else if (wWidth == 1280 && wHeight == 720)
	{
		byRes = VIDEO_FORMAT_HD720;
	}
	else if ((wWidth == 1920 && wHeight == 1080) || (wWidth == 1920 && wHeight == 1088))
	{
		byRes = VIDEO_FORMAT_HD1080;
	}
	else if (wWidth == 1048 && wHeight == 1152)
	{
		byRes = VIDEO_FORMAT_16CIF;
	}
	else if (wWidth == 352 && wHeight == 240)
	{
		byRes = VIDEO_FORMAT_SIF;
	}
	else if (wWidth == 352 && wHeight == 480)
	{
		byRes = VIDEO_FORMAT_2SIF;
	}
	else if (wWidth == 704 && wHeight == 480)
	{
		byRes = VIDEO_FORMAT_4SIF;
	}
	else if (wWidth == 640 && wHeight == 480)
	{
		byRes = VIDEO_FORMAT_VGA;
	}
	else if (wWidth == 800 && wHeight == 600)
	{
		byRes = VIDEO_FORMAT_SVGA;
	}
	else if (wWidth == 1024 && wHeight == 768)
	{
		byRes = VIDEO_FORMAT_XGA;
	}

	if (VIDEO_FORMAT_INVALID == byRes)
	{
		OspPrintf( TRUE, FALSE, "[GetResByWH] unexpected width.%d or height.%d, ignore it\n", wWidth, wHeight);
	}
}

/*=============================================================================
  函 数 名: IsConfHD 
  功    能: 判断高清
  参    数: 
  注    意: 无
  返 回 值: void
-------------------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人      修改内容
  2008/06/13    4.0     张宝卿      从会议移植过来
=============================================================================*/
BOOL32 CRecCfg::IsConfHD(const TCapSupport &tCap)
{
	BOOL32 bFormatHD = FALSE;
    u8 byVideoType = tCap.GetMainVideoType();
    if (MEDIA_TYPE_H264 != byVideoType)
    {
        return bFormatHD;
    }
    
    u8 byConfFormat = tCap.GetMainVideoResolution();
    switch( byConfFormat )
    {
    case VIDEO_FORMAT_W4CIF:
    case VIDEO_FORMAT_HD720:
    case VIDEO_FORMAT_SXGA:
    case VIDEO_FORMAT_UXGA:
    case VIDEO_FORMAT_HD1080:
        bFormatHD = TRUE;
        break;
    default:
        break;
    }
    return bFormatHD;
}

/*=============================================================================
函 数 名： OpenDB
功    能： 打开数据库
算法实现： 
全局变量： 
参    数：  无
返 回 值： 无
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/01/11  4.7		    zhangli               create
=============================================================================*/
u16 CRecCfg::OpenDB()
{
	CString cDBHostIp;
	CString cDBUserName;
	CString cDBPWord;
	
	cDBHostIp = g_tCfg.m_achDBHostIP;
	cDBUserName = g_tCfg.m_achDBUsername;
	cDBPWord = g_tCfg.m_achDBPassword;
	u16 wRet = g_cTDBOperate.OpenDB(cDBHostIp, cDBUserName, cDBPWord);
	if (VOD_OK != wRet)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "[PublishOneFile] Open Failed Again!IP:%s,user:%s,pwd:%s,error:%d\n",
			g_tCfg.m_achDBHostIP, g_tCfg.m_achDBUsername, g_tCfg.m_achDBPassword, wRet);

		g_tCfg.m_bOpenDBSucceed = FALSE;
		return wRet;
	}
	
	g_tCfg.m_bOpenDBSucceed = TRUE;
	return wRet;
}
/*=============================================================================
函 数 名： MediaInfoRec2Mcu
功    能： 
算法实现： 
全局变量： 
参    数：  TMediaInfo tMediaInfo
           TPlayFileMediaInfo &tPlayFileMediaInfo
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/8/4   4.0		周广程                  创建
=============================================================================*/
void CChnInst::MediaInfoRec2Mcu( TMediaInfo tMediaInfo, TPlayFileMediaInfo &tPlayFileMediaInfo )
{
    tPlayFileMediaInfo.SetStreamNum( tMediaInfo.m_byNum );
    tPlayFileMediaInfo.SetAudio( tMediaInfo.m_abyMediaType[0] );
    tPlayFileMediaInfo.SetVideo( tMediaInfo.m_abyMediaType[1], tMediaInfo.m_awWidth[1], tMediaInfo.m_awHeight[1] );
    tPlayFileMediaInfo.SetDVideo( tMediaInfo.m_abyMediaType[2], tMediaInfo.m_awWidth[2], tMediaInfo.m_awHeight[2] );
}


void CChnInst::Reclog( s8* pszFmt, ... )
{
    s8 achPrintBuf[KDV_MAX_PATH];
    s32  nBufLen = 0;
    va_list argptr;
    if( g_bPrintRecLog )
    {		  
        nBufLen = sprintf( achPrintBuf, "[Rec]: " ); 
        va_start( argptr, pszFmt );
		vsnprintf(achPrintBuf + nBufLen, KDV_MAX_PATH - nBufLen - 1, pszFmt, argptr );
        //nBufLen += vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end(argptr); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }
    return;
}

void CChnInst::OnDaemStartRecReq(CMessage * const pMsg, CApp * pcApp)
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
	for(u16 wLoop=0; wLoop< g_byRecChnNum; wLoop++)
	{
		CChnInst * pInst;
		pInst = (CChnInst *)pcApp->GetInstance(wLoop+1);

		// [8/24/2010 liuxu] 添加了终端是否连接上的判断
		if (TRecChnnlStatus::STATE_RECREADY == pInst->CurState() && !(pInst->IsMtConnect()))
		{
			// [8/25/2010 liuxu] 如果源App值为AID_REC_SERVER，则是终端对录像机服务器发起的录像
			// 否则是mcu发起的录像
			if (AID_REC_SERVER == GETAPP(pMsg->srcid))
			{
				pInst->SetMtConnect(TRUE);
				pInst->SetMTNodeInfo(pMsg->srcnode, pMsg->srcid);		
				RecServLevelLog(RECSERV_LEVEL_LOG_COMMON, "Select Chanel[%d] to start recording\n", (wLoop + 1));
			}

			post(MAKEIID(GetAppID(), (wLoop+1)), pMsg->event, pMsg->content, pMsg->length);
			return;
		}
	}

	OspPrintf(TRUE, FALSE, "[Rec] The Record Channels are not enough\n");
	cServMsg.SetErrorCode(ERR_REC_VALIDCHN);
	TMt tMt = *(TMt*)cServMsg.GetMsgBody();
	cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
	cServMsg.CatMsgBody((u8*)&g_cRecApp.m_tEqp, sizeof(g_cRecApp.m_tEqp));

	// 如果消息来源于录像机服务器,则给服务器实例发送消息
	if (AID_REC_SERVER == GETAPP(pMsg->srcid))
	{
		SetMTNodeInfo(pMsg->srcnode, pMsg->srcid);
		SendMsgToRecSevr(REC_MCU_STARTREC_NACK, &cServMsg);
	}else
	{
		SendMsgToMcu(REC_MCU_STARTREC_NACK, &cServMsg);
	}
}

void CChnInst::OnMtRegReq(CMessage * const pMsg, CApp * pcApp)
{
    // 处理回复消息体
    CServMsg cServMsg(pMsg->content, pMsg->length);

	// 验证消息体是否合法
    if (sizeof(TMTRegInfo) != cServMsg.GetMsgBodyLen())
    {
		TErrorInfo tErr = {0};
		tErr.m_wErrCode = ERR_REC_SERVER_INVALID_PARAM;
		OspPost(pMsg->srcid,
			RECSERV_MT_REG_NACK,
			&tErr,
			sizeof(tErr),
			pMsg->srcnode,
        MAKEIID(GetAppID(), GetInsID()));

        return;
    }

    TMTRegInfo tMtRegInfo;
    memcpy( (void *)&tMtRegInfo, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
    tMtRegInfo.m_aliase[MAXLEN_RECORD_NAME] = '\0';
	// 防止访问数组g_cRecApp.m_szMTWorkPath越界
	u16 wMtId = GETINS(pMsg->srcid);
	ASSERT((wMtId) < MAXNUM_RECORDER_CHNNL);
	if (strlen(tMtRegInfo.m_aliase))
	{
		// 终端别名不为空,根据终端别名形成工作路径
		snprintf( g_cRecApp.m_szMTWorkPath[wMtId - 1], MAX_FILE_PATH_LEN - 1,
			"%s\\%s", 
			g_tCfg.m_szRecordPath, 
			tMtRegInfo.m_aliase);
	}else
	{
		// 终端别名为空,根据终端别名形成工作路径
		snprintf( g_cRecApp.m_szMTWorkPath[wMtId - 1], MAX_FILE_PATH_LEN - 1,
			"%s\\%s", 
			g_tCfg.m_szRecordPath, 
			MT_DEFAULT_NAME);		
	}

	::CreateDirectory( g_cRecApp.GetMtWorkingPath(wMtId), NULL );             
    OspPrintf( TRUE, FALSE, "Create work file path : %s!\n" , g_cRecApp.GetMtWorkingPath(wMtId)); 

	// 设置ack信息
    TMtRegAck tAck;
    tAck.SetEqpAlias( g_tCfg.m_szAlias );
    tAck.SetChnnlNum( g_byRecChnNum ); // 录像通道数
    tAck.SetPeriEqpIpAddr( g_tCfg.m_dwRecIpAddr );
    tAck.SetStartPort( g_tCfg.m_wRecStartPort );
    tAck.SetVersion(DEVVER_RECORDER);
    tAck.SetMcuEqp((u8)g_tCfg.m_wMcuId, g_tCfg.m_byEqpId, g_tCfg.m_byEqpType);

	cServMsg.ClearHdr();
	cServMsg.SetMsgBody((u8*)&tAck, sizeof(tAck));
	// 回复Ack
    OspPost(pMsg->srcid,
        RECSERV_MT_REG_ACK,
        cServMsg.GetServMsg(),
        cServMsg.GetServMsgLen(),
        pMsg->srcnode,
        MAKEIID(GetAppID(), GetInsID()));
}

BOOL32 CChnInst::SendMsgToRecSevr(u16 wEvent, CServMsg * const pcServMsg)
{
    RecServLevelLog( RECSERV_LEVEL_LOG_BELOW, "RecIns(%d) Post %s to MT\n", GetInsID(), OspEventDesc(wEvent));
    
    OspPost( m_dwMtInstId, 
        wEvent, 
        pcServMsg->GetServMsg(), 
        pcServMsg->GetServMsgLen(), 
        m_dwMtNodeId,
        MAKEIID(GetAppID(), GetInsID()));
    
    return TRUE;
}


BOOL32 CChnInst::SendMsgToRecSevrDaem(u16 wEvent, CServMsg * const pcServMsg)
{
    RecServLevelLog( RECSERV_LEVEL_LOG_BELOW, "RecIns(%d) Post %s to RecServer Daemon\n", GetInsID(), OspEventDesc(wEvent));
    
    OspPost( MAKEIID(AID_REC_SERVER, DAEMON), 
        wEvent, 
        pcServMsg->GetServMsg(), 
        pcServMsg->GetServMsgLen(), 
        m_dwMtNodeId,
        MAKEIID(GetAppID(), GetInsID()));
    
    return TRUE;
}


void CChnInst::OnMtDisconnect(CMessage * const pMsg)
{
    u16 wRet;
    
	// 此通道时录像通道
    if( GetInsID() <= g_byRecChnNum )
    {
        // 录像通道
        if( TRecChnnlStatus::STATE_RECREADY != CurState() )
        {
			// 停止录像
            wRet = ::RPStopRec( m_dwDevice, 0 );
            if( RP_OK != wRet  )
            {
                Reclog("Fail to force stop device %d, error code: %d\n", m_dwDevice, wRet);
            }
            else
            {
				LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "[OnMtDisconnect]call device%d \"rpStopRecord\" success.\n", m_dwDevice );
            }
            
            m_tChnnlStatus.m_byState = TRecChnnlStatus::STATE_RECREADY;
            m_tChnnlStatus.m_tProg.SetCurProg( 0 );
            m_tChnnlStatus.m_tProg.SetTotalTime( 0 );
            m_tChnnlStatus.SetRecordName("");            
            NEXTSTATE( TRecChnnlStatus::STATE_RECREADY );
            SendChnNotify();
            m_cConfId.SetNull();
            KillTimer(EV_REC_RECPROGTIME);
        }
        
    }else			// 非录像通道
    {
        // 未定义的实例号
        OspPrintf(TRUE, FALSE, "[MsgForceStopProc]Undefined Instance ID: %d\n", GetInsID());
        return;
    }

	// 清空终端录像文件路径
	// memset(g_cRecApp.m_szMTWorkPath[GETINS(m_dwMtInstId) - 1], 0, MAX_FILE_PATH_LEN);
	// 清空终端节点信息
    m_dwMtNodeId = 0;
    m_dwMtInstId = 0;
    
	// 将此instance设置为为连接
    SetMtConnect(FALSE);
    
	//释放使用文件
    memset( g_cRecApp.m_achUsingFile[GetInsID()], 0, MAX_FILE_NAME_LEN );
    
	return;
}

void CChnInst::SetMTNodeInfo(u32 dwNodeId, u32 dwInstId)
{
    m_dwMtNodeId = dwNodeId;
    m_dwMtInstId = dwInstId;
}

/*=============================================================================
函 数 名： PublishOneFile
功    能： 发布录像文件，如果发布失败，则重连数据库再次发布
算法实现： 
全局变量： 
参    数：  s8 *szFullFileName,文件全路径
			BOOL32 isWriteFileInfo，是否取文件大小和时长，如果是模式为立即发布，开始录像发布时文件可能还没创建，大小和时长填0
返 回 值： errorCode 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/04/13  4.7		    zhangli               create
=============================================================================*/
u16 CChnInst::PublishOneFile(s8 *szFullFileName, BOOL32 isWriteFileInfo)
{
	LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "[PublishOneFile]\npublish file:%s\npublishMode:%d\nchnlIdx:%d\n", 
		szFullFileName, m_byPublishMode, GetInsID());

	//发布日期
	time_t tCurTime = time(NULL);
	struct tm *ptTime = localtime(&tCurTime);
	s8 achPublishTime[32] = {0};
	
	sprintf(achPublishTime, "%4.4u-%u-%u %u:%u:%u", ptTime->tm_year + 1900, ptTime->tm_mon + 1,
				ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec);

	//时长和文件大小
	TFileInfo tFileInfo;
	memset(&tFileInfo, 0, sizeof(TFileInfo));
	s32 wRet = VOD_OK;

	if (isWriteFileInfo)
	{
		wRet = RPGetFileInfo(MAXNUM_RECORDER_CHNNL, szFullFileName, &tFileInfo);
		if (RP_OK != wRet)
		{
			LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "[PublishOneFile] Get FileInfo Failed! error:%d\n", wRet);
		}
		else
		{
			tFileInfo.m_dwSize = tFileInfo.m_dwSize/1024;
		}
	}
	
	//mms path
	s8 szMMSName[MAX_FILE_NAME_LEN]  = {0};
	g_cRecApp.Path2MMS(szFullFileName, szMMSName);
	
	LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "mms name:%s\nfileSize:%d\nduration:%d\npublishpath:%s", 
		szMMSName, tFileInfo.m_dwSize, tFileInfo.m_dwDuration, g_cRecApp.m_achPublishPath);
	
	//publish
	wRet = g_cTDBOperate.PublishOneFile(g_cRecApp.m_achPublishPath, szMMSName, tFileInfo.m_dwSize, &achPublishTime[0], 
		(BOOL)m_byPublishLevel, tFileInfo.m_dwDuration);

	if (VOD_OK != wRet)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "Publish file failed, so try to open db again, error:%d\n",
			szFullFileName, wRet, GetInsID());

		if (VOD_OK != g_cRecApp.OpenDB())
		{
			return wRet;
		}

		wRet = g_cTDBOperate.PublishOneFile(g_cRecApp.m_achPublishPath, szMMSName, tFileInfo.m_dwSize, &achPublishTime[0], 
			(BOOL)m_byPublishLevel, tFileInfo.m_dwDuration);

		if (VOD_OK != wRet)
		{
			LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "Publish file Failed Again!!!!\n");
		}
	}

	if (VOD_OK == wRet)
	{
		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "[PublishOneFile]Publish file success!\n");
	}
	return wRet;
}

/*=============================================================================
函 数 名： UpdatePublishOneFile
功    能： 更新录像文件，如果更新失败，则重连数据库再次更新
算法实现： 
全局变量： 
参    数：  s8 *szFullFileName,文件全路径
返 回 值： errorCode 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/04/13  4.7		    zhangli               create
=============================================================================*/
u16 CChnInst::UpdatePublishOneFile(char *szFullFileName)
{
	LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "[UpdatePublishOneFile]\nupdate file:%s\npublishMode:%d\nchnlIdx:%d\n", 
		szFullFileName, m_byPublishMode, GetInsID());

	//时长和文件大小
	TFileInfo tFileInfo;
	memset(&tFileInfo, 0, sizeof(TFileInfo));
	s32 wRet = RPGetFileInfo(0, szFullFileName, &tFileInfo);
	if (RP_OK != wRet)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "[UpdatePublishOneFile] Get duration and size Failed! error:%d\n", wRet);
		return wRet;
	}
	tFileInfo.m_dwSize = tFileInfo.m_dwSize/1024;
	//mms path
	s8 szMMSName[MAX_FILE_NAME_LEN]  = {0};
	g_cRecApp.Path2MMS(szFullFileName, szMMSName);
	
	LogPrint(LOG_LVL_DETAIL, MID_RECEQP_COMMON, "\nmms name:%s\nfileSize:%d\nduration:%d\n", 
		szMMSName, tFileInfo.m_dwSize, tFileInfo.m_dwDuration);

	//update
	wRet = g_cTDBOperate.UpdatePublishOneFile(g_cRecApp.m_achPublishPath, szMMSName, tFileInfo.m_dwSize, tFileInfo.m_dwDuration);
	if (VOD_OK != wRet)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "update file failed, so try to open database, error:%d\n", wRet);

		if (VOD_OK != g_cRecApp.OpenDB())
		{
			return wRet;
		}
		
		wRet = g_cTDBOperate.UpdatePublishOneFile(g_cRecApp.m_achPublishPath, szMMSName, tFileInfo.m_dwSize, tFileInfo.m_dwDuration);

		if (VOD_OK != wRet)
		{
			LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "update file Failed Again! error:%d\n", wRet);
		}
	}
	
	if (VOD_OK == wRet)
	{
		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "[PublishOneFile]update file success!\n");
	}

	return wRet;
}

/*=============================================================================
函 数 名： AddOneLiveSource
功    能： 添加直播点到数据库
算法实现： 
全局变量： 
参    数：  s8 *szFullFileName,文件全路径
返 回 值： errorCode 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/01/11  4.7		    zhangli               create
=============================================================================*/
u16 CChnInst::AddOneLiveSource()
{
	CString cHostIp;
	CString cPPName;
	
	cHostIp = g_tCfg.m_szMachineName;
	cPPName = m_achOrignalFileName;
	u16 wRet = g_cTDBOperate.AddOneLiveSource(cPPName, cHostIp, FALSE);
	if (VOD_OK != wRet)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "AddOneLiveSource failed, so try to open database, error:%d\n", wRet);
		
		if (VOD_OK != g_cRecApp.OpenDB())
		{
			return wRet;
		}
		
		wRet = g_cTDBOperate.AddOneLiveSource(cPPName, cHostIp, FALSE);
		
		if (VOD_OK != wRet)
		{
			LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "AddOneLiveSource Failed Again! error:%d\n", wRet);
		}
	}
	
	if (VOD_OK == wRet)
	{
		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "[PublishOneFile]AddOneLiveSource success!\n");
	}

	return wRet;
}

/*=============================================================================
函 数 名： DeleteOneLiveSource
功    能： 从数据库中删除直播点
算法实现： 
全局变量： 
参    数：  s8 *szFullFileName,文件全路径
返 回 值： errorCode 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/01/11  4.7		    zhangli               create
=============================================================================*/
u16 CChnInst::DeleteOneLiveSource()
{
	u16 wRet = VOD_OK;
	if(!g_tCfg.m_bOpenDBSucceed)
	{
		wRet = g_cRecApp.OpenDB();
		if (VOD_OK != wRet)
		{
			return wRet;
		}
	}

	CString cHostIp;
	CString cPPName;
	
	cHostIp = g_tCfg.m_szMachineName;
	cPPName = m_achOrignalFileName;
	wRet = g_cTDBOperate.DeleteOneLiveSource(cPPName, cHostIp);

	if (VOD_OK != wRet)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "DeleteOneLiveSource failed, so try to open database, error:%d\n", wRet);
		
		if (VOD_OK != g_cRecApp.OpenDB())
		{
			return wRet;
		}
		
		wRet = g_cTDBOperate.DeleteOneLiveSource(cPPName, cHostIp);
		
		if (VOD_OK != wRet)
		{
			LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "DeleteOneLiveSource Failed Again! error:%d\n", wRet);
		}
	}
	
	if (VOD_OK == wRet)
	{
		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "[PublishOneFile]DeleteOneLiveSource success!\n");
	}

	return wRet;
}

/*=============================================================================
函 数 名： InitCOMServer
功    能： 初始化media server 相关变量
算法实现： 
全局变量： 
参    数：  无
返 回 值： TRUE/FALSE
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/01/07  4.7		    zhangli               create
=============================================================================*/
BOOL32 CChnInst::InitCOMServer()
{
	if (m_pBCPubPoint != NULL)
	{
		m_pBCPubPoint->Release();
		m_pBCPubPoint = NULL;
	}

	if (m_pPubPoint != NULL)
	{
		m_pPubPoint->Release();
		m_pPubPoint = NULL;
	}

	if (g_pPubPoints != NULL)
	{
		g_pPubPoints->Release();
		g_pPubPoints = NULL;
	}
	
	if (g_pServer != NULL)
	{
		g_pServer->Release();
		g_pServer = NULL;
	}
	
	CoUninitialize();
//	s8 errorBuff[256] = {0};
	//初始化COM
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
//		sprintf(errorBuff, "initialize com failed.errorcode:%x\n", hr);
//		AfxMessageBox(errorBuff);
		return FALSE;
	}

	//retrieve a pointer to an IWMSServer interface.
	hr = CoCreateInstance(CLSID_WMSServer, NULL, CLSCTX_ALL, IID_IWMSServer, (void **)&g_pServer);
	if (FAILED(hr))
	{
		g_pServer = NULL;
//		sprintf(errorBuff, "the recorder isn't supprot broadcast point. errorcode:%x\n", hr);
//		AfxMessageBox(errorBuff);
		return FALSE;
	}
	
	// Retrieve a pointer to the IWMSPublishingPoints interface and retrieve the number of publishing points.
	hr = g_pServer->get_PublishingPoints(&g_pPubPoints);
	if (FAILED(hr))
	{
		g_pPubPoints = NULL;
//		sprintf(errorBuff, "retrieve a pointer to an IWMSPublishingPoints interface failed. errorcode:%x\n", hr);
//		AfxMessageBox(errorBuff);
		return FALSE;
	}

	return TRUE;
}

/*=============================================================================
函 数 名： CreateBrdPubPoint
功    能： 创建一个直播发布点
算法实现： 
全局变量： 
参    数：  无
返 回 值： 无
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/01/07  4.7		    zhangli               create
=============================================================================*/
BOOL32 CChnInst::CreateBrdPubPoint()
{
	if (NULL == g_pServer || NULL == g_pPubPoints)
	{
		return FALSE;
	}

	if (NULL != m_pPubPoint)
	{
		DeleteBrdPubPoint();
	}

	if (NULL != m_pPubPoint)
	{
		return FALSE;
	}

	HRESULT hr = S_OK;
	if (!CreatePlayList())
	{
		return FALSE;
	}

	//创建发布点
	CComBSTR bstrName(m_achOrignalFileName); 	
	CComBSTR bstrPath(m_achPlayListName);
	
	// [2013/2/27 zhangli]发布点名字重复
	hr = g_pPubPoints->Add(bstrName, WMS_PUBLISHING_POINT_BROADCAST, bstrPath, &m_pPubPoint);

	u8 byNumber = 0;
	s8 achPPName[MAX_FILE_NAME_LEN] = {0};
	memcpy(achPPName, m_achOrignalFileName, strlen(m_achOrignalFileName));
	while (hr == NS_E_DUPLICATE_NAME && byNumber <= 255)
	{
		if (m_pPubPoint != NULL)
		{
			m_pPubPoint->Release();
			m_pPubPoint = NULL;
		}
		memset(achPPName, 0, MAX_FILE_NAME_LEN);
		sprintf(achPPName, "%s-%d", m_achOrignalFileName, byNumber);
		bstrName = achPPName;
		hr = g_pPubPoints->Add(bstrName, WMS_PUBLISHING_POINT_BROADCAST, bstrPath, &m_pPubPoint);
		byNumber++;
	}

	memcpy(m_achOrignalFileName, achPPName, strlen(achPPName));

	LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC, "m_achOrignalFileName:%s\n", m_achOrignalFileName);

	if (FAILED(hr) || m_pPubPoint == NULL) 
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, 
			"retrieve a pointer to an IWMSPublishingPoint interface failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());

		return FALSE;
	}

	SetTimer(EV_REC_PUBLISHPOINT_TIMER, CHECK_PUBLISHPOINT_INTERVAL);

	// 获取广播发布点指针
	hr = m_pPubPoint->QueryInterface(IID_IWMSBroadcastPublishingPoint, (void **)&m_pBCPubPoint);
	if (FAILED(hr) || NULL == m_pBCPubPoint) 
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, 
			"retrieve a pointer to an IWMSBroadcastPublishingPoint interface failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		return FALSE;
	}

	// 禁止"第一个用户接入自动开启发布点"
	hr = m_pBCPubPoint->put_AllowClientToStartAndStop(VARIANT_FALSE);
	if (FAILED(hr)) 
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "put_AllowClientToStartAndStop failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
	}
	return TRUE;

	////////设置能够匿名登录 ////////////////////////////////////////////////////////////////////////////
// 	IWMSPlugins *pPlugins;
// 	m_pPubPoint->get_Authenticators(&pPlugins);
// 	
// 	long lCount = 0;
// 	if (NULL != pPlugins)
// 	{
// 		pPlugins->get_Count(&lCount);
// 	}
//	
//	IWMSPlugin*           pPlugin = NULL;
//	IDispatch*            pDispatch = NULL;
//	CComVariant           varIndex;
//	
//	IWMSAdminAnonUser*    pAdminAnonUser = NULL;
//	
//	for (long i = 0; i < lCount; ++i)
//	{
//		varIndex = i;
//		hr = pPlugins->get_Item(varIndex, &pPlugin);
//		if (FAILED(hr) || pPlugin == NULL)
//		{
//			continue;
//		}
//		
//		hr = pPlugin->get_CustomInterface(&pDispatch);
//		if (FAILED(hr) || pDispatch == NULL)
//		{
//			pPlugin->Release();
//			continue;
//		}
//		
//		hr = pDispatch->QueryInterface(IID_IWMSAdminAnonUser, (void**)&pAdminAnonUser);
//		if (FAILED(hr) || pAdminAnonUser == NULL)
//		{
//			pPlugin->Release();
//			pDispatch->Release();
//			continue;
//		}
//		
//		hr = pPlugin->put_Enabled(VARIANT_TRUE);
//		
//		LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC,  "put_Enabled. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
//		
//		pPlugin->Release();
//		pDispatch->Release();
//		pAdminAnonUser->Release();
//		break;
//	}
//	pPlugins->Release();
}

/*=============================================================================
函 数 名： ProcTimerPpFailed
功    能： 定时开启直播发布点处理
算法实现： 
全局变量： 
参    数：  CMessage * const pcMsg
返 回 值： 无
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/01/07  4.7		    zhangli               create
=============================================================================*/
void CChnInst::ProcTimerPpFailed(CMessage * const pcMsg)
{
	if (NULL == m_pPubPoint)
	{
		return;
	}

	TFileInfo tFileInfo;
	if (RP_OK != RPGetFileInfo(MAXNUM_RECORDER_CHNNL, m_achFullName, &tFileInfo))
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "[PublishOneFile] Get FileInfo Failed!\n");

		SetTimer(EV_REC_PUBLISHPOINT_TIMER, CHECK_PUBLISHPOINT_INTERVAL);
		return;
	}

	if (tFileInfo.m_dwSize/1024 < g_tCfg.m_wMinSizeForBroadcast)
	{
		LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC,  "file(%dKB) is too small to start pp,dur:%d\n", tFileInfo.m_dwSize/1024, tFileInfo.m_dwDuration);

		SetTimer(EV_REC_PUBLISHPOINT_TIMER, CHECK_PUBLISHPOINT_INTERVAL);
		return;
	}
	
	//开启发布点
	HRESULT hr = 0;
	if (NULL == m_pBCPubPoint)
	{
		hr = m_pPubPoint->QueryInterface(IID_IWMSBroadcastPublishingPoint, (void **)&m_pBCPubPoint);
		if (FAILED(hr) || NULL == m_pBCPubPoint) 
		{
			LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, 
				"retrieve a pointer to an IWMSBroadcastPublishingPoint interface failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
			return;
		}
	}	

	////////start publish point /////////////////////////////////////////////////////////
	WMS_BROADCAST_PUBLISHING_POINT_STATUS ppsStatus;
	hr = m_pBCPubPoint->get_BroadcastStatus(&ppsStatus);
	if (FAILED(hr)) 
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, 
			"Retrieve the current status of the publishing point failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
	}
	else if (ppsStatus == WMS_BROADCAST_PUBLISHING_POINT_STOPPED)
	{
		hr = m_pBCPubPoint->Start();
		if (FAILED(hr))
		{
			SetTimer(EV_REC_PUBLISHPOINT_TIMER, CHECK_PUBLISHPOINT_INTERVAL);

			LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "start publishing point failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_RECEQP_REC,  "start publishing point succeeded,size:%d,dur:%d\n", tFileInfo.m_dwSize/1024,tFileInfo.m_dwDuration);
		}
	}
}

/*=============================================================================
函 数 名： ProcRecorderQuit
功    能： 退出录像
算法实现： 
全局变量： 
参    数：  CMessage * const pcMsg
返 回 值： 无
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/01/07  4.7		    zhangli               create
=============================================================================*/
void CChnInst::ProcRecorderQuit(CMessage * const pcMsg)
{
	//立即发布清除发布点
	if (PUBLISH_MODE_IMMED == m_byPublishMode)
	{
		DeleteBrdPubPoint();
		DeleteOneLiveSource();
		DeletePlayList();
	}

	// 录完发布和立即发布
	if(PUBLISH_MODE_FIN == m_byPublishMode || PUBLISH_MODE_IMMED == m_byPublishMode)
	{
		if (g_tCfg.m_bOpenDBSucceed)
		{
			PublishOneFile(m_achFullName);
		}
	}

	if (g_pPubPoints != NULL)
	{
		g_pPubPoints->Release();
		g_pPubPoints = NULL;
	}
	
	if (g_pServer != NULL)
	{
		g_pServer->Release();
		g_pServer = NULL;
	}
	
	CoUninitialize();

	//释放信号量
	OspSemGive(g_hSemQuit);
}

/*=============================================================================
函 数 名： DeleteBrdPubPoint
功    能： 删除直播发布点
算法实现： 
全局变量： 
参    数：  CMessage * const pcMsg
返 回 值： 无
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/01/07  4.7		    zhangli               create
=============================================================================*/
void CChnInst::DeleteBrdPubPoint()
{
	HRESULT hr = S_OK;

	if (m_pBCPubPoint != NULL)
	{
		hr = m_pBCPubPoint->Stop();
		if (FAILED(hr))
		{
			LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "Stop publishing point failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		}

		m_pBCPubPoint->Release();
		m_pBCPubPoint = NULL;
	}
	
	if (m_pPubPoint != NULL)
	{
		m_pPubPoint->Release();
		m_pPubPoint = NULL;
	}

	if (g_pPubPoints != NULL)
	{
		CComVariant varIndex(m_achOrignalFileName);
		hr = g_pPubPoints->Remove(varIndex);
		if (hr == RPC_S_SERVER_UNAVAILABLE && InitCOMServer())	// RPC_S_SERVER_UNAVAILABLE 与服务器断开
		{
			LogPrint(LOG_LVL_WARNING, MID_RECEQP_REC,  "DeleteBrdPubPoint again,chnnlidx:%d!\n", GetInsID());
			hr = g_pPubPoints->Remove(varIndex);
		}
		if (FAILED(hr))
		{
			LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "Remove publishing point failed. errorcode:%x,chnnlidx:%d,m_achOrignalFileName:%s\n", 
				hr, GetInsID(), m_achOrignalFileName);
		}
	}
}

/*=============================================================================
函 数 名： CreatePlayList
功    能： 创建播放列表
算法实现： 
全局变量： 
参    数：  无
返 回 值： 无
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/02/01  4.7		    zhangli               create
=============================================================================*/
BOOL32 CChnInst::CreatePlayList()
{
	if (g_pServer == NULL)
	{
		return FALSE;
	}
	BOOL32 bIsOk = FALSE;

	HRESULT          hr = 0;
	IXMLDOMDocument  *pPlaylist = NULL;
	CComVariant varPath;
	IXMLDOMNode     *pPriority_Child = NULL;
	CComBSTR bstrVersion;
	CComBSTR bstrName;
	CComBSTR bstrNamespaceURI;
	IXMLDOMElement   *pElement_Smil = NULL;
	IXMLDOMElement   *pElement_Seq = NULL;
	IXMLDOMNode      *pNode = NULL;
	IXMLDOMNode      *pRoot = NULL;
	IXMLDOMNode      *pProc_Inst = NULL;
	IXMLDOMElement   *pElement_Media = NULL;

	// Create the playlist object.
	hr = g_pServer->CreatePlaylist(&pPlaylist);
	if (hr == RPC_S_SERVER_UNAVAILABLE && InitCOMServer()) // RPC_S_SERVER_UNAVAILABLE = 0x800706ba 与服务器断开
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "CreatePlaylist failed, try again,chnIdx:%d\n", GetInsID());

		if (pPlaylist != NULL)
		{
			pPlaylist->Release();
			pPlaylist = NULL;
		}
		hr = g_pServer->CreatePlaylist(&pPlaylist);
	}
	if (FAILED(hr) || pPlaylist == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[CreatePlayList]CreatePlaylist failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		return FALSE;
	}
	// Create the processing instruction node.
	bstrName  = "wsx";
	bstrNamespaceURI = "";
	hr = pPlaylist->createNode((CComVariant)NODE_PROCESSING_INSTRUCTION, bstrName, bstrNamespaceURI, &pProc_Inst);
	if (FAILED(hr))
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[CreatePlayList]createNode failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		goto exit;
	}
    
	// Add the processing instruction to the file structure.
	
	hr = pPlaylist->appendChild(pProc_Inst, &pPriority_Child);
	if (FAILED(hr) || pPriority_Child == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[CreatePlayList]appendChild failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		goto exit;
	}
	
	bstrVersion = "version = 1.0";
	pProc_Inst->put_text(bstrVersion);
	
	// Create and add the root node of the playlist.
	bstrName = "smil";
	hr = pPlaylist->createElement(bstrName, &pElement_Smil);
	if (FAILED(hr) || pElement_Smil == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[CreatePlayList]createElement failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		goto exit;
	}
	
	
	hr = pPlaylist->appendChild(pElement_Smil, &pRoot);
	if (FAILED(hr) || pRoot == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[CreatePlayList]appendChild failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		goto exit;
	}
    

	bstrName = "seq";
	hr = pPlaylist->createElement(bstrName, &pElement_Seq);
	if (FAILED(hr) || pElement_Seq == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[CreatePlayList]createElement(seq) failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		goto exit;
	}

	if (pRoot != NULL)
	{
		pRoot->Release();
		pRoot = NULL;
	}
	hr = pElement_Smil->appendChild(pElement_Seq, &pRoot);


	// Create a media element and add a src attribute.
	bstrName = "media";
	hr = pPlaylist->createElement(bstrName, &pElement_Media);
	if (FAILED(hr) || pElement_Media == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[CreatePlayList]createElement 2 failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		goto exit;
	}
	
	bstrName = "src";
	varPath = m_achFullName;
	hr = pElement_Media->setAttribute(bstrName, varPath);
	if (FAILED(hr))
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[CreatePlayList]setAttribute failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		goto exit;
	}
    
	// Add the media element to the root node.
	hr = pRoot->appendChild(pElement_Media, &pNode);
	if (FAILED(hr))
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[CreatePlayList]appendChild 2 failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		goto exit;
	}
	memset(m_achPlayListName, 0, sizeof(m_achPlayListName));
	sprintf(m_achPlayListName, "%s.wsx", m_achFullName);
	// Save the playlist.
	varPath = m_achPlayListName;
	hr = pPlaylist->save(varPath);
	if (FAILED(hr))
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[CreatePlayList]save failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		goto exit;
	} 

	bIsOk = TRUE;
	goto exit;

exit:
	if (pPlaylist != NULL)	
	{
		pPlaylist->Release();
	}
	
	if (pPriority_Child != NULL)
	{
		pPriority_Child->Release();
	}

	if (pElement_Smil != NULL)
	{
		pElement_Smil->Release();
	}
	if (pNode != NULL)
	{
		pNode->Release();
	}
	if (pRoot != NULL)
	{
		pRoot->Release();
	}
	if (pProc_Inst != NULL)
	{
		pProc_Inst->Release();
	}

	if (pElement_Media != NULL)
	{
		pElement_Media->Release();
	}

	return bIsOk;
}

/*=============================================================================
函 数 名： DeletePlayList
功    能： 删除播放列表
算法实现： 
全局变量： 
参    数：  无
返 回 值： 无
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/02/01  4.7		    zhangli               create
=============================================================================*/
void CChnInst::DeletePlayList()
{
	s8 szPath[MAX_FILE_NAME_LEN] = {0};
	s8 szName[MAX_FILE_NAME_LEN] = {0};
	g_cRecApp.SplitPath(m_achPlayListName, szPath, szName, TRUE);
	RPDeleteFile(szPath, szName, TRUE);
}

/*=============================================================================
函 数 名： AddToPlayList
功    能： 添加一条播放记录到播放列表
算法实现： 
全局变量： 
参    数：  无
返 回 值： 无
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/02/01  4.7		    zhangli               create
=============================================================================*/

void CChnInst::AddToPlayList()
{
	if (m_pBCPubPoint == NULL)
	{
		return;
	}

	HRESULT hr = 0;
	CComBSTR bstrElementName;
	CComBSTR bstrAttrName;

	IXMLDOMElement  *pElement_Media = NULL;
	IXMLDOMElement  *pElement_Seq = NULL;
	IXMLDOMNode     *pNode_Seq = NULL;
	IXMLDOMElement  *pElement_Smil = NULL;
	IXMLDOMNode     *pPriority_Child = NULL;
	CComVariant     varAttr;	
	IWMSPlaylist    *pPlaylist = NULL;

	hr = m_pBCPubPoint->get_SharedPlaylist(&pPlaylist);
	if (FAILED(hr) || pPlaylist == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[AddPlayList]get_SharedPlaylist failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		return;
	}
	
	IXMLDOMNodeList *pNodeList = NULL;
	IXMLDOMNode     *pNode = NULL;
	IXMLDOMNode     *pOldNode = NULL;

	hr = pPlaylist->get_documentElement(&pElement_Smil);
	if (FAILED(hr) || pElement_Smil == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[AddPlayList]get_documentElement failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		goto exit;
	}

	hr = pElement_Smil->get_firstChild(&pNode_Seq);
	if (FAILED(hr) || pNode_Seq == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[AddPlayList]get_firstChild failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		goto exit;
	}

	hr = pNode_Seq->QueryInterface(&pElement_Seq);
	if (FAILED(hr) || pElement_Seq == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[AddPlayList]QueryInterface failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		goto exit;
	}

	bstrElementName = "media";
	hr = pPlaylist->getElementsByTagName(bstrElementName, &pNodeList);
	if (SUCCEEDED(hr) && pNodeList != NULL)
	{
		long lCount = 0;
		hr = pNodeList->get_length(&lCount);
		if (SUCCEEDED(hr) && lCount > 5)
		{
			hr = pNodeList->get_item(0, &pNode);
			if (SUCCEEDED(hr))
			{
				hr = pElement_Seq->removeChild(pNode, &pOldNode);
			}
		}
	}

	hr = pPlaylist->createElement(bstrElementName, &pElement_Media);
	if (FAILED(hr))
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[AddPlayList]createElement failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		goto exit;
	}

	// Set the src attribute for the media element.
	bstrAttrName = "src";
	varAttr = m_achFullName;
	hr = pElement_Media->setAttribute(bstrAttrName, varAttr);
	if (FAILED(hr))
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[AddPlayList]setAttribute failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
		goto exit;
	}
	
	pElement_Seq->appendChild((IXMLDOMNode *)pElement_Media, &pPriority_Child);
	if (FAILED(hr))
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC,  "[AddPlayList]appendChild failed. errorcode:%x,chnnlidx:%d\n", hr, GetInsID());
	}

	goto exit;

exit:
	if (pElement_Media != NULL)
	{
		pElement_Media->Release();
	}
	if (pElement_Seq != NULL)
	{
		pElement_Seq->Release();
	}
	if (pPriority_Child != NULL)
	{
		pPriority_Child->Release();
	}
	if (pPlaylist != NULL)
	{
		pPlaylist->Release();
	}
	if (pNode_Seq != NULL)
	{
		pNode_Seq->Release();
	}
	if (pElement_Smil != NULL)
	{
		pElement_Smil->Release();
	}
}

API void precmsg(void)
{
    g_bPrintRecLog = TRUE;
	logenablemod(MID_RECEQP_REC);
	logenablemod(MID_RECEQP_COMMON);
	logenablemod(MID_RECEQP_PLAY);
	logenablemod(MID_RECEQP_ASFLIB);
	logenablemod(MID_RECEQP_RPCTRL);
	logenablemod(MID_RECEQP_RPCTRL);
}

API void nprecmsg(void)
{
    g_bPrintRecLog = FALSE;
}

API void showdebuginfo(void)
{
    OspPrintf(1, 0, "m_byPlayChnNum                 :%d\n", g_tCfg.m_byPlayChnNum);
	OspPrintf(1, 0, "m_byRecChnNum                  :%d\n", g_tCfg.m_byRecChnNum);
	OspPrintf(1, 0, "m_dwMaxRecFileLen              :%d\n", g_tCfg.m_dwMaxRecFileLen);
	OspPrintf(1, 0, "m_bIsWriteAsfWhenLoseFrame     :%d\n", g_tCfg.m_bIsWriteAsfWhenLoseFrame	);
	OspPrintf(1, 0, "m_bIsNeedKeyFrameWhenLoseFrame :%d\n", g_tCfg.m_bIsNeedKeyFrameWhenLoseFrame);
	OspPrintf(1, 0, "m_byAudioBuffSize              :%d\n", g_tCfg.m_byAudioBuffSize);
	OspPrintf(1, 0, "m_byVideoBuffSize              :%d\n", g_tCfg.m_byVideoBuffSize);
	OspPrintf(1, 0, "m_byDsBuffSize                 :%d\n", g_tCfg.m_byDsBuffSize);
	OspPrintf(1, 0, "m_byLeftBuffSize               :%d\n", g_tCfg.m_byLeftBuffSize);
	OspPrintf(1, 0, "m_byMinSizeForBroadcast        :%d\n", g_tCfg.m_wMinSizeForBroadcast);
}