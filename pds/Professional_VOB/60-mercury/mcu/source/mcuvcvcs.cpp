/*****************************************************************************
   模块名      : mcu
   文件名      : mcuvcvcs.cpp
   相关文件    : mcuvc.h
   文件实现功能: MCU会议调度业务函数
   作者        : 付秀华
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   08/12/08                付秀华        创建
******************************************************************************/
 
#include "evmcumcs.h"
#include "evmcuvcs.h"
#include "evmcumt.h"
#include "evmcueqp.h"
#include "evmcu.h"
#include "evmcutest.h"
#include "evmcudcs.h"
#include "evmp.h"
#include "mcuvc.h"
#include "mcsssn.h"
//#include "eqpssn.h"
#include "mtadpssn.h"
//#include "mcuutility.h"

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
    函数名      ：DaemonProcVcsMcuSendPackinfoRqp
    功能        ：VCS请求保存分组信息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华          创建
====================================================================*/
void CMcuVcInst::DaemonProcVcsMcuSavePackinfoReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	if (cServMsg.GetTotalPktNum() <= cServMsg.GetCurPktIdx())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[SavePack]uncorrect messagebody(total num:%d  curidx:%d)\n",
		          cServMsg.GetTotalPktNum(), cServMsg.GetCurPktIdx());
		cServMsg.SetMsgBody();
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		return;
	}

	CConfId cConfId = cServMsg.GetConfId();	
	// 对不存在的调度席/正调度的调度席不可更新分组信息
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	u8      byConfPos  = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;
	
	GetAllConfHeadFromFile(acConfId, sizeof(acConfId));
	for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE); nPos++)
	{
		if (acConfId[nPos] == cServMsg.GetConfId())
		{
			byConfPos = (u8)nPos;
			break;
		}
	}
	
	if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE == byConfPos)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "confinfo_head.dat has no temp with specified confid\n");
		cServMsg.SetErrorCode(ERR_MCU_TEMPLATE_NOTEXIST);
		cServMsg.SetMsgBody();
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
	}
	else
	{
		u8 byEditer = g_cMcuVcApp.GetTempEditerInfo(byConfPos);
		if (byEditer && byEditer != cServMsg.GetSrcSsnId())
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS,"reject request from inst%d, because Inst%d is in modification of tempid",
				      cServMsg.GetSrcSsnId(), byEditer);
			cServMsg.GetConfId().Print();

		    cServMsg.SetErrorCode(ERR_MCU_TEMPLATE_INEDITING);
			cServMsg.SetMsgBody();
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
		}

		if (g_cMcuVcApp.IsConfOnGoing(cConfId))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS,"调度席正在被调度，不可进行修改\n");
		    cServMsg.SetErrorCode(ERR_MCU_TEMPLATE_INVC);
			cServMsg.SetMsgBody();
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			return;
		}

		if (!byEditer)
		{
			g_cMcuVcApp.SetTempEditerInfo(byConfPos, cServMsg.GetSrcSsnId());
		}

		KillTimer(MCUVC_PACKINFO_TIMER + byConfPos);
	
		BOOL32 bOverWrite = !cServMsg.GetCurPktIdx();
		SetUnProConfDataToFile(byConfPos, (s8*)cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen(),bOverWrite );
		cServMsg.SetMsgBody();
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

		if (cServMsg.GetCurPktIdx() == cServMsg.GetTotalPktNum() - 1)
		{
			NotifyVCSPackInfo(cConfId);
			g_cMcuVcApp.SetTempEditerInfo(byConfPos, 0);
		}
		else
		{
			SetTimer(MCUVC_PACKINFO_TIMER + byConfPos, 3000, byConfPos);			
		}
	}

	return;
}

/*====================================================================
    函数名      ：DaemonProcPackInfoTimer
    功能        ：分组信息获取超时
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华          创建
====================================================================*/
void CMcuVcInst::DaemonProcPackInfoTimer( const CMessage * pcMsg )
{
	KillTimer(pcMsg->event);

	u8 byConfIdx = *(pcMsg->content);
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	GetAllConfHeadFromFile(acConfId, sizeof(acConfId));
	CConfId cConfId = acConfId[byConfIdx];
	ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Warning: pack info is not complete for confid:");
	cConfId.Print();

	NotifyVCSPackInfo(cConfId);
	g_cMcuVcApp.SetTempEditerInfo(byConfIdx, 0);

}
/*====================================================================
    函数名      ：ProcVcsMcuCreateConfReq
    功能        ：VCS创建建会议处理函数 
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华          创建

====================================================================*/
void CMcuVcInst::ProcVcsMcuCreateConfReq( const CMessage * pcMsg )
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

	
	//先解析tConfInfoEx信息，BAS判资源是否足够会用到[12/29/2011 chendaiwei]
	TConfInfoEx tConfInfoEx;
	if(GetConfExInfoFromMsg(cServMsg,tConfInfoEx))
	{
		tTmpConf.SetHasConfExFlag(TRUE);
	}

	m_tConfEx = tConfInfoEx;

	switch( CurState() )
	{
	case STATE_IDLE:
		{
			//清空实例
			ClearVcInst();
			cServMsg.SetErrorCode(0);       

			//设置开始时间
			tTmpConf.m_tStatus.SetOngoing();
			tTmpConf.SetStartTime( time( NULL ) );
      
			// 已达最大会议数，创会时不考虑是否级联
			if (g_cMcuVcApp.IsConfNumOverCap(tTmpConf.m_tStatus.IsOngoing(), tTmpConf.GetConfSource()))
			{
				cServMsg.SetErrorCode( ERR_MCU_CONFNUM_EXCEED );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Conference %s failure because exceed max conf num!\n", tTmpConf.GetConfName() );
				return;
			}

			if (0 == g_cMcuVcApp.GetMpNum() || 0 == g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323))
			{
				cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Conference %s failure because No_Mp or No_MtAdp!\n", tTmpConf.GetConfName() );
				return;
			}
			
			//开辟终端数据区
			if( NULL == m_ptMtTable )
			{
				m_ptMtTable = new TConfMtTable;
				START_CONF_CHECK_MEMORY(m_ptMtTable, cServMsg, tTmpConf);
			}

			memset( m_ptMtTable, 0, sizeof( TConfMtTable ) );

			NewMcuBasMgr();
			ClearBasMgr();
			
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
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Conference %s failure for conf full!\n", tTmpConf.GetConfName() );
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
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "MVidType.%d with FPS.%d is unexpected, adjust it\n",
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
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "SVidType.%d with FPS.%d is unexpected, adjust it\n",
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
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "DSVidType.%d with FPS.%d is unexpected, adjust it\n",
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

			tConfMode = m_tConf.m_tStatus.GetConfMode();
			if(0 == g_cMcuAgent.GetGkIpAddr())
			{
				tConfMode.SetRegToGK(FALSE);
			}

			// 设定会议的定时呼叫间隔
			if( m_tConf.m_tStatus.GetCallInterval() < MIN_CONF_CALLINTERVAL )
			{
				tConfMode.SetCallInterval( MIN_CONF_CALLINTERVAL );
			}
			else
			{
				tConfMode.SetCallInterval( m_tConf.m_tStatus.GetCallInterval() );
			}
			// 设定会议定时呼叫的次数，对于调度会议中的所有定时呼叫终端，采取无限次呼叫
			tConfMode.SetCallTimes( DEFAULT_CONF_CALLTIMES );
			m_tConf.m_tStatus.SetConfMode( tConfMode );
			
			m_tConf.m_tStatus.SetPrsing( FALSE );
       		m_tConfAllMtInfo.Clear();
			m_tConfAllMtInfo.m_tLocalMtInfo.SetConfIdx( m_byConfIdx );
			m_tConfAllMcuInfo.Clear();
        
			// VCS默认启用PRS
		    TConfAttrb tAttrb = m_tConf.GetConfAttrb();
			tAttrb.SetPrsMode(TRUE);
			m_tConf.SetConfAttrb(tAttrb);

			//会议信息 逻辑检测
			if ( !IsConfInfoCheckPass(cServMsg, m_tConf, wErrCode))
			{
				CreateConfSendMcsNack( byOldInsId, wErrCode, cServMsg );
				return;
			}

			//会议数据整理, 在此处整理出定时呼叫的实体和需手动呼叫的实体
 			VCSConfDataCoordinate(cServMsg, byMtNum, byCreateMtId);
//         
			//若会议结束模式为无终端自动结束，应该有终端
			if(0 == byMtNum && m_tConf.GetConfAttrb().IsReleaseNoMt())
			{
				ClearHduSchemInfo();
				CreateConfSendMcsNack( byOldInsId, ERR_MCU_NOMTINCONF, cServMsg );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Conference %s create failed because on mt in conf!\n", m_tConf.GetConfName() );
				return;
			}

			//MCU的外设能力是否充分
			if ( !IsVCSEqpInfoCheckPass(wErrCode))
			{
				ClearHduSchemInfo();
				CreateConfEqpInsufficientNack( byCreateMtId, byOldInsId, wErrCode, cServMsg );
				return;
			}

			// 会议处理: 即时会议, 无预约会议
			if( NULL == m_ptSwitchTable )
			{
				m_ptSwitchTable = new TConfSwitchTable;
				START_CONF_CHECK_MEMORY(m_ptSwitchTable, cServMsg, tTmpConf);
			}
			if( NULL == m_ptConfOtherMcTable )
			{
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

// 			//MCU的外设能力是否充分
// 			if ( !IsVCSEqpInfoCheckPass(wErrCode))
// 			{
// 				CreateConfEqpInsufficientNack( byCreateMtId, byOldInsId, wErrCode, cServMsg );
// 				return;
// 			}

			TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();

			if( tConfAttrib.IsDiscussConf() )
			{
				// 顾振华 [4/29/2006] 在这里启动全体混音
				//tianzhiyong 2010/03/21 增加开启模式
				StartMixing(mcuWholeMix);
			}

			//应答 
			cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
			cServMsg.SetConfId( m_tConf.GetConfId() );
			if( m_byCreateBy == CONF_CREATE_MCS && !bConfFromFile )
			{
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
			}
            else if (m_byCreateBy == CONF_CREATE_MT && !bConfFromFile)
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
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "conference %s created and started!\n",m_tConf.GetConfName());
			}
			else if ( CONF_DATAMODE_VAANDDATA == m_tConf.GetConfAttrb().GetDataMode() )
			{
				//视讯和数据
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "conference %s created and started with data conf function !\n", m_tConf.GetConfName());
			}
			else
			{
				//数据会议
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "conference %s created and started with data conf function Only !\n", m_tConf.GetConfName());
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
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "charge postponed due to GetRegGKDriId.%d, ChargeRegOK.%d !\n",
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

			InviteUnjoinedMt(cServMsg);

			//发Trap消息
			TConfNotify tConfNotify;
			CConfId cConfId;
			cConfId = m_tConf.GetConfId( );
			memcpy( tConfNotify.m_abyConfId, &cConfId, sizeof(cConfId) );
			astrncpy( tConfNotify.m_abyConfName, m_tConf.GetConfName(),
					  sizeof(tConfNotify.m_abyConfName), MAXLEN_CONFNAME);
			SendTrapMsg( SVC_AGT_CONFERENCE_START, (u8*)&tConfNotify, sizeof(tConfNotify) );	
        
			//设置终端状态刷新的间隔
			m_tRefreshParam.dwMcsRefreshInterval = g_cMcuVcApp.GetMcsRefreshInterval();

			//定时无限次呼叫未加入的定时呼叫实体
			SetTimer( MCUVC_INVITE_UNJOINEDMT_TIMER, 1000*m_tConf.m_tStatus.GetCallInterval() );

			//定时通告MCS关于下级MCU的当前媒体源信息
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
			//cServMsg.CatMsgBody( (u8 *)&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
			SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
        
			//发给会控终端表 
			SendMtListToMcs(LOCAL_MCUIDX);	

			// 发送给会控当前的调度会议的模式
			// 默认创建时都为单方调度模式
			u8 byAutoMode = m_cVCSConfStatus.GetCurVCMode();
			m_cVCSConfStatus.VCCDefaultStatus();
			if( 0 != byAutoMode)
			{
				RestoreVCConf(byAutoMode);
			}
			else
			{
				VCSConfStatusNotif();
			}
			
			
			u8 byScrId = cServMsg.GetSrcSsnId();
			if (byScrId != 0)
			{
				m_cVCSConfStatus.SetCurSrcSsnId(byScrId);
			}

			if (CONF_CREATE_MT == m_byCreateBy)
			{
				ProcMMCUGetConfCtrlMsg();
				//zjj20100329 vcs下级会议所有终端初始化为自动呼叫
				for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
				{
					if (m_tConfAllMtInfo.MtInConf(byMtId) &&
						!m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
						m_ptMtTable->GetMtType(byMtId) != MT_TYPE_MMCU)
					{						
						m_ptMtTable->SetCallMode( byMtId,CONF_CALLMODE_TIMER );
					}	
				}
			}


			//zjj20091102 读取文件会议信息文件中的预案信息
			if(!m_cCfgInfoRileExtraDataMgr.ReadExtraDataFromConfInfoFile(m_byConfIdx))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "ReadExtraDataFromConfInfoFile faield!\n");
			}
		}
           
		break;
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	
	return;
}
/*====================================================================
    函数名      ：GetVCMTByAlias
    功能        ：根据终端别名获取终端的TMt信息,若该终端尚未分配ID信息，
	              则分配对应资源
    算法实现    ：
    引用全局变量：
    输入参数说明：tMtAlias：   终端别名
	              wMtDialRate：终端呼叫码率
				  byForce:     是否确保资源分配成功，
				               若id资源已被耗尽，释放其它不在线终端(除主席 发言人)
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/08                付秀华          创建
====================================================================*/
TMt CMcuVcInst::GetVCMTByAlias(TMtAlias& tMtAlias, u16 wMtDialRate, u8 byForce)
{
	u8 byMtId = AddMt(tMtAlias, wMtDialRate, CONF_CALLMODE_NONE);
	if (byForce && !byMtId)
	{
		// 释放一般性资源给对应终端，不能释放主席 发言人或者在电视墙中的终端
		for (u8 byMtIdLoop = 1; byMtIdLoop <= MAXNUM_CONF_MT; byMtIdLoop++)
		{
			if (!m_tConfAllMtInfo.MtJoinedConf(byMtIdLoop) &&
				m_tConf.GetChairman().GetMtId() != byMtIdLoop &&
				m_tConf.GetSpeaker().GetMtId() != byMtIdLoop &&
				!m_ptMtTable->IsMtInHdu(byMtIdLoop) &&
				!m_ptMtTable->IsMtInTvWall(byMtIdLoop))
			{
                TMt tMt = m_ptMtTable->GetMt(byMtIdLoop); 
				RemoveMt(tMt, TRUE);
				break;
			}
		}

		byMtId = AddMt(tMtAlias, wMtDialRate, CONF_CALLMODE_NONE);
	}

	//发给会控会议所有终端信息
	CServMsg cServMsg;
	//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
	//cServMsg.CatMsgBody( (u8 *)&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//发给会控终端表 
	SendMtListToMcs(LOCAL_MCUIDX);	

	return m_ptMtTable->GetMt(byMtId);

}
/*====================================================================
    函数名      ：ProcVcsMcuVCMTReq
    功能        ：VCS创建建会议处理函数 
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/08                付秀华          创建

====================================================================*/
void CMcuVcInst::ProcVcsMcuVCMTReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	switch(CurState())
	{
	case STATE_ONGOING:
		{
			//action abort when no chairman online
			TMt tChairMan = m_tConf.GetChairman();
			if( ( tChairMan.IsNull()  || 
				!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
				m_byCreateBy != CONF_CREATE_MT 
				)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			u8* pbyMsgBody = cServMsg.GetMsgBody();

			TMt tVCMT;
			u8 byMTDtype = *pbyMsgBody++;
			if (VCS_DTYPE_MTID == byMTDtype)
			{
				tVCMT = *(TMt*)pbyMsgBody;
				pbyMsgBody += sizeof(TMt);
			}
			else if (VCS_DTYPE_MTALIAS == byMTDtype)
			{
				u8 byAliasType = *pbyMsgBody++;
				u8 byAliasLen  = *pbyMsgBody++;
				TMtAlias tMtAlias;
				if (!tMtAlias.SetAlias(byAliasType, byAliasLen, (s8*)pbyMsgBody))
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "bad alias info(type:%d byaliaslen:%d)\n",
							  byAliasType, byAliasLen);
					return;
				}
				pbyMsgBody += byAliasLen;
				
				u8 byForce = !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode());
				tVCMT = GetVCMTByAlias(tMtAlias, ntohs( *(u16*)pbyMsgBody ),byForce);
				pbyMsgBody += sizeof(u16);
			}
			else
			{
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "bad mt description type(%d)\n",
					      byMTDtype);
				return;
			}


			u8 byVCType  = *(u8*)pbyMsgBody;
			pbyMsgBody += sizeof(u8); 

			//zhouyiliang 20100824 vcs会议调度的是本地备份终端的时候return，不做处理
			if ( m_cVCSConfStatus.GetVcsBackupChairMan() ==  tVCMT ) 
			{
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "mt:%d is vcs backupchairman ,can't call \n",
					      tVCMT.GetMtId());
				return;
			}

			//强拆 
			u8 byForceCallType = *(u8*)pbyMsgBody; 
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "operate(vctype:%d, forcecalltype:%d) mt(mcuid:%d mtid:%d) byMTDtype.%d req\n",
				    byVCType, byForceCallType, tVCMT.GetMcuId(), tVCMT.GetMtId(),byMTDtype );


			//zjj, 在组呼模式下,强拆类型不作调度处理,只作呼叫
			if( VCS_FORCECALL_CMD == byForceCallType )
			{
                //[11/23/2011 zhushengze]终端已经入会，不再强拆
                if (m_tConfAllMtInfo.MtJoinedConf(tVCMT.GetMcuId(), tVCMT.GetMtId()))
                {
                    cServMsg.SetEventId(cServMsg.GetEventId() + 2);
                    SendReplyBack(cServMsg, cServMsg.GetEventId());
                    LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ProcVcsMcuVCMTReq]mt.%d has joined conf",tVCMT.GetMtId());
                    return;
                }

				if( ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() ) )
				{
					VCSCallMT( cServMsg,tVCMT,byForceCallType );
					return;
				}
				else if( m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE &&
					( !IsMtNotInOtherHduChnnl(tVCMT,0,0) || !IsMtNotInOtherTvWallChnnl(tVCMT,0,0) ) )
				{
					VCSCallMT( cServMsg,tVCMT,byForceCallType );
					return;
				}				
			}

			//zjj20100325  取消强拆的终端要改成手动呼叫(代码合并)
			if( VCS_FORCECALL_CANCELCMD == byForceCallType )
			{
				if( ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() ) )
				{
					m_ptMtTable->SetCallMode( GetLocalMtFromOtherMcuMt(tVCMT).GetMtId(),CONF_CALLMODE_NONE );
					cServMsg.SetEventId(cServMsg.GetEventId() + 1);
					SendReplyBack(cServMsg, cServMsg.GetEventId());
				}
				else
				{
					VCSClearTvWallChannelByMt( tVCMT,TRUE );
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Error, not GroupMode, can't recv VCS_FORCECALL_CANCELCMD.\n");
					cServMsg.SetEventId(cServMsg.GetEventId() + 2);
					SendReplyBack(cServMsg, cServMsg.GetEventId());
				}
				return;
			}
			// 对调度资源的有效性进行判断,必须在受邀列表中，且不为定时呼叫的固定非可调用资源(高清电视墙、绑定终端、下级mcu)
			if (!m_tConfAllMtInfo.MtInConf(tVCMT.GetMcuId(), tVCMT.GetMtId())
				//|| (CONF_CALLMODE_TIMER == VCSGetCallMode(tVCMT))
				)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_NOVCSOURCE);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			// 判断上一个请求调度的终端是否完成调度
			TMt tOldReqVCMT = m_cVCSConfStatus.GetReqVCMT();
			if (!tOldReqVCMT.IsNull() && !(tOldReqVCMT == tVCMT))
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_VCMTING);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);				
				cServMsg.SetMsgBody((u8*)&tOldReqVCMT, sizeof(TMt));
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			// 针对多方多画面,连续点击一个终端,在请求调度的终端未完成调度前不做响应,画面合成器未处理完全也不做响应
			if ( VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() )
			{
				// 在请求调度的终端未完成调度前不做响应
				if (!tOldReqVCMT.IsNull())
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_VCMTING);
					cServMsg.SetEventId(cServMsg.GetEventId() + 2);				
					cServMsg.SetMsgBody((u8*)&tOldReqVCMT, sizeof(TMt));
					SendReplyBack(cServMsg, cServMsg.GetEventId());
					return;
				}
				// 画面合成器未处理完全也不做响应
				if (!IsChangeVmpParam())
				{
					cServMsg.SetErrorCode(ERR_VMPBUSY);
					cServMsg.SetEventId(cServMsg.GetEventId() + 2);
					SendReplyBack(cServMsg, cServMsg.GetEventId());
					return;
				}
			}
			
			// 保存当前操作的vcsssn信息
			u8 byScrId = cServMsg.GetSrcSsnId();
			if (byScrId != 0)
			{
				m_cVCSConfStatus.SetCurSrcSsnId(byScrId);
			}

			TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();

			// 对调度资源的与会情况进行判断，若已与会，在调度模式下默认为进行挂断操作			
			BOOL byVCMTOnline = m_tConfAllMtInfo.MtJoinedConf(tVCMT.GetMcuId(), tVCMT.GetMtId());
			
			if (byVCMTOnline && !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) )
			{
				VCSDropMT(tVCMT);						
			}

	
			switch(m_cVCSConfStatus.GetCurVCMode())
			{
			// 单方调度模式下
			case VCS_SINGLE_MODE:
				{
					// 该模式下，不会出现在线非调度终端
					if (byVCMTOnline && !(tCurVCMT == tVCMT))
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Error, non-curVCMT on line\n");
						cServMsg.SetEventId(cServMsg.GetEventId() + 2);
						SendReplyBack(cServMsg, cServMsg.GetEventId());
						return;
					}

					// 对在线调度终端, 挂断直接回应ACK
					if (tVCMT == tCurVCMT)
					{
						cServMsg.SetEventId(cServMsg.GetEventId() + 1);
						SendReplyBack(cServMsg, cServMsg.GetEventId());
					}
					// 对非在线终端调度，呼叫新调度终端
					else
					{		
						// 设置当前请求调度的终端，呼叫成功后才挂断当前调度终端
						m_cVCSConfStatus.SetReqVCMT(tVCMT);
						VCSCallMT(cServMsg, tVCMT, byForceCallType);
					}
				}
				break;
			case VCS_MULTW_MODE:
				{
					// 对在线的所有调度资源调度，直接删除该终端而无需其它业务
					if (byVCMTOnline)
					{
						cServMsg.SetEventId(cServMsg.GetEventId() + 1);
						SendReplyBack(cServMsg, cServMsg.GetEventId());
						return;
					}
					// 对不在线的调度资源，进行呼叫再选看
					else
					{
						m_cVCSConfStatus.SetReqVCMT(tVCMT);
						VCSCallMT(cServMsg, tVCMT, byForceCallType);
					}					

				}

				break;
			case VCS_MULVMP_MODE:
				{
					// 对在线的所有调度资源调度，直接删除该终端而无需其它业务
					if (byVCMTOnline)
					{
						cServMsg.SetEventId(cServMsg.GetEventId() + 1);
						SendReplyBack(cServMsg, cServMsg.GetEventId());
						return;
					}

					// 对于不再线的调度资源，进行呼入进画面合成
					m_cVCSConfStatus.SetReqVCMT(tVCMT);
					VCSCallMT(cServMsg, tVCMT, byForceCallType);
				}
				break;
			case VCS_GROUPSPEAK_MODE:
			case VCS_GROUPTW_MODE:
			case VCS_GROUPVMP_MODE:
			case VCS_GROUPCHAIRMAN_MODE:
			case VCS_GROUPROLLCALL_MODE:
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
				if (VCS_VCTYPE_DROPMT == byVCType)
				{				
					VCSDropMT(tVCMT);					
				}
				else
				{
					if (!m_tConfAllMtInfo.MtJoinedConf(tVCMT.GetMcuId(), tVCMT.GetMtId()))
					{
						// 不在线则自动呼叫并申请发言人
						m_cVCSConfStatus.SetReqVCMT(tVCMT);
						VCSCallMT(cServMsg, tVCMT, byForceCallType);
					}
					else
					{
						// 组呼模式下再次点击当前调度终端,清空该当前调度终端
						if (tVCMT == tCurVCMT)
						{
							tVCMT.SetNull();							
						}
						if( tVCMT.IsNull() 
							&& CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode() 
							&& !m_tApplySpeakQue.IsQueueNull()
							)
						{
							if(  m_tApplySpeakQue.GetQueueHead( tVCMT ) )
							{
								ChgCurVCMT( tVCMT );	
							}
							else
							{
								ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Fail to get Queue Head\n" );
							}
						}
						else
						{
							ChgCurVCMT(tVCMT);
						}						
					}
				}
				break;
			default:
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Wrong work mode\n");
				break;
			}
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	VCSConfStatusNotif();
}
/*====================================================================
    函数名      ：ProcVcsMcuVCModeReq
    功能        ：VCS调度模式请求处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/08                付秀华          创建

====================================================================*/
void CMcuVcInst::ProcVcsMcuVCModeReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			u8 byMode = m_cVCSConfStatus.GetCurVCMode();
			cServMsg.SetMsgBody(&byMode, sizeof(u8));
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
	    break;
	}
	
}
/*====================================================================
    函数名      ：ProcVcsMcuVCModeChgReq
    功能        ：VCS修改调度模式的请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/08                付秀华          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuVCModeChgReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			u8 byNewMode = *(u8 *)cServMsg.GetMsgBody();
			

			if( byNewMode != VCS_SINGLE_MODE &&
				byNewMode != VCS_MULVMP_MODE &&
				byNewMode != VCS_GROUPSPEAK_MODE &&
				byNewMode != VCS_GROUPVMP_MODE &&
				byNewMode != VCS_GROUPCHAIRMAN_MODE &&
				byNewMode != VCS_GROUPROLLCALL_MODE )
			{
				ConfPrint( LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuVCModeChgReq] error mode.%d\n",byNewMode );
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			//action abort when no chairman online
			TMt tChairMan = m_tConf.GetChairman();
			if( ( tChairMan.IsNull()  || 
				!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
				m_byCreateBy != CONF_CREATE_MT
				)
			{
				// change to mode other than single mode is not allowed 
				if ( VCS_SINGLE_MODE !=  byNewMode ) 
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
					cServMsg.SetEventId(cServMsg.GetEventId() + 2);
					SendReplyBack(cServMsg, cServMsg.GetEventId());
					return;						
				}
			}

			u8 byOldMode = m_cVCSConfStatus.GetCurVCMode();
			if ( byOldMode != byNewMode )
			{
				// 切换至新模式,所需外设是否足够
				if (VCS_MULTW_MODE == byNewMode || VCS_GROUPTW_MODE == byNewMode)
				{
					// [5/30/2011 liuxu] 重写了FindUsableTWChan, 用FindNextTvwChnnl代替
					if( FindNextTvwChnnl(m_cVCSConfStatus.GetCurUseTWChanInd()) )
					/*if (!FindUsableTWChan(m_cVCSConfStatus.GetCurUseTWChanInd()))*/
					{
						// 未找到可用的通道索引，则拒绝
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcVcsMcuVCModeChgReq] Find no usable TW!\n");
						cServMsg.SetErrorCode(ERR_MCU_VCS_NOUSABLETW);
						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						return;
					}
				}
				//zjj20091102 新增组呼点名模式，也要起画面合成
				//zjj20091102 因为组呼点名人模式和组呼画面合成模式都要用画面合成器，而且这两个模式可以直接切换，
				//所以在这两个模式之间切换也不用去找画面合成设备了
				else if (VCS_MULVMP_MODE == byNewMode || 
						(
							( VCS_GROUPVMP_MODE == byNewMode ||VCS_GROUPROLLCALL_MODE == byNewMode ) &&
							!( VCS_GROUPVMP_MODE == byNewMode && VCS_GROUPROLLCALL_MODE == byOldMode ) && 
							!( VCS_GROUPROLLCALL_MODE == byNewMode && VCS_GROUPVMP_MODE == byOldMode )
						)
				)
				{
					// 查询是否存在可用的VMP,存在就先占有
					if (!FindUsableVMP())
					{
						// 未找到可用的画面合成器，则拒绝
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcVcsMcuVCModeChgReq] Find no usable VMP!\n");
						cServMsg.SetErrorCode(ERR_MCU_VCS_NOUSABLEVMP);
						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						return;
					}		
				}
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

				// 保存当前操作的vcsssn信息
				u8 byScrId = cServMsg.GetSrcSsnId();
				if (byScrId != 0)
				{
					m_cVCSConfStatus.SetCurSrcSsnId(byScrId);
				}

				// 恢复并初始化为新模式
				RestoreVCConf(byNewMode);
				cServMsg.SetMsgBody(&byNewMode, sizeof(u8));
				SendMsgToAllMcs(cServMsg.GetEventId() + 3, cServMsg);

			}
			else
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "No change about vcmode\n");
				cServMsg.SetErrorCode(ERR_MCU_VCS_NOMODECHG);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			}
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuVCModeChgReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
	    break;
	}
	
}
/*====================================================================
    函数名      ：ProcVcsMcuMuteReq
    功能        ：VCS修改静哑音模式的请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/08                付秀华          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuMuteReq( CServMsg& cServMsg )
{
/*	CServMsg cServMsg(pcMsg->content, pcMsg->length);*/

	switch(CurState())
	{
	case STATE_ONGOING:
		{
		    if (cServMsg.GetMsgBodyLen() < sizeof(u8) * 4)
		    {
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuMuteReq] invalid param\n");
				return;
		    }	
			
			//action abort when no chairman online
			TMt tChairMan = m_tConf.GetChairman();
			if( ( tChairMan.IsNull()  || 
				!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
				m_byCreateBy != CONF_CREATE_MT
				)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			BOOL32 bCmdFrmOtherMcu = FALSE;

			if ( cServMsg.GetMsgBodyLen() >= 5 )
			{
				bCmdFrmOtherMcu = *(u8*)(cServMsg.GetMsgBody() + 4 * sizeof(u8)) ? TRUE : FALSE;
			}

			u8 byMode = *(u8*)cServMsg.GetMsgBody();
			if( !bCmdFrmOtherMcu && byMode != m_cVCSConfStatus.GetCurVCMode() )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "Mode changed, operation cancel.( byMode:%u, CurVCMode: %u )\n",
					byMode, m_cVCSConfStatus.GetCurVCMode());
				cServMsg.SetErrorCode(ERR_MCU_VCS_CANCEL);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
			
			u8  byMuteOpenFlag = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8));         //是否开启
			u8  byOprObj       = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8) * 2);     //操作本地/远端
			u8  byAudProcType  = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8) * 3);     //处理静音/哑音
			
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "MuteReq: %s, %s, %s\n", 
				(byOprObj != VCS_OPR_LOCAL) ? "remote" : "local", 
				byAudProcType == VCS_AUDPROC_MUTE ? "dumb" : "mute",
				byMuteOpenFlag ? "open" : "close");

			if (VCS_OPR_LOCAL == byOprObj)
			{
				// 保存本地静哑音状态
				m_cVCSConfStatus.SetLocAudProcType(byMuteOpenFlag, byAudProcType);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

				// 本地静哑音，针对主席终端
				TMt tMt;
				tMt = m_tConf.GetChairman();
				if (!tMt.IsNull())
				{
					VCSMTMute(tMt, byMuteOpenFlag, byAudProcType);
				}
			}
			else
			{
				// 保存远端静哑音状态
				m_cVCSConfStatus.SetRemAudProcType(byMuteOpenFlag, byAudProcType);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

				// 远端静哑音，针对所有在线的可调度终端 + 下级的mcu
				for (u8 byMTId = 1; byMTId <= MAXNUM_CONF_MT; byMTId++)
				{
					if (m_tConfAllMtInfo.MtJoinedConf(byMTId)  
						//(CONF_CALLMODE_NONE == m_ptMtTable->GetCallMode(byMTId) || MT_TYPE_SMCU == m_ptMtTable->GetMtType(byMTId)))
						//m_tConf.GetChairman().GetMtId() != byMTId 
						)
					{
						TMt tMt = m_ptMtTable->GetMt(byMTId);
						if (!tMt.IsNull())
						{
							// [9/19/2010 xliang] filter chairman optionally
							if ( !bCmdFrmOtherMcu && m_tConf.GetChairman().GetMtId() == byMTId)
							{
								continue;
							}

							// [9/17/2010 xliang] filter mmcu
							if(  MT_TYPE_MMCU == tMt.GetMtType() )
							{
								continue;
							}

							VCSMTMute(tMt, byMuteOpenFlag, byAudProcType);
							
							//////////////////////////////////////////////////////////////////
							/*
							远端静哑音的变更：（以远端静音为例）级联场景下，之前的理解是远端(下级终端)收不到任何声音,
							包括上级传给它的以及下级本身发送给它的。这就需要上级透传消息给下级，下面的代码就是透传消
							息的逻辑，其前提是当前下级终端都调度完毕后，再启用远端静音。
							如果上级再未调度任何终端时，就启用远端静音，随后再调度下级终端，这时对远端静哑音的处理在
							MtOnlineChange中，这时消息会由于级联通道尚未开通完毕，而无法传达到下级。

							所以这个策略就不对称，要对称的话，改动会比较大，且大容量场景下消息负载将会很大。
							下面是关于远端静音新的理解：远端不听本端的音频。级联环境中，非本级的即为远端，上级即本端。
							这样，下级终端不收上级的音频，但是可以收和它同级以及再下级的音频。

							在这个理解下，上级对下级MCU说要做的就是在必要地方拆建交换，而不需透传消息给下级。
							*/
							////////////////////////////////////////////////////////////////////
							// [2010/09/06 xliang] pass the msg to smcu
// 							if( MT_TYPE_SMCU == tMt.GetMtType() )
// 							{
// 								ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVcsMcuMuteReq] pass the msg to mt.%d(smcu)!\n", byMTId );
// 
// 								SendMsgToMt(byMTId, VCS_MCU_MUTE_REQ, cServMsg);
// 							}
						}					
						
					}
				}
			}
			
			// 通告会议状态信息
			VCSConfStatusNotif();
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuMuteReq] Wrong message %u(%s) received in state %u!\n", 
			     cServMsg.GetEventId(), ::OspEventDesc( cServMsg.GetEventId() ), CurState() );
		break;
	}

}
/*====================================================================
    函数名      ：ProcVCMTOverTime
    功能        ：调度终端处理超时
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/08                付秀华          创建

====================================================================*/
void CMcuVcInst::ProcVCMTOverTime( void )
{
	KillTimer(MCUVC_VCMTOVERTIMER_TIMER);

	TMt tReqVCMT = m_cVCSConfStatus.GetReqVCMT();
	if (!tReqVCMT.IsNull())
	{
		CServMsg cServMsg;
		cServMsg.SetConfId(m_tConf.GetConfId());
		cServMsg.SetConfIdx(m_byConfIdx);
		cServMsg.SetMsgBody((u8*)&tReqVCMT, sizeof(TMt));
		cServMsg.SetErrorCode(ERR_MCU_VCS_VCMTOVERTIME);
		SendMsgToAllMcs(MCU_VCS_VCMT_NOTIF, cServMsg);

		if (tReqVCMT.IsLocal())
		{
			if( !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) )
			{
				VCSClearTvWallChannelByMt( tReqVCMT,TRUE );
			}
			RemoveJoinedMt( tReqVCMT, TRUE );
		}

		TMt tNull;
		tNull.SetNull();
		m_cVCSConfStatus.SetReqVCMT(tNull);
	}

}

/*====================================================================
    函数名      : VCSFindMtInTvWallAndChangeSwitch
    功能        ：停止或恢复终端到电视墙地的交换
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
	11/05/26                刘旭            重新整理
====================================================================*/
void CMcuVcInst::VCSFindMtInTvWallAndChangeSwitch( TMt tMt,BOOL32 bIsStopSwitch,u8 byMode )
{
	//此函数现在只支持拆建视频或音频，不能两个都支持
	if( MODE_BOTH == byMode )
	{
		return;
	}

	// 一个会议支持的电视墙通道的总数
	const u8 byMaxTvwNum = MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL + MAXNUM_PERIEQP_CHNNL * MAXNUM_MAP;
	CConfTvwChnnl acTvwChnnl[ byMaxTvwNum ];
	memset(acTvwChnnl, 0, sizeof(acTvwChnnl));

	u8 byTvwId, byTvwChnnlIdx;

	// 根据源进行搜索
	u8 byFindNum = FindAllTvwChnnl(tMt, acTvwChnnl, byMaxTvwNum);
	for (u8 byLoop = 0; byLoop < byFindNum; ++byLoop)
	{
		// 此通道不在此会议中
		if (acTvwChnnl[byLoop].GetConfIdx() != m_byConfIdx)
		{
			continue;
		}
		
		// 不在线, 不继续操作
		if (!g_cMcuVcApp.IsPeriEqpConnected(acTvwChnnl[byLoop].GetEqpId()))
		{
			continue;
		}
		
		byTvwId = acTvwChnnl[byLoop].GetEqpId();
		byTvwChnnlIdx = acTvwChnnl[byLoop].GetChnnlIdx();
		
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_HDU, "Tvwall channel(%d, %d) is mt(%d,%d) bIsStopSwitch(%d)\n",
			byTvwId,
			byTvwChnnlIdx,
			acTvwChnnl[byLoop].GetMember().GetMcuId(),
			acTvwChnnl[byLoop].GetMember().GetMtId(),
			bIsStopSwitch);
		
		if( bIsStopSwitch )
		{
			StopSwitchToPeriEqp( byTvwId, byTvwChnnlIdx, FALSE, byMode );
		}
		else
		{
			StartSwitchToPeriEqp( tMt, 0, byTvwId, byTvwChnnlIdx, byMode, SWITCH_MODE_SELECT,FALSE,TRUE,FALSE,TRUE,FALSE);
		}
	}
}

/*====================================================================
    函数名      ProcVcsMcuTvWallMtStartMixing
    功能        ：开始电视墙通道中的终端混音
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuTvWallMtStartMixing( const CMessage *pcMsg )
{
	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	u8 byIsStartMixing = *(u8*)cServMsg.GetMsgBody();

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "byIsStartMixing is %d\n",byIsStartMixing );

	if( m_cVCSConfStatus.GetMtInTvWallCanMixing() == byIsStartMixing )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "Cur TvWall canMixing(%d) .needn't change\n",m_cVCSConfStatus.GetMtInTvWallCanMixing() );		
		return;
	}
	
	BOOL32 bStartMix = (1 == byIsStartMixing) ? TRUE : FALSE;

	if( m_cVCSConfStatus.GetCurVCMode() != VCS_SINGLE_MODE && m_tConf.m_tStatus.IsNoMixing() )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "Fail to change status because Current mode is not single mode(%d) and no mixing.\n",m_cVCSConfStatus.GetCurVCMode() );
		return;
	}

	m_cVCSConfStatus.SetMtInTvWallCanMixing( bStartMix );
	
	VCSChangeTvWallMtMixStatus( bStartMix );

	VCSConfStatusNotif();

}

/*====================================================================
    函数名      ProcVcsMcuClearAllTvWallChannel
    功能        ：清楚某个电视墙通道
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuClearAllTvWallChannel( void )
{
	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	if( m_cVCSConfStatus.GetTVWallManageMode() != VCS_TVWALLMANAGE_MANUAL_MODE )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "Cur TvWall manager mode is not manual mode.cant't clear channel\n",m_cVCSConfStatus.GetTVWallManageMode() );		
		return;
	}

	if( m_cVCSConfStatus.GetIsTvWallOperating() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "Now is operation tvWall.Nack\n" );	
		return;
	}

	//操作电视墙的伪原子操作
	//要在完成所有交换操作后才能进行下一此的操作
	m_cVCSConfStatus.SetIsTvWallOperating( TRUE );

	VCSClearAllTvWallChannel();

	m_cVCSConfStatus.SetIsTvWallOperating( FALSE );

}

/*====================================================================
    函数名      ProcVcsMcuClearOneTvWallChannel
    功能        ：清楚某个电视墙通道
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
	11/05/26                刘旭            重新整理
====================================================================*/
void CMcuVcInst::ProcVcsMcuClearOneTvWallChannel( const CMessage *pcMsg )
{
	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	if( m_cVCSConfStatus.GetTVWallManageMode() != VCS_TVWALLMANAGE_MANUAL_MODE )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "Cur TvWall manager mode is not manual mode.cant't clear channel\n",m_cVCSConfStatus.GetTVWallManageMode() );		
		return;
	}

	u8 *pbyBuf = (u8*)cServMsg.GetMsgBody();
	u8 byEqpId = *pbyBuf++;
	u8 byChannelIdx = *pbyBuf;

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "eqpid(%d) eqpType(%d) channelIdx(%d)\n", byEqpId, g_cMcuVcApp.GetEqpType(byEqpId), byChannelIdx );

	if ( !IsValidTvw(byEqpId, byChannelIdx))
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "eqp type is wrong or channelIdx is too large \n " );
		return;
	}

	if(	!g_cMcuVcApp.IsPeriEqpConnected( byEqpId ) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "eqp is not connected\n " );
		return;
	}

	if( m_cVCSConfStatus.GetIsTvWallOperating() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "Now is operation tvWall.Nack\n" );		
		return;
	}

	// 待查找的
	CTvwChnnl cTvwChnnl;
	cTvwChnnl.Clear();
	if (!GetTvwChnnl(byEqpId, byChannelIdx, cTvwChnnl))
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "Get tvw chnnl(%d, %d) failed\n", byEqpId, byChannelIdx );		
		return;
	}

	// 通道成员为空, 不需要清空
	if(cTvwChnnl.GetMember().IsNull())
	{
		return;
	}

	// 正式开始清除
	m_cVCSConfStatus.SetIsTvWallOperating( TRUE );

	// 提取通道成员
	TMt tOldMember = (TMt)cTvwChnnl.GetMember();

	// Hdu通道类型处理
	if (IsValidHduChn(byEqpId, byChannelIdx))
	{
		// 已经在HDU电视墙中的终端停交换
		// [2013/03/11 chenbing] VCS会议不支持HDU多画面,子通道置0
		ChangeHduSwitch( NULL, byEqpId, byChannelIdx, 0, TW_MEMBERTYPE_VCSSPEC, TW_STATE_STOP );				
	}
	// 普通电视墙通道类型处理
	else
	{
		ChangeTvWallSwitch( &tOldMember, byEqpId, byChannelIdx, TW_MEMBERTYPE_VCSSPEC, TW_STATE_STOP );			
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "TvWall channel(%d) is has mt(%d.%d),has stopped switch to tvwall\n ",
		byChannelIdx, tOldMember.GetMcuId(), tOldMember.GetMtId());

	// 终端被清出电视墙时, 要考虑挂断挂断它
	if (!(m_cVCSConfStatus.GetCurVCMT() == tOldMember ) 
		&& !m_ptMtTable->IsMtInHdu( GetLocalMtFromOtherMcuMt(tOldMember).GetMtId() ) 
		&& !m_ptMtTable->IsMtInTvWall( GetLocalMtFromOtherMcuMt(tOldMember).GetMtId()))
	{
		if (m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE)
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ClearOneTvWallChannel] cur mode is VCS_SINGLE_MODE VCSDropMT mt is in hdu(%d)\n",
				m_ptMtTable->IsMtInHdu( GetLocalMtFromOtherMcuMt(tOldMember).GetMtId() ) 
				);
			
			// 挂断终端
			VCSDropMT( tOldMember );
		}
		// 多方多画面下如果也不在画面合成中了, 也需要被挂断
		else if ( m_cVCSConfStatus.GetCurVCMode() == VCS_MULVMP_MODE ) 
			
		{
			TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
			if (!tConfVmpParam.IsMtInMember(tOldMember))
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ClearTvw(%d,%d)] DropMT(%d,%d) in VcsMode.%d\n",
					byEqpId, byChannelIdx, tOldMember.GetMcuId(), tOldMember.GetMtId(),  m_cVCSConfStatus.GetCurVCMode());
				
				VCSDropMT( tOldMember );
			}
		}
	}

	// 处理完后, 将新的Tvw通道状态变化通知给会控台
	TPeriEqpStatus tStatus;
	if( g_cMcuVcApp.GetPeriEqpStatus( byEqpId,&tStatus )) 
	{
		cServMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
		SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
	}
	
	m_cVCSConfStatus.SetIsTvWallOperating( FALSE );
}

/*====================================================================
    函数名      ProcVcsMcuChgTvWallMgrMode
    功能        ：改变电视墙调度模式
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuChgTvWallMgrMode( const CMessage *pcMsg )
{
	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	//action abort when no chairman online
	TMt tChairMan = m_tConf.GetChairman();
	if( ( tChairMan.IsNull()  || 
		!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
		m_byCreateBy != CONF_CREATE_MT
		)
	{
		cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
		cServMsg.SetEventId(cServMsg.GetEventId() + 2);
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}

	u8 byMode = *(u8*)cServMsg.GetMsgBody();

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "[ChgTvWallMgrMode from %d to %d\n", m_cVCSConfStatus.GetTVWallManageMode(), byMode );

	if( !( byMode >= VCS_TVWALLMANAGE_MANUAL_MODE &&
		byMode <= VCS_TVWALLMANAGE_REVIEW_MODE )
		)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuChgTvWallMgrMode] wrong tvwall manage mode(%d).Nack\n",byMode );
		//cServMsg.SetErrorCode( 16 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//只有在组呼会议下才有自动和预览模式
	if( byMode != VCS_TVWALLMANAGE_MANUAL_MODE && 
		!ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() )
		)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuChgTvWallMgrMode]  byMode(%d) can't in VCS GroupConfMode(%d).Nack\n",
				byMode,m_cVCSConfStatus.GetCurVCMode() );
		//cServMsg.SetErrorCode( 16 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}


	if( byMode == m_cVCSConfStatus.GetTVWallManageMode() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuChgTvWallMgrMode] curmode is %d. needn't change.Nack\n",byMode );
		//cServMsg.SetErrorCode( 16 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	if( m_cVCSConfStatus.GetIsTvWallOperating() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuChgTvWallMgrMode] Now is operation tvWall.Nack\n" );
		cServMsg.SetErrorCode(ERR_MCU_VCS_PLANISOPERATING);
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		return;
	}

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	//操作电视墙的伪原子操作
	//要在完成所有交换操作后才能进行下一此的操作
	m_cVCSConfStatus.SetIsTvWallOperating( TRUE );

	m_cVCSConfStatus.SetTVWallManageMode( byMode );

	//改为自动模式候，所有终端从第一个通道开始进入电视墙
	m_cVCSConfStatus.SetCurUseTWChanInd( 0 );

	//改变模式清空所有电视墙通道
	VCSClearAllTvWallChannel();	

	VCSConfStatusNotif();

	m_cVCSConfStatus.SetIsTvWallOperating( FALSE );
}

/*====================================================================
    函数名      ProcVcsMcuGetAllPlanData
    功能        ：通知界面现有的所有预案数据
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
====================================================================*/
void CMcuVcInst::ProcMcsVcsMcuGetAllPlanData( const CMessage *pcMsg )
{
	u16 wPrintModule = 0;
	BOOL32 bIsConsoleVCS = TRUE;

	if( MCS_CONF == m_tConf.GetConfSource())
	{
		wPrintModule = MID_MCU_MCS;
		bIsConsoleVCS = FALSE;
	}
	else
	{
		wPrintModule = MID_MCU_VCS;
	}

	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, wPrintModule, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	if(g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_SWITCH ||
		g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_IDLE )
	{
		ConfPrint(LOG_LVL_WARNING,MID_MCU_NPLUS,"N+1 1 Mode, not support plan!\n");
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		
		return;
	}

	if( !m_cCfgInfoRileExtraDataMgr.CheckPlanData() )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "[ProcVcsMcuGetAllPlanData] checkplandatafailed!\n " );
		//cServMsg.SetErrorCode( 16 );
		
		//MCS读取预案失败，视作无模板创会[11/22/2012 chendaiwei]
		if(!bIsConsoleVCS)
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}
		
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	if( !m_cCfgInfoRileExtraDataMgr.IsReadPlan() )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "[ProcVcsMcuGetAllPlanData] Plan Manager is not initialize.Nack\n " );
		//cServMsg.SetErrorCode( 16 );
		
		//MCS读取预案失败，视作无模板创会[11/22/2012 chendaiwei]
		if(!bIsConsoleVCS)
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}

		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	TMtVcsPlanNames tVcsPlanNames = {0};
	
	u8 idx = 0;
	s8 *pachPlanName[ VCS_MAXNUM_PLAN ];
	for( idx = 0;idx < VCS_MAXNUM_PLAN;idx++ )
	{
		pachPlanName[idx] = &tVcsPlanNames.m_achAlias[idx][0];
	}

	u8 byPlanNum = 0;
	m_cCfgInfoRileExtraDataMgr.GetAllPlanName( (s8**)pachPlanName,byPlanNum );

	if( 0 == byPlanNum )
	{	
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "byPlanNum is 0 ,no planData. can't notify data\n " );		
		return;
	}

	if( byPlanNum > VCS_MAXNUM_PLAN )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "byPlanNum(%d) is too big. can't notify data\n ",byPlanNum );		
		return;
	}

	CConfId tConfId = m_tConf.GetConfId();
	
	for( idx = 0;idx < byPlanNum;idx++ )
	{
		NotifyOnePlanDataToConsole( pachPlanName[idx],MCU_VCS_ALLPLANDATA_SINGLE_NOTIFY,bIsConsoleVCS );
	}
}

/*====================================================================
    函数名      ProcVcsMcuModifyPlanName
    功能        ：修改预案名
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
====================================================================*/
void CMcuVcInst::ProcMcsVcsMcuModifyPlanName( const CMessage *pcMsg )
{
	u16 wPrintModule = 0;
	if(VCS_CONF == m_tConf.GetConfSource())
	{
		wPrintModule = MID_MCU_VCS;
	}
	else
	{
		wPrintModule = MID_MCU_MCS;
	}

	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, wPrintModule, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);	

	if( m_cVCSConfStatus.GetIsPlanDataOperation() )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "Plan is operating.Nack\n " );		
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANISMODIFYING ); //TBD 新增ErrorCode?  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	m_cVCSConfStatus.SetIsPlanDataOperation( TRUE );

	u8* pbyBuf = cServMsg.GetMsgBody();
	if(pbyBuf == NULL)
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,"Modify Plan Buff is NULL\n ");
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	u8 byOldPlanNameLen = *pbyBuf;
	pbyBuf++;
	s8* pbyOldPlanName = new s8[byOldPlanNameLen+1];
	if( NULL == pbyOldPlanName )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "alloc memery pbyOldPlanName error!!!\n" ); 
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	memcpy( pbyOldPlanName,pbyBuf,byOldPlanNameLen );
	pbyOldPlanName[byOldPlanNameLen] = '\0';

	if( 0 == strcmp( "",pbyOldPlanName ) ) 
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "OldPlanName is empty!!!.\n " );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		return;
	}

	pbyBuf += byOldPlanNameLen;
	u8 byNewPlanNameLen = *pbyBuf;
	pbyBuf++;
	s8* pbyNewlanName = new s8[byNewPlanNameLen + 1];
	if( NULL == pbyNewlanName )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "alloc memery pbyNewlanName error!!!\n" ); 
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	memcpy( pbyNewlanName,pbyBuf,byNewPlanNameLen );
	pbyNewlanName[ byNewPlanNameLen ] = '\0';
	

	if( strlen( pbyOldPlanName ) != byOldPlanNameLen ||
		strlen( pbyNewlanName ) != byNewPlanNameLen )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "OldPlanName(%s) Len(%d) is not recLen(%d) or NewPlanName(%s) Len(%d) is not recLen(%d).Nack\n ",
			pbyOldPlanName,strlen( pbyOldPlanName ),byOldPlanNameLen,
			pbyNewlanName,strlen( pbyNewlanName ),byNewPlanNameLen
			);

		//cServMsg.SetErrorCode( 15 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}


	if( byOldPlanNameLen > (VCS_MAXLEN_ALIAS - 1) || 0 == byOldPlanNameLen)
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "OldPlanName(%s) Len is than 31 or is 0.Nack\n ",pbyOldPlanName );

		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNAMELENNOTVALID );//TBD 新增ErrorCode?  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}

	if( byNewPlanNameLen > (VCS_MAXLEN_ALIAS - 1) || 0 == byNewPlanNameLen)
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "NewPlanName(%s) Len is than 31 or is 0.Nack\n ",pbyNewlanName );

		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNAMELENNOTVALID );//TBD 新增ErrorCode?  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}

	if( !m_cCfgInfoRileExtraDataMgr.IsHasPlanName(pbyOldPlanName) )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "OldPlanName(%s) is not Exist.Nack\n ",pbyOldPlanName );
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNAMENOTEXIST );//TBD 新增ErrorCode?  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}

	if( m_cCfgInfoRileExtraDataMgr.IsHasPlanName(pbyNewlanName) )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "NewPlanName(%s) is Exist.Nack\n ",pbyNewlanName );
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNAMEALREADYEXIST );//TBD 新增ErrorCode?  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}

	if( !m_cCfgInfoRileExtraDataMgr.CheckPlanData() )
	{
		//MCS读取预案失败，视作无模板创会[11/22/2012 chendaiwei]
		if(MCS_CONF == m_tConf.GetConfSource())
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "[Modify]CheckPlanData failed!\n " );
		//cServMsg.SetErrorCode( 12 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}


	if( !m_cCfgInfoRileExtraDataMgr.IsReadPlan() )
	{
		//MCS读取预案失败，视作无模板创会[11/22/2012 chendaiwei]
		if(MCS_CONF == m_tConf.GetConfSource())
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "Plan Manager is not initialize.Nack\n " );
		//cServMsg.SetErrorCode( 12 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}

	TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
	if( m_cCfgInfoRileExtraDataMgr.ModifyPlanName( pbyOldPlanName,pbyNewlanName, m_byConfIdx,tConfAttrbEx.IsDefaultConf() ) )
	{
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	}
	else
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "Modify PlanName(%s) to PlanName occor error!!!.Nack\n ",
							pbyOldPlanName,pbyNewlanName );
		//cServMsg.SetErrorCode( 12 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyOldPlanName;
		delete []pbyNewlanName;
		return;
	}
	delete []pbyOldPlanName;
	delete []pbyNewlanName;

	//VCSNotifyAllPlanName();

	cServMsg.SetEventId( MCU_VCS_MODIFYPLANNAME_NOTIFY );
	SendMsgToAllMcs( MCU_VCS_MODIFYPLANNAME_NOTIFY,cServMsg );

	m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );

}

/*====================================================================
    函数名      ProcMcsVcsMcuDelPlanName
    功能        ：删除预案
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
	12/02/22				陈代伟			修改
====================================================================*/
void CMcuVcInst::ProcMcsVcsMcuDelPlanName( const CMessage *pcMsg )
{
	u16 wPrintModule = 0;
	BOOL32 bNotifyVCS = TRUE;

	if( MCS_CONF == m_tConf.GetConfSource())
	{
		wPrintModule = MID_MCU_MCS;
		bNotifyVCS = FALSE;
	}
	else
	{
		wPrintModule = MID_MCU_VCS;
	}

	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, wPrintModule, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	if( m_cVCSConfStatus.GetIsPlanDataOperation() )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "Plan is operating.Nack\n " );		
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANISMODIFYING );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	m_cVCSConfStatus.SetIsPlanDataOperation( TRUE );

	s8 *pbyPlanName = (s8*)cServMsg.GetMsgBody();

	u8 byStrLen = strlen( pbyPlanName );

	if( byStrLen > (VCS_MAXLEN_ALIAS - 1) || 0 == byStrLen)
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "planName(%s) Len is than 31 or is 0.Nack\n ",pbyPlanName );
		
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}

	if( !m_cCfgInfoRileExtraDataMgr.IsHasPlanName(pbyPlanName) )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "planName(%s) is not Exist.Nack\n ",pbyPlanName );
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNAMENOTEXIST ); //TBD 错误码是否需要更新  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}

	if( !m_cCfgInfoRileExtraDataMgr.CheckPlanData() )
	{
		//MCS读取预案失败，视作无模板创会[11/22/2012 chendaiwei]
		if(MCS_CONF == m_tConf.GetConfSource())
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "[Del]CheckPlanData failed\n " );
		//cServMsg.SetErrorCode( 12 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}

	if( !m_cCfgInfoRileExtraDataMgr.IsReadPlan() )
	{
		//MCS读取预案失败，视作无模板创会[11/22/2012 chendaiwei]
		if(MCS_CONF == m_tConf.GetConfSource())
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "Plan Manager is not initialize.Nack\n " );
		//cServMsg.SetErrorCode( 12 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}

	TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
	if( m_cCfgInfoRileExtraDataMgr.DelPlanName( pbyPlanName, m_byConfIdx,tConfAttrbEx.IsDefaultConf() ) )
	{
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	}
	else
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "DelPlanName(%s) occor error!!!.Nack\n ",pbyPlanName );
		//cServMsg.SetErrorCode( 12 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}

	NotifyAllPlanNameToConsole(bNotifyVCS);

	m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );

}

/*====================================================================
    函数名      ProcMcsVcsMcuAddPlanName
    功能        ：添加预案
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
	12/02/22				chendaiwei      modify
====================================================================*/
void CMcuVcInst::ProcMcsVcsMcuAddPlanName( const CMessage *pcMsg )
{
	u16 wModule = 0;
	BOOL32 bIsConsoleVCS = TRUE; //标识控制台是MCS还是VCS  [2/22/2012 chendaiwei]

	if( VCS_CONF == m_tConf.GetConfSource() )
	{
		 wModule = MID_MCU_VCS;
	}
	else
	{
		 wModule = MID_MCU_MCS;
		 bIsConsoleVCS = FALSE;
	}

	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, wModule, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	if( m_cVCSConfStatus.GetIsPlanDataOperation() )
	{
		ConfPrint(LOG_LVL_WARNING, wModule,  "Plan is operating.Nack\n " );		
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANISMODIFYING ); //TBD 是否需要新增ErrorCode  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	m_cVCSConfStatus.SetIsPlanDataOperation( TRUE );


	s8 *pbyBuf = (s8*)cServMsg.GetMsgBody();

	if( NULL == pbyBuf )
	{
		ConfPrint(LOG_LVL_WARNING, wModule,  "Add Plan Buff is NULL\n ");
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}
	
	u8 byStrLen = *pbyBuf;
	

	if( byStrLen > (VCS_MAXLEN_ALIAS - 1) || 0 == byStrLen)
	{
		ConfPrint(LOG_LVL_WARNING, wModule,  "planName Len(%d) is than 31 or is 0.Nack\n ",byStrLen );		
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNAMELENNOTVALID ); //TBD 是否需要新增ErrorCode  [2/22/2012 chendaiwei]
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}

	pbyBuf++;
	s8 *pbyPlanName = new s8[ byStrLen + 1 ];
	if( NULL == pbyPlanName )
	{
		ConfPrint(LOG_LVL_WARNING, wModule,  "Alloc memory pbyPlanName is error.\n " );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}
	memcpy( pbyPlanName,pbyBuf,byStrLen );
	pbyPlanName[byStrLen] = '\0';	


	if( 0 == strcmp( "",pbyPlanName ) ) 
	{
		ConfPrint(LOG_LVL_WARNING, wModule,  "pbyPlanName is empty!!!.\n " );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyPlanName;
		return;
	}
	
	if( byStrLen != strlen(pbyPlanName) ) 
	{
		ConfPrint(LOG_LVL_WARNING, wModule,  "pbyPlanName len is not expected,error!!!.<expectd.%d,acutal.%d>\n ",byStrLen,strlen(pbyPlanName) );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		delete []pbyPlanName;
		return;
	}

	pbyBuf += byStrLen;
	u8 byMtAliasNum = *pbyBuf;

	BOOL32 bIsAddPlanName = TRUE;//( 0 == byMtAliasNum  );

	ConfPrint(LOG_LVL_DETAIL, wModule,  "[ProcVcsMcuAddPlanName] planName(%s) Len(%d) MtALiasNum(%d)\n ",
		pbyPlanName,byStrLen,byMtAliasNum );

	if(!m_cCfgInfoRileExtraDataMgr.CheckPlanData())
	{
		//MCS读取预案失败，视作无模板创会[11/22/2012 chendaiwei]
		if(!bIsConsoleVCS)
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}
		ConfPrint(LOG_LVL_WARNING, wModule,  "[Add]checkPlandata failed!\n " );
		delete []pbyPlanName;
		//cServMsg.SetErrorCode( 11 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}
	
	if( !m_cCfgInfoRileExtraDataMgr.IsReadPlan() )
	{
		//MCS读取预案失败，视作无模板创会[11/22/2012 chendaiwei]
		if(!bIsConsoleVCS)
		{
			cServMsg.SetErrorCode(ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN);
		}
		ConfPrint(LOG_LVL_WARNING, wModule,  "Plan Manager is not initialize.Nack\n " );
		delete []pbyPlanName;
		//cServMsg.SetErrorCode( 11 );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;
	}

	if( m_cCfgInfoRileExtraDataMgr.IsHasPlanName(pbyPlanName) )
	{
		/*ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "planName(%s) is Exist.Nack\n ",pbyPlanName );
		delete []pbyPlanName;
		cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNAMEALREADYEXIST );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
		return;*/
		ConfPrint(LOG_LVL_KEYSTATUS, wModule,  "planName(%s) is Exist. so Save PlanData\n ",pbyPlanName );
		bIsAddPlanName = FALSE;
	}
	
	if( bIsAddPlanName )
	{	
		if( (bIsConsoleVCS && m_cCfgInfoRileExtraDataMgr.GetMtPlanNum() >= VCS_MAXNUM_PLAN )
			|| (!bIsConsoleVCS && m_cCfgInfoRileExtraDataMgr.GetMtPlanNum() >= MCS_MAXNUM_PLAN) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, wModule,  "Plan Number(%d) is equal or rather than MAXNUM_PLAN(%d).Nack\n ",
				m_cCfgInfoRileExtraDataMgr.GetMtPlanNum(),bIsConsoleVCS?VCS_MAXNUM_PLAN:MCS_MAXNUM_PLAN );
			delete []pbyPlanName;
			cServMsg.SetErrorCode( ERR_MCU_VCS_PLANNUMOVERFLOW ); //TBD 是否需要新增ErrorCode  [2/22/2012 chendaiwei]
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
			return;
		}
		
		TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
		if( m_cCfgInfoRileExtraDataMgr.AddPlanName( pbyPlanName, m_byConfIdx,tConfAttrbEx.IsDefaultConf()) )
		{
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}
		else
		{
			ConfPrint(LOG_LVL_KEYSTATUS, wModule,  "AddPlanName(%s) occor error!!!.Nack\n ",pbyPlanName );
			delete []pbyPlanName;
			//cServMsg.SetErrorCode( 11 );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
			return;
		}

		NotifyAllPlanNameToConsole(bIsConsoleVCS);
	}
	else//保存预案
	{
		if( !m_cCfgInfoRileExtraDataMgr.IsHasPlanName(pbyPlanName) )
		{
			ConfPrint(LOG_LVL_WARNING, wModule,  "planName(%s) is not Exist.Nack\n ",pbyPlanName );
			delete []pbyPlanName;
			//cServMsg.SetErrorCode( 11 );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
			return;
		}

		pbyBuf++;
		TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
		if( m_cCfgInfoRileExtraDataMgr.SaveMtPlanAliasByPlanName( pbyPlanName,
													(TMtVCSPlanAlias*)pbyBuf,
													byMtAliasNum, 
													m_byConfIdx,
													tConfAttrbEx.IsDefaultConf())
													)
		{
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, wModule,  "SaveMtPlanAliasByPlanName is occor error.Nack\n " );
			delete []pbyPlanName;
			//cServMsg.SetErrorCode( 16 );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	
			m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
			return;
		}
		
		NotifyOnePlanDataToConsole( pbyPlanName,MCU_VCS_ONEPLANDATA_NOTIFY,bIsConsoleVCS );
	}

	if(pbyPlanName)
	{
		delete []pbyPlanName;
		pbyPlanName = NULL;
	}

	m_cVCSConfStatus.SetIsPlanDataOperation( FALSE );
	

}

/*====================================================================
    函数名      NotifyAllPlanNameToConsole
    功能        ：通知界面所有现有的预案名字
    算法实现    ：
    引用全局变量：
    输入参数说明：[IN]BOOL32 bNotifyVCS为TRUE通知VCS，为FALSE通知MCS
				
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
	12/02/22				陈代伟          修改
====================================================================*/
void CMcuVcInst::NotifyAllPlanNameToConsole( BOOL32 bNotifyVCS )
{
	u16 wPrintModule = 0;

	if(bNotifyVCS)
	{
		wPrintModule = MID_MCU_VCS;
	}
	else
	{
		wPrintModule = MID_MCU_MCS;
	}

	CServMsg cMsg;
	TMtVcsPlanNames tVcsPlanNames = {0};
	
	s8 *pachPlanName[ VCS_MAXNUM_PLAN ];
	u8 idx = 0;
	for( idx = 0;idx < VCS_MAXNUM_PLAN;idx++ )
	{
		pachPlanName[idx] = &tVcsPlanNames.m_achAlias[idx][0];
	}
	
	u8 byPlanNum = 0;
	m_cCfgInfoRileExtraDataMgr.GetAllPlanName( (s8**)pachPlanName,byPlanNum );
	tVcsPlanNames.m_byPlanNum = byPlanNum;
	
	for( idx = 0;idx < byPlanNum;idx++)
	{
		ConfPrint(LOG_LVL_DETAIL, wPrintModule,  "PlanName%d:(%s)\n",idx+1,&tVcsPlanNames.m_achAlias[idx][0] );
	}

	cMsg.SetMsgBody( (u8*)&tVcsPlanNames,sizeof( tVcsPlanNames ) );

	SendMsgToAllMcs( MCU_VCS_ALLPLANNAME_NOTIFY,cMsg );

}

/*====================================================================
    函数名		  NotifyOnePlanDataToConsole
    功能        ：通知界面某个预案的数据
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
				  [IN]BOOL32 bNotifyVCS 为TRUE通知VCS，为FALSE通知MCS
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
	12/02/22				陈代伟			修改
====================================================================*/
void CMcuVcInst::NotifyOnePlanDataToConsole( s8* pbyPlanName,u16 wEvent,BOOL32 bNotifVCS )
{
	/*lint -save -esym(429, pbyPlanName)*/

	u16 wPrintModule = 0;
	if(bNotifVCS)
	{
		wPrintModule = MID_MCU_VCS;
	}
	else
	{
		wPrintModule = MID_MCU_MCS;
	}

	if( !m_cCfgInfoRileExtraDataMgr.IsHasPlanName( pbyPlanName ) )
	{
		ConfPrint(LOG_LVL_WARNING, wPrintModule,  "pbyPlanName(%s) is not exists,can't notify!!!\n",pbyPlanName );
		return;
	}

	u8 byMtPlanAliasNum = m_cCfgInfoRileExtraDataMgr.GetMtPlanAliasNumByPlanName( pbyPlanName );

	
	ConfPrint(LOG_LVL_DETAIL, wPrintModule,  "byMtPlanAliasNum is %d.!!!\n",byMtPlanAliasNum );
	
 
	TMtVCSPlanAlias *pMtPlanAlias = NULL;
	if( byMtPlanAliasNum > 0 )
	{
		pMtPlanAlias= new TMtVCSPlanAlias[ byMtPlanAliasNum ];

		if( NULL == pMtPlanAlias )
		{
			ConfPrint(LOG_LVL_WARNING, wPrintModule,  "apply memory error,can't notify!!!\n" );
			return;
		}

		if( !m_cCfgInfoRileExtraDataMgr.GetMtPlanAliasByPlanName( pbyPlanName,pMtPlanAlias,byMtPlanAliasNum ) )
		{
			ConfPrint(LOG_LVL_WARNING, wPrintModule,  "GetMtPlanAliasByPlanName occor error,can't notify!!!\n" );
			delete []pMtPlanAlias;
			return;
		}
	}
	

	CServMsg cServMsg;	
	//CConfId tConfId = m_tConf.GetConfId();
	//cServMsg.SetMsgBody( (u8*)&tConfId,sizeof( tConfId ) );
	u8 byStrLen = strlen( pbyPlanName );
	cServMsg.SetMsgBody( &byStrLen,sizeof(byStrLen) );
	cServMsg.CatMsgBody( (u8*)pbyPlanName,byStrLen );
	cServMsg.CatMsgBody( &byMtPlanAliasNum,sizeof( byMtPlanAliasNum) );
	if( byMtPlanAliasNum > 0 )
	{
		cServMsg.CatMsgBody( (u8*)pMtPlanAlias,sizeof( TMtVCSPlanAlias ) * byMtPlanAliasNum );
	}	

	SendMsgToAllMcs(wEvent, cServMsg);

	if( NULL != pMtPlanAlias )
	{
		delete []pMtPlanAlias;
	}	

	/*lint -restore*/
}

/*====================================================================
    函数名      VCSClearMtDisconnectReason
    功能        ：清空终端的离线原因
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/05/06                周晶晶          创建
====================================================================*/
void CMcuVcInst::VCSClearMtDisconnectReason( TMt tMt )
{
	//只处理本地的终端和下级mcu
	if( !tMt.IsLocal() )
	{
		return;
	}


	TMtStatus tMtStatus;
	if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{	
		if( MT_TYPE_MT == m_ptMtTable->GetMtType( tMt.GetMtId() ) )
		{
			if( m_ptMtTable->GetMtStatus(tMt.GetMtId(),&tMtStatus) )
			{
				tMtStatus.SetMtDisconnectReason( MTLEFT_REASON_NORMAL );
				m_ptMtTable->SetMtStatus( tMt.GetMtId(),&tMtStatus );
			}
			MtStatusChange( &tMt,TRUE );
		}
		else if( MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) )
		{
			TConfMcInfo *ptMcInfo = NULL;
			TConfMtInfo *ptConfMtInfo = NULL;
			u16 wMcuIdx = GetMcuIdxFromMcuId( tMt.GetMtId() );
			ptMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );
			if( NULL == ptMcInfo )
			{
				return;
			}
			ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr( wMcuIdx );//tMt.GetMtId() ); 
			if( NULL == ptConfMtInfo )
			{
				return;
			}
			u8 byMtId = 0;
			while( byMtId < MAXNUM_CONF_MT )
			{				
				if( ptConfMtInfo->MtInConf( byMtId ) )
				{
					ptMcInfo->m_atMtStatus[byMtId].SetMtDisconnectReason( MTLEFT_REASON_NORMAL );
					MtStatusChange( (TMt*)&(ptMcInfo->m_atMtStatus[byMtId]));
				}											
				++byMtId;
			}	
				
		}
	}
	else
	{
		if( m_ptMtTable->GetMtStatus(tMt.GetMtId(),&tMtStatus) )
		{
			tMtStatus.SetMtDisconnectReason( MTLEFT_REASON_NORMAL );
			m_ptMtTable->SetMtStatus( tMt.GetMtId(),&tMtStatus );
		}
		MtStatusChange( &tMt,TRUE );
	}
}

/*====================================================================
    函数名      VCSClearTvWallChannelByMt
    功能        ：清空某个电视墙中的某个终端
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
    11/05/27                刘旭            重构
====================================================================*/
void CMcuVcInst::VCSClearTvWallChannelByMt( TMt tMt, BOOL32 bOnlyClearTvWallStatus /*= FALSE*/ )
{
	// 一个会议支持的电视墙通道的总数
#define MAX_TVW_CHNNL_NUM  (u8)(MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL + MAXNUM_PERIEQP_CHNNL * MAXNUM_MAP)

	CConfTvwChnnl acTvwChnnlFind[MAX_TVW_CHNNL_NUM];					// 待寻找的电视墙通道
	u8		abyTvwIdFind[MAX_TVW_CHNNL_NUM];							// 存储找到的电视墙设备的ID
	memset(abyTvwIdFind, 0, sizeof(abyTvwIdFind));
	
	const u8 byFindNum = FindAllTvwChnnl(tMt, acTvwChnnlFind, MAX_TVW_CHNNL_NUM);

	u8 byTvwEqpId, byTvwChnnlIdx;								// 找到的电视墙设备号和通道号
	TPeriEqpStatus tStatus;

	for (u8 byLoop = 0; byLoop < byFindNum; ++byLoop)
	{
		if (acTvwChnnlFind[byLoop].IsNull())
		{
			continue;
		}

		byTvwEqpId = acTvwChnnlFind[byLoop].GetEqpId();
		byTvwChnnlIdx = acTvwChnnlFind[byLoop].GetChnnlIdx();

		if (!g_cMcuVcApp.IsPeriEqpConnected(byTvwEqpId))
		{
			continue;
		}

		// 是Hdu设备
		if (IsValidHduChn(byTvwEqpId, byTvwChnnlIdx))
		{
			// [2013/03/11 chenbing] VCS会议不支持HDU多画面,子通道置0
			// Bug00154723：vcs单方调度时调度一个在别的会议中的终端进墙，取消强拆，终端未进墙但是界面显示终端别名
			ChangeHduSwitch( NULL,
				byTvwEqpId,
				byTvwChnnlIdx,
				0/*chenbing*/, 
				acTvwChnnlFind[byLoop].GetMember().byMemberType,
				TW_STATE_STOP,
				MODE_BOTH, FALSE, FALSE);
		}
		// 是普通电视墙设备
		else
		{
			// 停交换
			ChangeTvWallSwitch(&tMt,
				byTvwEqpId,
				byTvwChnnlIdx,
				acTvwChnnlFind[byLoop].GetMember().byMemberType,
				TW_STATE_STOP);
		}
		
		// [11/11/2011 liuxu] 电视墙轮询时, 不应该完全清空通道
		// 本来应该放到ChangeHduSwitch里处理的, 但是保险起见, 先这里处理
		if ( TW_MEMBERTYPE_TWPOLL == acTvwChnnlFind[byLoop].GetMember().byMemberType )
		{
			if( g_cMcuVcApp.GetPeriEqpStatus( byTvwEqpId, &tStatus ) )
			{
				if (IsValidHduChn(byTvwEqpId, byTvwChnnlIdx))
				{
					tStatus.m_tStatus.tHdu.atVideoMt[byTvwChnnlIdx].byMemberType = acTvwChnnlFind[byLoop].GetMember().byMemberType;
					tStatus.m_tStatus.tHdu.atVideoMt[byTvwChnnlIdx].SetConfIdx(m_byConfIdx);
				}
				else
				{
					tStatus.m_tStatus.tTvWall.atVideoMt[byTvwChnnlIdx].byMemberType = acTvwChnnlFind[byLoop].GetMember().byMemberType;
					tStatus.m_tStatus.tTvWall.atVideoMt[byTvwChnnlIdx].SetConfIdx(m_byConfIdx);
				}
				
				g_cMcuVcApp.SetPeriEqpStatus( byTvwEqpId, &tStatus);
				
				// 马上开启下一次轮询
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byTvwEqpId, byTvwChnnlIdx, dwTimerIdx) )
				{
					KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
					CMessage cMsg;
					memset(&cMsg, 0, sizeof(cMsg));
					cMsg.event = u16(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
					cMsg.content = (u8*)&dwTimerIdx;
					cMsg.length  = sizeof(u32);
					ProcTWPollingChangeTimerMsg(&cMsg);
				}
			}
		}

		// 依次将Hdu Id填入abyTvwIdFind中
		for ( u8 byCounter = 0; byCounter < MAX_TVW_CHNNL_NUM; ++byCounter)
		{
			if ( 0 == abyTvwIdFind[byCounter] )
			{
				abyTvwIdFind[byCounter] = byTvwEqpId;
				break;
			}

			if ( byTvwEqpId == abyTvwIdFind[byCounter] )
			{
				break;
			}
		}
	}
	
	if( !(m_cVCSConfStatus.GetCurVCMT() == tMt) 
		&& m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE 
		&& !m_ptMtTable->IsMtInTvWall( GetLocalMtFromOtherMcuMt(tMt).GetMtId() ) 
		&& !m_ptMtTable->IsMtInHdu( GetLocalMtFromOtherMcuMt(tMt).GetMtId() ))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[VCSClearTvWallChannelByMt] cur mode is VCS_SINGLE_MODE VCSDropMT is mt in hdu(%d) mtconfindex(%d) conidx(%d) curVCMT(%d.%d)\n",
			m_ptMtTable->IsMtInTvWall( GetLocalMtFromOtherMcuMt(tMt).GetMtId() ) ,
			tMt.GetConfIdx(),m_byConfIdx,
			m_cVCSConfStatus.GetCurVCMT().GetMcuId(),
			m_cVCSConfStatus.GetCurVCMT().GetMtId()
			);
		
		VCSDropMT( tMt );
	}

	if( !bOnlyClearTvWallStatus &&
		m_cVCSConfStatus.GetMtInTvWallCanMixing() && 
		m_tConf.m_tStatus.IsMixing() && m_tConf.m_tStatus.IsSpecMixing() &&
		VCSMtNotInTvWallCanStopMixing(tMt) )
	{
		RemoveSpecMixMember( &tMt, 1, FALSE, TRUE );
	}
	
	// 上报Tvwall设备通道状态变化
	u8 byNext = 0;
	CServMsg cServMsg;
	while ( byNext < MAX_TVW_CHNNL_NUM && 0 != abyTvwIdFind[byNext] )
	{
		u8 byTvwId = abyTvwIdFind[byNext];
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  
			"[VCSClearTvWallChannelByMt] notify HDU eqp(id:%u) status\n", 
			byTvwId );

		if( g_cMcuVcApp.GetPeriEqpStatus( byTvwId, &tStatus ) )
		{
			cServMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
			SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
		}

		++byNext;
	}
}

/*====================================================================
    函数名      VCSClearAllTvWallChannel
    功能        ：清空所有电视墙通道并拆除到电视墙的通道
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
    11/05/27                刘旭            重构
====================================================================*/
void CMcuVcInst::VCSClearAllTvWallChannel( const TSwitchInfo *ptSwitchInfo,const u8 byNum )
{
	// 一个会议支持的电视墙通道的总数
	const u8 byMaxTvwChnnlNum = MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL + MAXNUM_PERIEQP_CHNNL * MAXNUM_MAP;
	CConfTvwChnnl acTvwChnnlFind[byMaxTvwChnnlNum];		// 待寻找的电视墙通道
	u8 byEqpId, byChnnlIdx;							// 电视墙通道对应的设备号和在设备中的通道号
	
	// 用于判断是否要清空通道. 如果要清空的通道中的终端恰好是又需要进入的终端, 则此通道不必清空
	BOOL32 bCanStopSwitch = TRUE;	
	TMt tDropMt;									// 待退出电视墙通道的终端
	const TSwitchInfo* ptTvWallSwitchInfo = NULL;	// 便于多次遍历ptSwitchInfo

	// 获取所有电视墙通道
	const u8 byTvwChnnlNum = GetAllCfgedTvwChnnl( acTvwChnnlFind, byMaxTvwChnnlNum );

	// 依次遍历获取得到的每个电视墙通道进行清除工作
	for ( u8 byLoop = 0; byLoop < byTvwChnnlNum; ++byLoop )
	{
		// 稳妥点判断是否为空
		if (acTvwChnnlFind[byLoop].IsNull())
		{
			continue;
		}

		byEqpId = acTvwChnnlFind[byLoop].GetEqpId();
		byChnnlIdx = acTvwChnnlFind[byLoop].GetChnnlIdx();

		// [12/28/2011 liuxu] 不在线, 也要进行操作, 以避免电视墙掉线时仍有终端在墙内,
		// 电视墙重新上线后电视墙模式发生改变时, 又把终端呼入墙内

		// 判断成员类型是否是vcs会议中的类型, 以及是否是被此会议占用
		if( !IsVcsTvwMemberType(acTvwChnnlFind[byLoop].GetMember().byMemberType) 
			&& acTvwChnnlFind[byLoop].GetMember().GetConfIdx() != m_byConfIdx )
		{
			continue;
		}

		// [6/8/2011 liuxu] 如果该电视墙通道在轮询, 必须停止轮询
		u8 byPollState = POLL_STATE_NONE;
		if( m_tTWMutiPollParam.GetPollState(byEqpId, byChnnlIdx, byPollState) && POLL_STATE_NONE != byPollState)
		{
			// [10/27/2011 liuxu] 如果在轮询, 需要停轮询
			m_tTWMutiPollParam.SetPollState(byEqpId, byChnnlIdx, POLL_STATE_NONE);	

			//获取定时器索引
			u32 dwTimerIdx = 0;
			if(!m_tTWMutiPollParam.GetTimerIdx(byEqpId, byChnnlIdx, dwTimerIdx))
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[ProcMcsMcuHduPollMsg] Cann't get tvw<EqpId:%d, ChnId:%d> TimerIdx!\n", byEqpId, byChnnlIdx);
				continue;
			}

			// 杀死定时器
			KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);	
		}
		
		// 重置bCanStopSwitch
		bCanStopSwitch = TRUE;

		// 根据待进入的终端信息确认是否要真正清空此通道
		ptTvWallSwitchInfo = ptSwitchInfo;
		if( NULL != ptTvWallSwitchInfo 
			&& byNum > 0 
			&& acTvwChnnlFind[byLoop].GetMember().IsLocal())
		{
			for( u8 byIdx = 0; byIdx < byNum; byIdx++, ptTvWallSwitchInfo++ )
			{							
				if( ptTvWallSwitchInfo->GetSrcMt() == (TMt)acTvwChnnlFind[byLoop].GetMember()
					&& byEqpId == ptTvWallSwitchInfo->GetDstMt().GetEqpId() 
					&& ptTvWallSwitchInfo->GetDstChlIdx() == byChnnlIdx )
				{
					// 如果要清空的通道中的终端恰好是又需要进入的终端, 则此通道不必清空
					bCanStopSwitch = FALSE;
					break;
				}
			}
		}

		// 需要拆除的继续拆除
		if( bCanStopSwitch )
		{	
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[VCSClearAllTvWallChannel] dorp mt(%d,%d)\n",
				acTvwChnnlFind[byLoop].GetMember().GetMcuId(),
				acTvwChnnlFind[byLoop].GetMember().GetMtId() );
			
			tDropMt = (TMt)acTvwChnnlFind[byLoop].GetMember();

			// hdu电视墙处理
			if (IsValidHduChn(byEqpId, byChnnlIdx))
			{
				// [2013/03/11 chenbing] VCS会议不支持HDU多画面,子通道置0
				ChangeHduSwitch(NULL,
					byEqpId,
					byChnnlIdx,
					0, 
					acTvwChnnlFind[byLoop].GetMember().byMemberType, 
					TW_STATE_STOP,
					MODE_BOTH,
					FALSE,
					m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE ? TRUE:FALSE
				);
			}
			// 普通电视墙处理
			else 
			{
				ChangeTvWallSwitch(&tDropMt,
					byEqpId,
					byChnnlIdx,
					acTvwChnnlFind[byLoop].GetMember().byMemberType,
					TW_STATE_STOP);
			}
			
			// 终端被清出电视墙时, 要考虑挂断挂断它
			if (!(m_cVCSConfStatus.GetCurVCMT() == tDropMt ) 
				&& !m_ptMtTable->IsMtInHdu( GetLocalMtFromOtherMcuMt(tDropMt).GetMtId() ) 
				&& !m_ptMtTable->IsMtInTvWall( GetLocalMtFromOtherMcuMt(tDropMt).GetMtId()))
			{
				// 单方调度下需要挂断
				if (m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ClearOneTvWallChannel] cur mode is VCS_SINGLE_MODE VCSDropMT mt is in hdu(%d)\n",
						m_ptMtTable->IsMtInHdu( GetLocalMtFromOtherMcuMt(tDropMt).GetMtId() ) 
						);
					
					// 挂断终端
					VCSDropMT( tDropMt );
					
				}
				// 多方多画面下如果也不在画面合成中了, 也需要被挂断
				else if ( m_cVCSConfStatus.GetCurVCMode() == VCS_MULVMP_MODE )
					
				{
					TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
					if (!tConfVmpParam.IsMtInMember(tDropMt))
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ClearTvw(%d,%d)] DropMT(%d,%d) in VcsMode.%d\n",
							byEqpId, byChnnlIdx, tDropMt.GetMcuId(), tDropMt.GetMtId(),  m_cVCSConfStatus.GetCurVCMode());
						
						VCSDropMT( tDropMt );
					}
				}
			}
		}
	}
}

/*====================================================================
    函数名      VCSChangeTvWallMtMixStatus
    功能        ：开启或关闭电视墙混音状态
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
    11/05/27                刘旭            重构
====================================================================*/
void CMcuVcInst::VCSChangeTvWallMtMixStatus( BOOL32 bIsStartMixing )
{
	TEqp tHduEqp;
	tHduEqp.SetNull();	

	if( bIsStartMixing &&
		m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE )
	{	
		if( !m_tConf.m_tStatus.IsMixing() )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[VCSChangeTvWallMtMixStatus] StartMixing(mcuPartMix)!\n" );
			
			if (!StartMixing(mcuPartMix))
			{
				CServMsg cServMsg;
				cServMsg.SetConfIdx(m_byConfIdx);
				cServMsg.SetConfId(m_tConf.GetConfId());

				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[VCSChangeTvWallMtMixStatus] Find no mixer\n");
				
				if (ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_NOMIXSPEAKER);
				}
				else
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_NOMIXER);
				}
				m_cVCSConfStatus.SetMtInTvWallCanMixing( FALSE );
				SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cServMsg);
				return;
			}
			else
			{
				TMt tMt = m_tConf.GetChairman();
				VCSConfStopMTSel( tMt,MODE_AUDIO );
				m_tConf.m_tStatus.SetSpecMixing();
				ChangeSpecMixMember(&tMt, 1);
				
				if( !m_cVCSConfStatus.GetCurVCMT().IsNull() )
				{
					tMt = m_cVCSConfStatus.GetCurVCMT();
					VCSConfStopMTSel( tMt,MODE_AUDIO );						
					AddSpecMixMember( &tMt,1,TRUE );
				}
			}
		}
		else
		{
			TMt tMt = GetLocalMtFromOtherMcuMt( m_tConf.GetChairman() );
			VCSConfStopMTSel( tMt,MODE_AUDIO );
			if( !m_ptMtTable->IsMtInMixing( tMt.GetMtId() ) )
			{
				AddSpecMixMember( &tMt,1,TRUE );
			}
			
			if( !m_cVCSConfStatus.GetCurVCMT().IsNull() )
			{
				tMt = m_cVCSConfStatus.GetCurVCMT();
				VCSConfStopMTSel( tMt,MODE_AUDIO );
				AddSpecMixMember( &tMt,1,TRUE );
			}
		}
	}
	
	// 5/27/2011 liuxu] 进行整理, 消除了电视墙和tvwall的冗余处理
	// 一个会议支持的电视墙通道的总数
	const u8 byMaxTvwChnnlNum = MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL + MAXNUM_PERIEQP_CHNNL * MAXNUM_MAP;
	CConfTvwChnnl acTvwChnnlFind[byMaxTvwChnnlNum];		// 待寻找的电视墙通道
	
	TTvwMember tTvwMember;							// 电视墙通道成员
	u8 byMixMemNum = 0;								// 参与混音的成员个数
	CServMsg cMixMemServMsg;						
	TMt tLocalMt;
	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	// 获取所有电视墙通道
	const u8 byTvwChnnlNum = GetAllCfgedTvwChnnl( acTvwChnnlFind, byMaxTvwChnnlNum );
	for ( u8 byLoop = 0; byLoop < byTvwChnnlNum; ++byLoop )
	{
		tTvwMember = acTvwChnnlFind[byLoop].GetMember();
		if( !IsVcsTvwMemberType( tTvwMember.byMemberType ) 
			|| ( tTvwMember.GetConfIdx() && tTvwMember.GetConfIdx() != m_byConfIdx) )
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "The chnnl Member of Conf Hdu Chnnl.%d is invalid\n", byLoop);
			continue;
		}

		TMt tMt = (TMt)tTvwMember;
		if( FALSE == bIsStartMixing )
		{	
			if( VCSMtInTvWallCanStopMixing(tMt) )
			{
				RemoveSpecMixMember( &tMt, 1, FALSE, TRUE );
			}
			tLocalMt = GetLocalMtFromOtherMcuMt(tMt);
			//级联单回传下进墙终端如果非调度终端同时也不在画面合成中，则将下级MCU踢出混音器，以免下级未配混音器时还能听到下级上传通道的声音
			if (!tMt.IsLocal() && !IsLocalAndSMcuSupMultSpy(tMt.GetMcuIdx()) && 
				m_ptMtTable->IsMtInMixing(tLocalMt.GetMtId()) &&
				GetLocalMtFromOtherMcuMt(m_cVCSConfStatus.GetCurVCMT()).GetMtId() != tLocalMt.GetMtId() && 
				!tConfVmpParam.IsMtInMember(tMt))
			{
				RemoveSpecMixMember( &tLocalMt, 1, FALSE, TRUE );
			}
			//单方调度模式下，取消一键混音后，在电视墙中终端就听主席
			/*if( !m_tConf.m_tStatus.IsMixing() &&
				m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE )
			{
				GoOnSelStep( tMt, MODE_AUDIO, TRUE );
			}*/
		}
		else if( bIsStartMixing )
		{
			// [12/1/2011 liuxu] 如果终端只是显示在墙里, 而没有真正建交换, 则不参与混音
			if ( 0 == g_cMcuVcApp.GetChnnlMMode(acTvwChnnlFind[byLoop].GetEqpId(), acTvwChnnlFind[byLoop].GetChnnlIdx()) 
				|| !m_tConfAllMtInfo.MtJoinedConf(tMt))
			{
				ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "The chnnl(%d,%d)'s MODE Is 0\n",
				 acTvwChnnlFind[byLoop].GetEqpId(),acTvwChnnlFind[byLoop].GetChnnlIdx());
				continue;
			}

			if (byMixMemNum == 0)
			{
				cMixMemServMsg.SetMsgBody((u8*)&tMt,sizeof(TMt));
			}
			else
			{
				cMixMemServMsg.CatMsgBody((u8*)&tMt,sizeof(TMt));
			}
			
			byMixMemNum ++;
		}
	}

	if (bIsStartMixing)
	{
		TMt *ptMt = (TMt*)(cMixMemServMsg.GetMsgBody());
		u8 bymixnum = cMixMemServMsg.GetMsgBodyLen() / sizeof(TMt);
		AddSpecMixMember(ptMt, byMixMemNum, FALSE);
	}

	if( !bIsStartMixing && m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE )
	{
		if( m_tConf.m_tStatus.IsMixing() )
		{
			//在这里由于时序问题，所以要等到混音器回停止响应候再执行这个函数停止所有的电视墙终端的混音
			StopMixing();
			return;
		}
		//主席选看当前终端
		if( !m_cVCSConfStatus.GetCurVCMT().IsNull() )
		{
			GoOnSelStep( m_cVCSConfStatus.GetCurVCMT(),MODE_AUDIO,FALSE );
			GoOnSelStep( m_cVCSConfStatus.GetCurVCMT(),MODE_AUDIO,TRUE );
		}
	}
}

/*====================================================================
    函数名      VCSCanMtNotInTvWallStopMixing
    功能        ：进电视墙通终端退出电视墙时是否可以退出混音
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
====================================================================*/
BOOL32 CMcuVcInst::VCSMtNotInTvWallCanStopMixing( TMt tMt )
{
	if( tMt.IsNull() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt IsNull.Can't stop mixing \n" );
		return FALSE;
	}	

	if( m_tConf.GetChairman() == tMt )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt is chairman .Can't stop mixing \n" );
		return FALSE;
	}

	if( m_cVCSConfStatus.GetCurVCMT() == tMt )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt Is CurVcmt.Can't stop mixing \n" );
		return FALSE;
	}	
	
	if( tMt.IsLocal() &&
		!m_ptMtTable->IsMtInMixing( tMt.GetMtId() ) ) 
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt Is not in mixing.Can't stop mixing \n" );
		return FALSE;
	}	

	TMt tLocalMt = GetLocalMtFromOtherMcuMt(tMt);

	if( tMt.IsLocal() &&
		(m_ptMtTable->IsMtInTvWall( tMt.GetMtId() ) || m_ptMtTable->IsMtInHdu( tMt.GetMtId() ))
		)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt Is in tvwall(%d) or in hdu(%d).Can't stop mixing \n",
			m_ptMtTable->IsMtInTvWall( tMt.GetMtId() ),
			m_ptMtTable->IsMtInHdu( tMt.GetMtId() ));
		return FALSE;
	}

	if( !tMt.IsLocal() && VCS_TVWALLMANAGE_REVIEW_MODE != m_cVCSConfStatus.GetTVWallManageMode() )
	{
		TPeriEqpStatus tStatus;
		u8 byEqpId = HDUID_MIN;
		while ( byEqpId <= HDUID_MAX  )
		{					
			memset( &tStatus,0,sizeof(tStatus) );
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
			if (1 == tStatus.m_byOnline)
			{			
				u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byEqpId));
				if (0 == byHduChnNum)
				{
					byEqpId++;
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "GetHduChnNumAcd2Eqp failed!\n");
					continue;
				}

				for(u8 byLoop = 0; byLoop < byHduChnNum; byLoop++)
				{
					if( (TMt)tStatus.m_tStatus.tHdu.atVideoMt[byLoop] == tMt )
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt Is in hdu(%d) chnl(%d).Can't stop mixing \n",
							byEqpId,byLoop );
						return FALSE;
					}					
				}
			}		
			byEqpId++;
		}

		byEqpId=TVWALLID_MIN;
		while (  byEqpId < TVWALLID_MAX )
		{					
			memset( &tStatus,0,sizeof(tStatus) );
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
			if (1 == tStatus.m_byOnline)
			{					
				u8 byMemberNum = tStatus.m_tStatus.tTvWall.byChnnlNum;
				for(u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
				{
					if ( (TMt)tStatus.m_tStatus.tTvWall.atVideoMt[byLoop] == tMt )
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt Is in tvwall(%d) chnl(%d).Can't stop mixing \n",
							byEqpId,byLoop );
						return FALSE;
					}
				}
			}
			byEqpId++;
		}			
	}

	if( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE ||
		m_cVCSConfStatus.GetCurVCMode() == VCS_MULVMP_MODE )
	{
		TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
		u8 byVmpChnlIdx = tVmpParam.GetChlOfMtInMember( tMt );		
		if( byVmpChnlIdx > 0 && byVmpChnlIdx < MAXNUM_VMP_MEMBER )
		{
			if(  tVmpParam.IsMtInMember( tMt ) )
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "param tMt Is vmp channel(%d).Can't stop mixing \n", byVmpChnlIdx);
				return FALSE;
			}
			else
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "param tMt Is vmp channel(%d).but not in mtmember \n", byVmpChnlIdx);
			}			
		}
	}

	return TRUE;
}
/*====================================================================
    函数名      IsHasMtInHduOrTwByMcuIdx
    功能        ：电视墙通道中是否存在某个MCU下的终端
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    12/12/14                田志勇          创建
====================================================================*/
BOOL32 CMcuVcInst::IsHasMtInHduOrTwByMcuIdx(u16 wSmcuIdx)
{
	if( wSmcuIdx == INVALID_MCUIDX )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[IsHasMtInHduOrTwByMcuIdx]wSmcuIdx == INVALID_MCUIDX,So Return!\n" );
		return FALSE;
	}	
	u8 byMtId = GetFstMcuIdFromMcuIdx(wSmcuIdx);
	TPeriEqpStatus tStatus;
	u8 byEqpId = HDUID_MIN;
	while ( byEqpId <= HDUID_MAX  )
	{					
		memset( &tStatus,0,sizeof(tStatus) );
		g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
		if (1 == tStatus.m_byOnline)
		{			
			u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byEqpId));
			if (0 == byHduChnNum)
			{
				byEqpId++;
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "GetHduChnNumAcd2Eqp failed!\n");
				continue;
			}

			for(u8 byLoop = 0; byLoop < byHduChnNum; byLoop++)
			{
				if( GetLocalMtFromOtherMcuMt((TMt)tStatus.m_tStatus.tHdu.atVideoMt[byLoop]).GetMtId() == byMtId )
				{
					if ( 0 == g_cMcuVcApp.GetChnnlMMode(byEqpId, byLoop) )
					{
						continue;
					}
					else
					{
						return TRUE;
					}
				}					
			}
		}		
		byEqpId++;
	}

	byEqpId=TVWALLID_MIN;
	while (  byEqpId < TVWALLID_MAX )
	{					
		memset( &tStatus,0,sizeof(tStatus) );
		g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
		if (1 == tStatus.m_byOnline)
		{					
			u8 byMemberNum = tStatus.m_tStatus.tTvWall.byChnnlNum;
			for(u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
			{
				if ( GetLocalMtFromOtherMcuMt((TMt)tStatus.m_tStatus.tTvWall.atVideoMt[byLoop]).GetMtId() == byMtId )
				{
					if ( 0 == g_cMcuVcApp.GetChnnlMMode(byEqpId, byLoop) )
					{
						continue;
					}
					else
					{
						return TRUE;
					}
				}
			}
		}
		byEqpId++;
	}	
	return FALSE;
}
/*====================================================================
    函数名      VCSMtInTvWallCanStopMixing
    功能        ：在电视墙中的终端是否可以停止混音
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
====================================================================*/
BOOL32 CMcuVcInst::VCSMtInTvWallCanStopMixing( TMt tMt )
{
	if( tMt.IsNull() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "param tMt IsNull.Can't stop mixing \n" );
		return FALSE;
	}	
	//
	if( tMt.IsLocal() &&
		!m_ptMtTable->IsMtInMixing( tMt.GetMtId() ) ) 
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "param tMt Is not in mixing.Can't stop mixing \n" );
		return FALSE;
	}

	if( m_tConf.GetChairman() == tMt )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "param tMt Is ChairMan.Can't stop mixing \n" );
		return FALSE;
	}

	if( m_cVCSConfStatus.GetCurVCMT() == tMt )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "param tMt Is CurVcmt.Can't stop mixing \n" );
		return FALSE;
	}

	TMtStatus tStatus;
	if ( !GetMtStatus(tMt, tStatus) 
		|| !tStatus.IsInMixing() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "tmt(%d,%d) is not in mixing\n" ,tMt.GetMcuIdx(),tMt.GetMtId());
		return FALSE;
	}			

	TMt tLocalMt = GetLocalMtFromOtherMcuMt(tMt);

	if( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE ||
		m_cVCSConfStatus.GetCurVCMode() == VCS_MULVMP_MODE)
	{
		TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
		u8 byVmpChnlIdx = tVmpParam.GetChlOfMtInMember( tMt );		
		if( byVmpChnlIdx > 0 && byVmpChnlIdx < MAXNUM_VMP_MEMBER )
		{
			if( tVmpParam.IsMtInMember( tMt ) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "param tMt Is in vmp channel(%d).Can't stop mixing \n", byVmpChnlIdx);
				return FALSE;
			}
			else
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[VCSMtInTvWallCanStopMixing] param tMt Is in vmp channel(%d). but not in mtmember\n", byVmpChnlIdx);
			}
		}
	}

	return TRUE;
}

/*====================================================================
    函数名      SetSingleMtInTW
    功能        ：手动模式中的终端进电视墙
    算法实现    ：
    引用全局变量：
    输入参数说明：	TSwitchInfo ptSwitchInfo	[i]
					const CMessage &cServMsg	[i]
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    11/4/27                 xueliang        创建
    11/05/27                刘旭            重构
====================================================================*/
void CMcuVcInst::SetSingleMtInTW(TSwitchInfo *ptSwitchInfo, CServMsg &cServMsg)
{
	if ( NULL == ptSwitchInfo )
	{
		return;
	}

	TMt tOrgSrcMt = ptSwitchInfo->GetSrcMt();
	TMt tDstMt = ptSwitchInfo->GetDstMt();
	u8 byDstChannelIdx = ptSwitchInfo->GetDstChlIdx();
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] tSrcMt(%d.%d) EqpType(%d) EqpId(%d) byDstChlIdx(%d)\n",
		tOrgSrcMt.GetMcuId(),tOrgSrcMt.GetMtId(),tDstMt.GetEqpType(),
		tDstMt.GetEqpId(),byDstChannelIdx );
	

	//当下级有当前调度终端，且同一个下级的其它终端想进电视墙，回NACK
	if( !tOrgSrcMt.IsLocal() 
		&& !IsLocalAndSMcuSupMultSpy(tOrgSrcMt.GetMcuId()) 
		&& IsMtInMcu(GetLocalMtFromOtherMcuMt(tOrgSrcMt), m_cVCSConfStatus.GetCurVCMT()) 
		&& !( m_cVCSConfStatus.GetCurVCMT() == tOrgSrcMt ))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] CurVCMT(%d.%d) and Mt(%d.%d) is in same smcu,Mt can't in tvwall \n ",
			m_cVCSConfStatus.GetCurVCMT().GetMcuId(),
			m_cVCSConfStatus.GetCurVCMT().GetMtId(),
			tOrgSrcMt.GetMcuId(),
			tOrgSrcMt.GetMtId() );
		
		cServMsg.SetErrorCode( ERR_MCU_VCS_NOUSABLEBACKCHNNL );
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		
		return;
	}		
		
	/*u8 byVmpChnlIdx = m_tConf.m_tStatus.GetVmpParam().GetChlOfMtInMember( tOrgSrcMt );
	//组呼画面合成模式下，当下级已经有终端进入画面合成，那么此下级其它终端不能进电撒墙
	TVMPParam tvmpParam = m_tConf.m_tStatus.GetVmpParam();
	if( !tOrgSrcMt.IsLocal() 
		&& !IsLocalAndSMcuSupMultSpy(tOrgSrcMt.GetMcuId()) 
		&& m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE 
		&& (byVmpChnlIdx > 0 && byVmpChnlIdx < MAXNUM_MPUSVMP_MEMBER ) 
		&& !( tOrgSrcMt == *tvmpParam.GetVmpMember( byVmpChnlIdx ) )
		)
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] other mt is in vmp chl(%d) and it is same smcu to tOrgSrcMt(%d.%d) ,Mt can't in tvwall \n ",
			m_tConf.m_tStatus.GetVmpParam().GetChlOfMtInMember( tOrgSrcMt ),
			tOrgSrcMt.GetMcuId(),
			tOrgSrcMt.GetMtId()						
			);
		
		cServMsg.SetErrorCode( ERR_MCU_VCS_NOUSABLEBACKCHNNL );
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		return;
	}*/
	if (!tOrgSrcMt.IsLocal() && m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE &&
		!IsLocalAndSMcuSupMultSpy(tOrgSrcMt.GetMcuIdx()))
	{
		TVMPParam_25Mem tvmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
		TVMPMember *ptVMPMember;
		for (u8 byVmpChnIdx = 0 ; byVmpChnIdx < tvmpParam.GetMaxMemberNum() ; byVmpChnIdx++)
		{
			ptVMPMember = tvmpParam.GetVmpMember(byVmpChnIdx);
			if (ptVMPMember == NULL || ptVMPMember->IsNull() 
				|| ptVMPMember->IsLocal() || *ptVMPMember == tOrgSrcMt)
			{
				continue;
			}
			if (GetLocalMtFromOtherMcuMt(*ptVMPMember) == GetLocalMtFromOtherMcuMt(tOrgSrcMt))
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] other mt is in vmp chl(%d) and it is same smcu to ptVMPMember(%d.%d) ,Mt can't in tvwall \n ",
					byVmpChnIdx,ptVMPMember->GetMcuId(),ptVMPMember->GetMtId());
				cServMsg.SetErrorCode( ERR_MCU_VCS_NOUSABLEBACKCHNNL );
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
		}
	}
		
	// 电视墙非法, Nack
	if ( !IsValidTvw(tDstMt.GetEqpId(), byDstChannelIdx)
		|| !g_cMcuVcApp.IsPeriEqpConnected( tDstMt.GetEqpId() ))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] Wrong Peri Type (%d) or is has not registered \n ",
			tDstMt.GetEqpType() );
		
		cServMsg.SetErrorCode(ERR_MCU_VCS_NOUSABLETW);
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
	
		return;
	}
		
	TMt tSrcMt = GetLocalMtFromOtherMcuMt( tOrgSrcMt );
	TMt tDropMt;
	tDropMt.SetNull();
	TLogicalChannel tChannel;
	
	//同一个下级mcu的终端只能有一个进入电视墙通道，所以要把前一个剔除出电视墙
	if( !tOrgSrcMt.IsLocal() 
		&& !IsLocalAndSMcuSupMultSpy(tOrgSrcMt.GetMcuId()) 
		&& m_tConfAllMtInfo.MtJoinedConf( tSrcMt.GetMtId() ) )
	{		
		FindSmcuMtInTvWallAndStop( tOrgSrcMt, &tDropMt );				
		OnMMcuSetIn(tOrgSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
	}
	
	// [5/27/2011 liuxu] 对Tvwall和Hdu进行统一处理
	CTvwChnnl cDstTvwChnnl;
	if (!GetTvwChnnl( tDstMt.GetEqpId(), byDstChannelIdx, cDstTvwChnnl ))
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_VCS, "[SetSingleMtInTW]Get Tvwall Chnnl(%d, %d) falied\n", tDstMt.GetEqpId(), byDstChannelIdx);
		cServMsg.SetErrorCode(ERR_MCU_VCS_NOUSABLETW);
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		return;
	}

	// 先把旧的通道成员踢出此通道
	if( !cDstTvwChnnl.GetMember().IsNull() )
	{
		if( cDstTvwChnnl.GetMember().GetMcuId() == tOrgSrcMt.GetMcuId() &&
			cDstTvwChnnl.GetMember().GetMtId() == tOrgSrcMt.GetMtId())
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] Mt is already in TvWall\n" );
			return;		
		}
		
		TMt tOldMt = cDstTvwChnnl.GetMember();
		
		//停止旧成员到电视墙的交换
		if (IsValidHduChn(tDstMt.GetEqpId(), byDstChannelIdx))
		{
			//已经在电视墙中的终端停交换
			// [2013/03/11 chenbing] VCS会议不支持HDU多画面,子通道置0
			ChangeHduSwitch( NULL, tDstMt.GetEqpId(), byDstChannelIdx, 0, TW_MEMBERTYPE_VCSSPEC,
							TW_STATE_STOP, MODE_BOTH, FALSE, FALSE );
		}
		else
		{
			ChangeTvWallSwitch( &tOldMt, tDstMt.GetEqpId(), byDstChannelIdx, TW_MEMBERTYPE_VCSSPEC, TW_STATE_STOP );
		}
			
		//非当前调度终端退出电视墙，且已经起一键混音功能，就退出定制混音
		if( m_cVCSConfStatus.GetMtInTvWallCanMixing() && 
			m_tConf.m_tStatus.IsMixing() && m_tConf.m_tStatus.IsSpecMixing() &&
			VCSMtNotInTvWallCanStopMixing(	tOldMt )			
			)
		{
			RemoveSpecMixMember( &tOldMt, 1, FALSE );
		}
		
		// [2013/04/08 chenbing] 添加会议号判断，其他会议的终端不允许进行挂断操作
		// 修复Bug00134330，tOldMt为其他会议终端并且MtId为1时，VCS中当前主席MtId为1，
		// 进入此逻辑后会挂断当前主席
		if( !(m_cVCSConfStatus.GetCurVCMT() == tOldMt) &&
			m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE &&
			!m_ptMtTable->IsMtInTvWall( GetLocalMtFromOtherMcuMt(tOldMt).GetMtId() ) &&
			!m_ptMtTable->IsMtInHdu( GetLocalMtFromOtherMcuMt(tOldMt).GetMtId() ) &&
			(tOldMt.GetConfIdx() == m_byConfIdx))//会议号相同，才能执行挂断操作
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS,  "[SetSingleMtInTW] VCSDropMt Chairman.confId:%d ChairmanMcuId: %d ChairmanMtId: %d \n",
				m_tConf.GetChairman().GetConfIdx(), m_tConf.GetChairman().GetMcuId(), m_tConf.GetChairman().GetMtId() );
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS,  "[SetSingleMtInTW] VCSDropMt tOldMt.confId:%d McuId: %d MtId: %d \n",
				tOldMt.GetConfIdx(), tOldMt.GetMcuId(), tOldMt.GetMtId( ));
			VCSDropMT( tOldMt );
		}
	}

	// 新终端是否在线,终端已在线不加判逻辑通道未开，电话终端支持
	BOOL byOnline = m_tConfAllMtInfo.MtJoinedConf( tOrgSrcMt.GetMcuId(), tOrgSrcMt.GetMtId() );
	if( byOnline )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] mt is online,start switch to TVWall \n " );

		//BOOL32 bStartTvwOk = TRUE;
		if (IsValidHduChn( tDstMt.GetEqpId(), byDstChannelIdx))
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] mt is online,start switch to HDU \n " );
			// [2013/03/11 chenbing] VCS会议不支持HDU多画面,子通道置0
			ChangeHduSwitch( &tOrgSrcMt, tDstMt.GetEqpId(), byDstChannelIdx, 0, TW_MEMBERTYPE_VCSSPEC, TW_STATE_START );
		}
		else
		{
			//建交换
			ChangeTvWallSwitch( &tOrgSrcMt,
				tDstMt.GetEqpId(),
				byDstChannelIdx,
				TW_MEMBERTYPE_VCSSPEC,
				TW_STATE_START );

		}
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] mt is offline,save it in TVWall \n " );
		
		//未入会的终端在其上线或打开后向通道以后建到电视墙的交换
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( tDstMt.GetEqpId(),&tStatus );

		// [2013/04/08 chenbing] 当前电视墙通道被其他会议使用则不能进墙
		// 修复Bug00134330：Vcs调度终端进入了其他会议占用的电视墙通道
		if ( m_byConfIdx == tStatus.m_tStatus.tTvWall.atVideoMt[byDstChannelIdx].GetConfIdx()
			 || 0 == tStatus.m_tStatus.tTvWall.atVideoMt[byDstChannelIdx].GetConfIdx())
		{
			tStatus.m_tStatus.tTvWall.atVideoMt[byDstChannelIdx].SetMt( tOrgSrcMt );
			tStatus.m_tStatus.tTvWall.atVideoMt[byDstChannelIdx].byMemberType = TW_MEMBERTYPE_VCSSPEC;
			g_cMcuVcApp.SetPeriEqpStatus( tDstMt.GetEqpId(),&tStatus );
		}

		cServMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
		SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
		
		//Bug00156555：VCS单方调度一个呼不上来的终端进hdu或hdu2，终端会一直停留在电视墙通道中
		//yrl20131108 该处添加VCS_SINGLE_MODE支持，因为MCUVC_VCMTOVERTIMER_TIMER处理时m_cVCSConfStatus.SetReqVCMT()为空，
		//导致终端不能清出电视墙通道
		if( m_cVCSConfStatus.GetCurVCMode() == VCS_MULVMP_MODE
			|| m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE)
		{
			m_cVCSConfStatus.SetReqVCMT( tOrgSrcMt );
		}
		
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[SetSingleMtInTW] m_byConfIdx:%d HduChnMtConfId:%d tOrgSrcMt.McuId:%d tOrgSrcMt.MtId:%d\n ",
			m_byConfIdx, tStatus.m_tStatus.tTvWall.atVideoMt[byDstChannelIdx].GetConfIdx(), tOrgSrcMt.GetMcuId(), tOrgSrcMt.GetMtId());

		VCSCallMT( cServMsg,tOrgSrcMt,VCS_FORCECALL_REQ );
	}	
}

/*====================================================================
    函数名      ProcVcsMcuMtInTvWallInManuMode
    功能        ：手动模式中的终端进电视墙
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    11/4/27                 xueliang           创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuMtInTvWallInManuMode(CServMsg& cServMsg)
{
	m_cVCSConfStatus.SetIsTvWallOperating( TRUE );

	u8 *pbyBuf = cServMsg.GetMsgBody();
	u8 bySwitchInfoNum = *pbyBuf;
	if( 0 == bySwitchInfoNum )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ProcVcsMcuMtInTvWallInManuMode] bySwitchInfoNum is 0.Nack\n " );
		
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		m_cVCSConfStatus.SetIsTvWallOperating( FALSE );
		return;
	}

	//ack
	SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

	pbyBuf ++;
	for( u8 byIdx = 0; byIdx<bySwitchInfoNum; byIdx++ )
	{
		CServMsg cOutMsg(cServMsg.GetServMsg() ,SERV_MSGHEAD_LEN);
		TSwitchInfo *ptSwitchInfo = (TSwitchInfo*)(pbyBuf + byIdx * sizeof( TSwitchInfo ));
		SetSingleMtInTW(ptSwitchInfo, cOutMsg);
	}

	m_cVCSConfStatus.SetIsTvWallOperating( FALSE );
	return;

}

/*====================================================================
    函数名      ProcVcsMcuMtInTvWallInReviewMode
    功能        ：预览模式中的终端进电视墙
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuMtInTvWallInReviewMode(CServMsg& cServMsg)
{
	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsgSend;

	//预览模式下操作电视墙的伪原子操作
	//因为预览模式发过来的交换信息较多，要在完成所有交换操作后才能进行下一此的操作
	m_cVCSConfStatus.SetIsTvWallOperating( TRUE );

	u8 *pbyBuf = cServMsg.GetMsgBody();
	// [6/2/2011 liuxu] 要判断指针
	if ( !pbyBuf )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Damaged msg\n", CurState());
		return;
	}

	u8 bySwitchInfoNum = *pbyBuf;

	if( 0 == bySwitchInfoNum )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "bySwitchInfoNum is 0.Nack\n " );
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		m_cVCSConfStatus.SetIsTvWallOperating( FALSE );
		return;
	}

	if( m_cVCSConfStatus.GetTVWallManageMode() != VCS_TVWALLMANAGE_REVIEW_MODE )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "cur TvWallManagerMode is not VCS_TVWALLMANAGE_REVIEW_MODE\n" );
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		m_cVCSConfStatus.SetIsTvWallOperating( FALSE );
		return;
	}

    SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

	pbyBuf++;
	TSwitchInfo *ptSwitchInfo = NULL; 

	TMt tOrgSrcMt;// = tSwitchInfo.GetSrcMt();
	TMt tDstMt;// = tSwitchInfo.GetDstMt();

	u8 byDstChannelIdx = 0;//tSwitchInfo.GetDstChlIdx();

	TTWSwitchInfo tSwitchInfo;

	BOOL32 bCanSwitchToTW = TRUE;

	TMt tDropMt;

	TPeriEqpStatus tStatus;
	
	u8 byIdx = 0;

	//预览模式下终端进入电视墙，先清除所有电视墙通道(如果是上级的终端就看和原来通道是否一样，是同一个终端就不清除，下级一律清除)
	VCSClearAllTvWallChannel( (TSwitchInfo*)pbyBuf,bySwitchInfoNum );

	CTvwChnnl cTvwChnnl;						// 定义一个Tvw通道
	
	TVMPParam tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);// m_tConf.m_tStatus.GetVmpParam();
	//第二步把传上来的信息中源终端建到电视墙的交换
	//如果是本级的终端就无条件建交换，如果是下级的就先查找同一个下级的其它终端是否有进电视墙的
	//如果没有就建交换，如果有就保存成员信息上报界面。
	for( byIdx = 0; byIdx < bySwitchInfoNum; byIdx++ , pbyBuf += sizeof( TSwitchInfo ))
	{
		ptSwitchInfo = (TSwitchInfo*)pbyBuf;
		if (!ptSwitchInfo)
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Invalid pointer\n", CurState());
			return;
		}

		tOrgSrcMt = ptSwitchInfo->GetSrcMt();		
		tDstMt = ptSwitchInfo->GetDstMt();
		byDstChannelIdx = ptSwitchInfo->GetDstChlIdx();

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  
			"DstEqpType(%d) DstEqpId(%d) SrcMt(%d.%d) eqpConnected(%d) dstChlIdx(%d)\n",
			tDstMt.GetEqpType(), tDstMt.GetEqpId(), tOrgSrcMt.GetMcuId(), tOrgSrcMt.GetMtId(),
			g_cMcuVcApp.IsPeriEqpConnected( tDstMt.GetEqpId() ),byDstChannelIdx );

		// [5/30/2011 liuxu] 将tvwall类型判断进行分装
		if (!IsValidTvwEqp(tDstMt) || !IsValidTvw(tDstMt.GetEqpId(), byDstChannelIdx))
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "Invalid Tvw Eqp(%d, %d)\n" , tDstMt.GetEqpId(), byDstChannelIdx);
			continue;
		}

		if( !g_cMcuVcApp.IsPeriEqpConnected( tDstMt.GetEqpId() ) )
		{
			continue;
		}		

		tSwitchInfo.SetSrcMt(tOrgSrcMt);
		tSwitchInfo.SetDstMt(tDstMt);
		tSwitchInfo.SetDstChlIdx(byDstChannelIdx);
		tSwitchInfo.SetMemberType(TW_MEMBERTYPE_VCSSPEC);
		tSwitchInfo.SetMode(MODE_VIDEO);
		
		g_cMcuVcApp.GetPeriEqpStatus( tDstMt.GetEqpId(), &tStatus );
		bCanSwitchToTW = TRUE;
		tDropMt.SetNull();

		if( !m_tConfAllMtInfo.MtJoinedConf( tOrgSrcMt.GetMcuId(),tOrgSrcMt.GetMtId() ) )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "tOrgSrcMt(%d,%d) is not JoinedConf.So don't switch to TW\n",
								tOrgSrcMt.GetMcuId(),tOrgSrcMt.GetMtId() );

			bCanSwitchToTW = FALSE;
		}

		if( bCanSwitchToTW 
			&& !tOrgSrcMt.IsLocal() 
			&& !IsLocalAndSMcuSupMultSpy(tOrgSrcMt.GetMcuId()) 
			&& !(tOrgSrcMt == m_cVCSConfStatus.GetCurVCMT()) 
		   )
		{
			if( ( m_cVCSConfStatus.GetCurVCMT().GetMcuId() == tOrgSrcMt.GetMcuId() &&
							m_cVCSConfStatus.GetCurVCMT().GetMtId() != tOrgSrcMt.GetMtId() ) 
					|| 
				GetFstMcuIdFromMcuIdx(tOrgSrcMt.GetMcuId()) == GetFstMcuIdFromMcuIdx(m_cVCSConfStatus.GetCurVCMT().GetMcuId())
				)
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "Smcu(%d) has CurVcmt(%d.%d) is not SrcMt.So don't switch to TW\n",
					m_cVCSConfStatus.GetCurVCMT().GetMcuId(),m_cVCSConfStatus.GetCurVCMT().GetMcuId(),
					m_cVCSConfStatus.GetCurVCMT().GetMtId(),tOrgSrcMt.GetMcuId(),tOrgSrcMt.GetMtId()
					);

				bCanSwitchToTW = FALSE;
			}


			// 12/30/2010 liuxu][走读]逻辑与下面VCSLOG描述不相符，byVmpChnlIdx取到的是tOrgSrcMt的
			u8 byVmpChnlIdx = tConfVmpParam.GetChlOfMtInMember( tOrgSrcMt );

			if( bCanSwitchToTW 
				&& m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE 
				&& ( byVmpChnlIdx > 0 && byVmpChnlIdx < MAXNUM_VMP_MEMBER ) 
				&& !( tOrgSrcMt == *tConfVmpParam.GetVmpMember( byVmpChnlIdx ) )
			  )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "now mode is GROUPVMP_MODE and Smcu(%d) has a mt in vmp channel(%d) and the mt is not srcMt(%d.%d).So don't switch to TW\n",
					tOrgSrcMt.GetMcuId(),byVmpChnlIdx,
					tOrgSrcMt.GetMcuId(),tOrgSrcMt.GetMtId()
					);

				bCanSwitchToTW = FALSE;
			}

			//如果是第一个交换信息，总是建交换，这是和界面约定好的
			//如果有多个同一个下级的终端，要建交换的放在最前面，放在后面的保存成员信息，不建交换
			if( byIdx != 0 
				&& bCanSwitchToTW 
				&& ( m_cVCSConfStatus.GetCurVCMode() != VCS_GROUPVMP_MODE 
					// [12/30/2010 liuxu][走读] 以下两个||啰嗦
				     || ( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE && !( byVmpChnlIdx > 0 && byVmpChnlIdx < MAXNUM_VMP_MEMBER ) ) 
					 || ( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE && ( byVmpChnlIdx > 0 && byVmpChnlIdx < MAXNUM_VMP_MEMBER ) 
						  && !( tOrgSrcMt == *tConfVmpParam.GetVmpMember( byVmpChnlIdx ) ) )
					)
				)
			{				
				tDropMt.SetNull();
				FindSmcuMtInTvWallAndStop( tOrgSrcMt, &tDropMt, FALSE);
				if(  !tDropMt.IsNull() )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "Smcu(%d) has a mt(%d.%d) in tvwall and the mt is not srcMt(%d.%d).So don't switch to TW\n",
							tOrgSrcMt.GetMcuId(),tDropMt.GetMcuId(),tDropMt.GetMtId(),
							tOrgSrcMt.GetMcuId(),tOrgSrcMt.GetMtId()
							);
					bCanSwitchToTW = FALSE;
				}								
			}
		}
		
		// [5/31/2011 liuxu] 对Tvwall和Hdu的处理进行了合并
		// 获取电视墙通道, 获取失败continue
		if (!GetTvwChnnl(tDstMt.GetEqpId(), byDstChannelIdx, cTvwChnnl))
		{
			continue;
		}		
		
		// 获取在墙内的当前终端
		tDropMt = (TMt)cTvwChnnl.GetMember();
		
		// 如果新入终端与旧终端不同,先要把旧终端踢出墙
		if ( tDropMt.IsNull()						// 旧为空
			|| !(tDropMt == tOrgSrcMt)				// 新旧不相等
			|| !bCanSwitchToTW)						// 不能建到电视墙
		{
			// 把旧终端踢出墙
			if( IsValidHduChn(tDstMt.GetEqpId(), byDstChannelIdx) )
			{
				// [2013/03/11 chenbing] VCS会议不支持HDU多画面,子通道置0
				ChangeHduSwitch( NULL, tDstMt.GetEqpId(), byDstChannelIdx, 0, 
					cTvwChnnl.GetMember().byMemberType, TW_STATE_STOP,
					MODE_BOTH, FALSE, FALSE);	
			}
			else 
			{
				ChangeTvWallSwitch(&tDropMt, tDstMt.GetEqpId(), byDstChannelIdx,
					cTvwChnnl.GetMember().byMemberType, TW_STATE_STOP);
			}
			
			// 新终端进墙
			if( bCanSwitchToTW )
			{						
				VCSConfMTToTW( tSwitchInfo );
			}
			else
			{
				// tzy 组呼会议，预览电视墙模式时，VCS会议不在线终端会抢断MCS会议终端占用的电视墙通道
				if ( tDropMt.GetConfIdx() == 0 ||
					tDropMt.GetConfIdx() == m_byConfIdx)
				{
					// 更新通道
					if( g_cMcuVcApp.GetPeriEqpStatus( tDstMt.GetEqpId(),&tStatus ) )
					{
						tStatus.m_tStatus.tTvWall.atVideoMt[byDstChannelIdx].SetMt( tOrgSrcMt );
						tStatus.m_tStatus.tTvWall.atVideoMt[byDstChannelIdx].byMemberType = TW_MEMBERTYPE_VCSSPEC;
						g_cMcuVcApp.SetPeriEqpStatus( tDstMt.GetEqpId(), &tStatus );
					}
				}
			}
		}

		if( !tDropMt.IsNull() )
		{				
			//非当前调度终端退出电视墙，且已经起一键混音功能，就退出定制混音
			if( m_cVCSConfStatus.GetMtInTvWallCanMixing() 
				&& m_tConf.m_tStatus.IsMixing() 
				&& m_tConf.m_tStatus.IsSpecMixing() 
				&& VCSMtNotInTvWallCanStopMixing( tDropMt ))
			{
				RemoveSpecMixMember( &tDropMt, 1, FALSE );
			}				
		}		
		
		// 将最新的通道状态上报给各会控
		if (g_cMcuVcApp.GetPeriEqpStatus( tDstMt.GetEqpId(),&tStatus ))
		{
			cServMsgSend.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
			SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsgSend);
		}		
		
		// 是否要恢复分辨率
		if( NeedChangeVFormat(tDropMt) 
			&& !m_ptMtTable->IsMtInTvWall( GetLocalMtFromOtherMcuMt(tDropMt).GetMtId() ) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "Restore Format mt(%d.%d)\n", tDropMt.GetMcuId(), tDropMt.GetMtId() );
			ChangeVFormat( tDropMt,FALSE );					//恢复分辨率			
		}
	}

	m_cVCSConfStatus.SetIsTvWallOperating( FALSE );
}

/*====================================================================
    函数名      : VCSTransOutMsgToInnerMsg
    功能        ：将外部通信消息体转换为内部处理消息体
    算法实现    ：
    引用全局变量：
    输入参数说明：[IN/OUT]cOutMsg:外部消息体
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                付秀华          创建
====================================================================*/
void CMcuVcInst::VCSTransOutMsgToInnerMsg(CServMsg& cOutMsg)
{
	CServMsg cInnerMsg = cOutMsg;
	u8* pbyBuf = cOutMsg.GetMsgBody();

	if (VCS_MCU_SETMTINTVWALL_REQ == cOutMsg.GetEventId())
	{
		// 将与MCS通信的消息体转化为简单不变的内部处理消息体(u8(交换数) + TSwitchInfo + ...
		u8 bySwitchInfoNum = *pbyBuf++;
		cInnerMsg.SetMsgBody(&bySwitchInfoNum, sizeof(bySwitchInfoNum));
		TSwitchInfo tTemSwitchInfo;
		for (u8 byLoop = 0; byLoop < bySwitchInfoNum; byLoop++ )
		{
			if (VCS_DTYPE_MTALIAS == *pbyBuf++)
			{
				TMt tVCMT; 
				u8 byAliasType = *pbyBuf++;
				u8 byAliasLen  = *pbyBuf++;
				TMtAlias tMtAlias;
				if (tMtAlias.SetAlias(byAliasType, byAliasLen, (s8*)pbyBuf))
				{
					pbyBuf += byAliasLen;
					u8 byForce = !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode());
					tVCMT = GetVCMTByAlias(tMtAlias, ntohs( *(u16*)pbyBuf ),byForce);
					pbyBuf += sizeof(u16);

				}
				else
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuMtInTwMsg]bad alias info(type:%d byaliaslen:%d)\n",
							  byAliasType, byAliasLen);
					pbyBuf = pbyBuf + byAliasLen + sizeof(u16);
					tVCMT.SetNull();
				}
				tTemSwitchInfo = *(TSwitchInfo*)pbyBuf;
				tTemSwitchInfo.SetSrcMt(tVCMT);
			}
			else
			{
				tTemSwitchInfo = *(TSwitchInfo*)pbyBuf;
			}
			cInnerMsg.CatMsgBody((u8*)&tTemSwitchInfo, sizeof(TSwitchInfo));
			pbyBuf += sizeof(TSwitchInfo);
		}
	}
	else
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSTransOutMsgToInnerMsg]unproc msg(%d)\n",
			      cOutMsg.GetEventId());
	}

	cOutMsg = cInnerMsg;
}
/*====================================================================
    函数名      ProcVcsMcuMtInTwMsg
    功能        ：终端进电视墙通道请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26                周晶晶          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuMtInTwMsg( const CMessage *pcMsg )
{
	if( CurState() != STATE_ONGOING )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "CMcuVcInst: Wrong handle in state %u!\n", CurState());
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	if( m_cVCSConfStatus.GetIsTvWallOperating() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuMtInTwMsg] Now is operation tvWall.Nack\n" );
		cServMsg.SetErrorCode(ERR_MCU_VCS_PLANISOPERATING);
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		return;
	}

	//action abort when no chairman online
	TMt tChairMan = m_tConf.GetChairman();
	if( ( tChairMan.IsNull()  || !m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) 
		&& m_byCreateBy != CONF_CREATE_MT )
	{
		cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
		cServMsg.SetEventId(cServMsg.GetEventId() + 2);
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}
	
	u8 bySwitchInfoNum = *cServMsg.GetMsgBody();
	if( 0 == bySwitchInfoNum )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuMtInTwMsg] bySwitchInfoNum is 0.Nack\n " );
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		return;
	}	

	// 判断上一个请求调度的终端是否完成调度
	TMt tOldReqVCMT = m_cVCSConfStatus.GetReqVCMT();
	if (!tOldReqVCMT.IsNull())
	{
		cServMsg.SetErrorCode(ERR_MCU_VCS_VCMTING);
		cServMsg.SetEventId(cServMsg.GetEventId() + 2);				
		cServMsg.SetMsgBody((u8*)&tOldReqVCMT, sizeof(TMt));
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}
	// 画面合成相关模式,合成器未开启也不做响应
	if (g_cMcuVcApp.GetVMPMode(m_tVmpEqp) == CONF_VMPMODE_NONE 
		&& (VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() ||
			VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode() ||
			VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode())
		)
	{
		cServMsg.SetErrorCode(ERR_MCU_VMPNOTSTART);
		cServMsg.SetEventId(cServMsg.GetEventId() + 2);
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}

	CServMsg cInnerMsg = cServMsg;
	VCSTransOutMsgToInnerMsg(cInnerMsg);

	switch(m_cVCSConfStatus.GetTVWallManageMode())
	{
	case VCS_TVWALLMANAGE_REVIEW_MODE:
		{
			ProcVcsMcuMtInTvWallInReviewMode( cInnerMsg );
			break;
		}

	case VCS_TVWALLMANAGE_MANUAL_MODE:
		{
			ProcVcsMcuMtInTvWallInManuMode( cInnerMsg);
			break;
		}

	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuMtInTwMsg]unproc twmode(%d)\n", m_cVCSConfStatus.GetTVWallManageMode());
	    break;
	}
}

/*====================================================================
    函数名      ：ProcVcsMcuGroupCallMtReq
    功能        ：组呼终端请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/04/11                付秀华          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuGroupCallMtReq(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	CServMsg cServBackMsg;
	cServBackMsg.SetServMsg(pcMsg->content, SERV_MSGHEAD_LEN);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			//action abort when no chairman online
			TMt tChairMan = m_tConf.GetChairman();
			if( ( tChairMan.IsNull()  || !m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) 
				&& m_byCreateBy != CONF_CREATE_MT )
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			SendReplyBack(cServBackMsg, cServMsg.GetEventId() + 1);

			// u8(组数)+u8(组名长度)+s8字符串(组名)+u8(终端数)+TMt数组
			u8* pbyMsg = (u8*)cServMsg.GetMsgBody();
			u8 byGroupNum = *pbyMsg++;

			u16 wErrCode       = 0;
			u8  byCallMtNum    = 0;
			u8  byGroupNameLen = 0;
			for (u8 byGroupIdx = 0; byGroupIdx < byGroupNum; byGroupIdx++)
			{
				byGroupNameLen = *pbyMsg++;

				if (!m_cVCSConfStatus.OprGroupCallList((s8*)pbyMsg, byGroupNameLen, TRUE, wErrCode))
				{
					cServMsg.SetMsgBody(pbyMsg, byGroupNameLen);
					cServMsg.SetErrorCode(wErrCode);
					SendReplyBack(cServBackMsg, cServMsg.GetEventId() + 3);
					continue;
				}

				pbyMsg += byGroupNameLen;
				byCallMtNum = *pbyMsg++;
				TMt* ptMt = (TMt*)pbyMsg;
				for (u8 byIdx = 0; byIdx < byCallMtNum; byIdx++)
				{
					if (m_tConfAllMtInfo.MtInConf(ptMt->GetMtId()) &&
						!m_tConfAllMtInfo.MtJoinedConf(ptMt->GetMtId()))
					{
						m_ptMtTable->SetCallMode( ptMt->GetMtId(),CONF_CALLMODE_TIMER );
						InviteUnjoinedMt(cServBackMsg, ptMt);						
					}
					else
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ProcVcsMcuGroupCallMtReq]mtid(%d) not in mtlist of the conf or has joined the conf\n",
							   ptMt->GetMtId());
					}

					ptMt++;
					pbyMsg += sizeof(TMt);
				}			
			}

			VCSConfStatusNotif();
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuGroupCallMtReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}
/*====================================================================
    函数名      ：ProcVcsMcuGroupDropMtReq
    功能        ：挂断组呼终端请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/04/11                付秀华          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuGroupDropMtReq(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	CServMsg cServBackMsg;
	cServBackMsg.SetServMsg(pcMsg->content, SERV_MSGHEAD_LEN);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			SendReplyBack(cServBackMsg, cServMsg.GetEventId() + 1);		

			// u8(组数)+u8(组名长度)+s8字符串(组名)+u8(终端数)+TMt数组
			u8* pbyMsg = (u8*)cServMsg.GetMsgBody();
			u8 byGroupNum = *pbyMsg++;

			u16 wErrCode       = 0;
			u8  byCallMtNum    = 0;
			u8  byGroupNameLen = 0;

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVcsMcuGroupDropMtReq]byGroupNum.%d\n", byGroupNum);
			for (u8 byGroupIdx = 0; byGroupIdx < byGroupNum; byGroupIdx++)
			{
				byGroupNameLen = *pbyMsg++;

				if (!m_cVCSConfStatus.OprGroupCallList((s8*)pbyMsg, byGroupNameLen, FALSE, wErrCode))
				{
					cServMsg.SetMsgBody(pbyMsg, byGroupNameLen);
					cServMsg.SetErrorCode(wErrCode);
					SendReplyBack(cServBackMsg, cServMsg.GetEventId() + 3);
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "OprGroupCallList(%s,%d) failed, err.%d\n", (s8*)pbyMsg, byGroupNameLen, wErrCode);
					continue;
				}

				pbyMsg += byGroupNameLen;
				byCallMtNum = *pbyMsg++;
				TMt* ptMt = (TMt*)pbyMsg;
				for (u8 byIdx = 0; byIdx < byCallMtNum; byIdx++)
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcVcsMcuGroupDropMtReq] Group Drop Mt.%d at loop[%d,%d]\n",ptMt->GetMtId(), byIdx, byCallMtNum );
					if (m_tConfAllMtInfo.MtJoinedConf(ptMt->GetMtId()))
					{
						VCSDropMT(*ptMt);
					}
					else
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVcsMcuGroupDropMtReq]mtid(%d) not joined the conf IsInTvWall(%d) IsInHdu(%d)\n",
							   ptMt->GetMtId(),m_ptMtTable->IsMtInTvWall( ptMt->GetMtId() ),
							   m_ptMtTable->IsMtInTvWall( ptMt->GetMtId() ) );
						
						// [10/20/2011 liuxu] 组呼挂断时, 它所属的终端都需要清空出电视墙
						//VCSClearTvWallChannelByMt( *ptMt );	
						
						RemoveJoinedMt( *ptMt,TRUE );						
					}

					// [10/20/2011 liuxu] 组呼挂断时, 它所属的终端都需要清空出电视墙
					VCSClearTvWallChannelByMt( *ptMt );	

					m_ptMtTable->SetCallMode( ptMt->GetMtId(),CONF_CALLMODE_NONE );
					VCSClearMtDisconnectReason( *ptMt );					

					ptMt++;
					pbyMsg += sizeof(TMt);
				}			
			}

			VCSConfStatusNotif();

		}
		break;

	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuGroupDropMtReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}
/*====================================================================
    函数名      ：ProcVcsMcuStartChairPollReq
    功能        ：开启主席轮询请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/04/11                付秀华          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuStartChairPollReq(const CMessage *pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			//action abort when no chairman online
			TMt tChairMan = m_tConf.GetChairman();
			if( ( tChairMan.IsNull()  || 
				!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
				m_byCreateBy != CONF_CREATE_MT
				)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			if (m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP)
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcVcsMcuStartChairPollReq]Chairpoll has already started\n");
				cServMsg.SetErrorCode(ERR_MCU_VCS_CHAIRPOLLING);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			}

			SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

			u8 byPollIntval = *(u8*)cServMsg.GetMsgBody();
			m_cVCSConfStatus.SetChairPollState(VCS_POLL_START);
			m_cVCSConfStatus.SetPollIntval(byPollIntval);

			VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);
			TMt tPollMt = VCSGetNextPollMt();
			if (!tPollMt.IsNull())
			{
				TSwitchInfo tSwitchInfo;
				tSwitchInfo.SetSrcMt(tPollMt);
				tSwitchInfo.SetDstMt(m_tConf.GetChairman());
				tSwitchInfo.SetMode(MODE_VIDEO);
				VCSConfSelMT(tSwitchInfo);

			}
			
			m_cVCSConfStatus.SetCurChairPollMt(tPollMt);
			VCSConfStatusNotif();

			SetTimer(MCUVC_VCS_CHAIRPOLL_TIMER, 1000 * byPollIntval);
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuStartChairPollReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}
/*====================================================================
    函数名      ：ProcChairPollTimer
    功能        ：开启主席轮询请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/04/11                付秀华          创建
====================================================================*/
void CMcuVcInst::ProcChairPollTimer( void )
{
	KillTimer(MCUVC_VCS_CHAIRPOLL_TIMER);

	if (!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
	{
		TMt tNull;
		tNull.SetNull();
		m_cVCSConfStatus.SetCurChairPollMt(tNull);
		m_cVCSConfStatus.SetChairPollState(VCS_POLL_STOP);
		return;
	}

	if (VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState())
	{
		return;
	}

	// miaoqingsong [06/27/2011] 按主席轮询选看逻辑：定时器到先停上次选看再启下次选看，故放开以下注掉代码
 	TMt tChairman = m_tConf.GetChairman();
	//zhouyiliang 20121225 如果上次轮询的和这次要轮询的是同一个终端，不重新拆建交换
	TMt tPollMt = VCSGetNextPollMt();
	TMtStatus tMtState;
	m_ptMtTable->GetMtStatus(tChairman.GetMtId(), &tMtState);
	TMt tSelectVideoMt = tMtState.GetSelectMt(MODE_VIDEO);
	if ( !tPollMt.IsNull() && !tSelectVideoMt.IsNull() && ( tPollMt == tSelectVideoMt ) )
	{
		SetTimer(MCUVC_VCS_CHAIRPOLL_TIMER, 1000 * m_cVCSConfStatus.GetPollIntval());
		return;
	}
	// miaoqingsong [06/27/2011] 按主席轮询选看逻辑：定时器到先停上次选看再启下次选看，故放开以下注掉代码
	if (!tSelectVideoMt.IsNull() )
	{
		VCSConfStopMTSel(tChairman, MODE_VIDEO);
	}

	// zjj
// 	TMtStatus tMtState;
// 	m_ptMtTable->GetMtStatus(tChairman.GetMtId(), &tMtState);	
// 	if (!tMtState.GetSelectMt(MODE_VIDEO).IsNull())
// 	{
// 		VCSConfStopMTSel(tChairman, MODE_VIDEO);
// 	}


	if (!tPollMt.IsNull())
	{
		TSwitchInfo tSwitchInfo;
		tSwitchInfo.SetSrcMt(tPollMt);
		tSwitchInfo.SetDstMt(tChairman);
		tSwitchInfo.SetMode(MODE_VIDEO);
		VCSConfSelMT(tSwitchInfo);
	}
	else
	{
		StopSwitchToSubMt( 1,&tChairman,MODE_VIDEO );		
		// 无可轮询的终端, 主席看自己, 直到有新终端或者结束轮询
		NotifyMtReceive(tChairman, tChairman.GetMtId());
	}
	m_cVCSConfStatus.SetCurChairPollMt(tPollMt);
	VCSConfStatusNotif();

	SetTimer(MCUVC_VCS_CHAIRPOLL_TIMER, 1000 * m_cVCSConfStatus.GetPollIntval());
}
/*====================================================================
    函数名      ：ProcVcsMcuStopChairPollReq
    功能        ：停止主席轮询请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/04/11                付秀华          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuStopChairPollReq(const CMessage *pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch(CurState())
	{
	case STATE_ONGOING:
		{

			SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
			VcsStopChairManPoll();
// 			KillTimer(MCUVC_VCS_CHAIRPOLL_TIMER);
// 			if (m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP)
// 			{
// 				TMt tNullMt;
// 				tNullMt.SetNull();
// 				m_cVCSConfStatus.SetChairPollState(VCS_POLL_STOP);
// 				m_cVCSConfStatus.SetCurChairPollMt(tNullMt);
// 				m_cVCSConfStatus.SetPollIntval(0);
// 
// 				VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);
// 
// 				TMt tChairMan = m_tConf.GetChairman();
// 
//                 // miaoqingsong [06/22/2011] VCS组呼电视墙模式/组呼主席模式/组呼点名模式下，主席与会不为空
// 				// 并且存在调度终端的情形下，停主席轮询，需要恢复主席选看当前调度终端的视频
// 				if ( (m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPTW_MODE ||
// 					  m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPCHAIRMAN_MODE ||
// 					  m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE) 
// 					  && !m_cVCSConfStatus.GetCurVCMT().IsNull() 
// 					  && !tChairMan.IsNull()	// filter chairman
// 					  && m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId())
// 					 ) 
// 				{
// 					TSwitchInfo tSwitchInfo;
// 					tSwitchInfo.SetSrcMt( m_cVCSConfStatus.GetCurVCMT() );
// 					tSwitchInfo.SetDstMt( m_tConf.GetChairman() );
// 					tSwitchInfo.SetMode( MODE_VIDEO );
// 					VCSConfSelMT( tSwitchInfo );
// 				}
// 
// 				VCSConfStatusNotif();
// 			}

			
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuStopChairPollReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}


/*====================================================================
    函数名      VcsStopChairManPoll
    功能        ：停止主席轮询处理
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    12/09/06                zhouyiliang          创建
====================================================================*/
void CMcuVcInst::VcsStopChairManPoll()
{
	switch(CurState())
	{
	case STATE_ONGOING:
		{

			KillTimer(MCUVC_VCS_CHAIRPOLL_TIMER);
			if (m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP)
			{
				TMt tNullMt;
				tNullMt.SetNull();
				m_cVCSConfStatus.SetChairPollState(VCS_POLL_STOP);
				m_cVCSConfStatus.SetCurChairPollMt(tNullMt);
				m_cVCSConfStatus.SetPollIntval(0);
				

				TMt tChairMan = m_tConf.GetChairman();

                // miaoqingsong [06/22/2011] VCS组呼电视墙模式/组呼主席模式/组呼点名模式下，主席与会不为空
				// 并且存在调度终端的情形下，停主席轮询，需要恢复主席选看当前调度终端的视频
				if ( !tChairMan.IsNull()	// filter chairman
					  && m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId())
					 ) 
				{
					if( (m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPTW_MODE ||
						  m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPCHAIRMAN_MODE ||
						  m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE) 
							&& !m_cVCSConfStatus.GetCurVCMT().IsNull() &&
							m_tConfAllMtInfo.MtJoinedConf(m_cVCSConfStatus.GetCurVCMT().GetMcuId(), m_cVCSConfStatus.GetCurVCMT().GetMtId())
						)
					{
						VCSConfStopMTSel( m_tConf.GetChairman(), MODE_VIDEO,FALSE );
						TSwitchInfo tSwitchInfo;
						tSwitchInfo.SetSrcMt( m_cVCSConfStatus.GetCurVCMT() );
						tSwitchInfo.SetDstMt( m_tConf.GetChairman() );
						tSwitchInfo.SetMode( MODE_VIDEO );
						VCSConfSelMT( tSwitchInfo );
					}
					else
					{
						TMtStatus tMtStatus; 
						m_ptMtTable->GetMtStatus( m_tConf.GetChairman().GetMtId(), &tMtStatus );
						TMt tVidSrc = tMtStatus.GetSelectMt( MODE_VIDEO );

						VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);
						if( tVidSrc.IsNull() )
						{
							RestoreRcvMediaBrdSrc( m_tConf.GetChairman().GetMtId(),MODE_VIDEO );
						}					
					}
				}
				else
				{
					VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);
				}
				
				

				VCSConfStatusNotif();
			}

			
		}
		break;
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VcsStopChairManPoll] handle in wrong state %u!\n", CurState() );
		break;
	}
}
/*====================================================================
    函数名      ：ProcVcsMcuAddMtReq
    功能        ：临时增加终端请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/04/11                付秀华          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuAddMtReq(const CMessage *pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			//action abort when no chairman online
			TMt tChairMan = m_tConf.GetChairman();
			if( ( tChairMan.IsNull()  || 
				!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
				m_byCreateBy != CONF_CREATE_MT
				)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			//TAddMtInfo* ptAddMtInfo = (TAddMtInfo*)cServMsg.GetMsgBody();
			CServMsg cSendServMsg;
			cSendServMsg.SetConfIdx( m_byConfIdx );
			cSendServMsg.SetConfId( m_tConf.GetConfId() );
			
			TAddMtInfo* ptAddMtInfo = NULL;
			u8 *pbyBuf = cServMsg.GetMsgBody();
			u8 byCount = *pbyBuf;
			if( 0 == byCount )
			{
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcVcsMcuAddMtReq] byCount is 0.Nack\n");
				return;
			}
			
			pbyBuf++;

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ProcVcsMcuAddMtReq] byCount.%d\n",byCount );

			
			for( u8 byIdx = 0;byIdx<byCount;byIdx++ )
			{
				ptAddMtInfo = (TAddMtInfo*)pbyBuf;
				//不能呼叫会议本身
				if (mtAliasTypeE164 == ptAddMtInfo->m_AliasType)
				{
					if (0 == strcmp(ptAddMtInfo->m_achAlias, m_tConf.GetConfE164()))
					{
						cServMsg.SetErrorCode(ERR_MCU_NOTCALL_CONFITSELF);
						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcVcsMcuAddMtReq]Cannot Call conf itself.\n");
						return;
					}
				}

				//是否超过性能限制
				if (!m_cVCSConfStatus.IsAddEnable())
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_OVERADDMTNUM);
					SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
					ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcVcsMcuAddMtReq]over addmt num\n");
					return;
				}
				
				//增加受邀终端列表
				u8 byMtId = AddMt(*ptAddMtInfo, ptAddMtInfo->GetCallBitRate(), ptAddMtInfo->GetCallMode());
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ProcVcsMcuAddMtReq]add mt-%d callmode-%d DialBitRate-%d type-%d alias-", 
						byMtId, ptAddMtInfo->GetCallMode(), ptAddMtInfo->GetCallBitRate(), 
						ptAddMtInfo->m_AliasType );
				if ( ptAddMtInfo->m_AliasType == mtAliasTypeTransportAddress )
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "%s:%d!\n", StrOfIP(ptAddMtInfo->m_tTransportAddr.GetIpAddr()), 
									   ptAddMtInfo->m_tTransportAddr.GetPort() );
				}
				else
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "%s!\n",	ptAddMtInfo->m_achAlias);
				}

				if (0 == byMtId)
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcVcsMcuAddMtReq]Fail to alloc mtid\n");
					SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
					return;
				}

				//SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

				cSendServMsg.SetServMsg( cServMsg.GetServMsg(), SERV_MSGHEAD_LEN );
    

				TMt tMt = m_ptMtTable->GetMt( byMtId );
				if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
				{
					//InviteUnjoinedMt( cServMsg, &tMt, TRUE, TRUE );
					InviteUnjoinedMt( cSendServMsg, &tMt, TRUE, TRUE );
				}

				//发给会控会议所有终端信息
				//cSendServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
				//cSendServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
				SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cSendServMsg );

				//发给会控终端表
				SendMtListToMcs(LOCAL_MCUIDX);

				m_cVCSConfStatus.OprNewMt(tMt, TRUE);

				pbyBuf += sizeof( TAddMtInfo );

			}
			
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

			VCSConfStatusNotif();
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuAddMtReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}
/*====================================================================
    函数名      ：ProcVcsMcuDelMtReq
    功能        ：临时删除终端请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/04/11                付秀华          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuDelMtReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
			//action abort when no chairman online
			TMt tChairMan = m_tConf.GetChairman();
			if( ( tChairMan.IsNull()  || 
				!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
				m_byCreateBy != CONF_CREATE_MT
				)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
				cServMsg.SetEventId(cServMsg.GetEventId() + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
				return;
			}

			TMt tMt = *(TMt*)cServMsg.GetMsgBody();
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

			if (!tMt.IsNull() && 
				m_cVCSConfStatus.OprNewMt(tMt, FALSE))
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ProcVcsMcuDelMtReq]drop mt(mcuid:%d, mtid:%d)\n", 
					   tMt.GetMcuId(), tMt.GetMtId());
				VCSDropMT(tMt);

				VCSConfStatusNotif();
			}
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuDelMtReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcVcsMcuChangeDualStatusReq
    功能        ：改变会议当前双流使能
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/13/2010              张宝卿          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuChangeDualStatusReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    switch(CurState())
    {
    case STATE_ONGOING:
        {
            u8 byStatus = *cServMsg.GetMsgBody();
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "byStatus.%d\n",byStatus );

			if (CONF_DUALSTATUS_ENABLE == byStatus || CONF_DUALSTATUS_DISABLE == byStatus)
			{
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);
			}
			else
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuChangeDualStatusReq] unexpected DsStatus:%d!\n", byStatus);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			}

            
            u8 byOldStatus = m_cVCSConfStatus.IsConfDualEnable() ? CONF_DUALSTATUS_ENABLE : CONF_DUALSTATUS_DISABLE;

			if (byStatus != byOldStatus)
			{
				if (CONF_DUALSTATUS_DISABLE == byStatus)
				{
					StopDoubleStream(TRUE, FALSE);
				}
				m_cVCSConfStatus.SetConfDualEnable(byStatus);
				VCSConfStatusNotif();
			}
            
        }
        break;
    default:
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
        ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuChangeDualStatusReq] Wrong message %u(%s) received in state %u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;
    }
}

/*====================================================================
    函数名      ：ProcVcsMcuMtJoinConfRsp
    功能        ：终端入会申请应答
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    11/12/2010              周晶晶          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuMtJoinConfRsp( const CMessage * pcMsg )
{
	if( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuMtJoinConfRsp] Wrong message %u(%s) received in state %u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	TMtAlias    tFstAlias = *( TMtAlias * )( cServMsg.GetMsgBody() + sizeof(u8) * 2 );
	TMtAlias    tMtH323Alias = *( TMtAlias * )( cServMsg.GetMsgBody() + sizeof(u8) * 2 + sizeof(TMtAlias) );
	TMtAlias    tMtE164Alias = *( TMtAlias * )( cServMsg.GetMsgBody() + sizeof(TMtAlias) * 2 + sizeof(u8) * 2 );
	TMtAlias    tMtAddr      = *( TMtAlias * )( cServMsg.GetMsgBody() + sizeof(TMtAlias) * 3 + sizeof(u8) * 2 );
	u8          byEncrypt    = *(cServMsg.GetMsgBody());
	u8			bySrcDriId = *(cServMsg.GetMsgBody() + sizeof(u8) );
	u16         wCallRate    = *(u16*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)));
	u8			bySrcSsnId = *(u8*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)+sizeof(u16)));
	u8			byMtInConf = *(u8*)(cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)+sizeof(u16)+sizeof(u8));
    wCallRate = htons( wCallRate ); 
	u8 byMtId = 0;
	

    switch(cServMsg.GetEventId())
    {
    case VCS_MCU_MTJOINCONF_ACK:
        {
            if (byMtInConf == 0)
            {
                if ( !tMtE164Alias.IsNull() )
                {
                    tFstAlias = tMtE164Alias;
                }
                else if ( !tMtH323Alias.IsNull() )
                {
                    tFstAlias = tMtH323Alias;
                }
                else
                {
                    tFstAlias = tMtAddr;
                }
            }
			

            wCallRate -= GetAudioBitrate( m_tConf.GetMainAudioMediaType() );               
        
			//分配终端ID号
			byMtId = AddMt( tFstAlias, wCallRate, CONF_CALLMODE_NONE/*, TRUE*/ );

			//终端呼叫入会，其必不在受邀列表，如果为单方调度则设置为请求调度终端，待通道打开后和主席互相选看
			if ( VCS_CONF == m_tConf.GetConfSource() && !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
				CONF_CREATE_MT != m_byCreateBy 
				)
			{
                //[11/22/2011 zhushengze]终端已经入会，MCUVC_VCMTOVERTIMER_TIMER就不会在之后被杀掉
                //导致已经入会终端MCUVC_VCMTOVERTIMER_TIMER到来时被remove
                if (m_tConfAllMtInfo.MtJoinedConf( byMtId ))
                {
                    LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[ProcVcsMcuMtJoinConfRsp]mt.%d has joined conf!\n", byMtId);
                    return;
                }

				//zjj20110105 在单方调度模式下,如果在调度终端时前一个调度终端还在调度过程中就拒绝其它终端的入会请求
				if( !m_cVCSConfStatus.GetReqVCMT().IsNull()	)
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuMtJoinConfRsp] Fail to Call Mt(%s),because now has req Mt(%d.%d)\n",
							tFstAlias.m_achAlias,m_cVCSConfStatus.GetReqVCMT().GetMcuId(),
							m_cVCSConfStatus.GetReqVCMT().GetMtId()
							);
					cServMsg.SetSrcMtId(CONF_CREATE_MT);
					cServMsg.SetSrcDriId( bySrcDriId );
					cServMsg.SetSrcSsnId( bySrcSsnId );
					cServMsg.SetConfIdx( m_byConfIdx );
					cServMsg.SetConfId( m_tConf.GetConfId() );
					cServMsg.SetEventId( MCU_MT_CREATECONF_NACK );
					g_cMtAdpSsnApp.SendMsgToMtAdpSsn(bySrcDriId, MCU_MT_CREATECONF_NACK, cServMsg);
					return;
				}
				TMt tMt = m_ptMtTable->GetMt(byMtId);
				m_cVCSConfStatus.SetReqVCMT( tMt );
				SetTimer( MCUVC_VCMTOVERTIMER_TIMER, (g_cMcuVcApp.GetVcsMtOverTimeInterval() + 5)*1000  );			
			}

			// zbq [08/09/2007] 经过允许Ip段呼入,但呼叫别名又不是Ip的终端,此处需保存其Ip
			if ( !(tFstAlias == tMtAddr) )
			{
				m_ptMtTable->SetMtAlias( byMtId, &tMtAddr );
				// guzh [10/29/2007] 保存TMtExt 里面的IP地址
				m_ptMtTable->SetIPAddr( byMtId, tMtAddr.m_tTransportAddr.GetIpAddr() );
			}
        
			// xsl [11/8/2006] 接入板终端计数加1
			// xliang [2/14/2009] 区分是MT还是MCU,补计数
			//u8 byDriId = cServMsg.GetSrcDriId();		
			g_cMcuVcApp.IncMtAdpMtNum( bySrcDriId, m_byConfIdx, byMtId );
			m_ptMtTable->SetDriId(byMtId, bySrcDriId);
	// 		if(byType == TYPE_MCU)
	// 		{
	// 			//接入板的终端计数还要+1
	// 			//在mtadplib那端已通过判断，所以肯定有剩余接入点让其占用
	// 			g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum++;
	// 		}
			
			if( byMtId > 0  )
			{
				//标记为呼入终端
				//m_ptMtTable->SetMtCallingIn( byMtId, TRUE );
            
				// zbq [08/31/2007] 受邀终端重新呼入保持其受邀身份
            
                if ( 0 == byMtInConf )
                {
                    m_ptMtTable->SetNotInvited( byMtId, TRUE );
                }
				
            
				//m_ptMtTable->SetAddMtMode(byMtId, ADDMTMODE_MTSELF);
				
				
				m_ptMtTable->SetMtType( byMtId, MT_TYPE_MT );
				
				
				m_ptMtTable->SetMtAlias(byMtId, &tMtH323Alias);
				m_ptMtTable->SetMtAlias(byMtId, &tMtE164Alias);
				if( /*!bMtInConf*/
					!m_tConfAllMtInfo.MtJoinedConf( byMtId )&& 
					VCS_CONF == m_tConf.GetConfSource() && 				
					CONF_CREATE_MT != m_byCreateBy
					)
				{
					TMt tMt = m_ptMtTable->GetMt( byMtId );
					m_cVCSConfStatus.OprNewMt(tMt, TRUE);
					VCSConfStatusNotif();
				}
				
				cServMsg.SetConfIdx( m_byConfIdx );
				cServMsg.SetConfId( m_tConf.GetConfId() );
				cServMsg.SetDstMtId( byMtId );
				cServMsg.SetMsgBody(&byEncrypt, sizeof(u8));
				TCapSupport tCap = m_tConf.GetCapSupport();
				cServMsg.CatMsgBody( (u8*)&tCap, sizeof(tCap));
				TMtAlias tAlias;
				tAlias.SetH323Alias(m_tConf.GetConfName());
				cServMsg.CatMsgBody((u8 *)&tAlias, sizeof(tAlias));

				// Ex能力集需要带到本端Mtadp侧，避免出现本端Mtadp侧没有进行能力集更新的时候（即还没有Ex能力时），对端能力集到来，
				// 导致比出的共同双流能力有误
				TCapSupportEx tCapEx = m_tConf.GetCapSupportEx();
				cServMsg.CatMsgBody( (u8*)&tCapEx, sizeof(tCapEx));

				// 组织扩展能力集勾选给Mtadp [12/8/2011 chendaiwei]
				TVideoStreamCap atMSVideoCap[MAX_CONF_CAP_EX_NUM];
				u8 byCapNum = MAX_CONF_CAP_EX_NUM;
				m_tConfEx.GetMainStreamCapEx(atMSVideoCap,byCapNum);
				
				TVideoStreamCap atDSVideoCap[MAX_CONF_CAP_EX_NUM];
				u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;
				m_tConfEx.GetDoubleStreamCapEx(atDSVideoCap,byDSCapNum);
				
				cServMsg.CatMsgBody((u8*)&atMSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
				cServMsg.CatMsgBody((u8*)&atDSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);

				TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//音频能力
				//从会议属性中取出会议支持的音频类型
				m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
				cServMsg.CatMsgBody((u8*)&atAudioTypeDesc[0], sizeof(TAudioTypeDesc)* MAXNUM_CONF_AUDIOTYPE);

				cServMsg.SetEventId( MCU_MT_CREATECONF_ACK );
				cServMsg.SetSrcMtId(CONF_CREATE_MT);
				cServMsg.SetSrcDriId( bySrcDriId );
				cServMsg.SetSrcSsnId( bySrcSsnId );
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn(bySrcDriId, MCU_MT_CREATECONF_ACK, cServMsg);			
				
				g_cMcuVcApp.UpdateAgentAuthMtNum();
			}
			else
			{
				cServMsg.SetSrcMtId(CONF_CREATE_MT);
				cServMsg.SetSrcDriId( bySrcDriId );
				cServMsg.SetSrcSsnId( bySrcSsnId );
				cServMsg.SetConfIdx( m_byConfIdx );
				cServMsg.SetConfId( m_tConf.GetConfId() );
				cServMsg.SetEventId( MCU_MT_CREATECONF_NACK );
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn(bySrcDriId, MCU_MT_CREATECONF_NACK, cServMsg);

				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt 0x%x join conf %s request was refused because conf full!\n", 
						  tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
			}

			//cServMsg.SetEventId( MCU_MT_CREATECONF_ACK );
			//cServMsg.SetMsgBody( cServMsg.GetMsgBody(),sizeof(u8) + sizeof(TCapSupport) + sizeof(TMtAlias) );
            //g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), MCU_MT_CREATECONF_ACK , cServMsg);
        }
        break;
	case VCS_MCU_MTJOINCONF_NACK:
		{
			cServMsg.SetSrcMtId(CONF_CREATE_MT);
			cServMsg.SetSrcDriId( bySrcDriId );
			cServMsg.SetSrcSsnId( bySrcSsnId );
			cServMsg.SetConfIdx( m_byConfIdx );
			cServMsg.SetConfId( m_tConf.GetConfId() );
			cServMsg.SetEventId( MCU_MT_CREATECONF_NACK );			
			cServMsg.SetErrorCode( ERR_MCU_CONFNUM_EXCEED );			
            g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), MCU_MT_CREATECONF_NACK , cServMsg);
		}
		break;
    default:  
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuMtJoinConfRsp] Wrong message %u(%s) received\n", 
				::OspEventDesc( pcMsg->event )
				);
        break;
    }
}

/*====================================================================
函数名      ：ProcVcsMcuGroupModeConfLockCmd
功能        ：组呼会议加锁和解锁命令
算法实现    ：
引用全局变量：
输入参数说明：const CMessage * pcMsg, 传入的消息 
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
05/19/2011              朱胜泽          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuGroupModeConfLockCmd( const CMessage * pcMsg )
{
    if( STATE_ONGOING != CurState() )
    {
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVcsMcuGroupModeConfLockCmd] Wrong message %u(%s) received in state %u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        return;
    }
    
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    u8 byIsLock = *(u8*)cServMsg.GetMsgBody();

    //级联下级不能改变锁定状态
    if (m_tCascadeMMCU.IsNull())
    {
		BOOL32 bIsLock = ( 1 == byIsLock ) ? TRUE : FALSE;
        m_cVCSConfStatus.SetGroupModeLock( bIsLock);
    }
       
    VCSConfStatusNotif();
}
/*====================================================================
    函数名      ：ProcVcsMcuMsg
    功能        ：VCS相关消息处理入口
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/08                付秀华          创建

====================================================================*/
void CMcuVcInst::ProcVcsMcuMsg( const CMessage * pcMsg )
{
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVcsMcuMsg] %d(%s) passed\n", pcMsg->event, OspEventDesc(pcMsg->event));


	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	

	// 若会议处于被上级调度状态,则拒绝任何调度操作
	// 对于MCS会议，收到预案操作相关消息，应该放过校验[11/16/2012 chendaiwei]
	if ( CONF_CREATE_MT == m_byCreateBy  &&
	    !(m_tConf.GetConfSource() == MCS_CONF
		 && (VCS_MCU_ADDPLANNAME_REQ == pcMsg->event  ||
		    VCS_MCU_DELPLANNAME_REQ == pcMsg->event   ||
		    VCS_MCU_MODIFYPLANNAME_REQ == pcMsg->event||
		    VCS_MCU_GETALLPLANDATA_REQ == pcMsg->event ))
		)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS,"[ProcVcsMcuMsg]Conf is Cascade by other mcu.So Can't do anything.\n" );
		cServMsg.SetErrorCode(ERR_MCU_VCS_MMCUINVC);
		cServMsg.SetEventId(cServMsg.GetEventId() + 2);
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}

	//开会时清空实例
	if( MCU_SCHEDULE_VCSCONF_START == pcMsg->event )
	{
		//清空实例
		ClearVcInst();
	}
	
	switch(pcMsg->event)
	{
	case MCU_SCHEDULE_VCSCONF_START:
		ProcVcsMcuCreateConfReq(pcMsg);
		break;
	case VCS_MCU_VCMT_REQ:
		ProcVcsMcuVCMTReq(pcMsg);
		break;
	case VCS_MCU_VCMODE_REQ:
		ProcVcsMcuVCModeReq(pcMsg);
	    break;
	case VCS_MCU_CHGVCMODE_REQ:
		ProcVcsMcuVCModeChgReq(pcMsg);
		break;
	case VCS_MCU_MUTE_REQ:
		ProcVcsMcuMuteReq(cServMsg);
		break;
	case VCS_MCU_GROUPCALLMT_REQ:
		ProcVcsMcuGroupCallMtReq(pcMsg);
		break;
	case VCS_MCU_GROUPDROPMT_REQ:
		ProcVcsMcuGroupDropMtReq(pcMsg);
		break;
	case VCS_MCU_STARTCHAIRMANPOLL_REQ:
		ProcVcsMcuStartChairPollReq(pcMsg);
		break;
	case VCS_MCU_STOPCHAIRMANPOLL_REQ:
		ProcVcsMcuStopChairPollReq(pcMsg);
		break;
	case VCS_MCU_ADDMT_REQ:
		ProcVcsMcuAddMtReq(pcMsg);
		break;
	case VCS_MCU_DELMT_REQ:
	    ProcVcsMcuDelMtReq(pcMsg);
		break;
	//zjj20091026
	case VCS_MCU_SETMTINTVWALL_REQ:
		ProcVcsMcuMtInTwMsg( pcMsg );
		break;

	//zjj20091102
	case VCS_MCU_ADDPLANNAME_REQ://VCS请求添加预案
		ProcMcsVcsMcuAddPlanName( pcMsg );
		break;
	case VCS_MCU_DELPLANNAME_REQ:			//VCS请求删除预案
		ProcMcsVcsMcuDelPlanName( pcMsg );
		break;
	case VCS_MCU_MODIFYPLANNAME_REQ:		//VCS请求修改预案名称
		ProcMcsVcsMcuModifyPlanName( pcMsg );
		break;
	//case VCS_MCU_SAVEPLANDATA_REQ:			//VCS请求保存某个预案
		//ProcVcsMcuSavePlanData( pcMsg );
		//break;
	case VCS_MCU_GETALLPLANDATA_REQ:		//VCS请求获得所有预案数据信息
		ProcMcsVcsMcuGetAllPlanData( pcMsg );
		break;
	case VCS_MCU_CHGTVWALLMODE_REQ:			//vcs请求mcu改变电视墙模式
		ProcVcsMcuChgTvWallMgrMode( pcMsg );
		break;
	//case VCS_MCU_REVIEWMODEMTINTVWALL_REQ:	//VCS通知mcu终端进入电视墙通道(只限于在预览模式下)
		//break;
	case VCS_MCU_CLEARONETVWALLCHANNEL_CMD:	//VCS通知mcu清除某个电视墙通道信息(只限于非自动模式)
		ProcVcsMcuClearOneTvWallChannel( pcMsg );
		break;
	case VCS_MCU_CLEARALLTVWALLCHANNEL_CMD:	//VCS通知mcu清除全部电视墙通道信息(只限于非自动模式)
		ProcVcsMcuClearAllTvWallChannel();
		break;
	case VCS_MCU_TVWALLSTARMIXING_CMD:		//vcs通知mcu设置电视墙通道终端混音状态	
		ProcVcsMcuTvWallMtStartMixing( pcMsg );
		break;

	case VCS_MCU_START_MONITOR_UNION_REQ:
		ProcVcsMcuStartMonReq(pcMsg);
		break;

	case VCS_MCU_STOP_MONITOR_UNION_REQ:
		ProcVcsMcuStopMonReq(pcMsg);
		break;
	case VCS_MCU_STOP_ALL_MONITORUNION_CMD:
		ProcVcsMcuStopAllMonCmd(pcMsg);
		break;
	/*case VCS_MCU_LOCKPLAN_REQ:				//vcs请求锁定或解锁某个预案
		ProcVcsMcuLockPlan( pcMsg );
		break;
	case VCS_MCU_VCSUNLOCKPLAN_CMD:
		ProcVcsMcuUnLockPlanByVcsssnInst( pcMsg );
		break;
	*/
	case VCS_MCU_CHGDUALSTATUS_REQ:
		ProcVcsMcuChangeDualStatusReq( pcMsg );
        break;

	case VCS_MCU_MTJOINCONF_ACK:			//终端入会申请同意
	case VCS_MCU_MTJOINCONF_NACK:			//终端入会申请拒绝	
		ProcVcsMcuMtJoinConfRsp( pcMsg );
		break;
    case VCS_MCU_GROUPMODELOCK_CMD://[5/19/2011 zhushengze]组呼会议锁定状态控制
        ProcVcsMcuGroupModeConfLockCmd( pcMsg );
		break;    
	default:
	    break;
	}
}
/*====================================================================
    函数名      ：ProcVcsMcuRlsMtMsg
    功能        ：VCS抢占中释放终端消息处理入口
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/08                付秀华          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuRlsMtMsg( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	if (cServMsg.GetMsgBodyLen() < sizeof(TMt))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuRlsMtMsg] wrong msg lenth(real:%d, less than %d)\n",
			    cServMsg.GetMsgBodyLen(), sizeof(TMt));
		return;
	}
	u8* pbyMsgBody = cServMsg.GetMsgBody();
	TMt tMt = *(TMt*)pbyMsgBody;
	pbyMsgBody += sizeof(TMt);

	s8 abyConfName[MAXLEN_CONFNAME + 1] = {0};
	u8 byPermitOrNot = FALSE;

	const u16 wCpyLen = cServMsg.GetMsgBodyLen() - sizeof(TMt);
	memcpy(abyConfName, pbyMsgBody, min(MAXLEN_CONFNAME, wCpyLen));
	if (VCS_MCU_RELEASEMT_ACK == pcMsg->event)
	{
		cServMsg.SetEventId(MCU_MT_RELEASEMT_ACK);
		byPermitOrNot = TRUE;
	}
	else if (VCS_MCU_RELEASEMT_NACK == pcMsg->event)
	{
		cServMsg.SetEventId(MCU_MT_RELEASEMT_NACK);
		byPermitOrNot = FALSE;
	}
	else
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuRlsMtMsg] receive un exist msg(%d)\n", pcMsg->event);
		return;
	}

	SendMsgToAllMcs(MCU_VCS_RELEASEMT_NOTIF, cServMsg);

	cServMsg.SetMsgBody(&byPermitOrNot, sizeof(byPermitOrNot));
	cServMsg.CatMsgBody((u8*)abyConfName,  strlen(abyConfName));
	SendMsgToMt(tMt.GetMtId(), cServMsg.GetEventId(), cServMsg);


}
/*====================================================================
    函数名      ：VCSConfDataCoordinate
    功能        ：VCS创会会议数据整理
	              对于主席(即调度席绑定的终端)、高清电视墙终端、下级mcu采取无限次定时呼叫
				  而对于其它本级可调度终端采取手动呼叫的方式
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
void CMcuVcInst::VCSConfDataCoordinate(CServMsg &cServMsg, u8 &byMtNum, u8 &byCreateMtId)
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
	TMtAlias tBackAlias ;

    u8 byLoop = 0;
    u8 byMtId = 0;
    u8 byMtType = TYPE_MT;
    s8 *pszAliaseBuf = NULL;
    u16 wAliasBufLen  = 0;
    u16 awMtDialRate[MAXNUM_CONF_MT];
    s8 *pszMsgBuf = NULL;
    TMtAlias tMtAliasArray[MAXNUM_CONF_MT];

    wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
    pszAliaseBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16));
    // 解析可调度的终端列表
    UnPackTMtAliasArray( pszAliaseBuf, wAliasBufLen, &m_tConf, 
                         tMtAliasArray, awMtDialRate, byMtNum );

	//zjj20110411 组呼终端列表不包括上级mcu
	if( CONF_CREATE_MT == m_byCreateBy) 
	{
		m_tOrginMtList.SetMtList(byMtNum-1, &tMtAliasArray[1], &awMtDialRate[1]);
	}
	else
	{		
		m_tOrginMtList.SetMtList(byMtNum, tMtAliasArray, awMtDialRate);
	}

	// 得到创建者的别名
	if( CONF_CREATE_MT == m_byCreateBy )
	{
        wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
		u32 nLen = sizeof(TConfInfo)+sizeof(u16)+wAliasBufLen;
		TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
		if(tConfAttrb.IsHasTvWallModule())
			nLen += sizeof(TMultiTvWallModule);
		if(tConfAttrb.IsHasVmpModule())
			nLen += sizeof(TVmpModule);
		u8 byIsHDTWCfg = *(u8*)(cServMsg.GetMsgBody() + nLen);
		nLen += sizeof(u8);
		if (byIsHDTWCfg)
		{
			nLen += sizeof(THDTvWall);
		}
		
		//下级hdunum默认设置为0
		u8 byHduChnlNum = *(u8*)(cServMsg.GetMsgBody() + nLen);
		nLen += sizeof(u8);
		
		u8 byIsVCSSMCUCfg = *(u8*)(cServMsg.GetMsgBody() + nLen);
		nLen += sizeof(u8);
		if (byIsVCSSMCUCfg)
		{
			u16 wAliasBufLen2 = ntohs(*(u16*)(cServMsg.GetMsgBody() + nLen));
			nLen += sizeof(u16) + wAliasBufLen2;
		}
		u8 byMTPackExist = *(u8*)(cServMsg.GetMsgBody() + nLen);
		nLen += sizeof(u8);
		tBackAlias = *(TMtAlias *)(cServMsg.GetMsgBody() + nLen);
		nLen += sizeof(TMtAlias);
		nLen += sizeof(u8);

		// wConfExInfoLength长度有可能为0，终端创会时无条件添加u16[11/14/2012 chendaiwei]
		u16 wConfExInfoLength = ntohs( *(u16*)(cServMsg.GetMsgBody() + nLen));
		nLen = nLen + wConfExInfoLength + sizeof(u16);

		if( cServMsg.GetMsgBodyLen() > nLen )
		{
			ptMtH323Alias = (TMtAlias *)(cServMsg.GetMsgBody()+nLen);
			ptMtE164Alias = (TMtAlias *)(cServMsg.GetMsgBody()+nLen)+1;
			ptMtAddr = (TMtAlias *)(cServMsg.GetMsgBody()+nLen)+2;
			byMtType = *(cServMsg.GetMsgBody()+nLen+sizeof(TMtAlias)*3);
		}
	}
	
	//置无终端与会状态 
	m_tConfAllMtInfo.RemoveAllJoinedMt();

	// 仅增加主席终端及上级MCU成员
	if( CONF_CREATE_MT == m_byCreateBy) 
	{
		byMtId = AddMt(tMtAliasArray[0], awMtDialRate[0], CONF_CALLMODE_NONE);
        
        // zbq [12/19/2007] 直接创会终端须在此补入IP，避免E164优先情况下的漏填
        if ( tMtAliasArray[0].m_AliasType != mtAliasTypeTransportAddress && ptMtAddr )
        {
            m_ptMtTable->SetMtAlias( byMtId, ptMtAddr );
            m_ptMtTable->SetIPAddr( byMtId, ptMtAddr->m_tTransportAddr.GetIpAddr() );                    
        }

        // xsl [11/8/2006] 接入板终端计数加1
		// [9/9/2010 xliang] 区分是MT还是MCU, 是MCU则接入板终端计数需额外再+1
// 		u16 wExtraNum = (byMtType == TYPE_MCU)? 1: 0;
//         g_cMcuVcApp.IncMtAdpMtNum( cServMsg.GetSrcDriId(), m_byConfIdx, byMtId, wExtraNum );
        g_cMcuVcApp.IncMtAdpMtNum( cServMsg.GetSrcDriId(), m_byConfIdx, byMtId, byMtType );
        m_ptMtTable->SetDriId(byMtId, cServMsg.GetSrcDriId());
	
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
		}
		else
		{
			m_ptMtTable->SetMtType(byMtId, MT_TYPE_MT);
		}
	}
	//处理主席
    TMtAlias tMtAlias = m_tConf.GetChairAlias();
	u8 byChairMtId = AddMt(tMtAlias);
	m_tConf.SetNoChairman();
	if( byChairMtId > 0 )
	{
        m_tConf.SetChairman( m_ptMtTable->GetMt(byChairMtId) );
	}

	pszMsgBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16) + wAliasBufLen);

	//处理电视墙模板
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if( tConfAttrb.IsHasTvWallModule() )
	{
        TPeriEqpStatus tTWStatus;
		TMultiTvWallModule *ptMultiTvWallModule = (TMultiTvWallModule *)pszMsgBuf;
        m_tConfEqpModule.SetMultiTvWallModule( *ptMultiTvWallModule );
        m_tConfEqpModule.SetTvWallInfo( *ptMultiTvWallModule );

		CServMsg CTvwEqpStatusMsg;
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
				
				// [9/19/2011 liuxu] 如果该电视墙被其它会议占用了,则不能直接修改
				TTvwMember tTempTvwMbr = tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop];
				if ( tTempTvwMbr.byMemberType != TW_MEMBERTYPE_NULL
					&& tTempTvwMbr.GetConfIdx() != 0
					&& tTempTvwMbr.GetConfIdx() != m_byConfIdx )
				{
					continue;
				}

				// VCS电视墙模板目前采取动态配置成员，因而模板中通道不指定成员，但需要指定哪些通道使用
				// 使用时将指定通道按顺序使用，将可用通道信息保存到外设状态中
				// [10/19/2011 liuxu] 开会时不预占用
//                 tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byMemberType;
//                 tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);

				// 仍支持VCS预指定通道成员
				if( byMtIndex > 0 )//索引以1为基准
				{
                    if( byMtIndex <= MAXNUM_CONF_MT )
                    {
                        byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAliasArray[byMtIndex-1] );
                        if( byMtId > 0 )
                        {
                            TMt tMt = m_ptMtTable->GetMt( byMtId );
                            m_tConfEqpModule.SetTvWallMemberInTvInfo( tTvWallModule.m_tTvWall.GetEqpId(), byLoop, tMt );
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

						case TW_MEMBERTYPE_SWITCHVMP:
							tMt.SetNull();
							bValid = TRUE;
							break;

                        case TW_MEMBERTYPE_DOUBLESTREAM:
                            tMt = m_tDoubleStreamSrc;
                            bValid = TRUE;
                            break;

                        default:
                            ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Invalid member type %d for idx %d in CreateConf TvWall Module\n", 
                                    byMemberType, 
                                    byLoop);        
                            bValid = FALSE;
                        }
                        if (bValid)
                        {
                            // 标记下来，等终端上来。会触发主席发言人变更
                            m_tConfEqpModule.SetTvWallMemberInTvInfo( tTvWallModule.m_tTvWall.GetEqpId(), byLoop, tMt );
                         }                            
                    }
                    else
                    {
                        ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Invalid MtIndex :%d in CreateConf TvWall Module\n", byMtIndex);
                    }
				}
			}
			
            g_cMcuVcApp.SetPeriEqpStatus(tTvWallModule.m_tTvWall.GetEqpId(), &tTWStatus);

			if (tTWStatus.m_byOnline)
			{
				// 通知会控
				CTvwEqpStatusMsg.SetMsgBody((u8*)&tTWStatus, sizeof(tTWStatus));
				SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, CTvwEqpStatusMsg);
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

	// VCS: + 1byte(u8: 0 1  是否配置了高清电视墙)
	//      +(可选, THDTvWall])
	//      + 1byte(u8: 会议配置HDU的通道总数)
	//      + (可选, THduModChnlInfo+...)
	//      + 1byte(是否为级联调度)＋(可选，2byte[u16 网络序，配置打包的总长度]+下级mcu配置[1byte(别名类型)+1byte(别名长度)+xbyte(别名字符串)+2byte(呼叫码率)...))
	//      + 1byte(是否支持分组)＋(可选，2byte(u16 网络序，信息总长)+内容(1byte(组数)+n组[1TVCSGroupInfo＋m个TVCSEntryInfo])
	//       + 1 TMtAlias (本地地址的备份)
	//      + 1byte (vcs 自动进入该指定的模式)
	// 高清电视墙
    if (*pszMsgBuf++)
    {
		m_tConfEqpModule.SetHDTvWall((THDTvWall*)pszMsgBuf);
		for( byLoop = 1; byLoop <= m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_tHDTvWall.m_byHDTWDevNum; byLoop++)
		{
			TMtAlias tTmpAlias;
			memcpy(&tTmpAlias,&m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_tHDTvWall.m_atHDTWModuleAlias[byLoop-1],sizeof(THDTWModuleAlias));

			// 添加高清到受邀列表中，且为无限次定时呼叫
			byMtId = AddMt( tTmpAlias, 
							m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_tHDTvWall.m_awHDTWDialBitRate[byLoop-1],
							CONF_CALLMODE_TIMER );
		}
		pszMsgBuf += sizeof(THDTvWall);
		
		byMtNum += m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.m_union.m_tHDTvWall.m_byHDTWDevNum;
    }
	else
	{
		// 未配置高清电视墙，清空参数
		THDTvWall tNull;
		tNull.SetNull();
		m_tConfEqpModule.SetHDTvWall(&tNull);
	}

	// HDU
	u8 byHduChnlNum = *pszMsgBuf++;
	if (byHduChnlNum)
	{
		m_tConfEqpModule.m_tHduModule.SetHduModuleInfo(byHduChnlNum, pszMsgBuf);
		pszMsgBuf += byHduChnlNum * sizeof(THduModChnlInfo);
	}
	else
	{
		m_tConfEqpModule.m_tHduModule.SetNull();
	}

	// 下级MCU
	// 采取无限次定时呼叫
    if (*pszMsgBuf++)
    {
		u8 byDevNum = 0;
		wAliasBufLen = ntohs( *(u16*)pszMsgBuf );
		pszAliaseBuf = (char*)(pszMsgBuf + sizeof(u16));
		// 解析作为高清电视墙的高清终端列表
		UnPackTMtAliasArray( pszAliaseBuf, wAliasBufLen, &m_tConf, 
							 tMtAliasArray, awMtDialRate, byDevNum );

		for( byLoop = 1; byLoop <= byDevNum; byLoop++)
		{
			// 添加高清到受邀列表中，且为无限次定时呼叫
			byMtId = AddMt( tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], CONF_CALLMODE_NONE );
		}
		pszAliaseBuf += sizeof(u16) + wAliasBufLen;
		
		byMtNum += byDevNum;
    }
	pszMsgBuf++;
	//zhouyiliang 20100820 读本地终端的备份
	
	memcpy( &tBackAlias, pszMsgBuf, sizeof(TMtAlias));
	pszMsgBuf += sizeof(TMtAlias);
	if ( !tBackAlias.IsNull() ) 
	{
		u8 bybackMt = AddMt( tBackAlias );
		//zhouyiliang 20100818 本机地址备份地址记录
		if ( bybackMt > 0 ) 
		{
			m_cVCSConfStatus.SetVcsBackupChairMan( m_ptMtTable->GetMt( bybackMt ) );

		}
	}
	else
	{
		TMt tNullMt;
		tNullMt.SetNull();
		m_cVCSConfStatus.SetVcsBackupChairMan( tNullMt );
	}

	// [4/20/2011 xliang] vcs AutoMode
	u8 byVcsAutoMode = *(u8*)pszMsgBuf;
	if( ISGROUPMODE(byVcsAutoMode) )
	{	
		m_cVCSConfStatus.SetCurVCMode(byVcsAutoMode); 
	}
	pszMsgBuf++;

	//保存扩展能力集勾选信息[12/27/2011 chendaiwei]
	m_tConfEx.Clear();
	TConfInfoEx tTempConfEx = GetConfInfoExFromConfAttrb(m_tConf);
	
	if(m_tConf.HasConfExInfo())
	{
		u16 wConfExLength = 0;
		BOOL32 bExistUnknowType = FALSE;
		UnPackConfInfoEx(m_tConfEx,(u8*)pszMsgBuf,wConfExLength,bExistUnknowType);
		TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];
		u8   byAudioCapNum = m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
		if (byAudioCapNum > 0)//会议扩展信息中有音频能力信息则需要用扩展信息中去覆盖
		{
			tTempConfEx.SetAudioTypeDesc(atAudioTypeDesc,byAudioCapNum);
		}
		m_tConfEx.AddCapExInfo(tTempConfEx);
	}
	else
	{
		m_tConfEx = tTempConfEx;
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

	if(tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264
		&& tCapSupport.GetDStreamMediaType() == MEDIA_TYPE_H264)
	{
		TDStreamCap tTempDsCap = tCapSupport.GetDStreamCapSet();
		
#ifdef _8KH_	
		tTempDsCap.SetH264ProfileAttrb(tCapSupport.GetMainStreamProfileAttrb());
#elif defined _8KE_
		//3. 8KG双流始终为BP(此处保护)
		tTempDsCap.SetH264ProfileAttrb(emBpAttrb);
#else      
		//4. 8000A依赖于主流 
		tTempDsCap.SetH264ProfileAttrb(tCapSupport.GetMainStreamProfileAttrb());
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

	//RestoreVCConf(byVcsAutoMode);

	g_cMcuVcApp.UpdateAgentAuthMtNum();

    return;
}
/*====================================================================
    函数名      ：IsVCSEqpInfoCheckPass
    功能        ：外设是否满足要求，并预留部分外设资源
    算法实现    ：
    引用全局变量：
    输入参数说明：u16       &wErrCode : [OUT]不充分错误码
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/05                付秀华        创建
====================================================================*/
BOOL32 CMcuVcInst::IsVCSEqpInfoCheckPass(u16 &wErrCode)
{
    if ( m_tConf.GetConfId().IsNull() )
    {
        ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsVCSEqpInfoCheckPass] tConfInfo.IsNull, ignore it\n" );
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
            ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsVCSEqpInfoCheckPass]Conference %s create failed because of multicast address occupied!\n", m_tConf.GetConfName() );
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
            ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsVCSEqpInfoCheckPass]Conference %s create failed because of distributed conf multicast address occupied!\n", m_tConf.GetConfName() );
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
            ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsVCSEqpInfoCheckPass]Conference %s create failed because of no DCS registered!\n", m_tConf.GetConfName() );
            return FALSE;
        }
    }

	//但对于VCS会议，默认模板设置为启用适配器，此时不进行预留，待调度时根据需要启用
	//对于画面合成器及电视墙均不进行资源效验预留

	//需要丢包重传的会议,预留包重传通道 
	if (tConfAttrib.IsResendLosePack() && !AssignPrsChnnl4Conf())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsVCSEqpInfoCheckPass] not enough prs chanl\n");
	}

    // guzh [8/29/2007] 双媒体会议能力限制,不支持低速组播
#ifdef _MINIMCU_
    if ( ISTRUE(m_byIsDoubleMediaConf) )                
    {
        if ( m_tConf.GetConfAttrb().IsMulticastMode() && 
             m_tConf.GetConfAttrb().IsMulcastLowStream() )
        {
            wErrCode = ERR_MCU_MCLOWNOSUPPORT;
            ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Conf.%s not support multicast low stream\n", m_tConf.GetConfName() );
            return FALSE;                
        }
    }
#endif

    //会议开始自动录像
	//  [12/27/2009 pengjie] Modify 这里修改不能隔天定时录像问题，并把该部分代码提取函数接口
	if( !SetAutoRec(wErrCode) )
	{
		return FALSE;
	}
	// End Modify

	//把会议指针存到数据区
	if( !g_cMcuVcApp.AddConf( this ) )
	{
		wErrCode = ERR_MCU_CONFOVERFLOW;
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "Conference %s create failed because of full instance!\n", m_tConf.GetConfName() );
		return FALSE;
	}

    return TRUE;
}

/*====================================================================
    函数名      ：VCSCallMT
    功能        ：VCS呼叫终端
    算法实现    ：
    引用全局变量：
    输入参数说明：cServMsgHdr: 请求的消息头
				  TMt:         邀请的终端
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
void CMcuVcInst::VCSCallMT(CServMsg& cServMsgHdr, TMt& tMt, u8  byCallType)
{
	CServMsg cServMsg;
	cServMsg.SetServMsg(cServMsgHdr.GetServMsg(), SERV_MSGHEAD_LEN);
    cServMsg.SetConfIdx(m_byConfIdx) ;
    cServMsg.SetConfId(m_tConf.GetConfId());

	//不在受邀终端列表中
    TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMcuId());
	if(tMtInfo.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSCallMT] Call MT(MCUID:%d MTID:%d) whose mcu is not in conf\n",
			           tMt.GetMcuId(), tMt.GetMtId());
		//终端不在可调度的资源范围内
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINVC );
		cServMsg.SetEventId( cServMsg.GetEventId() + 2 );
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}

	if( tMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{	
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSCallMT] Call MT(MCUID:%d MTID:%d) has in conf\n",
			           tMt.GetMcuId(), tMt.GetMtId());
		return;
	}

	TMtAlias tMtAlias;
    if (tMt.IsLocal() && 
		!m_ptMtTable->GetDialAlias( tMt.GetMtId(), &tMtAlias ))
    {
        cServMsg.SetErrorCode( ERR_MCU_CALLEDMT_NOADDRINFO);
		cServMsg.SetEventId( cServMsg.GetEventId() + 2 );
		SendReplyBack(cServMsg, cServMsg.GetEventId());
        ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSCallMT] GetDialAlias failed, byMtId.%d\n", tMt.GetMtId());
        return;
	}

	cServMsg.SetEventId( cServMsg.GetEventId() + 1 );	
	SendReplyBack(cServMsg, cServMsg.GetEventId());
	

	//zhouyiliang20100702 取得debuginfo文件中设置的超时时间,单位是s，所以用时x1000
	u8 byVcMtOverTime = g_cMcuVcApp.GetVcsMtOverTimeInterval();

	if (VCS_FORCECALL_CMD == byCallType)
	{
		// 抢占型终端调度最大时间间隔为调度终端的最大时间间隔+5s
		SetTimer( MCUVC_VCMTOVERTIMER_TIMER, (byVcMtOverTime + 5)*1000  );
	}
	else
	{
		// 设置调度终端的最大时间间隔，间隔时间后终端未呼上则提示调度终端超时
		//zjj 20090908 改为10秒===》zhouyiliang20100702改为从debuginfo文件读取。
		SetTimer( MCUVC_VCMTOVERTIMER_TIMER, byVcMtOverTime*1000 );
	}
	
	//不在本MCU上
	if( !tMt.IsLocal())
	{
		cServMsg.SetEventId( cServMsg.GetEventId() + 1 );	
		SendReplyBack(cServMsg, cServMsg.GetEventId());

		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
			sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
		astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
			sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
		
		TMsgHeadMsg tHeadMsg;
		//u8 byFstMcuId = 0,bySecMcuId = 0;
		u8 abyMcuId[MAX_CASCADEDEPTH - 1];
		memset( &abyMcuId[0],0,sizeof( abyMcuId ) );
		memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
		tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		cServMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
		SendMsgToMt( (u8)tMt.GetMcuId(), MCU_MCU_REINVITEMT_REQ, cServMsg );
		return;
	}
	else
	{
		//邀请本地终端
		InviteUnjoinedMt(cServMsg, &tMt, FALSE, FALSE, byCallType);
	}

	return;
}

/*====================================================================
    函数名      ：VCSDropMT
    功能        ：VCS挂断终端
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt:     邀请的终端
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
void CMcuVcInst::VCSDropMT(TMt tMt)
{
	CServMsg cServMsg;
	cServMsg.SetSrcSsnId(m_cVCSConfStatus.GetCurSrcSsnId());
    cServMsg.SetConfIdx(m_byConfIdx);
    cServMsg.SetConfId(m_tConf.GetConfId()) ;

	//不在受邀终端列表中
	TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMcuId());
	if(tMtInfo.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSDropMT] Del MT(MCUID:%d MTID:%d) whose mcu is not in conf\n",
			            tMt.GetMcuId(), tMt.GetMtId());
		return;
	}
	if(!tMtInfo.MtJoinedConf(tMt.GetMtId()))
	{	
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSDropMT] Del MT(MCUID:%d MTID:%d) has not in conf\n",
			            tMt.GetMcuId(), tMt.GetMtId());
		return;
	}

	// [8/12/2010 xliang] 主席不能被挂断保护
	if(m_tConf.GetChairman() == tMt)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSDropMT] chairman(%d, %d) shouldn't be dropped!\n");
		return;
	}
	
	ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "VCSDropMT(%d,%d)\n", tMt.GetMcuId(), tMt.GetMtId());

    TMt tVCMT = m_cVCSConfStatus.GetCurVCMT();
	TMt tNullMT;
	tNullMT.SetNull();
	TMt tChairMan = m_tConf.GetChairman();
	if (tMt == GetLocalMtFromOtherMcuMt(m_cVCSConfStatus.GetCurVCMT()))
	{
		m_cVCSConfStatus.SetCurVCMT(tNullMT);
//		ChangeConfRestoreMixParam();
		if (VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
			!m_tConf.GetChairman().IsNull())
		{			
			if (m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMtId()))
			{				
				NotifyMtReceive(tChairMan, tChairMan.GetMtId());								
			}
		}

		// 如果对于组呼发言人及组呼电视墙模式, 挂断当前发言人自动更改本地终端作发言人
		u8 byMode = m_cVCSConfStatus.GetCurVCMode();
		if (VCS_GROUPSPEAK_MODE == byMode || VCS_GROUPTW_MODE == byMode)
		{
			TMt tChairMan = m_tConf.GetChairman();
			ChangeSpeaker(&tChairMan, FALSE, FALSE);
		}
	}
	else
	{
		//yrl20131120删除电视墙中的其他终端会把主席选看的交换拆掉
		if( VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
			tMt == m_cVCSConfStatus.GetCurVCMT() &&
			!tChairMan.IsNull() )
		{
			StopSelectSrc( tChairMan,MODE_BOTH,FALSE,TRUE );
		}					
	}

	if (tMt == GetLocalMtFromOtherMcuMt(m_cVCSConfStatus.GetReqVCMT()))
	{
		m_cVCSConfStatus.SetReqVCMT(tNullMT);
	}
	if ((tMt.IsLocal() && m_ptMtTable->IsMtInMixing(tMt.GetMtId())) || 
		(!tMt.IsLocal() && m_ptConfOtherMcTable->IsMtInMixing(tMt)))
    {
		RemoveSpecMixMember(&tMt, 1, FALSE,TRUE);
    }
	//该终端是非直连终端
	if( !tMt.IsLocal())
	{
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy( tReq.m_szUserName, tMcsReg.m_achUser, 
				  sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser) );
		astrncpy( tReq.m_szUserPwd, tMcsReg.m_achPwd, 
				  sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd) );

		u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
		memset( &abyMcuId[0],0,sizeof(abyMcuId) );
		TMsgHeadMsg tHeadMsg;
		TMt tDstMt;
		memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
		tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tDstMt );
			
		//u8 byMcuId = GetFstMcuIdFromMcuIdx(tMt.GetMcuId());
		cServMsg.SetEventId(MCU_MCU_DROPMT_REQ);
		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody((u8 *)&tDstMt, sizeof(TMt));
		cServMsg.CatMsgBody((u8 *)&tHeadMsg, sizeof(TMsgHeadMsg));
		SendMsgToMt( (u8)tDstMt.GetMcuId(), MCU_MCU_DROPMT_REQ, cServMsg );

		TMt tSMCU = m_ptMtTable->GetMt( (u8)tDstMt.GetMcuId() );//tMt.GetMcuId());
		if (m_cVCSConfStatus.GetCurVCMode() != VCS_MULVMP_MODE)
		{
			// 取消选看
			VCSConfStopMTSel(tSMCU, MODE_BOTH);
			// 取消被选看
			VCSConfStopMTSeled(tSMCU, MODE_BOTH);
		}

		// 若回传通道放该终端，清空信息
		TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMcuId());
		if (ptMcInfo != NULL && tMt == ptMcInfo->m_tMMcuViewMt)
		{
			VCSConfStopMTToTW(ptMcInfo->m_tMMcuViewMt);
			ptMcInfo->m_tMMcuViewMt.SetNull();		
		}

		//zbq [05/13/2010] 清出等待申请队列，FIXME: 跨级标识？
		/*OL32 bIsSendToChairman = FALSE;
        if (m_tApplySpeakQue.IsMtInQueue(tMt) )
        {
            m_tApplySpeakQue.ProcQueueInfo(tMt, bIsSendToChairman, FALSE);
            NotifyMcsApplyList( bIsSendToChairman );
        }*/
		return;
	}	

	TMt tLocalMt = tMt;
	//挂断本地终端前，先将终端恢复成非静哑音状态
	//若为高清终端，需要恢复分辨率
	if (tMt.IsLocal())
	{
		VCSMTMute(tMt, FALSE, VCS_AUDPROC_MUTE);
		VCSMTMute(tMt, FALSE, VCS_AUDPROC_SILENCE);
	}
	else
	{
		tLocalMt = GetLocalMtFromOtherMcuMt( tMt );
	}

	TLogicalChannel tSrcRvsChannl;

	if ( m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
	{
		if ( tSrcRvsChannl.GetChannelType() == MEDIA_TYPE_H264 &&
			( IsVidFormatHD( tSrcRvsChannl.GetVideoFormat() ) ||
			  tSrcRvsChannl.GetVideoFormat() == VIDEO_FORMAT_4CIF ) )
		{
			ChangeVFormat(tMt/*, tSrcRvsChannl.GetVideoFormat()*/);
		}
	}

	RemoveJoinedMt( tMt, TRUE );

	//zjj20100330 
	TMtStatus tMtStatus;
	if( m_ptMtTable->GetMtStatus(tMt.GetMtId(),&tMtStatus) )
	{
		tMtStatus.SetMtDisconnectReason( MTLEFT_REASON_NORMAL );
		m_ptMtTable->SetMtStatus( tMt.GetMtId(),&tMtStatus );
	}
	
	//zjj20100325 组呼下手动挂断一个终端端，呼叫模式改为手动
	if( ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode() ) && !(tMt == m_tConf.GetChairman()) &&
		!(tMt == m_cVCSConfStatus.GetVcsBackupChairMan()) )
	{		
		m_ptMtTable->SetCallMode( GetLocalMtFromOtherMcuMt(tMt).GetMtId(),CONF_CALLMODE_NONE );
	}

    if( tVCMT == tMt )
    {
        TMt tSpeakMt;
        if( CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode() &&
            !m_tApplySpeakQue.IsQueueNull() )
        {
            
            if( m_tApplySpeakQue.GetQueueHead(tSpeakMt) ) 
            {			
                ChgCurVCMT( tSpeakMt );
            }
            else
            {
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"[VCSDropMT] Fail to get Queue Head!\n" );
            }
        }
        else
        {	
            tSpeakMt.SetNull();
            ChgCurVCMT( tSpeakMt );				
        }
	}
}


/*====================================================================
    函数名      ：VCSConfSelMT
    功能        ：VCS会议选看终端, 单方调度，进行音视频选看；多方进行视频选看并进行混音
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：BOOL
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
	10/03/23				周晶晶		  多回传修改
====================================================================*/
BOOL CMcuVcInst::VCSConfSelMT( TSwitchInfo tSwitchInfo,BOOL32 bNeedPreSetin /*= TRUE*/ )
{    
 
    if (CurState() != STATE_ONGOING)
    {
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CFG, "[VCSConfSelMT] InstID(%d) in wrong state(%d)\n", GetInsID(), CurState());
		return FALSE;
    }
 	CServMsg  cServMsg;
	cServMsg.SetSrcSsnId(m_cVCSConfStatus.GetCurSrcSsnId());
    cServMsg.SetConfIdx(m_byConfIdx);
    cServMsg.SetConfId(m_tConf.GetConfId()) ;

    // 如果选看终端或被选看终端为空，就直接返回
	TMt	tDstMt, tSrcMt;
    tSrcMt = tSwitchInfo.GetSrcMt();
    tDstMt = tSwitchInfo.GetDstMt();
    if ( tDstMt.IsNull() || tSrcMt.IsNull())
    {
        ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfSelMT] Mt(%d,%d) Select see Mt(%d,%d), returned with nack!\n", 
                tDstMt.GetMcuId(), 
                tDstMt.GetMtId(),
                tSrcMt.GetMcuId(), 
                tSrcMt.GetMtId());
        return FALSE;
    }    

	// 参与选看的终端均在线
	TMt tOrigDstMt = tDstMt;
	if( TYPE_MT == tDstMt.GetType() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfSelMT] Mt(%d,%d) select see Mt(%d,%d), selmode(%d)\n", 
			     tDstMt.GetMcuId(), tDstMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSwitchInfo.GetMode() );

		tDstMt = GetLocalMtFromOtherMcuMt( tDstMt );				
		tDstMt = m_ptMtTable->GetMt(tDstMt.GetMtId());
		tSwitchInfo.SetDstMt( tDstMt );	
		
        //目的终端未入会议
        if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
        {
            ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfSelMT] Specified Mt(%u,%u) not joined conference!\n", 
                     tDstMt.GetMcuId(), tDstMt.GetMtId() );
			//VCS选看业务非用户主动请求,不给提示
//          cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
// 			SendMsgToAllMcs(MCU_VCS_VCMT_NOTIF, cServMsg);
            return FALSE;
        }   
	}
 
    TMt tOrigSrcMt = tSrcMt;
	if ( TYPE_MT == tSrcMt.GetType() )
	{
		// 若为下级终端，将选看原修改为其所在的下级MCU
		//tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);
		//tSwitchInfo.SetSrcMt( tSrcMt );
		//源终端未入会议，NACK
		if( !m_tConfAllMtInfo.MtJoinedConf( GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId() ) )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfSelMT] Select source Mt(%u,%u) has not joined current conference!\n",
					 tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
			//VCS选看业务非用户主动请求,不给提示
// 			cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
// 			SendMsgToAllMcs(MCU_VCS_VCMT_NOTIF, cServMsg);
			return FALSE;
		}  
	}

    //终端状态和选看模式是否匹配,不匹配NACK
	TMtStatus tSrcMtStatus;
    TMtStatus tDstMtStatus;   
    u8 bySrcMtID = GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId();
	u8 byDstMtId = tDstMt.GetMtId();
	u8 bySwitchMode    = tSwitchInfo.GetMode();
	m_ptMtTable->GetMtStatus(bySrcMtID, &tSrcMtStatus);
    m_ptMtTable->GetMtStatus(byDstMtId, &tDstMtStatus);
	if( (!tSrcMtStatus.IsSendVideo() ||
         (tDstMt.GetType() == TYPE_MT && !tDstMtStatus.IsReceiveVideo()) ) &&
         (bySwitchMode == MODE_VIDEO || bySwitchMode == MODE_BOTH) )
	{
        bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_AUDIO : MODE_NONE;
		
	}	
    
    if( (!tSrcMtStatus.IsSendAudio() ||
         (tDstMt.GetType() == TYPE_MT && !tDstMtStatus.IsReceiveAudio()) ) &&
        ( bySwitchMode == MODE_AUDIO || bySwitchMode == MODE_BOTH ) )
    {
        bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_VIDEO : MODE_NONE;
	
    }

    if ( MODE_NONE == bySwitchMode )
    {
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfSelMT] fail to sel(%d) because of the openning of dual logical channel\n",
			    tSwitchInfo.GetMode());
        return FALSE;
	}
	else
	{
		tSwitchInfo.SetMode(bySwitchMode);
	}



	//选看模式和会议状态是否匹配
	//混音时调整选看模式
	if( m_tConf.m_tStatus.IsMixing() )
	{
        //选看音频
		if( MODE_AUDIO == bySwitchMode )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfSelMT] Conference %s is mixing now. Cannot switch only audio!\n", 
                     m_tConf.GetConfName() );
// 			cServMsg.SetErrorCode( ERR_MCU_SELAUDIO_INMIXING );
// 			SendMsgToAllMcs( MCU_VCS_VCMT_NOTIF, cServMsg );
            return FALSE;
		}
        //其他，转为选看视频
		else
		{
            //选看音视频，提示音频选看失败
            if ( MODE_BOTH == bySwitchMode )
            {
                ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfSelMT] Conference %s is mixing now. Cannot switch audio!\n", 
                         m_tConf.GetConfName() );
//              cServMsg.SetErrorCode( ERR_MCU_SELBOTH_INMIXING );
// 				SendMsgToAllMcs( MCU_VCS_VCMT_NOTIF, cServMsg );
				bySwitchMode = MODE_VIDEO;
            }
			tSwitchInfo.SetMode( bySwitchMode );
		}
	}

	if( bNeedPreSetin && !tSrcMt.IsLocal() )
	{
		if( IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()) )
		{
			TMt tDst;
			tDst.SetNull();
			TPreSetInReq tPreSetInReq;
			tDst.SetNull();
			tPreSetInReq.m_tSpyMtInfo.SetSpyMt(tSrcMt);
			tPreSetInReq.m_tSpyInfo.m_tSpySwitchInfo.m_tDstMt = tSwitchInfo.GetDstMt();
			tPreSetInReq.m_bySpyMode = tSwitchInfo.GetMode();			
			tPreSetInReq.SetEvId(VCS_MCU_CONFSELMT_CMD);
			tPreSetInReq.m_tReleaseMtInfo.SetCount(0);
			
			//zjj20100915 一下这段代码因为有bug删除，找不到是谁加的，谁有印象找我
			//vcs主席调度下，监控和主席选看互相绑定
			/*TMt tMcSrc;
			u8  byReleaseMode = MODE_NONE;
			for (u8 byLoop = 0; byLoop < MAXNUM_MCU_MC + MAXNUM_MCU_VC; byLoop++)
			{
				for (u8 byChnIdx = 0; byChnIdx < MAXNUM_MC_CHANNL; byChnIdx++)
				{
					tMcSrc.SetNull();
					if(g_cMcuVcApp.GetMcSrc(byLoop, &tMcSrc, byChnIdx, MODE_VIDEO))
					{
						if (!tMcSrc.IsNull() && tMcSrc.GetConfIdx() == m_byConfIdx)
						{
							tPreSetInReq.m_tReleaseMtInfo.m_tMt = tMcSrc;
							tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum++;
							byReleaseMode += MODE_VIDEO;
						}
					}

					if(g_cMcuVcApp.GetMcSrc(byLoop, &tMcSrc, byChnIdx, MODE_AUDIO))
					{
						if (!tMcSrc.IsNull() && tMcSrc.GetConfIdx() == m_byConfIdx)
						{
							tPreSetInReq.m_tReleaseMtInfo.m_tMt = tMcSrc;
							tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum++;
							byReleaseMode += MODE_AUDIO;
						}
					}
					if (byReleaseMode != MODE_NONE)
					{
						tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = byReleaseMode;
						break;
					}	
				}
				if (byReleaseMode != MODE_NONE)
				{
					break;
				}	
			}*/

			// [pengjie 2010/9/13] 填目的端能力
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()) );
			//zjl20101116 如果当前终端已回传则能力集要与已回传目的能力集取小
			if(!GetMinSpyDstCapSet(tSrcMt, tSimCapSet))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[VCSConfSelMT] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
					tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
				return FALSE;
			}
			
			tPreSetInReq.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		    // End
		
			if( m_ptMtTable->GetMtStatus( tSwitchInfo.GetDstMt().GetMtId(), &tDstMtStatus ) )
			{
				TMt tSrc;
				TMt tMcSrc;
				tSrc = tDstMtStatus.GetSelectMt( tSwitchInfo.GetMode() );
				if( !tSrc.IsNull() && VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState() )
				{
					tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = tSwitchInfo.GetMode();
					tPreSetInReq.m_tReleaseMtInfo.m_tMt = tSrc;
					if( MODE_BOTH == tSwitchInfo.GetMode() || MODE_VIDEO == tSwitchInfo.GetMode() )
					{
						++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
					}
					if( MODE_BOTH == tSwitchInfo.GetMode() || MODE_AUDIO == tSwitchInfo.GetMode() )
					{
						++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
					}

					u8 byMtInMcNum = GetCanRlsNumInVcsMc(tSrc, MODE_VIDEO);
					tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum += byMtInMcNum;
					
					if(MODE_AUDIO == tSwitchInfo.GetMode())
					{
						tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_BOTH;
						
						byMtInMcNum = GetCanRlsNumInVcsMc(tSrc, MODE_AUDIO);
						tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum += byMtInMcNum;
						
					}
// 					tMcSrc.SetNull();
// 					g_cMcuVcApp.GetMcSrc( m_cVCSConfStatus.GetCurSrcSsnId(), &tMcSrc, VCS_VCMT_MCCHLIDX, MODE_VIDEO );
// 					if( tMcSrc == tSrc && (tSwitchInfo.GetMode() == MODE_BOTH || tSwitchInfo.GetMode() == MODE_VIDEO) )
// 					{
// 						++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
// 						if( MODE_AUDIO == tSwitchInfo.GetMode() )
// 						{
// 							tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_BOTH;
// 						}
// 					}
					//zhouyiliang 20101015 点名双画面的情况，被选看终端还进了画面合成
					if ( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE ) 
					{
						++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
					}					
				}
				else
				{
					if( VCS_POLL_STOP != m_cVCSConfStatus.GetChairPollState() && 
						!m_cVCSConfStatus.GetCurVCMT().IsNull() && 
						m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPCHAIRMAN_MODE )
					{						
						tMcSrc.SetNull();
						g_cMcuVcApp.GetMcSrc( m_cVCSConfStatus.GetCurSrcSsnId(), &tMcSrc, VCS_VCMT_MCCHLIDX, MODE_VIDEO );
						if( tMcSrc == tSrc &&
							( MODE_BOTH == tSwitchInfo.GetMode() || MODE_VIDEO == tSwitchInfo.GetMode() )
							)
						{
							tPreSetInReq.m_tReleaseMtInfo.m_tMt = tSrc;
							tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = tSwitchInfo.GetMode();
							++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;							
						}
						tMcSrc.SetNull();
						g_cMcuVcApp.GetMcSrc( m_cVCSConfStatus.GetCurSrcSsnId(), &tMcSrc, VCS_VCMT_MCCHLIDX, MODE_AUDIO );						
						if( tMcSrc == tSrc &&
							( MODE_BOTH == tSwitchInfo.GetMode() || MODE_AUDIO == tSwitchInfo.GetMode() )
							)
						{
							tPreSetInReq.m_tReleaseMtInfo.m_tMt = tSrc;
							tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = tSwitchInfo.GetMode();
							++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
						}
					}
				}
			}
			
			if (m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_AUTO_MODE
				&& !IsMtNotInOtherHduChnnl(tPreSetInReq.m_tReleaseMtInfo.m_tMt,0,0)
				&& tPreSetInReq.m_tReleaseMtInfo.m_tMt == m_cVCSConfStatus.GetCurVCMT())
			{
				tPreSetInReq.m_tReleaseMtInfo.m_tMt.SetNull();
				tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 0;
				tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 0;
				//byReleaseMode = MODE_NONE;
			}

			OnMMcuPreSetIn( tPreSetInReq );
			return TRUE;
		}
	}else
	{
		// [11/23/2010 liuxu] 通知下级开始建交换
		if (!tSrcMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()))
		{
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));
			SendMMcuSpyNotify( tSrcMt, VCS_MCU_CONFSELMT_CMD, tSimCapSet);
		}
	}

	if ( VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState() &&
		tSwitchInfo.GetDstMt().GetMtId() == m_tConf.GetChairman().GetMtId() &&
		( tSwitchInfo.GetMode() == MODE_BOTH || tSwitchInfo.GetMode() == MODE_VIDEO )
		)
	{
		VCSConfStopMTSel( m_tConf.GetChairman(), tSwitchInfo.GetMode(),FALSE );
	}
	
	BOOL32 bBasEnough   = TRUE;
	BOOL32 bNeedVidAdpt = FALSE;
	BOOL32 bNeedAudAdpt = FALSE;
	
	// [9/26/2011 liuxu] 实际建交换时用tRealSwitchInfo代替tSwitchInfo
	// 以便修改bySwitchMode, 这样就不会因为tSwitchInfo中的模式时Both
	// 而出现交换建立错误
	TSwitchInfo tRealSwitchInfo = tSwitchInfo;

	if (bySwitchMode == MODE_VIDEO || bySwitchMode == MODE_BOTH)
	{
		tRealSwitchInfo.SetMode(MODE_VIDEO);
		bNeedVidAdpt = IsNeedSelAdpt(tOrigSrcMt, tDstMt, MODE_VIDEO);
		if(bNeedVidAdpt)
		{
		   if(!StartSelAdapt(tOrigSrcMt, tDstMt, MODE_VIDEO))
		   {
			  bBasEnough = FALSE;
			  ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[VCSConfSelMT] mt(mcuid:%d, mtid:%d) sel(mode:%d) mt(mcuid:%d, mtid:%d) using adp Failed!\n",
			   tDstMt.GetMcuId(), tDstMt.GetMtId(), bySwitchMode, tOrigSrcMt.GetMcuId(), tOrigSrcMt.GetMtId());
			  StopSwitchToSubMt(1,&tDstMt,MODE_VIDEO);
		   }
		}
		else
		{
			VCSSwitchSrcToDst( tRealSwitchInfo );
		}
	}

	if (bySwitchMode == MODE_AUDIO || bySwitchMode == MODE_BOTH)
	{
		tRealSwitchInfo.SetMode(MODE_AUDIO);
		bNeedAudAdpt = IsNeedSelAdpt(tOrigSrcMt, tDstMt, MODE_AUDIO);
		if(bNeedAudAdpt)
		{
			if(!StartSelAdapt(tOrigSrcMt, tDstMt, MODE_AUDIO))
			{
				bBasEnough = FALSE;
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[VCSConfSelMT] mt(mcuid:%d, mtid:%d) sel(mode:%d) mt(mcuid:%d, mtid:%d) using Audadp Failed!\n",
					tDstMt.GetMcuId(), tDstMt.GetMtId(), bySwitchMode, tOrigSrcMt.GetMcuId(), tOrigSrcMt.GetMtId());
				StopSwitchToSubMt(1,&tDstMt,MODE_AUDIO);
			}
		}
		else
		{
			VCSSwitchSrcToDst( tRealSwitchInfo );
		}
	}

	if(!bBasEnough)
	{
		if(tDstMt == m_tConf.GetChairman())
		{
			if (MODE_VIDEO == bySwitchMode)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCNOACCEPTVIDEO);
			}

			if (MODE_AUDIO == bySwitchMode)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_LOCNOACCEPTAUDIO);
			}
		}
		else
		{
			if (MODE_VIDEO == bySwitchMode)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_REMNOACCEPTVIDEO);
			}
			
			if (MODE_AUDIO == bySwitchMode)
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_REMNOACCEPTAUDIO);
			}
		}
		if (cServMsg.GetErrorCode() != 0)
		{
			SendMsgToAllMcs(MCU_VCS_VCMT_NOTIF, cServMsg);
		}
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[VCSConfSelMT]mt(mcuid:%d, mtid:%d) sel mt(mcuid:%d, mtid:%d) need adp, but fail to start adp\n",
			    tDstMt.GetMcuId(), tDstMt.GetMtId(), tOrigSrcMt.GetMcuId(), tOrigSrcMt.GetMtId());
		return FALSE;
	}

    //级联选看，将被选看者setin
    if ( !tOrigSrcMt.IsLocal() && !IsLocalAndSMcuSupMultSpy(tOrigSrcMt.GetMcuId()) )
    {   
        OnMMcuSetIn( tOrigSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
    }

    // 级联选看，将选看者setout
    if( tOrigDstMt.GetMtType() == MT_TYPE_SMCU || tOrigDstMt.GetMtType() == MT_TYPE_MMCU )
    {
		u16 wMcuIdx = GetMcuIdxFromMcuId( tDstMt.GetMtId() );
        TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);//tDstMt.GetMtId());
        if(ptConfMcInfo != NULL)
        {
            ptConfMcInfo->m_tSpyMt = tSrcMt;
            //send output 消息
            TSetOutParam tOutParam;
			TMsgHeadMsg tHeadMsg;
			memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
            tOutParam.m_nMtCount = 1;
            tOutParam.m_atConfViewOutInfo[0].m_tMt = tDstMt;
            tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
            tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;
            CServMsg cServMsg2;
			//tHeadMsg.m_tMsgSrc.m_tMt = tDstMt;
			cServMsg2.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
			cServMsg2.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
            cServMsg2.SetEventId(MCU_MCU_SETOUT_NOTIF);
            SendMsgToMt(tDstMt.GetMtId(), MCU_MCU_SETOUT_NOTIF, cServMsg2);
        }
    }

	TMt tOldSelSrc;// 获得旧视频选看源，用于调分辨率
    // 更改选看终端(即VCS绑定终端)的状态
    if( MODE_AUDIO == tSwitchInfo.GetMode() || 
        MODE_VIDEO == tSwitchInfo.GetMode() ||
        MODE_BOTH  == tSwitchInfo.GetMode() )
    {
        m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus ); // 选看
		if (MODE_VIDEO == tSwitchInfo.GetMode() || MODE_BOTH  == tSwitchInfo.GetMode())
		{
			tOldSelSrc = tDstMtStatus.GetSelectMt(MODE_VIDEO);
		}
        tDstMtStatus.SetSelectMt( tOrigSrcMt, tSwitchInfo.GetMode() );
        m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
    }

	// 调整新旧选看源终端分辨率
	TVMPParam_25Mem  tVMPParam   = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	if (tVMPParam.GetVMPMode() != CONF_VMPMODE_NONE &&
		(MODE_VIDEO == tSwitchInfo.GetMode() || MODE_BOTH  == tSwitchInfo.GetMode()))
	{
		// 有旧选看源
		if (ChgMtVidFormatRequired(tOldSelSrc))
		{
			ChangeMtVideoFormat(tOldSelSrc, FALSE);
		}
		if (ChgMtVidFormatRequired(tOrigSrcMt))
		{
			ChangeMtVideoFormat(tOrigSrcMt, FALSE);
		}
	}

    //判断选看者是否调整码率
	u16 wBitrate = 0;
	TLogicalChannel tLogicalChannel;
    if (!bNeedVidAdpt && IsNeedAdjustMtSndBitrate(GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId(), wBitrate) &&
		m_ptMtTable->GetMtLogicChnnl(GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE) &&
		wBitrate < tLogicalChannel.GetFlowControl())
    {
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[VCSConfSelMT] mt.%d, mode %d, old flowcontrol :%d, min flowcontrol :%d\n",
                GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId(), 
                tLogicalChannel.GetMediaType(), tLogicalChannel.GetFlowControl(), wBitrate);                   
        
        tLogicalChannel.SetFlowControl(wBitrate);
        cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
		SendMsgToMt(GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
    }

    return TRUE;
}

/*====================================================================
    函数名      ：VCSConfStopMTSel
    功能        ：VCS会议对tDstMt终端停止选看
    算法实现    ：
    引用全局变量：
    输入参数说明：tDstMt:选看终端
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
void CMcuVcInst::VCSConfStopMTSel( TMt tDstMt, u8 byMode,BOOL32 bIsRestoreSeeBrd /*= TRUE*/ )
{	
    //change mode if mixing
    if ( MODE_BOTH == byMode )
    {
        if( m_tConf.m_tStatus.IsMixing() )
        {
            byMode = MODE_VIDEO;
        }
    }
	
    ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfStopMTSel] Mcu%dMt%d Cancel Select See.bIsRestoreSeeBrd.%d\n", tDstMt.GetMcuId(), tDstMt.GetMtId(),bIsRestoreSeeBrd );
    
    // 选看的目标可能是下级终端
    tDstMt = GetLocalMtFromOtherMcuMt(tDstMt);

	if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfStopMTSel] Dst Mt%u-%u not joined conference!\n", 
			             tDstMt.GetMcuId(), tDstMt.GetMtId() );
		return;
	}

	// 清除选看状态
	TMtStatus tMtStatus; 
	m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );

	TMt tVidSrc = tMtStatus.GetSelectMt( MODE_VIDEO );
	TMt tAudSrc = tMtStatus.GetSelectMt( MODE_AUDIO );

	if( tVidSrc.IsNull() && MODE_VIDEO == byMode )
	{
		return;
	}
	if( tAudSrc.IsNull() && MODE_AUDIO == byMode )
	{
		return;
	}
	if( tVidSrc.IsNull() && tAudSrc.IsNull() && MODE_BOTH == byMode )
	{
		return;
	}

	if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
	{
		tMtStatus.RemoveSelByMcsMode( MODE_VIDEO );

		// 重新调整选看者的发送码率, 目前只支持调整本级选看者的码率		
		if (!tVidSrc.IsNull() && tVidSrc.IsLocal())
		{
			TMtStatus tSrcStatus;
			m_ptMtTable->GetMtStatus(tVidSrc.GetMtId(), &tSrcStatus);
			if ( m_tConfAllMtInfo.MtJoinedConf( tVidSrc.GetMtId() ) 
				&& tSrcStatus.IsSendVideo() 
			   )
			{
				NotifyMtSend( tVidSrc.GetMtId(), MODE_VIDEO );
			}
			
		}
		if( !tVidSrc.IsLocal() && IsLocalAndSMcuSupMultSpy(tVidSrc.GetMcuId()))
		{
			FreeRecvSpy( tVidSrc,MODE_VIDEO );
		}
		
	}

	if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
	{
		tMtStatus.RemoveSelByMcsMode( MODE_AUDIO );

		// 重新调整选看者的发送码率, 目前只支持调整本级选看者的码率
		
		if (!tAudSrc.IsNull() && tAudSrc.IsLocal())
		{
			TMtStatus tSrcStatus;
			m_ptMtTable->GetMtStatus(tAudSrc.GetMtId(), &tSrcStatus);
			if ( m_tConfAllMtInfo.MtJoinedConf( tAudSrc.GetMtId() ) 
				&& tSrcStatus.IsSendAudio() 
			   )
			{
				NotifyMtSend( tAudSrc.GetMtId(), MODE_AUDIO );
			}
		}
		if( !tAudSrc.IsLocal() && IsLocalAndSMcuSupMultSpy(tAudSrc.GetMcuId()))
		{
			FreeRecvSpy( tAudSrc,MODE_AUDIO );
		}
	}
	BOOL32 byIsCanRestoreBrdSrc = FALSE;
	if( bIsRestoreSeeBrd )
	{
		byIsCanRestoreBrdSrc = TRUE;
		//zjj20091026 vcs会议主席轮询时,不能恢复广播原到主席的交换
		if( m_tConf.GetConfSource() == VCS_CONF &&		 
			m_tConf.GetChairman() == tDstMt && 
			ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() )
			)
		{		
			if( m_cVCSConfStatus.GetChairPollState() == VCS_POLL_START ||
				m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE
				)
			{		
				if( MODE_BOTH == byMode )
				{
					byMode = MODE_AUDIO;
				}
				if( MODE_VIDEO == byMode)
				{
					byIsCanRestoreBrdSrc = FALSE;
				}
			}
			
		}
	}
	// 跨格式选看释放对应的适配资源
	ReleaseResbySel(tDstMt, byMode,byIsCanRestoreBrdSrc);
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[VCSConfStopMTSel] Mcu%dMt%d Cancel Select See.byIsCanRestoreBrdSrc.%d\n", tDstMt.GetMcuId(), tDstMt.GetMtId(),byIsCanRestoreBrdSrc );
	// 清除选看的终端相关信息
    TMt tNullMt;
	tNullMt.SetNull();
    if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
    {
		tMtStatus.SetSelectMt(tNullMt, MODE_VIDEO);
    }
	if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
	{
		tMtStatus.SetSelectMt(tNullMt, MODE_AUDIO);
	}
	m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tMtStatus );
	// 调整该终端分辨率
	if ((MODE_VIDEO == byMode || MODE_BOTH == byMode)
		&& ChgMtVidFormatRequired(tVidSrc))
	{
		TVMPParam_25Mem	tVMPParam  = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp); 
		ChangeMtVideoFormat(tVidSrc, FALSE); 
	}
 
	if (bIsRestoreSeeBrd)
	{
		if( byIsCanRestoreBrdSrc  )
		{
			// 更改选看者音视频源
			RestoreRcvMediaBrdSrc( tDstMt.GetMtId(), byMode );
		}	
		else 
		{
			if( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE &&
				m_cVCSConfStatus.GetChairPollState() != VCS_POLL_START )
			{
				//zjl 20110510 StopSwitchToSubMt 接口重载替换
				//StopSwitchToSubMt( tDstMt.GetMtId(),byMode );
				StopSwitchToSubMt(1, &tDstMt, byMode);
			}
		}
	}
	

	// 立刻通知该选看终端的状态变化
    MtStatusChange( &tDstMt, TRUE );

    return;
}

/*====================================================================
    函数名      ：VCSConfStopMTSeled
    功能        ：VCS会议tSrcMt终端被选看
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrcMt:被选看终端
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
void CMcuVcInst::VCSConfStopMTSeled(TMt tSrcMt, u8 byMode)
{
	ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfStopMTSeled]Stop all mt sel spec mt(mcuid:%d, mtid:%d)\n", 
		    tSrcMt.GetMcuId(), tSrcMt.GetMtId());

	// 遍历所有选看该终端的实体，拆交换并清空其选看状态，通知看自己
	TMt       tMt;
	TMtStatus tMtStatus;
    TMt       tNullMt;
	tNullMt.SetNull();

	tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);

	//批量拆视频选看
	TMt atStopVideoDstMt[MAXNUM_CONF_MT];
	u8 byStopVideoMtNum = 0;
	//批量拆音频选看
	TMt atStopAudioDstMt[MAXNUM_CONF_MT];
	u8 byStopAudioMtNum = 0;

	u8         bySel = FALSE;
	for ( u8 byMtIdxLp = 1; byMtIdxLp <= MAXNUM_CONF_MT; byMtIdxLp ++ )
	{
		if( byMtIdxLp != tSrcMt.GetMtId() ) // 排除本身
		{
			tMt = m_ptMtTable->GetMt( byMtIdxLp );
			if( !tMt.IsNull() ) // 该终端是否有效
			{
				bySel = FALSE;
				m_ptMtTable->GetMtStatus(byMtIdxLp, &tMtStatus);
				TMt tSelMt;
				if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
				{
					tSelMt = tMtStatus.GetSelectMt(MODE_VIDEO);
					if( (!tSelMt.IsLocal() && tSelMt.GetMcuId() == tSrcMt.GetMtId()) ||
						tSelMt == tSrcMt ) 
					{
						bySel = TRUE;
						//zjl 20110510 StopSwitchToSubMt 接口重载替换
						//StopSwitchToSubMt( tMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE);
						atStopVideoDstMt[byStopVideoMtNum] = tMt;
						byStopVideoMtNum ++;

						tMtStatus.SetSelectMt(tNullMt, MODE_VIDEO);
						m_ptMtTable->SetMtStatus(byMtIdxLp, &tMtStatus);

						if (IsNeedAdapt(tSelMt, m_ptMtTable->GetMt(byMtIdxLp), MODE_VIDEO)/*IsNeedSelApt(tSelMt.GetMtId(), byMtIdxLp, MODE_VIDEO)*/)
						{
							StopSelAdapt(tSelMt, m_ptMtTable->GetMt(byMtIdxLp), MODE_VIDEO);
							//StopHdVidSelAdp(tSrcMt, m_ptMtTable->GetMt(byMtIdxLp), MODE_VIDEO);
						}
					}
				}
				if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
				{
					tSelMt = tMtStatus.GetSelectMt(MODE_AUDIO);
					if( (!tSelMt.IsLocal() && tSelMt.GetMcuId() == tSrcMt.GetMtId()) ||
						tSelMt == tSrcMt ) 
					{
						bySel = TRUE;
						
						//zjl 20110510 StopSwitchToSubMt 接口重载替换
						//StopSwitchToSubMt( tMt.GetMtId(), MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);
						atStopAudioDstMt[byStopAudioMtNum] = tMt;
						byStopAudioMtNum ++;

						tMtStatus.SetSelectMt(tNullMt, MODE_VIDEO);
						m_ptMtTable->SetMtStatus(byMtIdxLp, &tMtStatus);
						if (IsNeedAdapt(tSrcMt, m_ptMtTable->GetMt(byMtIdxLp), MODE_AUDIO)/*IsNeedSelApt(tSelMt.GetMtId(), byMtIdxLp, MODE_AUDIO)*/)
						{
							StopSelAdapt(tSrcMt, m_ptMtTable->GetMt(byMtIdxLp), MODE_AUDIO);
							//StopAudSelAdp(tSrcMt, m_ptMtTable->GetMt(byMtIdxLp));
						}
					}
				}

				if (bySel)
				{
					TMt tTempMt = m_ptMtTable->GetMt(byMtIdxLp);
				    MtStatusChange( &tTempMt, TRUE );
				}


			}
		}
	}	

	if (byStopVideoMtNum > 0)
	{
		StopSwitchToSubMt(byStopVideoMtNum, atStopVideoDstMt, MODE_VIDEO, FALSE);
	}

	if (byStopAudioMtNum > 0)
	{
		StopSwitchToSubMt(byStopAudioMtNum, atStopAudioDstMt, MODE_AUDIO, FALSE);
	}
}

/*====================================================================
    函数名      ：VCSSwitchSrcToDst
    功能        ：完成VCS会议的码流交换
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
void CMcuVcInst::VCSSwitchSrcToDst( const TSwitchInfo &tSwitchInfo )
{
	TMt	tDstMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetDstMt());
	TMt tSrcMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetSrcMt());
	u8	byMode = tSwitchInfo.GetMode();

    // 通知码流发送端发送码流      
    NotifyMtSend( tSrcMt.GetMtId(), byMode );

	TMt tUnlocalSrcMt = tSwitchInfo.GetSrcMt();
	TMt tUnlocalDstMt = tSwitchInfo.GetDstMt();
	CRecvSpy tSpyResource;
	u16 wSpyPort = SPY_CHANNL_NULL;	
	if( m_cSMcuSpyMana.GetRecvSpy( tUnlocalSrcMt, tSpyResource ) )
	{
		wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
	}
	//zjj 20091025选看不走桥
    //g_cMpManager.SetSwitchBridge(tSrcMt, 0, byMode);

	if (TYPE_MT == tDstMt.GetType())
	{
		//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
		//StartSwitchToSubMt(tUnlocalSrcMt, 0, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE, FALSE,FALSE,wSpyPort);
		TSwitchGrp tSwitchGrp;
		tSwitchGrp.SetSrcChnl(0);
		tSwitchGrp.SetDstMtNum(1);
		tSwitchGrp.SetDstMt(&tDstMt);
		StartSwitchToAll(tUnlocalSrcMt, 1, &tSwitchGrp, byMode, SWITCH_MODE_SELECT, TRUE, FALSE, wSpyPort);

		TMtStatus tMtStatus;
		if( m_ptMtTable->GetMtStatus( tDstMt.GetMtId(),&tMtStatus ) )
		{
			tMtStatus.SetSelectMt( tUnlocalSrcMt,byMode );
			m_ptMtTable->SetMtStatus( tDstMt.GetMtId(),&tMtStatus );
		}
		
	}
	else if (TYPE_MCUPERI == tDstMt.GetType())
	{
		StartSwitchToPeriEqp(tUnlocalSrcMt, 0, tDstMt.GetEqpId(), tSwitchInfo.GetDstChlIdx(), byMode, SWITCH_MODE_SELECT);
	}
	
	
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (tConfAttrb.IsResendLosePack() && !(tUnlocalSrcMt == tUnlocalDstMt))
	{
		//把目的的Rtcp交换给源
        u8 bySrcChnnl = 0;
        if (TYPE_MT == tDstMt.GetType())
        {
            bySrcChnnl = 0;
        }
        else if (TYPE_MCUPERI == tDstMt.GetType())
        {
            bySrcChnnl = tSwitchInfo.GetDstChlIdx();
        }
        BuildRtcpSwitchForSrcToDst(tUnlocalDstMt, tUnlocalSrcMt, byMode, bySrcChnnl, TRUE);
	}

}
/*====================================================================
    函数名      ：VCSConfMTToTW
    功能        ：VCS会议将终端选入电视墙
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：BOOL
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
BOOL CMcuVcInst::VCSConfMTToTW( TTWSwitchInfo tTWSwitchInfo )
{  
	// 对于高清终端作为电视墙，实际转换为选看业务
 	if (TYPE_MT == tTWSwitchInfo.GetDstMt().GetType())
	{
		TSwitchInfo tSwitchInfo = (TSwitchInfo)tTWSwitchInfo;
		return VCSConfSelMT(tSwitchInfo);
	}

    if (CurState() != STATE_ONGOING)
    {
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfMTToTW] InstID(%d) in wrong state(%d)\n", GetInsID(), CurState());
		return FALSE;
    }

	CServMsg cServMsg;
    cServMsg.SetConfIdx(m_byConfIdx);
    cServMsg.SetConfId(m_tConf.GetConfId()) ;
	u8 bySrcId = m_cVCSConfStatus.GetCurSrcSsnId();
	if (bySrcId != 0)
	{
		cServMsg.SetSrcSsnId(bySrcId);
	}

    TMt tSrcMT  = tTWSwitchInfo.GetSrcMt();
    tSrcMT = GetLocalMtFromOtherMcuMt(tSrcMT); 
	TMt tOrigMt = tTWSwitchInfo.GetSrcMt();

	u8 byDstId = tTWSwitchInfo.GetDstMt().GetEqpId();

	// 对于源终端为下级终端，通知下级MCU选入该终端
	if (!tOrigMt.IsLocal() && !IsLocalAndSMcuSupMultSpy(tOrigMt.GetMcuIdx()))
    {		
        OnMMcuSetIn(tOrigMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
    }

		
    switch(tTWSwitchInfo.GetMemberType()) 
    {
	case TW_MEMBERTYPE_VCSAUTOSPEC:
	case TW_MEMBERTYPE_VCSSPEC:
		{
			//源终端必须已经加入会议
			if (FALSE == m_tConfAllMtInfo.MtJoinedConf(tSrcMT.GetMtId()))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfMTToTW] select source MT%u-%u has not joined current conference! Error!\n",
						tSrcMT.GetMcuId(), tSrcMT.GetMtId());
// 				cServMsg.SetErrorCode(ERR_MCU_MT_NOTINCONF);
// 				SendMsgToAllMcs(MCU_VCS_VCMT_NOTIF, cServMsg);
				return FALSE;
			}

			TMtStatus tMtStatus;
			//源终端必须具备视频发送能力
			m_ptMtTable->GetMtStatus(tSrcMT.GetMtId(), &tMtStatus);
			if (FALSE == tMtStatus.IsSendVideo())
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfMTToTW] select source MT%u-%u has not ability to send video!\n",
						tSrcMT.GetMcuId(), tSrcMT.GetMtId());
// 				cServMsg.SetErrorCode(ERR_MCU_SRCISRECVONLY);
// 				SendMsgToAllMcs(MCU_VCS_VCMT_NOTIF, cServMsg);
				return FALSE;
			}	
		}
        break;
    default:
        ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfMTToTW] Wrong TvWall Member Type!\n");
		return FALSE;
	}

	// VCS仅终端进入电视墙
	ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfMTToTW] tOrigMt(%d,%d)!\n",tOrigMt.GetMcuId(),tOrigMt.GetMtId() );
	tOrigMt.SetMtType(MT_TYPE_MT);

	// [8/19/2011 liuxu] 防止终端进入电视墙失败,却进入了混音
	BOOL32 bStartTvwOk = TRUE;
	if (byDstId >= TVWALLID_MIN && byDstId <= TVWALLID_MAX )
	{
	    ChangeTvWallSwitch(&tOrigMt,
                            tTWSwitchInfo.GetDstMt().GetEqpId(),
                            tTWSwitchInfo.GetDstChlIdx(),
                            tTWSwitchInfo.GetMemberType(), 
							TW_STATE_START);
	}
	else if (byDstId >= HDUID_MIN && byDstId <= HDUID_MAX)
	{
		// [2013/03/11 chenbing] VCS会议不支持HDU多画面,子通道置0
		bStartTvwOk = ChangeHduSwitch(&tOrigMt, byDstId, tTWSwitchInfo.GetDstChlIdx(), 0,
                         tTWSwitchInfo.GetMemberType(), TW_STATE_START);
	}
	else
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[VCSConfMTToTW] wrong dest id(%d)\n", byDstId);
		return FALSE;
	}

	return bStartTvwOk;
}
/*====================================================================
    函数名      ：VCSConfStopMTToTW
    功能        ：VCS会议将终端踢出电视墙
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
void CMcuVcInst::VCSConfStopMTToTW(TMt tMt)
{
	TMt tOrigMt = tMt;
	
	TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
	if(tConfAttrib.IsHasTvWallModule() )
    {
		if (!tMt.IsLocal())
		{
			tMt = m_ptMtTable->GetMt(GetFstMcuIdFromMcuIdx(tMt.GetMcuId()));
		}

        TMultiTvWallModule tMultiTvWallModule;
        m_tConfEqpModule.GetMultiTvWallModule( tMultiTvWallModule );
        TTvWallModule tTvWallModule;
        for( u8 byTvLp = 0; byTvLp < tMultiTvWallModule.GetTvModuleNum(); byTvLp++ )
        {
            tMultiTvWallModule.GetTvModuleByIdx(byTvLp, tTvWallModule);
			u8 byEqpId = tTvWallModule.m_tTvWall.GetEqpId();
            if( g_cMcuVcApp.IsPeriEqpConnected(byEqpId))
			{
				TPeriEqpStatus tTvwallStatus;
				g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
				if (tTvwallStatus.m_byOnline == 1)
				{
					u8 byTmpMtId = 0;
					u8 byMtConfIdx = 0;
					u8 byMemberNum = tTvwallStatus.m_tStatus.tTvWall.byChnnlNum;
					for(u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
					{
						byTmpMtId = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMtId();
						byMtConfIdx = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
						if (tMt.GetMtId() == byTmpMtId && m_byConfIdx == byMtConfIdx)
						{
							StopSwitchToPeriEqp(byEqpId, byLoop);
							tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
							tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
							tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
            
							//清终端状态
							m_ptMtTable->SetMtInTvWall(byTmpMtId, FALSE);
						}               
					}
					g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tTvwallStatus);
				}
			}
		}
	}
}

/*====================================================================
    函数名      ：GoOnSelStep
    功能        ：根据会议状态，继续进行选看操作步骤
    算法实现    ：
    引用全局变量：
    输入参数说明：tMt：选看相关终端
	              byMediaType：选看媒体类型
				  bySelDirect: 选看主席(TRUE)/被主席选看(FALSE)
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
void CMcuVcInst::GoOnSelStep(TMt tMt, u8 byMediaType, u8 bySelDirect)
{
	if (bySelDirect)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] mt(mcuid:%d, mtid:%d) go on sel chairman\n",
			   tMt.GetMcuId(), tMt.GetMtId());
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] chairman go on sel mt(mcuid:%d, mtid:%d)\n",
			   tMt.GetMcuId(), tMt.GetMtId());
	}

	// 判断本地主席终端是否在线，不在线则返回
	TMt tChairMan;
	tChairMan = m_tConf.GetChairman();
	if (tChairMan.IsNull() || 
		!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()))
	{
		return;
	}

	// 下级会议不进行选看操作
	if (CONF_CREATE_MT == m_byCreateBy)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] No sel step for subconf\n");
		return;
	}

	// 组呼模式下不进行主席的选看被选看操作
	if (ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) || 
		m_cVCSConfStatus.GetCurVCMode() == VCS_MULVMP_MODE )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] No sel step in this mode(%d)\n", m_cVCSConfStatus.GetCurVCMode());
		return;
	}

	TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();
	if( bySelDirect )
	{
		if( !m_cVCSConfStatus.GetReqVCMT().IsNull()  )
		{
			if( !(tMt==m_cVCSConfStatus.GetReqVCMT()) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] tmt(%d) is not GetReqVCMT(%d)\n",tMt.GetMtId(),m_cVCSConfStatus.GetReqVCMT().GetMtId() );
				return;
			}			
		}
		else 
		{
			if( !tCurVCMT.IsNull() )
			{		
				if( !(tMt==tCurVCMT) )
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] tmt(%d) is not GetCurVCMT(%d)\n",tMt.GetMtId(),tCurVCMT.GetMtId() );
					return;	
				}							
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] tmt(%d) is not GetReqVCMT(%d) and GetCurVCMT(%d)\n",tMt.GetMtId(),m_cVCSConfStatus.GetReqVCMT().GetMtId(),tCurVCMT.GetMtId() );
				return;	
			}
		}
	}
	
	
	// 当前调度终端或当前调度终端所在的mcu才可进行主席选看或被主席选看,排除以下
	// 由于时序问题，对于选看主席, 放宽要求, 只需非固定资源即可
	if ((!tCurVCMT.IsNull() && 
		(tCurVCMT == tMt || tMt.GetMtId() == tCurVCMT.GetMcuId())) ||
		(TRUE == bySelDirect /*&&	 CONF_CALLMODE_NONE == VCSGetCallMode(tMt)*/))
	{
		// 构造交换通道
		TSwitchInfo tSwitchInfo;
		if (bySelDirect)
		{
			tSwitchInfo.SetDstMt(tMt);
			tSwitchInfo.SetSrcMt(tChairMan);
		}
		else
		{
			tSwitchInfo.SetDstMt(tChairMan);
			tSwitchInfo.SetSrcMt(tMt);
		}

		switch(m_cVCSConfStatus.GetCurVCMode())
		{
		case VCS_SINGLE_MODE:
			if (MODE_VIDEO == byMediaType)
			{
				tSwitchInfo.SetMode(MODE_VIDEO);
			}
			else if (MODE_AUDIO == byMediaType)
			{
				tSwitchInfo.SetMode(MODE_AUDIO);
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[GoOnSelStep] uncorrect sel mediatype(%d) in single mode\n", byMediaType);
				return;
			}
			break;
		case VCS_MULTW_MODE:
			{
				// 电视墙模式下为混音模式，所以仅需对调度资源的视频进行选看
				if (MODE_VIDEO == byMediaType)
				{
					tSwitchInfo.SetMode(MODE_VIDEO);
					// 对于当前调度资源进行选看时,判断选看时所需资源是否充足,
					// 不足则释放之前占用的所有选看资源,尽量保证当前调度选看成功
					u8 bySrcId = (tSwitchInfo.GetSrcMt().IsLocal() ? tSwitchInfo.GetSrcMt().GetMtId() : GetFstMcuIdFromMcuIdx(tSwitchInfo.GetSrcMt().GetMcuId()));
					u8 byDstId = (tSwitchInfo.GetDstMt().IsLocal() ? tSwitchInfo.GetDstMt().GetMtId() : GetFstMcuIdFromMcuIdx(tSwitchInfo.GetDstMt().GetMcuId()));
					TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(bySrcId);
					TSimCapSet tDstCap = m_ptMtTable->GetDstSCS(byDstId);
					CBasChn *pcBasChn=NULL;
					TBasChnCapData tBasChnCapData;
					if (IsNeedSelApt(bySrcId, byDstId, MODE_VIDEO) && 					
						!g_cMcuVcApp.GetIdleBasChn(tSrcCap, tDstCap, MODE_VIDEO,&pcBasChn,&tBasChnCapData))
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[GoOnSelStep]not enough bas for all mt in twmode, so stop mt sel chairman\n");
						for (u8 byMtId = 0; byMtId < MAXNUM_CONF_MT; byMtId++)
						{
							if (byMtId != bySrcId && byMtId != byDstId &&
								m_tConfAllMtInfo.MtJoinedConf(byMtId))
							{
								VCSConfStopMTSel(m_ptMtTable->GetMt(byMtId), MODE_VIDEO);
							}

						}

					}
				}
				else
				{ 
					return;
				}
			}
		    break;
		case VCS_MULVMP_MODE:
			// 该模式下均看画面合成图像，不进行选看
			return;

		default:
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[GoOnSelStep] wrong vc mode(%d)\n", m_cVCSConfStatus.GetCurVCMode());
			return;

		}
		//[nizhijun 2011/01/13] 选看失败，如果是主席选看的话，让主席看自己
		if (!VCSConfSelMT(tSwitchInfo) && !bySelDirect)
		{
			NotifyMtReceive(m_tConf.GetChairman(), m_tConf.GetChairman().GetMtId());
		}
	}
}

/*====================================================================
    函数名      ：FindSmcuMtInTvWallAndStop
    功能        ：调度下级终端时要把同级的其它终端剔出电视墙
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt tNewCurVCMT 新调度的终端 
				  TMt *	ptDropMt 剔除出电视墙的终端
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/09/14                周晶晶        创建
====================================================================*/
void CMcuVcInst::FindSmcuMtInTvWallAndStop(const TMt& tNewCurVCMT, TMt *const ptDropMt, BOOL32 bIsStopSwitch /* = TRUE*/ )
{
	// 新入终端为空,不做处理
	if( tNewCurVCMT.IsNull() )
	{
		return;
	}

	// [5/30/2011 liuxu] 只有单回传时才会做如下处理
	if(IsLocalAndSMcuSupMultSpy(tNewCurVCMT.GetMcuId()))
	{
		return;
	}

	if (ptDropMt != NULL)
	{
		ptDropMt->SetNull();
	}

	// [5/30/2011 liuxu] 对HDU和电视墙进行统一整理
	// 一个会议支持的电视墙通道的总数
	const u8 byMaxTvwChnnlNum = MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL + MAXNUM_PERIEQP_CHNNL * MAXNUM_MAP;
	CConfTvwChnnl acTvwChnnlFind[byMaxTvwChnnlNum];					// 待寻找的电视墙通道
	u8		abyTvwIdFind[byMaxTvwChnnlNum];							// 存储找到的电视墙设备的ID
	memset(abyTvwIdFind, 0, sizeof(abyTvwIdFind));
	
	const u8 byFindNum = FindAllTvwChnnlByMcu(tNewCurVCMT, acTvwChnnlFind, byMaxTvwChnnlNum);

	u8 byTvwEqpId, byTvwChnnlIdx;								// 找到的电视墙设备号和通道号
	TTvwMember tTvwMember;										// 电视墙通道成员

	// 遍历所有找到的电视墙通道进行处理
	for (u8 byLoop = 0; byLoop < byFindNum; ++byLoop)
	{
		if (acTvwChnnlFind[byLoop].IsNull())
		{
			continue;
		}
		
		byTvwEqpId = acTvwChnnlFind[byLoop].GetEqpId();
		byTvwChnnlIdx = acTvwChnnlFind[byLoop].GetChnnlIdx();
		tTvwMember = acTvwChnnlFind[byLoop].GetMember();
		
		// 终端在线时的处理
		if (g_cMcuVcApp.IsPeriEqpConnected(byTvwEqpId))
		{
			// 单回传时允许一个下级终端进入多个电视墙通道
			if ((TMt)tTvwMember == tNewCurVCMT)
			{
				continue;	
			}

			// 通道成员必须是TW_MEMBERTYPE_VCSAUTOSPEC和TW_MEMBERTYPE_VCSSPEC
			if (!IsVcsTvwMemberType(acTvwChnnlFind[byLoop].GetMember().byMemberType))
			{
				continue;
			}

			// 通道成员非本会议的, 不处理
			if ( tTvwMember.GetConfIdx() && tTvwMember.GetConfIdx() != m_byConfIdx)
			{
				continue;
			}

			if( NULL != ptDropMt)
			{
				*ptDropMt = (TMt)tTvwMember;
			}
			
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  
				"[FindSmcuMtInTvWallAndStop] dorp mt(%d,%d) bIsStopSwitch(%d)\n",
				tTvwMember.GetMcuId(), tTvwMember.GetMtId(), bIsStopSwitch );
			
			if( bIsStopSwitch )
			{		
				TMt tOldMt = (TMt)tTvwMember;
		
				// Hdu设备处理
				if (IsValidHduChn(byTvwEqpId, byTvwChnnlIdx))
				{
					// [2013/03/11 chenbing] VCS会议不支持HDU多画面,子通道置0
					ChangeHduSwitch( NULL, byTvwEqpId, byTvwChnnlIdx, 0, tTvwMember.byMemberType, 
						TW_STATE_STOP, MODE_BOTH, FALSE, FALSE );

					//zjj20120414 vcs单通道轮询，单回传下级保该下级的调度终端或当前进墙终端，所以清空电视墙后马上去轮下一个终端，以免黑屏
					u8 byTwPollState = POLL_STATE_NONE;
					if( !tOldMt.IsLocal() && !IsLocalAndSMcuSupMultSpy(tOldMt.GetMcuId()) && 
						m_tTWMutiPollParam.GetPollState(byTvwEqpId, byTvwChnnlIdx, byTwPollState )  )
					{
						u32 dwTimerIdx = 0;
						if( byTwPollState == POLL_STATE_NORMAL &&
							m_tTWMutiPollParam.GetTimerIdx(byTvwEqpId, byTvwChnnlIdx, dwTimerIdx) )
						{
							SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx,100,dwTimerIdx );
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  
									"[FindSmcuMtInTvWallAndStop] byTvwEqpId.%d byTvwChnnlIdx.%d restart timer\n",
									byTvwEqpId, byTvwChnnlIdx );
						}
					}
				}
				// Tvwall设备处理
				else
				{
					ChangeTvWallSwitch(&tOldMt, byTvwEqpId, byTvwChnnlIdx, tTvwMember.byMemberType, TW_STATE_STOP);
				}

				
			
				//非当前调度终端退出电视墙，且已经起一键混音功能，就退出定制混音
				if( m_cVCSConfStatus.GetMtInTvWallCanMixing() 
					&& m_tConf.m_tStatus.IsMixing() 
					&& m_tConf.m_tStatus.IsSpecMixing() 
					&& VCSMtNotInTvWallCanStopMixing( tOldMt )		
					)
				{
					RemoveSpecMixMember( &tOldMt, 1, FALSE );
				}
			}

		}else				// 终端不在线
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "Get Conf Tvwall(%d, %d) failed\n", byTvwEqpId, byTvwChnnlIdx);
			
			//20101101_tzy VCS会议时，单回传时,同一个下级但原来电视墙终端与当前上传终端不一致则不用恢复
			if( GetFstMcuIdFromMcuIdx( tTvwMember.GetMcuId() ) == GetFstMcuIdFromMcuIdx( tNewCurVCMT.GetMcuId() )
				&& !((TMt)tTvwMember == tNewCurVCMT)
				&& (IsVcsTvwMemberType(tTvwMember.byMemberType) )
				&& tTvwMember.GetConfIdx() == m_byConfIdx ) 
			{
				// 清空该通道
				ClearTvwChnnlStatus(byTvwEqpId, byTvwChnnlIdx);
				
				// 依次将Hdu Id填入abyTvwIdFind中, 以便统一发送
				// 避免同一个设备下多个通道被清空时多次发送
				for ( u8 byCounter = 0; byCounter < byMaxTvwChnnlNum; ++byCounter)
				{
					// 为0说明为空
					if ( 0 == abyTvwIdFind[byCounter] )
					{
						abyTvwIdFind[byCounter] = byTvwEqpId;
						break;
					}
					
					// 已经插入就不需要再插入了
					if ( byTvwEqpId == abyTvwIdFind[byCounter] )
					{
						break;
					}
				}
			}
		}
	}

	// 不在线电视墙设备统一清空并通知界面
	u8 byNext = 0;
	TPeriEqpStatus tStatus;
	CServMsg cServMsg;
	while ( byNext < byMaxTvwChnnlNum && 0 != abyTvwIdFind[byNext] )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[FindSmcuMtInTvWallAndStop] notify tvw eqp(%d) status\n", abyTvwIdFind[byNext] );
		if( g_cMcuVcApp.GetPeriEqpStatus( abyTvwIdFind[byNext], &tStatus ) )
		{
			cServMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
			SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
		}
		
		++byNext;
	}
}

/*====================================================================
    函数名      ：ChgCurVCMT
    功能        ：根据会议状态，切换当前调度终端
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
void CMcuVcInst::ChgCurVCMT(TMt tNewCurVCMT)
{
	TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();
	if (tCurVCMT == tNewCurVCMT)
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ChgCurVCMT] NewCurVCMT hasn't change\n");	
		return;
	}
	
	u8  byVCMode = m_cVCSConfStatus.GetCurVCMode();
	
	//yanghuaizhi 20120822 点名双画面多回传上次画面合成未处理完成，直接return(VMP notify还没到/未更新到m_tLastVmpParam信息里)
	//只有执行了Setvmpchnl处理,才会建交换更新m_tLastVmpParam信息,画面合成处理才结束.
	// 针对场景:vmp notify刚回,发了级联调分辨率,但还未收到调分辨率Ack(m_tLastVmpParam未更新)时,
	// 点了下一终端,调vmp时,PresetinReq信息中无可释放终端(根据m_tLastVmpParam获得),若只有1路带宽会报带宽满
	// 为保证vmp处理的原子性,前一个处理完成后,再执行下一个处理.
	if (VCS_GROUPROLLCALL_MODE == byVCMode || VCS_GROUPVMP_MODE == byVCMode)
	{
		// 画面合成外设处理中
		if (!m_tConfInStatus.IsVmpNotify())
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ChgCurVCMT]VCS_GROUPROLLCALL_MODE/VCS_GROUPVMP_MODE last vmp req not notify yet! return\n");
			return;
		} /* notify刚回,但还有级联调分辨率处理,Ack后才会Setvmpchnl.Setvmpchnl后才表示真正ok
		else if (!tCurVCMT.IsNull() && !tCurVCMT.IsLocal() && m_tConfAllMtInfo.MtJoinedConf(tCurVCMT)
			&& IsLocalAndSMcuSupMultSpy(tCurVCMT.GetMcuIdx())
			&& !m_tLastVmpParam.IsMtInMember(tCurVCMT))
		{
			// 若下级是非科达MT,且前适配不足时,不会更新Lastvmpparam
			if(!IsKedaMt(tCurVCMT, FALSE) && m_tVmpChnnlInfo.GetHDChnnlNum() == m_tVmpChnnlInfo.GetMaxNumHdChnnl())
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ChgCurVCMT]tCurVCMT(%d,%d) is noneKedaMt, VmpHdChl[%d] is full.\n");
			}
			else
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ChgCurVCMT]VCS_GROUPROLLCALL_MODE/VCS_GROUPVMP_MODE last vmp req not setvmpchl yet! return\n");
				return;
			}
		}*/
	}

	//zhouyiliang 20101229重整代码 新调度终端是本地终端或者是单回传下级终端且不是自动多画面模式上次调度响应没到的情况，切换当前调度终端
	if ( ( tNewCurVCMT.IsLocal() || (!tNewCurVCMT.IsLocal() && !IsLocalAndSMcuSupMultSpy(tNewCurVCMT.GetMcuIdx()))) 
		&& !( VCS_GROUPVMP_MODE == byVCMode  && !m_tConfInStatus.IsVmpNotify() )
		)
	{
/*#ifdef _8KE_
		if( ( VCS_GROUPVMP_MODE == byVCMode || VCS_GROUPROLLCALL_MODE == byVCMode ) &&
			!tNewCurVCMT.IsNull() && !IsKedaMt(tNewCurVCMT) )
		{
			if( !IsNoneKedaMtCanInMpu(tNewCurVCMT) )
			{
				TMt tNullMt;
				tNullMt.SetNull();
				m_cVCSConfStatus.SetReqVCMT(tNullMt);
				//VCSConfStatusNotif();
				NotifyMcsAlarmInfo(0,ERR_VMP_8000E_NOTSPPORT_NONEKEDAMT);					
				return;
			}
		}
#endif*/
		m_cVCSConfStatus.SetCurVCMT(tNewCurVCMT);
//		ChangeConfRestoreMixParam();
	}

	// [6/29/2011 liuxu] 组呼模式下的特殊处理
	if (ISGROUPMODE(byVCMode))
	{
		if (!tNewCurVCMT.IsNull())
		{
			// 单回传下的处理
			if (!tNewCurVCMT.IsLocal() && !IsLocalAndSMcuSupMultSpy( tNewCurVCMT.GetMcuId()))
			{
				OnMMcuSetIn(tNewCurVCMT, m_cVCSConfStatus.GetCurSrcSsnId(), SWITCH_MODE_SELECT);
				
				// 获取电视墙管理模式
				const u8 byTvwMgrMode = m_cVCSConfStatus.GetTVWallManageMode();
				
				//zjj20091025 福建公安新需求,如果在组呼模式下
				//调度下级终端时，要把同一个下级的不同终端剔除除电视墙,因为现在只能有一个回传终端		
				TMt tDropMt;
				tDropMt.SetNull();
				
				if( byTvwMgrMode == VCS_TVWALLMANAGE_MANUAL_MODE )
				{
					FindSmcuMtInTvWallAndStop( tNewCurVCMT,&tDropMt );	
				}
			}			
		}
	}

	//20101222_tzy 只有在新调度终端非本级并且新调度终端所在MCU支持多回传时，才走多回传逻辑，否则走原来单回传逻辑
	//走单回传情景包括，取消调度，新调度终端为本级或者新调度为下级终端但不支持多回传
	if( ISGROUPMODE(byVCMode) 
		&& m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_AUTO_MODE 
		&& (tNewCurVCMT.IsNull() 
		    || tNewCurVCMT.IsLocal() 
			|| !IsLocalAndSMcuSupMultSpy(tNewCurVCMT.GetMcuId())))
	{
		TMt  tDropOutMT;
		tDropOutMT.SetNull();

		//zjj20090911调度下级终端时要把同级的其它终端剔出电视墙
		if(!tNewCurVCMT.IsNull() && !tNewCurVCMT.IsLocal() )
		{
			//zhouyiliang 20101229重整代码 合并单回传重复代码
			OnMMcuSetIn(tNewCurVCMT, m_cVCSConfStatus.GetCurSrcSsnId(), SWITCH_MODE_SELECT);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ChgCurVCMT]FindSmcuMtInTvWallAndStop\n" );
			FindSmcuMtInTvWallAndStop( tNewCurVCMT,&tDropOutMT );

			tDropOutMT.SetNull();			
		}

		// 下一个终端调度成功再进行，当前调度终端替换，原终端进电视墙的操作
		if (!tCurVCMT.IsNull() 
			&& m_tConfAllMtInfo.MtJoinedConf(tCurVCMT.GetMcuId(), tCurVCMT.GetMtId()))
		{
			TEqp tEqp;
			
			tEqp.SetNull();
			u8 byNeedInTW = TRUE;

			// 若当前调度的终端与新调度的终端属于同一个下级MCU,则无需将当前调度的终端放入电视墙
			if (!tCurVCMT.IsLocal() 
				&& !tNewCurVCMT.IsLocal()  
				&& IsMtInMcu(GetLocalMtFromOtherMcuMt(tCurVCMT), tNewCurVCMT))
			{
				byNeedInTW = FALSE;
			}

			// [5/30/2011 liuxu] 重写了FindUsableTWChan, 用FindNextTvwChnnl代替.
			CConfTvwChnnl cNextTvwChnnl;

			if (byNeedInTW)
			{
				// 找到可用的电视墙通道，则将原调度终端选看到电视墙；没有则给通知，并恢复到单方调度模式
				// [5/30/2011 liuxu] 重写了FindUsableTWChan, 用FindNextTvwChnnl代替
				u16 wNextChanIdx = FindNextTvwChnnl(m_cVCSConfStatus.GetCurUseTWChanInd(), &tCurVCMT, &cNextTvwChnnl, FALSE );
				tDropOutMT = (TMt)cNextTvwChnnl.GetMember();				

				if( wNextChanIdx 
					&& ( (IsValidHduChn(cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx()) && CheckHduAbility(tCurVCMT,cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx()))
						|| (IsValidTvw(cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx()) && !IsValidHduChn(cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx())))
				  )					
				{	
					if ( !tDropOutMT.IsNull() && !(tDropOutMT == tCurVCMT))
					{
						if (IsValidHduChn(cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx()))
						{
							// [2013/03/11 chenbing] VCS会议不支持HDU多画面,子通道置0
							ChangeHduSwitch( NULL,cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx(),0, TW_MEMBERTYPE_VCSAUTOSPEC, TW_STATE_STOP,MODE_BOTH,FALSE,FALSE );
						}
						else
						{
							ChangeTvWallSwitch( &tDropOutMT,cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx(),TW_MEMBERTYPE_VCSAUTOSPEC, TW_STATE_STOP);
						}
					}

					// [10/9/2011 liuxu] 不替换找到的单回传下同下级终端所占的通道,而是用找到的第一个可用通道
					CConfTvwChnnl cActTvwChnnl;
					cActTvwChnnl.Clear();
					if (GetCfgTvwChnnl( (u8)wNextChanIdx, cActTvwChnnl))
					{
						tEqp = g_cMcuVcApp.GetEqp( cActTvwChnnl.GetEqpId() );
						
						TTWSwitchInfo tSwitchInfo;
						tSwitchInfo.SetSrcMt(tCurVCMT);
						tSwitchInfo.SetDstMt(tEqp);

						tSwitchInfo.SetDstChlIdx(cActTvwChnnl.GetChnnlIdx());
						tSwitchInfo.SetMemberType(TW_MEMBERTYPE_VCSAUTOSPEC);
						tSwitchInfo.SetMode(MODE_VIDEO);
						
						VCSConfMTToTW(tSwitchInfo);
					}
				}

				// [8/16/2011 liuxu] 代码2： 单回传或者撤销当前调度终端情况下, 不管能否进入电视墙, 旧调度终端都要占用此通道
				if(wNextChanIdx)
					m_cVCSConfStatus.SetCurUseTWChanInd(wNextChanIdx);
			}			
		}
	}

	if( !IsLocalAndSMcuSupMultSpy( tNewCurVCMT.GetMcuId()) )
	{
		BOOL32 bIsSendToChairman = FALSE;
		if( m_tApplySpeakQue.ProcQueueInfo( tNewCurVCMT,bIsSendToChairman,FALSE ) )
		{
			NotifyMcsApplyList( bIsSendToChairman );
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ChgCurVCMT] Drop tMt(%d.%d) from Queue. \n",
						tNewCurVCMT.GetMcuId(),
						tNewCurVCMT.GetMtId()
						);
		}
		if( byVCMode != VCS_MULVMP_MODE &&
			byVCMode != VCS_GROUPVMP_MODE )
		{
			NotifyMtSpeakStatus( tCurVCMT, emCanceled );
		}			
		NotifyMtSpeakStatus( tNewCurVCMT, emAgreed );
	}

	switch(byVCMode)
	{
	case VCS_SINGLE_MODE:
		if (!tCurVCMT.IsNull() && 
			m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_MANUAL_MODE &&
			!m_ptMtTable->IsMtInTvWall( tCurVCMT.GetMtId() ) &&
            !m_ptMtTable->IsMtInHdu( tCurVCMT.GetMtId() ) && 
            IsMtNotInOtherTvWallChnnl(tCurVCMT,0,0) &&
			IsMtNotInOtherHduChnnl(tCurVCMT,0,0)
            )
		{
			VCSDropMT(tCurVCMT);
		}
		else
		{
			// 在电视墙里，变为非调度终端，停选看
			VCSConfStopMTSel(tCurVCMT, MODE_BOTH);
		}
		if ( !tNewCurVCMT.IsNull() && m_tConf.m_tStatus.IsSpecMixing() )
		{				
			AddSpecMixMember(&tNewCurVCMT, 1, TRUE);
		}
		break;

	case VCS_MULVMP_MODE:
		// 无需特殊操作		
	    break;

	case VCS_GROUPSPEAK_MODE:
	//20091026福建公安新需求，新增主席模式
	case VCS_GROUPCHAIRMAN_MODE:
	//zjj20091102 新增组呼点名模式
	case VCS_GROUPROLLCALL_MODE:
			// 新发言人模式代码调整(各终端看主席，主席看当前的调度终端，主席及当前调度终端混音)
			if (m_tConf.m_tStatus.IsSpecMixing() && 
				(tNewCurVCMT.IsNull() || tNewCurVCMT.IsLocal() || !IsLocalAndSMcuSupMultSpy(tNewCurVCMT.GetMcuId())))
			{
				if( (!m_cVCSConfStatus.GetMtInTvWallCanMixing() 
					|| (IsMtNotInOtherHduChnnl(tCurVCMT,0,0) 
						&& IsMtNotInOtherTvWallChnnl(tCurVCMT,0,0) 
						&& m_cVCSConfStatus.GetMtInTvWallCanMixing())) )
	
				{		
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[ChgCurVCMT] call RemoveSpecMixMember The tCurVCMT is (%d %d)!\n", 
						tCurVCMT.GetMcuId(), tCurVCMT.GetMtId());

					RemoveSpecMixMember( &tCurVCMT, 1, FALSE, FALSE );
					if (!tCurVCMT.IsLocal() && 
						GetLocalMtFromOtherMcuMt(tCurVCMT).GetMtId() !=  GetLocalMtFromOtherMcuMt(tNewCurVCMT).GetMtId() &&
						(!m_cVCSConfStatus.GetMtInTvWallCanMixing() || !IsHasMtInHduOrTwByMcuIdx(tCurVCMT.GetMcuIdx())))
					{
						TMt tLocalMt = GetLocalMtFromOtherMcuMt(tCurVCMT);
						RemoveSpecMixMember(&tLocalMt,1,FALSE,FALSE);
					}
				}
			}
			if (tNewCurVCMT.IsNull() )
			{
				if (VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState())
				{
					VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);
				}
				if( byVCMode == VCS_GROUPSPEAK_MODE )
				{
					TMt tChairMan = m_tConf.GetChairman();
					ChangeSpeaker(&tChairMan, FALSE, FALSE);
				}
			}
			else
			{
				
				//zjj20091026 福建公安新需求，组呼发言人模式当前调度终端做发言人
				if( VCS_GROUPSPEAK_MODE == byVCMode )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ChgCurVCMT] byVCMode is VCS_GROUPSPEAK_MODE ChangeSpeaker to curVcmt. \n" );
					ChangeSpeaker(&tNewCurVCMT, FALSE, FALSE);

					//20091102发言人模式 发言人看自己
					if( tNewCurVCMT.IsLocal() )
					{
						NotifyMtReceive(tNewCurVCMT, tNewCurVCMT.GetMtId());		
					}							
				}
				else
				{
					/*zhouyiliang 20110301点名双画面多回传上次画面合成的notify还没到，直接return
					if (VCS_GROUPROLLCALL_MODE == byVCMode && !m_tConfInStatus.IsVmpNotify() 
						&& IsLocalAndSMcuSupMultSpy(tNewCurVCMT.GetMcuId()) ) 
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ChgCurVCMT]VCS_GROUPROLLCALL_MODE last vmp req not notify yet! return\n");
						return;
					}*/
					TSwitchInfo tSwitchInfo;
					tSwitchInfo.SetSrcMt(tNewCurVCMT);
					tSwitchInfo.SetDstMt(m_tConf.GetChairman());
					tSwitchInfo.SetMode(MODE_VIDEO);
					//zjj20110223 当在主席轮询时调度某个支持多回传的下级mcu的终端时还是要尝试presetin,以将该终端
						//设置成为当前调度终端,保证监控画面可以显示该终端图像
					if ( VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState() || 
							(VCS_GROUPCHAIRMAN_MODE == byVCMode && !tNewCurVCMT.IsLocal() &&
								IsLocalAndSMcuSupMultSpy(tNewCurVCMT.GetMcuId())
							)
						)
					{
						//主席选看当前调度终端					
						BOOL32 bSelSuc = VCSConfSelMT(tSwitchInfo);
						if (!bSelSuc)
						{
							VCSConfStopMTSel(tSwitchInfo.GetDstMt(),tSwitchInfo.GetMode());
						}
					}
				
				}				

				if (m_tConf.m_tStatus.IsSpecMixing()
					&& (tNewCurVCMT.IsLocal() || !IsLocalAndSMcuSupMultSpy(tNewCurVCMT.GetMcuId())))
				{				
					AddSpecMixMember(&tNewCurVCMT, 1, TRUE);
				}
			}

			//zjj20091102
			if( VCS_GROUPROLLCALL_MODE == byVCMode )
			{
				m_cVCSConfStatus.SetReqVCMT(tNewCurVCMT);
				ChangeVmpStyle(tNewCurVCMT, TRUE);
			}
			break;

		case VCS_GROUPVMP_MODE:	
			if (!tNewCurVCMT.IsNull()
				&& m_tConfInStatus.IsVmpNotify()  //zhouyiliang 20101216 vcs自动画面合成如果上次notify还没来，这次不调整
				)
			{
				//组呼画面合成模式要把调度终端进入混音,
				//这里只是将非本地进入混音，因为本地的会在画面合成回VMP_MCU_CHANGESTATUS_NOTIF消息时由
				//ChangeSpecMixMember方法进入混音
				TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
				if ( !tConfVmpParam.IsMtInMember(tNewCurVCMT))
				{
					m_cVCSConfStatus.SetReqVCMT(tNewCurVCMT);
					ChangeVmpStyle(tNewCurVCMT, TRUE);					
				}
				else
				{
					// 2011-10-27 add by pgf:如果新调度终端已经在VMP中，就不需要再调VMP了，直接设，然后由界面发MCS_MCU_STARTSWITCHMT_REQ，再建到监控的交换
					m_cVCSConfStatus.SetCurVCMT(tNewCurVCMT);
				}
			}
			break;

		default:
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ChgCurVCMT]Wrong vcmode\n");
			break;
	}
	
	TMt tNullMt;
	tNullMt.SetNull();
	m_cVCSConfStatus.SetReqVCMT(tNullMt);

	// 通告给所有的VCS会议状态的改变
	VCSConfStatusNotif();
}

/*====================================================================
函数名      ：FindNextTWChan
功能        ：查询下一个可用的电视墙通道
算法实现    ：(1) 遍历算法是从swCurChnnlIdx+1位置开始,依次遍历后面通道; 遍历过程中会返回0索引循环找
			  (2) 如果ptNextMt为空, 则返回swCurChnnlIdx后面第一个可用的通道的索引; 找不到返回0
			  (3) 否则如果ptNextMt为下级终端, 且单回传, 如果能找到与它同级的其它终端所占的通道,
				  则返回此通道, 否则返回第一个可用的通道. 找不到返回0
			  (4) 否则返回第一个可用的通道. 找不到返回0
引用全局变量：
输入参数说明：[IN]		wCurChnnlIdx : 当前正使用的通道索引
			  [IN]         ptNextMt: 欲进入电视墙的终端
			  [OUT]		pcNextChnnl: 若没有未用的通道，被抢占的终端信息
			  [in] bUseSingleSpyFirst: 是否优先使用单回传下ptNextMt同级终端所占的通道
			  默认是true, 单回传时优先使用ptNextMt(非本级)同级所占的电视墙通道
返回值说明  ：s16 : 返回当前可用的通道索引号, 没找到返回值小于0， 找到则大于或等于0
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/05/28                刘旭          创建
====================================================================*/
u16 CMcuVcInst::FindNextTvwChnnl( const u16 wCurChnnlIdx, 
								 const TMt* ptNextMt/* = NULL*/, 
								 CConfTvwChnnl* pcNextChnnl /*= NULL*/,
								 const BOOL32 bUseSingleSpyFirst/* = TRUE*/)
{
	// 会议总共配置的电视墙通道个数
	const u16 wConfCfgedTvwChnNum = GetVcsHduAndTwModuleChnNum();
	if ( wCurChnnlIdx > wConfCfgedTvwChnNum )
	{
		return 0;
	}

	// 主动将pcNextChnnl清空
	if (pcNextChnnl)
	{
		pcNextChnnl->Clear();
	}
	
	CConfTvwChnnl cTvwChnnl;								// 待寻找的通道
	BOOL32 bFindChnnlFinded = FALSE;						// 单回传时第一个可用的通道是否找到的标志
	u16 wNextChnnlIdx = 0;									// 待返回的通道索引值

	// 从swCurChnnlIdx++位置开始遍历所有tvw通道, 直到找到合适的通道. 最多遍历byConfCfgedTvwChnNum次
	for ( u16 wLoop = 0, wNextIdx = wCurChnnlIdx + 1; wLoop < wConfCfgedTvwChnNum; ++wLoop, ++wNextIdx)
	{
		// 如果下一个通道索引大于最大索引, 则从1 开始遍历
		if ( wNextIdx > wConfCfgedTvwChnNum )
		{
			wNextIdx = 1;
		}
	
		if (!GetCfgTvwChnnl((u8)wNextIdx, cTvwChnnl))
		{
			continue;
		}

		// 通道所在设备不在线, continue;增加对HDU通道状态的判断,状态不对则continue;
		if ( !g_cMcuVcApp.IsPeriEqpConnected(cTvwChnnl.GetEqpId()) || 
			 (HDUID_MIN <= cTvwChnnl.GetEqpId() && cTvwChnnl.GetEqpId() <= HDUID_MAX && 
			 cTvwChnnl.GetStatus() != THduChnStatus::eREADY && cTvwChnnl.GetStatus() != THduChnStatus::eRUNNING)
		   )
		{
			continue;
		}

		// 通道成员类型不是VCS会议自动类型, continue
		if ( TW_MEMBERTYPE_VCSAUTOSPEC != cTvwChnnl.GetConfMemberType()
			&& cTvwChnnl.GetConfIdx() != 0 )
		{
			continue;
		}

		// 通道被其他会议占用就continue,不管里面是否有成员
		if (/* !cTvwChnnl.GetMember().IsNull() &&*/
			cTvwChnnl.GetMember().GetConfIdx() != 0 
			&& cTvwChnnl.GetMember().GetConfIdx() != m_byConfIdx)
		{
			continue;
		}

		// 单回传时, 下级终端进入电视墙, 优先替换它同级所在的电视墙通道
		if (ptNextMt && !ptNextMt->IsNull() &&!ptNextMt->IsLocal() && !IsLocalAndSMcuSupMultSpy(ptNextMt->GetMcuIdx()))
		{
			// 把第一个可用的位置记录下来
			if (!bFindChnnlFinded)
			{
				bFindChnnlFinded = TRUE;
				wNextChnnlIdx = wNextIdx;
				if ( pcNextChnnl )
				{
					*pcNextChnnl = cTvwChnnl;
				}
				
				// 如果不优先替换多回传所占通道, 那么这里就可以返回了
				if (!bUseSingleSpyFirst)
				{
					return wNextChnnlIdx;
				}
			}

			// 找到下级终端的同级终端终端所占的通道, 返回此通道
			if (!cTvwChnnl.GetMember().IsNull() 
				&& GetFstMcuIdFromMcuIdx( cTvwChnnl.GetMember().GetMcuId() ) == GetFstMcuIdFromMcuIdx( ptNextMt->GetMcuId() ))
			{
				if ( pcNextChnnl )
				{
					*pcNextChnnl = cTvwChnnl;
				}
				
				// [10/9/2011 liuxu] 修改成不替换该通道, 而是取消同级所在的其它通道, 但返回
				// 第一个找到的可用通道
				
				if (wNextChnnlIdx
					&& wNextIdx != wNextChnnlIdx
					&& bUseSingleSpyFirst)
				{
					//举例:  本来已经轮到2了,现在该进第三个通道. 由于采取优先替换策略, 
					//		 而找到的通道又不是第三个通道, 那么计数应该还停留在2
					wNextChnnlIdx = (wNextChnnlIdx > 1) ? ( wNextChnnlIdx -1 ): wNextChnnlIdx;
					return wNextChnnlIdx;
				}else
				{
					wNextChnnlIdx = wNextIdx;
				}
				
				break;	//return wNextIdx;
			}
		}
		// 其它情况下, 可以直接返回此电视墙通道
		else
		{
			if ( pcNextChnnl )
			{
				*pcNextChnnl = cTvwChnnl;
			}

			return wNextIdx;
		}
		
		continue;
	}

	// 在以上遍历中, 如果参数ptNextMt为空, 则这里返回-1
	// 否则如果ptNextMt为下级终端, 且单回传, 则这里返回最早找到的
	// 否则这里(ptNextMt为本级, 或支持多回传)也返回-1
	return wNextChnnlIdx;
}

/*==============================================================================
函数名    :  GetVcsRestoreConfVmpParam
功能      :  获得vcs切换场景时的默认VmpParam信息
算法实现  :  
参数说明  :  
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-03-20                yanghuaizhi
==============================================================================*/
TVMPParam_25Mem CMcuVcInst::GetVcsRestoreConfVmpParam()
{
	TVMPParam_25Mem  tParam;
	tParam.SetVMPAuto(TRUE);
	tParam.SetVMPMode(CONF_VMPMODE_AUTO);
	tParam.SetVMPStyle(VMP_STYLE_DYNAMIC);
	tParam.SetVMPBrdst(TRUE);
	tParam.SetVMPSchemeId(0);

	return tParam;
}

/*====================================================================
    函数名      ：FindUsableVMP
    功能        ：查询可用的画面合成器，并且占用该画面合成器
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
BOOL CMcuVcInst::FindUsableVMP()
{
	/* 暂时给一套默认值
	TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
	tConfAttrib.SetHasVmpModule(TRUE);
	m_tConf.SetConfAttrb(tConfAttrib);

	TVMPParam  tParam;
	memset(&tParam, 0, sizeof(tParam));	// [12/3/2010 xliang] set 0 is a must as it has no construct.

	tParam.SetVMPAuto(TRUE);
	tParam.SetVMPMode(CONF_VMPMODE_AUTO);
	tParam.SetVMPStyle(VMP_STYLE_DYNAMIC);
	tParam.SetVMPBrdst(TRUE);
	tParam.SetVMPSchemeId(0);

	//TODO:待yanghuaizhi调整
	m_tConfEqpModule.SetVmpModuleParam(tParam);

	tConfAttrib = m_tConf.GetConfAttrb();
	if(tConfAttrib.IsHasVmpModule())			
	{*/
		u8 byIdleVMPNum = 0;
		u8 abyVMPId[MAXNUM_PERIEQP];
		memset(abyVMPId, 0, sizeof(abyVMPId));
		g_cMcuVcApp.GetIdleVMP(abyVMPId, byIdleVMPNum, sizeof(abyVMPId));

		//是否有画面合成器
		if( byIdleVMPNum != 0)
		{
			//TVmpModule tVmpModule = GetVmpModule();
			TVMPParam_25Mem tVmpParam = GetVcsRestoreConfVmpParam();

			u8  byVmpId = 0;
            if ( CONF_VMPMODE_AUTO == tVmpParam.GetVMPMode() )
            {
                //当前所有的空闲VMP是否支持所需的合成风格
                u16 wError  = 0;

                if ( IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError) ) 
                {
					// vmp后适配vicp资源占用判断，建后适配列表
					if (!PrepareVmpOutCap(byVmpId))
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[FindUsableVMP] There is not enough vicp.\n");
						g_cMcuVcApp.ClearVmpInfo(g_cMcuVcApp.GetEqp(byVmpId));
						return FALSE;
					}

					m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );//VCS都是广播vmp，此记录正确
					m_tVmpEqp.SetConfIdx( m_byConfIdx );
                }
                else
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[FindUsableVMP] no vmp support thise style\n");
					return FALSE;
				}
			}
			else
			{
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[FindUsableVMP] VCSCONF hasn't non-auto vmp\n");
				return FALSE;
			}

			// 有则占用画面合成器
			AddVmpIdIntoVmpList(byVmpId);
            TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::RESERVE;//先占用,超时后未成功再放弃
			tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);
			tPeriEqpStatus.SetConfIdx( m_byConfIdx );
			g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[FindUsableVMP]vmp(%d) has been used by conf(%d)\n", byVmpId, m_byConfIdx);
			// 开启时更新最大前适配数目
			TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
			TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
			tVmpChnnlInfo.clear();// 开启时清空前适配信息
			u8 byMaxStyleNum = 0; //最大支持几风格以内不降分辨率
			u8 byMaxHdChnnlNum = GetMaxHdChnlNumByVmpId(m_tConf, byVmpId, byMaxStyleNum);
			tVmpChnnlInfo.SetMaxNumHdChnnl(byMaxHdChnnlNum);
			tVmpChnnlInfo.SetMaxStyleNum(byMaxStyleNum);
			g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);

			/*清空通道
			for( u8 byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++ )
			{
				m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );
			}*/
		}
		else
		{
			return FALSE;
		}
	/*}
	else
	{
		return FALSE;
	}*/

	return TRUE;
}

/*====================================================================
    函数名      ：RestoreVCConf
    功能        ：恢复会议到未进行任何调度操作状态的新模式
    算法实现    ：1.对于新模式,释放不需要的资源
	              2.切换至该模式的初始状态
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
void CMcuVcInst::RestoreVCConf(u8 byNewMode)
{
	if (CurState() != STATE_ONGOING)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[RestoreVCConf] wrong curstate(%d)\n", CurState());
		return;
	}

	CServMsg cServMsg;
	cServMsg.SetConfIdx(m_byConfIdx);
	cServMsg.SetConfId(m_tConf.GetConfId());

	u8 byOldMode = m_cVCSConfStatus.GetCurVCMode();

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "restore vcsconf from %d to %d\n", byOldMode, byNewMode);

	//zhouyiliang 20100812 如果是组呼之间且换，多回传要先做相应的free
	if( ISGROUPMODE(byOldMode) && ISGROUPMODE(byNewMode) )
	{
		//zhouyiliang 20100812 1.无条件free一次
		TMt tCurVcMt =	m_cVCSConfStatus.GetCurVCMT();
		if ( !tCurVcMt.IsNull() && !tCurVcMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tCurVcMt.GetMcuId())  ) 
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[RestoreVCConf] stop mc switch to curVcMt, StopSwitchToAllMcWatchingSrcMt:McuId.%d,MtId.%d\n"
				,tCurVcMt.GetMcuId(),tCurVcMt.GetMtId());
			AdjustSwitchToAllMcWatchingSrcMt( tCurVcMt,TRUE,MODE_BOTH,TRUE,VCS_VCMT_MCCHLIDX );
		}
	}

    // 停止本地主席终端的码流发送
    //NotifyMtSend(m_tConf.GetChairman().GetMtId(), MODE_VIDEO, FALSE);
	// 对于保持在线的主席终端状态的恢复
	// 若主席终端在选看下级MCU，则取消
	// 非组呼模式下主席轮询的停止
	if (!ISGROUPMODE(byNewMode))
	{
		KillTimer(MCUVC_VCS_CHAIRPOLL_TIMER);
		m_cVCSConfStatus.SetChairPollState(VCS_POLL_STOP);
	}

	if (!(ISGROUPMODE(byNewMode) && ISGROUPMODE(byOldMode)) ||
		//新组呼发言人模式需要停止主席选看
		( ( byOldMode == VCS_GROUPSPEAK_MODE || byOldMode == VCS_GROUPTW_MODE ||
			byOldMode == VCS_GROUPCHAIRMAN_MODE || byOldMode == VCS_GROUPROLLCALL_MODE ) &&
		   byNewMode != byOldMode &&
     	   m_cVCSConfStatus.GetChairPollState() == VCS_POLL_STOP )
		)
	{
		TMtStatus tChairmanStat;
		if (m_ptMtTable->GetMtStatus(m_tConf.GetChairman().GetMtId(), &tChairmanStat))
		{
			if (!tChairmanStat.GetSelectMt(MODE_VIDEO).IsNull())
			{
				VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);	
			}

			if (!tChairmanStat.GetSelectMt(MODE_AUDIO).IsNull())
			{
				VCSConfStopMTSel(m_tConf.GetChairman(), MODE_AUDIO);
			}
		}
	}

	// 画面合成器资源的释放
	// 先停止画面合成，避免一个个挂在线终端时，不必要的画面合成切换
	if (g_cMcuVcApp.GetVMPMode(m_tVmpEqp) != CONF_VMPMODE_NONE &&
		byNewMode != VCS_MULVMP_MODE && byNewMode != VCS_GROUPVMP_MODE &&
		byNewMode != VCS_GROUPROLLCALL_MODE )
	{
		TPeriEqpStatus tPeriEqpStatus;
		if (!m_tVmpEqp.IsNull() &&
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
			tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState != TVmpStatus::WAIT_STOP)
		{
			u8 byVmpIdx = m_tVmpEqp.GetEqpId() - VMPID_MIN;
			SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER+byVmpIdx, TIMESPACE_WAIT_VMPRSP);
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
			SendMsgToEqp( m_tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cServMsg );
		}
	}

	// 混音器资源的释放
	// 停止混音器	
	//zjj20091102 如果新模式是单方调度且没起一键混音，才能停止混音
	if( m_tConf.m_tStatus.IsMixing() && !ISGROUPMODE(byNewMode) && 
		byNewMode != VCS_MULTW_MODE && byNewMode != VCS_MULVMP_MODE &&		
		( byNewMode == VCS_SINGLE_MODE && !m_cVCSConfStatus.GetMtInTvWallCanMixing() ) 
		)
	{
		StopMixing();
	}

	u8 byCanStopToTvWall = 0;

	// 电视墙资源的释放
	// 电视墙通道处理
	if (GetVcsHduAndTwModuleChnNum())						// 会议配置了电视墙
	{
		// 一个会议支持的电视墙通道的总数
		const u8 byMaxTvwChnnlNum = MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL + MAXNUM_PERIEQP_CHNNL * MAXNUM_MAP;
		CConfTvwChnnl acTvwChnnlFind[byMaxTvwChnnlNum];		// 待寻找的电视墙通道
		u8 byEqpId, byChnnlIdx;								// 电视墙通道对应的设备号和在设备中的通道号
		
		u8 abyTvwEqpId[128] = { 0 };

		u8 byMtConfIdx;										// 电视墙通道成员的会议idx
		u8 byInTWMtId;										// 电视墙通道成员id
		TMt tDropMt;
		u8 byLoop;											
		//TSwitchInfo tSwitchInfo;

		// 获取所有电视墙通道
		const u8 byTvwChnnlNum = GetAllCfgedTvwChnnl( acTvwChnnlFind, byMaxTvwChnnlNum );
		for ( byLoop = 0; byLoop < byTvwChnnlNum; ++byLoop )
		{
			if (acTvwChnnlFind[byLoop].IsNull())
			{
				continue;
			}
			
			byEqpId = acTvwChnnlFind[byLoop].GetEqpId();
			byChnnlIdx = acTvwChnnlFind[byLoop].GetChnnlIdx();
			
			if ( 0 != acTvwChnnlFind[byLoop].GetMember().GetConfIdx() 
				&& acTvwChnnlFind[byLoop].GetMember().GetConfIdx() != m_byConfIdx )
			{
				continue;
			}

			// 不在线, 则不进行操作
			if (!g_cMcuVcApp.IsPeriEqpConnected(byEqpId))
			{
				continue;
			}
			
			byCanStopToTvWall = 1;
			byMtConfIdx =  acTvwChnnlFind[byLoop].GetMember().GetConfIdx();
			
			u8 byMemberType = acTvwChnnlFind[byLoop].GetMember().byMemberType;
			
			//zjj20091102 福建公安新需求，非组呼到非组呼，组呼到组呼并且成员类型是TW_MEMBERTYPE_VCSSPEC不挂断	
			if ( ((!ISGROUPMODE( byOldMode) && !ISGROUPMODE( byNewMode)) || (ISGROUPMODE( byOldMode) && ISGROUPMODE( byNewMode)) ) 
				&& ( TW_MEMBERTYPE_VCSSPEC == byMemberType || TW_MEMBERTYPE_VCSAUTOSPEC == byMemberType ) )
			{
				byCanStopToTvWall = 0;
			}
			
			// [6/8/2011 liuxu] vcs组呼模式之间切换时, 通道在轮询的不挂终端
			if ((ISGROUPMODE( byOldMode) && ISGROUPMODE( byNewMode))
				&& TW_MEMBERTYPE_TWPOLL == byMemberType )
			{
				byCanStopToTvWall = 0;
			}

			if (!ISGROUPMODE( byNewMode))
			{
				// [6/8/2011 liuxu] 电视墙在轮询,也要停止
				u8 byPollState = POLL_STATE_NONE;
				if( m_tTWMutiPollParam.GetPollState(byEqpId, byChnnlIdx, byPollState) && POLL_STATE_NONE != byPollState)
				{
					//获取定时器索引
					u32 dwTimerIdx = 0;
					if(!m_tTWMutiPollParam.GetTimerIdx(byEqpId, byChnnlIdx, dwTimerIdx))
					{
						ConfPrint( LOG_LVL_WARNING, MID_MCU_HDU, "[ProcMcsMcuHduPollMsg] Cann't get tvw<EqpId:%d, ChnId:%d> TimerIdx!\n", byEqpId, byChnnlIdx);
						continue;
					}
					
					// 再次设置定时器,如果组呼到非组呼,终端会挂断,找不到下一个轮询终端,最终会释放资源
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx,100,dwTimerIdx);	
				}
			}
			
			if (m_byConfIdx == byMtConfIdx && byCanStopToTvWall )
			{							
				byInTWMtId  = GetLocalMtFromOtherMcuMt((TMt)acTvwChnnlFind[byLoop].GetMember()).GetMtId();
				
				StopSwitchToPeriEqp(byEqpId, byChnnlIdx);
				tDropMt = GetLocalMtFromOtherMcuMt( (TMt)acTvwChnnlFind[byLoop].GetMember() );

				ClearTvwChnnlStatus(byEqpId, byChnnlIdx);

				//清终端状态
				m_ptMtTable->SetMtInHdu(byInTWMtId, FALSE);
				TMt tMt = m_ptMtTable->GetMt(byInTWMtId);
				MtStatusChange(&tMt, TRUE);
				
				//zjj20091102 清楚出混音
				if( !tDropMt.IsNull() 
					&& m_ptMtTable->IsMtInMixing( tDropMt.GetMtId() )) 
				{
					RemoveSpecMixMember( &tDropMt, 1, FALSE );
				}	
				
				// 标清电视墙终端退出时需要恢复分辨率
				if (!IsValidHduChn(byEqpId, byChnnlIdx))
				{
					if( NeedChangeVFormat(tDropMt) 
						&& !m_ptMtTable->IsMtInTvWall( GetLocalMtFromOtherMcuMt(tDropMt).GetMtId() ) )
					{
						ChangeVFormat( tDropMt, FALSE );					//恢复分辨率
					}
				}

				// 加入abyTvwEqpId数组, 待后续一起上报
				for (u8 byTvwNotify = 0; byTvwNotify < 128; ++byTvwNotify)
				{
					if ( byEqpId && byEqpId == abyTvwEqpId[byTvwNotify] )
					{
						break;
					}

					if ( 0 == abyTvwEqpId[byTvwNotify] )
					{
						abyTvwEqpId[byTvwNotify] = byEqpId;
						break;
					}
				}
			}
			
			//zjj20091102在单方模式下，且又没起电视墙混音，电视墙中的终端就要恢复看主席
			/*if( 0 == byCanStopToTvWall && VCS_SINGLE_MODE == byNewMode )
			{						
				if( !acTvwChnnlFind[byLoop].GetMember().IsNull() )
				{						
					tDropMt = GetLocalMtFromOtherMcuMt( (TMt)acTvwChnnlFind[byLoop].GetMember() );
					
					tSwitchInfo.SetSrcMt( m_tConf.GetChairman() );
					tSwitchInfo.SetDstMt( tDropMt );						
					tSwitchInfo.SetMode( MODE_BOTH );						
					VCSConfSelMT( tSwitchInfo );
				}
			}*/			
		}

		// 集中上报tvwall状态
		TPeriEqpStatus tTvwallStatus;
		for ( byLoop = 0; byLoop < 128; ++byLoop )
		{
			if ( abyTvwEqpId[byLoop] 
				&& g_cMcuVcApp.GetPeriEqpStatus(abyTvwEqpId[byLoop], &tTvwallStatus))
			{
				cServMsg.SetMsgBody((u8 *)&tTvwallStatus, sizeof(tTvwallStatus));
				SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
			}else
			{
				break;
			}
		}
	}

	BOOL32 bPollStart = m_cVCSConfStatus.GetChairPollState() == VCS_POLL_START;
	// 非组呼之间切换, 轮询要停止
	bPollStart = bPollStart && ISGROUPMODE(byOldMode) && ISGROUPMODE(byNewMode);

	// 释放选看终端
	/*for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		// [8/29/2011 liuxu] 如果在组呼之间切换时,主席轮询不能被拆
		TMt tLoopMt = m_ptMtTable->GetMt(byMtId);
		if (bPollStart && tLoopMt == m_tConf.GetChairman())
		{
			continue;
		}

		if (m_tConfAllMtInfo.MtJoinedConf(byMtId) && 
			//CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtId) &&
			MT_TYPE_MT ==m_ptMtTable->GetMtType(byMtId) &&
			byMtId != m_tConf.GetChairman().GetMtId())
		{
			VCSConfStopMTSel(tLoopMt, MODE_BOTH);
		}
	}*/

	//zjj20101222 后面的一些操作比较慢,结束组呼后界面可能因此滞留,所以先报模式给界面,之后的操作mcu在后台做
	if( ISGROUPMODE(byOldMode) && !ISGROUPMODE(byNewMode) )
	{
		m_cVCSConfStatus.SetCurVCMode(byNewMode);
		// 通告给所有的VCS会议状态的改变
		VCSConfStatusNotif();
	}

	u16 wMcuIdx = INVALID_MCUIDX;
	// 调度资源的释放
	// 非组呼模式内切换，挂断所有非定时呼叫的调度资源
	if (!(ISGROUPMODE(byNewMode)  && ISGROUPMODE(byOldMode)))
	{
		TMt tMt;
		BOOL32 bIsNotInTWorHdu = FALSE;
		for( u8 byIndex = 1; byIndex <= MAXNUM_CONF_MT; byIndex++ )
		{	
			tMt = m_ptMtTable->GetMt(byIndex);
			//zhouyiliang 20110119 主席和备份主席即使不在线也要保持定时呼叫
			if(!m_tConfAllMtInfo.MtJoinedConf(byIndex) && 
				!(tMt == m_cVCSConfStatus.GetVcsBackupChairMan()) &&
				!(tMt == m_tConf.GetChairman())
			  )
			{
				m_ptMtTable->SetCallMode( byIndex,CONF_CALLMODE_NONE );
				continue;
			}
			bIsNotInTWorHdu = IsMtNotInOtherTvWallChnnl( byIndex,0,0 ) && IsMtNotInOtherHduChnnl( byIndex,0,0 );
			if ( m_tConf.GetChairman().GetMtId() != byIndex &&//CONF_CALLMODE_NONE == m_ptMtTable->GetCallMode(byIndex) &&
				m_ptMtTable->GetMtType(byIndex) != MT_TYPE_MMCU &&
				//zjj20091025 福建公安新需求，在电视墙中的终端不挂断
				/*IsMtNotInOtherTvWallChnnl( byIndex,0,0 ) &&
				IsMtNotInOtherHduChnnl( byIndex,0,0 )*/
				bIsNotInTWorHdu
				//zhouyiliang 20100824 vcs本地地址1+1备份的mt不能drop
				&& m_cVCSConfStatus.GetVcsBackupChairMan().GetMtId() != byIndex
				) 
			{				
				VCSDropMT(tMt);
			}
			else if (!bIsNotInTWorHdu)
			{
				// 在hdu中，不挂断，清除选看，单方调度切多方多画面，因调度终端在电视墙不挂断，导致终端一直选看主席而未看vmp
				VCSConfStopMTSel(tMt, MODE_BOTH);
			}
			/*else if (MT_TYPE_SMCU == m_ptMtTable->GetMtType(byIndex))
			{
				TMt tSMt;
				wMcuIdx = GetMcuIdxFromMcuId( byIndex );
				//在线的SMCU,挂断其下所有的非定时呼叫的调度资源
				TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);//byIndex);
				if (NULL == ptMcInfo)
				{
					continue;
				}
				for (u8 bySIndex = 1; bySIndex <= MAXNUM_CONF_MT; bySIndex++)
				{
					tSMt.SetMcuId(byIndex);
					tSMt.SetMtId(bySIndex);
					if (m_tConfAllMtInfo.MtJoinedConf(wMcuIdx, bySIndex) //&& 
						//CONF_CALLMODE_NONE == VCSGetCallMode(tSMt)
						)
					{
						VCSDropMT(tSMt);
					}
				}
            
				// 若正在选看，则取消选看
				if (!ptMcInfo->m_tSpyMt.IsNull())
				{
					VCSConfStopMTSel(tMt, MODE_BOTH);
				}
				// 清空回传通道中的信息
				ptMcInfo->m_tMMcuViewMt.SetNull();
			}*/
		}
	}

	if ((ISGROUPMODE(byOldMode) && !ISGROUPMODE(byNewMode)) ||
		CONF_CREATE_MT == m_byCreateBy)
	{
		RlsAllBasForConf();
	}

	// 发言人清空
	ChangeSpeaker(NULL, FALSE, FALSE);

	// [1/24/2011 liuxu][添加]停止会议录像
	if ((ISGROUPMODE(byOldMode) && !ISGROUPMODE(byNewMode))
		&& !m_tConf.m_tStatus.IsNoRecording())
	{
		//send it to recorder
		TMt tConfMt;
		tConfMt.SetNull();

		cServMsg.SetChnIndex( m_byRecChnnl );
		cServMsg.SetMsgBody( (u8*)&tConfMt, sizeof( tConfMt ) );
		cServMsg.CatMsgBody( (u8*)&m_tRecEqp, sizeof( m_tRecEqp ) );
		SendMsgToEqp( m_tRecEqp.GetEqpId(), MCU_REC_STOPREC_REQ, cServMsg );
	}

	if( !m_cVCSConfStatus.GetCurVCMT().IsNull() )
	{
		NotifyMtSpeakStatus( m_cVCSConfStatus.GetCurVCMT(), emCanceled );
	}

	// 恢复主席终端的非静哑音状态
	// 模式切换时不恢复静哑音状态
// 	VCSMTMute(m_tConf.GetChairman(), FALSE, VCS_AUDPROC_MUTE);
// 	VCSMTMute(m_tConf.GetChairman(), FALSE, VCS_AUDPROC_SILENCE);

	// 目前对于组呼会议要求BAS充足,否则不允许进入组呼
	u16 wErrCode;
	if (!ISGROUPMODE(byOldMode) &&
		ISGROUPMODE(byNewMode) &&
		!PrepareAllNeedBasForConf(&wErrCode))
	{
		cServMsg.SetErrorCode(wErrCode);
		SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cServMsg);
		RestoreVCConf(VCS_SINGLE_MODE);		
		return;
	}

	// VCS会议状态恢复
	if (ISGROUPMODE(byOldMode) && ISGROUPMODE(byNewMode))
	{
		// miaoqingsong [05/12/2011] 添加是否是在电视墙中混音终端的判断，组呼会议间切换一定要把前一次
        // 的调度终端退出混音,在这里只是清非本地的，因为本地的会在下面的ChangeSpecMixMember方法中清除
		TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();
		m_cVCSConfStatus.VCCRestoreGroupStatus();		
		
		if( m_tConf.m_tStatus.IsMixing() &&
			!tCurVCMT.IsNull() &&
			!tCurVCMT.IsLocal() &&
			( ( IsMtNotInOtherHduChnnl( tCurVCMT,0,0 ) &&
			IsMtNotInOtherTvWallChnnl( tCurVCMT,0,0 ) ) ||
			!m_cVCSConfStatus.GetMtInTvWallCanMixing() )
			//!m_ptMtTable->IsMtInHdu( m_cVCSConfStatus.GetCurVCMT().GetMcuId() ) &&
			//!m_ptMtTable->IsMtInTvWall( m_cVCSConfStatus.GetCurVCMT().GetMcuId() ) 	
		  )

		{
			RemoveSpecMixMember( &tCurVCMT, 1, FALSE );
			if (!tCurVCMT.IsLocal() && 
				(!m_cVCSConfStatus.GetMtInTvWallCanMixing() || !IsHasMtInHduOrTwByMcuIdx(tCurVCMT.GetMcuIdx())))
			{
				TMt tLocalMt = GetLocalMtFromOtherMcuMt(tCurVCMT);
				RemoveSpecMixMember(&tLocalMt,1,FALSE,FALSE);
			}
		}
	}
	else
	{
		m_cVCSConfStatus.VCCRestoreStatus();
		//zhouyiliang 20100917 单方调度切换到多方多画面,根据当前在线终端调整当前可用通道号,因为可能有终端在电视墙中，没有挂断
		if (VCS_SINGLE_MODE == byOldMode && VCS_MULVMP_MODE == byNewMode ) 
		{
			u8 byOnLineMtNum = 1;
			for( u8 byIndex = 1; byIndex <= MAXNUM_CONF_MT; byIndex++ )
			{
				if ( !m_tConfAllMtInfo.MtJoinedConf(byIndex) 
				|| m_cVCSConfStatus.GetVcsBackupChairMan().GetMtId() == byIndex 
				|| m_tConf.GetChairman().GetMtId() == byIndex) 
				{
					continue;
				}
				byOnLineMtNum++;
			}
			m_cVCSConfStatus.SetCurUseVMPChanInd( byOnLineMtNum );
		}
	}

	//zjj20091102 如果是单方调度而且起了电视墙一键混音也需要起混音
	if (ISGROUPMODE(byNewMode) 
		|| VCS_MULTW_MODE == byNewMode 
		|| VCS_MULVMP_MODE == byNewMode 
		|| ( VCS_SINGLE_MODE == byNewMode && m_cVCSConfStatus.GetMtInTvWallCanMixing() )
		)
	{
		TMt tChairMan = m_tConf.GetChairman();
		if (!m_tConf.m_tStatus.IsMixing())
		{
			// [miaoqingsong 20111031] 添加混音器状态打印
// 			for ( u8 byLoopEqpId = 1; byLoopEqpId <= MAXNUM_MCU_PERIEQP ; byLoopEqpId++ )
// 			{
// 				u8 byMixerSubtype = UNKONW_MIXER;
// 				g_cMcuAgent.GetMixerSubTypeByEqpId(byLoopEqpId,byMixerSubtype);
// 				if( g_cMcuVcApp.IsPeriEqpValid( byLoopEqpId ) && 
// 					(byMixerSubtype == EAPU_MIXER || byMixerSubtype == APU2_MIXER || byMixerSubtype == MIXER_8KI)&&
// 					g_cMcuVcApp.m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus.m_byOnline )
// 				{
// 					OspPrintf( TRUE, FALSE, "[RestoreVCConf] EMixer Group's status is %d!\n", 
// 						g_cMcuVcApp.m_atPeriEqpTable[byLoopEqpId-1].m_tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[0].m_byGrpState );    
// 				}     
// 			}

			// 组呼模式均启动定制混音, 多方多画面或电视墙使用全体混音
			BOOL32 dwStartResult = FALSE;

			//tianzhiyong 2010/03/21 增加开启方式和开启模式
			dwStartResult = StartMixing(mcuPartMix);
			if (!dwStartResult)
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[RestoreVCConfInGroupMode] Find no mixer\n");
				if (ISGROUPMODE(byNewMode))
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_NOMIXSPEAKER);
				}
				else
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_NOMIXER);
				}
				SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cServMsg);
			}
			else
			{
				// 初始状态只将主席放入混音器
				for (u8 byMixMtId = 1; byMixMtId <= MAXNUM_CONF_MT; byMixMtId++)
				{
					if (tChairMan.GetMtId() == byMixMtId)
					{
						m_ptMtTable->SetMtInMixing(byMixMtId, TRUE, TRUE);
					}
					else if( (m_ptMtTable->IsMtInTvWall( byMixMtId ) ||
						m_ptMtTable->IsMtInHdu( byMixMtId ) ) &&
						m_ptMtTable->GetMtType( byMixMtId ) != MT_TYPE_SMCU
						)
					{
						m_ptMtTable->SetMtInMixing(byMixMtId, TRUE, TRUE);
					}
					else
					{
						if( m_ptMtTable->GetMtType( byMixMtId ) != MT_TYPE_SMCU )
						{
							m_ptMtTable->SetMtInMixing(byMixMtId, FALSE, TRUE);
						}
					}
				}
			}
		}
		else
		{
			// 仅主席终端留在混音器中
/*			TMt tMt;*/
// 			for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
// 			{
// 				tMt = m_ptMtTable->GetMt(byMtId);
// 				if (m_ptMtTable->IsMtInMixGrp(byMtId) &&
// 					!(tMt == tChairMan))
// 				{
// 					RemoveMixMember(&tMt);
// 					StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
// 										(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())),
// 										FALSE, MODE_AUDIO );
// 					g_cMpManager.StopSwitchToSubMt(tMt, MODE_AUDIO);   
// 					SwitchMixMember(&tMt, TRUE);
// 				}
// 			}
			//20091102 所有会议模式都起定制混音
			//if (ISGROUPMODE(byNewMode))
			//{
				m_tConf.m_tStatus.SetSpecMixing();
				ChangeSpecMixMember(&tChairMan, 1);
				if (!m_ptMtTable->IsMtInMixing(tChairMan.GetMtId()))
				{
					AddSpecMixMember(&tChairMan,1);
				}
			//}
			
			//else
			//{
			//	SwitchToAutoMixing();
			//}
		}
	}

	//非组呼模式切到组呼模式，重载组呼列表
	if (!ISGROUPMODE(byOldMode) && ISGROUPMODE(byNewMode))
	{
		ReLoadOrigMtList();
	}
	if( ISGROUPMODE(byOldMode) && !ISGROUPMODE(byNewMode) )
	{
		ReLoadOrigMtList( FALSE );
	}

	//zbq [05/13/2010] 组呼模式切换到非组呼模式，清空发言/抢答申请列表
    if ( (!ISGROUPMODE(byOldMode) && ISGROUPMODE(byNewMode)) ||
		(ISGROUPMODE(byOldMode) && !ISGROUPMODE(byNewMode))		
		)
    {
        m_tApplySpeakQue.ClearQueue();
        NotifyMcsApplyList( TRUE );
		m_cVCSConfStatus.SetConfSpeakMode( CONF_SPEAKMODE_NORMAL );
		m_tConf.SetConfSpeakMode( CONF_SPEAKMODE_NORMAL );
		m_cVCSConfStatus.ClearAllNewMt();
    }

	//离开发言人模式，也要清除抢答状态
	if( byOldMode == VCS_GROUPSPEAK_MODE &&
		m_cVCSConfStatus.GetConfSpeakMode() == CONF_SPEAKMODE_ANSWERINSTANTLY &&
		byOldMode != byNewMode )
	{
		m_tApplySpeakQue.ClearQueue();
        NotifyMcsApplyList( TRUE );
		m_cVCSConfStatus.SetConfSpeakMode( CONF_SPEAKMODE_NORMAL );
		m_tConf.SetConfSpeakMode( CONF_SPEAKMODE_NORMAL );		
	}
	
	// 新模式状态的初始化
	switch(byNewMode)
	{
	case VCS_SINGLE_MODE:
		break;
	case VCS_MULTW_MODE:
		break;
	case VCS_MULVMP_MODE:
	case VCS_GROUPVMP_MODE:
	//zjj20091102 新增组呼点名模式，也要起画面合成
	case VCS_GROUPROLLCALL_MODE:
		{
			//会议画面合成参数恢复
			//TConfAttrb tConfattrb = m_tConf.GetConfAttrb();
			TVMPParam_25Mem tVmpParam = GetVcsRestoreConfVmpParam();
			/*if (tConfattrb.IsHasVmpModule())
			{
				tVmpParam = GetVmpParam25MemFromModule();
				/*TVmpModule tModule = GetVmpModule();
				TVMPParam tTmpParam = tModule.GetVmpParam();
				memcpy(&tVmpParam, &tTmpParam, sizeof(TVMPParam));*
				//(TVMPParam)tVmpParam = tModule.GetVmpParam();//临时方式，需模版支持或VCS策略调整，不用模版方式开启
			}
			else
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVcsMcuVCModeChgReq] must has vmp module currently\n");
				break;						
			}*/

			// 若已启动画面合成,则修改画面合成参数至单主席一画面状态
			TPeriEqpStatus tVmpState;
			if (/*m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE &&*/
				!m_tVmpEqp.IsNull() &&
				g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpState) &&
				tVmpState.GetConfIdx() == m_byConfIdx)
			{
				if (TVmpStatus::WAIT_START== tVmpState.m_tStatus.tVmp.m_byUseState ||
					TVmpStatus::START == tVmpState.m_tStatus.tVmp.m_byUseState)
				{
					tVmpState.m_tStatus.tVmp.SetVmpParam(tVmpParam);
					g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpState);
					//m_tConf.m_tStatus.SetVmpParam(tVmpParam);
					AdjustVmpParam(m_tVmpEqp.GetEqpId(), &tVmpParam);
				}
				else
				{
					/*SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, TIMESPACE_WAIT_VMPRSP);
					tVmpState.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_START;
					g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpState);*/
					
					tVmpState.m_tStatus.tVmp.SetVmpParam(tVmpParam);
					g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpState);
					// 启动画面合成
					//m_tConf.m_tStatus.SetVmpParam(tVmpParam);
					AdjustVmpParam(m_tVmpEqp.GetEqpId(), &tVmpParam, TRUE);
				}

				// 组呼自动多画面时,主席看vmp,切到点名双画面时要拆掉vmp到主席的交换
				//组呼发言人模式，切到点名双画面也要拆发言人到主席的交换（changespeaker null对vcs会议不RestoreAllSubMtJoinedConfWatchingSrcMt）
				if ( (VCS_GROUPVMP_MODE == byOldMode || VCS_GROUPSPEAK_MODE == byOldMode )
					&& VCS_GROUPROLLCALL_MODE == byNewMode 
					&& m_cVCSConfStatus.GetChairPollState() != VCS_POLL_START )
				{
					TMt tChairMan = m_tConf.GetChairman();
					StopPrsMemberRtcp(tChairMan, MODE_VIDEO);
					StopSwitchToSubMt(1, &tChairMan, MODE_VIDEO);
				}

				//zhouyiliang 20101224 点名双画面时主席不看画面合成，切到自动多画面的时候要补一条画面合成到主席的交换
				if ( VCS_GROUPROLLCALL_MODE == byOldMode 
					&& VCS_GROUPVMP_MODE == byNewMode 
					&& tVmpParam.IsVMPBrdst() 
					&& m_cVCSConfStatus.GetChairPollState() != VCS_POLL_START )
				{
					/*u8 byVmpSubType = GetVmpSubType(m_tVmpEqp.GetEqpId());
					u8 byMaxOutChnl = MAXNUM_MPU_OUTCHNNL;
					if (MPU2_VMP_ENHACED == byVmpSubType ||
						MPU2_VMP_BASIC == byVmpSubType)//MPU2需计算后适配数
					{
						byMaxOutChnl = m_tVMPOutParam.GetVmpOutCount();
					}
					u8  bySrcChnnl = GetVmpOutChnnlByDstMtId( m_tConf.GetChairman().GetMtId() );
					if ( 0xFF == bySrcChnnl ) 
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[RestoreVCConf]get chairman:%d vmp out channl failed!\n",  m_tConf.GetChairman().GetMtId());
					}
					else if(bySrcChnnl < byMaxOutChnl)
					{
						//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
						//StartSwitchToSubMt(GetLocalVidBrdSrc(), bySrcChnnl, m_tConf.GetChairman().GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode 非默认值，是SWITCH_MODE_SELECT
						TSwitchGrp tSwitchGrp;
						tSwitchGrp.SetSrcChnl(bySrcChnnl);
						tSwitchGrp.SetDstMtNum(1);
						TMt tChairman = m_tConf.GetChairman();
						tSwitchGrp.SetDstMt(&tChairman);
						StartSwitchToAll(GetLocalVidBrdSrc(), 1, &tSwitchGrp, MODE_VIDEO, SWITCH_MODE_SELECT);
					}	*/
					SwitchNewVmpToSingleMt( m_tConf.GetChairman() );
				}
				if( VCS_GROUPROLLCALL_MODE == byNewMode && m_cVCSConfStatus.GetChairPollState() != VCS_POLL_START )
				{
					NotifyMtReceive( m_tConf.GetChairman(),m_tConf.GetChairman().GetMtId() );
				}
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[RestoreVCConf]Hasn't reserve vmp for conf(confidx:%d)\n",
					      m_byConfIdx);
			}
		}			
		break;

	case VCS_GROUPSPEAK_MODE:
	case VCS_GROUPTW_MODE:
	case VCS_GROUPCHAIRMAN_MODE:
		// 主席变为发言人
		{
			TMt tSpeaker = m_tConf.GetChairman();
			ChangeSpeaker(&tSpeaker, FALSE, FALSE);
		}
		break;
	default:
		ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[RestoreVCConf] uncorrect vcmode(%d)\n", byNewMode);
	    break;
	}
	
	m_cVCSConfStatus.SetCurVCMode(byNewMode);

	// 通告给所有的VCS会议状态的改变
	VCSConfStatusNotif();

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[RestoreVCConf] restore vcsconf for mode(%d) end\n", byNewMode);
	return;	
}

/*====================================================================
    函数名      ：SwitchToAutoMixing
    功能        ：切换至自动混音模式
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
BOOL32 CMcuVcInst::SwitchToAutoMixing()
{
	if (m_tConf.m_tStatus.IsNoMixing())
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[SwitchToAutoMixing] fail to automixing because conf mix status is %d, not in mixing\n",
			   m_tConf.m_tStatus.GetMixerMode());
		return FALSE;
	}

	m_tConf.m_tStatus.SetAutoMixing();
	TMt tMt;
	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
		{
			if (m_ptMtTable->GetMtType(byMtId) == MT_TYPE_VRSREC)
			{
				//过滤vrs新录播实体
				continue;
			}
			tMt = m_ptMtTable->GetMt(byMtId);
			AddMixMember(&tMt);
			StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
								 (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt)), 
								 MODE_AUDIO, SWITCH_MODE_SELECT);
		}
	}
	return TRUE;
}

/*====================================================================
    函数名      ：VCSMTMute
    功能        ：对指定终端进行静哑音操作
    算法实现    ：
    引用全局变量：
    输入参数说明：tMt：           操作终端
	              byMuteOpenFlag：开启(1)或关闭(0)
				  byMuteType：    静音(1)或哑音(0)
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
void CMcuVcInst::VCSMTMute(TMt tMt, u8 byMuteOpenFlag, u8 byMuteType)
{
	// 对于非本地终端，静哑音针对它所在的mcu进行操作
	if (!tMt.IsLocal())
	{
		tMt = m_ptMtTable->GetMt(GetFstMcuIdFromMcuIdx(tMt.GetMcuId()));
	}

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[VCSMTMute] TMt(mcuid:%d mtid:%d) OpenFlag:%d MuteType:%d\n", tMt.GetMcuId(), tMt.GetMtId(), byMuteOpenFlag, byMuteType);

	u8 byVendor = m_ptMtTable->GetManuId(tMt.GetMtId());
	if(byVendor == MT_MANU_KDC)
	{
		u16 wEvent;
		CServMsg cMsg;
		if(byMuteType == 1)
		{
			wEvent = MCU_MT_MTMUTE_CMD;
		}
		else
		{
			wEvent = MCU_MT_MTDUMB_CMD;
		}	
		cMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));
		cMsg.CatMsgBody(&byMuteOpenFlag, sizeof(byMuteOpenFlag));
		cMsg.SetDstMtId(tMt.GetMtId());
		SendMsgToMt(tMt.GetMtId(), wEvent, cMsg);
	}else if (byVendor == MT_MANU_KDCMCU)
	{
		TMsgHeadMsg tHeadMsg;
		memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
		tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
		
		CServMsg cMsg;
		cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );

		cMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		cMsg.CatMsgBody( (u8 *)&byMuteOpenFlag, sizeof(byMuteOpenFlag));
		cMsg.CatMsgBody( (u8 *)&byMuteType, sizeof(byMuteType));
		
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "Req to mute mcu[%d,%d]\n", tMt.GetMcuId(), tMt.GetMtId());
		
		SendMsgToMt( (u8)tMt.GetMtId(), MCU_MCU_MTAUDMUTE_REQ, cMsg );

		//调整非KEDA终端（KEDAMCU和Tandberg）静音和哑音
		//tMt = GetLocalMtFromOtherMcuMt(tMt);
        //AdjustKedaMcuAndTaideMuteInfo(&tMt, byMuteType, byMuteOpenFlag);
	}
	else
	{                    
        //调整非KEDA终端（KEDAMCU和Tandberg）静音和哑音
		tMt = GetLocalMtFromOtherMcuMt(tMt);
        AdjustKedaMcuAndTaideMuteInfo(&tMt, byMuteType, byMuteOpenFlag);
	}
                
}
/*====================================================================
    函数名      ：GetVCSAutoVmpMember
    功能        ：VCS自动画面合成，获取当前情况下进入画面合成的成员
    算法实现    ：
    引用全局变量：
    输入参数说明：[IN\OUT]ptVmpMember：各通道成员信息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/23                付秀华        创建
====================================================================*/
u8 CMcuVcInst::GetVCSAutoVmpMember(u8 byVmpId, TMt* ptVmpMember)
{
   // 获取会议最大风格
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	u8 byVmpCapChlNum = MAXNUM_SDVMP_MEMBER; // [12/21/2009 xliang] 8KE写死16
#else
    u8 byVmpCapChlNum = CMcuPfmLmt::GetMaxCapVMPByConfInfo(m_tConf);
#endif
	// 占用的vmp设备所能支持的最大风格
    TPeriEqpStatus tPeriEqpStatus;
    g_cMcuVcApp.GetPeriEqpStatus(byVmpId , &tPeriEqpStatus);
    u8 byEqpMaxChl = tPeriEqpStatus.m_tStatus.tVmp.m_byChlNum;
	// 当前最多支持进入画面合成的终端数
	u8 byMaxMemberNum  = min(byVmpCapChlNum, byEqpMaxChl); 

	//zjj20091102 广西公安新需求，组呼画面合成模式最多支持4画面
	if( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE &&
		byMaxMemberNum > 4 )
	{
		byMaxMemberNum = 4;
	}
	
	//zjj20091102 广西公安新需求，组呼画面点名模式最多支持2画面
	if( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE &&
		byMaxMemberNum >= 1 )
	{
		if( m_cVCSConfStatus.GetReqVCMT().IsNull()
			&& m_cVCSConfStatus.GetCurVCMT().IsNull() ) // 加上且当前没有正在调度的终端时，风格才定在1画面
		{
			byMaxMemberNum = 1;
		}
		else
		{
			byMaxMemberNum = 2;
		}
		
	}


	u8 byVmpStyle = GetVmpDynStyle(byVmpId, byMaxMemberNum);

// 	if (m_tVmpEqp.GetEqpId() > (VMPID_MIN + 8))
// 	{
// 		byVmpStyle = VMP_STYLE_FOUR;
// 	}

	if (VMP_STYLE_NONE == byVmpStyle)
	{
		return byVmpStyle;
	}


	TVMPParam_25Mem tVmpParam;
	tVmpParam.SetVMPStyle(byVmpStyle);
	byMaxMemberNum = tVmpParam.GetMaxMemberNum();     

	TMt atVmpMember[MAXNUM_CONF_MT];
	memset(atVmpMember, 0, MAXNUM_CONF_MT * sizeof(TMt));
	// 保证第一个通道内放主席
	u8 byChnlNum = 0;
	BOOL32 byHasChairman = HasJoinedChairman(); 
	if (byHasChairman)
	{
		atVmpMember[byChnlNum++] = m_tConf.GetChairman();
	}

	// ConfVmpParam不能用作特殊处理，通过EqpstatusVmpParam信息获得成员信息,RestoreVCConf有做特殊处理
	TVMPParam_25Mem  tLastVMPParam   = tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam();
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[GetVCSAutoVmpMember]last vmp param: \n");
	if(g_bPrintEqpMsg)
	{
		tLastVMPParam.Print();
	}

	// 保留之前设置的画面合成成员
	TMt tMt;
	TVMPMember *ptMember = NULL;  
	u8 byLastMemNum = tLastVMPParam.GetMaxMemberNum();
	for (u8 byLoop = 0; byLoop < byLastMemNum; byLoop++)
	{
		tMt.SetNull();
		ptMember = tLastVMPParam.GetVmpMember(byLoop);
		memcpy(&tMt, ptMember, sizeof(TMt));
		if (tMt.IsNull() || tMt.GetMtId() > MAXNUM_CONF_MT)
		{
			continue;
		}
		//zhouyiliang 20100824 自动画面合成的时候备份终端不进自动画面合成
		if ( m_tConf.GetChairman() == tMt || m_cVCSConfStatus.GetVcsBackupChairMan() == tMt )
		{
			continue;
		}
		// 防止高清电视墙放入画面合成中
		// 对于VCS会议定时呼叫的资源为固定资源
		/*if (CONF_CALLMODE_TIMER == VCSGetCallMode(tMt))
		{
			continue;
		}
		*/
		

		// 非回传通道中的下级终端不进画面合成成员统计
		// 因为调度终端总要放进画面合成的如果调度终端同级的其它终端在上一次的画面合成成员中,那就还占用那个通道
		if (!tMt.IsLocal()  )
		{
			if(!m_tConfAllMtInfo.MtJoinedConf(tMt))
			{
				continue;
			}

			if( !IsLocalAndSMcuSupMultSpy( tMt.GetMcuId() ) )
			{
				//zhouyiliang20101115单回传，tMt和curvcmt不是同一个终端，但他们同属一个mcu下，替换
				u8 byMtFstMcuId = GetFstMcuIdFromMcuIdx(tMt.GetMcuId());
				u8 byCurFstMcuId = GetFstMcuIdFromMcuIdx( m_cVCSConfStatus.GetCurVCMT().GetMcuId());
//				TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo( byMtFstMcuId );
//				if ( NULL != ptMcInfo &&
//					!(ptMcInfo->m_tMMcuViewMt == tMt) 
//					)
//				{
				//zhouyiliang 20101112用GetFstMcuIdFromMcuIdx来判断是否是同一mcu下的终端（可能第3级和第2级的替换）
				if( !(tMt == m_cVCSConfStatus.GetCurVCMT()) &&
					byMtFstMcuId == byCurFstMcuId 
					)
				{
					atVmpMember[byChnlNum].SetNull();
					atVmpMember[byChnlNum] = m_cVCSConfStatus.GetCurVCMT();
					byChnlNum++;
					continue;
				}					
				//}
			}			
		}
		

		if (m_tConfAllMtInfo.MtJoinedConf(tMt.GetMcuId(), tMt.GetMtId()))
		{
			atVmpMember[byChnlNum++] = tMt;
		}
	}

	//u8 byMtId    = 1;
	//u8 byMaxMtId = MAXNUM_CONF_MT;
//	u8 byChannel = 0;
	if( !ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() ) )
	{
		for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
		{
			if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
			{
				if (byMtId == m_tConf.GetChairman().GetMtId()
				 || tLastVMPParam.IsMtInMember(m_ptMtTable->GetMt(byMtId))
				 // 若终端后向通道未打开，不让白占画面合成器通道
				 || !m_ptMtTable->IsLogicChnnlOpen(byMtId, LOGCHL_VIDEO, FALSE)
				 || byMtId == m_cVCSConfStatus.GetVcsBackupChairMan().GetMtId()
				 || byMtId == m_cVCSConfStatus.GetCurVCMT().GetMtId() //zhouyiliang 20100915当前调度终端在后面加用CurUseVMPChanInd加
				 || m_ptMtTable->GetMtType(byMtId) == MT_TYPE_VRSREC //vrs新录播不占合成通道
				 )
				{
					continue;
				}				
				atVmpMember[byChnlNum++] = m_ptMtTable->GetMt(byMtId);
			}
		}

	}


	if( VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode() ||
		VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() ||
		VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode()
		)
	{
		u8	byChannel = 0;
		u8 byIsFind = 0;
		TMt tReqMt = m_cVCSConfStatus.GetReqVCMT();
		if ( VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() )
		{	
			tReqMt = m_cVCSConfStatus.GetCurVCMT();

		}
		//zhouyiliang 20100911curvcmt等到presetinack的时候再设，此时用reqvcmt
		if( !/*m_cVCSConfStatus.GetCurVCMT()*/tReqMt.IsNull() &&
			m_tConfAllMtInfo.MtJoinedConf( /*m_cVCSConfStatus.GetCurVCMT()*/tReqMt.GetMcuId(),
										/*m_cVCSConfStatus.GetCurVCMT()*/tReqMt.GetMtId() )
			)
		{
		
			for(u8 byIdx = 0; byIdx < byMaxMemberNum; byIdx++)
			{
				//ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[GetVCSAutoVmpMember] Now vmp Member mcuid:%d mtid:%d\n", 
				//		   atVmpMember[byIdx].GetMcuId(), atVmpMember[byIdx].GetMtId());
				if( atVmpMember[byIdx] == tReqMt/*m_cVCSConfStatus.GetCurVCMT()*/ )
				{
					byIsFind = 1;
					break;
				}
			}		
			
			if( 0 == byIsFind )
			{
				byChannel = (u8)m_cVCSConfStatus.GetCurUseVMPChanInd();
				if( byChannel >= byMaxMemberNum )
				{
					byChannel = 1;
				}
				atVmpMember[ byChannel ] = tReqMt/*m_cVCSConfStatus.GetCurVCMT()*/;
				++byChannel;
				++byChnlNum;
				if( byChannel >= byMaxMemberNum )
				{
					byChannel = 1;
				} //加保护,点名模式,只更新第1路通道,vpumap不够时,不支持一大一小画面,byMaxMemberNum将不是2
				else if (VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode() && byChannel > 1)
				{
					byChannel = 1;
				}
				m_cVCSConfStatus.SetCurUseVMPChanInd( byChannel );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[GetVCSAutoVmpMember] Update vcs CurUseVMPChanInd.%d:\n",byChannel);
			}

		}			
	
	}

	u8 byFinalMemNum = min(byChnlNum, byMaxMemberNum);
	if (ptVmpMember != NULL)
	{
		memset(ptVmpMember, 0, sizeof(TMt) * MAXNUM_VMP_MEMBER);
		memcpy(ptVmpMember, atVmpMember, sizeof(TMt) * byFinalMemNum);
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[GetVCSAutoVmpMember] final vcs autovmp member:\n");
		if (g_bPrintEqpMsg)
		{
			for(u8 byIdx = 0; byIdx < byFinalMemNum; byIdx++)
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "%d.mcuid:%d mtid:%d\n", 
						  byIdx, ptVmpMember[byIdx].GetMcuId(), ptVmpMember[byIdx].GetMtId());
			}
		}

	}

	byVmpStyle = GetVmpDynStyle(byVmpId, byFinalMemNum);
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[GetVCSAutoVmpMember] final autovmp style: %d\n", byVmpStyle);

	return byVmpStyle;
}

/*====================================================================
    函数名      ：SetVCSVmpMemInChanl
    功能        ：将vmp成员选入通道，建立码流交换，将剔出画面合成的终端挂断
    算法实现    ：
    引用全局变量：
    输入参数说明：[IN]pVmpMember：各通道对应成员
	              [IN]byVmpSytle: 指定的画面合成风格
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/23                付秀华        创建
====================================================================*/
void CMcuVcInst::SetVCSVmpMemInChanl(TMt* pVmpMember, u8 byVmpSytle)
{
	// 此接口未被使用，作废
	if (NULL == pVmpMember || VMP_STYLE_NONE == byVmpSytle)
	{
		return;
	}

	// 清空上一次成员
	TVMPParam  tLastVMPParam   = m_tConf.m_tStatus.GetVmpParam();

	u8         byMaxMemberNum  = tLastVMPParam.GetMaxMemberNum();
	u8         byLoop          = 0;
	for(byLoop = 0; byLoop < byMaxMemberNum; byLoop++ )
	{
		m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );			
	} 
	m_tConf.m_tStatus.SetVmpStyle(byVmpSytle);
	TVMPParam_25Mem  tNewVMPParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);

	// 将新组织的成员码流交换到对应的通道中
	for (byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++)
	{
		TMt tMt = pVmpMember[byLoop];
		if (!tMt.IsNull())
		{
			SetVmpChnnl(m_tVmpEqp.GetEqpId(), tMt, byLoop, VMP_MEMBERTYPE_VAC);
			
			tNewVMPParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
			// 调整终端的分辨率
			ChangeMtVideoFormat(tMt);
		}
		else
		{
			// 对原残留终端
			TMt tOldMt = (TMt)(*tLastVMPParam.GetVmpMember(byLoop));
			if (!tOldMt.IsNull())
			{
				StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
			}
			
		}
	}

	// 挂断被踢出画面合成器的成员
	if ( CONF_VMPMODE_NONE != tLastVMPParam.GetVMPMode() )
	{
		for( s32 nIndex = 0; nIndex < tLastVMPParam.GetMaxMemberNum(); nIndex ++ )
		{
			TMt tVMPMemberOld = (TMt)(*tLastVMPParam.GetVmpMember(nIndex));
        
			if ( !m_tConf.m_tStatus.m_tVMPParam.IsMtInMember(tVMPMemberOld) &&
				m_tConfAllMtInfo.MtJoinedConf(tVMPMemberOld.GetMcuId(), tVMPMemberOld.GetMtId())) 
			{
				// ChangeMtVideoFormat( tVMPMemberOld.GetMtId(), &m_tLastVmpParam, FALSE );
				if (!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
				{
					VCSDropMT(tVMPMemberOld);
				}
				// HD-VMP临时版，只记录下一次可用的高清替换通道(1或2通道)
				if (g_cMcuVcApp.IsDistinguishHDSDMt() &&
					m_tVmpEqp.GetEqpId() > (VMPID_MIN + 8) &&
					m_tVmpEqp.GetEqpId() <= VMPID_MAX)
				{
					if (nIndex > 0 && nIndex < 3)
					{
						m_cVCSConfStatus.SetCurUseVMPChanInd(nIndex + 1);
					}
				}
				else
				{
					m_cVCSConfStatus.SetCurUseVMPChanInd(nIndex + 1);
				}
			}
		}             
	}

}

/*====================================================================
    函数名      ：ProcVCSChairOffLine
    功能        ：本地主席终端不在线给所有界面提示并且恢复会议状态
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/23                付秀华        创建
====================================================================*/
void CMcuVcInst::VCSChairOffLineProc()
{
	if ( VCS_CONF == m_tConf.GetConfSource() && 
		m_byCreateBy != CONF_CREATE_MT )
	{
		//zhouyiliang 20100818 vcs 切换本机地址（本机地址1+1备份）
		BOOL32 bChange = VCSChangeChairMan();
		if ( !bChange &&  !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
		{
			// 将会议切换至单方调度状态
			RestoreVCConf(VCS_SINGLE_MODE);	

			CServMsg cServMsg;
			cServMsg.SetConfIdx(m_byConfIdx);
			cServMsg.SetConfId(m_tConf.GetConfId()) ;
			cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
			
			SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cServMsg);
			return;
		}
	}

}

/*====================================================================
    函数名      ：GetCallMode
    功能        ：获得指定实体的呼叫类型
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt：实体信息
    返回值说明  ：CONF_CALLMODE_TIMER(不可调度资源)/CONF_CALLMODE_NONE(可调度资源)
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/23                付秀华        创建
====================================================================*/
u8 CMcuVcInst::VCSGetCallMode(TMt tMt)
{
	if (tMt.IsNull())
	{
		// 无效终端，视为不可调度资源
		return CONF_CALLMODE_TIMER;
	}

	if (tMt.IsLocal())
	{
		return m_ptMtTable->GetCallMode(tMt.GetMtId());
	}
	else
	{
		TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMcuId());
		if (ptMcInfo != NULL)
		{
			for(s32 nLoop = 0; nLoop < MAXNUM_CONF_MT; nLoop++)
			{
				if(ptMcInfo->m_atMtExt[nLoop].GetMtId() == tMt.GetMtId())
				{
					return ptMcInfo->m_atMtExt[nLoop].GetCallMode();
				}
			}
		}
	}

	return CONF_CALLMODE_TIMER;
}

/*====================================================================
    函数名      ：ChangeVFormat
    功能        ：改变指定终端的视频分辨率
    算法实现    ：
    引用全局变量：
    输入参数说明：tMt：指定终端
	              byNewFormat: 指定改变后的格式
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/23                付秀华        创建
====================================================================*/
void CMcuVcInst::ChangeVFormat(TMt tMt, u8 byStart, u8 byNewFormat, u8 byEqpId, u8 byVmpStyle, u8 byPos, u32 dwResW, u32 dwResH, u8 byHduChnId)
{

    if (tMt.IsNull() || tMt.GetType() == TYPE_MCUPERI)
    {
        ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[ChangeHDVFormat] dstmt.%d is not mt or kdc mt. type.%d\n",
                tMt.GetMtId(), tMt.GetType());
        return;
    }

	// [10/18/2011 liuxu] mcu终端的分辨率不需要进行调整
	// 转而调整其真实的实体上传终端
	if (IsMcu(tMt) || !tMt.IsLocal())
	{
		CascadeAdjMtRes(tMt, byNewFormat, byStart, byVmpStyle, byPos, dwResW, dwResH, byEqpId, byHduChnId);
		return;
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ChangeVFormat] tMt.IsLocal() tMt(%d, %d) videoformat change to %d\n",
			    tMt.GetMcuId(), tMt.GetMtId(), byNewFormat);

    // 对于高清终端放入标清电视墙,采取降分辨率
    TLogicalChannel tSrcRvsChannl;
    if ( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
    {
		u8 byChnnlType   = tSrcRvsChannl.GetMediaType();
		u8 byChnnlFormat;
		if (byStart) 
		{
			byChnnlFormat = byNewFormat;
		}else
		{
			byChnnlFormat = tSrcRvsChannl.GetVideoFormat();
		}

		// [pengjie 2010/9/13] 多回传调分辨率逻辑调整
        SendChgMtVidFormat( tMt.GetMtId(), byChnnlType, byChnnlFormat, FALSE, byStart, dwResW, dwResH);

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF,  "[ChangeVFormat] mt(%d) videoformat change to %d\n",
			     tMt.GetMtId(), byChnnlFormat);

    }
    return;
}

/*====================================================================
    函数名      ：VCSMTAbilityNotif
    功能        ：指定终端能力不足通告
    算法实现    ：
    引用全局变量：
    输入参数说明：tMt：指定终端
	              byMediaType: 不足的类型
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/23                付秀华        创建
====================================================================*/
void CMcuVcInst::VCSMTAbilityNotif(TMt tMt, u8 byMediaType)
{
	//对于VCS会议, 需提示该终端无法接收图像
	if (VCS_CONF == m_tConf.GetConfSource())
	{
		CServMsg  cMsg;
		cMsg.SetConfId(m_tConf.GetConfId());
		cMsg.SetConfIdx(m_byConfIdx);
		cMsg.SetErrorCode(0);
		if (tMt == m_cVCSConfStatus.GetCurVCMT())
		{
			if (MODE_VIDEO == byMediaType)
			{
				cMsg.SetErrorCode(ERR_MCU_VCS_REMNOACCEPTVIDEO);
			}
			else if (MODE_AUDIO == byMediaType)
			{
				cMsg.SetErrorCode(ERR_MCU_VCS_REMNOACCEPTAUDIO);
			}
		}
		else if (tMt == m_tConf.GetChairman())
		{
			if (MODE_VIDEO == byMediaType)
			{
				cMsg.SetErrorCode(ERR_MCU_VCS_LOCNOACCEPTVIDEO);
			}
			else if (MODE_AUDIO == byMediaType)
			{
				cMsg.SetErrorCode(ERR_MCU_VCS_LOCNOACCEPTAUDIO);
			}
		}

		if (cMsg.GetErrorCode() != 0)
		{
			SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cMsg);
		}
	}	

}
/*====================================================================
    函数名      ：ChangeSpecMixMember
    功能        ：修改定制混音参数
    算法实现    ：
    引用全局变量：
    输入参数说明：ptMixMember:指向定制混音成员的指针
	              byMemberNum:定制混音成员数
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/04/14                付秀华        创建
====================================================================*/
void CMcuVcInst::ChangeSpecMixMember(TMt* ptMixMember, u8 byMemberNum)
{
	if(!m_tConf.m_tStatus.IsMixing())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ChangeSpecMixMember]mix has not started\n");
		return;
	}

	TMt tMt;
	u8  byInMixMem = FALSE;
	u8 byMtIdx = 0;
	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
		{
			continue;
		}

		byInMixMem = FALSE;
		for (byMtIdx = 0; byMtIdx < byMemberNum; byMtIdx++)
		{
			if ( GetLocalMtFromOtherMcuMt( ptMixMember[byMtIdx] ).GetMtId() == byMtId )
			{
				byInMixMem = TRUE;
				break;
			}
		}

		//如果vcs会议已经起电视墙一键混音，在电视墙内的终端也得在混音成员内,就不能退出混音组
		if( !byInMixMem && m_tConf.GetConfSource() == VCS_CONF &&
			m_cVCSConfStatus.GetMtInTvWallCanMixing() &&
			( m_ptMtTable->IsMtInHdu( byMtId ) || m_ptMtTable->IsMtInTvWall( byMtId ) )
			)
		{
			byInMixMem = TRUE;
		}


		tMt = m_ptMtTable->GetMt(byMtId);
		if (m_ptMtTable->IsMtInMixGrp(byMtId) && !byInMixMem && m_ptMtTable->GetMtType(byMtId) == MT_TYPE_MT)
		{
			RemoveSpecMixMember(&tMt, 1, FALSE,FALSE);
		}
		
		if (!m_ptMtTable->IsMtInMixGrp(byMtId) && byInMixMem)
		{
			AddSpecMixMember(&tMt, 1, TRUE);
		}

		for (byMtIdx = 0; byMtIdx < byMemberNum; byMtIdx++)
		{
			if( byInMixMem && !ptMixMember[byMtIdx].IsLocal() )
			{			
				AddSpecMixMember(&ptMixMember[byMtIdx], 1, TRUE);
			}
		}
	}

}

/*====================================================================
    函数名      ：VCSConfStatusNotif
    功能        ：通知所有VCS会议状态
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/04/14                付秀华        创建
====================================================================*/
void CMcuVcInst::VCSConfStatusNotif()
{
	CServMsg cServMsg;
	cServMsg.SetConfId(m_tConf.GetConfId());
	cServMsg.SetConfIdx(m_byConfIdx);
	cServMsg.SetEventId(MCU_VCS_CONFSTATUS_NOTIF);
	cServMsg.SetMsgBody( (u8*)&m_cVCSConfStatus, sizeof(CBasicVCCStatus) );
	SendMsgToAllMcs(MCU_VCS_CONFSTATUS_NOTIF, cServMsg);
}

/*====================================================================
    函数名      ：VCSGetNextPollMt
    功能        ：获取下一个轮询终端
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/04/14                付秀华        创建
====================================================================*/
/*lint -save -e850*/
TMt CMcuVcInst::VCSGetNextPollMt()
{
	u8  byMtId = 0;
	TMt tCurPollMt = m_cVCSConfStatus.GetCurChairPollMt();
	if (!tCurPollMt.IsNull())
	{
		byMtId = tCurPollMt.GetMtId();
	}
	byMtId++;

	if (byMtId > MAXNUM_CONF_MT)
	{
		byMtId = 1;
	}

	u8 byLoopNum = 0;
	u8  byStartMtId   = byMtId;
	u8  byLoopRestart = FALSE; 
	TMt tNextPollMt;
	tNextPollMt.SetNull();
	TMtStatus tMtStatus;
	BOOL32 bLoop = TRUE;
	while ( bLoop ) 
	{
		for (; byMtId <= MAXNUM_CONF_MT; byMtId++)
		{
			if (byStartMtId == byMtId && byLoopRestart)
			{
				break;
			}

			//zjj 20090910		
			if( m_ptMtTable->GetMainType( byMtId ) != TYPE_MT )
			{
				continue;
			}

			//zjj 20090910		
			if( MT_TYPE_MMCU == m_ptMtTable->GetMtType( byMtId ) || 
				MT_TYPE_SMCU == m_ptMtTable->GetMtType( byMtId ) ||
				MT_TYPE_VRSREC == m_ptMtTable->GetMtType( byMtId ) //过滤vrs新录播
				)
			{
				continue;
			}		
			
            // miaoqingsong [20110616] 增加条件判断：VCS会议主席轮询时过滤掉备份终端
			if (m_tConfAllMtInfo.MtJoinedConf((byMtId)) && 
				m_tConf.GetChairman().GetMtId() != byMtId && 
				m_cVCSConfStatus.GetVcsBackupChairMan().GetMtId() != byMtId//&& 
				//m_ptMtTable->GetCallMode(byMtId) != CONF_CALLMODE_TIMER
				)
			{				
				if( m_ptMtTable->GetMtStatus(byMtId, &tMtStatus) &&
					(tMtStatus.IsVideoLose() || MT_MANU_CHAORAN == m_ptMtTable->GetManuId(byMtId)) )
				{
					ConfPrint( LOG_LVL_WARNING, MID_MCU_VCS, "[VCSGetNextPollMt] Mt.%d Is Lose Video,jump it\n",byMtId );
					continue;	
				}
				tNextPollMt = m_ptMtTable->GetMt(byMtId);
				break;
			}

			if (MAXNUM_CONF_MT == byMtId)
			{
				byMtId = 0;
				byLoopRestart = TRUE;
			}
	

		}

		if( !tNextPollMt.IsNull() )
		{
			break;
		}
		else if ( byMtId > MAXNUM_CONF_MT && byLoopNum < 1) 
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[VCSGetNextPollMt] byMtId > MAXNUM_CONF_MT\n" );
			byMtId = 1;
			byLoopNum++;
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[VCSGetNextPollMt] Get Next Poll Mt error !\n");
			break;
		}
		
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[VCSGetNextPollMt] Next Poll Mt(%d,%d) mainType.%d mtType.%d!\n",
		tNextPollMt.GetMcuId(),tNextPollMt.GetMtId(),m_ptMtTable->GetMainType(tNextPollMt.GetMtId()),
		m_ptMtTable->GetMtType( tNextPollMt.GetMtId() ));
	
	return tNextPollMt;
}
/*lint -restore*/

/*====================================================================
    函数名      ：IsNeedHdSelApt
    功能        ：判断两终端选看是否需要选看适配
    算法实现    ：
    引用全局变量：
    输入参数说明：bySwitchMode:MODE_VIDEO, MODE_AUDIO
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/04/14                付秀华        创建
====================================================================*/
BOOL CMcuVcInst::IsNeedSelApt(u8 bySrcMtId, u8 byDstMtId, u8 bySwitchMode)
{
	if (bySwitchMode != MODE_AUDIO && bySwitchMode != MODE_VIDEO)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsNeedHdSelApt] wrong input param about bySwitchMode(%d)\n", bySwitchMode);
		return FALSE;
	}

	// 视频选看
    if(MODE_VIDEO == bySwitchMode)
    {
		//取源终端与目的终端的能力集
		TLogicalChannel tSrcLogChnl;
		TLogicalChannel tDstLogChnl;
		m_ptMtTable->GetMtLogicChnnl(bySrcMtId, LOGCHL_VIDEO, &tSrcLogChnl, FALSE);
		m_ptMtTable->GetMtLogicChnnl(byDstMtId, LOGCHL_VIDEO, &tDstLogChnl, TRUE);

		//对于POLOCOM终端进行选看，支持不进行分辨率判断，直接建交换
		if (g_cMcuVcApp.IsVidAdjustless4Polycom() && MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byDstMtId))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsNeedHdSelApt] polocom no need to use bas\n");
			return FALSE;
		}
		// 若格式不一致
		else if (tSrcLogChnl.GetChannelType() != tDstLogChnl.GetChannelType())
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsNeedHdSelApt] src type.%d matchless with dst type.%d, ocuupy new adp!\n",
				    tSrcLogChnl.GetChannelType(), tDstLogChnl.GetChannelType());
            return TRUE;
		}
		else if (IsResG(tSrcLogChnl.GetVideoFormat(), tDstLogChnl.GetVideoFormat()))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsNeedHdSelApt] src res.%d matchless with dst res.%d, ocuupy new adp!\n",
				    tSrcLogChnl.GetVideoFormat(), tDstLogChnl.GetVideoFormat());
			return TRUE;
		}
		// zjj20090929此时源的目的终端未发送码流，码率为0，取呼叫码率粗略判断
		else if (m_ptMtTable->GetDialBitrate(bySrcMtId) > m_ptMtTable->GetDialBitrate(byDstMtId) &&
			     VCS_CONF == m_tConf.GetConfSource())
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsNeedHdSelApt] src bitrate.%d large than dst bitrate.%d!\n", 
				    m_ptMtTable->GetDialBitrate(bySrcMtId), m_ptMtTable->GetDialBitrate(byDstMtId));
			return TRUE;
		}
	}

	// 音频选看
	if (MODE_AUDIO == bySwitchMode)
	{
		//取源终端与目的终端的能力集
		TLogicalChannel tSrcLogChnl;
		TLogicalChannel tDstLogChnl;
		m_ptMtTable->GetMtLogicChnnl(bySrcMtId, LOGCHL_AUDIO, &tSrcLogChnl, FALSE);
		m_ptMtTable->GetMtLogicChnnl(byDstMtId, LOGCHL_AUDIO, &tDstLogChnl, TRUE);

		if (tSrcLogChnl.GetChannelType() != tDstLogChnl.GetChannelType())
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[IsNeedHdSelApt] src audio type.%d matchless with dst audio type.%d, ocuupy new adp!\n",
				    tSrcLogChnl.GetChannelType(), tDstLogChnl.GetChannelType());
			return TRUE;
		}
	}
	return FALSE;
}

/*====================================================================
    函数名      ：ProcMMCUGetConfCtrl
    功能        ：由上级MCU接管会议控制,恢复到发言人模式,呼叫所有终端
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/04/14                付秀华        创建
====================================================================*/
void CMcuVcInst::ProcMMCUGetConfCtrlMsg()
{
	// 恢复下级会议到单方调度模式,并修改会议创建方式,禁止下级调度
	RestoreVCConf(VCS_SINGLE_MODE);
	m_byCreateBy = CONF_CREATE_MT;

	u16 wErrCode;
	if (!PrepareAllNeedBasForConf(&wErrCode))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcMMCUGetConfCtrlMsg]not enough bas\n");
	}

	ReLoadOrigMtList();

	CServMsg cServMsg;
	TMt tCallMT;
	
	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (m_tConfAllMtInfo.MtInConf(byMtId) &&
			!m_tConfAllMtInfo.MtJoinedConf(byMtId)&&
			MT_TYPE_MMCU != m_ptMtTable->GetMtType( byMtId )
			)
		{   
			tCallMT = m_ptMtTable->GetMt(byMtId);
			InviteUnjoinedMt(cServMsg, &tCallMT);
		}	
	}
}

/*====================================================================
    函数名      ：ReLoadOrigMtList
    功能        ：重新加载会议最初列表
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/04/14                付秀华        创建
====================================================================*/
void CMcuVcInst::ReLoadOrigMtList( BOOL32 bIsAdd /*= TRUE*/ )
{
	for (u8 byMtLoop = 1; byMtLoop <= MAXNUM_CONF_MT; byMtLoop++)
	{
		if (m_tConf.GetChairman().GetMtId() != byMtLoop &&
			m_ptMtTable->GetMt(byMtLoop).GetMtId() != 0 &&
			MT_TYPE_MMCU != m_ptMtTable->GetMtType( byMtLoop )
			&& m_cVCSConfStatus.GetVcsBackupChairMan().GetMtId() != byMtLoop )
		{
            TMt tMt = m_ptMtTable->GetMt(byMtLoop); 
			RemoveMt(tMt, FALSE,FALSE );
		}			
	}

	CServMsg cServMsg;	
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	if( bIsAdd )
	{
		TMtAlias* patMtAlias = NULL;
		u16* pawMtDialRate = NULL;
		u8 byMtNum = m_tOrginMtList.GetMtNum();
		for (u8 byMtIdx = 0; byMtIdx < byMtNum; byMtIdx++)
		{
			AddMt(m_tOrginMtList.GetMtAliasByIdx(byMtIdx), 
				  m_tOrginMtList.GetMtDialRateByIdx(byMtIdx),
				  CONF_CALLMODE_NONE);
		}
	}

	VCSConfStatusNotif();

	//发给会控会议所有终端信息	
	cServMsg.SetServMsg( NULL,0 );
	cServMsg.SetMsgBody( NULL,0 );
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//发给会控终端表 
	SendMtListToMcs(LOCAL_MCUIDX);
}

/*====================================================================
    函数名      ：NotifyVCSPackInfo
    功能        ：通告所VCS指定调度席的分组信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/04/14                付秀华        创建
====================================================================*/
void CMcuVcInst::NotifyVCSPackInfo(CConfId& cConfId, u8 byVcsId)
{
	if (cConfId.GetConfSource() != VCS_CONF)
	{
		return;
	}

	u8 byConfPos = MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE;
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	GetAllConfHeadFromFile(acConfId, sizeof(acConfId));
	for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
	{
		if (acConfId[nPos] == cConfId)
		{
			byConfPos = (u8)nPos;
			break;
		}
	}

	if (byConfPos < (MAXNUM_MCU_CONF + MAXNUM_MCU_TEMPLATE))
	{
		CServMsg cServMsg;
		cServMsg.SetConfId(cConfId);

		u32 dwTotalLen = GetUnProConfDataToFileLen(byConfPos);
		u8  byPackNum = (u8)(dwTotalLen / MAX_VCSPACKIFNOLEN + ((dwTotalLen % MAX_VCSPACKIFNOLEN) ? 1 : 0));
		cServMsg.SetTotalPktNum(byPackNum);

		u32 dwOutLen   = 0;
		u32 dwBeginPos = 0;
		for (u8 byPackIdx = 0; byPackIdx < byPackNum; byPackIdx++)
		{
			dwOutLen = MAX_VCSPACKIFNOLEN;
			cServMsg.SetCurPktIdx(byPackIdx);
			cServMsg.SetMsgBody();
			GetUnProConfDataToFile(byConfPos, (s8*)cServMsg.GetMsgBody(), dwOutLen, dwBeginPos);
			dwBeginPos += dwOutLen;
			cServMsg.SetMsgBodyLen((u16)dwOutLen);
			if (byVcsId)
			{
				SendMsgToMcs(byVcsId, MCU_VCS_PACKINFO_NOTIFY, cServMsg);
			}
			else
			{
				SendMsgToAllMcs(MCU_VCS_PACKINFO_NOTIFY, cServMsg);
			}
		}
	}
}

/*====================================================================
    函数名      HandleVCSAutoVmpSpyRepalce
    功能        ：处理vcs自动画面合成多回传带宽已满时，替换presetin带下去的可释放终端逻辑
    算法实现    ：
    引用全局变量：
    输入参数说明：const tPreSetInRsp, TPreSetInRsp类型
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
   2010-7-19                zhouyiliang     modify
====================================================================*/
void CMcuVcInst::HandleVCSAutoVmpSpyRepalce(TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();

	TMultiCacMtInfo tMtInfo;
	tMtInfo.m_byCasLevel = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCasLevel;
	memcpy( &tMtInfo.m_abyMtIdentify[0],
		&tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_abyMtIdentify[0],
		sizeof(tMtInfo.m_abyMtIdentify)
		);
	TMt tReleaseMt = GetMtFromMultiCascadeMtInfo( tMtInfo,
					tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt
					);

	if ( tPreSetInRsp.m_tSetInReqInfo.GetEvId() != MCS_MCU_STARTVMP_REQ 
		|| tSrc.IsNull() || tReleaseMt.IsNull() ) //不是自动画面合成发的presetin
	{
		return;
	}
	
	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	BOOL32 bIsVcsAuto = m_tConf.GetConfSource() == VCS_CONF && tConfVmpParam.IsVMPAuto();
	//zhouyiliang 20100813 替换可释放终端，m_byIsNeedRelease再保护一次
	if ( bIsVcsAuto && tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsNeedRelease )
	{
		u8 byReplaceVmpPos = 1;
		//查找可替换终端在vmpparam中的通道号,如果不在return
		if ( !tConfVmpParam.FindVmpMember( tReleaseMt, byReplaceVmpPos ) ) 
		{
			return;
		}
	
		//如果可替换的不是presetinreq要求的那个通道,要恢复，否则就会少一个成员或者style不对		 
		if (tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos != byReplaceVmpPos )
		{
				TVMPParam_25Mem tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(m_tVmpEqp);
				TVMPMember tOldMember = *tLastVmpParam.GetVmpMember(/*1*/tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos);
				tConfVmpParam.SetVmpMember( tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos,tOldMember );
				u16 byCurUseChnId = m_cVCSConfStatus.GetCurUseVMPChanInd();
				u8 byMaxVmpMember = tConfVmpParam.GetMaxMemberNum();
				//上次本来要放置的位置为空，这次要加在这个位置，回传带宽又满了，所以不能放这个位置，style应该恢复上次的
				if ( tOldMember.IsNull() )
				{
					u8 byStyl = tLastVmpParam.GetVMPStyle();
					tConfVmpParam.SetVMPStyle( byStyl );
					
				}	
				g_cMcuVcApp.SetConfVmpParam(m_tVmpEqp, tConfVmpParam);
				//恢复presetin之前的curUseVmpChanInd，如本来要变4画面，通道满，只能3画面，curUseVmpChan就不再是1，而是原来的3了
				byCurUseChnId = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos;
				m_cVCSConfStatus.SetCurUseVMPChanInd( byCurUseChnId );
		}

		tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos = byReplaceVmpPos;
	}
}

/*====================================================================
    函数名      ：FindVCSCanReplaceSpyMt
    功能        ：找一个能够替换的mt，并找到它所在多回传通道和在外设的通道号
    算法实现    ：
    引用全局变量：
    输入参数说明：
	              【out】swCanReplaceSpyIndex 可替换的多回传通道号
				  【out】byCanReplaceChnnPos 可替换的通道号（mt所在的vmp通道或者hdu通道号等）
    返回值说明  ：TMt  返回可替换的mt，IsNull表示没找到可替换mt
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
   2010-7-19                zhouyiliang     modify
   2010-08-31               周翼亮          modify
====================================================================*/
TMt CMcuVcInst::FindVCSCanReplaceSpyMt( u8 byMtInEqpType , const TMt& tSrc ,s16& swCanReplaceSpyIndex, u8& byCanReplaceChnnPos )
{
	// 此接口不再被使用，作废
	swCanReplaceSpyIndex = -1;
	TMt tCanReplaceMt;
	tCanReplaceMt.SetNull();
	if (m_tConf.GetConfSource() != VCS_CONF ) //如果不是vcs会议
	{
		return tCanReplaceMt;
	}

	switch( byMtInEqpType )
	{
	case EQP_TYPE_VMP:
		{
			if ( !m_tConf.m_tStatus.m_tVMPParam.IsVMPAuto() )//vmp 自动画面合成替换查找,自动画面合成才有替换策略
			{
				return tCanReplaceMt;
			}

			//zhoyiliang 20100716 找到最先进入画面合成的本级（同一下级）终端，替换。
			//zhouyiliang 20100831 替换策略改变原来替换同一下级终端改为，替换最先进入画面合成的那个mt
			u8 byMaxVmpMember = m_tConf.m_tStatus.m_tVMPParam.GetMaxMemberNum();
		
			//找到同级mcu里第一个画面合成成员的替换优先级为0 的（除主席外）位置
			//zhouyiliang 20100831 替换策略改变原来替换同一下级终端改为，替换最先进入画面合成的那个mt
			u8 byLowestPrioIndex = 1;
			for ( u8 byLoop = 1 ; byLoop < byMaxVmpMember ; byLoop++ )
			{
				TVMPMember tTempMember = *m_tConf.m_tStatus.m_tVMPParam.GetVmpMember( byLoop );
				//u8 byReplacePrio = m_cVCSConfStatus.GetVmpMemberReplacePrio(byLoop);
				if (tTempMember.IsNull()) //为空的跳过
				{
					continue;
				}
				TMt tTempMt = (TMt)(tTempMember);
				if ( /*tTempMt->GetMcuId() == tSrc.GetMcuId()
					&& tTempMt->GetMtId() != tSrc.GetMtId()*/ 
					!( tTempMt == tSrc ))
					//&& byReplacePrio == 0  ) 
				{
					byLowestPrioIndex = byLoop;
					break;
				}
			}

			//先找byLowestPrioIndex以后的可替换终端（最先进vmpparam的同级终端），如果没有再找之前的
			//zhouyiliang 20100831 替换策略改变原来替换同一下级终端改为，替换最先进入画面合成的那个mt
			BOOL32 bFind = FALSE;
			for ( u8 byLoopHigh = byLowestPrioIndex + 1 ; byLoopHigh < byMaxVmpMember ; byLoopHigh++ )
			{
				TVMPMember tTempMember = *m_tConf.m_tStatus.m_tVMPParam.GetVmpMember( byLoopHigh );
				if (tTempMember.IsNull()) //为空的跳过
				{
					continue;
				}
				TMt tTempMt = (TMt)(tTempMember);
				//zhouyiliang 20100831 替换策略改变原来替换同一下级终端改为，替换最先进入画面合成的那个mt
				if ( /* tTempMt->GetMcuId() == tSrc.GetMcuId()
					&& tTempMt->GetMtId() != tSrc.GetMtId()*/
					!( tTempMt == tSrc ) ) 
				{
					swCanReplaceSpyIndex = m_cSMcuSpyMana.FindSpyMt(tTempMt);
						
					//要替换的vmpmember不在回传通道，找下一个
					if (-1 == swCanReplaceSpyIndex ) 
					{
						 continue;
					}
					//找到了，但是如果在电视墙一样不能做为可替换终端
					if(	IsMtNotInOtherHduChnnl( tTempMt,0,0 ) &&
							IsMtNotInOtherTvWallChnnl( tTempMt,0,0 ) )
					{
						 //新加入的vmpmember的位置。
						byCanReplaceChnnPos = byLoopHigh;
						bFind = TRUE;
						break;
					}				  
				}
			}

			//如果没有再找之前的,第0个是主席，不用比较
			for ( u8 byLoopLow = 1 ; byLoopLow < byLowestPrioIndex && !bFind; byLoopLow++ )
			{
				TVMPMember tTempMember = *m_tConf.m_tStatus.m_tVMPParam.GetVmpMember( byLoopLow );
				if (tTempMember.IsNull()) //为空的跳过
				{
					continue;
				}
				TMt tTempMt = (TMt)(tTempMember);
				//zhouyiliang 20100831 替换策略改变原来替换同一下级终端改为，替换最先进入画面合成的那个mt
				if ( /*tTempMt->GetMcuId() == tSrc.GetMcuId()
					&& tTempMt->GetMtId() != tSrc.GetMtId()*/
					!( tTempMt == tSrc ) ) 
				{
					swCanReplaceSpyIndex = m_cSMcuSpyMana.FindSpyMt(tTempMt);
					//要替换的vmpmember不在回传通道，找下一个
					if (-1 == swCanReplaceSpyIndex) 
					{
						 continue;
					}
					//找到了，但是如果在电视墙一样不能做为可替换终端
					if(	IsMtNotInOtherHduChnnl( tTempMt,0,0 ) &&
							IsMtNotInOtherTvWallChnnl( tTempMt,0,0 ) )
					{
					   //新加入的vmpmember的位置。
					   byCanReplaceChnnPos = byLoopLow;
						bFind = TRUE;
						break;
					}
				}
			}

			//如果0的前后都没有优先级为1的，就替换0本身
			if (FALSE == bFind) 
			{
				TVMPMember tTempLowestMember = *m_tConf.m_tStatus.m_tVMPParam.GetVmpMember( byLowestPrioIndex );
				if ( !tTempLowestMember.IsNull()) //为空的跳过
				{
					 TMt tTempMt = (TMt)(tTempLowestMember);
					 //zhouyiliang 20100831 替换策略改变原来替换同一下级终端改为，替换最先进入画面合成的那个mt
					if ( /*tTempMt->GetMcuId() == tSrc.GetMcuId()
							&& tTempMt->GetMtId() != tSrc.GetMtId() */
							!( tTempMt == tSrc ) ) 
					{
						swCanReplaceSpyIndex = m_cSMcuSpyMana.FindSpyMt(tTempMt);
						//要替换的vmpmember在回传通道，找到了，但是如果在电视墙一样不能做为可替换终端
						if (-1 != swCanReplaceSpyIndex  && IsMtNotInOtherHduChnnl( tTempMt,0,0 ) &&
							IsMtNotInOtherTvWallChnnl( tTempMt,0,0 ) ) 
						{
							//新加入的vmpmember的位置。
							byCanReplaceChnnPos = byLowestPrioIndex;
							bFind = TRUE;
						}
					}
				}
			}

			//可替换mt
			if (-1 != swCanReplaceSpyIndex && bFind ) 
			{
				TVMPParam tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
				tCanReplaceMt = (TMt)(*tVmpParam.GetVmpMember( byCanReplaceChnnPos ));
			}
			
		}
		break;

	case EQP_TYPE_HDU:
		break;

	default:
		break;
	}

	return tCanReplaceMt;
}

/*====================================================================
    函数名      ：VCSChangeChairMan
    功能        ：vcs本机地址1+1备份，切换掉线的主席地址为备份主席地址，各模式进行业务恢复
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：TMt  返回可替换的mt，IsNull表示没找到可替换mt
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
   2010-8-18                zhouyiliang     create
====================================================================*/
BOOL32 CMcuVcInst::VCSChangeChairMan( ) 
{
	BOOL32 bRet = FALSE;
	if ( m_tConf.GetConfSource() != VCS_CONF ) //保护
	{
		return FALSE;
	}
	//调用changechairman
	TMt tBackupChairman = m_cVCSConfStatus.GetVcsBackupChairMan();
	//备份终端在线
	if ( !tBackupChairman.IsNull() && m_tConfAllMtInfo.MtJoinedConf( tBackupChairman ) ) 
	{
		TMt toldChairMan = m_tConf.GetChairman();
		TMt tCurMt = m_cVCSConfStatus.GetCurVCMT();
		//	各模式进行业务恢复
		u8 byMode = m_cVCSConfStatus.GetCurVCMode();
		VCSMTMute(tBackupChairman, m_cVCSConfStatus.IsLocMute(), VCS_AUDPROC_MUTE);
		VCSMTMute(tBackupChairman, m_cVCSConfStatus.IsLocSilence(), VCS_AUDPROC_SILENCE);

		ChangeChairman( &tBackupChairman );
		m_cVCSConfStatus.SetVcsBackupChairMan( toldChairMan );


		switch (byMode)
		{
		case VCS_SINGLE_MODE:
			{
				//单方调度模式：新主席成功与当前调度终端进行两两选看
				TSwitchInfo tSwitchInfo;
				
				//新主席选看当前调度终端
				if( !tCurMt.IsNull() )
				{
					VCSConfStopMTSel( toldChairMan, MODE_BOTH );
									
					tSwitchInfo.SetSrcMt( tCurMt );
					tSwitchInfo.SetDstMt( tBackupChairman );
					tSwitchInfo.SetMode(MODE_BOTH);
					VCSConfSelMT(tSwitchInfo);
				}
				
				//调度在线终端选看新主席
				for ( u8 byLoopMt = 1 ; byLoopMt < MAXNUM_CONF_MT ; byLoopMt++ )
				{
					TMt tMt = m_ptMtTable->GetMt(byLoopMt);
					if ( tMt.IsNull() || tMt == tBackupChairman || tMt == toldChairMan ) 
					{
						continue;
					}
					if (!m_tConfAllMtInfo.MtJoinedConf(tMt) ) 
					{
						continue;
					}
					VCSConfStopMTSel( tMt, MODE_BOTH );	
					tSwitchInfo.SetSrcMt( tBackupChairman );
					tSwitchInfo.SetDstMt(  tMt );
					tSwitchInfo.SetMode(MODE_BOTH);
					VCSConfSelMT(tSwitchInfo);
				}
				break;
			}

		case VCS_GROUPROLLCALL_MODE:
		case VCS_GROUPVMP_MODE:
		case VCS_MULVMP_MODE:
			{
				//多方多画面、点名双画面、自动画面合成:新主席进画面合成及混音器
				//点名双画面，新主席选看当前调度终端
				if ( VCS_GROUPROLLCALL_MODE == byMode ) 
				{
					VCSConfStopMTSel( toldChairMan, MODE_VIDEO );	
					TSwitchInfo tSwitchInfo;
					tSwitchInfo.SetSrcMt( tCurMt );
					tSwitchInfo.SetDstMt( tBackupChairman );
					tSwitchInfo.SetMode( MODE_VIDEO );
					VCSConfSelMT(tSwitchInfo);
				}
				TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
				if ( tConfVmpParam.IsMtInMember( toldChairMan ) )//保护
				{
					//新主席进画面合成
					TVMPMember* ptChairmanMemeber = tConfVmpParam.GetVmpMember(0);//取主席所在的那个vmpmemeber
					ptChairmanMemeber->SetMemberTMt( tBackupChairman );
					g_cMcuVcApp.SetConfVmpParam(m_tVmpEqp, tConfVmpParam);
					AdjustVmpParam(m_tVmpEqp.GetEqpId(), &tConfVmpParam ,FALSE,FALSE);//主席肯定是本地终端，不需再发presetin
				}
				break;
			}

		case VCS_GROUPSPEAK_MODE:
			{
				//发言人模式:无发言人均看新主席,新主席进混音
				if ( tCurMt.IsNull() ) 
				{	
					ChangeSpeaker(&tBackupChairman);
				}
				break;
			}
		
		case VCS_GROUPCHAIRMAN_MODE:
			{
				ChangeSpeaker(&tBackupChairman);
				
				//新主席选看当前调度终端
				if ( !tCurMt.IsNull() ) 
				{
					VCSConfStopMTSel( toldChairMan, MODE_VIDEO );
					TSwitchInfo tSwitchInfo;
					tSwitchInfo.SetSrcMt( tCurMt );
					tSwitchInfo.SetDstMt( tBackupChairman );
					tSwitchInfo.SetMode(MODE_VIDEO);
					VCSConfSelMT(tSwitchInfo);
				}
				break;
			}
		default:
			break;
		}
		
		//zhouyiliang 20101209 新主席进混音,老主席出混音
		if ( m_tConf.m_tStatus.IsMixing() && !m_ptMtTable->IsMtInMixGrp( tBackupChairman.GetMtId() ) ) 
		{
			AddSpecMixMember(&tBackupChairman,1,TRUE);
			if (m_ptMtTable->IsMtInMixGrp( toldChairMan.GetMtId() )) 
			{
				RemoveSpecMixMember( &toldChairMan, 1, FALSE );
			}			
		}

	

		bRet = TRUE;
	}else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "backup chairman is off-line\n");
	}

	if (bRet)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "changechairman ok\n");
	}else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "changechairman failed\n");
	}

	return bRet;
}

/*====================================================================
    函数名      ：ProcVcsMcuStopAllMonCmd
    功能        ：关闭监控联动
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    11/03/18    4.6         薛亮          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuStopAllMonCmd(const CMessage *pcMsg)
{
	if( NULL== pcMsg )
	{
		return ;
	}
	
	STATECHECK;

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	u8 byMcsId = cServMsg.GetSrcSsnId();
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVcsMcuStopAllMonCmd] byMcsId is %d\n", byMcsId);

	StopSwitchToMonitor(byMcsId, TRUE);
}

/*====================================================================
    函数名      ：ProcVcsMcuStopMonReq
    功能        ：关闭监控联动
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26    4.6         薛亮          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuStopMonReq(const CMessage *pcMsg )
{
	if( NULL== pcMsg )
	{
		return ;
	}
	
	STATECHECK;
    
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	TSwitchDstInfo tSwitchDstInfo = *(TSwitchDstInfo*) ( cServMsg.GetMsgBody() );
	
	TSwitchDstInfo tTempSwitchInfo;
	memcpy(&tTempSwitchInfo, &tSwitchDstInfo, sizeof(TSwitchDstInfo));
	
	//[2011/11/10/zhangli]不能依赖mcs侧保存的数据tSwitchDstInfo.m_tSrcMt来停。监控下级终端时，先监控mt1成功，
	//再监控mt2只有audio成功，仍然监控mt1的video，停监控时，tSwitchDstInfo.m_tSrcMt=mt2，导致mt1的视频带宽无法释放
	//StopSwitchToMonitor(tSwitchDstInfo, cServMsg.GetSrcSsnId());
	
	TMt tOldSrc;
	if (MODE_BOTH == tSwitchDstInfo.m_byMode || MODE_VIDEO == tSwitchDstInfo.m_byMode)
	{
		if(g_cMcuVcApp.GetMonitorSrc(cServMsg.GetSrcSsnId(), MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, &tOldSrc))
		{
			if(!tOldSrc.IsNull())
			{
				tTempSwitchInfo.m_tSrcMt = tOldSrc;
				tTempSwitchInfo.m_byMode = MODE_AUDIO;
				StopSwitchToMonitor(tTempSwitchInfo, cServMsg.GetSrcSsnId());
			}
		}
	}
	
	if (MODE_BOTH == tSwitchDstInfo.m_byMode || MODE_AUDIO == tSwitchDstInfo.m_byMode)
	{
		tOldSrc.SetNull();
		if(g_cMcuVcApp.GetMonitorSrc(cServMsg.GetSrcSsnId(), MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, &tOldSrc))
		{
			if(!tOldSrc.IsNull())
			{
				tTempSwitchInfo.m_tSrcMt = tOldSrc;
				tTempSwitchInfo.m_byMode = MODE_VIDEO;
				StopSwitchToMonitor(tTempSwitchInfo, cServMsg.GetSrcSsnId());
			}
		}
	}

	// ack
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
}

/*====================================================================
    函数名      ：ProcVcsMcuStartMonReq
    功能        ：开始监控联动
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/10/26    4.6         薛亮          创建
====================================================================*/
void CMcuVcInst::ProcVcsMcuStartMonReq( const CMessage *pcMsg )
{
	if( NULL== pcMsg )
	{
		return ;
	}

	STATECHECK;
    
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	//action abort when no chairman online
	TMt tChairMan = m_tConf.GetChairman();
	if( ( tChairMan.IsNull()  || 
		!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) ) &&
		m_byCreateBy != CONF_CREATE_MT
		)
	{
		cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
		cServMsg.SetEventId(cServMsg.GetEventId() + 2);
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}

	CServMsg	cMsg( pcMsg->content, pcMsg->length );
	TSwitchDstInfo tSwitchDstInfo = *(TSwitchDstInfo*) ( cServMsg.GetMsgBody() );

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVcsMcuStartMonReq] param is: mode: %u, srcChannel: %u, VidIp: 0x%x, VidPort: %d\n\tAudIp:0x%x, AudPort:%d, srcmt(%d,%d)\n", 
				tSwitchDstInfo.m_byMode, 
				tSwitchDstInfo.m_bySrcChnnl, 
				tSwitchDstInfo.m_tDstVidAddr.GetIpAddr(),
				tSwitchDstInfo.m_tDstVidAddr.GetPort(),
				tSwitchDstInfo.m_tDstAudAddr.GetIpAddr(),
				tSwitchDstInfo.m_tDstAudAddr.GetPort(),
				tSwitchDstInfo.m_tSrcMt.GetMcuId(),
				tSwitchDstInfo.m_tSrcMt.GetMtId()
			);

	//tSwitchDstInfo 参数整理和校验
	TMt tSrcMt = tSwitchDstInfo.m_tSrcMt;
	u8 byMode = tSwitchDstInfo.m_byMode;
	u32 dwDstIp = tSwitchDstInfo.m_tDstVidAddr.GetIpAddr();
	
	if( MODE_NONE == byMode || 0 == dwDstIp )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "tSwitchDstInfo param is wrong( bymode:%u, DstIp:0x%X ) \n", byMode, dwDstIp);
		return;	
	}

	/*if (m_tConf.GetConfAttrb().IsSatDCastMode()
		&& IsMultiCastMt(tSrcMt.GetMtId()))
	{
		if (IsSatMtOverConfDCastNum(tSrcMt))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcMtMcuStartSwitchMtReq] over max upload mt num. nack!\n");
			cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
    }*/
	
	
	// 给对端回ack
    SendReplyBack( cMsg, cMsg.GetEventId() + 1 );

	u8 byMcsId = cServMsg.GetSrcSsnId();

	// [12/23/2010 xliang] if UI ask the same Src in the same Monitor channel, MCU should do nothing.
	TMt tMcAudCurSrc;
	TMt tMcVidCurSrc;
	g_cMcuVcApp.GetMonitorSrc(byMcsId, MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, &tMcVidCurSrc);
	g_cMcuVcApp.GetMonitorSrc(byMcsId, MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, &tMcAudCurSrc);
	if( ( byMode == MODE_BOTH && tMcVidCurSrc == tSrcMt && tMcAudCurSrc == tSrcMt )
		|| ( byMode == MODE_VIDEO && tMcVidCurSrc == tSrcMt )
		|| ( byMode == MODE_AUDIO && tMcAudCurSrc == tSrcMt )
		)
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "repeated mt(%d,%d), mode: %u is in the monitor channel, mcu do nothing\n",
			tSrcMt.GetMcuId(), tSrcMt.GetMtId(), byMode);
		return;
	}

	// [1/27/2011 xliang] filter mode
	if( byMode == MODE_BOTH && tMcVidCurSrc == tSrcMt && (!(tMcAudCurSrc == tSrcMt)) )
	{
		byMode = MODE_AUDIO;
	}
	else if( byMode == MODE_BOTH && (!(tMcVidCurSrc == tSrcMt))  && tMcAudCurSrc == tSrcMt)
	{
		byMode = MODE_VIDEO;
	}
	tSwitchDstInfo.m_byMode = byMode;

	//多回传
	if( !tSrcMt.IsLocal() )
	{
		if( IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()) )
		{
			TPreSetInReq tSpySrcInitInfo;
			tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt(tSrcMt);
			tSpySrcInitInfo.m_bySpyMode = byMode;
			tSpySrcInitInfo.SetEvId(cServMsg.GetEventId());
			
			TMcsRegInfo tMcRegInfo;
			if (g_cMcuVcApp.GetMcsRegInfo(byMcsId, &tMcRegInfo))
			{
				tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchDstInfo.SetMcIp(tMcRegInfo.GetMcsIpAddr());
				tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchDstInfo.SetMcSSRC(tMcRegInfo.GetMcsSSRC());
			}
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcVcsMcuStartMonReq] VcsIp.%s McSSrc.%x!\n", 
										 StrOfIP(tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchDstInfo.GetMcIp()),
										 tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchDstInfo.GetMcSSRC());
		
			tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchDstInfo.m_bySrcChnnl  = tSwitchDstInfo.m_bySrcChnnl;
			tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchDstInfo.m_tDstVidAddr = tSwitchDstInfo.m_tDstVidAddr;
			tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchDstInfo.m_tDstAudAddr = tSwitchDstInfo.m_tDstAudAddr;

// 			tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_stChlIdx = tSwitchInfo.GetDstChlIdx();
// 			tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_bySrcChlIdx = tSwitchInfo.GetSrcChlIdx();
// 			tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_byMcInstId  = cServMsg.GetSrcSsnId();
			
			// [pengjie 2010/9/13] 填目的端能力
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()) );
			//zjl20101116 如果当前终端已回传则能力集要与已回传目的能力集取小
			if(!GetMinSpyDstCapSet(tSrcMt, tSimCapSet))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcVcsMcuStartMonReq] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
					tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
				return;
			}
			
			tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		    // End

			// 将上次在这个通道的终端设成可替换终端
			TMt tMcSrc;
			u8 byReleaseMode = MODE_NONE;
			if( g_cMcuVcApp.GetMonitorSrc(byMcsId, MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, &tMcSrc) )
			{
				if( !tMcSrc.IsNull() )
				{
					if( tMcSrc == tSrcMt )
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcVcsMcuStartMonReq] GetMcSrc == NewMt, MODE_VIDEO\n" );
					}
					else
					{
						byReleaseMode += MODE_VIDEO;
					}
				}
			}
			
			if( g_cMcuVcApp.GetMonitorSrc(byMcsId, MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, &tMcSrc) )
			{
				if( !tMcSrc.IsNull() )
				{
					if( tMcSrc == tSrcMt )
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcVcsMcuStartMonReq] GetMcSrc == NewMt, MODE_AUDIO !\n" );
					}
					else
					{
						byReleaseMode += MODE_AUDIO;
					}
				}
			}
			
			if (MODE_NONE != byReleaseMode  ) 
			{
				tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode = byReleaseMode;
				if( MODE_BOTH == tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode )
				{
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
				}
				else if( MODE_VIDEO == tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode )
				{
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
				}
				else if( MODE_AUDIO == tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode )
				{
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
				}
				//zhouyiliang 20120424 监控联动的优先级比调度低，不认为是可替换画面合成
// 				if ( m_tConf.GetConfSource() == VCS_CONF && m_tConf.m_tStatus.m_tVMPParam.IsVMPAuto() )
// 				{
// 					++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
// 				}
				
				tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMcSrc;								
			}
			OnMMcuPreSetIn( tSpySrcInitInfo );
			return;
		}
		else	//单回传逻辑
		{
			//当下级有当前调度终端，且同一个下级的其它终端被选看，回NACK
			if( m_tConf.GetConfSource() == VCS_CONF && !m_cVCSConfStatus.GetCurVCMT().IsNull()
				&&  IsMtInMcu(GetLocalMtFromOtherMcuMt(tSrcMt), m_cVCSConfStatus.GetCurVCMT()) 
				&& !( m_cVCSConfStatus.GetCurVCMT() == tSrcMt ))
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVcsMcuMtInTwMsg] CurVCMT(%d.%d) and Mt(%d.%d) is in same smcu,Mt can't be selected \n ",
					m_cVCSConfStatus.GetCurVCMT().GetMcuId(),
					m_cVCSConfStatus.GetCurVCMT().GetMtId(),
					tSrcMt.GetMcuId(),
					tSrcMt.GetMtId() );
				
				cServMsg.SetErrorCode( ERR_MCU_VCS_NOUSABLEBACKCHNNL );
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
			//查看MC
			TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcMt.GetMcuId());
			if(ptMcInfo == NULL)
			{
				cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
				SendReplyBack( cMsg, cMsg.GetEventId() + 2 );
				return;
			}
			//构造选看源
			TMt tMt;
			tMt.SetMcuIdx(tSrcMt.GetMcuId());
			tMt.SetMtId(0);
			TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
			if(ptStatus == NULL)
			{                    
				cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
				SendReplyBack( cMsg, cMsg.GetEventId() + 2 );
				return;
			}
			
			OnMMcuSetIn( tSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
		}
	}
	
	//建交换，做记录
	u16 wMcInstId = cServMsg.GetSrcSsnId();
	StartSwitchToMonitor(tSwitchDstInfo, wMcInstId);

// 	if( !ProcStartMonitorOpr(tSwitchDstInfo, wErrorCode) )
// 	{
// 		cServMsg.SetErrorCode( wErrorCode );
// 		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
// 	}

	return;
}

/*=============================================================================
函 数 名： StartSwitchToMonitor
功    能： 监控交换
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/08/15   4.6		薛亮                  创建
=============================================================================*/
BOOL32 CMcuVcInst::StartSwitchToMonitor(TSwitchDstInfo &tSwitchDstInfo, u16 wMcInstId)
{
	if( !g_cMcuVcApp.IsMcConnected( wMcInstId ) )
	{
		return FALSE;
	}

	TMt tSrcMt = tSwitchDstInfo.m_tSrcMt;
	u8	byMode = tSwitchDstInfo.m_byMode;
	u8  bySrcChnnl = tSwitchDstInfo.m_bySrcChnnl;


	TMt tSrc = GetLocalMtFromOtherMcuMt( tSrcMt );
	u8 bySwitchMode = byMode;
	//检测非KDC终端是否处于哑音状态，是则不交换音频码流	
	if( TYPE_MT == m_ptMtTable->GetMainType( tSrc.GetMtId() ) && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( tSrc.GetMtId() ) && 
		m_ptMtTable->IsMtAudioDumb( tSrc.GetMtId() ) )
	{
		if( byMode == MODE_AUDIO )
		{
			//return TRUE;
			bySwitchMode = MODE_NONE;
		}
		else if ( byMode == MODE_BOTH )
		{
			//byMode = MODE_VIDEO;
			bySwitchMode = MODE_VIDEO;
		}
	}

	//级联多回传支持
	TMt tMcSrc;
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH ) // 音频
	{
		if( g_cMcuVcApp.GetMonitorSrc(wMcInstId, MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, &tMcSrc) )
		{
			if( !tMcSrc.IsNull() )
			{
				if( tMcSrc == tSrcMt )
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "StartSwitchToMonitor GetMcSrc == NewMt(%d,%d), MODE_AUDIO\n",
						tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
				}
				else
				{
					FreeRecvSpy( tMcSrc, MODE_AUDIO);
					g_cMcuVcApp.SetMonitorSrc( wMcInstId, MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, tSrcMt );
				}
			}
		}
		else
		{
			g_cMcuVcApp.SetMonitorSrc( wMcInstId, MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, tSrcMt );
		}
	}
	
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH ) // 视频
	{
		if( g_cMcuVcApp.GetMonitorSrc(wMcInstId, MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, &tMcSrc) )
		{
			if( !tMcSrc.IsNull() )
			{
				if( tMcSrc == tSrcMt )
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "StartSwitchToMonitor GetMcSrc == NewMt(%d,%d), MODE_VIDEO\n",
						tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
				}
				else
				{
					FreeRecvSpy( tMcSrc, MODE_VIDEO);
					g_cMcuVcApp.SetMonitorSrc( wMcInstId, MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, tSrcMt );
				}
			}
		}
		else
		{
			g_cMcuVcApp.SetMonitorSrc( wMcInstId, MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, tSrcMt );
		}
	}



	CRecvSpy tSpyResource;
	u16 wSpyPort = SPY_CHANNL_NULL;	
	if( m_cSMcuSpyMana.GetRecvSpy( tSrcMt, tSpyResource ) )
	{
		wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
	}


	//建交换
	tSwitchDstInfo.m_byMode = bySwitchMode;
	if( !g_cMpManager.StartSwitchToDst(tSwitchDstInfo, wSpyPort) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VCS, "StartSwitchToDst() failed! Cannot switch to specified eqp!\n" );
		return FALSE;
	}

	

	if( !tSrcMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuIdx()) )
	{
		TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));
		SendMMcuSpyNotify( tSrcMt, VCS_MCU_START_MONITOR_UNION_REQ, tSimCapSet);
	}

	//通知src开始发送码流编关键帧
	NotifySrcSend(tSrcMt, byMode, bySrcChnnl, TRUE);
	
	//发送notif给界面
	tSwitchDstInfo.m_byMode = byMode;
	NotifyUIStartMonitor(tSwitchDstInfo, wMcInstId);

	return TRUE;

}

/*=============================================================================
函 数 名： NotifyUIStartMonitor
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/08/15   4.6		薛亮                  创建
=============================================================================*/
void CMcuVcInst::NotifyUIStartMonitor(TSwitchDstInfo &tSwitchDstInfo, u16 wMcInstId)
{

	TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
	TDoublePayload tDVPayload;
	TDoublePayload tDAPayload;
	TMt tSrcMt = tSwitchDstInfo.m_tSrcMt;
	TMt tLocalMt = GetLocalMtFromOtherMcuMt(tSrcMt);

	if( tSrcMt.GetType() == TYPE_MT )
	{
		TSimCapSet    tSrcSCS  = m_ptMtTable->GetSrcSCS( tLocalMt.GetMtId() ); 
		
		if( MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() || 
			MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() || 
			CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() )
		{
			tDVPayload.SetRealPayLoad( tSrcSCS.GetVideoMediaType() );
			tDVPayload.SetActivePayload( GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType() ) );
		}
		else
		{
			tDVPayload.SetRealPayLoad( tSrcSCS.GetVideoMediaType() );
			tDVPayload.SetActivePayload( tSrcSCS.GetVideoMediaType() );
		}
		if( CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() )
		{
			tDAPayload.SetRealPayLoad( tSrcSCS.GetAudioMediaType() );
			tDAPayload.SetActivePayload( GetActivePayload(m_tConf, tSrcSCS.GetAudioMediaType() ) );
		}
		else
		{
			tDAPayload.SetRealPayLoad( tSrcSCS.GetAudioMediaType() );
			tDAPayload.SetActivePayload( tSrcSCS.GetAudioMediaType() );
		}
	}
	else if( tSrcMt.GetType() == TYPE_MCUPERI )
	{
		//FIXME: 扩展应用：可根据不同外设的某一输出取不同的payload 
		tDVPayload.SetRealPayLoad(m_tConf.GetMainVideoMediaType());
		tDVPayload.SetActivePayload(m_tConf.GetMainVideoMediaType());
	}
	else
	{
		// do nothing
	}

	TLogicalChannel tAudLog;
	m_ptMtTable->GetMtLogicChnnl( tLocalMt.GetMtId(),LOGCHL_AUDIO,&tAudLog,FALSE );
	
	CServMsg cMsg;
	cMsg.SetMsgBody( (u8 *)&tSwitchDstInfo, sizeof(tSwitchDstInfo));
	cMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
	cMsg.CatMsgBody( (u8 *)&tDVPayload, sizeof(tDVPayload));
	cMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
	cMsg.CatMsgBody( (u8 *)&tDAPayload, sizeof(tDAPayload));
	
	// [8/17/2010 xliang] 下面特例能否去掉
	// zw [06/26/2008] 适应AAC LC格式
	TAudAACCap tAudAACCap;
	if ( MEDIA_TYPE_AACLC == m_tConf.GetMainAudioMediaType() )
	{
		tAudAACCap.SetMediaType(MEDIA_TYPE_AACLC);
		tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
		//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
		tAudAACCap.SetChnlType(GetAACChnlTypeByAudioTrackNum(tAudLog.GetAudioTrackNum()));
		tAudAACCap.SetBitrate(96);
		tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
		
		cMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
	}
	//tianzhiyong 2010/04/02 适应AAC LD格式
	else if ( MEDIA_TYPE_AACLD == m_tConf.GetMainAudioMediaType() )
	{
		tAudAACCap.SetMediaType(MEDIA_TYPE_AACLD);
		tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
		//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
		tAudAACCap.SetChnlType(GetAACChnlTypeByAudioTrackNum(tAudLog.GetAudioTrackNum()));
		tAudAACCap.SetBitrate(96);
		tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
		
		cMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[NotifyUIStartMonitor] m_tConf.GetMainAudioMediaType() does'nt equals 103 or 104 !\n");
		cMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
	}

	//支持UI监控图像丢包重传 // [8/28/2010 xliang] FIXME: 目前只支持终端进监控的丢包重传
	// 直接告知界面
	//[liu lijiu][2010/09/25]增加双流以及多回传情况下的丢包重传
	TTransportAddr  tVidAddr;
	TTransportAddr  tAudAddr;
	tVidAddr.SetNull();
	tAudAddr.SetNull();
	u8 byMode = tSwitchDstInfo.m_byMode;


	//[2011/11/03/zhangli]用统一接口获取，原写法不支持单回传
	if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
	{
		if (!GetRemoteRtcpAddr(tSrcMt, tSwitchDstInfo.m_bySrcChnnl, MODE_AUDIO, tAudAddr))
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[NotifyUIStartMonitor]GetRemoteRtcpAddr failed\n");
		}
	}
	
	if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
	{
		if (!GetRemoteRtcpAddr(tSrcMt, tSwitchDstInfo.m_bySrcChnnl, MODE_VIDEO, tVidAddr))
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[NotifyUIStartMonitor]GetRemoteRtcpAddr failed\n");
		}
	}

	//监控的双流时
	if (byMode == MODE_SECVIDEO)
	{
		TLogicalChannel tEqpLogicalChannel;
		u8 byChlNum = 0;
		u8 byPrsId = 0;
		u8 byPrsChnId = 0;
		if (FindPrsChnForBrd(MODE_SECVIDEO, byPrsId, byPrsChnId) &&
			g_cMcuVcApp.GetPeriEqpLogicChnnl(byPrsId, MODE_VIDEO, &byChlNum, &tEqpLogicalChannel, TRUE))
		{
			tVidAddr.SetIpAddr(tEqpLogicalChannel.GetRcvMediaCtrlChannel().GetIpAddr());
			tVidAddr.SetPort(tEqpLogicalChannel.GetRcvMediaCtrlChannel().GetPort() + PORTSPAN * byPrsChnId + 2);
		}	
	}

	//视频RTCP通道
	cMsg.CatMsgBody( (u8 *)&tVidAddr, sizeof(tVidAddr) );	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[NotifyUIStartMonitor]tell ui monitor video rtcp port(%s, %d)\n", 
		    StrOfIP(tVidAddr.GetIpAddr()), tVidAddr.GetPort());

	//音频RTCP通道
	cMsg.CatMsgBody( (u8 *)&tAudAddr, sizeof(tAudAddr) );	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[NotifyUIStartMonitor]tell ui monitor audio rtcp port(%s, %d)\n", 
		        StrOfIP(tAudAddr.GetIpAddr()), tAudAddr.GetPort());
	
	SendMsgToMcs(  (u8)wMcInstId, MCU_VCS_START_MONITOR_NOTIF, cMsg );
}


/*=============================================================================
函 数 名： AdjustSwitchToMonitorWatchingSrc
功    能： 调整对应界面的在用监控的交换
算法实现： 
全局变量： 
参    数： TMt		&tSrc		[i] 
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/09/10   4.6		薛亮                  创建
=============================================================================*/
void CMcuVcInst::AdjustSwitchToMonitorWatchingSrc(const TMt &tSrc,u8 byMode /*= MODE_NONE*/,BOOL32 bIsStop /*= TRUE*/,BOOL32 bIsOnlySwitch /*= FALSE*/)
{
	TSwitchDstInfo tSwitchDstInfo;
	TMonitorData   tMonitorData;
	u8 byIndex;

	
	
	for( byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( !g_cMcuVcApp.IsMcConnected( byIndex ) )
		{
			continue;
		}

		for( u16 wLoop = 0; wLoop < MAXNUM_MONITOR_NUM; wLoop ++ )
		{
			if( !g_cMcuVcApp.GetMonitorData(byIndex, wLoop, tMonitorData) )
			{
				continue;
			}

			memset( &tSwitchDstInfo,0,sizeof(tSwitchDstInfo) );
			tSwitchDstInfo.m_tSrcMt = tMonitorData.GetMonitorSrc();
			if( tSwitchDstInfo.m_tSrcMt == tSrc || ( IsMcu(tSrc) && IsMtInMcu(tSrc, tSwitchDstInfo.m_tSrcMt) ) )
			{
				tSwitchDstInfo.m_byMode = tMonitorData.GetMode();
				if( MODE_NONE != byMode )
				{
					if(tSwitchDstInfo.m_byMode == byMode )
					{
						if( MODE_BOTH == byMode || MODE_VIDEO == byMode )
						{
							tSwitchDstInfo.m_tDstVidAddr = tMonitorData.GetDstAddr();		
						}
						if( MODE_BOTH == byMode || MODE_AUDIO == byMode )
						{
							tSwitchDstInfo.m_tDstAudAddr = tMonitorData.GetDstAddr();		
						}
									
					
						if( !bIsStop )
						{
							StartSwitchToMonitor( tSwitchDstInfo,byIndex );
						}
						else
						{
							StopSwitchToMonitor(tSwitchDstInfo, byIndex, bIsOnlySwitch);
						}
					}
				}
				else
				{
					if(tSwitchDstInfo.m_byMode == MODE_VIDEO )
					{
						tSwitchDstInfo.m_tDstVidAddr = tMonitorData.GetDstAddr();
					}
					else
					{
						tSwitchDstInfo.m_tDstAudAddr = tMonitorData.GetDstAddr();
					}
					if( !bIsStop )
					{
						StartSwitchToMonitor( tSwitchDstInfo,byIndex );
					}
					else
					{
						StopSwitchToMonitor(tSwitchDstInfo, byIndex, bIsOnlySwitch);
					}
				}
				
				
				
			}
		}
	}
	return ;
}

/*=============================================================================
函 数 名： StopSwitchToMonitor
功    能： 停止对应界面的在用监控的交换
算法实现： 
全局变量： 
参    数： u16		wMcInstId	[i]
		   BOOL32	bConf		[i] 是否仅停该会议对应的监控联动
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/08/15   4.6		薛亮                  创建
=============================================================================*/
void CMcuVcInst::StopSwitchToAllMonitor(BOOL32 bConf /*= FALSE*/ )
{
	u8 byIndex;
	for( byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( !g_cMcuVcApp.IsMcConnected( byIndex ) )
		{
			continue;
		}

		StopSwitchToMonitor(byIndex, bConf);
	}

	return ;
}

/*=============================================================================
函 数 名： StopSwitchToMonitor
功    能： 停止对应界面的在用监控的交换
算法实现： 
全局变量： 
参    数： u16		wMcInstId	[i]
		   BOOL32	bConf		[i] 是否仅停该会议对应的监控联动
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/08/15   4.6		薛亮                  创建
=============================================================================*/
BOOL32 CMcuVcInst::StopSwitchToMonitor(u16 wMcInstId, BOOL32 bConf /*= FALSE*/ )
{
	if( wMcInstId == 0 || wMcInstId > MAXNUM_MCU_MC + MAXNUM_MCU_VC )
	{
		return FALSE;
	}
	
	TSwitchDstInfo tSwitchDstInfo;
	TMonitorData   tMonitorData;
	for( u16 wLoop = 0; wLoop < MAXNUM_MONITOR_NUM; wLoop ++ )
	{
		if( !g_cMcuVcApp.GetMonitorData(wMcInstId, wLoop, tMonitorData) )
		{
			continue;
		}
		
		tSwitchDstInfo.m_tSrcMt = tMonitorData.GetMonitorSrc();
		if( bConf && tSwitchDstInfo.m_tSrcMt.GetConfIdx() != m_byConfIdx )
		{
			continue;
		}

		tSwitchDstInfo.m_byMode = tMonitorData.GetMode();
		if(tSwitchDstInfo.m_byMode == MODE_VIDEO )
		{
			tSwitchDstInfo.m_tDstVidAddr = tMonitorData.GetDstAddr();
		}
		else
		{
			tSwitchDstInfo.m_tDstAudAddr = tMonitorData.GetDstAddr();
		}
		
		StopSwitchToMonitor(tSwitchDstInfo, wMcInstId);			
	}

	return TRUE;
}

/*=============================================================================
函 数 名： StopSwitchToMonitor
功    能： 停止对应界面对应接收地址的监控交换
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/08/15   4.6		薛亮                  创建
=============================================================================*/
BOOL32 CMcuVcInst::StopSwitchToMonitor(TSwitchDstInfo &tSwitchDstInfo, u16 wMcInstId, BOOL32 bIsOnlySwitch/* = FALSE*/)
{
	//1,stop switch
	g_cMpManager.StopSwitchToDst(tSwitchDstInfo);

	if( bIsOnlySwitch )
	{
		return TRUE;
	}

	//2,多回传资源清理及monitor table整理
	TMt tSrcMt = tSwitchDstInfo.m_tSrcMt;
	u8  byMode = tSwitchDstInfo.m_byMode;
	TMt tMtNull;
	tMtNull.SetNull();

	if( byMode == MODE_AUDIO || byMode == MODE_BOTH ) // 停音频
	{
		if( !tSrcMt.IsNull() && !tSrcMt.IsLocal())
		{
			FreeRecvSpy( tSrcMt, MODE_AUDIO);
		}

		g_cMcuVcApp.SetMonitorSrc( wMcInstId, MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, tMtNull );
	}
	
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH ) // 停视频
	{
		if( !tSrcMt.IsNull() && !tSrcMt.IsLocal())
		{
			FreeRecvSpy( tSrcMt, MODE_VIDEO);
		}

		g_cMcuVcApp.SetMonitorSrc( wMcInstId, MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, tMtNull );
	}

	return TRUE;
}

/*=============================================================================
函 数 名： GetVcsHduAndTwModuleChnNum
功    能： 获得当前VCS会议配置的电视墙模板通道数
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/10/29   4.6		田志勇                 创建
2011/05/25   4.6		刘  旭                 修改
                                             将TMultiTvWallModule寻找通道数目的逻辑进行了修改,
											 并移到了TMultiTvWallModule内部
=============================================================================*/
u8 CMcuVcInst::GetVcsHduAndTwModuleChnNum()
{
	u8 byTwAndHduChnNum = 0;
	BOOL32 bVcsConf =  ( VCS_CONF == m_tConf.GetConfSource()  ) ? TRUE : FALSE;
	byTwAndHduChnNum += m_tConfEqpModule.m_tHduModule.GetHduChnlNum( bVcsConf ); 
	
	TMultiTvWallModule tMultiTvWallModule;
	m_tConfEqpModule.GetMultiTvWallModule( tMultiTvWallModule );
	byTwAndHduChnNum += tMultiTvWallModule.GetChnnlNum( bVcsConf );	

	return byTwAndHduChnNum;
}

/*=============================================================================
函 数 名： SetMtInTvWallAndHduInFailPresetin
功    能： 当Presetin失败时在预览电视墙模式下设置终端入电视墙
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/11/24   4.6		田志勇                 创建
2011/05/25   4.6	    刘旭				   修改
=============================================================================*/
void CMcuVcInst::SetMtInTvWallAndHduInFailPresetinAndInReviewMode( TPreSetInRsp &tPreSetInRsp )
{
	if( VCS_CONF != m_tConf.GetConfSource() || m_cVCSConfStatus.GetTVWallManageMode() != VCS_TVWALLMANAGE_REVIEW_MODE )
	{
		return;
	}

	const TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	TPeriEqpStatus tStatus;
	
	// hdu消息
	if ( MCS_MCU_START_SWITCH_HDU_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() &&
		tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo.m_bySrcMtType == TW_MEMBERTYPE_VCSSPEC)
	{
		// 提取hdu设备和通道
		const u8 byDstChnnlIdx = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo.m_byDstChlIdx;
		const TEqp tHduEqp = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo.m_tHdu;

		if ( !g_cMcuVcApp.GetPeriEqpStatus( tHduEqp.GetEqpId(), &tStatus ))
		{
			return;
		}

		tStatus.m_tStatus.tHdu.atVideoMt[byDstChnnlIdx].SetMt(tSrc);
		tStatus.m_tStatus.tHdu.atVideoMt[byDstChnnlIdx].byMemberType = TW_MEMBERTYPE_VCSSPEC;
		g_cMcuVcApp.SetPeriEqpStatus( tHduEqp.GetEqpId(), &tStatus );
	}

	// tvwall设备消息
	if (MCS_MCU_START_SWITCH_TW_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() &&
		tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_bySrcMtType == TW_MEMBERTYPE_VCSSPEC )
	{
		// 提取hdu设备和通道
		const u8 byDstChnnlIdx = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_byDstChlIdx;
		const TEqp tTvwEqp = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_tTvWall;

		if ( !g_cMcuVcApp.GetPeriEqpStatus( tTvwEqp.GetEqpId(), &tStatus ))
		{
			return;
		}

		tStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx].SetMt( tSrc );
		tStatus.m_tStatus.tTvWall.atVideoMt[byDstChnnlIdx].byMemberType = TW_MEMBERTYPE_VCSSPEC;
		g_cMcuVcApp.SetPeriEqpStatus( tTvwEqp.GetEqpId(), &tStatus );
	}

	// 上报状态
	CServMsg cServMsgSend;
	cServMsgSend.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
	SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsgSend);	
}


/*=============================================================================
函 数 名： ShowCfgExtraData
功    能： 打印vcs预案基本信息
算法实现： 
全局变量： 
参    数： 
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/11/14   4.6		周嘉麟                 创建
=============================================================================*/
void CMcuVcInst::ShowCfgExtraData()
{
	StaticLog("VCS Scheme Num. %d\n", m_cCfgInfoRileExtraDataMgr.GetMtPlanNum());
	StaticLog("VCS Scheme IsRead. %d\n", m_cCfgInfoRileExtraDataMgr.IsReadPlan());
	StaticLog("VCS DataLen. %d\n", m_cCfgInfoRileExtraDataMgr.GetMSDataLen());
	
	u8 idx = 0;
	TMtVcsPlanNames tVcsPlanNames = {0};	
	s8 *pachPlanName[ VCS_MAXNUM_PLAN ];
	for( idx = 0;idx < VCS_MAXNUM_PLAN;idx++ )
	{
		pachPlanName[idx] = &tVcsPlanNames.m_achAlias[idx][0];
	}
	u8 byPlanNum = 0;
	m_cCfgInfoRileExtraDataMgr.GetAllPlanName( (s8**)pachPlanName,byPlanNum);

	for ( idx = 0;idx < VCS_MAXNUM_PLAN;idx++ )
	{
		StaticLog("VCS PlanName. %s  \n", pachPlanName[idx]);
	}
}
