/*****************************************************************************
   模块名      : mcu
   文件名      : mcuvcdcs.cpp
   相关文件    : mcuvc.h
   文件实现功能: MCU与DCS业务交换函数
   作者        : 张宝卿
   版本        : V4.0  Copyright(C) 2005-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/12/14  4.0		   张宝卿	   创建                                   
******************************************************************************/

#include "evmcudcs.h"
#include "evmcumcs.h"
#include "evmcueqp.h"
#include "evmcu.h"
#include "mcuvc.h"
//#include "mcudata.h"
//#include "mcustruct.h"
//#include "mcudcspdu.h"

/*=============================================================================
  函 数 名： DaemonProcMcuDcsConnectedNtf
  功    能： (1) 处理DCS注册成功消息
			 (2) 
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/14    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::DaemonProcMcuDcsConnectedNtf( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TDcsInfo tDcsInfo = *(TDcsInfo*)cServMsg.GetMsgBody();
	
	//取DCS的ID号
	if ( 0 == tDcsInfo.m_byDcsId )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "[DcsNtf] the DCS id should not be NULL, ignore it !\n");
		return;
	}
	u8 byDcsId = tDcsInfo.m_byDcsId;	

	//查DCS结构数组
	BOOL32 bNoSpace = TRUE;
    s32 nIndex = 0;
	for( ; nIndex < MAXNUM_MCU_DCS; nIndex ++ )
	{
		if ( g_cMcuVcApp.m_atPeriDcsTable[nIndex].IsNull() )
		{
			g_cMcuVcApp.m_atPeriDcsTable[nIndex].m_byDcsId = byDcsId;
			bNoSpace = FALSE;
			break;
		}
	}
	//没有空间, 异常
	if ( TRUE == bNoSpace || nIndex >= MAXNUM_MCU_DCS )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "[DcsNtf] there is no space for new DCS.%d which is impossible, check it !\n", byDcsId );
		return;
	}
	
	//获取当前注册DCS的地址和端口
	g_cMcuVcApp.m_atPeriDcsTable[nIndex].m_dwDcsIp  = tDcsInfo.m_dwDcsIp;
	g_cMcuVcApp.m_atPeriDcsTable[nIndex].m_wDcsPort = tDcsInfo.m_wDcsPort;	
	
	//刷新DCS当前的状态
	g_cMcuVcApp.SetPeriDcsConnected( g_cMcuVcApp.m_atPeriDcsTable[nIndex].m_byDcsId, TRUE );
	
	//是配置的DCS, 置状态(目前代理只支持一个)
	if ( tDcsInfo.m_dwDcsIp == ntohl( g_cMcuAgent.GetDcsIp()) ) 
	{
		g_cMcuVcApp.SetPeriDcsValid( byDcsId );
	}
	else
	{
		ConfPrint( LOG_LVL_ERROR, MID_MCU_CONF, "Dcs %d register failed due to no config !\n", byDcsId );
		return;
	}

/*
	//代理支持多个DCS
	BOOL32 bValid = FALSE;
	u32 adwDcsIp[MAXNUM_MCU_DCS];
	adwDcsIp = ntohl( g_cMcuAgent.GetDcsIp() );
	for( s32 nIndex = 0; nIndex < MAXNUM_MCU_DCS; nIndex ++ )
	{
		if ( adwDcsIp[nIndex] == tDcsData.m_dwDcsIp )
		{
			g_cMcuVcApp.SetPeriDcsValid( byDcsId );
			bValid = TRUE;
			break;
		}
	}
	if ( FALSE == bValid )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get dcs ip: %s from agent failed !\n", tDcsData.m_dwDcsIp );
		return;
	}
    
    //设置DCS别名(别名转到DCS列表初始化时构造)
	g_cMcuVcApp.SetDcsAlias( g_cMcuVcApp.m_atPeriDcsTable[nIndex].m_byDcsId );
*/
	
    //上报会控
    TPeriDcsStatus tDcsStatus;
    if (g_cMcuVcApp.GetPeriDcsStatus(byDcsId, &tDcsStatus)) 
    {
        CServMsg cTempServMsg;
        cTempServMsg.SetEventId(MCU_MCS_MCUPERIDCSSTATUS_NOTIF);
        cTempServMsg.SetMsgBody( (u8*)&tDcsStatus, sizeof(TPeriDcsStatus) );
        SendMsgToAllMcs( cTempServMsg.GetEventId(), cTempServMsg );
    }

    // N+1模式下通知主mcu数据会议能力变更 [12/21/2006-zbq]
    if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_MASTER_CONNECTED)
    {
		//组织packed EqpCap数据[1/11/2012 chendaiwei]
		//按照目前pack方式，pack后Buffer最大值
		u8 abyBuffer[NPLUS_PACK_EXINFO_BUF_LEN] = {0};
		u16 wLen = 0;
		g_cNPlusApp.PackNplusExInfo(g_cNPlusApp.GetMcuEqpCapEx(),abyBuffer,wLen);		
        TNPlusEqpCap tEqpCap = g_cNPlusApp.GetMcuEqpCap();
        cServMsg.SetEventId(MCU_NPLUS_EQPCAP_NOTIF);
        cServMsg.SetMsgBody((u8 *)&tEqpCap, sizeof(tEqpCap));
		cServMsg.CatMsgBody((u8*)&abyBuffer[0],wLen);
        g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }

    ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Dcs 0x%x register success !\n", tDcsInfo.m_dwDcsIp );
    return;

}

