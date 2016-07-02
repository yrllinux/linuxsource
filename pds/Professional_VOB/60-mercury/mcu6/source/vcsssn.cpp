/*****************************************************************************
   模块名      : mcu
   文件名      : vcsssn.cpp
   相关文件    : vcsssn.h
   文件实现功能: MCU与VCS会话类
   作者        : 付秀华
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2008/11/20              付秀华      创建
******************************************************************************/



#include "evmcu.h"
#include "evmcumcs.h"
#include "evmcuvcs.h"
#include "evmcutest.h"
#include "mcuvc.h"
//#include "mcuerrcode.h"
//#include "umconst.h"
#include "nplusmanager.h"
#include "mcucfg.h"
#include "vcsssn.h"

#ifdef _VXWORKS_
#include "brddrvLib.h"
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

CVcsSsnApp	g_cVcsSsnApp;	                   // VCS会话应用实例

#ifdef _UTF8
	CUsrManage  g_cVCSUsrManage((u8*)DIR_DATA, (u8*)MCU_VCSUSER_FILENAME, ENCODE_UTF8 , TRUE);   //VCS的用户管理对象
#else
	CUsrManage  g_cVCSUsrManage((u8*)DIR_DATA, (u8*)MCU_VCSUSER_FILENAME);   //VCS的用户管理对象 // 以前版本默认GBK不用转 [pengguofeng 7/18/2013]
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVcsSsnInst::CVcsSsnInst( void )
{

}

CVcsSsnInst::~CVcsSsnInst( void )
{

}

/*====================================================================
    函数名      ：DaemonInstanceEntry
    功能        ：Daemon实例消息处理入口函数，必须override
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/11/20              付秀华        创建
====================================================================*/
void CVcsSsnInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "CVcsSsnInst: The received msg's pointer in the msg entry is NULL!");
		return;
	}

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[VcsSsnDaem%u]: Event %u(%s) received!\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));

	for( u8 byLoop = 0; byLoop < MAXNUM_MCU_VC; byLoop++ )
	{
		if( pcMsg->event == (MCUVC_RELEASECONF_TIMER + byLoop) )
		{
			DaemonProcWaitReleaseTimeOut( pcMsg );
			return;
		}
	}
	
	switch( pcMsg->event )
	{
	case MCUVC_VCSSN_MMCURLSCTRL_CMD:
		{
			CServMsg cServMsg(pcMsg->content, pcMsg->length);
			if (!g_cVcsSsnApp.ChkVCConfStatus(cServMsg.GetConfId()))
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "MMCU conf is over and no other vcs is in control of this conf now. Release it!\n");
				g_cVcsSsnApp.VCSRlsConf(cServMsg.GetConfId());
			}	
		}
		break; 

	case OSP_POWERON:
		VCSDaemonProcPowerOn();
		break;

	case MCUVC_STARTRELEASECONFTIMER_CMD:
		{
			u8 byWaitTimerIdx = *(u8 *)(pcMsg->content);
			SetTimer(MCUVC_RELEASECONF_TIMER + byWaitTimerIdx, g_cMcuVcApp.GetDelayReleaseConfTime()*60*1000);
		}
		break;

	default:
		CMcsSsnInst::DaemonInstanceEntry( pcMsg, pcApp );
		break;
	}
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
    2008/11/20              付秀华        创建
====================================================================*/

#define VCS_EV_LOG(level)	MCU_EV_LOG(level, MID_MCU_VCS, "Vcs")

