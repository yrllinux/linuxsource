/******************************************************************************
   模块名      : 终端适配会话
   文件名      : TMtssn.cpp
   相关文件    : TMtssn.h
   文件实现功能: 终端适配会话实现
   作者        : 贾清亮
   版本        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/11/08  3.0         贾清亮      创建
   2005/02/19  3.6         万春雷      级联修改、与3.5版本合并
******************************************************************************/

#include "evmcumt.h"
#include "evmcu.h"
#include "evmcuvcs.h"
#include "mcuvc.h"
#include "mtadpssn.h"
#include "mcuver.h"
#include "evmcueqp.h"
//#include "mcuerrcode.h"

CMtAdpSsnApp     g_cMtAdpSsnApp;

//构造
CMtAdpSsnInst::CMtAdpSsnInst()
{
	m_byMtAdpId		 = 0;
	m_dwMtAdpAppIId	 = 0;
	m_byProtocolType = 0;
	m_byMaxMtNum     = 0;
	m_dwMtAdpNode	 = INVALID_NODE;
}

//析构
CMtAdpSsnInst::~CMtAdpSsnInst()
{

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
#define MT_EV_LOG(level)	MCU_EV_LOG(level, MID_MCU_MT, "Mt")

void CMtAdpSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	
	if( NULL == pcMsg )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT, "MtAdpSsn[InstanceEntry]: Null pointer message Received in InstanceEntry.\n" );
		return;
	}
	
	switch( pcMsg->event ) 
	{
	case MTADP_MCU_REGISTER_REQ:
		MT_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMtAdpRegisterReq( pcMsg );
		break;
	case OSP_DISCONNECT:
		MT_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMtAdpDisconnect( pcMsg );
		break;
    
    case MTADP_MCU_GETMSSTATUS_REQ:
    case MCU_MSSTATE_EXCHANGE_NTF:
		MT_EV_LOG(LOG_LVL_KEYSTATUS);
        ProcMtAdpGetMsStatusReq( pcMsg );
        break;

	case MT_MCU_ROUNDTRIPDELAY_ACK:			//终端发给MCU的成功应答
	case MT_MCU_ROUNDTRIPDELAY_REQ:			//MT发给MCU的RoundTripDelay请求
		break;

	case MT_MCU_REGISTERGK_ACK:				//在GK上注册成功
	case MT_MCU_REGISTERGK_NACK:			//在GK上注册失败
	case MT_MCU_UNREGISTERGK_ACK:			//在GK上注销成功
	case MT_MCU_UNREGISTERGK_NACK:			//在GK上注销失败
	case MT_MCU_UPDATE_GKANDEPID_NTF:       //更新其他适配板的GK上注册的GatekeeperID/EndpointID信息的通知
		// [9/15/2011 liuxu] MT_MCU_UPDATE_GKANDEPID_NTF消息太多, 降低它的优先级
		if(pcMsg->event != MT_MCU_UPDATE_GKANDEPID_NTF)
		{
			MT_EV_LOG(LOG_LVL_KEYSTATUS);
		}else
		{
			MT_EV_LOG(LOG_LVL_DETAIL);
		}
		ProcMtAdpMcuGKMsg( pcMsg );
		break;

	case MCU_MCU_REGISTER_NOTIF:	//合并级联的注册消息
	case MCU_MCU_ROSTER_NOTIF:		//mcu合并级连式的花名册通知
	case MCU_MCU_MTLIST_REQ:		//请求对方MCU的终端列表 
	case MCU_MCU_MTLIST_ACK:		//请求对方MCU的终端列表的成功应答
	case MCU_MCU_MTLIST_NACK:		//请求对方MCU的终端列表的失败应答
	case MCU_MCU_VIDEOINFO_REQ:		//请求对方MCU的视频信息
	case MCU_MCU_VIDEOINFO_ACK:		//请求对方MCU的视频信息的成功应答
	case MCU_MCU_VIDEOINFO_NACK:	//请求对方MCU的视频信息的失败应答
	case MCU_MCU_CONFVIEWCHG_NOTIF:	//亩mcu-mcu会议视图发生变化通知   
	case MCU_MCU_AUDIOINFO_REQ:		//请求对方MCU的音频信息
	case MCU_MCU_AUDIOINFO_ACK:		//请求对方MCU的音频信息的成功应答
	case MCU_MCU_AUDIOINFO_NACK:	//请求对方MCU的音频信息的失败应答
		
	case MCU_MCU_INVITEMT_REQ:		//对方MCU指定本级邀请终端请求			
	case MCU_MCU_INVITEMT_ACK:		//对方MCU指定本级邀请终端成功应答
	case MCU_MCU_INVITEMT_NACK:		//对方MCU指定本级邀请终端失败应答
	case MCU_MCU_NEWMT_NOTIF:		//对方MCU通知本级其有新终端加入
	case MCU_MCU_REINVITEMT_REQ:	//对方MCU指定本级重邀终端请求
	case MCU_MCU_REINVITEMT_ACK:    //重邀终端请求的成功应答
	case MCU_MCU_REINVITEMT_NACK:   //重邀终端请求的失败应答
	case MCU_MCU_DROPMT_REQ:        //mcu-mcu挂断终端请求
	case MCU_MCU_DROPMT_ACK:        //mcu-mcu挂断终端请求成功应答
	case MCU_MCU_DROPMT_NACK:       //mcu-mcu挂断终端请求失败应答
	case MCU_MCU_DROPMT_NOTIF:		//mcu-mcu挂断终端通知

	case MCU_MCU_DELMT_REQ:         //mcu-mcu删除终端请求
	case MCU_MCU_DELMT_ACK:			//mcu-mcu删除终端成功应答
	case MCU_MCU_DELMT_NACK:		//mcu-mcu删除终端失败应答
	case MCU_MCU_DELMT_NOTIF:       //删除终端通知
		
	case MCU_MCU_SETMTCHAN_REQ:    //mcu-mcu请求设置媒体通道开关
	case MCU_MCU_SETMTCHAN_ACK:    //mcu-mcu请求设置媒体通道开关成功应答
	case MCU_MCU_SETMTCHAN_NACK:   //mcu-mcu请求设置媒体通道开关失败应答
	case MCU_MCU_SETMTCHAN_NOTIF:  //mcu-mcu媒体通道开关状态通知

	case MCU_MCU_CALLALERTING_NOTIF:	//正在呼叫通报
	
	case MCU_MCU_SETIN_REQ:         //mcu-mcu指定终端图像进入view
	case MCU_MCU_SETIN_ACK:         //mcu-mcu指定终端图像进入view 成功应答
	case MCU_MCU_SETIN_NACK:        //mcu-mcu指定终端图像进入view 失败应答

	case MCU_MCU_SETOUT_REQ:         //mcu-mcu指定view输入到终端
	case MCU_MCU_SETOUT_ACK:         //mcu-mcu指定view输入到终端成功应答
	case MCU_MCU_SETOUT_NACK:        //mcu-mcu指定view输入到终端失败应答
	case MCU_MCU_SETOUT_NOTIF:       //mcu-mcu指定view输入到终端通知

// [pengjie 2010/8/12]
	case MCU_MCU_FECC_CMD:			//跨MCU进行摄像头远遥

	case MCU_MCU_SENDMSG_NOTIF:     //mcu-mcu短消息
	case MCU_MCU_STARTMIXER_CMD:    //mcu-mcu
	case MCU_MCU_STARTMIXER_NOTIF:  //mcu-mcu
	case MCU_MCU_STOPMIXER_CMD:     //mcu-mcu
	case MCU_MCU_STOPMIXER_NOTIF:   //mcu-mcu
	case MCU_MCU_GETMIXERPARAM_REQ: //mcu-mcu
	case MCU_MCU_GETMIXERPARAM_ACK: //mcu-mcu
	case MCU_MCU_GETMIXERPARAM_NACK: //mcu-mcu
	case MCU_MCU_MIXERPARAM_NOTIF:  //mcu-mcu
	case MCU_MCU_ADDMIXMEMBER_CMD:  //mcu-mcu
	case MCU_MCU_REMOVEMIXMEMBER_CMD:  //mcu-mcu

	case MCU_MCU_LOCK_REQ:            //mcu-mcu 请求锁定对方MCU
	case MCU_MCU_LOCK_ACK:            //mcu-mcu 请求锁定对方MCU成功应答
	case MCU_MCU_LOCK_NACK:           //mcu-mcu 请求锁定对方MCU失败应答
		
	case MCU_MCU_MTSTATUS_CMD:        //mcu-mcu 请求对方MCU某一终端状态
	case MCU_MCU_MTSTATUS_NOTIF:      //mcu-mcu 通知对方MCU自己某一所在直连终端状态
        
    case MCU_MCU_AUTOSWITCH_REQ:      //RadMcu->KedaMcu 级联轮询请求
    case MCU_MCU_AUTOSWITCH_ACK:      //KedaMcu->RadMcu 级联轮询成功应答
    case MCU_MCU_AUTOSWITCH_NACK:     //KedaMcu->RadMcu 级联轮询失败应答

	case MCU_MCU_ADJMTRES_REQ:		  //级联调MT视频分辨率 (非标)
	case MCU_MCU_ADJMTRES_ACK:
	case MCU_MCU_ADJMTRES_NACK:

// [pengjie 2010/6/19] 级联降帧率 (非标)
	case MCU_MCU_ADJMTFPS_REQ:        
	case MCU_MCU_ADJMTFPS_ACK:
	case MCU_MCU_ADJMTFPS_NACK:

	//zjj20100201
	//  pengjie[9/29/2009] 级联多回传消息
	case MCU_MCU_MULTSPYCAP_NOTIF:
	case MCU_MCU_PRESETIN_REQ:
	case MCU_MCU_PRESETIN_ACK:
	case MCU_MCU_PRESETIN_NACK:
	case MCU_MCU_SPYCHNNL_NOTIF:
	case MCU_MCU_SWITCHTOMCUFAIL_NOTIF:
	case MCU_MCU_REJECTSPY_NOTIF:
	case MCU_MCU_BANISHSPY_CMD:
	case MCU_MCU_MTEXTINFO_NOTIF:
	//lukunpeng 2010/06/10 由于现在是由上级管理下级带宽占用情况，故不需要再通过下级通知上级
	//case MCU_MCU_BANISHSPY_NOTIF:
	case MCU_MCU_SPYFASTUPDATEPIC_CMD:

    case MCU_MCU_APPLYSPEAKER_REQ:      //跨级申请发言人支持
    case MCU_MCU_APPLYSPEAKER_ACK:
    case MCU_MCU_APPLYSPEAKER_NACK:
    case MCU_MCU_APPLYSPEAKER_NOTIF:
	case MCU_MCU_SPEAKSTATUS_NTF:
	case MCU_MCU_SPEAKERMODE_NOTIFY:
    case MCU_MCU_CANCELMESPEAKER_REQ:
    case MCU_MCU_CANCELMESPEAKER_ACK:
    case MCU_MCU_CANCELMESPEAKER_NACK:
	
// [8/6/2010 xliang] mt mute and dumb operation
	case MCU_MCU_MTAUDMUTE_REQ:
	case MCU_MCU_MTAUDMUTE_ACK:
	case MCU_MCU_MTAUDMUTE_NACK:

	case VCS_MCU_MUTE_REQ:					// [9/6/2010 xliang] vcs 远端静音哑音透传
    case MCU_MCU_CHANGEMTSECVIDSEND_CMD:    //[5/4/2011 zhushengze]VCS控制发言人发双流
    case MCU_MCU_TRANSPARENTMSG_NOTIFY:     //[2/23/2012 zhushengze]界面、终端消息透传
	case MCU_MCU_ADJMTBITRATE_CMD:
		MT_EV_LOG(LOG_LVL_DETAIL);
		ProcMcuCasCadeMsg( pcMsg );
		break;	
	
	case MT_MCU_CREATECONF_REQ:				//终端发给MCU的创建会议请求
    case MT_MCU_LOWERCALLEDIN_NTF:          // guzh [6/19/2007] 下级MCU主动呼叫
	case MT_MCU_DROPCONF_CMD:				//主席终端发给MCU的中止会议请求
	case MT_MCU_INVITEMT_ACK:				//MT接受应答，MT->MCU
	case MT_MCU_INVITEMT_NACK:				//MT拒绝应答，MT->MCU
	case MT_MCU_DELMT_REQ:					//主席终端发给MCU的强制终端退出请求
	case MT_MCU_APPLYCHAIRMAN_REQ:			//终端发给MCU的申请主席请求
	case MT_MCU_CANCELCHAIRMAN_REQ:			//主席终端发送给MCU的取消主席的请求
	case MT_MCU_GETCHAIRMAN_REQ:			//查询具有主席权限的终端ID
	case MT_MCU_SPECSPEAKER_REQ:			//主席终端发给MCU的指定发言人请求
	case MT_MCU_SPECSPEAKER_CMD:			//主席终端发给MCU的取消发言人请求
	case MT_MCU_CANCELSPEAKER_CMD:
	case MT_MCU_APPLYSPEAKER_NOTIF:			//终端发给MCU的申请发言请求
	case MT_MCU_STARTSELMT_CMD:				//主席终端发给MCU的选看终端命令
	case MT_MCU_STOPSELMT_CMD:				//主席终端发给MCU的停止选看终端命令
	case MT_MCU_STARTSELMT_REQ:
	case MT_MCU_OPENLOGICCHNNL_ACK:			//终端发给MCU的成功应答
	case MT_MCU_OPENLOGICCHNNL_NACK:		//终端发给MCU的失败应答
	case MT_MCU_OPENLOGICCHNNL_REQ:			//MT发给MCU的打开逻辑通道请求
	case MT_MCU_CLOSELOGICCHNNL_NOTIF:		//MT发给MCU的关闭逻辑通道请求
	case MT_MCU_LOGICCHNNLOPENED_NTF:       //终端（协议栈）发给MCU的通道成功建立的通知

	case MT_MCU_MEDIALOOPON_REQ:
	case MT_MCU_MEDIALOOPOFF_CMD:
	case MT_MCU_FREEZEPIC_CMD:
	case MT_MCU_FASTUPDATEPIC_CMD:
	case MT_MCU_FLOWCONTROL_CMD:			//码流控制命令
	case MT_MCU_FLOWCONTROLINDICATION_NOTIF://码流控制指示
	case MT_MCU_CAPBILITYSET_NOTIF:			//终端发给MCU的能力集通知
	case MT_MCU_MSD_NOTIF:					//终端适配发给业务的主从确定结果的通知
	case MT_MCU_JOINEDMTLIST_REQ:			//终端发给MCU的会议与会终端列表查询请求
	case MT_MCU_JOINEDMTLISTID_REQ:			//终端发给MCU的会议与会终端列表及其ID查询请求
	case MT_MCU_GETMTALIAS_REQ:				//查询特定终端别名
	case MT_MCU_MTALIAS_NOTIF:				//终端汇报别名
	case MT_MCU_DELAYCONF_REQ:				//消息体为u16 分钟
	case MT_MCU_GETMTSTATUS_REQ:			//终端发给MCU的查询会议中某个终端状态请求
	case MT_MCU_GETMTSTATUS_ACK:			//MT接受应答，MT->MCU
	case MT_MCU_GETMTSTATUS_NACK:			//MT拒绝应答，MT->MCU
	case MT_MCU_MTSTATUS_NOTIF:				//MT状态改变时主动发给MCU状态消息
	case MT_MCU_GETCONFINFO_REQ:			//终端发给MCU的查询会议完整信息请求
	case MT_MCU_MTCONNECTED_NOTIF:			//和终端建立连接成功通知
	case MT_MCU_MTJOINCONF_NOTIF:			//终端成功入会通知
	case MT_MCU_MTTYPE_NOTIF:               //终端类型通知（320接入时的额外通知）
	case MT_MCU_MTDISCONNECTED_NOTIF:		//和终端连接断开通知
	case MT_MCU_SPECCHAIRMAN_REQ:			//主席终端发给MCU的指定主席请求
	case MT_MCU_STARTMTSELME_REQ:			//终端发给MCU的其它终端选看自己的请求
	case MT_MCU_STOPMTSELME_CMD:			//终端发给MCU的停止其它终端选看自己的通知
	case MT_MCU_STARTBROADCASTMT_REQ:		//终端发给MCU的请求MCU组播其它终端的命令
	case MT_MCU_STOPBROADCASTMT_CMD:		//终端发给MCU的停止MCU组播其它终端的命令
	case MT_MCU_GETMTSELSTUTS_REQ:			//终端发给MCU的查询其它终端选看情况的请求
	case MT_MCU_MTMUTE_CMD:					//主席终端命令MCU进行终端静音设置
	case MT_MCU_MTDUMB_CMD:					//主席终端命令MCU进行终端哑音设置
	case MT_MCU_MTCAMERA_CTRL_CMD:			//发给终端摄像机镜头运动命令
	case MT_MCU_MTCAMERA_CTRL_STOP:			//主席命令终端摄像头停止移动
	case MT_MCU_MTCAMERA_RCENABLE_CMD:		//发给终端摄像机遥控器使能命令
	case MT_MCU_MTCAMERA_SAVETOPOS_CMD:		//发给终端摄像机要求将当前位置信息存入指定位置
	case MT_MCU_MTCAMERA_MOVETOPOS_CMD:		//发给终端摄像机要求调整到指定位置
	// libo [4/4/2005]VideoSource
	case MT_MCU_SELECTVIDEOSOURCE_CMD:           //MT要求MCU设置终端视频源
	case MT_MCU_VIDEOSOURCESWITCHED_CMD:
	// libo [4/4/2005]end
   	case MT_MCU_STARTVAC_REQ:				//主席请求MCU开始语音激励控制发言		
   	case MT_MCU_STOPVAC_REQ:				//主席请求MCU停止语音激励控制发言
	case MT_MCU_STARTDISCUSS_REQ:			//开始会议讨论请求
	case MT_MCU_STOPDISCUSS_REQ:			//主席请求MCU结束会议讨论
	case MT_MCU_STARTVMP_REQ:				//开始视频复合请求
	case MT_MCU_STOPVMP_REQ:				//结束视频复合请求
	case MT_MCU_CHANGEVMPPARAM_REQ:			//主席请求MCU改变视频复合参数
	case MT_MCU_GETVMPPARAM_REQ:			//查询视频复合成员请求	
	case MT_MCU_STARTVMPBRDST_REQ:			//主席命令MCU开始把画面合成图像广播到终端
	case MT_MCU_STOPVMPBRDST_REQ:			//主席命令MCU停止把画面合成图像广播到终端
	case MT_MCU_STARTSWITCHVMPMT_REQ:		//主席选看VMP
	case MT_MCU_GETMTPOLLPARAM_ACK:			//Mt应答MCU查询终端轮询参数  
	case MT_MCU_GETMTPOLLPARAM_NACK:		//MT拒绝MCU查询终端轮询参数   
	case MT_MCU_POLLMTSTATE_NOTIF:			//MT通知MCU当前的终端轮询状态
	case MT_MCU_ADDMT_REQ:					//主席终端发给MCU的邀请终端请求
	case MT_MCU_SENDMSG_CMD:				//终端至终端短消息命令
	case MT_MCU_SENDMCMSG_CMD:				//终端至会议控制台短消息命令	
    case MT_MCU_GETBITRATEINFO_ACK:         //mcu获取终端码率应答
    case MT_MCU_GETBITRATEINFO_NACK:
    case MT_MCU_GETBITRATEINFO_NOTIF:       
    case MT_MCU_MATRIX_ALLSCHEMES_NOTIF:    //内置矩阵
    case MT_MCU_MATRIX_ONESCHEME_NOTIF:
    case MT_MCU_MATRIX_SAVESCHEME_NOTIF:
    case MT_MCU_MATRIX_SETCURSCHEME_NOTIF:
    case MT_MCU_MATRIX_CURSCHEME_NOTIF:
    case MT_MCU_EXMATRIXINFO_NOTIFY:        //外置矩阵      
    case MT_MCU_EXMATRIX_GETPORT_ACK:        
    case MT_MCU_EXMATRIX_GETPORT_NACK:       
    case MT_MCU_EXMATRIX_GETPORT_NOTIF:
    case MT_MCU_EXMATRIX_PORTNAME_NOTIF:        
    case MT_MCU_EXMATRIX_ALLPORTNAME_NOTIF:   
    case MT_MCU_ALLVIDEOSOURCEINFO_NOTIF:   //扩展视频源
    case MT_MCU_VIDEOSOURCEINFO_NOTIF:
	case MT_MCU_ADDMIXMEMBER_CMD:           //主席增加混音终端
	case MT_MCU_REMOVEMIXMEMBER_CMD:		//主席移除混音终端 
    case MT_MCU_VIEWBRAODCASTINGSRC_CMD:    //主席终端强制广播
	case MT_MCU_APPLYMIX_NOTIF:             //终端发给MCU的申请参加混音请求
	case MT_MCU_CALLMTFAILURE_NOTIFY:       //终端给MCU的呼叫失败通知
	case MT_MCU_GETH239TOKEN_REQ:           //终端给MCU的 获取 H239令牌 权限请求
	case MT_MCU_OWNH239TOKEN_NOTIF:         //终端给MCU的 拥有 H239令牌 权限通知
	case MT_MCU_RELEASEH239TOKEN_NOTIF:     //终端给MCU的 释放 H239令牌 权限通知
    case MT_MCU_BANDWIDTH_NOTIF:            //终端带宽指示通知

    case MT_MCU_CONF_STOPCHARGE_ACK:        //在GK上停止计费成功
    case MT_MCU_CONF_STOPCHARGE_NACK:       //在GK上停止计费失败
    case MT_MCU_CONF_STARTCHARGE_ACK:       //在GK上开始计费成功
    case MT_MCU_CONF_STARTCHARGE_NACK:      //在GK上开始计费失败
	case MT_MCU_CONF_CHARGEEXP_NOTIF:		//在GK上计费的会议发生异常通知
    case MT_MCU_CHARGE_REGGK_NOTIF:         //向GK注册状态通知

//    case MT_MCU_GETMTVERID_ACK:             //获取终端版本号成功应答
    case MT_MCU_GETMTVERID_NACK:            //获取终端版本号失败应答

    case MT_MCU_CALLFAIL_HDIFULL_NOTIF:

	case POLY_MCU_GETH239TOKEN_ACK:			//获取PolyMCU的H239TOKEN 同意应答
	case POLY_MCU_GETH239TOKEN_NACK:		//获取PolyMCU的H239TOKEN 拒绝应答
	case POLY_MCU_OWNH239TOKEN_NOTIF:		//PolyMCU通知当前的TOKEN的拥有者
	case POLY_MCU_RELEASEH239TOKEN_CMD:		//PolyMCU释放H329TOKEN 命令
		
	case MT_MCU_RELEASEMT_REQ:
    case MT_MCU_APPLYCANCELSPEAKER_REQ:
	case MT_MCU_MTVIDEOALIAS_NOTIF:         //终端视频源别名通知
    case MT_MCU_TRANSPARENTMSG_NOTIFY:
	case MTADP_MCU_GETMTLIST_NOTIF:
	case MTADP_MCU_GETSMCUMTALIAS_NOTIF:	//获得下级mcu终端别名通知

	case REC_MCU_STARTREC_ACK:				//开始录像Ack
	case REC_MCU_STARTREC_NACK:				//开始录像Nack
	case REC_MCU_RECORDCHNSTATUS_NOTIF:		// 录像状态上报
	case REC_MCU_RECORDPROG_NOTIF:			//录像进度上报
	case REC_MCU_LISTALLRECORD_ACK:			//获取文件列表Ack
	case REC_MCU_LISTALLRECORD_NACK:		//获取文件列表Nack
	case REC_MCU_FILEGROUPLIST_NOTIF:		// 节目组列表上报
	case REC_MCU_LISTALLRECORD_NOTIF:		// 文件名列表上报
	case REC_MCU_STARTPLAY_ACK:				//开始录像Ack
	case REC_MCU_STARTPLAY_NACK:			//开始录像Nack
	case REC_MCU_PLAYCHNSTATUS_NOTIF:		// 放像状态上报
	case REC_MCU_PLAYPROG_NOTIF:			//放像进度上报
	case REC_MCU_SEEK_ACK:					// 放像进度拖动Ack
	case REC_MCU_SEEK_NACK:				// 放像进度拖动Nack

		ProcMtAdpMcuMsg( pcMsg );
		MT_EV_LOG(LOG_LVL_DETAIL);
		break;

	case MCU_MT_CREATECONF_ACK:				//MCU发给MT的创建会议应答
	case MCU_MT_CREATECONF_NACK:			//MCU拒绝MT的创建会议请求
	case MCU_MT_INVITEMT_REQ:				//MCU邀请终端加入指定会议请求
	case MCU_MT_DELMT_CMD:					//MCU强制终端退出会议命令
	case MCU_MT_DELMT_ACK:					//MCU回的成功应答
	case MCU_MT_DELMT_NACK:					//MCU回的失败应答
	case MCU_MT_MTJOINED_NOTIF:				//终端加入会议通知
	case MCU_MT_MTLEFT_NOTIF:				//终端离开会议通知
	case MCU_MT_APPLYCHAIRMAN_ACK:			//终端发给MCU的申请主席请求成功应答
	case MCU_MT_APPLYCHAIRMAN_NACK:			//终端发给MCU的申请主席请求失败应答
	case MCU_MT_CANCELCHAIRMAN_NOTIF:		//MCU发给终端的取消主席请求
	case MCU_MT_CANCELCHAIRMAN_ACK:			//MCU->MT
	case MCU_MT_CANCELCHAIRMAN_NACK:
	case MCU_MT_GETCHAIRMAN_ACK:			//查询主席ID应答
	case MCU_MT_GETCHAIRMAN_NACK:
	case MCU_MT_SPECSPEAKER_ACK:			//MCU回的成功应答
	case MCU_MT_SPECSPEAKER_NACK:			//MCU回的失败应答
	case MCU_MT_SPECSPEAKER_NOTIF:			//MCU发给终端的指定发言通知
	case MCU_MT_CANCELSPEAKER_NOTIF:		//MCU发给终端的取消发言请求
	case MCU_MT_APPLYSPEAKER_NOTIF:			//通知主席终端申请发言请求
	case MCU_MT_STARTSELMT_ACK:
	case MCU_MT_STARTSELMT_NACK:
	case MCU_MT_YOUARESEEING_NOTIF:			//MCU告知终端当前视频源
	case MCU_MT_SEENBYOTHER_NOTIF:			//MCU通知终端被选看
	case MCU_MT_CANCELSEENBYOTHER_NOTIF:	//MCU通知终端被取消选看
	case MCU_MT_OPENLOGICCHNNL_REQ:			//MCU发给MT的打开逻辑通道请求
	case MCU_MT_OPENLOGICCHNNL_ACK:			//MCU发给终端的成功应答
	case MCU_MT_OPENLOGICCHNNL_NACK:		//MCU发给终端的失败应答
	case MCU_MT_CLOSELOGICCHNNL_CMD:		//MCU发给MT的关闭逻辑通道请求
	case MCU_MT_CHANNELON_CMD:				//MCU发给终端的ChannelOn消息
	case MCU_MT_CHANNELOFF_CMD:				//MCU发给终端的ChannelOff消息
	case MCU_MT_MEDIALOOPON_ACK:			//终端远端环回ack
	case MCU_MT_MEDIALOOPON_NACK:			//终端远端环回nack
	case MCU_MT_FREEZEPIC_CMD:				//MCU发给终端冻结图像命令
	case MCU_MT_FASTUPDATEPIC_CMD:			//MCU发给终端快速更新图像命令
	case MCU_MT_FLOWCONTROL_CMD:
	case MCU_MT_FLOWCONTROLINDICATION_NOTIF:
    case MCU_MT_GETBITRATEINFO_REQ:         //mcu请求终端码率信息
	case MCU_MT_ROUNDTRIPDELAY_REQ:			//MCU发给MT的RoundTripDelay请求
	case MCU_MT_ROUNDTRIPDELAY_ACK:			//MCU发给终端的成功应答
	case MCU_MT_CAPBILITYSET_NOTIF:			//MCU发给终端的能力集通知
	case MCU_MT_JOINEDMTLIST_ACK:			//MCU返回成功结果
	case MCU_MT_JOINEDMTLIST_NACK:			//MCU返回失败结果
	case MCU_MT_JOINEDMTLISTID_ACK:			//MCU返回成功结果
	case MCU_MT_JOINEDMTLISTID_NACK:
	case MCU_MT_GETMTALIAS_ACK:				//查询特定终端别名应答，消息体TMt+TMtAlias
	case MCU_MT_GETMTALIAS_NACK:
	case MCU_MT_DELAYCONF_ACK:
	case MCU_MT_DELAYCONF_NACK:
	case MCU_MT_DELAYCONF_NOTIF:			//消息体为u16 分钟
	case MCU_MT_CONFWILLEND_NOTIF:
	case MCU_MT_GETMTSTATUS_ACK:			//MCU回的成功应答
	case MCU_MT_GETMTSTATUS_NACK:			//MCU回的失败应答
	case MCU_MT_GETMTSTATUS_REQ:			//MCU发给终端查询状态请求
	case MCU_MT_GETCONFINFO_ACK:			//MCU回的成功应答
	case MCU_MT_GETCONFINFO_NACK:			//MCU回的失败应答
	case MCU_MT_CONF_NOTIF:					//会议信息通知
	case MCU_MT_SIMPLECONF_NOTIF:           //简化会议信息通知 2005-10-11
	case MCU_MT_SPECCHAIRMAN_ACK:			//MCU回的成功应答
	case MCU_MT_SPECCHAIRMAN_NACK:			//MCU回的失败应答
	case MCU_MT_APPLYCHAIRMAN_NOTIF:		//通知主席终端申请主席请求
    case MCU_MT_APPLYCHAIRMAN_CMD:			//MCU发给中兴MCU申请主席的通知(适应远遥需求特殊处理)
	case MCU_MT_CANCELCHAIRMAN_CMD:			//mcu发给中兴mcu取消中兴请求
	case MCU_MT_SPECCHAIRMAN_NOTIF:			//MCU发给终端的指定主席通知
	case MCU_MT_STARTMTSELME_ACK:			//MCU发给终端的其它终端选看自己的请求应答
	case MCU_MT_STARTMTSELME_NACK:			//MCU发给终端的其它终端选看自己的拒绝
	case MCU_MT_GETMTSELSTUTS_ACK:			//MCU发给终端的查询其它终端选看情况的应答
	case MCU_MT_GETMTSELSTUTS_NACK:			//MCU发给终端的查询其它终端选看情况的的拒绝
	case MCU_MT_MTMUTE_CMD:					//MCU发给终端某一会议实例对终端静音设置
	case MCU_MT_MTDUMB_CMD:					//MCU发给终端某一会议实例对终端进行哑音设置
	case MCU_MT_MTCAMERA_CTRL_CMD:			//发给终端摄像机镜头运动命令
	case MCU_MT_MTCAMERA_CTRL_STOP:			//发给终端摄像机镜头停止运动命令
	case MCU_MT_MTCAMERA_RCENABLE_CMD:		//发给终端摄像机遥控器使能命令
	case MCU_MT_MTCAMERA_SAVETOPOS_CMD:		//发给终端摄像机要求将当前位置信息存入指定位置
	case MCU_MT_MTCAMERA_MOVETOPOS_CMD:		//发给终端摄像机要求调整到指定位置
	case MCU_MT_SETMTVIDSRC_CMD:            //发给终端的选择视频源命令
    
	case MCU_MT_VIDEOSOURCESWITCHED_CMD:

  	case MCU_MT_STARTVAC_ACK:				//MCU同意主席开始语音激励控制发言的请求   
   	case MCU_MT_STARTVAC_NACK:				//MCU拒绝主席开始语音激励控制发言的请求   
   	case MCU_MT_STARTVAC_NOTIF:				//MCU给主席开始语音激励控制发言的通知
   	case MCU_MT_STOPVAC_ACK:				//MCU同意主席停止语音激励控制发言的请求  
   	case MCU_MT_STOPVAC_NACK:				//MCU拒绝主席停止语音激励控制发言的请求  
   	case MCU_MT_STOPVAC_NOTIF:				//MCU给主席停止语音激励控制发言的通知
	case MCU_MT_STARTDISCUSS_ACK:			//MCU同意开始会议讨论	
	case MCU_MT_STARTDISCUSS_NACK:			//MCU拒绝开始会议讨论	
	case MCU_MT_STARTDISCUSS_NOTIF:			//MCU开始会议讨论通知	
	case MCU_MT_STOPDISCUSS_ACK:			//MCU同意主席结束会议讨论的请求	
	case MCU_MT_STOPDISCUSS_NACK:			//MCU拒绝主席结束会议讨论的请求	
	case MCU_MT_STOPDISCUSS_NOTIF:			//MCU给主席结束会议讨论的通知	
	case MCU_MT_STARTVMP_ACK:				//MCU同意视频复合请求	
	case MCU_MT_STARTVMP_NACK:				//MCU不同意视频复合请求	
	case MCU_MT_STARTVMP_NOTIF:				//MCU开始视频复合的通知	
	case MCU_MT_STOPVMP_ACK:				//MCU同意视频结束复合请求	
	case MCU_MT_STOPVMP_NACK:				//MCU不同意结束视频复合请求	
	case MCU_MT_STOPVMP_NOTIF:				//MCU结束视频复合的通知	
	case MCU_MT_CHANGEVMPPARAM_ACK:			//MCU同意主席的改变视频复合参数请求	
	case MCU_MT_CHANGEVMPPARAM_NACK:		//MCU拒绝主席的改变视频复合参数请求	
	case MCU_MT_GETVMPPARAM_ACK:			//MCU同意查询视频复合成员	
	case MCU_MT_GETVMPPARAM_NACK:			//MCU不同意查询视频复合成员	
	case MCU_MT_VMPPARAM_NOTIF:				//MCU给主席的视频复合参数通知	
	case MCU_MT_STARTVMPBRDST_ACK:			//MCU同意广播视频复合码流, 消息体无	
	case MCU_MT_STARTVMPBRDST_NACK:			//MCU拒绝广播视频复合码流, 消息体无	
	case MCU_MT_STOPVMPBRDST_ACK:			//MCU同意停止广播视频复合码流, 消息体无	
	case MCU_MT_STOPVMPBRDST_NACK:			//MCU拒绝停止广播视频复合码流, 消息体无
	case MCU_MT_STARTMTPOLL_CMD:			//MCU命令该终端开始轮询选看  
	case MCU_MT_STOPMTPOLL_CMD:				//MCU命令该终端停止轮询选看  
	case MCU_MT_PAUSEMTPOLL_CMD:			//MCU命令该终端暂停轮询选看   
	case MCU_MT_RESUMEMTPOLL_CMD:			//MCU命令该终端继续轮询选看
	case MCU_MT_GETMTPOLLPARAM_REQ:			//MCU向终端查询轮询参数
	case MCU_MT_ADDMT_ACK:					//MCU回的成功应答
	case MCU_MT_ADDMT_NACK:					//MCU回的失败应答
	case MCU_MT_SENDMSG_NOTIF:				//至终端短消息通知
	case MCU_MT_CHANGESPEAKER_NOTIF:        //MCU通知终端发言人改
	case MCU_MT_CHANGECHAIRMAN_NOTIF:		//MCU通知终端主席改变
	case MCU_MT_REGISTERGK_REQ:             //在GK上注册
	case MCU_MT_UNREGISTERGK_REQ:           //在GK上注销	
	case MCU_MT_MATRIX_SAVESCHEME_CMD:		//保存矩阵方案请求
    case MCU_MT_MATRIX_GETALLSCHEMES_CMD:
    case MCU_MT_MATRIX_GETONESCHEME_CMD:
    case MCU_MT_MATRIX_SETCURSCHEME_CMD:
    case MCU_MT_MATRIX_GETCURSCHEME_CMD:
    case MCU_MT_EXMATRIX_GETINFO_CMD:
    case MCU_MT_EXMATRIX_SETPORT_CMD:
    case MCU_MT_EXMATRIX_GETPORT_REQ:
    case MCU_MT_EXMATRIX_SETPORTNAME_CMD:
    case MCU_MT_EXMATRIX_GETALLPORTNAME_CMD:        
    case MCU_MT_GETVIDEOSOURCEINFO_CMD:    //终端扩展视频源   
    case MCU_MT_SETVIDEOSOURCEINFO_CMD:       
    case MCU_MT_SELECTEXVIDEOSRC_CMD:
	case MCU_MT_STARTBROADCASTMT_ACK:       //MCU发给终端的组播其它终端的响应
	case MCU_MT_STARTBROADCASTMT_NACK:      //MCU发给终端的组播其它终端的响应
	case MCU_MT_MTSOURCENAME_NOTIF:         //终端视频源名称指示
	case MCU_MT_MTAPPLYMIX_NOTIF:           //通知主席终端其它终端申请参加混音请求
	case MCU_MT_MTADDMIX_NOTIF:             //通知终端已成功参加混音
	case MCU_MT_DISCUSSPARAM_NOTIF:			//通知终端定制混音状态 // [11/25/2010 xliang] for MT of T3, R3 type
	case MCU_MT_GETH239TOKEN_ACK:           //MCU给终端的 获取H239令牌权限请求的 回应通知
	case MCU_MT_GETH239TOKEN_NACK:          //
	case MCU_MT_RELEASEH239TOKEN_CMD:       //MCU给终端的 释放H239令牌 权限命令
	case MCU_MT_OWNH239TOKEN_NOTIF:         //MCU给终端的 拥有 H239令牌 权限通知
	case MCU_MT_CONFPWD_NTF:                //MCU同步终端适配层该会议的会议密码，用于合并级联请求的认证校验
        
//    case MCU_MT_SATDCONFADDR_NOTIF:       //卫星会议组播地址通知
    case MCU_MT_SATDCONFCHGADDR_CMD:        //卫星会议终端接收地址改变命令
    case MCU_MT_VIDEOPARAMCHANGE_CMD:       //终端视频参数调整 xliang [080731]
	case MCU_MT_VIDEOFPSCHANGE_CMD:         //终端帧率调整 pengjie [20100422]

    case MCU_MT_CONF_STARTCHARGE_REQ:       //会控开始计费请求
    case MCU_MT_CONF_STOPCHARGE_REQ:        //会控结束计费请求

    case MCU_MT_UNREGGK_NPLUS_CMD:          //N+1备份下，备份MCU注销主MCU实体
    case MCU_MT_CONF_STOPCHARGE_CMD:        //N+1备份下，备份MCU停止主MCU会议计费
    case MCU_MT_SETMTVOLUME_CMD:            //设置终端音量请求 add by wangliang 2006/12/26
    case MCU_MT_RESTORE_MCUE164_NTF:        //N+1备份下，备份MCU注销 假MCU.E164
        
    case MCU_MT_SOMEOFFERRING_NOTIF:        //MCU发给终端的 通用通知
    case MCU_MT_GETMTVERID_REQ:             //MCU发给终端的获取终端版本的请求
	
	case MCU_POLY_GETH239TOKEN_REQ:			//KEDAMCU与PolyMCU级联，获取PolyMCU的H239TOKEN请求
	case MCU_POLY_OWNH239TOKEN_NOTIF:		//KEDAMCU与PolyMCU级联，通知PolyMCU H239TOKEN为当前MCU所有
	case MCU_POLY_RELEASEH239TOKEN_CMD:		//KEDAMCU与PolyMCU级联，KEDAMCU停止双流的 命令

	case MCU_MT_STARTSWITCHVMPMT_ACK:		//主席选看VMP应答
	case MCU_MT_STARTSWITCHVMPMT_NACK:

	case MCU_MT_RELEASEMT_ACK:
	case MCU_MT_RELEASEMT_NACK:

	case MCU_MT_MMCUCONFNAMESHOWTYPE_CMD:
	case MCU_MT_CHANGEADMINLEVEL_CMD:

	case MCU_MT_MTSPEAKSTATUS_NTF:          //MCU发给终端 发言人状态
    case MCU_MT_CONFSPEAKMODE_NTF:          //MCU发给终端 会议发言模式 通知

    case MCU_MT_APPLYCANCELSPEAKER_ACK:
    case MCU_MT_APPLYCANCELSPEAKER_NACK:

	case MCU_MT_GETMTVIDEOALIAS_CMD:        //获取终端视频源别名信息
	case MCU_MTADP_GETMTLIST_CMD:
	case MCU_MTADP_GETSMCUMTALIAS_CMD:		//追加获得下级指定终端别名内容

    case MCU_MT_CHANGEMTSECVIDSEND_CMD:     //[5/4/2011 zhushengze]VCS控制发言人发双流
    case MCU_MT_TRANSPARENTMSG_NOTIFY:      //[2/23/2012 zhushengze]界面、终端消息透传
	// vrs新录播相关消息支持
	case MCU_REC_STARTREC_REQ:				//开始录像
	case MCU_REC_LISTALLRECORD_REQ:			//获取文件列表
	case MCU_REC_STARTPLAY_REQ:				//开启放像
	case MCU_REC_SEEK_REQ:					//放像进度拖动
	case MCU_REC_GETRECORDCHNSTATUS_REQ:	//MCU请求获得录像机录像状态请求
	case MCU_REC_GETPLAYCHNSTATUS_REQ:		//MCU请求获得录像机放像状态请求
	case MCU_REC_GETRECPROG_CMD:			//获取录相进度
	case MCU_REC_GETPLAYPROG_CMD:			//获取放像进度

		MT_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcuMtAdpMsg( pcMsg );
		break;

	case MCU_MT_ENTERPASSWORD_REQ:			//MCU请求终端输入密码		
	case MT_MCU_ENTERPASSWORD_ACK:			//终端回应密码
	case MT_MCU_ENTERPASSWORD_NACK:
		{
			MT_EV_LOG(LOG_LVL_DETAIL);
			if ( AID_MCU_MTADP == GETAPP( pcMsg->srcid ) )	
			{
				//当mcu设置了会议密码时要求直连mcu进行密码回应
				ProcMtAdpMcuMsg( pcMsg );
			}
			else
			{
				//当下级mcu设置了会议密码时要求上级mcu进行密码回应
				ProcMcuMtAdpMsg( pcMsg );
			}
			break;	
		}

//	case MCU_MT_REREGISTERGK_REQ:           //原有GK注册的323适配板掉线后，重新采用一块323适配板注册
	case MCU_MT_UPDATE_REGGKSTATUS_NTF:     //更新其他适配板的MCU及会议实体的GK注册信息的通知
	case MCU_MT_UPDATE_GKANDEPID_NTF:       //更新其他适配板的GK上注册的GatekeeperID/EndpointID信息的通知
	case MCU_MTADP_GKADDR_UPDATE_CMD:		//GK地址配置更改
		MT_EV_LOG(LOG_LVL_KEYSTATUS);
		ProcMcuMtAdpGKMsg( pcMsg );
		break;
        
    case MCU_MT_MTADPCFG_CMD:        
    case MCU_MT_SETQOS_CMD:
        {
			MT_EV_LOG(LOG_LVL_KEYSTATUS);
            CServMsg cMsg(pcMsg->content, pcMsg->length);
            SendMsgToH323MtAdp(cMsg);
        }        
        break;    
		
	default:
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "MtAdpSsn[InstanceEntry]: Wrong message %u(%s) received in InstanceEntry()!\n", pcMsg->event, 
			::OspEventDesc( pcMsg->event ) );
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
void CMtAdpSsnInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	if (NULL == pcMsg)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "MtAdpSsn[DaemonInstanceEntry]: The received msg's pointer in the msg entry is NULL!");
		return;
	}

	switch (pcMsg->event)
	{
	case MCU_APPTASKTEST_REQ:			//GUARD Probe Message
		DaemonProcAppTaskRequest( pcMsg );
		break;
	default:
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "MtAdpSsn[DaemonInstanceEntry]: Wrong message %u(%s) received! AppId = %u\n", pcMsg->event, 
			::OspEventDesc( pcMsg->event ),pcApp!=NULL?pcApp->appId:0 );
		break;
	}
	
	return;
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
void CMtAdpSsnInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );

	return;
}

