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
#include "eqpssn.h"
#include "mcuerrcode.h"
#include "mtadpssn.h"
#include "mcuutility.h"
#include "mpmanager.h"
#include "radiusinterface.h"

#if defined(_VXWORKS_)
#include <inetLib.h>
#include "brddrvlib.h"
#elif defined(_LINUX_)
#include "boardwrapper.h"
#endif

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
    CServMsg cMtMsg;

    CServMsg  cServMsg( pcMsg->content, pcMsg->length );

    TConfInfo tTmpConf = *(TConfInfo *)cServMsg.GetMsgBody();
    BOOL32 bConfFromFile = tTmpConf.m_tStatus.IsTakeFromFile();
    tTmpConf.m_tStatus.SetTakeFromFile(FALSE);

	// MCS VCS会议互斥
// 	if (g_cMcuVcApp.GetConfNum(TRUE, TRUE, FALSE, MCS_CONF) > 0)
// 	{
// 		cServMsg.SetErrorCode(ERR_MCU_VCSMCS_ONETYPECONF);
// 		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
// 		ConfLog(FALSE, "[ProcMcsMcuCreateConfReq]Only one type conf at the same time\n");
// 		return;
// 	}

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
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq]Conference %s failure because exceed max conf num!\n", tTmpConf.GetConfName() );
				return;
			}

			if (0 == g_cMcuVcApp.GetMpNum() || 0 == g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323))
			{
				cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq]Conference %s failure because No_Mp or No_MtAdp!\n", tTmpConf.GetConfName() );
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
					ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] Conference %s failure for conf full!\n", tTmpConf.GetConfName() );
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
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] MVidType.%d with FPS.%d is unexpected, adjust it\n",
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
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] SVidType.%d with FPS.%d is unexpected, adjust it\n",
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
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] DSVidType.%d with FPS.%d is unexpected, adjust it\n",
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
         
			   // VCS通过模板创会沿用会议模板的会议id
	//         if ( !bConfFromFile )
	//         {
	//             m_tConf.SetConfId( g_cMcuVcApp.MakeConfId(m_byConfIdx, 0, m_tConf.GetUsrGrpId(),
	// 				                                      m_tConf.GetConfSource()) );
	//         }
        
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
        
			//若会议结束模式为无终端自动结束，应该有终端
			if(0 == byMtNum && m_tConf.GetConfAttrb().IsReleaseNoMt())
			{
				CreateConfSendMcsNack( byOldInsId, ERR_MCU_NOMTINCONF, cServMsg );
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] Conference %s create failed because on mt in conf!\n", m_tConf.GetConfName() );
				return;
			}

			// 会议处理: 即时会议, 无预约会议
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

			//MCU的外设能力是否充分
			if ( !IsVCSEqpInfoCheckPass(wErrCode))
			{
				CreateConfEqpInsufficientNack( byCreateMtId, byOldInsId, wErrCode, cServMsg );
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
                
                // guzh [7/19/2007] 发送给创建者
                SendMsgToMt(CONF_CREATE_MT, MCU_MT_CREATECONF_ACK, cAckMsg);
            }


			//打印信息 
			if ( CONF_DATAMODE_VAONLY == m_tConf.GetConfAttrb().GetDataMode() )
			{
				//视讯会议
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] conference %s created and started!\n",m_tConf.GetConfName());
			}
			else if ( CONF_DATAMODE_VAANDDATA == m_tConf.GetConfAttrb().GetDataMode() )
			{
				//视讯和数据
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] conference %s created and started with data conf function !\n", m_tConf.GetConfName());
			}
			else
			{
				//数据会议
				ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] conference %s created and started with data conf function Only !\n", m_tConf.GetConfName());
			}


			   //保存到文件(由上级MCU的呼叫创建的会议不保存)，N+1备份模式不保存到文件
			   //VCS会议不需要进行文件保存，MCU重启后VCS会议不自动启动，而由调度员启动
	//         if ( CONF_CREATE_NPLUS == m_byCreateBy && MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
	//         {
	//             NPlusLog("[ProcVcsMcuCreateConfReq] conf info don't save to file in N+1 restore mode.\n");
	//         }
	//         else
	//         {
	//             g_cMcuVcApp.SaveConfToFile( m_byConfIdx, FALSE );
	//         }
															
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
					ConfLog( FALSE, "[ProcVcsMcuCreateConfReq] charge postponed due to GetRegGKDriId.%d, ChargeRegOK.%d !\n",
									 g_cMcuVcApp.GetRegGKDriId(), g_cMcuVcApp.GetChargeRegOK() );
				}
			}

			// 顾振华 [4/29/2006]。原来通知MCS发送在保存到文件上面。
			// 移动到这里是为了保证Gk的信息是正确的。通知所有会控。
			cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
			SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

			// 邀请终端，如果上面已经标记