void CVcsSsnInst::InstanceEntry( CMessage * const pcMsg )
{

	// 对于设置为无效状态的实例不进行任何消息处理
	if (CurState() == STATE_INVALID)
	{
		return;
	}

	if (NULL == pcMsg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[CVcsSsnInst%u]: The received msg's pointer in the msg entry is NULL!", GetInsID());
		return;
	}

// 	if (MCU_MCS_MTSTATUS_NOTIF != pcMsg->event)
// 	{
// 		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS, "[Vcs%u]: Event %u(%s) recv\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));
// 	}

	switch (pcMsg->event)
	{
	// 初始化VCS普通实例的状态
	case OSP_POWERON:
		{
			VCS_EV_LOG(LOG_LVL_KEYSTATUS);
			// 对于VCS创建32个实例，只有后16个实例有效，支持16个VCS同时连接
			if (GetInsID() <= MAXNUM_MCU_MC)
			{
				NEXTSTATE(STATE_INVALID) ;
			}
		}
    	break;

	case OSP_DISCONNECT:				       //控制台断链通知	
		VCS_EV_LOG(LOG_LVL_KEYSTATUS);
		QuitCurVCConf( FALSE );
		CMcsSsnInst::InstanceEntry( pcMsg );   //基类完成对应断链处理 
		break;                       

    // VCS用户管理操作,共用原有的MCS消息，但使用不同的用户信息
    case MCS_MCU_ADDUSER_REQ:           //VCS请求MCU添加用户
    case MCS_MCU_DELUSER_REQ:           //VCS请求MCU删除用户
    case MCS_MCU_GETUSERLIST_REQ:       //VCS请求MCU得到用户列表
    case MCS_MCU_CHANGEUSER_REQ:        //VCS请求MCU修改用户信息

    case MCS_MCU_ADDUSERGRP_REQ:        //VCS请求MCU添加用户组
    case MCS_MCU_CHANGEUSERGRP_REQ:     //VCS请求MCU修改用户组
    case MCS_MCU_DELUSERGRP_REQ:        //VCS请求MCU删除用户组
    case MCS_MCU_GETUSERGRP_REQ:        //VCS请求MCU获取用户组信息
		VCS_EV_LOG(LOG_LVL_KEYSTATUS);
        ProcVCSUserManagerMsg( pcMsg );
        break;

	// VCS软件名称自定义
	case VCS_MCU_GETSOFTNAME_REQ:
	case VCS_MCU_CHGSOFTNAME_REQ:
		VCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcVCSSoftNameMsg( pcMsg );
		break;
		
	case VCS_MCU_SETUNPROCFGINFO_REQ:
	case VCS_MCU_GETUNPROCFGINFO_REQ:
		VCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcVCSMcuUnProcCfgMsg( pcMsg );
		break;

	// 模板相关操作
	case MCS_MCU_MODIFYTEMPLATE_REQ:        //VCS修改模板
	case MCS_MCU_DELTEMPLATE_REQ:           //VCS删除模板
		VCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcVCSConfTemMsg( pcMsg );
		break;              

	case VCS_MCU_OPRCONF_REQ:           //VCS请求操作某调度席
	case VCS_MCU_QUITCONF_REQ:          //VCS请求退出某调度席
		VCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcVCSConfOprMsg( pcMsg );
		break;

	case VCS_MCU_CREATECONF_BYTEMPLATE_REQ: //VCS通过会议模板创建会议
	case VCS_MCU_SENDPACKINFO_REQ:
		VCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcVCSMcuVCDaemonMsg( pcMsg );
		break;

	case VCS_MCU_VCMT_REQ:                  //VCS调度请求	
	case VCS_MCU_VCMODE_REQ:                //会议当前的调度模式请求    
	case VCS_MCU_CHGVCMODE_REQ:             //切换会议模式
    case VCS_MCU_MUTE_REQ:                  //VCS静哑音请求
	case VCS_MCU_GROUPCALLMT_REQ:           //VCS组呼请求
	case VCS_MCU_GROUPDROPMT_REQ:           //VCS停止组呼请求
	case VCS_MCU_STARTCHAIRMANPOLL_REQ:     //VCS请求开启主席轮询  
	case VCS_MCU_STOPCHAIRMANPOLL_REQ:      //VCS请求停止主席轮询
	case VCS_MCU_ADDMT_REQ:                 //VCS临时增加终端
	case VCS_MCU_DELMT_REQ:                 //VCS删除临时增加终端

	case VCS_MCU_RELEASEMT_ACK:             //VCS同意释放终端
	case VCS_MCU_RELEASEMT_NACK:            //VCS拒绝释放终端

	case VCS_MCU_SETMTINTVWALL_REQ:			//VCS请求将终端放入电视墙通道
	
	//zjj20091102
	case VCS_MCU_ADDPLANNAME_REQ:			//VCS请求添加预案
	case VCS_MCU_DELPLANNAME_REQ:			//VCS请求删除预案
	case VCS_MCU_MODIFYPLANNAME_REQ:		//VCS请求修改预案名称
	//case VCS_MCU_SAVEPLANDATA_REQ:			//VCS请求保存某个预案
	case VCS_MCU_GETALLPLANDATA_REQ:		//VCS请求获得所有预案数据信息
	case VCS_MCU_CHGTVWALLMODE_REQ:			//vcs请求mcu改变电视墙模式
	//case VCS_MCU_REVIEWMODEMTINTVWALL_REQ:	//VCS通知mcu终端进入电视墙通道(只限于在预览模式下)
	case VCS_MCU_CLEARONETVWALLCHANNEL_CMD:	//VCS通知mcu清除某个电视墙通道信息(只限于手动模式)
	case VCS_MCU_CLEARALLTVWALLCHANNEL_CMD:	//VCS通知mcu清除全部电视墙通道信息(只限于手动模式)
	case VCS_MCU_TVWALLSTARMIXING_CMD:		//vcs通知mcu设置电视墙通道终端混音状态
	case VCS_MCU_CHGDUALSTATUS_REQ:
	case VCS_MCU_START_MONITOR_UNION_REQ:	//VCS 开启监控联动
	case VCS_MCU_STOP_MONITOR_UNION_REQ:	//VCS 停止监控联动
	case VCS_MCU_STOP_ALL_MONITORUNION_CMD:	//VCS 关闭会议所有监控联动
	case MCS_MCU_GETIFRAME_CMD:				//UI ask for I Frame

	case VCS_MCU_MTJOINCONF_ACK:			//终端入会申请同意
	case VCS_MCU_MTJOINCONF_NACK:			//终端入会申请拒绝
    case VCS_MCU_GROUPMODELOCK_CMD:	//[5/19/2011 zhushengze]组呼会议加解锁操作
    case MCS_MCU_TRANSPARENTMSG_NOTIFY:     //[2/23/2012 zhushengze]界面、终端消息透传
		VCS_EV_LOG(LOG_LVL_DETAIL);
		ProcVCSMcuVCMsg( pcMsg );
		break;

	//应答消息
	case MCU_VCS_OPRCONF_ACK:
	case MCU_VCS_OPRCONF_NACK:
	case MCU_VCS_QUITCONF_ACK:
	case MCU_VCS_QUITCONF_NACK:		

	case MCU_VCS_CREATECONF_BYTEMPLATE_ACK:
	case MCU_VCS_CREATECONF_BYTEMPLATE_NACK:
		
	case MCU_VCS_VCMT_ACK:
	case MCU_VCS_VCMT_NACK:
	case MCU_VCS_VCMT_NOTIF:

	case MCU_VCS_VCMODE_ACK:
	case MCU_VCS_VCMODE_NACK:

    case MCU_VCS_CHGVCMODE_ACK:
	case MCU_VCS_CHGVCMODE_NACK:
	case MCU_VCS_CHGVCMODE_NOTIF:

    case MCU_VCS_MUTE_ACK:
	case MCU_VCS_MUTE_NACK:

    case MCU_VCS_GROUPCALLMT_ACK:
	case MCU_VCS_GROUPCALLMT_NACK:
	case MCU_VCS_GROUPCALLMT_NOTIF:

	case MCU_VCS_GROUPDROPMT_ACK:
	case MCU_VCS_GROUPDROPMT_NACK:

    case MCU_VCS_STARTCHAIRMANPOLL_ACK:
	case MCU_VCS_STARTCHAIRMANPOLL_NACK:
	case MCU_VCS_STOPCHAIRMANPOLL_ACK:
	case MCU_VCS_STOPCHAIRMANPOLL_NACK:

	case MCU_VCS_ADDMT_ACK:
	case MCU_VCS_ADDMT_NACK:
	case MCU_VCS_DELMT_ACK:
	case MCU_VCS_DELMT_NACK:
		
    case MCU_VCS_CONFSTATUS_NOTIF:

	case MCU_VCS_GETSOFTNAME_ACK:
	case MCU_VCS_GETSOFTNAME_NACK:
    case MCU_VCS_CHGSOFTNAME_ACK:
	case MCU_VCS_CHGSOFTNAME_NACK:
	case VCS_MCU_SOFTNAME_NOTIF:

    case MCU_VCS_SETUNPROCFGINFO_NOTIF:
		
    case MCU_VCS_RELEASEMT_REQ:
	case MCU_VCS_RELEASEMT_NOTIF:

	case MCU_VCS_SETMTINTVWALL_ACK:
	case MCU_VCS_SETMTINTVWALL_NACK:

	//zjj20091102
	case MCU_VCS_ADDPLANNAME_ACK:
	case MCU_VCS_ADDPLANNAME_NACK:
	case MCU_VCS_DELPLANNAME_ACK:
	case MCU_VCS_DELPLANNAME_NACK:	
	case MCU_VCS_MODIFYPLANNAME_ACK:
	case MCU_VCS_MODIFYPLANNAME_NACK:
	//case MCU_VCS_SAVEPLANDATA_ACK:
	//case MCU_VCS_SAVEPLANDATA_NACK:
	case MCU_VCS_GETALLPLANDATA_ACK:
	case MCU_VCS_GETALLPLANDATA_NACK:
	case MCU_VCS_CHGTVWALLMODE_ACK:
	case MCU_VCS_CHGTVWALLMODE_NACK:
	//case MCU_VCS_REVIEWMODEMTINTVWALL_ACK:
	//case MCU_VCS_REVIEWMODEMTINTVWALL_NACK:
	//zjj20091102
	case MCU_VCS_ONEPLANDATA_NOTIFY:
	case MCU_VCS_ALLPLANNAME_NOTIFY:
	case MCU_VCS_ALLPLANDATA_SINGLE_NOTIFY:
	case MCU_VCS_MODIFYPLANNAME_NOTIFY:

	case MCU_VCS_SENDPCAKINFO_ACK:
	case MCU_VCS_SENDPCAKINFO_NACK:
	case MCU_VCS_PACKINFO_NOTIFY:
	case MCU_VCS_CHGDUALSTATUS_ACK:
    case MCU_VCS_CHGDUALSTATUS_NACK:
    case MCU_VCS_STARTDOUBLESTREAM_NOTIFY:

	case MCU_VCS_START_MONITOR_UNION_ACK:
	case MCU_VCS_START_MONITOR_UNION_NACK:
	case MCU_VCS_START_MONITOR_NOTIF:
	case MCU_VCS_STOP_MONITOR_UNION_ACK:
	case MCU_VCS_STOP_MONITOR_UNION_NACK:
	case MCU_VCS_MTJOINCONF_REQ:
		VCS_EV_LOG(LOG_LVL_DETAIL);
		ProcMcuVcsMsg( pcMsg );
		break;


	default:
		CMcsSsnInst::InstanceEntry( pcMsg );
		

	}
}
/*====================================================================
    函数名      ：VCSDaemonProcPowerOn
    功能        ：处理上电后启动工作
    算法实现    ：加载VCS用户管理模块
    引用全局变量：
    输入参数说明：const CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
  2010/11/11    1.0         
====================================================================*/
void CVcsSsnInst::VCSDaemonProcPowerOn( void )
{

    //sgx VCS用户列表模块重新从文件装载用户列表
	g_cMcuVcApp.VcsReloadMcuUserList();   
	// 文件中读取所有用户分配的任务信息
    g_cMcuVcApp.LoadVCSUsersTaskInfo();

	// 初始化普通实例的状态
	for (u16 wInsID = 1; wInsID <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); wInsID++)
	{
		post( MAKEIID(GetAppID(), wInsID), OSP_POWERON );
	}

    
}