/*====================================================================
    函数名      : SendMsgToMtAdp
    功能        ：发消息给MtAdp
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
void CMtAdpSsnInst::SendMsgToMtAdp(u16 wEvent, u8 * const pbyMsg, u16 wLen)
{
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MT, "[SendMsgToMtAdp]: Send Msg %u(%s) to MTAdpId%u Protocol%u MaxMtNum%u m_dwMtAdpAppIId.%d m_dwMtAdpNode.%d!\n", 
              wEvent, ::OspEventDesc(wEvent), m_byMtAdpId, m_byProtocolType, m_byMaxMtNum, m_dwMtAdpAppIId, m_dwMtAdpNode);

    post(m_dwMtAdpAppIId, wEvent, pbyMsg, wLen, m_dwMtAdpNode);
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
void CMtAdpSsnInst::SendMsgToMcu( u16 wEvent, u8 * const pbyMsg, u16 wLen )
{
    CServMsg cServMsg( pbyMsg, wLen );

	if( wEvent == MT_MCU_CREATECONF_REQ ||
        wEvent == MT_MCU_LOWERCALLEDIN_NTF ||
		wEvent == MTADP_MCU_REGISTER_REQ ||
		wEvent == MCU_MTADP_DISCONNECTED_NOTIFY ||
        wEvent == MT_MCU_REGISTERGK_NACK ||
        wEvent == MT_MCU_REGISTERGK_ACK ||
        wEvent == MT_MCU_UNREGISTERGK_ACK ||
        wEvent == MT_MCU_UNREGISTERGK_NACK ||
        wEvent == MT_MCU_CONF_STOPCHARGE_ACK ||
        wEvent == MT_MCU_CONF_STOPCHARGE_NACK ||
        wEvent == MT_MCU_CONF_STARTCHARGE_ACK ||
        wEvent == MT_MCU_CONF_STARTCHARGE_NACK ||
		wEvent == MT_MCU_CONF_CHARGEEXP_NOTIF ||
        wEvent == MT_MCU_CHARGE_REGGK_NOTIF ||
		wEvent == MT_MCU_CALLFAIL_HDIFULL_NOTIF ) // xliang [10/31/2008] 
	{
		g_cMcuVcApp.SendMsgToDaemonConf( wEvent, pbyMsg, wLen );
	}		
	else
	{
		g_cMcuVcApp.SendMsgToConf( cServMsg.GetConfId(), wEvent, pbyMsg, wLen );
	}
}

/*=============================================================================
  函 数 名： SendMsgToH323MtAdp
  功    能： 发消息给H323MtAdp
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpSsnInst::SendMsgToH323MtAdp( CServMsg &cMsg )
{
    if(PROTOCOL_TYPE_H323 == m_byProtocolType)
    {
        SendMsgToMtAdp(cMsg.GetEventId(), cMsg.GetServMsg(), cMsg.GetServMsgLen());
    }

	return;
}

/*====================================================================
    函数名      : ProcMcuCasCadeMsg
    功能        ：处理mcu级联消息
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
void CMtAdpSsnInst::ProcMcuCasCadeMsg( CMessage * const pcMsg )
{
	u16 wAppId = GETAPP( pcMsg->srcid );

	switch(CurState()) 
	{
	case STATE_NORMAL:
		
		if( AID_MCU_MTADP == wAppId )
		{
			SendMsgToMcu( pcMsg->event, pcMsg->content, pcMsg->length );
		}
		else
		{
			SendMsgToMtAdp( pcMsg->event, pcMsg->content, pcMsg->length );
		}

		break;
	default:
		break;
	}
}


/*====================================================================
    函数名      : ProcMtAdpRegisterReq
    功能        ：处理MtAdp的注册消息
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
void CMtAdpSsnInst::ProcMtAdpRegisterReq( CMessage * const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMtAdpReg tMtAdpReg =  *( TMtAdpReg* )cServMsg.GetMsgBody();
	TMtAdpRegRsp tMtAdpRegRsp;
    s8 achMcuAlias[MAXLEN_ALIAS] = {0};
	memset( &tMtAdpRegRsp, 0, sizeof(tMtAdpRegRsp) );

	u32 dwSysSSrc = g_cMSSsnApp.GetMSSsrc();
	
	switch( CurState() ) 
	{
	case STATE_IDLE:
    //zbq[12/11/2007]状态机严格翻转
	//case STATE_NORMAL: 
        {	
        if (cServMsg.GetMsgBodyLen() != sizeof(TMtAdpReg)
			&& cServMsg.GetMsgBodyLen() != (sizeof(TMtAdpReg) + IPV6_STR_LEN) )
        {
            LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "CMtAdpSsnInst: invalid msg body len:%d, (should %d) nack.\n", 
                cServMsg.GetMsgBodyLen(), sizeof(TMtAdpReg));
            post( pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(),  pcMsg->srcnode );
            ::OspDisconnectTcpNode( pcMsg->srcnode );
			// 升级mtadp后，linux编不过，所以暂时用 [pengguofeng 5/18/2012]
            return;
        }
        
		if( tMtAdpReg.GetDriId() != GetInsID() )
		{
			LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "CMtAdpSsnInst: MtAdp's DriId: %d not equal to InsId: %d, nack.\n",tMtAdpReg.GetDriId(), GetInsID());
			post( pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(),  pcMsg->srcnode );
			::OspDisconnectTcpNode( pcMsg->srcnode );
			return;
		}
		//目前暂时只支持H323/H320,以后加入SIP支持
		if( tMtAdpReg.GetProtocolType() > PROTOCOL_TYPE_H320 )
		{
			LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "CMtAdpSsnInst: MtAdp's ProtocolType: %d not support, nack.\n",tMtAdpReg.GetDriId() );
			post( pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(),  pcMsg->srcnode );
			::OspDisconnectTcpNode( pcMsg->srcnode );
			return;
		}
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)// [3/1/2010 xliang] remove check below for 8000E
		//认证,是否是本MCU的单板
// 		if( tMtAdpReg.GetIpAddr() != ntohl(g_cMcuAgent.GetBrdIpAddr(tMtAdpReg.GetDriId())) )
// 		{
// 			//NACK
// 			LogPrint( LOG_LVL_ERROR, MID_MCU_MT, "CMtAdpSsnInst: other mcu board mtadp[Ip= %0x, Id= %d] register while agent info[Ip = %0x], nack!\n", 
//                                     tMtAdpReg.GetIpAddr(), tMtAdpReg.GetDriId(), 
//                                     ntohl(g_cMcuAgent.GetBrdIpAddr(tMtAdpReg.GetDriId())) );
// 			SendMsgToMtAdp( pcMsg->event+2, pcMsg->content, pcMsg->length );
// 			::OspDisconnectTcpNode( pcMsg->srcnode );
// 			return;
// 		}
#endif
		u8 byIpType = IP_NONE;
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMtAdpRegisterReq]MsgLen:%d TMtAdp_Size:%d IPV6_len:%d\n",
			cServMsg.GetMsgBodyLen(), sizeof(tMtAdpReg), IPV6_STR_LEN);
		if ( tMtAdpReg.GetVersion() == DEVVER_MTADPV6 )
		{
			//[pengguofeng 5/11/2012]验证消息长度，不正确则回NACK 
			if ( cServMsg.GetMsgBodyLen() != sizeof(tMtAdpReg) + sizeof(u8)*IPV6_STR_LEN 
				&& cServMsg.GetMsgBodyLen() != sizeof(tMtAdpReg) )
			{
				cServMsg.SetErrorCode(ERR_MCU_VER_UNMATCH);
				post(pcMsg->srcid, pcMsg->event + 2,
					cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
				
				LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "MtadpSsn: Mtadp.%d(0x%x) version %d differs from MCU %d, NACK!\n",                       
					GetInsID(),
					tMtAdpReg.GetIpAddr(),
					tMtAdpReg.GetVersion(),
					DEVVER_MTADPV6 );
				
				
				// 非内置，断链
				if (tMtAdpReg.GetIpAddr() != ntohl(g_cMcuAgent.GetMpcIp()) )
				{
					::OspDisconnectTcpNode(pcMsg->srcnode);
				}
				return;
			}

			//get IpV6
			u8	abyIpV6[IPV6_STR_LEN] = {0};
			u8  abyIpV6Null[IPV6_STR_LEN];
			memset(abyIpV6Null, 0, sizeof(abyIpV6Null));
			memcpy(abyIpV6, cServMsg.GetMsgBody() + sizeof(TMtAdpReg), sizeof(abyIpV6));
			
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT,"[ProcMtAdpRegisterReq]get Ipv6 from Msg:%s\n", abyIpV6);
			if ( memcmp(abyIpV6, abyIpV6Null, sizeof(abyIpV6)) != 0 )
			{
				//is IPV6
				if ( tMtAdpReg.GetIpAddr() != 0 )
				{
					//BOTH
					byIpType = IP_V4V6;
				}
				else
					byIpType = IP_V6;
			}
			else if ( tMtAdpReg.GetIpAddr() != 0)
			{
				byIpType = IP_V4;
			}
		} else
		// 版本验证失败，拒绝注册, zgc, 2007-09-28
        if ( tMtAdpReg.GetVersion() != 0 &&
             tMtAdpReg.GetVersion() != DEVVER_MTADP )
        {
            cServMsg.SetErrorCode(ERR_MCU_VER_UNMATCH);
            post(pcMsg->srcid, pcMsg->event + 2,
                 cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);

            LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "MtadpSsn: Mtadp.%d(0x%x) version %d differs from MCU %d, NACK!\n",                       
                      GetInsID(),
                      tMtAdpReg.GetIpAddr(),
                      tMtAdpReg.GetVersion(),
                      DEVVER_MTADP );


            // 非内置，断链
            if (tMtAdpReg.GetIpAddr() != ntohl(g_cMcuAgent.GetMpcIp()) )
            {
                ::OspDisconnectTcpNode(pcMsg->srcnode);
            }
            return;
        }

		//[pengguofeng 5/12/2012]老版本设成V4
		if ( tMtAdpReg.GetVersion() == DEVVER_MTADP )
		{
			byIpType = IP_V4;
		}

		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[MtAdpSsn]: MtAdp %d {Version:%d IPtype:%d} register to mcu success!\n",
			tMtAdpReg.GetDriId(), tMtAdpReg.GetVersion(), byIpType);
		
		//保存实例数据
		m_byMtAdpId         = tMtAdpReg.GetDriId();
		m_byMtAdpAttachMode = tMtAdpReg.GetAttachMode();
		m_byProtocolType    = tMtAdpReg.GetProtocolType() ;
		m_byMaxMtNum        = tMtAdpReg.GetMaxMtNum();
		m_dwMtAdpNode       = pcMsg->srcnode;
		m_dwMtAdpAppIId     = pcMsg->srcid;
	   
		//注册断链通知
		::OspNodeDiscCBRegQ( pcMsg->srcnode, GetAppID(), GetInsID() );       

        TLocalInfo tLocalInfo;
        if(SUCCESS_AGENT != g_cMcuAgent.GetLocalInfo(&tLocalInfo))
        {
            LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[MtAdpSsn]: get local cfg info failed!\n" );
            tMtAdpRegRsp.SetHeartBeatInterval( DEF_DISCCHECKTIME );
            tMtAdpRegRsp.SetHeartBeatFailTimes( DEF_DISCCHECKTIMES );
        }
        else
        {
            tMtAdpRegRsp.SetHeartBeatInterval( tLocalInfo.GetCheckTime() );
            tMtAdpRegRsp.SetHeartBeatFailTimes( (u8)tLocalInfo.GetCheckTimes() );

            TMtAdpCfg tCfg;
            tCfg.SetAudInfoRefreshTime(tLocalInfo.GetAudioRefreshTime());
            tCfg.SetVidInfoRefreshTime(tLocalInfo.GetVideoRefreshTime());
            tCfg.SetPartListRefreshTime(tLocalInfo.GetListRefreshTime());
            
            CServMsg cCfgMsg;
            cCfgMsg.SetEventId(MCU_MT_MTADPCFG_CMD);
            cCfgMsg.SetMsgBody((u8*)&tCfg, sizeof(tCfg));
            SendMsgToH323MtAdp(cCfgMsg);
        }

		//应答注册请求
		tMtAdpRegRsp.SetVcIp( ntohl( g_cMcuAgent.GetMpcIp() ) );
		tMtAdpRegRsp.SetMcuNetId( LOCAL_MCUID);
		tMtAdpRegRsp.SetGkIp( ntohl( g_cMcuAgent.GetGkIpAddr() ) );
		tMtAdpRegRsp.SetH225H245Port( g_cMcuAgent.Get225245StartPort() );
		tMtAdpRegRsp.SetH225H245MtNum( g_cMcuAgent.Get225245MtNum() );

        tMtAdpRegRsp.SetMcuVersion(GetMcuVersion());
		
		//是否为主协议适配板
		if ( tMtAdpReg.GetIpAddr() == g_cMcuAgent.GetRRQMtadpIp() )
		{
			tMtAdpRegRsp.SetMtAdpMaster(TRUE);
		}
		g_cMcuAgent.GetMcuAlias( achMcuAlias, MAXLEN_ALIAS );
		tMtAdpRegRsp.SetAlias( achMcuAlias );
        
        //是否支持GK计费
        tMtAdpRegRsp.SetIsGKCharge( g_cMcuAgent.GetIsGKCharge() );
		
		//设置MCU类型, zgc, 2007/04/03
		tMtAdpRegRsp.SetMcuType( GetMcuPdtType() );

        // guzh [4/30/2007] 设置别名显示方式
        tMtAdpRegRsp.SetCasAliasType( g_cMcuVcApp.GetCascadeAliasType() ); 
        tMtAdpRegRsp.SetAdminLevel( g_cMcuVcApp.GetCascadeAdminLevel() );

		// guzh [6/12/2007] 增加会话值通知
        LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[MtAdpSsn: Reg Ack with System SSRC.%u.\n", dwSysSSrc);
        tMtAdpRegRsp.SetMSSsrc(dwSysSSrc); 
        
        // zbq [05/14/2007] 是否启用呼叫匹配
        tMtAdpRegRsp.SetUseCallingMatch( g_cMcuVcApp.IsLimitAccessByMtModal() );

		s8 achPassword[GK_RRQ_PWD_LEN]={0};
		strncpy(achPassword,g_cMcuAgent.GetGkRRQPassword(),sizeof(achPassword));
		
		tMtAdpRegRsp.SetGkRRQUsePwdFlag(g_cMcuAgent.GetGkRRQUsePwdFlag());
		tMtAdpRegRsp.SetGkRRQPassword(achPassword);

		cServMsg.SetMsgBody( (u8*)&tMtAdpRegRsp, sizeof( tMtAdpRegRsp ) );
		
		// [11/23/2011 liuxu] 添加缓冲发送下级终端列表的配置信息
		u8 byAdpSMcuListBufFlag = g_cMcuVcApp.GetMtAdpSMcuListBufFlag();
		cServMsg.CatMsgBody( (u8*)&byAdpSMcuListBufFlag, sizeof(byAdpSMcuListBufFlag));
		u32 dwMtAdpSMcuListInterval = htonl(g_cMcuVcApp.GetRefreshMtAdpSMcuListInterval());
		cServMsg.CatMsgBody( (u8*)&dwMtAdpSMcuListInterval, sizeof(dwMtAdpSMcuListInterval));

#ifdef _UTF8
        //[4/8/2013 liaokang] 编码方式
        u8 byEncoding = emenCoding_Utf8;
        cServMsg.CatMsgBody( &byEncoding, sizeof(u8));
#endif
		
		SendMsgToMtAdp( pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

        g_cMcuVcApp.AddMtAdp( tMtAdpReg );

		g_cMcuVcApp.SetMtAdpIpType(tMtAdpReg.GetDriId(), byIpType);

		if( 0 != g_cMcuAgent.GetGkIpAddr() && PROTOCOL_TYPE_H323 == m_byProtocolType)
		{
            // zbq [04/25/2007] 终端适配Ucf或urj回来未通知MCU，导致注册状态不一致.
            // 这里无条件刷新注册一次，保证此种情况下 和 主备“同时”重启情况下主
            // 接入注册GK状态的成功。这里考虑由主接入上报URQ的结果，可以解决前一个
            // 状态不一致的问题，但可能不能解决主备“同时”重启的问题。进一步优化中 ...
            if ( /*!g_cMcuVcApp.GetConfRegState(0) && */tMtAdpRegRsp.IsMtAdpMaster() )
            {
                //在GK上注册MCU
                g_cMcuVcApp.SetRegGKDriId( m_byMtAdpId );
                g_cMcuVcApp.RegisterConfToGK( 0, m_byMtAdpId );
                
                //在GK上注册会议实体(包括会议和模板)
                for( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
                {
                    //注册模板
                    TConfMapData tConfMapData = g_cMcuVcApp.GetConfMapData( byConfIdx );
                    if ( tConfMapData.IsTemUsed() )
                    {
                        g_cMcuVcApp.RegisterConfToGK( byConfIdx, m_byMtAdpId, TRUE, FALSE );
                    }
                    //注册会议
                    else if ( tConfMapData.IsValidConf() )
                    {
                        g_cMcuVcApp.RegisterConfToGK( byConfIdx, m_byMtAdpId, FALSE, FALSE );
                    }
                }
            }
            else if (g_cMcuVcApp.GetRegGKDriId() != 0)
            {
                g_cMcuVcApp.RegisterConfToGK( 0, g_cMcuVcApp.GetRegGKDriId() );
            }

			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "CMtAdpSsnInst: MtAdp's DriId:%d Register To GK.\n", m_byMtAdpId);
		}        

        //set qos        
        TQosInfo tAgentQos;
        if(SUCCESS_AGENT == g_cMcuAgent.GetQosInfo(&tAgentQos))
        {
            TMcuQosCfgInfo tMcuQosCfg;
            tMcuQosCfg.SetQosType(tAgentQos.GetQosType());
            tMcuQosCfg.SetAudLevel(tAgentQos.GetAudioLevel());
            tMcuQosCfg.SetVidLevel(tAgentQos.GetVideoLevel());
            tMcuQosCfg.SetDataLevel(tAgentQos.GetDataLevel());
            tMcuQosCfg.SetSignalLevel(tAgentQos.GetSignalLevel());
            tMcuQosCfg.SetIpServiceType(tAgentQos.GetIpServiceType());                
                 
            CServMsg cQosMsg;
            cQosMsg.SetEventId(MCU_MT_SETQOS_CMD);
            cQosMsg.SetMsgBody((u8*)&tMcuQosCfg, sizeof(tMcuQosCfg));  
            SendMsgToH323MtAdp(cQosMsg);
        }
        else
        {
            LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[ProcEqpMcuRegMsg] get qos info failed!\n");
        }    
		
	    //通知Mcu
	    SendMsgToMcu(pcMsg->event,pcMsg->content,pcMsg->length);

		NEXTSTATE( STATE_NORMAL );
		break;
        }
    default:
        LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "MtAdpSsn: Wrong message %u(%s) received in current state %u!\n",
                  pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());         
        break;
	}

    return; 
}