// 			if (bInviteMtNow)                            
			{
				InviteUnjoinedMt(cServMsg);
			}

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
			m_tConfAllMtInfo.m_tLocalMtInfo.SetMcuId( LOCAL_MCUID );
			cServMsg.SetMsgBody( (u8 *)&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
			SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
        
			//发给会控终端表 
			SendMtListToMcs(LOCAL_MCUID);	

			// 发送给会控当前的调度会议的模式
			// 默认创建时都为单方调度模式
			m_cVCSConfStatus.VCCDefaultStatus();
			VCSConfStatusNotif();
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
			u8* pbyMsgBody = cServMsg.GetMsgBody();

			TMt tVCMT;
			memcpy(&tVCMT, pbyMsgBody, sizeof(TMt));
			pbyMsgBody += sizeof(TMt);

			u8 byVCType  = *(u8*)pbyMsgBody;
			pbyMsgBody += sizeof(u8); 

			//强拆 
			u8 byForceCallType = *(u8*)pbyMsgBody; 
			ConfLog(FALSE, "[ProcVcsMcuVCMTReq] operate(vctype:%d, forcecalltype:%d) mt(mcuid:%d mtid:%d) req\n",
				    byVCType, byForceCallType, tVCMT.GetMcuId(), tVCMT.GetMtId());
			// 对调度资源的有效性进行判断,必须在受邀列表中，且不为定时呼叫的固定非可调用资源(高清电视墙、绑定终端、下级mcu)
			if (!m_tConfAllMtInfo.MtInConf(tVCMT.GetMcuId(), tVCMT.GetMtId())
				|| (CONF_CALLMODE_TIMER == VCSGetCallMode(tVCMT)))
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
			
			// 保存当前操作的vcsssn信息
			u8 byScrId = cServMsg.GetSrcSsnId();
			if (byScrId != 0)
			{
				m_cVCSConfStatus.SetCurSrcSsnId(byScrId);
			}

			TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();

			// 对调度资源的与会情况进行判断，若已与会，在调度模式下默认为进行挂断操作			
			BOOL byVCMTOnline = m_tConfAllMtInfo.MtJoinedConf(tVCMT.GetMcuId(), tVCMT.GetMtId());
			if (byVCMTOnline && !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
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
						OspPrintf(TRUE, FALSE, "[ProcVcsMcuVCMTReq] Error, non-curVCMT on line\n");
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
						ChgCurVCMT(tVCMT);
					}
				}
				break;
			default:
				ConfLog(FALSE, "[ProcVcsMcuVCMTReq] Wrong work mode\n");
				break;
			}
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
			u8 byOldMode = m_cVCSConfStatus.GetCurVCMode();
			if ( byOldMode != byNewMode)
			{
				// 切换至新模式,所需外设是否足够
				if (VCS_MULTW_MODE == byNewMode || VCS_GROUPTW_MODE == byNewMode)
				{
					if (!FindUsableTWChan(m_cVCSConfStatus.GetCurUseTWChanInd()))
					{
						// 未找到可用的通道索引，则拒绝
						EqpLog("[ProcVcsMcuVCModeChgReq] Find no usable TW!\n");
						cServMsg.SetErrorCode(ERR_MCU_VCS_NOUSABLETW);
						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						return;
					}
				}
				else if (VCS_MULVMP_MODE == byNewMode || VCS_GROUPVMP_MODE == byNewMode)
				{
					// 查询是否存在可用的VMP,存在就先占有
					if (!FindUsableVMP())
					{
						// 未找到可用的画面合成器，则拒绝
						EqpLog("[ProcVcsMcuVCModeChgReq] Find no usable VMP!\n");
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
				VcsLog("[ProcVcsMcuVCModeChgReq] No change about vcmode\n");
				cServMsg.SetErrorCode(ERR_MCU_VCS_NOMODECHG);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
			}
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "[ProcVcsMcuVCModeChgReq] Wrong message %u(%s) received in state %u!\n", 
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
void CMcuVcInst::ProcVcsMcuMuteReq( const CMessage * pcMsg )
{
	VcsLog("[ProcVcsMcuMuteReq] %d(%s) passed\n", pcMsg->event, OspEventDesc(pcMsg->event));

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	switch(CurState())
	{
	case STATE_ONGOING:
		{
		    if (cServMsg.GetMsgBodyLen() < sizeof(u8) * 4)
		    {
				ConfLog(FALSE, "[ProcVcsMcuMuteReq] message %u(%s) received with uncorrect length!\n",
					    pcMsg->event, ::OspEventDesc((pcMsg->event)));
				return;
		    }	
			
			u8 byMode = *(u8*)cServMsg.GetMsgBody();
			if(byMode != m_cVCSConfStatus.GetCurVCMode())
			{
				VcsLog("[ProcVcsMcuMuteReq] Mode changed, operation cancel\n");
				cServMsg.SetErrorCode(ERR_MCU_VCS_CANCEL);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}
			
			u8  byMuteOpenFlag = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8));         //是否开启
			u8  byOprObj       = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8) * 2);     //操作本地/远端
			u8  byAudProcType  = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8) * 3);     //处理静音/哑音
			
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
					if (m_tConfAllMtInfo.MtJoinedConf(byMTId) && 
						(CONF_CALLMODE_NONE == m_ptMtTable->GetCallMode(byMTId) || MT_TYPE_SMCU == m_ptMtTable->GetMtType(byMTId)))
					{
						TMt tMt = m_ptMtTable->GetMt(byMTId);
						if (!tMt.IsNull())
						{
							VCSMTMute(tMt, byMuteOpenFlag, byAudProcType);
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
		ConfLog( FALSE, "[ProcVcsMcuMuteReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
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
void CMcuVcInst::ProcVCMTOverTime(const CMessage * pcMsg)
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
			RemoveJoinedMt( tReqVCMT, TRUE );
		}

		TMt tNull;
		tNull.SetNull();
		m_cVCSConfStatus.SetReqVCMT(tNull);
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
						InviteUnjoinedMt(cServBackMsg, ptMt);
					}
					else
					{
						VcsLog("[ProcVcsMcuGroupCallMtReq]mtid(%d) not in mtlist of the conf or has joined the conf\n",
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
		ConfLog( FALSE, "[ProcVcsMcuGroupCallMtReq] Wrong message %u(%s) received in state %u!\n", 
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
			for (u8 byGroupIdx = 0; byGroupIdx < byGroupNum; byGroupIdx++)
			{
				byGroupNameLen = *pbyMsg++;

				if (!m_cVCSConfStatus.OprGroupCallList((s8*)pbyMsg, byGroupNameLen, FALSE, wErrCode))
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
					if (m_tConfAllMtInfo.MtJoinedConf(ptMt->GetMtId()))
					{
						VCSDropMT(*ptMt);
					}
					else
					{
						VcsLog("[ProcVcsMcuGroupDropMtReq]mtid(%d) not joined the conf\n",
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
		ConfLog( FALSE, "[ProcVcsMcuGroupDropMtReq] Wrong message %u(%s) received in state %u!\n", 
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
			if (m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP)
			{
				VcsLog("[ProcVcsMcuStartChairPollReq]Chairpoll has already started\n");
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
		ConfLog( FALSE, "[ProcVcsMcuStartChairPollReq] Wrong message %u(%s) received in state %u!\n", 
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
void CMcuVcInst::ProcChairPollTimer(const CMessage *pcMsg)
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

	TMt tChairman = m_tConf.GetChairman();
	TMtStatus tMtState;
	m_ptMtTable->GetMtStatus(tChairman.GetMtId(), &tMtState);	
	if (!tMtState.GetSelectMt(MODE_VIDEO).IsNull())
	{
		VCSConfStopMTSel(tChairman, MODE_VIDEO);
	}

	TMt tPollMt = VCSGetNextPollMt();
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
			KillTimer(MCUVC_VCS_CHAIRPOLL_TIMER);
			if (m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP)
			{
				TMt tNullMt;
				tNullMt.SetNull();
				m_cVCSConfStatus.SetChairPollState(VCS_POLL_STOP);
				m_cVCSConfStatus.SetCurChairPollMt(tNullMt);
				m_cVCSConfStatus.SetPollIntval(0);

				VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);

				VCSConfStatusNotif();
			}

			
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "[ProcVcsMcuStopChairPollReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
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
			TAddMtInfo* ptAddMtInfo = (TAddMtInfo*)cServMsg.GetMsgBody();
			
			//不能呼叫会议本身
			if (mtAliasTypeE164 == ptAddMtInfo->m_AliasType)
			{
				if (0 == strcmp(ptAddMtInfo->m_achAlias, m_tConf.GetConfE164()))
				{
					cServMsg.SetErrorCode(ERR_MCU_NOTCALL_CONFITSELF);
					SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
					VcsLog("[ProcVcsMcuAddMtReq]Cannot Call conf itself.\n");
					return;
				}
			}

			//是否超过性能限制
			if (!m_cVCSConfStatus.IsAddEnable())
			{
				cServMsg.SetErrorCode(ERR_MCU_VCS_OVERADDMTNUM);
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				VcsLog("[ProcVcsMcuAddMtReq]over addmt num\n");
				return;
			}
			
			//增加受邀终端列表
			u8 byMtId = AddMt(*ptAddMtInfo, ptAddMtInfo->GetCallBitRate(), ptAddMtInfo->GetCallMode());
			VcsLog("[ProcVcsMcuAddMtReq]add mt-%d callmode-%d DialBitRate-%d type-%d alias-", 
					byMtId, ptAddMtInfo->GetCallMode(), ptAddMtInfo->GetCallBitRate(), 
					ptAddMtInfo->m_AliasType );
			if ( ptAddMtInfo->m_AliasType == mtAliasTypeTransportAddress )
			{
				VcsLog("%s:%d!\n", StrOfIP(ptAddMtInfo->m_tTransportAddr.GetIpAddr()), 
							       ptAddMtInfo->m_tTransportAddr.GetPort() );
			}
			else
			{
				VcsLog("%s!\n",	ptAddMtInfo->m_achAlias);
			}

			if (0 == byMtId)
			{
				VcsLog("[ProcVcsMcuAddMtReq]Fail to alloc mtid\n");
				SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
				return;
			}

			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

			TMt tMt = m_ptMtTable->GetMt( byMtId );
			if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
			{
				InviteUnjoinedMt( cServMsg, &tMt, TRUE, TRUE );
			}

			//发给会控会议所有终端信息
			cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
			SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

			//发给会控终端表
			SendMtListToMcs(LOCAL_MCUID);

			m_cVCSConfStatus.OprNewMt(tMt, TRUE);
			VCSConfStatusNotif();
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "[ProcVcsMcuAddMtReq] Wrong message %u(%s) received in state %u!\n", 
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
			TMt tMt = *(TMt*)cServMsg.GetMsgBody();
			SendReplyBack(cServMsg, cServMsg.GetEventId() + 1);

			if (!tMt.IsNull() && 
				m_cVCSConfStatus.OprNewMt(tMt, FALSE))
			{
				VcsLog("[ProcVcsMcuDelMtReq]drop mt(mcuid:%d, mtid:%d)\n", 
					   tMt.GetMcuId(), tMt.GetMtId());
				VCSDropMT(tMt);

				VCSConfStatusNotif();
			}
		}
		break;
	default:
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		ConfLog( FALSE, "[ProcVcsMcuDelMtReq] Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
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
	VcsLog("[ProcVcsMcuMsg] %d(%s) passed\n", pcMsg->event, OspEventDesc(pcMsg->event));

	// 判断本地主席终端是否在线，不在线则不允许进行任何调度操作
	if (VCS_MCU_VCMT_REQ == pcMsg->event || VCS_MCU_CHGVCMODE_REQ == pcMsg->event)
	{
		CServMsg cServMsg(pcMsg->content, pcMsg->length);
		TMt tChairMan;
		tChairMan = m_tConf.GetChairman();
		if (tChairMan.IsNull() || 
			!m_tConfAllMtInfo.MtJoinedConf(tChairMan.GetMcuId(), tChairMan.GetMtId()) &&
			m_byCreateBy != CONF_CREATE_MT &&
			!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
		{
			cServMsg.SetErrorCode(ERR_MCU_VCS_LOCMTOFFLINE);
			cServMsg.SetEventId(cServMsg.GetEventId() + 2);
			SendReplyBack(cServMsg, cServMsg.GetEventId());
			return;
		}
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
		ProcVcsMcuMuteReq(pcMsg);
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
		ConfLog(FALSE, "[ProcVcsMcuRlsMtMsg] wrong msg lenth(real:%d, less than %d)\n",
			    cServMsg.GetMsgBodyLen(), sizeof(TMt));
		return;
	}
	u8* pbyMsgBody = cServMsg.GetMsgBody();
	TMt tMt = *(TMt*)pbyMsgBody;
	pbyMsgBody += sizeof(TMt);

	s8 abyConfName[MAXLEN_CONFNAME + 1] = {0};
	u8 byPermitOrNot = FALSE;
	memcpy(abyConfName, pbyMsgBody, min(MAXLEN_CONFNAME, cServMsg.GetMsgBodyLen() - sizeof(TMt)));
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
		ConfLog(FALSE, "[ProcVcsMcuRlsMtMsg] receive un exist msg(%d)\n", pcMsg->event);
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

    wAliasBufLen = ntohs( *(u16*)(cServMsg.GetMsgBody() + sizeof(TConfInfo)) );
    pszAliaseBuf = (char*)(cServMsg.GetMsgBody() + sizeof(TConfInfo) + sizeof(u16));
    // 解析可调度的终端列表
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
		u8 byIsHDTWCfg = *(u8*)(cServMsg.GetMsgBody() + nLen);
		nLen += sizeof(u8);
		if (byIsHDTWCfg)
		{
			nLen += sizeof(THDTvWall);
		}
		u8 byIsVCSSMCUCfg = *(u8*)(cServMsg.GetMsgBody() + nLen);
		nLen += sizeof(u8);
		if (byIsVCSSMCUCfg)
		{
			u16 wAliasBufLen = ntohs(*(u16*)(cServMsg.GetMsgBody() + nLen));
			nLen += sizeof(u16) + wAliasBufLen;
		}

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

	for( byLoop=1; byLoop<=byMtNum; byLoop++)
	{
		//终端呼叫入会时，不论创会还是加入已有会议，均保留其呼入的DRI接入信息
		if( CONF_CREATE_MT == m_byCreateBy && 1 == byLoop ) 
		{
			byMtId = AddMt(tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], CONF_CALLMODE_NONE);
            
            // zbq [12/19/2007] 直接创会终端须在此补入IP，避免E164优先情况下的漏填
            if ( tMtAliasArray[byLoop-1].m_AliasType != mtAliasTypeTransportAddress )
            {
                m_ptMtTable->SetMtAlias( byMtId, ptMtAddr );
                m_ptMtTable->SetIPAddr( byMtId, ptMtAddr->m_tTransportAddr.GetIpAddr() );                    
            }

            // xsl [11/8/2006] 接入板终端计数加1
            g_cMcuVcApp.IncMtAdpMtNum( cServMsg.GetSrcDriId(), m_byConfIdx, byMtId );
            m_ptMtTable->SetDriId(byMtId, cServMsg.GetSrcDriId());
		}
		else
		{
			// 将可调度终端信息保存，并加入受邀列表
			// 默认为单方调度模式启动，呼叫模式为手动
			byMtId = AddMt( tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], CONF_CALLMODE_NONE );
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
			}
			else
			{
				m_ptMtTable->SetMtType(byMtId, MT_TYPE_MT);
			}
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
		//对于主席需要将其设置为无限次定时呼叫
		m_ptMtTable->SetCallMode(byMtId, CONF_CALLMODE_TIMER);
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
				// VCS电视墙模板目前采取动态配置成员，因而模板中通道不指定成员，但需要指定哪些通道使用
				// 使用时将指定通道按顺序使用，将可用通道信息保存到外设状态中
                tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byMemberType;
                tTWStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
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
		for( byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++ )
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
                    }                          
                }
                else
                {
                    ConfLog(FALSE, "Invalid MtIndex :%d in CreateConf Vmp Module\n", byMtIndex);
                }
			}
		}
		m_tConfEqpModule.SetVmpModule( *ptVmpModule );
		pszMsgBuf += sizeof(TVmpModule);
	}
    else
    {
        TVmpModule tVmpModule;
        memset( &tVmpModule, 0, sizeof(tVmpModule) );
        m_tConfEqpModule.SetVmpModule( tVmpModule );
		TMt tNullMt;
        tNullMt.SetNull();
        for( byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++ )
		{
			m_tConfEqpModule.SetVmpMember( byLoop, tNullMt );
		}
    }

	// VCS: + 1byte(u8: 0 1  是否配置了高清电视墙)
	//      +(可选, THDTvWall])
	//      + 1byte(u8: 会议配置HDU的通道总数)
	//      + (可选, THduModChnlInfo+...)
	//      + 1byte(是否为级联调度)＋(可选，2byte[u16 网络序，配置打包的总长度]+下级mcu配置[1byte(别名类型)+1byte(别名长度)+xbyte(别名字符串)+2byte(呼叫码率)...))
	//      + 1byte(是否支持分组)＋(可选，2byte(u16 网络序，信息总长)+内容(1byte(组数)+n组[1TVCSGroupInfo＋m个TVCSEntryInfo])

	// 高清电视墙
    if (*pszMsgBuf++)
    {
		m_tConfEqpModule.SetHDTvWall((THDTvWall*)pszMsgBuf);
		for( byLoop = 1; byLoop <= m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum; byLoop++)
		{
			// 添加高清到受邀列表中，且为无限次定时呼叫
			byMtId = AddMt( m_tConfEqpModule.m_tHDTvWall.m_atHDTWModuleAlias[byLoop-1], 
							m_tConfEqpModule.m_tHDTvWall.m_awHDTWDialBitRate[byLoop-1],
							CONF_CALLMODE_TIMER );
		}
		pszMsgBuf += sizeof(THDTvWall);
		
		byMtNum += m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum;
    }
	else
	{
		// 未配置高清电视墙，清空参数
		THDTvWall tNull;
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
			byMtId = AddMt( tMtAliasArray[byLoop-1], awMtDialRate[byLoop-1], CONF_CALLMODE_TIMER );
		}
		pszAliaseBuf += sizeof(u16) + wAliasBufLen;
		
		byMtNum += byDevNum;
    }

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
        ConfLog( FALSE, "[IsVCSEqpInfoCheckPass] tConfInfo.IsNull, ignore it\n" );
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
            ConfLog( FALSE, "[IsVCSEqpInfoCheckPass]Conference %s create failed because of multicast address occupied!\n", m_tConf.GetConfName() );
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
            ConfLog( FALSE, "[IsVCSEqpInfoCheckPass]Conference %s create failed because of distributed conf multicast address occupied!\n", m_tConf.GetConfName() );
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
            ConfLog( FALSE, "[IsVCSEqpInfoCheckPass]Conference %s create failed because of no DCS registered!\n", m_tConf.GetConfName() );
            return FALSE;
        }
    }

	//但对于VCS会议，默认模板设置为启用适配器，此时不进行预留，待调度时根据需要启用
	//对于画面合成器及电视墙均不进行资源效验预留

	//需要丢包重传的会议,预留包重传通道 
	if (tConfAttrib.IsResendLosePack())
	{
		u8 byEqpId;
        u8 byChnIdx;
		TPeriEqpStatus tStatus;
        BOOL32 bNoIdlePrs = FALSE;
		u8 byNeedPrsChls = DEFAULT_PRS_CHANNELS;
		if(EQP_CHANNO_INVALID != m_byVidBasChnnl)
		{
			byNeedPrsChls += 1;
		}
		if(EQP_CHANNO_INVALID != m_byBrBasChnnl)
		{
			byNeedPrsChls += 1;
		}
		if(EQP_CHANNO_INVALID != m_byAudBasChnnl)
		{
			byNeedPrsChls += 1;
		}
		
		// if (g_cMcuVcApp.GetIdlePrsChl(byEqpId, byChnIdx, byChnIdx2, byChnIdxAud))
		TPrsChannel tPrsChannel;
		if( g_cMcuVcApp.GetIdlePrsChls( byNeedPrsChls, tPrsChannel) )
		{
			byEqpId = tPrsChannel.GetPrsId();
			m_tPrsEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_PRS);
			m_tPrsEqp.SetConfIdx(m_byConfIdx);
			byChnIdx = 0;
			m_byPrsChnnl = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			m_byPrsChnnl2 = tPrsChannel.m_abyPrsChannels[byChnIdx++];
			m_byPrsChnnlAud = tPrsChannel.m_abyPrsChannels[byChnIdx++];

			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
			//目前一个PRS可以为多个会议服务，因此修改为在每个PRS通道中记录该通道服务的会议索引
			//zgc, 2007/04/24
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnl, m_byConfIdx );
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnl2, m_byConfIdx );
			tStatus.m_tStatus.tPrs.SetChnConfIdx( m_byPrsChnnlAud, m_byConfIdx );

			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl].SetReserved(TRUE);
			tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl2].SetReserved(TRUE);
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAud].SetReserved(TRUE);
			g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
			
            if(EQP_CHANNO_INVALID != m_byVidBasChnnl)
            {
                m_byPrsChnnlVidBas = tPrsChannel.m_abyPrsChannels[byChnIdx++];
                g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
                tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVidBas].SetReserved(TRUE);                        
                tStatus.SetConfIdx(m_byConfIdx);                        
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
            }                  

            if(EQP_CHANNO_INVALID != m_byBrBasChnnl)
            {
                m_byPrsChnnlBrBas = tPrsChannel.m_abyPrsChannels[byChnIdx++];
                g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
                tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlBrBas].SetReserved(TRUE);
                tStatus.SetConfIdx(m_byConfIdx);                        
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
            }   
            
            if(EQP_CHANNO_INVALID != m_byAudBasChnnl)
            {
                m_byPrsChnnlAudBas = tPrsChannel.m_abyPrsChannels[byChnIdx++];
                g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus);
                tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAudBas].SetReserved(TRUE);
                tStatus.SetConfIdx(m_byConfIdx);                        
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);
            }                                        
            
			ConfLog(FALSE, "Prs eqp %d chl %d, chl2 %d, chlaud %d, chlVidBas %d, chlBrBas %d, chlAudBas %d reserved for conf %s\n",
                            byEqpId, m_byPrsChnnl,m_byPrsChnnl2,m_byPrsChnnlAud, m_byPrsChnnlVidBas, 
                            m_byPrsChnnlBrBas, m_byPrsChnnlAudBas, m_tConf.GetConfName());
		}
		else
		{
            bNoIdlePrs = TRUE;
        }
        
        // 分配资源失败
		// VCS丢包重传未占用到不给提示,非必须资源
        if(bNoIdlePrs &&
		   m_tConf.GetConfSource() != VCS_CONF)
        {
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
		ConfLog(FALSE, "[VCSCallMT] Call MT(MCUID:%d MTID:%d) whose mcu is not in conf\n",
			           tMt.GetMcuId(), tMt.GetMtId());
		//终端不在可调度的资源范围内
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINVC );
		cServMsg.SetEventId( cServMsg.GetEventId() + 2 );
		SendReplyBack(cServMsg, cServMsg.GetEventId());
		return;
	}

	if( tMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{	
		ConfLog(FALSE, "[VCSCallMT] Call MT(MCUID:%d MTID:%d) has in conf\n",
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
        ConfLog(FALSE, "[VCSCallMT] GetDialAlias failed, byMtId.%d\n", tMt.GetMtId());
        return;
	}

	cServMsg.SetEventId( cServMsg.GetEventId() + 1 );	
	SendReplyBack(cServMsg, cServMsg.GetEventId());

	if (VCS_FORCECALL_CMD == byCallType)
	{
		// 抢占型终端调度最大时间间隔为15s
		SetTimer( MCUVC_VCMTOVERTIMER_TIMER, 15000 );
	}
	else
	{
		// 设置调度终端的最大时间间隔5s，5s后终端未呼上则提示调度终端超时
		SetTimer( MCUVC_VCMTOVERTIMER_TIMER, 5000 );
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
		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		SendMsgToMt( tMt.GetMcuId(), MCU_MCU_REINVITEMT_REQ, cServMsg );
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
		ConfLog(FALSE, "[VCSDropMT] Del MT(MCUID:%d MTID:%d) whose mcu is not in conf\n",
			            tMt.GetMcuId(), tMt.GetMtId());
		return;
	}
	if(!tMtInfo.MtJoinedConf(tMt.GetMtId()))
	{	
		ConfLog(FALSE, "[VCSDropMT] Del MT(MCUID:%d MTID:%d) has not in conf\n",
			            tMt.GetMcuId(), tMt.GetMtId());
		return;
	}

	TMt tNullMT;
	tNullMT.SetNull();
	if (tMt == m_cVCSConfStatus.GetCurVCMT())
	{
		m_cVCSConfStatus.SetCurVCMT(tNullMT);
		if (VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
			!m_tConf.GetChairman().IsNull())
		{
			TMt tChairMan = m_tConf.GetChairman();
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

	if (tMt == m_cVCSConfStatus.GetReqVCMT())
	{
		m_cVCSConfStatus.SetReqVCMT(tNullMT);
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

		cServMsg.SetEventId(MCU_MCU_DROPMT_REQ);
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody((u8 *)&tMt, sizeof(TMt));
		SendMsgToMt( tMt.GetMcuId(), MCU_MCU_DROPMT_REQ, cServMsg );

		TMt tSMCU = m_ptMtTable->GetMt(tMt.GetMcuId());
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

		return;
	}		


	//挂断本地终端前，先将终端恢复成非静哑音状态
	//若为高清终端，需要恢复分辨率
	if (tMt.IsLocal())
	{
		VCSMTMute(tMt, FALSE, VCS_AUDPROC_MUTE);
		VCSMTMute(tMt, FALSE, VCS_AUDPROC_SILENCE);
		TLogicalChannel tSrcRvsChannl;
		if ( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
		{
			if ( tSrcRvsChannl.GetChannelType() == MEDIA_TYPE_H264 &&
				( IsVidFormatHD( tSrcRvsChannl.GetVideoFormat() ) ||
				  tSrcRvsChannl.GetVideoFormat() == VIDEO_FORMAT_4CIF ) )
			{
				ChangeVFormat(tMt, tSrcRvsChannl.GetVideoFormat());
			}
		}
	}
	
	RemoveJoinedMt( tMt, TRUE );		
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
====================================================================*/
BOOL CMcuVcInst::VCSConfSelMT( TSwitchInfo tSwitchInfo )
{    
 
    if (CurState() != STATE_ONGOING)
    {
		CfgLog("[VCSConfSelMT] InstID(%d) in wrong state(%d)\n", GetInsID(), CurState());
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
        ConfLog(FALSE, "[VCSConfSelMT] Mt(%d,%d) Select see Mt(%d,%d), returned with nack!\n", 
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
		ConfLog( FALSE, "[VCSConfSelMT] Mt(%d,%d) select see Mt(%d,%d), selmode(%d)\n", 
			     tDstMt.GetMcuId(), tDstMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSwitchInfo.GetMode() );

		tDstMt = GetLocalMtFromOtherMcuMt( tDstMt );				
		tDstMt = m_ptMtTable->GetMt(tDstMt.GetMtId());
		tSwitchInfo.SetDstMt( tDstMt );	
		
        //目的终端未入会议
        if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
        {
            ConfLog( FALSE, "[VCSConfSelMT] Specified Mt(%u,%u) not joined conference!\n", 
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
		tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);
		tSwitchInfo.SetSrcMt( tSrcMt );
		//源终端未入会议，NACK
		if( !m_tConfAllMtInfo.MtJoinedConf( tSrcMt.GetMtId() ) )
		{
			ConfLog( FALSE, "[VCSConfSelMT] Select source Mt(%u,%u) has not joined current conference!\n",
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
    u8 bySrcMtID = tSrcMt.GetMtId();
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
		//VCS选看业务非用户主动请求,不给提示
// 		if (MODE_AUDIO == tSwitchInfo.GetMode())
// 		{
//  		cServMsg.SetErrorCode(ERR_MCU_VCS_NOVCMTAUD);
// 		}
// 		else if (MODE_VIDEO == tSwitchInfo.GetMode())
// 		{
// 			cServMsg.SetErrorCode(ERR_MCU_VCS_NOVCMTVID);
// 		}
// 		else
// 		{
// 	        cServMsg.SetErrorCode( ERR_MCU_SRCISRECVONLY );
// 		}
// 		SendMsgToAllMcs( MCU_VCS_VCMT_NOTIF, cServMsg );

		ConfLog(FALSE, "[VCSConfSelMT] fail to sel(%d) because of the openning of dual logical channel\n",
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
			ConfLog( FALSE, "[VCSConfSelMT] Conference %s is mixing now. Cannot switch only audio!\n", 
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
                ConfLog( FALSE, "[VCSConfSelMT] Conference %s is mixing now. Cannot switch audio!\n", 
                         m_tConf.GetConfName() );
//              cServMsg.SetErrorCode( ERR_MCU_SELBOTH_INMIXING );
// 				SendMsgToAllMcs( MCU_VCS_VCMT_NOTIF, cServMsg );
				bySwitchMode = MODE_VIDEO;
            }
			tSwitchInfo.SetMode( bySwitchMode );
		}
	}

    // 对于终端视频选看需要判断两者的视频能力是否匹配
	BOOL byIsNeedHDAdpt= IsNeedSelApt(bySrcMtID, byDstMtId, bySwitchMode);
	if (byIsNeedHDAdpt && MODE_VIDEO == bySwitchMode &&
		StartHdVidSelAdp(tOrigSrcMt, tDstMt, bySwitchMode))
	{
	    VcsLog("[VCSConfSelMT] mt(mcuid:%d, mtid:%d) sel(mode:%d) mt(mcuid:%d, mtid:%d) using hdadp suc\n",
			   tDstMt.GetMcuId(), tDstMt.GetMtId(), bySwitchMode, tOrigSrcMt.GetMcuId(), tOrigSrcMt.GetMtId());
	}
	else if (byIsNeedHDAdpt && MODE_AUDIO == bySwitchMode &&
		     StartAudSelAdp(tOrigSrcMt, tDstMt))
	{
	    VcsLog("[VCSConfSelMT] mt(mcuid:%d, mtid:%d) sel(mode:%d) mt(mcuid:%d, mtid:%d) using audadp suc\n",
			   tDstMt.GetMcuId(), tDstMt.GetMtId(), bySwitchMode, tOrigSrcMt.GetMcuId(), tOrigSrcMt.GetMtId());

	}
    else if (!byIsNeedHDAdpt)
    {
		VCSSwitchSrcToDst( tSwitchInfo );
    }
	else
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
		VcsLog("[VCSConfSelMT]mt(mcuid:%d, mtid:%d) sel mt(mcuid:%d, mtid:%d) need adp, but fail to start adp\n",
			    tDstMt.GetMcuId(), tDstMt.GetMtId(), tOrigSrcMt.GetMcuId(), tOrigSrcMt.GetMtId());
		return FALSE;
	}

    //级联选看，将被选看者setin
    if ( !tOrigSrcMt.IsLocal() )
    {
        //查看MC
        TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tOrigSrcMt.GetMcuId());
        if(ptMcInfo == NULL)
        {
			ConfLog( FALSE, "[VCSConfSelMT] MCU(mcuid:%d) Info(ptMcInfo) is NULL!\n",
				     tOrigSrcMt.GetMcuId());
            return FALSE;
        }
        //构造选看源
        TMt tMt;
        tMt.SetMcuId(tOrigSrcMt.GetMcuId());
        tMt.SetMtId(0);
        TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
        if(ptStatus == NULL)
        {
			ConfLog( FALSE, "[VCSConfSelMT] MCU(mcuid:%d) status(ptStatus) is NULL!\n",
				     tOrigSrcMt.GetMcuId());
            return FALSE;
        }
    
        OnMMcuSetIn( tOrigSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
    }

    // 级联选看，将选看者setout
    if( tOrigDstMt.GetMtType() == MT_TYPE_SMCU || tOrigDstMt.GetMtType() == MT_TYPE_MMCU )
    {
        TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tDstMt.GetMtId());
        if(ptConfMcInfo != NULL)
        {
            ptConfMcInfo->m_tSpyMt = tSrcMt;
            //send output 消息
            TSetOutParam tOutParam;
            tOutParam.m_nMtCount = 1;
            tOutParam.m_atConfViewOutInfo[0].m_tMt = tDstMt;
            tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
            tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;
            CServMsg cServMsg2;
            cServMsg2.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
            cServMsg2.SetEventId(MCU_MCU_SETOUT_NOTIF);
            SendMsgToMt(tDstMt.GetMtId(), MCU_MCU_SETOUT_NOTIF, cServMsg2);
        }
    }

    // 更改选看终端(即VCS绑定终端)的状态
    if( MODE_AUDIO == tSwitchInfo.GetMode() || 
        MODE_VIDEO == tSwitchInfo.GetMode() ||
        MODE_BOTH  == tSwitchInfo.GetMode() )
    {
        m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus ); // 选看
        tDstMtStatus.SetSelectMt( tOrigSrcMt, tSwitchInfo.GetMode() );
        m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
    }

    //判断选看者是否调整码率
	u16 wBitrate = 0;
	TLogicalChannel tLogicalChannel;
    if (!byIsNeedHDAdpt && IsNeedAdjustMtSndBitrate(tSrcMt.GetMtId(), wBitrate) &&
		m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE) &&
		wBitrate < tLogicalChannel.GetFlowControl())
    {
        Mt2Log("[VCSConfSelMT] mt.%d, mode %d, old flowcontrol :%d, min flowcontrol :%d\n",
                tSrcMt.GetMtId(), 
                tLogicalChannel.GetMediaType(), tLogicalChannel.GetFlowControl(), wBitrate);                   
        
        tLogicalChannel.SetFlowControl(wBitrate);
        cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
		SendMsgToMt(tSrcMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
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
void CMcuVcInst::VCSConfStopMTSel( TMt tDstMt, u8 byMode )
{	
    //change mode if mixing
    if ( MODE_BOTH == byMode )
    {
        if( m_tConf.m_tStatus.IsMixing() )
        {
            byMode = MODE_VIDEO;
        }
    }
	 
    ConfLog(FALSE, "[VCSConfStopMTSel] Mcu%dMt%d Cancel Select See.\n", tDstMt.GetMcuId(), tDstMt.GetMtId() );
    
    // 选看的目标可能是下级终端
    tDstMt = GetLocalMtFromOtherMcuMt(tDstMt);

	if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
	{
		ConfLog( FALSE, "[VCSConfStopMTSel] Dst Mt%u-%u not joined conference!\n", 
			             tDstMt.GetMcuId(), tDstMt.GetMtId() );
		return;
	}

	// 清除选看状态
	TMtStatus tMtStatus; 
	m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
	if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
	{
		tMtStatus.RemoveSelByMcsMode( MODE_VIDEO );

		// 重新调整选看者的发送码率, 目前只支持调整本级选看者的码率
		TMt tVidSrc = tMtStatus.GetSelectMt( MODE_VIDEO );
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
	}

	if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
	{
		tMtStatus.RemoveSelByMcsMode( MODE_AUDIO );

		// 重新调整选看者的发送码率, 目前只支持调整本级选看者的码率
		TMt tAudSrc = tMtStatus.GetSelectMt( MODE_AUDIO );
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
	}

	// 跨格式选看释放对应的适配资源
	ReleaseResbySel(tDstMt, byMode);
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
 
	// 更改选看者音视频源
	RestoreRcvMediaBrdSrc( tDstMt.GetMtId(), byMode );

	// 立刻通知该选看终端的状态变化
    MtStatusChange( tDstMt.GetMtId(), TRUE );

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
	ConfLog(FALSE, "[VCSConfStopMTSeled]Stop all mt sel spec mt(mcuid:%d, mtid:%d)\n", 
		    tSrcMt.GetMcuId(), tSrcMt.GetMtId());

	// 遍历所有选看该终端的实体，拆交换并清空其选看状态，通知看自己
	TMt       tMt;
	TMtStatus tMtStatus;
    TMt       tNullMt;
	tNullMt.SetNull();

	tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);

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
						StopSwitchToSubMt( tMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE);
						tMtStatus.SetSelectMt(tNullMt, MODE_VIDEO);
						m_ptMtTable->SetMtStatus(byMtIdxLp, &tMtStatus);

						if (IsNeedSelApt(tSelMt.GetMtId(), byMtIdxLp, MODE_VIDEO))
						{
							StopHdVidSelAdp(tSrcMt, m_ptMtTable->GetMt(byMtIdxLp), MODE_VIDEO);
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
						StopSwitchToSubMt( tMt.GetMtId(), MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);
						tMtStatus.SetSelectMt(tNullMt, MODE_VIDEO);
						m_ptMtTable->SetMtStatus(byMtIdxLp, &tMtStatus);
						if (IsNeedSelApt(tSelMt.GetMtId(), byMtIdxLp, MODE_AUDIO))
						{
							StopAudSelAdp(tSrcMt, m_ptMtTable->GetMt(byMtIdxLp));
						}
					}
				}

				if (bySel)
				{
				    MtStatusChange( byMtIdxLp, TRUE );
				}


			}
		}
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
	CServMsg	cServMsg;
	TMt	tDstMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetDstMt());
	TMt tSrcMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetSrcMt());
	u8	byMode = tSwitchInfo.GetMode();

    // 通知码流发送端发送码流      
    NotifyMtSend( tSrcMt.GetMtId(), byMode );

    g_cMpManager.SetSwitchBridge(tSrcMt, 0, byMode);

	if (TYPE_MT == tDstMt.GetType())
	{
		StartSwitchToSubMt(tSrcMt, 0, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE, FALSE);
	}
	else if (TYPE_MCUPERI == tDstMt.GetType())
	{
		StartSwitchToPeriEqp(tSrcMt, 0, tDstMt.GetEqpId(), tSwitchInfo.GetDstChlIdx(), byMode, SWITCH_MODE_SELECT);
	}
	
	
	if (m_tConf.GetConfAttrb().IsResendLosePack())
	{
		//把目的的Rtcp交换给源
		TLogicalChannel tLogicalChannel;
        u8 bySrcChnnl = 0;
		u32 dwDstIp;
		u16 wDstPort;

        if (TYPE_MT == tDstMt.GetType())
        {
            bySrcChnnl = 0;
        }
        else if (TYPE_MCUPERI == tDstMt.GetType())
        {
            bySrcChnnl = tSwitchInfo.GetDstChlIdx();
        }
        
        if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
        {
            m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);

		    dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
		    wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();

            //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传                        
            SwitchVideoRtcpToDst(dwDstIp, wDstPort, tDstMt, bySrcChnnl, MODE_VIDEO, SWITCH_MODE_SELECT, TRUE);
        }
    
        if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
        {
            m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE);

		    dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
		    wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();

            SwitchVideoRtcpToDst(dwDstIp, wDstPort, tDstMt, bySrcChnnl, MODE_AUDIO, SWITCH_MODE_SELECT, TRUE);
        }
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
		OspPrintf(TRUE, FALSE, "[VCSConfMTToTW] InstID(%d) in wrong state(%d)\n", GetInsID(), CurState());
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

	u8 byDstId = tTWSwitchInfo.GetDstMt().GetEqpId();
    // 对于高清终端放入标清电视墙，改变分辨率
    TLogicalChannel tSrcRvsChannl;
    if ( byDstId >= TVWALLID_MIN && byDstId <= TVWALLID_MAX &&
		 m_ptMtTable->GetMtLogicChnnl( tSrcMT.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
    {
        if ( tSrcRvsChannl.GetChannelType() == MEDIA_TYPE_H264 &&
            ( IsVidFormatHD( tSrcRvsChannl.GetVideoFormat() ) ||
              tSrcRvsChannl.GetVideoFormat() == VIDEO_FORMAT_4CIF ) )
        {
            VcsLog("[VCSConfMTToTW] Mt<%d> VidType.%d with format.%d support no tvwall\n",
                   tSrcMT.GetMtId(), tSrcRvsChannl.GetChannelType(), tSrcRvsChannl.GetVideoFormat() );
			ChangeVFormat(tSrcMT, VIDEO_FORMAT_CIF);
        }
    }

	// 对于源终端为下级终端，通知下级MCU选入该终端
	TMt tOrigMt = tTWSwitchInfo.GetSrcMt();
	if (!tOrigMt.IsLocal())
    {		
        OnMMcuSetIn(tOrigMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
    }

		
    switch(tTWSwitchInfo.GetMemberType()) 
    {
	case TW_MEMBERTYPE_VCSAUTOSPEC:
		{
			//源终端必须已经加入会议
			if (FALSE == m_tConfAllMtInfo.MtJoinedConf(tSrcMT.GetMtId()))
			{
				ConfLog(FALSE, "[VCSConfMTToTW] select source MT%u-%u has not joined current conference! Error!\n",
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
				ConfLog(FALSE, "[VCSConfMTToTW] select source MT%u-%u has not ability to send video!\n",
						tSrcMT.GetMcuId(), tSrcMT.GetMtId());
// 				cServMsg.SetErrorCode(ERR_MCU_SRCISRECVONLY);
// 				SendMsgToAllMcs(MCU_VCS_VCMT_NOTIF, cServMsg);
				return FALSE;
			}	
		}
        break;
    default:
        ConfLog(FALSE, "[VCSConfMTToTW] Wrong TvWall Member Type!\n");
		return FALSE;
	}

	if (byDstId >= TVWALLID_MIN && byDstId <= TVWALLID_MAX )
	{
	    ChangeTvWallSwitch(&tSrcMT,
                            tTWSwitchInfo.GetDstMt().GetEqpId(),
                            tTWSwitchInfo.GetDstChlIdx(),
                            tTWSwitchInfo.GetMemberType(), TW_STATE_START);
	}
	else if (byDstId >= HDUID_MIN && byDstId <= HDUID_MAX)
	{
		ChangeHduSwitch(&tSrcMT,
                         byDstId,
                         tTWSwitchInfo.GetDstChlIdx(),
                         tTWSwitchInfo.GetMemberType(), TW_STATE_START/*, MODE_VIDEO*/);
	}
	else
	{
		ConfLog(FALSE, "[VCSConfMTToTW] wrong dest id(%d)\n", byDstId);
		return FALSE;
	}
	return TRUE;
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
			tMt = m_ptMtTable->GetMt(tMt.GetMcuId());
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

// 					cServMsg.SetMsgBody((u8 *)&tTvwallStatus, sizeof(tTvwallStatus));
// 					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
				}
			}
		}
	}

	// 高清电视墙
	if (m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum != 0 )
	{
		u8 byNum = m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum;
		u8 byMtId = 0;
		THDTWModuleAlias* ptAlias = (THDTWModuleAlias*)m_tConfEqpModule.m_tHDTvWall.m_atHDTWModuleAlias;
		for (u8 byIndex = 0; byIndex < byNum; byIndex++)
		{
			byMtId = m_ptMtTable->GetMtIdByAlias((TMtAlias*)ptAlias);
			if (byMtId > 0 && m_tConfAllMtInfo.MtJoinedConf(byMtId))
			{
				TMtStatus tMTStatus;
				m_ptMtTable->GetMtStatus(byMtId, &tMTStatus);
				if (tOrigMt == tMTStatus.GetSelectMt(MODE_VIDEO))
				{
					VCSConfStopMTSel(m_ptMtTable->GetMt(byMtId), MODE_VIDEO);
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
		VcsLog("[GoOnSelStep] mt(mcuid:%d, mtid:%d) go on sel chairman\n",
			   tMt.GetMcuId(), tMt.GetMtId());
	}
	else
	{
		VcsLog("[GoOnSelStep] chairman go on sel mt(mcuid:%d, mtid:%d)\n",
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
		VcsLog("[GoOnSelStep] No sel step for subconf\n");
		return;
	}

	// 组呼模式下不进行主席的选看被选看操作
	if (ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
	{
		VcsLog("[GoOnSelStep] No sel step in this mode(%d)", m_cVCSConfStatus.GetCurVCMode());
		return;
	}
	
	TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();
	// 当前调度终端或当前调度终端所在的mcu才可进行主席选看或被主席选看,排除以下
	// 由于时序问题，对于选看主席, 放宽要求, 只需非固定资源即可
	if ((!tCurVCMT.IsNull() && 
		(tCurVCMT == tMt || tMt.GetMtId() == tCurVCMT.GetMcuId())) ||
		(TRUE == bySelDirect &&	 CONF_CALLMODE_NONE == VCSGetCallMode(tMt)))
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
				VcsLog("[GoOnSelStep] uncorrect sel mediatype(%d) in single mode\n", byMediaType);
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
					u8 bySrcId = (tSwitchInfo.GetSrcMt().IsLocal() ? tSwitchInfo.GetSrcMt().GetMtId() : tSwitchInfo.GetSrcMt().GetMcuId());
					u8 byDstId = (tSwitchInfo.GetDstMt().IsLocal() ? tSwitchInfo.GetDstMt().GetMtId() : tSwitchInfo.GetDstMt().GetMcuId());
					if (IsNeedSelApt(bySrcId, byDstId, MODE_VIDEO) && 
						IsNeedNewSelAdp(bySrcId, byDstId, MODE_VIDEO) && 
						!g_cMcuVcApp.IsIdleHDBasVidChlExist())
					{
						EqpLog("[GoOnSelStep]not enough bas for all mt in twmode, so stop mt sel chairman\n");
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
			break;
		default:
			OspPrintf(TRUE, FALSE, "[GoOnSelStep] wrong vc mode(%d)\n", m_cVCSConfStatus.GetCurVCMode());
			return;
		    break;
		}
		VCSConfSelMT(tSwitchInfo);


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
		VcsLog("[ChgCurVCMT] NewCurVCMT hasn't change\n");	
		return;
	}
	m_cVCSConfStatus.SetCurVCMT(tNewCurVCMT);

	u8  byVCMode = m_cVCSConfStatus.GetCurVCMode();
	switch(byVCMode)
	{
	case VCS_SINGLE_MODE:
		if (!tCurVCMT.IsNull())
		{
			VCSDropMT(tCurVCMT);
		}
		break;
	case VCS_MULTW_MODE:
	case VCS_GROUPTW_MODE:
		// 下一个终端调度成功再进行，当前调度终端替换，原终端进电视墙的操作
		if (!tCurVCMT.IsNull() && 
			 m_tConfAllMtInfo.MtJoinedConf(tCurVCMT.GetMcuId(), tCurVCMT.GetMtId()))
		{
			u8 byNeedInTW = TRUE;

			// 若当前调度的终端与新调度的终端属于同一个下级MCU,则无需将当前调度的终端放入电视墙
			if (!tCurVCMT.IsLocal() && !tNewCurVCMT.IsLocal() && tCurVCMT.GetMcuId() == tNewCurVCMT.GetMcuId())
			{
				byNeedInTW = FALSE;
			}
			if (byNeedInTW)
			{
				TEqp tEqp;
				TMt  tDropOutMT;
				tEqp.SetNull();
				tDropOutMT.SetNull();
				u8 byChanIdx = 0;
				// 找到可用的电视墙通道，则将原调度终端选看到电视墙；没有则给通知，并恢复到单方调度模式
				u16 dwChanIdx = FindUsableTWChan(m_cVCSConfStatus.GetCurUseTWChanInd(), &tDropOutMT, 
												 &tEqp, &byChanIdx, &tCurVCMT);
				if (!dwChanIdx)
				{
					CServMsg cServMsg;
					cServMsg.SetConfIdx(m_byConfIdx);
					cServMsg.SetConfId(m_tConf.GetConfId());
					cServMsg.SetEventId(MCU_VCS_VCMT_NOTIF);
					cServMsg.SetErrorCode(ERR_MCU_VCS_NOUSABLETW);
					SendMsgToMcs(m_cVCSConfStatus.GetCurSrcSsnId(), MCU_VCS_VCMT_NOTIF, cServMsg);

					// 将会议切换为组播发言人模式或者单方调度模式
					if (ISGROUPMODE(byVCMode))
					{
						RestoreVCConf(VCS_GROUPSPEAK_MODE);
					}
					else
					{
						RestoreVCConf(VCS_SINGLE_MODE);
					}
					return;
				}
				else
				{
					TTWSwitchInfo tSwitchInfo;
					tSwitchInfo.SetSrcMt(tCurVCMT);
					tSwitchInfo.SetDstMt(tEqp);
					tSwitchInfo.SetDstChlIdx(byChanIdx);
					tSwitchInfo.SetMemberType(TW_MEMBERTYPE_VCSAUTOSPEC);
					tSwitchInfo.SetMode(MODE_VIDEO);
					if (VCSConfMTToTW(tSwitchInfo))
					{
						m_cVCSConfStatus.SetCurUseTWChanInd(dwChanIdx);
						// 该通道原有终端自动挂断
						if(!tDropOutMT.IsNull() && VCS_MULTW_MODE == byVCMode)
						{
							// 对于原通道放入的为下级MCU,挂断该MCU下回传通道中的终端
							if (tDropOutMT.IsLocal() && 
								MT_TYPE_SMCU == m_ptMtTable->GetMtType(tDropOutMT.GetMtId()))
							{
								TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tDropOutMT.GetMtId());
								tDropOutMT = ptConfMcInfo->m_tMMcuViewMt;
							}
							ConfLog(FALSE, "[ChgCurVCMT]mcuid:%d mtid:%d enter tw(%d) %d channel and drop mcuid:%d mtid:%d\n", 
								   tCurVCMT.GetMcuId(), tCurVCMT.GetMtId(), tEqp.GetEqpId(), byChanIdx, tDropOutMT.GetMcuId(), tDropOutMT.GetMtId());
							VCSDropMT(tDropOutMT);
						}
					}
				}
			}

		}

		if (VCS_MULTW_MODE == byVCMode)
		{
			VCSConfStopMTSel(m_tConf.GetChairman(), MODE_VIDEO);
		}

		if (VCS_GROUPTW_MODE == byVCMode)
		{
			if (tNewCurVCMT.IsNull())
			{
				TMt tChairMan = m_tConf.GetChairman();
				ChangeSpeaker(&tChairMan, FALSE, FALSE);
			}
			else
			{
				ChangeSpeaker(&tNewCurVCMT, FALSE, FALSE);
			}
		}
		break;
	case VCS_MULVMP_MODE:
		// 无需特殊操作
	    break;
	case VCS_GROUPSPEAK_MODE:
		if (tNewCurVCMT.IsNull())
		{
			TMt tChairMan = m_tConf.GetChairman();
			ChangeSpeaker(&tChairMan, FALSE, FALSE);
		}
		else
		{
			ChangeSpeaker(&tNewCurVCMT, FALSE, FALSE);
		}
	    break;
	case VCS_GROUPVMP_MODE:
		if (!tNewCurVCMT.IsNull())
		{
			ChangeVmpStyle(tNewCurVCMT, TRUE);
		}
		break;
	default:
		OspPrintf(TRUE, FALSE, "[ChgCurVCMT]Wrong vcmode\n");
	    break;
	}

	TMt tNullMt;
	tNullMt.SetNull();
	m_cVCSConfStatus.SetReqVCMT(tNullMt);

	// 通告给所有的VCS会议状态的改变
	VCSConfStatusNotif();
}
/*====================================================================
    函数名      ：FindUsableTWChan
    功能        ：查询下一个可用的电视墙通道，没有空闲的则抢占最早使用的通道号,
	              并将该通道中的原有终端挂断
    算法实现    ：
    引用全局变量：
    输入参数说明：[IN]wCurUseChanIndex: 当前正使用的通道索引
	              [IN/OUT] ptDropOutMT: 若没有未用的通道，被抢占的终端信息
 	              [IN/OUT]        tEqp: 获得的可用的电视墙信息
	              [IN/OUT]   byChanIdx: 获得的可用的电视墙对应通道信息
				  [IN]          ptInMT: 欲进入电视墙的终端
    返回值说明  ：u16:返回当前可用的通道索引号，若索引号为0，则没有可用的
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/28                付秀华        创建
====================================================================*/
u16 CMcuVcInst::FindUsableTWChan(u16 wCurUseChanIndex, TMt* ptDropOutMT, TEqp* ptEqp, u8* pbyChanIdx, TMt* ptInMT)
{
	u16 wUsableChanNum  = 0;       // 所有可用的通道数
	u16 wChanIndex      = 0;       // 通道索引
	u16 wNextChanIndex  = 0;       // 下一个可用的通道索引
	u16 wFirstChanIndex = 0;       // 第一个可用的通道索引

	TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
	// 对于下级的终端进电视墙，优先占用原下级终端的通道
	// 目前应用模式不可能从该段返回，下级除主席外只能存在一个终端在线，
	// 终端从当前调度终端状态才能切换至进电视墙状态，而在前一个状态时，已将导致原进电视墙的下级终端挂断
	if (ptInMT != NULL && !ptInMT->IsNull() && !ptInMT->IsLocal())
	{
		u8 byInMTId = ptInMT->GetMcuId();
		if(tConfAttrib.IsHasTvWallModule() )
		{
			TMultiTvWallModule tMultiTvWallModule;
			m_tConfEqpModule.GetMultiTvWallModule( tMultiTvWallModule );
			TTvWallModule tTvWallModule;
			for( u8 byTvLp = 0; byTvLp < tMultiTvWallModule.GetTvModuleNum(); byTvLp++ )
			{
				tMultiTvWallModule.GetTvModuleByIdx(byTvLp, tTvWallModule);
				if( g_cMcuVcApp.IsPeriEqpConnected( tTvWallModule.m_tTvWall.GetEqpId()) )
				{
					TPeriEqpStatus tStatus;
					TMt tTWMember;
					g_cMcuVcApp.GetPeriEqpStatus(tTvWallModule.m_tTvWall.GetEqpId(), &tStatus);
					for (u8 byIndex = 0; byIndex < MAXNUM_PERIEQP_CHNNL; byIndex++)
					{
						wChanIndex++;
						tTWMember = (TMt)tStatus.m_tStatus.tTvWall.atVideoMt[byIndex];
						tTWMember = GetLocalMtFromOtherMcuMt(tTWMember);
						if (tTWMember.GetMtId() == byInMTId)
						{
							if(ptEqp != NULL)
							{
								*ptEqp = tTvWallModule.GetTvEqp();
							}
							if (pbyChanIdx != NULL)
							{
								*pbyChanIdx = byIndex;
							}
							VcsLog("smcu(mtid:%d) has in tw %d channel, new smt(mcuid:%d mtid:%d) still use this channel\n", 
								   tTWMember.GetMtId(), byIndex, ptInMT->GetMcuId(), ptInMT->GetMtId());
							return wChanIndex;
						}
					}
				}
				else
				{
					wChanIndex += MAXNUM_PERIEQP_CHNNL;
				}
			}
		}

		if (m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum != 0)
		{
			u8 byNum = m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum;
			u8 byMtId = 0;
			THDTWModuleAlias* ptAlias = (THDTWModuleAlias*)m_tConfEqpModule.m_tHDTvWall.m_atHDTWModuleAlias;
			// 检查高清电视墙的在线状态
			for (u8 byIndex = 0; byIndex < byNum; byIndex++)
			{
				wChanIndex++;
				byMtId = m_ptMtTable->GetMtIdByAlias((TMtAlias*)(ptAlias + byIndex));
				if (byMtId > 0 && m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
					m_tConfEqpModule.m_tHDTvWall.m_abyMemberType[byIndex][0] != TW_MEMBERTYPE_NULL)
				{
					TMtStatus tMTStatus;
					m_ptMtTable->GetMtStatus(byMtId, &tMTStatus);
					TMt tSelMT = tMTStatus.GetSelectMt(MODE_VIDEO);
					tSelMT = GetLocalMtFromOtherMcuMt(tSelMT); 
					if (tSelMT.GetMtId() == byInMTId)
					{
						if(ptEqp != NULL)
						{
							*ptEqp = m_ptMtTable->GetMt(byMtId);
						}
						if (pbyChanIdx != NULL)
						{
							*pbyChanIdx = 0;
						}
						VcsLog("smcu(mtid:%d) has in hdtw(mtid:%d), new smt(mcuid:%d mtid:%d) still use this channel\n", 
							    tSelMT.GetMtId(), byMtId, ptInMT->GetMcuId(), ptInMT->GetMtId());
						return wChanIndex;
					}
				}
			}
		}

		if ( m_tConfEqpModule.m_tHduModule.GetHduChnlNum() != 0 )
		{
			u8 byHduChnlNum = m_tConfEqpModule.m_tHduModule.GetHduChnlNum();
			THduModChnlInfo tChnlInfo;
			u8 byHduId, byHduChnlIdx = 0;
			for( u8 byChnlIdx = 0; byChnlIdx < byHduChnlNum; byChnlIdx++ )
			{
				tChnlInfo    = m_tConfEqpModule.m_tHduModule.GetOneHduChnlInfo(byChnlIdx);
				byHduId      = tChnlInfo.GetHduID();
				byHduChnlIdx = tChnlInfo.GetHduChnlIdx();
				if( byHduChnlIdx < MAXNUM_HDU_CHANNEL &&
					byHduId >= HDUID_MIN && byHduId <= HDUID_MAX &&
					g_cMcuVcApp.IsPeriEqpConnected(byHduId) &&
					TW_MEMBERTYPE_VCSAUTOSPEC == tChnlInfo.GetMemberType())
				{
					TPeriEqpStatus tStatus;
					g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tStatus);
					wChanIndex++;
					if (tStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetMtId() == byInMTId)
					{
						if(ptEqp != NULL)
						{
							*ptEqp = tStatus.m_tStatus.tHdu.atHduChnStatus[byHduChnlIdx].GetEqp();
						}
						if (pbyChanIdx != NULL)
						{
							*pbyChanIdx = byHduChnlIdx;
						}
						VcsLog("smcu(mtid:%d) has in HDU(hduid:%d) %d channel, new smt(mcuid:%d mtid:%d) still use this channel\n", 
							    byInMTId, byHduId, byHduChnlIdx, ptInMT->GetMcuId(), ptInMT->GetMtId());
						return wChanIndex;
					}
				}
			}
		}

	}

	// 本级终端进入电视墙
	wUsableChanNum  = 0;       // 所有可用的通道数
	wChanIndex      = 0;       // 通道索引
	wNextChanIndex  = 0;       // 下一个可用的通道索引
	wFirstChanIndex = 0;       // 第一个可用的通道索引

	// hdu通道搜索
	if (m_tConfEqpModule.m_tHduModule.GetHduChnlNum() != 0 )
	{
		u8 byHduChnlNum = m_tConfEqpModule.m_tHduModule.GetHduChnlNum();
		THduModChnlInfo tChnlInfo;
		u8 byHduId, byHduChnlIdx = 0;
		for( u8 byChnlIdx = 0; byChnlIdx < byHduChnlNum; byChnlIdx++ )
		{
			wChanIndex++;
			tChnlInfo    = m_tConfEqpModule.m_tHduModule.GetOneHduChnlInfo(byChnlIdx);
			byHduId      = tChnlInfo.GetHduID();
			byHduChnlIdx = tChnlInfo.GetHduChnlIdx();
			if( byHduChnlIdx < MAXNUM_HDU_CHANNEL &&
				byHduId >= HDUID_MIN && byHduId <= HDUID_MAX &&
				g_cMcuVcApp.IsPeriEqpConnected(byHduId) &&
				TW_MEMBERTYPE_VCSAUTOSPEC == tChnlInfo.GetMemberType() &&				
				(0 == wFirstChanIndex || wChanIndex > wCurUseChanIndex))
			{
				TPeriEqpStatus tStatus;
				g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tStatus);
				// 该通道已经被占用，要踢除
				if (ptDropOutMT != NULL)
				{
					*ptDropOutMT = tStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx];
				}
				if (ptEqp != NULL)
				{
					*ptEqp = tStatus.m_tStatus.tHdu.atHduChnStatus[byHduChnlIdx].GetEqp();
				}
				if (pbyChanIdx != NULL)
				{
					*pbyChanIdx = byHduChnlIdx;
				}

				if (0 == wFirstChanIndex)
				{
					wFirstChanIndex = wChanIndex;
				}

				if (wChanIndex > wCurUseChanIndex)
				{
					wNextChanIndex = wChanIndex;
					return wNextChanIndex;
				}

			}
		}
	}

	if(0 == wNextChanIndex && tConfAttrib.IsHasTvWallModule() )
    {
        TMultiTvWallModule tMultiTvWallModule;
        m_tConfEqpModule.GetMultiTvWallModule( tMultiTvWallModule );
        TTvWallModule tTvWallModule;
        for( u8 byTvLp = 0; byTvLp < tMultiTvWallModule.GetTvModuleNum(); byTvLp++ )
        {
            tMultiTvWallModule.GetTvModuleByIdx(byTvLp, tTvWallModule);
            if( g_cMcuVcApp.IsPeriEqpConnected( tTvWallModule.m_tTvWall.GetEqpId()) )
            {
				for (u8 byIndex = 0; byIndex < MAXNUM_PERIEQP_CHNNL; byIndex++)
				{
					wChanIndex++;
					if (tTvWallModule.m_abyMemberType[byIndex] != TW_MEMBERTYPE_NULL &&
						(0 == wFirstChanIndex || wChanIndex > wCurUseChanIndex))
					{
						// 获取到当前通道的下一个在线可用通道的状态
						TPeriEqpStatus tStatus;
						g_cMcuVcApp.GetPeriEqpStatus(tTvWallModule.m_tTvWall.GetEqpId(), &tStatus);
						TMt tOldMt = (TMt)tStatus.m_tStatus.tTvWall.atVideoMt[byIndex];
						// 该通道已经被占用，要踢除
						if (ptDropOutMT != NULL)
						{
							*ptDropOutMT = tOldMt;
						}
						if (ptEqp != NULL)
						{
							*ptEqp = tTvWallModule.GetTvEqp();
						}
						if (pbyChanIdx != NULL)
						{
							*pbyChanIdx = byIndex;
						}

						if (0 == wFirstChanIndex)
						{
							wFirstChanIndex = wChanIndex;
						}

						if (wChanIndex > wCurUseChanIndex)
						{
							wNextChanIndex = wChanIndex;
							return wNextChanIndex;
						}

					}
				}
            }
			else
			{
				wChanIndex += MAXNUM_PERIEQP_CHNNL;
			}
        }
    }

	// 若未在普通墙中找到当前使用通道的下一个可用通道，搜索高清电视墙
	if ( 0 == wNextChanIndex && m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum != 0 )
	{
		u8 byNum = m_tConfEqpModule.m_tHDTvWall.m_byHDTWDevNum;
		u8 byMtId = 0;
		THDTWModuleAlias* ptAlias = (THDTWModuleAlias*)m_tConfEqpModule.m_tHDTvWall.m_atHDTWModuleAlias;
		// 检查高清电视墙的在线状态
		for (u8 byIndex = 0; byIndex < byNum; byIndex++)
		{
			wChanIndex++;
			byMtId = m_ptMtTable->GetMtIdByAlias((TMtAlias*)(ptAlias + byIndex));
			if (byMtId > 0 && m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
				m_tConfEqpModule.m_tHDTvWall.m_abyMemberType[byIndex][0] != TW_MEMBERTYPE_NULL &&
				(0 == wFirstChanIndex || wChanIndex > wCurUseChanIndex))
			{

				TMtStatus tMTStatus;
				m_ptMtTable->GetMtStatus(byMtId, &tMTStatus);
				if (ptDropOutMT != NULL)
				{
					*ptDropOutMT = tMTStatus.GetSelectMt(MODE_VIDEO);
				}	
				if (ptEqp != NULL)
				{
					*ptEqp = m_ptMtTable->GetMt(byMtId);						
				}
				if (pbyChanIdx != NULL)
				{
					*pbyChanIdx = 0;
				}

				if (0 == wFirstChanIndex)
				{
					wFirstChanIndex = wChanIndex;
				}
				if (wChanIndex > wCurUseChanIndex)
				{
					wNextChanIndex = wChanIndex;
					return wNextChanIndex;
				}
			}						
		}
	}

	return wFirstChanIndex;
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
	// 暂时给一套默认值
	TConfAttrb tConfAttrib = m_tConf.GetConfAttrb();
	tConfAttrib.SetHasVmpModule(TRUE);
	m_tConf.SetConfAttrb(tConfAttrib);

	TVmpModule tModule;
	TVMPParam  tParam;
	tParam.SetVMPAuto(TRUE);
	tParam.SetVMPMode(CONF_VMPMODE_AUTO);
	tParam.SetVMPStyle(VMP_STYLE_DYNAMIC);
	tParam.SetVMPBrdst(TRUE);
	tParam.SetVMPSchemeId(0);
	tModule.SetVmpParam(tParam);
	m_tConfEqpModule.SetVmpModule(tModule);


	tConfAttrib = m_tConf.GetConfAttrb();
	if(tConfAttrib.IsHasVmpModule())			
	{
		u8 byIdleVMPNum = 0;
		u8 abyVMPId[MAXNUM_PERIEQP];
		memset(&abyVMPId, 0, sizeof(abyVMPId));
		g_cMcuVcApp.GetIdleVMP(abyVMPId, byIdleVMPNum, sizeof(abyVMPId));

		//是否有画面合成器
		if( byIdleVMPNum != 0)
		{
			TVMPParam tVmpParam;
			tVmpParam = m_tConfEqpModule.GetVmpModule().GetVmpParam();

			u8  byVmpId = 0;
            if ( CONF_VMPMODE_AUTO == tVmpParam.GetVMPMode() )
            {
                //当前所有的空闲VMP是否支持所需的合成风格
                u16 wError  = 0;

                if ( IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError) ) 
                {
					m_tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
					m_tVmpEqp.SetConfIdx( m_byConfIdx );
					
                }
                else
				{
					VcsLog("[FindUsableVMP] no vmp support thise style\n");
					return FALSE;
				}
			}
			else
			{
                VcsLog("[FindUsableVMP] VCSCONF hasn't non-auto vmp\n");
				return FALSE;
			}

			// 有则占用画面合成器
            TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::RESERVE;//先占用,超时后未成功再放弃
			tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam  = tVmpParam;
			tPeriEqpStatus.SetConfIdx( m_byConfIdx );
			g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
			EqpLog("[FindUsableVMP]vmp(%d) has been used by conf(%d)\n", byVmpId, m_byConfIdx);

			//清空通道
			for( u8 byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++ )
			{
				m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byLoop );
			}
	
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
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
		return;
	}

	VcsLog("[RestoreVCConf] restore vcsconf for mode(%d) begin\n", byNewMode);

	CServMsg cServMsg;
	cServMsg.SetConfIdx(m_byConfIdx);
	cServMsg.SetConfId(m_tConf.GetConfId());

	u8 byOldMode = m_cVCSConfStatus.GetCurVCMode();

    // 停止本地主席终端的码流发送
    NotifyMtSend(m_tConf.GetChairman().GetMtId(), MODE_VIDEO, FALSE);
	// 对于保持在线的主席终端状态的恢复
	// 若主席终端在选看下级MCU，则取消
	// 非组呼模式下主席轮询的停止
	if (!ISGROUPMODE(byNewMode))
	{
		KillTimer(MCUVC_VCS_CHAIRPOLL_TIMER);
		m_cVCSConfStatus.SetChairPollState(VCS_POLL_STOP);
	}
	if (!(ISGROUPMODE(byNewMode) && ISGROUPMODE(byOldMode)))
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
	if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE &&
		byNewMode != VCS_MULVMP_MODE && byNewMode != VCS_GROUPVMP_MODE)
	{
		TPeriEqpStatus tPeriEqpStatus;
		if (!m_tVmpEqp.IsNull() &&
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus) &&
			tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState != TVmpStatus::WAIT_STOP)
		{
			SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, 6*1000);
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
			SendMsgToEqp( m_tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cServMsg );
		}

		// Bug00013728存在时序问题 等到画面合成器停止成功后才放弃画面合成器 