/*====================================================================
    函数名      ：ProcVCSSoftNameMsg
    功能        ：处理VCS软件名称修改消息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/11/20              付秀华        创建
====================================================================*/
void CVcsSsnInst::ProcVCSSoftNameMsg( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	switch( CurState() )
	{
	case STATE_NORMAL:
		switch(pcMsg->event)
		{
		case VCS_MCU_GETSOFTNAME_REQ:
		case VCS_MCU_CHGSOFTNAME_REQ:
			::OspPost(MAKEIID( AID_MCU_CONFIG, 1 ), cServMsg.GetEventId(), cServMsg.GetServMsg(), 
				cServMsg.GetServMsgLen());
			break;
		default:
			LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVCSSoftNameMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
				pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
			break;
		}
		break;
	default:
		break;
	}	
}


/*====================================================================
    函数名      ：ProcUserManagerMsg
    功能        ：处理用户管理消息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/11/20              付秀华        创建
====================================================================*/
void CVcsSsnInst::ProcVCSUserManagerMsg( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	CVCSUsrInfo *ptVCSUserInfo  = NULL;
    TUsrGrpInfo tUsrGrpInfo;
    
    // 当前VCS的用户所属组
    u8 byMcsGrpId = CMcsSsn::GetUserGroup( GetInsID() );

    // 操作对象所属组
    u8 byUsrGrpId;
    
    // guzh [12/14/2006] 如果是备份切换后的工作方式,没有修改权限
//     if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
//     {
//         switch (pcMsg->event)
//         {
//         case MCS_MCU_ADDUSER_REQ:
//         case MCS_MCU_DELUSER_REQ:
//         case MCS_MCU_CHANGEUSER_REQ:
//         case MCS_MCU_ADDUSERGRP_REQ:
//         case MCS_MCU_CHANGEUSERGRP_REQ:
//         case MCS_MCU_DELUSERGRP_REQ:
// 			cServMsg.SetErrorCode(ERR_MCU_NPLUS_EDITUSER);
// 			SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
// 			return;
//         default:
//             break;
//         }
//     }

	switch (pcMsg->event)
	{
	case MCS_MCU_ADDUSER_REQ:       //VCS请求MCU添加用户
        {
			// 对于用户的添加采取原MCS消息，但消息体不同，因而对消息内容进行检验，作保护
			if (cServMsg.GetMsgBodyLen() < sizeof(CVCSUsrInfo))
			{
				LogPrint(LOG_LVL_ERROR, MID_MCU_VCS,"[ProcVCSUserManagerMsg] error:MCS_MCU_ADDUSER_REQ with wrong message body!\n");
				return;
			}
			
		    ptVCSUserInfo  = (CVCSUsrInfo*)cServMsg.GetMsgBody();

		    cServMsg.SetMsgBody((u8*)ptVCSUserInfo->GetName(), MAX_CHARLENGTH);
            byUsrGrpId = ptVCSUserInfo->GetUsrGrpId();

            if ( byMcsGrpId != USRGRPID_SADMIN && 
                 byMcsGrpId != byUsrGrpId )
            {
                // 不能操作非所属组
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
			    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			    return;
            }

            //BOOL32 bRet = FALSE;
            u8 byMaxNum = 0;
            u8 byNum = 0;

            /*bRet = */
			g_cMcuVcApp.GetVCSUsrGrpUserCount(byUsrGrpId, byMaxNum, byNum);
            if ( byNum >= byMaxNum )
            {
                // 当前组用户满
			    cServMsg.SetErrorCode(UM_NOT_ENOUGHMEM);
			    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			    return;
            }

			// 添加用户信息
		    if (g_cVCSUsrManage.AddUser(ptVCSUserInfo))
		    {       
			    SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

			    cServMsg.SetMsgBody((u8*)ptVCSUserInfo, sizeof(CVCSUsrInfo));

                // 通知所在用户组
				g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn( byUsrGrpId, pcMsg->event + 3, 
											         cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		    }
		    else
		    {
			    cServMsg.SetErrorCode(g_cVCSUsrManage.GetLastError());
			    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				return;
		    }

			// 添加用户分配的任务信息
			TUserTaskInfo tUserTaskInfo;
			tUserTaskInfo.SetUserTaskInfo(ptVCSUserInfo->GetName(), ptVCSUserInfo->GetTaskNum(), ptVCSUserInfo->GetTask());
		    g_cMcuVcApp.AddVCSUserTaskInfo(tUserTaskInfo); 

        }
		break;

	case MCS_MCU_DELUSER_REQ:       //VCS请求MCU删除用户
		{
			if (cServMsg.GetMsgBodyLen() < sizeof(CVCSUsrInfo))
			{
				LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVCSUserManagerMsg] error:MCS_MCU_DELUSER_REQ with wrong message body!\n");
				return;
			}
			ptVCSUserInfo  = (CVCSUsrInfo*)cServMsg.GetMsgBody();
			byUsrGrpId = ptVCSUserInfo->GetUsrGrpId();
			
			if ( byMcsGrpId != USRGRPID_SADMIN && 
				 byMcsGrpId != byUsrGrpId )
			{
				// 不能操作非所属组
				cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
				SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				return;
			}

			if (0 == strncmp(m_achUser, ptVCSUserInfo->GetName(), (MAXLEN_PWD-1)))
			{
				// 不能删除自己
				cServMsg.SetErrorCode(UM_ACTOR_ONLY);
				SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				return;
			}

			cServMsg.SetMsgBody((u8*)ptVCSUserInfo->GetName(), MAX_CHARLENGTH);
			if (g_cVCSUsrManage.DelUser(ptVCSUserInfo))
			{
				SendMsgToMcs( pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

				// 通知所在用户组
				g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn( byUsrGrpId, pcMsg->event + 3, 
													  cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			}
			else
			{
				cServMsg.SetErrorCode( g_cVCSUsrManage.GetLastError() );
				SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				return;
			}

			// 删除用户对应的任务信息
			g_cMcuVcApp.DelVCSUserTaskInfo(ptVCSUserInfo->GetName());
		}

		break;

	case MCS_MCU_CHANGEUSER_REQ:  //VCS请求MCU修改用户
		{
			if (cServMsg.GetMsgBodyLen() < sizeof(CVCSUsrInfo))
			{
				LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVCSUserManagerMsg] error:MCS_MCU_CHANGEUSER_REQ with wrong message body!\n");
				return;
			}

			ptVCSUserInfo  = (CVCSUsrInfo*)cServMsg.GetMsgBody();
			byUsrGrpId = ptVCSUserInfo->GetUsrGrpId();

			cServMsg.SetMsgBody((u8*)ptVCSUserInfo->GetName(), MAX_CHARLENGTH);

			if ( byMcsGrpId != USRGRPID_SADMIN && 
				 byMcsGrpId != byUsrGrpId )
			{
				// 不能操作非所属组
				cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
				SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());		
				return;
			}
        
			if (g_cVCSUsrManage.ModifyInfo(ptVCSUserInfo))
			{
				SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

				cServMsg.SetMsgBody((u8*)ptVCSUserInfo, sizeof(CVCSUsrInfo));
				g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn( byUsrGrpId, pcMsg->event + 3, 
													  cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			}
			else
			{
				cServMsg.SetErrorCode( g_cVCSUsrManage.GetLastError() );
				SendMsgToMcs( pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			}

			// 修改用户分配的任务信息
			TUserTaskInfo tUserTaskInfo;
			tUserTaskInfo.SetUserTaskInfo(ptVCSUserInfo->GetName(), ptVCSUserInfo->GetTaskNum(), ptVCSUserInfo->GetTask());
			g_cMcuVcApp.ChgVCSUserTaskInfo(tUserTaskInfo); 
		}
			
			break;
		
	case MCS_MCU_GETUSERLIST_REQ:   //VCS请求MCU得到当前用户组所有的用户列表
		{
			CLongMessage cLongMsg;
			u8 abyExUsrBuf[sizeof(CExUsrInfo) * USERNUM_PERPKT];
			u8 abyVCSUserBuf[sizeof(CVCSUsrInfo) * USERNUM_PERPKT];

			u8 byUsrItr = 0; // 查找用户的指针
			u8 byUserNumInPack = 0;

            BOOL32 bRet = FALSE;
            u8 byUsrMaxNum = 0;
            u8 byUsrNum = 0;

            bRet = g_cMcuVcApp.GetVCSUsrGrpUserCount(byMcsGrpId, byUsrMaxNum, byUsrNum);
            
            // 计算需要发的总包数
            cLongMsg.m_uTotal = byUsrNum / USERNUM_PERPKT;
            if ( byUsrNum % USERNUM_PERPKT != 0 )
            {
                cLongMsg.m_uTotal++;
            }
            cLongMsg.m_uCurr = 1;

			if ( !bRet || byUsrNum == 0)
			{
                if (!bRet)
                {
                    // 获取失败,Nack
                    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());  // Nack
                }
                else
                {
                    // 数量为0,Ack
                    SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()); // Ack
                }
				
				LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVCSUserManagerMsg] No users in current group\n");
			}
			else
			{
				SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()); // Ack

                // Notify
				while ( cLongMsg.m_uCurr <= cLongMsg.m_uTotal ) 
				{
					byUserNumInPack = 0;
					if ( !g_cMcuVcApp.GetVCSCurUserList(byMcsGrpId, abyExUsrBuf, byUsrItr, byUserNumInPack) )
					{
						LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVCSUserManagerMsg] GetVCSCurUserList Failed at index: %d\n", byUsrItr);
                        return;
					}
					cLongMsg.m_uNumUsr = byUserNumInPack;

					PackUserTaskInfo(byUserNumInPack, abyExUsrBuf, abyVCSUserBuf);

					cServMsg.SetMsgBody((u8*)&cLongMsg, sizeof(cLongMsg));
					cServMsg.CatMsgBody(abyVCSUserBuf, byUserNumInPack * sizeof(CVCSUsrInfo));

					SendMsgToMcs( pcMsg->event + 3, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

                    cLongMsg.m_uCurr ++;
				}
			}
		}
		break;

	// 用户组消息保持不变，仅保存到不同变量
    case MCS_MCU_ADDUSERGRP_REQ:    //VCS添加用户组
        // 只有超级管理员有权限
        {
            tUsrGrpInfo = *(TUsrGrpInfo*)cServMsg.GetMsgBody();

            if ( byMcsGrpId != USRGRPID_SADMIN  )
            {
                // 不能操作
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
                cServMsg.SetMsgBody( (u8*)tUsrGrpInfo.GetUsrGrpName(), MAX_CHARLENGTH );
			    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                return;
            }
            else
            {
                u16 wErrorCode = 0;
                byUsrGrpId = g_cMcuVcApp.AddVCSUserGroup(tUsrGrpInfo, wErrorCode);
                if ( byUsrGrpId == USRGRPID_INVALID )
                {
			        cServMsg.SetErrorCode( wErrorCode );
                    cServMsg.SetMsgBody( (u8*)tUsrGrpInfo.GetUsrGrpName(), MAX_CHARLENGTH );
			        SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                }
                else
                {
                    tUsrGrpInfo.SetUsrGrpId(byUsrGrpId);
                    cServMsg.SetMsgBody( (u8*)&tUsrGrpInfo, sizeof(TUsrGrpInfo) );

                    SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	

                    cServMsg.SetEventId( MCU_MCS_USERGRP_NOTIF );
			        g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn( byUsrGrpId, MCU_MCS_USERGRP_NOTIF, 
											              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                }
            }
        }
        break;
        
    case MCS_MCU_CHANGEUSERGRP_REQ:    //VCS修改用户组
        // 只要超级管理员有权限
        {
            tUsrGrpInfo = *(TUsrGrpInfo*)cServMsg.GetMsgBody();
            u8 byTargetId = tUsrGrpInfo.GetUsrGrpId();
            if ( byMcsGrpId != USRGRPID_SADMIN  )
            {
                // 不能操作
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);                
                cServMsg.SetMsgBody( (u8*)&byTargetId, sizeof(u8) );
			    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                return;
            }
            else
            {
                u16 wErrorCode = 0;
                if ( !g_cMcuVcApp.ChgVCSUserGroup(tUsrGrpInfo, wErrorCode) )
                {
			        cServMsg.SetErrorCode( wErrorCode );
                    cServMsg.SetMsgBody( (u8*)&byTargetId, sizeof(u8) );
			        SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                }
                else
                {
                    SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	

                    cServMsg.SetEventId( MCU_MCS_USERGRP_NOTIF );

			        g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn( byMcsGrpId, MCU_MCS_USERGRP_NOTIF, 
											              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                }
            }
        }
        break;

    case MCS_MCU_DELUSERGRP_REQ:    //VCS删除用户组及相关资源
        // 只要超级管理员有权限
        {
            u8 byDelGrpId = *cServMsg.GetMsgBody();
            if ( byMcsGrpId != USRGRPID_SADMIN )
            {
                // 不能操作
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
			    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());		
                return;
            }
            else
            {
                u16 wErrorCode = 0;
                if ( !g_cMcuVcApp.DelVCSUserGroup(byDelGrpId, wErrorCode) )
                {
			        cServMsg.SetErrorCode( wErrorCode );
			        SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                }
                else
                {
                    SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	

                    // Nofity
                    cServMsg.SetEventId( MCU_MCS_DELUSERGRP_NOTIF );
                    
			        g_cVcsSsnApp.Broadcast2SpecGrpVcsSsn( byMcsGrpId, MCU_MCS_DELUSERGRP_NOTIF, 
											              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                }
            }
        }
        break;
        
    case MCS_MCU_GETUSERGRP_REQ:    //VCS请求用户组信息
        // 只要超级管理员有权限
        {
            if ( byMcsGrpId != USRGRPID_SADMIN )
            {
                // 不能操作
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
                cServMsg.SetMsgBody();
			    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());		
            }
            else
            {
                u8 byNum = 0;
                TUsrGrpInfo *ptInfo = NULL;
                g_cMcuVcApp.GetVCSUserGroupInfo( byNum, &ptInfo );
                
                cServMsg.SetMsgBody( &byNum, sizeof(u8) );
                cServMsg.CatMsgBody( (u8*)ptInfo, sizeof(TUsrGrpInfo) * byNum );

                SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
            }
        }
        break;

    default:
        break;
	}

	return;
}
/*====================================================================
    函数名      ：ProcVCSMcuUnProcCfgMsg
    功能        ：处理VCS关于MCU无需处理信息的设置及读取
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/11/20              付秀华        创建
====================================================================*/
void CVcsSsnInst::ProcVCSMcuUnProcCfgMsg( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8* pbyMsgBody = cServMsg.GetMsgBody();

	s8  achFullName[64];
	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUUNPROCCFGFILENAME);

	switch(pcMsg->event)
	{
	case VCS_MCU_SETUNPROCFGINFO_REQ:
		{
			u32 dwCfgLen = cServMsg.GetMsgBodyLen();
			dwCfgLen = (dwCfgLen > MAXLEN_MCUUNPROCCFG_INI ? MAXLEN_MCUUNPROCCFG_INI : dwCfgLen);

			FILE* hFile = NULL;
			if ((hFile = fopen(achFullName, "wb")) != NULL)
			{
			    SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				fwrite(pbyMsgBody, sizeof(u8), dwCfgLen, hFile);
				fclose(hFile);
			}
			else
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcVCSMcuUnProcCfgMsg] fail to open mcuunproccfg.ini\n");
				SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				return;
			}
			CMcsSsn::BroadcastToAllMcsSsn(pcMsg->event + 3, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

		}

		break;
	case VCS_MCU_GETUNPROCFGINFO_REQ:
		{
			FILE* hFile = NULL;
			if ((hFile = fopen(achFullName, "rb")) != NULL)
			{
				u8 achCfgInfo[MAXLEN_MCUUNPROCCFG_INI];
				memset(achCfgInfo, 0, sizeof(achCfgInfo));						
				u32 dwCfgLen = fread(achCfgInfo, sizeof(u8), sizeof(achCfgInfo), hFile);
				fclose(hFile);
				cServMsg.SetMsgBody(achCfgInfo, (u16)dwCfgLen);
				SendMsgToMcs(pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			}
			else
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcVCSMcuUnProcCfgMsg] fail to open mcuunproccfg.ini\n");
				SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				return;
			}

		}
		break;
	default:
	    break;
	}


}
/*====================================================================
    函数名      ：PackUserTaskInfo
    功能        ：将用户任务信息打包入用户信息
    算法实现    ：
    引用全局变量：
    输入参数说明：[IN]byUserNumInPack：打包的用户数
	              [IN]pbyExUserBuf：不包含用户任务信息的数据区指针
	              [OUT]pbyVCSUserBuf：包含用户任务信息的数据区指针
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/11/20              付秀华        创建

====================================================================*/
void CVcsSsnInst::PackUserTaskInfo(u8 byUserNumInPack, u8 *pbyExUserBuf, u8 *pbyVCSUserBuf)
{
	TUserTaskInfo tUserTaskInfo;
	CVCSUsrInfo   cVCSUserInfo;
	CExUsrInfo*   pcVCSUserInfo;
	for (u16 wIndex = 0; wIndex < byUserNumInPack; wIndex++)
	{
		cVCSUserInfo.SetNull();
		tUserTaskInfo.SetNull();

		pcVCSUserInfo = (CExUsrInfo*)&cVCSUserInfo;
		memcpy(pcVCSUserInfo, pbyExUserBuf, sizeof(CExUsrInfo));		
		if (g_cMcuVcApp.GetSpecVCSUserTaskInfo(((CExUsrInfo*)pbyExUserBuf)->GetName(), tUserTaskInfo))
		{
			cVCSUserInfo.AssignTask(tUserTaskInfo.GetUserTaskNum(), tUserTaskInfo.GetUserTaskInfo());
		}
		memcpy(pbyVCSUserBuf, &cVCSUserInfo, sizeof(cVCSUserInfo));

		pbyExUserBuf  += sizeof(CExUsrInfo);
		pbyVCSUserBuf += sizeof(CVCSUsrInfo); 
	}

}
/*====================================================================
    函数名      ：ProcVCSConfTemMsg
    功能        ：处理VCS的会议模板消息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/12/18              付秀华        创建
====================================================================*/
void CVcsSsnInst::ProcVCSConfTemMsg( CMessage * const pcMsg )
{
	CServMsg cServMsg(pcMsg->content,pcMsg->length);

	switch( CurState() )
	{
	case STATE_NORMAL:
		switch(pcMsg->event)
		{
		case MCS_MCU_MODIFYTEMPLATE_REQ:
		case MCS_MCU_DELTEMPLATE_REQ:
			{
				// VCS对于正在开会的调度席不允许修改/删除
				CConfId cConfId = cServMsg.GetConfId();
			    u8 byConfIdx = g_cMcuVcApp.GetConfIdx(cServMsg.GetConfId());

                TTemplateInfo tOldTemInfo;
                if(!g_cMcuVcApp.GetTemplate(byConfIdx, tOldTemInfo))
                {
                    cServMsg.SetErrorCode(ERR_MCU_TEMPLATE_NOTEXIST);
                    LogPrint(LOG_LVL_WARNING, MID_MCU_VCS,  "[ProcVCSConfTemMsg] get template confIdx<%d> failed\n", byConfIdx);
                    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                    return;
                }

				// 搜索当前会议是否存在由该待修改模板所创建起的会议
				if(g_cMcuVcApp.IsConfE164Repeat(tOldTemInfo.m_tConfInfo.GetConfE164(), FALSE ,TRUE))
				{
					cServMsg.SetErrorCode(ERR_MCU_TEMPLATE_INVC);
					LogPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcVCSConfTemMsg] template is in vc\n");
					SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
					return;
				}

				if (MCS_MCU_DELTEMPLATE_REQ == pcMsg->event)
				{
					// VCS调度席删除需对应删除给用户配置的该调度席任务
					cConfId = cServMsg.GetConfId();
					g_cMcuVcApp.DelVCSSpecTaskInfo(cConfId);
				}
			}
			break;
		default:
		    break;
		}
		// 特殊处理完成交给基类处理
		CMcsSsnInst::InstanceEntry( pcMsg );		
		break;

	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVCSConfTemMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}	

}