/*====================================================================
    函数名      : ProcMtAdpDisconnect
    功能        ：MtAdp断链处理
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
void CMtAdpSsnInst::ProcMtAdpDisconnect(CMessage * const pcMsg)
{
	LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[MtAdpSsn]: Mtadp disconnect messege %u(%s) received!,Dri id is:%d,ProtocolType is:%d,MaxMtNum is:%d.\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byMtAdpId, m_byProtocolType, m_byMaxMtNum );

	u8 byRRQReset = FALSE;
	if( *( u32 * )pcMsg->content == m_dwMtAdpNode )	//本实例对应连接断
	{
        if (INVALID_NODE != m_dwMtAdpNode)
        {
            OspDisconnectTcpNode(m_dwMtAdpNode);
        }
        
		if( g_cMcuVcApp.GetRegGKDriId() == m_byMtAdpId )
		{
			g_cMcuVcApp.SetConfRegState( 0, 0 );
			g_cMcuVcApp.SetRegGKDriId(0);
			byRRQReset = TRUE;
		}

		//清除实例状态
		m_byMtAdpId		 = 0;
		m_dwMtAdpAppIId	 = 0;
		m_byProtocolType = 0;
		m_byMaxMtNum     = 0;
		m_dwMtAdpNode    = INVALID_NODE;

		TMtAdpReg tMtAdpReg;
		tMtAdpReg.SetDriId( (u8)GetInsID() );

		CServMsg cServMsg;
		cServMsg.SetMsgBody((u8 *)&tMtAdpReg,sizeof(tMtAdpReg));
		cServMsg.CatMsgBody(&byRRQReset,sizeof(byRRQReset));

		//从CMcuVcInst中移到此处处理，RemoveMtadp和AddMtadp都应该在mtadpssn中处理。分别放在vcinst和mtadpssninst中处理
		//会有时序问题[12/22/2012 chendaiwei]
		g_cMcuVcApp.RemoveMtAdp( tMtAdpReg.GetDriId() );	
	    SendMsgToMcu( MCU_MTADP_DISCONNECTED_NOTIFY,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());

		NEXTSTATE( STATE_IDLE );
	}	
}

/*=============================================================================
函 数 名： ProcMtAdpGetMsStatusReq
功    能： 适配获取mcu主备状态
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/2/21  4.0			许世林                  创建
=============================================================================*/
void CMtAdpSsnInst::ProcMtAdpGetMsStatusReq( CMessage * const pcMsg )
{
    // guzh [4/11/2007] 如果没有通过注册就发送获取请求，很有可能是重新启动前的连接，不理睬
    if ( CurState() == STATE_IDLE )
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[ProcMtAdpGetMsStatusReq]: Wrong message %u(%s) received in state.%u InsID.%u srcnode.%u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID(), pcMsg->srcnode);

        return;
    }

    if (MCU_MSSTATE_EXCHANGE_NTF == pcMsg->event)
    {
        // 如果同步失败,断开相应的连接
        CServMsg cServMsg(pcMsg->content, pcMsg->length);
        u8 byIsSwitchOk = *cServMsg.GetMsgBody();
        if (0 == byIsSwitchOk)
        {
            OspDisconnectTcpNode( m_dwMtAdpNode );
            return;
        }        
    }
    
    if( MCU_MSSTATE_ACTIVE == g_cMSSsnApp.GetCurMSState() )
    {        
        TMcuMsStatus tMsStatus;
        tMsStatus.SetMsSwitchOK(g_cMSSsnApp.IsMsSwitchOK());
        
        CServMsg cServMsg;
        cServMsg.SetMsgBody((u8*)&tMsStatus, sizeof(tMsStatus));
        SendMsgToMtAdp(MCU_MTADP_GETMSSTATUS_ACK, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

        LogPrint(LOG_LVL_DETAIL, MID_MCU_MT, "[ProcMtAdpGetMsStatusReq]: IsMsSwitchOK :%d.\n", tMsStatus.IsMsSwitchOK());
    }

    return;
}