/*=============================================================================
  函 数 名： DaemonProcMcuDcsDisconnectedNtf
  功    能： (1) 清空当前状态
			 (2) 通知所有的会议
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/15    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::DaemonProcMcuDcsDisconnectedNtf( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TDcsInfo tDcsInfo = *(TDcsInfo*)cServMsg.GetMsgBody();
	
	//取DCS的ID号
	if ( 0 == tDcsInfo.m_byDcsId )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "[DcsNtf] the DCS id should not be NULL, ignore it !\n");
		return;
	}
	u8 byDcsId = tDcsInfo.m_byDcsId;	
	
	//取地址和端口
	u32 dwDcsDisconnectIp  = tDcsInfo.m_dwDcsIp;
	//u16 wDcsDisconnectPort = tDcsInfo.m_wDcsPort;
    
	//查找对应的结构成员, 清空状态
	for( s32 nIndex = 0; nIndex < MAXNUM_MCU_DCS; nIndex ++ )
	{
		if ( !g_cMcuVcApp.m_atPeriDcsTable[nIndex].IsNull() )
		{
			if ( byDcsId == g_cMcuVcApp.m_atPeriDcsTable[nIndex].m_byDcsId &&
				 dwDcsDisconnectIp == g_cMcuVcApp.m_atPeriDcsTable[nIndex].m_dwDcsIp  )
			{
                g_cMcuVcApp.SetPeriDcsConnected( byDcsId, FALSE );
				g_cMcuVcApp.m_atPeriDcsTable[nIndex].SetNull();
				break;
			}
		}
	}

	//发消息通知所有会议
	cServMsg.Init();
	cServMsg.SetEventId( MCU_DCSDISCONNECTED_NOTIF );
	cServMsg.SetMsgBody( &byDcsId, sizeof(u8) );
	g_cMcuVcApp.BroadcastToAllConf( MCU_DCSDISCONNECTED_NOTIF, pcMsg->content, pcMsg->length );

    //上报会控
    TPeriDcsStatus tDcsStatus;
    if (g_cMcuVcApp.GetPeriDcsStatus(byDcsId, &tDcsStatus)) 
    {
        CServMsg cTempServMsg;
        cTempServMsg.SetEventId(MCU_MCS_MCUPERIDCSSTATUS_NOTIF);
        cTempServMsg.SetMsgBody( (u8*)&tDcsStatus, sizeof(TPeriDcsStatus) );
        SendMsgToAllMcs( cTempServMsg.GetEventId(), cTempServMsg );
    }  

    // N+1模式下通知主mcu数据会议能力变更 [12/21/2006-zbq]
    if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_MASTER_CONNECTED)
    {
		//组织packed EqpCap数据[1/11/2012 chendaiwei]
		//按照目前pack方式，pack后Buffer最大值
		u8 abyBuffer[NPLUS_PACK_EXINFO_BUF_LEN] = {0};
		u16 wLen = 0;
		g_cNPlusApp.PackNplusExInfo(g_cNPlusApp.GetMcuEqpCapEx(),abyBuffer,wLen);	
        TNPlusEqpCap tEqpCap = g_cNPlusApp.GetMcuEqpCap();
        cServMsg.SetEventId(MCU_NPLUS_EQPCAP_NOTIF);
        cServMsg.SetMsgBody((u8 *)&tEqpCap, sizeof(tEqpCap));
		cServMsg.CatMsgBody((u8*)&abyBuffer[0],wLen);
        g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }
    
    return;
}

/*=============================================================================
  函 数 名： DaemonProcDcsMcuReleaseConfRsp
  功    能： 在Daemon里处理DCS撤会响应
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/16    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::DaemonProcDcsMcuReleaseConfRsp( const CMessage * pcMsg )
{
	//消息检测
	if ( NULL == pcMsg )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get release conf rsp msg failed! \n");
		return;
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs release conf rsp msg: %d<%s> received in inst %d !\n", 
			pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
	}
	
	//消息处理
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	switch( pcMsg->event )
	{
	case DCSSSN_MCU_RELEASECONF_ACK:
		{
			//上报 MCS  DEL 
		}
		break;

	case DCSSSN_MCU_RELEASECONF_NACK:
		{
			//会议索引
			u8 byConfIdx = *( cServMsg.GetMsgBody() );

			//错误类型
			u8 byErrorId = *( cServMsg.GetMsgBody() + sizeof(u8) );
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "release data conf %d failed due to reason: %d !\n", byConfIdx, byErrorId );

			//上报 MCS  DEL
		}
		break;

	case DCSSSN_MCU_CONFRELEASED_NOTIF:
		{
			//清空状态
			m_byDcsIdx = 0;
            m_tConfInStatus.SetDataConfOngoing(FALSE);

			//会议索引
			u8 byConfIdx = *( cServMsg.GetMsgBody() );

			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Data conf %d released succeed on Dcs !\n", byConfIdx );
			
			//上报 MCS ...DEL
		}
		break;

	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected dcs release conf rsp msg received, ignore it !\n");
		break;
	}
}

/*=============================================================================
  函 数 名： ProcMcuDcsCreateConfReq
  功    能： 发给DCS会话创会请求
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/16    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::SendMcuDcsCreateConfReq()
{	
	//会议属性
	switch( m_tConf.GetConfAttrb().GetDataMode() )
	{
	//只支持音视频
	case CONF_DATAMODE_VAONLY:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Conf data mode should not be va only !\n" );
		break;
		
	//支持数据和音视频
	case CONF_DATAMODE_VAANDDATA:	
		{		
			//没有DCS在线
			if ( FALSE == FindOnlineDcs() )
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "data conf: %s created failed due to no DCS registered yet !\n", m_tConf.GetConfName() );
				
				//等待DCS上线, 重新创会
				SetTimer( MCUVC_RECREATE_DATACONF_TIMER, TIMESPACE_RECREATE_DATACONF );

				//上报MCS: 无DCS, 数据会议创建失败
			}
			else
			{
				CServMsg cServMsg;
				memset( &cServMsg, 0, sizeof(cServMsg) );
				
				TDcsConfInfo tDcsConfInfo;
				memset( &tDcsConfInfo, 0, sizeof(tDcsConfInfo) );
				
				//会议id
				cServMsg.SetConfIdx( m_byConfIdx );
				
				//名称
				s8 achConfName[MAXLEN_CONFNAME];
				strncpy( achConfName, m_tConf.GetConfName(), sizeof(achConfName) );
				tDcsConfInfo.SetConfName( achConfName );
				
				//密码
				if ( NULL != m_tConf.GetConfPwd() )
				{
					tDcsConfInfo.SetConfPwd( m_tConf.GetConfPwd() );
				}
				//带宽
				u16 wBandWidth = m_tConf.GetDcsBitRate();
				if ( 0 == wBandWidth )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get dsc data conf bitrate failed !\n");
					return;
				}
				tDcsConfInfo.SetBandwidth( wBandWidth );
				
				//是否接受主动呼入, 现默认为 FALSE
				//应该由MCS来决定, 在界面上作出选择, 相应的会议属性更改, 在此处判断
				tDcsConfInfo.SetSupportJoinMt( FALSE );
				
				cServMsg.SetEventId( MCU_DCSSSN_CREATECONF_REQ );
				cServMsg.SetMsgBody( (u8*)&tDcsConfInfo, sizeof(tDcsConfInfo) );

				SendMsgToDcsSsn( m_byDcsIdx, cServMsg );
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "send to dcsssn create conf %s req success !\n", m_tConf.GetConfName() );
			}
		}
		break;

	//只支持数据
	case CONF_DATAMODE_DATAONLY:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Current system couldn't support data only conf !\n");
		break;
		
	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected data conf mode: %d, ignore it!\n", m_tConf.GetConfAttrb().GetDataMode() );
		break;
	}
	
}

/*=============================================================================
  函 数 名： ProcDcsMcuCreateConfRsp
  功    能： 处理DCS对创会的应答
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/16    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcDcsMcuCreateConfRsp( const CMessage * pcMsg )
{
	//会议状态
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in create conf rsp!\n" );
		return;
	}
	//消息检测
	if ( NULL == pcMsg )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get create conf rsp msg failed! \n");
		return;
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs create conf rsp msg: %d<%s> received in inst %d !\n", 
			pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
	}	
	//消息处理
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	switch( pcMsg->event) 
	{
	case DCSSSN_MCU_CREATECONF_ACK:
		{
			//校验confidx
			if ( m_byConfIdx != cServMsg.GetConfIdx() )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx %d in create Data conf ack should be %d !\n", 
						   cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			
			 //上报 MCS 	 
		}
		break;

	case DCSSSN_MCU_CREATECONF_NACK:
		{
			//校验confidx
			if ( m_byConfIdx != cServMsg.GetConfIdx() )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx %d in create Data conf Nack should be %d !\n", 
					       cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//错误类型
			u8 byErrorId = *cServMsg.GetMsgBody();
			
			if ( !m_tConfInStatus.IsDataConfRecreated() )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "create data conf %s failed due reason: %d", m_tConf.GetConfName(), byErrorId );
			}
			else
			{
				m_tConfInStatus.SetDataConfRecreated(FALSE);
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Recreate data conf %s failed due reason: %d", m_tConf.GetConfName(), byErrorId );
			}
			
			//1. 如果是DCS会议满, 则寻找下一个空闲的DCS
			//2. 错误类型考虑获取描述
			
			//上报 MCS 
			
		}
		break;

	case DCSSSN_MCU_CONFCREATED_NOTIF:
		{
			//校验confidx
			if ( cServMsg.GetConfIdx() != m_byConfIdx )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx %d in create Data conf Notify should be %d !\n", 
						   cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//提取数据会议信息
			TDcsConfInfo tDcsConfInfo = *(TDcsConfInfo*)cServMsg.GetMsgBody();

			//会议名
			u32 dwRet = memcmp( tDcsConfInfo.GetConfName(), m_tConf.GetConfName(), sizeof(u8) * MAXLEN_CONFNAME );
			//不匹配
			if ( 0 !=dwRet )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf name %s in create Data conf Notify should be %s !\n",
						   tDcsConfInfo.GetConfName(), m_tConf.GetConfName() );
				//撤会
				SendMcuDcsReleaseConfReq();
				return;
			}

			//会议密码
			if ( NULL != m_tConf.GetConfPwd() ) 
			{
				dwRet = memcmp( tDcsConfInfo.GetConfPwd(), m_tConf.GetConfPwd(), sizeof(u8) * MAXLEN_PWD );
				//不匹配
				if ( 0 != dwRet )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf pwd %d in create conf Notify should be %s !\n",
							   tDcsConfInfo.GetConfPwd(), m_tConf.GetConfPwd() );
					//撤会
					SendMcuDcsReleaseConfReq();
					return;
				}
			}
			
			//会议码率
			if ( 0 != tDcsConfInfo.GetBandwidth() )
			{
				//MCU指定DCS会议码率大于64, DCS取指定值
				if ( 64 < m_tConf.GetDcsBitRate() ) 
				{
					if ( tDcsConfInfo.GetBandwidth() != m_tConf.GetDcsBitRate() )
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf bandwidth %d in create conf Notify should be %d !\n", 
								   tDcsConfInfo.GetBandwidth(), m_tConf.GetDcsBitRate() );
						//撤会
						SendMcuDcsReleaseConfReq();
						return;
					}
				}
				//MCU指定DCS会议码率小于64, DCS自动调节为64
				else	
				{
					if ( 64 != tDcsConfInfo.GetBandwidth() )
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf bandwidth %d in create conf Notify should be 64 !\n", 
							tDcsConfInfo.GetBandwidth() );
						//撤会
						SendMcuDcsReleaseConfReq();
						return;
					}
				}
			}
			//码率空
			else
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf bandwidth %d in create conf Notify should be %d !\n", 
						 tDcsConfInfo.GetBandwidth(), m_tConf.GetDcsBitRate() );
				//撤会
				SendMcuDcsReleaseConfReq();
				return;
			}
			//是否接受主动呼入
			//取MCS界面设置, 和通知的参数作比较
			/*			
			if ( 界面TRUE  )
			{
				if ( !tDcsConfInfo.IsSupportJoinMt() ) 
				{
					//撤会
					SendMcuDcsReleaseConfReq();
					return;
				}
			}
			else
			{
				if ( tDcsConfInfo.IsSupportJoinMt() ) 
				{
					//撤会
					SendMcuDcsReleaseConfReq();
					return;
				}
			}
			*/
			
            m_tConfInStatus.SetDataConfOngoing(TRUE);

			//如果是重新创建的会议
			if ( m_tConfInStatus.IsDataConfRecreated() )
			{
				m_tConfInStatus.SetDataConfRecreated(FALSE);
				KillTimer( MCUVC_RECREATE_DATACONF_TIMER );

				//重新邀请原先入会的数据终端
				ReInviteHasBeenInDataConfMt();

				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "Data Conf %s Recreated and started on Dcs %d belong to Mcu Inst %d !\n",
					 m_tConf.GetConfName(), m_byDcsIdx, GetInsID() );
			}
			else
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "Data Conf %s created and started on Dcs %d belong to Mcu Inst %d !\n",
					 m_tConf.GetConfName(), m_byDcsIdx, GetInsID() );
			}
						
			 //	上报MCS: 创建数据会议成功
			
		}
		break;

	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected dcs create conf rsp msg %d<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
}

