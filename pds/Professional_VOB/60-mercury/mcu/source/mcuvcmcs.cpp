/*****************************************************************************
   模块名      : mcu_new
   文件名      : mcuvcmcs.cpp
   相关文件    : mcuvc.h
   文件实现功能: MCU业务交换函数
   作者        : 胡昌威
   版本        : V2.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/06  2.0         胡昌威      创建
   2005/02/19  3.6         万春雷      级联修改、与3.5版本合并
******************************************************************************/

#include "evmcumcs.h"
#include "evmcumt.h"
#include "evmcueqp.h"
#include "evmcu.h"
#include "evmcutest.h"
#include "evmcudcs.h"
#include "evmp.h"
#include "mcuvc.h"
#include "mcsssn.h"
#include "eqpssn.h"
//#include "mcuerrcode.h"
#include "mtadpssn.h"
#include "commonlib.h"
//#include "mcuutility.h"
//#include "mpmanager.h"
//#include "radiusinterface.h"

#ifdef _VXWORKS_
    #include "brddrvLib.h"
    #include <inetLib.h>
#endif

// MPC2 支持
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        #include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif


/*====================================================================
    函数名      ：DaemonProcMcuMcsConnectedNotif
    功能        ：会控注册成功处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/18    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::DaemonProcMcuMcsConnectedNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8		byInstId = *( u8 * )cServMsg.GetMsgBody();
	TMcsRegInfo	tRegReq = *( TMcsRegInfo * )( cServMsg.GetMsgBody() + sizeof( u8 ) );
	TLogicalChannel tLogicalChannel;
    
	//设置前向视频通道
	tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( tRegReq.GetMcsIpAddr() );
	tLogicalChannel.m_tRcvMediaChannel.SetPort( tRegReq.GetStartPort() );
	tLogicalChannel.m_tRcvMediaCtrlChannel.SetIpAddr( tRegReq.GetMcsIpAddr());
	tLogicalChannel.m_tRcvMediaCtrlChannel.SetPort( tRegReq.GetStartPort() + 1 );
    tLogicalChannel.m_tSndMediaCtrlChannel.SetIpAddr( 0 );	//设0交换时绑定用
	tLogicalChannel.m_tSndMediaCtrlChannel.SetPort( 0 );
	g_cMcuVcApp.SetMcLogicChnnl( byInstId, MODE_VIDEO, tRegReq.GetVideoChnNum(), &tLogicalChannel );
    
	//设置前向音频通道
	tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( tRegReq.GetMcsIpAddr() );
	tLogicalChannel.m_tRcvMediaChannel.SetPort( tRegReq.GetStartPort() + 2 );
	tLogicalChannel.m_tRcvMediaCtrlChannel.SetIpAddr( tRegReq.GetMcsIpAddr());
	tLogicalChannel.m_tRcvMediaCtrlChannel.SetPort( tRegReq.GetStartPort() + 3 );
    tLogicalChannel.m_tSndMediaCtrlChannel.SetIpAddr( 0 );	//设0交换时绑定用
	tLogicalChannel.m_tSndMediaCtrlChannel.SetPort( 0 );
	g_cMcuVcApp.SetMcLogicChnnl( byInstId, MODE_AUDIO, tRegReq.GetVideoChnNum(), &tLogicalChannel );

	//设置前向双流通道
	tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( tRegReq.GetMcsIpAddr() );
	tLogicalChannel.m_tRcvMediaChannel.SetPort( tRegReq.GetStartPort() + 4 );
	tLogicalChannel.m_tRcvMediaCtrlChannel.SetIpAddr( tRegReq.GetMcsIpAddr());
	tLogicalChannel.m_tRcvMediaCtrlChannel.SetPort( tRegReq.GetStartPort() + 5 );
    tLogicalChannel.m_tSndMediaCtrlChannel.SetIpAddr( 0 );	//设0交换时绑定用
	tLogicalChannel.m_tSndMediaCtrlChannel.SetPort( 0 );
	g_cMcuVcApp.SetMcLogicChnnl( byInstId, MODE_SECVIDEO, tRegReq.GetVideoChnNum(), &tLogicalChannel );

	//save info
	g_cMcuVcApp.SetMcConnected( byInstId, TRUE );
	g_cMcuVcApp.SetMcsRegInfo( byInstId, tRegReq );
    // 新增 处理在线MCS的IP信息  [04/24/2012 liaokang]
	if( byInstId <= MAXNUM_MCU_MC )
	{
		g_cMcuAgent.AddRegedMcsIp( tRegReq.GetMcsIpAddr() );
	}
}


/*====================================================================
    函数名      ：DaemonProcMcuMcsDisconnectedNotif
    功能        ：会控断链通知处理 
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/18    1.0         LI Yi         创建
	03/11/22    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::DaemonProcMcuMcsDisconnectedNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8	byInstId = *( u8 * )cServMsg.GetMsgBody();

	//处理所有会议的保护状态
	TConfInfo *ptConfInfo;
	TConfProtectInfo *ptConfProtectInfo;

    CServMsg cLockMsg;
    TMcu tMcu;
    tMcu.SetMcu(0);
    u8 byLock = 0;
    cLockMsg.SetEventId(MCU_MCU_LOCK_REQ);
    cLockMsg.SetMsgBody((u8*)&tMcu, sizeof(tMcu));
    cLockMsg.CatMsgBody(&byLock, sizeof(byLock));
    CMcuVcInst *pConfInst = NULL;

	for( u8 byLoop = MIN_CONFIDX; byLoop <= MAX_CONFIDX; byLoop++ )
	{
        pConfInst = g_cMcuVcApp.GetConfInstHandle( byLoop );
        if(NULL == pConfInst)
        {
            continue;
        }

		ptConfInfo = &pConfInst->m_tConf;
        if( ptConfInfo->m_tStatus.IsOngoing())
		{
			 ptConfProtectInfo = g_cMcuVcApp.GetConfProtectInfo( byLoop );

		     if( ptConfInfo->m_tStatus.GetProtectMode() == CONF_LOCKMODE_LOCK && 
				 ptConfProtectInfo->GetLockedMcSsnId()== byInstId )
			 {
				 //解除锁定
                 ptConfInfo->m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );
				 ptConfProtectInfo->SetLockByMcs(0);
                 
                 // guzh [7/25/2006] 同时，如果该MCS把会议设置为无主席模式没有释放，则取消无主席模式
                 if (ptConfInfo->m_tStatus.m_tConfMode.IsNoChairMode())
                 {
                    ptConfInfo->m_tStatus.SetNoChairMode(FALSE);
                 }
                 
                 // xsl [7/26/2006] 解除下级mcu锁定     
                 cLockMsg.SetConfId(ptConfInfo->GetConfId());
                 g_cMcuVcApp.SendMsgToConf(pConfInst->m_byConfIdx, MCS_MCU_LOCKSMCU_REQ, 
                                           cLockMsg.GetServMsg(), cLockMsg.GetServMsgLen());

                 // zbq [11/22/2007] 解除点名模式
				 // miaoqingsong [20120111] 如果该MCS把会议设置为点名业务模式，断链时则取消会议点名业务，
				 //                 “清除点名模式”在取消会议点名业务时做，所以这里注掉！
                 if ( ROLLCALL_MODE_NONE != ptConfInfo->m_tStatus.GetRollCallMode() )
                 {
					 CServMsg cStopRollMsg;
					 cStopRollMsg.SetEventId(MCS_MCU_STOPROLLCALL_REQ);
					 cStopRollMsg.SetConfId(ptConfInfo->GetConfId());
					 
					 g_cMcuVcApp.SendMsgToConf( pConfInst->m_byConfIdx, MCS_MCU_STOPROLLCALL_REQ, 
					     cStopRollMsg.GetServMsg(), cStopRollMsg.GetServMsgLen() );
                     //ptConfInfo->m_tStatus.SetRollCallMode(ROLLCALL_MODE_NONE);
                 }
			 }
			 
             if( ptConfInfo->m_tStatus.GetProtectMode() == CONF_LOCKMODE_NEEDPWD )
			 {
				 //下次登录需重新输入密码
                 ptConfProtectInfo->SetMcsPwdPassed(byInstId, FALSE);
			 }
		}
	}

#if defined(_8KH_) ||  defined(_8KE_) || defined(_8KI_)
	//会控断链时，看是否当前正在抓包的mcs断链，是的话停止抓包，清抓包状态
	emNetCapState emCurNetCapState = g_cMcuVcApp.GetNetCapStatus();
	//校验状态
	if (emCurNetCapState == emNetCapStart && g_cMcuVcApp.GetOprNetCapMcsInstID() == byInstId )
	{
		//停止抓包
		ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_MCS,"[DaemonProcMcuMcsDisconnectedNotif]stop netcap!\n");
		g_cMcuVcApp.StopNetCap();
		//状态通知
		g_cMcuVcApp.SetNetCapStatus(emNetCapIdle);
		u8 byCurState = (u8)emNetCapIdle;
		cServMsg.SetMsgBody(&byCurState,sizeof(u8));
		SendMsgToAllMcs(MCU_MCS_NETCAPSTATUS_NOTIFY,cServMsg);

	}

	if( g_cMcsSsnApp.GetCurrentDebugMode() != emDebugModeNone && 
		g_cMcsSsnApp.GetCurrentDebugModeInsId() == byInstId )
	{
		g_cMcsSsnApp.SetCurrentDebugMode( emDebugModeNone );
		g_cMcsSsnApp.SetCurrentDebugModeInsId( 0 );
	#ifdef _LINUX_
		SetTelnetDown();
		SetSshDown();
	#endif
		u8 byCurMode = (u8)emDebugModeNone;
		cServMsg.SetMsgBody(&byCurMode,sizeof(u8));		
		SendMsgToAllMcs(MCU_MCS_OPENDEBUGMOED_NOTIFY,cServMsg);
	}
#endif

	//stop switch to MC
	g_cMcuVcApp.BroadcastToAllConf( MCU_MCSDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
	
	//clear info
	g_cMcuVcApp.SetMcConnected( byInstId, FALSE );
    // 新增 处理在线MCS的IP信息  [04/24/2012 liaokang]
    TMcsRegInfo tMcsRegInfo;
    g_cMcuVcApp.GetMcsRegInfo( byInstId, &tMcsRegInfo );
    g_cMcuAgent.DeleteRegedMcsIp( tMcsRegInfo.GetMcsIpAddr() );
}

/*====================================================================
    函数名      ：ProcMcsMcuCreateConfReq
    功能        ：创建会议处理函数 
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/01    1.0         LI Yi         创建
	04/06/03    3.0         胡昌威        修改
	05/12/20	4.0			张宝卿		  集成T120
====================================================================*/
void CMcuVcInst::ProcMcsMcuCreateConfReq( const CMessage * pcMsg )
{
	TConfMode  tConfMode;
    u8 byMtNum = 0;
    u8 byCreateMtId;
    u8 byOldInsId = 0;
    u16 wErrCode = 0;

    CServMsg  cServMsg( pcMsg->content, pcMsg->length );

    TConfInfo tTmpConf = *(TConfInfo *)cServMsg.GetMsgBody();
	
    BOOL32 bConfFromFile = tTmpConf.m_tStatus.IsTakeFromFile();
    tTmpConf.m_tStatus.SetTakeFromFile(FALSE);

	//  [12/29/2009 pengjie] Modify AES加密会议，如果会议码率大于4M，将其限制为4M（性能原因）
	if( tTmpConf.GetBitRate() > (4096 - GetAudioBitrate(tTmpConf.GetMainAudioMediaType())) && 
		tTmpConf.GetMediaKey().GetEncryptMode() == CONF_ENCRYPTMODE_AES )
	{
		tTmpConf.SetBitRate(4096 - GetAudioBitrate(tTmpConf.GetMainAudioMediaType()));
	}
	if( tTmpConf.GetSecBitRate() >= (4096 - GetAudioBitrate(tTmpConf.GetSecAudioMediaType())) && 
		tTmpConf.GetMediaKey().GetEncryptMode() == CONF_ENCRYPTMODE_AES )
	{
		tTmpConf.SetSecBitRate(0);
	}
	// End Modify  


	//先解析tConfInfoEx信息，BAS判资源是否足够会用到[12/29/2011 chendaiwei]
	TConfInfoEx tConfInfoEx;
	if(GetConfExInfoFromMsg(cServMsg,tConfInfoEx))
	{
		tTmpConf.SetHasConfExFlag(TRUE);
	}
			
	

	switch( CurState() )
	{
	case STATE_WAITFOR:
		// 处理Mcu支持的外设能力和会议外设需求能力
		{                        
			TEqpCapacity tConfEqpCap;
			TEqpCapacity tMcuEqpCap;			
			u8  byConfIdx = cServMsg.GetConfIdx();
			
			g_cMcuVcApp.GetConfEqpDemand( tTmpConf, tConfEqpCap);
			if( tConfEqpCap.m_tTvCap.IsNeedEqp() ) // 电视墙的数目
			{
				u16 wAliasLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
				s8 *pMsgBuf = (s8*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16) + wAliasLen);
				TMultiTvWallModule *ptMultiTvWallModule = (TMultiTvWallModule *)pMsgBuf;
				tConfEqpCap.m_tTvCap.SetNeedChannles( ptMultiTvWallModule->GetTvModuleNum() );// 电视墙数目
			}

			g_cMcuVcApp.GetMcuEqpCapacity( tMcuEqpCap );
			BOOL32 bRet = g_cMcuVcApp.AnalyEqpCapacity( tTmpConf/*m_tConf*/,tConfInfoEx, tConfEqpCap, tMcuEqpCap );            

			//对于预约会议，不需要比较外设资源是否足够，可以直接开启，真正开启的时候再比较
			if ( !tTmpConf.m_tStatus.IsOngoing())
			{
				bRet = TRUE;
			}

            // [12/28/2006-zbq] N+1备份回滚或恢复的会议，认为外设已经通过，直接创会
			//zjj20121023 Bug00110525 N+1备份回滚或恢复的会议条件修改，原来的直接判是否MCU_NPLUS_IDLE太粗糙
			if( !bRet && CONF_CREATE_NPLUS != cServMsg.GetSrcMtId()/*MCU_NPLUS_IDLE == g_cNPlusApp.GetLocalNPlusState()*/ )
			{
				// 设置定时器去重新开会		
				if( bConfFromFile )
				{
                    // 超过最大尝试次数
					if( MAX_TIMES_CREATE_CONF < m_wRestoreTimes ) 
					{
						if (!g_cMcuVcApp.RemoveConfFromFile(tTmpConf.GetConfId()))
						{
							ConfPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMcsMcuCreateConfReq] remove conf from file failed\n");
						}
                        else
                        {
                            ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "[ProcMcsMcuCreateConfReq] conf.%s has been remove from file due to RestoreTimes.%d\n", tTmpConf.GetConfName(), m_wRestoreTimes);
                        }
						g_cMcuVcApp.SetConfStore(byConfIdx-MIN_CONFIDX, FALSE); // 以后不再恢复
						NEXTSTATE(STATE_IDLE);
                        m_wRestoreTimes = 1;
					}
					else
					{
                        SetTimer(MCU_WAITEQP_CREATE_CONF_NOTIFY, m_wRestoreTimes <= 3 ? 10000 : m_wRestoreTimes*LEN_WAIT_TIME, (u32)byConfIdx );// 延时
                        m_wRestoreTimes++;
					}					
				}				
				
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Conference %s create failed because NO enough equipment(%d)!\n", tTmpConf.GetConfName(), m_wRestoreTimes );
				return;
			}
			else
			{
				if( bConfFromFile )
				{
                    // guzh [4/10/2007] 这里暂时不能从文件从删除，否则会导致文件丢失
                    /*
					// 从文件中移除
					if (!g_cMcuVcApp.RemoveConfFromFile(tTmpConf.GetConfId()))
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMcsMcuCreateConfReq] remove conf from file failed\n");
					}
                    */
					g_cMcuVcApp.SetConfStore(byConfIdx-MIN_CONFIDX, FALSE);// 以后不再恢复

					NEXTSTATE(STATE_IDLE);

					if ( g_cMSSsnApp.IsDoubleLink() )
					{
						//恢复会议的时候，通知主备备份恢复的会议
						ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"conf:%s Set RecoverTimer:MCUVC_RECOVERCONF_SYN_TIMER\n",tTmpConf.GetConfName());
						CServMsg cMsgToDeamon;
						u8 byStart = 1;//告诉deamoninstance开启timer,直接给Msmanager发的话可能会导致很短时间内多次同步
						cMsgToDeamon.SetMsgBody((u8*)&byStart,sizeof(byStart));
						post(MAKEIID(GetAppID(),CInstance::DAEMON),MCUVC_RECOVERCONF_SYN_TIMER,cMsgToDeamon.GetServMsg(),cMsgToDeamon.GetServMsgLen());
					}
									
                    m_wRestoreTimes = 1;

                    KillTimer(MCU_WAITEQP_CREATE_CONF_NOTIFY);
				
				}
			}
		}  
		//lint -fallthrough
		// 此处没有Break，因为当Mcu支持会议需求的外设时，继续执行下面的程序来创会
		
	case STATE_IDLE:
		{
			//清空实例
			ClearVcInst();
			cServMsg.SetErrorCode(0);       
			
			//非周期性预约会议，根据StartTime判断是一般预约会议还是即时会议
			//周期性预约会议处理放在后面做
			if ( !tTmpConf.IsCircleScheduleConf() )
			{
				//设置开始时间
				if( 0 == tTmpConf.GetStartTime() ||
					MT_MCU_CREATECONF_REQ == pcMsg->event )
				{
					tTmpConf.m_tStatus.SetOngoing();
					tTmpConf.SetStartTime( time( NULL ) );
				}
				
				// zgc [5/18/2007] 判断预约会议开始时间
				if( tTmpConf.GetStartTime() <= time(NULL) + 1*60 )
				{
					tTmpConf.m_tStatus.SetOngoing();
				}
			}
			
			// guzh [4/16/2007] 判断即时会议开始时间
			if ( tTmpConf.m_tStatus.IsOngoing() &&
				tTmpConf.GetStartTime() > time(NULL)+1*60 &&
				// zbq [08/02/2007] N+1备份恢复的会议直接召开
				CONF_CREATE_NPLUS != cServMsg.GetSrcMtId() )
			{
				cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );            
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s failure because start time error!\n", tTmpConf.GetConfName() );
				return;
			}
			
			// 已达最大会议数，创会时不考虑是否级联
			if ( g_cMcuVcApp.IsConfNumOverCap(tTmpConf.m_tStatus.IsOngoing(), tTmpConf.GetConfSource()) )
			{
				// guzh [6/18/2007] 上级MCU呼叫，配置为允许单个及时会议的情况，通知本级MCS
				/*if (1 == g_cMcuVcApp.GetMaxOngoingConfNum() &&
				cServMsg.GetSrcMtId() == CONF_CREATE_MT)
				{
				//处理终端列表
				u16 wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
				char* pszAliaseBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16));        
				TMtAlias tMtAliasArray[MAXNUM_CONF_MT];
				u16 awMtDialRate[MAXNUM_CONF_MT];
				UnPackTMtAliasArray( pszAliaseBuf, wAliasBufLen, &tTmpConf, 
				tMtAliasArray, awMtDialRate, byMtNum );
				
				  CServMsg cNotifMsg;
				  u32 dwMMcuIp = tMtAliasArray[0].m_tTransportAddr.GetNetSeqIpAddr();
				  
					cNotifMsg.SetMsgBody((u8*)&dwMMcuIp, sizeof(u32));
					cNotifMsg.SetErrorCode( ERR_MCU_CONFNUM_EXCEED );
					SendMsgToAllMcs(MCU_MCS_MMCUCALLIN_NOTIF, cNotifMsg);              
					}
					else
				{*/
                cServMsg.SetErrorCode( ERR_MCU_CREATECONFERROR_CONFISHOLDING );
				//}
				//[2011/11/29/zhangli]调用CreateConfSendMcsNack接口提示，否则预约会议无法提示，下同
				CreateConfSendMcsNack(byOldInsId, ERR_MCU_CREATECONFERROR_CONFISHOLDING, cServMsg);
//				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				if ( bConfFromFile )
				{
					g_cMcuVcApp.RemoveConfFromFile(tTmpConf.GetConfId());
				}      
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s failure because exceed max conf num!\n", tTmpConf.GetConfName() );
				return;
			}
			
			if (0 == g_cMcuVcApp.GetMpNum() || 0 == g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323))
			{
				cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
//				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				CreateConfSendMcsNack(byOldInsId, ERR_MCU_CONFOVERFLOW, cServMsg);
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s failure because No_Mp or No_MtAdp!\n", tTmpConf.GetConfName() );
				return;
			}
			
			//开辟终端数据区
			if( NULL == m_ptMtTable )
			{
				m_ptMtTable = new TConfMtTable;
				START_CONF_CHECK_MEMORY(m_ptMtTable, cServMsg, tTmpConf);
				m_tApplySpeakQue.Init();
			}
			memset( m_ptMtTable, 0, sizeof( TConfMtTable ) );
// 			 
 			//开辟bas管理类数据区
// 			if ( NULL == m_pcMcuBasMgr )
// 			{
// 				m_pcMcuBasMgr = new CMcuBasMgr;
// 			}
			NewMcuBasMgr();
			ClearBasMgr();
			//        memset( m_pcMcuBasMgr, 0, sizeof( CMcuBasMgr ) );
			
			m_dwSpeakerViewId  = OspTickGet();//102062000;//
			m_dwSpeakerVideoId = m_dwSpeakerViewId+100;//102052024;//
			m_dwVmpViewId      = m_dwSpeakerVideoId+100;
			m_dwVmpVideoId     = m_dwVmpViewId+100;
			m_dwSpeakerAudioId = m_dwVmpVideoId+100;
			m_dwMixerAudioId   = m_dwSpeakerAudioId+100;
			
			//得到创建者
			m_byCreateBy = cServMsg.GetSrcMtId();
			
			m_byConfIdx = g_cMcuVcApp.GetTemConfIdxByE164(tTmpConf.GetConfE164());      
			//预约会议就算存为模板，confidx还是从空闲idx中取一个
			if( 0 == m_byConfIdx || !tTmpConf.m_tStatus.IsOngoing() /*|| tTmpConf.IsCircleScheduleConf() */)
			{
				m_byConfIdx = g_cMcuVcApp.GetIdleConfidx(); 
				if(0 == m_byConfIdx)
				{                      
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s failure for conf full!\n", tTmpConf.GetConfName() );
					return;
				}
			}           
			byOldInsId = g_cMcuVcApp.GetConfMapInsId(m_byConfIdx);
			g_cMcuVcApp.SetConfMapInsId(m_byConfIdx, (u8)GetInsID());
			
			//[chendaiwei 2010/10/17]如果是NPlus创会过滤掉混音的状态,
			//通过设置会议模板智能混音属性来支持智能混音，定制及VAC均暂时不支持
			if ( CONF_CREATE_NPLUS == m_byCreateBy )
			{
				TConfAttrb tAttr = tTmpConf.GetConfAttrb();
				if(tTmpConf.m_tStatus.GetMixerMode() == mcuWholeMix
				    || tTmpConf.m_tStatus.GetMixerMode() == mcuVacWholeMix)
				{
					tAttr.SetDiscussConf(TRUE);			
				}
				else
				{
					tAttr.SetDiscussConf(FALSE);
				}
				
				tTmpConf.m_tStatus.SetMixerMode(mcuNoMix);
				tTmpConf.SetConfAttrb(tAttr);
			}
			//得到会议信息
			
			m_tConf = tTmpConf;    
			m_tConfEx = tConfInfoEx;
			
			//zbq[12/04/2007] 兼容老模板会议帧率定义
			if ( MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
				!m_tConf.IsMainVidUsrDefFPS() )
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[CreateConf] MVidType.%d with FPS.%d is unexpected, adjust it\n",
					m_tConf.GetMainVideoMediaType(), m_tConf.GetMainVidUsrDefFPS() );
				
				//保护给一个普通的满帧
				if ( 0 == m_tConf.GetMainVidUsrDefFPS() )
				{
					m_tConf.SetMainVidUsrDefFPS(30);
				}
				else
				{
					m_tConf.SetMainVidUsrDefFPS(m_tConf.GetMainVidUsrDefFPS());
				}
			}
			if ( MEDIA_TYPE_H264 == m_tConf.GetSecVideoMediaType() &&
				!m_tConf.IsSecVidUsrDefFPS() )
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[CreateConf] SVidType.%d with FPS.%d is unexpected, adjust it\n",
					m_tConf.GetSecVideoMediaType(), m_tConf.GetSecVidUsrDefFPS() );
				
				//保护给一个普通的满帧
				if ( 0 == m_tConf.GetSecVidUsrDefFPS() )
				{
					m_tConf.SetSecVidUsrDefFPS(30);
				}
				else
				{
					m_tConf.SetSecVidUsrDefFPS(m_tConf.GetSecVidUsrDefFPS());
				}
			}
			if ( MEDIA_TYPE_H264 == m_tConf.GetCapSupport().GetDStreamMediaType() &&
				!m_tConf.IsDStreamUsrDefFPS() )
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[CreateConf] DSVidType.%d with FPS.%d is unexpected, adjust it\n",
					m_tConf.GetCapSupport().GetDStreamMediaType(), m_tConf.GetDStreamUsrDefFPS() );
				
				//保护给一个普通的1/3满帧
				if ( 0 == m_tConf.GetDStreamUsrDefFPS() )
				{
					m_tConf.SetDStreamUsrDefFPS(10);
				}
				else
				{
					m_tConf.SetDStreamUsrDefFPS(m_tConf.GetDStreamUsrDefFPS());
				}
			}
			
			//按配置文件，是否调整会议帧率能力
			//与Aethra对通时，务必将FPS配置成10。（修改mcuGeneralParam/ConfFPS）
			u8 byDbgFps = g_cMcuVcApp.GetDbgConfFPS();
			if ( byDbgFps != 0 )
			{
				m_tConf.SetMainVidUsrDefFPS(byDbgFps);
			}
			
			//zbq[11/15/2008] debug增加第二双流能力
			if (MEDIA_TYPE_NULL == m_tConf.GetCapSupportEx().GetSecDSType())
			{
				AddSecDSCapByDebug(m_tConf);
			}
			
			tConfMode = m_tConf.m_tStatus.GetConfMode();
			if(0 == g_cMcuAgent.GetGkIpAddr())
			{
				tConfMode.SetRegToGK(FALSE);
			}
			
			if( m_tConf.m_tStatus.GetCallInterval() < MIN_CONF_CALLINTERVAL )
			{
				tConfMode.SetCallInterval( MIN_CONF_CALLINTERVAL );
			}
			else
			{
				tConfMode.SetCallInterval( m_tConf.m_tStatus.GetCallInterval() );
			}
			if( DEFAULT_CONF_CALLTIMES == m_tConf.m_tStatus.GetCallTimes() )
			{
				tConfMode.SetCallTimes( DEFAULT_CONF_CALLTIMES );
			}
			else if( m_tConf.m_tStatus.GetCallTimes() < MIN_CONF_CALLTIMES )
			{
				tConfMode.SetCallTimes( MIN_CONF_CALLTIMES );
			}
			else
			{
				tConfMode.SetCallTimes( m_tConf.m_tStatus.GetCallTimes() );
			}		
			m_tConf.m_tStatus.SetConfMode( tConfMode );
			
			m_tConf.m_tStatus.SetPrsing( FALSE );
			m_tConfAllMtInfo.Clear();
			m_tConfAllMtInfo.m_tLocalMtInfo.SetConfIdx( m_byConfIdx );
			m_tConfAllMcuInfo.Clear();
			
			// guzh [4/10/2007] 如果不是从会议恢复的文件，则重新生成CConfId，
			// 否则沿用旧的，这样才能覆盖原来的会议信息文件产生唯一会议CConfId
			if ( !bConfFromFile )
			{
				m_tConf.SetConfId( g_cMcuVcApp.MakeConfId(m_byConfIdx, 0, m_tConf.GetUsrGrpId(),
					m_tConf.GetConfSource()) );
			}
			
			//会议信息 逻辑检测
			if ( !IsConfInfoCheckPass(cServMsg, m_tConf, wErrCode))
			{
				CreateConfSendMcsNack( byOldInsId, wErrCode, cServMsg );
				return;
			}
			//会议数据整理
			ConfDataCoordinate(cServMsg, byMtNum, byCreateMtId);


				
			
			//若会议结束模式为无终端自动结束，应该有终端
			if(0 == byMtNum && m_tConf.GetConfAttrb().IsReleaseNoMt())
			{
				ClearHduSchemInfo();
				CreateConfSendMcsNack( byOldInsId, ERR_MCU_NOMTINCONF, cServMsg );
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because on mt in conf!\n", m_tConf.GetConfName() );
				return;
			}
			
			// N+1 备份认为不支持预约会议 [12/20/2006-zbq]
			// 会议处理: 即时会议
			if ( CONF_CREATE_NPLUS == m_byCreateBy ||
				( CONF_CREATE_NPLUS != m_byCreateBy && m_tConf.m_tStatus.IsOngoing() )  )
			{				
//				//清除GK注册信息，即时模板本身注册成功
//				m_tConf.m_tStatus.SetRegToGK( FALSE );
				
				//MCU的外设能力是否充分，及部分外设操作
				if ( !IsEqpInfoCheckPass(cServMsg, wErrCode))
				{
					ClearHduSchemInfo();
					CreateConfEqpInsufficientNack( byCreateMtId, byOldInsId, wErrCode, cServMsg );
					
					if (ERR_MCU_NOENOUGH_HDBAS_CONF == wErrCode)
					{
// 						TMcuHdBasStatus tStatus;
// 						m_pcBasMgr->GetHdBasStatus(tStatus, m_tConf);
// 						
// 						CServMsg cMsg;
// 						cMsg.SetMsgBody((u8*)&tStatus, sizeof(tStatus));
// 						cMsg.SetEventId(MCU_MCS_MAUSTATUS_NOTIFY);
// 						SendMsgToMcs(cServMsg.GetSrcSsnId(), MCU_MCS_MAUSTATUS_NOTIFY, cMsg);
					}
					
					return;
				}	

				NPlusVmpParamCoordinate(cServMsg);

				if( NULL == m_ptSwitchTable )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[instance%d]create confswitchtable\n", GetInsID());
					m_ptSwitchTable = new TConfSwitchTable;
					START_CONF_CHECK_MEMORY(m_ptSwitchTable, cServMsg, tTmpConf);
				}
				
				if( NULL == m_ptConfOtherMcTable )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[instance%d]create TConfOtherMcTable\n", GetInsID());
					m_ptConfOtherMcTable = new TConfOtherMcTable;
					START_CONF_CHECK_MEMORY(m_ptConfOtherMcTable, cServMsg, tTmpConf);
				}
				if (m_ptSwitchTable != NULL)
				{
					memset( m_ptSwitchTable, 0, sizeof( TConfSwitchTable ) );
				}
				
				if (m_ptConfOtherMcTable != NULL)
				{
					memset( m_ptConfOtherMcTable, 0, sizeof( TConfOtherMcTable ) );
					m_ptConfOtherMcTable->Init();
				}
				
				//清除GK注册信息，即时模板本身注册成功
				m_tConf.m_tStatus.SetRegToGK( FALSE );
				
				TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();

                //pgf:卫星分散会议不能开启语音激励和智能混音
                if (!m_tConf.GetConfAttrb().IsSatDCastMode())
                {
                    if (tConfAttrib.IsDiscussConf())
                    {
                        // 顾振华 [4/29/2006] 在这里启动全体混音
                        //tianzhiyong 2010/04/15 增加开启模式
                        if(m_tConf.GetConfAttrbEx().IsSupportAutoVac())
                        {
                            StartMixing(mcuVacWholeMix);
                        }
                        else
                        {
                            StartMixing(mcuWholeMix);
                        }
                    }
                    else
                    {
                        if (m_tConf.GetConfAttrbEx().IsSupportAutoVac()) 
                        {
                            CServMsg cTempServMsg;				
                            cTempServMsg.SetEventId(MCS_MCU_STARTVAC_REQ);				
                            MixerVACReq(cTempServMsg);
                        }
				    }
                }
                else
                {
                    ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, 
                        "[ProcMcsMcuCreateConfReq]SatDCast mode not support Vac or WholeMix!\n");
                }

				
				//应答 
				cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
				cServMsg.SetConfId( m_tConf.GetConfId() );
				if( m_byCreateBy == CONF_CREATE_MCS && !bConfFromFile)
				{
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
				}
				else if ( m_byCreateBy == CONF_CREATE_MT && !bConfFromFile )
				{
					// zbq [07/16/2007] 终端/上级MCU创会成功，此处才可以回ACK
					CServMsg cAckMsg;
					cAckMsg.SetSrcDriId( cServMsg.GetSrcDriId() );
					
					TCapSupport tCapSupport = m_tConf.GetCapSupport();
					
					// FIXME: 此处依赖于创会者在终端列表头的策略
					cAckMsg.SetDstMtId( 1 );
					cAckMsg.SetConfIdx( m_byConfIdx );
					cAckMsg.SetConfId( m_tConf.GetConfId() );
					cAckMsg.SetSrcSsnId( cServMsg.GetSrcSsnId() );
					u8 byIsEncrypt = GETBBYTE(tConfAttrib.GetEncryptMode() != CONF_ENCRYPTMODE_NONE);
					cAckMsg.SetMsgBody(&byIsEncrypt, sizeof(u8));
					cAckMsg.CatMsgBody((u8*)&tCapSupport, sizeof(tCapSupport));
					TMtAlias tAlias;
					tAlias.SetH323Alias(m_tConf.GetConfName());
					cAckMsg.CatMsgBody((u8 *)&tAlias, sizeof(tAlias));
					
					// Ex能力集需要带到本端Mtadp侧，避免出现本端Mtadp侧没有进行能力集更新的时候（即还没有Ex能力时），对端能力集到来，
					// 导致比出的共同双流能力有误
					TCapSupportEx tCapEx = m_tConf.GetCapSupportEx();
					cAckMsg.CatMsgBody( (u8*)&tCapEx, sizeof(tCapEx));
					
					// 组织扩展能力集勾选给Mtadp [12/8/2011 chendaiwei]
					TVideoStreamCap atMSVideoCap[MAX_CONF_CAP_EX_NUM];
					u8 byCapNum = MAX_CONF_CAP_EX_NUM;
					m_tConfEx.GetMainStreamCapEx(atMSVideoCap,byCapNum);
					
					TVideoStreamCap atDSVideoCap[MAX_CONF_CAP_EX_NUM];
					u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;
					m_tConfEx.GetDoubleStreamCapEx(atDSVideoCap,byDSCapNum);
					
					cAckMsg.CatMsgBody((u8*)&atMSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
					cAckMsg.CatMsgBody((u8*)&atDSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
					
					TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//音频能力
					//从会议属性中取出会议支持的音频类型
					m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
					cAckMsg.CatMsgBody((u8*)&atAudioTypeDesc[0], sizeof(TAudioTypeDesc)* MAXNUM_CONF_AUDIOTYPE);

					// guzh [7/19/2007] 发送给创建者
					SendMsgToMt(CONF_CREATE_MT, MCU_MT_CREATECONF_ACK, cAckMsg);
				}
				
				//打印信息 
				if ( CONF_DATAMODE_VAONLY == m_tConf.GetConfAttrb().GetDataMode() )
				{
					//视讯会议
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "conference %s created and started!\n",m_tConf.GetConfName());
				}
				else if ( CONF_DATAMODE_VAANDDATA == m_tConf.GetConfAttrb().GetDataMode() )
				{
					//视讯和数据
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "conference %s created and started with data conf function !\n", m_tConf.GetConfName());
				}
				else
				{
					//数据会议
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "conference %s created and started with data conf function Only !\n", m_tConf.GetConfName());
				}
				
				//zbq[12/18/2008] 手动开启成功，清定时等待
				KillTimer(MCU_WAITEQP_CREATE_CONF_NOTIFY);
				
				//保存到文件(由上级MCU的呼叫创建的会议不保存)，N+1备份模式不保存到文件
				if ( CONF_CREATE_NPLUS == m_byCreateBy && MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcMcsMcuCreateConfReq] conf info don't save to file in N+1 restore mode.\n");
				}
				else
				{
					g_cMcuVcApp.SaveConfToFile( m_byConfIdx, FALSE );
				}
				
				//设置自动结束时钟
				if( m_tConf.GetDuration() != 0 )
				{
					SetTimer( MCUVC_ONGOING_CHECK_TIMER, TIMESPACE_ONGOING_CHECK );
				}
				
				//设置会议保护状态
				if ( CONF_CREATE_MCS == m_byCreateBy && cServMsg.GetSrcSsnId() != 0)
				{
					if ( CONF_LOCKMODE_LOCK == m_tConf.m_tStatus.GetProtectMode() )
					{
						m_tConfProtectInfo.SetLockByMcs( cServMsg.GetSrcSsnId() );
					}
					else if( CONF_LOCKMODE_NEEDPWD == m_tConf.m_tStatus.GetProtectMode() )
					{
						m_tConfProtectInfo.SetMcsPwdPassed(cServMsg.GetSrcSsnId(), TRUE);
					}
				}
				
				//向DCS发起创会请求
				if ( CONF_DATAMODE_VAANDDATA == m_tConf.GetConfAttrb().GetDataMode() ||
					CONF_DATAMODE_DATAONLY  == m_tConf.GetConfAttrb().GetDataMode() )
				{
					SendMcuDcsCreateConfReq();
				}
				
				//向GK注册会议
				//m_byRegGKDriId = g_cMcuVcApp.GetRegGKDriId();
				BOOL32 bInviteMtNow = FALSE;
				
				if( !g_cMcuVcApp.IsTemRegGK(m_byConfIdx) && g_cMcuAgent.GetGkIpAddr() != 0 && 
					!m_tConf.m_tStatus.IsRegToGK()  && (0 != g_cMcuVcApp.GetRegGKDriId()) )
				{
					g_cMcuVcApp.RegisterConfToGK( m_byConfIdx, g_cMcuVcApp.GetRegGKDriId() );
					
					//注册成功后邀请终端
					memcpy(m_abySerHdr, cServMsg.GetServMsg(), sizeof(m_abySerHdr));
					m_tConfInStatus.SetInviteOnGkReged(TRUE);
					m_tConfInStatus.SetRegGkNackNtf(TRUE);
				}
				else 
				{	
					// 标记邀请终端
					if(0 != g_cMcuAgent.GetGkIpAddr())
					{
						m_tConf.m_tStatus.SetRegToGK(TRUE);
					}
					bInviteMtNow = TRUE;
				}
				
				// 向GK请求开始会议计费 [11/09/2006-zbq]
				if ( m_tConf.IsSupportGkCharge() && g_cMcuVcApp.GetRegGKDriId() != 0 &&
					//zbq [03/26/2007] 计费链路建立失败，允许创会，后续发起计费
					g_cMcuVcApp.GetChargeRegOK() )
				{               
					g_cMcuVcApp.ConfChargeByGK( m_byConfIdx, g_cMcuVcApp.GetRegGKDriId(), FALSE, m_byCreateBy, byMtNum );
				}
				else
				{
					if ( m_tConf.IsSupportGkCharge() ) 
					{
						ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[CreateConf] charge postponed due to GetRegGKDriId.%d, ChargeRegOK.%d !\n",
							g_cMcuVcApp.GetRegGKDriId(), g_cMcuVcApp.GetChargeRegOK() );
					}
				}
				
				// 顾振华 [4/29/2006]。原来通知MCS发送在保存到文件上面。
				// 移动到这里是为了保证Gk的信息是正确的。通知所有会控。
				cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
				if(m_tConf.HasConfExInfo())
				{
					u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
					u16 wPackDataLen = 0;
					PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
					cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
				}

				SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
				
				// 邀请终端，如果上面已经标记
				if (bInviteMtNow)                            
				{
					cServMsg.SetNoSrc();
					cServMsg.SetTimer( TIMESPACE_WAIT_AUTOINVITE );
					cServMsg.SetMsgBody( NULL, 0 );
					InviteUnjoinedMt( cServMsg );
				}
				
				//发Trap消息
				TConfNotify tConfNotify;
				CConfId cConfId;
				cConfId = m_tConf.GetConfId( );
				memcpy( tConfNotify.m_abyConfId, &cConfId, sizeof(cConfId) );
				astrncpy( tConfNotify.m_abyConfName, m_tConf.GetConfName(),
					sizeof(tConfNotify.m_abyConfName), MAXLEN_CONFNAME);
				SendTrapMsg( SVC_AGT_CONFERENCE_START, (u8*)&tConfNotify, sizeof(tConfNotify) );	
				
				//设置定时邀请时钟
				m_tRefreshParam.dwMcsRefreshInterval = g_cMcuVcApp.GetMcsRefreshInterval();
				
				//zjj20100419 上传带宽限制
				//lukunpeng 2010/06/10 不需要管理本级的上传还有多少带宽可用，完全交给上级控制
// 				if( IsSupportMultiSpy() )
// 				{
// 					m_cLocalSpyMana.SetConfRemainSpyBW( m_tConf.GetSndSpyBandWidth() );
// 				}			
				SetTimer( MCUVC_INVITE_UNJOINEDMT_TIMER, 1000*m_tConf.m_tStatus.GetCallInterval() );
				
				SetTimer( MCUVC_MCUSRC_CHECK_TIMER, TIMESPACE_MCUSRC_CHECK);
				
				u8 m_byUniformMode = tConfAttrib.IsAdjustUniformPack();
				cServMsg.SetMsgBody((u8 *)&m_byUniformMode, sizeof(u8));
				cServMsg.SetConfIdx(m_byConfIdx);
				SendMsgToAllMp(MCU_MP_CONFUNIFORMMODE_NOTIFY, cServMsg);
				
				//改变会议实例状态
				NEXTSTATE( STATE_ONGOING );
				
				//发给会控会议所有终端信息
				m_tConfAllMtInfo.m_tLocalMtInfo.SetMcuIdx( LOCAL_MCUIDX );
				//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
				//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
				SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
				
				//发给会控终端表 
				SendMtListToMcs(LOCAL_MCUIDX);	
				
				// [11/1/2011 liuxu]发终端状态给会控
				MtStatusChange(NULL, TRUE);
				
				//通知n+1备份服务器更新会议信息
				if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
				{
					ProcNPlusConfInfoUpdate(TRUE);
				}  
				m_swCurSatMtNum = m_tConf.GetSatDCastChnlNum();
			}
			else //预约会议
			{	
			
				//周期性预约会议
				if ( m_tConf.IsCircleScheduleConf() )
				{
					//周期性预约会议，有效时间界面只选日期，不设时分秒，所以保证durastart的时分秒为00:00:00,duraend的时分秒为23:59:59
					TDurationDate tDuraDate = m_tConfEx.GetDurationDate();
					ModifyCircleScheduleConfDuraDate(tDuraDate);
					m_tConfEx.SetDurationDate(&tDuraDate);	
					
					BOOL32 bScheduleConfNeedStart = IsCircleScheduleConfNeedStart();
					if ( bScheduleConfNeedStart )
					{
						CServMsg cOngoingConfMsg;
						memcpy(&cOngoingConfMsg,&cServMsg,sizeof(cServMsg));
						TConfInfo* ptConfInfo =	(TConfInfo *)cOngoingConfMsg.GetMsgBody();
						if ( NULL != ptConfInfo )
						{
							ptConfInfo->m_tStatus.SetOngoing();
							ptConfInfo->SetStartTime( time( NULL ) );
							//清掉周期性预约会议模式
							ptConfInfo->ClearSchedulConfMode();
							u8 byInsID = AssignIdleConfInsID(GetInsID()+1);
							if(0 != byInsID)
							{
								cOngoingConfMsg.SetSrcMtId(CONF_CREATE_SCH);
								::OspPost(MAKEIID( AID_MCU_VC, byInsID ), MCU_SCHEDULE_CONF_START, 
									cOngoingConfMsg.GetServMsg(), cOngoingConfMsg.GetServMsgLen());
								ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuCreateConfReq]MCU_SCHEDULE_CONF_START send to Ins.%d\n", byInsID);	
							}
							else
							{
								ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMcsMcuCreateConfReq] no idle confinst!\n");
								
							}
						}
					}
					
					//更新本预约会议的下次开启时间
					TKdvTime tNextOngoingtime = GetNextOngoingTime(m_tConf,m_tConfEx.GetDurationDate());
					TKdvTime tNullTime;//构造为memset为0
					//下次开启时间为空，回nack
					if (tNextOngoingtime == tNullTime   )
					{
			
						if ( !bScheduleConfNeedStart )
						{
							wErrCode = (u16)ERR_MCU_CIRCLESCHEDULECONF_TIME_WRONG;
						
						}
						else //开启成功即时会议后，下次开启时间为空，周期性预约会议也没有开启的必要
						{
							wErrCode = (u16)ERR_MCU_CIRCLESCHEDULECONF_WILLNOT_START;
						}
						//结会，给mcs回提示
						cServMsg.SetErrorCode( wErrCode );
						SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );            
						ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuCreateConfReq]CircleScheduleConference %s cannot changeto ongoing from now on!\n", tTmpConf.GetConfName() );
						ReleaseConf();
						
						return;
					
					}
				
					
					m_tConfEx.SetNextStartTime(&tNextOngoingtime);
					
					
				}

				m_tConf.m_tStatus.SetScheduled();
				
				//把会议指针存到数据区
				if( !g_cMcuVcApp.AddConf( this ) )	//add into table
				{
					ClearHduSchemInfo();
					CreateConfSendMcsNack( byOldInsId, ERR_MCU_CONFOVERFLOW, cServMsg, TRUE);
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because of full instance!\n", m_tConf.GetConfName() );
					return;
				}		
				
				//应答会控 
				SendMsgToMcs( cServMsg.GetSrcSsnId(), cServMsg.GetEventId() + 1, cServMsg );
				
				//预约会议不能独享控制权 
				if( CONF_LOCKMODE_LOCK == m_tConf.m_tStatus.GetProtectMode() )
				{
					m_tConf.m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );
				}
				if( CONF_LOCKMODE_NEEDPWD == m_tConf.m_tStatus.GetProtectMode() )
				{
					m_tConfProtectInfo.SetMcsPwdPassed(cServMsg.GetSrcSsnId(), TRUE);
				}			
				
				//通知所有会控
				g_cMcuVcApp.ConfInfoMsgPack(this, cServMsg);
				SendMsgToAllMcs(MCU_MCS_TEMSCHCONFINFO_NOTIF, cServMsg);
				
				// 2011-11-28 add by pgf: zoujunlong需求：添加VMP模板参数信息上报
				TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
				if( tConfAttrb.IsHasVmpModule() )
				{		
					CServMsg cVmpMsg;
					TVmpModule tVmpModule = GetVmpModule(); 
					cServMsg.SetMsgBody( (u8*)&tVmpModule, sizeof(TVmpModule) );
					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPINFO, cVmpMsg);
				}
				
				//保存到文件
				BOOL32 bRetTmp = g_cMcuVcApp.SaveConfToFile( m_byConfIdx, FALSE );
				if (!bRetTmp)
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[creat conf]SaveConfToFile error!!!\n");
				}
				
				//zjj20100419 上传带宽限制
				//lukunpeng 2010/06/10 不需要管理本级的上传还有多少带宽可用，完全交给上级控制
// 				if( IsSupportMultiSpy() )
// 				{
// 					m_cLocalSpyMana.SetConfRemainSpyBW( m_tConf.GetSndSpyBandWidth() );
// 				}
				
				//设置时钟
				SetTimer( MCUVC_SCHEDULED_CHECK_TIMER, TIMESPACE_SCHEDULED_CHECK );
				
				//打印
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "A scheduled conference %s created!\n", m_tConf.GetConfName() );
				
				//改变会议实例状态
				NEXTSTATE( STATE_SCHEDULED );
			}
			
			//zjj20091102 读取文件会议信息文件中的预案信息
			TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
			BOOL32 bIsConfFileGBK = IsConfFileGBK(); // mcs读预案 [pengguofeng 7/24/2013]
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuCreateConfReq]conf.%d default.%d is GBK.%d\n",
				m_byConfIdx, tConfAttrbEx.IsDefaultConf(), bIsConfFileGBK);
			if(!m_cCfgInfoRileExtraDataMgr.ReadExtraDataFromConfInfoFile(m_byConfIdx,tConfAttrbEx.IsDefaultConf()))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_MCS, "ReadExtraDataFromConfInfoFile faield!\n");
			}
			else
			{
				if ( bIsConfFileGBK )
				{
					m_cCfgInfoRileExtraDataMgr.TransEncoding2Utf8();
				}
			}
			
			break;
		}
	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	
	return;
}

/*====================================================================
    函数名      ：IsConfFileGBK
    功能        ：会议是否为GBK格式
    算法实现    ：判断有没有UTF8标志
	注意		：MCS预案只有4.7才加入，老版本都默认FALSE
    引用全局变量：
    输入参数说明：none
    返回值说明  ：BOOL32
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	2013/07/17  4.7         彭国锋          创建
====================================================================*/
BOOL32 CMcuVcInst::IsConfFileGBK()
{
	if ( m_tConf.GetConfSource() != MCS_CONF )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[IsConfFileGBK] only hanle MCS conf\n");
		return FALSE;
	}

	TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
	u8 byFileIndex = 0xFF;
	//1. 缺省会议
	if ( tConfAttrbEx.IsDefaultConf())
	{
		byFileIndex = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;
	}
	else
	{
		byFileIndex = m_byConfIdx - MIN_CONFIDX;
	}

	//2.read file
	TConfStore tConfStoreBuf;
	TPackConfStore *ptPackConfStore = (TPackConfStore *)&tConfStoreBuf;
	if ( !GetConfFromFile(byFileIndex, ptPackConfStore))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[IsConfFileGBK]get conf_%d.dat failed\n", byFileIndex);
		return FALSE;
	}

	u16 wPackConfDataLen = 0;
	TConfStore tConfStore;
	if (!UnPackConfStore(ptPackConfStore, tConfStore,wPackConfDataLen))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[IsConfFileGBK]UnPackConfStore conf_%d.dat failed\n", byFileIndex);
		return FALSE;
	}

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[IsConfFileGBK]conf.%d fileidx.%d name.%s \n",
		m_byConfIdx, byFileIndex, tConfStore.m_tConfInfo.GetConfName());
	u16 wConfExInfoLen = 0;
	u8 byEncoding = emenCoding_GBK;
	BOOL32 bUnknown = FALSE;

	// 以下接口会改变m_tConfEx，所以申请一个临时变量 [pengguofeng 7/19/2013]
	TConfInfoEx tConfInfoEx;
	memcpy(&tConfInfoEx, &m_tConfEx, sizeof(TConfInfoEx));
	UnPackConfInfoEx(tConfInfoEx, tConfStore.m_byConInfoExBuf, wConfExInfoLen, bUnknown, NULL, NULL, &byEncoding);

	if ( byEncoding == emenCoding_GBK )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*====================================================================
    函数名      ：IsConfInfoCheckPass
    功能        ：会议信息是否通过检查
    算法实现    ：
    引用全局变量：
    输入参数说明：const CServMsg &cMsg
                  TConfInfo &tConfInfo
                  u16       &wErrCode
                  BOOL32    bTemplate
    返回值说明  ：BOOL32
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	07/12/25    4.0         张宝卿          创建
====================================================================*/
BOOL32 CMcuVcInst::IsConfInfoCheckPass(const CServMsg &cMsg, TConfInfo &tConfInfo, u16 &wErrCode, BOOL32 bTemplate)
{
    BOOL32 bConfFromFile = tConfInfo.m_tStatus.IsTakeFromFile();

    if ( tConfInfo.GetConfId().IsNull() )
    {
        //ConfLog( FALSE, "[ConfInfoCheck] ConfID.IsNull, ignore it\n" );
        //return FALSE;
    }
	
    // guzh[4/25/2007] 会议码率限制
    // zgc [6/18/2007] 修改限制条件，改为会议主视频码率加上主音频码率不能大于最大会议码率
    if ( tConfInfo.GetBitRate() + GetAudioBitrate(tConfInfo.GetMainAudioMediaType()) > MAX_CONFBITRATE )
    {
        wErrCode = ERR_MCU_CONFBITRATE;
        if ( bConfFromFile )
        {
            g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
        }      
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conference %s create failed because bitrate(Vid.%d, Aud.%d) error!\n", tConfInfo.GetConfName(), tConfInfo.GetBitRate(), GetAudioBitrate(tConfInfo.GetMainAudioMediaType()) );
        return FALSE;
    }

    if ( !bTemplate )
    {
        //会议的E164号码已存在，拒绝 
	    if( g_cMcuVcApp.IsConfE164Repeat( tConfInfo.GetConfE164(), bTemplate,tConfInfo.m_tStatus.IsOngoing() ) )
	    {
            wErrCode = ERR_MCU_CONFE164_REPEAT;
            if ( bConfFromFile )
            {
                g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
            }      
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s E164 repeated and create failure!\n", tConfInfo.GetConfName() );
            return FALSE;
        }

	    //会议名已存在，拒绝
	    if( g_cMcuVcApp.IsConfNameRepeat( tConfInfo.GetConfName(), bTemplate ,tConfInfo.m_tStatus.IsOngoing()) )
	    {
            wErrCode = ERR_MCU_CONFNAME_REPEAT;
            if ( bConfFromFile )
            {
                g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
            }      
		    ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s name repeated and create failure!\n", m_tConf.GetConfName() );
		    return FALSE;
	    }

	    //会议已结束，拒绝 
		if ( !tConfInfo.IsCircleScheduleConf() )//非周期性预约会议
		{
			//手动结束，当前时间>开启时间+30min，非手动结束，当前时间>开启时间+持续时间，return FALSE
			if ( tConfInfo.GetDuration() != 0 && ( time( NULL ) > tConfInfo.GetStartTime() + tConfInfo.GetDuration() * 60 )  )
			{
				wErrCode = ERR_MCU_STARTTIME_WRONG;
				if ( bConfFromFile )
				{
					g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
				}      
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s too late and canceled!\n", tConfInfo.GetConfName() );
				return FALSE;
			}
		}
		else//周期性预约会议，不再检测，checktimer里面已经检测过了
		{

		}
	

        //duplicate confid
        u8 abyConfId[sizeof(CConfId)];
        tConfInfo.GetConfId().GetConfId(abyConfId, sizeof(abyConfId));
        if( OSPERR_ALIAS_REPEAT == SetAlias( ( const char* )abyConfId, sizeof( CConfId ) ) )
        {
            wErrCode = ERR_MCU_CONFIDREPEAT;
            if ( bConfFromFile )
            {
                g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
            }      
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because of repeat conference ID!\n", tConfInfo.GetConfName() );
            return FALSE;
        }        
    }


	//dynamic vmp and vmp module conflict
	TConfAttrb tConfAttrb = tConfInfo.GetConfAttrb();
	if( tConfAttrb.IsHasVmpModule() && 
		CONF_VMPMODE_AUTO == tConfInfo.m_tStatus.GetVMPMode() )
	{
        wErrCode = ERR_MCU_DYNAMCIVMPWITHMODULE;
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf or temp %s create failed because has module with dynamic vmp!\n", tConfInfo.GetConfName() );
		return FALSE;		
	}

	//无效的会议保护方式,拒绝
	if( tConfInfo.m_tStatus.GetProtectMode() > CONF_LOCKMODE_LOCK )
	{
        wErrCode = ERR_MCU_INVALID_CONFLOCKMODE;
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf or temp %s protect mode invalid and nack!\n", tConfInfo.GetConfName() );
		return FALSE;
	}

	//无效的会议呼叫策略,拒绝
	if( tConfInfo.m_tStatus.GetCallMode() > CONF_CALLMODE_TIMER )
	{
        wErrCode = ERR_MCU_INVALID_CALLMODE;
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf or temp %s call mode invalid and nack!\n", tConfInfo.GetConfName() );
		return FALSE;
	}

    // xsl [11/16/2006] 若没有配置GK或配置的GK不支持计费，但需要GK计费则不允许创会
    if (tConfInfo.IsSupportGkCharge())
    {
        if (0 == g_cMcuAgent.GetGkIpAddr())
        {
            if ( !bTemplate )
            {
                wErrCode = ERR_MCU_GK_UNEXIST_CREATECONF;
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s not config gk in gk charge conf and nack!\n", tConfInfo.GetConfName() );
                return FALSE;                
            }
            else
            {
                NotifyMcsAlarmInfo( cMsg.GetSrcSsnId(), ERR_MCU_GK_UNEXIST_CREATECONF );
            }
        }
        else
        {
            if ( !g_cMcuAgent.GetIsGKCharge())
            {
                if ( !bTemplate )
                {
                    wErrCode = ERR_MCU_GK_NOCHARGE_CREATECONF;
                    ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s configed gk support no charge conf and nack!\n", tConfInfo.GetConfName() );
                    return FALSE;
                }
                else
                {
                    NotifyMcsAlarmInfo( cMsg.GetSrcSsnId(), ERR_MCU_GK_NOCHARGE_CREATECONF );
                }
            }
        }
    }

    //若不用适配器，不得创建双格式或双速会议模板
    TConfAttrb ttmpAttrb = tConfInfo.GetConfAttrb();
    if ( !ttmpAttrb.IsUseAdapter() && 
        ( MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType() ||
          MEDIA_TYPE_NULL != tConfInfo.GetSecAudioMediaType() ||
          0 != tConfInfo.GetSecBitRate() ) 
       )
    {        
        ttmpAttrb.SetUseAdapter(TRUE);
        tConfInfo.SetConfAttrb(ttmpAttrb);
    }
    
    //若会议不需要适配，则在会议属性中不启用适配器
    if (MEDIA_TYPE_NULL == tConfInfo.GetSecVideoMediaType() &&
        MEDIA_TYPE_NULL == tConfInfo.GetSecAudioMediaType() &&
        0 == tConfInfo.GetSecBitRate() &&
        ttmpAttrb.IsUseAdapter() &&
        // 高清会议允许多分辨率终端入会, zgc, 2008-08-09
        !IsHDConf( tConfInfo ) &&
        // H264双流格式允许适配, zgc, 2008-08-21
        !(( (tConfInfo.GetCapSupport().GetDStreamMediaType() == VIDEO_DSTREAM_MAIN ||
		     tConfInfo.GetCapSupport().GetDStreamMediaType() == VIDEO_DSTREAM_MAIN_H239) &&
           tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264) ||
          tConfInfo.GetCapSupport().GetDStreamMediaType() == VIDEO_DSTREAM_H264_H239 ||
          tConfInfo.GetCapSupport().GetDStreamMediaType() == VIDEO_DSTREAM_H264) &&
        tConfInfo.GetCapSupport().GetDStreamMediaType() != VIDEO_DSTREAM_H264_H263PLUS_H239 &&
		// VCS会议默认都启用适配器
		tConfInfo.GetConfSource() != VCS_CONF)
    {
        ttmpAttrb.SetUseAdapter(FALSE);
        tConfInfo.SetConfAttrb(ttmpAttrb);
        ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "template/conf %s need not bas adapter, auto modify it in conf attrib.\n", tConfInfo.GetConfName());
    }

    // 非高清会议不自动勾选画面合成自适应, zgc, 2008-08-20
    /*
	// MP4/MP2 CIF以上的格式 开启画面合成自适应，zgc，2008-01-17
	if ( ( MEDIA_TYPE_MP4 == tConfInfo.GetMainVideoMediaType() 
		|| MEDIA_TYPE_H262 == tConfInfo.GetMainVideoMediaType() 
		)
		&& 
		( VIDEO_FORMAT_2CIF == tConfInfo.GetMainVideoFormat()
		|| VIDEO_FORMAT_4CIF == tConfInfo.GetMainVideoFormat()
		|| VIDEO_FORMAT_AUTO == tConfInfo.GetMainVideoFormat()
		)
		)
	{
		if ( !tConfInfo.IsVmpAutoAdapt() )
		{
			tConfInfo.SetVmpAutoAdapt( TRUE );
			ConfLog(FALSE, " conf.%d need vmp auto adapt, open.\n", m_byConfIdx );
		}
	}

	// H264/D1格式，开启画面合成自适应, zgc, 2008-04-02
	if ( MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType() &&
		VIDEO_FORMAT_CIF != tConfInfo.GetMainVideoFormat() )
	{
		if ( !tConfInfo.IsVmpAutoAdapt() )
		{
			tConfInfo.SetVmpAutoAdapt( TRUE );
			ConfLog(FALSE, " conf.%d need vmp auto adapt, open.\n", m_byConfIdx );
		}
	}*/
    if ( IsHDConf(tConfInfo) )
    {
        if ( !tConfInfo.IsVmpAutoAdapt() )
        {
            tConfInfo.SetVmpAutoAdapt( TRUE );
            ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, " conf.%d need vmp auto adapt, open.\n", m_byConfIdx );
		}
    }

    // guzh [12/27/2007] H264的D1及以上分辨率能力限制
    if ( MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType() )
    {
        // guzh 2008/08/07 放开限制
        /*
        if (VIDEO_FORMAT_CIF != tConfInfo.GetMainVideoFormat() ||
            tConfInfo.GetBitRate() >= 2048)
        {
            if ( MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType() ||
                 0 != tConfInfo.GetSecBitRate() ||
                 ttmpAttrb.IsHasTvWallModule() )
            {
                wErrCode = ERR_MCU_CONFFORMAT;
                ConfLog( FALSE, "template/conf %s not support H264-D1 and extra feacher at the same time\n", tConfInfo.GetConfName() );
                return FALSE;
            }
        }
        */

#ifdef _MINIMCU_
        if ( CMcuPfmLmt::IsConfFormatHD(tConfInfo) && 
             !g_cMcuAgent.IsMcu8000BHD() )
        {
            wErrCode = ERR_MCU_CONFFORMAT;
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "template/conf %s not support H264-HD in MCU8000B-NoHD\n", tConfInfo.GetConfName() );
            return FALSE;
        }
#endif

    }        

    //未设置会议加密模式，设置密钥加密模式，无效
    if( CONF_ENCRYPTMODE_NONE == ttmpAttrb.GetEncryptMode() &&
        CONF_ENCRYPTMODE_NONE != tConfInfo.GetMediaKey().GetEncryptMode() )
    {
        TMediaEncrypt tEncrypt = tConfInfo.GetMediaKey();            
        tEncrypt.Reset();
        tConfInfo.SetMediaKey(tEncrypt);
    }   

    //会议加密
    //设置了会议加密模式，未设置密钥加密模式，生效
    if( CONF_ENCRYPTMODE_NONE != ttmpAttrb.GetEncryptMode() &&
        CONF_ENCRYPTMODE_NONE == tConfInfo.GetMediaKey().GetEncryptMode() )
    {
        TMediaEncrypt tEncrypt = tConfInfo.GetMediaKey();
        tEncrypt.SetEncryptMode(ttmpAttrb.GetEncryptMode());
        
        //DES加密
        if( CONF_ENCRYPTMODE_DES == ttmpAttrb.GetEncryptMode() )
        {
            //自动密钥. 另: 手动密钥获取的结构已传入,下同.
            if ( !ttmpAttrb.IsEncKeyManual() )
            {
                tEncrypt.SetEncryptKey(GetRandomKey(), LEN_DES);
            }
        }
        //AES加密
        else if( CONF_ENCRYPTMODE_AES == ttmpAttrb.GetEncryptMode() )
        {
            //自动密钥生成. 
            if ( !ttmpAttrb.IsEncKeyManual() )
            {
                tEncrypt.SetEncryptKey(GetRandomKey(), LEN_AES);
            }
        }
        
        tConfInfo.SetMediaKey(tEncrypt);
    }    

    //加密情况下，不支持归一化重整处理方式
    if( CONF_ENCRYPTMODE_NONE != m_tConf.GetMediaKey().GetEncryptMode() )
    {
        ttmpAttrb.SetUniformMode( CONF_UNIFORMMODE_NONE );
        tConfInfo.SetConfAttrb( ttmpAttrb );
    }
    
    // 顾振华@2006.4.6 组播会议，模版需要检查组播地址是否合法
    if ( ttmpAttrb.IsMulticastMode() )
    {
        u32 dwCastIp = ttmpAttrb.GetMulticastIp();
        u16 wCastPort = ttmpAttrb.GetMulticastPort();
        
        BOOL32 bIpInvalid = FALSE;
        if ( dwCastIp < MULTICAST_IP_START || dwCastIp > MULTICAST_IP_END ||
             (dwCastIp > MULTICAST_IP_RESV_START && dwCastIp < MULTICAST_IP_RESV_END ) ||
             wCastPort == 0)
        {
            bIpInvalid = TRUE;
        }
        
        if (bIpInvalid)
        {
            wErrCode = ERR_MCU_MCASTADDR_INVALID;
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf or temp.%s create failed because of multicast address invalid!\n", tConfInfo.GetConfName() );
            return FALSE;
        }
    }

    // 顾振华@2006.4.11 分散会议        
    if ( ttmpAttrb.IsSatDCastMode() )
    {
        u32 dwCastIp = ttmpAttrb.GetSatDCastIp();
        u16 wCastPort = ttmpAttrb.GetSatDCastPort();
        // 需要检查地址是否合法
        BOOL32 bIpInvalid = FALSE;
        if ( dwCastIp < MULTICAST_IP_START || dwCastIp > MULTICAST_IP_END ||
             (dwCastIp > MULTICAST_IP_RESV_START && dwCastIp < MULTICAST_IP_RESV_END ) ||
             wCastPort == 0)
        {
            bIpInvalid = TRUE;
        }
        //BOOL32 bAdpNeeded = FALSE;
		//卫星会议需求：允许以下配置的卫星会议开启
/*
        if (tConfInfo.GetSecBitRate() != 0 ||
            /*!tConfInfo.GetCapSupport().GetSecondSimCapSet().IsNull()
			(
			//zjl 20101029 主格式不等于辅格式（音视频）
				(tConfInfo.GetCapSupport().GetMainVideoType() != MEDIA_TYPE_NULL &&
					tConfInfo.GetCapSupport().GetSecVideoType() != MEDIA_TYPE_NULL &&
					tConfInfo.GetCapSupport().GetMainVideoType() != tConfInfo.GetCapSupport().GetSecVideoType()
					  ) ||
				 (tConfInfo.GetCapSupport().GetMainAudioType() != MEDIA_TYPE_NULL &&
					tConfInfo.GetCapSupport().GetSecAudioType() != MEDIA_TYPE_NULL &&
					tConfInfo.GetCapSupport().GetMainAudioType() != tConfInfo.GetCapSupport().GetSecAudioType()
						)
					)
				)

        {
            // 双速双格式会议不能选择分散会议方式
            bAdpNeeded = TRUE;
        }
   */     
        // 发言人只能看自己
/*        if (ttmpAttrb.GetSpeakerSrc() != CONF_SPEAKERSRC_SELF)
        {
            ttmpAttrb.SetSpeakerSrc( CONF_SPEAKERSRC_SELF );
            ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Conf or temp.%s change speakersrc to self.\n", tConfInfo.GetConfName() );
        }
        // 不启用适配
        if (ttmpAttrb.IsUseAdapter())
        {
            ttmpAttrb.SetUseAdapter(FALSE);
            ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Conf or temp.%s no use adapter\n", tConfInfo.GetConfName() );
        }
*/
        // 不讨论会议
        if (ttmpAttrb.IsDiscussConf())
        {
            ttmpAttrb.SetDiscussConf(FALSE);
            ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Conf or temp.%s no start discuss\n", tConfInfo.GetConfName() );
        }
		//卫星会议本地终端还是支持prs的，故这里不做特殊处理
//         // 不丢包重传
//         if (ttmpAttrb.IsResendLosePack())
//         {
//             ttmpAttrb.SetPrsMode(FALSE);
//             ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "Conf or temp.%s no use prs\n", tConfInfo.GetConfName() );
//         }
        // xsl [7/20/2006] 不支持级联
        if (ttmpAttrb.IsSupportCascade())
        {
            ttmpAttrb.SetSupportCascade(FALSE);
        }            
        tConfInfo.SetConfAttrb( ttmpAttrb );
        
        // xsl [7/21/2006] 回传通道数不能为0
        if (tConfInfo.GetSatDCastChnlNum() == 0)
        {
            tConfInfo.SetSatDCastChnlNum(1);
        }
        
        if (bIpInvalid/*|| bAdpNeeded*/)
        {                
            if (bIpInvalid)
            {
                wErrCode = ERR_MCU_DCASTADDR_INVALID;
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conf or temp.%s create failed because of distributed conf multicast address invalid!\n", tConfInfo.GetConfName() );
            }
            else
            {
                wErrCode = ERR_MCU_DCAST_NOADP;
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conf or temp.%s create failed because of distributed conf not support second rate or format!\n", tConfInfo.GetConfName() );
            }  
            return FALSE;
        }
    }

    // guzh [9/1/2007] 8000B/8000C双媒体会议支持
#ifdef _MINIMCU_
    if ( tConfInfo.GetSecBitRate() > 0 &&                 
         MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType() && 
         ( tConfInfo.GetMainVideoMediaType() != tConfInfo.GetSecVideoMediaType() ||
           tConfInfo.GetMainVideoFormat() != tConfInfo.GetSecVideoFormat() )
        )
    {
        // 不支持低速组播
        if ( tConfInfo.GetConfAttrb().IsMulticastMode() && tConfInfo.GetConfAttrb().IsMulcastLowStream() )
        {
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conf or temp.%s not support multicast low stream\n", tConfInfo.GetConfName() );
            wErrCode = ERR_MCU_MCLOWNOSUPPORT;
            return FALSE;
        }
        
        // 不支持低速录像
        TConfAutoRecAttrb tRecAttrb = tConfInfo.GetAutoRecAttrb();
        if ( tRecAttrb.IsAutoRec() && tRecAttrb.GetRecParam().IsRecLowStream() )
        {
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conf or temp.%s can't support auto record low stream\n", tConfInfo.GetConfName() );
            wErrCode = ERR_MCU_RECLOWNOSUPPORT;
            return FALSE;    
        }
    }
#endif

    //zbq[11/18/2008] mau相关的处理：适配会议给出提示
//     if (tConfInfo.GetConfSource() != VCS_CONF &&
//         ((IsHDConf(tConfInfo) &&
// 		  (0 != tConfInfo.GetSecBitRate() ||
// 		   (MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType() && 0 != tConfInfo.GetSecVideoMediaType()) ||
// 		   tConfInfo.GetConfAttrbEx().IsResEx1080() ||
// 		   tConfInfo.GetConfAttrbEx().IsResEx720()  ||
// 		   tConfInfo.GetConfAttrbEx().IsResEx4Cif() ||
// 		   tConfInfo.GetConfAttrbEx().IsResExCif()
// 		  )
// 		 )
// 		 ||
// 		 // 双双流
// 		 (tConfInfo.GetCapSupport().IsDStreamSupportH239() &&
// 		  MEDIA_TYPE_H264 == tConfInfo.GetDStreamMediaType() &&
// 		  (g_cMcuVcApp.IsSupportSecDSCap() || MEDIA_TYPE_NULL != tConfInfo.GetCapSupportEx().GetSecDSType())
// 		 )
//         )
// 	   )
//     {
//         CBasMgr cBasMgr;
//         if (!cBasMgr.IsHdBasSufficient(tConfInfo))
//         {
//             ConfLog(FALSE, "temp.%s's mau might be insufficient\n", tConfInfo.GetConfName());
//             NotifyMcsAlarmInfo(cMsg.GetSrcSsnId(), ERR_MCU_NOENOUGH_HDBAS_TEMP);
//         }
// 
//         TMcuHdBasStatus tHdBasStatus;
//         cBasMgr.GetHdBasStatus(tHdBasStatus, tConfInfo);
// 
//         CServMsg cServMsg;
//         cServMsg.SetMsgBody((u8*)&tHdBasStatus, sizeof(tHdBasStatus));
//         cServMsg.SetEventId(MCU_MCS_MAUSTATUS_NOTIFY);
//         SendMsgToMcs(cMsg.GetSrcSsnId(), MCU_MCS_MAUSTATUS_NOTIFY, cServMsg);
//     }

    return TRUE;
}
/*====================================================================
    函数名      ：RlsAllBasForConf
    功能        ：释放会议所有占用的BAS
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	2010-8-4    4.6			周嘉麟			重写
====================================================================*/
void CMcuVcInst::RlsAllBasForConf()
{
	StopAllBrdAdapt();
	StopAllSelAdapt();
	ReleaseBasChn();
	ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[RlsAllBasForConf]RlsAllBasForConf!\n");
    return;
}

/*====================================================================
    函数名      ：PrepareAllNeedBasForConf
    功能        ：根据会议属性预留好所有可能需要的BAS
    算法实现    ：
    引用全局变量：
    输入参数说明：u16       &wErrCode : [OUT]不充分错误码
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	07/12/25    4.0         张宝卿          创建
    08/11/18    4.5         张宝卿          增加对MAU的校验
====================================================================*/
BOOL32 CMcuVcInst::PrepareAllNeedBasForConf(u16* pwErrCode)
{
	CBasChn *aptNeedChn[MAXNUM_PERIEQP]={NULL};	//通道指针数组
	TBasChnCapData atBasChnParam[MAXNUM_PERIEQP];
	u16 wErrorCode = ERR_MCU_NOENOUGH_HDBAS_CONF;
	if(g_cMcuVcApp.IsBasChnlEnoughForConf(m_tConf, m_tConfEx, aptNeedChn,atBasChnParam,wErrorCode))
	{
		ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[PrepareAllNeedBasForConf]PrepareAllNeedBasForConf:%d OK\n",m_byConfIdx);
		g_cMcuVcApp.OcuppyBasChn(aptNeedChn, MAXNUM_PERIEQP);
		AssignBasBrdChn(aptNeedChn, atBasChnParam, MAXNUM_PERIEQP,m_tConf.GetConfId());
		return TRUE;
	}
	else
	{
		if ( NULL == pwErrCode  )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[PrepareAllNeedBasForConf] pwErrCode is NULL!\n");
			return FALSE;
		}
		*pwErrCode = wErrorCode;
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[PrepareAllNeedBasForConf] Bas is not enough for conf!\n");
	}
	return FALSE;
}
/*====================================================================
    函数名      ：IsEqpInfoCheckPass
    功能        ：外设信息是否通过检查，部分外设操作
    算法实现    ：
    引用全局变量：
    输入参数说明：const CServMsg &cSrcMsg: [IN] 创会源
                  u16       &wErrCode : [OUT]不充分错误码
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	07/12/25    4.0         张宝卿          创建
    08/11/18    4.5         张宝卿          增加对MAU的校验
====================================================================*/
BOOL32 CMcuVcInst::IsEqpInfoCheckPass(const CServMsg &cSrcMsg, u16 &wErrCode)
{
    u8 byIdleVMPNum = 0;
    u8 abyVMPId[MAXNUM_PERIEQP];
    memset(abyVMPId, 0, sizeof(abyVMPId));
    g_cMcuVcApp.GetIdleVMP(abyVMPId, byIdleVMPNum, sizeof(abyVMPId));

    TVMPParam_25Mem tVmpParam = GetVmpParam25MemFromModule();

    if ( m_tConf.GetConfId().IsNull() )
    {
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "[EqpInfoCheck] tConfInfo.IsNull, ignore it\n" );
        return FALSE;
    }

    //处理各种会议属性，预留资源
    TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();

    // 顾振华@2006.4.6 组播会议，需要检查组播地址是否可用
    if ( tConfAttrib.IsMulticastMode() )
    {
        u32 dwCastIp  = tConfAttrib.GetMulticastIp();
        u16 wCastPort = tConfAttrib.GetMulticastPort();
        
        BOOL32 bAddrUsed = FALSE;
        
        // 检查所有的即时会议，是否使用了本地址段
        bAddrUsed = g_cMcuVcApp.IsMulticastAddrOccupied(dwCastIp, wCastPort);
        if (bAddrUsed)
        {
            wErrCode = ERR_MCU_MCASTADDR_USED;
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because of multicast address occupied!\n", m_tConf.GetConfName() );
            return FALSE;
        }
    }
    // 顾振华@2006.4.12 分散会议，检查组播地址是否可用
    if (tConfAttrib.IsSatDCastMode())
    {
        u32 dwCastIp  = tConfAttrib.GetSatDCastIp();
        u16 wCastPort = tConfAttrib.GetSatDCastPort();
        
        BOOL32 bAddrUsed = FALSE;
        
        // 检查所有的即时会议，是否使用了本地址段
        bAddrUsed = g_cMcuVcApp.IsDistrConfCastAddrOccupied(dwCastIp, wCastPort);
        
        if (bAddrUsed)
        {
            wErrCode = ERR_MCU_DCASTADDR_USED;
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conference %s create failed because of distributed conf multicast address occupied!\n", m_tConf.GetConfName() );
            return FALSE;
        }                
    }

    //是否需要创建数据会议            
    if ( CONF_DATAMODE_VAANDDATA == tConfAttrib.GetDataMode() || 
         CONF_DATAMODE_DATAONLY  == tConfAttrib.GetDataMode() )
    {
        BOOL32 bHasDcs = FALSE;
        for ( s32 nIndex = 1; nIndex <= MAXNUM_MCU_DCS; nIndex ++ )
        {
            //如果没有DCS在线
            if ( g_cMcuVcApp.IsPeriDcsValid( nIndex ) && 
                 g_cMcuVcApp.IsPeriDcsConnected( nIndex ) )
            {
                bHasDcs = TRUE;
                break;
            }
        }
        if(!bHasDcs)
        {
            wErrCode = ERR_MCU_NOIDLEDCS;
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conference %s create failed because of no DCS registered!\n", m_tConf.GetConfName() );
            return FALSE;
        }
    }

    //zbq[12/27/2006]查询到该MCU当前所有空闲VMP的ID 暨 调整VMP是否支持判断方案
	if( tConfAttrib.IsHasVmpModule() )
	{
		//是否有画面合成器
		if( 0 == byIdleVMPNum )
		{
            if ( CONF_CREATE_NPLUS == m_byCreateBy )
            {
                NotifyMcsAlarmInfo( cSrcMsg.GetSrcSsnId(), ERR_MCU_NPLUS_MASTER_VMP_NONE );
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "the master mcu couldn't restore VMP due to no VMP !\n" );

                // N+1备份回滚创建的会议，如果本主端没有VMP能力，不恢复画面合成，
                // 且要清空相关的数据。否则，该会议在备端再次恢复后 不仅会导致会
                // 议状态错乱，而且会由于主端没有VMP能力，无法再将会议回滚回来 [12/28/2006-zbq]
                if ( MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() )
                {
                    m_tConf.m_tStatus.SetVmpBrdst( FALSE );
                    m_tConf.SetHasVmpModule( FALSE );
                    m_tConf.m_tStatus.SetVMPMode( CONF_VMPMODE_NONE );
                    
                    //清空通道
                    for( u8 byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
                    {
                        m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );
                    }
                }
                else
                {
                    //N+1备份恢复会议，按现在的VMP能力的逻辑和保护，不会来到这里
                    ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conf.%s created by NPlus error which is impossible, check it !\n", m_tConf.GetConfName() );
                }
            }
            else
            {
                wErrCode = ERR_MCU_NOIDLEVMP;
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conference %s create failed because no idle vmp!\n", m_tConf.GetConfName() );
                return FALSE;
            }
		}
        /*else
        {
            m_tConf.m_tStatus.SetVMPMode(CONF_VMPMODE_CTRL);
            m_tConf.m_tStatus.SetVmpBrdst(m_tConfEqpModule.GetVmpModule().m_tVMPParam.IsVMPBrdst());
        }*/
	}
    
    if( tConfAttrib.IsHasTvWallModule() )
    {
        BOOL32 bTvWallDisconnected = FALSE;  //  是否存在电视墙不在线
        TMultiTvWallModule tMultiTvWallModule;
        m_tConfEqpModule.GetMultiTvWallModule( tMultiTvWallModule );
        TTvWallModule tTvWallModule;
		THduChnlInfo  atHduChnlInfo[MAXNUM_TVWALL_CHNNL_INSMOUDLE];
        for( u8 byTvLp = 0; byTvLp < tMultiTvWallModule.GetTvModuleNum(); byTvLp++ )
        {
            tMultiTvWallModule.GetTvModuleByIdx(byTvLp, tTvWallModule);
			//zjl[20091208]会议模板新增设置在hdu预案中设置终端成员
			u8 byTvWallId = tTvWallModule.GetTvEqp().GetEqpId();
			u8 byEqpType  = tTvWallModule.GetTvEqp().GetEqpType();
			if (EQP_TYPE_HDU_SCHEME == byEqpType)
			{
				u8 byHduSchemeNum = 0;
				THduStyleInfo atHduStyleInfoTable[MAX_HDUSTYLE_NUM];
				u16 wRet = g_cMcuAgent.ReadHduSchemeTable(&byHduSchemeNum, atHduStyleInfoTable);
				if (SUCCESS_AGENT != wRet)
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[IsEqpInfoCheckPass] ReadHduSchemeTable failed!\n");
				}
				else
				{
					for (u8 byStyleIdx = 0 ; byStyleIdx < byHduSchemeNum; byStyleIdx ++)
					{
						if (atHduStyleInfoTable[byStyleIdx].GetStyleIdx() == byTvWallId)
						{
							atHduStyleInfoTable[byStyleIdx].GetHduChnlTable(atHduChnlInfo);
							for (u8 byChnl = 0; byChnl < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byChnl++)
							{
								if( TW_MEMBERTYPE_NULL != tTvWallModule.m_abyMemberType[byChnl] &&
									!g_cMcuVcApp.IsPeriEqpConnected( atHduChnlInfo[byChnl].GetEqpId()))
								{
									ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[IsEqpInfoCheckPass]HDU(%d,%d) (%d) is not connected,So Return!\n\n",
									atHduChnlInfo[byChnl].GetEqpId(),atHduChnlInfo[byChnl].GetChnlIdx(),tTvWallModule.m_abyMemberType[byChnl]);
									wErrCode = ERR_MCU_NOIDLETVWALL;
									return FALSE;
								}
							}
						}
					}
				}
			}
			else
			{
				if( !g_cMcuVcApp.IsPeriEqpConnected( tTvWallModule.m_tTvWall.GetEqpId()) )
				{
					bTvWallDisconnected = TRUE;
					break;
				}
			}            
        }
		
        if( bTvWallDisconnected )
        {
            memset((void*)&m_tConfEqpModule, 0, sizeof(m_tConfEqpModule));
            wErrCode = ERR_MCU_NOIDLETVWALL;
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because Tv %d is outline!\n", 
                                m_tConf.GetConfName(), tTvWallModule.m_tTvWall.GetEqpId() );
			return FALSE;
        }
    }
	
    //是否开始混音, 预留混音器 
    //卫星分散会议不支持自动开启讨论和语音激励，所以不必检测混音资源
    if( !m_tConf.GetConfAttrb().IsSatDCastMode() && 
        ( tConfAttrib.IsDiscussConf() || 
          m_tConf.GetConfAttrbEx().IsSupportAutoVac() ) )
    {
        // 顾振华，在这里先检查是否有混音器 [4/29/2006]
        // 到后面再开始混音
        u8 byEqpId = 0;
        TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];
		memset(atAudioTypeDesc, 0, sizeof(atAudioTypeDesc));
		u8 byAudioCapNum = m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);

        byEqpId = g_cMcuVcApp.GetIdleMixer(0, byAudioCapNum, atAudioTypeDesc);
        if (0 == byEqpId)
        {
            if( CONF_CREATE_MT == m_byCreateBy ) //终端创会不直接返回
            {
                tConfAttrib.SetDiscussConf(FALSE);
                m_tConf.SetConfAttrb(tConfAttrib);
            }
			//[chendaiwei2010/10/17] N+1备端恢复会议或主端回滚会议，取消智能混音如果没有空闲的混音器
			else if (CONF_CREATE_NPLUS == m_byCreateBy )
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "Failed to recover N+1 conf with AutoMixing, cancel auto mixing!\n" );

                tConfAttrib.SetDiscussConf(FALSE);
                m_tConf.SetConfAttrb(tConfAttrib);
			}
            else
            {
                wErrCode = ERR_MCU_NOIDLEMIXER;
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Conference %s create failed because no idle mixer!\n", m_tConf.GetConfName() );
                return FALSE;
            }                    
        }
        else
        {
            //tianzhiyong 2010/04/15  检查该混音器是否可用
            if (!CheckMixerIsValide(byEqpId))
            {
				wErrCode = ERR_MCU_NOIDLEMIXER_INCONF ;
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because CheckMixerIsValide() mixer!\n", m_tConf.GetConfName() );
                return FALSE;
            }
        }
    }

    //bas modify 2
    //需要媒体类型适配的会议,预留适配器                       
    if ( tConfAttrib.IsUseAdapter() &&
		 !PrepareAllNeedBasForConf(&wErrCode))
	{
		return FALSE;
	}

	//需要丢包重传的会议,预留包重传通道 
	if (tConfAttrib.IsResendLosePack())
	{
		if (!AssignPrsChnnl4Conf())
        {
			// 分配资源失败
			//zjl[20091209]如果prs资源不足导致会议无法召开，需要释放会议占用的bas资源
			RlsAllBasForConf();
            wErrCode = ERR_MCU_NOIDLEPRS;
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because no idle prs!\n", m_tConf.GetConfName() );
			return FALSE;
		}
	}

    // guzh [8/29/2007] 双媒体会议能力限制,不支持低速组播
#ifdef _MINIMCU_
    if ( ISTRUE(m_byIsDoubleMediaConf) )                
    {
        if ( m_tConf.GetConfAttrb().IsMulticastMode() && 
             m_tConf.GetConfAttrb().IsMulcastLowStream() )
        {
            wErrCode = ERR_MCU_MCLOWNOSUPPORT;
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf.%s not support multicast low stream\n", m_tConf.GetConfName() );
            return FALSE;                
        }
    }
#endif

	// 会议开始自动录像
	//  [12/27/2009 pengjie] Modify 这里修改不能隔天定时录像问题，并把该部分代码提取函数接口
	if( !SetAutoRec(wErrCode) )
	{
		return FALSE;
	}
	// End Modify

    //songkun,20110609,入会信息提前进行，模板画面合成会判断会议是否正常添加，
    //以决定构造消息体给VMP
    //把会议指针存到数据区
    if(!g_cMcuVcApp.AddConf( this ) )
    {
          RlsAllBasForConf();
          wErrCode = ERR_MCU_CONFOVERFLOW;
          ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because of full instance!\n", m_tConf.GetConfName() );
          return FALSE;
    }

    //需动态分屏的会议开始画面合成,模版开启(自动/定制)vmp,由模版统一管理,不再支持把auto单独放入m_tConf.m_tStatus处理.
	if( /*CONF_VMPMODE_AUTO == m_tConf.m_tStatus.GetVMPMode() ||*/ 
		tConfAttrib.IsHasVmpModule() )			
	{
		//是否有画面合成器 
		if( 0 != byIdleVMPNum )
		{
            //可以支持当前合成风格的空闲VMP的ID
            u8 byVmpId = 0;

            /*if ( CONF_VMPMODE_AUTO != m_tConf.m_tStatus.GetVMPMode() )
            {*/
                //当前所有的空闲VMP是否支持所需的合成风格
                u16 wError = 0;
                if ( IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError) ) 
                {
                    /*m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
                    m_tVmpEqp.SetConfIdx( m_byConfIdx );*/

					/* 自动vmp时,更新tVmpParam信息<yhz此判断逻辑要删除,界面不应该把vmp信息存到TConfInfo中,bug解决后删除>
					if ( CONF_VMPMODE_AUTO == m_tConf.m_tStatus.GetVMPMode() )
					{
						tVmpParam.SetVMPAuto(TRUE);
						tVmpParam.SetVMPBrdst(m_tConf.m_tStatus.m_tVMPParam.IsVMPBrdst());
						tVmpParam.SetVMPSchemeId(m_tConf.m_tStatus.m_tVMPParam.GetVMPSchemeId());
						tVmpParam.SetIsRimEnabled((u8)m_tConf.m_tStatus.m_tVMPParam.GetIsRimEnabled());
					}*/
					//执行开启画面合成处理
					// 卫星会议需要check vmpparam [pengguofeng 1/17/2013]
					// CheckVmpParam会修改tVmpParam，此处用临时变量去判断 [pengguofeng 1/22/2013]
					TVMPParam_25Mem tTmpVmpParam = tVmpParam;
					if ( m_tConf.GetConfAttrb().IsSatDCastMode()
						&& !CheckVmpParam(byVmpId,/*tVmpParam*/tTmpVmpParam, wErrCode) )
					{
						m_tConfInStatus.SetVmpModuleUsed(TRUE); //  [pengguofeng 1/19/2013]防止后面changeVMpSwitch判断错
						LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[IsEqpInfoCheckPass]Sat Conf not Support VmpParam Error:%d\n", wErrCode);
					}
					else //开启VMP
					{
						//u8 byIsVmpModule = TRUE;
						CServMsg cServMsg = cSrcMsg;
						cServMsg.SetEqpId(byVmpId);
						cServMsg.SetEventId(MCS_MCU_STARTVMP_REQ);
						cServMsg.SetMsgBody( (u8 *)&tVmpParam, 	sizeof(tVmpParam) );
						//cServMsg.CatMsgBody( (u8 *)&byIsVmpModule, sizeof(u8) );
						ProcStartVmpReq(cServMsg);
					}
                }
                else
                {
                    if ( CONF_CREATE_NPLUS == m_byCreateBy )
                    {
                        NotifyMcsAlarmInfo( cSrcMsg.GetSrcSsnId(), ERR_MCU_ALLIDLEVMP_NO_SUPPORT_NPLUS );
                        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because all the idle vmp can't support the VMP style!\n", m_tConf.GetConfName() );

                        // N+1备份回滚创建的会议，如果本主端VMP能力小于备端，不恢复画面合成，
                        // 且要清空相关的数据。否则，该会议在备端再次恢复后会导致会议状态错乱。[12/28/2006-zbq]
                        if ( MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() )
                        {
                            m_tConf.m_tStatus.SetVmpBrdst( FALSE );
                            m_tConf.SetHasVmpModule( FALSE );
                            m_tConf.m_tStatus.SetVMPMode( CONF_VMPMODE_NONE );

                            //清空通道
                            for( u8 byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
                            {
                                m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );
                            }
                        }
                        else
                        {
                            //N+1备份恢复会议，按现在的VMP能力的逻辑和保护，不会来到这里
                            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf.%s created by NPlus error which is impossible, check it !\n", m_tConf.GetConfName() );
                        }
                    }
                    else
                    {
						RlsAllBasForConf();
                        g_cMcuVcApp.RemoveConf( m_byConfIdx );    
                        wErrCode = wError;
                        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because all the idle vmp can't support the VMP style!\n", m_tConf.GetConfName() );
                        return FALSE;
                    }
                }
            /*}
			//自动VMP直接设置画面合成参数  
			else
			{
                //当前所有的空闲VMP是否支持所需的合成风格
                u16 wError = 0;
                if ( IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError) ) 
                {
					//VMP确定后，马上填充 m_tVmpEqp [01/13/2006-zbq]
					m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
					m_tVmpEqp.SetConfIdx( m_byConfIdx );
					
                    u8 byMtNum = (u8)m_tConfAllMtInfo.GetAllMtNum();

					memset( &tVmpParam, 0 ,sizeof(tVmpParam) );
					tVmpParam.SetVMPAuto( TRUE );
					tVmpParam.SetVMPBrdst( m_tConf.m_tStatus.m_tVMPParam.IsVMPBrdst() );
					tVmpParam.SetVMPStyle( GetVmpDynStyle(byMtNum) );
                    tVmpParam.SetVMPSchemeId(m_tConf.m_tStatus.m_tVMPParam.GetVMPSchemeId());
					tVmpParam.SetIsRimEnabled((u8)m_tConf.m_tStatus.m_tVMPParam.GetIsRimEnabled());
                }
                else
                {
                    // N+1备份自动画面合成恢复失败，清理当前所有的状态 [01/16/2007-zbq]
                    if ( CONF_CREATE_NPLUS == m_byCreateBy ) 
                    {
                        NotifyMcsAlarmInfo( cSrcMsg.GetSrcSsnId(), ERR_MCU_ALLIDLEVMP_NO_SUPPORT_NPLUS );
                        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because all the idle vmp can't support the VMP style!\n", m_tConf.GetConfName() );
                        
                        // N+1备份回滚创建的会议，如果本主端VMP能力小于备端，不恢复画面合成，
                        // 且要清空相关的数据。否则，该会议在备端再次恢复后会导致会议状态错乱。[12/28/2006-zbq]
                        if ( MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() )
                        {
                            m_tConf.m_tStatus.SetVmpBrdst( FALSE );
                            m_tConf.SetHasVmpModule( FALSE );
                            m_tConf.m_tStatus.SetVMPMode( CONF_VMPMODE_NONE );
                            
                            //清空通道
                            for( u8 byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
                            {
                                m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );
                            }
                        }
                        else
                        {
                            //N+1备份恢复会议，按现在的VMP能力的逻辑和保护，不会来到这里
                            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf.%s created by NPlus error which is impossible, check it !\n", m_tConf.GetConfName() );
                        }                                
                    }
                    else
                    {
						RlsAllBasForConf();
                        //正常会议的创建，按现在的VMP能力的逻辑和保护，不会来到这里。保护起见。[01/16/2007-zbq]
                        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf.%s created which is impossible, check it !\n", m_tConf.GetConfName() );
                        wErrCode = ERR_MCU_NOIDLEVMP;
                        g_cMcuVcApp.RemoveConf( m_byConfIdx );    
                        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because no idle vmp!\n", m_tConf.GetConfName() );
                        return FALSE;
                    }
                }
			}
            
            TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_START;//先占用,超时后未成功再放弃
			tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpParam;
			tPeriEqpStatus.SetConfIdx( m_byConfIdx );
			g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			
			m_tConf.m_tStatus.SetVmpStyle( tVmpParam.GetVMPStyle() );
			
			u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType; //获取子类型
			u8 byMaxVmpMem = 0;
			if (VMP != byVmpSubType &&
				VMP_8KE != byVmpSubType) //新VMP最大成员数均为20 // [2/28/2010 xliang]  
			{
				byMaxVmpMem = MAXNUM_MPUSVMP_MEMBER;
			}
			else
			{
				byMaxVmpMem = MAXNUM_SDVMP_MEMBER;
			}
			//清空通道
			for( u8 byLoop = 0; byLoop < byMaxVmpMem; byLoop++ )
			{
				m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );
			}

			//开始画面合成        
            if (!tVmpParam.IsVMPAuto() ||
                (tVmpParam.IsVMPAuto() && tVmpParam.GetVMPStyle() != VMP_STYLE_NONE))
            {
				AdjustVmpParam(&tVmpParam, TRUE);
            }*/
		}
        else if( CONF_CREATE_MT == m_byCreateBy ) //终端创会不直接返回
        {
            m_tConf.m_tStatus.SetVMPMode(CONF_VMPMODE_NONE);
        }
		else if ( CONF_CREATE_NPLUS == m_byCreateBy )
        {
            // N+1备份创会在没有VMP的情况下，上边已经作过处理。此处直接跳过。
        }
        else
		{
			RlsAllBasForConf();
            g_cMcuVcApp.RemoveConf( m_byConfIdx );
			wErrCode = ERR_MCU_NOIDLEVMP;
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because no idle vmp!\n", m_tConf.GetConfName() );
			return FALSE;
		}				
	}

	// [2013/05/07 chenbing]  
	if ( m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo()
		&& m_tConf.m_tStatus.IsOngoing()
	   )
	{
		u8 byHduMode = HDUCHN_MODE_FOUR;
		for(u8 byLoopChnIdx = 0; byLoopChnIdx < (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL; byLoopChnIdx++)
		{
			if(m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].IsValid())
			{
				u8 byHduEqpId = m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_byHduEqpId;
				u8 byParentChnnlIdx = m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_byChIdx;
			
				CServMsg cSMg;
				cSMg.SetEqpId(byHduEqpId);
				cSMg.SetChnIndex(byParentChnnlIdx);
				cSMg.SetMsgBody((u8 *)&byHduMode, sizeof(u8));
				//切换HDU2对应的通道为四风格
				ChangeHduVmpMode(cSMg);
			}
		}
	}

//     //把会议指针存到数据区
//     if( !g_cMcuVcApp.AddConf( this ) )
//     {
// 		RlsAllBasForConf();
//         wErrCode = ERR_MCU_CONFOVERFLOW;
//         ConfLog( FALSE, "Conference %s create failed because of full instance!\n", m_tConf.GetConfName() );
//         return FALSE;
//     }

    return TRUE;
}

/*====================================================================
    函数名      ：GetConfExInfoFromMsg
    功能        ：从创会消息获得TConfInfoEx数据
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/12/28    4.0         chendaiwei    创建
====================================================================*/
BOOL32 CMcuVcInst::GetConfExInfoFromMsg(CServMsg &cServMsg, TConfInfoEx &tConfInfoEx)
{
	TConfInfo tConfInfo = *(TConfInfo*)(cServMsg.GetMsgBody());
    u16 wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
    s8 *pszAliaseBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16));
    
	u32 nLen = sizeof(TConfInfo)+sizeof(u16)+wAliasBufLen;

	TConfAttrb tConfAttrb = tConfInfo.GetConfAttrb();
	if(tConfAttrb.IsHasTvWallModule())
		nLen += sizeof(TMultiTvWallModule);
	if(tConfAttrb.IsHasVmpModule())
			nLen += sizeof(TVmpModule);

	//VCS信息[12/27/2011 chendaiwei]
	u8 * pbyVCSInfoBuf = cServMsg.GetMsgBody()+nLen;
	if (VCS_CONF == tConfInfo.GetConfSource())
	{	
		u8 byIsSupportHDTW = *pbyVCSInfoBuf++;
		if (byIsSupportHDTW)
		{
			pbyVCSInfoBuf += sizeof(THDTvWall);
		}
		
		u8 byHduNum = *pbyVCSInfoBuf++;
		if (byHduNum)
		{
			pbyVCSInfoBuf += byHduNum * sizeof(THduModChnlInfo);
		}
		
		u8 bySMCUExist = *pbyVCSInfoBuf++;
		if (bySMCUExist)
		{
			wAliasBufLen = htons(*(u16*)pbyVCSInfoBuf);
			pbyVCSInfoBuf += sizeof(u16);
			wAliasBufLen = ntohs(wAliasBufLen);
			pbyVCSInfoBuf += wAliasBufLen;					
		}
		
		// 会议模板中不包含分组信息
		pbyVCSInfoBuf++;
		//zhouyiliang 20100820 增加备份终端
		pbyVCSInfoBuf += sizeof(TMtAlias);
		
		// [4/20/2011 xliang] VCAutoMode
		pbyVCSInfoBuf ++;
	}

	//拷贝会议模板勾选信息[12/26/2011 chendaiwei]
	TConfInfoEx tOriginalEx = GetConfInfoExFromConfAttrb(tConfInfo);

	if( ntohs(*(u16*)pbyVCSInfoBuf) != 0 )
	{
		u16 wConfInfoExSize = 0;
		
		BOOL32 bExistUnknowType = FALSE;
		UnPackConfInfoEx(tConfInfoEx,pbyVCSInfoBuf,wConfInfoExSize,bExistUnknowType);
		TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];
		u8   byAudioCapNum = tConfInfoEx.GetAudioTypeDesc(atAudioTypeDesc);
		if (byAudioCapNum > 0)//会议扩展信息中有音频能力信息则需要用扩展信息中去覆盖
		{
			tOriginalEx.SetAudioTypeDesc(atAudioTypeDesc,byAudioCapNum);
		}
		tConfInfoEx.AddCapExInfo(tOriginalEx);

		return TRUE;
	}
	else
	{
		tConfInfoEx = tOriginalEx;

		return FALSE;
	}
}

void CMcuVcInst::NPlusVmpParamCoordinate (CServMsg &cServMsg)
{
	u8 *pBuf = cServMsg.GetMsgBody();
	TConfInfo tConfInfo = *(TConfInfo*)pBuf;
	pBuf+= sizeof(TConfInfo);

    u16 wAliasBufLen = ntohs( *(u16*)pBuf );
	pBuf+= sizeof(wAliasBufLen);
	pBuf+=wAliasBufLen;

	
	TConfAttrb tConfAttrb = tConfInfo.GetConfAttrb();
	if(tConfAttrb.IsHasTvWallModule())
		pBuf += sizeof(TMultiTvWallModule);
	if(tConfAttrb.IsHasVmpModule())
		pBuf += sizeof(TVmpModule);
	
	if (VCS_CONF == tConfInfo.GetConfSource())
	{	
		u8 byIsSupportHDTW = *pBuf++;
		if (byIsSupportHDTW)
		{
			pBuf += sizeof(THDTvWall);
		}
		
		u8 byHduNum = *pBuf++;
		if (byHduNum)
		{
			pBuf += byHduNum * sizeof(THduModChnlInfo);
		}
		
		u8 bySMCUExist = *pBuf++;
		if (bySMCUExist)
		{
			wAliasBufLen = htons(*(u16*)pBuf);
			pBuf += sizeof(u16);
			wAliasBufLen = ntohs(wAliasBufLen);
			pBuf += wAliasBufLen;					
		}
		
		// 会议模板中不包含分组信息
		pBuf++;
		//zhouyiliang 20100820 增加备份终端
		pBuf += sizeof(TMtAlias);
		
		// [4/20/2011 xliang] VCAutoMode
		pBuf ++;
	}
	
	if( tConfInfo.HasConfExInfo())
	{
		u16 wConfInfoExSize = 0;
		TConfInfoEx tConfInfoEx;
		BOOL32 bExistUnknowType = FALSE;
		UnPackConfInfoEx(tConfInfoEx,pBuf,wConfInfoExSize,bExistUnknowType);
		
		pBuf+=wConfInfoExSize;
	}
	
	//针对N+1创会，消息体附带SMCU INFO[11/20/2012 chendaiwei]
	if(m_byCreateBy == CONF_CREATE_NPLUS )
	{
		TSmcuCallnfo atSmcuCallInfo[MAXNUM_SUB_MCU];
		pBuf += sizeof(atSmcuCallInfo);

		TNPlusVmpParam tNlpusVmpparam[MAXNUM_PERIEQP];
		u8 byVmpNum = *pBuf;
		pBuf++;

		TNplusVmpModule atVmpModule[MAXNUM_PERIEQP];
		memcpy(&tNlpusVmpparam[0],pBuf,sizeof(tNlpusVmpparam[0])*byVmpNum);
		
		for ( u8 byVmpNumIdx = 0; byVmpNumIdx < byVmpNum; byVmpNumIdx ++ )
		{
			u8 byIdleEqpId = 0;

			u8 byEqualAbilityVmpId = 0;
			u8 bySlightlyLargerAbilityVmpId = 0;
			u8 byMinAbilityVmpId = 0;

			if(!IsVmpSupportNplus(tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam.m_byVmpSubType,tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam.m_byVMPStyle,byEqualAbilityVmpId,bySlightlyLargerAbilityVmpId,byMinAbilityVmpId))
			{
				continue;
			}
			
			//1模式
			if( g_cNPlusApp.GetLocalNPlusState() ==MCU_NPLUS_SLAVE_IDLE
				|| MCU_NPLUS_SLAVE_SWITCH ==  g_cNPlusApp.GetLocalNPlusState())
			{
				if(byEqualAbilityVmpId != 0)
				{
					byIdleEqpId = byEqualAbilityVmpId;
				}
				else
				{
					if( bySlightlyLargerAbilityVmpId != 0)
					{
						byIdleEqpId = bySlightlyLargerAbilityVmpId;
					}
				}
			}
			//N模式
			else
			{
				if(byMinAbilityVmpId != 0)
				{
					byIdleEqpId = byMinAbilityVmpId;
				}
				else if (byEqualAbilityVmpId != 0)
				{
					byIdleEqpId = byEqualAbilityVmpId;
				}
				else if ( bySlightlyLargerAbilityVmpId!= 0)
				{
					byIdleEqpId = bySlightlyLargerAbilityVmpId;
				}
			}
			
			if(byIdleEqpId == 0)
			{
				ConfPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"vmpStyle.%d VmpSubType.%d Find Matached Vmp failed!\n",
					tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam.m_byVMPStyle,
					tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam.m_byVmpSubType);
				continue;
			}
			else
			{
				ConfPrint(LOG_LVL_DETAIL,MID_MCU_NPLUS,"VmpStyle.%d SubType.%d Find Matached Vmp<%d> success!\n",
					tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam.m_byVMPStyle,
					tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam.m_byVmpSubType,
					byIdleEqpId);
			}

			atVmpModule[byVmpNumIdx].m_byEqpId = byIdleEqpId;

			TVMPParam_25Mem tVmpParam;
			TVMPMember tVmpMember;
			memcpy(&tVmpParam,&tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam,sizeof(TVmpBasicParam)-sizeof(u8));

			atVmpModule[byVmpNumIdx].m_byConfIdx = m_byConfIdx;
			
			//Auto不需要设置VMP成员
			if( tNlpusVmpparam[byVmpNumIdx].m_tVmpBaiscParam.m_byVMPAuto != 1)
			{
				for( u8 byChnIdx = 0; byChnIdx < MAXNUM_VMP_MEMBER; byChnIdx++ )
				{
					TMtAlias tTmpAlias;
					u8 byVmpMemberId = 0;
					u8 byMemberType = tNlpusVmpparam[byVmpNumIdx].m_tVmpMemer.m_tVmpChnnlInfo[byChnIdx].m_byMemberType;
					tTmpAlias.m_AliasType = mtAliasTypeTransportAddress;
					tTmpAlias.m_tTransportAddr = tNlpusVmpparam[byVmpNumIdx].m_tVmpMemer.m_tVmpChnnlInfo[byChnIdx].m_tMtInVmp.GetMtAddr();
					
					byVmpMemberId = m_ptMtTable->GetMtIdByAlias(&tTmpAlias);
					
					//byVmpMemberId == 0表示无成员，跟随类型仍然设置  [5/2/2013 chendaiwei]
					atVmpModule[byVmpNumIdx].m_byMemberType[byChnIdx] = byMemberType;

					TMt tMt;
					BOOL32 bValid = FALSE;
                    switch (byMemberType)
                    {
                    case VMP_MEMBERTYPE_SPEAKER:
						//处理发言人
						tMt = m_tConf.GetSpeaker();                                
                        bValid = TRUE;
                        break;
                    case VMP_MEMBERTYPE_CHAIRMAN:
                        // 处理主席
						tMt = m_tConf.GetChairman();                                
                        bValid = TRUE;
                        break;
                    case VMP_MEMBERTYPE_POLL:		// 轮询，创会时肯定没有轮询
                    case VMP_MEMBERTYPE_VMPCHLPOLL:	// vmp单通道轮询，创会时肯定没有vmp单通道轮询
                    case VMP_MEMBERTYPE_DSTREAM:	// 双流跟随，创会时肯定没有双流源
                        tMt.SetNull();
                        bValid = TRUE;
                        break;
                    default:
                        ConfPrint( LOG_LVL_WARNING, MID_MCU_NPLUS,  "[NplusVmpParamCoordinate]Invalid member type %d\n", 
							byMemberType);        
                        bValid = FALSE;
                    }
                    if (bValid && !tMt.IsNull())
                    {
						atVmpModule[byVmpNumIdx].m_byMtId[byChnIdx] = tMt.GetMtId();
						tVmpMember.SetMemberTMt(tMt);
                    }
					else
					{
						atVmpModule[byVmpNumIdx].m_byMtId[byChnIdx] = byVmpMemberId;
						tVmpMember.SetMemberTMt(m_ptMtTable->GetMt(byVmpMemberId));
					}
                
					tVmpMember.SetMemberType(byMemberType);
					tVmpParam.SetVmpMember(byChnIdx,tVmpMember);
				}
			}

			//记录当前占用VMP MOdule信息
			g_cMcuVcApp.NplusSaveVmpModuleByConfIdx(m_byConfIdx,&atVmpModule[byVmpNumIdx],1);

			CServMsg cMsg;
			cMsg.SetEqpId(byIdleEqpId);
			cMsg.SetEventId(MCS_MCU_STARTVMP_REQ);
			cMsg.SetMsgBody( (u8 *)&tVmpParam, 	sizeof(TVMPParam_25Mem) );
			ProcStartVmpReq(cMsg);
		}

		g_cMcuVcApp.NplusSaveVmpModuleByConfIdx(m_byConfIdx,atVmpModule,byVmpNum);
	}

	return;
}


/*====================================================================
    函数名      ：ConfDataCoordinate
    功能        ：创会会议数据整理
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	07/12/25    4.0         张宝卿          创建
    08/11/18    4.5         张宝卿          本地刷新启用适配的快捷支持
====================================================================*/
void CMcuVcInst::ConfDataCoordinate(CServMsg &cServMsg, u8 &byMtNum, u8 &byCreateMtId)
{
    //能力集整理
    TCapSupport tCapSupport = m_tConf.GetCapSupport();
    tCapSupport.SetSupportH224( TRUE );	
    if(m_tConf.GetConfAttrb().GetDataMode() != CONF_DATAMODE_VAONLY)
    {
        tCapSupport.SetSupportT120(TRUE);
    }
    else
    {
        tCapSupport.SetSupportT120(FALSE);
    }
    
    //zbq[01/04/2008] 强制清空FEC
    TCapSupportEx tCapEx = m_tConf.GetCapSupportEx();
    tCapEx.SetVideoFECType(FECTYPE_NONE);
    tCapEx.SetAudioFECType(FECTYPE_NONE);
    tCapEx.SetDVideoFECType(FECTYPE_NONE);
    m_tConf.SetCapSupportEx(tCapEx);

    //防止会控双流格式与主格式不一致
    if(VIDEO_DSTREAM_MAIN == tCapSupport.GetDStreamType())
    {
        tCapSupport.SetDStreamType(VIDEO_DSTREAM_MAIN);
    }

	if(VIDEO_DSTREAM_MAIN_H239 == tCapSupport.GetDStreamType())
	{
		tCapSupport.SetDStreamType(VIDEO_DSTREAM_MAIN_H239);
	}

    tCapSupport.SetEncryptMode(m_tConf.GetConfAttrb().GetEncryptMode());		
    tCapSupport.SetSupportMMcu(m_tConf.GetConfAttrb().IsSupportCascade()); 
    
    // guzh [6/6/2007] 能力集码率保护
    TSimCapSet  tSimCapSet;
    tSimCapSet = tCapSupport.GetMainSimCapSet();
    if ( tSimCapSet.GetVideoMaxBitRate() == 0 )
    {
        tSimCapSet.SetVideoMaxBitRate(m_tConf.GetBitRate());
        tCapSupport.SetMainSimCapSet(tSimCapSet);
    }
    tSimCapSet = tCapSupport.GetSecondSimCapSet();
    if ( tSimCapSet.GetVideoMediaType() != MEDIA_TYPE_NULL &&
        tSimCapSet.GetVideoMaxBitRate() == 0 )
    {
        tSimCapSet.SetVideoMaxBitRate(m_tConf.GetBitRate());
        tCapSupport.SetSecondSimCapSet(tSimCapSet);
    }
    
    m_tConf.SetCapSupport( tCapSupport );

    //处理终端列表
    
    //创建会议的终端别名
    TMtAlias *ptMtH323Alias = NULL;
    TMtAlias *ptMtE164Alias = NULL;
    TMtAlias *ptMtAddr = NULL;

    u8 byLoop = 0;
    u8 byMtId = 0;
    u8 byMtType = TYPE_MT;
    s8 *pszAliaseBuf = NULL;
    u16 wAliasBufLen  = 0;
    u16 awMtDialRate[MAXNUM_CONF_MT];
    s8 *pszMsgBuf = NULL;
    TMtAlias tMtAliasArray[MAXNUM_CONF_MT];
	BOOL32 bInMtTable = TRUE ; // xliang [12/26/2008] 
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();

    wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
    pszAliaseBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16));
    
    UnPackTMtAliasArray( pszAliaseBuf, wAliasBufLen, &m_tConf, 
                         tMtAliasArray, awMtDialRate, byMtNum );

	//得到创建者的别名
	if( CONF_CREATE_MT == m_byCreateBy )
	{

        u32 nLen = sizeof(TConfInfo)+sizeof(u16)+wAliasBufLen;
		if(tConfAttrb.IsHasTvWallModule())
			nLen += sizeof(TMultiTvWallModule);
		if(tConfAttrb.IsHasVmpModule())
			nLen += sizeof(TVmpModule);
		
		// wConfExInfoLength长度有可能为0，终端创会时无条件添加u16[11/14/2012 chendaiwei]
		u16 wConfExInfoLength = ntohs( *(u16*)(cServMsg.GetMsgBody() + nLen));
		nLen = nLen + wConfExInfoLength + sizeof(u16);
	
		if( cServMsg.GetMsgBodyLen() > nLen )
		{
			ptMtH323Alias = (TMtAlias *)(cServMsg.GetMsgBody()+nLen);
			ptMtE164Alias = (TMtAlias *)(cServMsg.GetMsgBody()+nLen)+1;
			ptMtAddr = (TMtAlias *)(cServMsg.GetMsgBody()+nLen)+2;
			byMtType = *(cServMsg.GetMsgBody()+nLen+sizeof(TMtAlias)*3);
			bInMtTable = *(BOOL32 *)(cServMsg.GetMsgBody()+nLen+sizeof(TMtAlias)*3+sizeof(u8));// xliang [12/26/2008] 
		}
	}

	//置无终端与会状态 
	m_tConfAllMtInfo.RemoveAllJoinedMt();

	for( byLoop=1; byLoop<=byMtNum; byLoop++)
	{
		//终端呼叫入会时，不论创会还是加入已有会议，均保留其呼入的DRI接入信息
		if( CONF_CREATE_MT == m_byCreateBy && 1 == byLoop )
		{
			if( byMtType == TYPE_MCU )
			{
				byMtId = AddMt( tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], CONF_CALLMODE_NONE );
			}
			else
			{
				byMtId = AddMt( tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], m_tConf.m_tStatus.GetCallMode() );
			}
			           
			if(!bInMtTable)
			{
				m_byMtIdNotInvite = byMtId; // xliang [12/26/2008] 表明该终端稍后不应作为受邀MT
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ConfdataCoordinate] Mt.%u should not be in Invite table\n", m_byMtIdNotInvite);
			}
				// zbq [12/19/2007] 直接创会终端须在此补入IP，避免E164优先情况下的漏填
            if ( ptMtAddr && tMtAliasArray[byLoop-1].m_AliasType != mtAliasTypeTransportAddress )
            {
                m_ptMtTable->SetMtAlias( byMtId, ptMtAddr );
                m_ptMtTable->SetIPAddr( byMtId, ptMtAddr->m_tTransportAddr.GetIpAddr() );                    
            }

            // xsl [11/8/2006] 接入板终端计数加1
			// xliang [2/4/2009] 区分是MT还是MCU, 是MCU则接入板终端计数需额外再+1
			u8 byDriId = cServMsg.GetSrcDriId();
//			u16 wExtraNum = (byMtType == TYPE_MCU)? 1: 0;
//			g_cMcuVcApp.IncMtAdpMtNum( byDriId, m_byConfIdx, byMtId, wExtraNum);
            g_cMcuVcApp.IncMtAdpMtNum( byDriId, m_byConfIdx, byMtId, byMtType);
            m_ptMtTable->SetDriId(byMtId, byDriId);
// 			if(byMtType == TYPE_MCU)
// 			{
// 				//接入板的终端计数还要+1
// 				//在mtadplib那端已通过判断，所以肯定有多于2个接入点让其占用
// 				g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum++;
// 			}
		}
		else
		{
			byMtId = AddMt( tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], m_tConf.m_tStatus.GetCallMode() );
		}
					
		if( CONF_CALLMODE_TIMER == m_tConf.m_tStatus.GetCallMode() && 
			DEFAULT_CONF_CALLTIMES != m_tConf.m_tStatus.GetCallTimes() )
		{
			m_ptMtTable->SetCallLeftTimes( byMtId, (m_tConf.m_tStatus.GetCallTimes() - 1) );
		}

		if( CONF_CREATE_MT == m_byCreateBy && 1 == byLoop )
		{
			byCreateMtId = byMtId;

			if(ptMtH323Alias != NULL)
			{
				m_ptMtTable->SetMtAlias(byMtId, ptMtH323Alias);
			}
			if(ptMtE164Alias != NULL)
			{
				m_ptMtTable->SetMtAlias(byMtId, ptMtE164Alias);
			}
            if(ptMtAddr != NULL)
            {
                m_ptMtTable->SetMtAlias(byMtId, ptMtAddr);
            }
			if(byMtType == TYPE_MCU) 
			{
				if( m_tConf.GetConfAttrb().IsSupportCascade() )
				{
					m_ptMtTable->SetMtType(byMtId, MT_TYPE_MMCU);
				}
				else
				{
					SetInOtherConf( TRUE,byMtId );
					m_ptMtTable->SetMtType(byMtId, MT_TYPE_MT);
				}
				
				//对于上级MCU，将作为非受邀终端，断开后将自动删除出列表
				m_ptMtTable->SetNotInvited( byMtId, TRUE );
                //m_ptMtTable->SetAddMtMode(byMtId, ADDMTMODE_MTSELF);
			}
			else
			{
				m_ptMtTable->SetMtType(byMtId, MT_TYPE_MT);
			}

            // guzh[5/23/2007]这里只能将其作为不在线终端
            // 添加，否则会导致下面画面合成成员判断出错.
            //m_tConfAllMtInfo.AddJoinedMt(byMtId);
            m_tConfAllMtInfo.AddMt(byMtId);
		}
	}

	//处理主席
    TMtAlias tMtAlias;
	tMtAlias = m_tConf.GetChairAlias();
	byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAlias );
	m_tConf.SetNoChairman();
	if( byMtId > 0 )
	{
        m_tConf.SetChairman( m_ptMtTable->GetMt(byMtId) );
	}

	//处理发言人
	tMtAlias = m_tConf.GetSpeakerAlias();
	byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAlias );
	m_tConf.SetNoSpeaker();
	if( byMtId > 0)
	{
        m_tConf.SetSpeaker( m_ptMtTable->GetMt(byMtId) );
	}

	pszMsgBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16) + wAliasBufLen);

	//处理电视墙模板
	if( tConfAttrb.IsHasTvWallModule() )
	{
        TPeriEqpStatus tTWStatus;
		TMultiTvWallModule *ptMultiTvWallModule = (TMultiTvWallModule *)pszMsgBuf;
        m_tConfEqpModule.SetMultiTvWallModule( *ptMultiTvWallModule );
        m_tConfEqpModule.SetTvWallInfo( *ptMultiTvWallModule );

        TTvWallModule tTvWallModule;
        u8 byMtIndex = 0;
        u8 byMemberType = 0;

		CServMsg CTvwEqpStatusMsg;
        for( u8 byTvLp = 0; byTvLp < ptMultiTvWallModule->GetTvModuleNum(); byTvLp++ )
        {
            ptMultiTvWallModule->GetTvModuleByIdx(byTvLp, tTvWallModule);

			//zjl[20091208]会议模板新增设置在hdu预案中设置终端成员
			u8 byEqpType  = tTvWallModule.GetTvEqp().GetEqpType();			
			u8 byTvWallId = tTvWallModule.GetTvEqp().GetEqpId();
			
			TTvwMember tTvwMbr;

			if (EQP_TYPE_HDU_SCHEME == byEqpType)
			{
				u8 byHduSchemeNum = 0;
				THduStyleInfo atHduStyleInfoTable[MAX_HDUSTYLE_NUM];
				u16 wRet = g_cMcuAgent.ReadHduSchemeTable(&byHduSchemeNum, atHduStyleInfoTable);
				if (SUCCESS_AGENT == wRet)
				{
					THduChnlInfo  atHduChnlInfo[MAXNUM_TVWALL_CHNNL_INSMOUDLE];

					u16 wTempLoop = 0;
					for ( wTempLoop = 0; wTempLoop < byHduSchemeNum && wTempLoop < MAX_HDUSTYLE_NUM; wTempLoop++)
					{
						if (atHduStyleInfoTable[wTempLoop].GetStyleIdx() == byTvWallId)
						{
							atHduStyleInfoTable[wTempLoop].GetHduChnlTable(atHduChnlInfo);
							break;
						}
					}
					
					if (wTempLoop >= byHduSchemeNum || byHduSchemeNum > MAX_HDUSTYLE_NUM)
					{
						continue;
					}

					//atHduStyleInfoTable[byTvWallId-1].GetHduChnlTable(atHduChnlInfo);
					
					for (u8 byChnl = 0; byChnl < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byChnl++)
					{
						byMtIndex    = tTvWallModule.m_abyTvWallMember[byChnl]; 
						byMemberType = tTvWallModule.m_abyMemberType[byChnl];

						if (!IsValidHduChn(atHduChnlInfo[byChnl].GetEqpId(), atHduChnlInfo[byChnl].GetChnlIdx()))
						{
							continue;
						}
						
						if(!g_cMcuVcApp.GetPeriEqpStatus(atHduChnlInfo[byChnl].GetEqpId(), &tTWStatus))
						{
							//ConfLog(FALSE, "[ConfDataCoordinate] unexpected PeriEqpStatus! HDU_SCHEME: %d, EQPID:%d,CHNLID:%d\n",
							//	byTvWallId, atHduChnlInfo[byChnl].GetEqpId(), atHduChnlInfo[byChnl].GetChnlIdx());
							continue;
						}
					
						tTvwMbr = tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()];
						
						//20100702_tzy 判断当前电视墙通道是否被其他会议占用，如果被占用则不能使用该通道
						if (0 != tTvwMbr.GetConfIdx()
							&& tTvwMbr.GetConfIdx() != m_byConfIdx)
						{
							ConfPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ConfDataCoordinate] TW<EqpId:%d, ChnId:%d> has been used by other conf:%d!\n", 
								atHduChnlInfo[byChnl].GetEqpId(), 
								atHduChnlInfo[byChnl].GetChnlIdx(),
								tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].GetConfIdx());
							continue;
						}
						
						if (byMtIndex > 0 )
						{
							if( byMtIndex <= MAXNUM_CONF_MT )
							{
								byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAliasArray[byMtIndex-1] );
								if (byMtId > 0)
								{
									TMt tMt = m_ptMtTable->GetMt( byMtId );
									m_tConfEqpModule.SetTvWallMemberInTvInfo( byTvWallId, byChnl, tMt);		
									
									// [2013/04/27 chenbing] 此段代码无意义，终端打开逻辑通道后会调用ChangeHduSwitch处理,可考虑删除
									// 已有终端在墙里, 先把旧的显示出墙
									if (tTvwMbr.GetConfIdx() == 0 || tTvwMbr.GetConfIdx() == m_byConfIdx)
									{
										RefreshMtStatusInTw( (TMt)tTvwMbr, FALSE, TRUE);
										
										tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].byMemberType = byMemberType;
										tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].SetConfIdx(m_byConfIdx);
										
										// [11/1/2011 liuxu] 预设进墙,显示在墙里面
										tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].SetMt(tMt);
										
										// 只有电视墙在线, 才会设置终端显示在墙里面
										if (tTWStatus.m_byOnline)
										{
											RefreshMtStatusInTw(tMt, TRUE, TRUE);
										}										
									}
								}
							}
							// 如果是 193，说明是其他跟随方式
							else if (byMtIndex == MAXNUM_CONF_MT + 1)
							{   
								TMt tMt;
								BOOL32 bValid = FALSE;
								switch (byMemberType)
								{
								case TW_MEMBERTYPE_SPEAKER:
									//处理发言人
									tMt = m_tConf.GetSpeaker();                                
									bValid = TRUE;
									break;

								case TW_MEMBERTYPE_CHAIRMAN:
									// 处理主席
									tMt = m_tConf.GetChairman();                                
									bValid = TRUE;
									break;

								case TW_MEMBERTYPE_POLL:
									// 轮询，创会时肯定没有轮询
									tMt.SetNull();
									bValid = TRUE;
									break;

									//新增会议模板hdu选看vmp，在启动vmp时会把码流打给hdu
								case TW_MEMBERTYPE_SWITCHVMP:
								    tMt.SetNull();
									bValid = TRUE;
									break;

								case TW_MEMBERTYPE_DOUBLESTREAM:
									tMt = m_tDoubleStreamSrc;
									bValid = TRUE;
									break;

								default:
									ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Invalid member type %d for idx %d in CreateConf TvWall Module\n", 
										byMemberType, 
										byLoop);        
									bValid = FALSE;
								}
								if (bValid)
								{
									// 标记下来，等终端上来。会触发主席发言人变更
									m_tConfEqpModule.SetTvWallMemberInTvInfo( tTvWallModule.m_tTvWall.GetEqpId(), byChnl, tMt );
									tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].byMemberType = byMemberType;
									tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].SetConfIdx(m_byConfIdx);
								}                            
							}
							else
							{
								ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Invalid MtIndex :%d in CreateConf TvWall Module\n", byMtIndex);
							}
						}

						g_cMcuVcApp.SetPeriEqpStatus(atHduChnlInfo[byChnl].GetEqpId(), &tTWStatus);

						if(tTWStatus.m_byOnline)
						{
							// 通知会控
							CTvwEqpStatusMsg.SetMsgBody((u8*)&tTWStatus, sizeof(tTWStatus));
							SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, CTvwEqpStatusMsg);
						}
					}
				}
				else
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "[ConfDataCoordinate] ReadHduSchemeTable failed!\n");
				}
			}
			else
			{
				g_cMcuVcApp.GetPeriEqpStatus(tTvWallModule.m_tTvWall.GetEqpId(), &tTWStatus);

				for( byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++ )
				{
					byMtIndex = tTvWallModule.m_abyTvWallMember[byLoop]; 
					byMemberType = tTvWallModule.m_abyMemberType[byLoop];
					tTvwMbr = tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop];
					
					if( byMtIndex > 0 )//索引以1为基准
					{
						if( byMtIndex <= MAXNUM_CONF_MT )
						{
							byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAliasArray[byMtIndex-1] );
							if( byMtId > 0 )
							{
								TMt tMt = m_ptMtTable->GetMt( byMtId );
								m_tConfEqpModule.SetTvWallMemberInTvInfo( tTvWallModule.m_tTvWall.GetEqpId(), byLoop, tMt );
								
								if (tTvwMbr.GetConfIdx() == 0 || tTvwMbr.GetConfIdx() == m_byConfIdx )
								{
									// [11/1/2011 liuxu] 防止冲突
									RefreshMtStatusInTw((TMt)tTvwMbr, FALSE, FALSE);
									
									// 顾振华 [5/11/2006] 这里支持指定跟随方式
									// tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = TW_MEMBERTYPE_MCSSPEC;
									tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byMemberType;
									tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
									
									// [11/1/2011 liuxu] 预设进墙,显示在墙里面
									tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetMt(tMt);
									
									// 只有电视墙在线, 才会设置终端显示在墙里面
									if (tTWStatus.m_byOnline)
									{
										RefreshMtStatusInTw(tMt, TRUE, FALSE);
									}
								}
							}
						}
						// 如果是 193，说明是其他跟随方式
						else if (byMtIndex == MAXNUM_CONF_MT + 1)
						{   
							TMt tMt;
							BOOL32 bValid = FALSE;
							switch (byMemberType)
							{
							case TW_MEMBERTYPE_SPEAKER:
								//处理发言人
								tMt = m_tConf.GetSpeaker();                                
								bValid = TRUE;
								break;
							case TW_MEMBERTYPE_CHAIRMAN:
								// 处理主席
								tMt = m_tConf.GetChairman();                                
								bValid = TRUE;
								break;
							case TW_MEMBERTYPE_POLL:
								// 轮询，创会时肯定没有轮询
								tMt.SetNull();
								bValid = TRUE;
								break;

							default:
								ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Invalid member type %d for idx %d in CreateConf TvWall Module\n", 
									byMemberType, 
									byLoop);        
								bValid = FALSE;
							}
							if (bValid)
							{
								// 标记下来，等终端上来。会触发主席发言人变更
								m_tConfEqpModule.SetTvWallMemberInTvInfo( tTvWallModule.m_tTvWall.GetEqpId(), byLoop, tMt );

								if (tTvwMbr.GetConfIdx() == 0 || tTvwMbr.GetConfIdx() == m_byConfIdx )
								{
									tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byMemberType;
									tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
								}
							}                            
						}
						else
						{
							ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Invalid MtIndex :%d in CreateConf TvWall Module\n", byMtIndex);
						}
					}

					g_cMcuVcApp.SetPeriEqpStatus(tTvWallModule.m_tTvWall.GetEqpId(), &tTWStatus);
					
					if(tTWStatus.m_byOnline)
					{
						// 通知会控
						CTvwEqpStatusMsg.SetMsgBody((u8*)&tTWStatus, sizeof(tTWStatus));
						SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, CTvwEqpStatusMsg);
					}
				}
			}
            
        }
		pszMsgBuf += sizeof(TMultiTvWallModule);
	}
    else
    {
        TMultiTvWallModule tMultiTvWallModule;
        memset( &tMultiTvWallModule, 0, sizeof(tMultiTvWallModule) );
        m_tConfEqpModule.SetMultiTvWallModule( tMultiTvWallModule );
	
        for( byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++ )
		{
			m_tConfEqpModule.m_tTvWallInfo[byLoop].ClearTvMember();
		}
    }

	//暂存基本的画面合成模板（20风格及以下）[3/15/2013 chendaiwei]
	TVmpModule tVmpModule;
	if( tConfAttrb.IsHasVmpModule() )
	{
		memcpy(&tVmpModule,pszMsgBuf,sizeof(tVmpModule));

		pszMsgBuf += sizeof(TVmpModule);
	}

	//拷贝会议模板勾选信息[12/26/2011 chendaiwei]
	//20130513 重复代码去除
	/*m_tConfEx.Clear();
	TConfInfoEx tTempEx = GetConfInfoExFromConfAttrb(m_tConf);
	if(m_tConf.HasConfExInfo())
	{
		u16 wConfInfoExSize = 0;
		BOOL32 bExistUnknowType = FALSE;
		UnPackConfInfoEx(m_tConfEx,(u8*)pszMsgBuf,wConfInfoExSize,bExistUnknowType,&m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall,&m_tConfEqpModule.m_tVmpModuleInfo);
		TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];
		u8   byAudioCapNum = m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
		if (byAudioCapNum > 0)//会议扩展信息中有音频能力信息则需要用扩展信息中去覆盖
		{
			tTempEx.SetAudioTypeDesc(atAudioTypeDesc,byAudioCapNum);
		}
		m_tConfEx.AddCapExInfo(tTempEx);
		pszMsgBuf += wConfInfoExSize;
	}
	else
	{
		m_tConfEx = tTempEx;
	}*/

	//解析25风格VMP模板和多画面电视墙模板信息[5/23/2013 chendaiwei]
	if(m_tConf.HasConfExInfo())
	{
		u16 wConfInfoExSize = 0;
		BOOL32 bExistUnknowType = FALSE;
		TConfInfoEx tTmpConfInfoEx;
		UnPackConfInfoEx(tTmpConfInfoEx,(u8*)pszMsgBuf,wConfInfoExSize,bExistUnknowType,&m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall,&m_tConfEqpModule.m_tVmpModuleInfo);
		pszMsgBuf += wConfInfoExSize;
	}

	//HDU多画面通道预案处理逻辑[3/8/2013 chendaiwei]
	//1.设置预案通道成员信息到TPeriEqpStatus中[不设置，策略待定]
	//2.m_tHduVmpModule中子通道成员由mtalias下标映射为MtId
	if(m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo())
	{
		for(u8 byLoopChnIdx = 0; byLoopChnIdx < (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL; byLoopChnIdx++)
		{
			if(m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].IsValid())
			{
				u8 byHduEqpId = m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_byHduEqpId;
				u8 byParentChnnlIdx = m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_byChIdx;
				for (u8 bySubChnlIdx = 0; bySubChnlIdx < HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnlIdx++)
				{
					TPeriEqpStatus tTWStatus;
					u8 byMtIndex =  m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_abyTvWallMember[bySubChnlIdx];
					u8 byMemberType = m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_abyMemberType[bySubChnlIdx];

					if (!IsValidHduChn(byHduEqpId,byParentChnnlIdx))
					{
						continue;
					}
					
					if(!g_cMcuVcApp.GetPeriEqpStatus(byHduEqpId, &tTWStatus))
					{
						continue;
					}

					//TODO:随TPeriEqpStatus修改而修改[2/28/2013 chendaiwei]
// 						tTvwMbr = tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()];
// 						
// 						//20100702_tzy 判断当前电视墙通道是否被其他会议占用，如果被占用则不能使用该通道
// 						if (0 != tTvwMbr.GetConfIdx()
// 							&& tTvwMbr.GetConfIdx() != m_byConfIdx)
// 						{
// 							ConfPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ConfDataCoordinate] TW<EqpId:%d, ChnId:%d> has been used by other conf:%d!\n", 
// 								atHduChnlInfo[byChnl].GetEqpId(), 
// 								atHduChnlInfo[byChnl].GetChnlIdx(),
// 								tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].GetConfIdx());
// 							continue;
// 						}
					
					if (byMtIndex > 0 )
					{
						if( byMtIndex <= MAXNUM_CONF_MT )
						{
							byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAliasArray[byMtIndex-1] );
							if (byMtId > 0)
							{
								TMt tMt = m_ptMtTable->GetMt( byMtId );
								//mt别名Idx转化成mt ID 设置接口
								m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.SetHduVmpSubChnlMember(byLoopChnIdx,bySubChnlIdx,byMtId);
				
								LogPrint(LOG_LVL_DETAIL,MID_MCU_CONF,"[Confdatacoordinate]HDU Vmp Module:byLoopChIdx.%d byEqpId.%d byChIdx.%d bySubChnlIdx.%d byMemberType.%d\n",byLoopChnIdx,byHduEqpId,
									m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_byChIdx,bySubChnlIdx,byMtId);

								//TODO:待TPeriEqpStaus定义好取消注释
								// 已有终端在墙里, 先把旧的显示出墙
// 									if (tTvwMbr.GetConfIdx() == 0 || tTvwMbr.GetConfIdx() == m_byConfIdx)
// 									{
// 										RefreshMtStatusInTw( (TMt)tTvwMbr, FALSE, TRUE);
// 										
// 										tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].byMemberType = byMemberType;
// 										tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].SetConfIdx(m_byConfIdx);
// 										
// 										// [11/1/2011 liuxu] 预设进墙,显示在墙里面
// 										tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].SetMt(tMt);
// 										
// 										// 只有电视墙在线, 才会设置终端显示在墙里面
// 										if (tTWStatus.m_byOnline)
// 										{
// 											RefreshMtStatusInTw(tMt, TRUE, TRUE);
// 										}										
// 									}
							}
						}
						//  [3/8/2013 chendaiwei] 如果是 193，说明是其他跟随方式 暂不支持跟随
						/*else if (byMtIndex == MAXNUM_CONF_MT + 1)
						{   
							TMt tMt;
							BOOL32 bValid = FALSE;
							switch (byMemberType)
							{
							case TW_MEMBERTYPE_SPEAKER:
								//处理发言人
								tMt = m_tConf.GetSpeaker();                                
								bValid = TRUE;
								break;

							case TW_MEMBERTYPE_CHAIRMAN:
								// 处理主席
								tMt = m_tConf.GetChairman();                                
								bValid = TRUE;
								break;

							case TW_MEMBERTYPE_POLL:
								// 轮询，创会时肯定没有轮询
								tMt.SetNull();
								bValid = TRUE;
								break;

								//新增会议模板hdu选看vmp，在启动vmp时会把码流打给hdu
							case TW_MEMBERTYPE_SWITCHVMP:
								tMt.SetNull();
								bValid = TRUE;
								break;

							case TW_MEMBERTYPE_DOUBLESTREAM:
								tMt = m_tDoubleStreamSrc;
								bValid = TRUE;
								break;

							default:
								ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Invalid member type %d for idx %d in CreateConf HDU vmp channl Module\n", 
									byMemberType, 
									byLoop);        
								bValid = FALSE;
							}
							if (bValid)
							{
								// 标记下来，等终端上来。会触发主席发言人变更
								m_tConfEqpModule.SetTvWallMemberInTvInfo( byHduEqpId, byParentChnnlIdx, tMt,bySubChnlIdx );
								
								tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].byMemberType = byMemberType;
								tTWStatus.m_tStatus.tHdu.atVideoMt[atHduChnlInfo[byChnl].GetChnlIdx()].SetConfIdx(m_byConfIdx);
							}                            
						}*/
						else
						{
							ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Invalid MtIndex :%d in CreateConf Hdu Vmp Module\n", byMtIndex);
						}
					}
					
// 					g_cMcuVcApp.SetPeriEqpStatus(byHduEqpId, &tTWStatus);
// 
// 					if(tTWStatus.m_byOnline)
// 					{
// 						// 通知会控
// 						CTvwEqpStatusMsg.SetMsgBody((u8*)&tTWStatus, sizeof(tTWStatus));
// 						SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, CTvwEqpStatusMsg);
// 					}
				}
			}
		}
	}
	else
	{
		m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.Clear();
	}

	//处理画面合成模板 
	if( tConfAttrb.IsHasVmpModule() )
	{		
        u8 byMtIndex = 0;
        u8 byMemberType = 0;
		TMt tMt;
        BOOL32 bValid = FALSE;
		for( byLoop = 0; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
		{
			if( byLoop < MAXNUM_MPUSVMP_MEMBER)
			{
				byMtIndex = tVmpModule.m_abyVmpMember[byLoop];
				byMemberType = tVmpModule.m_abyMemberType[byLoop];
			}
			else
			{
				byMtIndex = m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byLoop];
				byMemberType = m_tConfEqpModule.m_tVmpModuleInfo.m_abyMemberType[byLoop];
			}

			if( byMtIndex > 0 )
			{
                if(byMtIndex <= MAXNUM_CONF_MT)
                {
                    byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAliasArray[byMtIndex-1] );
                    if( byMtId > 0 )
                    {
                        tMt = m_ptMtTable->GetMt( byMtId );
						m_tConfEqpModule.SetVmpMember( byLoop, tMt ); 	
						m_tConfEqpModule.SetVmpMemberType(byLoop,byMemberType);
                    }
                }	
                // 如果是 193，说明是其他跟随方式
                else if (byMtIndex == MAXNUM_CONF_MT + 1)
                {
					bValid = FALSE;
                    switch (byMemberType)
                    {
                    case VMP_MEMBERTYPE_SPEAKER:
						//处理发言人
						tMt = m_tConf.GetSpeaker();                                
                        bValid = TRUE;
                        break;
                    case VMP_MEMBERTYPE_CHAIRMAN:
                        // 处理主席
						tMt = m_tConf.GetChairman();                                
                        bValid = TRUE;
                        break;
                    case VMP_MEMBERTYPE_POLL:		// 轮询，创会时肯定没有轮询
                    case VMP_MEMBERTYPE_VMPCHLPOLL:	// vmp单通道轮询，创会时肯定没有vmp单通道轮询
                    case VMP_MEMBERTYPE_DSTREAM:	// 双流跟随，创会时肯定没有双流源
                        tMt.SetNull();
                        bValid = TRUE;
                        break;
                    default:
                        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Invalid member type %d for idx %d in CreateConf Vmp Module\n", 
							byMemberType, 
							byLoop);        
                        bValid = FALSE;
                    }
                    if (bValid)
                    {
						//跟随且无成员 Member仍然是193
						if(!tMt.IsNull())
						{
							m_tConfEqpModule.SetVmpMember( byLoop, tMt );
						}
						else
						{
							m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byLoop] = MAXNUM_CONF_MT + 1;
						}

						m_tConfEqpModule.SetVmpMemberType(byLoop,byMemberType);
                    }                          
                }
                else
                {
                    ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Invalid MtIndex :%d in CreateConf Vmp Module\n", byMtIndex);
                }
			}
		}
		
		m_tConfEqpModule.SetVmpModuleParam( tVmpModule.m_tVMPParam );
	}
    else
    {
        TVmpModuleInfo tVmpModuleInfo;
        memset( &tVmpModuleInfo, 0, sizeof(tVmpModuleInfo) );
        m_tConfEqpModule.SetVmpModule( tVmpModuleInfo );
    }

	//针对N+1创会，消息体附带SMCU INFO[11/20/2012 chendaiwei]
	if(m_byCreateBy == CONF_CREATE_NPLUS )
	{
		TSmcuCallnfo atSmcuCallInfo[MAXNUM_SUB_MCU];
		memcpy(&atSmcuCallInfo[0],pszMsgBuf,sizeof(atSmcuCallInfo));
		for( u8 byTmpMtId = 1; byTmpMtId <= byMtNum; byTmpMtId++)
		{
			for(u8 bySmcuIdx = 0; bySmcuIdx < MAXNUM_SUB_MCU; bySmcuIdx++)
			{
				if(tMtAliasArray[byTmpMtId-1].m_AliasType == mtAliasTypeE164
				  && 0 == memcmp(atSmcuCallInfo[bySmcuIdx].m_achAlias,tMtAliasArray[byTmpMtId-1].m_achAlias,sizeof(tMtAliasArray[byTmpMtId-1].m_achAlias)))
				{
					u8 byId = m_ptMtTable->GetMtIdByAlias(&tMtAliasArray[byTmpMtId-1]);
					TMtAlias tTmpDialAlias;
					if( m_ptMtTable->GetDialAlias(byId,&tTmpDialAlias))
					{
						tTmpDialAlias.m_tTransportAddr = atSmcuCallInfo[bySmcuIdx].m_dwMtAddr;
						m_ptMtTable->SetDialAlias(byId,&tTmpDialAlias);

						LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[confdatacoordinate]Mt.%d DialAlias<Type:E164,Alias:%s,Ip:0x%x,Port:%d>\n",byId,tTmpDialAlias.m_achAlias,tTmpDialAlias.m_tTransportAddr.GetIpAddr(),tTmpDialAlias.m_tTransportAddr.GetPort());
					}

					break;
				}
			}
		}

		pszMsgBuf += sizeof(atSmcuCallInfo);
	}

	if(tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264
		&& tCapSupport.GetDStreamMediaType() == MEDIA_TYPE_H264)
	{
		TDStreamCap tTempDsCap = tCapSupport.GetDStreamCapSet();
		
#ifdef _8KH_	
		//tTempDsCap.SetH264ProfileAttrb(tCapSupport.GetMainStreamProfileAttrb());
#elif defined _8KE_
		//3. 8KG双流始终为BP(此处保护)
		tTempDsCap.SetH264ProfileAttrb(emBpAttrb);
#else      
		//4. 8000A依赖于主流 
		//tTempDsCap.SetH264ProfileAttrb(tCapSupport.GetMainStreamProfileAttrb());
#endif
		tCapSupport.SetDStreamCapSet(tTempDsCap);

		m_tConf.SetCapSupport(tCapSupport);
	}

#if defined (_8KH_) || defined (_8KE_) || defined (_8KI_)
	TVideoStreamCap atDSVideoCap[MAX_CONF_CAP_EX_NUM];
	u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;

	ConstructDSCapExFor8KH8KE(tCapSupport,atDSVideoCap);
	m_tConfEx.SetDoubleStreamCapEx(atDSVideoCap,MAX_CONF_CAP_EX_NUM);
#endif

    //zbq[11/18/2008] 启用适配的快捷支持
    TConfAttrb tAttrb = m_tConf.GetConfAttrb();
    
    if (0 != m_tConf.GetSecBitRate() ||
        (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() &&
         0 != m_tConf.GetSecVideoMediaType()))
    {
        tAttrb.SetUseAdapter(TRUE);
    }
    else
    {	//是否有扩展能力集勾选[12/14/2011 chendaiwei]
        if ( m_tConfEx.HasCapEx()
            ||
            (m_tConf.GetCapSupportEx().GetSecDSType() != MEDIA_TYPE_NULL &&
             m_tConf.GetCapSupportEx().GetSecDSType() != 0)
            ||
            (m_tConf.GetSecAudioMediaType() != MEDIA_TYPE_NULL &&
             m_tConf.GetSecAudioMediaType() != 0 &&            
             m_tConf.GetSecAudioMediaType() != m_tConf.GetMainAudioMediaType())
           )
        {
            tAttrb.SetUseAdapter(TRUE);
        }
        else
        {
            tAttrb.SetUseAdapter(FALSE);
        }
    }
    m_tConf.SetConfAttrb(tAttrb);
	
	g_cMcuVcApp.UpdateAgentAuthMtNum();
    
    //FIXME: 临时处理
//     if (!m_tConf.GetCapSupport().GetSecondSimCapSet().IsNull() &&
//         m_tConf.GetSecVidFrameRate() < 25 )
//     {
//         TSimCapSet tSim2 = m_tConf.GetCapSupport().GetSecondSimCapSet();
//         tSim2.SetVideoFrameRate(25);
//         TCapSupport tConfCap = m_tConf.GetCapSupport();
//         tConfCap.SetSecondSimCapSet(tSim2);
//         m_tConf.SetCapSupport(tConfCap);
//     }

    return;
}

/*====================================================================
    函数名      ：ProcConfRegGkAck
    功能        ：成功注册GK处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/07/13    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcConfRegGkAck( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

    if(m_tConf.m_tStatus.IsRegToGK() &&
       !g_cMcuVcApp.GetRRQDriTransed())
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_GUARD, "[ProcConfRegGkAck] ignore it due to status is ok!\n");
		return;

	}

	m_tConf.m_tStatus.SetRegToGK( TRUE );
	
    u8 byReg = 1;
    cServMsg.SetMsgBody(&byReg, sizeof(byReg));
	SendMsgToAllMcs( MCU_MCS_CONFREGGKSTATUS_NOTIF, cServMsg );
	//通知所有会控，通知ConfMode就可以了
    // 顾振华 [4/30/2006] 可以不用通知
	//cServMsg.SetMsgBody( ( u8 * )&( m_tConf.m_tStatus.m_tConfMode ), sizeof( TConfMode ) );
	//SendMsgToAllMcs( MCU_MCS_CONFMODE_NOTIF, cServMsg );

	// 会议注册GK以后才邀请终端
	// GK 注册的ACK到来，无条件尝试呼叫当前不在线的终端
	// if(m_tConfInStatus.IsInviteOnGkReged())
	{		
        cServMsg.SetServMsg(m_abySerHdr, sizeof(m_abySerHdr));
        cServMsg.SetNoSrc();
        cServMsg.SetTimer( TIMESPACE_WAIT_AUTOINVITE );
        cServMsg.SetMsgBody( NULL, 0 );
        InviteUnjoinedMt( cServMsg );

		m_tConfInStatus.SetInviteOnGkReged(FALSE);
	}
}

/*====================================================================
    函数名      ：ProcConfRegGkNack
    功能        ：未成功注册GK处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/07/13    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcConfRegGkNack( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );    

	if( m_tConf.m_tStatus.IsRegToGK() || m_tConfInStatus.IsRegGkNackNtf() )
	{        
        m_tConf.m_tStatus.SetRegToGK( FALSE );
        
        u8 byReg = 0;
        cServMsg.SetMsgBody(&byReg, sizeof(byReg));
		SendMsgToAllMcs( MCU_MCS_CONFREGGKSTATUS_NOTIF, cServMsg );		

		m_tConfInStatus.SetRegGkNackNtf(FALSE);
	}
}

/*=============================================================================
函 数 名： ProcGKChargeRsp
功    能： 会议计费响应处理
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/17  4.0			许世林                  创建
2006/12/26  4.0         张宝卿                  支持N+1模式下信息的保存和发送
=============================================================================*/
void CMcuVcInst::ProcGKChargeRsp( const CMessage * pcMsg )
{
    if ( STATE_ONGOING != CurState() )
    {
        return;
    }
    
    CServMsg cServMsg( pcMsg->content, pcMsg->length );

    switch( pcMsg->event )
    {
    case MT_MCU_CONF_STARTCHARGE_ACK:
        {
            TAcctSessionId tSsnId = *(TAcctSessionId*)cServMsg.GetMsgBody();
            memcpy(&m_tChargeSsnId, &tSsnId, sizeof(m_tChargeSsnId));
            
            m_tConf.m_tStatus.SetGkCharge(TRUE);
            
            // N+1模式下计费信息变更同步到备份MCU
            if ( MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() )
            {
                TNPlusConfData tConfData;
				TNPlusVmpParam tNplusVmpParam[MAXNUM_PERIEQP];
				u8 byVmpNum = 0;
				
				GetNPlusDataFromConf( tConfData,&tNplusVmpParam[0],byVmpNum);
				
				TNPlusConfExData tConfExData;
				u16 wPackDataLen = GetNPlusDataExFromConf(tConfExData);
				
                cServMsg.SetConfId( m_tConf.GetConfId() );
                cServMsg.SetMsgBody( (u8*)&tConfData, sizeof(tConfData) );
				cServMsg.CatMsgBody(tConfExData.m_byConInfoExBuf, wPackDataLen);
				cServMsg.CatMsgBody((u8*)&tConfExData.m_atSmcuCallInfo[0], sizeof(tConfExData.m_atSmcuCallInfo));

				cServMsg.CatMsgBody((u8*)&byVmpNum,sizeof(byVmpNum));
				for( u8 byIdx = 0; byIdx < byVmpNum; byIdx ++ )
				{
					cServMsg.CatMsgBody((u8*)&tNplusVmpParam[byIdx].m_tVmpBaiscParam,sizeof(tNplusVmpParam[byIdx].m_tVmpBaiscParam));
					u8 byChnNum = tNplusVmpParam[byIdx].m_tVmpBaiscParam.GetMaxMemberNum();
					cServMsg.CatMsgBody((u8*)&byChnNum,sizeof(u8));
					cServMsg.CatMsgBody((u8*)&tNplusVmpParam[byIdx].m_tVmpMemer.m_tVmpChnnlInfo[0],byChnNum*sizeof(TNPlusVmpChnlMember));
				}
					
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS, "[ProcGkChargeRsp] conf:%s VmpNum:%d\n", m_tConf.GetConfName(),byVmpNum);

                cServMsg.SetEventId( MCU_NPLUS_CONFDATAUPDATE_REQ );
                
                g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
            }
        }
    	break;

    case MT_MCU_CONF_STOPCHARGE_ACK:
        
        // N+1模式下, 此时会议已经结束, N+1备份会清理相关信息. 此处只清理会议信息
        memset(&m_tChargeSsnId, 0, sizeof(m_tChargeSsnId));
    	break;

    case MT_MCU_CONF_CHARGEEXP_NOTIF:
    case MT_MCU_CONF_STARTCHARGE_NACK:

        m_tConf.m_tStatus.SetGkCharge(FALSE);
        
        // gk计费认证失败或者计费发生异常直接结束会议[11/21/2006-zbq]
        ReleaseConf();
        NEXTSTATE( STATE_IDLE );
        break;
        
    case MT_MCU_CHARGE_REGGK_NOTIF:
        
        if ( m_tConf.IsSupportGkCharge() && !m_tConf.m_tStatus.IsGkCharge() ) 
        {
            u8 byMtNum = 0;
            for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
            {
                // zbq [03/26/2007] N+1创会，或者是会议恢复，能邀请的终端肯定是邀请过的或是列表内的。即时此时邀请失败。
                if ( m_tConfAllMtInfo.MtInConf(byMtId) )
                {
                    byMtNum ++;
                }
            }
            g_cMcuVcApp.ConfChargeByGK( m_byConfIdx, g_cMcuVcApp.GetRegGKDriId(), FALSE, m_byCreateBy, byMtNum );
        }
        break;

    default:
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "[ProcGKChargeRsp] unexpected msg.%d<%s> received !\n", 
                         pcMsg->event, OspEventDesc(pcMsg->event) );
        break;
    }

    return;
}

/*====================================================================
    函数名      ：ProcMcsMcuReleaseConfReq
    功能        ：结束会议处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/03    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuReleaseConfReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

    TMt tApplyListMt[MAXNUM_CONF_MT];
    u8 byListLen = 0;
	switch( CurState() )
	{
	case STATE_ONGOING:
	case STATE_SCHEDULED:
		//来自非主席终端，nack 
		if( cServMsg.GetEventId() == MT_MCU_DROPCONF_CMD && 
			cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId() )
		{
			//取消其主席 
			SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
			return;
		}

		// 终端不可结束VCS会议
		if (cServMsg.GetEventId() == MT_MCU_DROPCONF_CMD && 
			VCS_CONF == m_tConf.GetConfSource())
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMcsMcuReleaseConfReq] VCS Conf cann't be released by mt\n");
			return;
		}
		// 对于VCS会议由终端开启的会议(即处于级联调度中)不可由ui释放会议
		if (CONF_CREATE_MT == m_byCreateBy &&
			VCS_CONF == m_tConf.GetConfSource() &&
			cServMsg.GetEventId() == MCS_MCU_RELEASECONF_REQ)
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "[ProcMcsMcuReleaseConfReq]vcs conf in cascade can't be released by ui\n");
			return;
		}
		
		
		//应答
		if( cServMsg.GetEventId() == MCS_MCU_RELEASECONF_REQ )
		{		
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}    
        
        //通告发言申请列表中终端发言申请状态
        m_tApplySpeakQue.GetQueueList(tApplyListMt, byListLen);
        if (byListLen <= MAXNUM_CONF_MT)
        {
            for (u8 byPos = 0; byPos < byListLen; byPos++)
            {
                if (!tApplyListMt[byPos].IsNull())
                {
                    NotifyMtSpeakStatus(tApplyListMt[byPos], emDenid);
                }
            }
        }

		//结束会议
		ReleaseConf( TRUE );
        
        NEXTSTATE( STATE_IDLE );
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuChangeLockModeConfReq
    功能        ：会议控制台要求改变会议保护方式
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/13    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuChangeLockModeConfReq( const CMessage * pcMsg )
{
    STATECHECK;
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byLockMode = *(u8 *)cServMsg.GetMsgBody();

	//无效的会议保护方式,NACK
	if (CONF_LOCKMODE_NONE    != byLockMode && 
		CONF_LOCKMODE_NEEDPWD != byLockMode &&
        CONF_LOCKMODE_LOCK    != byLockMode)
	{
		cServMsg.SetErrorCode(ERR_MCU_INVALID_CONFLOCKMODE);
		SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
		return;
	}
	//ACK
	SendReplyBack(cServMsg, cServMsg.GetEventId()+1);

    ChangeConfLockMode(cServMsg);

	return;
}

/*====================================================================
    函数名      ：ChangeConfLockMode
    功能        ：会议锁定模式变更处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CServMsg &cServMsg
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	07/11/22    4.0         张宝卿        创建
====================================================================*/
void CMcuVcInst::ChangeConfLockMode( CServMsg &cServMsg )
{
    u8 byLockMode = *(u8*)cServMsg.GetMsgBody();
    
    //设置会议保护状态
    m_tConf.m_tStatus.SetProtectMode(byLockMode);
    
    if (CONF_LOCKMODE_LOCK == m_tConf.m_tStatus.GetProtectMode())
    {
        m_tConfProtectInfo.SetLockByMcs(cServMsg.GetSrcSsnId());
        TMcsRegInfo tMcsRegInfo;
        g_cMcuVcApp.GetMcsRegInfo(cServMsg.GetSrcSsnId(), &tMcsRegInfo);
        cServMsg.SetMsgBody((u8*)&tMcsRegInfo.m_dwMcsIpAddr, sizeof(u32));
        cServMsg.CatMsgBody((u8*)tMcsRegInfo.m_achUser, MAXLEN_PWD);
        
        //FIXME: 这里关于点名锁定的提示不提示给自己有MCS保护
        SendMsgToAllMcs(MCU_MCS_LOCKUSERINFO_NOTIFY, cServMsg);
    }
    else if (CONF_LOCKMODE_NEEDPWD == m_tConf.m_tStatus.GetProtectMode())
    {
        m_tConfProtectInfo.SetMcsPwdPassed(cServMsg.GetSrcSsnId(), TRUE);
    }
	//zbq[06/03/2008] 会议模式转为开放，则清空历史密码
	else if (CONF_LOCKMODE_NONE == m_tConf.m_tStatus.GetProtectMode())
	{
		m_tConfProtectInfo.ResetMcsPwdPassed();
		m_tConf.SetConfPwd(NULL);

		//实时刷新会控
		cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
		if(m_tConf.HasConfExInfo())
		{
			u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
			u16 wPackDataLen = 0;
			PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
			cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
		}
		SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
	}
    
    //通知所有会控，除了发起端
    cServMsg.SetMsgBody(&byLockMode, sizeof(byLockMode));
    SendMsgToAllMcs(MCU_MCS_CONFLOCKMODE_NOTIF, cServMsg);

    return;
}

/*====================================================================
    函数名      ：ProcMcsMcuGetLockInfoReq
    功能        ：会控要求得到会议独享信息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/13    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetLockInfoReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch (CurState())
	{
	case STATE_ONGOING:

		//若会议被独享 
        cServMsg.SetMsgBody();
		if (CONF_LOCKMODE_LOCK == m_tConf.m_tStatus.GetProtectMode())
		{
			if (m_tConfProtectInfo.IsLockByMcs())
			{
				TMcsRegInfo tMcsRegInfo;
				g_cMcuVcApp.GetMcsRegInfo( m_tConfProtectInfo.GetLockedMcSsnId(), &tMcsRegInfo );
				cServMsg.SetMsgBody( (u8*)&tMcsRegInfo.m_dwMcsIpAddr, sizeof(u32) );
				cServMsg.CatMsgBody( (u8*)tMcsRegInfo.m_achUser, MAXLEN_PWD );
			}
			else if(m_tConfProtectInfo.IsLockByMcu())
			{
                u8 byMcuId = m_tConfProtectInfo.GetLockedMcuId();
				TMtAlias tMtAlias;
				m_ptMtTable->GetMtAlias(byMcuId, 
					mtAliasTypeTransportAddress,
					&tMtAlias);
				
				cServMsg.SetMsgBody( (u8*)&tMtAlias.m_tTransportAddr.m_dwIpAddr, sizeof(u32) );
				if (! m_ptMtTable->GetMtAlias(byMcuId, 
					                          mtAliasTypeH323ID,
					                          &tMtAlias))
				{
					if (!m_ptMtTable->GetMtAlias(byMcuId, 
				                          		 mtAliasTypeE164,
						                         &tMtAlias))
					{
						strncpy( tMtAlias.m_achAlias, "mcu", sizeof(tMtAlias.m_achAlias) );
					}
				}
				tMtAlias.m_achAlias[MAXLEN_PWD-1] = 0;
				cServMsg.CatMsgBody( (u8*)tMtAlias.m_achAlias, MAXLEN_PWD );	
            }
            SendReplyBack(cServMsg, cServMsg.GetEventId()+1);	
		}		
		else
		{
			SendReplyBack(cServMsg, cServMsg.GetEventId()+2);	
		}
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
				pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
		break;
	}

	return;
}

/*====================================================================
    函数名      ：ProcMcsMcuEnterPwdRsp
    功能        ：会议控制台的输入密码应答
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/13    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuEnterPwdRsp( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	char *pchPWD = (char *)cServMsg.GetMsgBody();

	if( pcMsg->event == MCS_MCU_ENTERCONFPWD_ACK )
	{
       if( (memcmp( pchPWD, m_tConf.GetConfPwd() , MAXLEN_PWD ) == 0 ) )
	   {
		   m_tConfProtectInfo.SetMcsPwdPassed(cServMsg.GetSrcSsnId(), TRUE);   
	   }
	   else
	   {
		   //通知会控密码错误
		   SendMsgToMcs(  cServMsg.GetSrcSsnId(), MCU_MCS_WRONGCONFPWD_NOTIF, cServMsg );
	   }
	}

}

/*====================================================================
    函数名      ：ProcMcsMcuChangeConfPwdReq
    功能        ：会议控制台请求MCU修改会议密码
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/13    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuChangeConfPwdReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	CServMsg cServMsgNtf;	
	s8 *pchPWD = (s8 *)cServMsg.GetMsgBody();
	s8 szPwd[MAXLEN_PWD+1];

	switch( CurState() )
	{
	case STATE_ONGOING:	
	case STATE_SCHEDULED:
		astrncpy(szPwd, m_tConf.GetConfPwd(),sizeof(szPwd), MAXLEN_PWD+1); 
		//修改密码
		m_tConf.SetConfPwd( pchPWD );
		
		//ACK
		SendReplyBack( cServMsg, cServMsg.GetEventId()+1 );

		//通知所有会控
		SendMsgToAllMcs( MCU_MCS_CHANGECONFPWD_NOTIF, cServMsg );
		
		//更改会控的密码保护
		//zbq[06/03/2008] memcmp取的长度不对
		//if(memcmp(pchPWD, szPwd, strlen(szPwd)) != 0)
		if(strcmp(pchPWD, szPwd) != 0)
		{
			if(m_tConf.GetStatus().GetConfMode().GetLockMode() == CONF_LOCKMODE_NEEDPWD)
			{
                m_tConfProtectInfo.ResetMcsPwdPassed();
                m_tConfProtectInfo.SetMcsPwdPassed(cServMsg.GetSrcSsnId());
			}
		}
		
		g_cMcuVcApp.RefreshHtml();

		//MCU同步终端适配层该会议的会议密码，用于合并级联请求的认证校验
		cServMsgNtf.SetMsgBody((u8*)m_tConf.GetConfPwd(), MAXLEN_PWD);
		cServMsgNtf.SetEventId(MCU_MT_CONFPWD_NTF);
		BroadcastToAllSubMtJoinedConf( MCU_MT_CONFPWD_NTF, cServMsgNtf );

		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuSaveConfReq
    功能        ：保存即时会议信息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/03/10    1.0         Qzj		      创建
	03/11/13    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcMcsMcuSaveConfReq(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt tMt;
	TMtAlias tMtAlias;
	u8   bySaveDefault;

	switch( CurState() )
	{

	case STATE_SCHEDULED:

		//预约会议可修改会议时会保存会议信息，因此这里不需要再保存，只会ACK
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		break;

	case STATE_ONGOING:

		bySaveDefault = *(u8*)cServMsg.GetMsgBody();
		
		// 应答 
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
        
        // guzh [8/29/2006] 只允许超级管理员保存为缺省会议
        if (bySaveDefault)
        {
               if (USRGRPID_SADMIN != CMcsSsn::GetUserGroup(cServMsg.GetSrcSsnId()) )
               {
                   // 通知没有能够保存为缺省
                   NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_SAVEDEFCONF_NOPERMIT );
                   bySaveDefault = FALSE;
               }
        }
		
		// 保存发言人
		tMt = GetLocalSpeaker();
		if( m_tConf.HasSpeaker() && tMt.GetType() == TYPE_MT )
		{
            // zbq [08/08/2007] 发言人优先保存其呼叫别名
			if ( m_ptMtTable->GetDialAlias( tMt.GetMtId(), &tMtAlias )
                 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias ) 
				 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias)
				 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeE164, &tMtAlias) 
				 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias) )
			{
				m_tConf.SetSpeakerAlias( tMtAlias );
			}
			else
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "\n\n Cannot get speaker's Mt Alias.\n\n");
			}
		}

		// 保存主席 
		tMt = m_tConf.GetChairman();
		if( m_tConf.HasChairman() && tMt.GetType() == TYPE_MT )
		{
            // zbq [08/08/2007] 主席优先保存其呼叫别名
			if ( m_ptMtTable->GetDialAlias( tMt.GetMtId(), &tMtAlias )
                 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias )
				 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias)
				 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeE164, &tMtAlias) 
				 || m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias) )
			{
				m_tConf.SetChairAlias( tMtAlias );
			}
			else
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "\n\n Cannot get chairman's Mt Alias.\n\n");
			}
		}
		
		//保存到文件
		g_cMcuVcApp.SaveConfToFile( m_byConfIdx, FALSE, bySaveDefault );

		break;

	default :
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuModifyConfReq
    功能        ：修改预约会议处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/03    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuModifyConfReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TConfInfo	tConf;
	TConfAttrb  tConfAttrib;

	switch( CurState() )
	{
	case STATE_SCHEDULED:

		//得到消息并设置会议号
		tConf = *( TConfInfo * )cServMsg.GetMsgBody();		
		tConfAttrib = tConf.GetConfAttrb();	

		//周期性预约会议
		if ( tConf.IsCircleScheduleConf() )
		{
			//解析修改后的有效时间
			TConfInfoEx tConfInfoEx;
			TDurationDate tDuraDate;
			if ( GetConfExInfoFromMsg(cServMsg,tConfInfoEx) )
			{
				tDuraDate = tConfInfoEx.GetDurationDate();
				//保证durastart的时分秒为00:00:00,duraend的时分秒为23:59:59
				ModifyCircleScheduleConfDuraDate(tDuraDate);
				
			}
			TKdvTime tNextStarttime = GetNextOngoingTime(tConf,tDuraDate,TRUE);
			TKdvTime tNullTime;//构造就是memset为0
			//找不到下次开启的时间,开不了会，return 
			if ( tNullTime == tNextStarttime )
			{

				cServMsg.SetErrorCode( (u16)ERR_MCU_CIRCLESCHEDULECONF_TIME_WRONG );
				SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuModifyConfReq]CirclescheduleConference %s timesetting error，errorcode：%d!\n", 
					m_tConf.GetConfName() ,ERR_MCU_CIRCLESCHEDULECONF_TIME_WRONG);
				return;
			}
		
		

		}
		else //普通预约会议
		{
			//if too old, send Nack; Duration time is 0 denoted that meeting could be ended at any time
			if( ( tConf.GetDuration() == 0 && time( NULL ) > tConf.GetStartTime() + 30 * 60 || 
				( tConf.GetDuration() != 0 && 
				time( NULL ) > tConf.GetStartTime() + tConf.GetDuration() * 60 ) ) )
			{
				cServMsg.SetErrorCode( ERR_MCU_STARTTIME_WRONG );
				SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s too late and canceled!\n", m_tConf.GetConfName() );
				return;
			}

		}
        
	
		
		//会议的E164号码已存在，拒绝 
		if( 0 != strcmp( (char*)tConf.GetConfE164(), (char*)m_tConf.GetConfE164() ) &&
			g_cMcuVcApp.IsConfE164Repeat( tConf.GetConfE164(), FALSE,FALSE ) )
		{
			cServMsg.SetErrorCode( ERR_MCU_CONFE164_REPEAT );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s E164 repeated and create failure!\n", m_tConf.GetConfName() );
			return;
		}

		//会议名已存在，拒绝
		if( 0 != strcmp( (char*)tConf.GetConfName(), (char*)m_tConf.GetConfName() ) &&
			g_cMcuVcApp.IsConfNameRepeat( tConf.GetConfName(), FALSE ,FALSE) )
		{
			cServMsg.SetErrorCode( ERR_MCU_CONFNAME_REPEAT );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s name repeated and create failure!\n", m_tConf.GetConfName() );
			return;
		}

		//dynamic vmp and vmp module conflict
		if( tConfAttrib.IsHasVmpModule() && tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_AUTO)
		{
			cServMsg.SetErrorCode( ERR_MCU_DYNAMCIVMPWITHMODULE );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s create failed because has module with dynamic vmp!\n", m_tConf.GetConfName() );
			return;		
		}

	    //无效的会议保护方式,拒绝
		if( tConf.m_tStatus.GetProtectMode() > CONF_LOCKMODE_LOCK )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_CONFLOCKMODE );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s protect mode invalid and nack!\n", m_tConf.GetConfName() );
			return;
		}

		//无效的会议呼叫策略,拒绝
		if( tConf.m_tStatus.GetCallMode() > CONF_CALLMODE_TIMER )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_CALLMODE );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conference %s call mode invalid and nack!\n", m_tConf.GetConfName() );
			return;
		}

        //若开始时间为0,立即开始 
        if( tConf.GetStartTime() == 0 )
        {
            tConf.SetStartTime( time( NULL ) );
            
            u8 byMode = CONF_TAKEMODE_SCHEDULED;
            cServMsg.SetMsgBody(&byMode, sizeof(byMode));
            SendMsgToAllMcs( MCU_MCS_RELEASECONF_NOTIF, cServMsg );
        }        

		post( MAKEIID( GetAppID(), GetInsID() ), MCU_SCHEDULE_CONF_START, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "A scheduled conference %s modified and restart!\n",m_tConf.GetConfName() );

		//release conference
		ReleaseConf( TRUE );

		NEXTSTATE( STATE_IDLE );	
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuDelayConfReq
    功能        ：会议控制台要求延长会议
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/13    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuDelayConfReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u16 wDelayTime;

	switch( CurState() )
	{
	case STATE_ONGOING:
	case STATE_SCHEDULED:
        {
			if (m_tConf.GetDuration() == 0)
			{
				if (MT_MCU_DELAYCONF_REQ == pcMsg->event)
				{
					SendMsgToMt(cServMsg.GetSrcMtId(), MCU_MT_DELAYCONF_NACK, cServMsg);
				}
				else
				{
					SendMsgToMcs(cServMsg.GetSrcSsnId(), MCU_MCS_DELAYCONF_NACK, cServMsg);
				}
				
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Conf %s is manual release. Can not delay!\n", m_tConf.GetConfName());
				return;
			}
			
			if (MT_MCU_DELAYCONF_REQ == pcMsg->event)
			{
				TMt tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
				if (!(tMt == m_tConf.GetChairman()))
				{
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "Conf %s has delayed fail by no chairman!\n", m_tConf.GetConfName());
					SendMsgToMt(cServMsg.GetSrcMtId(), MCU_MT_DELAYCONF_NACK, cServMsg);
					return;
				}
				
				SendMsgToMt( cServMsg.GetSrcMtId() , MCU_MT_DELAYCONF_ACK, cServMsg );
			}
			else
			{
				SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_DELAYCONF_ACK, cServMsg );
			}
			
			wDelayTime = * ( u16* )cServMsg.GetMsgBody();
			
			//[2011/12/15/zhangli]界面限定可以设置1-65535，这里可能超过，如果当前的+设置的>65535，就设置为65535
			u16 wMaxTime = 65535;		//~0
			u32 dwCurrTime = ntohs(wDelayTime) + m_tConf.GetDuration();
			if (dwCurrTime >= wMaxTime)
			{
				dwCurrTime = wMaxTime;
			}
			
			m_tConf.SetDuration(u16(dwCurrTime));        
			
			//Notification
			BroadcastToAllSubMtJoinedConf( MCU_MT_DELAYCONF_NOTIF, cServMsg ); 		
			SendMsgToAllMcs( MCU_MCS_DELAYCONF_NOTIF, cServMsg );
			
			if( pcMsg->event == MT_MCU_DELAYCONF_REQ )
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Conf %s has delayed %d minute by chairman!\n", 
					m_tConf.GetConfName(),  ntohs( wDelayTime ) );
			}
			else
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "Conf %s has delayed %d minute by mcs!\n", 
					m_tConf.GetConfName(),  ntohs( wDelayTime ) );
			}
		}

		
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,  "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuChangeVacHoldTimeReq
    功能        ：会议控制台要改边语音激励持续时间
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/04/13    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuChangeVacHoldTimeReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byHoldTime;

	switch( CurState() )
	{
	case STATE_ONGOING:
	case STATE_SCHEDULED:

        // xsl [8/19/2006] 卫星分散会议不支持此操作
        if (m_tConf.GetConfAttrb().IsSatDCastMode())
        {
	        cServMsg.SetErrorCode( ERR_MCU_DCAST_NOOP );            
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuChangeVacHoldTimeReq] distributed conf not supported vac\n");
            return;
        }

        byHoldTime = * ( u8* )cServMsg.GetMsgBody();
		m_tConf.SetTalkHoldTime( byHoldTime );
		
		SetMixerSensitivity( m_tConf.GetTalkHoldTime() );

        //Ack
		SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_CHANGEVACHOLDTIME_ACK, cServMsg );

		//Notification
		SendMsgToAllMcs( MCU_MCS_CHANGEVACHOLDTIME_NOTIF, cServMsg );
		
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuSpecChairmanReq
    功能        ：指定主席处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/06    1.0         LI Yi         创建
    02/12/18    1.0         LI Yi         改变过程改为平滑过渡
	03/11/24    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcMcsMcuSpecChairmanReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt		tNewChairman;

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		//来自非主席终端,拒绝
		if( cServMsg.GetEventId() == MT_MCU_SPECCHAIRMAN_REQ && 
			cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId() )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_OPER );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );

			//取消其主席
			SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
				
			return;
		}

		//无主席模式,拒绝 
		if( m_tConf.m_tStatus.IsNoChairMode() )
		{
			cServMsg.SetErrorCode( ERR_MCU_NOCHAIRMANMODE );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				
			return;
		}
		
		tNewChairman = *( TMt * )cServMsg.GetMsgBody();
		tNewChairman = m_ptMtTable->GetMt( tNewChairman.GetMtId() );
		
		//与当前主席相同
		if( tNewChairman == m_tConf.GetChairman() )
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Same chairman specified in conference %s! failure!\n", 
				m_tConf.GetConfName() );
			cServMsg.SetErrorCode( ERR_MCU_SAMECHAIRMAN );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	//nack
			return;
		}

		//新主席不与会
		if( !m_tConfAllMtInfo.MtJoinedConf( tNewChairman.GetMtId() ) )
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "New chairman MT%u not in conference %s! Error!\n", 
				    tNewChairman.GetMtId(), m_tConf.GetConfName() );
			cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	//nack
			return;
		}

		//会议正处于“主席轮询选看”模式下，切换主席则停掉轮询
		if ( (CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode())
			 ||(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()) )
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "New chairman MT%u in conference %s  switch ,but now Chairman Polling,so stop polling!\n", 
				    tNewChairman.GetMtId(), m_tConf.GetConfName() );
           ProcStopConfPoll();
		}

		//应答
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );	//ack

		//改变主席
		ChangeChairman( &tNewChairman );

		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuCancelChairmanReq
    功能        ：取消主席处理函数
    算法实现    ：和指定主席操作合并
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/19    1.0         LI Yi         创建
	03/11/25    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcMcsMcuCancelChairmanReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:
		//来自非主席终端,nack
		if( cServMsg.GetEventId() == MT_MCU_CANCELCHAIRMAN_REQ && 
			cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId() )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_OPER );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );

			//取消其主席
			SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
			return;
		}
		
		//无主席 
		if( !m_tConf.HasChairman() )
		{
			if( cServMsg.GetEventId() == MCS_MCU_CANCELCHAIRMAN_REQ )
			{
				cServMsg.SetErrorCode( ERR_MCU_NOCHAIRMAN );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			}
			return;
		}

		// 取消主席时，如果当前正处于“主席轮询”状态，则首先停止主席轮询(消除“主席选看”)
		// 20110414_miaoqs 主席轮询选看功能添加
		if ( (CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()) 
			||(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()))
		{
			// 停主席轮询
			ProcStopConfPoll();
		}
				
        //应答
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

		//取消主席
		ChangeChairman( NULL );

		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuSetConfChairMode
    功能        ：会议控制台设置会议的主席方式
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/12/02    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuSetConfChairMode( const CMessage * pcMsg )
{
    STATECHECK;

	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byChairMode = *( u8* )cServMsg.GetMsgBody();

	m_tConf.m_tStatus.SetNoChairMode( byChairMode == 1 ? FALSE : TRUE );

	if( m_tConf.m_tStatus.IsNoChairMode() && HasJoinedChairman() )
	{
		ChangeChairman( NULL );	
	}
	
	//通知所有会控
	SendMsgToAllMcs( MCU_MCS_CONFCHAIRMODE_NOTIF, cServMsg );
    
    return;
}

/*====================================================================
    函数名      ：ProcMcsMcuSpecSpeakerReq
    功能        ：指定发言终端处理函数
    算法实现    ：指定新发言人直接返回应答，播放录像则等待应答返回
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/06    1.0         LI Yi         创建
    02/12/18    1.0         LI Yi         切换过程改为新老之间平滑过渡
	03/11/25    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcMcsMcuSpecSpeakerReq( const CMessage * pcMsg )
{
	STATECHECK

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
    cServMsg.SetErrorCode(0);
		
	// 处于演讲模式的语音激励控制发言状态,不能指定发言人
    // mqs [2011/03/22] 语音激励按照声音强度指定发言人，与实际的指定发言人操作互斥
	if (m_tConf.m_tStatus.IsVACing())
	{
        if (MT_MCU_SPECSPEAKER_CMD != pcMsg->event)
        {
		    cServMsg.SetErrorCode(ERR_MCU_VACNOSPEAKERSPEC);
		    SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
        }
		return;
	}
		
	//来自非主席终端，nack 
	if( (cServMsg.GetEventId() == MT_MCU_SPECSPEAKER_CMD ||
         cServMsg.GetEventId() == MT_MCU_SPECSPEAKER_REQ) && 
		 cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId() )
	{
		//取消其主席
		SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );			
		return;
	}
	
	//正在放像，不能指定发言人 
	if( m_tConf.m_tStatus.IsPlaying() 
		|| m_tConf.m_tStatus.IsPlayReady())
	{
		cServMsg.SetErrorCode( ERR_MCU_CONFPLAYING );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "It playing now in conference %s! Cannot spcify the speaker!\n", 
			     m_tConf.GetConfName() );
		return;
	}

	
	TMt			tNewSpeaker;
	TMt         tRealMt;
 	tNewSpeaker = *( TMt * )cServMsg.GetMsgBody();
	u8 byIsSecSpeaker = 0;
	if (cServMsg.GetMsgBodyLen() > sizeof(TMt))
	{
		byIsSecSpeaker = *(u8*)(cServMsg.GetMsgBody()+sizeof(TMt));
	}

	tRealMt     = tNewSpeaker;
	if( IsLocalMcuId(tNewSpeaker.GetMcuIdx()))
	{
		tNewSpeaker = m_ptMtTable->GetMt( tNewSpeaker.GetMtId() );
		// 对于主席终控台指定smcu做发言人时,传入type是MT,需将其更新为MCU,与mcs指定mcu做发言人保持一致
		tRealMt = tNewSpeaker;
	}
	else
	{
		tNewSpeaker = m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx( tNewSpeaker.GetMcuIdx() ) );
	}

	//新发言人未与会,nack
	if( tNewSpeaker.GetType() == TYPE_MT && 
		!m_tConfAllMtInfo.MtJoinedConf( tNewSpeaker.GetMcuId(), tNewSpeaker.GetMtId() ) )
	{
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "New speaker MT%u not in conference %s! Error!\n", 
			   tNewSpeaker.GetMtId(), m_tConf.GetConfName() );
		return;
	}

	//新发言人是只接收终端,nack
	//fix 仅判断了直属mcu及终端的状态
	TMtStatus	tMtStatus;
	m_ptMtTable->GetMtStatus(tNewSpeaker.GetMtId(), &tMtStatus);
	if( tNewSpeaker.GetType() == TYPE_MT && 
		( !tMtStatus.IsSendVideo() ) && 
		( !tMtStatus.IsSendAudio() ) )
	{
		cServMsg.SetErrorCode( ERR_MCU_RCVONLYMT );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "New speaker MT%u is receive only MT! Error!\n", 
				         tNewSpeaker.GetMtId() );
		return;
	}

    // xsl [7/20/2006] 卫星分散会议时需要判断回传通道数, 若支持替换需要考虑目的终端是否在vmp、tvwall、会控监控、被选看、mtw、终端录像、双流、发言等情况
    if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsMultiCastMt(tNewSpeaker.GetMtId()) )
    {
		if ( (IsSpeakerCanBrdVid(&tNewSpeaker)
			// 不应该拒掉，在后面的ChgSpeakerInHdu/VMP里处理即可 [pengguofeng 2/22/2013]
// 				|| m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_SPEAKER)
				)
			&& //IsSatMtOverConfDCastNum(tNewSpeaker, emSpeaker, 0xff, 0xff, 0xff,0xff, 0xff, m_tConf.GetSpeaker().GetMtId()))
			!IsSatMtCanContinue(GetLocalMtFromOtherMcuMt(tNewSpeaker).GetMtId(),emSpeaker))
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSpecSpeakerReq]Add Speaker:%d result in Over Sat Chnnl Num!\n",
				tNewSpeaker.GetMtId());

			if(cServMsg.GetEventId() != MT_MCU_SPECSPEAKER_CMD)
			{
				cServMsg.SetErrorCode(ERR_MCU_DCAST_OVERCHNNLNUM);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			}
			return;
		}
    }

	//有终端在发双流，不允许双广播
	if (byIsSecSpeaker > 0 && !m_tDoubleStreamSrc.IsNull())
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSpecSpeakerReq] m_tDoubleStreamSrc is not NULL,nack!\n");
		
		if(cServMsg.GetEventId() != MT_MCU_SPECSPEAKER_CMD)
		{
			cServMsg.SetErrorCode(ERR_MCU_SECSPEAKER_DSNOTNULL);
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		}
		return;
	}

	// mqs [2011/03/22] mcu向MCS回指定发言人ACK消息
	if( cServMsg.GetEventId() != MT_MCU_SPECSPEAKER_CMD )
	{
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	}
	
	
	//新发言人已是发言人
	//fix 判断意义，为什么不返回?条件判断是否存在问题?
	if( tNewSpeaker == m_tConf.GetSpeaker() )	
	{			
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"Same speaker specified in conference %s! Cannot spcify the speaker!\n", 
			m_tConf.GetConfName() );
	}

	//改变发言人 
	if (byIsSecSpeaker)
	{
		ChangeSecSpeaker(tRealMt, TRUE);
	}
	else
	{
		ChangeSpeaker( &tRealMt,FALSE,TRUE,TRUE );
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuCancelSpeakerReq
    功能        ：取消发言人处理函数
    算法实现    ：和指定发言合并处理
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/19    1.0         LI Yi         创建
	03/11/27    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcMcsMcuCancelSpeakerReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byIsSecSpeaker = 0;
	if (cServMsg.GetMsgBodyLen() > 0)
	{
		byIsSecSpeaker = *(u8*)(cServMsg.GetMsgBody());
	}

    TMt tSpeaker;

	switch( CurState() )
	{
	case STATE_ONGOING:

		//处于演讲模式的语音激励控制发言状态,不能指定发言人
		if (m_tConf.m_tStatus.IsVACing())
		{
            if (MT_MCU_CANCELSPEAKER_CMD != pcMsg->event)
            {
			    cServMsg.SetErrorCode(ERR_MCU_VACNOSPEAKERCANCEL);
			    SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            }
			return;
		}
		
		//来自非主席终端，nack
		if( cServMsg.GetEventId() == MT_MCU_SPECSPEAKER_CMD && 
			cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId() )
		{
			//取消其主席
			SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
			return;
		}
		
		//没有发言人
		if(byIsSecSpeaker ==0 && !m_tConf.HasSpeaker()
			|| byIsSecSpeaker > 0 && GetSecVidBrdSrc().IsNull())
		{
			if( cServMsg.GetEventId() == MCS_MCU_CANCELSPEAKER_REQ )
			{
				cServMsg.SetErrorCode( ERR_MCU_NOSPEAKER );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			}
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"No speaker in conference %s now! Cannot cancel the speaker!\n", 
				m_tConf.GetConfName() );
			return;
		}
				
		//应答
		if( cServMsg.GetSrcMtId() == 0 )	//not MT source
		{
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}
		
		//停组播
		tSpeaker = m_tConf.GetSpeaker();
		if( m_ptMtTable->IsMtMulticasting( tSpeaker.GetMtId() ) )
		{
			// 2011-8-11 add by pgf: Bug00061176:会议组播时，取消发言人导致画面合成至组播地址的交换被拆除
			// 遗留问题：如果发言人正好是广播源，且外设都没有启动的情况下，停组播交换在ChangeVidBrdSrc还会再进行一次。
			if ( m_tConf.GetConfAttrb().IsMulticastMode() && tSpeaker == GetVidBrdSrc())
			{
				g_cMpManager.StopMulticast( tSpeaker, 0, MODE_VIDEO );
			}
			if ( m_tConf.GetConfAttrb().IsMulticastMode() && tSpeaker == GetAudBrdSrc())
			{
				g_cMpManager.StopMulticast( tSpeaker, 0, MODE_AUDIO );
			}
			// 2011-8-11 add end
			m_ptMtTable->SetMtMulticasting( tSpeaker.GetMtId(), FALSE );
		}
		
		//取消发言人
		if (byIsSecSpeaker > 0)
		{
			TMt tNullMt;
			ChangeSecSpeaker(tNullMt, FALSE);
		}
		else
		{
			ChangeSpeaker( NULL );
		}
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );

		break;
	}
}


/*====================================================================
    函数名      ：ProcMcsMcuSeeSpeakerCmd
    功能        ：强制广播源
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/27    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcMcsMcuSeeSpeakerCmd( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byForceBrd;   

	switch( CurState() )
	{
	case STATE_ONGOING:
		{
        if (MT_MCU_VIEWBRAODCASTINGSRC_CMD == pcMsg->event)
        {
            if (m_tConf.GetChairman().GetMtId() != cServMsg.GetSrcMtId())
            {
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSeeSpeakerCmd] MtId<%d> isn't chairman, ignore it!\n");
                return;
            }
        }

		byForceBrd = *(u8*)cServMsg.GetMsgBody();
		
        //讨论、定制混音时不允许强制广播
        if(m_tConf.m_tStatus.IsNoMixing())
        {
            m_tConf.m_tStatus.SetMustSeeSpeaker( byForceBrd );
        }
        else
        {
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "No must see speaker while mixing!\n");
            return;
        }
        
        // 顾振华,张宝卿@2006.4.17, 4.20 强制广播策略 
        
        //开启强制广播
        BOOL32 bRestoreAud;
        BOOL32 bRestoreVid;
		BOOL32 bNewVmpBrd = FALSE;	//是否有MPU在广播
		if( m_tConf.m_tStatus.IsMustSeeSpeaker() )
		{
			// xliang [6/8/2009] 目前会议中是否有MPU在广播
			if(g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) && GetLocalVidBrdSrc() == m_tVmpEqp)
			{
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				if(byVmpSubType != VMP)
				{
					bNewVmpBrd = TRUE;
				}
			}

			u8 byVidMtNum = 0;
			u8 byAudMtNum = 0;
			TMt atVidDstMt[MAXNUM_CONF_MT];
			TMt atAudDstMt[MAXNUM_CONF_MT];

            //不论是否有广播源: 清选看, 收看(收听)广播源
            for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
            {
				TMtStatus tMtStatus;
                if ( m_tConfAllMtInfo.MtJoinedConf( byMtId ) ) 
                {
					if (m_ptMtTable->GetMtType(byMtId) == MT_TYPE_VRSREC)
					{
						//过滤vrs新录播
						continue;
					}
					TMt tMt = m_ptMtTable->GetMt(byMtId);
                    bRestoreAud = TRUE;
                    bRestoreVid = TRUE;

                    TMt tMtVideo;
                    TMt tMtAudio;                    

                    m_ptMtTable->GetMtSrc( byMtId, &tMtVideo, MODE_VIDEO );
                    m_ptMtTable->GetMtSrc( byMtId, &tMtAudio, MODE_AUDIO );

                    if (GetLocalSpeaker() == tMt)
                    {
                        // 是发言人，如果是在看主席或者上一发言人的状态，符合模版设置则不清除
            			if( tMtVideo == GetLocalMtFromOtherMcuMt(m_tConf.GetChairman()) && 
				            m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_CHAIR  )
                        {
                            // 不恢复看视频广播，保留选看
                            bRestoreVid = FALSE;
                        }
                        if (tMtVideo == GetLocalMtFromOtherMcuMt(m_tLastSpeaker) &&
                            m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_LAST)
                        {
                            // 不恢复看视频广播，保留选看
                            bRestoreVid = FALSE;
                        }

            			if( tMtAudio == GetLocalMtFromOtherMcuMt(m_tConf.GetChairman()) && 
				            m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_CHAIR &&
                            m_tConf.GetConfAttrb().GetSpeakerSrcMode() == MODE_BOTH)
                        {
                            // 不恢复看音频广播，保留选看
                            bRestoreAud = FALSE;
                        }
            			if( tMtAudio == GetLocalMtFromOtherMcuMt(m_tLastSpeaker) && 
				            m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_LAST &&
                            m_tConf.GetConfAttrb().GetSpeakerSrcMode() == MODE_BOTH)
                        {
                            // 不恢复看音频广播，保留选看
                            bRestoreAud = FALSE;
                        }                        
                    }

					//[2012/1/13 zhangli]强制广播不影响主席的状态
					if (!m_tConf.GetChairman().IsNull() && m_tConf.GetChairman() == tMt)
					{
						bRestoreAud = FALSE;
						bRestoreVid = FALSE;
					}

                    //有音频源
                    if ( !tMtAudio.IsNull() && bRestoreAud)
                    {
                        //指定恢复接收音频广播媒体源
                        //RestoreRcvMediaBrdSrc( byMtId, MODE_AUDIO );   
						atAudDstMt[byAudMtNum] = m_ptMtTable->GetMt( byMtId );
						byAudMtNum++;      
                    }
                    
                    //有视频源
                    if ( !tMtVideo.IsNull() && bRestoreVid) 
                    {
                        u16 wSrcSndBitrate  = m_ptMtTable->GetMtSndBitrate(tMtVideo.GetMtId());
                        u16 wSrcDialBitrate = m_ptMtTable->GetSndBandWidth(tMtVideo.GetMtId());
                        if (wSrcSndBitrate < wSrcDialBitrate && !(tMtVideo == m_tDoubleStreamSrc))
                        {
/*
                            CServMsg cTmpServMsg;
                            TLogicalChannel tLogicalChannel;
                            if (TRUE == m_ptMtTable->GetMtLogicChnnl(tMtVideo.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
                            {
                                // 修改发送终端的发送码率
                                tLogicalChannel.SetFlowControl(wSrcDialBitrate);
                                m_ptMtTable->SetMtLogicChnnl(tMtVideo.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);
                                cTmpServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
                                SendMsgToMt(tMtVideo.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cTmpServMsg);
                            }*/

                            
                            // 修改本终端的接收码率
//                            u16 wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(byMtId);
//                            m_ptMtTable->SetMtReqBitrate(byMtId, wMtDialBitrate);
                        }
                        if( bNewVmpBrd )
						{
							// xliang [6/8/2009] Make Mts watching other Mts before watch VMP now ; 
							// Mts already watching VMP maintain their status.
							m_ptMtTable->GetMtStatus( byMtId, &tMtStatus );
							if (! tMtStatus.GetSelectMt( MODE_VIDEO ).IsNull())
							{
								//SwitchNewVmpToSingleMt(tMt);	//画面合成广播单独建交换								
								StopSelectSrc( tMt,MODE_VIDEO );
								bRestoreVid = FALSE;
							}
						}
						else
						{
							//指定恢复接收视频广播媒体源
							//RestoreRcvMediaBrdSrc( byMtId, MODE_VIDEO );
							atVidDstMt[byVidMtNum] = tMt;
							byVidMtNum++;
						}
					}

                    m_ptMtTable->GetMtStatus( byMtId, &tMtStatus );  
					
					//[201112/02/zhangli]如果有选看释放适配器、释放带宽
					TMt tSelMt = tMtStatus.GetSelectMt(MODE_VIDEO);
					if (bRestoreVid && !tSelMt.IsNull())
					{
						/*if (IsNeedSelAdpt(tSelMt, tMt, MODE_VIDEO))
						{
							StopSelAdapt(tSelMt, tMt, MODE_VIDEO);
						}

						FreeRecvSpy(tSelMt, MODE_VIDEO);
						tSelMt.SetNull();
						tMtStatus.SetSelectMt(tSelMt, MODE_VIDEO);*/
						StopSelectSrc( tMt,MODE_VIDEO,FALSE,FALSE );
						
					}
					
					tSelMt = tMtStatus.GetSelectMt(MODE_AUDIO);
					if (bRestoreAud && !tSelMt.IsNull())
					{
						/*if (IsNeedSelAdpt(tSelMt, tMt, MODE_AUDIO))
						{
							StopSelAdapt(tSelMt, tMt, MODE_AUDIO);
						}
						FreeRecvSpy(tSelMt, MODE_AUDIO);
						tSelMt.SetNull();
						tMtStatus.SetSelectMt(tSelMt, MODE_AUDIO);*/
						StopSelectSrc( tMt,MODE_AUDIO,FALSE,FALSE );
					}

                    //m_ptMtTable->SetMtStatus( byMtId, &tMtStatus );              
                }
            }

			if( byVidMtNum != 0 )
			{
				RestoreRcvMediaBrdSrc( byVidMtNum, atVidDstMt, MODE_VIDEO );
			}
			if( byAudMtNum != 0 )
			{
				RestoreRcvMediaBrdSrc( byAudMtNum, atAudDstMt, MODE_AUDIO );
			}
		}        
		else
		{
            //停止强制广播，不做任何处理
		}
        
		ConfModeChange();

		break;
		}
	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ProcMcsMcuAddMtExReq
    功能        ：调试版会议控制台增加终端处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
	              消息体为TMcu+TAddMtInfoEx(增加终端的数组)
    返回值说明  ：
 ====================================================================*/
void CMcuVcInst::ProcMcsMcuAddMtExReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	TMcu tMcu;
    tMcu.SetNull();
	TAddMtInfoEx  *ptAddMtInfo = NULL;
	u8            byMtNum = 0;

    if (cServMsg.GetMsgBodyLen() >= sizeof(TMcu))
    {
		tMcu = *(TMcu*)(cServMsg.GetMsgBody());
		byMtNum= ( cServMsg.GetMsgBodyLen() - sizeof(TMcu) ) / sizeof( TAddMtInfoEx );
		ptAddMtInfo = (TAddMtInfoEx *)(cServMsg.GetMsgBody() + sizeof(TMcu));
    }
	else
	{
		return;
	}

	STATECHECK

	//不能呼叫会议本身
	if ( ptAddMtInfo && mtAliasTypeE164 == ptAddMtInfo->m_AliasType)
	{
		if (ptAddMtInfo && 0 == strcmp(ptAddMtInfo->m_achAlias, m_tConf.GetConfE164()))
		{
			cServMsg.SetErrorCode(ERR_MCU_NOTCALL_CONFITSELF);
			SendReplyBack(cServMsg, MCU_MCS_ADDMT_NACK);
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuAddMtExReq]CMcuVcInst: Cannot Call conf itself.\n");
			return;
		}
	}

	// 级联
	if (!tMcu.IsLocal())
	{
		//u16 wMcuIdx = tMcu.GetMcuId();
		u8 byMtId = GetFstMcuIdFromMcuIdx(tMcu.GetMcuId());
		if (!m_tConfAllMtInfo.m_tLocalMtInfo.MtJoinedConf(byMtId))
		{
			cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
			SendReplyBack( cServMsg, MCU_MCS_ADDMT_NACK );
			return;
		}

		CServMsg cMsg;
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
				 sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
		astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
				 sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
		cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		cMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cMsg.CatMsgBody(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
		SendMsgToMt( byMtId, MCU_MCU_INVITEMT_REQ, cMsg );

		SendReplyBack( cServMsg, MCU_MCS_ADDMT_ACK );
		return;
	}

	//主动邀请, ack
	SendReplyBack( cServMsg, MCU_MCS_ADDMT_ACK );

	//增加受邀终端列表
	for (u8 byLoop = 0; byLoop < byMtNum; byLoop++)
	{
		u8  byMtId;
		TMt tMt;
		byMtId = AddMt( ptAddMtInfo[byLoop], ptAddMtInfo[byLoop].GetCallBitRate(), 
						(ptAddMtInfo[byLoop].m_byCallMode == CONF_CALLMODE_TIMER || ptAddMtInfo[byLoop].m_byCallMode == CONF_CALLMODE_NONE) ? ptAddMtInfo[byLoop].m_byCallMode : m_tConf.m_tStatus.GetCallMode() );
    
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuAddMtExReq]Mcs add mt-%d DialBitRate-%d Alias-", 
				byMtId, ptAddMtInfo[byLoop].GetCallBitRate());
		if ( ptAddMtInfo[byLoop].m_AliasType == mtAliasTypeTransportAddress )
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT,  "%s:%d!\n",				
					  StrOfIP(ptAddMtInfo[byLoop].m_tTransportAddr.GetIpAddr()), 
					  ptAddMtInfo[byLoop].m_tTransportAddr.GetPort() );
		}
		else
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT, "%s!\n",	ptAddMtInfo[byLoop].m_achAlias);
		}

		// 会议终端数已满或者已添加该终端
		if ( 0 == byMtId )
		{
// 			cServMsg.SetErrorCode(ERR_MCU_NOTCALL_CONFITSELF);
// 			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
// 			ConfLog(FALSE, "CMcuVcInst: alloc MTID Fail.\n");
			return;
		}
		else
		{
			// 终端能力级由会控干预指定，保存指定的能力级用于打开通道时使用
			m_ptMtTable->SetMtCapSpecByMCS(byMtId, TRUE);
			m_ptMtTable->m_tMTInfoEx[byMtId].clear();
			m_ptMtTable->m_tMTInfoEx[byMtId].SetMainVideoCap(const_cast<TVideoStreamCap&>(ptAddMtInfo[byLoop].GetMtCapInfo().GetMainVideoCap()));
			m_ptMtTable->m_tMTInfoEx[byMtId].SetDStreamVideoCap(const_cast<TVideoStreamCap&>(ptAddMtInfo[byLoop].GetMtCapInfo().GetDStreamVideoCap()));
			TAddMtCapInfo& tCapInfo = const_cast<TAddMtCapInfo&>(ptAddMtInfo[byLoop].GetMtCapInfo());
			ConfPrint( LOG_LVL_DETAIL, MID_MCU_MT, "[ProcMcsMcuAddMtExReq]MainMaxBitRate-%d MainMediaType-%d MainResolution-%d MainFrameRate-%d\n",
				                    tCapInfo.GetMainMaxBitRate(), tCapInfo.GetMainMediaType(),
							        tCapInfo.GetMainResolution(), 
									tCapInfo.IsMainFrameRateUserDefined()?tCapInfo.GetUserDefMainFrameRate():tCapInfo.GetMainFrameRate());
			
			ConfPrint( LOG_LVL_DETAIL, MID_MCU_MT,"[ProcMcsMcuAddMtExReq]DStreamMaxBitRate-%d DStreamMediaType-%d DStreamResolution-%d DStreamFrameRate-%d\n",
				                    tCapInfo.GetDstreamMaxBitRate(), tCapInfo.GetDstreamMediaType(),
							        tCapInfo.GetDstreamResolution(), 
									tCapInfo.IsDstreamFrameRateUserDefined()?tCapInfo.GetUserDefDstreamFrameRate():tCapInfo.GetDstreamFrameRate());

		}


		if (byMtId > 0 && !m_tConfAllMtInfo.MtJoinedConf(byMtId))
		{
			if (CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode( byMtId ) && 
				DEFAULT_CONF_CALLTIMES != m_tConf.m_tStatus.GetCallTimes())
			{
				m_ptMtTable->SetCallLeftTimes( byMtId, (m_tConf.m_tStatus.GetCallTimes()-1) );
			}
			tMt = m_ptMtTable->GetMt( byMtId );

			//不管终端呼叫模式怎样均保证呼叫一次，此处使用会议能力
			InviteUnjoinedMt( cServMsg, &tMt, TRUE, TRUE );

			m_ptMtTable->SetAddMtMode(byMtId, ADDMTMODE_MCS);				

		}
	}



	//发给会控会议所有终端信息
	//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
	//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//发给会控终端表
	SendMtListToMcs(LOCAL_MCUIDX);

	return;
}

/*====================================================================
    函数名      ：ProcMcsMcuAddMtReq
    功能        ：会议控制台增加终端处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/05    1.0         LI Yi         创建
	03/11/27    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcMcsMcuAddMtReq( const CMessage * pcMsg )
{
	STATECHECK
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	u8   byMtNum = 0;
	TMcu tMcu;
    tMcu.SetNull();
	TAddMtInfo  *ptAddMtInfo = NULL;
	BOOL32 bLowLevelMcuCalledIn = FALSE; 
	TMsgHeadMsg tHeadMsg,tHeadMsgAck;
	BOOL32 bIsLocal = TRUE;
	if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
	{
        if (cServMsg.GetMsgBodyLen() >= sizeof(TMcuMcuReq)+sizeof(TMt))
        {
		    byMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TMcuMcuReq)-sizeof(TMt) )/sizeof( TAddMtInfo);
		    ptAddMtInfo = (TAddMtInfo *)(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq)+sizeof(TMcu));
		    tMcu = *(TMcu*)(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq)); 

			if( cServMsg.GetMsgBodyLen() > (byMtNum * sizeof(TAddMtInfo) + sizeof(TMcuMcuReq) + sizeof( TMcu ) ) )
			{
				tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq)+sizeof(TMcu)+byMtNum*sizeof(TAddMtInfo));			
				tHeadMsgAck.m_tMsgSrc = tHeadMsg.m_tMsgDst;
				tHeadMsgAck.m_tMsgDst = tHeadMsg.m_tMsgSrc;	
			}					
			bIsLocal = tMcu.IsMcuIdLocal();
			
        }
	}
	else
	{
        if (cServMsg.GetMsgBodyLen() >= sizeof(TMcu))
        {
		    tMcu = *(TMcu*)(cServMsg.GetMsgBody());
			tMcu.SetType( TYPE_MCU );
		    byMtNum= ( cServMsg.GetMsgBodyLen() - sizeof(TMt) )/sizeof( TAddMtInfo );
		    ptAddMtInfo = (TAddMtInfo *)(cServMsg.GetMsgBody()+sizeof(TMcu));
			// xliang [8/29/2008] 增加高级别mcu反呼的标志，
			//随后会有相应处理，SetNotInvited有别于正常逻辑,以应对级联会议中的加密会议
			if(cServMsg.GetMsgBodyLen() > sizeof(TMcu) + byMtNum * sizeof(TAddMtInfo))
			{		
				bLowLevelMcuCalledIn = *(BOOL32 *)(cServMsg.GetMsgBody() + sizeof(tMcu) + byMtNum * sizeof(TAddMtInfo));
			}			
			if( MT_MCU_ADDMT_REQ == cServMsg.GetEventId() || bLowLevelMcuCalledIn)// [9/27/2010 xliang] 反呼的肯定是作为本级添加的
			{
				bIsLocal = TRUE;
			}
			else
			{
				bIsLocal = tMcu.IsLocal();
			}			
        }
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuAddMtReq] Add Mt From Mcu(%d.%d.%d) level.%d \n",tMcu.GetMcuId(),tMcu.GetMtId(),
				tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],tHeadMsg.m_tMsgDst.m_byCasLevel
				);

	u8  byLoop, byMtId;
	TMt tMt;

	

	//来自非主席终端, nack
	if (MT_MCU_ADDMT_REQ == cServMsg.GetEventId() && 
		cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId())
	{
		cServMsg.SetErrorCode( ERR_MCU_INVALID_OPER );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );

		//取消主席
		SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );

		return;
	}

	//不能呼叫会议本身
	if ( ptAddMtInfo && mtAliasTypeE164 == ptAddMtInfo->m_AliasType)
	{
		if ( ptAddMtInfo && 0 == strcmp(ptAddMtInfo->m_achAlias, m_tConf.GetConfE164()))
		{
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
			cServMsg.SetErrorCode(ERR_MCU_NOTCALL_CONFITSELF);
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Cannot Call conf itself.\n");
			return;
		}
	}

	//是否在本级上增加
	if (!bIsLocal)
	{
		u16 wMcuIdx = INVALID_MCUIDX;
		if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
		{
			byMtId = (u8)tMcu.GetMcuId();
			wMcuIdx = GetMcuIdxFromMcuId( byMtId );
			if( INVALID_MCUIDX == wMcuIdx )
			{
				cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Cannot Find McuId Info.McuId.%d\n",byMtId);
				return;
			}			
		}
		else
		{
			wMcuIdx = tMcu.GetMcuIdx();
		}

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuAddMtReq] Mcu is not local.wMcuIdx.%d\n",wMcuIdx );

		//u8 byFstMcuId = 0,bySecMcuId = 0;
		u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
		memset( &abyMcuId[0],0,sizeof(abyMcuId) );
		if( m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] ) )
		{
			if( 0 != abyMcuId[1] &&
				m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],1,&wMcuIdx ) 
				)
			{
				if( !m_tConfAllMtInfo.GetMtInfo( wMcuIdx ).MtJoinedConf(abyMcuId[1]) )
				{
					if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
					{
						cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
					}					
					cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					return;
				}				
			}
			
			if( 0 == abyMcuId[1] &&
				!m_tConfAllMtInfo.m_tLocalMtInfo.MtJoinedConf(abyMcuId[0])
				)
			{
				if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
				{
					cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
				}
				cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
		}

		CServMsg cMsg;
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
				 sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
		astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
				 sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
		cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		if(cServMsg.GetEventId() != MCU_MCU_INVITEMT_REQ)
		{		
			memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
			
			tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMcu,tMcu );
			/*tMcu = tHeadMsg.m_tMsgDst.m_tMt;
			memset( &abyMcuId[0],0,sizeof(abyMcuId) );			
			if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
			{			
				m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] );
				if( 0 != abyMcuId[1] )
				{
					tHeadMsg.m_tMsgDst.m_byCasLevel = 1;				
					tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( abyMcuId[0] );
					tHeadMsg.m_tMsgDst.m_tMt.SetMtId( abyMcuId[1] );
				}
				else
				{
					tHeadMsg.m_tMsgDst.m_byCasLevel = 0;
					tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( abyMcuId[0] );
				}				
			}*/								
		}
		/*else
		{
			tHeadMsg.m_tMsgDst.m_tMt = tMcu;
		}*/
		
		
		cMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cMsg.CatMsgBody((u8 *)&tMcu, sizeof(TMcu));		
		if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
		{			
			cMsg.CatMsgBody( (u8*)ptAddMtInfo, byMtNum * sizeof(TAddMtInfo) );
		}
		else
		{
			cMsg.CatMsgBody(cServMsg.GetMsgBody()+sizeof(TMcu), cServMsg.GetMsgBodyLen()-sizeof(TMcu));
		}
		cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		
		SendMsgToMt( (u8)tMcu.GetMcuId(), MCU_MCU_INVITEMT_REQ, cMsg );

		if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
		{
			//tMt = m_ptMtTable->GetMt( tMt.GetMcuId() );
			//tHeadMsgAck.m_tMsgSrc.m_tMt = tMcu;
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
			cServMsg.CatMsgBody( (u8*)&tMcu,sizeof(TMcu) );
		}
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
	}

	//主动邀请, ack
	if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
	{
		//tHeadMsgAck.m_tMsgSrc.m_tMt = tMcu;
		cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
		cServMsg.CatMsgBody( (u8*)&tMcu,sizeof(TMcu) );
	}
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	//增加受邀终端列表
	for (byLoop = 0; byLoop < byMtNum; byLoop++)
	{
		if (!ptAddMtInfo)
		{
			break;
		}
		
		const u8 byCallMode = ( (ptAddMtInfo[byLoop].m_byCallMode == CONF_CALLMODE_TIMER || ptAddMtInfo[byLoop].m_byCallMode == CONF_CALLMODE_NONE)) 
			? (ptAddMtInfo[byLoop].m_byCallMode) : (m_tConf.m_tStatus.GetCallMode()) ;

		byMtId = AddMt( ptAddMtInfo[byLoop], ptAddMtInfo[byLoop].GetCallBitRate(), byCallMode);
        
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Mcs add mt-%d DialBitRate-%d CallMode-%d aliasType-%d\n", 
				byMtId, ptAddMtInfo[byLoop].GetCallBitRate(), 
				ptAddMtInfo[byLoop].GetCallMode(),
				ptAddMtInfo[byLoop].m_AliasType );
        if ( ptAddMtInfo[byLoop].m_AliasType == mtAliasTypeTransportAddress )
        {
            ConfPrint( LOG_LVL_DETAIL, MID_MCU_MT, "%s:%d!\n",				
                      StrOfIP(ptAddMtInfo[byLoop].m_tTransportAddr.GetIpAddr()), 
                      ptAddMtInfo[byLoop].m_tTransportAddr.GetPort() );
        }
        else
        {
            ConfPrint( LOG_LVL_DETAIL, MID_MCU_MT, "%s!\n",	ptAddMtInfo[byLoop].m_achAlias);
        }

		// 终端能力级不由会控干预指定
		m_ptMtTable->SetMtCapSpecByMCS(byMtId, FALSE);

		if (byMtId > 0 && !m_tConfAllMtInfo.MtJoinedConf(byMtId))
		{
			if (CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode( byMtId ) && 
				DEFAULT_CONF_CALLTIMES != m_tConf.m_tStatus.GetCallTimes())
			{
				m_ptMtTable->SetCallLeftTimes( byMtId, (m_tConf.m_tStatus.GetCallTimes()-1) );
			}
			tMt = m_ptMtTable->GetMt( byMtId );

			//不管终端呼叫模式怎样均保证呼叫一次
			InviteUnjoinedMt( cServMsg, &tMt, TRUE, TRUE, VCS_FORCECALL_REQ, bLowLevelMcuCalledIn );

			// libo [3/29/2005]
			if (MT_MCU_ADDMT_REQ == cServMsg.GetEventId())
			{
				m_ptMtTable->SetAddMtMode(byMtId, ADDMTMODE_CHAIRMAN);
			}
			else
			{
				m_ptMtTable->SetAddMtMode(byMtId, ADDMTMODE_MCS);
			}
			// libo [3/29/2005]end
		}

		// libo [3/30/2005]
		if (0 == byMtId || 
			(byMtId > 0 && MT_MCU_ADDMT_REQ == cServMsg.GetEventId() && 
			 m_tConfAllMtInfo.MtJoinedConf(byMtId)))
		{
			cServMsg.SetEventId(MCU_MT_ADDMT_NACK);
			cServMsg.SetMsgBody((u8 *)&ptAddMtInfo[byLoop], sizeof(TMtAlias));
			SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_ADDMT_NACK, cServMsg);
		}
		// libo [3/30/2005]end
	}

	//发给会控会议所有终端信息
	//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
	//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//发给会控终端表
	SendMtListToMcs(LOCAL_MCUIDX);

	return;
}

/*====================================================================
    函数名      ：ProcMcsMcuDelMtReq
    功能        ：会议控制台删除终端处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/05    1.0         LI Yi         创建
	03/11/25    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcMcsMcuDelMtReq( const CMessage * pcMsg )
{
	STATECHECK
		
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt tMt;
	TMsgHeadMsg tHeadMsg,tHeadMsgAck;
	BOOL32 bIsLocal = TRUE;
	u8 byMtId = 0;
	u16 wMcuIdx = INVALID_MCUIDX;
	if(cServMsg.GetEventId() == MCU_MCU_DELMT_REQ)
	{
		tMt = *( TMt * )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));
		if( cServMsg.GetMsgBodyLen() > ( sizeof(TMcuMcuReq) + sizeof( TMt ) ) )
		{
			tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody()+sizeof(TMcuMcuReq) + sizeof(TMt) );
			tHeadMsgAck.m_tMsgSrc = tHeadMsg.m_tMsgDst;
			tHeadMsgAck.m_tMsgDst = tHeadMsg.m_tMsgSrc;
		}	
		
		bIsLocal = tMt.IsMcuIdLocal();
		wMcuIdx = GetMcuIdxFromMcuId( (u8)tMt.GetMcuId() );		
		byMtId = tMt.GetMtId();
	}
	else
	{
		tMt = *( TMt * )cServMsg.GetMsgBody();
// 		//终端来的消息转换一下
// 		if( MT_MCU_DELMT_REQ == cServMsg.GetEventId() )
// 		{
// 			tMt = m_ptMtTable->GetMt( tMt.GetMtId() );
// 		}
		bIsLocal = tMt.IsLocal();
		wMcuIdx = tMt.GetMcuIdx();
		byMtId = tMt.GetMtId();
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuDelMtReq] Del Mt(%d.%d.%d) level.%d\n",wMcuIdx,byMtId,
				tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],tHeadMsg.m_tMsgDst.m_byCasLevel 
				);

	//来自非主席终端，nack
	if( cServMsg.GetEventId() == MT_MCU_DELMT_REQ && 
		cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId() )
	{
		cServMsg.SetErrorCode( ERR_MCU_INVALID_OPER );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		
		//取消主席
		SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
		
		return;
	}

	TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tMt.GetMcuId());
	//会议表中没有
	if(tMtInfo.IsNull())
	{
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//已经不在会议中,NACK
	if( !tMtInfo.MtInConf( tMt.GetMtId() ) )
	{	
		if(cServMsg.GetEventId() == MCU_MCU_DELMT_REQ)
		{
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
		}
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//不是本级MCU下的终端
    if( !bIsLocal )//!tMt.IsLocal() )
	{
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
			sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
		astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
			sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
		
		if(cServMsg.GetEventId() != MCU_MCU_DELMT_REQ)
		{
			memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
			tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo(tMt,tMt);
			/*tMt = tHeadMsg.m_tMsgDst.m_tMt;
			if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
			{
				u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
				memset( &abyMcuId[0],0,sizeof(abyMcuId) );
				//u8 byFstMcuId = 0,bySecMcuId = 0;
				m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] );
				if( 0 != abyMcuId[1] )
				{
					tHeadMsg.m_tMsgDst.m_byCasLevel = 1;
					tHeadMsg.m_tMsgDst.m_abyMtIdentify[0] = tMt.GetMtId();
					tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( abyMcuId[0] );
					tHeadMsg.m_tMsgDst.m_tMt.SetMtId( abyMcuId[1] );
				}
				else
				{
					tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( abyMcuId[0] );
					tHeadMsg.m_tMsgDst.m_tMt.SetMtId( tMt.GetMtId() );
					tHeadMsg.m_tMsgDst.m_byCasLevel = 0;
				}
			}*/									
		}
		/*else
		{
			tHeadMsg.m_tMsgDst.m_tMt = tMt;
		}*/

		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		cServMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		
		SendMsgToMt( (u8)tMt.GetMcuId(), MCU_MCU_DELMT_REQ, cServMsg );

		if(cServMsg.GetEventId() == MCU_MCU_DELMT_REQ)
		{
			tMt = m_ptMtTable->GetMt( (u8)tMt.GetMcuId() );
			//tHeadMsgAck.m_tMsgSrc.m_tMt = tMt;
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
			cServMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
		}
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
	}
	if(cServMsg.GetEventId() == MCU_MCU_DELMT_REQ)
	{		
		//tHeadMsgAck.m_tMsgSrc.m_tMt = tMt;
		cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof(TMsgHeadMsg) );
		cServMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
	}
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	tMt = m_ptMtTable->GetMt( tMt.GetMtId() );

	
	MtOnlineChange( tMt, FALSE, MTLEFT_REASON_NORMAL );

	// xliang [12/24/2008] 在VMP批量轮询时
	if (m_tConf.m_tStatus.m_tVMPParam.IsVMPBatchPoll())
	{
		//要删的MT出现在当前版面中，则清该MT对应的VMP通道
		u8 byMemberId;
		if( m_tVmpPollParam.IsMtInCurPollBlok(tMt.GetMtId(),&byMemberId) )
		{
			StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byMemberId, FALSE, MODE_VIDEO);
		}
	}
	// vrs新录播支持
	if (tMt.GetMtType() == MT_TYPE_VRSREC)
	{
		ReleaseVrsMt(tMt.GetMtId());
	}
	else
	{
		RemoveMt( tMt, TRUE );
	}
}

//消息体：2 byte(终端数,网络序,值为N,0为广播到所有终端) + N个TMt +TROLLMSG
//OSPEVENT( MCS_MCU_SENDRUNMSG_CMD,    EV_MCSVC_BGN + 181 );
/*====================================================================
    函数名      ：  
    功能        ：短消息发送处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/15    1.0         LI Yi         创建
	03/11/25    3.0         胡昌威        修改
	10/08/12	4.6			xl			  modify
====================================================================*/
void CMcuVcInst::ProcMcsMcuSendMsgReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
//	CServMsg    cModifyedMsg( pcMsg->content, pcMsg->length );

	u16 wMtNum = *(u16*)cServMsg.GetMsgBody();
//	TMt	*ptMt = (TMt *)( cServMsg.GetMsgBody() + sizeof(u16) );
	CRollMsg* ptROLLMSG = (CRollMsg*)( cServMsg.GetMsgBody() + ntohs(wMtNum)*sizeof(TMt) + sizeof(u16) );

    // xsl [11/3/2006] 由配置文件决定是否将终端的短消息转发给会议中的终端
    if (MT_MCU_SENDMSG_CMD == pcMsg->event)
    {
        if (!g_cMcuVcApp.IsTransmitMtShortMsg())
        {
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSendMsgReq] not permit transmit mt short message in mcu config\n");
            return;
        }
		//终端发的消息体中已经带了源的别名，不需要源mt了，否则会多显示一个
		if ( NULL != ptROLLMSG )
		{
			TMTLABEL tSrcMt;
			ptROLLMSG->SetMsgSrcMtId(tSrcMt);
		}
	

// 		// [8/26/2010 xliang] reconstruct msgbody:
// 		TMt	tTmpMt;
// 		cModifyedMsg.SetMsgBody();
// 		cModifyedMsg.SetMsgBody(( u8 *)&wMtNum, sizeof(wMtNum));
// 		for(u16 wNumIdx = 0; wNumIdx < wMtNum; wNumIdx ++)
// 		{
// 			tTmpMt = *(TMt *)( cServMsg.GetMsgBody() + sizeof(u16) + sizeof(TMt) * wNumIdx );
// 			u16 wMcuIdx = GetMcuIdxFromMcuId( (u8)tTmpMt.GetMcuId() );	
// 			u8 byMtId = tTmpMt.GetMtId();
// 			tTmpMt.SetMcuIdx( wMcuIdx );
// 			tTmpMt.SetMtId(byMtId);
// 			cModifyedMsg.CatMsgBody(( u8 *)&tTmpMt, sizeof(tTmpMt));
// 		}
// 		cModifyedMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );
    }

	switch( CurState() )
	{
	case STATE_ONGOING:

		if( wMtNum == 0 )	//发给所有终端 MCU_MT_SENDMSG_NOTIF（包括下级MCU，在对端收到时处理）
		{
			CServMsg	cSendMsg;
			TMt tMt;
			tMt.SetNull();
			cSendMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
			cSendMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );

			for(u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
			{
				if( m_tConfAllMtInfo.MtJoinedConf(byLoop) && byLoop != cServMsg.GetSrcMtId() )
                {
                    if (m_ptMtTable->GetMtType(byLoop) == MT_TYPE_MT)
                    {
						cServMsg.SetMsgBody( cServMsg.GetMsgBody() , (sizeof(TMt) + ptROLLMSG->GetTotalMsgLen()));
                        SendMsgToMt( byLoop, MCU_MT_SENDMSG_NOTIF, cSendMsg );	
                    }
                    else if(m_ptMtTable->GetMtType(byLoop) == MT_TYPE_SMCU)
                    {
                        SendMsgToMt( byLoop, MCU_MCU_SENDMSG_NOTIF, cSendMsg );
                    }
                }			
			}

			//BroadcastToAllSubMtJoinedConf( MCU_MT_SENDMSG_NOTIF, cSendMsg );
		}
		else //发给某些终端 
		{
			if( !m_cSmsControl.IsStateIdle() )
			{
				//notify Error to UI
				NotifyMcsAlarmInfo(0, ERR_MCU_SMSBUSY);
				return;
			}
			
			// [1/7/2011 xliang] send sms through several times
			m_cSmsControl.SetState(CSmsControl::BUSY);
			m_cSmsControl.SetServMsg(cServMsg);

			ProcBatchMtSmsOpr(cServMsg);
// 			TMt	*ptMt = (TMt *)( cModifyedMsg.GetMsgBody() + sizeof(u16) );
// 			for( u16 wLoop = 0; wLoop < ntohs(wMtNum); wLoop++ )
// 			{
// 				ProcSingleMtSmsOpr(cServMsg, ptMt, ptROLLMSG);
// 				ptMt++;
// 			}
		}
		break;
		
	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuGetMtListReq
    功能        ：得到终端列表处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/27    3.0         胡昌威        创建
	10/12/31    4.6          liuxu        修改
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetMtListReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	//得到MCU号
	TMcu tMcu = *(TMcu*)cServMsg.GetMsgBody(); 

	u16 wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
	if( !m_tCascadeMMCU.IsNull() &&
		INVALID_MCUIDX == wMMcuIdx )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcsMcuGetMtListReq] Fail to Get m_tCascadeMMCU McuIdx\n" );
	}
	//所有终端列表
	if( INVALID_MCUIDX == tMcu.GetMcuIdx() )
	{	
		tMcu.SetNull();

		//先发其它MC
		u16 wMcuIdx = INVALID_MCUIDX;						// 待遍历m_tConfAllMtInfo得到的wMcuIdx
		TConfMtInfo *ptConfMtInfo = NULL;					// 待遍历使用的TConfMtInfo
		TConfMcInfo *ptConfMcInfo = NULL;					// 待遍历使用的TConfMcInfo
		for( u16 wLoop = 0; wLoop < m_tConfAllMtInfo.GetMaxMcuNum(); wLoop++ )
		{
			// 为空, 则Continue
			if( m_tConfAllMtInfo.GetMtInfo(wLoop).IsNull() )
			{
				continue;
			}

			// liuxu, 改直接访问为指针访问, 封装细节
			ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr(wLoop);
			if (NULL == ptConfMtInfo)
			{
				continue;
			}

			wMcuIdx = ptConfMtInfo->GetMcuIdx();
            // guzh [4/30/2007] 上级MCU列表过滤
			if( !IsLocalMcuId(wMcuIdx)
				&& ( wMcuIdx != wMMcuIdx || g_cMcuVcApp.IsShowMMcuMtList() )
               )      
			{
				SendMtListToMcs(wMcuIdx); // 非本级，则向mtadp发请求，不用上报MMCU [pengguofeng 7/2/2013]
/*				ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );				
				if (NULL == ptConfMcInfo)
				{
					continue;
				}

				tMcu.SetMcuIdx( ptConfMtInfo->GetMcuIdx() );
				cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
				
				for(s32 nLoop=0; nLoop<MAXNUM_CONF_MT; nLoop++)
				{
					if( ptConfMcInfo->m_atMtStatus[nLoop].IsNull() || 
						ptConfMcInfo->m_atMtStatus[nLoop].GetMtId() == 0 ) //自己
					{
						continue;
					}

					cServMsg.CatMsgBody((u8 *)&(ptConfMcInfo->m_atMtExt[nLoop]), sizeof(TMtExt));
				}
				
				SendReplyBack( cServMsg, MCU_MCS_MTLIST_NOTIF );
*/
			}
		}

		// [pengjie 2010/5/17] 通知界面下级各会议扩展信息（是否支持多回传）
		SendConfExtInfoToMcs( cServMsg.GetSrcSsnId() );
		// End

	    //本级的最后发 
		tMcu.SetNull();
		tMcu.SetMcu( LOCAL_MCUID );
		tMcu.SetMcuIdx( LOCAL_MCUIDX );		
	}		

	//不是本级的MCU
	if(!tMcu.IsLocal() )
	{
        // guzh [4/30/2007] 上级MCU列表过滤
        if ( !g_cMcuVcApp.IsShowMMcuMtList() && 
             !m_tCascadeMMCU.IsNull() && wMMcuIdx == tMcu.GetMcuIdx() )
        {
        }
        else
        {
			SendMtListToMcs(tMcu.GetMcuIdx()); // 向下级发请求,不用上报MMcu [pengguofeng 7/2/2013]
/*            TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( tMcu.GetMcuId() );
            cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
            for(s32 nLoop=0; nLoop<MAXNUM_CONF_MT; nLoop++)
            {
                if( NULL == ptConfMcInfo ||
					ptConfMcInfo->m_atMtStatus[nLoop].IsNull() || 
                    ptConfMcInfo->m_atMtStatus[nLoop].GetMtId() == 0 ) //自己
                {
                    continue;
                }
                //TMcMtStatus:public TMtStatus
                cServMsg.CatMsgBody((u8 *)&(ptConfMcInfo->m_atMtExt[nLoop]), sizeof(TMtExt));
            }
            
            SendReplyBack( cServMsg, MCU_MCS_MTLIST_NOTIF );
*/
			// [pengjie 2010/5/17] 通知界面下级各会议扩展信息（是否支持多回传）
			SendConfExtInfoToMcs( cServMsg.GetSrcSsnId(),GetFstMcuIdFromMcuIdx(tMcu.GetMcuIdx()) );
			// End
        }        
	}
	else
	{
		tMcu.SetEqpId( m_ptMtTable->m_byMaxNumInUse );

		//得到本级别名列表
		cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
		cServMsg.CatMsgBody( ( u8 * )m_ptMtTable->m_atMtExt, 
			                 m_ptMtTable->m_byMaxNumInUse * sizeof( TMtExt ) );

		for (u8 byMtId = 1; byMtId <= m_ptMtTable->m_byMaxNumInUse; byMtId ++)
		{
			TMtAlias tMtAlias;
			if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeH320Alias, &tMtAlias ))
			{
				if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeH320ID, &tMtAlias ))
				{
					if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeH323ID, &tMtAlias ))
					{
						if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeE164, &tMtAlias ))
						{
							if( !m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeTransportAddress, &tMtAlias ) )
							{
								m_ptMtTable->GetDialAlias( byMtId, &tMtAlias );
							}
						}
					}
				}		
			}
			
			cServMsg.CatMsgBody( (u8*)tMtAlias.m_achAlias, VALIDLEN_ALIAS + MAXLEN_CONFNAME );
		}

		SendReplyBack( cServMsg, MCU_MCS_MTLIST_NOTIF ); // 本级则不用管 [pengguofeng 7/2/2013]	}
	}
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
}


/*====================================================================
    函数名      ：ProcMcsMcuRefreshMcuCmd
    功能        ：刷新其他MCU MT列表
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/2/24     3.6         Jason        创建
	10/12/30    4.6         liuxu        修改
====================================================================*/
void CMcuVcInst::ProcMcsMcuRefreshMcuCmd( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	//得到MCU号
	TMcu tMcu = *(TMcu*)cServMsg.GetMsgBody(); 

	u16 wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
	if( !m_tCascadeMMCU.IsNull() && !IsValidMcuId(wMMcuIdx))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcsMcuRefreshMcuCmd] Fail to Get m_tCascadeMMCU McuIdx\n" );
	}

	for( u16 wLoop = 0; wLoop < m_tConfAllMtInfo.GetMaxMcuNum(); wLoop++ )
	{
		u16 wMcuIdx = m_tConfAllMtInfo.GetMtInfo(wLoop).GetMcuIdx();
		if( wMcuIdx == tMcu.GetMcuIdx() )
		{
			//请求终端列表
			TMcuMcuReq tReq;
			memset(&tReq, 0, sizeof(tReq));
			cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
			u8 abyMcuId[MAX_CASCADEDEPTH-1];
			memset( &abyMcuId[0],0,sizeof(abyMcuId) );
			if( m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] ))
			{
				SendMsgToMt( abyMcuId[0], MCU_MCU_MTLIST_REQ,  cServMsg);
			
				if( m_tCascadeMMCU.IsNull() || 
					(!m_tCascadeMMCU.IsNull() && wMMcuIdx != wMcuIdx) )
				{
					//为下级mcu，获取下级直连终端状态
					OnGetMtStatusCmdToSMcu( abyMcuId[0] );
				}
			}
			break;
		}
	}
}


/*====================================================================
    函数名      ：ProcMcsMcuGetConfInfoReq
    功能        ：得到会议信息处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/27    3.0         胡昌威          创建
    08/11/18    4.5         张宝卿          处理MAU信息获取
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetConfInfoReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	switch (pcMsg->event)
	{
    case MCS_MCU_GETCONFINFO_REQ:
    case MT_MCU_GETCONFINFO_REQ:
        
        //发给会议信息表
        cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
        break;

    case MCS_MCU_GETMAUSTATUS_REQ:
        {
//             TMcuHdBasStatus tStatus;
//             m_pcBasMgr->GetHdBasStatus(tStatus, m_tConf);
//             cServMsg.SetMsgBody((u8*)&tStatus, sizeof(tStatus));
//             SendReplyBack( cServMsg, cServMsg.GetEventId()+1 );
        }
        break;
    default:
        ConfPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMcsMcuGetConfInfoReq] unexpected msg.%d<%s>!\n",
                                pcMsg->event, OspEventDesc(pcMsg->event));
        break;
	}
    return;
}

/*====================================================================
    函数名      ：ProcMcsMcuVMPReq
    功能        ：处理Vmp给Mcu的通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/10/31    4.0         libo          创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuVMPReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    cServMsg.SetEventId(pcMsg->event);

    VmpCommonReq(cServMsg);
}


/*====================================================================
    函数名      ：CheckVmpParam
    功能        ：检查并修正画面合成参数
    算法实现    ：
    引用全局变量：
    输入参数说明：tVmpParam[IN/OUT] :画面合成参数
	              wErrorCode[IN/OUT]:参数不合理错误码
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/06/09    4.5         付秀华        整理创建
	10/01/18	4.6.3		薛亮		  修改
====================================================================*/
BOOL32 CMcuVcInst::CheckVmpParam(const u8 byVmpId, TVMPParam_25Mem& tVmpParam, u16& wErrorCode)
{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	if(tVmpParam.GetMaxMemberNum() > MAXNUM_SDVMP_MEMBER)//8ke 8kh不支持16风格以上
	{
		//主要针对终端控制台上的操作做限制
		return FALSE;
	}
#endif
	
	if (!IsValidVmpId(byVmpId))
	{
		return FALSE;
	}

	//检查是否动态分屏
	if( tVmpParam.GetVMPStyle() == VMP_STYLE_DYNAMIC )
	{
		//卫星会议不支持自动画面合成
		if (m_tConf.GetConfAttrb().IsSatDCastMode()) 
		{
			wErrorCode = ERR_MCU_DCAST_NOOP;
			return FALSE;
		}
		if( !tVmpParam.IsVMPAuto() )
		{
			wErrorCode = ERR_MCU_DYNAMICMUSTBYAUTO;		
			return FALSE;
		}
		return TRUE;
	}  
	
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	// 成员设置错误则直接清空
	UpdateVmpMembersWithConfInfo(tVmpEqp, tVmpParam);
	
	//zjj20100428 看会控指定的成员是否有单回传mcu下的终端占用多个通道的情况
	wErrorCode = UpdateVmpMembersWithMultiSpy(tVmpEqp, tVmpParam);
	if (0 != wErrorCode)
	{
		return FALSE;
	}
	
	return TRUE;
}
/*====================================================================
    函数名      ：VmpCommonReq
    功能        ：处理Vmp给Mcu的通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/04    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::VmpCommonReq(CServMsg & cServMsg)
{
	
	switch( CurState() )
	{
	case STATE_ONGOING:

		//来自非主席终端，nack
		if( cServMsg.GetSrcMtId() != 0 && 
			cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId() )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_OPER );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			
			//取消主席
			SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
				
			return;
		}

		switch(cServMsg.GetEventId())
		{
		case MCS_MCU_STARTVMP_REQ:       //会控开始视频复合请求
		case MT_MCU_STARTVMP_REQ:        //主席开始视频复合请求 
			{
				ProcStartVmpReq(cServMsg);
				break;
			}
	    case MCS_MCU_CHANGEVMPPARAM_REQ: //会控会议控制台请求MCU改变视频复合参数
	    case MT_MCU_CHANGEVMPPARAM_REQ:  //主席会议控制台请求MCU改变视频复合参数
			{
				ProcChangeVmpParamReq(cServMsg); 
				break;
			}
		case MCS_MCU_START_VMPBATCHPOLL_REQ:// xliang [12/18/2008] 开始vmp 批量轮询
			{
				ProcStartVmpBatchPollReq(cServMsg);
				break;
			}
	    case MCS_MCU_STOPVMP_REQ:        //会控结束视频复合请求	
	    case MT_MCU_STOPVMP_REQ:         //主席结束视频复合请求
			{
				ProcStopVmpReq(cServMsg);
				break;
			}
	    case MCS_MCU_GETVMPPARAM_REQ:    //会控查询视频复合参数请求
	    case MT_MCU_GETVMPPARAM_REQ:     //主席查询视频复合参数请求
			{
				TVMPParam_25Mem tVmpParam;
				u8 byVmpId;
				TEqp tVmpEqp;
				cServMsg.SetMsgBody();
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "%s return param:", OspEventDesc(cServMsg.GetEventId()));
				//MCS_MCU_GETVMPPARAM_ACK内容：n*(vmpparam+vmpid)
				for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
				{
					if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
					{
						continue;
					}
					byVmpId = m_abyVmpEqpId[byIdx];
					tVmpEqp = g_cMcuVcApp.GetEqp(byVmpId);
					tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
					cServMsg.CatMsgBody( (u8*)&tVmpParam, sizeof(tVmpParam) );
					cServMsg.CatMsgBody( &byVmpId, sizeof(byVmpId) );
					
					tVmpParam.Print();
				}
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

			}
			break;

		case MCS_MCU_STARTVMPBRDST_REQ:         //会议控制台命令MCU开始把画面合成图像广播到终端
		case MT_MCU_STARTVMPBRDST_REQ:		//主席命令MCU开始把画面合成图像广播到终端
			{
				ProcStartVmpBrdReq(cServMsg);
				break;
			}

		case MCS_MCU_STOPVMPBRDST_REQ:          //会议控制台命令MCU停止把画面合成图像广播到终端
		case MT_MCU_STOPVMPBRDST_REQ:		//主席命令MCU停止把画面合成图像广播到终端			
			{
				ProcStopVmpBrdReq(cServMsg);
				break;
			}
		default:
			break;
		}

		break;

	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "Wrong message %u(%s) received in state %u!\n", 
			   cServMsg.GetEventId(), ::OspEventDesc(cServMsg.GetEventId()), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuGetMtListReq
    功能        ：处理图像合成请求处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/10/31    4.0         libo        创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuVmpTwReq(const CMessage * pcMsg)
{    
    if (STATE_ONGOING != CurState())
    {
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Wrong message %u(%s) received in state %u!\n", 
        pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    cServMsg.SetEventId(pcMsg->event);

    VmpTwCommonReq(cServMsg);
}
/*====================================================================
    函数名      ：VmpTwCommonReq
    功能        ：处理多画面电视墙合成请求处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/12/03    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::VmpTwCommonReq(CServMsg & cServMsg)
{
    u8 byVmpTwId;
    u8 byVmpTwStyle;
    u8 byLoop;
    TVMPParam_25Mem tVmpTwParam;
    TPeriEqpStatus tPeriEqpStatus;
    TMt tMt;

    //zbq[01/08/2007] 非科达终端拒绝加入VMPTW
    BOOL32 bNonKEDAReject = FALSE;
    TLogicalChannel tLogicChan;

    //得到空闲的画面合成器 重新调整MPW是否启用 判断策略 [12/27/2006-zbq]
    u8 byIdleMPWNum = 0;
    u8 abyIdleMPWId[MAXNUM_PERIEQP];
    memset( abyIdleMPWId, 0, sizeof(abyIdleMPWId) );
    g_cMcuVcApp.GetIdleVmpTw( abyIdleMPWId, byIdleMPWNum, sizeof(abyIdleMPWId) );

    switch(cServMsg.GetEventId())
    {
    case MCS_MCU_STARTVMPTW_REQ:       //会控开始视频复合请求

		GetVmpParamFormBuf(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen(), tVmpTwParam);
        if(g_bPrintEqpMsg)
        {
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[VmpTwCommonReq] change vmp param:\n");
            tVmpTwParam.Print();
        }

        //对于会控控制模式下，若跟随方式无相应终端，则终端信息清空       
        memset(&tMt, 0, sizeof(TMt));
        for (byLoop = 0; byLoop < tVmpTwParam.GetMaxMemberNum(); byLoop++)
        {
            TVMPMember tVMPMember = *tVmpTwParam.GetVmpMember(byLoop);
            if (0 != tVMPMember.GetMtId())
            {
                if ((VMP_MEMBERTYPE_CHAIRMAN == tVMPMember.GetMemberType() &&
                     !HasJoinedChairman()) ||
                    (VMP_MEMBERTYPE_SPEAKER == tVMPMember.GetMemberType() &&
                     (!HasJoinedSpeaker() || GetLocalSpeaker().GetType() == TYPE_MCUPERI) ) )
                {
                    tMt.SetNull();
                    tVMPMember.SetMemberTMt(tMt);
                    tVmpTwParam.SetVmpMember(byLoop, tVMPMember);
                }
                else if ( m_ptMtTable->GetMtLogicChnnl(tVMPMember.GetMtId(), LOGCHL_VIDEO, &tLogicChan, FALSE) &&
                          MEDIA_TYPE_H264 == tLogicChan.GetChannelType() &&
                          VIDEO_FORMAT_CIF != tLogicChan.GetVideoFormat() &&
						  !IsKedaMt(tVMPMember, TRUE))
                {
                    tMt.SetNull();
                    tVMPMember.SetMemberTMt( tMt );
                    tVmpTwParam.SetVmpMember( byLoop, tVMPMember );
                    
                    bNonKEDAReject = TRUE;
                }                
                else if ( !m_tConfAllMtInfo.MtJoinedConf(tVMPMember) )
                {
                    tMt.SetNull();
                    tVMPMember.SetMemberTMt(tMt);
                    tVmpTwParam.SetVmpMember(byLoop, tVMPMember);
                }
                else
                {
                    if (!tVMPMember.IsLocal())
                    {
                        //有风险
                        OnMMcuSetIn((TMt)tVMPMember, 0, SWITCH_MODE_SELECT);
                        tMt = m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx( tVMPMember.GetMcuIdx() ));
                        tVMPMember.SetMemberTMt(tMt);
                        tVmpTwParam.SetVmpMember(byLoop, tVMPMember);
                    }
                }
            }
        }
        if ( bNonKEDAReject )
        {
            NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_NONKEDAMT_JOINVMP );
        }

        //检查当前会议的画面合成状态
        if (CONF_VMPTWMODE_NONE != m_tConf.m_tStatus.GetVmpTwMode())
        {
            cServMsg.SetErrorCode(ERR_MCU_VMPRESTART);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);

            return;
        }

        //检查是否动态分屏
        if (VMP_STYLE_DYNAMIC == tVmpTwParam.GetVMPStyle())
        {
            cServMsg.SetErrorCode(ERR_INVALID_VMPSTYLE);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);

            return;
        }

        //是否有空闲的画面合成器 
        if (0 == byIdleMPWNum)
        {
            cServMsg.SetErrorCode(ERR_MCU_NOIDLEVMP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);

            return;
        }
        else
        {
            u16 wError = 0;
            if ( !IsMCUSupportVmpStyle(tVmpTwParam.GetVMPStyle(), byVmpTwId, EQP_TYPE_VMPTW, wError) )
            {
                cServMsg.SetErrorCode( ERR_MCU_ALLIDLEVMP_NO_SUPPORT );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
        }

        // xsl [7/20/2006]卫星分散会议需要检查回传通道数
        if (m_tConf.GetConfAttrb().IsSatDCastMode()) 
        {            
            if (//IsOverSatCastChnnlNum(tVmpTwParam))
				!IsSatMtCanContinue())
            {
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[VmpTwCommonReq-start] over max upload mt num. nack!\n");
                cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
        }

        m_tVmpTwEqp = g_cMcuVcApp.GetEqp(byVmpTwId);
        m_tVmpTwEqp.SetConfIdx(m_byConfIdx);

        // 回 ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

        //设置画面合成参数 
        g_cMcuVcApp.GetPeriEqpStatus(byVmpTwId, &tPeriEqpStatus);
        tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::RESERVE;//先占用,超时后未成功再放弃
        tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpTwParam);
        tPeriEqpStatus.SetConfIdx(m_byConfIdx);
        g_cMcuVcApp.SetPeriEqpStatus(byVmpTwId, &tPeriEqpStatus);

        // 用户由 自动复合->非自动复合 需要先进行停止操作，两者不允许直接CHANGE过渡
        byVmpTwStyle = tVmpTwParam.GetVMPStyle();
        m_tConf.m_tStatus.SetVmpTwStyle(byVmpTwStyle);

        //开始画面合成
        ChangeVmpTwParam(&tVmpTwParam, TRUE);

        break;

    case MCS_MCU_CHANGEVMPTWPARAM_REQ: //会控会议控制台请求MCU改变视频复合参数
    {
		GetVmpParamFormBuf(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen(), tVmpTwParam);
        if(g_bPrintEqpMsg)
        {
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[VmpTwCommonReq] change vmp param:\n");
            tVmpTwParam.Print();
        }  

        //检查当前会议的画面合成状态 
        if (CONF_VMPTWMODE_NONE == m_tConf.m_tStatus.GetVmpTwMode())
        {
            cServMsg.SetErrorCode(ERR_MCU_VMPNOTSTART);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            
            return;
        }

        // 顾振华 [5/22/2006] change的时候也要检查
        u16 wError = 0;
        if (!IsVMPSupportVmpStyle(tVmpTwParam.GetVMPStyle(), m_tVmpTwEqp.GetEqpId() , wError ))
        {
            cServMsg.SetErrorCode(wError);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);

            // 切换失败，增加提醒 [12/27/2006-zbq]
            // FIXME: 该提醒太过明朗，有待修改或删去，暂时保留
            byVmpTwId = 0;
            if ( IsMCUSupportVmpStyle(tVmpTwParam.GetVMPStyle(), byVmpTwId, EQP_TYPE_VMPTW, wError) )
            {
                NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_OTHERVMP_SUPPORT );
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[VmpTwCommonReq] curr VMPTW.%d can't support the style needed, other VMPTW.%d could\n",
                        m_tVmpTwEqp.GetEqpId(), byVmpTwId );
            }
            return;
        }

        //检查是否动态分屏
        if (VMP_STYLE_DYNAMIC == tVmpTwParam.GetVMPStyle())
        {
            cServMsg.SetErrorCode(ERR_INVALID_VMPSTYLE);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);

            return;
        }      
        
        //设置画面合成参数
        for (byLoop = 0; byLoop < tVmpTwParam.GetMaxMemberNum(); byLoop++)
        {
            TVMPMember tVMPMember = *tVmpTwParam.GetVmpMember(byLoop);
            if (0 != tVMPMember.GetMtId() && !tVMPMember.IsLocal())
            {
                //有风险
                OnMMcuSetIn((TMt)tVMPMember, 0, SWITCH_MODE_SELECT);
                TMt tTempMt;
                tTempMt = m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx(tVMPMember.GetMcuId()) );
                tVMPMember.SetMemberTMt(tTempMt);
                tVmpTwParam.SetVmpMember(byLoop, tVMPMember);
            }
        }

        // xsl [7/20/2006]卫星分散会议需要检查回传通道数
        if (m_tConf.GetConfAttrb().IsSatDCastMode()) 
        {            
            if (//IsOverSatCastChnnlNum(tVmpTwParam))
				!IsSatMtCanContinue())
            {
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[VmpTwCommonReq-change] over max upload mt num. nack!\n");
                cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
        }

        //ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId() , &tPeriEqpStatus);
        tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpTwParam);
        g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus);

        // 改变合成参数 
        ChangeVmpTwParam(&tVmpTwParam);
        break;
    }
    case MCS_MCU_STOPVMPTW_REQ:        //会控结束视频复合请求	

        //检查当前会议的画面合成状态 
        if (CONF_VMPTWMODE_NONE == m_tConf.m_tStatus.GetVmpTwMode())
        {
            cServMsg.SetErrorCode(ERR_MCU_VMPNOTSTART);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        //ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

        //停止
        g_cEqpSsnApp.SendMsgToPeriEqpSsn(m_tVmpTwEqp.GetEqpId(), MCU_VMPTW_STOPVIDMIX_REQ, 
                                         (u8 *)cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        break;

//    case MCS_MCU_GETVMPPARAM_REQ:    //会控查询视频复合参数请求
//    tVmpParam = m_tConf.m_tStatus.GetVmpParam();
//    cServMsg.SetMsgBody( (u8*)&tVmpParam, sizeof(TVMPParam) );
//    SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
//
//    g_cEqpSsnApp.SendMsgToPeriEqpSsn( m_tVmpEqp.GetEqpId(), 
//    MCU_VMP_GETVIDMIXPARAM_REQ, NULL, 0 );
//        break;
	default:
		break;
    }

}

/*====================================================================
    函数名      ：ProcMcsMcuGetConfStatusReq
    功能        ：得到会议状态处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/27    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetConfStatusReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

    cServMsg.SetMsgBody( ( u8 * const )&m_tConf.m_tStatus, sizeof( TConfStatus ) );	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
}

/*====================================================================
    函数名      ：ProcMcsMcuMcuMediaSrcReq
    功能        ：得到会议状态处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/2/27    3.6         Jason       创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuMcuMediaSrcReq( const CMessage * pcMsg )
{
	if (NULL == m_ptConfOtherMcTable)
	{
		return;
	}

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMcu tMcu =  *(TMcu *)cServMsg.GetMsgBody(); 

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuMcuMediaSrcReq] McuMcuId.%d - McuMtId.%d\n", 
		     tMcu.GetMcuIdx(), tMcu.GetMtId() );

	cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu) );
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
	u16 wMcuIdx = 0;
	//u8 byFstMcuId = 0,bySecMcuId = 0;
	u8 abyMcuId[MAX_CASCADEDEPTH-1];
	memset( &abyMcuId[0],0,sizeof(abyMcuId) );
	
	TMt tMt;

	TConfMcInfo *pMcInfo = NULL;
	for(s32 nLoop=0; nLoop < TConfOtherMcTable::GetMaxMcuNum(); nLoop++)
	{
		pMcInfo = m_ptConfOtherMcTable->GetMcInfo(nLoop);
		if (NULL == pMcInfo)
		{
			continue;
		}

		wMcuIdx = pMcInfo->GetMcuIdx();
		if( !IsValidSubMcuId(nLoop))
		{
			continue;
		}

		if( !m_tConfAllMcuInfo.GetMcuIdByIdx(wMcuIdx,&abyMcuId[0]) )
		{
			continue;
		}
		
		tMt = GetMcuMediaSrc(wMcuIdx);
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcTimerMcuSrcCheck] McuId.%d - MtMcuId.%d MtId.%d MtType.%d MtConfIdx.%d\n", 
				 wMcuIdx, tMt.GetMcuId(), tMt.GetMtId(), tMt.GetMtType(), tMt.GetConfIdx() );
		
		tMcu.SetMcuId(wMcuIdx);
		cServMsg.SetEventId(MCU_MCS_MCUMEDIASRC_NOTIF);
		cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
		cServMsg.CatMsgBody((u8 *)&tMt, sizeof(tMt));
		
		SendReplyBack(cServMsg, MCU_MCS_MCUMEDIASRC_NOTIF);
	}

	return;
}


/*====================================================================
    函数名      ProcMcsMcuLockSMcuReq
    功能        ：锁定下级MCU请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/2/27    3.6         Jason       创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuLockSMcuReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
 	TMcu *ptMcu = (TMcu *)cServMsg.GetMsgBody();
	u8 byLock = *(cServMsg.GetMsgBody()+sizeof(TMcu));

	if( NULL == m_ptConfOtherMcTable )
	{
		cServMsg.SetErrorCode( ERR_LOCKSMCU_NOEXIT );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	
	u16 wMMcuIdx = INVALID_MCUIDX;
	if( !m_tCascadeMMCU.IsNull() )
	{
		wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
	}
	if( (!ptMcu->IsNull() && ptMcu->IsLocal() ) || 
		( !m_tCascadeMMCU.IsNull() && ptMcu->GetMcuIdx() == wMMcuIdx/*m_tCascadeMMCU.GetMtId()*/ ) )
	{
		cServMsg.SetErrorCode( ERR_LOCKSMCU_NOEXIT );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	/* [2012/1/10 zhangli]byLock=1锁定，=0解锁，加锁时把会议的锁定模式m_byLockMode设置为CONF_LOCKMODE_LOCK(=2)，
	但是下级会直接把byLock(=1)的值发给界面，导致界面显示为CONF_LOCKMODE_NEEDPWD(=1)模式。这里根据byLock把正确的模式发给下级*/
	u8 byLockMode = CONF_LOCKMODE_NONE;
	if (ISTRUE(byLock))
	{
		byLockMode = CONF_LOCKMODE_LOCK;
	}

	CServMsg cMsg;
	cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen()-sizeof(u8));
	cMsg.CatMsgBody(&byLockMode, sizeof(u8));
	cMsg.SetEventId(MCU_MCU_LOCK_REQ);

	TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptMcu);
	u8   byMcuNum = 0;
	TConfMcInfo *ptMcInfo = NULL;
	
	if( ptMcu && !ptMcu->IsNull() && INVALID_MCUIDX != ptMcu->GetMcuIdx() )
	{
		ptMcInfo = m_ptConfOtherMcTable->GetMcInfo( ptMcu->GetMcuIdx() );
		if( NULL == ptMcInfo )
		{
			cServMsg.SetErrorCode( ERR_LOCKSMCU_NOEXIT );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
		SendMsgToMt( GetFstMcuIdFromMcuIdx( tLocalMt.GetMcuIdx() ), MCU_MCU_LOCK_REQ, cMsg);
	}
	else
	{
		for( u16 wLoop = 0; wLoop < TConfOtherMcTable::GetMaxMcuNum(); wLoop++ )
		{
			ptMcInfo = (m_ptConfOtherMcTable->GetMcInfo(wLoop));
			if( ( NULL == ptMcInfo || !IsValidSubMcuId(ptMcInfo->GetMcuIdx())) 
				|| ( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->GetMcuIdx() == wMMcuIdx ) )
			{
				continue;
			}

			if( m_tConfAllMcuInfo.IsSMcuByMcuIdx(ptMcInfo->GetMcuIdx()) )
			{
				SendMsgToMt( GetFstMcuIdFromMcuIdx(ptMcInfo->GetMcuIdx()), MCU_MCU_LOCK_REQ, cMsg);
			}
			
			byMcuNum++;
		}

		if( 0 == byMcuNum )
		{
			cServMsg.SetErrorCode( ERR_LOCKSMCU_NOEXIT );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
	}

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
}

/*====================================================================
    函数名      ：ProcMcsMcuVACReq
    功能        ：语音激励控制处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/27    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuVACReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    STATECHECK

    // xsl [8/19/2006] 卫星分散会议不支持语音激励
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
		cServMsg.SetErrorCode( ERR_MCU_DCAST_NOOP );            
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcMcsMcuVACReq] distributed conf not supported vac!\n");
        return;
    }

    MixerVACReq(cServMsg);

    return;
}

/*====================================================================
    函数名      ：ProcMcsMcuVACReq
    功能        ：语音激励控制处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/27    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::MixerVACReq(CServMsg & cServMsg)
{
    TPeriEqpStatus tPeriEqpStatus;
    u8 byEqpId = 0;

    //来自非主席终端，nack
    if (cServMsg.GetSrcMtId() != 0 &&
        cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId())
    {
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR: Message is from ordinary terminal!\n");
        cServMsg.SetErrorCode(ERR_MCU_INVALID_OPER);
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);

        //取消主席
        SendReplyBack(cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF);
        return;
    }

    u16 wError = 0;
    switch (cServMsg.GetEventId())
    {		
    case MCS_MCU_STARTVAC_REQ:        //会议控制台请求MCU开始语音激励控制发言
    case MT_MCU_STARTVAC_REQ:
        
        //已处于语音激励控制发言状态, NACK
        if (m_tConf.m_tStatus.IsVACing())
        {
			ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq]ERROR: already in VAC mode.so nack!\n");
            cServMsg.SetErrorCode( ERR_MCU_CONFINVACMODE );
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
		//正在放像，不能开启语音激励 
		if( m_tConf.m_tStatus.IsPlaying() 
			|| m_tConf.m_tStatus.IsPlayReady())
		{
			cServMsg.SetErrorCode( ERR_MCU_CONFPLAYING );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			ConfPrint( LOG_LVL_WARNING, MID_MCU_MIXER, "[MixerVACReq]Conf(%s) playing now!So Can't StartVac!\n", 
				m_tConf.GetConfName() );
			return;
		}
        //没有空闲的混音器
        if(m_tConf.m_tStatus.IsNoMixing())
        {
			TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];
			memset(atAudioTypeDesc, 0, sizeof(atAudioTypeDesc));
			u8 byAudioCapNum = m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);

            byEqpId = g_cMcuVcApp.GetIdleMixer(0, byAudioCapNum, atAudioTypeDesc); // 得到空闲的且混音容量最大的混音器
            if( byEqpId == 0 )
            {
                ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR: no idle MIXER!\n");
                cServMsg.SetErrorCode(ERR_MCU_NOIDLEMIXER_INCONF);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
                return;
            }
			if (!CheckMixerIsValide(byEqpId))
			{
                ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR: mixer(%d) is invalid!\n",byEqpId);                cServMsg.SetErrorCode(ERR_MCU_NOIDLEMIXER_INCONF);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
        }        
        else
        {
            byEqpId =  m_tMixEqp.GetEqpId();
        }

        // 如果已开始轮询，则不能开始语音激励
		// mqs [03/21/2011] 因为两者都是设置发言人，并且策略不同，所以不能同时开启
        if (m_tConf.m_tStatus.GetPollMode() != CONF_POLLMODE_NONE) 
        {
			ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR:ConfPolling can't start vac!\n");
            cServMsg.SetErrorCode(ERR_MCU_CONFSTARTPOLLVACMODE);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        // xsl [7/24/2006] 定制混音不能开启语音激励
        if (m_tConf.m_tStatus.IsSpecMixing())
        {
            ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR:MixSpec can't start VAC!\n");
            cServMsg.SetErrorCode(ERR_MCU_VACMUTEXSPECMIX);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        // guzh [1/30/2007] 8000B 能力限制        
        if ( ! CMcuPfmLmt::IsVacOprSupported( m_tConf, m_tConfAllMtInfo.GetLocalJoinedMtNum(), wError ) )
        {
			ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR: 8000b's capacity limit!\n");
            cServMsg.SetErrorCode(wError);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
        
		//20100708_tzy 当在线终端数大于混音器能力时拒绝
        if (m_tConfAllMtInfo.GetLocalJoinedMtNum() > GetMaxMixNum(byEqpId))
        {
			ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR: JoinedMtNum(%d)  exceed Mixer(%d)'s capacity(%d). nack!\n",
				     m_tConfAllMtInfo.GetLocalJoinedMtNum(), byEqpId, GetMaxMixNum(byEqpId));

            cServMsg.SetErrorCode(ERR_MCU_OVERMAXMIXERCHNNL);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
        }
       
		//ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

		//lukp[12/07/2009] 由于MCU_MT_STARTVAC_NOTIF 与 MCU_MT_STARTVAC_ACK回给mt的消息一样。
		//在此过滤掉MT_MCU_STARTVAC_REQ
		if (HasJoinedChairman() && (MT_MCU_STARTVAC_REQ != cServMsg.GetEventId()))
		{
			SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STARTVAC_NOTIF, cServMsg);
        }

        if (m_tConf.m_tStatus.IsMixing())
        {
			u8 byMixerSubtype = UNKONW_MIXER;
			g_cMcuAgent.GetMixerSubTypeByEqpId(m_tMixEqp.GetEqpId(),byMixerSubtype);
            if (byMixerSubtype == EAPU_MIXER || byMixerSubtype == APU2_MIXER)
            {
                //混音开启情况下，只要发送开启语音激励命令
                SendMsgToEqp(m_tMixEqp.GetEqpId(),MCU_MIXER_STARTVAC_CMD,cServMsg);
            }
            m_tConf.m_tStatus.SetVACing();
            SetMixerSensitivity(m_tConf.GetTalkHoldTime());

            u8 byMtId = m_tVacLastSpeaker.GetMtId();

            if (!m_tVacLastSpeaker.IsNull() &&
                byMtId != GetLocalSpeaker().GetMtId() &&
                !m_ptMtTable->IsMtAudioDumb(byMtId) &&
                (!m_tConf.m_tStatus.IsSpecMixing() ||
                (m_tConf.m_tStatus.IsSpecMixing() && m_ptMtTable->IsMtInMixing(byMtId))))
            {
                //change video switch
                ChangeSpeaker(&m_tVacLastSpeaker);
            }

            //通知主席及所有会控
            SendMsgToAllMcs(MCU_MCS_STARTVAC_NOTIF, cServMsg);

            //zbq [11/26/2007] 单独上报
            MixerStatusChange();
        }
        else
        {
			u8 byMixerSubtype = UNKONW_MIXER;
			g_cMcuAgent.GetMixerSubTypeByEqpId(byEqpId,byMixerSubtype);
            if (byMixerSubtype == EAPU_MIXER || byMixerSubtype == APU2_MIXER )
            {
                StartEmixerVac(byEqpId);
            }
            else//APU时走原来逻辑
            {
                StartMixing( mcuVacMix ,byEqpId);
            }
        }
        break;

    case MCS_MCU_STOPVAC_REQ:     //会议控制台请求MCU停止语音激励控制发言	
    case MT_MCU_STOPVAC_REQ:      //终端请求MCU停止语音激励控制发言
        
		//尚未处于处于语音激励控制发言状态, NACK
        if (!m_tConf.m_tStatus.IsVACing())
        {
			ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixerVACReq] ERROR: Conf is not VAC. So can't stop!\n");
            cServMsg.SetErrorCode(ERR_MCU_CONFNOTINVACMODE);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
        //ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
        
        if (m_tConf.m_tStatus.IsNoMixing())
        {
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[MixerVACReq] begin to stopVacing!\n");
            
			u8 byMixerSubtype = UNKONW_MIXER;
			g_cMcuAgent.GetMixerSubTypeByEqpId(m_tMixEqp.GetEqpId(),byMixerSubtype);
            if (byMixerSubtype == EAPU_MIXER || byMixerSubtype == APU2_MIXER)
            {
                SendMsgToEqp(m_tMixEqp.GetEqpId(),MCU_MIXER_STOPVAC_CMD,cServMsg);
                g_cMcuVcApp.GetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);
                tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[0].m_byGrpState = TMixerGrpStatus::READY;
				tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpId = 0;
				tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byConfId = 0;
                g_cMcuVcApp.SetPeriEqpStatus(m_tMixEqp.GetEqpId(), &tPeriEqpStatus);
                m_tConf.m_tStatus.SetVACing(FALSE);
                //停止向混音器交换数据
                TMt            tMt;
                for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
                {
                    memset(&tMt,0,sizeof(TMt));
                    if (m_tConfAllMtInfo.MtJoinedConf(byMtId))            
                    {
                        tMt = m_ptMtTable->GetMt( byMtId );
                        StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
                            (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt)), FALSE, MODE_AUDIO );
                        m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);
                    }   
                }
                if (HasJoinedChairman())
                {
                    SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cServMsg );
                }
                
				//通知主席及所有会控
                SendMsgToAllMcs(MCU_MCS_STOPVAC_NOTIF, cServMsg);
                
				//混音状态通知,通知主席及所有会控
                MixerStatusChange();            
                NotifyChairmanMixMode();
                if ( !m_tCascadeMMCU.IsNull() )
                {
                    OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId());
                }
                cServMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
    
                m_tVacLastSpeaker.SetNull();
				TMixParam tMixParam ;
				m_tConf.m_tStatus.SetMixerParam(tMixParam);
                m_tMixEqp.SetNull();
				m_byLastMixMode = mcuNoMix;
				memset( m_abyMixMtId, 0, sizeof(m_abyMixMtId) );
				memset(m_atMixMt,0,sizeof(m_atMixMt));
            }
            else//APU时走原来逻辑
            {
                StopMixing();
            }
        }
        else
        {
			u8 byMixerSubtype = UNKONW_MIXER;
			g_cMcuAgent.GetMixerSubTypeByEqpId(m_tMixEqp.GetEqpId(),byMixerSubtype);
            if (byMixerSubtype == EAPU_MIXER || byMixerSubtype == APU2_MIXER)
            {
                SendMsgToEqp( m_tMixEqp.GetEqpId(), MCU_MIXER_STOPVAC_CMD, cServMsg );
            }
            // guzh [11/7/2007] 只设置状态
            m_tConf.m_tStatus.SetVACing(FALSE);
            //通知主席及所有会控
            SendMsgToAllMcs(MCU_MCS_STOPVAC_NOTIF, cServMsg);
            if (HasJoinedChairman())
            {
                SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cServMsg);
            }
            //zbq [11/26/2007] 单独上报
            MixerStatusChange();  
			m_byLastMixMode = mcuWholeMix;
        }

        break;

    default:
        break;
    }
}
/*====================================================================
    函数名      ：ProcMcsMcuMixReq
    功能        ：混音控制处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/27    3.0         胡昌威        创建
====================================================================*/		
void CMcuVcInst::ProcMcsMcuMixReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	

	STATECHECK;

    switch(cServMsg.GetEventId())
    {
    case MCS_MCU_STARTDISCUSS_REQ:      //会控开始智能混音 - 用于本级
        ProcMixStart(cServMsg);
        break;
        
    case MCS_MCU_STOPDISCUSS_REQ:       //会控停止智能混音 - 用于本级停
        ProcMixStop(cServMsg);
        break;

    case MCS_MCU_GETMIXPARAM_REQ:       //会议控制台查询混音参数 - 用于本级停
        {
            u8 byMixMode = m_tConf.m_tStatus.GetMixerParam().GetMode();
            cServMsg.SetMsgBody((u8*)&byMixMode, sizeof(u8));
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);		
            break;
        }
    default:	break;
    }
    return;
}

/*=============================================================================
  函 数 名： ProcMcsMcuChgMixDelayReq
  功    能： 混音延时设置
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::ProcMcsMcuChgMixDelayReq(const CMessage * pcMsg)
{
    STATECHECK

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if(0 == m_tMixEqp.GetEqpId())
    {
        cServMsg.SetErrorCode(ERR_MCU_CONFNOTMIXING);
        SendReplyBack(cServMsg, pcMsg->event+2);
        return;
    }
    
    u16 wDelayTime = *(u16 *)cServMsg.GetMsgBody();

    m_tConf.SetMixDelayTime(ntohs(wDelayTime));
    SetMixDelayTime(wDelayTime);

    SendReplyBack(cServMsg, pcMsg->event+1);
    SendMsgToAllMcs(MCU_MCS_CHANGEMIXDELAY_NOTIF, cServMsg);

    return;
}

/*====================================================================
    函数名      ：ProcStopVmpPoll
    功能        ：停止Vmp单通道轮询
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    12/05/02    4.7         杨怀志         创建
====================================================================*/
void CMcuVcInst::ProcStopVmpPoll( void )
{
	u8 byPollState = m_tVmpPollParam.GetPollState();
	// 停止轮询时,vmp轮询通道无需清理,只需KillTimer即可
    KillTimer(MCUVC_VMPPOLLING_CHANGE_TIMER); 

    ConfPrint( LOG_LVL_DETAIL, MID_MCU_VMP, "[ProcStopVmpPoll] stop vmp channel poll.\n");

	// 停轮询清轮询状态
    m_tVmpPollParam.SetPollState(POLL_STATE_NONE);
	m_tVmpPollParam.SetMediaMode(MODE_NONE);
	
	//停止轮询的时候清掉当前轮询终端
	TMtPollParam tMtPollParam = m_tVmpPollParam.GetMtPollParam();
	TMt tNullMt;
	tNullMt.SetNull();
	tMtPollParam.SetTMt(tNullMt);
	m_tVmpPollParam.SetMtPollParam(tMtPollParam);

	// 原本在轮询中,停止时向界面发通知
	if (POLL_STATE_NONE != byPollState)
	{
		CServMsg cServMsg;
		cServMsg.SetMsgBody((u8 *)&m_tVmpPollParam, sizeof(m_tVmpPollParam));
		SendMsgToAllMcs(MCU_MCS_VMPPOLLSTATE_NOTIF, cServMsg);
	}

	return;
}

/*====================================================================
    函数名      ：GetNextMtPolledForVmp
    功能        ：得到下一个符合被轮询条件的终端,用于vmp单通道轮询
    算法实现    ：
    引用全局变量：
    输入参数说明：[IN/OUT] u8 &byPollPos 当前/下一个轮询的终端
                  [IN/OUT] TPollInfo& tPollInfo  轮询的信息
    返回值说明  ：TMtPollParam *，如果找不到下一个，则返回 NULL
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    12/05/02    4.7         杨怀志         创建
====================================================================*/
TMtPollParam *CMcuVcInst::GetNextMtPolledForVmp(u8 &byPollPos, TPollInfo& tPollInfo)
{
    // 首先检查用户是否指定了轮询位置
    if ( m_tVmpPollParam.IsSpecPos() )
    {
        byPollPos = m_tVmpPollParam.GetSpecPos();
        m_tVmpPollParam.ClearSpecPos();
        if ( byPollPos == POLLING_POS_START )
        {
            // 从头开始轮询
            byPollPos = 0;
        }
    }
    else
    {
        byPollPos ++;
    }   

    u8 wFindMtNum = 0;
    BOOL32 bJoined = FALSE;
    BOOL32 bSendVideo = FALSE;
	BOOL32 bVideoLose = FALSE;
	
	// [7/1/2010 xliang] 级联回传过滤：（目前包括发言人，合成）
	BOOL32 bCasSpyFiltered		 = FALSE;
    BOOL32 bCasMcuSpeaker		 = FALSE;
	BOOL32 bCasVmpMemberFiltered = FALSE;

    TMtPollParam tCurMtParam, *ptMtPollParam = NULL;
    do 
    {
        // 新一轮的轮询开始
        if (byPollPos >= m_tVmpPollParam.GetPolledMtNum())
        {
            // 根据轮询次数决定下面的方向
            u32 dwPollNum = tPollInfo.GetPollNum();
            if (1 != dwPollNum)
            {
                // rewind to top
                byPollPos = 0;
                if (0 != dwPollNum)
                {
                    tPollInfo.SetPollNum(dwPollNum - 1);
                }            
            }
            else
            {
                // polling to end, all over
                tPollInfo.SetPollNum(0);
                return NULL;
            }
        }
        
		ptMtPollParam = m_tVmpPollParam.GetPollMtByIdx(byPollPos);
        if ( NULL != ptMtPollParam)
        {
			tCurMtParam = *ptMtPollParam;
        }
		else
		{
            break;
		}

        if (tCurMtParam.IsLocal())
        {
            TMtStatus tMtStatus;
            m_ptMtTable->GetMtStatus(tCurMtParam.GetMtId(), &tMtStatus);
            bJoined = m_tConfAllMtInfo.MtJoinedConf( tCurMtParam.GetMtId() );
            bSendVideo = tMtStatus.IsSendVideo();
            
            // guzh [7/27/2006] 本级肯定不是下级MCU发言人，一定要赋值
            bCasMcuSpeaker = FALSE;

			bCasVmpMemberFiltered = FALSE;

            // zbq [03/09/2007] 跳过当前没有视频的终端
            bVideoLose = tMtStatus.IsVideoLose();
			
        }
        else
        {
            TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tCurMtParam.GetMcuId());
            if (NULL != ptMcInfo)
            {
                TMcMtStatus *pMcMtStatus = ptMcInfo->GetMtStatus((TMt &)tCurMtParam);
                if (NULL != pMcMtStatus)
                {
                    bJoined = m_tConfAllMtInfo.MtJoinedConf(tCurMtParam.GetMcuId(), tCurMtParam.GetMtId());

					//级联终端状态本字段非实时上报，仅依赖于list请求时下级MCU的终端状态. 可能由于下级终端打开逻辑通道的响应慢而导致误判.
					bSendVideo = TRUE;
					bCasVmpMemberFiltered = FALSE;
					
					// [7/1/2010 xliang]  如果上级有其他指定操作使下级某终端在回传，对于单回传而言做过滤.
					// 例如：
					// 1,上级已有下级某终端做全局发言人，则跳过轮询到的下级成员
					// 2,VMP中有指定的下级成员，则需跳过所有轮询到的下级成员
					// 2011-11-3 add by pgf
					// 3.已经有单回传MCU的成员或自身在VMP中，则过滤在此MCU下的任一轮询终端
								
					if( !IsLocalAndSMcuSupMultSpy(tCurMtParam.GetMcuId()) )
					{
						bCasMcuSpeaker = ( GetFstMcuIdFromMcuIdx(m_tConf.GetSpeaker().GetMcuIdx()) == GetFstMcuIdFromMcuIdx(tCurMtParam.GetMcuIdx()));
						
						TEqp tVmpEqp = g_cMcuVcApp.GetEqp(GetTheOnlyVmpIdFromVmpList());
						TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
						if( tVmpParam.GetVMPMode() != CONF_VMPMODE_NONE)
						{
							if( tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_VMPCHLPOLL))
							{
								for (u8 byChnnl = 0; byChnnl < MAXNUM_VMP_MEMBER; byChnnl++)
								{
									// compare VmpMember with tCurPollMt
									TVMPMember *ptVmpMember = tVmpParam.GetVmpMember(byChnnl);
									
									// pass Null Member or MEMBERTYPE_POLL
									if ( ptVmpMember == NULL || ptVmpMember->IsNull()
										|| ptVmpMember->GetMemberType() == VMP_MEMBERTYPE_VMPCHLPOLL)
									{
										continue;
									}
									
									// 比较他们在本级的表示即可
									TMt tTempMt = (TMt)(*ptVmpMember);
									if ( GetLocalMtFromOtherMcuMt(tCurMtParam.GetTMt()) == GetLocalMtFromOtherMcuMt(tTempMt))
									{
										ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "VmpMember<%d %d>(chnnl:%d) and PollMt<%d %d> are in same mcu\n",
											ptVmpMember->GetMcuIdx(), ptVmpMember->GetMtId(), byChnnl,
											tCurMtParam.GetMcuIdx(), tCurMtParam.GetMtId());
										bCasVmpMemberFiltered = TRUE;
										break;
									}
									
								}
							}
						}

					}
					else //如果下级mcu支持多回传则不必考虑上级是否有回传源
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "multiSpy support, no need to casfilter!\n");
						bCasMcuSpeaker = FALSE;
						bCasVmpMemberFiltered = FALSE;
					}

                    // 跳过当前没有视频的下级MCU终端
                    bVideoLose = pMcMtStatus->IsVideoLose();
                }
            }
        }

		// 20120604 yhz 标记
		//若后续MPU与8khvmp也支持vmp单通道轮询,终端不能进vmp多通道时,此处需追加判断,跳过已在其它通道的终端
		
		bCasSpyFiltered	= ( bCasMcuSpeaker || bCasVmpMemberFiltered );

		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[GetNextMtPolledForVmp] SpeakerFilter:(%d), VmpFilter:(%d)!\n", 
			bCasMcuSpeaker, bCasVmpMemberFiltered );
		
        if ( bJoined &&  
             !bCasSpyFiltered &&
             !bVideoLose &&
             bSendVideo 
            )
        {
            break;
        }
        else
        {
            ConfPrint( LOG_LVL_DETAIL, MID_MCU_VMP, "[GetNextMtPolledForVmp] MT(%d,%d) skipped for Joined.%d, SendVideo.%d, SpeakerFilter.%d, VmpFilter.%d!\n", 
                tCurMtParam.GetMcuId(), 
                tCurMtParam.GetMtId(),
                bJoined, 
                bSendVideo, 
                bCasMcuSpeaker,
				bCasVmpMemberFiltered);
        }

        byPollPos++;
        wFindMtNum++;

    // 直到找满一圈
    }while (wFindMtNum<m_tVmpPollParam.GetPolledMtNum());

    if (wFindMtNum == m_tVmpPollParam.GetPolledMtNum())
    {
        return NULL;
    }
     
    // 保证当前轮询信息的正确性
    tPollInfo.SetMtPollParam( tCurMtParam );
    TMtPollParam *ptNextMt = m_tVmpPollParam.GetPollMtByIdx(byPollPos);    
    return ptNextMt;
}

/*====================================================================
    函数名      ：ProcVmpPollingChangeTimerMsg
    功能        ：定时轮询消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    12/05/02    4.7         杨怀志         创建
====================================================================*/
void CMcuVcInst::ProcVmpPollingChangeTimerMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
    TMtPollParam * ptCurPollMt;
	
    switch(CurState())
    {
    case STATE_ONGOING:
    {
        if (POLL_STATE_NONE == m_tVmpPollParam.GetPollState()) 
        {
            break;
        }

		if ( 0 == m_tVmpPollParam.GetPolledMtNum() )
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_VMP, "[ProcVmpPollingChangeTimerMsg] PolledMtNum is 0. stop poll\n");
            ProcStopVmpPoll();
			break;
		}		
        
		// 获得画面合成中vmp单通道跟随的通道号
		TEqp tVmpEqp = g_cMcuVcApp.GetEqp(GetTheOnlyVmpIdFromVmpList());
		TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
		u8 byVmpPollChlId = tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_VMPCHLPOLL);
		if (MAXNUM_VMP_MEMBER == byVmpPollChlId)
		{
			ProcStopVmpPoll();
			ConfPrint( LOG_LVL_WARNING, MID_MCU_VMP, "[ProcVmpPollingChangeTimerMsg] There is no channel for VmpChnlPoll. stop poll\n");
			break;
		}
		
		// 暂停时,给界面刷状态,可能轮询列表有更新
		if (POLL_STATE_PAUSE == m_tVmpPollParam.GetPollState())
		{
			cServMsg.SetMsgBody( (u8*)&m_tVmpPollParam, sizeof(m_tVmpPollParam) );
			SendMsgToAllMcs( MCU_MCS_VMPPOLLSTATE_NOTIF, cServMsg );
            break;
		}

		TVmpPollInfo tPollInfo = m_tVmpPollParam.GetVmpPollInfo();
		
        // xliang [3/31/2009] 记录上次轮到的MT
		TMt tLastPolledMt = tPollInfo.GetMtPollParam().GetTMt(); 
        
        //记录轮询是否有次数限制
		BOOL32 bIsPollNumNoLimit = ( 0 == tPollInfo.GetPollNum() ) ? TRUE : FALSE; 
		u8 byPollIdx = m_tVmpPollParam.GetCurrentIdx();

		// 得到下一个符合被轮询条件的终端
		ptCurPollMt = GetNextMtPolledForVmp(byPollIdx, tPollInfo); 
		
		if (ptCurPollMt == NULL)
		{
			ProcStopVmpPoll();
			ConfPrint( LOG_LVL_WARNING, MID_MCU_VMP, "[ProcVmpPollingChangeTimerMsg] Err, Get CurPollMt is Null, LastMt:(%d, %d). stop poll\n", tLastPolledMt.GetMcuId(), tLastPolledMt.GetMtId());
			return;
		}

		ConfPrint( LOG_LVL_DETAIL, MID_MCU_VMP, "[ProcVmpPollingChangeTimerMsg] Get CurPollMt is (%d, %d), LastMt:(%d, %d).\n",
			ptCurPollMt->GetMcuId(), ptCurPollMt->GetMtId(), tLastPolledMt.GetMcuId(), tLastPolledMt.GetMtId());

		m_tVmpPollParam.SetVmpPollInfo(tPollInfo);
		m_tVmpPollParam.SetCurrentIdx(byPollIdx);
	
		// 设置Timer
		SetTimer(MCUVC_VMPPOLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());
		
		//通知所有会控
		m_tVmpPollParam.SetMtPollParam(*ptCurPollMt);
		cServMsg.SetMsgBody( (u8*)&m_tVmpPollParam, sizeof(m_tVmpPollParam) );
		SendMsgToAllMcs( MCU_MCS_VMPPOLLSTATE_NOTIF, cServMsg );

		TVMPMember *ptVmpMember = tVmpParam.GetVmpMember(byVmpPollChlId);
		// 新旧轮询终端不一致时,调整画面合成,vmp正在处理时不更新vmp成员
		if (NULL != ptVmpMember && !((TMt)*ptCurPollMt == (TMt)*ptVmpMember) && IsChangeVmpParam())
		{
			ptVmpMember->SetMt((TMt)*ptCurPollMt);
			tVmpParam.SetVmpMember(byVmpPollChlId, *ptVmpMember);
			/*TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
			tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpParam;
			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);*/ 

			// 调整画面合成
			AdjustVmpParam(tVmpEqp.GetEqpId(), &tVmpParam);
		}

        break;
    }
    default:
        break;
    }

    return;
}

/*====================================================================
    函数名      ：ProcMcsMcuVmpPollMsg
    功能        ：Vmp单通道轮询消息处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    12/05/02    4.7         杨怀志         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuVmpPollMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
	
    STATECHECK;
	
    switch(pcMsg->event)
    {
    case MCS_MCU_STARTVMPPOLL_CMD://开始Vmp单通道轮询
		{
			//只有当前未轮询,才能开始新的轮询
			if (POLL_STATE_NONE != m_tVmpPollParam.GetPollState() && !m_tVmpPollParam.GetMtPollParam().IsNull()) 
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuVmpPollMsg] Vmp channel is already in polling, currect poll mt is (%d,%d).\n",
					m_tVmpPollParam.GetMtPollParam().GetMcuId(), m_tVmpPollParam.GetMtPollParam().GetMtId());
				break;
			}

			// 获得当前唯一的画面合成器
			u8 byVmpId = GetTheOnlyVmpIdFromVmpList();
			if (byVmpId == 0)
			{
				// 未找到唯一vmp
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuVmpPollMsg] Can not find the only vmp id.\n");
				break;
			}

			//检查当前会议的画面合成子类型，目前仅支持8KH/8KI支持单通道轮询 
			u8 byVmpSubType = GetVmpSubType(byVmpId);
			if(byVmpSubType == VMP_8KI || byVmpSubType == VMP_8KH)
			{
			}
			else
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuVmpPollMsg] Vmp.%d is not 8kivmp.\n", byVmpId);
				break;
			}

			TEqp tVmpEqp = g_cMcuVcApp.GetEqp(byVmpId);

			//检查当前会议的画面合成状态 
			if( g_cMcuVcApp.GetVMPMode(tVmpEqp) == CONF_VMPMODE_NONE )
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuVmpPollMsg] Vmp is not start.\n");
				break;
			}
			
			//得到Vmp单通道轮询参数
			m_tVmpPollParam = *(TVmpPollParam *)cServMsg.GetMsgBody();       
			
			// 标记为开始轮询，设置定时器
			m_tVmpPollParam.SetPollState(POLL_STATE_NORMAL);
			m_tVmpPollParam.SpecPollPos(POLLING_POS_START);
			
			SetTimer(MCUVC_VMPPOLLING_CHANGE_TIMER, 10);
		}
        break;

    case MCS_MCU_STOPVMPPOLL_CMD://停止Vmp单通道轮询
		ProcStopVmpPoll();
        break;
		
    case MCS_MCU_PAUSEVMPPOLL_CMD://暂停Vmp单通道轮询     
		
        m_tVmpPollParam.SetPollState(POLL_STATE_PAUSE);
        cServMsg.SetMsgBody((u8 *)&m_tVmpPollParam, sizeof(m_tVmpPollParam));
        SendMsgToAllMcs(MCU_MCS_VMPPOLLSTATE_NOTIF, cServMsg);
		
        KillTimer(MCUVC_VMPPOLLING_CHANGE_TIMER);			
        break;
		
    case MCS_MCU_RESUMEVMPPOLL_CMD://继续Vmp单通道轮询   
		
        m_tVmpPollParam.SetPollState(POLL_STATE_NORMAL);
        cServMsg.SetMsgBody((u8 *)&m_tVmpPollParam, sizeof(m_tVmpPollParam));
        SendMsgToAllMcs(MCU_MCS_VMPPOLLSTATE_NOTIF, cServMsg);
		
		// 从轮询暂停处重新开始
		m_tVmpPollParam.SpecPollPos( m_tVmpPollParam.GetCurrentIdx() );
		
        SetTimer(MCUVC_VMPPOLLING_CHANGE_TIMER, 10);
        break;
		
    case MCS_MCU_GETVMPPOLLPARAM_REQ://查询Vmp单通道轮询参数 
		{
			TEqp tVmpEqp = g_cMcuVcApp.GetEqp(GetTheOnlyVmpIdFromVmpList());
			if (g_cMcuVcApp.GetVMPMode(tVmpEqp) != CONF_VMPMODE_NONE && 
				POLL_STATE_NONE != m_tVmpPollParam.GetPollState())
			{
				cServMsg.SetMsgBody((u8 *)&m_tVmpPollParam, sizeof(m_tVmpPollParam));
				SendReplyBack(cServMsg, pcMsg->event + 1);
			}
			else //画面合成未开启时,回复未轮询状态,考虑vmp掉线等待上线时的特殊情况
			{
				TVmpPollParam tTmpVmpPollParam = m_tVmpPollParam;
				tTmpVmpPollParam.SetPollState(POLL_STATE_NONE);
				tTmpVmpPollParam.SetMediaMode(MODE_NONE);
				cServMsg.SetMsgBody((u8 *)&tTmpVmpPollParam, sizeof(tTmpVmpPollParam));
				SendReplyBack(cServMsg, pcMsg->event + 1);
			}
		}
        break;

    default:
        break;
    }
}

/*====================================================================
    函数名      ：ProcPollMsg
    功能        ：轮询消息处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/13    1.0         胡昌威         创建
    03/11/27    3.0         胡昌威         修改
====================================================================*/
void CMcuVcInst::ProcMcsMcuPollMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
    TPollInfo tPollInfo;
	
    STATECHECK;
	
    switch(pcMsg->event)
    {		
    case MCS_MCU_STARTPOLL_CMD://开始轮询
		{
			//只有当前未轮询,才能开始新的轮询
			if (CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode()) 
			{
				break;
			}
			
		

			//如果开始了语音激励，则不能开始轮询
			if (m_tConf.m_tStatus.IsVACing())
			{
				/// To Notify
				LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "No poll while Vac!\n");
				break;
			}       
			
			//得到会议轮询参数
			tPollInfo = *(TPollInfo *)cServMsg.GetMsgBody();
			u8 byPollMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TPollInfo)) / sizeof(TMtPollParam);
			TMtPollParam *ptParam = (TMtPollParam*)(cServMsg.GetMsgBody() + sizeof(TPollInfo));
			m_tConfPollParam.InitPollParam( byPollMtNum, ptParam );         
			
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuPollMsg] start poll as mode.%d!\n", tPollInfo.GetMediaMode());
			
			// xsl [8/23/2006] 判断卫星会议是否停止轮询
			/*if (m_tConf.GetConfAttrb().IsSatDCastMode())
			{
				//若回传个数已满其轮询终端中有不发码流的终端则拒绝轮询
				if (IsOverSatCastChnnlNum(0))
				{
					const TMtPollParam *ptMt;
					for(u8 byIdx = 0; byIdx < m_tConfPollParam.GetPolledMtNum(); byIdx++)
					{
						ptMt = m_tConfPollParam.GetPollMtByIdx(byIdx);
						// 分散会议是不支持级联的，不用考虑
						if (m_ptMtTable->GetMtSndBitrate(ptMt->GetMtId()) == 0)
						{
							ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuPollMsg-start] over max upload mt num. nack!\n");  
							NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_DCAST_OVERCHNNLNUM);
							
							memset(&m_tConfPollParam, 0, sizeof(m_tConfPollParam));
							return;
						}
					}
				}            
			}     */
			// miaoqingsong [2011/08/26] 当会议主席为上，下级MCU时，不允许开启主席轮询选看
			if ( ( tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN ) &&
					(MT_TYPE_SMCU == m_tConf.GetChairman().GetMtType() || MT_TYPE_MMCU == m_tConf.GetChairman().GetMtType())
				)
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuPollMsg-start] SMCU can't select. so nack!\n" );  
				NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_CHAIMANPOLL_NACK);	
				return;
			}
			//zhouyiliang 20120518 当前录像机载放像的时候不允许开启会议轮询（带音频或不带音频），与mcs切发言人保持一致
			if ( ( tPollInfo.GetMediaMode() == MODE_VIDEO || tPollInfo.GetMediaMode() == MODE_BOTH ) &&
					( m_tConf.m_tStatus.IsPlaying() || m_tConf.m_tStatus.IsPlayReady() )
				)
			{
					NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_CONFPLAYING);
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuPollMsg-start] Cannot poll because it's playing!\n" );
					return;

			}
			// guzh [6/22/2007] 标记为开始轮询，设置定时器
			tPollInfo.SetPollState(POLL_STATE_NORMAL);
			m_tConfPollParam.SpecPollPos(POLLING_POS_START);        
			m_tConf.m_tStatus.SetPollInfo(tPollInfo);
			
			u8 byConfPollMode = CONF_POLLMODE_NONE;
			switch (tPollInfo.GetMediaMode())
			{
			case MODE_VIDEO:
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_VIDEO);
				break;
			case MODE_BOTH:
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_SPEAKER);
				break;
			case MODE_VIDEO_SPY:
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_VIDEO_SPY);
				break;
			case MODE_BOTH_SPY:
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_SPEAKER_SPY);
				break;
			case MODE_VIDEO_BOTH:
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_VIDEO_BOTH);
				break;
			case MODE_BOTH_BOTH:
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_SPEAKER_BOTH);
				break;
			case MODE_VIDEO_CHAIRMAN:                                         //20110411_miaoqs 主席轮询选看功能添加
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_VIDEO_CHAIRMAN);
				break;
			case MODE_BOTH_CHAIRMAN:                                          
				m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_BOTH_CHAIRMAN);
				break;
			default:
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuPollMsg] unexpected poll mode.%d rcved, ignore it!\n", tPollInfo.GetMediaMode());
				break;
			}

			// miaoqingsong [2011/09/27] 开启上传轮询之前，先清上传通道
			if ( (CONF_POLLMODE_VIDEO_SPY == m_tConf.m_tStatus.GetPollMode()) || 
				 (CONF_POLLMODE_SPEAKER_SPY == m_tConf.m_tStatus.GetPollMode()) 
				 )
			{
				// 释放回传通道带宽
				TMtStatus tMtStatus;
				m_ptMtTable->GetMtStatus(m_tCascadeMMCU.GetMtId(), &tMtStatus);
				FreeRecvSpy( tMtStatus.GetVideoMt(), MODE_BOTH );

				// 拆回传通道交换
				StopSpyMtCascaseSwitch();

				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuPollMsg] First FreeRecvSpy and topSpyMtCascaseSwitch before start spy's poll!\n");
			}

			// miaoqingsong [2011/08/10] 设置轮询定时器之前，轮询模式为主席轮询模式，先停主席原有的选看
			if ( (CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()) ||
				(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode())
				)
			{
				TMtStatus tMtStatus;
				m_ptMtTable->GetMtStatus(m_tConf.GetChairman().GetMtId(), &tMtStatus);
				
				if (!tMtStatus.GetSelectMt(MODE_VIDEO).IsNull())
				{
					StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO, FALSE, FALSE);
				} 
				
				if(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode() && !tMtStatus.GetSelectMt(MODE_AUDIO).IsNull())
				{
					StopSelectSrc(m_tConf.GetChairman(), MODE_AUDIO, FALSE, FALSE);
				}

			
			}

			if (CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode())
			{
				ConfModeChange();
				
				SetTimer(MCUVC_POLLING_CHANGE_TIMER, 10);
			}
			break;
		}
		
    case MCS_MCU_STOPPOLL_CMD://停止轮询
		ProcStopConfPoll();
        break;
		
    case MCS_MCU_PAUSEPOLL_CMD://暂停轮询     
		
        m_tConf.m_tStatus.SetPollState(POLL_STATE_PAUSE);
        cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
        SendMsgToAllMcs(MCU_MCS_POLLSTATE_NOTIF, cServMsg);
		
        ConfStatusChange();
		
        KillTimer(MCUVC_POLLING_CHANGE_TIMER);			
        break;
		
    case MCS_MCU_RESUMEPOLL_CMD://继续轮询   
		
        m_tConf.m_tStatus.SetPollState(POLL_STATE_NORMAL);
        cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
        SendMsgToAllMcs(MCU_MCS_POLLSTATE_NOTIF, cServMsg);
		
        ConfStatusChange();
		
		// 从轮询暂停处重新开始, zgc, 20070622
		m_tConfPollParam.SpecPollPos( m_tConfPollParam.GetCurrentIdx() );
		
        SetTimer(MCUVC_POLLING_CHANGE_TIMER, 10);
        break;
		
    case MCS_MCU_GETPOLLPARAM_REQ://查询轮询参数 
		{
			/*if(m_tConfPollParam.GetLastPolledMt().IsNull())
			{
				//当前轮询没开启，或者当前轮询开启了，但是轮询的参数没设置成功（多回传情况，轮下级，带宽不足，不会设置lastpoll）
				SendReplyBack(cServMsg, pcMsg->event + 2);	
			}
			else*/
			//zjj20130709 没有轮询终端也要回ack,万一下级终端都是没开通道但是有视频源的，那回nack，界面就无法显示暂停和停止了(特别达容量测试时)
			{
				cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
				cServMsg.CatMsgBody((u8 *)m_tConfPollParam.GetPollMtByIdx(0),
				m_tConfPollParam.GetPolledMtNum()*sizeof(TMtPollParam ));
				SendReplyBack(cServMsg, pcMsg->event + 1);	
			}
					
			break;
		}

		
		// zbq [03/13/2007] 更新轮询列表
    case MCS_MCU_CHANGEPOLLPARAM_CMD:
        {
            //得到新的会议轮询参数
            TPollInfo tTmpPollInfo;
            tTmpPollInfo = *(TPollInfo*)cServMsg.GetMsgBody();
			
            TConfPollParam tTmpPollParam;
            u8 byPollMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TPollInfo)) / sizeof(TMtPollParam);
            TMtPollParam *ptParam = (TMtPollParam*)(cServMsg.GetMsgBody() + sizeof(TPollInfo));
            tTmpPollParam.SetPollList(byPollMtNum, ptParam);
            
            // 判断当前轮询终端是否在新列表中
            TMtPollParam tPollingMt = m_tConf.m_tStatus.GetMtPollParam();
            u8 byIdx = POLLING_POS_INVALID;
			//增加判断是否为空, zgc, 2007-06-21
            if ( !tPollingMt.IsNull() && !tTmpPollParam.IsExistMt(tPollingMt, byIdx) )
            {
                // notify NACK
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_DELCURPOLLMT_FAIL);
                break;
            }
			
            // 保存新列表
            m_tConfPollParam.SetPollList(byPollMtNum, ptParam);
			
			// 设置当前轮询终端在新轮询列表中的位置, zgc, 2007-05-24
			m_tConfPollParam.SetCurrentIdx( byIdx );
			
            //刷新其他MCS轮询终端列表
            PollParamChangeNotify();
        }
        break;
		//指定轮询位置
	case MCS_MCU_SPECPOLLPOS_REQ:
		{
			if( POLL_STATE_NONE == m_tConf.m_tStatus.GetPollInfo()->GetPollState() )
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcsMcuPollMsg] <MCS_MCU_SPECPOLLPOS_REQ> Conf not in polling!\n");
				cServMsg.SetErrorCode( ERR_MCU_SPECPOLLPOS_CONFNOTPOLL );
				SendReplyBack(cServMsg, pcMsg->event + 2);
				break;
			}
			TMt tPollMt = *(TMt *)cServMsg.GetMsgBody();
            u8 byMtIdx;
            if ( !m_tConfPollParam.IsExistMt(tPollMt, byMtIdx) )
            {
                cServMsg.SetErrorCode( ERR_MCU_SPECPOLLPOS_MTNOTINCONF );
                SendReplyBack(cServMsg, pcMsg->event + 2);
                break;
            }
			
            SendReplyBack(cServMsg, pcMsg->event + 1);
			
            // 指定轮询位置            
            if (POLL_STATE_PAUSE == m_tConf.m_tStatus.GetPollState())
            {
                TPollInfo tCurPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
                TMtPollParam tCurMtParam = *(m_tConfPollParam.GetPollMtByIdx(byMtIdx));
                tCurPollInfo.SetMtPollParam( tCurMtParam );
                m_tConf.m_tStatus.SetPollInfo(tCurPollInfo);
                m_tConfPollParam.SetCurrentIdx(byMtIdx);
				
                ConfStatusChange();
            }
            else //POLL_STATE_NORMAL
            {
                m_tConfPollParam.SpecPollPos(byMtIdx);
				SetTimer( MCUVC_POLLING_CHANGE_TIMER, 10 );
			}
		}
		break;
    default:
        break;
    }
}

/*====================================================================
    函数名      ：PollParamChangeNotify
    功能        ：轮询终端列表改变 通知所有的会控 刷新
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/03/21    4.0         张宝卿         创建
====================================================================*/
void CMcuVcInst::PollParamChangeNotify( void )
{
    CServMsg cServMsg;
    cServMsg.SetEventId( MCU_MCS_POLLPARAMCHANGE_NTF );
    cServMsg.SetMsgBody( (u8*)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo) );
    cServMsg.CatMsgBody( (u8*)m_tConfPollParam.GetPollMtByIdx(0), m_tConfPollParam.GetPolledMtNum() * sizeof(TMtPollParam) );
    SendMsgToAllMcs( MCU_MCS_POLLPARAMCHANGE_NTF, cServMsg );

    return;
}

/*====================================================================
    函数名      ：ProcStopConfPoll
    功能        ：停止会议轮询
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/06/22    4.0         顾振华         创建
====================================================================*/
void CMcuVcInst::ProcStopConfPoll(BOOL32 bNeedAdjustVmp)
{
    u8 byPrvPollMode = m_tConf.m_tStatus.GetPollMode();
    u8 byPollMeida = m_tConf.m_tStatus.GetPollMedia();

    KillTimer(MCUVC_POLLING_CHANGE_TIMER);       
    
    //取消轮询，则取消轮询交换及相应复合状态
    //CancelOneVmpFollowSwitch( VMP_MEMBERTYPE_POLL, TRUE );
    if (bNeedAdjustVmp && GetVmpCountInVmpList() > 0)
    {
        ChangeVmpChannelParam(NULL, VMP_MEMBERTYPE_POLL);
    }
    
    if (CONF_VMPTWMODE_NONE != m_tConf.m_tStatus.GetVmpTwMode())
    {
        ChangeVmpTwChannelParam(NULL, VMPTW_MEMBERTYPE_POLL);
    }
	
    TMt tSpyMt;
    tSpyMt.SetNull();
	tSpyMt = m_tConfPollParam.GetLastPolledMt();

	TMt tLocalPollMt = GetLocalMtFromOtherMcuMt( tSpyMt );

	// miaoqingsong [20110602] 增加条件判断：画面合成过程中，停止主席轮询，主席终端应该接收画面合成视频码流
	// Bug00055059 画面合成过程中，停止主席轮询，主席终端仍然选看最后一个终端（应该接收画面合成）
    if ( !g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) ||
		( MODE_VIDEO_CHAIRMAN == byPollMeida || MODE_BOTH_CHAIRMAN == byPollMeida ))		
    {
		CRecvSpy tSpyResource;
		u16 wSpyStartPort = SPY_CHANNL_NULL;
		if( m_cSMcuSpyMana.GetRecvSpy( tSpyMt, tSpyResource ) )
		{
			wSpyStartPort = tSpyResource.m_tSpyAddr.GetPort();
		}
		
		TMt tMt;
        tMt.SetNull();

        switch (byPollMeida)
        {
        case MODE_VIDEO:
            
            //拆本地视频广播
            //ChangeVidBrdSrc(NULL);					

            //恢复本地视频广播
            if(HasJoinedSpeaker() && 
				( !IsMultiCastMt(GetLocalSpeaker().GetMtId()) || IsMultiCastMt(tLocalPollMt.GetMtId())
					|| GetCurSatMtNum() > 0)
				)
            {
                tMt = m_tConf.GetSpeaker();
                ChangeVidBrdSrc(&tMt);
            }
            else
            {		
                ChangeVidBrdSrc(NULL);
            }

			if( SPY_CHANNL_NULL != wSpyStartPort )
			{
				FreeRecvSpy( tSpyMt, MODE_VIDEO );	
			}
            break;
            
        case MODE_BOTH:
            
            //拆本地发言
            tMt = m_tConf.GetSpeaker();
			if (!IsMultiCastMt(GetLocalSpeaker().GetMtId()) || 
				IsMultiCastMt(tLocalPollMt.GetMtId()) || GetCurSatMtNum() > 0)
			{
				ChangeVidBrdSrc(&tMt);
			}

            break;
            
        case MODE_VIDEO_SPY:
        case MODE_BOTH_SPY:
			{
				// 清上传源(以免在停止上传时还占用回传带宽)
				u8 byFreeMode = MODE_NONE;
				if( MODE_VIDEO_SPY == byPollMeida )
				{
					byFreeMode = MODE_VIDEO;
				}
				else
				{
					byFreeMode = MODE_BOTH;
				}

				if( SPY_CHANNL_NULL != wSpyStartPort )
				{
					FreeRecvSpy( tSpyMt, byFreeMode );
				}
				
				StopSpyMtCascaseSwitch();
			}
            break;
            
        case MODE_VIDEO_BOTH:
            {
				//拆本地视频广播
				//ChangeVidBrdSrc(NULL);
				
				//恢复本地视频广播
				if(HasJoinedSpeaker() && (!IsMultiCastMt(GetLocalSpeaker().GetMtId()) || 
						IsMultiCastMt(tLocalPollMt.GetMtId()) || GetCurSatMtNum() > 0))
				{
					tMt = m_tConf.GetSpeaker();
					ChangeVidBrdSrc(&tMt);
				}
				else
				{
					ChangeVidBrdSrc(NULL);
				}
				
				// 清上传源，尝试释放回传带宽
				u8 byFreeMode = MODE_NONE;
				if( MODE_VIDEO_SPY == byPollMeida )
				{
					byFreeMode = MODE_VIDEO;
				}
				else
				{
					byFreeMode = MODE_BOTH;
				}
				
				if( SPY_CHANNL_NULL != wSpyStartPort )
				{
					FreeRecvSpy( tSpyMt, byFreeMode );
				}
				StopSpyMtCascaseSwitch();
			}
            break;
            
        case MODE_BOTH_BOTH:
            {
				//拆本地发言
				tMt = m_tConf.GetSpeaker();
				if (!IsMultiCastMt(GetLocalSpeaker().GetMtId()) || 
						IsMultiCastMt(tLocalPollMt.GetMtId()) || GetCurSatMtNum() > 0)
				{
					ChangeVidBrdSrc(&tMt);
				}

				// 清上传源,此时不需要释放回传带宽（带音频的会议轮询停止时，不取消发言人）
				StopSpyMtCascaseSwitch();
			}
            break;

		case MODE_VIDEO_CHAIRMAN:

			//停止主席选看
			StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO);           // 20110414_miaoqs 主席轮询选看功能添加
			break;

		case MODE_BOTH_CHAIRMAN:

			//停止主席选看
            StopSelectSrc(m_tConf.GetChairman(), MODE_BOTH);
			break;
            
        default:
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuPollMsg] unexpected poll mode.%d rcved, ignore it!\n", byPollMeida);
            break;
        }
    }
	else
	{
		//如果是带音频轮询就不用释放视频，因为此时轮询到的终端是发言人，不取消
		if( m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_POLL) &&
			MODE_BOTH != byPollMeida
			)
		{
			FreeRecvSpy( tSpyMt, MODE_VIDEO );
		}
		else
		{
			if( MODE_VIDEO == byPollMeida )
			{
				FreeRecvSpy( tSpyMt, MODE_VIDEO );
			}
		}
		
	}

	// miaoqingsong [06/29/2011] 停轮询清轮询状态和会议状态
	m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_NONE);
    m_tConf.m_tStatus.SetPollState(POLL_STATE_NONE);
	m_tConf.m_tStatus.SetPollMedia(MODE_NONE);

	ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcStopConfPoll]stop poll,clear pollstate in confstatus!\n");
	
	//zhouyiliang 20110518 停止轮询的时候清掉当前轮询终端。
	TPollInfo* ptPollInfo = m_tConf.m_tStatus.GetPollInfo();
	if (NULL == ptPollInfo)
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcStopConfPoll]GetPollInfo pointer is null\n");
		return;
	}
	TMtPollParam tMtPollParam = ptPollInfo->GetMtPollParam();
	TMt tNullMt;
	tNullMt.SetNull();
	tMtPollParam.SetTMt(tNullMt);
	ptPollInfo->SetMtPollParam(tMtPollParam);
	m_tConf.m_tStatus.SetPollInfo(*ptPollInfo);

	CServMsg cServMsg;
    cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
    SendMsgToAllMcs(MCU_MCS_POLLSTATE_NOTIF, cServMsg);
        
    //stop TvWall
    TPeriEqpStatus tTWStatus, tHduStatus;
    u8 byChnlIdx;
    u8 byEqpId;
    
    for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
    {
        if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
        {
            for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
            {
                if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus) &&
                    TW_MEMBERTYPE_POLL == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType &&
                    m_byConfIdx == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx())
                {
                    ChangeTvWallSwitch(NULL, byEqpId, byChnlIdx, TW_MEMBERTYPE_POLL, TW_STATE_STOP);
                }
            }
        }
    }
    // stop HDU   
    for (byEqpId = HDUID_MIN; byEqpId <= HDUID_MAX; byEqpId++)
    {
		if(IsValidHduEqp(g_cMcuVcApp.GetEqp(byEqpId)))
        {
			u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byEqpId));
			if (0 == byHduChnNum)
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[ProcStopConfPoll] GetHduChnNumAcd2Eqp failed!\n");
				continue;
			}

            for (byChnlIdx = 0; byChnlIdx < byHduChnNum; byChnlIdx++)
            {
                if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tHduStatus) &&
                    TW_MEMBERTYPE_POLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType &&
                    m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx())
                {
					// [2013/03/11 chenbing] HDU多画面目前不支持轮询跟随,子通道置0
                    ChangeHduSwitch(NULL, byEqpId, byChnlIdx, 0, TW_MEMBERTYPE_POLL, TW_STATE_STOP, FALSE);
                }
            }
        }
    }

	
    ConfStatusChange();

    return;
}

/*====================================================================
    函数名      ：ProcPollingChangeTimerMsg
    功能        ：定时轮询消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/13    1.0         胡昌威         创建
    03/11/28    3.0         胡昌威         修改
====================================================================*/
void CMcuVcInst::ProcPollingChangeTimerMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
    TMtPollParam * ptCurPollMt;
	
    switch(CurState())
    {
    case STATE_ONGOING:
    {
        if (CONF_POLLMODE_NONE == m_tConf.m_tStatus.GetPollMode() || 
			m_tConf.m_tStatus.GetPollState() == POLL_STATE_PAUSE ) 
        {
            break;
        }

		if ( 0 == m_tConfPollParam.GetPolledMtNum() )
		{
            ProcStopConfPoll();
			break;
		}		
        
		TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
		
        // xliang [3/31/2009] 记录上次轮到的MT
		TMt tLastPolledMt = tPollInfo.GetMtPollParam().GetTMt(); 
        
        //记录轮询是否有次数限制, zgc, 2007-06-20
		BOOL32 bIsPollNumNoLimit = ( 0 == tPollInfo.GetPollNum() ) ? TRUE : FALSE; 
		u8 byPollIdx = m_tConfPollParam.GetCurrentIdx();

		// 20110413_miaoqingsong 得到下一个符合被轮询条件的终端
		ptCurPollMt = GetNextMtPolled(byPollIdx, tPollInfo); 
		
		if (ptCurPollMt == NULL)
		{
			ProcStopConfPoll();
	
			//第一次开启，无可轮询终端，上报提示[8/5/2013 chendaiwei]
			if(tLastPolledMt.IsNull())
			{
				NotifyMcsAlarmInfo( 0, ERR_MCU_POLLING_NOMTORVIDSRC );
			}

			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Err ProcPollingChangeTimerMsg Get CurPollMt is Null, LastMt:(%d, %d). stop poll\n", tLastPolledMt.GetMcuId(), tLastPolledMt.GetMtId());
			return;
		}

		m_tConf.m_tStatus.SetPollInfo(tPollInfo);
		m_tConfPollParam.SetCurrentIdx(byPollIdx);
	
		// xsl [8/23/2006] 判断卫星会议是否停止轮询
		/*if (m_tConf.GetConfAttrb().IsSatDCastMode())
		{
			BOOL32 bPollWitchAudio = (CONF_POLLMODE_SPEAKER == m_tConf.m_tStatus.GetPollMode());
			if (IsMtSendingVidToOthers(tLastPolledMt, bPollWitchAudio, FALSE, 0) && //上一个终端还在向其他实体发送码流
				m_ptMtTable->GetMtSndBitrate(ptCurPollMt->GetMtId()) == 0 && //下一个终端没有在发送码流
				IsOverSatCastChnnlNum(0))
			{
                ProcStopConfPoll();
			    break;
			}
		}*/

		
		//lukunpeng 2010/06/30 把Timer放到前面来做，防止多回传失败后，不能继续进行
		SetTimer(MCUVC_POLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());
		
		// zbq [03/09/2007] 通知下一个即将被轮询到的终端
		//NotifyMtToBePolledNext();

		if (NULL != ptCurPollMt)
		{
			   
			m_tConf.m_tStatus.SetMtPollParam( *ptCurPollMt );
			//zjj20121124 放在后面做通知所有会控  
			//cServMsg.SetMsgBody( (u8*)&tPollInfo, sizeof(TPollInfo) );
			//SendMsgToAllMcs( MCU_MCS_POLLSTATE_NOTIF, cServMsg );
		}
		
		//由此处统一建立会议轮询的回传
		TMt tSpySourceMt = ptCurPollMt->GetTMt();
		if( !tSpySourceMt.IsLocal() &&
			IsLocalAndSMcuSupMultSpy(tSpySourceMt.GetMcuId()) 
			)
		{
			TPreSetInReq tSpySrcInitInfo;
			tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt( tSpySourceMt );

			// [20111129_miaoqingsong] 为兼容支持多回传的低版本，级联多回传轮询模式转换移至上级进行
			if ( (MODE_VIDEO == tPollInfo.GetMediaMode()) || 
				 (MODE_VIDEO_SPY == tPollInfo.GetMediaMode()) || 
				 (MODE_VIDEO_BOTH == tPollInfo.GetMediaMode()) || 
				 (MODE_VIDEO_CHAIRMAN == tPollInfo.GetMediaMode()) 
				)
			{
				tSpySrcInitInfo.m_bySpyMode = MODE_VIDEO;
			} 
			else
			{
				tSpySrcInitInfo.m_bySpyMode = MODE_BOTH;
			}
			//tSpySrcInitInfo.m_bySpyMode = tPollInfo.GetMediaMode();
			tSpySrcInitInfo.SetEvId(MCUVC_POLLING_CHANGE_TIMER);
			tSpySrcInitInfo.m_tSpyInfo.m_tSpyPollInfo.m_wKeepTime = ptCurPollMt->GetKeepTime();
			tSpySrcInitInfo.m_tSpyInfo.m_tSpyPollInfo.m_byPollingPos = m_tConfPollParam.GetCurrentIdx();
			
			// [pengjie 2010/9/13] 填目的端能力
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSpySourceMt.GetMcuId()) );	
			//zjl20101116 如果当前终端已回传则能力集要与已回传目的能力集取小
			if(!GetMinSpyDstCapSet(tSpySourceMt, tSimCapSet))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcPollingChangeTimerMsg] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
					tSpySourceMt.GetMcuId(), tSpySourceMt.GetMtId() );
				return;
			}
			
			tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		    // End

			TMt tLastMt = m_tConfPollParam.GetLastPolledMt();
			if( !tLastMt.IsNull() )
			{
				if( m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO ||
					m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_SPY ||
					m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_BOTH ||
					m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_CHAIRMAN
					)
				{
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_VIDEO;
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
				}
				else
				{
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_BOTH;
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
					//zhouyiliang 20120824 带音频的本地轮询和带音频的本地上传轮询看当前的发言人是不是lastmt，不是的话不计数(可能轮的过程中取消了发言人)
					//fixme:CONF_POLLMODE_SPEAKER_SPY模式没考虑
					if ( CONF_POLLMODE_SPEAKER == m_tConf.m_tStatus.GetPollMode() || 
						CONF_POLLMODE_SPEAKER_BOTH == m_tConf.m_tStatus.GetPollMode())
					{
						if ( !(m_tConf.GetSpeaker() == tLastMt) )
						{
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum--;
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum--;

						}
					}
										
				}
				
				tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tLastMt;
				tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
				//tSpySrcInitInfo.m_tReleaseMtInfo.m_tCanReleaseMtInfo = BuildMultiCascadeMtInfo( tSpySrcInitInfo.m_tReleaseMtInfo.m_tCanReleaseMtInfo.m_tMt );
				
				// vmp 只对调整vmp跟随通道的模式进行可释放计数统计
				u8 byVmpCount = GetVmpCountInVmpList();
				if( /*g_cMcuVcApp.GetVMPMode(m_tVmpEqp) != CONF_VMPMODE_NONE*/byVmpCount > 0 && 
					(MODE_VIDEO == tPollInfo.GetMediaMode() || MODE_BOTH == tPollInfo.GetMediaMode()
					|| MODE_VIDEO_BOTH == tPollInfo.GetMediaMode() || MODE_BOTH_BOTH == tPollInfo.GetMediaMode())
					)
				{
					u8 bySpeakerPollNum = 0;
					bySpeakerPollNum += GetVmpChnnlNumBySpecMemberType( VMP_MEMBERTYPE_POLL, &tLastMt);
					
					// 2011-11-8 add by pgf: 过滤带音频的模式,　MODE_BOTH_SPY　策略上已经过滤掉了
					if ( tPollInfo.GetMediaMode() == MODE_BOTH ||
						tPollInfo.GetMediaMode() == MODE_BOTH_BOTH )
					{
						bySpeakerPollNum += GetVmpChnnlNumBySpecMemberType(VMP_MEMBERTYPE_SPEAKER, &tLastMt);
					}
					
					if ( bySpeakerPollNum > 0)
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "VMP has %d chnnl of VMP_MEMBERTYPE_SPEAKER and VMP_MEMBERTYPE_POLL\n", bySpeakerPollNum);
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum += bySpeakerPollNum;
					}
// 					TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
// 					u8 byChnIdx = tVmpParam.GetChlOfMtInMember(m_tConfPollParam.GetLastPolledMt());
// 					if( byChnIdx != MAXNUM_VMP_MEMBER )
// 					{
// 						if( VMP_MEMBERTYPE_MCSSPEC == tVmpParam.GetVmpMember(byChnIdx)->GetMemberType()  
// 							|| VMP_MEMBERTYPE_VAC == tVmpParam.GetVmpMember(byChnIdx)->GetMemberType() )
// 						{
// 							
// 						}
// 						else if( VMP_MEMBERTYPE_POLL == tVmpParam.GetVmpMember(byChnIdx)->GetMemberType()  )
// 						{
// 							++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
// 						}
// 						else
// 						{
// 							if( ( tPollInfo.GetMediaMode() == MODE_BOTH || 
// 								tPollInfo.GetMediaMode() == MODE_BOTH_BOTH	)
// 								&& VMP_MEMBERTYPE_SPEAKER == tVmpParam.GetVmpMember(byChnIdx)->GetMemberType() )
// 							{			
// 								//如果发言人由于发言人跟随自动进入了画面合成,可释放视频目的数要加1
// 								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
// 							}
// 						}
// 					}
// 					else
// 					{
// 						//exception
// 						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcPollingChangeTimerMsg] GetChlOfMtInMember error for (%d, %d)\n",
// 							m_tConfPollParam.GetLastPolledMt().GetMcuId(),m_tConfPollParam.GetLastPolledMt().GetMtId() );
// 						
// 					}
				}
				

				TPeriEqpStatus tStatus;
				u8 byChnlIdx;
				u8 byEqpId;
				u8 byTvWallMode = 0;
				//如果发言人由于发言人跟随自动进入了标清电视墙,可释放音视频目的数要加1
				for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
				{
					if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
					{
						if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus))
						{						
							for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
							{							
								if (tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx() == m_byConfIdx &&
									tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetMcuId() == m_tConfPollParam.GetLastPolledMt().GetMcuId() &&
									tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetMtId() == m_tConfPollParam.GetLastPolledMt().GetMtId()
									)
								{
									if( TW_MEMBERTYPE_SPEAKER == tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType &&
										( tPollInfo.GetMediaMode() == MODE_BOTH || 
												tPollInfo.GetMediaMode() == MODE_BOTH_BOTH )
										)
									{
										++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
										++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
									}

									if( TW_MEMBERTYPE_POLL == tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType) 
									{
										byTvWallMode = g_cMcuVcApp.GetTvWallOutputMode(byEqpId);
										if (TW_OUTPUTMODE_AUDIO == byTvWallMode)
										{
											++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
										}
										else if (TW_OUTPUTMODE_VIDEO == byTvWallMode)
										{
											++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
										}
										else if (TW_OUTPUTMODE_BOTH == byTvWallMode)
										{
											++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
											++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
										}
									}	
								}								
							}
						}
					}
				}

				//发言人跟随时，同步调整HDU中的图像
				u8 byHduChnNum = 0;
				//如果发言人由于发言人跟随自动进入了标清电视墙,可释放视音频目的数要加1
				for (byEqpId = HDUID_MIN; byEqpId <= HDUID_MAX; byEqpId++)
				{
					if(IsValidHduEqp(g_cMcuVcApp.GetEqp(byEqpId)))
					{
						byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byEqpId));
						if (0 == byHduChnNum)
						{						
							continue;
						}

						if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus))
						{						
							for (byChnlIdx = 0; byChnlIdx < byHduChnNum; byChnlIdx++)
							{							
								if ( m_byConfIdx == tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx() &&
									tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetMcuId() == m_tConfPollParam.GetLastPolledMt().GetMcuId() &&
									tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetMtId() == m_tConfPollParam.GetLastPolledMt().GetMtId()
									)
								{
									if( TW_MEMBERTYPE_SPEAKER == tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType &&
										( tPollInfo.GetMediaMode() == MODE_BOTH || 
												tPollInfo.GetMediaMode() == MODE_BOTH_BOTH )
										)
									{
										++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
										++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
									}
									if( TW_MEMBERTYPE_POLL == tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType )
									{
										if( m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO ||
											m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_SPY ||
											m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_BOTH 
											)
										{
											++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
										}
										else
										{
											++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
											++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
										}
									}
								}
							}
						}
					}
				}				
			}

			

			if ( !OnMMcuPreSetIn( tSpySrcInitInfo ) && 
				NULL != ptCurPollMt )
			{
				cServMsg.SetMsgBody( (u8*)&tPollInfo, sizeof(TPollInfo) );
				SendMsgToAllMcs( MCU_MCS_POLLSTATE_NOTIF, cServMsg );
			}
			

			return;
		}
			// End Modify


		// zbq [03/09/2007] 通知下一个即将被轮询到的终端
		NotifyMtToBePolledNext();		
        
		//TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
		u8 byVmpCount = GetVmpCountInVmpList();
		if (NULL != ptCurPollMt)
		{
			cServMsg.SetMsgBody( (u8*)&tPollInfo, sizeof(TPollInfo) );
			SendMsgToAllMcs( MCU_MCS_POLLSTATE_NOTIF, cServMsg );

			// 添加此标记，表示要不要发言人再作一次VMP的参数调整
			BOOL32 bVmpNeedChgBySpeaker = TRUE;
// 			BOOL32 bNoneKeda = FALSE;
// 			if (!ptCurPollMt->IsNull())
// 			{
// 				bNoneKeda = (!IsKedaMt(*ptCurPollMt, TRUE)) || (!IsKedaMt(*ptCurPollMt, FALSE));
// 			}

			//通知所有会控
			TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);
			//变更画面复合的相应轮询更随的交换
			// xliang [3/31/2009] 重整逻辑
			if (byVmpCount > 0)
			{
				u8 byVmpPollCount = GetVmpChnnlNumBySpecMemberType( VMP_MEMBERTYPE_POLL);//轮询跟随通道数
				/* xliang [4/21/2009] 主席在选看VMP失效
				if( tVmpParam.IsVMPSeeByChairman() )
				{
					m_tConf.m_tStatus.SetVmpSeebyChairman(FALSE);
					
                    // 状态同步刷新到TPeriStatus中
					TPeriEqpStatus tVmpStatus;
					g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
					tVmpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
					g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
					
					// xliang [4/14/2009] 主席选看VMP的交换停掉
					StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO);
				}*/
				
				// [11/19/2009 xliang] 级联轮询+VMP中有下级成员，则跳过所有轮询到的下级成员,即不轮询之？
				/*
				if( tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
					&& tVmpParam.HasUnLocalMemberInVmp()
					&& !ptCurPollMt->IsLocal()
					)
				{
					//FIXME
					ConfLog(FALSE,"should not poll mt(%d,%d)!\n", ptCurPollMt->GetMcuId(), ptCurPollMt->GetMtId());
					SetTimer(MCUVC_POLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());
					// zbq [03/09/2007] 通知下一个即将被轮询到的终端
					NoifyMtToBePolledNext();
					break;
				}
				*/

				// xliang [4/2/2009] 带音频的会议轮询,VMP轮询跟随，VMP发言人跟随3者不能共存
				// MPU2支持轮询跟随和发言人跟随同时存在.
				// 发言人跟随通道取消，上报MCS错误提示
				// 带音频的上传+本地轮询,同样与VMP轮询跟随，VMP发言人跟随3者不能共存
				/*u8 byVmpSubType = GetVmpSubType(m_tVmpEqp.GetEqpId());
				if( (tPollInfo.GetMediaMode() == MODE_BOTH || tPollInfo.GetMediaMode() == MODE_BOTH_BOTH)
					&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) //带音频的会议轮询=轮流变发言人
					&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)  
					&& !IsAllowVmpMemRepeated(m_tVmpEqp.GetEqpId())  //  [2/17/2012 pengguofeng]新需求：8000A、8000H均通过配置文件决定
					)	
				{
					NotifyMcsAlarmInfo( 0, ERR_AUDIOPOLL_CONFLICTVMPFOLLOW );
					
					//调整VMP param
					u8 byLoop;
					TVMPMember tVmpMember;
					for(byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++)
					{
						tVmpMember = *tVmpParam.GetVmpMember(byLoop);
						if(tVmpMember.GetMemberType() == VMP_MEMBERTYPE_SPEAKER)
						{
							ClearOneVmpMember(m_tVmpEqp.GetEqpId(), byLoop, tVmpParam);
							break;
						}
					}
				}*/

				// [miaoqingsong 20111205] 增加判断条件：主席轮询/上传轮询到的MT均不进VMP视频轮询跟随通道
				if( byVmpPollCount > 0 && 
					!(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
					!(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY)
					//|| (tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) //带音频的会议轮询进发言人跟随通道
					//&& tPollInfo.GetMediaMode() == MODE_BOTH ) 
					)
				{
					bVmpNeedChgBySpeaker = FALSE;
					// xliang [3/19/2009] 对于MPU，若轮询到的MT已经在其他通道中且该通道是发言人跟随或者该Mt是发言人
					// 则不进VMP中的轮询跟随通道  ----废除 xliang [4/2/2009] 
					// xliang [4/2/2009] 对于新VMP，若轮询到的MT已经在其他通道中均不进轮询跟随通道

					// [2/5/2010 xliang] 重整
					//u8 byMtVmpChnl = tVmpParam.GetChlOfMtInMember(*(TMt*)ptCurPollMt /*tLocalMt*/);
					/*if( (!IsAllowVmpMemRepeated(m_tVmpEqp.GetEqpId())) 
						&& tVmpParam.IsMtInMember(*(TMt*)ptCurPollMt)
						)
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcPollingChangeTimerMsg] Mt.(%u,%u) has already in certain VMP channel!\n",
							ptCurPollMt->GetMcuId(), ptCurPollMt->GetMtId() );
					}
					else
					{*/
						/*目前统一在轮询处理时释放vmp轮询跟随通道多回传资源
						//轮询跟随可能在多个vmp通道内
						for (u8 byChlNum=0; byChlNum<tVmpParam.GetMaxMemberNum(); byChlNum++)
						{
							TVMPMember tOldMember = *tVmpParam.GetVmpMember(byChlNum);
							if (!tOldMember.IsNull() && !tOldMember.IsLocal() && !(tOldMember == *ptCurPollMt))
							{
								// 释放轮询跟随多回传资源
								if (VMP_MEMBERTYPE_POLL == tOldMember.GetMemberType())
								{
									/* 非科达终端,仅保留1路轮询跟随通道(规格更新,不能设多路跟随)
									if (bNoneKeda && byChlNum != tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_POLL))
									{
										ClearOneVmpMember(byChlNum, tVmpParam);
										continue;
									}*
									FreeRecvSpy( tOldMember, MODE_VIDEO );
								}
								// 如果是带音频的轮询,同样在此处释放发言人跟随多回传资源
								else if (VMP_MEMBERTYPE_SPEAKER == tOldMember.GetMemberType())
								{
									// 带音频的本地轮询,带音频的回传轮询+本地轮询
									if (tPollInfo.GetMediaMode() == MODE_BOTH ||
										tPollInfo.GetMediaMode() == MODE_BOTH_BOTH)
									{
										/* 非科达终端,仅保留1路轮询跟随通道
										if (bNoneKeda && byChlNum != tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_SPEAKER))
										{
											ClearOneVmpMember(byChlNum, tVmpParam);
											continue;
										}*
										FreeRecvSpy( tOldMember, MODE_VIDEO );
									}
								}
							}
						}*/
						//u8 byVmpMemType = VMP_MEMBERTYPE_POLL;
						/*if( tPollInfo.GetMediaMode() == MODE_BOTH )	// xliang [3/31/2009] 带音频的会议轮询--->发言人跟随通道
						{
							byVmpMemType = VMP_MEMBERTYPE_SPEAKER;
						}*/
						//ChangeVmpChannelParam(ptCurPollMt/*&tLocalMt*/, byVmpMemType,&tLastPolledMt);
						/*bVmpNeedChgBySpeaker = FALSE;
					}*/
				}
			}

			if ( (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE) && 
				 !(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
				 !(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY) 
				 )
			{
				ChangeVmpTwChannelParam(ptCurPollMt, VMPTW_MEMBERTYPE_POLL);
			}

			
			//  [12/10/2009 pengjie] Modify 级联多回传支持 若是下级终端且支持多回传，则发presetin消息

			//lukunpeng 2010/06/07 由此处统一释放老发言人的回传
			/*
			TMt  tOldSpeaker = m_tConf.GetSpeaker();
			if(!tOldSpeaker.IsNull()
				&& !tOldSpeaker.IsLocal()
				&& IsLocalAndSMcuSupMultSpy(tOldSpeaker.GetMcuId())
				&& (tPollInfo.GetMediaMode() == MODE_BOTH || tPollInfo.GetMediaMode() ==MODE_BOTH_BOTH))
			{
				FreeRecvSpy( tOldSpeaker, MODE_BOTH);
				StopSpeakerFollowSwitch( MODE_BOTH );
			}
			*/

			// zbq [09/06/2007] 级联轮询不带音频也SetIn,是否轮询到在GetParam里过滤
            // guzh [6/14/2007] 下级终端要SetIn
            if ( !ptCurPollMt->IsLocal() )
            {
                OnMMcuSetIn(*(TMt*)ptCurPollMt, 0, SWITCH_MODE_BROADCAST);
            }
			
            //整理给出不同模式下的交换
			TMt tLastMt = m_tConfPollParam.GetLastPolledMt();
			u8 bySrcChnnl = (tLastMt == m_tPlayEqp ? m_byPlayChnnl : 0);
			TMt tFeedBackMt;
            tFeedBackMt.SetNull();

			BOOL32 bIsVmpBrd = (GetVidBrdSrc().GetEqpType() == EQP_TYPE_VMP);
			//tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
            switch ( tPollInfo.GetMediaMode() )
            {
            case MODE_VIDEO:
				{
					FreeRecvSpy( tLastMt, MODE_VIDEO );	
					if (!bIsVmpBrd)
					{
						ChangeVidBrdSrc( ptCurPollMt );
					}
					m_tConfPollParam.SetLastPolledMt( (TMt)(*ptCurPollMt) );
				}
                break;
            
            case MODE_BOTH:
				{
					//本地发言
					ChangeSpeaker(ptCurPollMt, TRUE, bVmpNeedChgBySpeaker, FALSE );//VMP发言人通道已经在轮询中处理，此处不再需要
					m_tConfPollParam.SetLastPolledMt( (TMt)(*ptCurPollMt) );
				}
                break;

			case MODE_VIDEO_SPY:
			case MODE_BOTH_SPY:
				{
					u8 byMode = MODE_NONE;
					if( MODE_VIDEO_SPY == tPollInfo.GetMediaMode())
					{
						byMode = MODE_VIDEO;
					}
					else
					{
						byMode = MODE_BOTH;
					}

                    if ( /*!m_tConfPollParam.GetLastPolledMt()*/!tLastMt.IsNull() && 
						 /*!m_tConfPollParam.GetLastPolledMt()*/!tLastMt.IsLocal() )
					{
						FreeRecvSpy( /*m_tConfPollParam.GetLastPolledMt()*/tLastMt, byMode );
					}

					tFeedBackMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);
					OnSetOutView(tFeedBackMt, byMode);
					m_tConfPollParam.SetLastPolledMt( (TMt)(*ptCurPollMt) );
				}
				break;

            case MODE_VIDEO_BOTH:
				{
// 					TMt tLastMt = m_tConfPollParam.GetLastPolledMt();
// 					u8 bySrcChnnl = (tLastMt == m_tPlayEqp ? m_byPlayChnnl : 0);
					
					//本地视频广播
					FreeRecvSpy( tLastMt, MODE_VIDEO );
					if (!bIsVmpBrd)
					{
						ChangeVidBrdSrc(ptCurPollMt);
					}

					//视频回传
					tFeedBackMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);
					OnSetOutView(tFeedBackMt, MODE_VIDEO);
					m_tConfPollParam.SetLastPolledMt( (TMt)(*ptCurPollMt) );
				}
                break;

            case MODE_BOTH_BOTH:
                {
					//本地发言
					ChangeSpeaker(ptCurPollMt, TRUE, bVmpNeedChgBySpeaker, FALSE );//VMP发言人通道已经在轮询中处理，此处不再需要
					//音视频回传
					tFeedBackMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);
					OnSetOutView(tFeedBackMt, MODE_BOTH);
					m_tConfPollParam.SetLastPolledMt( (TMt)(*ptCurPollMt) );
				}
                break;

			// 20110414_miaoqs 主席轮询选看功能添加
			case MODE_VIDEO_CHAIRMAN:            
                {
					/*if ( !tLastPolledMt.IsNull() )
					{
						StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO, FALSE, FALSE );
					}*/
				
					if (HasJoinedChairman())
					{
						//ChangeSelectSrc((TMt)(*ptCurPollMt), m_tConf.GetChairman(), MODE_VIDEO);
						//直接走选看逻辑
						//上次轮询的终端和这次要轮询的终端是同一个，不再做选看
						if ( !m_tConfPollParam.GetLastPolledMt().IsNull() && m_tConfPollParam.GetLastPolledMt() == (TMt)(*ptCurPollMt) )
						{
							break;
						}
						TSwitchInfo tSwitchInfo;
						tSwitchInfo.SetSrcMt((TMt)*ptCurPollMt);
						tSwitchInfo.SetDstMt(m_tConf.GetChairman());
						tSwitchInfo.SetMode(MODE_VIDEO);
						cServMsg.SetNoSrc();
						cServMsg.SetSrcMtId(0);
						cServMsg.SetEventId(MCS_MCU_STARTSWITCHMT_REQ);
						cServMsg.SetMsgBody((u8*)&tSwitchInfo, sizeof(TSwitchInfo));
						ProcStartSelSwitchMt(cServMsg);
					}
					
					m_tConfPollParam.SetLastPolledMt( (TMt)(*ptCurPollMt) );
				}
				break;

			case MODE_BOTH_CHAIRMAN:             
                {
					/*if ( !tLastPolledMt.IsNull() )
					{
						StopSelectSrc(m_tConf.GetChairman(), MODE_BOTH, FALSE, FALSE);
					}*/
					
					if (HasJoinedChairman())
					{
						//ChangeSelectSrc((TMt)(*ptCurPollMt), m_tConf.GetChairman(), MODE_BOTH);
						//直接走选看逻辑
						//上次轮询的终端和这次要轮询的终端是同一个，不再做选看
						if ( !m_tConfPollParam.GetLastPolledMt().IsNull() && m_tConfPollParam.GetLastPolledMt() == (TMt)(*ptCurPollMt) )
						{
							break;
						}
						TSwitchInfo tSwitchInfo;
						tSwitchInfo.SetSrcMt((TMt)*ptCurPollMt);
						tSwitchInfo.SetDstMt(m_tConf.GetChairman());
						tSwitchInfo.SetMode(MODE_BOTH);
						cServMsg.SetNoSrc();
						cServMsg.SetSrcMtId(0);
						cServMsg.SetEventId(MCS_MCU_STARTSWITCHMT_REQ);
						cServMsg.SetMsgBody((u8*)&tSwitchInfo, sizeof(TSwitchInfo));
						ProcStartSelSwitchMt(cServMsg);
					}

					m_tConfPollParam.SetLastPolledMt( (TMt)(*ptCurPollMt) );
				}
				break;

            default:

				ConfPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcsMcuPollMsg] unexpected poll mode.%d rcved, ignore it!\n", tPollInfo.GetMediaMode());
                break;
            }          
            
			// 全部多回传资源都释放干净后,再做vmp处理
			if (!bVmpNeedChgBySpeaker)
			{
				u8 byVmpMemType = VMP_MEMBERTYPE_POLL;
				ChangeVmpChannelParam(ptCurPollMt/*&tLocalMt*/, byVmpMemType,&tLastPolledMt);
			}

			//TvWall
			TPeriEqpStatus tTWStatus;
			u8 byChnlIdx;
			u8 byEqpId;
           
			for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
			{
				if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
				{
					for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
					{
						// [miaoqingsong 20111205] 增加判断条件：主席轮询/上传轮询到的MT均不进电视墙视频轮询跟随通道
						if ( g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus) &&
							 (TW_MEMBERTYPE_POLL == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType) &&
							 (m_byConfIdx == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx()) && 
							 !(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
							 !(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY) 
							 )
						{
							ChangeTvWallSwitch(ptCurPollMt, byEqpId, byChnlIdx, TW_MEMBERTYPE_POLL, TW_STATE_CHANGE);
						}
					}
				}
			}
           
            //hdu
			TPeriEqpStatus tHduStatus;
			u8 byHduChnlIdx;
			u8 byHduEqpId;
			
			for (byHduEqpId = HDUID_MIN; byHduEqpId <= HDUID_MAX; byHduEqpId++)
			{
				if(IsValidHduEqp(g_cMcuVcApp.GetEqp(byHduEqpId)))
				{
					u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byHduEqpId));
					if (0 == byHduChnNum)
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU, "[ReleaseConf] GetHduChnNumAcd2Eqp failed!\n");
						continue;
					}
					for (byHduChnlIdx = 0; byHduChnlIdx < byHduChnNum; byHduChnlIdx++)
					{
						// [miaoqingsong 20111205] 增加判断条件：主席轮询/上传轮询到的MT均不进电视墙视频轮询跟随通道
						if ( g_cMcuVcApp.GetPeriEqpStatus(byHduEqpId, &tHduStatus) &&
							 (TW_MEMBERTYPE_POLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].byMemberType) &&
							 (m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetConfIdx()) &&
							 !(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
							 !(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY)
							)
						{
							// [2013/03/11 chenbing] HDU多画面目前不支持批量轮询,子通道置0
							ChangeHduSwitch(ptCurPollMt, byHduEqpId, byHduChnlIdx, 0, TW_MEMBERTYPE_POLL, TW_STATE_START, m_tConf.m_tStatus.GetPollMedia() ); 
						}
						else
						{
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcPollingChangeTimerMsg] HduEqp%d is not exist or not polling \n", byHduEqpId);
						}
					}
				}
			}
		}
		else
		{
			// xliang [5/27/2009] 轮询列表里没有MT，则把会议轮询停掉
			ProcStopConfPoll();
			/*
			if ( bIsPollNumNoLimit )
			{
				//NotifyMcsAlarmInfo( 0, ERR_MCU_POLLING_NOMTORVIDSRC );
				TMtPollParam tPollMt;
				tPollMt.SetNull();
				tPollInfo.SetMtPollParam( tPollMt );
				m_tConf.m_tStatus.SetPollInfo( tPollInfo );

				cServMsg.SetMsgBody( (u8*)&tPollInfo, sizeof(TPollInfo) );
				SendMsgToAllMcs( MCU_MCS_POLLSTATE_NOTIF, cServMsg );

				SetTimer(MCUVC_POLLING_CHANGE_TIMER, 1000*5);
			}
            else
            {
                ProcStopConfPoll();
            }
			*/
		}

        break;
    }
    default:
        break;
    }

    return;
}

/*====================================================================
    函数名      NotifyMtToBePolledNext
    功能        ：发送短消息通知下一个被轮询到的终端
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/05/09    4.0         张宝卿         创建
====================================================================*/
void CMcuVcInst::NotifyMtToBePolledNext( void )
{
	
    TMtPollParam *ptNextPollMt;
    u8 byPollIdx = m_tConfPollParam.GetCurrentIdx();
    TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
	ptNextPollMt = GetNextMtPolled(byPollIdx, tPollInfo);
	/*
    if ( NULL != ptNextPollMt &&
	//zbq[12/25/2007]级联暂不支持
	ptNextPollMt->IsLocal())
    {
	Mcu2MtNtfMsgType emMsgType = emBePolledNextNtf;
	u32 dwMsgType = htonl((u32)emMsgType);
	
	  CServMsg cServMsg;
	  cServMsg.SetConfId( m_tConf.GetConfId() );
	  cServMsg.SetEventId( MCU_MT_SOMEOFFERRING_NOTIF );
	  cServMsg.SetMsgBody( (u8*)&dwMsgType, sizeof(u32) );
	  
        SendMsgToMt( ptNextPollMt->GetMtId(), MCU_MT_SOMEOFFERRING_NOTIF, cServMsg );
		}
	*/

	if( !g_cMcuVcApp.IsSendStaticText() )
	{
		return;
	}
	
	// [pengjie 2010/4/26] 轮询通知统一走静态文本短消息
    if ( NULL != ptNextPollMt )
    {
        s8 achPollingNtf[256] = "您即将被轮询!\n";
        
        u16 wMtNum = 1;
        wMtNum = htons(wMtNum);
        
        CRollMsg cRollMsg;
        cRollMsg.SetRollMsgContent( (u8*)achPollingNtf, strlen(achPollingNtf) );
        cRollMsg.SetRollTimes( 1 );
        cRollMsg.SetType( ROLLMSG_TYPE_STATICTEXT );
        
		cRollMsg.SetMsgId((u8)MSGID_POLL); //  [pengguofeng 5/10/2013]

        CServMsg cServMsg;
        cServMsg.SetConfId( m_tConf.GetConfId() );
        cServMsg.SetEventId( MCS_MCU_SENDRUNMSG_CMD );
        cServMsg.SetMsgBody( (u8*)&wMtNum, sizeof(u16) );
        cServMsg.CatMsgBody( (u8*)ptNextPollMt, sizeof(TMt) );
        cServMsg.CatMsgBody( (u8*)&cRollMsg, sizeof(CRollMsg) );
        
        CMessage cMsg;
        cMsg.event = MCS_MCU_SENDRUNMSG_CMD;
        cMsg.content = cServMsg.GetServMsg();
        cMsg.length = cServMsg.GetServMsgLen();
        ProcMcsMcuSendMsgReq(&cMsg);
    }
    return;
}


/*====================================================================
函数名      ：NotifyMtSpeakStatus
功能        ：通知终端当前的发言状态
算法实现    ：
引用全局变量：
输入参数说明：void
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
05/13/2010  4.6         张宝卿         创建
====================================================================*/
void CMcuVcInst::NotifyMtSpeakStatus( TMt tMt, emMtSpeakerStatus emStatus)
{
	
	u32 dwStatus = (u32)emStatus;
	CServMsg cServMsg;
	TMsgHeadMsg tHeadMsg;
	if( tMt.IsLocal() )
	{
		dwStatus = htonl(dwStatus);		
		cServMsg.SetConfId( m_tConf.GetConfId() );
		cServMsg.SetEventId( MCU_MT_MTSPEAKSTATUS_NTF );
		cServMsg.SetMsgBody( (u8*)&dwStatus, sizeof(u32) );
		
		SendMsgToMt( tMt.GetMtId(), MCU_MT_MTSPEAKSTATUS_NTF, cServMsg );
	}
    else 
	{				
		tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
		
		cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		cServMsg.CatMsgBody( (u8*)&tMt,sizeof(TMt) );
		cServMsg.CatMsgBody( (u8*)&dwStatus,sizeof(u32) );

		cServMsg.SetEventId( MCU_MCU_SPEAKSTATUS_NTF );
		SendMsgToMt( (u8)tMt.GetMcuId(), MCU_MCU_SPEAKSTATUS_NTF, cServMsg );
	}
	
	
    ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[NotifyMtSpeakStatus] Mt(%d.%d.%d) level.%d speak status.%d has been notified!\n", tMt.GetMcuId(),tMt.GetMtId(),
		tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],tHeadMsg.m_tMsgDst.m_byCasLevel,emStatus
		);
	
    return;
}

/*====================================================================
函数名      ：NotiyfMtConfSpeakMode
功能        ：通知终端 会议 当前的发言模式
算法实现    ：
引用全局变量：
输入参数说明：void
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
05/13/2010  4.6         张宝卿         创建
====================================================================*/
void CMcuVcInst::NotiyfMtConfSpeakMode(u8 byMtId, u8 byMode )
{
    emConfSpeakMode emMode;
    switch (byMode)
    {
    case CONF_SPEAKMODE_NORMAL:
        emMode = emSpeakNormal;
        break;
    case CONF_SPEAKMODE_ANSWERINSTANTLY:
        emMode = emAnswerInstantly;
        break;
    default:
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[NotiyfMtConfSpeakMode] unexpected speak mode.%d!\n", byMode);
        return;
    }
	
	u32 dwStatus = (u32)emMode;	
	dwStatus = htonl( dwStatus );

	
    CServMsg cServMsg;
    cServMsg.SetConfId( m_tConf.GetConfId() );
    cServMsg.SetEventId( MCU_MT_CONFSPEAKMODE_NTF );
    cServMsg.SetMsgBody( (u8*)&dwStatus, sizeof(u32) );
    
    SendMsgToMt( byMtId, MCU_MT_CONFSPEAKMODE_NTF, cServMsg );
	
    return;
}

/*=============================================================================
  函 数 名： ProcMcsMcuHduBatchPollMsg
  功    能： hdu批量轮询消息处理函数
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  09/04/09    4.6         江乐斌         创建
  2013/03/11  4.7.2       陈兵           修改(HDU多画面支持)
=============================================================================*/
void CMcuVcInst::ProcMcsMcuHduBatchPollMsg(const CMessage *pcMsg)
{
	if ( NULL == pcMsg )
	{
	 	ConfPrint( LOG_LVL_ERROR, MID_MCU_HDU, "[ProcMcsMcuHduBatchPollMsg] pcMsg is null!\n" );
	    return;
	}
    
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    u8  byIndex;

	switch ( pcMsg->event )
	{
	// 开始批量轮询
	case MCS_MCU_STARTHDUBATCHPOLL_REQ:
	{
        THduPollSchemeInfo tHduPollSchemeInfo;
        THduChnlCfgInfo tHduChnlCfgInfo[MAXNUM_HDUCFG_CHNLNUM];

		// [12/22/2010 liuxu][走读] 先要清空，以避免残留
		memset(m_tHduBatchPollInfo.m_tChnlBatchPollInfo, 0, sizeof(m_tHduBatchPollInfo.m_tChnlBatchPollInfo));

		THduChnlPollInfo *ptHduChnlPollInfo = (THduChnlPollInfo*)m_tHduBatchPollInfo.m_tChnlBatchPollInfo;


		if( POLL_STATE_NONE != m_tHduBatchPollInfo.GetStatus() )
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuHduBatchPollMsg] hdu is already in batch polling\n");
			return;
		}

		tHduPollSchemeInfo = *(THduPollSchemeInfo*)cServMsg.GetMsgBody();
        tHduPollSchemeInfo.m_tHduStyleCfgInfo.GetHduChnlCfgTable( tHduChnlCfgInfo );
        m_tHduBatchPollInfo.SetHduSchemeIdx( tHduPollSchemeInfo.m_tHduStyleCfgInfo.GetStyleIdx() );
        m_tHduBatchPollInfo.SetKeepTime( tHduPollSchemeInfo.GetKeepTime() );
        m_tHduBatchPollInfo.SetCycles( tHduPollSchemeInfo.GetCycles() );
        u8 bySchemeIdx = tHduPollSchemeInfo.m_tHduStyleCfgInfo.GetStyleIdx();

		u8 byHduChnlInPollNum = 0;        //  在该预案中参与批量轮询的通道数
        u8 byChnlInUseNum = 0;            //  该预案中批量轮询前被占用的通道数 
		BOOL bIsChnlInUse = FALSE;        //  是否有通道被占用
		BOOL bIsSchemeChnInOtherConf = FALSE; //当前批量轮询预案通道资源是否有被其他会议占用，
											  //有则nack, 没有则停掉所有当前会议占用的电视墙(dec5和hdu)再进行批量轮询
		
		CServMsg cHduStatMsg;
		TPeriEqpStatus tHduStatus;
		for ( byIndex=0; byIndex < MAXNUM_HDUCFG_CHNLNUM; byIndex++ )
		{
			if ( tHduChnlCfgInfo[byIndex].GetEqpId() >= HDUID_MIN
				&& tHduChnlCfgInfo[byIndex].GetEqpId() <= HDUID_MAX
				)
			{
				g_cMcuVcApp.GetPeriEqpStatus( tHduChnlCfgInfo[byIndex].GetEqpId(), &tHduStatus );
                // 有预案通道被其他会议占用，直接nack
				if ( tHduStatus.m_byOnline == 1
					&& m_byConfIdx != tHduStatus.m_tStatus.tHdu.atVideoMt[tHduChnlCfgInfo[byIndex].GetChnlIdx()].GetConfIdx()
					&& 0 != tHduStatus.m_tStatus.tHdu.atVideoMt[tHduChnlCfgInfo[byIndex].GetChnlIdx()].GetConfIdx())
                {
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[ProcMcsMcuHduBatchPollMsg] current hdu scheme's chnls is used in other conf! please release them! \n" );
                    cServMsg.SetErrorCode( ERR_MCU_HDUBATCHPOLL_CHNLINUSE );
					SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 2, cServMsg );
					
					return;					
				}
				
				
			}			
		}

		//Bug00160221：电视墙轮询时开启批量轮询，电视墙轮询刷新异常
		//yrl20131108 该段提前处理，因为下面批量轮询将MemberType改变，导致电视墙轮询不能停止
		u8 byHduIdx = tHduChnlCfgInfo[byIndex].GetEqpId();
		u8 byChnIdx = tHduChnlCfgInfo[byIndex].GetChnlIdx();
		CServMsg cSMsg;
		CMessage cMsg;
		for ( byIndex=0; byIndex < MAXNUM_HDUCFG_CHNLNUM; byIndex++ )
		{
			byHduIdx = tHduChnlCfgInfo[byIndex].GetEqpId();
			byChnIdx = tHduChnlCfgInfo[byIndex].GetChnlIdx();
			g_cMcuVcApp.GetPeriEqpStatus( byHduIdx, &tHduStatus );
			if ( byHduIdx >= HDUID_MIN && byHduIdx <= HDUID_MAX )
			{
				if ( HDUCHN_MODE_ONE == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byChnIdx) )
				{
					if (tHduStatus.m_byOnline == 1 
						&& INVALID_MCUIDX != tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].GetMcuId()
						&& m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].GetConfIdx()
						)
					{
						if (TW_MEMBERTYPE_TWPOLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].byMemberType)
						{
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU, "[ProcMcsMcuHduBatchPollMsg] Stop Hdu<EqpId:%d, ChnId:%d> Poll!\n", byHduIdx, byChnIdx);
							cSMsg.SetMsgBody((u8*)&byHduIdx, sizeof(u8));
							cSMsg.CatMsgBody((u8*)&byChnIdx, sizeof(u8));
							cMsg.content = cSMsg.GetServMsg();
							cMsg.length  = cSMsg.GetServMsgLen();
							cMsg.event   = MCS_MCU_STOPHDUPOLL_CMD;
							ProcMcsMcuHduPollMsg(&cMsg);
						}
					}
				}
			}
		}

		for ( byIndex=0; byIndex < MAXNUM_HDUCFG_CHNLNUM; byIndex++ )
		{
			if ( tHduChnlCfgInfo[byIndex].GetEqpId() >= HDUID_MIN
				&& tHduChnlCfgInfo[byIndex].GetEqpId() <= HDUID_MAX
				)
			{
				g_cMcuVcApp.GetPeriEqpStatus( tHduChnlCfgInfo[byIndex].GetEqpId(), &tHduStatus );
                
				if ( tHduStatus.m_byOnline == 1
					&& m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[tHduChnlCfgInfo[byIndex].GetChnlIdx()].GetConfIdx() )
                {
					ChangeHduSwitch( NULL, tHduChnlCfgInfo[byIndex].GetEqpId(),tHduChnlCfgInfo[byIndex].GetChnlIdx(),
									0, TW_MEMBERTYPE_NULL, TW_STATE_STOP, MODE_BOTH );							
				}
				// 重新取hdustatus
				g_cMcuVcApp.GetPeriEqpStatus( tHduChnlCfgInfo[byIndex].GetEqpId(), &tHduStatus );
				// [2013/08/02 chenbing] 开启批量轮询需要设置会议号，防止其他会议占用
				tHduStatus.m_tStatus.tHdu.atVideoMt[tHduChnlCfgInfo[byIndex].GetChnlIdx()].byMemberType = TW_MEMBERTYPE_BATCHPOLL;
				tHduStatus.m_tStatus.tHdu.atVideoMt[tHduChnlCfgInfo[byIndex].GetChnlIdx()].SetConfIdx(m_byConfIdx);
				ptHduChnlPollInfo[byHduChnlInPollNum].SetPosition( byIndex );
				ptHduChnlPollInfo[byHduChnlInPollNum].SetChnlIdx( tHduChnlCfgInfo[byIndex].GetChnlIdx() );
				ptHduChnlPollInfo[byHduChnlInPollNum].SetEqpId( tHduChnlCfgInfo[byIndex].GetEqpId());
				tHduStatus.m_tStatus.tHdu.atHduChnStatus[tHduChnlCfgInfo[byIndex].GetChnlIdx()].SetSchemeIdx(bySchemeIdx );
				g_cMcuVcApp.SetPeriEqpStatus( tHduChnlCfgInfo[byIndex].GetEqpId(), &tHduStatus );
				byHduChnlInPollNum ++;

				// 批量轮询设置会议号后需要上报刷新
				
				cHduStatMsg.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
				SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cHduStatMsg);
			}			
		}

		//清理预案下电视墙通道
//		for ( byIndex=0; byIndex < MAXNUM_HDUCFG_CHNLNUM; byIndex++ )
// 		{
// 			u8 byHduIdx = tHduChnlCfgInfo[byIndex].GetEqpId();
// 			u8 byChnIdx = tHduChnlCfgInfo[byIndex].GetChnlIdx();
// 			g_cMcuVcApp.GetPeriEqpStatus( byHduIdx, &tHduStatus );
// 			if ( byHduIdx >= HDUID_MIN && byHduIdx <= HDUID_MAX )
// 			{
// 				// [2013/03/11 chenbing] 批量轮询需要停止四风格 
// 				if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byChnIdx) )
// 				{
// 					if( tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].GetConfIdx()/*会议号不为0时需要判断会议号*/
// 						&& m_byConfIdx != tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].GetConfIdx()
// 					  )
// 					{
// 						break;
// 					}
// 
// 					for ( u8 bySubIndex=0; bySubIndex<HDU_MODEFOUR_MAX_SUBCHNNUM; bySubIndex++)
// 					{
// 						if (THduChnStatus::eRUNNING == tHduStatus.m_tStatus.tHdu.GetChnStatus(byChnIdx, bySubIndex))
// 						{
// 							//拆除所有正在运行的通道
// 							ChangeHduSwitch(NULL, byHduIdx, byChnIdx, bySubIndex, 
// 								TW_MEMBERTYPE_MCSSPEC, TW_STATE_STOP, MODE_VIDEO);	
// 						}	
// 					}
// 				}
// 				else
// 				{
// 					if (tHduStatus.m_byOnline == 1 
// 						&& INVALID_MCUIDX != tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].GetMcuId()
// 						&& m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].GetConfIdx()
// 						)
// 					{
// 						if (TW_MEMBERTYPE_TWPOLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].byMemberType)
// 						{
// 							CServMsg cSMsg;
// 							CMessage cMsg;
// 							ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU, "[ProcMcsMcuHduBatchPollMsg] Stop Hdu<EqpId:%d, ChnId:%d> Poll!\n", byHduIdx, byChnIdx);
// 							cSMsg.SetMsgBody((u8*)&byHduIdx, sizeof(u8));
// 							cSMsg.CatMsgBody((u8*)&byChnIdx, sizeof(u8));
// 							cMsg.content = cSMsg.GetServMsg();
// 							cMsg.length  = cSMsg.GetServMsgLen();
// 							cMsg.event   = MCS_MCU_STOPHDUPOLL_CMD;
// 				    		ProcMcsMcuHduPollMsg(&cMsg);
// 						}
// 						else
// 						{
// 							ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU, "[ProcMcsMcuHduBatchPollMsg] Stop Hdu<EqpId:%d, ChnId:%d> running!\n", byHduIdx, byChnIdx);
// 							tMember = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx];
// 							// [2013/03/11 chenbing] HDU多画面目前不支持批量轮询,子通道置0,
// 							// [2013/03/11 chenbing] [Bug00133888]TMt不为空，导致没有停止HDU通道
// 							// StartSwitchToPeriEqp中建交换状态判断失败，没有开启HDU
// 							ChangeHduSwitch(NULL, byHduIdx, byChnIdx, 0/*子通道为0*/, 
// 							tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].byMemberType,	TW_STATE_STOP);	
// 						}
// 					}
// 				}
// 			}
// 		}
	

        u32 dwCycles = tHduPollSchemeInfo.GetCycles();
		m_tHduBatchPollInfo.SetChnlPollNum( byHduChnlInPollNum );
		
		m_tConf.m_tStatus.GetHduPollInfo()->SetPollNum( dwCycles );
        m_tConf.m_tStatus.GetHduPollInfo()->SetKeepTime( tHduPollSchemeInfo.GetKeepTime() );

		// 开始第一版的轮询 
		if ( !HduBatchPollOfOneCycle( TRUE ) )
		{
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuHduBatchPollMsg] have no Mt in current conf!\n" );
			cServMsg.SetErrorCode( ERR_MCU_HDUBATCHPOLL_NONEMT );
			SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 2, cServMsg );
            return;
		}


		TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
		tConfAttrbEx.SetSchemeIdxInBatchPoll( bySchemeIdx );
		m_tConf.SetConfAttrbEx( tConfAttrbEx );

		tHduPollSchemeInfo.SetStatus( POLL_STATE_NORMAL );

		// 保存预案信息
		memcpy( (void*)&m_tHduPollSchemeInfo, (void*)&tHduPollSchemeInfo, sizeof( tHduPollSchemeInfo ) );

		cServMsg.SetMsgBody( (u8*)&tHduPollSchemeInfo, sizeof( tHduPollSchemeInfo ) );
		SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 1, cServMsg);

		m_tConf.m_tStatus.m_tConfMode.SetHduInBatchPoll( POLL_STATE_NORMAL );
		m_tHduBatchPollInfo.SetStatus( POLL_STATE_NORMAL );		
		// 通知会控会议状态改变
		ConfStatusChange();            
		SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );

		//通知所有会控
		cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
		if(m_tConf.HasConfExInfo())
		{
			u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
			u16 wPackDataLen = 0;
			PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
			cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
		}
	    SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
		
		SetTimer(MCUVC_HDUBATCHPOLLI_CHANGE_TIMER, 1000 * tHduPollSchemeInfo.GetKeepTime() );

	}
	break;
    
	// 停止hdu批量轮询
	case MCS_MCU_STOPHDUBATCHPOLL_REQ:
	{
        StopHduBatchPollSwitch(TRUE);

		SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 1, cServMsg );

		m_tHduBatchPollInfo.SetNull();
		m_tConf.m_tStatus.m_tConfMode.SetHduInBatchPoll( POLL_STATE_NONE );

 		TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
		tConfAttrbEx.SetSchemeIdxInBatchPoll( 0 );
		m_tConf.SetConfAttrbEx( tConfAttrbEx );
		THduPollInfo tHduPollInfo;		
		tHduPollInfo = *m_tConf.m_tStatus.GetHduPollInfo();
		tHduPollInfo.SetKeepTime( 10 );     //默认10秒
		tHduPollInfo.SetPollNum( 0 );
        m_tConf.m_tStatus.SetHduPollInfo( tHduPollInfo );
		// 通知会控会议状态改变
        ConfStatusChange();            
        m_tHduPollSchemeInfo.SetNull();
		cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof( m_tHduPollSchemeInfo ) );
        SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );

		//通知所有会控
		cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
		if(m_tConf.HasConfExInfo())
		{
			u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
			u16 wPackDataLen = 0;			
			PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
			cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
		}
	    SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

		KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
	}
	    break;
    
	// 暂停hdu批量轮询
	case MCS_MCU_PAUSEHDUBATCHPOLL_REQ:
	{
		SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 1, cServMsg );

	    m_tHduBatchPollInfo.SetStatus( POLL_STATE_PAUSE );
        m_tHduPollSchemeInfo.SetStatus( POLL_STATE_PAUSE );

		m_tConf.m_tStatus.m_tConfMode.SetHduInBatchPoll( POLL_STATE_PAUSE );
		ConfStatusChange();            // 通知会控会议状态改变
		cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof( m_tHduPollSchemeInfo ) );
        SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );

		//通知所有会控
		cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
		if(m_tConf.HasConfExInfo())
		{
			u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
			u16 wPackDataLen = 0;
			PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
			cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
		}
	    SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
		
		KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
	}
	    break;
    
	// 恢复hdu批量轮询
	case MCS_MCU_RESUMEHDUBATCHPOLL_REQ:
	{
		SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 1, cServMsg );
		m_tHduBatchPollInfo.SetStatus( POLL_STATE_NORMAL );
		m_tHduPollSchemeInfo.SetStatus( POLL_STATE_NORMAL );
		
		SetTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER, 1000 * m_tHduBatchPollInfo.GetKeepTime() );

		m_tConf.m_tStatus.m_tConfMode.SetHduInBatchPoll( POLL_STATE_NORMAL );
		ConfStatusChange();            // 通知会控会议状态改变
		cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof( m_tHduPollSchemeInfo ) );
		SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );
		//通知所有会控
		cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
		if(m_tConf.HasConfExInfo())
		{
			u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
			u16 wPackDataLen = 0;
			PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
			cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
		}
	    SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

	}
	    break;
		
	case MCS_MCU_GETBATCHPOLLSTATUS_CMD:
		{
			cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof( m_tHduPollSchemeInfo ) );
			SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );
		}
		break;

	default:
		ConfPrint( LOG_LVL_ERROR, MID_MCU_HDU,  "[ProcMcsMcuHduBatchPollMsg] message type(%u) is wrong!\n" , pcMsg->event );
    
    } 	
	return;
}

/*=============================================================================
  函 数 名： ProcMcsMcuHduPollMsg
  功    能： hdu轮询消息处理函数
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  09/04/09    4.6         江乐斌         创建
=============================================================================*/
void CMcuVcInst::ProcMcsMcuHduPollMsg(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch (pcMsg->event)
	{
		case MCS_MCU_STARTHDUPOLL_CMD:
			{
				TPeriEqpStatus tHduStatus;
				TTvWallPollParam tTWPollParam = *(TTvWallPollParam *)(cServMsg.GetMsgBody());
				TTvWallPollInfo  tTwPollInfo  = tTWPollParam.GetTWPollInfo();

				TEqp tHdu          = tTWPollParam.GetTvWall();
				u8   byHduId       = tTWPollParam.GetTvWall().GetEqpId();
				u8   byHduChnId    = tTWPollParam.GetTWChnnl();
				u8   bySchemeIdx   = tTWPollParam.GetSchemeIdx();
				u8   byPolledMtNum = tTWPollParam.GetPolledMtNum();
				u8   byMode        = tTWPollParam.GetMediaMode();
				TMtPollParam *ptMtPollParam = tTWPollParam.GetPollMtByIdx(0);
				
				u8 byPollState   = POLL_STATE_NONE;
				u32 dwTimerIdx    = 0;

				u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(tHdu);
				if (0 == byHduChnNum)
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[ProcMcsMcuHduPollMsg] GetHduChnNumAcd2Eqp failed!\n");
					return;
				}

				if (byHduChnId >= byHduChnNum || !(byHduId >= HDUID_MIN && byHduId <= HDUID_MAX))
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[ProcMcsMcuHduPollMsg] hdu <EqpId:%d, ChnId:%d> is invalid :%d\n", byHduId, byHduChnId);
                    break;
				}
				
				g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
				// [2013/03/11 chenbing] 四风格不能进行单通道轮询 
				if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byHduChnId) )
				{
					ConfPrint( LOG_LVL_ERROR, MID_MCU_HDU, "[ProcMcsMcuHduPollMsg] Chnnl %d is can't polling, because it is HDUCHN_MODE_FOUR\n", byHduChnId);
					return;
				}

				tHduStatus.m_tStatus.tHdu.atHduChnStatus[byHduChnId].SetSchemeIdx(bySchemeIdx);
                g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);

				THduMember tHduMem = tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnId];
				if(0 != tHduMem.GetConfIdx() && tHduMem.GetConfIdx() != m_byConfIdx)          //其他会议的没有权限操作, 本会议的替换
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[ProcMcsMcuHduPollMsg] Confidx %d is already in polling, can't replace it\n", tHduMem.GetConfIdx());
					break;
				}

                if( m_tTWMutiPollParam.GetPollState(byHduId, byHduChnId, byPollState) &&
					POLL_STATE_NONE != byPollState)
				{
					// 防止由于m_tTWMutiPollParam中通道轮询状态没有清空导致轮询失败
					if (tHduMem.byMemberType != TW_MEMBERTYPE_NULL && tHduMem.GetConfIdx() == m_byConfIdx )
					{
						ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuHduPollMsg] hdu <EqpId:%d, ChnId:%d> is already in polling\n", byHduId, byHduChnId);
						break;
					}
				}

				//设置当前通道轮询信息
				m_tTWMutiPollParam.SetTWPollInfo(byHduId, byHduChnId, tTwPollInfo);
                m_tTWMutiPollParam.InitPollParam(byHduId, byHduChnId, byPolledMtNum, ptMtPollParam);			

// 				// xsl [8/23/2006] 判断卫星会议是否停止轮询
// 				if (m_tConf.GetConfAttrb().IsSatDCastMode())
// 				{            
// 					//若回传个数已满其轮询终端中有不发码流的终端则拒绝轮询
// 					if (IsOverSatCastChnnlNum(0))
// 					{
// 						//逻辑有误，不会进到[8/8/2012 chendaiwei]
// // 						for(u8 byIdx = 0; byIdx < byPolledMtNum; byIdx++)
// // 						{
// // 							TMtPollParam *ptMtPoll = NULL;
// // 							if (m_tTWMutiPollParam.GetPollMtByIdx(byHduId, byHduChnId, byIdx, ptMtPoll) &&
// // 								ptMtPoll && m_ptMtTable->GetMtSndBitrate(ptMtPoll->GetMtId()) == 0)
// // 							{
// // 								ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuHduPollMsg-start] over max upload mt num. nack!\n");  
// // 								NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_DCAST_OVERCHNNLNUM);
// // 								return;
// // 							}
// // 						}
// 					}            
// 				}    
				                   
				u8 byCurPollPos = 0;
				m_tTWMutiPollParam.GetCurrentIdx(byHduId, byHduChnId, byCurPollPos);
				m_tTWMutiPollParam.SetIsStartAsPause(byHduId, byHduChnId, 0);
				
				//获取定时器索引
				if(!m_tTWMutiPollParam.GetTimerIdx(byHduId, byHduChnId, dwTimerIdx))
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF," [ProcMcsMcuHduPollMsg] Cannot HDU<EqpId:%d, ChnId:%d> getTimerIdx!\n",byHduId, byHduChnId);
					return;
				}

				// [10/28/2011 liuxu] 此时轮询才算成功
				m_tTWMutiPollParam.SetPollState(byHduId, byHduChnId, POLL_STATE_NORMAL);

				TMtPollParam *ptCurPollMt = GetMtTWPollParam(byCurPollPos, byHduId, byHduChnId);
				m_tTWMutiPollParam.SetCurrentIdx(byHduId, byHduChnId, byCurPollPos);
				if (NULL != ptCurPollMt)
				{
					u32 dwSetTimer = ptCurPollMt->GetKeepTime();
					TMt tOrgMt     = *ptCurPollMt;   
				
					//通知所有会控
				    m_tTWMutiPollParam.SetCurPolledMt(byHduId, byHduChnId, *ptCurPollMt);
					m_tTWMutiPollParam.GetTWPollParam(byHduId, byHduChnId, tTWPollParam);
					tTWPollParam.SetConfIdx(m_byConfIdx);
					m_tTWMutiPollParam.SetTWPollParam(byHduId, byHduChnId, tTWPollParam);
					cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
					SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
					
					BOOL32 bCanInTvw = TRUE;
					if ( VCS_CONF == m_tConf.GetConfSource() )
					{
						//当下级有当前调度终端，且同一个下级的其它终端想进电视墙，回NACK
						if( !tOrgMt.IsLocal() 
							&& !IsLocalAndSMcuSupMultSpy(tOrgMt.GetMcuId()) 
							&& IsMtInMcu(GetLocalMtFromOtherMcuMt(tOrgMt), m_cVCSConfStatus.GetCurVCMT()) 
							&& !( m_cVCSConfStatus.GetCurVCMT() == tOrgMt ))
						{
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  
								"[ProcMcsMcuHduPollMsg] CurVCMT(%d.%d) and Mt(%d.%d) is in same smcu,Mt can't in tvwall \n ",
								m_cVCSConfStatus.GetCurVCMT().GetMcuId(),
								m_cVCSConfStatus.GetCurVCMT().GetMtId(),
								tOrgMt.GetMcuId(),
								tOrgMt.GetMtId() );

							bCanInTvw = FALSE;							
						}	
					}

					if( bCanInTvw )
					{
						// [2013/03/11 chenbing] HDU多画面目前不支持批量轮询,子通道置0
						ChangeHduSwitch(&tOrgMt, byHduId, byHduChnId, 0, TW_MEMBERTYPE_TWPOLL, TW_STATE_START, byMode);
					}
					ConfStatusChange();
					
					//设置轮询时钟
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, 1000 * dwSetTimer, dwTimerIdx);
				}
				else
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuHduPollMsg-start] Cannot CurPollTMt<CurPos:%d> Param in hdu<EqpId%d, ChnId:%d>!\n", 
									byCurPollPos, byHduId, byHduChnId);
					//设置轮询时钟
					// [2013/06/27 chenbing] (未找到终端是否可以不再设置定时器)
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, 1000 * 5, dwTimerIdx);
					// [2013/06/27 chenbing] 修改Bug00145985，此处需要设置会议号，如不设置将导致定时器到后查询到可轮询终端时，
					// 将给界面上报当前轮询状态时无会议号，导致结束会议无法清除轮询状态。
					m_tTWMutiPollParam.GetTWPollParam(byHduId, byHduChnId, tTWPollParam);
					tTWPollParam.SetConfIdx(m_byConfIdx);
					m_tTWMutiPollParam.SetTWPollParam(byHduId, byHduChnId, tTWPollParam);
				}      

				// [10/20/2011 liuxu] 预占用电视墙
				g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
				tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnId].SetConfIdx(m_byConfIdx);
				tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnId].byMemberType = TW_MEMBERTYPE_TWPOLL;
                g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);
				
				CServMsg cHduStatusMsg;
				cHduStatusMsg.SetMsgBody( (u8*)&tHduStatus, sizeof( tHduStatus ) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cHduStatusMsg );
			}
			break;
	    case MCS_MCU_PAUSEHDUPOLL_CMD: 
			{
				u8 byHduId = *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));
				m_tTWMutiPollParam.SetPollState(byHduId, byChnId, POLL_STATE_PAUSE);
				TTvWallPollParam tTWPollParam;
				m_tTWMutiPollParam.GetTWPollParam(byHduId, byChnId, tTWPollParam);

			    cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
				
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byHduId, byChnId, dwTimerIdx) )
				{
					KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
				}	
			}
			break;
		case MCS_MCU_RESUMEHDUPOLL_CMD:
			{
				u8 byHduId = *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));
				m_tTWMutiPollParam.SetPollState(byHduId, byChnId, POLL_STATE_NORMAL);
				TTvWallPollParam tTWPollParam;
				m_tTWMutiPollParam.GetTWPollParam(byHduId, byChnId, tTWPollParam);

				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
				m_tTWMutiPollParam.SetIsStartAsPause(byHduId, byChnId, 1);
				
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byHduId, byChnId, dwTimerIdx) )
				{
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, 1000, dwTimerIdx);
				}
			}
			break;

	    case MCS_MCU_STOPHDUPOLL_CMD:
			{	
				u8 byHduId = *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));

				TPeriEqpStatus tHduStatus;
 				g_cMcuVcApp.GetPeriEqpStatus( byHduId, &tHduStatus );
				if ( tHduStatus.m_tStatus.tHdu.atVideoMt[byChnId].byMemberType != TW_MEMBERTYPE_TWPOLL
					|| tHduStatus.m_tStatus.tHdu.atVideoMt[byChnId].GetConfIdx() != m_byConfIdx )
				{
					break;
				}

				//由于需要清空上报界面，所以先记录下来用于拆到电视墙交换
				TMt tHduMem = m_tTWMutiPollParam.GetCurPolledMt(byHduId, byChnId);

				TMtPollParam tMtPollParam;
                memset(&tMtPollParam, 0, sizeof(tMtPollParam));
				m_tTWMutiPollParam.SetCurPolledMt(byHduId, byChnId, tMtPollParam);
				m_tTWMutiPollParam.SetPollState(byHduId, byChnId, POLL_STATE_NONE);				
				m_tTWMutiPollParam.SetPollNum(byHduId, byChnId, 0);

				TMtPollParam atTWPollParam[MAXNUM_CONF_MT];
				m_tTWMutiPollParam.InitPollParam(byHduId, byChnId, MAXNUM_CONF_MT, atTWPollParam);
				m_tTWMutiPollParam.SetPolledMtNum(byHduId, byChnId, 0);

				TTvWallPollParam tTWPollParam;				
				m_tTWMutiPollParam.GetTWPollParam(byHduId, byChnId, tTWPollParam);
				
				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);				
 				
				if (!tHduMem.IsNull())
				{
					// [2013/03/11 chenbing] HDU多画面目前不支持批量轮询,子通道置0
					ChangeHduSwitch(NULL, byHduId, byChnId, 0, TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);
				}			
				m_tTWMutiPollParam.SetIsStartAsPause(byHduId, byChnId, 0);	
				TTvWallPollParam tTWPollParamNull;				
				m_tTWMutiPollParam.SetTWPollParam(byHduId, byChnId, tTWPollParamNull);

				// 重新获取状态
 				g_cMcuVcApp.GetPeriEqpStatus( byHduId, &tHduStatus );
				// 预占用的电视墙通道, 需要释放
				if (tHduStatus.m_tStatus.tHdu.atVideoMt[byChnId].byMemberType == TW_MEMBERTYPE_TWPOLL
					&& tHduStatus.m_tStatus.tHdu.atVideoMt[byChnId].GetConfIdx() == m_byConfIdx )
				{
					tHduStatus.m_tStatus.tHdu.atVideoMt[byChnId].byMemberType = TW_MEMBERTYPE_NULL;
					tHduStatus.m_tStatus.tHdu.atVideoMt[byChnId].SetConfIdx(0);
				}

				tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnId].SetSchemeIdx( 0 );
				g_cMcuVcApp.SetPeriEqpStatus( byHduId, &tHduStatus );
				
				CServMsg cHduStatusMsg;
				cHduStatusMsg.SetMsgBody( (u8*)&tHduStatus, sizeof( tHduStatus ) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cHduStatusMsg );

				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byHduId, byChnId, dwTimerIdx) )
				{
					KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
				}
			}
			break;
		case MCS_MCU_CHANGEHDUPOLLPARAM_CMD:
			{
				TTvWallPollParam tTWPollParam = *(TTvWallPollParam *)(cServMsg.GetMsgBody());
				TTvWallPollInfo  tTwPollInfo  = tTWPollParam.GetTWPollInfo();

				u8   byPolledMtNum = tTWPollParam.GetPolledMtNum();
				u8   byHduId       = tTWPollParam.GetTvWall().GetEqpId();
				u8   byHduChnId    = tTWPollParam.GetTWChnnl();

				//设置当前通道轮询信息
				m_tTWMutiPollParam.SetTWPollInfo(byHduId, byHduChnId, tTwPollInfo);

				//获取所有轮询终端
				TMtPollParam *ptMtPollParam = tTWPollParam.GetPollMtByIdx(0);
				
				// 保存新列表
				tTWPollParam.SetPollList(byPolledMtNum, ptMtPollParam);

				//通知所有会控
				m_tTWMutiPollParam.SetTWPollParam(byHduId, byHduChnId, tTWPollParam);
				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(tTWPollParam));
				SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
			}
			break;
		case MCS_MCU_GETHDUPOLLPARAM_REQ:
			{
				u8 byHduId = *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));
				TTvWallPollParam tTWPollParam;
				if(!m_tTWMutiPollParam.GetTWPollParam(byHduId, byChnId, tTWPollParam))
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[ProcMcsMcuHduPollMsg->GETHDUPOLLPARAM_REQ] Cannot Get <EqpId:%d, ChnId:%d> HduPollParam!\n",
						           byHduId, byChnId);
					SendReplyBack(cServMsg, pcMsg->event + 2);
					return;
				}

				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendReplyBack(cServMsg, pcMsg->event + 1);	
			}
			break;
		default:
			break;
	}
	return;
}

/*=============================================================================
  函 数 名： ProcMcsMcuTWPollMsg
  功    能： 电视墙轮询消息处理函数
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::ProcMcsMcuTWPollMsg(const CMessage *pcMsg)
{
	 CServMsg cServMsg(pcMsg->content, pcMsg->length);
  
	 switch (pcMsg->event)
	 {
		case MCS_MCU_STARTTWPOLL_CMD:
			{
				TPeriEqpStatus tTWStatus;
				TTvWallPollParam tTWPollParam = *(TTvWallPollParam *)(cServMsg.GetMsgBody());
				TTvWallPollInfo  tTWPollInfo  = tTWPollParam.GetTWPollInfo();
				TEqp tTvWall       = tTWPollParam.GetTvWall();
				u8   byTWId        = tTWPollParam.GetTvWall().GetEqpId();
				u8   byTWChnId     = tTWPollParam.GetTWChnnl();
				u8   bySchemeIdx   = tTWPollParam.GetSchemeIdx();
				u8   byPolledMtNum = tTWPollParam.GetPolledMtNum();
				u8   byMode        = tTWPollParam.GetMediaMode();
			
				//获取所有轮询终端
				TMtPollParam *ptMtPollParam = tTWPollParam.GetPollMtByIdx(0);
				u8 byPollState   = POLL_STATE_NONE;
				u32 dwTimerIdx    = 0;
				
				if (byTWChnId >= MAXNUM_PERIEQP_CHNNL ||  !(byTWId >= TVWALLID_MIN && byTWId <= TVWALLID_MAX) )
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuTWPollMsg] DEC5<EqpId:%, ChnId:%d>  is invalid!\n", byTWId, byTWChnId);
                    break;
				}
				
				g_cMcuVcApp.GetPeriEqpStatus(byTWId, &tTWStatus);
				TTWMember tTwMem = tTWStatus.m_tStatus.tTvWall.atVideoMt[byTWChnId];

				if(0 != tTwMem.GetConfIdx() && tTwMem.GetConfIdx() != m_byConfIdx)//其他会议的没有权限操作, 本会议的替换
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuTWPollMsg] Confidx %d is already in polling, can't replace it\n", tTwMem.GetConfIdx());
					break;
				}

				if( m_tTWMutiPollParam.GetPollState(byTWId, byTWChnId, byPollState) &&
					POLL_STATE_NONE != byPollState)
				{
					// 防止由于m_tTWMutiPollParam中通道轮询状态没有清空导致轮询失败
					if (tTwMem.byMemberType != TW_MEMBERTYPE_NULL && tTwMem.GetConfIdx() == m_byConfIdx )
					{
						ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuTWPollMsg] tvwall <EqpId:%d, ChnId:%d> is already in polling\n", 
							byTWId, byTWChnId);
						break;
					}					
				}
				
				//设置当前通道轮询信息
				m_tTWMutiPollParam.SetTWPollInfo(byTWId, byTWChnId, tTWPollInfo);
				m_tTWMutiPollParam.InitPollParam(byTWId, byTWChnId, byPolledMtNum, ptMtPollParam);
				// [10/28/2011 liuxu] 此时还不能设置轮询状态, 因为后面还可能轮询失败
				// m_tTWMutiPollParam.SetPollState(byTWId, byTWChnId, POLL_STATE_NORMAL);			
		        
// 				// xsl [8/23/2006] 判断卫星会议是否停止轮询
// 				if (m_tConf.GetConfAttrb().IsSatDCastMode())
// 				{            
// 					//若回传个数已满其轮询终端中有不发码流的终端则拒绝轮询
// 					if (IsOverSatCastChnnlNum(0))
// 					{
// 						//逻辑有误，不会进到 [8/8/2012 chendaiwei]
// // 						for(u8 byIdx = 0; byIdx < byPolledMtNum; byIdx++)
// // 						{
// // 							TMtPollParam *ptMtPoll = NULL;
// // 							if (m_tTWMutiPollParam.GetPollMtByIdx(byTWId, byTWChnId, byIdx, ptMtPoll) &&
// // 								ptMtPoll && m_ptMtTable->GetMtSndBitrate(ptMtPoll->GetMtId()) == 0)
// // 							{
// // 								ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuTWPollMsg-start] over max upload mt num. nack!\n");  
// // 								NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_DCAST_OVERCHNNLNUM);
// // 								return;
// // 							}
// // 						}
// 					}            
// 				}                        

				// [10/28/2011 liuxu] 移到获取轮询终端之前
				//获取定时器索引
				if(!m_tTWMutiPollParam.GetTimerIdx(byTWId, byTWChnId, dwTimerIdx))
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF," [ProcMcsMcuHduPollMsg] Cannot get DEC5<EqpId:%d, ChnId:%d> TimerIdx!\n", byTWId, byTWChnId);
					return;
				}
				
				u8 byCurPollPos = 0;
				m_tTWMutiPollParam.GetCurrentIdx(byTWId, byTWChnId, byCurPollPos);

				//这里用公用函数GetMtTWPollParam
				TMtPollParam *ptCurPollMt = GetMtTWPollParam(byCurPollPos, byTWId, byTWChnId);
				m_tTWMutiPollParam.SetCurrentIdx(byTWId, byTWChnId, byCurPollPos);
				m_tTWMutiPollParam.SetIsStartAsPause(byTWId, byTWChnId, 0);
				
				// [10/28/2011 liuxu] 此时轮询才算成功
				m_tTWMutiPollParam.SetPollState(byTWId, byTWChnId, POLL_STATE_NORMAL);
				
				if (NULL != ptCurPollMt)
				{
					TMt tOrgMt = *ptCurPollMt;   
					u32 dwPollTimer = ptCurPollMt->GetKeepTime();
				
					//通知所有会控
					m_tTWMutiPollParam.SetCurPolledMt(byTWId, byTWChnId, *ptCurPollMt);
					m_tTWMutiPollParam.GetTWPollParam(byTWId, byTWChnId, tTWPollParam);
					tTWPollParam.SetConfIdx(m_byConfIdx);
					m_tTWMutiPollParam.SetTWPollParam(byTWId, byTWChnId, tTWPollParam);

					cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
					SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
					
					ChangeTvWallSwitch(&tOrgMt, byTWId, byTWChnId, TW_MEMBERTYPE_TWPOLL, TW_STATE_START);
					
					ConfStatusChange();
					
					//设置轮询时钟
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, 1000 * dwPollTimer, dwTimerIdx);
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuTWPollMsg-start] SetTimer EventId: %d, Timer:%d, TimerIdx:%d ", 
									MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, dwPollTimer, dwTimerIdx);
				}
				else
				{
					//设置轮询时钟
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, 1000 * 5, dwTimerIdx);
				}            

				// [10/19/2011 liuxu] 不管怎样, 先预占用
				g_cMcuVcApp.GetPeriEqpStatus(byTWId, &tTWStatus);
				tTWStatus.m_tStatus.tTvWall.atVideoMt[byTWChnId].SetConfIdx(m_byConfIdx);
				tTWStatus.m_tStatus.tTvWall.atVideoMt[byTWChnId].byMemberType = TW_MEMBERTYPE_TWPOLL;
                g_cMcuVcApp.SetPeriEqpStatus(byTWId, &tTWStatus);
				
				CServMsg cTvwStatusMsg;
				cTvwStatusMsg.SetMsgBody( (u8*)&tTWStatus, sizeof( tTWStatus ) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cTvwStatusMsg );
			}
			break;

		case MCS_MCU_PAUSETWPOLL_CMD:
			{				
				u8 byTWId =  *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));

				ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "---------DEC5<EqpID:%d, ChnId:%d>---------\n", byTWId, byChnId);
				
				m_tTWMutiPollParam.SetPollState(byTWId, byChnId, POLL_STATE_PAUSE);
				TTvWallPollParam tTWPollParam;
				m_tTWMutiPollParam.GetTWPollParam(byTWId, byChnId, tTWPollParam);
				
				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
				
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byTWId, byChnId, dwTimerIdx) )
				{
					KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
				}		
				
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuTWPollMsg-PAUSE] kill EventId: %d, TimerIdx:%d \n", 
									MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, dwTimerIdx);
			}
			break;

		case MCS_MCU_RESUMETWPOLL_CMD:
			{
				u8 byTWId  = *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));
				m_tTWMutiPollParam.SetPollState(byTWId, byChnId, POLL_STATE_NORMAL);
				TTvWallPollParam tTWPollParam;
				m_tTWMutiPollParam.GetTWPollParam(byTWId, byChnId, tTWPollParam);
				
				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
				m_tTWMutiPollParam.SetIsStartAsPause(byTWId, byChnId, 1);
				
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byTWId, byChnId, dwTimerIdx) )
				{
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, 1000, dwTimerIdx);
				}

				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuTWPollMsg-RESUME] SetTimer EventId: %d, Timer:1000, TimerIdx:%d \n", 
									MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, dwTimerIdx);
			}
			break;

		case MCS_MCU_STOPTWPOLL_CMD:
			{
				u8 byTWId  = *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));

				TPeriEqpStatus tTvwStatus;
				g_cMcuVcApp.GetPeriEqpStatus( byTWId, &tTvwStatus );
				if ( tTvwStatus.m_tStatus.tTvWall.atVideoMt[byChnId].byMemberType != TW_MEMBERTYPE_TWPOLL
					|| tTvwStatus.m_tStatus.tTvWall.atVideoMt[byChnId].GetConfIdx() != m_byConfIdx )
				{
					break;
				}
				
				//由于需要清空上报界面，所以先记录下来用于拆到电视墙交换
				TMt tTWMem = m_tTWMutiPollParam.GetCurPolledMt(byTWId, byChnId);
				
				//清空当前电视墙通道所有轮询信息			
				TMtPollParam tMtPollParam;
                memset(&tMtPollParam, 0, sizeof(tMtPollParam));				
				m_tTWMutiPollParam.SetCurPolledMt(byTWId, byChnId, tMtPollParam);
				m_tTWMutiPollParam.SetPollState(byTWId, byChnId, POLL_STATE_NONE);			
				m_tTWMutiPollParam.SetPollNum(byTWId, byChnId, 0);
				
				TMtPollParam atTWPollParam[MAXNUM_CONF_MT];
				m_tTWMutiPollParam.InitPollParam(byTWId, byChnId, MAXNUM_CONF_MT, atTWPollParam);
				m_tTWMutiPollParam.SetPolledMtNum(byTWId, byChnId, 0);

				TTvWallPollParam tTWPollParam;
				m_tTWMutiPollParam.GetTWPollParam(byTWId, byChnId, tTWPollParam);
				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);				
				
				if (!tTWMem.IsNull())
				{
					ChangeTvWallSwitch(&tTWMem, byTWId, byChnId, TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);
				}else
				{
					g_cMcuVcApp.GetPeriEqpStatus( byTWId, &tTvwStatus );
					if ( tTvwStatus.m_tStatus.tTvWall.atVideoMt[byChnId].byMemberType != TW_MEMBERTYPE_TWPOLL
						|| tTvwStatus.m_tStatus.tTvWall.atVideoMt[byChnId].GetConfIdx() != m_byConfIdx )
					{
						break;
					}

					CServMsg cStatusMsg;
					cStatusMsg.SetMsgBody( (u8*)&tTvwStatus, sizeof( tTvwStatus ) );
					SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cStatusMsg );
				}

				m_tTWMutiPollParam.SetIsStartAsPause(byTWId, byChnId, 0);	
				TTvWallPollParam tTWPollParamNull;				
				m_tTWMutiPollParam.SetTWPollParam(byTWId, byChnId, tTWPollParamNull);
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byTWId, byChnId, dwTimerIdx) )
				{
					KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
				}

				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[ProcMcsMcuTWPollMsg-STOP] kill EventId: %d, TimerIdx:%d \n", 
									MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, dwTimerIdx);
			}
			break;

		case MCS_MCU_GETTWPOLLPARAM_REQ:
			{
				u8 byTWId = *(u8 *)(cServMsg.GetMsgBody());
				u8 byChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8));
				TTvWallPollParam tTWPollParam;
				if(!m_tTWMutiPollParam.GetTWPollParam(byTWId, byChnId, tTWPollParam))
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[ProcMcsMcuHduPollMsg->GETTWPOLLPARAM_REQ] Cannot Get <EqpId:%d, ChnId:%d> HduPollParam!\n",
						byTWId, byChnId);
					SendReplyBack(cServMsg, pcMsg->event + 2);	
					return;
				}
				
				cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				SendReplyBack(cServMsg, pcMsg->event + 1);	
			}
			break;
		default:
			break;
	 }

    return;
}

/*=============================================================================
  函 数 名： ProcHduPollingChangeTimerMsg
  功    能： hdu轮询定时切换处理函数
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  09/04/11    4.6         江乐斌         创建
=============================================================================*/
void CMcuVcInst::ProcHduBatchPollChangeTimerMsg(const CMessage *pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);	

    switch(CurState())
    {
    case STATE_ONGOING:
    {
		if ( POLL_STATE_NONE == m_tHduBatchPollInfo.GetStatus() )
		{
            ConfPrint(LOG_LVL_ERROR, MID_MCU_HDU, "current state is not hdu batch polling! \n" );
			KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
				
		    return;
		}
        
		//StopHduBatchPollSwitch();
       
		// 轮询结束
		if ( ( !HduBatchPollOfOneCycle(FALSE) ) 
			|| ( 0 != m_tHduBatchPollInfo.GetCycles() 
				&& m_tHduBatchPollInfo.GetCurrentCycle() == m_tHduBatchPollInfo.GetCycles() )
			)
		{
			StopHduBatchPollSwitch(TRUE);
			m_tHduBatchPollInfo.SetNull();
			m_tConf.m_tStatus.m_tConfMode.SetHduInBatchPoll( POLL_STATE_NONE );
			TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
			tConfAttrbEx.SetSchemeIdxInBatchPoll( 0 );
			m_tConf.SetConfAttrbEx(tConfAttrbEx);
			m_tHduPollSchemeInfo.SetStatus( POLL_STATE_NONE );
			ConfStatusChange();            // 通知会控会议状态改变

			cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof( m_tHduPollSchemeInfo ) );
			SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );
			
			//通知所有会控
			cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
			if(m_tConf.HasConfExInfo())
			{
				u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
				u16 wPackDataLen = 0;
				PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
				cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
			}
			SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

			KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
		}
		//正常轮询
		else
		{
			cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof(m_tHduPollSchemeInfo) );
            SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );
			
			SetTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER, 1000 * m_tHduBatchPollInfo.GetKeepTime() );
		}

	    break;
	}
	default:
		ConfPrint( LOG_LVL_ERROR, MID_MCU_HDU,  "[ProcHduBatchPollChangeTimerMsg] state(%d) is wrong! \n", CurState() );
	}
	return;
}

/*=============================================================================
  函 数 名： ProcTWPollingChangeTimerMsg
  功    能： 电视墙轮询定时切换处理函数
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::ProcTWPollingChangeTimerMsg(const CMessage *pcMsg)
{
	BOOL32 bClearTwPollStatus = FALSE;
	
	u8 byPollState = 0xff;
	u8 byCurPollPos = 0xff;
	u8 byIsStartAsPause = 0xff;
	u8 byTWId  = 0xff;
	u8 byChnId = 0xff;
	u8 byMode = 0xff;

	u32 byTimerIdx = 0xffff;

	CServMsg cServMsg;

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			byTimerIdx   = *(u32*)pcMsg->content;
			ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP, "OnTWPollChgTimer:EvId:%d, Timerid:%d\n",pcMsg->event, byTimerIdx);
			
			TTvWallPollParam *ptTWPollParam = m_tTWMutiPollParam.GetTWPollParamByTimerIdx((u8)byTimerIdx);
			if (NULL == ptTWPollParam)
			{
				bClearTwPollStatus = TRUE;
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcTWPollingChangeTimerMsg] GetTWPollParamByTimerIdx failed!\n");
				break;
			}
			
			byPollState      = ptTWPollParam->GetPollState();
			byCurPollPos     = ptTWPollParam->GetCurrentIdx();
			byIsStartAsPause = ptTWPollParam->GetIsStartAsPause();
			byTWId           = ptTWPollParam->GetTvWall().GetEqpId();
			byChnId          = ptTWPollParam->GetTWChnnl();
			byMode			= ptTWPollParam->GetMediaMode();
			
			if ( POLL_STATE_NORMAL != byPollState ) 
			{
				if( byPollState != POLL_STATE_PAUSE)
				{
					bClearTwPollStatus = TRUE;
				}
				
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Tvw.(%d, %d) PollState is not running!\n", byTWId, byChnId);
				break;
			}
			
			TPeriEqpStatus tTvwStatus;
			g_cMcuVcApp.GetPeriEqpStatus( byTWId, &tTvwStatus );
			
			TTvwMember tTvwMember;
			if (byTWId >= TVWALLID_MIN && byTWId <= TVWALLID_MAX)
			{		
				tTvwMember = tTvwStatus.m_tStatus.tTvWall.atVideoMt[byChnId];
			}
			else if(byTWId >= HDUID_MIN && byTWId <= HDUID_MAX)
			{
				tTvwMember = tTvwStatus.m_tStatus.tHdu.atVideoMt[byChnId];  
			}
			else
			{
				bClearTwPollStatus = TRUE;
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Tvw.(%d, %d) invalid tvw eqp, over!\n", byTWId, byChnId);
				break;
			}
			
			if (( tTvwMember.byMemberType != TW_MEMBERTYPE_TWPOLL && tTvwMember.byMemberType != TW_MEMBERTYPE_NULL)
				|| (tTvwMember.GetConfIdx() != m_byConfIdx &&  tTvwMember.GetConfIdx() != 0 ))
			{
				bClearTwPollStatus = TRUE;
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Tvw.(%d, %d) is not at polling or not belong to this conf\n", byTWId, byChnId);
				break;
			}
			
			TMtPollParam *ptCurPollMt = NULL;
			
			if ( 0 == byIsStartAsPause)   
			{
				byCurPollPos++;
			}
			else // 由暂停恢复时，接着当前终端轮询   
			{
				m_tTWMutiPollParam.SetIsStartAsPause(byTWId, byChnId, 0);
			}
			
			ptCurPollMt = GetMtTWPollParam(byCurPollPos, byTWId, byChnId);
			
			BOOL32 bStopPoll = FALSE;
			// [2013/01/29 chenbing] 
// 			// 判断卫星会议是否停止轮询
// 			if (m_tConf.GetConfAttrb().IsSatDCastMode())
// 			{
// 				TMt tLastPollMt = m_tTWMutiPollParam.GetCurPolledMt(byTWId, byChnId);
// 				if (!tLastPollMt.IsNull() &&
// 					IsMtSendingVidToOthers(tLastPollMt, FALSE, TRUE, 0) && //上一个终端还在向其他实体发送码流
// 					m_ptMtTable->GetMtSndBitrate(ptCurPollMt->GetMtId()) == 0 && //下一个终端没有在发送码流
// 					IsOverSatCastChnnlNum(0))
// 				{
// 					bStopPoll = TRUE;
// 					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "SatDCastMode Tvw.(%d, %d) poll failed\n", byTWId, byChnId);
// 				}
// 			}
			
			if (NULL != ptCurPollMt && !bStopPoll && !ptCurPollMt->IsNull())
			{
				m_tTWMutiPollParam.SetCurPolledMt(byTWId, byChnId, *ptCurPollMt);
				m_tTWMutiPollParam.SetCurrentIdx(byTWId, byChnId, byCurPollPos);
				
				u32 dwSetTimer = ptCurPollMt->GetKeepTime();
				TMt tSrcOrg  = *ptCurPollMt;
				TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);
				
				TTvWallPollParam tTWPollParam;
				m_tTWMutiPollParam.GetTWPollParam(byTWId, byChnId, tTWPollParam);
				cServMsg.SetMsgBody( (u8*)&tTWPollParam, sizeof(TTvWallPollParam) );
				
				// [9/1/2011 liuxu] VCS会议时单回传下如果上传通道被占用,则不能进墙
				BOOL32 bCanInTvw = TRUE;
				if ( VCS_CONF == m_tConf.GetConfSource() )
				{
					//当下级有当前调度终端，且同一个下级的其它终端想进电视墙，回NACK
					if( !tSrcOrg.IsLocal() 
						&& !IsLocalAndSMcuSupMultSpy(tSrcOrg.GetMcuId()) 
						&& IsMtInMcu(GetLocalMtFromOtherMcuMt(tSrcOrg), m_cVCSConfStatus.GetCurVCMT()) 
						&& !( m_cVCSConfStatus.GetCurVCMT() == tSrcOrg ))
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  
							"[ProcMcsMcuHduPollMsg] CurVCMT(%d.%d) and Mt(%d.%d) is in same smcu,Mt can't in tvwall \n ",
							m_cVCSConfStatus.GetCurVCMT().GetMcuId(),
							m_cVCSConfStatus.GetCurVCMT().GetMtId(),
							tSrcOrg.GetMcuId(),
							tSrcOrg.GetMtId() );
						
						bCanInTvw = FALSE;							
						
						cServMsg.SetEventId( MCU_MCS_ALARMINFO_NOTIF );
						cServMsg.SetErrorCode( ERR_MCU_CONFSNDBANDWIDTHISFULL );	
						SendMsgToAllMcs( MCU_MCS_ALARMINFO_NOTIF, cServMsg );
					}	
				}
				
				if(bCanInTvw)
				{
					if (byTWId >= TVWALLID_MIN && byTWId <= TVWALLID_MAX)
					{		
						SendMsgToAllMcs( MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg );
						ChangeTvWallSwitch(&tSrcOrg, byTWId, byChnId, TW_MEMBERTYPE_TWPOLL, TW_STATE_CHANGE);      
					}
					else if(byTWId >= HDUID_MIN && byTWId <= HDUID_MAX)
					{
						SendMsgToAllMcs( MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg );
						// [2013/03/11 chenbing] HDU多画面目前不支持批量轮询,子通道置0
						ChangeHduSwitch(&tSrcOrg, byTWId, byChnId, 0, TW_MEMBERTYPE_TWPOLL, TW_STATE_START, byMode);    
					}
					else
					{
						ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcHduPollingChangeTimerMsg] UnKnow TWId:%d!\n", byTWId);
					}
				}else  // 把旧的提出墙
				{
					if (byTWId >= TVWALLID_MIN && byTWId <= TVWALLID_MAX)
					{		
						SendMsgToAllMcs( MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg );
						ChangeTvWallSwitch(NULL, byTWId, byChnId, TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);      
					}
					else if(byTWId >= HDUID_MIN && byTWId <= HDUID_MAX)
					{
						SendMsgToAllMcs( MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg );
						// [2013/03/11 chenbing] HDU多画面目前不支持批量轮询,子通道置0
						ChangeHduSwitch(NULL, byTWId, byChnId, 0, TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP, byMode);      
					}
					else
					{
						ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcHduPollingChangeTimerMsg] UnKnow TWId:%d!\n", byTWId);
					}
				}

				SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + byTimerIdx, 1000 * dwSetTimer, byTimerIdx);
				
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_EQP, "TvwPoll: new mt(%d,%d) at poll, remain %u\n", 
					tSrcOrg.GetMcuId(),tSrcOrg.GetMtId(), ptCurPollMt->GetPollNum());
			}
			else
			{
				bClearTwPollStatus = TRUE;
			}
		}
		break;
			
		default:
			break;
	}
	
	if ( bClearTwPollStatus )
	{
		TMt tTWMem = m_tTWMutiPollParam.GetCurPolledMt(byTWId, byChnId);
		
		TMtPollParam tMtPollParam;
		memset(&tMtPollParam, 0, sizeof(tMtPollParam));
		m_tTWMutiPollParam.SetCurPolledMt(byTWId, byChnId, tMtPollParam);
		m_tTWMutiPollParam.SetPollState(byTWId, byChnId, POLL_STATE_NONE);
		
		TTvWallPollParam tTWPollParam;
		m_tTWMutiPollParam.GetTWPollParam(byTWId, byChnId, tTWPollParam);
		
		cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
		
		
		if (byTWId >= TVWALLID_MIN && byTWId <= TVWALLID_MAX)
		{
			SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
			ChangeTvWallSwitch(&tTWMem, byTWId, byChnId, TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);
		}
		else if(byTWId >= HDUID_MIN && byTWId <= HDUID_MAX)
		{
			SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
			// [2013/03/11 chenbing] HDU多画面目前不支持批量轮询,子通道置0
			ChangeHduSwitch(NULL, byTWId, byChnId, 0, TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);
		}
		
		KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + byTimerIdx);
		
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcTWPollingChangeTimerMsg] tvwall[%d,%d] polling finished\n", byTWId, byChnId);
	}

	return;
}

/*====================================================================
    函数名      SendConfExtInfoToMcs
    功能        ：会议扩展信息通知界面(下级会议是否支持多回传)
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 bySrcSsnId mcu和mcu的会话号（即发给那个mcs）
	              0：默认发给所有会控
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    20100517    4.6         pengjie        create
====================================================================*/
void CMcuVcInst::SendConfExtInfoToMcs( u8 bySrcSsnId/*=0*/,u8 byMtId/*=0*/ )
{
	CServMsg cServMsg;

	cServMsg.SetConfIdx( m_byConfIdx );
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetMcuId( LOCAL_MCUID );
	cServMsg.SetEventId( MCU_MCS_CONFEXTINFO_NOTIF );

	u8 byIsSupMultSpy = FALSE;
	TMt tMt;

	if( 0 != byMtId )
	{
		if( m_ptMtTable->GetMtType(byMtId) == MT_TYPE_SMCU )
		{
			tMt = m_ptMtTable->GetMt( byMtId );
			if( IsLocalAndSMcuSupMultSpy( GetMcuIdxFromMcuId(tMt.GetMtId()) ) )
			{			
				byIsSupMultSpy = 1;	
				cServMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
				cServMsg.CatMsgBody( (u8 *)&byIsSupMultSpy, sizeof(u8) );
				if( bySrcSsnId == 0 )
				{
					SendMsgToAllMcs( MCU_MCS_CONFEXTINFO_NOTIF, cServMsg );
				}
				else
				{
					cServMsg.SetSrcSsnId( bySrcSsnId );
					SendMsgToMcs( bySrcSsnId, MCU_MCS_CONFEXTINFO_NOTIF, cServMsg );
				}
			}
		}
	}
	else
	{
		for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
			{			
				if( m_ptMtTable->GetMtType(byLoop) == MT_TYPE_SMCU )
				{				
					tMt = m_ptMtTable->GetMt( byLoop );
					if( IsLocalAndSMcuSupMultSpy( GetMcuIdxFromMcuId(tMt.GetMtId()) ) )
					{			
						byIsSupMultSpy = 1;	
						cServMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
						cServMsg.CatMsgBody( (u8 *)&byIsSupMultSpy, sizeof(u8) );
						if( bySrcSsnId == 0 )
						{
							SendMsgToAllMcs( MCU_MCS_CONFEXTINFO_NOTIF, cServMsg );
						}
						else
						{
							cServMsg.SetSrcSsnId( bySrcSsnId );
							SendMsgToMcs( bySrcSsnId, MCU_MCS_CONFEXTINFO_NOTIF, cServMsg );
						}
					}					
				}
			}
		}
	}
	
	return;
}

/*====================================================================
    函数名      ProcCancelSpyMtMsg
    功能        ：会议取消回传通道终端
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/04/15    4.0         周晶晶         创建
====================================================================*/
void CMcuVcInst::ProcCancelSpyMtMsg( const CMessage * pcMsg )
{
	STATECHECK;

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	if( m_tCascadeMMCU.IsNull() )
	{
		SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcCancelSpyMtMsg] m_tCascadeMMCU is null.so can't Cancel SpyMt.\n" );
        return;
	}

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	// 释放回传通道
	TMtStatus tMtStatus;
	m_ptMtTable->GetMtStatus(m_tCascadeMMCU.GetMtId(), &tMtStatus);
	FreeRecvSpy( tMtStatus.GetVideoMt(), MODE_BOTH );
    
	StopSpyMtCascaseSwitch();
}

/*====================================================================
    函数名      ：ProcMcsMcuRollCallMsg
    功能        ：会议点名控制
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/11/07    4.0         张宝卿         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuRollCallMsg(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    STATECHECK;

    switch(cServMsg.GetEventId())
    {
    case MCS_MCU_STARTROLLCALL_REQ:
        {
            u8 byRollCallMode = *(u8*)cServMsg.GetMsgBody();
            if ( byRollCallMode == ROLLCALL_MODE_NONE ||
                 byRollCallMode >  ROLLCALL_MODE_CALLEE )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_MODE);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] unexpected Rollcall mode.%d\n", byRollCallMode);
                return;
            }

            if ( m_tConf.m_tStatus.IsMustSeeSpeaker() )
            { 
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_MUSTSEESPEAKER);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] can't start rollcall due to conf mode.mustseespeaker\n");
                return;
            }

            // zbq[11/24/2007] 会议轮询的时候不允许开启点名
            if ( m_tConf.m_tStatus.GetPollMode() != CONF_POLLMODE_NONE )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_POLL);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] can't start rollcall due to conf pollmode.%d\n", m_tConf.m_tStatus.GetPollMode());
                return;
            }

            // zbq[03/31/2008] 点名资源预审(FIXME：未占用，可能会被抢)
            u8 byGrpId = 0;
            u8 byEqpId = 0;
            if ( // 新点名逻辑支持本会议内冲混音模式，所以这里必须判断本会议
				// 是否占用了混音器, zgc, 2008-05-22
				m_tConf.m_tStatus.IsNoMixing() &&
				!m_tConf.m_tStatus.IsVACing() )
            {
				TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];
				memset(atAudioTypeDesc, 0, sizeof(atAudioTypeDesc));
				u8 byAudioCapNum = m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);

				byEqpId = g_cMcuVcApp.GetIdleMixer(0, byAudioCapNum, atAudioTypeDesc);
                if (0 == byEqpId)
				{
					cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_NOMIXER);
					SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg]GetIdleMixer() can't find idle mixer!\n");
					return;
				}
				else
				{
					//tianzhiyong 2010/04/15  检查该混音器是否可用
					if (!CheckMixerIsValide(byEqpId))
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg]CheckMixerIsValide() mixer(%d) is invalide!\n",byEqpId);
						cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_NOMIXER);
						SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
						return;
					}
				}
            }

            u8 byIdleVMPNum = 0;
            u8 abyIdleVMPId[MAXNUM_PERIEQP];
            memset( abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
            g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );    
			// BUG10614会议点名被错误拒掉, zgc, 2008-04-28
			// 非VMP模式不需要判断是否有VMP空闲
            // 新点名逻辑支持本会议内切换合成模式，所以这里必须判断是否是本会议占用了画面
            // 合成器，zgc, 2008-05-23
			u8 byVmpCount = GetVmpCountInVmpList();
            if ( byIdleVMPNum == 0 && byRollCallMode == ROLLCALL_MODE_VMP &&
                0 == byVmpCount )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_NOVMP);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] can't start rollcall due to no idle vmp\n");
                return;
            }
            // zgc, 2008-05-29, 检查VMP能力，如果连左右两画面都无法支持，则拒绝点名
            if ( byRollCallMode == ROLLCALL_MODE_VMP && 0 == byVmpCount)
            {
                u16 wError = 0;
                u8 byVmpId = 0;
                BOOL32 bRet = IsMCUSupportVmpStyle(VMP_STYLE_VTWO, byVmpId, EQP_TYPE_VMP, wError);
                if ( !bRet )
                {
                    cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_VMPABILITY);
                    SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] can't start rollcall due to vmp ability is not enough!\n");
                    return;
                }
            }

            SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

            //定制混音、智能混音、语音激励、画面合成（自动和非自动）、选看：给出提示
            if ( m_tConf.m_tStatus.IsSpecMixing() )
            {
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_SPECMIXING);
            }
            if ( m_tConf.m_tStatus.IsAutoMixing() )
            {
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_AUTOMIXING);
            }
            if ( m_tConf.m_tStatus.IsVACing() )
            {
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_VACING);
            }
			// BUG10614会议点名被错误拒掉, zgc, 2008-04-28
			// 非VMP模式不需要提示
            // 修改变更提示, zgc, 2008-05-23
            if ( //m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_CTRL 
                0 < byVmpCount &&
				ROLLCALL_MODE_VMP == byRollCallMode )
            {
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_VMPSTYLE);
            }
            //else if ( m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_AUTO
			//		&& byRollCallMode == ROLLCALL_MODE_VMP )
            //{
            //    NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_VMPSTYLE);
            //}

            // 会议正在合成，点名模式不是VMP模式时，需要提示会停合成, zgc, 2008-05-23
            // 修改变更提示, zgc, 2008-05-23
            if ( 0 < byVmpCount &&
                 ROLLCALL_MODE_VMP != byRollCallMode )
            {
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_VMPING );
            }
			
            BOOL32 bSelExist = FALSE;
            for( u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++ )
            {
                if ( m_tConfAllMtInfo.MtJoinedConf( byMtIdx ) ) 
                {
					// 目前策略：上传终端过适配，设置上传通道为上级MCU的选看源
					// miaoqingsong [20110916] 过滤掉选看源为上传通道中终端的默认选看
					if (!m_tCascadeMMCU.IsNull() && 
						m_tCascadeMMCU.GetMtId() == byMtIdx)
					{
						continue;
					}

                    TMtStatus tMtStatus;
                    if ( m_ptMtTable->GetMtStatus( byMtIdx, &tMtStatus ) &&
                         ( !tMtStatus.GetSelectMt(MODE_AUDIO).IsNull() ||
                           !tMtStatus.GetSelectMt(MODE_VIDEO).IsNull() ) )
                    {
                        bSelExist = TRUE;
                        break;
                    }
                }
            }
            if ( bSelExist )
            {
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_SEL);
            }

            // zbq [11/22/2007] 锁定当前会议和所有的下级会议
            CServMsg cSMsg;
            CMessage cMsg;
            BOOL32 bSMcuExist = FALSE;
            for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
            {
                //zbq[01/09/2009] 合并级联会议请求锁定下级
                if ( m_tConf.GetConfAttrb().IsSupportCascade() &&
                     m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
                     m_ptMtTable->GetMt(byMtId).GetMtType() == MT_TYPE_SMCU )
                {
                    bSMcuExist = TRUE;
                    break;
                }
            }
            if ( bSMcuExist )
            {
                TMcu tMcu;
                cSMsg.SetNoSrc();
                cSMsg.SetEventId(MCS_MCU_LOCKSMCU_REQ);
                cSMsg.SetMsgBody((u8*)&tMcu, sizeof(tMcu));
                
                cMsg.content = cSMsg.GetServMsg();
                cMsg.length = cSMsg.GetServMsgLen();
                ProcMcsMcuLockSMcuReq(&cMsg);                
            }

			if( CONF_LOCKMODE_LOCK != m_tConf.m_tStatus.GetProtectMode() )
			{
				u8 byLockMode = CONF_LOCKMODE_LOCK;
				cSMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
				cSMsg.SetMsgBody((u8*)&byLockMode, sizeof(u8));
				ChangeConfLockMode(cSMsg);
				m_tConf.m_tStatus.m_tConfMode.SetOldLockMode( CONF_LOCKMODE_NONE );
			}
			else
			{
				m_tConf.m_tStatus.m_tConfMode.SetOldLockMode( CONF_LOCKMODE_LOCK );
			}

            

            // zbq [11/22/2007] 置当前会议为无主席会议
            m_tConf.m_tStatus.SetNoChairMode( TRUE );
            if( m_tConf.m_tStatus.IsNoChairMode() && HasJoinedChairman() )
            {
                ChangeChairman( NULL );	
            }

            m_tConf.m_tStatus.SetRollCallMode( byRollCallMode );
            
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] RollCallMode.%d\n", byRollCallMode);

            cServMsg.SetEventId( MCU_MCS_STARTROLLCALL_NOTIF );
            SendMsgToAllMcs( cServMsg.GetEventId(), cServMsg );
        }
    	break;

    case MCS_MCU_CHANGEROLLCALL_REQ:
        {
            if ( m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_NONE ||
                 m_tConf.m_tStatus.GetRollCallMode() >  ROLLCALL_MODE_CALLEE )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_MODE);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] Rollcall mode.%d, can't change caller or callee\n", m_tConf.m_tStatus.GetRollCallMode());
                return;
            }
            if ( cServMsg.GetMsgBodyLen() != sizeof(TMt)*2 )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_MSGLEN);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] unexpected msglen.%d, need.%d\n", cServMsg.GetMsgBodyLen(), sizeof(TMt)*2 );
                return;
            }

			//zhouyiliang 20110115 点名人与备点名人模式，快速切换被点名人的时候，画面合成忙的时候nack
			TPeriEqpStatus tPeriEqpStatus;
			if ( !IsLastMutiSpyRollCallFinished() ) 
			{
				// 添加打印
				m_tConfInStatus.Print();

				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuRollCallMsg]LastRollCall Oprator not finished!\n");
				cServMsg.SetErrorCode(ERR_MCU_LASTROLLCALL_NOTFINISH);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                return;
				
			}

            TMt tNewRollCaller = *(TMt*)cServMsg.GetMsgBody();
            TMt tNewRollCallee = *(TMt*)(cServMsg.GetMsgBody() + sizeof(TMt));

            if ( tNewRollCaller.IsNull() )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_CALLERNULL);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] Caller Null, ignore it\n");
                return;
            }

            if ( tNewRollCallee.IsNull() )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_CALLEENULL);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] Callee Null, ignore it\n");
                return;
            }

            BOOL32 bCallerChged = tNewRollCaller == m_tRollCaller ? FALSE : TRUE;
            BOOL32 bCalleeChged = tNewRollCallee == m_tRollCallee ? FALSE : TRUE;

            if ( bCallerChged && !tNewRollCaller.IsLocal() )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_CALLER_SMCUMT);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] Mt<%d,%d> can't be caller, it belong to SMcu\n", 
                        tNewRollCaller.GetMcuId(), tNewRollCaller.GetMtId() );
                return;
            }

            SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcsMcuRollCallMsg] bCallerChged(%d) bCalleeChged(%d) StopSelectSrc bCallerChged\n",
							bCallerChged,bCalleeChged );

            TMt tOldRollCaller = m_tRollCaller;
            TMt tOldRollCallee = m_tRollCallee;

            m_tRollCaller = tNewRollCaller;
            m_tRollCallee = tNewRollCallee;

			u8 byVmpCount = GetVmpCountInVmpList();
			u8 byVmpId = GetTheMainVmpIdFromVmpList();
			TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
			TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			// miaoqingsong [20110909] 开启点名时，先停当前点名人的所有视频选看，音频选看在调整混音中停
// 			if ( tOldRollCaller.IsNull() || tOldRollCallee.IsNull() )
// 			{
// 				TMtStatus tMtStatus;
// 				m_ptMtTable->GetMtStatus(tNewRollCaller.GetMtId(), &tMtStatus);
// 				
// 				if ( !tMtStatus.GetSelectMt(MODE_VIDEO).IsNull() )
// 				{
// 					StopSelectSrc(tNewRollCaller, MODE_VIDEO, FALSE, FALSE);
// 				}
// 			}

            if ( !bCallerChged && !bCalleeChged )
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] there is no change to caller and callee both\n");
                return;
            }

            // 定义变量byVMPStyle记录当前画面合成的风格
			u8 byVMPStyle = tConfVmpParam.GetVMPStyle();

            // 非VMP点名模式，需要停合成, zgc, 2008-05-23
            if ( ROLLCALL_MODE_VMP != m_tConf.m_tStatus.GetRollCallMode() )
            {
				if (byVmpCount > 0)
				{
					//停止时模板失效
					TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
					if( tConfAttrb.IsHasVmpModule() )
					{
						TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
						tConfAttrib.SetHasVmpModule(FALSE);
						m_tConf.SetConfAttrb( tConfAttrib );
					}
					
					// MCU8000B:在关画面合成之前需要将十六画面合成风格调整两画面合成风格，以便支持非VMP点名模式能够正确地开启混音
					// 20110426_miaoqingsong Bug00048137 MCU8000B开启十六画面合成器后进行非VMP点名模式不能开启混音问题
#ifdef _MINIMCU_
					if ( VMP_STYLE_SIXTEEN == byVMPStyle )
					{
						tConfVmpParam.SetVMPStyle(VMP_STYLE_VTWO);
						NotifyMcsAlarmInfo( 0, ERR_MCU_ROLLCALL_VMPSTYLE );
					}
#endif
					TEqp tTmpVmpEqp;
					for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
					{
						if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
						{
							continue;
						}
						tTmpVmpEqp = g_cMcuVcApp.GetEqp( m_abyVmpEqpId[byIdx] );
						
						if (!tTmpVmpEqp.IsNull() &&
							g_cMcuVcApp.GetPeriEqpStatus(tTmpVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
							tPeriEqpStatus.GetConfIdx() == m_byConfIdx)
						{
							// 直接停止
							g_cEqpSsnApp.SendMsgToPeriEqpSsn( tTmpVmpEqp.GetEqpId(),MCU_VMP_STOPVIDMIX_REQ, 
								(u8*)cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
							ProcVMPStopSucRsp(m_abyVmpEqpId[byIdx]);
						}
					}
					/*TPeriEqpStatus tPeriEqpStatus;
					if (!tVmpEqp.IsNull() &&
						g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
						tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
						tPeriEqpStatus.m_tStatus.tVmp.m_byUseState != TVmpStatus::WAIT_STOP)
					{
						SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, TIMESPACE_WAIT_VMPRSP);
						tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
						g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tPeriEqpStatus);
						g_cEqpSsnApp.SendMsgToPeriEqpSsn( m_tVmpEqp.GetEqpId(),MCU_VMP_STOPVIDMIX_REQ, 
							(u8*)cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
						
						
					}*/
				}
				else //无画面合成时，清m_tVmpEqp，禁止掉线vmp在上线后恢复(Bug00127809)
				{
					m_tVmpEqp.SetNull();
				}
            }

            // zbq [11/23/2007] 给点名人或被点名人发言人位置
            if ( bCallerChged &&
                 ROLLCALL_MODE_CALLEE != m_tConf.m_tStatus.GetRollCallMode() )
            {
				// KDV-BUG2004: 点名人或被点名人被强制为发言人，而在MTC上终端状态未改变
				// zgc, 2008-05-21, 用ChangeSpeaker函数代替
                //m_tConf.SetSpeaker( tNewRollCaller );
				ChangeSpeaker( &tNewRollCaller,FALSE,FALSE,TRUE );
            }
            
            //通知所有会控
			// KDV-BUG2004: 点名人或被点名人被强制为发言人，而在MTC上终端状态未改变
			// zgc, 2008-05-21, 在ChangeSpeaker函数中处理
            //cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
            //SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

			//  [12/19/2009 pengjie] Modify 级联多回传支持，如果要改变被点名人，且他是下级支持多回传的MT，则发Presetin进行握手看是否能够多回传
			if( (bCalleeChged || (bCallerChged && ROLLCALL_MODE_CALLEE != m_tConf.m_tStatus.GetRollCallMode())) && 
				!tNewRollCallee.IsNull() && !tNewRollCallee.IsLocal()  )
			{
				if( IsLocalAndSMcuSupMultSpy(tNewRollCallee.GetMcuId()) )
				{
					TMt tDstMt;
					tDstMt.SetNull();
					TPreSetInReq tSpySrcInitInfo;
					tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt( tNewRollCallee );
					tSpySrcInitInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tCaller = tNewRollCaller;
					tSpySrcInitInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tOldCaller = tOldRollCaller;
					tSpySrcInitInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tOldCallee = tOldRollCallee;
					tSpySrcInitInfo.m_bySpyMode = MODE_VIDEO;  // 点名建立的选看，其实只是视频的选看
					if( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
					{
						tSpySrcInitInfo.m_bySpyMode = MODE_BOTH;  // 点名建立的选看，其实只是视频的选看
					}
					TMcsRegInfo tMcRegInfo;
					if (g_cMcuVcApp.GetMcsRegInfo(cServMsg.GetSrcSsnId(), &tMcRegInfo))
					{
						tSpySrcInitInfo.m_tSpyInfo.m_tSpyRollCallInfo.SetMcIp(tMcRegInfo.GetMcsIpAddr());
						tSpySrcInitInfo.m_tSpyInfo.m_tSpyRollCallInfo.SetMcSSRC(tMcRegInfo.GetMcsSSRC());
					}	
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuRollCallMsg] McsIp.%s, McSSrc.%x!\n", 
												 StrOfIP(tSpySrcInitInfo.m_tSpyInfo.m_tSpyRollCallInfo.GetMcIp()),
												 tSpySrcInitInfo.m_tSpyInfo.m_tSpyRollCallInfo.GetMcSSRC());
					
					tSpySrcInitInfo.SetEvId(cServMsg.GetEventId());

					// [pengjie 2010/9/13] 填目的端能力
					TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tNewRollCallee.GetMcuId()) );
					//zjl20101116 如果当前终端已回传则能力集要与已回传目的能力集取小					
					if(!GetMinSpyDstCapSet(tNewRollCallee, tSimCapSet))
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcsMcuRollCallMsg] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
							tNewRollCallee.GetMcuId(), tNewRollCallee.GetMtId() );
						return;
					}
					
					tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		            // End

					if( !tOldRollCallee.IsNull() )
					{			
						tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tOldRollCallee;
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_VIDEO;
						tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
						//20120821 zhouyiliang 点名与被点名人模式下或者ROLLCALL_MODE_CALLER模式下，选看成功才加上视频
						if (ROLLCALL_MODE_VMP ==  m_tConf.m_tStatus.GetRollCallMode() 
							|| ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() )
						{
							TMtStatus tStatus ;
							if ( m_ptMtTable->GetMtStatus(m_tRollCaller.GetMtId(),&tStatus) )
							{
								//如果选看不成功要减掉（可能需要bas，不成功的情况）
								if ( !(tStatus.GetSelectMt(MODE_VIDEO) == tOldRollCallee) )
								{
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum--;
								}
							}
							else
							{
								StaticLog("[ProcMcsMcuRollCallMsg]get m_tRollCaller mtstatus failed!\n");
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum--;
							}
						}
						
						//加1路监控
						TMt tMcSrc;
						g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcSrc, MCS_ROLLCALL_MCCHLIDX , MODE_VIDEO );
						if( tMcSrc == tOldRollCallee )
						{
							++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
						}
						
						/*	
						for( u8 byChanId = 0;byChanId < MAXNUM_MC_CHANNL;++byChanId )
						{
							g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcSrc, byChanId , MODE_VIDEO );
							if( tMcSrc == tOldRollCallee )
							{
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
							}
							g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcSrc, byChanId , MODE_AUDIO );
							if( tMcSrc == tOldRollCallee )
							{
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
							}
						}*/

						u8 byChlPos = 0;
						if( m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP )
						{
							/*TMtStatus tStatus;
							m_ptMtTable->GetMtStatus( tNewRollCaller.GetMtId(),&tStatus );
							if( tStatus.GetSelectMt(MODE_VIDEO) == tOldRollCallee )
							{
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
							}*/
							byChlPos = tConfVmpParam.GetChlOfMtInMember(tOldRollCallee);
							if(	tConfVmpParam.GetVMPMode() == CONF_VMPMODE_CTRL  &&
								MAXNUM_VMP_MEMBER != byChlPos )
							{
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
							}

						}

						

						//如果时被点名人模式,被点名人是发言人,把发言人跟随的可能性都考虑进去
						//如果发言人由于发言人跟随自动进入了画面合成,可释放视频目的数要加1						
						if( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() &&
							m_tConf.GetSpeaker() == tOldRollCallee
							)
						{							
							byChlPos = tConfVmpParam.GetChlOfMtInMember(tOldRollCallee);
							if(	tConfVmpParam.GetVMPMode() == CONF_VMPMODE_AUTO &&
								MAXNUM_VMP_MEMBER != byChlPos &&				
								tConfVmpParam.GetVmpMember(byChlPos)->GetMemberType() == VMP_MEMBERTYPE_SPEAKER					
								)
							{
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
							}
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_BOTH;
							++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;							


							TPeriEqpStatus tStatus;
							u8 byChnlIdx;
							u8 byEqpId;
							//如果发言人由于发言人跟随自动进入了标清电视墙,可释放音视频目的数要加1
							for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
							{
								if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
								{
									if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus))
									{						
										for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
										{							
											if (TW_MEMBERTYPE_SPEAKER == tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType &&
												tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx() == m_byConfIdx &&
												tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetMcuId() == tOldRollCallee.GetMcuId() &&
												tStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetMtId() == tOldRollCallee.GetMtId()
												)
											{
												++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
												++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
											}
										}
									}
								}
							}

							//发言人跟随时，同步调整HDU中的图像
							u8 byHduChnNum = 0;
							//如果发言人由于发言人跟随自动进入了标清电视墙,可释放视音频目的数要加1
							for (byEqpId = HDUID_MIN; byEqpId <= HDUID_MAX; byEqpId++)
							{
								if(IsValidHduEqp(g_cMcuVcApp.GetEqp(byEqpId)))
								{
									byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byEqpId));
									if (0 == byHduChnNum)
									{						
										continue;
									}

									if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus))
									{						
										for (byChnlIdx = 0; byChnlIdx < byHduChnNum; byChnlIdx++)
										{							
											if (TW_MEMBERTYPE_SPEAKER == tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType &&
												m_byConfIdx == tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx() &&
												tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetMcuId() == tOldRollCallee.GetMcuId() &&
												tStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetMtId() == tOldRollCallee.GetMtId()
												)
											{
												++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
												++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
											}
										}
									}
								}
							}
						}

						
					}
		
					OnMMcuPreSetIn( tSpySrcInitInfo );  	
					//zhouyiliang 20110118 点名下级终端，等下级回ack或nack后才置回true，期间不允许
					//再切换被点名人，防止时序问题
					if (bCalleeChged)
					{
						StaticLog("[ProcMcsMcuRollCallMsg]Start wait lastrollcall fininsh \n") ;
						SetLastMultiSpyRollCallFinish(FALSE);
					}
					
					
					return;
				}
			}

			if ( bCalleeChged &&
                 ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
            {
				// KDV-BUG2004: 点名人或被点名人被强制为发言人，而在MTC上终端状态未改变
				// zgc, 2008-05-21, 用ChangeSpeaker函数代替
                //m_tConf.SetSpeaker( tNewRollCallee );
				ChangeSpeaker( &tNewRollCallee,FALSE,FALSE,TRUE );
            }

            // 停语音激励
            if ( m_tConf.m_tStatus.IsVACing() )
            {
                CServMsg cStopVacMsg;
                cStopVacMsg.SetEventId(MCS_MCU_STOPVAC_REQ);
                MixerVACReq(cStopVacMsg);
            }
			
			//调整混音
            BOOL32 bLocalMixStarted = FALSE;

			if ( m_tConf.m_tStatus.GetMixerMode() != mcuNoMix)
			{	
				BOOL32 bRemoveMixMember = FALSE;
				BOOL32 bLocalAutoMix = FALSE;
				
				//停讨论
				if ( m_tConf.m_tStatus.IsAutoMixing() )
				{
					bRemoveMixMember = TRUE;
					bLocalAutoMix = TRUE;		
					m_tConf.m_tStatus.SetAutoMixing(FALSE);
				}
				
				if (bRemoveMixMember)
				{
					TMt atDstMt[MAXNUM_CONF_MT];
					memset(atDstMt, 0, sizeof(atDstMt));
					u8  byDstMtNum = 0;
					for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
					{
						if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
						{
							TMt tMt = m_ptMtTable->GetMt(byMtId);
							RemoveMixMember(&tMt, FALSE);
							StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
								(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+byMtId), FALSE, MODE_AUDIO);
							
							if (bLocalAutoMix)
							{
								//zjl 20110510 StopSwitchToSubMt 接口重载替换
								//StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);
								atDstMt[byDstMtNum] = tMt;
								byDstMtNum ++;
							}
							
							// 恢复从组播地址接收
							/*if (m_tConf.GetConfAttrb().IsSatDCastMode() && m_ptMtTable->IsMtInMixGrp(byMtId))
							{
								ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_AUDIO);
							}*/
						}
					}

					if (byDstMtNum > 0)
					{
						StopSwitchToSubMt(byDstMtNum, atDstMt, MODE_AUDIO, FALSE);
					}
				}

				if ( bCallerChged  || bCalleeChged)
				{
					//本来就是定制混音
					if ( m_tConf.m_tStatus.IsSpecMixing() )
					{
						//第一次点名，移除当前所有终端
						if ( tOldRollCaller.IsNull() || tOldRollCallee.IsNull())
						{
							for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
							{							
								if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
									m_ptMtTable->IsMtInMixing(byMtId))
								{
									TMt tMt = m_ptMtTable->GetMt(byMtId);
									RemoveSpecMixMember(&tMt, 1, FALSE, FALSE);
								}
							}

						}
					}
				}
				
				m_tConf.m_tStatus.SetSpecMixing(TRUE);
				
				if (bCallerChged && !m_tMixEqp.IsNull())
				{
					if ( !tOldRollCaller.IsNull() )
                    {
						RemoveSpecMixMember(&tOldRollCaller, 1, FALSE, FALSE);
					}

					AddSpecMixMember(&tNewRollCaller, 1, FALSE);
				}

				if (bCalleeChged && !m_tMixEqp.IsNull())
				{
					BOOL32 bInSameSMcu = FALSE;

					if (!tOldRollCallee.IsLocal() && 
						!tNewRollCallee.IsLocal() && 
						tOldRollCallee.GetMcuId() == tNewRollCallee.GetMcuId())
					{
						bInSameSMcu = TRUE;
					}

					BOOL32 bStopNoMix = FALSE;

					if (!bInSameSMcu)
					{
						bStopNoMix = TRUE;
					}
					AddSpecMixMember(&tNewRollCallee, 1, FALSE);
					if ( !tOldRollCallee.IsNull() )
					{
						RemoveSpecMixMember(&tOldRollCallee, 1, FALSE, bStopNoMix);
					}
				}

				// 如果当前本来是讨论模式, 需整理N模式的交换
				if ( bRemoveMixMember )
				{
					// 					for (u8 byMtId = 1; byMtId < MAXNUM_CONF_MT; byMtId++)
					// 					{
					// 						if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
					// 						{
					// 							TMt tMt = m_ptMtTable->GetMt(byMtId);
					// 							if (m_tConf.m_tStatus.IsSpecMixing() && !m_ptMtTable->IsMtInMixing(byMtId))
					// 							{
					// 								SwitchMixMember(&tMt, TRUE);
					// 							}
					// 						}           
					// 					}
					
					if( m_tConf.m_tStatus.IsSpecMixing() )
					{
						SwitchMixMember( TRUE );
					}
                }
			}
			else
			{
				//出于时序问题考虑，只Start一次
				if ( !bLocalMixStarted )
				{
					bLocalMixStarted = TRUE;
					
					CServMsg cMsg;
					cMsg.SetMsgBody((u8*)&tNewRollCaller, sizeof(TMt));
					cMsg.CatMsgBody((u8*)&tNewRollCallee, sizeof(TMt));
					ProcMixStart(cMsg);

				// MCU8000B:会议点名开启混音后如果之前十六画面合成风格，则由两画面风格恢复为原来的十六画面合成风格以避免停画面合成交换不彻底的问题
				// 20110426_miaoqingsong Bug00048137 MCU8000B开启十六画面合成器后进行非VMP点名模式不能开启混音问题
                #ifdef _MINIMCU_
					u8 byVMPStyle = m_tConf.m_tStatus.GetVmpStyle();
					if ( VMP_STYLE_SIXTEEN == byVMPStyle )
					{
						m_tConf.m_tStatus.SetVmpStyle(VMP_STYLE_SIXTEEN);
					}
                #endif
				}                    
            }
			
            
            //调整广播码流
            TVMPParam_25Mem tVmpParam;
            if ( m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP )
            {
				tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
                tVmpParam.SetVMPBrdst(TRUE);
                tVmpParam.SetVMPMode(CONF_VMPMODE_CTRL);
                tVmpParam.SetVMPAuto(FALSE);
                tVmpParam.SetVMPSchemeId(0);
                tVmpParam.SetIsRimEnabled(FALSE);
                
            #ifdef _MINIMCU_
                tVmpParam.SetVMPStyle(VMP_STYLE_VTWO);
            #else
                tVmpParam.SetVMPStyle(VMP_STYLE_HTWO);
                u16 wError = 0;
                BOOL32 bRet;
				if (byVmpCount == 0)
				{
					bRet = IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError);
				} 
				else
				{
					bRet = IsVMPSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, wError);
				}
                if ( !bRet )
                {
                    //NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_STYLECHANGE);           
                    tVmpParam.SetVMPStyle(VMP_STYLE_VTWO);
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] rollcall change to style.%d due to vmp ability is not enough!\n",
                                tVmpParam.GetVMPStyle() );
                }    
            #endif                
            }


            if ( bCallerChged )
            {
                if ( ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() )
                {
					// 点名人广播在ChangeSpeaker中处理，这里只要检查是不是本地就可以了，
					// 按照目前逻辑，这里的打印应该是不可能出现的，zgc, 2008-05-22
                    //if ( tNewRollCaller.IsLocal() )
                    //{
                    //    ChangeVidBrdSrc( &tNewRollCaller );
                    //}
					if ( !tNewRollCaller.IsLocal() )
					{
						ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "New roll caller(Mt.%d) is not local mt, it's impossible\n", tNewRollCaller.GetMtId() );
					}
                }
                else if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
                {
                    u8 byCallerPos = 0;
                    if ( !tOldRollCaller.IsNull() &&
                         tConfVmpParam.GetVMPMode() != CONF_VMPMODE_NONE )
                    {
						/*lint -save -esym(645,tVmpParam)*/
                        byCallerPos = tVmpParam.GetChlOfMtInMember(tOldRollCaller);
						
                    }
                    //旧点名人不存在，占0通道
                    if ( byCallerPos >= MAXNUM_VMP_MEMBER ) 
                    {
                        byCallerPos = 0;
                    }
                    TVMPMember tVmpMember;
                    memset(&tVmpMember, 0, sizeof(tVmpMember));
                    tVmpMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
                    tVmpMember.SetMemberTMt(tNewRollCaller);
                    tVmpMember.SetMemStatus(MT_STATUS_AUDIENCE);
                    tVmpParam.SetVmpMember(byCallerPos, tVmpMember);
                }
            }
            if ( bCalleeChged )
            {
                if ( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
                {
					// 被点名人广播在ChangeSpeaker中处理，这里只要进行回传设置，
					// zgc, 2008-05-22
                    if ( !tNewRollCallee.IsLocal() )
                    {
                        TMcu tSMcu;
                        tSMcu = GetLocalMtFromOtherMcuMt( tNewRollCallee );
                        OnMMcuSetIn(tNewRollCallee, cServMsg.GetSrcSsnId(), SWITCH_MODE_BROADCAST);
                    }
                    //ChangeVidBrdSrc( &tNewRollCallee );
                }
                else if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
                {
                    u8 byCalleePos = 1;
                    if ( !tOldRollCallee.IsNull() &&
                         tConfVmpParam.GetVMPMode() != CONF_VMPMODE_NONE )
                    {
                        byCalleePos = tVmpParam.GetChlOfMtInMember(GetLocalMtFromOtherMcuMt(tOldRollCallee));
                    }
                    
                    //旧点名人不存在，占1通道
                    if ( byCalleePos >= MAXNUM_VMP_MEMBER )
                    {
                        byCalleePos = 1;
                    }
                    TVMPMember tVmpMember;
                    memset(&tVmpMember, 0, sizeof(tVmpMember));
                    tVmpMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
                    tVmpMember.SetMemberTMt(GetLocalMtFromOtherMcuMt(tNewRollCallee));
                    tVmpMember.SetMemStatus(MT_STATUS_AUDIENCE);
                    tVmpParam.SetVmpMember(byCalleePos, tVmpMember);
                }
            }
            if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
            {
                if ( CONF_VMPMODE_CTRL == tConfVmpParam.GetVMPMode() )
                {
                    //TPeriEqpStatus tPeriEqpStatus;
                    /*g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
                    tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
                    g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  */
//                     
                    //ChangeVmpParam(&tVmpParam);
					// xliang [1/6/2009] 区分新老VMP设VMP param
					AdjustVmpParam(tVmpEqp.GetEqpId(), &tVmpParam);

                }
                else if ( CONF_VMPMODE_AUTO == tConfVmpParam.GetVMPMode() )
                {
                    /*g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
                    tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
                    g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  */
                    
                    //ChangeVmpParam(&tVmpParam);
					// xliang [1/6/2009] 区分新老VMP设VMP param
					AdjustVmpParam(tVmpEqp.GetEqpId(), &tVmpParam);
					
                    ConfModeChange();
                }
                else
                {
					// 此处不设置vmpid，需自己找空闲vmp
                    CServMsg cMsg;
                    cMsg.SetEventId(MCS_MCU_STARTVMP_REQ);
                    cMsg.SetMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
					
                    VmpCommonReq(cMsg);
                }
            }

            //调整选看

            // zbq [11/24/2007] VMP时，开启点名，VMP模式，需手动清当前视频选看；其他情况下的选看都会自动被冲掉.
            // 将除点名人和被点名人之外的其他所有终端的选看全停掉
			if ( ( tOldRollCaller.IsNull() || tOldRollCallee.IsNull() ) &&
                 ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
            {
                for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
                {
					// miaoqingsong [20110916] 过滤掉上级MCU选看上传通道终端的选看
					if (!m_tCascadeMMCU.IsNull() && 
						m_tCascadeMMCU.GetMtId() == byMtId)
					{
						continue;
					} 
 
                    if ( m_tConfAllMtInfo.MtJoinedConf( byMtId ) &&
                         byMtId != tNewRollCaller.GetMtId() &&
                         byMtId != tNewRollCallee.GetMtId() ) 
                    {
                        TMtStatus tMtStatus;
                        if ( m_ptMtTable->GetMtStatus( byMtId, &tMtStatus ) &&
                             !tMtStatus.GetSelectMt(MODE_VIDEO).IsNull() )
                        {
                            TMt tMt = m_ptMtTable->GetMt(byMtId);
                            StopSelectSrc(tMt, MODE_VIDEO);
                        }
                    }
                }
            }

            BOOL32 bSelAdjusted = FALSE;

            if ( bCallerChged )
            {
                if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
                {
                    if ( !tOldRollCaller.IsNull() )
                    {
                        StopSelectSrc(tOldRollCaller, MODE_VIDEO);   
						//zjl切换一次点名人，拆掉前一个点名人的桥交换
						if (!(tOldRollCaller == GetSecVidBrdSrc()))
						{
							g_cMpManager.RemoveSwitchBridge(tOldRollCaller, 0, MODE_VIDEO);
						}
                    }
					//zyl　20121106第一次点名，bCallerChged和bCalleeChged都为true,要停掉点名前的选看
					if (bCalleeChged)
					{
						StopSelectSrc(tNewRollCaller, MODE_VIDEO,FALSE,FALSE);  
					}

					//点名人改变了，先停到被点名人的选看，释放资源
					if( tNewRollCallee.IsLocal() )
					{
						StopSelectSrc(tNewRollCallee, MODE_VIDEO,FALSE,FALSE); 
					}

					if( !tNewRollCallee.IsLocal() && !IsLocalAndSMcuSupMultSpy(tNewRollCallee.GetMcuIdx()))
					{
						OnMMcuSetIn( tNewRollCallee, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
					}
                    
                    //选看失败，恢复看广播，可能看vmp或看自己
					//点名人选看被点名人
                    if (!ChangeSelectSrc( tNewRollCallee, tNewRollCaller, MODE_VIDEO))
                    {
                        RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
                    }
					else
					{
						if( !tNewRollCallee.IsLocal() && IsLocalAndSMcuSupMultSpy(tNewRollCallee.GetMcuId()) )
						{
							s16 swIndex = m_cSMcuSpyMana.FindSpyMt(tNewRollCallee);
							if( -1 != swIndex )
							{
								m_cSMcuSpyMana.IncSpyModeDst(swIndex, MODE_VIDEO, 1 );
							}
						}						
					}
					
					//被点名人选看点名人
                    if ( tNewRollCallee.IsLocal() )
                    {
                        if (!ChangeSelectSrc(tNewRollCaller, tNewRollCallee, MODE_VIDEO))
                        {
                            RestoreRcvMediaBrdSrc(tNewRollCallee.GetMtId(), MODE_VIDEO);
                        }
                    }
                    bSelAdjusted = TRUE;
                }
                else if ( ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() )
                {
                    if ( !tOldRollCaller.IsNull() )
                    {
                        StopSelectSrc(tOldRollCaller, MODE_VIDEO);                    
                    }
					//zyl　20121106第一次点名，bCallerChged和bCalleeChged都为true,要停掉点名前的选看
					if ( bCalleeChged )
					{
						StopSelectSrc(tNewRollCaller, MODE_VIDEO);  
					}
                    if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
                    {
                        RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
                    }
					else
					{
						if( !tNewRollCallee.IsLocal() && IsLocalAndSMcuSupMultSpy(tNewRollCallee.GetMcuId()) )
						{
							s16 swIndex = m_cSMcuSpyMana.FindSpyMt(tNewRollCallee);
							if( -1 != swIndex )
							{
								m_cSMcuSpyMana.IncSpyModeDst(swIndex, MODE_VIDEO, 1 );
							}
						}
					}
                }
                else // ROLLCALL_MODE_CALLEE
                {
					//zjj20091031
					if(  tNewRollCallee.IsLocal() )
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcsMcuRollCallMsg] bCallerChged(%d) bCalleeChged(%d) StopSelectSrc(tNewRollCallee)\n",
							bCallerChged,bCalleeChged );
						//20100708_tzy 当被点名人为下级终端时，不需要停选看，因为此时下级各终端通过上级广播收看被点名人
						if( tNewRollCallee.IsLocal() )
						{
							StopSelectSrc(tNewRollCallee, MODE_VIDEO,FALSE ,FALSE);
						}	
					}

                    if ( tNewRollCallee.IsLocal() )
                    {
                        if (!ChangeSelectSrc(tNewRollCaller, tNewRollCallee, MODE_VIDEO))
                        {
                            RestoreRcvMediaBrdSrc(tNewRollCallee.GetMtId(), MODE_VIDEO);
                        }
                    }
                }
            }	


            if ( bCalleeChged )
            {
				if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
                {
					if( bCallerChged &&
						!tOldRollCaller.IsNull() &&
						!tNewRollCaller.IsNull() 
						)
					{						
						StopSelectSrc(tNewRollCaller, MODE_VIDEO,FALSE,FALSE);
					}
					
					if( !bCallerChged )
					{
						StopSelectSrc(tNewRollCaller, MODE_VIDEO,FALSE,FALSE);
					}

                    if ( tOldRollCallee.IsLocal() &&
                        !tOldRollCallee.IsNull() )
                    {
                        StopSelectSrc(tOldRollCallee, MODE_VIDEO);
						//zjl切换一次被点名人，拆掉前一个被点名人的桥交换
						if (!(tOldRollCallee == GetSecVidBrdSrc()))
						{
							g_cMpManager.RemoveSwitchBridge(tOldRollCallee, 0, MODE_VIDEO);
						}
                    }
                    if ( !bSelAdjusted )
                    {
                        if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
                        {
                            RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
                        }
                        if ( tNewRollCallee.IsLocal() )
                        {
                            if (!ChangeSelectSrc(tNewRollCaller, tNewRollCallee, MODE_VIDEO))
                            {
                                RestoreRcvMediaBrdSrc(tNewRollCallee.GetMtId(), MODE_VIDEO);
                            }
                        }
                    }
                }
                else if ( ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() )
                {
					//zjj20091031 
					if( !bCallerChged )
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcsMcuRollCallMsg] bCallerChged(%d) bCalleeChged(%d) StopSelectSrc(tOldRollCaller)\n",
							bCallerChged,bCalleeChged );
						StopSelectSrc(tOldRollCaller, MODE_VIDEO,FALSE ,FALSE);
					}

					if( !tNewRollCallee.IsLocal() )
					{						
						OnMMcuSetIn( tNewRollCallee, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
					}

					if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
                    {
                        RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
                    }
                }
                else // ROLLCALL_MODE_CALLEE
                {
                    if ( tOldRollCallee.IsLocal() &&
                         !tOldRollCallee.IsNull() )
                    {
                        StopSelectSrc(tOldRollCallee, MODE_VIDEO);
                    }
                    if ( tNewRollCallee.IsLocal() )
                    {
                        if (!ChangeSelectSrc(tNewRollCaller, tNewRollCallee, MODE_VIDEO))
                        {
                            RestoreRcvMediaBrdSrc(tNewRollCallee.GetMtId(), MODE_VIDEO);
                        }
                    }
                }
            }

            cServMsg.SetMsgBody( (u8*)&m_tRollCaller, sizeof(TMt) );
            cServMsg.CatMsgBody( (u8*)&tNewRollCallee, sizeof(TMt) );
            cServMsg.SetEventId( MCU_MCS_CHANGEROLLCALL_NOTIF );
            SendMsgToAllMcs( cServMsg.GetEventId(), cServMsg );

            ConfStatusChange();
        }
    	break;

    case MCS_MCU_STOPROLLCALL_REQ:
        {
            if ( m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_NONE )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_MODE);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] RollCall mode.%d already\n", m_tConf.m_tStatus.GetRollCallMode());
                return;
            }
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
            RollCallStop( cServMsg );
        }
        break;

    default:
        break;
    }

    return;

	/*lint -restore*/
}

/*====================================================================
    函数名      ：RollCallStop
    功能        ：会议点名停止
    算法实现    ：
    引用全局变量：
    输入参数说明：CServMsg &cServMsg
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    12/05/07    4.0         张宝卿         创建
====================================================================*/
void CMcuVcInst::RollCallStop( CServMsg &cServMsg )
{
    //zbq [11/22/2007] 解除会议锁定
	if( CONF_LOCKMODE_LOCK != m_tConf.m_tStatus.m_tConfMode.GetOldLockMode() )
	{
		CServMsg cSMsg;
		u8 byLockMode = CONF_LOCKMODE_NONE;
		cSMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
		cSMsg.SetMsgBody((u8*)&byLockMode, sizeof(u8));
		ChangeConfLockMode(cSMsg);
		m_tConf.m_tStatus.m_tConfMode.SetOldLockMode( CONF_LOCKMODE_NONE );
	}
    
    //恢复主席模式
    m_tConf.m_tStatus.SetNoChairMode( FALSE );
    
    //zbq[11/24/2007] 点名真正开启过，清理点名人和被点名人
    if ( !m_tRollCaller.IsNull() || !m_tRollCallee.IsNull() )
    {
        //停混音
        CServMsg cMsg;
        ProcMixStop(cMsg, TRUE);
        
        //停广播
        if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
        {
            CServMsg cSendMsg;
            cSendMsg.SetConfIdx( m_byConfIdx );
            cSendMsg.SetConfId( m_tConf.GetConfId() );
            cSendMsg.SetEventId(MCS_MCU_STOPVMP_REQ);
            VmpCommonReq(cSendMsg);

			//退出VMP时，需要清掉点名人和被点名人到桥的交换
            //g_cMpManager.RemoveSwitchBridge(m_tRollCaller, 0);

			//[2012/9/13 zhangli]如果被点名人正在双广播则不拆视频桥 
			if (!(m_tRollCallee == GetSecVidBrdSrc()))
			{
				g_cMpManager.RemoveSwitchBridge(m_tRollCallee, 0);
			}
			g_cMpManager.RemoveSwitchBridge(m_tRollCallee, 0, MODE_AUDIO);
        }
        RestoreAllSubMtJoinedConfWatchingSrcMt(GetVidBrdSrc(), MODE_VIDEO);
        
        //停选看
        StopSelectSrc(m_tRollCaller, MODE_BOTH);
        if ( m_tRollCallee.IsLocal() )
        {
            StopSelectSrc(m_tRollCallee, MODE_BOTH);
        }

		/*if ( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
		{
			FreeRecvSpy(m_tRollCallee, MODE_VIDEO);
		}*/
		TMt tMcSrc;
		if( 0 != cServMsg.GetSrcSsnId() )
		{
			g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcSrc, MCS_ROLLCALL_MCCHLIDX , MODE_VIDEO );
			if( tMcSrc == m_tRollCallee )
			{
				StopSwitchToMc( cServMsg.GetSrcSsnId(), MCS_ROLLCALL_MCCHLIDX, TRUE, MODE_VIDEO );
			}
		}
		else
		{
			for( u8 byMcId = 1;byMcId <= MAXNUM_MCU_MC;++byMcId )
			{
				g_cMcuVcApp.GetMcSrc( byMcId, &tMcSrc, MCS_ROLLCALL_MCCHLIDX , MODE_VIDEO );
				if( tMcSrc == m_tRollCallee )
				{				
					StopSwitchToMc( byMcId, MCS_ROLLCALL_MCCHLIDX, TRUE, MODE_VIDEO );
				}
				g_cMcuVcApp.GetMcSrc( byMcId, &tMcSrc, MCS_ROLLCALL_MCCHLIDX , MODE_AUDIO );
				if( tMcSrc == m_tRollCallee )
				{
					StopSwitchToMc( byMcId, MCS_ROLLCALL_MCCHLIDX, TRUE,  MODE_AUDIO );
				}				
			}
		}
		
		
		
		/*for( u8 byChanId = 0;byChanId < MAXNUM_MC_CHANNL;++byChanId )
		{
			g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcSrc, byChanId , MODE_VIDEO );
			if( tMcSrc == m_tRollCallee )
			{				
				StopSwitchToMc( cServMsg.GetSrcSsnId(), byChanId, TRUE, MODE_VIDEO );
			}
			g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcSrc, byChanId , MODE_AUDIO );
			if( tMcSrc == m_tRollCallee )
			{
				StopSwitchToMc( cServMsg.GetSrcSsnId(), byChanId, TRUE,  MODE_AUDIO );
			}
			
		}*/
        
        m_tRollCaller.SetNull();
        m_tRollCallee.SetNull();                
    }
    m_tConf.m_tStatus.SetRollCallMode(ROLLCALL_MODE_NONE);
    
    cServMsg.SetEventId( MCU_MCS_STOPROLLCALL_NOTIF );
    SendMsgToAllMcs( cServMsg.GetEventId(), cServMsg );

    return;
}

/*====================================================================
    函数名      ：DaemonProcMcsMcuListAllConfReq
    功能        ：查询会议信息处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/26    1.0         LI Yi         创建
    04/01/12    3.0         胡昌威         修改
====================================================================*/
void CMcuVcInst::DaemonProcMcsMcuListAllConfReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	NtfMcsMcuCurListAllConf(cServMsg);

	return;
}

/*====================================================================
函数名      ：SendExtMcuStatus
功能        ： 发送额外的MCU状态信息告知界面，针对HDU扩容使用
算法实现    ：
引用全局变量：
输入参数说明：byCurIndex 新增单板起始
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/09/09              倪志俊		  创建
====================================================================*/
void CMcuVcInst::SendExtMcuStatusNotify(u8 byMcsId, u8 byCurIndex, BOOL32 bIsSendAll /*= False*/ )
{
	CServMsg cExtMcuStatusMsg;
	
	u8		byExtPeriEqpNum = 0;	                       
	TEqp	atPeriEqp[MAXNUM_MCU_PERIEQP - MAXNUM_OLDMCU_PERIEQP];	    
	u8      byEqpOnline[MAXNUM_MCU_PERIEQP -MAXNUM_OLDMCU_PERIEQP];     
	u32     dwPeriEqpIpAddr[MAXNUM_MCU_PERIEQP - MAXNUM_OLDMCU_PERIEQP];  
	
	if ( !g_cMcuVcApp.GetMcuExtCurStatus(byCurIndex+1, byExtPeriEqpNum, atPeriEqp, byEqpOnline, dwPeriEqpIpAddr) )
	{
		OspPrintf(TRUE,FALSE, "[SendExtMcuStatusNotify]GetMcuExtCurStatus failed\n");
		return;
	}
	
	if ( byExtPeriEqpNum >0 )
	{
		cExtMcuStatusMsg.SetMsgBody(&byExtPeriEqpNum,sizeof(u8));
		u8 nLoop = 0;
		for ( nLoop = 0; nLoop < byExtPeriEqpNum; nLoop++ )
		{
			cExtMcuStatusMsg.CatMsgBody( (u8*)atPeriEqp + nLoop*(sizeof(TEqp)), sizeof(TEqp) );
		}
		
		for ( nLoop = 0; nLoop < byExtPeriEqpNum; nLoop++ )
		{
			cExtMcuStatusMsg.CatMsgBody( (u8*)byEqpOnline + nLoop*(sizeof(u8)), sizeof(u8) );
		}
		for ( nLoop = 0; nLoop < byExtPeriEqpNum; nLoop++ )
		{
			cExtMcuStatusMsg.CatMsgBody( (u8*)atPeriEqp + nLoop*(sizeof(u32)), sizeof(u32) );	
		}
		
		if (bIsSendAll)
		{
			SendMsgToAllMcs(MCU_MCS_MCUEXTSTATUS_NOTIFY , cExtMcuStatusMsg );
		}
		else
		{
			SendMsgToMcs(byMcsId, MCU_MCS_MCUEXTSTATUS_NOTIFY , cExtMcuStatusMsg );
		}
	}
	return;
}

/*====================================================================
    函数名      ：DaemonProcMcsMcuGetMcuStatusReq
    功能        ：查询MCU状态处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/20    1.0         LI Yi         创建
	03/06/10	1.2			LI Yi         从MC会话实例中转移过来
    03/11/28    3.0         胡昌威         修改
====================================================================*/
void CMcuVcInst::DaemonProcMcsMcuGetMcuStatusReq( const CMessage * pcMsg )
{
   	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	TMcuStatus tMcuStatus;
	if (TRUE == g_cMcuVcApp.GetMcuCurStatus(tMcuStatus))
	{
		cServMsg.SetMsgBody((u8 *)&tMcuStatus, sizeof(tMcuStatus));
		//Ack 
		if (MCS_MCU_GETMCUSTATUS_REQ == pcMsg->event)
		{               
			SendMsgToMcs(cServMsg.GetSrcSsnId(), pcMsg->event+1, cServMsg);
		}	
		//notify
		SendMsgToMcs(cServMsg.GetSrcSsnId(), MCU_MCS_MCUSTATUS_NOTIF, cServMsg );
		
		//ext notify for HDU扩容
		SendExtMcuStatusNotify( cServMsg.GetSrcSsnId(), MAXNUM_OLDMCU_PERIEQP );

	}
	else
	{
		//Nack
		SendMsgToMcs(cServMsg.GetSrcSsnId(), pcMsg->event+2, cServMsg);
	}

	return;
}

/*====================================================================
    函数名      ：DaemonProcMcsMcuStopSwitchMcReq
    功能        ：停止向会议控制台交换处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/20    1.0         LI Yi         创建
	04/05/17    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::DaemonProcMcsMcuStopSwitchMcReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	g_cMcuVcApp.BroadcastToAllConf( pcMsg->event, pcMsg->content, pcMsg->length );

	//Ack
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
}

/*=============================================================================
    函 数 名： DaemonProcMcsMcuStopSwitchMcReq
    功    能： 停止向电视墙交换处理函数
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/02/25  3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::DaemonProcMcsMcuStopSwitchTWReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	g_cMcuVcApp.BroadcastToAllConf( pcMsg->event, pcMsg->content, pcMsg->length );

	//Ack
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
}

/*====================================================================
    函数名      ：ProcMcStopSwitch
    功能        ：停止向会议控制台交换交换处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/05/17    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMcStopSwitch( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8  byChannelNum;
	u8	byInstId;
	TLogicalChannel tLogicalChannel;

	switch( cServMsg.GetEventId() )
	{
	case MCS_MCU_STOPSWITCHMC_REQ:
		{
			TSwitchInfo tSwtichInfo = *(TSwitchInfo*)cServMsg.GetMsgBody();
			StopSwitchToMc( cServMsg.GetSrcSsnId(), cServMsg.GetChnIndex(), TRUE, tSwtichInfo.GetMode());
		}		
		break;
	case MCU_MCSDISCONNECTED_NOTIF:
		{
			byInstId = *( u8 * )cServMsg.GetMsgBody();
			if( g_cMcuVcApp.GetMcLogicChnnl( byInstId, MODE_VIDEO, &byChannelNum, &tLogicalChannel ) )
			{
				while( byChannelNum-- != 0 )
				{
					StopSwitchToMc( byInstId, byChannelNum, FALSE, MODE_BOTH, FALSE );
					StopSwitchToMc( byInstId, byChannelNum, FALSE, MODE_SECVIDEO, FALSE );
				}
			}
			StopSwitchToMonitor(byInstId, TRUE);

			// 若有在访问vrs文件列表，需挂断vrs实体
			if (m_tPlayEqpAttrib.GetListRecordSrcSsnId() != 0
				&& m_tPlayEqpAttrib.GetListRecordSrcSsnId() == cServMsg.GetSrcSsnId())
			{
				if (m_tPlayEqp.GetType() == TYPE_MT && m_tPlayEqp.GetMtType() == MT_TYPE_VRSREC)
				{
					if (m_ptMtTable->GetRecChlState(m_tPlayEqp.GetMtId()) != TRecChnnlStatus::STATE_IDLE
						&& m_ptMtTable->GetRecChlState(m_tPlayEqp.GetMtId()) != TRecChnnlStatus::STATE_CALLING)
					{
						ReleaseVrsMt(m_tPlayEqp.GetMtId(), 0, FALSE);
					}
				}
			}
		}
		break;
    default:
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "ProcMcStopSwitch: unexpected msg %d<%s> received !\n", cServMsg.GetEventId(), OspEventDesc(cServMsg.GetEventId()) );
        break;
	}
}

/*=============================================================================
    函 数 名： ProcMcsMcuStartSwitchTWReq
    功    能： 开始电视墙通道交换处理函数
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/02/25  3.6			万春雷                  创建
    2005/05/23  3.6			libo                  三种跟随方式的实现
=============================================================================*/
void CMcuVcInst::ProcMcsMcuStartSwitchTWReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    StartSwitchTWReq(cServMsg);
}

//4.6.1 新加  jlb
/*=============================================================================
    函 数 名： ProcMcsMcuStopSwitchHduReq
    功    能： 停止HDU通道交换处理函数
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
 2009/01/17     4.6         江乐斌                 创建       
=============================================================================*/
void CMcuVcInst::ProcMcsMcuStopSwitchHduReq( const CMessage * pcMsg )      
{
	if (CurState() != STATE_ONGOING)
    {
        return;
    }
	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	THduSwitchInfo tHduSwitchInfo;
	TMt	tEqp;
	
	tHduSwitchInfo = *( THduSwitchInfo * )cServMsg.GetMsgBody();
	tEqp = tHduSwitchInfo.GetDstMt();

	// [2013/03/11 chenbing]  
	u8 byHduSubChnId = tHduSwitchInfo.GetDstSubChn();
	 
	// Nack
	if(!IsValidHduEqp(tEqp))
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[CMcuVcInst]::ProcMcsMcuStopSwitchHduReq: Wrong PeriEquipment type %u or it has not registered!\n", 
			tEqp.GetEqpType() );
		cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	TMt tRemoveMt;
    TPeriEqpStatus tHduStatus;
    u8             byHduId = 0;
    u8             byChnlNo = 0;
    byHduId = tEqp.GetEqpId();
    byChnlNo   = tHduSwitchInfo.GetDstChlIdx();
    g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
	tRemoveMt = tHduStatus.m_tStatus.tHdu.GetChnMt(byChnlNo, byHduSubChnId);

	if (tRemoveMt.GetType() != TYPE_MCUPERI)
	{
		if (tRemoveMt.GetConfIdx() != m_byConfIdx)
		{
			return;
		}
	}

	//lukunpeng 2010/07/15 为了统一管理，直接调用ChangeHduSwitch
	ChangeHduSwitch(NULL, byHduId, byChnlNo, byHduSubChnId, tHduSwitchInfo.GetMemberType(), TW_STATE_STOP);

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	return;
}

/*=============================================================================
    函 数 名： ProcMcsMcuChangeHduVolume
    功    能： 设置hdu音量
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
 2009/01/17     4.6         江乐斌                 创建       
=============================================================================*/
void CMcuVcInst::ProcMcsMcuChangeHduVolume( const CMessage * pcMsg ) 
{
	if ( NULL == pcMsg )
	{
		ConfPrint( LOG_LVL_ERROR, MID_MCU_HDU, "[ProcMcsMcuChangeHduVolume] pcMsg is Null!\n");
		return;
	}
	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    THduVolumeInfo tHduVolumeInfo = *(THduVolumeInfo*)cServMsg.GetMsgBody();
    
	TPeriEqpStatus tHduStatus;
	g_cMcuVcApp.GetPeriEqpStatus( tHduVolumeInfo.GetEqpId(), &tHduStatus );

	const u8 byChnlIdx = tHduVolumeInfo.GetChnlIdx();

	// [10/24/2011 liuxu] 本会议设置空的电视墙通道, 要预占用
	//yrl20131024该段注掉，设静音不应该占用
//     if ( 0 == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx() && tHduVolumeInfo.GetIsMute() )
//     {
//         tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetConfIdx( m_byConfIdx );
//     }

    if ( m_byConfIdx != tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx()
		&& tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx() != 0)
    {
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[CMcuVcInst]::ProcMcsMcuChangeHduVolume: Can not change hdu volume or set mute with different ConfIdx\n");
        cServMsg.SetErrorCode( ERR_MCU_HDUCHANGEVOLUME );
		SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 2, cServMsg );
		return;
    }

	// [10/24/2011 liuxu] 取消静音时, 如果此时通道为空, 则应该释放预占用
	//yrl20131028该段同上注掉
// 	if ( !tHduVolumeInfo.GetIsMute() 
// 		&& tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].IsNull()
// 		&& m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx()
// 		&& tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType == TW_MEMBERTYPE_NULL)
// 	{
// 		tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetConfIdx( 0 );
// 	}

    tHduStatus.m_tStatus.tHdu.atHduChnStatus[tHduVolumeInfo.GetChnlIdx()].SetIsMute( tHduVolumeInfo.GetIsMute() );
    tHduStatus.m_tStatus.tHdu.atHduChnStatus[tHduVolumeInfo.GetChnlIdx()].SetVolume( tHduVolumeInfo.GetVolume() );

	g_cMcuVcApp.SetPeriEqpStatus( tHduVolumeInfo.GetEqpId(), &tHduStatus );

    SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 1, cServMsg );

    SendMsgToEqp( tHduVolumeInfo.GetEqpId(), MCU_HDU_CHANGEVOLUME_CMD, cServMsg );

	cServMsg.SetMsgBody( (u8*)&tHduStatus, sizeof( tHduStatus ) );
	SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );

    return;
}

/*=============================================================================
    函 数 名： ProcMcsMcuStartSwitchHduReq
    功    能： 开始HDU通道交换处理函数
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
 2009/01/17     4.6         江乐斌                 创建       
=============================================================================*/
void CMcuVcInst::ProcMcsMcuStartSwitchHduReq( const CMessage * pcMsg )     
{
    if ( NULL == pcMsg )
    {
		ConfPrint( LOG_LVL_ERROR, MID_MCU_HDU,  "[ProcMcsMcuStartSwitchHduReq] pcMsg is NULL!\n");
		return;
    }

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	if (MCUVC_STARTSWITCHHDU_NOTIFY ==  pcMsg->event)
	{
		cServMsg.SetMsgBody(pcMsg->content, sizeof( THduSwitchInfo ));
	}

	StartSwitchHduReq(cServMsg);
}
/*=============================================================================
    函 数 名： StartSwitchHduReq
    功    能： 开始hdu通道交换处理函数
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/09/15  4.0			libo                  创建
=============================================================================*/
void CMcuVcInst::StartSwitchHduReq(CServMsg & cServMsg)
{
	// 取消息内容
	const THduSwitchInfo &tHduSwitchInfo = *(THduSwitchInfo*)cServMsg.GetMsgBody();

    switch(CurState())
    {
    case STATE_ONGOING:
	{
		TMt	 tSrcMt					= tHduSwitchInfo.GetSrcMt();						// 源
		
		const TMt	 tDstMt			= tHduSwitchInfo.GetDstMt();
		
		const u8   byHduId			= tDstMt.GetEqpId();								// 目的Hdu的Id
		const u8   byDstChnnlIdx	= tHduSwitchInfo.GetDstChlIdx();					// 目的Hdu的通道
		u8   byHduSubChnId	= 0;												// HDU子通道
		
        //类型不对或者在本MCU上未登记
        if (!IsValidHduEqp(tDstMt) || !g_cMcuVcApp.IsPeriEqpConnected(byHduId))
        {
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: Wrong PeriEquipment type %u or chnnlidx %d or it(eqpid:%d) has not registered!\n", 
				tDstMt.GetEqpType(), byDstChnnlIdx, byHduId);
            cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

		// 获取此Hdu总共的通道数
		u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(tDstMt);
		if (0 == byHduChnNum || byDstChnnlIdx >= byHduChnNum)
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[StartSwitchHduReq] GetHduChnNumAcd2Eqp failed!\n");
			cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
		}

 	    //电视墙轮询优先级最高，不能选看VMP或选看终端
		//[nizhijun 2010/10/28] 增加对于通道状态是否为running的判断
		TPeriEqpStatus tHduStatus;
        g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
        if ( (TW_MEMBERTYPE_TWPOLL ==  tHduStatus.m_tStatus.tHdu.atVideoMt[byDstChnnlIdx].byMemberType)
			&& (THduChnStatus::eRUNNING == tHduStatus.m_tStatus.tHdu.atHduChnStatus[byDstChnnlIdx].GetStatus())
			)
        {
			ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: current chnl is in polling ,can not switch VMP or mt!\n");
          	cServMsg.SetErrorCode(ERR_MCU_NOTSWITCHVMPORMT);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
		}

		// [2013/03/11 chenbing] 多回传模式 
		u8 bySpyMode = MODE_BOTH;
		u8 byHduMode = HDUCHN_MODE_ONE;
		// [2013/03/11 chenbing]
		if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byDstChnnlIdx) )
		{
			byHduSubChnId = tHduSwitchInfo.GetDstSubChn();
			if (  byHduSubChnId >= HDU_MODEFOUR_MAX_SUBCHNNUM )
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU,\
					"[CMcuVcInst]StartSwitchHduReq: current HduSubChnId(%d) Error or ChnStatus(%d) Error !\n",
					byDstChnnlIdx, tHduStatus.m_tStatus.tHdu.GetChnStatus(byDstChnnlIdx, byHduSubChnId));
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
			bySpyMode = MODE_VIDEO;			// 多回传模式在四风格下只传视频
			byHduMode = HDUCHN_MODE_FOUR;
			g_cMcuVcApp.SetChnnlMMode( byHduId, byDstChnnlIdx, MODE_VIDEO);
		} 

        if (tSrcMt.GetType() == TYPE_MCUPERI)
		{
			//选看的vmp为空时                      
			if (GetVmpCountInVmpList() == 0)
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: PeriEquipment type %u has not registered!\n", 
					tDstMt.GetEqpType());
				cServMsg.SetErrorCode(ERR_MCU_MCUPERI_NOTEXIST);
	            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}

			//tSrcMt = m_tVmpEqp;
		}

        // 未打开后向通道的情况在下面代码中保护
		if ( tSrcMt.GetType() != TYPE_MCUPERI && TW_MEMBERTYPE_MCSSPEC == tHduSwitchInfo.GetMemberType())
		{
			u8 byMtId = tSrcMt.GetMtId();
			if( !tSrcMt.IsLocal() )
			{
				byMtId = GetFstMcuIdFromMcuIdx( tSrcMt.GetMcuId() );
			}

		    TLogicalChannel tSrcRvsChannl;
			if (!m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuStartSwitchHduReq]: SrcMt(%d) GetMtLogicChnnl fail! Type<%d>\n", tSrcMt.GetMtId(),tSrcRvsChannl.GetMediaType());
				cServMsg.SetErrorCode(ERR_MCU_GETMTLOGCHN_FAIL);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
		}
// 
//         // xsl [7/20/2006] 卫星分散会议时需要判断回传通道数
//         if (m_tConf.GetConfAttrb().IsSatDCastMode())
//         {
//             if (IsOverSatCastChnnlNum(tSrcMt))
//             {
//                 ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuStartSwitchHduReq] over max upload mt num. nack!\n");
//                 cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
//                 SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
//                 return;
//             }
//         }

		// [9/15/2010 xliang] filter the same MT in same Hdu channel
		if( TW_MEMBERTYPE_MCSSPEC == tHduSwitchInfo.GetMemberType() 
			|| TW_MEMBERTYPE_VCSSPEC == tHduSwitchInfo.GetMemberType()
			|| TW_MEMBERTYPE_VCSAUTOSPEC== tHduSwitchInfo.GetMemberType())
		{
			if (tHduStatus.m_tStatus.tHdu.GetChnStatus(byDstChnnlIdx, byHduSubChnId) != THduChnStatus::eIDLE
				&& tHduStatus.m_tStatus.tHdu.GetChnMt(byDstChnnlIdx, byHduSubChnId) == tSrcMt &&
				tHduStatus.m_tStatus.tHdu.GetMemberType(byDstChnnlIdx, byHduSubChnId) != TW_MEMBERTYPE_DOUBLESTREAM)
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "UI drag the same mt.(%d,%d) in the same hdu channel.%d memtype.%d, MCU will do nothing!\n",
					tSrcMt.GetMcuId(), tSrcMt.GetMtId(), byDstChnnlIdx,tHduStatus.m_tStatus.tHdu.atVideoMt[byDstChnnlIdx].byMemberType);

				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
				
				return;
			}
		}

		if ( TW_MEMBERTYPE_POLL == tHduSwitchInfo.GetMemberType() )   // 区分是否带音频轮询
		{
			bySpyMode = m_tConf.m_tStatus.GetPollMedia();
		}

// 		if (!tSrcMt.IsNull() 
// 			&& tSrcMt.GetType() != TYPE_MCUPERI 
// 			&& !tSrcMt.IsLocal() 
// 			&& !IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()))
//         {	
//             OnMMcuSetIn(tSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
//         }
	
        switch(tHduSwitchInfo.GetMemberType()) 
        {
        case TW_MEMBERTYPE_MCSSPEC:
		case TW_MEMBERTYPE_VCSSPEC:
		case TW_MEMBERTYPE_VCSAUTOSPEC:
			{
				//源终端必须具备视频发送能力
// 				TMtStatus      tMtStatus;
// 				GetMtStatus(tSrcMt, tMtStatus);
// 				
// 				if (FALSE == tMtStatus.IsSendVideo())
// 				{
// 					cServMsg.SetErrorCode(ERR_MCU_SRCISRECVONLY);
// 					SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
// 					return;
// 				}
				
				//源终端必须已经加入会议
				if (FALSE == m_tConfAllMtInfo.MtJoinedConf(tSrcMt))
				{
					log(LOGLVL_DEBUG1, "[ProcMcsMcuStartSwitchHduReq]: select source MT%u-%u has not joined current conference! Error!\n",
						tSrcMt.GetMcuId(), tSrcMt.GetMtId());
					cServMsg.SetErrorCode(ERR_MCU_MT_NOTINCONF);
					SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);	//nack
					return;
				}
			}
            break;

        case TW_MEMBERTYPE_SPEAKER:     //注意改变发言人时改变交换
			tSrcMt.SetNull();							// [9/14/2011 liuxu] 主动置空,避免mcs传值错误
            if (TRUE == HasJoinedSpeaker())
            {
                tSrcMt = m_tConf.GetSpeaker();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;

        case TW_MEMBERTYPE_CHAIRMAN:    //注意改变主席时改变交换
			tSrcMt.SetNull();							// [9/14/2011 liuxu] 主动置空,避免mcs传值错误
            if (TRUE == HasJoinedChairman())
            {
                tSrcMt = m_tConf.GetChairman();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;

        case TW_MEMBERTYPE_POLL:        //注意改变轮询广播源时改变交换
			tSrcMt.SetNull();							// [9/14/2011 liuxu] 主动置空,避免mcs传值错误
            if ( CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode()
				&& CONF_POLLMODE_VIDEO_CHAIRMAN != m_tConf.m_tStatus.GetPollMode()		// 主席轮询时不进墙
				&& CONF_POLLMODE_BOTH_CHAIRMAN != m_tConf.m_tStatus.GetPollMode()		// 带音频主席轮询也不进墙
				&& CONF_POLLMODE_SPEAKER_SPY != m_tConf.m_tStatus.GetPollMode()			// 上传轮询过滤
				&& CONF_POLLMODE_VIDEO_SPY != m_tConf.m_tStatus.GetPollMode()			// 上传轮询过滤
				)
            {
                tSrcMt = (TMt)m_tConf.m_tStatus.GetMtPollParam();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;
			
		case TW_MEMBERTYPE_SWITCHVMP:      //hdu选看vmp
			if (tSrcMt.GetEqpType() != EQP_TYPE_VMP || !IsVmpIdInVmpList(tSrcMt.GetEqpId()))
			{
				tSrcMt.SetNull();
			}
			else
			{
				tSrcMt = g_cMcuVcApp.GetEqp( tSrcMt.GetEqpId() );
			}
			/*tSrcMt.SetNull();							// [9/14/2011 liuxu] 主动置空,避免mcs传值错误
		    if (!m_tVmpEqp.IsNull())
			{
			    tSrcMt = (TMt)m_tVmpEqp;
			}
			else
			{
				tSrcMt.SetNull();
			}*/
			break;

		case TW_MEMBERTYPE_BATCHPOLL:
			break;

		case TW_MEMBERTYPE_TWPOLL:
			break;

		case TW_MEMBERTYPE_DOUBLESTREAM:
			{
				if (m_tDoubleStreamSrc.IsNull())
				{
					tSrcMt.SetNull();
				}
				else
				{
					tSrcMt = m_tDoubleStreamSrc;
				}				
			}
			break;
        default:
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuStartSwitchHduReq]: Wrong TvWall Member Type!\n");
            break;
		}

		//级联多回传支持 这里如果是下级且支持多回传的mcu，则发presetin消息
		if(!tSrcMt.IsNull() && !tSrcMt.IsLocal() )
		{
			if( IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()) )
			{
				TLogicalChannel tMtAudSendLogicalChannel;
				u8 bySubEqpType = 0;
				//对于老的HDU板卡不能解AACLC和AACLD双声道的码流，那么就不建音频交换，多回传时也只回传视频即可
				if( g_cMcuAgent.GetHduSubTypeByEqpId(byHduId,bySubEqpType) && 
					(HDU_SUBTYPE_HDU_M == bySubEqpType ||
					HDU_SUBTYPE_HDU_H == bySubEqpType  ||
					HDU_SUBTYPE_HDU_L == bySubEqpType) &&
					tSrcMt.GetType() == TYPE_MT && 
					m_ptMtTable->GetMtLogicChnnl(GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId(),LOGCHL_AUDIO,&tMtAudSendLogicalChannel,FALSE) &&
					(tMtAudSendLogicalChannel.GetChannelType() == MEDIA_TYPE_AACLC || tMtAudSendLogicalChannel.GetChannelType() == MEDIA_TYPE_AACLD) && 
					tMtAudSendLogicalChannel.GetAudioTrackNum() == 2)
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuStartSwitchHduReq]HDU NOT SUPPORT AACLC OR AACLC DOUBLETRACK,SO REMOVE AUDIO!\n");
					bySpyMode = MODE_VIDEO;
				}
				TPreSetInReq tSpySrcInitInfo;
				tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt(tSrcMt);
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyHduInfo.m_tHdu = tDstMt;
				tSpySrcInitInfo.m_bySpyMode = bySpyMode;
				tSpySrcInitInfo.SetEvId(cServMsg.GetEventId());
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyHduInfo.m_bySrcMtType = tHduSwitchInfo.GetMemberType();
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyHduInfo.m_bySchemeIdx = tHduSwitchInfo.GetSchemeIdx();
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyHduInfo.m_byDstChlIdx = tHduSwitchInfo.GetDstChlIdx();
				// [2013/03/11 chenbing] 追加子通道
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyHduInfo.SetSubChnIdx(byHduSubChnId);

				// [pengjie 2010/9/13] 填目的端能力
				TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()) );
				//zjl20101116 如果当前终端已回传则能力集要与已回传目的能力集取小
				if(!GetMinSpyDstCapSet(tSrcMt, tSimCapSet))
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StartSwitchHduReq] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
						tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
					return;
				}
				
				tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		        // End

				// [pengjie 2010/8/31] 替换逻辑处理(支持HDU多画面)
				if( !tHduStatus.m_tStatus.tHdu.IsChnNull(byDstChnnlIdx, byHduSubChnId) )
				{
					//修改获取终端接口tHduStatus.m_tStatus.tHdu.atVideoMt[byDstChnnlIdx]
					tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = (TMt)tHduStatus.m_tStatus.tHdu.GetChnMt(byDstChnnlIdx, byHduSubChnId);
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= g_cMcuVcApp.GetChnnlMMode(byHduId, byDstChnnlIdx);
					tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
					if ( MODE_VIDEO == bySpyMode || MODE_BOTH == bySpyMode)
					{
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
					}

					if ( MODE_AUDIO == bySpyMode || MODE_BOTH == bySpyMode)
					{
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
					}
				}
			    // End
				OnMMcuPreSetIn( tSpySrcInitInfo );  
				
				// 先回应界面，不然界面会卡半天，但是这样可能现在有问题
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
				
				return;
			}
		}
		// End Modify
		/*TMt tOldSrcMt  =  (TMt)tHduStatus.m_tStatus.tHdu.GetChnMt(byDstChnnlIdx, byHduSubChnId);
		u8  byStatus   = tHduStatus.m_tStatus.tHdu.GetChnStatus(byDstChnnlIdx, byHduSubChnId);
		if (!tOldSrcMt.IsNull() && byStatus == THduChnStatus::eRUNNING)
		{
			u8  byOldMemberType = tHduStatus.m_tStatus.tHdu.GetMemberType(byDstChnnlIdx, byHduSubChnId);
			ChangeHduSwitch( NULL,byHduId, byDstChnnlIdx, byHduSubChnId, byOldMemberType, TW_STATE_STOP,MODE_BOTH,FALSE,TRUE,FALSE );
			g_cMcuVcApp.GetPeriEqpStatus( byHduId, &tHduStatus );
		} */
		// [2013/03/11 chenbing]  
		tHduStatus.m_tStatus.tHdu.SetSchemeIdx( tHduSwitchInfo.GetSchemeIdx(), byDstChnnlIdx, byHduSubChnId);
        g_cMcuVcApp.SetPeriEqpStatus( byHduId, &tHduStatus );

        cServMsg.SetConfIdx(tSrcMt.GetConfIdx());

        //send Ack reply
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

		// [2013/08/13 chenbing] 若会议模板中配置了HDU通道成员,操作HDU通道时需要清除会议模板通道中的成员
		ClearHduChnnlInHduMoudle(byHduId, byDstChnnlIdx, byHduSubChnId, byHduMode);

		TMtStatus      tMtStatus;
		GetMtStatus(tSrcMt, tMtStatus);
		
		u8 byMode = MODE_NONE;
		if (!tMtStatus.IsSendAudio())
		{
			byMode = MODE_VIDEO;
		}
		else if(!tMtStatus.IsSendVideo())
		{
			byMode = MODE_AUDIO;
		}
		else
		{
			byMode = MODE_BOTH;
		}

		if ( TW_MEMBERTYPE_POLL == tHduSwitchInfo.GetMemberType() )   // 区分是否带音频轮询
		{
			if(byMode == MODE_AUDIO && m_tConf.m_tStatus.GetPollMedia() == MODE_VIDEO)
			{
				byMode = MODE_NONE;
			}
			else if(byMode == MODE_AUDIO && m_tConf.m_tStatus.GetPollMedia() == MODE_BOTH)
			{
				byMode = MODE_AUDIO;
			}
			else if(byMode == MODE_VIDEO && m_tConf.m_tStatus.GetPollMedia() == MODE_BOTH)
			{
				byMode = MODE_VIDEO;
			}
			else
			{
				byMode = m_tConf.m_tStatus.GetPollMedia();
			}

			// [2013/03/11 chenbing] HDU多画面目前不支持轮询跟随,子通道置0
			ChangeHduSwitch(&tSrcMt, byHduId, byDstChnnlIdx, 0,	TW_MEMBERTYPE_POLL, TW_STATE_START, byMode);
		}
		else
		{
			ChangeHduSwitch(&tSrcMt, byHduId, byDstChnnlIdx, byHduSubChnId,\
				tHduSwitchInfo.GetMemberType(), TW_STATE_START, byMode, FALSE, TRUE, TRUE, byHduMode);
		}
        break;
    }
    default:
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuStartSwitchHduReq]: Wrong handle in state %u!\n", CurState());
        break;
	}

	return;
}

/*=============================================================================
函 数 名： ClearHduChnnlInHduMoudle
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： void
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20130813	4.0			陈兵					创建
=============================================================================*/
BOOL32 CMcuVcInst::ClearHduChnnlInHduMoudle(u8 byHduId, u8 byHduChnId, u8 byHduSubChnId, u8 byHduMode)
{
	if ( byHduId < HDUID_MIN || byHduId > HDUID_MAX )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_HDU, "[ClearHduChnnlInHduMoudle] HduId(%d) is vaild!!!\n", byHduId);
		return FALSE;
	}

	if ( (FALSE == m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo() && HDUCHN_MODE_FOUR == byHduMode)
		|| (FALSE == m_tConf.GetConfAttrb().IsHasTvWallModule() && HDUCHN_MODE_ONE == byHduMode)
	   )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ClearHduChnnlInHduMoudle] HDU-Module is NULL Don't Clear Hdu-Module Member!!!\n");
		return FALSE;
	}

	u8 byHduSchemeNum = 0;
	THduStyleInfo atHduStyleInfoTable[MAX_HDUSTYLE_NUM];
	u16 wRet = g_cMcuAgent.ReadHduSchemeTable(&byHduSchemeNum, atHduStyleInfoTable);

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU, "[ClearHduChnnlInHduMoudle] byHduMode(%d) Hdu2Vmp-Module(%d) Hdu-Module(%d)!!!\n",
		byHduMode, m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo(), m_tConf.GetConfAttrb().IsHasTvWallModule());

	// HDU2多画面模板
	if( m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo() && HDUCHN_MODE_FOUR == byHduMode)
	{
		if (SUCCESS_AGENT == wRet)
		{
			//遍历所有HDU通道
			for (u8 byChnId = 0; byChnId < MAXNUM_HDUBRD*MAXNUM_HDU_CHANNEL; byChnId++)
			{
				if( byHduId == m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byChnId].m_byHduEqpId
					&& byHduChnId == m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byChnId].m_byChIdx)
				{
					m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byChnId].m_abyTvWallMember[byHduSubChnId] = 0;
					m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_atHduVmpChnInfo[byChnId].m_abyMemberType[byHduSubChnId] = TW_MEMBERTYPE_NULL;
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ClearHduChnnlInHduMoudle] HduVmpInfo HduId(%d) byHduChnId(%d)!\n", byHduId,
						byHduChnId );
					return TRUE;
				}
			}
		}
	}

	// HDU模板
	if ( m_tConf.GetConfAttrb().IsHasTvWallModule() && HDUCHN_MODE_ONE == byHduMode)
	{
		if (SUCCESS_AGENT == wRet)
		{
			//遍历所有预案
			for (u8 bySchemeId = 0; bySchemeId < byHduSchemeNum && bySchemeId < MAXNUM_PERIEQP_CHNNL; bySchemeId++)
			{
				THduChnlInfo  atHduChnlTable[MAXNUM_HDUCFG_CHNLNUM];
				atHduStyleInfoTable[bySchemeId].GetHduChnlTable(atHduChnlTable);
				//遍历预案下的所有通道
				for (u8 byStyleChnId = 0; byStyleChnId < MAXNUM_HDUCFG_CHNLNUM; byStyleChnId++)
				{
					if(    atHduChnlTable[byStyleChnId].GetEqpId() == byHduId
						&& atHduChnlTable[byStyleChnId].GetChnlIdx() == byHduChnId
					  )
					{
						m_tConfEqpModule.m_tTvWallInfo[bySchemeId].m_atTvWallMember[byStyleChnId].SetNull();
						m_tConfEqpModule.m_tMultiTvWallModule.m_atTvWallModule[bySchemeId].m_abyTvWallMember[byStyleChnId] = 0;
						// 对于TW_MEMBERTYPE_VCSAUTOSPEC类型标识含义为自动模式下该通道可用，所以对其不清空
						if (m_tConfEqpModule.m_tMultiTvWallModule.m_atTvWallModule[bySchemeId].m_abyMemberType[byStyleChnId] != TW_MEMBERTYPE_VCSAUTOSPEC)
						{
							m_tConfEqpModule.m_tMultiTvWallModule.m_atTvWallModule[bySchemeId].m_abyMemberType[byStyleChnId] = 0;
						}
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ClearHduChnnlInHduMoudle] MemberType(%d)!\n",
							m_tConfEqpModule.m_tMultiTvWallModule.m_atTvWallModule[bySchemeId].m_abyMemberType[byStyleChnId]);
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}


//4.7.2 新加
/*=============================================================================
函 数 名： ChangeMtResFpsInHduVmp
功    能： 改变Mt在HDU多画面中的分辨率
算法实现： 
全局变量： 
参    数： 	TMt tMt,				需要调整的tMt
			u8 byHduId, 
			u8 byHduChnId,
			u8 byHduSubChnId,	
			BOOL32& bIsStartSwitch, TRUE: 可以建交换，FALSE：不能建交换
			BOOL32 bIsStart,		TRUE：下调分辨率，FALSE：恢复分辨率
			BOOL32 bForceAdjust,	TRUE：强制下调分辨率，FALSE：非强制
模块功能： HDU多画面接口添加
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		 版本		修改人		走读人    修改内容
2013/03/11   4.7.2       陈兵                  创建(HDU多画面支持)
=============================================================================*/
BOOL32 CMcuVcInst::ChangeMtResFpsInHduVmp(TMt tMt,
										  u8 byHduId, 
										  u8 byHduChnId,
										  u8 byHduSubChnId/* = 0*/,
										  BOOL32& bIsStartSwitch,
									      BOOL32 bIsStart/* = TRUE*/,
										  BOOL32 bForceAdjust
										  )
{
	if (tMt.IsNull())
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ChangeMtResFpsInHduVmp] tMt(%d, %d) isnull so return!!!\n",tMt.GetMcuId(), tMt.GetMtId());
		return FALSE;
	}

	TExInfoForResFps tExInfoForResFps;
	TPeriEqpStatus tHduStatus;
	g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);

	tExInfoForResFps.m_byEqpid = byHduId;
	tExInfoForResFps.m_byPos   = byHduChnId;
	tExInfoForResFps.m_byHduSubChnId = byHduSubChnId;
	tExInfoForResFps.m_bIsForceAdjust = bForceAdjust;
	tExInfoForResFps.m_byEvent = EvMask_HDUVMP;
	if (bIsStart)
	{
		tExInfoForResFps.m_byOccupy = Occupy_InChannel;
	}
	else
	{
		tExInfoForResFps.m_byOccupy = Occupy_OutChannel;
	}

	// 判断是否需要调整帧率
	BOOL32 bIsNeedAjsFps = IsNeedAdjustMtVidFps(tMt, tExInfoForResFps);
	if (bIsNeedAjsFps)
	{
		//获得要调整到的帧率
		u8  byNewFps = tExInfoForResFps.m_byNewFps;
		// 发送调帧率消息
		SendChgMtFps(tMt, LOGCHL_VIDEO, byNewFps, bIsStart);
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ChangeMtResFpsInHduVmp] tMt(%d, %d) bIsStart <%d> Adjusted NewFps: <%d>!!!\n",
			tMt.GetMcuId(), tMt.GetMtId(), bIsStart, byNewFps);
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ChangeMtResFpsInHduVmp] Current Fps:<%d> don't Adjusted!!!\n", tExInfoForResFps.m_byNewFps);
	}

	//下调帧率，不能建交换的直接返回
	if (!tExInfoForResFps.m_bResultSetVmpChl && bIsStart)
	{
		bIsStartSwitch = tExInfoForResFps.m_bResultSetVmpChl;
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ChangeMtResFpsInHduVmp] Fps IsStartSwitch:<%d> !!!\n", bIsStartSwitch);
		return bIsNeedAjsFps;
	}

	// 判断是否需要调整分辨率
	BOOL32 bIsNeedAjsRes = IsNeedAdjustMtVidFormat(tMt, tExInfoForResFps);
	if (bIsNeedAjsRes)
	{
		//获得要调整到的分辨率
		u8  byNewRes = tExInfoForResFps.m_byNewFormat;
		// 发送调分辨率消息
		ChangeVFormat(tMt, bIsStart, byNewRes, byHduId, 0xff, byHduSubChnId, 0, 0, byHduChnId);
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ChangeMtResFpsInHduVmp] tMt(%d, %d) bIsStart<%d> Adjusted NewRes: <%d> !!!\n",
			tMt.GetMcuId(), tMt.GetMtId(), bIsStart, byNewRes);
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ChangeMtResFpsInHduVmp] Current Res:<%d> don't Adjusted!!!\n", tExInfoForResFps.m_byNewFormat);
	}

	//能不能建交换
	bIsStartSwitch = tExInfoForResFps.m_bResultSetVmpChl;
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ChangeMtResFpsInHduVmp] Res IsStartSwitch:<%d> !!!\n", bIsStartSwitch);


	return bIsNeedAjsRes;
}

//4.7.2 新加
/*=============================================================================
函 数 名： ProcMcsMcuChangeHduVmpMode
功    能： 改变HDU通道风格
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
模块功能： HDU多画面接口添加
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		 版本		修改人		走读人    修改内容
2013/03/11   4.7.2       陈兵                  创建(HDU多画面支持)
=============================================================================*/
void CMcuVcInst::ProcMcsMcuChangeHduVmpMode(const CMessage * pcMsg)
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	ChangeHduVmpMode(cServMsg);
}

/*=============================================================================
函 数 名： ChangeHduVmpMode
功    能： 向外设改变HDU通道风格消息
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
模块功能： HDU多画面接口添加
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		 版本		修改人		走读人    修改内容
2013/03/11   4.7.2       陈兵                  创建(HDU多画面支持)
=============================================================================*/
void CMcuVcInst::ChangeHduVmpMode(CServMsg cServMsg)
{
	u8			byHduId = cServMsg.GetEqpId();
	u8          byHduChnId = cServMsg.GetChnIndex();
	u8			byNewHduMode = *(u8 *)cServMsg.GetMsgBody();

	TPeriEqpStatus  tHduStatus;
	if ( (byHduId < HDUID_MIN || byHduId > HDUID_MAX)
		|| !g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus)
		|| !g_cMcuVcApp.IsPeriEqpConnected(byHduId)
        )
	{
		NotifyMcsAlarmInfo(0, ERR_MCU_CHGMODE_INPUTPARAM_ERROR);
		ConfPrint( LOG_LVL_ERROR, MID_MCU_MCS,
			"[ProcMcsMcuChangeHduVmpMode]: error byHduId (%d) onLine(%d)!!!\n",
			byHduId, g_cMcuVcApp.IsPeriEqpConnected(byHduId) );
		return;
	}

	u8 byConfIdx = tHduStatus.m_tStatus.tHdu.GetConfIdx(byHduChnId);
	// 不允许不同的会议号操作同一个多画面 
	if ( byConfIdx && byConfIdx != m_byConfIdx )
	{
		NotifyMcsAlarmInfo(0, ERR_MCU_CHGMODE_INPUTPARAM_ERROR);
		ConfPrint( LOG_LVL_ERROR, MID_MCU_MCS,
			"[ProcMcsMcuChangeHduVmpMode]: Wrong OldConfIdx (%d) CurConfIdx(%d)!!!\n", byConfIdx, m_byConfIdx);
		return;
	}
	
	if( !IsValidHduChn(byHduId, byHduChnId)
		/*|| THduChnStatus::eWAITCHGMODE == tHduStatus.m_tStatus.tHdu.GetChnStatus(byHduChnId)*/
	  )
	{
		ConfPrint( LOG_LVL_ERROR, MID_MCU_MCS,
			"[ProcMcsMcuChangeHduVmpMode]: Wrong IsNoValidHduChn: (%d)!!!\n", byHduChnId );
		
		NotifyMcsAlarmInfo(0, ERR_MCU_CHGMODE_INPUTPARAM_ERROR);
		return;
	}
	
	// 当前通道已经是需要切换的风格
	if ( tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byHduChnId) == byNewHduMode )
	{
		ConfPrint( LOG_LVL_DETAIL, MID_MCU_MCS,
			"[ProcMcsMcuChangeHduVmpMode]: New HduChnMode and Old HduChnMode is same [%d]!!!\n", byNewHduMode );
		return;
	}

	// 当前通道不支持四风格 
	if (HDUCHN_MODE_FOUR == byNewHduMode && HDU_MODEFOUR_MAX_SUBCHNNUM != tHduStatus.m_tStatus.tHdu.GetChnMaxVmpMode(byHduChnId) )
	{
		ConfPrint( LOG_LVL_DETAIL, MID_MCU_MCS,
			"[ProcMcsMcuChangeHduVmpMode]: Current HduChnnl can't Support HduFourMode HduId:(%d) HduChnId!!!\n", byHduId, byHduChnId );
		NotifyMcsAlarmInfo(0, ERR_MCU_HDUCHN_NOMULTIMODE);
		return;
	}
	
	ConfPrint( LOG_LVL_DETAIL, MID_MCU_MCS,
		"[ProcMcsMcuChangeHduVmpMode]: Mcs Send HduId(%d) HduChnId(%d) NewHduMode(%d) !!!\n", byHduId, byHduChnId, byNewHduMode);

	// 当前通道为四风格，另一通道也为四风格
	if (  HDUCHN_MODE_FOUR == byNewHduMode
		&& HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byHduChnId, TRUE) )
	{
		ConfPrint( LOG_LVL_DETAIL, MID_MCU_MCS,
			"[ProcMcsMcuChangeHduVmpMode]: Other HduChnnl already is HDUCHN_MODE_FOUR !!!\n" );
		NotifyMcsAlarmInfo(0, ERR_MCU_HDUCHN_MULTIMODE_TOOMORE);
		return;
	}

	// 由一风格切换到四风格
	if ( HDUCHN_MODE_ONE == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byHduChnId)
		&& HDUCHN_MODE_FOUR == byNewHduMode )
	{
		// 判断通道中有无终端存在
		if ( !tHduStatus.m_tStatus.tHdu.GetChnMt(byHduChnId).IsNull())
		{
			u8 byHduMemberType = tHduStatus.m_tStatus.tHdu.GetMemberType(byHduChnId, 0);
			//拆除交换
			ChangeHduSwitch(NULL, byHduId, byHduChnId, 0, byHduMemberType, TW_STATE_STOP, MODE_BOTH);
		}
 		// 重新获取状态，ChangeHduSwitch中会发生改变
		g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
		tHduStatus.m_tStatus.tHdu.SetConfIdx(m_byConfIdx, byHduChnId );
	}// 由四风格切换到一风格
	else if (  HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byHduChnId)
		    && HDUCHN_MODE_ONE == byNewHduMode)
	{
		for (u8 byIndex=0; byIndex<HDU_MODEFOUR_MAX_SUBCHNNUM; byIndex++)
		{
			// 判断通道中有无终端存在
			if ( !tHduStatus.m_tStatus.tHdu.GetChnMt(byHduChnId, byIndex).IsNull())
			{
				//拆除交换
				ChangeHduSwitch(NULL, byHduId, byHduChnId, byIndex,	TW_MEMBERTYPE_MCSSPEC, TW_STATE_STOP, MODE_VIDEO,
					FALSE, TRUE, TRUE, HDUCHN_MODE_FOUR);
			}
		}
		// 重新获取状态，ChangeHduSwitch中会发生改变
		g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
	}
	
	//设置通道状态
	tHduStatus.m_tStatus.tHdu.SetChnStatus(byHduChnId, 0, THduChnStatus::eWAITCHGMODE);
	g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);

	cServMsg.SetConfId(m_tConf.GetConfId());
	SendChangeHduModeReq(cServMsg);

	return;
}

//4.7.2 新加
/*=============================================================================
函 数 名： SendChangeHduModeReq
功    能： 向外设改变HDU通道风格消息
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
模块功能： HDU多画面接口添加
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		 版本		修改人		走读人    修改内容
2013/03/11   4.7.2       陈兵                  创建(HDU多画面支持)
=============================================================================*/
void CMcuVcInst::SendChangeHduModeReq(CServMsg cServMsg)
{

	SendMsgToEqp( cServMsg.GetEqpId(), MCU_HDU_CHGHDUVMPMODE_REQ, cServMsg );
	ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP,
		"[SendChangeHduModeMsg]: Send MCU_HDU_CHGHDUVMPMODE_REQ  OK!!!\n" );

}

/*=============================================================================
    函 数 名： StartSwitchTWReq
    功    能： 开始电视墙通道交换处理函数
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/09/15  4.0			libo                  创建
=============================================================================*/
void CMcuVcInst::StartSwitchTWReq(CServMsg & cServMsg)
{
    TTWSwitchInfo  tTWSwitchInfo;
    TMtStatus      tMtStatus;
    TMt	           tDstMt;
    TMt            tSrcMt;
    u8             byTvWallId = 0;
    u8             byDstChnnlIdx;
    //TMt            tSrcOrg;             //针对级联作保留
    TPeriEqpStatus tTvWallStatus;
    switch(CurState())
    {
    case STATE_ONGOING:

        tTWSwitchInfo = *(TTWSwitchInfo *)cServMsg.GetMsgBody();
        tSrcMt = tTWSwitchInfo.GetSrcMt();
        tDstMt = tTWSwitchInfo.GetDstMt();
        byTvWallId = tDstMt.GetEqpId();
        byDstChnnlIdx = tTWSwitchInfo.GetDstChlIdx();

        //类型不对或者在本MCU上未登记
        if (EQP_TYPE_TVWALL != tDstMt.GetEqpType() ||
            !g_cMcuVcApp.IsPeriEqpConnected(byTvWallId) ||
            byDstChnnlIdx >= MAXNUM_PERIEQP_CHNNL)
        {
			ConfPrint( LOG_LVL_DETAIL, MID_MCU_MCS, "CMcuVcInst: Wrong PeriEquipment type %u or it has not registered!\n", 
                            tDstMt.GetEqpType() );

            cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
		
        if (g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTvWallStatus))
        {
			if( tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx].GetConfIdx() != 0 &&
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx].GetConfIdx() != m_byConfIdx &&
			   !tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx].IsNull())
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[StartSwitchTWReq] TvWall<EqpId:%d,ChnId:%d> has been ocuppied by Conf%d\n",
								byTvWallId, byDstChnnlIdx,
								tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx].GetConfIdx()
							);
				
				cServMsg.SetErrorCode(ERR_MCU_HDUBATCHPOLL_CHNLINUSE);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
        }
		
        if (tSrcMt.GetType() == TYPE_MCUPERI)
        {
			//选看的vmp为空时                      
			if (m_tVmpEqp.IsNull())
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: PeriEquipment type %u has not registered!\n", 
					tDstMt.GetEqpType());
				cServMsg.SetErrorCode(ERR_MCU_MCUPERI_NOTEXIST);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
			
			tSrcMt = m_tVmpEqp;
		}

        // 未打开后向通道的情况在下面代码中保护
		/*
		if ( tSrcMt.GetType() != TYPE_MCUPERI )
		{
			if (!m_ptMtTable->GetMtLogicChnnl( tSrcMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
			{
				ConfLog(FALSE, "[StartSwitchTWReq]: GetMtLogicChnnl fail!\n");
				cServMsg.SetErrorCode(ERR_MCU_GETMTLOGCHN_FAIL);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			    return;
			}
		}
		// xliang [7/13/2009] DEC5 adjust resolution strategy
		// zjl    [02/24/2010] DEC5 降分辨率在changetvwall中统一处理
		if ( tSrcRvsChannl.GetChannelType() == MEDIA_TYPE_H264 &&
			( IsVidFormatHD( tSrcRvsChannl.GetVideoFormat() ) ||
			tSrcRvsChannl.GetVideoFormat() == VIDEO_FORMAT_4CIF ) )
		{
			// nack for none keda MT whose resolution is about  h264 4Cif 
			if ( tSrcOrg.IsLocal() && // ensure that it's MT, not MCU
				MT_MANU_KDC != m_ptMtTable->GetManuId(tSrcMt.GetMtId()) 
				)
			{
				
				ConfLog(FALSE, "[StartSwitchTWReq] Mt<%d> VidType.%d with format.%d support no tvwall\n",
					tSrcMt.GetMtId(), tSrcRvsChannl.GetChannelType(), tSrcRvsChannl.GetVideoFormat() );
				cServMsg.SetErrorCode(ERR_MT_MEDIACAPABILITY);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
				
				return;
			}
			else
			{
				// force the mt to adjust its resolution to Cif 
				
				u8 byNewFormat = VIDEO_FORMAT_CIF;

				if(!tSrcOrg.IsLocal())
				{
					TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcOrg.GetMcuId());
					TMt tLastSrcMt = ptMcInfo->m_tLastMMcuViewMt;
					if (!tLastSrcMt.IsNull())			
					{
						//恢复上次回传源Mt的分辨率
						// [11/11/2009] Xliang 这里并没有严格按照回传源是否之前在Dec5中。
						CascadeAdjMtRes( tLastSrcMt, byNewFormat, FALSE);
					}
					
					CascadeAdjMtRes( tSrcOrg, byNewFormat, TRUE);
				}
				else
				{
					u8 byChnnlType = LOGCHL_VIDEO;
					
					CServMsg cMsg;
					cMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
					cMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
					cMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
					SendMsgToMt( tSrcMt.GetMtId(), cMsg.GetEventId(), cMsg );
					
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[StartSwitchTWReq]send videoformat<%d>(%s) change msg to mt<%d>!\n",
						byNewFormat, GetResStr(byNewFormat), tSrcMt.GetMtId() );
				}
				
				// [12/30/2009 xliang] VMP 前适配通道整理
				TChnnlMemberInfo tChnnlMemInfo;
				for(u8 byLoop = 0; byLoop < MAXNUM_SVMPB_HDCHNNL; byLoop ++)
				{
					m_tVmpChnnlInfo.GetHdChnnlInfo(byLoop, &tChnnlMemInfo);
					if( tChnnlMemInfo.GetMt() == tSrcOrg )
					{
						m_tVmpChnnlInfo.ClearOneChnnl(tChnnlMemInfo.GetMtId()); 
					}
				}
			}
        }
		*/
        // [9/15/2010 xliang] filter the same MT in same tvwall channel
		if( TW_MEMBERTYPE_MCSSPEC == tTWSwitchInfo.GetMemberType() 
			|| TW_MEMBERTYPE_VCSSPEC == tTWSwitchInfo.GetMemberType())
		{
			if (tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx] == tSrcMt)
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "UI drag the same mt.(%d,%d) in the same tw channel.%d, MCU will do nothing!\n",
					tSrcMt.GetMcuId(), tSrcMt.GetMtId(), byDstChnnlIdx);
				
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
				
				return;
			}
		}
	
        // xsl [7/20/2006] 卫星分散会议时需要判断回传通道数
        if (m_tConf.GetConfAttrb().IsSatDCastMode()
			&& IsMultiCastMt(tSrcMt.GetMtId())
			&& !IsSatMtSend(tSrcMt))
        {
            //暂时不支持替换
//            TPeriEqpStatus tTWStatus;
//            g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTWStatus);
//            BOOL32 bCheck = FALSE;
//            TMt tDstChnnlMt = tTWStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx];
//            if (!tDstChnnlMt.IsNull())
//            {
//                //若目的通道内终端还在其他通道内，则检查；否则替换不检查
//                bCheck = !IsMtNotInOtherTvWallChnnl(tDstChnnlMt.GetMtId(), byTvWallId, byDstChnnlIdx);
//            }
//            else
//            {
//                bCheck = TRUE;
//            }
            
            if (/*IsOverSatCastChnnlNum(tSrcMt)*/
				//IsSatMtOverConfDCastNum(tSrcMt, emTvWall, 0xff, 0xff, 0xff, byTvWallId,byDstChnnlIdx))
				!IsSatMtCanContinue())
            {
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[StartSwitchTWReq] over max upload mt num. nack!\n");
                cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
        }
	
        switch(tTWSwitchInfo.GetMemberType()) 
        {
        case TW_MEMBERTYPE_MCSSPEC:
		case TW_MEMBERTYPE_VCSAUTOSPEC:
		//zjj20091025
		case TW_MEMBERTYPE_VCSSPEC:

            //源终端必须具备视频发送能力
			GetMtStatus(tSrcMt, tMtStatus);

            if (FALSE == tMtStatus.IsSendVideo() &&
				m_tConf.GetConfSource() != VCS_CONF)
            {
                cServMsg.SetErrorCode(ERR_MCU_SRCISRECVONLY);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
                return;
            }
			
            //源终端必须已经加入会议
            if (!m_tConfAllMtInfo.MtJoinedConf(tSrcMt) &&
				m_tConf.GetConfSource() != VCS_CONF)
            {
                log(LOGLVL_DEBUG1, "CMcuVcInst: select source MT%u-%u has not joined current conference! Error!\n",
                                    tSrcMt.GetMcuId(), tSrcMt.GetMtId());
                cServMsg.SetErrorCode(ERR_MCU_MT_NOTINCONF);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);	//nack
                return;
            }
            break;

        case TW_MEMBERTYPE_SPEAKER://注意改变发言人时改变交换
			
            if (TRUE == HasJoinedSpeaker())
            {
                tSrcMt = m_tConf.GetSpeaker();
            }
			else
			{
				tSrcMt.SetNull();
			}

            break;

        case TW_MEMBERTYPE_CHAIRMAN://注意改变主席时改变交换
			tSrcMt.SetNull();							// [9/14/2011 liuxu] 主动置空,避免mcs传值错误
            if (TRUE == HasJoinedChairman())
            {
                tSrcMt = m_tConf.GetChairman();
            }

            break;

        case TW_MEMBERTYPE_POLL://注意改变轮询广播源时改变交换
			tSrcMt.SetNull();							// [9/14/2011 liuxu] 主动置空,避免mcs传值错误
            if (CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode()
				&& CONF_POLLMODE_VIDEO_CHAIRMAN != m_tConf.m_tStatus.GetPollMode()		// 主席轮询过滤
				&& CONF_POLLMODE_BOTH_CHAIRMAN != m_tConf.m_tStatus.GetPollMode()		// 主席轮询过滤
				&& CONF_POLLMODE_SPEAKER_SPY != m_tConf.m_tStatus.GetPollMode()			// 上传轮询过滤
				&& CONF_POLLMODE_VIDEO_SPY != m_tConf.m_tStatus.GetPollMode()			// 上传轮询过滤
				)
            {
                tSrcMt = (TMt)m_tConf.m_tStatus.GetMtPollParam();
            }

            break;

        default:
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"CMcuVcInst: Wrong TvWall Member Type!\n");
            break;
		}

		//  [12/11/2009 pengjie] Modify 级联多回传支持 这里如果是下级且支持多回传的mcu，则发presetin消息
		if(!tSrcMt.IsLocal() && !tSrcMt.IsNull() )
		{
			if( IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()) )
			{
				TPreSetInReq tSpySrcInitInfo;
				tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt(tSrcMt);
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_tTvWall = tDstMt;
				tSpySrcInitInfo.m_bySpyMode = MODE_BOTH;
				tSpySrcInitInfo.SetEvId(cServMsg.GetEventId());
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_bySrcMtType = tTWSwitchInfo.GetMemberType();
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_bySchemeIdx = tTWSwitchInfo.GetSchemeIdx();
				tSpySrcInitInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_byDstChlIdx = tTWSwitchInfo.GetDstChlIdx();
				
				// [pengjie 2010/9/13] 填目的端能力
				TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()) );	
				if(tSimCapSet.IsNull())
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StartSwitchTWReq] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
						tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
					return;
				}
				// 标清电视墙只有cif的能力
				tSimCapSet.SetVideoResolution( VIDEO_FORMAT_CIF );
				//zjl20101116 如果当前终端已回传则能力集要与已回传目的能力集取小
				if (!GetMinSpyDstCapSet(tSrcMt, tSimCapSet))
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StartSwitchTWReq] GetMinSpyDstCapSet failed!\n");
					return;
				}
				tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		        // End

				// [pengjie 2010/8/31] 增加替换逻辑处理
				if( !tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx].IsNull() )
				{
					tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = \
						(TMt)tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx];
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_BOTH;
					tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
				}
			    // End
		
				OnMMcuPreSetIn( tSpySrcInitInfo );  
				
				// 先回应界面，不然界面会卡半天，但是这样可能现在有问题
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
				
				return;
			}
		}
		// End Modify

        //send Ack reply
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

		if (!tSrcMt.IsNull() && !tSrcMt.IsLocal())
        {
            OnMMcuSetIn(tSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
        }

        ChangeTvWallSwitch(&tSrcMt, byTvWallId, byDstChnnlIdx, tTWSwitchInfo.GetMemberType(), TW_STATE_START);

        break;

    default:
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
        break;
	}	
}

/*=============================================================================
    函 数 名： ProcMcsMcuStopSwitchTWReq
    功    能： 停止电视墙通道交换处理函数
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/02/25  3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMcsMcuStopSwitchTWReq( const CMessage * pcMsg )
{
    if (CurState() != STATE_ONGOING)
    {
        return;
    }

	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TTWSwitchInfo tTWSwitchInfo;
	TMt	tEqp;

	tTWSwitchInfo = *( TTWSwitchInfo * )cServMsg.GetMsgBody();
	tEqp = tTWSwitchInfo.GetDstMt();

	//error, Nack
	if( tEqp.GetEqpType() != EQP_TYPE_TVWALL )
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcSsnInst: Wrong PeriEquipment type %u or it has not registered!\n", 
			             tEqp.GetEqpType() );
		cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	// 该段从回ACK消息的下面提到这里，防止其它也在开电视墙的会议重复发stop请求
	// zgc, 2008-01-25
	TMt tRemoveMt;
    TPeriEqpStatus tTWStatus;
    u8             byTvWallId = 0;
    u8             byChnlNo;
    byTvWallId = tEqp.GetEqpId();
    byChnlNo   = tTWSwitchInfo.GetDstChlIdx();
    g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTWStatus);
    tRemoveMt = tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlNo];
    if (tRemoveMt.GetConfIdx() != m_byConfIdx)
    {
        return;
    }
    
	StopSwitchToPeriEqp( tEqp.GetEqpId(), tTWSwitchInfo.GetDstChlIdx() );

	//lukunpeng 2010/07/22 电视墙代码整理,为什么有现成的封装接口不调用，非要走下面的单独逻辑
	//而且下面的逻辑考虑也不完全，比如恢复分辨率等。
	/*
	//  [12/9/2009 pengjie] Modify 级联多回传支持，这里尝试释放回传资源
	FreeRecvSpy( tRemoveMt, MODE_BOTH );
	//  End
	
	tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlNo].byMemberType = 0;//tTWSwitchInfo.GetMemberType();
    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlNo].SetNull();
	tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlNo].SetConfIdx(0);
    g_cMcuVcApp.SetPeriEqpStatus(byTvWallId, &tTWStatus);

    if( IsMtNotInOtherTvWallChnnl(tRemoveMt, byTvWallId, byChnlNo) )  
    {
//         m_ptMtTable->SetMtInTvWall(tLocalMt.GetMtId(), FALSE);
//         MtStatusChange(); 

		//2010/07/19 lukp 更新终端电视墙状态
		TMtStatus tMtStatus;
		GetMtStatus(tRemoveMt, tMtStatus);
		tMtStatus.SetInTvWall(FALSE);
		SetMtStatus(tRemoveMt, tMtStatus);
		MtStatusChange(&tRemoveMt);
		
		//恢复终端的原始分辨率，本机或下级均可
		RecoverMtResInMpu( tRemoveMt );
    }
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMcsMcuStopSwitchTWReq] Mt<OrgMcuId:%d, OrgMtId:%d> <LocalMcuId:%d, LocalMtId:%d>is in other TvWallChn!\n",
			tRemoveMt.GetMcuId(), tRemoveMt.GetMtId(),
			tLocalMt.GetMcuId(), tLocalMt.GetMtId());
	}
    	
	CServMsg cMsg;
    cMsg.SetMsgBody((u8 *)&tTWStatus, sizeof(tTWStatus));
    SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cMsg);
	*/

	//lukunpeng 2010/07/22 电视墙代码整理,为什么有现成的封装接口不调用，非要走上面的单独逻辑
	//而且上面的逻辑考虑也不完全，比如恢复分辨率等。
	ChangeTvWallSwitch(NULL, byTvWallId, byChnlNo, tTWSwitchInfo.GetMemberType(), TW_STATE_STOP);
	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
    //清除TvWall模版相应成员信息
    TMt tNullMt;
    tNullMt.SetNull();
    u8 byMtPos;
    u8 byMemberType = 0;
    BOOL32 bExist = m_tConfEqpModule.GetTvWallMemberByMt(byTvWallId, tRemoveMt, byMtPos, byMemberType);
    if( bExist )
    {
        m_tConfEqpModule.SetTvWallMemberInTvInfo(byTvWallId, byChnlNo, tNullMt);    // remove tv wall info
        m_tConfEqpModule.m_tMultiTvWallModule.RemoveMtByTvId(byTvWallId, tRemoveMt.GetMtId());  // remove mutilTvWallmodule
    }

	
	
}

/*=============================================================================
  函 数 名： IsMtNotInOtherTvWallChnnl
  功    能： 是否需要清除终端在电视墙通道的状态
  算法实现： 
  全局变量： 
  参    数： TMt tMt
             u8 byTvWallId
             u8 byChnlId
			 BOOL32 bIsExceptSelf
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcInst::IsMtNotInOtherTvWallChnnl(TMt tMt, u8 byTvWallId, u8 byChnlId,BOOL32 bIsExceptSelf,BOOL32 bIsFakeInTvWall)
{
	TMt tMtInTv;
	TPeriEqpStatus tTWStatus;
	//zjj20091026做保护
	if (!bIsExceptSelf)
	{
		if( byTvWallId >= TVWALLID_MIN && byTvWallId <= TVWALLID_MAX )
		{
			g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTWStatus);
			
			//是否在本电视墙的其他通道里
			for(u8 byLp = 0; byLp < tTWStatus.m_tStatus.tTvWall.byChnnlNum; byLp++)
			{
				if(byLp != byChnlId)
				{
					tMtInTv = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byLp];
					//zjl[20100205]这里先判断会议id，如果当前local的是其他会议的终端则会得到当前会议相同id的终端，导致会议号判断有误
					if (tMtInTv.GetConfIdx() == m_byConfIdx)
					{					
						if( tMtInTv == tMt )
						{
							//对于VCS会议来说增加通道模式判断，如果为MODE_NONE则表示为假进墙终端，实际墙中并无该终端图像
							if (bIsFakeInTvWall && VCS_CONF == m_tConf.GetConfSource() && MODE_NONE == g_cMcuVcApp.GetChnnlMMode(byTvWallId,byLp))
							{
								ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU, "[IsMtNotInOtherTvWallChnnl]VCS_CONF Mt(%d,%d) Is Not In (%d,%d)!\n",
									tMt.GetMcuId(),tMt.GetMtId(),byTvWallId,byLp);	
							} 
							else
							{
								return FALSE;
							}
						}
					}
				}
			}
		}
	}
    //是否在其他电视墙的通道里
    for(u8 byTvId = TVWALLID_MIN; byTvId <= TVWALLID_MAX; byTvId++)
    {
        if(byTvWallId == byTvId)
            continue;
        
        g_cMcuVcApp.GetPeriEqpStatus(byTvId, &tTWStatus);
        if( tTWStatus.m_byOnline )
        {
            for(u8 byLp = 0; byLp < tTWStatus.m_tStatus.tTvWall.byChnnlNum; byLp++)
            {
                tMtInTv = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byLp];
				//zjl[20100205]这里先判断会议id，如果当前local的是其他会议的终端则会得到当前会议相同id的终端，导致判断会议号有误
				if (tMtInTv.GetConfIdx() == m_byConfIdx)
				{					
					if(tMtInTv == tMt)
					{
						//对于VCS会议来说增加通道模式判断，如果为MODE_NONE则表示为假进墙终端，实际墙中并无该终端图像
						if (bIsFakeInTvWall && VCS_CONF == m_tConf.GetConfSource() && MODE_NONE == g_cMcuVcApp.GetChnnlMMode(byTvId,byLp))
						{
							ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU, "[IsMtNotInOtherTvWallChnnl]VCS_CONF Mt(%d,%d) Is Not In (%d,%d)!\n",
								tMt.GetMcuId(),tMt.GetMtId(),byTvId,byLp);	
						} 
						else
						{
							return FALSE;
						}
					}
				}				
            }
        }
    }
	
    return TRUE;
}

/*=============================================================================
  函 数 名： IsMtNotInOtherTvWallChnnl
  功    能： 是否需要清除终端在电视墙通道的状态
  算法实现： 
  全局变量： 
  参    数： u8 byMtId
             u8 byTvWallId
             u8 byChnlId
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcInst::IsMtNotInOtherTvWallChnnl(u8 byMtId, u8 byTvWallId, u8 byChnlId)
{
	TMt tMtInTv;
    TPeriEqpStatus tTWStatus;
	//zjj20091026做保护
	if( byTvWallId >= TVWALLID_MIN && byTvWallId <= TVWALLID_MAX )
	{
		g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTWStatus);
		
		//是否在本电视墙的其他通道里
		for(u8 byLp = 0; byLp < tTWStatus.m_tStatus.tTvWall.byChnnlNum; byLp++)
		{
			if(byLp != byChnlId)
			{
				tMtInTv = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byLp];
				//zjl[20100205]这里先判断会议id，如果当前local的是其他会议的终端则会得到当前会议相同id的终端，导致会议号判断有误
				if (tMtInTv.GetConfIdx() == m_byConfIdx)
				{
					tMtInTv = GetLocalMtFromOtherMcuMt( tMtInTv );
					if(tMtInTv.GetMtId() == byMtId)
					{
						//对于VCS会议来说增加通道模式判断，如果为MODE_NONE则表示为假进墙终端，实际墙中并无该终端图像
						if (VCS_CONF == m_tConf.GetConfSource() && MODE_NONE == g_cMcuVcApp.GetChnnlMMode(byTvWallId,byLp))
						{
							ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU, "[IsMtNotInOtherTvWallChnnl]VCS_CONF Mt(%d) Is Not In (%d,%d)!\n",
								byMtId,byTvWallId,byLp);	
						} 
						else
						{
							return FALSE;
						}
					}
				}
            }
        }
    }
	
    //是否在其他电视墙的通道里
    for(u8 byTvId = TVWALLID_MIN; byTvId <= TVWALLID_MAX; byTvId++)
    {
        if(byTvWallId == byTvId)
            continue;
        
        g_cMcuVcApp.GetPeriEqpStatus(byTvId, &tTWStatus);
        if( tTWStatus.m_byOnline )
        {
            for(u8 byLp = 0; byLp < tTWStatus.m_tStatus.tTvWall.byChnnlNum; byLp++)
            {
                tMtInTv = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byLp];
				//zjl[20100205]这里先判断会议id，如果当前local的是其他会议的终端则会得到当前会议相同id的终端，导致判断会议号有误
				if (tMtInTv.GetConfIdx() == m_byConfIdx)
				{
					tMtInTv = GetLocalMtFromOtherMcuMt( tMtInTv );
					if(tMtInTv.GetMtId() == byMtId)
					{
						//对于VCS会议来说增加通道模式判断，如果为MODE_NONE则表示为假进墙终端，实际墙中并无该终端图像
						if (VCS_CONF == m_tConf.GetConfSource() && MODE_NONE == g_cMcuVcApp.GetChnnlMMode(byTvId,byLp))
						{
							ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU, "[IsMtNotInOtherTvWallChnnl]VCS_CONF Mt(%d) Is Not In (%d,%d)!\n",
								byMtId,byTvId,byLp);	
						} 
						else
						{
							return FALSE;
						}
					}
				}				
            }
        }
    }
	
    return TRUE;
}

/*====================================================================
    函数名      ：ProcTvwallConnectedNotif
    功能        ：电视墙上电通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/09/15    4.0         libo          创建
====================================================================*/
void CMcuVcInst::ProcTvwallConnectedNotif(const CMessage * pcMsg)
{
    if (STATE_ONGOING != CurState())
    {
        //ConfPrint( LOG_LVL_ERROR, MID_MCU_EQP,  "[ProcTvwallConnectedNotif]CurState() = %d\n", CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TPeriEqpRegReq tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();

    TPeriEqpStatus tTvWallStatus;

    TTWSwitchInfo tTWSwitchInfo;
    u8 byMemberType;
    u8 bySrcMtConfIdx;
    u8 bySrcMtId;
    u8 byLoop;
    TEqp tEqp;
	TMt tTvwSrcMt;

    tEqp.SetMcuEqp((u8)tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());

    g_cMcuVcApp.GetPeriEqpStatus(tRegReq.GetEqpId(), &tTvWallStatus);
    u8 byMemberNum = tTvWallStatus.m_tStatus.tTvWall.byChnnlNum;
    for (byLoop = 0; byLoop < byMemberNum; byLoop++)
    {
        bySrcMtConfIdx = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
        if (m_byConfIdx == bySrcMtConfIdx)
        {
            byMemberType = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType;
            bySrcMtId = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMtId();
            if (TW_MEMBERTYPE_MCSSPEC == byMemberType && !m_tConfAllMtInfo.MtJoinedConf(bySrcMtId))
            {
                tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
                tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
            }
        }
    }
    g_cMcuVcApp.SetPeriEqpStatus(tRegReq.GetEqpId(), &tTvWallStatus);

    for (byLoop = 0; byLoop < byMemberNum; byLoop++)
    {
		// [9/20/2011 liuxu] 重新取状态, 以避免后面保存时用旧状态覆盖新状态
		g_cMcuVcApp.GetPeriEqpStatus(tRegReq.GetEqpId(), &tTvWallStatus);

        bySrcMtConfIdx = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
		tTvwSrcMt = (TMt)tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop];
        if (m_byConfIdx == bySrcMtConfIdx && !tTvwSrcMt.IsNull())
        {
            byMemberType = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType;
			if (VCS_CONF == m_tConf.GetConfSource() && ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
				m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE) 
			{
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx( (u8)0 );
				g_cMcuVcApp.SetPeriEqpStatus(tRegReq.GetEqpId(), &tTvWallStatus);
				ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU,"[ProcTvwallConnectedNotif]VCS GROUP REVIEWTW MODE AND Mt(%d,%d) not online,So Continue!\n",
					tTvwSrcMt.GetMcuId(),tTvwSrcMt.GetMtId());
				continue;
			}
            if (0 != byMemberType)
            {
				// [8/30/2011 liuxu] 需要区分各种跟随状态来设定源终端, 因为电视墙重新上线时,
				// 轮询跟随对象很有可能已经变了
				switch (byMemberType)
				{
				case TW_MEMBERTYPE_POLL:
					{
						if(m_tConfPollParam.GetCurrentMtPolled())
							// 设为当前的轮询终端
							tTvwSrcMt = m_tConfPollParam.GetCurrentMtPolled()->GetTMt();
						else
							// 说明当前没有轮询
							tTvwSrcMt.SetNull();
					}					
					break;

				case TW_MEMBERTYPE_CHAIRMAN:
					{
						// 设为当前主席
						tTvwSrcMt = m_tConf.GetChairman();
					}
					break;

				case TW_MEMBERTYPE_SPEAKER:
					{
						tTvwSrcMt = m_tConf.GetSpeaker();
					}
					break;

				default:
					break;
				}

                memset(&tTWSwitchInfo, 0, sizeof(TTWSwitchInfo));
                tTWSwitchInfo.SetMemberType(byMemberType);
                tTWSwitchInfo.SetSrcMt(tTvwSrcMt);
                tTWSwitchInfo.SetDstMt(tEqp);
                tTWSwitchInfo.SetDstChlIdx(byLoop);

                cServMsg.SetMsgBody((u8 *)&tTWSwitchInfo, sizeof(TTWSwitchInfo));

				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ProcTvwallConnectedNotif] tMt(%d,%d) bMemberType.%d\n",
					tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMcuId(),
					tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMtId(),
					byMemberType
					);

				//20101026_tzy VCS会议时，单回传时,同一个下级但原来电视墙终端与当前上传终端不一致则不用恢复
				TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();
				if (VCS_CONF == m_tConf.GetConfSource() && !tCurVCMT.IsNull()
					&& !tCurVCMT.IsLocal() && tCurVCMT.GetMcuIdx() == tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMcuIdx()
					&& tCurVCMT.GetMtId() != tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMtId()
					&& !IsLocalAndSMcuSupMultSpy(tCurVCMT.GetMcuIdx()))
				{
					tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
					tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
					tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx( (u8)0 );
					g_cMcuVcApp.SetPeriEqpStatus(tRegReq.GetEqpId(), &tTvWallStatus);
					cServMsg.SetMsgBody((u8 *)&tTvWallStatus, sizeof(tTvWallStatus));
					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
					continue;
				}

				cServMsg.SetEventId(MCS_MCU_START_SWITCH_TW_REQ);

				// [9/20/2011 liuxu] 补丁, StartSwitchTWReq中会判断源是不是已经在目的tvw通道中,如果是就会中止
				// 解决策略是这里先清空
                tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
                tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
				g_cMcuVcApp.SetPeriEqpStatus(tRegReq.GetEqpId(), &tTvWallStatus);

				StartSwitchTWReq(cServMsg);
            }
        }
    }

    return;
}

/*====================================================================
    函数名      ：ProcTvwallDisconnectedNotif
    功能        ：电视墙断链
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/05/17    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcTvwallDisconnectedNotif(const CMessage * pcMsg)
{
    if (STATE_ONGOING != CurState())
    {
        //ConfPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[TvwallDisconnectedNotif]CurState() = %d\n", CurState());
        return;
    }
	
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();
	
    TPeriEqpStatus tTvWallStatus;
    u8 bySrcMtConfIdx;
	
    g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tTvWallStatus);
    u8 byMemberNum = tTvWallStatus.m_tStatus.tTvWall.byChnnlNum;
    for (u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
    {
        bySrcMtConfIdx = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
        if (m_byConfIdx == bySrcMtConfIdx)
        {
			TMt tChnMt = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop];
			RefreshMtStatusInTw(tChnMt, FALSE, FALSE);
            StopSwitchToPeriEqp(tEqp.GetEqpId(), byLoop);
			
			//  [12/9/2009 pengjie] Modify 级联多回传支持，这里尝试释放回传资源
			FreeRecvSpy( tChnMt, MODE_BOTH );


//             bySrcMtId = GetLocalMtFromOtherMcuMt( tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop] ).GetMtId();
//             m_ptMtTable->SetMtInTvWall(bySrcMtId, FALSE);
// 			TMt tMt = m_ptMtTable->GetMt( bySrcMtId );
//             MtStatusChange( &tMt );
// 			
//             StopSwitchToPeriEqp(tEqp.GetEqpId(), byLoop);
// 			//  [12/9/2009 pengjie] Modify 级联多回传支持，这里尝试释放回传资源
// 			FreeRecvSpy( (TMt)tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop], MODE_BOTH );

			// [8/30/2011 liuxu] 重新上线后, 被轮询终端很可能已经变了
			if (TW_MEMBERTYPE_POLL == tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType)
			{
				//轮训视频跟随属性不变，终端清空，但所属会议不能清，以便上线后可以轮训视频跟随
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
			}
			if (VCS_CONF == m_tConf.GetConfSource() && ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
				m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE)
			{
				//VCS组呼预览电视墙模式下，电视墙掉线后再上线不做恢复
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
			}
			if(VCS_CONF == m_tConf.GetConfSource() && m_cVCSConfStatus.GetMtInTvWallCanMixing() && 
				!(tChnMt == m_cVCSConfStatus.GetCurVCMT()) && 
				(
				(tChnMt.IsLocal() && m_ptMtTable->IsMtInMixing(tChnMt.GetMtId())) || 
				(!tChnMt.IsLocal() && m_ptConfOtherMcTable->IsMtInMixing(tChnMt))
				) &&
				IsMtNotInOtherTvWallChnnl(tChnMt,tEqp.GetEqpId(),0,TRUE) && IsMtNotInOtherHduChnnl(tChnMt,0,0)
			  )
			{
				RemoveSpecMixMember(&tChnMt,1);
			}
		}
    }

	g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tTvWallStatus);

    //  zjlhdupoll 
    // 处理电视墙多通道轮询
	for(u8 byIdx = 0; byIdx < MAXNUM_PERIEQP_CHNNL; byIdx++)
	{
		TTvWallPollParam tTWPollParam;
		memset(&tTWPollParam, 0, sizeof(TTvWallPollParam));
		if (m_tTWMutiPollParam.GetTWPollParam(tEqp.GetEqpId(), byIdx, tTWPollParam) &&
			POLL_STATE_NONE != tTWPollParam.GetPollState())
		{
			TMtPollParam tMtPollParam;
			memset(&tMtPollParam, 0, sizeof(tMtPollParam));
			tTWPollParam.SetMtPollParam(tMtPollParam);
			tTWPollParam.SetPollState(POLL_STATE_NONE);
			
			cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
			SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
			
			u32 dwTimerIdx = 0;
			if (!m_tTWMutiPollParam.GetTimerIdx(tEqp.GetEqpId(), byIdx, dwTimerIdx))
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcTvwallDisconnectedNotif] Cannot get TW<EqpId:%d, ChnId:%d> TimerIdx",
					tEqp.GetEqpId(), byIdx);
			} 
			KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
		}
	}
}

//4.6 新加  jlb
/*====================================================================
    函数名      ProcHduMcuStatusNotif
    功能        ：Hdu状态通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/01/20    4.6         江乐斌        创建
====================================================================*/
// void CMcuVcInst::ProcHduMcuStatusNotif( const CMessage * pcMsg )
// {
// 	return;
// }

/*====================================================================
    函数名      ProcHduConnectedNotif
    功能        ：hdu连接通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/01/20    4.6         江乐斌        创建
====================================================================*/
void CMcuVcInst::ProcHduConnectedNotif(const CMessage * pcMsg)
{
     if (STATE_ONGOING != CurState())
     {
		 ConfPrint( LOG_LVL_ERROR, MID_MCU_HDU, "[ProcHduConnectedNotif]CurState() = %d\n", CurState());
         return;
     }
	 
     CServMsg cServMsg(pcMsg->content, pcMsg->length);
     TPeriEqpRegReq tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();
 
	 TEqp tEqp;
     tEqp.SetMcuEqp((u8)tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());
	 ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,  "[ProcHduConnectedNotif] HDU ID CONNECTED(%d) \n",tRegReq.GetEqpId() );
 
	 u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(tEqp);
	 if (0 == byHduChnNum)
	 {
		 ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU,"[ProcHduConnectedNotif] GetHduChnNumAcd2Eqp failed!\n");
		 return;
	 }

	 THduSwitchInfo tHduSwitchInfo;
     u8 byMemberType = 0;
     u8 bySrcMtConfIdx;
	 u8 bySchmeIdx;
	 TMt tInHduMt;
	 TPeriEqpStatus tHduStatus;
	 TTvWallPollParam tTWPollParam;

	 BOOL32 bIsChgHduPicture = TRUE;	// [2013/08/14 chenbing] 是否改变HDU显示背景图片 
     for (u8 byLoop = 0; byLoop < byHduChnNum; byLoop++)
     {
		 // [12/15/2010 xliang] 因为之后对tHduStatus有变动，所以每次循环进来都应获取一下最新的tHduStatus
		 g_cMcuVcApp.GetPeriEqpStatus(tRegReq.GetEqpId(), &tHduStatus);
		 tHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetStatus(THduChnStatus::eREADY);
		 // [2013/03/11 chenbing] 保存此HDU支持的最大通道数
		 tHduStatus.m_tStatus.tHdu.SetChnnlNum(tRegReq.GetChnnlNum());

		 g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);
		 //[nizhijun 2010/10/29]处理HDU上线(掉线HDU) begin
		 memset(&tTWPollParam, 0, sizeof(TTvWallPollParam));
		 if (m_tTWMutiPollParam.GetTWPollParam(tEqp.GetEqpId(), byLoop, tTWPollParam))
		 {
			 //通道轮询中，HDU断线后又上线处理
			 if (POLL_STATE_NORMAL == tTWPollParam.GetPollState())
			 {
				 cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				 SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
				 
				 u32 dwSetTimer = 0;
				 u32 dwTimerIdx = 0;
				 if(m_tTWMutiPollParam.GetTimerIdx(tEqp.GetEqpId(), byLoop, dwTimerIdx))
				 {
					 dwSetTimer = tTWPollParam.GetKeepTime();
					 SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx, 1000 * dwSetTimer, dwTimerIdx);
				 }
				 else
				 {
					 ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF," [ProcHduConnectedNotif] Cannot HDU<EqpId:%d, ChnId:%d> getTimerIdx!\n",tEqp.GetEqpId(), byLoop);
				 }
				 
			 }
			 //通道轮询暂停中，HDU断线后又上线处理
			 else if(POLL_STATE_PAUSE == tTWPollParam.GetPollState())
			 {
				 cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
				 SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
			 }
		 }
		 else
		 {
			// [2013/03/11 chenbing] 四风格下的处理 
			if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byLoop) )
			{
				BOOL32 bIsChgHduMode = TRUE;
				//开启HDU通道
				for (u8 byIndex = 0; byIndex < HDU_MODEFOUR_MAX_SUBCHNNUM; byIndex++)
				{
					g_cMcuVcApp.GetPeriEqpStatus(tRegReq.GetEqpId(), &tHduStatus);
					tInHduMt = (TMt)tHduStatus.m_tStatus.tHdu.GetChnMt(byLoop, byIndex);
					bySrcMtConfIdx = tHduStatus.m_tStatus.tHdu.GetConfIdx(byLoop, byIndex);
					bySchmeIdx     = tHduStatus.m_tStatus.tHdu.GetSchemeIdx(byLoop, byIndex);
					if (m_byConfIdx == bySrcMtConfIdx )
					{
						bIsChgHduMode = bIsChgHduMode ? TRUE : FALSE;
						if( !tInHduMt.IsNull() )
						{
							byMemberType = tHduStatus.m_tStatus.tHdu.GetMemberType(byLoop, byIndex);
							memset(&tHduSwitchInfo, 0, sizeof(THduSwitchInfo));
							tHduSwitchInfo.SetMemberType(byMemberType);
							tHduSwitchInfo.SetSrcMt(tInHduMt);
							tHduSwitchInfo.SetDstMt(tEqp);
							tHduSwitchInfo.SetDstChlIdx(byLoop);
							tHduSwitchInfo.SetSchemeIdx(bySchmeIdx);
							// [2013/03/11 chenbing] 追加子通道
							tHduSwitchInfo.SetDstSubChn(byIndex);
							cServMsg.SetMsgBody((u8 *)&tHduSwitchInfo, sizeof(THduSwitchInfo));	
							cServMsg.SetEventId(MCS_MCU_START_SWITCH_HDU_REQ);
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU,  "[ProcHduConnectedNotif]srcmt(%d,%d) channel(%d)\n",
								tHduStatus.m_tStatus.tHdu.GetMcuId(byLoop, byIndex),
								tHduStatus.m_tStatus.tHdu.GetMtId(byLoop, byIndex),
								byLoop
								);

							if (byIndex)
							{
								tHduStatus.m_tStatus.tHdu.SetConfIdx(0, byLoop, byIndex);
							}
							tHduStatus.m_tStatus.tHdu.SetChnNull(byLoop, byIndex);
							tHduStatus.m_tStatus.tHdu.SetMemberType(TW_MEMBERTYPE_NULL, byLoop, byIndex);
							tHduStatus.m_tStatus.tHdu.SetSchemeIdx(0, byLoop, byIndex);
							g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);
							StartSwitchHduReq( cServMsg );
							bIsChgHduMode = FALSE;
							bIsChgHduPicture = FALSE;
						}
					}
					else
					{
						bIsChgHduMode = FALSE;
					}
				}//for over
				
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU,  "[ProcHduConnectedNotif]bIsChgHduMode (%d)\n", bIsChgHduMode);
				// 四风格通道中无终端时也要恢复为四风格
				if ( bIsChgHduMode )
				{
					u8 byHduMode = HDUCHN_MODE_FOUR;
					CServMsg cSMsg;
					cSMsg.SetEqpId(tRegReq.GetEqpId());
					cSMsg.SetChnIndex(byLoop);
					cSMsg.SetMsgBody((u8 *)&byHduMode, sizeof(u8));
					SendChangeHduModeReq(cSMsg);
				}
			
				continue;
			}

			// [2013/03/11 chenbing]  一风格下的处理 
			tInHduMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop];
			bySrcMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
			bySchmeIdx     = tHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].GetSchemeIdx();
			if (m_byConfIdx == bySrcMtConfIdx && !tInHduMt.IsNull())
			{
				byMemberType = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType;
				if (VCS_CONF == m_tConf.GetConfSource() && ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
					m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE) 
				{
					tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
					tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
					tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = TW_MEMBERTYPE_NULL;
					g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);
					ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU,"[ProcHduConnectedNotif]VCS GROUP REVIEWTW MODE AND Mt(%d,%d) not online,So Continue!\n",
						tInHduMt.GetMcuId(),tInHduMt.GetMtId());
					continue;
				}
				memset(&tHduSwitchInfo, 0, sizeof(THduSwitchInfo));
				tHduSwitchInfo.SetMemberType(byMemberType);
				tHduSwitchInfo.SetSrcMt(tInHduMt);
				tHduSwitchInfo.SetDstMt(tEqp);
				tHduSwitchInfo.SetDstChlIdx(byLoop);
				tHduSwitchInfo.SetSchemeIdx(bySchmeIdx);
				cServMsg.SetMsgBody((u8 *)&tHduSwitchInfo, sizeof(THduSwitchInfo));	
				cServMsg.SetEventId(MCS_MCU_START_SWITCH_HDU_REQ);     
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU,  "[ProcHduConnectedNotif]srcmt(%d,%d) channel(%d)\n",
					 tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetMcuId(),
					 tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetMtId(),
					 byLoop
					 );

				//[nizhijun 2010/11/23]对于Bug00041995，这里取到tHduSwitchInfo后，需将tmt在清空下，
				//因为在changhduswitch中如果是级联的情况，如果tmt存在的话，会清空该终端回传的信息
				tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
				tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
				tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = TW_MEMBERTYPE_NULL;
				tHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetSchemeIdx(0);
				g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);
				StartSwitchHduReq( cServMsg );
				bIsChgHduPicture = FALSE;
			}
		 }
     }

	 // 改变HDU背景图片
	 //Bug00159500:HDU2-S--背景图片设置为黑屏，电视墙重启后，背景图像变为默认图像
	 if ( bIsChgHduPicture )
	 {
		 TEqpExCfgInfo tEqpExCfgInfo;
		 u16 wRet = g_cMcuAgent.GetEqpExCfgInfo( tEqpExCfgInfo );
		 if (SUCCESS_AGENT != wRet)
		 {
			 ConfPrint(LOG_LVL_ERROR, MID_MCU_CFG, "[ProcHduConnectedNotif] McuAgent GetEqpExCfgInfo failed. ret:%d!\n", wRet);
			 return;
		 }
		
		 u8 byVidPlayPolicy = tEqpExCfgInfo.m_tHDUExCfgInfo.m_byIdleChlShowMode;
		 OspPost(MAKEIID(AID_MCU_PERIEQPSSN, tRegReq.GetEqpId()), MCU_HDU_CHANGEPLAYPOLICY_NOTIF, (void*)&byVidPlayPolicy, sizeof(u8));
     }
}

/*====================================================================
    函数名      ：ProcHduDisconnectedNotif
    功能        ：Hdu断链
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/01/20    4.6         江乐斌        创建
====================================================================*/
void CMcuVcInst::ProcHduDisconnectedNotif( const CMessage * pcMsg )
{
	if (STATE_ONGOING != CurState())
    {
        return;
    }
	
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();
	
	u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(tEqp);
	if (0 == byHduChnNum)
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[ProcHduDisconnectedNotif] GetHduChnNumAcd2Eqp failed!\n");
		return;
	}

    TPeriEqpStatus tHduStatus;
    g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);

    u8 bySrcMtConfIdx = 0;

	for (u8 byLoop = 0; byLoop < byHduChnNum; byLoop++)
    { 
		bySrcMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
		// [2013/03/11 chenbing] 四风格下清除每个子通道状态 
		if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byLoop) )
		{
			BOOL32 bIsStopSwitch = FALSE;
			for (u8 byIndex=0; byIndex<HDU_MODEFOUR_MAX_SUBCHNNUM; byIndex++)
			{
				if ( m_byConfIdx == bySrcMtConfIdx )
				{
					TMt tChnMt = tHduStatus.m_tStatus.tHdu.GetChnMt(byLoop, byIndex);
					RefreshMtStatusInTw(tChnMt, FALSE, TRUE);
					ChangeMtResFpsInHduVmp(tChnMt, tEqp.GetEqpId(), byLoop, byIndex, bIsStopSwitch, FALSE);
					StopSwitchToPeriEqp(tEqp.GetEqpId(), byLoop, TRUE, MODE_VIDEO, SWITCH_MODE_BROADCAST, byIndex);
					// 级联多回传支持，这里尝试释放回传资源
					FreeRecvSpy( tChnMt, MODE_VIDEO );
				}
				tHduStatus.m_tStatus.tHdu.SetChnStatus(byLoop, byIndex, THduChnStatus::eIDLE);
			}
		}
		else
		{
			tHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetStatus(THduChnStatus::eIDLE);
			if (m_byConfIdx == bySrcMtConfIdx)
			{
				TMt tChnMt = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop];
				RefreshMtStatusInTw(tChnMt, FALSE, TRUE);
				StopSwitchToPeriEqp(tEqp.GetEqpId(), byLoop);
				
				//  [12/9/2009 pengjie] Modify 级联多回传支持，这里尝试释放回传资源
				FreeRecvSpy( tChnMt, MODE_BOTH );
				
				// [8/30/2011 liuxu] 重新上线后, 被轮询终端很可能已经变了
				if (TW_MEMBERTYPE_POLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType)
				{
					//轮训视频跟随属性不变，终端清空，但所属会议不能清，以便上线后可以继续轮训视频跟随
					tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
				}
				if (VCS_CONF == m_tConf.GetConfSource() && ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
					m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE)
				{
					//VCS组呼预览电视墙模式下，电视墙掉线后再上线不做恢复
					tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
					tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
				}
				if(VCS_CONF == m_tConf.GetConfSource() && m_cVCSConfStatus.GetMtInTvWallCanMixing() && 
					!(tChnMt == m_cVCSConfStatus.GetCurVCMT()) && 
					(
					(tChnMt.IsLocal() && m_ptMtTable->IsMtInMixing(tChnMt.GetMtId())) || 
					(!tChnMt.IsLocal() && m_ptConfOtherMcTable->IsMtInMixing(tChnMt))
					) &&
					IsMtNotInOtherTvWallChnnl(tChnMt,0,0) && IsMtNotInOtherHduChnnl(tChnMt,tEqp.GetEqpId(),0,TRUE)
					)
				{
					RemoveSpecMixMember(&tChnMt,1);
				}
			}
	    }
	}
	g_cMcuVcApp.SetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);
    //zjlhdupoll
	// 处理电视墙多通道轮询
	for(u8 byIdx = 0; byIdx < byHduChnNum; byIdx++)
	{
		TTvWallPollParam tTWPollParam;
		memset(&tTWPollParam, 0, sizeof(TTvWallPollParam));
		if (m_tTWMutiPollParam.GetTWPollParam(tEqp.GetEqpId(), byIdx, tTWPollParam) &&
			POLL_STATE_NONE != tTWPollParam.GetPollState())
		{
			TMtPollParam tMtPollParam;
			memset(&tMtPollParam, 0, sizeof(tMtPollParam));
			tTWPollParam.SetMtPollParam(tMtPollParam);
			tTWPollParam.SetPollState(POLL_STATE_NONE);
			
			cServMsg.SetMsgBody((u8*)&tTWPollParam, sizeof(TTvWallPollParam));
			SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
			
			u32 dwTimerIdx = 0;
			if (!m_tTWMutiPollParam.GetTimerIdx(tEqp.GetEqpId(), byIdx, dwTimerIdx))
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcTvwallDisconnectedNotif] Cannot get HDU<EqpId:%d, ChnId:%d> TimerIdx!\n",
					tEqp.GetEqpId(), byIdx);
			} 
			KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
		}
	}

	return;	
}
/*====================================================================
    函数名      ：CreateConfEqpInsufficientNack
    功能        ：外设不充分统一处理
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/05/12    4.0         张宝卿        创建
====================================================================*/
void CMcuVcInst::CreateConfEqpInsufficientNack( u8 byCreateMtId, u8 byOldInsId, const s32 nErrCode, CServMsg &cServMsg )
{    
    if( CONF_CREATE_MT == cServMsg.GetSrcMtId() )
    {
        // guzh [7/11/2007] 先发消息后删除
        CServMsg cMtMsg;
        SendMsgToMt( byCreateMtId, MCU_MT_DELMT_CMD, cMtMsg );

        // xsl [11/10/2006] 终端创会失败时释放资源
        g_cMcuVcApp.DecMtAdpMtNum( m_ptMtTable->GetDriId(byCreateMtId), m_byConfIdx, byCreateMtId );
		m_ptMtTable->SetDriId(byCreateMtId, 0);   
		
		// xliang [2/4/2009] 对于终端类型是MCU的情形，接入板终端计数值还需-1
// 		u8 byMtType = m_ptMtTable->GetMtType(byCreateMtId);
// 		if( MT_TYPE_MMCU == byMtType || MT_TYPE_SMCU == byMtType )
// 		{
// 			g_cMcuVcApp.DecMtAdpMtNum( m_ptMtTable->GetDriId(byCreateMtId), m_byConfIdx, byCreateMtId, FALSE,TRUE );
// 		}

		g_cMcuVcApp.UpdateAgentAuthMtNum();
    }

	//释放prs
	ReleaseAllPrsChn();
	//释放bas
	RlsAllBasForConf();
	// 释放Prs通道
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnl )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnl = EQP_CHANNO_INVALID;
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnl2 )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl2].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnl2 = EQP_CHANNO_INVALID;
// 	}
// 		
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlAud )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAud].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlAud = EQP_CHANNO_INVALID;
// 	}
// 	
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlVidBas )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVidBas].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlVidBas = EQP_CHANNO_INVALID;
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlBrBas )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlBrBas].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlBrBas = EQP_CHANNO_INVALID;
// 	}
// 	
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlAudBas )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAudBas].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlAudBas = EQP_CHANNO_INVALID;
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut1 )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut1].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlVmpOut1 = EQP_CHANNO_INVALID;
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut2 )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut2].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlVmpOut2 = EQP_CHANNO_INVALID;
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut3 )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut3].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlVmpOut3 = EQP_CHANNO_INVALID;
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut4 )
// 	{
// 		TPeriEqpStatus tStatus;
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
//         tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut4].SetReserved(FALSE);                        
//         tStatus.SetConfIdx(m_byConfIdx);                        
//         g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
//         m_byPrsChnnlVmpOut4 = EQP_CHANNO_INVALID;
// 	}

	// 释放Bas通道
// 	if( EQP_CHANNO_INVALID != m_byAudBasChnnl )
// 	{				
//         g_cMcuVcApp.SetBasChanReserved(m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl, FALSE);
//         m_byAudBasChnnl = EQP_CHANNO_INVALID;
// 		memset( &m_tAudBasEqp, 0, sizeof(m_tAudBasEqp) );
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byVidBasChnnl )
// 	{
//         g_cMcuVcApp.SetBasChanReserved(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, FALSE);	
// 		m_byVidBasChnnl = EQP_CHANNO_INVALID;
// 		memset( &m_tVidBasEqp, 0, sizeof(m_tVidBasEqp) );
// 	}
// 
// 	if( EQP_CHANNO_INVALID != m_byBrBasChnnl )
// 	{
//         g_cMcuVcApp.SetBasChanReserved(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, FALSE);			
//         m_byBrBasChnnl = EQP_CHANNO_INVALID;
// 		memset( &m_tBrBasEqp, 0, sizeof(m_tBrBasEqp) );
// 	}

	{   // vmp
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tStatus );
		if( TVmpStatus::RESERVE == tStatus.m_tStatus.tVmp.m_byUseState )
		{
			tStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;//先占用,超时后未成功再放弃
			tStatus.SetConfIdx( m_byConfIdx );
			g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tStatus );
		}
	}

	CreateConfSendMcsNack(byOldInsId, nErrCode, cServMsg, TRUE);
}

/*====================================================================
    函数名      ：CreateConfSendMcsNack
    功能        ：按错误码返回NACK给MCS
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/05/15    4.0         张宝卿        创建
====================================================================*/
void CMcuVcInst::CreateConfSendMcsNack( u8 byOldInsId, const s32 nErrCode, CServMsg &cServMsg, BOOL32 bDeleteAlias )
{
	if (CONF_CREATE_SCH == cServMsg.GetSrcMtId())
	{
		//[2011/11/23/zhangli]mcs侧加了限制，如果m_cConfId不相等则不处理，避免提示其他会议的警告。为了解决这个问题，
		//mcs侧加“m_cConfId!=NULL”一定会提示的条件，这里不设置cSMsg的m_cConfId，直接下发消息
		//NotifyMcsAlarmInfo(0, nErrCode);
		CServMsg cSMsg;
		cSMsg.SetEventId(MCU_MCS_ALARMINFO_NOTIF);
		cSMsg.SetErrorCode(nErrCode);
		cSMsg.SetMcuId(LOCAL_MCUID);
		CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ALARMINFO_NOTIF, cSMsg.GetServMsg(), cSMsg.GetServMsgLen());
	}
	else
	{
		cServMsg.SetErrorCode(nErrCode);
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
	}

    if (bDeleteAlias)
    {
        DeleteAlias();
    }    
    
    g_cMcuVcApp.SetConfMapInsId(m_byConfIdx, byOldInsId);

	ClearVcInst();
    return;
}

/*====================================================================
    函数名      ：AddSecDSCapByDebug
    功能        ：根据debug调整是否需要第二双流能力（第一双流能力需为H239/H264）
    算法实现    ：临时写死为：h263+ XGA 5fps
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/05/15    4.0         张宝卿        创建
====================================================================*/
void CMcuVcInst::AddSecDSCapByDebug(TConfInfo &tConfInfo)
{
    if (!g_cMcuVcApp.IsSupportSecDSCap())
    {
        return;
    }

    if (MEDIA_TYPE_H264 == tConfInfo.GetDStreamMediaType() &&
        tConfInfo.GetCapSupport().IsDStreamSupportH239())
    {
        TCapSupportEx tCapEx = tConfInfo.GetCapSupportEx();

        tCapEx.SetSecDSBitRate(tConfInfo.GetBitRate()); //同第一路双流，暂填全能力
        tCapEx.SetSecDSFrmRate(5);
        tCapEx.SetSecDSType(MEDIA_TYPE_H263PLUS);
        tCapEx.SetSecDSRes(VIDEO_FORMAT_XGA);

        tConfInfo.SetCapSupportEx(tCapEx);
        ConfPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[AddSecDSCapByDebug] add dual ds cap<h263+, XGA, 5fps>\n");
    }
    return;
}


/*=============================================================================
函 数 名： NotifyMcsAlarmInfo
功    能： 通知mcs提示信息消息
算法实现： 
全局变量： 
参    数： u16 wErrorCode
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/8/9  4.0			许世林                  创建
=============================================================================*/
void CMcuVcInst::NotifyMcsAlarmInfo(u8 byMcsId, u16 wErrorCode)
{
    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_MCS_ALARMINFO_NOTIF);
    cServMsg.SetErrorCode(wErrorCode);
	// 填写会议索引和会议号, zgc, 2007-03-26
	u16 wInsID = GetInsID();
	if( DAEMON == wInsID )
	{
		cServMsg.SetConfIdx( 0 );
	}
	else
	{
		cServMsg.SetConfIdx( m_byConfIdx );
		cServMsg.SetConfId( m_tConf.GetConfId() );
	}

    if (0 == byMcsId)
    {
        SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cServMsg);
    }
    else
    {
        SendMsgToMcs(byMcsId, MCU_MCS_ALARMINFO_NOTIF, cServMsg);
    }    

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MCS, "NotifyMcsAlarmInfo(%d, %d)\n", byMcsId, wErrorCode);

    return;
}

/*====================================================================
    函数名      ：ProcTimerAutoRec
    功能        ：处理会议开始后自动录像定时器到
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/07/18    4.0         顾振华        创建
====================================================================*/
void CMcuVcInst::ProcTimerAutoRec( const CMessage * pcMsg )
{
    // 结束定时器
    KillTimer( pcMsg->event );

    BOOL32 bMsgStart = FALSE;
    BOOL32 bMsgEnd = FALSE;

    time_t tiCurTime = time(NULL);
    
    TConfAutoRecAttrb tAttrib = m_tConf.GetAutoRecAttrb();

    time_t tiStartTime = 0;
    time_t tiEndTime = 0;
    tAttrib.GetStartTime().GetTime(tiStartTime);
    TKdvTime kdvEndTime = tAttrib.GetEndTime();
    kdvEndTime.GetTime( tiEndTime );

    if ( tiStartTime <= tiCurTime || tiStartTime - tiCurTime <= 10)
    {
        // 应该开始自动录像了
        bMsgStart = TRUE;
    }
    if (kdvEndTime.GetYear() != 0)
    {
        if ( tiEndTime <= tiCurTime || tiEndTime - tiCurTime <= 10 )
        {
            // 应该结束录像了
            bMsgEnd = TRUE;
        }
    }

    if (bMsgStart && !bMsgEnd)
    {
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "Time to Start Auto Record...\n");
        
        // 如果已经在录像，则不理睬本消息。
        if ( m_tConf.m_tStatus.IsNoRecording() )
        {    
			TMtAlias tVrsCallAlias;
			BOOL32 bIsVrsRecorder = GetVrsCallAliasByVrsCfgId(tAttrib.GetRecEqp(), tVrsCallAlias);
            // 如果录像机在线
            if (bIsVrsRecorder || g_cMcuVcApp.IsRecorderOnline(tAttrib.GetRecEqp()) )
            {
                // 创建一条开始录像消息 MCS_MCU_STARTREC_REQ消息内容：
				// TMt + TEqp + TRecStartPara + 录像名 + TSimCapSet + TVideoStreamCap + TAudioTypeDesc + tMtalias + 用户组Id
                CServMsg cServMsg;
                TMt tMt;
                tMt.SetNull();
                cServMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));
                TEqp tRec;
				// vrs按空eqp标识
				if (!bIsVrsRecorder)
				{
					tRec.SetMcuEqp( LOCAL_MCUID, tAttrib.GetRecEqp(), EQP_TYPE_RECORDER );
				}
                cServMsg.CatMsgBody((u8*)&tRec, sizeof(TEqp));

                TRecStartPara tParam = tAttrib.GetRecParam();
                cServMsg.CatMsgBody((u8*)&tParam, sizeof(TRecStartPara));
                s8 aszRecName[KDV_MAX_PATH] = {0};
    
                // xsl [8/17/2006] 会控上不再指定录像文件名，否则会议模板只能用一次，第二次就会因录像文件名重复二导致会议录像失败
//                if (strlen( tAttrib.GetRecName() ) == 0 )
//                {
                    // 用户没填，自动按照时间产生
                    time_t tTime = time(NULL);
                    TKdvTime tKdvTime;
                    tKdvTime.SetTime( &tTime );

					//  [5/31/2013 guodawei] 自动录像适应低速率
					u16 wBitrate = 0;
					if (tParam.IsRecLowStream())
					{
						wBitrate = m_tConf.GetSecBitRate() + GetAudioBitrate(m_tConf.GetMainAudioMediaType());
					}
                    else
					{
						wBitrate = m_tConf.GetBitRate() + GetAudioBitrate(m_tConf.GetMainAudioMediaType());
					}
					
					//会议名称中的下划线'_'作为录像文件名称，属于非法字符，用'-'替换[6/28/2012 chendaiwei]
					s8 achConfName[MAXLEN_CONFNAME+1] = {0};
					memcpy(achConfName,m_tConf.GetConfName(),MAXLEN_CONFNAME+1);
					for( u8 byIdx = 0; byIdx < MAXLEN_CONFNAME+1; byIdx++)
					{
						if(achConfName[byIdx] == '_')
						{
							achConfName[byIdx] = '-';
						}
					}
			
                    sprintf(aszRecName, "%s-%04d-%02d-%02d %02d-%02d-%02d_%d_%dK",
                            achConfName,
                            tKdvTime.GetYear(),
                            tKdvTime.GetMonth(),
                            tKdvTime.GetDay(), 
                            tKdvTime.GetHour(),
                            tKdvTime.GetMinute(),
                            tKdvTime.GetSecond(),
                            m_tConf.GetUsrGrpId(),
                            wBitrate );
					
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "Rec filname is %s\n",aszRecName);

//                }
//                else
//                {
//                    strncpy(aszRecName, tAttrib.GetRecName(), KDV_MAX_PATH - 1);
//                }
				if (bIsVrsRecorder)
				{
					// vrs新录播别名长度按129解析
					aszRecName[KDV_NAME_MAX_LENGTH] = 0;
					cServMsg.CatMsgBody((u8*)aszRecName, KDV_NAME_MAX_LENGTH+1);
				}
				else
				{
					// 录像机外设别名长度按128解析
					aszRecName[KDV_NAME_MAX_LENGTH-1] = 0;
					cServMsg.CatMsgBody((u8*)aszRecName, KDV_NAME_MAX_LENGTH);
				}

				// 主流适配参数下空
				TSimCapSet tSimCap;
                cServMsg.CatMsgBody((u8*)&tSimCap, sizeof(tSimCap));
				// 双流适配参数下空
				TVideoStreamCap tSecCapSet;
                cServMsg.CatMsgBody((u8*)&tSecCapSet, sizeof(tSecCapSet));
				// 音频适配参数下空
				TAudioTypeDesc tAudCapSet;
                cServMsg.CatMsgBody((u8*)&tAudCapSet, sizeof(tAudCapSet));
				// tMtalias(新录播alias)
                cServMsg.CatMsgBody((u8*)&tVrsCallAlias, sizeof(tVrsCallAlias));

                cServMsg.SetEventId(MCS_MCU_STARTREC_REQ);
                cServMsg.SetConfIdx(m_byConfIdx);
                cServMsg.SetConfId( m_tConf.GetConfId() );
                cServMsg.SetNoSrc();

				CMessage cMsg;
				cMsg.length = cServMsg.GetServMsgLen();
				cMsg.content = cServMsg.GetServMsg();
				ProcMcsMcuStartRecReq(&cMsg);
//                 g_cMcuVcApp.SendMsgToConf(m_byConfIdx, 
//                                           MCS_MCU_STARTREC_REQ, 
//                                           cServMsg.GetServMsg(),
//                                           cServMsg.GetServMsgLen());
            }

        }

        // 设置结束自动录像定时器
        if ( kdvEndTime.GetYear() != 0 && 
            g_cMcuVcApp.IsRecorderOnline(tAttrib.GetRecEqp()) )
        {
            // guzh [4/17/2007] 采用绝对定时器
            SetAbsTimer(MCUVC_CONFSTARTREC_TIMER, 
                        kdvEndTime.GetYear(),
                        kdvEndTime.GetMonth(),
                        kdvEndTime.GetDay(),
                        kdvEndTime.GetHour(),
                        kdvEndTime.GetMinute(),
                        kdvEndTime.GetSecond() );
        }

        return;
    }

    if (bMsgEnd)
    {
        // 自动结束时间到
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "Time to End Auto Record...\n");

        // 如果不在录像，则不理睬本消息。
        if (!m_tConf.m_tStatus.IsNoRecording())
        {
            CServMsg cServMsg;
            cServMsg.SetEventId(MCS_MCU_STOPREC_REQ);
            cServMsg.SetConfIdx(m_byConfIdx);
            cServMsg.SetConfId( m_tConf.GetConfId() );
            cServMsg.SetNoSrc();

            TMt tMt;
            tMt.SetNull();
            cServMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));

            g_cMcuVcApp.SendMsgToConf(m_byConfIdx, 
                                      MCS_MCU_STOPREC_REQ, 
                                      cServMsg.GetServMsg(),
                                      cServMsg.GetServMsgLen());

        }

		//  [12/26/2009 pengjie] Modify 这里不能就这样结束录像了，不然明天还要录像的话没有定时器去触法了
		u16 wErrCode = 0;
		SetAutoRec(wErrCode);
		// End Modify

        return;
    }

    // 其他情况，不应该发生
    u32 dwTimerSpace = 1000;
    if ( tiStartTime > tiCurTime )
    {
        dwTimerSpace = (tiStartTime - tiCurTime) * 1000;        
        SetTimer( MCUVC_CONFSTARTREC_TIMER, dwTimerSpace );
    }
    else if ( tiEndTime >  tiCurTime )
    {
        dwTimerSpace = (tiEndTime - tiCurTime) * 1000;        
        SetTimer( MCUVC_CONFSTARTREC_TIMER, dwTimerSpace );
    }    
}

/*=============================================================================
函 数 名： ProcTimerUpdataAutoRec
功    能： 刷新自动录像定时器
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20091225    4.0			pengjie                create
=============================================================================*/
void CMcuVcInst::ProcTimerUpdataAutoRec( const CMessage * pcMsg )
{
	// 结束定时器
    KillTimer( pcMsg->event );

	u16 wErrCode = 0;     
	SetAutoRec( wErrCode );

	return;
}

/*=============================================================================
函 数 名： ProcTimerConfHasBrdSrc
功    能： 设置定时器3秒内无广播源，则暂停录像（界面不感知）
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20100929    4.6			pengjie                create
=============================================================================*/
void CMcuVcInst::ProcTimerConfHasBrdSrc( void )
{
	KillTimer( MCUVC_CHECK_CONFBRDSRC_TIMER );

	// [12/15/2010 liuxu]如果此时会议录像结束了, 则无操作
	if (m_tConf.m_tStatus.IsNoRecording())
	{
		return;
	}

	CServMsg cServMsg;
	TMt tMt;
	tMt.SetNull();
	cServMsg.SetChnIndex( m_byRecChnnl );
	cServMsg.SetMsgBody( (u8*)&tMt, sizeof( tMt ) );
	cServMsg.CatMsgBody( (u8*)&m_tRecEqp, sizeof( m_tRecEqp ) );

	BOOL32 bIsNeedPauseRec = FALSE;
	if( m_tRecPara.IsRecDStream() )
	{
		// 若需要录双流，还需判断双流源
		if( GetVidBrdSrc().IsNull() && GetAudBrdSrc().IsNull() && m_tDoubleStreamSrc.IsNull() )
		{
			bIsNeedPauseRec = TRUE;
		}
	}
	else
	{
		if( GetVidBrdSrc().IsNull() && GetAudBrdSrc().IsNull() )
		{
			bIsNeedPauseRec = TRUE;
		}
	}

	if( bIsNeedPauseRec )
	{
		// [12/15/2010 liuxu] 如果会议已经暂停,则不需要再暂停
		if (! m_tConf.m_tStatus.IsRecPause())
		{
			// 仍无广播源，设置会议为内部暂停录像
			SendMsgToEqp( m_tRecEqp.GetEqpId(), MCU_REC_PAUSEREC_REQ, cServMsg );
			m_tConf.m_tStatus.SetRecPauseByInternal();
		}
	}
	else
	{
		if( m_tConf.m_tStatus.IsRecPauseByInternal() )
		{
			SendMsgToEqp( m_tRecEqp.GetEqpId(), MCU_REC_RESUMEREC_REQ, cServMsg );
		}
	}

	return;
}

/*=============================================================================
函 数 名： ProcTimerDPalyTokenNtf
功    能： 双流放像，定时发239令牌环通知,只针对科达mcu
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20100226    4.0			pengjie                create
=============================================================================*/
void CMcuVcInst::ProcTimer239TokenNtf( void )
{
	KillTimer( MCUVC_NOTIFYMCUH239TOKEN_TIMER );

	u8  byManuId;
	TMt tMt;
    for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( !m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
		{
			continue;
		}

        byManuId = m_ptMtTable->GetManuId( byLoop );
		// 只给mcu发令牌环通知
		if( byManuId == MT_MANU_KDCMCU )
		{
    		tMt = m_ptMtTable->GetMt( byLoop );
			NotifyH239TokenOwnerInfo( &tMt );
		}
	}

	//重新设置定时器
	SetTimer( MCUVC_NOTIFYMCUH239TOKEN_TIMER, 10000);

	return;
}

/*=============================================================================
函 数 名： SetAutoRec
功    能： 设置制动录像
算法实现： 将以前制动录像设置逻辑相关代码整合为一个函数，并修改不能隔天定时录像问题
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20091225    4.0			pengjie                create
=============================================================================*/
BOOL32 CMcuVcInst::SetAutoRec( u16 &wErrCode )
{
    TConfAutoRecAttrb tAutoRecAttrb = m_tConf.GetAutoRecAttrb();
    if ( tAutoRecAttrb.IsAutoRec() )
    {
        BOOL32 bInvalid = FALSE;    // 是否不进行自动录像

        time_t tiCurTime = ::time(NULL);
        TKdvTime tCurTime;
        tCurTime.SetTime( &tiCurTime );

        // 首先检查今天是否需要录像
        tm *ptmWeekDay = ::localtime( &tiCurTime );
        if ( !tAutoRecAttrb.IsRecWeekDay(ptmWeekDay->tm_wday) )
        {                    
            // 本周今天不需要录像
            bInvalid = TRUE;
        }

        TKdvTime tRecStartTime = tAutoRecAttrb.GetStartTime();
        TKdvTime tRecEndTime = tAutoRecAttrb.GetEndTime();
        
        BOOL32 bStartInstant = (tRecStartTime.GetYear() == 0);  // 用户是否配置成立即开始
        BOOL32 bEndManual = (tRecEndTime.GetYear() == 0);   // 用户是否配置成手动结束

        if ( bStartInstant )
        {
            // 如果没有设置起始时间，认为是立即开始
            tRecStartTime.SetTime( &tiCurTime );
        }

        // 模板只设置每天的什么时候，不含年月日，故调整成今天
        time_t tiStartTime = 0;                    
        time_t tiEndTime = 0;
                   
        tRecStartTime.SetYear( tCurTime.GetYear() );
        tRecStartTime.SetMonth( tCurTime.GetMonth() );
        tRecStartTime.SetDay( tCurTime.GetDay() );     
        tRecStartTime.GetTime(tiStartTime);
        
        if (!bEndManual)
        {
            // 如果配置了结束时间，则调整成今天的时刻                    
            tRecEndTime.SetYear( tCurTime.GetYear() );
            tRecEndTime.SetMonth( tCurTime.GetMonth() );
            tRecEndTime.SetDay( tCurTime.GetDay() );                    
            tRecEndTime.GetTime(tiEndTime);
        }
        
        if ( (!bEndManual) && (tiStartTime > tiEndTime ) )
        {
            // 结束时间比开始早，不进行自动录像
            bInvalid = TRUE;
        }

        if ( (!bEndManual) && (tiEndTime < tiCurTime) )
        {
            // 如果结束时间已过，则不再需要开始
            bInvalid = TRUE;
        }

        if (!bInvalid)
        {
            if (tiStartTime <= tiCurTime)
            {                            
                // 发现开始时间已过，则需要立即开始
                tRecStartTime.SetTime( &tiCurTime );
            }
        }

        // 记录下本次会议录像真正开始/结束的绝对时间，后面再用
        tAutoRecAttrb.SetStartTime( tRecStartTime );
        if ( !bEndManual )
        {
            tAutoRecAttrb.SetEndTime( tRecEndTime );
        }                    
        m_tConf.SetAutoRecAttrb( tAutoRecAttrb );
        
        // 设置定时器开始
        if (!bInvalid)
        {
            // 检查一下当前录像机是否在线，如果不在线，提示，拒绝会议
            if ( !g_cMcuVcApp.IsRecorderOnline(tAutoRecAttrb.GetRecEqp()) )
            {
				RlsAllBasForConf();
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf %s Request Recorder %d is not online now.\n", 
                                m_tConf.GetConfName(), 
                                tAutoRecAttrb.GetRecEqp());
                wErrCode = ERR_MCU_CONFSTARTREC_MAYFAIL;
				return FALSE;
            }

        #ifdef _MINIMCU_
            if ( ISTRUE(m_byIsDoubleMediaConf) && tAutoRecAttrb.GetRecParam().IsRecLowStream() )
            {
                wErrCode = ERR_MCU_RECLOWNOSUPPORT;
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "Conf.%s can't support auto record low stream\n", m_tConf.GetConfName() );
                return FALSE;
            }
        #endif                    
            // guzh [4/17/2007] 采用绝对定时器
            if (tiStartTime <= tiCurTime)
            {
                SetTimer( MCUVC_CONFSTARTREC_TIMER, 10);
            }
            else
            {
                TKdvTime tKdvTime;
                tKdvTime.SetTime(&tiStartTime);   
                SetAbsTimer(MCUVC_CONFSTARTREC_TIMER, 
                            tKdvTime.GetYear(),
                            tKdvTime.GetMonth(),
                            tKdvTime.GetDay(),
                            tKdvTime.GetHour(),
                            tKdvTime.GetMinute(),
                            tKdvTime.GetSecond() );
            }    

			ConfPrint( LOG_LVL_DETAIL, MID_MCU_REC, "[SetAutoRec] Now Set Timer: MCUVC_CONFSTARTREC_TIMER!\n" );
        }
		//  [12/26/2009 pengjie] 解决定时录像不能隔天问题，如果今天不录像，那么设置定时器，明天凌晨0点，检查下一天是否需要进行会议定时录像
		else
		{
			TKdvTime tKdvNextDayNewTime;
			tm *ptmDay = ::localtime( &tiCurTime );
			ptmDay->tm_hour = 23;
			ptmDay->tm_min  = 59;
			ptmDay->tm_sec  = 59;
			time_t tiDayLastTime = ::mktime( ptmDay );
			tiDayLastTime = tiDayLastTime + 1;
			tKdvNextDayNewTime.SetTime( &tiDayLastTime );
			SetAbsTimer(MCUVC_UPDATA_AUTOREC_TIMER, 
				tKdvNextDayNewTime.GetYear(),
				tKdvNextDayNewTime.GetMonth(),
				tKdvNextDayNewTime.GetDay(),
				tKdvNextDayNewTime.GetHour(),
				tKdvNextDayNewTime.GetMinute(),
				tKdvNextDayNewTime.GetSecond() );
			ConfPrint( LOG_LVL_DETAIL, MID_MCU_REC, "[SetAutoRec] Now Set Timer:MCUVC_UPDATA_AUTOREC_TIMER!\n" );
		}	
    }

	return TRUE;
}

/*=============================================================================
函 数 名： ProcMcsMcuSaveConfToTemplateReq
功    能： 保存当前会议为模板
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/4/20   4.0			周广程                  创建
=============================================================================*/
void CMcuVcInst::DaemonProcMcsMcuSaveConfToTemplateReq( const CMessage * pcMsg )
{
	//u8 byLoop = 0;
	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byConfIdx = *(u8*)cServMsg.GetMsgBody();
	TTemplateInfo  tTemInfo;
	
	TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
	if (tMapData.IsValidConf())
	{
		//get tConfInfo
		if (NULL == g_cMcuVcApp.GetConfInstHandle(byConfIdx))
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[ProcMcsMcuSaveConfToTemplateReq]: GetConfInstHandle NULL! -- %d\n", m_byConfIdx);
			return;
		}
		CMcuVcInst* pcInstance = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
		if (!pcInstance)
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[ProcMcsMcuSaveConfToTemplateReq]: GetConfInstHandle NULL! -- %d\n", m_byConfIdx);
			return;
		}
		
		tTemInfo.m_tConfInfo = pcInstance->m_tConf;
		if (CONF_LOCKMODE_LOCK == tTemInfo.m_tConfInfo.m_tStatus.GetProtectMode())
		{
			tTemInfo.m_tConfInfo.m_tStatus.SetProtectMode(CONF_LOCKMODE_NONE);
		}
		
		//		// 将当前进行的画面合成保存为模板, zgc, 2007/04/25
		//		if( pcInstance->m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE ) 
		//		{
		//			tTemInfo.m_atVmpModule.EmptyMember();
		//			TVMPParam tTempParam = pcInstance->m_tConf.m_tStatus.GetVmpParam();
		//			tTemInfo.m_atVmpModule.SetVmpParam( tTempParam );
		//			u8 byMaxVmpMemberNum = tTempParam.GetMaxMemberNum();
		//			TVMPMember *pVmpMem = NULL;
		//			for( byLoop = 0; byLoop < byMaxVmpMemberNum; byLoop ++ )
		//			{
		//				pVmpMem = tTempParam.GetVmpMember( byLoop );
		//				tTemInfo.m_atVmpModule.SetVmpMember( byLoop, pVmpMem->GetMtId(), pVmpMem->GetMemberType() );
		//			}
		//		}
		//		else
		//		{
		//			
		//		}
		//
		//		// 将当前使用的多画面电视墙保存为模板, zgc, 2007/04/25
		//		if( pcInstance->m_tConf.m_tStatus.GetVmpTwMode != CONF_VMPTWMODE_NONE )
		//		{
		//
		//		}
		//		else
		//		{
		//
		//		}
		
		//保存即时会议时，清空当前会议状态中的外设状态
        // guzh [11/7/2007] 
        tTemInfo.m_tConfInfo.m_tStatus.SetNoMixing();
        tTemInfo.m_tConfInfo.m_tStatus.SetVACing(FALSE);
        tTemInfo.m_tConfInfo.m_tStatus.SetNoPlaying();
        tTemInfo.m_tConfInfo.m_tStatus.SetNoRecording();
        tTemInfo.m_tConfInfo.m_tStatus.SetVMPMode(CONF_VMPMODE_NONE);
        tTemInfo.m_tConfInfo.m_tStatus.SetVmpTwMode(CONF_VMPTWMODE_NONE);
        tTemInfo.m_tConfInfo.m_tStatus.SetPollMode(CONF_POLLMODE_NONE);
		//[03/04/2010] zjl modify (电视墙多通道轮询代码合并)
		//      tTemInfo.m_tConfInfo.m_tStatus.SetTvWallPollState(POLL_STATE_NONE);
		tTemInfo.m_tConfInfo.m_tStatus.SetTakeMode(CONF_TAKEMODE_TEMPLATE);
		
		//		tTemInfo.m_tMultiTvWallModule = pcInstance->m_tConfEqpModule.m_tMultiTvWallModule; 
		//		tTemInfo.m_atVmpModule = pcInstance->m_tConfEqpModule.GetVmpModule();
		tTemInfo.m_tMultiTvWallModule.Clear();
		tTemInfo.m_atVmpModule.EmptyMember();
		TConfAttrb tConfAttrib = tTemInfo.m_tConfInfo.GetConfAttrb();
		tConfAttrib.SetHasTvWallModule( FALSE );
		tConfAttrib.SetHasVmpModule( FALSE );
		tTemInfo.m_tConfInfo.SetConfAttrb( tConfAttrib );
		
		if(pcInstance)
			tTemInfo.m_byMtNum = pcInstance->m_ptMtTable->m_byMaxNumInUse;
		
        tTemInfo.EmptyAllTvMember();
        tTemInfo.EmptyAllVmpMember();
		
		//get alias array
        u8 byMemberType = 0;
		TMt tMt;
		TMtAlias tMtAlias;
		for (u8 byLoop = 0; pcInstance && byLoop < pcInstance->m_ptMtTable->m_byMaxNumInUse; byLoop++)
		{
			tMt = pcInstance->m_ptMtTable->GetMt(byLoop+1);
			if (!tMt.IsNull())
			{
				if (MT_TYPE_MMCU == tMt.GetMtType())
				{
					continue;
				}
				
				//由于mtAliasTypeH320Alias类型不能用作呼叫信息，所以不进行保存
				if (pcInstance->m_ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeH320ID, &tMtAlias))
				{
					tTemInfo.m_atMtAlias[byLoop] = tMtAlias;			
				}
				else if (pcInstance->m_ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeTransportAddress, &tMtAlias))
				{
					tTemInfo.m_atMtAlias[byLoop] = tMtAlias;			
				}
				else 
				{
					if (pcInstance->m_ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeE164, &tMtAlias))
					{
						tTemInfo.m_atMtAlias[byLoop] = tMtAlias;			
					} 
					else 
					{
						if (pcInstance->m_ptMtTable->GetMtAlias((byLoop+1), mtAliasTypeH323ID, &tMtAlias))
						{
							tTemInfo.m_atMtAlias[byLoop] = tMtAlias;
						}
					}
				}
				tTemInfo.m_awMtDialBitRate[byLoop] = pcInstance->m_ptMtTable->GetDialBitrate(byLoop+1);
				
				//更新模板映射关系
				TConfAttrb tConfAttrb = tTemInfo.m_tConfInfo.GetConfAttrb();
				if (tConfAttrb.IsHasTvWallModule())
				{
					for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL; byTvLp++)
                    {                            
                        u8 byTvId = pcInstance->m_tConfEqpModule.m_tTvWallInfo[byTvLp].m_tTvWallEqp.GetEqpId();
                        for(u8 byTvChlLp = 0; byTvChlLp < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byTvChlLp++)
                        {
                            if(TRUE ==pcInstance-> m_tConfEqpModule.IsMtInTvWallChannel(byTvId, byTvChlLp, tMt, byMemberType) )
                            {
                                tTemInfo.SetMtInTvChannel(byTvId, byTvChlLp, byLoop+1, byMemberType);
                            }
                        }
                    }
				}
				if (tConfAttrb.IsHasVmpModule())
				{
                    for (u8 byVmpIdx = 0; byVmpIdx < MAXNUM_MPUSVMP_MEMBER; byVmpIdx++)
                    {
                        if( pcInstance->m_tConfEqpModule.IsMtAtVmpChannel(byVmpIdx, tMt, byMemberType) )
						{
							tTemInfo.m_atVmpModule.SetVmpMember(byVmpIdx, tMt.GetMtId(), byMemberType);
						}
                    }						
				}            
			}// if (!tMt.IsNull())
		}// for(u8 byLoop = 0; byLoop < m_ptMtTable->m_byMax...
	}// if(g_cMcuVcApp.GetConfMapData...	
	
	TConfMapData tConfMapData = g_cMcuVcApp.GetConfMapData( byConfIdx );
	
	tTemInfo.m_byConfIdx = byConfIdx;
	
	if ( tConfMapData.IsTemUsed() )
	{
		TTemplateInfo tOldTempInfo;
		g_cMcuVcApp.GetTemplate( byConfIdx, tOldTempInfo );
		tTemInfo.m_tConfInfo.SetConfId( tOldTempInfo.m_tConfInfo.GetConfId() );
		
		if ( !g_cMcuVcApp.ModifyTemplate( tTemInfo, TRUE ) )
		{
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSaveConfToTemplateReq]Modify template %s failed!\n", tTemInfo.m_tConfInfo.GetConfName() );
            return;
		}
		
		SendReplyBack(cServMsg, pcMsg->event+1);
        g_cMcuVcApp.TemInfo2Msg(tTemInfo, cServMsg);
        SendMsgToAllMcs(MCU_MCS_TEMSCHCONFINFO_NOTIF, cServMsg); 
	}
	else
	{
		//会议的E164号码已存在，拒绝 
		if( g_cMcuVcApp.IsConfE164Repeat( tTemInfo.m_tConfInfo.GetConfE164(), TRUE ,FALSE) )
        {
            cServMsg.SetErrorCode( ERR_MCU_CONFE164_REPEAT );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSaveConfToTemplateReq]template %s E164 repeated and create failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
            return;
        }
		//会议名已存在，拒绝
        if( g_cMcuVcApp.IsConfNameRepeat( tTemInfo.m_tConfInfo.GetConfName(), TRUE ,FALSE) )
        {
            cServMsg.SetErrorCode( ERR_MCU_CONFNAME_REPEAT );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSaveConfToTemplateReq]template %s name repeated and create failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
            return;
        } 
		
		if(!g_cMcuVcApp.AddTemplate(tTemInfo))
        {
			// 返回的错误码
			cServMsg.SetErrorCode( ERR_MCU_TEMPLATE_NOFREEROOM );
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuSaveConfToTemplateReq] add template %s failed\n", tTemInfo.m_tConfInfo.GetConfName());
			SendReplyBack(cServMsg, pcMsg->event+2);
			return;
		}                 
		cServMsg.SetConfId( tTemInfo.m_tConfInfo.GetConfId() );
		SendReplyBack(cServMsg, pcMsg->event+1);
		g_cMcuVcApp.TemInfo2Msg(tTemInfo, cServMsg);
		SendMsgToAllMcs(MCU_MCS_TEMSCHCONFINFO_NOTIF, cServMsg); 
	}
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "Save conf %s to template successfully!\n", tTemInfo.m_tConfInfo.GetConfName() );
	return;
}

/*==============================================================================
函数名    :  ProcMcsMcuVmpPriSeizeRsp
功能      :  处理会控vmp抢占应答消息
算法实现  :  
参数说明  :  
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2008-12-12					薛亮							创建
==============================================================================*/
void CMcuVcInst::ProcMcsMcuVmpPriSeizeRsp(const CMessage *pcMsg)
{
	STATECHECK;
	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	BOOL32 bNeedSeize = TRUE;

	TMt tSeizeMt = *(TMt*)cServMsg.GetMsgBody();	//最初要抢占的MT
	u8 byVmpId = cServMsg.GetEqpId();
	if (!IsValidVmpId(byVmpId) || !IsVmpIdInVmpList(byVmpId))
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcsMcuVmpPriSeizeRsp]Vmp(id:%d) is not in conf(%d).\n", byVmpId, m_byConfIdx );
	}
	
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	TVMPParam_25Mem  tVMPParam   = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);

	if (MCS_MCU_VMPPRISEIZE_ACK == pcMsg->event)
	{
		u8 bySeizedMtNum = (cServMsg.GetMsgBodyLen()-sizeof(TMt)) / sizeof(TSeizeChoice);//被抢占的MT数目
		TSeizeChoice tSeizeChoice[MAXNUM_MPU2VMP_E20_HDCHNNL];
		u8 byMtIdx=0;

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "Mt.(%u,%u) Seize Mt choice info:\n==================\n",tSeizeMt.GetMcuId(), tSeizeMt.GetMtId() );
		for(byMtIdx=0; byMtIdx<bySeizedMtNum; byMtIdx++)
		{
			tSeizeChoice[byMtIdx] = *(TSeizeChoice *)(cServMsg.GetMsgBody()+sizeof(TMt)+sizeof(TSeizeChoice)*byMtIdx);
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "Mt.(%u,%u): byStopVmp(%u), byStopSpeaker(%u), byStopSelected(%u)\n",
				tSeizeChoice[byMtIdx].tSeizedMt.GetMcuId(), tSeizeChoice[byMtIdx].tSeizedMt.GetMtId(), 
				tSeizeChoice[byMtIdx].byStopVmp,
				tSeizeChoice[byMtIdx].byStopSpeaker, 
				tSeizeChoice[byMtIdx].byStopSelected);
		}

		for(byMtIdx=0; byMtIdx<bySeizedMtNum; byMtIdx++)
		{
			// 根据被抢占选项进行操作
			ExecuteMtBeSeizedChoice(tVmpEqp, tVMPParam, tSeizeChoice[byMtIdx]);
		}

		if(bNeedSeize)
		{
			// vmpparam可能有更新，重新获取vmpparam，如发言人跟随时，取消发言人会更新发言人跟随通道
			tVMPParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			// 对抢占终端处理
			ExecuteMtSeizeChoice(tVmpEqp, tVMPParam, tSeizeMt);
		}
		else
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[ProcMcsMcuVmpPriSeizeRsp] no need to seize!\n");
		}
	}
	else 
	{	
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuVmpPriSeizeRsp] receive MCS_MCU_VMPPRISEIZE_NACK!\n");
		CServMsg cMsg;
		cMsg.SetEqpId(byVmpId);
		cMsg.SetMsgBody( (u8*)&tVMPParam, sizeof(tVMPParam) );
		SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cMsg ); //Nack时,告诉会控更新VMPPARAM
	}
	// 无论是否抢占,清空m_atVMPTmpMember记录信息
	TVmpPriSeizeInfo tPriSeiInfo;
	g_cMcuVcApp.SetVmpPriSeizeInfo(tVmpEqp, tPriSeiInfo);
}

/*==============================================================================
函数名    :  ExecuteMtBeSeizedChoice
功能      :  根据被抢占选项进行操作
算法实现  :  
参数说明  :  const TEqp &tVmpEqp
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130531					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::ExecuteMtBeSeizedChoice(const TEqp &tVmpEqp, TVMPParam_25Mem &tVMPParam, TSeizeChoice &tSeizeChoice)
{
	//获得tMt在vmp中所有位置,包含mcu与其上传通道终端间的相互映射
	u8 abyChnlNo[MAXNUM_VMP_MEMBER];
	u8 byChnlNum = 0;
	memset(abyChnlNo,0,sizeof(abyChnlNo));
	//根据被抢占选项进行操作
	//1,该终端退出画面合成
	if(tSeizeChoice.byStopVmp)
	{
		if (MODE_VIDEO == tSeizeChoice.byStopVmp)
		{
			// xliang [3/25/2009] 策略更改，局部成员调整，不进行全局重整
			TMt tMtBeSeized = tSeizeChoice.tSeizedMt;
			//获得tMtBeSeized在vmp中所有位置,包含mcu映射上传通道终端
			GetChlOfMtInVmpParam(tVMPParam, tMtBeSeized, byChnlNum, abyChnlNo);
		} //关闭双流跟随通道
		else if (MODE_SECVIDEO == tSeizeChoice.byStopVmp)
		{
			byChnlNum = 1;
			abyChnlNo[0] = tVMPParam.GetChlOfMemberType(VMP_MEMBERTYPE_DSTREAM);
		}
	
		if (0 != byChnlNum)
		{
			u8	byChl = abyChnlNo[0];	//前适配仅支持单个通道
			// 清此通道成员及其占用的多回传资源
			ClearOneVmpMember(tVmpEqp.GetEqpId(), byChl, tVMPParam);
		}
	}
	//2,该终端取消发言人
	if(tSeizeChoice.byStopSpeaker == 1)
	{
		BOOL32 bCancelSpeaker = TRUE; //能否取消发言人

		//处于演讲模式的语音激励控制发言状态,不能指定发言人
		if (m_tConf.m_tStatus.IsVACing())
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[ProcMcsMcuVmpPriSeizeRsp] Conf is VACing! Speaker cannot be canceled!\n");
			bCancelSpeaker = FALSE;
		}

		TMt tSpeaker = m_tConf.GetSpeaker();
		if( !m_tConf.HasSpeaker() )//没有发言人
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "No speaker in conference %s now! Cannot cancel the speaker!\n", 
				m_tConf.GetConfName() );
			bCancelSpeaker = FALSE;
		}
		if(bCancelSpeaker)
		{
			//停组播
			if( m_ptMtTable->IsMtMulticasting( tSpeaker.GetMtId() ) )
			{
				if ( m_tConf.GetConfAttrb().IsMulticastMode() && tSpeaker == GetVidBrdSrc())
				{
					g_cMpManager.StopMulticast( tSpeaker, 0, MODE_VIDEO );
				}
				if ( m_tConf.GetConfAttrb().IsMulticastMode() && tSpeaker == GetAudBrdSrc())
				{
					g_cMpManager.StopMulticast( tSpeaker, 0, MODE_AUDIO );
				}
				
				m_ptMtTable->SetMtMulticasting( tSpeaker.GetMtId(), FALSE );
			}
			
			//取消发言人
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMcsMcuVmpPriSeizeRsp]begin change speaker!\n");
			ChangeSpeaker( NULL );
		}

	}

	//3,该终端取消被选看
	if(tSeizeChoice.byStopSelected == 1)
	{
		TMtStatus	tMtStatus;
		TMt			tSrc;
		TMt			tDstMt;
		BOOL32		bSelectedSrc = FALSE; //是否正在被选看
		TMt tMtBeSeized = tSeizeChoice.tSeizedMt;
		TMt tSrcViewMt;
		//取消所有正在选看该选看源MT的交换
		for(u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			if(!m_tConfAllMtInfo.MtJoinedConf( byLoop ))
			{
				continue;
			}
			m_ptMtTable->GetMtStatus(byLoop,&tMtStatus);
			tSrc = tMtStatus.GetSelectMt( MODE_VIDEO ); //获取选看源
			// 可能终端选看的是mcu,而被抢占终端正好是该mcu的上传通道终端,同样需要取消该mcu的选看
			// 场景:二级mcu的上传通道终端在vmp中,该二级mcu被另一终端选看
			if (IsMcu(tSrc))
			{
				tSrcViewMt = GetSMcuViewMt(tSrc, TRUE);
			}
			if(tSrc == tMtBeSeized || tSrcViewMt == tMtBeSeized)//该MT的选看源是被抢占的MT,避免选看源是mcu
			{
				//取消选看
				bSelectedSrc = TRUE;
				tDstMt = m_ptMtTable->GetMt(byLoop);

				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMcsMcuVmpPriSeizeRsp]Cancel Mt.%u selectseeing Mt.%u.\n",
					byLoop,tSeizeChoice.tSeizedMt.GetMtId());

				StopSelectSrc(tDstMt, MODE_VIDEO);
				
				// 设置发言人源为无源
				if ( HasJoinedSpeaker() && tDstMt == GetLocalSpeaker() )
				{
					SetSpeakerSrcSpecType( MODE_BOTH, SPEAKER_SRC_NOTSEL );
				}
			}
		}

		if(!bSelectedSrc)
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMcsMcuVmpPriSeizeRsp]Mt.%u is not the selected source!\n",tSeizeChoice.tSeizedMt.GetMtId());
		}
	}

	return;
}

/*==============================================================================
函数名    :  ExecuteMtBeSeizedChoice
功能      :  根据被抢占选项进行操作
算法实现  :  
参数说明  :  const TEqp &tVmpEqp
			 TVMPParam_25Mem &tVMPParam
			 const TMt &tSeizeMt
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130531					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::ExecuteMtSeizeChoice(const TEqp &tVmpEqp, TVMPParam_25Mem &tVMPParam, const TMt &tSeizeMt)
{
	//对原先要抢占的Mt进行操作，设个定时，防时序问题
	u16 wTimer = 10;		
	//获得tMt在vmp中所有位置,包含mcu与其上传通道终端间的相互映射
	u8 abyChnlNo[MAXNUM_VMP_MEMBER];
	u8 byChnlNum = 0;
	memset(abyChnlNo,0,sizeof(abyChnlNo));
	TVmpPriSeizeInfo tPriSeiInfo = g_cMcuVcApp.GetVmpPriSeizeInfo(tVmpEqp);
	// 判断是否是双流抢占
	BOOL32 bDStreamSeize = FALSE;
	
	//获得tSeizeMt在vmp中所有位置,包含mcu映射上传通道终端
	GetChlOfMtInVmpParam(tVMPParam, tSeizeMt, byChnlNum, abyChnlNo);

	if (tSeizeMt == (TMt)tPriSeiInfo.m_tPriSeizeMember)
	{
		if (VMP_MEMBERTYPE_DSTREAM == tPriSeiInfo.m_tPriSeizeMember.GetMemberType())
		{
				bDStreamSeize = TRUE;
		}
	}
	u8 byIndex = tPriSeiInfo.m_byChlIdx;

	//针对新加一个成员，引起抢占的情况: 重新调整一次合成成员
	if( (!tVMPParam.IsMtInMember(tSeizeMt) && 0 == byChnlNum) || bDStreamSeize)
	{	
		if ( byIndex < MAXNUM_VMP_MEMBER)
		{
			TVMPMember tVMPMember = tPriSeiInfo.m_tPriSeizeMember;
			// 双流跟随,需更新真实双流源,可能抢占前是smcu,此时已可以获得真实双流源
			if (VMP_MEMBERTYPE_DSTREAM == tVMPMember.GetMemberType())
			{
				// 防止此时双流已停止
				if (m_tDoubleStreamSrc.IsNull())
				{
					tVMPMember.SetNull();
				}
				else
				{
					TMt tRealDSMt = GetConfRealDsMt();
					if (!tRealDSMt.IsNull())
					{
						tVMPMember.SetMemberTMt(tRealDSMt);
					}
				}
			}
			tVMPParam.SetVmpMember(byIndex, tVMPMember);
			
			AdjustVmpParam(tVmpEqp.GetEqpId(), &tVMPParam);
		}
	}
	else
	{
		//从abyChnlNo中取位置,包含下级mcu与其上传通道终端间相互映射
		u8 byMemberIdx = abyChnlNo[0];//tVMPParam.GetChlOfMtInMember(tSeizeMt);
		if ( (TMt)tPriSeiInfo.m_tPriSeizeMember == tSeizeMt )
		{
			if(tPriSeiInfo.m_wEventId == MCS_MCU_STARTSWITCHMT_REQ)
			{
				CServMsg cSendMsg;
				cSendMsg.SetSrcSsnId(tPriSeiInfo.m_bySrcSsnId);//用于区分是否mcs拖拽选看
				cSendMsg.SetSrcMtId(0);
				cSendMsg.SetEventId(MCS_MCU_STARTSWITCHMT_REQ);
				cSendMsg.SetMsgBody((u8*)&tPriSeiInfo.m_tSwitchInfo, sizeof(TSwitchInfo));
				ProcStartSelSwitchMt(cSendMsg);
			}
			else
			{
				// [1/19/2010 xliang] SetTimer 参数调整。参数要含3个信息：mcuId， mtId， byVmpid
				u32 nTimerParam = tSeizeMt.GetMcuId() * 100000 + tSeizeMt.GetMtId() * 100 + tVmpEqp.GetEqpId() ;
				SetTimer(MCUVC_MTSEIZEVMP_TIMER, wTimer, nTimerParam );
			}
		}
	}

	return;
}

/*==============================================================================
函数名    : ProcMtSeizeVmp 
功能      : 抢占定时处理
算法实现  :  
参数说明  :  
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2008-12-12					薛亮							创建
==============================================================================*/
void CMcuVcInst::ProcMtSeizeVmpTimer(const CMessage *pcMsg)
{
// 	u8 bySeizeMtId  = (u8)((*((u32*)pcMsg->content))/100);		//之前要抢占的终端id
// 	u8 bySeizeTimes  = (u8)((*((u32*)pcMsg->content))%100)+1;	//抢占次数

	u16 wSeizeMcuId = (u16)((*((u32*)pcMsg->content)) / 100000);	//之前要抢占的终端McuId
	u32 nTmpRes  = ((*((u32*)pcMsg->content)) % 100000);	
	u8 bySeizeMtId  = (u8)(nTmpRes / 100) ;						//之前要抢占的终端MtId
	u8 byVmpId  = (u8)(nTmpRes % 100);					//byVmpId
	if (!IsValidVmpId(byVmpId))
	{
		return;
	}
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );

	TMt tSeizeMt;

	if( LOCAL_MCUIDX == wSeizeMcuId )
	{
		tSeizeMt = m_ptMtTable->GetMt(bySeizeMtId);
	}
	else
	{
		tSeizeMt = m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx(wSeizeMcuId)); //借用此接口填一下driId, confId, mcuId, mtId 在下面填
		tSeizeMt.SetMcuIdx( wSeizeMcuId );
		tSeizeMt.SetMtId(bySeizeMtId);
	}
	
	ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMtSeizeVmpTimer] Seize Mt Info: Mt.(%u,%u)-confIdx.%u \n", tSeizeMt.GetMcuId(), tSeizeMt.GetMtId(), tSeizeMt.GetConfIdx());

// 	BOOL32 bNeedSeizePromt = FALSE;	//是否需要重新发抢占提示 //该字段目前已无用
// 	if(bySeizeTimes > 3)
// 	{
// 		bNeedSeizePromt = TRUE;
// 	}
	//获取之前要抢占的终端vmpMemberType
	TVMPMember tVMPMember;
	tVMPMember.SetNull();
	TVMPParam_25Mem  tVMPParam   = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	u8         byMemberNum = tVMPParam.GetMaxMemberNum();
	u8	       byMemberType = ~0;
	for (u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
	{
		
		//取画面合成成员
		tVMPMember = *(tVMPParam.GetVmpMember(byLoop));
		if( tVMPMember.GetMcuIdx() == wSeizeMcuId 
			&& tVMPMember.GetMtId() == bySeizeMtId
			)
		{
			byMemberType = tVMPMember.GetMemberType();
			break;
		}
	}

	if(byMemberType == 0) //如果在超时到来之前，整个画面合成被停止了，则membertype应为0
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[ProcMtSeizeVmpTimer] vmp has been stopped,no need to change Mt.%u's format!\n",bySeizeMtId);
		return;
	}

	//取VMPstyle, channel idx
	u8 byVmpStyle = tVMPParam.GetVMPStyle();
	u8 byChlPos	= tVMPParam.GetChlOfMtInMember(tSeizeMt);

	TVMPParam_25Mem tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
	
	if(tLastVmpParam.IsMtInMember(tSeizeMt))//之前已经在VMP中，且建了交换.(针对某成员升为发言人等特殊身份导致抢占的情况)
	{
		//do nothing
		ChangeMtVideoFormat(tSeizeMt, FALSE);
	}
	else 
	{
		//调分辨率建交换
		ChangeMtResFpsInVmp(byVmpId, tSeizeMt, &tVMPParam, TRUE, byMemberType, byChlPos, FALSE);
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtSeizeVmpTimer] Mt.(%u,%u) Set Vmp channel.%u!\n", wSeizeMcuId, bySeizeMtId, byChlPos);
		/*if(bRet)
		{
			//设置通道，建交换
			SetVmpChnnl(byVmpId, (TMt)tVMPMember, byChlPos, byMemberType);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtSeizeVmpTimer] Mt.(%u,%u) Set Vmp channel.%u!\n", wSeizeMcuId, bySeizeMtId, byChlPos);
		}*/
// 		else if(!bNeedSeizePromt) //如果需要再次发抢占提示，这里就不需要定时了
// 		{	
// 			//继续定时
// 			u32 nTimerParam = wSeizeMcuId * 100000 + bySeizeMtId * 100 + bySeizeTimes ;
// 			SetTimer(MCUVC_MTSEIZEVMP_TIMER, 1000, nTimerParam);	       
// 		}
	}
	
}

/*==============================================================================
函数名    :  CheckMpuAudPollConflct
功能      :  会议带音频轮询
算法实现  :  会议带音频轮询时，画面合成器不支持轮询跟随和发言人跟随同时存在
参数说明  :  
返回值说明:  u16 返回0表示正常，否则返回errorcode
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-8-20					薛亮
==============================================================================*/
u16 CMcuVcInst::CheckMpuAudPollConflct(u8 byVmpId, TVMPParam_25Mem& tVmpParam, BOOL32 bChgParam /*= FALSE*/)
{
	u16 wRet = 0;
	TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
	if (!IsAllowVmpMemRepeated(byVmpId) &&
		CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode() 
		&& tPollInfo.GetMediaMode() == MODE_BOTH 
		&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER)
		&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
		)
	{
		if( !bChgParam )
		{
			// 			wErrorCode = ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
			// 			return FALSE;
			wRet = (u16)ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
		}
		else
		{
			TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
			TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			u8 byConflictMemType = 0;
			if (tConfVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL))
			{
				//此次新增了发言人跟随,把其作为冲突源
				byConflictMemType = VMP_MEMBERTYPE_SPEAKER;
			}
			else if(tConfVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER))
			{
				byConflictMemType = VMP_MEMBERTYPE_POLL;
			}
			else
			{
				//do nothing, maintain the variable byConflictMemType be 0
			}
			
			if(byConflictMemType != 0)
			{
				
				//调整VMP param
				u8 byLoop;
				TVMPMember* ptVmpMember;
				for (byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop ++)
				{
					ptVmpMember = tVmpParam.GetVmpMember(byLoop);
					if (NULL == ptVmpMember)
					{
						continue;
					}
					if(ptVmpMember->GetMemberType() == byConflictMemType)
					{
						// xliang [4/3/2009] 会控带过来的TVMPParam参数中mode值是空的，这里保护一下
						tVmpParam.SetVMPMode(CONF_VMPMODE_CTRL);
						ClearOneVmpMember(tVmpEqp.GetEqpId(), byLoop, tVmpParam, TRUE);
						break;
					}
				}
				
				// 				wErrorCode = ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
				// 				return FALSE;
				wRet = (u16)ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
			}
		}
	}
	return wRet;
}

/*==============================================================================
函数名    :  CheckSameMtInMultChnnl
功能      :  会议中不允许同一个MT占了多个成员通道
算法实现  :  
参数说明  :  
返回值说明:  u16 返回0表示正常，否则返回errorcode
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-8-20					薛亮							
==============================================================================*/
u16 CMcuVcInst::CheckSameMtInMultChnnl(u8 byVmpId, TVMPParam_25Mem & tVmpParam, BOOL32 bChgParam/* = FALSE*/)
{
	// xliang [3/12/2009] LOCAL_MCUID定义成192是有问题的,当MT.192是MCU的话，就会误判。
	// 一个会议中呼满192个mt，也是很小概率的。 该宏暂不改动，不能改成0，改成193对其他代码有何冲击
	//zhouyiliang20101112用GetChlOfMtInMember来解决重复问题
	//u8 abyMcMtInVMPStatis[MAXNUM_CONF_MT+1][MAXNUM_CONF_MT+1] = {0};
	u16 wRet = 0;
	BOOL32	bRepeatedVmpMem = FALSE;
	BOOL32  bRepeatedOtherMem = FALSE;
	BOOL32  bRepeatedMMcuMem = FALSE;
	BOOL32  bRepeatedG400IPCMtMem = FALSE;
	BOOL32  bRepeatedSpeMcuMem = FALSE; //简单级联MCU是否在多通道
	u8 bySpeakerChnnl	= 0;
	u8 byPollChnnl		= 0;
	u8 byVmpPollChnnl	= 0;	//vmp单通道轮询个数
	u8 byDStreamChnnl	= 0;	//双流跟随通道个数
	u8 byMcuId = 0;
	TVMPMember tVMPMember;
	TVMPMember tLastVMPMember;
	TMt tRealMt;
	BOOL32 bIsChnlMemberChange = FALSE;//记录通道成员是否有变
	u8 byChlCount = 0;
	u8 abyChnlNo[MAXNUM_VMP_MEMBER];
	BOOL32 bStyleOK = FALSE;
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
	if (bChgParam) //当前风格是否前适配充足
	{
		bStyleOK = tVmpChnnlInfo.GetMaxStyleNum() >= tVmpParam.GetMaxMemberNum();
	}

	for( u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		if (NULL == tVmpParam.GetVmpMember( byLoop ))
		{
			tVMPMember.SetNull();
		} 
		else
		{
			tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		}
		if (NULL == tConfVmpParam.GetVmpMember( byLoop ))
		{
			tLastVMPMember.SetNull();
		} 
		else
		{
			tLastVMPMember = *tConfVmpParam.GetVmpMember( byLoop );
		}
		
		// xliang [3/24/2009] membertype的过滤，要无视tVMPMember.IsNull()，
		// 否则当发言人跟随但会议中无发言人的情况下，限制失效
		if(tVMPMember.GetMemberType() == VMP_MEMBERTYPE_SPEAKER)
		{
			bySpeakerChnnl ++;
			//在风格支持(前适配充足)情况下，跳过跟随通道，保证外厂商终端可以打开跟随通道
			if (bStyleOK)
			{
				continue;
			}
		}
		if(tVMPMember.GetMemberType() == VMP_MEMBERTYPE_POLL)
		{
			byPollChnnl ++;
			//在风格支持(前适配充足)情况下，跳过跟随通道，保证外厂商终端可以打开跟随通道
			if (bStyleOK)
			{
				continue;
			}
		}
		if(tVMPMember.GetMemberType() == VMP_MEMBERTYPE_VMPCHLPOLL)
		{
			byVmpPollChnnl ++;
		}
		if (tVMPMember.GetMemberType() == VMP_MEMBERTYPE_DSTREAM)
		{
			byDStreamChnnl ++;
			//跳过双流跟随通道
			continue;
		}
		
		if(tVMPMember.IsNull()) 
		{
			continue;
		}

		//zhouyiliang20101112用GetChlOfMtInMember来解决重复问题,从最后一个位置找
		//byMcuId =  tVMPMember.GetMcuIdx() ;
		//u8 byLastMemPos = tVmpParam.GetChlOfMtInMember( (TMt)tVMPMember,TRUE );
		tRealMt = tVMPMember;
		// 通道中是mcu,获得其上传通道终端
		if (IsMcu(tVMPMember))
		{
			tRealMt = GetSMcuViewMt(tVMPMember, TRUE);
			if (tRealMt.IsNull())//无上传通道终端
			{
				tRealMt = tVMPMember;
			}
		}
		// mcu需映射上传通道终端做统计
		if (IsMcu(tVMPMember) || !tVMPMember.IsLocal())
		{
			GetChlOfMtInVmpParam(tVmpParam, tVMPMember, byChlCount);
		}
		else
		{
			tVmpParam.GetChlOfMtInMember(tVMPMember,FALSE,MAXNUM_VMP_MEMBER,abyChnlNo,&byChlCount);
		}
		bIsChnlMemberChange = !(tVMPMember == tLastVMPMember && tVMPMember.GetMemberType() == tLastVMPMember.GetMemberType());
		//if( abyMcMtInVMPStatis[byMcuId][tVMPMember.GetMtId()] == 1)
		// 针对因下级改变上传通道终端导致的终端在多通道,不影响其它vmp操作,仅限制拖入mcu或其上传终端进vmp
		if (bIsChnlMemberChange
			&& byChlCount > 1 )
		{
			bRepeatedVmpMem = TRUE;
			// 先判终端是否在线，模版开启画面合成时，终端不在线，无法判终端是否为KedaMt
			if (!m_tConfAllMtInfo.MtJoinedConf(tVMPMember))
			{
				// 不在线终端不做接下来的check
				continue;
			}
			if (tVMPMember == m_tCascadeMMCU)
			{
				bRepeatedMMcuMem = TRUE;
				continue;
			}
			// 简单级联mcu不能进多通道
			if (IsNotSupportCascadeMcu(tVMPMember))
			{
				bRepeatedSpeMcuMem = TRUE;
				continue;
			}
			//遍历是否有非科达终端在多个通道中
			if (!IsKedaMt(tVMPMember, TRUE) || !IsKedaMt(tVMPMember, FALSE))//下级终端要传FALSE来判非科达
			{
				bRepeatedOtherMem = TRUE;
				continue;
			}
			//针对拖入一个(上传通道终端为非科达终端的)mcu,需判此非科达终端是否已在其它通道
			if (IsMcu(tVMPMember) 
				&& !IsKedaMt(tRealMt, FALSE) 
				)
			{
				bRepeatedOtherMem = TRUE;
				continue;
			}
			//IPC前端是否进了vmp多通道
			if (IsG400IPCMt(tVMPMember))
			{
				bRepeatedG400IPCMtMem = TRUE;
			}
		}
//		else
//		{
//			abyMcMtInVMPStatis[byMcuId][tVMPMember.GetMtId()] ++ ;
//		}
	}
	// 合成开启时,有Other终端/G400IPCMt在多个通道时会清理,照常开启(临时版本)
	// 合成改变时,有Other/G400IPCMt终端在多个通道时,直接返回nack,取消这次改变
	if (/*bChgParam && */
		(bRepeatedOtherMem || bRepeatedG400IPCMtMem))
	{
		wRet =  (u16)ERR_MCU_REPEATEDVMPNONEKEDAMEMBER; //vmp中非科达终端重复
	}
	
	// 合成开启时,有MMCU在多个通道时会清理,照常开启(临时版本)
	// 合成改变时,有MMCU在多个通道时,直接返回nack,取消这次改变
	if (/*bChgParam &&*/ bRepeatedMMcuMem)
	{
		wRet =  (u16)ERR_MCU_REPEATEDVMPMMCUMEMBER; //vmp中MMCU重复
	}
	if (/*bChgParam &&*/ bRepeatedSpeMcuMem)
	{
		wRet =  (u16)ERR_MCU_REPEATEDVMPNONEKEDAMEMBER; //vmp中简单级联MCU重复
	}

	if(bRepeatedVmpMem &&
		!IsAllowVmpMemRepeated(byVmpId))
	{
		wRet = (u16)ERR_MCU_REPEATEDVMPMEMBER;
	}

	if(bySpeakerChnnl > 1 /*&&		//有多个通道被设成发言人跟随
		!IsAllowVmpMemRepeated()*/)	//新需求：8000A和8000H都通过配置文件来决定
	{
		wRet =  (u16)ERR_MCU_REPEATEDVMPSPEAKER;
	}
	
	if(byPollChnnl > 1 /*&&			//有多个通道被设成轮询跟随
		!IsAllowVmpMemRepeated()*/)	//新需求：8000A和8000H都通过配置文件来决定
	{
		wRet =  (u16)ERR_MCU_REPEATEDVMPPOLL;
	}
	
	if(byVmpPollChnnl > 1)			//有多个通道被设成Vmp通道轮询跟随
	{
		wRet = (u16)ERR_MCU_REPEATEDVMPPOLL;
	}
	
	if(byDStreamChnnl > 1)			//有多个通道被设成双流跟随
	{
		wRet =  (u16)ERR_MCU_REPEATEDVMPDSTREAM;
	}

	return wRet;

}

/*==============================================================================
函数名    :  CheckAdpChnnlAtStart
功能      :  开始合成时的前适配通道限制过滤
算法实现  :  
参数说明  :  u8 byMaxHdChnnlNum		[i]	最大前适配通道数
返回值说明:  BOOL32
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-8-20					薛亮							创建
==============================================================================*/
BOOL32 CMcuVcInst::CheckAdpChnnlAtStart(u8 byVmpId, TVMPParam_25Mem &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg)
{
	u8 byIdxInHdChnnl  = 0;
	u8 byIdxOutHdChnnl = 0;
	BOOL32 bOverHdChnnlLmt = FALSE;
	TVmpHdChnnlMemInfo tVmpHdChnnlMemInfo;
	TMt tLocalVmpMt;
	//获得tmt在vmp中所有位置
	u8 abyChnlNo[MAXNUM_VMP_MEMBER];
	u8 byChnlNum = 0;
	TMt tRealSpeaker;
	BOOL32 bNoneKeda = FALSE;
	BOOL32 bDStream = FALSE;
	BOOL32 bG400IPC = FALSE;
	//发言人处理，发言人在多个通道内不进前适配
	if(m_tConf.HasSpeaker() && tVmpParam.IsMtInMember(m_tConf.GetSpeaker() /*GetLocalSpeaker()*/) 
		&& !tVmpParam.IsVMPBrdst()	//VMP广播会把发言人视频广播冲掉
		)
	{
		//获得真正的发言人,mcu时,发言人应该是其上传通道终端
		tRealSpeaker = m_tConf.GetSpeaker();
		if (IsMcu(tRealSpeaker))
		{
			tRealSpeaker = GetSMcuViewMt(tRealSpeaker, TRUE);
		}
		bNoneKeda = ( (!IsKedaMt(m_tConf.GetSpeaker(), TRUE)) || (!tRealSpeaker.IsNull() && !IsKedaMt(tRealSpeaker, FALSE)) );		//是否非keda
		bG400IPC = IsG400IPCMt(tRealSpeaker);
		//tVmpParam.GetChlOfMtInMember( m_tConf.GetSpeaker(), FALSE, MAXNUM_VMP_MEMBER, abyChnlNo, &byChnlNum );
		GetChlOfMtInVmpParam(tVmpParam, m_tConf.GetSpeaker(), byChnlNum, abyChnlNo);
		LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[CheckAdpChnnlAtStart]SpeakerNo:%u,MaxHdChlNo:%u.\n",
			byChnlNum, byMaxHdChnnlNum);
		// 在多个通道,且是非科达终端或bG400IPC终端,仅保留1路
		if ((bNoneKeda || bG400IPC) &&
			byChnlNum > 1 &&
			byChnlNum <= MAXNUM_VMP_MEMBER)
		{
			for (u8 byNum=1; byNum<byChnlNum; byNum++)
			{
				tVmpParam.ClearVmpMember(abyChnlNo[byNum]);
			}
			byChnlNum = 1;
		}
		// 通道数为1时,可以占用前适配
		if (1 == byChnlNum)
		{
			if( byIdxInHdChnnl < byMaxHdChnnlNum )
			{
				tVmpHdChnnlMemInfo.tMtInHdChnnl[byIdxInHdChnnl++] = m_tConf.GetSpeaker();
			}
			else
			{
				tVmpHdChnnlMemInfo.tMtOutHdChnnl[byIdxOutHdChnnl++] = m_tConf.GetSpeaker();
				bOverHdChnnlLmt = TRUE;
				tVmpParam.ClearVmpMember(abyChnlNo[0]);
			}
		}
	}

	TVMPMember tVMPMember;
	TMt tRealMt;
	u8 byChnnlType = LOGCHL_VIDEO;
	TLogicalChannel tLogicChannel;
	BOOL32 bNeedAdjustRes = FALSE;
	u8 byMtStandardFormat = VIDEO_FORMAT_INVALID;
	u8 byReqRes = VIDEO_FORMAT_INVALID;
	BOOL32 bIsMMcu = FALSE;
	//发言人以外成员处理
	for( u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		// 加保护,防止指针返回为空
		if (NULL == tVmpParam.GetVmpMember( byLoop )) {
			tVMPMember.SetNull();
		} else {
			tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		}
		
		if( tVMPMember.IsNull() )
		{
			continue;	//空成员跳过
		}
		// 是否是MMCU
		bIsMMcu = (tVMPMember == m_tCascadeMMCU) || IsNotSupportCascadeMcu(tVMPMember);
		tRealMt = tVMPMember;
		if (IsMcu(tRealMt) && !bIsMMcu)
		{
			tRealMt = GetSMcuViewMt(tRealMt, TRUE);
		}
		
		if( tRealSpeaker == tRealMt)
		//if( GetLocalSpeaker().GetMtId() == tVMPMember.GetMtId() )
		{
			//take speaker into account before,so continue
			continue;
		}

		// 双流跟随通道成员为双流源
		bDStream = FALSE;
		byChnnlType = LOGCHL_VIDEO;
		if (VMP_MEMBERTYPE_DSTREAM == tVMPMember.GetMemberType())
		{
			bDStream = TRUE;
			byChnnlType = LOGCHL_SECVIDEO;
		}
		
		bNoneKeda = ( (!IsKedaMt(tVMPMember, TRUE)) || (!tRealMt.IsNull() && !IsKedaMt(tRealMt, FALSE)) );		//是否非keda
		bG400IPC = IsG400IPCMt(tRealMt);
		if( ( !tVmpParam.IsVMPBrdst()					//VMP广播会把发言人视频广播以及选看全冲掉
			&&  IsSelectedbyOtherMtInMultiCas(tVMPMember) )		//被选看
			|| bNoneKeda				//是否非keda终端
			|| bDStream					//双流源
			|| bIsMMcu					//MMCU
			|| bG400IPC
			)//注：对于判非keda，若传参为0，也会被判为非keda，所以之前要过滤传参为0的情况
		{
			//tVmpParam.GetChlOfMtInMember( tVMPMember, FALSE, MAXNUM_VMP_MEMBER, abyChnlNo, &byChnlNum );
			GetChlOfMtInVmpParam(tVmpParam, tVMPMember, byChnlNum, abyChnlNo);
			if (byChnlNum > 1 && byChnlNum <= MAXNUM_VMP_MEMBER && !bDStream) //忽略双流跟随
			{
				// 非科达终端不支持进多个通道,保留1个通道,其余清空
				// MMCU不支持进多个通道,保留1个通道,其余清空
				// G400IPC不支持进多个通道,保留1个通道,其余清空
				if (bNoneKeda || bIsMMcu || bG400IPC)
				{
					for (u8 byNum=1; byNum<byChnlNum; byNum++)
					{
						tVmpParam.ClearVmpMember(abyChnlNo[byNum]);
					}
					byChnlNum = 1;
				}
				//在多个通道中,不占用前适配
				else
				{
					continue;
				}
			}

			bNeedAdjustRes = FALSE;
			tLocalVmpMt = GetLocalMtFromOtherMcuMt( tVMPMember );
			if ( m_ptMtTable->GetMtLogicChnnl( tLocalVmpMt.GetMtId()/*tVMPMember*/, byChnnlType, &tLogicChannel, FALSE )
				&& MEDIA_TYPE_H264 == tLogicChannel.GetChannelType()
				)
			{
				byMtStandardFormat = tLogicChannel.GetVideoFormat();
				bNeedAdjustRes = VidResAdjust(byVmpId, tVmpParam.GetVMPStyle(), byLoop, byMtStandardFormat, byReqRes);
				
				if (bNeedAdjustRes) //比较下来是需要调整的MT，才有可能让其进前适配通道
				{
					if( byIdxInHdChnnl < byMaxHdChnnlNum )
					{
						tVmpHdChnnlMemInfo.tMtInHdChnnl[byIdxInHdChnnl++] = tVMPMember;
					}
					else
					{
						tVmpHdChnnlMemInfo.tMtOutHdChnnl[byIdxOutHdChnnl++] = tVMPMember;

						bOverHdChnnlLmt = TRUE;
						tVmpParam.ClearVmpMember( byLoop);
					}
				}
			}

		}
	}
	LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[CheckAdpChnnlAtStart]IdxInHdChnnlNo:%u,IdxOutHdChnnlNo:%u.\n",
			byIdxInHdChnnl, byIdxOutHdChnnl);
	if(bOverHdChnnlLmt)
	{
		// 调整好后,已无成员,返回false 场景:0个前适配时,成员全被踢出
		if ( tVmpParam.GetVMPMemberNum() == 0)
		{
			return FALSE;
		}
		LogPrint(LOG_LVL_WARNING, MID_MCU_VMP, "[CheckAdpChnnlAtStart]IdxInHdChnnlNo:%u,IdxOutHdChnnlNo:%u.\n",
			byIdxInHdChnnl, byIdxOutHdChnnl);
		cServMsg.SetMsgBody((u8*)&tVmpHdChnnlMemInfo,sizeof(tVmpHdChnnlMemInfo));
		SendReplyBack(cServMsg, MCU_MCS_VMPOVERHDCHNNLLMT_NTF);
		//SendMsgToAllMcs(MCU_MCS_VMPOVERHDCHNNLLMT_NTF,cServMsg);
	}

	return TRUE;
}

/*==============================================================================
函数名    :  CheckAdpChnnlAtChange
功能      :  改变合成参数处理中的前适配通道限制过滤
算法实现  :  
参数说明  :  u8 byMaxHdChnnlNum		[i]	最大前适配通道数
返回值说明:  void
--------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-8-20                  薛亮
==============================================================================*/
BOOL32 CMcuVcInst::CheckAdpChnnlAtChange(u8 byVmpId, TVMPParam_25Mem &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg)
{
	// [1/19/2010 xliang] FIXME: 成员为下级VIP

	// 成员定义部分
	TMt tLocalVmpMember;			//local化的成员
	TVMPMember tVMPMember;			//通道对应成员
	TVMPMember tLstVmpMember;		//通道对应旧成员
	u8 byChnnlType = LOGCHL_VIDEO;
	TLogicalChannel tLogicChannel;
	BOOL32 bNeedAdjustRes = FALSE;
	u8 byMtStandardFormat = VIDEO_FORMAT_INVALID;	//成员原本分辨率
	u8 byReqRes = VIDEO_FORMAT_INVALID;				//成员要调整到的分辨率
	TMt tRealMt;		//通道真实显示终端(mcu映射上传通道终端)
	BOOL32 bSpeaker = FALSE;
	BOOL32 bNoneKeda = FALSE;
	BOOL32 bSelected = FALSE;
	BOOL32 bDStream = FALSE;
	BOOL32 bIsMMcu	= FALSE;
	BOOL32 bG400IPC = FALSE;

	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
	
	TExInfoForResFps tExInfoForRes;
	BOOL32 bIsNeedAjsRes;
	BOOL32 bSeizePromt; //获得是否有抢占

	//找当前所添加的合成成员
	for( u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		// 加保护,放在指针返回为空
		if (NULL == tVmpParam.GetVmpMember( byLoop )) {
			tVMPMember.SetNull();
		} else {
			tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		}

		if (tVMPMember.IsNull())
		{
			continue;
		}

		if (NULL == tConfVmpParam.GetVmpMember(byLoop)) {
			tLstVmpMember.SetNull();
		} else {
			tLstVmpMember = *tConfVmpParam.GetVmpMember(byLoop);
		}

		// 追加通道属性判断
		if(!tLstVmpMember.IsNull() 
			&& tVMPMember.GetMcuId() == tLstVmpMember.GetMcuId() 
 			&& tVMPMember.GetMtId() == tLstVmpMember.GetMtId() 
			&& tVMPMember.GetMemberType() == tLstVmpMember.GetMemberType()
			)
		{
			continue;
		}

		// 双流跟随通道成员为双流源
		bDStream = FALSE;
		byChnnlType = LOGCHL_VIDEO;
		if (VMP_MEMBERTYPE_DSTREAM == tVMPMember.GetMemberType())
		{
			// 只响应界面将某通道指定为双流跟随时的抢占,若tLstVmpMember已是双流跟随,则不在此处做抢占
			if (VMP_MEMBERTYPE_DSTREAM == tLstVmpMember.GetMemberType())
			{
				continue;
			}
			bDStream = TRUE;
			byChnnlType = LOGCHL_SECVIDEO;
		} 

		// 非H264格式不占用前适配
		tLocalVmpMember  = GetLocalMtFromOtherMcuMt( tVMPMember );
		if (!(m_ptMtTable->GetMtLogicChnnl( tLocalVmpMember.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) &&
			  MEDIA_TYPE_H264 == tLogicChannel.GetChannelType()))
		{
				continue;
		}

		//这次新增加的终端
		tExInfoForRes.m_byEvent = EvMask_VMP;
		tExInfoForRes.m_byOccupy = Occupy_InChannel;
		tExInfoForRes.m_byEqpid = byVmpId;
		tExInfoForRes.m_byPos = byLoop;
		tExInfoForRes.m_tVmpParam = tVmpParam;
		tExInfoForRes.m_byMemberType = tVMPMember.GetMemberType(); //支持双流跟随
		tExInfoForRes.m_bSeizePromt = TRUE;	//需要抢占提示
		tExInfoForRes.m_bConsiderVmpBrd = FALSE; //无视画面合成广播，拖被选看终端需要抢占
		bIsNeedAjsRes = IsNeedAdjustMtVidFormat((TMt)tVMPMember, tExInfoForRes);
		bSeizePromt = tExInfoForRes.m_bResultSeizePromt;
		if (bSeizePromt)
		{
			TMt tSeizeMt = (TMt)tVMPMember;
			cServMsg.SetEqpId(byVmpId);
			cServMsg.SetMsgBody((u8*)&tSeizeMt,sizeof(TMt));	//需要抢占高清适配通道的终端
			u8 byMtNum = tVmpChnnlInfo.GetHDChnnlNum();
			TChnnlMemberInfo atChnnlMemInfo[MAXNUM_MPU2VMP_E20_HDCHNNL];
			for (u8 byIdx=0; byIdx<byMtNum; byIdx++)
			{
				tVmpChnnlInfo.GetHdChnnlInfo(byIdx, &atChnnlMemInfo[byIdx]);
			}
			//添加可选择被抢占终端信息
			cServMsg.CatMsgBody((u8 *)atChnnlMemInfo,sizeof(TChnnlMemberInfo) * byMtNum);
			u8 byForceSeize = 0;		//是否强制抢占,此字段不能删除，界面靠此字段显示取消按钮
			cServMsg.CatMsgBody(&byForceSeize, sizeof(byForceSeize)); 
			SendReplyBack(cServMsg, MCU_MCS_VMPPRISEIZE_REQ);
			//记录
			TVmpPriSeizeInfo tPriSeiInfo;
			tPriSeiInfo.m_tPriSeizeMember = tVMPMember;
			tPriSeiInfo.m_byChlIdx = byLoop;
			g_cMcuVcApp.SetVmpPriSeizeInfo(tVmpEqp, tPriSeiInfo);
			return FALSE;
		}
		

// 		if(!tVMPMember.IsNull() && !m_tLastVmpParam.IsMtInMember(tVMPMember))
		/*{
			tLocalVmpMember  = GetLocalMtFromOtherMcuMt( tVMPMember );
			if ( m_ptMtTable->GetMtLogicChnnl( tLocalVmpMember.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) &&
				MEDIA_TYPE_H264 == tLogicChannel.GetChannelType()
				)
			{
				byMtStandardFormat = tLogicChannel.GetVideoFormat();
				bNeedAdjustRes = VidResAdjust(byVmpId, tVmpParam.GetVMPStyle(), byLoop, byMtStandardFormat, byReqRes);
				if(bNeedAdjustRes)
				{
					tRealMt = IsMcu(tVMPMember) ? GetSMcuViewMt(tVMPMember,TRUE) : tVMPMember;
					//获得真正的发言人,mcu时,发言人应该是其上传通道终端
					tRealSpeaker = m_tConf.GetSpeaker();
					if (IsMcu(tRealSpeaker))
					{
						tRealSpeaker = GetSMcuViewMt(tRealSpeaker, TRUE);
					}
					//特殊身份判断
					bSpeaker = (!tRealSpeaker.IsNull() && tRealSpeaker == tRealMt);	//是否是发言人
					bNoneKeda = ( (!IsKedaMt(tVMPMember, TRUE)) || (!tRealMt.IsNull() && !IsKedaMt(tRealMt, FALSE)) );		//是否非keda
					bSelected = IsSelectedbyOtherMtInMultiCas(tVMPMember);	//是否被选看
					bIsMMcu = (tVMPMember == m_tCascadeMMCU);
					bG400IPC = IsG400IPCMt(tVMPMember);
					if( (bSpeaker && !tVmpParam.IsVMPBrdst())
						|| bNoneKeda 
						|| bSelected
						|| bDStream
						|| bIsMMcu
						|| bG400IPC
						)
					{
						//tVmpParam.GetChlOfMtInMember( tVMPMember, FALSE, MAXNUM_VMP_MEMBER, abyChnlNo, &byChnlNum );
						GetChlOfMtInVmpParam(tVmpParam, tVMPMember, byChnlNum);

						// 在多个通道中,不占用前适配,非科达终端在CheckMpuMember中会判断,跳过双流跟随通道
						if (byChnlNum > 1 && !bDStream)
						{
							continue;
						}
						
						// 0个前适配时,不做抢占提示,直接返回Nack
						if (0 == byMaxHdChnnlNum)
						{
							ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[VmpCommonReq-change] Vip vmp Chnnl is not enough. nack!\n");
							cServMsg.SetErrorCode( ERR_VMP_NO_VIP_VMPCHNNL );
							SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
							return FALSE;
						}

						//若当前要被替换的终端在前适配，则前适配通道还是够的
						if (!tLstVmpMember.IsNull())
						{
							tVmpChnnlInfo.GetHdChnnlInfoByPos(byLoop, &tChnlInfo);
							if (!tChnlInfo.GetMt().IsNull())
							{
								continue;
							}
							/*TMt tLstRealMt = (TMt)tLstVmpMember;
							if (IsMcu(tLstRealMt))
							{
								tLstRealMt = GetSMcuViewMt(tLstRealMt, TRUE);
							}
							if (0 < m_tVmpChnnlInfo.GetChnlCountByMt(tLstRealMt))
							{
								continue;
							}/
						}

						if(tVmpChnnlInfo.GetHDChnnlNum() == byMaxHdChnnlNum) //前适配通道已满，要抢占提示
						{
							TMt tSeizeMt = (TMt)tVMPMember;
							/*if (IsMcu(tSeizeMt))
							{
								tSeizeMt = GetSMcuViewMt(tSeizeMt, TRUE);
							}/
							cServMsg.SetMsgBody((u8*)&tSeizeMt,sizeof(TMt));	//需要抢占高清适配通道的终端
							u8 byMtNum = tVmpChnnlInfo.GetHDChnnlNum();
							TChnnlMemberInfo atChnnlMemInfo[MAXNUM_MPU2VMP_E20_HDCHNNL];
							//u8 abyChnlNo[MAXNUM_MPU2VMP_E13_HDCHNNL];
							//m_tVmpChnnlInfo.GetChnlInfoList(byMtNum, abyChnlNo, atChnnlMemInfo);
							for (u8 byIdx=0; byIdx<byMtNum; byIdx++)
							{
								tVmpChnnlInfo.GetHdChnnlInfo(byIdx, &atChnnlMemInfo[byIdx]);
							}
							//添加可选择被抢占终端信息
							cServMsg.CatMsgBody((u8 *)atChnnlMemInfo,sizeof(TChnnlMemberInfo) * byMtNum);
							
							u8 byForceSeize = 0;		//是否强制抢占 // [1/19/2010 xliang] 新增字段，需界面同步改动
							cServMsg.CatMsgBody(&byForceSeize, sizeof(byForceSeize)); 
							SendReplyBack(cServMsg, MCU_MCS_VMPPRISEIZE_REQ);
							//记录
							TVmpPriSeizeInfo tPriSeiInfo;
							tPriSeiInfo.m_tPriSeizeMember = tVMPMember;
							tPriSeiInfo.m_byChlIdx = byLoop;
							g_cMcuVcApp.SetVmpPriSeizeInfo(tVmpEqp, tPriSeiInfo);
							//m_atVMPTmpMember[byLoop] = tVMPMember;
							//m_tVmpAdaptChnSeizeOpr.m_tMt = tVMPMember;
							return FALSE;
						}
					}
				}
			}
		}*/
	}
	return TRUE;
}
/*==============================================================================
函数名    :  SendVmpPriSeizeReq
功能      :  抢占提示消息打包
算法实现  :  
参数说明  :  TMt tMt [i] 抢占终端；
			 u8 byForceSeize [i] 是否强制抢占(预留)
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本			修改人          走读人          修改记录
2011-11-30					yanghuaizhi
==============================================================================*/
void CMcuVcInst::SendVmpPriSeizeReq(u8 byVmpId, TMt &tSeizeMt, u8 byForceSeize)
{
	if (!IsValidVmpId(byVmpId))
	{
		return;
	}

	CServMsg cServMsg;
	cServMsg.SetEqpId(byVmpId);
	cServMsg.SetMsgBody((u8*)&tSeizeMt,sizeof(TMt));	//需要抢占高清适配通道的终端
	// 获得按终端整理后的前适配信息
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
	u8 byMtNum = tVmpChnnlInfo.GetHDChnnlNum();
	TChnnlMemberInfo atChnnlMemInfo[MAXNUM_MPU2VMP_E20_HDCHNNL];
	//u8 abyChnlNo[MAXNUM_MPU2VMP_E13_HDCHNNL];//目前前适配终端个数都为1
	//m_tVmpChnnlInfo.GetChnlInfoList(byMtNum, abyChnlNo, atChnnlMemInfo);
	for (u8 byIdx=0; byIdx<byMtNum; byIdx++)
	{
		tVmpChnnlInfo.GetHdChnnlInfo(byIdx, &atChnnlMemInfo[byIdx]);
	}

	//添加可选择被抢占终端信息
	cServMsg.CatMsgBody((u8 *)atChnnlMemInfo,sizeof(TChnnlMemberInfo) * byMtNum);
	cServMsg.CatMsgBody(&byForceSeize, sizeof(byForceSeize)); //此字段不能删除，界面靠此字段显示取消按钮
	SendMsgToAllMcs(MCU_MCS_VMPPRISEIZE_REQ, cServMsg);
}

/*==============================================================================
函数名    :  SendVmpParamToChairMan
功能      :  通知主席终端刷新vmpparam信息
算法实现  :  
参数说明  :  
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2012-07-09                yanghuaizhi
==============================================================================*/
void CMcuVcInst::SendVmpParamToChairMan()
{
	CServMsg cServMsg;
	u8 byVmpCount = GetVmpCountInVmpList();
	TEqp tVmpEqp;
	tVmpEqp.SetNull();
	if (1 == byVmpCount)
	{
		u8 byVmpId = GetTheOnlyVmpIdFromVmpList();
		tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	}
	else
	{
		if (HasJoinedChairman())
		{
			SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVMP_NOTIF, cServMsg );
			return;
		}
	}
	// 有主席,且vmp合成中,给主席发送VmpParam
	if (HasJoinedChairman() /*&& CONF_VMPMODE_NONE != g_cMcuVcApp.GetVMPMode(m_tVmpEqp)*/)
	{
		TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
		TVMPMember *ptVmpMember = NULL;
		u8 byLoop = 0;
		
		for(byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++)
		{
			ptVmpMember = tVmpParam.GetVmpMember(byLoop);
			if( ptVmpMember == NULL )
			{
				continue;
			}
			if( ptVmpMember->IsNull())
			{
				// 为空
			}
			else
			{
				if( !ptVmpMember->IsLocal() )
				{
					TMt tTempMt;
					tTempMt.SetMcuId( ptVmpMember->GetMcuId() );
					tTempMt.SetMtId( ptVmpMember->GetMtId() );
					tTempMt = GetLocalMtFromOtherMcuMt( tTempMt );
					ptVmpMember->SetMt( tTempMt );
				}
			}
		}
		
		for(; byLoop < MAXNUM_VMP_MEMBER; byLoop++)
		{
			ptVmpMember = tVmpParam.GetVmpMember(byLoop);
			if( ptVmpMember == NULL )
			{
				continue;
			}
			ptVmpMember->SetMcuId( 0 );
			ptVmpMember->SetMtId( 0 );
		}
		
		cServMsg.SetMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
		SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_VMPPARAM_NOTIF, cServMsg );
	}
}

/*==============================================================================
函数名    :  UpdateVmpDStream
功能      :  调整vmp双流跟随真实双流源,上报会控
算法实现  :  
参数说明  :  
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2012-07-31                yanghuaizhi
==============================================================================*/
void CMcuVcInst::UpdateVmpDStream(const TMt tMt)
{
	if (tMt.IsNull())
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VMP, "[UpdateVmpDStream]tMt is null.\n");
		return;
	}

	// 会控模式下有双流跟随通道
	TEqp tVmpEqp;
	TVMPParam_25Mem tConfVmpParam;
	TPeriEqpStatus tPeriEqpStatus;
	TVMPParam_25Mem tVmpParam;
	TVMPParam_25Mem tLastVmpParam;
	u8 byChlIdx;
	TVMPMember tTmpMember;
	TVMPMember* ptMember;
	CServMsg cServMsg;
	for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			continue;
		}
		tVmpEqp = g_cMcuVcApp.GetEqp( m_abyVmpEqpId[byIdx] );

		tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
		if (tConfVmpParam.GetVMPMode() == CONF_VMPMODE_CTRL)
		{
			if (g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus))
			{
				tVmpParam = tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam();
				tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
				byChlIdx = tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_DSTREAM);
				//有双流跟随通道
				if (byChlIdx < MAXNUM_VMP_MEMBER)
				{
					// 更新外设状态信息
					tTmpMember.SetNull();
					ptMember = tVmpParam.GetVmpMember(byChlIdx);
					ptMember->SetMemberTMt(tMt);
					tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);
					g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					// 更新会议信息
					ptMember = tConfVmpParam.GetVmpMember(byChlIdx);
					if (VMP_MEMBERTYPE_DSTREAM == ptMember->GetMemberType())
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[UpdateVmpDStream]m_tConf is updated, chlIdx:%d.\n", byChlIdx);
						tTmpMember = *ptMember;
						ptMember->SetMemberTMt(tMt);
						//m_tConf.m_tStatus.SetVmpParam(tVmpParam);
						g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tConfVmpParam);
					}
					// 外设刷新双流跟随别名
					RefreshVmpChlMemalias(tVmpEqp.GetEqpId());
					// 更新m_tLastVmpParam信息
					ptMember = tLastVmpParam.GetVmpMember(byChlIdx);
					if (VMP_MEMBERTYPE_DSTREAM == ptMember->GetMemberType() &&
						tTmpMember == *ptMember)
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[UpdateVmpDStream]m_tLastVmpParam is updated.\n");
						ptMember->SetMemberTMt(tMt);
					}
					g_cMcuVcApp.SetLastVmpParam(tVmpEqp, tLastVmpParam);
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[UpdateVmpDStream]PeriEqpStatus is updated, chlIdx:%d, tMt(%d,%d).\n",
						byChlIdx, tMt.GetMcuId(), tMt.GetMtId());
					// 通知会控,刷vmp通道成员+刷下级双流源在vmp图标
					cServMsg.Init();
					cServMsg.SetEqpId(tVmpEqp.GetEqpId());
					cServMsg.SetMsgBody( (u8*)&tConfVmpParam, sizeof(tConfVmpParam) );
					SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
					/*cServMsg.SetMsgBody( (u8*)&tPeriEqpStatus, sizeof(TPeriEqpStatus) );
 					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );*/
				}
			}
		}
	}
}

/*==============================================================================
函数名    :  CheckAdpChnnlLmt
功能      :  前适配通道限制过滤
算法实现  :  
参数说明  :  BOOL32 bChgParam	[i]		TRUE，表改变合成参数处理中的过滤；
										FALSE，表刚开始合成处理中的过滤
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-8-20					薛亮
==============================================================================*/
BOOL32 CMcuVcInst::CheckAdpChnnlLmt(u8 byVmpId, TVMPParam_25Mem& tVmpParam, CServMsg &cServMsg, BOOL32 bChgParam /* = FALSE */)
{
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
	
	ConfPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[CheckAdpChnnlLmt] byVmpSubType == %d\n",byVmpSubType);

	// 开启时更新最大前适配数目
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
	if (!bChgParam)
	{
		tVmpChnnlInfo.clear();// 开启时清空前适配信息
		u8 byMaxStyleNum = 0; //最大支持多少风格内不降分辨率
		u8 byMaxHdChnnlNum = GetMaxHdChnlNumByVmpId(m_tConf, byVmpId, byMaxStyleNum);
		tVmpChnnlInfo.SetMaxNumHdChnnl(byMaxHdChnnlNum);
		tVmpChnnlInfo.SetMaxStyleNum(byMaxStyleNum);
		g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
	}

	// 点名时,不考虑前适配抢占
	if(VMP == byVmpSubType || tVmpParam.IsVMPAuto() || ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode())
	{
		return TRUE;
	}

	// MPU的4画面及以下,MPU2的支持的画面个数以下都不需要调整
	// 8kh1080p/30fps非适配会议开启的vmp4画面及以下也不需要调整
	u8 byMemberNum = tVmpParam.GetMaxMemberNum();
	if (/*(MPU_SVMP == byVmpSubType && byMemberNum <= 4) ||
		(Is8khVmpInConf1080p30fps(m_tConf, byVmpId) && byMemberNum <= 4) ||*/ //8kh不再做特殊判断
		byMemberNum <= tVmpChnnlInfo.GetMaxStyleNum())
	{
		//无需调整时,初始化前适配信息
		tVmpChnnlInfo.clearHdChnnl();
		g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
		return TRUE;
	}

	if(!bChgParam)
	{
		return CheckAdpChnnlAtStart(byVmpId, tVmpParam, tVmpChnnlInfo.GetMaxNumHdChnnl(), cServMsg);
	}
	else
	{
		return CheckAdpChnnlAtChange(byVmpId, tVmpParam, tVmpChnnlInfo.GetMaxNumHdChnnl(), cServMsg);
	}

}

/*==============================================================================
函数名    :  CheckMpuMember
功能      :  校验MPU成员
算法实现  :  详见注释
参数说明  :  TVMPParam &	[in]	合成参数
			 u16 &			[out]	错误码
返回值说明:  BOOL32					fail则返回FALSE
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-8-19	 4.6			薛亮							整理创建
==============================================================================*/
BOOL32 CMcuVcInst::CheckMpuMember(u8 byVmpId, TVMPParam_25Mem& tVmpParam, u16& wErrorCode, BOOL32 bChgParam /*= FALSE */)
{
	//自动画面合成不进行以下校验
	if (tVmpParam.IsVMPAuto())
	{
		return TRUE;
	}
	
	//VMP不支持主席跟随
	if(tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_CHAIRMAN))
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[VmpCommonReq]MPU not support chairman-membertype!\n");
		wErrorCode = ERR_VMPUNSUPPORTCHAIRMAN;
		return FALSE;
	}


		//会议中不允许同一个MT占了多个成员通道,追加判断Other终端在多个通道内
	u16 wRet = 0;
	wRet = CheckSameMtInMultChnnl(byVmpId, tVmpParam, bChgParam);
	if ( wRet != 0 )//wRet即为errorcode
	{
		wErrorCode = wRet;
		return FALSE;
	}		
	//会议带音频轮询时，画面合成器不支持轮询跟随和发言人跟随同时存在
	wRet = CheckMpuAudPollConflct(byVmpId, tVmpParam, bChgParam);
	if ( wRet != 0 )//wRet即为errorcode
	{
		wErrorCode = wRet;
		return FALSE;
	}


	return TRUE;
}

/*==============================================================================
函数名    :  GetTheMainVmpIdFromVmpList
功能      :  获得一个主要vmp，用于点名时有多个合成器，指定一个用作点名
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
u8 CMcuVcInst::GetTheMainVmpIdFromVmpList()
{
	u8 byVmpId = 0;
	// 若当前有广播vmp，则定为广播vmp
	if (!m_tVmpEqp.IsNull())
	{
		if (IsVmpIdInVmpList(m_tVmpEqp.GetEqpId()))//广播vmp可能掉线
		{
			byVmpId = m_tVmpEqp.GetEqpId();
		}
	}
	else// 找第一个非广播的vmp，若无广播也无非广播vmp，则返回0
	{
		for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
		{
			if (IsValidVmpId(m_abyVmpEqpId[byIdx]))
			{
				byVmpId = m_abyVmpEqpId[byIdx];
				break;
			}
		}
	}

	return byVmpId;
}

/*==============================================================================
函数名    :  IsVmpIdInVmpList
功能      :  判断会议vmpList中是否有指定VmpId
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsVmpIdInVmpList(u8 byVmpId)
{
	if(!IsValidVmpId(byVmpId))
	{
		return FALSE;
	}

	for (u8 byIdx = 0; byIdx < MAXNUM_CONF_VMP; byIdx++)
	{
		if (byVmpId == m_abyVmpEqpId[byIdx])
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*==============================================================================
函数名    :  RemoveVmpIdFromVmpList
功能      :  将指定vmp从索引删除
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::RemoveVmpIdFromVmpList(u8 byVmpId)
{
	for (u8 byIdx = 0; byIdx < MAXNUM_CONF_VMP; byIdx++)
	{
		if (byVmpId == m_abyVmpEqpId[byIdx])
		{
			m_abyVmpEqpId[byIdx] = 0;
		}
	}

	// 模版vmp相关处理,若清除的vmpid是模版开启的vmp，则清除模版vmp
	if (IsValidVmpId(byVmpId) && m_tModuleVmpEqp.GetEqpId() == byVmpId)
	{
		m_tModuleVmpEqp.SetNull();
		//停止画面复合后，VMP模板不再生效
		m_tConfInStatus.SetVmpModuleUsed(TRUE);
	}

	if( IsValidVmpId(byVmpId) && m_byCreateBy == CONF_CREATE_NPLUS)
	{
		g_cMcuVcApp.NplusRemoveVmpModuleEqpId(byVmpId);

	}
}

/*==============================================================================
函数名    :  AddVmpIdIntoVmpList
功能      :  将指定vmp添加到索引
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::AddVmpIdIntoVmpList(u8 byVmpId)
{
	if(!IsValidVmpId(byVmpId))
	{
		return;
	}

	u8 byTmpPos = MAXNUM_CONF_VMP;
	for (u8 byIdx = 0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		// 找到第一个可存储位置
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]) && MAXNUM_CONF_VMP == byTmpPos)
		{
			byTmpPos = byIdx;
		}
		// 已在索引中，则无需添加
		if (byVmpId == m_abyVmpEqpId[byIdx])
		{
			return;
		}
	}

	if (MAXNUM_CONF_VMP != byTmpPos)
	{
		m_abyVmpEqpId[byTmpPos] = byVmpId;
	}
}

/*==============================================================================
函数名    :  GetVmpCountInVmpList
功能      :  获得索引中占用合成器个数
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
u8 CMcuVcInst::GetVmpCountInVmpList()
{
	u8 byCount = 0;
	for (u8 byIdx = 0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			byCount++;
		}
	}
	return byCount;
}

/*==============================================================================
函数名    :  GetTheOnlyVmpIdFormVmpList
功能      :  从索引中获得仅有的默认合成器
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
u8 CMcuVcInst::GetTheOnlyVmpIdFromVmpList(void)
{
	u8 byVmpId = 0;
	// 从索引数组中找到唯一的一个会议占用的合成器，若占用多个，返回0
	for (u8 byIdx = 0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			if (0 == byVmpId)
			{
				byVmpId = m_abyVmpEqpId[byIdx];
			}
			else
			{
				byVmpId = 0;
				break;
			}
		}
	}

	return byVmpId;
}

/*==============================================================================
函数名    :  IsVmpStatusUseStateInVmpList
功能      :  判断当前会议占用的合成器中是否有指定UseState的合成器
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsVmpStatusUseStateInVmpList(u8 byUseState)
{
	u8 byVmpId;
	TPeriEqpStatus tPeriEqpStatus; 
	for (u8 byIdx = 0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		byVmpId = m_abyVmpEqpId[byIdx];
		if (IsValidVmpId(byVmpId))
		{
			if (g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus ))
			{
				if (byUseState == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

/*==============================================================================
函数名    :  ClearAdpChlInfoInAllVmp
功能      :  清某终端在vmp的所有前适配
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::ClearAdpChlInfoInAllVmp(TMt tMt)
{
	if (tMt.IsNull())
	{
		return;
	}
	if (IsMcu(tMt) && !(tMt == m_tCascadeMMCU))
	{
		//找到此MCU最底层上传终端
		TMt tTmpMt = GetSMcuViewMt(tMt, TRUE);
		if (!tTmpMt.IsNull())
		{
			tMt = tTmpMt;
		}
	}

	u8 byVmpId;
	TEqp tVmpEqp;
	TVmpChnnlInfo tVmpChnnlInfo;
	//按终端清除前适配通道
	for (u8 byTmpIdx=0; byTmpIdx<MAXNUM_CONF_VMP; byTmpIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byTmpIdx]))
		{
			continue;
		}
		byVmpId = m_abyVmpEqpId[byTmpIdx];
		tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
		tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);

		tVmpChnnlInfo.ClearChnlByMt(tMt);
		g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
	}
}

/*==============================================================================
函数名    :  ChangeMtResFpsInTW
功能      :  调整画面合成内终端分辨率
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-04					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::ChangeMtResFpsInTW(TMt tMt, BOOL32 bOccupy)
{
	BOOL32 bRet = TRUE;
	TExInfoForResFps tExInfoForResFps;
	tExInfoForResFps.m_byEvent = EvMask_TVWall;
	if (bOccupy)
	{
		tExInfoForResFps.m_byOccupy = Occupy_InChannel;
	}
	else
	{
		tExInfoForResFps.m_byOccupy = Occupy_OutChannel;
	}
	
	BOOL32 bIsNeedAjsRes = IsNeedAdjustMtVidFormat(tMt, tExInfoForResFps);
	u8 byNewRes = tExInfoForResFps.m_byNewFormat;	//获得要调整到的分辨率
	BOOL32 bSetChlRes = tExInfoForResFps.m_bResultSetVmpChl; //获得是否Setchnl
	BOOL32 bStart = tExInfoForResFps.m_bResultStart; //获得给下级发送调整还是恢复分辨率
	BOOL32 bIsNeedAjsFps = IsNeedAdjustMtVidFps(tMt, tExInfoForResFps);
	BOOL32 bSetChlFps = tExInfoForResFps.m_bResultSetVmpChl; //获得是否Setchnl
	u8 byNewFps = tExInfoForResFps.m_byNewFps;


	if (bIsNeedAjsRes)
	{
		// 发送调分辨率消息
		ChangeVFormat(tMt, bStart, byNewRes);
	}

 	if (bIsNeedAjsFps)
	{
		// 发送调帧率消息
		SendChgMtFps(tMt, LOGCHL_VIDEO, byNewFps, bStart);
	}

	if (bOccupy)
	{
		// 进通道时不能建通道交换，认为调分辨率失败
		if (!bSetChlRes || !bSetChlFps)
		{
			bRet = FALSE;
		}
	}
	
	return bRet;
}

/*==============================================================================
函数名    :  ChangeMtResFpsInVmp
功能      :  调整画面合成内终端分辨率
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::ChangeMtResFpsInVmp(u8 byVmpId, TMt tMt, TVMPParam_25Mem *ptVMPParam, 
									   BOOL32 bOccupy/* = TRUE*/, 
									   u8 byMemberType/* = VMP_MEMBERTYPE_NULL*/, 
									   u8 byPos/* = MAXNUM_VMP_MEMBER*/,
									   BOOL32 bMsgMcs/* = FALSE*/)
{
	if (!IsValidVmpId(byVmpId) || tMt.IsNull() || ptVMPParam == NULL)
	{
		return FALSE;
	}

	BOOL32 bRet = TRUE;
	TExInfoForResFps tExInfoForResFps;
	tExInfoForResFps.m_byEvent = EvMask_VMP;
	if (bOccupy)
	{
		tExInfoForResFps.m_byOccupy = Occupy_InChannel;
	}
	else
	{
		tExInfoForResFps.m_byOccupy = Occupy_OutChannel;
	}
	tExInfoForResFps.m_byEqpid = byVmpId;
	tExInfoForResFps.m_byPos = byPos;
	tExInfoForResFps.m_byMemberType = byMemberType;
	tExInfoForResFps.m_tVmpParam = *ptVMPParam;
	tExInfoForResFps.m_bConsiderVmpBrd = FALSE;//合成成员调整时，无视vmp广播
	BOOL32 bIsNeedAjsRes = IsNeedAdjustMtVidFormat(tMt, tExInfoForResFps);
	u8 byNewRes = tExInfoForResFps.m_byNewFormat;	//获得要调整到的分辨率
	BOOL32 bSetVmpChlRes = tExInfoForResFps.m_bResultSetVmpChl; //获得是否Setvmpchnl
	BOOL32 bSeizePromt = tExInfoForResFps.m_bResultSeizePromt; //获得是否有抢占
	BOOL32 bStart = tExInfoForResFps.m_bResultStart; //获得给下级发送调整还是恢复分辨率
	/* 有抢占，则不做后续处理,此函数不做抢占处理，此返回值不会为TRUE
	if (bSeizePromt)
	{
		u8 bySeizeVmpid = tExInfoForResFps.m_byEqpid;
		u8 bySeizePos = tExInfoForResFps.m_byPos;
		// 抢占处理
	}*/

	BOOL32 bIsNeedAjsFps = IsNeedAdjustMtVidFps(tMt, tExInfoForResFps);
	BOOL32 bSetVmpChlFps = tExInfoForResFps.m_bResultSetVmpChl; //获得是否Setvmpchnl
	u8 byNewFps = tExInfoForResFps.m_byNewFps;
	
	if (bIsNeedAjsRes)
	{
		// 发送调分辨率消息
		ChangeVFormat(tMt, bStart, byNewRes, byVmpId, ptVMPParam->GetVMPStyle(), byPos);
	}
	
	if (bIsNeedAjsFps)
	{
		// 发送调帧率消息
		SendChgMtFps(tMt, LOGCHL_VIDEO, byNewFps, bStart);
	}

	// 确定可建交换，终端出vmp通道，不做建交换处理
	if (bOccupy)
	{
		if (bSetVmpChlRes && bSetVmpChlFps)
		{
			// 本级终端直接Setvmpchnl,不需要调分辨率的下级终端直接建交换,需要调分辨率的下级终端等调分辨率Ack后Setvmpchnl
			if (tMt.IsLocal() || !bIsNeedAjsRes)
			{
				// 终端已建交换的话，无需再重新建交换
				TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
				TVMPParam_25Mem tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
				TVMPMember tLastVmpMem = *tLastVmpParam.GetVmpMember(byPos);
				if (tLastVmpMem.GetMemberType() == byMemberType &&
					tLastVmpMem.GetMcuId() == tMt.GetMcuId() &&
					tLastVmpMem.GetMtId() == tMt.GetMtId())
				{
					//不做处理
				}
				else
				{
					SetVmpChnnl(byVmpId, tMt, byPos, byMemberType, bMsgMcs);
				}
			}
		}
		else
		{
			// 进通道时，最终得出不能做Setvmpchnl处理，认为调分辨率失败
			bRet = FALSE;
		}
	}

	return bRet;
}

/*==============================================================================
函数名    :  IsNotSupportCascadeMcu
功能      :  判断某MCU是否是简单级联的MCU
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-04					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsNotSupportCascadeMcu(const TMt &tMt)
{
	// 只支持判本级下的mcu
	if (tMt.IsNull() || !tMt.IsLocal())
	{
		return FALSE;
	}

	// 本会议为简单级联会议时，利用ManuId判断
	if (!m_tConf.GetConfAttrb().IsSupportCascade())
	{
		// 配置简单级联会议，不能用IsMcu()接口判断mcu
		u8 byManuId = m_ptMtTable->GetManuId( tMt.GetMtId() );
		if(byManuId == MT_MANU_KDCMCU)
		{
			return TRUE;
		}
	}
	else if (IsMcu(tMt)) //未配置简单级联会议，可能下级配置了简单级联
	{
		// 不支持MMCU的下级mcu认定为简单级联mcu
		TMultiCapSupport tCapSupport;
		m_ptMtTable->GetMtMultiCapSupport( tMt.GetMtId(), &tCapSupport );
		if (!tCapSupport.IsSupportMMcu())
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*==============================================================================
函数名    :  IsVmpSeeByChairman
功能      :  主席终端是否在选看画面合成
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-04					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsVmpSeeByChairman()
{
	// 有主席，且主席选看源是vmp时
	if (HasJoinedChairman())
	{
		TMt tChairMan = m_tConf.GetChairman();
		u8 byMode = MODE_VIDEO;
		TMtStatus tDstMtStatus;
		m_ptMtTable->GetMtStatus( tChairMan.GetMtId(), &tDstMtStatus );
		TMt tSelMt = tDstMtStatus.GetSelectMt(byMode);
		if ( !tSelMt.IsNull() &&
			tSelMt.GetType() == TYPE_MCUPERI &&
			tSelMt.GetEqpType() == EQP_TYPE_VMP)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*==============================================================================
函数名    :  IsCanAdjustMtRes
功能      :  返回终端能不能调分辨率
算法实现  :  
参数说明  :  
返回值说明:  FALSE：不能调，TRUE：可调
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					   陈兵							
==============================================================================*/
BOOL32 CMcuVcInst::IsCanAdjustMtRes(TMt tMt)
{
	if (   (m_tCascadeMMCU == tMt)
		|| IsNotSupportCascadeMcu(tMt)
		/*|| !IsKedaMt(tMt, TRUE)*/
		|| (!tMt.IsNull() && !IsKedaMt(tMt, FALSE))
		|| IsG400IPCMt(tMt)
		|| (MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId())
			&& !GetSMcuViewMt(tMt).IsNull() && !IsKedaMt(GetSMcuViewMt(tMt), FALSE))
	   )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/*==============================================================================
函数名    :  IsCanAdjustMtFps
功能      :  返回终端能不能调帧率
算法实现  :  
参数说明  :  
返回值说明:  FALSE：不能调，TRUE：可调
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-08-07					陈兵							
==============================================================================*/
BOOL32 CMcuVcInst::IsCanAdjustMtFps(TMt tMt)
{
	BOOL32 bIsMtCanAdjust = FALSE;

	if ( (m_tCascadeMMCU == tMt)
		|| IsNotSupportCascadeMcu(tMt)
		|| IsG400IPCMt(tMt)
		)
	{
		return FALSE;
	}

	if (tMt.IsLocal())
	{
		// 本级科达终端认为可调
		if (!IsMcu(tMt) && IsKedaMt(tMt, FALSE))
		{
			bIsMtCanAdjust = TRUE;
		}
		else if(IsMcu(tMt) && IsNeedChangeFpsMt(tMt.GetMtId()))
		{
			TMt tMtTemp = GetSMcuViewMt(tMt, TRUE);
			// 下级Mcu无上传源认为可调
			if (tMtTemp.IsNull())
			{
				bIsMtCanAdjust = TRUE;
			}
			else if (!tMtTemp.IsNull() && IsKedaMt(tMtTemp, FALSE))
			{
				//下级Mcu上传源为科达终端认为可调
				bIsMtCanAdjust = TRUE;
			}
		}
	}
	else
	{
		// 下级为科达终端认为可调
		if ( IsKedaMt(tMt, FALSE) )
		{
			bIsMtCanAdjust = TRUE;
		}
	}

	return bIsMtCanAdjust;
}

/*==============================================================================
函数名    :  IsVipMemForVmp
功能      :  返回终端vmp的vip属性
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
TChnnlMemberInfo CMcuVcInst::IsVipMemForVmp(const TMt &tMt, TExInfoForResFps &tExInfoForRes)
{
	TChnnlMemberInfo tChnnlMemInfo;
	if (tMt.IsNull())
	{
		return tChnnlMemInfo;
	}

	BOOL32 bIsMMcu = (m_tCascadeMMCU == tMt) || IsNotSupportCascadeMcu(tMt);
	BOOL32 bDStream = FALSE;
	if (VMP_MEMBERTYPE_DSTREAM == tExInfoForRes.m_byMemberType)
	{
		bDStream = TRUE;
	}
	// 若是mcu,则取其上传通道终端,用于前适配处理
	TMt tRealMt = tMt;
	if (IsMcu(tRealMt) && !bDStream && !bIsMMcu) //双流源不做映射,MMCU也不做映射
	{
		tRealMt = GetSMcuViewMt(tRealMt, TRUE);
	}
	BOOL32 bNoneKeda = ( (!IsKedaMt(tMt, TRUE)) || (!tRealMt.IsNull() && !IsKedaMt(tRealMt, FALSE)) || tExInfoForRes.m_bNonKeda );	//是否非keda终端(该特殊身份提前判)
	BOOL32 bG400IPCMt = IsG400IPCMt(tRealMt); // 8000H-M特有需求: IPC前端进前适配通道 [pengguofeng 10/31/2012]

	//获得真正的发言人,mcu时,发言人应该是其上传通道终端
	TMt tRealSpeaker = m_tConf.GetSpeaker();
	if (IsMcu(tRealSpeaker))
	{
		tRealSpeaker = GetSMcuViewMt(tRealSpeaker, TRUE);
	}
	BOOL32 bSpeaker = (!tRealSpeaker.IsNull() && tRealSpeaker == tRealMt);	//是否是发言人
	BOOL32 bSelected = IsSelectedbyOtherMtInMultiCas(tRealMt);				//是否被选看

	tChnnlMemInfo.SetMt(tRealMt);
	// 8000H-M需求:监控IPC前端必须进前适配通道 [pengguofeng 11/2/2012]
	if ( bG400IPCMt)
	{
		tChnnlMemInfo.SetAttrIPC();
	}
	// 双流源不考虑其它vip属性
	else if (bDStream)
	{
		tChnnlMemInfo.SetAttrDstream();
	}
	// 上级MMCU进vmp也不考虑其它vip属性
	else if (bIsMMcu)
	{
		tChnnlMemInfo.SetAttrMMcu();
	}
	else if (bNoneKeda)
	{
		tChnnlMemInfo.SetAttrNoneKeda();
	}
	else
	{
		// 当前是否vmp在广播
		BOOL32 bIsBrdstVmp = FALSE;
		TMt tVidBrdSrc = GetVidBrdSrc();
		if (!tVidBrdSrc.IsNull() && EQP_TYPE_VMP == tVidBrdSrc.GetEqpType())
		{
			bIsBrdstVmp = TRUE;
		}
		if(bSpeaker && !bIsBrdstVmp)
		{
			tChnnlMemInfo.SetAttrSpeaker();
		}

		BOOL32 bSelectedByRoll = FALSE;		// xliang [5/7/2009] 点名造成的选看。因为时序问题，这里特殊处理下
		if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
			&& ( m_tRollCaller == tMt 
			|| m_tRollCallee == tMt ) )  
		{
			bSelectedByRoll = TRUE;
		}

		if( ( bSelected 
			&& ( (!bIsBrdstVmp && !bSpeaker)	//非VMP广播下发言人会冲走被选看
			|| !tExInfoForRes.m_bConsiderVmpBrd ) )
			|| bSelectedByRoll									//点名造成的选看，同VMP广播与否无关
			)
		{
			tChnnlMemInfo.SetAttrSelected();
		}
	}

	return tChnnlMemInfo;
}

/*==============================================================================
函数名    :  IsVmpSupportNplus
功能      :  针对Nplus备份多画面合成
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsVmpSupportNplus(u8 byVmpSubType, u8 byVmpStyle, u8 &byEqualAbilityVmpId, u8 &bySlightlyLargerAbilityVmpId, u8 &byMinAbilityVmpId)
{
	// 参数判断
	if (byVmpSubType != MPU2_VMP_ENHACED &&
		byVmpSubType != MPU2_VMP_BASIC &&
		byVmpSubType != MPU_SVMP &&
		byVmpSubType != VMP )
	{
		return FALSE;
	}
	byEqualAbilityVmpId = 0;
	bySlightlyLargerAbilityVmpId = 0;
	byMinAbilityVmpId = 0;

	// MPU2_VMP_ENHACED > MPU2_VMP_BASIC > MPU_SVMP > VMP 
	u8 byIdleVMPNum = 0;
	u8 abyIdleVMPId[MAXNUM_PERIEQP];
	memset( abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
	g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );
	u8 byTmpVmpId;
	u8 byTmpVmpSubType;
	TPeriEqpStatus tPeriEqpStatus; 
	u16 wError = 0;
	u8 byTmpSLargerVmpId = 0;
	u8 byTmpSLargerVmpSubType = byVmpSubType;
	u8 byTmpMinVmpId = 0;
	u8 byTmpMinVmpSubType = byVmpSubType;
	for (u8 byIdx=0; byIdx<byIdleVMPNum; byIdx++)
	{
		byTmpVmpId = abyIdleVMPId[byIdx];
		if (g_cMcuVcApp.GetPeriEqpStatus(byTmpVmpId, &tPeriEqpStatus) &&
			IsVMPSupportVmpStyle(byVmpStyle, byTmpVmpId, wError))
		{
			byTmpVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
			if (byTmpVmpSubType == byVmpSubType)
			{
				if (0 == byEqualAbilityVmpId)
				{
					byEqualAbilityVmpId = byTmpVmpId;
				}
			}
			else if (byTmpVmpSubType > byVmpSubType)//能力比传入能力强
			{
				if (0 == byTmpSLargerVmpId)//第一次记录较强能力id
				{
					byTmpSLargerVmpId = byTmpVmpId;
					byTmpSLargerVmpSubType = byTmpVmpSubType;
				}
				else
				{
					if (byTmpSLargerVmpSubType > byTmpVmpSubType)//之后判比存的能力小的话更新，变为较小
					{
						byTmpSLargerVmpId = byTmpVmpId;
						byTmpSLargerVmpSubType = byTmpVmpSubType;
					}
				}
			}
			else//更新最小能力
			{
				if (0 == byTmpMinVmpId)
				{
					byTmpMinVmpId = byTmpVmpId;
					byTmpMinVmpSubType = byTmpVmpSubType;
				}
				else
				{
					if (byTmpMinVmpSubType > byTmpVmpSubType)//之后判比临时存的能力小的话更新临时最小能力，变为较小
					{
						byTmpMinVmpId = byTmpVmpId;
						byTmpMinVmpSubType = byTmpVmpSubType;
					}
				}
			}
		}
	}
	
	// 更新稍大的vmpid，与最小的vmpid
	bySlightlyLargerAbilityVmpId = byTmpSLargerVmpId;
	byMinAbilityVmpId = byTmpMinVmpId;

	// 1个都未找到
	if (0 == byEqualAbilityVmpId &&
		0 == bySlightlyLargerAbilityVmpId &&
		0 == byMinAbilityVmpId)
	{
		return FALSE;
	}

	return TRUE;
}

/*==============================================================================
函数名    :  GetVMPBrdBitrate
功能      :  获得vmp对应通道码率
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
u16 CMcuVcInst::GetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx)
{
	return g_cMcuVcApp.GetVMPOutChlBitrate(tVmpEqp, byIdx);
}

/*==============================================================================
函数名    :  SetVMPBrdBitrate
功能      :  设置vmp对应通道码率
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::SetVMPOutChlBitrate(TEqp tVmpEqp, u8 byIdx, u16 wBr)
{
	g_cMcuVcApp.SetVMPOutChlBitrate(tVmpEqp, byIdx, wBr);
}

/*==============================================================================
函数名    :  SetVMPBrdst
功能      :  设置vmp对应广播模式,需更新Eqpstatus中的广播状态
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::SetVMPBrdst(TEqp tVmpEqp, u8 byVMPBrdst)
{
	g_cMcuVcApp.SetVMPBrdst(tVmpEqp, byVMPBrdst);
	// 同步外设状态
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(tVmpEqp.GetEqpId() , &tPeriEqpStatus);
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam.SetVMPBrdst(byVMPBrdst);
	g_cMcuVcApp.SetPeriEqpStatus(tVmpEqp.GetEqpId() , &tPeriEqpStatus);
}

/*==============================================================================
函数名    :  IsMtNeedAdjustFpsInConfVmp
功能      :  获得终端是否在降帧率的vmp中
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsMtNeedAdjustFpsInConfVmp(TMt &tMt, TExInfoForResFps &tExInfoForFps)
{
	if (tMt.IsNull())
	{
		return FALSE;
	}
	
	if (tMt.GetType() != TYPE_MT)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsMtNeedAdjustResInConfVmp] dstmt.%d is not mt. type.%d manu.%d\n",
			tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return FALSE;
	}
	
    if ( !m_tConf.IsVmpAutoAdapt() )	//当前会议不是画面合成自适应
    {
        return FALSE;					//终端进vmp不降帧率
    }
	
	// 终端逻辑通道是否打开，双流跟随时需判双流通道
	u8 byChnnlType = LOGCHL_VIDEO;
	u8 byMemberType = tExInfoForFps.m_byMemberType;
	if (VMP_MEMBERTYPE_DSTREAM == byMemberType)
	{
		byChnnlType = LOGCHL_SECVIDEO;
	}

	TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMt );
	TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( tMt ) ||
		!m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) )
    {
        return FALSE;
    }
	
	u8 byMtStandardFps = tLogicChannel.GetChanVidFPS(); // 获得终端原始帧率
	if (tLogicChannel.GetChannelType() != MEDIA_TYPE_H264 || byMtStandardFps <= 30)
	{
        return FALSE;
	}

	BOOL32 bIsNeedAdjFps = FALSE;		//是否要调整帧率
	
	// 判断是否需要预判指定VmpParam
	u8 bySpecVmpId = 0;
	if (IsValidVmpId(tExInfoForFps.m_byEqpid))
	{
		bySpecVmpId = tExInfoForFps.m_byEqpid;
	}

	u8 byVmpId;
	TEqp tVmpEqp;
	TVMPParam_25Mem tConfVmpParam;
	u8 byTmpChnlCount = 0;
	u8 byTmpDSChl = 0;
	u8 byVmpSubType;
	for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			continue;
		}
		byVmpId = m_abyVmpEqpId[byIdx];
		byVmpSubType = GetVmpSubType(byVmpId);
		tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
		//终端在mpu/8kg/8kh时需要降帧,8ki先按全不降帧处理，测试后再调整
		if (MPU_SVMP == byVmpSubType || VMP_8KE == byVmpSubType || VMP_8KH == byVmpSubType /*|| VMP_8KI == byVmpSubType*/)
		{
			// 若指定用临时vmpParam，则用临时VmpParam
			if (bySpecVmpId == byVmpId)
			{
				tConfVmpParam = tExInfoForFps.m_tVmpParam;
			}
			else
			{
				tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			}

			// 双流跟随不能用GetChlOfMtInVmpParam方法
			if (VMP_MEMBERTYPE_DSTREAM == byMemberType)
			{
				byTmpDSChl = tConfVmpParam.GetChlOfMemberType(byMemberType);
				if (byTmpDSChl < MAXNUM_VMP_MEMBER)
				{
					byTmpChnlCount = 1;
				}
			}
			else
			{
				GetChlOfMtInVmpParam(tConfVmpParam, tMt, byTmpChnlCount);
			}

			// 存在
			if (byTmpChnlCount > 0)
			{
				/* 针对8kivmp做特殊处理，在前适配的成员不需要降帧
				if (VMP_8KI == byVmpSubType && VMP_MEMBERTYPE_DSTREAM != byMemberType)
				{
					// 该记录为不含双流跟随通道个数
					if (g_cMcuVcApp.GetVmpAdpChnlCountByMt(tVmpEqp, tMt) > 0)
					{
						continue;
					}
				}*/

				// 不明细区分外设，MPU2以外的外设都认为要降帧率
				bIsNeedAdjFps = TRUE;
				break;
			}
		}
	}

	return bIsNeedAdjFps;
}

/*==============================================================================
函数名    :  IsMtNeedAdjustResInConfVmp
功能      :  获得终端在vmp中信息（占用通道数、最小通道分辨率、是否要调整分辨率）
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsMtNeedAdjustResInConfVmp(const TMt &tMt, TExInfoForResFps &tExInfoForRes, u8 &byVmpChlRes, u8 &byChlCount)
{
	if (tMt.IsNull())
	{
		return FALSE;
	}

	if (tMt.GetType() != TYPE_MT)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsMtNeedAdjustResInConfVmp] dstmt.%d is not mt. type.%d manu.%d\n",
			tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return FALSE;
	}
	
    if ( !m_tConf.IsVmpAutoAdapt() )	//当前会议不是画面合成自适应
    {
        return FALSE;					//终端进vmp不降分辨率
    }

	// 终端逻辑通道是否打开，双流跟随时需判双流通道
	u8 byMemberType = tExInfoForRes.m_byMemberType;
	u8 byChnnlType = LOGCHL_VIDEO;
	if (VMP_MEMBERTYPE_DSTREAM == byMemberType)
	{
		// 双流跟随通道不能降分辨率
		byChnnlType = LOGCHL_SECVIDEO;
	}

	TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMt );
	TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( tMt ) ||
		!m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) )
    {
        return FALSE;
    }

	BOOL32 bIsH264Mt = MEDIA_TYPE_H264 == tLogicChannel.GetChannelType();

	BOOL32 bIsNeedAdjust = FALSE;
	u8 byMtStandardFormat = tLogicChannel.GetVideoFormat(); // 获得终端原始分辨率
	byVmpChlRes = byMtStandardFormat;

	// 判断是否需要预判指定VmpParam
	u8 bySpecVmpId = 0;
	if (IsValidVmpId(tExInfoForRes.m_byEqpid))
	{
		bySpecVmpId = tExInfoForRes.m_byEqpid;
	}
	
	u8 byVmpId;
	TEqp tVmpEqp;
	TVMPParam_25Mem tConfVmpParam;
	TVmpChnnlInfo tVmpChnnlInfo;
	u8 byMtInVmpid = 0;	// 若终端只在1个vmp的1个通道内，标记此vmpid，用于前适配占用
	u8 byMtInVmpPos = MAXNUM_VMP_MEMBER; // 若终端只在1个vmp的1个通道内，标记此通道号，用于前适配占用
	u8 byMtInVmpCount = 0;

	u8 abyChnlNo[MAXNUM_VMP_MEMBER]; // 找到tMt所在最小通道的位置,包含统计Mcu及其上传通道终端
	memset(abyChnlNo, 0, sizeof(abyChnlNo));
	u8 byTmpChnlCount = 0;
	u8 byPos = MAXNUM_VMP_MEMBER;
	u8 byReqRes = VIDEO_FORMAT_INVALID;
	u8 byTmpDSChl = 0;
	for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			continue;
		}
		byReqRes = VIDEO_FORMAT_INVALID;
		byVmpId = m_abyVmpEqpId[byIdx];
		tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
		// 若指定用临时vmpParam，则用临时VmpParam
		if (bySpecVmpId == byVmpId)
		{
			tConfVmpParam = tExInfoForRes.m_tVmpParam;
		}
		else
		{
			tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
		}
		tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
		// 在最大支持不降分辨率风格以内，不做统计，264格式可以跳过前适配充足的合成器
		if (tConfVmpParam.GetMaxMemberNum() <= tVmpChnnlInfo.GetMaxStyleNum() && bIsH264Mt)
		{
			continue;
		}
		
		// 初始化byTmpChnlCount与abyChnlNo
		memset(abyChnlNo, 0, sizeof(abyChnlNo));
		byTmpChnlCount = 0;

		// 双流跟随不能用GetChlOfMtInVmpParam方法
		if (VMP_MEMBERTYPE_DSTREAM == byMemberType)
		{
			byTmpDSChl = tConfVmpParam.GetChlOfMemberType(byMemberType);
			if (byTmpDSChl < MAXNUM_VMP_MEMBER)
			{
				byTmpChnlCount = 1;
				abyChnlNo[0] = byTmpDSChl;
			}
		}
		else
		{
			GetChlOfMtInVmpParam(tConfVmpParam, tMt, byTmpChnlCount, abyChnlNo);
		}

		// 非H264终端必须降至CIF
		if (!bIsH264Mt)
		{
			if (byTmpChnlCount > 0)
			{
				byVmpChlRes = VIDEO_FORMAT_CIF;
				bIsNeedAdjust = TRUE;
				break;
			}
			else
			{
				continue;
			}
		}

		for (u8 byChlIdx=0; byChlIdx<byTmpChnlCount; byChlIdx++)
		{
			// 需要调整时，判是否比存储的分辨率小，小则更新最小分辨率
			if (VidResAdjust(byVmpId, tConfVmpParam.GetVMPStyle(), abyChnlNo[byChlIdx], byVmpChlRes, byReqRes))
			{
				byVmpChlRes = byReqRes;
				bIsNeedAdjust = TRUE;
			}
		}
		if (byTmpChnlCount > 0)
		{
			byMtInVmpCount += byTmpChnlCount; //通道计数
			if (1 == byTmpChnlCount)
			{
				byMtInVmpid = byVmpId;		//记录该vmpid
				byMtInVmpPos = abyChnlNo[0];//记录该vmppos
			}
		}
	}

	// 若总通道数为1，则标记终端所在的vmpid
	if (1 == byMtInVmpCount)
	{
		tExInfoForRes.m_byEqpid = byMtInVmpid;
		tExInfoForRes.m_byPos = byMtInVmpPos;
	}
	// 更新通道数
	byChlCount = byMtInVmpCount;

	return bIsNeedAdjust;
}

/*==============================================================================
函数名    :  IsNeedAdjustMtVidFps
功能      :  判断是否需要调整该终端帧率,若需要调整返回要调整的帧率
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsNeedAdjustMtVidFps(TMt &tMt, TExInfoForResFps &tExInfoForFps)
{
	if (tMt.IsNull())
	{
        return FALSE;
	}
	
	if (tMt.GetType() != TYPE_MT)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFps] dstmt.%d is not mt. type.%d manu.%d\n",
			tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return FALSE;
	}
	
	u8 byEvent = tExInfoForFps.m_byEvent;	//触发方式
	u8 byOccupy = tExInfoForFps.m_byOccupy;	//占用通道/取消占用通道
	BOOL32 bConsiderVmpBrd = tExInfoForFps.m_bConsiderVmpBrd;
	BOOL32 bSeizePromt = tExInfoForFps.m_bSeizePromt; //是否做抢占处理，需要抢占提示
	u8 byEqpid = tExInfoForFps.m_byEqpid;
	u8 byPos = tExInfoForFps.m_byPos;
	BOOL32 bNonKeda = tExInfoForFps.m_bNonKeda;
	u8 byMemberType = tExInfoForFps.m_byMemberType;
	
	// 终端逻辑通道是否打开，双流跟随时需判双流通道
	u8 byChnnlType = LOGCHL_VIDEO;
	if (VMP_MEMBERTYPE_DSTREAM == byMemberType)
	{
		byChnnlType = LOGCHL_SECVIDEO;
	}
	// 画面合成支持电话终端，未开通道但需要设置通道成员Setvmpchnl,支持下级电话终端
	if (byEvent == EvMask_VMP && byOccupy == Occupy_InChannel && LOGCHL_VIDEO == byChnnlType)
	{
		if (IsPhoneMt(tMt))
		{
			tExInfoForFps.m_bResultSetVmpChl = TRUE;
			return FALSE;
		}
	}
	TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMt );
	TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( tMt ) ||
		!m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) )
    {
        return FALSE;
    }
	
	u8 byMtStandardFps = tLogicChannel.GetChanVidFPS(); // 获得终端原始帧率
	BOOL32 bIsNeedAdjFps = FALSE;		//是否要调整帧率
	u8 byNewFps = byMtStandardFps;		//要调整到的帧率
	
	// 只针对H264终端调整帧率，非264不调整帧率,原始帧率小于等于30帧的终端无需降帧
	if (tLogicChannel.GetChannelType() != MEDIA_TYPE_H264 || byNewFps <= 30)
	{
		tExInfoForFps.m_byNewFps = byNewFps;
		tExInfoForFps.m_bResultSetVmpChl = TRUE;
		return FALSE;
	}
	
	BOOL32 bIsMtCanAdjust = FALSE; 
	if (IsCanAdjustMtFps(tMt))
	{
		bIsMtCanAdjust = TRUE;
	}
	
	// 是否在标清电视墙内
	BOOL32 bInTw = IsMtInTvWall(tMt); //终端是否在标清电视墙

	// 是否在hdu-vmp中
	BOOL32 bIsInHduVmp = FALSE;


	u8 byMtInHduFps = 0;
	u8 byMediaMode = (byChnnlType == LOGCHL_VIDEO) ? MODE_VIDEO : MODE_SECVIDEO;

	// [2013/04/08 chenbing] 在HDU多画面中的终端才需要判断是否需要调整帧率
	TPeriEqpStatus tHduStatus;
	g_cMcuVcApp.GetPeriEqpStatus(byEqpid, &tHduStatus);
	if (TW_MEMBERTYPE_BATCHPOLL == tHduStatus.m_tStatus.tHdu.GetMemberType(byPos))
	{
		bIsInHduVmp = IsBatchPollMtInHduVmp(tMt,TRUE);
	}
	else
	{
		bIsInHduVmp = IsMtInHduVmp(tMt,0,0,0,FALSE,FALSE,TRUE);
	}
	
	// 是否在需要降帧率的vmp中
	TExInfoForResFps tTmpExInfoForFps = tExInfoForFps;
	BOOL32 bIsNeedAdjustFpsInVmp = IsMtNeedAdjustFpsInConfVmp(tMt, tTmpExInfoForFps);
	
	switch (byOccupy)
	{
	case Occupy_InChannel:
		{
			// 终端不能调帧率
			if (!bIsMtCanAdjust)
			{
				bIsNeedAdjFps = FALSE;
				if (byEvent == EvMask_VMP)
				{
					if (bIsNeedAdjustFpsInVmp)
					{
						// 需要调整时，不能建交换
						tExInfoForFps.m_bResultSetVmpChl = FALSE;
					}
					else
					{
						// 终端不需要调，可以建交换
						tExInfoForFps.m_bResultSetVmpChl = TRUE;
					}
				}
				else if (byEvent == EvMask_HDUVMP)//hduvmp逻辑
				{
					// 不能建交换
					tExInfoForFps.m_bResultSetVmpChl = FALSE;
				}
				else if (byEvent == EvMask_TVWall && bInTw)
				{
					//tvwall逻辑
					tExInfoForFps.m_bResultSetVmpChl = FALSE;
				}
			}
			else
			{
				tExInfoForFps.m_bResultSetVmpChl = TRUE;//可调帧率的终端，可建交换
				// 需要调帧率
				if (bInTw || bIsInHduVmp || bIsNeedAdjustFpsInVmp
					|| EvMask_HDUVMP == byEvent
					|| tExInfoForFps.m_bIsForceAdjust)
				{
					bIsNeedAdjFps = TRUE;
					byNewFps = byNewFps/2;
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFps] bInTw: %d bIsNeedAdjustFpsInHduVmp: %d bIsNeedAdjustFpsInVmp: %d\n",
							bInTw, bIsInHduVmp, bIsNeedAdjustFpsInVmp);
				}
			}
		}
		break;
	case Occupy_OutChannel:
		{
			if (!bIsMtCanAdjust)
			{
				bIsNeedAdjFps = FALSE;//不能调帧率的终端，不调帧率
			}
			else
			{	
				// 出通道时，终端不在任何外设通道才恢复帧率
				if ((!bInTw && !bIsInHduVmp && !bIsNeedAdjustFpsInVmp) || tExInfoForFps.m_bIsForceAdjust)
				{
					bIsNeedAdjFps = TRUE;//出通道恢复帧率
				}
				else
				{
					// 原因：终端降到标清时帧率会自动减，恢复分辨率时可能会同时恢复帧率，
					// 存在需要恢复分辨率但又要降帧的场景
					bIsNeedAdjFps = TRUE;//出通道仍在别的降帧外设中，重调一次帧率
					byNewFps = byNewFps/2;
				}
			}
		}
		break;
	default:
		{
			// 非进出通道引起的调帧率
			if (bIsMtCanAdjust)
			{
				// 需要调帧率
				if (bInTw || bIsInHduVmp || bIsNeedAdjustFpsInVmp)
				{
					bIsNeedAdjFps = TRUE;
					byNewFps = byNewFps/2;
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFps] bInTw: %d bIsNeedAdjustFpsInHduVmp: %d bIsNeedAdjustFpsInVmp: %d\n",
						bInTw, bIsInHduVmp, bIsNeedAdjustFpsInVmp);
				}
			}
		}
		break;
	}
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFps] dstmt: %d bIsNeedAdjFps: %d byNewFps: %d m_bResultSetVmpChl: %d\n",
		tMt.GetMtId(), bIsNeedAdjFps, byNewFps, tExInfoForFps.m_bResultSetVmpChl);

	tExInfoForFps.m_byNewFps = byNewFps;
	return bIsNeedAdjFps;
}

/*==============================================================================
函数名    :  IsNeedAdjustMtVidFormat
功能      :  判断是否需要调整该终端分辨率,若需要调整返回要调整的分辨率
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::IsNeedAdjustMtVidFormat(const TMt &tMt, TExInfoForResFps &tExInfoForRes)
{
	if (tMt.IsNull())
	{
        return FALSE;
	}

	if (tMt.GetType() != TYPE_MT)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] dstmt.%d is not mt. type.%d manu.%d\n",
			tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return FALSE;
	}
	
	u8 byEvent = tExInfoForRes.m_byEvent;	//触发方式
	u8 byOccupy = tExInfoForRes.m_byOccupy;	//占用通道/取消占用通道
	BOOL32 bConsiderVmpBrd = tExInfoForRes.m_bConsiderVmpBrd;
	BOOL32 bSeizePromt = tExInfoForRes.m_bSeizePromt; //是否做抢占处理，需要抢占提示
	u8 byEqpid = tExInfoForRes.m_byEqpid;
	u8 byPos = tExInfoForRes.m_byPos;
	BOOL32 bNonKeda = tExInfoForRes.m_bNonKeda;
	u8 byMemberType = tExInfoForRes.m_byMemberType;

	// 终端逻辑通道是否打开，双流跟随时需判双流通道
	u8 byChnnlType = LOGCHL_VIDEO;
	if (VMP_MEMBERTYPE_DSTREAM == byMemberType)
	{
		byChnnlType = LOGCHL_SECVIDEO;
	}
	// 画面合成支持电话终端，未开通道但需要设置通道成员Setvmpchnl,支持下级电话终端
	if (byEvent == EvMask_VMP && byOccupy == Occupy_InChannel && LOGCHL_VIDEO == byChnnlType)
	{
		if (IsPhoneMt(tMt))
		{
			tExInfoForRes.m_bResultSetVmpChl = TRUE;
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] dstmt.%d is Phone mt.\n", tMt.GetMtId());
			return FALSE;
		}
	}
	TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMt );
	TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( tMt ) ||
		!m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) )
    {
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] Open LogicChannel Error!\n");
		return FALSE;
    }

	u8 byMtStandardFormat = tLogicChannel.GetVideoFormat(); // 获得终端原始分辨率
	BOOL32 bIsNeedAdjRes = FALSE;		//是否要调整分辨率
	u8 byNewRes = byMtStandardFormat;	//要调整到的分辨率

	// 1.获得终端在各外设中情况
	BOOL32 bInTw = IsMtInTvWall(tMt);	//终端是否在标清电视墙
	BOOL32 bInHduVmp = FALSE;			//终端是否在hdu-vmp中，hduvmp提供接口，终端在hdu-vmp中的分辨率。
	u8 byMinResInHduVmp = VIDEO_FORMAT_INVALID;	//获得终端在hdu-vmp中的最小通道分辨率
	u8 byMinFpsInHduVmp = 0;
	
	TPeriEqpStatus tHduStatus;
	g_cMcuVcApp.GetPeriEqpStatus(byEqpid, &tHduStatus);
	if (TW_MEMBERTYPE_BATCHPOLL == tHduStatus.m_tStatus.tHdu.GetMemberType(byPos))
	{
		bInHduVmp = IsBatchPollMtInHduVmp(tMt,TRUE);
	}
	else
	{
		bInHduVmp = IsMtInHduVmp(tMt,0,0,0,FALSE,FALSE,TRUE);
	}
	TSimCapSet tSimCapSet = GetHduChnSupportCap(tMt, tExInfoForRes.m_byEqpid);
	byMinResInHduVmp = tSimCapSet.GetVideoResolution();

	u8 byMinResInVmp = VIDEO_FORMAT_INVALID;	//获得终端在vmp中的最小通道分辨率
	u8 byInVmpCount = 0;						//获得终端在vmp的通道个数，不统计支持全部不降分辨率进vmp的外设
	TExInfoForResFps tTmpExInfoForRes = tExInfoForRes;
	BOOL32 bNeedAdjResInVmp = IsMtNeedAdjustResInConfVmp(tMt, tTmpExInfoForRes, byMinResInVmp, byInVmpCount);
	// 只有一个通道时，会返回该通道所在的合成器VmpId，用于拖发言人选看时占前适配处理
	u8 byMtInVmpid = 0;
	u8 byMtInVmpPos = MAXNUM_VMP_MEMBER;
	TVmpChnnlInfo tVmpChnnlInfo;
	BOOL32 bIsInVmpHdAdpChl = FALSE;//该终端是否在vmp前适配通道中，在vmp前适配通道的终端分辨率不受vmp影响
	if (bNeedAdjResInVmp && 1 == byInVmpCount)
	{
		if (IsValidVmpId(tTmpExInfoForRes.m_byEqpid))
		{
			byMtInVmpid = tTmpExInfoForRes.m_byEqpid;
			byMtInVmpPos = tTmpExInfoForRes.m_byPos;
			// 记录该终端是否在vmp当前前适配中，用于接下来进入非vmp通道的判断
			if (byEvent != EvMask_VMP)
			{
				TEqp tTmpEqp = g_cMcuVcApp.GetEqp(byMtInVmpid);
				tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tTmpEqp);
				TChnnlMemberInfo tVmpChlInfo;
				tVmpChnnlInfo.GetHdChnnlInfoByPos(byMtInVmpPos, &tVmpChlInfo);
				if (!tVmpChlInfo.IsAttrNull())
				{
					bIsInVmpHdAdpChl = TRUE;
				}
			}
		}
	}
	// 不在所有外设中，调分辨率的bStart应更新为FALSE，表示恢复分辨率
	if (!bInTw && !bInHduVmp && !bNeedAdjResInVmp)
	{
		tExInfoForRes.m_bResultStart = FALSE;
	}
	else
	{
		tExInfoForRes.m_bResultStart = TRUE;
	}
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] byEvent(%d) byOccupy(%d)!\n", byEvent, byOccupy);
	// 2.判断是否要调整终端分辨率
	BOOL32 bUpdateVmpAdpChnnl = FALSE;	//后续处理，是否调整前适配
	switch (byEvent)
	{
	case EvMask_TVWall:
		{
			//标清电视墙处理
			if (byOccupy == Occupy_InChannel)//进墙逻辑
			{
				// cif终端无需调整分辨率,非cif终端需要调整到cif
				if (VIDEO_FORMAT_CIF != byMtStandardFormat)
				{
					bIsNeedAdjRes = TRUE;
					byNewRes = VIDEO_FORMAT_CIF;
				}
			} 
			else if (byOccupy == Occupy_OutChannel)//出墙逻辑
			{
				// cif终端不调,仍在其它TW中不调
				if (VIDEO_FORMAT_CIF == byMtStandardFormat || bInTw)
				{
					bIsNeedAdjRes = FALSE;
					byNewRes = VIDEO_FORMAT_CIF;
				}
				else if (!bInHduVmp && !bNeedAdjResInVmp)//不在hdu通道,vmp通道无需调，恢复终端原始分辨率
				{
					bIsNeedAdjRes = TRUE;
					byNewRes = byMtStandardFormat;
				}
				else if (!bNeedAdjResInVmp)//终端仅在hdu-vmp中处理
				{
					bIsNeedAdjRes = TRUE;		//需调整
					byNewRes = byMinResInHduVmp;//分辨率根据HDU-vmp通道来
				} 
				else if (!bInHduVmp)//终端仅在vmp中处理
				{
					bIsNeedAdjRes = TRUE;		//需调整
					byNewRes = byMinResInVmp;	//分辨率根据vmp通道来
					bUpdateVmpAdpChnnl = TRUE;	//影响前适配
					bSeizePromt &= FALSE;		//不做抢占
				}
				else// 同时在hdu-vmp和vmp中
				{
					if (byMinResInHduVmp == byMtStandardFormat) //hdu-vmp不调整终端分辨率，单纯依赖vmp
					{
						bIsNeedAdjRes = TRUE;		//需调整
						byNewRes = byMinResInVmp;	//分辨率根据vmp通道来
						bUpdateVmpAdpChnnl = TRUE;	//影响前适配
						bSeizePromt &= FALSE;		//不做抢占
					}
					else //hdu-vmp会调整vmp，目前为1080终端
					{
						// 因hdu无前适配，将终端原始分辨率更新为hdu需要的的分辨率，再供vmp来调整
						byMtStandardFormat = byMinResInHduVmp;
						bIsNeedAdjRes = TRUE;		//需调整
						byNewRes = byMinResInVmp;	//分辨率根据vmp通道来
						bUpdateVmpAdpChnnl = TRUE;	//影响前适配
						bSeizePromt &= FALSE;		//不做抢占
					}
				}
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat]Event[%d], Occupy[%d] is wrong!\n",byEvent, byOccupy);
			}
		}
		break;
	case EvMask_HDUVMP:
		{
			TMt tMtTemp = tMt;
			//hdu-vmp处理
			if (byOccupy == Occupy_InChannel)//进通道逻辑
			{
				// 获取MPEG-4 AUTO下的分辨率
				if ( VIDEO_FORMAT_AUTO == byMtStandardFormat )
				{
					byMtStandardFormat = GetAutoResByBitrate(VIDEO_FORMAT_AUTO, m_tConf.GetBitRate());
				}

				//本级不能调分辨率的逻辑处理
				if ( !IsCanAdjustMtRes(tMtTemp) )
				{
					if (IsSrcResThanDst(byMtStandardFormat, byMinResInHduVmp))
					{
						byNewRes = byMtStandardFormat;
						//不能建交换
						tExInfoForRes.m_bResultSetVmpChl = FALSE;
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] Don't adjust SrcRes byNewRes(%d)\n", byNewRes);
						return FALSE;			//无需调整
					}
					else
					{
						byNewRes = byMtStandardFormat;
						//可以建交换
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] Don't Need adjust SrcRes byNewRes(%d)!\n", byNewRes);
						return FALSE;		   //无需调整
					}
				}

				// 下级Mcu上传通道成员为空,不调分辨率 
				if (tMt.IsLocal())
				{
					 if ( MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()))
					 {
						tMtTemp = GetSMcuViewMt(tMt, TRUE);
						if ( tMtTemp.IsNull() )
						{
							tExInfoForRes.m_bResultSetVmpChl = TRUE;
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] tMt.IsLocal SMcuViewMt is null!\n");
							return FALSE;					 //不需调整
						}
					 }
				}
 				else
 				{
					TMt tTemp = tMt;
					TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tTemp.GetMcuIdx());
					if (NULL != ptConfMcInfo && MT_TYPE_SMCU == ptConfMcInfo->GetMtExt(tTemp)->GetMtType())
 					{
						tMtTemp = GetSMcuViewMt(tTemp, TRUE);
						if ( tMtTemp.IsNull() )
						{
							tExInfoForRes.m_bResultSetVmpChl = TRUE;
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] OtherSMcu SMcuViewMt is null!\n");
							return FALSE;					 //不需调整
						}
 					}
 				}

				if (bInTw)//TvWall中
				{
					bIsNeedAdjRes = FALSE;
					byNewRes = VIDEO_FORMAT_CIF;
					tExInfoForRes.m_bResultSetVmpChl = TRUE;
				}
				else if (bNeedAdjResInVmp)//在vmp中
				{
					if ( IsSrcResThanDst(byMinResInVmp, byMinResInHduVmp) )
					{
						bIsNeedAdjRes = TRUE;//需调整
						byNewRes = byMinResInHduVmp;//分辨率取小
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
					else if( bIsInVmpHdAdpChl && IsSrcResThanDst(byMtStandardFormat, byMinResInHduVmp) )
					{
						bIsNeedAdjRes = TRUE;		//在VMP前适配需调整
						byNewRes = byMinResInHduVmp;
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
					else
					{
						bIsNeedAdjRes = FALSE;		//无需调整
						byNewRes = byMinResInVmp;
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
				}
				else if (bInHduVmp)		  // 已经在HDU2多画面中
				{
					//强制调整分辨率
					if (tExInfoForRes.m_bIsForceAdjust)
					{
						bIsNeedAdjRes = TRUE;//需调整
						byNewRes = byMinResInHduVmp;
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
					else
					{
						bIsNeedAdjRes = FALSE;		  //无需调整
						byNewRes = byMinResInHduVmp;
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
				}
				else// 不在TvWall和vmp,HDU2多画面中中
				{
					if ( IsSrcResThanDst(byMtStandardFormat, byMinResInHduVmp) )
					{
						bIsNeedAdjRes = TRUE;//需调整
						byNewRes = byMinResInHduVmp;//分辨率取小
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
					else
					{
						bIsNeedAdjRes = FALSE;		  //无需调整
						byNewRes = byMtStandardFormat;
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
				}

				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[Occupy_InChannel]bInTw[%d], bNeedAdjResInVmp[%d]\n", bInTw, bNeedAdjResInVmp);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[Occupy_InChannel]byMinResInVmp[%d] byNewRes[%d]\n", byMinResInVmp, byNewRes);
			} 
			else if (byOccupy == Occupy_OutChannel)//出通道逻辑
			{
				if (bInTw)		//无需恢复
				{
					bIsNeedAdjRes = FALSE;
					byNewRes = VIDEO_FORMAT_CIF;
				}
				else if (bNeedAdjResInVmp)//在vmp中
				{
					//VMP中的分辨率大于HDU多画面分辨率
					if ( IsSrcResThanDst(byMinResInVmp, byMinResInHduVmp) )
					{
						bIsNeedAdjRes = TRUE;//需恢复
						byNewRes = byMinResInVmp;//分辨率取大
					}
					else if( bIsInVmpHdAdpChl && IsSrcResThanDst(byMtStandardFormat, byMinResInHduVmp) )
					{
						bIsNeedAdjRes = TRUE;		//在VMP前适配需调整
						byNewRes = byMtStandardFormat;
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
					}
					else
					{
						bIsNeedAdjRes = FALSE;		//无需恢复
						byNewRes = byMinResInVmp;
					}
				}
				else if (bInHduVmp)//仍在HDU2其他多画面中
				{
					//强制恢复逻辑通道能力
					if (tExInfoForRes.m_bIsForceAdjust)
					{
						bIsNeedAdjRes = TRUE;//需调整
						byNewRes = byMtStandardFormat;//分辨率取大
					}	
					else
					{
						bIsNeedAdjRes = FALSE;		//无需恢复
						byNewRes = byMinResInHduVmp;
					}
				}
				else// 不在TvWall和vmp中
				{
					bIsNeedAdjRes = TRUE;//需调整
					byNewRes = byMtStandardFormat;//分辨率取大
				}
				tExInfoForRes.m_bResultSetVmpChl = TRUE;
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[Occupy_OutChannel]bInTw[%d], bNeedAdjResInVmp[%d]\n", bInTw, bNeedAdjResInVmp);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[Occupy_OutChannel]byMinResInVmp[%d] byNewRes[%d]\n", byMinResInVmp, byNewRes);
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "byOccupy Error!!!\n");
			}
		}
		break;
	case EvMask_VMP:
		{
			//vmp处理
			if (byOccupy == Occupy_InChannel)//进通道逻辑
			{
				BOOL32 bChangeVmpStyle = FALSE;
				TChnnlMemberInfo tChnnlMemInfo = IsVipMemForVmp(tMt, tExInfoForRes);//忽略不能降分辨率的外厂商终端
				if (IsValidVmpId(byEqpid) && (tChnnlMemInfo.IsAttrNull() || tChnnlMemInfo.IsCanAdjResMt()))
				{
					TEqp tTmpVmpEqp = g_cMcuVcApp.GetEqp(byEqpid);
					TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tTmpVmpEqp);
					TVMPParam_25Mem tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tTmpVmpEqp);
					if (tLastVmpParam.GetVMPMode() != CONF_VMPMODE_NONE &&
						tConfVmpParam.GetVMPStyle() != tLastVmpParam.GetVMPStyle())
					{
						bChangeVmpStyle = TRUE;
					}
					
				}
				if (bInTw)//在TW中不调
				{
					bIsNeedAdjRes = FALSE;
					byNewRes = VIDEO_FORMAT_CIF;
					tExInfoForRes.m_bResultSetVmpChl = TRUE;//需要建交换
				}
				else if (!bInHduVmp || byMinResInHduVmp == byMtStandardFormat)//不在hduvmp中或hduvmp不影响终端分辨率
				{
					// 只在vmp中，且无需调分辨率，直接返回
					if (!bNeedAdjResInVmp)
					{
						if (bChangeVmpStyle)
						{
							bIsNeedAdjRes = TRUE;		//风格切换时，由大风格切到不降分辨率风格时，需调整终端分辨率
						}
						else
						{
							bIsNeedAdjRes = FALSE;		//无需调整
						}
						byNewRes = byMinResInVmp;	//分辨率根据vmp通道来
						bUpdateVmpAdpChnnl = FALSE;	//不影响前适配
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
						tExInfoForRes.m_bResultStart = TRUE;
					}
					else{
						bIsNeedAdjRes = TRUE;		//需调整
						byNewRes = byMinResInVmp;	//分辨率根据vmp通道来
						bUpdateVmpAdpChnnl = TRUE;	//影响前适配
						bSeizePromt &= TRUE;		//抢占,若传入参数为不抢占，则不抢占
					}
				} 
				else// 同时在hdu-vmp和vmp中,hdu-vmp影响终端分辨率
				{
					// 在vmp中，但无需调分辨率，vmp通道比较大
					if (!bNeedAdjResInVmp)
					{
						if (bChangeVmpStyle)
						{
							bIsNeedAdjRes = TRUE;		//风格切换时，由大风格切到不降分辨率风格时，需调整终端分辨率
						}
						else
						{
							bIsNeedAdjRes = FALSE;		//无需调整
						}
						byNewRes = byMinResInHduVmp;	//分辨率为hdu调过的分辨率
						bUpdateVmpAdpChnnl = FALSE;	//不影响前适配
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
						tExInfoForRes.m_bResultStart = TRUE;
					}
					else{
						// 因hdu无前适配，将终端原始分辨率更新为hdu需要的的分辨率，再供vmp来调整
						byMtStandardFormat = byMinResInHduVmp;
						bIsNeedAdjRes = TRUE;		//需调整
						byNewRes = byMinResInVmp;	//分辨率根据vmp通道来
						bUpdateVmpAdpChnnl = TRUE;	//影响前适配
						bSeizePromt &= TRUE;		//抢占,若传入参数为不抢占，则不抢占
					}
				}
			} 
			else if (byOccupy == Occupy_OutChannel)//出通道逻辑
			{
				if (bInTw)//在TW中不调
				{
					bIsNeedAdjRes = FALSE;
					byNewRes = VIDEO_FORMAT_CIF;
				}
				else if (!bInHduVmp || byMinResInHduVmp == byMtStandardFormat)//不在hduvmp中或hduvmp不影响终端分辨率
				{
					if (!bNeedAdjResInVmp)//vmp通道无需调,恢复终端分辨率
					{
						bIsNeedAdjRes = TRUE;
						byNewRes = byMtStandardFormat;
					}
					else//vmp通道需要调,依赖vmp通道
					{
						bIsNeedAdjRes = TRUE;		//需调整
						byNewRes = byMinResInVmp;	//分辨率根据vmp通道来
						bUpdateVmpAdpChnnl = TRUE;	//影响前适配
						bSeizePromt &= FALSE;		//不做抢占
					}
				}
				else// 同时在hdu-vmp和vmp中,hdu-vmp影响终端分辨率
				{
					if (!bNeedAdjResInVmp)//vmp通道无需调,调整到hdu-vmp需要的分辨率
					{
						bIsNeedAdjRes = TRUE;
						byNewRes = byMinResInHduVmp;
					}
					else//hdu-vmp和vmp互相影响
					{
						// 因hdu无前适配，将终端原始分辨率更新为hdu需要的的分辨率，再供vmp来调整
						byMtStandardFormat = byMinResInHduVmp;
						bIsNeedAdjRes = TRUE;		//需调整
						byNewRes = byMinResInVmp;	//分辨率根据vmp通道来
						bUpdateVmpAdpChnnl = TRUE;	//影响前适配
						bSeizePromt &= FALSE;		//不做抢占
					}
				}
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat]Event[%d], Occupy[%d] is wrong!\n",byEvent, byOccupy);
			}
		}
		break;
	case EvMask_Speaker:
	case EvMask_BeSelected:
	default:
		{
			// 非事件触发导致的调分辨率，影响画面合成
			if (bInTw)//在TW中不调
			{
				bIsNeedAdjRes = FALSE;
				byNewRes = VIDEO_FORMAT_CIF;
			}
			else if (!bInHduVmp || byMinResInHduVmp == byMtStandardFormat)//不在hduvmp中或hduvmp不影响终端分辨率
			{
				if (!bNeedAdjResInVmp)//vmp通道无需调,恢复终端分辨率
				{
					bIsNeedAdjRes = FALSE;
					byNewRes = byMtStandardFormat;
				}
				else//vmp通道需要调,依赖vmp通道
				{
					bIsNeedAdjRes = TRUE;		//需调整
					byNewRes = byMinResInVmp;	//分辨率根据vmp通道来
					bUpdateVmpAdpChnnl = TRUE;	//影响前适配
					/* 终端选看时做抢占
					if (EvMask_BeSelected == byEvent && Occupy_BeSelected == byOccupy)
					{
						bSeizePromt &= TRUE;		//选看抢占
					}
					else
					{
						bSeizePromt &= FALSE;		//不做抢占
					}*/
				}
			}
			else// 同时在hdu-vmp和vmp中,hdu-vmp影响终端分辨率
			{
				if (!bNeedAdjResInVmp)//vmp通道无需调,调整到hdu-vmp需要的分辨率
				{
					bIsNeedAdjRes = FALSE;
					byNewRes = byMinResInHduVmp;
				}
				else//hdu-vmp和vmp互相影响
				{
					// 因hdu无前适配，将终端原始分辨率更新为hdu需要的的分辨率，再供vmp来调整
					byMtStandardFormat = byMinResInHduVmp;
					bIsNeedAdjRes = TRUE;		//需调整
					byNewRes = byMinResInVmp;	//分辨率根据vmp通道来
					bUpdateVmpAdpChnnl = TRUE;	//影响前适配
					bSeizePromt &= FALSE;		//不做抢占
				}
			}
		}
		break;
	}

	// 3.后续处理
	// vmp前适配调整
	TVMPParam_25Mem tConfVmpParam;
	TEqp tVmpEqp;
	BOOL32 bIsInVmpAdpChl = FALSE;
	u8 byReqResInVmp = byNewRes;
	if (bUpdateVmpAdpChnnl)
	{
		//1.判断是否是VIP终端
		TChnnlMemberInfo tChnnlMemInfo = IsVipMemForVmp(tMt, tExInfoForRes);
		
		//2.判断终端是否要占前适配
		BOOL32 bSeizeAdpChl = FALSE;
		//前适配是针对H264格式的，非H264格式不占用vmp前适配
		if (MEDIA_TYPE_H264 != tLogicChannel.GetChannelType())
		{
			// 针对外厂商MPEG4 AUTO同样需要判断是否是4CIF分辨率
			u8 byNoH264Res = byMtStandardFormat;
			if (VIDEO_FORMAT_AUTO == byNoH264Res)
			{
				byNoH264Res = GetAutoResByBitrate(byNoH264Res,m_tConf.GetBitRate());
			}
			//非H264格式进vmp，一律分辨率更新为cif，mpu2也不支持解mpeg4 4cif
			if (byNoH264Res == VIDEO_FORMAT_4CIF && !tChnnlMemInfo.IsAttrNull() && !tChnnlMemInfo.IsCanAdjResMt())
			{
				// 非H264 4cif，且为不可降分辨率vip终端，不能调分辨率，不能建交换
				bIsNeedAdjRes = FALSE;
				tExInfoForRes.m_bResultSetVmpChl = FALSE;
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat] NoneKeda Mt.(%u,%u) which is MPEG4 with 4CIF in format is not support by VMP!\n",
							tMt.GetMcuId(), tMt.GetMtId() );
				return bIsNeedAdjRes;
			}
			bIsNeedAdjRes = TRUE;
			tExInfoForRes.m_byNewFormat = VIDEO_FORMAT_CIF;
		}
		else
		{
			if (tChnnlMemInfo.IsAttrNull())
			{
				//非vip,不占前适配
			}
			else if (tChnnlMemInfo.IsCanAdjResMt())
			{
				// 可降分辨率vip，判占用通道数,仅在1个通道内可占前适配
				if (1 == byInVmpCount)
				{
					bSeizeAdpChl = TRUE;
					// 如果是发言人，统计发言人跟随通道数，若跟随通道大于1，则不做抢占
					// 防止在更新第一个跟随通道时，认为发言人在1个通道内，而抢占前适配，之后又要释放前适配
					if (tChnnlMemInfo.IsAttrSpeaker())
					{
						u8 byVmpSpeakerNum = GetVmpChnnlNumBySpecMemberType(VMP_MEMBERTYPE_SPEAKER);
						u8 byVmpPollNum = GetVmpChnnlNumBySpecMemberType(VMP_MEMBERTYPE_POLL);
						if ( m_tConf.m_tStatus.GetPollMode() != CONF_POLLMODE_NONE)
						{
							// 轮询的计数，根据轮询的媒体模式来区别
							u8 byPollMode = m_tConf.m_tStatus.GetPollInfo()->GetMediaMode();
							if ( byPollMode == MODE_BOTH || byPollMode == MODE_BOTH_BOTH)
							{
								byVmpSpeakerNum += byVmpPollNum;
							}
						}
						if (byVmpSpeakerNum > 1)
						{
							bSeizeAdpChl = FALSE;
						}
					}
				}
				else
				{
					bSeizeAdpChl = FALSE;
				}
			}
			else
			{
				//不可降分辨率vip
				bSeizeAdpChl = TRUE;
			}
		}
		//3.终端占前适配
		if (bSeizeAdpChl)
		{
			BOOL32 bSelectedByRoll = FALSE;		// xliang [5/7/2009] 点名造成的选看。因为时序问题，这里特殊处理下
			if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
				&& ( m_tRollCaller == tMt 
				|| m_tRollCallee == tMt ) )  
			{
				bSelectedByRoll = TRUE;
			}
			
			tVmpEqp = g_cMcuVcApp.GetEqp( byMtInVmpid );
			tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			// 锁定要占前适配的合成器，仅在1个通道时，byMtInVmpid为该终端所在的vmpid，若在多合成器时，需使用正在调整的合成器
			if (!IsValidVmpId(byMtInVmpid) && IsValidVmpId(byEqpid))
			{
				byMtInVmpid = byEqpid;
				byMtInVmpPos = byPos;
				tVmpEqp = g_cMcuVcApp.GetEqp( byMtInVmpid );
				tConfVmpParam = tExInfoForRes.m_tVmpParam;
			}

			// 若未锁定vmp，则不做前适配占用处理
			if (IsValidVmpId(byMtInVmpid) && byMtInVmpPos < MAXNUM_VMP_MEMBER)
			{
				tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
				BOOL32 bUnableAjustResMt = !tChnnlMemInfo.IsCanAdjResMt();
				// 计算通道个数
				u8 byHdChnnlNum = tVmpChnnlInfo.GetHDChnnlNum();
				u8 byRemainChnlNum = tVmpChnnlInfo.GetMaxNumHdChnnl() - byHdChnnlNum; //剩余前适配个数
				u8 byUsedChnlCount = 0; //本通道已占用的前适配个数,可能要替换的终端在前适配中
				// 被替换终端在前适配中,已占用前适配+1
				TChnnlMemberInfo tTmpMemInfo;
				tVmpChnnlInfo.GetHdChnnlInfoByPos(byMtInVmpPos, &tTmpMemInfo);
				if (!tTmpMemInfo.GetMt().IsNull())
				{
					byUsedChnlCount++;
				}
				// 前适配按照通道依次占用，针对外厂商终端进多通道特殊场景，每次按需要占1个前适配统计，直到不能占用为止
				u8 bySeizeCount = 1 - byUsedChnlCount; //要抢占的前适配个数
				LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[GetMtFormatInMpu]MaxHdChnnlNo:%u,HdChnnlNo:%u,RemainChnlNo:%u.\n",
					tVmpChnnlInfo.GetMaxNumHdChnnl(), byHdChnnlNum, byRemainChnlNum);
				LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[GetMtFormatInMpu]Mtid:%u,AllChnlNo:%u,UsedChnlNo:%u.\n",
					tMt.GetMtId(), byInVmpCount, byUsedChnlCount);
				if (byRemainChnlNum >= bySeizeCount)	//有剩余HD前适配通道可用则占之
				{
					//进vmp高清前适配通道，分辨率不用降,可能要调上去
					tVmpChnnlInfo.SetHdChnnlInfo(byMtInVmpPos, &tChnnlMemInfo);
					bIsNeedAdjRes = bUnableAjustResMt ? FALSE : TRUE;
					tExInfoForRes.m_bResultSetVmpChl = TRUE;
					byReqResInVmp = byMtStandardFormat;
					bIsInVmpAdpChl = TRUE;
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]Mt.(%u,%u) -> HD adapter channel, no need to change it's video format.\n",
						tMt.GetMcuId(), tMt.GetMtId() );
				}
				else //前适配抢占相关
				{
					// 额外处理，按能力顺序占用前适配通道，优先保证发言人
					u8 byReplacePos = 0XFF;
					TChnnlMemberInfo tReplaceMemInfo;
					tVmpChnnlInfo.GetReplaceVmpVipChl(tChnnlMemInfo, byReplacePos, tReplaceMemInfo);
					TVMPParam_25Mem tTmpLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
					if (byReplacePos < MAXNUM_VMP_MEMBER && !bSeizePromt)
					{
						// 腾出该通道前适配给优先的vip用
						tVmpChnnlInfo.ClearChnlByVmpPos(byReplacePos);
						tVmpChnnlInfo.SetHdChnnlInfo(byMtInVmpPos, &tChnnlMemInfo);
						bIsNeedAdjRes = bUnableAjustResMt ? FALSE : TRUE;
						tExInfoForRes.m_bResultSetVmpChl = TRUE;
						byReqResInVmp = byMtStandardFormat;
						bIsInVmpAdpChl = TRUE;
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]Mt.(%u,%u) -> HD adapter channel, no need to change it's video format.\n",
							tMt.GetMcuId(), tMt.GetMtId() );
						// 更新前适配信息
						g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
						// 后续处理：被替换的是外厂商终端则需拆交换，是被选看终端则需降分辨率
						if (tReplaceMemInfo.IsCanAdjResMt())
						{
							ChangeMtVideoFormat(tReplaceMemInfo.GetMt());
						}
						else
						{
							u8 byMode = MODE_VIDEO;
							if (tReplaceMemInfo.IsAttrDstream())
							{
								byMode = MODE_SECVIDEO;//双流占前适配，Mode为第二路视频
							}
							StopSwitchToPeriEqp(tVmpEqp.GetEqpId(), byReplacePos, FALSE, byMode);
							// 更新LastVmpParam，保证下次调整有前适配时能正常建交换
							tTmpLastVmpParam.ClearVmpMember(byReplacePos);
							g_cMcuVcApp.SetLastVmpParam(tVmpEqp, tTmpLastVmpParam);
							NotifyMcsAlarmInfo(0, ERR_MCU_UNABLEADJUSTRESMTOUTVMPADPCHL);
							//告诉外设关闭该通道，释放前适配
							{
								u8 byTmp = 0XFF;//此字段保留,因旧mpu有做处理,为兼容,另开一个字节标识关闭通道
								u8 byAdd = 0;//标识关闭通道
								LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]Send MCU_VMP_ADDREMOVECHNNL_CMD To Vmp, byChnnl:%d.\n", byReplacePos);
								CServMsg cTmpMsg;
								cTmpMsg.SetChnIndex(byReplacePos);
								cTmpMsg.SetMsgBody( &byTmp, sizeof(byTmp) );
								cTmpMsg.CatMsgBody( &byAdd, sizeof(byAdd) );
								SendMsgToEqp( tVmpEqp.GetEqpId(), MCU_VMP_ADDREMOVECHNNL_CMD, cTmpMsg);
							}
						}
					}
					// 传参为需要抢占时，才做抢占
					else if (bSeizeAdpChl)
					{
						if (!bSeizePromt)
						{
							//不做抢占时，不抢占
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]bSeizePromt is %d, no seize operation!\n", bSeizePromt);
							if (bUnableAjustResMt)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat] The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is full! \
									Since VMP mode is [AUTO VMP] mode, the Mt will not enter any channel.\n",
									tMt.GetMcuId(), tMt.GetMtId() );
								bIsNeedAdjRes = FALSE;
							}
						}
						else if (tConfVmpParam.IsVMPAuto())
						{
							//自动画面合成不抢占
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]no seize operation in VMP Auto mode!\n");
							if (bUnableAjustResMt)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat] The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is full! \
									Since VMP mode is [AUTO VMP] mode, the Mt will not enter any channel.\n",
									tMt.GetMcuId(), tMt.GetMtId() );
								bIsNeedAdjRes = FALSE;
							}
						}
						else if( CONF_SPEAKERSRC_CHAIR == m_tConf.GetConfAttrb().GetSpeakerSrc() 
							&& HasJoinedChairman() && (m_tConf.GetChairman() == tMt) && !(tMt == m_tConf.GetSpeaker())
							&& HasJoinedSpeaker()
							)
						{
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]Speaker select see chairman mode, in this mode, no seize prompt is required.\n");
							if (bUnableAjustResMt)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is full! \
									Since VMP mode is [SpeakerSrc Chair] mode, the Mt will not enter any channel.\n",
									tMt.GetMcuId(), tMt.GetMtId() );
								bIsNeedAdjRes = FALSE;
							}
						}
						else if (tConfVmpParam.GetVMPStyle() != tTmpLastVmpParam.GetVMPStyle())
						{
							//切换vmp风格时,不抢占
							//如MPU四画面切多画面时,前适配减少,都是vip的话,最后一个不抢占,直接降分辨率.
							//如之前有1个vip终端是降分辨率的,前适配已满,切风格时会全调整,此vip仍按降分辩率处理
							if (bUnableAjustResMt)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is full! \
									Since VMP mode is [Batch Poll] mode, the Mt will not enter any channel.\n",
									tMt.GetMcuId(), tMt.GetMtId() );
								bIsNeedAdjRes = FALSE;
							}
						}
						else if (bSelectedByRoll)
						{
							//点名开启的画面合成不抢占
							if (bUnableAjustResMt)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is full! \
									Since VMP mode is [Batch Poll] mode, the Mt will not enter any channel.\n",
									tMt.GetMcuId(), tMt.GetMtId() );
								bIsNeedAdjRes = FALSE;
							}
						}
						else if (0 == tVmpChnnlInfo.GetMaxNumHdChnnl())
						{
							//前适配通道个数为0时,不抢占
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]no seize operation in 0 vip vmp chnnl!\n");
							//0前适配时，跟随通道可能跟入不可降分辨率终端，不能建交换
							if (bUnableAjustResMt)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is 0!\n",
									tMt.GetMcuId(), tMt.GetMtId() );
								bIsNeedAdjRes = FALSE;
							}
						}
						else if (byInVmpCount > 1 && bUnableAjustResMt)
						{
							//非科达终端因特殊情况占用多通道时,前适配不足,不做抢占
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is full! \
								Since the mt is in %d channels of vmp, the Mt will not enter any channel.\n",
								tMt.GetMcuId(), tMt.GetMtId(), byInVmpCount);
							bIsNeedAdjRes = FALSE;
						}
						/*else if ((tConfVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_SPEAKER) == byMtInVmpPos &&
								  tTmpLastVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_SPEAKER) == byMtInVmpPos)
								 || (tConfVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_POLL) == byMtInVmpPos &&
								  tTmpLastVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_POLL) == byMtInVmpPos))
						{
							//跟随不抢占
							if (bUnableAjustResMt)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]The Mt.(%u,%u) is UnableAjustResMt and VMP VIP channel is 0!\n",
									tMt.GetMcuId(), tMt.GetMtId() );
								bIsNeedAdjRes = FALSE;
							}
						}*/
						else	
						{	
							// 需要抢占提示
							tExInfoForRes.m_bResultSeizePromt = TRUE;
							tExInfoForRes.m_byEqpid = byMtInVmpid; //更新抢占vmpid
							tExInfoForRes.m_byPos = byMtInVmpPos;  //更新抢占通道
							bIsNeedAdjRes = FALSE;
						}
						// 占此通道但不占前适配时,清空此通道前适配信息
						tVmpChnnlInfo.ClearChnlByVmpPos(byMtInVmpPos);
						if (!bIsNeedAdjRes)
						{
							tExInfoForRes.m_bResultSetVmpChl = FALSE;
						}
						else
						{
							tExInfoForRes.m_bResultSetVmpChl = TRUE;
						}
					}
				}
				g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
			}
			else
			{
				// 外厂商终端进了多通道，又非进vmp通道触发
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[IsNeedAdjustMtVidFormat]Mt.(%u,%u) bSeizeAdpChl:%d, byInVmpCount:%d.\n",
					tMt.GetMcuId(), tMt.GetMtId(), bSeizeAdpChl, byInVmpCount);
				bIsNeedAdjRes = FALSE;
				tExInfoForRes.m_bResultSetVmpChl = FALSE;
			}
		}
		else
		{
			//无需占前适配
			ClearAdpChlInfoInAllVmp(tMt);
			tExInfoForRes.m_bResultSetVmpChl = TRUE;
			/*if (IsValidVmpId(byMtInVmpid))
			{	
				tVmpEqp = g_cMcuVcApp.GetEqp( byMtInVmpid );
				tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
				if (!tChnnlMemInfo.IsAttrDstream())
				{
					tVmpChnnlInfo.ClearChnlByMt(tMt);
				}
				if (byMtInVmpPos < MAXNUM_VMP_MEMBER)
				{
					tVmpChnnlInfo.ClearChnlByVmpPos(byMtInVmpPos);
				}
				g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
			}*/
		}
		
		//res compare print
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "--------【Mt.(%u,%u) (Channel.%u): Resolution compare】------\n",tMt.GetMcuId(), tMt.GetMtId(), byPos);
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "Original Res: \t%s\n",GetResStr(byMtStandardFormat));
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "Require Res: \t%s\n",GetResStr(byNewRes));
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "======================================================\n");
		if(bNeedAdjResInVmp && bIsInVmpAdpChl )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "Need Adjust: \t0 (Enter HD adapter channel)\n");
			byNewRes = byReqResInVmp;
		}
		else if (!bNeedAdjResInVmp)
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "Need Adjust: \t0 \n");
		}
		else
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "Need Adjust: \t1 \n");
		}
	}

	// 赋值要调整到的分辨率
    tExInfoForRes.m_byNewFormat = byNewRes;
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[IsNeedAdjustMtVidFormat] dstmt: %d bIsNeedAdjRes: %d byNewRes: %d bResultSetVmpChl: %d bSeizePromt:%d bStart:%d\n",
		tMt.GetMtId(), bIsNeedAdjRes, byNewRes, tExInfoForRes.m_bResultSetVmpChl, tExInfoForRes.m_bResultSeizePromt, tExInfoForRes.m_bResultStart);

	return bIsNeedAdjRes;
}

/*==============================================================================
函数名    :  GetVmpParamFormBuf
功能      :  从消息内容中获得tVmpParam信息
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::GetVmpParamFormBuf(u8 *pbyBuf, u16 wBufLen, TVMPParam_25Mem &tVmpParam)
{
	// 传入参数长度不能小于基本20成员信息
	if (NULL == pbyBuf || wBufLen < sizeof(TVMPParam))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[GetVmpParamFormBuf]Buf is null or Buf length(%d) is not correct.\n", wBufLen);
		return FALSE;
	}
	
	// 定义临时指针，取出合成风格
	u8 byMaxMemberNum = 0;
	u8 *pbyTmpBuf = pbyBuf;//m_byVMPAuto
	tVmpParam.SetVMPAuto(*pbyTmpBuf);
	pbyTmpBuf++;//m_byVMPBrdst
	tVmpParam.SetVMPBrdst(*pbyTmpBuf);
	pbyTmpBuf++;//m_byVMPStyle
	tVmpParam.SetVMPStyle(*pbyTmpBuf);
	byMaxMemberNum = tVmpParam.GetMaxMemberNum();
	pbyTmpBuf++;//m_byVMPSchemeId
	tVmpParam.SetVMPSchemeId(*pbyTmpBuf);
	pbyTmpBuf++;//m_byVMPMode
	tVmpParam.SetVMPMode(*pbyTmpBuf);
	pbyTmpBuf++;//m_byRimEnabled
	tVmpParam.SetIsRimEnabled(*pbyTmpBuf);
	pbyTmpBuf++;//m_byVMPBatchPoll
	tVmpParam.SetVMPBatchPoll(*pbyTmpBuf);
	pbyTmpBuf++;//m_byVMPSeeByChairman
	tVmpParam.SetVMPSeeByChairman(*pbyTmpBuf);
	pbyTmpBuf++;//m_atVMPMember
	for (u8 byIdx=0; byIdx<byMaxMemberNum && byIdx<MAXNUM_MPU2VMP_MEMBER; byIdx++)
	{
		tVmpParam.SetVmpMember(byIdx, *(TVMPMember*)pbyTmpBuf);
		pbyTmpBuf += sizeof(TVMPMember);
	}
	return TRUE;
}

/*==============================================================================
函数名    :  ProcStartVmpReq
功能      :  处理改变画面合成参数请求
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-08						薛亮							
==============================================================================*/
void CMcuVcInst::ProcStartVmpReq(CServMsg &cServMsg)
{
	TVMPParam_25Mem	tVmpParam;// = *( TVMPParam* )cServMsg.GetMsgBody();
	GetVmpParamFormBuf(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen(), tVmpParam);
	/*u8 byIsVmpModule = 0;
	if (cServMsg.GetMsgBodyLen() > sizeof(TVMPParam))
	{
		byIsVmpModule = *(cServMsg.GetMsgBody() + sizeof(TVMPParam));
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP,"[ProcStartVmpReq]Start vmp by module, IsVmpModule[%d]!\n", byIsVmpModule); 
	}*/
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	u8 byVmpCount = GetVmpCountInVmpList();
	BOOL32 bIsVmpModule = FALSE;//是否是会议模版开启的画面合成,有模版，未占用合成器时，开启为模版开启
	if (tConfAttrb.IsHasVmpModule() && !m_tConfInStatus.IsVmpModuleUsed() && 0 == byVmpCount)
	{
		bIsVmpModule = TRUE;
	}

	if( MT_MCU_STARTVMP_REQ == cServMsg.GetEventId() )
	{		
		// 终端开启画面合成前提条件为当前合成个数为0
		if (byVmpCount > 0)
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "ChairmanMt start vmp, vmp count:%d, nack!\n" , byVmpCount);
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );					
			return;
		}
		// 对终控台发来的合成信息进行格式化
		FormatVmpParamFromMTCMsg(tVmpParam);
	}

	u8 byVmpId		= cServMsg.GetEqpId();

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "start vmp(Id:%d) param:\n" , byVmpId);
	tVmpParam.Print();

	u16 wError = 0;
	if (!CheckStartVmpReq(byVmpId, tVmpParam, bIsVmpModule, cServMsg, wError))
	{
		cServMsg.SetErrorCode( wError );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(byVmpId, &tPeriEqpStatus);
	if (bIsVmpModule)
	{
		m_tModuleVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
		m_tModuleVmpEqp.SetConfIdx( m_byConfIdx );
	}
	AddVmpIdIntoVmpList(byVmpId);
	tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::RESERVE;//先占用,超时后未成功再放弃
	tPeriEqpStatus.SetConfIdx( m_byConfIdx );
	//m_tConf.m_tStatus.SetVmpStyle( tVmpParam.GetVMPStyle() );
	//开启画面合成时,初始化vmp单通道轮询状态
	TMtPollParam tNullPollParam;
	tNullPollParam.SetNull();
	m_tVmpPollParam.SetPollState(POLL_STATE_NONE);
	m_tVmpPollParam.SetMtPollParam(tNullPollParam);
	tVmpParam.SetVMPSeeByChairman(FALSE);	//开启时,初始化主席选看vmp状态,防止主席终端开启vmp时,发过来错误值
	
	//ACK
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );	
	
	
	//开启画面合成器前，对界面配置的转发板进行判断，若不在线，自动更换新的
	g_cMcuVcApp.ChkAndRefreshMpForEqp(byVmpId);
	
	/**************************以下为不回nack的过滤***********************/

	tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);	//tVmpParam最终给PeriEqpStatus
	g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );

	m_byVmpOperating = 1;	//标志进入VMP原子操作
	
	//开始画面合成
	AdjustVmpParam(byVmpId, &tVmpParam, TRUE);
}

/*==============================================================================
函数名    :  ProcStopVmpReq
功能      :  处理改变画面合成参数请求
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::ProcStopVmpReq(CServMsg &cServMsg)
{
	TPeriEqpStatus tPeriEqpStatus;
	u8 byVmpId = cServMsg.GetEqpId();
	if (!IsValidVmpId(byVmpId))
	{
		byVmpId = GetTheOnlyVmpIdFromVmpList();
		if (!IsValidVmpId(byVmpId))
		{
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
		}
	}
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	
	KillTimer(MCUVC_WAIT_ALLVMPPRESETIN_ACK_TIMER+byVmpId-VMPID_MIN);
	// 仍在等待画面合成开始回应,手动触发回应超时请求,释放画面合成资源
	if (!tVmpEqp.IsNull() &&
		g_cMcuVcApp.GetPeriEqpStatus(tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
		tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
		(TVmpStatus::RESERVE == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState
		|| TVmpStatus::WAIT_START == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState))
	{
		// 直接触发超时处理
		CMessage cMsg;
		//lukunpeng 2010/06/24 此处必须初始化
		memset(&cMsg, 0, sizeof(cMsg));
		ProcVmpRspWaitTimer(&cMsg);	
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
		return;
	}
	
	//检查当前会议的画面合成状态 
	if( g_cMcuVcApp.GetVMPMode(tVmpEqp) == CONF_VMPMODE_NONE)
	{
		cServMsg.SetErrorCode( ERR_MCU_VMPNOTSTART );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//ACK
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
	//正等待画面合成停止响应,无需再进行释放处理
	if (TVmpStatus::WAIT_STOP == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
	{
		return;
	}
	
	TConfAttrb tConfattrb = m_tConf.GetConfAttrb();
	//停止时模板失效
	if( tConfattrb.IsHasVmpModule() && tVmpEqp.GetEqpId() == m_tModuleVmpEqp.GetEqpId())
	{
		TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
		tConfAttrib.SetHasVmpModule(FALSE);
		m_tConf.SetConfAttrb( tConfAttrib );
	}
	
	if( IsValidVmpId(tVmpEqp.GetEqpId()) && m_byCreateBy == CONF_CREATE_NPLUS)
	{
		g_cMcuVcApp.NplusRemoveVmpModuleEqpId(byVmpId);
	}
	
	//停止vmp单通道轮询
	ProcStopVmpPoll();
	
	u8 byVmpIdx = byVmpId - VMPID_MIN;
	SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER+byVmpIdx, TIMESPACE_WAIT_VMPRSP);
	tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
	g_cMcuVcApp.SetPeriEqpStatus(tVmpEqp.GetEqpId(), &tPeriEqpStatus);
	
	//停止
	g_cEqpSsnApp.SendMsgToPeriEqpSsn( tVmpEqp.GetEqpId(),MCU_VMP_STOPVIDMIX_REQ, 
		(u8*)cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

	return;
}

/*==============================================================================
函数名    :  ProcChangeVmpParamReq
功能      :  处理改变画面合成参数请求
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-1-22					薛亮							create
==============================================================================*/
void CMcuVcInst::ProcChangeVmpParamReq(CServMsg &cServMsg)
{
    TVMPParam_25Mem tVmpParam;
	
	// 从消息体获得合成信息
	GetVmpParamFormBuf(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen(), tVmpParam);
	if( MT_MCU_CHANGEVMPPARAM_REQ == cServMsg.GetEventId() )
	{
		FormatVmpParamFromMTCMsg(tVmpParam);
	}
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "change vmp param:\n");
	tVmpParam.Print();

	// 获得对应的合成器Id
	u8 byVmpId = cServMsg.GetEqpId();
	
	// 调整时各种Check处理
	u16 wError = 0;
	if (!CheckChangeVmpReq(byVmpId, tVmpParam, wError))
	{
		cServMsg.SetErrorCode( wError );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//抢占提示过滤，抢占时不需要回复Nack
	if (!CheckAdpChnnlLmt(byVmpId, tVmpParam, cServMsg, TRUE) )
	{
		return;
	}

	//ACK
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
	// xliang [1/6/2009] 区分新老VMP
	AdjustVmpParam(byVmpId, &tVmpParam);

	return;
}

/*==============================================================================
函数名    :  ProcStartVmpBrdReq
功能      :  开启合成广播请求处理
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::ProcStartVmpBrdReq(CServMsg &cServMsg)
{
    TVMPParam_25Mem tVmpParam;
	TPeriEqpStatus tPeriEqpStatus;
	u8 byVmpSubType = 0;  //VMP子类型
	u8 byLoop = 0;

	u8 byVmpId = cServMsg.GetEqpId();
	if (!IsValidVmpId(byVmpId))
	{
		if (MT_MCU_STARTVMPBRDST_REQ == cServMsg.GetEventId())
		{
			byVmpId = GetTheOnlyVmpIdFromVmpList();
		}
		if (!IsValidVmpId(byVmpId))
		{
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
	}
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	
	//检查当前会议的画面合成状态
	if (CONF_VMPMODE_NONE == g_cMcuVcApp.GetVMPMode(tVmpEqp))
	{
		cServMsg.SetErrorCode( ERR_MCU_VMPNOTSTART );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
	// 有vmp在广播，需更新旧vmp广播模式
	BOOL32 bHasOldVmpBrdst = FALSE;
	if (!m_tVmpEqp.IsNull() && tVmpEqp.GetEqpId() != m_tVmpEqp.GetEqpId())
	{
		bHasOldVmpBrdst = TRUE;
		SetVMPBrdst(m_tVmpEqp, FALSE);
		//刷界面
		TVMPParam_25Mem tOldBrdstVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
		cServMsg.SetEqpId(m_tVmpEqp.GetEqpId());
		cServMsg.SetMsgBody( (u8*)&tOldBrdstVmpParam, sizeof(tOldBrdstVmpParam) );
		SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
	}

	// 更新新vmp广播模式
	SetVMPBrdst(tVmpEqp, TRUE);
	tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	m_tVmpEqp = tVmpEqp;//记录新vmp广播
	
	//开始广播画面合成码流
	ChangeVidBrdSrc(&tVmpEqp);
	//针对mpu,在非广播切广播时,需要重新调整vmp各后向通道码率
	if (GetVmpSubType(tVmpEqp.GetEqpId()) != VMP)
	{
		AdjustVmpBrdBitRate();
	}
	
	// 只有非vmp间广播切换时才需要冲击发言人vip
	if (!bHasOldVmpBrdst)
	{
		//8kh 如果是广播和非广播之间切换，1080p30适配会议vmp广播时需开启1080p30编码
		AdjustVmpOutChnlInChgVmpBrd(tVmpEqp, TRUE);//非广播切为广播
		
		//冲击发言人
		ChgSpeakerResInVmpBrd();
	}

	g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

	//通知会控跟主席
	cServMsg.SetEqpId(tVmpEqp.GetEqpId());
	cServMsg.SetMsgBody( (u8*)&tVmpParam, sizeof(tVmpParam) );
	SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
	SendVmpParamToChairMan();
	
	//FastUpdate to Vmp
	NotifyFastUpdate(tVmpEqp, 0);
	if (MPU_SVMP == byVmpSubType || VMP_8KE == byVmpSubType || VMP_8KH == byVmpSubType)
	{
		for (u8 byTmpIdx=1; byTmpIdx<MAXNUM_MPU_OUTCHNNL; byTmpIdx++)
		{
			NotifyFastUpdate(tVmpEqp, byTmpIdx);
		}
	}// MPU2/8KIVMP时,按照后适配个数请求关键帧
	else if (MPU2_VMP_BASIC == byVmpSubType || MPU2_VMP_ENHACED == byVmpSubType || VMP_8KI == byVmpSubType)
	{
		TKDVVMPOutParam tVMPOutParam = g_cMcuVcApp.GetVMPOutParam(tVmpEqp);
		for (u8 byTmpIdx=1; byTmpIdx<tVMPOutParam.GetVmpOutCount(); byTmpIdx++)
		{
			NotifyFastUpdate(tVmpEqp, byTmpIdx);
		}
	}
	
	return;
}

/*==============================================================================
函数名    :  ProcStopVmpBrdReq
功能      :  停止合成广播请求处理
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::ProcStopVmpBrdReq(CServMsg &cServMsg)
{
    TVMPParam_25Mem tVmpParam;
	TPeriEqpStatus tPeriEqpStatus;
	u8 byVmpSubType = 0;  //VMP子类型
	u8 byLoop = 0;

	//检查当前会议的画面合成状态
	tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	if( tVmpParam.GetVMPMode() == CONF_VMPMODE_NONE )
	{
		cServMsg.SetErrorCode( ERR_MCU_VMPNOTSTART );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//检查画面合成的广播状态
	u8 byVmpId = cServMsg.GetEqpId();//终控台不支持广播非广播切换
	if( !tVmpParam.IsVMPBrdst() || byVmpId != m_tVmpEqp.GetEqpId())
	{
		cServMsg.SetErrorCode( ERR_MCU_VMPNOTBRDST );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	//停止广播画面合成码流
	tVmpParam.SetVMPBrdst(FALSE);
	SetVMPBrdst(m_tVmpEqp, FALSE);				

	if( HasJoinedSpeaker() )
	{
		TMt tTempMt = m_tConf.GetSpeaker();
		if( m_tConf.GetConfAttrb().IsSatDCastMode() &&
			 IsMultiCastMt(tTempMt.GetMtId()) &&
			 GetCurSatMtNum() == 0 && !IsSatMtSend(GetLocalSpeaker()))
		{
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[VmpCommonReq]stopvmpbrd satMtNum(0),so can't recover vidbrdsrc(%d)\n",
				GetCurSatMtNum(),tTempMt.GetMtId());
			ChangeVidBrdSrc( NULL );
		}
		else
		{
			ChangeVidBrdSrc( &tTempMt );
		}
	}
	else
	{
		ChangeVidBrdSrc( NULL );
	}

	//8kh 如果是广播和非广播之间切换，1080p30适配会议vmp非广播时需停1080p30编码
	AdjustVmpOutChnlInChgVmpBrd(m_tVmpEqp, FALSE);//广播切为非广播

	// MPU与MPU2时,需要考虑发言人进前适配,调整恢复发言人分辨率
	if(GetVmpCountInVmpList() > 0)
	{
		TMt tSpeaker = m_tConf.GetSpeaker();
		if( HasJoinedSpeaker() && ChgMtVidFormatRequired(tSpeaker))
		{
			ChangeMtVideoFormat( tSpeaker, FALSE, FALSE, TRUE);
		}
	}
	
	//通知会控与主席
	cServMsg.SetEqpId(m_tVmpEqp.GetEqpId());
	cServMsg.SetMsgBody( (u8*)&tVmpParam, sizeof(tVmpParam) );
	SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
	// 取消vmp广播时，清除m_tVmpEqp
	m_tVmpEqp.SetNull();
	SendVmpParamToChairMan();

	return;
}

/*==============================================================================
函数名    :  AdjustVmpOutChnlInChgVmpBrd
功能      :  8kh8ki合成广播切为非广播时特殊处理
算法实现  :  
参数说明  :  TEqp tVmpEqp [in]
			 BOOL32 bIsVmpBrd [in] 非广播切为广播时置TRUE，广播切为非广播时置FALSE
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::AdjustVmpOutChnlInChgVmpBrd(TEqp tVmpEqp, BOOL32 bStartVmpBrd)
{
	u8 byVmpSubtype = GetVmpSubType(tVmpEqp.GetEqpId());
	BOOL32 bNeedChgVmpOutChl = FALSE;
	
	//8kh 如果是广播和非广播之间切换
	if (VMP_8KH == byVmpSubtype)
	{
		//适配会议
		if (IsConfExcludeDDSUseAdapt())
		{
			//判断主格式，1080p30适配会议vmp非广播时需停1080p30编码
			if (emBpAttrb == m_tConf.GetProfileAttrb() &&
				m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
				m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 &&
				m_tConf.GetMainVidUsrDefFPS() >= 25 &&
				m_tConf.GetMainVidUsrDefFPS() <= 30)
			{
				bNeedChgVmpOutChl = TRUE;
			}
			// 720p30fpsHP适配会议,广播非广播间切换,需要改变vmp输出通道
			if (emHpAttrb == m_tConf.GetProfileAttrb() &&
				m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
				m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD720 &&
				m_tConf.GetMainVidUsrDefFPS() >= 25 &&
				m_tConf.GetMainVidUsrDefFPS() <= 30)
			{
				bNeedChgVmpOutChl = TRUE;
			}
		}
	}
	else if (VMP_8KI == byVmpSubtype)
	{
		//适配会议
		if (IsConfExcludeDDSUseAdapt())
		{
			// 1080P/i 25/30fps适配会议vmp非广播时需停1080p30编码
			if (m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
				m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 &&
				m_tConf.GetMainVidUsrDefFPS() >= 25 &&
				m_tConf.GetMainVidUsrDefFPS() <= 30)
			{
				bNeedChgVmpOutChl = TRUE;
			}
			// 720P 50/60fps适配会议,广播非广播间切换,需要改变vmp输出通道
			if (m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
				m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD720 &&
				m_tConf.GetMainVidUsrDefFPS() > 30)
			{
				bNeedChgVmpOutChl = TRUE;
			}
		}
	}
	
	if (bNeedChgVmpOutChl)
	{
		if (VMP_8KI == byVmpSubtype)
		{
			// 更新第0路编码码率，是否停止编码的标志
			u8 byVmpOutChnnl = 0;
			u16 wMinBitRate = 0;
			if (bStartVmpBrd)// 非广播切广播时获取需要的码率，广播切非广播时码率置0
			{
				wMinBitRate = GetMinMtRcvBitByVmpChn(tVmpEqp.GetEqpId(), TRUE, byVmpOutChnnl);
			}
			g_cMcuVcApp.SetVMPOutChlBitrate(tVmpEqp, byVmpOutChnnl, wMinBitRate);
		}

		// 高清电视墙选看vmp处理
		u8 byHduChnNum = 0;
		TPeriEqpStatus tHduStatus;
		TMt tMtInTv;
		for(u8 byEqpHduId = HDUID_MIN; byEqpHduId <= HDUID_MAX; byEqpHduId++)
		{
			byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byEqpHduId));
			if (0 == byHduChnNum)
			{
				//ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[VmpCommonReq] GetHduChnNumAcd2Eqp2 failed!\n");		
				continue;
			}
			
			if (g_cMcuVcApp.GetPeriEqpStatus(byEqpHduId, &tHduStatus) &&
				tHduStatus.m_byOnline )
			{
				for(u8 byLp = 0; byLp < byHduChnNum; byLp++)
				{
					tMtInTv = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byLp];
					if(tMtInTv.GetConfIdx() == m_byConfIdx)
					{
						// 增加判断终端所在会议号，否则会将其他会议的终端误判
						if(tMtInTv == tVmpEqp)
						{
							// [2013/03/11 chenbing] HDU多画面目前不支持选看VMP,子通道置0
							ChangeHduSwitch(&tVmpEqp, byEqpHduId, byLp, 0, TW_MEMBERTYPE_SWITCHVMP, TW_STATE_START);
						}
					}
				}
			}
		}
		
		// 关闭/打开第0通道编码
		u8 byVmpOutChnnl = 0;
		CServMsg cMsg;
		u8 abyOutChnlActive[MAXNUM_MPU_OUTCHNNL];//输出通道是否Active,EmVmpOutChnlStatus
		memset(abyOutChnlActive,emVmpOutChnlNotChange,sizeof(abyOutChnlActive));
		// 开启vmp广播时，激活通道
		if (bStartVmpBrd)
		{
			abyOutChnlActive[byVmpOutChnnl] = emVmpOutChnlActive;	//激活
		}
		else
		{
			// 停止vmp广播时，关闭通道
			abyOutChnlActive[byVmpOutChnnl] = emVmpOutChnlInactive;	//关闭
		}
		for (u8 byIdx=0; byIdx<sizeof(abyOutChnlActive); byIdx++)
		{
			LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"VMP Outchnl[%d] Active: %d\n", byIdx,abyOutChnlActive[byIdx]);
		}
		cMsg.SetConfId(m_tConf.GetConfId());
		cMsg.SetMsgBody(abyOutChnlActive,sizeof(abyOutChnlActive));
		SendMsgToEqp(tVmpEqp.GetEqpId(), MCU_VMP_STARTSTOPCHNNL_CMD, cMsg);
	}

	return;
}

/*==============================================================================
函数名    :  ChgSpeakerResInVmpBrd
功能      :  vmp广播冲击发言人
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::ChgSpeakerResInVmpBrd()
{
	//冲击发言人
	TEqp tTmpVmpEqp;
	TVmpChnnlInfo tVmpChnnlInfo;
	u8 byHdChnnlNum;
	TChnnlMemberInfo tChnnlMemInfo;
	BOOL32 bChgSpeakerRes = FALSE;
	u8 byLoop = 0;
	BOOL32 bIsAdpFull = FALSE;
	TVMPParam_25Mem tVmpParam;
	for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			continue;
		}
		tTmpVmpEqp = g_cMcuVcApp.GetEqp( m_abyVmpEqpId[byIdx] );
		tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tTmpVmpEqp);
		byHdChnnlNum = tVmpChnnlInfo.GetHDChnnlNum();
		// 前适配满，发言人释放1路前适配时，尝试调整一次vmp成员，让之前被抢的不可降分辨率终端占前适配
		bIsAdpFull = byHdChnnlNum >= tVmpChnnlInfo.GetMaxNumHdChnnl();
		
		for(byLoop = 0; byLoop < byHdChnnlNum; byLoop ++)
		{
			tVmpChnnlInfo.GetHdChnnlInfo(byLoop, &tChnnlMemInfo);
			
			//之前ChangeVidBrdSrc(&m_tVmpEqp)中会调StopSelectSrc(),该函数会将被选看的MT退出前适配通道,所以下面只需对发言VIP做处理
			if( tChnnlMemInfo.IsAttrSpeaker() )
			{
				//调整Mt分辨率
				bChgSpeakerRes = TRUE;
				ChangeMtVideoFormat(tChnnlMemInfo.GetMt());
				break;//可能进多个通道
			}
		}
		if (bChgSpeakerRes)
		{
			if (bIsAdpFull)
			{
				// 被停交换的通道需要重新下参，保证正常进入前适配
				tVmpParam = g_cMcuVcApp.GetConfVmpParam(tTmpVmpEqp);
				AdjustVmpParam(tTmpVmpEqp.GetEqpId(), &tVmpParam, FALSE, FALSE);
			}
			// 释放发言人前适配，自动调整一次前适配，另需要进前适配的终端进前适配
			//AdjustVmpHdChnlInfo(tTmpVmpEqp);

			break;
		}
	}

	return;
}

/*==============================================================================
函数名    :  ForamtVmpParamFromMTCMsg
功能      :  对终控台发来的合成信息进行格式化
算法实现  :  
参数说明  :  TVMPParam_25Mem &tVmpParam in/out
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::FormatVmpParamFromMTCMsg(TVMPParam_25Mem &tVmpParam)
{
	TVMPMember *ptVmpMember = NULL;
	for( u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); ++byLoop )
	{
		ptVmpMember =  tVmpParam.GetVmpMember(byLoop);
		if( ptVmpMember == NULL || ptVmpMember->IsNull() )
		{
			continue;
		}
		if( ptVmpMember->GetMcuId() == 0 && ptVmpMember->GetMtId() == 0 )
		{
			tVmpParam.GetVmpMember(byLoop)->SetMcuId(0);
			tVmpParam.GetVmpMember(byLoop)->SetMtId(0);
		}
		else
		{
			tVmpParam.GetVmpMember(byLoop)->SetMcuId( 
				m_ptMtTable->GetMt( ptVmpMember->GetMtId() ).GetMcuId()
				);
		}
	}

	return;
}

/*==============================================================================
函数名    :  CheckStartVmpReq
功能      :  画面合成开启时的各种Check
算法实现  :  
参数说明  :  u8 &byVmpId in/out
			 TVMPParam_25Mem &tVmpParam in/out
			 BOOL32 bIsVmpModule,
			 CServMsg &cServMsg
			 error code
返回值说明:  BOOL32
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130527					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::CheckStartVmpReq(u8 &byVmpId, TVMPParam_25Mem &tVmpParam, BOOL32 bIsVmpModule, CServMsg &cServMsg, u16 &wError)
{
	wError = 0;
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	TPeriEqpStatus tPeriEqpStatus;
	u8 byVmpCount = GetVmpCountInVmpList();
	// 指定vmpid情况下，若该外设状态不为空闲，则不做开启处理
	if (!tVmpEqp.IsNull())
	{
		if (g_cMcuVcApp.GetPeriEqpStatus(tVmpEqp.GetEqpId(), &tPeriEqpStatus) &&
			TVmpStatus::IDLE != tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
		{
			wError = ERR_MCU_VMPRESTART;
			return FALSE;
		}
	}
	else
	{
		//检查当前会议的画面合成状态,模版开启自动合成时,Mode不为空,需跳过
		if (IsVmpStatusUseStateInVmpList(TVmpStatus::WAIT_START) ||
			IsVmpStatusUseStateInVmpList(TVmpStatus::RESERVE))
		{
			wError = ERR_MCU_VMPRESTART;
			return FALSE;
		}
	}

	if (!FindVmpSupportVmpParam(byVmpId, tVmpParam.GetVMPStyle(), byVmpCount, wError))
	{
		return FALSE;
	}

	// vmp后适配vicp资源占用判断，建后适配列表
	if (!PrepareVmpOutCap(byVmpId, tVmpParam.IsVMPBrdst()))
	{
		TKDVVMPOutParam tVMPOutParam;
		g_cMcuVcApp.SetVMPOutParam(tVmpEqp, tVMPOutParam);
		//模版开启vmp失败后,VMP模板不再生效
		m_tConfInStatus.SetVmpModuleUsed(TRUE);
		wError = ERR_VMP_VICP_NOT_ENOUGH;
		return FALSE;
	}

	BOOL32 bIsVmpCreateByNPlus = FALSE;
	if((m_byCreateBy == CONF_CREATE_NPLUS && g_cMcuVcApp.NPlusIsVmpOccupyByConf(byVmpId,m_byConfIdx)))
	{
		bIsVmpCreateByNPlus = TRUE;
		ConfPrint(LOG_LVL_DETAIL,MID_MCU_NPLUS,"===============N+1 correct byVmpId.%d==========\n",byVmpId);
	}

	//合成参数校验
	// 会议模版开启的画面合成,由于终端上线在后面,不做合成参数校验
	if (!bIsVmpModule && 
		!bIsVmpCreateByNPlus)
	{
		if (!CheckVmpParam(byVmpId, tVmpParam, wError ) )
		{
			return FALSE;
		}
	}

	g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	u8 byVmpDynStyle;
	if (tVmpParam.IsVMPAuto() )
	{
		// [pengjie 2010/3/30] 多回传上来后下级的mcu还是要来合成
		u8 byMtNum =  m_tConfAllMtInfo.GetLocalJoinedMtNum();

		// [7/2/2010 xliang] 区分单回传/多回传, 确定是否要将下级发言终端算进此次合成成员
		// [pengjie 2010/3/22] 这里还要判断发言人是否存在
		TMt tSpeaker = m_tConf.GetSpeaker();
		if( !tSpeaker.IsNull() && !tSpeaker.IsLocal() )
		{
			if( IsLocalAndSMcuSupMultSpy(tSpeaker.GetMcuId()) )
			{
				byMtNum ++;
			}
		}
		byVmpDynStyle = GetVmpDynStyle(byVmpId, byMtNum);
		if (VMP_STYLE_NONE == byVmpDynStyle)
		{
			wError = ERR_INVALID_VMPSTYLE;
			return FALSE;
		}
		else
		{
			// 保存成自动算出的具体合成方式	
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "auto vmp style is confirmed to style:%u\n", byVmpDynStyle); 
			tVmpParam.SetVMPStyle( byVmpDynStyle );
		}
	} // 所有vmp都做CheckMpuMember处理,不区分vmp
	else
	{
		// MPU的通道成员限制（非自动模式下）
		if ( !CheckMpuMember(byVmpId, tVmpParam, wError ) )
		{
			//模版开启vmp失败后,VMP模板不再生效
			m_tConfInStatus.SetVmpModuleUsed(TRUE);
			return FALSE;
		}
	}
	
	// xsl [7/20/2006]卫星分散会议需要检查回传通道数
	if (m_tConf.GetConfAttrb().IsSatDCastMode()) 
	{
		// 卫星分散会议支持多vmp
		if (/*byVmpCount > 0 || */!IsSatMtCanContinue(0,emStartVmp,NULL,0xFF,0xFF,&tVmpParam))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[VmpCommonReq] over max upload mt num. nack!\n");
			//模版开启vmp失败后,VMP模板不再生效
			m_tConfInStatus.SetVmpModuleUsed(TRUE);
			wError = ERR_MCU_DCAST_OVERCHNNLNUM; 
			return FALSE;
		}
	}
	
	//对于有高清前适配通道的VMP，
	//在开始之前过滤vmp param是否有超出VMP前适配通道能力,超出的踢出成员，但整体不拒（自动画面合成除外）
	//VIP身份中，发言人优先级最高。
	//对于8000H有可能会出现前适配通道为0的情况，此时如果就拖一个需要前适配的终端进画面合成，画面合成不应该开启
	if (!CheckAdpChnnlLmt(byVmpId, tVmpParam, cServMsg))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[VmpCommonReq] Vip vmp Chnnl is not enough. nack!\n");
		wError = ERR_VMP_NO_VIP_VMPCHNNL; 
		return FALSE;
	}
	
	return TRUE;
}


/*==============================================================================
函数名    :  FindVmpSupportVmpParam
功能      :  找合适vmpid，或指定vmpid时判是否支持
算法实现  :  
参数说明  :  u8 &byVmpId in/out
			 u8 byVmpStyle in
			 u8 byVmpCount in
返回值说明:  error code
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130527					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::FindVmpSupportVmpParam(u8 &byVmpId,u8 byVmpStyle, u8 byVmpCount, u16 &wError)
{
	wError = 0;
	// 界面是否指定了vmpid，若指定了开启指定vmp
	if (IsValidVmpId(byVmpId))
	{
		if (!IsVMPSupportVmpStyle(byVmpStyle, byVmpId, wError))
		{
			return FALSE;
		}
	}
	else
	{
		// 目前业务，未指定vmpid场景下，只允许开1个vmp
		if (byVmpCount > 0)
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_VMP, "start vmp(Id:%d), vmp count:%d, nack!\n" , byVmpId, byVmpCount);					
			wError = ERR_MCU_VMPRESTART;
			return FALSE;
		}
		//选VMP
		u8 byIdleVMPNum = 0;
		u8 abyIdleVMPId[MAXNUM_PERIEQP];
		memset( abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
		g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );
		if( byIdleVMPNum == 0 )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP,"[VmpCommonReq]No Idle VMP!\n"); 
			wError = ERR_MCU_NOIDLEVMP;
			return FALSE;
		}
		else
		{
			if ( !IsMCUSupportVmpStyle(byVmpStyle, byVmpId, EQP_TYPE_VMP, wError) )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

/*==============================================================================
函数名    :  UpdateVmpMembersWithConfInfo
功能      :  根据会议信息更新vmp成员
算法实现  :  
参数说明  :  u8 byVmpId in
			 u8 byVmpStyle in
			 u8 byVmpCount in
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::UpdateVmpMembersWithConfInfo(const TEqp &tVmpEqp, TVMPParam_25Mem& tVmpParam)
{
	TMt tMt;
	tMt.SetNull();
	u8 byLoop = 0;
	TVMPMember tVMPMember;
	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	for( byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		if (NULL == tVmpParam.GetVmpMember( byLoop ))
		{
			tVMPMember.SetNull();
		}
		else
		{
			tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		}
		//20110607 zjl 走读  成员为空时成员类型也是空，没必要再循环，continue即可
		if( !tVMPMember.IsNull() || tVMPMember.GetMemberType() != VMP_MEMBERTYPE_MCSSPEC)
		{
			if( ( VMP_MEMBERTYPE_CHAIRMAN == tVMPMember.GetMemberType() && !HasJoinedChairman() ) 
				|| ( VMP_MEMBERTYPE_SPEAKER == tVMPMember.GetMemberType() 
					&& // 顾振华 [5/23/2006] 只允许终端进入通道，不允许放像设备
					( !HasJoinedSpeaker() || m_tConf.GetSpeaker().GetType() == TYPE_MCUPERI || IsVrsRecMt(m_tConf.GetSpeaker())) )
				||( VMP_MEMBERTYPE_POLL == tVMPMember.GetMemberType()  && 
					(POLL_STATE_NONE == m_tConf.m_tStatus.GetPollState() /*||
 					 // xliang [3/30/2009] 轮询跟随情况,调整Param. 否则之后判断同一个MT进多个通道限制会误判
				     (!tTmpPollMt.IsNull() && !(tTmpPollMt == tTmpMt))*/))
				||( VMP_MEMBERTYPE_DSTREAM == tVMPMember.GetMemberType() && 
					m_tDoubleStreamSrc.IsNull())//双流跟随通道,无双流源时,清空
			  )
			{
				tVMPMember.SetMemberTMt( tMt );
			}
			// 2011-8-16 add by pgf 发言人跟随和轮询视频跟随，再开启会议轮询带音频，两个子通道轮询不同步
			// 支持终端进vmp多通道时,才需要同步,终端不支持进vmp多通道时,若此终端已在vmp其它通道中,不会更新轮询跟随通道
			else if ( tVMPMember.GetMemberType() == VMP_MEMBERTYPE_POLL)
			{
				// 填的是会议轮询参数的当前轮询到的终端
				if ( m_tConf.m_tStatus.GetPollState() != POLL_STATE_NONE)
				{
					TMt tmpTmt;
					tmpTmt.SetNull();
					// 2011-12-2 add by pgf: 主席轮询或上传轮询时，不会引起VMP的成员改变
					if ( m_tConf.m_tStatus.GetPollInfo()->GetMediaMode() != MODE_VIDEO_CHAIRMAN
						&& m_tConf.m_tStatus.GetPollInfo()->GetMediaMode() != MODE_BOTH_CHAIRMAN
						&& m_tConf.m_tStatus.GetPollInfo()->GetMediaMode() != MODE_VIDEO_SPY
						&& m_tConf.m_tStatus.GetPollInfo()->GetMediaMode() != MODE_BOTH_SPY
						)
					{
						tmpTmt = m_tConfPollParam.GetCurrentMtPolled()->GetTMt();
					}

					// 允许终端进vmp多通道,或通道从非轮询切为轮询跟随时,更新轮询跟随通道成员
					if (IsAllowVmpMemRepeated(tVmpEqp.GetEqpId()) || 
						!tConfVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL))
					{
						tVMPMember.SetMemberTMt(tmpTmt);
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[CheckVmpParam] chnnl.%d,type.%d,member is<%d,%d>\n",
							byLoop, VMP_MEMBERTYPE_POLL, tmpTmt.GetMcuId(), tmpTmt.GetMtId());
					}
				}
			}
			// 2011-8-16 add end
			else if (tVMPMember.GetMemberType() == VMP_MEMBERTYPE_MCSSPEC && !m_tConfAllMtInfo.MtJoinedConf(tVMPMember))
			{
				tVMPMember.SetMemberTMt( tMt );
			}		
			else if( VMP_MEMBERTYPE_SPEAKER == tVMPMember.GetMemberType() 
					&& HasJoinedSpeaker() 
					&& !(m_tConf.GetSpeaker().GetType() == TYPE_MCUPERI || IsVrsRecMt(m_tConf.GetSpeaker())) 
					)
			{
				//tVMPMember.SetMemberTMt(GetLocalSpeaker());	// [1/18/2010 xliang] 不用local的成员
				tVMPMember.SetMemberTMt(m_tConf.GetSpeaker());
			}
			// 双流跟随通道,有双流源时更新通道成员
			else if (tVMPMember.GetMemberType() == VMP_MEMBERTYPE_DSTREAM 
					&& !m_tDoubleStreamSrc.IsNull())
			{
				// 录放像机不进vmp,vrs新录播也不进
				if (TYPE_MCUPERI == m_tDoubleStreamSrc.GetType() || IsVrsRecMt(m_tDoubleStreamSrc))
				{
					tVMPMember.SetMemberTMt( tMt );
				}
				else{
					TMt tRealDSMt = GetConfRealDsMt();
					if (tRealDSMt.IsNull())
					{
						tRealDSMt = m_tDoubleStreamSrc;
					}
					tVMPMember.SetMemberTMt( tRealDSMt );
				}
			}
			//zjj20100428 后面有个成员校验，校验过了再setin
			else if ( tVMPMember.GetMcuIdx() != LOCAL_MCUIDX )
			{
				// [1/18/2010 xliang]  在这里SETIN 风险较大，往后移. 另外此处保持原有成员信息，不进行本级转换
				// 关于兼容问题。新MCU均保持保持原有成员信息，这一点要统一。
				// 所以即使对于下级mcu不支持多回传，这里也不再进行成员转换。
				
				/*if ( !IsSMcuSupMultSpy(tVMPMember.GetMcuId()) )
				{
					OnMMcuSetIn(tVMPMember, 0, SWITCH_MODE_SELECT);
				}*/
				
			}
			else
			{
				tVMPMember.SetMemberTMt( m_ptMtTable->GetMt(tVMPMember.GetMtId()) );
			}
			tVmpParam.SetVmpMember( byLoop, tVMPMember );
		}
	}

	return;
}

/*==============================================================================
函数名    :  UpdateVmpMembersWithConfInfo
功能      :  看会控指定的成员是否有单回传mcu下的终端占用多个通道的情况
算法实现  :  
参数说明  :  u8 byVmpId in
			 u8 byVmpStyle in
			 u8 byVmpCount in
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130527					yanghuaizhi							
==============================================================================*/
u16 CMcuVcInst::UpdateVmpMembersWithMultiSpy(const TEqp &tVmpEqp, TVMPParam_25Mem& tVmpParam)
{
	u16 wErrorCode = 0;
	u8 byLoop1 = 0;
	u8 byTempMcuId = 0;
	u8 byTempMcuId1 = 0;
	TVMPMember tVMPMember;
	TVMPMember tVMPMember1;
	TVMPMember tTempMem;
	TVMPMember* pvmpMemLast = NULL;
	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	BOOL32 bIsAllowVmpMemRepeated = IsAllowVmpMemRepeated(tVmpEqp.GetEqpId());
	for(u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		if (NULL == tVmpParam.GetVmpMember( byLoop ))
		{
			tVMPMember.SetNull();
		}
		else
		{
			tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		}


		if( tVMPMember.IsNull() || !tVMPMember.IsLocal() && IsLocalAndSMcuSupMultSpy(tVMPMember.GetMcuId()))
		{
			continue;
		}

		//跳过双流跟随通道
		if (VMP_MEMBERTYPE_DSTREAM == tVMPMember.GetMemberType())
		{
			continue;
		}

		if (tVMPMember.IsLocal())
		{
			byTempMcuId = tVMPMember.GetMtId();
		}
		else
		{
			byTempMcuId = GetFstMcuIdFromMcuIdx( tVMPMember.GetMcuId() );
		}

		for( byLoop1 = byLoop + 1; byLoop1 < tVmpParam.GetMaxMemberNum(); byLoop1++ )
		{
			if (NULL == tVmpParam.GetVmpMember( byLoop1 ))
			{
				tVMPMember1.SetNull();
			} 
			else
			{
				tVMPMember1 = *tVmpParam.GetVmpMember( byLoop1 );
			}

			if( tVMPMember1.IsNull() )
			{
				continue;
			}			
			
			//跳过双流跟随通道
			if (VMP_MEMBERTYPE_DSTREAM == tVMPMember1.GetMemberType())
			{
				continue;
			}

			if( tVMPMember1 == tVMPMember ) 			
			{
				if( bIsAllowVmpMemRepeated )
				{
					continue;
				}
				wErrorCode = ERR_MCU_REPEATEDVMPMEMBER;
				return FALSE;
			}
			
			if( tVMPMember1.IsLocal() )//&& tVMPMember.GetMtType() == MT_TYPE_SMCU)
			{
				byTempMcuId1 = tVMPMember1.GetMtId();
			}
			else
			{
				byTempMcuId1 = GetFstMcuIdFromMcuIdx(tVMPMember1.GetMcuId());
			}
			

			if( byTempMcuId1 == byTempMcuId )
			{
				if( m_ptMtTable->GetMtType(byTempMcuId) == MT_TYPE_SMCU &&
					( IsLocalAndSMcuSupMultSpy(GetMcuIdxFromMcuId(&byTempMcuId) ) ||
					bIsAllowVmpMemRepeated )
					)
				{
					//zhouyiliang 20101019 单回传允许重复进多个通道，所有同级mcu下的终端设置成最后一个画面合成里的该mcu下的终端
					if (!IsLocalAndSMcuSupMultSpy(GetMcuIdxFromMcuId(&byTempMcuId)) && (!tVMPMember1.IsLocal() || IsMcu(tVMPMember1))) 
					{
						//zhouyiliang 20101206 跟上次画面合成参数比较，按最后时间进入的来设置，而不是最后位置
						u8 byLastMemberPos = MAXNUM_VMP_MEMBER;//tVmpParam.GetSMcuLastMtMemberPos( tVMPMember1.GetMcuId() );
						for (u8 byIndex = 0; byIndex < tVmpParam.GetMaxMemberNum()/*GetVMPMemberNum()*/ ; byIndex++ )
						{
							if (NULL == tVmpParam.GetVmpMember( byIndex ))
							{
								tTempMem.SetNull();
							} 
							else
							{
								tTempMem = *tVmpParam.GetVmpMember( byIndex );
							}
							//跳过双流跟随通道
							if (VMP_MEMBERTYPE_DSTREAM == tTempMem.GetMemberType())
							{
								continue;
							}
							if ( !tConfVmpParam.IsMtInMember(tTempMem) /*&& tTempMem.GetMcuId() == tVMPMember.GetMcuId()*/
									&& GetLocalMtFromOtherMcuMt(tTempMem) == GetLocalMtFromOtherMcuMt(tVMPMember)
									&& tConfVmpParam.GetVMPStyle() != VMP_STYLE_NONE && tConfVmpParam.GetVMPMemberNum() != 0
								) 
							{
								//tVmpMember 是最后进来的
								byLastMemberPos = byIndex;
								break;

							}
							// 开启vmp时,LastVmpParam成员数为0,从tVmpParam中找最后一个通道即可
							else if (tConfVmpParam.GetVMPMemberNum() == 0 &&
								GetLocalMtFromOtherMcuMt(tTempMem) == GetLocalMtFromOtherMcuMt(tVMPMember))
							{
								byLastMemberPos = byIndex;//找最后一个,不用break
							}
						}
						
						if (  byLastMemberPos != MAXNUM_VMP_MEMBER )
						{
							//只覆盖终端,不覆盖通道类型与成员状态
							//TVMPMember vmpMemLast = *tVmpParam.GetVmpMember( byLastMemberPos );
							pvmpMemLast = tVmpParam.GetVmpMember( byLoop );
							if (NULL != pvmpMemLast && NULL != tVmpParam.GetVmpMember( byLastMemberPos ))
							{
								pvmpMemLast->SetMemberTMt(*tVmpParam.GetVmpMember( byLastMemberPos ));
								tVmpParam.SetVmpMember(byLoop, *pvmpMemLast );
								if (byLastMemberPos !=  byLoop1 && NULL != tVmpParam.GetVmpMember( byLoop1 )) 
								{
									pvmpMemLast = tVmpParam.GetVmpMember( byLoop1 );
									pvmpMemLast->SetMemberTMt(*tVmpParam.GetVmpMember( byLastMemberPos ));
									tVmpParam.SetVmpMember(byLoop1, *pvmpMemLast );	
								}
							}
							
						}
					}
					continue;
				}
			
				wErrorCode = ERR_MCUNOMULTISPYONLYUSEONEVMPCHANNEL;
				return wErrorCode;
			}
		}
	}

	return wErrorCode;
}

/*==============================================================================
函数名    :  CheckChangeVmpReq
功能      :  画面合成调整时的各种Check
算法实现  :  
参数说明  :  u8 &byVmpId in/out
			 TVMPParam_25Mem &tVmpParam in/out
			 BOOL32 bIsVmpModule,
			 CServMsg &cServMsg
返回值说明:  error code
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130527					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::CheckChangeVmpReq(u8 &byVmpId, TVMPParam_25Mem &tVmpParam, u16 &wError)
{
	wError = 0;
	if (!IsValidVmpId(byVmpId))
	{
		// 主席终端不会指定vmpid，需使用当前仅有的vmp直接做处理
		byVmpId = GetTheOnlyVmpIdFromVmpList();
		if (!IsValidVmpId(byVmpId))
		{
			return FALSE;
		}
	} 

	// 判vmp是否支持对应风格
	if (!IsVMPSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, wError))
	{
		return FALSE;
	}

	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );

	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	//检查当前会议的画面合成状态 
	if( tConfVmpParam.GetVMPMode() == CONF_VMPMODE_NONE )
	{
		wError = ERR_MCU_VMPNOTSTART;
		return FALSE;
	}
	
	if ( !CheckVmpParam(byVmpId, tVmpParam, wError ) )
	{
		return FALSE;
	}

	if( tVmpParam.IsVMPAuto() )
	{
		//自动画面合成能力限制
		u8 byVmpDynStle = GetVmpDynStyle(byVmpId,  m_tConfAllMtInfo.GetLocalJoinedMtNum() );
		if ((tVmpParam.GetVMPStyle() == VMP_STYLE_DYNAMIC) &&
			VMP_STYLE_NONE == byVmpDynStle)
		{
			wError = ERR_INVALID_VMPSTYLE;
			return FALSE;
		}

		tVmpParam.SetVMPStyle( byVmpDynStle );
	}
	
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	
	// 画面合成校验,不区分vmp外设
	if( !tVmpParam.IsVMPAuto())
	{
		if (!CheckMpuMember(byVmpId, tVmpParam, wError, TRUE))
		{
			return FALSE;
		}
	}

	// xsl [7/20/2006]卫星分散会议需要检查回传通道数
	if (m_tConf.GetConfAttrb().IsSatDCastMode())
	{
		/*u8     byNewChnMtid = 0;
		u8     byChnId      = 0xFF;
		TVMPMember *ptVmpMember = NULL;
		TVMPMember *ptConfVmpMember = NULL;
		TPollInfo *ptPollInfo  = NULL;
		for (u8 byLoop1 = 0; byLoop1 < tVmpParam.GetMaxMemberNum(); ++byLoop1 )
		{
			ptVmpMember = tVmpParam.GetVmpMember(byLoop1);
			if (ptVmpMember != NULL)
			{
				if (!ptVmpMember->IsNull())
				{
					ptConfVmpMember = tConfVmpParam.GetVmpMember(byLoop1);
					if (ptConfVmpMember != NULL && !(*tConfVmpParam.GetVmpMember(byLoop1) == *ptVmpMember))
					{
						byNewChnMtid = GetLocalMtFromOtherMcuMt(*(TMt *)ptVmpMember).GetMtId();
						byChnId      = byLoop1;
						break;
					}
				}
				else
				{
					if (ptVmpMember->GetMemberType() != tConfVmpParam.GetVmpMember(byLoop1)->GetMemberType())
					{
						switch (ptVmpMember->GetMemberType())
						{
						case VMP_MEMBERTYPE_SPEAKER:
							if (!GetLocalSpeaker().IsNull())
							{
								byNewChnMtid = GetLocalSpeaker().GetMtId();
								byChnId      = byLoop1;
							}
							break;
						case VMP_MEMBERTYPE_CHAIRMAN:
							if (!m_tConf.GetChairman().IsNull())
							{
								byNewChnMtid = m_tConf.GetChairman().GetMtId();
								byChnId      = byLoop1;
							}
							break;
						case VMP_MEMBERTYPE_POLL:
							ptPollInfo = m_tConf.m_tStatus.GetPollInfo();
							if (ptPollInfo != NULL && !ptPollInfo->GetMtPollParam().GetTMt().IsNull())
							{
								byNewChnMtid = GetLocalMtFromOtherMcuMt(ptPollInfo->GetMtPollParam().GetTMt()).GetMtId();
								byChnId      = byLoop1;
							}
							break;
						case VMP_MEMBERTYPE_DSTREAM:
							if (!m_tDoubleStreamSrc.IsNull())
							{
								byNewChnMtid = m_tDoubleStreamSrc.GetMtId();
								byChnId      = byLoop1;
							}
							break;
						default:
							break;
						}
					}
				}
			}
		}*/
		if (/*byNewChnMtid != 0 &&*/ !IsSatMtCanContinue(0,emVmpChnChange,&tVmpEqp,0xFF, 0xFF, &tVmpParam))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[VmpCommonReq-change] over max upload mt num. nack!\n");
			wError = ERR_MCU_DCAST_OVERCHNNLNUM;
			return FALSE;
		}
	}
	
	return TRUE;
}

/*==============================================================================
函数名    :  StartVmpSwitchGrp2AllMt
功能      :  建vmp通道到各终端的交换,对终端分组建交换
算法实现  :  
参数说明  :  TEqp &tVmpEqp in
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130527					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::StartVmpSwitchGrp2AllMt(const TEqp &tVmpEqp)
{
	if (tVmpEqp.IsNull() || !IsValidVmpId(tVmpEqp.GetEqpId()))
	{
		return;
	}
	
	TMt tLocalVidBrdSrc =  GetLocalVidBrdSrc();
	u8 byVmpSubType = GetVmpSubType(tVmpEqp.GetEqpId());
	TSwitchGrp atSwitchGrp[MAXNUM_MPU2_OUTCHNNL];	//交换组信息数组长度9,用于批量建交换
	u8 abyOutChnl[MAXNUM_MPU2_OUTCHNNL];			//后适配通道数组:长度9,用于记录过通道终端Idx
	u8 abyTmpMtIdx[MAXNUM_CONF_MT+1];				//终端Idx缓存数组长度193
	TMt atDstMt[MAXNUM_CONF_MT+1];					//连续终端缓存数组:长度193,用于批量建交换
	u8 bySwitchGrpNum = 0;	//交换组信息数组长度
	u8 bySrcChnnl = 0;
	TMt tMt;
	TMtStatus tMtStatus;
	TMt tSelectedMt;
	
	if( VMP != byVmpSubType )
	{
		TVideoStreamCap tStrCap = m_tConf.GetMainSimCapSet().GetVideoCap();
		u8 byConfMVSrcChnnl = GetVmpOutChnnlByRes(tVmpEqp.GetEqpId(),
												   tStrCap.GetResolution(), 
												   tStrCap.GetMediaType(),
												   tStrCap.GetUserDefFrameRate(),
												   tStrCap.GetH264ProfileAttrb());
		// [1/19/2011 xliang] 组播会议组播数据
		if (m_tConf.GetConfAttrb().IsMulticastMode())
		{	
			// [1/30/2013 liaokang] 增加判断
			if( 0xFF != byConfMVSrcChnnl )
			{
				g_cMpManager.StartMulticast(tLocalVidBrdSrc, byConfMVSrcChnnl, MODE_VIDEO);
				m_ptMtTable->SetMtMulticasting(tLocalVidBrdSrc.GetMtId());
			}
		}
		
		//  [3/2/2012 pengguofeng]添加卫星会议交换
		if ( m_tConf.GetConfAttrb().IsSatDCastMode() )
		{		
			// [1/30/2013 liaokang] 增加判断
			if( 0xFF != byConfMVSrcChnnl )
			{
				g_cMpManager.StartDistrConfCast(tLocalVidBrdSrc, MODE_VIDEO, byConfMVSrcChnnl);
			}		
		}

		//---------------群组交换--------------
		//接收群组分类
		memset(abyOutChnl, 0, sizeof(abyOutChnl));
		memset(abyTmpMtIdx, 0, sizeof(abyTmpMtIdx));
		
		for(u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++)
		{
			if( !m_tConfAllMtInfo.MtJoinedConf( byMtIdx ) )
			{
				continue;
			}
			
			bySrcChnnl = GetVmpOutChnnlByDstMtId( byMtIdx, tVmpEqp.GetEqpId());

			if( bySrcChnnl != 0xFF )
			{
				// vrs新录播过滤
				if (m_ptMtTable->GetMtType(byMtIdx) == MT_TYPE_VRSREC)
				{
					continue;
				}

				if( IsMultiCastMt( byMtIdx ) && 0xFF != byConfMVSrcChnnl && bySrcChnnl != byConfMVSrcChnnl )
				{		
					NotifyMtReceive( m_ptMtTable->GetMt(byMtIdx), byMtIdx );
					continue;							
				}

				m_ptMtTable->GetMtStatus(byMtIdx, &tMtStatus);
				tSelectedMt = tMtStatus.GetSelectMt( MODE_VIDEO ); 
				// xliang [4/24/2009] 点名人被点名人互相选看，VMP不交换码流给他们
				if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
					&& (m_tRollCaller.GetMtId() == byMtIdx 
					|| ( m_tRollCallee.IsLocal() && m_tRollCallee.GetMtId() == byMtIdx))
					)
				{
					//zhouyiliang 20101231 如果点名人选看的不是被点名人，也让其看画面合成，被点名人一样
					//zjj20100304 这里不用判断选看源，如果是第一次起点名有可能选看源不存在
					if (!tSelectedMt.IsNull() && (m_tRollCaller.GetMtId() == byMtIdx && tSelectedMt == m_tRollCallee || 
						m_tRollCallee.GetMtId() == byMtIdx && tSelectedMt == m_tRollCaller)
						) 
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP,"[StartMpuVmpBrd] byMtIdx.%d is RollCaller or RollCallee.so not recv vmp.\n",byMtIdx );
						continue;
					}
				}			
			
				// [1/20/2010 xliang] 上传轮询过滤
				tMt = m_ptMtTable->GetMt(byMtIdx);
				if( MT_TYPE_MMCU == m_ptMtTable->GetMtType(byMtIdx) )
				{
					TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
					if ( CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode() 
						&& ( MODE_VIDEO_SPY == tPollInfo.GetMediaMode() || MODE_BOTH_SPY == tPollInfo.GetMediaMode()) 
						)
					{
						continue; 
					}
				}			
				
				// [8/29/2011 liuxu] vcs主席轮询过滤
				if ( m_tConf.GetChairman() == tMt )
				{
					if ( m_tConf.GetConfSource() == VCS_CONF &&
						(m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP || VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode() )
						)
					{
						continue;
					}
					if( !tSelectedMt.IsNull() )
					{
						continue;
					}					
				}
				
				
				abyTmpMtIdx[byMtIdx] = abyOutChnl[bySrcChnnl];	//更新TmpMtIdx
				abyOutChnl[bySrcChnnl] = byMtIdx;	//更新abyOutChnl
			}
		}
		//更新终端缓存数组与交换组信息数组
		bySwitchGrpNum = 0;	//交换组信息数组长度
		u8 byDstMtIdx = 0;		//连续终端缓存数组插入位置
		for (u8 byOutputChl = 0; byOutputChl < MAXNUM_MPU2_OUTCHNNL; byOutputChl++)
		{
			if (0 != abyOutChnl[byOutputChl])//此通道有终端
			{
				u8 byMtNum = 0;
				atSwitchGrp[bySwitchGrpNum].SetSrcChnl( byOutputChl );
				atSwitchGrp[bySwitchGrpNum].SetDstMt( &atDstMt[byDstMtIdx] );

				//更新终端缓存数组
				u8 byInsMtIdx = abyOutChnl[byOutputChl];	//要插入的终端Idx
				do 
				{
					atDstMt[byDstMtIdx] = m_ptMtTable->GetMt(byInsMtIdx);
					byInsMtIdx = abyTmpMtIdx[byInsMtIdx];	//获得下一个要插入的终端Idx
					byMtNum++;	//记录此通道对应终端个数
					byDstMtIdx++; //连续终端数组下标递增
				} while (0 != byInsMtIdx);	//Idx不为0时表示还有下个终端
				
				atSwitchGrp[bySwitchGrpNum].SetDstMtNum( byMtNum );
				bySwitchGrpNum++;
			}
		}
	}
	else//vpu处理
	{
		g_cMpManager.StartSwitchToBrd(tLocalVidBrdSrc, 0, TRUE);

		if (m_tConf.GetConfAttrb().IsMulticastMode() )
		{
			if ( !m_tConf.GetConfAttrb().IsMulcastLowStream())
			{
			
				g_cMpManager.StartMulticast( tLocalVidBrdSrc, 0, MODE_VIDEO);
			}
			else
			{
				g_cMpManager.StartMulticast( tLocalVidBrdSrc, 1, MODE_VIDEO);
			}
			m_ptMtTable->SetMtMulticasting(tLocalVidBrdSrc.GetEqpId());
		}
		if ( m_tConf.GetConfAttrb().IsSatDCastMode() )
		{		
			g_cMpManager.StartDistrConfCast(tLocalVidBrdSrc, MODE_VIDEO, 0);
		}
		TMt atMtList[MAXNUM_CONF_MT+1];
		u8 bySecNum = 0,byNum = 0;
		TSimCapSet tSimCapSet;
		for(u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++)
		{
			// vrs新录播过滤
			if (m_ptMtTable->GetMtType(byMtIdx) == MT_TYPE_VRSREC)
			{
				continue;
			}

			m_ptMtTable->GetMtStatus(byMtIdx, &tMtStatus);
			tSelectedMt = tMtStatus.GetSelectMt( MODE_VIDEO ); 
			// xliang [4/24/2009] 点名人被点名人互相选看，VMP不交换码流给他们
			if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
				&& (m_tRollCaller.GetMtId() == byMtIdx 
				|| ( m_tRollCallee.IsLocal() && m_tRollCallee.GetMtId() == byMtIdx))
				)
			{
				//zhouyiliang 20101231 如果点名人选看的不是被点名人，也让其看画面合成，被点名人一样
				//TMtStatus tStat;
				//zjj20100304 这里不用判断选看源，如果是第一次起点名有可能选看源不存在
				if (!tSelectedMt.IsNull() && (m_tRollCaller.GetMtId() == byMtIdx && tSelectedMt == m_tRollCallee || 
					m_tRollCallee.GetMtId() == byMtIdx && tSelectedMt == m_tRollCaller)
					) 
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP,"[StartMpuVmpBrd] byMtIdx.%d is RollCaller or RollCallee.so not recv vmp.\n",byMtIdx );
					continue;
				}
			}
		
			// [1/20/2010 xliang] 上传轮询过滤
			tMt = m_ptMtTable->GetMt(byMtIdx);
			if( MT_TYPE_MMCU == m_ptMtTable->GetMtType(byMtIdx) )
			{
				TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
				if ( CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode() 
					&& ( MODE_VIDEO_SPY == tPollInfo.GetMediaMode() || MODE_BOTH_SPY == tPollInfo.GetMediaMode()) 
					)
				{
					continue; 
				}
			}			
			
			// [8/29/2011 liuxu] vcs主席轮询过滤
			if ( m_tConf.GetChairman() == tMt )
			{
				if ( m_tConf.GetConfSource() == VCS_CONF &&
					(m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP || VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode() )
					)
				{
					continue;
				}
				if( !tSelectedMt.IsNull() )
				{
					continue;
				}					
			}
			
			if ( !CanMtRecvVideo( tMt, tLocalVidBrdSrc ) )
			{
				tSimCapSet = m_ptMtTable->GetDstSCS( byMtIdx );
				if( MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() 
					&& (tSimCapSet.GetVideoMediaType() == m_tConf.GetSecVideoMediaType() )
					)
				{
					atDstMt[bySecNum++] = tMt;		
				}
			}
			else
			{
				atMtList[byNum++] = tMt;
			}
		}
		
		if ( byNum > 0)
		{		
		    StartSwitchFromBrd(tLocalVidBrdSrc, 0, byNum, atMtList);
		}
		atSwitchGrp[0].SetSrcChnl(1);
		atSwitchGrp[0].SetDstMtNum(bySecNum);
		atSwitchGrp[0].SetDstMt(atDstMt);
		bySwitchGrpNum = 1;
	}
	
	// 建交换
	StartSwitchToAll( tLocalVidBrdSrc, bySwitchGrpNum, atSwitchGrp, MODE_VIDEO, SWITCH_MODE_SELECT );
	
	return;
}

/*==============================================================================
函数名    :  SendPreSetInReqForVmp
功能      :  画面合成级联多回传处理
算法实现  :  
参数说明  :  TEqp &tVmpEqp in
			 TVMPParam_25Mem &tVmpParam in/out
			 bStart in
			 bSendPreSetin out
返回值说明:  BOOL32
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130529					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::SendPreSetInReqForVmp(const TEqp &tVmpEqp, TVMPParam_25Mem *ptVMPParam, BOOL32 bStart, BOOL32 &bSendPreSetIn)
{
	u8 byVmpId = tVmpEqp.GetEqpId();
	if (!IsValidVmpId(byVmpId) || NULL == ptVMPParam)
	{
		return FALSE;
	}

	//定义部分
	bSendPreSetIn = FALSE;
	TVMPMember* ptVMPMember = NULL;
	TVMPMember* ptConfVmpMember = NULL;
	CVmpChgFormatResult cVmpChgFormatResult;
	u8 byNewFormat = 0;
	BOOL32 bMemChange = FALSE;
	TPreSetInReq tPreSetInReq;
	TSimCapSet tSimCapSet;
	u8 byVmpSpeakerNum = 0;
	u8 byVmpPollNum = 0;
	u8 byTWSpeakerVNum = 0;
	u8 byTWSpeakerANum = 0;
	u8 byTWPollVNum = 0;
	u8 byTWPollANum = 0;
	u8 byHduSpeakerVNum = 0;
	u8 byHduSpeakerANum = 0;
	u8 byHduPollVNum = 0;
	u8 byHduPollANum = 0;
	u8 byManuID = MT_MANU_KDC;
	TMt tNullMt;
	tNullMt.SetNull();
	TConfMcInfo *ptMcInfo = NULL;
	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	for(u8 byLoop = 0; byLoop < ptVMPParam->GetMaxMemberNum() ; byLoop++ )
	{
		ptVMPMember = ptVMPParam->GetVmpMember( byLoop );
		ptConfVmpMember = tConfVmpParam.GetVmpMember(byLoop);
		
		if (ptVMPMember == NULL
			|| ptVMPMember->IsNull())
		{
			continue;
		}
		
		if (ptVMPMember->IsLocal())
		{
			continue;
		}

		if (ptConfVmpMember == NULL)
		{
			continue;
		}

		// 电话终端不发PresetIn请求,下级电话终端不做额外多回传处理
		if (IsPhoneMt(*ptVMPMember))
		{
			continue;
		}

		bMemChange = TRUE;
		//tOldVmpMeme = *m_tLastVmpParam.GetVmpMember( byLoop ); 级联多回传请求与LastVmpParam无关
		//针对旧成员是双流跟随,新成员是会控指定,且两成员一样时,仍是change
		//由非双流跟随切为双流跟随时,bNeedPreSetIn为FALSE,不会进此逻辑
		if( ((TMt)(*ptVMPMember) == (TMt)(*ptConfVmpMember)) &&
			VMP_MEMBERTYPE_DSTREAM != ptConfVmpMember->GetMemberType()
			)
		{
			bMemChange = FALSE;
		}

		//针对VCS会议,可能出现多个下级同时移位现象,移位情况下不再发presetin请求,在changevmpswitch中也不再对移位终端做freespy处理
		BOOL32 bInOtherchan = FALSE;
		if ( VCS_CONF == m_tConf.GetConfSource() && tConfVmpParam.IsMtInMember(*ptVMPMember)) 
		{
			bInOtherchan = TRUE;
		}

		// [3/20/2012 yhz] 现在不考虑发言人跟随(恢复)挪位,只要新旧成员不相同,就对新成员发PresetIn请求
		if (bMemChange && !bInOtherchan)
		{
			// 双流跟随通道不做级联多回传处理
			if ( IsLocalAndSMcuSupMultSpy(ptVMPMember->GetMcuId()) && 
				 VMP_MEMBERTYPE_DSTREAM != ptVMPMember->GetMemberType())
			{
				TMt tMtVmpMember = (TMt)(*ptVMPMember);
				if (!GetMtFormat(byVmpId, tMtVmpMember, ptVMPParam, byNewFormat))
				{
					//对于多回传下级终端,若GetMtFormat失败,不发PresetinReq,也不应该保留通道成员,
					//不然会导致未发Presetin但却在通道里还建了交换,到释放多回传时,又多释放
					//若是跟随通道,也应清理跟随通道属性,因为改变发言人时,是在改变发言人处理中先根据跟随通道个数释放多回传资源的
					//不清的话,会出现有的跟随通道无终端有的跟随通道有终端
					//场景:vmp中多路发言人跟随,拖1下级非科达终端做发言人,前适配不足时,GetMtFormat会返回FALSE
					if (ptVMPMember->GetMemberType() == VMP_MEMBERTYPE_MCSSPEC)
					{
						ptVMPParam->ClearVmpMember(byLoop);
					}
					else
					{
						ptVMPMember->SetMt(tNullMt);
					}
					continue;
				}
				
				bSendPreSetIn = TRUE;

				// [8/2/2010 xliang] 
				u8 byKeepRes = ( cVmpChgFormatResult.IsKeepResInAdpChnnl() || cVmpChgFormatResult.IsSendMsg2Mt() )? 1: 0;
				
				//发Pre Setin (包含了级联调分辨率)
				memset(&tPreSetInReq, 0, sizeof(tPreSetInReq));
				TMt tSpyMt = (TMt)(*ptVMPMember);
				tPreSetInReq.m_tSpyMtInfo.SetSpyMt( tSpyMt );
				tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_tVmp = tVmpEqp;
				tPreSetInReq.m_bySpyMode = MODE_VIDEO;
				tPreSetInReq.SetEvId(MCS_MCU_STARTVMP_REQ);
				
//					tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byRes = byNewFormat;
				tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byPos = byLoop;		
//					tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byKeepOrgRes = byKeepRes;
				tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byMemberType = ptVMPMember->GetMemberType();
				tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byMemStatus = ptVMPMember->GetMemStatus();

				// [pengjie 2010/9/13] 填目的端能力
				tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(ptVMPMember->GetMcuId()) );	
				if(tSimCapSet.IsNull())
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[AdjustVmpParam] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
						ptVMPMember->GetMcuId(), ptVMPMember->GetMtId() );
					return FALSE;
				}
				// 调整到画面合成要求的分辨率
				tSimCapSet.SetVideoResolution( byNewFormat );
				//zjl20101116 如果当前终端已回传则能力集要与已回传目的能力集取小
				if (!GetMinSpyDstCapSet((TMt)(*ptVMPMember), tSimCapSet))
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[AdjustVmpParam] GetMinSpyDstCapSet failed!\n");
					return FALSE;
				}
				
				tPreSetInReq.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		        // End

				//zhouyiliang20100806 多级级联画面合成调整，如果是vcs会议自动画面合成的话将可替换的mt提前传下去
				s16 swspyIndex = -1;
				u8  byChnnIndex = 0;
				TMt tCanReplaceSpyMt ;
				if ( tConfVmpParam.IsVMPAuto())
				{
					//zhouyiliang 20100831 FindVCSCanReplaceSpyMt里面注释掉原有的vcs自动画面合成替换策略，沿用老的顺序替换策略，替换的顺序是谁先进替换谁
					//tCanReplaceSpyMt = FindVCSCanReplaceSpyMt(EQP_TYPE_VMP,tPreSetInReq.m_tSrc,swspyIndex,byChnnIndex);
//							if ( tCanReplaceSpyMt.IsNull() && 1 < ptVMPParam->GetMaxMemberNum() ) //如果是风格没满的情况
//						{
//							tCanReplaceSpyMt = FindVCSCanReplaceSpyMt(EQP_TYPE_VMP,tPreSetInReq.m_tSrc,swspyIndex,byChnnIndex);
//						}
					if (m_tConf.GetConfSource() == VCS_CONF ) 
					{
						tCanReplaceSpyMt = (TMt)(*tConfVmpParam.GetVmpMember( byLoop ));
					}
					else if ( m_tConf.GetSpeaker() == (TMt)(*ptVMPMember) ) //zhouyiliang 20101015mcs自动画面合成只有发言人才需要带releasemt下去
					{
						tCanReplaceSpyMt = m_tLastSpeaker;
					}	
								
					
				}
				//zhouyiliang 20100825 mcs会议的定制画面合成，如果是替换操作也要将releasemt带下去
				else /*if( !m_tConf.m_tStatus.m_tVMPParam.IsVMPAuto() )*/
				{
					tCanReplaceSpyMt = (TMt)(*tConfVmpParam.GetVmpMember( byLoop ));
				}
				
				if ( !tCanReplaceSpyMt.IsNull() ) 
				{	
					tPreSetInReq.m_tReleaseMtInfo.m_tMt = tCanReplaceSpyMt;
					tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_VIDEO;
					tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 0;//初始化可释放终端个数为0

					//发言人跟随与轮询跟随更新时,需统计各跟随通道可释放终端个数
					if ( ptVMPMember->GetMemberType() == tConfVmpParam.GetVmpMember(byLoop)->GetMemberType() &&
						 (ptVMPMember->GetMemberType() == VMP_MEMBERTYPE_SPEAKER || ptVMPMember->GetMemberType() == VMP_MEMBERTYPE_POLL)
						)
					{
						// vmp所有跟随通道
						byVmpSpeakerNum = GetVmpChnnlNumBySpecMemberType(VMP_MEMBERTYPE_SPEAKER, &tCanReplaceSpyMt);
						byVmpPollNum = GetVmpChnnlNumBySpecMemberType(VMP_MEMBERTYPE_POLL, &tCanReplaceSpyMt);
						
						// tvWall所有跟随通道
						GetTvWallChnnlNumBySpecMemberType(TW_MEMBERTYPE_SPEAKER, byTWSpeakerVNum, byTWSpeakerANum);
						GetTvWallChnnlNumBySpecMemberType(TW_MEMBERTYPE_POLL, byTWPollVNum, byTWPollANum);
						
						// hdu所有跟随通道
						GetHduChnnlNumBySpecMemberType(TW_MEMBERTYPE_SPEAKER, byHduSpeakerVNum, byHduSpeakerANum);
						GetHduChnnlNumBySpecMemberType(TW_MEMBERTYPE_POLL, byHduPollVNum, byHduPollANum);
						
						if ( m_tConf.m_tStatus.GetPollMode() != CONF_POLLMODE_NONE)
						{
							// 轮询的计数，根据轮询的媒体模式来区别
							u8 byPollMode = m_tConf.m_tStatus.GetPollInfo()->GetMediaMode();
							
							// 带音频轮询时，需统计发言人跟随与轮询跟随之和
							if ( byPollMode == MODE_BOTH || byPollMode == MODE_BOTH_BOTH)
							{
								tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum
									+ byVmpSpeakerNum + byVmpPollNum + byTWSpeakerVNum + byTWPollVNum + byHduSpeakerVNum + byHduPollVNum;
							}
							// 不带音频轮询时，且该通道是轮询跟随时，只统计轮询跟随数量
							else if (ptVMPMember->GetMemberType() == VMP_MEMBERTYPE_POLL)
							{
								tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum
									+ byVmpPollNum + byTWPollVNum + byHduPollVNum;
							}
							// 有不带音频轮询时，但手动切发言人时，针对发言人跟随通道，统计仍需按发言人跟随数统计
							else
							{
								tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum
									+ byVmpSpeakerNum + byTWSpeakerVNum + byHduSpeakerVNum;
							}
						}
						else
						{
							if ( ptVMPMember->GetMemberType() == VMP_MEMBERTYPE_SPEAKER)
							{
								tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum
									+ byVmpSpeakerNum + byTWSpeakerVNum + byHduSpeakerVNum;
							}
						}
					}
					else //非发言人跟随与轮询跟随导致的通道更新,可释放终端仅为此通道旧成员
					{
						tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
					}

					//vcs自动画面合成的话，如果还打到监控窗口的，可以释放的音视频目的数为2，1
					if ( m_tConf.GetConfSource() == VCS_CONF && tConfVmpParam.IsVMPAuto() )
					{
						u8 byMtInMcNum = GetCanRlsNumInVcsMc(tCanReplaceSpyMt, MODE_VIDEO);
						tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum += byMtInMcNum;
						
						byMtInMcNum = GetCanRlsNumInVcsMc(tCanReplaceSpyMt, MODE_AUDIO);
						if (byMtInMcNum > 0)
						{
							tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum += byMtInMcNum;
							tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_BOTH;
						}

						//zhouyiliang 20101015 点名双画面，主席选看调度终端视频目的数再加1
						if ( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE ) 
						{
							//判断主席选看的终端是否是可替换终端
							TMt tChairman = m_tConf.GetChairman();
							if ( !tChairman.IsNull() ) 
							{
								TMtStatus tStat;
								m_ptMtTable->GetMtStatus(tChairman.GetMtId(), &tStat );
								if (tStat.GetSelectMt( MODE_VIDEO ) == tCanReplaceSpyMt  ) 
								{
									tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum++;
								}
								if (tStat.GetSelectMt( MODE_AUDIO ) == tCanReplaceSpyMt  ) 
								{
									tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum++;
								}
								
							}

							
						}
					
					}
					if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() &&
								m_tRollCallee == tPreSetInReq.m_tSpyMtInfo.GetSpyMt() )
					{
						//被点名人加一个监控的目的数
						tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum++;
					}
				}
						
				BOOL32 bSendPreSetinOk = OnMMcuPreSetIn( tPreSetInReq );

				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[AdjustVmpParam] send PreSetIn to vmpmember.(%d,%d in chnl.%d) \n",
						ptVMPMember->GetMcuId(), ptVMPMember->GetMtId(), byLoop);

				if( ( !bSendPreSetinOk )
					&& bStart && tPeriEqpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::RESERVE)
				{
					//如果是第一次开启，发送PreSetIn失败的情况下，vmp状态机要翻转成初始态
					u8 byVmpIdx = byVmpId - VMPID_MIN;
					KillTimer(MCUVC_VMP_WAITVMPRSP_TIMER+byVmpIdx);
					tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
					tPeriEqpStatus.SetConfIdx( 0 );
					tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tConfVmpParam);
					g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					RemoveVmpIdFromVmpList(byVmpId);//从列表中清除
					
					//update UI
					CServMsg cMsg;
					cMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cMsg);
					
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[AdjustVmpParam] send PreSetIn to vmpmember.(%d,%d in chnl.%d) failed, so turn vmp state into idle state\n",
						ptVMPMember->GetMcuId(), ptVMPMember->GetMtId(), byLoop);
					
					return FALSE;
				}

				//注意，在发PreSetIn时，要把此终端信息从Param中删除，等在PreSetInAck中处理时会加进去
				//对于非会控指定成员,清除时保留type,多路轮询跟随时,把跟随type清掉,时序上会出现误判,将其当做关闭通道处理.
				// ChangeVmpSwitch处理会误释放多回传资源
				// 20121226 发PreSetIn时，把清通道改为用旧通道信息，等Ack后刷新
				// 发言人跟随+轮询跟随时，轮询到下级多回传终端时，第一个ACK回后发消息给外设，因后1个跟随通道还为空，外设会做removechl处理导致闪黑屏
				ptVMPParam->SetVmpMember(byLoop, *ptConfVmpMember);
			}
		}
	}

	return TRUE;
}

/*==============================================================================
函数名    :  SendPreSetInReqForVmp
功能      :  画面合成级联多回传处理
算法实现  :  
参数说明  :  u8 byVmpId in
			 TVMPParam_25Mem *ptVMPParam in
返回值说明:  TVmpCommonAttrb
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130529					yanghuaizhi							
==============================================================================*/
TVmpCommonAttrb CMcuVcInst::GetVmpCommonAttrb(u8 byVmpId, TVMPParam_25Mem *ptVMPParam, BOOL32 bStart)
{
	TVmpCommonAttrb tVmpCommonAttrb;
	if (!IsValidVmpId(byVmpId) || NULL == ptVMPParam)
	{
		return tVmpCommonAttrb;
	}

	//vmp style
	tVmpCommonAttrb.m_byVmpStyle = ptVMPParam->GetVMPStyle();

	//number of members and all members' info
	/*
	note:	auto vmp: the value is the actual number of members 
		customer vmp: the value is related to style 
	*/
	u8 byMemberNum = BatchTransVmpMemberToTVMPMemberEx(
		ptVMPParam->GetVmpMember(0),
		ptVMPParam->GetMaxMemberNum(),
		tVmpCommonAttrb.m_atMtMember,
		MAXNUM_VMP_MEMBER);

	if (ptVMPParam->IsVMPAuto() || VCS_CONF == m_tConf.GetConfSource())
	{
		//maintain the memberNum be actual number
	}
	else
	{
		byMemberNum = ptVMPParam->GetMaxMemberNum();
	}
	tVmpCommonAttrb.m_byMemberNum = byMemberNum;
	
	//清掉其余通道的交换
	u8 byLoop = byMemberNum;
	if( ROLLCALL_MODE_VMP != m_tConf.m_tStatus.GetRollCallMode() )
	{
		for (; byLoop < MAXNUM_VMP_MEMBER; byLoop++)
		{
			StopSwitchToPeriEqp(byVmpId, byLoop, TRUE, MODE_VIDEO);
			ptVMPParam->ClearVmpMember(byLoop);
		}
	}

	//zjj 20091102 两画面是替换策略，所以先把第二路交换停止，因为第二路永远是调度终端(是要替换的终端)
	if( VCS_CONF == m_tConf.GetConfSource() && 
		VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode() &&
		ptVMPParam->GetVMPStyle() == VMP_STYLE_HTWO )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP,  "[ChangeVmpParam] VCS_GROUPROLLCALL_MODE StopSwitchToPeriEqp to vmp channel(1) byMember(%d)\n",byMemberNum );
		StopSwitchToPeriEqp(byVmpId, 1, FALSE, MODE_VIDEO);
	}

	// payload, EncryptKey
	TVMPMember *ptVmpMember = NULL;
	TLogicalChannel tChnnl;
	BOOL32 bRet = FALSE;
    BOOL32 bIsHasMember = FALSE;
	u8 byMtId = 0;
	u8 byManuId = 0;
	for (byLoop = 0; byLoop < ptVMPParam->GetMaxMemberNum(); byLoop++)
	{
		ptVmpMember = ptVMPParam->GetVmpMember(byLoop);
		if( ptVmpMember == NULL || ptVmpMember->IsNull())
		{
			continue;
		}
        else
        {
            bIsHasMember = TRUE;
        }

		
		if( !ptVmpMember->IsLocal() )
		{
			byMtId = GetFstMcuIdFromMcuIdx( ptVmpMember->GetMcuIdx() );			
			bRet = m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_VIDEO, &tChnnl, FALSE);
		}
		else
		{
			byMtId = ptVmpMember->GetMtId();
			bRet = m_ptMtTable->GetMtLogicChnnl(ptVmpMember->GetMtId(), LOGCHL_VIDEO, &tChnnl, FALSE);
		}
		
		if (bRet)
		{
			tVmpCommonAttrb.m_tVideoEncrypt[byLoop] = tChnnl.GetMediaEncrypt();
			tVmpCommonAttrb.m_tDoublePayload[byLoop].SetRealPayLoad(tChnnl.GetChannelType());
			tVmpCommonAttrb.m_tDoublePayload[byLoop].SetActivePayload(tChnnl.GetActivePayload());
			tVmpCommonAttrb.m_abyRcvH264DependInMark[byLoop] = IsRcvH264DependInMark(byMtId);
		}
        else
        {
            //zbq [12/23/2009] 模板开启的画面合成，终端还没呼叫上来，此处给密钥保护提供
            tVmpCommonAttrb.m_tVideoEncrypt[byLoop] = m_tConf.GetMediaKey();
        }
	}

    //开启自动画面合成时无画面合成成员,密钥保护
    if ( !bIsHasMember && bStart && ptVMPParam->IsVMPAuto() )
    {
        tVmpCommonAttrb.m_tVideoEncrypt[0] = m_tConf.GetMediaKey();  
    }

	//need prs or not
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	tVmpCommonAttrb.m_byNeedPrs = tConfAttrb.IsResendLosePack();//yhz-

	//画面合成风格
	u8 byVmpSubType = GetVmpSubType(byVmpId);
    u8 bySchemeId = ptVMPParam->GetVMPSchemeId();
    TVmpStyleCfgInfo tMcuVmpStyle;    
    if(0 == bySchemeId)
    {
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[ChangeVmpParam] SchemeId :%d, use default\n", bySchemeId);
        tMcuVmpStyle.ResetDefaultColor();
        
        //zbq[05/08/2009] 高清VMP 背景默认为黑
        if (VMP != byVmpSubType)
        {
            tMcuVmpStyle.SetBackgroundColor(0);
        }
    }  
    else
    {		
        u8  byVmpStyleNum = 0;
        TVmpAttachCfg atVmpStyle[MAX_VMPSTYLE_NUM];
        if (SUCCESS_AGENT == g_cMcuAgent.ReadVmpAttachTable(&byVmpStyleNum, atVmpStyle) && 
            bySchemeId <= MAX_VMPSTYLE_NUM) // 顾振华 [4/30/2006] 这里应该检查是否越界即可
        {
            //zbq[04/02/2008] 取对应方案号的方案，而不是直接取索引
            u8 byStyleNo = 0;
            for( ; byStyleNo < byVmpStyleNum && byStyleNo < MAX_VMPSTYLE_NUM; byStyleNo ++ )
            {
                if ( bySchemeId == atVmpStyle[byStyleNo].GetIndex() )
                {
                    break;
                }
            }
            if ( byVmpStyleNum == byStyleNo ||
				MAX_VMPSTYLE_NUM == byStyleNo )
            {
                tMcuVmpStyle.ResetDefaultColor();
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[ChangeVmpParam] Get vmp cfg failed! use default(NoIdx)\n");                
            }
            else
            {
                tMcuVmpStyle.SetBackgroundColor(atVmpStyle[byStyleNo].GetBGDColor());
                tMcuVmpStyle.SetFrameColor(atVmpStyle[byStyleNo].GetFrameColor());
                tMcuVmpStyle.SetChairFrameColor(atVmpStyle[byStyleNo].GetChairFrameColor());
                tMcuVmpStyle.SetSpeakerFrameColor(atVmpStyle[byStyleNo].GetSpeakerFrameColor());
                tMcuVmpStyle.SetSchemeId(atVmpStyle[byStyleNo].GetIndex());
                tMcuVmpStyle.SetFontType( atVmpStyle[byStyleNo].GetFontType() );
                tMcuVmpStyle.SetFontSize( atVmpStyle[byStyleNo].GetFontSize() );
                tMcuVmpStyle.SetTextColor( atVmpStyle[byStyleNo].GetTextColor() );
                tMcuVmpStyle.SetTopicBkColor( atVmpStyle[byStyleNo].GetTopicBGDColor() );
                tMcuVmpStyle.SetDiaphaneity( atVmpStyle[byStyleNo].GetDiaphaneity() );
                
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[ChangeVmpParam] get vmpstyle info success, SchemeId :%d\n", bySchemeId);
				
            }
        }
        else
        {
            tMcuVmpStyle.ResetDefaultColor();
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[ChangeVmpParam] Get vmp cfg failed! use default\n");
        }
    }
    ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[ChangeVmpParam] GRDColor.0x%x, AudFrmColor.0x%x, ChairFrmColor.0x%x, SpeakerFrmColor.0x%x\n",
		tMcuVmpStyle.GetBackgroundColor(),
		tMcuVmpStyle.GetFrameColor(),
		
		tMcuVmpStyle.GetChairFrameColor(),
		tMcuVmpStyle.GetSpeakerFrameColor() );
	
    tMcuVmpStyle.SetIsRimEnabled( ptVMPParam->GetIsRimEnabled() );

	memcpy(&tVmpCommonAttrb.m_tVmpStyleCfgInfo, &tMcuVmpStyle, sizeof(tMcuVmpStyle));
	
	//设置画面合成空闲通道的背景 yhz-
	TEqpExCfgInfo tEqpExCfgInfo;
	if( SUCCESS_AGENT != g_cMcuAgent.GetEqpExCfgInfo( tEqpExCfgInfo ) )
	{
		tEqpExCfgInfo.Init();
	}
	tVmpCommonAttrb.m_tVmpExCfgInfo = tEqpExCfgInfo.m_tVMPExCfgInfo;
    LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP,  "[ChangeVmpParam]Set Vmp IdleChlShowMode: %d \n", 
		tVmpCommonAttrb.m_tVmpExCfgInfo.m_byIdleChlShowMode );	

	return tVmpCommonAttrb;
}

/*==============================================================================
函数名    :  ProcStartVmpBatchPollReq
功能      :  处理开始画面合成批量轮询请求
算法实现  :  
参数说明  :  
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-9-7                   薛亮
==============================================================================*/
void CMcuVcInst::ProcStartVmpBatchPollReq(CServMsg &cServMsg)
{
	//检查当前会议的画面合成状态
	if( g_cMcuVcApp.GetVMPMode(m_tVmpEqp) != CONF_VMPMODE_NONE )
	{
		cServMsg.SetErrorCode( ERR_MCU_VMPRESTART );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		
		return;
	}
	
	//FIXME: 暂未对于级联的处理
	TVMPParam tVmpParam = *( TVMPParam* )cServMsg.GetMsgBody();
	m_tVmpBatchPollInfo = *(TVmpBatchPollInfo*)(cServMsg.GetMsgBody()+sizeof(TVMPParam));
	
	if(m_tVmpBatchPollInfo.GetPollNum() == 0 )
	{
		m_tVmpBatchPollInfo.SetLoopNoLimt(TRUE);
	}

	u8 byVmpId;
	u8 byIdleVMPNum = 0;
    u8 abyIdleVMPId[MAXNUM_PERIEQP];
    memset( abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
    g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );
	if( byIdleVMPNum == 0 )
	{
		cServMsg.SetErrorCode( ERR_MCU_NOIDLEVMP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	else
	{
		u16 wError = 0;
		if ( !IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError) )
		{
			cServMsg.SetErrorCode( wError );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
		else if(byVmpId == 0)//表明没有可用于批量轮询的新vmp,只有老vmp
		{
			//wError = ERR_MCU_NOIDLEMPU;
			cServMsg.SetErrorCode( wError );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "No Mpu for VMP Batch Poll!\n");
			return;
		}
	}


	
	//记录当前是要vmp批量轮询,先置位，之后不行再清
	//这里要记录在m_tConf中，因为之后m_tConf作为实参传入IsMCUSupportVmpStyle()
	m_tConf.m_tStatus.m_tVMPParam.SetVMPBatchPoll( 1 );//该值在之后收到VMP->MCU notif后会被间接清掉,所以补充下行代码
	tVmpParam.SetVMPBatchPoll(1);
	
	if(g_bPrintEqpMsg)
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[VmpCommonReq] start vmp param:\n");
		tVmpParam.Print();
	}


	//设置vmp member
	TVMPMember tVMPMember;
	TMt tMt;
	BOOL32 bPollOver	= FALSE;
	BOOL32 bFirst		= TRUE;
	u8 byMtInConfNum    = (u8)m_tConfAllMtInfo.GetAllMtNum();	//与会的终端数
	u8 byCurPolledMtNum = m_tVmpPollParam.GetVmpPolledMtNum();	//当前轮询过的MT数目
	u8 byMtId			= m_tVmpPollParam.GetCurrentIdx() + 1;	//当前轮询到的终端id
	u8 byVmpChnnlIdx	= 0;
	for(; byMtId <= MAXNUM_CONF_MT; byMtId ++)
	{
		if(!m_tConfAllMtInfo.MtInConf( byMtId ))//非与会终端，跳过
		{
			continue;
		}
		if( byVmpChnnlIdx < tVmpParam.GetMaxMemberNum() )
		{
			tMt = m_ptMtTable->GetMt(byMtId);
			tVMPMember.SetMemberTMt( tMt );
			tVMPMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
			tVmpParam.SetVmpMember( byVmpChnnlIdx, tVMPMember );
			m_tVmpPollParam.SetCurPollBlokMtId(byVmpChnnlIdx, byMtId);//记录版面信息
			byVmpChnnlIdx++;
			byCurPolledMtNum++;
			//已经轮到最后一个终端了
			if(byCurPolledMtNum == byMtInConfNum)//第一次poll，可以用数目来判断。
			{
				bPollOver = TRUE; 
				break;
			}
		}
		else //第一版已满
		{
			byVmpChnnlIdx = 0;	//member id重新从0开始
			bFirst = FALSE;		//下次就不是第一版了
			break;
		}
	}
	
	//把以下信息保存到m_tVmpPollParam: 
	m_tVmpPollParam.SetVmpPolledMtNum(byCurPolledMtNum);//已经轮询过的MT数目
	m_tVmpPollParam.SetCurrentIdx(byMtId - 1);			//下次要轮到的MT Id - 1
	m_tVmpPollParam.SetVmpChnnlIdx(byVmpChnnlIdx);		//下次轮询用的vmp channel index
	m_tVmpPollParam.SetIsBatchFirst(bFirst);			//下次是否第一版面
	m_tVmpPollParam.SetIsVmpPollOver(bPollOver);		//设置vmp一轮批量轮询是否结束
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "VMP Batch Poll: CurPolledMtNum is %u,MtId is %u,MemberId is %u\n",\
		byCurPolledMtNum, byMtId, byVmpChnnlIdx);
	
	
	m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	
	//设置画面合成参数
	//这里先将tVMPParam暂存进TPeriEqpStatus中,当收到VMP notify后再存进m_tConf. 
	//注意m_tConf此时已经对m_tVMPParam.m_byVMPBatchPoll赋了值(刚进case处)
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	
	tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_START;//先占用,超时后未成功再放弃
	tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);
	tPeriEqpStatus.SetConfIdx( m_byConfIdx );
	g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	//ACK
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, TIMESPACE_WAIT_VMPRSP);
	
	//开始画面合成 
	//AdjustVmpParam(&tVmpParam, TRUE);vmp批量轮询不支持
	
}

/*==============================================================================
函数名    :  ProcVmpBatchPollTimer
功能      :  VMP 批量轮询定时处理
算法实现  :  
参数说明  :  
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2008-12-22					薛亮							创建
==============================================================================*/
void CMcuVcInst::ProcVmpBatchPollTimer( void )
{
	//设置vmp member
	CServMsg	cMsg;
	TVMPParam	tVMPParam;
	TVMPMember	tVMPMember;
	TMt			tMt;
	u32 dwPollTimes		= m_tVmpBatchPollInfo.GetPollNum();		//轮询次数
	tVMPParam			= m_tConf.m_tStatus.m_tVMPParam;
	u16 wMtInConfNum    = m_tConfAllMtInfo.GetAllMtNum();		//所有受邀的终端数
	u8 byMaxMtIdInConf	= m_tConfAllMtInfo.GetMaxMtIdInConf();	//所有受邀的终端中ID最大值
	u8 byCurPolledMtNum = m_tVmpPollParam.GetVmpPolledMtNum();	//当前轮询过的MT数目
	u8 byMtId			= m_tVmpPollParam.GetCurrentIdx()+1;	//当前轮询到的终端id
	u8 byMemberId		= m_tVmpPollParam.GetVmpChnnlIdx();		//member id
	BOOL32 bPollOver	= m_tVmpPollParam.IsVmpPollOver();		//一轮poll是否结束
	BOOL32 bFirst		= m_tVmpPollParam.IsBatchFirst();		//是否还是第一版
	BOOL32 bLoopNoLimit	= m_tVmpBatchPollInfo.IsLoopNoLimit();	//是否无限轮询
	
	//【注】如果之前第一版未满，之后又加了几个终端.这种情况bFirst=FALSE， 定时以t算
	
	if(bPollOver) //一轮轮询结束，轮询次数-1
	{
		if(bLoopNoLimit) 
		{
			//新一轮poll
			byMtId = 1; //Mt id，member id 都从头开始
			byMemberId = 0;
		}
		else
		{
			dwPollTimes --;
			if(dwPollTimes == 0)
			{
				//整个轮询结束，停止vmp
				cMsg.SetEventId(MCS_MCU_STOPVMP_REQ);
				cMsg.SetMsgBody();
				g_cMcuVcApp.SendMsgToConf(cMsg.GetConfId(),cMsg.GetEventId());
				
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcVmpBatchPollTimer]Batch Poll times is full, stop VMP!\n");
				
				return;
			}
			else
			{
				//新一轮poll
				byMtId = 1;				//Mt id，member id 都从头开始
				byMemberId = 0;
				byCurPolledMtNum = 0;	// 当前轮询过的MT数目清0
				bPollOver = FALSE;
			}
		}
	}
	//FIXME:待优化，目前统一处理
	//优化：对于只有第一版的情况，若当前与会终端数和轮询过的终端数目不吻合，表明有增删终端的发生，
	//这时要重新设置第一版的参数；反之，只需修改轮询次数即可，不用重设参数,发消息changeVmpParam
	// 	if( bFirst && wMtInConfNum == byCurPolledMtNum )
	// 	{
	// 		byMtId ++;
	// 		bPollOver = FALSE;
	// 		if(byMtId > )
	// 		m_tVmpBatchPollInfo.SetPollNum(dwPollTimes);
	// 		//之后定时t,而非t*PolledMtNum，也非T
	// 		return;
	// 	}
	//	else
	//	{
	while(byMtId <= MAXNUM_CONF_MT)
	{
		if(!m_tConfAllMtInfo.MtInConf( byMtId ))//非受邀终端，跳过
		{
			byMtId++;
			continue;
		}
		if(!m_tConfAllMtInfo.MtJoinedConf( byMtId ))//受邀但非与会MT
		{
			//定时时间要调整----1 ms
			m_tVmpBatchPollInfo.SetTmpt(1);
		}
		if( byMemberId < tVMPParam.GetMaxMemberNum() )
		{
			tMt = m_ptMtTable->GetMt(byMtId);
			tVMPMember.SetMemberTMt( tMt );
			tVMPMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
			tVMPParam.SetVmpMember( byMemberId, tVMPMember );
			m_tVmpPollParam.SetCurPollBlokMtId(byMemberId,byMtId); //记录版面信息
			bFirst = FALSE; //不管可视版面还停留在第一版与否，均置FALSE
			byMtId++;
			byCurPolledMtNum++;
			
			if (++byMemberId == tVMPParam.GetMaxMemberNum()) //本版已满，下次member id从0开始
			{
				byMemberId = 0;
			}
			//不能通过判断数目来确定是否是一次轮询结束，在某些增删MT的场合会误判，
			//这里通过比较下一次要轮询的MT Id和会议受邀终端中最大Mt Id来确定。
			if( byMtId > byMaxMtIdInConf )//已经轮到最后一个终端了
			{
				bPollOver = TRUE; //一轮批量轮询over
			}
		}
		else
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcVmpBatchPollTimer]invalid VMP channel!\n");
		}
		break;
	}
	//	}
	
	//把以下信息保存到m_tVmpPollParam: 
	m_tVmpPollParam.SetVmpPolledMtNum(byCurPolledMtNum);//当前轮询过的MT数目
	m_tVmpPollParam.SetCurrentIdx(byMtId - 1);			//下次要轮到的MT Id，此处要减1
	m_tVmpPollParam.SetVmpChnnlIdx(byMemberId);			//vmp member Id
	m_tVmpPollParam.SetIsBatchFirst(bFirst);			//当前是否还是第一版面
	m_tVmpPollParam.SetIsVmpPollOver(bPollOver);		//设置vmp一轮批量轮询是否结束
	
	m_tVmpBatchPollInfo.SetPollNum(dwPollTimes);		//轮询次数
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcVmpBatchPollTimer]VMP Batch Poll: CurPolledMtNum is %u,MtId is %u,MemberId is %u,Left Poll times is%u\n",\
		byCurPolledMtNum, byMtId, byMemberId, dwPollTimes);
	
	cMsg.SetEventId(MCS_MCU_CHANGEVMPPARAM_REQ);
	cMsg.SetMsgBody((u8 *)&tVMPParam,sizeof(tVMPParam));
	g_cMcuVcApp.SendMsgToConf(cMsg.GetConfId(),cMsg.GetEventId(),
		cMsg.GetServMsg(),cMsg.GetServMsgLen());
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcVmpBatchPollTimer] Send msg MCS_MCU_CHANGEVMPPARAM_REQ to conf.\n");
	
	//FIXME；如果vmp批量轮询已经轮询到最后一个且轮询次数已满，则不再定时轮询
	// 	u32 dwPollt = m_tVmpBatchPollInfo.m_dwPollt;
	// 	SetTimer(MCUVC_VMPBATCHPOLL_TIMER,dwPollt);
}

/*==============================================================================
函数名    :  ProcMcsMcuVmpCmd
功能      :  处理mcs给mcu的vmp 命令
算法实现  :  
参数说明  :  
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2008-12-24					薛亮							创建
==============================================================================*/
void CMcuVcInst::ProcMcsMcuVmpCmd(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);	
    //TPollInfo tPollInfo;
	
    STATECHECK;
	
    switch(pcMsg->event)
    {
	case MCS_MCU_PAUSE_VMPBATCHPOLL_CMD:
		{
			//一个会议是否只允许一种轮询存在，不是的话FIXME
			m_tConf.m_tStatus.SetPollState(POLL_STATE_PAUSE);
			cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
			SendMsgToAllMcs(MCU_MCS_POLLSTATE_NOTIF, cServMsg);
			
			ConfStatusChange();
			
			KillTimer(MCUVC_VMPBATCHPOLL_TIMER);

			//

			break;
		}
	case MCS_MCU_RESUME_VMPBATCHPOLL_CMD:	//Resume
		{
			//判断是否当前整版的MT都被删除
			BOOL32 bAllMtInCurBlokDeled = TRUE;  
			TVMPParam tVMPParam;
			tVMPParam		= m_tConf.m_tStatus.m_tVMPParam;
			u8 byMaxMtId	= 0;	//整版删除的Mt中Id最大者  
			u8 byMemberId	= 0;
			for ( ; byMemberId < tVMPParam.GetMaxMemberNum(); byMemberId ++)
			{
				u8 byMtId = m_tVmpPollParam.GetMtInCurPollBlok(byMemberId);
				if(m_tConfAllMtInfo.MtInConf(byMtId))
				{
					bAllMtInCurBlokDeled = FALSE;
					break;
				}
				if(byMtId > byMaxMtId)
				{
					byMaxMtId = byMtId;
				}
			}
			//出现整版Mt删除的情况,修改下次要轮的MTId和memberId
			if(bAllMtInCurBlokDeled)
			{
				m_tVmpPollParam.SetCurrentIdx(byMaxMtId-1);
				m_tVmpPollParam.SetVmpChnnlIdx(0);
			}

			m_tConf.m_tStatus.SetPollState(POLL_STATE_NORMAL);
			cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
			SendMsgToAllMcs(MCU_MCS_POLLSTATE_NOTIF, cServMsg);
			
			ConfStatusChange();
			
			u32 dwTimert = m_tVmpBatchPollInfo.GetPollt();
			SetTimer(MCUVC_VMPBATCHPOLL_TIMER, dwTimert);

			break;
		}
	case MCS_MCU_STOP_VMPBATCHPOLL_CMD:
		{
			cServMsg.SetNoSrc();
			cServMsg.SetEventId(MCS_MCU_STOPVMP_REQ);
			cServMsg.SetMsgBody();
			g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(),cServMsg.GetEventId());
			
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMcsMcuVmpCmd]VMP Batch Poll stops!\n");
			break;
		}
	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[ProcMcsMcuVmpCmd]Unexpected Message event is received!\n");
		break;
	}
}

/*==============================================================================
函数名    :  ClearOneVmpMember
功能      :  将VMP中某个通道的成员清掉
算法实现  :  该通道成员即使为空，但有成员类型也要清
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-4-2	4.6				薛亮							create
==============================================================================*/
void CMcuVcInst::ClearOneVmpMember(u8 byVmpId, u8 byChnnl, TVMPParam_25Mem &tVmpParam, BOOL32 bRecover)
{
	// 加保护,byChnnl为非正常通道时,返回
	if (byChnnl >= MAXNUM_VMP_MEMBER || !IsValidVmpId(byVmpId))
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_VMP, "[ClearOneVmpMember] byChnnl:%d not available for vmp[%d].\n", byChnnl, byVmpId);
		return;
	}
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[ClearOneVmpMember] byChnnl:%d,bReCover:%d.\n", byChnnl,bRecover);
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );

	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	TVMPParam_25Mem tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
	TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
	TVMPMember tVmpMember = *tConfVmpParam.GetVmpMember(byChnnl);
	tConfVmpParam.ClearVmpMember(byChnnl);
	tVmpParam.ClearVmpMember(byChnnl);
	TVMPMember *ptLastVmpMember = tLastVmpParam.GetVmpMember(byChnnl);
	// 双流跟随通道停双流交换
	u8 byMode = MODE_VIDEO;
	if (VMP_MEMBERTYPE_DSTREAM == tVmpMember.GetMemberType())
	{
		byMode = MODE_SECVIDEO;
	}

	// 清通道不调vmp外设时,需刷新外设通道台标
	s8 achAlias[VALIDLEN_ALIAS+1] = {0};
	TMt tTmpMt;
	CServMsg cServMsg;

	if(!ptLastVmpMember->IsNull())
	{
		if(bRecover)
		{
			//恢复到上一次的VMP param，交换都不变
			tVmpParam.SetVmpMember(byChnnl, *ptLastVmpMember);
			tTmpMt = *ptLastVmpMember;
		}
		else
		{
			//清交换
			StopSwitchToPeriEqp(tVmpEqp.GetEqpId(), byChnnl, FALSE, byMode);
			tVmpChnnlInfo.ClearChnlByVmpPos(byChnnl); //清该Mt对应的HD通道(如果存在对应通道)
			g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
			// 将此终端踢出vmp后,释放一路多回传带宽,只针对非双流跟随通道
			if (!tVmpMember.IsNull() && !tVmpMember.IsLocal() && MODE_VIDEO == byMode)
			{
				FreeRecvSpy( tVmpMember, MODE_VIDEO );
			}
			tLastVmpParam.ClearVmpMember(byChnnl);
			g_cMcuVcApp.SetLastVmpParam(tVmpEqp, tLastVmpParam);

			//告诉外设清除该通道,MCU_VMP_ADDREMOVECHNNL_CMD该消息目前仅用于清某通道,若add需用adjustVmpParam下参
			cServMsg.SetChnIndex(byChnnl);
			u8 byTmp = 0XFF;//此字段保留,因旧mpu有做处理,为兼容,另开一个字节标识关闭通道
			u8 byAdd = 0;//标识关闭通道
			cServMsg.SetMsgBody( &byTmp, sizeof(byTmp) );
			cServMsg.CatMsgBody( &byAdd, sizeof(byAdd) );
			SendMsgToEqp( tVmpEqp.GetMtId(), MCU_VMP_ADDREMOVECHNNL_CMD, cServMsg);
		}
	}

	/* 刷新外设通道台标,vpu与mpu不需发送别名信息
	u8 byVmpSubType = GetVmpSubType(tVmpEqp.GetEqpId());
	if (byVmpSubType != VMP && byVmpSubType != MPU_SVMP)
	{
		cServMsg.Init();
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ClearOneVmpMember]vmp member.%d info: mt(%d,%d), alias is %s!\n", 
			byChnnl, tTmpMt.GetMcuId(), tTmpMt.GetMtId(), achAlias);
		if (GetMtAliasInVmp(tTmpMt, achAlias))
		{
			cServMsg.SetMsgBody( (u8*)achAlias, sizeof(achAlias) );
		}
		cServMsg.SetChnIndex(byChnnl);
		SendMsgToEqp(tVmpEqp.GetEqpId(), MCU_VMP_CHANGEMEMALIAS_CMD, cServMsg);
	}*/

	//m_tConf.m_tStatus.SetVmpParam(tVmpParam);
	g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tConfVmpParam);
	cServMsg.Init();
	
	//告诉会控刷界面
	cServMsg.SetEqpId(tVmpEqp.GetEqpId());
	cServMsg.SetMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
	SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );	

	// xliang [4/10/2009] 同时更新tPeriEqpStatus，mcs要依据此刷MT图标,而不是依据TMtStatus :(
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tConfVmpParam);
	g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	/*cServMsg.SetMsgBody();
	cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
	SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );*/

	return;
}


/*==============================================================================
    函数名    :  ProcMcuMcuSpeakModeNtf
    功能      :  上级发下来的发言人发言模式的通知
    算法实现  :  
    参数说明  :  
    返回值说明:  
 -------------------------------------------------------------------------------
    修改记录  :  
    日  期       版本          修改人          走读人          修改记录
    2010-5-12	 4.6		   张宝卿							create
==============================================================================*/
/*void CMcuVcInst::ProcMcuMcuSpeakModeNtf( const CMessage *pcMsg )
{
	STATECHECK;

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    u32 dwStatus = *(u32*)cServMsg.GetMsgBody();

	emConfSpeakMode emMode = (emConfSpeakMode)dwStatus;

	if( emSpeakNormal != emMode &&
		emAnswerInstantly != emMode )
	{
		ConfLog( FALSE,"[ProcMcuMcuSpeakModeNtf] Wrong Speak Mode.%d \n",dwStatus );
		return;
	}
	u8 byMode = CONF_SPEAKMODE_NORMAL;
	switch( dwStatus )
	{
	case emSpeakNormal:
		byMode = CONF_SPEAKMODE_NORMAL;
		break;
	case emAnswerInstantly:
		byMode = CONF_SPEAKMODE_ANSWERINSTANTLY;
		break;
	default:
		break;
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuSpeakModeNtf] Notify Speak Mode.%d\n Event(%s)",
		byMode,OspEventDesc( cServMsg.GetEventId() ) );

	if( VCS_CONF == m_tConf.GetConfSource() )
	{
		m_cVCSConfStatus.SetConfSpeakMode( byMode );
	}


	//通知终端刷新MTC-BOX
    for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
    {
        if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
        {
            continue;
        }
		if( MT_TYPE_MT == m_ptMtTable->GetMtType(byMtId) )
		{
			NotiyfMtConfSpeakMode(byMtId, byMode);
		}
    }
}*/

/*==============================================================================
    函数名    :  ProcMcsMcuChangeSpeakModeReq
    功能      :  MCS重新设置会议模式
    算法实现  :  
    参数说明  :  
    返回值说明:  
 -------------------------------------------------------------------------------
    修改记录  :  
    日  期       版本          修改人          走读人          修改记录
    2010-5-12	 4.6		   张宝卿							create
==============================================================================*/
void CMcuVcInst::ProcMcsMcuChangeSpeakModeReq(const CMessage * pcMsg)
{
    STATECHECK;

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    u8 byMode = *(u8*)cServMsg.GetMsgBody();

    u8 byOldMode = m_tConf.GetConfSpeakMode();
    switch (byMode)
    {
    case CONF_SPEAKMODE_NORMAL:
    case CONF_SPEAKMODE_ANSWERINSTANTLY:
        
    	break;
    
    default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[ProcMcsMcuChangeSpeakModeReq] Wrong Mode Value.%d!\n",byMode );		
        return;
    }

	SendReplyBack(cServMsg, pcMsg->event + 1);

	

	if( cServMsg.GetEventId() == MCS_MCU_CHGSPEAKMODE_REQ )
	{
		//刷新申请发言/抢答列表
		if ( byOldMode != byMode )
		{
			m_tApplySpeakQue.ClearQueue();
			NotifyMcsApplyList( TRUE );
		}
	}

	if (VCS_CONF == m_tConf.GetConfSource())
    {
        m_cVCSConfStatus.SetConfSpeakMode(byMode);
    }
    m_tConf.SetConfSpeakMode(byMode);

	if( VCS_CONF == m_tConf.GetConfSource() &&
			m_byCreateBy != CONF_CREATE_MT )
	{
		//通知UI
		if( CONF_SPEAKMODE_ANSWERINSTANTLY == byMode &&
			!( m_tConf.GetSpeaker() == m_tConf.GetChairman() ) 
			)
		{
			TMt tMt;
			tMt.SetNull();
			ChgCurVCMT( tMt );
		}
		else
		{
			VCSConfStatusNotif();
		}
	}

    //通知终端刷新MTC-BOX
    for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
    {
        if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
        {
            continue;
        }
		if( MT_TYPE_MT == m_ptMtTable->GetMtType(byMtId) )
		{
			NotiyfMtConfSpeakMode(byMtId, byMode);
		}
		else if( MT_TYPE_SMCU == m_ptMtTable->GetMtType(byMtId) )
		{
			//NotiyfMtConfSpeakMode( byMtId,byMode,FALSE,MCU_MCU_SPEAKERMODE_NOTIFY );
			cServMsg.SetEventId( MCU_MCU_SPEAKERMODE_NOTIFY );
			SendMsgToMt( byMtId,MCU_MCU_SPEAKERMODE_NOTIFY,cServMsg );
		}

    }

    return;
}

/*==============================================================================
    函数名    :  ProcMcsMcuGetSpeakListReq
    功能      :  
    算法实现  :  
    参数说明  :  
    返回值说明:  
 -------------------------------------------------------------------------------
    修改记录  :  
    日  期       版本          修改人          走读人          修改记录
    2010-5-12	 4.6		   张宝卿							create
==============================================================================*/
void CMcuVcInst::ProcMcsMcuGetSpeakListReq(const CMessage * pcMsg)
{
	STATECHECK;

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	u8 byLen = MAXNUM_CONF_MT;
    TMt atMt[MAXNUM_CONF_MT];
    m_tApplySpeakQue.GetQueueList(atMt, byLen);

    cServMsg.SetMsgBody((u8*)&byLen, sizeof(u8));
	if (0 != byLen)
	{
		cServMsg.CatMsgBody((u8*)atMt, sizeof(TMt) * byLen);
	}    
    SendReplyBack(cServMsg, pcMsg->event + 1);
    
    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuGetSpeakListReq] MCU_MCS_MTAPPLYSPEAKERLIST_ACK with Len.%d!\n", byLen);


	return;
}

/*====================================================================
    函数名      ProcMcsMcuGetIFrameCmd
    功能        ：界面向MCU请求关键帧处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	10/11/12     4.6          薛亮          创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetIFrameCmd(const CMessage *pcMsg)
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt tMt = *(TMt *)cServMsg.GetMsgBody();
	u8 byMode = *(u8*) (cServMsg.GetMsgBody() + sizeof(TMt));

	if( tMt.IsNull() )
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcsMcuGetIFrameCmd]tMt(%d,%d) is invalid!\n", tMt.GetMcuId(), tMt.GetMtId());
		return;
	}

	// 支持请求外设Vmp关键帧
	if (tMt.GetType() == TYPE_MCUPERI && EQP_TYPE_VMP == tMt.GetEqpType())
	{
		// MCS监控选看画面合成时,会向mcu请求vmp的关键帧,需根据当前会议获得相应通道
		TVideoStreamCap tStrCap = m_tConf.GetMainSimCapSet().GetVideoCap();
		u8 bySrcChnnl = GetVmpOutChnnlByRes(tMt.GetEqpId(),
			tStrCap.GetResolution(), 
			tStrCap.GetMediaType(),
			tStrCap.GetUserDefFrameRate(),
			tStrCap.GetH264ProfileAttrb());
		if ( 0xFF == bySrcChnnl ) //没找到对应的通道
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ProcMcsMcuGetIFrameCmd] Get MCS vmp outchannl failed \n");
			return;
		}
		
		// 8kh有特殊场景
		if (VMP_8KH == GetVmpSubType(tMt.GetEqpId()))
		{
			// 会议是1080p30fpsBP适配会议或720p30fpsHP适配会议时,为避免vmp广播非广播切换时通道改变,取1通道
			if (IsConfExcludeDDSUseAdapt())
			{
				if (VIDEO_FORMAT_HD1080 == tStrCap.GetResolution() && 
					tStrCap.GetUserDefFrameRate() < 50 &&
					tStrCap.GetH264ProfileAttrb() == emBpAttrb)
				{
					if (IsMSSupportCapEx(VIDEO_FORMAT_HD720))
					{
						bySrcChnnl = 1;
					}
					else if (IsMSSupportCapEx(VIDEO_FORMAT_4CIF))
					{
						bySrcChnnl = 2;
					}
					else if (IsMSSupportCapEx(VIDEO_FORMAT_CIF))
					{
						bySrcChnnl = 3;
					}
				}
				if (VIDEO_FORMAT_HD720 == tStrCap.GetResolution() && 
					tStrCap.GetUserDefFrameRate() < 50 &&
					tStrCap.GetH264ProfileAttrb() == emHpAttrb)
				{
					if (IsMSSupportCapEx(VIDEO_FORMAT_HD720, tStrCap.GetUserDefFrameRate(), emBpAttrb))
					{
						bySrcChnnl = 1;
					}
					else if (IsMSSupportCapEx(VIDEO_FORMAT_4CIF))
					{
						bySrcChnnl = 2;
					}
					else if (IsMSSupportCapEx(VIDEO_FORMAT_CIF))
					{
						bySrcChnnl = 3;
					}
				}
			}
		}
		// 8ki有特殊场景
		else if (VMP_8KI == GetVmpSubType(tMt.GetEqpId()))
		{
			// 会议是1080p30fps或720p60fps适配会议时,为避免vmp广播非广播切换时通道改变,取1通道
			if (IsConfExcludeDDSUseAdapt()) //适配会议
			{
				if ((m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 && m_tConf.GetMainVidUsrDefFPS() <= 30) ||
					(m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD720 && m_tConf.GetMainVidUsrDefFPS() > 30))
				{
					TKDVVMPOutParam tVMPOutParam = g_cMcuVcApp.GetVMPOutParam(tMt);
					TVideoStreamCap tTmpStrCap = tVMPOutParam.GetVmpOutCapIdx(1);
					// 720/4cif/cif都未勾选时,是不允许非广播的,仍取第0路
					if (MEDIA_TYPE_H264 == tTmpStrCap.GetMediaType())
					{
						bySrcChnnl = 1;
					}
					else
					{
						bySrcChnnl = 0;
					}
				}
			}
		}

		byMode = bySrcChnnl;
	}
	else if( byMode != MODE_VIDEO && byMode != MODE_SECVIDEO)
	{
		ConfPrint( LOG_LVL_WARNING ,MID_MCU_CONF, "[ProcMcsMcuGetIFrameCmd]mode.%d is invalid!\n", byMode);
		return;
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuGetIFrameCmd]begin to call NotifyFastUpdate to tMt(%d,%d)\n", tMt.GetMcuId(), tMt.GetMtId());
	
	NotifyFastUpdate(tMt, byMode);

}

/*====================================================================
    函数名       ProcBatchMtSmsOpr
    功能        ：批量处理短消息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CServMsg &cServMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/01/11     4.6          薛亮          创建
====================================================================*/
void CMcuVcInst::ProcBatchMtSmsOpr(const CServMsg &cServMsg)
{
	u16 wMtNum = *(u16*)cServMsg.GetMsgBody(); 
	wMtNum  = ntohs(wMtNum);
	CRollMsg* ptROLLMSG = (CRollMsg*)( cServMsg.GetMsgBody() + wMtNum*sizeof(TMt) + sizeof(u16) );
	
	u16 wPos = m_cSmsControl.GetMtPos();
	TMt	*ptMt = (TMt *)( cServMsg.GetMsgBody() + sizeof(u16) + (sizeof(TMt) * wPos) );
	u8 byPackNum = 0;
	for( ;  wPos < wMtNum; wPos++ )
	{
		// 每处理100个算一批
		if( ++byPackNum == 101 )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "SetTimer MCUVC_SMSPACK_TIMER for %d(ms)!!\n", m_cSmsControl.GetTimerSpan());
			m_cSmsControl.SetMtPos(wPos);
			SetTimer(MCUVC_SMSPACK_TIMER, m_cSmsControl.GetTimerSpan());
			return;
		}
		ProcSingleMtSmsOpr(cServMsg, ptMt, ptROLLMSG);
		ptMt++;
	}
	
	m_cSmsControl.Init();

}

/*====================================================================
    函数名       ProcSmsPackTimer
    功能        ：批量处理短消息定时处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage *pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/01/11     4.6          薛亮          创建
====================================================================*/
void CMcuVcInst::ProcSmsPackTimer( void )
{

	//u8 bySrcMtId  = (u8)(*((u32*)pcMsg->content));
	
	CServMsg	*pcServMsg = m_cSmsControl.GetServMsg();

	ProcBatchMtSmsOpr(*pcServMsg);

}

/*====================================================================
函数名      ：ProcMcsMcuTransparentMsgNotify
功能        ：处理界面、终端消息透传
算法实现    ：
引用全局变量：
输入参数说明：const CMessage * pcMsg, 传入的消息 
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2/23/2012               朱胜泽          创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuTransparentMsgNotify( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    //目的终端数
    u16 wMtNum = ntohs(*( u16* )cServMsg.GetMsgBody());
    //目的终端
    TMt *ptDstMt = ( TMt* )( cServMsg.GetMsgBody() + sizeof(u16) );
    //界面、终端协商消息体
    u8 *pbyMsgContent = ( u8* )( cServMsg.GetMsgBody() + sizeof(u16) + sizeof(TMt)*wMtNum );
    //界面、终端协商消息体长度
    u16 wMsgLen = cServMsg.GetMsgBodyLen() - sizeof(u16) - sizeof(TMt)*wMtNum;
    OspPrintf( TRUE, FALSE, "[ProcMcsMcuTransparentMsgNotify] mcs send to %d Mts msg size is %d!\n", wMtNum, wMsgLen);
    
    CServMsg cMsg;
    cMsg.SetSrcSsnId(cServMsg.GetSrcSsnId()/*m_cVCSConfStatus.GetCurSrcSsnId()*/);
    cMsg.SetConfIdx(m_byConfIdx);
    cMsg.SetConfId(m_tConf.GetConfId());
    
    if ( wMtNum > 0 )
    {
        for ( u16 wLoop = 0; wLoop < wMtNum; wLoop++)
        {
            if ( ptDstMt == NULL )
            {
				break;
            }
            
            if ( ptDstMt->IsLocal() )
            {//本地直属终端
                
                if ( !m_tConfAllMtInfo.MtJoinedConf( ptDstMt->GetMtId() ) )
                {//终端未入会
                    
                    ConfPrint(LOG_LVL_WARNING, MID_MCU_MCS,"[ProcMcsMcuTransparentMsgNotify]MT:%d not joined conf !\n", ptDstMt->GetMtId());
                    ptDstMt++;
                    continue;
                }
                
                cMsg.SetEventId(MCU_MT_TRANSPARENTMSG_NOTIFY);
                cMsg.SetMsgBody((u8*)pbyMsgContent, wMsgLen);
                BOOL32 bRet = SendMsgToMt(ptDstMt->GetMtId(), cMsg.GetEventId(), cMsg);
                OspPrintf( TRUE, FALSE, "[ProcMcsMcuTransparentMsgNotify]send to mt msg size is %d\n", cMsg.GetMsgBodyLen());
                if (bRet != 1)
                {
                    ConfPrint(LOG_LVL_WARNING, MID_MCU_MCS,"MCU_MT_TRANSPARENTMSG_NOTIFY send failed!\n");
                }
                
            }
            else
            {//非本地终端
                
                TMsgHeadMsg tHeadMsg;
                TMt tSMcu;
                tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( *ptDstMt, tSMcu);
                
                cMsg.SetEventId(MCU_MCU_TRANSPARENTMSG_NOTIFY);
                cMsg.SetMsgBody((u8*)&tHeadMsg, sizeof(tHeadMsg));
                cMsg.CatMsgBody((u8*)&tSMcu, sizeof(TMt));
                cMsg.CatMsgBody((u8*)pbyMsgContent, wMsgLen);
                SendMsgToMt((u8)tSMcu.GetMcuId(), cMsg.GetEventId(), cMsg);
            }
            ptDstMt++;
        }
    }
    else
    {
        ConfPrint(LOG_LVL_WARNING, MID_MCU_MCS,"[ProcMcsMcuTransparentMsgNotify] Dst mt num.%d is illegal!\n", wMtNum);
    }
    
    return;
}
/*====================================================================
    函数名      : MCSFindMtInTvWallAndChangeSwitch
    功能        ：停止或恢复终端到电视墙地的交换
    算法实现    ：
    引用全局变量：
    输入参数说明： 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                田志勇          创建
====================================================================*/
void CMcuVcInst::MCSFindMtInTvWallAndChangeSwitch( TMt tMt,BOOL32 bIsStopSwitch,u8 byMode )
{
	//此函数现在只支持拆建视频或音频，不能两个都支持
	if( tMt.IsNull() || MODE_BOTH == byMode )
	{
		return;
	}
	TPeriEqpStatus tPeriStatus;
	TMt tInTwOrHduChnMt;
	BOOL32 bIsCanChangeSwitch = FALSE;
	u8 byTwOrHduChnNum = 0;
	//循环遍历所有DEC5电视墙通道
	for (u8 byTwIdx = TVWALLID_MIN; byTwIdx <= TVWALLID_MAX; byTwIdx++)
	{
		if (!g_cMcuVcApp.IsPeriEqpValid(byTwIdx) || !g_cMcuVcApp.GetPeriEqpStatus(byTwIdx, &tPeriStatus)
			|| !tPeriStatus.m_byOnline)
		{
			BOOL32 bInvalidEqp = !g_cMcuVcApp.IsPeriEqpValid(byTwIdx);
			BOOL32 bInvalidEqpStatus = !g_cMcuVcApp.GetPeriEqpStatus(byTwIdx, &tPeriStatus);
			BOOL32 bOffline = !tPeriStatus.m_byOnline;

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[MCSFindMtInTvWallAndChangeSwitch]Tw(%d) IsValid(%d) GetStatus(%d) OnLine(%d),So Continue!\n",byTwIdx,
				bInvalidEqp,bInvalidEqpStatus,bOffline);
			continue;
		}
		byTwOrHduChnNum = tPeriStatus.m_tStatus.tTvWall.byChnnlNum;
		for (u8 byChnIdx = 0; byChnIdx < min(byTwOrHduChnNum,MAXNUM_PERIEQP_CHNNL); byChnIdx++)
		{
			bIsCanChangeSwitch = FALSE;
			tInTwOrHduChnMt = tPeriStatus.m_tStatus.tTvWall.atVideoMt[byChnIdx];
			if (tInTwOrHduChnMt.IsNull())
			{
				continue;
			}
			if (tMt == tInTwOrHduChnMt || IsMtInMcu(tMt,tInTwOrHduChnMt))
			{
				bIsCanChangeSwitch = TRUE;
			} 
			if (bIsCanChangeSwitch)
			{
				if( bIsStopSwitch )
				{
					StopSwitchToPeriEqp( byTwIdx, byChnIdx, FALSE, byMode );
				}
				else
				{
					StartSwitchToPeriEqp( tInTwOrHduChnMt, 0, byTwIdx, byChnIdx, byMode,SWITCH_MODE_SELECT,FALSE,TRUE,FALSE,TRUE,FALSE);
				}
			}
		}
	}

	//循环遍历所有HDU电视墙通道
	for (u8 byHduIdx = HDUID_MIN; byHduIdx <= HDUID_MAX; byHduIdx++)
	{
		if (!g_cMcuVcApp.IsPeriEqpValid(byHduIdx) || !g_cMcuVcApp.GetPeriEqpStatus(byHduIdx, &tPeriStatus)
			|| !tPeriStatus.m_byOnline)
		{
			BOOL32 bInvalidEqp = !g_cMcuVcApp.IsPeriEqpValid(byHduIdx);
			BOOL32 bInvalidEqpStatus = !g_cMcuVcApp.GetPeriEqpStatus(byHduIdx, &tPeriStatus);
			BOOL32 bOffline = !tPeriStatus.m_byOnline;
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[MCSFindMtInTvWallAndChangeSwitch]Hdu(%d) IsValid(%d) GetStatus(%d) OnLine(%d),So Continue!\n",byHduIdx,
				bInvalidEqp,bInvalidEqpStatus,bOffline);
			continue;
		}
		byTwOrHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byHduIdx));
		for (u8 byChnIdx = 0; byChnIdx < min(byTwOrHduChnNum,MAXNUM_HDU_CHANNEL); byChnIdx++)
		{
			bIsCanChangeSwitch = FALSE;
			tInTwOrHduChnMt = tPeriStatus.m_tStatus.tHdu.atVideoMt[byChnIdx];
			if (tInTwOrHduChnMt.IsNull())
			{
				continue;
			}
			if (tMt == tInTwOrHduChnMt || IsMtInMcu(tMt,tInTwOrHduChnMt))
			{
				bIsCanChangeSwitch = TRUE;
			} 
			if (bIsCanChangeSwitch)
			{
				if( bIsStopSwitch )
				{
					StopSwitchToPeriEqp( byHduIdx, byChnIdx, FALSE, byMode );
				}
				else
				{
					StartSwitchToPeriEqp( tInTwOrHduChnMt, 0, byHduIdx, byChnIdx, byMode, SWITCH_MODE_SELECT,FALSE,TRUE,FALSE,TRUE,FALSE);
				}
			}
		}
	}
}
/*====================================================================
    函数名      : ClearHduSchemInfo
    功能        ：清理HDU预案相关信息
    算法实现    ：
    引用全局变量：
    输入参数说明： 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    12/08/31                田志勇          创建
====================================================================*/
void CMcuVcInst::ClearHduSchemInfo()
{
    if( m_tConf.GetConfAttrb().IsHasTvWallModule() )
    {
		CServMsg cServMsgSend;
        TMultiTvWallModule tMultiTvWallModule;
        m_tConfEqpModule.GetMultiTvWallModule( tMultiTvWallModule );
        TTvWallModule tTvWallModule;
		THduChnlInfo  atHduChnlInfo[MAXNUM_TVWALL_CHNNL_INSMOUDLE];
		TPeriEqpStatus tHduStatus;
        for( u8 byTvLp = 0; byTvLp < tMultiTvWallModule.GetTvModuleNum(); byTvLp++ )
        {
            tMultiTvWallModule.GetTvModuleByIdx(byTvLp, tTvWallModule);
			u8 byTvWallId = tTvWallModule.GetTvEqp().GetEqpId();
			u8 byChnIdx = 0;
			u8 byHduSchemeNum = 0;
			THduStyleInfo atHduStyleInfoTable[MAX_HDUSTYLE_NUM];
			u16 wRet = g_cMcuAgent.ReadHduSchemeTable(&byHduSchemeNum, atHduStyleInfoTable);
			if (SUCCESS_AGENT != wRet)
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ClearHduSchemInfo] ReadHduSchemeTable failed!\n");
			}
			else
			{
				for (u8 byStyleIdx = 0 ; byStyleIdx < byHduSchemeNum; byStyleIdx ++)
				{
					if (atHduStyleInfoTable[byStyleIdx].GetStyleIdx() == byTvWallId)
					{
						atHduStyleInfoTable[byStyleIdx].GetHduChnlTable(atHduChnlInfo);
						for (u8 byChnl = 0; byChnl < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byChnl++)
						{   
							if (!g_cMcuVcApp.IsPeriEqpValid( atHduChnlInfo[byChnl].GetEqpId() ) ||
								!g_cMcuVcApp.GetPeriEqpStatus(atHduChnlInfo[byChnl].GetEqpId(), &tHduStatus))
							{
								ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU, "[ClearHduSchemInfo]fail to get status(hduid:%d)\n", atHduChnlInfo[byChnl].GetEqpId());
								continue;
							}
							byChnIdx = atHduChnlInfo[byChnl].GetChnlIdx();
							if (tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].GetConfIdx() == m_byConfIdx)
							{
								tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].byMemberType = 0;
								tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].SetNull();
								tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].SetConfIdx(0);
								g_cMcuVcApp.SetPeriEqpStatus(atHduChnlInfo[byChnl].GetEqpId(), &tHduStatus);
								cServMsgSend.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
								SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsgSend);
							}
						}
					}
				}
			}      
        }
    }
}

/*====================================================================
    函数名      : IsCircleScheduleConfNeedStart
    功能        ：周期性预约会议是否需要开启即时会议
    算法实现    ：
    引用全局变量：
    输入参数说明： 
    返回值说明  ：TRUE：需要开启即时还以，FALSE：不开启
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/02/21                周翼亮          创建
====================================================================*/
BOOL32 CMcuVcInst::IsCircleScheduleConfNeedStart( )
{
	if ( !m_tConf.IsCircleScheduleConf() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[IsCircleScheduleConfNeedStart]conf:%s is not a circlescheduleconf!\n",m_tConf.GetConfName());	
		return FALSE;
	}
	time_t nTimeNow = time(NULL);
	//1.先比有效时间，超出范围直接回false
	//1.1.durastarttime 为空，没限制starttime,否则当前时间应该晚于durastarttime
	if ( !m_tConfEx.GetDurationDate().IsDuraStartTimeNull() )
	{
		TKdvTime tDuraStartTime = m_tConfEx.GetDurationDate().GetDuraStartTime();
		time_t nDuraStartTime = 0;
		tDuraStartTime.GetTime(nDuraStartTime);
		if ( nTimeNow < nDuraStartTime) //当前还没到有效开启最早时间
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow < nDuraStartTime !\n",
			m_tConf.GetConfName());	
			return FALSE;
		}

	}
	//1.2.duraEndtime 为空，没限制endtime,否则当前时间应该早于duraEndtime
	if ( !m_tConfEx.GetDurationDate().IsDuraEndTimeNull() )
	{
		TKdvTime tDuraEndTime = m_tConfEx.GetDurationDate().GetDuraEndTime();
		time_t nDuraEndTime = 0;
		tDuraEndTime.GetTime(nDuraEndTime);
		if ( nTimeNow > nDuraEndTime ) //当前比有效开启最后时间还晚，会议不能开启，当前预约会议应该结束
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow > nDuraEndTime,endconf !\n",
				m_tConf.GetConfName());
			return FALSE;
		}
		
	}

	//2.如果下次开启时间有值，说明已经开启即时会议成功过，直接比当前时间和下次开启时间
	TKdvTime tNextStartTime = m_tConfEx.GetNextStartTime();
	if ( !m_tConfEx.IsNextStartTimeNull() )
	{	
		time_t nNextTime = 0;
		tNextStartTime.GetTime(nNextTime);
		if ( nTimeNow < nNextTime )//还没到开启时间
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow < nNextTime !\n",
				m_tConf.GetConfName());
			return FALSE;
		}
		else
		{
			if ( ( m_tConf.GetDuration() == 0 && nTimeNow > (nNextTime + 30*60) ) ||
				( m_tConf.GetDuration() !=0 && nTimeNow > (nNextTime + m_tConf.GetDuration()*60) )
				)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF,
					"[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow > nNextTime + duratime(or30min) !\n",m_tConf.GetConfName());
				return FALSE;
			}
			//当前时间在今天还可以开的范围内
			return TRUE;
		}
	} 

	
	//3.还没开启成功过即时会议，看当前时间是否符合开启条件
	//3.1.每日型：比较时分秒
	TKdvTime tStartTime = m_tConf.GetKdvStartTime();
	TKdvTime tKdvTimeNow ;
	tKdvTimeNow.SetTime(&nTimeNow);
	if (m_tConf.GetScheduleConfMode() == CIRCLE_SCHEDULE_CONFMODE_DAY )
	{
		tStartTime.SetYear(tKdvTimeNow.GetYear());
		tStartTime.SetMonth(tKdvTimeNow.GetMonth());
		tStartTime.SetDay(tKdvTimeNow.GetDay());
		time_t nStartTime = 0;
		tStartTime.GetTime(nStartTime);
		if ( nTimeNow < nStartTime  )//还没到要开启时间
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow < nStartTime !\n",
				m_tConf.GetConfName());
			return FALSE;
		}
		else //今天开启的话看当前是否超过开启时间半小时或者开启时间+持续时间
		{
			if ( ( m_tConf.GetDuration() == 0 && nTimeNow > (nStartTime + 30*60) ) ||
					( m_tConf.GetDuration() !=0 && nTimeNow > (nStartTime + m_tConf.GetDuration()*60) )
				)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF,
					 "[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow > nStartTime + duratime(or30min) !\n",m_tConf.GetConfName());
				return FALSE;
			}
			//当前时间在今天还可以开的范围内
			return TRUE;
		}
		
	}//end CIRCLE_SCHEDULE_CONFMODE_DAY
	//3.2.每周型，比较勾选星期和时分秒
	else if ( m_tConf.GetScheduleConfMode() == CIRCLE_SCHEDULE_CONFMODE_WEEK )
	{
		struct tm * ptTime;
		ptTime = localtime(&nTimeNow);
		//先比星期是否勾选
		u8 byWeekDay = ptTime->tm_wday;
		if ( m_tConf.IsWeekDaySelected(byWeekDay) )//比较时分秒
		{
			tStartTime.SetYear(tKdvTimeNow.GetYear());
			tStartTime.SetMonth(tKdvTimeNow.GetMonth());
			tStartTime.SetDay(tKdvTimeNow.GetDay());
			time_t nStartTime = 0;
			tStartTime.GetTime(nStartTime);
			if ( nTimeNow < nStartTime  )//还没到要开启时间
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow < nStartTime !\n",
					m_tConf.GetConfName());
				return FALSE;
			}
			else
			{
				if ( ( m_tConf.GetDuration() == 0 && nTimeNow > (nStartTime + 30*60) ) ||
					( m_tConf.GetDuration() !=0 && nTimeNow > (nStartTime + m_tConf.GetDuration()*60) )
					)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF,
						"[IsCircleScheduleConfNeedStart]circlescheduleconf:%s nTimeNow > nStartTime + duratime(or30min) !\n",m_tConf.GetConfName());
					return FALSE;
				}
				//当前时间在今天还可以开的范围内
				return TRUE;
			}
		}
		else
		{
			//每周型没勾选今天的星期
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[IsCircleScheduleConfNeedStart]circlescheduleconf:%s not select today weekday !\n",
				m_tConf.GetConfName());
			return FALSE;
		}

	}//end CIRCLE_SCHEDULE_CONFMODE_WEEK 

	//4.所有返回false的情况都过滤掉了，返回TRUE
	return FALSE;
	
}


/*====================================================================
    函数名      : GetNextOngoingTime
    功能        ：获取周期性预约会议下次开启的时间
    算法实现    ：
    引用全局变量：
    输入参数说明： tConfInfo:会议的confinfo。 tValidDuraDate:会议有效时间
					bAcceptCurOlder:是否接受比当前还早的时间（早半小时内或者当前早于开启+持续时间）
					仅开启会议或者修改会议的时候有用，别的时候不应该考虑（因为正常的话这种情况今天肯定已经开过了）
    返回值说明  ：TKdvTime：下次开启时间
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/02/21                周翼亮          创建
====================================================================*/
TKdvTime CMcuVcInst::GetNextOngoingTime( const TConfInfo& tConfInfo, const TDurationDate& tValidDuraDate,BOOL32 bAcceptCurOlder /*= FALSE*/)
{
	TKdvTime tNullTime;
	memset(&tNullTime,0,sizeof(TKdvTime));
	TKdvTime tNextTime;
	memset(&tNextTime,0,sizeof(TKdvTime));
	if ( !tConfInfo.IsCircleScheduleConf() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[GetNextOngoingTime]conf:%s is not a circleshceduleconf !\n",tConfInfo.GetConfName());
		return tNullTime;
	}
	time_t nDuraStartTime = 0;
	time_t nDuraEndTime = 0;
	time_t nTimeNow = time(NULL);
	if ( !tValidDuraDate.IsDuraStartTimeNull() )
	{
		tValidDuraDate.GetDuraStartTime().GetTime(nDuraStartTime);
	}
	if ( !tValidDuraDate.IsDuraEndTimeNull() )
	{
		tValidDuraDate.GetDuraEndTime().GetTime(nDuraEndTime);
	}

	//今天开启即时会议的时间
	TKdvTime tConfTodayStartTime = tConfInfo.GetKdvStartTime();
	TKdvTime tKdvTimeNow ;
	tKdvTimeNow.SetTime(&nTimeNow);
	tConfTodayStartTime.SetYear(tKdvTimeNow.GetYear());
	tConfTodayStartTime.SetMonth(tKdvTimeNow.GetMonth());
	tConfTodayStartTime.SetDay(tKdvTimeNow.GetDay());
	time_t nConfTodayStartTime = 0;
	time_t nStartToFindNexttime = 0;
	tConfTodayStartTime.GetTime(nConfTodayStartTime);
	nStartToFindNexttime = nConfTodayStartTime;

	//当前时间还不到有效最早开启时间，直接从有效最早开启时间开始找下次开启时间
	if ( nDuraStartTime != 0 && nDuraStartTime > nTimeNow )
	{
		TKdvTime tValidEarlistStarttime = tValidDuraDate.GetDuraStartTime();
		tValidEarlistStarttime.SetHour(tConfTodayStartTime.GetHour());
		tValidEarlistStarttime.SetMinute(tConfTodayStartTime.GetMinute());
		tValidEarlistStarttime.SetSecond(tConfTodayStartTime.GetSecond());
		tValidEarlistStarttime.GetTime(nStartToFindNexttime);
	
	}

	//每日型，日+1，
	if (tConfInfo.GetScheduleConfMode() == CIRCLE_SCHEDULE_CONFMODE_DAY)
	{
		time_t nNexttime = nStartToFindNexttime;
		if ( nStartToFindNexttime > nTimeNow )//开始找的时间比当前时间晚，下次开启时间为开始找的时间
		{	
		
		}
		else //开始找的时间早于当前的时间，根据bAcceptCurOlder，看今天的开启时间是否符合，不符合找明天的
		{
		
			if ( bAcceptCurOlder && 
					(( tConfInfo.GetDuration() == 0 && nTimeNow < (nStartToFindNexttime + 30*60) ) ||
						( tConfInfo.GetDuration() !=0 && nTimeNow < (nStartToFindNexttime + tConfInfo.GetDuration()*60) )
					)
				)
			{
				nNexttime = nTimeNow;
			}
			else
			{
				nNexttime +=  24*3600;//+1天时间
			}
			
		}
		
		//有效时间范围外，返回空
		if (  nDuraEndTime != 0 && nDuraEndTime < nNexttime )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[GetNextOngoingTime]conf:%s next starttime out of durationdate !\n",tConfInfo.GetConfName());
			return tNullTime;
		}
	
		tNextTime.SetTime(&nNexttime);
		
	}
	//每周型，找到勾选的下一个星期再算
	else if ( tConfInfo.GetScheduleConfMode() == CIRCLE_SCHEDULE_CONFMODE_WEEK )
	{
		time_t nNexttime = nStartToFindNexttime;
		if ( nStartToFindNexttime > nTimeNow )//开始找的时间比当前时间晚，今天还开不了，从可以开的时候开
		{	
			//从可以开的时候找勾选
			struct tm * ptTime;
			ptTime = localtime(&nStartToFindNexttime);
			u8 byInc = 0;
			for (u8 byWeekIdx =0 ;byWeekIdx < 7;byWeekIdx ++)
			{
				u8 byNextWeekDay = (ptTime->tm_wday + byInc)%7;
				if ( tConfInfo.IsWeekDaySelected(byNextWeekDay) )//勾选了
				{
					break;
				}
				byInc++;
			}
			if ( byInc >= 8 )//找一周时间都没找到勾选，异常情况
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[GetNextOngoingTime]conf:%s no next select weekday!\n",tConfInfo.GetConfName());
				return tNullTime;
			}
			nNexttime +=  byInc*24*3600;
			
		}
		else //开始找的时间早于当前的时间，根据bAcceptCurOlder，看今天的开启时间是否符合，不符合找下一个勾选
		{

			struct tm * ptTime;
			ptTime = localtime(&nTimeNow);
			if ( bAcceptCurOlder && tConfInfo.IsWeekDaySelected(ptTime->tm_wday)&&
				(( tConfInfo.GetDuration() == 0 && nTimeNow < (nStartToFindNexttime + 30*60) ) ||
				( tConfInfo.GetDuration() !=0 && nTimeNow < (nStartToFindNexttime + tConfInfo.GetDuration()*60) )
				)
				)
			{
				nNexttime = nTimeNow;
				
			}
			else
			{
				//找下一个勾选
		
				u8 byInc = 1;
				for (u8 byWeekIdx =0 ;byWeekIdx < 7;byWeekIdx ++)
				{
					u8 byNextWeekDay = (ptTime->tm_wday + byInc)%7;
					if ( tConfInfo.IsWeekDaySelected(byNextWeekDay) )//勾选了
					{
						break;
					}
					byInc++;
				}
				if ( byInc >= 8 )//找一周时间都没找到勾选，异常情况
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[GetNextOngoingTime]conf:%s no next select weekday!\n",tConfInfo.GetConfName());
					return tNullTime;
				}
				nNexttime +=  byInc*24*3600;
			}
		}
		
	
		if ( nDuraEndTime != 0 && nDuraEndTime < nNexttime )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[GetNextOngoingTime]conf:%s next starttime out of durationdate !\n",tConfInfo.GetConfName());
			return tNullTime;
		}
		tNextTime.SetTime(&nNexttime);
	}
	return tNextTime;
}

/*====================================================================
    函数名      : IsCircleScheduleConf
    功能        ：根据kdvstarttime判断是否是周期性预约会议
    算法实现    ：
    引用全局变量：
    输入参数说明： const TKdvTime& tStartTime:会议开启时间
    返回值说明  ：TRUE:周期性预约会议，FALSE：非周期性预约会议
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/02/21                周翼亮          创建
====================================================================*/
BOOL32 CMcuVcInst::IsCircleScheduleConf( const TKdvTime& tStartTime )
{

		u16 wYear = tStartTime.GetYear();
		u16 wScheMode = (wYear & 0xE000)>>13;
		//周期性预约会议
		if ( wScheMode < CIRCLE_SCHEDULE_CONFMODE_NUM && wScheMode > CIRCLE_SCHEDULE_CONFMODE_NONE )
		{
			return TRUE;
		}
		return FALSE;
}



/*====================================================================
    函数名      : ModifyCircleScheduleConfDuraDate
    功能        ：保证durastart的时分秒为00:00:00,duraend(有值的话)的时分秒为23:59:59
    算法实现    ：
    引用全局变量：
    输入参数说明： tDuarationDate
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/02/21                周翼亮          创建
====================================================================*/
void CMcuVcInst::ModifyCircleScheduleConfDuraDate(TDurationDate& tDuarationDate )
{
	if ( !tDuarationDate.IsDuraStartTimeNull() )
	{
		TKdvTime tDuraStartTime = tDuarationDate.GetDuraStartTime();
		tDuraStartTime.SetHour(0);
		tDuraStartTime.SetMinute(0);
		tDuraStartTime.SetSecond(0);
		tDuarationDate.SetDuraStartTime(tDuraStartTime);
	}

	if (!tDuarationDate.IsDuraEndTimeNull())
	{
		TKdvTime tDuraEndTime = tDuarationDate.GetDuraEndTime();
		tDuraEndTime.SetHour(23);
		tDuraEndTime.SetMinute(59);
		tDuraEndTime.SetSecond(59);
		tDuarationDate.SetDuraEndTime(tDuraEndTime);
	}
}

/*====================================================================
    函数名      : GetVmpModule
    功能        ：从新的画面合成模板中获取原20风格画面合成模板
				 （即从TVmpModuleInfo === > TVmpModule）
    算法实现    ：
    引用全局变量：
    输入参数说明 void
    返回值说明  ：TVmpModule 返回组织好的原20风格vmp模板
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/03/16                chendaiwei    创建
====================================================================*/
TVmpModule CMcuVcInst::GetVmpModule( void )
{
	TVmpModule tVmpModule;
	TVMPParam tParam;
	memset(&tParam,0,sizeof(tParam));

	u32 byBasicParamLen = sizeof(TVMPParam)-MAXNUM_MPUSVMP_MEMBER*sizeof(TVMPMember);
	memcpy(&tParam,&m_tConfEqpModule.m_tVmpModuleInfo,byBasicParamLen);

	memcpy(&tVmpModule.m_abyVmpMember[0],&m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[0],MAXNUM_MPUSVMP_MEMBER);
	memcpy(&tVmpModule.m_abyMemberType[0],&m_tConfEqpModule.m_tVmpModuleInfo.m_abyMemberType[0],MAXNUM_MPUSVMP_MEMBER);

	for( u8 byIdx = 0; byIdx < MAXNUM_MPUSVMP_MEMBER; byIdx++)
	{
		TVMPMember tMember;
		memset(&tMember,0,sizeof(tMember));

		tMember.SetMemberType(m_tConfEqpModule.m_tVmpModuleInfo.m_abyMemberType[byIdx]);
		TMt tMt = m_ptMtTable->GetMt(m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byIdx]);
		tMember.SetMemberTMt(tMt);

		tParam.SetVmpMember(byIdx,tMember);
	}

	tVmpModule.SetVmpParam(tParam);

	return tVmpModule; 
}

/*====================================================================
    函数名      : GetVmpParam25MemFromModule
    功能        ：从新的画面合成模板中获取原20风格画面合成模板
				 （即从TVmpModuleInfo === > TVmpModule）
    算法实现    ：
    引用全局变量：
    输入参数说明 void
    返回值说明  ：TVmpModule 返回组织好的原20风格vmp模板
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/03/16                chendaiwei    创建
====================================================================*/
TVMPParam_25Mem CMcuVcInst::GetVmpParam25MemFromModule( void )
{
	TVMPParam_25Mem tParam;
	memset(&tParam,0,sizeof(tParam));

	u32 byBasicParamLen = sizeof(TVMPParam)-MAXNUM_MPUSVMP_MEMBER*sizeof(TVMPMember);
	memcpy(&tParam,&m_tConfEqpModule.m_tVmpModuleInfo,byBasicParamLen);

	for( u8 byIdx = 0; byIdx < MAXNUM_VMP_MEMBER; byIdx++)
	{
		TVMPMember tMember;
		memset(&tMember,0,sizeof(tMember));

		tMember.SetMemberType(m_tConfEqpModule.m_tVmpModuleInfo.m_abyMemberType[byIdx]);
		TMt tMt = m_ptMtTable->GetMt(m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byIdx]);
		tMember.SetMemberTMt(tMt);

		tParam.SetVmpMember(byIdx,tMember);
	}

	return tParam; 
}


// END OF FILE

