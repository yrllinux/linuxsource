/*****************************************************************************
   模块名      : mcu_new
   文件名      : mtssn.cpp
   相关文件    : mtssn.h
   文件实现功能: 下级MT会话应用类定义
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/07/25  0.9         李屹        创建
   2002/01/07  1.0         李屹        使用宏H323进行改造
   2009/09/01  4.6         张宝卿      移植到4.6高清平台
******************************************************************************/

// mtssn.cpp: implementation of the CMtSsnInst class.
//
//////////////////////////////////////////////////////////////////////
#include "osp.h"
#include "kdvsys.h"
#include "mtssn.h"
#include "evmcumt.h"
#include "evmodem.h"
#include "modemssn.h"

#ifdef _MSC_VER
#include <ws2tcpip.h>
#endif

CMtSsnApp	g_cMtSsnApp;	//下级MCU会话应用实例

SOCKET CMtSsnInst::m_hSocket = INVALID_SOCKET;
extern CModemSsnApp	g_cModemSsnApp;

TMcuTopo	g_atMcuTopo[MAXNUM_TOPO_MCU];
u8		    g_wMcuTopoNum;
TMtTopo		g_atMtTopo[MAXNUM_TOPO_MT];	//保存全网中MT拓扑信息
u16		    g_wMtTopoNum;		        //保存全网中MT数目

u8              g_byMcuModemNum = 0;		//Mcu Modem的个数,为0则系统中无需控制modem
TMcuModemTopo   g_atMcuModemTopo[16];       //Mcu Modem信息


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMtSsnInst::CMtSsnInst()
{
#ifndef H323
	m_dwMtNode = INVALID_NODE;
#endif
	m_byMtId = 0;
	m_byTimeoutTimes = 0;
	timeold = 0;
	eventold = 0;

}

CMtSsnInst::~CMtSsnInst()
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
void CMtSsnInst::InstanceDump( u32 param )
{
	char achType[10];
	u8	byPrimaryMt;
	char achState[16];
 
	switch( CurState() )
	{
	case STATE_IDLE:
		strncpy( achState, "IDLE", sizeof( achState ));
		achState[sizeof( achState ) -1] = '\0';
		break;
#ifndef H323
	case STATE_CONNECTED:
		strncpy( achState, "CONNECTED", sizeof( achState ));
		achState[sizeof( achState ) -1] = '\0';
		break;
#endif
	case STATE_NORMAL:
		strncpy( achState, "NORMAL", sizeof( achState ));
		achState[sizeof( achState ) -1] = '\0';
		break;	
	case STATE_CALLING:
		strncpy( achState, "CALLING", sizeof( achState ));
		achState[sizeof( achState ) -1] = '\0';
		break;
	case STATE_WAITING:
		strncpy( achState, "WAITING", sizeof( achState ));
		achState[sizeof( achState ) -1] = '\0';
		break;
	default:
		strncpy( achState, "ERROR", sizeof( achState ));
		achState[sizeof( achState ) -1] = '\0';
		break;
	}

	if( ::topoIsSecondaryMt( LOCAL_MCUID, m_byMtId, g_atMtTopo, g_wMtTopoNum ) )
	{
		strncpy( achType, "SECONDARY", sizeof( achType ) );
		byPrimaryMt = ::topoGetPrimaryMt( LOCAL_MCUID, m_byMtId, g_atMtTopo, 
			g_wMtTopoNum );
	}
	else
	{
		strncpy( achType, "COMMON", sizeof( achType ) );
		byPrimaryMt = m_byMtId;
	}
	achType[sizeof( achType ) -1] = 0;

	TMtTopo tMtTopo;
	tMtTopo = ::topoGetMtInfo( LOCAL_MCUID, m_byMtId, g_atMtTopo, g_wMtTopoNum );
	                      
#ifndef H323               //"MTID INSTID NODE STATUS      MTALIAS TYPE    PRIMEMTID \n"
	OspPrintf( TRUE, FALSE, "%4u %6u %5u %-12s %-8s %-8s %5u\n",
		                    m_byMtId, GetInsID(), m_dwMtNode, achState, tMtTopo.GetAlias(), achType, byPrimaryMt );
#else                    //"MTID    INSTID  STATUS   MTALIAS  TYPE         PRIMEMTID
	OspPrintf( TRUE, FALSE,"%4u %5u %-8s %-8s %-8s %5u\n",
		                    m_byMtId, GetInsID(), achState, tMtTopo.GetAlias(), achType, byPrimaryMt );
#endif
	
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
void CMtSsnInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		log(LOGLVL_EXCEPTION, "CMtSsnInst: The received msg's pointer in the msg entry is NULL!");
		return;
	}
	
	switch( pcMsg->event )
	{
#ifndef H323
	case MT_MCU_READY_NOTIF:		//终端通知MCU它自己准备得到加入会议的邀请
		ProcMtMcuReadyNotif( pcMsg );
		break;
#endif	//H323
	case TIMER_ROUNDTRIPDELAY_REQ:	//send roundTripDelayRequest
		ProcTimerRoundTripDelayReq( pcMsg );
		break;
	case TIMER_WAIT_ROUNDTRIPDELAY_RSP:	//roundTripDelayResponse time out
		ProcTimerWaitRoundTripDelayRsp( pcMsg );
		break;
	case MT_MCU_ROUNDTRIPDELAY_ACK:		//终端返回应答
		ProcMtMcuRoundTripDelayRsp( pcMsg );
		break;

	case OSP_DISCONNECT:			//终端断链通知
		ProcMtDisconnect( pcMsg );
		break;
    case MT_MCU_REG_REQ:			//终端发起的登记请求
		ProcMtMcuRegReq( pcMsg );
		break;
	//case MT_MCU_GETMCUTOPO_REQ:	//终端请求查询MCU拓扑
		ProcMtMcuGetMcuTopoReq( pcMsg );
		break;
	case MCU_MT_INVITEMT_REQ:		//邀请终端加入会议
		ProcMcuMtInviteMtReq( pcMsg );
		break;
	case MCU_MT_OPENLOGICCHNNL_REQ:	//打开终端双流通道
		ProcMcuMtOpenLogicChnReq( pcMsg );
		break;
	case MCU_MT_CLOSELOGICCHNNL_CMD:
		ProcMcuMtCloseLogicChnCmd( pcMsg );
		break;

	case MT_MCU_INVITEMT_ACK:		//邀请终端加入会议成功应答
	case MT_MCU_INVITEMT_NACK:		//邀请终端加入会议拒绝应答
		ProcMtMcuInviteMtRsp( pcMsg );
		break;

	case MCU_MT_CAPBILITYSET_NOTIF:
		ProcMcuMtCapbilitySetNotif( pcMsg );
		break;

	case MT_MCU_APPLYJOINCONF_REQ:	//终端申请加入
		ProcMtMcuApplyJoinRequest( pcMsg );
		break;
	case MCU_MT_APPLYJOINCONF_ACK:	//申请入会应答
	case MCU_MT_APPLYJOINCONF_NACK:	//申请入会应答
		ProcMcuMtApplyJoinRsp( pcMsg );
		break;
	case MT_MCU_CREATECONF_REQ:		//终端创建会议请求
		ProcMtMcuCreateConfRequest( pcMsg );
		break;
	case MCU_MT_CREATECONF_ACK:		//终端创建会议完整信息应答
	case MCU_MT_CREATECONF_NACK:	//终端创建会议完整信息应答
		ProcMcuMtCreateConfRsp( pcMsg );
		break;
	case TIMER_WAIT_CALL_RSP:		//等待呼叫应答超时
		ProcTimerWaitCallRsp( pcMsg );
		break;
	case TIMER_WAIT_REPLY:			//等待命令超时消息
		ProcTimerWaitReply( pcMsg );
		break;
	case MCU_MT_DELMT_CMD:			//强制终端退出会议
		ProcMcuMtDropMtCmd( pcMsg );
		break;
	//case MCU_MT_MCUSWITCH_NOTIF:	//跨MCU交换改变通知
	case MCU_MT_SPECCHAIRMAN_NOTIF:	//指定主席终端
	case MCU_MT_CANCELCHAIRMAN_NOTIF:	//取消主席终端
	case MCU_MT_SPECSPEAKER_NOTIF:	    //指定发言终端
	case MCU_MT_CANCELSPEAKER_NOTIF:	//取消发言终端
	//case MCU_MT_STARTRCV_CMD:		//通知终端开始接收
	//case MCU_MT_STOPRCV_CMD:		//通知终端停止接收
	//case MCU_MT_STARTSEND_CMD:		//通知终端开始发送
	//case MCU_MT_STOPSEND_CMD:		//通知终端停止发送
	case MCU_MT_FREEZEPIC_CMD:			//冻结图像
	case MCU_MT_FASTUPDATEPIC_CMD:		//快速更新图像
	//case MCU_MT_MTCAMERA_MOVE_CMD:		//终端摄像头移动
	//case MCU_MT_MTCAMERA_FOCUS_CMD:		//终端摄像头调焦
	//case MCU_MT_MTCAMERA_ZOOM_CMD:		//终端摄像头视野调节
	case MCU_MT_MTCAMERA_RCENABLE_CMD:	//终端摄像头遥控器使能
	//case MCU_MT_MTCAMERA_BRIGHTNESS_CMD://终端摄像头亮度调节
	case MCU_MT_MTCAMERA_MOVETOPOS_CMD:	//终端摄像头调整到指定位置
	case MCU_MT_MTCAMERA_SAVETOPOS_CMD:	//终端摄像头保存到指定位置
	case MCU_MT_APPLYCHAIRMAN_NOTIF://终端申请主席通知
	case MCU_MT_APPLYSPEAKER_NOTIF:	//终端申请发言通知
	//case MCU_MT_APPLYQUITCONF_NOTIF://终端申请退出会议通知
	//case MCU_MT_APPLYJOINCONF_NOTIF://终端申请加入会议通知
	//case MCU_MT_CAPBILITYSET_NOTIF://MCU发给终端的能力集通知
//	case MCU_MT_SENDMSG_NOTIF:		//发送短消息
	//case MCU_MT_RESETDECODER_CMD:	//解码器复位
	case MCU_MT_CONF_NOTIF:			//会议通知
	case MCU_MT_MTJOINED_NOTIF:		//终端加入会议通知
	case MCU_MT_MTLEFT_NOTIF:		//终端退出会议通知
	//case MCU_MT_ADDMT_NOTIF:		//会议增加受邀终端通知
	//case MCU_MT_DELMT_NOTIF:		//会议删除受邀终端通知
	case MCU_MT_SPECCHAIRMAN_ACK:	//主席终端指定主席请求应答
	case MCU_MT_SPECCHAIRMAN_NACK:	//主席终端指定主席请求应答
	case MCU_MT_SPECSPEAKER_ACK:	//主席终端指定发言人请求应答
	case MCU_MT_SPECSPEAKER_NACK:	//主席终端指定发言人请求应答
	case MCU_MT_ADDMT_ACK:			//主席终端邀请终端请求应答
	case MCU_MT_ADDMT_NACK:			//主席终端邀请终端请求应答
	case MCU_MT_DELMT_ACK:			//主席终端强制终端退出请求应答
	case MCU_MT_DELMT_NACK:			//主席终端强制终端退出请求应答
	case MCU_MT_OPENLOGICCHNNL_ACK:	//打开逻辑通道应答
	case MCU_MT_OPENLOGICCHNNL_NACK://打开逻辑通道应答
	//case MCU_MT_OPENLOGICCHNNL_REQ:	//MCU打开逻辑通道请求，被串行化
	//case MCU_MT_STARTSWITCHMT_ACK:		//交换请求应答
	//case MCU_MT_STARTSWITCHMT_NACK:		//交换请求应答
	//case MCU_MT_STOPSWITCHMT_ACK:	//停止交换请求应答
	//case MCU_MT_STOPSWITCHMT_NACK:	//停止交换请求应答
	case MCU_MT_APPLYCHAIRMAN_ACK:	//申请主席应答
	case MCU_MT_APPLYCHAIRMAN_NACK:	//申请主席应答
	case MCU_MT_GETCONFINFO_ACK:	//终端查询会议完整信息应答
	case MCU_MT_GETCONFINFO_NACK:	//终端查询会议完整信息应答
	//case MCU_MT_INQUIRECONF_ACK:	//终端查询会议状态应答
	//case MCU_MT_INQUIRECONF_NACK:	//终端查询会议状态应答
	case MCU_MT_JOINEDMTLIST_ACK:	//查询与会终端列表请求应答
	case MCU_MT_JOINEDMTLIST_NACK:	//查询与会终端列表请求应答
	//case MCU_MT_INVITEDMTLIST_ACK:	//查询受邀终端列表请求
	//case MCU_MT_INVITEDMTLIST_NACK:	//查询受邀终端列表请求
	//case MCU_MT_STARTMIX_CMD:		//开始混音
	//case MCU_MT_STOPMIX_CMD:		//停止混音
	//case MCU_MT_SETBITRATE_CMD:		//设置终端码率
	//case MCU_MT_LISTALLCONF_ACK:    //列出所有会议信息应答
	//case MCU_MT_STARTPOLL_CMD:		//开始轮循
	//case MCU_MT_STOPPOLL_CMD:		//停止轮循
	//case MCU_MT_SETPOLLINTERVAL_CMD://设置轮循时间间隔
	//case MCU_MT_FLOWADJUST_CMD:
    //立案系统特殊命令
	case MCU_MT_STARTMTSELME_ACK:  //让其它终端选看自己的应答
	case MCU_MT_STARTMTSELME_NACK: //让其它终端选看自己的拒绝
	case MCU_MT_GETMTSELSTUTS_ACK: //得到其它终端选看情况的应答
	case MCU_MT_GETMTSELSTUTS_NACK://得到其它终端选看情况的拒绝
	//立案
	//case MCU_MT_APPLYCASE_ACK:      //MCU给终端发送的应答 
	//case MCU_MT_APPLYCASE_NACK:     //MCU给终端发送的应答
	//Satellite MCU
	//case MCU_MT_MTSTATUS_NOTIF:     //终端媒体状态通知
	
	case 30254: //MCU_MT_STARTDS_REQ
	case 30255: //MCU_MT_STARTDS_REQ + 1
	case 25011://MCU_MT_SENDIFRAME_NOTIF
		ProcMcuMtMsg( pcMsg );
		break;
	case MCU_MT_SENDMSG_NOTIF:		//发送短消息
		ProcMcuMtMsg( pcMsg );
		break;
	case MT_MCU_APPLYCHAIRMAN_REQ:	//终端申请主席
	//case MT_MCU_APPLYSPEAKER_CMD:	//终端申请发言
	//case MT_MCU_APPLYQUITCONF_CMD:	//终端申请退出
	case MT_MCU_DROPCONF_CMD:		//主席终端中止会议请求
	case MT_MCU_SPECCHAIRMAN_REQ:	//主席终端指定主席请求
	case MT_MCU_SPECSPEAKER_REQ:	//主席终端指定发言人请求
	//case MT_MCU_CANCELCHAIRMAN_CMD:	//主席终端取消主席请求
	case MT_MCU_CANCELSPEAKER_CMD:	//主席终端取消发言人请求
	case MT_MCU_ADDMT_REQ:			//主席终端邀请终端请求
	case MT_MCU_DELMT_REQ:			//主席终端强制终端退出请求
	case MT_MCU_OPENLOGICCHNNL_ACK:		//打开逻辑通道应答
	case MT_MCU_OPENLOGICCHNNL_NACK:	//打开逻辑通道应答
	case MT_MCU_OPENLOGICCHNNL_REQ:		//终端打开逻辑通道请求
	//case MT_MCU_MTCAMERA_MOVE_CMD:		//主席命令终端摄像头移动
	//case MT_MCU_MTCAMERA_FOCUS_CMD:		//主席命令终端摄像头调焦
	//case MT_MCU_MTCAMERA_ZOOM_CMD:		//主席命令终端摄像头视野调节
	case MT_MCU_MTCAMERA_RCENABLE_CMD:	//主席命令终端摄像头遥控器使能
	//case MT_MCU_MTCAMERA_BRIGHTNESS_CMD://主席命令终端摄像头亮度调节
	case MT_MCU_MTCAMERA_MOVETOPOS_CMD:	//主席命令终端摄像头调整到指定位置
	case MT_MCU_MTCAMERA_SAVETOPOS_CMD:	//主席命令终端摄像头保存到指定位置
	//case MT_MCU_STARTSWITCHMT_REQ:		//终端交换请求
	//case MT_MCU_STOPSWITCHMT_REQ:   //终端停止交换请求
	case MT_MCU_STARTSELMT_CMD:     //323接口下主席选看标准接口
	case MT_MCU_STOPSELMT_CMD:
	case MT_MCU_SENDMSG_CMD:		//短消息请求
	case MT_MCU_SENDMCMSG_CMD:		//短消息请求给主MCU控制台
	case MT_MCU_JOINEDMTLIST_REQ:	//查询与会终端列表请求
	//case MT_MCU_INVITEDMTLIST_REQ:	//查询受邀终端列表请求
	case MT_MCU_CAPBILITYSET_NOTIF:	//终端发能力集通知
	case MT_MCU_MTSTATUS_NOTIF:		//终端状态上报
	//case MT_MCU_FLOWADJUST_CMD:	  //终端请求视频源码率改变	
		//立案系统特殊命令
	case MT_MCU_STARTMTSELME_REQ:   //让其它终端选看自己的请求
	//case MT_MCU_STOPMTSELME_NOTIFY: //让其它终端停止选看自己的通知
	//case MT_MCU_STARTBROADCASTMT_CMD: //让MCU广播其它终端的命令
	case MT_MCU_STOPBROADCASTMT_CMD:  //让MCU停止广播其它终端的命令
	case MT_MCU_GETMTSELSTUTS_REQ:    //得到其它终端选看情况的请求
    //case MT_MCU_APPLYCASE_REQ:       //终端向MCU发送立案请求		
		ProcMtMcuMsg( pcMsg );
		break;
	//case MCU_MT_MTRCENABLE_REQ:		//终端遥控器使能
	//case MCU_MT_MTFARSITEMUTE_REQ:	//终端远端静音设置
	//case MCU_MT_MTNEARSITEMUTE_REQ:	//终端近端静音设置
	//case MCU_MT_SETMTVIDEO_REQ:		//设置终端视频输入源
	//case MCU_MT_SETMTAUDIO_REQ:		//设置终端音频输入源
	//case MCU_MT_QUERYPOLLPARAM_REQ:	//查询轮循参数
	//立案
	//case MCU_MT_APPLYCASE_REQ:     //MCU向终端发送立案请求
		ProcMcuMtRequest( pcMsg );
		break;
	//case MT_MCU_MTRCENABLE_ACK:		//终端遥控器使能应答
	//case MT_MCU_MTRCENABLE_NACK:	//终端遥控器使能应答
	//case MT_MCU_MTFARSITEMUTE_ACK:	//终端远端静音设置应答
	//case MT_MCU_MTFARSITEMUTE_NACK:	//终端远端静音设置应答
	//case MT_MCU_MTNEARSITEMUTE_ACK:	//终端近端静音设置应答
	//case MT_MCU_MTNEARSITEMUTE_NACK://终端近端静音设置应答
	//case MT_MCU_SETMTVIDEO_ACK:		//设置终端视频源应答
	//case MT_MCU_SETMTVIDEO_NACK:	//设置终端视频源应答
	//case MT_MCU_SETMTAUDIO_ACK:		//设置终端音频源应答
	//case MT_MCU_SETMTAUDIO_NACK:	//设置终端音频源应答
	//case MT_MCU_QUERYPOLLPARAM_ACK:	//查询轮循参数应答
	//case MT_MCU_QUERYPOLLPARAM_NACK:	//查询轮循参数应答
	//立案
	//case MT_MCU_APPLYCASE_ACK:      //MT给MCU的立案申请应答
	//case MT_MCU_APPLYCASE_NACK:      //MT给MCU的立案申请应答
		ProcMtMcuResponse( pcMsg );
		break;
#ifndef H323
	//case MT_MCU_INQUIRECONF_REQ:	//终端查询会议是否存在
	//case MT_MCU_LISTALLCONF_REQ:	//列出该MCU上所有会议信息
#endif
		ProcMtMcuDaemonConfMsg( pcMsg );
		break;

	//以下消息暂不处理
	//case MT_MCU_MAXSKEW_NOTIF:
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received, Inst.%d!\n",
                                pcMsg->event, ::OspEventDesc( pcMsg->event ), GetInsID() );
		break;
	}
}