/*=============================================================================
    函 数 名： ProcMcuMtAdpGKMsg
    功    能： 处理MCU到终端的GK消息
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/11   3.6			万春雷                  创建
=============================================================================*/
void CMtAdpSsnInst::ProcMcuMtAdpGKMsg( CMessage * const pcMsg )
{
	//CServMsg cServMsg( pcMsg->content, pcMsg->length );

	switch(CurState()) 
	{
	case STATE_NORMAL:
		
		//用于注册的适配模块板断链，则进行注册迁移，重新选择适配模块板 注册MCU及会议实体

/*		if( MCU_MT_REREGISTERGK_REQ == pcMsg->event )
		{
			if( !g_cMcuVcApp.GetConfRegState(0) && g_cMcuAgent.GetGkIpAddr()  && 
				0 == g_cMcuVcApp.GetRegGKDriId() && PROTOCOL_TYPE_H323 == m_byProtocolType )
			{
				//在GK上注册MCU
				g_cMcuVcApp.SetRegGKDriId(m_byMtAdpId);
				//g_cMcuVcApp.RegisterConfToGK( 0, m_byMtAdpId );

				//在GK上注册会议实体
				CMcuVcInst *pConf;
				for( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
				{
					pConf = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
					if( pConf != NULL ) 			
					{
						pConf->m_byRegGKDriId = m_byMtAdpId;
						//g_cMcuVcApp.RegisterConfToGK( (byLoop+1), pConf->m_byRegGKDriId );
					}
				}

				SendMsgToMtAdp( pcMsg->event, pcMsg->content, pcMsg->length );

				LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "CMtAdpSsnInst: MtAdp's DriId:%d Register To GK Again.\n", m_byMtAdpId);
			}
		}
		else*/
        if ( MCU_MT_UPDATE_REGGKSTATUS_NTF == pcMsg->event || 
			 MCU_MT_UPDATE_GKANDEPID_NTF == pcMsg->event ||
			 MCU_MTADP_GKADDR_UPDATE_CMD == pcMsg->event )
		{
			SendMsgToMtAdp( pcMsg->event, pcMsg->content, pcMsg->length );
		}

		break;

	default:
		break;
	}
}
	
