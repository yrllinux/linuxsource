/*****************************************************************************
   模块名      : mcu
   文件名      : mcsssn.cpp
   相关文件    : mcsssn.h
   文件实现功能: MCU与会议控制台会话类
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/07/25  0.9         李屹        创建
   2003/10/27  3.0         胡昌威      重大修改
   2005/02/19  3.6         万春雷      级联修改、与3.5版本合并
******************************************************************************/

#include "evmcu.h"
#include "evmcumcs.h"
#include "evmcuvcs.h"
#include "evmcutest.h"
#include "mcuvc.h"
#include "mcsssn.h"
#include "commonlib.h"
//#include "mcuerrcode.h"

#ifndef _UMCONST_H_
#include "umconst.h"
#endif

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
        //#include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

CMcsSsnApp	g_cMcsSsnApp;	//会议控制台会话应用实例
#ifdef _LINUX_
	#ifdef _UTF8
	CUsrManage  g_cUsrManage((u8*)DIR_DATA, NULL, ENCODE_UTF8, TRUE);   //用户管理对象
	#else
	CUsrManage  g_cUsrManage((u8*)DIR_DATA);   //用户管理对象
	#endif
#else
	#ifdef _UTF8
	CUsrManage  g_cUsrManage((u8*)DIR_DATA, NULL, ENCODE_UTF8, TRUE);   //用户管理对象
	#else
	CUsrManage  g_cUsrManage;   //用户管理对象
	#endif
#endif
CAddrBook   *g_pcAddrBook = NULL;  //地址簿管理对象

u8 CMcsSsn::m_abyUsrGrp[MAXNUM_MCU_MC + MAXNUM_MCU_VC];
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMcsSsnInst::CMcsSsnInst()
{
	m_dwMcsNode = INVALID_NODE;		//初始没有会议控制台连接
	m_dwMcsIp = 0;
	cCurConfId.SetNull();
}

CMcsSsnInst::~CMcsSsnInst()
{

}