API void startds(u8 byConfIdx, u8 byMtId)
{
	ITSatMsgHdr tSatHdr;
	tSatHdr.SetConfIdx(byConfIdx);
	tSatHdr.SetEventId(30254);
	tSatHdr.SetSrcId(byMtId);
	u8 byStart = 1;

	CServMsg cMsg;
	cMsg.SetMsgBody((u8*)&tSatHdr, sizeof(tSatHdr));
	cMsg.CatMsgBody(&byStart, sizeof(u8));

	::OspPost(MAKEIID(AID_MCU_MTSSN, CInstance::DAEMON),
			  30254,
			  cMsg.GetMsgBody(),
			  cMsg.GetMsgBodyLen());
}

API void stopds(u8 byConfIdx, u8 byMtId)
{
	ITSatMsgHdr tSatHdr;
	tSatHdr.SetConfIdx(byConfIdx);
	tSatHdr.SetEventId(30254);
	tSatHdr.SetSrcId(byMtId);
	u8 byStart = 0;
	
	CServMsg cMsg;
	cMsg.SetMsgBody((u8*)&tSatHdr, sizeof(tSatHdr));
	cMsg.CatMsgBody(&byStart, sizeof(u8));
	
	::OspPost(MAKEIID(AID_MCU_MTSSN, CInstance::DAEMON),
				30254,
				cMsg.GetMsgBody(),
				cMsg.GetMsgBodyLen());

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
    03/05/30    1.0         JQL           创建
====================================================================*/
void CMtSsnInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	
	if( NULL == pcMsg )
	{
		log(LOGLVL_EXCEPTION, "CMtSsnInst: The received msg's pointer in the msg entry is NULL!");
		return;
	}
		
	switch( pcMsg->event )
	{

	case MCU_APPTASKTEST_REQ:			//GUARD Probe Message
		DaemonProcAppTaskRequest( pcMsg );
		break;
	case OSP_POWERON:
		DaemonProcPowerOn( pcMsg, pcApp );
		break;	

    /*For Satellite MCU That need to transport to special mt*/
	case MT_MCU_CREATECONF_REQ:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			TMt		tMt;
			u8		byMtId = *( u8* )cSatMsg.GetSatMsgBody();
			CSatConfInfo * pSatConfInfo = ( CSatConfInfo* )( cSatMsg.GetSatMsgBody() + 1 );
			TConfInfo tConfFullInfo;
			
			log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received from mt %d.\n", pcMsg->event, ::OspEventDesc( pcMsg->event ), byMtId );
			
			//设置会议终端信息
			pSatConfInfo->SetConfMtInfoMcuId( (u8)LOCAL_MCUID );
			
            //FIXME: 换一种方式 传递终端信息
            //tConfFullInfo.SetMtInfo( 0, pSatConfInfo->GetConfMtInfo() );
			
			//设置主席
			if( pSatConfInfo->GetChairmanId() != 0 )
			{
				tMt.SetMt( LOCAL_MCUID, pSatConfInfo->GetChairmanId() );
				tConfFullInfo.SetChairman( tMt );
			}
			//设置发言人
			if( pSatConfInfo->GetSpeakerId() != 0 )
			{
				tMt.SetMt( LOCAL_MCUID, pSatConfInfo->GetSpeakerId() );
				tConfFullInfo.SetSpeaker( tMt );
			}
			//设置会议名
			tConfFullInfo.SetConfName( pSatConfInfo->GetConfName() );
			
            //换一种方式传递终端信息
            //tConfFullInfo.SetConfLineNum( pSatConfInfo->GetConfLineNum() );
            //tConfFullInfo.SetIntervTime( pSatConfInfo->GetSatIntervTime() );

			//构造消息体
			CServMsg cServMsg;
			cServMsg.SetEventId( MT_MCU_CREATECONF_REQ );
			cServMsg.SetMcuId(LOCAL_MCUID);
            cServMsg.SetSrcMtId(byMtId);
			cServMsg.SetMsgBody( (u8*)&tConfFullInfo, sizeof( tConfFullInfo ) );
			PostMessage2Conf( CInstance::PENDING, cServMsg );
		}
		break;

	case MT_MCU_DROPCONF_CMD:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			TMt		tMt;
			u8		byMtId = *( u8* )cSatMsg.GetSatMsgBody();

			log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received from mt %d.\n", pcMsg->event, ::OspEventDesc( pcMsg->event ), byMtId );

			tMt.SetMt( LOCAL_MCUID, byMtId );

			CServMsg cServMsg;
			cServMsg.SetEventId( MT_MCU_DROPCONF_CMD );
            cServMsg.SetMcuId(tMt.GetMcuId());
            cServMsg.SetSrcMtId(tMt.GetMtId());

			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );
		}
		break;

    //FIXME:
    /*
	case MT_MCU_APPLYJOINCONF_REQ:
	case MT_MCU_APPLYQUITCONF_CMD:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			TMt		tMt;
			u8		byMtId = *( u8* )cSatMsg.GetSatMsgBody();

			log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received from mt %d.\n", pcMsg->event, ::OspEventDesc( pcMsg->event ), byMtId );

			tMt.SetMt( LOCAL_MCUID, byMtId );

			CServMsg cServMsg;
			cServMsg.SetEventId( pcMsg->event );
			cServMsg.SetSrcMt( tMt.GetMcuId(), tMt.GetMtId() );
			cServMsg.SetMsgBody( (u8*)&tMt, sizeof( tMt ) );
			
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );

		}
		break;

	
	case MT_MCU_CANCELCHAIRMAN_CMD:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			TMt		tMt;
			u8		byMtId = *( u8* )cSatMsg.GetSatMsgBody();

			log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received from mt %d.\n", pcMsg->event, ::OspEventDesc( pcMsg->event ), byMtId );

			tMt.SetMt( LOCAL_MCUID, byMtId );

			CServMsg cServMsg;
			cServMsg.SetEventId( MT_MCU_CANCELCHAIRMAN_CMD );
			cServMsg.SetSrcMt( tMt.GetMcuId(), tMt.GetMtId() );
			
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );
		}
		break;
        */

	case MT_MCU_APPLYCHAIRMAN_REQ:
		{
			log( LOGLVL_EXCEPTION, "Test");
			
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			TMt		tMt;
			u8		byMtId = *( u8* )cSatMsg.GetSatMsgBody();

			log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received from mt %d.\n", pcMsg->event, ::OspEventDesc( pcMsg->event ), byMtId );

			tMt.SetMt( LOCAL_MCUID, byMtId );

			CServMsg cServMsg;
			cServMsg.SetEventId( MT_MCU_APPLYCHAIRMAN_REQ );
            cServMsg.SetMcuId(tMt.GetMcuId());
            cServMsg.SetSrcMtId(tMt.GetMtId());
			cServMsg.SetMsgBody( ( u8* )&tMt, sizeof( tMt ) );
			
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );
		}
		break;
	
	case MT_MCU_APPLYSPEAKER_NOTIF:
		{
	
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			TMt		tMt;
			u8		byMtId = *( u8* )cSatMsg.GetSatMsgBody();

			log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received from mt %d.\n", pcMsg->event, ::OspEventDesc( pcMsg->event ), byMtId );

			tMt.SetMt( LOCAL_MCUID, byMtId );

			CServMsg cServMsg;
			cServMsg.SetEventId( MT_MCU_APPLYSPEAKER_NOTIF );
            cServMsg.SetMcuId(tMt.GetMcuId());
            cServMsg.SetSrcMtId(tMt.GetMtId());
			cServMsg.SetMsgBody( ( u8* )&tMt, sizeof( tMt ) );
			
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );
		}
		break;

	case MT_MCU_SPECSPEAKER_REQ:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			TMt		tMt;
			TMt		tSpeaker;
			
			u8		byMtId = *( u8* )cSatMsg.GetSatMsgBody();
			u8		bySpeakerId = *( u8* )( cSatMsg.GetSatMsgBody() + 1 );

			log( LOGLVL_EXCEPTION, "CMtSsnInst: Message %u(%s) received from mt %d.\n", pcMsg->event, ::OspEventDesc( pcMsg->event ), byMtId );

			tMt.SetMt( LOCAL_MCUID, byMtId );
			tSpeaker.SetMt( LOCAL_MCUID, bySpeakerId );

			CServMsg cServMsg;
			cServMsg.SetEventId( MT_MCU_SPECSPEAKER_REQ );
            cServMsg.SetMcuId(tMt.GetMcuId());
            cServMsg.SetSrcMtId(tMt.GetMtId());
			cServMsg.SetMsgBody( ( u8* )&tSpeaker, sizeof( tSpeaker ) );
			
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );
		}
		break;

	case MT_MCU_CANCELSPEAKER_CMD:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			TMt		tMt;
			u8		byMtId = *( u8* )cSatMsg.GetSatMsgBody();

			log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received from mt %d.\n", pcMsg->event, ::OspEventDesc( pcMsg->event ), byMtId );

			tMt.SetMt( LOCAL_MCUID, byMtId );

			CServMsg cServMsg;
			cServMsg.SetEventId( MT_MCU_CANCELSPEAKER_CMD );
            cServMsg.SetMcuId(tMt.GetMcuId());
            cServMsg.SetSrcMtId(tMt.GetMtId());
			
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );
		}
		break;

	case MT_MCU_STARTSELMT_CMD:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			TSwitchInfo tSwitchInfo;
			
			u8		byDstMtId = *( u8* )cSatMsg.GetSatMsgBody();
			u8		bySrcMtId = *( u8* )( cSatMsg.GetSatMsgBody() + 1 );

			log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received from mt %d.\n", pcMsg->event, ::OspEventDesc( pcMsg->event ), byDstMtId );

			tSwitchInfo.SetMode(MODE_BOTH);

            TMt tDstMt;
            tDstMt.SetMt(LOCAL_MCUID, byDstMtId);
			tSwitchInfo.SetDstMt(tDstMt);

            TMt tSrcMt;
            tSrcMt.SetMt(LOCAL_MCUID, bySrcMtId);
			tSwitchInfo.SetSrcMt(tSrcMt);			

			CServMsg cServMsg;
			cServMsg.SetEventId( MT_MCU_STARTSELMT_CMD );
            cServMsg.SetMcuId(LOCAL_MCUID);
            cServMsg.SetSrcMtId(byDstMtId);
			cServMsg.SetMsgBody( (u8*)&tSwitchInfo, sizeof( tSwitchInfo ) );
			
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );
		}
		break;

	case MT_MCU_STOPSELMT_CMD:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			TSwitchInfo tSwitchInfo;
			
			u8		byDstMtId = *( u8* )cSatMsg.GetSatMsgBody();

			log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received from mt %d.\n", pcMsg->event, ::OspEventDesc( pcMsg->event ), byDstMtId );

			tSwitchInfo.SetMode(MODE_BOTH);
            TMt tMt;
            tMt.SetMt(LOCAL_MCUID, byDstMtId);
			tSwitchInfo.SetDstMt(tMt);
			

			CServMsg cServMsg;
			cServMsg.SetEventId( MT_MCU_STOPSELMT_CMD );
            cServMsg.SetMcuId(LOCAL_MCUID);
            cServMsg.SetSrcMtId(byDstMtId);
			cServMsg.SetMsgBody( (u8*)&tSwitchInfo, sizeof( tSwitchInfo ) );
			
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );
		}
		break;

	//case MT_MCU_STARTBROADCASTMT_CMD:
	case MT_MCU_STOPBROADCASTMT_CMD:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			TMt		tMt;
			u8      byMtId;
			u8      byTargetMtId;

			byMtId = *( u8* )cSatMsg.GetSatMsgBody();
			byTargetMtId = *( u8* )( cSatMsg.GetSatMsgBody() + sizeof( u8 ) );

			log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received from mt %d.\n", pcMsg->event, ::OspEventDesc( pcMsg->event ), byMtId );

			CServMsg cServMsg;
			cServMsg.SetEventId( pcMsg->event );
            cServMsg.SetMcuId(LOCAL_MCUID);
            cServMsg.SetSrcMtId(byMtId);

			tMt.SetMt( LOCAL_MCUID, byTargetMtId );
			cServMsg.SetMsgBody( (u8*)&tMt, sizeof( tMt ) );

			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );	

		}
		break;

	case MT_MCU_SPECCHAIRMAN_REQ:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			TMt		tMt;
			u8      byMtId;
			u8      byTargetMtId;
			
			byMtId = *( u8* )cSatMsg.GetSatMsgBody();
			byTargetMtId = *( u8* )( cSatMsg.GetSatMsgBody() + sizeof( u8 ) );
			
			log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received from mt %d.\n", pcMsg->event, ::OspEventDesc( pcMsg->event ), byMtId );
			
			CServMsg cServMsg;
			cServMsg.SetEventId( pcMsg->event );
            cServMsg.SetMcuId(LOCAL_MCUID);
            cServMsg.SetSrcMtId(byMtId);
			
			tMt.SetMt( LOCAL_MCUID, byTargetMtId );
			cServMsg.SetMsgBody( (u8*)&tMt, sizeof( tMt ) );
			
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );	
			
		}
		break;

	case MCU_MULTICAST_MSG:
		{
            CServMsg cServMsg( pcMsg->content, pcMsg->length );
            
            //FIXME: 这个在MCUVC业务里怎么体现
			//u32 dwDstIp = g_cMpManager.GetMpAddr();
            //if ( dwDstIp == 0 ) //如果MP未注册上来则无法发送
            //	break;
            
            //回答上面的问题，暂时这样处理
            
            u32  dwDstIp;
            for (u8 byMpId = 0; byMpId < MAXNUM_DRI; byMpId++)
            {
                if (g_cMcuVcApp.IsMpConnected(byMpId))
                {
                    dwDstIp = g_cMcuVcApp.GetMpIpAddr(byMpId);
                }
            }

			u16	wMulticastPort = g_cMcuVcApp.GetExistSatCastPort();

            //只发高清组
			if (SIGNAL_CHN_HD == cServMsg.GetChnIndex())
			{
				dwDstIp = 0xe10a0a0b;
				dwDstIp = htonl(dwDstIp);
				
				wMulticastPort = 8400;
				
				if( m_hSocket != INVALID_SOCKET )
				{
					SOCKADDR_IN tUdpAddr;
					int         nTotalSendLen;
					
					memset( &tUdpAddr, 0, sizeof( tUdpAddr ) );
					tUdpAddr.sin_family = AF_INET; 
					
#ifdef WIN32
					tUdpAddr.sin_port        = htons(8400);
					tUdpAddr.sin_addr.s_addr = dwDstIp;
#else
					tUdpAddr.sin_port        = htons( wMulticastPort );
					tUdpAddr.sin_addr.s_addr = 	dwDstIp;
#endif
					
					nTotalSendLen = sendto( m_hSocket,
						( char* )cServMsg.GetMsgBody(),
						cServMsg.GetMsgBodyLen(),
						0, 
						(struct sockaddr *)&tUdpAddr, sizeof ( tUdpAddr ) );
					
					if( nTotalSendLen < cServMsg.GetMsgBodyLen() )
					{
						log( LOGLVL_EXCEPTION, "CMtSsnInst: Send Control Signal To Multicast Address Failed.\n" );
					}
					else
					{
						SatLog("[MCU_MULTICAST_MSG] %d<%s> to 0x%x@%d!\n",
                            cServMsg.GetEventId(),
                            OspEventDesc(cServMsg.GetEventId()),
                            dwDstIp,
                            wMulticastPort); 
					}
				}

			}
			//只发标清组
			else if (SIGNAL_CHN_SD == cServMsg.GetChnIndex())
			{
				dwDstIp = 0xe10a0a0a;
				dwDstIp = htonl(dwDstIp);
				
				wMulticastPort = 8400;
				
				if( m_hSocket != INVALID_SOCKET )
				{
					SOCKADDR_IN tUdpAddr;
					int         nTotalSendLen;
					
					memset( &tUdpAddr, 0, sizeof( tUdpAddr ) );
					tUdpAddr.sin_family = AF_INET; 
					
#ifdef WIN32
					tUdpAddr.sin_port        = htons(8400);
					tUdpAddr.sin_addr.s_addr = dwDstIp;
#else
					tUdpAddr.sin_port        = htons( wMulticastPort );
					tUdpAddr.sin_addr.s_addr = 	dwDstIp;
#endif
					
					nTotalSendLen = sendto( m_hSocket,
						( char* )cServMsg.GetMsgBody(),
						cServMsg.GetMsgBodyLen(),
						0, 
						(struct sockaddr *)&tUdpAddr, sizeof ( tUdpAddr ) );
					
					if( nTotalSendLen < cServMsg.GetMsgBodyLen() )
					{
						log( LOGLVL_EXCEPTION, "CMtSsnInst: Send Control Signal To Multicast Address Failed.\n" );
					}
					else
					{
						SatLog("[MCU_MULTICAST_MSG] %d<%s> to 0x%x@%d!\n",
                            cServMsg.GetEventId(),
                            OspEventDesc(cServMsg.GetEventId()),
                            dwDstIp,
                            wMulticastPort); 
					}
				}

			}
			//高标组都发
			else
			{
				dwDstIp = 0xe10a0a0a;
				dwDstIp = htonl(dwDstIp);
				
				wMulticastPort = 8400;
				
				if( m_hSocket != INVALID_SOCKET )
				{
					SOCKADDR_IN tUdpAddr;
					int         nTotalSendLen;
					
					memset( &tUdpAddr, 0, sizeof( tUdpAddr ) );
					tUdpAddr.sin_family = AF_INET; 
					
#ifdef WIN32
					tUdpAddr.sin_port        = htons(8400);
					tUdpAddr.sin_addr.s_addr = dwDstIp;
#else
					tUdpAddr.sin_port        = htons( wMulticastPort );
					tUdpAddr.sin_addr.s_addr = 	dwDstIp;
#endif
					
					nTotalSendLen = sendto( m_hSocket,
						( char* )cServMsg.GetMsgBody(),
						cServMsg.GetMsgBodyLen(),
						0, 
						(struct sockaddr *)&tUdpAddr, sizeof ( tUdpAddr ) );
					
					if( nTotalSendLen < cServMsg.GetMsgBodyLen() )
					{
						log( LOGLVL_EXCEPTION, "CMtSsnInst: Send Control Signal To Multicast Address Failed.\n" );
					}
					else
					{
						SatLog("[MCU_MULTICAST_MSG] %d<%s> to 0x%x@%d!\n",
                            cServMsg.GetEventId(),
                            OspEventDesc(cServMsg.GetEventId()),
                            dwDstIp,
                            wMulticastPort); 
					}
				}

				dwDstIp = 0xe10a0a0b;
				dwDstIp = htonl(dwDstIp);
				
				wMulticastPort = 8400;
				
				if( m_hSocket != INVALID_SOCKET )
				{
					SOCKADDR_IN tUdpAddr;
					int         nTotalSendLen;
					
					memset( &tUdpAddr, 0, sizeof( tUdpAddr ) );
					tUdpAddr.sin_family = AF_INET; 
					
#ifdef WIN32
					tUdpAddr.sin_port        = htons(8400);
					tUdpAddr.sin_addr.s_addr = dwDstIp;
#else
					tUdpAddr.sin_port        = htons( wMulticastPort );
					tUdpAddr.sin_addr.s_addr = 	dwDstIp;
#endif
					
					nTotalSendLen = sendto( m_hSocket,
						( char* )cServMsg.GetMsgBody(),
						cServMsg.GetMsgBodyLen(),
						0, 
						(struct sockaddr *)&tUdpAddr, sizeof ( tUdpAddr ) );
					
					if( nTotalSendLen < cServMsg.GetMsgBodyLen() )
					{
						log( LOGLVL_EXCEPTION, "CMtSsnInst: Send Control Signal To Multicast Address Failed.\n" );
					}
					else
					{
						SatLog("[MCU_MULTICAST_MSG] %d<%s> to 0x%x@%d!\n",
                            cServMsg.GetEventId(),
                            OspEventDesc(cServMsg.GetEventId()),
                            dwDstIp,
                            wMulticastPort); 
					}
				}

			}
            
        }
		break;

        /*

			CServMsg cServMsg( pcMsg->content, pcMsg->length );
			CSatMsg	 cSatMsg;
			CSatConfInfo cSatConfInfo;
			cSatMsg.SetConfId( cServMsg.GetChnIndex() );
			cSatMsg.SetEventId( cServMsg.GetEventId() );
			cSatMsg.SetSatMsgBody( cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );

			assert( cSatMsg.IsValidSatMsg() );

            //FIXME: 这个在MCUVC业务里怎么体现
			//u32  dwDstIp = g_cMpManager.GetMpAddr();
			//if ( dwDstIp == 0 ) //如果MP未注册上来则无法发送
			//	break;
            
            //回答上面的问题，暂时这样处理

			u32  dwDstIp;
            for (u8 byMpId = 0; byMpId < MAXNUM_DRI; byMpId++)
            {
                if (g_cMcuVcApp.IsMpConnected(byMpId))
                {
                    dwDstIp = g_cMcuVcApp.GetMpIpAddr(byMpId);
                }
            }
            
//			g_cMcuAgent.GetMpIpAddr( &dwDstIp, 4 );
			if( m_hSocket != INVALID_SOCKET )
			{
				SOCKADDR_IN tUdpAddr;
				int         nTotalSendLen;
				u32			dwMulticastAddr = g_cMcuVcApp.GetExistSatCastIp();
				u16			wMulticastPort = g_cMcuVcApp.GetExistSatCastPort();
				
                //FIXME: 这个通过配置文件实现
				//g_cMcuAgent.GetMulticastSignalInfo( &dwMulticastAddr, &wMulticastPort );				
				memset( &tUdpAddr, 0, sizeof( tUdpAddr ) );
				tUdpAddr.sin_family = AF_INET; 
        #ifdef WIN32
				tUdpAddr.sin_port = htons(MCU_SENDTOMULTI_PORT);
				tUdpAddr.sin_addr.s_addr = dwDstIp;
        #else
				tUdpAddr.sin_port = htons( wMulticastPort );
				tUdpAddr.sin_addr.s_addr = 	dwDstIp;
        #endif
				
				assert( cSatMsg.IsValidSatMsg() );
				
				nTotalSendLen = sendto( m_hSocket,
                                        ( char* )cSatMsg.GetSatMsg(),
                                        cSatMsg.GetSatMsgLen(),
                                        0, 
					                    (struct sockaddr *)&tUdpAddr,
                                        sizeof ( tUdpAddr ) );

				if( nTotalSendLen < cSatMsg.GetSatMsgLen() )
				{
					log( LOGLVL_EXCEPTION, "CMtSsnInst: Send Control Signal To Multicast Address Failed.\n" );
				}
                else
                {
                    SatLog("[MCU_MULTICAST_MSG] %d<%s>"); 
                }
			}
		}
		break;
        */

	//终端到终端短消息
	case MT_MCU_SENDMSG_CMD:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			TMt		tSrcMt;
			TMt		tDstMt;
			u8 tSrcMtId = *( u8* )cSatMsg.GetSatMsgBody();
			u8 tDstMtId = *( u8* )(cSatMsg.GetSatMsgBody()+sizeof(u8));
			
			CServMsg cServMsg;
			tSrcMt.SetMt( LOCAL_MCUID, tSrcMtId );
			tDstMt.SetMt( LOCAL_MCUID, tDstMtId );
			cServMsg.SetEventId( pcMsg->event );
			cServMsg.SetMsgBody( (u8*)&tSrcMt, sizeof(TMt) );
			cServMsg.CatMsgBody( (u8*)&tDstMt, sizeof(TMt) );
			cServMsg.CatMsgBody( (u8*)( cSatMsg.GetSatMsgBody() + 2*sizeof( u8 ) ), cSatMsg.GetSatMsgBodyLen()-2*sizeof( u8 ) );
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );
		}
		break;

	//终端到会控短消息
	case MT_MCU_SENDMCMSG_CMD:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			
			CServMsg cServMsg;
			cServMsg.SetEventId( pcMsg->event );
			cServMsg.SetMsgBody( (u8*)( cSatMsg.GetSatMsgBody()), cSatMsg.GetSatMsgBodyLen() );
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );
		}
		break;

	//终端操作画面合成
	//case MT_MCU_STARTVMP_CMD:
	//case MT_MCU_STARTVMPBRDST_CMD:
	//case MT_MCU_STOPVMPBRDST_CMD:
	//case MT_MCU_STARTMIX_CMD:
    case 1:
		if ( FilterMessage( pcMsg->event ) )
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );

				log( LOGLVL_EXCEPTION, "CMtSsnInst: Message %u(%s) received!\n", pcMsg->event, ::OspEventDesc( pcMsg->event ) );
			
			CServMsg cServMsg;
			cServMsg.SetEventId( pcMsg->event );
			cServMsg.SetMsgBody( (u8*)( cSatMsg.GetSatMsgBody()), cSatMsg.GetSatMsgBodyLen() );
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );
		}
		break;
	//case MT_MCU_STOPMIX_CMD:
	//case MT_MCU_PAUSEMIX_CMD:
	//case MT_MCU_RESUMEMIX_CMD:
	case MT_MCU_ADDMIXMEMBER_CMD:
	//case MT_MCU_DELMIXMEMBER_CMD:
	//case MT_MCU_CHANGEVMPPARAM_CMD:
	//case MT_MCU_STOPVMP_CMD:
	//case MT_MCU_SIGNNOW_NOTIF:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );

				log( LOGLVL_EXCEPTION, "CMtSsnInst: Message %u(%s) received!\n", pcMsg->event, ::OspEventDesc( pcMsg->event ) );
			
			CServMsg cServMsg;
			cServMsg.SetEventId( pcMsg->event );
			cServMsg.SetMsgBody( (u8*)( cSatMsg.GetSatMsgBody()), cSatMsg.GetSatMsgBodyLen() );
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );
		}
		break;

	case MT_MCU_MTDUMB_CMD:
	case MT_MCU_MTMUTE_CMD:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			CServMsg cServMsg;
			cServMsg.SetEventId( pcMsg->event );
			cServMsg.SetMsgBody( (u8*)(cSatMsg.GetSatMsgBody()), cSatMsg.GetSatMsgBodyLen() );
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );
		}
		break;

    //case MT_MCU_ADDMT_REQ:
    //case MT_MCU_DELMT_REQ:
	case MT_MCU_ADDMT_REQ:
	case MT_MCU_DELMT_REQ:
		{
			CSatMsg cSatMsg( pcMsg->content, pcMsg->length );
			CServMsg cServMsg;
			cServMsg.SetEventId( pcMsg->event );
			cServMsg.SetMsgBody(  (u8*)(cSatMsg.GetSatMsgBody()), cSatMsg.GetSatMsgBodyLen() );
			PostMessage2Conf( cSatMsg.GetConfId(), cServMsg );
		}
		break;

	case 25011/*MCU_MT_SENDIFRAME_NOTIF*/:
		{
			CServMsg cServMsg;
			
			ITSatMsgHdr tSatHdr;
			ITSatMtId   tSatMtId;
			
			tSatHdr = *(ITSatMsgHdr*)pcMsg->content;
			tSatMtId = *(ITSatMtId*)( pcMsg->content + sizeof(ITSatMsgHdr) );

			cServMsg.SetEventId( MT_MCU_FASTUPDATEPIC_CMD );
			cServMsg.SetSrcMtId( tSatHdr.GetSrcId() );
			cServMsg.SetConfIdx( tSatHdr.GetConfIdx() );

			u8 byMode = MODE_VIDEO;
			cServMsg.SetMsgBody(&byMode, sizeof(u8));
			PostMessage2Conf( tSatHdr.GetConfIdx(), cServMsg );
			
			byMode = MODE_SECVIDEO;
			cServMsg.SetMsgBody(&byMode, sizeof(u8));
			PostMessage2Conf( tSatHdr.GetConfIdx(), cServMsg );

		}
		break;

	case 30254: /*MCU_MT_STARTDS_REQ*/
		{
			CServMsg cServMsg;
			
			ITSatMsgHdr tSatHdr;
			ITSatMtId   tSatMtId;
			u8 byStartDS = 0;
			
			tSatHdr = *(ITSatMsgHdr*)pcMsg->content;
			//tSatMtId = *(ITSatMtId*)( pcMsg->content + sizeof(ITSatMsgHdr) );
			byStartDS = *(pcMsg->content + sizeof(ITSatMsgHdr));// + sizeof(ITSatMtId));

			//发起双流
			if (0 != byStartDS)
			{
				cServMsg.SetEventId( MT_MCU_OPENLOGICCHNNL_REQ );
				cServMsg.SetSrcMtId( tSatHdr.GetSrcId() );
				cServMsg.SetConfIdx( tSatHdr.GetConfIdx() );
				
				TLogicalChannel tLogicChn;
				tLogicChn.SetMediaType(MODE_SECVIDEO);
				cServMsg.SetMsgBody((u8*)&tLogicChn, sizeof(tLogicChn));
				PostMessage2Conf( tSatHdr.GetConfIdx(), cServMsg );

				//MtssnLog
				OspPrintf(TRUE, FALSE, "[MCU_MT_STARTDS_REQ] bStart.TRUE from mt.%d!\n", tSatHdr.GetSrcId());
			}
			//停止双流
			else
			{
				cServMsg.SetEventId( MT_MCU_CLOSELOGICCHNNL_NOTIF );
				cServMsg.SetSrcMtId( tSatHdr.GetSrcId() );
				cServMsg.SetConfIdx( tSatHdr.GetConfIdx() );
				
				u8 byOutgoingChn = 0;
				TLogicalChannel tLogicChn;
				tLogicChn.SetMediaType(MODE_SECVIDEO);
				cServMsg.SetMsgBody((u8*)&tLogicChn, sizeof(tLogicChn));
				cServMsg.CatMsgBody(&byOutgoingChn, sizeof(u8));
				PostMessage2Conf( tSatHdr.GetConfIdx(), cServMsg );

				OspPrintf(TRUE, FALSE, "[MCU_MT_STARTDS_REQ] bStart.FALSE from mt.%d!\n", tSatHdr.GetSrcId());
			}
		}
		break;

	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received, Inst.%d!\n",
                                pcMsg->event, ::OspEventDesc( pcMsg->event ), GetInsID() );
		break;
	}
}

