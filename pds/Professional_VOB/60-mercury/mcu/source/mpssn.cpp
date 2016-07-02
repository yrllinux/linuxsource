/*****************************************************************************
   模块名      : MP会话模块
   文件名      : mpssn.cpp
   相关文件    : 
   文件实现功能: MP会话模块实例的实现
   作者        : 胡昌威
   版本        : V0.1  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人         修改内容
   2003/07/10  0.1         胡昌威          创建
******************************************************************************/

#include "evmcu.h"
#include "evmp.h"
#include "mcuvc.h"
#include "mpssn.h"
#include "mcuver.h"
//#include "mcuerrcode.h"

CMpSsnApp g_cMpSsnApp;

//构造
CMpSsnInst::CMpSsnInst()
{
    m_byMpId = 0;
}

//析构
CMpSsnInst::~CMpSsnInst()
{

}

/*====================================================================
    函数名      : SendMsgToMp
    功能        ：发消息给MP
    算法实现    ：
    引用全局变量：
    输入参数说明：wEvent 消息ID
				  pbyMsg 消息体
                  wLen   消息长度
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
void CMpSsnInst::SendMsgToMp( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	post(m_dwMpIId,wEvent,pbyMsg, wLen, m_dwMpNode);

	return;
}

/*====================================================================
    函数名      : SendMsgToMcu
    功能        ：发消息给Mcu
    算法实现    ：
    引用全局变量：
    输入参数说明：wEvent 消息ID
				  pbyMsg 消息体
                  wLen   消息长度
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
void CMpSsnInst::SendMsgToMcu( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	g_cMcuVcApp.SendMsgToDaemonConf( wEvent, pbyMsg, wLen );
	
	return;
}

/*====================================================================
    函数名      : InstanceEntry
    功能        ：普通实例入口
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
void CMpSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	switch( pcMsg->event ) 
	{
	  //转发MCU发往MP的消息
	  case MCU_MP_ADDSWITCH_REQ:
	  case MCU_MP_REMOVESWITCH_REQ:
	  case MCU_MP_GETSWITCHSTATUS_REQ:
	  case MCU_MP_RELEASECONF_NOTIFY:
	  case MCU_MP_ADDMULTITOONESWITCH_REQ:		//MCU发给MP的增加多点到一点交换请求
	  case MCU_MP_REMOVEMULTITOONESWITCH_REQ:	//MCU发给MP的移除多点到一点交换请求
	  case MCU_MP_STOPMULTITOONESWITCH_REQ:		//MCU发给MP的停止多点到一点交换请求
	  case MCU_MP_ADDRECVONLYSWITCH_REQ:		//MCU发给MP的增加只接收交换请求
	  case MCU_MP_REMOVERECVONLYSWITCH_REQ:	    //MCU发给MP的移除只接收交换请求
	  case MCU_MP_SETRECVSWITCHSSRC_REQ:	    //MCU发给MP的变更接收交换ssrc请求
      case MCU_MP_CONFUNIFORMMODE_NOTIFY:       //归一重整
      case MCU_MP_ADDBRDSRCSWITCH_REQ:          // MCU发给MP的拆建广播交换的请求 
      case MCU_MP_ADDBRDDSTSWITCH_REQ:
      case MCU_MP_REMOVEBRDSRCSWITCH_REQ:
	  case MCU_MP_GETDSINFO_REQ:
      case MCU_MP_PINHOLE_NOTIFY:
      case MCU_MP_STOPPINHOLE_NOTIFY:
	  case MCU_MP_ADDSENDSELFMUTEPACK_CMD:
	  case MCU_MP_DELSENDSELFMUTEPACK_CMD:
          SendMsgToMp(pcMsg->event,pcMsg->content,pcMsg->length);
		  break;

	  //处理MP注册消息d
	  case MP_MCU_REG_REQ:
		  ProcMpRegisterReq(pcMsg);
		  break;
	  case MP_MCU_GETMSSTATUS_REQ:
      case MCU_MSSTATE_EXCHANGE_NTF:
		  ProcMpGetMsStatusReq(pcMsg);
		  break;
	  //处理Mp断链
	  case OSP_DISCONNECT:
		  ProcMpDisconnect(pcMsg);
		  break;

	  //转发MP发往MCU的消息
	  case MP_MCU_ADDSWITCH_ACK:	  
	  case MP_MCU_REMOVESWITCH_ACK:
	  
	  case MP_MCU_GETSWITCHSTATUS_ACK:
	  case MP_MCU_ADDMULTITOONESWITCH_ACK:		    //MP接受MCU的增加多点到一点交换请求	  
	  case MP_MCU_REMOVEMULTITOONESWITCH_ACK:		//MP接受MCU的移除多点到一点交换请求
	  
	  case MP_MCU_STOPMULTITOONESWITCH_ACK:		    //MP接受MCU的停止多点到一点交换请求
	  case MP_MCU_ADDRECVONLYSWITCH_ACK:		        //MP接受MCU的增加只接收交换请求	  
	  case MP_MCU_REMOVERECVONLYSWITCH_ACK:	        //MP接受MCU的移除只接收交换请求	  
	  case MP_MCU_SETRECVSWITCHSSRC_ACK:	        //MP接受MCU的变更接收交换ssrc请求
	  
      case MP_MCU_ADDBRDSRCSWITCH_ACK:                  //MP接收拆建广播交换的请求
      case MP_MCU_ADDBRDDSTSWITCH_ACK:
      case MP_MCU_REMOVEBRDSRCSWITCH_ACK:
      
      case MP_MCU_ADDSWITCH_NACK:
      case MP_MCU_ADDMULTITOONESWITCH_NACK:		    //MP拒绝MCU的增加多点到一点交换请求

      case MP_MCU_ADDBRDSRCSWITCH_NACK:
      case MP_MCU_ADDBRDDSTSWITCH_NACK:

    case MP_MCU_BRDSRCSWITCHCHANGE_NTF:
        
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[MpSsn]Msg %d(%s) passed for conf.%d\n",
            pcMsg->event, OspEventDesc(pcMsg->event), cServMsg.GetConfIdx());
        
		  g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfIdx(), pcMsg->event, 
									 pcMsg->content, pcMsg->length );
          break;

	  //case MP_MCU_FLUXOVERRUN_NOTIFY:				//zgc [2007/01/11] MP通知MCU码流转发能力超限
      case MP_MCU_FLUXSTATUS_NOTIFY:
	  case MP_MCU_GETDSINFO_ACK:
	  case MP_MCU_GETDSINFO_NACK:
		  SendMsgToMcu(pcMsg->event, pcMsg->content, pcMsg->length);
		  break;

          // 以下暂时不处理
      case MP_MCU_GETSWITCHSTATUS_NACK:
      case MP_MCU_REMOVESWITCH_NACK:
      case MP_MCU_REMOVEMULTITOONESWITCH_NACK:	//MP拒绝MCU的移除多点到一点交换请求
      case MP_MCU_STOPMULTITOONESWITCH_NACK:		//MP拒绝MCU的停止多点到一点交换请求
      case MP_MCU_ADDRECVONLYSWITCH_NACK:		    //MP拒绝MCU的增加只接收交换请求
      case MP_MCU_REMOVERECVONLYSWITCH_NACK:	    //MP拒绝MCU的移除只接收交换请求
      case MP_MCU_SETRECVSWITCHSSRC_NACK:			//MP拒绝MCU的变更接收交换ssrc请求

      case MP_MCU_REMOVEBRDSRCSWITCH_NACK:
		  break;
	  default:
		  break;
	} 

	return;
}

/*=============================================================================
    函 数 名： DaemonInstanceEntry
    功    能： 实例消息处理Daemon入口函数，必须override
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
               CApp* pcApp
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/12  4.0			万春雷                  创建
=============================================================================*/
void CMpSsnInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	if (NULL == pcMsg)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "MpSsn[DaemonInstanceEntry]: The received msg's pointer is NULL!");
		return;
	}

	switch (pcMsg->event)
	{
	case MCU_APPTASKTEST_REQ:			//GUARD Probe Message
		DaemonProcAppTaskRequest( pcMsg );
		break;
	default:
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "MpSsn[DaemonInstanceEntry]: Wrong message %u(%s) received! AppId=%u\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ),pcApp!=NULL?pcApp->appId:0 );
		break;
	}
	
	return;
}