/*====================================================================
    函数名      ：InstanceDump
    功能        ：重载打印信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 param, 打印状态过滤
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/18    1.0         LI Yi         创建
====================================================================*/
void CMcsSsnInst::InstanceDump( u32 param )
{
	StaticLog("%6u %4u 0x%x\n", GetInsID(), m_dwMcsNode,param );
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
====================================================================*/
#define MCS_EV_LOG(level)	MCU_EV_LOG(level, MID_MCU_MCS, "Mcs")

void CMcsSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	// 对于设置为无效状态的实例不进行任何消息处理
	if (CurState() == STATE_INVALID)
	{
		return;
	}

	if (NULL == pcMsg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "CMcsSsnInst: The received msg's pointer in the msg entry is NULL!");
		return;
	}

	switch (pcMsg->event)
	{
	case OSP_POWERON:
		{
			MCS_EV_LOG(LOG_LVL_KEYSTATUS);
			// 对于MCS创建32个实例，只有前16个实例有效，支持16个MCS同时连接
			if (GetInsID() > MAXNUM_MCU_MC)
			{
				NEXTSTATE( STATE_INVALID );
			}
		}
    	break;
	case OSP_DISCONNECT:				//控制台断链通知
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsDisconnect(pcMsg);
		break;
	case OSP_OVERFLOW:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcReoOverFlow(pcMsg);		//会议实例已满
		break;

	/*----------------------处理MCU得到的会控消息----------------------------------*/
	
    case MCS_MCU_GETMCUSTATUS_CMD:          //查询MCU状态请求
	case MCS_MCU_GETMCUSTATUS_REQ:	              
	case MCS_MCU_LISTALLCONF_REQ:	        //列出该MCU上所有会议信息
	case MCS_MCU_STOPSWITCHMC_REQ:	        //会议控制台停止播放请求
	case MCS_MCU_STOP_SWITCH_TW_REQ:	    //会议控制台向外设停止播放请求
	case MCS_MCU_GETMCUPERIEQPSTATUS_REQ:	//查询MCU外设状态
	case MCS_MCU_GETRECSTATUS_REQ:	        //查询录像机状态请求
	case MCS_MCU_GETMIXERSTATUS_REQ:	    //查询混音器状态请求
	case MCS_MCU_GETPERIDCSSTATUS_REQ:		//查询数字会议状态请求

	//录像文件管理
	case MCS_MCU_LISTALLRECORD_REQ:         //会控列表请求
	case MCS_MCU_DELETERECORD_REQ:          //会控请求删除文件
	case MCS_MCU_RENAMERECORD_REQ:          //会控请求更改文件名
	case MCS_MCU_PUBLISHREC_REQ:			//发布录像请求
	case MCS_MCU_CANCELPUBLISHREC_REQ:      //取消发布录像请求

    case MCS_MCU_CREATECONF_BYTEMPLATE_REQ:     //会议控制台在MCU上按会议模板创建一个即时会议
    case MCS_MCU_CREATESCHCONF_BYTEMPLATE_REQ:  //根据模板创建预约会议
    case MCS_MCU_CREATETEMPLATE_REQ:            //增加模板
    case MCS_MCU_MODIFYTEMPLATE_REQ:            //修改模板
    case MCS_MCU_DELTEMPLATE_REQ:               //删除模板

	case MCS_MCU_SAVECONFTOTEMPLATE_REQ:	//会议控制台请求将当前会议保存为会议模板(预留), zgc, 2007/04/20

    case MCS_MCU_CREATECONF_REQ:                //会议控制台直接创建会议

	//主控热备份单元测试接口
	case EV_TEST_TEMPLATEINFO_GET_REQ:
	case EV_TEST_CONFINFO_GET_REQ:
	case EV_TEST_CONFMTLIST_GET_REQ:
	case EV_TEST_ADDRBOOK_GET_REQ:
	case EV_TEST_MCUCONFIG_GET_REQ:
	case MCS_MCU_GETCRIMAC_REQ: 
		ProcMcsMcuDaemonConfMsg(pcMsg);
		break;   
		
	//建立连接
	case MCS_MCU_CONNECT_REQ:		  //控制台通知MCU请求准入
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsMcuConnectReq(pcMsg);
		break;
	case MCS_MCU_CURRTIME_REQ :		  //控制台查询mcu当前时间
		ProcMcsInquiryMcuTime(pcMsg);
		break;
        
        //获取mcu主备状态
    case MCS_MCU_GETMSSTATUS_REQ:        
    case MCU_MSSTATE_EXCHANGE_NTF:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
        ProcMcsMcuGetMsStatusReq(pcMsg);
        break;

        //控制台修改mcu系统时间
    case MCS_MCU_CHANGESYSTIME_REQ:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
        ProcMcsMcuChgSysTime(pcMsg);
        break;
    
	//会议管理
	case MCS_MCU_RELEASECONF_REQ:     //会议控制台请求MCU结束一个会议
	case MCS_MCU_CHANGECONFLOCKMODE_REQ: //会议控制台请求MCU锁定会议
	case MCS_MCU_CHANGECONFPWD_REQ:   //会议控制台请求MCU更改会议密码
	case MCS_MCU_GETLOCKINFO_REQ:     //得到会议独享消息
	case MCS_MCU_ENTERCONFPWD_ACK:    //会议控制台回应MCU的密码请求   
	case MCS_MCU_ENTERCONFPWD_NACK:   //会议控制台拒绝MCU的密码请求 
	case MCS_MCU_SAVECONF_REQ:        //会议控制台请求MCU保存会议
	case MCS_MCU_MODIFYCONF_REQ:	  //会控修改预约会议
	case MCS_MCU_DELAYCONF_REQ:       //会议控制台请求MCU延长会议
	case MCS_MCU_CHANGEVACHOLDTIME_REQ://会议控制台请求MCU改变语音激励切换时间

	//会议操作
	case MCS_MCU_SPECCHAIRMAN_REQ:    //会议控制台指定一台终端为主席
	case MCS_MCU_CANCELCHAIRMAN_REQ:  //会议控制台取消当前会议主席
	case MCS_MCU_SETCHAIRMODE_CMD:    //会议控制台设置会议的主席方式
	case MCS_MCU_MTSEESPEAKER_CMD:    //强制广播发言人
	case MCS_MCU_SPECSPEAKER_REQ:     //会议控制台指定一台终端发言
	case MCS_MCU_CANCELSPEAKER_REQ:   //会议控制台取消会议发言人
    case MCS_MCU_SPECOUTVIEW_REQ:     //会议控制台指定回传通道
	case MCS_MCU_ADDMT_REQ:           //会议控制台邀请终端入会	
	case MCS_MCU_ADDMTEX_REQ:         //会议控制台邀请终端入会的特殊调试消息，包含建立打开通道所使用的能力级
	case MCS_MCU_DELMT_REQ:           //会议控制台驱逐终端离会
	case MCS_MCU_STARTSWITCHMC_REQ:   //会控选看终端
	case MCS_MCU_SENDRUNMSG_CMD:      //会议控制台命令MCU向终端发送短消息，终端号为0表示发给所有终端
	case MCS_MCU_GETMTLIST_REQ:       //会议控制台发给MCU的得到终端列表请求
	case MCS_MCU_REFRESHMCU_CMD:	  //会议控制台发给MCU的刷新其他MCU的命令
	case MCS_MCU_GETCONFINFO_REQ:     //会议控制台向MCU查询会议信息
    case MCS_MCU_GETMAUSTATUS_REQ:    //会议控制台向MCU查询MAU信息
	case MCS_MCU_STARTSWITCHVMPMT_REQ: // xliang [1/17/2009] 主席选看VMP
	//会议控制---轮询控制
	case MCS_MCU_STARTPOLL_CMD:       //会议控制台命令该会议开始轮询广播
	case MCS_MCU_STOPPOLL_CMD:        //会议控制台命令该会议停止轮询广播  
	case MCS_MCU_PAUSEPOLL_CMD:       //会议控制台命令该会议暂停轮询广播   
	case MCS_MCU_RESUMEPOLL_CMD:      //会议控制台命令该会议继续轮询广播
	case MCS_MCU_GETPOLLPARAM_REQ:    //会议控制台向MCU查询会议轮询参数
    case MCS_MCU_CHANGEPOLLPARAM_CMD: //会议控制台命令该会议更新轮询列表
	case MCS_MCU_SPECPOLLPOS_REQ:	  //会议控制台指定轮询位置

    //电视墙轮询控制
    case MCS_MCU_STARTTWPOLL_CMD:
    case MCS_MCU_STOPTWPOLL_CMD:
    case MCS_MCU_PAUSETWPOLL_CMD:
    case MCS_MCU_RESUMETWPOLL_CMD:
    case MCS_MCU_GETTWPOLLPARAM_REQ:

	//hdu轮询控制
    case MCS_MCU_STARTHDUPOLL_CMD:
    case MCS_MCU_STOPHDUPOLL_CMD:
    case MCS_MCU_PAUSEHDUPOLL_CMD:
    case MCS_MCU_CHANGEHDUPOLLPARAM_CMD:	
    case MCS_MCU_RESUMEHDUPOLL_CMD:
    case MCS_MCU_GETHDUPOLLPARAM_REQ:

   //会议控制---语音激励控制发言
	case MCS_MCU_STARTVAC_REQ:        //会议控制台请求MCU开始语音激励控制发言		
	case MCS_MCU_STOPVAC_REQ:         //会议控制台请求MCU停止语音激励控制发言
	
	//会议控制---会议讨论
	case MCS_MCU_STARTDISCUSS_REQ:    //开始会议讨论请求
	case MCS_MCU_STOPDISCUSS_REQ:     //结束会议讨论请求
	case MCS_MCU_ADDMIXMEMBER_CMD:    //增加混音成员
	case MCS_MCU_REMOVEMIXMEMBER_CMD: //移除混音成员
    case MCS_MCU_CHANGEMIXDELAY_REQ:
    case MCS_MCU_REPLACEMIXMEMBER_CMD:  //替换混音成员

    //zbq[11/01/2007] 混音优化
    //case MCS_MCU_STARTDISCUSS_CMD:    //开始会议讨论命令 - 用于跨级开始讨论操作
    //case MCS_MCU_STOPDISCUSS_CMD:     //结束会议讨论命令 - 用于跨级开始讨论操作
    //case MCS_MCU_GETDISCUSSPARAM_REQ: //查询讨论参数请求
    case MCS_MCU_GETMIXPARAM_REQ:
        
	//会议控制---视频复合控制
	case MCS_MCU_STARTVMP_REQ:        //开始视频复合请求
	case MCS_MCU_STOPVMP_REQ:         //结束视频复合请求	
	case MCS_MCU_CHANGEVMPPARAM_REQ:  //会议控制台请求MCU改变视频复合参数			
	case MCS_MCU_GETVMPPARAM_REQ:     //查询视频复合成员请求
	case MCS_MCU_STARTVMPBRDST_REQ:   //会控请求MCU广播视频复合码流
	case MCS_MCU_STOPVMPBRDST_REQ:    //会控请求MCU停止广播视频复合码流
        
	case MCS_MCU_VMPPRISEIZE_ACK:		//vmp抢占应答// xliang [1/20/2009] 
	case MCS_MCU_VMPPRISEIZE_NACK:

	case MCS_MCU_STARTVMPPOLL_CMD:		//开始Vmp单通道轮询
	case MCS_MCU_STOPVMPPOLL_CMD:		//停止Vmp单通道轮询
	case MCS_MCU_PAUSEVMPPOLL_CMD:		//暂停Vmp单通道轮询
	case MCS_MCU_RESUMEVMPPOLL_CMD:		//继续Vmp单通道轮询
	case MCS_MCU_GETVMPPOLLPARAM_REQ:	//查询Vmp单通道轮询参数请求
		
	case MCS_MCU_START_VMPBATCHPOLL_REQ://开始画面合成批量轮询
	case MCS_MCU_PAUSE_VMPBATCHPOLL_CMD://暂停画面合成批量轮询
	case MCS_MCU_STOP_VMPBATCHPOLL_CMD: //停止画面合成批量轮询	
    //会议控制---复合电视墙控制
	case MCS_MCU_STARTVMPTW_REQ:       //开始复合电视墙请求
	case MCS_MCU_STOPVMPTW_REQ:        //结束复合电视墙请求	
	case MCS_MCU_CHANGEVMPTWPARAM_REQ: //会议控制台请求MCU改变复合电视墙参数			
//	case MCS_MCU_GETVMPPARAM_REQ:      //查询视频复合成员请求
//	case MCS_MCU_STARTVMPBRDST_REQ:    //会控请求MCU广播视频复合码流
//	case MCS_MCU_STOPVMPBRDST_REQ:     //会控请求MCU停止广播视频复合码流

	//会议控制---得到会议状态
	case MCS_MCU_GETCONFSTATUS_REQ:   //会议控制台向MCU查询会议状态

    //会议控制---点名
    case MCS_MCU_STARTROLLCALL_REQ:
    case MCS_MCU_STOPROLLCALL_REQ:
    case MCS_MCU_CHANGEROLLCALL_REQ:
	//会议控制取消回传通道中的终端
	case MCS_MCU_CANCELCASCADESPYMT_REQ:
        
	//终端控制
	case MCS_MCU_CALLMT_REQ:          //会议控制台请求MCU呼叫终端
	case MCS_MCU_SETCALLMTMODE_REQ:   //会议控制台设置MCU呼叫终端方式
	case MCS_MCU_DROPMT_REQ:          //会议控制台挂断终端请求
	case MCS_MCU_STARTSWITCHMT_REQ:   //会控强制目的终端选看源终端
	case MCS_MCU_STOPSWITCHMT_REQ:    //会控取消目的终端选看源终端
	case MCS_MCU_GETMTSTATUS_REQ:     //会控向MCU查询终端状态
	case MCS_MCU_GETALLMTSTATUS_REQ:  //会控向MCU查询所有终端状态
	case MCS_MCU_GETMTALIAS_REQ:      //会控向MCU查询终端别名
	case MCS_MCU_GETALLMTALIAS_REQ:	  //会控向MCU查询所有终端别名
	case MCS_MCU_SETMTBITRATE_CMD:    //会议控制台命令MCU调节终端码率
	case MCS_MCU_SETMTVIDSRC_CMD:     //会控要求MCU设置终端视频源
	case MCS_MCU_MTAUDMUTE_REQ:       //会控要求MCU设置终端静音
    case MCS_MCU_GETMTBITRATE_REQ:    //会控查询终端码率
	case MCS_MCU_SETMTVOLUME_CMD:	  //会控实时设置终端音量，zgc,2006-12-26
    case MCS_MCU_GETMTEXTINFO_REQ:    //会控请求终端的二次扩展信息，版本等

	//终端摄像头调节	
	case MCS_MCU_MTCAMERA_CTRL_CMD:         //会议控制台命令终端摄像机镜头运动
	case MCS_MCU_MTCAMERA_CTRL_STOP:        //会议控制台命令终端摄像机镜头停止运动
	case MCS_MCU_MTCAMERA_RCENABLE_CMD:     //会议控制台命令终端摄像机遥控使能	
	case MCS_MCU_MTCAMERA_SAVETOPOS_CMD:    //会议控制台命令终端摄像机将当前位置信息存入指定位置	
	case MCS_MCU_MTCAMERA_MOVETOPOS_CMD:    //会议控制台命令终端摄像机调整到指定位置

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

	//录放像控制
	case MCS_MCU_STARTREC_REQ:              //会控向MCU请求录像
	case MCS_MCU_PAUSEREC_REQ:              //会控向MCU暂停录像
	case MCS_MCU_RESUMEREC_REQ:             //会控向MCU恢复录像
	case MCS_MCU_STOPREC_REQ:               //会控向MCU停止录像
	case MCS_MCU_CHANGERECMODE_REQ:			//请求改变录像模式
	case MCS_MCU_STARTPLAY_REQ:             //会控开始放像请求
	case MCS_MCU_PAUSEPLAY_REQ:             //会控暂停放像请求
	case MCS_MCU_RESUMEPLAY_REQ:            //会控恢复放像请求
	case MCS_MCU_STOPPLAY_REQ:              //会控停止放像请求
	case MCS_MCU_FFPLAY_REQ:                //会控快进放像请求
	case MCS_MCU_FBPLAY_REQ:                //会控快退放像请求
	case MCS_MCU_SEEK_REQ:                  //会控调整放像进度请求
    case MCS_MCU_GETRECPROG_CMD:            //会控查询录相进度
    case MCS_MCU_GETPLAYPROG_CMD:           //会控查询放相进度
	case MCS_MCU_CLOSELISTRECORD_CMD:		//会控关闭文件列表（用于挂断vrs新录播实体）
	case MCS_MCU_GETRECSTATUS_CMD:			//mcs请求录像状态

	//电视墙控制
	case MCS_MCU_START_SWITCH_TW_REQ:       //会议控制台请求将指定Mt的图像交换到指定外设的指定索引的通道上
	case MCS_MCU_MCUMEDIASRC_REQ:
	case MCS_MCU_LOCKSMCU_REQ:
	case MCS_MCU_GETMCULOCKSTATUS_REQ: 
		
	//Hdu控制   4.6.1新加版本  jlb
	case MCS_MCU_START_SWITCH_HDU_REQ: 
	case MCS_MCU_STOP_SWITCH_HDU_REQ:
	case MCS_MCU_CHANGEHDUVOLUME_REQ:
	case MCS_MCU_CHGHDUVMPMODE_CMD:

	//批量轮询
	case MCS_MCU_STARTHDUBATCHPOLL_REQ:
	case MCS_MCU_STOPHDUBATCHPOLL_REQ:
	case MCS_MCU_RESUMEHDUBATCHPOLL_REQ:
	case MCS_MCU_PAUSEHDUBATCHPOLL_REQ:
	case MCS_MCU_GETBATCHPOLLSTATUS_CMD:

	//发言模式
    case MCS_MCU_CHGSPEAKMODE_REQ:

	case MCS_MCU_GETIFRAME_CMD:
	
		//界面、终端消息透传
    case MCS_MCU_TRANSPARENTMSG_NOTIFY:

	// 会控获取终端视频源别名信息
	case MCS_MCU_GETMTVIDEOALIAS_REQ:

	case VCS_MCU_ADDPLANNAME_REQ:			//MCS请求添加预案
	case VCS_MCU_DELPLANNAME_REQ:			//MCS请求删除预案
	case VCS_MCU_MODIFYPLANNAME_REQ:		//MCS请求修改预案名称
	case VCS_MCU_GETALLPLANDATA_REQ:		//MCS请求获得所有预案数据信息
	case MCS_MCU_CHANGEMTSECVIDSEND_CMD:    //[5/4/2011 zhushengze]VCS控制发言人发双流
		MCS_EV_LOG(LOG_LVL_DETAIL);
		ProcMcsMcuMsg(pcMsg);
		break;
	/*---------------------处理MCU发给会控的消息------------------------------------------*/

    //3.2	会议管理
    //3.2.1	请求会议列表	  
	case MCU_MCS_CONFINFO_NOTIF:    //会议完整信息通知

    //3.2.2	创建会议	     
	case MCU_MCS_CONFREGGKSTATUS_NOTIF: //会议在GK上注册状态通知
    case MCU_MCS_TEMSCHCONFINFO_NOTIF:
    case MCU_MCS_MODIFYTEMPLATE_ACK:
    case MCU_MCS_MODIFYTEMPLATE_NACK:
    case MCU_MCS_DELTEMPLATE_ACK:
    case MCU_MCS_DELTEMPLATE_NACK:
    case MCU_MCS_DELTEMPLATE_NOTIF:
	case MCU_MCS_MMCUCALLIN_NOTIF: // guzh [6/20/2007] 上级MCU呼叫通知
                    
    //3.2.3	结束会议   
	case MCU_MCS_RELEASECONF_ACK:  //MCU成功结束会议应答  
	case MCU_MCS_RELEASECONF_NACK: //MCU拒绝结束会议   
	case MCU_MCS_RELEASECONF_NOTIF://MCU拒绝结束会议

    //3.2.4	保护会议     
	case MCU_MCS_CHANGECONFLOCKMODE_ACK:   //MCU成功锁定会议应答  
	case MCU_MCS_CHANGECONFLOCKMODE_NACK:  //MCU拒绝锁定会议  
	case MCU_MCS_CHANGECONFPWD_ACK:    //MCU成功更改会议密码应答
	case MCU_MCS_CHANGECONFPWD_NACK:   //MCU拒绝更改会议密码
	case MCU_MCS_ENTERCONFPWD_REQ:     //MCU要求会议控制台输入会议密码
	case MCU_MCS_WRONGCONFPWD_NOTIF:   //MCU提示会议控制台密码错误
	case MCU_MCS_LOCKUSERINFO_NOTIFY:  //MCU提示会议控制台当前独享会控的信息
	case MCU_MCS_GETLOCKINFO_ACK:      //得到会议独享应答
	case MCU_MCS_GETLOCKINFO_NACK:     //得到会议独享拒绝

    //3.2.5	保存会议   
	case MCU_MCS_SAVECONF_ACK: //MCU成功保存会议应答   
	case MCU_MCS_SAVECONF_NACK://MCU拒绝保存会议


    //3.2.6	修改预约会议   
	case MCU_MCS_MODIFYCONF_ACK://MCU成功修改预约会议   
	case MCU_MCS_MODIFYCONF_NACK: //MCU拒绝修改预约会议


    //3.2.7	延长会议	
	case MCU_MCS_DELAYCONF_ACK:	//MCU成功延长会议应答	
	case MCU_MCS_DELAYCONF_NACK://MCU拒绝延长会议应答
	case MCU_MCS_DELAYCONF_NOTIF://MCU延长会议通知
	case MCU_MCS_CONFTIMELEFT_NOTIF://MCU通知会控会议结束剩余时间
		
    //3.3	会议操作
    //3.3.1	指定主席	
	case MCU_MCS_SPECCHAIRMAN_ACK://MCU成功指定会议主席应答	
	case MCU_MCS_SPECCHAIRMAN_NACK://MCU拒绝指定会议主席应答
	case MCU_MCS_SPECCHAIRMAN_NOTIF://指定主席通知
	case MCU_MCS_MTAPPLYCHAIRMAN_NOTIF://终端申请主席通知

    //3.3.2	取消主席		
	case MCU_MCS_CANCELCHAIRMAN_ACK://MCU成功取消会议主席应答	
	case MCU_MCS_CANCELCHAIRMAN_NACK://会议中无主席	
	case MCU_MCS_CANCELCHAIRMAN_NOTIF://取消主席通知

    //3.3.3	指定发言	  
	case MCU_MCS_SPECSPEAKER_ACK://MCU成功指定发言者 
	case MCU_MCS_SPECSPEAKER_NACK://指定之前的发言者被取消 
	case MCU_MCS_SPECSPEAKER_NOTIF://指定发言通知
	case MCU_MCS_MTAPPLYSPEAKER_NOTIF://终端申请发言
	case MCU_MCS_MTAPPLYSPEAKERLIST_NOTIF://终端申请发言

    //3.3.4	取消发言  
	case MCU_MCS_CANCELSPEAKER_ACK: //MCU成功取消会议主席应答   
	case MCU_MCS_CANCELSPEAKER_NACK://取消发言人失败
	case MCU_MCS_CANCELSPEAKER_NOTIF://取消发言人通知

    //3.3.5 指定回传
    case MCU_MCS_SPECOUTVIEW_ACK:   //MCU成功指定本会场向上级回传的终端
    case MCU_MCS_SPECOUTVIEW_NACK:  //指定本会场向上级回传的终端失败

    //3.3.6	增加终端	
	case MCU_MCS_ADDMT_ACK://被邀终端成功加入会议应答	
	case MCU_MCS_ADDMT_NACK://被邀终端没有加入会议

    //3.3.7	删除终端		
	case MCU_MCS_DELMT_ACK://终端被成功驱逐	
	case MCU_MCS_DELMT_NACK://消息描述：拒绝驱逐终端
	
    //3.3.10	发送短消息		
	case MCU_MCS_SENDRUNMSG_NOTIF://MCU给会议控制台的发送短消息通知

    //3.3.11	请求终端列表   
	case MCU_MCS_GETMTLIST_ACK: //MCU回应会议控制台的得到终端列表请求   
	case MCU_MCS_GETMTLIST_NACK://MCU拒绝给出会议列表   
	case MCU_MCS_MTLIST_NOTIF:  //MCU发给会议控制台的终端列表通知
	case MCU_MCS_GETMTLIST_NOTIF: //MCU给界面发的终端列表消息
	case MCU_MCS_MTTYPECHANGE_NOTIF:    //MCU发给会议控制台的终端类型改变通知
	case MCU_MCS_CONFALLMTINFO_NOTIF:   //MCU发给会议控制台的终端所有信息通知		

    case MCU_MCS_GETMTEXTINFO_ACK:      //获取终端二次扩展信息同意应答
    case MCU_MCS_GETMTEXTINFO_NACK:     //获取终端二次扩展信息拒绝应答

    //3.3.12	查询会议信息  
	case MCU_MCS_GETCONFINFO_ACK://MCU回应会议控制台的会议信息请求   
	case MCU_MCS_GETCONFINFO_NACK://MCU拒绝会议控制台的会议信息请求

    //3.3.13    查询MAU信息
    case MCU_MCS_GETMAUSTATUS_ACK:  //MCU回应会议控制台的MAU信息请求   
    case MCU_MCS_GETMAUSTATUS_NACK: //MCU拒绝会议控制台的MAU信息请求
    case MCU_MCS_MAUSTATUS_NOTIFY:  //MCU通知会议控制台的MAU信息

	//3.3.14    发言模式调整
    case MCU_MCS_CHGSPEAKMODE_ACK:  //MCU回应同意发言模式调整
    case MCU_MCS_CHGSPEAKMODE_NACK: //MCU回应拒绝发言模式调整	
    //3.4	会议控制
    //3.4.1	轮询操作
    //3.4.1.1	轮询命令   
    //3.4.1.2	查询轮询参数     
	case MCU_MCS_GETPOLLPARAM_ACK:      //MCU应答会议控制台查询会议轮询参数   
	case MCU_MCS_GETPOLLPARAM_NACK:     //MCU拒绝会议控制台查询会议轮询参数   
	case MCU_MCS_POLLSTATE_NOTIF:       //MCU通知会议控制台当前的轮询状态
    case MCU_MCS_POLLPARAMCHANGE_NTF:   //MCU通知所有的会控刷新轮询终端列表
	case MCU_MCS_SPECPOLLPOS_ACK:		//MCU应答会议控制台指定轮询位置
	case MCU_MCS_SPECPOLLPOS_NACK:		//MCU拒绝会议控制台指定轮询位置

    //电视墙轮询
    case MCU_MCS_GETTWPOLLPARAM_ACK:
    case MCU_MCS_GETTWPOLLPARAM_NACK:
    case MCU_MCS_TWPOLLSTATE_NOTIF:

	//hdu轮询
    case MCU_MCS_GETHDUPOLLPARAM_ACK:
    case MCU_MCS_GETHDUPOLLPARAM_NACK:
    case MCU_MCS_HDUPOLLSTATE_NOTIF:

	//vmp单通道轮询
    case MCU_MCS_GETVMPPOLLPARAM_ACK:
    case MCU_MCS_GETVMPPOLLPARAM_NACK:
    case MCU_MCS_VMPPOLLSTATE_NOTIF:

    //3.4.2	语音激励控制
    //3.4.2.1	启用语音激励控制发言     
	case MCU_MCS_STARTVAC_ACK://MCU同意会议控制台开始语音激励控制发言的请求   
	case MCU_MCS_STARTVAC_NACK://MCU拒绝会议控制台开始语音激励控制发言的请求
	case MCU_MCS_STARTVAC_NOTIF://MCU给会议控制台开始语音激励控制发言的通知

    //3.4.2.2	取消语音激励控制发言     
	case MCU_MCS_STOPVAC_ACK://MCU同意会议控制台停止语音激励控制发言的请求  
	case MCU_MCS_STOPVAC_NACK://MCU拒绝会议控制台停止语音激励控制发言的请求
	case MCU_MCS_STOPVAC_NOTIF:// libo [5/20/2005]

	//3.4.2.3  会议控制台改变语音激励切换时间
	case MCU_MCS_CHANGEVACHOLDTIME_ACK:  //MCU同意会议控制台改变语音激励切换时间的请求  
	case MCU_MCS_CHANGEVACHOLDTIME_NACK: //MCU拒绝会议控制台改变语音激励切换时间的请求
	case MCU_MCS_CHANGEVACHOLDTIME_NOTIF://MCU给会议控制台改变语音激励切换时间的通知

    //3.4.3	会议混音控制
    //3.4.3.1	开始混音	
	case MCU_MCS_STARTDISCUSS_ACK:      //MCU同意开始会议讨论
	case MCU_MCS_STARTDISCUSS_NACK:     //MCU拒绝开始会议讨论
	//case MCU_MCS_STARTDISCUSS_NOTIF:  //MCU开始会议讨论通知

    //3.4.3.2	结束混音		
	case MCU_MCS_STOPDISCUSS_ACK:       //MCU同意会议控制台结束会议讨论的请求	
	case MCU_MCS_STOPDISCUSS_NACK:      //MCU拒绝会议控制台结束会议讨论的请求
	//case MCU_MCS_STOPDISCUSS_NOTIF:   //MCU给会议控制台结束会议讨论的通知

    //3.4.3.3	查询混音参数

    //zbq[11/01/2007] 混音优化
	//case MCU_MCS_GETDISCUSSPARAM_ACK:  //MCU同意查询讨论参数	
	//case MCU_MCS_GETDISCUSSPARAM_NACK: //MCU拒绝查询讨论参数
	//case MCU_MCS_DISCUSSPARAM_NOTIF:   //MCU给会议控制台的讨论参数通知
    case MCU_MCS_MIXPARAM_NOTIF:
    case MCU_MCS_GETMIXPARAM_ACK:

	//3.4.3.4  混音成员控制	
	case MCU_MCS_MTAPPLYMIX_NOTIF:    //终端申请加入混音通知

	//3.4.3.5  定制混音下混音成员整体替换控制
//	case MCU_MCS_MODIFYMIXMEMBER_ACK:  //MCU同意定制混音下混音成员整体替换控制
//	case MCU_MCS_MODIFYMIXMEMBER_NACK: //MCU拒绝定制混音下混音成员整体替换控制

        //混音延时设置
    case MCU_MCS_CHANGEMIXDELAY_ACK:
    case MCU_MCS_CHANGEMIXDELAY_NACK:
    case MCU_MCS_CHANGEMIXDELAY_NOTIF:

    //3.4.4	视频复合控制
    //3.4.4.1	开始视频复合		
	case MCU_MCS_STARTVMP_ACK:   //MCU同意视频复合请求	
	case MCU_MCS_STARTVMP_NACK:  //MCU不同意视频复合请求
	case MCU_MCS_STARTVMP_NOTIF: //视频复合成功开始通知

    //3.4.4.2 停止视频复合		
	case MCU_MCS_STOPVMP_ACK:   //MCU同意视频结束复合请求	
	case MCU_MCS_STOPVMP_NACK:  //MCU不同意结束视频复合请求
	case MCU_MCS_STOPVMP_NOTIF: //视频复合成功结束通知
	
    //3.4.4.3	改变视频复合参数	
	case MCU_MCS_CHANGEVMPPARAM_ACK://MCU同意会议控制台的改变混音参数请求	
	case MCU_MCS_CHANGEVMPPARAM_NACK://MCU拒绝会议控制台的改变混音参数请求
    case MCU_MCS_CHANGEVMPPARAM_NOTIF:

    //3.4.4.4	查询视频复合参数			
	case MCU_MCS_GETVMPPARAM_ACK://MCU同意查询混音成员	
	case MCU_MCS_GETVMPPARAM_NACK://MCU不同意查询混音成员	
	case MCU_MCS_VMPPARAM_NOTIF://MCU给会议控制台的混音参数通知
	
    //3.4.4.5	复合视频流广播控制
	case MCU_MCS_STARTVMPBRDST_ACK:  //MCU同意广播视频复合码流	
	case MCU_MCS_STARTVMPBRDST_NACK: //MCU拒绝广播视频复合码流	
	case MCU_MCS_STOPVMPBRDST_ACK:   //MCU同意停止广播视频复合码流	
	case MCU_MCS_STOPVMPBRDST_NACK:  //MCU拒绝停止广播视频复合码流

    //3.4.5	查询会议状态		
	case MCU_MCS_GETCONFSTATUS_ACK://MCU给出查询会议状态应答	
	case MCU_MCS_GETCONFSTATUS_NACK://MCU拒绝给出查询会议状态应答	
	case MCU_MCS_CONFSTATUS_NOTIF://MCU给会议控制台的会议状态通知
        
    //------------------------------------------------------------------
    case MCU_MCS_STARTVMPTW_ACK:
    //MCU不同意复合电视墙请求, 消息体： 无
    case MCU_MCS_STARTVMPTW_NACK:
    //复合电视墙成功开始通知, 消息体：  TVMPParam
    case MCU_MCS_STARTVMPTW_NOTIF:

//3.4.4.2 停止复合电视墙
    //MCU同意结束复合电视墙请求, 消息体：  无
    case MCU_MCS_STOPVMPTW_ACK:
    //MCU不同意结束复合电视墙请求, 消息体：  无
    case MCU_MCS_STOPVMPTW_NACK:
    //复合电视墙成功结束通知, 消息体：  无
    case MCU_MCS_STOPVMPTW_NOTIF:

//3.4.4.3	改变复合电视墙参数
    //MCU同意会议控制台的改变复合电视墙参数请求, 消息体： TVMPParam
    case MCU_MCS_CHANGEVMPTWPARAM_ACK:
    //MCU拒绝会议控制台的改变复合电视墙参数请求, 消息体：  无
    case MCU_MCS_CHANGEVMPTWPARAM_NACK:
    //MCU给会议控制台的视频复合参数通知, 消息体：  TVMPParam
	case MCU_MCS_VMPTWPARAM_NOTIF:

    //点名控制
    case MCU_MCS_STARTROLLCALL_ACK:
    case MCU_MCS_STARTROLLCALL_NACK:
    case MCU_MCS_STARTROLLCALL_NOTIF:

    case MCU_MCS_STOPROLLCALL_ACK:
    case MCU_MCS_STOPROLLCALL_NACK:
    case MCU_MCS_STOPROLLCALL_NOTIF:
        
    case MCU_MCS_CHANGEROLLCALL_ACK:
    case MCU_MCS_CHANGEROLLCALL_NACK:
    case MCU_MCS_CHANGEROLLCALL_NOTIF:
	//取消回传通道中终端和交换
	case MCU_MCS_CANCELCASCADESPYMT_ACK:
	case MCU_MCS_CANCELCASCADESPYMT_NACK:
        
    //3.5	终端操作
    //3.5.1	呼叫终端	
	case MCU_MCS_CALLMT_ACK://被呼叫终端成功应答	
	case MCU_MCS_CALLMT_NACK://被呼叫终端没有应答
	case MCU_MCS_MTONLINECHANGE_NOTIF: //终端在线状态变化通知
	case MCU_MCS_CALLMTFAILURE_NOTIF: //呼叫终端失败通知
		
    //3.5.2	设置呼叫终端方式	
	case MCU_MCS_SETCALLMTMODE_ACK://成功设置MCU呼叫终端方式	
	case MCU_MCS_SETCALLMTMODE_NACK://拒绝设置MCU呼叫终端方式	

    //3.5.3	挂断终端		
	case MCU_MCS_DROPMT_ACK://终端被成功挂断	
	case MCU_MCS_DROPMT_NACK://MCU拒绝挂断该终端


    //3.5.4	强制终端选看		
	case MCU_MCS_STARTSWITCHMT_ACK://MCU应答会控强制目的终端选看源终端	
	case MCU_MCS_STARTSWITCHMT_NACK://拒绝会控强制目的终端选看源终端
		
    //3.5.5	取消终端选看		
	case MCU_MCS_STOPSWITCHMT_ACK://MCU应答会控取消目的终端选看源终端	
	case MCU_MCS_STOPSWITCHMT_NACK://MCU拒绝会控取消目的终端选看源终端


    //3.5.6	查询终端状态		
	case MCU_MCS_GETMTSTATUS_ACK://MCU应答会议控制台查询终端状态	
	case MCU_MCS_GETMTSTATUS_NACK://MCU拒绝会议控制台的查询终端状态	
	case MCU_MCS_MTSTATUS_NOTIF://MCU给会议控制台的查询终端状态通知
	case MCU_MCS_ALLMTSTATUS_NOTIF://所有终端状态通知

	case MCU_MCS_GETALLMTSTATUS_ACK:	//MCU应答会议控制台查询所有终端状态
	case MCU_MCS_GETALLMTSTATUS_NACK:	//MCU拒绝会议控制台的查询所有终端状态

    //3.5.7	查询终端别名		
	case MCU_MCS_GETMTALIAS_ACK://MCU应答会议控制台查询终端别名	
	case MCU_MCS_GETMTALIAS_NACK://MCU没有终端的别名信息	
	case MCU_MCS_MTALIAS_NOTIF://MCU给会议控制台的终端别名通知
	case MCU_MCS_ALLMTALIAS_NOTIF: //MCU给会议控制台的所有终端别名通知

	case MCU_MCS_GETALLMTALIAS_ACK:			//MCU应答会议控制台查询所有终端别名
	case MCU_MCS_GETALLMTALIAS_NACK:		//MCU拒绝会议控制台的查询所有终端别名

    //3.5.10	设置终端视频源	
	//case MCU_MCS_SETMTVIDSRC_ACK://MCU应答会议控制台设置终端视频源的要求	
	//case MCU_MCS_SETMTVIDSRC_NACK://MCU拒绝会议控制台设置终端视频源的要求	
	//case MCU_MCS_SETMTVIDSRC_NOTIF://MCU通知会议控制台重新设置了终端视频源

    //3.5.11	设置终端音频源	
	//case MCU_MCS_SETMTAUDSRC_ACK://MCU应答会议控制台设置终端音频源的要求	
	//case MCU_MCS_SETMTAUDSRC_NACK://MCU拒绝会议控制台设置终端音频源的要求	
	//case MCU_MCS_SETMTAUDSRC_NOTIF://MCU通知会议控制台重新设置了终端音频源
	
    //3.5.13	设置终端静音	
	case MCU_MCS_MTAUDMUTE_ACK://MCU应答会议控制台设置终端静音	
	case MCU_MCS_MTAUDMUTE_NACK://MCU拒绝会议控制台设置终端静音	
	case MCU_MCS_MTAUDMUTE_NOTIF://MCU通知会议控制台设置终端静音


    //3.5.15有关视频内置矩阵操作
    case MCU_MCS_MATRIX_ALLSCHEMES_NOTIF:
    case MCU_MCS_MATRIX_ONESCHEME_NOTIF:
    case MCU_MCS_MATRIX_SAVESCHEME_NOTIF:
    case MCU_MCS_MATRIX_SETCURSCHEME_NOTIF:
    case MCU_MCS_MATRIX_CURSCHEME_NOTIF:

        //外置矩阵
    case MCU_MCS_EXMATRIXINFO_NOTIFY:   
    case MCU_MCS_EXMATRIX_GETPORT_ACK:
    case MCU_MCS_EXMATRIX_GETPORT_NACK:
    case MCU_MCS_EXMATRIX_GETPORT_NOTIF:
    case MCU_MCS_EXMATRIX_PORTNAME_NOTIF:
    case MCU_MCS_EXMATRIX_ALLPORTNAME_NOTIF:

        //扩展视频源
    case MCU_MCS_ALLVIDEOSOURCEINFO_NOTIF:
    case MCU_MCS_VIDEOSOURCEINFO_NOTIF:
		
    //3.6.1.1.1	请求录像	
	case MCU_MCS_STARTREC_ACK://MCU同意会控放像请求	
	case MCU_MCS_STARTREC_NACK://MCU拒绝会控放像请求	
	case MCU_MCS_RECPROG_NOTIF://MCU通知会控录像进度

    //3.6.1.1.2	暂停录像请求	
	case MCU_MCS_PAUSEREC_ACK://MCU同意会控暂停放像请求	
	case MCU_MCS_PAUSEREC_NACK://MCU拒绝会控暂停放像请求

    //3.6.1.1.3	恢复录像	
	case MCU_MCS_RESUMEREC_ACK://MCU同意会控恢复放像请求	
	case MCU_MCS_RESUMEREC_NACK://MCU拒绝会控恢复放像请求

    //3.6.1.1.4	停止录像		
	case MCU_MCS_STOPREC_ACK://MCU同意会控停止放像请求	
	case MCU_MCS_STOPREC_NACK://MCU拒绝会控停止放像请求

    //3.6.1.2	放像操作
    //3.6.1.2.1	开始播放		
	case MCU_MCS_STARTPLAY_ACK://MCU开始放像应答	
	case MCU_MCS_STARTPLAY_NACK://MCU拒绝开始放像	
	case MCU_MCS_PLAYPROG_NOTIF://放像进度通知

	case MCU_MCS_CHANGERECMODE_ACK:			//请求改变录像模式
	case MCU_MCS_CHANGERECMODE_NACK:		//请求改变录像模式
	
    //3.6.1.2.2	暂停播放		
	case MCU_MCS_PAUSEPLAY_ACK://MCU暂停放像应答	
	case MCU_MCS_PAUSEPLAY_NACK://MCU拒绝暂停放像

    //3.6.1.2.3	恢复放像		
	case MCU_MCS_RESUMEPLAY_ACK://MCU恢复放像应答	
	case MCU_MCS_RESUMEPLAY_NACK://MCU拒绝恢复放像

    //3.6.1.2.4	停止放像	
	case MCU_MCS_STOPPLAY_ACK://MCU停止放像应答	
	case MCU_MCS_STOPPLAY_NACK://MCU停止恢复放像

    //3.6.1.2.5	请求快进放像		
	case MCU_MCS_FFPLAY_ACK://MCU快进放像应答	
	case MCU_MCS_FFPLAY_NACK://MCU快进放像

    //3.6.1.2.6	请求快退放像	
	case MCU_MCS_FBPLAY_ACK://MCU快进放像应答	
	case MCU_MCS_FBPLAY_NACK://MCU快进放像

    //3.6.1.2.7	放像进度调整	
	case MCU_MCS_SEEK_ACK://MCU同意调整放像进度应答	
	case MCU_MCS_SEEK_NACK://MCU拒绝调整放像进度应答	

    //3.6.3	电视墙操作	
	case MCU_MCS_START_SWITCH_TW_ACK:      //MCU同意交换图像	
	case MCU_MCS_START_SWITCH_TW_NACK:     //MCU同意拒绝图像		
	case MCU_MCS_STOP_SWITCH_TW_ACK:       //MCU同意交换图像	
	case MCU_MCS_STOP_SWITCH_TW_NACK:      //MCU拒绝交换图像
		
	case MCU_MCS_GETPERIDCSSTATUS_ACK:		//MCU应答查询数字会议状态请求
	case MCU_MCS_GETPERIDCSSTATUS_NACK:		//MCU拒绝查询数字会议状态请求
	
	case MCU_MCS_MCUMEDIASRC_NOTIF:
	case MCU_MCS_MCUMEDIASRC_ACK:
	case MCU_MCS_MCUMEDIASRC_NACK:
	case MCU_MCS_LOCKSMCU_ACK:
	case MCU_MCS_LOCKSMCU_NACK:
	case MCU_MCS_GETMCULOCKSTATUS_ACK:
	case MCU_MCS_GETMCULOCKSTATUS_NACK:
	case MCU_MCS_MCULOCKSTATUS_NOTIF:

    case MCU_MCS_CALLMTMODE_NOTIF:
    case MCU_MCS_CONFLOCKMODE_NOTIF:
    case MCU_MCS_CONFCHAIRMODE_NOTIF:
    case MCU_MCS_CHANGECONFPWD_NOTIF:
    case MCU_MCS_CONFMODE_NOTIF:
    case MCU_MCS_SIMCONFINFO_NOTIF:

    case MCU_MCS_GETMTBITRATE_ACK:
    case MCU_MCS_GETMTBITRATE_NACK:
    case MCU_MCS_MTBITRATE_NOTIF:

    case MCU_MCS_MCUPERIDCSSTATUS_NOTIF:
    case MCU_MCS_LISTALLRECORD_NOTIF:  //录像记录列表通知

	case MCU_MCS_MPFLUXOVERRUN_NOTIFY: //zgc [2007/01/12] 通知MCS码流转发能力超限

	case MCU_MCS_VMPPRISEIZE_REQ:	   // xliang [1/20/2009] VMP 抢占提示请求
	case MCU_MCS_VMPOVERHDCHNNLLMT_NTF:
	case MCU_MCS_STARTSWITCHVMPMT_ACK:	// xliang [3/31/2009] 主席选看VMP应答
	case MCU_MCS_STARTSWITCHVMPMT_NACK:

    // 4.6 HDU
	case MCU_MCS_START_SWITCH_HDU_ACK:   //  jlb  [2009/02/05] 
	case MCU_MCS_START_SWITCH_HDU_NACK:
	case MCU_MCS_STOP_SWITCH_HDU_ACK:
	case MCU_MCS_STOP_SWITCH_HDU_NACK:
	case MCU_MCS_CHGHDUVMPMODE_NOTIF:
		
	//修改Hdu预案应答
	case MCU_MCS_CHANGEHDUSCHEMEINFO_ACK:
	case MCU_MCS_CHANGEHDUSCHEMEINFO_NACK:
	case MCU_MCS_HDUSCHEMEINFO_NOTIF:
    
	//批量轮询应答
	case MCU_MCS_STARTHDUBATCHPOLL_ACK:
	case MCU_MCS_STARTHDUBATCHPOLL_NACK:
	case MCU_MCS_STOPHDUBATCHPOLL_ACK:
	case MCU_MCS_STOPHDUBATCHPOLL_NACK:
	case MCU_MCS_RESUMEHDUBATCHPOLL_ACK:
	case MCU_MCS_RESUMEHDUBATCHPOLL_NACK:
	case MCU_MCS_PAUSEHDUBATCHPOLL_ACK:
	case MCU_MCS_PAUSEHDUBATCHPOLL_NACK:
	
	//批量轮询通知消息
	case MCU_MCS_HDUBATCHPOLL_STATUS_NOTIF:

	// [pengjie 2010/5/18] 下级会议扩展属性通知界面（是否支持多回传）
	case MCU_MCS_CONFEXTINFO_NOTIF:
	
	//[2/28/2012 zhushengze]界面、终端消息透传
    case MCU_MCS_TRANSPARENTMSG_NOTIFY:

	//  终端视频源别名信息通知界面
	case MCU_MCS_GETMTVIDEOALIAS_ACK:
	case MCU_MCS_GETMTVIDEOALIAS_NACK:
	case MCU_MCS_MTVIDEOALIAS_NOTIF:

	case MCU_VCS_ADDPLANNAME_ACK:
	case MCU_VCS_ADDPLANNAME_NACK:
	case MCU_VCS_DELPLANNAME_ACK:
	case MCU_VCS_DELPLANNAME_NACK:
	case MCU_VCS_MODIFYPLANNAME_ACK:
	case MCU_VCS_MODIFYPLANNAME_NACK:
	case MCU_VCS_ALLPLANNAME_NOTIFY:
	case MCU_VCS_ONEPLANDATA_NOTIFY:
	case MCU_VCS_GETALLPLANDATA_ACK:
	case MCU_VCS_GETALLPLANDATA_NACK:
	case MCU_VCS_ALLPLANDATA_SINGLE_NOTIFY:
	case MCU_VCS_MODIFYPLANNAME_NOTIFY:
	case MCU_MCS_NETCAPSTATUS_NOTIFY: //抓包状态通报
	case MCU_MCS_OPENDEBUGMOED_NOTIFY:
		if(MCU_MCS_CALLMTFAILURE_NOTIF != pcMsg->event
			&& MCU_MCS_CONFALLMTINFO_NOTIF != pcMsg->event
			&& MCU_MCS_RECPROG_NOTIF != pcMsg->event
			&& MCU_MCS_PLAYPROG_NOTIF != pcMsg->event
			&& MCU_MCS_MTSTATUS_NOTIF != pcMsg->event)
		{
			MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		}else
		{
			MCS_EV_LOG(LOG_LVL_DETAIL);
		}
		ProcMcuMcsMsg(pcMsg, TRUE);
		break;

        //提示信息通知消息
    case MCU_MCS_ALARMINFO_NOTIF:

        // 用户消息由业务控制过滤，本处不过滤
    case MCU_MCS_ADDUSER_NOTIF:
    case MCU_MCS_CHANGEUSER_NOTIF:
    case MCU_MCS_USERGRP_NOTIF:
    case MCU_MCS_DELUSERGRP_NOTIF:
        
        // 地址簿消息都不过滤
    case MCU_MCS_ADDRBOOK_ADDENTRY_NOTIF:
    case MCU_MCS_ADDRBOOK_DELENTRY_NOTIF:
    case MCU_MCS_ADDRBOOK_MODIFYENTRY_NOTIF:
    case MCU_MCS_ADDRBOOK_ADDGROUP_NOTIF:
    case MCU_MCS_ADDRBOOK_DELGROUP_NOTIF:
    case MCU_MCS_ADDRBOOK_MODIFYGROUP_NOTIF:
    case MCU_MCS_ADDRBOOK_ADDENTRYTOGROUP_NOTIF:
    case MCU_MCS_ADDRBOOK_DELENTRYFROMGROUP_NOTIF:

        // 不带会议CConfId的请求

    ////3.2	会议管理。创会请求，NACK可能还没有CConfId生成
	case MCU_MCS_CREATECONF_ACK:  //会议控制台在MCU上创建一个会议成功应答  
	case MCU_MCS_CREATECONF_NACK: //会议控制台在MCU上创建一个会议失败
	case MCU_MCS_CREATECONF_BYTEMPLATE_ACK://会议模板成功地转变成即时会议
	case MCU_MCS_CREATECONF_BYTEMPLATE_NACK://会议模板转变成即时会议失败
    case MCU_MCS_CREATESCHCONF_BYTEMPLATE_ACK:
    case MCU_MCS_CREATESCHCONF_BYTEMPLATE_NACK:
    case MCU_MCS_CREATETEMPLATE_ACK:
    case MCU_MCS_CREATETEMPLATE_NACK:
	case MCU_MCS_SAVECONFTOTEMPLATE_ACK:
	case MCU_MCS_SAVECONFTOTEMPLATE_NACK:
        
	//3.1.1	建链    
	case MCU_MCS_CONNECT_ACK:  //MCU准入应答   
	case MCU_MCS_CONNECT_NACK: //MCU拒绝应答

    //3.1.2	时间同步  
	case MCU_MCS_CURRTIME_ACK:   //mcu成功应答   
	case MCU_MCS_CURRTIME_NACK:  //mcu拒绝应答
	
    //3.1.3	查询MCU状态
	case MCU_MCS_GETMCUSTATUS_ACK:       //返回的成功应答 
	case MCU_MCS_GETMCUSTATUS_NACK:      //返回的失败应答
	case MCU_MCS_MCUSTATUS_NOTIF:        //MCU状态通知
	case MCU_MCS_MCUEXTSTATUS_NOTIFY:	 //mcu额外状态通知	
	case MCU_MCS_MCUPERIEQPSTATUS_NOTIF: //外设状态通知

	case MCU_MCS_LISTALLCONF_ACK:   //应答MCS_MCU_LISTALLCONF_REQ 
	case MCU_MCS_LISTALLCONF_NACK:  //MCU上没有会议  

	case MCU_MCS_RECORDEREXCPT_NOTIF://消息描述：会议录放像异常通知
	case MCU_MCS_RECORDCHNSTATUS_NOTIF://新录播录像状态上报

    //3.6.1.3	查询及记录文件操作
    //3.6.1.3.1	查询录像机状态	
	case MCU_MCS_GETRECSTATUS_ACK://MCU查询录像机状态应答	
	case MCU_MCS_GETRECSTATUS_NACK://MCU查询录像机状态拒绝

    //3.6.1.3.2	记录列表请求
	case MCU_MCS_LISTALLRECORD_ACK:	   //查询记录应答
	case MCU_MCS_LISTALLRECORD_NACK:   //查询记录拒绝	

    //3.6.1.3.3	删除记录文件请求	
	case MCU_MCS_DELETERECORD_ACK:   //会控请求删除文件应答	
	case MCU_MCS_DELETERECORD_NACK:  //会控请求删除文件拒绝
        
    //3.6.1.3.3	删除记录文件请求	
	case MCU_MCS_RENAMERECORD_ACK:   //会控请求更改文件应答	
	case MCU_MCS_RENAMERECORD_NACK:  //会控请求更改文件拒绝

	//3.6.1.4 文件发布操作
	//3.6.1.4.1 会控请求MCU发布文件
	case MCU_MCS_PUBLISHREC_ACK:			//发布录像应答	
	case MCU_MCS_PUBLISHREC_NACK:			//发布录像拒绝

	//3.6.1.4.2 会控请求MCU取消发布文件
    case MCU_MCS_CANCELPUBLISHREC_ACK:     //取消发布应答
	case MCU_MCS_CANCELPUBLISHREC_NACK:    //取消发布拒绝
		
    //3.6.2	混音器操作	
	case MCU_MCS_GETMIXERSTATUS_ACK:       //查询混音器请求应答	
	case MCU_MCS_GETMIXERSTATUS_NACK:      //查询混音器请求拒绝

	case MCU_MCS_GETMCUPERIEQPSTATUS_ACK:  //MCU应答会议控制台的查询外设状态请求	
	case MCU_MCS_GETMCUPERIEQPSTATUS_NACK: //MCU拒会议控制台的查询外设状态请求

    //3.3.8	会控选看		
	case MCU_MCS_STARTSWITCHMC_ACK://MCU应答会议控制台选看终端	
	case MCU_MCS_STARTSWITCHMC_NACK://MCU拒绝会议控制台选看终端	
	case MCU_MCS_STARTPLAY_NOTIF://MCU给会控的开始选看通知

    //3.3.9	会控取消选看	
	case MCU_MCS_STOPSWITCHMC_ACK://MCU应答会议控制台选看终端	
	case MCU_MCS_STOPSWITCHMC_NACK://MCU拒绝会议控制台选看终端	
	case MCU_MCS_STOPPLAY_NOTIF://MCU给会控的停止选看通知
    
	// 4.6.1 改变hdu音量大小
	case MCU_MCS_CHANGEHDUVOLUME_ACK:     // 改变音量大小应答
	case MCU_MCS_CHANGEHDUVOLUME_NACK:    // 改变音量大小拒绝
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcuMcsMsg(pcMsg, FALSE);
		break;

    case MCU_MCS_DELUSER_NOTIF:
        // 删除用户特殊处理，如果是删除了自己，则和Mcs断链
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcuMcsMsg(pcMsg, FALSE);
        ProcMcuDelUser(pcMsg);
		break;


        //MCU用户管理操作
    case MCS_MCU_ADDUSER_REQ:           //会议控制台请求MCU添加用户
    case MCS_MCU_DELUSER_REQ:           //会议控制台请求MCU删除用户
    case MCS_MCU_GETUSERLIST_REQ:       //会议控制台请求MCU得到用户列表
    case MCS_MCU_CHANGEUSER_REQ:        //会议控制台请求MCU修改用户密码

    case MCS_MCU_ADDUSERGRP_REQ:        //会议控制台请求MCU添加用户组
    case MCS_MCU_CHANGEUSERGRP_REQ:     //会议控制台请求MCU修改用户组
    case MCS_MCU_DELUSERGRP_REQ:        //会议控制台请求MCU删除用户组
    case MCS_MCU_GETUSERGRP_REQ:        //会议控制台请求MCU获取用户组信息
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
        ProcUserManagerMsg( pcMsg );
        break;
        
        //MCU地址簿操作
    case MCS_MCU_ADDRBOOK_GETENTRYLIST_REQ:
    case MCS_MCU_ADDRBOOK_GETGROUPLIST_REQ:
    case MCS_MCU_ADDRBOOK_ADDENTRY_REQ:
    case MCS_MCU_ADDRBOOK_DELENTRY_REQ:
    case MCS_MCU_ADDRBOOK_MODIFYENTRY_REQ:
    case MCS_MCU_ADDRBOOK_ADDGROUP_REQ:
    case MCS_MCU_ADDRBOOK_DELGROUP_REQ:
    case MCS_MCU_ADDRBOOK_MODIFYGROUP_REQ:
    case MCS_MCU_ADDRBOOK_ADDENTRYTOGROUP_REQ:
    case MCS_MCU_ADDRBOOK_DELENTRYFROMGROUP_REQ:
    case MCU_ADDRBOOK_GETENTRYLIST_NOTIF:
    case MCU_ADDRBOOK_GETGROUPLIST_NOTIF:
        ProcAddrBookMsg( pcMsg );
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
        break;
        
    //mcu配置界面化消息
    case MCS_MCU_CPUPERCENTAGE_REQ:         //MCS请求mcu 的cpu占有率
    case MCS_MCU_CPUPERCENTAGE_CMD:    
    case MCS_MCU_CHANGEMCUEQPCFG_REQ:       //MCS请求修改mcu设备配置信息     
    case MCS_MCU_GETMCUEQPCFG_REQ:          //MCS请求mcu设备配置信息        
    case MCS_MCU_CHANGEBRDCFG_REQ:          //mcs修改单板配置信息        
    case MCS_MCU_GETBRDCFG_REQ:             //mcs 请求单板配置信息   
    case MCS_MCU_CHANGEMCUGENERALCFG_REQ:   //mcs修改mcu配置信息基本设置        
    case MCS_MCU_GETMCUGENERALCFG_REQ:      // mcs 请求mcu基本配置信息
    case MCS_MCU_GETVMPSCHEMEINFO_REQ:
    case MCS_MCU_REBOOTBRD_REQ:
//	case MCS_MCU_UPLOADFILENAME_REQ:
    case MCS_MCU_UPDATEBRDVERSION_CMD:
	case MCS_MCU_STARTUPDATEDSCVERSION_REQ:		// mcs请求更新DSC板版本, zgc, 2007-08-20
	case MCS_MCU_DSCUPDATEFILE_REQ:
	case MCS_MCU_UPDATETELNETLOGININFO_REQ:		//会议控制台请求MCU更新telnet登陆信息, zgc, 2007/10/19
        //远程重启
    case MCS_MCU_REBOOT_CMD:	
    case MCS_MCU_GETHDUSCHEMEINFO_REQ:      //获取预案
    case MCS_MCU_CHANGEHDUSCHEMEINFO_REQ:   //修改预案
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
        ProcMcuCfgMsg( pcMsg );
        break;
    case MCU_MCS_DOWNLOADCFG_REQ:           // MCS 请求上下载配置文件
    case MCU_MCS_UPLOADCFG_REQ:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
        ProcMcuCfgFileMsg( pcMsg );
        break;

    //响应消息    
    case MCU_MCS_CPUPERCENTAGE_ACK:
    case MCU_MCS_CPUPERCENTAGE_NACK:
    case MCU_MCS_CPUPERCENTAGE_NOTIF:
    case MCU_MCS_CHANGEMCUEQPCFG_ACK:
    case MCU_MCS_CHANGEMCUEQPCFG_NACK:
    case MCU_MCS_GETMCUEQPCFG_ACK:
    case MCU_MCS_GETMCUEQPCFG_NACK:
    case MCU_MCS_MCUEQPCFG_NOTIF:
    case MCU_MCS_CHANGEBRDCFG_ACK:
    case MCU_MCS_CHANGEBRDCFG_NACK:
    case MCU_MCS_GETBRDCFG_ACK:
    case MCU_MCS_GETBRDCFG_NACK:
    case MCU_MCS_BRDCFG_NOTIF:
    case MCU_MCS_CHANGEMCUGENERALCFG_ACK:
    case MCU_MCS_CHANGEMCUGENERALCFG_NACK:
    case MCU_MCS_GETMCUGENERALCFG_ACK:
    case MCU_MCS_GETMCUGENERALCFG_NACK:
    case MCU_MCS_MCUGENERALCFG_NOTIF:
    case MCU_MCS_REBOOTBRD_ACK:
    case MCU_MCS_REBOOTBRD_NACK:
    case MCU_MCS_GETVMPSCHEMEINFO_ACK:
    case MCU_MCS_GETVMPSCHEMEINFO_NACK:
    case MCU_MCS_BOARDSTATUS_NOTIFY:
    case MCU_MCS_UPDATEBRDVERSION_NOTIF:
//	case MCU_MCS_UPLOADFILENAME_ACK:
//	case MCU_MCS_UPLOADFILENAME_NACK:
	case MCU_MCS_NEWDSCREGREQ_NOTIFY:  //zgc [2007/03/06] 通知MCS提醒用户有新的DSC板发起注册

	case MCU_MCS_STARTUPDATEDSCVERSION_ACK:		//开始DSC升级成功回应
	case MCU_MCS_STARTUPDATEDSCVERSION_NACK:	//开始DSC升级失败回应
	case MCU_MCS_DSCUPDATEFILE_ACK:				//文件包接收成功回应
	case MCU_MCS_DSCUPDATEFILE_NACK:			//文件包接收失败回应

	case MCU_MCS_UPDATETELNETLOGININFO_ACK:		//更新telnet登陆成功, zgc, 2007/10/19
	case MCU_MCS_UPDATETELNETLOGININFO_NACK:	//更新telnet登陆失败, zgc, 2007/10/19

	case MCU_MCS_GETHDUSCHEMEINFO_ACK:
	case MCU_MCS_GETHDUSCHEMEINFO_NACK:
	case MCU_MCS_GETCRIMAC_ACK:
	case MCU_MCS_GETCRIMAC_NACK:
	case MCU_MCS_GETCRIMAC_NOTIF:
        MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcuCfgRsp(pcMsg);
        break;

    //N+1备份消息
    case MCS_MCU_NPLUSROLLBACK_REQ:
    case MCS_MCU_NPLUSRESET_CMD:
		MCS_EV_LOG(LOG_LVL_DETAIL);
        ProcMcsNPlusReq(pcMsg);
        break;

    case MCU_MCS_NPLUSROLLBACK_ACK:
    case MCU_MCS_NPLUSROLLBACK_NACK:
		MCS_EV_LOG(LOG_LVL_DETAIL);
        ProcNPlusMcsRsp(pcMsg);
        break;
            
	//单元测试消息: (1) 主控热备份
	case EV_TEST_TEMPLATEINFO_GET_ACK:
	case EV_TEST_CONFINFO_GET_ACK:
	case EV_TEST_CONFMTLIST_GET_ACK:
	case EV_TEST_ADDRBOOK_GET_ACK:
	case EV_TEST_MCUCONFIG_GET_ACK:
		MCS_EV_LOG(LOG_LVL_DETAIL);
		ProcMcuUnitTestMsg(pcMsg);
		break;
		
	// [6/28/2011 liuxu] 界面通知刷新log
	case MCS_MCU_FLUSHLOG_CMD:
		mculogflush();
		break;
#if defined(_8KH_) ||  defined(_8KE_) || defined(_8KI_)
	//抓包相关
	case MCS_MCU_STARTNETCAP_CMD:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsMcuStartNetCapCMD(pcMsg);
		break;
	case MCS_MCU_STOPNETCAP_CMD:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsMcuStoptNetCapCmd(pcMsg);
		break;
	case MCS_MCU_GETNETCAPSTATUS_REQ:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsMcuGetNetCapStatusReq(pcMsg);
		break;
	case MCS_MCU_OPENDEBUGMODE_CMD:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsMcuOpenDebugModeCmd(pcMsg);
		break;
	case MCS_MCU_STOPDEBUGMODE_CMD:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsMcuStopDebugModeCmd(pcMsg);
		break;
	case MCS_MCU_GETDEBUGMODE_REQ:
		MCS_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcsMcuGetDebugModeReq(pcMsg);
		break;
#endif
	//转发
	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[InstanceEntry]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
}