/*====================================================================
    函数名      ：DaemonProcPowerOn
    功能        ：处理系统启动消息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/10    1.0			JQL        创建
====================================================================*/
void CMtSsnInst::DaemonProcPowerOn( const CMessage * pcMsg, CApp * pcApp )
{
	u16 wIndex;
	TMt atMt[MAXNUM_MCU_MT];
	u16 wMtNum;
	CServMsg cServMsg;

	wMtNum = topoGetAllSubMt( LOCAL_MCUID, g_atMtTopo, g_wMtTopoNum, atMt, sizeof( atMt )/sizeof( atMt[0] ) );

    OspPrintf(TRUE, FALSE, "[DaemonProcPowerOn] topGetAllSubMt.%d with MtTopoNum.%d!\n",
                            wMtNum, g_wMtTopoNum);
    printf("[DaemonProcPowerOn] topGetAllSubMt.%d with MtTopoNum.%d!\n",
                            wMtNum, g_wMtTopoNum);
	
	for( wIndex = 0; wIndex < wMtNum && wIndex < pcApp->GetInstanceNumber(); wIndex++ )
	{
		//模拟终端建链过程
		//终端会话的实例号对应终端的ID号
        TMt tMtSrc;
        tMtSrc.SetMt( LOCAL_MCUID, atMt[wIndex].GetMtId() );
        cServMsg.SetMcuId( LOCAL_MCUID );
		cServMsg.SetEventId( MT_MCU_REG_REQ );
		cServMsg.SetMsgBody( ( u8* )&atMt[wIndex], sizeof( atMt[wIndex] ) );
		post( MAKEIID( GetAppID(), atMt[wIndex].GetMtId() ), MT_MCU_REG_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() ) ;

		//模拟发送终端就绪
		cServMsg.SetEventId( MT_MCU_READY_NOTIF );
		cServMsg.SetMsgBody( ( u8* )&atMt[wIndex], sizeof( atMt[wIndex] ) );
		post( MAKEIID( GetAppID(), atMt[wIndex].GetMtId() ), MT_MCU_READY_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() ) ;
	}

	//Create UDP Signalling Socket
	u32 dwMuticastAddr = g_cMcuVcApp.GetExistSatCastIp();
	u16 wPort = g_cMcuVcApp.GetExistSatCastPort();

    
    //FIXME: 通过配置文件获取
	if( 0 == dwMuticastAddr || 0 == wPort )//!g_cMcuAgent.GetMulticastInfo( &dwMuticastAddr, &wPort ) )
	{
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Cannot Get Multicast info.\n" );
		return;
	}
	
	//Bind this socket at multicast Port, In order to receive multicast signal packet. 
	if( ( m_hSocket = CreateUdpSocket( g_cMcuAgent.GetMpcIp(), 0 ) ) == INVALID_SOCKET )
	{
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Create Signal UDP Socket Error.\n" );
		return;
	}

	//join the signal broadcast address
//	if( !JoinSocketMulticast( m_hSocket, dwMuticastAddr, g_cMcuAgent.GetVcIpAddr() ) )
//	{
//		log( LOGLVL_EXCEPTION, "CMtSsnInst: Join MulitCastAddr Error.\n" );
//		return;
//	}
//	else
//	{
//		log( LOGLVL_DEBUG1, "CMtSsnInst: Receive Conference Multicast Signal Address %.8x, Port %d.\n", 
//							g_cMcuAgent.GetVcIpAddr(), wPort );
//	}
}