/*====================================================================
    函数名      : ProcMtAdpMcuGKMsg
    功能        ：处理终端到MCU的GK消息
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/02/28    3.0         胡昌威        创建
====================================================================*/
void CMtAdpSsnInst::ProcMtAdpMcuGKMsg( CMessage * const pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byConfIdx  = cServMsg.GetConfIdx();
    
	switch(CurState()) 
	{
	case STATE_NORMAL:
		
		//更新其他适配板的GK上注册的GatekeeperID/EndpointID信息的通知
		if( MT_MCU_UPDATE_GKANDEPID_NTF == pcMsg->event )
		{
			TH323EPGKIDAlias *ptGKIDAlias = (TH323EPGKIDAlias*)(cServMsg.GetMsgBody());
			TH323EPGKIDAlias *ptEPIDAlias = ptGKIDAlias + 1;

			if( memcmp( (void*)ptGKIDAlias, g_cMcuVcApp.GetH323GKIDAlias(), sizeof(TH323EPGKIDAlias) ) || 
				memcmp( (void*)ptGKIDAlias, g_cMcuVcApp.GetH323EPIDAlias(), sizeof(TH323EPGKIDAlias) ) )
			{
				g_cMcuVcApp.SetH323GKIDAlias( ptGKIDAlias );
				g_cMcuVcApp.SetH323EPIDAlias( ptEPIDAlias );

				cServMsg.SetMsgBody( (u8*)ptGKIDAlias, sizeof(TH323EPGKIDAlias) );
				cServMsg.CatMsgBody( (u8*)ptEPIDAlias, sizeof(TH323EPGKIDAlias) );

				for( u8 byOtherDri = 1; byOtherDri <= MAXNUM_DRI; byOtherDri++)
				{
					if( g_cMcuVcApp.IsMtAdpConnected(byOtherDri) && 
						PROTOCOL_TYPE_H323 == g_cMcuVcApp.GetMtAdpProtocalType(byOtherDri) && 
						g_cMcuAgent.GetGkIpAddr()  && 0 != g_cMcuVcApp.GetRegGKDriId() && 
						byOtherDri != g_cMcuVcApp.GetRegGKDriId() )
					{
						cServMsg.SetDstDriId( byOtherDri );
						g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byOtherDri, MCU_MT_UPDATE_GKANDEPID_NTF, cServMsg );
					}
				}
			}
            
            // N+1备份，由主适配板完成更新MCU的GKID和EPID，以使得倒换后再次注册前的注销操作成功[12/18/2006-zbq]
            if ( GetInsID() == g_cMcuVcApp.GetRegGKDriId() &&
                 MCU_NPLUS_IDLE != g_cNPlusApp.GetLocalNPlusState() )
            {
                if ( 0 != g_cMcuAgent.GetGkIpAddr() &&
                     MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() ) 
                {
                    TRASInfo tRASInfo;
                    tRASInfo.SetEPID( ptEPIDAlias );
                    tRASInfo.SetGKID( ptGKIDAlias );
                    tRASInfo.SetGKIp( ntohl(g_cMcuAgent.GetGkIpAddr()) );
                    tRASInfo.SetRRQIp( g_cMcuAgent.GetRRQMtadpIp() );

                    // zbq [03/15/2007] 同步RAS信息增加主MCU的E164一项
                    TLocalInfo tLocalInfo;
                    g_cMcuAgent.GetLocalInfo( &tLocalInfo );
                    tRASInfo.SetMcuE164( tLocalInfo.GetE164Num() );

                    CServMsg cSendServMsg;
                    cSendServMsg.SetEventId( MCU_NPLUS_RASINFOUPDATE_REQ );
                    cSendServMsg.SetMsgBody( (u8*)&tRASInfo, sizeof(TRASInfo) );
                    g_cNPlusApp.PostMsgToNPlusDaemon(VC_NPLUS_MSG_NOTIF, cSendServMsg.GetServMsg(), cSendServMsg.GetServMsgLen());
                }
            }
			return;
		}

        if( cServMsg.GetSrcDriId() != g_cMcuVcApp.GetRegGKDriId() )
        {
            LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[Mtadpssn]: SrcDriId.(%d) != g_cMcuVcApp.GetRegGKDriId().(%d) %u(%s) return!\n", 
                                    cServMsg.GetSrcDriId(), g_cMcuVcApp.GetRegGKDriId(), 
                                    pcMsg->event, ::OspEventDesc( pcMsg->event ) );
            return;
        }

        // zbq [03/30/2007] 只作通知处理，不调整业务
        if ( MT_MCU_UNREGISTERGK_NACK == pcMsg->event )
        {
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[Mtadpssn]: ConfIdx<%d> unreg result.%d<%s>!\n", 
                    byConfIdx, pcMsg->event, OspEventDesc(pcMsg->event) );
            return;
        }
	
		//注册为会议及mcu信息统一注册，故只要有ack，mcu即已经正常注册
		if( pcMsg->event ==  MT_MCU_REGISTERGK_ACK )// MCU的注册回应
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[Mtadpssn]: ConfIdx<%d> register to GK success!\n", byConfIdx );
		}
		else if( 0 == byConfIdx && MT_MCU_REGISTERGK_NACK == pcMsg->event )
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_MT2, "[Mtadpssn]: ConfIdx<%d> register to GK failure!\n", byConfIdx );
		}

        //[12/2/2011 zhushengze]通告其他接入板会议注册状态放到VC中进行处理
        //在CMtAdpSsnInst做此动作会同处理接入板注册时将所有会议当前注册状态通告接入板存在时序问题