/*====================================================================
    函数名      ：DaemonInstanceEntry
    功能        ：实例消息处理入口函数，必须override
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
				  CApp* pcApp
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/29    1.0         
====================================================================*/
void CMcsSsnInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "CMcsSsnInst: The received msg's pointer in the msg entry is NULL!");
		return;
	}
	
	switch( pcMsg->event )
	{
    case OSP_POWERON:
        DaemonProcPowerOn();
        break;
	case MCU_APPTASKTEST_REQ:			//GUARD Probe Message
		DaemonProcAppTaskRequest(pcMsg);
		break;
	case TEST_MCU_TESTUSERINFO_CMD:
		DaemonProcIntTestUserInfo(pcMsg);
		break;
        //N+1注册成功，需要同步用户组和用户信息
    case NPLUS_VC_DATAUPDATE_NOTIF:
        DaemonProcNPlusUsrGrpNotif( pcMsg );
        break;
	case MCU_ADDRINITIALIZED_NOTIFY:
		DaemonProcAddrInitialized(pcMsg);
		break;

	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[DaemonInstanceEntry]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u appId.%u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode,pcApp!=NULL?pcApp->appId:0);
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuConnectReq
    功能        ：控制台通知MCU请求准入处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
void CMcsSsnInst::ProcMcsMcuConnectReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMcsRegInfo	tRegReq = *( TMcsRegInfo * )cServMsg.GetMsgBody();
    // guzh [10/11/2007] 手动解密（网络传输过来的是加密的内容）
    tRegReq.DecPwd();

    //[4/9/2013 liaokang] 编码方式
	/*lint -save -esym(550,byMcsEncoding)*/
    u8 byMcsEncoding = emenCoding_GBK;
    /*lint -restore*/
    if( cServMsg.GetMsgBodyLen() >= (sizeof(TMcsRegInfo) + sizeof(u8) + sizeof(TMcsRegInfo)) )
    {
        byMcsEncoding = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMcsRegInfo) );
