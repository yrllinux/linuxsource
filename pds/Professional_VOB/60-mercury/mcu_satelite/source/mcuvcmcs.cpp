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
#include "mcuerrcode.h"
#include "mtadpssn.h"
#include "mcuutility.h"
#include "mpmanager.h"
#include "radiusinterface.h"
#include "modemssn.h"

#if defined(_VXWORKS_)
    #include <inetLib.h>
    #include "brddrvlib.h"
#elif defined(_LINUX_)
    #include "boardwrapper.h"
#endif

extern TMtTopo		g_atMtTopo[MAXNUM_TOPO_MT];	//保存全网中MT拓扑信息
extern u16		    g_wMtTopoNum;		        //保存全网中MT数目


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

	//save info
	g_cMcuVcApp.SetMcConnected( byInstId, TRUE );
	g_cMcuVcApp.SetMcsRegInfo( byInstId, tRegReq );
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
	TLogicalChannel tLogicalChannel;

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
                 if ( ROLLCALL_MODE_NONE != ptConfInfo->m_tStatus.GetRollCallMode() )
                 {
                     ptConfInfo->m_tStatus.SetRollCallMode(ROLLCALL_MODE_NONE);
                 }
			 }
			 
             if( ptConfInfo->m_tStatus.GetProtectMode() == CONF_LOCKMODE_NEEDPWD )
			 {
				 //下次登录需重新输入密码
                 ptConfProtectInfo->SetMcsPwdPassed(byInstId, FALSE);
			 }
		}
	}

	//stop switch to MC
	g_cMcuVcApp.BroadcastToAllConf( MCU_MCSDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );
	
	//clear info
	g_cMcuVcApp.SetMcConnected( byInstId, FALSE );

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
    CServMsg cMtMsg;

    CServMsg  cServMsg( pcMsg->content, pcMsg->length );

    TConfInfo tTmpConf = *(TConfInfo *)cServMsg.GetMsgBody();
    BOOL32 bConfFromFile = tTmpConf.m_tStatus.IsTakeFromFile();
    tTmpConf.m_tStatus.SetTakeFromFile(FALSE);

	TCapSupportEx tCapEx;

	switch( CurState() )
	{
	case STATE_WAITEQP:
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
			BOOL32 bRet = g_cMcuVcApp.AnalyEqpCapacity( tConfEqpCap, tMcuEqpCap );            

            // [12/28/2006-zbq] N+1备份回滚或恢复的会议，认为外设已经通过，直接创会
			if( !bRet && MCU_NPLUS_IDLE == g_cNPlusApp.GetLocalNPlusState() )
			{
				// 设置定时器去重新开会		
				if( bConfFromFile )
				{
                    // 超过最大尝试次数
					if( MAX_TIMES_CREATE_CONF < m_wRestoreTimes ) 
					{
						if (!g_cMcuVcApp.RemoveConfFromFile(tTmpConf.GetConfId()))
						{
							ConfLog( FALSE, "[ProcMcsMcuCreateConfReq] remove conf from file failed\n");
						}
                        else
                        {
                            ConfLog( FALSE, "[ProcMcsMcuCreateConfReq] conf.%s has been remove from file due to RestoreTimes.%d\n", tTmpConf.GetConfName(), m_wRestoreTimes);
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
				
				ConfLog( FALSE, "Conference %s create failed because NO enough equipment(%d)!\n", tTmpConf.GetConfName(), m_wRestoreTimes );
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
						OspPrintf(TRUE, FALSE, "[ProcMcsMcuCreateConfReq] remove conf from file failed\n");
					}
                    */
					g_cMcuVcApp.SetConfStore(byConfIdx-MIN_CONFIDX, FALSE);// 以后不再恢复

					NEXTSTATE(STATE_IDLE);
                    m_wRestoreTimes = 1;

                    KillTimer(MCU_WAITEQP_CREATE_CONF_NOTIFY);
				}
			}
		}  // 此处没有Break，因为当Mcu支持会议需求的外设时，继续执行下面的程序来创会

	case STATE_IDLE:

		//清空实例
		ClearVcInst();
        cServMsg.SetErrorCode(0);       

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

        // guzh [4/16/2007] 判断即时会议开始时间
        if ( tTmpConf.m_tStatus.IsOngoing() &&
             tTmpConf.GetStartTime() > time(NULL)+1*60 &&
             // zbq [08/02/2007] N+1备份恢复的会议直接召开
             CONF_CREATE_NPLUS != cServMsg.GetSrcMtId() )
        {
            cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );            
            ConfLog( FALSE, "Conference %s failure because start time error!\n", tTmpConf.GetConfName() );
			return;
        }
        
        // 已达最大会议数，创会时不考虑是否级联
		if ( g_cMcuVcApp.IsConfNumOverCap(tTmpConf.m_tStatus.IsOngoing(), tTmpConf.GetConfSource()) )
		{
			cServMsg.SetErrorCode( ERR_MCU_CONFNUM_EXCEED );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            if ( bConfFromFile )
            {
                g_cMcuVcApp.RemoveConfFromFile(tTmpConf.GetConfId());
            }      
			ConfLog( FALSE, "Conference %s failure because exceed max conf num!\n", tTmpConf.GetConfName() );
			return;
		}

		if (0 == g_cMcuVcApp.GetMpNum() || 0 == g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323))
		{
			cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			ConfLog( FALSE, "Conference %s failure because No_Mp or No_MtAdp!\n", tTmpConf.GetConfName() );
			return;
		}
		
		//开辟终端数据区
		if( NULL == m_ptMtTable )
		{
			m_ptMtTable = new TConfMtTable;
		}

		memset( m_ptMtTable, 0, sizeof( TConfMtTable ) );
		
		m_dwSpeakerViewId  = OspTickGet();//102062000;//
		m_dwSpeakerVideoId = m_dwSpeakerViewId+100;//102052024;//
		m_dwVmpViewId      = m_dwSpeakerVideoId+100;
		m_dwVmpVideoId     = m_dwVmpViewId+100;
		m_dwSpeakerAudioId = m_dwVmpVideoId+100;
		m_dwMixerAudioId   = m_dwSpeakerAudioId+100;
		
		//得到创建者
		m_byCreateBy = cServMsg.GetSrcMtId();
        
        m_byConfIdx = g_cMcuVcApp.GetTemConfIdxByE164(tTmpConf.GetConfE164());                    
        if(0 == m_byConfIdx)
        {
            m_byConfIdx = g_cMcuVcApp.GetIdleConfidx();
            if(0 == m_byConfIdx)
            {                      
                ConfLog( FALSE, "Conference %s failure for conf full!\n", tTmpConf.GetConfName() );
                return;
            }
        }           
        byOldInsId = g_cMcuVcApp.GetConfMapInsId(m_byConfIdx);
        g_cMcuVcApp.SetConfMapInsId(m_byConfIdx, (u8)GetInsID());

		//得到会议信息
	    m_tConf = tTmpConf;        

        //zbq[12/04/2007] 兼容老模板会议帧率定义
        if ( MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
             !m_tConf.IsMainVidUsrDefFPS() )
        {
            ConfLog( FALSE, "[CreateConf] MVidType.%d with FPS.%d is unexpected, adjust it\n",
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
            ConfLog( FALSE, "[CreateConf] SVidType.%d with FPS.%d is unexpected, adjust it\n",
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
            ConfLog( FALSE, "[CreateConf] DSVidType.%d with FPS.%d is unexpected, adjust it\n",
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

        //zbq[11/15/2008] debug增加第二双流能力
        if (MEDIA_TYPE_NULL == m_tConf.GetCapSupportEx().GetSecDSType())
        {
            AddSecDSCapByDebug(m_tConf);
        }

#ifdef _SATELITE_
		if (IsHDConf(m_tConf) &&
			g_cMcuVcApp.IsConfAdpManually())
		{
			tCapEx = m_tConf.GetCapSupportEx();
			
			tCapEx.SetSecDSBitRate(m_tConf.GetBitRate()); //同第一路双流，暂填全能力
			tCapEx.SetSecDSFrmRate(5);
			tCapEx.SetSecDSType(MEDIA_TYPE_H263);
			tCapEx.SetSecDSRes(VIDEO_FORMAT_SVGA);
			
			m_tConf.SetCapSupportEx(tCapEx);
			ConfLog(FALSE, "[AddSecDSCapByDebug] add dual ds cap<h263+, SVGA, 5fps>, satelite\n");

		}

#endif
		

		//卫星分散会议，debug增加第二路主流能力: 模拟卫星会议的环境
		if (g_cMcuVcApp.IsConfAdpManually() &&
			IsHDConf(m_tConf) &&
			m_tConf.GetConfAttrb().IsSatDCastMode() )
		{
			TSimCapSet tSimCap;
			tSimCap.SetAudioMediaType(MEDIA_TYPE_PCMA);
			tSimCap.SetVideoFrameRate(25);
			tSimCap.SetVideoMaxBitRate(512);
			tSimCap.SetVideoMediaType(MEDIA_TYPE_H263);
			tSimCap.SetVideoResolution(VIDEO_FORMAT_CIF);
			
			TCapSupport tConfCap = m_tConf.GetCapSupport();
			tConfCap.SetSecondSimCapSet(tSimCap);
			m_tConf.SetCapSupport(tConfCap);

			m_tConf.SetSecBitRate(512);
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
            CreateConfSendMcsNack( byOldInsId, ERR_MCU_NOMTINCONF, cServMsg );
            ConfLog( FALSE, "Conference %s create failed because on mt in conf!\n", m_tConf.GetConfName() );
            return;
        }

        // N+1 备份认为不支持预约会议 [12/20/2006-zbq]
		// 会议处理: 即时会议
		if ( CONF_CREATE_NPLUS == m_byCreateBy ||
            ( CONF_CREATE_NPLUS != m_byCreateBy && m_tConf.m_tStatus.IsOngoing() )  )
		{
			if( NULL == m_ptSwitchTable )
			{
				m_ptSwitchTable = new TConfSwitchTable;
			}
			if( NULL == m_ptConfOtherMcTable )
			{
				m_ptConfOtherMcTable = new TConfOtherMcTable;
			}
			
            if (m_ptSwitchTable != NULL)
            {
                memset( m_ptSwitchTable, 0, sizeof( TConfSwitchTable ) );
            }
			
            if (m_ptConfOtherMcTable != NULL)
            {
                memset( m_ptConfOtherMcTable, 0, sizeof( TConfOtherMcTable ) );
            }
			
			//清除GK注册信息，即时模板本身注册成功
            m_tConf.m_tStatus.SetRegToGK( FALSE );

            //MCU的外设能力是否充分，及部分外设操作
            if ( !IsEqpInfoCheckPass(cServMsg, wErrCode))
            {
                CreateConfEqpInsufficientNack( byCreateMtId, byOldInsId, wErrCode, cServMsg );

                if (ERR_MCU_NOENOUGH_HDBAS_CONF == wErrCode)
                {
                    TMcuHdBasStatus tStatus;
                    m_cBasMgr.GetHdBasStatus(tStatus, m_tConf);
                    
                    CServMsg cMsg;
                    cMsg.SetMsgBody((u8*)&tStatus, sizeof(tStatus));
                    cMsg.SetEventId(MCU_MCS_MAUSTATUS_NOTIFY);
                    SendMsgToMcs(cServMsg.GetSrcSsnId(), MCU_MCS_MAUSTATUS_NOTIFY, cMsg);
                }
                return;
            }

            TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();

            if( tConfAttrib.IsDiscussConf() )
            {
                // 顾振华 [4/29/2006] 在这里启动全体混音
                StartMixing( mcuWholeMix );
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
                
                // guzh [7/19/2007] 发送给创建者
                SendMsgToMt(CONF_CREATE_MT, MCU_MT_CREATECONF_ACK, cAckMsg);
            }

			//打印信息 
			if ( CONF_DATAMODE_VAONLY == m_tConf.GetConfAttrb().GetDataMode() )
			{
				//视讯会议
				ConfLog( FALSE, "conference %s created and started!\n",m_tConf.GetConfName());
			}
			else if ( CONF_DATAMODE_VAANDDATA == m_tConf.GetConfAttrb().GetDataMode() )
			{
				//视讯和数据
				ConfLog( FALSE, "conference %s created and started with data conf function !\n", m_tConf.GetConfName());
			}
			else
			{
				//数据会议
				ConfLog( FALSE, "conference %s created and started with data conf function Only !\n", m_tConf.GetConfName());
			}

            //zbq[12/18/2008] 手动开启成功，清定时等待
            KillTimer(MCU_WAITEQP_CREATE_CONF_NOTIFY);

			//保存到文件(由上级MCU的呼叫创建的会议不保存)，N+1备份模式不保存到文件
            if ( CONF_CREATE_NPLUS == m_byCreateBy && MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
            {
                NPlusLog("[ProcMcsMcuCreateConfReq] conf info don't save to file in N+1 restore mode.\n");
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
                    ConfLog( FALSE, "[CreateConf] charge postponed due to GetRegGKDriId.%d, ChargeRegOK.%d !\n",
                                     g_cMcuVcApp.GetRegGKDriId(), g_cMcuVcApp.GetChargeRegOK() );
                }
            }

            //卫星会议 向网管注册申请 频率
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                ProcCreateConfAccordNms(pcMsg);
                ApplySatFrequence();
            }

            // 顾振华 [4/29/2006]。原来通知MCS发送在保存到文件上面。
            // 移动到这里是为了保证Gk的信息是正确的。通知所有会控。
            cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
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

			SetTimer( MCUVC_INVITE_UNJOINEDMT_TIMER, 1000*m_tConf.m_tStatus.GetCallInterval() );
			
			SetTimer( MCUVC_MCUSRC_CHECK_TIMER, TIMESPACE_MCUSRC_CHECK);
            
			u8 m_byUniformMode = tConfAttrib.IsAdjustUniformPack();
			cServMsg.SetMsgBody((u8 *)&m_byUniformMode, sizeof(u8));
            cServMsg.SetConfIdx(m_byConfIdx);
			SendMsgToAllMp(MCU_MP_CONFUNIFORMMODE_NOTIFY, cServMsg);

            //改变会议实例状态
            NEXTSTATE( STATE_ONGOING );

            //发给会控会议所有终端信息
            m_tConfAllMtInfo.m_tLocalMtInfo.SetMcuId( LOCAL_MCUID );
            cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
            SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
            
            //发给会控终端表 
            SendMtListToMcs(LOCAL_MCUID);	
            
            //通知n+1备份服务器更新会议信息
            if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
            {
                ProcNPlusConfInfoUpdate(TRUE);
            }            
		}
		else //预约会议
		{					
            m_tConf.m_tStatus.SetScheduled();

			//把会议指针存到数据区
			if( !g_cMcuVcApp.AddConf( this ) )	//add into table
			{
                CreateConfSendMcsNack( byOldInsId, ERR_MCU_CONFOVERFLOW, cServMsg, TRUE);
				ConfLog( FALSE, "Conference %s create failed because of full instance!\n", m_tConf.GetConfName() );
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

			//保存到文件
			BOOL32 bRetTmp = g_cMcuVcApp.SaveConfToFile( m_byConfIdx, FALSE );
            if (!bRetTmp)
            {
                OspPrintf(TRUE, FALSE, "[creat conf]SaveConfToFile error!!!\n");
            }

			//设置时钟
			SetTimer( MCUVC_SCHEDULED_CHECK_TIMER, TIMESPACE_SCHEDULED_CHECK );

			//打印
			ConfLog( FALSE, "A scheduled conference %s created!\n", m_tConf.GetConfName() );
			
			//改变会议实例状态
			NEXTSTATE( STATE_SCHEDULED );
        }		
		break;
	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	
	return;

}

/*====================================================================
    函数名      :ProcCreateConfAccordNms
    功能        :网管安排会议
    算法实现    :
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/19    1.0         zhangsh         创建
    09/09/11    4.6         张宝卿          从卫星版本移植过来
====================================================================*/
void CMcuVcInst::ProcCreateConfAccordNms( const CMessage* pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	
    u8 byMtLoop;
	u32 dwMtIp;
	
    TSchConfInfoHeader tSchInfo;
	tSchInfo.SetConfId( m_tConf.GetConfId() );
	tSchInfo.SetPassWord( (u8*)m_tConf.GetConfPwd() );
	tSchInfo.SetStartTime( m_tConf.GetStartTime() );
	tSchInfo.SetEndTime( m_tConf.GetStartTime() + m_tConf.GetDuration()*60 );
	tSchInfo.SetMessageType( MCU_NMS_SCHEDULE_CONF_REQ );
	tSchInfo.SetMtNum( m_tConfAllMtInfo.GetAllMtNum() );
	tSchInfo.SetSerialId( GetSerialId() );
	cServMsg.SetMsgBody( (u8*)&tSchInfo, sizeof(TSchConfInfoHeader) );
	
    //add all Mt Ip
	for ( byMtLoop = 1; byMtLoop < MAXNUM_CONF_MT; byMtLoop ++ )
	{
		if ( m_tConfAllMtInfo.MtInConf( byMtLoop ) )
		{
			dwMtIp = ::topoGetMtInfo( LOCAL_MCUID, byMtLoop, g_atMtTopo, g_wMtTopoNum ).GetIpAddr();
			dwMtIp = htonl( dwMtIp );
			cServMsg.CatMsgBody( (u8*)&dwMtIp, sizeof(u32) );
		}
	}
	g_cMcuVcApp.SendMsgToDaemonConf( MCU_NMS_SENDNMSMSG_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	SetTimer( TIMER_SCHEDCONF, 5*1000 );
	
    return;
}

/*====================================================================
    函数名      :ProcNmsMcuSchedConf
    功能        :预约会议验证,如果网管不回，则默认失败
    算法实现    
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/02/02    1.0         zhangsh         创建
    09/09/11    4.6         张宝卿          从卫星版本移植过来
====================================================================*/
void CMcuVcInst::ProcNmsMcuSchedConf( const CMessage*pcMsg )
{
	TSchConfNotif tSchConf = *(TSchConfNotif*) pcMsg->content;
	CServMsg cServMsg;
	u32 *pNackIp;
	u32 dwMtIp;
	u8  abyConfId[sizeof( CConfId )];
	//cServMsg.SetSrcSsnId( m_byMcSsnId );
	//cServMsg.SetSerialNo( m_dwMcSerialNo );
	//cServMsg.SetSrcMcu( g_cMcuAgent.GetId() );
	
	switch( CurState() )
	{
	//FIXME: 稍后严格 保障状态机
	case STATE_ONGOING:

	case STATE_SCHEDULED:
    case STATE_WAITAUTH:
	case STATE_WAITEQP:

		
        KillTimer( TIMER_SCHEDCONF );
		
        //FIXME: 此处不设置状态机
        //m_tConf.m_tStatus.SetScheduled();	//set state

		//m_tConf.SetMcuId( (u8)g_cMcuAgent.GetId() );
		m_tConf.GetConfId().GetConfId( abyConfId, sizeof( CConfId ) );
	
        //如果网管拒绝了所有终端,会议结束
		if ( tSchConf.GetMtNum() == m_tConfAllMtInfo.GetAllMtNum() || tSchConf.GetMcs() == 0 )
		{
			//cServMsg.SetErrorCode( ERR_MCU_NMSNOPARTICIPANT );
			//SendReplyBack( cServMsg, MCU_VCCTRL_CREATECONF_NACK );
            
            NotifyMcsAlarmInfo(0, ERR_MCU_NMSNOPARTICIPANT);
			ReleaseConf( TRUE );
			NEXTSTATE( STATE_IDLE );
			ConfLog( FALSE, "[ProcNmsMcuSchedConf] Nms del all Mt!\n");
			return;
		}
		
        //网管删除了部分终端
		if ( tSchConf.GetMtNum() != 0 )
		{
			pNackIp = (u32*)(pcMsg->content+sizeof(TSchConfNotif));
			for ( u16 wDelMt = 0 ; wDelMt < tSchConf.GetMtNum() ; wDelMt ++ )
			{
				for ( u16 byMtLoop = 1; byMtLoop < MAXNUM_CONF_MT; byMtLoop ++ )
				{
					if ( m_tConfAllMtInfo.MtInConf( byMtLoop ) )
					{
						dwMtIp = ::topoGetMtInfo( LOCAL_MCUID, byMtLoop, g_atMtTopo, g_wMtTopoNum ).GetIpAddr();
						dwMtIp = htonl( dwMtIp );
						if ( *pNackIp == dwMtIp )
						{
                            TMt tMt = m_ptMtTable->GetMt(byMtLoop);

							//FIXME: 这里的remove的含义是从模板里临时删除，暂时不处理
							RemoveMt( tMt, TRUE );
						}
					}
				}
				pNackIp ++;
			}			
		}

        if (STATE_WAITAUTH == CurState())
        {
            //zbq[09/11/2009] 这里暂时不直接作对会议的发起处理
            /*
            cServMsg.SetMsgBody( (u8*)&m_tConf,sizeof(m_tConf) );
            OspPost( MAKEIID( AID_MCU_VC,GetInsID() ), MCU_CREATECONF_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
            */
		    break;
        }
        else
        {
            //FIXME: 不作此项校验
            /*
       		if( SetAlias( ( const char* )abyConfId, sizeof( CConfId ) )== OSPERR_ALIAS_REPEAT )
		    {
			    //cServMsg.SetErrorCode( ERR_MCU_CONFIDREPEAT );
			    //SendReplyBack( cServMsg, MCU_VCCTRL_CREATECONF_NACK );
                
                NotifyMcsAlarmInfo(0, ERR_MCU_CONFIDREPEAT);
			    ReleaseConf();
			    NEXTSTATE( STATE_IDLE);
			    ConfLog( FALSE, "[ProcNmsMcuSchedConf] Conf.%s create failed due to repeat conference ID at NMS SCH!\n", m_tConf.GetConfName() );
			    return;
		    }*/
		    
            //zbq[09/11/2009] 正常的会议流程已经作过add，这里只作校验
            /*
		    if( !g_cMcuVcApp.AddConf( &m_tConf ) )	//add into table
		    {
			    cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
			    SendReplyBack( cServMsg, MCU_VCCTRL_CREATECONF_NACK );
			    DeleteAlias();
			    ReleaseConf();
			    NEXTSTATE( STATE_IDLE);
			    if ( bConfMsg )
				    log( LOGLVL_EXCEPTION, "CMcuVcInst: Confernece %s create failed because of full instance!\n", m_tConf.GetConfName() );
			    return;
		    }*/
	    
            //zbq[09/11/2009] 后续严格处理，暂不作串行
            /*
		    cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
		    cServMsg.SetConfId( m_tConf.GetConfId() );
		    
            //NAck
		    g_cMcSsnApp.SendMsgToMcs( cServMsg.GetSrcSsnId(), MCU_VCCTRL_CREATECONF_ACK, cServMsg );
		    
            //Notification
		    g_cMcSsnApp.SendMsgToAllMcs( MCU_VCCTRL_CONF_NOTIF, cServMsg );
		    
            //write file
		    ::AddConfInfoToFile( m_tConf );*/


		    //SetTimer( TIMER_SCHEDULED_CHECK, TIMESPACE_SCHEDULED_CHECK );
		    break;
        }

	default:
		ConfLog( FALSE, "[ProcNmsMcuSchedConf] Wrong message %u(%s) received in state %u!\n", 
			            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
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
        ConfLog( FALSE, "Conference %s create failed because bitrate(Vid.%d, Aud.%d) error!\n", tConfInfo.GetConfName(), tConfInfo.GetBitRate(), GetAudioBitrate(tConfInfo.GetMainAudioMediaType()) );
        return FALSE;
    }

    if ( !bTemplate )
    {
        //会议的E164号码已存在，拒绝 
	    if( g_cMcuVcApp.IsConfE164Repeat( tConfInfo.GetConfE164(), bTemplate ) )
	    {
            wErrCode = ERR_MCU_CONFE164_REPEAT;
            if ( bConfFromFile )
            {
                g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
            }      
            ConfLog( FALSE, "Conference %s E164 repeated and create failure!\n", tConfInfo.GetConfName() );
            return FALSE;
        }

	    //会议名已存在，拒绝
	    if( g_cMcuVcApp.IsConfNameRepeat( tConfInfo.GetConfName(), bTemplate ) )
	    {
            wErrCode = ERR_MCU_CONFNAME_REPEAT;
            if ( bConfFromFile )
            {
                g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
            }      
		    ConfLog( FALSE, "Conference %s name repeated and create failure!\n", m_tConf.GetConfName() );
		    return FALSE;
	    }

	    //会议已结束，拒绝 
	    if( tConfInfo.GetDuration() != 0 && 
		    ( time( NULL ) > tConfInfo.GetStartTime() + tConfInfo.GetDuration() * 60 ))
	    {
            wErrCode = ERR_MCU_STARTTIME_WRONG;
            if ( bConfFromFile )
            {
                g_cMcuVcApp.RemoveConfFromFile(tConfInfo.GetConfId());
            }      
		    ConfLog( FALSE, "Conference %s too late and canceled!\n", tConfInfo.GetConfName() );
		    return FALSE;
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
            ConfLog( FALSE, "Conference %s create failed because of repeat conference ID!\n", tConfInfo.GetConfName() );
            return FALSE;
        }        
    }


	//dynamic vmp and vmp module conflict
	if( tConfInfo.GetConfAttrb().IsHasVmpModule() && 
		CONF_VMPMODE_AUTO == tConfInfo.m_tStatus.GetVMPMode() )
	{
        wErrCode = ERR_MCU_DYNAMCIVMPWITHMODULE;
		ConfLog( FALSE, "Conf or temp %s create failed because has module with dynamic vmp!\n", tConfInfo.GetConfName() );
		return FALSE;		
	}

	//无效的会议保护方式,拒绝
	if( tConfInfo.m_tStatus.GetProtectMode() > CONF_LOCKMODE_LOCK )
	{
        wErrCode = ERR_MCU_INVALID_CONFLOCKMODE;
		ConfLog( FALSE, "Conf or temp %s protect mode invalid and nack!\n", tConfInfo.GetConfName() );
		return FALSE;
	}

	//无效的会议呼叫策略,拒绝
	if( tConfInfo.m_tStatus.GetCallMode() > CONF_CALLMODE_TIMER )
	{
        wErrCode = ERR_MCU_INVALID_CALLMODE;
		ConfLog( FALSE, "Conf or temp %s call mode invalid and nack!\n", tConfInfo.GetConfName() );
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
                ConfLog( FALSE, "Conference %s not config gk in gk charge conf and nack!\n", tConfInfo.GetConfName() );
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
                    ConfLog( FALSE, "Conference %s configed gk support no charge conf and nack!\n", tConfInfo.GetConfName() );
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
        !((tConfInfo.GetCapSupport().GetDStreamMediaType() == VIDEO_DSTREAM_MAIN &&
           tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264) ||
          tConfInfo.GetCapSupport().GetDStreamMediaType() == VIDEO_DSTREAM_H264_H239 ||
          tConfInfo.GetCapSupport().GetDStreamMediaType() == VIDEO_DSTREAM_H264) &&
        tConfInfo.GetCapSupport().GetDStreamMediaType() != VIDEO_DSTREAM_H264_H263PLUS_H239 &&
		// VCS会议默认都启用适配器
		tConfInfo.GetConfSource() != VCS_CONF)
    {
        ttmpAttrb.SetUseAdapter(FALSE);
        tConfInfo.SetConfAttrb(ttmpAttrb);
        ConfLog(FALSE, "template/conf %s need not bas adapter, auto modify it in conf attrib.\n", tConfInfo.GetConfName());
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
            ConfLog(FALSE, " conf.%d need vmp auto adapt, open.\n", m_byConfIdx );
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
            ConfLog( FALSE, "template/conf %s not support H264-HD in MCU8000B-NoHD\n", tConfInfo.GetConfName() );
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
            ConfLog( FALSE, "Conf or temp.%s create failed because of multicast address invalid!\n", tConfInfo.GetConfName() );
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
        BOOL32 bAdpNeeded = FALSE;
        if (tConfInfo.GetSecBitRate() != 0 ||
            !tConfInfo.GetCapSupport().GetSecondSimCapSet().IsNull())
        {
            // 双速双格式会议不能选择分散会议方式
            bAdpNeeded = TRUE;

			// FIXME: 卫星分散会议，暂时支持双格式会议，稍后和独立的卫星会议严格区分开来
			bAdpNeeded = FALSE;
        }
        
        // 发言人只能看自己
        if (ttmpAttrb.GetSpeakerSrc() != CONF_SPEAKERSRC_SELF)
        {
            ttmpAttrb.SetSpeakerSrc( CONF_SPEAKERSRC_SELF );
            ConfLog( FALSE, "Conf or temp.%s change speakersrc to self.\n", tConfInfo.GetConfName() );
        }
        // 不启用适配
        if (ttmpAttrb.IsUseAdapter())
        {
            ttmpAttrb.SetUseAdapter(FALSE);
            ConfLog( FALSE, "Conf or temp.%s no use adapter\n", tConfInfo.GetConfName() );
        }
        // 不讨论会议
        if (ttmpAttrb.IsDiscussConf())
        {
            ttmpAttrb.SetDiscussConf(FALSE);
            ConfLog( FALSE, "Conf or temp.%s no start discuss\n", tConfInfo.GetConfName() );
        }
        // 不丢包重传
        if (ttmpAttrb.IsResendLosePack())
        {
            ttmpAttrb.SetPrsMode(FALSE);
            ConfLog( FALSE, "Conf or temp.%s no use prs\n", tConfInfo.GetConfName() );
        }
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
        
        if (bIpInvalid || bAdpNeeded)
        {                
            if (bIpInvalid)
            {
                wErrCode = ERR_MCU_DCASTADDR_INVALID;
                ConfLog( FALSE, "Conf or temp.%s create failed because of distributed conf multicast address invalid!\n", tConfInfo.GetConfName() );
            }
            else
            {
                wErrCode = ERR_MCU_DCAST_NOADP;
                ConfLog( FALSE, "Conf or temp.%s create failed because of distributed conf not support second rate or format!\n", tConfInfo.GetConfName() );
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
            ConfLog( FALSE, "Conf or temp.%s not support multicast low stream\n", tConfInfo.GetConfName() );
            wErrCode = ERR_MCU_MCLOWNOSUPPORT;
            return FALSE;
        }
        
        // 不支持低速录像
        TConfAutoRecAttrb tRecAttrb = tConfInfo.GetAutoRecAttrb();
        if ( tRecAttrb.IsAutoRec() && tRecAttrb.GetRecParam().IsRecLowStream() )
        {
            ConfLog( FALSE, "Conf or temp.%s can't support auto record low stream\n", tConfInfo.GetConfName() );
            wErrCode = ERR_MCU_RECLOWNOSUPPORT;
            return FALSE;    
        }
    }
#endif

    //zbq[11/18/2008] mau相关的处理：适配会议给出提示
    if (tConfInfo.GetConfSource() != VCS_CONF &&
        ((IsHDConf(tConfInfo) &&
		  (0 != tConfInfo.GetSecBitRate() ||
		   (MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType() && 0 != tConfInfo.GetSecVideoMediaType()) ||
		   tConfInfo.GetConfAttrbEx().IsResEx1080() ||
		   tConfInfo.GetConfAttrbEx().IsResEx720()  ||
		   tConfInfo.GetConfAttrbEx().IsResEx4Cif() ||
		   tConfInfo.GetConfAttrbEx().IsResExCif()
		  )
		 )
		 ||
		 // 双双流
		 (tConfInfo.GetCapSupport().IsDStreamSupportH239() &&
		  MEDIA_TYPE_H264 == tConfInfo.GetDStreamMediaType() &&
		  (g_cMcuVcApp.IsSupportSecDSCap() || MEDIA_TYPE_NULL != tConfInfo.GetCapSupportEx().GetSecDSType())
		 )
        )
	   )
    {
        if (!m_cBasMgr.IsHdBasSufficient(tConfInfo))
        {
            ConfLog(FALSE, "temp.%s's mau might be insufficient\n", tConfInfo.GetConfName());
            NotifyMcsAlarmInfo(cMsg.GetSrcSsnId(), ERR_MCU_NOENOUGH_HDBAS_TEMP);
        }

        TMcuHdBasStatus tHdBasStatus;
        m_cBasMgr.GetHdBasStatus(tHdBasStatus, tConfInfo);

        CServMsg cServMsg;
        cServMsg.SetMsgBody((u8*)&tHdBasStatus, sizeof(tHdBasStatus));
        cServMsg.SetEventId(MCU_MCS_MAUSTATUS_NOTIFY);
        SendMsgToMcs(cMsg.GetSrcSsnId(), MCU_MCS_MAUSTATUS_NOTIFY, cServMsg);
    }

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
	07/12/25    4.0         张宝卿          创建
    08/11/18    4.5         张宝卿          增加对MAU的校验
====================================================================*/
void CMcuVcInst::RlsAllBasForConf()
{
	if (m_tConf.m_tStatus.IsAudAdapting() || EQP_CHANNO_INVALID != m_byAudBasChnnl)
	{
		StopBasAdapt(ADAPT_TYPE_AUD);
        g_cMcuVcApp.SetBasChanStatus(m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl, TBasChnStatus::READY);
        g_cMcuVcApp.SetBasChanReserved(m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl, FALSE);
	}

    if (m_tConf.m_tStatus.IsVidAdapting() || EQP_CHANNO_INVALID != m_byVidBasChnnl)
	{
		StopBasAdapt(ADAPT_TYPE_VID);
        g_cMcuVcApp.SetBasChanStatus(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, TBasChnStatus::READY);
        g_cMcuVcApp.SetBasChanReserved(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, FALSE);
	}

    if (m_tConf.m_tStatus.IsBrAdapting() || EQP_CHANNO_INVALID != m_byBrBasChnnl)
	{
		StopBasAdapt(ADAPT_TYPE_BR);
        g_cMcuVcApp.SetBasChanStatus(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, TBasChnStatus::READY);
        g_cMcuVcApp.SetBasChanReserved(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, FALSE);
	}

    if (m_tConf.m_tStatus.IsCasdAudAdapting() || EQP_CHANNO_INVALID != m_byCasdAudBasChnnl)
	{
		StopBasAdapt(ADAPT_TYPE_CASDAUD);
        g_cMcuVcApp.SetBasChanStatus(m_tCasdAudBasEqp.GetEqpId(), m_byCasdAudBasChnnl, TBasChnStatus::READY);
        g_cMcuVcApp.SetBasChanReserved(m_tCasdAudBasEqp.GetEqpId(), m_byCasdAudBasChnnl, FALSE);
	}

    if (m_tConf.m_tStatus.IsCasdVidAdapting() || EQP_CHANNO_INVALID != m_byCasdVidBasChnnl)
	{
		StopBasAdapt(ADAPT_TYPE_CASDVID);
        g_cMcuVcApp.SetBasChanStatus(m_tCasdVidBasEqp.GetEqpId(), m_byCasdVidBasChnnl, TBasChnStatus::READY);
        g_cMcuVcApp.SetBasChanReserved(m_tCasdVidBasEqp.GetEqpId(), m_byCasdVidBasChnnl, FALSE);
	}

    //释放高清适配通道
    StopHDMVBrdAdapt();
    StopHDDSBrdAdapt();
    StopHDCascaseAdp();

    //释放mau
    m_cBasMgr.ReleaseHdBasChn();

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
    if ( IsHDConf(m_tConf) )
    {
        //zbq[11/18/2008] MAU校验 及 占用
        if (!m_cBasMgr.IsHdBasSufficient(m_tConf))
        {
            ConfLog(FALSE, "Conf.%s's mau is insufficient\n", m_tConf.GetConfName());
            *pwErrCode = ERR_MCU_NOENOUGH_HDBAS_CONF;
            return FALSE;
        }

        if (!m_cBasMgr.OcuppyHdBasChn(m_tConf))
        {
            ConfLog(FALSE, "Conf.%s's ocuppy mau(s) failed!\n", m_tConf.GetConfName());
            *pwErrCode = ERR_MCU_OCUPPYHDBAS;
            return FALSE;
        }
    }
    else
    {
        //标清主流
        u8     byEqpId;
        u8     byChnIdx;
        
        BOOL32 bVidBasCap = TRUE;
        BOOL32 bBrBasCap  = TRUE;
        
        TPeriEqpStatus tStatus;
        u8     byMainMediaType;
        u8     bySecondMediaType;
        u8     byMainVidFormat;
        u8     bySecVidFormat;

        byMainMediaType = m_tConf.GetMainVideoMediaType();
        bySecondMediaType = m_tConf.GetSecVideoMediaType();
        byMainVidFormat = m_tConf.GetMainVideoFormat();
        bySecVidFormat = m_tConf.GetSecVideoFormat();
        if ( (MEDIA_TYPE_NULL != bySecondMediaType && byMainMediaType != bySecondMediaType)
            ||(byMainMediaType == bySecondMediaType && byMainVidFormat != bySecVidFormat) )
        {
            if (g_cMcuVcApp.GetIdleBasChl(ADAPT_TYPE_VID, byEqpId, byChnIdx))
            {
                m_tVidBasEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_BAS);
                m_tVidBasEqp.SetConfIdx(m_byConfIdx);
                m_byVidBasChnnl = byChnIdx;
                EqpLog("m_byVidBasChnnl = %d\n", m_byVidBasChnnl);
            }
            else
            {
                bVidBasCap = FALSE;
                ConfLog(FALSE, "no idle video adapte channel!\n");
            }
        }
		
        if (0 != m_tConf.GetSecBitRate())
        {
        #ifdef _MINIMCU_
            // 8000B/8000C 双媒体会议
            if (m_byVidBasChnnl != EQP_CHANNO_INVALID)
            {      
                m_byIsDoubleMediaConf = 1;
                ConfLog(FALSE, "conf uses double media@bitrate support!\n");
                //码率适配不需要
            }
            else
            {
        #endif                    
                if (g_cMcuVcApp.GetIdleBasChl(ADAPT_TYPE_BR, byEqpId, byChnIdx))
                {
                    m_tBrBasEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_BAS);
                    m_tBrBasEqp.SetConfIdx(m_byConfIdx);
                    m_byBrBasChnnl = byChnIdx;
                    EqpLog("m_byBrBasChnnl = %d\n", m_byBrBasChnnl);
                }
                else
                {
                    bBrBasCap = FALSE;
                    ConfLog(FALSE, "no idle bitrate adapte channel!\n");
                }
        #ifdef _MINIMCU_
            }
        #endif
        }

        if (/*!bAudBasCap || */!bVidBasCap || !bBrBasCap)
        {
            *pwErrCode = ERR_MCU_NOIDLEADAPTER;
            ConfLog(FALSE, "Conference %s create failed because no idle VID/BR adapter!\n",
                    m_tConf.GetConfName());
            return FALSE;
        }

        //标清双双流 适配 或 标清SXGA|fps>5双流适配
        if((m_tConf.GetCapSupportEx().GetSecDSType() != MEDIA_TYPE_NULL &&
            m_tConf.GetCapSupportEx().GetSecDSType() != 0))
        {
            BOOL32 bMpuSufficient = FALSE;
            BOOL32 bMauSufficient = FALSE;
            
            if (!m_cBasMgr.IsHdBasSufficient(m_tConf))
            {
                *pwErrCode = ERR_MCU_NOENOUGH_HDBAS_CONF;
                ConfLog(FALSE, "Conf.%s's has no enough mpu or mau!\n", m_tConf.GetConfName());
                return FALSE;
            }
            if (!m_cBasMgr.OcuppyHdBasChn(m_tConf))
            {
                *pwErrCode = ERR_MCU_OCUPPYMPU;
                ConfLog(FALSE, "Conf.%s's ocuppy mpu chnnl failed!\n", m_tConf.GetConfName());
                return FALSE;
            }
        }
    }

    //音频
    u8     byEqpId;
    u8     byChnIdx;
    BOOL32 bAudBasCap = TRUE;
    u8     byMainAudType;
    u8     bySecondAudType;
    byMainAudType = m_tConf.GetMainAudioMediaType();
    bySecondAudType = m_tConf.GetSecAudioMediaType();
    if (MEDIA_TYPE_NULL != bySecondAudType && byMainAudType != bySecondAudType)
    {
        if (g_cMcuVcApp.GetIdleBasChl(ADAPT_TYPE_AUD, byEqpId, byChnIdx))
        {
            m_tAudBasEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_BAS);
            m_tAudBasEqp.SetConfIdx(m_byConfIdx);
            m_byAudBasChnnl = byChnIdx;
            EqpLog("m_byAudBasChnnl = %d\n", m_byAudBasChnnl);
        }
        else
        {
            bAudBasCap = FALSE;
            ConfLog(FALSE, "no idle audio adapte channel!\n");
        }
    }
    
    if (!bAudBasCap)
    {
        *pwErrCode = ERR_MCU_NOIDLEADAPTER;
        ConfLog(FALSE, "Conference %s create failed because no idle AUD adapter!\n",
                m_tConf.GetConfName());
        
        //释放高清适配
        m_cBasMgr.ReleaseHdBasChn();

        //释放标清主流通道
        if (m_tBrBasEqp.IsNull())
        {
            g_cMcuVcApp.SetBasChanStatus(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, TBasChnStatus::READY);
            g_cMcuVcApp.SetBasChanReserved(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, FALSE);
        }
        if (m_tVidBasEqp.IsNull())
        {
            g_cMcuVcApp.SetBasChanStatus(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, TBasChnStatus::READY);
            g_cMcuVcApp.SetBasChanReserved(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, FALSE);
        }

        return FALSE;
    }

	return TRUE;
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
    memset(&abyVMPId, 0, sizeof(abyVMPId));
    g_cMcuVcApp.GetIdleVMP(abyVMPId, byIdleVMPNum, sizeof(abyVMPId));

    TVMPParam tVmpParam = m_tConfEqpModule.GetVmpModule().GetVmpParam();

    if ( m_tConf.GetConfId().IsNull() )
    {
        ConfLog( FALSE, "[EqpInfoCheck] tConfInfo.IsNull, ignore it\n" );
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
            ConfLog( FALSE, "Conference %s create failed because of multicast address occupied!\n", m_tConf.GetConfName() );
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
            ConfLog( FALSE, "Conference %s create failed because of distributed conf multicast address occupied!\n", m_tConf.GetConfName() );
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
            ConfLog( FALSE, "Conference %s create failed because of no DCS registered!\n", m_tConf.GetConfName() );
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
                ConfLog( FALSE, "the master mcu couldn't restore VMP due to no VMP !\n" );

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
                    ConfLog( FALSE, "Conf.%s created by NPlus error which is impossible, check it !\n", m_tConf.GetConfName() );
                }
            }
            else
            {
                wErrCode = ERR_MCU_NOIDLEVMP;
                ConfLog( FALSE, "Conference %s create failed because no idle vmp!\n", m_tConf.GetConfName() );
                return FALSE;
            }
		}
        else
        {
            m_tConf.m_tStatus.SetVMPMode(CONF_VMPMODE_CTRL);
            m_tConf.m_tStatus.SetVmpBrdst(m_tConfEqpModule.GetVmpModule().m_tVMPParam.IsVMPBrdst());
        }
	}
    
    if( tConfAttrib.IsHasTvWallModule() )
    {
        BOOL32 bTvWallDisconnected = FALSE;  //  是否存在电视墙不在线
        TMultiTvWallModule tMultiTvWallModule;
        m_tConfEqpModule.GetMultiTvWallModule( tMultiTvWallModule );
        TTvWallModule tTvWallModule;
        for( u8 byTvLp = 0; byTvLp < tMultiTvWallModule.GetTvModuleNum(); byTvLp++ )
        {
            tMultiTvWallModule.GetTvModuleByIdx(byTvLp, tTvWallModule);
            if( !g_cMcuVcApp.IsPeriEqpConnected( tTvWallModule.m_tTvWall.GetEqpId()) )
            {
                bTvWallDisconnected = TRUE;
                break;
            }
        }
        if( bTvWallDisconnected )
        {
            memset((void*)&m_tConfEqpModule, 0, sizeof(m_tConfEqpModule));
            wErrCode = ERR_MCU_NOIDLETVWALL;
			ConfLog( FALSE, "Conference %s create failed because Tv %d is outline!\n", 
                                m_tConf.GetConfName(), tTvWallModule.m_tTvWall.GetEqpId() );
			return FALSE;
        }
    }
	
	//是否开始混音, 预留混音器 
	if( tConfAttrib.IsDiscussConf() )
	{
        // 顾振华，在这里先检查是否有混音器 [4/29/2006]
        // 到后面再开始混音
        u8 byEqpId = 0;
	    u8 byGrpId = 0;
        g_cMcuVcApp.GetIdleMixGroup( byEqpId, byGrpId );
        
        if (0 == byEqpId)
		{
            if( CONF_CREATE_MT == m_byCreateBy ) //终端创会不直接返回
            {
                tConfAttrib.SetDiscussConf(FALSE);
                m_tConf.SetConfAttrb(tConfAttrib);
            }
            else
            {
                wErrCode = ERR_MCU_NOIDLEMIXER;
				ConfLog( FALSE, "Conference %s create failed because no idle mixer!\n", m_tConf.GetConfName() );
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
		if (!AssignPrsChnnl())
        {
			// 分配资源失败
            wErrCode = ERR_MCU_NOIDLEPRS;
			ConfLog( FALSE, "Conference %s create failed because no idle prs!\n", m_tConf.GetConfName() );
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
            ConfLog( FALSE, "Conf.%s not support multicast low stream\n", m_tConf.GetConfName() );
            return FALSE;                
        }
    }
#endif

    //会议开始自动录像
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
                ConfLog(FALSE, "Conf %s Request Recorder %d is not online now.\n", 
                                m_tConf.GetConfName(), 
                                tAutoRecAttrb.GetRecEqp());
                wErrCode = ERR_MCU_CONFSTARTREC_MAYFAIL;
				return FALSE;
            }

        #ifdef _MINIMCU_
            if ( ISTRUE(m_byIsDoubleMediaConf) && tAutoRecAttrb.GetRecParam().IsRecLowStream() )
            {
                wErrCode = ERR_MCU_RECLOWNOSUPPORT;
                ConfLog( FALSE, "Conf.%s can't support auto record low stream\n", m_tConf.GetConfName() );
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
        }
    }

    //需动态分屏的会议开始画面合成 
	if( CONF_VMPMODE_AUTO == m_tConf.m_tStatus.GetVMPMode() || 
		tConfAttrib.IsHasVmpModule() )			
	{
		//是否有画面合成器 
		if( 0 != byIdleVMPNum )
		{
            //可以支持当前合成风格的空闲VMP的ID
            u8 byVmpId = 0;

            if ( CONF_VMPMODE_AUTO != m_tConf.m_tStatus.GetVMPMode() )
            {
                //当前所有的空闲VMP是否支持所需的合成风格
                u16 wError = 0;
                if ( IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError) ) 
                {
                    m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
                    m_tVmpEqp.SetConfIdx( m_byConfIdx );
                }
                else
                {
                    if ( CONF_CREATE_NPLUS == m_byCreateBy )
                    {
                        NotifyMcsAlarmInfo( cSrcMsg.GetSrcSsnId(), ERR_MCU_ALLIDLEVMP_NO_SUPPORT_NPLUS );
                        ConfLog( FALSE, "Conference %s create failed because all the idle vmp can't support the VMP style!\n", m_tConf.GetConfName() );

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
                            ConfLog( FALSE, "Conf.%s created by NPlus error which is impossible, check it !\n", m_tConf.GetConfName() );
                        }
                    }
                    else
                    {
                        g_cMcuVcApp.RemoveConf( m_byConfIdx );    
                        wErrCode = wError;
                        ConfLog( FALSE, "Conference %s create failed because all the idle vmp can't support the VMP style!\n", m_tConf.GetConfName() );
                        return FALSE;
                    }
                }
            }
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
                }
                else
                {
                    // N+1备份自动画面合成恢复失败，清理当前所有的状态 [01/16/2007-zbq]
                    if ( CONF_CREATE_NPLUS == m_byCreateBy ) 
                    {
                        NotifyMcsAlarmInfo( cSrcMsg.GetSrcSsnId(), ERR_MCU_ALLIDLEVMP_NO_SUPPORT_NPLUS );
                        ConfLog( FALSE, "Conference %s create failed because all the idle vmp can't support the VMP style!\n", m_tConf.GetConfName() );
                        
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
                            ConfLog( FALSE, "Conf.%s created by NPlus error which is impossible, check it !\n", m_tConf.GetConfName() );
                        }                                
                    }
                    else
                    {
                        //正常会议的创建，按现在的VMP能力的逻辑和保护，不会来到这里。保护起见。[01/16/2007-zbq]
                        ConfLog( FALSE, "Conf.%s created which is impossible, check it !\n", m_tConf.GetConfName() );
                        wErrCode = ERR_MCU_NOIDLEVMP;
                        g_cMcuVcApp.RemoveConf( m_byConfIdx );    
                        ConfLog( FALSE, "Conference %s create failed because no idle vmp!\n", m_tConf.GetConfName() );
                        return FALSE;
                    }
                }
			}
            
            TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TRUE;//先占用,超时后未成功再放弃
			tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpParam;
			tPeriEqpStatus.SetConfIdx( m_byConfIdx );
			g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			
			m_tConf.m_tStatus.SetVmpStyle( tVmpParam.GetVMPStyle() );
			
			u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType; //获取子类型
			u8 byMaxVmpMem = 0;
			if(byVmpSubType != VMP) //新VMP最大成员数均为20
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
            }                                     
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
            g_cMcuVcApp.RemoveConf( m_byConfIdx );
			wErrCode = ERR_MCU_NOIDLEVMP;
			ConfLog( FALSE, "Conference %s create failed because no idle vmp!\n", m_tConf.GetConfName() );
			return FALSE;
		}				
	}

    //把会议指针存到数据区
    if( !g_cMcuVcApp.AddConf( this ) )
    {
        wErrCode = ERR_MCU_CONFOVERFLOW;
        ConfLog( FALSE, "Conference %s create failed because of full instance!\n", m_tConf.GetConfName() );
        return FALSE;
    }

    return TRUE;
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
	BOOL32 bInMtTable ; // xliang [12/26/2008] 

    wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
    pszAliaseBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16));
    
    UnPackTMtAliasArray( pszAliaseBuf, wAliasBufLen, &m_tConf, 
                         tMtAliasArray, awMtDialRate, byMtNum );

	//得到创建者的别名
	if( CONF_CREATE_MT == m_byCreateBy )
	{
        u16 wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
		s32 nLen = sizeof(TConfInfo)+sizeof(u16)+wAliasBufLen;
		if(m_tConf.GetConfAttrb().IsHasTvWallModule())
			nLen += sizeof(TMultiTvWallModule);
		if(m_tConf.GetConfAttrb().IsHasVmpModule())
			nLen += sizeof(TVmpModule);
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
			byMtId = AddMt( tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], m_tConf.m_tStatus.GetCallMode() );
            
			if(!bInMtTable)
			{
				m_byMtIdNotInvite = byMtId; // xliang [12/26/2008] 表明该终端稍后不应作为受邀MT
				CallLog("[ConfdataCoordinate] Mt.%u should not be in Invite table\n", m_byMtIdNotInvite);
			}
				// zbq [12/19/2007] 直接创会终端须在此补入IP，避免E164优先情况下的漏填
            if ( tMtAliasArray[byLoop-1].m_AliasType != mtAliasTypeTransportAddress )
            {
                m_ptMtTable->SetMtAlias( byMtId, ptMtAddr );
                m_ptMtTable->SetIPAddr( byMtId, ptMtAddr->m_tTransportAddr.GetIpAddr() );                    
            }

            // xsl [11/8/2006] 接入板终端计数加1
			// xliang [2/4/2009] 区分是MT还是MCU
			u8 byDriId = cServMsg.GetSrcDriId();
			g_cMcuVcApp.IncMtAdpMtNum( byDriId, m_byConfIdx, byMtId );
            m_ptMtTable->SetDriId(byMtId, byDriId);
			if(byMtType == TYPE_MCU)
			{
				//接入板的终端计数还要+1
				//在mtadplib那端已通过判断，所以肯定有多于2个接入点让其占用
				g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum++;
			}
		}
		else
		{
			//FIXME: 稍后与卫星分散会议拆分开来
			if (tMtAliasArray[byLoop-1].m_AliasType == mtAliasTypeTransportAddress &&
				tMtAliasArray[byLoop-1].m_tTransportAddr.GetIpAddr() == 0xac103c80 &&
				0 != m_tConf.GetSecBitRate())
			{
				awMtDialRate[byLoop-1] = m_tConf.GetSecBitRate();
			}

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
				m_ptMtTable->SetMtType(byMtId, MT_TYPE_MMCU);
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
	if( m_tConf.GetConfAttrb().IsHasTvWallModule() )
	{
        TPeriEqpStatus tTWStatus;
		TMultiTvWallModule *ptMultiTvWallModule = (TMultiTvWallModule *)pszMsgBuf;
        m_tConfEqpModule.SetMultiTvWallModule( *ptMultiTvWallModule );
        m_tConfEqpModule.SetTvWallInfo( *ptMultiTvWallModule );

        TTvWallModule tTvWallModule;
        u8 byMtIndex = 0;
        u8 byMemberType = 0;
        for( u8 byTvLp = 0; byTvLp < ptMultiTvWallModule->GetTvModuleNum(); byTvLp++ )
        {
            ptMultiTvWallModule->GetTvModuleByIdx(byTvLp, tTvWallModule);
            g_cMcuVcApp.GetPeriEqpStatus(tTvWallModule.m_tTvWall.GetEqpId(), &tTWStatus);
			for( byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++ )
			{
				byMtIndex = tTvWallModule.m_abyTvWallMember[byLoop]; 
                byMemberType = tTvWallModule.m_abyMemberType[byLoop];
				if( byMtIndex > 0 )//索引以1为基准
				{
                    if( byMtIndex <= MAXNUM_CONF_MT )
                    {
                        byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAliasArray[byMtIndex-1] );
                        if( byMtId > 0 )
                        {
                            TMt tMt = m_ptMtTable->GetMt( byMtId );
                            m_tConfEqpModule.SetTvWallMemberInTvInfo( tTvWallModule.m_tTvWall.GetEqpId(), byLoop, tMt );
                        
                            // 顾振华 [5/11/2006] 这里支持指定跟随方式
                            // tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = TW_MEMBERTYPE_MCSSPEC;
                            tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byMemberType;
                            tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
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
                            ConfLog(FALSE, "Invalid member type %d for idx %d in CreateConf TvWall Module\n", 
                                    byMemberType, 
                                    byLoop);        
                            bValid = FALSE;
                        }
                        if (bValid)
                        {
                            // 标记下来，等终端上来。会触发主席发言人变更
                            m_tConfEqpModule.SetTvWallMemberInTvInfo( tTvWallModule.m_tTvWall.GetEqpId(), byLoop, tMt );
                            tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byMemberType;
                            tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
                        }                            
                    }
                    else
                    {
                        ConfLog(FALSE, "Invalid MtIndex :%d in CreateConf TvWall Module\n", byMtIndex);
                    }
				}
			}
            g_cMcuVcApp.SetPeriEqpStatus(tTvWallModule.m_tTvWall.GetEqpId(), &tTWStatus);
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

	//处理画面合成模板 
	if( m_tConf.GetConfAttrb().IsHasVmpModule() )
	{
        TVMPParam tVmpParam;
		memset( &tVmpParam, 0 ,sizeof(tVmpParam) );
        
		TVmpModule *ptVmpModule = (TVmpModule*)pszMsgBuf;
		tVmpParam = ptVmpModule->m_tVMPParam;
        u8 byMtIndex = 0;
        u8 byMemberType = 0;
		for( byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
		{
			byMtIndex = ptVmpModule->m_abyVmpMember[byLoop];
            byMemberType = ptVmpModule->m_abyMemberType[byLoop];
			if( byMtIndex > 0 )
			{
                if(byMtIndex <= MAXNUM_CONF_MT)
                {
                    byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAliasArray[byMtIndex-1] );
                    if( byMtId > 0 )
                    {
                        TMt tMt = m_ptMtTable->GetMt( byMtId );
						m_tConfEqpModule.SetVmpMember( byLoop, tMt ); 

						// xliang [4/8/2009] 小心SetVmpMember接口的用意。tVmpParam要补一下
						TVMPMember tVmpMember;
						tVmpMember.SetMemberTMt( tMt );
						tVmpMember.SetMemberType( byMemberType ); //此处member type是会控指定
						tVmpParam.SetVmpMember( byLoop, tVmpMember );
						
                    }
                }	
                // 如果是 193，说明是其他跟随方式
                else if (byMtIndex == MAXNUM_CONF_MT + 1)
                {
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
                    case VMP_MEMBERTYPE_POLL:
                        // 轮询，创会时肯定没有轮询
                        tMt.SetNull();
                        bValid = TRUE;
                        break;
                    default:
                        ConfLog(FALSE, "Invalid member type %d for idx %d in CreateConf Vmp Module\n", 
                                byMemberType, 
                                byLoop);        
                        bValid = FALSE;
                    }
                    if (bValid)
                    {
                        m_tConfEqpModule.SetVmpMember( byLoop, tMt );

						// xliang [4/8/2009] 小心SetVmpMember接口的用意。tVmpParam要补一下
						TVMPMember tVmpMember;
						tVmpMember.SetMemberTMt( tMt );
						tVmpMember.SetMemberType( byMemberType ); 
						tVmpParam.SetVmpMember( byLoop, tVmpMember );
                    }                          
                }
                else
                {
                    ConfLog(FALSE, "Invalid MtIndex :%d in CreateConf Vmp Module\n", byMtIndex);
                }
			}
		}

		ptVmpModule->SetVmpParam( tVmpParam );
		m_tConfEqpModule.SetVmpModule( *ptVmpModule );
	}
    else
    {
        TVmpModule tVmpModule;
        memset( &tVmpModule, 0, sizeof(tVmpModule) );
        m_tConfEqpModule.SetVmpModule( tVmpModule );
		TMt tNullMt;
        tNullMt.SetNull();
        for( byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
		{
			m_tConfEqpModule.SetVmpMember( byLoop, tNullMt );
		}
    }

    //zbq[11/18/2008] 启用适配的快捷支持
    TConfAttrb tAttrb = m_tConf.GetConfAttrb();
    
    if (0 != m_tConf.GetSecBitRate() ||
        (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() &&
         0 != m_tConf.GetSecVideoMediaType()))
    {
        tAttrb.SetUseAdapter(TRUE);
    }
    else
    {
        if (
            (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
             (m_tConf.GetConfAttrbEx().IsResEx1080() ||
              m_tConf.GetConfAttrbEx().IsResEx720()  ||
              m_tConf.GetConfAttrbEx().IsResEx4Cif() ||
              m_tConf.GetConfAttrbEx().IsResExCif())
            ) 
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
	if(m_tConf.m_tStatus.IsRegToGK())
	{
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

	//会议注册GK以后才邀请终端
	if(m_tConfInStatus.IsInviteOnGkReged())
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
                GetNPlusDataFromConf( tConfData );
                cServMsg.SetConfId( m_tConf.GetConfId() );
                cServMsg.SetMsgBody( (u8*)&tConfData, sizeof(tConfData) );
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
        ConfLog( FALSE, "[ProcGKChargeRsp] unexpected msg.%d<%s> received !\n", 
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
			MtLog("[ProcMcsMcuReleaseConfReq] VCS Conf cann't be released by mt\n");
			return;
		}
		
		
		//应答
		if( cServMsg.GetEventId() == MCS_MCU_RELEASECONF_REQ )
		{		
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}      

		//结束会议
		ReleaseConf( TRUE );
        
        NEXTSTATE( STATE_IDLE );
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
		ConfLog(FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
				OspPrintf( TRUE, FALSE, "\n\n Cannot get speaker's Mt Alias.\n\n");
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
				OspPrintf( TRUE, FALSE, "\n\n Cannot get chairman's Mt Alias.\n\n");
			}
		}
		
		//保存到文件
		g_cMcuVcApp.SaveConfToFile( m_byConfIdx, FALSE, bySaveDefault );
			
		break;

	default :
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
        
		//if too old, send Nack; Duration time is 0 denoted that meeting could be ended at any time
		if( ( time( NULL ) > tConf.GetStartTime() + 30 * 60 || 
			  ( tConf.GetDuration() != 0 && 
			    time( NULL ) > tConf.GetStartTime() + tConf.GetDuration() * 60 ) ) )
		{
			cServMsg.SetErrorCode( ERR_MCU_STARTTIME_WRONG );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfLog( FALSE, "Conference %s too late and canceled!\n", m_tConf.GetConfName() );
			return;
		}
		
		//会议的E164号码已存在，拒绝 
		if( 0 != strcmp( (char*)tConf.GetConfE164(), (char*)m_tConf.GetConfE164() ) &&
			g_cMcuVcApp.IsConfE164Repeat( tConf.GetConfE164(), FALSE ) )
		{
			cServMsg.SetErrorCode( ERR_MCU_CONFE164_REPEAT );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfLog( FALSE, "Conference %s E164 repeated and create failure!\n", m_tConf.GetConfName() );
			return;
		}

		//会议名已存在，拒绝
		if( 0 != strcmp( (char*)tConf.GetConfName(), (char*)m_tConf.GetConfName() ) &&
			g_cMcuVcApp.IsConfNameRepeat( tConf.GetConfName(), FALSE ) )
		{
			cServMsg.SetErrorCode( ERR_MCU_CONFNAME_REPEAT );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfLog( FALSE, "Conference %s name repeated and create failure!\n", m_tConf.GetConfName() );
			return;
		}

		//dynamic vmp and vmp module conflict
		if( tConfAttrib.IsHasVmpModule() && tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_AUTO)
		{
			cServMsg.SetErrorCode( ERR_MCU_DYNAMCIVMPWITHMODULE );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfLog( FALSE, "Conference %s create failed because has module with dynamic vmp!\n", m_tConf.GetConfName() );
			return;		
		}

	    //无效的会议保护方式,拒绝
		if( tConf.m_tStatus.GetProtectMode() > CONF_LOCKMODE_LOCK )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_CONFLOCKMODE );
			SendMsgToMcs( cServMsg.GetSrcSsnId() , MCU_MCS_MODIFYCONF_NACK, cServMsg );
			ConfLog( FALSE, "Conference %s protect mode invalid and nack!\n", m_tConf.GetConfName() );
			return;
		}

		//无效的会议呼叫策略,拒绝
		if( tConf.m_tStatus.GetCallMode() > CONF_CALLMODE_TIMER )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_CALLMODE );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			ConfLog( FALSE, "Conference %s call mode invalid and nack!\n", m_tConf.GetConfName() );
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
		
		ConfLog( FALSE, "A scheduled conference %s modified and restart!\n",m_tConf.GetConfName() );

		//release conference
		ReleaseConf( TRUE );

		NEXTSTATE( STATE_IDLE );	
		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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

            ConfLog(FALSE, "Conf %s is manual release. Can not delay!\n", m_tConf.GetConfName());
            return;
        }

        if (MT_MCU_DELAYCONF_REQ == pcMsg->event)
        {
            TMt tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
            if (!(tMt == m_tConf.GetChairman()))
            {
                ConfLog(FALSE, "Conf %s has delayed fail by no chairman!\n", m_tConf.GetConfName());
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
		m_tConf.SetDuration( m_tConf.GetDuration() + ntohs( wDelayTime ) );        

		//Notification
		BroadcastToAllSubMtJoinedConf( MCU_MT_DELAYCONF_NOTIF, cServMsg ); 		
		SendMsgToAllMcs( MCU_MCS_DELAYCONF_NOTIF, cServMsg );
		
		if( pcMsg->event == MT_MCU_DELAYCONF_REQ )
		{
			ConfLog( FALSE, "Conf %s has delayed %d minute by chairman!\n", 
				 m_tConf.GetConfName(),  ntohs( wDelayTime ) );
		}
		else
		{
			ConfLog( FALSE, "Conf %s has delayed %d minute by mcs!\n", 
				 m_tConf.GetConfName(),  ntohs( wDelayTime ) );
		}

		
		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
            ConfLog( FALSE, "[ProcMcsMcuChangeVacHoldTimeReq] distributed conf not supported vac\n");
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
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
			ConfLog( FALSE, "Same chairman specified in conference %s! failure!\n", 
				m_tConf.GetConfName() );
			cServMsg.SetErrorCode( ERR_MCU_SAMECHAIRMAN );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	//nack
			return;
		}

		//新主席不与会
		if( !m_tConfAllMtInfo.MtJoinedConf( tNewChairman.GetMtId() ) )
		{
			ConfLog( FALSE, "New chairman MT%u not in conference %s! Error!\n", 
				    tNewChairman.GetMtId(), m_tConf.GetConfName() );
			cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	//nack
			return;
		}

		//应答
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );	//ack

		//改变主席
		ChangeChairman( &tNewChairman );

		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
				
        //应答
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );


		//取消主席
		ChangeChairman( NULL );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	CServMsg	cServMsg( pcMsg->content, pcMsg->length ), cTempMsg;
	TMt			tNewSpeaker;
	TMt         tRealMt;
	TMtStatus	tMtStatus;

	STATECHECK

    cServMsg.SetErrorCode(0);
		
	//处于演讲模式的语音激励控制发言状态,不能指定发言人
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
	if( m_tConf.m_tStatus.IsPlaying() )
	{
		cServMsg.SetErrorCode( ERR_MCU_CONFPLAYING );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		ConfLog( FALSE, "It playing now in conference %s! Cannot spcify the speaker!\n", 
			   m_tConf.GetConfName() );
		return;
	}

	tNewSpeaker = *( TMt * )cServMsg.GetMsgBody();
	tRealMt = tNewSpeaker;
	if( tNewSpeaker.GetMcuId() == LOCAL_MCUID )
	{
		tNewSpeaker = m_ptMtTable->GetMt( tNewSpeaker.GetMtId() );
	}
	else
	{
		tNewSpeaker = m_ptMtTable->GetMt( tNewSpeaker.GetMcuId());
	}

	//新发言人未与会
	if( tNewSpeaker.GetType() == TYPE_MT && 
		!m_tConfAllMtInfo.MtJoinedConf( tNewSpeaker.GetMcuId(), tNewSpeaker.GetMtId() ) )
	{
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		ConfLog( FALSE, "New speaker MT%u not in conference %s! Error!\n", 
			   tNewSpeaker.GetMtId(), m_tConf.GetConfName() );
		return;
	}

	//新发言人是只接收终端
	m_ptMtTable->GetMtStatus(tNewSpeaker.GetMtId(), &tMtStatus);
	if( tNewSpeaker.GetType() == TYPE_MT && 
		( !tMtStatus.IsSendVideo() ) && 
		( !tMtStatus.IsSendAudio() ) )
	{
		cServMsg.SetErrorCode( ERR_MCU_RCVONLYMT );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	
		ConfLog( FALSE, "New speaker MT%u is receive only MT! Error!\n", 
				         tNewSpeaker.GetMtId() );
		return;
	}
    
    //适配能力检查，如没有相应的适配能力，则给出提示！
    if (!IsHDConf(m_tConf) &&
        m_tConf.GetConfAttrb().IsUseAdapter())
    {
        BOOL32 byFlag = TRUE;
        u16 wAdaptBitRate = 0;
        TSimCapSet tSrcSimCapSet;
        TSimCapSet tDstSimCapSet;

        if (!tNewSpeaker.IsNull())
        {
            if (!g_cMcuVcApp.IsPeriEqpConnected(m_tVidBasEqp.GetEqpId())
                && ( IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, &tNewSpeaker) || IsNeedCifAdp()) )
            {
                byFlag &= FALSE;
            }

            if (!g_cMcuVcApp.IsPeriEqpConnected(m_tBrBasEqp.GetEqpId()) &&
                IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, &tNewSpeaker))
            {
                byFlag &= FALSE;
            }

            if (!g_cMcuVcApp.IsPeriEqpConnected(m_tAudBasEqp.GetEqpId()) &&
                IsNeedAudAdapt(tDstSimCapSet, tSrcSimCapSet, &tNewSpeaker))
            {
                byFlag &= FALSE;
            }

            if (FALSE == byFlag)
            {
                cServMsg.SetErrorCode(ERR_BASCHANNEL_INVALID);
                ConfLog(FALSE, "New speaker MT%u need bas! Error!\n", tNewSpeaker.GetMtId());
            }
        }
    }

	if(!tRealMt.IsLocal() )
	{
		//目前暂不支持下级mcu的终端状态通知,故能力集判断先关闭
		/*
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tRealMt.GetMcuId());
		if(ptConfMcInfo != NULL)
		{
			TMcMtStatus *ptStatus = ptConfMcInfo->GetMtStatus(tRealMt);
			if(ptStatus != NULL)
			{
				if( ( !ptStatus->IsSendVideo() ) && 
					( !ptStatus->IsSendAudio() ) ) 
				{
					cServMsg.SetErrorCode( ERR_MCU_RCVONLYMT );
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	
					ConfLog( FALSE, "New speaker MCU%uMT%u is receive only MT! Error!\n", 
						tRealMt.GetMcuId(), tRealMt.GetMtId() );
					return;
				}
			}
		}
		*/
	}

	//新发言人已是发言人
	if( tNewSpeaker == GetLocalSpeaker() )	
	{			
		ConfLog( FALSE, "Same speaker specified in conference %s! Cannot spcify the speaker!\n", 
				m_tConf.GetConfName() );
	}

    // xsl [7/20/2006] 卫星分散会议时需要判断回传通道数, 若支持替换需要考虑目的终端是否在vmp、tvwall、会控监控、被选看、mtw、终端录像、双流、发言等情况
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        if (IsOverSatCastChnnlNum(tNewSpeaker.GetMtId()))
        {
            ConfLog(FALSE, "[ProcMcsMcuSpecSpeakerReq] over max upload mt num. nack!\n");
            
            if( cServMsg.GetEventId() != MT_MCU_SPECSPEAKER_CMD )
            {
                cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            }            
            return;
        }
    }    

	//应答
	if( cServMsg.GetEventId() != MT_MCU_SPECSPEAKER_CMD )
	{
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	}
	
	//改变发言人
	ChangeSpeaker( &tRealMt );

    //下级Mc只能指定本级终端的发言
	if( !tRealMt.IsLocal() )
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tRealMt.GetMcuId());
		if(ptConfMcInfo != NULL)
		{
			TMt tMt; //自己
			tMt.SetMcuId(tRealMt.GetMcuId());
			tMt.SetMtId(0);
			TMcMtStatus *ptStatus = ptConfMcInfo->GetMtStatus(tMt);
			if(ptStatus != NULL)
			{
				OnMMcuSetIn(tRealMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_BROADCAST);
                
                // 将下级发言人加入定制混音列表中
                if( m_tConf.m_tStatus.IsSpecMixing() )
                {
                    // xsl [7/28/2006] 若已经在混音则不重复加入
                    if (!m_ptConfOtherMcTable->IsMtInMixing(tRealMt))
                    {
                        TMcu tMcu;
                        tMcu.SetMcu(tRealMt.GetMcuId());
                        cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
                        cServMsg.CatMsgBody((u8 *)&tRealMt, sizeof(TMt));
                        OnAddRemoveMixToSMcu(&cServMsg, TRUE);   
                    }                     
                }                
			}
		}
	}
	
	// xliang [5/26/2009] 通知MCS或MT销毁window. FIXME
	/*
	CServMsg cMsg;
	if(MT_MCU_SPECSPEAKER_CMD == cServMsg.GetEventId())
	{

	}
	else if(MCS_MCU_SPECSPEAKER_REQ)
	{
		//通知主席MT销毁相关对话框
		if( HasJoinedChairman() )
		{
			TMt tMt = m_tConf.GetChairman();
			//...
		}
	}
	*/

	
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
		tSpeaker = m_tConf.GetSpeaker();
		if( !m_tConf.HasSpeaker() )
		{
			if( cServMsg.GetEventId() == MCS_MCU_CANCELSPEAKER_REQ )
			{
				cServMsg.SetErrorCode( ERR_MCU_NOSPEAKER );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			}
			ConfLog( FALSE, "No speaker in conference %s now! Cannot cancel the speaker!\n", 
				m_tConf.GetConfName() );
			return;
		}
				
		//应答
		if( cServMsg.GetSrcMtId() == 0 )	//not MT source
		{
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}
		
		//停组播
		if( m_ptMtTable->IsMtMulticasting( tSpeaker.GetMtId() ) )
		{
			g_cMpManager.StopMulticast( tSpeaker, 0, MODE_BOTH );
			m_ptMtTable->SetMtMulticasting( tSpeaker.GetMtId(), FALSE );
		}
		
		//取消发言人
		ChangeSpeaker( NULL );

		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
                ConfLog(FALSE, "[ProcMcsMcuSeeSpeakerCmd] MtId<%d> isn't chairman, ignore it!\n");
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
            ConfLog(FALSE, "No must see speaker while mixing!\n");
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
			if(m_tConf.m_tStatus.IsBrdstVMP() && m_tVidBrdSrc == m_tVmpEqp)
			{
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				if(byVmpSubType != VMP)
				{
					bNewVmpBrd = TRUE;
				}
			}

            //不论是否有广播源: 清选看, 收看(收听)广播源
            for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
            {
				TMtStatus tMtStatus;
                if ( m_tConfAllMtInfo.MtJoinedConf( byMtId ) ) 
                {
                    bRestoreAud = TRUE;
                    bRestoreVid = TRUE;

                    TMt tMtVideo;
                    TMt tMtAudio;                    

                    m_ptMtTable->GetMtSrc( byMtId, &tMtVideo, MODE_VIDEO );
                    m_ptMtTable->GetMtSrc( byMtId, &tMtAudio, MODE_AUDIO );

                    if (GetLocalSpeaker() == m_ptMtTable->GetMt(byMtId))
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

                    //有音频源
                    if ( !tMtAudio.IsNull() && bRestoreAud)
                    {
                        //指定恢复接收音频广播媒体源
                        RestoreRcvMediaBrdSrc( byMtId, MODE_AUDIO );        
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
								TMt tDstMt = m_ptMtTable->GetMt(byMtId);
								SwitchNewVmpToSingleMt(tDstMt);	//画面合成广播单独建交换
							}
						}
						else
						{
							//指定恢复接收视频广播媒体源
							RestoreRcvMediaBrdSrc( byMtId, MODE_VIDEO );
						}
					}
                    //清除原来保存用于恢复的选看状态，因强制广播后毋需恢复
                    TMt tNullMt;
                    tNullMt.SetNull();

                    m_ptMtTable->GetMtStatus( byMtId, &tMtStatus );                
                    tMtStatus.SetSelectMt(tNullMt, MODE_AUDIO);
                    // guzh [11/7/2007] Fixme Video应该告看自己
                    tMtStatus.SetSelectMt(tNullMt, MODE_VIDEO);
                
                    m_ptMtTable->SetMtStatus( byMtId, &tMtStatus );              
                }
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
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
	if (mtAliasTypeE164 == ptAddMtInfo->m_AliasType)
	{
		if (0 == strcmp(ptAddMtInfo->m_achAlias, m_tConf.GetConfE164()))
		{
			cServMsg.SetErrorCode(ERR_MCU_NOTCALL_CONFITSELF);
			SendReplyBack(cServMsg, MCU_MCS_ADDMT_NACK);
			ConfLog(FALSE, "[ProcMcsMcuAddMtExReq]CMcuVcInst: Cannot Call conf itself.\n");
			return;
		}
	}

	// 级联
	if (LOCAL_MCUID != tMcu.GetMcuId())
	{
		u8 byMtId = tMcu.GetMcuId();
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
    
		ConfLog(FALSE, "[ProcMcsMcuAddMtExReq]Mcs add mt-%d DialBitRate-%d Alias-", 
				byMtId, ptAddMtInfo[byLoop].GetCallBitRate());
		if ( ptAddMtInfo[byLoop].m_AliasType == mtAliasTypeTransportAddress )
		{
			OspPrintf(TRUE, FALSE, "%s:%d!\n",				
					  StrOfIP(ptAddMtInfo[byLoop].m_tTransportAddr.GetIpAddr()), 
					  ptAddMtInfo[byLoop].m_tTransportAddr.GetPort() );
		}
		else
		{
			OspPrintf(TRUE, FALSE, "%s!\n",	ptAddMtInfo[byLoop].m_achAlias);
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
			OspPrintf(TRUE, FALSE, "[ProcMcsMcuAddMtExReq]MainMaxBitRate-%d MainMediaType-%d MainResolution-%d MainFrameRate-%d\n",
				                    tCapInfo.GetMainMaxBitRate(), tCapInfo.GetMainMediaType(),
							        tCapInfo.GetMainResolution(), 
									tCapInfo.IsMainFrameRateUserDefined()?tCapInfo.GetUserDefMainFrameRate():tCapInfo.GetMainFrameRate());
			
			OspPrintf(TRUE, FALSE, "[ProcMcsMcuAddMtExReq]DStreamMaxBitRate-%d DStreamMediaType-%d DStreamResolution-%d DStreamFrameRate-%d\n",
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
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//发给会控终端表
	SendMtListToMcs(LOCAL_MCUID);

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
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	u8   byMtNum = 0;
	TMcu tMcu;
    tMcu.SetNull();
	TAddMtInfo  *ptAddMtInfo = NULL;

	if (MCU_MCU_INVITEMT_REQ == cServMsg.GetEventId())
	{
        if (cServMsg.GetMsgBodyLen() >= sizeof(TMcuMcuReq)+sizeof(TMt))
        {
		    byMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TMcuMcuReq)-sizeof(TMt) )/sizeof( TAddMtInfo);
		    ptAddMtInfo = (TAddMtInfo *)(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq)+sizeof(TMcu));
		    tMcu = *(TMcu*)(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq)); 
        }
	}
	else
	{
        if (cServMsg.GetMsgBodyLen() >= sizeof(TMcu))
        {
		    tMcu = *(TMcu*)(cServMsg.GetMsgBody());
		    byMtNum= ( cServMsg.GetMsgBodyLen() - sizeof(TMt) )/sizeof( TAddMtInfo );
		    ptAddMtInfo = (TAddMtInfo *)(cServMsg.GetMsgBody()+sizeof(TMcu));

			for (u8 byIdx = 0; byIdx < byMtNum; byIdx ++)
			{
				//FIXME: 卫星会议临时修改，稍后与卫星分散会议拆分
				if (ptAddMtInfo[byIdx].m_AliasType == mtAliasTypeTransportAddress &&
					ptAddMtInfo[byIdx].m_tTransportAddr.GetIpAddr() == 0xac103c80)
				{
					if (0 != m_tConf.GetSecBitRate())
					{
						ptAddMtInfo->m_wDialBitRate = m_tConf.GetSecBitRate();
					}
				}
			}
        }
	}

	u8  byLoop, byMtId;
	TMt tMt;

	STATECHECK


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
	if (mtAliasTypeE164 == ptAddMtInfo->m_AliasType)
	{
		if (0 == strcmp(ptAddMtInfo->m_achAlias, m_tConf.GetConfE164()))
		{
			cServMsg.SetErrorCode(ERR_MCU_NOTCALL_CONFITSELF);
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			ConfLog(FALSE, "CMcuVcInst: Cannot Call conf itself.\n");
			return;
		}
	}

	//是否在本级上增加
	if (LOCAL_MCUID != tMcu.GetMcuId())
	{
		byMtId = tMcu.GetMcuId();
		if (!m_tConfAllMtInfo.m_tLocalMtInfo.MtJoinedConf(byMtId))
		{
			cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
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

		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
	}

	//主动邀请, ack
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	//增加受邀终端列表
	for (byLoop = 0; byLoop < byMtNum; byLoop++)
	{
		byMtId = AddMt( ptAddMtInfo[byLoop], ptAddMtInfo[byLoop].GetCallBitRate(), 
                        (ptAddMtInfo[byLoop].m_byCallMode == CONF_CALLMODE_TIMER || ptAddMtInfo[byLoop].m_byCallMode == CONF_CALLMODE_NONE) ? ptAddMtInfo[byLoop].m_byCallMode : m_tConf.m_tStatus.GetCallMode() );
        
		ConfLog(FALSE, "Mcs add mt-%d DialBitRate-%d type-%d alias-", 
				byMtId, ptAddMtInfo[byLoop].GetCallBitRate(), 
				ptAddMtInfo[byLoop].m_AliasType );
        if ( ptAddMtInfo[byLoop].m_AliasType == mtAliasTypeTransportAddress )
        {
            OspPrintf(TRUE, FALSE, "%s:%d!\n",				
                      StrOfIP(ptAddMtInfo[byLoop].m_tTransportAddr.GetIpAddr()), 
                      ptAddMtInfo[byLoop].m_tTransportAddr.GetPort() );
        }
        else
        {
            OspPrintf(TRUE, FALSE, "%s!\n",	ptAddMtInfo[byLoop].m_achAlias);
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
			InviteUnjoinedMt( cServMsg, &tMt, TRUE, TRUE );

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
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//发给会控终端表
	SendMtListToMcs(LOCAL_MCUID);

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
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt tMt;
	if(cServMsg.GetEventId() == MCU_MCU_DELMT_REQ)
	{
		tMt = *( TMt * )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));
	}
	else
	{
		tMt = *( TMt * )cServMsg.GetMsgBody();
	}

	STATECHECK

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

	TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMcuId());
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
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//不是本级MCU下的终端
    if( !tMt.IsLocal() )
	{
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
			sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
		astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
			sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		
		SendMsgToMt( tMt.GetMcuId(), MCU_MCU_DELMT_REQ, cServMsg );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
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
	RemoveMt( tMt, TRUE );
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
====================================================================*/
void CMcuVcInst::ProcMcsMcuSendMsgReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u16 wMtNum = *(u16*)cServMsg.GetMsgBody();
	TMt	*ptMt = (TMt *)( cServMsg.GetMsgBody() + sizeof(u16) );
	CRollMsg* ptROLLMSG = (CRollMsg*)( cServMsg.GetMsgBody() + ntohs(wMtNum)*sizeof(TMt) + sizeof(u16) );

    CServMsg	cSendMsg;
	TMt tMt;
    
    // xsl [11/3/2006] 由配置文件决定是否将终端的短消息转发给会议中的终端
    if (MT_MCU_SENDMSG_CMD == pcMsg->event)
    {
        if (!g_cMcuVcApp.IsTransmitMtShortMsg())
        {
            ConfLog(FALSE, "[ProcMcsMcuSendMsgReq] not permit transmit mt short message in mcu config\n");
            return;
        }
        // guzh [4/7/2007] 级联MCU的消息入口是 MCU_MCU_SENDMSG_NOTIF
        /*
		//阻止下级向上级传递短消息, zgc, 2007-04-06
		if ( m_ptMtTable->GetMtType( cServMsg.GetSrcMtId() ) == MT_TYPE_SMCU )
		{
			if( !HasJoinedChairman()
				|| m_tConf.GetChairman().GetMtId() != cServMsg.GetSrcMtId()
			  )
			{
				ConfLog(FALSE, "[ProcMcsMcuSendMsgReq] Can not transmit short message from smcu to mmcu!\n");
				return;
			}
		}		
        */
    }

	switch( CurState() )
	{
	case STATE_ONGOING:

		if( wMtNum == 0 )	//发给所有终端 MCU_MT_SENDMSG_NOTIF（可能包括上下级MCU，在对端收到时处理）
		{
			tMt.SetNull();
			cSendMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
			cSendMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );
			BroadcastToAllSubMtJoinedConf( MCU_MT_SENDMSG_NOTIF, cSendMsg );
		}
		else //发给某些终端 
		{
			for( u16 wLoop = 0; wLoop < ntohs(wMtNum); wLoop++ )
			{
				if( ptMt->IsLocal() )
				{				
					tMt = m_ptMtTable->GetMt( ptMt->GetMtId() );
					cSendMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
					cSendMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );
					SendMsgToMt( tMt.GetMtId(), MCU_MT_SENDMSG_NOTIF, cSendMsg );
				}
				else
				{
					TMt tMcuMt = m_ptMtTable->GetMt( ptMt->GetMcuId() );
					TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMt->GetMcuId());
					if(ptMcInfo == NULL)
					{
						continue;
					}
					cSendMsg.SetMsgBody( (u8*)ptMt, sizeof(TMt) );
					cSendMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );
					SendMsgToMt( tMcuMt.GetMtId(), MCU_MCU_SENDMSG_NOTIF, cSendMsg );
				}
				ptMt++;
			}
		}
		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetMtListReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	//得到MCU号
	TMcu tMcu = *(TMcu*)cServMsg.GetMsgBody(); 

	//所有终端列表
	if( tMcu.GetMcuId() == 0 )
	{	
		//先发其它MC
		for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
		{
			u8 byMcuId = m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId();
            // guzh [4/30/2007] 上级MCU列表过滤
			if( byMcuId != 0 && 
                ( byMcuId != m_tCascadeMMCU.GetMtId() || g_cMcuVcApp.IsShowMMcuMtList() )
               )      
			{					
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( byMcuId );
				tMcu.SetMcu( m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId() );
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
			}
		}
	    //本级的最后发 
		tMcu.SetMcu( LOCAL_MCUID );
	}		

	//不是本级的MCU
	if(!tMcu.IsLocal() )
	{
        // guzh [4/30/2007] 上级MCU列表过滤
        if ( !g_cMcuVcApp.IsShowMMcuMtList() && 
             !m_tCascadeMMCU.IsNull() && m_tCascadeMMCU.GetMtId() == tMcu.GetMcuId() )
        {
        }
        else
        {
            TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( tMcu.GetMcuId() );
            cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
            for(s32 nLoop=0; nLoop<MAXNUM_CONF_MT; nLoop++)
            {
                if( ptConfMcInfo->m_atMtStatus[nLoop].IsNull() || 
                    ptConfMcInfo->m_atMtStatus[nLoop].GetMtId() == 0 ) //自己
                {
                    continue;
                }
                //TMcMtStatus:public TMtStatus
                cServMsg.CatMsgBody((u8 *)&(ptConfMcInfo->m_atMtExt[nLoop]), sizeof(TMtExt));
            }
            
            SendReplyBack( cServMsg, MCU_MCS_MTLIST_NOTIF );
        }        
	}
	else
	{
		//得到本级别名列表
		cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
		cServMsg.CatMsgBody( ( u8 * )m_ptMtTable->m_atMtExt, 
			                 m_ptMtTable->m_byMaxNumInUse * sizeof( TMtExt ) );
		SendReplyBack( cServMsg, MCU_MCS_MTLIST_NOTIF );
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
====================================================================*/
void CMcuVcInst::ProcMcsMcuRefreshMcuCmd( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	//得到MCU号
	TMcu tMcu = *(TMcu*)cServMsg.GetMsgBody(); 

	for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
	{
		u8 byMcuId = m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId();
		if( byMcuId == tMcu.GetMcuId() )
		{
			//请求终端列表
			TMcuMcuReq tReq;
			memset(&tReq, 0, sizeof(tReq));
			cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
			SendMsgToMt( byMcuId, MCU_MCU_MTLIST_REQ,  cServMsg);
			
			if( m_tCascadeMMCU.IsNull() || 
				(!m_tCascadeMMCU.IsNull() && m_tCascadeMMCU.GetMtId() != byMcuId) )
			{
				//为下级mcu，获取下级直连终端状态
				OnGetMtStatusCmdToSMcu( byMcuId );
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
            TMcuHdBasStatus tStatus;
            m_cBasMgr.GetHdBasStatus(tStatus, m_tConf);
            cServMsg.SetMsgBody((u8*)&tStatus, sizeof(tStatus));
            SendReplyBack( cServMsg, cServMsg.GetEventId()+1 );
        }
        break;
    default:
        OspPrintf(TRUE, FALSE, "[ProcMcsMcuGetConfInfoReq] unexpected msg.%d<%s>!\n",
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
====================================================================*/
BOOL32 CMcuVcInst::CheckVmpParam(TVMPParam& tVmpParam, u16& wErrorCode)
{
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

	// 成员设置错误则直接清空
	TMt tMt;
	for(u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		TVMPMember tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		TMt tTmpMt = GetLocalMtFromOtherMcuMt(tVMPMember);
		tMt.SetNull();
// 			TMt tTmpPollMt = m_tConf.m_tStatus.GetPollInfo()->GetMtPollParam().GetTMt();		

		if( tTmpMt.GetMtId() != 0 )
		{
			if( ( VMP_MEMBERTYPE_CHAIRMAN == tVMPMember.GetMemberType() && !HasJoinedChairman() ) 
				|| ( VMP_MEMBERTYPE_SPEAKER == tVMPMember.GetMemberType() 
					&& // 顾振华 [5/23/2006] 只允许终端进入通道，不允许放像设备
					( !HasJoinedSpeaker() || GetLocalSpeaker().GetType() == TYPE_MCUPERI ) )
				||( VMP_MEMBERTYPE_POLL == tVMPMember.GetMemberType()  && 
					(POLL_STATE_NONE == m_tConf.m_tStatus.GetPollState() /*||
 					 // xliang [3/30/2009] 轮询跟随情况,调整Param. 否则之后判断同一个MT进多个通道限制会误判
				     (!tTmpPollMt.IsNull() && !(tTmpPollMt == tTmpMt))*/))
			  )
			{							
				tVMPMember.SetMemberTMt( tMt );
			}
			else if (!m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMtId()))
			{
				// guzh [9/26/2006] 终端不在线
				tVMPMember.SetMemberTMt( tMt );
			}
			else if ( tVMPMember.GetMcuId() != LOCAL_MCUID )
			{
				//有风险
				OnMMcuSetIn(tVMPMember, 0, SWITCH_MODE_SELECT);
				
				tMt = m_ptMtTable->GetMt( tVMPMember.GetMcuId()); // xliang [4/7/2009] FIXME:这里修改tmt是否合理

				tVMPMember.SetMemberTMt( tMt );
			}
			else
			{
				tVMPMember.SetMemberTMt( m_ptMtTable->GetMt(tVMPMember.GetMtId()) );
			}
			tVmpParam.SetVmpMember( byLoop, tVMPMember );
		}
	}

	return TRUE;
}

/*==============================================================================
函数名    :  CheckMpuAudPollConflct
功能      :  会议带音频轮询
算法实现  :  会议带音频轮询时，画面合成器不支持轮询跟随和发言人跟随同时存在
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-8-20					薛亮
==============================================================================*/
void CMcuVcInst::CheckMpuAudPollConflct(TVMPParam& tVmpParam, BOOL32 bChgParam /*= FALSE*/)
{
	
	TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
	if ( tPollInfo.GetMediaMode() == MODE_BOTH 
		&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER)
		&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
		)
	{
		if( !bChgParam )
		{
// 			wErrorCode = ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
// 			return FALSE;
			throw (u16)ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
		}
		else
		{
			u8 byConflictMemType = 0;
			if (m_tLastVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL))
			{
				//此次新增了发言人跟随,把其作为冲突源
				byConflictMemType = VMP_MEMBERTYPE_SPEAKER;
			}
			else if(m_tLastVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER))
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
				TVMPMember tVmpMember;
				for (byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop ++)
				{
					tVmpMember = *tVmpParam.GetVmpMember(byLoop);
					if(tVmpMember.GetMemberType() == byConflictMemType)
					{
						// xliang [4/3/2009] 会控带过来的TVMPParam参数中mode值是空的，这里保护一下
						tVmpParam.SetVMPMode(CONF_VMPMODE_CTRL);
						ClearOneVmpMember(byLoop, tVmpParam, TRUE);
						break;
					}
				}

// 				wErrorCode = ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
// 				return FALSE;
				throw (u16)ERR_AUDIOPOLL_CONFLICTVMPFOLLOW;
			}
		}
	}
}