/*=============================================================================
  函 数 名： ProcMcuDcsReleaseConfReq
  功    能： 发给DCS会话撤会请求
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/16    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::SendMcuDcsReleaseConfReq()
{
	//会议状态
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "conf state isn't ongoing, release data conf failed !\n" );
		return;
	}	
	//DCS状态
	if ( !g_cMcuVcApp.IsPeriDcsValid(m_byDcsIdx) || !g_cMcuVcApp.IsPeriDcsConnected(m_byDcsIdx) )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs %d not exist, data conf %s release failed !\n", m_byConfIdx, m_tConf.GetConfName() );
		return;
	}
	
	//发撤会消息
	CServMsg cServMsg;
	cServMsg.SetConfIdx( m_byConfIdx );
	cServMsg.SetEventId( MCU_DCSSSN_RELEASECONF_REQ );
	cServMsg.SetMsgBody();
	SendMsgToDcsSsn( m_byDcsIdx, cServMsg );

}

/*=============================================================================
  函 数 名： SendMcuDcsAddMtReq
  功    能： 发给DCS增加终端的请求
  算法实现： 
  全局变量： 
  参    数： u8 byMtId
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/16    4.0			张宝卿                  创建
  2005/01/06	4.0			张宝卿					策略修改
=============================================================================*/
void CMcuVcInst::SendMcuDcsAddMtReq( u8 byMtId, u32 dwMtIp, u16 wPort )
{
	//会议状态
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in del mt req !\n" );
		return;
	}	
	//本端的DCS ip检测
	if ( 0 == g_cMcuVcApp.m_atPeriDcsTable[m_byDcsIdx - 1].m_dwDcsIp ) 
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Get current dcs Ip failed !\n");
		return;
	}
	//DCS状态
	if ( !g_cMcuVcApp.IsPeriDcsValid(m_byDcsIdx) || !g_cMcuVcApp.IsPeriDcsConnected(m_byDcsIdx) )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs %d not exist, data conf %s release failed !\n", m_byDcsIdx, m_tConf.GetConfName() );
		return;
	}

	TDcsMtInfo tDcsMtInfo;
	memset( &tDcsMtInfo, 0, sizeof(tDcsMtInfo) );

	//终端号和地址
	tDcsMtInfo.m_byMtId = byMtId;
	tDcsMtInfo.m_dwMtIp = m_ptMtTable->GetIPAddr( byMtId );

	//对端主呼
	if ( m_ptMtTable->IsNotInvited( byMtId ) )
	{
		//本端做主
		if ( !m_ptMtTable->IsMtIsMaster( byMtId ) )
		{
			//等待下级加入
			tDcsMtInfo.m_emJoinedType = TDcsMtInfo::emBelowJoin;
			tDcsMtInfo.m_wTimeOut	  = TIME_WAITFORJOIN;	//等待下级加入超时值(s)	
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "mcu data conf %s add mt%d as master request send out !\n", m_tConf.GetConfName(), byMtId );
		}
		//本端做从
		else
		{
			//等待上级MCU邀请
			tDcsMtInfo.m_emJoinedType = TDcsMtInfo::emAboveInvite;
			tDcsMtInfo.m_wTimeOut	  = TIME_WAITFORINVITE;	//等待上级邀请超时值(s)	
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "mcu join mt%d's data conf %s as slave request send out !\n", byMtId, m_tConf.GetConfName() );
		}

	}
	//本端主呼
	else
	{
		//邀请下级加入
		tDcsMtInfo.m_emJoinedType = TDcsMtInfo::emInviteBelow;
		tDcsMtInfo.m_dwMtIp  = dwMtIp;
		tDcsMtInfo.m_wPort   = wPort;
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "mcu data conf %s invite mt%d as master request send out !\n", m_tConf.GetConfName(), byMtId );
	}
	
	//发撤会消息
	CServMsg cServMsg;
	cServMsg.SetConfIdx( m_byConfIdx );
	cServMsg.SetEventId( MCU_DCSSSN_ADDMT_REQ );
	cServMsg.SetMsgBody( (u8*)&tDcsMtInfo, sizeof(tDcsMtInfo) );

	SendMsgToDcsSsn( m_byDcsIdx, cServMsg );

	return;
}