#ifdef _UTF8
        if( emenCoding_Utf8 == byMcsEncoding )
        {
            memset( &tRegReq, 0, sizeof(TMcsRegInfo) );
            tRegReq.SetMcuMcsVer();
            tRegReq = *( TMcsRegInfo * )(cServMsg.GetMsgBody() + sizeof(TMcsRegInfo) + sizeof(u8));
            tRegReq.DecPwd();
        }
#endif
    }
   
	u8 byInstId = ( u8 )GetInsID();
	CLoginRequest LoginRequest;
	
	u32 dwSysSSrc = g_cMSSsnApp.GetMSSsrc();
    
	switch( CurState() )
	{
	case STATE_IDLE:
        {
		    //保存信息
		    m_dwMcsNode = pcMsg->srcnode;	
		    m_dwMcsIId  = pcMsg->srcid;
			m_dwMcsIp = tRegReq.m_dwMcsIpAddr;

            emMCUMSState emLocalMSState = MCU_MSSTATE_ACTIVE;
		    emLocalMSState  = g_cMSSsnApp.GetCurMSState();                
		    if (MCU_MSSTATE_OFFLINE == emLocalMSState)
		    {
			    //MCU尚未初始化完毕，拒绝，断开连接
				cServMsg.SetErrorCode( ERR_MCU_INITIALIZING );
			    SendMsgToMcs( pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );	
                OspDelay(100);
                
			    ::OspDisconnectTcpNode( m_dwMcsNode );
			    m_dwMcsNode = INVALID_NODE;	            
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[Mcsssn][ProcMcsMcuConnectReq]: invalid ms state, NACK.\n");

			    return;
		    }           
 
			// 对于VCS接入需判断其是否授权

			u16 wInstID = GetInsID();
#ifndef _NOVCSCHECK_
			if (wInstID > MAXNUM_MCU_MC && wInstID <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC)
				&& g_cMcuVcApp.IsVCSNumOverLicense())
			{
                cServMsg.SetErrorCode(ERR_MCU_VCSMCS_LOGVCSNUMOVER);
                SendMsgToMcs( pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );		
                
                OspDelay(100);
                
                ::OspDisconnectTcpNode( m_dwMcsNode );
			    m_dwMcsNode = INVALID_NODE;	            
                LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[Mcsssn][ProcMcsMcuConnectReq]: Over VCS access ability\n");
				return;
			}
#endif
			// MCS和VCS登陆互斥
// 			u8  byEnableLog = TRUE;
// 			if (wInstID > 0 && wInstID <= MAXNUM_MCU_MC)
// 			{
// 				for (u16 wInstIdx = MAXNUM_MCU_MC + 1; wInstIdx <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); wInstIdx++)
// 				{
// 					if (g_cMcuVcApp.IsMcConnected(wInstIdx))
// 					{
// 						byEnableLog = FALSE;
// 						break;
// 					}
// 				}
// 			}
// 			else
// 			{
// 				for (u16 wInstIdx = 1; wInstIdx <= MAXNUM_MCU_MC; wInstIdx++)
// 				{
// 					if (g_cMcuVcApp.IsMcConnected(wInstIdx))
// 					{
// 						byEnableLog = FALSE;
// 						break;
// 					}
// 				}
// 
// 			}
// 
// 			if(!byEnableLog)
// 			{
//                 cServMsg.SetErrorCode(ERR_MCU_VCSMCS_LOGONLYONE);
//                 SendMsgToMcs( pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );		
//                 
//                 OspDelay(100);
//                 
//                 ::OspDisconnectTcpNode( m_dwMcsNode );
// 			    m_dwMcsNode = INVALID_NODE;	            
//                 LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[Mcsssn][ProcMcsMcuConnectReq]: MCS VCS login only one\n");
// 				return;
// 			}
			
            //校验版本
            if ( tRegReq.GetMcuMcsVer() != MCUMCS_VER
#ifdef _UTF8
                || emenCoding_GBK == byMcsEncoding    //[5/10/2013 liaokang] 支持多国语言  MCU以UTF-8方式编码，不再兼容老MCS
#endif
                )
            {
                cServMsg.SetErrorCode(ERR_MCU_VER_UNMATCH);
                SendMsgToMcs( pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );		
                
                OspDelay(100);
                
                ::OspDisconnectTcpNode( m_dwMcsNode );
			    m_dwMcsNode = INVALID_NODE;	            
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcMcsMcuConnectReq]: mcs version(%d) differs from mcu(%d), NACK.\n",
                          tRegReq.GetMcuMcsVer(), MCUMCS_VER);

                return;
            }		    
       
		    LoginRequest.SetName( (char *)tRegReq.GetUser() );
		    LoginRequest.SetPassword( (char *)tRegReq.GetPwd() ); 

			// 1－16 17－32采取用户帐户分开管理
            u8 byGrpId = USRGRPID_INVALID;
			BOOL32 dwValid = FALSE;
			u16 wErrCode = 0;
			if (wInstID > 0 && wInstID <= MAXNUM_MCU_MC)
			{
 				dwValid  = CMcsSsn::CheckLoginValid(g_cUsrManage, &LoginRequest, byGrpId);
				wErrCode = g_cUsrManage.GetLastError(); 
			}
			else
			{
				dwValid  = CMcsSsn::CheckLoginValid(g_cVCSUsrManage, &LoginRequest, byGrpId);
				wErrCode = g_cVCSUsrManage.GetLastError();
			}

		    if (!dwValid)
		    {
			    //拒绝
			    cServMsg.SetErrorCode( wErrCode );
			    SendMsgToMcs( pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );		
			    
                // 等待Osp发送消息，否则消息发送不了
                OspDelay(100);

			    //断开连接                
			    ::OspDisconnectTcpNode( m_dwMcsNode );
			    m_dwMcsNode = INVALID_NODE;
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcMcsMcuConnectReq]: user name/pwd invalid, NACK.\n");

			    return;
		    }

            // 记录下用户名
		    memcpy( m_achUser, tRegReq.GetUser(), MAXLEN_PWD );
		    m_achUser[ MAXLEN_PWD -1 ] = 0;

			CMcsSsn::SetUserGroup(GetInsID(), byGrpId);
            
            LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuConnectReq]: User Logged in, Name: %s, Group Id: %d.\n", 
                    tRegReq.GetUser(), 
                    byGrpId );

        
		    //注册断链接收实例
		    ::OspNodeDiscCBRegQ( m_dwMcsNode, GetAppID(), GetInsID() );
		    
		    //通知 daemon
		    cServMsg.SetMsgBody( ( u8 * )&byInstId, sizeof( byInstId ) );
		    cServMsg.CatMsgBody( ( u8 * )&tRegReq, sizeof( tRegReq ) );
		    g_cMcuVcApp.SendMsgToDaemonConf( MCU_MCSCONNECTED_NOTIF, cServMsg.GetServMsg(), 
			    cServMsg.GetServMsgLen() );

		    //应答
            cServMsg.SetSrcSsnId( ( u8 )GetInsID() );
		    cServMsg.SetMcuId( ( u8 )LOCAL_MCUID );

            TMcsRegRsp tRsp;
            tRsp.SetMcu( GetMcuPdtType(), LOCAL_MCUID );
		    
            TMtAlias tMcuAlias;
	        s8   achMcuAlias[32];
	        g_cMcuAgent.GetMcuAlias( achMcuAlias, sizeof(achMcuAlias) );
            u32 dwLocalIp = g_cMcuAgent.GetMpcIp();
		    tMcuAlias.SetH323Alias( achMcuAlias );
            tMcuAlias.m_tTransportAddr.SetNetSeqIpAddr(dwLocalIp);
            tRsp.SetMcuAlias(tMcuAlias);

            u32  dwAnotherMcuIP = 0;
            if (g_cMSSsnApp.IsDoubleLink())
            {
                dwAnotherMcuIP = g_cMSSsnApp.GetAnotherMcuIP();
	        }
            tRsp.SetRemoteMpcIp(dwAnotherMcuIP);

			BOOL32 bIsActive = FALSE;
			if(MCU_MSSTATE_ACTIVE == emLocalMSState)
			{
				bIsActive = TRUE;
			}
            tRsp.SetLocalActive(bIsActive);
            tRsp.SetUsrGrpId(byGrpId);

            u8 byOsType = 0;
        #ifdef WIN32
            byOsType = OS_TYPE_WIN32;
        #elif defined(_VXWORKS_)
            byOsType = OS_TYPE_VXWORKS;
        #else   
            byOsType = OS_TYPE_LINUX;
        #endif
            tRsp.SetMcuOsType(byOsType);
			
			tRsp.SetSysSSrc(dwSysSSrc);

		    cServMsg.SetMsgBody( (u8*)&tRsp, sizeof(tRsp) );

	    #ifdef _MINIMCU_
            TMcu8kbPfmFilter tFilter = CMcuPfmLmt::GetFilter();
			cServMsg.CatMsgBody( (u8*)&tFilter, sizeof(TMcu8kbPfmFilter) );
	    #endif
			
			//增加MCU版本号信息[12/31/2011 chendaiwei]
			u8 bySupportHP = 1;
			cServMsg.CatMsgBody(&bySupportHP,sizeof(u8));

            LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[Mcsssn][ProcMcsMcuConnectReq]: Mcs Connect Ack: \n");
            if (g_bPrintMcsMsg)
            {                
                tRsp.Print();
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MCS, "[Mcsssn] support HP: %d\n",bySupportHP);
            }

            //[4/8/2013 liaokang] 支持多国语言  MCU以UTF-8方式编码
#ifdef _UTF8
            u8 byCodingForm = emenCoding_Utf8;
            cServMsg.CatMsgBody(&byCodingForm,sizeof(u8));
#endif

		    SendMsgToMcs( pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );
                           
		    NEXTSTATE( STATE_NORMAL );
        }
		break;

	default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcsMcuConnectReq]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
}


/*=============================================================================
函 数 名： ProcMcsMcuGetMsStatusReq
功    能： 获取mcu主备状态
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/2/22  4.0			许世林                  创建
=============================================================================*/
void CMcsSsnInst::ProcMcsMcuGetMsStatusReq( const CMessage * pcMsg )
{
    // guzh [4/11/2007] 如果没有通过注册就发送获取请求，很有可能是重新启动前的连接，不理睬
    if ( CurState() == STATE_IDLE )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcMcsMcuGetMsStatusReq]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        
        return;
    }
    
    if (MCU_MSSTATE_EXCHANGE_NTF == pcMsg->event)
    {
        // 如果同步失败,断开相应的连接
        u8 byIsSwitchOk = *pcMsg->content;
        if (0 == byIsSwitchOk)
        {
            OspDisconnectTcpNode( m_dwMcsNode );
            return;
        }        
    }
    
    if( MCU_MSSTATE_ACTIVE == g_cMSSsnApp.GetCurMSState() )
    {
        TMcuMsStatus tMsStatus;
        tMsStatus.SetMsSwitchOK(g_cMSSsnApp.IsMsSwitchOK());
    
        CServMsg cServMsg(pcMsg->content, pcMsg->length);
        cServMsg.SetMsgBody((u8*)&tMsStatus, sizeof(tMsStatus));
        SendMsgToMcs(MCU_MCS_GETMSSTATUS_ACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

        LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuGetMsStatusReq] IsMsSwitchOK :%d.\n", tMsStatus.IsMsSwitchOK());
    }
        
    return;
}

/*====================================================================
    函数名      ：ProcMcsInquiryMcuTime
    功能        ：会议控制台查询mcu当前时间处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/02/08    1.0         秦重军         创建
====================================================================*/
void CMcsSsnInst::ProcMcsInquiryMcuTime( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TKdvTime tCurMcuTime;
	
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[Mcsssn]: Message %u(%s) received, Current State %d!\n",
			                pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());

	switch( CurState() )
	{
	case STATE_NORMAL ://连接建立
		time_t tMcuTime;
		::time( &tMcuTime );
		tCurMcuTime.SetTime( &tMcuTime ); 

		cServMsg.SetSrcSsnId( ( u8 )GetInsID() );
		cServMsg.SetMsgBody( (u8 *)&tCurMcuTime, sizeof( tCurMcuTime ) );
		SendMsgToMcs( pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		break;
	default :
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcMcsInquiryMcuTime]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}

	return;
}

/*=============================================================================
  函 数 名： ProcMcsMcuChgSysTime
  功    能： 控制台修改mcu系统时间
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
=============================================================================*/
void CMcsSsnInst::ProcMcsMcuChgSysTime( const CMessage * pcMsg)
{
    CServMsg cMsg(pcMsg->content, pcMsg->length);
    if (sizeof(TKdvTime) != cMsg.GetMsgBodyLen())
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcMcsMcuChgSysTime] msg body is invalid! len: %d\n", cMsg.GetMsgBodyLen());
        return;
    }
    
    // xsl [10/25/2006] 若license设置了截止日期，不允许时间同步
    if (g_cMcuVcApp.GetMcuExpireDate().GetYear() != 0)
    {
        cMsg.SetMsgBody(NULL, 0);
		cMsg.SetSrcSsnId( ( u8 )GetInsID() );
        cMsg.SetErrorCode(ERR_MCU_OPERATION_NOPERMIT);
		SendMsgToMcs(pcMsg->event + 2, cMsg.GetServMsg(), cMsg.GetServMsgLen());
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "can't syn time for mcu expire date.\n");
        return;
    }

    TKdvTime *ptTime = (TKdvTime *)cMsg.GetMsgBody();
    time_t tTime;
    ptTime->GetTime(tTime);
    u16 wRet = g_cMcuAgent.SetSystemTime(tTime);
    if (SUCCESS_AGENT != wRet)
    {	
		cMsg.SetMsgBody(NULL, 0);
		cMsg.SetSrcSsnId( ( u8 )GetInsID() );
		SendMsgToMcs(pcMsg->event + 2, cMsg.GetServMsg(), cMsg.GetServMsgLen());
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcMcsMcuChgSysTime] change system time failed, ret: %d\n", wRet);        
    }
    else
    {		
		TKdvTime tCurMcuTime;
		time_t tMcuTime;
		::time( &tMcuTime );
		tCurMcuTime.SetTime( &tMcuTime );         
    
		cMsg.SetSrcSsnId( ( u8 )GetInsID() );
		cMsg.SetMsgBody( (u8 *)&tCurMcuTime, sizeof( tCurMcuTime ) );
		SendMsgToMcs( pcMsg->event + 1, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
    }

    return;
}

/*====================================================================
    函数名      ：ProcMcsDisconnect
    功能        ：控制台断链通知处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
void CMcsSsnInst::ProcMcsDisconnect( const CMessage * pcMsg )
{
    if (STATE_NORMAL != CurState())
    {
        return;
    }

    //LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[Mcsssn%u]: Message %u(%s) received!\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));

	u8 byInstId = ( u8 )GetInsID();
    
	CServMsg cServMsg;
	if ( *(u32*)pcMsg->content == m_dwMcsNode )	//本实例对应连接断
	{		
        if (INVALID_NODE != m_dwMcsNode)
        {
            OspDisconnectTcpNode(m_dwMcsNode);
        }
        
		//send notification to VC Daemon
		cServMsg.SetMsgBody( ( u8 * )&byInstId, sizeof( byInstId ) );
        cServMsg.SetEventId( MCU_MCSDISCONNECTED_NOTIF );
		g_cMcuVcApp.SendMsgToDaemonConf( cServMsg.GetEventId(), 
                                         cServMsg.GetServMsg(), 
				                         cServMsg.GetServMsgLen() );
		//clear
		m_dwMcsNode = INVALID_NODE;

        // 清除用户组信息
        CMcsSsn::SetUserGroup( GetInsID(), USRGRPID_INVALID );
		cServMsg.SetSrcSsnId(byInstId);
		post( MAKEIID( AID_MCU_CONFIG, 1 ), MCSSSN_MCUCFG_MCSDISSCONNNECT_NOTIFY, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

		NEXTSTATE( STATE_IDLE );
	}	

	return;
}

/*====================================================================
    函数名      ：SendMsgToMc
    功能        ：发消息给本实例对应的会议控制台
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：成功返回TRUE，若失败返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/28    1.0         LI Yi         创建
====================================================================*/
BOOL32 CMcsSsnInst::SendMsgToMcs( u16 wEvent, u8 * const pbyMsg, u16 wLen, BOOL32 bMsFilter )
{
    if (bMsFilter && !g_cMSSsnApp.JudgeSndMsgPass())
    {
        return TRUE;
    }

//    CServMsg cMsg(pbyMsg, wLen);
//    LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[SendMsgToMcs] confid :");
//    cMsg.GetConfId().Print();
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[SendMsgToMcs] Event %u(%s) To Console %u\n", wEvent, OspEventDesc(wEvent), GetInsID() );
    
	if (OspIsValidTcpNode(m_dwMcsNode))
	{
		post( m_dwMcsIId, wEvent, pbyMsg, wLen, m_dwMcsNode );
		return( TRUE );
	}
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "CMcsSsnInst: ABNORMAL McSsn: Node=%u(IP: 0x%x), CurState=%u, InstId=%d\n", 
			                     m_dwMcsNode, OspNodeIpGet(m_dwMcsNode), CurState(), GetInsID() );
		return( FALSE );
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuMsg
    功能        ：会议控制台发来消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/03/21    1.0         Qzj           创建
====================================================================*/
void CMcsSsnInst::ProcMcsMcuMsg(const CMessage * pcMsg)
{
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
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcsMcuMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        break;
    }
}


/*====================================================================
    函数名      ：ProcMcuMcsResponse
    功能        ：所有会议控制台命令应答消息统一处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/01    1.0         LI Yi         创建
====================================================================*/
void CMcsSsnInst::ProcMcuMcsMsg( const CMessage * pcMsg, BOOL32 bCheckGrp )
{
	CServMsg cServMsg(pcMsg->content,pcMsg->length);

    BOOL32 bSend = TRUE;
    u8 byGrpId = CMcsSsn::GetUserGroup( GetInsID() );

	switch( CurState() )
	{
	case STATE_NORMAL:
        
        if ( bCheckGrp )
        {
            // 要检查组权限，如果是自己组或者是超级管理员，或者非组消息则通过            
            if ( byGrpId == USRGRPID_SADMIN || 
                 byGrpId == cServMsg.GetConfId().GetUsrGrpId() ||
                 cServMsg.GetConfId().IsNull() ||
                 cServMsg.GetConfId().GetUsrGrpId() == 0 )
            {
                bSend = TRUE;
            }
            else
            {
                bSend = FALSE;
            }
        }
        if (bSend)
        {
            SendMsgToMcs( pcMsg->event, pcMsg->content, pcMsg->length );
        }
        else
        {
            LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcuMcsMsg]: Message %u(%s) In InsID.%u is Dropped for CConf Group Id(%d) <--> Ssn Group Id(%d)\n",
                    pcMsg->event, ::OspEventDesc( pcMsg->event ), GetInsID(), 
                    cServMsg.GetConfId().GetUsrGrpId(), byGrpId);
        }
		
		break;
	default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcuMcsMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuDaemonConfMsg
    功能        ：控制台发来的给业务Daemon实例消息处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