/*==============================================================================
函数名    :  CheckSameMtInMultChnnl
功能      :  会议中不允许同一个MT占了多个成员通道
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-8-20					薛亮							
==============================================================================*/
void CMcuVcInst::CheckSameMtInMultChnnl(TVMPParam & tVmpParam)
{
	// xliang [3/12/2009] LOCAL_MCUID定义成192是有问题的,当MT.192是MCU的话，就会误判。
	// 一个会议中呼满192个mt，也是很小概率的。 该宏暂不改动，不能改成0，改成193对其他代码有何冲击
	u8 abyMcMtInVMPStatis[MAXNUM_CONF_MT+1][MAXNUM_CONF_MT+1] = {0};
	BOOL32	bRepeatedVmpMem = FALSE;
	u8 bySpeakerChnnl	= 0;
	u8 byPollChnnl		= 0;
	for( u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		TVMPMember tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		
		// xliang [3/24/2009] membertype的过滤，要无视tVMPMember.IsNull()，
		// 否则当发言人跟随但会议中无发言人的情况下，限制失效
		if(tVMPMember.GetMemberType() == VMP_MEMBERTYPE_SPEAKER)
		{
			bySpeakerChnnl ++;
		}
		if(tVMPMember.GetMemberType() == VMP_MEMBERTYPE_POLL)
		{
			byPollChnnl ++;
		}
		
		if(tVMPMember.IsNull()) 
		{
			continue;
		}
		
		if( abyMcMtInVMPStatis[tVMPMember.GetMcuId()][tVMPMember.GetMtId()] == 1)
		{
			bRepeatedVmpMem = TRUE;
			break;
		}
		else
		{
			abyMcMtInVMPStatis[tVMPMember.GetMcuId()][tVMPMember.GetMtId()] ++ ;
		}
	}
	if(bRepeatedVmpMem &&
		!g_cMcuVcApp.IsAllowVmpMemRepeated())
	{
		throw (u16)ERR_MCU_REPEATEDVMPMEMBER;
		return;
	}
	if(bySpeakerChnnl > 1)	//有多个通道被设成发言人跟随
	{
		throw (u16)ERR_MCU_REPEATEDVMPSPEAKER;
		return;
	}
	
	if(byPollChnnl > 1)//有多个通道被设成轮询跟随
	{
		throw (u16)ERR_MCU_REPEATEDVMPPOLL;
		return;
	}

}