/*=============================================================================
  函 数 名： ProcDcsMcuAddMtRsp
  功    能： 处理DCS增加终端的应答
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/16    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcDcsMcuAddMtRsp( const CMessage * pcMsg )
{
	//会议状态
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in add mt rsp!\n" );
		return;
	}
	//消息检测
	if ( NULL == pcMsg )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get add mt rsp msg failed! \n");
		return;
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs add mt rsp msg: %d<%s> received in inst %d !\n", 
			pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
	}
	//消息处理
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	switch( pcMsg->event )
	{
	case DCSSSN_MCU_ADDMT_ACK:
		{
			//校验confidx
			if ( cServMsg.GetConfIdx() != m_byConfIdx )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx %d received in add mt ack should be %d", 
						 cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//终端ID
			u8 byMtId = *cServMsg.GetMsgBody();
			if ( 0 == byMtId )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong mt id %d received in add mt ack !\n", byMtId );
				return;
			}

			// 上报MCS
		}
		break;

	case DCSSSN_MCU_ADDMT_NACK:
		{
			//校验confidx
			if ( cServMsg.GetConfIdx() != m_byConfIdx )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx %d received in add mt Nack should be %d\n", 
						   cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//终端ID
			u8 byMtId = *cServMsg.GetMsgBody();
			if ( 0 == byMtId )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong mt id %d received in add mt nack !\n", byMtId );
				return;
			}
			//错误类型
			u8 byErrorId = *(cServMsg.GetMsgBody() + sizeof(u8));
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "conf %s add mt%d failed due to reason: %d \n", 
					   m_tConf.GetConfName(), byMtId, byErrorId );

			// 上报MCS
		}
		break;

	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected msg %d<%s> received in add mt rsp 1\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

}

/*=============================================================================
  函 数 名： SendMcuDcsDelMtReq
  功    能： 发给DCS删除终端的请求
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/16    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::SendMcuDcsDelMtReq( u8 byMtId )
{
	//会议状态
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in add mt rsp!\n" );
		return;
	}
	//DCS状态
	if ( !g_cMcuVcApp.IsPeriDcsValid(m_byDcsIdx) || !g_cMcuVcApp.IsPeriDcsConnected(m_byDcsIdx) )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs %d not exist, del data mt%d failed !\n", m_byDcsIdx, byMtId );
		return;
	}

	//发删除消息
	CServMsg cServMsg;
	cServMsg.SetConfIdx( m_byConfIdx );
	cServMsg.SetEventId( MCU_DCSSSN_DELMT_REQ );
	cServMsg.SetMsgBody( &byMtId, sizeof(u8) );
	SendMsgToDcsSsn( m_byDcsIdx, cServMsg );
	return;
}

/*=============================================================================
  函 数 名： ProcDcsMcuDelMtRsp
  功    能： 处理DCS删除终端的应答 
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/16    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcDcsMcuDelMtRsp( const CMessage * pcMsg )
{
	//会议状态
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in del mt rsp!\n" );
		return;
	}
	//消息检测
	if ( NULL == pcMsg )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get del mt rsp msg failed! \n");
		return;
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs del mt rsp msg: %d<%s> received in inst %d !\n", 
			       pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
	}
	//消息处理
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	switch( pcMsg->event )
	{
	case DCSSSN_MCU_DELMT_ACK:
		{
			//校验confidx
			if ( cServMsg.GetConfIdx() != m_byConfIdx )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx %d received in del mt ack should be %d !\n", 
						 cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//终端ID
			u8 byMtId = *cServMsg.GetMsgBody();
			if ( 0 == byMtId )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong mt id %d received in del mt ack !\n", byMtId );
				return;
			}

			//上报MCS
		}
		break;

	case DCSSSN_MCU_DELMT_NACK:
		{
			//校验confidx
			if ( cServMsg.GetConfIdx() != m_byConfIdx )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx %d received in del mt Nack should be %d !\n", 
						   cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//终端ID
			u8 byMtId = *cServMsg.GetMsgBody();
			if ( 0 == byMtId )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong mt id %d received in del mt Nack !\n", byMtId );
				return;
			}
			//错误类型
			u8 byErrorId = *(cServMsg.GetMsgBody() + sizeof(u8) );
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Conf %s del mt%d failed due to reason: %d !\n", m_tConf.GetConfName(), byErrorId );

			//上报MCS

		}
		break;

	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected msg %d<%s> received in del mt rsp !", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}

}

/*=============================================================================
  函 数 名： ProcDcsMcuMtJoinedNtf
  功    能： 处理DCS终端上线通知
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/16    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcDcsMcuMtJoinedNtf( const CMessage * pcMsg )
{
	//会议状态
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in mt joined notify !\n" );
		return;
	}
	//消息检测
	if ( NULL == pcMsg )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get mt joined notify msg failed! \n");
		return;
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs mt joined notify msg: %d<%s> received in inst %d !\n", 
			pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
	}
	//消息处理
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TDcsMtInfo tDcsMtInfo = *(TDcsMtInfo*)cServMsg.GetMsgBody();

	switch( pcMsg->event )
	{
	case DCSSSN_MCU_MTJOINED_NOTIF:
		{
			//校验confidx
			if ( m_byConfIdx != cServMsg.GetConfIdx() )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx: %d received in mt joined conf notify should be %d \n",
						 cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//终端ID
			u8 byMtId = tDcsMtInfo.m_byMtId;
			if ( 0 == byMtId )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong mt id %d received in mt joined conf notify !\n");
				return;
			}
			
			//上线终端是MCU
			if ( TDcsMtInfo::emT120Mcu == tDcsMtInfo.m_byDeviceType )
			{
				//本端做从, 加入上级
				if ( m_ptMtTable->IsMtIsMaster(byMtId) )
				{
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,  "mt%d joined data conf %s as Master_Dcs !\n", byMtId, m_tConf.GetConfName() );
				}
				//本端做主, 下级MCU加入本端
				else
				{
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "mt%d joined data conf %s as Slave_Dcs !\n", byMtId, m_tConf.GetConfName() );
				}
			}
			//上线终端是MT
			else
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "mt%d joined data conf %s as a general DCMT !\n", byMtId, m_tConf.GetConfName() );
			}

			//设置终端当前状态: 在数据会议中
			if ( FALSE == m_ptMtTable->IsMtInDataConf( byMtId ) )
			{
				m_ptMtTable->SetMtInDataConf( byMtId, TRUE );	
			}
			else
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "Mt%d should not in data conf %s already !\n", byMtId, m_tConf.GetConfName() );
				return;
			}

			// 上报MCS
		}
		break;

	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected msg %d<%s> received in mt joined notify !\n" );
		break;
	}
}

/*=============================================================================
  函 数 名： ProcDcsMcuMtLeftNtf
  功    能： 处理DCS终端离线通知
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/16    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcDcsMcuMtLeftNtf( const CMessage * pcMsg )
{
	//会议状态
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in mt left notify !\n" );
		return;
	}
	//消息检测
	if ( NULL == pcMsg )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get mt left notify msg failed! \n");
		return;
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs mt left notify msg: %d<%s> received in inst %d !\n", 
			pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
	}
	//消息处理
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	switch( pcMsg->event )
	{
	case DCSSSN_MCU_MTLEFT_NOTIF:
		{
			//校验confidx
			if ( m_byConfIdx != cServMsg.GetConfIdx() )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong confidx: %d received in mt left conf notify should be %d \n",
						 cServMsg.GetConfIdx(), m_byConfIdx );
				return;
			}
			//终端ID
			u8 byMtId = *cServMsg.GetMsgBody();
			if ( 0 == byMtId )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong mt id %d received in mt left conf notify !\n");
				return;
			}			
			//掉线终端的类型: 后续扩展 ?
			
			//设置终端当前状态: 离数据会议
			if ( TRUE == m_ptMtTable->IsMtInDataConf( byMtId ) )
			{
				m_ptMtTable->SetMtInDataConf( byMtId, FALSE );
			}
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Mt%d left Data Conf %s belong to MCU Inst %d !\n", byMtId, m_tConf.GetConfName(), GetInsID() );
			
			// 上报MCS

		}
		break;
		
	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected msg %d<%s> received in mt left notify !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
}

/*=============================================================================
  函 数 名： ProcDcsMcuStatusNotif
  功    能： DCS上下线通知 
			 (1) 目前只处理下线, 清空状态
			 (2) 重新创建数据会议
			 (3) 后续扩展在这里考虑DCS会议满的处理
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/29    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcDcsMcuStatusNotif( const CMessage * pcMsg )
{
	//会议状态
	if ( STATE_ONGOING != CurState() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "wrong conf state in dcs disconnect notify !\n" );
		return;
	}
	//消息检测
	if ( NULL == pcMsg )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "get dcs status notify msg failed! \n");
		return;
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "dcs status notify msg: %d<%s> received in inst %d !\n", 
			pcMsg->event, OspEventDesc(pcMsg->event), GetInsID() );
	}
	//消息处理
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	switch( pcMsg->event )
	{
	case MCU_DCSDISCONNECTED_NOTIF:
		{
			//本会议建立了数据会议
			if ( 0 != m_byDcsIdx )
			{
				TDcsInfo tDcsInfo = *(TDcsInfo*)cServMsg.GetMsgBody();
				
				//取DCS的ID号
				if ( 0 == tDcsInfo.m_byDcsId )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "[DcsNtf] the DCS id should not be NULL, ignore it !\n");
					return;
				}
				u8 byDcsId = tDcsInfo.m_byDcsId;
				
				//本会议的DCS断链
				if ( byDcsId == m_byDcsIdx )
				{
					//清空状态
					m_byDcsIdx = 0;
                    m_tConfInStatus.SetDataConfOngoing(FALSE);

					//重新创建数据会议
					SetTimer( MCUVC_RECREATE_DATACONF_TIMER, TIMESPACE_RECREATE_DATACONF );                  
				}
			}

            
		}
		break;
	case MCU_DCSCONNCETED_NOTIF:
		{
			//这个消息在DAEMON中已经完成了处理
			//后续扩展考虑发到这里做相关的处理             
		}
		break;

		//DCS会议满, 在此处处理该通知, 标记其状态. 作以下用途:
		//  (1)  如果再进行DCS的创会, 不必查找此DCS
		//	(2)  直到收到会议未满通知, 解除该约束

	default:
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_DCS,  "unexpected dcs status notify msg %d<%s> received !\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
}

/*=============================================================================
  函 数 名： ProcTimerRecreateDataConf
  功    能： 重新查找DCS, 请求创建数据会议

  算法实现： (1) 不管是断链, 还是DCS错步注册导致的重新创会, 第一次重新创会不成功
                 的可能性很小,  如果真的没有成功, 可能是其他的原因( 如: 人为切断
				 网线), 此时, 为减小MCU消耗, 后续的重新创会时间间隔延长一倍, 至10s
			 (2) 置会议重新创建状态, 以对MCU邀请进来的终端发起重新邀请
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/1/10    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcTimerRecreateDataConf( void )
{
	if ( FALSE == FindOnlineDcs() )
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,"There is no DCS online still, recreate data conf %s failed !\n", m_tConf.GetConfName() );

		KillTimer( MCUVC_RECREATE_DATACONF_TIMER );
		SetTimer( MCUVC_RECREATE_DATACONF_TIMER, TIMESPACE_RECREATE_DATACONF * 2 );
		return;
	}
	else
	{
        m_tConfInStatus.SetDataConfRecreated(TRUE);
		SendMcuDcsCreateConfReq();
	}
}

/*=============================================================================
  函 数 名： ReInviteHasBeenInDataConfMt
  功    能： 重新邀请原先在数据会议中的终端
  算法实现： (1) 这里只恢复MCU主呼邀进来的终端
			 (2) 对对端主呼入会的数据终端暂无办法
  全局变量： 
  参    数： 
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/1/10    4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ReInviteHasBeenInDataConfMt()
{			
	for ( s32 nIndex = 1; nIndex <= MAXNUM_CONF_MT; nIndex ++ )
	{
		//当前与会终端是否曾在数据会议中
		if ( m_ptMtTable->IsMtInDataConf( nIndex ) )
		{
			//被邀请的
			if ( !m_ptMtTable->IsNotInvited( nIndex ) )
			{
				u32 dwDcsIp  = 0;
				u16 wDcsPort = 0;
				if ( TRUE == m_ptMtTable->GetMtDcsAddr( nIndex, dwDcsIp, wDcsPort ) )
				{
					//清空状态
					m_ptMtTable->SetMtInDataConf( nIndex, FALSE );
					m_ptMtTable->SetMtDcsAddr( nIndex, 0, 0 );

					//重复邀请终端入会
					SendMcuDcsAddMtReq( nIndex, dwDcsIp, wDcsPort );
				}
			}
			else
			{
				//主呼进来的终端, 这里无法再让它加入 (因无法拿到其DCS地址)
				//如果KEDA终端, 可自定义消息, 让其再加一次, 第三方就没辙了
			}
		}
		m_ptMtTable->SetMtInDataConf( nIndex, FALSE );
	}
}

/*=============================================================================
  函 数 名： FindOnlineDcs
  功    能： 找在线的DCS
  算法实现： 这里存在一个弊端, 没有完全实现对所有DCS作创会试探
  全局变量： 
  参    数： 
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/1/10    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CMcuVcInst::FindOnlineDcs()
{
	for( s32 nIndex = 1; nIndex <= MAXNUM_MCU_DCS; nIndex ++ )
	{
		if ( g_cMcuVcApp.IsPeriDcsValid(nIndex) &&
			 g_cMcuVcApp.IsPeriDcsConnected(nIndex) )
		{
			//保存当前DCS号
			m_byDcsIdx = nIndex;
			return TRUE;		
		}
	}
	return FALSE;

}

// END OF FILE