void CMcsSsnInst::ProcMcsMcuDaemonConfMsg( const CMessage * pcMsg )
{
	CServMsg	cServMsg(pcMsg->content,pcMsg->length);
    
    // xsl [11/28/2006] n+1备份模式不允许创建模板
    if (pcMsg->event == MCS_MCU_CREATECONF_BYTEMPLATE_REQ ||
        pcMsg->event == MCS_MCU_CREATESCHCONF_BYTEMPLATE_REQ ||
        pcMsg->event == MCS_MCU_CREATETEMPLATE_REQ ||
        pcMsg->event == MCS_MCU_SAVECONFTOTEMPLATE_REQ)
    {
        if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_IDLE ||
            g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_SWITCH)
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcMcsMcuDaemonConfMsg] mcs can't create template in n+1 mode!\n");
            cServMsg.SetErrorCode(ERR_MCU_NPLUS_CREATETEMPLATE);
            SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
            return;
        }
    }    

	switch( CurState() )
	{
	case STATE_NORMAL:
	    cServMsg.SetMcuId( LOCAL_MCUID );
		cServMsg.SetSrcSsnId( ( u8 )GetInsID() );
		cServMsg.SetSrcMtId( 0 );
		cServMsg.SetEventId( pcMsg->event );
		if (pcMsg->event == MCS_MCU_LISTALLRECORD_REQ && !cServMsg.GetConfId().IsNull())
		{
			g_cMcuVcApp.SendMsgToConf(cServMsg.GetConfId(), pcMsg->event,
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
		else
		{
			g_cMcuVcApp.SendMsgToDaemonConf( cServMsg.GetEventId(), cServMsg.GetServMsg(), 
			                             cServMsg.GetServMsgLen() );
		}
		break;
	default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcsMcuDaemonConfMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
}

/*====================================================================
    函数名      ：ProcReoOverFlow
    功能        ：会议实例满创建失败处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/01    1.0         LI Yi         创建
====================================================================*/
void CMcsSsnInst::ProcReoOverFlow( const CMessage * pcMsg )
{
	CServMsg cServMsg;

	LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[Mcsssn]: Message %u(%s) received, Conference instance full!\n",
			                    pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch( CurState() )
	{
	case STATE_NORMAL:
		cServMsg.SetServMsg( pcMsg->content, pcMsg->length );
		cServMsg.SetErrorCode( ERR_MCU_CONFOVERFLOW );
		SendMsgToMcs( cServMsg.GetEventId() + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );
		break;
	default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcReoOverFlow]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
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
    03/05/29    1.0         
====================================================================*/
void CMcsSsnInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );
}

/*====================================================================
    函数名      ：DaemonProcPowerOn
    功能        ：处理上电后启动工作
    算法实现    ：加载地址簿和用户管理模块
    引用全局变量：
    输入参数说明：const CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/29    1.0         
====================================================================*/
void CMcsSsnInst::DaemonProcPowerOn( void )
{
	//启动地址薄服务  
// 	g_pcAddrBook = new CAddrBook(MAX_ADDRENTRY, MAX_ADDRGROUP);
// 	if(NULL == g_pcAddrBook)
// 	{
// 		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "MCU: memory allocation for address book failed!\n");
// 	}
// 	else
// 	{
// 		if(!g_pcAddrBook->LoadAddrBook(GetAddrBookPath()))
// 		{
// 			printf("MCU: LoadAddrBook() from %s failed!\n", GetAddrBookPath());
// 		}
// 	}

	//用户列表模块重新从文件装载用户列表
	g_cMcuVcApp.ReloadMcuUserList();

	// 初始化普通实例的状态
	for (u16 wInsID = 1; wInsID <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); wInsID++)
	{
		post( MAKEIID(GetAppID(), wInsID), OSP_POWERON );
	}

    
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMcsSsn::CMcsSsn() 
{
    memset( m_abyUsrGrp, USRGRPID_INVALID, sizeof(m_abyUsrGrp));
	m_byCurrendDebugMode = (u8)emDebugModeNone;
	m_byCurrendDebugInsId = 0;
}

CMcsSsn::~CMcsSsn()
{

}

/*====================================================================
    函数名      ：BroadcastToAllMcsSsn
    功能        ：发消息给所有已经连接的会议控制台对应的会话实例
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
	08/11/21                付秀华        对于广播型消息发给所有与界面通信的APP
====================================================================*/
void CMcsSsn::BroadcastToAllMcsSsn( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//本端主用则允许向外部各App投递消息，否则丢弃
	if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
	{
		return;
	}
	
	// CInstance::EACH 发送给所有的非空闲实例
	::OspPost(MAKEIID( AID_MCU_MCSSN, CInstance::EACH ), wEvent, pbyMsg, wLen);
	::OspPost(MAKEIID( AID_MCU_VCSSN, CInstance::EACH ), wEvent, pbyMsg, wLen);
/* // for test [pengguofeng 7/8/2013]
	CServMsg cMsg(pbyMsg, wLen);
	TMtExtU atMtExtU[200];
	memset(atMtExtU, 0, sizeof(atMtExtU));
	
	u8 byMtNum = 0; 
	TMcu tMcu;
	tMcu.SetNull();

	if ( wEvent == MCU_MCS_GETMTLIST_NOTIF 
		)
	{
		
		{
			if ( HandleMtListNtfUtf8(cMsg, atMtExtU, byMtNum, tMcu) )
				StaticLog("解析成功\n");
			else
				StaticLog("解析失败\n");
		}
	}

	if ( wEvent == MCU_MCS_MTLIST_NOTIF )
	{
		HandleMtListNtf(cMsg, atMtExtU, byMtNum, tMcu);
	}
*/
	return;
}

/*====================================================================
    函数名      ：SendMsgToMcsSsn
    功能        ：发消息给指定的会议控制台对应的会话实例
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wInstId, 对应的实例号
				  u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：成功返回TRUE，若该下级终端未登记返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/28    1.0         LI Yi         创建
	08/11/21                付秀华        通过实例号范围区分两类不同的负责界面通信
	                                      的APP(g_cMcsSsnApp, g_cVcsSsnApp)
====================================================================*/
BOOL32 CMcsSsn::SendMsgToMcsSsn( u16 wInstId, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	// g_cMcsSsnApp 的1－16的实例实际有效，g_cVcsSsnApp 的16－32的实例实际有效
	if (wInstId > 0 && wInstId <= MAXNUM_MCU_MC)
	{
		::OspPost(MAKEIID( AID_MCU_MCSSN, wInstId ), wEvent, pbyMsg, wLen);
	}
	else if (wInstId > MAXNUM_MCU_MC && wInstId <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC))
	{
		::OspPost(MAKEIID( AID_MCU_VCSSN, wInstId), wEvent, pbyMsg, wLen);
	}
	else if (CInstance::DAEMON == wInstId)
	{
		::OspPost(MAKEIID( AID_MCU_VCSSN, wInstId), wEvent, pbyMsg, wLen);
	}
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "CMcsSsn: wrong instance id %u!\n", wInstId);
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
    函数名      ：Broadcast2SpecGrpMcsSsn
    功能        ：发消息给同用户组的会议控制台对应的会话实例
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
====================================================================*/
void CMcsSsn::Broadcast2SpecGrpMcsSsn( u8 byGrdId, u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
	//本端主用则允许向外部各App投递消息，否则丢弃
	if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
	{
		return;
	}

    u8 bySsnGrpId = 0;
	for (u8 byInstID = 1; byInstID <= MAXNUM_MCU_MC; byInstID++)
	{
        bySsnGrpId = CMcsSsn::GetUserGroup( byInstID );
        if ( bySsnGrpId != USRGRPID_INVALID )
        {
            if ( bySsnGrpId == byGrdId ||
                 bySsnGrpId == USRGRPID_SADMIN )
            {
                // 如果是特定组或者超级用户组
                ::OspPost(MAKEIID( AID_MCU_MCSSN, byInstID ), wEvent, pbyMsg, wLen);
            }
            else
            {
            
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[Broadcast2SpecGrpMcsSsn]: Message %u(%s) In InsID.%u is Dropped for Spec Group Id(%d) <--> Ssn Group Id(%d)\n",
                        wEvent, ::OspEventDesc( wEvent ), byInstID, 
                        byGrdId, bySsnGrpId);
            }
        }
	}

	return;
}

/*====================================================================
    函数名      ：SetCurrentDebugMode
    功能        ：设置当前调试模式
    算法实现    ：
    引用全局变量：
    输入参数说明：emDebugMode emMode
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013/10/18  4.7.3       周晶晶         创建
====================================================================*/
void CMcsSsn::SetCurrentDebugMode( emDebugMode emMode )
{
	m_byCurrendDebugMode = (u8)emMode;
}

/*====================================================================
    函数名      ：GetCurrentDebugMode
    功能        ：获取当前调试模式
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：emDebugMode
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013/10/18  4.7.3       周晶晶         创建
====================================================================*/
emDebugMode CMcsSsn::GetCurrentDebugMode( void )
{
	return (emDebugMode)m_byCurrendDebugMode;
}

/*====================================================================
    函数名      ：SetCurrentDebugModeInsId
    功能        ：设置开启调试模式的mcs
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byInsId 
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013/10/18  4.7.3       周晶晶         创建
====================================================================*/
void CMcsSsn::SetCurrentDebugModeInsId( u8 byInsId )
{
	m_byCurrendDebugInsId = byInsId;
}

/*====================================================================
    函数名      ：GetCurrentDebugModeInsId
    功能        ：获取开启调试模式mcs实例id
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：u8
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013/10/18  4.7.3       周晶晶         创建
====================================================================*/
u8 CMcsSsn::GetCurrentDebugModeInsId( void )
{
	return m_byCurrendDebugInsId;
}

// 设置McSsn的用户组
void CMcsSsn::SetUserGroup( u16 byInsId, u8 byGrpId ) 
{
    if ( byInsId < 1 || byInsId > MAXNUM_MCU_MC + MAXNUM_MCU_VC )
        return;

    m_abyUsrGrp[byInsId - 1] = byGrpId;
}

// 获取McSsn的用户组
u8 CMcsSsn::GetUserGroup( u16 byInsId )
{
    if ( byInsId < 1 || byInsId > MAXNUM_MCU_MC + MAXNUM_MCU_VC )
    {
        return USRGRPID_INVALID;
    }
    else
    {
        return m_abyUsrGrp[byInsId - 1];
    }
}

// 检验登陆
BOOL32 CMcsSsn::CheckLoginValid(CUsrManage& cUsrManageObj, CLoginRequest *pcUsr, u8 &byUserRole)
{
    //若为N+1备份工作模式（即切换后），手动检查登陆
    //否则调用用户管理模块
    BOOL32 bValid = FALSE;
    byUserRole = USRGRPID_INVALID;
    CNPlusInst *pcInst = NULL;
    u8 byMaxUsrNum = 0;
    u8 byUsrNum = 0;

    if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
    {
        u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
        if (0 == byInsId || byInsId > MAXNUM_NPLUS_MCU)
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[GetUsrGrpUserCount] McuSwitchedInsId is invalid in NPlus switched mode.\n");
            return FALSE;
        }
        else
        {
            CApp *pcApp = &g_cNPlusApp;
            pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL != pcInst)
            {
                // 首先支持本地admin登陆
                if ( strncmp( pcUsr->GetName(), "admin" , MAXLEN_PWD) == 0 )
                {
                    bValid = g_cUsrManage.CheckLoginValid(pcUsr);
                    if (bValid)
                    {
                        byUserRole = USRGRPID_SADMIN;
                        return TRUE;
                    }
                }

                // 否则支持N端所有用户的登陆请求
                pcInst->GetGrpUsrCount(USRGRPID_SADMIN, byMaxUsrNum, byUsrNum);
                CExUsrInfo *pcUsersInfo = pcInst->GetUserPtr();

                for (u8 byLoop = 0; byLoop < byUsrNum; byLoop ++)
                {
                    if ( pcUsersInfo[byLoop].IsEqualName(  pcUsr->GetName() ) &&
                         pcUsersInfo[byLoop].IsPassWordEqual( pcUsr->GetPassword() ) )
                    {
                        byUserRole = pcUsersInfo[byLoop].GetUsrGrpId();
                        return TRUE;
                    }
                }
                

            }
            return FALSE;
        }
    }


    // 普通情况，非N+1
    bValid = cUsrManageObj.CheckLoginValid(pcUsr);
    if (!bValid)
    {
        return FALSE;
    }

    // 查找并记录下本用户的组信息    
    if ( strncmp( pcUsr->GetName(), "admin", MAXLEN_PWD ) == 0 )
    {
        byUserRole = USRGRPID_SADMIN;
    }
    else
    {
        s32 nUsrNum = cUsrManageObj.GetUserNum();
        CExUsrInfo cUsrInfo;
        for (s32 nUsrLoop = 0; nUsrLoop < nUsrNum; nUsrLoop ++)
        {
            cUsrManageObj.GetUserFullInfo( &cUsrInfo, nUsrLoop );
            if ( cUsrInfo.IsEqualName( pcUsr->GetName() ) )
            {
                byUserRole = cUsrInfo.GetUsrGrpId();
                break;
            }
        }
    }
    return TRUE;
}


/*=============================================================================
  函 数 名： ProcMcuCfgMsg
  功    能： mcu配置消息分发
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/9/15    4.0			许世林                  创建
=============================================================================*/
void CMcsSsnInst::ProcMcuCfgMsg( const CMessage *const pcMsg)
{
	CServMsg cMsg(pcMsg->content, pcMsg->length);
	cMsg.SetSrcSsnId( ( u8 )GetInsID() );
	cMsg.SetEventId(pcMsg->event);

	//本端离线或者备用且未完全同步则丢弃(MTADP/MP/MC/外设 离线消息除外)，
	if (FALSE == g_cMSSsnApp.JudgeRcvMsgPass())
	{
		return;
	}

	switch (CurState())
	{
	case STATE_NORMAL:
        // guzh [7/17/2007] 权限控制
        switch (pcMsg->event)
        {
        case MCS_MCU_CHANGEMCUEQPCFG_REQ:       //MCS请求修改mcu设备配置信息     
        case MCS_MCU_CHANGEBRDCFG_REQ:          //mcs修改单板配置信息        
        case MCS_MCU_CHANGEMCUGENERALCFG_REQ:   //mcs修改mcu配置信息基本设置        
        case MCS_MCU_REBOOT_CMD:
        case MCS_MCU_REBOOTBRD_REQ:
            if (CMcsSsn::GetUserGroup(GetInsID()) != USRGRPID_SADMIN)
            {
                u16 wNackEvent = pcMsg->event + 2;
                if (MCS_MCU_REBOOT_CMD == pcMsg->event)
                {
                    wNackEvent = MCU_MCS_ALARMINFO_NOTIF;
                }
                // NACK，没有权限
                cMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
                cMsg.SetMsgBody();
                SendMsgToMcs(wNackEvent, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
                return;
            }
            break;
        default:
            break;
        }
        if (MCS_MCU_REBOOT_CMD == pcMsg->event)
        {
            McsMcuRebootCmd(pcMsg);
        }
        else
        {
            ::OspPost( MAKEIID( AID_MCU_CONFIG, 1 ), cMsg.GetEventId(), cMsg.GetServMsg(), cMsg.GetServMsgLen() );
        }		
		break;
	default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[CMcsSsnInst]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode );
		break;
	}   

	return;
}

/*=============================================================================
  函 数 名： ProcMcuCfgRsp
  功    能： mcu配置消息响应
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/9/15    4.0			许世林                  创建
=============================================================================*/
void CMcsSsnInst::ProcMcuCfgRsp(const CMessage * pcMsg)
{
    BOOL32 bUseMsgFilter = TRUE;
    switch( CurState() )
    {
    case STATE_NORMAL:
        // 备板升级成功的回应被允许上报会控
        if ( pcMsg->event == MCU_MCS_UPDATEBRDVERSION_NOTIF ||
             pcMsg->event == MCU_MCS_BOARDSTATUS_NOTIFY )
        {
            // 该类消息永远通过
            bUseMsgFilter = FALSE;
        }
        SendMsgToMcs(pcMsg->event, pcMsg->content, pcMsg->length, bUseMsgFilter);
        break;
    default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcuCfgRsp]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        break;
    }   

    return;
}

/*=============================================================================
  函 数 名： ProcMcuCfgFileMsg
  功    能： mcu配置文件上下载消息响应
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/4/4     4.0			顾振华                创建
=============================================================================*/
void CMcsSsnInst::ProcMcuCfgFileMsg(const CMessage * pcMsg)
{
	CServMsg cMsg(pcMsg->content, pcMsg->length);

	//本端离线或者备用且未完全同步则丢弃(MTADP/MP/MC/外设 离线消息除外)，
	if (FALSE == g_cMSSsnApp.JudgeRcvMsgPass())
	{
		return;
	}

    u8  abyCfgContent[MAXLEN_MCUCFG_INI];
    u32 dwCfgFileLen = MAXLEN_MCUCFG_INI;
    
    switch( CurState() )
    {
    case STATE_NORMAL:
        if (CMcsSsn::GetUserGroup(GetInsID()) != USRGRPID_SADMIN)
        {
            // NACK，没有权限
            cMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
            cMsg.SetMsgBody();
            SendMsgToMcs(pcMsg->event+2, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
            return;
        }
        // 请求配置
        if (MCU_MCS_DOWNLOADCFG_REQ == pcMsg->event)
        {
            BOOL32 bRet = g_cMcuVcApp.GetCfgFileData(abyCfgContent, 
                                                   dwCfgFileLen, 
                                                   dwCfgFileLen,
                                                   TRUE);
            if (!bRet)
            {
                // 读取失败，nack
                cMsg.SetErrorCode(ERR_MCU_CFG_FILEERR);
                SendMsgToMcs(pcMsg->event + 2,
                             cMsg.GetServMsg(), 
                             cMsg.GetServMsgLen());
            }
            else
            {
                u32 dwNetFileLen = htonl(dwCfgFileLen);
                cMsg.SetMsgBody((u8*)(&dwNetFileLen), sizeof(u32));
                cMsg.CatMsgBody(abyCfgContent, (u16)dwCfgFileLen);

                SendMsgToMcs(pcMsg->event + 1,
                             cMsg.GetServMsg(), 
                             cMsg.GetServMsgLen());

            }            
        }
        // 上传配置
        else
        {
            dwCfgFileLen = ntohl(*(u32*)cMsg.GetMsgBody());
            dwCfgFileLen = (dwCfgFileLen > MAXLEN_MCUCFG_INI) ?
                            MAXLEN_MCUCFG_INI :
                            dwCfgFileLen;
            memcpy( abyCfgContent, 
                    (u8*)(cMsg.GetMsgBody()+sizeof(u32)), 
                    dwCfgFileLen );

            BOOL32 bRet = g_cMcuVcApp.SetCfgFileData( abyCfgContent, 
                                                    dwCfgFileLen, 
                                                    TRUE);
            cMsg.SetMsgBody(NULL);
            if (!bRet)
            {
                // 写文件失败
                cMsg.SetErrorCode(ERR_MCU_CFG_FILEERR);
                SendMsgToMcs(pcMsg->event + 2,
                             cMsg.GetServMsg(), 
                             cMsg.GetServMsgLen());
            }
            else
            {
			#if defined(_8KH_) || defined(_8KE_) || defined(_8KI_)
				
				TGKProxyCfgInfo tGkPxyCfgInfo;
				g_cMcuAgent.GetGkProxyCfgInfoFromCfgFile(tGkPxyCfgInfo);
				g_cMcuAgent.SetGkProxyCfgInfo(tGkPxyCfgInfo);

				//[支持配置文件导入导出]将mcucfg.ini中的DMZ信息写入pxysrvcfg.ini[5/14/2013 chendaiwei]
				TProxyDMZInfo tDMzInfo;
				g_cMcuAgent.GetProxyDMZInfoFromMcuCfgFile(tDMzInfo);
				g_cMcuAgent.SetProxyDMZInfo(tDMzInfo);

				//[支持配置文件导入导出]将mcucfg.ini中的多运营商信息写入multimanucfg.ini[5/14/2013 chendaiwei]
				TMultiManuNetAccess tMultiNetAccess;
				g_cMcuAgent.GetMultiManuNetAccess(tMultiNetAccess,TRUE);
				g_cMcuAgent.WriteMultiManuNetAccess(tMultiNetAccess);

				TMultiManuNetCfg tMultNetCfg;
				memcpy(&tMultNetCfg.m_tMultiManuNetAccess,&tMultiNetAccess,sizeof(tMultiNetAccess));
				tMultNetCfg.SetGkIp(tGkPxyCfgInfo.GetGkIpAddr());
				tMultNetCfg.m_byEnableMultiManuNet = (u8)tGkPxyCfgInfo.IsProxyUsed();
				g_cMcuAgent.SetMultiManuNetCfg(tMultNetCfg);

			#endif

                SendMsgToMcs(pcMsg->event + 1,
                             cMsg.GetServMsg(), 
                             cMsg.GetServMsgLen());
            }
        }

        break;
    default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcuCfgFileMsg]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        break;
    }   

    return;
}


/*=============================================================================
  函 数 名： ProcMcsNPlusReq
  功    能： mcu N-Plus消息响应
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/12/15    4.0			顾振华                  创建
=============================================================================*/
void CMcsSsnInst::ProcMcsNPlusReq(const CMessage * pcMsg)
{
    CServMsg cMsg(pcMsg->content, pcMsg->length);

    switch( CurState() )
    {
    case STATE_NORMAL:
        if (CMcsSsn::GetUserGroup(GetInsID()) != USRGRPID_SADMIN)
        {
            // NACK，没有权限
            cMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
            SendMsgToMcs(pcMsg->event + 2,
                         cMsg.GetServMsg(), 
                         cMsg.GetServMsgLen() );
            return;
        }
        OspPost(MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON), pcMsg->event, pcMsg->content, pcMsg->length);        

        break;
    default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcuCfgRsp]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        break;
    }   

    return;
}