/*====================================================================
    函数名      ：ProcVCSMcuVCDaemonMsg
    功能        ：处理VCS直接分发给VC DAEMON的消息(含创会请求，组呼配置信息更新请求)
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/12/04              付秀华        创建

====================================================================*/
void CVcsSsnInst::ProcVCSMcuVCDaemonMsg( const CMessage * pcMsg )
{
	LogPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVCSMcuPackinfoMsg]: Console.%u receive command message %u(%s)!\n", 
            GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));

	CServMsg	cServMsg(pcMsg->content,pcMsg->length);
	switch( CurState() )
	{
	case STATE_NORMAL:
	    cServMsg.SetMcuId( LOCAL_MCUID );
		cServMsg.SetSrcSsnId( ( u8 )GetInsID() );
		cServMsg.SetSrcMtId( 0 );
		cServMsg.SetEventId( pcMsg->event );
		g_cMcuVcApp.SendMsgToDaemonConf( cServMsg.GetEventId(), cServMsg.GetServMsg(), 
			                             cServMsg.GetServMsgLen() );
		break;

	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVCSMcuCreateConfMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
}
/*====================================================================
    函数名      ：ProcVCSMcuVCMsg
    功能        ：处理VCS的调度请求
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/12/08              付秀华        创建

====================================================================*/
void CVcsSsnInst::ProcVCSMcuVCMsg( const CMessage * pcMsg )
{
	LogPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[CVcsSsnInst%d][ProcVCSMcuVCMsg]: Console%d receive command message %d(%s)!\n",
			GetInsID(), GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));  

    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
    switch (CurState())
    {
    case STATE_NORMAL:
        cServMsg.SetMcuId(LOCAL_MCUID);
        cServMsg.SetSrcSsnId((u8)GetInsID());
        cServMsg.SetSrcMtId(0);
        cServMsg.SetEventId(pcMsg->event);
        
        g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(), pcMsg->event,
                                  cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		
        break;

    default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcMcsMcuMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        break;
    }
}