// 		if( byConfIdx <= MAX_CONFIDX )
// 		{
// 			//u8 byConfIdx  = 0; //0－mcu，1－MAXNUM_MCU_CONF 会议
// 			u8 byRegState = 0; //0－未注册，1－成功注册
// 			u8 byOtherDri = 1;
// 			TMtAlias tMtAlias;
// 			
// 			if( MT_MCU_REGISTERGK_ACK == pcMsg->event )
// 			{
// 				byRegState = 1;
// 			}
// 			g_cMcuVcApp.SetConfRegState( byConfIdx, byRegState );
// 
//     		//同步此时的注册MCU及会议实体 注册信息 到其他适配板
// 			if( 0 == byConfIdx )
// 			{ 	
// 				char achMcuAlias[MAXLEN_ALIAS];
// 				g_cMcuAgent.GetE164Number( achMcuAlias, MAXLEN_ALIAS );
// 				tMtAlias.SetE164Alias( achMcuAlias );
// 				cServMsg.SetMcuId( LOCAL_MCUID );
// 				cServMsg.SetMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
// 				cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
// 				cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );
// 			}
// 			else
// 			{		
//                 TConfInfo* ptConfInfo = NULL;
//                 TTemplateInfo tTemInfo;
//                 
//                 TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
//                 if(tMapData.IsValidConf())
//                 {
// 					CMcuVcInst* pcVcinst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
//                     if( NULL ==  pcVcinst)
//                     {
//                         //zbq [10/11/2007] 主备倒换 辅助接入板带会议的模板注册状态被冲问题：
//                         //    如果是带会议的模板但却没有会议，尝试同步其模板的当前注册状态
//                         LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[ProcMtAdpMcuGKMsg] RegAck for Conf.%d received while conf is not create yet which should be there before this moment, just take a try of its template once more\n", byConfIdx);
// 
//                         if(tMapData.IsTemUsed())
//                         {
//                             if(!g_cMcuVcApp.GetTemplate(byConfIdx, tTemInfo))
//                             {
//                                 LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[ProcMtAdpMcuGKMsg] RegAck for Conf.%d whose template unexist yet, ignore it\n", byConfIdx);
//                                 return;
//                             }
//                             else
//                             {
//                                 ptConfInfo = &tTemInfo.m_tConfInfo;
//                                 LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[ProcMtAdpMcuGKMsg] RegAck for Conf.%d whose template exist, continue\n", byConfIdx);
//                             }
//                         }
//                         return;
//                     }
//                     else
//                     {
// 						ptConfInfo = &pcVcinst->m_tConf;
//                     }
//                 }
//                 else if(tMapData.IsTemUsed())
//                 {
//                     if(!g_cMcuVcApp.GetTemplate(byConfIdx, tTemInfo))
//                     {
//                         LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[ProcMtAdpMcuGKMsg] RegAck for Template.%d while template unexist, ignore it\n", byConfIdx);
//                         return;
//                     }
//                     else
//                     {
//                         ptConfInfo = &tTemInfo.m_tConfInfo;
//                     }
//                 }
//                 else
//                 {
//                     LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[ProcMtAdpMcuGKMsg] RegAck for Conf/Temp.%d while both of them unexist, ignore it\n", byConfIdx);
//                     return;
//                 }
// 				
// 				//MT_MCU_UNREGISTERGK_ACK MT_MCU_UNREGISTERGK_NACK
// 				if( MT_MCU_REGISTERGK_ACK  == pcMsg->event || 
// 					MT_MCU_REGISTERGK_NACK == pcMsg->event )
// 				{
// 					tMtAlias.SetE164Alias( ptConfInfo->GetConfE164() );
// 					cServMsg.SetConfId( ptConfInfo->GetConfId() );
// 				}
// 				cServMsg.SetMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
// 				cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
// 				cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );
// 
//                 if (byRegState == 0)
//                 {
//                     LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[ProcMtAdpMcuGKMsg]conf.%d reg state is NULL, Alias is:\n ", byConfIdx);
//                     tMtAlias.Print();
//                 }
// 			}
// 
// 			for( byOtherDri = 1; byOtherDri <= MAXNUM_DRI; byOtherDri++)
// 			{
// 				if( g_cMcuVcApp.IsMtAdpConnected(byOtherDri) && 
// 					PROTOCOL_TYPE_H323 == g_cMcuVcApp.GetMtAdpProtocalType(byOtherDri) && 
// 					g_cMcuAgent.GetGkIpAddr()  && 0 != g_cMcuVcApp.GetRegGKDriId() && 
// 					byOtherDri != g_cMcuVcApp.GetRegGKDriId() )
// 				{
// 					cServMsg.SetDstDriId( byOtherDri );
// 					g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byOtherDri, MCU_MT_UPDATE_REGGKSTATUS_NTF, cServMsg );
// 				}
// 			}
// 		}
		
        // 会议在GK上注册的回应
        //if( 0 != cServMsg.GetConfIdx() ||
        //    (0 == cServMsg.GetConfIdx() && MT_MCU_REGISTERGK_NACK == cServMsg.GetEventId()) )
		{
			SendMsgToMcu( pcMsg->event, pcMsg->content, pcMsg->length );
		}
		
		break;

	default:
		break;
	}
}