/*=============================================================================
  函 数 名： ProcNPlusMcsRsp
  功    能： mcu N-Plus消息响应回复
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/12/15    4.0			顾振华                  创建
=============================================================================*/
void CMcsSsnInst::ProcNPlusMcsRsp(const CMessage * pcMsg)
{
    switch( CurState() )
    {
    case STATE_NORMAL:
        SendMsgToMcs(pcMsg->event, pcMsg->content, pcMsg->length);
        break;

    default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcuCfgRsp]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
			      pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);
        break;
    }
    return;
}



/*=============================================================================
函 数 名： ProcMcuUnitTestMsg
功    能： 处理MCU单元测试消息的响应
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2006/2/15    4.0			张宝卿                  创建
=============================================================================*/
void CMcsSsnInst::ProcMcuUnitTestMsg( const CMessage * pcMsg )
{
	switch( CurState() )
	{
	case STATE_NORMAL:
		SendMsgToMcs(pcMsg->event, pcMsg->content, pcMsg->length, FALSE);
		break;
	default:
		LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcuUnitTestMsg]: Wrong inst state in state.%u InsID.%u srcnode.%u!\n", 
			CurState(), GetInsID(), pcMsg->srcnode);
		break;
	}
	return;
}

/*=============================================================================
  函 数 名： McsMcuRebootCmd
  功    能： 会控远程重启
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
#ifdef WIN32
extern HWND g_hwndMain;
#endif

void CMcsSsnInst::McsMcuRebootCmd( const CMessage * const pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u32 dwMpcIp = *(u32*)cServMsg.GetMsgBody();

#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[McsMcuRebootCmd] User Request Reboot MPC IP: 0x%x, cur mpcip:0x%x\n", 
              ntohl(dwMpcIp), ntohl(g_cMcuAgent.GetMpcIp()));
    if(0 != dwMpcIp && dwMpcIp != g_cMcuAgent.GetMpcIp())
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[McsMcuRebootCmd] failed due to mpcip.0x%x, cur mpcip.0x%x\n",
                                 dwMpcIp, g_cMcuAgent.GetMpcIp());
        return;
    }

#endif
	
	// [6/29/2011 liuxu] 退出前先flush
	mculogflush();
	OspDelay(20);
	
//[2011/02/11 zhushz] mcs修改mcu ip 放到reboot时修改,防止发生断链
#ifndef _MINIMCU_
	if (g_cMcuAgent.IsMpcNetCfgBeModifedByMcs())
	{
  /*#if defined (_8KI_)
		g_cCfgParse.Make8KINetCfgEffect();
  #else*/
		//取得新的网络配置
		TMcuNewNetCfg tMcuNewNetCfg;
		g_cMcuAgent.GetNewNetCfg(tMcuNewNetCfg);
		u32 dwMpcNewIp = tMcuNewNetCfg.GetNewMpcIP();
 		u8  byMpcNewInterface = tMcuNewNetCfg.GetNewMpcInterface();
		u32 dwMpcNewMaskIp = tMcuNewNetCfg.GetNewMpcMaskIp();
		u32 wMpcNewGateWay = tMcuNewNetCfg.GetNewMpcDefGateWay();

		//修改网络配置
		g_cMcuAgent.SetMpcIp(dwMpcNewIp, byMpcNewInterface);
		g_cMcuAgent.SetInterface(byMpcNewInterface);
		g_cMcuAgent.SetMaskIp(dwMpcNewMaskIp, byMpcNewInterface);
		g_cMcuAgent.SetGateway(wMpcNewGateWay);
  //#endif

		//恢复网络配置修改标志位
		g_cMcuAgent.SetIsNetCfgBeModifed(FALSE);
	}
#endif
// 设置DSCinfo中的netparam放到重启消息处理函数，防止和会控断链
#ifdef _MINIMCU_
	TDSCModuleInfo tDscInfo, tLastDscInfo;
	g_cMcuAgent.GetDscInfo( &tDscInfo );
	g_cMcuAgent.GetLastDscInfo( &tLastDscInfo );
	printf("[McsMcuRebootCmd] The new dscinfo is :\n");
	tDscInfo.cosPrint();
	printf("[McsMcuRebootCmd] The old dscinfo is :\n");
	tLastDscInfo.cosPrint();
	TMINIMCUNetParamAll tNetParamAllNew, tNetParamAllLast;
	tDscInfo.GetMcsAccessAddrAll( tNetParamAllNew );
	tLastDscInfo.GetMcsAccessAddrAll( tNetParamAllLast );
	if ( !tNetParamAllLast.IsEqualTo(tNetParamAllNew) )
	{
		BOOL32 bSuccess = TRUE;
		bSuccess = g_cMcuAgent.SaveDscLocalInfoToNip( &tDscInfo );
		bSuccess &= g_cMcuAgent.SaveRouteToNipByDscInfo( &tDscInfo );
		if ( !bSuccess )
		{
			printf("[McsMcuRebootCmd] Save dsc localinfo to nip failed!Roll back!\n");
			bSuccess = TRUE;
			bSuccess &= ( SUCCESS_AGENT == g_cMcuAgent.SetDscInfo( &tLastDscInfo, TRUE ) ) ? TRUE : FALSE;
			bSuccess &= g_cMcuAgent.SaveDscLocalInfoToNip( &tLastDscInfo );
			bSuccess &= g_cMcuAgent.SaveRouteToNipByDscInfo( &tLastDscInfo );
			if ( !bSuccess )
			{
				printf("[McsMcuRebootCmd] Mcu dscinfo roll back ERROR!\n");
			}				
		}
		else
		{
			printf("[McsMcuRebootCmd] Save mcs access addr successfully!\n" );
		}
	}
	else
	{
		printf("[McsMcuRebootCmd] The mcu McsAccessAddr is not changed!\n");
		// 设置路由
		if (  !g_cMcuAgent.SaveRouteToNipByDscInfo( &tDscInfo ) )
		{
			printf("[McsMcuRebootCmd] Save route failed! roll back!\n");
			BOOL32 bSuccess = TRUE;
			bSuccess &= ( SUCCESS_AGENT == g_cMcuAgent.SetDscInfo( &tLastDscInfo, TRUE ) ) ? TRUE : FALSE;
			bSuccess &= g_cMcuAgent.SaveRouteToNipByDscInfo( &tLastDscInfo );
			if ( !bSuccess )
			{
				printf( "[McsMcuRebootCmd]Save route roll back failed!\n" );
			}
		}
	}

#endif
	//LogPrint(LOG_LVL_DETAIL, MID_PUB_ALWAYS, "[McsMcuRebootCmd]mcs req reboot\n");
	//printf("[McsMcuRebootCmd]mcs req reboot\n");

	// reboot所有MPU(Bas)单板 [5/3/2012 chendaiwei]
	if(g_cMcuAgent.IsNeedRebootAllMpuBas())
	{
		u8 byMpuBasNum = 0;
		TEqpMpuBasInfo atMpuBasInfo[MAXNUM_PERIEQP];     
		u16 wRet = g_cMcuAgent.ReadMpuBasTable(&byMpuBasNum, atMpuBasInfo);

		if (SUCCESS_AGENT != wRet)
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_MCS,"Read MpuBas Table error,failed to reboot All MPU brd!\n");
		}
		else
		{
			TBoardInfo atBrdInfo[MAX_BOARD_NUM];
			u8 byBrdNum = 0;
			wRet = g_cMcuAgent.ReadBrdTable(&byBrdNum,atBrdInfo);

			if(SUCCESS_AGENT != wRet)
			{
				LogPrint(LOG_LVL_ERROR,MID_MCU_MCS,"Read Brd Table error,failed to reboot All MPU brd!\n");
			}
			else
			{
				for( u8 byBasIdx = 0; byBasIdx < byMpuBasNum; byBasIdx ++ )
				{
					for(u8 byIdx = 0; byIdx < byBrdNum; byIdx ++)
					{
						if(atBrdInfo[byIdx].GetBrdId() == atMpuBasInfo[byBasIdx].GetRunBrdId())
						{
							wRet = g_cMcuAgent.RebootBoard(atBrdInfo[byIdx].GetLayer(), atBrdInfo[byIdx].GetSlot(), atBrdInfo[byIdx].GetType());
							if (SUCCESS_AGENT != wRet)
							{
								LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[McsMcuRebootCmd] Reboot MPU Board failed, ret :%d\n", wRet);
							}

							break;
						}
					}
				}
			}
		}
	}

#ifdef WIN32
    PostMessage( g_hwndMain, WM_CLOSE, NULL, NULL );

#elif (defined(_8KE_) || defined(_8KH_) || defined(_8KI_) ) && defined(_LINUX_)	
	//清业务正常启动标记
//	s8    achProfileName[MAXLEN_MCU_FILEPATH] = {0};
//	sprintf(achProfileName, "%s/%s", DIR_DATA, RUNSTATUS_8KE_CHKFILENAME);
//	s32 dwRunSt = 0;
//	SetRegKeyInt( achProfileName, SECTION_RUNSTATUS, KEY_MCU8KE, dwRunSt );
	//execute reboot
	s8   chCmdline[256] =  {0};   
	sprintf(chCmdline,  "reboot");
	system(chCmdline);

	McuAgentQuit(FALSE);
    OspDelay(100);
#else

	// 测试发现，如果调用 OspQuit 会立即出发OSP_DISCONNECT消息，导致硬件主备切换失败
    // 暂时不退出Osp，让对端Osp检测到断链
    // 这里会导致其他外设过很久才检测到断链,甚至发生重启成功后外设端状态错乱

	McuAgentQuit(FALSE);
    OspDelay(100);	

    if (!g_cMSSsnApp.IsDoubleLink())
    {
        // 不是双链
        OspQuit();
    }

	BrdHwReset();
#endif


    
    return;
}