void CMtSsnInst::InstanceExit()
{
	if( m_hSocket != INVALID_SOCKET )
	{
		u32 dwMulticastAddr = g_cMcuAgent.GetCastIpAddr();
		u16 wPort = g_cMcuAgent.GetCastPort();
		
        //FIXME: 通过配置文件获取
		//g_cMcuAgent.GetMulticastInfo( &dwMulticastAddr, &wPort );
		
		DropSocketMulticast( m_hSocket, dwMulticastAddr, 0/*FIXME: 通过配置文件获取 g_cMcuAgent.GetVcIpAddr() */);
	}
}

/*====================================================================
    函数名      ：SendMsgToSubMt
    功能        ：发消息给本实例对应的下级终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/28    1.0         LI Yi         创建
====================================================================*/
/*
#ifndef H323
BOOL CMtSsnInst::SendMsgToSubMt( u16 wEvent, u8 *  pbyMsg, u16 wLen )
{

	if( m_hSocket == INVALID_SOCKET )
    {
		return FALSE;
    }

	CServMsg cServMsg( pbyMsg, wLen );
	
	switch ( wEvent )
	{
	//case MCU_MT_MTSTATUS_NOTIF:
	case MCU_MT_SENDMSG_NOTIF:
		break;
	case MCU_MT_APPLYCHAIRMAN_NOTIF:
		 cServMsg.SetEventId(MCU_MT_APPLYCHAIRMAN_NOTIF );
		 break;
	case MCU_MT_APPLYSPEAKER_NOTIF:
		cServMsg.SetEventId(MCU_MT_APPLYSPEAKER_NOTIF );
		break;
	case 25011://MCU_MT_SENDIFRAME_NOTIF:
		cServMsg.SetEventId(25011);
		break;
	default:
		return TRUE;
	}	

	SOCKADDR_IN tUdpAddr;
	int         nTotalSendLen;

    //FIXME: 要考虑卫星会议的组播和非卫星会议的组播的耦合性
	u32 dwIP = g_cMcuVcApp.GetExistSatCastIp();
	u16 wPort = g_cMcuVcApp.GetExistSatCastPort();

	//g_cMcuAgent.GetMulticastSignalInfo(&dwIP, &wPort);
	memset( &tUdpAddr, 0, sizeof( tUdpAddr ) );
    tUdpAddr.sin_family = AF_INET; 
    tUdpAddr.sin_port = htons(wPort);
	tUdpAddr.sin_addr.s_addr = 	::topoGetMtInfo( LOCAL_MCUID,
                                                 m_byMtId,
                                                 g_atMtTopo, 
			                                     g_wMtTopoNum ).GetIpAddr();
	
	CSatMsg cSatMsg;
	if ( wEvent == MCU_MT_SENDMSG_NOTIF )
    {
		cSatMsg.SetEventId( MCU_MT_SENDMSG_NOTIF );
    }
	else
    {
		cSatMsg.SetEventId( cServMsg.GetEventId() );
    }
	cSatMsg.SetConfId( cServMsg.GetChnIndex() );
	cSatMsg.SetSatMsgBody( cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
	assert( cSatMsg.IsValidSatMsg() );
	
	nTotalSendLen = sendto( m_hSocket,
                            ( char* )cSatMsg.GetSatMsg(),
                            cSatMsg.GetSatMsgLen(),
                            0, 
							(struct sockaddr *)&tUdpAddr, sizeof ( tUdpAddr ) );

	if( nTotalSendLen < cSatMsg.GetSatMsgLen() )
	{
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Send Control Signal To Mt %d Failed.\n.", m_byMtId );
	}
    else
    {
        SatLog("[SendMsgToSubMt] send signal ctrl msg.%d<%s> to Mt.%d\n",
                wEvent,
                OspEventDesc(wEvent), m_byMtId);
    }
	return TRUE;

}
#endif
*/

