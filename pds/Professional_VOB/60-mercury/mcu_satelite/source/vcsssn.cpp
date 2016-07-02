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
#include "mcuerrcode.h"
#include "umconst.h"
#include "nplusmanager.h"
#include "mcucfg.h"
#include "vcsssn.h"

#if defined(_VXWORKS_)
#include "brddrvlib.h"
#elif defined(_LINUX_)
#include "boardwrapper.h"
#endif

CVcsSsnApp	g_cVcsSsnApp;	                   // VCS会话应用实例

CUsrManage  g_cVCSUsrManage((u8*)DIR_DATA, (u8*)MCU_VCSUSER_FILENAME);   //VCS的用户管理对象


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
void CVcsSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	// 对于设置为无效状态的实例不进行任何消息处理
	if (CurState() == STATE_INVALID)
	{
		return;
	}

	if (NULL == pcMsg)
	{
		OspPrintf(TRUE, FALSE, "[CVcsSsnInst%u]: The received msg's pointer in the msg entry is NULL!", GetInsID());
		return;
	}

	if (MCU_MCS_MTSTATUS_NOTIF != pcMsg->event)
	{
		VcsLog("[CVcsSsnInst%u]: Event %u(%s) received!\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));
	}

	switch (pcMsg->event)
	{
	// 初始化VCS普通实例的状态
	case OSP_POWERON:
		{
			// 对于VCS创建32个实例，只有后16个实例有效，支持16个VCS同时连接
			if (GetInsID() <= MAXNUM_MCU_MC)
			{
				NEXTSTATE(STATE_INVALID) ;
			}
		}
    	break;

	case OSP_DISCONNECT:				       //控制台断链通知		
		QuitCurVCConf();
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

        ProcVCSUserManagerMsg( pcMsg );
        break;

	// VCS软件名称自定义
	case VCS_MCU_GETSOFTNAME_REQ:
	case VCS_MCU_CHGSOFTNAME_REQ:
		ProcVCSSoftNameMsg( pcMsg );
		break;
		
	case VCS_MCU_SETUNPROCFGINFO_REQ:
	case VCS_MCU_GETUNPROCFGINFO_REQ:
		ProcVCSMcuUnProcCfgMsg( pcMsg );
		break;

	// 模板相关操作
	case MCS_MCU_MODIFYTEMPLATE_REQ:        //VCS修改模板
	case MCS_MCU_DELTEMPLATE_REQ:           //VCS删除模板
		ProcVCSConfTemMsg( pcMsg );
		break;              

	case VCS_MCU_OPRCONF_REQ:           //VCS请求操作某调度席
	case VCS_MCU_QUITCONF_REQ:          //VCS请求退出某调度席
		ProcVCSConfOprMsg( pcMsg );
		break;

	case VCS_MCU_CREATECONF_BYTEMPLATE_REQ: //VCS通过会议模板创建会议
		ProcVCSMcuCreateConfMsg( pcMsg );
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
		ProcMcuVcsMsg( pcMsg );
		break;


	default:
		CMcsSsnInst::InstanceEntry( pcMsg );
		

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
		OspPrintf(TRUE, FALSE, "[ProcVCSSoftNameMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
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
				OspPrintf(TRUE, FALSE,"[ProcVCSUserManagerMsg] error:MCS_MCU_ADDUSER_REQ with wrong message body!\n");
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

            BOOL32 bRet = FALSE;
            u8 byMaxNum = 0;
            u8 byNum = 0;

            bRet = g_cMcuVcApp.GetVCSUsrGrpUserCount(byUsrGrpId, byMaxNum, byNum);
            if (byNum >= byMaxNum)
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
				OspPrintf(TRUE, FALSE, "[ProcVCSUserManagerMsg] error:MCS_MCU_DELUSER_REQ with wrong message body!\n");
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
				OspPrintf(TRUE, FALSE, "[ProcVCSUserManagerMsg] error:MCS_MCU_CHANGEUSER_REQ with wrong message body!\n");
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
				
				OspPrintf(TRUE, FALSE, "[ProcVCSUserManagerMsg] No users in current group\n");
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
						OspPrintf(TRUE, FALSE, "[ProcVCSUserManagerMsg] GetVCSCurUserList Failed at index: %d\n", byUsrItr);
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
				VcsLog("[ProcVCSMcuUnProcCfgMsg] fail to open mcuunproccfg.ini\n");
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
				VcsLog("[ProcVCSMcuUnProcCfgMsg] fail to open mcuunproccfg.ini\n");
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
                    VcsLog( "[ProcVCSConfTemMsg] get template confIdx<%d> failed\n", byConfIdx);
                    SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                    return;
                }

				// 搜索当前会议是否存在由该待修改模板所创建起的会议
				if(g_cMcuVcApp.IsConfE164Repeat(tOldTemInfo.m_tConfInfo.GetConfE164(), FALSE ))
				{
					cServMsg.SetErrorCode(ERR_MCU_TEMPLATE_INVC);
					VcsLog("[ProcVCSConfTemMsg] template is in vc\n");
					SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
					return;
				}

				if (MCS_MCU_DELTEMPLATE_REQ == pcMsg->event)
				{
					// VCS调度席删除需对应删除给用户配置的该调度席任务
					CConfId cConfId = cServMsg.GetConfId();
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
		OspPrintf(TRUE, FALSE, "[ProcVCSConfTemMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}	

}

/*====================================================================
    函数名      ：ProcVCSMcuCreateConfMsg
    功能        ：处理VCS的创会请求
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/12/04              付秀华        创建

====================================================================*/
void CVcsSsnInst::ProcVCSMcuCreateConfMsg( const CMessage * pcMsg )
{
	CServMsg	cServMsg(pcMsg->content,pcMsg->length);
    
    
//     //  n+1备份模式不允许创建模板
//     if (pcMsg->event == MCS_MCU_CREATECONF_BYTEMPLATE_REQ ||
//         pcMsg->event == MCS_MCU_CREATESCHCONF_BYTEMPLATE_REQ ||
//         pcMsg->event == MCS_MCU_CREATETEMPLATE_REQ ||
//         pcMsg->event == MCS_MCU_SAVECONFTOTEMPLATE_REQ)
//     {
//         if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_IDLE ||
//             g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_SWITCH)
//         {
//             OspPrintf(TRUE, FALSE, "[ProcMcsMcuDaemonConfMsg] mcs can't create template in n+1 mode!\n");
//             cServMsg.SetErrorCode(ERR_MCU_NPLUS_CREATETEMPLATE);
//             SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
//             return;
//         }
//     }    

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
		OspPrintf(TRUE, FALSE, "[ProcVCSMcuCreateConfMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
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
	VcsLog("[CVcsSsnInst%d][ProcVCSMcuVCMsg]: Console%d receive command message %d(%s)!\n",
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
		OspPrintf(TRUE, FALSE, "[ProcMcsMcuMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
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
	VcsLog("[CVcsSsnInst%d][ProcMcuVcsMsg]: Command reply Message %u(%s) to Console%d received!\n",
			GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event), GetInsID() );


	switch( CurState() )
	{
	case STATE_NORMAL:
        SendMsgToMcs( pcMsg->event, pcMsg->content, pcMsg->length );
		break;

	default:
		OspPrintf(TRUE, FALSE, "[ProcMcuVcsMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
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
	VcsLog("[ProcVCSConfOprMsg]: Console.%u receive command message %u(%s)!\n", 
            GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));

	CServMsg cServMsg(pcMsg->content, pcMsg->length);


	CConfId cConfId = cServMsg.GetConfId();

	switch( CurState() )
	{
	case STATE_NORMAL:
        switch( pcMsg->event)
        {
        case VCS_MCU_OPRCONF_REQ:
			{
				// 会议若被上级调度席开启，下级调度席不允许其它VCS操作
				CMcuVcInst* pVCInst = NULL;
				for (u8 byConfIdx= MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
				{
					pVCInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
					if (pVCInst != NULL && pVCInst->m_tConf.GetConfId() == cConfId &&
						CONF_CREATE_MT == pVCInst->m_byCreateBy)
					{
						cServMsg.SetErrorCode(ERR_MCU_VCS_MMCUINVC);
						SendMsgToMcs(cServMsg.GetEventId() + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
						return;						
					}

				}
				TUserTaskInfo tTaskInfo;
				if(g_cMcuVcApp.IsYourTask(m_achUser, cConfId))
				{
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
					QuitCurVCConf();
				}
				else
				{
					VcsLog("[ProcVCSConfOprMsg] VCS_MCU_QUITCONF_REQ to quit conf which is not current operated conf\n");
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
		OspPrintf(TRUE, FALSE, "[ProcVCSConfOprMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
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
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2008/12/12              付秀华        创建
====================================================================*/
void CVcsSsnInst::QuitCurVCConf()
{
	CConfId cConfID = g_cVcsSsnApp.GetCurConfID(GetInsID());
	if (cConfID.IsNull())
	{
		return;
	}
	
	CConfId cNull;
	g_cVcsSsnApp.SetCurConfID(GetInsID(), cNull);
	if (!g_cVcsSsnApp.ChkVCConfStatus(cConfID))
	{
		// 没有任何调度员操作该会议，则自动结束该会议
		CServMsg cServMsg;
        cServMsg.SetMcuId(LOCAL_MCUID);
        cServMsg.SetSrcSsnId(0);       // 不需要回应ACK消息给控制台
        cServMsg.SetSrcMtId(0);
		cServMsg.SetConfId(cConfID);
        cServMsg.SetEventId(MCS_MCU_RELEASECONF_REQ);
        
        g_cMcuVcApp.SendMsgToConf(cConfID, MCS_MCU_RELEASECONF_REQ,
                                  cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
	}

	
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
                VcsLog( "[Broadcast2SpecGrpVcsSsn]: Message %u(%s) In InsID.%u is send for Spec Group Id(%d) <--> Ssn Group Id(%d)\n",
                        wEvent, ::OspEventDesc( wEvent ), byInstID, 
                        byGrdId, bySsnGrpId);
            }
            else
            {
            
                VcsLog( "[Broadcast2SpecGrpVcsSsn]: Message %u(%s) In InsID.%u is Dropped for Spec Group Id(%d) <--> Ssn Group Id(%d)\n",
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
		OspPrintf(TRUE, FALSE, "[GetVCSSSsnData] NULL == pbyBuf\n");
		return FALSE;
	}

	dwOutBufLen = sizeof(m_acCurConfID);
	if (dwInBufLen < dwOutBufLen)
	{
		OspPrintf(TRUE, FALSE, "[GetVCSSSsnData] dwInBufLen(%d) < dwOutBufLen(%d)\n", 
			      dwInBufLen, dwOutBufLen);
		return FALSE;
	}

	memcpy(pbyBuf, &m_acCurConfID, dwOutBufLen);
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
		OspPrintf(TRUE, FALSE, "[SetVCSSSsnData] NULL == pbyBuf\n");
		return FALSE;
	}

	if (dwBufLen != sizeof(m_acCurConfID))
	{
		OspPrintf(TRUE, FALSE, "[SetVCSSSsnData] input lenth(%d) is not correct\n", dwBufLen);
		return FALSE;
	}

	memcpy(&m_acCurConfID, pbyBuf, dwBufLen);
	return TRUE;
}




















