/*====================================================================
    函数名      ：ProcUserManagerMsg
    功能        ：处理用户管理消息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：成功返回TRUE，若该下级终端未登记返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/28    1.0         LI Yi         创建
    06/06/21    4.0         顾振华        增加用户组逻辑
====================================================================*/
void CMcsSsnInst::ProcUserManagerMsg( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	CExUsrInfo  *ptUserFullInfo = NULL;
    TUsrGrpInfo tUsrGrpInfo;
    
    // 当前会控的用户所属组
    u8 byMcsGrpId = CMcsSsn::GetUserGroup( GetInsID() );

    // 操作对象所属组
    u8 byUsrGrpId;
    
    // guzh [12/14/2006] 如果是备份切换后的工作方式,没有修改权限
    if (MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState())
    {
        switch (pcMsg->event)
        {
        case MCS_MCU_ADDUSER_REQ:
        case MCS_MCU_DELUSER_REQ:
        case MCS_MCU_CHANGEUSER_REQ:
        case MCS_MCU_ADDUSERGRP_REQ:
        case MCS_MCU_CHANGEUSERGRP_REQ:
        case MCS_MCU_DELUSERGRP_REQ:
			cServMsg.SetErrorCode(ERR_MCU_NPLUS_EDITUSER);
			SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			return;
        default:
            break;
        }
    }

	switch (pcMsg->event)
	{
	case MCS_MCU_ADDUSER_REQ:       //会议控制台请求MCU添加用户
        {
		    ptUserFullInfo = (CExUsrInfo*)cServMsg.GetMsgBody();

		    cServMsg.SetMsgBody((u8*)ptUserFullInfo->GetName(), MAX_CHARLENGTH);
            byUsrGrpId = ptUserFullInfo->GetUsrGrpId();

            if ( byMcsGrpId != USRGRPID_SADMIN && 
                 byMcsGrpId != byUsrGrpId )
            {
                // 不能操作非所属组
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
			    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			    return;
            }

           /* BOOL32 bRet = FALSE;*/
            u8 byMaxNum = 0;
            u8 byNum = 0;

            /*bRet = */
			g_cMcuVcApp.GetMCSUsrGrpUserCount( byUsrGrpId, byMaxNum, byNum );
            if ( byNum >= byMaxNum)
            {
                // 当前组用户满
			    cServMsg.SetErrorCode(UM_NOT_ENOUGHMEM);
			    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			    return;
            }

		    if (g_cUsrManage.AddUser(ptUserFullInfo))
		    {       
			    SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

			    cServMsg.SetMsgBody((u8*)ptUserFullInfo, sizeof(CExUsrInfo));

                // 通知所在用户组
			    g_cMcsSsnApp.Broadcast2SpecGrpMcsSsn( byUsrGrpId, MCU_MCS_ADDUSER_NOTIF, 
											         cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		    }
		    else
		    {
			    cServMsg.SetErrorCode(g_cUsrManage.GetLastError());
			    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		    }
        }
		break;

	case MCS_MCU_DELUSER_REQ:       //会议控制台请求MCU删除用户

		ptUserFullInfo = (CExUsrInfo*)cServMsg.GetMsgBody();
        byUsrGrpId = ptUserFullInfo->GetUsrGrpId();
		
        if ( byMcsGrpId != USRGRPID_SADMIN && 
             byMcsGrpId != byUsrGrpId )
        {
            // 不能操作非所属组
			cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
			SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			return;
        }

		if (0 == strncmp(m_achUser, ptUserFullInfo->GetName(), (MAXLEN_PWD-1)))
		{
            // 不能删除自己
			cServMsg.SetErrorCode(UM_ACTOR_ONLY);
			SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			return;
		}

        cServMsg.SetMsgBody((u8*)ptUserFullInfo->GetName(), MAX_CHARLENGTH);
		if (g_cUsrManage.DelUser(ptUserFullInfo))
		{
			SendMsgToMcs( pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

            // 通知所在用户组
			g_cMcsSsnApp.Broadcast2SpecGrpMcsSsn( byUsrGrpId, MCU_MCS_DELUSER_NOTIF, 
											      cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
		else
		{
			cServMsg.SetErrorCode( g_cUsrManage.GetLastError() );
			SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}

		break;

	case MCS_MCU_CHANGEUSER_REQ:  //会议控制台请求MCU修改用户

		ptUserFullInfo = (CExUsrInfo*)cServMsg.GetMsgBody();
        byUsrGrpId = ptUserFullInfo->GetUsrGrpId();

		cServMsg.SetMsgBody((u8*)ptUserFullInfo->GetName(), MAX_CHARLENGTH);

        if ( byMcsGrpId != USRGRPID_SADMIN && 
             byMcsGrpId != byUsrGrpId )
        {
            // 不能操作非所属组
			cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
			SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());		
            return;
        }
        
		if (g_cUsrManage.ModifyInfo(ptUserFullInfo))
		{
			SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

			cServMsg.SetMsgBody((u8*)ptUserFullInfo, sizeof(CExUsrInfo));
			g_cMcsSsnApp.Broadcast2SpecGrpMcsSsn( byUsrGrpId, MCU_MCS_CHANGEUSER_NOTIF, 
											      cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
		else
		{
			cServMsg.SetErrorCode( g_cUsrManage.GetLastError() );
			SendMsgToMcs( pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		}
		
		break;
		
	case MCS_MCU_GETUSERLIST_REQ:   //会议控制台请求MCU得到用户列表
		{
			CLongMessage cLongMsg;
			u8 abyBuf[sizeof(CExUsrInfo) * USERNUM_PERPKT];   // CExUsrInfo size 249

			u8 byUsrItr = 0; // 查找用户的指针
			u8 byUserNumInPack = 0;

            BOOL32 bRet = FALSE;
            u8 byUsrMaxNum = 0;
            u8 byUsrNum = 0;

            bRet = g_cMcuVcApp.GetMCSUsrGrpUserCount(byMcsGrpId, byUsrMaxNum, byUsrNum);
            
            // 计算需要发的总包数
            cLongMsg.m_uTotal = byUsrNum / USERNUM_PERPKT;
            if ( byUsrNum % USERNUM_PERPKT != 0 )
            {
                cLongMsg.m_uTotal ++;
            }
            cLongMsg.m_uCurr = 1;

			if ( !bRet || byUsrNum == 0)
			{
                if (!bRet)
                {
                    // 获取失败,Nack
                    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());  // Nack
                }
                else
                {
                    // 数量为0,Ack
                    SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()); // Ack
                }
				
				LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcUserManagerMsg] No users in current group\n");
			}
			else
			{
				SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()); // Ack

                // Notify
				while ( cLongMsg.m_uCurr <= cLongMsg.m_uTotal ) 
				{
					byUserNumInPack = 0;
					if ( !g_cMcuVcApp.GetMCSCurUserList(byMcsGrpId, abyBuf, byUsrItr, byUserNumInPack) )
					{
						LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcUserManagerMsg] GetMcuCurUserList Failed at index: %d\n", byUsrItr);
                        return;
					}
					cLongMsg.m_uNumUsr = byUserNumInPack;

					cServMsg.SetMsgBody((u8*)&cLongMsg, sizeof(cLongMsg));
					cServMsg.CatMsgBody(abyBuf, byUserNumInPack * sizeof(CExUsrInfo));

					SendMsgToMcs( MCU_MCS_USERLIST_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

                    cLongMsg.m_uCurr ++;
				}
			}
		}
		break;
    case MCS_MCU_ADDUSERGRP_REQ:    //会议控制台添加用户组
        // 只要超级管理员有权限
        {
            tUsrGrpInfo = *(TUsrGrpInfo*)cServMsg.GetMsgBody();

            if ( byMcsGrpId != USRGRPID_SADMIN  )
            {
                // 不能操作
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
                cServMsg.SetMsgBody( (u8*)tUsrGrpInfo.GetUsrGrpName(), MAX_CHARLENGTH );
			    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                return;
            }
            else
            {
                u16 wErrorCode = 0;
                byUsrGrpId = g_cMcuVcApp.AddMCSUserGroup(tUsrGrpInfo, wErrorCode);
                if ( byUsrGrpId == USRGRPID_INVALID )
                {
			        cServMsg.SetErrorCode( wErrorCode );
                    cServMsg.SetMsgBody( (u8*)tUsrGrpInfo.GetUsrGrpName(), MAX_CHARLENGTH );
			        SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                }
                else
                {
                    tUsrGrpInfo.SetUsrGrpId(byUsrGrpId);
                    cServMsg.SetMsgBody( (u8*)&tUsrGrpInfo, sizeof(TUsrGrpInfo) );

                    SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	

                    cServMsg.SetEventId( MCU_MCS_USERGRP_NOTIF );
			        g_cMcsSsnApp.Broadcast2SpecGrpMcsSsn( byUsrGrpId, MCU_MCS_USERGRP_NOTIF, 
											              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                }
            }
        }
        break;
        
    case MCS_MCU_CHANGEUSERGRP_REQ:    //会议控制台修改用户组
        // 只要超级管理员有权限
        {
            tUsrGrpInfo = *(TUsrGrpInfo*)cServMsg.GetMsgBody();
            u8 byTargetId = tUsrGrpInfo.GetUsrGrpId();
            if ( byMcsGrpId != USRGRPID_SADMIN  )
            {
                // 不能操作
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);                
                cServMsg.SetMsgBody( (u8*)&byTargetId, sizeof(u8) );
			    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                return;
            }
            else
            {
                u16 wErrorCode = 0;
                if ( !g_cMcuVcApp.ChgMCSUserGroup(tUsrGrpInfo, wErrorCode) )
                {
			        cServMsg.SetErrorCode( wErrorCode );
                    cServMsg.SetMsgBody( (u8*)&byTargetId, sizeof(u8) );
			        SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                }
                else
                {
                    SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	

                    cServMsg.SetEventId( MCU_MCS_USERGRP_NOTIF );

			        g_cMcsSsnApp.Broadcast2SpecGrpMcsSsn( byMcsGrpId, MCU_MCS_USERGRP_NOTIF, 
											              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                }
            }
        }
        break;

    case MCS_MCU_DELUSERGRP_REQ:    //会议控制台删除用户组及相关资源
        // 只要超级管理员有权限
        {
            u8 byDelGrpId = *cServMsg.GetMsgBody();
            if ( byMcsGrpId != USRGRPID_SADMIN )
            {
                // 不能操作
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
			    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());		
                return;
            }
            else
            {
                u16 wErrorCode = 0;
                if ( !g_cMcuVcApp.DelMCSUserGroup(byDelGrpId, wErrorCode) )
                {
			        cServMsg.SetErrorCode( wErrorCode );
			        SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
                }
                else
                {
                    SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	

                    // Nofity
                    cServMsg.SetEventId( MCU_MCS_DELUSERGRP_NOTIF );
                    
			        g_cMcsSsnApp.Broadcast2SpecGrpMcsSsn( byMcsGrpId, MCU_MCS_DELUSERGRP_NOTIF, 
											              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                }
            }
        }
        break;
        
    case MCS_MCU_GETUSERGRP_REQ:    //会议控制台请求用户组信息
        // 只要超级管理员有权限
        {
            if ( byMcsGrpId != USRGRPID_SADMIN )
            {
                // 不能操作
			    cServMsg.SetErrorCode(UM_OPERATE_NOTPERMIT);
                cServMsg.SetMsgBody();
			    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());		
            }
            else
            {
                u8 byNum = 0;
                TUsrGrpInfo *ptInfo = NULL;
                g_cMcuVcApp.GetMCSUserGroupInfo( byNum, &ptInfo );
                
                cServMsg.SetMsgBody( &byNum, sizeof(u8) );
                cServMsg.CatMsgBody( (u8*)ptInfo, sizeof(TUsrGrpInfo) * byNum );

                SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());	
            }
        }
        break;

    default:
        break;
	}

	return;
}


/*=============================================================================
函 数 名： ProcAddrBookMsg
功    能： 地址簿消息处理
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/2/14  4.0			许世林                  创建
=============================================================================*/
void CMcsSsnInst::ProcAddrBookMsg( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if(NULL == g_pcAddrBook)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[CMcsSsnInst][ProcAddrBookMsg] g_pcAddrBook == NULL!\n");
		if (cServMsg.GetSrcSsnId() != 0)
		{
			cServMsg.SetErrorCode(ERR_MCU_ADDRBOOKINIT);
			SendMsgToMcs(pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
        return;
    }
    switch(pcMsg->event)
    {
    case MCS_MCU_ADDRBOOK_GETENTRYLIST_REQ:
    case MCU_ADDRBOOK_GETENTRYLIST_NOTIF:    
        {
            if(MCS_MCU_ADDRBOOK_GETENTRYLIST_REQ == pcMsg->event)
            {
                SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()); //ack
            }

            u32 dwSize = g_pcAddrBook->GetAddrEntryUsedSize();
			if(0 == dwSize)
            {
                return;
            }

            u8 byTotalPktNum = (u8)(dwSize/ADDRENTRYNUM_PERPKT);
            if(dwSize%ADDRENTRYNUM_PERPKT > 0)
            {
                byTotalPktNum++;
            }
            cServMsg.SetTotalPktNum(byTotalPktNum);
            
            u8 byCurPktIdx = 0;
            u32 dwCurNum = 0;
			u32 dwCurV1Num = 0;
            CAddrEntry cAddrEntry;
            TMcuAddrEntry atAddrEntry[ADDRENTRYNUM_PERPKT];
			// [6/25/2013 guodawei]
			TMcuAddrEntryV1 atAddrEntryV1[ADDRENTRYNUM_PERPKT];

            u32 dwStartIdx = g_pcAddrBook->GetFirstAddrEntry( &cAddrEntry );
            if(INVALID_INDEX == dwStartIdx)
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] get first addr entry failed!\n");
                return;
            }
            AddrEntryOut2In(&cAddrEntry, &atAddrEntry[dwCurNum++]);
			AddrEntryV1Out2In(&cAddrEntry, &atAddrEntryV1[dwCurV1Num++]);

            while( 0 != g_pcAddrBook->GetNextAddrEntry(dwStartIdx, &cAddrEntry))
            {                    
                AddrEntryOut2In(&cAddrEntry, &atAddrEntry[dwCurNum++]);
				AddrEntryV1Out2In(&cAddrEntry, &atAddrEntryV1[dwCurV1Num++]);

                dwStartIdx = cAddrEntry.GetEntryIndex();
                
                if(ADDRENTRYNUM_PERPKT == dwCurNum || ADDRENTRYNUM_PERPKT == dwCurV1Num)
                {
                    cServMsg.SetCurPktIdx(byCurPktIdx++);

					//  [6/25/2013 guodawei]
					u8 byMemberNum = atAddrEntryV1->GetMemberNum();
					cServMsg.SetMsgBody((u8*)&byMemberNum, sizeof(u8));
					for (u8 byIdx = 0; byIdx < atAddrEntryV1->GetMemberNum(); byIdx++)
					{
						u16 wMemberLen = htons(atAddrEntryV1->GetMemberLen(byIdx));
						cServMsg.CatMsgBody((u8*)&wMemberLen, sizeof(u16));
					}
					cServMsg.CatMsgBody((u8*)atAddrEntryV1, sizeof(TMcuAddrEntryV1) * dwCurV1Num);
					SendMsgToMcs(MCU_MCS_ADDRBOOK_GETENTRYLISTEXT_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                    dwCurNum = 0;
					dwCurV1Num = 0;
                }                
            }    
            
            if(dwCurNum > 0 || dwCurV1Num > 0)
            {
                cServMsg.SetCurPktIdx(byCurPktIdx++);

				u8 byMemberNum = atAddrEntryV1->GetMemberNum();
				cServMsg.SetMsgBody((u8*)&byMemberNum, sizeof(u8));
				for (u8 byIdx = 0; byIdx < atAddrEntryV1->GetMemberNum(); byIdx++)
				{
					u16 wMemberLen = htons(atAddrEntryV1->GetMemberLen(byIdx));
					cServMsg.CatMsgBody((u8*)&wMemberLen, sizeof(u16));
				}
				cServMsg.CatMsgBody((u8*)atAddrEntryV1, sizeof(TMcuAddrEntryV1) * dwCurV1Num);
				SendMsgToMcs(MCU_MCS_ADDRBOOK_GETENTRYLISTEXT_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
            }
        }
        break;

    case MCS_MCU_ADDRBOOK_GETGROUPLIST_REQ:
    case MCU_ADDRBOOK_GETGROUPLIST_NOTIF:
        {
            if(MCS_MCU_ADDRBOOK_GETGROUPLIST_REQ == pcMsg->event)
            {
                SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()); //ack
            }
                        
            u32 dwSize = g_pcAddrBook->GetAddrGroupUsedSize();
			if(0 == dwSize)
            {                                
                return;
            }
            
            u8 byTotalPktNum = (u8)(dwSize/ADDRGROUPNUM_PERPKT);
            if(dwSize%ADDRGROUPNUM_PERPKT > 0)
            {
                byTotalPktNum++;
            }
            cServMsg.SetTotalPktNum(byTotalPktNum);
            
            u8 byCurPktIdx = 0;
            u32 dwCurNum = 0;
			u32 dwCurV1Num = 0;
            CAddrMultiSetEntry cAddrGroup;
            TMcuAddrGroup atAddrGroup[ADDRGROUPNUM_PERPKT];
			TMcuAddrGroupV1 atAddrGroupV1[ADDRGROUPNUM_PERPKT];
            u32 dwStartIdx = g_pcAddrBook->GetFirstAddrMultiSetEntry( &cAddrGroup );
            if(INVALID_INDEX == dwStartIdx)
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] get first addr group failed!\n");
                return;
            }
            AddrGroupOut2In(&cAddrGroup, &atAddrGroup[dwCurNum++]);
			AddrGroupV1Out2In(&cAddrGroup, &atAddrGroupV1[dwCurV1Num++]);

            while( 0 != g_pcAddrBook->GetNextAddrMultiSetEntry(dwStartIdx, &cAddrGroup))
            {                    
                AddrGroupOut2In(&cAddrGroup, &atAddrGroup[dwCurNum++]);
				AddrGroupV1Out2In(&cAddrGroup, &atAddrGroupV1[dwCurV1Num++]);

                dwStartIdx = cAddrGroup.GetGroupIndex();

                if(ADDRGROUPNUM_PERPKT == dwCurNum || ADDRGROUPNUM_PERPKT == dwCurV1Num)
                {
                    cServMsg.SetCurPktIdx(byCurPktIdx++);

					u8 byMemberNum = atAddrGroupV1->GetMemberNum();
					cServMsg.SetMsgBody((u8*)&byMemberNum, sizeof(u8));
					for (u8 byIdx = 0; byIdx < atAddrGroupV1->GetMemberNum(); byIdx++)
					{
						u16 wMemberLen = htons(atAddrGroupV1->GetMemberLen(byIdx));
						cServMsg.CatMsgBody((u8*)&wMemberLen, sizeof(u16));
					}
					cServMsg.CatMsgBody((u8*)atAddrGroupV1, sizeof(TMcuAddrGroupV1) * dwCurV1Num);
                    SendMsgToMcs(MCU_MCS_ADDRBOOK_GETGROUPLISTEXT_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                    dwCurNum = 0;
					dwCurV1Num = 0;
                }                
            }        

            if(dwCurNum > 0 || dwCurV1Num > 0)
            {
                cServMsg.SetCurPktIdx(byCurPktIdx++);

				u8 byMemberNum = atAddrGroupV1->GetMemberNum();
				cServMsg.SetMsgBody((u8*)&byMemberNum, sizeof(u8));
				for (u8 byIdx = 0; byIdx < atAddrGroupV1->GetMemberNum(); byIdx++)
				{
					u16 wMemberLen = htons(atAddrGroupV1->GetMemberLen(byIdx));
					cServMsg.CatMsgBody((u8*)&wMemberLen, sizeof(u16));
				}
				cServMsg.CatMsgBody((u8*)atAddrGroupV1, sizeof(TMcuAddrGroupV1) * dwCurV1Num);
                SendMsgToMcs(MCU_MCS_ADDRBOOK_GETGROUPLISTEXT_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
            }
        }
        break;

    case MCS_MCU_ADDRBOOK_ADDENTRY_REQ:
        {
            TMcuAddrEntry *ptAddrEntry = (TMcuAddrEntry *)cServMsg.GetMsgBody();
            TADDRENTRY tAddrEntry;
            AddrEntryIn2Out(ptAddrEntry, &tAddrEntry);
	
			//  [6/17/2013 guodawei]
			s8 *lpMsgBody = (s8 *)(cServMsg.GetMsgBody() + sizeof(TMcuAddrEntry));
			tagAddrEntryInfo tAddrInfo = {0};
			memcpy(&tAddrInfo, &tAddrEntry.GetEntryInfo(), sizeof(tagAddrEntryInfo));

			u16 wEntryNameLen = ntohs(*(u16 *)lpMsgBody);
			lpMsgBody += sizeof(u16);
			memcpy(tAddrInfo.achEntryName, lpMsgBody, wEntryNameLen);
			lpMsgBody += wEntryNameLen;

			u16 wMtAliasLen = ntohs(*(u16 *)lpMsgBody);
			lpMsgBody += sizeof(u16);
			memcpy(tAddrInfo.achMtAlias, lpMsgBody, wMtAliasLen);
			lpMsgBody += wMtAliasLen;

			tAddrEntry.SetEntryInfo(&tAddrInfo);

            if(INVALID_INDEX != g_pcAddrBook->IsAddrEntryExist(tAddrEntry.GetEntryInfo()))
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[CMcsSsnInst][ProcAddrBookMsg] addr entry is already exist\n");
                cServMsg.SetErrorCode(ADDRBOOK_EXIST_ELEMENT);
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            
            u32 dwIndex = g_pcAddrBook->AddAddrEntry( tAddrEntry.GetEntryInfo() );
            if ( INVALID_INDEX == dwIndex )
            {
				LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] add addr entry failed\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }

            ptAddrEntry->SetEntryIdx(dwIndex);
            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_ADDENTRY_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;

    case MCS_MCU_ADDRBOOK_DELENTRY_REQ:
        {
            u32 dwIdx = ntohl(*(u32 *)cServMsg.GetMsgBody());
            if(!g_pcAddrBook->DelAddrEntry( dwIdx ))
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] del addr entry failed\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
			CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_DELENTRY_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;

    case MCS_MCU_ADDRBOOK_MODIFYENTRY_REQ:
        {
            TMcuAddrEntry *ptAddrEntry = (TMcuAddrEntry *)cServMsg.GetMsgBody();
            TADDRENTRY tAddrEntry;
            AddrEntryIn2Out(ptAddrEntry, &tAddrEntry);

			//  [6/17/2013 guodawei]
			s8 *lpMsgBody = (s8 *)(cServMsg.GetMsgBody() + sizeof(TMcuAddrEntry));
			tagAddrEntryInfo tAddrInfo = {0};
			memcpy(&tAddrInfo, &tAddrEntry.GetEntryInfo(), sizeof(tagAddrEntryInfo));
			
			u16 wEntryNameLen = ntohs(*(u16 *)lpMsgBody);
			lpMsgBody += sizeof(u16);
			memcpy(tAddrInfo.achEntryName, lpMsgBody, wEntryNameLen);
			lpMsgBody += wEntryNameLen;
			
			u16 wMtAliasLen = ntohs(*(u16 *)lpMsgBody);
			lpMsgBody += sizeof(u16);
			memcpy(tAddrInfo.achMtAlias, lpMsgBody, wMtAliasLen);
			lpMsgBody += wMtAliasLen;
			
			tAddrEntry.SetEntryInfo(&tAddrInfo);
                        
            BOOL32 bRet = g_pcAddrBook->ModifyAddrEntry( tAddrEntry.GetEntryIndex(), tAddrEntry.GetEntryInfo() );
            if ( !bRet )
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] modify addr entry failed\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            
            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
            CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_MODIFYENTRY_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;

    case MCS_MCU_ADDRBOOK_ADDGROUP_REQ:
        {
            TMcuAddrGroup *ptAddrGroup = (TMcuAddrGroup *)cServMsg.GetMsgBody();
            TADDRMULTISETENTRY tAddrMultiSetEntry;
            AddrGroupIn2Out(ptAddrGroup, &tAddrMultiSetEntry);

			//  [6/17/2013 guodawei]
			s8 *lpMsgBody = (s8 *)(cServMsg.GetMsgBody() + sizeof(TMcuAddrGroup));
			tagMultiSetEntryInfo tEntryInfo;
			memcpy(&tEntryInfo, &tAddrMultiSetEntry.GetEntryInfo(), sizeof(tagAddrEntryInfo));
			
			u16 wGroupNameLen = ntohs(*(u16 *)lpMsgBody);
			lpMsgBody += sizeof(u16);
			memcpy(tEntryInfo.achGroupName, lpMsgBody, wGroupNameLen);
			lpMsgBody += wGroupNameLen;

			tAddrMultiSetEntry.SetEntryInfo(&tEntryInfo);

            if(INVALID_INDEX != g_pcAddrBook->IsAddrMultiSetEntryExist(tAddrMultiSetEntry.GetEntryInfo()))
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] addr group is already exist\n");
                cServMsg.SetErrorCode(ADDRBOOK_EXIST_ELEMENT);
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            
            u32 dwIndex = g_pcAddrBook->AddAddrMultiSetEntry( tAddrMultiSetEntry.GetEntryInfo() );
            if ( INVALID_INDEX == dwIndex )
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] add addr group failed\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            
            ptAddrGroup->SetEntryIdx(dwIndex);
            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

            u8 byConfTableLen = (u8)tAddrMultiSetEntry.GetListEntryNum();
            u32 adwConfEntryTable[ MAXNUM_ENTRY_TABLE ] = { 0 };
            tAddrMultiSetEntry.GetListEntryIndex( adwConfEntryTable, byConfTableLen );
            for ( u8 byIdx = 0; byIdx < byConfTableLen; byIdx++ )
            {
                g_pcAddrBook->AddAddrEntry( dwIndex, adwConfEntryTable[byIdx] );
            }

            CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_ADDGROUP_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;

    case MCS_MCU_ADDRBOOK_DELGROUP_REQ:
        {
            u32 dwIdx = ntohl(*(u32 *)cServMsg.GetMsgBody());
            if(!g_pcAddrBook->DelAddrMultiSetEntry( dwIdx ))
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] del addr group failed\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
            CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_DELGROUP_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;

    case MCS_MCU_ADDRBOOK_MODIFYGROUP_REQ:
        {
            TMcuAddrGroup *ptAddrGroup = (TMcuAddrGroup *)cServMsg.GetMsgBody();
            TADDRMULTISETENTRY tAddrMultiSetEntry;
            AddrGroupIn2Out(ptAddrGroup, &tAddrMultiSetEntry);

			//  [6/17/2013 guodawei]
			s8 *lpMsgBody = (s8 *)(cServMsg.GetMsgBody() + sizeof(TMcuAddrGroup));
			tagMultiSetEntryInfo tEntryInfo;
			memcpy(&tEntryInfo, &tAddrMultiSetEntry.GetEntryInfo(), sizeof(tagAddrEntryInfo));
			
			u16 wGroupNameLen = ntohs(*(u16 *)lpMsgBody);
			lpMsgBody += sizeof(u16);
			memcpy(tEntryInfo.achGroupName, lpMsgBody, wGroupNameLen);
			lpMsgBody += wGroupNameLen;
			
			tAddrMultiSetEntry.SetEntryInfo(&tEntryInfo);
            
            CAddrMultiSetEntry cAddrEntry;
            g_pcAddrBook->GetAddrMultiSetEntry( &cAddrEntry, tAddrMultiSetEntry.GetEntryIndex() );
            BOOL32 bRet = g_pcAddrBook->ModifyAddrGroup( tAddrMultiSetEntry.GetEntryIndex(), 
                                                         tAddrMultiSetEntry.GetEntryInfo(),
                                                         cAddrEntry.GetEntryCount(),
                                                         cAddrEntry.GetEntryTable() );
            if ( !bRet )
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] modify addr group failed\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            
            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());          
            CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_MODIFYGROUP_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;

    case MCS_MCU_ADDRBOOK_ADDENTRYTOGROUP_REQ:
        {
            TMcuAddrGroup *ptAddrGroup = (TMcuAddrGroup *)cServMsg.GetMsgBody();
            u32 dwGroupEntryIndex = ptAddrGroup->GetEntryIdx();
            u32 dwAddrEntryNum = ptAddrGroup->GetEntryNum();
            u32 adwConfEntryTable[ MAXNUM_ENTRY_TABLE ] = { 0 };
            ptAddrGroup->GetAllEntryIdx(adwConfEntryTable, dwAddrEntryNum);
            
            if( !g_pcAddrBook->IsAddrMultiSetEntryValid(dwGroupEntryIndex))
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] add addr to group failed, entry idx invalid\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
     
            TMcuAddrGroup tMcuAddrGroup;
            tMcuAddrGroup.SetEntryIdx(ptAddrGroup->GetEntryIdx());
            tMcuAddrGroup.SetGroupName(ptAddrGroup->GetGroupName());
            u32 adwAddEntryTable[ MAXNUM_ENTRY_TABLE ] = { 0 };
            u32 dwAddEntryNum = 0;
            for ( u32 dwIdx = 0; dwIdx < dwAddrEntryNum; dwIdx++ )
            {

                u32 dwIndex = g_pcAddrBook->AddAddrEntry( dwGroupEntryIndex, adwConfEntryTable[dwIdx] );
                if ( dwIndex == INVALID_INDEX )
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] add addr to group failed, EntryNum :%d\n", dwAddrEntryNum);
                    cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                    SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

                    if (dwAddEntryNum >= 1)
                    {
                        tMcuAddrGroup.SetAllEntryIdx(adwAddEntryTable, dwAddEntryNum);
                        CServMsg cNotifMsg;
                        cNotifMsg.SetMsgBody((u8*)&tMcuAddrGroup, sizeof(TMcuAddrGroup));

						//  [6/17/2013 guodawei]
						cNotifMsg.CatMsgBody((cServMsg.GetMsgBody() + sizeof(TMcuAddrGroup)), 
							cServMsg.GetMsgBodyLen() - sizeof(TMcuAddrGroup));

                        CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_ADDENTRYTOGROUP_NOTIF, 
                                              cNotifMsg.GetServMsg(), cNotifMsg.GetServMsgLen());
                        LogPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[ProcAddrBookMsg] not all entry be added to Group:%d, actual add num is :%d\n",
                            dwGroupEntryIndex, dwAddEntryNum);

                    }

                    //有部分条目添加成功要写文件
                    //return;
                    break;
                }
                adwAddEntryTable[dwAddEntryNum] = dwIndex;
                dwAddEntryNum++;

            }

            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());          
            CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_ADDENTRYTOGROUP_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;

    case MCS_MCU_ADDRBOOK_DELENTRYFROMGROUP_REQ:
        {
            TMcuAddrGroup *ptAddrGroup = (TMcuAddrGroup *)cServMsg.GetMsgBody();
            
            u32 dwGroupEntryIndex = ptAddrGroup->GetEntryIdx();
            u32  dwAddrEntryNum = ptAddrGroup->GetEntryNum();
            u32 adwConfEntryTable[ MAXNUM_ENTRY_TABLE ] = { 0 };
            ptAddrGroup->GetAllEntryIdx( adwConfEntryTable, dwAddrEntryNum );

            if(! g_pcAddrBook->IsAddrMultiSetEntryValid(dwGroupEntryIndex))
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcAddrBookMsg] del addr from group failed, entry idx invalid\n");
                cServMsg.SetErrorCode((u16)g_pcAddrBook->GetLastError());
                SendMsgToMcs(pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
                return;
            }
            
            for ( u32 dwIdx = 0; dwIdx < dwAddrEntryNum; dwIdx++ )
            {
                g_pcAddrBook->DelAddrEntry( dwGroupEntryIndex, adwConfEntryTable[dwIdx] );                
            }

            SendMsgToMcs(pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());          
            CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_ADDRBOOK_DELENTRYFROMGROUP_NOTIF, 
                                              cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        break;
        
    default: return;
    }

    if (pcMsg->event == MCS_MCU_ADDRBOOK_GETENTRYLIST_REQ ||
        pcMsg->event == MCU_ADDRBOOK_GETENTRYLIST_NOTIF ||
        pcMsg->event == MCS_MCU_ADDRBOOK_GETGROUPLIST_REQ ||
        pcMsg->event == MCU_ADDRBOOK_GETGROUPLIST_NOTIF)
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[CMcsSsnInst][ProcAddrBookMsg] Get addrbook info, no necessary to save!\n");
        return;
    }
    else if(!g_pcAddrBook->SaveAddrBook(GetAddrBookPath()))          
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[CMcsSsnInst][ProcAddrBookMsg] SaveAddrBook failed!\n");
    }

    return;
}


/*=============================================================================
函 数 名： AddrEntryOut2In
功    能： 
算法实现： 
全局变量： 
参    数： CAddrEntry &cAddrIn
           TMcuAddrEntry &tAddrOut
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/2/15  4.0			许世林                  创建
=============================================================================*/
void CMcsSsnInst::AddrEntryOut2In(CAddrEntry *pcAddrIn, TMcuAddrEntry *ptAddrOut, u8 byNum)
{
    if(NULL == pcAddrIn || NULL == ptAddrOut)
    {
        return;
    }
    
    for(u8 byIdx = 0; byIdx < byNum; byIdx++)
    {
        ptAddrOut[byIdx].SetEntryIdx(pcAddrIn[byIdx].GetEntryIndex());
        TADDRENTRYINFO tInfo;
        pcAddrIn[byIdx].GetEntryInfo(&tInfo);
        ptAddrOut[byIdx].SetCallRate(/*ntohs(*/tInfo.wCallRate/*)*/);   //内存中呼叫码率是主机序的
        ptAddrOut[byIdx].SetEntryName(tInfo.achEntryName);
        ptAddrOut[byIdx].SetH320Id(tInfo.achH320Id);
        ptAddrOut[byIdx].SetMtAlias(tInfo.achMtAlias);
        ptAddrOut[byIdx].SetMtE164(tInfo.achMtNumber);
        ptAddrOut[byIdx].SetMtIpAddr(ntohl(tInfo.dwMtIpAddr));
        if(0 == tInfo.bH320Terminal)
        {
            ptAddrOut[byIdx].SetMtProtocolType(PROTOCOL_TYPE_H323);
        }
        else
        {
            ptAddrOut[byIdx].SetMtProtocolType(PROTOCOL_TYPE_H320);
        }
    }    

    return;
}