BOOL CMtSsnInst::SendMsgToSubMt( u16 wEvent, u8 *  pbyMsg, u16 wLen )
{
	ITSatMsgHdr tSatHdr;
	CServMsg cServMsg( pbyMsg, wLen );
	
	if( m_hSocket == INVALID_SOCKET )
		return FALSE;
	
	switch ( wEvent )
	{
	//case MCU_MT_MTSTATUS_NOTIF:
	case MCU_MT_SENDMSG_NOTIF:
	case MCU_MT_APPLYCHAIRMAN_NOTIF:
	case MCU_MT_APPLYSPEAKER_NOTIF:
	case 25011://MCU_MT_SENDIFRAME_NOTIF:
	case 30255://STARTSAT_RSP
		break;
	default:
		return TRUE;
	}	
	tSatHdr.SetConfIdx( cServMsg.GetConfIdx() );
	tSatHdr.SetEventId( wEvent/*cServMsg.GetEventId()*/ );
	//tSatHdr.SetMsgSN( g_cMtSsnApp.GetSN() );
	tSatHdr.SetSrcId( cServMsg.GetSrcMtId() );
	tSatHdr.SetDstId( cServMsg.GetDstMtId() );	
	cServMsg.SetMsgBody( (u8*)&tSatHdr, sizeof(ITSatMsgHdr) );
	
	cServMsg.CatMsgBody( pbyMsg + 48, wLen - 48 );
	cServMsg.SetChnIndex(SIGNAL_CHN_BOTH);
	g_cMtSsnApp.SendMultiCastMsg( cServMsg );

	if (wEvent == 25011)
	{
		OspPrintf(TRUE, FALSE, "[SendMsgToSubMt] MCU_MT_SENDIFRAME_NOTIF send to mt.%d!\n", cServMsg.GetDstMtId());
	}
	return TRUE;
	
}
/*====================================================================
    函数名      ：ProcMcuMtCapbilitySetNotif
    功能        ：MCU发送给终端的能力集通知
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wEvent, 事件号
				  u8 * const pbyMsg, 发送的消息指针，缺省为NULL
				  u16 wLen, 消息长度，缺省为0
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/10    1.0         JQL         创建
====================================================================*/
void CMtSsnInst::ProcMcuMtCapbilitySetNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt tDstMt = *( TMt* )cServMsg.GetMsgBody();
	
    TLogicalChannel tLogicalChannel;
	memset( &tLogicalChannel, 0, sizeof( tLogicalChannel ) );

    TCapSupport tCapSupport = *(TCapSupport*)(cServMsg.GetMsgBody() + sizeof(TMt));
	TCapSupportEx tCapEx  = *(TCapSupportEx*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TCapSupport));
	u32 dwRcvIpAddr;
	BOOL bMtHd = FALSE;

	switch( CurState() ) 
	{
	case STATE_NORMAL:
	case STATE_CONNECTED: //终端主动加入

		//高清终端取会议主能力打开后向逻辑通道
		//标清终端取辅能力；如果会议没有辅能力，放弃面向标清的通道打开
		bMtHd = ::topoGetMtInfo( LOCAL_MCUID, tDstMt.GetMtId(), g_atMtTopo, g_wMtTopoNum ).IsMtHd();
		
		if (!bMtHd &&
			MEDIA_TYPE_H264 == tCapSupport.GetMainVideoType())
		{
			if (tCapSupport.GetSecVideoType() == MEDIA_TYPE_NULL)
			{
				log( LOGLVL_EXCEPTION, "CMtSsnInst: Mt.%d could't open F/R logicChn due to ConfSecCap.Null!\n", tDstMt.GetMtId());
				return;
			}
			else
			{
				TSimCapSet tSimCap = tCapSupport.GetSecondSimCapSet();
				tCapSupport.SetMainSimCapSet(tSimCap);

				//模拟单音频能力交叉交互
				u8 byMainAudType = tCapSupport.GetMainAudioType();
				if (tCapSupport.GetMainAudioType() == MEDIA_TYPE_NULL)
				{
					TSimCapSet tSimCapSet = tCapSupport.GetMainSimCapSet();
					tSimCapSet.SetAudioMediaType(byMainAudType);
					tCapSupport.SetMainSimCapSet(tSimCapSet);
				}
			}
		}
/*
		//双流能力整理
		if (!bMtHd && tCapEx.IsDDStreamCap())
		{
			TDStreamCap tDStreamCap;
			tDStreamCap.SetMediaType(tCapEx.GetSecDSType());
			tDStreamCap.SetResolution(tCapEx.GetSecDSRes());
			tDStreamCap.SetFrameRate(tCapEx.GetSecDSFrmRate());
			tDStreamCap.SetMaxBitRate(tCapEx.GetSecDSBitRate());
			tDStreamCap.SetSupportH239(tCapSupport.IsDStreamSupportH239());

			tCapSupport.SetDStreamCapSet(tDStreamCap);
		}
*/		
        //AUDIO
		tLogicalChannel.m_byMediaType = MODE_AUDIO;
        tLogicalChannel.SetChannelType(tCapSupport.GetMainAudioType());

		//FIXME: 这里根据音频格式获取带宽
		//tLogicalChannel.SetFlowControl(48);

		cServMsg.SetSrcMtId( tDstMt.GetMtId() );
		cServMsg.SetMsgBody( ( u8* )&tLogicalChannel, sizeof( tLogicalChannel ) );
        cServMsg.SetMcuId( tDstMt.GetMcuId() );
        cServMsg.SetSrcMtId( tDstMt.GetMtId() );
		cServMsg.SetEventId( MT_MCU_OPENLOGICCHNNL_REQ );
		SendMsgToConf( cServMsg.GetConfIdx(),
			           MT_MCU_OPENLOGICCHNNL_REQ, 
					   cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

		//VIDEO
		tLogicalChannel.m_byMediaType = MODE_VIDEO;
        tLogicalChannel.SetChannelType(tCapSupport.GetMainVideoType());
        tLogicalChannel.SetVideoFormat(tCapSupport.GetMainVideoResolution());
		tLogicalChannel.SetFlowControl(tCapSupport.GetMainSimCapSet().GetVideoMaxBitRate());
        
        if (MEDIA_TYPE_H264 == tCapSupport.GetMainVideoType())
        {
            tLogicalChannel.SetChanVidFPS(tCapSupport.GetMainVidUsrDefFPS());
        }
        else
        {
            tLogicalChannel.SetChanVidFPS(tCapSupport.GetMainVidFrmRate());
        }
		cServMsg.SetSrcMtId( tDstMt.GetMtId() );
		cServMsg.SetMsgBody( ( u8* )&tLogicalChannel, sizeof( tLogicalChannel ) );
        cServMsg.SetMcuId( tDstMt.GetMcuId() );
        cServMsg.SetSrcMtId( tDstMt.GetMtId() );

		cServMsg.SetEventId( MT_MCU_OPENLOGICCHNNL_REQ );
		SendMsgToConf( cServMsg.GetConfIdx(),
			           MT_MCU_OPENLOGICCHNNL_REQ, 
					   cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

		//At the same time, We forge to cheat the VC to open the forward channel.
		//VIDEO
		dwRcvIpAddr = ::topoGetMtInfo( LOCAL_MCUID, tDstMt.GetMtId(), g_atMtTopo, g_wMtTopoNum ).GetIpAddr();
		
        //VIDEO_ACK
		tLogicalChannel.m_byMediaType = MODE_VIDEO;
        tLogicalChannel.SetChannelType(tCapSupport.GetMainVideoType());
        tLogicalChannel.SetVideoFormat(tCapSupport.GetMainVideoResolution());
        
        if (MEDIA_TYPE_H264 == tCapSupport.GetMainVideoType())
        {
            tLogicalChannel.SetChanVidFPS(tCapSupport.GetMainVidUsrDefFPS());
        }
        else
        {
            tLogicalChannel.SetChanVidFPS(tCapSupport.GetMainVidFrmRate());
        }

		tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( dwRcvIpAddr );
		tLogicalChannel.m_tRcvMediaChannel.SetPort( MT_RCVMCUMEDIA_PORT );
		tLogicalChannel.m_tRcvMediaCtrlChannel.SetIpAddr( dwRcvIpAddr );
		tLogicalChannel.m_tRcvMediaCtrlChannel.SetPort( MT_RCVMCUMEDIA_PORT + 1 );
        cServMsg.SetSrcMtId( tDstMt.GetMtId() );
		cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
		SendMsgToConf( cServMsg.GetConfIdx(),
			           MT_MCU_OPENLOGICCHNNL_ACK, 
					   cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

        //AUDIO_ACK
		tLogicalChannel.m_byMediaType = MODE_AUDIO;
		tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( dwRcvIpAddr );
        tLogicalChannel.SetChannelType(tCapSupport.GetMainAudioType());
		tLogicalChannel.m_tRcvMediaChannel.SetPort( MT_RCVMCUMEDIA_PORT + 2 );
		tLogicalChannel.m_tRcvMediaCtrlChannel.SetIpAddr( dwRcvIpAddr );
		tLogicalChannel.m_tRcvMediaCtrlChannel.SetPort( MT_RCVMCUMEDIA_PORT + 3 );
		cServMsg.SetSrcMtId( tDstMt.GetMtId() );
		cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
		SendMsgToConf( cServMsg.GetConfIdx(),
			           MT_MCU_OPENLOGICCHNNL_ACK, 
					   cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

        break;

	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u, Inst.%d!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMtMcuDaemonConfMsg
    功能        ：处理终端到业务Daemon实例的入口
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/27    1.0         LI Yi         创建
====================================================================*/
void CMtSsnInst::ProcMtMcuDaemonConfMsg( const CMessage * pcMsg )
{
	if( NULL == pcMsg )
	{
		log(LOGLVL_EXCEPTION, "CMtSsnInst: The received msg's pointer in the msg entry is NULL!");
		return;
	}
	
#ifdef _DEBUG
	log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received!\n", pcMsg->event, 
		::OspEventDesc( pcMsg->event ) );
#endif
	switch( CurState() )
	{
#ifndef H323
	case STATE_CONNECTED :
#endif
	case STATE_NORMAL:
	case STATE_WAITING:
		SendMsgToDaemonConf( pcMsg->event, pcMsg->content, pcMsg->length );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u, inst.%d!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID() );
		break;
	}
}


/*====================================================================
    函数名      ：ProcMtMcuRegReq
    功能        ：终端发起的登记请求处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
void CMtSsnInst::ProcMtMcuRegReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt		tRegMt = *( TMt * )cServMsg.GetMsgBody();

    OspPrintf(TRUE, FALSE, "CMtSsnInst: Message %u(%s) from MT%u received, InstId=%u!\n", 
		pcMsg->event, ::OspEventDesc( pcMsg->event ), tRegMt.GetMtId(), GetInsID() );
    
    printf("CMtSsnInst: Message %u(%s) from MT%u received, InstId=%u!\n", 
		pcMsg->event, ::OspEventDesc( pcMsg->event ), tRegMt.GetMtId(), GetInsID());

	//log( LOGLVL_IMPORTANT, "CMtSsnInst: Message %u(%s) from MT%u received, InstId=%u!\n", 
	//	pcMsg->event, ::OspEventDesc( pcMsg->event ), tRegMt.GetMtId(), GetInsID() );

	switch( CurState() )
	{
	case STATE_IDLE:

    //FIXME:    此处严格串行状态机
    case STATE_CONNECTED:
		//check the validation of registered MT
//		if( !g_cMcuAgent.IsSubMt( tRegMt.GetMtId() ) || tRegMt.GetMcuId() != LOCAL_MCUID|| )
		
        //FIXME: 这里是否需要从全网拓扑作参照对比
        /*
		if( !topoIsValidMt( tRegMt.GetMcuId(), tRegMt.GetMtId(), tRegMt.GetAlias(), g_atMtTopo, g_wMtTopoNum ) )
		{
			//send NACK
			log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong MT%u-%u registered and was refused!\n", tRegMt.GetMcuId(), tRegMt.GetMtId() );
			cServMsg.SetErrorCode( ERR_MCU_REGMT_INVALID );
			post( pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(), 
				cServMsg.GetServMsgLen(), pcMsg->srcnode );
			::OspDisconnectTcpNode( pcMsg->srcnode );
			return;
		}*/

		//cannot get its switch info
/*		if( !g_cMcuAgent.GetSubMtSwitchInfo( tRegMt.GetMtId(), dwSwitchIpAddr, wSwitchPort ) )
		{
			log( LOGLVL_EXCEPTION, "CMtSsnInst: Cannot get switch information of subordinate MT%u and was refused!\n", tRegMt.GetMtId() );
			cServMsg.SetErrorCode( ERR_MCU_REGMT_WRONGCONFIG );
			post( pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(), 
				cServMsg.GetServMsgLen(), pcMsg->srcnode );
			::OspDisconnectTcpNode( pcMsg->srcnode );
			return;
		}
*/		
		//accept and send Ack
#ifndef H323
		//自定义接口时需要将终端号保存为实例alias
/*		byTemp = tRegMt.GetMtId();
		if( SetAlias( ( const char * )&byTemp, sizeof( u8 ) ) == OSPERR_ALIAS_REPEAT )	//实例已存在，表示该终端已注册
 		{
			log( LOGLVL_EXCEPTION, "CMtSsnInst: This MT%u registered repeated once!\n", tRegMt.GetMtId() );
			cServMsg.SetErrorCode( ERR_MCU_REGMT_WRONGCONFIG );
			post( pcMsg->srcid, pcMsg->event + 2, cServMsg.GetServMsg(), 
				cServMsg.GetServMsgLen(), pcMsg->srcnode );
			::OspDisconnectTcpNode( pcMsg->srcnode );
			return;
		}*/

/*		if( ::OspNodeDiscCBReg( pcMsg->srcnode, GetAppID(), GetInsID() ) !=  OSP_OK ) 	//注册断链接收实例
		{
			log( LOGLVL_EXCEPTION, "CMtSsnInst:: Node %d disconnected before register callback!\n", pcMsg->srcnode );
			DeleteAlias();
			return;
		}*/
#endif

#ifndef H323
		m_dwMtNode = pcMsg->srcnode;	//save MT node
		m_byMtId = tRegMt.GetMtId();	//save MT ID
		
		NEXTSTATE( STATE_CONNECTED );
#endif
        NEXTSTATE( STATE_CONNECTED );
        printf("[ProcMtMcuRegReq] Inst.%d change state to.%d!\n", GetInsID(), STATE_CONNECTED);

		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMtDisconnect
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
void CMtSsnInst::ProcMtDisconnect( const CMessage * pcMsg )
{
	TMt	tMt;
	CServMsg	cServMsg;

	log( LOGLVL_EXCEPTION, "CMtSsnInst: OSP disconnect messege %u(%s) received!,Mt id is:%d.\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byMtId );

	switch( CurState() )
	{
#ifndef H323
	case STATE_CONNECTED:	//等待邀请
#endif
	case STATE_CALLING:		//等待邀请应答
	case STATE_NORMAL:
	case STATE_WAITING:
		log( LOGLVL_IMPORTANT, "CMtSsnInst: Message %u(%s) received! MT%u disconnected!\n", 
			 pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byMtId );

		//send notification to daemon conference instance
		tMt.SetMt( LOCAL_MCUID, m_byMtId );
		cServMsg.SetMsgBody( ( u8 * )&tMt, sizeof( tMt ) );
		SendMsgToDaemonConf( MCU_MTDISCONNECTED_NOTIF, cServMsg.GetServMsg(), 
			cServMsg.GetServMsgLen() );

		//clear
		m_byMtId = 0;
		DeleteAlias();	//Clear the Alias
#ifdef H323
		m_hsCall = NULL;
#else
		m_dwMtNode = INVALID_NODE;
#endif

		KillTimer( TIMER_WAIT_REPLY );
		KillTimer( TIMER_WAIT_CALL_RSP );
		KillTimer( TIMER_ROUNDTRIPDELAY_REQ );
		KillTimer( TIMER_WAIT_ROUNDTRIPDELAY_RSP );

		NEXTSTATE( STATE_IDLE );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMtMcuGetMcuTopoReq
    功能        ：终端查询MCU拓扑处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/09/20    1.0         LI Yi         创建
====================================================================*/
void CMtSsnInst::ProcMtMcuGetMcuTopoReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMtTopo		atMtInOneMcu[MAXNUM_MCU_MT];
	TMt		tMt = *( TMt * )cServMsg.GetMsgBody();
	u16		wMtNumInOneMcu;
	u16		wLoop, wLoop2;

	switch( CurState() )
	{
#ifndef H323
	case STATE_CONNECTED ://当前仅允许终端登记成功后进行一次拓扑获取操作，
						  //暂不允许动态获取拓扑
#else
	case STATE_IDLE:
#endif
		cServMsg.SetServMsg( ( u8 * )pcMsg->content, pcMsg->length );
		cServMsg.SetMsgBody( ( u8 *)g_atMcuTopo, sizeof( TMcuTopo ) * g_wMcuTopoNum );
	
#ifndef H323
		SendMsgToSubMt( pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
#else
		post( MAKEIID( AID_MT_MCUSSN, 1, TCP_GATEWAY ), pcMsg->event + 1, cServMsg.GetServMsg(), 
		      cServMsg.GetServMsgLen(), pcMsg->srcnode );
		log( LOGLVL_DEBUG1, "CMtSsnInst: Send message %u(%s) to subordinate MT%u!\n", 
			pcMsg->event + 1, ::OspEventDesc( pcMsg->event + 1 ), tMt.GetMtId() );
#endif

		//send MT topology notification
		for( wLoop = 0; wLoop < g_wMcuTopoNum; wLoop++ )
		{
			wMtNumInOneMcu = 0;
			for( wLoop2 = 0; wLoop2 < g_wMtTopoNum; wLoop2++ )
			{
				if( g_atMtTopo[wLoop2].GetMcuId() == g_atMcuTopo[wLoop].GetMcuId() )
                {
					atMtInOneMcu[wMtNumInOneMcu++] = g_atMtTopo[wLoop2];
                }
			}
	
			cServMsg.SetMsgBody( ( u8 * )atMtInOneMcu, wMtNumInOneMcu * sizeof( TMtTopo ) );
#ifndef H323
			SendMsgToSubMt( MCU_MT_MTTOPO_NOTIF, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
#else
			post( MAKEIID( AID_MT_MCUSSN, 1, TCP_GATEWAY ), MCU_MT_MTTOPO_NOTIF, cServMsg.GetServMsg(), 
				cServMsg.GetServMsgLen(), pcMsg->srcnode );
			log( LOGLVL_DEBUG1, "CMtSsnInst: Send message %u(%s) to subordinate MT%u!\n", 
				MCU_MT_MTTOPO_NOTIF, "MCU_MT_MTTOPO_NOTIF", tMt.GetMtId() );
#endif
		}

		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcuMtMsg
    功能        ：MCU发到终端一般消息处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/05    1.0         LI Yi         创建
    02/11/11    2.0         Liaoweijiang  修改
====================================================================*/
void CMtSsnInst::ProcMcuMtMsg( const CMessage * pcMsg )
{

	switch( CurState() )
	{
#ifndef H323
	case STATE_CONNECTED ://处理终端查询会议列表应答
#endif
	case STATE_NORMAL:
	case STATE_WAITING:
#ifndef H323
		SendMsgToSubMt( pcMsg->event, pcMsg->content, pcMsg->length );
#else
		SendH323MsgToSubMt( pcMsg );
#endif
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMtMcuMsg
    功能        ：终端发到MCU普通自发消息（非应答）处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/05    1.0         LI Yi         创建
    02/11/11    2.0         Liaoweijiang  修改
====================================================================*/
void CMtSsnInst::ProcMtMcuMsg( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	
#ifdef _DEBUG
	log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received from MT%u. Destination is %u\n", 
		pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byMtId,  
		cServMsg.GetDstMtId() );
#endif

	switch( CurState() )
	{
	case STATE_NORMAL:
	case STATE_WAITING:
		//323接口消息无会议号、消息号和源MT，此地补上！
#ifdef H323
		cServMsg.SetConfId( m_cConfId );
		cServMsg.SetSrcMt( LOCAL_MCUID, m_byMtId );
		cServMsg.SetEventId( pcMsg->event );
		if( pcMsg->event == MT_MCU_CAPBILITYSET_NOTIF )	//发能力集请求
		{
			//start roundTripDelay timer
			m_bGotRoundTripDelayRsp = FALSE;		//从未收到过应答
			SetTimer( TIMER_ROUNDTRIPDELAY_REQ, MAXDELAY_MT_ROUNDTRIPDELAY * 1000 );
		}
#endif
		//send it to conference instance
		SendMsgToConf( cServMsg.GetConfIdx(), pcMsg->event, 
			cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcuMtRequest
    功能        ：MCU发到MT的请求处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/04    2.0         LI Yi         创建
====================================================================*/
void CMtSsnInst::ProcMcuMtRequest( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u16			wTimer;

#ifdef _DEBUG
	log( LOGLVL_DEBUG1, "CMtSsnInst: Request message %u(%s) to MT%u received!\n", 
		pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byMtId );
#endif

	switch( CurState() )
	{
	case STATE_NORMAL:
#ifndef H323
		SendMsgToSubMt( pcMsg->event, pcMsg->content, pcMsg->length );
#else
		SendH323MsgToSubMt( pcMsg );
#endif
		
		//set timer
		wTimer = cServMsg.GetTimer() * TIMER_MTSSN_COEFF / 10;
		if( wTimer > 0 )	//need reply
		{
			//save message header
			cServMsg.GetServMsg( ( u8 * )m_abySavedMsgHdr, sizeof( m_abySavedMsgHdr ) );
			SetTimer( TIMER_WAIT_REPLY, wTimer * 1000 );
			log( LOGLVL_DEBUG1, "CMtSsnInst: Need to wait %u seconds for the response.\n", wTimer );
			//save event
			m_wSavedEvent = pcMsg->event;
			NEXTSTATE( STATE_WAITING );
		}
		break;
	case STATE_WAITING:
		cServMsg.SetErrorCode( ERR_MCU_WAITINGSTATE );
		cServMsg.SetMsgBody( NULL, 0 );
		log( LOGLVL_IMPORTANT, "CMtSsnInst: Current state is STATE_WAITING! Cannot process another command now!\n" );
		SendMsgToConf( cServMsg.GetConfIdx(), pcMsg->event + 2, 
			cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMtMcuResponse
    功能        ：MT返回应答处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/05    1.0         LI Yi         创建
    02/11/11    2.0         Liaoweijiang  修改
====================================================================*/
void CMtSsnInst::ProcMtMcuResponse( const CMessage * pcMsg )
{
	CServMsg	cServMsg;
#ifdef H323
	u16		wErrorCode;
#else
	u16		wSerialNo;
#endif

#ifdef _DEBUG
	log( LOGLVL_DEBUG1, "CMtSsnInst: Response message %u(%s) from MT%u received!\n", 
		pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byMtId );
#endif

	switch( CurState() )
	{
	case STATE_WAITING:
#ifdef H323		//H323接口需要重新安装保存的消息头
		cServMsg.SetServMsg( pcMsg->content, pcMsg->length );
		wErrorCode = cServMsg.GetErrorCode();
		cServMsg.SetServMsg( ( u8 * const )m_abySavedMsgHdr, sizeof( m_abySavedMsgHdr ) );
		cServMsg.SetErrorCode( wErrorCode );
		cServMsg.SetMsgBody( pcMsg->content + SERV_MSGHEAD_LEN, pcMsg->length - SERV_MSGHEAD_LEN );
#else			//自定义接口需要匹配流水号
		//compare the serial no
		cServMsg.SetServMsg( ( u8 * const )m_abySavedMsgHdr, sizeof( m_abySavedMsgHdr ) );
		wSerialNo = cServMsg.GetSerialNO();
		cServMsg.SetServMsg( pcMsg->content, pcMsg->length );
		if( wSerialNo != cServMsg.GetSerialNO() )	//not the correct reply
		{
			log( LOGLVL_EXCEPTION, "CMtSsnInst: Response serial no. do not match! Original no. is %u while this one's is %u. Discard it!\n", 
				wSerialNo, cServMsg.GetSerialNO() );
			return;
		}
#endif

		KillTimer( TIMER_WAIT_REPLY );
		//send it to conference instance
		SendMsgToConf( cServMsg.GetConfIdx(), pcMsg->event, 
			cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

		NEXTSTATE( STATE_NORMAL );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcuMtInviteMtReq
    功能        ：MCU发到MT的邀请入会处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/11    2.0         LI Yi         创建
	03/02/20    2.0         Qzj           修改自定义模式下的处理方式
====================================================================*/
void CMtSsnInst::ProcMcuMtInviteMtReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = *( TMt * )cServMsg.GetMsgBody();
	u8          byConfDVid = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMt));

	TTransportAddr tAddr;
	u8 byType;
    u32 dwMode;
    u32 dwBitRate;
    u8 byMode;
    u32 dwFrq;

	switch( CurState() )
	{

	//Special operation for Satellite MCU, ACK VC directorly!
	case STATE_CONNECTED:

    //FIXME: 稍后严格串行 状态机
    case STATE_NORMAL:
        
        //cServMsg.SetSrcMt( tMt.GetMcuId(), tMt.GetMtId() );
		
		//cheat vc, we only ACK directory
		post( MAKEIID( GetAppID(), GetInsID() ), pcMsg->event + 1, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() ); 

		tAddr.SetIpAddr(::topoGetMtInfo( LOCAL_MCUID, m_byMtId, g_atMtTopo, g_wMtTopoNum ).m_dwMtModemIp );
		tAddr.SetPort( ::topoGetMtInfo( LOCAL_MCUID, m_byMtId, g_atMtTopo, g_wMtTopoNum ).m_wMtModemPort );
		byType = ::topoGetMtInfo( LOCAL_MCUID, m_byMtId, g_atMtTopo, g_wMtTopoNum ).m_byMtModemType;
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
		cServMsg.CatMsgBody( (u8*)&tAddr, sizeof(TTransportAddr) );
		cServMsg.CatMsgBody( &byType, sizeof(u8) );
		
        g_cModemSsnApp.SendMsgToModemSsn( GetInsID() + MAXNUM_MCU_MODEM, MODEM_MCU_REG_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()  );
		//Rember the conference we currently in!
		m_cConfId = cServMsg.GetConfId();
        
        cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
        dwMode = DEM_CLKSRC_INTER;
        cServMsg.CatMsgBody( (u8*)&dwMode, sizeof(dwMode) );
        dwMode = MOD_CLKSRC_INTER;
        cServMsg.CatMsgBody( (u8*)&dwMode, sizeof(dwMode) );
        g_cModemSsnApp.SendMsgToModemSsn( GetInsID() + MAXNUM_MCU_MODEM, MCU_MODEM_SETSRC, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()  );
        
        dwBitRate = g_cMcuVcApp.GetConfSndBitRate( cServMsg.GetConfIdx() );
        cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
        cServMsg.CatMsgBody( (u8*)&dwBitRate, sizeof(u32) );
        byMode = 2;
        cServMsg.CatMsgBody( (u8*)&byMode, sizeof(u8) );
        g_cModemSsnApp.SendMsgToModemSsn( GetInsID() + MAXNUM_MCU_MODEM, MCU_MODEM_SETBITRATE, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()  );
        
		//双格式会议终端接收带宽同步加倍，以锁定Modem
		//统一在频率申请里 处理
// 		if (byConfDVid != 0)
// 		{
// 			dwBitRate = g_cMcuVcApp.GetConfRcvBitRate( cServMsg.GetConfIdx() ) * 2;
// 		}
// 		else
// 		{
			dwBitRate = g_cMcuVcApp.GetConfRcvBitRate( cServMsg.GetConfIdx() );
//		}
        
        cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
        cServMsg.CatMsgBody( (u8*)&dwBitRate, sizeof(u32) );
        byMode = 1;
        cServMsg.CatMsgBody( (u8*)&byMode, sizeof(u8) );
        g_cModemSsnApp.SendMsgToModemSsn( GetInsID() + MAXNUM_MCU_MODEM, MCU_MODEM_SETBITRATE, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
        
        
        dwFrq = g_cMcuVcApp.GetConfSndFreq( cServMsg.GetConfIdx() );
        cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
        cServMsg.CatMsgBody( (u8*)&dwFrq, sizeof(u32) );
        g_cModemSsnApp.SendMsgToModemSsn( GetInsID() + MAXNUM_MCU_MODEM, MCU_MODEM_RECEIVE, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()  );

        SatLog("[ProcMcuMtInviteMtReq] Inst.%d, MtId.%d chg state to calling!\n", GetInsID(), m_byMtId);
		
        NEXTSTATE( STATE_CALLING );
		break;

		//以下状态只有自定义接口可能出现
	case STATE_CALLING:
		cServMsg.SetErrorCode( ERR_MCU_WAITINGSTATE );
		log( LOGLVL_IMPORTANT, "CMtSsnInst: Current state is STATE_CALLING! Cannot process another command now!\n" );
		SendMsgToConf( cServMsg.GetConfIdx(), pcMsg->event + 2, 
			cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		break;

    //FIXME: S稍后严格串行状态机
	//case STATE_NORMAL:
	case STATE_WAITING:
		cServMsg.SetErrorCode( ERR_MCU_MTBUSY );
		log( LOGLVL_IMPORTANT, "CMtSsnInst: Current state is In Conference! Cannot process another command now!\n" );
		SendMsgToConf( cServMsg.GetConfIdx(), pcMsg->event + 2, 
			cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		break;

	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u, inst.%d!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcuMtOpenLogicChnReq
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/11/26    4.6         zhangbq         创建
====================================================================*/
void CMtSsnInst::ProcMcuMtOpenLogicChnReq(const CMessage * pcMsg)
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TLogicalChannel	tLogicChn = *( TLogicalChannel * )cServMsg.GetMsgBody();
	
	
	switch( CurState() )
	{
		
	//Special operation for Satellite MCU, ACK VC directorly!
	case STATE_CONNECTED:		
	//FIXME: 稍后严格串行 状态机
    case STATE_NORMAL:

		cServMsg.SetSrcMtId(cServMsg.GetDstMtId());

		TTransportAddr tRcvMediaChn;
		tRcvMediaChn.SetPort(MT_MCU_STARTPORT + PORTSPAN * (cServMsg.GetSrcMtId() - 1) + 4);
		tLogicChn.SetRcvMediaChannel(tRcvMediaChn);

		cServMsg.SetMsgBody((u8*)&tLogicChn, sizeof(tLogicChn));
		SendMsgToConf( cServMsg.GetConfIdx(),
					   pcMsg->event + 1, 
					   cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

		break;
	}

	return;
}

/*====================================================================
    函数名      ：ProcMcuMtCloseLogicChnCmd
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/11/26    4.6         zhangbq         创建
====================================================================*/
void CMtSsnInst::ProcMcuMtCloseLogicChnCmd(const CMessage * pcMsg)
{
	return;
}


/*====================================================================
    函数名      ：ProcMtMcuInviteMtRsp
    功能        ：MT返回的邀请应答处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/11    2.0         LI Yi         创建
	03/02/20    2.0         Qzj           修改自定义模式下的处理方式
====================================================================*/
void CMtSsnInst::ProcMtMcuInviteMtRsp( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
#ifdef H323
	u16		wErrorCode;
#endif

#ifdef _DEBUG
	log( LOGLVL_DEBUG1, "CMtSsnInst: INVITE Response message %u(%s) from MT%u received!\n", 
		pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byMtId );
#endif

	switch( CurState() )	//该应答不需要匹配流水号，因为回滚的问题
	{
	case STATE_CALLING:
//		KillTimer( TIMER_WAIT_CALL_RSP );

#ifdef H323		//H323接口需要重新安装保存的消息头
		wErrorCode = cServMsg.GetErrorCode();
		cServMsg.SetServMsg( ( u8 * const )m_abySavedMsgHdr, sizeof( m_abySavedMsgHdr ) );
		cServMsg.SetErrorCode( wErrorCode );
		cServMsg.SetMsgBody( pcMsg->content + SERV_MSGHEAD_LEN, pcMsg->length - SERV_MSGHEAD_LEN );
#endif

        SatLog("[ProcMtMcuInviteMtRsp] Mt.%d Inst.%d get msg.%d<%s>!\n",
                m_byMtId, GetInsID(), pcMsg->event, OspEventDesc(pcMsg->event));

		if( pcMsg->event == MT_MCU_INVITEMT_ACK )	//ack
		{
			//send it to conference instance
			SendMsgToConf( cServMsg.GetConfIdx(), pcMsg->event, 
				           cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			
			NEXTSTATE( STATE_NORMAL );
		}
		else //nack
		{
			//send nack to conference instance
			SendMsgToConf( cServMsg.GetConfIdx(), pcMsg->event, 
				           cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

#ifndef H323	//自定义接口不释放实例
			NEXTSTATE( STATE_CONNECTED );
#else		//323接口时需要释放该实例
			
            //clear
			m_byMtId = 0;	//自定义不能清零
			DeleteAlias();
			m_hsCall = NULL;

			NEXTSTATE( STATE_IDLE );
#endif
		}
			
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMtMcuApplyJoinRequest
    功能        ：MT发来的申请加入消息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/02/20    2.0         Qzj           创建
====================================================================*/
void CMtSsnInst::ProcMtMcuApplyJoinRequest( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u16 wTimer;
	TMt	tMt;
#ifdef H323
	MYCALL	*ptCall;
	u8	byMtId;
#endif

	tMt = *( TMt * )cServMsg.GetMsgBody();

#ifdef _DEBUG
	log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received from MT%u. Destination is %u.\n", 
		pcMsg->event, ::OspEventDesc( pcMsg->event ), tMt.GetMtId(), 
		cServMsg.GetDstMtId() );
#endif

	switch( CurState() )
	{
#ifndef H323	//目前只考虑自定义接口
	case STATE_CONNECTED:
#else			//H323接口
	case STATE_IDLE:
		ptCall = g_cH323AdpApp.FindCall( tMt.GetMtId() );
		if( ptCall == NULL )
		{
			log( LOGLVL_EXCEPTION, "CMtSsnInst: Cannot find call handle for called MT%u-%u, Nack!\n", 
				tMt.GetMcuId(), tMt.GetMtId() );
			return;
		}

		//如果该终端已经在另一个会议中，Nack
		byMtId = tMt.GetMtId();
		if( SetAlias( ( const char * )&byMtId, sizeof( u8 ) ) == OSPERR_ALIAS_REPEAT )
		{
#ifdef _DEBUG
			log( LOGLVL_DEBUG1, "CMtSsnInst: Called MT%u-%u in another conference now. Nack!\n", 
				tMt.GetMcuId(), tMt.GetMtId() );
#endif		

			//drop it
			::kdvcmCallDrop( ptCall->hsCall, TRUE );
			return;
		}

		//save information
		m_hsCall = ptCall->hsCall;
		m_byMtId = ptCall->byMtId;
#endif

		//send it to conference instance
		SendMsgToConf( cServMsg.GetConfIdx(), pcMsg->event, 
			pcMsg->content, pcMsg->length );

		//set timer
		wTimer = cServMsg.GetTimer() * TIMER_MTSSN_COEFF / 10;
		if( wTimer == 0 )
			wTimer = 20;
		
		//save message header
		cServMsg.GetServMsg( ( u8 * )m_abySavedMsgHdr, sizeof( m_abySavedMsgHdr ) );
		
		SetTimer( TIMER_WAIT_CALL_RSP, wTimer * 1000 );
#ifdef _DEBUG
		log( LOGLVL_DEBUG1, "CMtSsnInst: Need to wait %u seconds for the JOIN application response.\n", wTimer );
#endif
		//save event
		m_wSavedEvent = pcMsg->event;

		m_cConfId = cServMsg.GetConfId();	//save conference id

		NEXTSTATE( STATE_CALLING );
		break;
#ifndef H323		//以下状态只有自定义接口可能出现
	case STATE_CALLING:
		cServMsg.SetErrorCode( ERR_MCU_WAITINGSTATE );
		log( LOGLVL_IMPORTANT, "CMtSsnInst: Current state is STATE_CALLING! Cannot process another join request now!\n" );
		SendMsgToSubMt( pcMsg->event + 2, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		break;
	case STATE_NORMAL:
	case STATE_WAITING:
		cServMsg.SetErrorCode( ERR_MCU_MTBUSY );
		log( LOGLVL_IMPORTANT, "CMtSsnInst: Current state is In Conference! Cannot process another command now!\n" );
		SendMsgToSubMt( pcMsg->event + 2, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		break;
#endif
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcuMtApplyJoinRsp
    功能        ：MCU对MT申请加入消息的反应
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/02/20    2.0         Qzj           创建
====================================================================*/
void CMtSsnInst::ProcMcuMtApplyJoinRsp( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

#ifdef _DEBUG
	
	log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received to MT%u. Destination is %u.\n", 
		pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byMtId, 
		cServMsg.GetDstMtId() );
#endif

	switch( CurState() )
	{
	case STATE_CALLING :

		//需要进行状态切换
		if( pcMsg->event == MCU_MT_APPLYJOINCONF_ACK )
		{//同意终端加入会议
#ifndef H323
			KillTimer( TIMER_WAIT_CALL_RSP );

			SendMsgToSubMt( pcMsg->event, pcMsg->content, pcMsg->length );

			NEXTSTATE( STATE_NORMAL );
#else		//323接口先不切换状态，也不关闭定时器。
			//等到呼叫完全成功后收到MT_MCU_INVITEMT_ACK时再使用
			::kdvcmCallAnswer( m_hsCall, TRUE );

			//收到MT_MCU_INVITEMT_ACK时使用
			cServMsg.SetNoSrc();
			cServMsg.SetEventId( 0 );
			memcpy( &m_abySavedMsgHdr, cServMsg.GetServMsg(), sizeof( m_abySavedMsgHdr ) );
#endif
		}
		else
		{//反对终端加入会议，或已经将申请发往相应会议主席
#ifndef H323
			SendMsgToSubMt( pcMsg->event, pcMsg->content, pcMsg->length );

			NEXTSTATE( STATE_CONNECTED );
#else
			::kdvcmCallDrop( m_hsCall, TRUE );
			
			//clear
			m_byMtId = 0;	//自定义不能清零
			m_hsCall = NULL;
			
			DeleteAlias();
			NEXTSTATE( STATE_IDLE );
#endif
		}
		
		break;
	default :
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}
/*====================================================================
    函数名      ：ProcMtMcuCreateConfRequest
    功能        ：终端创建会议要求消息处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/02/20    1.0         Qzj		      创建
====================================================================*/
void CMtSsnInst::ProcMtMcuCreateConfRequest( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	
#ifdef _DEBUG
	log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received from MT%u. Destination is %u.\n", 
		pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byMtId, 
		cServMsg.GetDstMtId() );
#endif

#ifndef H323//只考虑自定义情况
	switch( CurState() )
	{
	case STATE_CONNECTED:
		//send it to conference instance
		SendMsgToConf( cServMsg.GetConfIdx(), pcMsg->event, 
			pcMsg->content, pcMsg->length );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
#endif
}

/*====================================================================
    函数名      ：ProcMcuMtCreateConfRsp
    功能        ：mcu对终端创建会议要求的应答
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/15    1.0         LI Yi         创建
====================================================================*/
void CMtSsnInst::ProcMcuMtCreateConfRsp( const CMessage * pcMsg )
{

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
#ifdef _DEBUG
	log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received to MT%u. Destination is %u.\n", 
		pcMsg->event, ::OspEventDesc( pcMsg->event ), m_byMtId, 
		cServMsg.GetDstMtId() );
#endif

#ifndef H323//只考虑自定义情况
	switch( CurState() )
	{
	case STATE_CONNECTED :
		SendMsgToSubMt( pcMsg->event, pcMsg->content, pcMsg->length );
		if( pcMsg->event == MCU_MT_CREATECONF_ACK )//创建成功，终端加入会议
		{
			m_cConfId = cServMsg.GetConfId();
			NEXTSTATE( STATE_NORMAL );
		}
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
#endif
}

/*====================================================================
    函数名      ：ProcTimerWaitCallRsp
    功能        ：等待呼叫应答超时处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/15    1.0         LI Yi         创建
====================================================================*/
void CMtSsnInst::ProcTimerWaitCallRsp( const CMessage * pcMsg )
{
	CServMsg	cServMsg;
	TMt		tMt;

#ifdef _DEBUG
	log( LOGLVL_DEBUG1, "CMtSsnInst: Message TIMER_WAIT_CALL_RSP received!\n" );
#endif

	switch( CurState() )
	{
	case STATE_CALLING:
		cServMsg.SetServMsg( ( u8 * const )m_abySavedMsgHdr, sizeof( m_abySavedMsgHdr ) );
		cServMsg.SetErrorCode( ERR_MCU_TIMEROUT );
		//cServMsg.SetNoDst();
		tMt.SetMt( LOCAL_MCUID, m_byMtId );
        
        //FIXME: 这个别名如何处理
		//tMt.SetAlias( ::topoGetMtInfo( tMt.GetMcuId(), tMt.GetMtId(), g_atMtTopo, g_wMtTopoNum ).GetAlias() );
		cServMsg.SetMsgBody( ( u8 * )&tMt, sizeof( tMt ) );

		//send nack to self, do not need to change state soon
		post( MAKEIID( GetAppID(), GetInsID() ), m_wSavedEvent + 2, 
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMtMcuReadyNotif
    功能        ：终端通知MCU它自己准备得到加入会议的邀请，该消息只有自
				  定义接口才有响应
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/26    1.0         Liaoweijiang  创建
====================================================================*/
void CMtSsnInst::ProcMtMcuReadyNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tRegMt = *( TMt* )cServMsg.GetMsgBody();
	
#ifdef _DEBUG
	log( LOGLVL_DEBUG1, "CMtSsnInst: Message %u(%s) received!\n", pcMsg->event, 
		::OspEventDesc( pcMsg->event ) );
#endif

	switch( CurState() )
	{
#ifndef H323
	case STATE_CONNECTED:
		//send notification to all conference instances
		cServMsg.SetMsgBody( ( u8 * )&tRegMt, sizeof( TMt ) );
		SendMsgToDaemonConf( MCU_MTCONNECTED_NOTIF, cServMsg.GetServMsg(), 
			cServMsg.GetServMsgLen() );
		break;
#endif
	case STATE_IDLE:
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcTimerWaitReply
    功能        ：等待命令应答超时处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
====================================================================*/
void CMtSsnInst::ProcTimerWaitReply( const CMessage * pcMsg )
{
	CServMsg	cServMsg;
	TMt		tMt;

#ifdef _DEBUG
	log( LOGLVL_DEBUG1, "CMtSsnInst: Message TIMER_WAIT_REPLY received!\n" );
#endif

	switch( CurState() )
	{
	case STATE_WAITING:
		cServMsg.SetServMsg( ( u8 * const )m_abySavedMsgHdr, sizeof( m_abySavedMsgHdr ) );
		cServMsg.SetErrorCode( ERR_MCU_TIMEROUT );
		//cServMsg.SetNoDst();
		//set message body if necessary
		switch( m_wSavedEvent )
		{
		case MCU_MT_INVITEMT_REQ:
		case MCU_MT_GETMTSTATUS_REQ:
		//case MCU_MT_MTRCENABLE_REQ:
		//case MCU_MT_MTFARSITEMUTE_REQ:
		//case MCU_MT_MTNEARSITEMUTE_REQ:
		//case MCU_MT_APPLYCASE_REQ:
			tMt.SetMt( LOCAL_MCUID, m_byMtId );
			cServMsg.SetMsgBody( ( u8 * )&tMt, sizeof( tMt ) );
			break;
		default:
			cServMsg.SetMsgBody( NULL, 0 );
			break;
		}
		//send nack to self, do not need to change state soon
		post( MAKEIID( GetAppID(), GetInsID() ), m_wSavedEvent + 2, 
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		break;
	default:
		log( LOGLVL_EXCEPTION, "[ProcTimerWaitReply] CMtSsnInst: Wrong message %u(%s) received in state %u, inst.%d!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState(), GetInsID() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcuMtDropMtCmd
    功能        ：删除终端处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/05/23    1.0         LI Yi         创建
====================================================================*/
void CMtSsnInst::ProcMcuMtDropMtCmd( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	
	switch( CurState() )
	{
	case STATE_NORMAL:
	case STATE_CALLING:
		//在MT加入会议的整个过程中都有可能收到剔除MT的命令
		//所有非IDLE终端都会收到该消息，需要根据会议号比较
		if( m_cConfId == cServMsg.GetConfId() )
		{
			//自定义接口只需进入CONNECTED状态，不释放本实例
			NEXTSTATE( STATE_CONNECTED );

			//FIXME: TOPO稍后处理，先释放modem
			/*
			if ( ::topoGetMtInfo( LOCAL_MCUID, m_byMtId, g_atMtTopo, g_wMtTopoNum ).m_byMtConnected == 1 )
				return;

			*/

			if ( ::topoGetMtInfo( LOCAL_MCUID, m_byMtId, g_atMtTopo, g_wMtTopoNum ).m_dwMtModemIp == 0 )
				return;
			g_cModemSsnApp.SendMsgToModemSsn( GetInsID() + 17, MCU_MODEM_RELEASE, cServMsg.GetServMsg(), cServMsg.GetServMsgLen()  );
			
		}
		break;
	case STATE_CONNECTED:	//do nothing
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcTimerRoundTripDelayReq
    功能        ：定时发送roundTripDelayReq处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/02/26    1.0         LI Yi         创建
====================================================================*/

void CMtSsnInst::ProcTimerRoundTripDelayReq( const CMessage * pcMsg )
{
	switch( CurState() )
	{
#ifndef H323    //非323版本，在整个连接过程中都有RoundTripDelay
	case STATE_CALLING:
	case STATE_CONNECTED:
#endif
	case STATE_WAITING:
	case STATE_NORMAL:
		//start roundTripDelay
		m_byTimeoutTimes = 0;

#ifdef H323		
		::kdvcmCallRoundTripDelay( m_hsCall, MAXDELAY_MT_ROUNDTRIPDELAY / 2, TRUE );
#else
		SendMsgToSubMt( MCU_MT_ROUNDTRIPDELAY_REQ );
#endif
		
		SetTimer( TIMER_WAIT_ROUNDTRIPDELAY_RSP, MAXDELAY_MT_ROUNDTRIPDELAY * 1000 / 2 );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(TIMER_ROUNDTRIPDELAY_REQ) received in state %u!\n", 
			pcMsg->event, CurState() );
		break;
	}
}


/*====================================================================
    函数名      ：ProcTimerWaitRoundTripDelayRsp
    功能        ：等待roundTripDelayRsp超时处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/02/26    1.0         LI Yi         创建
====================================================================*/

void CMtSsnInst::ProcTimerWaitRoundTripDelayRsp( const CMessage * pcMsg )
{
	switch( CurState() )
	{
#ifndef H323    //非323版本，在整个连接过程中都有RoundTripDelay
	case STATE_CALLING:
	case STATE_CONNECTED:
#endif
	case STATE_WAITING:
	case STATE_NORMAL:
		KillTimer( TIMER_WAIT_ROUNDTRIPDELAY_RSP );	//该消息可能由协议适配模块发出，所以先关闭定时器

		//start roundTripDelay
		m_byTimeoutTimes++;
		log( LOGLVL_IMPORTANT, "CMtSsnInst: RoundTripDelay to MT%u timed out! Time=%u.\n", 
			m_byMtId, m_byTimeoutTimes );
		if( m_byTimeoutTimes < 3 )	//try again
		{
#ifdef H323			
			::kdvcmCallRoundTripDelay( m_hsCall, MAXDELAY_MT_ROUNDTRIPDELAY / 2, TRUE );
#else
			SendMsgToSubMt( MCU_MT_ROUNDTRIPDELAY_REQ );
#endif
			SetTimer( TIMER_WAIT_ROUNDTRIPDELAY_RSP, MAXDELAY_MT_ROUNDTRIPDELAY * 1000 / 2 );
		}
		else if( m_byTimeoutTimes >= 3 && m_bGotRoundTripDelayRsp == TRUE )	
				//disconnect if occured 3 times and ever got response from the terminal
		{
#ifdef H323
			::kdvcmCallDrop( m_hsCall, TRUE );
#endif			
			post( MAKEIID( GetAppID(), GetInsID() ), OSP_DISCONNECT );
		}
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(TIMER_ROUNDTRIPDELAY_REQ) received in state %u!\n", 
			pcMsg->event, CurState() );
		break;
	}
}


/*====================================================================
    函数名      ：ProcMtMcuRoundTripDelayRsp
    功能        ：roundTripDelayRsp处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/02/26    1.0         LI Yi         创建
====================================================================*/
void CMtSsnInst::ProcMtMcuRoundTripDelayRsp( const CMessage * pcMsg )
{
	switch( CurState() )
	{
#ifndef H323    //非323版本，在整个连接过程中都有RoundTripDelay
	case STATE_CALLING:
	case STATE_CONNECTED:
#endif
	case STATE_WAITING:
	case STATE_NORMAL:
		KillTimer( TIMER_WAIT_ROUNDTRIPDELAY_RSP );
		m_bGotRoundTripDelayRsp = TRUE;
		//start roundTripDelay timer
		SetTimer( TIMER_ROUNDTRIPDELAY_REQ, MAXDELAY_MT_ROUNDTRIPDELAY * 1000 );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMtSsnInst: Wrong message %u(TIMER_ROUNDTRIPDELAY_REQ) received in state %u!\n", 
			pcMsg->event, CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：SendH323MsgToSubMt
    功能        ：等待命令应答超时处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/29    1.0         LI Yi         创建
====================================================================*/
#ifdef H323
void CMtSsnInst::SendH323MsgToSubMt( const CMessage * pcMsg )
{

}
#endif

/*====================================================================
    函数名      ：ProcAppTaskRequest
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

void CMtSsnInst::DaemonProcAppTaskRequest( const CMessage * pcMsg )
{
	post( pcMsg->srcid, MCU_APPTASKTEST_ACK, pcMsg->content, pcMsg->length );
}

//////////////For Transport Message////////////////////////////////
SOCKET CMtSsnInst::CreateUdpSocket( u32 dwIpAddr, u16 wPort )
{
	//创建发送数据SOCKET
	SOCKADDR_IN tUdpAddr;
	SOCKET		tSocket;		
	
	memset( &tUdpAddr, 0, sizeof( tUdpAddr ) );
    tUdpAddr.sin_family = AF_INET; 
    tUdpAddr.sin_port = wPort;
	tUdpAddr.sin_addr.s_addr = dwIpAddr;
	
	//Allocate a socket
    tSocket = socket ( AF_INET, SOCK_DGRAM , 0 );
	
	if( tSocket == INVALID_SOCKET )
	{
		log( LOGLVL_EXCEPTION ,"CMtSsnInst: Create UDP Socket Error.\n" );
		return tSocket;
	}
	
	if( bind( tSocket, (SOCKADDR *)&tUdpAddr, sizeof( tUdpAddr ) ) == SOCKET_ERROR )
	{
		SockClose( m_hSocket );//bind failed!
		tSocket = INVALID_SOCKET;

		log( LOGLVL_EXCEPTION, "CMtSsnInst: Bind socket  Error.\n" );
		return tSocket;
	}
	return tSocket;

}

BOOL CMtSsnInst::JoinSocketMulticast( SOCKET sock ,u32 dwMulticastIP ,u32 dwIfIP )
{
	struct ip_mreq mreq;

	memset(&mreq, 0, sizeof(mreq));	
	mreq.imr_multiaddr.s_addr = dwMulticastIP;
	mreq.imr_interface.s_addr = dwIfIP;

	if( sock == INVALID_SOCKET )
		return FALSE;
	
	if( setsockopt( sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq) ) != 0 )
	{
		return FALSE;
	}

	char ttl = 10;
	if( setsockopt( sock, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl)) !=0 )
	{
		return FALSE;
	}
	
	in_addr  addr;
	addr.s_addr = dwIfIP ;	
	if( setsockopt( sock, IPPROTO_IP, IP_MULTICAST_IF, (char *)&addr, sizeof(addr)) != 0 )
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMtSsnInst::DropSocketMulticast( SOCKET sock ,u32 dwMulticastIP, u32 dwIfIP )
{
		struct ip_mreq mreq;
		memset( &mreq, 0, sizeof(mreq) );	
		mreq.imr_multiaddr.s_addr = dwMulticastIP;
		mreq.imr_interface.s_addr = dwIfIP;
		
		if( sock == INVALID_SOCKET )
			return FALSE;
		
		if(setsockopt( sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) != 0)
		{
			return FALSE;
		}
		return TRUE;	
}


void CMtSsnInst::PostMessage2Conf( u16 wConfIdx, const CServMsg & cServMsg )
{
	CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(wConfIdx);
	if (NULL == pcVcInst)
	{
		return;
	}
	u8 byInsId = pcVcInst->GetInsID();
	post( MAKEIID( AID_MCU_VC, byInsId ), cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
}

BOOL CMtSsnInst::FilterMessage( u32 eventid )
{
	time_t timenew;
	time(&timenew);
	if ( eventid == eventold )
	{
		if ( ( timenew - timeold ) > 2 )
		{
			timeold = timenew;
			return TRUE;
		}
		else
		{
			timeold = timenew;
			return FALSE;
		}
	}
	else
	{
		eventold = eventid;
		timeold = timenew;
		return TRUE;
	}
	
}


void CMtSsnInst::SendMsgToConf(u8 byConfIdx, u16 wEvent, u8 * const pbyMsg, u16 wLen)
{
    CMcuVcInst *pInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
    if (NULL == pInst)
    {
        OspPrintf(TRUE, FALSE, "[CMtSsnInst::SendMsgToConf] confIdx.%d unexist!\n", byConfIdx);
        return;
    }

    u8 byInstId = pInst->GetInsID();

    u16 wRet = OspPost(MAKEIID(AID_MCU_VC, byInstId), wEvent, pbyMsg, wLen);
    if (OSP_OK != wRet)
    {
        OspPrintf(TRUE, FALSE, "[CMtSsnInst::SendMsgToConf] wEvent.%d<%s> send to ConfIdx.%d failed!\n",
            wEvent, OspEventDesc(wEvent), byConfIdx);
    }

    return;
}

void CMtSsnInst::SendMsgToDaemonConf(u16 wEvent, u8 * const pbyMsg /* = NULL */, u16 wLen /* = 0  */)
{
    
    u16 wRet = OspPost(MAKEIID(AID_MCU_VC, CInstance::DAEMON), wEvent, pbyMsg, wLen);
    if (OSP_OK != wRet)
    {
        OspPrintf(TRUE, FALSE, "[CMtSsnInst::SendMsgToConf] wEvent.%d<%s> send to daemon conf failed!\n",
            wEvent, OspEventDesc(wEvent));
    }
    
    return;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMtConfig::CMtConfig()
{

}

CMtConfig::~CMtConfig()
{

}



