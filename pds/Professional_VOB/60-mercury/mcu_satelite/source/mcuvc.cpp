/*****************************************************************************
   模块名      : mcu
   文件名      : mcuvc.cpp
   相关文件    : mcuvc.h
   文件实现功能: MCU业务应用类消息映射函数定义
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/07/25  0.9         李屹        创建
   2005/02/19  3.6         万春雷      级联修改、与3.5版本合并
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
#include "mcuerrcode.h"
#include "mtadpssn.h"
#include "mcuutility.h"
#include "mpssn.h"
#include "eqpssn.h"
#include "mpmanager.h"
#include "dcsssn.h"
#include "evmodem.h"
#include "vctopo.h"
#include "satconst.h"


#if defined(_VXWORKS_)
#include <vxWorks.h>
#include <usrLib.h>
#include <inetLib.h>
#endif
#if defined(_LINUX_)
#include "boardwrapper.h"
#else
#include "brddrvlib.h"
#endif

CMcuVcApp	        g_cMcuVcApp;	//MCU业务应用实例
CMpManager          g_cMpManager;   //MP管理类全局变量
CAgentInterface     g_cMcuAgent;    //mcu agent

extern u8              g_byMcuModemNum;
extern TMcuModemTopo   g_atMcuModemTopo[MAXNUM_MCU_MODEM];

BOOL32	g_bpIntMsg  = FALSE;
BOOL32  g_bpUnitMsg = FALSE;
API void showintmsg()
{
	g_bpIntMsg = !g_bpIntMsg;
}
API void showunitmsg()
{
	g_bpUnitMsg = !g_bpUnitMsg;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMcuVcInst::CMcuVcInst()
{
	// NEXTSTATE( STATE_IDLE );

	ClearVcInst();

	m_ptMtTable = NULL;
	m_ptSwitchTable = NULL;
	m_ptConfOtherMcTable = NULL;

    m_tPlayFileMediaInfo.clear();
}

CMcuVcInst::~CMcuVcInst()
{
    MCU_SAFE_DELETE(m_ptMtTable);
    MCU_SAFE_DELETE(m_ptSwitchTable);
    MCU_SAFE_DELETE(m_ptConfOtherMcTable);
}

/*====================================================================
    函数名      ：InstanceDump
    功能        ：重载打印信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 param, 打印状态过滤，0表示打印所有，
					STATE_SCHEDULED打印预约，STATE_MASTER
					打印主MCU会议
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/18    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::InstanceDump( u32 param )
{
	ConfLog( FALSE, "\n---------------Conf: %s-------------------\n",m_tConf.GetConfName() );
	switch( HIWORD16( param ) ) 
	{
	case DUMPCONFALL:

		ConfLog( FALSE, "\nInsID: %d  Create by.%d\n", GetInsID(), m_byCreateBy );

		m_tConf.Print();

		m_tConf.GetConfAttrb().Print();
   
		ShowConfMod();

		m_tConf.m_tStatus.Print();
        m_tConf.m_tStatus.GetConfMode().Print();
                
        if ( CurState() == STATE_ONGOING )
        {            
            if ( m_tConf.m_tStatus.GetProtectMode() != CONF_LOCKMODE_NONE)
            {
                OspPrintf(TRUE, FALSE, "\n");
                m_tConfProtectInfo.Print();
            }
            OspPrintf(TRUE, FALSE, "\n");
            m_tConfInStatus.Print();
            if ( m_tConfInStatus.IsPollSwitch())
            {
                m_tPollSwitchParam.Print();
            }

            if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
            {
                m_tConf.m_tStatus.GetVmpParam().Print();
            }		
            
            if ( m_tConf.m_tStatus.GetPollState() != POLL_STATE_NONE )
            {
                m_tConfPollParam.Print();
            }
            
            if ( m_tConf.m_tStatus.GetTvWallPollState() != POLL_STATE_NONE )
            {
                m_tTvWallPollParam.Print();
            }
            
			if ( m_tConf.m_tStatus.GethduPollState() != POLL_STATE_NONE )
			{
				m_tHduPollParam.Print();
			}
            ShowConfEqp();
        }        		
        
        //ShowConfMt();

		break;

	case DUMPCONFINFO:

		ConfLog( FALSE, "\nInsID: %d  Create by.%d\n", GetInsID(), m_byCreateBy );

		m_tConf.Print();

		// zgc, 2008-04-25, 增加会议发言人源的指定方式打印
		OspPrintf( TRUE, FALSE, "\nType of spec speaker src:\nAud: %d\nVid: %d\n", 
					m_bySpeakerAudSrcSpecType, m_bySpeakerVidSrcSpecType );

		m_tConf.GetConfAttrb().Print();

		m_tConf.m_tStatus.Print();		
		
		m_tConf.m_tStatus.GetConfMode().Print();
		
		if (CONF_LOCKMODE_NEEDPWD == m_tConf.m_tStatus.GetProtectMode())
		{
			OspPrintf( TRUE, FALSE, "\nAll the mc pwd status: \n");

			for( u8 byIdx = 1; byIdx <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIdx ++)
			{
				if (g_cMcuVcApp.IsMcConnected(byIdx))
				{
					OspPrintf( TRUE, FALSE, "MC.%d pwd passed.%d\n", byIdx, m_tConfProtectInfo.IsMcsPwdPassed(byIdx) );
				}
			}
		}

		// 对于VCS会议
		if (VCS_CONF == m_tConf.GetConfSource())
		{
			OspPrintf(TRUE, FALSE, "\nVCSConfStatus:\n");
			m_cVCSConfStatus.VCCPrint();
		}

		break;

	case DUMPCONFMT:

        {
            u16 wParam = LOWORD16(param);
            u8 byMtId = (u8)wParam;
            
            ShowConfMt(byMtId);
        }

		break;
        
    case DUMPCHGMTRES:
        {
            u16 wParam = LOWORD16(param);
            u8 byMtId = LOBYTE(wParam);
            u8 byRes = HIBYTE(param);
            
            if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
            {
                OspPrintf(TRUE, FALSE, "[InstanceDump] Mt.%d unexist in conf.%d, try another...\n", byMtId, m_byConfIdx);
                return;
            }
            u8 byChnnlType = LOGCHL_VIDEO;

            CServMsg cServMsg;
            cServMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
            cServMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
            cServMsg.CatMsgBody( &byRes, sizeof(u8) );
            SendMsgToMt( byMtId, cServMsg.GetEventId(), cServMsg );
            
            OspPrintf(TRUE, FALSE, "[InstanceDump] chnnl type: %d, send videoformat<%d> change msg to mt<%d>!\n",
                byChnnlType, byRes, byMtId );
        }
        break;

	case DUMPBASINFO:
		
		{
			u16 wParam = LOWORD16(param);
            u8 byBasId = (u8)wParam;

			ShowBasInfo(byBasId);
		}
		break;

	case DUMPMTMONITOR:

		ShowMtMonitor();

		break;
		
	case DUMPCONFSWITCH:

		ShowConfSwitch();

		break;

	case DUMPCONFEQP:

		ShowConfEqp();

		break;

	case DUMPMTSTAT:

		ShowMtStat();

		break;
	case DUMPVMPADAPT:
		 ShowVmpAdaptMember();
		 break;
	default:
		break;
	}
}

/*====================================================================
    函数名      ：DaemonInstanceDump
    功能        ：重载打印信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 param
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/09    1.0         JQL            创建
	04/03/10    3.0         胡昌威         修改
====================================================================*/
void CMcuVcInst::DaemonInstanceDump( u32 param )
{
	u8   byIndex;
	TLogicalChannel tLogicalChannel;
	CConfId       cConfId;
	u32           dwVSndIp = 0, dwASndIp = 0, dwVRcvIp = 0,dwARcvIp = 0;
	u16           wVSndPort = 0, wASndPort = 0, wVRcvPort = 0, wARcvPort = 0;
	TMt           tMt[4];
	BOOL32        bDcsExist = FALSE;

	cConfId.SetNull();

	OspPrintf(TRUE, FALSE,"\nData information in VC:\n");
	switch( HIWORD16( param ) )
	{
	case DUMPMCINFO: //MC
		if( LOWORD16( param ) == DUMPSRC )
		{
			//MCINST CHANNEL SNDASRC   SNDVSRC
			OspPrintf( TRUE, FALSE, "MCINST CHANNEL  SNDVSRC      SNDASRC   \n");
			OspPrintf( TRUE, FALSE, "------ ------- ----------   ----------\n");
			for( byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
			{
				if( g_cMcuVcApp.IsMcConnected( byIndex ) )
				{
					u8 byChannelNum;
					if( g_cMcuVcApp.GetMcLogicChnnl( byIndex, MODE_VIDEO, &byChannelNum, &tLogicalChannel ) )
					{
						while( byChannelNum-- != 0 )
						{
							if( g_cMcuVcApp.GetMcSrc( byIndex, &tMt[0], byChannelNum, MODE_VIDEO ) 
								|| g_cMcuVcApp.GetMcSrc( byIndex, &tMt[1], byChannelNum, MODE_AUDIO ) )
							{
								ConfLog( FALSE, "%6u %7u %3u-%3u-%2u %3u-%3u-%2u\n", byIndex, byChannelNum,
								tMt[0].GetMcuId(), tMt[0].GetMtId(), tMt[0].GetEqpId(),
								tMt[1].GetMcuId(), tMt[1].GetMtId(), tMt[1].GetEqpId() );
							}
						}
					}
				}
			}
		}
		else
		{
			OspPrintf( TRUE, FALSE, "MCINST SNDVIP   SNDVSTARTPORT SNDVIP   SNDVSTARTPORT MCSNDVCHNNL MCSIP   MCSSSRC  GRP\n");
			OspPrintf( TRUE, FALSE, "------ -------- ------------- -------- ------------- ---------- -------- -------- ---\n");
			
			for (byIndex = 1; byIndex < (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++)
			{
				if (g_cMcuVcApp.IsMcConnected(byIndex))
				{
					u8 byChannelNum;
					TMcsRegInfo	tMcsReg;
					if (g_cMcuVcApp.GetMcLogicChnnl(byIndex, MODE_VIDEO, &byChannelNum, &tLogicalChannel))
					{
						dwVSndIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
						wVSndPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
					}

					if (g_cMcuVcApp.GetMcLogicChnnl(byIndex, MODE_AUDIO, &byChannelNum, &tLogicalChannel))
					{	
						dwASndIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
						wASndPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
					}
					g_cMcuVcApp.GetMcsRegInfo(byIndex, &tMcsReg);

					OspPrintf(TRUE, FALSE, "%6u %.8x %13u %.8x %13u %10u %.8x %.8x %3d\n", 
						      byIndex, dwVSndIp, wVSndPort, dwASndIp, wASndPort, byChannelNum, tMcsReg.GetMcsIpAddr(), tMcsReg.GetMcsSSRC(), CMcsSsn::GetUserGroup(byIndex)); 
				}
			}
		}
		break;

	case DUMPEQPINFO: //Peri
		OspPrintf( TRUE, FALSE, "EQPID  SNDVIP   SPORT SNDAIP   SPORT CHNNUM RCVVIP   RPORT RCVAIP   RPORT CHNNUM\n");
		OspPrintf( TRUE, FALSE, "------ -------- ----- -------- ----- ------ -------- ----- -------- ----- ------\n");
		for( byIndex = 1; byIndex < MAXNUM_MCU_PERIEQP; byIndex++ )
		{
			u8 byFwdChannelNum;
			u8 byRvsChannelNum;

			if( g_cMcuVcApp.IsPeriEqpConnected( byIndex ) )
			{	
				if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byIndex, MODE_VIDEO, &byFwdChannelNum, &tLogicalChannel, TRUE) )
				{
					dwVSndIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
		            wVSndPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
				}

				if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byIndex, MODE_AUDIO, &byFwdChannelNum, &tLogicalChannel, TRUE) )
				{
					dwASndIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
		            wASndPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
				}

				if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byIndex, MODE_VIDEO, &byRvsChannelNum, &tLogicalChannel, FALSE) )
				{
					dwVRcvIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
		            wVRcvPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
				}

				if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byIndex, MODE_AUDIO, &byRvsChannelNum, &tLogicalChannel, FALSE) )
				{
					dwARcvIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
		            wARcvPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
				}
				OspPrintf( TRUE, FALSE, "%6u %.8x %5u %.8x %5u %6u %.8x %5u %.8x %5u %6u\n", byIndex,
				           dwVSndIp, wVSndPort, dwASndIp, wASndPort, byFwdChannelNum,
					       dwVRcvIp, wVRcvPort, dwARcvIp, wARcvPort, byRvsChannelNum);
			}
		}
        
        for( byIndex = 1; byIndex <= MAXNUM_MCU_DCS; byIndex ++ )
        {
            if ( g_cMcuVcApp.IsPeriDcsConnected(byIndex) )
            {
                bDcsExist = TRUE;
                break;
            }
        }
        if ( bDcsExist )
        {
            OspPrintf( TRUE, FALSE, "\nDCSID  DCSIP    DCSPORT\n");
            OspPrintf( TRUE, FALSE, "------ -------- -----\n");

            for( byIndex = 1; byIndex <= MAXNUM_MCU_DCS; byIndex ++ )
            {
                if ( g_cMcuVcApp.IsPeriDcsConnected(byIndex) )
                {
                    OspPrintf( TRUE, FALSE, "%6u %.8x %5u \n", byIndex,
                               g_cMcuVcApp.m_atPeriDcsTable[byIndex-1].m_dwDcsIp, 
                               g_cMcuVcApp.m_atPeriDcsTable[byIndex-1].m_wDcsPort);
                }
            }
        }
        OspPrintf( TRUE, FALSE, "________________________________________________________________________________\n");
		break;
	case DUMPEQPSTATUS:
		g_cMcuVcApp.EqpStatusShow((u8  )LOWORD16(param));
		break;
    case DUMPTEMPLATE:
        g_cMcuVcApp.ShowTemplate();
        break;
    case DUMPCONFMAP:
        g_cMcuVcApp.ShowConfMap();
        break;
    case DUMPRLSCONF:
        ReleaseConf( TRUE );
        NEXTSTATE( STATE_IDLE );
        break;
    case DUMPRLSBAS:
        m_cBasMgr.ReleaseHdBasChn();
        break;
    case DUMPBASINFO:
        g_cMcuVcApp.ShowBasInfo();
        break;
	default:
		break;
	}
}

/*====================================================================
    函数名      ：SendMsgToMt
    功能        ：给会议中的下级终端发送消息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId. 接受消息的终端ID号，该终端的数据必须在会议实例的终端数据区中，且有效。
				  u16 wEvent. 事件号
				  u8 byMtId 终端Id号
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
  03/12/09      3.0			JQL			  创建
====================================================================*/
BOOL32 CMcuVcInst::SendMsgToMt( u8 byMtId, u16 wEvent, CServMsg & cServMsg )
{
	if (byMtId == 0)
	{
		ConfLog( FALSE, "Send Message(%s) To Mt.%d not exist.\n", OspEventDesc(wEvent), byMtId );
		return FALSE;
	}
    else if (byMtId == CONF_CREATE_MT)
    {
        // guzh [7/31/2006] 
        // 如果是创会消息，当时MtId还没有赋值。需要直接取其接入DriId
        CallLog("Send msg to Mt Dri.%d, event: %s(%d)\n", 
                cServMsg.GetSrcDriId(), 
                OspEventDesc(wEvent),
                wEvent);

		cServMsg.SetMcuId( LOCAL_MCUID );
    
    #ifdef _SATELITE_
        return g_cMtSsnApp.SendMsgToMtSsn( byMtId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
    #else
        return g_cMtAdpSsnApp.SendMsgToMtAdpSsn( cServMsg.GetSrcDriId(), wEvent, cServMsg );
    #endif
    
    }
	else
	{		
        TMt tDstMt = m_ptMtTable->GetMt( byMtId );
        
		if( GetInsID() != CInstance::DAEMON )
		{
			cServMsg.SetConfId( m_tConf.GetConfId() );
			cServMsg.SetConfIdx( m_byConfIdx );
		}

        if (MCU_MT_FLOWCONTROL_CMD == wEvent)
        {
            TLogicalChannel tLogicalChannel = *(TLogicalChannel *)cServMsg.GetMsgBody();     
            u16 wBitrate = 0;
            
            if (MODE_VIDEO == tLogicalChannel.GetMediaType())
            {
                //是否需要调整码率
                if (IsNeedAdjustMtSndBitrate(byMtId, wBitrate) && wBitrate < tLogicalChannel.GetFlowControl())
                {
                    Mt2Log("[SendFlowctrl2Mt] mt.%d, mode %d, old flowcontrol :%d, min flowcontrol :%d\n",
                            byMtId, 
                            tLogicalChannel.GetMediaType(), tLogicalChannel.GetFlowControl(), wBitrate);                   
                    
                    tLogicalChannel.SetFlowControl(wBitrate);
                    cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));                                        
                }

				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

                // xsl [8/1/2006] 终端发送码率大于4M通知终端按4M发送码流
                if ( byVmpSubType == VMP && // xliang [4/20/2009] MPU不做该限制
					tLogicalChannel.GetFlowControl() > MAXBITRATE_MTSEND_INVMP &&
                    (m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tDstMt) || 
                    m_tConf.m_tStatus.GetVmpTwParam().IsMtInMember(tDstMt)))
                {
                    tLogicalChannel.SetFlowControl(MAXBITRATE_MTSEND_INVMP); 
                    cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel)); 
                    
                    Mt2Log("[SendFlowctrl2Mt] mt in vmp, mode %d, old flowcontrol :%d, min flowcontrol :%d\n",
                            tLogicalChannel.GetMediaType(), tLogicalChannel.GetFlowControl(), wBitrate);  
                }

                // xsl [8/7/2006] 若在回传通道内，则发送码率应小于上级mcu接收带宽
                if( !m_tCascadeMMCU.IsNull() )
	            {
                    wBitrate = m_ptMtTable->GetMtReqBitrate(m_tCascadeMMCU.GetMtId(), TRUE);
		            TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
		            if( ptConfMcInfo != NULL && !ptConfMcInfo->m_tSpyMt.IsNull() )
		            {
			            if( tLogicalChannel.GetFlowControl() > wBitrate &&
                            ptConfMcInfo->m_tSpyMt.GetMtId() == tDstMt.GetMtId() &&
				            ptConfMcInfo->m_tSpyMt.GetMcuId() == tDstMt.GetMcuId() )
                        {
                            tLogicalChannel.SetFlowControl(wBitrate);
                            cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel)); 
                            
                            Mt2Log("[SendFlowctrl2Mt] mt in spying, mode %d, old flowcontrol :%d, min flowcontrol :%d\n",
                            tLogicalChannel.GetMediaType(), tLogicalChannel.GetFlowControl(), wBitrate);  
                        }
                    }
                }
                
                //是否小于发送带宽
                wBitrate = m_ptMtTable->GetSndBandWidth(byMtId);
                if (wBitrate > 0 && wBitrate <= m_tConf.GetBitRate() && wBitrate < tLogicalChannel.GetFlowControl())
                {                   
                    Mt2Log("[SendFlowctrl2Mt] mode %d, old flowcontrol :%d, bandwidth :%d\n",
                            tLogicalChannel.GetMediaType(), tLogicalChannel.GetFlowControl(), wBitrate);
                    
                    tLogicalChannel.SetFlowControl(wBitrate);
                    cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));     
                }                
            }   
            else if (MODE_SECVIDEO == tLogicalChannel.GetMediaType())
            {
                if(!m_tDoubleStreamSrc.IsNull())
			    {
                    u16 wMinBitRate = GetLeastMtReqBitrate(FALSE, MEDIA_TYPE_NULL, m_tDoubleStreamSrc.GetMtId()); 
				    if (tLogicalChannel.GetFlowControl() > wMinBitRate)
                    {
                        tLogicalChannel.SetFlowControl(wMinBitRate);
                        cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));     

                        Mt2Log("[SendFlowctrl2Mt] mode %d, old flowcontrol :%d, min flowcontrol :%d\n",
                            tLogicalChannel.GetMediaType(), tLogicalChannel.GetFlowControl(), wMinBitRate); 
                    }
			    }
            }

            if (tLogicalChannel.GetFlowControl() == tLogicalChannel.GetCurrFlowControl() &&
                tLogicalChannel.GetFlowControl() > 64)  //兼容MT3.6的FlowControl处理
            {
                return TRUE;
            }
            else
            {
               tLogicalChannel.SetCurrFlowControl(tLogicalChannel.GetFlowControl());
            }
            
            u8 byChannel;
            if (MODE_VIDEO == tLogicalChannel.GetMediaType())
            {
                byChannel = LOGCHL_VIDEO;
            }
            else if (MODE_AUDIO == tLogicalChannel.GetMediaType())
            {
                byChannel = LOGCHL_AUDIO;
            }
            else if (MODE_SECVIDEO == tLogicalChannel.GetMediaType())
            {
                byChannel = LOGCHL_SECVIDEO;
            }
            else
            {
                return FALSE;
            }
            m_ptMtTable->SetMtLogicChnnl(byMtId, byChannel, &tLogicalChannel, FALSE);

            Mt2Log("[SendFlowctrl2Mt] Mt.%d(%s), bitrate: %d, mode :%d\n", 
                    byMtId, StrOfIP(tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr()), 
                    tLogicalChannel.GetFlowControl(), tLogicalChannel.GetMediaType());
        }
        else if(MCU_MT_YOUARESEEING_NOTIF == wEvent)
        {
            TMt tSrcMt = *(TMt *)cServMsg.GetMsgBody();            
            Mt2Log("[SendMsg] send msg %s to Mt.%d, SrcMt(mtid:%d, mtmaintype:%d, mtsubtype:%d)\n",
                    OspEventDesc(wEvent), byMtId, tSrcMt.GetMtId(), tSrcMt.GetType(), tSrcMt.GetMtType());         
        }
        else if ( MCU_MT_FASTUPDATEPIC_CMD == wEvent )
        {
            u8 byMode = *(cServMsg.GetMsgBody());

#ifdef _SATELITE_
			wEvent = 25011/*MCU_MT_SENDIFRAME_NOTIF*/;
			cServMsg.SetDstMtId(tDstMt.GetMtId());
			//cServMsg.SetConfIdx((u8)GetInsID());
#endif
            Mt2Log("[SendMsg] send msg %s to Mt.%d for mode.%d\n",
                    OspEventDesc(wEvent), byMtId, byMode );   
        }
		// xliang [4/16/2009] print test
        else if ( MCU_MCU_ADJMTRES_REQ == wEvent )
		{	
			TMt tMt = *(TMt *)cServMsg.GetMsgBody();
			Mt2Log("[SendMsg] send msg %s to Mt.%d to ask Mt(%u,%u) adjust Video resolution.\n",
				OspEventDesc(wEvent), byMtId, tMt.GetMcuId(), tMt.GetMtId());
		}
		else if ( MCU_MT_INVITEMT_REQ == wEvent )
		{
	#ifdef _SATELITE_
			u8 byConfSecVid = 0;
			if ( MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() &&
				 0 != m_tConf.GetSecVideoMediaType() )
			{
				byConfSecVid = 1;
			}
			cServMsg.CatMsgBody(&byConfSecVid, sizeof(u8));
	#endif
		}
		cServMsg.SetMcuId( LOCAL_MCUID );
        
    #ifdef _SATELITE_
        return g_cMtSsnApp.SendMsgToMtSsn( tDstMt.GetMtId(), wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
    #else
        return g_cMtAdpSsnApp.SendMsgToMtAdpSsn( tDstMt, wEvent, cServMsg );
    #endif
	}
}

/*====================================================================
    函数名      ：SendMsgToEqp
    功能        ：发消息给外设
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
				  u16 wEvent. 事件号
				  u8 byEqpId 外设Id号
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/26    3.0         胡昌威         创建
====================================================================*/
BOOL32 CMcuVcInst::SendMsgToEqp( u8 byEqpId, u16 wEvent, CServMsg & cServMsg )
{
	if( byEqpId == 0 )
	{
		ConfLog( FALSE, "Send Message.%d<%s> To Eqp %d not exist.\n", 
                 wEvent, OspEventDesc(wEvent), byEqpId );
		return FALSE;
	}
	else
	{
		if( GetInsID() != CInstance::DAEMON )
		{
			cServMsg.SetConfId( m_tConf.GetConfId() );
			cServMsg.SetConfIdx( m_byConfIdx );
		}
		cServMsg.SetMcuId( LOCAL_MCUID );       

		return g_cEqpSsnApp.SendMsgToPeriEqpSsn( byEqpId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );		 
	}	
}

/*====================================================================
    函数名      ：SendMsgToMcs
    功能        ：发消息给会控
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
				  u16 wEvent. 事件号
				  u8 byMcsId 会控Id号
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/26    3.0         胡昌威         创建
====================================================================*/
BOOL32 CMcuVcInst::SendMsgToMcs( u8 byMcsId, u16 wEvent, CServMsg & cServMsg )
{
	if( byMcsId == 0 )
	{
		ConfLog( FALSE, "Send Message.%d(%s) To Mcs.%d not exist.\n", wEvent, OspEventDesc(wEvent), byMcsId );
		return FALSE;
	}
	else
	{
		if( GetInsID() != CInstance::DAEMON )
		{
			cServMsg.SetConfId( m_tConf.GetConfId() );
			cServMsg.SetConfIdx( m_byConfIdx );
		}
		cServMsg.SetMcuId( LOCAL_MCUID );
		return CMcsSsn::SendMsgToMcsSsn( byMcsId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );		 
	}
}

/*====================================================================
    函数名      ：SendMsgToAllMcs
    功能        ：发消息给所有会控
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
				  u16 wEvent. 事件号
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/26    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::SendMsgToAllMcs( u16 wEvent, CServMsg & cServMsg )
{
	if( GetInsID() != CInstance::DAEMON )
	{
		cServMsg.SetConfIdx( m_byConfIdx );
		cServMsg.SetConfId( m_tConf.GetConfId() );
	}
	cServMsg.SetMcuId( LOCAL_MCUID );
	CMcsSsn::BroadcastToAllMcsSsn( wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
}

/*====================================================================
    函数名      ：SendMsgToAllMp
    功能        ：发消息给所有MP
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wEvent. 事件号
                  CServMsg & cServMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/04/27    3.6         libo          创建
====================================================================*/
void CMcuVcInst::SendMsgToAllMp(u16 wEvent, CServMsg & cServMsg)
{
	if (GetInsID() != CInstance::DAEMON)
	{
		cServMsg.SetConfIdx(m_byConfIdx);
		//cServMsg.SetConfId( m_tConf.GetConfId() );
	}
	cServMsg.SetMcuId(LOCAL_MCUID);
	g_cMpSsnApp.BroadcastToAllMpSsn(wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
}

/*=============================================================================
  函 数 名： SendMsgToDcsSsn
  功    能： 发消息给DCS会话的某个实例
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： BOOL32
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/16    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::SendMsgToDcsSsn( u8 byInst, CServMsg &cServMsg )
{
	u32 dwRet = OspPost( MAKEIID(AID_MCU_DCSSSN, byInst), cServMsg.GetEventId(), 
						 cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), 0, 
						 MAKEIID(GetAppID(), GetInsID()) );
	if ( OSP_OK != dwRet )
	{
        ConfLog( FALSE, "send msg %d<%s> to dcsssn failed, ret:%d !\n", 
            cServMsg.GetEventId(), OspEventDesc( cServMsg.GetEventId() ), dwRet );
        return FALSE;
	}	

	return TRUE;
}

/*====================================================================
    函数名      ：BroadcastToAllSubMtJoinedConf
    功能        ：发广播消息到所有参加本会议的直连终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/03    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::BroadcastToAllSubMtJoinedConf( u16 wEvent, CServMsg & cServMsg )
{
	cServMsg.SetConfIdx( m_byConfIdx );
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetDstMtId( 0 );
	g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn( wEvent, cServMsg );
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
    02/07/25    1.0         LI Yi         创建
	03/11/17    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		ConfLog(FALSE, "[CMcuVcInst] The received msg's pointer in the msg InstanceEntry is NULL!");
		return;
	}

	//主备数据倒换时，[MSMag <->McuVc]模块互锁，锁定 2005-12-15
	g_cMSSsnApp.EnterMSSynLock(AID_MCU_VC);

	if ( pcMsg->event > MCUVC_FASTUPDATE_TIMER_ARRAY && pcMsg->event <= MCUVC_FASTUPDATE_TIMER_ARRAY + MAXNUM_CONF_MT )
	{
		ProcMcuMtFastUpdateTimer(pcMsg);
		
		//主备数据倒换时，[MSMag <->McuVc]模块互锁，解锁 2005-12-15
		g_cMSSsnApp.LeaveMSSynLock(AID_MCU_VC);
		return;
	}
    
    if ( pcMsg->event > MCUVC_FASTUPDATE_TIMER_ARRAY4EQP && pcMsg->event <= MCUVC_FASTUPDATE_TIMER_ARRAY4EQP + MAXNUM_PERIEQP*3)
    {
        ProcMcuEqpFastUpdateTimer(pcMsg);
        g_cMSSsnApp.LeaveMSSynLock(AID_MCU_VC);
        return;
    }
    
	switch( pcMsg->event )
	{
	case OSP_POWERON:				//上电消息
		break;
    
	case OSP_OVERFLOW:
        {
			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
			SendMsgToMcs(cServMsg.GetSrcSsnId(), cServMsg.GetEventId()+2, cServMsg);
        }
		break;
    
    /*----------------------------会议管理-------------------------------*/

	//会议管理---创建会议
	case MCS_MCU_CREATECONF_REQ:          //会议控制台在MCU上创建一个会议
	case MCU_CREATECONF_FROMFILE:		  //自身创建会议消息 
	case MCU_SCHEDULE_CONF_START:         //预约会议变为即时会议
	case MT_MCU_CREATECONF_REQ:		      //创建会议消息
    case MCU_CREATECONF_NPLUS:
		ProcMcsMcuCreateConfReq(pcMsg);
		break;
	case MCU_WAITEQP_CREATE_CONF_NOTIFY:
		ProcWaitEqpToCreateConf(pcMsg);
		break;
        
    case MCUVC_CONFSTARTREC_TIMER:        //自动录像
        ProcTimerAutoRec(pcMsg);
        break;
        
	//会议管理---结束会议
	case MCS_MCU_RELEASECONF_REQ:         //会议控制台请求MCU结束一个会议
	case MT_MCU_DROPCONF_CMD:		      //主席终端中止会议请求
		ProcMcsMcuReleaseConfReq(pcMsg);
		break;

	//会议管理---保护会议
	case MCS_MCU_CHANGECONFLOCKMODE_REQ:  //会议控制台请求MCU改变会议保护方式
		ProcMcsMcuChangeLockModeConfReq(pcMsg);
		break;
	case MCS_MCU_ENTERCONFPWD_ACK:        //会议控制台回应MCU的密码请求   
	case MCS_MCU_ENTERCONFPWD_NACK:       //会议控制台拒绝MCU的密码请求
		ProcMcsMcuEnterPwdRsp(pcMsg);
		break;
	case MCS_MCU_CHANGECONFPWD_REQ:       //会议控制台请求MCU改变会议密码
		ProcMcsMcuChangeConfPwdReq(pcMsg);
		break;
	case MCS_MCU_GETLOCKINFO_REQ:
		ProcMcsMcuGetLockInfoReq(pcMsg); //会控得到会议独享消息请求
		break;

	//会议管理---保存会议
	case MCS_MCU_SAVECONF_REQ:            //会议控制台请求MCU保存会议
		ProcMcsMcuSaveConfReq(pcMsg);
		break;

	//会议管理---修改会议
	case MCS_MCU_MODIFYCONF_REQ:	      //修改会议消息
		ProcMcsMcuModifyConfReq(pcMsg);
		break;

	//会议管理---延长会议
	case MT_MCU_DELAYCONF_REQ:			  //终端请求MCU延长会议
	case MCS_MCU_DELAYCONF_REQ:           //会议控制台请求MCU延长会议
		ProcMcsMcuDelayConfReq(pcMsg);
		break;

	//数据会议管理---来自DCS的应答 2005-12-16
	case DCSSSN_MCU_CREATECONF_ACK:
	case DCSSSN_MCU_CREATECONF_NACK:
	case DCSSSN_MCU_CONFCREATED_NOTIF:
		ProcDcsMcuCreateConfRsp(pcMsg);
		break;

	case MCS_MCU_CHANGEVACHOLDTIME_REQ:	  //会议控制台请求MCU改变语音激励切换时间
		ProcMcsMcuChangeVacHoldTimeReq(pcMsg);
		break;

    case NMS_MCU_SCHEDULE_CONF_NOTIF:
		ProcNmsMcuSchedConf( pcMsg );
        break; 

    case TIMER_SCHEDCONF:
        ProcSchedConfTimeOut( pcMsg );
		break;

    case NMS_MCU_APPLYCONFFREQ_NOTIF:
    case MCUVC_APPLYFREQUENCE_CHECK_TIMER:
        ProcGetFreqFromNMS( pcMsg );
        break;

	/*----------------------------会议操作-------------------------------*/

	//会议操作---主席
	case MCS_MCU_SPECCHAIRMAN_REQ:       //会议控制台指定一台终端为主席
	case MT_MCU_SPECCHAIRMAN_REQ:		 //主席终端指定主席请求
		ProcMcsMcuSpecChairmanReq(pcMsg);
		break;
	case MCS_MCU_CANCELCHAIRMAN_REQ:     //会议控制台取消当前会议主席 
	case MT_MCU_CANCELCHAIRMAN_REQ:		 //主席终端取消自己主席权限请求
		ProcMcsMcuCancelChairmanReq(pcMsg);
		break;
    case MCS_MCU_SETCHAIRMODE_CMD:       //会议控制台设置会议的主席方式
        ProcMcsMcuSetConfChairMode(pcMsg);
		break;

	//会议操作---发言
	case MCS_MCU_SPECSPEAKER_REQ:        //会议控制台指定一台终端发言
	case MT_MCU_SPECSPEAKER_REQ:		 //普通终端指定发言人请求
	case MT_MCU_SPECSPEAKER_CMD:		 //主席终端指定发言人请求
		ProcMcsMcuSpecSpeakerReq(pcMsg);
		break;
	case MCS_MCU_CANCELSPEAKER_REQ:      //会议控制台取消会议Speaker
	case MT_MCU_CANCELSPEAKER_CMD:		 //主席终端取消发言人请求
		ProcMcsMcuCancelSpeakerReq(pcMsg);
		break;
	case MCS_MCU_MTSEESPEAKER_CMD:
    case MT_MCU_VIEWBRAODCASTINGSRC_CMD:    //强制广播
		ProcMcsMcuSeeSpeakerCmd(pcMsg);
		break;
        
    case MCS_MCU_SPECOUTVIEW_REQ:       //会议控制台指定回传通道
        ProcMcsMcuSpecOutViewReq(pcMsg);
        break;        

    //级联MCU
	case MCU_MCU_REGISTER_NOTIF:
		ProcMcuMcuRegisterNotify(pcMsg);
		break;
	case MCU_MCU_ROSTER_NOTIF:
		ProcMcuMcuRosterNotify(pcMsg);
		break;
	case MCU_MCU_MTLIST_REQ:
		ProcMcuMcuMtListReq(pcMsg);
		break;
	case MCU_MCU_MTLIST_ACK:
		ProcMcuMcuMtListAck(pcMsg);
		break;
	case MCU_MCU_VIDEOINFO_REQ:
		ProcMcuMcuVideoInfoReq(pcMsg);
		break;
	case MCU_MCU_VIDEOINFO_ACK:
		ProcMcuMcuVideoInfoAck(pcMsg);
		break;
	case MCU_MCU_CONFVIEWCHG_NOTIF:
		ProcMcuMcuConfViewChgNtf(pcMsg);
		break;
	case MCU_MCU_AUDIOINFO_REQ:
		ProcMcuMcuAudioInfoReq(pcMsg);
		break;
	case MCU_MCU_AUDIOINFO_ACK:
		ProcMcuMcuAudioInfoAck(pcMsg);
        break;
	case MCU_MCU_NEWMT_NOTIF:
		ProcMcuMcuNewMtNotify(pcMsg);
		break;
	case MCU_MCU_CALLALERTING_NOTIF:
		ProcMcuMcuCallAlertMtNotify(pcMsg);
		break;
	case MCU_MCU_DROPMT_NOTIF:
		ProcMcuMcuDropMtNotify(pcMsg);
		break;
	case MCU_MCU_DELMT_NOTIF:
		ProcMcuMcuDelMtNotify(pcMsg);
		break;
	case MCU_MCU_SETMTCHAN_REQ:
		ProcMcuMcuSetMtChanReq(pcMsg);
		break;
	case MCU_MCU_SETMTCHAN_NOTIF:
		ProcMcuMcuSetMtChanNotify(pcMsg);
		break;
	case MCU_MCU_SETIN_REQ:
		ProcMcuMcuSetInReq(pcMsg);
		break;
	case MCU_MCU_SETOUT_REQ:
		ProcMcuMcuSetOutReq(pcMsg);
		break;
	case MCU_MCU_SETOUT_NOTIF:
		ProcMcuMcuSetOutNotify(pcMsg);
		break;

	case MCU_MCU_STARTMIXER_CMD:
		ProcMcuMcuStartMixerCmd(pcMsg);
		break;
/*		
	case MCU_MCU_STARTMIXER_REQ:
		ProcMcuMcuStartMixerReq(pcMsg);
		break;
	case MCU_MCU_STARTMIXER_ACK:
		ProcMcuMcuStartMixerAck(pcMsg);
		break;
	case MCU_MCU_STARTMIXER_NACK:
		ProcMcuMcuStartMixerNack(pcMsg);
		break;
*/
	case MCU_MCU_STARTMIXER_NOTIF:
		ProcMcuMcuStartMixerNotif(pcMsg);
		break;
	case MCU_MCU_STOPMIXER_CMD:
		ProcMcuMcuStopMixerCmd(pcMsg);
		break;
	/*
	case MCU_MCU_STOPMIXER_REQ:
		ProcMcuMcuStopMixerReq(pcMsg);
		break;
	case MCU_MCU_STOPMIXER_ACK:
		ProcMcuMcuStopMixerAck(pcMsg);
		break;
	case MCU_MCU_STOPMIXER_NACK:
		ProcMcuMcuStopMixerNack(pcMsg);
		break;
	*/
	case MCU_MCU_STOPMIXER_NOTIF:
		ProcMcuMcuStopMixerNotif(pcMsg);
		break;
	case MCU_MCU_ADJMTRES_REQ:
		ProcMcuMcuAdjustMtResReq(pcMsg);
		break;
	case MCU_MCU_ADJMTRES_ACK:
		ProcMcuMcuAdjustMtResAck(pcMsg);
		break;
	case MCU_MCU_ADJMTRES_NACK:
		break;
	case MCU_MCU_GETMIXERPARAM_REQ:
		ProcMcuMcuGetMixerParamReq(pcMsg);
		break;
	case MCU_MCU_GETMIXERPARAM_ACK:
		ProcMcuMcuGetMixerParamAck(pcMsg);
		break;
	case MCU_MCU_GETMIXERPARAM_NACK:
		ProcMcuMcuGetMixerParamNack(pcMsg);
		break;
	case MCU_MCU_MIXERPARAM_NOTIF:
		ProcMcuMcuMixerParamNotif(pcMsg);
		break;
	case MCU_MCU_ADDMIXMEMBER_CMD:
		ProcMcuMcuAddMixerMemberCmd(pcMsg);
		break;
	case MCU_MCU_REMOVEMIXMEMBER_CMD:
		ProcMcuMcuRemoveMixerMemberCmd(pcMsg);
		break;
	case MCU_MCU_LOCK_REQ:
		ProcMcuMcuLockReq(pcMsg);
		break;
	case MCU_MCU_LOCK_ACK:
		ProcMcuMcuLockAck(pcMsg);
		break;
	case MCU_MCU_LOCK_NACK:
		ProcMcuMcuLockNack(pcMsg);
		break;
	case MCU_MCU_MTSTATUS_CMD:
		ProcMcuMcuMtStatusCmd(pcMsg);
		break;
	case MCU_MCU_MTSTATUS_NOTIF:
		ProcMcuMcuMtStatusNotif(pcMsg);
		break;
    case MCU_MCU_AUTOSWITCH_REQ:
        ProcMcuMcuAutoSwitchReq(pcMsg);
        break;
    case MCU_MCU_AUTOSWITCH_ACK:
    case MCU_MCU_AUTOSWITCH_NACK:           
        ProcMcuMcuAutoSwitchRsp(pcMsg);
        break;
    case MCUVC_AUTOSWITCH_TIMER:
        ProcMcuMcuAutoSwitchTimer(pcMsg);
        break;
		
	case MCS_MCU_ADDMT_REQ:              //会议控制台增加终端	
	case MT_MCU_ADDMT_REQ:			     //主席终端增加终端
	case MCU_MCU_INVITEMT_REQ:           //MCU-MCU增加终端
		ProcMcsMcuAddMtReq(pcMsg);
		break;

	case MCS_MCU_ADDMTEX_REQ:
		ProcMcsMcuAddMtExReq(pcMsg);     //调试版会控增加终端处理，含打开通道使用的能力级
		break;

    case MCU_MCU_INVITEMT_ACK:
    case MCU_MCU_INVITEMT_NACK:
        break;
	case MCS_MCU_DELMT_REQ:              //会议控制台驱逐终端离会
	case MT_MCU_DELMT_REQ:			     //主席终端强制终端退出请求
	case MCU_MCU_DELMT_REQ:			     //主席终端强制终端退出请求
		ProcMcsMcuDelMtReq(pcMsg);
		break;

	// 会议操作 --- hdu  //4.6.1 新加  jlb
    case MCS_MCU_START_SWITCH_HDU_REQ:
    case MCUVC_STARTSWITCHHDU_NOTIFY:
        ProcMcsMcuStartSwitchHduReq( pcMsg );
        break;

    case MCS_MCU_STOP_SWITCH_HDU_REQ:
		ProcMcsMcuStopSwitchHduReq( pcMsg );
		break;

    case MCS_MCU_CHANGEHDUVOLUME_REQ:
		ProcMcsMcuChangeHduVolume( pcMsg );
		break;

    //会议操作---选看
	case MT_MCU_STARTSELMT_CMD:			 //标准选看命令
	case MT_MCU_STARTSELMT_REQ:          //
	case MCS_MCU_STARTSWITCHMT_REQ:	     //控制台要求交换命令
	case MCS_MCU_STARTSWITCHMC_REQ:      //会控选看终端
		ProcMtMcuStartSwitchMtReq(pcMsg);
		break;
	case MCS_MCU_STARTSWITCHVMPMT_REQ:	 //主席终端选看画面合成 // xliang [12/31/2008]  
	case MT_MCU_STARTSWITCHVMPMT_REQ:	
		ProcMcsMcuStartSwitchVmpMtReq(pcMsg);
		break;
	case MCS_MCU_START_SWITCH_TW_REQ:    //会议控制台向电视墙开始播放请求
		ProcMcsMcuStartSwitchTWReq(pcMsg);
		break;
	case MT_MCU_STOPSELMT_CMD:			 //标准停止选看命令
	case MCS_MCU_STOPSWITCHMT_REQ:	     //控制台要求交换命令
		ProcMtMcuStopSwitchMtReq(pcMsg);
		break;

    //会议操作---短消息
	case MCS_MCU_SENDRUNMSG_CMD:         //会议控制台命令MCU向终端发送短消息，终端号为0表示发给所有终端
	case MT_MCU_SENDMSG_CMD:             //终端短消息请求
		ProcMcsMcuSendMsgReq(pcMsg);
		break;
	case MCU_MCU_SENDMSG_NOTIF:          //跨MCU的短消息
		ProcMcuMcuSendMsgReq(pcMsg);
		break;

	//会议操作---得到终端列表
	case MCS_MCU_GETMTLIST_REQ:          //会议控制台发给MCU的得到终端列表请求
        ProcMcsMcuGetMtListReq(pcMsg);
		break;
	case MCS_MCU_REFRESHMCU_CMD:
		ProcMcsMcuRefreshMcuCmd(pcMsg);
		break;
		//vmp抢占应答：// xliang [12/12/2008] 
	case MCS_MCU_VMPPRISEIZE_ACK:
	case MCS_MCU_VMPPRISEIZE_NACK:
		ProcMcsMcuVmpPriSeizeRsp(pcMsg);
		break;
	case MCUVC_MTSEIZEVMP_TIMER:
		ProcMtSeizeVmpTimer(pcMsg);
		break;
	case MCUVC_VMPBATCHPOLL_TIMER:
		ProcVmpBatchPollTimer(pcMsg);

	//会议操作---得到会议信息
	case MCS_MCU_GETCONFINFO_REQ:        //会议控制台向MCU查询会议信息 
	case MT_MCU_GETCONFINFO_REQ:         //终端向MCU查询会议信息
    case MCS_MCU_GETMAUSTATUS_REQ:       //会议控制台向MCU查询MAU信息 
        ProcMcsMcuGetConfInfoReq(pcMsg);
		break;

	//会议控制---视频复合控制
	case MCS_MCU_STARTVMP_REQ:			//会控开始视频复合请求
	case MCS_MCU_STOPVMP_REQ:			//会控结束视频复合请求	
	case MCS_MCU_CHANGEVMPPARAM_REQ:	//会控会议控制台请求MCU改变混音参数			
	case MCS_MCU_GETVMPPARAM_REQ:		//会控查询混音成员请求
	case MCS_MCU_STARTVMPBRDST_REQ:		//会议控制台命令MCU开始把画面合成图像广播到终端
	case MCS_MCU_STOPVMPBRDST_REQ:		//会议控制台命令MCU停止把画面合成图像广播到终端
	case MT_MCU_STARTVMP_REQ:			//主席开始视频复合请求
	case MT_MCU_STOPVMP_REQ:			//主席结束视频复合请求	
	case MT_MCU_CHANGEVMPPARAM_REQ:		//主席会议控制台请求MCU改变混音参数
	case MT_MCU_GETVMPPARAM_REQ:		//主席查询混音成员请求
	case MT_MCU_STARTVMPBRDST_REQ:		//主席命令MCU开始把画面合成图像广播到终端
	case MT_MCU_STOPVMPBRDST_REQ:		//主席命令MCU停止把画面合成图像广播到终端
	case MCS_MCU_START_VMPBATCHPOLL_REQ://开始画面合成批量轮询// xliang [12/31/2008] 
        ProcMcsMcuVMPReq(pcMsg);
		break;
	case MCS_MCU_PAUSE_VMPBATCHPOLL_CMD:	//暂停画面合成批量轮询// xliang [12/31/2008]  
	case MCS_MCU_RESUME_VMPBATCHPOLL_CMD:	//恢复画面合成批量轮询
	case MCS_MCU_STOP_VMPBATCHPOLL_CMD:		//停止画面合成批量轮询
		ProcMcsMcuVmpCmd(pcMsg);
		break;
	case VMP_MCU_STARTVIDMIX_ACK:       //VMP给MCU开始工作确认
	case VMP_MCU_STARTVIDMIX_NACK:      //VMP给MCU开始工作拒绝
	case VMP_MCU_STOPVIDMIX_ACK:        //VMP给MCU停止工作确认
	case VMP_MCU_STOPVIDMIX_NACK:       //VMP给MCU停止工作拒绝
	case VMP_MCU_CHANGEVIDMIXPARAM_ACK: //VMP给MCU改变复合参数确认
	case VMP_MCU_CHANGEVIDMIXPARAM_NACK://VMP给MCU改变复合参数拒绝
	case VMP_MCU_GETVIDMIXPARAM_ACK:    //VMP给MCU回送复合参数确认
	case VMP_MCU_GETVIDMIXPARAM_NACK:   //VMP给MCU回送复合参数拒绝
        ProcVmpMcuRsp(pcMsg);
		break;
    case VMP_MCU_STARTVIDMIX_NOTIF:     //VMP给MCU开始工作通知
    case VMP_MCU_STOPVIDMIX_NOTIF:      //VMP给MCU停止工作通知
    case VMP_MCU_CHANGESTATUS_NOTIF:    //VMP给MCU改变复合参数通知
    case MCU_VMPCONNECTED_NOTIF:
    case MCU_VMPDISCONNECTED_NOTIF:     //VMP断链通知
    case VMP_MCU_NEEDIFRAME_CMD:        //画面合成请求I帧
        ProcVmpMcuNotif(pcMsg); 
        break;
	case MCUVC_VMP_WAITVMPRSP_TIMER:      //MCU等待VMP应答定时
		ProcVmpRspWaitTimer(pcMsg);
		break;

    //会议控制---复合电视墙控制
	case MCS_MCU_STARTVMPTW_REQ:        //会控开始复合电视墙请求
	case MCS_MCU_STOPVMPTW_REQ:         //会控结束复合电视墙请求
	case MCS_MCU_CHANGEVMPTWPARAM_REQ:  //会控会议控制台请求MCU改变复合电视墙参数
//	case MCS_MCU_GETVMPPARAM_REQ:		//会控查询混音成员请求
//	case MCS_MCU_STARTVMPBRDST_REQ:		//会议控制台命令MCU开始把画面合成图像广播到终端
//	case MCS_MCU_STOPVMPBRDST_REQ:		//会议控制台命令MCU停止把画面合成图像广播到终端
        ProcMcsMcuVmpTwReq(pcMsg);
        break;
    case VMPTW_MCU_STARTVIDMIX_ACK:       //VMPTW给MCU开始工作确认
    case VMPTW_MCU_STARTVIDMIX_NACK:      //VMPTW给MCU开始工作拒绝
    case VMPTW_MCU_STOPVIDMIX_ACK:        //VMPTW给MCU停止工作确认
    case VMPTW_MCU_STOPVIDMIX_NACK:       //VMPTW给MCU停止工作拒绝
    case VMPTW_MCU_CHANGEVIDMIXPARAM_ACK: //VMPTW给MCU改变复合参数确认
    case VMPTW_MCU_CHANGEVIDMIXPARAM_NACK://VMPTW给MCU改变复合参数拒绝
//    case VMPTW_MCU_GETVIDMIXPARAM_ACK:    //VMPTW给MCU回送复合参数确认
//    case VMPTW_MCU_GETVIDMIXPARAM_NACK:   //VMPTW给MCU回送复合参数拒绝
        ProcVmpTwMcuRsp(pcMsg);
        break;
    case VMPTW_MCU_STARTVIDMIX_NOTIF:     //VMPTW给MCU开始工作通知
	case VMPTW_MCU_STOPVIDMIX_NOTIF:      //VMPTW给MCU停止工作通知
	case VMPTW_MCU_CHANGESTATUS_NOTIF:    //VMPTW给MCU改变复合参数通知
    case MCU_VMPTWCONNECTED_NOTIF:
	case MCU_VMPTWDISCONNECTED_NOTIF:     //VMPTW断链通知
	case VMPTW_MCU_NEEDIFRAME_CMD:        //VMPTW给MCU请求I帧
	    ProcVmpTwMcuNotif(pcMsg); 
		break;

	//会议控制---得到会议状态
	case MCS_MCU_GETCONFSTATUS_REQ:     //会议控制台向MCU查询会议状态
        ProcMcsMcuGetConfStatusReq(pcMsg);
		break;
	case MCS_MCU_MCUMEDIASRC_REQ:
		ProcMcsMcuMcuMediaSrcReq(pcMsg);
		break;
	case MCS_MCU_LOCKSMCU_REQ:
		ProcMcsMcuLockSMcuReq(pcMsg);
		break;	
	case MCS_MCU_GETMCULOCKSTATUS_REQ:
		ProcMcsMcuGetMcuLockStatusReq(pcMsg);
		break;

    //会议控制---语音激励控制发言
	case MCS_MCU_STARTVAC_REQ:        //会议控制台请求MCU开始语音激励控制发言		
	case MCS_MCU_STOPVAC_REQ:         //会议控制台请求MCU停止语音激励控制发言
	case MT_MCU_STARTVAC_REQ:         //终端请求MCU开始语音激励控制发言
	case MT_MCU_STOPVAC_REQ:          //终端请求MCU停止语音激励控制发言
        ProcMcsMcuVACReq(pcMsg);
		break;		
				
	//会议控制---混音控制
	case MCS_MCU_STARTDISCUSS_REQ:     //开始会议讨论请求 - 用于本级开始讨论操作
	case MCS_MCU_STOPDISCUSS_REQ:      //结束会议讨论请求 - 用于本级开始讨论操作

    //zbq[11/01/2007] 混音优化
    case MCS_MCU_GETMIXPARAM_REQ:      //会议控制台查询讨论参数请求
    //case MCS_MCU_STARTDISCUSS_CMD:     //开始会议讨论命令 - 用于跨级开始讨论操作
	//case MCS_MCU_STOPDISCUSS_CMD:      //结束会议讨论命令 - 用于跨级开始讨论操作
	//case MCS_MCU_GETDISCUSSPARAM_REQ:  //会议控制台查询讨论参数请求

        ProcMcsMcuMixReq(pcMsg);
		break;
	case MT_MCU_STARTDISCUSS_REQ:      //终端开始会议讨论请求
	case MT_MCU_STOPDISCUSS_REQ:       //终端请求MCU结束会议讨论
		ProcMtMcuMixReq(pcMsg);
		break;
        //混音延时设置
    case MCS_MCU_CHANGEMIXDELAY_REQ:
        ProcMcsMcuChgMixDelayReq(pcMsg);
        break;
	case MIXER_MCU_STARTMIX_ACK:        //同意开始混音应答
	case MIXER_MCU_STARTMIX_NACK:       //拒绝开始混音应答
	case MIXER_MCU_STOPMIX_ACK:         //同意停止混音应答
	case MIXER_MCU_STOPMIX_NACK:        //拒绝停止混音应答
	case MIXER_MCU_ADDMEMBER_ACK:       //加入成员应答消息
	case MIXER_MCU_ADDMEMBER_NACK:      //加入成员应答消息
	case MIXER_MCU_REMOVEMEMBER_ACK:    //删除成员应答消息
	case MIXER_MCU_REMOVEMEMBER_NACK:   //删除成员应答消息
	case MIXER_MCU_FORCEACTIVE_ACK:     //强制成员混音应答
	case MIXER_MCU_FORCEACTIVE_NACK:    //强制成员混音拒绝
	case MIXER_MCU_CANCELFORCEACTIVE_ACK:    //取消成员强制混音应答
	case MIXER_MCU_CANCELFORCEACTIVE_NACK:   //取消成员强制混音拒绝
		ProcMixerMcuRsp(pcMsg);
		break;
	case MIXER_MCU_GRPSTATUS_NOTIF:     //混音组状态通知
	case MIXER_MCU_ACTIVEMMBCHANGE_NOTIF:  //混音激励成员改变通知
	case MIXER_MCU_CHNNLVOL_NOTIF:      //某通道音量通知消息
	case MCU_MIXERCONNECTED_NOTIF:      //混音器上线通知
	case MCU_MIXERDISCONNECTED_NOTIF:   //混音器断链通知
    case MIXER_MCU_MIXERSTATUS_NOTIF:   //混音器通道准备完成通知
	    ProcMixerMcuNotif(pcMsg);
		break;
	case MCUVC_MIX_WAITMIXERRSP_TIMER:      //MCU等待VMP应答定时
		ProcMixerRspWaitTimer(pcMsg);
		break;
	case MT_MCU_ADDMIXMEMBER_CMD:           //主席增加混音终端
	case MCS_MCU_ADDMIXMEMBER_CMD:          //增加混音成员
		ProcMcsMcuAddMixMemberCmd(pcMsg);
		break;
	case MCS_MCU_REMOVEMIXMEMBER_CMD:       //移除混音成员
		ProcMcsMcuRemoveMixMemberCmd(pcMsg);
		break;
    case MCS_MCU_REPLACEMIXMEMBER_CMD:      //替换混音成员
        ProcMcsMcuReplaceMixMemberCmd(pcMsg);
        break;
	case MT_MCU_APPLYMIX_NOTIF:             //终端发给MCU的申请参加混音请求
		ProcMtMcuApplyMixNotify(pcMsg);
		break;

	//会议控制---轮询控制
	case MCS_MCU_STARTPOLL_CMD:         //会议控制台命令该会议开始轮询广播
	case MCS_MCU_STOPPOLL_CMD:          //会议控制台命令该会议停止轮询广播  
	case MCS_MCU_PAUSEPOLL_CMD:         //会议控制台命令该会议暂停轮询广播
	case MCS_MCU_RESUMEPOLL_CMD:        //会议控制台命令该会议继续轮询广播
	case MCS_MCU_GETPOLLPARAM_REQ:      //会议控制台向MCU查询会议轮询参数
    case MCS_MCU_CHANGEPOLLPARAM_CMD:   //会议控制台命令该会议更新轮询列表
	case MCS_MCU_SPECPOLLPOS_REQ:		//会议控制台指定会议轮询位置
		ProcMcsMcuPollMsg(pcMsg);
		break;
	case MCUVC_POLLING_CHANGE_TIMER:
		ProcPollingChangeTimerMsg(pcMsg);     //轮询定时消息处理
		break;

    //电视墙轮询
    case MCS_MCU_STARTTWPOLL_CMD:           
    case MCS_MCU_STOPTWPOLL_CMD:
    case MCS_MCU_PAUSETWPOLL_CMD:
    case MCS_MCU_RESUMETWPOLL_CMD:
    case MCS_MCU_GETTWPOLLPARAM_REQ:
        ProcMcsMcuTWPollMsg(pcMsg);
        break;
    
    //hdu轮询
    case MCS_MCU_STARTHDUPOLL_CMD:           
    case MCS_MCU_STOPHDUPOLL_CMD:
    case MCS_MCU_PAUSEHDUPOLL_CMD:
    case MCS_MCU_RESUMEHDUPOLL_CMD:
    case MCS_MCU_GETHDUPOLLPARAM_REQ:
        ProcMcsMcuHduPollMsg(pcMsg);
        break;

	//hdu批量轮询
	case MCS_MCU_STARTHDUBATCHPOLL_REQ:
	case MCS_MCU_STOPHDUBATCHPOLL_REQ:
	case MCS_MCU_RESUMEHDUBATCHPOLL_REQ:
	case MCS_MCU_PAUSEHDUBATCHPOLL_REQ:
		ProcMcsMcuHduBatchPollMsg(pcMsg);
        break;

    // hdu批量轮询定时处理
	case MCUVC_HDUBATCHPOLLI_CHANGE_TIMER:
		ProcHduBatchPollChangeTimerMsg( pcMsg );
		break;
	
	// hdu单通道轮询定时处理
    case MCUVC_HDUPOLLING_CHANGE_TIMER:
        ProcHduPollingChangeTimerMsg(pcMsg);
        break;

    case MCUVC_TWPOLLING_CHANGE_TIMER:
        ProcTWPollingChangeTimerMsg(pcMsg);
        break;

    //会议控制---点名
    case MCS_MCU_STARTROLLCALL_REQ:
    case MCS_MCU_STOPROLLCALL_REQ:
    case MCS_MCU_CHANGEROLLCALL_REQ:
        ProcMcsMcuRollCallMsg(pcMsg);
        break;
        
    /*----------------------------终端控制-------------------------------*/
    //双流FastUpdate
    case MCUVC_SECVIDEO_FASTUPDATE_TIMER:
        ProcMcuMtSecVideoFastUpdateTimer(pcMsg);
        break;

	//终端控制---呼叫与挂断终端
	case MCS_MCU_CALLMT_REQ:            //会议控制台请求MCU呼叫终端
	case MCU_MCU_REINVITEMT_REQ:        //上级MCU请求MCU呼叫终端
		ProcMcsMcuCallMtReq(pcMsg);
		break;
	case MT_MCU_CALLMTFAILURE_NOTIFY:
        ProcMtMcuCallFailureNotify(pcMsg);
		break;
	case MCS_MCU_DROPMT_REQ:            //会议控制台挂断终端请求
	case MCU_MCU_DROPMT_REQ:            //上级MCU挂断终端请求
		ProcMcsMcuDropMtReq(pcMsg);
		break;
	case MCS_MCU_SETCALLMTMODE_REQ:     //会议控制台设置MCU呼叫终端方式
		ProcMcsMcuSetCallMtModeReq(pcMsg);
		break;

	//终端控制---查询终端状态与别名
	case MT_MCU_GETMTSTATUS_REQ:        //终端发给MCU的查询会议中某个终端状态请求
	case MCS_MCU_GETMTSTATUS_REQ:       //会控向MCU查询终端状态
		ProcMcsMcuGetMtStatusReq(pcMsg);
		break;
 	case MCS_MCU_GETALLMTSTATUS_REQ:    //会控向MCU查询所有终端状态
		ProcMcsMcuGetAllMtStatusReq(pcMsg);
		break;
	case MCS_MCU_GETMTALIAS_REQ:        //会控向MCU查询终端别名
	case MT_MCU_GETMTALIAS_REQ:			//获取终端别名
		ProcMtMcuGetMtAliasReq(pcMsg);
		break;
	case MCS_MCU_GETALLMTALIAS_REQ:		//会控向MCU查询所有终端别名
		ProcMcsMcuGetAllMtAliasReq(pcMsg);
		break;
    case MCS_MCU_GETMTBITRATE_REQ:      //会控查询终端码率
        ProcMcsMcuGetMtBitrateReq(pcMsg);
        break;
    case MCS_MCU_GETMTEXTINFO_REQ:      //会控查询终端的扩展信息:版本号等
        ProcMcsMcuGetMtExtInfoReq(pcMsg);
        break;
		
	//终端控制---其它控制
	case MCS_MCU_MTCAMERA_CTRL_CMD:         //会议控制台命令终端摄像机镜头运动	
	case MCS_MCU_MTCAMERA_CTRL_STOP:        //会议控制台命令终端摄像机镜头停止运动
	case MCS_MCU_MTCAMERA_RCENABLE_CMD:     //会议控制台命令终端摄像机遥控使能	
	case MCS_MCU_MTCAMERA_SAVETOPOS_CMD:    //会议控制台命令终端摄像机将当前位置信息存入指定位置	
	case MCS_MCU_MTCAMERA_MOVETOPOS_CMD:    //会议控制台命令终端摄像机调整到指定位置

	case MCS_MCU_SETMTVIDSRC_CMD:           //会控要求MCU设置终端视频源

	case MT_MCU_MTCAMERA_CTRL_CMD:		    //主席命令终端摄像头移动
	case MT_MCU_MTCAMERA_CTRL_STOP:		    //主席命令终端摄像头停止移动
	case MT_MCU_MTCAMERA_RCENABLE_CMD:	    //主席命令终端摄像头遥控器使能
	case MT_MCU_MTCAMERA_MOVETOPOS_CMD:	    //主席命令终端摄像头调整到指定位置
	case MT_MCU_MTCAMERA_SAVETOPOS_CMD:	    //主席命令终端摄像头保存到指定位置

    case MT_MCU_SELECTVIDEOSOURCE_CMD:           //会控要求MCU设置终端视频源
		ProcMcsMcuCamCtrlCmd(pcMsg);
		break;
    case MT_MCU_VIDEOSOURCESWITCHED_CMD:
        ProcMtMcuVideoSourceSwitched(pcMsg);
        break;
	case MCU_MCU_FECC_CMD:
		ProcMMcuMcuCamCtrlCmd(pcMsg);
		break;
		
	case MCS_MCU_SETMTBITRATE_CMD:          //会议控制台命令MCU调节终端码率

	case MT_MCU_MTMUTE_CMD:                    //主席终端命令MCU进行终端静音设置
	case MT_MCU_MTDUMB_CMD:                    //主席终端命令MCU进行终端哑音设置		
		ProcMcsMcuMtOperCmd(pcMsg);
		break;

	case MCS_MCU_SETMTVOLUME_CMD:			//会控命令MCU调节终端音量,zgc 12/26/2006
		ProcMcsMcuSetMtVolumeCmd(pcMsg);
		break;

	case MCS_MCU_MTAUDMUTE_REQ:             //会控要求MCU设置终端静音
       
        //终端内置矩阵控制
    case MCS_MCU_MATRIX_GETALLSCHEMES_CMD:
    case MCS_MCU_MATRIX_GETONESCHEME_CMD:
    case MCS_MCU_MATRIX_SAVESCHEME_CMD:
    case MCS_MCU_MATRIX_SETCURSCHEME_CMD:
    case MCS_MCU_MATRIX_GETCURSCHEME_CMD:
	
        //终端外置矩阵
    case MCS_MCU_EXMATRIX_GETINFO_CMD:      //获取终端外置矩阵类型
    case MCS_MCU_EXMATRIX_SETPORT_CMD:      //设置外置矩阵连接端口号
    case MCS_MCU_EXMATRIX_GETPORT_REQ:      //请求获取外置矩阵连接端口
    case MCS_MCU_EXMATRIX_SETPORTNAME_CMD:  //设置外置矩阵连接端口名
    case MCS_MCU_EXMATRIX_GETALLPORTNAME_CMD://请求获取外置矩阵的所有端口名
        
        //终端扩展视频源
    case MCS_MCU_GETVIDEOSOURCEINFO_CMD:
    case MCS_MCU_SETVIDEOSOURCEINFO_CMD:
        
        //终端切换扩展视频源
    case MCS_MCU_SELECTEXVIDEOSRC_CMD:

		ProcMcsMcuMtOperReq(pcMsg);

		break;
		
    //终端控制---来自于终端的请求与应答	

	case MT_MCU_MTCONNECTED_NOTIF:				//终端与MCU成功建立连接
		ProcMtMcuConnectedNotif(pcMsg);	
		break;
	case MT_MCU_MTDISCONNECTED_NOTIF:	        //终端主动挂断MCU
	    ProcMtMcuDisconnectedNotif(pcMsg);	
		break;
	case MT_MCU_MTJOINCONF_NOTIF:               //终端成功入会通知
        ProcMtMcuMtJoinNotif(pcMsg);
		break;
	case MT_MCU_MTTYPE_NOTIF:                   //终端类型通知（320接入时的额外通知）
		ProcMtMcuMtTypeNotif( pcMsg );
		break;
	case MT_MCU_FLOWCONTROL_CMD:				//终端要求改变接收码率 - 码流控制命令
        ProcMtMcuFlowControlCmd(pcMsg, FALSE);
		break;
	case MT_MCU_FLOWCONTROLINDICATION_NOTIF:	//终端要求改变发送码率 - 码流控制指示
        ProcMtMcuFlowControlIndication(pcMsg);
		break;
	case MT_MCU_MTSTATUS_NOTIF:					//终端状态通知
		ProcMtMcuMtStatusNotif(pcMsg);	
		break;
	case MT_MCU_INVITEMT_ACK:					//终端接收邀请
		ProcMtMcuInviteMtAck(pcMsg);	
		break;
	case MT_MCU_INVITEMT_NACK:					//终端拒绝邀请
		ProcMtMcuInviteMtNack(pcMsg);	
		break;
	case MT_MCU_GETCHAIRMAN_REQ:
		ProcMtMcuGetChairmanReq(pcMsg);
		break;
	case MT_MCU_MTJOINCONF_REQ:					//终端申请加入
		ProcMtMcuApplyJoinReq(pcMsg);
		break;
	case MT_MCU_APPLYCHAIRMAN_REQ:				//终端申请主席
		ProcMtMcuApplyChairmanReq(pcMsg);
		break;	
	case MT_MCU_APPLYSPEAKER_NOTIF:				//终端申请发言
		ProcMtMcuApplySpeakerNotif(pcMsg);
		break;
	case MT_MCU_SENDMCMSG_CMD:					//短消息请求给主MCU控制台
		ProcMtMcuSendMcMsgReq(pcMsg);
		break;
	case MT_MCU_OPENLOGICCHNNL_ACK:				//打开逻辑通道应答
	case MT_MCU_OPENLOGICCHNNL_NACK:			//打开逻辑通道应答
		ProcMtMcuOpenLogicChnnlRsp(pcMsg);
		break;
	case MT_MCU_OPENLOGICCHNNL_REQ:				//终端打开逻辑通道请求
	case MT_MCU_LOGICCHNNLOPENED_NTF:
		ProcMtMcuOpenLogicChnnlReq(pcMsg);
		break;
	case MT_MCU_CLOSELOGICCHNNL_NOTIF:
		ProcMtMcuCloseLogicChnnlNotify(pcMsg);
		break;
	case MT_MCU_MEDIALOOPON_REQ:
	case MT_MCU_MEDIALOOPOFF_CMD:
		ProcMtMcuMediaLoopOpr(pcMsg);
		break;
	case MT_MCU_JOINEDMTLIST_REQ:				//查询与会终端列表请求
		ProcMtMcuJoinedMtListReq(pcMsg);
		break;
	case MT_MCU_JOINEDMTLISTID_REQ:
		ProcMtMcuJoinedMtListIdReq(pcMsg);
		break;
	case MT_MCU_CAPBILITYSET_NOTIF:				//终端发给MCU的能力集通知
		ProcMtMcuCapSetNotif(pcMsg);
		break;
	case MT_MCU_MTALIAS_NOTIF:
		ProcMtMcuMtAliasNotif(pcMsg);
		break;
    case MCU_MT_ENTERPASSWORD_REQ: //用于级联
		ProcMtMcuEnterPwdReq(pcMsg);
		break;
	case MT_MCU_ENTERPASSWORD_ACK:				//终端回应密码
	case MT_MCU_ENTERPASSWORD_NACK:
        ProcMtMcuEnterPwdRsp(pcMsg);
		break;
	case MT_MCU_GETH239TOKEN_REQ:               //终端给MCU的 获取 H239令牌 权限请求
        ProcMtMcuGetH239TokenReq(pcMsg);
		break;
	case MT_MCU_OWNH239TOKEN_NOTIF:             //终端给MCU的 拥有 H239令牌 权限通知
        ProcMtMcuOwnH239TokenNotify(pcMsg);
		break;
	case MT_MCU_RELEASEH239TOKEN_NOTIF:         //终端给MCU的 释放 H239令牌 权限通知
        ProcMtMcuReleaseH239TokenNotify(pcMsg);
		break;
	case POLY_MCU_GETH239TOKEN_ACK:			//获取PolyMCU的H239TOKEN 同意应答
	case POLY_MCU_GETH239TOKEN_NACK:		//获取PolyMCU的H239TOKEN 拒绝应答
	case POLY_MCU_OWNH239TOKEN_NOTIF:		//PolyMCU通知当前的TOKEN的拥有者
	case POLY_MCU_RELEASEH239TOKEN_CMD:		//PolyMCU释放H329TOKEN 命令
		ProcPolyMCUH239Rsp(pcMsg);
		break;
	//终端数据会议控制 --- 来自终端的应答
	case DCSSSN_MCU_ADDMT_ACK:					//DCS终端删除应答
	case DCSSSN_MCU_ADDMT_NACK:
		ProcDcsMcuAddMtRsp(pcMsg);
		break;
	case DCSSSN_MCU_DELMT_ACK:					//DCS终端增加应答
	case DCSSSN_MCU_DELMT_NACK:
		ProcDcsMcuDelMtRsp(pcMsg);
		break;
	case DCSSSN_MCU_MTJOINED_NOTIF:				//DCS终端上线通知
		ProcDcsMcuMtJoinedNtf(pcMsg);
		break;
	case DCSSSN_MCU_MTLEFT_NOTIF:				//DCS终端下线通知
		ProcDcsMcuMtLeftNtf(pcMsg);
		break;
	case MCU_DCSCONNCETED_NOTIF:				//数据终端的状态通知
	case MCU_DCSDISCONNECTED_NOTIF:
		ProcDcsMcuStatusNotif(pcMsg);
		break;

	case MT_MCU_RELEASEMT_REQ:
		ProcMtMcuReleaseMtReq(pcMsg);
		break;

	//其它暂未处理的终端消息
	case MT_MCU_FREEZEPIC_CMD:                 //终端发给MCU冻结图像命令
		break;
	case MT_MCU_FASTUPDATEPIC_CMD:             //终端发给MCU快速更新图像
		ProcMtMcuFastUpdatePic(pcMsg);
		break;

	case MT_MCU_GETMTSTATUS_ACK:
	case MT_MCU_GETMTSTATUS_NACK:
    case MT_MCU_GETBITRATEINFO_ACK:
    case MT_MCU_GETBITRATEINFO_NACK:
    case MT_MCU_GETBITRATEINFO_NOTIF:
    case MT_MCU_GETMTVERID_ACK:
    case MT_MCU_GETMTVERID_NACK:
        
	   ProcMtMcuOtherMsg(pcMsg);	
	   break;

//内置矩阵
    case MT_MCU_MATRIX_ALLSCHEMES_NOTIF:    
    case MT_MCU_MATRIX_ONESCHEME_NOTIF:
    case MT_MCU_MATRIX_SAVESCHEME_NOTIF:
    case MT_MCU_MATRIX_SETCURSCHEME_NOTIF:
    case MT_MCU_MATRIX_CURSCHEME_NOTIF:

 //外置矩阵    
    case MT_MCU_EXMATRIXINFO_NOTIFY:         
    case MT_MCU_EXMATRIX_GETPORT_ACK:        
    case MT_MCU_EXMATRIX_GETPORT_NACK:       
    case MT_MCU_EXMATRIX_GETPORT_NOTIF:
    case MT_MCU_EXMATRIX_PORTNAME_NOTIF:        
    case MT_MCU_EXMATRIX_ALLPORTNAME_NOTIF:    
        
//扩展视频源
    case MT_MCU_ALLVIDEOSOURCEINFO_NOTIF:
    case MT_MCU_VIDEOSOURCEINFO_NOTIF:

		ProcMtMcuMatrixMsg(pcMsg);
		break;

        //终端带宽指示
    case MT_MCU_BANDWIDTH_NOTIF:
        ProcMtMcuBandwidthNotif(pcMsg);
        break;

	//外设控制---录放像控制
    case MCU_RECCONNECTED_NOTIF:
        ProcMcuRecConnectedNotif(pcMsg);
        break;
    case MCU_RECDISCONNECTED_NOTIF:	//录像机断链通知
		ProcMcuRecDisconnectedNotif(pcMsg);
		break;
	case MCS_MCU_STARTREC_REQ:              //会控向MCU请求开始录像
		ProcMcsMcuStartRecReq(pcMsg);
		break;
	case MCS_MCU_PAUSEREC_REQ:              //会控向MCU暂停录像
		ProcMcsMcuPauseRecReq(pcMsg);
		break;
	case MCS_MCU_RESUMEREC_REQ:             //会控向MCU恢复录像
		ProcMcsMcuResumeRecReq(pcMsg);
		break;
	case MCS_MCU_STOPREC_REQ:               //会控向MCU停止录像
		ProcMcsMcuStopRecReq(pcMsg);
		break;
	case MCS_MCU_CHANGERECMODE_REQ:
		ProcMcsMcuChangeRecModeReq(pcMsg);
		break;
	case MCS_MCU_STARTPLAY_REQ:             //会控开始放像请求
		ProcMcsMcuStartPlayReq(pcMsg);
		break;
	case MCS_MCU_PAUSEPLAY_REQ:             //会控暂停放像请求
		ProcMcsMcuPausePlayReq(pcMsg);
		break;
	case MCS_MCU_RESUMEPLAY_REQ:            //会控恢复放像请求
		ProcMcsMcuResumePlayReq(pcMsg);
		break;
	case MCS_MCU_STOPPLAY_REQ:              //会控停止放像请求
		ProcMcsMcuStopPlayReq(pcMsg);
		break;
    case MCS_MCU_GETRECPROG_CMD:            //会控查询录相进度
    case MCS_MCU_GETPLAYPROG_CMD:           //会控查询放相进度
        ProcMcsMcuGetRecPlayProgCmd(pcMsg);
        break;
	case MCS_MCU_FFPLAY_REQ:                //会控快进放像请求
	case MCS_MCU_FBPLAY_REQ:                //会控快退放像请求
	case MCS_MCU_SEEK_REQ:                  //会控调整放像进度请求
		ProcMcsMcuSeekReq(pcMsg);
		break;
	case MCUVC_RECPLAY_WAITMPACK_TIMER:		//等待交换建立定时器超时,zgc, 2008-03-26
		ProcRecPlayWaitMpAckTimer(pcMsg);
		break;
	case REC_MCU_PLAYCHNSTATUS_NOTIF:		//录像机放像信道状态通知
		ProcRecMcuPlayChnnlStatusNotif(pcMsg);
		break;
	case REC_MCU_RECORDCHNSTATUS_NOTIF:		//录像机录像信道状态通知
		ProcRecMcuRecChnnlStatusNotif(pcMsg);
		break;
	case REC_MCU_RECORDPROG_NOTIF:			//当前录像进度通知
	case REC_MCU_PLAYPROG_NOTIF:			//当前放像进度通知
		ProcRecMcuProgNotif(pcMsg);
		break;    
	case REC_MCU_NEEDIFRAME_CMD:            //录像机请求关键帧
        ProcRecMcuNeedIFrameCmd(pcMsg);     
		break;

     //外设控制---码流适配器控制
    case BAS_MCU_STARTADAPT_ACK:			//启动适配应答
	case BAS_MCU_STARTADAPT_NACK:			//停止适配应答
		ProcBasMcuRsp(pcMsg);
		break;

    case HDBAS_MCU_STARTADAPT_ACK:      //启动高清适配应答
    case HDBAS_MCU_STARTADAPT_NACK:     //启动高清适配拒绝
        ProcHdBasMcuRsp(pcMsg);
        break;

	case MCU_BASCONNECTED_NOTIF:			//码率适配登记通知
		ProcMcuBasConnectedNotif(pcMsg);
		break;
	case MCU_BASDISCONNECTED_NOTIF:			//码率适配断链通知
		ProcMcuBasDisconnectedNotif(pcMsg);
		break;
	
	case VMP_MCU_VMPSTATUS_NOTIF:
    case BAS_MCU_BASSTATUS_NOTIF:
    case HDBAS_MCU_BASSTATUS_NOTIF:
        break;
    
    case HDBAS_MCU_CHNNLSTATUS_NOTIF:
        ProcHdBasChnnlStatusNotif( pcMsg );
        break;

    case HDU_MCU_CHNNLSTATUS_NOTIF:
		ProcHduMcuChnnlStatusNotif( pcMsg );
		break;

	case MCUVC_WAITBASRRSP_TIMER:				//等待适配器启动应答超时
	case MCUVC_WAITBASRRSP_TIMER+1:			//等待适配器启动应答超时
	case MCUVC_WAITBASRRSP_TIMER+2:			//等待适配器启动应答超时
    case MCUVC_WAITBASRRSP_TIMER+3:
    case MCUVC_WAITBASRRSP_TIMER+4:
		ProcBasStartupTimeout(pcMsg);
		break;
    case BAS_MCU_NEEDIFRAME_CMD:
        ProcBasMcuCommand(pcMsg);
        break;

	case HDU_MCU_NEEDIFRAME_CMD:
		ProcHduMcuNeedIFrameCmd(pcMsg);
		break;
		
    case MCUVC_SENDFLOWCONTROL_TIMER:
        ProcSendFlowctrlToDSMtTimeout(pcMsg);
		break;
		
	case MCU_SMCUOPENDVIDEOCHNNL_TIMER:
		ProcSmcuOpenDVideoChnnlTimer(pcMsg);
		break;

	case MCU_PRSCONNECTED_NOTIF:		//发给MCU内部会议实例通知PRS建链成功
		ProcPrsConnectedNotif(pcMsg);
		break;
	case MCU_PRSDISCONNECTED_NOTIF:		//发给MCU内部会议实例通知PRS断链
		ProcPrsDisconnectedNotif(pcMsg);
		break;
	case PRS_MCU_SETSRC_ACK:			//PRS给MCU保存信息源确认
	case PRS_MCU_SETSRC_NACK:			//PRS给MCU保存信息源拒绝
//	case PRS_MCU_ADDRESENDCH_ACK:		//PRS给MCU停止工作确认
//	case PRS_MCU_ADDRESENDCH_NACK:		//PRS给MCU停止工作拒绝
//	case PRS_MCU_REMOVERESENDCH_ACK:	//PRS给MCU改变复合参数确认
//	case PRS_MCU_REMOVERESENDCH_NACK:	//PRS给MCU改变复合参数拒绝
	case PRS_MCU_REMOVEALL_ACK:			//PRS给MCU改变复合参数确认
	case PRS_MCU_REMOVEALL_NACK:		//PRS给MCU改变复合参数拒绝
		ProcPrsMcuRsp(pcMsg);
		break;

	case MCS_MCU_STOPSWITCHMC_REQ:
	case MCU_MCSDISCONNECTED_NOTIF:
		ProcMcStopSwitch(pcMsg);
		break;

	case MCS_MCU_STOP_SWITCH_TW_REQ:
		ProcMcsMcuStopSwitchTWReq(pcMsg);
		break;

    case MCU_TVWALLCONNECTED_NOTIF:
        ProcTvwallConnectedNotif(pcMsg);
        break;
	case MCU_TVWALLDISCONNECTED_NOTIF:
		ProcTvwallDisconnectedNotif(pcMsg);
		break;	
		
    //4.6 jlb
    case HDU_MCU_STATUS_NOTIF:
		ProcHduMcuStatusNotif( pcMsg );  
		break;

	case MCU_HDUCONNECTED_NOTIF:                    
		ProcHduConnectedNotif(pcMsg);
		break;

	case MCU_HDUDISCONNECTED_NOTIF:
		ProcHduDisconnectedNotif(pcMsg);
		break;


    //来自于终端与外设的应答一般处理	
	case REC_MCU_PAUSEREC_ACK:			//暂停录像应答
	case REC_MCU_RESUMEREC_ACK:			//恢复录像应答	
	case REC_MCU_CHANGERECMODE_ACK:		//改变录像模式
	case REC_MCU_PAUSEPLAY_ACK:			//暂停放像应答
	case REC_MCU_RESUMEPLAY_ACK:		//恢复放像应答
	case REC_MCU_STOPPLAY_ACK:			//停止放像应答
	case REC_MCU_FFPLAY_ACK:			//放像快进应答
	case REC_MCU_FBPLAY_ACK:			//放像快退应答
	case REC_MCU_SEEK_ACK:				//放像进度调整应答
		ProcCommonOperAck(pcMsg);
		break;

    case REC_MCU_STARTREC_ACK:			//开始录像应答
    case REC_MCU_STOPREC_ACK:			//停止录像应答
	case REC_MCU_STARTPLAY_ACK:			//开始放像应答
        ProcRecMcuResp(pcMsg);
        break;

	//来自于终端与外设的拒绝应答一般处理	
    case REC_MCU_STARTREC_NACK:			//开始录像应答
	case REC_MCU_PAUSEREC_NACK:			//暂停录像应答
	case REC_MCU_RESUMEREC_NACK:		//恢复录像应答
	case REC_MCU_STOPREC_NACK:			//停止录像应答
	case REC_MCU_CHANGERECMODE_NACK:	//改变录像模式
	case REC_MCU_STARTPLAY_NACK:		//开始放像应答
	case REC_MCU_PAUSEPLAY_NACK:		//暂停放像应答
	case REC_MCU_RESUMEPLAY_NACK:		//恢复放像应答
	case REC_MCU_STOPPLAY_NACK:			//停止放像应答
	case REC_MCU_FFPLAY_NACK:			//放像快进应答
	case REC_MCU_FBPLAY_NACK:			//放像快退应答
	case REC_MCU_SEEK_NACK:				//放像进度调整应答
		ProcCommonOperNack(pcMsg);
		break;
	
	//立案系统消息
	case MT_MCU_STARTMTSELME_REQ:
	case MT_MCU_STOPMTSELME_CMD:
	case MT_MCU_STARTBROADCASTMT_REQ:
	case MT_MCU_STOPBROADCASTMT_CMD:
	case MT_MCU_GETMTSELSTUTS_REQ:
		ProcBuildCaseSpecialMessage(pcMsg);
        break;

	//Mp 消息
	case MP_MCU_REG_REQ:
	case MP_MCU_ADDSWITCH_ACK:
	case MP_MCU_ADDSWITCH_NACK:
	case MP_MCU_REMOVESWITCH_ACK:
	case MP_MCU_REMOVESWITCH_NACK:
	case MP_MCU_GETSWITCHSTATUS_ACK:
	case MP_MCU_GETSWITCHSTATUS_NACK:
	case MP_MCU_ADDMULTITOONESWITCH_ACK:
	case MP_MCU_ADDMULTITOONESWITCH_NACK:
	case MP_MCU_REMOVEMULTITOONESWITCH_ACK:
	case MP_MCU_REMOVEMULTITOONESWITCH_NACK:
	case MP_MCU_STOPMULTITOONESWITCH_ACK:
	case MP_MCU_STOPMULTITOONESWITCH_NACK:
	case MP_MCU_ADDRECVONLYSWITCH_ACK:
	case MP_MCU_ADDRECVONLYSWITCH_NACK:
	case MP_MCU_REMOVERECVONLYSWITCH_ACK:
	case MP_MCU_REMOVERECVONLYSWITCH_NACK:
	case MP_MCU_SETRECVSWITCHSSRC_ACK:
	case MP_MCU_SETRECVSWITCHSSRC_NACK:
        // guzh [3/29/2007]
    case MP_MCU_ADDBRDSRCSWITCH_ACK:
    case MP_MCU_ADDBRDDSTSWITCH_ACK:
    case MP_MCU_ADDBRDSRCSWITCH_NACK:
    case MP_MCU_ADDBRDDSTSWITCH_NACK:
    case MP_MCU_REMOVEBRDSRCSWITCH_ACK:
    case MP_MCU_REMOVEBRDSRCSWITCH_NACK:
    case MP_MCU_BRDSRCSWITCHCHANGE_NTF:    // 广播源被实际移除通知         
		ProcMpMessage(pcMsg);
		break;

	//处理Mp断链
	case MCU_MP_DISCONNECTED_NOTIFY:
		ProcMpDissconnected(pcMsg);
		break;

	//处理MtAdp断链
	case MCU_MTADP_DISCONNECTED_NOTIFY:
		ProcMtAdpDissconnected(pcMsg);
		break;

	case MT_MCU_MSD_NOTIF:
		ProcMtMcuMsdRsp(pcMsg);
		break;
	case MCUVC_SCHEDULED_CHECK_TIMER:			//预约会议定时检查
		ProcTimerScheduledCheck(pcMsg);
		break;
	case MCUVC_ONGOING_CHECK_TIMER:			//进行中会议定时检查
		ProcTimerOngoingCheck(pcMsg);	
		break;
	case MCUVC_INVITE_UNJOINEDMT_TIMER:		//定时邀请未与会终端
		ProcTimerInviteUnjoinedMt(pcMsg);
		break;
	case MCUVC_REFRESH_MCS_TIMER:
		ProcTimerRefreshMcs(pcMsg);
		break;
	case MCUVC_MCUSRC_CHECK_TIMER:
		ProcTimerMcuSrcCheck(pcMsg);
		break;
	case MCUVC_RECREATE_DATACONF_TIMER:
		ProcTimerRecreateDataConf(pcMsg);
		break;
    case MCUVC_WAIT_CASCADE_CHANNEL_TIMER:
        ProcTimerReopenCascadeChannel();
        break;
	case MT_MCU_REGISTERGK_ACK:         //在GK上注册成功
		ProcConfRegGkAck(pcMsg);
		break;
	case MT_MCU_REGISTERGK_NACK:		//在GK上注册失败
		ProcConfRegGkNack(pcMsg);
		break;
    
    case MT_MCU_CONF_STARTCHARGE_ACK:
    case MT_MCU_CONF_STARTCHARGE_NACK:
	case MT_MCU_CONF_CHARGEEXP_NOTIF:
    case MT_MCU_CONF_STOPCHARGE_ACK:
    case MT_MCU_CHARGE_REGGK_NOTIF:
        ProcGKChargeRsp(pcMsg);
        break;

    case MCUVC_CHANGE_VMPCHAN_SPEAKER_TIMER:
    case MCUVC_CHANGE_VMPCHAN_CHAIRMAN_TIMER:
    case MCUVC_CHANGE_VMPPARAM_TIMER:
        ProcTimerChangeVmpParam(pcMsg);
        break;

	case MT_MCU_UNREGISTERGK_ACK:       //在GK上注销成功
	case MT_MCU_UNREGISTERGK_NACK:      //在GK上注销失败
	case VMP_MCU_SETCHANNELBITRATE_ACK: //改变码率应答
	case VMP_MCU_SETCHANNELBITRATE_NACK://改变码率拒绝
	case BAS_MCU_STOPADAPT_ACK:			//启动适配拒绝
	case BAS_MCU_STOPADAPT_NACK:		//停止适配拒绝
    case HDBAS_MCU_STOPADAPT_ACK:       //停止高清适配应答
    case HDBAS_MCU_STOPADAPT_NACK:      //停止高清适配拒绝
	case MCU_MCU_REINVITEMT_ACK:
	case MCU_MCU_SETIN_ACK:
	case MCU_MCU_SETIN_NACK:
	case MCU_MCU_DROPMT_ACK:
	case MCU_MCU_DROPMT_NACK:
	case MCU_MCU_DELMT_ACK:
	case MCU_MCU_DELMT_NACK:
		break;

        //N+1注册成功或会议回滚，需要同步会议信息
    case NPLUS_VC_DATAUPDATE_NOTIF:
        ProcNPlusConfDataUpdate(pcMsg);
        break;
	case REC_MCU_RECSTATUS_NOTIF:
		break;
	case EV_MCU_OPENSECVID_TIMER:
		ProcPolycomSecVidChnnl(pcMsg);
		break;
		
	case TVWALL_MCU_STATUS_NOTIF:
		break;

    //卫星MODEM操作
    case MODEM_MCU_REG_ACK:
    case MODEM_MCU_REG_NACK:
        ProcModemConfRegRsp( pcMsg );
		break;

	case MODEM_MCU_ERRSTAT_NOTIF:
	case MODEM_MCU_RIGHTSTAT_NOTIF:
		ProcModemMcuStatusNotif( pcMsg );
		break;

/*-------------------------------------------------------------------------------------*/
	//VCS通过模板创会
	case MCU_SCHEDULE_VCSCONF_START:
	case VCS_MCU_VCMT_REQ:
	case VCS_MCU_VCMODE_REQ:
	case VCS_MCU_CHGVCMODE_REQ:
	case VCS_MCU_MUTE_REQ:
	case VCS_MCU_GROUPCALLMT_REQ:
	case VCS_MCU_GROUPDROPMT_REQ:
	case VCS_MCU_STARTCHAIRMANPOLL_REQ:
	case VCS_MCU_STOPCHAIRMANPOLL_REQ:
	case VCS_MCU_ADDMT_REQ:
	case VCS_MCU_DELMT_REQ:
		ProcVcsMcuMsg(pcMsg);
		break;
	case MCUVC_VCMTOVERTIMER_TIMER:
		ProcVCMTOverTime(pcMsg);
		break;
	case MCUVC_VCS_CHAIRPOLL_TIMER:
		ProcChairPollTimer(pcMsg);
		break;
    case VCS_MCU_RELEASEMT_ACK:
	case VCS_MCU_RELEASEMT_NACK:
		ProcVcsMcuRlsMtMsg(pcMsg);
		break;
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in InstanceEntry()!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ) );
		break;
	}

	//主备数据倒换时，[MSMag <->McuVc]模块互锁，解锁 2005-12-15
	g_cMSSsnApp.LeaveMSSynLock(AID_MCU_VC);

	return;
}

/*====================================================================
    函数名      :DaemonInstanceEntry
    功能        :Daemon实例处理函数入口
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg,  传入的消息
				  CApp* pApp ,传入应用指针
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/26    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::DaemonInstanceEntry( CMessage* const pcMsg, CApp* pcApp )
{
	if( NULL == pcMsg )
	{
		ConfLog( FALSE, "[CMcuVcInst] The received msg's pointer in the msg DaemonEntry is NULL!");
		return;
	}

	//主备数据倒换时，[MSMag <->McuVc]模块互锁，锁定 2005-12-15
	g_cMSSsnApp.EnterMSSynLock(AID_MCU_VC);

	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	switch ( pcMsg->event )
	{
	case OSP_POWERON:
		DaemonProcPowerOn( pcMsg );
		break;

    case MCS_MCU_CREATECONF_REQ:
        DaemonProcMcsMcuCreateConfReq(pcMsg);
        break;

	case MCUVC_WAIT_MPREG_TIMER:              // 定时等待Mp注册
		ProcTimerWaitMpRegCheck(pcMsg);
		break;

	case MCU_MSSTATE_EXCHANGE_NTF:
		DaemonProcMcuMSStateNtf(pcMsg);       //备用板升级为主用板时 通知[McuVc]模块主备用状态变更
		break;
	case MT_MCU_CREATECONF_REQ:
		DaemonProcMtMcuCreateConfReq(pcMsg);
		break;
	case MCS_MCU_LISTALLCONF_REQ:             //列出该MCU上所有会议信息
		DaemonProcMcsMcuListAllConfReq( pcMsg, pcApp );
		break;   
	case MCS_MCU_GETMCUSTATUS_CMD:
	case MCS_MCU_GETMCUSTATUS_REQ:	          //查询MCU状态
		DaemonProcMcsMcuGetMcuStatusReq(pcMsg, pcApp);
		break;
	case MCU_APPTASKTEST_REQ:			      //GUARD Probe Message
		DaemonProcAppTaskRequest(pcMsg);
		break;
	case MCU_EQPCONNECTED_NOTIF:	          //外设登记成功
		DaemonProcMcuEqpConnectedNotif(pcMsg);
		break;
	case MCU_EQPDISCONNECTED_NOTIF:           //外设断链
		DaemonProcMcuEqpDisconnectedNotif(pcMsg);
		break;
	case MCU_DCSCONNCETED_NOTIF:			  //DCS登记成功
		DaemonProcMcuDcsConnectedNtf(pcMsg);  
		break;
	case MCU_DCSDISCONNECTED_NOTIF:			  //DCS断链
		DaemonProcMcuDcsDisconnectedNtf(pcMsg);
		break;
	case MCU_MCSCONNECTED_NOTIF:		      //会控登记成功
		DaemonProcMcuMcsConnectedNotif(pcMsg);
		break;
	case MCU_MCSDISCONNECTED_NOTIF:           //会控断链
		DaemonProcMcuMcsDisconnectedNotif(pcMsg);
		break;
    case MCS_MCU_CREATECONF_BYTEMPLATE_REQ: //会议控制台在MCU上按会议模板创建一个会议(会议模板变成即时会议)
    case MCS_MCU_CREATESCHCONF_BYTEMPLATE_REQ:  //根据模板创建预约会议
	case VCS_MCU_CREATECONF_BYTEMPLATE_REQ:
        DaemonProcMcsMcuCreateConfByTemplateReq(pcMsg);        
        break;       
    case MCS_MCU_CREATETEMPLATE_REQ:            //增加模板
    case MCS_MCU_MODIFYTEMPLATE_REQ:            //修改模板
    case MCS_MCU_DELTEMPLATE_REQ:               //删除模板
        DaemonProcMcsMcuTemplateOpr(pcMsg);
        break;

	case MCS_MCU_SAVECONFTOTEMPLATE_REQ:	//会议控制台请求将当前会议保存为会议模板(预留), zgc, 2007/04/20
		DaemonProcMcsMcuSaveConfToTemplateReq(pcMsg);
		break;

    case MT_MCU_REGISTERGK_ACK:				//在GK上注册成功
    case MT_MCU_REGISTERGK_NACK:			//在GK上注册失败
    case MT_MCU_UNREGISTERGK_ACK:			//在GK上注销成功
    case MT_MCU_UNREGISTERGK_NACK:			//在GK上注销失败
        DaemonProcGkRegRsp(pcMsg);
        break;

    case MCU_CREATECONF_NPLUS:
        DaemonProcCreateConfNPlus(pcMsg);
        break;

    case MT_MCU_CONF_STOPCHARGE_ACK:
    case MT_MCU_CONF_STOPCHARGE_NACK:
    case MT_MCU_CONF_STARTCHARGE_ACK:
    case MT_MCU_CONF_STARTCHARGE_NACK:
	case MT_MCU_CONF_CHARGEEXP_NOTIF:
    case MT_MCU_CHARGE_REGGK_NOTIF:
        DaemonProcGKChargeRsp(pcMsg);
        break;

    case MCU_MCUREREGISTERGK_NOITF:          //在GK上重新注册mcu别名
        DaemonProcMcuReRegisterGKNtf(pcMsg);
        break;

    case HDBAS_MCU_BASSTATUS_NOTIF:           //高清适配器状态通知
	case REC_MCU_RECSTATUS_NOTIF:		      //录像机状态通知
	case MIXER_MCU_MIXERSTATUS_NOTIF:	      //混音器状态通知消息
	case BAS_MCU_BASSTATUS_NOTIF:		      //适配器状态通知
	case VMP_MCU_VMPSTATUS_NOTIF:             //画面合成器状态通知
    case VMPTW_MCU_VMPTWSTATUS_NOTIF:         //复合电视墙状态通知
	case TVWALL_MCU_STATUS_NOTIF:             //电视墙状态通知
	case PRS_MCU_PRSSTATUS_NOTIF:			  //VMP给MCU的状态上报
	case PRS_MCU_SETSRC_NOTIF:			      //PRS给MCU的保存信息源结果
//	case PRS_MCU_ADDRESENDCH_NOTIF:		      //PRS给MCU的停止结果
//	case PRS_MCU_REMOVERESENDCH_NOTIF:	      //PRS给MCU的状态改变结果
	case PRS_MCU_REMOVEALL_NOTIF:		      //PRS给MCU的状态改变结果
    case HDU_MCU_STATUS_NOTIF:                //高清电视墙状态通知
		DaemonProcPeriEqpMcuStatusNotif(pcMsg);
		break;

	//case MIXER_MCU_GRPSTATUS_NOTIF:			  //混音组状态通知
	//	break;

	case BAS_MCU_CHNNLSTATUS_NOTIF://适配器通道状态通知
		ProcBasChnnlStatusNotif( pcMsg);
		break;

    case HDBAS_MCU_CHNNLSTATUS_NOTIF://高清适配器通道状态通知
        DaemonProcHDBasChnnlStatusNotif( pcMsg );
		break;

    case HDU_MCU_CHNNLSTATUS_NOTIF:
        DaemonProcHduMcuChnnlStatusNotif( pcMsg );
		break;

	case MCS_MCU_STOPSWITCHMC_REQ:		      //会议控制台停止播放请求
		DaemonProcMcsMcuStopSwitchMcReq(pcMsg);
		break;
	case MCS_MCU_STOP_SWITCH_TW_REQ:	      //停止向电视墙交换请求
		DaemonProcMcsMcuStopSwitchTWReq(pcMsg);
		break;
		
	case MCS_MCU_LISTALLRECORD_REQ:         //会控列表请求
		ProcMcsMcuListAllRecordReq(pcMsg);
		break;
	case REC_MCU_LISTALLRECORD_NOTIF:		//列出录像机中所有记录应答
		ProcRecMcuListAllRecNotif(pcMsg);
		break;
	case MCS_MCU_DELETERECORD_REQ:          //会控请求删除文件
		ProcMcsMcuDeleteRecordReq(pcMsg);
		break;
    case MCS_MCU_RENAMERECORD_REQ:          //会控请求更改文件名
        ProcMcsMcuRenameRecordReq(pcMsg);
        break;
	case MCS_MCU_PUBLISHREC_REQ:            //发布录像请求
		ProcMcsMcuPublishRecReq(pcMsg);
		break;
	case MCS_MCU_CANCELPUBLISHREC_REQ:		//撤销发布录像请求
		ProcMcsMcuCancelPublishRecReq(pcMsg);
		break;

	case REC_MCU_RECORDCHNSTATUS_NOTIF:	      //录像机录像信道状态通知
	case REC_MCU_PLAYCHNSTATUS_NOTIF:	      //录像机放像信道状态通知
		DaemonProcRecMcuChnnlStatusNotif(pcMsg);
		break;

	case REC_MCU_RECORDPROG_NOTIF:		      //当前录像进度通知
	case REC_MCU_PLAYPROG_NOTIF:		      //当前放像进度通知
		DaemonProcRecMcuProgNotif(pcMsg);
		break;
	                                           //!录像文件管理应答消息处理
		
	case REC_MCU_LISTALLRECORD_NACK:    //录像机拒绝录像列表请求
	case REC_MCU_PUBLISHREC_NACK:		//发布录像
    case REC_MCU_CANCELPUBLISHREC_NACK: //取消发布录像
	case REC_MCU_DELETERECORD_NACK:     //删除录像记录拒绝消息
	case REC_MCU_RENAMERECORD_NACK:     //更改录像记录拒绝消息
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		break;
		
	case REC_MCU_PUBLISHREC_ACK:		//发布录像
    case REC_MCU_CANCELPUBLISHREC_ACK:  //取消发布录像
	case REC_MCU_DELETERECORD_ACK:      //删除录像记录应答消息
	case REC_MCU_RENAMERECORD_ACK:      //更改录像记录应答消息
	case REC_MCU_LISTALLRECORD_ACK:     //录像机列应答(完毕)消息
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		break;


	case MCS_MCU_GETMCUPERIEQPSTATUS_REQ:     //查询MCU外设状态
	case MCS_MCU_GETRECSTATUS_REQ:	          //查询录像机状态请求
	case MCS_MCU_GETMIXERSTATUS_REQ:	      //查询混音器状态请求
    case MCS_MCU_GETPERIDCSSTATUS_REQ:        //查询DCS状态请求
	//case MCS_MCU_GETBASSTATUS_REQ:	      //查询码率适配器状态请求
		DaemonProcMcsMcuGetPeriEqpStatusReq(pcMsg);
		break;
		
	case REC_MCU_EXCPT_NOTIF:			      //录像机异常通知消息
		DaemonProcCommonNotif(pcMsg);
		break;

	case MT_MCU_MTJOINCONF_REQ:
		DaemonProcMtMcuApplyJoinReq(pcMsg);
		break;
		
	//MP Message
	case MCU_MP_DISCONNECTED_NOTIFY:          //要处理Mp断链
	case MP_MCU_REG_REQ:
        g_cMpManager.ProcMpToMcuMessage(pcMsg);
        break;
        
    //case MP_MCU_FLUXOVERRUN_NOTIFY:
    case MP_MCU_FLUXSTATUS_NOTIFY:
        DaemonProcMpFluxNotify(pcMsg);
        break;

	//Mtadp Message
	case MCU_MTADP_DISCONNECTED_NOTIFY:       //要处理MtAdp断链
	case MTADP_MCU_REGISTER_REQ:
		g_cMpManager.ProcMtAdpToMcuMessage(pcMsg);
		break;
	
	//数据会议撤会后的消息处理
	case DCSSSN_MCU_RELEASECONF_ACK:
	case DCSSSN_MCU_RELEASECONF_NACK:
	case DCSSSN_MCU_CONFRELEASED_NOTIF:
		DaemonProcDcsMcuReleaseConfRsp(pcMsg);
		break;
        
    //N+1 备份主板注册备板失败
    case MCU_NPLUS_REG_NACK:
        DaemonProcNPlusRegBackupRsp(pcMsg);
        break;

    case MCU_NMS_SENDNMSMSG_CMD:
        DaemonProcSendMsgToNms(pcMsg);
        break;

	//主控热备份单元测试接口
	case EV_TEST_TEMPLATEINFO_GET_REQ:
	case EV_TEST_CONFINFO_GET_REQ:
	case EV_TEST_CONFMTLIST_GET_REQ:
	case EV_TEST_ADDRBOOK_GET_REQ:
	case EV_TEST_MCUCONFIG_GET_REQ:
		DaemonProcUnitTestMsg(pcMsg);
		break;
	case MT_MCU_CALLFAIL_HDIFULL_NOTIF:
		DaemonProcHDIFullNtf(pcMsg);
		break;

    case MODEM_MCU_REG_ACK:
        DaemonProcModemReg( pcMsg );
		break;

	case MCUVC_CONFINFO_MULTICAST_TIMER:
		DaemonProcTimerMultiCast( pcMsg, pcApp );
		break;

	default:
		ConfLog( FALSE, "[CMcuVcInst] Wrong message %u(%s) received in DaemonEntry()!\n", 
			             pcMsg->event, ::OspEventDesc( pcMsg->event ) );
		break;
	}

	//主备数据倒换时，[MSMag <->McuVc]模块互锁，解锁 2005-12-15
	g_cMSSsnApp.LeaveMSSynLock(AID_MCU_VC);

	return;
}

/*=============================================================================
    函 数 名： DaemonProcMcuMSStateNtf
    功    能： 备用板升级为主用板时 通知[McuVc]模块主备用状态变更
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/31  4.0			万春雷                  创建
=============================================================================*/
void CMcuVcInst::DaemonProcMcuMSStateNtf( const CMessage * pcMsg )
{   
    u8 byIsSwitchOk = *( pcMsg->content );

	CServMsg cServMsg;
	cServMsg.SetSrcMtId(0);
	cServMsg.SetSrcSsnId(0);
    cServMsg.SetMsgBody( &byIsSwitchOk, sizeof(u8) );

    // guzh [9/14/2006] 
    //1.主备发生切换时,通知所有外设、DCS、MP、MtAdp、Mc 
    //如果倒换失败,让其断开连接
    g_cMpSsnApp.BroadcastToAllMpSsn( MCU_MSSTATE_EXCHANGE_NTF, &byIsSwitchOk, sizeof(u8) );
    g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn( MCU_MSSTATE_EXCHANGE_NTF, cServMsg );
    CMcsSsn::BroadcastToAllMcsSsn( MCU_MSSTATE_EXCHANGE_NTF, &byIsSwitchOk, sizeof(u8) );
    g_cEqpSsnApp.BroadcastToAllPeriEqpSsn( MCU_MSSTATE_EXCHANGE_NTF, &byIsSwitchOk, sizeof(u8) );
    g_cDcsSsnApp.BroadcastToAllDcsSsn( MCU_MSSTATE_EXCHANGE_NTF,  &byIsSwitchOk, sizeof(u8) );
    
    // guzh [9/14/2006] 如果没有同步Ok, 直接退出    
    if ( !byIsSwitchOk )
    {
        return;
    }
 
	//2.主备发生切换时,新的主用板 将 [界面化配置信息] 重新通知当前所有会控
	::OspPost(MAKEIID(AID_MCU_CONFIG, 1), MCU_MSSTATE_EXCHANGE_NTF, pcMsg->content, pcMsg->length);

	//3.主备发生切换时,新的主用板将 [MCU状态] 重新通知当前所有会控
	TMcuStatus tMcuStatus;
	if (TRUE == g_cMcuVcApp.GetMcuCurStatus(tMcuStatus))
	{
		//notify all mcs
		cServMsg.SetMsgBody((u8 *)&tMcuStatus, sizeof(tMcuStatus));
		SendMsgToAllMcs(MCU_MCS_MCUSTATUS_NOTIF, cServMsg);
	}
	
	//4.主备发生切换时,新的主用板将 [地址簿状态] 重新通知当前所有会控
    CMcsSsn::BroadcastToAllMcsSsn(MCU_ADDRBOOK_GETENTRYLIST_NOTIF);
    CMcsSsn::BroadcastToAllMcsSsn(MCU_ADDRBOOK_GETGROUPLIST_NOTIF);    

	//5.主备发生切换时,新的主用板将 [用户状态] 重新通知当前所有会控
    CMcsSsn::BroadcastToAllMcsSsn(MCS_MCU_GETUSERLIST_REQ);


	//6.主备发生切换时,新的主用板将 [会议状态/会控锁定状态/终端状态/SMCU_MEDAISRC/MMCU锁定状态] 重新通知当前所有会控
	//notify all mcs
	NtfMcsMcuCurListAllConf(cServMsg);

	return;
}

/*=============================================================================
  函 数 名： DaemoProcMcsMcuCreateConfByTemplateReq
  功    能： 通过模板创建会议
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::DaemonProcMcsMcuCreateConfByTemplateReq(const CMessage * pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TTemplateInfo tTemConf;   
	u8 byConfIdx = g_cMcuVcApp.GetConfIdx(cServMsg.GetConfId());

	if (!g_cMcuVcApp.GetTemplate(byConfIdx, tTemConf))
	{
		ConfLog(FALSE, "[DaemoProcMcsMcuCreateConfByTemplateReq] invalid confidx :%d\n", byConfIdx);
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//ongoing conf
	if (MCS_MCU_CREATECONF_BYTEMPLATE_REQ == pcMsg->event 
		|| VCS_MCU_CREATECONF_BYTEMPLATE_REQ == pcMsg->event)
	{
		tTemConf.m_tConfInfo.m_tStatus.SetOngoing();
		tTemConf.m_tConfInfo.SetStartTime( time(NULL) );       
	}
	//schedule conf
	else
	{
		tTemConf.m_tConfInfo.m_tStatus.SetScheduled();
		TKdvTime *ptStartTime = (TKdvTime *)cServMsg.GetMsgBody();
		time_t time;
		ptStartTime->GetTime(time);
		tTemConf.m_tConfInfo.SetStartTime(time);
	}   

	g_cMcuVcApp.TemInfo2Msg(tTemConf, cServMsg);
	cServMsg.SetSrcMtId(CONF_CREATE_MCS);
	cServMsg.SetConfIdx(byConfIdx);

    u8 byInsID = AssignIdleConfInsID();
    if(0 != byInsID)
    {
		// 对于mcs会议、vcs会议实际处理区分开
		if (VCS_MCU_CREATECONF_BYTEMPLATE_REQ == pcMsg->event)
		{
			ConfLog(FALSE, "[DaemonProcMcsMcuCreateConfByTemplateReq] VCSConf start command to inst%d\n",
				           byInsID);
			post(MAKEIID( AID_MCU_VC, byInsID ), MCU_SCHEDULE_VCSCONF_START, 
                 cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
		else
		{
			ConfLog(FALSE, "[DaemonProcMcsMcuCreateConfByTemplateReq] MCSConf start command to inst%d\n",
				           byInsID);
			post(MAKEIID( AID_MCU_VC, byInsID ), MCU_SCHEDULE_CONF_START, 
					cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
    }
	else
    {
        cServMsg.SetErrorCode(ERR_MCU_CONFNUM_EXCEED);
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        ConfLog(FALSE, "[DaemoProcMcsMcuCreateConfByTemplateReq] assign instance id failed!\n");
    }

	return;
}

/*=============================================================================
函 数 名： DaemonProcCreateConfNPlus
功    能： n+1模式创会
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/28  4.0			许世林                  创建
=============================================================================*/
void CMcuVcInst::DaemonProcCreateConfNPlus(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TNPlusConfData *ptConfData = (TNPlusConfData *)cServMsg.GetMsgBody();
    CApp *pcApp = &g_cMcuVcApp;

    //主mcu会议回滚
    if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_MASTER_CONNECTED)
    {
        //先查找此会议是否已经存在，根据会议名称和e164号匹配
        for (u8 byInsId = 1; byInsId <= MAXNUM_MCU_CONF; byInsId++)
        {
            CMcuVcInst *pcInst = (CMcuVcInst *)pcApp->GetInstance( byInsId );
            if (NULL != pcInst && pcInst->CurState() == STATE_ONGOING)
            {
                if (strcmp(pcInst->m_tConf.GetConfName(), ptConfData->m_tConf.GetConfName()) == 0 &&
                    strcmp(pcInst->m_tConf.GetConfE164(), ptConfData->m_tConf.GetConfE164()) == 0)
                {
                    //先结束这个会议
                    NPlusLog("[DaemonProcCreateConfNPlus] release conf %s before rollback.\n", pcInst->m_tConf.GetConfName());
                    pcInst->ReleaseConf(FALSE);
                    pcInst->NextState(STATE_IDLE);                    

                    break;
                }
            }
        }
    }
    //备份mcu上会议恢复
    else if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_SWITCH)
    {
        NPlusLog("[DaemonProcCreateConfNPlus] conf restore in N+1 mode\n");
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[DaemonProcCreateConfNPlus] invalid n+1 mode(%d) in conf restore.\n",
                  g_cNPlusApp.GetLocalNPlusState());
        return;
    }

    u8 byInsID = AssignIdleConfInsID();
    if (0 != byInsID)
    {
        if (OSP_OK == post(MAKEIID( AID_MCU_VC, byInsID ), MCU_CREATECONF_NPLUS, pcMsg->content, pcMsg->length))
        {            
		    CMcuVcInst *pcInst = (CMcuVcInst *)pcApp->GetInstance( byInsID );
		    pcInst->NextState(STATE_WAITEQP);
        }        
    }
    else
    {
        ConfLog(FALSE, "[DaemonProcCreateConfNPlus] assign instance id failed! \n");
    }
    return;
}

/*=============================================================================
  函 数 名： DaemonProcMcsMcuTemplateOpr
  功    能： 模板操作
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::DaemonProcMcsMcuTemplateOpr(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);  
    u8 byConfIdx = g_cMcuVcApp.GetConfIdx(cServMsg.GetConfId());

    switch(pcMsg->event)
    {
    case MCS_MCU_CREATETEMPLATE_REQ:
    case MCS_MCU_MODIFYTEMPLATE_REQ:
        {
            TTemplateInfo  tTemInfo;
			s8* pszUnProcInfoHead = NULL;
			u16 wUnProcLen = 0;
            g_cMcuVcApp.Msg2TemInfo(cServMsg, tTemInfo, &pszUnProcInfoHead, &wUnProcLen);
            tTemInfo.m_byConfIdx = byConfIdx;

            //检验会议信息逻辑
            u16 wErrCode = 0;
            if ( !IsConfInfoCheckPass(cServMsg, tTemInfo.m_tConfInfo, wErrCode, TRUE))
            {
                cServMsg.SetErrorCode( wErrCode );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
			
			// VMP合成性能限制, zgc, 2008-03-26
			if ( tTemInfo.m_tConfInfo.GetConfAttrb().IsHasVmpModule() )
			{
				TVMPParam tVMPParam = tTemInfo.m_atVmpModule.GetVmpParam();
				// 按会议信息计算最大支持合成能力
				u8 byMaxChnlNumByConf = CMcuPfmLmt::GetMaxCapVMPByConfInfo(tTemInfo.m_tConfInfo);
				u8 byTempChnl = GetVmpChlNumByStyle( tVMPParam.GetVMPStyle() );
				if ( byTempChnl > byMaxChnlNumByConf )
				{
					cServMsg.SetErrorCode( ERR_INVALID_VMPSTYLE );
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					return;
				}
			}

            //若会议结束模式为无终端自动结束，应该有终端
            if(0 == tTemInfo.m_byMtNum && tTemInfo.m_tConfInfo.GetConfAttrb().IsReleaseNoMt())
            {
                cServMsg.SetErrorCode( ERR_MCU_NOMTINCONF );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                ConfLog( FALSE, "template %s create failed because no mt in auto-end conf!\n", tTemInfo.m_tConfInfo.GetConfName() );
                return;
            }

            if(MCS_MCU_CREATETEMPLATE_REQ == pcMsg->event)
            {
                //会议的E164号码已存在，拒绝 
                if( g_cMcuVcApp.IsConfE164Repeat( tTemInfo.m_tConfInfo.GetConfE164(), TRUE ) )
                {
                    cServMsg.SetErrorCode( ERR_MCU_CONFE164_REPEAT );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    ConfLog( FALSE, "template %s E164 repeated and create failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
                    return;
                }
                
                //会议名已存在，拒绝
                if( g_cMcuVcApp.IsConfNameRepeat( tTemInfo.m_tConfInfo.GetConfName(), TRUE ) )
                {
                    cServMsg.SetErrorCode( ERR_MCU_CONFNAME_REPEAT );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    ConfLog( FALSE, "template %s name repeated and create failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
                    return;
                }           

                if(!g_cMcuVcApp.AddTemplate(tTemInfo))
                {
                    // 返回的错误码
                    cServMsg.SetErrorCode( ERR_MCU_TEMPLATE_NOFREEROOM );
                    ConfLog(FALSE, "[DaemonProcMcsMcuTemplateOpr] add template %s failed\n", tTemInfo.m_tConfInfo.GetConfName());
                    SendReplyBack(cServMsg, pcMsg->event+2);
                    return;
                }                 
                cServMsg.SetConfId( tTemInfo.m_tConfInfo.GetConfId() );
            }
            else
            {                
                TTemplateInfo tOldTemInfo;
                if(!g_cMcuVcApp.GetTemplate(byConfIdx, tOldTemInfo))
                {
                    cServMsg.SetErrorCode( ERR_MCU_TEMPLATE_NOTEXIST );
                    ConfLog(FALSE, "[DaemonProcMcsMcuTemplateOpr] get template confIdx<%d> failed\n", byConfIdx);
                    SendReplyBack(cServMsg, pcMsg->event+2);
                    return;
                }

                //会议的E164号码已存在，拒绝 
                BOOL32 bSameE164 = (0 == strcmp((s8*)tTemInfo.m_tConfInfo.GetConfE164(), (s8*)tOldTemInfo.m_tConfInfo.GetConfE164()));
                if( !bSameE164 &&
                    g_cMcuVcApp.IsConfE164Repeat( tTemInfo.m_tConfInfo.GetConfE164(), TRUE ) )
                {
                    cServMsg.SetErrorCode( ERR_MCU_CONFE164_REPEAT );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    ConfLog( FALSE, "template %s E164 repeated and modify failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
                    return;
                }
                
                //会议名已存在，拒绝
                BOOL32 bSameName = (0 == strcmp( (s8*)tTemInfo.m_tConfInfo.GetConfName(), (s8*)tOldTemInfo.m_tConfInfo.GetConfName()));
                if( !bSameName &&
                    g_cMcuVcApp.IsConfNameRepeat( tTemInfo.m_tConfInfo.GetConfName(), TRUE ) )
                {
                    cServMsg.SetErrorCode( ERR_MCU_CONFNAME_REPEAT );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    ConfLog( FALSE, "template %s name repeated and create failure!\n", tTemInfo.m_tConfInfo.GetConfName() );
                    return;
                }    
                
                if(!g_cMcuVcApp.ModifyTemplate(tTemInfo, bSameE164))
                {
                    ConfLog(FALSE, "[DaemonProcMcsMcuTemplateOpr] modify template %s failed\n", tTemInfo.m_tConfInfo.GetConfName());
                    SendReplyBack(cServMsg, pcMsg->event+2);
                    return;
                }

                if(!bSameE164)
                {
                    cServMsg.SetConfId(tOldTemInfo.m_tConfInfo.GetConfId());
                    SendMsgToAllMcs(MCU_MCS_DELTEMPLATE_NOTIF, cServMsg); 
                }
            }
			// 对于存在无需处理的信息，将其保存在分开的文件中
			if (pszUnProcInfoHead != NULL && wUnProcLen != 0)
			{
				CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
				u8      byConfPos  = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;

				// 在头信息记录中 获取 会议或模板的相对位置
				GetAllConfHeadFromFile(acConfId, sizeof(acConfId));

				//查找已有会议：此会议已保存，覆盖，不包含缺省会议位置
				for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
				{
					if (acConfId[nPos] == tTemInfo.m_tConfInfo.GetConfId())
					{
						byConfPos = (u8)nPos;
						break;
					}
				}
				//一般走不到，该模板不存在未存于文件中
				if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE == byConfPos)
				{
					OspPrintf(TRUE, FALSE, "[DaemonProcMcsMcuTemplateOpr] confinfo_head.dat has no temp with specified confid\n");
				}
				else
				{
					SetUnProConfDataToFile(byConfPos, pszUnProcInfoHead, wUnProcLen);
				}
			}

            SendReplyBack(cServMsg, pcMsg->event+1);
            g_cMcuVcApp.TemInfo2Msg(tTemInfo, cServMsg);
            SendMsgToAllMcs(MCU_MCS_TEMSCHCONFINFO_NOTIF, cServMsg); 
        }
        break;

    case MCS_MCU_DELTEMPLATE_REQ:
        {            
            if(!g_cMcuVcApp.DelTemplate(byConfIdx))
            {
                ConfLog(FALSE, "[DaemonProcMcsMcuTemplateOpr] del template confidx<%d> failed\n", byConfIdx);
                SendReplyBack(cServMsg, pcMsg->event+2);
                return;
            }
            SendReplyBack(cServMsg, pcMsg->event+1);
            SendMsgToAllMcs(MCU_MCS_DELTEMPLATE_NOTIF, cServMsg); 
        }
        break;

    default:
        break;
    }

    return;
}

/*=============================================================================
  函 数 名： DaemonProcGkRegRsp
  功    能： gk注册响应处理函数
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::DaemonProcGkRegRsp(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 byConfIdx = cServMsg.GetConfIdx();
    
    if ( 0 == byConfIdx )
    {
        if ( m_tConfInStatus.IsRegGkNackNtf() && MT_MCU_REGISTERGK_NACK == pcMsg->event )
        {
            //MCU 注册GK别名冲突
            u16 wErrCode = cServMsg.GetErrorCode();
            if ( ERR_MCU_RAS_DUPLICATE_ALIAS == wErrCode ) 
            {
                NotifyMcsAlarmInfo( 0, wErrCode );
                m_tConfInStatus.SetRegGkNackNtf(FALSE);
            }
        }
        return;
    }
    else
    {
        if ( MT_MCU_REGISTERGK_NACK == pcMsg->event )
        {
            //模板 或 会议 注册GK别名冲突
            u16 wErrCode = cServMsg.GetErrorCode();
            if ( ERR_MCU_RAS_DUPLICATE_ALIAS == wErrCode ) 
            {
                NotifyMcsAlarmInfo( 0, wErrCode );
            }
        }
    }

    if ( byConfIdx < MIN_CONFIDX || byConfIdx > MAX_CONFIDX )
    {
        ConfLog(FALSE, "[DaemonProcGkRegRsp] confidx received :%d\n", byConfIdx);
        return;
    }

    //更新会议注册状态
    CMcuVcInst *pcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
    if (NULL != pcInst && pcInst->CurState() == STATE_ONGOING)
    {
        g_cMcuVcApp.SendMsgToConf(byConfIdx, pcMsg->event, pcMsg->content, pcMsg->length);
    }
    
    //更新模板注册状态
    TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
    if(!tMapData.IsTemUsed())
    {            
        return;
    }

    if(MT_MCU_REGISTERGK_ACK == pcMsg->event)
    {
        g_cMcuVcApp.SetTemRegGK(byConfIdx, TRUE);
    }
    else if(MT_MCU_UNREGISTERGK_ACK == pcMsg->event)
    {
        g_cMcuVcApp.SetTemRegGK(byConfIdx, FALSE);
    }
    else if(MT_MCU_REGISTERGK_NACK == pcMsg->event)
    {
        TTemplateInfo tTemInfo;
        if(g_cMcuVcApp.GetTemplate(byConfIdx, tTemInfo))
        {
            u8 byReg = 0;
            cServMsg.SetMsgBody(&byReg, sizeof(byReg));
            cServMsg.SetConfId(tTemInfo.m_tConfInfo.GetConfId());
            SendMsgToAllMcs( MCU_MCS_CONFREGGKSTATUS_NOTIF, cServMsg ); 
        }
    }

    return;
}

/*=============================================================================
  函 数 名： DaemonProcGKChargeRsp
  功    能： GK计费响应处理函数
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/11/09    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::DaemonProcGKChargeRsp( const CMessage * pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );

    u8 byInstID = 0;
    CApp *pcApp = &g_cMcuVcApp;
    CMcuVcInst* pcVcInst = NULL;

    switch( cServMsg.GetEventId() )
    {
    // zbq [03/26/2007] GK 计费注册情况通知
    case MT_MCU_CHARGE_REGGK_NOTIF:

        if ( 0 != cServMsg.GetErrorCode() )
        {
            // 只有计费的MCU的数量超出 GK容量：128
            NotifyMcsAlarmInfo( 0, cServMsg.GetErrorCode() );
            g_cMcuVcApp.SetChargeRegOK( FALSE );
        }
        else
        {
            g_cMcuVcApp.SetChargeRegOK( TRUE );
        }

        CMcuVcInst * pcVcInst;
        for ( ; byInstID <= MAXNUM_MCU_CONF; byInstID++ )
        {
            pcVcInst = (CMcuVcInst *)pcApp->GetInstance(byInstID);
            if ( NULL != pcVcInst )
            {
                if( STATE_IDLE != pcVcInst->CurState() )
                {
                    g_cMcuVcApp.SendMsgToConf( pcVcInst->m_byConfIdx, cServMsg.GetEventId(),
                                               cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
                }
            }
        }
        break;

    case MT_MCU_CONF_STARTCHARGE_ACK:
		
        g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfIdx(), cServMsg.GetEventId(), 
                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		ConfLog( FALSE, "conf<%d> start charge success !\n", cServMsg.GetConfIdx() );
        break;
        
    case MT_MCU_CONF_STARTCHARGE_NACK:

		NotifyMcsAlarmInfo( 0, cServMsg.GetErrorCode() );        
        g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfIdx(), cServMsg.GetEventId(), NULL, 0 );
        ConfLog( FALSE, "conf<%d> start charge failed !\n", cServMsg.GetConfIdx() );
        break;

    case MT_MCU_CONF_STOPCHARGE_ACK:

        g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfIdx(), cServMsg.GetEventId(), NULL, 0 );
        ConfLog( FALSE, "conf<%d> stop charge success !\n", cServMsg.GetConfIdx() );
    	break;

    case MT_MCU_CONF_STOPCHARGE_NACK:

		NotifyMcsAlarmInfo( 0, cServMsg.GetErrorCode() );
        ConfLog( FALSE, "conf<%d> stop charge failed !\n", cServMsg.GetConfIdx() );
        break;

	case MT_MCU_CONF_CHARGEEXP_NOTIF:

		NotifyMcsAlarmInfo( 0, cServMsg.GetErrorCode() );
		g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfIdx(), cServMsg.GetEventId(), NULL, 0 );
		ConfLog( FALSE, "conf<%d> charge status has got exception !\n", cServMsg.GetConfIdx() );
		break;

    default:
        break;
    }
    return;
}

/*=============================================================================
    函数名      ：DaemonProcNPlusRegBackupRsp
    功能        ：N+1 备份主MCU注册备份MCU响应处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
-------------------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    06/12/18    4.0         张宝卿         创建
=============================================================================*/
void CMcuVcInst::DaemonProcNPlusRegBackupRsp( const CMessage * pcMsg )
{
    // 这里只处理NACK，ACK相关可以在读状态的时候直接读到
    if ( MCU_NPLUS_REG_NACK != pcMsg->event )
    {
        ConfLog( FALSE, "[DaemonProcNPlusRegBackupRsp] unexpected msg.%d<%s> received !\n",
                                                pcMsg->event, OspEventDesc(pcMsg->event) );
        return;
    }
    if ( MCU_NPLUS_MASTER_IDLE != g_cNPlusApp.GetLocalNPlusState() ) 
    {
        ConfLog( FALSE, "[DaemonProcNPlusRegBackupRsp] unexpected NPlus state.%d !\n",
                                                   g_cNPlusApp.GetLocalNPlusState() );
        return;
    }
    // guzh [1/15/2007]
/*
    //更新McuStatus
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    TMcuStatus tMcuStatus;
    if ( GetMcuCurStatus(tMcuStatus) ) 
    {
        tMcuStatus.m_byNPlusState = (u8)cServMsg.GetErrorCode();
    }
*/
    return;
}

/*=============================================================================
  函 数 名： AssignIdleConfInsID
  功    能： 分配会议空闲实例
  算法实现： 
  全局变量： 
  参    数： void
             u8 byStartInsId 
  返 回 值： u8 
=============================================================================*/
u8 CMcuVcInst::AssignIdleConfInsID(u8 byStartInsId)
{
    CApp *pcApp = &g_cMcuVcApp;
    CMcuVcInst* pcVcInst = NULL;
    for (u8 byInstID = byStartInsId; byInstID <= MAXNUM_MCU_CONF; byInstID++)
    {
        pcVcInst = (CMcuVcInst *)pcApp->GetInstance(byInstID);
        if (NULL != pcVcInst)
        {
            if(STATE_IDLE == pcVcInst->CurState())
            {
                return byInstID;
            }
        }
    }

    return 0;
}

/*====================================================================
    函数名      ：DaemonProcMcsMcuCreateConfReq
    功能        ：处理会控召开的会议
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/01/25    4.0         张宝卿         创建
====================================================================*/
void CMcuVcInst::DaemonProcMcsMcuCreateConfReq(const CMessage * pcMsg)
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );

    u8 byInsID = 0;
    if ( g_cMcuVcApp.GetMpNum() > 0 || 
         g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323) > 0 )
    {
        // N+1 模式下，不允许会控创会
        if ( g_cNPlusApp.GetLocalNPlusState() != MCU_NPLUS_SLAVE_IDLE &&
             g_cNPlusApp.GetLocalNPlusState() != MCU_NPLUS_SLAVE_SWITCH )
        {
            byInsID = AssignIdleConfInsID(byInsID+1);
            if(0 != byInsID)
            {
                cServMsg.SetConfIdx(0);
                cServMsg.SetSrcMtId(CONF_CREATE_MCS);
                ::OspPost(MAKEIID( AID_MCU_VC, byInsID ), MCS_MCU_CREATECONF_REQ, 
                                cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                CApp *pcApp = &g_cMcuVcApp;
                CMcuVcInst *pInst = (CMcuVcInst *)pcApp->GetInstance( byInsID );
            }
            else
            {
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                ConfLog(FALSE, "[DaemonProcMcsMcuCreateConfReq] assign instance id failed!\n");
            }                        
        }
        else
        {
            NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_NPLUS_CREATETEMPLATE );
        }
    }
    else
    {
        //一般不会跑到这里
        NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_CREATECONF_MCS_NOMPMTADP );
    }
    
    return;
}

/*====================================================================
    函数名      ：ProcTimerWaitMpRegCheck
    功能        ：(1) 定时检查Mp是否注册, 从而开启相应等待开启的会议
                  (2) 如果是即时会议，置实例状态 STATE_WAITEQP
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/17    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcTimerWaitMpRegCheck( const CMessage * pcMsg )
{
    CServMsg cServMsg;

	TConfStore tConfStoreBuf;
	TPackConfStore *ptPackConfStore = (TPackConfStore *)&tConfStoreBuf;		
	u16 wAliasBufLen = 0;
	u8* pbyAliasBuf  = NULL; //终端别名数组打包缓冲
	u8* pbyModuleBuf = NULL; //电视墙//画面合成模板缓冲
    u8 byInsID = 0;
	u8 achMTPackInfo[SERV_MSG_LEN - SERV_MSGHEAD_LEN];  //存放分组信息的缓存

    BOOL32 bHasMpMtAdp = (g_cMcuVcApp.GetMpNum() > 0 && 
                          g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323) > 0);

    BOOL32 bPowerOn = (BOOL32)(*(u32*)pcMsg->content);

    // guzh [4/13/2007] 修改策略
    // 目前策略为：上电后第一次即刻恢复所有模版
    // 但是会议则等待MP/MtAdp

	for (u8 byLoop = 0; byLoop < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); byLoop++)
	{
        if ( !bPowerOn && !bHasMpMtAdp )
        {
            break;
        }
        
		//提取后的 tConfStore 为 TPackConfStore 结构，即已进行Pack处理的会议数据
        if (g_cMcuVcApp.HasConfStore( byLoop ) && ::GetConfFromFile(byLoop, ptPackConfStore))
		{
			wAliasBufLen = htons(ptPackConfStore->m_wAliasBufLen);
			pbyAliasBuf  = (u8 *)(ptPackConfStore+1);
			pbyModuleBuf = pbyAliasBuf+ptPackConfStore->m_wAliasBufLen;

            // guzh [4/11/2007] 在这里标记从文件创会
            ptPackConfStore->m_tConfInfo.m_tStatus.SetTakeFromFile(TRUE);

            cServMsg.SetMsgBody((u8*)&ptPackConfStore->m_tConfInfo, sizeof(TConfInfo));
            cServMsg.CatMsgBody((u8*)&wAliasBufLen, 2 );
            cServMsg.CatMsgBody(pbyAliasBuf, ntohs(wAliasBufLen) );

            //电视墙模板 
            if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
            {
                cServMsg.CatMsgBody(pbyModuleBuf, sizeof(TMultiTvWallModule));
                pbyModuleBuf += sizeof(TMultiTvWallModule);
            }
            //画面合成模板 
            if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasVmpModule())
            {
                cServMsg.CatMsgBody(pbyModuleBuf, sizeof(TVmpModule));
                pbyModuleBuf += sizeof(TVmpModule);
			}

			// 对于VCS创建的会议模板，包含所需要的额外信息 
			//                  + 1byte(u8: 0 1  是否配置了高清电视墙)
			//                  +(可选, THDTvWall)
			//                  + 1byte(u8: 会议配置HDU的通道总数)
			//                  + (可选, THduModChnlInfo+...)	
			//                  + 1byte(是否为级联调度)＋(可选，2byte[u16 网络序，配置打包的总长度]+下级mcu配置[1byte(终端类型)+1byte(别名长度)+xbyte(别名字符串)+2byte(呼叫码率)...)])
			//                  + 1byte(是否支持分组)＋(可选，2byte(u16 网络序，信息总长)+内容(1byte(组数)+n组[1TVCSGroupInfo＋m个TVCSEntryInfo])
			if (VCS_CONF == ptPackConfStore->m_tConfInfo.GetConfSource())
			{
				u8* pbyVCSInfoBuf = pbyModuleBuf;
				u8 byIsSupportHDTW = *pbyVCSInfoBuf++;
				cServMsg.CatMsgBody(&byIsSupportHDTW, sizeof(u8));
				if (byIsSupportHDTW)
				{
					cServMsg.CatMsgBody((u8*)pbyVCSInfoBuf, sizeof(THDTvWall));
					pbyVCSInfoBuf += sizeof(THDTvWall);
				}

				u8 byHduNum = *pbyVCSInfoBuf++;
				cServMsg.CatMsgBody(&byHduNum, sizeof(u8));
				if (byHduNum)
				{
					cServMsg.CatMsgBody((u8*)pbyVCSInfoBuf, byHduNum * sizeof(THduModChnlInfo));
					pbyVCSInfoBuf += byHduNum * sizeof(THduModChnlInfo);
				}
				
				u8 bySMCUExist = *pbyVCSInfoBuf++;
				cServMsg.CatMsgBody(&bySMCUExist, sizeof(u8));
				if (bySMCUExist)
				{
					wAliasBufLen = htons(*(u16*)pbyVCSInfoBuf);
					cServMsg.CatMsgBody((u8*)&wAliasBufLen, sizeof(u16));
					pbyVCSInfoBuf += sizeof(u16);
					wAliasBufLen = ntohs(wAliasBufLen);
					cServMsg.CatMsgBody((u8*)pbyVCSInfoBuf, wAliasBufLen);
					pbyVCSInfoBuf += wAliasBufLen;					
				}

				u8 byMTPackExist = *pbyVCSInfoBuf++;
				if (byMTPackExist)
				{
					if (GetUnProConfDataToFile(byLoop, (s8*)achMTPackInfo, wAliasBufLen))
					{
						// 成功获取对应分组
						cServMsg.CatMsgBody((u8*)&byMTPackExist, sizeof(u8));
						cServMsg.CatMsgBody(achMTPackInfo, wAliasBufLen);
					}
					else
					{
						// 文件可能被删除
						byMTPackExist =  FALSE;
						cServMsg.CatMsgBody((u8*)&byMTPackExist, sizeof(u8));
					}
				}
				else
				{
					cServMsg.CatMsgBody(&byMTPackExist, sizeof(u8));
				}
			}
            if ( ptPackConfStore->m_tConfInfo.m_tStatus.IsTemplate() && bPowerOn)
            {
                // 如果是模版,则保存  
				TTemplateInfo  tTemInfo;
                g_cMcuVcApp.Msg2TemInfo(cServMsg, tTemInfo);
                g_cMcuVcApp.AddTemplate(tTemInfo);

                g_cMcuVcApp.SetConfRegState( byLoop + 1, FALSE );
                SendMsgToAllMcs(MCU_MCS_TEMSCHCONFINFO_NOTIF, cServMsg); 
                continue;
            }
            else if (!ptPackConfStore->m_tConfInfo.m_tStatus.IsTemplate() &&
                     bHasMpMtAdp && 
                     g_cNPlusApp.GetLocalNPlusState() != MCU_NPLUS_MASTER_CONNECTED &&
                     g_cNPlusApp.GetLocalNPlusState() != MCU_NPLUS_MASTER_IDLE)
            {                
                byInsID = AssignIdleConfInsID(byInsID+1);
                if(0 != byInsID)
                {
                    cServMsg.SetConfIdx(byLoop+1);
                    ::OspPost(MAKEIID( AID_MCU_VC, byInsID ), MCU_CREATECONF_FROMFILE, 
                        cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                    CApp *pcApp = &g_cMcuVcApp;
                    CMcuVcInst *pInst = (CMcuVcInst *)pcApp->GetInstance( byInsID );
                    pInst->NextState(STATE_WAITEQP);

                    OspPrintf(TRUE, FALSE, "MCU_CREATECONF_FROMFILE send to Ins.%d, byLoop.%d\n", byInsID, byLoop);
                }
            }
		}
	}

	if (!bHasMpMtAdp)
	{
		SetTimer( MCUVC_WAIT_MPREG_TIMER, TIMESPACE_WAIT_MPREG, FALSE);
		if ( 0 == g_cMcuVcApp.GetMpNum())
		{
			ConfLog( FALSE, "[MCU]: NO Mp regitered, Waiting......\n" );
		}
		if ( 0 == g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323))
		{
			ConfLog( FALSE, "[MCU]: NO MtAdp regitered, Waiting......\n" );
		}
	}
	
	return;
}

/*====================================================================
    函数名      ：ProcTimerScheduledCheck
    功能        ：预约会议定时检查处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/01    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcTimerScheduledCheck( const CMessage * pcMsg )
{
	CServMsg cServMsg;
	
	switch( CurState() )
	{
	case STATE_SCHEDULED:

		//未到开始时间
		if( time( NULL ) < m_tConf.GetStartTime() )	
		{
			SetTimer( MCUVC_SCHEDULED_CHECK_TIMER, TIMESPACE_SCHEDULED_CHECK );
			break;
		}
		
        // guzh [5/17/2007] 标记为即时会议
        m_tConf.m_tStatus.SetOngoing();
        g_cMcuVcApp.ConfInfoMsgPack(this, cServMsg);
        
		cServMsg.SetSrcMtId( CONF_CREATE_SCH );
        cServMsg.SetConfIdx(m_byConfIdx);
		
		//不要使用别名转发；虽然此消息能正确发出，但在ReleaseConf()）中，别名又会被删除
		post( MAKEIID( GetAppID(), GetInsID() ), MCU_SCHEDULE_CONF_START, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		
		ConfLog(FALSE, "CMcuVcInst: A Scheduled conference %s started!\n", m_tConf.GetConfName() );
        
		//release conference
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
    函数名      ：ProcTimerOngoingCheck
    功能        ：进行中会议定时检查是否结束处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/01    1.0         LI Yi         创建
	03/11/15    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcTimerOngoingCheck( const CMessage * pcMsg )
{
	CServMsg	cServMsg;
	u16 wLeftMin;

	switch( CurState() )
	{
	case STATE_ONGOING:
		//会议结束前一刻钟提示所有会控与主席终端
		if( time( NULL ) > m_tConf.GetStartTime() + m_tConf.GetDuration() * 60 - 15*60 )	//not stop
		{
			wLeftMin = m_tConf.GetDuration() - (time( NULL ) - m_tConf.GetStartTime())/60;

			if( wLeftMin == 15 || wLeftMin == 10 || wLeftMin == 5 
				|| wLeftMin == 4 || wLeftMin == 3|| wLeftMin == 2 || wLeftMin == 1 )
			{
				wLeftMin = htons( wLeftMin );
				cServMsg.SetMsgBody( (u8*)&wLeftMin, 2);  

				BroadcastToAllSubMtJoinedConf( MCU_MT_CONFWILLEND_NOTIF, cServMsg );   
				SendMsgToAllMcs( MCU_MCS_CONFTIMELEFT_NOTIF, cServMsg );
			}
		}

        //检查是否到了结束时间
		if( time( NULL ) < m_tConf.GetStartTime() + m_tConf.GetDuration() * 60 )	//not stop
		{
			SetTimer( MCUVC_ONGOING_CHECK_TIMER, TIMESPACE_ONGOING_CHECK );
			break;
		}
		
		//结束会议
		ReleaseConf( TRUE );
		ConfLog( FALSE, "Conference %s released at stop time!\n", m_tConf.GetConfName() );

		NEXTSTATE( STATE_IDLE );
		break;
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcTimerInviteUnjoinedMt
    功能        ：定时邀请未与会终端处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/03    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcTimerInviteUnjoinedMt( const CMessage * pcMsg )
{
	CServMsg	cServMsg;
    TMt tMt;
	u8  byLoop;

#ifdef _DEBUG
	s8	achStr[64];
	m_tConf.GetConfId().GetConfIdString( achStr, sizeof( achStr ) );
	log( LOGLVL_DEBUG1, "CMcuVcInst: Conference %s (confid=%s) now invite all unjoined MT!\n", 
		m_tConf.GetConfName(), achStr );
#endif

	switch( CurState() )
	{
	case STATE_ONGOING:

		//send invite message to all connected MTs
		cServMsg.SetConfId( m_tConf.GetConfId() );
		cServMsg.SetNoSrc();
		cServMsg.SetTimer( TIMESPACE_WAIT_AUTOINVITE );
		cServMsg.SetMsgBody( NULL, 0 );
		for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			if(!m_tConfAllMtInfo.MtInConf( byLoop ) )
			{
				continue;
			}
			if(m_tConfAllMtInfo.MtJoinedConf(byLoop))
			{
				continue;
			}
            // libo [3/30/2005]
            if (m_ptMtTable->GetAddMtMode(byLoop) == ADDMTMODE_CHAIRMAN)
			{
				continue;
			}
            // libo [3/30/2005]end

			if( CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode( byLoop ) )
			{
				u32 dwLeftTimes = m_ptMtTable->GetCallLeftTimes( byLoop );

				//无穷次呼叫，则直接进行呼叫
				if( DEFAULT_CONF_CALLTIMES == m_tConf.m_tStatus.GetCallTimes() )
				{
					tMt = m_ptMtTable->GetMt( byLoop );
					InviteUnjoinedMt( cServMsg, &tMt );
				}
				//有限次呼叫，检测剩余呼叫次数
				else if( dwLeftTimes > 0 )
				{
					dwLeftTimes--;
					m_ptMtTable->SetCallLeftTimes( byLoop, dwLeftTimes );
					tMt = m_ptMtTable->GetMt( byLoop );
					InviteUnjoinedMt( cServMsg, &tMt );
				}
			}			
		}		
		
		SetTimer( MCUVC_INVITE_UNJOINEDMT_TIMER, 1000*m_tConf.m_tStatus.GetCallInterval() );	
		
		break;
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcCommonOperAck
    功能        ：终端或外设操作同意应答统一处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/14    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcCommonOperAck( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:
		//send messge
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcCommonOperNack
    功能        ：终端或外设操作拒绝应答统一处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/14    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcCommonOperNack( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:
		//send messge
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：DaemonProcAppTaskRequest
    功能        ：GUARD模块探测消息处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/21    1.0         杨皞昀         创建
====================================================================*/
void CMcuVcInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );
}

/*====================================================================
    函数名      ：DaemonProcCommonNotif
    功能        ：终端或外设普通通知处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/20    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::DaemonProcCommonNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	switch( pcMsg->event )
	{
	case REC_MCU_EXCPT_NOTIF:
		SendMsgToAllMcs( MCU_MCS_RECORDEREXCPT_NOTIF, cServMsg );

		break;
	default:
		break;
	}
}



/*====================================================================
    函数名      ：DaemonProcTestMcuGetAllMasterConfIdCmd
    功能        ：处理Tester获得所用以本MCU为主MCU的会议
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/04    1.0         JQL           创建
====================================================================*/
void CMcuVcInst::DaemonProcTestMcuGetAllMasterConfIdCmd( const CMessage* pcMsg )
{	
    /*
	CConfId aCConfId[MAXNUM_MCU_CONF];
	u16 wIndex = 0;

	memset( aCConfId, 0, sizeof( aCConfId ) );
	
    for(u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
    {
        CMcuVcInst *pcIns = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
        if(NULL != pcIns)
        {
            aCConfId[wIndex++] = pcIns->m_tConf.GetConfId();
        }
    }	

	//确保能发送数据到Tester
	if( wIndex == 0 )
		wIndex = 1;
	SetSyncAck( ( const u8*)aCConfId, wIndex* sizeof( CConfId ) );

    */  
}

/*====================================================================
    函数名      : DaemonProcTestMcuGetConfFullInfoCmd
    功能        ：处理Tester获得所用以本MCU为主MCU的会议
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/04    1.0         JQL           创建
====================================================================*/
void CMcuVcInst::DaemonProcTestMcuGetConfFullInfoCmd( const CMessage* pcMsg )
{
    /*
	CConfId cConfId = *(CConfId*)pcMsg->content;

    u8 byConfIdx = g_cMcuVcApp.GetConfIdx(cConfId);
    CMcuVcInst *pcIns = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
    if(NULL != pcIns)
    {
        SetSyncAck( (const u8*)&pcIns->m_tConf, sizeof( TConfInfo ) );
    }
    */
}

/*====================================================================
    函数名      ：ReleaseMtRes
    功能        ：释放MT占用的资源
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bForced	[in]忽略MT类型
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/07/22    3.0         胡昌威        创建
	2/4/2009	4.5			薛亮		  修改 
====================================================================*/
void CMcuVcInst::ReleaseMtRes( u8 byMtId, BOOL32 bNeglectMtType )
{
    u8 byDriId = m_ptMtTable->GetDriId(byMtId);
	
	TCapSupport tCapSupport;
	m_ptMtTable->GetMtCapSupport(byMtId,&tCapSupport);
	// xliang [10/27/2008] 判断是否是HD MT 且mcu内置接入
	if( tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 &&
		(tCapSupport.GetMainVideoResolution() == VIDEO_FORMAT_HD1080 ||
		tCapSupport.GetMainVideoResolution() == VIDEO_FORMAT_HD720 )&&
		/*tCapSupport.GetMainVideoResolution() == VIDEO_FORMAT_4CIF*/ 
		( byDriId == MCU_BOARD_MPC || 
		byDriId == MCU_BOARD_MPCD ) )
	{
		g_cMcuVcApp.DecMtAdpMtNum( byDriId, m_byConfIdx, byMtId, TRUE);
	}
	else
	{
		g_cMcuVcApp.DecMtAdpMtNum( byDriId, m_byConfIdx, byMtId );
	}
	// xliang [2/4/2009] 对于终端类型是MCU的情形，计数值还需-1
	u8 byMtType = m_ptMtTable->GetMtType(byMtId);
	if( (MT_TYPE_MMCU == byMtType || MT_TYPE_SMCU == byMtType)
		&& !bNeglectMtType )
	{
		g_cMcuVcApp.DecMtAdpMtNum( byDriId, m_byConfIdx, byMtId, FALSE,TRUE );
	}

	u8 byMpId = m_ptMtTable->GetMpId( byMtId );
	g_cMcuVcApp.DecMpMtNum( byMpId, m_byConfIdx, byMtId, m_tConf.GetBitRate() );
}

/*=============================================================================
    函 数 名： StopAllMtRecoderToPeriEqp
    功    能： 停止所有终端录像：停止向录像机交换
    算法实现： 
    全局变量： 
    参    数：
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/03/30  3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::StopAllMtRecoderToPeriEqp( )
{
	for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
	{					
		if( m_tConfAllMtInfo.MtJoinedConf( byMtId ) && 
			!m_ptMtTable->IsMtNoRecording( byMtId ) )
		{
			TEqp tRecEqp;
			u8   byRecChn;

			m_ptMtTable->GetMtRecordInfo( byMtId ,&tRecEqp, &byRecChn );
			StopSwitchToPeriEqp( tRecEqp.GetEqpId(), byRecChn);
			m_ptMtTable->SetMtNoRecording(byMtId);
		}
	}
}

/*====================================================================
    函数名      ：ReleaseConf
    功能        ：从会议表中删除该会议；
	              通知外设，MC，MT和SMCU；
				  停止会议中所有交换；
				  停止所有定时器（MASTER）；
				  删除文件（SCHEDULE）；
				  回到IDLE状态
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/08    1.0         LI Yi         创建
	03/11/13    3.0         胡昌威        修改
	05/12/20	4.0			张宝卿		  T120集成
====================================================================*/
void CMcuVcInst::ReleaseConf( BOOL32 bRemoveFromFile )
{
	CServMsg	cServMsg;
	TConfMtInfo	tConfMtInfo;
	TPeriEqpStatus tPeriEqpStatus;
//	u8  byVmpConfIdx = 0;     //  hdu选看画面合成时，结束会议时保存vmp confIdx,
	                          //  以便停止hdu时正确获得选看vmp的会议索引号
	                          
//	TMt tMt;
    
	//delete file
	//对于VCS会议不保存当前会议信息于文件
	if( bRemoveFromFile && 
		( CurState() == STATE_SCHEDULED || CurState() == STATE_ONGOING )
		&& m_tConf.GetConfSource() != VCS_CONF)
	{
		if(!g_cMcuVcApp.RemoveConfFromFile( m_tConf.GetConfId() ))
        {
            OspPrintf(TRUE, FALSE, "[ReleaseConf] remove conf from file failed\n");
        }
	}

    // xsl [11/16/2006] 是否支持gk计费
    if ( 0 != g_cMcuAgent.GetGkIpAddr() &&g_cMcuAgent.GetIsGKCharge() &&  
         0 != g_cMcuVcApp.GetRegGKDriId() &&
         m_tConf.IsSupportGkCharge() && m_tConf.m_tStatus.IsGkCharge() )
    {
        //向GK发送停止计费请求
        g_cMcuVcApp.ConfChargeByGK( m_byConfIdx, g_cMcuVcApp.GetRegGKDriId(), TRUE );
        
    }

	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetNoSrc();

    //Notify all meeting consoles
    u8 byMode = CONF_TAKEMODE_ONGOING;
    if(STATE_SCHEDULED == CurState())
    {
        byMode = CONF_TAKEMODE_SCHEDULED;
    }
//    cServMsg.SetMsgBody(&byMode, sizeof(byMode));
//    SendMsgToAllMcs( MCU_MCS_RELEASECONF_NOTIF, cServMsg );
    
	
    TConfMapData tMapData;
    if(m_byConfIdx < MIN_CONFIDX || m_byConfIdx > MAX_CONFIDX)
    {
        OspPrintf(TRUE, FALSE, "[ReleaseConf] invalid confidx :%d\n", m_byConfIdx);
    }
    else
    {
        tMapData = g_cMcuVcApp.GetConfMapData(m_byConfIdx);
    }
	//在GK上注销 
	if( !tMapData.IsTemUsed() && m_tConf.m_tStatus.IsRegToGK() > 0 && g_cMcuVcApp.GetRegGKDriId() > 0 )
	{
		TMtAlias tMtAlias;
		tMtAlias.SetE164Alias( m_tConf.GetConfE164() );
		cServMsg.SetConfId( m_tConf.GetConfId() );
		cServMsg.SetConfIdx( m_byConfIdx );
		cServMsg.SetDstDriId( g_cMcuVcApp.GetRegGKDriId() );
		cServMsg.SetMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
		g_cMtAdpSsnApp.SendMsgToMtAdpSsn( g_cMcuVcApp.GetRegGKDriId(), MCU_MT_UNREGISTERGK_REQ, cServMsg );

		//注销的会议注册记录后 同步 其他适配模块板的 注册MCU及会议实体 注册信息
		g_cMpManager.UpdateRRQInfoAfterURQ( m_byConfIdx );
	}
    
	if( STATE_SCHEDULED == CurState() )
	{
		//释放端口
//		for( u8 byIndex = 1; byIndex <= MAXNUM_CONF_MT; byIndex++ )
//		{
//			if( m_tConfAllMtInfo.MtInConf( byIndex ) )
//			{
//				ReleaseMtRes( byIndex );
//			}
//		}
		
		g_cMcuVcApp.RemoveConf( m_byConfIdx );

        // [12/08/2006]
        cServMsg.SetMsgBody(&byMode, sizeof(byMode));
        SendMsgToAllMcs( MCU_MCS_RELEASECONF_NOTIF, cServMsg );

        //clear map info
        g_cMcuVcApp.SetConfMapInsId(m_byConfIdx, 0);
		
		DeleteAlias();
		
		ConfLog( FALSE, "schedule Conference %s was released!\n", m_tConf.GetConfName() );

		ClearVcInst();
//		MCU_SAFE_DELETE(m_ptMtTable)
		return;
	}

	///////////////////////////////////////////////
	//以下只有即时会议需要处理    

	//发Trap消息
	TConfNotify tConfNotify;
	CConfId cConfId;
	cConfId = m_tConf.GetConfId( );
	memcpy( tConfNotify.m_abyConfId, &cConfId, 16 );
	memcpy( tConfNotify.m_abyConfName, m_tConf.GetConfName(), MAXLEN_CONFNAME );
    
    SendTrapMsg( SVC_AGT_CONFERENCE_STOP, (u8*)&tConfNotify, sizeof(tConfNotify) );
 	
	//杀死所有时钟
	KillTimer( MCUVC_REFRESH_MCS_TIMER );
	KillTimer( MCUVC_INVITE_UNJOINEDMT_TIMER );
	KillTimer( MCUVC_POLLING_CHANGE_TIMER );
    KillTimer( MCUVC_TWPOLLING_CHANGE_TIMER );
	KillTimer( MCUVC_MCUSRC_CHECK_TIMER );
	KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
	KillTimer( MCUVC_HDUPOLLING_CHANGE_TIMER );
	KillTimer( MCUVC_VCMTOVERTIMER_TIMER );
	KillTimer( MCUVC_VCS_CHAIRPOLL_TIMER );

	//拆除所有交换桥
	u8 bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
	if( !m_tVidBrdSrc.IsNull() )
	{
		g_cMpManager.RemoveSwitchBridge( m_tVidBrdSrc, bySrcChnnl, MODE_VIDEO );
	}
	
	//停止双流
	ClearH239TokenOwnerInfo( NULL );
	StopDoubleStream( TRUE, TRUE );

	if( !m_tAudBrdSrc.IsNull() )
	{
		g_cMpManager.RemoveSwitchBridge( m_tAudBrdSrc, bySrcChnnl, MODE_AUDIO );
	}

	//组播会议组播数据
	if( m_tConf.GetConfAttrb().IsMulticastMode() && !m_tVidBrdSrc.IsNull() )
	{
		g_cMpManager.StopMulticast( m_tVidBrdSrc, bySrcChnnl );
	}

	//停止所有交换
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        if (!m_tVidBrdSrc.IsNull())
        {
#ifdef _SATELITE_
			g_cMpManager.StopSatConfCast(m_tVidBrdSrc, CAST_FST, MODE_BOTH, bySrcChnnl);
			g_cMpManager.StopSatConfCast(m_tVidBrdSrc, CAST_SEC, MODE_BOTH, bySrcChnnl);

			for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
			{
				if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
					byMtId != m_tVidBrdSrc.GetMtId())
				{
					TMt tMtNull;
					tMtNull.SetNull();
					m_ptMtTable->SetMtSrc(byMtId, &tMtNull, MODE_BOTH);
				}
			}
#else
			g_cMpManager.StopDistrConfCast(m_tVidBrdSrc, MODE_BOTH, bySrcChnnl);
#endif
        }        
    }
    else
    {
        StopSwitchToAllSubMtJoinedConf( MODE_BOTH );
    }

	StopSwitchToAllMcInConf();	
	StopSwitchToAllPeriEqpInConf();

	//停止混音
	if( m_tConf.m_tStatus.IsMixing() || m_tConf.m_tStatus.IsVACing() )
	{
		StopMixing();

		//放弃混音组
 		g_cMcuVcApp.GetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
		tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState = TMixerGrpStatus::READY;
        tPeriEqpStatus.SetConfIdx( 0 );
		g_cMcuVcApp.SetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );

        cServMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
        SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
	}

	//停止画面合成
	if (CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode())
	{
		SendMsgToEqp( m_tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cServMsg );

		//放弃画面合成器
 		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus );
		tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
//		byVmpConfIdx = tPeriEqpStatus.GetConfIdx();
		tPeriEqpStatus.SetConfIdx( 0 );
		g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tPeriEqpStatus ); 

        if (m_tConf.m_tStatus.GetVmpParam().IsVMPBrdst())
        {
            ChangeVidBrdSrc(NULL);
        }
	}
    
    //停止多画面电视墙
    if ( CONF_VMPTWMODE_NONE != m_tConf.m_tStatus.GetVmpTwMode() )
    {
        SendMsgToEqp( m_tVmpTwEqp.GetEqpId(), MCU_VMPTW_STOPVIDMIX_REQ, cServMsg );

        //释放vpu资源
        g_cMcuVcApp.GetPeriEqpStatus( m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus );
        tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = FALSE;
        tPeriEqpStatus.SetConfIdx( 0 );
        g_cMcuVcApp.SetPeriEqpStatus( m_tVmpTwEqp.GetEqpId(), &tPeriEqpStatus );
    }
	
	//停止会议录像：停止向录像机交换
	if( m_tConf.m_tStatus.IsRecording() /*&& HasJoinedSpeaker()*/ )
	{
        StopSwitchToPeriEqp( m_tRecEqp.GetEqpId(), m_byRecChnnl, TRUE );
	}
	
	//停止所有终端录像：停止向录像机交换
	StopAllMtRecoderToPeriEqp( );

	//停止放像
	if( !m_tConf.m_tStatus.IsNoPlaying() )
	{
		cServMsg.SetMsgBody( ( u8 * )&m_tPlayEqp, sizeof( m_tPlayEqp ) );	//set TEqp
		cServMsg.SetChnIndex( m_byPlayChnnl );
		SendMsgToEqp( m_tPlayEqp.GetEqpId(), MCU_REC_STOPPLAY_REQ, cServMsg );
	}

	//停止媒体类型适配, 释放预留适配器 -- modify bas 2
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

	//丢包重传停止
	if( m_tConf.m_tStatus.IsPrsing() )
	{
		StopPrs(PRSCHANMODE_BOTH, TRUE);

        StopPrs(PRSCHANMODE_AUDIO, TRUE);
        StopPrs(PRSCHANMODE_AUDBAS, TRUE);
        StopPrs(PRSCHANMODE_VIDBAS, TRUE);
        StopPrs(PRSCHANMODE_BRBAS, TRUE);

        // FIXME：高清适配丢包重传未添加, zgc

		// xliang [4/30/2009] FIXME 广播源4出
		StopPrs(PRSCHANMODE_VMPOUT1, TRUE);
		StopPrs(PRSCHANMODE_VMPOUT2, TRUE);
		StopPrs(PRSCHANMODE_VMPOUT3, TRUE);
		StopPrs(PRSCHANMODE_VMPOUT4, TRUE);
	}

	//释放保留通道
	if( m_tConf.GetConfAttrb().IsResendLosePack() )
	{
		TPeriEqpStatus tStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl].SetReserved( FALSE );
		tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnl2].SetReserved( FALSE );
        tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAud].SetReserved( FALSE );
        if(EQP_CHANNO_INVALID != m_byPrsChnnlAudBas && m_byPrsChnnlAudBas < MAXNUM_PRS_CHNNL)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlAudBas].SetReserved(FALSE);
        }
        if(EQP_CHANNO_INVALID != m_byPrsChnnlVidBas && m_byPrsChnnlVidBas < MAXNUM_PRS_CHNNL)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVidBas].SetReserved(FALSE);
        }
        if(EQP_CHANNO_INVALID != m_byPrsChnnlBrBas && m_byPrsChnnlBrBas < MAXNUM_PRS_CHNNL)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlBrBas].SetReserved(FALSE);
        }
		// xliang [4/30/2009] 释放广播源4出对应的PRS通道
		if(EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut1 && m_byPrsChnnlVmpOut1 < MAXNUM_PRS_CHNNL)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut1].SetReserved(FALSE);
        }
		if(EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut2 && m_byPrsChnnlVmpOut2 < MAXNUM_PRS_CHNNL)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut2].SetReserved(FALSE);
        }
		if(EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut3 && m_byPrsChnnlVmpOut3 < MAXNUM_PRS_CHNNL)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut3].SetReserved(FALSE);
        }
		if(EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut4 && m_byPrsChnnlVmpOut4 < MAXNUM_PRS_CHNNL)
        {
            tStatus.m_tStatus.tPrs.m_tPerChStatus[m_byPrsChnnlVmpOut4].SetReserved(FALSE);
        }
		g_cMcuVcApp.SetPeriEqpStatus( m_tPrsEqp.GetEqpId(), &tStatus );
	}

	//停止轮询
	TPollInfo *ptPollInfo = m_tConf.m_tStatus.GetPollInfo();
	if( ptPollInfo->GetMediaMode() == MODE_VIDEO)
	{
		ChangeVidBrdSrc( NULL );
	}
    
	//停止hdu批量轮询
	if ( POLL_STATE_NONE != m_tHduBatchPollInfo.GetStatus() )
	{
		u8 byHduChlNum = 0;
		u8 byHduEqpId = 0;
		u8 byChnlIdx = 0;
		u8 byChnlPollNum = m_tHduBatchPollInfo.GetChnlPollNum();
		TPeriEqpStatus tHduStatus;
        for ( ; byHduChlNum < byChnlPollNum; byHduChlNum ++ )
        {
			byHduEqpId = m_tHduBatchPollInfo.m_tChnlBatchPollInfo[byHduChlNum].GetEqpId();
			byChnlIdx = m_tHduBatchPollInfo.m_tChnlBatchPollInfo[byHduChlNum].GetChnlIdx();
			if ( byHduEqpId >= HDUID_MIN && byHduEqpId <= HDUID_MAX )
			{
                g_cMcuVcApp.GetPeriEqpStatus( byHduEqpId, &tHduStatus );
				if ( 0 == tHduStatus.m_byOnline )    // hdu不在线
				{
					tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].SetNull();
					g_cMcuVcApp.SetPeriEqpStatus( byHduEqpId, &tHduStatus );
					cServMsg.SetMsgBody( (u8*)&tHduStatus, sizeof(tHduStatus) );
					SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF,  cServMsg);
				}
			}
			else
			{
				CfgLog( FALSE, "[ProcMcsMcuHduBatchPollMsg] stop hdu(%d) batch poll error!\n", byHduEqpId);
				return;
			}
        }
		
		m_tHduBatchPollInfo.SetNull();
		m_tConf.m_tStatus.m_tConfMode.SetHduInBatchPoll( POLL_STATE_NONE );
		
		TConfAttrbEx tConfAttrbEx = m_tConf.GetConfAttrbEx();
		tConfAttrbEx.SetSchemeIdxInBatchPoll( 0 );
		m_tConf.SetConfAttrbEx( tConfAttrbEx );
		
        m_tHduPollSchemeInfo.SetNull();
        ConfStatusChange();            // 通知会控会议状态改变

		//通知所有会控
		cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
		SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
		
		KillTimer( MCUVC_HDUBATCHPOLLI_CHANGE_TIMER );
	}

	//停止向电视墙交换
	u8  byLoop = 0,  byEqpId = 0;
	TPeriEqpStatus tTvwallStatus, tHduStatus;
	memset(&tTvwallStatus, 0x0, sizeof(tTvwallStatus));
	memset(&tHduStatus, 0x0, sizeof(tHduStatus));

	//停止HDU
	for ( byEqpId=HDUID_MIN; byEqpId < HDUID_MAX; byEqpId++ )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byEqpId) && 
			EQP_TYPE_HDU == g_cMcuVcApp.GetEqpType(byEqpId))
		{			
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tHduStatus);
            if (1 == tHduStatus.m_byOnline)
            {
                u8 byMtConfIdx;
				u8 byMtMainType;

				for(byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; byLoop++)
				{
                    byMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
					byMtMainType = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetType();
					if ( m_byConfIdx == byMtConfIdx ) // || TYPE_MCUPERI == byMtMainType
					{
						StopSwitchToPeriEqp(byEqpId, byLoop);
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = 0;
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
					}
				}
				tHduStatus.m_tStatus.tHdu.byChnnlNum = 0;
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tHduStatus);
				
                cServMsg.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
            }
		}

	}
	//停止电视墙
	for ( byEqpId=TVWALLID_MIN; byEqpId < TVWALLID_MAX; byEqpId++ )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byEqpId) && 
			EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
		{			
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
            if (1 == tTvwallStatus.m_byOnline)
            {
                u8 byMtConfIdx;
                u8 byMemberNum = tTvwallStatus.m_tStatus.tTvWall.byChnnlNum;
				for(byLoop = 0; byLoop < byMemberNum; byLoop++)
				{
                    byMtConfIdx = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
					if (m_byConfIdx == byMtConfIdx)
					{
						StopSwitchToPeriEqp(byEqpId, byLoop);
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
					}
				}
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tTvwallStatus);
				
                cServMsg.SetMsgBody((u8 *)&tTvwallStatus, sizeof(tTvwallStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
            }
		}
	}

	//拆除所有与会终端的dump
	if (NULL != m_ptMtTable)
	{
		for( byLoop = 0; byLoop < m_ptMtTable->m_byMaxNumInUse; byLoop++)
		{
			TMt tMcuMt = m_ptMtTable->GetMt(byLoop + 1);
			if(!tMcuMt.IsNull())
			{
				if( m_ptMtTable->IsMtAudioSending( tMcuMt.GetMtId() ) )
				{
					g_cMpManager.StopRecvMt( tMcuMt, MODE_AUDIO );
				}
				if( m_ptMtTable->IsMtVideoSending( tMcuMt.GetMtId() ) )
				{
					g_cMpManager.StopRecvMt( tMcuMt, MODE_VIDEO );
				}			
			}	
		}

	}

	//挂断所有终端(将所有可能正在处理呼叫的终端也挂断)
	cServMsg.SetConfIdx( m_byConfIdx );
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetDstMtId( 0 );
	cServMsg.SetMsgBody( NULL, 0 );
	g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn( MCU_MT_DELMT_CMD, cServMsg);
	
	//释放端口
	for( u8 byIndex = 1; byIndex <= MAXNUM_CONF_MT; byIndex++ )
	{	
		if( m_tConfAllMtInfo.MtInConf( byIndex ) )
		{
			ReleaseMtRes( byIndex );

            // xsl [10/11/2006] 释放端口的同时释放终端交换地址信息
            g_cMcuVcApp.ReleaseMtPort( m_byConfIdx, byIndex );
            m_ptMtTable->ClearMtSwitchAddr( byIndex );
		}
	}

	cServMsg.SetConfIdx( m_byConfIdx );
	g_cMpSsnApp.BroadcastToAllMpSsn( MCU_MP_RELEASECONF_NOTIFY,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());

	//如果是数据会议
	if ( CONF_DATAMODE_VAONLY != m_tConf.GetConfAttrb().GetDataMode() )
	{
		//且已经召开
		if ( m_tConfInStatus.IsDataConfOngoing() )
		{
			//告知DCS删除该会议
			SendMcuDcsReleaseConfReq();
		}
	}
	
    // [12/08/2006]
    cServMsg.SetMsgBody(&byMode, sizeof(byMode));
    SendMsgToAllMcs( MCU_MCS_RELEASECONF_NOTIF, cServMsg );

	//从数据区清除
	g_cMcuVcApp.RemoveConf( m_byConfIdx );

    //clear map info
    g_cMcuVcApp.SetConfMapInsId(m_byConfIdx, 0);

	DeleteAlias();
	ConfLog( FALSE, "Ongoing Conference %s was released!\n", m_tConf.GetConfName() );

    //通知n+1备份服务器更新会议信息
    if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
    {
        ProcNPlusConfInfoUpdate(FALSE);
    }
    
    //释放高清适配通道
    StopHDMVBrdAdapt();
    StopHDDSBrdAdapt();
    StopHDCascaseAdp();

    //释放mau
    m_cBasMgr.ReleaseHdBasChn();
	//释放选看BAS通道
	u8 byChnlNum = 0;
	u8 abyBasId[MAXNUM_PERIEQP]   = {0};
	u8 abyChnlIdx[MAXNUM_PERIEQP] = {0};
	TPeriEqpStatus tEqpStatus;
	m_cSelChnGrp.GetUsedAdptChnlInfo(byChnlNum, abyBasId, abyChnlIdx);
	for (u8 byIdx = 0; byIdx < byChnlNum; byIdx++)
	{
		CServMsg cServMsg;
		cServMsg.SetChnIndex(abyChnlIdx[byIdx]);
		cServMsg.SetConfId(m_tConf.GetConfId());
		if (!g_cMcuAgent.IsEqpBasHD(abyBasId[byIdx]))
		{
			//向适配器发送停止消息
			SendMsgToEqp(abyBasId[byIdx], MCU_BAS_STOPADAPT_REQ, cServMsg);

			//释放通道
			if (g_cMcuVcApp.GetPeriEqpStatus(abyBasId[byIdx], &tEqpStatus))
			{
				tEqpStatus.m_tStatus.tBas.tChnnl[abyChnlIdx[byIdx]].SetStatus(TBasChnStatus::READY);
				tEqpStatus.m_tStatus.tBas.tChnnl[abyChnlIdx[byIdx]].SetReserved(FALSE);
				g_cMcuVcApp.SetPeriEqpStatus(abyBasId[byIdx], &tEqpStatus);
				g_cMcuVcApp.SendPeriEqpStatusToMcs(abyBasId[byIdx]);
			}

		}
		else
		{
			SendMsgToEqp(abyBasId[byIdx], MCU_HDBAS_STOPADAPT_REQ, cServMsg);
			g_cMcuVcApp.ReleaseHDBasChn(abyBasId[byIdx], abyChnlIdx[byIdx]);
		}

	}
	m_cSelChnGrp.Clear();

#ifdef _SATELITE_

	//停止所有的MODEM,同时频点释放
	//if ( g_byMcuModemNum != 0 )
	{
		//停止MCU的MODEM
		StopConfModem( m_tConf.GetConfAttrbEx().GetSatDCastChnlNum() );
		
		//释放频点
		if ( m_tConf.GetConfAttrbEx().GetSatDCastChnlNum() != 0 )
		{
			ReleaseConfFreq();
		}
		g_cMcuVcApp.SetConfGetReceiveFreq( m_byConfIdx, FALSE );
		g_cMcuVcApp.SetConfGetSendFreq( m_byConfIdx, FALSE );
	}

	ReleaseSatFrequence();

	//释放终端
	m_tConfAllMtInfo.RemoveAllJoinedMt();
	RefreshConfState();
	
#endif

	//清空会议实例
	ClearVcInst();

#ifdef _SATELITE_
	//RefreshConfState();
#endif

	//通知所有会议,当前可能有终端
	post( MAKEIID( GetAppID(), CInstance::EACH ), MCUVC_INVITE_UNJOINEDMT_TIMER );

	return;
}

/*====================================================================
    函数名      ：AdjustMtSendBR
    功能        ：调节终端发送码率
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId    要调节的MT
				  u16 wBitrate 目标码率
				  u8 byMode    视频类型 MODE_VIDEO / MODE_SECVIDEO  
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/03/20    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::AdjustMtVideoSrcBR( u8 byMtId, u16 wBitrate, u8 byMode )
{
	if( CurState() == STATE_ONGOING )
	{
		CServMsg cServMsg;
		TLogicalChannel tLogicalChannel;
		if( MODE_VIDEO == byMode )
		{
			TMt tMtSrc;
			m_ptMtTable->GetMtSrc( byMtId, &tMtSrc, MODE_VIDEO );
			if( tMtSrc.GetType() == TYPE_MT && 
				tMtSrc.GetMtId() > 0 &&
				m_ptMtTable->GetMtLogicChnnl( tMtSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE ) )
			{
				tLogicalChannel.SetFlowControl( wBitrate );
				cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );

                //zbq[07/28/2009] 双流flowctrl修正：不响应非免适配终端的flowctrl，AdpParam自然调整
                if (IsHDConf(m_tConf) &&
                    tMtSrc == m_tVidBrdSrc &&
                    m_cMtRcvGrp.IsMtNeedAdp(byMtId))
                {
                }
                else
                {
                    SendMsgToMt( tMtSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );
                }
			}
		}
		else
		{
			if( TYPE_MT == m_tDoubleStreamSrc.GetType() && 
				m_tConfAllMtInfo.MtJoinedConf( m_tDoubleStreamSrc.GetMtId() ) && 
				m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) )
			{
				tLogicalChannel.SetFlowControl( wBitrate );
				cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
				
                //zbq[07/28/2009] 双流flowctrl修正：不响应非免适配终端的flowctrl，AdpParam自然调整
                if (IsHDConf(m_tConf) &&
                    m_cMtRcvGrp.IsMtNeedAdp(byMtId, FALSE))
                {
                }
                else
                {
                    SendMsgToMt( m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );
                }
			}
		}
	}

	return;
}

/*====================================================================
    函数名      ：StartMtSendbyNeed
    功能        ：在需要的时候让终端开始发
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt & tSrc 要停发的终端
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/02/14    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::StartMtSendbyNeed( TSwitchChannel* ptSwitchChannel )
{
	if( CurState() == STATE_ONGOING )
	{	
		TMt tSrc =  ptSwitchChannel->GetSrcMt();

		if( !m_tConfAllMtInfo.MtJoinedConf( tSrc.GetMtId() ) )
		{
			return;
		}
		
		if( tSrc.GetType() == TYPE_MT && !ptSwitchChannel->IsNull() )
		{
			u8  byMode;
			if (ptSwitchChannel->GetRcvPort() % PORTSPAN == 2)
			{
				byMode = MODE_AUDIO;
			}
			else if (ptSwitchChannel->GetRcvPort() % PORTSPAN == 0)
			{
				byMode = MODE_VIDEO;
			}
			else
			{
				return;
			}

            u32 dwVmpIp = 0;
            u8 byEqpType = 0;
            if (!m_tVmpEqp.IsNull())
            {
                g_cMcuAgent.GetPeriInfo(m_tVmpEqp.GetEqpId(), &dwVmpIp, &byEqpType);
            }

            if (tSrc == m_tVidBrdSrc &&
                IsDelayVidBrdVCU() &&
                dwVmpIp == ptSwitchChannel->GetDstIP())
            {
                NotifyMtSend( tSrc.GetMtId(), byMode, TRUE );

                NotifyFastUpdate(tSrc, 0, TRUE);
                CallLog("[StartMtSendbyNeed] NotifyFastUpdate Mt.%d util here due to Tandberg!\n", tSrc.GetMtId());
            }
            else
            {
                NotifyMtSend( tSrc.GetMtId(), byMode, TRUE );
            }
        }
    }
    return;
}

/*====================================================================
函数名      ：StopMtSendbyNeed
功能        ：在需要的时候让终端停发
算法实现    ：
引用全局变量：
输入参数说明：const TMt & tSrc 要停发的终端
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
04/02/14    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::StopMtSendbyNeed( TSwitchChannel* ptSwitchChannel )
{
	if( CurState() == STATE_ONGOING )
	{
		TMt tSrc =  ptSwitchChannel->GetSrcMt();

		if( !m_tConfAllMtInfo.MtJoinedConf( tSrc.GetMtId() ) )
		{
			return;
		}
		
		if( tSrc.GetType() == TYPE_MT )
		{
			u8  byMode;
			if (ptSwitchChannel->GetRcvPort() % PORTSPAN == 2)
			{
				byMode = MODE_AUDIO;
			}
			else if (ptSwitchChannel->GetRcvPort() % PORTSPAN == 0)
			{
				byMode = MODE_VIDEO;
			}
			else
			{
				return;
			}

			u16 wRcvPort = ptSwitchChannel->GetRcvPort();
//			u8  byMpId  = m_ptMtTable->GetMpId( tSrc.GetMtId() );
			u32 dwSrcIp = m_ptMtTable->GetIPAddr( tSrc.GetMtId() );
            u32 dwDstIp = ptSwitchChannel->GetDstIP();

//			if( byMode == MODE_AUDIO )
//			{
//			    NotifyMtSend( tSrc.GetMtId(), byMode, FALSE );
//			}
//			else
			{
                //只有移除交换后才通知终端停止发送码流，防止错误的flowcontrol 0
				if( !m_ptSwitchTable->IsValidSwitchSrcIp( dwSrcIp, wRcvPort, byMode ) )
				{
					//zbq[09/10/2008] 广播逻辑下广播源切换有时序问题，此处临时规避
					if (MT_TYPE_MT == m_tVidBrdSrc.GetMtType() &&
						m_tVidBrdSrc.GetMtId() == m_ptMtTable->GetMtIdByIp(dwSrcIp))
					{
						MpManagerLog("[StopMtSendbyNeed] Src<%s@%d> remove slapped due to it's vidsrc.(removed dst<%s@%d> switch)\n", 
									  StrOfIP(dwSrcIp), wRcvPort, StrOfIP(dwDstIp), ptSwitchChannel->GetDstPort() );						
					}
					else
					{
						MpManagerLog("[StopMtSendbyNeed] Src<%s@%d> Is a invalid switch src.(removed dst<%s@%d> switch)\n", 
									   StrOfIP(dwSrcIp), wRcvPort, StrOfIP(dwDstIp), ptSwitchChannel->GetDstPort() );
						NotifyMtSend( tSrc.GetMtId(), byMode, FALSE );
					}
				}
				else
				{
					MpManagerLog("[StopMtSendbyNeed] Src<%s@%d> Is a valid switch src.(removed dst<%s@%d> switch)\n", 
                                StrOfIP(dwSrcIp), wRcvPort, StrOfIP(dwDstIp), ptSwitchChannel->GetDstPort() );

                    // xsl [8/1/2006] 若4M以上会议，移除画面合成通道后，终端还在向其他*发送码流，则修改终端发送码率
                    if (m_tConf.GetBitRate() > MAXBITRATE_MTSEND_INVMP)
                    {
                        u32 dwVmpIp; 
                        u8 byPeriType;
                        if (SUCCESS_AGENT == g_cMcuAgent.GetPeriInfo(m_tVmpEqp.GetEqpId(), &dwVmpIp, &byPeriType) &&
                            ntohl(dwVmpIp) == dwDstIp)
                        {
                            NotifyMtSend(tSrc.GetMtId(), MODE_VIDEO);
                        }
                        else if (SUCCESS_AGENT == g_cMcuAgent.GetPeriInfo(m_tVmpTwEqp.GetEqpId(), &dwVmpIp, &byPeriType) &&
                                ntohl(dwVmpIp) == dwDstIp)
                        {
                            NotifyMtSend(tSrc.GetMtId(), MODE_VIDEO);
                        }
                    }
				}
			}
		}
	}	
}

/*=============================================================================
函 数 名： NotifyRecStartPlaybyNeed
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/2/27  4.0			周广程                  创建
=============================================================================*/
void CMcuVcInst::NotifyRecStartPlaybyNeed( TSwitchChannel* ptSwitchChannel )
{
	CServMsg cServMsg;
	cServMsg.SetChnIndex( m_byPlayChnnl );
	if( ptSwitchChannel->GetSrcMt() == m_tPlayEqp && m_tConf.m_tStatus.IsNoPlaying() )
	{
		u8  byMode;
		if (ptSwitchChannel->GetRcvPort() % PORTSPAN == 2)
		{
			byMode = MODE_AUDIO;
		}
		else if (ptSwitchChannel->GetRcvPort() % PORTSPAN == 0)
		{
			byMode = MODE_VIDEO;
		}
		else if ( ptSwitchChannel->GetRcvPort() % PORTSPAN == 4 )
		{
			byMode = MODE_SECVIDEO;
		}
		else
		{
			return;
		}
			
		if( m_tPlayEqpAttrib.IsDStreamPlay() && m_tPlayEqpAttrib.IsDStreamFile() )
		{
			if( byMode == MODE_AUDIO || byMode == MODE_VIDEO )
			{
				//音频和第一路视频不作处理
			}
			else
			{
				KillTimer( MCUVC_RECPLAY_WAITMPACK_TIMER );
				m_tConf.m_tStatus.SetPlaying();
				SendMsgToEqp( m_tPlayEqp.GetEqpId() , MCU_EQP_SWITCHSTART_NOTIF, cServMsg);
			}
		}
		else
		{
			if( byMode == MODE_AUDIO )
			{
				//如果正在进行VMP广播，则仍向REC发送通知，否则音频不作处理
				if ( m_tConf.m_tStatus.IsBrdstVMP() )
				{
					KillTimer( MCUVC_RECPLAY_WAITMPACK_TIMER );
					m_tConf.m_tStatus.SetPlaying();
					SendMsgToEqp( m_tPlayEqp.GetEqpId() , MCU_EQP_SWITCHSTART_NOTIF, cServMsg);
				}
            }
            else
            {
				KillTimer( MCUVC_RECPLAY_WAITMPACK_TIMER );
                m_tConf.m_tStatus.SetPlaying();
                SendMsgToEqp( m_tPlayEqp.GetEqpId() , MCU_EQP_SWITCHSTART_NOTIF, cServMsg);
			}
		}
	}
}

/*====================================================================
    函数名      ：NotifyMtSend
    功能        ：通知直连下级MT发送码流，同时通知互斥终端停发
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byDstMtId, 要通知的MT
				  u8 byMode, 发送模式，缺省为MODE_BOTH
				  BOOL32 bStart, TRUE-开始 FALSE-停止
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/19    1.0         LI Yi         创建
	04/02/14    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::NotifyMtSend( u8 byDstMtId, u8 byMode, BOOL32 bStart )
{
	CServMsg	cServMsg;
	TLogicalChannel tLogicalChannel;
	u16  wBitRate;

    u8 byManuId = m_ptMtTable->GetManuId( byDstMtId );
	if(byManuId != MT_MANU_KDC && byManuId != MT_MANU_KDCMCU) // xsl [8/7/2006] 支持mcu flowcontrol
	{
        //非keda厂商在节省带宽、视频、发送码流时
        if((MODE_BOTH == byMode || MODE_VIDEO == byMode) && bStart && g_cMcuVcApp.IsSavingBandwidth())
        {
            NotifyOtherMtSend(byDstMtId, TRUE);
        }
		return;
	}

	//通知终端开始发送。如果发送的是音频，将码率设为64k
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		if( m_ptMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_AUDIO, &tLogicalChannel, FALSE ) )
		{
			if( bStart )
			{
				u16 wAudioBand = GetAudioBitrate( m_tConf.GetMainAudioMediaType() );
				tLogicalChannel.SetFlowControl( wAudioBand );
				cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
				SendMsgToMt( byDstMtId, MCU_MT_FLOWCONTROL_CMD, cServMsg );
				
				TMt tDstMt;
				tDstMt.SetNull();
				cServMsg.SetMsgBody( (u8*)&tDstMt, sizeof(tDstMt) );
				SendMsgToMt( byDstMtId, MCU_MT_SEENBYOTHER_NOTIF, cServMsg );
			}
			else
			{
				wBitRate =  0;		
				tLogicalChannel.SetFlowControl( wBitRate );
				cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
				SendMsgToMt( byDstMtId, MCU_MT_FLOWCONTROL_CMD, cServMsg );	
			}
		}
	}

	//如果为视频，将该终端的码率设为会议码率
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		if( m_ptMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_VIDEO, &tLogicalChannel, FALSE ) )
		{
			if( bStart )
			{
                // xsl [1/21/2006] 如果有双流源，折半，没有则全速；
                // zbq [6/23/2009] 进一步：有双流源，但该终端非主流源，也不接收双流，则全速；
                u16 wDialBitrate = 0;
				
#ifdef _SATELITE_
				wDialBitrate = m_ptMtTable->GetDialBitrate(byDstMtId);
#else
				wDialBitrate = m_ptMtTable->GetSndBandWidth(byDstMtId);
#endif
				if (m_tDoubleStreamSrc.IsNull())
                {
                    tLogicalChannel.SetFlowControl( wDialBitrate );
                }
                else
                {
                    TLogicalChannel tDSFwdChn;
                    if (!(m_tVidBrdSrc == m_ptMtTable->GetMt(byDstMtId)) &&
                        !m_ptMtTable->GetMtLogicChnnl(byDstMtId, LOGCHL_SECVIDEO, &tDSFwdChn, TRUE))
                    {
                        tLogicalChannel.SetFlowControl( wDialBitrate );
                    }
                    else
                    {
                        tLogicalChannel.SetFlowControl( GetDoubleStreamVideoBitrate(wDialBitrate ));
                    }
                }

				cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
				SendMsgToMt( byDstMtId, MCU_MT_FLOWCONTROL_CMD, cServMsg );

				TMt tDstMt;
				tDstMt.SetNull();
				cServMsg.SetMsgBody( (u8*)&tDstMt, sizeof(tDstMt) );
				SendMsgToMt( byDstMtId, MCU_MT_SEENBYOTHER_NOTIF, cServMsg );
			}
			else
			{               
				tLogicalChannel.SetFlowControl( 0 );
				cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
				SendMsgToMt( byDstMtId, MCU_MT_FLOWCONTROL_CMD, cServMsg );	
			}
		}
	}

#ifdef _SATELITE_
	if (byMode == MODE_NONE)
	{
		return;
	}

	//这里要加MODEM控制
	if ( g_byMcuModemNum == 0 ||
		 ::topoGetMtInfo( LOCAL_MCUID, byDstMtId, g_atMtTopo, g_wMtTopoNum).GetConnected() != 1 ||
		 ::topoGetMtInfo( LOCAL_MCUID, byDstMtId, g_atMtTopo, g_wMtTopoNum).GetModemIp() == 0 )
	{
		return;
	}

	if (bStart)
	{
		TMt tMt = m_ptMtTable->GetMt(byDstMtId);
		u8 byModemId = g_cMcuVcApp.GetConfMtUsedMcuModem( m_byConfIdx, tMt, TRUE );

		if (0 != byModemId &&
			MODE_AUDIO == byMode)
		{
			ConfLog(FALSE, "[NotifyMtSend] MT.%d has got modem.%d, ignore the new apply!\n", byDstMtId, byModemId);
			return;
		}

		u32 dwFreq;
		u32 dwBitRate;
		byModemId = g_cMcuVcApp.GetConfIdleMcuModem( m_byConfIdx, TRUE );
		if ( byModemId == 0 )
		{
			ConfLog(FALSE, "[NotifyMtSend] no idle rcv modem bind for MT.%d, bStart.%d!\n", byModemId, bStart);
			return;
		}
		
		g_cMcuVcApp.GetMcuModemRcvData( byModemId, dwFreq, dwBitRate );
		if ( dwFreq != 0)
		{
			TMt tMt = m_ptMtTable->GetMt(byDstMtId);
			
			//tMt.SetMt( LOCAL_MCUID, byDstMtId );
			cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
			cServMsg.CatMsgBody( (u8*)&dwFreq, sizeof(u32) );
			SatLog( "[NotifyMtSend] Mt-%u is send:Freq - %u, bStart.%d\n", byDstMtId, dwFreq, bStart );
			
			g_cMcuVcApp.SetMtOrEqpUseMcuModem( tMt, byModemId, TRUE, TRUE );
			g_cModemSsnApp.SendMsgToModemSsn( byDstMtId + MAXNUM_MCU_MODEM, MCU_MODEM_SEND, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		}
		else
		{
			OspPrintf( TRUE, FALSE, "Cannot Get Frq-%d for this Mt-%u, bStart.%d\n", dwFreq, byDstMtId, bStart );
		}
	}
	else
	{
		TMt tMt = m_ptMtTable->GetMt(byDstMtId);
		u8 byModemId = g_cMcuVcApp.GetConfMtUsedMcuModem( m_byConfIdx, tMt, TRUE );

		if ( byModemId != 0 ) 
		{
			cServMsg.SetMsgBody( (u8*)&tMt,sizeof(tMt) );

			g_cMcuVcApp.ReleaseConfUsedMcuModem( m_byConfIdx, tMt, TRUE );
			g_cModemSsnApp.SendMsgToModemSsn( byDstMtId + MAXNUM_MCU_MODEM, MCU_MODEM_STOPSEND, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );	
		}
	}

	m_ptMtTable->SetMtCurrUpLoad(byDstMtId, bStart);

	RefreshMtBitRate();
#endif

	return;

}

/*====================================================================
    函数名      ：NotifyEqpSend
    功能        ：通知外设(停止)接收码流：主要针对卫星会议中给广播下行外设申请/释放Modem资源
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byDstMtId, 要通知的EqpId
				  u8 byMode
				  BOOL32 bStart, 开始接收还是停止接收
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/11/19    4.6         张宝卿         创建
====================================================================*/
void CMcuVcInst::NotifyEqpSend(u8 byDstEqpId, u8 byMode, BOOL32 bStart)
{
	if (!g_cMcuVcApp.IsPeriEqpConnected(byDstEqpId))
	{
		ConfLog(FALSE, "[NotifyEqpSend] Eqp.%d unexist, check it!\n", byDstEqpId);
		return;
	}
	TEqp tEqp = g_cMcuVcApp.GetEqp(byDstEqpId);

	// 音频、视频输出均已OK.
	// 这里需要给视频第二输出（Vid）申请和释放发送Modem
	// FIXME: 该逻辑稍后进行整合，Modem和多输出作绑定统一管理，音频多输出给多给Modem
	if (byMode == MODE_AUDIO)
	{
		ConfLog(FALSE, "[NotifyEqpSend] byMode.%d, needn't aud sec send, check it!\n");
		return;
	}


}

/*====================================================================
    函数名      ：NotifyMtReceive
    功能        ：通知终端(停止)接收码流
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 bySrcMtId, 接收源终端号
				  u8 byDstMtId, 要通知的MT
				  BOOL32 bStart, 开始接收还是停止接收
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/19    1.0         LI Yi         创建
	04/03/05    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::NotifyMtReceive(  const TMt & tSrc, u8 byDstMtId )
{
	TMt TNullMt, tSeeSrc;
	TNullMt.SetNull();//源为NULL接收外设
	TNullMt.SetMcuId( tSrc.GetMcuId() );
	TMt tDstMt = m_ptMtTable->GetMt( byDstMtId );
	CServMsg cServMsg;

	MtLog("[NotifyMtReceive]mt(%d) see src(mtid:%d, mttype:%d, mtsubtype:%d)\n",
		    byDstMtId, tSrc.GetMtId(), tSrc.GetType(), tSrc.GetMtType());
	if( tSrc.GetType() == TYPE_MT  )
	{
		tSeeSrc = tSrc;
	}
    else if (TYPE_MCUPERI == tSrc.GetType() &&
             EQP_TYPE_BAS == tSrc.GetEqpType() &&
             m_cMtRcvGrp.IsMtNeedAdp(byDstMtId) &&
			 m_tVidBrdSrc.GetType() != TYPE_MCUPERI)
    {
        tSeeSrc = m_tVidBrdSrc;
    }
	else
	{
		tSeeSrc = TNullMt;
	}

	cServMsg.SetMsgBody( (u8*)&tSeeSrc, sizeof(tSeeSrc) );
	SendMsgToMt( tDstMt.GetMtId(), MCU_MT_YOUARESEEING_NOTIF, cServMsg );
}

/*=============================================================================
  函 数 名： NotifyOtherMtSend
  功    能： 通知其他厂商终端是否发送视频码流
  算法实现： 
  全局变量： 
  参    数：  u8 byDstMtId
             BOOL32 bStart
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::NotifyOtherMtSend( u8 byDstMtId, BOOL32 bStart )
{
    CServMsg	cServMsg;
    TLogicalChannel tLogicalChannel;
    u16  wBitRate = 0;

    if( m_ptMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_VIDEO, &tLogicalChannel, FALSE ) )
    {
        if( bStart )
        {            
            //  xsl [1/21/2006] 如果有双流源，折半，没有则全速
            u16 wDialBitrate = m_ptMtTable->GetSndBandWidth(byDstMtId);
            if (!m_tDoubleStreamSrc.IsNull())
            {
                tLogicalChannel.SetFlowControl( GetDoubleStreamVideoBitrate(wDialBitrate) );
            }
            else
            {
                tLogicalChannel.SetFlowControl( wDialBitrate );
            }		
        }
        else
        {     
            tLogicalChannel.SetFlowControl( wBitRate );                
        }        

        cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
        SendMsgToMt( byDstMtId, MCU_MT_FLOWCONTROL_CMD, cServMsg );     
    }

    return;
}

/*=============================================================================
函 数 名： IsNeedAdjustMtSndBitrate
功    能： 是否需要调整终端的发送码率
算法实现： 
全局变量： 
参    数： u8 byMtId
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/22  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMcuVcInst::IsNeedAdjustMtSndBitrate(u8 byMtId, u16 &wSndBitrate)
{   
    TMt tSrcMt = m_ptMtTable->GetMt(byMtId);    
    TMtStatus tStatus;   
    
    //是否发言人或双流源
    if (tSrcMt == GetLocalSpeaker() || tSrcMt == m_tDoubleStreamSrc)
    {        
        //由于非keda终端需要将码率交换回去，所以计算最低接收带宽时需要考虑源终端
        u8 bySrcMtId = 0;
        if (MT_MANU_KDC == m_ptMtTable->GetManuId(byMtId))
        {
            bySrcMtId = byMtId;
        }

        //双速会议调节
        if (0 != m_tConf.GetSecBitRate())
        {
            u16 wMinBitRate = m_ptMtTable->GetDialBitrate(byMtId);
            u16 wMtBitRate;
            
            for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
            {
                if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && bySrcMtId != byLoop)
                {
                    m_ptMtTable->GetMtStatus(byLoop, &tStatus);

                    wMtBitRate = m_ptMtTable->GetMtReqBitrate(byLoop, TRUE);
                    if (wMtBitRate != 0 &&                         
                        (wMtBitRate*(g_cMcuVcApp.GetBitrateScale()+100)/100) >= m_ptMtTable->GetDialBitrate(byMtId) &&
                        wMtBitRate < wMinBitRate)
                    {
                        wMinBitRate = wMtBitRate;
                    }

                    // guzh [3/13/2007] 双流源还可能被选看
                    if ( tSrcMt == m_tDoubleStreamSrc &&
                         !(tSrcMt == m_tVidBrdSrc) &&
                         tStatus.GetSelectMt(MODE_VIDEO) == tSrcMt &&
                         wMtBitRate < wMinBitRate )
                    {
                        wMinBitRate = wMtBitRate;
                    }
                }
            }    
            if (wMinBitRate == m_tConf.GetBitRate())
            {
                return FALSE;
            }
            wSndBitrate = wMinBitRate;            
        }
        else
        {
            //这里只取同格式终端最小码率 (有适配时需适配的终端通过适配器交换)
            // guzh [9/19/2007] FIXME：辅格式发言，最低计算可能存在问题，第二个参数是否应该传递src的格式
            wSndBitrate = GetLeastMtReqBitrate(TRUE, m_tConf.GetMainVideoMediaType(), bySrcMtId);
        }        
                
        return TRUE;
    }   

    //是否被其他终端选看
	u16 wMinBitRate = 0xffff;
	u16 wMtReqBit = 0;
    for(u8 byIdx = 1; byIdx <= MAXNUM_CONF_MT; byIdx++)
    {       
        if (byMtId != byIdx && m_tConfAllMtInfo.MtJoinedConf(byIdx))
        {
            if (m_ptMtTable->GetMtStatus(byIdx, &tStatus))
            {
                if (tStatus.GetSelectMt(MODE_VIDEO) == tSrcMt)
                {                
                    // guzh [9/19/2007] FIXME：考虑被多个终端选看的情况
					// zgc [2008-01-22] 已修改FIXME
                    wMtReqBit = m_ptMtTable->GetMtReqBitrate(byIdx, TRUE);
					wMinBitRate = ( wMinBitRate <= wMtReqBit ) ? wMinBitRate : wMtReqBit;
                }
            }
        }
    }	
	if ( wMinBitRate < 0xffff )
	{
		wSndBitrate = wMinBitRate;
		return TRUE;
	}

    return FALSE;
}

/*=============================================================================
  函 数 名： IsMtSendingVideo
  功    能： 判断发双流终端是否发送第一路码流
  算法实现： 开双流时调节第一路码率，关双流时恢复第一路码率
  全局变量： 
  参    数： u8 byMtId
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcInst::IsDStreamMtSendingVideo(TMt tMt)
{
	TMtStatus tStatus;	

	//是否广播源
	if (tMt == GetLocalSpeaker())
	{
		return TRUE;
	}

	//是否在录像
	m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tStatus);
	if (tStatus.m_tRecState.IsRecording())
	{
		return TRUE;
	}

	//是否在回传通道里
    if (!m_tCascadeMMCU.IsNull())
    {
        TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
        if (NULL != ptInfo &&
            m_tConfAllMtInfo.MtJoinedConf(ptInfo->m_tSpyMt.GetMcuId(), ptInfo->m_tSpyMt.GetMtId()) &&
            tMt == ptInfo->m_tSpyMt) 
        {   
            return TRUE;
        }        
    }   

	//是否被其他终端选看
	for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (byLoop == m_tDoubleStreamSrc.GetMtId() || !m_tConfAllMtInfo.MtJoinedConf(byLoop))
        {
            continue;
        }      
		
		if (m_ptMtTable->GetMtStatus(byLoop, &tStatus))
        {
            if (tStatus.GetSelectMt(MODE_VIDEO) == tMt)
            {               
                return TRUE;
            }
        }
    }

	return FALSE;
}

/*====================================================================
    函数名      ：ChangeSelectSrc
    功能        ：改变选看源
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/06/02    3.6         LI BO         创建
    06/04/24    4.0         张宝卿        检测源和终端的能力集
====================================================================*/
BOOL CMcuVcInst::ChangeSelectSrc(TMt tSrcMt, TMt tDstMt, u8 byMode)
{
    TSwitchInfo  tSwitchInfo;
    tSwitchInfo.SetDstMt(tDstMt);
    tSwitchInfo.SetSrcMt(tSrcMt);
    
    if (m_tConf.m_tStatus.IsMixing())
    {
        tSwitchInfo.SetMode(MODE_VIDEO);
    }
    else
    {
        tSwitchInfo.SetMode(byMode);
    }
    
    //zbq[01/03/2008]禁止下级选看上级
    if ( !tDstMt.IsLocal() ||
         (tDstMt.IsLocal() && m_ptMtTable->GetMtType(tDstMt.GetMtId()) == MT_TYPE_SMCU) )
    {
        ConfLog(FALSE, "[ChangeSelectSrc] DstMt<%d,%d> isn't local, ignore it\n", tDstMt.GetMcuId(), tDstMt.GetMtId());
        return FALSE;
    }
    
    //取源终端与目的终端的能力集
    TSimCapSet tSrcSCS;
    if (tSrcMt.IsLocal())
    {
        tSrcSCS = m_ptMtTable->GetSrcSCS( tSrcMt.GetMtId() );
    }
    else
    {
        TMt tLocalSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);
        tSrcSCS = m_ptMtTable->GetSrcSCS( tLocalSrcMt.GetMtId() );
    }    
    TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS( tDstMt.GetMtId() );
    
    //选看模式和对应能力集不匹配
    BOOL32 bAccord2Adp = FALSE;
    u8 bySwitchMode = tSwitchInfo.GetMode();
    if ( !IsSelModeAndCapMatched( bySwitchMode, tSrcSCS, tDstSCS, bAccord2Adp ) ) 
    {
        OspPrintf( TRUE, FALSE, "[ChangeSelectSrc]Select mode.%d failed!\n", tSwitchInfo.GetMode() );
        return FALSE;
    }
    tSwitchInfo.SetMode( bySwitchMode );

    // 这里需要考虑是否是广播交换, zgc, 2008-06-03
    if( tSrcMt == m_tVidBrdSrc &&
        ( !( (tSrcMt==m_tRollCaller && GetLocalMtFromOtherMcuMt(tDstMt)==GetLocalMtFromOtherMcuMt(m_tRollCallee)) 
          || (tSrcMt==GetLocalMtFromOtherMcuMt(m_tRollCallee) && tDstMt==m_tRollCaller) ) ) &&
        ( MODE_VIDEO == tSwitchInfo.GetMode() || MODE_BOTH == tSwitchInfo.GetMode() ) )
    {
        ConfLog( FALSE, "[ChangeSelectSrc]Dst(Mt.%d) receive Src(Mt.%d) as video broadcast src!\n", tDstMt.GetMtId(), tSrcMt.GetMtId() );

        StartSwitchFromBrd( tSrcMt, 0, 1, &tDstMt );
        
        if ( MODE_BOTH == tSwitchInfo.GetMode() )
        {
            tSwitchInfo.SetMode( MODE_AUDIO );
        }
        else
        {
            // 这里可以直接return，因为在这种情况下实际并没有发生选看
            return TRUE;
        }
	}
    
    // guzh [2/28/2007] 记录选看
    TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
    tMtStatus.SetSelectMt( tSrcMt, tSwitchInfo.GetMode() );
    m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tMtStatus );
    
    CServMsg    cServMsgHdr;
    cServMsgHdr.SetEventId(MCS_MCU_STARTSWITCHMT_REQ);
    SwitchSrcToDst(tSwitchInfo, cServMsgHdr);    

    //ConfLog( FALSE, "Mt.%d select set Mt.%d, mode.%d\n", tDstMt.GetMtId(), tSrcMt.GetMtId(), byMode );
    ConfLog( FALSE, "Mt.%d select set Mt.%d, mode.%d\n", tDstMt.GetMtId(), tSrcMt.GetMtId(), tSwitchInfo.GetMode() );

    return TRUE;
}


/*====================================================================
    函数名      ：StopSelectSrc
    功能        ：停止终端选看
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt tDstMt, u8 byMode, u8 byIsRestore
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	07/11/14	4.0			张宝卿         创建
====================================================================*/
void CMcuVcInst::StopSelectSrc( TMt tDstMt, u8 byMode, u8 byIsRestore )
{
	//跨格式选看释放对应的适配资源
	ReleaseResbySel(tDstMt, byMode);

    TMt tNullMt;
    TMtStatus tMtStatus; 

	TMt tSrc;
	TMtStatus tSrcStatus;
	
    //change mode if mixing
    if ( MODE_BOTH == byMode )
    {
        if( m_tConf.m_tStatus.IsMixing() )
        {
            byMode = MODE_VIDEO;
        }
    }
	 
    ConfLog(FALSE, "Mcu%dMt%d Cancel Select See.\n", tDstMt.GetMcuId(), tDstMt.GetMtId() );
    
    // guzh [7/28/2006] 选看的目标可能是下级终端
    tDstMt = GetLocalMtFromOtherMcuMt(tDstMt);

	if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
	{
		ConfLog( FALSE, "[StopSelectSrc] Dst Mt%u-%u not joined conference!\n", 
			             tDstMt.GetMcuId(), tDstMt.GetMtId() );
		return;
	}
    // xsl [7/21/2006] 恢复终端接收地址
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
        {
            ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_AUDIO);
        }

        if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
        {
            ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_VIDEO);
        }                
    }           
	
	//清除选看状态, 这里肯定是清除所有的选看状态，不是清除部分
	m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
	tMtStatus.RemoveSelByMcsMode( byMode );

	// zgc, 2008-01-22, 重新调整选看源的发送码率,记录选看源
	tSrc = tMtStatus.GetSelectMt( MODE_VIDEO );

	// 清空上次选看终端		
	tNullMt.SetNull();
    // 顾振华@2006.4.17 增加选看类型
	tMtStatus.SetSelectMt(tNullMt, byMode);

	m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tMtStatus );

	// xliang [1/9/2009] 区分起初选看的是MT还是EQP(目前是VMP)
	if(tSrc.GetType() == TYPE_MT)
	{
		// xliang [12/12/2008] 选看状态重置后，判断是否是在MPU中的取消选看
		//是，则降该MT的分辨率
		TPeriEqpStatus tPeriEqpStatus; 
		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
		u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
		
		if ( (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE 	//会议在vmp中
			&& m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tSrc))	//选看源MT在vmp合成成员中
			&& ( byVmpSubType != VMP)	//用的VMP是有前适配通道的VMP
			) 
		{	
			//降分辨率// xliang [4/21/2009] 
			TVMPParam  tVMPParam   = m_tConf.m_tStatus.GetVmpParam();
			u8  byVmpStyle = tVMPParam.GetVMPStyle();  
			u8	byChlPos = tVMPParam.GetChlOfMtInMember(tSrc);
			
			ChangeMtVideoFormat(tSrc, &tVMPParam, TRUE, FALSE);
		}
		// 重新调整选看源的发送码率，zgc, 2008-01-22
		m_ptMtTable->GetMtStatus(tSrc.GetMtId(), &tSrcStatus);
		if ( m_tConfAllMtInfo.MtJoinedConf( tSrc.GetMtId() ) 
			&& tSrcStatus.IsSendVideo() 
			&& (MODE_VIDEO == byMode || MODE_BOTH == byMode)
			)
		{
			NotifyMtSend( tSrc.GetMtId(), MODE_VIDEO );
		}
	}
	
	//若有发言人，重新选看发言人
	if ( byIsRestore )
	{
#ifdef _SATELITE_
		g_cMpManager.StopSwitchSrc2Dst(tSrc, tDstMt, byMode);
		RefreshConfState();
		

		//FIXME: 临时保护在这里
		if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
		{
			if (!m_tAudBrdSrc.IsNull())
			{
				m_ptMtTable->SetMtSrc(tDstMt.GetMtId(), &m_tAudBrdSrc, MODE_AUDIO);
			}
		}
		if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
		{
			if (!m_tVidBrdSrc.IsNull())
			{
				m_ptMtTable->SetMtSrc(tDstMt.GetMtId(), &m_tVidBrdSrc, MODE_VIDEO);
			}
		}
#else
		RestoreRcvMediaBrdSrc( tDstMt.GetMtId(), byMode );
#endif
	}
    // zgc, 2008-05-28, 不需要重新接受广播源则直接停交换
    else
    {

#ifdef _SATELITE_
		g_cMpManager.StopSwitchSrc2Dst(tSrc, tDstMt, byMode);
		RefreshConfState();
		
#else
		StopSwitchToSubMt( tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT );
#endif
    }
    
    //暂时不考虑逻辑覆盖，直接再清一次
//     BOOL32 bAccord2Adp = FALSE;
//     TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
//     TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS(tDstMt.GetMtId());
// 
//     u8 bySelMode = (MODE_VIDEO == byMode || MODE_BOTH == byMode) ? MODE_VIDEO : MODE_NONE;
//     if (MODE_VIDEO == bySelMode &&
//         IsSelModeAndCapMatched(bySelMode, tSrcSCS, tDstSCS, bAccord2Adp))
//     {
//         if (!g_cMcuVcApp.IsSelAccord2Adp() || !bAccord2Adp)
//         {
//             return;
//         }
// 
//         StopHdVidSelAdp(tSrc, tDstMt, MODE_VIDEO);
//     }
    MtStatusChange( tDstMt.GetMtId(), TRUE );

    return;
}

/*====================================================================
    函数名      ：ChangeChairman
    功能        ：改变主席（包括单纯撤销）；
	              通知MC，MT和下级MCU；
				  如果新、旧主席直连，通知其自身改变状态
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt * ptNewChairman, 新主席，NULL表示单纯撤销
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/04    1.0         LI Yi         创建
	03/11/25    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ChangeChairman( TMt * ptNewChairman )
{
	CServMsg	cServMsg;
	TMt		tOldChairman = m_tConf.GetChairman();
	cServMsg.SetConfId( m_tConf.GetConfId() );
	//TMt    tSpeakerSrc;
	BOOL32 bHasOldChairman = FALSE;

    TPeriEqpStatus tTWStatus;
    TPeriEqpStatus tHduStatus;
	TPeriEqpStatus tVmpStatus;

    u8 byChnlIdx;
    u8 byEqpId;

	if(ptNewChairman != NULL)
    {
	    *ptNewChairman = GetLocalMtFromOtherMcuMt(* ptNewChairman);
    }
			    
	//取消原主席
	if( HasJoinedChairman() )
	{	
		bHasOldChairman = TRUE;

		cServMsg.SetMsgBody( ( u8 * )&tOldChairman, sizeof( tOldChairman ) );
		cServMsg.SetNoSrc();
		SendMsgToMt( tOldChairman.GetMtId(), MCU_MT_CANCELCHAIRMAN_NOTIF,cServMsg );
		m_tConf.SetNoChairman();
		ConfLog( FALSE, "Old chairman MT%u was cancelled!\n", tOldChairman.GetMtId() );

		// xliang [4/2/2009] 主席选看VMP此时失效
		if(m_tConf.m_tStatus.IsVmpSeeByChairman())
		{
			m_tConf.m_tStatus.SetVmpSeebyChairman(FALSE);
			// 状态同步刷新到TPeriStatus中
			g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
			tVmpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
			g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
			//清交换及选看源状态
			StopSelectSrc(tOldChairman,MODE_VIDEO);
		}
		/* zgc, 2008-05-05, 这里的代码不需要，在最后调整主席交换的函数里统一处理
		//如果发言人的源是主席
		if( HasJoinedSpeaker() )
		{
			// 视频
			m_ptMtTable->GetMtSrc( GetLocalSpeaker().GetMtId(), &tSpeakerSrc, MODE_VIDEO );
			if( tSpeakerSrc == tOldChairman && 
				m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_CHAIR && 
				ptNewChairman == NULL &&
				// 如果非会议内部逻辑选看，则不要停止, zgc, 2008-04-14
				SPEAKER_SRC_CONFSEL == GetSpeakerSrcSpecType( MODE_VIDEO ) )
			{
                TMt tLocalSpeaker = GetLocalSpeaker();
				if( m_ptMtTable->GetManuId( tLocalSpeaker.GetMtId() ) == MT_MANU_KDC )
				{
					//StopSwitchToSubMt( tLocalSpeaker.GetMtId(), MODE_BOTH, SWITCH_MODE_SELECT, TRUE );
					StopSwitchToSubMt( tLocalSpeaker.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT, TRUE );

                    // 如果当前有混音，把N-1模式的混音交换恢复建立给发言人 [12/20/2006-zbq]
					// 音频单独处理，zgc, 2008-04-14					
                    //if ( m_tConf.m_tStatus.IsMixing() )
                    //{
                    //   SwitchMixMember( &tLocalSpeaker, FALSE );
                    //}
					
				}
				else
				{
                    //非kdc厂商回传自身视频, 节省带宽时，通知mt发送码流
                    if(g_cMcuVcApp.IsSavingBandwidth())
                    {
                        NotifyOtherMtSend(tLocalSpeaker.GetMtId(), TRUE);
                    }                    
					
					// 音频单独处理，zgc, 2008-04-25
					//u8 byMode = m_tConf.m_tStatus.IsMixing() ? MODE_VIDEO : MODE_BOTH;
					//StartSwitchToSubMt( tLocalSpeaker, 0, tLocalSpeaker.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE );
					StartSwitchToSubMt( tLocalSpeaker, 0, tLocalSpeaker.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT, TRUE );
				}
			}

			//音频单独处理
			m_ptMtTable->GetMtSrc( GetLocalSpeaker().GetMtId(), &tSpeakerSrc, MODE_AUDIO );
			if( tSpeakerSrc == tOldChairman && 
				m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_CHAIR && 
				ptNewChairman == NULL &&
				m_tConf.GetConfAttrb().GetSpeakerSrcMode() == MODE_BOTH &&
				// 如果非会议自动设定，则不要停止, zgc, 2008-04-14
				SPEAKER_SRC_CONFSEL == GetSpeakerSrcSpecType( MODE_AUDIO ) )
			{
                TMt tLocalSpeaker = GetLocalSpeaker();
				if( m_ptMtTable->GetManuId( tLocalSpeaker.GetMtId() ) == MT_MANU_KDC )
				{
					StopSwitchToSubMt( tLocalSpeaker.GetMtId(), MODE_AUDIO, SWITCH_MODE_SELECT, TRUE );
					
                    // 如果当前有混音，把N-1模式的混音交换恢复建立给发言人 [12/20/2006-zbq]
                    if ( m_tConf.m_tStatus.IsMixing() )
                    {
                        SwitchMixMember( &tLocalSpeaker, FALSE );
                    }
				}
				else
				{
                    //非kdc厂商回传自身视频, 节省带宽时，通知mt发送码流
                    //if(g_cMcuVcApp.IsSavingBandwidth())
                    //{
                    //    NotifyOtherMtSend(tLocalSpeaker.GetMtId(), TRUE);
                    //}
					//u8 byMode = m_tConf.m_tStatus.IsMixing() ? MODE_VIDEO : MODE_BOTH;
					//StartSwitchToSubMt( tLocalSpeaker, 0, tLocalSpeaker.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE );
					
					if ( !m_tConf.m_tStatus.IsMixing() )
					{
						StartSwitchToSubMt( tLocalSpeaker, 0, tLocalSpeaker.GetMtId(), MODE_AUDIO, SWITCH_MODE_SELECT, TRUE );
					}
				}
			}
		}*/

        //强制广播下，恢复其视频源[01/24/2007-zbq]
        if ( m_tConf.m_tStatus.IsMustSeeSpeaker() )
        {
            //指定恢复接收音频广播媒体源
            RestoreRcvMediaBrdSrc( tOldChairman.GetMtId(), MODE_BOTH );
        }        

        for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
        {
            if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
            {
                for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
                {
                    if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus) &&
                        tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType == TW_MEMBERTYPE_CHAIRMAN &&
                        tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx() == m_byConfIdx)
                    {
                        ChangeTvWallSwitch(&tOldChairman, byEqpId, byChnlIdx, TW_MEMBERTYPE_CHAIRMAN, TW_STATE_STOP, FALSE);
                    }
                }
            }
        }

		for (byEqpId = HDUID_MIN; byEqpId <= HDUID_MAX; byEqpId++)
        {
            if (EQP_TYPE_HDU == g_cMcuVcApp.GetEqpType(byEqpId))
            {
                for (byChnlIdx = 0; byChnlIdx < MAXNUM_HDU_CHANNEL; byChnlIdx++)
                {
                    if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tHduStatus) &&
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].byMemberType == TW_MEMBERTYPE_CHAIRMAN &&
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byChnlIdx].GetConfIdx() == m_byConfIdx &&
						NULL == ptNewChairman)
                    {

						ChangeHduSwitch(&tOldChairman, byEqpId, byChnlIdx, TW_MEMBERTYPE_CHAIRMAN, TW_STATE_STOP);
                    }
                }
            }
        }


		//适应远摇中兴MCU的特殊情况，向所有的中兴MCU发一次取消主席请求
        if (g_cMcuVcApp.IsApplyChairToZxMcu())
		{
			for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId  ++ )
			{
				if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
					MT_MANU_ZTEMCU == m_ptMtTable->GetManuId(byMtId) )
				{            
					CServMsg cSerMsg;
					SendMsgToMt( byMtId, MCU_MT_CANCELCHAIRMAN_CMD, cSerMsg );                
				}
			}
		}
	}

	//设置新主席
	if( ptNewChairman != NULL )
	{	
		cServMsg.SetMsgBody( ( u8 * )ptNewChairman, sizeof( TMt ) );
		cServMsg.SetNoSrc();
		SendMsgToMt( ptNewChairman->GetMtId(), MCU_MT_SPECCHAIRMAN_NOTIF,cServMsg );
		m_tConf.SetChairman( *ptNewChairman );

		ConfLog( FALSE, "New chairman MT%u is specified!\n", ptNewChairman->GetMtId() );

        //适应远摇中兴MCU的特殊情况，向所有的中兴MCU发一次申请主席请求 [zbq 06-04-20]
        if (g_cMcuVcApp.IsApplyChairToZxMcu())
		{
			for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId  ++ )
			{
				if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
					MT_MANU_ZTEMCU == m_ptMtTable->GetManuId(byMtId) )
				{            
					CServMsg cSerMsg;
					SendMsgToMt( byMtId, MCU_MT_APPLYCHAIRMAN_CMD, cSerMsg );                
				}
			}
		}
	}

    //通知所有会控
    TBasicConfInfo tBasicConfInfo;
    tBasicConfInfo.SetSpeaker(m_tConf.GetSpeaker());
    tBasicConfInfo.SetChairman(m_tConf.GetChairman());
	cServMsg.SetMsgBody( ( u8 * )&tBasicConfInfo, sizeof( tBasicConfInfo ) );
	SendMsgToAllMcs( MCU_MCS_SIMCONFINFO_NOTIF, cServMsg );
	
	//通知所有终端
	//简化通知终端使用的会议信息结构 2005-10-11
	TConfStatus tConfStatus = m_tConf.GetStatus();
	TSimConfInfo tSimConfInfo;
	tSimConfInfo.m_tSpeaker  = GetLocalSpeaker();
	tSimConfInfo.m_tChairMan = m_tConf.GetChairman();
	tSimConfInfo.SetVACMode(tConfStatus.IsVACing());
	tSimConfInfo.SetVMPMode(tConfStatus.GetVMPMode());
    // guzh [11/6/2007] 
	tSimConfInfo.SetDiscussMode(tConfStatus.IsMixing());
	cServMsg.SetMsgBody( ( u8 * )&tSimConfInfo, sizeof( tSimConfInfo ) );
	BroadcastToAllSubMtJoinedConf( MCU_MT_SIMPLECONF_NOTIF, cServMsg );
    if (NULL != ptNewChairman)
    {
        TMt tSpeaker = m_tConf.GetSpeaker();
        m_tConf.SetSpeaker(GetLocalSpeaker());
        cServMsg.SetMsgBody((u8 *)&m_tConf, sizeof(TConfInfo));
        SendMsgToMt(ptNewChairman->GetMtId(), MCU_MT_CONF_NOTIF, cServMsg);
        m_tConf.SetSpeaker(tSpeaker);
    }

    //zbq[09/24/2007] 主席终端的后视频向通道打开，调整相关交换
    TLogicalChannel tLogicChan;
    //zbq [10/11/2007] 本处理取消主席 共用
	//zgc [04/21/2008] 音频视频分开处理
	/*
    if ( NULL == ptNewChairman ||
         ( NULL != ptNewChairman &&
           m_ptMtTable->GetMtLogicChnnl(ptNewChairman->GetMtId(), LOGCHL_VIDEO, &tLogicChan, FALSE)))
    {
		TMt *ptOldChairman = NULL;
		if ( bHasOldChairman )
		{
			ptOldChairman = &tOldChairman;
		}
        AdjustChairmanSwitch( ptOldChairman );
    }*/
	// 音频通道
	if ( NULL == ptNewChairman ||
		( NULL != ptNewChairman &&
           m_ptMtTable->GetMtLogicChnnl(ptNewChairman->GetMtId(), LOGCHL_AUDIO, &tLogicChan, FALSE)))
	{
		AdjustChairmanAudSwitch();
	}
	// 视频通道
	if ( NULL == ptNewChairman ||
		( NULL != ptNewChairman &&
		m_ptMtTable->GetMtLogicChnnl(ptNewChairman->GetMtId(), LOGCHL_VIDEO, &tLogicChan, FALSE)))
	{
		TMt *ptOldChairman = NULL;
		if ( bHasOldChairman )
		{
			ptOldChairman = &tOldChairman;
		}
		AdjustChairmanVidSwitch( ptOldChairman );
	}
	

    //n+1备份更新主席信息
    if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
    {
        ProcNPlusChairmanUpdate();
    }

#ifdef _SATELITE_
    RefreshConfState();
	
#endif

    return;
}


/*====================================================================
    函数名      ：AdjustChairmasSwitch
    功能        ：调整当前主席的相关交换：四个跟随建立
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/09/24    4.0         张宝卿        创建
====================================================================*/
void CMcuVcInst::AdjustChairmanSwitch( TMt *ptOldChair )
{
    //zbq [10/11/2007] 本处理取消主席 共用
    /*if ( !HasJoinedChairman() )
    {
        ConfLog( FALSE, "[AdjustChairmanSwitch] No chairman, ignore it\n" );
        return;
    }*/
    
    TMt tChairMan = m_tConf.GetChairman();

	//发言人看主席时；发言人源的调整, zgc, 2008-02-25
	u8 bySelMode = m_tConf.GetConfAttrb().GetSpeakerSrcMode();
	ChangeSpeakerSrc( bySelMode, emReasonChangeChairman );

	// zgc, 2007-12-24,
	// 以下VMP及VMPTW加入跟随通道的处理 从 加入电视墙跟随通道之上移到 
	// 后向逻辑通道是否打开判断之上，因为 ChangeVmpChannelParam 和 ChangeVmpTwChannelParam
	// 是双向处理函数，不需要判断后向逻辑通道是否打开
	//加入画面合成跟随通道
    if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE)
    {
        // xsl [10/27/2006]卫星分散会议时需要判断回传通道数
        if (m_tConf.GetConfAttrb().IsSatDCastMode() &&
            m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_CHAIRMAN))
        {
            if (IsOverSatCastChnnlNum(tChairMan.GetMtId()))
            {
                ConfLog(FALSE, "[ChangeChairman] over max upload mt num. nack!\n");            
                NotifyMcsAlarmInfo(0, ERR_MCU_DCAST_OVERCHNNLNUM);
                return;
            }
        }    
        
        if (IsChangeVmpParam(&tChairMan) &&
            // zbq [05/31/2007] 若所有VMP通道都不是主席跟随，不需要调整通道风格
            ( m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_CHAIRMAN) ||
			 m_tConf.m_tStatus.GetVmpParam().IsMtInMember( tChairMan ) || 
			 ( ptOldChair != NULL && m_tConf.m_tStatus.GetVmpParam().IsMtInMember( *ptOldChair ) ) )
		)
        {
            ChangeVmpChannelParam(&tChairMan, VMP_MEMBERTYPE_CHAIRMAN, ptOldChair);
        }
        else
        {
            SetTimer(MCUVC_CHANGE_VMPCHAN_CHAIRMAN_TIMER, TIMESPACE_CHANGEVMPPARAM, VMP_MEMBERTYPE_CHAIRMAN);
        }
    }
    
    //加入多画面电视墙跟随通道
    if (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE)
    {
        // xsl [10/27/2006]卫星分散会议时需要判断回传通道数
        if (m_tConf.GetConfAttrb().IsSatDCastMode() &&
            m_tConf.m_tStatus.GetVmpTwParam().IsTypeInMember(VMPTW_MEMBERTYPE_CHAIRMAN))
        {
            if (IsOverSatCastChnnlNum(tChairMan.GetMtId()))
            {
                ConfLog(FALSE, "[ChangeChairman] over max upload mt num. nack!\n");            
                NotifyMcsAlarmInfo(0, ERR_MCU_DCAST_OVERCHNNLNUM);
                return;
            }
        }    
        
        ChangeVmpTwChannelParam(&tChairMan, VMPTW_MEMBERTYPE_CHAIRMAN);
    }

    TLogicalChannel tLogicChan;
    if ( !m_ptMtTable->GetMtLogicChnnl(tChairMan.GetMtId(), LOGCHL_VIDEO, &tLogicChan, FALSE))
    {
        ConfLog( FALSE, "[AdjustChairmanSwitch] ChairMt.%d's RL chan unexist, ignore it\n", tChairMan.GetMtId() );
        return;
    }
        
	/*
    if( HasJoinedSpeaker() && 
        HasJoinedChairman() && 
        m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_CHAIR )
    {
        if( //!m_tConf.m_tStatus.IsBrdstVMP() &&
            //!( m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO ) && 
            ( m_tConf.GetConfAttrb().GetSpeakerSrcMode() == MODE_VIDEO || 
            m_tConf.GetConfAttrb().GetSpeakerSrcMode() == MODE_BOTH ) )
        {
            u8 bySelMode = m_tConf.GetConfAttrb().GetSpeakerSrcMode();
            ChangeSelectSrc(tChairMan, GetLocalSpeaker(), bySelMode);
        }
    }
	*/

    /*
    //如果有发言人，则把发言人当前视频源号发给主席(若终端要知道当前视频源号，需要增加非标消息通知)        
    if (m_tConf.HasSpeaker())
    {
        CServMsg cServMsgTmp;
        TMt tMtSpeaker = m_tConf.GetSpeaker();
        u8 byCurrVidSrcNo = m_ptMtTable->GetCurrVidSrcNo(tMtSpeaker.GetMtId());

        cServMsgTmp.SetSrcMtId(tMtSpeaker.GetMtId());
        cServMsgTmp.SetMsgBody((u8 *)ptNewChairman, sizeof(TMt));
        cServMsgTmp.CatMsgBody((u8 *)&byCurrVidSrcNo, sizeof(u8));
        MtVideoSourceSwitched(cServMsgTmp);
    }
    */
  
    //加入电视墙跟随通道
	TPeriEqpStatus tTWStatus;
    u8 byChnlIdx;
    u8 byEqpId;
    for (byEqpId = 1; byEqpId <= MAXNUM_MCU_PERIEQP; byEqpId++)
    {
        if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
        {
            for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
            {
                if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus) &&
                    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType == TW_MEMBERTYPE_CHAIRMAN &&
                    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx() == m_byConfIdx)
                {
                    // xsl [10/27/2006]卫星分散会议时需要判断回传通道数
                    if (m_tConf.GetConfAttrb().IsSatDCastMode())
                    {
                        if (IsOverSatCastChnnlNum(tChairMan.GetMtId()))
                        {
                            ConfLog(FALSE, "[ChangeChairman] over max upload mt num. nack!\n");            
                            NotifyMcsAlarmInfo(0, ERR_MCU_DCAST_OVERCHNNLNUM);
                            return;
                        }
                    }    
                    
                    ChangeTvWallSwitch(&tChairMan, byEqpId, byChnlIdx, TW_MEMBERTYPE_CHAIRMAN, TW_STATE_CHANGE);
                }
            }
        }
    }
    
    return;
}

/*=============================================================================
函 数 名： AdjustChairmanVidSwitch
功    能： 调整主席视频交换
算法实现： 
全局变量： 
参    数： TMt *ptOldChair = NULL
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/21   4.0			周广程                  创建
=============================================================================*/
void CMcuVcInst::AdjustChairmanVidSwitch( TMt *ptOldChair )
{
	//zbq [10/11/2007] 本处理取消主席 共用
    /*if ( !HasJoinedChairman() )
    {
        ConfLog( FALSE, "[AdjustChairmanSwitch] No chairman, ignore it\n" );
        return;
    }*/
    
    TMt tChairMan = m_tConf.GetChairman();

	//发言人看主席时；发言人源的调整, zgc, 2008-02-25
	u8 bySelMode = m_tConf.GetConfAttrb().GetSpeakerSrcMode();
	// 增加对发言人源的码流模式的判断，防止误调整，zgc, 2008-04-21
	if ( bySelMode == MODE_VIDEO || bySelMode == MODE_BOTH )
	{
		ChangeSpeakerSrc( MODE_VIDEO, emReasonChangeChairman );
	}

	// zgc, 2007-12-24,
	// 以下VMP及VMPTW加入跟随通道的处理 从 加入电视墙跟随通道之上移到 
	// 后向逻辑通道是否打开判断之上，因为 ChangeVmpChannelParam 和 ChangeVmpTwChannelParam
	// 是双向处理函数，不需要判断后向逻辑通道是否打开
	//加入画面合成跟随通道
    if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE)
    {
        // xsl [10/27/2006]卫星分散会议时需要判断回传通道数
        if (m_tConf.GetConfAttrb().IsSatDCastMode() &&
            m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_CHAIRMAN))
        {
            if (IsOverSatCastChnnlNum(tChairMan.GetMtId()))
            {
                ConfLog(FALSE, "[ChangeChairman] over max upload mt num. nack!\n");            
                NotifyMcsAlarmInfo(0, ERR_MCU_DCAST_OVERCHNNLNUM);
                return;
            }
        }    
        
        if (IsChangeVmpParam(&tChairMan) &&
            // zbq [05/31/2007] 若所有VMP通道都不是主席跟随，不需要调整通道风格
            ( m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_CHAIRMAN) ||
			 m_tConf.m_tStatus.GetVmpParam().IsMtInMember( tChairMan ) || 
			 ( ptOldChair != NULL && m_tConf.m_tStatus.GetVmpParam().IsMtInMember( *ptOldChair ) ) )
		)
        {
			ConfLog(FALSE, "[AdjustChairmanVidSwitch] begin to call FUNCTION[ChangeVmpChannelParam]!\n");
            ChangeVmpChannelParam(&tChairMan, VMP_MEMBERTYPE_CHAIRMAN, ptOldChair);
        }
        else
        {
            SetTimer(MCUVC_CHANGE_VMPCHAN_CHAIRMAN_TIMER, TIMESPACE_CHANGEVMPPARAM, VMP_MEMBERTYPE_CHAIRMAN);
        }
    }
    
    //加入多画面电视墙跟随通道
    if (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE)
    {
        // xsl [10/27/2006]卫星分散会议时需要判断回传通道数
        if (m_tConf.GetConfAttrb().IsSatDCastMode() &&
            m_tConf.m_tStatus.GetVmpTwParam().IsTypeInMember(VMPTW_MEMBERTYPE_CHAIRMAN))
        {
            if (IsOverSatCastChnnlNum(tChairMan.GetMtId()))
            {
                ConfLog(FALSE, "[ChangeChairman] over max upload mt num. nack!\n");            
                NotifyMcsAlarmInfo(0, ERR_MCU_DCAST_OVERCHNNLNUM);
                return;
            }
        }    
        
        ChangeVmpTwChannelParam(&tChairMan, VMPTW_MEMBERTYPE_CHAIRMAN);
    }


	if(tChairMan.IsNull())	
	{
		ConfLog( FALSE, "[AdjustChairmanVidSwitch] no new chairman, ignore further process!\n");
		return;
	}
    TLogicalChannel tLogicChan;
    if ( !m_ptMtTable->GetMtLogicChnnl(tChairMan.GetMtId(), LOGCHL_VIDEO, &tLogicChan, FALSE))
    {
        ConfLog( FALSE, "[AdjustChairmanVidSwitch] ChairMt.%d's video RL chan unexist, ignore it\n", tChairMan.GetMtId() );
        return;
    }
  
    //加入电视墙跟随通道
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
                    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType == TW_MEMBERTYPE_CHAIRMAN &&
                    tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx() == m_byConfIdx)
                {
                    // xsl [10/27/2006]卫星分散会议时需要判断回传通道数
                    if (m_tConf.GetConfAttrb().IsSatDCastMode())
                    {
                        if (IsOverSatCastChnnlNum(tChairMan.GetMtId()))
                        {
                            ConfLog(FALSE, "[ChangeChairman] over max upload mt num. nack!\n");            
                            NotifyMcsAlarmInfo(0, ERR_MCU_DCAST_OVERCHNNLNUM);
                            return;
                        }
                    }    
                    
                    ChangeTvWallSwitch(&tChairMan, byEqpId, byChnlIdx, TW_MEMBERTYPE_CHAIRMAN, TW_STATE_CHANGE);
                }
            }
        }
    }
    
    //加入HDU跟随通道
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
                    tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].byMemberType == TW_MEMBERTYPE_CHAIRMAN &&
                    tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetConfIdx() == m_byConfIdx)
                {
                    // xsl [10/27/2006]卫星分散会议时需要判断回传通道数
                    if (m_tConf.GetConfAttrb().IsSatDCastMode())
                    {
                        if (IsOverSatCastChnnlNum(tChairMan.GetMtId()))
                        {
                            ConfLog(FALSE, "[ChangeChairman] over max upload mt num. nack! by hdu\n");            
                            NotifyMcsAlarmInfo(0, ERR_MCU_DCAST_OVERCHNNLNUM);
                            return;
                        }
                    }    
                    
                    ChangeHduSwitch(&tChairMan, byHduEqpId, byHduChnlIdx, TW_MEMBERTYPE_CHAIRMAN, TW_STATE_CHANGE);
                }
            }
        }
    }
	
    return;
}

/*=============================================================================
函 数 名： AdjustChairmanAudSwitch
功    能： 调整主席音频交换
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/21   4.0		周广程                  创建
=============================================================================*/
void CMcuVcInst::AdjustChairmanAudSwitch( void )
{
	//发言人看主席时；发言人源的调整, zgc, 2008-02-25
	u8 bySelMode = m_tConf.GetConfAttrb().GetSpeakerSrcMode();

    // 增加对发言人源的码流模式的判断，防止误调整，zgc, 2008-04-21
	if ( bySelMode == MODE_BOTH )
	{
		ChangeSpeakerSrc( MODE_AUDIO, emReasonChangeChairman );
	}
}

/*====================================================================
    函数名      ：ChangeVidBrdSrc
    功能        ：改变视频广播源（包括单纯撤销）；
	              通知MC，MT
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt * ptNewVidBrdSrc, 新的视频广播源，NULL表示单纯撤销
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/25    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ChangeVidBrdSrc(TMt * ptNewVidBrdSrc)
{
    CServMsg cServMsg;
    u8	bySrcChnnl;
    u16 wAdaptBitRate = 0;  //modify bas 2
    TSimCapSet tSrcSimCapSet;
    TSimCapSet tDstSimCapSet;
    TLogicalChannel tLogicChnnl;
    TLogicalChannel tLogicalChannel;
    TLogicalChannel tH239LogicChnnl;
    TLogicalChannel tNullLogicChnnl;

    TMt tOldSrc;
    tOldSrc.SetNull();
    BOOL32 bStopOldVidSrcNow = FALSE;   // 是否立即切换广播源
	BOOL32 bNewVmpBrd = FALSE;			// 是否是新VMP广播

    if (ptNewVidBrdSrc != NULL)
    {
        *ptNewVidBrdSrc = GetLocalMtFromOtherMcuMt(*ptNewVidBrdSrc);
    }

	//　停止广播原有视频源
    if (m_tVidBrdSrc.IsNull())
    {
        bStopOldVidSrcNow = TRUE;
    }
    else
    {
		//启用适配并且开启了双流, 恢复老的第一路发送源的发送码率,没有适配器则根据FlowControl调整
        //flowctrl modify - 是否还有必要恢复老的第一路发送源的发送码率？被切掉的VidSrc要停止发码流的。
        // guzh [8/24/2007] 回答上面的问题，这里是有必要flowctrl到原来标准码率的。因为广播源可能还在回传、或者录像、VMP中，所以要先恢复
        if (!m_tDoubleStreamSrc.IsNull() &&
            !m_tVidBrdSrc.IsNull() && TYPE_MT == m_tVidBrdSrc.GetType())
        {
            TLogicalChannel tLogicalChannel;
            if (TRUE == m_ptMtTable->GetMtLogicChnnl(m_tVidBrdSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
            {
                tLogicalChannel.SetFlowControl(m_ptMtTable->GetSndBandWidth( m_tVidBrdSrc.GetMtId()));
                cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
                SendMsgToMt(m_tVidBrdSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
            }
        }
        
        if (m_tConf.m_tStatus.IsPrsing())
        {
            //停止第一路视频广播源的重传
            if( ptNewVidBrdSrc == NULL)
            {
                StopPrs(PRSCHANMODE_FIRST, FALSE);
            }   
            
            //vmp.2 双速单格式会议，停止vmp第二路码流的prs
            if (m_tVidBrdSrc == m_tVmpEqp)
            {
                if (0 != m_tConf.GetSecBitRate() && 
                    MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
                {
                    StopPrs(PRSCHANMODE_VMP2, TRUE);
                }
            }
        }        

		if (ptNewVidBrdSrc != NULL)
		{		
			//画面合成开始广播时停止媒体类型适配
            // 停止高清主视频适配, zgc, 2008-08-07
            if ( *ptNewVidBrdSrc == m_tVmpEqp && m_tConf.m_tStatus.IsHdVidAdapting() )
            {
                //StopHDVidAdapt();
            }

			if (*ptNewVidBrdSrc == m_tVmpEqp && m_tConf.m_tStatus.IsVidAdapting())
			{
                //modify bas 2
				StopBasAdapt(ADAPT_TYPE_VID);
			}

			//画面合成开始广播时停止码率适配
			if (*ptNewVidBrdSrc == m_tVmpEqp && m_tConf.m_tStatus.IsBrAdapting())
			{
				StopBasAdapt(ADAPT_TYPE_BR);
			}
		}

        // zgc, 2008-05-23, 和新广播交换逻辑冲突，这里不能拆桥，必须等到MP的移除广播源
        //    成功通知收到后才能拆桥
        /*
        // zbq [08/23/2007] 当前源在回传通道里，则不能拆桥
        if ( !m_tCascadeMMCU.IsNull() &&
             m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId())->m_tSpyMt == m_tVidBrdSrc )
        {
        }
        else
        {
            //拆除交换桥
            bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
            g_cMpManager.RemoveSwitchBridge(m_tVidBrdSrc, bySrcChnnl, MODE_VIDEO);            
        }
        
        //  xsl [3/10/2006] 双速或双格式会议广播源为vmp时，拆除第二路交换桥
        if ((0 != m_tConf.GetSecBitRate() || MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType())
            && m_tVidBrdSrc == m_tVmpEqp)
        {
            g_cMpManager.RemoveSwitchBridge(m_tVidBrdSrc, bySrcChnnl+1, MODE_VIDEO);           
        }
        */
        
        TLogicalChannel tLogicalChannel;

        //　只停那些收看广播源的终端
        TMt tMt = m_tVidBrdSrc;
        tOldSrc = tMt;
        m_tVidBrdSrc.SetNull();	
        if (NULL == ptNewVidBrdSrc)
        {
            RestoreAllSubMtJoinedConfWatchingSrcMt(tMt, MODE_VIDEO);

            //zbq[01/05/2009] 停止当前的适配交换
            if (IsHDConf(m_tConf) &&
                m_tConf.m_tStatus.IsHdVidAdapting())
            {
                StopAllHdBasSwitch();
            }

            //刷新状态和表
            m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_VID, FALSE);
            RefreshRcvGrp();

            // xliang [2/6/2009] 判断是否是新VMP广播停止
			if( tOldSrc == m_tVmpEqp )
			{
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				if(byVmpSubType != VMP)
				{
					bNewVmpBrd = TRUE;
				}
			}
			if(bNewVmpBrd)
			{
				// xliang [5/6/2009] 停PRS
				if (m_tConf.m_tStatus.IsPrsing())
				{
					StopPrs(PRSCHANMODE_VMPOUT1, TRUE);
					StopPrs(PRSCHANMODE_VMPOUT2, TRUE);
					StopPrs(PRSCHANMODE_VMPOUT3, TRUE);
					StopPrs(PRSCHANMODE_VMPOUT4, TRUE);
				}
				bySrcChnnl = 0;
				// 				g_cMpManager.StopSwitchToBrd(tOldSrc, bySrcChnnl);  // 不需停止广播源发送
				// xliang [2/10/2009] 立即拆桥
				g_cMpManager.RemoveSwitchBridge(tOldSrc, bySrcChnnl, MODE_VIDEO);
				g_cMpManager.RemoveSwitchBridge(tOldSrc, bySrcChnnl+1, MODE_VIDEO);
				g_cMpManager.RemoveSwitchBridge(tOldSrc, bySrcChnnl+2, MODE_VIDEO);
				g_cMpManager.RemoveSwitchBridge(tOldSrc, bySrcChnnl+3, MODE_VIDEO);
				CallLog("New Vmp Broadcast Stop: RemoveSwitchBridge!\n");
				
				// xliang [4/2/2009] 恢复主席选看VMP
// 				if (m_tConf.m_tStatus.IsVmpSeeByChairman())
// 				{
// 					TMt tDstMt = m_tConf.GetChairman();
// 					SwitchNewVmpToSingleMt(tDstMt);
// 				}

				// xliang [3/14/2009] tell Mt(s) which saw VMP before to watch themselves at this moment 
				// except for chairman's watching VMP 
				TMt tLoopMt;
				TMt tCurSrc;
				for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
                {
                    if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
                    {
						m_ptMtTable->GetMtSrc( byLoop, &tCurSrc, MODE_VIDEO ); 
						if( tCurSrc == tOldSrc )	//该终端看的是之前的广播源(VMP)
						{
							tLoopMt = m_ptMtTable->GetMt( byLoop );
							if( tLoopMt == m_tConf.GetChairman() 
								&& m_tConf.m_tStatus.IsVmpSeeByChairman() )
							{
								//SwitchNewVmpToSingleMt(tLoopMt);	//之前不拆，所以这里不用额外补
							}
							else
							{
								NotifyMtReceive( tLoopMt, byLoop );	//watch self
							}
						}
						
                    }
                }
				
			}
			else
			{
				bySrcChnnl = (tOldSrc == m_tPlayEqp ? m_byPlayChnnl : 0);
				// 停止广播源发送
				g_cMpManager.StopSwitchToBrd(tOldSrc, bySrcChnnl);
			}

            // 如果是卫星分散组播，则停止
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
				//FIXME: 卫星分散和卫星独立
#ifdef _SATELITE_
				g_cMpManager.StopSatConfCast(tMt, CAST_FST, MODE_VIDEO, bySrcChnnl);
				g_cMpManager.StopSatConfCast(tMt, CAST_SEC, MODE_VIDEO, bySrcChnnl);
				
				for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
				{
					if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
						byMtId != m_tVidBrdSrc.GetMtId())
					{
						TMt tMtNull;
						tMtNull.SetNull();
						m_ptMtTable->SetMtSrc(byMtId, &tMtNull, MODE_VIDEO);
					}
				}
#else
				g_cMpManager.StopDistrConfCast(tMt, MODE_VIDEO, bySrcChnnl);
#endif
                TMt tLoopMt;
                // // 顾振华 [4/25/2006] 通知终端看自己
                for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
                {
                    if( m_tConfAllMtInfo.MtJoinedConf( byLoop ))
                    {
                        tLoopMt = m_ptMtTable->GetMt( byLoop );
                        NotifyMtReceive( tLoopMt, byLoop );
                    }
                }
            }            
        }
	
		//画面合成停止广播时重启媒体类型适配
        if (TYPE_MCUPERI == tMt.GetType() && tMt == m_tVmpEqp &&
            m_tConf.GetConfAttrb().IsUseAdapter())
        {           
            // 区分高清适配和普通适配，zgc, 2008-08-07
            if ( IsHDConf(m_tConf) )
            {
                TMt tSrcTemp;
                tSrcTemp.SetNull();
                if (!m_tConf.m_tStatus.IsHdVidAdapting() && 
                    m_cMtRcvGrp.IsNeedAdp())
                {
                    StartHDMVBrdAdapt();
                }
            }
            else 
            {
                if (!m_tConf.m_tStatus.IsVidAdapting() &&
                    IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
                {
                    StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
                else if( !m_tConf.m_tStatus.IsVidAdapting() && IsNeedCifAdp() )
                {
                    IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate);
                    StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
                
                if (!m_tConf.m_tStatus.IsBrAdapting() &&
                    IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
                {
                    StartAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
            }           
        }

        //停止向录像机交换
        if (!m_tConf.m_tStatus.IsNoRecording())
        {
            StopSwitchToPeriEqp(m_tRecEqp.GetEqpId(), m_byRecChnnl, FALSE, MODE_VIDEO);
        }

        //停止向码率适配器交换 - modify bas 2
        //if (m_tConf.m_tStatus.IsVidAdapting())
        if (EQP_CHANNO_INVALID != m_byVidBasChnnl)
        {
            //zbq[08/07/2007] 切换源而非删除源，不停BAS的交换；其新建交换可在MP业务或DS业务过滤掉
            if ( NULL == ptNewVidBrdSrc )
            {
                StopSwitchToPeriEqp(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, FALSE, MODE_VIDEO);
            }
        }

        //if (m_tConf.m_tStatus.IsBrAdapting())
        if (EQP_CHANNO_INVALID != m_byBrBasChnnl)
        {
            //zbq[08/07/2007] 切换源而非删除源，不停BAS的交换；其新建交换可在MP业务或DS业务过滤掉
            if ( NULL == ptNewVidBrdSrc )
            {
                StopSwitchToPeriEqp(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, FALSE, MODE_VIDEO);
            }
        }

#ifdef _SATELITE_
		RefreshConfState();
		
#endif
	}

    //　开始广播新视频源
    if (NULL != ptNewVidBrdSrc)
    {
        m_tVidBrdSrc = *ptNewVidBrdSrc;

        RefreshRcvGrp();
		
		// xliang [2/5/2009] 新VMP广播码流走新逻辑
		if( m_tVidBrdSrc == m_tVmpEqp )
		{
			TPeriEqpStatus tPeriEqpStatus; 
			g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
			u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
			if(byVmpSubType != VMP)
			{
				bNewVmpBrd = TRUE;
			}
		}
		if(bNewVmpBrd)	
		{
			//清选看，目前假设所有与会终端都接收到VMP广播(即MT各个接收群组均收到VMP某一路的码流)
			TMtStatus tMtStatus;
			u8 bySelMode = MODE_NONE;
			TMt tSelMt;
			u8 byLoop;
			for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop ++)
			{
				if(m_tConfAllMtInfo.MtJoinedConf( byLoop ))
				{
					tSelMt = m_ptMtTable->GetMt(byLoop);
					m_ptMtTable->GetMtStatus(byLoop, &tMtStatus);

					if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
						&& ( m_tRollCaller == tSelMt || m_tRollCallee == tSelMt )
						)
					{
						TMt tSelectedMt = tMtStatus.GetSelectMt( MODE_VIDEO ); //获取选看源
						if(!tSelectedMt.IsNull())	//判断选看源真实存在，可能之前选看失败
						{
							continue;// xliang [4/24/2009] 点名人被点名人互相选看，VMP不交换码流给他们
						}
					}

					bySelMode = tMtStatus.GetSelByMcsDragMode();
					if ( MODE_VIDEO == bySelMode || MODE_BOTH == bySelMode )
					{
						StopSelectSrc( tSelMt, MODE_VIDEO, FALSE );
					}
				}
			}
			
			//PRS
			if (m_tConf.GetConfAttrb().IsResendLosePack())
			{
				//根据会议能力确定具体有几路PRS，在ChangePrsSrc里会进行过滤。这里统一设值
				ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_VMPOUT1);
				ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_VMPOUT2);
				ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_VMPOUT3);
				ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_VMPOUT4);
			}

			//建立交换桥(4个通道)
			bySrcChnnl = 0;
			g_cMpManager.SetSwitchBridge(*ptNewVidBrdSrc, bySrcChnnl  , MODE_VIDEO); 
			g_cMpManager.SetSwitchBridge(*ptNewVidBrdSrc, bySrcChnnl+1, MODE_VIDEO); 
			g_cMpManager.SetSwitchBridge(*ptNewVidBrdSrc, bySrcChnnl+2, MODE_VIDEO); 
			g_cMpManager.SetSwitchBridge(*ptNewVidBrdSrc, bySrcChnnl+3, MODE_VIDEO);
			//群组交换
			
			//接收群组分类(不包含h263,mpeg4)
			u8 abyRcvMtFormat[4] = { VIDEO_FORMAT_HD1080,
									VIDEO_FORMAT_HD720,
									VIDEO_FORMAT_4CIF,
									VIDEO_FORMAT_CIF
									};
			u8  byRcvIdx = 0;
			u8  byRes = 0;
			u8	byMVType;
			u8  byNum = 0;
			u8	abyMtId[MAXNUM_CONF_MT] = { 0 };
			for(byRcvIdx; byRcvIdx < 4; byRcvIdx ++ )
			{

				byMVType = MEDIA_TYPE_H264; //目前只是H264
				byRes = abyRcvMtFormat[byRcvIdx];
				byNum = 0;	//每次都先置0
				m_cMtRcvGrp.GetMVMtList(byMVType, byRes, byNum, abyMtId, TRUE);//强制取接收MT列表，忽略免适配区
				if(byNum > 0)
				{
					for(u8 byMtIdx = 0; byMtIdx < byNum; byMtIdx ++)
					{
						
						if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
							&& (m_tRollCaller.GetMtId() == abyMtId[byMtIdx] 
							|| m_tRollCallee.GetMtId() == abyMtId[byMtIdx])
							)
						{
							m_ptMtTable->GetMtStatus(abyMtId[byMtIdx], &tMtStatus);
							TMt tSelectedMt = tMtStatus.GetSelectMt( MODE_VIDEO ); 
							if(!tSelectedMt.IsNull())	//判断选看源真实存在，可能之前选看失败
							{
								continue;// xliang [4/24/2009] 点名人被点名人互相选看，VMP不交换码流给他们
							}
						}
						bySrcChnnl = GetVmpOutChnnlByRes(byRes, m_tVmpEqp.GetEqpId(), byMVType);
						ConfLog(FALSE, "VMP(srcChnnl:%u)->MT.%u\n", bySrcChnnl, abyMtId[byMtIdx]);
						if(bySrcChnnl < MAXNUM_MPU_OUTCHNNL)	
						{

#ifndef _SATELITE_
							StartSwitchToSubMt(*ptNewVidBrdSrc, bySrcChnnl, abyMtId[byMtIdx], MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode 非默认值，是SWITCH_MODE_SELECT
#else
							//同主视频的给出组播，其他不予理睬
							if (byRes == m_tConf.GetMainVideoFormat())
							{
								g_cMpManager.StartSatConfCast(*ptNewVidBrdSrc, CAST_FST, MODE_VIDEO, bySrcChnnl);						
							}
							m_ptMtTable->SetMtSrc(abyMtId[byMtIdx], ptNewVidBrdSrc, MODE_VIDEO);
#endif
							CallLog("Send New Vmp Chnnl.%u(%s) to Mt.%u\n",bySrcChnnl,GetResStr(byRes),abyMtId[byMtIdx]);
						}
					}
				}
			}
			
			//mp4 
			byMVType = MEDIA_TYPE_MP4;
			byRes	= VIDEO_FORMAT_CIF; //目前对于mpeg4，h263取接收群组，Res不做参考
			byNum = 0;	//每次都先置0
			m_cMtRcvGrp.GetMVMtList(byMVType, byRes, byNum, abyMtId, TRUE);//强制取接收MT列表，忽略免适配区
			
			if(byNum > 0)
			{
				for(u8 byMtIdx = 0; byMtIdx < byNum; byMtIdx ++)
				{
					
					if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
						&& (m_tRollCaller.GetMtId() == abyMtId[byMtIdx] 
						|| m_tRollCallee.GetMtId() == abyMtId[byMtIdx])
						)
					{
						m_ptMtTable->GetMtStatus(abyMtId[byMtIdx], &tMtStatus);
						TMt tSelectedMt = tMtStatus.GetSelectMt( MODE_VIDEO ); 
						if(!tSelectedMt.IsNull())	//判断选看源真实存在，可能之前选看失败
						{
							continue;// xliang [4/24/2009] 点名人被点名人互相选看，VMP不交换码流给他们
						}
					}
					bySrcChnnl = GetVmpOutChnnlByRes(byRes, m_tVmpEqp.GetEqpId(), byMVType);
					ConfLog(FALSE, "VMP->MT: bySrcChnnl is %u\n", bySrcChnnl);
					if(bySrcChnnl < MAXNUM_MPU_OUTCHNNL)	
					{
						StartSwitchToSubMt(*ptNewVidBrdSrc, bySrcChnnl, abyMtId[byMtIdx], MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode 非默认值，是SWITCH_MODE_SELECT
// #ifdef _SATELITE_
// 						g_cMpManager.StartSatConfCast(*ptNewVidBrdSrc, FALSE, MODE_VIDEO, bySrcChnnl);
// #endif
						CallLog("Send New Vmp Chnnl.%u(%s) to Mt.%u\n",bySrcChnnl,GetResStr(byRes),abyMtId[byMtIdx]);
					}
				}
			}

			//h263
			byMVType = MEDIA_TYPE_H263;
			byRes	= VIDEO_FORMAT_CIF;
			byNum = 0;	//每次都先置0
			m_cMtRcvGrp.GetMVMtList(byMVType, byRes, byNum, abyMtId, TRUE);//强制取接收MT列表，忽略免适配区
			
			if(byNum > 0)
			{

				for(u8 byMtIdx = 0; byMtIdx < byNum; byMtIdx ++)
				{
					
					if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() 
						&& (m_tRollCaller.GetMtId() == abyMtId[byMtIdx] 
						|| m_tRollCallee.GetMtId() == abyMtId[byMtIdx])
						)
					{
						m_ptMtTable->GetMtStatus(abyMtId[byMtIdx], &tMtStatus);
						TMt tSelectedMt = tMtStatus.GetSelectMt( MODE_VIDEO ); 
						if(!tSelectedMt.IsNull())	//判断选看源真实存在，可能之前选看失败
						{
							continue;// xliang [4/24/2009] 点名人被点名人互相选看，VMP不交换码流给他们
						}
					}
					bySrcChnnl = GetVmpOutChnnlByRes(byRes, m_tVmpEqp.GetEqpId(), byMVType);
					ConfLog(FALSE, "VMP->MT: bySrcChnnl is %u\n", bySrcChnnl);
					if(bySrcChnnl < MAXNUM_MPU_OUTCHNNL)	
					{
#ifndef _SATELITE_
						StartSwitchToSubMt(*ptNewVidBrdSrc, bySrcChnnl, abyMtId[byMtIdx], MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode 非默认值，是SWITCH_MODE_SELECT
#else
						if (IsHDConf(m_tConf))
						{
							g_cMpManager.StartSatConfCast(*ptNewVidBrdSrc, CAST_SEC, MODE_VIDEO, bySrcChnnl);
						}
						else
						{
							g_cMpManager.StartSatConfCast(*ptNewVidBrdSrc, CAST_FST, MODE_VIDEO, bySrcChnnl);
						}
						m_ptMtTable->SetMtSrc(abyMtId[byMtIdx], ptNewVidBrdSrc, MODE_VIDEO);
#endif
						CallLog("Send New Vmp Chnnl.%u(%s) to Mt.%u\n",bySrcChnnl,GetResStr(byRes),abyMtId[byMtIdx]);
					}
				}
			}

		}
		else //是老vmp或其他外设/MT
		{

			//启用适配并且开启了双流, 强制降低新的第一路发送源的发送码率,没有适配器则根据FlowControl调整
			if (!m_tDoubleStreamSrc.IsNull() &&
				!m_tVidBrdSrc.IsNull() && TYPE_MT == m_tVidBrdSrc.GetType())
			{
				u16 wDialBitrate = 0;
				TLogicalChannel tLogicalChannel;
				if (TRUE == m_ptMtTable->GetMtLogicChnnl(m_tVidBrdSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
				{
					wDialBitrate = m_ptMtTable->GetSndBandWidth(m_tVidBrdSrc.GetMtId());
					tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(wDialBitrate));
					cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
					SendMsgToMt(m_tVidBrdSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
				} 
			 }
 

			//改变丢包重传源
			if (m_tConf.GetConfAttrb().IsResendLosePack())
			{
				ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_FIRST);
            
				//若已经在适配则改变prs源，若没有适配在适配ack中改变prs源
				if(m_tConf.m_tStatus.IsVidAdapting())
				{
					ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_VIDBAS, TRUE);
				}
				if(m_tConf.m_tStatus.IsBrAdapting())
				{
					ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_BRBAS, TRUE);
				}
            
				// 高清适配丢包重传, zgc, 2008-08-13            
				if( m_tConf.m_tStatus.IsHdVidAdapting() )
				{
					ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_HDBAS_VID, TRUE);
				}
            
				// FIXME: 高清适配双流丢包重传未添加, zgc, 2008-08-13
            
				//vmp.2, 启动vmp第二路的prs
				if (0 != m_tConf.GetSecBitRate() && 
					MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType() &&                
					m_tVidBrdSrc == m_tVmpEqp)
				{
					ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_VMP2);
				}
			}

			//建立交换桥
			bySrcChnnl = (*ptNewVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
			g_cMpManager.SetSwitchBridge(*ptNewVidBrdSrc, bySrcChnnl, MODE_VIDEO);
        
			// xsl [1/21/2006] 双速或双格式会议广播源为vmp时，搭建第二路交换桥
			if (m_tVidBrdSrc == m_tVmpEqp &&
				((0 != m_tConf.GetSecBitRate() || MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType())))
			{
				g_cMpManager.SetSwitchBridge(*ptNewVidBrdSrc, bySrcChnnl+1, MODE_VIDEO);           
			}

			//开始向录像机交换
			if (!m_tConf.m_tStatus.IsNoRecording())
			{
				AdjustRecordSrcStream( MODE_VIDEO );
			}

            //zbq[04/15/2009]
            if (IsDelayVidBrdVCU())
            {
                //Vmp轮询跟随状态下要抓到Tandberg的关键帧，延迟到VMP交换建好发youareseeing和fastupdate
            }
            else
            {
                //通知终端开始发送
                if (ptNewVidBrdSrc->GetType() == TYPE_MT)
                {			
                    NotifyMtSend(ptNewVidBrdSrc->GetMtId(), MODE_VIDEO);
			    }
            }

            //zbq[07/15/2009] 防止按低码率呼入 误判
            RefreshRcvGrp();

			//开始媒体类型适配
			if ( IsHDConf(m_tConf) )
			{
				if (m_tConf.GetConfAttrb().IsUseAdapter() && 
					m_cMtRcvGrp.IsNeedAdp())
				{
					StartHDMVBrdAdapt();

                    //zbq[07/30/2009] 录像过适配，无条件请适配关键帧3个：解决VidSrc切换，且解决
                    //                VidSrc晚置；初始录像的IFrm请求与此冲突的依赖底mau/mpu过滤
                    TEqp tBas;
                    u8 byBasChn = 0;
                    if (!m_tConf.m_tStatus.IsNoRecording() &&
                        IsRecordSrcBas(MODE_VIDEO, tBas, byBasChn))
                    {
                        NotifyEqpFastUpdate(tBas, byBasChn, TRUE);
                    }
				}
				else
				{
					StopHDMVBrdAdapt();
				}
			}
			else
			{
				if (ptNewVidBrdSrc->GetType() == TYPE_MT &&
					m_tConf.GetConfAttrb().IsUseAdapter())
				{
					if (!m_tConf.m_tStatus.IsVidAdapting() &&
						IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, ptNewVidBrdSrc))
					{
						StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
					}
					else if( !m_tConf.m_tStatus.IsVidAdapting() && IsNeedCifAdp() )
					{
						IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, ptNewVidBrdSrc); // 得到目标格式和分辨率
						StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
					}

					if (!m_tConf.m_tStatus.IsBrAdapting() &&
						IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, ptNewVidBrdSrc))
					{
						StartAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
					}
				}
			}

			//组播会议组播数据，低速组播时由适配器交换到组播地址
			if (m_tConf.GetConfAttrb().IsMulticastMode() && !m_tConf.GetConfAttrb().IsMulcastLowStream())
			{
				g_cMpManager.StartMulticast(m_tVidBrdSrc, bySrcChnnl, MODE_VIDEO);
				m_ptMtTable->SetMtMulticasting(m_tVidBrdSrc.GetMtId());
			}
                       
			// 顾振华@2006.04.12 如果是卫星分散组播
			if (m_tConf.GetConfAttrb().IsSatDCastMode())
			{

				//稍后遵从卫星会议的独立管理
			#ifdef _SATELITE_
				//此处只负责第一组播，第二组播从bas/vmp后适配出
				g_cMpManager.StartSatConfCast(m_tVidBrdSrc, CAST_FST, MODE_VIDEO, bySrcChnnl);

				for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
				{
					if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
						byMtId != m_tVidBrdSrc.GetMtId())
					{
						m_ptMtTable->SetMtSrc(byMtId, &m_tVidBrdSrc, MODE_VIDEO);
					}
				}
			#else
				g_cMpManager.StartDistrConfCast(m_tVidBrdSrc, MODE_VIDEO, bySrcChnnl);
			#endif
				TMtStatus tMtStatus;
				for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
				{
					if ( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
					{                    
						// xsl [7/21/2006]若在选看，拆掉交换，告诉其从组播地址接收
						if (m_ptMtTable->GetMtStatus(byLoop, &tMtStatus))
						{
							TMt tTmpMt = tMtStatus.GetSelectMt(MODE_VIDEO);
							if (!tTmpMt.IsNull())
							{
								StopSwitchToSubMt(byLoop, MODE_VIDEO, SWITCH_MODE_SELECT);                            
								ChangeSatDConfMtRcvAddr(byLoop, LOGCHL_VIDEO);
							}
						}

						// 通知终端接收源
						NotifyMtReceive( m_tVidBrdSrc, byLoop );
					}
				}
			}
			else
			{
				// 开始向各终端交换
				// guzh [3/21/2007] 改用新接口
				StartSwitchToAllSubMtJoinedConf(m_tVidBrdSrc, bySrcChnnl, tOldSrc, bStopOldVidSrcNow);

				//开始向码流适配器交换 -- modify bas 2
				// 增加高清适配判断, zgc, 2008-08-06
				u8 byAdaptChnNum = 0;
				if ( m_tConf.m_tStatus.IsHdVidAdapting() )
				{
					/*
					if ( IsNeedHDVidAdapt( tSrcSimCapSet, m_tVidBrdSrc ) )
					{
						if ( ChangeHDVidAdapt( tSrcSimCapSet, m_tVidBrdSrc ) )
						{
							byAdaptChnNum = m_cConfBasChnMgr.GetVidChnUseNum();
							THDBasVidChnStatus *ptVidChn = NULL;
							ptVidChn = m_cConfBasChnMgr.GetHDBasVidChnStatus( CConfBasChnMgr::emHD_FstVidChn );
							if ( NULL != ptVidChn )
							{
								StartSwitchToPeriEqp( m_tVidBrdSrc, bySrcChnnl, ptVidChn->GetEqpId(),
													 ptVidChn->GetChnIdx(), MODE_VIDEO);
							}
							if ( byAdaptChnNum == 2 )
							{
								ptVidChn = m_cConfBasChnMgr.GetHDBasVidChnStatus( CConfBasChnMgr::emHD_SecVidChn );
								if ( NULL != ptVidChn )
								{
									StartSwitchToPeriEqp( m_tVidBrdSrc, bySrcChnnl, ptVidChn->GetEqpId(),
														 ptVidChn->GetChnIdx(), MODE_VIDEO);
								}
							}
						}
						else
						{
							ConfLog( FALSE, "[ChangeVidBrdSrc] ChangeHDVidAdapt failed!\n" );
						}
					}
					*/
				}
				else // FIXME: 这里会对普通适配和高清适配混杂的情况产生冲击, zgc, 2008-08-08
				{
					if (m_tConf.m_tStatus.IsVidAdapting())
					{                
						if (IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, ptNewVidBrdSrc) 
							|| IsNeedCifAdp() )
						{
							StartSwitchToPeriEqp(*ptNewVidBrdSrc, bySrcChnnl, m_tVidBasEqp.GetEqpId(),
												 m_byVidBasChnnl, MODE_VIDEO);

							ChangeAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
						}
					}

					if (m_tConf.m_tStatus.IsBrAdapting())
					{                
						if (IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, ptNewVidBrdSrc) )
						{
							StartSwitchToPeriEqp(*ptNewVidBrdSrc, bySrcChnnl, m_tBrBasEqp.GetEqpId(),
												  m_byBrBasChnnl, MODE_VIDEO);

							ChangeAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
						}
					}
				}
			}
		}
		
#ifdef _SATELITE_
		RefreshConfState();
		
#endif
    }

	return;
}

/*====================================================================
    函数名      ：ChangeAudBrdSrc
    功能        ：改变音频广播源（包括单纯撤销）；
	              通知MC，MT和
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt * ptNewAudBrdSrc, 新的音频广播源，NULL表示单纯撤销
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/25    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ChangeAudBrdSrc( TMt * ptNewAudBrdSrc )
{
    u8	bySrcChnnl;    
    CServMsg cServMsg;
	TSimCapSet tSrcSimCapSet;   //modify bas 2
	TSimCapSet tDstSimCapSet;   //modify bas 2

    //停止广播原有音频源
    if( !m_tAudBrdSrc.IsNull() )
    {
        //停止第一路音频广播源的重传
        if( ptNewAudBrdSrc == NULL && m_tConf.m_tStatus.IsPrsing() )
        {
            StopPrs(PRSCHANMODE_AUDIO, FALSE);
        }        

        // zbq [08/23/2007] 当前源在回传通道里，则不能拆桥
        if ( !m_tCascadeMMCU.IsNull() &&
            m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId())->m_tSpyMt == m_tAudBrdSrc )
        {
        }
        else
        {
            //拆除交换桥
            bySrcChnnl = (m_tAudBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
            g_cMpManager.RemoveSwitchBridge( m_tAudBrdSrc, bySrcChnnl, MODE_AUDIO );
        }
        
        TMt tMt = m_tAudBrdSrc;
        m_tAudBrdSrc.SetNull();
        //只停那些收看广播源的终端
        if( ptNewAudBrdSrc == NULL )
        {
            RestoreAllSubMtJoinedConfWatchingSrcMt( tMt, MODE_AUDIO );
            // 如果是卫星分散组播，则停止
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
#ifdef _SATELITE_
				g_cMpManager.StopSatConfCast(tMt, CAST_FST, MODE_AUDIO, bySrcChnnl);
				g_cMpManager.StopSatConfCast(tMt, CAST_SEC, MODE_AUDIO, bySrcChnnl);

				for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
				{
					if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
						byMtId != m_tVidBrdSrc.GetMtId())
					{
						TMt tMtNull;
						tMtNull.SetNull();
						m_ptMtTable->SetMtSrc(byMtId, &tMtNull, MODE_AUDIO);
					}
				}
#else
				g_cMpManager.StopDistrConfCast(tMt, MODE_AUDIO, bySrcChnnl);
#endif
            }
        }

        //停止向录像机交换
        if( !m_tConf.m_tStatus.IsNoRecording() )
        {
            StopSwitchToPeriEqp( m_tRecEqp.GetEqpId(), m_byRecChnnl, FALSE, MODE_AUDIO );
        }
		
		//停止向码流适配器交换 -- modify bas 2
        if (EQP_CHANNO_INVALID != m_byAudBasChnnl)
        {
            StopSwitchToPeriEqp(m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl, FALSE, MODE_AUDIO);
        }
		
#ifdef _SATELITE_
		RefreshConfState();
		
#endif
    }

    //开始广播新音频源
    if (ptNewAudBrdSrc != NULL)
    {				
        m_tAudBrdSrc = *ptNewAudBrdSrc;

		bySrcChnnl = (*ptNewAudBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
        if (m_tConf.m_tStatus.IsMixing() && *ptNewAudBrdSrc == m_tMixEqp)
        {
            bySrcChnnl = m_byMixGrpId;
        }
        g_cMpManager.SetSwitchBridge(*ptNewAudBrdSrc, bySrcChnnl, MODE_AUDIO);
        
        // xsl [8/22/2006] 通知终端开始发送
        if (ptNewAudBrdSrc->GetType() == TYPE_MT)
        {			
            NotifyMtSend(ptNewAudBrdSrc->GetMtId(), MODE_AUDIO);
        }

        //改变丢包重传源
        if (m_tConf.GetConfAttrb().IsResendLosePack())
        {
            ChangePrsSrc(m_tAudBrdSrc, PRSCHANMODE_AUDIO);
            
            //若已经在适配则改变prs源，若没有适配在适配ack中改变prs源
            if(m_tConf.m_tStatus.IsAudAdapting())
            {
                ChangePrsSrc(m_tVidBrdSrc, PRSCHANMODE_AUDBAS, TRUE);
            }
 
        }

        //开始媒体类型适配 - modify bas 2
        if (ptNewAudBrdSrc->GetType() == TYPE_MT)
        {
            if (m_tConf.GetConfAttrb().IsUseAdapter())
            {
                if (!m_tConf.m_tStatus.IsAudAdapting() &&
                    IsNeedAudAdapt(tDstSimCapSet, tSrcSimCapSet, ptNewAudBrdSrc))
                {
                    StartAdapt(ADAPT_TYPE_AUD, 0, &tDstSimCapSet, &tSrcSimCapSet);
                }
            }
        }

        //开始向录像机交换
        if (!m_tConf.m_tStatus.IsNoRecording())
        {
            AdjustRecordSrcStream( MODE_AUDIO );
        }

        //组播会议组播数据
        if (m_tConf.GetConfAttrb().IsMulticastMode())
        {
            g_cMpManager.StartMulticast(*ptNewAudBrdSrc, bySrcChnnl, MODE_AUDIO);
            m_ptMtTable->SetMtMulticasting(ptNewAudBrdSrc->GetMtId());
        }
        // 顾振华@2006.4.13开始音频分散组播。
        if (m_tConf.GetConfAttrb().IsSatDCastMode())
        {
#ifdef _SATELITE_
			g_cMpManager.StartSatConfCast(*ptNewAudBrdSrc, CAST_FST, MODE_AUDIO, bySrcChnnl);

			//FIXME: 本动作对上一个函数有依赖性，尽快解耦
			//g_cMpManager.StartSatConfCast(*ptNewAudBrdSrc, CAST_SEC, MODE_AUDIO, bySrcChnnl, TRUE);

			for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
			{
				if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
					byMtId != m_tVidBrdSrc.GetMtId())
				{
					m_ptMtTable->SetMtSrc(byMtId, ptNewAudBrdSrc, MODE_AUDIO);
				}
			}
#else
			g_cMpManager.StartDistrConfCast(*ptNewAudBrdSrc, MODE_AUDIO, bySrcChnnl);
#endif
            // xsl [7/21/2006]若在选看，拆掉交换，告诉其从组播地址接收
            TMtStatus tMtStatus;
            for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
            {
                if( m_tConfAllMtInfo.MtJoinedConf( byLoop ))
                {                                      
                    if (m_ptMtTable->GetMtStatus(byLoop, &tMtStatus))
                    {
                        TMt tTmpMt = tMtStatus.GetSelectMt(MODE_AUDIO);
                        if (!tTmpMt.IsNull())
                        {
                            StopSwitchToSubMt(byLoop, MODE_AUDIO, SWITCH_MODE_SELECT);                            
                            ChangeSatDConfMtRcvAddr(byLoop, LOGCHL_AUDIO);
                        }
                    }
                }
            }

            // xsl [9/21/2006] 通知一下终端状态, 解决有发言人情况下停止混音终端状态通知问题
            MtStatusChange();
        }
        else
        {
            //开始向各终端交换
            if (!m_tConf.m_tStatus.IsMixing())
            {
                StartSwitchToAllSubMtJoinedConf(*ptNewAudBrdSrc, bySrcChnnl);
            }
        }

		//开始向码率适配器交换 -- modify bas 2
        if (m_tConf.m_tStatus.IsAudAdapting())
        {            
            if (IsNeedAudAdapt(tDstSimCapSet, tSrcSimCapSet, ptNewAudBrdSrc))
            {
                StartSwitchToPeriEqp(*ptNewAudBrdSrc, bySrcChnnl, m_tAudBasEqp.GetEqpId(),
                    m_byAudBasChnnl, MODE_AUDIO);
                
                ChangeAdapt(ADAPT_TYPE_AUD, 0, &tDstSimCapSet, &tSrcSimCapSet);
            }
        }
		
#ifdef _SATELITE_
		RefreshConfState();
		
#endif
	}

	return;
}

/*=============================================================================
函 数 名： IsSpeakerCanBrdVid
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/3/5   4.0		周广程                  创建
=============================================================================*/
BOOL32 CMcuVcInst::IsSpeakerCanBrdVid(void)
{
	//改变视频交换, 强制广播时不改变画面合成比发言人优先级高的策略
	if ( m_tConf.m_tStatus.IsBrdstVMP() )
	{
		return FALSE;
	}

	if ( !m_tConf.m_tStatus.IsNoPlaying() )
	{
		return FALSE;
	}

	if ( CONF_POLLMODE_VIDEO == m_tConf.m_tStatus.GetPollMode() )
	{
		return FALSE;
	}

	// KDV-BUG2004: 点名人或被点名人被强制为发言人，而在MTC上终端状态未改变
	// 对于由于点名造成的发言人变化，且是VMP合成模式，则不允许发言人广播视频，
	// 目的是为了减少建广播交换的码流开销
	// zgc, 2008-05-21, 					
	if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
	{
		return FALSE;
	}

	return TRUE;
}

/*=============================================================================
函 数 名： IsSpeakerCanBrdAud
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/3/5   4.0		周广程                  创建
=============================================================================*/
BOOL32 CMcuVcInst::IsSpeakerCanBrdAud(void)
{
	if (VCS_CONF == m_tConf.GetConfSource() &&
		!m_tMixEqp.IsNull())
	{
		TPeriEqpStatus tPeriEqpStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
		u8  byState = tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState;
		if (TMixerGrpStatus::WAIT_START_SPECMIX == byState ||
			TMixerGrpStatus::WAIT_START_AUTOMIX == byState)
		{
			return FALSE;
		}
	}

	if ( !m_tConf.m_tStatus.IsNoMixing() 
		&& !m_tConf.m_tStatus.IsMustSeeSpeaker() )
	{
		return FALSE;
	}

	if ( !m_tConf.m_tStatus.IsNoPlaying() )
	{
		return FALSE;
	}

	// KDV-BUG2004: 点名人或被点名人被强制为发言人，而在MTC上终端状态未改变
	// zgc, 2008-05-21, 对于由于点名造成的发言人变化，不允许发言人广播音频
	// 因为点名操作里是必然要开混音器混音的
	if ( ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
	{
		return FALSE;
	}

	return TRUE;
}


/*=============================================================================
函 数 名： ChangeSpeakerSrc
功    能： 
算法实现： 
全局变量： 
参    数：  u8 byMode
           emChangeSpeakerSrcReason emReason
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/12   4.0			周广程                  创建
=============================================================================*/
void CMcuVcInst::ChangeSpeakerSrc( u8 byMode, emChangeSpeakerSrcReason emReason )
{
	if ( !HasJoinedSpeaker() || m_tConf.GetSpeaker().GetType() == TYPE_MCUPERI )
	{
		ConfLog( FALSE, "[ChangeSpeakerSrc] No speaker or speaker is perieqp.\n" );
		return;
	}

	if ( emReason == emReasonUnkown )
	{
		ConfLog( FALSE, "[ChangeSpeakerSrc] Change reason is unkown!\n" );
		return;
	}
	
	TMt tLocalSpeaker = GetLocalMtFromOtherMcuMt( m_tConf.GetSpeaker() );
	TMt tLocalChairman = GetLocalMtFromOtherMcuMt( m_tConf.GetChairman() ); 
	TMt tLocalLastSpeaker = GetLocalMtFromOtherMcuMt( m_tLastSpeaker ); 
	
	u8 bySelMode = m_tConf.GetConfAttrb().GetSpeakerSrcMode();
	u8 byTempMode = MODE_NONE;

	if ( byMode == MODE_BOTH )
	{
		ChangeSpeakerSrc( MODE_AUDIO, emReason );
		ChangeSpeakerSrc( MODE_VIDEO, emReason );
		return;
	}
	
	if ( byMode == MODE_VIDEO && (bySelMode == MODE_BOTH || bySelMode == MODE_VIDEO) )
	{
		byTempMode = MODE_VIDEO;
	}
	if ( byMode == MODE_AUDIO && bySelMode == MODE_BOTH )
	{
		byTempMode = MODE_AUDIO;
	}

	if ( byTempMode == MODE_NONE && emReason != emReasonChangeBrdSrc )
	{
		return;
	}
	
	u8 bySpeakerSrcType = m_tConf.GetConfAttrb().GetSpeakerSrc();

	if ( (bySpeakerSrcType == CONF_SPEAKERSRC_LAST && emReason == emReasonChangeChairman) ||
		(bySpeakerSrcType == CONF_SPEAKERSRC_SELF && emReason != emReasonChangeBrdSrc) ||
		( m_tConf.m_tStatus.IsMixing() && byMode == MODE_AUDIO && emReason != emReasonChangeBrdSrc ) )
	{
		//发言人选看上一次发言人模式下，主席发生change不需要进行调整;
		//发言人看自己且不是广播源变化引起的，不需要进行调整;
		//会议在混音时，音频交换不需要调整，广播源变化除外;
		return;
	}

	BOOL32 bRet = TRUE;

	// 获得发言人当前源的情况
	TMtStatus tStatus;
	m_ptMtTable->GetMtStatus( tLocalSpeaker.GetMtId(), &tStatus );

	TMt tOldSrc;
	u8 byOldSrcChn = 0;
	u8 bySpeakerSrcSpecType = SPEAKER_SRC_NOTSEL;
	BOOL32 bIsSel = FALSE;

	m_ptMtTable->GetMtSrc( tLocalSpeaker.GetMtId(), &tOldSrc, byMode );
	byOldSrcChn = ( tOldSrc == m_tPlayEqp ) ? m_byPlayChnnl : 0;
	bySpeakerSrcSpecType = GetSpeakerSrcSpecType( byMode );
	if ( !tOldSrc.IsNull() && !(tOldSrc == tLocalLastSpeaker) && 
		(tStatus.GetSelByMcsDragMode() == MODE_BOTH ||
		 tStatus.GetSelByMcsDragMode() == byMode ) )
	{
		bIsSel = TRUE;
	}

	// 选看，但标志位未置，说明选看时不是发言人，这里需要设置正确的标志
	if ( bIsSel && bySpeakerSrcSpecType == SPEAKER_SRC_NOTSEL )
	{
		SetSpeakerSrcSpecType( byMode, SPEAKER_SRC_MCSDRAGSEL );
		bySpeakerSrcSpecType = GetSpeakerSrcSpecType( byMode );
	}

	// 由广播源变化引起的调整
	BOOL32 bCancelBrdSrc = TRUE;
	if ( emReason == emReasonChangeBrdSrc && 
		(( byMode == MODE_AUDIO && !m_tAudBrdSrc.IsNull() ) ||
		 ( byMode == MODE_VIDEO && !m_tVidBrdSrc.IsNull() )) )
	{
		bCancelBrdSrc = FALSE;

		u8 byBrdSrcChn = 0;
		TMt tBrdSrc;

		if ( byMode == MODE_AUDIO)
		{
			tBrdSrc = m_tAudBrdSrc;
		}
		else
		{
			tBrdSrc = m_tVidBrdSrc;
		}
		// 广播源不是发言人，建从广播源到发言人的广播交换
		if ( !(tBrdSrc == tLocalSpeaker) )
		{			
			byBrdSrcChn = ( tBrdSrc == m_tPlayEqp ) ? m_byPlayChnnl : 0;

			if ( byMode == MODE_VIDEO )
			{
				bRet = StartSwitchFromBrd( tBrdSrc, byBrdSrcChn, 1, &tLocalSpeaker );
			}
			else
			{
				bRet = StartSwitchToSubMt( tBrdSrc, byBrdSrcChn, tLocalSpeaker.GetMtId(), MODE_AUDIO );
			}
			if ( bRet )
			{
				SetSpeakerSrcSpecType( byMode, SPEAKER_SRC_NOTSEL );
				return;
			}
			ConfLog( FALSE, "[ChangeSpeakerSrc] Speaker(Mt.%d) receive broarcastsrc(id.%d)(mode.%d) failed.\n", 
					tLocalSpeaker.GetMtId(), tBrdSrc.GetMtId(), byMode );
		}
	}
	
	// 发言人选看主席的模式
	if( bySpeakerSrcType == CONF_SPEAKERSRC_CHAIR
		&& HasJoinedChairman() 
		&& byTempMode != MODE_NONE
		&& !(tLocalSpeaker == tLocalChairman) )
	{
		// 如果原来发言人选看码流源就是当前的主席终端，则不必重新选看
		if ( !tOldSrc.IsNull() && bIsSel && tOldSrc == tLocalChairman )
		{
			return;
		}

		if( // 选看不成要恢复，防止死屏 或 重置了广播通道，在下面恢复
			ChangeSelectSrc( m_tConf.GetChairman(), tLocalSpeaker, byTempMode ) )//有主席看主席
		{
			SetSpeakerSrcSpecType( byTempMode, SPEAKER_SRC_CONFSEL );
			return;				
		}
		ConfLog( FALSE, "[ChangeSpeakerSrc] Speaker(Mt.%d) select chairman(mt.%d)(mode.%d) failed.\n",
						tLocalSpeaker.GetMtId(), tLocalChairman.GetMtId(), byTempMode );
	}
	
	// 发言人选看上一次发言人模式
	if (bySpeakerSrcType == CONF_SPEAKERSRC_LAST
		&& byTempMode != MODE_NONE
		&& !m_tLastSpeaker.IsNull()
		&& !(tLocalSpeaker == tLocalLastSpeaker))
	{
		// 如果原来发言人选看码流源就是上一次发言人，则不必重新选看
		if ( !tOldSrc.IsNull() && bIsSel && tOldSrc == tLocalLastSpeaker )
		{
			return;
		}

		if ( //选看不成要恢复，防止死屏 或 重置了广播通道，在下面恢复
			ChangeSelectSrc( m_tLastSpeaker, tLocalSpeaker, byTempMode ) )
		{
			SetSpeakerSrcSpecType( byTempMode, SPEAKER_SRC_CONFSEL );
			return;	
		}
		ConfLog( FALSE, "[ChangeSpeakerSrc] Speaker(Mt.%d) select lastspeaker(mt.%d)(mode.%d) failed.\n",
						tLocalSpeaker.GetMtId(), tLocalLastSpeaker.GetMtId(), byTempMode );
	}
	
	// 这里负责处理发言人看自己以及上面处理失败时的保护
	if ( !tOldSrc.IsNull() && bIsSel &&
		SPEAKER_SRC_MCSDRAGSEL == bySpeakerSrcSpecType )
	{
		// 如果是发言人看自己且不是由于广播源变化（不包括取消广播源）引起的调整，
		// 则在之前不会引起修改发言人源交换的操作，这里不需要重新选看
		if ( bySpeakerSrcType != CONF_SPEAKERSRC_SELF || 
			( emReason == emReasonChangeBrdSrc && !bCancelBrdSrc ))
		{
			bRet = ChangeSelectSrc( tOldSrc, tLocalSpeaker, byMode );
			if ( !bRet )
			{
				RestoreRcvMediaBrdSrc( tLocalSpeaker.GetMtId(), byMode );
				SetSpeakerSrcSpecType( byMode, SPEAKER_SRC_NOTSEL );
			}
		}
	}
	else
	{
		RestoreRcvMediaBrdSrc( tLocalSpeaker.GetMtId(), byMode );
		SetSpeakerSrcSpecType( byMode, SPEAKER_SRC_NOTSEL );
	}	
}

/*=============================================================================
函 数 名： ChangeOldSpeakerSrc
功    能： 修改被取消的老发言人的源，必须在重设上一次发言人之前调用
算法实现： 
全局变量： 
参    数： u8 byMode
		   TMt tOldSpeaker
		   BOOL32 bIsHaveNewSpeaker 
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/15   4.0			周广程                  创建
=============================================================================*/
void CMcuVcInst::ChangeOldSpeakerSrc( u8 byMode, TMt tOldSpeaker, BOOL32 bIsHaveNewSpeaker )
{
	if ( tOldSpeaker.IsNull() )
	{
		return;
	}
	
	TMt tLocalOldSpeaker = GetLocalMtFromOtherMcuMt( tOldSpeaker );
	
	BOOL32 bIsJoinedMt = FALSE;
	if( tLocalOldSpeaker.GetType() == TYPE_MT && m_tConfAllMtInfo.MtJoinedConf( tLocalOldSpeaker.GetMtId() ) )
	{
		bIsJoinedMt = TRUE;
	}

	if ( byMode == MODE_BOTH )
	{
		ChangeOldSpeakerSrc( MODE_AUDIO, tOldSpeaker, bIsHaveNewSpeaker );
		ChangeOldSpeakerSrc( MODE_VIDEO, tOldSpeaker, bIsHaveNewSpeaker );
		return;
	}
	
	// 停止向原发言人的交换
    if ( bIsJoinedMt && !bIsHaveNewSpeaker )  // 取消发言人
    {
		TMt tSrc;
        m_ptMtTable->GetMtSrc( tLocalOldSpeaker.GetMtId(), &tSrc, byMode);

		TMt tLocalChairman = GetLocalMtFromOtherMcuMt(m_tConf.GetChairman());
		TMt tLocalLastSpeaker = GetLocalMtFromOtherMcuMt(m_tLastSpeaker);
		u8 bySpeakerSrcSpecType = GetSpeakerSrcSpecType( byMode );
		u8 bySpeakerSrc = m_tConf.GetConfAttrb().GetSpeakerSrc();
		u8 bySpeakerSrcMode = m_tConf.GetConfAttrb().GetSpeakerSrcMode();

		if ( !tSrc.IsNull() && bySpeakerSrcSpecType == SPEAKER_SRC_CONFSEL &&
			((tSrc == tLocalChairman && bySpeakerSrc == CONF_SPEAKERSRC_CHAIR) ||
			 (tSrc == tLocalLastSpeaker && bySpeakerSrc == CONF_SPEAKERSRC_LAST)) &&
			 ( byMode == MODE_VIDEO || (byMode == MODE_AUDIO && bySpeakerSrcMode == MODE_BOTH) ) )
		{
			if (m_ptMtTable->GetManuId( tLocalOldSpeaker.GetMtId() ) == MT_MANU_KDC)
			{
				StopSwitchToSubMt( tLocalOldSpeaker.GetMtId(), byMode, SWITCH_MODE_BROADCAST, TRUE);
			}
			else
			{
				//非kdc厂商回传自身视频, 节省带宽时，通知mt发送码流
				// 增加视频限制, zgc, 2008-04-25
				if(g_cMcuVcApp.IsSavingBandwidth() && byMode == MODE_VIDEO )
				{
					NotifyOtherMtSend( tLocalOldSpeaker.GetMtId(), TRUE );
				}     
				
				StartSwitchToSubMt( tLocalOldSpeaker, 0, tLocalOldSpeaker.GetMtId(), byMode, SWITCH_MODE_BROADCAST, TRUE);
			}	
		}
    }
	
	// 重建VMP或视频轮询到老发言人的视频交换
	if ( bIsJoinedMt && byMode == MODE_VIDEO )
	{
		TMtStatus tStatus;
		m_ptMtTable->GetMtStatus( tLocalOldSpeaker.GetMtId(), &tStatus );
		BOOL32 bIsSelMode = ( tStatus.GetSelByMcsDragMode() == MODE_VIDEO || tStatus.GetSelByMcsDragMode() == MODE_BOTH );
		// 增加对选看方式的判断, zgc, 2008-04-25
		if ( !bIsSelMode || ( bIsSelMode && GetSpeakerSrcSpecType( byMode ) == SPEAKER_SRC_CONFSEL ) )
		{
			RestoreRcvMediaBrdSrc( tLocalOldSpeaker.GetMtId(), MODE_VIDEO );  
		}
	}

	// 恢复标志为无选看
	SetSpeakerSrcSpecType( byMode, SPEAKER_SRC_NOTSEL );
}
/*=============================================================================
函 数 名： GetSpeakerSrcSpecType
功    能： 会议发言人的源是否是由会议自动指定
算法实现： 
全局变量： 
参    数： u8 byMode
返 回 值： u8
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/14   4.0		周广程                  创建
=============================================================================*/
u8 CMcuVcInst::GetSpeakerSrcSpecType(u8 byMode)
{
	switch( byMode )
	{
	case MODE_VIDEO:
		return m_bySpeakerVidSrcSpecType;
		break;
	case MODE_AUDIO:
		return m_bySpeakerAudSrcSpecType;
		break;
	default:
		ConfLog( FALSE, "[GetSpeakerSrcSpecType] Mode(%d) is error,return NOTSEL!\n", byMode );
		break;
	}
	return SPEAKER_SRC_NOTSEL;
}

/*=============================================================================
函 数 名： SetSpeakerSrcSpecType
功    能： 
算法实现： 
全局变量： 
参    数： u8 byMode
           u8 bySpecType
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/14   4.0		周广程                  创建
=============================================================================*/
void CMcuVcInst::SetSpeakerSrcSpecType(u8 byMode, u8 bySpecType )
{
	switch( byMode )
	{
	case MODE_VIDEO:
		m_bySpeakerVidSrcSpecType = bySpecType;
		break;
	case MODE_AUDIO:
		m_bySpeakerAudSrcSpecType = bySpecType;
		break;
	case MODE_BOTH:
		m_bySpeakerVidSrcSpecType = bySpecType;
		m_bySpeakerAudSrcSpecType = bySpecType;
		break;
	default:
		ConfLog( FALSE, "[SetSpeakerSrcSpecType] Mode(%d) is error,set failed!\n", byMode );
		break;
	}
	return;
}

/*=============================================================================
函 数 名： AdjustOldSpeakerSwitch
功    能： 调整被取消的老发言人的交换
算法实现： 
全局变量： 
参    数：  TMt tOldSpeaker				: 老发言人，未经过GetLocal的调整
			BOOL32 bIsHaveNewSpeaker	: 是否有新发言人
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/3/4   4.0		周广程                  创建
=============================================================================*/
void CMcuVcInst::AdjustOldSpeakerSwitch( TMt tOldSpeaker, BOOL32 bIsHaveNewSpeaker )
{
	if ( tOldSpeaker.IsNull() )
	{
		return;
	}

	TMt tLocalOldSpeaker = GetLocalMtFromOtherMcuMt( tOldSpeaker );

	BOOL32 bIsJoinedMt = FALSE;
	if( tLocalOldSpeaker.GetType() == TYPE_MT && m_tConfAllMtInfo.MtJoinedConf( tLocalOldSpeaker.GetMtId() ) )
	{
		bIsJoinedMt = TRUE;
	}

	CServMsg cServMsg;
	cServMsg.SetMsgBody( ( u8 * )&tOldSpeaker, sizeof( tOldSpeaker ) );
	cServMsg.SetNoSrc();

	if( tOldSpeaker.GetType() == TYPE_MT )
	{
		if(tOldSpeaker.IsLocal())
		{
			SendMsgToMt( tOldSpeaker.GetMtId() , MCU_MT_CANCELSPEAKER_NOTIF,cServMsg );
		}
	}
	else if( tOldSpeaker.GetType() == TYPE_MCUPERI )
	{
		cServMsg.SetChnIndex( m_byPlayChnnl );	// xliang [8/19/2008] Chanel index一定要设

		SendMsgToEqp( tOldSpeaker.GetEqpId(), MCU_REC_STOPPLAY_REQ, cServMsg );
		m_tConf.m_tStatus.SetNoPlaying();
		ConfModeChange();
	}

	// 老发言人的码流源重整逻辑, zgc, 2008-04-15
	ChangeOldSpeakerSrc( MODE_BOTH, tOldSpeaker, bIsHaveNewSpeaker );

	/*
	TMt tSpeakerSrc;
	// 停止向原发言人的交换
    if ( bIsJoinedMt && !bIsHaveNewSpeaker )  // 取消发言人
    {
        m_ptMtTable->GetMtSrc( tLocalOldSpeaker.GetMtId(), &tSpeakerSrc, MODE_VIDEO);
		
        if ((tSpeakerSrc == GetLocalMtFromOtherMcuMt(m_tConf.GetChairman()) && m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_CHAIR) || 
            ((tSpeakerSrc == GetLocalMtFromOtherMcuMt(m_tLastSpeaker)) && m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_LAST))
        {				
			if (m_ptMtTable->GetManuId( tLocalOldSpeaker.GetMtId() ) == MT_MANU_KDC)
			{
				StopSwitchToSubMt( tLocalOldSpeaker.GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
			}
			else
			{
				//非kdc厂商回传自身视频, 节省带宽时，通知mt发送码流
				if(g_cMcuVcApp.IsSavingBandwidth())
				{
					NotifyOtherMtSend( tLocalOldSpeaker.GetMtId(), TRUE );
				}     

				StartSwitchToSubMt( tLocalOldSpeaker, 0, tLocalOldSpeaker.GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE);
			}	
        }
					
		// 增加对音频的处理, zgc, 2008-02-02
		m_ptMtTable->GetMtSrc( tLocalOldSpeaker.GetMtId(), &tSpeakerSrc, MODE_AUDIO );
		if ( ((tSpeakerSrc == GetLocalMtFromOtherMcuMt(m_tConf.GetChairman()) && m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_CHAIR) || 
            ((tSpeakerSrc == GetLocalMtFromOtherMcuMt(m_tLastSpeaker)) && m_tConf.GetConfAttrb().GetSpeakerSrc() == CONF_SPEAKERSRC_LAST))
			&& MODE_BOTH == m_tConf.GetConfAttrb().GetSpeakerSrcMode() )
        {
			if (m_ptMtTable->GetManuId( tLocalOldSpeaker.GetMtId()) == MT_MANU_KDC )
			{
				StopSwitchToSubMt( tLocalOldSpeaker.GetMtId(), MODE_AUDIO, SWITCH_MODE_BROADCAST, TRUE );
			}
			else
			{
				//非kdc厂商回传自身视频, 节省带宽时，通知mt发送码流
				if( g_cMcuVcApp.IsSavingBandwidth() )
				{
					NotifyOtherMtSend( tLocalOldSpeaker.GetMtId(), TRUE );
				}     
				
				StartSwitchToSubMt( tLocalOldSpeaker, 0, tLocalOldSpeaker.GetMtId(), MODE_AUDIO, SWITCH_MODE_BROADCAST, TRUE );
			}
		}
    }
	
	// 重建VMP或视频轮询到老发言人的视频交换, zgc, 2008-02-02
	if ( bIsJoinedMt )
	{
		// 新发言人码流源调整逻辑修改, zgc, 2008-04-12
		TMtStatus tStatus;
		m_ptMtTable->GetMtStatus( tLocalOldSpeaker.GetMtId(), &tStatus );
		BOOL32 bIsSelMode = ( tStatus.GetSelByMcsDragMode() == MODE_VIDEO || tStatus.GetSelByMcsDragMode() == MODE_BOTH );
		if ( !bIsSelMode )
		{
			RestoreRcvMediaBrdSrc( tLocalOldSpeaker.GetMtId(), MODE_VIDEO );  
		}
	}
	*/
		
	//取消强制混音
	if( m_tConf.m_tStatus.IsMixing() )
	{		
		// xsl [7/24/2006] 取消发言时，不从定制混音列表中删除，//规格书要求保留在混音组
		// 对于VCS会议需要将取消的发言人从混音组中去除
		if (!m_tConfInStatus.IsLastSpeakerInMixing() &&
			VCS_CONF == m_tConf.GetConfSource() &&
			!(tOldSpeaker == m_tConf.GetChairman()))
		{
			if (tOldSpeaker.IsLocal())
			{
				// VCS即使混音参数为空也不自动停混音
				AddRemoveSpecMixMember(&tOldSpeaker, 1, FALSE, TRUE); 
			}
			 else
			{
				 CServMsg cMsg;
				 TMcu tMcu;
				 tMcu.SetMcu(tOldSpeaker.GetMcuId());
				 cMsg.SetMsgBody((u8*)&tMcu, sizeof(tMcu));
				 cMsg.CatMsgBody((u8*)&tOldSpeaker, sizeof(tOldSpeaker));
				 OnAddRemoveMixToSMcu(&cMsg, FALSE);
			}
		}    
		else
		{
			m_tConfInStatus.SetLastSpeakerInMixing(FALSE);
		}  
		
		// xsl [8/4/2006] 取消强制混音
		RemoveMixMember(&tLocalOldSpeaker, TRUE);
	}

	//TvWall
	TPeriEqpStatus tTWStatus;
    u8 byChnlIdx;
    u8 byEqpId;
    //TvWall
	for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
	{
		if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
		{
			if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus))
			{
				u8 byMemberType;
				u8 byMtConfIdx;
				for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
				{
					byMemberType = tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType;
					byMtConfIdx = tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx();
					if (TW_MEMBERTYPE_SPEAKER == byMemberType && m_byConfIdx == byMtConfIdx)
					{
						ChangeTvWallSwitch(&tOldSpeaker, byEqpId, byChnlIdx, TW_MEMBERTYPE_SPEAKER, TW_STATE_STOP, FALSE);
					}
				}
			}
		}
    }
    
	//Hdu
	TPeriEqpStatus tHduStatus;
    u8 byHduChnlIdx;
    u8 byHduEqpId;
	for (byHduEqpId = HDUID_MIN; byHduEqpId <= HDUID_MAX; byHduEqpId++)
	{
		if (EQP_TYPE_HDU == g_cMcuVcApp.GetEqpType(byHduEqpId))
		{
			if (g_cMcuVcApp.GetPeriEqpStatus(byHduEqpId, &tHduStatus))
			{
				u8 byMemberType;
				u8 byMtConfIdx;
				for (byHduChnlIdx = 0; byHduChnlIdx < MAXNUM_HDU_CHANNEL; byHduChnlIdx++)
				{
					byMemberType = tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].byMemberType;
					byMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetConfIdx();
					if (TW_MEMBERTYPE_SPEAKER == byMemberType && m_byConfIdx == byMtConfIdx && bIsHaveNewSpeaker == FALSE)
					{
						ChangeHduSwitch(&tOldSpeaker, byHduEqpId, byHduChnlIdx, TW_MEMBERTYPE_SPEAKER, TW_STATE_STOP);
					}
				}
			}
		}
    }    

    // xliang [1/21/2009] 由最后移至此处
    m_tConf.SetNoSpeaker();
	if ( tOldSpeaker.GetType() == TYPE_MT )
	{
		m_tLastSpeaker = tOldSpeaker;
	}

	// xliang [12/12/2008] 判断是否是vmp通道被抢占情况下的取消发言人
	//是，则降该MT的分辨率。兼容以前用老VMP的情况
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
	

	if ( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE &&	//会议在vmp中
		m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tOldSpeaker))	//老发言人MT在vmp合成成员中
	{	
		TVMPParam	tVMPParam   = m_tConf.m_tStatus.GetVmpParam();
		u8	byChlPos	= tVMPParam.GetChlOfMtInMember(tOldSpeaker);
		if(byVmpSubType != VMP)//用的VMP是HD能力的VMP
		{
			if( tVMPParam.GetVmpMember(byChlPos)->GetMemberType() == VMP_MEMBERTYPE_SPEAKER ) //该通道正好是发言人跟随通道
			{
				//该情况会重新ChangeVMPParam, 之后全体来一次分辨率调整，所以这里就不进行单独调整了
				CallLog("[adjustOldSpeakerSwitch] no need to change format for old Speaker Mt.%, \
					because the channel it occupied is【VMP_MEMBERTYPE_SPEAKER】.\n", tOldSpeaker.GetMtId());
			}
			else
			{
				// xliang [4/21/2009]  退出VMP高清前适配通道，调整分辨率 
				ChangeMtVideoFormat(tOldSpeaker, &tVMPParam );
			}
		}
		else
		{
			ChangeMtVideoFormat(tOldSpeaker, &tVMPParam);
		}
	}
	else if ( m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE && 
		m_tConf.m_tStatus.GetVmpTwParam().IsMtInMember(tOldSpeaker) )
    {
        TVMPParam tVmpTwParam = m_tConf.m_tStatus.GetVmpTwParam();
        ChangeMtVideoFormat(tOldSpeaker, &tVmpTwParam);
    }
	
	// xliang [12/17/2008] 下面的改变vmp分辨率参数整合到上面
    // xsl [8/28/2006] 改变vmp分辨率参数
	//     if ( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE && 
	//          m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tOldSpeaker) )
	//     {
	//         TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
	// 		ChangeMtVideoFormat(tOldSpeaker.GetMtId(), &tVmpParam);
	//     }
	//     else if ( m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE && 
	//               m_tConf.m_tStatus.GetVmpTwParam().IsMtInMember(tOldSpeaker) )
	//     {
	//         TVMPParam tVmpTwParam = m_tConf.m_tStatus.GetVmpTwParam();
	//         ChangeMtVideoFormat(tOldSpeaker.GetMtId(), &tVmpTwParam);
	//     }

	return;
}

/*=============================================================================
函 数 名： AdjustNewSpeakerSwitch
功    能： 调整新指定的发言人的交换
算法实现： 
全局变量： 
参    数：  TMt tNewSpeaker		: 新指定的发言人，未经过GetLocal的调整
			BOOL32 bAddToVmp	: 是否要加入画面合成
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/3/4    4.0			周广程                  创建
=============================================================================*/
void CMcuVcInst::AdjustNewSpeakerSwitch( TMt tNewSpeaker, BOOL32 bAddToVmp )
{
	TMt tLocalNewSpeaker;
	tLocalNewSpeaker.SetNull();
	if ( !tNewSpeaker.IsNull() )
	{
		tLocalNewSpeaker = GetLocalMtFromOtherMcuMt( tNewSpeaker );
	}
	
	if ( !tNewSpeaker.IsNull() )
	{
		if(	tLocalNewSpeaker.GetType() == TYPE_MT && tNewSpeaker.IsLocal() )		//直连终端
		{
			CServMsg cServMsg;
			cServMsg.SetMsgBody( ( u8 * )&tLocalNewSpeaker, sizeof( TMt ) );
			cServMsg.SetNoSrc();
			SendMsgToMt( tLocalNewSpeaker.GetMtId(), MCU_MT_SPECSPEAKER_NOTIF,cServMsg );
		}		
		m_tConf.SetSpeaker( tNewSpeaker );
	}

	//改变视频交换, 强制广播时不改变画面合成比发言人优先级高的策略
	if ( IsSpeakerCanBrdVid()
		 /* || (m_tConf.m_tStatus.IsMustSeeSpeaker() &&
		 CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode())*/)
	{
		if ( !tNewSpeaker.IsNull() )
		{
			TMtStatus tMtStatus;
			if ( tLocalNewSpeaker.GetType() == TYPE_MT )
			{
				m_ptMtTable->GetMtStatus( tLocalNewSpeaker.GetMtId(), &tMtStatus );
			}
			if ( tMtStatus.IsSendVideo() || TYPE_MCUPERI == tLocalNewSpeaker.GetType() )
			{
				// xliang [7/9/2009] （非发言人）主席选看VMP这里要停掉
				if(m_tConf.m_tStatus.GetVmpParam().IsVMPSeeByChairman() 
					&& !(m_tConf.GetChairman() == tLocalNewSpeaker)
					)
				{
					ConfLog(FALSE, "Chairman.%u stop watching VMP because other Mt's speaking!\n",
						m_tConf.GetChairman().GetMtId());
					m_tConf.m_tStatus.SetVmpSeebyChairman(FALSE);

					// 状态同步刷新到TPeriStatus中
					TPeriEqpStatus tVmpStatus;
					g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
					tVmpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
					g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );

					StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO);
				}
				ChangeVidBrdSrc( &tLocalNewSpeaker );
			}
			else
			{
				ChangeVidBrdSrc(NULL);
			}
		}
		else
		{
			ChangeVidBrdSrc(NULL);
		}
	}

	//改变语音交换
	if ( IsSpeakerCanBrdAud() )
	{
		if ( !tNewSpeaker.IsNull() )
		{
			TMtStatus tMtStatus;
			if ( tLocalNewSpeaker.GetType() == TYPE_MT )
			{
				m_ptMtTable->GetMtStatus( tLocalNewSpeaker.GetMtId(), &tMtStatus );
			}
			if ( tMtStatus.IsSendAudio() || TYPE_MCUPERI == tLocalNewSpeaker.GetType() )
			{
				ChangeAudBrdSrc( &tLocalNewSpeaker );
			}
			else
			{
				ChangeAudBrdSrc(NULL);
			}
		}
		else
		{
			ChangeAudBrdSrc(NULL);
		}
	}

	if ( !tNewSpeaker.IsNull() && tNewSpeaker.GetType() == TYPE_MT )
	{
		//指定混音时参加混音
		if( m_tConf.m_tStatus.IsSpecMixing() )
		{
            // xsl [7/28/2006] 记录原来是否在混音，决定取消发言人时是否从定制混音列表中删除
            if (tNewSpeaker.IsLocal())
            {
                if (m_ptMtTable->IsMtInMixing(tNewSpeaker.GetMtId()))
                {
                    m_tConfInStatus.SetLastSpeakerInMixing(TRUE);
                }
            }
            else
            {
                if (m_ptConfOtherMcTable->IsMtInMixing(tNewSpeaker))
                {
                    m_tConfInStatus.SetLastSpeakerInMixing(TRUE);
                }
            }
			
            if (!m_tConfInStatus.IsLastSpeakerInMixing())
            {
                // zbq [05/14/2007] 音频逻辑通道建立，方可把该发言人加入混音。
                // 否则，可能是音频逻辑通道打开失败 或 重呼挂断的发言人(音频逻
                // 辑通道还未打开)，后者将在音频逻辑通道成功打开时作相应的处理.
                TLogicalChannel tLogicChan;
                if (m_ptMtTable->GetMtLogicChnnl(tLocalNewSpeaker.GetMtId(), LOGCHL_AUDIO, &tLogicChan, TRUE))
                {
                    AddRemoveSpecMixMember( &tLocalNewSpeaker, 1, TRUE );
                }
            }		    
		}

		// 新发言人码流源调整逻辑, zgc, 2008-04-12
        if ( !IsSpeakerCanBrdVid() )
        {
            ChangeSpeakerSrc( MODE_VIDEO, emReasonChangeSpeaker );
        }
        if ( !IsSpeakerCanBrdAud() )
        {
            ChangeSpeakerSrc( MODE_AUDIO, emReasonChangeSpeaker );
        }
	}
	//加入画面合成
	// xliang [3/13/2009] change either format or param
	if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE && bAddToVmp)
	{        
		if ( IsChangeVmpParam(&tLocalNewSpeaker) )
		{
			if (m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_SPEAKER)	//不是发言人跟随，不需全局调整通道
				|| IsDynamicVmp()														//自动画面合成情况，全局调整通道
				|| ( m_tConf.m_tStatus.GetVmpParam().GetVMPSchemeId() != 0 )			//对于配置了VMP方案的也要全局调整，以刷新memstatus.(eg：发言人要改边框色)
				) 
			{
				//若某通道内有新发言人或旧发言人，则立即changeParam; 否则定时再去尝试
				if( (!tLocalNewSpeaker.IsNull() && m_tConf.m_tStatus.GetVmpParam().IsMtInMember( tLocalNewSpeaker )) //FIXME：用localspeaker是否合理？
					|| (!m_tLastSpeaker.IsNull() && m_tConf.m_tStatus.GetVmpParam().IsMtInMember( m_tLastSpeaker ) )
					)
				{
					ChangeVmpChannelParam(&tLocalNewSpeaker, VMP_MEMBERTYPE_SPEAKER, &m_tLastSpeaker );
				}
				else
				{
					SetTimer(MCUVC_CHANGE_VMPCHAN_SPEAKER_TIMER, TIMESPACE_CHANGEVMPPARAM, VMP_MEMBERTYPE_SPEAKER);
				}
			}
			else
			{
				if( m_tConf.m_tStatus.GetVmpParam().IsMtInMember( tLocalNewSpeaker ) )
				{
					TPeriEqpStatus tPeriEqpStatus; 
					g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

					TVMPParam	tVMPParam   = m_tConf.m_tStatus.GetVmpParam();   

					if(byVmpSubType != VMP)//用的VMP是MPU
					{
						
						// xliang [4/21/2009] 调整分辨率，让new speaker 进VMP前适配通道
						u8	byVmpStyle  = tVMPParam.GetVMPStyle();
						u8	byChlPos	= tVMPParam.GetChlOfMtInMember(tLocalNewSpeaker);

						if( !m_tConf.m_tStatus.IsBrdstVMP() )
						{
							// xliang [3/27/2009] 把之前被选看的成员交换都冲掉，这个冲掉在较后面体现，
							//所以这里先整理下前适配通道的信息：
							//先释放掉被选看Mt占的VMP前适配通道，随后让speaker去占前适配通道
							u8 byHdChnnlNum = MAXNUM_SVMP_HDCHNNL;
							if( byVmpSubType == MPU_DVMP || byVmpSubType == EVPU_DVMP)
							{	
								byHdChnnlNum = MAXNUM_DVMP_HDCHNNL;
							}
							//u8 byHdChnnlNum = m_tVmpChnnlInfo.m_byHDChnnlNum;
							TChnnlMemberInfo tChnnlMemInfo;
							u8 byLoop;
							for(byLoop = 0; byLoop < byHdChnnlNum; byLoop ++)
							{
								m_tVmpChnnlInfo.GetHdChnnlInfo(byLoop,&tChnnlMemInfo);
								if( tChnnlMemInfo.IsAttrSelected() )
								{
									// xliang [4/8/2009] 【被冲特例】被选看的MT是被新发言人选看，
									//                    这种情况该被选看MT的VIP身份
									//                    将获得保留，其交换不会被冲
									TMtStatus tMtStatus;
									u8 byMode = MODE_VIDEO;
									m_ptMtTable->GetMtStatus(tLocalNewSpeaker.GetMtId(),&tMtStatus);
									if (tChnnlMemInfo.GetMt() == tMtStatus.GetSelectMt( byMode ))
									{
										continue;
									}
									m_tVmpChnnlInfo.ClearOneChnnl(tChnnlMemInfo.GetMt()); 
									//调Mt分辨率的动作置后
									//u8 byPos = tVMPParam.GetChlOfMtInMember(tChnnlMemInfo.GetMt());
									//ChangeMtVideoFormat(tChnnlMemInfo.GetMtId(),byVmpSubType,byVmpStyle,byPos);
								}
							}
							//发言人尝试占前VMP适配通道
							ChangeMtVideoFormat(tLocalNewSpeaker, &tVMPParam);
						}

					}
					else	//老VMP直接恢复分辨率
					{
						ChangeMtVideoFormat(tLocalNewSpeaker, &tVMPParam, FALSE);	
					}
				}
			}
		}
			

		
		/*if (IsChangeVmpParam(&tLocalNewSpeaker) && 
			// zbq [05/31/2007] 若所有VMP通道都不是发言人跟随，不需要调整通道风格 
			(m_tConf.m_tStatus.GetVmpParam().IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) ||
			m_tConf.m_tStatus.GetVmpParam().IsMtInMember( tLocalNewSpeaker ) || 
			m_tConf.m_tStatus.GetVmpParam().IsMtInMember( m_tLastSpeaker ) )
			)
		{
			ChangeVmpChannelParam(&tLocalNewSpeaker, VMP_MEMBERTYPE_SPEAKER, &m_tLastSpeaker );
		}
		else
		{
			SetTimer(MCUVC_CHANGE_VMPCHAN_SPEAKER_TIMER, TIMESPACE_CHANGEVMPPARAM, VMP_MEMBERTYPE_SPEAKER);
		}
		*/
		// xsl [8/28/2006] 恢复画面合成分辨率
// 		if ( m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tLocalNewSpeaker) )
// 		{
// 			TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
// 			ChangeMtVideoFormat(tLocalNewSpeaker.GetMtId(), &tVmpParam, FALSE);
// 		}
	}
		
	//加入多画面电视墙
	if (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE && bAddToVmp)
	{
		ChangeVmpTwChannelParam(&tLocalNewSpeaker, VMPTW_MEMBERTYPE_SPEAKER);
		
		// xsl [8/28/2006] 恢复画面合成分辨率
		if ( m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tLocalNewSpeaker) )
		{
			TVMPParam tVmpTwParam = m_tConf.m_tStatus.GetVmpTwParam();
			ChangeMtVideoFormat(tLocalNewSpeaker, &tVmpTwParam, FALSE);
		}
	}
	
	//强制加入混音
	if( !tNewSpeaker.IsNull() && m_tConf.m_tStatus.IsMixing() )
	{
		AddMixMember( &tLocalNewSpeaker, DEFAULT_MIXER_VOLUME, TRUE );
	}
	
	//　TvWall　发言人跟随时，同步调整电视墙中的图像
	TPeriEqpStatus tTWStatus;
	u8 byChnlIdx;
	u8 byEqpId;
	for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
	{
		if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
		{
			if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus))
			{
				u8 byMemberType;
				u8 byMtConfIdx;
				for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
				{
					byMemberType = tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType;
					byMtConfIdx = tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx();
					if (TW_MEMBERTYPE_SPEAKER == byMemberType && m_byConfIdx == byMtConfIdx)
					{
						ChangeTvWallSwitch(&tLocalNewSpeaker, byEqpId, byChnlIdx, TW_MEMBERTYPE_SPEAKER, TW_STATE_CHANGE);
					}
				}
			}
		}
	}

	//发言人跟随时，同步调整HDU中的图像
	TPeriEqpStatus tHduStatus;
	u8 byHduChnlIdx;
	u8 byHduEqpId;
	for (byHduEqpId = HDUID_MIN; byHduEqpId <= HDUID_MAX; byHduEqpId++)
	{
		if (EQP_TYPE_HDU == g_cMcuVcApp.GetEqpType(byHduEqpId))
		{
			if (g_cMcuVcApp.GetPeriEqpStatus(byHduEqpId, &tHduStatus))
			{
				u8 byMemberType;
				u8 byMtConfIdx;
				for (byHduChnlIdx = 0; byHduChnlIdx < MAXNUM_HDU_CHANNEL; byHduChnlIdx++)
				{
					byMemberType = tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].byMemberType;
					byMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetConfIdx();
					if (TW_MEMBERTYPE_SPEAKER == byMemberType && m_byConfIdx == byMtConfIdx && !tNewSpeaker.IsNull())
					{
						ChangeHduSwitch(&tLocalNewSpeaker, byHduEqpId, byHduChnlIdx, TW_MEMBERTYPE_SPEAKER, TW_STATE_CHANGE);
					}
				}
			}
		}
	}


	// 新发言人码流源调整逻辑, zgc, 2008-04-12
    if ( !IsSpeakerCanBrdVid() )
    {
        ChangeSpeakerSrc( MODE_VIDEO, emReasonChangeSpeaker );
    }
    if ( !IsSpeakerCanBrdAud() )
    {
        ChangeSpeakerSrc( MODE_AUDIO, emReasonChangeSpeaker );
    }

	return;
}

/*====================================================================
    函数名      ：ChangeSpeaker
    功能        ：改变发言人（包括单纯撤销）；
	              通知MC，MT
				  如果新、旧发言人直连，通知其自身改变状态
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt * ptNewSpeaker, 新发言人，NULL表示单纯撤销。这个TMt要求是真正的终端，不要GetLocal过
				  BOOL32 bPolling, 是否是Polling导致的发言人改变                  
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/04    1.0         LI Yi         创建
	03/11/25    3.0         胡昌威        修改
	05/1/27     3.6         Jason        修改
====================================================================*/
void CMcuVcInst::ChangeSpeaker( TMt * ptNewSpeaker, BOOL32 bPolling, BOOL32 bAddToVmp )
{
	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );

    TMt tSpeakerMt;
    tSpeakerMt.SetNull();

	// 对于VCS会议均不采取自动将发言人加入画面合成
	if (VCS_CONF == m_tConf.GetConfSource())
	{
		bAddToVmp = FALSE;
	}

	if(ptNewSpeaker != NULL)
    {
        tSpeakerMt = *ptNewSpeaker;
    }
	
	TMt  tOldSpeaker = m_tConf.GetSpeaker();

    ConfLog( FALSE, "[ChangeSpeaker]Old speaker(mcuid:%d, mtid:%d) was cancelled and new speaker(mcuid:%d, mtid:%d) is specified!\n",
		            tOldSpeaker.GetMcuId(), tOldSpeaker.GetMtId(), tSpeakerMt.GetMcuId(), tSpeakerMt.GetMtId());

	//在带音频的轮询广播时，如有另一终端发言，轮询停止
	if( !bPolling )
	{
		if( m_tConf.m_tStatus.GetPollMedia() == MODE_BOTH)
		{
			m_tConf.m_tStatus.SetPollMode( CONF_POLLMODE_NONE );
			m_tConf.m_tStatus.SetPollState( POLL_STATE_NONE );
			cServMsg.SetMsgBody( (u8*)m_tConf.m_tStatus.GetPollInfo(), sizeof(TPollInfo) );
			SendMsgToAllMcs( MCU_MCS_POLLSTATE_NOTIF, cServMsg );
			KillTimer( MCUVC_POLLING_CHANGE_TIMER );

			//取消轮询，则取消轮询交换及相应复合状态
			CancelOneVmpFollowSwitch( VMP_MEMBERTYPE_POLL, TRUE );
		}
	}

	//取消原发言人 // xsl [8/4/2006] 过滤新发言人与老发言人相同的情况
	if( m_tConf.HasSpeaker() && (NULL == ptNewSpeaker || (NULL != ptNewSpeaker && !(tOldSpeaker == *ptNewSpeaker))) )
	{
		BOOL32 bHasNewSpeaker = ( NULL != ptNewSpeaker ) ? TRUE : FALSE;
		AdjustOldSpeakerSwitch( tOldSpeaker, bHasNewSpeaker );

        //  xsl [6/20/2006] 取消发言人或替换发言人时，若双流为发言人开启，则停止双流
        if ( ( NULL == ptNewSpeaker || ( TYPE_MCUPERI != ptNewSpeaker->GetType()
				                         //zbq [09/27/2007] 发言人是录像机，也认为停双流
                                        || ( TYPE_MCUPERI == ptNewSpeaker->GetType() && EQP_TYPE_RECORDER == ptNewSpeaker->GetEqpType() ) ) )
             // zbq [08/26/2007] 发言人不一定是 m_tVidBrdSrc，比如在VMP广播时的发言人
			 && TYPE_MCUPERI != tOldSpeaker.GetType() /*m_tVidBrdSrc.GetType()*/)
        {
            if (CONF_DUALMODE_SPEAKERONLY == m_tConf.GetConfAttrb().GetDualMode()
				&& !m_tDoubleStreamSrc.IsNull() 
                // zbq [08/26/2007] 发言人不一定是 m_tVidBrdSrc，比如在VMP广播时的发言人
                // zbq [08/31/2007] 发言人要取本地发言人
                && m_tDoubleStreamSrc == GetLocalMtFromOtherMcuMt(tOldSpeaker) /*m_tVidBrdSrc*/)
            {
                StopDoubleStream(TRUE, TRUE);
            }
        }               
	}
	else
	{
//		m_tLastSpeaker.SetNull();
	}

	AdjustNewSpeakerSwitch( tSpeakerMt, bAddToVmp );

	//通知所有会控
	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
	SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
	
	//通知所有终端
	//简化通知终端使用的会议信息结构 2005-10-11
	TConfStatus tConfStatus = m_tConf.GetStatus();
	TSimConfInfo tSimConfInfo;
	tSimConfInfo.m_tSpeaker  = GetLocalSpeaker();
	tSimConfInfo.m_tChairMan = m_tConf.GetChairman();
    // guzh [11/6/2007] 
	tSimConfInfo.SetVACMode(tConfStatus.IsVACing());
	tSimConfInfo.SetVMPMode(tConfStatus.GetVMPMode());
    // guzh [11/6/2007] 
	tSimConfInfo.SetDiscussMode(tConfStatus.IsMixing());
	cServMsg.SetMsgBody( ( u8 * )&tSimConfInfo, sizeof( tSimConfInfo ) );
	BroadcastToAllSubMtJoinedConf( MCU_MT_SIMPLECONF_NOTIF, cServMsg );

    //n+1备份更新发言人信息
    if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
    {
        ProcNPlusSpeakerUpdate();
    }	

#ifdef _SATELITE_
    RefreshConfState();
	

	RefreshMtBitRate();
#endif

    return;
}

/*====================================================================
    函数名      ：IsChangeVmpParam
    功能        ：是否需要改变画面合成参数

    算法实现    ：
    引用全局变量：
    输入参数说明：

    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/06/15    3.6         libo          创建
    06/04/24    4.0         张宝卿        适应自动合成加入主席和发言人
====================================================================*/
BOOL32 CMcuVcInst::IsChangeVmpParam(TMt * ptMt)
{  
    if ( !m_tConfInStatus.IsVmpNotify() )
    {
        EqpLog( "[IsChangeVmpParam] m_bVmpNotify: %d, No change !\n", m_tConfInStatus.IsVmpNotify() );
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
函 数 名： GetSatCastChnnlNum
功    能：  获取当前分散会议回传通道数
算法实现： 
全局变量： 
参    数： void
返 回 值： u8  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/8/22  4.0			许世林                  创建
=============================================================================*/
u8  CMcuVcInst::GetSatCastChnnlNum(u8 bySrcMtId)
{
    u8 byChnnlNum = 0;
    for(u8 byIdx = 1; byIdx <= MAXNUM_CONF_MT; byIdx++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byIdx) && bySrcMtId != byIdx 
            && (m_ptMtTable->GetMtSndBitrate(byIdx) > 0 || m_ptMtTable->GetMtSndBitrate(byIdx, LOGCHL_SECVIDEO) > 0) )
        {
            byChnnlNum++;
        }
    }
    return byChnnlNum;
}

/*=============================================================================
  函 数 名： IsOverSatCastChnnlNum
  功    能： 卫星分散会议中是否超过最大回传通道数
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcInst::IsOverSatCastChnnlNum(u8 bySrcMtId, u8 byIncNum/* = 1*/)
{       
    return ((GetSatCastChnnlNum(bySrcMtId)+byIncNum) > m_tConf.GetSatDCastChnlNum());
}

/*=============================================================================
  函 数 名： IsOverSatCastChnnlNum
  功    能： 卫星分散会议中是否超过最大回传通道数
  算法实现： 
  全局变量： 
  参    数： TVMPParam tVmpParam
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcInst::IsOverSatCastChnnlNum(TVMPParam &tVmpParam)
{
    u8 byIncNum = 0;
    for (u8 byMemIdx = 0; byMemIdx < tVmpParam.GetMaxMemberNum(); byMemIdx++)
    {
        TMt *ptMt = (TMt *)tVmpParam.GetVmpMember(byMemIdx);
        if (m_tConfAllMtInfo.MtJoinedConf(ptMt->GetMtId()) &&
            m_ptMtTable->GetMtSndBitrate(ptMt->GetMtId()) == 0)
        {
            byIncNum++;
        }
    }

    return IsOverSatCastChnnlNum(0, byIncNum);
}

/*=============================================================================
  函 数 名： ChangeSatDConfMtRcvAddr
  功    能： 改变分散会议中终端接收地址
  算法实现： 
  全局变量： 
  参    数： BOOL32 bMulti 若为多播则为创会时设定的多播地址，单播地址为0
  返 回 值： TTransportAddr 
=============================================================================*/
void CMcuVcInst::ChangeSatDConfMtRcvAddr(u8 byMtId, u8 byChnnlType, BOOL32 bMulti/*= TRUE*/)
{
    TTransportAddr tMtRcvAddr;   
    //多播
    if (bMulti)
    {
        tMtRcvAddr.SetIpAddr(m_tConf.GetConfAttrb().GetSatDCastIp());
        
        u16 wPort = m_tConf.GetConfAttrb().GetSatDCastPort();
        if (LOGCHL_VIDEO == byChnnlType)
        {
            tMtRcvAddr.SetPort(wPort);
        }        
        else if (LOGCHL_AUDIO == byChnnlType)
        {
            tMtRcvAddr.SetPort(wPort+2);
        }
        else if (LOGCHL_SECVIDEO == byChnnlType)
        {
            tMtRcvAddr.SetPort(wPort+4);
        }
        else
        {
            ConfLog(FALSE, "[ChangeSatDConfMtRcvAddr] invalid channel type %d!\n", byChnnlType);
        }
    }
    //单播
    else
    {
        TLogicalChannel tLogicChannel;
        m_ptMtTable->GetMtLogicChnnl(byMtId, byChnnlType, &tLogicChannel, TRUE);
        tMtRcvAddr.SetIpAddr(0);
        tMtRcvAddr.SetPort(tLogicChannel.GetRcvMediaChannel().GetPort());        
    }

    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_MT_SATDCONFCHGADDR_CMD);
    cServMsg.SetMsgBody(&byChnnlType, sizeof(byChnnlType));
    cServMsg.CatMsgBody((u8*)&tMtRcvAddr, sizeof(tMtRcvAddr));
    SendMsgToMt(byMtId, MCU_MT_SATDCONFCHGADDR_CMD, cServMsg);

    Mt2Log("[ChangeSatDConfMtRcvAddr] byMtId.%d byChnnlType.%d MtRcvAddr<0x%x, %d>.\n", 
        byMtId, byChnnlType, tMtRcvAddr.GetIpAddr(), tMtRcvAddr.GetPort());

    return;
}

/*=============================================================================
函 数 名： IsMtSendingVidToOthers
功    能： 终端除了视频广播，是否还在向其他实体发送码流, 仅用于分散会议
算法实现： 
全局变量： 
参    数： TMt tMt
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/8/23  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMcuVcInst::IsMtSendingVidToOthers(TMt tMt, BOOL32 bConfPoll/*with audio*/, BOOL32 bTwPoll, u8 bySelDstMtId)
{	
    TMtStatus tStatus;
    TPeriEqpStatus tTWStatus;

    //是否在广播(带音频会议轮询时不判断)
    if (!bConfPoll)
    {
        if (tMt == GetLocalSpeaker())
        {
            return TRUE;
        }
    }      

    //是否在vmp中
    if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE)
    {
        if (m_tConf.m_tStatus.GetVmpParam().IsMtInMember(tMt))
        {
            return TRUE;
        }
    }

    //是否在tvwall中(电视墙轮询时不判断)
    if (!bTwPoll)
    {
        for(u8 byTvId = TVWALLID_MIN; byTvId <= TVWALLID_MAX; byTvId++)
        {      
            g_cMcuVcApp.GetPeriEqpStatus(byTvId, &tTWStatus);
            if( tTWStatus.m_byOnline )
            {
                for(u8 byLp = 0; byLp < tTWStatus.m_tStatus.tTvWall.byChnnlNum; byLp++)
                {
                    TMt tMtInTv = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[byLp];
                    if(tMtInTv.GetMtId() == tMt.GetMtId())
                    {
                        return TRUE;
                    }
                }
            }
        }
    }    

    //是否在mtw中
    if (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE)
    {
        if (m_tConf.m_tStatus.GetVmpTwParam().IsMtInMember(tMt))
        {
            return TRUE;
        }
    }

    //是否在会控监控
    if (IsMtInMcSrc(tMt.GetMtId(), MODE_VIDEO))
    {
        return TRUE;
    }

    //是否在终端录像
	m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tStatus);
	if (tStatus.m_tRecState.IsRecording())
	{
		return TRUE;
	}

    //是否在被选看
    for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (!m_tConfAllMtInfo.MtJoinedConf(byLoop) || byLoop == bySelDstMtId)//去掉此目的终端，看是否还有其他终端在选看
        {
            continue;
        }      
		
		if (m_ptMtTable->GetMtStatus(byLoop, &tStatus))
        {
            if (tStatus.GetSelectMt(MODE_VIDEO) == tMt)
            {               
                return TRUE;
            }
        }
    }

    //分散会议不支持级联，不判断回传

    return FALSE;
}

/*=============================================================================
函 数 名： IsMtInMcSrc
功    能：  终端是否在会控监控
算法实现： 
全局变量： 
参    数： u8 byMtId
           u8 byMode
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/8/23  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMcuVcInst::IsMtInMcSrc(u8 byMtId, u8 byMode)
{
    TLogicalChannel tLogicalChannel;
    TMt tMt;
    for( u8 byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( g_cMcuVcApp.IsMcConnected( byIndex ) )
		{
			u8 byChannelNum;
			if( g_cMcuVcApp.GetMcLogicChnnl( byIndex, byMode, &byChannelNum, &tLogicalChannel ) )
			{
				while( byChannelNum-- != 0 )
				{
					if( g_cMcuVcApp.GetMcSrc( byIndex, &tMt, byChannelNum, byMode ) )
					{
						if (tMt.GetMtId() == byMtId)
                        {
                            return TRUE;
                        }
					}
				}
			}
		}
	}

    return FALSE;
}

/*====================================================================
    函数名      ：RestoreAllSubMtJoinedConfWatchingSrcMt
    功能        ：1. 与会终端中谁的视(音)频源为tSrc的, 若会议有发言人，
                     则收看(听)发言人，无发言人，停止向其的交换。
                  2. 未收视频源的不要停。
                  3. 在停止了向某与会终端的交换后，检查最后选看终端是否为空，
                     若不为空，则再将选看终端的视频源交换到该与会终端。
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt & tSrc, 指定终端 
				  u8 byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/26    1.0         JQL           创建
    06/04/14    4.0         张宝卿        停tSrc的交换后，判断恢复选看交换
====================================================================*/
BOOL32 CMcuVcInst::RestoreAllSubMtJoinedConfWatchingSrcMt(TMt tSrc, u8 byMode )
{
	TMt tCurSrc;
	u8  byMtLoop;
	TMtStatus tStat;

	tSrc = GetLocalMtFromOtherMcuMt(tSrc);
	for( byMtLoop = 1; byMtLoop <= MAXNUM_CONF_MT; byMtLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byMtLoop ) )
		{
			m_ptMtTable->GetMtStatus(byMtLoop, &tStat);
		    if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
			{				
				if (tStat.GetSelectMt(MODE_VIDEO) == tSrc)
				{
					StopSelectSrc(m_ptMtTable->GetMt(byMtLoop), MODE_VIDEO, FALSE);
				}

			    m_ptMtTable->GetMtSrc( byMtLoop, &tCurSrc, MODE_VIDEO ); 
			    if( tCurSrc == tSrc )
				{
					if(tSrc == m_tVmpEqp && m_tConf.m_tStatus.IsVmpSeeByChairman()
						&& m_tConf.GetChairman().GetMtId() == byMtLoop )
					{
						// xliang [4/2/2009] VMP 继续将码流给该MT，所以此处不停交换
					}
					else
					{
						//非KEDA厂商看自己，KEDA停交换
						RestoreRcvMediaBrdSrc( byMtLoop, MODE_VIDEO, FALSE );
					}
                    //判断恢复选看
//                    RestoreMtSelectStatus( byMtLoop, MODE_VIDEO );
				}
			}

            if( byMode == MODE_AUDIO || byMode == MODE_BOTH ) 
			{
				if (tStat.GetSelectMt(MODE_AUDIO) == tSrc)
				{
					StopSelectSrc(m_ptMtTable->GetMt(byMtLoop), MODE_AUDIO, FALSE);
				}
				m_ptMtTable->GetMtSrc( byMtLoop, &tCurSrc, MODE_AUDIO ); 
			    if( tCurSrc == tSrc/* || ( tSrc.GetMtId() == byMtLoop && tSrc.GetType() == TYPE_MT )*/ )
				{
                    //非KEDA厂商看自己，KEDA停交换
					RestoreRcvMediaBrdSrc( byMtLoop, MODE_AUDIO, FALSE );

                    //判断恢复选看
//                    RestoreMtSelectStatus( byMtLoop, MODE_AUDIO );
				}			
			}
		}
	}

	MtStatusChange();

	return TRUE;
}

/*====================================================================
    函数名      ：RestoreRcvMediaBrdSrc
    功能        ：指定恢复接收广播媒体源
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt & tMt, 指定终端
				  u8 byMode, 恢复模式，缺省为MODE_BOTH
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/23    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::RestoreRcvMediaBrdSrc( u8 byMtId, u8 byMode, BOOL32 bMsgMtStutas )
{
	CServMsg cServMsg;
	u8		 bySrcChnnl;
	TMt      tMt = m_ptMtTable->GetMt( byMtId );

	cServMsg.SetConfId( m_tConf.GetConfId() );

	// xliang [3/31/2009] 对于新VMP广播，恢复交换走普通交换
	BOOL32 bHdVmpBrdst = FALSE;
	if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE	//断链保护
		&& m_tConf.m_tStatus.IsBrdstVMP())
	{
		m_tVidBrdSrc = m_tVmpEqp;	// xliang [7/28/2009] 因为时序问题，这里务必赋下值
		TPeriEqpStatus tPeriEqpStatus; 
		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
		u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
		if(byVmpSubType != VMP)
		{
			bHdVmpBrdst = TRUE;
		}
	}

	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
        // xsl [8/19/2006] 卫星会议接收组播码流
		if( !m_tVidBrdSrc.IsNull() && !m_tConf.GetConfAttrb().IsSatDCastMode() )
		{
            //非kdc厂商有广播源且节省带宽时，通知mt停止发送码流
            if(g_cMcuVcApp.IsSavingBandwidth() && MT_MANU_KDC != m_ptMtTable->GetManuId( tMt.GetMtId()))
            {
                NotifyOtherMtSend(byMtId, FALSE);
            }

			if(bHdVmpBrdst)
			{
				ConfLog(FALSE, "[RestoreRcvMediaBrdSrc] Call SwitchNewVmpToSingleMt here!\n");
				SwitchNewVmpToSingleMt(tMt);
				return;
			}

		    bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0; 
            // guzh [3/21/2007]
            //StartSwitchToSubMt( m_tVidBrdSrc, bySrcChnnl, byMtId, MODE_VIDEO, SWITCH_MODE_BROADCAST, bMsgMtStutas );
            StartSwitchFromBrd(m_tVidBrdSrc, bySrcChnnl, 1, &tMt);
		}
	    else
		{
			if( m_ptMtTable->GetManuId( tMt.GetMtId() ) == MT_MANU_KDC )
			{
				StopSwitchToSubMt( byMtId, MODE_VIDEO, SWITCH_MODE_BROADCAST, bMsgMtStutas );

                // xsl [8/19/2006] 卫星会议接收组播码流
                if (m_tConf.GetConfAttrb().IsSatDCastMode())
                {
                    ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_VIDEO);
                }
			}
			else
			{
                //非kdc厂商无发言人时回传自身视频, 节省带宽时，通知mt发送码流
                if(g_cMcuVcApp.IsSavingBandwidth())
                {
                    NotifyOtherMtSend(byMtId, TRUE);
                }

				StartSwitchToSubMt( tMt, 0, tMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, bMsgMtStutas );
			}
		}
	}

	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
        // xsl [8/19/2006] 卫星会议接收组播码流
        // guzh [4/23/2007] 直接取音频广播源
		if( !m_tAudBrdSrc.IsNull() && !m_tConf.GetConfAttrb().IsSatDCastMode() )
		{
			if( m_tConf.m_tStatus.IsNoMixing()  )
			{
				bySrcChnnl = (m_tConf.GetSpeaker() == m_tPlayEqp) ? m_byPlayChnnl : 0; 
				StartSwitchToSubMt( GetLocalSpeaker(), bySrcChnnl, byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgMtStutas );
			}
		}
	    else
		{
			if( !m_tCascadeMMCU.IsNull() && byMtId == m_tCascadeMMCU.GetMtId() )
			{
				// 如果是上级级联的终端，则不破坏此回传通道交换
				// StartSwitchToSubMt( tMt, 0, tMt.GetMtId(), MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgMtStutas );
			}
			else
			{
				StopSwitchToSubMt( byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgMtStutas );

                // xsl [8/19/2006] 卫星会议接收组播码流
                if (m_tConf.GetConfAttrb().IsSatDCastMode())
                {
                    ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_AUDIO);
                }
			}
		}
	}
}

/*====================================================================
    函数名      ：ConfStatusChange
    功能        ：改变会议状态
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/23    1.0         LI Yi         创建
	04/03/07    3.0         胡昌威        胡昌威
====================================================================*/
void CMcuVcInst::ConfStatusChange( )
{
	CServMsg	cServMsg;

	//notify all meeting consoles
	cServMsg.SetMsgBody( ( u8 * )&( m_tConf.m_tStatus ), sizeof( TConfStatus ) );
	SendMsgToAllMcs( MCU_MCS_CONFSTATUS_NOTIF, cServMsg );	
	
	SendConfInfoToChairMt();
}

/*=============================================================================
  函 数 名： ConfModeChange
  功    能： 会议模式通知
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::ConfModeChange()
{
    CServMsg	cServMsg;    
    cServMsg.SetMsgBody( ( u8 * )&( m_tConf.m_tStatus.m_tConfMode ), sizeof( TConfMode ) );

    SendMsgToAllMcs( MCU_MCS_CONFMODE_NOTIF, cServMsg );	

    SendConfInfoToChairMt();
}

/*=============================================================================
  函 数 名： SendConfInfoToChair
  功    能： 发送会议信息给主席终端
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::SendConfInfoToChairMt(void)
{
    //发会议通知
    if( HasJoinedChairman() )
    {
        TMt tMt = m_tConf.GetChairman();
        TMt tSpeaker = m_tConf.GetSpeaker();
        m_tConf.SetSpeaker( GetLocalSpeaker() );
        CServMsg	cServMsg;    
        cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
        SendMsgToMt( tMt.GetMtId(), MCU_MT_CONF_NOTIF, cServMsg );
        m_tConf.SetSpeaker( tSpeaker );
    }
}

/*====================================================================
    函数名      ：MtStatusChange
    功能        ：改变终端状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8     byMtId  : 具体的状态改变终端ID, 0表示所有的终端.
                  BOOL32 bForcely: 是否马上上报。FALSE: 遵循上报最短周期约束，TRUE: 实时上报.
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/05/08    3.0         胡昌威        胡昌威
====================================================================*/
void CMcuVcInst::MtStatusChange( u8 byMtId, BOOL32 bForcely )
{
	CServMsg	cServMsg;
	TMtStatus   tMtStatus;

	u32 dwCurTick = OspTickGet();
	BOOL32 bHasStat = FALSE;

	u8 byLoop;
	
    if ( !bForcely )
    {
        if( dwCurTick - m_tRefreshParam.dwMcsLastTick >= m_tRefreshParam.GetMcsRefreshInterval() )
        {
            KillTimer( MCUVC_REFRESH_MCS_TIMER );
            m_tRefreshParam.dwMcsLastTick = dwCurTick;
            bHasStat = TRUE;
            
            cServMsg.SetMsgBody( );
            if ( 0 == byMtId )
            {
                for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
                {
                    if( m_tConfAllMtInfo.MtInConf( byLoop ) && 
                        m_ptMtTable->GetMtStatus( byLoop, &tMtStatus ) ) 
                    {
                        cServMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
                    }
                }                
            }
            else if ( byMtId <= MAXNUM_CONF_MT )
            {
                if( m_tConfAllMtInfo.MtInConf( byMtId ) && 
                    m_ptMtTable->GetMtStatus( byMtId, &tMtStatus ) ) 
                {
                    cServMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
                }
            }
            else
            {
                ConfLog( FALSE, "[MtStatusChange] unexpected MtId.%d\n", byMtId );
            }
            
            
            if ( 0 != cServMsg.GetMsgBodyLen() )
            {
                SendMsgToAllMcs( MCU_MCS_MTSTATUS_NOTIF, cServMsg );
            }
        }
        else
        {
            SetTimer( MCUVC_REFRESH_MCS_TIMER, m_tRefreshParam.dwMcsRefreshInterval*1000, byMtId );
        }
    }
    else
    {
        cServMsg.SetMsgBody( );
        if ( 0 == byMtId )
        {
            for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
            {
                if( m_tConfAllMtInfo.MtInConf( byLoop ) && 
                    m_ptMtTable->GetMtStatus( byLoop, &tMtStatus ) ) 
                {
                    cServMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
                }
            }                
        }
        else if ( byMtId <= MAXNUM_CONF_MT )
        {
            if( m_tConfAllMtInfo.MtInConf( byMtId ) && 
                m_ptMtTable->GetMtStatus( byMtId, &tMtStatus ) ) 
            {
                cServMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
            }
        }
        else
        {
            ConfLog( FALSE, "[MtStatusChange] unexpected MtId.%d\n", byMtId );
        }
        
        
        if ( 0 != cServMsg.GetMsgBodyLen() )
        {
            SendMsgToAllMcs( MCU_MCS_MTSTATUS_NOTIF, cServMsg );
        }        
    }

    return;
}

/*====================================================================
    函数名      : ProcTimerRefreshMmcuMcs
    功能        处理刷新会控时钟
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/11/04    3.5         胡昌威           创建
====================================================================*/
void CMcuVcInst::ProcTimerRefreshMcs(  const CMessage * pcMsg  )
{
	CServMsg	cServMsg;
	TMtStatus   tMtStatus;
	u8 byLoop;
    u8 byMtId = *(u8*)pcMsg->content;

	switch( CurState() )
	{
	case STATE_ONGOING:

		m_tRefreshParam.dwMcsLastTick = OspTickGet();

        if ( 0 == byMtId )
        {
            cServMsg.SetMsgBody( );
            for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
            {
                if( m_tConfAllMtInfo.MtInConf( byLoop ) && 
                    m_ptMtTable->GetMtStatus( byLoop, &tMtStatus ) ) 
                {
                    cServMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
                }
            }            
        }
        else if ( byMtId <= MAXNUM_CONF_MT )
        {
            if( m_tConfAllMtInfo.MtInConf( byMtId ) && 
                m_ptMtTable->GetMtStatus( byMtId, &tMtStatus ) ) 
            {
                cServMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
            }
        }
        else
        {
            ConfLog( FALSE, "[ProcTimerRefreshMcs] unexpected MtId.%d\n", byMtId );
        }

		SendMsgToAllMcs( MCU_MCS_MTSTATUS_NOTIF, cServMsg );
	
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message TIMER_REFRESH_MCS received in state %u!\n",CurState() );
		break;
	}
	
}

/*====================================================================
    函数名      : SendMcuMediaSrcNotify
    功能        : 发送MCU数据源通知
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bFource = FALSE TRUE-不论是否变化,强制通知
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/11/04    3.5         胡昌威           创建
====================================================================*/
void CMcuVcInst::SendMcuMediaSrcNotify(BOOL32 bFource)
{
	CServMsg cServMsg;

	TConfMcInfo * ptMcInfo = m_ptConfOtherMcTable->m_atConfOtherMcInfo;

	for (s32 nLoop = 0; nLoop < MAXNUM_SUB_MCU; nLoop++)
	{
		u8 byMcuId = ptMcInfo[nLoop].m_byMcuId;
		if (0 == byMcuId)
		{
			continue;
		}

		TMt tMt = GetMcuMediaSrc(byMcuId);

		//不进行强制通知，没有变化，则不再重复通知
		if (ptMcInfo[nLoop].m_tSrcMt.GetMcuId() == tMt.GetMcuId() && 
			ptMcInfo[nLoop].m_tSrcMt.GetMtId() == tMt.GetMtId() && 
			FALSE == bFource)
		{
			continue;
		}

		//如果查询的是上级mcu并且媒体源为空，则设置媒体源即是该mcu
		if ((tMt.IsNull() || 0 == tMt.GetMtId()) && 
			!m_tCascadeMMCU.IsNull() && 
			byMcuId == m_tCascadeMMCU.GetMtId())
		{
			tMt.SetMcuId(LOCAL_MCUID);
			tMt.SetMtId(byMcuId);
		}

		MMcuLog("[ProcTimerMcuSrcCheck] McuId.%d - MtMcuId.%d MtId.%d MtType.%d MtConfIdx.%d\n", 
				byMcuId, tMt.GetMcuId(), tMt.GetMtId(), tMt.GetMtType(), tMt.GetConfIdx());

        // guzh [4/8/2007] 设置新的媒体源
		ptMcInfo[nLoop].m_tSrcMt = tMt;
		TMcu tMcu;
		tMcu.SetMcuId(byMcuId);
		cServMsg.SetEventId(MCU_MCS_MCUMEDIASRC_NOTIF);
		cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
		cServMsg.CatMsgBody((u8 *)&tMt, sizeof(tMt));
		SendMsgToAllMcs(MCU_MCS_MCUMEDIASRC_NOTIF, cServMsg);
	}

	return;
}

/*====================================================================
    函数名      : ProcTimerMcuSrcCheck
    功能        ：mcu源检查时钟
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/11/04    3.5         胡昌威           创建
====================================================================*/
void CMcuVcInst::ProcTimerMcuSrcCheck(const CMessage * pcMsg)
{
	STATECHECK;

	SendMcuMediaSrcNotify(FALSE);

	SetTimer(MCUVC_MCUSRC_CHECK_TIMER, TIMESPACE_MCUSRC_CHECK);

	return;	
}

/*====================================================================
    函数名      ProcTimerChangeVmpParam
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/06/14    3.6         LIBO          创建
    06/04/25    4.0         张宝卿        适应自动合成主席和发言人的加入
====================================================================*/
void CMcuVcInst::ProcTimerChangeVmpParam(const CMessage * pcMsg)
{
    u32 dwVmpMemberType = *(u32 *)pcMsg->content;

    if (m_tConfInStatus.IsVmpNotify())
    {
        if (MCUVC_CHANGE_VMPPARAM_TIMER == pcMsg->event)
        {
            TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpParam();
            //ChangeVmpParam(&tVMPParam);

			// xliang [1/6/2009] 区分新老VMP设VMP param
			AdjustVmpParam(&tVMPParam);
        }
        else
        {
            TMt tMt;
            tMt.SetNull();
            if (VMP_MEMBERTYPE_SPEAKER == dwVmpMemberType)
            {
                tMt = GetLocalSpeaker(); //FIXME:取Local的是否合理
                EqpLog("\n[ProcTimerChangeVmpChanParam] - VMP_MEMBERTYPE_SPEAKER - mtid:%d\n\n", tMt.GetMtId());
            }
            else if (VMP_MEMBERTYPE_CHAIRMAN == dwVmpMemberType)
            {
                tMt = m_tConf.GetChairman();
                EqpLog("\n[ProcTimerChangeVmpChanParam] - VMP_MEMBERTYPE_CHAIRMAN - mtid:%d\n\n", tMt.GetMtId());
            }

            if (tMt.IsNull())
            {                
                return;
            }

            ChangeVmpChannelParam(&tMt, (u8)dwVmpMemberType);          
        }
    }
    else
    {
        SetTimer(pcMsg->event, TIMESPACE_CHANGEVMPPARAM, dwVmpMemberType);
    }
}

/*====================================================================
    函数名      ：MtOnlineChange
    功能        ：终端在线状态变化
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/08/12    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::MtOnlineChange( TMt tMt, BOOL32 bOnline, u8 byReason )
{
	u8 byOnline = bOnline;
	CServMsg	cServMsg;
    cServMsg.SetMsgBody( (u8*)&tMt, sizeof( TMt ) );
	cServMsg.CatMsgBody( (u8*)&byOnline, sizeof( u8 ) );
	cServMsg.CatMsgBody( (u8*)&byReason, sizeof( u8 ) );
	SendMsgToAllMcs( MCU_MCS_MTONLINECHANGE_NOTIF, cServMsg );

	// 对于VCS会议，还需要通告当前VCS会议的特殊状态信息
	if (VCS_CONF == m_tConf.GetConfSource())
	{
		TMt tCurVCMT  = m_cVCSConfStatus.GetCurVCMT();
		TMt tReqVCMT  = m_cVCSConfStatus.GetReqVCMT();
		TMt tChairMan = m_tConf.GetChairman();

		if (byOnline)
		{
			// 根据调度席当前的静哑音状态设置对应终端状态
			if (tChairMan == tMt)
			{
				VCSMTMute(tMt, m_cVCSConfStatus.IsLocMute(), VCS_AUDPROC_MUTE);
				VCSMTMute(tMt, m_cVCSConfStatus.IsLocSilence(), VCS_AUDPROC_SILENCE);
			}
			else
			{
				if (CONF_CALLMODE_NONE == VCSGetCallMode(tMt) || 
					MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()))
				{
					VCSMTMute(tMt, m_cVCSConfStatus.IsRemMute(), VCS_AUDPROC_MUTE);
					VCSMTMute(tMt, m_cVCSConfStatus.IsRemSilence(), VCS_AUDPROC_SILENCE);
				}
			}
		}

		TMt tNull;
		tNull.SetNull();
		if (tReqVCMT == tMt && byOnline && !tMt.IsLocal())
		{
			// 对于下级终端上线通告，认为已完成调度，而对于本级终端需要等到终端请求打开通道才认为调度完成
			KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
			ChgCurVCMT(tMt);
		}

		if (tCurVCMT == tMt && !byOnline)
		{
			// 对于当前调度的终端，在线－－>不在线
			m_cVCSConfStatus.SetCurVCMT(tNull);
			// 如果对于组呼发言人及组呼电视墙模式,自动更改本地终端作发言人
			u8 byMode = m_cVCSConfStatus.GetCurVCMode();
			if (VCS_GROUPSPEAK_MODE == byMode || VCS_GROUPTW_MODE == byMode)
			{
				TMt tChairMan = m_tConf.GetChairman();
				ChangeSpeaker(&tChairMan);
			}
		}


	    VCSConfStatusNotif();
	}
}

/*====================================================================
    函数名      ：SendReplyBack
    功能        ：将应答发给正确对象
    算法实现    ：根据消息头中的源MCU、MT和会话号决定方向
    引用全局变量：
    输入参数说明：CServMsg & cCmdReply, 应答消息
				  u16 wEvent, 应答消息号
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/09    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::SendReplyBack( CServMsg &cReplyMsg, u16 wEvent )
{
	if( IsMtCmdFromThisMcu( cReplyMsg ) )	   // 是连接本MCU的终端发的命令应答
	{
		 SendMsgToMt( cReplyMsg.GetSrcMtId(), wEvent, cReplyMsg ) ;
	}
	else if( IsMcCmdFromThisMcu( cReplyMsg ) ) // 是连接本MCU的会议控制台发的命令应答
	{
		 SendMsgToMcs( cReplyMsg.GetSrcSsnId(), wEvent, cReplyMsg );
	}

	return;
}

/*====================================================================
    函数名      ：IsMtCmdFromThisMcu
    功能        ：判断是否是本MCU终端发出命令的返回应答
    算法实现    ：
    引用全局变量：
    输入参数说明：const CServMsg & cCmdReply, 应答消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/08    1.0         LI Yi         创建
====================================================================*/
BOOL32 CMcuVcInst::IsMtCmdFromThisMcu(const CServMsg &cCmdReply) const
{
	if( cCmdReply.GetSrcMtId() !=0 )
		return( TRUE );
	else
		return( FALSE );
}

/*====================================================================
    函数名      ：IsMcCmdFromThisMcu
    功能        ：判断是否是本MCU会议控制台发出命令的返回应答
    算法实现    ：
    引用全局变量：
    输入参数说明：const CServMsg & cCmdReply, 应答消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/05    1.0         LI Yi         创建
====================================================================*/
BOOL32 CMcuVcInst::IsMcCmdFromThisMcu( const CServMsg & cCmdReply ) const
{
	if( cCmdReply.GetSrcMtId() == 0 && cCmdReply.GetSrcSsnId() != 0 )
		return( TRUE );
	else
		return( FALSE );
}

/*====================================================================
    函数名      ：ClearVcInst
    功能        ：清空实例 数据（m_ptMtTable、m_ptSwitchTable、m_ptConfOtherMcTable除外）
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/08    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ClearVcInst(void)
{
    m_tConf.Reset();

    memset(&m_tConfPollParam, 0, sizeof(m_tConfPollParam));
    memset(&m_tTvWallPollParam, 0, sizeof(m_tTvWallPollParam));
	memset( &m_tHduPollParam, 0x0, sizeof(m_tHduPollParam) );  // add by jlb
    memset(&m_tConfEqpModule, 0, sizeof(m_tConfEqpModule));
    memset(&m_tConfProtectInfo, 0, sizeof(m_tConfProtectInfo));

    memset(&m_tVmpEqp, 0, sizeof(m_tVmpEqp));
	//memset(&m_tLastVmpEqp, 0, sizeof(m_tLastVmpEqp));
	
    memset(&m_tMixEqp, 0, sizeof(m_tMixEqp));
    memset(&m_tAudBasEqp, 0, sizeof(m_tAudBasEqp));         //modify bas 2
    memset(&m_tVidBasEqp, 0, sizeof(m_tVidBasEqp));         //modify bas 2
    memset(&m_tBrBasEqp, 0, sizeof(m_tBrBasEqp));           //modify bas 2
    memset(&m_tCasdAudBasEqp, 0, sizeof(m_tCasdAudBasEqp)); //modify bas 2
    memset(&m_tCasdVidBasEqp, 0, sizeof(m_tCasdVidBasEqp)); //modify bas 2
    memset(&m_tRecEqp, 0, sizeof(m_tRecEqp));
    memset(&m_tPlayEqp, 0, sizeof(m_tPlayEqp));
    memset(&m_tPrsEqp, 0, sizeof(m_tPrsEqp));
    memset(m_abyCasChnCheckTimes, 0, sizeof(m_abyCasChnCheckTimes));
    memset(&m_tRefreshParam, 0, sizeof(m_tRefreshParam));
    memset(&m_abyMixMtId, 0, sizeof(m_abyMixMtId));

    m_tVidBrdSrc.SetNull();
    m_tAudBrdSrc.SetNull();
    m_tLastSpeaker.SetNull();
    m_tVacLastSpeaker.SetNull();
    m_tCascadeMMCU.SetNull();
    m_tConfAllMtInfo.m_tMMCU.SetNull();
	m_tConfAllMtInfo.RemoveAllJoinedMt();

    m_tLastSpyMt.SetNull();    
    memset(&m_tChargeSsnId, 0, sizeof(m_tChargeSsnId));
	memset(&m_tLastVmpParam, 0, sizeof(m_tLastVmpParam));
	memset(&m_tLastVmpTwParam, 0, sizeof(m_tLastVmpTwParam));
    memset(&m_tLastMixParam, 0, sizeof(m_tLastMixParam));
    m_tHduBatchPollInfo.SetNull();
    m_tHduPollSchemeInfo.SetNull();
	m_cVCSConfStatus.VCCDefaultStatus();
    
	m_tDoubleStreamSrc.SetNull();
    m_tH239TokenOwnerInfo.Clear();

    m_tConfInStatus.Reset();

    m_byMixGrpId        = 0;
    m_byRecChnnl        = EQP_CHANNO_INVALID;
    m_byPlayChnnl       = EQP_CHANNO_INVALID;
    m_byAudBasChnnl     = EQP_CHANNO_INVALID; //modify bas 2
    m_byVidBasChnnl     = EQP_CHANNO_INVALID; //modify bas 2
    m_byBrBasChnnl      = EQP_CHANNO_INVALID; //modify bas 2
    m_byCasdAudBasChnnl = EQP_CHANNO_INVALID;
    m_byCasdVidBasChnnl = EQP_CHANNO_INVALID;
    m_wVidBasBitrate    = 0;
    m_wBasBitrate       = 0;
	memset(m_awVMPBrdBitrate, 0, sizeof(m_awVMPBrdBitrate));

//    m_byRegGKDriId      = 0;
    m_byPrsChnnl        = EQP_CHANNO_INVALID;
    m_byPrsChnnl2       = EQP_CHANNO_INVALID;
    m_byPrsChnnlAud     = EQP_CHANNO_INVALID;
    m_byPrsChnnlAudBas  = EQP_CHANNO_INVALID;
    m_byPrsChnnlVidBas  = EQP_CHANNO_INVALID;
    m_byPrsChnnlBrBas   = EQP_CHANNO_INVALID;

	m_byPrsChnnlVmpOut1 = EQP_CHANNO_INVALID;
	m_byPrsChnnlVmpOut2	= EQP_CHANNO_INVALID;
	m_byPrsChnnlVmpOut3	= EQP_CHANNO_INVALID;
	m_byPrsChnnlVmpOut4	= EQP_CHANNO_INVALID;
	m_byPrsChnnlDsVidBas		= EQP_CHANNO_INVALID;

#ifdef _MINIMCU_
    m_byIsDoubleMediaConf = 0;
#endif

	// 增加对 向外设发送请求关键帧命令的保护, zgc, 2008-04-21
	m_dwVmpLastVCUTick = 0;				
	m_dwVmpTwLastVCUTick = 0;		
	m_dwVidBasChnnlLastVCUTick = 0;	
	m_dwAudBasChnnlLastVCUTick = 0;
	m_dwBrBasChnnlLastVCUTick = 0;
	m_dwCasdAudBasChnnlLastVCUTick = 0;
	m_dwCasdVidBasChnnlLastVCUTick = 0;

	m_wRestoreTimes      = 1;
    m_byConfIdx          = 0;
	m_byDcsIdx			 = 0;
    
    m_byCreateBy         = 0;
	m_byLastDsSrcMtId    = 0;
	m_byMtIdNotInvite    = 0;// xliang [12/26/2008] 
	// 会议发言人码流源的设定渠道, zgc, 2008-04-14
	m_bySpeakerAudSrcSpecType = SPEAKER_SRC_NOTSEL;
	m_bySpeakerVidSrcSpecType = SPEAKER_SRC_NOTSEL;

    m_cMtRcvGrp.Clear();
    m_cBasMgr.Clear();
	memset(&m_tLogicChnnl, 0, sizeof(m_tLogicChnnl));
	
	// xliang [12/18/2008] 清空vmp channel信息
	m_tVmpChnnlInfo.clear();
	//memset(m_abyMtVmpChnnl, 0, sizeof(m_abyMtVmpChnnl));
	memset(m_abyMtNeglectedByVmp, 0, sizeof(m_abyMtNeglectedByVmp));

	m_tLastVmpChnnlMemInfo.Init();
	memset(m_atVMPTmpMember,0,sizeof(m_atVMPTmpMember));
	m_bNewMemberSeize = 0;

	//m_tVmpCasMemInfo.Init();
	m_byVmpOperating = 0;
	
    KillTimer(MCUVC_SCHEDULED_CHECK_TIMER);
    KillTimer(MCUVC_ONGOING_CHECK_TIMER);
    KillTimer(MCUVC_INVITE_UNJOINEDMT_TIMER);
    KillTimer(MCUVC_MCUSRC_CHECK_TIMER);
	KillTimer(MCUVC_RECREATE_DATACONF_TIMER);
    KillTimer(MCUVC_CONFSTARTREC_TIMER);

    return;
}

/*====================================================================
    函数名      ：ConfLog
    功能        ：业务打印
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/08    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ConfLog( BOOL32 bFile, s8 * pszFmt, ... )
{
	s8 achPrintBuf[255];

    s32 nBufLen;
    va_list argptr;
	s32 nLen = sprintf( achPrintBuf, "[CONF.%d]:", m_byConfIdx );
    va_start( argptr, pszFmt );    
    nBufLen = vsprintf( achPrintBuf + nLen, pszFmt, argptr );   
    OspPrintf( nLen, bFile, achPrintBuf );
    va_end(argptr); 	
}

/*====================================================================
    函数名      ：GetPollParam
    功能        ：得到下一个符合被轮询条件的终端
    算法实现    ：
    引用全局变量：
    输入参数说明：[IN/OUT] u8 &byPollPos 当前/下一个轮询的终端
                  [IN/OUT] TPollInfo& tPollInfo  轮询的信息
    返回值说明  ：TMtPollParam *，如果找不到下一个，则返回 NULL
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         胡昌威         创建
	07/03/29	4.0			周广程			修改：增加判断是否是轮询的BOOL32参数
    07/04/04    4.0         顾振华         重写
====================================================================*/
TMtPollParam *CMcuVcInst::GetNextMtPolled(u8 &byPollPos, TPollInfo& tPollInfo)
{
    // guzh [4/5/2007] 首先检查用户是否指定了轮询位置
    if ( m_tConfPollParam.IsSpecPos() )
    {
        byPollPos = m_tConfPollParam.GetSpecPos();
        m_tConfPollParam.ClearSpecPos();
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
    BOOL32 bSendAudio = FALSE;
    BOOL32 bCasMcuSpeaker = FALSE;
    BOOL32 bVideoLose = FALSE;
    TMtPollParam tCurMtParam, *ptMtPollParam = NULL;
    do 
    {
        // 新一轮的轮询开始
        if (byPollPos >= m_tConfPollParam.GetPolledMtNum())
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
        
		ptMtPollParam = m_tConfPollParam.GetPollMtByIdx(byPollPos);
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
            // 顾振华@2006.4.14 增加音频的逻辑
            bSendAudio = tMtStatus.IsSendAudio();
            
            // guzh [7/27/2006] 本级肯定不是下级MCU发言人，一定要赋值
            bCasMcuSpeaker = FALSE;

            // zbq [03/09/2007] 跳过当前没有视频的终端
            bVideoLose = tMtStatus.IsVideoLose();
			// xliang [4/1/2009] tempararily modified
			//bVideoLose = FALSE;
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

					//zbq[06/30/2008] 级联终端状态本字段非实时上报，仅依赖于list请求时下级MCU的终端状态. 可能由于下级终端打开逻辑通道的响应慢而导致误判.
                    //bSendVideo = pMcMtStatus->IsSendVideo();
                    //bSendAudio = pMcMtStatus->IsSendAudio();
					bSendVideo = TRUE;
                    bSendAudio = TRUE;

                    bCasMcuSpeaker = ((m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO) &&
                                      GetLocalSpeaker().GetMtId() == tCurMtParam.GetMcuId());

                    // zbq [03/09/2007] 跳过当前没有视频的下级MCU终端
                    bVideoLose = pMcMtStatus->IsVideoLose();
                }
            }
        }

        if ( bJoined &&  
             !bCasMcuSpeaker &&
             !bVideoLose &&
             ( bSendVideo || 
               ( bSendAudio &&
                 m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_SPEAKER ) )
            )
        {
            break;
        }
        else
        {
            McsLog("[GetNextMtPolled] MT(%d,%d) skipped for Joined.%d, SendVideo.%d, Speaker.%d", 
                tCurMtParam.GetMcuId(), 
                tCurMtParam.GetMtId(),
                bJoined, 
                bSendVideo, 
                bCasMcuSpeaker);
        }

        byPollPos++;
        wFindMtNum++;

    // 直到找满一圈
    }while (wFindMtNum<m_tConfPollParam.GetPolledMtNum());

    if (wFindMtNum == m_tConfPollParam.GetPolledMtNum())
    {
        return NULL;
    }
     
    // 保证当前轮询信息的正确性
    tPollInfo.SetMtPollParam( tCurMtParam );
    TMtPollParam *ptNextMt = m_tConfPollParam.GetPollMtByIdx(byPollPos);    
    return ptNextMt;
}

/*=============================================================================
  函 数 名： GetMtTWPollParam
  功    能： 得到
  算法实现： 
  全局变量： 
  参    数： u8 &byPollPos
  返 回 值： TMtPollParam 
=============================================================================*/
TMtPollParam *CMcuVcInst::GetMtTWPollParam(u8 &byPollPos)
{
    u8 wFindMtNum = 0;
    TTvWallPollInfo * ptPollInfo = m_tConf.m_tStatus.GetTvWallPollInfo();    
    
    BOOL32 bJoined = FALSE;
    BOOL32 bSendVideo = FALSE;
    BOOL32 bMcuSpeaker = FALSE;
	TMtPollParam tCurMtParam, *ptMtPollParam = NULL;
    do 
    {
		// 从函数开头移到这里，对byPollPos进行保护，zgc, 2007-04-05
		if (byPollPos == m_tTvWallPollParam.GetPolledMtNum())
		{        
			u32 dwPollNum = ptPollInfo->GetPollNum();
			if (1 != dwPollNum)
			{
				if (0 != dwPollNum)
				{
					ptPollInfo->SetPollNum(dwPollNum - 1);
					m_tConf.m_tStatus.SetTvWallPollInfo(*ptPollInfo);
				}
				byPollPos = 0;
			}
			else
			{
				ptPollInfo->SetPollNum(0);
				m_tConf.m_tStatus.SetTvWallPollInfo(*ptPollInfo);
				return NULL;
			}
		}

        ptMtPollParam = m_tTvWallPollParam.GetPollMtByIdx(byPollPos);
        if ( NULL != ptMtPollParam )
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

            // guzh [7/27/2006] 本级不需要考虑这个问题，一定要赋值
            bMcuSpeaker = FALSE;
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

                    //zbq[2008/08/08] 级联终端状态本字段非实时上报，仅依赖于list请求时下级MCU的终端状态. 可能由于下级终端打开逻辑通道的响应慢而导致误判.
                    //bSendVideo = pMcMtStatus->IsSendVideo();
					bSendVideo = TRUE;

                    bMcuSpeaker = GetLocalSpeaker().GetMtId() == tCurMtParam.GetMcuId();
                    
                    if( bJoined && bSendVideo && !bMcuSpeaker )
                    {
                        OnMMcuSetIn((TMt&)tCurMtParam, 0, SWITCH_MODE_SELECT);
                    }
                }
            }
        }
        
        if (bJoined && bSendVideo && !bMcuSpeaker)
        {
            break;
        }
        else
        {
            EqpLog("[GetMtTWPollParam] MT(%d,%d) skipped for Joined.%d, SendVideo.%d, Speaker.%d", 
                tCurMtParam.GetMcuId(), 
                tCurMtParam.GetMtId(),
                bJoined, 
                bSendVideo, 
                bMcuSpeaker);
        }
        
        byPollPos++;

        wFindMtNum++;
        
    }while (wFindMtNum<m_tTvWallPollParam.GetPolledMtNum());
    
    if (wFindMtNum == m_tTvWallPollParam.GetPolledMtNum())
    {
        return NULL;
    }
    
    return m_tTvWallPollParam.GetPollMtByIdx(byPollPos);
}

/*=============================================================================
  函 数 名： GetMtHduPollParam
  功    能： 得到
  算法实现： 
  全局变量： 
  参    数： u8 &byPollPos
  返 回 值： TMtPollParam 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  09/03/10    4.6         江乐斌         创建
=============================================================================*/
TMtPollParam *CMcuVcInst::GetMtHduPollParam(u8 &byPollPos)
{
    u8 wFindMtNum = 0;
    THduPollInfo * ptPollInfo = m_tConf.m_tStatus.GetHduPollInfo();    
    
    BOOL32 bJoined = FALSE;
    BOOL32 bSendVideo = FALSE;
    BOOL32 bMcuSpeaker = FALSE;
	TMtPollParam tCurMtParam, *ptMtPollParam;
    do 
    {
		// xliang [7/27/2009] 对删Mt即退出轮询队列这一策略而言，
		//					  轮询到最末一个，此时删第1个Mt，会导致byPollPos > 轮询的Mt数
		//if (byPollPos == m_tHduPollParam.GetPolledMtNum())
		if (!(byPollPos < m_tHduPollParam.GetPolledMtNum()))
		{        
			u32 dwPollNum = ptPollInfo->GetPollNum();	//轮询次数
			if (1 != dwPollNum)
			{
				if (0 != dwPollNum)
				{
					ptPollInfo->SetPollNum(dwPollNum - 1);
					m_tConf.m_tStatus.SetHduPollInfo(*ptPollInfo);
				}
				byPollPos = 0;
			}
			else
			{
				ptPollInfo->SetPollNum(0);
				m_tConf.m_tStatus.SetHduPollInfo(*ptPollInfo);
				return NULL;
			}
		}

        ptMtPollParam = m_tHduPollParam.GetPollMtByIdx(byPollPos);
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

            // guzh [7/27/2006] 本级不需要考虑这个问题，一定要赋值
            bMcuSpeaker = FALSE;
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

                    //zbq[2008/08/08] 级联终端状态本字段非实时上报，仅依赖于list请求时下级MCU的终端状态. 可能由于下级终端打开逻辑通道的响应慢而导致误判.
                    //bSendVideo = pMcMtStatus->IsSendVideo();
					bSendVideo = TRUE;

                    bMcuSpeaker = GetLocalSpeaker().GetMtId() == tCurMtParam.GetMcuId();
                    
                    if( bJoined && bSendVideo && !bMcuSpeaker )
                    {
                        OnMMcuSetIn((TMt&)tCurMtParam, 0, SWITCH_MODE_SELECT);
                    }
                }
            }
        }
        
        if (bJoined && bSendVideo && !bMcuSpeaker)
        {
            break;
        }
        else
        {
            EqpLog("[GetMtTWPollParam] MT(%d,%d) skipped for Joined.%d, SendVideo.%d, Speaker.%d", 
                tCurMtParam.GetMcuId(), 
                tCurMtParam.GetMtId(),
                bJoined, 
                bSendVideo, 
                bMcuSpeaker);
        }
        
        byPollPos++;

        wFindMtNum++;
        
    }while (wFindMtNum<m_tHduPollParam.GetPolledMtNum());
    
    if (wFindMtNum == m_tHduPollParam.GetPolledMtNum())
    {
        return NULL;
    }
    
    return m_tHduPollParam.GetPollMtByIdx(byPollPos);
}

/*====================================================================
    函数名      ：VACChange
    功能        ：语音激励控制改变
    算法实现    ：
    引用全局变量：
    输入参数说明：TDiscussParam tDiscussParam 当前成员
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/21    1.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::VACChange(const TMixParam &tMixParam, u8 byExciteChn)
{
    TMt tMt;
    TMt tOldSpeaker;

    EqpLog("m_tConf.m_tStatus.GetMixerMode = %d\n",
           m_tConf.m_tStatus.GetMixerMode());

    if (byExciteChn == 0)
    {
        EqpLog( "voice active member num is: %d\n", byExciteChn);
        return;
    }

    tMt = m_ptMtTable->GetMt(byExciteChn);
    tOldSpeaker = GetLocalSpeaker();

    //控制发言人切换
    if (m_tConf.m_tStatus.IsVACing() && !(tMt == tOldSpeaker))
    {
        //已有时间切换间隔
        ChangeSpeaker(&tMt);
        m_tVacLastSpeaker = tMt;
        EqpLog("Vac speaker change and new speaker is: Mt%d\n", tMt.GetMtId());
    }
    
    return;
}

/*=============================================================================
  函 数 名： DaemonProcUnitTestMsg
  功    能： 单元测试消息处理, 包括
			 (1) 主控热备份
  算法实现： 
  全局变量： 
  参    数： CMessage *pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/2/15    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::DaemonProcUnitTestMsg( CMessage *pcMsg )
{
    /*
	switch( pcMsg->event )
	{
	case EV_TEST_TEMPLATEINFO_GET_REQ:
		ProcUnitTestGetTmpInfoReq(pcMsg);
		break;
	case EV_TEST_CONFINFO_GET_REQ:
		ProcUnitTestGetConfInfoReq(pcMsg);
		break;
	case EV_TEST_CONFMTLIST_GET_REQ:
		ProcUnitTestGetMtListInfoReq(pcMsg);
		break;
	case EV_TEST_ADDRBOOK_GET_REQ:
		ProcUnitTestGetAddrbookReq(pcMsg);
		break;
	case EV_TEST_MCUCONFIG_GET_REQ:
		ProcUnitTestGetMcuCfgReq(pcMsg);
		break;
	default:
		OspPrintf( TRUE, FALSE, "unexpected message %d<%s> received in DaemonProcUnitTestMsg !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
    */
}

/*=============================================================================
  函 数 名： ProcUnitTestGetTmpInfoReq
  功    能： 模拟会控请求模板信息
  算法实现： 
  全局变量： 
  参    数： CMessage *pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/2/15    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcUnitTestGetTmpInfoReq( CMessage *pcMsg )
{
    /*
	if ( g_bpUnitMsg )
	{
		OspPrintf( TRUE, FALSE, "[UnitTest]message %d<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
	}
	CConfId cConfId;
	u8		byConfIdx;

	//上报本板信息
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	cConfId = cServMsg.GetConfId();
	byConfIdx = g_cMcuVcApp.GetConfIdx( cConfId );

	// 1. 会议信息 TConfInfo
	if( byConfIdx != 0 )
	{
		TTemplateInfo tOldTemInfo;
		TConfInfo tConfInfo;
		if(!g_cMcuVcApp.GetTemplate(byConfIdx, tOldTemInfo))
		{
			ConfLog(FALSE, "[ProcUnitTestGetTmpInfoReq] get template confIdx<%d> failed\n", byConfIdx);
			cServMsg.SetEventId( pcMsg->event + 2 );
			OspPost( MAKEIID(AID_MCU_MCSSN, cServMsg.GetSrcSsnId()), cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			return;
        }
		g_cMcuVcApp.TemInfo2Msg( tOldTemInfo, cServMsg );
	}
	cServMsg.SetEventId( pcMsg->event + 1 );
	OspPost( MAKEIID(AID_MCU_MCSSN, cServMsg.GetSrcSsnId()), cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

  */
	return;
}

/*=============================================================================
  函 数 名： ProcUnitTestGetConfInfoReq
  功    能： 模拟会控请求会议信息
  算法实现： 
  全局变量： 
  参    数： CMessage *pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/2/15    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcUnitTestGetConfInfoReq( CMessage *pcMsg )
{
    /*
	if ( g_bpUnitMsg )
	{
		OspPrintf( TRUE, FALSE, "[UnitTest]message %d<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
	}
	CConfId cConfId;
	u8		byConfIdx;
	
	//上报本板信息
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	cConfId = cServMsg.GetConfId();
	byConfIdx = g_cMcuVcApp.GetConfIdx( cConfId );
	CMcuVcInst *pInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
	if ( NULL == pInst ) 
	{
		OspPrintf( TRUE, FALSE, "[UnitTest]get mcuvc inst handle failed !\n" );
		return;
	}
	
	// 1. 会议信息 TConfInfo
	if( byConfIdx != 0 )
	{
		TConfInfo tConfInfo;
		tConfInfo = pInst->m_tConf;
		
		cServMsg.SetEventId( pcMsg->event + 1 );
		cServMsg.SetMsgBody( (u8*)&tConfInfo, sizeof(TConfInfo) );
		OspPost( MAKEIID(AID_MCU_MCSSN, cServMsg.GetSrcSsnId()), cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	}
	*/
	return;
}

/*=============================================================================
  函 数 名： ProcUnitTestGetMtListInfoReq
  功    能： 模拟会控请求终端列表信息
  算法实现： 
  全局变量： 
  参    数： CMessage *pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/2/15    4.0			张宝卿                  创建
=============================================================================*/
#define MSUNITTEST_BUFLEN		(u16)(1024 * 20)
void CMcuVcInst::ProcUnitTestGetMtListInfoReq( CMessage *pcMsg )
{
    /*
	if ( g_bpUnitMsg )
	{
		OspPrintf( TRUE, FALSE, "[UnitTest]message %d<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
	}
	CConfId cConfId;
	u8		byConfIdx;
	
	//上报本板信息
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	cConfId = cServMsg.GetConfId();
	byConfIdx = g_cMcuVcApp.GetConfIdx( cConfId );

	// 1. 会议终端表 TConfMtTable
	TConfMtTable tConfMtTable;

	u8	byPktNum = sizeof(TConfMtTable) / MSUNITTEST_BUFLEN + 1;

	cServMsg.SetTotalPktNum( byPktNum );	//需要发送的总包数

	u8 *pMtTable = (u8*)&tConfMtTable;

	if ( g_cMcuVcApp.GetConfMtTable(byConfIdx, &tConfMtTable) )
	{		
		for( s32 nIndex = 0; nIndex < byPktNum; nIndex ++ )
		{
			cServMsg.SetCurPktIdx( nIndex );
			if ( nIndex < byPktNum - 1 )
			{
				cServMsg.SetMsgBody( pMtTable + nIndex * MSUNITTEST_BUFLEN, sizeof(u8) * MSUNITTEST_BUFLEN );
			}
			else
			{
				cServMsg.SetMsgBody( pMtTable + nIndex * MSUNITTEST_BUFLEN, 
										sizeof(tConfMtTable) - sizeof(u8) * MSUNITTEST_BUFLEN * nIndex );
			}
			cServMsg.SetEventId( pcMsg->event + 1 );
			OspPost( MAKEIID(AID_MCU_MCSSN, cServMsg.GetSrcSsnId()), cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			OspDelay(100);
		}
	}
	else
	{
		ConfLog(FALSE, "ProcUnitTestGetMtListInfoReq] get mt table failed !\n");
		return;
	}
    */
}
/*==============================================================================
函数名    :  DaemonProcHDIFullNtf
功能      :  处理因HDI接入能力满而导致MT呼叫MCU失败通知消息
算法实现  :  上报MCS
参数说明  :  CMessage *pcMsg
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2008-10-24					薛亮
==============================================================================*/
void CMcuVcInst::DaemonProcHDIFullNtf ( const CMessage *pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byHdiId = 0;
	byHdiId = cServMsg.GetSrcDriId();
	
	NotifyMcsAlarmInfo( 0, ERR_MCU_MTCALLFAIL_HDIFULL);
	McsLog("[DaemonProcHDIFullNtf]Send HDI FULL NOTIFY to All Mcs:HDI.%u is full\n",byHdiId);
}
/*=============================================================================
  函 数 名： ProcUnitTestGetMcuCfgReq
  功    能： 模拟会控请求MCU配置信息
  算法实现： 
  全局变量： 
  参    数： CMessage *pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/2/15    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcUnitTestGetMcuCfgReq( CMessage *pcMsg )
{
    /*
	if ( g_bpUnitMsg )
	{
		OspPrintf( TRUE, FALSE, "[UnitTest]message %d<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
	}
	
	u8 abyCfgBuf[MSUNITTEST_BUFLEN];
	u32 dwBufOut = 0;
	g_cMcuVcApp.GetCfgFileData( (u8*)&abyCfgBuf, sizeof(u8) * MSUNITTEST_BUFLEN, dwBufOut, TRUE );
	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	cServMsg.SetEventId( pcMsg->event + 1 );
	cServMsg.SetMsgBody( abyCfgBuf, sizeof(u8) * dwBufOut );
	
	// 上报本板配置信息
	OspPost( MAKEIID(AID_MCU_MCSSN, cServMsg.GetSrcSsnId()), cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
    */
}

/*=============================================================================
  函 数 名： ProcUnitTestGetAddrbookReq
  功    能： 模拟会控请求地址薄信息(这里还有完成地址薄上报，只是回了个消息)
  算法实现： 
  全局变量： 
  参    数： CMessage *pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/2/15    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcUnitTestGetAddrbookReq( CMessage *pcMsg )
{
    /*
	if ( g_bpUnitMsg )
	{
		OspPrintf( TRUE, FALSE, "[UnitTest]message %d<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
	}
	//上报本板地址薄信息
    */
}

/*====================================================================
    函数名      : IsMtNeedAdapt
    功能        : 终端是否需适配
    算法实现    :
    引用全局变量:
    输入参数说明: u8 byMtId 终端Id
				  u8 byAdaptType 适配类型
    返回值说明  :
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    04/02/14    3.0         胡昌威         创建
====================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::IsMtNeedAdapt(u8 byAdaptType, u8 byMtId, TMt *ptSrcMt)
{
    TSimCapSet tBrdSrcSCS;
    TSimCapSet tMtDstSCS;
    
    //保护一下
    if (!m_tConf.GetConfAttrb().IsUseAdapter())
    {
        return FALSE;
    }

    TMt tSrcMt;

    switch(byAdaptType)
    {
    case ADAPT_TYPE_AUD:
    case ADAPT_TYPE_CASDAUD:
        if (NULL == ptSrcMt ||
            (NULL != ptSrcMt && ptSrcMt->IsNull()))
        {
            //是否需要媒体类型适配
            if (TYPE_MT == m_tAudBrdSrc.GetType())
            {
                tSrcMt = m_tAudBrdSrc;
            }
        }
        else
        {
            tSrcMt = *ptSrcMt;
        }

        tMtDstSCS  = m_ptMtTable->GetDstSCS(byMtId);
        //支持rec放像适配
        if ( TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetEqpType() )
        {
            tBrdSrcSCS.SetAudioMediaType( m_tPlayEqpAttrib.GetAudioType() );
        }
        else
        {
            tBrdSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());     
        }  
        
        if (tBrdSrcSCS.GetAudioMediaType() != tMtDstSCS.GetAudioMediaType())
        {
            return TRUE;
        }
        break;

    case ADAPT_TYPE_VID:
    case ADAPT_TYPE_CASDVID:
        {
            if (NULL == ptSrcMt ||
                (NULL != ptSrcMt && ptSrcMt->IsNull()))
            {
                //是否需要媒体类型适配
                if (TYPE_MT == m_tVidBrdSrc.GetType())
                {
                    tSrcMt = m_tVidBrdSrc;
                }
            }
            else
            {
                tSrcMt = *ptSrcMt;
            }

            tMtDstSCS  = m_ptMtTable->GetDstSCS(byMtId);
            
            if ( TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetEqpType() )
            {
                tBrdSrcSCS.SetVideoMediaType(m_tPlayEqpAttrib.GetVideoType());
                tBrdSrcSCS.SetVideoResolution(tMtDstSCS.GetVideoResolution());
            }
            else
            {
                tBrdSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());
            }
                               
            if(MEDIA_TYPE_NULL != tBrdSrcSCS.GetVideoMediaType())
            {
                // zbq [08/20/2007] 适配器不支持H264的D1分辨率
                u8 bySrcType = tBrdSrcSCS.GetVideoMediaType();
                u8 byDstType = tMtDstSCS.GetVideoMediaType();
                u8 bySrcFormat = tBrdSrcSCS.GetVideoResolution();
                u8 byDstFormat = tMtDstSCS.GetVideoResolution();

                if ( bySrcType   != byDstType ||
                     bySrcFormat != byDstFormat )
                {
                    if ( (MEDIA_TYPE_H264 == bySrcType &&
                          VIDEO_FORMAT_4CIF == bySrcFormat)
                        ||
                         (MEDIA_TYPE_H264 == byDstType &&
                          VIDEO_FORMAT_4CIF == byDstFormat)
						//||
						//  ( MEDIA_TYPE_H264 == bySrcType && bySrcType == byDstType
						//  && bySrcFormat < byDstFormat )	
						  )
                    {
						ConfLog(FALSE, "[IsMtNeedAdapt] no need adapter! bySrcType: %u, bySrcFormat: %u, byDstType: %u, byDstFormat: %u\n",
							bySrcType, bySrcFormat, byDstType, byDstFormat);
                    }
                    else
                    {
                        return TRUE;
                    }
                }
                /*
                if ( (tBrdSrcSCS.GetVideoMediaType() != tMtDstSCS.GetVideoMediaType() )
                    || (tBrdSrcSCS.GetVideoResolution() != tMtDstSCS.GetVideoResolution() &&
                        tMtDstSCS.GetVideoResolution() != VIDEO_FORMAT_AUTO &&
                        MEDIA_TYPE_H264 != tMtDstSCS.GetVideoMediaType()) )//  [4/24/2006] 264格式没有分辨率
                {
                    return TRUE;
                }
                */
            }          
        }
        break;

    case ADAPT_TYPE_BR:
		{
			// 顾振华 [6/2/2006] 如果不是双速会议,永远不认为需要做码率适配
			if (m_tConf.GetSecBitRate() == 0)
			{
				return FALSE;
			}
        
            if (NULL == ptSrcMt ||
                (NULL != ptSrcMt && ptSrcMt->IsNull()))
			{
				//是否需要媒体类型适配
// 				if (TYPE_MT == m_tVidBrdSrc.GetType())
// 				{
					tSrcMt = m_tVidBrdSrc;
// 				}
			}
            else
            {
                tSrcMt = *ptSrcMt;
            }
            
            // xsl [8/25/2006] 源与目的相同，不进行适配，与判断是否启动会议码率适配的逻辑统一
            if (tSrcMt.GetMtId() == byMtId)
            {
                return FALSE;
            }

            // zbq [08/26/2007] 源端的码率应该取当前的发送码率
			// u16 wSrcBitrate = m_ptMtTable->GetDialBitrate(tSrcMt.GetMtId());
            u16 wSrcBitrate = m_ptMtTable->GetMtSndBitrate(tSrcMt.GetMtId());

            // zbq [08/20/2007] 码率适配启用同样要校验视频格式和分辨率
            tMtDstSCS  = m_ptMtTable->GetDstSCS(byMtId);

			// 录像机发送码率强制取会议第一码率
			if ( TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetEqpType() )
			{
				wSrcBitrate = m_tConf.GetBitRate();

                // zbq [08/20/2007] 码率适配启用同样要校验视频格式和分辨率
                tBrdSrcSCS.SetVideoMediaType(m_tPlayEqpAttrib.GetVideoType());
                tBrdSrcSCS.SetVideoResolution(tMtDstSCS.GetVideoResolution());
			}
            else
            {
                // zbq [08/20/2007] 码率适配启用同样要校验视频格式和分辨率
                tBrdSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());
            }

			//是否需要码率适配
			//判决 若发送源的发送码率 高于 接收端的接收码率 则需要码率适配   
			if (m_ptMtTable->GetMtReqBitrate(byMtId) > 0 && 
				(m_ptMtTable->GetMtReqBitrate(byMtId)*(g_cMcuVcApp.GetBitrateScale()+100)/100) < wSrcBitrate)
			{
                // zbq [08/20/2007] 适配器不支持H264的D1分辨率
                u8 bySrcType = tBrdSrcSCS.GetVideoMediaType();
                u8 byDstType = tMtDstSCS.GetVideoMediaType();
                u8 bySrcFormat = tBrdSrcSCS.GetVideoResolution();
                u8 byDstFormat = tMtDstSCS.GetVideoResolution();
                
                if ( (MEDIA_TYPE_H264 == bySrcType &&
                      VIDEO_FORMAT_4CIF == bySrcFormat)
                    ||
                     (MEDIA_TYPE_H264 == byDstType &&
                      VIDEO_FORMAT_4CIF == byDstFormat))
                {
                }
                else
                {
                    return TRUE;
                }
			}
		}
        break;

    default:
        break;
    }

    return FALSE;
}


/*=============================================================================
函 数 名： IsMtSrcBas
功    能： 终端源是否来自bas
算法实现： 
全局变量： 
参    数： u8 byMtId
u8 byMode
返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMcuVcInst::IsMtSrcBas(u8 byMtId, u8 byMode, u8 &byAdpType)
{
    byAdpType = ADAPT_TYPE_NONE;

    TMt tMt = m_ptMtTable->GetMt(byMtId);
    if(tMt.GetType() != TYPE_MT)
    {
        return FALSE;
    }
    
    TMt tMtSrc;
    if(!m_ptMtTable->GetMtSrc(byMtId, &tMtSrc, byMode))
    {
        return FALSE;
    }
    
    if(MODE_VIDEO == byMode)
    {
        // guzh [7/10/2007] 格式适配优先
        if(m_tConf.m_tStatus.IsVidAdapting() && IsMtNeedAdapt(ADAPT_TYPE_VID, byMtId, &tMtSrc))
        {
            byAdpType = ADAPT_TYPE_VID;
            return TRUE;
        }
        else if(m_tConf.m_tStatus.IsCasdVidAdapting() && IsMtNeedAdapt(ADAPT_TYPE_CASDVID, byMtId, &tMtSrc))
        {
            byAdpType = ADAPT_TYPE_CASDVID;
            return TRUE;
        }
        else if(m_tConf.m_tStatus.IsBrAdapting() && IsMtNeedAdapt(ADAPT_TYPE_BR, byMtId, &tMtSrc))
        {
            byAdpType = ADAPT_TYPE_BR;
            return TRUE;
        }
        else if (m_tConf.m_tStatus.IsHdVidAdapting() &&
			     m_cMtRcvGrp.IsMtNeedAdp(byMtId))
        {
			return TRUE;
        }
		else
        {
            return FALSE;
        }
    }   
    else if(MODE_AUDIO == byMode)
    {
        if(m_tConf.m_tStatus.IsAudAdapting() && IsMtNeedAdapt(ADAPT_TYPE_AUD, byMtId, &tMtSrc))
        {
            byAdpType = ADAPT_TYPE_AUD;
            return TRUE;
        }
        else if(m_tConf.m_tStatus.IsCasdAudAdapting() && IsMtNeedAdapt(ADAPT_TYPE_CASDAUD, byMtId, &tMtSrc))
        {
            byAdpType = ADAPT_TYPE_CASDAUD;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }    
    
    return FALSE;
}


/*=============================================================================
函 数 名： IsIsMtSrcVmp2
功    能： 终端源是否vmp第二路码流
算法实现： 
全局变量： 
参    数： u8 byMtId
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/9  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMcuVcInst::IsMtSrcVmp2(u8 byMtId) 
{
    if (0 == m_tConf.GetSecBitRate() || 
        MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() ||       
        !(m_tVidBrdSrc == m_tVmpEqp) ||
        m_ptMtTable->GetMtReqBitrate(byMtId) == m_tConf.GetBitRate())
    {
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
    函 数 名： StartAdapt
    功    能： 开启适配
    算法实现： 
    全局变量： 
    参    数： u8  byAdaptType     适配类型
               u16 wBitRate        适配码率
               TSimCapSet *pDstSCS 适配后的码流能力集
               TSimCapSet *pSrcSCS 适配前的原始的码流能力集
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/03/08  3.6			万春雷                  创建
    2007/01/30  4.0         顾振华                 8000B能力限制
=============================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::StartAdapt(u8 byAdaptType, u16 wBitRate, TSimCapSet *pDstSCS, TSimCapSet *pSrcSCS)
{
	u16 wWidth;
    u16 wHight;
	TSimCapSet tSrcSCS;
	TAdaptParam tAdaptParam;
    memset(&tAdaptParam, 0, sizeof(tAdaptParam));
    
    if (ADAPT_TYPE_AUD == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tAudBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

    if (ADAPT_TYPE_VID == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tVidBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

    if (ADAPT_TYPE_BR == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tBrBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

    if (ADAPT_TYPE_CASDAUD == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tCasdAudBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

    if (ADAPT_TYPE_CASDVID == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tCasdVidBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

    // guzh [1/30/2007] 8000B能力限制
    u16 wError = 0;
    if ( !CMcuPfmLmt::IsBasOprSupported( m_tConf, GetMixMtNumInGrp(), wError ) )
    {
        NotifyMcsAlarmInfo(0, wError);
        return FALSE;
    }

	if (NULL == pDstSCS || pDstSCS->IsNull())
	{
        if (ADAPT_TYPE_AUD == byAdaptType)
        {
		    tAdaptParam.SetVidType(MEDIA_TYPE_NULL);
		    tAdaptParam.SetAudType(m_tConf.GetMainAudioMediaType());
        }
        else
        {
            tAdaptParam.SetVidType(m_tConf.GetMainVideoMediaType());
            u8 byResolution = m_tConf.GetMainVideoFormat();
            m_tConf.GetVideoScale(m_tConf.GetMainVideoMediaType(), 
                                wWidth, wHight, &byResolution);
            tAdaptParam.SetResolution(wWidth, wHight);
		    tAdaptParam.SetAudType(MEDIA_TYPE_NULL);
        }
	}
	else
	{
		tAdaptParam.SetVidType(pDstSCS->GetVideoMediaType());	
		tAdaptParam.SetAudType(pDstSCS->GetAudioMediaType());
        u8 byResolution = pDstSCS->GetVideoResolution();
        m_tConf.GetVideoScale(pDstSCS->GetVideoMediaType(), wWidth, wHight, &byResolution);
        tAdaptParam.SetResolution(wWidth, wHight);
	}

	if (NULL == pSrcSCS || pSrcSCS->IsNull())
	{
		if (NULL == pDstSCS || pDstSCS->IsNull())
		{
			tSrcSCS.SetVideoMediaType(tAdaptParam.GetVidType());
			tSrcSCS.SetAudioMediaType(tAdaptParam.GetAudType());
		}
		else
		{
			tSrcSCS = *pDstSCS;
		}
	}
	else
	{
		tSrcSCS = *pSrcSCS;
	}

    if (ADAPT_TYPE_AUD != byAdaptType && ADAPT_TYPE_CASDAUD != byAdaptType)
    {
        if (0 != m_tConf.GetSecBitRate())//双速会议
        {
            // 若存在低速录制或者广播，直接调整为第二速（即此双速会议的最低限速）
            if ((m_tConf.GetConfAttrb().IsMulticastMode() && 
                 m_tConf.GetConfAttrb().IsMulcastLowStream()) || 
                 m_tRecPara.IsRecLowStream())
            {
                wBitRate = m_tConf.GetSecBitRate();
            }
            // 否则降至所需的最低速率（此速率高于或等于第二速）
            else
            {
                wBitRate = (wBitRate > m_tConf.GetSecBitRate()) ? wBitRate : m_tConf.GetSecBitRate();
            }
        }

        if (0 == wBitRate)
        {
            tAdaptParam.SetBitRate(m_tConf.GetBitRate());
        }
        else
        {
            if (wBitRate != m_tConf.GetBitRate())
            {
                tAdaptParam.SetBitRate(wBitRate);
            }
            else
            {
                tAdaptParam.SetBitRate(wBitRate);
            }
        }
        m_tConf.GetVideoScale(tAdaptParam.GetVidType(), wWidth, wHight);
        tAdaptParam.SetResolution(wWidth, wHight);

        if (ADAPT_TYPE_VID == byAdaptType)
        {
            m_wVidBasBitrate = wBitRate;            
        }
        else if (ADAPT_TYPE_BR == byAdaptType)
        {
            m_wBasBitrate = wBitRate;
        }
        ConfLog(FALSE, "Request start adapt(type.%d) and bitrate is:%u\n", byAdaptType, wBitRate);
    }
    else
    {
        tAdaptParam.SetBitRate(GetAudioBitrate(tAdaptParam.GetAudType()));
    }

	return StartBasAdapt(&tAdaptParam, byAdaptType, &tSrcSCS);
}

/*=============================================================================
    函 数 名： ChangeAdapt
    功    能： 改变适配(目前没有对级联适配支持)
    算法实现： 
    全局变量： 
    参    数： u16 wBitRate        适配码率
               TSimCapSet *pDstSCS 适配后的码流能力集
               TSimCapSet *pSrcSCS 适配前的原始的码流能力集
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/3/9    3.5			万春雷                  创建
=============================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::ChangeAdapt(u8 byAdaptType, u16 wBitRate, TSimCapSet *pDstSCS, TSimCapSet *pSrcSCS)
{
    u16 wWidth;
    u16 wHight;
	TSimCapSet tSrcSCS;
	TAdaptParam tAdaptParam;
    memset(&tAdaptParam, 0, sizeof(tAdaptParam));

    if (ADAPT_TYPE_AUD == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tAudBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

    if (ADAPT_TYPE_VID == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tVidBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

    if (ADAPT_TYPE_BR == byAdaptType)
    {
        if (!g_cMcuVcApp.IsPeriEqpConnected(m_tBrBasEqp.GetEqpId()))
        {
            return FALSE;
        }
    }

	if (NULL == pDstSCS || pDstSCS->IsNull())
	{
        if (ADAPT_TYPE_AUD == byAdaptType)
        {
		    tAdaptParam.SetVidType(MEDIA_TYPE_NULL);
		    tAdaptParam.SetAudType(m_tConf.GetMainAudioMediaType());
        }
        else
        {
            tAdaptParam.SetVidType(m_tConf.GetMainVideoMediaType());
		    tAdaptParam.SetAudType(MEDIA_TYPE_NULL);
            m_tConf.GetVideoScale(byAdaptType, wWidth, wHight);
            tAdaptParam.SetResolution(wWidth, wHight);
        }
	}
	else
	{
		tAdaptParam.SetVidType(pDstSCS->GetVideoMediaType());	
		tAdaptParam.SetAudType(pDstSCS->GetAudioMediaType());
        u8 byResolution = pDstSCS->GetVideoResolution();
        m_tConf.GetVideoScale(byAdaptType, wWidth, wHight, &byResolution);
        tAdaptParam.SetResolution(wWidth, wHight);
	}

    if (ADAPT_TYPE_AUD != byAdaptType && ADAPT_TYPE_CASDAUD != byAdaptType)
    {
//        if (0 != m_tConf.GetSecBitRate()) //双速会议
//        {
//            if ((m_tConf.GetConfAttrb().IsMulticastMode() && 
//                m_tConf.GetConfAttrb().IsMulcastLowStream()) || 
//                m_byRecLowStream)
//            {
//                wBitRate = m_tConf.GetSecBitRate();
//            }
//            else
//            {
//                wBitRate = (wBitRate > m_tConf.GetSecBitRate()) ? wBitRate : m_tConf.GetSecBitRate();
//            }
//        }

        if (0 == wBitRate)
        {
            tAdaptParam.SetBitRate(m_tConf.GetBitRate());
        }
        else
        {
            if (wBitRate != m_tConf.GetBitRate())
            {
                tAdaptParam.SetBitRate(wBitRate);
            }
            else
            {
                tAdaptParam.SetBitRate(wBitRate);
            }
        }
        m_tConf.GetVideoScale(tAdaptParam.GetVidType(), wWidth, wHight);
        tAdaptParam.SetResolution(wWidth, wHight);

        if (ADAPT_TYPE_VID == byAdaptType)
        {
            m_wVidBasBitrate = wBitRate;
        }
        else if (ADAPT_TYPE_BR == byAdaptType)
        {
            m_wBasBitrate = wBitRate;
        }

        ConfLog(TRUE, "Change adapt(type.%d) param and bitrate is:%u\n", byAdaptType, wBitRate);
    }
    else
    {
        tAdaptParam.SetBitRate(GetAudioBitrate(tAdaptParam.GetAudType()));
    }

	return ChangeBasAdaptParam(&tAdaptParam, byAdaptType, pSrcSCS);	

}

/*====================================================================
    函数名      ：SendTrapMsg
    功能        ：发 Trap 消息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/29    3.0         胡昌威          创建
====================================================================*/
void CMcuVcInst::SendTrapMsg( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
    post( MAKEIID( AID_MCU_AGENT, 1 ), wEvent, pbyMsg, wLen );

	return;
}

/*====================================================================
    函数名      ：ShowMtStat
    功能        ：打印会议终端状态
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/11    3.0         胡昌威          创建
====================================================================*/
void CMcuVcInst::ShowMtStat( void )
{
	if( !m_tConf.m_tStatus.IsOngoing() )
	{
		return;
	}

	//打印本地终端状态
	TMtStatus   tMtStatus;
	TCapSupport tCapSupport;
	OspPrintf( TRUE, FALSE, "\n本地终端状态: McuId-%d\n", LOCAL_MCUID );
	for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
		{
			m_ptMtTable->GetMtStatus( byLoop, &tMtStatus );
			m_ptMtTable->GetMtCapSupport( byLoop, &tCapSupport );
			tMtStatus.Print();
			tCapSupport.Print();
		}
	}

	//打印其它Mc上终端状态
	for( u8 byLoop1 = 0; byLoop1 < MAXNUM_SUB_MCU; byLoop1++ )
	{
		u8 byMcuId = m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop1].m_byMcuId;
		if( byMcuId == 0  )
		{
			continue;
		}
		
		OspPrintf( TRUE, FALSE, "\n其它终端状态: McuId-%d\n", byMcuId );
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( byMcuId );
		for( u8 byIdx = 1; byIdx <= MAXNUM_CONF_MT; byIdx++ )
		{	
            if ( !ptConfMcInfo->m_atMtStatus[byIdx-1].IsNull() )
            {
                OspPrintf(TRUE, FALSE, "Postion Index: %d\t", byIdx-1 );
                ptConfMcInfo->m_atMtStatus[byIdx-1].Print();
            }			
		}
	}

	return;
}

/*====================================================================
    函数名      ：ProcMpMessage
    功能        ：处理Mp的消息
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/10    3.0         胡昌威          创建
====================================================================*/
void CMcuVcInst::ProcMpMessage( CMessage * const pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TSwitchChannel *ptSwitchChannel;
	CMcuVcInst *pMcuVcInst = NULL;
    u8 byMpId = cServMsg.GetSrcDriId();
	
	if( CurState() != STATE_ONGOING )
	{
		return;
	}
	
	switch( pcMsg->event )
	{
	case MP_MCU_REG_REQ://Mp注册
        {
            // xsl [9/12/2006] 通知mp板是否归一重整
            TMp tMp = *(TMp*)cServMsg.GetMsgBody();
            u8 m_byUniformMode = m_tConf.GetConfAttrb().IsAdjustUniformPack();
			cServMsg.SetMsgBody((u8 *)&m_byUniformMode, sizeof(u8));
            cServMsg.SetConfIdx(m_byConfIdx);
			g_cMpSsnApp.SendMsgToMpSsn(tMp.GetMpId(), MCU_MP_CONFUNIFORMMODE_NOTIFY, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        
            // xsl [9/15/2006]调整桥交换
            AdjustSwitchBridge(tMp.GetMpId(), TRUE);
        }
		break;

	case MP_MCU_ADDSWITCH_ACK://成功增加交换
	    ptSwitchChannel = (TSwitchChannel*)cServMsg.GetMsgBody();
	    m_ptSwitchTable->AddSwitchTable( cServMsg.GetSrcDriId(), ptSwitchChannel );	  
	    StartMtSendbyNeed( ptSwitchChannel );
		NotifyRecStartPlaybyNeed( ptSwitchChannel );
		break;

	case MP_MCU_REMOVESWITCH_ACK://成功移除交换
	    ptSwitchChannel = (TSwitchChannel*)cServMsg.GetMsgBody();
        MpManagerLog( "[ProcMpMessage] MP_MCU_REMOVESWITCH_ACK : dst<%s@%d>\n", 
                       StrOfIP( ptSwitchChannel->GetDstIP() ), ptSwitchChannel->GetDstPort() );
	    m_ptSwitchTable->RemoveSwitchTable( cServMsg.GetSrcDriId(), ptSwitchChannel );
        // zgc, 2008-05-27, 非广播交换目的才需要检查是否要停广播
        if ( ptSwitchChannel->GetRcvPort() != 0 )
        {
            StopMtSendbyNeed( ptSwitchChannel );
        }
		break;
	case MP_MCU_ADDMULTITOONESWITCH_ACK:		//MP接受MCU的增加多点到一点交换请求
	    ptSwitchChannel = (TSwitchChannel*)cServMsg.GetMsgBody();
	    m_ptSwitchTable->ProcMultiToOneSwitch( ptSwitchChannel, 1 );
		break;
	case MP_MCU_REMOVEMULTITOONESWITCH_ACK:		//MP接受MCU的移除多点到一点交换请求
	    ptSwitchChannel = (TSwitchChannel*)cServMsg.GetMsgBody();
	    m_ptSwitchTable->ProcMultiToOneSwitch( ptSwitchChannel, 2 );
		break;
	case MP_MCU_STOPMULTITOONESWITCH_ACK:       //MP接受MCU的停止多点到一点交换请求 
	    ptSwitchChannel = (TSwitchChannel*)cServMsg.GetMsgBody();
	    m_ptSwitchTable->ProcMultiToOneSwitch( ptSwitchChannel, 3 );
		break;
		
	case MP_MCU_GETSWITCHSTATUS_ACK:
	case MP_MCU_ADDRECVONLYSWITCH_ACK:
	case MP_MCU_REMOVERECVONLYSWITCH_ACK:
	case MP_MCU_SETRECVSWITCHSSRC_ACK:
		break;	

	case MP_MCU_ADDSWITCH_NACK:
	case MP_MCU_ADDMULTITOONESWITCH_NACK:
		{
            // guzh [1/11/2007] 添加交换失败
			ConfLog(FALSE, "Fail to Add switch. Mp.%d is full\n", cServMsg.GetSrcDriId());
		}
		break;
	case MP_MCU_REMOVESWITCH_NACK:
		{
            ConfLog(FALSE, "Fail to Remove switch at Mp.%d\n", cServMsg.GetSrcDriId());
		}
		break;
	case MP_MCU_GETSWITCHSTATUS_NACK:	
	case MP_MCU_REMOVEMULTITOONESWITCH_NACK:
	case MP_MCU_STOPMULTITOONESWITCH_NACK:
	case MP_MCU_ADDRECVONLYSWITCH_NACK:
	case MP_MCU_REMOVERECVONLYSWITCH_NACK:
	case MP_MCU_SETRECVSWITCHSSRC_NACK:	
		break;	
        
        // guzh [3/26/2007]成功增加广播交换
    case MP_MCU_ADDBRDSRCSWITCH_ACK:
        ptSwitchChannel = (TSwitchChannel*)cServMsg.GetMsgBody();       
        m_ptSwitchTable->ProcBrdSwitch( 1, ptSwitchChannel, 1, byMpId );       
        StartMtSendbyNeed( ptSwitchChannel );   
        break;
    case MP_MCU_ADDBRDDSTSWITCH_ACK:
        {
        /*
        TSwitchChannel atChnnels[MAXNUM_CONF_MT];            
        u8 byChnlNum = cServMsg.GetMsgBodyLen() / (sizeof(TSwitchChannel)+3*sizeof(u8));
        u8 *byPtr = cServMsg.GetMsgBody();
        for ( u8 byLoop = 0; byLoop < byChnlNum; byLoop++ )
        {
        atChnnels[byLoop] = *(TSwitchChannel*)byPtr;
        byPtr += (sizeof(TSwitchChannel)+3*sizeof(u8));
        }
            */
            TSwitchChannel *ptChnnel = (TSwitchChannel*)cServMsg.GetMsgBody();
            u8 byChnlNum = cServMsg.GetMsgBodyLen() / sizeof(TSwitchChannel);
            
            MpManagerLog("[ProcMpMessage] Mp Added broadcast destination MT n=%d!\n", byChnlNum);
            m_ptSwitchTable->ProcBrdSwitch( byChnlNum, ptChnnel, 11, byMpId );        
            break;
        }
        // 成功移除广播源
    case MP_MCU_REMOVEBRDSRCSWITCH_ACK:
        ptSwitchChannel = (TSwitchChannel*)cServMsg.GetMsgBody();
        m_ptSwitchTable->ProcBrdSwitch( 1, ptSwitchChannel, 2, byMpId );
        // guzh [4/3/2007] 如果是终端,这里还不移交换.等 MP_MCU_BRDSRCSWITCHCHANGE_NTF
        break;
        
    case MP_MCU_BRDSRCSWITCHCHANGE_NTF:    // 广播源完成移除通知
        ProcVidBrdSrcChanged(pcMsg);
        break;
        
    case MP_MCU_ADDBRDSRCSWITCH_NACK:
    case MP_MCU_REMOVEBRDSRCSWITCH_NACK:
    case MP_MCU_ADDBRDDSTSWITCH_NACK:
        ConfLog( FALSE, "MP switch failed. %d(%s)!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ) );
        break;
        
    default:
        ConfLog( FALSE, "Wrong message %u(%s) received in DaemonEntry!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ) );
        break;	
    }
    
    return;
}


/*====================================================================
函数名      ：ProcVidBrdSrcChanged
功能        ：处理MP通知视频广播源已变化的通知
算法实现    ：1. 停止 Mt-->Mp 的码流交换
2. 停止桥交换(可能包括第二路)
3. etc...
引用全局变量：
输入参数说明：CMessage * const pcMsg, 传入的消息
返回值说明  ：无
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
07/03/27    4.0         顾振华          创建
====================================================================*/
void CMcuVcInst::ProcVidBrdSrcChanged( CMessage * const pcMsg )
{    
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    TMt tOldVidSrc = *(TMt*)cServMsg.GetMsgBody();
    u8 bySrcDriId = cServMsg.GetSrcDriId();
    
    if (tOldVidSrc.IsNull())
        return;
    
	// xliang [2/6/2009] 判断之前是否是新VMP广播
	BOOL32 bNewVmpBrd = FALSE;
	if( tOldVidSrc == m_tVmpEqp )
	{
		TPeriEqpStatus tPeriEqpStatus; 
		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
		u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
		if(byVmpSubType != VMP)
		{
			bNewVmpBrd = TRUE;
		}
	}
    TSwitchChannel tSwitchChannel;
    u32 dwMtSwitchIp;
    u16 wMtSwitchPort;
    u32 dwMtSrcIp;    
    g_cMpManager.GetSwitchInfo(tOldVidSrc,dwMtSwitchIp, wMtSwitchPort, dwMtSrcIp );
    // 只处理来自接入源的MP消息
    if ( bySrcDriId != g_cMcuVcApp.FindMp( dwMtSwitchIp ) )
    {
        return;
    }
    
    TLogicalChannel tLogicalChannel;
    
    //启用适配并且开启了双流, 恢复老的第一路发送源的发送码率,没有适配器则根据FlowControl调整
    if ( !m_tDoubleStreamSrc.IsNull() &&
        TYPE_MT == tOldVidSrc.GetType() )
    {        
        if (m_ptMtTable->GetMtLogicChnnl(tOldVidSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
        {
            tLogicalChannel.SetFlowControl(m_ptMtTable->GetSndBandWidth( tOldVidSrc.GetMtId()));
            cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(tOldVidSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
        }
    } 
    
    u8 bySrcChnnl = 0;

    // zbq [08/23/2007] 查看该终端是否在回传通道里，是则不能停其桥交换，否则会导致回传通道无码流
    if ( !m_tCascadeMMCU.IsNull() &&
         m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId())->m_tSpyMt == m_tVidBrdSrc )
    {
    }
    else
    {
        // 老广播源是终端且有普通交换存在，则不拆桥, zgc, 2008-05-23
        if ( tOldVidSrc.GetType() == TYPE_MCUPERI ||
            !m_ptSwitchTable->IsValidCommonSwitchSrcIp(dwMtSrcIp, wMtSwitchPort, MODE_VIDEO) )
        {
            if(bNewVmpBrd) // xliang [2/6/2009] 是新VMP，则要拆3路交换桥(在进该函数之前就拆掉)
			{
// 				bySrcChnnl = 0;
// 				g_cMpManager.RemoveSwitchBridge(tOldVidSrc, bySrcChnnl, MODE_VIDEO);
// 				g_cMpManager.RemoveSwitchBridge(tOldVidSrc, bySrcChnnl+1, MODE_VIDEO);
// 				g_cMpManager.RemoveSwitchBridge(tOldVidSrc, bySrcChnnl+2, MODE_VIDEO);
// 				CallLog("New VMP : RemoveSwitchBridge!\n");
			}
			else
			{
            // 拆除交换桥
            bySrcChnnl = (tOldVidSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
            g_cMpManager.RemoveSwitchBridge(tOldVidSrc, bySrcChnnl, MODE_VIDEO);
			}
        }
    }
    
    //  xsl [3/10/2006] 双速或双格式会议广播源为vmp时，拆除第二路交换桥
	// xliang [2/18/2009] mpu-vmp不在这里拆
    if (tOldVidSrc == m_tVmpEqp 
		&& !bNewVmpBrd 
		&& ((0 != m_tConf.GetSecBitRate() || MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType())))
    {
        // VMP不可能有普通交换，这里不进行判断，zgc, 2008-05-23
        g_cMpManager.RemoveSwitchBridge(tOldVidSrc, bySrcChnnl+1, MODE_VIDEO);           
    }	
    
    // 停止交换源发送                  
    tSwitchChannel.SetSrcMt( tOldVidSrc );        
    tSwitchChannel.SetSrcIp(dwMtSrcIp);
    tSwitchChannel.SetRcvIP(dwMtSwitchIp);
    tSwitchChannel.SetRcvPort(wMtSwitchPort);
    
    MpManagerLog("[ProcVidBrdSrcChanged]Recvied MT(%d,%d) %s switch removed notify.\n",
        tOldVidSrc.GetMcuId(), tOldVidSrc.GetMtId(), StrOfIP(dwMtSrcIp));
    
    m_ptSwitchTable->ProcBrdSwitch( 1, &tSwitchChannel, 2, bySrcDriId );
	if (TYPE_MT == tOldVidSrc.GetType())
    {    
        StopMtSendbyNeed(&tSwitchChannel);
    }    
}


/*====================================================================
    函数名      ：DaemonProcMpFluxNotify
    功能        ：MP能力限制相关消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/02/11    4.0         张宝卿          创建
====================================================================*/
void CMcuVcInst::DaemonProcMpFluxNotify( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    switch( pcMsg->event )
    {
    case MP_MCU_FLUXSTATUS_NOTIFY:
        {
            u16 wMpBand = *(u16*)cServMsg.GetMsgBody();
            g_cMcuVcApp.m_atMpData[cServMsg.GetSrcDriId()-1].m_wNetBandReal = ntohs(*(u16*)cServMsg.GetMsgBody());
            if ( ntohs(*(u16*)cServMsg.GetMsgBody()) > g_cMcuVcApp.m_atMpData[cServMsg.GetSrcDriId()-1].m_wNetBandAllowed )
            {
                u32 dwMpIp = g_cMcuVcApp.m_atMpData[cServMsg.GetSrcDriId()-1].m_tMp.GetIpAddr();
                dwMpIp = htonl(dwMpIp);
                cServMsg.SetMsgBody( (u8*)&dwMpIp, sizeof(u32) );
                SendMsgToAllMcs( MCU_MCS_MPFLUXOVERRUN_NOTIFY, cServMsg );
                break;                        
            }
            // MpManagerLog("[DaemonProcMpFluxNotify] MP_MCU_FLUXSTATUS_NOTIFY recieved, Mp.%d's Band = %d !\n", 
            //         cServMsg.GetSrcDriId(), wMpBand );
            break;
        }
    default:
        OspPrintf( TRUE, FALSE, "[DaemonProcMpFluxNotify] unexpected msg.%d<%s> recieved !\n",
                                 pcMsg->event, OspEventDesc(pcMsg->event) );
        break;
    }
}


/*====================================================================
    函数名      ：ProcMpDissconnected
    功能        ：处理Mp的断链
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/06/08    3.0         胡昌威          创建
====================================================================*/
void CMcuVcInst::ProcMpDissconnected( CMessage * const pcMsg )
{	
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMp tMp;
	TMt tMt;
	u8 byLoop;

 	switch( CurState() )
	{
	case STATE_ONGOING:

		//通知所属转发板的终端掉线
		tMp = *(TMp*)cServMsg.GetMsgBody();	 
		for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			if( m_tConfAllMtInfo.MtInConf( byLoop ) )
			{
				if( tMp.GetMpId() == m_ptMtTable->GetMpId( byLoop ) )
				{
					tMt = m_ptMtTable->GetMt( byLoop );
					RemoveJoinedMt( tMt, TRUE, MTLEFT_REASON_EXCEPT );
				}
			}
		}
        
        // xsl [9/15/2006]调整桥交换
        AdjustSwitchBridge(tMp.GetMpId(), FALSE);
		
		ConfLog( FALSE, "Mp %d disconnected, some mt droped!\n", tMp.GetMpId() );
		
        // xsl [11/14/2006] 清除此mp交换记录
        m_ptSwitchTable->ClearSwitchTable(tMp.GetMpId());

        // zbq [02/14/2007] 清除此mp对应的data信息
        g_cMcuVcApp.m_atMpData[tMp.GetMpId()-1].SetNull();

		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*====================================================================
    函数名      ：ProcMtAdpDissconnected
    功能        ：处理MtAdp的断链
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/06/08    3.0         胡昌威          创建
====================================================================*/
void CMcuVcInst::ProcMtAdpDissconnected( CMessage * const pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMtAdpReg tMtAdpReg;
	TMt tMt;
	u8 byLoop;

	//清掉相关终端
 	switch( CurState() )
	{
	case STATE_ONGOING:

		//通知所属适配板的终端掉线
		tMtAdpReg =  *( TMtAdpReg* )cServMsg.GetMsgBody();
		for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			if( m_tConfAllMtInfo.MtInConf( byLoop ) )
			{
				tMt = m_ptMtTable->GetMt( byLoop );
				if( tMtAdpReg.GetDriId() == tMt.GetDriId() )
				{
					RemoveJoinedMt( tMt, FALSE, MTLEFT_REASON_EXCEPT );                    
				}
			}
		}

		ConfLog( FALSE, "Mtadp %d disconnected, some mt droped!\n", tMtAdpReg.GetDriId() );
		
		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}  

	return;
}

/*====================================================================
    函数名      : GetVmpDynStyle
    功能        : 得到动态分屏的画面合成方式
    算法实现    : 
    引用全局变量: 无
    输入参数说明: byMtNum 终端数量
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/31    3.0         胡昌威        创建
    05/04/13    3.6         libo          根据需求书调整动态分屏的逻辑
    07/01/30    4.0         顾振华        8000B能力限制
	09/04/21    4.6			薛亮		  根据新VMP进行调整
====================================================================*/
u8 CMcuVcInst::GetVmpDynStyle( u8 byMtNum )
{
#ifdef _MINIMCU_
   
    // guzh [1/30/2007] 能力限制
    u16 wError = 0;
    // guzh [8/1/2007] 根据传入的终端数计算，而非当前所有在线终端
    u8 byVmpCapStyle = CMcuPfmLmt::GetMaxDynVmpStyle(m_tConf, byMtNum, GetMixMtNumInGrp(), wError);

    return byVmpCapStyle;
    
#else   // 8000
    TPeriEqpStatus tPeriEqpStatus;
    g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
    u8 byEqpMaxChl = tPeriEqpStatus.m_tStatus.tVmp.m_byChlNum;
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;// xliang [2/13/2009] 

    if ( byEqpMaxChl == 0 || !ISTRUE(tPeriEqpStatus.m_byOnline) )
    {
        return VMP_STYLE_NONE;
    }

    // 获取最大能力的风格
    u8 byVmpCapChlNum = CMcuPfmLmt::GetMaxCapVMPByConfInfo(m_tConf);
    if (byVmpCapChlNum == 0)
    {
        return VMP_STYLE_NONE;
    } 

	// xliang [2/12/2009] 区分VMP
	if( MPU_SVMP == byVmpSubType 
		|| MPU_DVMP == byVmpSubType
		|| EVPU_SVMP == byVmpSubType
		|| EVPU_DVMP == byVmpSubType
		)
	{
		if ( byMtNum > MAXNUM_MPUSVMP_MEMBER ) 
		{
			byMtNum = MAXNUM_MPUSVMP_MEMBER;
		}
	}
	else
	{
		if ( byMtNum > MAXNUM_SDVMP_MEMBER) 
		{
			byMtNum = MAXNUM_SDVMP_MEMBER;
		}
	}

    u8 byVmpStyle;
    switch(byMtNum)
	{
	case 0:
	case 1:
		byVmpStyle = VMP_STYLE_ONE;
		break;
	case 2:
		byVmpStyle = VMP_STYLE_VTWO;
		break;
	case 3:
		byVmpStyle = VMP_STYLE_THREE;
		break;
	case 4:
		byVmpStyle = VMP_STYLE_FOUR;
		break;
	case 5:
	case 6:
		byVmpStyle = VMP_STYLE_SIX;
		break;
	case 7:
		byVmpStyle = VMP_STYLE_SEVEN;
		break;
	case 8:
		byVmpStyle = VMP_STYLE_EIGHT;
		break;
	case 9:
		byVmpStyle = VMP_STYLE_NINE;
		break;
	case 10:
		byVmpStyle = VMP_STYLE_TEN;
		break;
	case 11:
	case 12:
	case 13:
		byVmpStyle = VMP_STYLE_THIRTEEN;
		break;
	case 14:
	case 15:
	case 16:
		byVmpStyle = VMP_STYLE_SIXTEEN;
		break;
	case 17:
	case 18:
	case 19:
	case 20:
		// xliang [2/13/2009] switch case前有MtNum过滤，所以此处不再区分vmp种类
		byVmpStyle = VMP_STYLE_TWENTY;
		break;
	default:
		byVmpStyle = VMP_STYLE_NONE;
		break;
	}
    u8 byCurChlNum = GetVmpChlNumByStyle( byVmpStyle );	 //当前风格所需要的通道数
    
    if (byCurChlNum > byVmpCapChlNum || byCurChlNum > byEqpMaxChl)
    {
        byVmpStyle = GetVmpDynStyle(byMtNum - 1);
    }    

	if ( byVmpStyle == VMP_STYLE_NONE )
	{
		return VMP_STYLE_NONE;
	}
    
	// 计算 最大可能满足需要的 VMP支持的 合成风格, zgc, 2008-03-03	
	TPeriEqpStatus atEqpStatus[MAXNUM_PERIEQP];
	memset( atEqpStatus, 0, sizeof(atEqpStatus) );
	u8 byIdx = 0;
	u8 byPeriIdLoop = VMPID_MIN;
    u8 byPeriIdMax = VMPID_MAX;
	
	while( byPeriIdLoop <= byPeriIdMax && byIdx < MAXNUM_PERIEQP )
	{
		if( g_cMcuVcApp.IsPeriEqpValid( byPeriIdLoop ) )
		{
			g_cMcuVcApp.GetPeriEqpStatus( byPeriIdLoop, atEqpStatus+byIdx );
			byIdx++;
		}
		byPeriIdLoop++;
	}
    u8 byValidVmpNum = byIdx;

	u8 abyStyleArray[32];
    u8 bySize = 32;
	u8 byTempSize = bySize;
	CMcuPfmLmt::GetSupportedVmpStyles( m_tConf, m_byConfIdx, atEqpStatus, byValidVmpNum, abyStyleArray, bySize );

	if ( bySize <= byTempSize && bySize > 0 )
	{
		u8 byLop = 0;
		for ( byLop = 0; byLop < bySize; byLop++ )
		{
			// 根据MT数算出的风格可以被VMP支持，则直接返回
			if ( abyStyleArray[byLop] == byVmpStyle )
			{
				return byVmpStyle;
			}
		}

		// 不被支持，则返回支持的最大风格
		return abyStyleArray[bySize-1];
	}
	else
	{
		return VMP_STYLE_NONE;
	}

    return byVmpStyle;

#endif   
}

/*=============================================================================
函 数 名： IsVMPSupportVmpStyle
功    能： 指定的画面合成器是否支持某画面合成风格
算法实现： 
全局变量： 
参    数： u8 byStyle
           u8 byEqpId: 判断的vmpId
           u16 &wError 返回的错误码
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/31  4.0			许世林                  创建
2006/5/09  4.0          顾振华                  增加设备参数
=============================================================================*/
BOOL32 CMcuVcInst::IsVMPSupportVmpStyle(u8 byStyle, u8 byEqpId, u16 &wError)
{
#ifndef _MINIMCU_
	// 8000
    TPeriEqpStatus tPeriEqpStatus;  
    g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tPeriEqpStatus);

	if ( !CMcuPfmLmt::IsVmpStyleSupport( m_tConf, m_byConfIdx, 
							       		 &tPeriEqpStatus, 1, byStyle, 
									     byEqpId ) )
    {
		wError = ERR_INVALID_VMPSTYLE;
		return FALSE;
    }

	return TRUE;
	
#else
	// guzh [1/30/2007] 对于8000B的性能保护
    if ( !CMcuPfmLmt::IsVmpOprSupported( m_tConf, byStyle, m_tConfAllMtInfo.GetLocalJoinedMtNum(), GetMixMtNumInGrp(), wError )  )
    {
        ConfLog( FALSE, "[IsVMPSupportVmpStyle] Eqp.%d: IsVmpOprSupported return false. Error=%d !\n", 
                        byEqpId, wError );
        return FALSE;
    }
    return TRUE;
#endif
}

/*=============================================================================
函 数 名： IsMCUSupportVmpStyle
功    能： 整个MCU是否支持某画面合成风格
算法实现： 
全局变量： 
参    数： u8 byStyle
           u8 byEqpId: 支持该风格的VMPID
           u8 byEqpType
           u16 &wError 返回的错误码
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/12/27  4.0         张宝卿                创建
=============================================================================*/
BOOL32 CMcuVcInst::IsMCUSupportVmpStyle( u8 byStyle, u8 &byEqpId, u8 byEqpType, u16 &wError )
{
#ifndef _MINIMCU_
    // 8000
    byEqpId = 0;
	
	TPeriEqpStatus atEqpStatus[MAXNUM_PERIEQP];
	memset( atEqpStatus, 0, sizeof(atEqpStatus) );
	u8 byIdx = 0;
	u8 byPeriIdLoop;
    u8 byPeriIdMax;
	
	if( EQP_TYPE_VMP == byEqpType )
	{
		byPeriIdLoop = VMPID_MIN;
        byPeriIdMax = VMPID_MAX;
    }
    else
    {
        byPeriIdLoop = VMPTWID_MIN;
        byPeriIdMax = VMPTWID_MAX;
    }
	
	while( byPeriIdLoop <= byPeriIdMax && byIdx < MAXNUM_PERIEQP )
	{
		if( g_cMcuVcApp.IsPeriEqpValid( byPeriIdLoop ) )
		{
			g_cMcuVcApp.GetPeriEqpStatus( byPeriIdLoop, atEqpStatus+byIdx );
			byIdx++;
		}
		byPeriIdLoop++;
	}
    u8 byValidVmpNum = byIdx;
	
	if ( !CMcuPfmLmt::IsVmpStyleSupport( m_tConf, m_byConfIdx, 
		atEqpStatus, byValidVmpNum, byStyle, 
		byEqpId ))
    {
		wError = ERR_MCU_ALLIDLEVMP_NO_SUPPORT;
		return FALSE;
    }
	
	return TRUE;
	
#else
	
    // guzh [7/25/2007] FIXME：8000B只需要找空闲的VMP即可，暂时这么做
    u8 byIdleVMPNum = 0;
    u8 abyIdleVMPId[MAXNUM_PERIEQP];
    memset( &abyIdleVMPId, 0, sizeof(abyIdleVMPId) );
    g_cMcuVcApp.GetIdleVMP( abyIdleVMPId, byIdleVMPNum, sizeof(abyIdleVMPId) );    
    if ( byIdleVMPNum > 0 )
    {
        byEqpId = abyIdleVMPId[0];
    }
    else
    {
        ConfLog( FALSE, "[IsMCUSupportVmpStyle] no idle vmp found!\n" );
        return FALSE;        
    }
    
    // guzh [1/30/2007] 对于8000B的性能保护
    if ( !CMcuPfmLmt::IsVmpOprSupported( m_tConf, byStyle, m_tConfAllMtInfo.GetLocalJoinedMtNum(), GetMixMtNumInGrp(), wError )  )
    {
        ConfLog( FALSE, "[IsMCUSupportVmpStyle] Eqp.%d: IsVmpOprSupported return false. Error=%d !\n", 
			byEqpId, wError );
        return FALSE;
    }
	
    return TRUE;
#endif
}



/*====================================================================
    函数名      : HasJoinedSpeaker
    功能        : 判断发言者是否与会
    算法实现    : 
    引用全局变量: 无
    输入参数说明: 无
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/31    3.0         胡昌威       创建
====================================================================*/
BOOL32 CMcuVcInst::HasJoinedSpeaker( void )
{
	if( !m_tConf.HasSpeaker() )
		return  FALSE;
    
	TMt	tSpeaker;
	tSpeaker = GetLocalSpeaker();
	if( tSpeaker.GetType() == TYPE_MT )
		return m_tConfAllMtInfo.MtJoinedConf( tSpeaker.GetMtId() );
	else if( tSpeaker.GetType() == TYPE_MCUPERI )
		return TRUE;
	else
		return FALSE;
}

/*====================================================================
    函数名      : HasJoinedChairman
    功能        : 判断主席是否与会
    算法实现    : 
    引用全局变量: 无
    输入参数说明: 无
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/31    3.0         胡昌威       创建
====================================================================*/
BOOL32 CMcuVcInst::HasJoinedChairman( void )
{	
	if( !m_tConf.HasChairman() )
	{
		return  FALSE;
	}
    TMt		tChairman = m_tConf.GetChairman();
    return m_tConfAllMtInfo.MtJoinedConf( tChairman.GetMtId() );
}
/*==============================================================================
函数名    :  IsSelectedbyOtherMt
功能      :  Mt是否被选看
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-3-18					薛亮							create
==============================================================================*/
BOOL32 CMcuVcInst::IsSelectedbyOtherMt(u8 bySrcMtId,u8 byMode)
{
	BOOL32 bSelected = FALSE;
	TMtStatus tMtStatus;
	TMt tSelectedMt;
	for(u8 byLoop =1 ; byLoop<= MAXNUM_CONF_MT;byLoop++) 
	{
		if(m_tConfAllMtInfo.MtJoinedConf( byLoop ))
		{
			m_ptMtTable->GetMtStatus(byLoop,&tMtStatus);
			//tSelectedMt = tMtStatus.GetVideoMt(); //虽然语义对，但是最初置选看源，用的是SetSelectMt，所以用下句
			tSelectedMt = tMtStatus.GetSelectMt( byMode ); //获取选看源
			if(!tSelectedMt.IsNull() && tSelectedMt.GetMtId() == bySrcMtId)
			{
				bSelected = TRUE;
				break;
			}
		}
	}
	return bSelected;

}
/*=============================================================================
函 数 名： GetMixMtNumInGrp
功    能： 得到混音通道内终端个数
算法实现： 
全局变量： 
参    数： void
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/8/4    4.0			许世林                  创建
=============================================================================*/
u8 CMcuVcInst::GetMixMtNumInGrp( void )
{
    u8 byMixMemberNum = 0;
    if ( mcuNoMix == m_tConf.m_tStatus.GetMixerMode() )
    {
        return 0;
    }

    for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
    {
        if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) && m_ptMtTable->IsMtInMixGrp( byLoop )  )            
        {
            byMixMemberNum++;
        }
    }

	return byMixMemberNum;
}

/*=============================================================================
函 数 名： GetMaxMixNum
功    能： 获取会议支持的最大混音个数
算法实现： 
全局变量： 
参    数： void
返 回 值： u8  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/8/19  4.0			许世林                  创建
=============================================================================*/
u8  CMcuVcInst::GetMaxMixNum(void)
{
    return (m_tConf.GetConfAttrb().IsSatDCastMode() ? MAXNUM_MIXERCHNNL_SATDCONF : MAXNUM_MIXER_CHNNL);
}

/*=============================================================================
    函 数 名： NtfMcsMcuCurListAllConf
    功    能： 通知会控当前MCU的会议信息
    算法实现： 
    全局变量： 
    参    数： CServMsg &cServMsg
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/01/05  4.0			万春雷                  创建
    2006/06/22  4.0         顾振华                  支持用户组
=============================================================================*/
BOOL32 CMcuVcInst::NtfMcsMcuCurListAllConf( CServMsg &cServMsg )
{
	TConfInfo  *ptConfInfo = NULL;
	TConfMtTable  tConfMtTable;
	TConfAllMtInfo tConfAllMtInfo;
	TConfEqpModule tConfEqpModule;
    
    cServMsg.SetNullConfId();

	for (u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
	{
		TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
		if (tMapData.IsTemUsed())
		{
			TTemplateInfo tTemConf;
			if (g_cMcuVcApp.GetTemplate(byConfIdx, tTemConf))
			{
				g_cMcuVcApp.TemInfo2Msg(tTemConf, cServMsg);
				SendOneMsgToMcs(cServMsg, MCU_MCS_TEMSCHCONFINFO_NOTIF);
			}
		}

		if (tMapData.IsValidConf())
		{
			CMcuVcInst *pcMcuVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
			if (NULL == pcMcuVcInst)
			{
				continue;
			}

			//依次发送每个会议的通知            
			if (NULL != g_cMcuVcApp.GetConfInstHandle(byConfIdx))
			{
				ptConfInfo = &g_cMcuVcApp.GetConfInstHandle(byConfIdx)->m_tConf;

				//获取信息成功，通知会议控制台
				cServMsg.SetConfId( ptConfInfo->GetConfId() );
				cServMsg.SetConfIdx( byConfIdx );                

				if (ptConfInfo->m_tStatus.IsScheduled())
				{
					g_cMcuVcApp.ConfInfoMsgPack(pcMcuVcInst, cServMsg);

					cServMsg.SetConfIdx(byConfIdx);  
					cServMsg.SetConfId(ptConfInfo->GetConfId());

					SendOneMsgToMcs(cServMsg, MCU_MCS_TEMSCHCONFINFO_NOTIF);
				}
				else
				{
					cServMsg.SetMsgBody((u8 *)ptConfInfo, sizeof(TConfInfo));

					SendOneMsgToMcs(cServMsg, MCU_MCS_CONFINFO_NOTIF);

					//若为VCS会议，则需同时发出vcs特殊状态
					if (VCS_CONF == ptConfInfo->GetConfSource())
					{
						cServMsg.SetMsgBody((u8* )&pcMcuVcInst->m_cVCSConfStatus, sizeof(CBasicVCCStatus));
						SendOneMsgToMcs(cServMsg, MCU_VCS_CONFSTATUS_NOTIF);
					}

					//发给会控终端信息表
					if (g_cMcuVcApp.GetConfAllMtInfo(byConfIdx, &tConfAllMtInfo))
					{
						cServMsg.SetConfId(ptConfInfo->GetConfId());
						cServMsg.SetConfIdx(byConfIdx);
						cServMsg.SetMsgBody((u8 *)&tConfAllMtInfo, sizeof(TConfAllMtInfo));
						SendOneMsgToMcs(cServMsg, MCU_MCS_CONFALLMTINFO_NOTIF);
					}

					//发给会控终端表
					if (!g_cMcuVcApp.GetConfMtTable(byConfIdx, &tConfMtTable))
					{
						continue;
					}
					//本Mcu
					TMcu tMcu;
					cServMsg.SetConfId(ptConfInfo->GetConfId());
					cServMsg.SetConfIdx(byConfIdx);
					tMcu.SetMcu(LOCAL_MCUID);
					cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
					cServMsg.CatMsgBody((u8 *)tConfMtTable.m_atMtExt, 
					                     tConfMtTable.m_byMaxNumInUse * sizeof(TMtExt));
					SendOneMsgToMcs(cServMsg, MCU_MCS_MTLIST_NOTIF);

					//其它Mcu
					for (u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++)
					{
						if (tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].IsNull())
						{
							continue;
						}
						u8 byMcuId = tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId();
						TConfMcInfo *ptInfo = pcMcuVcInst->m_ptConfOtherMcTable->GetMcInfo(byMcuId);
						if (NULL == ptInfo)
						{
							continue;
						}
						tMcu.SetMcu(tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId());
                        
                        // guzh [5/18/2007] 这里不在实例中，要访问上级MCU，采用tConfAllMtInfo.m_tMMCU
                        if ( !g_cMcuVcApp.IsShowMMcuMtList() && 
                            !tConfAllMtInfo.m_tMMCU.IsNull() && tConfAllMtInfo.m_tMMCU.GetMtId() == byMcuId )
                        {
                            // guzh [4/30/2007] 上级MCU列表过滤
                            continue;
                        }
                        

						cServMsg.SetMsgBody((u8*)&tMcu, sizeof(tMcu));
						for (s32 nLoop = 0; nLoop < MAXNUM_CONF_MT; nLoop++)
						{
							if (ptInfo->m_atMtStatus[nLoop].IsNull() || 
								0 == ptInfo->m_atMtStatus[nLoop].GetMtId()) //自己
							{
								continue;
							}
							cServMsg.CatMsgBody((u8 *)&(ptInfo->m_atMtExt[nLoop]), sizeof(TMtExt));
						}
						SendOneMsgToMcs(cServMsg, MCU_MCS_MTLIST_NOTIF);
					}
				}
			}
		}
	}
	
	//回应答
	SendOneMsgToMcs(cServMsg, MCU_MCS_LISTALLCONF_ACK);

	return TRUE;
}

/*=============================================================================
    函 数 名： SendOneMsgToMcs
    功    能： 发送一条消息给会控 所有MCS或指定会控
    算法实现： 
    全局变量： 
    参    数： CServMsg &cServMsg, 
	           u16 wEvent
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/01/05  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::SendOneMsgToMcs( CServMsg &cServMsg, u16 wEvent )
{
	if (IsMcCmdFromThisMcu(cServMsg))
	{
		SendReplyBack(cServMsg, wEvent);
		
	}
	else
	{
		SendMsgToAllMcs(wEvent, cServMsg);
	}
	return TRUE;
}


/*=============================================================================
    函 数 名： GetInstState
    功    能： 主备数据倒换时 获取 实例状态机 状态
    算法实现： 
    全局变量： 
    参    数： [OUT]  u8 &byState //STATE_IDLE STATE_SCHEDULED STATE_ONGOING
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::GetInstState( u8 &byState )
{
	byState = (u8)CurState();

	return TRUE;
}

/*=============================================================================
    函 数 名： SetInstState
    功    能： 主备数据倒换时 恢复 实例状态机 状态
	           同时同步公用区数据
    算法实现： 
    全局变量： 
    参    数： [OUT]  u8 byState //STATE_IDLE STATE_SCHEDULED STATE_ONGOING
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::SetInstState( u8 byState )
{
    //  xsl [3/3/2006] STATE_WAITEQP 为中间状态，不同步
	if (byState > STATE_ONGOING || STATE_WAITEQP == byState)
	{
		return FALSE;
	}

	if (STATE_IDLE == byState)
	{
		//若原有实例状态机不为STATE_IDLE，则释放会议
		if (STATE_IDLE != CurState())
		{
			//在外面已统一清除
			//g_cMcuVcApp.RemoveConf( m_byConfIdx, FALSE );

			DeleteAlias();
			
			//清空会议实例
			ClearVcInst();
		}
	}
	else
	{
		//若原有实例状态机为 STATE_IDLE 或 STATE_WAITFOR，则保存会议
        //说明：STATE_IDLE    为固有状态
        //      STATE_WAITEQP 为等待mp上线时即时会议下附带强置的状态
		if (STATE_IDLE == CurState() || STATE_WAITEQP == CurState())
		{
			//分配内存
			if (NULL == m_ptMtTable)
			{
				m_ptMtTable = new TConfMtTable;
				if (NULL == m_ptMtTable)
				{
					return FALSE;
				}
			}
			if (STATE_ONGOING == byState)
			{
				if (NULL == m_ptSwitchTable)
				{
					m_ptSwitchTable = new TConfSwitchTable;
					if (NULL == m_ptSwitchTable)
					{
						MCU_SAFE_DELETE(m_ptMtTable)
						return FALSE;
					}
				}
				if (NULL == m_ptConfOtherMcTable)
				{
					m_ptConfOtherMcTable = new TConfOtherMcTable;
					if (NULL == m_ptConfOtherMcTable)
					{
						MCU_SAFE_DELETE(m_ptMtTable)
						MCU_SAFE_DELETE(m_ptSwitchTable)
						return FALSE;
					}
				}
			}
		}
		
		g_cMcuVcApp.AddConf( this, FALSE );
	}
	NEXTSTATE( byState );

	return TRUE;
}

/*=============================================================================
    函 数 名： HangupInstTimer
    功    能： 主备数据倒换时 暂停 轮循、预约会议等会议定时器（非STATE_IDLE实例）
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::HangupInstTimer( void )
{
	if (STATE_IDLE == CurState())
	{
		return FALSE;
	}
	return TRUE;
}

/*=============================================================================
    函 数 名： ResumeInstTimer
    功    能： 主备数据倒换时 恢复 轮循、预约会议等会议定时器 （非STATE_IDLE实例）
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::ResumeInstTimer( void )
{
	if (STATE_IDLE == CurState())
	{
		return FALSE;
	}
	return TRUE;
}

/*=============================================================================
    函 数 名： GetConfMtTableData
    功    能： 主备数据倒换时 获取 会议终端表 信息
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [OUT]     u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::GetConfMtTableData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (STATE_IDLE == CurState() || NULL == m_ptMtTable)
	{
		return FALSE;
	}
	if (NULL == pbyBuf || dwInBufLen < sizeof(TConfMtTable))
	{
		return FALSE;
	}
	//可对终端列表进行pack处理，
	memcpy(pbyBuf, (s8*)m_ptMtTable, sizeof(TConfMtTable));
	dwOutBufLen = sizeof(TConfMtTable);

	return TRUE;
}

/*=============================================================================
    函 数 名： SetTConfMtTableData
    功    能： 主备数据倒换时 恢复 会议终端表 信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::SetConfMtTableData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (STATE_IDLE == CurState() || NULL == m_ptMtTable)
	{
		return FALSE;
	}
	//允许大于---	
	if (NULL == pbyBuf || dwInBufLen < sizeof(TConfMtTable)) //!= sizeof(TConfMtTable))
	{
		return FALSE;
	}
	//可对终端列表进行pack处理，
	memcpy((s8*)m_ptMtTable, pbyBuf, sizeof(TConfMtTable));

	return TRUE;
}

/*=============================================================================
    函 数 名： GetConfSwitchTableData
    功    能： 主备数据倒换时 获取 会议交换表 信息
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [OUT]     u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::GetConfSwitchTableData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (STATE_IDLE == CurState() || NULL == m_ptSwitchTable)
	{
		return FALSE;
	}
	if (NULL == pbyBuf || dwInBufLen < sizeof(TConfSwitchTable))
	{
		return FALSE;
	}
	//可对列表进行pack处理，
	memcpy(pbyBuf, (s8*)m_ptSwitchTable, sizeof(TConfSwitchTable));
	dwOutBufLen = sizeof(TConfSwitchTable);

	return TRUE;
}

/*=============================================================================
    函 数 名： SetConfSwitchTableData
    功    能： 主备数据倒换时 恢复 会议交换表 信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::SetConfSwitchTableData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (STATE_IDLE == CurState() || NULL == m_ptSwitchTable)
	{
		return FALSE;
	}
	if (NULL == pbyBuf || dwInBufLen < sizeof(TConfSwitchTable))
	{
		return FALSE;
	}
	//可对列表进行pack处理，
	memcpy((s8*)m_ptSwitchTable, pbyBuf, sizeof(TConfSwitchTable));

	return TRUE;
}

/*=============================================================================
    函 数 名： GetConfOtherMcTableData
    功    能： 主备数据倒换时 获取 其它Mc终端列表 信息
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [OUT]     u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::GetConfOtherMcTableData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (STATE_IDLE == CurState() || NULL == m_ptConfOtherMcTable)
	{
		return FALSE;
	}
	if (NULL == pbyBuf || dwInBufLen < (sizeof(TMSVcOtherMcuState)+sizeof(TConfOtherMcTable)))
	{
		return FALSE;
	}

	TMSVcOtherMcuState *ptOtherMcuState = (TMSVcOtherMcuState*)pbyBuf;
	u8 *pbyMcuDataBuf = pbyBuf+sizeof(TMSVcOtherMcuState);
	u8  byOtherMcuNum = 0;
	memset(pbyBuf, 0, sizeof(TMSVcOtherMcuState));
	
	for (u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++)
	{
		if (0 == m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop].m_byMcuId)
		{
			ptOtherMcuState->m_abyOnline[byLoop] = 0;
		}
		else
		{
			//可对终端列表进行pack处理，
			ptOtherMcuState->m_abyOnline[byLoop] = 1;
			memcpy(pbyMcuDataBuf, (u8*)(&m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]), sizeof(TConfMcInfo));
			pbyMcuDataBuf += sizeof(TConfMcInfo);

			byOtherMcuNum += 1;
		}
	}
	
	dwOutBufLen = sizeof(TMSVcOtherMcuState) + byOtherMcuNum*sizeof(TConfMcInfo);

	return TRUE;
}

/*=============================================================================
    函 数 名： SetConfOtherMcTableData
    功    能： 主备数据倒换时 恢复 其它Mc终端列表 信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::SetConfOtherMcTableData( u8 *pbyBuf, u32 dwInBufLen )
{
	if (STATE_IDLE == CurState() || NULL == m_ptConfOtherMcTable)
	{
		return FALSE;
	}
	if (NULL == pbyBuf || dwInBufLen < sizeof(TMSVcOtherMcuState))
	{
		return FALSE;
	}

	TMSVcOtherMcuState *ptOtherMcuState = (TMSVcOtherMcuState*)pbyBuf;
	u8 *pbyMcuDataBuf = pbyBuf+sizeof(TMSVcOtherMcuState);
	
	//可对终端列表进行pack处理，
	for (u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++)
	{
		if (1 == ptOtherMcuState->m_abyOnline[byLoop])
		{
			memcpy((s8*)(&m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]), pbyMcuDataBuf, sizeof(TConfMcInfo));
			pbyMcuDataBuf += sizeof(TConfMcInfo);	
		}
		else
		{
			m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop].SetNull();
		}
	}
	
	return TRUE;
}

/*=============================================================================
    函 数 名： GetInstOtherData
    功    能： 主备数据倒换时 获取 除上述动态分配的列表信息外的其他需同步 信息
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
			   [OUT]     u32 &dwOutBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::GetInstOtherData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen )
{
	if (STATE_IDLE == CurState())
	{
		return FALSE;
	}
	
	dwOutBufLen  = 0;
	u32 dwOutLen = OprInstOtherData(pbyBuf, dwInBufLen, TRUE);
	if (0 == dwOutLen)
	{
		return FALSE;
	}

	dwOutBufLen = dwOutLen;
	return TRUE;
}

/*=============================================================================
    函 数 名： SetInstOtherData
    功    能： 主备数据倒换时 恢复 除上述动态分配的列表信息外的其他需同步 信息
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 *pbyBuf
               [IN]  u32 dwInBufLen
               [IN]  BOOL32 bResumeTimer
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::SetInstOtherData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bResumeTimer )
{
	if (STATE_IDLE == CurState())
	{
		return FALSE;
	}

	if (0 == OprInstOtherData(pbyBuf, dwInBufLen, FALSE))
	{
		return FALSE;
	}

	//是否恢复会议实例涉及的定时器，只在第一次主备倒换时恢复
	if (bResumeTimer)
	{
		//即时会议 或者 预约会议
		if (STATE_ONGOING == CurState())
		{
			//恢复 即时会议自动结束时钟
			if (0 != m_tConf.GetDuration())
			{
				KillTimer(MCUVC_ONGOING_CHECK_TIMER);
				SetTimer(MCUVC_ONGOING_CHECK_TIMER, TIMESPACE_ONGOING_CHECK);
			}

			//恢复 自动呼叫终端时钟
			KillTimer(MCUVC_INVITE_UNJOINEDMT_TIMER);
			SetTimer(MCUVC_INVITE_UNJOINEDMT_TIMER, 1000*m_tConf.m_tStatus.GetCallInterval());
			
			//恢复 MCU视频源变更检测时钟
			KillTimer(MCUVC_MCUSRC_CHECK_TIMER);
			SetTimer(MCUVC_MCUSRC_CHECK_TIMER, TIMESPACE_MCUSRC_CHECK);
			
			//恢复 VMP主席更随变更检测时钟
			if (0)
			{
				KillTimer(MCUVC_CHANGE_VMPCHAN_CHAIRMAN_TIMER);
				SetTimer(MCUVC_CHANGE_VMPCHAN_CHAIRMAN_TIMER, TIMESPACE_CHANGEVMPPARAM, VMP_MEMBERTYPE_CHAIRMAN);
			}
			
			//恢复 VMP发言人更随变更检测时钟
			if (0)
			{
				KillTimer(MCUVC_CHANGE_VMPCHAN_SPEAKER_TIMER);
				SetTimer(MCUVC_CHANGE_VMPCHAN_SPEAKER_TIMER, TIMESPACE_CHANGEVMPPARAM, VMP_MEMBERTYPE_SPEAKER);
			}

			if (POLL_STATE_NORMAL == m_tConf.m_tStatus.GetPollState())
			{
				KillTimer(MCUVC_POLLING_CHANGE_TIMER);
				SetTimer(MCUVC_POLLING_CHANGE_TIMER, 1000*1);
			}

            if(POLL_STATE_NORMAL == m_tConf.m_tStatus.GetTvWallPollState())
            {
                KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER);
                SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER, 1000*1);
            }
            
			// fxh
			if (VCS_POLL_START == m_cVCSConfStatus.GetChairPollState())
			{
				KillTimer(MCUVC_VCS_CHAIRPOLL_TIMER);
				SetTimer(MCUVC_VCS_CHAIRPOLL_TIMER, 1000*m_cVCSConfStatus.GetPollIntval());
			}

            // guzh [4/17/2007] 
            // 自动录像的定时器采用绝对定时
		}
		else
		{
			//恢复 预约会议检测时钟
			SetTimer( MCUVC_SCHEDULED_CHECK_TIMER, TIMESPACE_SCHEDULED_CHECK );
		}
	}

	return TRUE;
}

/*=============================================================================
    函 数 名： OprInstOtherData
    功    能： 主备数据倒换时 同步 除上述动态分配的列表信息外的其他需同步 数据
	           由于同步的主从MCU应用环境一致，同步的数据结构暂不考虑紧凑压栈问题：Pack Ingore
    算法实现： 
    全局变量： 
    参    数： [IN/OUT]  u8 *pbyBuf
               [IN]      u32 dwInBufLen
               [IN]      BOOL32 bGet TRUE －获取 FALSE - 恢复 
    返 回 值： u32  同步的数据总长度
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
u32 CMcuVcInst::OprInstOtherData( u8 *pbyBuf, u32 dwInBufLen, BOOL32 bGet )
{
	u32 dwOtherDataLen = GetInstOtherDataLen();
	//允许大于---
	if (NULL == pbyBuf || dwInBufLen < dwOtherDataLen)
	{
		return 0;
	}

	if (bGet)
	{
		memcpy(pbyBuf, &m_tConf, sizeof(m_tConf));
		pbyBuf += sizeof(m_tConf);
		memcpy(pbyBuf, &m_tConfAllMtInfo, sizeof(m_tConfAllMtInfo));
		pbyBuf += sizeof(m_tConfAllMtInfo);
		memcpy(pbyBuf, &m_tConfProtectInfo, sizeof(m_tConfProtectInfo));
		pbyBuf += sizeof(m_tConfProtectInfo);
		memcpy(pbyBuf, &m_tConfPollParam, sizeof(m_tConfPollParam));
		pbyBuf += sizeof(m_tConfPollParam);
		memcpy(pbyBuf, &m_tTvWallPollParam, sizeof(m_tTvWallPollParam));
		pbyBuf += sizeof(m_tTvWallPollParam);
		memcpy( pbyBuf, &m_tHduPollParam, sizeof(m_tHduPollParam) );  // add by jlb
        pbyBuf += sizeof(m_tHduPollParam);
		memcpy(pbyBuf, &m_tConfEqpModule, sizeof(m_tConfEqpModule));
		pbyBuf += sizeof(m_tConfEqpModule);
		memcpy(pbyBuf, &m_tVidBrdSrc, sizeof(m_tVidBrdSrc));
		pbyBuf += sizeof(m_tVidBrdSrc);
		memcpy(pbyBuf, &m_tAudBrdSrc, sizeof(m_tAudBrdSrc));
		pbyBuf += sizeof(m_tAudBrdSrc);
		memcpy(pbyBuf, &m_tDoubleStreamSrc, sizeof(m_tDoubleStreamSrc));
		pbyBuf += sizeof(m_tDoubleStreamSrc);
		memcpy(pbyBuf, &m_tH239TokenOwnerInfo, sizeof(m_tH239TokenOwnerInfo));
		pbyBuf += sizeof(m_tH239TokenOwnerInfo);
		
		memcpy(pbyBuf, &m_tVmpEqp, sizeof(m_tVmpEqp));
		pbyBuf += sizeof(m_tVmpEqp);
		memcpy(pbyBuf, &m_tVmpTwEqp, sizeof(m_tVmpTwEqp));
		pbyBuf += sizeof(m_tVmpTwEqp);
		memcpy(pbyBuf, &m_tMixEqp, sizeof(m_tMixEqp));
		pbyBuf += sizeof(m_tMixEqp);
		memcpy(pbyBuf, &m_tRecEqp, sizeof(m_tRecEqp));
		pbyBuf += sizeof(m_tRecEqp);
		memcpy(pbyBuf, &m_tPlayEqp, sizeof(m_tPlayEqp));
		pbyBuf += sizeof(m_tPlayEqp);
		memcpy(pbyBuf, &m_tPlayEqpAttrib, sizeof(m_tPlayEqpAttrib));
		pbyBuf += sizeof(m_tPlayEqpAttrib);
		memcpy(pbyBuf, &m_tRecPara, sizeof(m_tRecPara));
		pbyBuf += sizeof(m_tRecPara);
		memcpy(pbyBuf, &m_tAudBasEqp, sizeof(m_tAudBasEqp));
		pbyBuf += sizeof(m_tAudBasEqp);
		memcpy(pbyBuf, &m_tVidBasEqp, sizeof(m_tVidBasEqp));
		pbyBuf += sizeof(m_tVidBasEqp);
		memcpy(pbyBuf, &m_tBrBasEqp, sizeof(m_tBrBasEqp));
		pbyBuf += sizeof(m_tBrBasEqp);
		memcpy(pbyBuf, &m_tCasdAudBasEqp, sizeof(m_tCasdAudBasEqp));
		pbyBuf += sizeof(m_tCasdAudBasEqp);
		memcpy(pbyBuf, &m_tCasdVidBasEqp, sizeof(m_tCasdVidBasEqp));
		pbyBuf += sizeof(m_tCasdVidBasEqp);
		memcpy(pbyBuf, &m_tPrsEqp, sizeof(m_tPrsEqp));
		pbyBuf += sizeof(m_tPrsEqp);
		
		memcpy(pbyBuf, &m_byMixGrpId, sizeof(m_byMixGrpId));
		pbyBuf += sizeof(m_byMixGrpId);
		memcpy(pbyBuf, &m_byRecChnnl, sizeof(m_byRecChnnl));
		pbyBuf += sizeof(m_byRecChnnl);
		memcpy(pbyBuf, &m_byPlayChnnl, sizeof(m_byPlayChnnl));
		pbyBuf += sizeof(m_byPlayChnnl);
		memcpy(pbyBuf, &m_byPrsChnnl, sizeof(m_byPrsChnnl));
		pbyBuf += sizeof(m_byPrsChnnl);
		memcpy(pbyBuf, &m_byPrsChnnl2, sizeof(m_byPrsChnnl2));
		pbyBuf += sizeof(m_byPrsChnnl2);
		memcpy(pbyBuf, &m_byPrsChnnlAud, sizeof(m_byPrsChnnlAud));
		pbyBuf += sizeof(m_byPrsChnnlAud);
		memcpy(pbyBuf, &m_byPrsChnnlAudBas, sizeof(m_byPrsChnnlAudBas));
		pbyBuf += sizeof(m_byPrsChnnlAudBas);
		memcpy(pbyBuf, &m_byPrsChnnlVidBas, sizeof(m_byPrsChnnlVidBas));
		pbyBuf += sizeof(m_byPrsChnnlVidBas);
		memcpy(pbyBuf, &m_byPrsChnnlBrBas, sizeof(m_byPrsChnnlBrBas));
		pbyBuf += sizeof(m_byPrsChnnlBrBas);
		memcpy(pbyBuf, &m_byAudBasChnnl, sizeof(m_byAudBasChnnl));
		pbyBuf += sizeof(m_byAudBasChnnl);
		memcpy(pbyBuf, &m_byVidBasChnnl, sizeof(m_byVidBasChnnl));
		pbyBuf += sizeof(m_byVidBasChnnl);
		memcpy(pbyBuf, &m_byBrBasChnnl, sizeof(m_byBrBasChnnl));
		pbyBuf += sizeof(m_byBrBasChnnl);
		memcpy(pbyBuf, &m_byCasdAudBasChnnl, sizeof(m_byCasdAudBasChnnl));
		pbyBuf += sizeof(m_byCasdAudBasChnnl);
		memcpy(pbyBuf, &m_byCasdVidBasChnnl, sizeof(m_byCasdVidBasChnnl));
		pbyBuf += sizeof(m_byCasdVidBasChnnl);
		memcpy(pbyBuf, &m_wVidBasBitrate, sizeof(m_wVidBasBitrate));
		pbyBuf += sizeof(m_wVidBasBitrate);
		memcpy(pbyBuf, &m_wBasBitrate, sizeof(m_wBasBitrate));
		pbyBuf += sizeof(m_wBasBitrate);
		memcpy(pbyBuf, &m_awVMPBrdBitrate, sizeof(m_awVMPBrdBitrate));	// xliang [8/5/2009] 
		pbyBuf += sizeof(m_awVMPBrdBitrate);
        // guzh [8/28/2007] 其实MINIMCU 没有主备倒换，为防止以后遗忘
#ifdef _MINIMCU_
        memcpy(pbyBuf, &m_byIsDoubleMediaConf, sizeof(m_byIsDoubleMediaConf));
        pbyBuf += sizeof(m_byIsDoubleMediaConf);
#endif

//		memcpy(pbyBuf, &m_tBasCapSet, sizeof(m_tBasCapSet));
//		pbyBuf += sizeof(m_tBasCapSet);
				
//		memcpy(pbyBuf, &m_byRegGKDriId, sizeof(m_byRegGKDriId));
//		pbyBuf += sizeof(m_byRegGKDriId);

		memcpy(pbyBuf, &m_tRefreshParam, sizeof(m_tRefreshParam));
		pbyBuf += sizeof(m_tRefreshParam);
		memcpy(pbyBuf, &m_tVacLastSpeaker, sizeof(m_tVacLastSpeaker));
		pbyBuf += sizeof(m_tVacLastSpeaker);
		memcpy(pbyBuf, &m_tLastSpeaker, sizeof(m_tLastSpeaker));
		pbyBuf += sizeof(m_tLastSpeaker);      
        
		memcpy(pbyBuf, &m_tConfInStatus, sizeof(m_tConfInStatus));
		pbyBuf += sizeof(m_tConfInStatus);

		memcpy(pbyBuf, &m_tCascadeMMCU, sizeof(m_tCascadeMMCU));
		pbyBuf += sizeof(m_tCascadeMMCU);

		memcpy(pbyBuf, &m_dwSpeakerViewId, sizeof(m_dwSpeakerViewId));
		pbyBuf += sizeof(m_dwSpeakerViewId);
		memcpy(pbyBuf, &m_dwVmpViewId, sizeof(m_dwVmpViewId));
		pbyBuf += sizeof(m_dwVmpViewId);
		memcpy(pbyBuf, &m_dwSpeakerVideoId, sizeof(m_dwSpeakerVideoId));
		pbyBuf += sizeof(m_dwSpeakerVideoId);
		memcpy(pbyBuf, &m_dwVmpVideoId, sizeof(m_dwVmpVideoId));
		pbyBuf += sizeof(m_dwVmpVideoId);
		memcpy(pbyBuf, &m_dwSpeakerAudioId, sizeof(m_dwSpeakerAudioId));
		pbyBuf += sizeof(m_dwSpeakerAudioId);
		memcpy(pbyBuf, &m_dwMixerAudioId, sizeof(m_dwMixerAudioId));
		pbyBuf += sizeof(m_dwMixerAudioId);
		memcpy(pbyBuf, &m_abySerHdr, sizeof(m_abySerHdr));
		pbyBuf += sizeof(m_abySerHdr);

		memcpy(pbyBuf, &m_byConfIdx, sizeof(m_byConfIdx));
		pbyBuf += sizeof(m_byConfIdx);	
        memcpy(pbyBuf, &m_byDcsIdx, sizeof(m_byDcsIdx));
        pbyBuf += sizeof(m_byDcsIdx);

        memcpy(pbyBuf, m_abyCasChnCheckTimes, sizeof(m_abyCasChnCheckTimes));   // guzh [2/25/2007]
		pbyBuf += sizeof(m_abyCasChnCheckTimes);     

        memcpy(pbyBuf, &m_tLastSpyMt, sizeof(m_tLastSpyMt));
        pbyBuf += sizeof(m_tLastSpyMt);
		memcpy(pbyBuf, &m_tLastVmpParam, sizeof(m_tLastVmpParam));
		pbyBuf += sizeof(m_tLastVmpParam);
		memcpy(pbyBuf, &m_tLastVmpTwParam, sizeof(m_tLastVmpTwParam));
		pbyBuf += sizeof(m_tLastVmpTwParam);
        memcpy(pbyBuf, &m_tChargeSsnId, sizeof(m_tChargeSsnId));
        pbyBuf += sizeof(m_tChargeSsnId);

        memcpy(pbyBuf, &m_abyMixMtId, sizeof(m_abyMixMtId));        // zbq [03/19/2007]
        pbyBuf += sizeof(m_abyMixMtId);
        memcpy(pbyBuf, &m_byCreateBy, sizeof(m_byCreateBy));
        pbyBuf += sizeof(m_byCreateBy);

        memcpy(pbyBuf, &m_tPollSwitchParam, sizeof(m_tPollSwitchParam));    // guzh [7/21/2007] 
        pbyBuf += sizeof(m_tPollSwitchParam);
        memcpy(pbyBuf, &m_tRollCaller, sizeof(m_tRollCaller));
        pbyBuf += sizeof(m_tRollCaller);
        memcpy(pbyBuf, &m_tRollCallee, sizeof(m_tRollCallee));
        pbyBuf += sizeof(m_tRollCallee);

        memcpy(pbyBuf, &m_tLastMixParam, sizeof(m_tLastMixParam));
        pbyBuf += sizeof(m_tLastMixParam);

		memcpy(pbyBuf, &m_bySpeakerVidSrcSpecType, sizeof(m_bySpeakerVidSrcSpecType));	//zgc, 2008-04-16
		pbyBuf += sizeof(m_bySpeakerVidSrcSpecType);
		memcpy(pbyBuf, &m_bySpeakerAudSrcSpecType, sizeof(m_bySpeakerAudSrcSpecType));
		pbyBuf += sizeof(m_bySpeakerAudSrcSpecType);
    
        memcpy(pbyBuf, &m_cMtRcvGrp, sizeof(m_cMtRcvGrp));
        pbyBuf += sizeof(m_cMtRcvGrp);
        memcpy(pbyBuf, &m_cBasMgr, sizeof(m_cBasMgr));
        pbyBuf += sizeof(m_cBasMgr);

		memcpy(pbyBuf,&m_byLastDsSrcMtId,sizeof(m_byLastDsSrcMtId));  // xliang [12/19/2008] 双流抢发所需
		pbyBuf += sizeof(m_byLastDsSrcMtId);
		memcpy(pbyBuf,&m_tLogicChnnl,sizeof(m_tLogicChnnl));
		pbyBuf += sizeof(m_tLogicChnnl);

		memcpy(pbyBuf,&m_byMtIdNotInvite,sizeof(m_byMtIdNotInvite)); // xliang [12/26/2008] 
		pbyBuf += sizeof(m_byMtIdNotInvite);

		// xliang [5/6/2009] prs 新增的对应广播源4出的通道
		memcpy(pbyBuf,&m_byPrsChnnlVmpOut1,sizeof(u8)); 
		pbyBuf += sizeof(u8);
		memcpy(pbyBuf,&m_byPrsChnnlVmpOut2,sizeof(u8)); 
		pbyBuf += sizeof(u8);
		memcpy(pbyBuf,&m_byPrsChnnlVmpOut3,sizeof(u8)); 
		pbyBuf += sizeof(u8);
		memcpy(pbyBuf,&m_byPrsChnnlVmpOut4,sizeof(u8)); 
		pbyBuf += sizeof(u8);
		memcpy(pbyBuf, &m_tHduBatchPollInfo, sizeof(m_tHduBatchPollInfo) );    // add by jlb
		pbyBuf += sizeof(m_tHduBatchPollInfo);
		memcpy(pbyBuf, &m_tHduPollSchemeInfo, sizeof(m_tHduPollSchemeInfo) );
		pbyBuf += sizeof(m_tHduPollSchemeInfo);

		// xliang [5/12/2009] VMP related
		memcpy(pbyBuf, &m_tVmpChnnlInfo,sizeof(m_tVmpChnnlInfo)); 
		pbyBuf += sizeof(m_tVmpChnnlInfo);
		memcpy(pbyBuf, &m_atVMPTmpMember, sizeof(m_atVMPTmpMember));
		pbyBuf += sizeof(m_atVMPTmpMember);
		memcpy(pbyBuf, &m_bNewMemberSeize, sizeof(m_bNewMemberSeize));
		pbyBuf += sizeof(m_bNewMemberSeize);
		memcpy(pbyBuf, &m_tLastVmpChnnlMemInfo, sizeof(m_tLastVmpChnnlMemInfo));
		pbyBuf += sizeof(m_tLastVmpChnnlMemInfo);
		memcpy(pbyBuf, &m_tVmpBatchPollInfo, sizeof(m_tVmpBatchPollInfo));
		pbyBuf += sizeof(m_tVmpBatchPollInfo);
		memcpy(pbyBuf, &m_tVmpPollParam, sizeof(m_tVmpPollParam));
		pbyBuf += sizeof(m_tVmpPollParam);
		memcpy(pbyBuf, &m_byVmpOperating, sizeof(m_byVmpOperating));
		pbyBuf += sizeof(m_byVmpOperating);

		// VCS会议状态信息同步
		memcpy(pbyBuf, &m_cVCSConfStatus, sizeof(m_cVCSConfStatus));
		pbyBuf += sizeof(m_cVCSConfStatus);		
	}
	else
	{
		memcpy(&m_tConf, pbyBuf, sizeof(m_tConf));
		pbyBuf += sizeof(m_tConf);
		memcpy(&m_tConfAllMtInfo, pbyBuf, sizeof(m_tConfAllMtInfo));
		pbyBuf += sizeof(m_tConfAllMtInfo);

		//会控锁定信息不强制同步, 因为同一个mcs在主从mpc上的Ssnid可能不一样
		TConfProtectInfo *ptConfProtectInfo = (TConfProtectInfo *)pbyBuf;
		m_tConfProtectInfo.SetLockByMcu(ptConfProtectInfo->GetLockedMcuId());
		//memcpy(&m_tConfProtectInfo, pbyBuf, sizeof(m_tConfProtectInfo));
		//如果是会控锁定会议，根据主板会控状态中的SSRC搜索本地同ID的会控，如果存在，
		//用本地会控ID重添LockMcs，zgc, 2008-03-25
		if ( CONF_LOCKMODE_LOCK == m_tConf.m_tStatus.GetProtectMode() )
		{
			TMSVcMCState tMSVcMCState;
			g_cMSSsnApp.GetMSVcMCState( tMSVcMCState );

			if ( tMSVcMCState.m_abyOnline[ptConfProtectInfo->GetLockedMcSsnId()-1] != 1 )
			{
				// 对应Id的MCS没有连接到主MPC, 则取消会议锁定
				ConfLog( FALSE, "[OprInstOtherData][ERROR] Lock conf.%s mcs.%d isn't connecting active mcu, it's impossible!\n ",
							m_tConf.GetConfName(), ptConfProtectInfo->GetLockedMcSsnId() );
				m_tConf.m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );
			}
			else
			{
				u32 dwSSRC = tMSVcMCState.m_tMcsRegInfo[ptConfProtectInfo->GetLockedMcSsnId()-1].GetMcsSSRC();
				u8 byIdx = 0;
				for ( byIdx = 0; byIdx < MAXNUM_MCU_MC; byIdx++ )
				{
					if ( !g_cMcuVcApp.IsMcConnected( byIdx+1 ) )
					{
						continue;
					}
					TMcsRegInfo tMcsRegInfo;
					g_cMcuVcApp.GetMcsRegInfo( byIdx+1, &tMcsRegInfo );
					if ( dwSSRC == tMcsRegInfo.GetMcsSSRC() )
					{
						//设置lockmcs为本地会控ID
						m_tConfProtectInfo.SetLockByMcs( byIdx+1 );
						break;
					}
				}
				//没有找到同SSRC的MCS连接备MCU，则取消会议锁定
				if ( byIdx == MAXNUM_MCU_MC ) 
				{
					ConfLog( FALSE, "[OprInstOtherData][ERROR] Can't find mcs<SSRC.%d> connecting standby mcu, it's impossible!\n ",
							dwSSRC );
					m_tConf.m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );
				}
			}			
		}
		pbyBuf += sizeof(m_tConfProtectInfo);
		
		memcpy(&m_tConfPollParam, pbyBuf, sizeof(m_tConfPollParam));
		pbyBuf += sizeof(m_tConfPollParam);
		memcpy(&m_tTvWallPollParam, pbyBuf, sizeof(m_tTvWallPollParam));
		pbyBuf += sizeof(m_tTvWallPollParam);
        memcpy(&m_tHduPollParam, pbyBuf, sizeof(m_tHduPollParam));        // add by jlb
		pbyBuf += sizeof(m_tHduPollParam);
		memcpy(&m_tConfEqpModule, pbyBuf, sizeof(m_tConfEqpModule));
		pbyBuf += sizeof(m_tConfEqpModule);
		memcpy(&m_tVidBrdSrc, pbyBuf, sizeof(m_tVidBrdSrc));
		pbyBuf += sizeof(m_tVidBrdSrc);
		memcpy(&m_tAudBrdSrc, pbyBuf, sizeof(m_tAudBrdSrc));
		pbyBuf += sizeof(m_tAudBrdSrc);
		memcpy(&m_tDoubleStreamSrc, pbyBuf, sizeof(m_tDoubleStreamSrc));
		pbyBuf += sizeof(m_tDoubleStreamSrc);
		memcpy(&m_tH239TokenOwnerInfo, pbyBuf, sizeof(m_tH239TokenOwnerInfo));
		pbyBuf += sizeof(m_tH239TokenOwnerInfo);
		
		memcpy(&m_tVmpEqp, pbyBuf, sizeof(m_tVmpEqp));
		pbyBuf += sizeof(m_tVmpEqp);
		memcpy(&m_tVmpTwEqp, pbyBuf, sizeof(m_tVmpTwEqp));
		pbyBuf += sizeof(m_tVmpTwEqp);
		memcpy(&m_tMixEqp, pbyBuf, sizeof(m_tMixEqp));
		pbyBuf += sizeof(m_tMixEqp);
		memcpy(&m_tRecEqp, pbyBuf, sizeof(m_tRecEqp));
		pbyBuf += sizeof(m_tRecEqp);
		memcpy(&m_tPlayEqp, pbyBuf, sizeof(m_tPlayEqp));
		pbyBuf += sizeof(m_tPlayEqp);
		memcpy(&m_tPlayEqpAttrib, pbyBuf, sizeof(m_tPlayEqpAttrib));
		pbyBuf += sizeof(m_tPlayEqpAttrib);
		memcpy(&m_tRecPara, pbyBuf, sizeof(m_tRecPara));
		pbyBuf += sizeof(m_tRecPara);
		memcpy(&m_tAudBasEqp, pbyBuf, sizeof(m_tAudBasEqp));
		pbyBuf += sizeof(m_tAudBasEqp);
		memcpy(&m_tVidBasEqp, pbyBuf, sizeof(m_tVidBasEqp));
		pbyBuf += sizeof(m_tVidBasEqp);
		memcpy(&m_tBrBasEqp, pbyBuf, sizeof(m_tBrBasEqp));
		pbyBuf += sizeof(m_tBrBasEqp);
		memcpy(&m_tCasdAudBasEqp, pbyBuf, sizeof(m_tCasdAudBasEqp));
		pbyBuf += sizeof(m_tCasdAudBasEqp);
		memcpy(&m_tCasdVidBasEqp, pbyBuf, sizeof(m_tCasdVidBasEqp));
		pbyBuf += sizeof(m_tCasdVidBasEqp);
		memcpy(&m_tPrsEqp, pbyBuf, sizeof(m_tPrsEqp));
		pbyBuf += sizeof(m_tPrsEqp);
		
		memcpy(&m_byMixGrpId, pbyBuf, sizeof(m_byMixGrpId));
		pbyBuf += sizeof(m_byMixGrpId);
		memcpy(&m_byRecChnnl, pbyBuf, sizeof(m_byRecChnnl));
		pbyBuf += sizeof(m_byRecChnnl);
		memcpy(&m_byPlayChnnl, pbyBuf, sizeof(m_byPlayChnnl));
		pbyBuf += sizeof(m_byPlayChnnl);
		memcpy(&m_byPrsChnnl, pbyBuf, sizeof(m_byPrsChnnl));
		pbyBuf += sizeof(m_byPrsChnnl);
		memcpy(&m_byPrsChnnl2, pbyBuf, sizeof(m_byPrsChnnl2));
		pbyBuf += sizeof(m_byPrsChnnl2);
		memcpy(&m_byPrsChnnlAud, pbyBuf, sizeof(m_byPrsChnnlAud));
		pbyBuf += sizeof(m_byPrsChnnlAud);
		memcpy(&m_byPrsChnnlAudBas, pbyBuf, sizeof(m_byPrsChnnlAudBas));
		pbyBuf += sizeof(m_byPrsChnnlAudBas);
		memcpy(&m_byPrsChnnlVidBas, pbyBuf, sizeof(m_byPrsChnnlVidBas));
		pbyBuf += sizeof(m_byPrsChnnlVidBas);
		memcpy(&m_byPrsChnnlBrBas, pbyBuf, sizeof(m_byPrsChnnlBrBas));
		pbyBuf += sizeof(m_byPrsChnnlBrBas);
		memcpy(&m_byAudBasChnnl, pbyBuf, sizeof(m_byAudBasChnnl));
		pbyBuf += sizeof(m_byAudBasChnnl);
		memcpy(&m_byVidBasChnnl, pbyBuf, sizeof(m_byVidBasChnnl));
		pbyBuf += sizeof(m_byVidBasChnnl);
		memcpy(&m_byBrBasChnnl, pbyBuf, sizeof(m_byBrBasChnnl));
		pbyBuf += sizeof(m_byBrBasChnnl);
		memcpy(&m_byCasdAudBasChnnl, pbyBuf, sizeof(m_byCasdAudBasChnnl));
		pbyBuf += sizeof(m_byCasdAudBasChnnl);
		memcpy(&m_byCasdVidBasChnnl, pbyBuf, sizeof(m_byCasdVidBasChnnl));
		pbyBuf += sizeof(m_byCasdVidBasChnnl);
		memcpy(&m_wVidBasBitrate, pbyBuf, sizeof(m_wVidBasBitrate));
		pbyBuf += sizeof(m_wVidBasBitrate);
		memcpy(&m_wBasBitrate, pbyBuf, sizeof(m_wBasBitrate));
		pbyBuf += sizeof(m_wBasBitrate);
		memcpy(&m_awVMPBrdBitrate, pbyBuf, sizeof(m_awVMPBrdBitrate));
		pbyBuf += sizeof(m_awVMPBrdBitrate);
#ifdef _MINIMCU_
        memcpy(&m_byIsDoubleMediaConf, pbyBuf, sizeof(m_byIsDoubleMediaConf));
        pbyBuf += sizeof(m_byIsDoubleMediaConf);
#endif
//		memcpy(&m_tBasCapSet, pbyBuf, sizeof(m_tBasCapSet));
//		pbyBuf += sizeof(m_tBasCapSet);
		
//		memcpy(&m_byRegGKDriId, pbyBuf, sizeof(m_byRegGKDriId));
//		pbyBuf += sizeof(m_byRegGKDriId);

        memcpy(&m_tRefreshParam, pbyBuf, sizeof(m_tRefreshParam));
		pbyBuf += sizeof(m_tRefreshParam);

		memcpy(&m_tVacLastSpeaker, pbyBuf, sizeof(m_tVacLastSpeaker));
		pbyBuf += sizeof(m_tVacLastSpeaker);
		memcpy(&m_tLastSpeaker, pbyBuf, sizeof(m_tLastSpeaker));
		pbyBuf += sizeof(m_tLastSpeaker);       

		memcpy(&m_tConfInStatus, pbyBuf, sizeof(m_tConfInStatus));
		pbyBuf += sizeof(m_tConfInStatus);

		memcpy(&m_tCascadeMMCU, pbyBuf, sizeof(m_tCascadeMMCU));
		pbyBuf += sizeof(m_tCascadeMMCU);


		memcpy(&m_dwSpeakerViewId, pbyBuf, sizeof(m_dwSpeakerViewId));
		pbyBuf += sizeof(m_dwSpeakerViewId);
		memcpy(&m_dwVmpViewId, pbyBuf, sizeof(m_dwVmpViewId));
		pbyBuf += sizeof(m_dwVmpViewId);
		memcpy(&m_dwSpeakerVideoId, pbyBuf, sizeof(m_dwSpeakerVideoId));
		pbyBuf += sizeof(m_dwSpeakerVideoId);
		memcpy(&m_dwVmpVideoId, pbyBuf, sizeof(m_dwVmpVideoId));
		pbyBuf += sizeof(m_dwVmpVideoId);
		memcpy(&m_dwSpeakerAudioId, pbyBuf, sizeof(m_dwSpeakerAudioId));
		pbyBuf += sizeof(m_dwSpeakerAudioId);
		memcpy(&m_dwMixerAudioId, pbyBuf, sizeof(m_dwMixerAudioId));
		pbyBuf += sizeof(m_dwMixerAudioId);
		memcpy(&m_abySerHdr, pbyBuf, sizeof(m_abySerHdr));
		pbyBuf += sizeof(m_abySerHdr);

		memcpy(&m_byConfIdx, pbyBuf, sizeof(m_byConfIdx));
		pbyBuf += sizeof(m_byConfIdx);
        memcpy(&m_byDcsIdx, pbyBuf, sizeof(m_byDcsIdx));
        pbyBuf += sizeof(m_byDcsIdx);
        
        memcpy(m_abyCasChnCheckTimes, pbyBuf, sizeof(m_abyCasChnCheckTimes));   // guzh [2/25/2007]
		pbyBuf += sizeof(m_abyCasChnCheckTimes);        
        
        memcpy(&m_tLastSpyMt, pbyBuf, sizeof(m_tLastSpyMt));
        pbyBuf += sizeof(m_tLastSpyMt);
		memcpy(&m_tLastVmpParam, pbyBuf, sizeof(m_tLastVmpParam));
		pbyBuf += sizeof(m_tLastVmpParam);
		memcpy(&m_tLastVmpTwParam, pbyBuf, sizeof(m_tLastVmpTwParam));
		pbyBuf += sizeof(m_tLastVmpTwParam);
        memcpy(&m_tChargeSsnId, pbyBuf, sizeof(m_tChargeSsnId));
        pbyBuf += sizeof(m_tChargeSsnId);
        memcpy(&m_abyMixMtId, pbyBuf, sizeof(m_abyMixMtId));
        pbyBuf += sizeof(m_abyMixMtId);
        memcpy(&m_byCreateBy, pbyBuf, sizeof(m_byCreateBy));
        pbyBuf += sizeof(m_byCreateBy);
        
        memcpy(&m_tPollSwitchParam, pbyBuf, sizeof(m_tPollSwitchParam));    // guzh [7/21/2007] 
        pbyBuf += sizeof(m_tPollSwitchParam);
        memcpy(&m_tRollCaller, pbyBuf, sizeof(m_tRollCaller));
        pbyBuf += sizeof(m_tRollCaller);
        memcpy(&m_tRollCallee, pbyBuf, sizeof(m_tRollCallee));
        pbyBuf += sizeof(m_tRollCallee);

        memcpy(&m_tLastMixParam, pbyBuf, sizeof(m_tLastMixParam));
        pbyBuf += sizeof(m_tLastMixParam);

		memcpy(&m_bySpeakerVidSrcSpecType, pbyBuf, sizeof(m_bySpeakerVidSrcSpecType));	//zgc, 2008-04-16
		pbyBuf += sizeof(m_bySpeakerVidSrcSpecType);
		memcpy(&m_bySpeakerAudSrcSpecType, pbyBuf, sizeof(m_bySpeakerAudSrcSpecType));
		pbyBuf += sizeof(m_bySpeakerAudSrcSpecType);

        memcpy(&m_cMtRcvGrp, pbyBuf, sizeof(m_cMtRcvGrp));
		pbyBuf += sizeof(m_cMtRcvGrp);
        memcpy(&m_cBasMgr, pbyBuf, sizeof(m_cBasMgr));
        pbyBuf += sizeof(m_cBasMgr);

		memcpy(&m_byLastDsSrcMtId, pbyBuf,sizeof(m_byLastDsSrcMtId));	// xliang [12/19/2008] 双流抢发所需
		pbyBuf += sizeof(m_byLastDsSrcMtId);
		memcpy(&m_tLogicChnnl, pbyBuf,sizeof(m_tLogicChnnl));
		pbyBuf += sizeof(m_tLogicChnnl);

		memcpy(&m_byMtIdNotInvite,pbyBuf,sizeof(m_byMtIdNotInvite)); // xliang [12/26/2008] 
		pbyBuf += sizeof(m_byMtIdNotInvite);

		// xliang [5/6/2009] prs 新增的对应广播源4出的通道
		memcpy(&m_byPrsChnnlVmpOut1, pbyBuf, sizeof(u8));
		pbyBuf += sizeof(u8);
		memcpy(&m_byPrsChnnlVmpOut2, pbyBuf, sizeof(u8));
		pbyBuf += sizeof(u8);
		memcpy(&m_byPrsChnnlVmpOut3, pbyBuf, sizeof(u8));
		pbyBuf += sizeof(u8);
		memcpy(&m_byPrsChnnlVmpOut4, pbyBuf, sizeof(u8));
		pbyBuf += sizeof(u8);

		memcpy(&m_tHduBatchPollInfo, pbyBuf, sizeof(m_tHduBatchPollInfo) );    // add by jlb
		pbyBuf += sizeof(m_tHduBatchPollInfo);
		memcpy(&m_tHduPollSchemeInfo, pbyBuf, sizeof(m_tHduPollSchemeInfo) );
		pbyBuf += sizeof(m_tHduPollSchemeInfo);
		
		//vmp related
		memcpy( &m_tVmpChnnlInfo, pbyBuf,sizeof(m_tVmpChnnlInfo)); 
		pbyBuf += sizeof(m_tVmpChnnlInfo);
		memcpy(&m_atVMPTmpMember, pbyBuf, sizeof(m_atVMPTmpMember));
		pbyBuf += sizeof(m_atVMPTmpMember);
		memcpy(&m_bNewMemberSeize, pbyBuf, sizeof(m_bNewMemberSeize));
		pbyBuf += sizeof(m_bNewMemberSeize);
		memcpy(&m_tLastVmpChnnlMemInfo, pbyBuf, sizeof(m_tLastVmpChnnlMemInfo));
		pbyBuf += sizeof(m_tLastVmpChnnlMemInfo);
		memcpy(&m_tVmpBatchPollInfo, pbyBuf, sizeof(m_tVmpBatchPollInfo));
		pbyBuf += sizeof(m_tVmpBatchPollInfo);
		memcpy(&m_tVmpPollParam, pbyBuf, sizeof(m_tVmpPollParam));
		pbyBuf += sizeof(m_tVmpPollParam);
		memcpy(&m_byVmpOperating, pbyBuf, sizeof(m_byVmpOperating));
		pbyBuf += sizeof(m_byVmpOperating);

		memcpy(&m_cVCSConfStatus, pbyBuf, sizeof(m_cVCSConfStatus));
		pbyBuf += sizeof(m_cVCSConfStatus);
	}

	return dwOtherDataLen;
}

/*=============================================================================
    函 数 名： GetInstOtherDataLen
    功    能： 主备数据倒换时 获取 除上述动态分配的列表信息外的其他需同步 数据总长度
	           由于同步的主从MCU应用环境一致，同步的数据结构暂不考虑紧凑压栈问题：Pack Ingore
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： u32  数据总长度
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/18  4.0			万春雷                  创建
=============================================================================*/
u32 CMcuVcInst::GetInstOtherDataLen( void )
{
	u32 dwOtherDataLen = 0;

	//10
	dwOtherDataLen += sizeof(m_tConf);
	dwOtherDataLen += sizeof(m_tConfAllMtInfo);
	dwOtherDataLen += sizeof(m_tConfProtectInfo);
	dwOtherDataLen += sizeof(m_tConfPollParam);
	dwOtherDataLen += sizeof(m_tTvWallPollParam);
	dwOtherDataLen += sizeof(m_tHduPollParam);      //add by jlb
	dwOtherDataLen += sizeof(m_tConfEqpModule);
	dwOtherDataLen += sizeof(m_tVidBrdSrc);
	dwOtherDataLen += sizeof(m_tAudBrdSrc);
	dwOtherDataLen += sizeof(m_tDoubleStreamSrc);
	dwOtherDataLen += sizeof(m_tH239TokenOwnerInfo);
	
	//13
	dwOtherDataLen += sizeof(m_tVmpEqp);
	dwOtherDataLen += sizeof(m_tVmpTwEqp);
	dwOtherDataLen += sizeof(m_tMixEqp);
	dwOtherDataLen += sizeof(m_tRecEqp);
	dwOtherDataLen += sizeof(m_tPlayEqp);
	dwOtherDataLen += sizeof(m_tPlayEqpAttrib);
	dwOtherDataLen += sizeof(m_tRecPara);
	dwOtherDataLen += sizeof(m_tAudBasEqp);
	dwOtherDataLen += sizeof(m_tVidBasEqp);
	dwOtherDataLen += sizeof(m_tBrBasEqp);
	dwOtherDataLen += sizeof(m_tCasdAudBasEqp);
	dwOtherDataLen += sizeof(m_tCasdVidBasEqp);
	dwOtherDataLen += sizeof(m_tPrsEqp);
	
	//17
	dwOtherDataLen += sizeof(m_byMixGrpId);
	dwOtherDataLen += sizeof(m_byRecChnnl);
	dwOtherDataLen += sizeof(m_byPlayChnnl);
	dwOtherDataLen += sizeof(m_byPrsChnnl);
	dwOtherDataLen += sizeof(m_byPrsChnnl2);
	dwOtherDataLen += sizeof(m_byPrsChnnlAud);
	dwOtherDataLen += sizeof(m_byPrsChnnlAudBas);
	dwOtherDataLen += sizeof(m_byPrsChnnlVidBas);
	dwOtherDataLen += sizeof(m_byPrsChnnlBrBas);
	dwOtherDataLen += sizeof(m_byAudBasChnnl);
	dwOtherDataLen += sizeof(m_byVidBasChnnl);
	dwOtherDataLen += sizeof(m_byBrBasChnnl);
	dwOtherDataLen += sizeof(m_byCasdAudBasChnnl);
	dwOtherDataLen += sizeof(m_byCasdVidBasChnnl);
	dwOtherDataLen += sizeof(m_wVidBasBitrate);
	dwOtherDataLen += sizeof(m_wBasBitrate);
	dwOtherDataLen += sizeof(m_awVMPBrdBitrate);
#ifdef _MINIMCU_
    dwOtherDataLen += sizeof(m_byIsDoubleMediaConf);
#endif
//	dwOtherDataLen += sizeof(m_tBasCapSet);
	
	//14
//	dwOtherDataLen += sizeof(m_byRegGKDriId);

    dwOtherDataLen += sizeof(m_tRefreshParam);

	dwOtherDataLen += sizeof(m_tVacLastSpeaker);
	dwOtherDataLen += sizeof(m_tLastSpeaker);
	dwOtherDataLen += sizeof(m_tConfInStatus);
	dwOtherDataLen += sizeof(m_tCascadeMMCU);
	
	//12
	dwOtherDataLen += sizeof(m_dwSpeakerViewId);
	dwOtherDataLen += sizeof(m_dwVmpViewId);
	dwOtherDataLen += sizeof(m_dwSpeakerVideoId);
	dwOtherDataLen += sizeof(m_dwVmpVideoId);
	dwOtherDataLen += sizeof(m_dwSpeakerAudioId);
	dwOtherDataLen += sizeof(m_dwMixerAudioId);
	dwOtherDataLen += sizeof(m_abySerHdr);
    
	dwOtherDataLen += sizeof(m_byConfIdx);
    dwOtherDataLen += sizeof(m_byDcsIdx);
    
    dwOtherDataLen += sizeof(m_abyCasChnCheckTimes);    // guzh [2/25/2007]
    dwOtherDataLen += sizeof(m_tLastSpyMt);
	dwOtherDataLen += sizeof(m_tLastVmpParam);
	dwOtherDataLen += sizeof(m_tLastVmpTwParam);
    dwOtherDataLen += sizeof(m_tChargeSsnId);
    dwOtherDataLen += sizeof(m_abyMixMtId);
    dwOtherDataLen += sizeof(m_byCreateBy);

    dwOtherDataLen += sizeof(m_tPollSwitchParam);       // guzh [7/21/2007] 
    dwOtherDataLen += sizeof(m_tRollCaller);
    dwOtherDataLen += sizeof(m_tRollCallee);
    dwOtherDataLen += sizeof(m_tLastMixParam);

	dwOtherDataLen += sizeof(m_bySpeakerVidSrcSpecType);	// zgc, 2008-04-17
	dwOtherDataLen += sizeof(m_bySpeakerAudSrcSpecType);

    dwOtherDataLen += sizeof(m_cMtRcvGrp);
    dwOtherDataLen += sizeof(m_cBasMgr);

	dwOtherDataLen += sizeof(m_byLastDsSrcMtId);		// xliang [12/19/2008] 双流抢发所需
	dwOtherDataLen += sizeof(m_tLogicChnnl);
	
	dwOtherDataLen += sizeof(m_byMtIdNotInvite);		// xliang [12/26/2008]
	
	dwOtherDataLen += sizeof(m_byPrsChnnlVmpOut1);
	dwOtherDataLen += sizeof(m_byPrsChnnlVmpOut2);
	dwOtherDataLen += sizeof(m_byPrsChnnlVmpOut3);
	dwOtherDataLen += sizeof(m_byPrsChnnlVmpOut4);
	dwOtherDataLen += sizeof(m_tHduBatchPollInfo);        // add by jlb
	dwOtherDataLen += sizeof(m_tHduPollSchemeInfo);       // add by jlb

	dwOtherDataLen += sizeof(m_tVmpChnnlInfo);
	dwOtherDataLen += sizeof(m_atVMPTmpMember);
	dwOtherDataLen += sizeof(m_bNewMemberSeize);
	dwOtherDataLen += sizeof(m_tLastVmpChnnlMemInfo);
	dwOtherDataLen += sizeof(m_tVmpBatchPollInfo);
	dwOtherDataLen += sizeof(m_tVmpPollParam);
	dwOtherDataLen += sizeof(m_byVmpOperating);

	dwOtherDataLen += sizeof(m_cVCSConfStatus);
	
	return dwOtherDataLen;
}

/*=============================================================================
  函 数 名： ProcWaitEqpToCreateConf
  功    能： 等待外设上线创会
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/02/25  4.0			刘辉云                  创建
=============================================================================*/
void CMcuVcInst::ProcWaitEqpToCreateConf(CMessage * const pcMsg)
{
    if ( STATE_WAITEQP != CurState() &&
		 STATE_PERIOK != CurState() )
    {
        return;
    }
	TConfStore tConfStoreBuf;
	TPackConfStore *ptPackConfStore = (TPackConfStore *)&tConfStoreBuf;		
	u16 wAliasBufLen = 0;
	u8* pbyAliasBuf  = NULL; // 终端别名数组打包缓冲
	u8* pbyModuleBuf = NULL; // 电视墙//画面合成模板缓冲
	u8 byInsID = 0;

	u8 byConfIdx = (u8)(*(u32*)pcMsg->content);
	// 提取后的 tConfStore 为 TPackConfStore 结构，即已进行Pack处理的会议数据
	BOOL32 bRet = g_cMcuVcApp.HasConfStore( byConfIdx-MIN_CONFIDX );
    BOOL32 bRet1  =  ::GetConfFromFile(byConfIdx-MIN_CONFIDX, ptPackConfStore);
	if (bRet && bRet1)
	{
		wAliasBufLen = htons(ptPackConfStore->m_wAliasBufLen);
		pbyAliasBuf  = (u8 *)(ptPackConfStore+1);
		pbyModuleBuf = pbyAliasBuf+ptPackConfStore->m_wAliasBufLen;

        // guzh [4/11/2007] 标记从文件创会
        ptPackConfStore->m_tConfInfo.m_tStatus.SetTakeFromFile(TRUE);
		
		CServMsg cServMsg;
		cServMsg.SetConfIdx( (u8)byConfIdx ); // confidx
		cServMsg.SetMsgBody((u8*)&ptPackConfStore->m_tConfInfo, sizeof(TConfInfo));
		cServMsg.CatMsgBody((u8*)&wAliasBufLen, 2 );
		cServMsg.CatMsgBody(pbyAliasBuf, ntohs(wAliasBufLen) );

		// 电视墙模板 
		if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
		{
			cServMsg.CatMsgBody(pbyModuleBuf, sizeof(TMultiTvWallModule));
			pbyModuleBuf += sizeof(TMultiTvWallModule);
		}
		// 画面合成模板 
		if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasVmpModule())
		{
			cServMsg.CatMsgBody(pbyModuleBuf, sizeof(TVmpModule));
			pbyModuleBuf += sizeof(TVmpModule);
		}
		
        ::OspPost(MAKEIID( AID_MCU_VC, GetInsID() ), MCU_CREATECONF_FROMFILE, 
                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

	}
	McsLog("[Conf%d] Wait for eqp to create conference: %s\n", 
            GetInsID(), tConfStoreBuf.m_tConfInfo.GetConfName() );

	return;
}

/*=============================================================================
  函 数 名： DaemonProcMcuRegGKFailedNtf
  功    能： Mcu 重新注册GK 通知
  算法实现： 
  全局变量： 
  参    数： const CApp* pApp
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::DaemonProcMcuReRegisterGKNtf( const CMessage * pcMsg )
{
    m_tConfInStatus.SetRegGkNackNtf(TRUE);
    return;
}

/*=============================================================================
  函 数 名： DaemonProcPowerOn
  功    能： 实例初始化
  算法实现： 
  全局变量： 
  参    数： const CApp* pApp
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::DaemonProcPowerOn( const CMessage * pcMsg )
{
	if (!g_cMcuVcApp.CreateTemplate())
	{
		OspPrintf(TRUE, FALSE, "[ProcReoPowerOn] allocate memory failed for conf template!!!\n");
		return;
	}

	TConfStore tConfStoreBuf;
	TPackConfStore *ptPackConfStore = (TPackConfStore *)&tConfStoreBuf;
	u8 byConfStoreNum = 0;

    // guzh [4/10/2007] 要从老的MakeTimee恢复,保证CConfId的历史唯一性
    u32 dwOldConfIdMakeTimes = 0;
    u32 dwTmp;
	u8 byLoop = 0;
    CConfId cConfId;
	for (byLoop = 0; byLoop < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); byLoop++)
	{
		//提取后的 tConfStore 为 TPackConfStore 结构，即已进行Pack处理的会议数据
        if (::GetConfFromFile(byLoop, ptPackConfStore))
		{
			byConfStoreNum++;
			g_cMcuVcApp.SetConfStore(byLoop, TRUE);	
            
            cConfId = ptPackConfStore->m_tConfInfo.GetConfId();
            dwTmp = g_cMcuVcApp.GetMakeTimesFromConfId(cConfId);
            if (dwTmp > dwOldConfIdMakeTimes) 
            {
                dwOldConfIdMakeTimes = dwTmp;
            }
		}
	}
    g_cMcuVcApp.SetConfIdMakeTimes(dwOldConfIdMakeTimes);

    // xsl [11/28/2006] N+1 备份模式不进行会议恢复 
    if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_IDLE ||
        g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_SWITCH)
    {        
        return;
    }
    
	//若有需自动恢复的会议Idx列表（包括模板），则启动MP检测定时器
	if( byConfStoreNum > 0 )
	{
        // guzh [4/13/2007] 设置标记表示是MCU启动第一次恢复
		SetTimer(MCUVC_WAIT_MPREG_TIMER, TIMESPACE_WAIT_MPREG, TRUE);
	}

	//卫星会议信息 定时刷新启动
	SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );

	//MCU MODEM注册
	for (byLoop =1 ; byLoop <= g_byMcuModemNum ; byLoop ++ )
	{
		TTransportAddr tAddr;;
		tAddr.SetIpAddr(g_atMcuModemTopo[byLoop-1].m_dwMcuModemIp );
		tAddr.SetPort( g_atMcuModemTopo[byLoop-1].m_wMcuModemPort );
		
		TMt tMt;
		tMt.SetNull();	//MCU控制的modem

		u8 byType = g_atMcuModemTopo[byLoop-1].m_byMcuModemType;

		CServMsg cServMsg;
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
		cServMsg.CatMsgBody( (u8*)&tAddr, sizeof(TTransportAddr) );
		cServMsg.CatMsgBody( &byType, sizeof(u8) );
		g_cModemSsnApp.SendMsgToModemSsn( byLoop, MODEM_MCU_REG_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	}

	return;
}

/*=============================================================================
函 数 名： GetNPlusConfData
功    能： 从会议信息获取N+1备份信息
算法实现： 
全局变量： 
参    数： TNPlusConfData &tConfData
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/30  4.0			许世林                  创建
=============================================================================*/
void CMcuVcInst::ProcNPlusConfDataUpdate(const CMessage * pcMsg)
{
    if ( CurState() != STATE_ONGOING )
    {
        return;
    }

    // N+1备份，不支持预约会议的备份倒换 [12/20/2006-zbq]
    if ( m_tConf.m_tStatus.IsScheduled() )
    {
        ConfLog( FALSE, "[ProcNPlusConfDataUpdate] conf.%s will not be backup as a scheduled conf !\n",
                         m_tConf.GetConfName() );
        return;
    }
    
    TNPlusConfData tConfData;
    GetNPlusDataFromConf( tConfData );

    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_CONFDATAUPDATE_REQ);
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8*)&tConfData, sizeof(tConfData));    
    
    //若为主mcu则srcid为daemon实例，若为备份mcu回滚模式则srcid为备份实例
    OspPost(pcMsg->srcid, VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

    //若为回滚模式，即源实例不是daemon，结束备份会议
    if (GETINS(pcMsg->srcid) != CInstance::DAEMON)
    {
        NPlusLog("[ProcNPlusConfDataUpdate] conf:%s rollback and release\n", m_tConf.GetConfName());
        ReleaseConf(FALSE);        
        NEXTSTATE( STATE_IDLE );
    }
    return;
}

/*=============================================================================
函 数 名： GetNPlusDataFromConf
功    能： 从会议获得N+1备份所需信息
算法实现： 
全局变量： 
参    数： TNPlusConfData &tNPlusData
           BOOL32          bCharge   :针对计费同步信息的处理，若为计费同步，则
                                      不关心 专门针对N+1的处理部分。否则，会导
                                      致时序问题。影响N+1的备份结果。zbq [03/27/2007]

返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/12/26  4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::GetNPlusDataFromConf( TNPlusConfData &tConfData, BOOL32 bCharge )
{
    memcpy(&tConfData.m_tConf, &m_tConf, sizeof(TConfInfo));
    
    TMtAlias tTmpAlias;
    TMt tTmpMt = m_tConf.GetChairman();    
    if (!tTmpMt.IsNull())
    {
        if (!m_ptMtTable->GetMtAlias(tTmpMt.GetMtId(), mtAliasTypeTransportAddress, &tTmpAlias))
        {
            ConfLog(FALSE, "[GetNPlusDataFromConf] get chairman ip addr alias failed.\n");            
        }        
    }
    tConfData.m_tConf.SetChairAlias(tTmpAlias);
    
    tTmpAlias.SetNull();
    tTmpMt = m_tConf.GetSpeaker();
    if (!tTmpMt.IsNull())
    {
        if (!m_ptMtTable->GetMtAlias(tTmpMt.GetMtId(), mtAliasTypeTransportAddress, &tTmpAlias))
        {
            ConfLog(FALSE, "[GetNPlusDataFromConf] get speaker ip addr alias failed.\n");            
        }       
    }
    tConfData.m_tConf.SetSpeakerAlias(tTmpAlias);
    
    u8 byIdx = 0;
    u16 wDialBitrate;
    for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
    {
        //只保存与会且主动邀请的终端
        if (!m_tConfAllMtInfo.MtJoinedConf(byMtId) || m_ptMtTable->IsNotInvited(byMtId))
        {
            continue;
        }
        
        if (m_ptMtTable->GetMtAlias(byMtId, mtAliasTypeTransportAddress, &tTmpAlias))
        {
            tConfData.m_atMtInConf[byIdx].SetMtAddr(tTmpAlias.m_tTransportAddr);
            wDialBitrate = m_ptMtTable->GetDialBitrate(byMtId);
            tConfData.m_atMtInConf[byIdx].SetCallBitrate(wDialBitrate);            
            byIdx++;
        }
        else
        {
            ConfLog(FALSE, "[GetNPlusDataFromConf] get mt.%d ip addr alias failed.\n", byMtId);
        }
        tConfData.m_byMtNum = byIdx;
    }
    
    if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE)
    {
        BOOL32 bHasVMPMem = FALSE;
        
        TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
        for (byIdx = 0; byIdx < tVmpParam.GetMaxMemberNum(); byIdx++)
        {
            TVMPMember *ptMemMt = tVmpParam.GetVmpMember(byIdx);
            if (!m_tConfAllMtInfo.MtJoinedConf(ptMemMt->GetMtId()) || m_ptMtTable->IsNotInvited(ptMemMt->GetMtId()))
            {
                continue;
            }
            
            if (m_ptMtTable->GetMtAlias(ptMemMt->GetMtId(), mtAliasTypeTransportAddress, &tTmpAlias))
            {
                tConfData.m_tVmpInfo.m_atMtInVmp[byIdx].SetMtAddr(tTmpAlias.m_tTransportAddr); 
                tConfData.m_tVmpInfo.m_abyMemberType[byIdx] = ptMemMt->GetMemberType();
                
                bHasVMPMem = TRUE;
            }
            else
            {
                ConfLog(FALSE, "[GetNPlusDataFromConf] get mt.%d ip addr alias failed.\n", tTmpMt.GetMtId());
            }
        }
        // zbq [03/27/2007] 计费信息的同步，不关心N+1的特殊处理
        if ( !bCharge )
        {
            // 若画面合成当前没有成员，则不回滚 空画面合成模式 [12/14/2006-zbq]
            if ( !bHasVMPMem )
            {
                tConfData.m_tConf.m_tStatus.SetVMPMode( CONF_VMPMODE_NONE );
            }
            m_tConf.SetHasVmpModule( bHasVMPMem );
        }

        // 若画面合成模式为自动合成，则模板为空 [01/11/2007-zbq]
        if ( CONF_VMPMODE_AUTO == tConfData.m_tConf.m_tStatus.GetVMPMode() ) 
        {
            tConfData.m_tConf.SetHasVmpModule( FALSE );
        }
    }
    
    // 若会议计费，则加载计费SessionId [12/26/2006-zbq]
    if ( m_tConf.IsSupportGkCharge() )
    {
        memcpy(&tConfData.m_tSsnId, &m_tChargeSsnId, sizeof(m_tChargeSsnId));
    }
    
    return;
}

/*=============================================================================
函 数 名： ProcNPlusConfInfoUpdate
功    能： N+1备份会议信息同步
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
void CMcuVcInst::ProcNPlusConfInfoUpdate(BOOL32 bStart)
{
    u8 byMode = bStart ? NPLUS_CONF_START : NPLUS_CONF_RELEASE;
    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_CONFINFOUPDATE_REQ);
    cServMsg.SetMsgBody(&byMode, sizeof(byMode));
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.CatMsgBody((u8*)&m_tConf, sizeof(m_tConf));
    g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

    return;
}

/*=============================================================================
函 数 名： ProcNPlusConfMtInfoUpdate
功    能： N+1备份终端信息同步
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
void CMcuVcInst::ProcNPlusConfMtInfoUpdate(void)
{
    TMtInfo atMtInConf[MAXNUM_CONF_MT];
    u8 byIdx = 0;
    u16 wDialBitrate;
    TMtAlias tTmpAlias;

    for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
    {
        //只保存 已经加入会议的非上级MCU 终端 [12/20/2006-zbq]
        if ( !m_tConfAllMtInfo.MtJoinedConf(byMtId) || 
             MT_TYPE_MMCU == m_ptMtTable->GetMt(byMtId).GetMtType() )
        {
            continue;
        }

        if (m_ptMtTable->GetMtAlias(byMtId, mtAliasTypeTransportAddress, &tTmpAlias))
        {          
            atMtInConf[byIdx].SetMtAddr(tTmpAlias.m_tTransportAddr);
            wDialBitrate = m_ptMtTable->GetDialBitrate(byMtId);
            atMtInConf[byIdx].SetCallBitrate(wDialBitrate);            
            byIdx++;
        }
        else
        {
            ConfLog(FALSE, "[ProcNPlusConfMtInfoUpdate] get mt.%d ip addr alias failed.\n", byMtId);
        }
    }

    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_CONFMTUPDATE_REQ);
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8*)atMtInConf, sizeof(TMtInfo)*byIdx);
    g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

    return;
}

/*=============================================================================
函 数 名： ProcNPlusChairmanUpdate
功    能： N+1备份主席同步
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
void CMcuVcInst::ProcNPlusChairmanUpdate(void)
{
    TMtAlias tTmpAlias;
    TMt tTmpMt = m_tConf.GetChairman();  
    if (!tTmpMt.IsNull())
    {
        if (!m_ptMtTable->GetMtAlias(tTmpMt.GetMtId(), mtAliasTypeTransportAddress, &tTmpAlias))        
        {
            ConfLog(FALSE, "[ProcNPlusChairmanUpdate] get chairman ip addr alias failed.\n");
        }
    }

    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_CHAIRUPDATE_REQ);
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8*)&tTmpAlias, sizeof(tTmpAlias));
    g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

    return;
}

/*=============================================================================
函 数 名： ProcNPlusSpeakerUpdate
功    能： N+1备份发言人同步
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
void CMcuVcInst::ProcNPlusSpeakerUpdate(void)
{
    TMtAlias tTmpAlias;
    TMt tTmpMt = m_tConf.GetSpeaker();  
    if (!tTmpMt.IsNull())
    {
        if (!m_ptMtTable->GetMtAlias(tTmpMt.GetMtId(), mtAliasTypeTransportAddress, &tTmpAlias))
        {
            ConfLog(FALSE, "[ProcNPlusSpeakerUpdate] get speaker ip addr alias failed.\n");
        }
    }

    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_SPEAKERUPDATE_REQ);
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8*)&tTmpAlias, sizeof(tTmpAlias));
    g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

    return;
}

/*=============================================================================
函 数 名： ProcNPlusVmpUpdate
功    能： N+1备份vmp信息同步
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
void CMcuVcInst::ProcNPlusVmpUpdate(void)
{ 
    u8 byIdx = 0;
    TMtAlias tTmpAlias;
    TNPlusVmpInfo tVmpInfo;  
    TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
    for (byIdx = 0; byIdx < tVmpParam.GetMaxMemberNum(); byIdx++)
    {
        TVMPMember *ptMemMt = tVmpParam.GetVmpMember(byIdx);
        if (!m_tConfAllMtInfo.MtJoinedConf(ptMemMt->GetMtId()) || m_ptMtTable->IsNotInvited(ptMemMt->GetMtId()))
        {
            continue;
        }

        if (m_ptMtTable->GetMtAlias(ptMemMt->GetMtId(), mtAliasTypeTransportAddress, &tTmpAlias))
        {
            tVmpInfo.m_atMtInVmp[byIdx].SetMtAddr(tTmpAlias.m_tTransportAddr);                        
            tVmpInfo.m_abyMemberType[byIdx] = ptMemMt->GetMemberType();
        }
        else
        {
            ConfLog(FALSE, "[ProcNPlusVmpUpdate] get mt.%d ip addr alias failed.\n", ptMemMt->GetMtId());
        }
    }

    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_VMPUPDATE_REQ);
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8*)&tVmpInfo, sizeof(tVmpInfo)); 
    cServMsg.CatMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
    g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

    return;
}

/*=============================================================================
  函 数 名： AdjustRecordSrcStream
  功    能： 录像机 录码流 调整
  算法实现： 
  全局变量： 
  参    数： u8    byMode   : MODE_VIDEO, MODE_AUDIO, MODE_SECVIDEO
  返 回 值： void 
  ---------------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/06/07    4.0			张宝卿                  创建
  2007/06/12    4.0         顾振华                  修改
=============================================================================*/
void CMcuVcInst::AdjustRecordSrcStream( u8 byMode )
{
    TMt tNewSrc;
    u8  byRecChnlIdx = 0;
    // 这里对高清会议暂时规避，以后需要增加判断是否需要高清适配适配, zgc, 2008-08-28
    BOOL32 bNeedBas = IsRecordSrcBas(byMode, tNewSrc, byRecChnlIdx);

    if (tNewSrc.IsNull())
    {
        return;
    }

    TSimCapSet tConfMainSCS = m_tConf.GetCapSupport().GetMainSimCapSet();   
    TSimCapSet tSrcMtSCS;
    if ( tNewSrc.GetType() == TYPE_MT )
    {
        tSrcMtSCS = m_ptMtTable->GetSrcSCS(tNewSrc.GetMtId());
    }
    else
    {
		// 2008-01-17, zgc, 双速双格式会议辅格式低速录象失败修改，从R4 MP4合并
        if (!bNeedBas)
        {
            // 直接默认主格式、主码率
            tSrcMtSCS.SetAudioMediaType( tConfMainSCS.GetAudioMediaType() );
            tSrcMtSCS.SetVideoMediaType( tConfMainSCS.GetVideoMediaType() );
            tSrcMtSCS.SetVideoResolution( tConfMainSCS.GetVideoResolution() );    
        }
        else
        {
            TMt tSpeaker = GetLocalSpeaker();
            if ( tSpeaker.GetType() == TYPE_MT )
            {
                tSrcMtSCS = m_ptMtTable->GetSrcSCS(tSpeaker.GetMtId());
            }
            else
            {
                // 对于放像器，直接默认主格式、主码率
                tSrcMtSCS.SetAudioMediaType( tConfMainSCS.GetAudioMediaType() );
                tSrcMtSCS.SetVideoMediaType( tConfMainSCS.GetVideoMediaType() );
                tSrcMtSCS.SetVideoResolution( tConfMainSCS.GetVideoResolution() );    
            }
        }
    }

    if ( MODE_AUDIO == byMode )
    {
        if ( bNeedBas )
        {
            if (m_tConf.m_tStatus.IsAudAdapting())
            {
                ChangeAdapt(ADAPT_TYPE_AUD, 0, &tConfMainSCS, &tSrcMtSCS);
                StartSwitchToPeriEqp( tNewSrc, m_byAudBasChnnl, 
                    m_tRecEqp.GetEqpId(), m_byRecChnnl, byMode);
            }
            else
            {
                StartAdapt(ADAPT_TYPE_AUD, 0, &tConfMainSCS, &tSrcMtSCS);
            }      
        }
        else if ( tNewSrc == m_tMixEqp )
        {
            // 会议讨论模式 录 混音器
            StartSwitchToPeriEqp(tNewSrc, m_byMixGrpId, m_tRecEqp.GetEqpId(),
                                 m_byRecChnnl, byMode);
        }
        else
        {
            u8 bySrcChan = (tNewSrc == m_tPlayEqp ? m_byPlayChnnl : 0);
            StartSwitchToPeriEqp(tNewSrc, bySrcChan, m_tRecEqp.GetEqpId(), 
                                 m_byRecChnnl, byMode);
        }
    }
    
    if ( MODE_VIDEO == byMode )
    {
        if ( bNeedBas)
        {
            if ( tNewSrc == m_tVidBasEqp )
            {
                
                if (m_tConf.m_tStatus.IsVidAdapting())
                {
					if( m_tRecPara.IsRecLowStream() )
					{
                        // guzh [6/18/2007] FIXME: 是否会被后面的changeadapt调高？
						ChangeAdapt(ADAPT_TYPE_VID, m_tConf.GetSecBitRate(), &tConfMainSCS, &tSrcMtSCS);
					}
                    StartSwitchToPeriEqp( m_tVidBasEqp, m_byVidBasChnnl, 
                                          m_tRecEqp.GetEqpId(), m_byRecChnnl, byMode );
                }
                else
                {
                    u16 wBitrate = m_tRecPara.IsRecLowStream() ? m_tConf.GetSecBitRate() : m_tConf.GetBitRate();
                    StartAdapt(ADAPT_TYPE_VID, wBitrate, &tConfMainSCS, &tSrcMtSCS);
                }
            }
            else if ( tNewSrc == m_tBrBasEqp )
            {
                if (m_tConf.m_tStatus.IsBrAdapting())
                {
                    ChangeAdapt(ADAPT_TYPE_BR, m_tConf.GetSecBitRate());
                    StartSwitchToPeriEqp( m_tBrBasEqp, m_byBrBasChnnl, 
                                          m_tRecEqp.GetEqpId(), m_byRecChnnl, byMode );                
                }
                else
                {
                    StartAdapt(ADAPT_TYPE_BR, m_tConf.GetSecBitRate());
                }
            }
			else
			{
				tNewSrc.SetConfIdx(m_byConfIdx);
				StartSwitchToPeriEqp(tNewSrc, byRecChnlIdx,
					                 m_tRecEqp.GetEqpId(), m_byRecChnnl, byMode);
			}
        }
        else
        {
            u8 bySrcChan = (tNewSrc == m_tPlayEqp ? m_byPlayChnnl : 0);
			if (tNewSrc == m_tVmpEqp)	// xliang [6/24/2009] 画面合成器srcChan不能填死为0
			{
				u8 byMediaType = m_tConf.GetMainVideoMediaType();
				u8 byRes = m_tConf.GetMainVideoFormat();
				bySrcChan = GetVmpOutChnnlByRes(byRes, m_tVmpEqp.GetEqpId(), byMediaType);
			}
            StartSwitchToPeriEqp(tNewSrc, bySrcChan, m_tRecEqp.GetEqpId(), 
                                 m_byRecChnnl, byMode);
        }
    }

    if ( MODE_SECVIDEO == byMode )
    {
        if (m_tRecPara.IsRecDStream() && !m_tDoubleStreamSrc.IsNull())
        {
            u8 bySrcChnnl = (m_tDoubleStreamSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
            StartSwitchToPeriEqp(m_tDoubleStreamSrc, bySrcChnnl, m_tRecEqp.GetEqpId(), m_byRecChnnl, MODE_SECVIDEO); 
        }
    }

    return;

}

/*=============================================================================
  函 数 名： NotifyMtFastUpdate
  功    能： 请求VCU，只能发送给指定终端
  算法实现： 
  全局变量： 
  参    数： byMode:      MODE_VIDEO 或 MODE_SECVIDEO
  返 回 值： void 
  ---------------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/07/27    4.0         顾振华                  修改
=============================================================================*/
void CMcuVcInst::NotifyMtFastUpdate( u8 byMtId, u8 byMode, BOOL32 bSetTimer )
{    
    u32 dwCurTick = OspTickGet();
	//xliang [080802] add print for vga ds test
	u32 dwCurTickInterval = dwCurTick - m_ptMtTable->GetLastTick(byMtId,(byMode==MODE_SECVIDEO));

	CallLog("[NotifyMtFastUpdate] the MtId is: %u, the video mode is: %u \n",byMtId,byMode);
	CallLog("[NotifyMtFastUpdate] the cur tick interval is: %u, the VCU interval is: %u\n",dwCurTickInterval,g_dwVCUInterval);
    if ( (byMode == MODE_VIDEO && dwCurTick - m_ptMtTable->GetLastTick(byMtId) > g_dwVCUInterval)
         || byMode == MODE_SECVIDEO )
    {
        CServMsg cServMsg;
        cServMsg.SetMsgBody(&byMode, sizeof(u8));
        
        //tandberg屏掉首次请求
        if(MT_MANU_TAIDE == m_ptMtTable->GetManuId(byMtId))
        {
            bSetTimer = TRUE;
            CallLog("[NotifyMtFastUpdate] the MtId.%u Fastupdate has been delayed due to MANU.Tandberg\n",byMtId);
        }
        else
        {
            SendMsgToMt(byMtId, MCU_MT_FASTUPDATEPIC_CMD, cServMsg);
        }
        m_ptMtTable->SetLastTick(byMtId, dwCurTick, (byMode==MODE_SECVIDEO));
    }
    // 设置定时器，等定时器到强制请求
    if (bSetTimer)
    {
        if (MODE_VIDEO == byMode)
        {
            SetTimer( MCUVC_FASTUPDATE_TIMER_ARRAY+byMtId, 1200, 100*byMtId+1 );
        }
        else if (MODE_SECVIDEO == byMode)
        {
            SetTimer( MCUVC_SECVIDEO_FASTUPDATE_TIMER, 1200, 100*byMtId+1 );
        }
        else
        {
            ConfLog( FALSE, "[NotifyMtFastUpdate] unexpected mode.%d, ignore it\n", byMode );
        }
    }
}

/*=============================================================================
  函 数 名： NotifyEqpFastUpdate
  功    能： 请求VCU，只能发给外设
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  ---------------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/07/27    4.0         顾振华                  修改
=============================================================================*/
void CMcuVcInst::NotifyEqpFastUpdate(const TMt &tDst, u8 byChnl, BOOL32 bSetTimer)
{
    CallLog("[NotifyEqpFastUpdate] the Peri is: %u, the chn is: %u !\n", tDst.GetEqpId(), byChnl);
    
    CServMsg cServMsg;
    u16 wEvent;
    
    u32 dwCurTick = OspTickGet();
    u32 dwLaskTick = 0;
    switch (tDst.GetEqpType())
    {
    case EQP_TYPE_VMP:
        {
            cServMsg.SetChnIndex(byChnl);
            wEvent = MCU_VMP_FASTUPDATEPIC_CMD;
            dwLaskTick = m_dwVmpLastVCUTick;
            if ( dwCurTick - dwLaskTick > g_dwVCUInterval )
            {
                m_dwVmpLastVCUTick = dwCurTick;
            }
            
            SendMsgToEqp(tDst.GetEqpId(), wEvent, cServMsg); 
            return;
            
            break;
        }
        
    case EQP_TYPE_VMPTW:
        wEvent = MCU_VMPTW_FASTUPDATEPIC_CMD;
        dwLaskTick = m_dwVmpTwLastVCUTick;
        if ( dwCurTick - dwLaskTick > g_dwVCUInterval )
        {
            m_dwVmpTwLastVCUTick = dwCurTick;
        }
        break;

    case EQP_TYPE_BAS:
        
        if ( !g_cMcuAgent.IsEqpBasHD( tDst.GetEqpId() ) )
        {
            cServMsg.SetChnIndex(byChnl);
            wEvent = MCU_BAS_FASTUPDATEPIC_CMD;
            dwLaskTick = GetBasLastVCUTick( byChnl );
            if ( dwCurTick - dwLaskTick > g_dwVCUInterval )
            {
                SetBasLastVCUTick( byChnl, dwCurTick );
            }
        }
        else
        {
            cServMsg.SetChnIndex(byChnl);
            wEvent = MCU_BAS_FASTUPDATEPIC_CMD;
            dwLaskTick = m_cBasMgr.GetChnVcuTick( tDst, byChnl );
            if ( dwCurTick - dwLaskTick > g_dwVCUInterval )
            {
                m_cBasMgr.SetChnVcuTick( tDst, byChnl, dwCurTick );
            }
        }
        break;
    case EQP_TYPE_RECORDER:
    default:
        return;
    }
    // 增加对 向外设发送请求关键帧命令的保护, zgc, 2008-04-21
    if ( dwCurTick - dwLaskTick > g_dwVCUInterval )
    {
        SendMsgToEqp(tDst.GetEqpId(), wEvent, cServMsg);  
    }

    // 设置定时器，等定时器到强制请求
    if (bSetTimer)
    {
        u8 byCount = 1;
        u32 dwParam = tDst.GetEqpId() << 16 | byChnl << 8 | byCount;
        SetTimer( MCUVC_FASTUPDATE_TIMER_ARRAY4EQP+tDst.GetEqpId()-MAXNUM_PERIEQP*3, 1200, dwParam );
    }
    return;
}


/*=============================================================================
  函 数 名： NotifyFastUpdate
  功    能： 请求VCU，发送给指定终端、外设
  算法实现： 
  全局变量： 
  参    数： byChnlMode     [in] 对于终端是 MODE_VIDEO 或 MODE_SECVIDEO
                                 对于BAS 是 通道号
                                 对于其他外设没有意义
  返 回 值： void 
  ---------------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/07/27    4.0         顾振华                  修改
=============================================================================*/
void CMcuVcInst::NotifyFastUpdate( const TMt &tDst, u8 byChnlMode, BOOL32 bSetTimer )
{
    if (tDst.GetType() == TYPE_MT)  // MT
    {
        NotifyMtFastUpdate(tDst.GetMtId(), byChnlMode, bSetTimer);
    }
    else    // Peri Eqp
    {
        NotifyEqpFastUpdate(tDst, byChnlMode, bSetTimer);
    }
}

/*=============================================================================
函 数 名： GetVidBrdSrc
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： TMt 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/7/23   4.0		周广程                  创建
=============================================================================*/
TMt CMcuVcInst::GetVidBrdSrc(void)
{
    return m_tVidBrdSrc;
}

/*=============================================================================
    函 数 名： RefreshRcvGrp
    功    能： 
    算法实现： byMtId = 0 刷新所有
               
    全局变量： 
    参    数： void
    返 回 值： TMt 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/11/20  4.5		    张宝卿                  创建
    2009/01/20  4.5         张宝卿                  修正：非适配会议皆进免适配区
=============================================================================*/
void CMcuVcInst::RefreshRcvGrp(u8 byMtId)
{
    if (byMtId > MAXNUM_CONF_MT)
    {
        ConfLog(FALSE, "[RefreshRcvGrp] unexpected mtid.%d\n", byMtId);
        return;
    }

    BOOL32 bConfNeedAdp = m_tConf.GetConfAttrb().IsUseAdapter();

    TSimCapSet tMVSrcSim;
    tMVSrcSim.Clear();
    GetMVBrdSrcSim(tMVSrcSim);

    TDStreamCap tDSSrcSim;
    tDSSrcSim.Clear();
    GetDSBrdSrcSim(tDSSrcSim);
    
    if(0 == byMtId)
    {
        u8 byIdx = 1;
        for(; byIdx <= MAXNUM_CONF_MT; byIdx++)
        {
            BOOL32 bMVNeedAdp = FALSE;
            BOOL32 bDSNeedAdp = FALSE;

            if (m_tConfAllMtInfo.MtJoinedConf(byIdx))
            {
                TCapSupport tMtCap;
                TSimCapSet tMtMVSim;
                TDStreamCap tMtDSSim;

                m_ptMtTable->GetMtCapSupport(byIdx, &tMtCap);
                
                //主流能力
                TLogicalChannel tLogicChn;
                if (!m_ptMtTable->GetMtLogicChnnl(byIdx, LOGCHL_VIDEO, &tLogicChn, TRUE))
                {
                    CallLog("[RefreshRcvGrp] Mt.%d's Forward chn isn't open yet!\n", byIdx);
                    continue;
                }

                tMtMVSim = tMtCap.GetMainSimCapSet();
                if (tMtMVSim.GetVideoMediaType() != tLogicChn.GetChannelType())
                {
                    tMtMVSim = tMtCap.GetSecondSimCapSet();
                    if (tMtMVSim.GetVideoMediaType() != tLogicChn.GetChannelType())
                    {
                        ConfLog(FALSE, "[RefreshRcvGrp] Mt.%d's Forward chn isn't accord to cap, check it!\n", byIdx);
                        continue;
                    }
                }

                //对应的能力描述可能是降分辨率处理的，对齐到通道
                if (tMtMVSim.GetVideoResolution() != tLogicChn.GetVideoFormat())
                {
                    tMtMVSim.SetVideoResolution(tLogicChn.GetVideoFormat());
                }

                //对应的能力的描述也可能实低码率呼叫的，对齐到前向通道
                if (tMtMVSim.GetVideoMaxBitRate() != tLogicChn.GetFlowControl())
                {
                    tMtMVSim.SetVideoMaxBitRate(tLogicChn.GetFlowControl());
                }

                //双流能力
                tMtDSSim = tMtCap.GetDStreamCapSet();

                if (bConfNeedAdp)
                {                    
                    bMVNeedAdp = tMtMVSim < tMVSrcSim ? TRUE : FALSE;
                    bDSNeedAdp = tMtDSSim < tDSSrcSim ? TRUE : FALSE;
                    
                    //zbq[02/06/2009] DS反向适配下强行保护H263p接收端进免适配区
                    if (tMtDSSim.GetMediaType() == MEDIA_TYPE_H263PLUS &&
                        tDSSrcSim.GetMediaType() == MEDIA_TYPE_H263PLUS )
                    {
                        bDSNeedAdp = FALSE;
                    }
                    //zbq[03/03/2009] 反向适配终端本身作源，过滤其到免适配区
                    if (bMVNeedAdp &&
                        MT_TYPE_MT == m_ptMtTable->GetMtType(m_tVidBrdSrc.GetMtId()) &&
                        m_tVidBrdSrc.GetMtId() == byIdx)
                    {
                        bMVNeedAdp = FALSE;
                    }
                }
                m_cMtRcvGrp.AddMem(byIdx, tMtMVSim, !bMVNeedAdp);
                m_cMtRcvGrp.AddMem(byIdx, tMtDSSim, !bDSNeedAdp);
            }
        }
        return;
    }

    if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
    {
        TCapSupport tMtCap;
        TSimCapSet tMtMVSim;
        TDStreamCap tMtDSSim;
        
        BOOL32 bMVExcept = TRUE;
        BOOL32 bDSExcept = TRUE;

        m_ptMtTable->GetMtCapSupport(byMtId, &tMtCap);

        //主流能力
        TLogicalChannel tLogicChn;
        if (!m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_VIDEO, &tLogicChn, TRUE))
        {
            CallLog("[RefreshRcvGrp] Mt.%d's Forward chn isn't open yet!\n", byMtId);
            return;
        }
        
        tMtMVSim = tMtCap.GetMainSimCapSet();
        if (tMtMVSim.GetVideoMediaType() != tLogicChn.GetChannelType())
        {
            tMtMVSim = tMtCap.GetSecondSimCapSet();
            if (tMtMVSim.GetVideoMediaType() != tLogicChn.GetChannelType())
            {
                ConfLog(FALSE, "[RefreshRcvGrp] Mt.%d's Forward chn isn't accord to cap, check it!\n", byMtId);
                return;
            }
        }
        
        //对应的能力描述可能是降分辨率处理的，对齐到通道
        if (tMtMVSim.GetVideoResolution() != tLogicChn.GetVideoFormat())
        {
            tMtMVSim.SetVideoResolution(tLogicChn.GetVideoFormat());
        }

        //双流能力
        tMtDSSim = tMtCap.GetDStreamCapSet();

        if (bConfNeedAdp)
        {
            bMVExcept = tMtMVSim < tMVSrcSim ? FALSE : TRUE;
            bDSExcept = tMtDSSim < tDSSrcSim ? FALSE : TRUE;

            //zbq[02/06/2009] DS反向适配下强行保护H263p接收端进免适配区
            if (tMtDSSim.GetMediaType() == MEDIA_TYPE_H263PLUS &&
                tDSSrcSim.GetMediaType() == MEDIA_TYPE_H263PLUS )
            {
                bDSExcept = TRUE;
            }
            //zbq[03/03/2009] 反向适配终端本身作源，过滤其到免适配区
            if (bMVExcept &&
                MT_TYPE_MT == m_ptMtTable->GetMtType(m_tVidBrdSrc.GetMtId()) &&
                m_tVidBrdSrc.GetMtId() == byMtId)
            {
                bMVExcept = TRUE;
            }

			//FIXME: merge
			if (m_tConf.m_tStatus.IsBrdstVMP())
			{
				bMVExcept = TRUE;
			}
        }
                
        m_cMtRcvGrp.AddMem(byMtId, tMtMVSim, bMVExcept);
        m_cMtRcvGrp.AddMem(byMtId, tMtDSSim, bDSExcept);
    }
    return;
}

/*=============================================================================
    函 数 名： RefreshBasParam4MVBrd
    功    能： 
    算法实现：根据当前广播源的能力整理适配参数，防止底图差异导致的填黑边的情况发生
    全局变量： 
    参    数： 
    返 回 值： TMt 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/05/05  4.5		    张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::RefreshBasParam4MVBrd()
{
    //刷新到初态
    m_cBasMgr.AssignBasChn(m_tConf, MODE_VIDEO);

    //源能力
    TSimCapSet tMVSrcSim;
    GetMVBrdSrcSim(tMVSrcSim);

    //双速会议不作处理
    if (m_tConf.GetSecBitRate() != 0)
    {
        return TRUE;
    }

    //调整比源能力大的输出参数
    TBasChn atBasChn[MAXNUM_CONF_MVCHN];
    u8 byNum = 0;
    m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_MVBRD);
    
    u8 byIdx = 0;
    for(; byIdx < byNum; byIdx ++)
    {
        THDBasVidChnStatus tStatus;
        memset(&tStatus, 0, sizeof(tStatus));
        
        TEqp tEqp = atBasChn[byIdx].GetEqp();
        u8 byChnId = atBasChn[byIdx].GetChnId();

        if (!m_cBasMgr.GetChnStatus(tEqp, byChnId, tStatus))
        {
            ConfLog(FALSE, "[RefreshBasParam4MVBrd] get eqp<%d, %d> status failed\n", 
                tEqp.GetEqpId(), byChnId);
            return FALSE;;
        }

        //主输出参数可能导致底图偏大
        THDAdaptParam *ptAdpParam = tStatus.GetOutputVidParam(0);
        if (NULL == ptAdpParam)
        {
            continue;
        }
        
        //跨格式适配 不需要调整底图
        if (tMVSrcSim.GetVideoMediaType() != ptAdpParam->GetVidType())
        {
            continue;
        }

        u8 byMVSrcRes = tMVSrcSim.GetVideoResolution();
        u16 wHeight = 0;
        u16 wWidth = 0;
        GetWHByRes(byMVSrcRes, wWidth, wHeight);
        
        if (0 == wHeight || 0 == wWidth)
        {
            continue;
        }
        
        if (ptAdpParam->GetHeight() <= wHeight &&
            ptAdpParam->GetWidth() <= wWidth)
        {
            continue;
        }
        
        ptAdpParam->SetResolution(wWidth, wHeight);
        
        if (!m_cBasMgr.UpdateChn(tEqp, byChnId, tStatus))
        {
            ConfLog(FALSE, "[RefreshBasParam4MVBrd] update status for Eqp.%d failed!\n", tEqp.GetEqpId());
            continue;
        }
    }

    return TRUE;
}

/*=============================================================================
    函 数 名： RefreshBasParam4MVBrd
    功    能： 
    算法实现：根据当前广播源的能力整理适配参数；
              主要用于mpu作适配资源的情况下，资源输公用资源，需要根据不同的源动态刷新
    全局变量： 
    参    数： 
    返 回 值：
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2009/05/05  4.5		    张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::RefreshBasParam4DSBrd()
{
    //恢复到初态
    m_cBasMgr.AssignBasChn(m_tConf, MODE_SECVIDEO);

    if (!m_cBasMgr.IsBrdGrpMpu())
    {
        return TRUE;
    }

    //源能力
    TDStreamCap tDSSrcSim;
    GetDSBrdSrcSim(tDSSrcSim);

    //非H263p作双流源，遵从初态即可
    if (MEDIA_TYPE_H263PLUS != tDSSrcSim.GetMediaType())
    {
        return TRUE;
    }

    //H263p作双流源，构造适配参数
    TBasChn atBasChn[MAXNUM_CONF_DSCHN];
    u8 byChnNum = 0;
    m_cBasMgr.GetChnGrp(byChnNum, atBasChn, CHN_ADPMODE_DSBRD);
    if (0 == byChnNum)
    {
        ConfLog(FALSE, "[RefreshBasParam4DSBrd] GetChnGrp failed!\n");
        return FALSE;
    }

    TPeriEqpStatus tEqpStatus;
    u8 byEqpId = atBasChn[0].GetEqpId();
    if (!g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tEqpStatus))
    {
        ConfLog(FALSE, "[RefreshBasParam4DSBrd] GetPeriEqpStatus.%d failed!\n", byEqpId);
        return FALSE;
    }

    if (atBasChn[0].GetChnId() >= MAXNUM_MPU_CHN)
    {
        ConfLog(FALSE, "[RefreshBasParam4DSBrd] atBasChn[0].GetChnId().%d failed!\n", atBasChn[0].GetChnId());
        return FALSE;
    }

    THDAdaptParam tHDAdpParam0;
    THDAdaptParam tHDAdpParam1;
    tHDAdpParam0.Reset();
    tHDAdpParam1.Reset();

    //H263p作源，输出 ConfRes/ConfFps 和 XGA/5fps
    
    //1。ConfRes/ConfFps
    tHDAdpParam0.SetVidType(MEDIA_TYPE_H264);
    tHDAdpParam0.SetBitRate(m_tConf.GetBitRate()*m_tConf.GetDStreamScale()/100);
    tHDAdpParam0.SetVidActiveType(GetActivePayload(m_tConf, MEDIA_TYPE_H264));
    
    tHDAdpParam0.SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());
    tHDAdpParam0.SetFrameRate(m_tConf.GetDStreamUsrDefFPS());
    
    u16 wWidth = 0;
    u16 wHeight = 0;
    GetWHByRes(m_tConf.GetDoubleVideoFormat(), wWidth, wHeight);
    tHDAdpParam0.SetResolution(wWidth, wHeight);

    //2。XGA/5fps
    tHDAdpParam0.SetVidType(MEDIA_TYPE_H264);
    tHDAdpParam0.SetBitRate(m_tConf.GetBitRate()*m_tConf.GetDStreamScale()/100);
    tHDAdpParam0.SetVidActiveType(GetActivePayload(m_tConf, MEDIA_TYPE_H264));
    
    tHDAdpParam0.SetIsNeedByPrs(m_tConf.GetConfAttrb().IsResendLosePack());
    u8 byDStreamFPS = 5;
    tHDAdpParam0.SetFrameRate(byDStreamFPS);
    
    wWidth = 0;
    wHeight = 0;
    GetWHByRes(VIDEO_FORMAT_XGA, wWidth, wHeight);
    tHDAdpParam0.SetResolution(wWidth, wHeight);
    

    THDBasVidChnStatus tVidStatus;
    memset(&tVidStatus, 0, sizeof(tVidStatus));
    tVidStatus = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(atBasChn[0].GetChnId());

    //参数一致，只挂0出
    if (VIDEO_FORMAT_XGA == m_tConf.GetDStreamUsrDefFPS() &&
        5 == m_tConf.GetDStreamUsrDefFPS())
    {
        tVidStatus.SetOutputVidParam(tHDAdpParam0, 0);
    }
    //0出挂Conf/Res，1出挂XGA/5fps
    else
    {
        tVidStatus.SetOutputVidParam(tHDAdpParam0, 0);
        tVidStatus.SetOutputVidParam(tHDAdpParam1, 1);
    }

    tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.SetVidChnStatus(tVidStatus, atBasChn[0].GetChnId());
 
    g_cMcuVcApp.SetPeriEqpStatus(atBasChn[0].GetEqpId(), &tEqpStatus);

    return TRUE;
}

/*=============================================================================
    函 数 名： RefreshBasParam
    功    能： 
    算法实现：
               
    全局变量： 
    参    数： BOOL32 bSet: 是否设置到bas里边去
    返 回 值： TMt 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/11/29  4.5		    张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::RefreshBasParam4AllMt(BOOL32 bSet, BOOL32 bDual)
{
    for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
    {
        if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
        {
            continue;
        }
        RefreshBasParam(byMtId, bSet, bDual);
    }
    return TRUE;
}

/*=============================================================================
    函 数 名： RefreshBasParam
    功    能： 
    算法实现：
               
    全局变量： 
    参    数： BOOL32 bSet: 是否设置到bas里边去
    返 回 值： TMt 
    -------------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/11/29  4.5		    张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::RefreshBasParam(u8 byMtId, BOOL32 bSet, BOOL32 bDual)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        ConfLog(FALSE, "[RefreshBasParam] unexpected mtid.%d\n", byMtId);
        return FALSE;
    }

    if(!bDual)
        return RefreshBasMVAdpParam(byMtId, bSet);
    else
        return RefreshBasDSAdpParam(byMtId, bSet);

}

/*=============================================================================
    函 数 名： RefreshBasMVAdpParam
    功    能： 
    算法实现：
               
    全局变量： 
    参    数： BOOL32 bSet: 是否设置到bas里边去
    返 回 值： TMt 
    -------------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/11/29  4.5		    张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::RefreshBasMVAdpParam(u8 byMtId, BOOL32 bSet)
{
    if (!m_cMtRcvGrp.IsMtNeedAdp(byMtId))
    {
        return TRUE;
    }

    TLogicalChannel tLogicChn;
    if (!m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_VIDEO, &tLogicChn, TRUE))
    {
        return TRUE;
    }

    TCapSupport tMtCap;
    m_ptMtTable->GetMtCapSupport(byMtId, &tMtCap);
    
    TSimCapSet tMtMVSim = m_ptMtTable->GetDstSCS(byMtId);
    
    if (tMtMVSim.IsNull() ||
        MEDIA_TYPE_NULL == tMtMVSim.GetVideoMediaType())
    {
        ConfLog(FALSE, "[RefreshBasMVAdpParam] get mt.%d's mv cap failed!\n", byMtId);
        return FALSE;
    }

    TEqp tBas;
    u8 byChnId = 0;
    u8 byOutIdx = 0;
    BOOL32 bRet = FALSE;
    bRet = m_cBasMgr.GetBasResource(tMtMVSim.GetVideoMediaType(),
                                    tMtMVSim.GetVideoResolution(),
                                    tBas, byChnId, byOutIdx);
    if (!bRet)
    {
        if (tMtMVSim.GetVideoMediaType() == m_tConf.GetMainVideoMediaType() &&
            tMtMVSim.GetVideoResolution() == m_tConf.GetMainVideoFormat())
        {
            ConfLog(FALSE, "[RefreshBasMVAdpParam] no pos for<vidtype.%d, res.%d>, adjust later!\n",
                tMtMVSim.GetVideoMediaType(), tMtMVSim.GetVideoResolution());
        }
        else
        {
            ConfLog(FALSE, "[RefreshBasMVAdpParam] get bas pos for<vidtype.%d, res.%d> failed!\n",
                            tMtMVSim.GetVideoMediaType(), tMtMVSim.GetVideoResolution());
        }
        return FALSE;
    }

    THDBasVidChnStatus tStatus;
    bRet = m_cBasMgr.GetChnStatus(tBas, byChnId, tStatus);
    if (!bRet)
    {
        ConfLog(FALSE, "[RefreshBasMVAdpParam] get eqp.<%d,%d> mv chn failed!\n", tBas.GetEqpId(), byChnId);
        return FALSE;
    }

    //刷入会议索引
    tBas = tStatus.GetEqp();
    tBas.SetConfIdx(m_byConfIdx);
    tStatus.SetEqp(tBas);

    //微调码率和帧率
    u8 byMtFrmRate = 0;
    if (MEDIA_TYPE_H264 == tMtMVSim.GetVideoMediaType())
    {
        byMtFrmRate = tMtMVSim.GetUserDefFrameRate();
    }
    else
    {
        byMtFrmRate = tMtMVSim.GetVideoFrameRate();
    }
    u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate(byMtId);

    THDAdaptParam tHDAdpParam = *tStatus.GetOutputVidParam(byOutIdx);

    //1、下调
    if ( (wMtBitRate != 0 && tHDAdpParam.GetBitrate() > wMtBitRate) ||
         (byMtFrmRate != 0 && tHDAdpParam.GetFrameRate() > byMtFrmRate))
    {
        if (wMtBitRate != 0)
        {
            ConfLog(FALSE, "[RefreshBasMVAdpParam] Eqp.%d, chn.%d, out.%d adj due to Mt<BR.%d> LE <BR.%d>!\n",
                tBas.GetEqpId(), byChnId, byOutIdx, wMtBitRate, tHDAdpParam.GetBitrate());

            tHDAdpParam.SetBitRate(wMtBitRate);
        }
        if (byMtFrmRate != 0)
        {
            ConfLog(FALSE, "[RefreshBasMVAdpParam] Eqp.%d, chn.%d, out.%d adj due to Mt<Fr.%d> LE <Fr.%d>!\n",
                tBas.GetEqpId(), byChnId, byOutIdx, byMtFrmRate, tHDAdpParam.GetFrameRate());
            tHDAdpParam.SetFrameRate(byMtFrmRate);
        }
        tStatus.SetOutputVidParam(tHDAdpParam, byOutIdx);
        m_cBasMgr.UpdateChn(tBas, byChnId, tStatus);
        
        if (bSet)
        {
            ChangeHDAdapt(tBas, byChnId);
        }
        return TRUE;
    }
    //2、上调
    else
    {
        u8 byMVType = tHDAdpParam.GetVidType();
        u16 wWidth = tHDAdpParam.GetWidth();
        u16 wHeight = tHDAdpParam.GetHeight();
        u8 byRes = GetResByWH(wWidth, wHeight);

        u8 byNum = 0;
        u8 abyMt[MAXNUM_CONF_MT];
        m_cMtRcvGrp.GetMVMtList(byMVType, byRes, byNum, abyMt);

        u8 byMinFrmRate = 0xff;
        u16 wMinBitRate = 0xffff;
        TCapSupport tMtCap;
        for (u8 byIdx = 0; byIdx < byNum; byIdx++)
        {
            memset(&tMtCap, 0, sizeof(tMtCap));
            m_ptMtTable->GetMtCapSupport(abyMt[byIdx], &tMtCap);
            
            TSimCapSet tCurMVSim;
            tCurMVSim = tMtCap.GetMainSimCapSet();
            if (tCurMVSim.IsNull() ||
                MEDIA_TYPE_NULL == tCurMVSim.GetVideoMediaType())
            {
                tCurMVSim = tMtCap.GetSecondSimCapSet();
            }
            if (tCurMVSim.IsNull() ||
                MEDIA_TYPE_NULL == tCurMVSim.GetVideoMediaType())
            {
                continue;
            }

            if (m_ptMtTable->GetMtReqBitrate(abyMt[byIdx]) != 0)
            {
                wMinBitRate = min(wMinBitRate, m_ptMtTable->GetMtReqBitrate(abyMt[byIdx]));
            }
            
            if (MEDIA_TYPE_H264 == tCurMVSim.GetVideoMediaType())
            {
                if (tCurMVSim.GetUserDefFrameRate() != 0)
                {
                    byMinFrmRate = min(tCurMVSim.GetUserDefFrameRate(), byMinFrmRate);
                }
            }
            else
            {
                if (tCurMVSim.GetVideoFrameRate() != 0)
                {
                    byMinFrmRate = min(tCurMVSim.GetVideoFrameRate(), byMinFrmRate);
                }
            }
        }

        if ((wMinBitRate != 0xffff && wMinBitRate > tHDAdpParam.GetBitrate())||
            (byMinFrmRate != 0xff && byMinFrmRate > tHDAdpParam.GetFrameRate()))
        {
            ConfLog(FALSE, "[RefreshBasMVAdpParam] Eqp.%d, chn.%d, out.%d \
adj due to Min<BR.%d, Fr.%d> GE <BR.%d, Fr.%d>!\n",
                            tBas.GetEqpId(),
                            byChnId,
                            byOutIdx,
                            wMinBitRate,
                            byMinFrmRate,
                            tHDAdpParam.GetBitrate(), tHDAdpParam.GetFrameRate());
            
            if (wMinBitRate != 0xffff)
            {
                tHDAdpParam.SetBitRate(wMinBitRate);
            }
            if (byMinFrmRate != 0xff)
            {
                tHDAdpParam.SetFrameRate(byMinFrmRate);
            }
            tStatus.SetOutputVidParam(tHDAdpParam, byOutIdx);
            m_cBasMgr.UpdateChn(tBas, byChnId, tStatus);
            
            if (bSet)
            {
                ChangeHDAdapt(tBas, byChnId);
            }
            return TRUE;
        }
    }
    return FALSE;
}

BOOL32 CMcuVcInst::RefreshBasMVSelAdpParam(const TEqp &tEqp, u8 byChnId)
{
    return TRUE;
}

BOOL32 CMcuVcInst::RefreshBasDSSelAdpParam(const TEqp &tEqp, u8 byChnId)
{
    return TRUE;
}

/*=============================================================================
    函 数 名： RefreshBasDSAdpParam
    功    能： 
    算法实现：
               
    全局变量： 
    参    数： BOOL32 bSet: 是否设置到bas里边去
    返 回 值： TMt 
    -------------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/11/29  4.5		    张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::RefreshBasDSAdpParam(u8 byMtId, BOOL32 bSet)
{
    if (!m_cMtRcvGrp.IsMtNeedAdp(byMtId, FALSE))
    {
        return TRUE;
    }

    TLogicalChannel tLogicChn;
    if (!m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_SECVIDEO, &tLogicChn, TRUE))
    {
        return TRUE;
    }

    TCapSupport tMtCap;
    m_ptMtTable->GetMtCapSupport(byMtId, &tMtCap);
    
    TDStreamCap tMtDSSim;
    tMtDSSim = tMtCap.GetDStreamCapSet();
    
    if (tMtDSSim.IsNull() ||
        MEDIA_TYPE_NULL == tMtDSSim.GetMediaType())
    {
        ConfLog(FALSE, "[RefreshBasDSAdpParam] get mt.%d's ds cap failed!\n", byMtId);
        return FALSE;
    }

    TEqp tBas;
    u8 byChnId = 0;
    u8 byOutIdx = 0;
    BOOL32 bRet = FALSE;
    bRet = m_cBasMgr.GetBasResource(tLogicChn.GetChannelType(),
                                    tLogicChn.GetVideoFormat(),
                                    tBas, 
                                    byChnId,
                                    byOutIdx,
                                    TRUE,
                                    IsDSSrcH263p() || IsConfDualEqMV(m_tConf));
    if (!bRet)
    {
        ConfLog(FALSE, "[RefreshBasDSAdpParam] get bas pos for<vidtype.%d, res.%d> failed!\n",
                        tMtDSSim.GetMediaType(), tMtDSSim.GetResolution());
        return FALSE;
    }

    THDBasVidChnStatus tStatus;
    memset(&tStatus, 0, sizeof(tStatus));

    bRet = m_cBasMgr.GetChnStatus(tBas, byChnId, tStatus);
    if (!bRet)
    {
        ConfLog(FALSE, "[RefreshBasDSAdpParam] get eqp.<%d.%d>ds chn failed!\n",
            tBas.GetEqpId(), byChnId);
        return FALSE;
    }

    //双流 主流/双流
    if (!tStatus.IsNull())
    {
        //刷入会议索引
        tBas = tStatus.GetEqp();
        tBas.SetConfIdx(m_byConfIdx);
        tStatus.SetEqp(tBas);
        
        //微调码率和帧率
        u8 byMtFrmRate = 0;
        if (MEDIA_TYPE_H264 == tMtDSSim.GetMediaType())
        {
            byMtFrmRate = tMtDSSim.GetUserDefFrameRate();
        }
        else
        {
            byMtFrmRate = tMtDSSim.GetFrameRate();
        }
        u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate(byMtId, FALSE);
        
        THDAdaptParam tHDAdpParam = *tStatus.GetOutputVidParam(byOutIdx);
        
        //1、下调
        if ((wMtBitRate != 0 && tHDAdpParam.GetBitrate() > wMtBitRate) ||
            (byMtFrmRate != 0 && tHDAdpParam.GetFrameRate() > byMtFrmRate))
        {
            if (0 != wMtBitRate)
            {
                ConfLog(FALSE, "[RefreshBasDSAdpParam] Eqp.%d, chn.%d, out.%d adj due to Mt.%d<BR.%d> LE <BR.%d>1!\n",
                                tBas.GetEqpId(), byChnId, byOutIdx, byMtId, wMtBitRate, tHDAdpParam.GetBitrate());

                tHDAdpParam.SetBitRate(wMtBitRate);
            }
            if (0 != byMtFrmRate)
            {
                ConfLog(FALSE, "[RefreshBasDSAdpParam] Eqp.%d, chn.%d, out.%d adj due to Mt.%d<Fr.%d> LE <Fr.%d>1!\n",
                    tBas.GetEqpId(), byChnId, byOutIdx, byMtId, byMtFrmRate, tHDAdpParam.GetFrameRate());

                tHDAdpParam.SetFrameRate(byMtFrmRate);
            }
            tStatus.SetOutputVidParam(tHDAdpParam, byOutIdx);
            m_cBasMgr.UpdateChn(tBas, byChnId, tStatus);
            
            if (bSet)
            {
                ChangeHDAdapt(tBas, byChnId);
            }
            return TRUE;
        }
        //2、上调
        else
        {
            u8 byDSType = tHDAdpParam.GetVidType();
            u16 wWidth = tHDAdpParam.GetWidth();
            u16 wHeight = tHDAdpParam.GetHeight();
            u8 byRes = GetResByWH(wWidth, wHeight);
            
            u8 byNum = 0;
            u8 abyMt[MAXNUM_CONF_MT];
            m_cMtRcvGrp.GetDSMtList(byDSType, byRes, byNum, abyMt);
            
            u8 byMinFrmRate = 0xff;
            u16 wMinBitRate = 0xffff;
            for (u8 byIdx = 0; byIdx < byNum; byIdx++)
            {
                memset(&tMtCap, 0, sizeof(tMtCap));
                m_ptMtTable->GetMtCapSupport(abyMt[byIdx], &tMtCap);
                
                TDStreamCap tCurDSSim;
                tCurDSSim = tMtCap.GetDStreamCapSet();
                if (tCurDSSim.IsNull())
                {
                    continue;
                }
                
                if (0 != m_ptMtTable->GetMtReqBitrate(abyMt[byIdx], FALSE))
                {
                    wMinBitRate = min(wMinBitRate, m_ptMtTable->GetMtReqBitrate(abyMt[byIdx], FALSE));
                }
                
                if (MEDIA_TYPE_H264 == tCurDSSim.GetMediaType())
                {
                    if (tCurDSSim.GetUserDefFrameRate() != 0)
                    {
                        byMinFrmRate = min(tCurDSSim.GetUserDefFrameRate(), byMinFrmRate);
                    }
                }
                else
                {
                    if (tCurDSSim.GetFrameRate() != 0)
                    {
                        byMinFrmRate = min(tCurDSSim.GetFrameRate(), byMinFrmRate);
                    }
                }
            }
            
            if ((wMinBitRate != 0xffff && wMinBitRate > tHDAdpParam.GetBitrate())||
                (byMinFrmRate != 0xff && byMinFrmRate > tHDAdpParam.GetFrameRate()))
            {
                ConfLog(FALSE, "[RefreshBasDSAdpParam] Eqp.%d, chn.%d, out.%d \
                                adj due to Min<BR.%d, Fr.%d> GE <BR.%d, Fr.%d>2!\n",
                                tBas.GetEqpId(),
                                byChnId,
                                byOutIdx,
                                wMinBitRate,
                                byMinFrmRate,
                                tHDAdpParam.GetBitrate(), tHDAdpParam.GetFrameRate());
                
                if (wMinBitRate != 0xffff)
                {
                    tHDAdpParam.SetBitRate(wMinBitRate);
                }
                if (byMinFrmRate != 0xff)
                {
                    tHDAdpParam.SetFrameRate(byMinFrmRate);
                }
                tStatus.SetOutputVidParam(tHDAdpParam, byOutIdx);
                m_cBasMgr.UpdateChn(tBas, byChnId, tStatus);
                
                if (bSet)
                {
                    ChangeHDAdapt(tBas, byChnId);
                }
                return TRUE;
            }
        }
    }

    return TRUE;
}

/*=============================================================================
    函 数 名： GetMVBrdSrcSim
    功    能： 获取源的能力
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/11/19  4.5		    张宝卿                  创建
=============================================================================*/
void CMcuVcInst::GetMVBrdSrcSim(TSimCapSet &tSrcSCS)
{
    //FIXME: 高清VMP 和 HD BAS的互斥
    if (m_tConf.m_tStatus.IsBrdstVMP())
    {
        return;
    }

    if (!m_tConf.GetConfAttrb().IsUseAdapter())
    {
        return;
    }  	
	
	if (!HasJoinedSpeaker() && CONF_POLLMODE_VIDEO != m_tConf.m_tStatus.GetPollMode())
    {
        return;
    }


    TMt tSrcMt;
    tSrcMt.SetNull();

	//fxh Bug00018647 只需要获取视频广播源即可,在发言人模式下,发言人同为视频广播源 
// 	if (HasJoinedSpeaker())
// 	{
//         tSrcMt = GetLocalSpeaker();
// 	}
//     else if (CONF_POLLMODE_VIDEO == m_tConf.m_tStatus.GetPollMode())
//     {
//         tSrcMt = m_tVidBrdSrc;
//     }
 	tSrcMt = m_tVidBrdSrc;

    if (tSrcMt.IsNull())
    {
        ConfLog(FALSE, "[GetMVBrdSrcSim] no src mt in conf\n");
        return;
    }

    //考虑录像机
    u8 byType = MEDIA_TYPE_NULL;
    u8 byAudType = MEDIA_TYPE_NULL;
    u16 wWidth = 0;
    u16 wHeight = 0;
    if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType())
    {
        m_tPlayFileMediaInfo.GetVideo( byType, wWidth, wHeight );
        byAudType = m_tPlayFileMediaInfo.GetAudio();
        EqpLog( "[GetMVBrdSrcSim] Type.%d, width.%d, wHeight.%d, AudType.%d\n", byType, wWidth, wHeight, byAudType );

        if ( byType != MEDIA_TYPE_NULL )
        {
            tSrcSCS.SetVideoMediaType( byType);
            tSrcSCS.SetVideoResolution( GetResByWH( wWidth, wHeight ) );
            tSrcSCS.SetVideoMaxBitRate( m_tConf.GetBitRate() );
        }
        else
        {
            tSrcSCS.SetVideoMediaType( m_tConf.GetMainVideoMediaType() );
            tSrcSCS.SetVideoResolution( m_tConf.GetMainVideoFormat() );
            tSrcSCS.SetVideoMaxBitRate( m_tConf.GetBitRate() );
        }

        if ( byAudType != MEDIA_TYPE_NULL )
        {
            tSrcSCS.SetAudioMediaType(byAudType);
        }
        else
        {
            tSrcSCS.SetAudioMediaType( m_tConf.GetMainAudioMediaType() );
        }

        u8 byConfFrame = 0;
        if ( m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264 )
        {
            byConfFrame = m_tConf.GetMainVidUsrDefFPS();
        }
        else
        {
            byConfFrame = m_tConf.GetMainVidFrameRate();
        }
        if ( tSrcSCS.GetVideoMediaType() == MEDIA_TYPE_H264 )
        {
            tSrcSCS.SetUserDefFrameRate( byConfFrame );
        }
        else
        {
            tSrcSCS.SetVideoFrameRate( byConfFrame );
        }
    }
    else
    {
        tSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());	

        //zbq[10/10/2008] 帧率保护
        u8 byFrmRate = 0;
        if (MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType())
        {
            byFrmRate = tSrcSCS.GetUserDefFrameRate();
        }
        else
        {
            byFrmRate = tSrcSCS.GetVideoFrameRate();
        }
        if (0 == byFrmRate)
        {
            u8 byConfFrmRt = 0;
            
            if (tSrcSCS.GetVideoMediaType() == m_tConf.GetMainVideoMediaType())
            {
                if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType())
                {
                    byConfFrmRt = m_tConf.GetMainVidUsrDefFPS();
                }
                else
                {
                    byConfFrmRt = m_tConf.GetMainVidFrameRate();
                }
            }
            else if (tSrcSCS.GetVideoMediaType() == m_tConf.GetSecVideoMediaType())
            {
                if (MEDIA_TYPE_H264 == m_tConf.GetSecVideoMediaType())
                {
                    byConfFrmRt = m_tConf.GetSecVidUsrDefFPS();
                }
                else
                {
                    byConfFrmRt = m_tConf.GetSecVidFrameRate();
                }
            }
            else
            {
                ConfLog(FALSE, "[GetMVBrdSrcSim] SrcSCS's vid type.%d unmatched, ignore it!\n", 
                                tSrcSCS.GetVideoMediaType());
            }

            if (MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType())
            {
                tSrcSCS.SetUserDefFrameRate(byConfFrmRt);
            }
            else
            {
                tSrcSCS.SetVideoFrameRate(byConfFrmRt);
            }
        }
        if (tSrcSCS.IsNull())
        {
            ConfLog(FALSE, "[GetMVBrdSrcSim] 广播源未向MCU打开视频逻辑通道!\n");
            return;
        }
        tSrcSCS.SetVideoMaxBitRate( m_ptMtTable->GetMtSndBitrate( tSrcMt.GetMtId() ) );
    }

    return;
}


/*=============================================================================
    函 数 名： GetDSSrcSim
    功    能： 获取双流源的能力
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/11/19  4.5		    张宝卿                  创建
=============================================================================*/
void CMcuVcInst::GetDSBrdSrcSim(TDStreamCap &tDSim)
{
    TMt tSrcMt;
    tSrcMt.SetNull();
    
    if (!m_tDoubleStreamSrc.IsNull())
    {
        tSrcMt = m_tDoubleStreamSrc;
    }
    
    if (tSrcMt.IsNull())
    {
        CallLog("[GetDSSrcSim] no ds src mt in conf\n");
        return;
    }
    
    //考虑录像机
    u8 byType = MEDIA_TYPE_NULL;
    u16 wWidth = 0;
    u16 wHeight = 0;
    if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType())
    {
        m_tPlayFileMediaInfo.GetDVideo( byType, wWidth, wHeight );
		EqpLog( "[GetDSSrcSim] Type.%d, width.%d, wHeight.%d\n", byType, wWidth, wHeight );     
        
        if ( byType != MEDIA_TYPE_NULL )
        {
            tDSim.SetMediaType( byType);
            tDSim.SetResolution( GetResByWH( wWidth, wHeight ) );
            tDSim.SetMaxBitRate( GetDoubleStreamVideoBitrate(m_tConf.GetBitRate(), FALSE) );
        }
        else
        {
            tDSim.SetMediaType( m_tConf.GetMainVideoMediaType() );
            tDSim.SetResolution( m_tConf.GetMainVideoFormat() );
            tDSim.SetMaxBitRate( GetDoubleStreamVideoBitrate(m_tConf.GetBitRate(), FALSE) );
        }
        u8 byConfFrame = 0;
        if ( m_tConf.GetDStreamMediaType() == MEDIA_TYPE_H264 )
        {
            byConfFrame = m_tConf.GetDStreamUsrDefFPS();
        }
        else
        {
            byConfFrame = m_tConf.GetDStreamFrameRate();
        }
        if ( tDSim.GetMediaType() == MEDIA_TYPE_H264 )
        {
            tDSim.SetUserDefFrameRate( byConfFrame );
        }
        else
        {
            tDSim.SetFrameRate( byConfFrame );
        }
    }
    else
    {
        //zbq[01/09/2009] 双流源能力取后向逻辑通道
        TLogicalChannel tDSRcvChn;
        if (m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_SECVIDEO, &tDSRcvChn, FALSE))
        {
            tDSim.SetMediaType(tDSRcvChn.GetChannelType());
            if (MEDIA_TYPE_H264 == tDSim.GetMediaType())
            {
                tDSim.SetUserDefFrameRate(tDSRcvChn.GetChanVidFPS());
            }
            else
            {
                tDSim.SetFrameRate(tDSRcvChn.GetChanVidFPS());
            }
            tDSim.SetMaxBitRate(tDSRcvChn.GetFlowControl());
            tDSim.SetResolution(tDSRcvChn.GetVideoFormat());
        }
        else
        {
            ConfLog(FALSE, "[GetDSSrcSim] DSSrc's rcv chn hasn't opened yet!\n");

            TCapSupport tCap;
            m_ptMtTable->GetMtCapSupport(tSrcMt.GetMtId(), &tCap);
            tDSim = tCap.GetDStreamCapSet();
            
            //zbq[10/10/2008] 帧率保护
            u8 byFrmRate = 0;
            if (MEDIA_TYPE_H264 == tDSim.GetMediaType())
            {
                byFrmRate = tDSim.GetUserDefFrameRate();
            }
            else
            {
                byFrmRate = tDSim.GetFrameRate();
            }
            if (0 == byFrmRate)
            {
                if (MEDIA_TYPE_H264 == tDSim.GetMediaType())
                {
                    tDSim.SetUserDefFrameRate(m_tConf.GetMainVidUsrDefFPS());
                }
                else
                {
                    tDSim.SetFrameRate(m_tConf.GetMainVidUsrDefFPS());
                }
            }
            if (tDSim.IsNull())
            {
                ConfLog(FALSE, "[GetDSSrcSim] DSSrc's ds cap is NULL!\n");
                return;
            }
            tDSim.SetMaxBitRate( m_ptMtTable->GetMtSndBitrate( tSrcMt.GetMtId(), LOGCHL_SECVIDEO ) );
        }
    }
    
    return;
}


/*=============================================================================
    函 数 名： GetSelSrcSim
    功    能： 获取双流选看源能力
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/11/19  4.5		    张宝卿                  创建
=============================================================================*/
void CMcuVcInst::GetSelSrcSim(const TEqp &tEqp, u8 byChnId, TDStreamCap &tDSim)
{

}


/*=============================================================================
    函 数 名： GetSelSrcSim
    功    能： 获取视频选看源能力
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/11/19  4.5		    张宝卿                  创建
=============================================================================*/
void CMcuVcInst::GetSelSrcSim(const TEqp &tEqp, u8 byChnId, TSimCapSet &tSim)
{

}


/*=============================================================================
    函 数 名： IsMtInMcSrc
    功    能： 判断终端在本会议中是否有对应的源
    算法实现： 
    全局变量： 
    参    数： u8 byChnRes: 打开通道用的Res, 只针对h264的会议处理
               BOOL32 &bAccord2MainCap：是否根据主能力打开主流
    返 回 值： void
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/12/30  4.5		    张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::IsMtMatchedSrc(u8 byMtId, u8 &byChnRes, BOOL32 &bAccord2MainCap)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return FALSE;
    }
    if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
    {
        return FALSE;
    }

    TCapSupport tCap;
    if (!m_ptMtTable->GetMtCapSupport(byMtId, &tCap))
    {
        ConfLog(FALSE, "[IsMtMatchedSrc] get mt.%d's cap failed!\n", byMtId);
        return FALSE;
    }

    TSimCapSet tSimCap;
    tSimCap.Clear();
    tSimCap = tCap.GetMainSimCapSet();

    if (tSimCap.IsNull() ||
        MEDIA_TYPE_NULL == tSimCap.GetVideoMediaType())
    {
        tSimCap = tCap.GetSecondSimCapSet();
    }
    if (tSimCap.IsNull() ||
        MEDIA_TYPE_NULL == tSimCap.GetVideoMediaType())
    {
        ConfLog(FALSE, "[IsMtMatchedSrc] mt.%d cap is null already, ignore it!\n", byMtId);
        return FALSE;
    }
    
    u8 byMediaType = tSimCap.GetVideoMediaType();
    u8 byRes = tSimCap.GetVideoResolution();

    if (MEDIA_TYPE_H264 != byMediaType)
    {
        if (byMediaType == m_tConf.GetMainVideoMediaType() ||
            byMediaType == m_tConf.GetSecVideoMediaType() )
        {
            bAccord2MainCap = TRUE;
            byChnRes = m_tConf.GetMainVideoFormat();
            return TRUE;
        }
    }
    else
    {
        if (byMediaType != m_tConf.GetMainVideoMediaType())
        {
            return FALSE;
        }
        if (byRes == m_tConf.GetMainVideoFormat() ||
            (byRes == VIDEO_FORMAT_HD1080 && m_tConf.GetConfAttrbEx().IsResEx1080()) ||
            (byRes == VIDEO_FORMAT_HD720 && m_tConf.GetConfAttrbEx().IsResEx720()) ||
            (byRes == VIDEO_FORMAT_4CIF && m_tConf.GetConfAttrbEx().IsResEx4Cif()) ||
            (byRes == VIDEO_FORMAT_CIF && m_tConf.GetConfAttrbEx().IsResExCif()))
        {
            //zbq[05/22/2009] 适配资源紧凑模式下，同会议主分辨率降速打开不再支持；此处需作降分辨率打开处理.
            if (byRes == m_tConf.GetMainVideoFormat())
            {
                u16 wDialBR = m_ptMtTable->GetDialBitrate(byMtId);
                u16 wConfBR = m_tConf.GetBitRate();
                
                //低速呼叫粗略鉴定
                BOOL32 bCallLowBR = (wConfBR - wDialBR) * 100 / wConfBR > 5;
                if (bCallLowBR)
                {
                    //尝试取临近分辨率
                }
                else
                {
                    bAccord2MainCap = TRUE;
                    byChnRes = byRes;
                    return TRUE;
                }
            }
            else
            {
                bAccord2MainCap = TRUE;
                byChnRes = byRes;
                return TRUE;
            }
        }

        //zbq[04/22/2009] 尝试取会议支持的临近分辨率
        byChnRes = 0xff;
		if (byRes == VIDEO_FORMAT_HD1080)
		{
			if (m_tConf.GetConfAttrbEx().IsResEx720())
			{
				byChnRes = VIDEO_FORMAT_HD720;
			}
			else if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
			{
				byChnRes = VIDEO_FORMAT_4CIF;
			}
			else if (m_tConf.GetConfAttrbEx().IsResExCif())
			{
				byChnRes = VIDEO_FORMAT_CIF;
			}
		}
		else if (byRes == VIDEO_FORMAT_HD720)
		{
			if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
			{
				byChnRes = VIDEO_FORMAT_4CIF;
			}
			else if (m_tConf.GetConfAttrbEx().IsResExCif())
			{
				byChnRes = VIDEO_FORMAT_CIF;
			}
		}
		else if (byRes == VIDEO_FORMAT_4CIF)
		{
			if (m_tConf.GetConfAttrbEx().IsResExCif())
			{
				byChnRes = VIDEO_FORMAT_CIF;
			}
		}
		else
		{
			//Do nothing
		}
        
        if (0xff != byChnRes)
        {
            bAccord2MainCap = TRUE;
            return TRUE;
        }
    }
	
    //zbq[04/22/2009] 双格式支持考虑
    tSimCap.Clear();
    tSimCap = tCap.GetSecondSimCapSet();
    if (!tSimCap.IsNull())
    {
        if (tSimCap.GetVideoMediaType() == m_tConf.GetSecVideoMediaType())
        {
            bAccord2MainCap = FALSE;
            byChnRes = tSimCap.GetVideoResolution();
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
    函 数 名： GetProximalGrp
    功    能： 获取向下最临近群组
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    09/01/05    4.5         张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::GetProximalGrp(u8 byMediaType, u8 byRes, u8 &byGrpType, u8 &byGrpRes)
{
    //非264没有可选的临近群组
    if (MEDIA_TYPE_H264 != byMediaType)
    {
        return FALSE;
    }

    byGrpRes = 0;
    byGrpType = byMediaType;

    switch (byRes)
    {
    case VIDEO_FORMAT_4CIF:
        
        if (m_tConf.GetConfAttrbEx().IsResExCif())
        {
            byGrpRes = VIDEO_FORMAT_CIF;
        }
        break;

    case VIDEO_FORMAT_HD720:

        if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
        {
            byGrpRes = VIDEO_FORMAT_4CIF;
        }
        else if (m_tConf.GetConfAttrbEx().IsResExCif())
        {
            byGrpRes = VIDEO_FORMAT_CIF;
        }
        break;

    case VIDEO_FORMAT_HD1080:

        if (m_tConf.GetConfAttrbEx().IsResEx720())
        {
            byGrpRes = VIDEO_FORMAT_HD720;
        }
        else if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
        {
            byGrpRes = VIDEO_FORMAT_4CIF;
        }
        else if (m_tConf.GetConfAttrbEx().IsResExCif())
        {
            byGrpRes = VIDEO_FORMAT_CIF;
        }
        break;

    case VIDEO_FORMAT_CIF:
    default:
        break;
    }

    if (byGrpRes != 0)
    {
        return TRUE;
    }
    return FALSE;
}


/*====================================================================
    函数名      ：IsDelayVidBrdVCU
    功能        ：是否推迟视频源的VCU和youareseeing的发送
	              通知MC，MT和
    算法实现    ：针对Tandberg在VMP轮询跟随时，进VMP码流慢的情况 特殊处理 判断实现
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/25    3.0         胡昌威        创建
====================================================================*/
BOOL32 CMcuVcInst::IsDelayVidBrdVCU()
{
    if (m_tVidBrdSrc.IsNull() ||
        MT_MANU_TAIDE == m_ptMtTable->GetManuId(m_tVidBrdSrc.GetMtId()))
    {
        return FALSE;
    }
    if (CONF_POLLMODE_VIDEO != m_tConf.m_tStatus.GetPollMode() &&
        CONF_POLLMODE_SPEAKER != m_tConf.m_tStatus.GetPollMode() )
    {
        return FALSE;
    }

    BOOL32 bVmpSelPoll = FALSE;

    u8 byIdx = 0;
    for (byIdx = 0; byIdx < MAXNUM_MPUSVMP_MEMBER; byIdx++)
    {
        TVMPMember *ptVmpMem = m_tConf.m_tStatus.m_tVMPParam.GetVmpMember(byIdx);
        if (ptVmpMem->IsNull())
        {
            continue;
        }
        if (ptVmpMem->GetMemberType() == VMP_MEMBERTYPE_POLL)
        {
            bVmpSelPoll = TRUE;
            break;
        }
    }

    return bVmpSelPoll;
}

/*=============================================================================
    函 数 名： GetMtMatchedRes
    功    能： 获取指定终端在指定视频格式下最接近会议支持的分辨率
    算法实现： 
    全局变量： 
    参    数： byMtId:     指定终端
			   byChnType:  指定视频格式
			   byChnRes:   返回指定视频格式最符合会议的可支持分辨率
    返 回 值： void
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    09/05/19    4.5		    付秀华                  创建
=============================================================================*/
BOOL32 CMcuVcInst::GetMtMatchedRes(u8 byMtId, u8 byChnType, u8& byChnRes)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return FALSE;
    }
    if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
    {
        return FALSE;
    }

    TCapSupport tCap;
    if (!m_ptMtTable->GetMtCapSupport(byMtId, &tCap))
    {
        ConfLog(FALSE, "[GetMtMatchedRes] get mt.%d's cap failed!\n", byMtId);
        return FALSE;
    }

	if (tCap.GetMainVideoType() == byChnType)
	{
		byChnRes  = tCap.GetMainVideoResolution();
	}
	else if (tCap.GetSecVideoType() == byChnType)
	{
		byChnRes  = tCap.GetSecVideoResolution();
	}
	else
	{
		ConfLog(FALSE, "[GetMtMatchedRes] mt not support this media type(%d)\n", byChnType);
		return FALSE;
	}

	u8 byConfRes = 0;
	if (m_tConf.GetMainVideoMediaType() == byChnType)
	{
		byConfRes = m_tConf.GetMainVideoFormat();
	}
	else
	{
		byConfRes = m_tConf.GetSecVideoFormat();
	}
	CallLog("[GetMtMatchedRes] conf res(%d) cap of mediatype(%d)\n", byConfRes, byChnType);

	// 对于mpeg4自适应特殊处理
	if (VIDEO_FORMAT_AUTO == byConfRes || VIDEO_FORMAT_AUTO == byChnRes)
	{
		CallLog("[GetMtMatchedRes] open mt video channl with mt res(%d)\n", byChnRes);
		return TRUE;
	}
	else if (IsResGE(byChnRes, byConfRes))
	{

        //zbq[05/22/2009] 适配资源紧凑模式下，同会议主分辨率降速打开不再支持；此处需作降分辨率打开处理.
        u16 wDialBR = m_ptMtTable->GetDialBitrate(byMtId);
        u16 wConfBR = m_tConf.GetBitRate();
        
        //低速呼叫粗略鉴定
        BOOL32 bCallLowBR = (wConfBR - wDialBR) * 100 / wConfBR > 5;
        
        if (bCallLowBR && g_cMcuVcApp.IsAdpResourceCompact())
        {
            u8 byResTmp = byChnRes;

            //尝试取临近分辨率
            if (VIDEO_FORMAT_HD720 == byChnRes ||
                VIDEO_FORMAT_HD1080 == byChnRes)
            {
                if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
                {
                    byChnRes = VIDEO_FORMAT_4CIF;
                }
                else if (m_tConf.GetConfAttrbEx().IsResExCif())
                {
                    byChnRes = VIDEO_FORMAT_CIF;
                }
            }
            else if (byChnRes == VIDEO_FORMAT_4CIF)
            {
                if (m_tConf.GetConfAttrbEx().IsResExCif())
                {
                    byChnRes = VIDEO_FORMAT_CIF;
                }
            }
            else
            {
                //Do nothing
			}

            if ( byResTmp != byChnRes )
            {
                CallLog("[GetMtMatchedRes] open mt video channl adj res(%d) to res(%d)\n", byResTmp, byChnRes);
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            byChnRes = byConfRes;
            CallLog("[GetMtMatchedRes] open mt video channl with conf res(%d)\n", byChnRes);
            return TRUE;
        }
	}
	else
	{
        if (byChnType != MEDIA_TYPE_H264 || byChnType != m_tConf.GetMainVideoMediaType())
        {
            return FALSE;
        }

		if (g_cMcuVcApp.IsSendFakeCap2Polycom() &&
			MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byMtId))
		{
			byChnRes = VIDEO_FORMAT_4SIF;
			CallLog("[GetMtMatchedRes] Open Mt.%d video logic chnnl adjusted to 4SIF due to it's Polycom!\n", byMtId);
			return TRUE;
		}
		// 对于VCS会议,H264会议,允许所有分辨率的终端入会,按照比会议小的终端支持的分辨率开通道
		// 对于组呼目前走会议逻辑
		else if (VCS_CONF == m_tConf.GetConfSource() &&
			     !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
		{
			return TRUE;
		}
		else
		{
			//zbq[04/22/2009] 尝试取会议支持的临近分辨率
			u8 byTmpChnRes = byChnRes;
            byChnRes = 0xff;
			if (VIDEO_FORMAT_HD720 == byTmpChnRes ||
                VIDEO_FORMAT_HD1080 == byTmpChnRes)
			{
				if (m_tConf.GetConfAttrbEx().IsResEx720())
				{
					byChnRes = VIDEO_FORMAT_HD720;
				}
				else if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
				{
					byChnRes = VIDEO_FORMAT_4CIF;
				}
				else if (m_tConf.GetConfAttrbEx().IsResExCif())
				{
					byChnRes = VIDEO_FORMAT_CIF;
				}
			}
			else if (byTmpChnRes == VIDEO_FORMAT_4CIF)
			{
				if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
				{
					byChnRes = VIDEO_FORMAT_4CIF;
				}
				else if (m_tConf.GetConfAttrbEx().IsResExCif())
				{
					byChnRes = VIDEO_FORMAT_CIF;
				}
			}
			else if (byTmpChnRes == VIDEO_FORMAT_CIF)
			{
				if (m_tConf.GetConfAttrbEx().IsResExCif())
				{
					byChnRes = VIDEO_FORMAT_CIF;
				}
			}
			else
			{
				//Do nothing
			}
        
			if (byChnRes != 0xff)
			{
				CallLog("[GetMtMatchedRes] open mt video channl with res(%d) with use of adp\n", byChnRes);
				return TRUE;
			}
		}
    }
	
    return FALSE;
}

/*====================================================================
    函数名      ：FindAcceptCodeChnl
    功能        ：查找指定终端是否可接收指定适配器适配后的码流
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/06/01    4.0         付秀华        创建
====================================================================*/
BOOL32 CMcuVcInst::FindAcceptCodeChnl(u8 byMtId, u8 byMode, u8 byEqpId, u8 byChnIdx, u8& byOutChnlIdx)
{
    TPeriEqpStatus tEqpStatus;
    if (!g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tEqpStatus))
    {
        OspPrintf(TRUE, FALSE, "[FindAcceptCodeChnl] get Eqp.%d status failed!\n", byEqpId);
        return FALSE;
    }
   
	TLogicalChannel tChnlInfo;
	if (byMode == MODE_VIDEO)
	{
		m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_VIDEO, &tChnlInfo, TRUE);
	}

	else if (byMode == MODE_AUDIO)
	{
		m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_AUDIO, & tChnlInfo, TRUE);
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[FindAcceptCodeChnl] wrong input param about byMode(%d)\n", byMode);
		return FALSE;
	}

	// 不完善版本:目前音频适配只选用原标清适配器,视频适配只选用高清适配器
	if (MODE_AUDIO == byMode)
	{
		if (byChnIdx >= tEqpStatus.m_tStatus.tBas.byChnNum)
		{
			OspPrintf(TRUE, FALSE, "[FindAcceptCodeChnl] wrong input chnlidx(%d) for sdbas(%d) with chnlnum %d\n",
				      byChnIdx, byEqpId, tEqpStatus.m_tStatus.tBas.byChnNum);
			return FALSE;
		}

		if (tChnlInfo.GetChannelType() == tEqpStatus.m_tStatus.tBas.tChnnl[byChnIdx].GetAudType())
		{
			byOutChnlIdx = byChnIdx;
			return TRUE;
		}		
		
		
	}
	else
	{
		THDBasVidChnStatus tVidChn;
		memset(&tVidChn, 0, sizeof(tVidChn));
		THDAdaptParam tParam;
		u8 byBasType = tEqpStatus.m_tStatus.tHdBas.GetEqpType();
		switch (byBasType)
		{
		case TYPE_MAU_NORMAL:
		case TYPE_MAU_H263PLUS:
			{
				if (byChnIdx >= MAXNUM_MAU_VCHN + MAXNUM_MAU_DVCHN)
				{
					OspPrintf(TRUE, FALSE, "[TMVChnGrp::AddChn] unexpected mau chnId.%d!\n", byChnIdx);
					return FALSE;
				}
            
				BOOL32 bChnMV = byChnIdx == 0 ? TRUE : FALSE;            
				if (bChnMV)
				{
					tVidChn = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus();
                
					for(u8 byIdx = 0; byIdx < MAXNUM_VOUTPUT; byIdx++)
					{
						tParam = *tVidChn.GetOutputVidParam(byIdx);
						if (tParam.GetVidType() == tChnlInfo.GetChannelType() &&
							IsResGE(tChnlInfo.GetVideoFormat(), GetResByWH(tParam.GetWidth(), tParam.GetHeight())))
						{
							byOutChnlIdx = byIdx;
							return TRUE;
						}
					}
				}
				else
				{
					tVidChn = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetDVidChnStatus();
                
					for(u8 byIdx = 0; byIdx < MAXNUM_VOUTPUT; byIdx++)
					{
						tParam = *tVidChn.GetOutputVidParam(byIdx);
						if (tParam.GetVidType() == tChnlInfo.GetChannelType() &&
							IsResGE(tChnlInfo.GetVideoFormat(), GetResByWH(tParam.GetWidth(), tParam.GetHeight())))
						{
							byOutChnlIdx = byIdx;
							return TRUE;
						}
					}
				}
			}
			break;
        
		case TYPE_MPU:
			{
				if (byChnIdx >= MAXNUM_MPU_CHN)
				{
					OspPrintf(TRUE, FALSE, "[TMVChnGrp::AddChn] unexpected mpu chnId.%d!\n", byChnIdx);
					return FALSE;
				}
				tVidChn = *tEqpStatus.m_tStatus.tHdBas.tStatus.tMpuBas.GetVidChnStatus(byChnIdx);
            
				for(u8 byIdx = 0; byIdx < MAXNUM_VOUTPUT; byIdx++)
				{
					tParam = *tVidChn.GetOutputVidParam(byIdx);
					if (tParam.GetVidType() == tChnlInfo.GetChannelType() &&
						IsResGE(tChnlInfo.GetVideoFormat(), GetResByWH(tParam.GetWidth(), tParam.GetHeight())))
					{
						byOutChnlIdx = byIdx;
						return TRUE;
					}
				}
			}
			break;
        
		default:
			break;
		}
	}


	return FALSE;
}

/*====================================================================
函数名      ：IsNeedNewSelAdp
功能        ：查找指定选看是否发需要使用新的适配器
算法实现    ：
引用全局变量：
输入参数说明：bySrcId:选看本地终端
	          byDstId:被选看本地终端
			  bySelMode:选看的类型
			  ptEqpId:可复用使用的适配器ID
			  pOutChnlIdx:可复用使用的适配器输出通道索引
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
09/06/01    4.0         付秀华        创建
====================================================================*/
BOOL32 CMcuVcInst::IsNeedNewSelAdp(u8 bySrcId, u8 byDstId, u8 bySelMode, u8* pbyEqpId /*= NULL*/, u8* pbyOutChnlIdx /*= NULL*/)
{
	u8 byOccupChnlNum, byChnIdx = 0;
	u8 abyEqpId[MAXNUM_PERIEQP];
	u8 abyChnIdx[MAXNUM_PERIEQP];
	if (m_cSelChnGrp.FindSelSrc(bySrcId, bySelMode, byOccupChnlNum, abyEqpId, abyChnIdx))
	{
		for (u8 byIdx = 0; byIdx < byOccupChnlNum; byIdx++)
		{			
			if (FindAcceptCodeChnl(byDstId, bySelMode, abyEqpId[byIdx], abyChnIdx[byIdx], byChnIdx))
			{
				if (pbyEqpId != NULL)
				{
					*pbyEqpId = abyEqpId[byIdx];
				}
				if (pbyOutChnlIdx != NULL)
				{
					*pbyOutChnlIdx = /*byChnIdx*/abyChnIdx[byIdx];
				}
				EqpLog("[StartHdVidSelAdp] Find acceptable code from bas(id:%d, inputchnl:%d, outputchnl:%d) for mt(mtid:%d)",
					   abyEqpId[byIdx], abyChnIdx[byIdx], byChnIdx, byDstId);
				return FALSE;
			}
		}
	}
	return TRUE;
}


/************************************************************************/
/*                                                                      */
/*                          八、卫星会议操作函数                        */
/*                                                                      */
/************************************************************************/


/*====================================================================
 函数名      :DaemonProcSendMsgToNms
 功能        :发送UDP网管消息
 算法实现    
 引用全局变量：
 输入参数说明：
 返回值说明  ：
 ----------------------------------------------------------------------
 修改记录    ：
 日  期      版本        修改人        修改内容
 03/12/19   1.0          zhangsh        创建
 09/08/28   4.6          张宝卿         从卫星版本移植过来
====================================================================*/
void CMcuVcInst::DaemonProcSendMsgToNms( const CMessage* pcMsg  )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    
    u32   dwIp;
    u16   wPort;
    SOCKET  m_hSocket;
    SOCKADDR_IN tUdpAddr;
    int         nTotalSendLen;
    
    g_cMcuVcApp.GetApplyFreqInfo( dwIp, wPort );
    
    memset( &tUdpAddr, 0, sizeof( tUdpAddr ) );
    tUdpAddr.sin_family = AF_INET; 
    tUdpAddr.sin_port = htons(wPort);
    tUdpAddr.sin_addr.s_addr = dwIp;
    
    m_hSocket = socket ( AF_INET, SOCK_DGRAM , 0 );
    if( m_hSocket == INVALID_SOCKET )
    {
        ConfLog( FALSE, "DaemonProcSendMsgToNms: Create UDP Socket Error.\n" );
        return ;
    }
    nTotalSendLen = sendto( m_hSocket,
                            (s8*)cServMsg.GetMsgBody(),
                            cServMsg.GetMsgBodyLen(),
                            0,
                            (struct sockaddr *)&tUdpAddr,
                            sizeof( tUdpAddr ) );
    SockClose( m_hSocket );

    if ( nTotalSendLen < cServMsg.GetMsgBodyLen() )
    {
        ConfLog( FALSE, "DaemonProcSendMsgToNms: Send Error.\n" );
        return ;
    }
}


/*====================================================================
 函数名      ：ApplySatFrequence
 功能        ：为即将召开的会议申请卫星频率
 算法实现    ：
 引用全局变量：
 输入参数说明：
 返回值说明  ：
 ----------------------------------------------------------------------
 修改记录    ：
 日  期      版本        修改人        修改内容
 09/08/28    4.6         张宝卿        创建
====================================================================*/
void CMcuVcInst::ApplySatFrequence()
{
	u32 dwFreq[5];
	u32 dwBrdBitRate;
	u32 dwRcvBitRate;

    if ( m_tConf.GetConfAttrb().IsSatDCastMode() /*&& 
         !g_cMcuVcApp.IsConfGetAllFreq( m_byConfIdx )*/ )
    {
        g_cMcuVcApp.SetConfInfo( m_byConfIdx );					
        
        //首先申请广播的
        dwBrdBitRate = GetAudioBitrate( m_tConf.GetMainAudioMediaType() );
        dwBrdBitRate = dwBrdBitRate * 5;//m_tConf.GetConfAttrbEx().GetSatDCastChnlNum();

		//双格式会议，广播频率的初相带宽加倍，以满足双向下行
		if (MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType() ||
			0 == m_tConf.GetSecVideoMediaType())
		{
			dwBrdBitRate += m_tConf.GetBitRate();
		}
		else
		{
			dwBrdBitRate += m_tConf.GetBitRate() + m_tConf.GetSecBitRate();
		}
        
        
        //如果要申请选看的则加上
        if ( 1 )
        {
            //FIMXE: 暂不考虑给选看申请带宽
        }

		//其次申请接收的
		if (MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType() ||
			0 == m_tConf.GetSecVideoMediaType())
		{
			dwRcvBitRate = m_tConf.GetBitRate();
        }
		else
		{
			dwRcvBitRate = m_tConf.GetBitRate() + m_tConf.GetSecBitRate();
		}
        
		dwBrdBitRate = dwBrdBitRate * 1024;
		dwRcvBitRate = dwRcvBitRate * 1024;

        u8 byBrdNum = 1;    //广播源个数
        
        SendCmdToNms( MCU_NMS_APPLYCONFFREQ_REQ, 
                      byBrdNum,
                      dwBrdBitRate,
                      5,//m_tConf.GetConfAttrbEx().GetSatDCastChnlNum(),
                      dwRcvBitRate,
                      dwFreq, 0);

        g_cMcuVcApp.SetConfBitRate( m_byConfIdx, 0, dwRcvBitRate );
        g_cMcuVcApp.SetConfBitRate( m_byConfIdx, dwBrdBitRate, 0 );

		SetTimer( MCUVC_APPLYFREQUENCE_CHECK_TIMER, TIMESPACE_APPLYFREQUENCE_CHECK );

        //FIXME: 暂时放弃面向网管的状态机保护
        //NEXTSTATE( STATE_WAITAUTH );
    }
    return;
}


/*====================================================================
 函数名      ：ReleaseSatFrequence
 功能        ：会议结束，释放卫星频率
 算法实现    ：
 引用全局变量：
 输入参数说明：
 返回值说明  ：
 ----------------------------------------------------------------------
 修改记录    ：
 日  期      版本        修改人        修改内容
 09/08/28     4.6         张宝卿        创建
====================================================================*/
void CMcuVcInst::ReleaseSatFrequence()
{
	u32 dwBrdBitRate;
	u32 dwRcvBitRate;
	u32 dwBrdFreq;
	u32 dwFreq[5];
	TMcuReleaseFreq  tReqFree;
	CServMsg cServMsg;
	u32 dwSn;
	dwSn = GetSerialId();
	switch( CurState() )
	{
	case STATE_IDLE:
	case STATE_MASTER:
	case STATE_ONGOING:
	case STATE_WAITAUTH:
	case STATE_PERIOK:
		
		memset( &tReqFree, 0, sizeof(TMcuReleaseFreq) );
		tReqFree.SetCmd( MCU_NMS_RELEASECONFFREQ_REQ );
		
		//广播的
		if ( g_cMcuVcApp.IsConfGetSendFreq( m_byConfIdx ) )
		{
			dwBrdBitRate = g_cMcuVcApp.GetConfSndBitRate( m_byConfIdx );
			dwBrdFreq = g_cMcuVcApp.GetConfSndFreq( m_byConfIdx );
			g_cMcuVcApp.SetConfGetSendFreq( m_byConfIdx, FALSE );
		}
		tReqFree.SetBrdBitRate( dwBrdBitRate );
		tReqFree.SetBrdFreq( dwBrdFreq );
		OspPrintf( TRUE,FALSE,"释放:广播码率 - %u 广播频点 - %u\n",dwBrdBitRate, dwBrdFreq );

		//接收的
		if ( g_cMcuVcApp.IsConfGetReceiveFreq( m_byConfIdx ) )
		{
			dwRcvBitRate = g_cMcuVcApp.GetConfRcvBitRate( m_byConfIdx );
			for ( u8 byNum = 0; byNum < m_tConf.GetConfAttrbEx().GetSatDCastChnlNum(); byNum ++)
			{
				dwFreq[byNum] = g_cMcuVcApp.GetConfRcvFreq( m_byConfIdx, byNum );
				OspPrintf( TRUE,FALSE,"释放:单播频点 - %u\n",dwFreq[byNum] );

				tReqFree.SetFreeFreq( byNum+1, dwFreq[byNum] );
			}
			g_cMcuVcApp.SetConfGetReceiveFreq( m_byConfIdx, FALSE );
		}
		g_cMcuVcApp.ReleaseConfInfo( m_byConfIdx );
		
		
		tReqFree.SetRcvBitRate( dwRcvBitRate );
		tReqFree.SetNum( m_tConf.GetConfAttrbEx().GetSatDCastChnlNum() );
		tReqFree.SetSN( dwSn );
		cServMsg.SetMsgBody( (u8*)&tReqFree, sizeof( TMcuReleaseFreq ) );
		cServMsg.SetConfIdx( m_byConfIdx );
		g_cMcuVcApp.SendMsgToDaemonConf( MCU_NMS_SENDNMSMSG_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		
		//SendCmdToNms( MCU_NMS_RELEASECONFFREQ_REQ, 1, dwBrdBitRate,m_tConf.GetConfLineNum(), dwRcvBitRate, dwFreq,dwBrdFreq );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong release freq message received in state %u!\n", CurState() );
		break;
	}
	return;
}


/*====================================================================
 函数名      :GetSerialId
 功能        :网管通讯序列号
 算法实现    :
 引用全局变量：
 输入参数说明：
 返回值说明  ：
 ----------------------------------------------------------------------
 修改记录    ：
 日  期      版本        修改人        修改内容
 03/12/19   1.0          zhangsh         创建
 09/08/28   4.6          张宝卿          从卫星版本移植过来
====================================================================*/
u32  CMcuVcInst::GetSerialId()
{
    u32 dwSn;
    u8 byInsId = (u8)GetInsID();
    m_dwSeq++;
    
    dwSn = byInsId << 16;
    dwSn = dwSn + ( m_dwSeq & 0x0000ffff );
    return dwSn;
}


/*====================================================================
函数名      ProcModemConfRegRsp
功能        Modem建链
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/11/11   1.0          zhangsh       创建
====================================================================*/
void CMcuVcInst::ProcModemConfRegRsp( const CMessage * pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    TMt tMt = *(TMt*)cServMsg.GetMsgBody();
    
    switch( CurState() ) 
    {
    case STATE_MASTER:

    //FIXME: 稍后作严格状态机 串行 处理
    case STATE_ONGOING:
        
        switch ( pcMsg->event )
        {
        case MODEM_MCU_REG_ACK:
            g_cMcuVcApp.SetMtModemConnected( tMt.GetMtId(), TRUE );
            break;
        case MODEM_MCU_REG_NACK:
            g_cMcuVcApp.SetMtModemConnected( tMt.GetMtId(), FALSE );
            break;
        }
        break;
    default:
        log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;
    }
}

/*====================================================================
函数名      DaemonProcModemReg
功能        中心MODEM连接状态
算法实现    ：
引用全局变量：
输入参数说明：const CMessage * pcMsg, 传入的消息
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
02/08/02    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::DaemonProcModemReg( const CMessage *pcMsg )
{
    u16 wModemId = *(u16*)pcMsg->content;
    if ( wModemId < 17 )
    {
        g_cMcuVcApp.SetMcuModemConnected( wModemId, TRUE );
        g_cMcuVcApp.SetModemSportNum( (u8)wModemId,(u8)wModemId );
    }
}

/*====================================================================
    函数名      ：RefreshConfState
    功能        ：更新会议状态，将业务中的会议信息转化为CSatConfInfo
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/19   1.0          JQL       创建
====================================================================*
void CMcuVcInst::RefreshConfState()
{
	CSatConfInfo cSatConfInfo;

	CServMsg	 cServMsg;
	u8			 bySpeakerId;

	//增加与会成员信息
	u8	byIndex = LOCAL_MCUID /*m_tConf.FindMcu( g_cMcuAgent.GetId() )*;
	TConfMtInfo	tMtInfo;	
	
	if( byIndex == ( u8 )-1 )
	{
		ConfLog( FALSE, "CMcuVcInst: Unexcepted Error In RefreshConfState.\n" );
		return;
	}
	tMtInfo = m_tConfAllMtInfo.GetMtInfo( byIndex );

	cSatConfInfo.SetConfMtInfo( tMtInfo );
	cSatConfInfo.SetConfId( (u8)GetInsID() );

	bySpeakerId = m_tConf.GetSpeaker().GetMtId();

	//会议组播地址 MAXNUM_MCU_MT + GetInsId的组播地址

	//GetMtMultiCastAddr( MAXNUM_MCU_MT + GetInsID(), &tSpeakerAddr );

    u32 dwMultiCastAddr = g_cMcuVcApp.GetExistSatCastIp();
    u16 wMultiCastPort = g_cMcuVcApp.GetExistSatCastPort();
	cSatConfInfo.SetBrdMeidaIpAddr( dwMultiCastAddr );
	cSatConfInfo.SetBrdMediaStartPort( wMultiCastPort );

	//设置会议的主席
	cSatConfInfo.SetChairmanId( m_tConf.GetChairman().GetMtId() );
	//设置会议的发言人
	cSatConfInfo.SetSpeakerId( bySpeakerId );
	//设置会议的ID
	cSatConfInfo.SetConfId( GetInsID() );
	//设置会议名
	cSatConfInfo.SetConfName( m_tConf.GetConfName() );
	cSatConfInfo.SetSatIntervTime( m_tConf.GetDuration() / 60 );
	cSatConfInfo.SetConfLineNum( m_tConf.GetConfAttrbEx().GetSatDCastChnlNum() );
	
    //设置会议是否在混音
	if( m_tConf.m_tStatus.IsMixing() )
	{
		cSatConfInfo.SetMixing();
	}
	else
	{
		cSatConfInfo.SetNoMixing();
	}

    if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
    {
		cSatConfInfo.SetVmpParam( m_tConf.m_tStatus.m_tVMPParam );
    }

	//设置会议是否在画面复合
	if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE  )//&& m_tConf.m_tStatus.GetVmpParam().IsBrdst() )
	{
		if ( m_tConf.m_tStatus.GetVmpParam().IsVMPBrdst() )
        {
			cSatConfInfo.SetVmpBrdst( TRUE );
        }
		else
        {
			cSatConfInfo.SetVmpBrdst( FALSE );
        }
		cSatConfInfo.SetVmp( TRUE );
	}
	else
	{
		cSatConfInfo.SetVmpBrdst( FALSE );
		cSatConfInfo.SetVmp( FALSE );
	}

	//所有终端发送ip地址
    u8 byMpId = 0;
    for (byMpId = 1; byMpId <= MAXNUM_DRI; byMpId ++)
    {
        if ( g_cMcuVcApp.IsMpConnected(byMpId) )
        {
            break;
        }
    }
	cSatConfInfo.SetMpIpAddress( g_cMcuVcApp.GetMpIpAddr(byMpId) );
	
    //终端是否在混音中
	for ( u8 i = 0 ; i < MAXNUM_CONF_MT / 8 ; i++ )
    {
// 		cSatConfInfo.m_byMtInMix[i] = m_tConf.m_tStatus.m_byMtInMix[i];
    }
	//回传列表
	for ( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop ++ )
	{
// 		if ( m_tConf.IsMtCurrUpLoad( g_cMcuAgent.GetId(), byLoop ) )
//         {
// 			cSatConfInfo.AddCurrUpLoadMt( byLoop );
//         }
	}
	//组播会议信息
	cServMsg.SetEventId( 25804 *MCU_MT_CONFSTATUS_NOTIF* );
	cServMsg.SetMsgBody( (u8*)&cSatConfInfo, sizeof( CSatConfInfo ) );
	cServMsg.SetChnIndex( (u8)GetInsID() );
	
	g_cMtSsnApp.SendMultiCastMsg( cServMsg );

    return;
		
}*/


void CMcuVcInst::RefreshConfState( void )
{
	RefreshConfState(TRUE);
	RefreshConfState(FALSE);
}

/*====================================================================
    函数名      ：RefreshConfState
    功能        ：更新会议状态，将业务中的会议信息转化为CSatConfInfo
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/19   1.0          zhangsh       创建
====================================================================*/
void CMcuVcInst::RefreshConfState( BOOL32 bHdGrp )
{
	if (STATE_ONGOING != CurState())
	{
		return;
	}

	CServMsg cServMsg;
	u16 byLoop;
	u16 wPort;
	u8  byFormat;
	cServMsg.SetConfId( m_tConf.GetConfId() );
	
    //消息头
	ITSatMsgHdr tMsgHdr;
	tMsgHdr.SetEventId( 25804 /*MCU_MT_CONFSTATUS_NOTIF*/ );
	//tMsgHdr.SetConfIdx((u8)GetInsID());
	tMsgHdr.SetConfIdx(m_byConfIdx);
	
	tMsgHdr.SetDstId( 0xFFFF );
	tMsgHdr.SetSrcId( 0 ); 
	//tMsgHdr.SetMsgSN( g_cMtSsnApp.GetSN() );
	cServMsg.SetMsgBody( (u8*)&tMsgHdr, sizeof( ITSatMsgHdr ) );
	
    //会议信息
	ITSatConfInfo   tSatConfInfo;

	if (bHdGrp)
	{
		tSatConfInfo.SetAudioType( m_tConf.GetMainAudioMediaType() );
		tSatConfInfo.SetVideoType( m_tConf.GetMainVideoMediaType() );
		tSatConfInfo.SetBitrate( m_tConf.GetBitRate() );
	}
	else
	{
		if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() &&
			0 != m_tConf.GetSecVideoMediaType())
		{
			tSatConfInfo.SetVideoType( GetSatMediaType( m_tConf.GetSecVideoMediaType() ) );
		}
		else
		{
			tSatConfInfo.SetVideoType( GetSatMediaType( m_tConf.GetMainAudioMediaType() ) );
		}
		if (MEDIA_TYPE_NULL != m_tConf.GetSecAudioMediaType() &&
			0 != m_tConf.GetSecAudioMediaType())
		{
			tSatConfInfo.SetAudioType( GetSatMediaType( m_tConf.GetSecAudioMediaType() ) );
		}
		else
		{
			tSatConfInfo.SetAudioType( GetSatMediaType( m_tConf.GetMainAudioMediaType() ) );
		}
		
		if (m_tConf.GetSecBitRate() != 0)
		{
			tSatConfInfo.SetBitrate( m_tConf.GetSecBitRate() );
		}
		else
		{
			tSatConfInfo.SetBitrate( m_tConf.GetBitRate() );
		}
	}

	tSatConfInfo.SetChairId( m_tConf.GetChairman().GetMtId() );
	tSatConfInfo.SetConfLineNum( m_tConf.GetConfAttrbEx().GetSatDCastChnlNum() );
	tSatConfInfo.SetConfName( m_tConf.GetConfName() );
	tSatConfInfo.SetConfSwitchTime( m_tConf.GetDuration() );

	if (bHdGrp)
	{
		tSatConfInfo.SetResolution( m_tConf.GetMainVideoFormat() );
	}
	else
	{
		tSatConfInfo.SetResolution( GetSatRes(m_tConf.GetMainVideoFormat()) );
	}
	
	tSatConfInfo.SetSpeakerId( m_tConf.GetSpeaker().GetMtId() );
	
    //----会议外设
	if ( m_tConf.m_tStatus.IsNoMixing() )
    {
		tSatConfInfo.SetNoMixing();
    }
	else
    {
		tSatConfInfo.SetMixing();
    }
	
	if ( CONF_ENCRYPTMODE_NONE != m_tConf.GetCapSupport().GetEncryptMode() )
    {
		tSatConfInfo.SetConfEncrypted( TRUE );
    }

	if ( CONF_VMPMODE_NONE != m_tConf.m_tStatus.GetVMPMode() )
	{
		tSatConfInfo.SetVmping();
		tSatConfInfo.m_tVmpMt.SetVmpStyle( m_tConf.m_tStatus.GetVmpParam().GetVMPStyle() );
		tSatConfInfo.m_tVmpMt.SetVmpBrd( m_tConf.m_tStatus.GetVmpParam().IsVMPBrdst() );
		
        for ( byFormat=1 ; byFormat <= m_tConf.m_tStatus.GetVmpParam().GetMaxMemberNum() ; byFormat++ )
        {
			if (byFormat >= 16)
			{
				continue;
			}
			tSatConfInfo.m_tVmpMt.SetMtId( byFormat,
                                           m_tConf.m_tStatus.GetVmpParam().GetVmpMember( byFormat)->GetMtId() );
        }
	}
	else
    {
		tSatConfInfo.SetNoVmping();
    }
	
    //－－会议终端
	for ( byLoop = 1; byLoop <= MAXNUM_CONF_MT ; byLoop ++ )
	{
		if ( m_tConfAllMtInfo.MtInConf( byLoop ) )
		{
			if ( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
			{
				BOOL32 bMtHd = ::topoGetMtInfo(LOCAL_MCUID, byLoop, g_atMtTopo, g_wMtTopoNum).IsMtHd();

				if (bMtHd && bHdGrp)
				{
					tSatConfInfo.m_tNonActiveMt.SetMember( byLoop );
					
					if ( m_ptMtTable->IsMtSigned( byLoop ) )
					{
						tSatConfInfo.m_tActiveMt.SetMember( byLoop );
					}
					if ( m_ptMtTable->IsMtCurrUpLoad( byLoop ) )
					{
						tSatConfInfo.m_tCanSendMt.SetMember( byLoop );
					}
				}
				if (!bMtHd && !bHdGrp)
				{
					tSatConfInfo.m_tNonActiveMt.SetMember( byLoop );
					
					if ( m_ptMtTable->IsMtSigned( byLoop ) )
					{
						tSatConfInfo.m_tActiveMt.SetMember( byLoop );
					}
					if ( m_ptMtTable->IsMtCurrUpLoad( byLoop ) )
					{
						tSatConfInfo.m_tCanSendMt.SetMember( byLoop );
					}
				}
			}
		}
		if ( m_ptMtTable->IsMtAudioDumb( byLoop ) )
        {
		    tSatConfInfo.m_tNearMuteMt.SetMember( byLoop );
        }
		if ( m_ptMtTable->IsMtAudioMute( byLoop ) )
        {
			tSatConfInfo.m_tFarMuteMt.SetMember( byLoop );
        }
	}
	
	cServMsg.CatMsgBody( (u8*)&tSatConfInfo, sizeof(ITSatConfInfo) );

	//地址信息
	ITSatConfAddrInfo tAddrInfo;
	tAddrInfo.tAudUnitcastSrcId.Set( 1100 );
	tAddrInfo.tVidUnitcastSrcId.Set( 1101 );

    
	tAddrInfo.tAudMulticastAddr.SetIpAddr(  g_cMcuVcApp.GetMcuMulticastDataIpAddr() );
	tAddrInfo.tAudMulticastAddr.SetPort( g_cMcuVcApp.GetMcuMulticastDataPort() + 2);

	//暂不考虑音频适配，音频维持第一组播地址；
	//非高清终端从第二组播地址接收视频码流
	/*
	//FIXME: 多组播
	if (m_tConf.GetSecVideoMediaType() == MEDIA_TYPE_NULL ||
		m_tConf.GetSecVideoMediaType() == 0)
	{
		tAddrInfo.tVidMulticastAddr.SetIpAddr( g_cMcuVcApp.GetMcuMulticastDataIpAddr() );
		tAddrInfo.tVidMulticastAddr.SetPort( g_cMcuVcApp.GetMcuMulticastDataPort() );
	}
	else
	{
		tAddrInfo.tVidMulticastAddr.SetIpAddr( g_cMcuVcApp.GetMcuMulticastDataIpAddr() );
		tAddrInfo.tVidMulticastAddr.SetPort( g_cMcuVcApp.GetMcuMulticastDataPort() );

		tAddrInfo.tSecVidMulticastAddr.SetIpAddr( g_cMcuVcApp.GetMcuSecMulticastIpAddr() );
		tAddrInfo.tSecVidMulticastAddr.SetPort( g_cMcuVcApp.GetMcuMulticastDataPort() );
	}
	*/

	if (IsHDConf(m_tConf))
	{
		if (!bHdGrp)
		{
			tAddrInfo.tVidMulticastAddr.SetIpAddr( g_cMcuVcApp.GetMcuSecMulticastIpAddr() );
			tAddrInfo.tVidMulticastAddr.SetPort( g_cMcuVcApp.GetMcuMulticastDataPort() );
		}
		else
		{
			tAddrInfo.tVidMulticastAddr.SetIpAddr( g_cMcuVcApp.GetMcuMulticastDataIpAddr() );
			tAddrInfo.tVidMulticastAddr.SetPort( g_cMcuVcApp.GetMcuMulticastDataPort() );
		}
	}
	else
	{
		tAddrInfo.tVidMulticastAddr.SetIpAddr( g_cMcuVcApp.GetMcuMulticastDataIpAddr() );
		tAddrInfo.tVidMulticastAddr.SetPort( g_cMcuVcApp.GetMcuMulticastDataPort() );
	}

    u8 byMpId = 0;
	u32 dwMpIp = 0xffffffff;
	for(byMpId = 1; byMpId <= MAXNUM_DRI; byMpId ++)
	{
		if (g_cMcuVcApp.IsMpConnected(byMpId))
		{
			dwMpIp = g_cMcuVcApp.GetMpIpAddr(byMpId);
		}
	}
	if (dwMpIp == 0xffffffff)
	{
		OspPrintf(TRUE, FALSE, "[RefreshConfState] get mp failed, check it!\n");
	}
	tAddrInfo.tMcuRcvMediaAddr.SetIpAddr( dwMpIp );
	
	wPort = g_cMcuVcApp.GetMcuRcvMtMediaStartPort();
	tAddrInfo.tMcuRcvMediaAddr.SetPort( wPort );

	wPort = g_cMcuVcApp.GetMtRcvMcuMediaPort();
	tAddrInfo.tAudUnitcastAddr.SetPort( wPort + 2 );
	tAddrInfo.tVidUnitcastAddr.SetPort( wPort );

	//tAddrInfo.tAudUnitcastAddr.SetIpAddr( dwMpIp );
	//tAddrInfo.tVidUnitcastAddr.SetIpAddr( dwMpIp );


	//各终端的接收情况
	if ( m_tConf.HasSpeaker() )
    {
		wPort = m_tConf.GetSpeaker().GetMtId();
    }
	else
    {
		wPort = 0;
    }
	for ( byLoop = 1; byLoop <= MAXNUM_CONF_MT ; byLoop ++ )
	{
		if ( MTSEE_BRDCAST == GetMtSrc( byLoop, MODE_VIDEO) )
        {
			//FIXME: 临时测试单播效果
			tAddrInfo.tVidMulticastMmbList.SetMember( byLoop );
        }
		else if ( MTSEE_UNICAST == GetMtSrc( byLoop, MODE_VIDEO) )
        {
			tAddrInfo.tVidUnitcastMmbList.SetMember( byLoop );
        }
		
		/*
		if ( m_tConf.m_tStatus.IsMonitorOutPut() && byLoop == m_tConf.GetChairman().GetMtId() )
		{
			tAddrInfo.tVidUnitcastMmbList.SetMember( byLoop );
			tAddrInfo.tVidMulticastMmbList.RemoveMember( byLoop );
		}*/


		if ( MTSEE_BRDCAST == GetMtSrc( byLoop, MODE_AUDIO) )
        {
			tAddrInfo.tAudMulticastMmbList.SetMember( byLoop );
        }
		else if ( MTSEE_UNICAST == GetMtSrc( byLoop, MODE_AUDIO) )
        {
			tAddrInfo.tAudUnitcastMmbList.SetMember( byLoop );
        }
	}
	if ( !m_tConf.m_tStatus.IsNoMixing() )
    {
		tAddrInfo.tAudMulticastSrcId.Set( BRDCST_MIXN );
    }
	else if ( m_tConf.HasSpeaker() )
    {
		tAddrInfo.tAudMulticastSrcId.Set( m_tConf.GetSpeaker().GetMtId() );
    }
	else
    {
		tAddrInfo.tAudMulticastSrcId.Set( 0 );
    }

	if ( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE && m_tConf.m_tStatus.GetVmpParam().IsVMPBrdst() )
    {
		tAddrInfo.tVidMulticastSrcId.Set( BRDCST_VMP );
    }
	else if ( m_tConf.HasSpeaker() )
    {
		tAddrInfo.tVidMulticastSrcId.Set( m_tConf.GetSpeaker().GetMtId() );
    }
	else
    {
		tAddrInfo.tVidMulticastSrcId.Set( 0 );
    }

	if (bHdGrp)
	{
		//FIXME: 临时粗暴修改，稍后统一整理
		ITSatConfAddrInfoHd tAddrInfoHd;
		memset(&tAddrInfoHd, 0, sizeof(tAddrInfoHd));
		memcpy(&tAddrInfoHd, &tAddrInfo, sizeof(ITSatConfAddrInfo));

		//补充双流部分
		tAddrInfoHd.tDSMulticastSrcId.Set(m_tDoubleStreamSrc.GetMtId());
		
		tAddrInfoHd.tDSMulticastAddr.SetIpAddr(g_cMcuVcApp.GetMcuMulticastDataIpAddr());
		tAddrInfoHd.tDSMulticastAddr.SetPort(g_cMcuVcApp.GetMcuMulticastDataPort() + 4);
		
		for ( byLoop = 1; byLoop <= MAXNUM_CONF_MT ; byLoop ++ )
		{
			BOOL32 bMtHd = ::topoGetMtInfo(LOCAL_MCUID, byLoop, g_atMtTopo, g_wMtTopoNum).IsMtHd();

			if ( bMtHd &&
				 MTSEE_BRDCAST == GetMtSrc( byLoop, MODE_SECVIDEO) &&
				 byLoop != m_tDoubleStreamSrc.GetMtId())
			{
				tAddrInfoHd.tDSMulticastMmbList.SetMember( byLoop );
			}
		}

		cServMsg.CatMsgBody( (u8*)&tAddrInfoHd, sizeof(ITSatConfAddrInfoHd) );
	}
	else
	{
		cServMsg.CatMsgBody( (u8*)&tAddrInfo, sizeof(ITSatConfAddrInfo) );
	}
	
	if (bHdGrp)
	{
		cServMsg.SetChnIndex(SIGNAL_CHN_HD);
	}
	else
	{
		cServMsg.SetChnIndex(SIGNAL_CHN_SD);
	}
    g_cMtSsnApp.SendMultiCastMsg( cServMsg );

	SatLog("[RefreshConfState] Conf Info refreshed once manualy!\n" );

    return;
}

/*====================================================================
    函数名      ：GetMtSrc
    功能        ：卫星会议获取 终端当前源
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/11/10    4.6         zhangbq       创建
====================================================================*/
u8 CMcuVcInst::GetMtSrc(u8 byMtId, u8 byMode)
{
	u8 bySeen = MTSEE_NONE;

	if ( 0 == byMtId || byMtId > MAXNUM_CONF_MT )
	{
		ConfLog( FALSE, "[GetMtSrc] unexpected MtId.%d\n", byMtId );
		return bySeen;
	}
	if (byMode != MODE_AUDIO &&
		byMode != MODE_VIDEO &&
		byMode != MODE_SECVIDEO )
	{
		ConfLog( FALSE, "[GetMtSrc] unexpected Mode.%d for MtId.%d\n", byMode, byMtId );
		return bySeen;
	}

	BOOL32 bRet = FALSE;
	TMtStatus tMtStatus;
	bRet = m_ptMtTable->GetMtStatus(byMtId, &tMtStatus);
	if (!bRet)
	{
		ConfLog(FALSE, "[GetMtSrc] get Mt.%d status failed !\n", byMtId);
		return bySeen;
	}

	switch (byMode)
	{
	case MODE_VIDEO:
		if (tMtStatus.GetSelectMt(byMode).IsNull() && !m_tVidBrdSrc.IsNull())
		{
			bySeen = MTSEE_BRDCAST;
		}
		else if (!tMtStatus.GetSelectMt(MODE_VIDEO).IsNull())
		{
			bySeen = MTSEE_UNICAST;
		}
		break;

	case MODE_AUDIO:
		if (tMtStatus.GetSelectMt(byMode).IsNull() && !m_tAudBrdSrc.IsNull())
		{
			bySeen = MTSEE_BRDCAST;
		}
		else if (!tMtStatus.GetSelectMt(MODE_AUDIO).IsNull())
		{
			bySeen = MTSEE_UNICAST;
		}
		break;

	case MODE_SECVIDEO:
		if (!m_tDoubleStreamSrc.IsNull())
		{
			bySeen = MTSEE_BRDCAST;
		}
		break;
	}
	return bySeen;
}

/*====================================================================
    函数名      ：DaemonProcTimerMultiCast
    功能        ：处理MCU启动消息
    算法实现    ：开始组播会议列表，终端拓扑
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/19   1.0          zhangsh       创建
====================================================================*/
void CMcuVcInst::DaemonProcTimerMultiCast( const CMessage * pcMsg, CApp* pcApp )
{
	CServMsg cServMsg;
	ITSatMsgHdr tMsgHdr;
	u16	     byIndex;
	u8		 bySend;
	BOOL	bFind = FALSE;
	BOOL	bFindKey = FALSE;
	BOOL bResult = FALSE;
	//首先清除原有定时器，可能此函数调用并不是真正的定时器到期
	KillTimer( MCUVC_CONFINFO_MULTICAST_TIMER );
	m_wTicks ++;
	//会议信息
	bySend = m_wTicks % 4;
	if ( bySend == 0 )
	{
		for( byIndex = 0; byIndex < MAXNUM_MCU_CONF+1; byIndex++ )
		{
			CMcuVcInst * pInst = ( CMcuVcInst*)pcApp->GetInstance( byIndex );
			if( pInst != NULL && pInst->CurState() != STATE_IDLE )
			{
				bFind = TRUE;
				break;
			}
		}
		if ( bFind )
			m_bySendBitMap |= 0x01;
		else
			m_bySendBitMap &=0xFE;
	}
	//会议列表信息
	bySend = m_wTicks % 6;
	if ( bySend == 0 )
	{
		m_bySendBitMap |= 0x04;
	}

	//10s后要发送密码信息
	if ( m_wTimes == 3 )
	{
		if ( m_wMtKeyNum == 0 )
		{
			m_wTimes++;
			m_wMtKeyNum = 1;
		}
	}
	//20s后要发送TOPO消息
	if ( m_wTimes == 6 )
	{
		if ( m_wMtTopoNum == 0 )
			m_wMtTopoNum = 1;
	}

	if ( m_wTicks == 7 )
	{
		m_wTicks = m_wTicks % 6;
		m_wTimes ++;
		if ( m_wTimes == 7 )
			m_wTimes = 0;
	}

	//强制发三次组播信息
	if (g_cMcuVcApp.IsConfRefreshNeeded())
	{
		for( byIndex = 1; byIndex <= MAXNUM_MCU_CONF; byIndex++ )
		{
			CMcuVcInst * pInst = ( CMcuVcInst*)pcApp->GetInstance( byIndex );
			if( pInst != NULL && pInst->CurState() != STATE_IDLE )
			{
				u8 byRefreshTimes = g_cMcuVcApp.GetConfRefreshTimes(pInst->m_byConfIdx);
				if (byRefreshTimes != 0)
				{
					byRefreshTimes --;
					g_cMcuVcApp.SetConfRefreshTimes(pInst->m_byConfIdx, byRefreshTimes);
					pInst->RefreshConfState();
					
					SatLog( "[TimerMulticast] Conf Info refreshed once, amoung the three times!\n" );
				}
			}
		}
		
		m_bySendBitMap &=0xFE;
		m_bySendBitMap |= 0x02;
		SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );
		return;
		
	}
	
	//发送会议信息组播包通知
	if ( (m_bySendBitMap & 0x01) == 0x01 )
	{
		for( byIndex = 1; byIndex <= MAXNUM_MCU_CONF; byIndex++ )
		{
			CMcuVcInst * pInst = ( CMcuVcInst*)pcApp->GetInstance( byIndex );
			if( pInst != NULL && pInst->CurState() != STATE_IDLE )
			{
				pInst->RefreshConfState();
				SatLog( "[TimerMulticast] Conf Info refreshed once!\n" );
			}
		}

		m_bySendBitMap &=0xFE;
		m_bySendBitMap |= 0x02;
		SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );
		return;
	}


	//会议信息发送后要发送码率
	if ( ( m_bySendBitMap & 0x02 ) == 0x02 )
	{
		for( byIndex = 1; byIndex < MAXNUM_MCU_CONF+1; byIndex++ )
		{
			CMcuVcInst * pInst = ( CMcuVcInst*)pcApp->GetInstance( byIndex );
			if( pInst != NULL && pInst->CurState() != STATE_IDLE )
			{
				pInst->RefreshMtBitRate();
				SatLog("[TimerMulticast] MT bitrate refreshed once!\n" );
				break;
			}
		}
		m_bySendBitMap &= 0xFD;
		SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );
		return;
	}
	//发送会议列表通知包
	if ( ( m_bySendBitMap & 0x04 ) == 0x04 )
	{

		SatLog( "[TimerMulticast] ConfList refreshed once!\n" );

		tMsgHdr.SetEventId( 25802/*MCU_MT_CONFLIST_NOTIF*/ );
		tMsgHdr.SetConfIdx( 0xFF );
		tMsgHdr.SetDstId( 0xFFFF );
		tMsgHdr.SetSrcId( 0 ); 
		//tMsgHdr.SetMsgSN( g_cMtSsnApp.GetSN() );
		cServMsg.SetMsgBody( (u8*)&tMsgHdr, sizeof( ITSatMsgHdr) );
		
		ITSatTransAddr tMcuSigAddr;
		u32 dwIp;
		u16 wPort;
		dwIp = htonl(g_cMcuAgent.GetMpcIp());
		wPort = g_cMcuVcApp.GetRcvMtSignalPort();
		tMcuSigAddr.SetIpAddr( dwIp );
		tMcuSigAddr.SetPort( wPort );
		cServMsg.CatMsgBody( (u8*)&tMcuSigAddr, sizeof( ITSatTransAddr) );
		
		ITSatConfList acSatConfList[MAXNUM_MCU_CONF];
		memset( &acSatConfList, 0 ,sizeof(acSatConfList) );
		
		u8 byConfNum = 0;
		for( byIndex = 0; byIndex < MAXNUM_MCU_CONF; byIndex++ )
		{
			CMcuVcInst* pInst = ( CMcuVcInst* )pcApp->GetInstance( byIndex );
			
			if( pInst != NULL && pInst->CurState() == STATE_ONGOING )
			{
				acSatConfList[byConfNum].SetConfIdx( (u8)pInst->m_byConfIdx );
				acSatConfList[byConfNum].SetConfName( pInst->m_tConf.GetConfName() );
				byConfNum++;
			}
		}
		cServMsg.SetEventId( 25802/*MCU_MT_CONFLIST_NOTIF*/ );
		cServMsg.CatMsgBody( (u8*)acSatConfList, byConfNum * sizeof( acSatConfList[0] ) );
		cServMsg.SetChnIndex(SIGNAL_CHN_BOTH);
		g_cMtSsnApp.SendMultiCastMsg( cServMsg );
		m_bySendBitMap &=0xFB;
		SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );
		return;
	}

	//发送密码信息
	if ( m_wMtKeyNum != 0 )
	{
		for( byIndex = 0; byIndex < MAXNUM_MCU_CONF+1; byIndex++ )
		{
			CMcuVcInst* pInst = ( CMcuVcInst* )pcApp->GetInstance( byIndex );
			
			if( pInst != NULL && pInst->CurState() != STATE_IDLE )
			{
				bResult |= pInst->RefreshConfMtKey();
				bFindKey = TRUE;
			}
		}

		if ( bResult )
		{
			m_wMtKeyNum = 0;
		}
		if ( bFindKey )
		{
			SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );
			return;
		}

		
	}

	//发送拓扑通知包	
	if( m_wMtTopoNum != 0 )
	{
		OspPrintf( TRUE, FALSE, "ConfTop - %u\n", m_wMtTopoNum );
		tMsgHdr.SetEventId( 25801/*MCU_MT_TOPO_NOTIF*/ );
		tMsgHdr.SetConfIdx( 0xFF );
		tMsgHdr.SetDstId( 0xFFFF );
		tMsgHdr.SetSrcId( 0 ); 
		//tMsgHdr.SetMsgSN( g_cMtSsnApp.GetSN() );
		cServMsg.SetMsgBody( (u8*)&tMsgHdr, sizeof( ITSatMsgHdr) );
		ITSatMultiPackHeader tHeadr;
		ITSatMtInfo	g_atSatMtTopo[40];
		u8 temp = 0;
		temp = g_wMtTopoNum % 40;
		byIndex = g_wMtTopoNum / 40 ;
		if (temp > 0)
			byIndex += 1;
		tHeadr.SetTotalNum( (u8)byIndex );

		temp = m_wMtTopoNum % 40;
		byIndex = m_wMtTopoNum / 40 ;
		if (temp > 0)
			byIndex += 1;
		tHeadr.SetCurrNum( (u8)byIndex );
		
		temp = 0;
		//开始打包
		for( byIndex = 0 ;byIndex < 40 ; byIndex++ )
		{
			g_atSatMtTopo[byIndex].SetMtId( g_atMtTopo[m_wMtTopoNum-1].GetMtId() );
			g_atSatMtTopo[byIndex].SetMtAlias( g_atMtTopo[m_wMtTopoNum-1].GetAlias() );
			g_atSatMtTopo[byIndex].SetMtConnectType( g_atMtTopo[m_wMtTopoNum-1].GetConnected() );
			g_atSatMtTopo[byIndex].SetMtModemIp( htonl( g_atMtTopo[m_wMtTopoNum-1].GetModemIp() ) );
			g_atSatMtTopo[byIndex].SetMtModemPort( g_atMtTopo[m_wMtTopoNum-1].GetModemPort() );
			g_atSatMtTopo[byIndex].SetMtModemType( g_atMtTopo[m_wMtTopoNum-1].GetModemType() );
			g_atSatMtTopo[byIndex].SetMtIp( htonl( g_atMtTopo[m_wMtTopoNum-1].GetIpAddr() ) );
			temp ++;
			m_wMtTopoNum ++;
			if ( (m_wMtTopoNum-1)== g_wMtTopoNum )
			{
				m_wMtTopoNum = 0;
				break;
			}
			if ( temp == 40 )
				break;
		}
		
		tHeadr.SetContentNum( temp );
		//cServMsg.CatMsgBody( (u8*)&g_dwStartTime, sizeof(u32) );

		//FIXME: StartTime 暂时不予处理
		u32 dwStartTime = 0x12345678;
		cServMsg.CatMsgBody( (u8*)&dwStartTime, sizeof(u32) );
		cServMsg.CatMsgBody( (u8*)&tHeadr, sizeof( ITSatMultiPackHeader ) );
		cServMsg.CatMsgBody( (u8*)&g_atSatMtTopo, sizeof(ITSatMtInfo)*temp );
		cServMsg.SetEventId( 25801/*MCU_MT_TOPO_NOTIF*/ );
		cServMsg.SetChnIndex(SIGNAL_CHN_BOTH);
		g_cMtSsnApp.SendMultiCastMsg( cServMsg );
		SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );
		return;
	}
	//设置定时器
	SetTimer( MCUVC_CONFINFO_MULTICAST_TIMER, TIMESPACE_SAT_MULTICAST_CHECK );
	return;

}

/*====================================================================
 函数名      RefreshMtBitRate
 功能        刷新所有终端码率
 算法实现    ：
 引用全局变量：
 输入参数说明：
 返回值说明  ：
 ----------------------------------------------------------------------
 修改记录    ：
 日  期      版本        修改人        修改内容
 03/12/19   1.0          zhangsh       创建
====================================================================*/
void CMcuVcInst::RefreshMtBitRate()
{
	if (STATE_ONGOING != CurState())
	{
		return;
	}

	ITSatMsgHdr tMsgHdr;
	CServMsg cServMsg;
	ITSatBitrate tBitRate;
	u16 wCurAudFlowRate;
	u16 wCurVidFlowRate;
	
	tMsgHdr.SetEventId( 25231/*MCU_MT_SETBITRATE_CMD*/ );
	tMsgHdr.SetConfIdx( 0xFF );
	tMsgHdr.SetDstId( 0xFFFF );
	tMsgHdr.SetSrcId( 0 ); 
	//tMsgHdr.SetMsgSN( g_cMtSsnApp.GetSN() );
	
	cServMsg.SetMsgBody( (u8*)&tMsgHdr, sizeof(ITSatMsgHdr) );
	
	for ( u8 byLoop = 1 ; byLoop < MAXNUM_CONF_MT; byLoop ++ )
	{
		TLogicalChannel tLogicChnVid;
		TLogicalChannel tLogicChnAud;

		BOOL32 bVidRet = m_ptMtTable->GetMtLogicChnnl(byLoop,
												      LOGCHL_VIDEO,
												      &tLogicChnVid, FALSE);

		BOOL32 bAudRet = m_ptMtTable->GetMtLogicChnnl(byLoop,
													  LOGCHL_AUDIO,
													  &tLogicChnAud, FALSE);
		if (!bVidRet && !bAudRet)
		{
			continue;
		}
		wCurAudFlowRate = tLogicChnAud.GetFlowControl();
		wCurVidFlowRate = tLogicChnVid.GetFlowControl();

		if ( wCurAudFlowRate != 0 ||
			 wCurVidFlowRate != 0 )
		{
			//wCurFlowRate = g_cMcuVcApp.GetMtAllowFlowRate( byLoop );
			//wCurFlowRate = m_ptMtTable->GetDialBitrate(byLoop);
			SatLog("[RefreshMtBitRate] Mt.%d's BR has been refreshed to.Aud.%d, Vid.%d!\n", byLoop, wCurAudFlowRate, wCurVidFlowRate );
		}
		else
		{
			//发言人保护
			if (m_tVidBrdSrc == m_ptMtTable->GetMt(byLoop))
			{
				ConfLog( FALSE, "[RefreshMtBitRate] Mt.%d's BR has been refreshed to<Vid.%d, Aud.%d> ignored due to it's speaker!\n", byLoop, wCurVidFlowRate, wCurAudFlowRate );
				continue;
			}
		}

		//FIXME: 临时作弊：稍后终端改, 因为其门限是128k
		if (wCurVidFlowRate == 0)
		{
			wCurAudFlowRate *= 3;
		}


		tBitRate.Set( wCurAudFlowRate + wCurVidFlowRate );

		//zbq 这里可以区分开发，节省信道
		cServMsg.SetChnIndex(SIGNAL_CHN_BOTH);
		cServMsg.CatMsgBody( (u8*)&tBitRate, sizeof( ITSatBitrate ) );
	}
	g_cMtSsnApp.SendMultiCastMsg( cServMsg );

	return;
}


/*====================================================================
函数名      RefreshConfMtKey
功能        发送MT的密码信息
算法实现    ：
引用全局变量：
输入参数说明：const CMessage * pcMsg, 传入的消息
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/01/03    1.0        zhangsh        创建
====================================================================*/
BOOL CMcuVcInst::RefreshConfMtKey( )
{
	if (STATE_ONGOING != CurState())
	{
		return FALSE;
	}
	CServMsg cServMsg;
	ITSatMsgHdr tMsgHdr;
	u16 wMtNum;
	BOOL bResult = FALSE;
	
	tMsgHdr.SetEventId( 25007/*MCU_MT_KEY_NOTIF*/ );
	tMsgHdr.SetConfIdx( 0xFF );
	tMsgHdr.SetDstId( 0xFFFF );
	tMsgHdr.SetSrcId( 0 ); 
	//tMsgHdr.SetMsgSN( g_cMtSsnApp.GetSN() );
	cServMsg.SetMsgBody( (u8*)&tMsgHdr, sizeof( ITSatMsgHdr) );
	
	ITSatMultiPackHeader tHeadr;
	ITSatMtKey	g_atSatMtKey[66];
	
	u8 InstId = (u8)GetInsID();
	
	
	u8 temp = 0;
	temp = g_wMtTopoNum % 66;
	u8 byIndex = g_wMtTopoNum / 66 ;
	if (temp > 0)
		byIndex += 1;
	tHeadr.SetTotalNum( (u8)byIndex );
	
	temp = m_wMtKeyNum % 66;
	byIndex = m_wMtKeyNum / 66 ;
	if (temp > 0)
		byIndex += 1;
	tHeadr.SetCurrNum( (u8)byIndex );
	
	temp = 0;
	//开始打包
	for( byIndex = 0 ;byIndex < 66; byIndex++ )
	{
		for ( wMtNum = m_wMtKeyNum; wMtNum < MAXNUM_CONF_MT ; wMtNum ++ )
		{
			//FIXME: 暂时不处理MtKey相关
			/*
			if ( g_cMcuVcApp.GetMtConfId( wMtNum ) == InstId )
			{
				g_atSatMtKey[byIndex].SetMtKey( wMtNum, (char*)g_cMcuVcApp.GetMtKey( wMtNum ) );
				temp ++;
				m_wMtKeyNum ++;
				break;
			}
			else
			{
				m_wMtKeyNum ++;
			}*/
		}
		if ( temp == 66 )
			break;
	}
	SatLog( "Conf %u MtKey - %u\n" ,InstId, m_wMtKeyNum);
	if ( m_wMtKeyNum == MAXNUM_MCU_MT )
	{
		bResult = TRUE;
		m_wMtKeyNum = 0;
	}
	tHeadr.SetContentNum( temp );
	cServMsg.CatMsgBody( (u8*)&tHeadr, sizeof( ITSatMultiPackHeader ) );
	cServMsg.CatMsgBody( (u8*)&g_atSatMtKey, sizeof(ITSatMtKey)*temp );
	cServMsg.SetEventId( 25007/*MCU_MT_KEY_NOTIF*/ );
	cServMsg.SetChnIndex(SIGNAL_CHN_BOTH);
	g_cMtSsnApp.SendMultiCastMsg( cServMsg );

	return bResult;
}


/*====================================================================
函数名      ProcModemMcuStatusNotif
功能        MODEM状态改变通知
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/11/11   1.0          zhangsh       创建
====================================================================*/
void CMcuVcInst::ProcModemMcuStatusNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt tMt = *(TMt*)cServMsg.GetMsgBody();
	BOOL bLastState = FALSE; //g_cMcuVcApp.IsMtModemError( tMt.GetMtId() );
	BOOL bNeedFresh = FALSE;
	switch( CurState() ) 
	{
	case STATE_MASTER:
		switch ( pcMsg->event )
		{
		case MODEM_MCU_ERRSTAT_NOTIF:
			g_cMcuVcApp.SetMtModemStatus( tMt.GetMtId(), TRUE );
			if ( bLastState != TRUE )
			{
				bNeedFresh = TRUE;
			}
			break;
		case MODEM_MCU_RIGHTSTAT_NOTIF:
			g_cMcuVcApp.SetMtModemStatus( tMt.GetMtId(), FALSE );
			if ( bLastState != FALSE )
			{
				bNeedFresh = TRUE;
			}
			break;
		}
		break;
		default:
			log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
				pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
			break;
	}
	//if ( bNeedFresh )
	MtStatusChange( tMt.GetMtId() );
	//RefreshMtState( tMt.GetMtId() );
}

//END FILE