/*=============================================================================
函 数 名： AddrGroupOut2In
功    能： 
算法实现： 
全局变量： 
参    数： CAddrMultiSetEntry *pcGroupIn
           TMcuAddrGroup *ptGroupOut
           u8 byNum
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/2/18  4.0			许世林                  创建
=============================================================================*/
void CMcsSsnInst::AddrGroupOut2In(CAddrMultiSetEntry *pcGroupIn, TMcuAddrGroup *ptGroupOut, u8 byNum)
{
    if(NULL == pcGroupIn || NULL == ptGroupOut)
    {
        return;
    }

    for(u8 byIdx = 0; byIdx < byNum; byIdx++)
    {
        ptGroupOut[byIdx].SetEntryIdx(pcGroupIn[byIdx].GetGroupIndex());
        TMULTISETENTRYINFO tInfo;
        memset(&tInfo, 0, sizeof(tInfo));
        pcGroupIn[byIdx].GetGroupInfo(&tInfo);
        ptGroupOut[byIdx].SetGroupName(tInfo.achGroupName);
        u8 byGroupNum = pcGroupIn[byIdx].GetEntryCount();
        ptGroupOut[byIdx].SetAllEntryIdx(pcGroupIn[byIdx].GetEntryTable(), byGroupNum);
    }

    return;
}

/*=============================================================================
函 数 名： AddrEntryOut2In
功    能： 
算法实现： 
全局变量： 
参    数： CAddrEntry &cAddrIn
           TMcuAddrEntry &tAddrOut
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/2/15  4.0			许世林                  创建
=============================================================================*/
void CMcsSsnInst::AddrEntryV1Out2In(CAddrEntry *pcAddrIn, TMcuAddrEntryV1 *ptAddrOut, u8 byNum)
{
    if(NULL == pcAddrIn || NULL == ptAddrOut)
    {
        return;
    }
    
    for(u8 byIdx = 0; byIdx < byNum; byIdx++)
    {
        ptAddrOut[byIdx].SetEntryIdx(pcAddrIn[byIdx].GetEntryIndex());
        TADDRENTRYINFO tInfo;
        pcAddrIn[byIdx].GetEntryInfo(&tInfo);
        ptAddrOut[byIdx].SetCallRate(/*ntohs(*/tInfo.wCallRate/*)*/);   //内存中呼叫码率是主机序的
        ptAddrOut[byIdx].SetEntryName(tInfo.achEntryName);
        ptAddrOut[byIdx].SetH320Id(tInfo.achH320Id);
        ptAddrOut[byIdx].SetMtAlias(tInfo.achMtAlias);
        ptAddrOut[byIdx].SetMtE164(tInfo.achMtNumber);
        ptAddrOut[byIdx].SetMtIpAddr(ntohl(tInfo.dwMtIpAddr));
        if(0 == tInfo.bH320Terminal)
        {
            ptAddrOut[byIdx].SetMtProtocolType(PROTOCOL_TYPE_H323);
        }
        else
        {
            ptAddrOut[byIdx].SetMtProtocolType(PROTOCOL_TYPE_H320);
        }
    }    

    return;
}


/*=============================================================================
函 数 名： AddrGroupOut2In
功    能： 
算法实现： 
全局变量： 
参    数： CAddrMultiSetEntry *pcGroupIn
           TMcuAddrGroup *ptGroupOut
           u8 byNum
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/2/18  4.0			许世林                  创建
=============================================================================*/
void CMcsSsnInst::AddrGroupV1Out2In(CAddrMultiSetEntry *pcGroupIn, TMcuAddrGroupV1 *ptGroupOut, u8 byNum)
{
    if(NULL == pcGroupIn || NULL == ptGroupOut)
    {
        return;
    }

    for(u8 byIdx = 0; byIdx < byNum; byIdx++)
    {
        ptGroupOut[byIdx].SetEntryIdx(pcGroupIn[byIdx].GetGroupIndex());
        TMULTISETENTRYINFO tInfo;
        memset(&tInfo, 0, sizeof(tInfo));
        pcGroupIn[byIdx].GetGroupInfo(&tInfo);
        ptGroupOut[byIdx].SetGroupName(tInfo.achGroupName);
        u8 byGroupNum = pcGroupIn[byIdx].GetEntryCount();
        ptGroupOut[byIdx].SetAllEntryIdx(pcGroupIn[byIdx].GetEntryTable(), byGroupNum);
    }

    return;
}


/*=============================================================================
函 数 名： AddrEntryIn2Out
功    能： 
算法实现： 
全局变量： 
参    数： TMcuAddrEntry *ptAddrIn
           TADDRENTRY *ptAddrOut
           u8 byNum
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/2/18  4.0			许世林                  创建
=============================================================================*/
void CMcsSsnInst::AddrEntryIn2Out(TMcuAddrEntry *ptAddrIn, TADDRENTRY *ptAddrOut)
{
    if(NULL == ptAddrIn || NULL == ptAddrOut)
    {
        return;
    }
    
    TADDRENTRYINFO tAddrEntryInfo;
    strcpy(tAddrEntryInfo.achEntryName, ptAddrIn->GetEntryName());
    strcpy(tAddrEntryInfo.achMtAlias, ptAddrIn->GetMtAlias());
    strcpy(tAddrEntryInfo.achH320Id, ptAddrIn->GetH320Id());
    strcpy(tAddrEntryInfo.achMtNumber, ptAddrIn->GetMtE164());
    tAddrEntryInfo.dwMtIpAddr = htonl(ptAddrIn->GetMtIpAddr());
    tAddrEntryInfo.wCallRate = htons(ptAddrIn->GetCallRate());
    tAddrEntryInfo.bH320Terminal = (PROTOCOL_TYPE_H320 == ptAddrIn->GetMtProtocolType()) ? 1 : 0;

    ptAddrOut->SetEntryIndex(ptAddrIn->GetEntryIdx());
    ptAddrOut->/*SetEntryInfo*/SetEntry(&tAddrEntryInfo);       //保证得到结果是网络序
    
    return;
}


/*=============================================================================
函 数 名： AddrGroupIn2Out
功    能： 
算法实现： 
全局变量： 
参    数： TMcuAddrGroup *ptGroupIn
           TADDRMULTISETENTRY *ptGroupOut
           u8 byNum
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/2/18  4.0			许世林                  创建
=============================================================================*/
void CMcsSsnInst::AddrGroupIn2Out(TMcuAddrGroup *ptGroupIn, TADDRMULTISETENTRY *ptGroupOut)
{
    if(NULL == ptGroupIn || NULL == ptGroupOut)
    {
        return;
    }

    TMULTISETENTRYINFO tGroupEntryInfo;
    memset(&tGroupEntryInfo, 0, sizeof(tGroupEntryInfo));
    strcpy(tGroupEntryInfo.achGroupName, ptGroupIn->GetGroupName());

    ptGroupOut->SetEntryIndex(ptGroupIn->GetEntryIdx());
    ptGroupOut->SetEntryInfo(&tGroupEntryInfo);
        					    
    u32 adwEntryIdx[MAXNUM_CONF_MT];
    u32 dwEntryNum = MAXNUM_CONF_MT/*sizeof(adwEntryIdx)*/;		
    ptGroupIn->GetAllEntryIdx(adwEntryIdx, dwEntryNum);

    ptGroupOut->SetListEntryNum(dwEntryNum);
    ptGroupOut->SetListEntryIndex(adwEntryIdx, dwEntryNum);

    return;
}

/*====================================================================
    函数名      DaemonProcIntTestUserInfo
    功能        ：处理用户管理集成测试消息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：成功返回TRUE，若该下级终端未登记返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/28    1.0         LI Yi         创建
====================================================================*/
void CMcsSsnInst::DaemonProcIntTestUserInfo( CMessage *pcMsg )
{
	u8 buf[33] = {"0"};
	u8 byResult = 255;
	u16 wError;
	memcpy( buf, pcMsg->content, 33 );
	switch( buf[32] ) 
	{
	case 1: //检测用户是否存在
		{
			CLoginRequest cLogin;
			cLogin.SetName( (char*)buf );
			cLogin.SetPassword( "~" );
			g_cUsrManage.CheckLoginValid( &cLogin );
			wError = g_cUsrManage.GetLastError();
			if ( wError == UM_LOGIN_ERRPSW )
				byResult = 1;
			else
				byResult = 0;
		}
		break;
	case 2:
		break;
	default :
		break;
	}
	SetSyncAck( &byResult,sizeof(byResult) );
	return ;
}

/*=============================================================================
函 数 名： DaemonProcNPlusUsrGrpNotif
功    能： N+1注册成功后同步用户信息
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/23  4.0			许世林                  创建
=============================================================================*/
void CMcsSsnInst::DaemonProcNPlusUsrGrpNotif( const CMessage * pcMsg )
{
     ProcNPlusGrpInfo(pcMsg);
     ProcNPlusUsrInfo(pcMsg);
     return;
}
/*=============================================================================
函 数 名： DaemonProcAddrInitialized
功    能： 处理地址簿初始化完成消息
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/11/17  4.0			付秀华                 创建
=============================================================================*/
void CMcsSsnInst::DaemonProcAddrInitialized( const CMessage * pcMsg )
{
	if (!pcMsg) return;
	
	if(pcMsg->length == sizeof(CAddrBook*))
	{
		g_pcAddrBook = *(CAddrBook* *)(pcMsg->content);
		CMcsSsn::BroadcastToAllMcsSsn(MCU_ADDRBOOK_GETENTRYLIST_NOTIF);
		CMcsSsn::BroadcastToAllMcsSsn(MCU_ADDRBOOK_GETGROUPLIST_NOTIF);
	}
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS,"[DaemonProcAddrInitialized]bad message body(reallenth:%d required lenth:%d)\n", pcMsg->length, sizeof(CAddrBook*));
	}

    return;
}

/*=============================================================================
函 数 名： ProcNPlusGrpInfo
功    能： N+1用户组处理函数
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/23  4.0			许世林                  创建
=============================================================================*/
void CMcsSsnInst::ProcNPlusGrpInfo( const CMessage * pcMsg )
{
    CUsrGrpsInfo *pcGrpsInfo = g_cMcuVcApp.GetUserGroupInfo();
    
    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_USRGRPUPDATE_REQ);
    cServMsg.SetMsgBody((u8*)pcGrpsInfo, sizeof(CUsrGrpsInfo));
    
    //若为主mcu则srcid为daemon实例，若为备份mcu回滚模式则srcid为备份实例
    OspPost(pcMsg->srcid, VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());    
    return;
}

/*=============================================================================
函 数 名： ProcNPlusUsrInfo
功    能： N+1用户用户处理函数
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/23  4.0			许世林                  创建
=============================================================================*/
void CMcsSsnInst::ProcNPlusUsrInfo( const CMessage * pcMsg )
{        
	u8 abyBuf[sizeof(CExUsrInfo) * USERNUM_PERPKT];   // CExUsrInfo size 249
    u8 byPackNum;
    s32 nUserNum = g_cUsrManage.GetUserNum();
	if (nUserNum % USERNUM_PERPKT > 0)
	{
		byPackNum = (u8)(nUserNum/USERNUM_PERPKT+1);
	}
	else
	{
		byPackNum = (u8)(nUserNum/USERNUM_PERPKT);
	}

    CServMsg cServMsg;
    cServMsg.SetEventId(MCU_NPLUS_USRINFOUPDATE_REQ);
    cServMsg.SetTotalPktNum(byPackNum);

    u8 byUsrItr = 0;
    u8 byUserNumInPack = 0;
    for (u8 byPackIdx = 0; byPackIdx < byPackNum; byPackIdx++)
    {
        cServMsg.SetCurPktIdx(byPackIdx);
        if ( !g_cMcuVcApp.GetMCSCurUserList(USRGRPID_SADMIN, abyBuf, byUsrItr, byUserNumInPack) )
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MCS, "[ProcNPlusUsrInfo] GetMcuCurUserList Failed at index: %d\n", byUsrItr);
            return;
		}
        cServMsg.SetMsgBody(abyBuf, byUserNumInPack * sizeof(CExUsrInfo));
        
        //若为主mcu则srcid为daemon实例，若为备份mcu回滚模式则srcid为备份实例
        OspPost(pcMsg->srcid, VC_NPLUS_MSG_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }	
    
    return;
}

// 删除用户
void CMcsSsnInst::ProcMcuDelUser( CMessage * const pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );

    s8* szName = (s8*)cServMsg.GetMsgBody();

    if ( strncmp(szName, m_achUser, MAX_CHARLENGTH) == 0 )
    {
        // 如果是自己则断开和MCS的连接
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcuDelUser] My Ssn User %s has been deleted, Disconnect with MCS!\n ", m_achUser );
        
        OspDisconnectTcpNode(m_dwMcsNode);
    }

}
#if defined(_8KH_) ||  defined(_8KE_) || defined(_8KI_)
/*====================================================================
函数名         ProcMcsMcuStartNetCapCMD
功能        ： 
算法实现    ： 处理MCS开始抓包请求
引用全局变量：
输入参数说明： 
返回值说明  ： void
----------------------------------------------------------------------
修改记录    ：
 日  期        版本        修改人        修改内容
2013/03/22     4.7         周翼亮         创建
====================================================================*/
void CMcsSsnInst::ProcMcsMcuStartNetCapCMD(  const CMessage * pcMsg ) 
{
	CServMsg cServMsg(pcMsg->content,pcMsg->length);

	//开始抓包
	if ( !g_cMcuVcApp.StartNetCap((u8)GetInsID()) )
	{
		cServMsg.SetErrorCode(1);
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS,"[ProcMcsMcuStartNetCapCMD]ERROR,StartNetCap failed problely curnetcapstate is not idle!\n");
		//error notify
		//CServMsg cServMsg;
		cServMsg.SetEventId(MCU_MCS_ALARMINFO_NOTIF);
		cServMsg.SetErrorCode(ERR_MCU_STARTNETCAP_ALREADYSTART);
		SendMsgToMcs( MCU_MCS_ALARMINFO_NOTIF, cServMsg.GetMsgBody(),cServMsg.GetMsgBodyLen());
		return;
	}
	//2、通知当前抓包状态
	NotifyAllMcsCurNetCapStatus();

}

/*====================================================================
函数名         ProcMcsMcuStoptNetCapCmd
功能        ： 
算法实现    ： 处理MCS停止抓包
引用全局变量：
输入参数说明： 
返回值说明  ： void
----------------------------------------------------------------------
修改记录    ：
 日  期        版本        修改人        修改内容
2013/03/22     4.7         周翼亮         创建
====================================================================*/
void CMcsSsnInst::ProcMcsMcuStoptNetCapCmd(  const CMessage * pcMsg ) 
{
	CServMsg cServMsg(pcMsg->content,pcMsg->length);
	//1、停止抓包
	if (!g_cMcuVcApp.StopNetCap())
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MCS,"[ProcMcsMcuStoptNetCapCmd]StopNetCap failed problely CurNetCapStatus is not start,return!\n");
		//error notify
		CServMsg cServMsg;
		cServMsg.SetEventId(MCU_MCS_ALARMINFO_NOTIF);
		cServMsg.SetErrorCode(ERR_MCU_STOPNETCAP_NOTSTARTYET);
		SendMsgToMcs( MCU_MCS_ALARMINFO_NOTIF, cServMsg.GetMsgBody(),cServMsg.GetMsgBodyLen());
		return;
	}
	//2、状态上报
	NotifyAllMcsCurNetCapStatus();

}

/*====================================================================
函数名         ProcMcsMcuGetNetCapStatusReq
功能        ： 
算法实现    ： 处理MCS获取当前抓包状态请求
引用全局变量：
输入参数说明： 
返回值说明  ： void
----------------------------------------------------------------------
修改记录    ：
 日  期        版本        修改人        修改内容
2013/03/22     4.7         周翼亮         创建
====================================================================*/
void CMcsSsnInst::ProcMcsMcuGetNetCapStatusReq(  const CMessage * pcMsg ) 
{
	CServMsg cServMsg(pcMsg->content,pcMsg->length);
	emNetCapState emCurNetCapState = g_cMcuVcApp.GetNetCapStatus();
	u8 byCurState = (u8)emCurNetCapState;
	cServMsg.SetMsgBody(&byCurState,sizeof(u8));
	SendMsgToMcs( pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );
}
/*====================================================================
函数名         NotifyAllMcsCurNetCapStatus
功能        ： 
算法实现    ： 通知所有MCS当前抓包状态
引用全局变量：
输入参数说明： 
返回值说明  ： void
----------------------------------------------------------------------
修改记录    ：
 日  期        版本        修改人        修改内容
2013/03/22     4.7         周翼亮         创建
====================================================================*/
void CMcsSsnInst::NotifyAllMcsCurNetCapStatus( )
{
	CServMsg cServMsg;
	emNetCapState emCurNetCapState = g_cMcuVcApp.GetNetCapStatus();
	u8 byCurState = (u8)emCurNetCapState;
	cServMsg.SetMsgBody(&byCurState,sizeof(u8));
	cServMsg.SetMcuId( LOCAL_MCUID );
	CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_NETCAPSTATUS_NOTIFY,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
} 

/*====================================================================
函数名         ProcMcsMcuOpenDebugModeCmd
功能        ： 
算法实现    ： 处理MCS开启调试模式请求
引用全局变量：
输入参数说明： 
返回值说明  ： void
----------------------------------------------------------------------
修改记录    ：
 日  期        版本        修改人        修改内容
2013/10/08     4.7.3         周晶晶         创建
====================================================================*/
void CMcsSsnInst::ProcMcsMcuOpenDebugModeCmd( const CMessage* pcMsg )
{
	//CServMsg cServMsg(pcMsg->content,pcMsg->length);
	
	
	//u8 byMode = *(u8*)cServMsg.GetMsgBody();

	/*if( emDebugModeftp != byMode || emDebugModeDebug != byMode )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MCS,"[ProcMcsMcuOpenDebugModeCmd]error mode.%d\n",byMode );
		return;
	}*/
	
#ifdef _LINUX_
	/*if( emDebugModeftp == byMode )
	{		
		//SetFtpUp();
	}
	else */
	//if( emDebugModeDebug == byMode )
	//{
	SetTelnetUp();
	SetSshUp();
	//SetFtpUp();
	//}
#endif
	g_cMcsSsnApp.SetCurrentDebugMode( emDebugModeDebug );
	g_cMcsSsnApp.SetCurrentDebugModeInsId( (u8)GetInsID() );

	NotifyAllMcsCurDebugMode();	
}

/*====================================================================
函数名         ProcMcsMcuStopDebugModeCmd
功能        ： 
算法实现    ： 停止调试模式
引用全局变量：
输入参数说明： 
返回值说明  ： void
----------------------------------------------------------------------
修改记录    ：
 日  期        版本        修改人        修改内容
2013/10/08     4.7.3        周晶晶         创建
====================================================================*/
void CMcsSsnInst::ProcMcsMcuStopDebugModeCmd( const CMessage* pcMsg )
{	
	g_cMcsSsnApp.SetCurrentDebugMode( emDebugModeNone );
	g_cMcsSsnApp.SetCurrentDebugModeInsId( 0 );
	
#ifdef _LINUX_
	SetTelnetDown();
	SetSshDown();
	//SetFtpDown();
#endif
	NotifyAllMcsCurDebugMode();
}

/*====================================================================
函数名         NotifyAllMcsCurDebugMode
功能        ： 
算法实现    ： 通知mcs当前调试模式
引用全局变量：
输入参数说明： 
返回值说明  ： void
----------------------------------------------------------------------
修改记录    ：
 日  期        版本        修改人        修改内容
2013/10/08     4.7.3        周晶晶         创建
====================================================================*/
void CMcsSsnInst::NotifyAllMcsCurDebugMode()
{
	CServMsg cServMsg;
	emDebugMode emCurDebugMode = g_cMcsSsnApp.GetCurrentDebugMode();
	u8 byCurMode = (u8)emCurDebugMode;
	cServMsg.SetMsgBody(&byCurMode,sizeof(u8));
	cServMsg.SetMcuId( LOCAL_MCUID );
	CMcsSsn::BroadcastToAllMcsSsn(MCU_MCS_OPENDEBUGMOED_NOTIFY,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
}

/*====================================================================
函数名         ProcMcsMcuGetDebugModeReq
功能        ： 
算法实现    ： mcs获取调试模式状态
引用全局变量：
输入参数说明： 
返回值说明  ： void
----------------------------------------------------------------------
修改记录    ：
 日  期        版本        修改人        修改内容
2013/10/08     4.7.3        周晶晶         创建
====================================================================*/
void CMcsSsnInst::ProcMcsMcuGetDebugModeReq(const CMessage* pcMsg )
{
	CServMsg cServMsg(pcMsg->content,pcMsg->length);
	emDebugMode emCurDebugMode = g_cMcsSsnApp.GetCurrentDebugMode();
	u8 byCurMode = (u8)emCurDebugMode;
	cServMsg.SetMsgBody(&byCurMode,sizeof(u8));
	SendMsgToMcs( pcMsg->event+1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), FALSE );
}
#endif

//END OF FILE