/*=============================================================================
    函 数 名： DaemonProcAppTaskRequest
    功    能： GUARD模块探测消息处理函数
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/12  4.0			万春雷                  创建
=============================================================================*/
void CMpSsnInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );

	return;
}

/*====================================================================
    函数名      : ProcMpRegisterReq
    功能        ：处理Mp的注册消息
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
void CMpSsnInst::ProcMpRegisterReq( CMessage * const pcMsg)
{
    CServMsg cServMsg( pcMsg->content ,pcMsg->length );
	TMp tMp = *(TMp*)cServMsg.GetMsgBody();
	// 增加版本校验,zgc,2007-09-28
	// 移到TMp结构中, zgc, 2007-10-10
	//	u16 wVersion = *(u16*)(cServMsg.GetMsgBody() + sizeof(TMp));
	//	wVersion = ntohs(wVersion);
	
    u16 wRecvStartPort = g_cMcuAgent.GetRecvStartPort();
	u8 byIsRestrictFluxOfMcuMp = g_cMcuVcApp.GetPerfLimit().IsLimitMp() ? 1 : 0;
	u32 dwSysSSrc = htonl(g_cMSSsnApp.GetMSSsrc());
	
	switch( CurState() )
	{
	case STATE_IDLE:
		//zbq[12/11/2007]状态机严格翻转
		//case STATE_NORMAL:
		{    
			//保存Mp的Node和IId
			m_dwMpNode = pcMsg->srcnode;
			m_dwMpIId = pcMsg->srcid;
			m_byMpId = tMp.GetMpId();
			m_byMpAttachMode = tMp.GetAttachMode();
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)		 
			SetMpIp( tMp.GetIpAddr());
#endif
			// 消息体长度校验, zgc, 2007-10-10
			if (cServMsg.GetMsgBodyLen() != sizeof(TMp))
			{
				LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "MpSsn: Mp(0x%x) register invalid msg len:%d(should be %d).\n", 
					tMp.GetIpAddr() ,cServMsg.GetMsgBodyLen(), sizeof(TMp));
				SendMsgToMp(pcMsg->event+2,pcMsg->content,pcMsg->length);
				::OspDisconnectTcpNode( pcMsg->srcnode );
				return;
			}
			
			//对Mp的Id进行判断
			if( m_byMpId != (u8)GetInsID() )
			{
				//NACK
				LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR,"Mp%d(0x%x) register to instance %d, Nack!\n", m_byMpId, tMp.GetIpAddr() , GetInsID());
				SendMsgToMp(pcMsg->event+2,pcMsg->content,pcMsg->length);
				if (m_byMpAttachMode != 1)
				{
					::OspDisconnectTcpNode( pcMsg->srcnode );
				}			
				return;
			}
			
			//认证,是否是本MCU的单板
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)// [2/25/2010 xliang] 8000E 系列不进行下面校验，因为没有单板配置项
#ifndef WIN32
			if( tMp.GetIpAddr() != ntohl( g_cMcuAgent.GetBrdIpAddr( m_byMpId ) ) )
			{
				//NACK
				LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR,"other mcu board mp[Ip= %0x, Id= %d] register, nack!\n", tMp.GetIpAddr(), m_byMpId);
				SendMsgToMp(pcMsg->event+2,pcMsg->content,pcMsg->length);
				if (m_byMpAttachMode != 1)
				{
					::OspDisconnectTcpNode( pcMsg->srcnode );
				}		   
				return;
			}
#endif
#endif
			// 版本验证失败，拒绝注册, zgc, 2007-09-28
			if ( tMp.GetVersion() != DEVVER_MP )
			{
				cServMsg.SetErrorCode(ERR_MCU_VER_UNMATCH);
				post(pcMsg->srcid, pcMsg->event + 2,
					cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
				
				LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "MpSsn: Mp.%d(0x%x) version %d differs from MCU %d, NACK!\n",                       
					GetInsID(),
					tMp.GetIpAddr(),
					tMp.GetVersion(),
					DEVVER_MP );
				
				
				// 非内置，断链
				if (m_byMpAttachMode != 1)
				{
					::OspDisconnectTcpNode(pcMsg->srcnode);
				}
				return;
			}
			
			LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[MpSsn]: Mp %d register to mcu success!\n", tMp.GetMpId() );
			
			//通知Mcu
			SendMsgToMcu(pcMsg->event,pcMsg->content,pcMsg->length);
			
			//注册断链接收实例
			::OspNodeDiscCBRegQ( pcMsg->srcnode, GetAppID(), GetInsID() );
			
			// guzh [6/12/2007] 增加会话值通知
			cServMsg.SetMsgBody( (u8*)&dwSysSSrc, sizeof(dwSysSSrc) );
			cServMsg.CatMsgBody( (u8*)&wRecvStartPort, sizeof(u16) );
			
			//zgc,2007/01/11,添加是否限制MP转发能力标志
			cServMsg.CatMsgBody( (u8*)&byIsRestrictFluxOfMcuMp, sizeof(u8) );
			
			// guzh [1/17/2007] 根据MP类型确定其码流限制的能力
			u16 wMaxCap = 0;
#ifdef _MINIMCU_
			// 对于8000B，要区分 MPC/MDSC/HDSC
			if ( m_byMpAttachMode == 1 )
			{
				wMaxCap = g_cMcuVcApp.GetPerfLimit().m_wMpcMaxMpAbility;
			}
			else
			{
				// 为了防止用户配置错误，直接采用内部默认值
				// u8 byType = BrdExtModuleIdentify();
				u8 byType = g_cMcuAgent.GetRunBrdTypeByIdx( m_byMpId );
				if ( byType == 0xFF )
				{
					wMaxCap = 0;
				}
				else if ( byType == BRD_TYPE_HDSC/*DSL8000_BRD_HDSC*/ )
				{
					wMaxCap = MAXLIMIT_CRI_MP_HDSC;
				}
				else
				{
					wMaxCap = MAXLIMIT_CRI_MP_MDSC;
				}
			}