/*====================================================================
    函数名      ：ProcMcuVcsMsg
    功能        ：所有VCS命令应答消息统一处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/12/08              付秀华        创建
====================================================================*/
void CVcsSsnInst::ProcMcuVcsMsg( const CMessage * pcMsg )
{
	LogPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[CVcsSsnInst%d][ProcMcuVcsMsg]: Command reply Message %u(%s) to Console%d received!\n",
			GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event), GetInsID() );


	switch( CurState() )
	{
	case STATE_NORMAL:
        SendMsgToMcs( pcMsg->event, pcMsg->content, pcMsg->length );
		break;

	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcMcuVcsMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
}
/*====================================================================
    函数名      ：ProcVCSConfOprMsg
    功能        ：处理VCS开始调度席操作请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/12/12              付秀华        创建
====================================================================*/
void CVcsSsnInst::ProcVCSConfOprMsg( const CMessage * pcMsg )
{
	LogPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcVCSConfOprMsg]: Console.%u receive command message %u(%s)!\n", 
            GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u8 byMcsId = cServMsg.GetSrcSsnId();
	CConfId cConfId = cServMsg.GetConfId();

	switch( CurState() )
	{
	case STATE_NORMAL:
        switch( pcMsg->event)
        {
        case VCS_MCU_OPRCONF_REQ:
			{
				// 会议若被上级调度席开启，下级调度席不允许其它VCS操作
// 				CMcuVcInst* pVCInst = NULL;
// 				for (u8 byConfIdx= MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
// 				{
// 					pVCInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
// 					if (pVCInst != NULL && pVCInst->m_tConf.GetConfId() == cConfId &&
// 						CONF_CREATE_MT == pVCInst->m_byCreateBy)
// 					{
// 						cServMsg.SetErrorCode(ERR_MCU_VCS_MMCUINVC);
// 						SendMsgToMcs(cServMsg.GetEventId() + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
// 						return;						
// 					}
// 
// 				}
				if(g_cMcuVcApp.IsYourTask(m_achUser, cConfId))
				{
					if( g_cVcsSsnApp.IsConfWaitRelease( cConfId ) )
					{
						u8 byTimerIdx = 0xff;
						if( g_cVcsSsnApp.GetTimerIdxByConfId( cConfId, byTimerIdx) )
						{
							KillTimer(  MCUVC_RELEASECONF_TIMER + byTimerIdx);
							g_cVcsSsnApp.CleanConfInWaitRelease( cConfId );
						}
					}

					SendMsgToMcs(cServMsg.GetEventId() + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
					g_cVcsSsnApp.SetCurConfID(GetInsID(), cConfId);
				}
				else
				{
					cServMsg.SetErrorCode(ERR_MCU_VCS_NOTALLOWOPR);
					SendMsgToMcs(cServMsg.GetEventId() + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
					return;
				}
			}
        	break;
        case VCS_MCU_QUITCONF_REQ:
			{
				if (g_cVcsSsnApp.GetCurConfID(GetInsID()) == cConfId)
				{
					SendMsgToMcs(cServMsg.GetEventId() + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
					/*//[2011/11/24/zhangli]监控信息不能清空，有多个vcs连接mcu时，其中一个vcs进行单通道监控联动业务，然后此
					vcs退出，此监控联动业务的终端无法停止，导致此终端占用的资源（bas、带宽等）不释放。原因：退出vcs，先接到
					停监控消息MCS_MCU_STOPSWITCHMC_REQ，然后接到vcs退出信息，然后接到停监控联动消息MCS_MCU_STOPSWITCHMC_REQ，
					此时监控联动通道的源被清除，导致源无法释放*/
                    //g_cMcuVcApp.ClearMcSrc(byMcsId);
					StopToMcByMcIndex(byMcsId);
					QuitCurVCConf( TRUE );
				}
				else
				{
					LogPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcVCSConfOprMsg] VCS_MCU_QUITCONF_REQ to quit conf which is not current operated conf\n");
					SendMsgToMcs(cServMsg.GetEventId() + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
					return;
				}
			}

            break;
        default:
            break;
        }
		break;

	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[ProcVCSConfOprMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
}
/*====================================================================
    函数名      ：QuitCurVCConf
    功能        ：退出当前调度席
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
	              BOOL32 bNormalQuit, 是否是正常退出
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/12/12              付秀华        创建
====================================================================*/
void CVcsSsnInst::QuitCurVCConf( BOOL32 bNormalQuit )
{
	CConfId cConfID = g_cVcsSsnApp.GetCurConfID(GetInsID());
	if (cConfID.IsNull())
	{
		return;
	}
	
	CConfId cNull;
	g_cVcsSsnApp.SetCurConfID(GetInsID(), cNull);
	// 没有任何调度员操作该会议，则自动结束该会议
	if (!g_cVcsSsnApp.ChkVCConfStatus(cConfID))
	{
		if( bNormalQuit )
		{
			g_cVcsSsnApp.VCSRlsConf(cConfID);
		}
		else
		{
			if( !g_cVcsSsnApp.IsConfWaitRelease( cConfID ) )
			{
				u8 byWaitTimerIdx = 0xff;
				if( g_cVcsSsnApp.SetWaitReleaseConf( cConfID, byWaitTimerIdx ) )
				{
					// 投递消息给daemonInstan，让其设置定时器
					post( MAKEIID(GetAppID(), DAEMON), MCUVC_STARTRELEASECONFTIMER_CMD, &byWaitTimerIdx, sizeof(u8) );
				}
				else
				{
					// 设置该会议延迟结束失败，则正常结会
					g_cVcsSsnApp.VCSRlsConf(cConfID);
					LogPrint( LOG_LVL_ERROR, MID_MCU_VCS, "SetWaitReleaseConf Failed !\n" );
				}
			}
		}
	}	
}

/*====================================================================
函数名      ：StopToMcByMcIndex
功能        ：通过vcs序号停监控
算法实现    ：
引用全局变量：
输入参数说明：u16 wMcIndex vcs序号	              
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/11/25  4.6         zhangli        创建
====================================================================*/
void CVcsSsnInst::StopToMcByMcIndex(u8 byMcIndex)
{
	CConfId cConfID = g_cVcsSsnApp.GetCurConfID(GetInsID());
	if (cConfID.IsNull())
	{
		return;
	}
	
	CServMsg cServMsg;
    cServMsg.SetMcuId(LOCAL_MCUID);
    cServMsg.SetSrcSsnId(byMcIndex);
    cServMsg.SetSrcMtId(0);
	cServMsg.SetConfId(cConfID);
    cServMsg.SetEventId(MCS_MCU_STOPSWITCHMC_REQ);
	
	TSwitchInfo tSwtichInfo;
	
	TMt tVidSrc;
	TMt tAudSrc;
	
	for(u8 byChannel = 0; byChannel < MAXNUM_MC_CHANNL; ++byChannel)
	{
		tVidSrc.SetNull();
		tAudSrc.SetNull();
		if(!(g_cMcuVcApp.GetMcSrc(byMcIndex, &tVidSrc, byChannel, MODE_VIDEO) && !tVidSrc.IsNull())
			&& !(g_cMcuVcApp.GetMcSrc(byMcIndex, &tAudSrc, byChannel, MODE_AUDIO) && !tAudSrc.IsNull()))
		{
			continue;
		}
		
		tSwtichInfo.SetDstChlIdx(byChannel);
		tSwtichInfo.SetMode(MODE_BOTH);
		
		cServMsg.SetMsgBody((u8 *)(&tSwtichInfo), sizeof(TSwitchInfo));
		g_cMcuVcApp.SendMsgToConf(cConfID, MCS_MCU_STOPSWITCHMC_REQ,
			cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
	}
}

/*====================================================================
    函数名      ：DaemonProcWaitReleaseTimeOut
    功能        ：在Daemon中处理延迟结会的timer
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    20110518                彭杰          创建
====================================================================*/
void CVcsSsnInst::DaemonProcWaitReleaseTimeOut( const CMessage * const pcMsg )
{
	KillTimer( pcMsg->event );

	CConfId cConfId;
	cConfId.SetNull();
	if( g_cVcsSsnApp.GetConfIdByTimerIdx( pcMsg->event - MCUVC_RELEASECONF_TIMER, cConfId ) )
	{
		if( !cConfId.IsNull() )
		{
			g_cVcsSsnApp.CleanConfInWaitRelease( cConfId );
			g_cVcsSsnApp.VCSRlsConf(cConfId);
		}
	}
}



/*====================================================================
                         CVcsSsn 
====================================================================*/
CVcsSsn::CVcsSsn( void )
{
}

CVcsSsn::~CVcsSsn( void )
{
}
/*====================================================================
    函数名      ：SetCurConfID
    功能        ：设置指定实例操作的会议ID号
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/12/15              付秀华        创建
====================================================================*/
void CVcsSsn::SetCurConfID( u16 byInsId, CConfId& cConfID )
{
	if (byInsId < 1)
	{
		return;
	}
	m_acCurConfID[byInsId - 1] = cConfID;
}

/*====================================================================
    函数名      ：GetCurConfID
    功能        ：获取指定实例操作的会议ID号
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/12/15              付秀华        创建
====================================================================*/
CConfId CVcsSsn::GetCurConfID( u16 byInsId )
{
	return m_acCurConfID[byInsId - 1];
}

/*====================================================================
    函数名      ：Broadcast2SpecGrpVcsSsn
    功能        ：发消息给同用户组的VCS对应的会话实例
    算法实现    ：
    引用全局变量：
    输入参数说明：byGrdId, 组号
				  u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/21                付秀华        创建
====================================================================*/
void CVcsSsn::Broadcast2SpecGrpVcsSsn( u8 byGrdId, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//本端主用则允许向外部各App投递消息，否则丢弃
	if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
	{
		return;
	}

    u8 bySsnGrpId = 0;
	for (u8 byInstID = MAXNUM_MCU_MC + 1; byInstID <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byInstID++)
	{
        bySsnGrpId = CMcsSsn::GetUserGroup( byInstID );
        if ( bySsnGrpId != USRGRPID_INVALID )
        {
            if ( bySsnGrpId == byGrdId ||
                 bySsnGrpId == USRGRPID_SADMIN )
            {
				// 如果是特定组或者超级用户组
				::OspPost(MAKEIID( AID_MCU_VCSSN, byInstID ), wEvent, pbyMsg, wLen);
                LogPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[Broadcast2SpecGrpVcsSsn]: Message %u(%s) In InsID.%u is send for Spec Group Id(%d) <--> Ssn Group Id(%d)\n",
                        wEvent, ::OspEventDesc( wEvent ), byInstID, 
                        byGrdId, bySsnGrpId);
            }
            else
            {
            
                LogPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[Broadcast2SpecGrpVcsSsn]: Message %u(%s) In InsID.%u is Dropped for Spec Group Id(%d) <--> Ssn Group Id(%d)\n",
                        wEvent, ::OspEventDesc( wEvent ), byInstID, 
                        byGrdId, bySsnGrpId);
            }
        }
	}

	return;
}

/*====================================================================
    函数名      ：GetVCSSSsnData
    功能        ：获取需要同步的VCSSSn信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/21                付秀华        创建
====================================================================*/
BOOL32 CVcsSsn::GetVCSSSsnData( u8 *pbyBuf, u32 dwInBufLen, u32 &dwOutBufLen)
{
	if (NULL == pbyBuf)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[GetVCSSSsnData] NULL == pbyBuf\n");
		return FALSE;
	}

	dwOutBufLen = sizeof(m_acCurConfID) + sizeof(m_acWaitReleaseConfId);
	if (dwInBufLen < dwOutBufLen)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[GetVCSSSsnData] dwInBufLen(%d) < dwOutBufLen(%d)\n", 
			      dwInBufLen, dwOutBufLen);
		return FALSE;
	}

	memcpy( pbyBuf, m_acCurConfID, sizeof(m_acCurConfID) );
	memcpy( (pbyBuf + sizeof(m_acCurConfID)), m_acWaitReleaseConfId, sizeof(m_acWaitReleaseConfId) );
	return TRUE;
}