/*==============================================================================
函数名    :  CheckAdpChnnlAtStart
功能      :  开始合成时的前适配通道限制过滤
算法实现  :  
参数说明  :  u8 byMaxHdChnnlNum		[i]	最大前适配通道数
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-8-20					薛亮
==============================================================================*/
void CMcuVcInst::CheckAdpChnnlAtStart(TVMPParam &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg)
{
	u8 byIdxInHdChnnl  = 0;
	u8 byIdxOutHdChnnl = 0;
	BOOL32 bOverHdChnnlLmt = FALSE;
	TVmpHdChnnlMemInfo tVmpHdChnnlMemInfo;
	if(m_tConf.HasSpeaker() && tVmpParam.IsMtInMember(GetLocalSpeaker()) //GetSpeaker()会有问题
		&& !tVmpParam.IsVMPBrdst()	//VMP广播会把发言人视频广播冲掉
		)
	{
		tVmpHdChnnlMemInfo.tMtInHdChnnl[byIdxInHdChnnl++] = GetLocalSpeaker();
	}
	for( u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		TVMPMember tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		
		if( tVMPMember.IsNull() )
		{
			continue;	//空成员跳过
		}
		
		if( GetLocalSpeaker().GetMtId() == tVMPMember.GetMtId() )
		{
			//take speaker into account before,so continue
			continue;
		}
		
		if( ( !tVmpParam.IsVMPBrdst()			//VMP广播会把发言人视频广播以及选看全冲掉
			&&  IsSelectedbyOtherMt(tVMPMember.GetMtId()) )						//被选看
			|| ( MT_MANU_KDC != m_ptMtTable->GetManuId(tVMPMember.GetMtId())  
			&& MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tVMPMember.GetMtId()) )	//是否非keda终端
			
			)//注：对于判非keda，若传参为0，也会被判为非keda，所以之前要过滤传参为0的情况
		{
			u8 byChnnlType = LOGCHL_VIDEO;
			TLogicalChannel tLogicChannel;
			BOOL32 bNeedAdjustRes = FALSE;
			TMt		tMt;
			if ( m_ptMtTable->GetMtLogicChnnl( tVMPMember.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) )
			{
				u8 byMtStandardFormat = tLogicChannel.GetVideoFormat();
				u8 byReqRes;
				bNeedAdjustRes = VidResAdjust( tVmpParam.GetVMPStyle(), byLoop, byMtStandardFormat, byReqRes);
				
				if (bNeedAdjustRes) //比较下来是需要调整的MT，才有可能让其进前适配通道
				{
					if( byIdxInHdChnnl < byMaxHdChnnlNum )
					{
						tVmpHdChnnlMemInfo.tMtInHdChnnl[byIdxInHdChnnl++] = m_ptMtTable->GetMt(tVMPMember.GetMtId());
					}
					else
					{
						tVmpHdChnnlMemInfo.tMtOutHdChnnl[byIdxOutHdChnnl++] = m_ptMtTable->GetMt(tVMPMember.GetMtId());
						bOverHdChnnlLmt = TRUE;
						tMt.SetNull();
						tVMPMember.SetMemberTMt( tMt );
						tVmpParam.SetVmpMember( byLoop, tVMPMember );
					}
				}
			}
			
		}
	}
	if(bOverHdChnnlLmt)
	{
		cServMsg.SetMsgBody((u8*)&tVmpHdChnnlMemInfo,sizeof(tVmpHdChnnlMemInfo));
		SendReplyBack(cServMsg, MCU_MCS_VMPOVERHDCHNNLLMT_NTF);
		//SendMsgToAllMcs(MCU_MCS_VMPOVERHDCHNNLLMT_NTF,cServMsg);
	}

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
void CMcuVcInst::CheckAdpChnnlAtChange(TVMPParam &tVmpParam, u8 byMaxHdChnnlNum, CServMsg &cServMsg)
{
	//找当前所添加的合成成员
	for( u8 byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++ )
	{
		TVMPMember tVMPMember = *tVmpParam.GetVmpMember( byLoop );
		if(!tVMPMember.IsNull() && !m_tLastVmpParam.IsMtInMember((TMt)tVMPMember))//这次新增加的终端
		{
			u8 byChnnlType = LOGCHL_VIDEO;
			TLogicalChannel tLogicChannel;
			BOOL32 bNeedAdjustRes = FALSE;
			if ( m_ptMtTable->GetMtLogicChnnl( tVMPMember.GetMtId(), byChnnlType, &tLogicChannel, FALSE ) )
			{
				u8 byMtStandardFormat = tLogicChannel.GetVideoFormat();
				u8 byReqRes;
				bNeedAdjustRes = VidResAdjust( tVmpParam.GetVMPStyle(), byLoop, byMtStandardFormat, byReqRes);
				if(bNeedAdjustRes)
				{
					//特殊身份判断
					BOOL32 bSpeaker = (GetLocalSpeaker() == (TMt)tVMPMember);							//是否是发言人
					BOOL32 bNoneKeda = ( MT_MANU_KDC != m_ptMtTable->GetManuId(tVMPMember.GetMtId())  
						&& MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tVMPMember.GetMtId()) );			//是否非keda
					BOOL32 bSelected = IsSelectedbyOtherMt(tVMPMember.GetMtId());						//是否被选看
					if( (bSpeaker && !tVmpParam.IsVMPBrdst())
						|| bNoneKeda 
						|| bSelected
						)
					{
						if(m_tVmpChnnlInfo.m_byHDChnnlNum == byMaxHdChnnlNum) //前适配通道已满，要抢占提示
						{
							u8 byChnnlIdx = 0;
							TChnnlMemberInfo tChnnlMemInfo;
							u8 bySeizeMtId = tVMPMember.GetMtId();
							TMt tSeizeMt = (TMt)tVMPMember;
							cServMsg.SetMsgBody((u8*)&tSeizeMt,sizeof(TMt));	//需要抢占高清适配通道的终端
							for(; byChnnlIdx < byMaxHdChnnlNum; byChnnlIdx++)	//所有占用HD的通道的MT信息
							{
								m_tVmpChnnlInfo.GetHdChnnlInfo(byChnnlIdx,&tChnnlMemInfo);
								cServMsg.CatMsgBody((u8 *)&tChnnlMemInfo,sizeof(tChnnlMemInfo));
							}
							SendReplyBack(cServMsg, MCU_MCS_VMPPRISEIZE_REQ);
							//SendMsgToAllMcs(MCU_MCS_VMPPRISEIZE_REQ, cServMsg);
							//记录
							m_atVMPTmpMember[byLoop] = tVMPMember;
							
							return;
						}
					}
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
void CMcuVcInst::CheckAdpChnnlLmt(u8 byVmpId, TVMPParam& tVmpParam, CServMsg &cServMsg, BOOL32 bChgParam /* = FALSE */)
{
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

	if(VMP == byVmpSubType || tVmpParam.IsVMPAuto())
	{
		return;
	}

	u8 byMpuBoardVer = tPeriEqpStatus.m_tStatus.tVmp.m_byBoardVer;	
	u8 byMaxHdChnnlNum = 0;
	if(byVmpSubType == MPU_SVMP || byVmpSubType == EVPU_SVMP)	//SVMP
	{
		byMaxHdChnnlNum = (byMpuBoardVer == MPU_BOARD_A128) ? MAXNUM_SVMP_HDCHNNL : MAXNUM_SVMPB_HDCHNNL;
	}
	else //DVMP //目前无dvmp外设
	{
		byMaxHdChnnlNum = MAXNUM_DVMP_HDCHNNL; 
	}
	
	if(!bChgParam)
	{
		CheckAdpChnnlAtStart(tVmpParam, byMaxHdChnnlNum, cServMsg);
	}
	else
	{
		CheckAdpChnnlAtChange(tVmpParam, byMaxHdChnnlNum, cServMsg);
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
BOOL32 CMcuVcInst::CheckMpuMember(TVMPParam& tVmpParam, u16& wErrorCode, BOOL32 bChgParam /*= FALSE */)
{
	//自动画面合成不进行以下校验
	if (tVmpParam.IsVMPAuto())
	{
		return TRUE;
	}

	//VMP不支持主席跟随
	if(tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_CHAIRMAN))
	{
		ConfLog(FALSE,"[VmpCommonReq]MPU not support chairman-membertype!\n");
		wErrorCode = ERR_VMPUNSUPPORTCHAIRMAN;
		return FALSE;
	}
	
	try
	{
		//会议中不允许同一个MT占了多个成员通道
		CheckSameMtInMultChnnl(tVmpParam);

		//会议带音频轮询时，画面合成器不支持轮询跟随和发言人跟随同时存在
		CheckMpuAudPollConflct(tVmpParam, bChgParam);
	
	}
	catch (u16 wErr)
	{
		wErrorCode = wErr;
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
	
	u8 byLoop = 0;
    TVMPParam tVmpParam;
    memset( &tVmpParam, 0, sizeof(TVMPParam) );
	TPeriEqpStatus tPeriEqpStatus;
    TMt tMt;

    //得到空闲的画面合成器 重新调整VMP是否启用 判断策略 [12/27/2006-zbq]
    u8 byIdleVMPNum = 0;
    u8 abyIdleVMPId[MAXNUM_PERIEQP];
    memset( &abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
    g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );

    //zbq[12/27/2007] 非科达终端参与VMP被拒
	// xliang [12/8/2008] FIXME:这里不拒，在之后结合VMP子类型和分辨率做判断拒与否
    //BOOL32 bNonKEDAReject = FALSE;
    //TLogicalChannel tLogicChan;
	u8 byVmpSubType = 0;  //VMP子类型

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
				ProcChangeVmpParamReq(cServMsg); // xliang [1/21/2009] Change VMP Param 处理
				break;
			}
		case MCS_MCU_START_VMPBATCHPOLL_REQ:// xliang [12/18/2008] 开始vmp 批量轮询
			{
				//检查当前会议的画面合成状态
				if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
				{
					cServMsg.SetErrorCode( ERR_MCU_VMPRESTART );
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					
					return;
				}
				
				//FIXME: 暂未对于级联的处理
				tVmpParam = *( TVMPParam* )cServMsg.GetMsgBody();
				m_tVmpBatchPollInfo = *(TVmpBatchPollInfo*)(cServMsg.GetMsgBody()+sizeof(TVMPParam));
				
				if(m_tVmpBatchPollInfo.GetPollNum() == 0 )
				{
					m_tVmpBatchPollInfo.SetLoopNoLimt(TRUE);
				}

				//记录当前是要vmp批量轮询,先置位，之后不行再清
				//这里要记录在m_tConf中，因为之后m_tConf作为实参传入IsMCUSupportVmpStyle()
				m_tConf.m_tStatus.m_tVMPParam.SetVMPBatchPoll( 1 );//该值在之后收到VMP->MCU notif后会被间接清掉,所以补充下行代码
				tVmpParam.SetVMPBatchPoll(1);
				
				if(g_bPrintEqpMsg)
				{
					EqpLog("[VmpCommonReq] start vmp param:\n");
					tVmpParam.Print();
				}
				
				u8 byVmpId;
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
						CallLog("No Mpu for VMP Batch Poll!\n");
						return;
					}
				}
				//设置vmp member
				TVMPMember tVMPMember;
				u8 byMtInConfNum    = (u8)m_tConfAllMtInfo.GetAllMtNum();//与会的终端数
				u8 byCurPolledMtNum = m_tVmpPollParam.GetVmpPolledMtNum();	//当前轮询过的MT数目
				u8 byMtId = m_tVmpPollParam.GetCurrentIdx()+1;	//当前轮询到的终端id
				BOOL32 bPollOver	= FALSE;
				BOOL32 bFirst		= TRUE;
				for(byMtId; byMtId < MAXNUM_CONF_MT; byMtId ++)
				{
					if(!m_tConfAllMtInfo.MtInConf( byMtId ))//非与会终端，跳过
					{
						continue;
					}
					if(byLoop < tVmpParam.GetMaxMemberNum() )
					{
						tMt = m_ptMtTable->GetMt(byMtId);
						tVMPMember.SetMemberTMt( tMt );
						tVMPMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
						tVmpParam.SetVmpMember( byLoop, tVMPMember );
						m_tVmpPollParam.SetCurPollBlokMtId(byLoop,byMtId);//记录版面信息
						byLoop++;
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
						byLoop = 0;//member id重新从0开始
						bFirst = FALSE;//下次就不是第一版了
						break;
					}
				}

				//把以下信息保存到m_tVmpPollParam: 
				m_tVmpPollParam.SetVmpPolledMtNum(byCurPolledMtNum);//当前轮询过的MT数目
				m_tVmpPollParam.SetCurrentIdx(byMtId);//下次要轮到的MT Id,这里不减1
				m_tVmpPollParam.SetVmpChnnlIdx(byLoop);	  //member Id，
				m_tVmpPollParam.SetIsBatchFirst(bFirst);	//下次是否第一版面
				m_tVmpPollParam.SetIsVmpPollOver(bPollOver); //设置vmp一轮批量轮询是否结束

				CallLog("VMP Batch Poll: CurPolledMtNum is %u,MtId is %u,MemberId is %u\n",\
					byCurPolledMtNum, byMtId, byLoop);


				m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
				m_tVmpEqp.SetConfIdx( m_byConfIdx );
				
				//设置画面合成参数
				//这里先将tVMPParam暂存进TPeriEqpStatus中,当收到VMP notify后再存进m_tConf. 
				//注意m_tConf此时已经对m_tVMPParam.m_byVMPBatchPoll赋了值(刚进case处)
				g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
				
				tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::RESERVE;//先占用,超时后未成功再放弃
				tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpParam;
				tPeriEqpStatus.SetConfIdx( m_byConfIdx );
				g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
				//ACK
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

				//开始画面合成 
				AdjustVmpParam(&tVmpParam, TRUE);

				break;
			}
	    case MCS_MCU_STOPVMP_REQ:        //会控结束视频复合请求	
	    case MT_MCU_STOPVMP_REQ:         //主席结束视频复合请求

			// 仍在等待画面合成开始回应,手动触发回应超时请求,释放画面合成资源
			if (!m_tVmpEqp.IsNull() &&
				g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
				tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
				TVmpStatus::WAIT_START == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
			{
				// 直接触发超时处理
				CMessage cMsg;
				ProcVmpRspWaitTimer(&cMsg);	
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
				return;
			}

			//检查当前会议的画面合成状态 
			if( m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_NONE)
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

			//停止时模板失效
			if( m_tConf.GetConfAttrb().IsHasVmpModule() )
			{
				TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
                tConfAttrib.SetHasVmpModule(FALSE);
				m_tConf.SetConfAttrb( tConfAttrib );
			}

			SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, 6*1000);
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tPeriEqpStatus);

			//停止
            g_cEqpSsnApp.SendMsgToPeriEqpSsn( m_tVmpEqp.GetEqpId(),MCU_VMP_STOPVIDMIX_REQ, 
				                              (u8*)cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			
			break;
			
	    case MCS_MCU_GETVMPPARAM_REQ:    //会控查询视频复合参数请求
	    case MT_MCU_GETVMPPARAM_REQ:     //主席查询视频复合参数请求

			tVmpParam = m_tConf.m_tStatus.GetVmpParam();
			cServMsg.SetMsgBody( (u8*)&tVmpParam, sizeof(TVMPParam) );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
            if (g_bPrintEqpMsg)
            {
                OspPrintf(TRUE, FALSE, "%s return param:", OspEventDesc(cServMsg.GetEventId()));
                tVmpParam.Print();
            }
            // guzh [7/25/2007] 向VMP获取参数有意义？
			/*
            g_cEqpSsnApp.SendMsgToPeriEqpSsn( m_tVmpEqp.GetEqpId(), 
				                              MCU_VMP_GETVIDMIXPARAM_REQ, NULL, 0 );
            */
			break;

		case MCS_MCU_STARTVMPBRDST_REQ:         //会议控制台命令MCU开始把画面合成图像广播到终端
		case MT_MCU_STARTVMPBRDST_REQ:		//主席命令MCU开始把画面合成图像广播到终端
			{
				if( m_byVmpOperating == 1 )	// xliang [4/17/2009] 伪原子策略FIXME
				{
					McsLog("VMP is busy!!!\n");
					cServMsg.SetErrorCode(ERR_VMPBUSY);
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					return;
				}
				
				//检查当前会议的画面合成状态
				if (CONF_VMPMODE_NONE == m_tConf.m_tStatus.GetVMPMode())
				{
					cServMsg.SetErrorCode( ERR_MCU_VMPNOTSTART );
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					return;
				}
				
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
				
				m_tConf.m_tStatus.SetVmpBrdst( TRUE );
				
				//开始广播画面合成码流
				ChangeVidBrdSrc(&m_tVmpEqp);

				// xliang [3/27/2009] 冲击VIP成员，VMP前适配成员信息要刷新
				//发言人和被选看的均被冲,选看被冲在ChangeVidBrdSrc()有进一步处理，此处不再处理
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				u8 byMpuBoardVer = tPeriEqpStatus.m_tStatus.tVmp.m_byBoardVer;
				u8 byHdChnnlNum = 0;

				if(byVmpSubType == MPU_SVMP)
				{
					byHdChnnlNum = (byMpuBoardVer == MPU_BOARD_B256) ? MAXNUM_SVMPB_HDCHNNL: MAXNUM_SVMP_HDCHNNL;
				}
				else if(byVmpSubType == EVPU_SVMP)
				{
					byHdChnnlNum = MAXNUM_SVMPB_HDCHNNL;
				}
				else if( byVmpSubType == MPU_DVMP || byVmpSubType == EVPU_DVMP)
				{	
					byHdChnnlNum = MAXNUM_DVMP_HDCHNNL;
				}
				else
				{
					//maintain byHdChnnlNum be 0
				}

				TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
				u8 byVmpStyle = tVmpParam.GetVMPStyle();
				
				//u8 byHdChnnlNum = m_tVmpChnnlInfo.m_byHDChnnlNum;
				TChnnlMemberInfo tChnnlMemInfo;
				u8 byMtId = 0;
				u8 byLoop;
				for(byLoop = 0; byLoop < byHdChnnlNum; byLoop ++)
				{
					m_tVmpChnnlInfo.GetHdChnnlInfo(byLoop, &tChnnlMemInfo);
					if( tChnnlMemInfo.IsAttrSpeaker() 
						//|| tChnnlMemInfo.IsAttrSelected() 
						)
					{
						byMtId = tChnnlMemInfo.GetMtId();
						u8 byPos = tVmpParam.GetChlOfMtInMember(tChnnlMemInfo.GetMt());
						//m_tVmpChnnlInfo.ClearOneChnnl(byMtId);
						//调整Mt分辨率
						ChangeMtVideoFormat(m_ptMtTable->GetMt(byMtId), &tVmpParam);
					}
				}
				
				//通知会控跟主席
				cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam) );
				SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
				if (HasJoinedChairman())
				{
					SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_VMPPARAM_NOTIF, cServMsg );
				}	
				
				//FastUpdate to Vmp
				NotifyFastUpdate(m_tVmpEqp, 0);
				if (MPU_SVMP == byVmpSubType && MPU_BOARD_B256 == byMpuBoardVer)
				{
					NotifyFastUpdate(m_tVmpEqp, 1);
					NotifyFastUpdate(m_tVmpEqp, 2);
					NotifyFastUpdate(m_tVmpEqp, 3);
				}

				break;
			}

		case MCS_MCU_STOPVMPBRDST_REQ:          //会议控制台命令MCU停止把画面合成图像广播到终端
		case MT_MCU_STOPVMPBRDST_REQ:		//主席命令MCU停止把画面合成图像广播到终端			
			
			if( m_byVmpOperating == 1 )	// xliang [4/17/2009] 伪原子策略FIXME
			{
				McsLog("VMP is busy!!!\n");
				cServMsg.SetErrorCode(ERR_VMPBUSY);
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
			//检查当前会议的画面合成状态
			if( m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_NONE )
			{
				cServMsg.SetErrorCode( ERR_MCU_VMPNOTSTART );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
			}
			
			//检查画面合成的广播状态
			if( !m_tConf.m_tStatus.IsBrdstVMP() )
			{
				cServMsg.SetErrorCode( ERR_MCU_VMPNOTBRDST );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
			}
/*
            // xsl [11/1/2006] 8000b mcu h264会议四画面及以上必须广播(自适应策略)
#ifdef _MINIMCU_
            if (m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264)            
            {
                u8 byVmpType = m_tConf.m_tStatus.GetVmpStyle();
                if (VMP_STYLE_ONE != byVmpType && VMP_STYLE_VTWO != byVmpType && VMP_STYLE_THREE != byVmpType)
                {
                    cServMsg.SetErrorCode( ERR_MCU_OPERATION_NOPERMIT );
				    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
            }
#endif
*/
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

			//停止广播画面合成码流
            m_tConf.m_tStatus.SetVmpBrdst( FALSE );

			McsLog("Start Stop VMP Broadcast now!!!\n");

			if( HasJoinedSpeaker() )
            {
                TMt tMt = GetLocalSpeaker();
                ChangeVidBrdSrc( &tMt );
            }
            else
            {
                ChangeVidBrdSrc( NULL );
			}

			// xliang [3/31/2009] FIXME: 发言人会恢复广播，若当前还有被选看的，则选看被发言人冲掉。
			// 因为时序关系，下段代码放在ChangeVidBrdSrc()后
			TPeriEqpStatus tPeriEqpStatus; 
			g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
			u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
			
			if(byVmpSubType != VMP)
			{
				TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
				u8 byVmpStyle = tVmpParam.GetVMPStyle();
				if( HasJoinedSpeaker() )
				{
					//被选看要被冲 --在之前ChangeVidBrdSrc()中会调StopSelectSrc(),该函数会将被选看的MT退出前适配通道
					/*
					u8 byHdChnnlNum = m_tVmpChnnlInfo.m_byHDChnnlNum;
					TChnnlMemberInfo tChnnlMemInfo;
					
					u8 byLoop;
					for(byLoop = 0; byLoop < byHdChnnlNum; byLoop ++)
					{
						m_tVmpChnnlInfo.GetHdChnnlInfo(byLoop,&tChnnlMemInfo);
						if( tChnnlMemInfo.IsAttrSelected() )
						{
							byMtId = tChnnlMemInfo.GetMtId();
							u8 byPos = tVmpParam.GetChlOfMtInMember(tChnnlMemInfo.GetMt());
							m_tVmpChnnlInfo.ClearOneChnnl(byMtId);
							//前适配通道是空出来了，但是此处该MT被选看的状态还未清，所以马上调分辨率又会占前适配通道
							//调整Mt分辨率
							ChangeMtVideoFormat(byMtId,byVmpSubType,byVmpStyle,byPos);
						}
					}*/
					if( tVmpParam.IsMtInMember(GetLocalSpeaker()/*m_tConf.GetSpeaker()*/) )
					{
						//发言人尝试进前适配通道
						u8 byMtId = m_tConf.GetSpeaker().GetMtId();
						u8 byPos = tVmpParam.GetChlOfMtInMember(m_tConf.GetSpeaker());
						ChangeMtVideoFormat(m_ptMtTable->GetMt(byMtId), &tVmpParam);
					}
				}
			}
			
			
			
            
            
             
            //通知会控跟主席
            cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam) );
            SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
            if( HasJoinedChairman() )
            {
                SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_VMPPARAM_NOTIF, cServMsg );
            }			
			break;
		}

		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
        ConfLog(FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
    TVMPParam tVmpTwParam;
    TPeriEqpStatus tPeriEqpStatus;
    TMt tMt;

    //zbq[01/08/2007] 非科达终端拒绝加入VMPTW
    BOOL32 bNonKEDAReject = FALSE;
    TLogicalChannel tLogicChan;

    //得到空闲的画面合成器 重新调整MPW是否启用 判断策略 [12/27/2006-zbq]
    u8 byIdleMPWNum = 0;
    u8 abyIdleMPWId[MAXNUM_PERIEQP];
    memset( &abyIdleMPWId, 0, sizeof(abyIdleMPWId) );
    g_cMcuVcApp.GetIdleVmpTw( abyIdleMPWId, byIdleMPWNum, sizeof(abyIdleMPWId) );

    switch(cServMsg.GetEventId())
    {
    case MCS_MCU_STARTVMPTW_REQ:       //会控开始视频复合请求

        tVmpTwParam = *(TVMPParam *)cServMsg.GetMsgBody();
        if(g_bPrintEqpMsg)
        {
            EqpLog("[VmpTwCommonReq] change vmp param:\n");
            tVmpTwParam.Print();
        }

        //对于会控控制模式下，若跟随方式无相应终端，则终端信息清空       
        memset(&tMt, 0, sizeof(TMt));
        for (byLoop = 0; byLoop < tVmpTwParam.GetMaxMemberNum(); byLoop++)
        {
            TVMPMember tVMPMember = *tVmpTwParam.GetVmpMember(byLoop);
            TMt tTmpMt = GetLocalMtFromOtherMcuMt(tVMPMember);
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
                          MT_MANU_KDC != m_ptMtTable->GetManuId(tVMPMember.GetMtId()) &&
                          MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tVMPMember.GetMtId()) )
                {
                    tMt.SetNull();
                    tVMPMember.SetMemberTMt( tMt );
                    tVmpTwParam.SetVmpMember( byLoop, tVMPMember );
                    
                    bNonKEDAReject = TRUE;
                }                
                else if ( !m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMtId()) )
                {
                    tMt.SetNull();
                    tVMPMember.SetMemberTMt(tMt);
                    tVmpTwParam.SetVmpMember(byLoop, tVMPMember);
                }
                else
                {
                    if (tVMPMember.GetMcuId() != LOCAL_MCUID)
                    {
                        //有风险
                        OnMMcuSetIn(tVMPMember, 0, SWITCH_MODE_SELECT);
                        tMt = m_ptMtTable->GetMt(tVMPMember.GetMcuId());
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
            if (IsOverSatCastChnnlNum(tVmpTwParam))
            {
                ConfLog(FALSE, "[VmpTwCommonReq-start] over max upload mt num. nack!\n");
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
        tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpTwParam;
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
        tVmpTwParam = *(TVMPParam *)cServMsg.GetMsgBody();
        if(g_bPrintEqpMsg)
        {
            EqpLog("[VmpTwCommonReq] change vmp param:\n");
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
            u8 byVmpTwId = 0;
            if ( IsMCUSupportVmpStyle(tVmpTwParam.GetVMPStyle(), byVmpTwId, EQP_TYPE_VMPTW, wError) )
            {
                NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_OTHERVMP_SUPPORT );
                ConfLog( FALSE, "[VmpTwCommonReq] curr VMPTW.%d can't support the style needed, other VMPTW.%d could\n",
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
            if (0 != tVMPMember.GetMtId() && LOCAL_MCUID != tVMPMember.GetMcuId())
            {
                //有风险
                OnMMcuSetIn(tVMPMember, 0, SWITCH_MODE_SELECT);
                TMt tMt;
                tMt = m_ptMtTable->GetMt(tVMPMember.GetMcuId());
                tVMPMember.SetMemberTMt(tMt);
                tVmpTwParam.SetVmpMember(byLoop, tVMPMember);
            }
        }

        // xsl [7/20/2006]卫星分散会议需要检查回传通道数
        if (m_tConf.GetConfAttrb().IsSatDCastMode()) 
        {            
            if (IsOverSatCastChnnlNum(tVmpTwParam))
            {
                ConfLog(FALSE, "[VmpTwCommonReq-change] over max upload mt num. nack!\n");
                cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
        }

        //ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

        g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId() , &tPeriEqpStatus);
        tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpTwParam;
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
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMcu tMcu =  *(TMcu *)cServMsg.GetMsgBody(); 

	MMcuLog( "[ProcMcsMcuMcuMediaSrcReq] McuMcuId.%d - McuMtId.%d\n", 
		     tMcu.GetMcuId(), tMcu.GetMtId() );

	cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu) );
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->m_atConfOtherMcInfo;
	for(s32 nLoop=0; nLoop<MAXNUM_SUB_MCU; nLoop++)
	{
		u8 byMcuId = ptMcInfo[nLoop].m_byMcuId;
		if(byMcuId == 0)
		{
			continue;
		}

		TMt tMt = GetMcuMediaSrc(byMcuId);

		//如果查询的是上级mcu并且媒体源为空，则设置媒体源即是该mcu
		if( (tMt.IsNull() || 0 == tMt.GetMtId()) && 
			!m_tCascadeMMCU.IsNull() && 
			byMcuId == m_tCascadeMMCU.GetMtId() )
		{
			tMt.SetMcuId(LOCAL_MCUID);
			tMt.SetMtId(byMcuId);
		}

		McsLog( "[ProcTimerMcuSrcCheck] McuId.%d - MtMcuId.%d MtId.%d MtType.%d MtConfIdx.%d\n", 
				 byMcuId, tMt.GetMcuId(), tMt.GetMtId(), tMt.GetMtType(), tMt.GetConfIdx() );

		ptMcInfo[nLoop].m_tSrcMt = tMt;
		TMcu tMcu;
		tMcu.SetMcuId(byMcuId);
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

	if( NULL == m_ptConfOtherMcTable )
	{
		cServMsg.SetErrorCode( ERR_LOCKSMCU_NOEXIT );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	if( ( ptMcu->IsLocal() ) || 
		( !m_tCascadeMMCU.IsNull() && ptMcu->GetMcuId() == m_tCascadeMMCU.GetMtId() ) )
	{
		cServMsg.SetErrorCode( ERR_LOCKSMCU_NOEXIT );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	CServMsg cMsg;
	cMsg.SetServMsg(cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_LOCK_REQ);

	u8   byMcuNum = 0;
	TConfMcInfo *ptMcInfo = NULL;

	if( 0 != ptMcu->GetMcuId() )
	{
		ptMcInfo = m_ptConfOtherMcTable->GetMcInfo( ptMcu->GetMcuId() );
		if( NULL == ptMcInfo )
		{
			cServMsg.SetErrorCode( ERR_LOCKSMCU_NOEXIT );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
		SendMsgToMt(ptMcu->GetMcuId(), MCU_MCU_LOCK_REQ, cMsg);
	}
	else
	{
		for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
		{
			ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
			if( ( 0 == ptMcInfo->m_byMcuId ) || 
				( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) )
			{
				continue;
			}
			SendMsgToMt(ptMcInfo->m_byMcuId, MCU_MCU_LOCK_REQ, cMsg);

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
        ConfLog( FALSE, "[ProcMcsMcuVACReq] distributed conf not supported vac\n");
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
    u8 byGrpId = 0;

    //来自非主席终端，nack
    if (cServMsg.GetSrcMtId() != 0 &&
        cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId())
    {
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
			ConfLog(FALSE, "[MixerVACReq]ERROR: already in VAC mode!\n");
            cServMsg.SetErrorCode( ERR_MCU_CONFINVACMODE );
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
        
        //没有空闲的混音器
        if(m_tConf.m_tStatus.IsNoMixing())
        {
            byEqpId = g_cMcuVcApp.GetIdleMixGroup( byEqpId, byGrpId );
            if( byEqpId == 0 )
            {
				ConfLog(FALSE, "[MixerVACReq] ERROR: no idle MIXER!\n");
                cServMsg.SetErrorCode(ERR_MCU_NOIDLEMIXER_INCONF);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
                return;
            }
        }        
        
        //如果已开始轮询，则不能开始语音激励
        if (m_tConf.m_tStatus.GetPollMode() != CONF_POLLMODE_NONE) 
        {
            cServMsg.SetErrorCode(ERR_MCU_CONFSTARTPOLLVACMODE);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        // xsl [7/24/2006] 定制混音不能开启语音激励
        if (m_tConf.m_tStatus.IsSpecMixing())
        {
            ConfLog(FALSE, "[MixerVACReq] MixSpec can't start vac. nack\n");
            cServMsg.SetErrorCode(ERR_MCU_VACMUTEXSPECMIX);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        // guzh [1/30/2007] 8000B 能力限制        
        if ( ! CMcuPfmLmt::IsVacOprSupported( m_tConf, m_tConfAllMtInfo.GetLocalJoinedMtNum(), wError ) )
        {
			ConfLog(FALSE, "[MixerVACReq]ERROR: 8000b limit!\n");
            cServMsg.SetErrorCode(wError);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        //ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

        if (m_tConf.m_tStatus.IsMixing())
        {
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
            if (HasJoinedChairman())
            {
                SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STARTVAC_NOTIF, cServMsg);
            }
            //zbq [11/26/2007] 单独上报
            MixerStatusChange();
        }
        else
        {
            StartMixing( mcuVacMix );
        }
        break;

    case MCS_MCU_STOPVAC_REQ:     //会议控制台请求MCU停止语音激励控制发言	
    case MT_MCU_STOPVAC_REQ:      //终端请求MCU停止语音激励控制发言
        //尚未处于处于语音激励控制发言状态, NACK
        if (!m_tConf.m_tStatus.IsVACing())
        {
            cServMsg.SetErrorCode(ERR_MCU_CONFNOTINVACMODE);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

        //ACK
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
        
        if (m_tConf.m_tStatus.IsNoMixing())
        {
            ConfLog(FALSE,"[MixerVACReq] begin to stopMixing!\n");
			StopMixing(); //停止语音激励，等ACK回来设置状态
        }
        else
        {
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
            ConfLog(FALSE, "No poll while Vac!\n");
            break;
        }       
        
        //得到会议轮询参数
        tPollInfo = *(TPollInfo *)cServMsg.GetMsgBody();
        u8 byPollMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TPollInfo)) / sizeof(TMtPollParam);
        TMtPollParam *ptParam = (TMtPollParam*)(cServMsg.GetMsgBody() + sizeof(TPollInfo));
        m_tConfPollParam.InitPollParam( byPollMtNum, ptParam );

        // xsl [8/23/2006] 判断卫星会议是否停止轮询
        if (m_tConf.GetConfAttrb().IsSatDCastMode())
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
                        ConfLog(FALSE, "[ProcMcsMcuPollMsg-start] over max upload mt num. nack!\n");  
                        NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_DCAST_OVERCHNNLNUM);

                        memset(&m_tConfPollParam, 0, sizeof(m_tConfPollParam));
                        return;
                    }
                }
            }            
        }     
        
        // guzh [6/22/2007] 标记为开始轮询，设置定时器
        tPollInfo.SetPollState(POLL_STATE_NORMAL);
        m_tConfPollParam.SpecPollPos(POLLING_POS_START);        
        m_tConf.m_tStatus.SetPollInfo(tPollInfo);

        if (MODE_VIDEO == tPollInfo.GetMediaMode())
        {
            m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_VIDEO);
        }
        else
        {
            m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_SPEAKER);
        }
        ConfModeChange();

        SetTimer(MCUVC_POLLING_CHANGE_TIMER, 10);

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

        cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
        cServMsg.CatMsgBody((u8 *)m_tConfPollParam.GetPollMtByIdx(0),
                            m_tConfPollParam.GetPolledMtNum()*sizeof(TMtPollParam ));
        SendReplyBack(cServMsg, pcMsg->event + 1);			
        break;

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
            u8 byIdx;
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
				ConfLog( FALSE, "[ProcMcsMcuPollMsg] <MCS_MCU_SPECPOLLPOS_REQ> Conf not in polling!\n");
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
                TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
                TMtPollParam tCurMtParam = *(m_tConfPollParam.GetPollMtByIdx(byMtIdx));
                tPollInfo.SetMtPollParam( tCurMtParam );
                m_tConf.m_tStatus.SetPollInfo(tPollInfo);
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
void CMcuVcInst::ProcStopConfPoll( void )
{
    u8 byPrvPollMode = m_tConf.m_tStatus.GetPollMode();
    u8 byPollMeida = m_tConf.m_tStatus.GetPollMedia();
    m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_NONE);
    m_tConf.m_tStatus.SetPollState(POLL_STATE_NONE);

    CServMsg cServMsg;
    cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo));
    SendMsgToAllMcs(MCU_MCS_POLLSTATE_NOTIF, cServMsg);
    
    KillTimer(MCUVC_POLLING_CHANGE_TIMER);
    
    //取消轮询，则取消轮询交换及相应复合状态
    //CancelOneVmpFollowSwitch( VMP_MEMBERTYPE_POLL, TRUE );
    if (CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode())
    {
        ChangeVmpChannelParam(NULL, VMP_MEMBERTYPE_POLL);
    }
    
    if (CONF_VMPTWMODE_NONE != m_tConf.m_tStatus.GetVmpTwMode())
    {
        ChangeVmpTwChannelParam(NULL, VMPTW_MEMBERTYPE_POLL);
    }
    
    if ( !m_tConf.m_tStatus.IsBrdstVMP() )		
    {
        if(HasJoinedSpeaker())
        {
            TMt tMt = GetLocalSpeaker();
            ChangeVidBrdSrc(&tMt);
        }
        else
        {
            ChangeVidBrdSrc(NULL);
            
            //zbq[08/29/2009] 纯视频轮询下，强制补拆声音同步的过bas交换 
            if (CONF_POLLMODE_VIDEO == byPrvPollMode)
            {
                TBasChn atBasChn[MAXNUM_CONF_MVCHN];
                u8 byNum = 0;
                m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_MVBRD);
                
                u8 byIdx = 0;
                for(; byIdx < byNum; byIdx ++)
                {
                    StopSwitchToPeriEqp(atBasChn[byIdx].GetEqpId(), atBasChn[byIdx].GetChnId(), TRUE, MODE_AUDIO);
                }
            }
        }
    }
        
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
        if (EQP_TYPE_HDU == g_cMcuVcApp.GetEqpType(byEqpId))
        {
            for (byChnlIdx = 0; byChnlIdx < MAXNUM_HDU_CHANNEL; byChnlIdx++)
            {
                if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tHduStatus) &&
                    TW_MEMBERTYPE_POLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType &&
                    m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx())
                {
                    ChangeHduSwitch(NULL, byEqpId, byChnlIdx, TW_MEMBERTYPE_POLL, TW_STATE_STOP, FALSE);
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
        if (CONF_POLLMODE_NONE == m_tConf.m_tStatus.GetPollMode()) 
        {
            break;
        }

		if ( 0 == m_tConfPollParam.GetPolledMtNum() )
		{
            ProcStopConfPoll();
			break;
		}		
        
		TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
		
		TMt tLastPolledMt = tPollInfo.GetMtPollParam().GetTMt(); // xliang [3/31/2009] 记录上次轮到的MT
        //记录轮询是否有次数限制, zgc, 2007-06-20
		BOOL32 bIsPollNumNoLimit = ( 0 == tPollInfo.GetPollNum() ) ? TRUE : FALSE; 
		u8 byPollIdx = m_tConfPollParam.GetCurrentIdx();
		ptCurPollMt = GetNextMtPolled(byPollIdx, tPollInfo);		
		m_tConf.m_tStatus.SetPollInfo(tPollInfo);
		m_tConfPollParam.SetCurrentIdx(byPollIdx);
	
		// xsl [8/23/2006] 判断卫星会议是否停止轮询
		if (m_tConf.GetConfAttrb().IsSatDCastMode())
		{
			BOOL32 bPollWitchAudio = (CONF_POLLMODE_SPEAKER == m_tConf.m_tStatus.GetPollMode());
			TMt tLastPollMt = (TMt)m_tConf.m_tStatus.GetMtPollParam();
			if (IsMtSendingVidToOthers(tLastPollMt, bPollWitchAudio, FALSE, 0) && //上一个终端还在向其他实体发送码流
				m_ptMtTable->GetMtSndBitrate(ptCurPollMt->GetMtId()) == 0 && //下一个终端没有在发送码流
				IsOverSatCastChnnlNum(0))
			{
                ProcStopConfPoll();
			    break;
			}
		}   
        
		if (NULL != ptCurPollMt)
		{               
			//通知所有会控
			TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);           
           
			m_tConf.m_tStatus.SetMtPollParam( *ptCurPollMt );
			cServMsg.SetMsgBody( (u8*)&tPollInfo, sizeof(TPollInfo) );
			SendMsgToAllMcs( MCU_MCS_POLLSTATE_NOTIF, cServMsg );
           
			//变更画面复合的相应轮询更随的交换
			// xliang [3/31/2009] 重整逻辑
			if (CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode())
			{
				TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
				
				// xliang [4/21/2009] 主席在选看VMP失效
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
				}

				// xliang [4/2/2009] 带音频的会议轮询,VMP轮询跟随，VMP发言人跟随3者不能共存
				// 发言人跟随通道取消，上报MCS错误提示
				if( tPollInfo.GetMediaMode() == MODE_BOTH
					&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) //带音频的会议轮询=轮流变发言人
					&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)  )	
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
							ClearOneVmpMember(byLoop, tVmpParam);
							break;
						}
					}
				}
				if( tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL) 
					//|| (tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) //带音频的会议轮询进发言人跟随通道
					//&& tPollInfo.GetMediaMode() == MODE_BOTH ) 
					)
				{
					// xliang [3/19/2009] 对于MPU，若轮询到的MT已经在其他通道中且该通道是发言人跟随或者该Mt是发言人
					// 则不进VMP中的轮询跟随通道  ----废除 xliang [4/2/2009] 
					// xliang [4/2/2009] 对于新VMP，若轮询到的MT已经在其他通道中均不进轮询跟随通道
					u8 byMtVmpChnl = tVmpParam.GetChlOfMtInMember(tLocalMt);
					TPeriEqpStatus tPeriEqpStatus; 
					g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
					if( ( byVmpSubType == MPU_SVMP || byVmpSubType == MPU_DVMP 
						|| byVmpSubType == EVPU_SVMP || byVmpSubType == EVPU_DVMP)
						&& tVmpParam.IsMtInMember(tLocalMt)
						//&& ( GetLocalSpeaker() == tLocalMt 
						//|| tVmpParam.GetVmpMember(byMtVmpChnl)->GetMemberType() == VMP_MEMBERTYPE_SPEAKER )
						)
					{
						McsLog("[ProcPollingChangeTimerMsg] Mt.%u has already in certain VMP channel!\n",tLocalMt.GetMtId());
					}
					else
					{
						u8 byVmpMemType = VMP_MEMBERTYPE_POLL;
						/*if( tPollInfo.GetMediaMode() == MODE_BOTH )	// xliang [3/31/2009] 带音频的会议轮询--->发言人跟随通道
						{
							byVmpMemType = VMP_MEMBERTYPE_SPEAKER;
						}*/
						ChangeVmpChannelParam(&tLocalMt, byVmpMemType,&tLastPolledMt);
					}
				}
			}

			if (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE)
			{
				ChangeVmpTwChannelParam(&tLocalMt, VMPTW_MEMBERTYPE_POLL);
			}
           
			if (MODE_VIDEO == m_tConf.m_tStatus.GetPollMedia())
			{
				if (!m_tConf.m_tStatus.IsBrdstVMP())
				{
					ChangeVidBrdSrc((TMt *)&tLocalMt);
				}
			}
			else
			{
				ChangeSpeaker(ptCurPollMt, TRUE);
			}
            // zbq [09/06/2007] 级联轮询不带音频也SetIn,是否轮询到在GetParam里过滤
            // guzh [6/14/2007] 下级终端要SetIn
            if ( !ptCurPollMt->IsLocal() )
            {
                OnMMcuSetIn(*ptCurPollMt, 0, SWITCH_MODE_BROADCAST, TRUE);
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
						if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus) &&
							TW_MEMBERTYPE_POLL == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType &&
							m_byConfIdx == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx())
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
				if (EQP_TYPE_HDU == g_cMcuVcApp.GetEqpType(byHduEqpId))
				{
					for (byHduChnlIdx = 0; byHduChnlIdx < MAXNUM_HDU_CHANNEL; byHduChnlIdx++)
					{
						if (g_cMcuVcApp.GetPeriEqpStatus(byHduEqpId, &tHduStatus) &&
							TW_MEMBERTYPE_POLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].byMemberType &&
							m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetConfIdx())
						{
							ChangeHduSwitch(ptCurPollMt, byHduEqpId, byHduChnlIdx, TW_MEMBERTYPE_POLL, TW_STATE_CHANGE, m_tConf.m_tStatus.GetPollMedia() );
						}
						else
						{
							McsLog("[ProcPollingChangeTimerMsg] HduEqp%d is not exist or not polling \n", byHduEqpId);
						}
					}
				}
			}

			SetTimer(MCUVC_POLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());
			// zbq [03/09/2007] 通知下一个即将被轮询到的终端
			NoifyMtToBePolledNext();
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
    函数名      ：NoifyMtToBePolledNext
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
void CMcuVcInst::NoifyMtToBePolledNext( void )
{
    TMtPollParam *ptNextPollMt;
    u8 byPollIdx = m_tConfPollParam.GetCurrentIdx();
    TPollInfo tPollInfo;
	ptNextPollMt = GetNextMtPolled(byPollIdx, tPollInfo);

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
=============================================================================*/
void CMcuVcInst::ProcMcsMcuHduBatchPollMsg(const CMessage *pcMsg)
{
	if ( NULL == pcMsg )
	{
	 	OspPrintf( TRUE, FALSE, "[ProcMcsMcuHduBatchPollMsg] pcMsg is null!\n" );
	    return;
	}
    
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    u8  byIndex;

	switch ( pcMsg->event )
	{
	// 开始批量轮询
	case MCS_MCU_STARTHDUBATCHPOLL_REQ:
	{
		TPeriEqpStatus tHduStatus;
        THduPollSchemeInfo tHduPollSchemeInfo;
        THduChnlCfgInfo tHduChnlCfgInfo[MAXNUM_HDUCFG_CHNLNUM];
		THduChnlPollInfo *ptHduChnlPollInfo = (THduChnlPollInfo*)m_tHduBatchPollInfo.m_tChnlBatchPollInfo;

		if( POLL_STATE_NONE != m_tHduBatchPollInfo.GetStatus() )
		{
			ConfLog(FALSE, "[ProcMcsMcuHduBatchPollMsg] hdu is already in batch polling\n");
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
		THduChnlInfoInUse tHduChnlInfoInUseTable[MAXNUM_HDUCFG_CHNLNUM];
		for ( byIndex=0; byIndex < MAXNUM_HDUCFG_CHNLNUM; byIndex++ )
		{
			if ( tHduChnlCfgInfo[byIndex].GetEqpId() >= HDUID_MIN
			    && tHduChnlCfgInfo[byIndex].GetEqpId() <= HDUID_MAX
				)
			{
				g_cMcuVcApp.GetPeriEqpStatus( tHduChnlCfgInfo[byIndex].GetEqpId(), &tHduStatus );
                // 有通道已被占用，需释放后再开启批量轮询
				if ( tHduStatus.m_byOnline == 1
					&& 0 != tHduStatus.m_tStatus.tHdu.atVideoMt[tHduChnlCfgInfo[byIndex].GetChnlIdx()].GetMcuId() )
                {
					bIsChnlInUse = TRUE;
                    tHduChnlInfoInUseTable[byChnlInUseNum].SetChlIdx( tHduChnlCfgInfo[byIndex].GetChnlIdx() );
					tHduChnlInfoInUseTable[byChnlInUseNum].SetEqpId( tHduChnlCfgInfo[byIndex].GetEqpId() );
					tHduChnlInfoInUseTable[byChnlInUseNum].SetConfIdx( 
						tHduStatus.m_tStatus.tHdu.atVideoMt[tHduChnlCfgInfo[byIndex].GetChnlIdx()].GetConfIdx() 
						);

				}
				// 保存预案中可以参与轮询的通道
                if ( !bIsChnlInUse )  
                {
					ptHduChnlPollInfo[byHduChnlInPollNum].SetPosition( byIndex );
					ptHduChnlPollInfo[byHduChnlInPollNum].SetChnlIdx( tHduChnlCfgInfo[byIndex].GetChnlIdx() );
					ptHduChnlPollInfo[byHduChnlInPollNum].SetEqpId( tHduChnlCfgInfo[byIndex].GetEqpId());
				    tHduStatus.m_tStatus.tHdu.atHduChnStatus[tHduChnlCfgInfo[byIndex].GetChnlIdx()].SetSchemeIdx(bySchemeIdx );
					g_cMcuVcApp.SetPeriEqpStatus( tHduChnlCfgInfo[byIndex].GetEqpId(), &tHduStatus );
					byHduChnlInPollNum ++;
                }
			}
			else
			{
				// do nothing!
			}
		}
        
		// 预案中有正在被使用的通道，返回被占用的通道信息列表
        if ( bIsChnlInUse )
        {
			ConfLog( FALSE, "[ProcMcsMcuHduBatchPollMsg] current hdu scheme's chnls is in use! please release them! \n" );
			cServMsg.SetErrorCode( ERR_MCU_HDUBATCHPOLL_CHNLINUSE );
			cServMsg.SetMsgBody( (u8*)tHduChnlInfoInUseTable, sizeof(tHduChnlInfoInUseTable) );
			SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 2, cServMsg );
			return;
        }
        u32 dwCycles = tHduPollSchemeInfo.GetCycles();
		m_tHduBatchPollInfo.SetChnlPollNum( byHduChnlInPollNum );
		m_tConf.m_tStatus.GetHduPollInfo()->SetPollNum( dwCycles );
        m_tConf.m_tStatus.GetHduPollInfo()->SetKeepTime( tHduPollSchemeInfo.GetKeepTime() );

		// 开始第一版的轮询 
		if ( !HduBatchPollOfOneCycle( TRUE ) )
		{
            ConfLog( FALSE, "[ProcMcsMcuHduBatchPollMsg] have no Mt in current conf!\n" );
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
	    SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
		
		SetTimer(MCUVC_HDUBATCHPOLLI_CHANGE_TIMER, 1000 * tHduPollSchemeInfo.GetKeepTime() );

	}
		break;
    
	// 停止hdu批量轮询
	case MCS_MCU_STOPHDUBATCHPOLL_REQ:
	{
        StopHduBatchPollSwitch();

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
	    SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

	}
	    break;
	
	default:
		OspPrintf( TRUE, FALSE, "[ProcMcsMcuHduBatchPollMsg] message type(%u) is wrong!\n" , pcMsg->event );
    
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

    switch(pcMsg->event)
    {
    case MCS_MCU_STARTHDUPOLL_CMD:
        {
            TPeriEqpStatus tHduInfo;
            THduPollInfo tPollInfo = *(THduPollInfo *)(cServMsg.GetMsgBody());
            
            if(tPollInfo.GetTWChnnl() >= MAXNUM_HDU_CHANNEL)
            {
                ConfLog(FALSE, "[ProcMcsMcuHduPollMsg] hdu chnnl id is invalid :%d\n", tPollInfo.GetTWChnnl());
                break;
            }
            
            //通道上是否已经有终端            
            g_cMcuVcApp.GetPeriEqpStatus(tPollInfo.GetTvWall().GetEqpId(), &tHduInfo);
			tHduInfo.m_tStatus.tHdu.atHduChnStatus[tPollInfo.GetTWChnnl()].SetSchemeIdx( tPollInfo.GetSchemeIdx() );
            g_cMcuVcApp.SetPeriEqpStatus( tPollInfo.GetTvWall().GetEqpId(), &tHduInfo );
			THduMember tHduMem = tHduInfo.m_tStatus.tHdu.atVideoMt[tPollInfo.GetTWChnnl()];
            if(0 != tHduMem.GetConfIdx() && tHduMem.GetConfIdx() != m_byConfIdx)          //其他会议的没有权限操作, 本会议的替换
            {
                ConfLog(FALSE, "[ProcMcsMcuHduPollMsg] Confidx %d is already in polling, can't replace it\n", tHduMem.GetConfIdx());
                break;
            }
            
            if(POLL_STATE_NONE != m_tConf.m_tStatus.GethduPollState())
            {
                ConfLog(FALSE, "[ProcMcsMcuHduPollMsg] hdu chnnl id :%d is already in polling\n", tPollInfo.GetTWChnnl());
                break;
            }

            //得到轮询mt参数
			u8 byPollMtNum = (cServMsg.GetMsgBodyLen() - sizeof(THduPollInfo)) / sizeof(TMtPollParam);
            TMtPollParam *atMtPollParam = (TMtPollParam *)( cServMsg.GetMsgBody() + sizeof(THduPollInfo) );
		    m_tHduPollParam.InitPollParam( byPollMtNum, atMtPollParam );
            m_tHduPollParam.SetTvWallEqp(tPollInfo.GetTvWall());
            m_tHduPollParam.SetTvWallChnnl(tPollInfo.GetTWChnnl());
			m_tHduPollParam.SetMode( tPollInfo.GetMediaMode() );
         
            // xsl [8/23/2006] 判断卫星会议是否停止轮询
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {            
                //若回传个数已满其轮询终端中有不发码流的终端则拒绝轮询
                if (IsOverSatCastChnnlNum(0))
                {
                    for(u8 byIdx = 0; byIdx < m_tHduPollParam.GetPolledMtNum(); byIdx++)
                    {
                        if (m_ptMtTable->GetMtSndBitrate(m_tHduPollParam.GetPollMtByIdx(byIdx)->GetMtId()) == 0)
                        {
                            ConfLog(FALSE, "[ProcMcsMcuHduPollMsg-start] over max upload mt num. nack!\n");  
                            NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_DCAST_OVERCHNNLNUM);
                            memset(&m_tHduPollParam, 0, sizeof(m_tHduPollParam));
                            return;
                        }
                    }
                }            
            }     

            m_tConf.m_tStatus.SetHduPollInfo(tPollInfo);
            m_tConf.m_tStatus.SetHduPollState(POLL_STATE_NORMAL);                        
            
			u8 byCurPollPos = m_tHduPollParam.GetCurrentIdx();
            TMtPollParam *ptCurPollMt = GetMtHduPollParam( byCurPollPos );
            m_tHduPollParam.SetCurrentIdx( byCurPollPos );
			m_tHduPollParam.SetIsStartAsPause( 0 );
			
            if (NULL != ptCurPollMt)
            {
                TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);   
                        
                //通知所有会控
                m_tConf.m_tStatus.SetHduMtPollParam(*ptCurPollMt);
                cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetHduPollInfo(), sizeof(THduPollInfo));
                SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);

                ChangeHduSwitch(&tLocalMt, tPollInfo.GetTvWall().GetEqpId(), tPollInfo.GetTWChnnl(), 
                                    TW_MEMBERTYPE_TWPOLL, TW_STATE_START, tPollInfo.GetMediaMode() );

                ConfStatusChange();
                
                //设置轮询时钟
                SetTimer(MCUVC_HDUPOLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());
            }
            else
            {
                //设置轮询时钟
                SetTimer(MCUVC_HDUPOLLING_CHANGE_TIMER, 1000 * 5);
            }            
        }
        break;

    case MCS_MCU_PAUSEHDUPOLL_CMD:
        {
            m_tConf.m_tStatus.SetHduPollState(POLL_STATE_PAUSE);
            cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetHduPollInfo(), sizeof(THduPollInfo));
            SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
                        
            KillTimer(MCUVC_HDUPOLLING_CHANGE_TIMER);			
        }
        break;

    case MCS_MCU_RESUMEHDUPOLL_CMD:
        {
            m_tConf.m_tStatus.SetHduPollState(POLL_STATE_NORMAL);
            cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetHduPollInfo(), sizeof(THduPollInfo));
            SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
			m_tHduPollParam.SetIsStartAsPause( 1 );
                        
            SetTimer(MCUVC_HDUPOLLING_CHANGE_TIMER,1000*1);
        }
        break;

    case MCS_MCU_STOPHDUPOLL_CMD:
        {
            TMtPollParam tMtPollParam;
            memset(&tMtPollParam, 0, sizeof(tMtPollParam));
           
			m_tConf.m_tStatus.SetHduPollState(POLL_STATE_NONE);
            m_tConf.m_tStatus.SetHduMtPollParam(tMtPollParam);
            cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetHduPollInfo(), sizeof(THduPollInfo));
            SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
            
            THduPollInfo *ptPollInfo = m_tConf.m_tStatus.GetHduPollInfo();
            TMt tMt = (TMt)ptPollInfo->GetMtPollParam();
            ChangeHduSwitch(&tMt, ptPollInfo->GetTvWall().GetEqpId(), ptPollInfo->GetTWChnnl(), 
                                TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);
			m_tHduPollParam.SetIsStartAsPause( 0 );

            TPeriEqpStatus tHduStatus;
			g_cMcuVcApp.GetPeriEqpStatus( ptPollInfo->GetTvWall().GetEqpId(), &tHduStatus );
            tHduStatus.m_tStatus.tHdu.atHduChnStatus[ptPollInfo->GetTWChnnl()].SetSchemeIdx( 0 );
            g_cMcuVcApp.SetPeriEqpStatus( ptPollInfo->GetTvWall().GetEqpId(), &tHduStatus );
            KillTimer(MCUVC_HDUPOLLING_CHANGE_TIMER);
        }
        break;

    case MCS_MCU_GETHDUPOLLPARAM_REQ:
        {
            cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetHduPollInfo(), sizeof(THduPollInfo));
            cServMsg.CatMsgBody((u8 *)m_tHduPollParam.GetPollMtByIdx(0),
                                m_tHduPollParam.GetPolledMtNum()*sizeof(TMtPollParam));
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

    switch(pcMsg->event)
    {
    case MCS_MCU_STARTTWPOLL_CMD:
        {
            TPeriEqpStatus tTWInfo;
            TTvWallPollInfo tPollInfo = *(TTvWallPollInfo *)(cServMsg.GetMsgBody());
            
            if(tPollInfo.GetTWChnnl() >= MAXNUM_PERIEQP_CHNNL)
            {
                ConfLog(FALSE, "[ProcMcsMcuTWPollMsg] tvwall chnnl id is invalid :%d\n", tPollInfo.GetTWChnnl());
                break;
            }
            
            //通道上已经是否有终端            
            g_cMcuVcApp.GetPeriEqpStatus(tPollInfo.GetTvWall().GetEqpId(), &tTWInfo);
            TTWMember tTwMem = tTWInfo.m_tStatus.tTvWall.atVideoMt[tPollInfo.GetTWChnnl()];
            if(0 != tTwMem.GetConfIdx() && tTwMem.GetConfIdx() != m_byConfIdx)//其他会议的没有权限操作, 本会议的替换
            {
                ConfLog(FALSE, "[ProcMcsMcuTWPollMsg] Confidx %d is already in polling, can't replace it\n", tTwMem.GetConfIdx());
                break;
            }
            
            if(POLL_STATE_NONE != m_tConf.m_tStatus.GetTvWallPollState())
            {
                ConfLog(FALSE, "[ProcMcsMcuTWPollMsg] tvwall chnnl id :%d is already in polling\n", tPollInfo.GetTWChnnl());
                break;
            }

            //得到轮询mt参数
			u8 byPollMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TTvWallPollInfo)) / sizeof(TMtPollParam);
            TMtPollParam *atMtPollParam = (TMtPollParam *)( cServMsg.GetMsgBody() + sizeof(TTvWallPollInfo) );
		    m_tTvWallPollParam.InitPollParam( byPollMtNum, atMtPollParam );
            m_tTvWallPollParam.SetTvWallEqp(tPollInfo.GetTvWall());
            m_tTvWallPollParam.SetTvWallChnnl(tPollInfo.GetTWChnnl());
         
            // xsl [8/23/2006] 判断卫星会议是否停止轮询
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {            
                //若回传个数已满其轮询终端中有不发码流的终端则拒绝轮询
                if (IsOverSatCastChnnlNum(0))
                {
                    for(u8 byIdx = 0; byIdx < m_tTvWallPollParam.GetPolledMtNum(); byIdx++)
                    {
                        if (m_ptMtTable->GetMtSndBitrate(m_tTvWallPollParam.GetPollMtByIdx(byIdx)->GetMtId()) == 0)
                        {
                            ConfLog(FALSE, "[ProcMcsMcuTWPollMsg-start] over max upload mt num. nack!\n");  
                            NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_DCAST_OVERCHNNLNUM);
                            memset(&m_tTvWallPollParam, 0, sizeof(m_tTvWallPollParam));
                            return;
                        }
                    }
                }            
            }     

            m_tConf.m_tStatus.SetTvWallPollInfo(tPollInfo);
            m_tConf.m_tStatus.SetTvWallPollState(POLL_STATE_NORMAL);                        
            
			u8 byCurPollPos = m_tTvWallPollParam.GetCurrentIdx();
            TMtPollParam *ptCurPollMt = GetMtTWPollParam( byCurPollPos );
            m_tTvWallPollParam.SetCurrentIdx( byCurPollPos );
			
            if (NULL != ptCurPollMt)
            {
                TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);   
                
                TLogicalChannel tSrcRvsChannl;
				if (!m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
				{
						ConfLog(FALSE, "[ProcMcsMcuTWPollMsg]: GetMtLogicChnnl fail!\n");
						return;
				}
				
				// xliang [7/13/2009] DEC5 adjust resolution strategy
				if ( tSrcRvsChannl.GetChannelType() == MEDIA_TYPE_H264 &&
					( IsVidFormatHD( tSrcRvsChannl.GetVideoFormat() ) ||
					tSrcRvsChannl.GetVideoFormat() == VIDEO_FORMAT_4CIF ) )
				{
					// nack for none keda MT whose resolution is about  h264 4Cif 
					if ( MT_MANU_KDC != m_ptMtTable->GetManuId(tLocalMt.GetMtId()) )
					{
						ConfLog(FALSE, "[ProcMcsMcuTWPollMsg] Mt<%d> VidType.%d with format.%d support no tvwall\n",
							tLocalMt.GetMtId(), tSrcRvsChannl.GetChannelType(), tSrcRvsChannl.GetVideoFormat() );
// 						cServMsg.SetErrorCode(ERR_MT_MEDIACAPABILITY);
// 						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						return;
					}
					else
					{
						// force the mt to adjust its resolution to Cif
						// FIXME: cascade adjusting
						u8 byChnnlType = LOGCHL_VIDEO;
						u8 byNewFormat = VIDEO_FORMAT_CIF;
						CServMsg cMsg;
						cMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
						cMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
						cMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
						SendMsgToMt( tLocalMt.GetMtId(), cMsg.GetEventId(), cMsg );
						
						McsLog( "[ProcMcsMcuTWPollMsg]send videoformat<%d>(%s) change msg to mt<%d>!\n",
							byNewFormat, GetResStr(byNewFormat), tLocalMt.GetMtId() );
					}
					
				}
                //通知所有会控
                m_tConf.m_tStatus.SetTvWallMtPollParam(*ptCurPollMt);
                cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo));
                SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);

                ChangeTvWallSwitch(&tLocalMt, tPollInfo.GetTvWall().GetEqpId(), tPollInfo.GetTWChnnl(), 
                                    TW_MEMBERTYPE_TWPOLL, TW_STATE_START);

                ConfStatusChange();
                
                //设置轮询时钟
                SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());
            }
            else
            {
                //设置轮询时钟
                SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER, 1000 * 5);
            }            
        }
        break;

    case MCS_MCU_PAUSETWPOLL_CMD:
        {
            m_tConf.m_tStatus.SetTvWallPollState(POLL_STATE_PAUSE);
            cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo));
            SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
                        
            KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER);			
        }
        break;

    case MCS_MCU_RESUMETWPOLL_CMD:
        {
            m_tConf.m_tStatus.SetTvWallPollState(POLL_STATE_NORMAL);
            cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo));
            SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
                        
            SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER,1000*1);
        }
        break;

    case MCS_MCU_STOPTWPOLL_CMD:
        {
            TMtPollParam tMtPollParam;
            memset(&tMtPollParam, 0, sizeof(tMtPollParam));

            m_tConf.m_tStatus.SetTvWallPollState(POLL_STATE_NONE);
            m_tConf.m_tStatus.SetTvWallMtPollParam(tMtPollParam);
            cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo));
            SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
            
            TTvWallPollInfo *ptPollInfo = m_tConf.m_tStatus.GetTvWallPollInfo();
            TMt tMt = (TMt)ptPollInfo->GetMtPollParam();
            ChangeTvWallSwitch(&tMt, ptPollInfo->GetTvWall().GetEqpId(), ptPollInfo->GetTWChnnl(), 
                                TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);

            KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER);
        }
        break;

    case MCS_MCU_GETTWPOLLPARAM_REQ:
        {
            cServMsg.SetMsgBody((u8 *)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo));
            cServMsg.CatMsgBody((u8 *)m_tTvWallPollParam.GetPollMtByIdx(0),
                                m_tTvWallPollParam.GetPolledMtNum()*sizeof(TMtPollParam));
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
            CfgLog( FALSE, " [ProcHduBatchPollChangeTimerMsg] current state is not hdu batch polling! \n" );
			KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
				
		    return;
		}
        
		StopHduBatchPollSwitch();