#else

			u8 byBrdId = g_cMcuAgent.GetBrdIdbyIpAddr(tMp.GetIpAddr());
			TBoardInfo tBrdInfo;
			g_cMcuAgent.GetBrdCfgById(byBrdId,&tBrdInfo);

			if ( m_byMpAttachMode == 1 )
			{
				wMaxCap = g_cMcuVcApp.GetPerfLimit().m_wMpcMaxMpAbility;
			}
			else
			{
				//zhouyiliang 20110113 is2.2板扩容
				wMaxCap = g_cMcuVcApp.GetPerfLimit().m_wCriMaxMpAbility;
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)//8000G转发板都是内置,用默认值
				u8 byType = g_cMcuAgent.GetRunBrdTypeByIdx( m_byMpId );
				if( BRD_TYPE_IS22 == byType )
				{
					wMaxCap = MAXLIMIT_CRI_MP_EX;
				}
				else if (BRD_TYPE_CRI2 == byType) 
				{
					//前网口
					if( tBrdInfo.GetPortChoice() == 0)
					{
						wMaxCap = MAXLIMIT_CRI_MP_EX;
					}
				}		
#endif
			}
#endif
			wMaxCap = htons(wMaxCap);
			cServMsg.CatMsgBody( (u8*)&wMaxCap, sizeof(u16) );

            //打洞时间间隔
            u32 dwPinHoleInterval = g_cMcuVcApp.GetPinHoleInterval();
            dwPinHoleInterval = htonl(dwPinHoleInterval);
            cServMsg.CatMsgBody( (u8*)&dwPinHoleInterval, sizeof(u32) );

			u8 bySendMutePackNum = g_cMcuVcApp.GetSendMutePackNum();
            cServMsg.CatMsgBody( (u8*)&bySendMutePackNum, sizeof(u8) );

			u8 bySendMutePackInterval = g_cMcuVcApp.GetSendMutePackInterval();
            cServMsg.CatMsgBody( (u8*)&bySendMutePackInterval, sizeof(u8) );
			
			//ACK
			SendMsgToMp( pcMsg->event+1,cServMsg.GetServMsg(),cServMsg.GetServMsgLen() );
			
			NEXTSTATE( STATE_NORMAL );
			break;
     }
	 default:
         LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "EqpSsn: Wrong message %u(%s) received in current state %u!\n",
			 pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());         
		 break;	   
	}
}