/*====================================================================
    函数名      : ProcMtAdpMcuMsg
    功能        ：中转适配消息给MCU业务模块
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
void CMtAdpSsnInst::ProcMtAdpMcuMsg( CMessage * const pcMsg )
{
	switch(CurState()) 
	{
	case STATE_NORMAL:
		
		SendMsgToMcu( pcMsg->event, pcMsg->content, pcMsg->length );

		break;
	default:
		break;
	}

}

/*====================================================================
    函数名      : ProcMcuMtAdpMsg
    功能        ：中转MCU业务模块消息给适配
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
void CMtAdpSsnInst::ProcMcuMtAdpMsg( CMessage * const pcMsg )
{
	switch(CurState()) 
	{
	case STATE_NORMAL:

		SendMsgToMtAdp( pcMsg->event, pcMsg->content, pcMsg->length );		
		
		break;

	default:
		break;
	}
}



//CMpConfig
/*--------------------------------------------------------------------------------*/

//构造
CMtAdpConfig::CMtAdpConfig()
{

}

//析构
CMtAdpConfig::~CMtAdpConfig()
{

}

/*====================================================================
    函数名      : SendMsgToMtAdpSsn
    功能        ：发消息给MtAdp会话
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt & tDstMt: 消息的目标
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
BOOL32 CMtAdpConfig::SendMsgToMtAdpSsn( const TMt & tDstMt, u16 wEvent, CServMsg & cServMsg )
{
    if ( 0 == tDstMt.GetDriId() || tDstMt.GetDriId() > MAXNUM_DRI)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[MtAdpCfg]: MtId.%d Invalid MtAdpId.%d for message %s.\n", 
                   tDstMt.GetMtId(),tDstMt.GetDriId(), OspEventDesc(wEvent) );
		return FALSE;
	}
	else if (!g_cMcuVcApp.IsMtAdpConnected(tDstMt.GetDriId()))
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[MtAdpCfg]: MtId.%d MtAdp.%d is not connected.\n", tDstMt.GetMtId(),tDstMt.GetDriId() );
		return FALSE;
	}
	else
	{
		//本端主用则允许向外部各App投递消息，否则丢弃
		if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
		{
			return TRUE;
		}
		
		//cServMsg.SetEventId(wEvent);
		cServMsg.SetDstDriId( tDstMt.GetDriId() );
		cServMsg.SetDstMtId( tDstMt.GetMtId() );
		cServMsg.SetMcuId( LOCAL_MCUID );

		return ( ::OspPost( MAKEIID( AID_MCU_MTADPSSN, tDstMt.GetDriId() ), wEvent, 
						cServMsg.GetServMsg(), cServMsg.GetServMsgLen() ) == OSP_OK );
	}
}

/*====================================================================
    函数名      : SendMsgToMtAdpSsn
    功能        ：发消息给MtAdp会话
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMpId: 消息的目标
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/02/28    3.0         胡昌威         创建
====================================================================*/
BOOL32 CMtAdpConfig::SendMsgToMtAdpSsn( u8 byMpId, u16 wEvent, CServMsg & cServMsg )
{
    if (0 == byMpId || byMpId > MAXNUM_DRI)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT,  "[MtAdpCfg]: SendMsgToMtAdpSsn Invalid MpId: %d.\n", byMpId);
		return FALSE;
	}
	else
	{
		//本端主用则允许向外部各App投递消息，否则丢弃
		if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
		{
			return TRUE;
		}

		cServMsg.SetMcuId( LOCAL_MCUID );			
		return ( ::OspPost( MAKEIID( AID_MCU_MTADPSSN, byMpId ), wEvent, 
						cServMsg.GetServMsg(), cServMsg.GetServMsgLen() ) == OSP_OK );
	}
}

/*====================================================================
    函数名      : BroadcastToAllMtAdpSsn
    功能        ：发消息给所有已连接的MtAdp会话
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
BOOL32 CMtAdpConfig::BroadcastToAllMtAdpSsn(u16 wEvent, const CServMsg & cServMsg )
{
	//本端主用则允许向外部各App投递消息，否则丢弃
	if (FALSE == g_cMSSsnApp.JudgeSndMsgPass())
	{
		return TRUE;
	}

	return ( OSP_OK == ::OspPost( MAKEIID( AID_MCU_MTADPSSN, CInstance::EACH ), wEvent, 
		                          cServMsg.GetServMsg(), cServMsg.GetServMsgLen() ) );
}


//END OF FILE