/*		
        if ( !HduBatchPollOfOneCycle( FALSE ) )
        {
            ConfLog( FALSE, "[ProcHduBatchPollChangeTimerMsg] have no Mt in current conf!\n" );
			cServMsg.SetErrorCode( ERR_MCU_HDUBATCHPOLL_NONEMT );
			SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 2, cServMsg );
            KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
			return;
        }
*/        
		// 轮询结束
		if ( ( !HduBatchPollOfOneCycle(FALSE) ) 
			|| ( 0 != m_tHduBatchPollInfo.GetCycles() 
				&& m_tHduBatchPollInfo.GetCurrentCycle() == m_tHduBatchPollInfo.GetCycles() )
			)
		{
			StopHduBatchPollSwitch(TRUE);
			m_tHduBatchPollInfo.SetNull();
			m_tConf.m_tStatus.m_tConfMode.SetHduInBatchPoll( POLL_STATE_NONE );
			m_tConf.GetConfAttrbEx().SetSchemeIdxInBatchPoll( 0 );
			m_tHduPollSchemeInfo.SetStatus( POLL_STATE_NONE );
			ConfStatusChange();            // 通知会控会议状态改变

			cServMsg.SetMsgBody( (u8*)&m_tHduPollSchemeInfo, sizeof( m_tHduPollSchemeInfo ) );
			SendMsgToAllMcs( MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF, cServMsg );
			
			//通知所有会控
			cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
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
		OspPrintf( TRUE, FALSE, "[ProcHduBatchPollChangeTimerMsg] state(%d) is wrong! \n", CurState() );
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
  07/05/09    4.6         江乐斌         创建
=============================================================================*/
void CMcuVcInst::ProcHduPollingChangeTimerMsg(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
    
    switch(CurState())
    {
    case STATE_ONGOING:
        {
            if (POLL_STATE_NONE == m_tConf.m_tStatus.GethduPollState()) 
            {
                break;
            }
			TMtPollParam *ptCurPollMt = NULL;
			u8 byCurPollPos = m_tHduPollParam.GetCurrentIdx();
            if ( 0 == m_tHduPollParam.GetIsStartAsPause() )   
            {
				byCurPollPos++;
            }
			else // 由暂停恢复时，接着当前终端轮询   
			{
				 m_tHduPollParam.SetIsStartAsPause( 0 );
			}

            ptCurPollMt = GetMtHduPollParam( byCurPollPos );
			m_tHduPollParam.SetCurrentIdx( byCurPollPos );
            
            BOOL32 bStopPoll = FALSE;
            // 判断卫星会议是否停止轮询
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                TMt tLastPollMt = (TMt)m_tConf.m_tStatus.GetHduMtPollParam();
                if (IsMtSendingVidToOthers(tLastPollMt, FALSE, TRUE, 0) && //上一个终端还在向其他实体发送码流
                    m_ptMtTable->GetMtSndBitrate(ptCurPollMt->GetMtId()) == 0 && //下一个终端没有在发送码流
                    IsOverSatCastChnnlNum(0))
                {
                    bStopPoll = TRUE;
                }
            }        

            if (NULL != ptCurPollMt && !bStopPoll)
            {
                //通知所有会控
                TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);
                
                m_tConf.m_tStatus.SetHduMtPollParam( *ptCurPollMt );
                cServMsg.SetMsgBody( (u8*)m_tConf.m_tStatus.GetHduPollInfo(), sizeof(THduPollInfo) );
                SendMsgToAllMcs( MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg );
                
                THduPollInfo *ptPollInfo = m_tConf.m_tStatus.GetHduPollInfo();
                ChangeHduSwitch(&tLocalMt, ptPollInfo->GetTvWall().GetEqpId(), ptPollInfo->GetTWChnnl(), 
                                   TW_MEMBERTYPE_TWPOLL, TW_STATE_CHANGE, m_tHduPollParam.GetMode());               
                                
                SetTimer(MCUVC_HDUPOLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());

                McsLog("[ProcHduPollingChangeTimerMsg] new mt %d in polling, remain poll times :%u\n", ptCurPollMt->GetMtId(), ptPollInfo->GetPollNum());
            
			    
			}
            else
            {              
                TMtPollParam tMtPollParam;
                memset(&tMtPollParam, 0, sizeof(tMtPollParam));

                m_tConf.m_tStatus.SetHduPollState(POLL_STATE_NONE);
                m_tConf.m_tStatus.SetHduMtPollParam(tMtPollParam);
                cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetHduPollInfo(), sizeof(THduPollInfo));
                SendMsgToAllMcs(MCU_MCS_HDUPOLLSTATE_NOTIF, cServMsg);
                
                THduPollInfo *ptPollInfo = m_tConf.m_tStatus.GetHduPollInfo();
                TMt tMt = (TMt)ptPollInfo->GetMtPollParam();
                ChangeHduSwitch(&tMt, ptPollInfo->GetTvWall().GetEqpId(), ptPollInfo->GetTWChnnl(), 
                                    TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);
                
                KillTimer(MCUVC_HDUPOLLING_CHANGE_TIMER);

                McsLog("[ProcTWPollingChangeTimerMsg] tvwall polling finished\n");
            }
            
            break;
        }
        
    default:
        break;
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
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
    
    switch(CurState())
    {
    case STATE_ONGOING:
        {
            if (POLL_STATE_NONE == m_tConf.m_tStatus.GetTvWallPollState()) 
            {
                break;
            }
			u8 byCurPollPos = m_tTvWallPollParam.GetCurrentIdx();
            byCurPollPos++;
            TMtPollParam *ptCurPollMt = GetMtTWPollParam( byCurPollPos );
			m_tTvWallPollParam.SetCurrentIdx( byCurPollPos );
            
            BOOL32 bStopPoll = FALSE;
            // xsl [8/23/2006] 判断卫星会议是否停止轮询
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                TMt tLastPollMt = (TMt)m_tConf.m_tStatus.GetTvWallMtPollParam();
                if (IsMtSendingVidToOthers(tLastPollMt, FALSE, TRUE, 0) && //上一个终端还在向其他实体发送码流
                    m_ptMtTable->GetMtSndBitrate(ptCurPollMt->GetMtId()) == 0 && //下一个终端没有在发送码流
                    IsOverSatCastChnnlNum(0))
                {
                    bStopPoll = TRUE;
                }
            }        

            if (NULL != ptCurPollMt && !bStopPoll)
            {
                //通知所有会控
                TMt tLocalMt = GetLocalMtFromOtherMcuMt(*ptCurPollMt);
                
				TLogicalChannel tSrcRvsChannl;
				if (!m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
				{
					ConfLog(FALSE, "[ProcMcsMcuTWPollMsg]: GetMtLogicChnnl fail!\n");
					return;
				}
				
				// xliang [7/13/2009] DEC5 adjust resolution strategy
				if ( tSrcRvsChannl.GetChannelType() == MEDIA_TYPE_H264 &&
					( IsVidFormatHD( tSrcRvsChannl.GetVideoFormat() ) ||
					tSrcRvsChannl.GetVideoFormat() == VIDEO_FORMAT_4CIF ) )
				{
					// nack for none keda MT whose resolution is about  h264 4Cif 
					if ( MT_MANU_KDC != m_ptMtTable->GetManuId(tLocalMt.GetMtId()) )
					{
						ConfLog(FALSE, "[ProcMcsMcuTWPollMsg] Mt<%d> VidType.%d with format.%d support no tvwall\n",
							tLocalMt.GetMtId(), tSrcRvsChannl.GetChannelType(), tSrcRvsChannl.GetVideoFormat() );
						// 						cServMsg.SetErrorCode(ERR_MT_MEDIACAPABILITY);
						// 						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						return;
					}
					else
					{
						// force the mt to adjust its resolution to Cif
						// FIXME: cascade adjusting
						u8 byChnnlType = LOGCHL_VIDEO;
						u8 byNewFormat = VIDEO_FORMAT_CIF;
						CServMsg cMsg;
						cMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
						cMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
						cMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
						SendMsgToMt( tLocalMt.GetMtId(), cMsg.GetEventId(), cMsg );
						
						McsLog( "[ProcMcsMcuTWPollMsg]send videoformat<%d>(%s) change msg to mt<%d>!\n",
							byNewFormat, GetResStr(byNewFormat), tLocalMt.GetMtId() );
					}
					
				}

                m_tConf.m_tStatus.SetTvWallMtPollParam( *ptCurPollMt );
                cServMsg.SetMsgBody( (u8*)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo) );
                SendMsgToAllMcs( MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg );
                
                TTvWallPollInfo *ptPollInfo = m_tConf.m_tStatus.GetTvWallPollInfo();
                ChangeTvWallSwitch(&tLocalMt, ptPollInfo->GetTvWall().GetEqpId(), ptPollInfo->GetTWChnnl(), 
                                   TW_MEMBERTYPE_TWPOLL, TW_STATE_CHANGE);               
                                
                SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER, 1000 * ptCurPollMt->GetKeepTime());

                McsLog("[ProcTWPollingChangeTimerMsg] new mt %d in polling, remain poll times :%u\n", ptCurPollMt->GetMtId(), ptPollInfo->GetPollNum());
            }
            else
            {              
                TMtPollParam tMtPollParam;
                memset(&tMtPollParam, 0, sizeof(tMtPollParam));

                m_tConf.m_tStatus.SetTvWallPollState(POLL_STATE_NONE);
                m_tConf.m_tStatus.SetTvWallMtPollParam(tMtPollParam);
                cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo));
                SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);
                
                TTvWallPollInfo *ptPollInfo = m_tConf.m_tStatus.GetTvWallPollInfo();
                TMt tMt = (TMt)ptPollInfo->GetMtPollParam();
                ChangeTvWallSwitch(&tMt, ptPollInfo->GetTvWall().GetEqpId(), ptPollInfo->GetTWChnnl(), 
                                    TW_MEMBERTYPE_TWPOLL, TW_STATE_STOP);
                
                KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER);

                McsLog("[ProcTWPollingChangeTimerMsg] tvwall polling finished\n");
            }
            
            break;
        }
        
    default:
        break;
    }
    
    return;
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
                McsLog("[ProcMcsMcuRollCallMsg] unexpected Rollcall mode.%d\n", byRollCallMode);
                return;
            }

            if ( m_tConf.m_tStatus.IsMustSeeSpeaker() )
            { 
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_MUSTSEESPEAKER);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] can't start rollcall due to conf mode.mustseespeaker\n");
                return;
            }

            // zbq[11/24/2007] 会议轮询的时候不允许开启点名
            if ( m_tConf.m_tStatus.GetPollMode() != CONF_POLLMODE_NONE )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_POLL);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] can't start rollcall due to conf pollmode.%d\n", m_tConf.m_tStatus.GetPollMode());
                return;
            }

            // zbq[03/31/2008] 点名资源预审(FIXME：未占用，可能会被抢)
            u8 byGrpId = 0;
            u8 byEqpId = 0;
            if ( 0 == g_cMcuVcApp.GetIdleMixGroup(byEqpId, byGrpId) &&
				// 新点名逻辑支持本会议内冲混音模式，所以这里必须判断本会议
				// 是否占用了混音器, zgc, 2008-05-22
				m_tConf.m_tStatus.IsNoMixing() &&
				!m_tConf.m_tStatus.IsVACing() )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_NOMIXER);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] can't start rollcall due to no idle mixer\n");
                return;
            }

            u8 byIdleVMPNum = 0;
            u8 abyIdleVMPId[MAXNUM_PERIEQP];
            memset( &abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
            g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );    
			// BUG10614会议点名被错误拒掉, zgc, 2008-04-28
			// 非VMP模式不需要判断是否有VMP空闲
            // 新点名逻辑支持本会议内切换合成模式，所以这里必须判断是否是本会议占用了画面
            // 合成器，zgc, 2008-05-23
            if ( byIdleVMPNum == 0 && byRollCallMode == ROLLCALL_MODE_VMP &&
                m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_NONE )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_NOVMP);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] can't start rollcall due to no idle vmp\n");
                return;
            }
            // zgc, 2008-05-29, 检查VMP能力，如果连左右两画面都无法支持，则拒绝点名
            if ( byRollCallMode == ROLLCALL_MODE_VMP )
            {
                u16 wError = 0;
                u8 byVmpId = 0;
                BOOL32 bRet = IsMCUSupportVmpStyle(VMP_STYLE_VTWO, byVmpId, EQP_TYPE_VMP, wError);
                if ( !bRet )
                {
                    cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_VMPABILITY);
                    SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                    McsLog("[ProcMcsMcuRollCallMsg] can't start rollcall due to vmp ability is not enough!\n");
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
                CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode() &&
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
            if ( CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode() &&
                 ROLLCALL_MODE_VMP != byRollCallMode )
            {
                NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_VMPING );
            }

            BOOL32 bSelExist = FALSE;
            for( u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++ )
            {
                if ( m_tConfAllMtInfo.MtJoinedConf( byMtIdx ) ) 
                {
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
                
                cMsg.content = cServMsg.GetServMsg();
                cMsg.length = cServMsg.GetServMsgLen();
                ProcMcsMcuLockSMcuReq(&cMsg);                
            }

            u8 byLockMode = CONF_LOCKMODE_LOCK;
            cSMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
            cSMsg.SetMsgBody((u8*)&byLockMode, sizeof(u8));
            ChangeConfLockMode(cSMsg);

            // zbq [11/22/2007] 置当前会议为无主席会议
            m_tConf.m_tStatus.SetNoChairMode( TRUE );
            if( m_tConf.m_tStatus.IsNoChairMode() && HasJoinedChairman() )
            {
                ChangeChairman( NULL );	
            }

            m_tConf.m_tStatus.SetRollCallMode( byRollCallMode );
            
            McsLog("[ProcMcsMcuRollCallMsg] RollCallMode.%d\n", byRollCallMode);

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
                McsLog("[ProcMcsMcuRollCallMsg] Rollcall mode.%d, can't change caller or callee\n", m_tConf.m_tStatus.GetRollCallMode());
                return;
            }
            if ( cServMsg.GetMsgBodyLen() != sizeof(TMt)*2 )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_MSGLEN);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] unexpected msglen.%d, need.%d\n", cServMsg.GetMsgBodyLen(), sizeof(TMt)*2 );
                return;
            }

            TMt tNewRollCaller = *(TMt*)cServMsg.GetMsgBody();
            TMt tNewRollCallee = *(TMt*)(cServMsg.GetMsgBody() + sizeof(TMt));

            if ( tNewRollCaller.IsNull() )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_CALLERNULL);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] Caller Null, ignore it\n");
                return;
            }

            if ( tNewRollCallee.IsNull() )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_CALLEENULL);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] Callee Null, ignore it\n");
                return;
            }

            BOOL32 bCallerChged = tNewRollCaller == m_tRollCaller ? FALSE : TRUE;
            BOOL32 bCalleeChged = tNewRollCallee == m_tRollCallee ? FALSE : TRUE;

            if ( bCallerChged && !tNewRollCaller.IsLocal() )
            {
                cServMsg.SetErrorCode(ERR_MCU_ROLLCALL_CALLER_SMCUMT);
                SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
                McsLog("[ProcMcsMcuRollCallMsg] Mt<%d,%d> can't be caller, it belong to SMcu\n", 
                        tNewRollCaller.GetMcuId(), tNewRollCaller.GetMtId() );
                return;
            }

            SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

            TMt tOldRollCaller = m_tRollCaller;
            TMt tOldRollCallee = m_tRollCallee;

            m_tRollCaller = tNewRollCaller;
            m_tRollCallee = tNewRollCallee;

            if ( !bCallerChged && !bCalleeChged )
            {
                McsLog("[ProcMcsMcuRollCallMsg] there is no change to caller and callee both\n");
                return;
            }

            // 非VMP点名模式，需要停合成, zgc, 2008-05-23
            if ( ROLLCALL_MODE_VMP != m_tConf.m_tStatus.GetRollCallMode() &&
                CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode() )
            {
                //停止时模板失效
			    if( m_tConf.GetConfAttrb().IsHasVmpModule() )
			    {
				    TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
                    tConfAttrib.SetHasVmpModule(FALSE);
                    m_tConf.SetConfAttrb( tConfAttrib );
                }

                //停止
				TPeriEqpStatus tPeriEqpStatus;
				if (!m_tVmpEqp.IsNull() &&
					g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
					tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
					tPeriEqpStatus.m_tStatus.tVmp.m_byUseState != TVmpStatus::WAIT_STOP)
				{
					SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, 6*1000);
					tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
					g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tPeriEqpStatus);
					g_cEqpSsnApp.SendMsgToPeriEqpSsn( m_tVmpEqp.GetEqpId(),MCU_VMP_STOPVIDMIX_REQ, 
													  (u8*)cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

				}
            }

            // zbq [11/23/2007] 给点名人或被点名人发言人位置
            if ( bCallerChged &&
                 ROLLCALL_MODE_CALLEE != m_tConf.m_tStatus.GetRollCallMode() )
            {
				// KDV-BUG2004: 点名人或被点名人被强制为发言人，而在MTC上终端状态未改变
				// zgc, 2008-05-21, 用ChangeSpeaker函数代替
                //m_tConf.SetSpeaker( tNewRollCaller );
				ChangeSpeaker( &tNewRollCaller );
            }
            if ( bCalleeChged &&
                 ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
            {
				// KDV-BUG2004: 点名人或被点名人被强制为发言人，而在MTC上终端状态未改变
				// zgc, 2008-05-21, 用ChangeSpeaker函数代替
                //m_tConf.SetSpeaker( tNewRollCallee );
				ChangeSpeaker( &tNewRollCallee );
            }
            //通知所有会控
			// KDV-BUG2004: 点名人或被点名人被强制为发言人，而在MTC上终端状态未改变
			// zgc, 2008-05-21, 在ChangeSpeaker函数中处理
            //cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
            //SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

            //调整混音
            BOOL32 bLocalMixStarted = FALSE;
            
            if ( bCallerChged )
            {
                if ( !m_tConf.m_tStatus.IsNoMixing() )
                {
                    BOOL32 bLocalAutoMix = FALSE;
                    
                    // 停语音激励
                    if ( m_tConf.m_tStatus.IsVACing() )
                    {
                        m_tConf.m_tStatus.SetVACing(FALSE);
                        //通知主席及所有会控
                        SendMsgToAllMcs(MCU_MCS_STOPVAC_NOTIF, cServMsg);
                        if (HasJoinedChairman())
                        {
                            SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cServMsg);
                        }
                    }
                    
                    //停讨论
                    if ( m_tConf.m_tStatus.IsAutoMixing() )
                    {
                        bLocalAutoMix = TRUE;

                        m_tConf.m_tStatus.SetAutoMixing(FALSE);
                        m_tConf.m_tStatus.SetSpecMixing(TRUE);
                        
                        //FIXME；是否要考虑上限 56
                        //拆除原先的N-1模式交换
                        for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
                        {
                            if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
                            {
                                StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
                                                    (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+byMtId), FALSE, MODE_AUDIO);
                                
                                StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);                       
                                
                                // 恢复从组播地址接收
                                if (m_tConf.GetConfAttrb().IsSatDCastMode() && m_ptMtTable->IsMtInMixGrp(byMtId))
                                {
                                    ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_AUDIO);
                                }
                                m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);
                            }
                        }
                    }
                    
                    //本来就是定制混音
                    if ( m_tConf.m_tStatus.IsSpecMixing() )
                    {
                        //第一次点名，移除当前所有终端
                        if ( tOldRollCaller.IsNull() )
                        {
                            for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
                            {
                                TMt tMt = GetLocalMtFromOtherMcuMt(tNewRollCallee);
                                
                                if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
                                    m_ptMtTable->IsMtInMixing(byMtId) &&
                                    !(m_ptMtTable->GetMt(byMtId) == tMt))
                                {
                                    TMt tMt = m_ptMtTable->GetMt(byMtId);
                                    AddRemoveSpecMixMember(&tMt, 1, FALSE, TRUE);
                                }
                            }
                            AddRemoveSpecMixMember(&tNewRollCaller, 1, TRUE);
                        }
                        else
                        {
                            AddRemoveSpecMixMember(&tNewRollCaller, 1, TRUE);
                            AddRemoveSpecMixMember(&tOldRollCaller, 1, FALSE);                        
                        }
                    }
                    
                    // 如果当前本来是讨论模式, 需整理N模式的交换
                    if ( bLocalAutoMix )
                    {
                        for (u8 byMtId = 1; byMtId < MAXNUM_CONF_MT; byMtId++)
                        {
                            if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
                            {
                                TMt tMt = m_ptMtTable->GetMt(byMtId);
                                if (m_tConf.m_tStatus.IsSpecMixing() && !m_ptMtTable->IsMtInMixing(byMtId))
                                {
                                    SwitchMixMember(&tMt, TRUE);
                                }
                            }           
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
                    }                    
                }
            }
            if ( bCalleeChged )
            {
                if ( !m_tConf.m_tStatus.IsNoMixing() )
                {
                    BOOL32 bLocalAutoMix = FALSE;
                    
                    // 停语音激励
                    if ( m_tConf.m_tStatus.IsVACing() )
                    {
                        m_tConf.m_tStatus.SetVACing(FALSE);
                        
                        //通知主席及所有会控
                        SendMsgToAllMcs(MCU_MCS_STOPVAC_NOTIF, cServMsg);
                        if (HasJoinedChairman())
                        {
                            SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cServMsg);
                        }
                    }
                    
                    //停讨论
                    if ( m_tConf.m_tStatus.IsAutoMixing() )
                    {
                        bLocalAutoMix = TRUE;

                        m_tConf.m_tStatus.SetAutoMixing(FALSE);
                        
                        //FIXME；是否要考虑上限 56
                        //拆除原先的N-1模式交换
                        for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
                        {
                            if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
                            {
                                StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
                                                    (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+byMtId), FALSE, MODE_AUDIO);
                                
                                StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);                       
                                
                                // 恢复从组播地址接收
                                if (m_tConf.GetConfAttrb().IsSatDCastMode() && m_ptMtTable->IsMtInMixGrp(byMtId))
                                {
                                    ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_AUDIO);
                                }
                                m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);
                            }
                        }
                    }
                    
                    //本来就是定制混音
                    if ( m_tConf.m_tStatus.IsSpecMixing() )
                    {
                        //第一次点名，移除当前所有终端
                        if ( tOldRollCallee.IsNull() )
                        {
                            for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
                            {
                                TMt tMt = GetLocalMtFromOtherMcuMt(tNewRollCaller);
                                
                                if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
                                    m_ptMtTable->IsMtInMixing(byMtId) &&
                                    !(m_ptMtTable->GetMt(byMtId) == tMt))
                                {
                                    TMt tMt = m_ptMtTable->GetMt(byMtId);
                                    AddRemoveSpecMixMember(&tMt, 1, FALSE, TRUE);
                                }
                            }
                            AddRemoveSpecMixMember(&tNewRollCallee, 1, TRUE);
                        }
                        else
                        {
                            AddRemoveSpecMixMember(&tOldRollCallee, 1, FALSE);
                            AddRemoveSpecMixMember(&tNewRollCallee, 1, TRUE);
                            
                            //zbq[11/30/2007] 新的Callee和旧的Callee不属同一个
                            //MCU，旧Callee所在的MCU移出上级混音组，不停其混音.
                            if (!(!tOldRollCallee.IsLocal() && 
                                  !tNewRollCallee.IsLocal() && 
                                  tOldRollCallee.GetMcuId() == tNewRollCallee.GetMcuId()))
                            {
                                TMt tSMcu = GetLocalMtFromOtherMcuMt(tOldRollCallee);
                                AddRemoveSpecMixMember(&tSMcu, 1, FALSE);                                
                            }
                        }
                    }
                    
                    // 如果当前本来是讨论模式, 需整理N模式的交换
                    if ( bLocalAutoMix )
                    {
                        for (u8 byMtId = 1; byMtId < MAXNUM_CONF_MT; byMtId++)
                        {
                            if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
                            {
                                TMt tMt = m_ptMtTable->GetMt(byMtId);
                                if (m_tConf.m_tStatus.IsSpecMixing() && !m_ptMtTable->IsMtInMixing(byMtId))
                                {
                                    SwitchMixMember(&tMt, TRUE);
                                }
                            }           
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
                    }                    
                }
            }
            
            //调整广播码流
            TVMPParam tVmpParam;
            if ( m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP )
            {
                tVmpParam = m_tConf.m_tStatus.GetVmpParam();
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
                u8 byVmpId = 0;
                BOOL32 bRet = IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError);
                if ( !bRet )
                {
                    //NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_STYLECHANGE);           
                    tVmpParam.SetVMPStyle(VMP_STYLE_VTWO);
                    McsLog("[ProcMcsMcuRollCallMsg] rollcall change to style.%d due to vmp ability is not enough!\n",
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
						ConfLog( FALSE, "New roll caller(Mt.%d) is not local mt, it's impossible\n", tNewRollCaller.GetMtId() );
					}
                }
                else if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
                {
                    u8 byCallerPos = 0;
                    if ( !tOldRollCaller.IsNull() &&
                         m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
                    {
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
                         m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
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
                if ( CONF_VMPMODE_CTRL == m_tConf.m_tStatus.GetVMPMode() )
                {
                    m_tConf.m_tStatus.SetVmpParam(tVmpParam);

                    TPeriEqpStatus tPeriEqpStatus;
                    g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
                    tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
                    g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
                    
                    //ChangeVmpParam(&tVmpParam);
					// xliang [1/6/2009] 区分新老VMP设VMP param
					AdjustVmpParam(&tVmpParam);

                }
                else if ( CONF_VMPMODE_AUTO == m_tConf.m_tStatus.GetVMPMode() )
                {
                    m_tConf.m_tStatus.SetVMPMode(CONF_VMPMODE_CTRL);

                    m_tConf.m_tStatus.SetVmpParam(tVmpParam);

                    TPeriEqpStatus tPeriEqpStatus;
                    g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
                    tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
                    g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
                    
                    //ChangeVmpParam(&tVmpParam);
					// xliang [1/6/2009] 区分新老VMP设VMP param
					AdjustVmpParam(&tVmpParam);
					
                    ConfModeChange();
                }
                else
                {
                    CServMsg cMsg;
                    cMsg.SetEventId(MCS_MCU_STARTVMP_REQ);
                    cMsg.SetMsgBody((u8*)&tVmpParam, sizeof(TVMPParam));
					
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
                    }
                    
                    //选看失败，恢复看广播，可能看vmp或看自己
					//点名人选看被点名人
                    if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
                    {
                        RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
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
                    if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
                    {
                        RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
                    }
                }
                else // ROLLCALL_MODE_CALLEE
                {
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
                    if ( tOldRollCallee.IsLocal() &&
                        !tOldRollCallee.IsNull() )
                    {
                        StopSelectSrc(tOldRollCallee, MODE_VIDEO);
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
                McsLog("[ProcMcsMcuRollCallMsg] RollCall mode.%d already\n", m_tConf.m_tStatus.GetRollCallMode());
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
    CServMsg cSMsg;
    u8 byLockMode = CONF_LOCKMODE_NONE;
    cSMsg.SetServMsg(cServMsg.GetServMsg(), SERV_MSGHEAD_LEN);
    cSMsg.SetMsgBody((u8*)&byLockMode, sizeof(u8));
    ChangeConfLockMode(cSMsg);
    
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
            CServMsg cMsg;
            cMsg.SetConfIdx( m_byConfIdx );
            cMsg.SetConfId( m_tConf.GetConfId() );
            cMsg.SetEventId(MCS_MCU_STOPVMP_REQ);
            VmpCommonReq(cMsg);
        }
        RestoreAllSubMtJoinedConfWatchingSrcMt(m_tVidBrdSrc, MODE_VIDEO);
        
        //停选看
        StopSelectSrc(m_tRollCaller, MODE_BOTH);
        if ( m_tRollCallee.IsLocal() )
        {
            StopSelectSrc(m_tRollCallee, MODE_BOTH);
        }
        
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
void CMcuVcInst::DaemonProcMcsMcuListAllConfReq( const CMessage * pcMsg, CApp* pcApp )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	NtfMcsMcuCurListAllConf(cServMsg);

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
void CMcuVcInst::DaemonProcMcsMcuGetMcuStatusReq( const CMessage * pcMsg, CApp* pcApp )
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
		StopSwitchToMc( cServMsg.GetSrcSsnId(), cServMsg.GetChnIndex() );
		break;
	case MCU_MCSDISCONNECTED_NOTIF:
		byInstId = *( u8 * )cServMsg.GetMsgBody();
		if( g_cMcuVcApp.GetMcLogicChnnl( byInstId, MODE_VIDEO, &byChannelNum, &tLogicalChannel ) )
		{
			while( byChannelNum-- != 0 )
            {
			    StopSwitchToMc( byInstId, byChannelNum, FALSE, MODE_BOTH, FALSE );
            }
		}
		break;
    default:
        ConfLog( FALSE, "ProcMcStopSwitch: unexpected msg %d<%s> received !\n", cServMsg.GetEventId(), OspEventDesc(cServMsg.GetEventId()) );
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
	TLogicalChannel tLogicalChannel;
	
	tHduSwitchInfo = *( THduSwitchInfo * )cServMsg.GetMsgBody();
	tEqp = tHduSwitchInfo.GetDstMt();
	
	// Nack
	if( tEqp.GetEqpType() != EQP_TYPE_HDU )
	{
		ConfLog( FALSE, "[CMcuVcInst]::ProcMcsMcuStopSwitchHduReq: Wrong PeriEquipment type %u or it has not registered!\n", 
			tEqp.GetEqpType() );
		cServMsg.SetErrorCode( ERR_MCU_WRONGEQP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	TMt tRemoveMt;
    TPeriEqpStatus tHduStatus;
    u8             byHduId = 0;
    u8             byChnlNo = 0;
    u8             byMtId;
    byHduId = tEqp.GetEqpId();
    byChnlNo   = tHduSwitchInfo.GetDstChlIdx();
    g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
    tRemoveMt = tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlNo];

	if (tRemoveMt.GetType() != TYPE_MCUPERI)
	{
		if (tRemoveMt.GetConfIdx() != m_byConfIdx)
		{
			return;
		}
	}
    byMtId = tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlNo].GetMtId();
	
	StopSwitchToPeriEqp( tEqp.GetEqpId(), tHduSwitchInfo.GetDstChlIdx() );
	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
    if( IsMtNotInOtherHduChnnl(byMtId, byHduId, byChnlNo) )  
    {
        m_ptMtTable->SetMtInHdu(byMtId, FALSE);
        MtStatusChange();
    }
    
    tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlNo].byMemberType = 0;
    tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlNo].SetNull();
	tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlNo].SetConfIdx( 0 );
    tHduStatus.m_tStatus.tHdu.atHduChnStatus[byChnlNo].SetSchemeIdx( 0 );
    g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);

	cServMsg.SetMsgBody((u8*)&tHduStatus, sizeof(tHduStatus));

	//  pengjie[7/27/2009]  修改在一个mcs关闭一个通道，另一个msc显示该通道的终端还在
	SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);  

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
		OspPrintf(TRUE, FALSE, "[ProcMcsMcuChangeHduVolume] pcMsg is Null!\n");
		return;
	}
	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    THduVolumeInfo tHduVolumeInfo = *(THduVolumeInfo*)cServMsg.GetMsgBody();
	TPeriEqpStatus tHduStatus;
    u8 byChnlIdx = tHduVolumeInfo.GetChnlIdx();
	g_cMcuVcApp.GetPeriEqpStatus( tHduVolumeInfo.GetEqpId(), &tHduStatus );

    if ( 0 == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx() )
    {
        tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetConfIdx( m_byConfIdx );
    }

    if ( m_byConfIdx != tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx() )
    {
		ConfLog( FALSE, "[CMcuVcInst]::ProcMcsMcuChangeHduVolume: Can not change hdu volume or set mute with different ConfIdx\n");
        cServMsg.SetErrorCode( ERR_MCU_HDUCHANGEVOLUME );
		SendMsgToMcs( cServMsg.GetSrcSsnId(), pcMsg->event + 2, cServMsg );
		return;
    }

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
		OspPrintf(TRUE, FALSE, "[ProcMcsMcuStartSwitchHduReq] pcMsg is NULL!\n");
		return;
    }
	
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
	THduSwitchInfo  tHduSwitchInfo;

	if (MCUVC_STARTSWITCHHDU_NOTIFY ==  pcMsg->event)
	{
		cServMsg.SetMsgBody(pcMsg->content, sizeof( THduSwitchInfo ));
	}

    tHduSwitchInfo = *(THduSwitchInfo*)cServMsg.GetMsgBody();

    TMtStatus      tMtStatus;
    TMt	           tDstMt;
    TMt            tSrcMt;
    TMt *          ptSrcMt = NULL;
    u8             byHduId = 0;
    BOOL32         bNeedSwitchMt = FALSE;    
    u8             byDstChnnlIdx;
    TMt            tSrcOrg;             //针对级联作保留

    TLogicalChannel tSrcRvsChannl;
    switch(CurState())
    {
    case STATE_ONGOING:
	{
		tSrcMt = tHduSwitchInfo.GetSrcMt();
		tDstMt = tHduSwitchInfo.GetDstMt();
        byHduId = tDstMt.GetEqpId();
        tSrcOrg = tSrcMt;
        byDstChnnlIdx = tHduSwitchInfo.GetDstChlIdx();

        //类型不对或者在本MCU上未登记
        if (EQP_TYPE_HDU != tDstMt.GetEqpType() ||
            !g_cMcuVcApp.IsPeriEqpConnected(byHduId) ||
            byDstChnnlIdx >= MAXNUM_HDU_CHANNEL)
        {
            ConfLog(FALSE, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: Wrong PeriEquipment type %u or chnnlidx %d or it(eqpid:%d) has not registered!\n", 
                            tDstMt.GetEqpType(), byDstChnnlIdx, byHduId);
            cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }

 	    //电视墙轮询优先级最高，不能选看VMP或选看终端
	    TPeriEqpStatus tHduStatus;
        g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
        if ( TW_MEMBERTYPE_TWPOLL ==  tHduStatus.m_tStatus.tHdu.atVideoMt[byDstChnnlIdx].byMemberType)
        {
			ConfLog(FALSE, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: current chnl is in polling ,can not switch VMP or mt!\n");
          	cServMsg.SetErrorCode(ERR_MCU_NOTSWITCHVMPORMT);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
		}

        if (tSrcMt.GetType() != TYPE_MCUPERI)
        {
			if (!tSrcMt.IsLocal())
			{
				tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);
			}
        }
		else
		{
			//选看的vmp为空时                      
			if (m_tVmpEqp.IsNull())
			{
				ConfLog(FALSE, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: PeriEquipment type %u has not registered!\n", 
					tDstMt.GetEqpType());
				cServMsg.SetErrorCode(ERR_MCU_MCUPERI_NOTEXIST);
	            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}

			tSrcMt = m_tVmpEqp;
		}


        // 未打开后向通道的情况在下面代码中保护
		if ( tSrcMt.GetType() != TYPE_MCUPERI && TW_MEMBERTYPE_MCSSPEC == tHduSwitchInfo.GetMemberType())
		{
			if (!m_ptMtTable->GetMtLogicChnnl( tSrcMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
			{
				ConfLog(FALSE, "[ProcMcsMcuStartSwitchHduReq]: SrcMt(%d) GetMtLogicChnnl fail!\n", tSrcMt.GetMtId());
				cServMsg.SetErrorCode(ERR_MCU_GETMTLOGCHN_FAIL);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
		}

        if (tSrcMt.GetType() != TYPE_MCUPERI && !tSrcOrg.IsLocal())
        {		
            OnMMcuSetIn(tSrcOrg, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
            tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);
        }

        // xsl [7/20/2006] 卫星分散会议时需要判断回传通道数
        if (m_tConf.GetConfAttrb().IsSatDCastMode())
        {
            if (IsOverSatCastChnnlNum(tSrcMt.GetMtId()))
            {
                ConfLog(FALSE, "[ProcMcsMcuStartSwitchHduReq] over max upload mt num. nack!\n");
                cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
        }
	
        switch(tHduSwitchInfo.GetMemberType()) 
        {
        case TW_MEMBERTYPE_MCSSPEC:

            tSrcMt = m_ptMtTable->GetMt(tSrcMt.GetMtId());

            //源终端必须具备视频发送能力
            m_ptMtTable->GetMtStatus(tSrcMt.GetMtId(), &tMtStatus);
            if (FALSE == tMtStatus.IsSendVideo())
            {
                cServMsg.SetErrorCode(ERR_MCU_SRCISRECVONLY);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
                return;
            }	
			
            //源终端必须已经加入会议
            if (FALSE == m_tConfAllMtInfo.MtJoinedConf(tSrcMt.GetMtId()))
            {
                log(LOGLVL_DEBUG1, "[ProcMcsMcuStartSwitchHduReq]: select source MT%u-%u has not joined current conference! Error!\n",
                                    tSrcMt.GetMcuId(), tSrcMt.GetMtId());
                cServMsg.SetErrorCode(ERR_MCU_MT_NOTINCONF);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);	//nack
                return;
            }

            bNeedSwitchMt = TRUE;
            break;

        case TW_MEMBERTYPE_SPEAKER:     //注意改变发言人时改变交换
			
            if (TRUE == HasJoinedSpeaker())
            {
                bNeedSwitchMt = TRUE;
                tSrcMt = GetLocalSpeaker();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;

        case TW_MEMBERTYPE_CHAIRMAN:    //注意改变主席时改变交换

            if (TRUE == HasJoinedChairman())
            {
                bNeedSwitchMt = TRUE;
                tSrcMt = m_tConf.GetChairman();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;

        case TW_MEMBERTYPE_POLL:        //注意改变轮询广播源时改变交换

            if (CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode())
            {
                bNeedSwitchMt = TRUE;
                tSrcMt = (TMt)m_tConf.m_tStatus.GetMtPollParam();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;
			
		case TW_MEMBERTYPE_SWITCHVMP:      //hdu选看vmp
		    if (!m_tVmpEqp.IsNull())
			{
			    tSrcMt = (TMt)m_tVmpEqp;
			    bNeedSwitchMt = TRUE;
			}
			else
			{
				tSrcMt.SetNull();
			}
			break;

		case TW_MEMBERTYPE_BATCHPOLL:
            bNeedSwitchMt = TRUE;
			break;

		case TW_MEMBERTYPE_TWPOLL:
			break;

        default:
            ConfLog(FALSE, "[ProcMcsMcuStartSwitchHduReq]: Wrong TvWall Member Type!\n");
            break;
		}

        tHduStatus.m_tStatus.tHdu.atHduChnStatus[byDstChnnlIdx].SetSchemeIdx(tHduSwitchInfo.GetSchemeIdx());
        g_cMcuVcApp.SetPeriEqpStatus( byHduId, &tHduStatus );

        cServMsg.SetConfIdx(tSrcMt.GetConfIdx());

        //send Ack reply
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
                
        if (bNeedSwitchMt)
        {
            ptSrcMt = &tSrcMt;
        }    

		if ( TW_MEMBERTYPE_POLL == tHduSwitchInfo.GetMemberType() )   // 区分是否带音频轮询
		{
			ChangeHduSwitch(ptSrcMt, byHduId, byDstChnnlIdx, TW_MEMBERTYPE_POLL, 
				   TW_STATE_START, m_tConf.m_tStatus.GetPollMedia() );
		}
		else if ( TW_MEMBERTYPE_BATCHPOLL == tHduSwitchInfo.GetMemberType() )   // 批量轮询时，音视频都建交换
		{
			ChangeHduSwitch( ptSrcMt, byHduId, \
				byDstChnnlIdx, TW_MEMBERTYPE_BATCHPOLL, TW_STATE_START, MODE_BOTH);
		}
        else
		{
			ChangeHduSwitch(ptSrcMt, byHduId, byDstChnnlIdx, 
				tHduSwitchInfo.GetMemberType(),	TW_STATE_START);
		}
        break;
    }
    default:
        ConfLog(FALSE, "[ProcMcsMcuStartSwitchHduReq]: Wrong handle in state %u!\n", CurState());
        break;
	}

	return;
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
    TMt *          ptSrcMt = NULL;
    u8             byTvWallId = 0;
    BOOL32         bNeedSwitchMt = FALSE;    
    u8             byDstChnnlIdx;
    TMt            tSrcOrg;             //针对级联作保留

    TLogicalChannel tSrcRvsChannl;
    switch(CurState())
    {
    case STATE_ONGOING:

        tTWSwitchInfo = *(TTWSwitchInfo *)cServMsg.GetMsgBody();
        tSrcMt = tTWSwitchInfo.GetSrcMt();
        tDstMt = tTWSwitchInfo.GetDstMt();
        byTvWallId = tDstMt.GetEqpId();
        tSrcOrg = tSrcMt;
        byDstChnnlIdx = tTWSwitchInfo.GetDstChlIdx();

        //类型不对或者在本MCU上未登记
        if (EQP_TYPE_TVWALL != tDstMt.GetEqpType() ||
            !g_cMcuVcApp.IsPeriEqpConnected(byTvWallId) ||
            byDstChnnlIdx >= MAXNUM_PERIEQP_CHNNL)
        {
            ConfLog(FALSE, "CMcuVcInst: Wrong PeriEquipment type %u or it has not registered!\n", 
                            tDstMt.GetEqpType());
            cServMsg.SetErrorCode(ERR_MCU_WRONGEQP);
            SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
            return;
        }
        
        if (tSrcMt.GetType() != TYPE_MCUPERI)
        {
			if (!tSrcMt.IsLocal())
			{
				tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt); 
			}
        }
		else
		{
			//选看的vmp为空时                      
			if (m_tVmpEqp.IsNull())
			{
				ConfLog(FALSE, "[CMcuVcInst]ProcMcsMcuStartSwitchHduReq: PeriEquipment type %u has not registered!\n", 
					tDstMt.GetEqpType());
				cServMsg.SetErrorCode(ERR_MCU_MCUPERI_NOTEXIST);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
			
			tSrcMt = m_tVmpEqp;
		}

        // 未打开后向通道的情况在下面代码中保护
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
		if ( tSrcRvsChannl.GetChannelType() == MEDIA_TYPE_H264 &&
			( IsVidFormatHD( tSrcRvsChannl.GetVideoFormat() ) ||
			tSrcRvsChannl.GetVideoFormat() == VIDEO_FORMAT_4CIF ) )
		{
			// nack for none keda MT whose resolution is about  h264 4Cif 
			if ( MT_MANU_KDC != m_ptMtTable->GetManuId(tSrcMt.GetMtId()) )
			{
				ConfLog(FALSE, "[StartSwitchTWReq] Mt<%d> VidType.%d with format.%d support no tvwall\n",
					tSrcMt.GetMtId(), tSrcRvsChannl.GetChannelType(), tSrcRvsChannl.GetVideoFormat() );
				cServMsg.SetErrorCode(ERR_MT_MEDIACAPABILITY);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
			else
			{
				// force the mt to adjust its resolution to Cif
				// FIXME: cascade adjusting
				u8 byChnnlType = LOGCHL_VIDEO;
				u8 byNewFormat = VIDEO_FORMAT_CIF;
				CServMsg cMsg;
				cMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
				cMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
				cMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
				SendMsgToMt( tSrcMt.GetMtId(), cMsg.GetEventId(), cMsg );
				
				McsLog( "[StartSwitchTWReq]send videoformat<%d>(%s) change msg to mt<%d>!\n",
				 byNewFormat, GetResStr(byNewFormat), tSrcMt.GetMtId() );

			}
			
        }

        if (!tSrcOrg.IsLocal())
        {		
            OnMMcuSetIn(tSrcOrg, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
            tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);
        }

        // xsl [7/20/2006] 卫星分散会议时需要判断回传通道数
        if (m_tConf.GetConfAttrb().IsSatDCastMode())
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
            
            if (IsOverSatCastChnnlNum(tSrcMt.GetMtId()))
            {
                ConfLog(FALSE, "[StartSwitchTWReq] over max upload mt num. nack!\n");
                cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
        }
	
        switch(tTWSwitchInfo.GetMemberType()) 
        {
        case TW_MEMBERTYPE_MCSSPEC:
		case TW_MEMBERTYPE_VCSAUTOSPEC:

            tSrcMt = m_ptMtTable->GetMt(tSrcMt.GetMtId());

            //源终端必须具备视频发送能力
            m_ptMtTable->GetMtStatus(tSrcMt.GetMtId(), &tMtStatus);
            if (FALSE == tMtStatus.IsSendVideo() &&
				m_tConf.GetConfSource() != VCS_CONF)
            {
                cServMsg.SetErrorCode(ERR_MCU_SRCISRECVONLY);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
                return;
            }	
			
            //源终端必须已经加入会议
            if (!m_tConfAllMtInfo.MtJoinedConf(tSrcMt.GetMtId()) &&
				m_tConf.GetConfSource() != VCS_CONF)
            {
                log(LOGLVL_DEBUG1, "CMcuVcInst: select source MT%u-%u has not joined current conference! Error!\n",
                                    tSrcMt.GetMcuId(), tSrcMt.GetMtId());
                cServMsg.SetErrorCode(ERR_MCU_MT_NOTINCONF);
                SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);	//nack
                return;
            }

            bNeedSwitchMt = TRUE;
            break;

        case TW_MEMBERTYPE_SPEAKER://注意改变发言人时改变交换
			
            if (TRUE == HasJoinedSpeaker())
            {
                bNeedSwitchMt = TRUE;
                tSrcMt = GetLocalSpeaker();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;

        case TW_MEMBERTYPE_CHAIRMAN://注意改变主席时改变交换

            if (TRUE == HasJoinedChairman())
            {
                bNeedSwitchMt = TRUE;
                tSrcMt = m_tConf.GetChairman();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;

        case TW_MEMBERTYPE_POLL://注意改变轮询广播源时改变交换

            if (CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode())
            {
                bNeedSwitchMt = TRUE;
                tSrcMt = (TMt)m_tConf.m_tStatus.GetMtPollParam();
            }
            else
            {
                tSrcMt.SetNull();
            }
            break;

        default:
            ConfLog(FALSE, "CMcuVcInst: Wrong TvWall Member Type!\n");
            break;
		}

        //send Ack reply
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
                
        if (bNeedSwitchMt)
        {
            ptSrcMt = &tSrcMt;
        }    

        ChangeTvWallSwitch(ptSrcMt, byTvWallId, byDstChnnlIdx, tTWSwitchInfo.GetMemberType(), TW_STATE_START);

        break;

    default:
        ConfLog(FALSE, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
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
	TLogicalChannel tLogicalChannel;

	tTWSwitchInfo = *( TTWSwitchInfo * )cServMsg.GetMsgBody();
	tEqp = tTWSwitchInfo.GetDstMt();

	//error, Nack
	if( tEqp.GetEqpType() != EQP_TYPE_TVWALL )
	{
		ConfLog( FALSE, "CMcSsnInst: Wrong PeriEquipment type %u or it has not registered!\n", 
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
    u8             byMtId;
    byTvWallId = tEqp.GetEqpId();
    byChnlNo   = tTWSwitchInfo.GetDstChlIdx();
    g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTWStatus);
    tRemoveMt = tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlNo];
    if (tRemoveMt.GetConfIdx() != m_byConfIdx)
    {
        return;
    }
    byMtId = tRemoveMt.GetMtId();

	StopSwitchToPeriEqp( tEqp.GetEqpId(), tTWSwitchInfo.GetDstChlIdx() );

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
        
    if( IsMtNotInOtherTvWallChnnl(byMtId, byTvWallId, byChnlNo) )  
    {
        m_ptMtTable->SetMtInTvWall(byMtId, FALSE);
        MtStatusChange(); 
		
		// xliang [7/13/2009] recover mt's resolution (take MPU into consideration)
		RecoverMtResInMpu( tRemoveMt );
    }
    
    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlNo].byMemberType = tTWSwitchInfo.GetMemberType();
    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlNo].SetNull();
    g_cMcuVcApp.SetPeriEqpStatus(byTvWallId, &tTWStatus);

    //清除TvWall模版相应成员信息
    TMt tNullMt;
    tNullMt.SetNull();
    u8 byMtPos;
    u8 byMemberType = 0;
    BOOL32 bExist = m_tConfEqpModule.GetTvWallMemberByMt(byTvWallId, tRemoveMt, byMtPos, byMemberType);
    if( bExist )
    {
        m_tConfEqpModule.SetTvWallMemberInTvInfo(byTvWallId, byChnlNo, tNullMt);    // remove tv wall info
        m_tConfEqpModule.m_tMultiTvWallModule.RemoveMtByTvId(byTvWallId, byMtId);  // remove mutilTvWallmodule
    }

	
	
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
    TPeriEqpStatus tTWStatus;
    g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTWStatus);

    //是否在本电视墙的其他通道里
    for(u8 byLp = 0; byLp < tTWStatus.m_tStatus.tTvWall.byChnnlNum; byLp++)
    {
        if(byLp != byChnlId)
        {
            TMt tMtInTv = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byLp];
			// 增加判断终端所在会议号，否则会将其他会议的终端误判
			// zgc, 2008-01-25
            if(tMtInTv.GetMtId() == byMtId && tMtInTv.GetConfIdx() == m_byConfIdx)
            {
                return FALSE;
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
                TMt tMtInTv = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byLp];
				// 增加判断终端所在会议号，否则会将其他会议的终端误判
				// zgc, 2008-01-25
                if(tMtInTv.GetMtId() == byMtId && tMtInTv.GetConfIdx() == m_byConfIdx)
                {
                    return FALSE;
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
        //OspPrintf(TRUE, FALSE, "[ProcTvwallConnectedNotif]CurState() = %d\n", CurState());
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

    tEqp.SetMcuEqp(tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());

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
        bySrcMtConfIdx = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
        if (m_byConfIdx == bySrcMtConfIdx)
        {
            byMemberType = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType;
            if (0 != byMemberType)
            {
                memset(&tTWSwitchInfo, 0, sizeof(TTWSwitchInfo));
                tTWSwitchInfo.SetMemberType(byMemberType);
                tTWSwitchInfo.SetSrcMt((TMt)tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop]);
                tTWSwitchInfo.SetDstMt(tEqp);
                tTWSwitchInfo.SetDstChlIdx(byLoop);

                cServMsg.SetMsgBody((u8 *)&tTWSwitchInfo, sizeof(TTWSwitchInfo));

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
        //OspPrintf(TRUE, FALSE, "[TvwallDisconnectedNotif]CurState() = %d\n", CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqp tEqp = *(TEqp *)cServMsg.GetMsgBody();

    TPeriEqpStatus tTvWallStatus;
    u8 bySrcMtConfIdx;
    u8 bySrcMtId;

    g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tTvWallStatus);
    u8 byMemberNum = tTvWallStatus.m_tStatus.tTvWall.byChnnlNum;
    for (u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
    {
        bySrcMtConfIdx = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
        if (m_byConfIdx == bySrcMtConfIdx)
        {
            bySrcMtId = tTvWallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMtId();
            m_ptMtTable->SetMtInTvWall(bySrcMtId, FALSE);
            MtStatusChange();

            StopSwitchToPeriEqp(tEqp.GetEqpId(), byLoop);
        }
    }
    
    // guzh [5/16/2007] 处理电视墙轮询
    if (POLL_STATE_NONE != m_tConf.m_tStatus.GetTvWallPollState())
    {
        TMtPollParam tMtPollParam;
        memset(&tMtPollParam, 0, sizeof(tMtPollParam));
        
        m_tConf.m_tStatus.SetTvWallPollState(POLL_STATE_NONE);
        m_tConf.m_tStatus.SetTvWallMtPollParam(tMtPollParam);
        cServMsg.SetMsgBody((u8*)m_tConf.m_tStatus.GetTvWallPollInfo(), sizeof(TTvWallPollInfo));
        SendMsgToAllMcs(MCU_MCS_TWPOLLSTATE_NOTIF, cServMsg);

        KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER);
    }

//    for(u8 byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++)
//    {
//        StopSwitchToPeriEqp(tEqp.GetEqpId(), byLoop);
//    }

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
void CMcuVcInst::ProcHduMcuStatusNotif( const CMessage * pcMsg )
{
//     CServMsg cServMsg(pcMsg->content, pcMsg->length);
//     TPeriEqpStatus *ptHduStatus = (TPeriEqpStatus*)cServMsg.GetMsgBody();
//     g_cMcuVcApp.SetPeriEqpStatus(ptHduStatus->GetEqpId(), ptHduStatus);
	
	return;
}

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
//     if (STATE_ONGOING != CurState())
//     {
//         return;
//     }
// 	
//     CServMsg cServMsg(pcMsg->content, pcMsg->length);
// 	
//     TPeriEqpRegReq tRegReq = *(TPeriEqpRegReq *)cServMsg.GetMsgBody();
// 	
//     TPeriEqpStatus tHduStatus;
// 	
//     THduSwitchInfo tHduSwitchInfo;
//     u8 byMemberType;
//     u8 bySrcMtConfIdx;
//     u8 byLoop;
//     TEqp tEqp;
// 	
//     tEqp.SetMcuEqp(tRegReq.GetMcuId(), tRegReq.GetEqpId(), tRegReq.GetEqpType());
// 	
//     g_cMcuVcApp.GetPeriEqpStatus(tRegReq.GetEqpId(), &tHduStatus);
// 
//     for (byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; byLoop++)
//     {
//         bySrcMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
//         if (m_byConfIdx == bySrcMtConfIdx)
//         {
// 		    byMemberType = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType;
//             if (0 != byMemberType)
//             {
//                 tHduSwitchInfo.SetMemberType(byMemberType);
//                 tHduSwitchInfo.SetSrcMt((TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop]);
//                 tHduSwitchInfo.SetDstMt(tEqp);
//                 tHduSwitchInfo.SetDstChlIdx(byLoop);
// 
// 				::OspPost(MAKEIID(AID_MCU_VC, GetInsID()), MCUVC_STARTSWITCHHDU_NOTIFY, 
// 					         (u8*)&tHduSwitchInfo, sizeof(tHduSwitchInfo));
//             }
//         }
//     }
// 	
//     return;
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

    TPeriEqpStatus tHduStatus;
    u8 bySrcMtConfIdx;
    u8 bySrcMtId;
	
    g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tHduStatus);
    u8 byMemberNum = tHduStatus.m_tStatus.tHdu.byChnnlNum;
    for (u8 byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; byLoop++)
    {
        bySrcMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
        if (m_byConfIdx == bySrcMtConfIdx)
        {
            bySrcMtId = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetMtId();
            m_ptMtTable->SetMtInHdu(bySrcMtId, FALSE);
            MtStatusChange();
			
            StopSwitchToPeriEqp(tEqp.GetEqpId(), byLoop);
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
		u8 byMtType = m_ptMtTable->GetMtType(byCreateMtId);
		if( MT_TYPE_MMCU == byMtType || MT_TYPE_SMCU == byMtType )
		{
			g_cMcuVcApp.DecMtAdpMtNum( m_ptMtTable->GetDriId(byCreateMtId), m_byConfIdx, byCreateMtId, FALSE,TRUE );
		}
    }

	// 释放Prs通道
	if( EQP_CHANNO_INVALID != m_byPrsChnnl )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnl = EQP_CHANNO_INVALID;
	}

	if( EQP_CHANNO_INVALID != m_byPrsChnnl2 )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl2].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnl2 = EQP_CHANNO_INVALID;
	}
		
	if( EQP_CHANNO_INVALID != m_byPrsChnnlAud )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAud].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlAud = EQP_CHANNO_INVALID;
	}
	
	if( EQP_CHANNO_INVALID != m_byPrsChnnlVidBas )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVidBas].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlVidBas = EQP_CHANNO_INVALID;
	}

	if( EQP_CHANNO_INVALID != m_byPrsChnnlBrBas )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlBrBas].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlBrBas = EQP_CHANNO_INVALID;
	}
	
	if( EQP_CHANNO_INVALID != m_byPrsChnnlAudBas )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAudBas].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlAudBas = EQP_CHANNO_INVALID;
	}

	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut1 )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut1].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlVmpOut1 = EQP_CHANNO_INVALID;
	}

	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut2 )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut2].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlVmpOut2 = EQP_CHANNO_INVALID;
	}

	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut3 )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut3].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlVmpOut3 = EQP_CHANNO_INVALID;
	}

	if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut4 )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus);
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut4].SetReserved(FALSE);                        
        tStatus.SetConfIdx(m_byConfIdx);                        
        g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
        m_byPrsChnnlVmpOut4 = EQP_CHANNO_INVALID;
	}

	// 释放Bas通道
	if( EQP_CHANNO_INVALID != m_byAudBasChnnl )
	{				
        g_cMcuVcApp.SetBasChanReserved(m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl, FALSE);
        m_byAudBasChnnl = EQP_CHANNO_INVALID;
		memset( &m_tAudBasEqp, 0, sizeof(m_tAudBasEqp) );
	}

	if( EQP_CHANNO_INVALID != m_byVidBasChnnl )
	{
        g_cMcuVcApp.SetBasChanReserved(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, FALSE);	
		m_byVidBasChnnl = EQP_CHANNO_INVALID;
		memset( &m_tVidBasEqp, 0, sizeof(m_tVidBasEqp) );
	}

	if( EQP_CHANNO_INVALID != m_byBrBasChnnl )
	{
        g_cMcuVcApp.SetBasChanReserved(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, FALSE);			
        m_byBrBasChnnl = EQP_CHANNO_INVALID;
		memset( &m_tBrBasEqp, 0, sizeof(m_tBrBasEqp) );
	}

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
	
    CreateConfSendMcsNack( byOldInsId, nErrCode, cServMsg, TRUE );

    return;
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
    if ( bDeleteAlias )
    {
        DeleteAlias();
    }    
    cServMsg.SetErrorCode( nErrCode );
    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
    g_cMcuVcApp.SetConfMapInsId(m_byConfIdx, byOldInsId);

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
        ConfLog(FALSE, "[AddSecDSCapByDebug] add dual ds cap<h263+, XGA, 5fps>\n");
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
        McsLog("Time to Start Auto Record...\n");
        
        // 如果已经在录像，则不理睬本消息。
        if ( m_tConf.m_tStatus.IsNoRecording() )
        {    
            // 如果录像机在线
            if ( g_cMcuVcApp.IsRecorderOnline(tAttrib.GetRecEqp()) )
            {
                // 创建一条开始录像消息
                CServMsg cServMsg;
                TMt tMt;
                tMt.SetNull();
                cServMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));
                TEqp tRec;
                tRec.SetMcuEqp( LOCAL_MCUID, tAttrib.GetRecEqp(), EQP_TYPE_RECORDER );
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
                    u16 wBitrate = m_tConf.GetBitRate() + GetAudioBitrate( m_tConf.GetMainAudioMediaType() );
                    sprintf(aszRecName, "%s-%04d-%02d-%02d %02d-%02d-%02d_%d_%dK",
                            m_tConf.GetConfName(),
                            tKdvTime.GetYear(),
                            tKdvTime.GetMonth(),
                            tKdvTime.GetDay(), 
                            tKdvTime.GetHour(),
                            tKdvTime.GetMinute(),
                            tKdvTime.GetSecond(),
                            m_tConf.GetUsrGrpId(),
                            wBitrate );