/*====================================================================
    函数名      ：SetVCSSSsnData
    功能        ：设置需要同步的VCSSSn信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	08/11/21                付秀华        创建
====================================================================*/
BOOL32 CVcsSsn::SetVCSSSsnData( u8 *pbyBuf, u32 dwBufLen )
{
	if (NULL == pbyBuf)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[SetVCSSSsnData] NULL == pbyBuf\n");
		return FALSE;
	}

	if ( dwBufLen != ( sizeof(m_acCurConfID) + sizeof(m_acWaitReleaseConfId) ) )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VCS, "[SetVCSSSsnData] input lenth(%d) is not correct\n", dwBufLen);
		return FALSE;
	}

	memcpy(m_acCurConfID, pbyBuf, sizeof(m_acCurConfID) );
	memcpy(m_acWaitReleaseConfId, (pbyBuf + sizeof(m_acCurConfID)), sizeof(m_acWaitReleaseConfId) );
	return TRUE;
}

/*====================================================================
    函数名      ：VCSRlsConf
    功能        ：VCS释放指定的会议
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/12/15              付秀华        创建
====================================================================*/
void CVcsSsn::VCSRlsConf(CConfId cConfId)
{
	CServMsg cServMsg;
    cServMsg.SetMcuId(LOCAL_MCUID);
    cServMsg.SetSrcSsnId(0);       // 不需要回应ACK消息给控制台
    cServMsg.SetSrcMtId(0);
	cServMsg.SetConfId(cConfId);
    cServMsg.SetEventId(MCS_MCU_RELEASECONF_REQ);
    
    g_cMcuVcApp.SendMsgToConf(cConfId, MCS_MCU_RELEASECONF_REQ,
                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
}
/*====================================================================
    函数名      ：ChkVCConfStatus
    功能        ：检查指定的会议是否仍处于被控制状态
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/12/15              付秀华        创建
====================================================================*/
BOOL CVcsSsn::ChkVCConfStatus( CConfId cConfId )
{
	for (u8 byInstId = MAXNUM_MCU_MC + 1; byInstId <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byInstId++)
	{
		if (cConfId == m_acCurConfID[byInstId - 1])
		{
			return TRUE;
		}
		
	}

	return FALSE;
}

/*====================================================================
    函数名      ：SetWaitReleaseConf
    功能        ：设置需要等待延迟结束的会议
    算法实现    ：
    引用全局变量：IN : const CConfId &cConfId 需要保存的会议Id
				  OUT: u8 byTimerIdx 保存该CConfId所用的索引，该索引对应相应的定时器
    输入参数说明：
    返回值说明  ：设置成功 TRUE, 设置失败 FLASE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    20110518                彭杰          创建
====================================================================*/
BOOL32 CVcsSsn::SetWaitReleaseConf( const CConfId &cConfId, u8 &byTimerIdx )
{
	if( cConfId.IsNull() )
	{
		return FALSE;
	}

	for( u8 byLoop = 0; byLoop < MAXNUM_MCU_VC; byLoop++ )
	{
		if( m_acWaitReleaseConfId[byLoop].IsNull() )
		{
			m_acWaitReleaseConfId[byLoop] = cConfId;
			byTimerIdx = byLoop;
			return TRUE;
		}
	}

	return FALSE;
}

/*====================================================================
    函数名      ：判断某会议是否在等待延迟结束
    功能        ：IsConfWaitRelease
    算法实现    ：
    引用全局变量：const CConfId &cConfId  会议Id
    输入参数说明：
    返回值说明  ：TRUE, FLASE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    20110518                彭杰          创建
====================================================================*/
BOOL32 CVcsSsn::IsConfWaitRelease( const CConfId &cConfId )
{
	if( cConfId.IsNull() )
	{
		return FALSE;
	}

	for( u8 byLoop = 0; byLoop < MAXNUM_MCU_VC; byLoop++ )
	{
		if( cConfId == m_acWaitReleaseConfId[byLoop] )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

/*====================================================================
    函数名      ：将某会议从等待延迟结束中清除
    功能        ：CleanConfInWaitRelease
    算法实现    ：
    引用全局变量：const CConfId &cConfId  会议Id
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    20110518                彭杰          创建
====================================================================*/
void CVcsSsn::CleanConfInWaitRelease( const CConfId &cConfId )
{
	if( cConfId.IsNull() )
	{
		return;
	}

	for( u8 byLoop = 0; byLoop < MAXNUM_MCU_VC; byLoop++ )
	{
		if( cConfId == m_acWaitReleaseConfId[byLoop] )
		{
			m_acWaitReleaseConfId[byLoop].SetNull();
		}
	}
}

/*====================================================================
    函数名      ：通过会议Id得到某个等待延迟结会的会议占用的TimerIdx
    功能        ：GetTimerIdxByConfId
    算法实现    ：
    引用全局变量：IN : const CConfId &cConfId  会议Id
	              OUT: u8 byTimerIdx 保存该CConfId所用的索引，该索引对应相应的定时器
    输入参数说明：
    返回值说明  ：TRUE, FLASE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    20110518                彭杰          创建
====================================================================*/
BOOL32 CVcsSsn::GetTimerIdxByConfId( const CConfId &cConfId, u8 &byTimerIdx )
{
	if( cConfId.IsNull() )
	{
		return FALSE;
	}

	for( u8 byLoop = 0; byLoop < MAXNUM_MCU_VC; byLoop++ )
	{
		if( cConfId == m_acWaitReleaseConfId[byLoop] )
		{
			byTimerIdx = byLoop;
			return TRUE;
		}
	}
	return FALSE;
}

/*====================================================================
    函数名      ：通过TimerIdx得到某个等待延迟结会的会议Id
    功能        ：GetConfIdByTimerIdx
    算法实现    ：
    引用全局变量：IN : u8 byTimerIdx 保存该CConfId所用的索引，该索引对应相应的定时器
	              OUT: const CConfId &cConfId  会议Id
    输入参数说明：
    返回值说明  ：TRUE, FLASE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    20110518                彭杰          创建
====================================================================*/
BOOL32 CVcsSsn::GetConfIdByTimerIdx( const u8 byTimerIdx, CConfId &cConfId)
{
	if( byTimerIdx < MAXNUM_MCU_VC )
	{
		cConfId = m_acWaitReleaseConfId[byTimerIdx];
		return TRUE;
	}
	return FALSE;
}


//END OF FILE















