/*====================================================================
    函数名      : ProcMpDisconnect
    功能        ：Mp断链处理
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
void CMpSsnInst::ProcMpDisconnect(CMessage * const pcMsg)
{
	LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[MpSsn]: MP disconnect messege %u(%s) received!,Dri id is:%d.\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byMpId );

	if( *( u32 * )pcMsg->content == m_dwMpNode )	//本实例对应连接断
	{
        if (INVALID_NODE != m_dwMpNode)
        {
            OspDisconnectTcpNode(m_dwMpNode);
        }
        
		m_byMpId = 0;
		m_dwMpNode = INVALID_NODE;
		SetMpIp(0);	// [12/17/2009 xliang] 

		TMp tMp;
        tMp.SetMpId((u8)GetInsID());

		//[2011/09/02/zhangli]mpIp下发给会议，会议根据此信息清除RTCP交换
		//在data层MP信息被remove，会议中取不到ip信息，这里需要带下去
		u32 dwMpIp = g_cMcuVcApp.GetMpIpAddr(tMp.GetMpId());
		tMp.SetIpAddr(dwMpIp);

		CServMsg cServMsg;
		cServMsg.SetMsgBody((u8 *)&tMp,sizeof(tMp));

		SendMsgToMcu(MCU_MP_DISCONNECTED_NOTIFY,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());

		NEXTSTATE( STATE_IDLE );
	}

	return;
}

/*=============================================================================
  函 数 名： ProcMpGetMsStatusReq
  功    能： 取主备倒换状态
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CMpSsnInst::ProcMpGetMsStatusReq(CMessage* const pcMsg)
{
    // guzh [4/11/2007] 如果没有通过注册就发送获取请求，很有可能是重新启动前的连接，不理睬
    if ( CurState() == STATE_IDLE )
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[ProcMpGetMsStatusReq]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        
        return;
    }
    
    if (MCU_MSSTATE_EXCHANGE_NTF == pcMsg->event)
    {
        // 如果同步失败,断开相应的连接
        u8 byIsSwitchOk = *pcMsg->content;
        if (0 == byIsSwitchOk)
        {
            OspDisconnectTcpNode( m_dwMpNode );
            return;
        }        
    }

	if( MCU_MSSTATE_ACTIVE == g_cMSSsnApp.GetCurMSState() ) // 不是主板时让外设自己超时
    {
        TMcuMsStatus tMsStatus;
        tMsStatus.SetMsSwitchOK(g_cMSSsnApp.IsMsSwitchOK());
        
        CServMsg cServMsg;
        cServMsg.SetMsgBody((u8*)&tMsStatus, sizeof(tMsStatus));
        SendMsgToMp(MCU_MP_GETMSSTATUS_ACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

        //LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMpGetMsStatusReq] IsMsSwitchOK :%d.\n", tMsStatus.IsMsSwitchOK());
    }
    return;
}
//CMpConfig
/*--------------------------------------------------------------------------------*/