//                }
//                else
//                {
//                    strncpy(aszRecName, tAttrib.GetRecName(), KDV_MAX_PATH - 1);
//                }
                cServMsg.CatMsgBody((u8*)aszRecName, strlen(aszRecName)+1);

                cServMsg.SetEventId(MCS_MCU_STARTREC_REQ);
                cServMsg.SetConfIdx(m_byConfIdx);
                cServMsg.SetConfId( m_tConf.GetConfId() );
                cServMsg.SetNoSrc();
    
                g_cMcuVcApp.SendMsgToConf(m_byConfIdx, 
                                          MCS_MCU_STARTREC_REQ, 
                                          cServMsg.GetServMsg(),
                                          cServMsg.GetServMsgLen());
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
        McsLog("Time to End Auto Record...\n");

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
	u8 byLoop = 0;

	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byConfIdx = *(u8*)cServMsg.GetMsgBody();
	TTemplateInfo  tTemInfo;

	if (g_cMcuVcApp.GetConfMapData(byConfIdx).IsValidConf())
	{
		//get tConfInfo
		if (NULL == g_cMcuVcApp.GetConfInstHandle(byConfIdx))
		{
			ConfLog( FALSE, "[ProcMcsMcuSaveConfToTemplateReq]: GetConfInstHandle NULL! -- %d\n", m_byConfIdx);
			return;
		}
		CMcuVcInst* pcInstance = g_cMcuVcApp.GetConfInstHandle(byConfIdx);

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
        tTemInfo.m_tConfInfo.m_tStatus.SetTvWallPollState(POLL_STATE_NONE);
		tTemInfo.m_tConfInfo.m_tStatus.SetTakeMode(CONF_TAKEMODE_TEMPLATE);

//		tTemInfo.m_tMultiTvWallModule = pcInstance->m_tConfEqpModule.m_tMultiTvWallModule; 
//		tTemInfo.m_atVmpModule = pcInstance->m_tConfEqpModule.GetVmpModule();
		tTemInfo.m_tMultiTvWallModule.Clear();
		tTemInfo.m_atVmpModule.EmptyMember();
		TConfAttrb tConfAttrib = tTemInfo.m_tConfInfo.GetConfAttrb();
		tConfAttrib.SetHasTvWallModule( FALSE );
		tConfAttrib.SetHasVmpModule( FALSE );
		tTemInfo.m_tConfInfo.SetConfAttrb( tConfAttrib );
		tTemInfo.m_byMtNum = pcInstance->m_ptMtTable->m_byMaxNumInUse;
            
        tTemInfo.EmptyAllTvMember();
        tTemInfo.EmptyAllVmpMember();

		//get alias array
        u8 byMemberType = 0;
		TMt tMt;
		TMtAlias tMtAlias;
		for (u8 byLoop = 0; byLoop < pcInstance->m_ptMtTable->m_byMaxNumInUse; byLoop++)
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
				if (tTemInfo.m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
				{
					for(u8 byTvLp = 0; byTvLp < MAXNUM_PERIEQP_CHNNL; byTvLp++)
                    {                            
                        u8 byTvId = pcInstance->m_tConfEqpModule.m_tTvWallInfo[byTvLp].m_tTvWallEqp.GetEqpId();
                        for(u8 byTvChlLp = 0; byTvChlLp < MAXNUM_PERIEQP_CHNNL; byTvChlLp++)
                        {
                            if(TRUE ==pcInstance-> m_tConfEqpModule.IsMtInTvWallChannel(byTvId, byTvChlLp, tMt, byMemberType) )
                            {
                                tTemInfo.SetMtInTvChannel(byTvId, byTvChlLp, byLoop+1, byMemberType);
                            }
                        }
                    }
				}
				if (tTemInfo.m_tConfInfo.GetConfAttrb().IsHasVmpModule())
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
            ConfLog( FALSE, "[ProcMcsMcuSaveConfToTemplateReq]Modify template %s failed!\n", tTemInfo.m_tConfInfo.GetConfName() );
            return;
		}
		
		SendReplyBack(cServMsg, pcMsg->event+1);
        g_cMcuVcApp.TemInfo2Msg(tTemInfo, cServMsg);
        SendMsgToAllMcs(MCU_MCS_TEMSCHCONFINFO_NOTIF, cServMsg); 
	}
	else
	{
		//会议的E164号码已存在，拒绝 
		if( g_cMcuVcApp.IsConfE164Repeat( tTemInfo.m_tConfInfo.GetConfE164(), TRUE ) )
        {
            cServMsg.SetErrorCode( ERR_MCU_CONFE164_REPEAT );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            ConfLog( FALSE, "[ProcMcsMcuSaveConfToTemplateReq]template %s E164 repeated and create failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
            return;
        }
		//会议名已存在，拒绝
        if( g_cMcuVcApp.IsConfNameRepeat( tTemInfo.m_tConfInfo.GetConfName(), TRUE ) )
        {
            cServMsg.SetErrorCode( ERR_MCU_CONFNAME_REPEAT );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            ConfLog( FALSE, "[ProcMcsMcuSaveConfToTemplateReq]template %s name repeated and create failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
            return;
        } 
	
		if(!g_cMcuVcApp.AddTemplate(tTemInfo))
        {
			// 返回的错误码
			cServMsg.SetErrorCode( ERR_MCU_TEMPLATE_NOFREEROOM );
			ConfLog(FALSE, "[ProcMcsMcuSaveConfToTemplateReq] add template %s failed\n", tTemInfo.m_tConfInfo.GetConfName());
			SendReplyBack(cServMsg, pcMsg->event+2);
			return;
         }                 
         cServMsg.SetConfId( tTemInfo.m_tConfInfo.GetConfId() );
		 SendReplyBack(cServMsg, pcMsg->event+1);
         g_cMcuVcApp.TemInfo2Msg(tTemInfo, cServMsg);
         SendMsgToAllMcs(MCU_MCS_TEMSCHCONFINFO_NOTIF, cServMsg); 
	}
	
	McsLog( "Save conf %s to template successfully!\n", tTemInfo.m_tConfInfo.GetConfName() );
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
	
	u8 bySeizeMtId = tSeizeMt.GetMtId();			//FIXME:级联抢占

	TVMPParam  tVMPParam   = m_tConf.m_tStatus.GetVmpParam();
	
	if (MCS_MCU_VMPPRISEIZE_ACK == pcMsg->event)
	{
		u8 bySeizedMtNum = (cServMsg.GetMsgBodyLen()-sizeof(TMt)) / sizeof(TSeizeChoice);//被抢占的MT数目
		TSeizeChoice tSeizeChoice[MAXNUM_SVMPB_HDCHNNL];
		u8 byMtIdx=0;
		CallLog("Mt.%u Seize Mt choice info:\n==================\n",bySeizeMtId);
		for(byMtIdx=0; byMtIdx<bySeizedMtNum; byMtIdx++)
		{
			tSeizeChoice[byMtIdx] = *(TSeizeChoice *)(cServMsg.GetMsgBody()+sizeof(TMt)+sizeof(TSeizeChoice)*byMtIdx);
			CallLog("Mt.%u: byStopVmp(%u), byStopSpeaker(%u), byStopSelected(%u)\n",
				tSeizeChoice[byMtIdx].tSeizedMt.GetMtId(), tSeizeChoice[byMtIdx].byStopVmp,
				tSeizeChoice[byMtIdx].byStopSpeaker, tSeizeChoice[byMtIdx].byStopSelected);
		}

		CServMsg cMsg;
		
		for(byMtIdx=0; byMtIdx<bySeizedMtNum; byMtIdx++)
		{
			//根据被抢占选项进行操作
			//1,该终端退出画面合成
			if(tSeizeChoice[byMtIdx].byStopVmp == 1)
			{
				// xliang [3/25/2009] 策略更改，局部成员调整，不进行全局重整
				TMt tMtBeSeized = tSeizeChoice[byMtIdx].tSeizedMt;
				u8	byChl = tVMPParam.GetChlOfMtInMember(tMtBeSeized);
				TMt tMt;
				TVMPMember tVMPMember;
				tMt.SetNull();
				tVMPMember.SetMemberTMt(tMt);
				tVMPMember.SetMemberType(0); //不管原先是发言人跟随还是其他，均清0
				tVMPParam.SetVmpMember(byChl,tVMPMember);
				m_tConf.m_tStatus.SetVmpParam(tVMPParam);
				memcpy(&m_tLastVmpParam, &tVMPParam, sizeof(TVMPParam));
				
				cServMsg.SetMsgBody((u8*)&tVMPParam,sizeof(tVMPParam));
				SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );	//告诉会控刷VMP界面

				// xliang [4/10/2009] 同时更新tPeriEqpStatus，mcs要依据此刷MT图标,而不是依据TMtStatus :(
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
				g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				cServMsg.SetMsgBody();
				cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );

				//清交换：
				StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byChl, FALSE, MODE_VIDEO);
				m_tVmpChnnlInfo.ClearOneChnnl(tMtBeSeized); //清该Mt对应的HD通道(如果存在对应通道)
				
				/*bNeedSeize = FALSE;
				
				TVMPMember tVMPMember;
				
				u8         byMemberNum = tVMPParam.GetMaxMemberNum();
				TMt tMtBeSeized = m_ptMtTable->GetMt(tSeizeChoice[byMtIdx].byMtId);
				
				m_tVmpChnnlInfo.ClearOneChnnl(tMtBeSeized.GetMtId()); //清该Mt对应的HD通道(如果存在对应通道)

				if(IsDynamicVmp()) //当前是自动画面合成模式,这里不能判断vmpStyle，要判断vmpMode
				{
					// xliang [12/17/2008] 自动画面合成时，vmpParam是没有成员信息的，
					// 所以没必要设tVMPMember，也没法赋值
					// 这里把该终端ID加入到VMP成员忽略列表，这样就不会再考虑该终端了
					AddToVmpNeglected(tSeizeChoice[byMtIdx].byMtId);
				}
				else
				{
					u8	byChl = tVMPParam.GetChlOfMtInMember(tMtBeSeized);
					TMt tMt;
					tMt.SetNull();
					tVMPMember.SetMemberTMt(tMt);
					tVMPMember.SetMemberType(0); //不管原先是发言人跟随还是其他，均清0
					tVMPParam.SetVmpMember(byChl,tVMPMember);
					//tVMPParam.Print();
				}

				cMsg.SetNoSrc();
				cMsg.SetEventId(MCS_MCU_CHANGEVMPPARAM_REQ);
				cMsg.SetMsgBody((u8 *)&tVMPParam,sizeof(tVMPParam));
				ProcChangeVmpParamReq(cMsg);
				CallLog("[ProcMcsMcuVmpPriSeizeRsp] Start Change Vmp Param!\n");

				*/
			}
			//2,该终端取消发言人
			if(tSeizeChoice[byMtIdx].byStopSpeaker == 1)
			{
				BOOL32 bCancelSpeaker = TRUE; //能否取消发言人

				//处于演讲模式的语音激励控制发言状态,不能指定发言人
				if (m_tConf.m_tStatus.IsVACing())
				{
					ConfLog( FALSE,"[ProcMcsMcuVmpPriSeizeRsp] Conf is VACing! Speaker cannot be canceled!\n");
					bCancelSpeaker = FALSE;
				}

				TMt tSpeaker = m_tConf.GetSpeaker();
				if( !m_tConf.HasSpeaker() )//没有发言人
				{
					ConfLog( FALSE, "No speaker in conference %s now! Cannot cancel the speaker!\n", 
						m_tConf.GetConfName() );
					bCancelSpeaker = FALSE;
				}
				if(bCancelSpeaker)
				{
					//停组播
					if( m_ptMtTable->IsMtMulticasting( tSpeaker.GetMtId() ) )
					{
						g_cMpManager.StopMulticast( tSpeaker, 0, MODE_BOTH );
						m_ptMtTable->SetMtMulticasting( tSpeaker.GetMtId(), FALSE );
					}
					
					//取消发言人
					CallLog("[ProcMcsMcuVmpPriSeizeRsp]begin change speaker!\n");
					ChangeSpeaker( NULL );
				}

// 				cMsg.SetNoSrc();
// 				cMsg.SetEventId(MCS_MCU_CANCELSPEAKER_REQ);	//取消发言人
// 				cMsg.SetMsgBody();
// 				g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(),cMsg.GetEventId(),
// 							cMsg.GetServMsg(),cMsg.GetServMsgLen());
// 
// 				CallLog("[ProcMcsMcuVmpPriSeizeRsp] Send msg MCS_MCU_CANCELSPEAKER_REQ to conf.\
// 					The seized Mt.%u!\n",tSeizeChoice[byMtIdx].byMtId);

			}

			//3,该终端取消被选看
			if(tSeizeChoice[byMtIdx].byStopSelected == 1)
			{
				TMtStatus	tMtStatus;
				TMt			tSrc;
				TMt			tDstMt;
				BOOL32		bSelectedSrc = FALSE; //是否正在被选看
				//取消所有正在选看该选看源MT的交换
				for(u8 byLoop=1; byLoop<=MAXNUM_CONF_MT;byLoop++ )
				{
					if(!m_tConfAllMtInfo.MtJoinedConf( byLoop ))
					{
						continue;
					}
					m_ptMtTable->GetMtStatus(byLoop,&tMtStatus);
					//tSrc = tMtStatus.GetVideoMt();// xliang [1/9/2009] 虽然语义对，但是最初置选看源，用的是SetSelectMt，所以用下句
					tSrc = tMtStatus.GetSelectMt( MODE_VIDEO ); //获取选看源
					if(tSrc == tSeizeChoice[byMtIdx].tSeizedMt)//该MT的选看源是被抢占的MT
					{
						//取消选看
						bSelectedSrc = TRUE;
						tDstMt = m_ptMtTable->GetMt(byLoop);
// 						cMsg.SetNoSrc();
// 						cMsg.SetEventId(MCS_MCU_STOPSWITCHMT_REQ);
// 						cMsg.SetMsgBody((u8 *)&tDstMt,sizeof(tDstMt));
// 						g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(),cMsg.GetEventId(),
// 							cMsg.GetServMsg(),cMsg.GetServMsgLen());
// 						
// 						CallLog("[ProcMcsMcuVmpPriSeizeRsp] Send msg MCS_MCU_STOPSWITCHMT_REQ to conf.\
// 							Cancel Mt.%u selectseeing Mt.%u.\n",byLoop,tSeizeChoice[byMtIdx].byMtId);
						CallLog("[ProcMcsMcuVmpPriSeizeRsp]Cancel Mt.%u selectseeing Mt.%u.\n",
							byLoop,tSeizeChoice[byMtIdx].tSeizedMt.GetMtId());
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
					CallLog("[ProcMcsMcuVmpPriSeizeRsp]Mt.%u is not the selected source!\n",tSeizeChoice[byMtIdx].tSeizedMt.GetMtId());
				}
			}
		}
		if(bNeedSeize)
		{
			//对原先要抢占的Mt进行操作，设个定时，防时序问题
			u16 wTimer = 1000;		//FIXME：1秒可能太长
			//（针对新加一个成员，引起抢占的情况）
			//1,vmp param整理: 将之前未加入合成成员的MT加入进来
			//2,补回change param的ACK给MCS
			TMt tTmpTmt = m_ptMtTable->GetMt(bySeizeMtId);

			if ( !tVMPParam.IsMtInMember(tTmpTmt) ) 
			{	
				u8 byIndex = 0;
				for(;byIndex < MAXNUM_MPUSVMP_MEMBER; byIndex++)
				{
					if(m_atVMPTmpMember[byIndex].GetMtId() == bySeizeMtId)
					{
						break;
					}
				}
				TVMPMember tVMPMember = m_atVMPTmpMember[byIndex];
				//tVMPMember.SetMemberTMt(tTmpTmt);
				//tVMPMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);	//FIXME: membertype 填死有问题，轮询跟随怎么办
				tVMPParam.SetVmpMember(byIndex,tVMPMember);
				m_tConf.m_tStatus.SetVmpParam(tVMPParam);
				memcpy(&m_tLastVmpParam, &tVMPParam, sizeof(TVMPParam));

				cServMsg.SetMsgBody((u8*)&tVMPParam,sizeof(tVMPParam));
				
				m_bNewMemberSeize = 1;	//标记此次抢占是新增了一个成员触发的抢占，之后要补建交换
				SendReplyBack(cServMsg, MCU_MCS_CHANGEVMPPARAM_ACK);	//补回ack

				SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );	//告诉会控刷界面

				// xliang [4/10/2009] 同时更新tPeriEqpStatus，mcs要依据此刷MT图标,而不是依据TMtStatus :(
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
				g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				cServMsg.SetMsgBody();
				cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
				SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );
			}
			
			SetTimer(MCUVC_MTSEIZEVMP_TIMER, wTimer, (u32)bySeizeMtId*100);
		}
		else
		{
			ConfLog(FALSE,"[ProcMcsMcuVmpPriSeizeRsp] no need to seize!\n");
		}
	}
	else // xliang [4/21/2009] 目前下面这段不执行
	{	
		//TMt tTmpTmt = m_ptMtTable->GetMt(bySeizeMtId);
		if ( tVMPParam.IsMtInMember(tSeizeMt) ) 
		{
			//将该vip身份的MT从画面合成成员中剔除
			u8	byChl = tVMPParam.GetChlOfMtInMember(tSeizeMt);
			TMt tMt;
			TVMPMember tVMPMember;
			tMt.SetNull();
			tVMPMember.SetMemberTMt(tMt);
			tVMPMember.SetMemberType(0); //不管原先是发言人跟随还是其他，均清0
			tVMPParam.SetVmpMember(byChl,tVMPMember);
			//tVMPParam.Print();
			CServMsg cMsg;
			cMsg.SetNoSrc();
			cMsg.SetEventId(MCS_MCU_CHANGEVMPPARAM_REQ);
			cMsg.SetMsgBody((u8 *)&tVMPParam,sizeof(tVMPParam));
			ProcChangeVmpParamReq(cMsg);

		}

		// xliang [12/17/2008] 把起先要抢占的终端ID加入到VMP成员忽略列表，
		//这样自动画面合成模式就不会再考虑该终端了
		AddToVmpNeglected(bySeizeMtId);
		ConfLog(FALSE,"receive MCS_MCU_VMPPRISEIZE_NACK!\n");
	}
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
	u8 bySeizeMtId  = (u8)((*((u32*)pcMsg->content))/100);		//之前要抢占的终端id
	u8 bySeizeTimes  = (u8)((*((u32*)pcMsg->content))%100)+1;	//抢占次数

	BOOL32 bNeedSeizePromt = FALSE;	//是否需要重新发抢占提示
	if(bySeizeTimes > 3)
	{
		bNeedSeizePromt = TRUE;
	}
	//获取之前要抢占的终端vmpMemberType
	TVMPMember tVMPMember;
	TVMPParam  tVMPParam   = m_tConf.m_tStatus.GetVmpParam();
	u8         byMemberNum = tVMPParam.GetMaxMemberNum();
	u8	       byMemberType = ~0;
	for (u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
	{
		
		//取画面合成成员
		tVMPMember = *(tVMPParam.GetVmpMember(byLoop));
		if( tVMPMember.GetMtId() == bySeizeMtId)
		{
			byMemberType = tVMPMember.GetMemberType();
			break;
		}
	}

	if(byMemberType == 0) //如果在超时到来之前，整个画面合成被停止了，则membertype应为0
	{
		ConfLog(FALSE,"vmp has been stopped,no need to change Mt.%u's format!\n",bySeizeMtId);
		return;
	}

	//取VMPstyle, channel idx
	u8 byVmpStyle = tVMPParam.GetVMPStyle();
	TMt tSeizeMt = m_ptMtTable->GetMt(bySeizeMtId);
	u8 byChlPos	= tVMPParam.GetChlOfMtInMember(tSeizeMt);

	//调整分辨率
	BOOL32 bRet = ChangeMtVideoFormat(tSeizeMt, &tVMPParam, TRUE, bNeedSeizePromt, FALSE);
	
	if(!m_bNewMemberSeize)//之前已经在VMP中，且建了交换.(针对某成员升为发言人等特殊身份导致抢占的情况)
	{
		//do nothing
	}
	else 
	{
		if(bRet)
		{
			//设置通道，建交换
			SetVmpChnnl((TMt)tVMPMember, byChlPos, byMemberType);
			CallLog("[ProcMtSeizeVmpTimer] Mt.%u Set Vmp channel.%u!\n",bySeizeMtId, byChlPos);
		}
		else if(!bNeedSeizePromt) //如果需要再次发抢占提示，这里就不需要定时了
		{	
			//继续定时
			SetTimer(MCUVC_MTSEIZEVMP_TIMER, 1000, (u32)bySeizeMtId*100+bySeizeTimes);	       
		}
	}
	
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
	TVMPParam	tVmpParam = *( TVMPParam* )cServMsg.GetMsgBody();
	TPeriEqpStatus tPeriEqpStatus;
	u8 byVmpSubType = 0;			//VMP子类型
	u8 byLoop		= 0;			
	u8 byVmpId		= 0;

	if(g_bPrintEqpMsg)
	{
		EqpLog("[VmpCommonReq] start vmp param:\n");
		tVmpParam.Print();
	}
	
	//检查当前会议的画面合成状态
	if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE ||
		//避免正等待画面合成器响应时界面再次发起开启画面合成请求,导致占用新画面合成器
		//而原合成器资源泄漏
		(!m_tVmpEqp.IsNull() &&
		g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tPeriEqpStatus) &&
		tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
		TVmpStatus::WAIT_START == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState))
	{
		cServMsg.SetErrorCode( ERR_MCU_VMPRESTART );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );					
		return;
	}
	
	//合成参数校验
	u16 wErrCode = 0;
	if ( !CheckVmpParam( tVmpParam, wErrCode ) )
	{
		cServMsg.SetErrorCode( wErrCode );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//选VMP
	u8 byIdleVMPNum = 0;
    u8 abyIdleVMPId[MAXNUM_PERIEQP];
    memset( &abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
    g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );
	if( byIdleVMPNum == 0 )
	{
		cServMsg.SetErrorCode( ERR_MCU_NOIDLEVMP );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		ConfLog(FALSE,"[VmpCommonReq]No Idle VMP!\n"); 
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
	}
	
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
	byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
	u8 byVmpDynStyle;
	if (tVmpParam.IsVMPAuto() )
	{
		//自动VMP过滤
		// 提前赋值,目前仅为函数GetVmpDynStyle中使用
		m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
		m_tVmpEqp.SetConfIdx( m_byConfIdx );
		byVmpDynStyle = GetVmpDynStyle(m_tConfAllMtInfo.GetLocalJoinedMtNum());
		m_tVmpEqp.SetNull();
		if (VMP_STYLE_NONE == byVmpDynStyle)
		{
			cServMsg.SetErrorCode( ERR_INVALID_VMPSTYLE );						
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
		else
		{
			// 保存成自动算出的具体合成方式	
			McsLog("auto vmp style is confirmed to style:%u\n", byVmpDynStyle); 
			tVmpParam.SetVMPStyle( byVmpDynStyle );
		}
	}
	else if (byVmpSubType != VMP)
	{
		// 仅对于MPU的通道成员限制（非自动模式下）
		if ( !CheckMpuMember( tVmpParam, wErrCode ) )
		{
			cServMsg.SetErrorCode( wErrCode );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
	}
	
	// xsl [7/20/2006]卫星分散会议需要检查回传通道数
	if (m_tConf.GetConfAttrb().IsSatDCastMode()) 
	{
		if (IsOverSatCastChnnlNum(tVmpParam))
		{
			ConfLog(FALSE, "[VmpCommonReq-start] over max upload mt num. nack!\n");
			cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return; 
		}
	}
	
	m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	m_tVmpEqp.SetConfIdx( m_byConfIdx );
	tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_START;//先占用,超时后未成功再放弃
	tPeriEqpStatus.SetConfIdx( m_byConfIdx );
	
	m_tConf.m_tStatus.SetVmpStyle( tVmpParam.GetVMPStyle() );
	
	//ACK
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );				
	
	/**************************以下为不回nack的过滤***********************/
	//对于有高清前适配通道的VMP，
	//在开始之前过滤vmp param是否有超出VMP前适配通道能力,超出的踢出成员，但整体不拒（自动画面合成除外）
	//VIP身份中，发言人优先级最高。
	CheckAdpChnnlLmt(byVmpId, tVmpParam, cServMsg);
	
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpParam;	//tVmpParam最终给PeriEqpStatus
	g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );

	m_byVmpOperating = 1;	//标志进入VMP原子操作
	
	//开始画面合成
	AdjustVmpParam(&tVmpParam, TRUE);
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
	u8 byVmpDynStle;
	u8 byLoop = 0;
    TVMPParam tVmpParam;
    memset( &tVmpParam, 0, sizeof(TVMPParam) );
	TPeriEqpStatus tPeriEqpStatus;
	/*TMt tMt;*/
	u8 byVmpSubType;
	u8 byMpuBoardVer = ~0;
	u16 wError = 0;
	tVmpParam = *( TVMPParam* )cServMsg.GetMsgBody();
	
	if(g_bPrintEqpMsg)
	{
		EqpLog("[VmpCommonReq] %s change vmp param:\n", OspEventDesc(cServMsg.GetEventId()));
		tVmpParam.Print();
	}
	
	//检查当前会议的画面合成状态 
	if( m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_NONE )
	{
		cServMsg.SetErrorCode( ERR_MCU_VMPNOTSTART );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//自动画面合成能力限制
	byVmpDynStle = GetVmpDynStyle( m_tConfAllMtInfo.GetLocalJoinedMtNum() );
	if ((tVmpParam.GetVMPStyle() == VMP_STYLE_DYNAMIC) && tVmpParam.IsVMPAuto() &&
		VMP_STYLE_NONE == byVmpDynStle)
	{
		cServMsg.SetErrorCode( ERR_INVALID_VMPSTYLE );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	// 顾振华 [5/22/2006] change的时候也要检查是否支持
	if ( !IsVMPSupportVmpStyle(tVmpParam.GetVMPStyle(),m_tVmpEqp.GetEqpId(), wError) )
	{
		cServMsg.SetErrorCode( wError  );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		
		// 切换失败，增加提醒 [12/27/2006-zbq]
		// FIXME: 该提醒太过明朗，有待修改或删去，暂时保留
		u8 byVmpId = 0;
		if ( IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError) )
		{
			if ( cServMsg.GetEventId() == MCS_MCU_CHANGEVMPPARAM_REQ ) 
			{
				NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_OTHERVMP_SUPPORT );
			}
			ConfLog( FALSE, "[VmpCommonReq] curr VMP.%d can't support the style needed, other VMP.%d could\n",
				m_tVmpEqp.GetEqpId(), byVmpId );
		}
		return;
	}    
	
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
	byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType; 	
	
	//设置画面合成参数
	if ( !CheckVmpParam( tVmpParam, wError ) )
	{
		cServMsg.SetErrorCode( wError );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
	}
	
	// xliang [3/5/2009]  会议中不允许同一个MT占了多个成员通道
	// xliang [3/24/2009] 会议中不允许多个通道同时设置成为发言人跟随通道或者轮询跟随通道
	//BOOL bConfHD = CMcuPfmLmt::IsConfFormatHD(m_tConf);
	/*if(bConfHD)*/
	if (byVmpSubType != VMP)
	{
		if (!CheckMpuMember(tVmpParam, wError, TRUE))
		{
			cServMsg.SetErrorCode( wError );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
	}

	// xsl [7/20/2006]卫星分散会议需要检查回传通道数
	if (m_tConf.GetConfAttrb().IsSatDCastMode()) 
	{                
		if (IsOverSatCastChnnlNum(tVmpParam))
		{
			ConfLog(FALSE, "[VmpCommonReq-change] over max upload mt num. nack!\n");
			cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
	}
	
	if ((tVmpParam.GetVMPStyle() == VMP_STYLE_DYNAMIC) && tVmpParam.IsVMPAuto())
	{                
		tVmpParam.SetVMPStyle( byVmpDynStle );
	}

	//抢占提示过滤
	CheckAdpChnnlLmt(m_tVmpEqp.GetEqpId(), tVmpParam, cServMsg, TRUE);

	//ACK
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
	m_byVmpOperating = 1;
	
	// xliang [3/20/2009] 发言人跟随新策略
	if(m_tLastVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) 
		&& !tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) 
		)//某通道取消了发言人跟随
	{
		m_tLastVmpChnnlMemInfo.Init();
	}
	else if(!m_tLastVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) 
		&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) 
		)//某通道变成了发言人跟随
	{
		if( m_tConf.HasSpeaker()
			&& tVmpParam.IsMtInMember( GetLocalSpeaker()/*m_tConf.GetSpeaker()*/ )  
			)
		{
			TMt tSpeaker = m_tConf.GetSpeaker();
			//ChangeVmpChannelParam( &tSpeaker, VMP_MEMBERTYPE_SPEAKER );
			AdjustVmpParambyMemberType(&tSpeaker, tVmpParam);
		}
		else
		{
			//Do nothing
		}
	}
	else //无发言人跟随相关操作
	{
		//Do nothing
	}
	
	// xliang [4/10/2009] 新增轮询跟随 参数过滤
	if(!m_tLastVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL) 
		&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
		)
	{
		//当前会议轮询的MT已在画面合成中，则member填空，保留member type
		TMt tPollMt = m_tConf.m_tStatus.GetMtPollParam().GetTMt();
		if(tVmpParam.IsMtInMember(tPollMt))
		{
			u8 byChnnl = tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_POLL);
			TVMPMember tVmpMember = *tVmpParam.GetVmpMember( byChnnl );
			TMt tNullMt;
			tNullMt.SetNull();
			tVmpMember.SetMemberTMt(tNullMt);
			tVmpParam.SetVmpMember( byChnnl, tVmpMember );
		}

	}
	
	// 保存参数
	// xliang [4/14/2009] tVmpParam并不全,这里务必补充主席选看和批量轮询状态字段
	tVmpParam.SetVMPSeeByChairman( m_tConf.m_tStatus.GetVmpParam().IsVMPSeeByChairman() );
	tVmpParam.SetVMPBatchPoll( m_tConf.m_tStatus.GetVmpParam().IsVMPBatchPoll() );
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = tVmpParam;	
	g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );

	// xliang [1/6/2009] 区分新老VMP
	AdjustVmpParam(&tVmpParam);

	return;
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
void CMcuVcInst::ProcVmpBatchPollTimer(const CMessage *pcMsg)
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
				
				CallLog("[ProcVmpBatchPollTimer]Batch Poll times is full, stop VMP!\n");
				
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
				CallLog("[ProcVmpBatchPollTimer]invalid VMP channel!\n");
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

	CallLog("[ProcVmpBatchPollTimer]VMP Batch Poll: CurPolledMtNum is %u,MtId is %u,MemberId is %u,Left Poll times is%u\n",\
		byCurPolledMtNum, byMtId, byMemberId, dwPollTimes);
	
	cMsg.SetEventId(MCS_MCU_CHANGEVMPPARAM_REQ);
	cMsg.SetMsgBody((u8 *)&tVMPParam,sizeof(tVMPParam));
	g_cMcuVcApp.SendMsgToConf(cMsg.GetConfId(),cMsg.GetEventId(),
		cMsg.GetServMsg(),cMsg.GetServMsgLen());
	
	CallLog("[ProcVmpBatchPollTimer] Send msg MCS_MCU_CHANGEVMPPARAM_REQ to conf.\n");

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
			for ( byMemberId; byMemberId < tVMPParam.GetMaxMemberNum(); byMemberId ++)
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
			
			CallLog("[ProcMcsMcuVmpCmd]VMP Batch Poll stops!\n");
			break;
		}
	default:
		ConfLog(FALSE,"[ProcMcsMcuVmpCmd]Unexpected Message event is received!\n");
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
void CMcuVcInst::ClearOneVmpMember(u8 byChnnl, TVMPParam &tVmpParam, BOOL32 bRecover)
{
	tVmpParam.ClearVmpMember(byChnnl);
	TVMPMember *ptLastVmpMember = m_tLastVmpParam.GetVmpMember(byChnnl);
	if(!ptLastVmpMember->IsNull())
	{
		if(bRecover)
		{
			//恢复到上一次的VMP param，交换都不变
			tVmpParam.SetVmpMember(byChnnl, *ptLastVmpMember);
		}
		else
		{
			//清交换
			StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byChnnl, FALSE, MODE_VIDEO);
            TMt tVmpMem = *(TMt*)ptLastVmpMember;
			m_tVmpChnnlInfo.ClearOneChnnl(tVmpMem); //清该Mt对应的HD通道(如果存在对应通道)
		}
	}

	m_tConf.m_tStatus.SetVmpParam(tVmpParam);
	memcpy(&m_tLastVmpParam, &tVmpParam, sizeof(TVMPParam));

	//告诉会控刷界面
	CServMsg cServMsg;
	cServMsg.SetMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
	SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );	

	// xliang [4/10/2009] 同时更新tPeriEqpStatus，mcs要依据此刷MT图标,而不是依据TMtStatus :(
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
	g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	cServMsg.SetMsgBody();
	cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
	SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );

	return;
}
// END OF FILE