// 		TPeriEqpStatus tPeriEqpStatus;
//  	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
// 		tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = FALSE;
// 		tPeriEqpStatus.SetConfIdx( 0 );
// 		g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus ); 

		m_tConf.m_tStatus.SetVMPMode(CONF_VMPMODE_NONE);

        if (m_tConf.m_tStatus.GetVmpParam().IsVMPBrdst())
        {
            ChangeVidBrdSrc(NULL);
        }

	}

	// 混音器资源的释放
	// 停止混音器
	if( m_tConf.m_tStatus.IsMixing() && !ISGROUPMODE(byNewMode) && 
		byNewMode != VCS_MULTW_MODE && byNewMode != VCS_MULVMP_MODE)
	{
		StopMixing();

		//放弃混音组 同样存在时序问题
// 		TPeriEqpStatus tPeriEqpStatus;
//  		g_cMcuVcApp.GetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
// 		tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = TMixerGrpStatus::READY;
//         tPeriEqpStatus.SetConfIdx( 0 );
// 		g_cMcuVcApp.SetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
// 
//         cServMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
//         SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
	}

	// 电视墙资源的释放
	// HDU释放
	u8  byLoop = 0,  byEqpId = 1;
	TPeriEqpStatus tTvwallStatus;
	for (byEqpId = HDUID_MIN; byEqpId <= HDUID_MAX; byEqpId++)
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byEqpId))
		{
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
            if (1 == tTvwallStatus.m_byOnline)
            {
                u8 byMtConfIdx;
				u8 byInTWMtId;
                u8 byMemberNum = tTvwallStatus.m_tStatus.tHdu.byChnnlNum;
			    for(byLoop = 0; byLoop < byMemberNum; byLoop++)
			    {
                    byMtConfIdx = tTvwallStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
					byInTWMtId  = tTvwallStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetMtId();
				    if (m_byConfIdx == byMtConfIdx)
				    {
					    StopSwitchToPeriEqp(byEqpId, byLoop);
                        tTvwallStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
                        tTvwallStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
						//清终端状态
						m_ptMtTable->SetMtInHdu(byInTWMtId, FALSE);
						MtStatusChange(byInTWMtId, TRUE);						
				    }
			    }
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tTvwallStatus);

                cServMsg.SetMsgBody((u8 *)&tTvwallStatus, sizeof(tTvwallStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
            }			

		}
	}

	// 标清电视墙释放
	for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byEqpId))
		{
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
            if (1 == tTvwallStatus.m_byOnline)
            {
                u8 byMtConfIdx;
				u8 byInTWMtId;
                u8 byMemberNum = tTvwallStatus.m_tStatus.tTvWall.byChnnlNum;
			    for(byLoop = 0; byLoop < byMemberNum; byLoop++)
			    {
                    byMtConfIdx = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
					byInTWMtId  = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMtId();
				    if (m_byConfIdx == byMtConfIdx)
				    {
					    StopSwitchToPeriEqp(byEqpId, byLoop);
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
						//清终端状态
						m_ptMtTable->SetMtInTvWall(byInTWMtId, FALSE);
						MtStatusChange(byInTWMtId, TRUE);						
				    }
			    }
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tTvwallStatus);

                cServMsg.SetMsgBody((u8 *)&tTvwallStatus, sizeof(tTvwallStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
            }			

		}
	}

	// 释放高清电视墙
	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (m_tConfAllMtInfo.MtJoinedConf(byMtId) && 
			CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtId) &&
			MT_TYPE_MT ==m_ptMtTable->GetMtType(byMtId) &&
			byMtId != m_tConf.GetChairman().GetMtId())
		{
			VCSConfStopMTSel(m_ptMtTable->GetMt(byMtId), MODE_BOTH);
		}
	}

	// 调度资源的释放
	// 非组呼模式内切换，挂断所有非定时呼叫的调度资源
	if (!(ISGROUPMODE(byNewMode)  && ISGROUPMODE(byOldMode)))
	{
		TMt tMt;
		for( u8 byIndex = 1; byIndex <= MAXNUM_CONF_MT; byIndex++ )
		{	
			tMt = m_ptMtTable->GetMt(byIndex);
			if(!m_tConfAllMtInfo.MtJoinedConf(byIndex)) 
			{
				continue;
			}
			if (CONF_CALLMODE_NONE == m_ptMtTable->GetCallMode(byIndex) &&
				m_ptMtTable->GetMtType(byIndex) != MT_TYPE_MMCU) 
			{
				VCSDropMT(tMt);
			}
			else if (MT_TYPE_SMCU == m_ptMtTable->GetMtType(byIndex))
			{
				TMt tSMt;
				//在线的SMCU,挂断其下所有的非定时呼叫的调度资源
				TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(byIndex);
				if (NULL == ptMcInfo)
				{
					continue;
				}
				for (u8 bySIndex = 1; bySIndex <= MAXNUM_CONF_MT; bySIndex++)
				{
					tSMt.SetMcuId(byIndex);
					tSMt.SetMtId(bySIndex);
					if (m_tConfAllMtInfo.MtJoinedConf(byIndex, bySIndex) && 
						CONF_CALLMODE_NONE == VCSGetCallMode(tSMt))
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
			}	

		}
	}

	if (ISGROUPMODE(byOldMode) && !ISGROUPMODE(byNewMode))
	{
		RlsAllBasForConf();
	}

	// 发言人清空
	ChangeSpeaker(NULL, FALSE, FALSE);

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
		m_cVCSConfStatus.VCCRestoreGroupStatus();
	}
	else
	{
		m_cVCSConfStatus.VCCRestoreStatus();
	}

	if (ISGROUPMODE(byNewMode) || VCS_MULTW_MODE == byNewMode || VCS_MULVMP_MODE == byNewMode)
	{
		TMt tChairMan = m_tConf.GetChairman();
		// 初始状态只将主席放入混音器
		for (u8 byMixMtId = 1; byMixMtId <= MAXNUM_CONF_MT; byMixMtId++)
		{
			if (tChairMan.GetMtId() == byMixMtId)
			{
				m_ptMtTable->SetMtInMixing(byMixMtId, TRUE);
			}
			else
			{
				m_ptMtTable->SetMtInMixing(byMixMtId, FALSE);
			}
		}

		if (!m_tConf.m_tStatus.IsMixing())
		{
			// 组呼模式均启动定制混音, 多方多画面或电视墙使用全体混音
			BOOL32 dwStartResult = FALSE;
			if (ISGROUPMODE(byNewMode))
			{
				dwStartResult = StartMixing(mcuPartMix);
			}
			else
			{
				dwStartResult = StartMixing(mcuWholeMix);
			}
			if (!dwStartResult)
			{
				VcsLog("[RestoreVCConfInGroupMode] Find no mixer\n");
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
			if (ISGROUPMODE(byNewMode))
			{
				m_tConf.m_tStatus.SetSpecMixing();
				ChangeSpecMixMember(&tChairMan, 1);
			}
			else
			{
				SwitchToAutoMixing();
			}
		}
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
		{
			//会议画面合成参数恢复
			TVMPParam tVmpParam;
			if (m_tConf.GetConfAttrb().IsHasVmpModule())
			{
				tVmpParam = m_tConfEqpModule.GetVmpModule().GetVmpParam();
			}
			else
			{
				OspPrintf(TRUE, FALSE, "[ProcVcsMcuVCModeChgReq] must has vmp module currently\n");
				break;						
			}

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
					m_tConf.m_tStatus.SetVmpParam(tVmpParam);
					AdjustVmpParam(&tVmpParam);
				}
				else
				{
					SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, 6*1000);
					tVmpState.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_START;
					g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpState);

					// 启动画面合成
					m_tConf.m_tStatus.SetVmpParam(tVmpParam);
					AdjustVmpParam(&tVmpParam, TRUE);
				}

			}
			else
			{
				OspPrintf(TRUE, FALSE, "[RestoreVCConf]Hasn't reserve vmp for conf(confidx:%d)\n",
					      m_byConfIdx);
			}
		}			
		break;
	case VCS_GROUPSPEAK_MODE:
	case VCS_GROUPTW_MODE:
		// 主席变为发言人
		{
			TMt tSpeaker = m_tConf.GetChairman();
			ChangeSpeaker(&tSpeaker, FALSE, FALSE);
		}
		break;
	default:
		VcsLog("[RestoreVCConf] uncorrect vcmode(%d)\n", byNewMode);
	    break;
	}

	m_cVCSConfStatus.SetCurVCMode(byNewMode);

	// 通告给所有的VCS会议状态的改变
	VCSConfStatusNotif();

	VcsLog("[RestoreVCConf] restore vcsconf for mode(%d) end\n", byNewMode);

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
		EqpLog("[SwitchToAutoMixing] fail to automixing because conf mix status is %d, not in mixing\n",
			   m_tConf.m_tStatus.GetMixerMode());
		return FALSE;
	}

	m_tConf.m_tStatus.SetAutoMixing();
	TMt tMt;
	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
		{
			tMt = m_ptMtTable->GetMt(byMtId);
			AddMixMember(&tMt);
			StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
								 (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())), 
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
		tMt = m_ptMtTable->GetMt(tMt.GetMcuId());
	}

    VcsLog("[VCSMTMute] TMt(mcuid:%d mtid:%d) OpenFlag:%d MuteType:%d\n", tMt.GetMcuId(), tMt.GetMtId(), byMuteOpenFlag, byMuteType);

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
u8 CMcuVcInst::GetVCSAutoVmpMember(TMt* ptVmpMember)
{
    // 获取会议最大风格
    u8 byVmpCapChlNum = CMcuPfmLmt::GetMaxCapVMPByConfInfo(m_tConf);
	// 占用的vmp设备所能支持的最大风格
    TPeriEqpStatus tPeriEqpStatus;
    g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
    u8 byEqpMaxChl = tPeriEqpStatus.m_tStatus.tVmp.m_byChlNum;
	// 当前最多支持进入画面合成的终端数
	u8 byMaxMemberNum  = min(byVmpCapChlNum, byEqpMaxChl); 

	u8 byVmpStyle = GetVmpDynStyle(byMaxMemberNum);

// 	if (m_tVmpEqp.GetEqpId() > (VMPID_MIN + 8))
// 	{
// 		byVmpStyle = VMP_STYLE_FOUR;
// 	}

	if (VMP_STYLE_NONE == byVmpStyle)
	{
		return byVmpStyle;
	}

	TVMPParam tVmpParam;
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


	TVMPParam  tLastVMPParam   = m_tConf.m_tStatus.GetVmpParam(); 
	EqpLog("[GetVCSAutoVmpMember]last vmp param: \n");
	if(g_bPrintEqpMsg)
	{
		tLastVMPParam.Print();
	}

	// 保留之前设置的画面合成成员
	TMt tMt;
	u8 byLoop = 0;
	TVMPMember *ptMember = NULL;  
	u8 byLastMemNum = tLastVMPParam.GetMaxMemberNum();
	for (; byLoop < byLastMemNum; byLoop++)
	{
		tMt.SetNull();
		ptMember = tLastVMPParam.GetVmpMember(byLoop);
		memcpy(&tMt, ptMember, sizeof(TMt));
		if (tMt.IsNull() || tMt.GetMtId() > MAXNUM_CONF_MT)
		{
			continue;
		}

		if (m_tConf.GetChairman() == tMt)
		{
			continue;
		}
		// 防止高清电视墙放入画面合成中
		// 对于VCS会议定时呼叫的资源为固定资源
		if (CONF_CALLMODE_TIMER == VCSGetCallMode(tMt))
		{
			continue;
		}
		
		// 非回传通道中的下级终端不进画面合成成员统计
		if (!tMt.IsLocal())
		{
			TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMcuId());
			if (!(ptMcInfo->m_tMMcuViewMt == tMt))
			{
				continue;
			}
		}

		if (m_tConfAllMtInfo.MtJoinedConf(tMt.GetMcuId(), tMt.GetMtId()))
		{
			atVmpMember[byChnlNum++] = tMt;
		}
	}

	u8 byMtId    = 1;
	u8 byMaxMtId = MAXNUM_CONF_MT;
	if (ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
	{
		// 对于组呼模式下画面合成成员为上一次画面合成成员 + 当前调度终端成员
		TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();
		if (tCurVCMT.IsNull() || !m_tConfAllMtInfo.MtJoinedConf(tCurVCMT.GetMcuId(), tCurVCMT.GetMtId()))
		{
			byMaxMtId = 0;
		}
		else
		{
			tCurVCMT = GetLocalMtFromOtherMcuMt(tCurVCMT);
			byMtId = tCurVCMT.GetMtId();
			byMaxMtId = byMtId;
		}
	}
	for (; byMtId <= byMaxMtId; byMtId++)
	{
		if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
		{
			if (byMtId == m_tConf.GetChairman().GetMtId()
			 || tLastVMPParam.IsMtInMember(m_ptMtTable->GetMt(byMtId))
			 || (CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtId) && 
				 m_ptMtTable->GetMtType(byMtId) != MT_TYPE_SMCU)
			 // 若终端后向通道未打开，不让白占画面合成器通道
			 || !m_ptMtTable->IsLogicChnnlOpen(byMtId, LOGCHL_VIDEO, FALSE))
			{
				continue;
			}

			if (MT_TYPE_SMCU == m_ptMtTable->GetMtType(byMtId))
			{
				TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(byMtId);
				if (ptMcInfo != NULL && !ptMcInfo->m_tMMcuViewMt.IsNull() &&
					m_tConfAllMtInfo.MtJoinedConf(ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId()) &&
					!tLastVMPParam.IsMtInMember(ptMcInfo->m_tMMcuViewMt))
				{
					atVmpMember[byChnlNum++] = ptMcInfo->m_tMMcuViewMt;
				}
			}
			else
			{
				atVmpMember[byChnlNum++] = m_ptMtTable->GetMt(byMtId);
			}

		}
	}

	// 确保新成员加入画面合成
	if (byChnlNum > byMaxMemberNum )
	{
		u16 wChanInd = m_cVCSConfStatus.GetCurUseVMPChanInd();		
		for (u8 byIdx = 0; byIdx < (byChnlNum - byMaxMemberNum); byIdx++)
		{
			wChanInd += byIdx;
			if (wChanInd >= byMaxMemberNum)
			{
				wChanInd = 1;
			}

			atVmpMember[wChanInd] = atVmpMember[byMaxMemberNum + byIdx];
			atVmpMember[byMaxMemberNum + byIdx].SetNull();
		}

	}

	u8 byFinalMemNum = min(byChnlNum, byMaxMemberNum);
	if (ptVmpMember != NULL)
	{
		memset(ptVmpMember, 0, sizeof(TMt) * MAXNUM_VMP_MEMBER);
		memcpy(ptVmpMember, atVmpMember, sizeof(TMt) * byFinalMemNum);
		EqpLog("[GetVCSAutoVmpMember] final vcs autovmp member:\n");
		if (g_bPrintEqpMsg)
		{
			for(u8 byIdx = 0; byIdx < byFinalMemNum; byIdx++)
			{
				OspPrintf(TRUE, FALSE, "%d.mcuid:%d mtid:%d\n", 
						  byIdx, ptVmpMember[byIdx].GetMcuId(), ptVmpMember[byIdx].GetMtId());
			}
		}

	}



	byVmpStyle = GetVmpDynStyle(byFinalMemNum);
	EqpLog("[GetVCSAutoVmpMember] final autovmp style: %d\n", byVmpStyle);

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
	TVMPParam  tNewVMPParam = m_tConf.m_tStatus.GetVmpParam();

	// 将新组织的成员码流交换到对应的通道中
	for (byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++)
	{
		TMt tMt = pVmpMember[byLoop];
		if (!tMt.IsNull())
		{
			SetVmpChnnl(tMt, byLoop, VMP_MEMBERTYPE_VAC);
			
			tNewVMPParam = m_tConf.m_tStatus.GetVmpParam();
			// 调整终端的分辨率
			ChangeMtVideoFormat(tMt, &tNewVMPParam);
		}
		else
		{
			// 对原残留终端
			TMt tOldMt = *(TMt*)tLastVMPParam.GetVmpMember(byLoop);
			if (!tOldMt.IsNull())
			{
				StopSwitchToPeriEqp(m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO);
			}
			
		}
	}

	// 挂断被踢出画面合成器的成员
	CServMsg cServMsg;
	if ( CONF_VMPMODE_NONE != tLastVMPParam.GetVMPMode() )
	{
		for( s32 nIndex = 0; nIndex < tLastVMPParam.GetMaxMemberNum(); nIndex ++ )
		{
			TMt tVMPMemberOld = *(TMt *)tLastVMPParam.GetVmpMember(nIndex);
        
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
	if (VCS_CONF == m_tConf.GetConfSource() && 
		m_byCreateBy != CONF_CREATE_MT &&
		!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
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
void CMcuVcInst::ChangeVFormat(TMt tMt, u8 byNewFormat)
{
    if (tMt.IsNull() || tMt.GetType() != TYPE_MT || m_ptMtTable->GetManuId(tMt.GetMtId()) != MT_MANU_KDC )
    {
        VcsLog("[ChangeHDVFormat] dstmt.%d is not mt or kdc mt. type.%d manu.%d\n",
                tMt.GetMtId(), tMt.GetType(), m_ptMtTable->GetManuId(tMt.GetMtId()));
        return;
    }
    // 对于高清终端放入标清电视墙,采取降分辨率
    TLogicalChannel tSrcRvsChannl;
    if ( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tSrcRvsChannl, FALSE ) )
    {
		u8 byChnnlType   = tSrcRvsChannl.GetMediaType();
		u8 byChnnlFormat = byNewFormat;

		CServMsg cServMsg;
		cServMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
		cServMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
		cServMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
		SendMsgToMt( tMt.GetMtId(), cServMsg.GetEventId(), cServMsg );

		VcsLog( "[ChangeHDVFormat] mt(%d) videoformat change to %d\n",
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
		OspPrintf(TRUE, FALSE, "[ChangeSpecMixMember]mix has not started\n");
		return;
	}

	TMt tMt;
	u8  byInMixMem = FALSE;
	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
		{
			continue;
		}

		byInMixMem = FALSE;
		for (u8 byMtIdx = 0; byMtIdx < byMemberNum; byMtIdx++)
		{
			if (ptMixMember[byMtIdx].GetMtId() == byMtId)
			{
				byInMixMem = TRUE;
				break;
			}
		}

		tMt = m_ptMtTable->GetMt(byMtId);
		if (m_ptMtTable->IsMtInMixGrp(byMtId) && !byInMixMem)
		{
			AddRemoveSpecMixMember(&tMt, 1, FALSE);
		}
		
		if (!m_ptMtTable->IsMtInMixGrp(byMtId) && byInMixMem)
		{
			AddRemoveSpecMixMember(&tMt, 1, TRUE);
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

	u8  byStartMtId   = byMtId;
	u8  byLoopRestart = FALSE; 
	TMt tNextPollMt;
	tNextPollMt.SetNull();
    for (; byMtId <= MAXNUM_CONF_MT; byMtId++)
	{
		if (byStartMtId == byMtId && byLoopRestart)
		{
			break;
		}

		if (m_tConfAllMtInfo.MtJoinedConf((byMtId)) && 
			m_tConf.GetChairman().GetMtId() != byMtId && 
			m_ptMtTable->GetCallMode(byMtId) != CONF_CALLMODE_TIMER)
		{
			tNextPollMt = m_ptMtTable->GetMt(byMtId);
			break;
		}

		if (MAXNUM_CONF_MT == byMtId)
		{
			byMtId = 0;
			byLoopRestart = TRUE;
		}
	}
	
	return tNextPollMt;
}

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
		ConfLog(FALSE, "[IsNeedHdSelApt] wrong input param about bySwitchMode(%d)\n", bySwitchMode);
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
			ConfLog(FALSE, "[IsNeedHdSelApt] polocom no need to use bas\n");
			return FALSE;
		}
		// 若格式不一致
		else if (tSrcLogChnl.GetChannelType() != tDstLogChnl.GetChannelType())
		{
			ConfLog(FALSE, "[IsNeedHdSelApt] src type.%d matchless with dst type.%d, ocuupy new adp!\n",
				    tSrcLogChnl.GetChannelType(), tDstLogChnl.GetChannelType());
            return TRUE;
		}
		else if (IsResG(tSrcLogChnl.GetVideoFormat(), tDstLogChnl.GetVideoFormat()))
		{
			ConfLog(FALSE, "[IsNeedHdSelApt] src res.%d matchless with dst res.%d, ocuupy new adp!\n",
				    tSrcLogChnl.GetVideoFormat(), tDstLogChnl.GetVideoFormat());
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
			ConfLog(FALSE, "[IsNeedHdSelApt] src audio type.%d matchless with dst audio type.%d, ocuupy new adp!\n",
				    tSrcLogChnl.GetChannelType(), tDstLogChnl.GetChannelType());
			return TRUE;
		}
	}
	return FALSE;
}