//构造
CMpConfig::CMpConfig()
{

}

//析构
CMpConfig::~CMpConfig()
{

}

/*====================================================================
    函数名      : SendMsgToMpSsn
    功能        ：发消息给Mp会话
    算法实现    ：
    引用全局变量：
    输入参数说明：byMpId Mp编号
	              wEvent 消息ID
				  pbyMsg 消息体
                  wLen   消息长度
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
void CMpConfig::SendMsgToMpSsn( u8 byMpId, u16 wEvent,  u8 * const pbyMsg, u16 wLen)
{
	//本端主用则允许向外部各App投递消息，否则丢弃
	if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
	{
		return;
	}
    
    ::OspPost( MAKEIID( AID_MCU_MPSSN, byMpId), wEvent, pbyMsg, wLen );

	return;
}


/*====================================================================
    函数名      : BroadcastToAllMpSsn
    功能        ：发消息给所有已连接的Mp会话
    算法实现    ：
    引用全局变量：
    输入参数说明：wEvent 消息ID
				  pbyMsg 消息体
                  wLen   消息长度
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
void CMpConfig::BroadcastToAllMpSsn(u16 wEvent, u8 * const pbyMsg, u16 wLen)
{
	//本端主用则允许向外部各App投递消息，否则丢弃
	if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
	{
		return;
	}

    ::OspPost( MAKEIID( AID_MCU_MPSSN, CInstance::EACH ), wEvent, pbyMsg, wLen );

	return;
}


//END OF FILE
